/* Generated by GOB (v2.0.17)   (do not edit directly) */

/* End world hunger, donate to the World Food Programme, http://www.wfp.org */

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 17

#define selfp (self->_priv)

#include <string.h> /* memset() */

#include "mn-vfs-message.h"

#include "mn-vfs-message-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */

#line 26 "src/mn-vfs-message.gob"

#include "mn-vfs-mailbox-backend.h"
#include "mn-message-mime.h"
#include "mn-util.h"

#line 32 "mn-vfs-message.c"
/* self casting macros */
#define SELF(x) MN_VFS_MESSAGE(x)
#define SELF_CONST(x) MN_VFS_MESSAGE_CONST(x)
#define IS_SELF(x) MN_IS_VFS_MESSAGE(x)
#define TYPE_SELF MN_TYPE_VFS_MESSAGE
#define SELF_CLASS(x) MN_VFS_MESSAGE_CLASS(x)

#define SELF_GET_CLASS(x) MN_VFS_MESSAGE_GET_CLASS(x)

/* self typedefs */
typedef MNVFSMessage Self;
typedef MNVFSMessageClass SelfClass;

/* here are local prototypes */
static void ___object_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void ___object_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
#line 0 "src/mn-vfs-message.gob"
static void mn_vfs_message_init (MNVFSMessage * o) G_GNUC_UNUSED;
#line 51 "mn-vfs-message.c"
#line 0 "src/mn-vfs-message.gob"
static void mn_vfs_message_class_init (MNVFSMessageClass * c) G_GNUC_UNUSED;
#line 54 "mn-vfs-message.c"

enum {
	PROP_0,
	PROP_VFS_URI
};

/* pointer to the class of our parent */
static MNMessageClass *parent_class = NULL;

/* Short form macros */
#define self_new mn_vfs_message_new
#define self_new_from_message mn_vfs_message_new_from_message
GType
mn_vfs_message_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (MNVFSMessageClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mn_vfs_message_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (MNVFSMessage),
			0 /* n_preallocs */,
			(GInstanceInitFunc) mn_vfs_message_init,
			NULL
		};

		type = g_type_register_static (MN_TYPE_MESSAGE, "MNVFSMessage", &info, (GTypeFlags)G_TYPE_FLAG_ABSTRACT);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((MNVFSMessage *)g_object_new(mn_vfs_message_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static MNVFSMessage * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static MNVFSMessage *
GET_NEW_VARG (const char *first, ...)
{
	MNVFSMessage *ret;
	va_list ap;
	va_start (ap, first);
	ret = (MNVFSMessage *)g_object_new_valist (mn_vfs_message_get_type (), first, ap);
	va_end (ap);
	return ret;
}


static void
___dispose (GObject *obj_self)
{
#define __GOB_FUNCTION__ "MN:VFS:Message::dispose"
	MNVFSMessage *self G_GNUC_UNUSED = MN_VFS_MESSAGE (obj_self);
	if (G_OBJECT_CLASS (parent_class)->dispose) \
		(* G_OBJECT_CLASS (parent_class)->dispose) (obj_self);
#line 35 "src/mn-vfs-message.gob"
	if(self->vfs_uri) { gnome_vfs_uri_unref ((gpointer) self->vfs_uri); self->vfs_uri = NULL; }
#line 119 "mn-vfs-message.c"
}
#undef __GOB_FUNCTION__

static void 
mn_vfs_message_init (MNVFSMessage * o G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:VFS:Message::init"
}
#undef __GOB_FUNCTION__
static void 
mn_vfs_message_class_init (MNVFSMessageClass * c G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:VFS:Message::class_init"
	GObjectClass *g_object_class G_GNUC_UNUSED = (GObjectClass*) c;

	parent_class = g_type_class_ref (MN_TYPE_MESSAGE);

	g_object_class->dispose = ___dispose;
	g_object_class->get_property = ___object_get_property;
	g_object_class->set_property = ___object_set_property;
    {
	GParamSpec   *param_spec;

	param_spec = g_param_spec_pointer
		("vfs_uri" /* name */,
		 NULL /* nick */,
		 NULL /* blurb */,
		 (GParamFlags)(G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (g_object_class,
		PROP_VFS_URI,
		param_spec);
    }
}
#undef __GOB_FUNCTION__

static void
___object_set_property (GObject *object,
	guint property_id,
	const GValue *VAL G_GNUC_UNUSED,
	GParamSpec *pspec G_GNUC_UNUSED)
#define __GOB_FUNCTION__ "MN:VFS:Message::set_property"
{
	MNVFSMessage *self G_GNUC_UNUSED;

	self = MN_VFS_MESSAGE (object);

	switch (property_id) {
	case PROP_VFS_URI:
		{
#line 38 "src/mn-vfs-message.gob"

      GnomeVFSURI *uri;

      g_assert(self->vfs_uri == NULL);

      uri = g_value_get_pointer(VAL);
      if (uri)
	self->vfs_uri = gnome_vfs_uri_ref(uri);
    
#line 179 "mn-vfs-message.c"
		}
		break;
	default:
/* Apparently in g++ this is needed, glib is b0rk */
#ifndef __PRETTY_FUNCTION__
#  undef G_STRLOC
#  define G_STRLOC	__FILE__ ":" G_STRINGIFY (__LINE__)
#endif
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}
#undef __GOB_FUNCTION__

static void
___object_get_property (GObject *object,
	guint property_id,
	GValue *VAL G_GNUC_UNUSED,
	GParamSpec *pspec G_GNUC_UNUSED)
#define __GOB_FUNCTION__ "MN:VFS:Message::get_property"
{
	MNVFSMessage *self G_GNUC_UNUSED;

	self = MN_VFS_MESSAGE (object);

	switch (property_id) {
	case PROP_VFS_URI:
		{
#line 48 "src/mn-vfs-message.gob"

      g_value_set_pointer(VAL, self->vfs_uri != NULL ? gnome_vfs_uri_ref(self->vfs_uri) : NULL);
    
#line 212 "mn-vfs-message.c"
		}
		break;
	default:
/* Apparently in g++ this is needed, glib is b0rk */
#ifndef __PRETTY_FUNCTION__
#  undef G_STRLOC
#  define G_STRLOC	__FILE__ ":" G_STRINGIFY (__LINE__)
#endif
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}
#undef __GOB_FUNCTION__



#line 52 "src/mn-vfs-message.gob"
MNVFSMessage * 
mn_vfs_message_new (GType type, MNVFSMailboxBackend * backend, const char * mid, GnomeVFSURI * dir_uri, const char * filename, MNMessageFlags flags, gboolean handle_status, GError ** err)
{
#line 233 "mn-vfs-message.c"
#define __GOB_FUNCTION__ "MN:VFS:Message::new"
#line 52 "src/mn-vfs-message.gob"
	g_return_val_if_fail (backend != NULL, (MNVFSMessage * )0);
#line 52 "src/mn-vfs-message.gob"
	g_return_val_if_fail (MN_IS_VFS_MAILBOX_BACKEND (backend), (MNVFSMessage * )0);
#line 52 "src/mn-vfs-message.gob"
	g_return_val_if_fail (dir_uri != NULL, (MNVFSMessage * )0);
#line 52 "src/mn-vfs-message.gob"
	g_return_val_if_fail (filename != NULL, (MNVFSMessage * )0);
#line 243 "mn-vfs-message.c"
{
#line 61 "src/mn-vfs-message.gob"
	
    GnomeVFSURI *message_uri;
    MNVFSMessage *message;

    message_uri = gnome_vfs_uri_append_file_name(dir_uri, filename);

    message = MN_VFS_MESSAGE(mn_message_new_from_uri_full(type,
							  MN_MAILBOX(MN_VFS_MAILBOX_BACKEND(backend)->mailbox),
							  mid,
							  message_uri,
							  flags,
							  handle_status,
							  err));

    if (message)
      message->vfs_uri = gnome_vfs_uri_ref(message_uri);

    gnome_vfs_uri_unref(message_uri);

    return message;
  }}
#line 267 "mn-vfs-message.c"
#undef __GOB_FUNCTION__

#line 83 "src/mn-vfs-message.gob"
MNVFSMessage * 
mn_vfs_message_new_from_message (MNVFSMessage * message, GnomeVFSURI * dir_uri, const char * filename, MNMessageFlags flags)
{
#line 274 "mn-vfs-message.c"
#define __GOB_FUNCTION__ "MN:VFS:Message::new_from_message"
#line 83 "src/mn-vfs-message.gob"
	g_return_val_if_fail (message != NULL, (MNVFSMessage * )0);
#line 83 "src/mn-vfs-message.gob"
	g_return_val_if_fail (MN_IS_VFS_MESSAGE (message), (MNVFSMessage * )0);
#line 83 "src/mn-vfs-message.gob"
	g_return_val_if_fail (dir_uri != NULL, (MNVFSMessage * )0);
#line 83 "src/mn-vfs-message.gob"
	g_return_val_if_fail (filename != NULL, (MNVFSMessage * )0);
#line 284 "mn-vfs-message.c"
{
#line 88 "src/mn-vfs-message.gob"
	
    GnomeVFSURI *message_uri;
    char *message_text_uri;
    MNVFSMessage *new_message;

    message_uri = gnome_vfs_uri_append_file_name(dir_uri, filename);
    message_text_uri = gnome_vfs_uri_to_string(message_uri, GNOME_VFS_URI_HIDE_NONE);

    new_message = mn_g_object_clone(message,
				    MN_MESSAGE_PROP_URI((char *) message_text_uri),
				    MN_MESSAGE_PROP_FLAGS(flags),
				    MN_VFS_MESSAGE_PROP_VFS_URI(message_uri),
				    NULL);

    gnome_vfs_uri_unref(message_uri);
    g_free(message_text_uri);

    return new_message;
  }}
#line 306 "mn-vfs-message.c"
#undef __GOB_FUNCTION__
