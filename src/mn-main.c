/* 
 * Copyright (c) 2003, 2004 Jean-Yves Lefort <jylefort@brutele.be>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"
#include <stdlib.h>
#include <gnome.h>
#include <libgnomevfs/gnome-vfs.h>
#include "mn-conf.h"
#include "mn-mailboxes.h"
#include "mn-ui.h"
#include "mn-util.h"
#include "mn-stock.h"

/*** variables ***************************************************************/

static gboolean arg_enable_info = FALSE;

/*** functions ***************************************************************/

static void	mn_main_list_handlers	(void);
static void	mn_main_info_log_cb	(const char	*log_domain,
					 GLogLevelFlags	log_level,
					 const char	*message,
					 gpointer	user_data);

/*** implementation **********************************************************/

static void
mn_main_list_handlers (void)
{
  const GType *types;
  int i;

  g_print(_("Compiled in handlers:\n"));

  types = mn_mailbox_get_types();
  for (i = 0; types[i]; i++)
    {
      MNMailboxClass *class;
      
      class = g_type_class_ref(types[i]);
      g_print("  %s\n", class->format);
      g_type_class_unref(class);
    }
}

static void
mn_main_info_log_cb (const char *log_domain,
		     GLogLevelFlags log_level,
		     const char *message,
		     gpointer user_data)
{
  if (arg_enable_info)
    g_log_default_handler(log_domain, log_level, message, user_data);
}

int
main (int argc, char **argv)
{
  gboolean arg_list_handlers = FALSE;
  const struct poptOption popt_options[] = {
    {
      "enable-info",
      0,
      POPT_ARG_NONE,
      &arg_enable_info,
      0,
      N_("Enable informational output"),
      NULL
    },
    {
      "list-handlers",
      0,
      POPT_ARG_NONE,
      &arg_list_handlers,
      0,
      N_("List compiled-in handlers and exit"),
      NULL
    },
    POPT_TABLEEND
  };
      
  g_log_set_fatal_mask(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL);
  g_log_set_handler(G_LOG_DOMAIN, G_LOG_LEVEL_INFO, mn_main_info_log_cb, NULL);

#ifdef ENABLE_NLS
  bindtextdomain(GETTEXT_PACKAGE, GNOMELOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);
#endif

  gnome_program_init(PACKAGE,
		     VERSION,
		     LIBGNOMEUI_MODULE,
		     argc,
		     argv,
		     GNOME_PARAM_HUMAN_READABLE_NAME, _("Mail Notification"),
		     GNOME_PROGRAM_STANDARD_PROPERTIES,
		     GNOME_PARAM_POPT_TABLE, popt_options,
		     NULL);

  if (arg_list_handlers)
    {
      mn_main_list_handlers();
      exit(0);
    }

  if (! gnome_vfs_init())
    g_critical(_("unable to initialize GnomeVFS"));

  mn_conf_init();
  mn_stock_init();
  mn_ui_init();
  
  mn_mailboxes_register();
  mn_mailboxes_install_timeout();
  gtk_main();

  return 0;
}
