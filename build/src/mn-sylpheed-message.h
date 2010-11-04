/* Generated by GOB (v2.0.17)   (do not edit directly) */

#include <glib.h>
#include <glib-object.h>


#include "mn-vfs-message.h"

#ifndef __MN_SYLPHEED_MESSAGE_H__
#define __MN_SYLPHEED_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Type checking and casting macros
 */
#define MN_TYPE_SYLPHEED_MESSAGE	(mn_sylpheed_message_get_type())
#define MN_SYLPHEED_MESSAGE(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_sylpheed_message_get_type(), MNSylpheedMessage)
#define MN_SYLPHEED_MESSAGE_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_sylpheed_message_get_type(), MNSylpheedMessage const)
#define MN_SYLPHEED_MESSAGE_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), mn_sylpheed_message_get_type(), MNSylpheedMessageClass)
#define MN_IS_SYLPHEED_MESSAGE(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), mn_sylpheed_message_get_type ())

#define MN_SYLPHEED_MESSAGE_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), mn_sylpheed_message_get_type(), MNSylpheedMessageClass)

/*
 * Main object structure
 */
#ifndef __TYPEDEF_MN_SYLPHEED_MESSAGE__
#define __TYPEDEF_MN_SYLPHEED_MESSAGE__
typedef struct _MNSylpheedMessage MNSylpheedMessage;
#endif
struct _MNSylpheedMessage {
	MNVFSMessage __parent__;
};

/*
 * Class definition
 */
typedef struct _MNSylpheedMessageClass MNSylpheedMessageClass;
struct _MNSylpheedMessageClass {
	MNVFSMessageClass __parent__;
};


/*
 * Public methods
 */
GType	mn_sylpheed_message_get_type	(void) G_GNUC_CONST;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
