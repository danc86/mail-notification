/* Generated by GOB (v2.0.9)   (do not edit directly) */

#include <glib.h>
#include <glib-object.h>


#include "mn-mailbox.h"

#ifndef __MN_PENDING_MAILBOX_H__
#define __MN_PENDING_MAILBOX_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Type checking and casting macros
 */
#define MN_TYPE_PENDING_MAILBOX	(mn_pending_mailbox_get_type())
#define MN_PENDING_MAILBOX(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_pending_mailbox_get_type(), MNPendingMailbox)
#define MN_PENDING_MAILBOX_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_pending_mailbox_get_type(), MNPendingMailbox const)
#define MN_PENDING_MAILBOX_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), mn_pending_mailbox_get_type(), MNPendingMailboxClass)
#define MN_IS_PENDING_MAILBOX(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), mn_pending_mailbox_get_type ())

#define MN_PENDING_MAILBOX_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), mn_pending_mailbox_get_type(), MNPendingMailboxClass)

/*
 * Main object structure
 */
#ifndef __TYPEDEF_MN_PENDING_MAILBOX__
#define __TYPEDEF_MN_PENDING_MAILBOX__
typedef struct _MNPendingMailbox MNPendingMailbox;
#endif
struct _MNPendingMailbox {
	MNMailbox __parent__;
};

/*
 * Class definition
 */
typedef struct _MNPendingMailboxClass MNPendingMailboxClass;
struct _MNPendingMailboxClass {
	MNMailboxClass __parent__;
};


/*
 * Public methods
 */
GType	mn_pending_mailbox_get_type	(void);
MNMailbox * 	mn_pending_mailbox_new	(MNURI * uri);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
