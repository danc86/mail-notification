/* Generated by GOB (v2.0.11)   (do not edit directly) */

#ifndef __MN_SHELL_PRIVATE_H__
#define __MN_SHELL_PRIVATE_H__

#include "mn-shell.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#line 26 "mn-shell.gob"

#include "mn-mail-icon.h"

#line 18 "mn-shell-private.h"
struct _MNShellPrivate {
#line 50 "mn-shell.gob"
	MNMailIcon * icon;
#line 51 "mn-shell.gob"
	GtkWidget * about;
#line 52 "mn-shell.gob"
	gboolean has_new;
#line 53 "mn-shell.gob"
	GSList * mailbox_properties_dialogs;
#line 28 "mn-shell-private.h"
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
