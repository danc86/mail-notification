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
#include <string.h>
#include <libintl.h>
#include <libbonoboui.h>

#ifdef HAVE_EVOLUTION_2_2
/* headers from the Evolution source tree */
#include "mail/mail-component.h"
#include "mail/em-folder-tree.h"
#else
#include <mail/mail-component.h>
#include <mail/em-folder-tree.h>
#endif

#include "mn-evolution.h"

/*** types *******************************************************************/

enum
{
  PROP_SELECTED_URI
};

/*** functions ***************************************************************/

static void mn_evolution_folder_tree_control_get_property (BonoboPropertyBag *bag,
							   BonoboArg *arg,
							   unsigned int arg_id,
							   CORBA_Environment *env,
							   gpointer user_data);
static void mn_evolution_folder_tree_control_set_property (BonoboPropertyBag *bag,
							   const BonoboArg *arg,
							   unsigned int arg_id,
							   CORBA_Environment *env,
							   gpointer user_data);

static void mn_evolution_folder_tree_control_selected_h (EMFolderTree *tree,
							 const char *full_name,
							 const char *uri,
							 guint32 flags,
							 gpointer user_data);

/*** implementation **********************************************************/

BonoboObject *
mn_evolution_folder_tree_control_factory_cb (BonoboGenericFactory *factory,
					     const char *iid,
					     gpointer closure)
{
  EMFolderTreeModel *model;
  GtkWidget *tree;
  BonoboControl *control;
  BonoboPropertyBag *pb;

  if (strcmp(iid, MN_EVOLUTION_FOLDER_TREE_CONTROL_IID) != 0)
    return NULL;

  model = mail_component_peek_tree_model(mail_component_peek());
  tree = em_folder_tree_new_with_model(model);
  gtk_widget_show(tree);

  control = bonobo_control_new(tree);

  pb = bonobo_property_bag_new(mn_evolution_folder_tree_control_get_property,
			       mn_evolution_folder_tree_control_set_property,
			       tree);

  bonobo_property_bag_add(pb,
			  "selected-uri",
			  PROP_SELECTED_URI,
			  BONOBO_ARG_STRING,
			  NULL,
			  dgettext(GETTEXT_PACKAGE, "The currently selected URI"),
			  0);

  bonobo_control_set_properties(control, bonobo_object_corba_objref(BONOBO_OBJECT(pb)), NULL);

  g_signal_connect_data(tree,
			"folder-selected",
			G_CALLBACK(mn_evolution_folder_tree_control_selected_h),
			pb,
			(GClosureNotify) bonobo_object_unref,
			0);

  return BONOBO_OBJECT(control);
}

static void
mn_evolution_folder_tree_control_get_property (BonoboPropertyBag *bag,
					       BonoboArg *arg,
					       unsigned int arg_id,
					       CORBA_Environment *env,
					       gpointer user_data)
{
  EMFolderTree *tree = user_data;

  switch (arg_id)
    {
    case PROP_SELECTED_URI:
      {
	char *selected_uri;

	selected_uri = em_folder_tree_get_selected_uri(tree);
	BONOBO_ARG_SET_STRING(arg, selected_uri);
	g_free(selected_uri);
      }
      break;

    default:
      bonobo_exception_set(env, ex_Bonobo_PropertyBag_NotFound);
      break;
    }
}

static void
mn_evolution_folder_tree_control_set_property (BonoboPropertyBag *bag,
					       const BonoboArg *arg,
					       unsigned int arg_id,
					       CORBA_Environment *env,
					       gpointer user_data)
{
  EMFolderTree *tree = user_data;

  switch (arg_id)
    {
    case PROP_SELECTED_URI:
      em_folder_tree_set_selected(tree, BONOBO_ARG_GET_STRING(arg));
      break;

    default:
      bonobo_exception_set(env, ex_Bonobo_PropertyBag_NotFound);
      break;
    }
}

static void
mn_evolution_folder_tree_control_selected_h (EMFolderTree *tree,
					     const char *full_name,
					     const char *uri,
					     guint32 flags,
					     gpointer user_data)
{
  BonoboPropertyBag *pb = user_data;
  BonoboArg *arg;

  arg = bonobo_arg_new(BONOBO_ARG_STRING);
  BONOBO_ARG_SET_STRING(arg, uri);

  bonobo_event_source_notify_listeners_full(pb->es,
					    "Bonobo/Property",
					    "change",
					    "selected-uri",
					    arg,
					    NULL);

  bonobo_arg_release(arg);
}
