/* Generated by GOB (v2.0.11) on Sun Mar  6 22:34:26 2005
   (do not edit directly) */

/* End world hunger, donate to the World Food Programme, http://www.wfp.org */

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 11

#define selfp (self->_priv)

#include <string.h> /* memset() */

#include "mn-maildir-mailbox.h"

#include "mn-maildir-mailbox-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */

#line 24 "mn-maildir-mailbox.gob"

#include "config.h"
#include <glib/gi18n.h>
#include "mn-mailbox-private.h"
#include "mn-vfs.h"
#include "mn-util.h"
#include "mn-message-mime.h"

#line 36 "mn-maildir-mailbox.c"
/* self casting macros */
#define SELF(x) MN_MAILDIR_MAILBOX(x)
#define SELF_CONST(x) MN_MAILDIR_MAILBOX_CONST(x)
#define IS_SELF(x) MN_IS_MAILDIR_MAILBOX(x)
#define TYPE_SELF MN_TYPE_MAILDIR_MAILBOX
#define SELF_CLASS(x) MN_MAILDIR_MAILBOX_CLASS(x)

#define SELF_GET_CLASS(x) MN_MAILDIR_MAILBOX_GET_CLASS(x)

/* self typedefs */
typedef MNMaildirMailbox Self;
typedef MNMaildirMailboxClass SelfClass;

/* here are local prototypes */
static void mn_maildir_mailbox_init (MNMaildirMailbox * o) G_GNUC_UNUSED;
static void mn_maildir_mailbox_class_init (MNMaildirMailboxClass * class) G_GNUC_UNUSED;
static GObject * ___2_mn_maildir_mailbox_constructor (GType type, unsigned int n_construct_properties, GObjectConstructParam * construct_params) G_GNUC_UNUSED;
static gboolean ___3_mn_maildir_mailbox_impl_is (MNMailbox * dummy, MNURI * uri) G_GNUC_UNUSED;
static void ___4_mn_maildir_mailbox_impl_check (MNMailbox * self) G_GNUC_UNUSED;

/* pointer to the class of our parent */
static MNMailboxClass *parent_class = NULL;

GType
mn_maildir_mailbox_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (MNMaildirMailboxClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mn_maildir_mailbox_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (MNMaildirMailbox),
			0 /* n_preallocs */,
			(GInstanceInitFunc) mn_maildir_mailbox_init,
			NULL
		};

		type = g_type_register_static (MN_TYPE_MAILBOX, "MNMaildirMailbox", &info, (GTypeFlags)0);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((MNMaildirMailbox *)g_object_new(mn_maildir_mailbox_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static MNMaildirMailbox * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static MNMaildirMailbox *
GET_NEW_VARG (const char *first, ...)
{
	MNMaildirMailbox *ret;
	va_list ap;
	va_start (ap, first);
	ret = (MNMaildirMailbox *)g_object_new_valist (mn_maildir_mailbox_get_type (), first, ap);
	va_end (ap);
	return ret;
}

static void 
mn_maildir_mailbox_init (MNMaildirMailbox * o G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:Maildir:Mailbox::init"
}
#undef __GOB_FUNCTION__
#line 35 "mn-maildir-mailbox.gob"
static void 
mn_maildir_mailbox_class_init (MNMaildirMailboxClass * class G_GNUC_UNUSED)
#line 111 "mn-maildir-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Maildir:Mailbox::class_init"
	GObjectClass *g_object_class = (GObjectClass *)class;
	MNMailboxClass *mn_mailbox_class = (MNMailboxClass *)class;

	parent_class = g_type_class_ref (MN_TYPE_MAILBOX);

#line 40 "mn-maildir-mailbox.gob"
	g_object_class->constructor = ___2_mn_maildir_mailbox_constructor;
#line 69 "mn-maildir-mailbox.gob"
	mn_mailbox_class->impl_is = ___3_mn_maildir_mailbox_impl_is;
#line 95 "mn-maildir-mailbox.gob"
	mn_mailbox_class->impl_check = ___4_mn_maildir_mailbox_impl_check;
#line 125 "mn-maildir-mailbox.c"
 {
#line 36 "mn-maildir-mailbox.gob"

    MN_MAILBOX_CLASS(class)->format = "Maildir";
  
#line 131 "mn-maildir-mailbox.c"
 }
}
#undef __GOB_FUNCTION__



#line 40 "mn-maildir-mailbox.gob"
static GObject * 
___2_mn_maildir_mailbox_constructor (GType type G_GNUC_UNUSED, unsigned int n_construct_properties, GObjectConstructParam * construct_params)
#line 141 "mn-maildir-mailbox.c"
#define PARENT_HANDLER(___type,___n_construct_properties,___construct_params) \
	((G_OBJECT_CLASS(parent_class)->constructor)? \
		(* G_OBJECT_CLASS(parent_class)->constructor)(___type,___n_construct_properties,___construct_params): \
		((GObject * )0))
{
#define __GOB_FUNCTION__ "MN:Maildir:Mailbox::constructor"
{
#line 42 "mn-maildir-mailbox.gob"
	
    GObject *object;
    MNMailbox *self;
    char *new_uri;
  
    object = PARENT_HANDLER(type, n_construct_properties, construct_params);
    self = MN_MAILBOX(object);

    new_uri = g_build_path("/", self->uri->text, "new", NULL);

    GDK_THREADS_ENTER();

    mn_mailbox_monitor(self,
		       new_uri,
		       GNOME_VFS_MONITOR_DIRECTORY,
		       MN_MAILBOX_MONITOR_EVENT_DELETED
		       | MN_MAILBOX_MONITOR_EVENT_CHANGED
		       | MN_MAILBOX_MONITOR_EVENT_CREATED);

    gdk_flush();
    GDK_THREADS_LEAVE();

    g_free(new_uri);

    return object;
  }}
#line 176 "mn-maildir-mailbox.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

#line 69 "mn-maildir-mailbox.gob"
static gboolean 
___3_mn_maildir_mailbox_impl_is (MNMailbox * dummy G_GNUC_UNUSED, MNURI * uri)
#line 183 "mn-maildir-mailbox.c"
#define PARENT_HANDLER(___dummy,___uri) \
	((MN_MAILBOX_CLASS(parent_class)->impl_is)? \
		(* MN_MAILBOX_CLASS(parent_class)->impl_is)(___dummy,___uri): \
		((gboolean )0))
{
#define __GOB_FUNCTION__ "MN:Maildir:Mailbox::impl_is"
#line 69 "mn-maildir-mailbox.gob"
	g_return_val_if_fail (uri != NULL, (gboolean )0);
#line 192 "mn-maildir-mailbox.c"
{
#line 71 "mn-maildir-mailbox.gob"
	
    gboolean is = FALSE;

    if (uri->vfs)
      {
	int i; 
	const char *constitutive_dirs[] = { "cur", "new", "tmp" };

	for (i = 0; i < G_N_ELEMENTS(constitutive_dirs); i++)
	  {
	    GnomeVFSURI *dir_uri;
	
	    dir_uri = gnome_vfs_uri_append_path(uri->vfs, constitutive_dirs[i]);
	    is = mn_vfs_test(dir_uri, G_FILE_TEST_IS_DIR);
	    gnome_vfs_uri_unref(dir_uri);
	
	    if (! is)
	      break;
	  }
      }
  
    return is;
  }}
#line 218 "mn-maildir-mailbox.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

#line 95 "mn-maildir-mailbox.gob"
static void 
___4_mn_maildir_mailbox_impl_check (MNMailbox * self G_GNUC_UNUSED)
#line 225 "mn-maildir-mailbox.c"
#define PARENT_HANDLER(___self) \
	{ if(MN_MAILBOX_CLASS(parent_class)->impl_check) \
		(* MN_MAILBOX_CLASS(parent_class)->impl_check)(___self); }
{
#define __GOB_FUNCTION__ "MN:Maildir:Mailbox::impl_check"
#line 95 "mn-maildir-mailbox.gob"
	g_return_if_fail (self != NULL);
#line 95 "mn-maildir-mailbox.gob"
	g_return_if_fail (MN_IS_MAILBOX (self));
#line 235 "mn-maildir-mailbox.c"
{
#line 97 "mn-maildir-mailbox.gob"
	
    GnomeVFSURI *new_uri;
    GnomeVFSResult result;
    GnomeVFSResult close_result;
    GnomeVFSDirectoryHandle *handle;
    GnomeVFSFileInfo *file_info;
    GSList *messages = NULL;
    
    new_uri = gnome_vfs_uri_append_path(self->uri->vfs, "new");
    result = gnome_vfs_directory_open_from_uri(&handle, new_uri, GNOME_VFS_FILE_INFO_FOLLOW_LINKS);

    if (result != GNOME_VFS_OK)
      {
	GDK_THREADS_ENTER();

	mn_mailbox_set_error(self, _("unable to open folder \"new\": %s"), gnome_vfs_result_to_string(result));
	mn_mailbox_end_check(self);

	gdk_flush();
	GDK_THREADS_LEAVE();

	goto end;
      }
  
    file_info = gnome_vfs_file_info_new();
    while ((result = gnome_vfs_directory_read_next(handle, file_info)) == GNOME_VFS_OK)
      if (file_info->name[0] != '.')
	{
	  GnomeVFSURI *message_uri;

	  message_uri = gnome_vfs_uri_append_file_name(new_uri, file_info->name);
	  messages = g_slist_append(messages, mn_message_new_from_uri(self->uri, message_uri));
	  gnome_vfs_uri_unref(message_uri);
	}
    gnome_vfs_file_info_unref(file_info);
    close_result = gnome_vfs_directory_close(handle);

    GDK_THREADS_ENTER();

    if (result == GNOME_VFS_OK || result == GNOME_VFS_ERROR_EOF)
      {
	if (close_result == GNOME_VFS_OK)
	  mn_mailbox_set_messages(self, messages);
	else
	  mn_mailbox_set_error(self, _("unable to close folder \"new\": %s"), gnome_vfs_result_to_string(close_result));
      }
    else
      mn_mailbox_set_error(self, _("error while reading folder \"new\": %s"), gnome_vfs_result_to_string(result));
    mn_mailbox_end_check(self);

    gdk_flush();
    GDK_THREADS_LEAVE();

    mn_g_object_slist_free(messages);

  end:
    gnome_vfs_uri_unref(new_uri);
  }}
#line 296 "mn-maildir-mailbox.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER
