/* Generated by GOB (v2.0.17)   (do not edit directly) */

#include <glib.h>
#include <glib-object.h>


#include "mn-pi-mailbox.h"

#ifndef __MN_IMAP_MAILBOX_H__
#define __MN_IMAP_MAILBOX_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#define MN_IMAP_MAILBOX_N_USE_IDLE	3

extern int mn_imap_mailbox_default_ports[MN_PI_MAILBOX_N_CONNECTION_TYPES];


typedef enum {
	MN_IMAP_MAILBOX_USE_IDLE_NEVER,
	MN_IMAP_MAILBOX_USE_IDLE_AUTODETECT,
	MN_IMAP_MAILBOX_USE_IDLE_ALWAYS
} MNIMAPMailboxUseIDLE;
#define MN_TYPE_IMAP_MAILBOX_USE_IDLE mn_imap_mailbox_use_idle_get_type()
GType mn_imap_mailbox_use_idle_get_type (void) G_GNUC_CONST;


/*
 * Type checking and casting macros
 */
#define MN_TYPE_IMAP_MAILBOX	(mn_imap_mailbox_get_type())
#define MN_IMAP_MAILBOX(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_imap_mailbox_get_type(), MNIMAPMailbox)
#define MN_IMAP_MAILBOX_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_imap_mailbox_get_type(), MNIMAPMailbox const)
#define MN_IMAP_MAILBOX_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), mn_imap_mailbox_get_type(), MNIMAPMailboxClass)
#define MN_IS_IMAP_MAILBOX(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), mn_imap_mailbox_get_type ())

#define MN_IMAP_MAILBOX_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), mn_imap_mailbox_get_type(), MNIMAPMailboxClass)

/* Private structure type */
typedef struct _MNIMAPMailboxPrivate MNIMAPMailboxPrivate;

/*
 * Main object structure
 */
#ifndef __TYPEDEF_MN_IMAP_MAILBOX__
#define __TYPEDEF_MN_IMAP_MAILBOX__
typedef struct _MNIMAPMailbox MNIMAPMailbox;
#endif
struct _MNIMAPMailbox {
	MNPIMailbox __parent__;
	/*< public >*/
	char * mailbox;
	MNIMAPMailboxUseIDLE use_idle_extension;
	/*< private >*/
	MNIMAPMailboxPrivate *_priv;
};

/*
 * Class definition
 */
typedef struct _MNIMAPMailboxClass MNIMAPMailboxClass;
struct _MNIMAPMailboxClass {
	MNPIMailboxClass __parent__;
};


/*
 * Public methods
 */
GType	mn_imap_mailbox_get_type	(void) G_GNUC_CONST;
#line 1915 "src/mn-imap-mailbox.gob"
char * 	mn_imap_mailbox_build_name	(const char * username,
					const char * server,
					const char * mailbox);
#line 80 "mn-imap-mailbox.h"

/*
 * Argument wrapping macros
 */
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define MN_IMAP_MAILBOX_PROP_MAILBOX(arg)    	"mailbox", __extension__ ({gchar *z = (arg); z;})
#define MN_IMAP_MAILBOX_GET_PROP_MAILBOX(arg)	"mailbox", __extension__ ({gchar **z = (arg); z;})
#define MN_IMAP_MAILBOX_PROP_USE_IDLE_EXTENSION(arg)    	"use_idle_extension", __extension__ ({gint z = (arg); z;})
#define MN_IMAP_MAILBOX_GET_PROP_USE_IDLE_EXTENSION(arg)	"use_idle_extension", __extension__ ({gint *z = (arg); z;})
#else /* __GNUC__ && !__STRICT_ANSI__ */
#define MN_IMAP_MAILBOX_PROP_MAILBOX(arg)    	"mailbox",(gchar *)(arg)
#define MN_IMAP_MAILBOX_GET_PROP_MAILBOX(arg)	"mailbox",(gchar **)(arg)
#define MN_IMAP_MAILBOX_PROP_USE_IDLE_EXTENSION(arg)    	"use_idle_extension",(gint )(arg)
#define MN_IMAP_MAILBOX_GET_PROP_USE_IDLE_EXTENSION(arg)	"use_idle_extension",(gint *)(arg)
#endif /* __GNUC__ && !__STRICT_ANSI__ */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
