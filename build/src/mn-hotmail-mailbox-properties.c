/* Generated by GOB (v2.0.17)   (do not edit directly) */

/* End world hunger, donate to the World Food Programme, http://www.wfp.org */

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 17

#define selfp (self->_priv)

#include <string.h> /* memset() */

#include "mn-hotmail-mailbox-properties.h"

#include "mn-hotmail-mailbox-properties-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */

#line 24 "src/mn-hotmail-mailbox-properties.gob"

#include "mn-stock.h"

#line 30 "mn-hotmail-mailbox-properties.c"
/* self casting macros */
#define SELF(x) MN_HOTMAIL_MAILBOX_PROPERTIES(x)
#define SELF_CONST(x) MN_HOTMAIL_MAILBOX_PROPERTIES_CONST(x)
#define IS_SELF(x) MN_IS_HOTMAIL_MAILBOX_PROPERTIES(x)
#define TYPE_SELF MN_TYPE_HOTMAIL_MAILBOX_PROPERTIES
#define SELF_CLASS(x) MN_HOTMAIL_MAILBOX_PROPERTIES_CLASS(x)

#define SELF_GET_CLASS(x) MN_HOTMAIL_MAILBOX_PROPERTIES_GET_CLASS(x)

/* self typedefs */
typedef MNHotmailMailboxProperties Self;
typedef MNHotmailMailboxPropertiesClass SelfClass;

/* here are local prototypes */
#line 0 "src/mn-hotmail-mailbox-properties.gob"
static void mn_hotmail_mailbox_properties_init (MNHotmailMailboxProperties * o) G_GNUC_UNUSED;
#line 47 "mn-hotmail-mailbox-properties.c"
#line 30 "src/mn-hotmail-mailbox-properties.gob"
static void mn_hotmail_mailbox_properties_class_init (MNHotmailMailboxPropertiesClass * class) G_GNUC_UNUSED;
#line 50 "mn-hotmail-mailbox-properties.c"

/* pointer to the class of our parent */
static MNWebmailMailboxPropertiesClass *parent_class = NULL;

GType
mn_hotmail_mailbox_properties_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (MNHotmailMailboxPropertiesClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mn_hotmail_mailbox_properties_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (MNHotmailMailboxProperties),
			0 /* n_preallocs */,
			(GInstanceInitFunc) mn_hotmail_mailbox_properties_init,
			NULL
		};

		type = g_type_register_static (MN_TYPE_WEBMAIL_MAILBOX_PROPERTIES, "MNHotmailMailboxProperties", &info, (GTypeFlags)0);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((MNHotmailMailboxProperties *)g_object_new(mn_hotmail_mailbox_properties_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static MNHotmailMailboxProperties * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static MNHotmailMailboxProperties *
GET_NEW_VARG (const char *first, ...)
{
	MNHotmailMailboxProperties *ret;
	va_list ap;
	va_start (ap, first);
	ret = (MNHotmailMailboxProperties *)g_object_new_valist (mn_hotmail_mailbox_properties_get_type (), first, ap);
	va_end (ap);
	return ret;
}

static void 
mn_hotmail_mailbox_properties_init (MNHotmailMailboxProperties * o G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:Hotmail:Mailbox:Properties::init"
}
#undef __GOB_FUNCTION__
#line 30 "src/mn-hotmail-mailbox-properties.gob"
static void 
mn_hotmail_mailbox_properties_class_init (MNHotmailMailboxPropertiesClass * class G_GNUC_UNUSED)
{
#line 107 "mn-hotmail-mailbox-properties.c"
#define __GOB_FUNCTION__ "MN:Hotmail:Mailbox:Properties::class_init"

	parent_class = g_type_class_ref (MN_TYPE_WEBMAIL_MAILBOX_PROPERTIES);

 {
#line 31 "src/mn-hotmail-mailbox-properties.gob"

    MNMailboxPropertiesClass *p_class = MN_MAILBOX_PROPERTIES_CLASS(class);

    p_class->type = "hotmail";
    p_class->stock_id = MN_STOCK_HOTMAIL;
    p_class->combo_label = "Windows Live Hotmail";
  
#line 121 "mn-hotmail-mailbox-properties.c"
 }
}
#undef __GOB_FUNCTION__


