/* 
 * Mail Notification
 * Copyright (C) 2003-2006 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"
#include <eel/eel.h>
#include "mn-bonobo-unknown.h"
#include "mn-evolution.h"

/*** variables ***************************************************************/

static MNBonoboUnknown *glue = NULL;

G_LOCK_DEFINE_STATIC(glue);

/*** implementation **********************************************************/

MNBonoboUnknown *
mn_evolution_glue_client_use (void)
{
  G_LOCK(glue);

  if (glue)
    g_object_ref(glue);
  else
    {
      glue = mn_bonobo_unknown_new(MN_EVOLUTION_GLUE_IID);
      eel_add_weak_pointer(&glue);
    }

  G_UNLOCK(glue);

  return glue;
}

void
mn_evolution_glue_client_unuse (void)
{
  G_LOCK(glue);

  g_object_unref(glue);

  G_UNLOCK(glue);
}
