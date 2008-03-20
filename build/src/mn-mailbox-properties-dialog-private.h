/* Generated by GOB (v2.0.15)   (do not edit directly) */

#ifndef __MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE_H__
#define __MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE_H__

#include "mn-mailbox-properties-dialog.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#line 25 "src/mn-mailbox-properties-dialog.gob"

#include "mn-mailbox-properties.h"

#line 18 "mn-mailbox-properties-dialog-private.h"
struct _MNMailboxPropertiesDialogPrivate {
#line 80 "src/mn-mailbox-properties-dialog.gob"
	GtkWindow * dialog_parent;
#line 83 "src/mn-mailbox-properties-dialog.gob"
	MNMailbox * mailbox;
#line 116 "src/mn-mailbox-properties-dialog.gob"
	gboolean apply_used;
#line 119 "src/mn-mailbox-properties-dialog.gob"
	MNMailboxConfiguration * orig_mailbox_configuration;
#line 123 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * type_label;
#line 124 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * type_combo;
#line 125 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * name_default_radio;
#line 126 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * name_default_label;
#line 127 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * name_other_radio;
#line 128 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * name_entry;
#line 129 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * delay_vbox;
#line 130 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * delay_default_radio;
#line 131 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * delay_default_label;
#line 132 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * delay_other_radio;
#line 133 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * delay_scale;
#line 135 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * apply_button;
#line 136 "src/mn-mailbox-properties-dialog.gob"
	GtkWidget * accept_button;
#line 138 "src/mn-mailbox-properties-dialog.gob"
	GtkListStore * store;
#line 140 "src/mn-mailbox-properties-dialog.gob"
	MNMailboxProperties * active_properties;
#line 142 "src/mn-mailbox-properties-dialog.gob"
	GtkSizeGroup * details_size_group;
#line 60 "mn-mailbox-properties-dialog-private.h"
};
#line 626 "src/mn-mailbox-properties-dialog.gob"
void 	mn_mailbox_properties_dialog_set_help_section	(GtkWidget * page, const char * section);
#line 64 "mn-mailbox-properties-dialog-private.h"
#line 643 "src/mn-mailbox-properties-dialog.gob"
void 	mn_mailbox_properties_dialog_type_changed_h	(MNMailboxPropertiesDialog * self, GtkComboBox * combobox);
#line 67 "mn-mailbox-properties-dialog-private.h"
#line 666 "src/mn-mailbox-properties-dialog.gob"
void 	mn_mailbox_properties_dialog_name_toggled_h	(MNMailboxPropertiesDialog * self, GtkToggleButton * button);
#line 70 "mn-mailbox-properties-dialog-private.h"
#line 672 "src/mn-mailbox-properties-dialog.gob"
void 	mn_mailbox_properties_dialog_delay_toggled_h	(MNMailboxPropertiesDialog * self, GtkToggleButton * button);
#line 73 "mn-mailbox-properties-dialog-private.h"
#line 678 "src/mn-mailbox-properties-dialog.gob"
char * 	mn_mailbox_properties_dialog_delay_format_value_h	(MNMailboxPropertiesDialog * self, double arg, GtkScale * scale);
#line 76 "mn-mailbox-properties-dialog-private.h"
#line 684 "src/mn-mailbox-properties-dialog.gob"
void 	mn_mailbox_properties_dialog_entry_activate_h	(MNMailboxPropertiesDialog * self, GtkEntry * entry);
#line 79 "mn-mailbox-properties-dialog-private.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif