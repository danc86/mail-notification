/*
 * Mail Notification
 * Copyright (C) 2003-2007 Jean-Yves Lefort <jylefort@brutele.be>
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

#ifndef _MN_EVOLUTION_GLUE_CLIENT_H
#define _MN_EVOLUTION_GLUE_CLIENT_H

#include "mn-bonobo-unknown.h"

MNBonoboUnknown	*mn_evolution_glue_client_use	(void);
void		mn_evolution_glue_client_unuse	(void);

#endif /* _MN_EVOLUTION_GLUE_CLIENT_H */
