/* Generated by GOB (v2.0.17)   (do not edit directly) */

#include <glib.h>
#include <glib-object.h>


#include "mn-message-view.h"

#ifndef __MN_STANDARD_MESSAGE_VIEW_H__
#define __MN_STANDARD_MESSAGE_VIEW_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Type checking and casting macros
 */
#define MN_TYPE_STANDARD_MESSAGE_VIEW	(mn_standard_message_view_get_type())
#define MN_STANDARD_MESSAGE_VIEW(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_standard_message_view_get_type(), MNStandardMessageView)
#define MN_STANDARD_MESSAGE_VIEW_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_standard_message_view_get_type(), MNStandardMessageView const)
#define MN_STANDARD_MESSAGE_VIEW_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), mn_standard_message_view_get_type(), MNStandardMessageViewClass)
#define MN_IS_STANDARD_MESSAGE_VIEW(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), mn_standard_message_view_get_type ())

#define MN_STANDARD_MESSAGE_VIEW_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), mn_standard_message_view_get_type(), MNStandardMessageViewClass)

/*
 * Main object structure
 */
#ifndef __TYPEDEF_MN_STANDARD_MESSAGE_VIEW__
#define __TYPEDEF_MN_STANDARD_MESSAGE_VIEW__
typedef struct _MNStandardMessageView MNStandardMessageView;
#endif
struct _MNStandardMessageView {
	MNMessageView __parent__;
};

/*
 * Class definition
 */
typedef struct _MNStandardMessageViewClass MNStandardMessageViewClass;
struct _MNStandardMessageViewClass {
	MNMessageViewClass __parent__;
};


/*
 * Public methods
 */
GType	mn_standard_message_view_get_type	(void) G_GNUC_CONST;
#line 78 "src/mn-standard-message-view.gob"
GtkWidget * 	mn_standard_message_view_new	(void);
#line 55 "mn-standard-message-view.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
