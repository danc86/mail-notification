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
#include <glib/gi18n-lib.h>
#include "mn-unsupported-mailbox.h"
#include "mn-stock.h"

/*** types *******************************************************************/

enum {
  PROP_0,
  PROP_REASON
};

struct _MNUnsupportedMailboxPrivate
{
  char *reason;
};

/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;

/*** functions ***************************************************************/

static void mn_unsupported_mailbox_class_init (MNUnsupportedMailboxClass *class);
static void mn_unsupported_mailbox_init (MNUnsupportedMailbox *mailbox);
static void mn_unsupported_mailbox_finalize (GObject *object);

static void mn_unsupported_mailbox_set_property (GObject *object,
						 guint prop_id,
						 const GValue *value,
						 GParamSpec *pspec);
static void mn_unsupported_mailbox_get_property (GObject *object,
						 unsigned int prop_id,
						 GValue *value,
						 GParamSpec *pspec);

/*** implementation **********************************************************/

GType
mn_unsupported_mailbox_get_type (void)
{
  static GType unsupported_mailbox_type = 0;
  
  if (! unsupported_mailbox_type)
    {
      static const GTypeInfo unsupported_mailbox_info = {
	sizeof(MNUnsupportedMailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_unsupported_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNUnsupportedMailbox),
	0,
	(GInstanceInitFunc) mn_unsupported_mailbox_init
      };
      
      unsupported_mailbox_type = g_type_register_static(MN_TYPE_MAILBOX,
							"MNUnsupportedMailbox",
							&unsupported_mailbox_info,
							0);
    }
  
  return unsupported_mailbox_type;
}

static void
mn_unsupported_mailbox_class_init (MNUnsupportedMailboxClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS(class);
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  parent_class = g_type_class_peek_parent(class);

  object_class->set_property = mn_unsupported_mailbox_set_property;
  object_class->get_property = mn_unsupported_mailbox_get_property;
  object_class->finalize = mn_unsupported_mailbox_finalize;

  mailbox_class->stock_id = MN_STOCK_UNSUPPORTED;
  mailbox_class->format = _("unsupported");

  g_object_class_install_property(object_class,
                                  PROP_REASON,
                                  g_param_spec_string("reason",
                                                      _("Reason"),
                                                      _("The reason why the mailbox is unsupported"),
                                                      NULL,
                                                      G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
mn_unsupported_mailbox_init (MNUnsupportedMailbox *mailbox)
{
  mailbox->priv = g_new0(MNUnsupportedMailboxPrivate, 1);
}

static void
mn_unsupported_mailbox_finalize (GObject *object)
{
  MNUnsupportedMailbox *mailbox = MN_UNSUPPORTED_MAILBOX(object);

  g_free(mailbox->priv->reason);
  g_free(mailbox->priv);

  G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void
mn_unsupported_mailbox_set_property (GObject *object,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *pspec)
{
  MNUnsupportedMailbox *mailbox = MN_UNSUPPORTED_MAILBOX(object);

  switch (prop_id)
    {
    case PROP_REASON:
      g_return_if_fail(mailbox->priv->reason == NULL);
      mailbox->priv->reason = g_value_dup_string(value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
}

static void
mn_unsupported_mailbox_get_property (GObject *object,
				     unsigned int prop_id,
				     GValue *value,
				     GParamSpec *pspec)
{
  MNUnsupportedMailbox *mailbox = MN_UNSUPPORTED_MAILBOX(object);

  switch (prop_id)
    {
    case PROP_REASON:
      g_value_set_string(value, mn_unsupported_mailbox_get_reason(mailbox));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
}

const char *
mn_unsupported_mailbox_get_reason (MNUnsupportedMailbox *mailbox)
{
  g_return_val_if_fail(MN_IS_UNSUPPORTED_MAILBOX(mailbox), NULL);

  return mailbox->priv->reason;
}

MNMailbox *
mn_unsupported_mailbox_new (const char *uri, const char *reason)
{
  g_return_val_if_fail(uri != NULL, NULL);
  g_return_val_if_fail(reason != NULL, NULL);

  return g_object_new(MN_TYPE_UNSUPPORTED_MAILBOX,
		      "uri", uri,
		      "reason", reason,
		      NULL);
}
