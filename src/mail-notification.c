/* 
 * Copyright (c) 2003 Jean-Yves Lefort <jylefort@brutele.be>
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
#include <gnome.h>
#include "mn-conf.h"
#include "mn-check.h"
#include "mn-dialog.h"
#include "mn-mailboxes.h"
#include "mn-settings.h"
#include "mn-ui.h"
#include "mn-util.h"

/*** functions ***************************************************************/

static void	mail_notification_list_handlers    (void);

/*** implementation **********************************************************/

static void
mail_notification_list_handlers (void)
{
  g_print(_("Compiled in handlers:\n"));
#ifdef WITH_MBOX
  g_print("  mbox\n");
#endif
#ifdef WITH_MH
  g_print("  MH\n");
#endif
#ifdef WITH_MAILDIR
  g_print("  Maildir\n");
#endif
#ifdef WITH_POP3
  g_print("  POP3\n");
#endif
#ifdef WITH_MH
  g_print("  Sylpheed\n");
#endif
}

int
main (int argc, char **argv)
{
  gboolean arg_enable_debug = FALSE;
  gboolean arg_list_handlers = FALSE;
  const struct poptOption popt_options[] = {
    {
      "enable-debug",
      0,
      POPT_ARG_NONE,
      &arg_enable_debug,
      0,
      N_("Enable debugging output"),
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
      
#ifdef ENABLE_NLS
  bindtextdomain(GETTEXT_PACKAGE, GNOMELOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);
#endif

  g_thread_init(NULL);
  if (g_thread_supported())
    gdk_threads_init();
  else
    mn_fatal(_("the GLib thread system is unavailable"));
  
  gnome_program_init(PACKAGE,
		     VERSION,
		     LIBGNOMEUI_MODULE,
		     argc,
		     argv,
		     GNOME_PARAM_HUMAN_READABLE_NAME, _("Mail Notification"),
		     GNOME_PROGRAM_STANDARD_PROPERTIES,
		     GNOME_PARAM_POPT_TABLE, popt_options,
		     NULL);

  mn_settings.debug = arg_enable_debug;
  if (arg_list_handlers)
    {
      mail_notification_list_handlers();
      exit(0);
    }

  mn_conf_init();
  mn_ui_init();
  
  mn_mailboxes_register_all();

  if (mn_conf_get_bool("/apps/mail-notification/local/enabled"))
    mn_check(0);
  if (mn_conf_get_bool("/apps/mail-notification/remote/enabled"))
    mn_check(MN_CHECK_REMOTE);

  mn_check_install();

  GDK_THREADS_ENTER();
  gtk_main();
  GDK_THREADS_LEAVE();

  mn_conf_deinit();

  return 0;
}
