/* 
 * Copyright (C) 2005 Jean-Yves Lefort <jylefort@brutele.be>
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
#include <eel/eel.h>
#include "egg-editable-toolbar.h"
#include "egg-toolbar-editor.h"
#include "mn-util.h"
#include "mn-stock.h"
#include "mn-shell.h"
#include "mn-conf.h"
#include "mn-properties-dialog.h"
#include "mn-summary-popup.h"
#include "mn-message-view.h"

/*** cpp *********************************************************************/

#define STATUS_PUSH(cid, str) \
  gtk_statusbar_push(GTK_STATUSBAR(self.statusbar), (cid), (str))
#define STATUS_POP(cid) \
  gtk_statusbar_pop(GTK_STATUSBAR(self.statusbar), (cid))
#define GET_ACTION(name) \
  gtk_action_group_get_action(self.action_group, (name))
#define FULLSCREEN \
  (self.leave_fullscreen_popup != NULL)

/*** types *******************************************************************/

typedef struct
{
  GtkWidget		*window;

  GtkWidget		*menubar;
  GtkWidget		*toolbar;
  GtkWidget		*statusbar;

  GtkWidget		*scrolled;
  GtkWidget		*message_view;

  GtkWidget		*edit_toolbars_dialog;
  GtkWidget		*edit_toolbars_editor;

  GtkUIManager		*ui_manager;
  GtkActionGroup	*action_group;

  EggToolbarsModel	*toolbars_model;
  char			*toolbars_file;
  unsigned int		toolbars_model_save_idle_id;

  GSList		*transients;
  unsigned int		help_cid;
  unsigned int		count_cid;

  GtkWidget		*leave_fullscreen_popup;
  gboolean		leave_fullscreen_popup_visible;
  unsigned int		leave_fullscreen_timeout_id;
} MainWindow;

/*** variables ***************************************************************/

static MainWindow self = { NULL };

/*** actions *****************************************************************/

static void mn_main_window_mail_reader_activate_h (void);
static void mn_main_window_update_activate_h (void);
static void mn_main_window_close_activate_h (void);
static void mn_main_window_quit_activate_h (void);
static void mn_main_window_edit_toolbars_activate_h (void);
static void mn_main_window_preferences_activate_h (void);
static void mn_main_window_help_contents_activate_h (void);
static void mn_main_window_about_activate_h (void);

static GtkActionEntry menu_entries[] = {
  /* Toplevel */
  { "Mail", NULL, N_("_Mail") },
  { "Edit", NULL, N_("_Edit") },
  { "View", NULL, N_("_View") },
  { "Help", NULL, N_("_Help") },

  /* Mail menu */
  {
    "MailMailReader",
    MN_STOCK_MAIL_READER,
    N_("_Mail Reader"),
    "<control>M",
    N_("Launch the mail reader"),
    mn_main_window_mail_reader_activate_h
  },
  {
    "MailUpdate",
    GTK_STOCK_REFRESH,
    N_("_Update"),
    "<control>R",		/* Reload */
    N_("Update the mail status"),
    mn_main_window_update_activate_h
  },
  {
    "MailClose",
    GTK_STOCK_CLOSE,
    N_("_Close"),
    "<control>W",
    N_("Close this window"),
    mn_main_window_close_activate_h
  },
  {
    "MailQuit",
    GTK_STOCK_QUIT,
    N_("_Quit"),
    "<control>Q",
    N_("Quit Mail Notification"),
    mn_main_window_quit_activate_h
  },

  /* Edit menu */
  {
    "EditToolbars",
    NULL,
    N_("_Toolbars"),
    NULL,
    N_("Customize the toolbars"),
    mn_main_window_edit_toolbars_activate_h
  },
  {
    "EditPreferences",
    GTK_STOCK_PREFERENCES,
    N_("_Preferences"),
    NULL,
    N_("Configure Mail Notification"),
    mn_main_window_preferences_activate_h
  },

  /* View menu */
  {
    "ViewToolbarsStyle",
    NULL,
    N_("Toolbars Styl_e"),
    NULL,
    N_("Customize the toolbars style"),
    NULL
  },

  /* Help menu */
  {
    "HelpContents",
    GTK_STOCK_HELP,
    N_("_Contents"),
    "F1",
    N_("Display help"),
    mn_main_window_help_contents_activate_h,
  },
  {
    "HelpAbout",
    GNOME_STOCK_ABOUT,
    N_("_About"),
    NULL,
    N_("Display credits"),
    mn_main_window_about_activate_h
  }
};

static GtkToggleActionEntry toggle_entries[] = {
  /* View menu */
  {
    "ViewToolbars",
    NULL,
    N_("_Toolbars"),
    NULL,
    N_("Show or hide the toolbars"),
    NULL,
    FALSE
  },
  {
    "ViewStatusbar",
    NULL,
    N_("_Statusbar"),
    NULL,
    N_("Show or hide the statusbar"),
    NULL,
    FALSE
  }
};

static GtkRadioActionEntry toolbars_style_entries[] = {
  /* View menu */
  {
    "ViewToolbarsStyleDesktopDefault",
    NULL,
    N_("_Desktop Default"),
    NULL,
    N_("Set the toolbars style to the desktop default setting"),
    MN_TOOLBAR_STYLE_DESKTOP_DEFAULT
  },
  {
    "ViewToolbarsStyleIconsOnly",
    NULL,
    N_("I_cons Only"),
    NULL,
    N_("Only display the toolbars icons"),
    MN_TOOLBAR_STYLE_ICONS
  },
  {
    "ViewToolbarsStyleTextOnly",
    NULL,
    N_("_Text Only"),
    NULL,
    N_("Only display the toolbars text"),
    MN_TOOLBAR_STYLE_TEXT
  },
  {
    "ViewToolbarsStyleTextBelowIcons",
    NULL,
    N_("Text Belo_w Icons"),
    NULL,
    N_("Display the toolbars text below the icons"),
    MN_TOOLBAR_STYLE_BOTH
  },
  {
    "ViewToolbarsStyleTextBesideIcons",
    NULL,
    N_("Text Be_side Icons"),
    NULL,
    N_("Display the toolbars text beside the icons"),
    MN_TOOLBAR_STYLE_BOTH_HORIZ
  }
};

/*** functions ***************************************************************/

static void mn_main_window_transient_weak_notify_cb (gpointer data,
						     GObject *former_object);

static void mn_main_window_weak_notify_cb (gpointer data,
					   GObject *former_object);

static void mn_main_window_notify_mail_reader_cb (GConfClient *client,
						  unsigned int cnxn_id,
						  GConfEntry *entry,
						  gpointer user_data);

static void mn_main_window_update_messages (void);
static void mn_main_window_update_sensitivity (void);

static void mn_main_window_create_toolbars_model (gboolean use_dot_file);

static void mn_main_window_ui_manager_connect_proxy_h (GtkUIManager *manager,
						       GtkAction *action,
						       GtkWidget *proxy,
						       gpointer user_data);
static void mn_main_window_ui_manager_disconnect_proxy_h (GtkUIManager *manager,
							  GtkAction *action,
							  GtkWidget *proxy,
							  gpointer user_data);

static void mn_main_window_menu_item_select_h (GtkMenuItem *item,
					       gpointer user_data);
static void mn_main_window_menu_item_deselect_h (GtkMenuItem *item,
						 gpointer user_data);

static void mn_main_window_link_view_action (GtkToggleAction *action, GCallback cb);

static void mn_main_window_update_toolbars_visibility (void);
static void mn_main_window_update_statusbar_visibility (void);
static void mn_main_window_update_toolbars_style (void);

static void mn_main_window_toolbars_model_save_changes (void);
static gboolean mn_main_window_toolbars_model_save_changes_cb (gpointer data);

static void mn_main_window_toolbars_model_update_flags_and_save_changes (void);

static void mn_main_window_edit_toolbars_create_editor (void);
static void mn_main_window_edit_toolbars_weak_notify_cb (gpointer data,
							 GObject *former_object);

static void mn_main_window_fullscreen (void);
static void mn_main_window_unfullscreen (void);

static gboolean mn_main_window_leave_fullscreen_timeout_cb (gpointer data);

static gboolean mn_main_window_leave_fullscreen_enter_notify_event_h (GtkWidget *widget,
								      GdkEventCrossing *event,
								      gpointer user_data);
static gboolean mn_main_window_leave_fullscreen_leave_notify_event_h (GtkWidget *widget,
								      GdkEventCrossing *event,
								      gpointer user_data);

static void mn_main_window_update_leave_fullscreen_popup_position (void);

static gboolean mn_main_window_fullscreen_key_press_event_h (GtkWidget *widget,
							     GdkEventKey *event,
							     gpointer user_data);
static void mn_main_window_fullscreen_menubar_deactivate_h (GtkMenuShell *menushell,
							    gpointer user_data);

/*** implementation **********************************************************/

void
mn_main_window_display (void)
{
  GError *err = NULL;
  GtkAccelGroup *accel_group;
  GSList *l;

  g_return_if_fail(mn_shell != NULL);

  if (self.window)
    {
      gtk_window_present(GTK_WINDOW(self.window));
      return;
    }

  /* we don't need the popup anymore */
  mn_summary_popup_destroy();

  self.ui_manager = gtk_ui_manager_new();

  g_object_connect(self.ui_manager,
		   "signal::connect-proxy", mn_main_window_ui_manager_connect_proxy_h, NULL,
		   "signal::disconnect-proxy", mn_main_window_ui_manager_disconnect_proxy_h, NULL,
		   NULL);

  self.action_group = gtk_action_group_new("MainWindowActions");
  gtk_action_group_set_translation_domain(self.action_group, NULL);

  gtk_action_group_add_actions(self.action_group,
			       menu_entries,
			       G_N_ELEMENTS(menu_entries),
			       NULL);
  gtk_action_group_add_toggle_actions(self.action_group,
				      toggle_entries,
				      G_N_ELEMENTS(toggle_entries),
				      NULL);
  gtk_action_group_add_radio_actions(self.action_group,
				     toolbars_style_entries,
				     G_N_ELEMENTS(toolbars_style_entries),
				     -1,
				     G_CALLBACK(mn_main_window_update_toolbars_style),
				     NULL);
  gtk_ui_manager_insert_action_group(self.ui_manager, self.action_group, 0);

  g_object_set(G_OBJECT(GET_ACTION("MailMailReader")),
	       "is-important", TRUE,
	       NULL);

  if (! gtk_ui_manager_add_ui_from_file(self.ui_manager, MN_INTERFACE_FILE("menus.xml"), &err))
    {
      g_critical(_("unable to load menus.xml: %s"), err->message);
      g_error_free(err);
    }

  self.toolbars_file = g_build_filename(mn_conf_dot_dir, "toolbars.xml", NULL);
  mn_main_window_create_toolbars_model(TRUE);

  mn_create_interface(MN_INTERFACE_FILE("main.glade"),
		      "window", &self.window,
		      "menubar", &self.menubar,
		      "toolbar", &self.toolbar,
		      "statusbar", &self.statusbar,
		      "scrolled", &self.scrolled,
		      NULL);
  g_object_weak_ref(G_OBJECT(self.window), mn_main_window_weak_notify_cb, NULL);

  /*
   * Glade forces an useless viewport into the scrolled window if we
   * use a custom widget, so we don't.
   */
  self.message_view = mn_message_view_new(FALSE, TRUE);
  gtk_widget_set_name(self.message_view, "mn-message-view");
  gtk_container_set_border_width(GTK_CONTAINER(self.message_view), 12);
  gtk_container_add(GTK_CONTAINER(self.scrolled), self.message_view);
  gtk_widget_show(self.message_view);
  
  accel_group = gtk_ui_manager_get_accel_group(self.ui_manager);
  gtk_window_add_accel_group(GTK_WINDOW(self.window), accel_group);
  
  /*
   * ~/.gnome2/accels/mail-notification is automatically loaded by the
   * libgnomeui module, we just need to save it ourselves.
   */
  g_signal_connect(accel_group, "accel-changed", G_CALLBACK(gnome_accelerators_sync), NULL);

  self.help_cid = gtk_statusbar_get_context_id(GTK_STATUSBAR(self.statusbar), _("Help messages"));
  self.count_cid = gtk_statusbar_get_context_id(GTK_STATUSBAR(self.statusbar), _("Count messages"));

  mn_conf_link(self.window, MN_CONF_MAIN_WINDOW_DIMENSIONS,
	       GET_ACTION("ViewToolbars"), MN_CONF_MAIN_WINDOW_VIEW_TOOLBARS,
	       GET_ACTION("ViewStatusbar"), MN_CONF_MAIN_WINDOW_VIEW_STATUSBAR,
	       GET_ACTION("ViewToolbarsStyleDesktopDefault"), MN_CONF_MAIN_WINDOW_TOOLBARS_STYLE, MN_TYPE_TOOLBAR_STYLE,
	       NULL);

  mn_main_window_link_view_action(GTK_TOGGLE_ACTION(GET_ACTION("ViewToolbars")), mn_main_window_update_toolbars_visibility);
  mn_main_window_link_view_action(GTK_TOGGLE_ACTION(GET_ACTION("ViewStatusbar")), mn_main_window_update_statusbar_visibility);

  mn_main_window_update_messages();
  mn_main_window_update_sensitivity();
  
  mn_conf_notification_add(self.window,
			   MN_CONF_COMMANDS_MAIL_READER_NAMESPACE,
			   mn_main_window_notify_mail_reader_cb,
			   NULL);
  mn_g_object_connect(self.window,
		      mn_shell->mailboxes,
		      "signal::notify::must-poll", mn_main_window_update_sensitivity, NULL,
		      "signal::messages-changed", mn_main_window_update_messages, NULL,
		      NULL);

  gtk_widget_grab_focus(self.message_view);
  gtk_widget_show(self.window);

  MN_LIST_FOREACH(l, self.transients)
    {
      GtkWindow *window = l->data;
      gtk_window_set_transient_for(window, GTK_WINDOW(self.window));
    }
}

gboolean
mn_main_window_is_displayed (void)
{
  return self.window != NULL;
}

void
mn_main_window_add_transient (GtkWindow *window)
{
  self.transients = g_slist_append(self.transients, window);
  g_object_weak_ref(G_OBJECT(window), mn_main_window_transient_weak_notify_cb, NULL);

  if (self.window)
    gtk_window_set_transient_for(window, GTK_WINDOW(self.window));
}

static void
mn_main_window_transient_weak_notify_cb (gpointer data, GObject *former_object)
{
  self.transients = g_slist_remove(self.transients, former_object);
}

static void
mn_main_window_weak_notify_cb (gpointer data, GObject *former_object)
{
  g_object_unref(self.ui_manager);
  self.ui_manager = NULL;

  g_object_unref(self.action_group);
  self.action_group = NULL;

  g_object_unref(self.toolbars_model);
  self.toolbars_model = NULL;

  g_free(self.toolbars_file);
  self.toolbars_file = NULL;

  mn_source_remove(&self.toolbars_model_save_idle_id);

  self.window = NULL;
  self.toolbar = NULL;
}

static void
mn_main_window_notify_mail_reader_cb (GConfClient *client,
				      unsigned int cnxn_id,
				      GConfEntry *entry,
				      gpointer user_data)
{
  GDK_THREADS_ENTER();
  mn_main_window_update_sensitivity();
  GDK_THREADS_LEAVE();
}

static void
mn_main_window_update_messages (void)
{
  GSList *messages;
  int count;

  messages = mn_mailboxes_get_messages(mn_shell->mailboxes);
  count = g_slist_length(messages);

  mn_message_view_set_messages(MN_MESSAGE_VIEW(self.message_view), messages);
  mn_g_object_slist_free(messages);

  if (count > 0)
    {
      char *status;

      status = g_strdup_printf(ngettext("%i new message", "%i new messages", count), count);
      STATUS_PUSH(self.count_cid, status);
      g_free(status);
    }
  else
    STATUS_POP(self.count_cid);
}

static void
mn_main_window_update_sensitivity (void)
{
  g_object_set(GET_ACTION("MailMailReader"),
	       "sensitive", mn_conf_has_command(MN_CONF_COMMANDS_MAIL_READER_NAMESPACE),
	       NULL);
  g_object_set(GET_ACTION("MailUpdate"),
	       "sensitive", mn_mailboxes_get_must_poll(mn_shell->mailboxes),
	       NULL);
}

static void
mn_main_window_create_toolbars_model (gboolean use_dot_file)
{
  if (self.toolbars_model)
    g_object_unref(self.toolbars_model);

  self.toolbars_model = egg_toolbars_model_new();

  g_object_connect(self.toolbars_model,
		   "signal-after::item-added", mn_main_window_toolbars_model_save_changes, NULL,
		   "signal-after::item-removed", mn_main_window_toolbars_model_save_changes, NULL,
		   "signal-after::toolbar-added", mn_main_window_toolbars_model_update_flags_and_save_changes, NULL,
		   "signal-after::toolbar-removed", mn_main_window_toolbars_model_update_flags_and_save_changes, NULL,
		   NULL);

  if (! use_dot_file || ! egg_toolbars_model_load(self.toolbars_model, self.toolbars_file))
    {
      if (! egg_toolbars_model_load(self.toolbars_model, MN_INTERFACE_FILE("toolbars.xml")))
	g_critical(_("unable to load toolbars.xml"));
    }
  
  if (egg_toolbars_model_n_toolbars(self.toolbars_model) < 1)
    egg_toolbars_model_add_toolbar(self.toolbars_model, 0, "DefaultToolbar");

  if (self.toolbar)
    egg_editable_toolbar_set_model(EGG_EDITABLE_TOOLBAR(self.toolbar), self.toolbars_model);
}

static void
mn_main_window_ui_manager_connect_proxy_h (GtkUIManager *manager,
					   GtkAction *action,
					   GtkWidget *proxy,
					   gpointer user_data)
{
  if (GTK_IS_MENU_ITEM(proxy))
    g_object_connect(proxy,
		     "signal::select", mn_main_window_menu_item_select_h, NULL,
		     "signal::deselect", mn_main_window_menu_item_deselect_h, NULL,
		     NULL);
}

static void
mn_main_window_ui_manager_disconnect_proxy_h (GtkUIManager *manager,
					      GtkAction *action,
					      GtkWidget *proxy,
					      gpointer user_data)
{
  if (GTK_IS_MENU_ITEM(proxy))
    g_object_disconnect(proxy,
			"any-signal", mn_main_window_menu_item_select_h, NULL,
			"any-signal", mn_main_window_menu_item_deselect_h, NULL,
			NULL);
}

static void
mn_main_window_menu_item_select_h (GtkMenuItem *item, gpointer user_data)
{
  GtkAction *action;
  char *tooltip = NULL;

  action = g_object_get_data(G_OBJECT(item), "gtk-action");
  g_return_if_fail(action != NULL);
  
  g_object_get(G_OBJECT(action), "tooltip", &tooltip, NULL);
  if (tooltip)
    {
      STATUS_PUSH(self.help_cid, tooltip);
      g_free(tooltip);
    }
}

static void
mn_main_window_menu_item_deselect_h (GtkMenuItem *item, gpointer user_data)
{
  STATUS_POP(self.help_cid);
}

static void
mn_main_window_update_toolbars_style (void)
{
  MNToolbarStyle style;
  EggTbModelFlags flag;
  int n_toolbars;
  int i;

  style = gtk_radio_action_get_current_value(GTK_RADIO_ACTION(GET_ACTION("ViewToolbarsStyleDesktopDefault")));
  if (style == MN_TOOLBAR_STYLE_DESKTOP_DEFAULT)
    g_object_get(gtk_widget_get_settings(self.toolbar), "gtk-toolbar-style", &style, NULL);

  if (FULLSCREEN && style == MN_TOOLBAR_STYLE_BOTH)
    style = MN_TOOLBAR_STYLE_ICONS;

  flag = 1 << (style + 1);
  n_toolbars = egg_toolbars_model_n_toolbars(self.toolbars_model);

  for (i = 0; i < n_toolbars; i++)
    {
      EggTbModelFlags flags;

      flags = egg_toolbars_model_get_flags(self.toolbars_model, i);
      flags &= ~ (EGG_TB_MODEL_ICONS_ONLY
		  | EGG_TB_MODEL_TEXT_ONLY
		  | EGG_TB_MODEL_ICONS_TEXT
		  | EGG_TB_MODEL_ICONS_TEXT_HORIZ);
      flags |= flag;
      egg_toolbars_model_set_flags(self.toolbars_model, i, flags);
    }
}

static void
mn_main_window_toolbars_model_save_changes (void)
{
  if (! self.toolbars_model_save_idle_id)
    self.toolbars_model_save_idle_id = g_idle_add(mn_main_window_toolbars_model_save_changes_cb, NULL);
}

static gboolean
mn_main_window_toolbars_model_save_changes_cb (gpointer data)
{
  GDK_THREADS_ENTER();
  egg_toolbars_model_save(self.toolbars_model, self.toolbars_file, "1.0");
  self.toolbars_model_save_idle_id = 0;
  GDK_THREADS_LEAVE();

  return FALSE;			/* remove */
}

static void
mn_main_window_toolbars_model_update_flags_and_save_changes (void)
{
  int i;
  int n_toolbars;
  EggTbModelFlags flag = EGG_TB_MODEL_ACCEPT_ITEMS_ONLY;

  n_toolbars = egg_toolbars_model_n_toolbars(self.toolbars_model);
  
  if (n_toolbars <= 1)
    flag |= EGG_TB_MODEL_NOT_REMOVABLE;

  for (i = 0; i < n_toolbars; i++)
    {
      EggTbModelFlags flags;

      flags = egg_toolbars_model_get_flags(self.toolbars_model, i);
      flags &= ~EGG_TB_MODEL_NOT_REMOVABLE;
      flags |= flag;
      egg_toolbars_model_set_flags(self.toolbars_model, i, flags);
    }

  mn_main_window_toolbars_model_save_changes();
}

static void
mn_main_window_link_view_action (GtkToggleAction *action, GCallback cb)
{
  g_return_if_fail(GTK_IS_TOGGLE_ACTION(action));
  g_return_if_fail(cb != NULL);

  cb();
  g_signal_connect(action, "toggled", cb, NULL);
}

static void
mn_main_window_update_toolbars_visibility (void)
{
  g_object_set(G_OBJECT(self.toolbar), "visible", gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(GET_ACTION("ViewToolbars"))), NULL);
}

static void
mn_main_window_update_statusbar_visibility (void)
{
  g_object_set(G_OBJECT(self.statusbar), "visible", ! FULLSCREEN && gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(GET_ACTION("ViewStatusbar"))), NULL);
}

static void
mn_main_window_mail_reader_activate_h (void)
{
  mn_execute_command(MN_CONF_COMMANDS_MAIL_READER_COMMAND);
}

static void
mn_main_window_update_activate_h (void)
{
  mn_mailboxes_check(mn_shell->mailboxes);
}

static void
mn_main_window_close_activate_h (void)
{
  gtk_widget_destroy(self.window);
}

static void
mn_main_window_quit_activate_h (void)
{
  g_object_unref(mn_shell);
}

static void
mn_main_window_edit_toolbars_activate_h (void)
{
  if (self.edit_toolbars_dialog)
    {
      gtk_window_present(GTK_WINDOW(self.edit_toolbars_dialog));
      return;
    }
  
  mn_create_interface(MN_INTERFACE_FILE("edit-toolbars.glade"),
		      "dialog", &self.edit_toolbars_dialog,
		      NULL);

  eel_add_weak_pointer(&self.edit_toolbars_dialog);
  g_object_weak_ref(G_OBJECT(self.edit_toolbars_dialog), mn_main_window_edit_toolbars_weak_notify_cb, NULL);

  gtk_window_set_transient_for(GTK_WINDOW(self.edit_toolbars_dialog), GTK_WINDOW(self.window));

  mn_main_window_edit_toolbars_create_editor();
  egg_editable_toolbar_set_edit_mode(EGG_EDITABLE_TOOLBAR(self.toolbar), TRUE);

  mn_conf_link(self.edit_toolbars_dialog, MN_CONF_MAIN_WINDOW_EDIT_TOOLBARS_DIALOG, NULL);
  gtk_widget_show(self.edit_toolbars_dialog);
}

static void
mn_main_window_edit_toolbars_create_editor (void)
{
  g_return_if_fail(self.edit_toolbars_dialog != NULL);

  if (self.edit_toolbars_editor)
    gtk_widget_destroy(self.edit_toolbars_editor);

  self.edit_toolbars_editor = egg_toolbar_editor_new(self.ui_manager, self.toolbars_model);
  eel_add_weak_pointer(&self.edit_toolbars_editor);

  gtk_container_set_border_width(GTK_CONTAINER(self.edit_toolbars_editor), 5);
  gtk_box_set_spacing(GTK_BOX(self.edit_toolbars_editor), 6);

  egg_toolbar_editor_load_actions(EGG_TOOLBAR_EDITOR(self.edit_toolbars_editor), MN_INTERFACE_FILE("toolbars.xml"));

  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(self.edit_toolbars_dialog)->vbox), self.edit_toolbars_editor);
  gtk_widget_show(self.edit_toolbars_editor);
}

static void
mn_main_window_edit_toolbars_weak_notify_cb (gpointer data, GObject *former_object)
{
  egg_editable_toolbar_set_edit_mode(EGG_EDITABLE_TOOLBAR(self.toolbar), FALSE);
}

static void
mn_main_window_preferences_activate_h (void)
{
  mn_properties_dialog_display();
}

static void
mn_main_window_help_contents_activate_h (void)
{
  mn_display_help(GTK_WINDOW(self.window), "main-window");
}

static void
mn_main_window_about_activate_h (void)
{
  mn_shell_display_about_dialog(mn_shell);
}

static void
mn_main_window_fullscreen (void)
{
  GtkWidget *button;

  self.leave_fullscreen_popup = gtk_window_new(GTK_WINDOW_POPUP);
  eel_add_weak_pointer(&self.leave_fullscreen_popup);

  button = gtk_button_new_from_stock(MN_STOCK_LEAVE_FULLSCREEN);
  gtk_container_add(GTK_CONTAINER(self.leave_fullscreen_popup), button);
  gtk_widget_show(button);

  g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_unfullscreen), self.window);

  gtk_widget_hide(self.menubar);
  mn_main_window_update_toolbars_style();
  mn_main_window_update_statusbar_visibility();

  self.leave_fullscreen_popup_visible = TRUE;
  mn_main_window_update_leave_fullscreen_popup_position();
  gtk_widget_show(self.leave_fullscreen_popup);

  g_signal_connect(gdk_screen_get_default(),
		   "size-changed",
		   G_CALLBACK(mn_main_window_update_leave_fullscreen_popup_position),
		   NULL);
  g_signal_connect(self.window,
		   "key-press-event",
		   G_CALLBACK(mn_main_window_fullscreen_key_press_event_h),
		   NULL);

  g_object_connect(self.leave_fullscreen_popup,
		   "signal::enter-notify-event", mn_main_window_leave_fullscreen_enter_notify_event_h, NULL,
		   "signal::leave-notify-event", mn_main_window_leave_fullscreen_leave_notify_event_h, NULL,
		   NULL);

  self.leave_fullscreen_timeout_id = g_timeout_add(5000, mn_main_window_leave_fullscreen_timeout_cb, NULL);
}

static void
mn_main_window_unfullscreen (void)
{
  g_signal_handlers_disconnect_by_func(gdk_screen_get_default(), mn_main_window_update_leave_fullscreen_popup_position, NULL);
  g_signal_handlers_disconnect_by_func(self.window, mn_main_window_fullscreen_key_press_event_h, NULL);

  mn_source_remove(&self.leave_fullscreen_timeout_id);
  gtk_widget_destroy(self.leave_fullscreen_popup);

  gtk_widget_show(self.menubar);
  mn_main_window_update_toolbars_style();
  mn_main_window_update_statusbar_visibility();
}

static gboolean
mn_main_window_leave_fullscreen_timeout_cb (gpointer data)
{
  GDK_THREADS_ENTER();

  self.leave_fullscreen_popup_visible = FALSE;
  mn_main_window_update_leave_fullscreen_popup_position();
  self.leave_fullscreen_timeout_id = 0;

  GDK_THREADS_LEAVE();
  
  return FALSE;			/* remove timeout */
}

static gboolean
mn_main_window_leave_fullscreen_enter_notify_event_h (GtkWidget *widget,
						      GdkEventCrossing *event,
						      gpointer user_data)
{
  mn_source_remove(&self.leave_fullscreen_timeout_id);
  self.leave_fullscreen_popup_visible = TRUE;
  mn_main_window_update_leave_fullscreen_popup_position();

  return FALSE;			/* propagate event */
}

static gboolean
mn_main_window_leave_fullscreen_leave_notify_event_h (GtkWidget *widget,
						      GdkEventCrossing *event,
						      gpointer user_data)
{
  mn_source_remove(&self.leave_fullscreen_timeout_id);
  self.leave_fullscreen_timeout_id = g_timeout_add(5000, mn_main_window_leave_fullscreen_timeout_cb, NULL);

  return FALSE;			/* propagate event */
}

static void
mn_main_window_update_leave_fullscreen_popup_position (void)
{
  GdkScreen *screen;
  int monitor_num;
  GdkRectangle screen_rect;
  int popup_width;
  int popup_height;
  int y;

  g_return_if_fail(self.leave_fullscreen_popup != NULL);

  screen = gdk_screen_get_default();
  monitor_num = gdk_screen_get_monitor_at_window(screen, GTK_WIDGET(self.window)->window);
  gdk_screen_get_monitor_geometry(screen, monitor_num, &screen_rect);
  gtk_window_get_size(GTK_WINDOW(self.leave_fullscreen_popup), &popup_width, &popup_height);

  y = screen_rect.y;
  if (! self.leave_fullscreen_popup_visible)
    y -= popup_height - 2;

  gtk_window_move(GTK_WINDOW(self.leave_fullscreen_popup), screen_rect.width - popup_width, y);
}

static gboolean
mn_main_window_fullscreen_key_press_event_h (GtkWidget *widget,
					     GdkEventKey *event,
					     gpointer user_data)
{
  unsigned int modifiers = gtk_accelerator_get_default_mod_mask();

  if (event->keyval == GDK_Escape && (event->state & modifiers) == 0)
    {
      gtk_window_unfullscreen(GTK_WINDOW(self.window));
      return TRUE;	/* do not propagate event */
    }
  else if (event->keyval == GDK_F10 && (event->state & modifiers) == 0)
    {
      g_signal_connect(self.menubar, "deactivate", G_CALLBACK(mn_main_window_fullscreen_menubar_deactivate_h), NULL);

      gtk_widget_show(self.menubar);
      gtk_menu_shell_select_first(GTK_MENU_SHELL(self.menubar), FALSE);

      return TRUE;	/* do not propagate event */
    }
  else
    return FALSE;	/* propagate event */
}

static void
mn_main_window_fullscreen_menubar_deactivate_h (GtkMenuShell *menushell,
						gpointer user_data)
{
  g_signal_handlers_disconnect_by_func(self.menubar, G_CALLBACK(mn_main_window_fullscreen_menubar_deactivate_h), NULL);
  gtk_menu_shell_deselect(menushell);
  gtk_widget_hide(self.menubar);
}

/* libglade callbacks */

GtkWidget *
mn_main_window_get_widget_cb (const char *widget_name,
			      const char *string1,
			      const char *string2,
			      int int1,
			      int int2)
{
  GtkWidget *widget;

  widget = gtk_ui_manager_get_widget(self.ui_manager, string1);
  g_return_val_if_fail(widget != NULL, NULL);

  return widget;
}

GtkWidget *
mn_main_window_toolbar_new_cb (void)
{
  GtkWidget *toolbar;

  toolbar = egg_editable_toolbar_new_with_model(self.ui_manager, self.toolbars_model);
  gtk_widget_show(toolbar);

  return toolbar;
}

void
mn_main_window_edit_toolbars_response_h (GtkDialog *dialog,
					 int response,
					 gpointer user_data)
{
  switch (response)
    {
    case GTK_RESPONSE_HELP:
      mn_display_help(GTK_WINDOW(self.edit_toolbars_dialog), "main-window-edit-toolbars");
      break;

    case 1:			/* use default */
      /* recreate the model, discarding the dot file */
      mn_main_window_create_toolbars_model(FALSE);
      /* and recreate the editor, since the model has changed */
      mn_main_window_edit_toolbars_create_editor();
      break;

    case 2:			/* add toolbar */
      egg_toolbars_model_add_toolbar(self.toolbars_model, -1, "UserCreated");
      break;

    case GTK_RESPONSE_CLOSE:
      gtk_widget_destroy(self.edit_toolbars_dialog);
      break;
    }
}

gboolean
mn_main_window_window_state_event_h (GtkWidget *widget,
				     GdkEventWindowState *event,
				     gpointer user_data)
{
  if (event->changed_mask & GDK_WINDOW_STATE_FULLSCREEN)
    {
      if (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)
	mn_main_window_fullscreen();
      else
	mn_main_window_unfullscreen();
    }

  return FALSE;			/* propagate event */
}
