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
#include <libgnome/gnome-i18n.h>
#include "mn-mailbox.h"
#ifdef WITH_MBOX
#include "mn-mbox-mailbox.h"
#endif
#ifdef WITH_MH
#include "mn-mh-mailbox.h"
#endif
#ifdef WITH_MAILDIR
#include "mn-maildir-mailbox.h"
#endif
#ifdef WITH_POP3
#include "mn-pop3-mailbox.h"
#endif
#ifdef WITH_SYLPHEED
#include "mn-sylpheed-mailbox.h"
#endif
#include "mn-unsupported-mailbox.h"

/*** cpp *********************************************************************/

#define MN_MAILBOX_TRY_NEW(type)					\
{									\
  MNMailboxClass *class;						\
									\
  class = g_type_class_ref(type);					\
  if (class->is(locator))						\
    {									\
      MNMailbox *mailbox;						\
									\
      mailbox = g_object_new(type,					\
			     "locator", locator,			\
			     NULL);					\
      if (mailbox->err)							\
	{								\
	  g_set_error(err,						\
		      MN_MAILBOX_ERROR,					\
		      MN_MAILBOX_ERROR_INITIALIZATION,			\
		      _("unable to initialize %s mailbox: %s"),		\
		      class->format,					\
		      mailbox->err->message);				\
	  g_object_unref(mailbox);					\
	  goto unsupported;						\
	}								\
									\
      return mailbox;							\
    }									\
}

/*** types *******************************************************************/

enum {
  PROP_0,
  PROP_LOCATOR
};

/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;

/*** functions ***************************************************************/

static void	mn_mailbox_class_init	(MNMailboxClass	*class);
static void	mn_mailbox_set_property	(GObject	*object,
					 guint		prop_id,
					 const GValue	*value,
					 GParamSpec	*pspec);
static void	mn_mailbox_init		(MNMailbox	*mailbox);
static void	mn_mailbox_finalize	(GObject	*object);

/*** implementation **********************************************************/

GType
mn_mailbox_get_type (void)
{
  static GType mailbox_type = 0;
  
  if (! mailbox_type)
    {
      static const GTypeInfo mailbox_info = {
	sizeof(MNMailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNMailbox),
	0,
	(GInstanceInitFunc) mn_mailbox_init,
      };
      
      mailbox_type = g_type_register_static(G_TYPE_OBJECT,
					    "MNMailbox",
					    &mailbox_info,
					    0);
    }
  
  return mailbox_type;
}

static void
mn_mailbox_class_init (MNMailboxClass *class)
{
  GObjectClass *object_class;

  parent_class = g_type_class_peek_parent(class);

  class->format = NULL;
  class->is_remote = FALSE;
  class->is = NULL;
  class->has_new = NULL;

  object_class = G_OBJECT_CLASS(class);
  object_class->set_property = mn_mailbox_set_property;
  object_class->finalize = mn_mailbox_finalize;

  g_object_class_install_property(object_class,
                                  PROP_LOCATOR,
                                  g_param_spec_string("locator",
                                                      "Locator",
                                                      _("The mailbox's locator"),
                                                      NULL,
                                                      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
mn_mailbox_set_property (GObject *object,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *pspec)
{
  MNMailbox *mailbox;

  mailbox = MN_MAILBOX(object);

  switch (prop_id)
    {
    case PROP_LOCATOR:
      mailbox->locator = g_value_dup_string(value);
      /*
       * The default visible name is the locator itself, subclasses
       * are free to override it.
       */
      mailbox->name = mailbox->locator;
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
}

static void
mn_mailbox_init (MNMailbox *mailbox)
{
  mailbox->locator = NULL;
  mailbox->name = NULL;
  mailbox->err = NULL;
}

static void
mn_mailbox_finalize (GObject *object)
{
  MNMailbox *mailbox;

  mailbox = MN_MAILBOX(object);

  g_free(mailbox->locator);
  if (mailbox->err)
    g_error_free(mailbox->err);

  G_OBJECT_CLASS(parent_class)->finalize(object);
}

MNMailbox *mn_mailbox_new (const char *locator, GError **err)
{
  g_return_val_if_fail(locator != NULL, NULL);

#ifdef WITH_POP3
  MN_MAILBOX_TRY_NEW(MN_TYPE_POP3_MAILBOX);
#endif

  if (! g_file_test(locator, G_FILE_TEST_EXISTS))
    {
      g_set_error(err, MN_MAILBOX_ERROR, MN_MAILBOX_ERROR_NOT_FOUND,
		  _("mailbox not found"));
      goto unsupported;
    }
  
#ifdef WITH_MBOX
  MN_MAILBOX_TRY_NEW(MN_TYPE_MBOX_MAILBOX);
#endif
#ifdef WITH_MH
  MN_MAILBOX_TRY_NEW(MN_TYPE_MH_MAILBOX);
#endif
#ifdef WITH_MAILDIR
  MN_MAILBOX_TRY_NEW(MN_TYPE_MAILDIR_MAILBOX);
#endif
#ifdef WITH_SYLPHEED
  MN_MAILBOX_TRY_NEW(MN_TYPE_SYLPHEED_MAILBOX);
#endif

  /* unknown format, set err and use the dummy MNUnsupportedMailbox class */

  g_set_error(err, MN_MAILBOX_ERROR, MN_MAILBOX_ERROR_UNKNOWN_FORMAT,
	      _("unknown mailbox format"));

 unsupported:
  return g_object_new(MN_TYPE_UNSUPPORTED_MAILBOX,
		      "locator", locator,
		      NULL);
}

GQuark
mn_mailbox_error_quark (void)
{
  static GQuark quark = 0;

  if (! quark)
    quark = g_quark_from_static_string("mn_mailbox_error");

  return quark;
}
