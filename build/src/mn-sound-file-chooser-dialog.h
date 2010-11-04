/* Generated by GOB (v2.0.17)   (do not edit directly) */

#include <glib.h>
#include <glib-object.h>


#include <gtk/gtk.h>

#ifndef __MN_SOUND_FILE_CHOOSER_DIALOG_H__
#define __MN_SOUND_FILE_CHOOSER_DIALOG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Type checking and casting macros
 */
#define MN_TYPE_SOUND_FILE_CHOOSER_DIALOG	(mn_sound_file_chooser_dialog_get_type())
#define MN_SOUND_FILE_CHOOSER_DIALOG(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_sound_file_chooser_dialog_get_type(), MNSoundFileChooserDialog)
#define MN_SOUND_FILE_CHOOSER_DIALOG_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), mn_sound_file_chooser_dialog_get_type(), MNSoundFileChooserDialog const)
#define MN_SOUND_FILE_CHOOSER_DIALOG_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), mn_sound_file_chooser_dialog_get_type(), MNSoundFileChooserDialogClass)
#define MN_IS_SOUND_FILE_CHOOSER_DIALOG(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), mn_sound_file_chooser_dialog_get_type ())

#define MN_SOUND_FILE_CHOOSER_DIALOG_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), mn_sound_file_chooser_dialog_get_type(), MNSoundFileChooserDialogClass)

/* Private structure type */
typedef struct _MNSoundFileChooserDialogPrivate MNSoundFileChooserDialogPrivate;

/*
 * Main object structure
 */
#ifndef __TYPEDEF_MN_SOUND_FILE_CHOOSER_DIALOG__
#define __TYPEDEF_MN_SOUND_FILE_CHOOSER_DIALOG__
typedef struct _MNSoundFileChooserDialog MNSoundFileChooserDialog;
#endif
struct _MNSoundFileChooserDialog {
	GtkFileChooserDialog __parent__;
	/*< private >*/
	MNSoundFileChooserDialogPrivate *_priv;
};

/*
 * Class definition
 */
typedef struct _MNSoundFileChooserDialogClass MNSoundFileChooserDialogClass;
struct _MNSoundFileChooserDialogClass {
	GtkFileChooserDialogClass __parent__;
};


/*
 * Public methods
 */
GType	mn_sound_file_chooser_dialog_get_type	(void) G_GNUC_CONST;
#line 129 "src/mn-sound-file-chooser-dialog.gob"
GtkWidget * 	mn_sound_file_chooser_dialog_new	(void);
#line 60 "mn-sound-file-chooser-dialog.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
