/* Generated by GOB (v2.0.9) on Thu Aug 19 00:41:30 2004
   (do not edit directly) */

/* End world hunger, donate to the World Food Programme, http://www.wfp.org */

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 9

#define selfp (self->_priv)

#include "mn-mh-mailbox.h"

#include "mn-mh-mailbox-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */

#line 22 "mn-mh-mailbox.gob"

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include <gdk/gdk.h>
#include "mn-mailbox-private.h"
#include "mn-vfs.h"

#line 35 "mn-mh-mailbox.c"
/* self casting macros */
#define SELF(x) MN_MH_MAILBOX(x)
#define SELF_CONST(x) MN_MH_MAILBOX_CONST(x)
#define IS_SELF(x) MN_IS_MH_MAILBOX(x)
#define TYPE_SELF MN_TYPE_MH_MAILBOX
#define SELF_CLASS(x) MN_MH_MAILBOX_CLASS(x)

#define SELF_GET_CLASS(x) MN_MH_MAILBOX_GET_CLASS(x)

/* self typedefs */
typedef MNMHMailbox Self;
typedef MNMHMailboxClass SelfClass;

/* here are local prototypes */
static void mn_mh_mailbox_init (MNMHMailbox * o) G_GNUC_UNUSED;
static void mn_mh_mailbox_class_init (MNMHMailboxClass * class) G_GNUC_UNUSED;
static GObject * ___2_mn_mh_mailbox_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_params) G_GNUC_UNUSED;
static gboolean ___3_mn_mh_mailbox_impl_is (MNMailbox * dummy, const char * uri) G_GNUC_UNUSED;
static void ___4_mn_mh_mailbox_impl_threaded_check (MNMailbox * self) G_GNUC_UNUSED;

/* pointer to the class of our parent */
static MNMailboxClass *parent_class = NULL;

GType
mn_mh_mailbox_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (MNMHMailboxClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mn_mh_mailbox_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (MNMHMailbox),
			0 /* n_preallocs */,
			(GInstanceInitFunc) mn_mh_mailbox_init,
			NULL
		};

		type = g_type_register_static (MN_TYPE_MAILBOX, "MNMHMailbox", &info, (GTypeFlags)0);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((MNMHMailbox *)g_object_new(mn_mh_mailbox_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static MNMHMailbox * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static MNMHMailbox *
GET_NEW_VARG (const char *first, ...)
{
	MNMHMailbox *ret;
	va_list ap;
	va_start (ap, first);
	ret = (MNMHMailbox *)g_object_new_valist (mn_mh_mailbox_get_type (), first, ap);
	va_end (ap);
	return ret;
}

static void 
mn_mh_mailbox_init (MNMHMailbox * o G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:MH:Mailbox::init"
}
#undef __GOB_FUNCTION__
#line 34 "mn-mh-mailbox.gob"
static void 
mn_mh_mailbox_class_init (MNMHMailboxClass * class G_GNUC_UNUSED)
#line 110 "mn-mh-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:MH:Mailbox::class_init"
	GObjectClass *g_object_class = (GObjectClass *)class;
	MNMailboxClass *mn_mailbox_class = (MNMailboxClass *)class;

	parent_class = g_type_class_ref (MN_TYPE_MAILBOX);

#line 39 "mn-mh-mailbox.gob"
	g_object_class->constructor = ___2_mn_mh_mailbox_constructor;
#line 61 "mn-mh-mailbox.gob"
	mn_mailbox_class->impl_is = ___3_mn_mh_mailbox_impl_is;
#line 74 "mn-mh-mailbox.gob"
	mn_mailbox_class->impl_threaded_check = ___4_mn_mh_mailbox_impl_threaded_check;
#line 124 "mn-mh-mailbox.c"
 {
#line 35 "mn-mh-mailbox.gob"

    MN_MAILBOX_CLASS(class)->format = "MH";
  
#line 130 "mn-mh-mailbox.c"
 }
}
#undef __GOB_FUNCTION__



#line 39 "mn-mh-mailbox.gob"
static GObject * 
___2_mn_mh_mailbox_constructor (GType type G_GNUC_UNUSED, guint n_construct_properties, GObjectConstructParam * construct_params)
#line 140 "mn-mh-mailbox.c"
#define PARENT_HANDLER(___type,___n_construct_properties,___construct_params) \
	((G_OBJECT_CLASS(parent_class)->constructor)? \
		(* G_OBJECT_CLASS(parent_class)->constructor)(___type,___n_construct_properties,___construct_params): \
		((GObject * )0))
{
#define __GOB_FUNCTION__ "MN:MH:Mailbox::constructor"
{
#line 41 "mn-mh-mailbox.gob"
	
    GObject *object;
    MNMailbox *self;
    char *sequences_uri;

    object = PARENT_HANDLER(type, n_construct_properties, construct_params);
    self = MN_MAILBOX(object);

    sequences_uri = g_build_path("/", mn_mailbox_get_uri(self), ".mh_sequences", NULL);
    mn_mailbox_monitor(self,
		       sequences_uri,
		       GNOME_VFS_MONITOR_FILE,
		       MN_MAILBOX_MONITOR_EVENT_CHANGED
		       | MN_MAILBOX_MONITOR_EVENT_DELETED
		       | MN_MAILBOX_MONITOR_EVENT_CREATED);
    g_free(sequences_uri);

    return object;
  }}
#line 168 "mn-mh-mailbox.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

#line 61 "mn-mh-mailbox.gob"
static gboolean 
___3_mn_mh_mailbox_impl_is (MNMailbox * dummy G_GNUC_UNUSED, const char * uri)
#line 175 "mn-mh-mailbox.c"
#define PARENT_HANDLER(___dummy,___uri) \
	((MN_MAILBOX_CLASS(parent_class)->impl_is)? \
		(* MN_MAILBOX_CLASS(parent_class)->impl_is)(___dummy,___uri): \
		((gboolean )0))
{
#define __GOB_FUNCTION__ "MN:MH:Mailbox::impl_is"
#line 61 "mn-mh-mailbox.gob"
	g_return_val_if_fail (uri != NULL, (gboolean )0);
#line 184 "mn-mh-mailbox.c"
{
#line 63 "mn-mh-mailbox.gob"
	
    char *sequences_uri;
    gboolean is;

    sequences_uri = g_build_path("/", uri, ".mh_sequences", NULL);
    is = mn_vfs_test(sequences_uri, G_FILE_TEST_IS_REGULAR);
    g_free(sequences_uri);
    
    return is;
  }}
#line 197 "mn-mh-mailbox.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

#line 74 "mn-mh-mailbox.gob"
static void 
___4_mn_mh_mailbox_impl_threaded_check (MNMailbox * self G_GNUC_UNUSED)
#line 204 "mn-mh-mailbox.c"
#define PARENT_HANDLER(___self) \
	{ if(MN_MAILBOX_CLASS(parent_class)->impl_threaded_check) \
		(* MN_MAILBOX_CLASS(parent_class)->impl_threaded_check)(___self); }
{
#define __GOB_FUNCTION__ "MN:MH:Mailbox::impl_threaded_check"
#line 74 "mn-mh-mailbox.gob"
	g_return_if_fail (self != NULL);
#line 74 "mn-mh-mailbox.gob"
	g_return_if_fail (MN_IS_MAILBOX (self));
#line 214 "mn-mh-mailbox.c"
{
#line 76 "mn-mh-mailbox.gob"
	
    char *sequences_uri;
    GnomeVFSResult result;
    GnomeVFSHandle *handle;
  
    sequences_uri = g_build_path("/", mn_mailbox_get_uri(self), ".mh_sequences", NULL);
    result = gnome_vfs_open(&handle, sequences_uri, GNOME_VFS_OPEN_READ);
    g_free(sequences_uri);

    if (result == GNOME_VFS_OK)
      {
	MNVFSReadLineContext *context = NULL;
	const char *line;
	gboolean has_new = FALSE;
	
	while ((result = mn_vfs_read_line(&context, handle, &line)) == GNOME_VFS_OK)
	  if (! strncmp(line, "unseen", 6))
	    {
	      int first;
	      
	      if (sscanf(line, "unseen: %d", &first) == 1)
		{
		  has_new = TRUE;
		  break;
		}
	    }

	mn_vfs_read_line_context_free(context);
	gnome_vfs_close(handle);

	GDK_THREADS_ENTER();
      
	if (result == GNOME_VFS_OK || result == GNOME_VFS_ERROR_EOF)
	  mn_mailbox_set_has_new(self, has_new);
	else
	  mn_mailbox_set_error(self, _("error while reading .mh_sequences: %s"), gnome_vfs_result_to_string(result));
	
	gdk_flush();
	GDK_THREADS_LEAVE();
      }
    else
      {
	GDK_THREADS_ENTER();
	mn_mailbox_set_error(self, _("unable to open .mh_sequences: %s"), gnome_vfs_result_to_string(result));
	gdk_flush();
	GDK_THREADS_LEAVE();
      }

    GDK_THREADS_ENTER();
    mn_mailbox_end_check(self);
    gdk_flush();
    GDK_THREADS_LEAVE();
  }}
#line 270 "mn-mh-mailbox.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER
