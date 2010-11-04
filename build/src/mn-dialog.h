/* Generated by GOB (v2.0.17)   (do not edit directly) */

#include <glib.h>
#include <glib-object.h>


#include <gtk/gtk.h>

#ifndef __MN_DIALOG_H__
#define __MN_DIALOG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Type checking and casting macros
 */
#define MN_TYPE_DIALOG	(mn_dialog_get_type())
#define MN_DIALOG(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_dialog_get_type(), MNDialog)
#define MN_DIALOG_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_dialog_get_type(), MNDialog const)
#define MN_DIALOG_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), mn_dialog_get_type(), MNDialogClass)
#define MN_IS_DIALOG(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), mn_dialog_get_type ())

#define MN_DIALOG_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), mn_dialog_get_type(), MNDialogClass)

/*
 * Main object structure
 */
#ifndef __TYPEDEF_MN_DIALOG__
#define __TYPEDEF_MN_DIALOG__
typedef struct _MNDialog MNDialog;
#endif
struct _MNDialog {
	GtkDialog __parent__;
};

/*
 * Class definition
 */
typedef struct _MNDialogClass MNDialogClass;
struct _MNDialogClass {
	GtkDialogClass __parent__;
};


/*
 * Public methods
 */
GType	mn_dialog_get_type	(void) G_GNUC_CONST;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
