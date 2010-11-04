/* Generated by GOB (v2.0.17)   (do not edit directly) */

/* End world hunger, donate to the World Food Programme, http://www.wfp.org */

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 17

#define selfp (self->_priv)

#include <string.h> /* memset() */

#include "mn-sound-player.h"

#include "mn-sound-player-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */

#line 24 "src/mn-sound-player.gob"

#include <sys/types.h>
#include <signal.h>
#include <glib/gi18n.h>
#include <gnome.h>
#include "mn-conf.h"
#include "mn-locked-callback.h"
#include "mn-util.h"

#line 36 "mn-sound-player.c"
/* self casting macros */
#define SELF(x) MN_SOUND_PLAYER(x)
#define SELF_CONST(x) MN_SOUND_PLAYER_CONST(x)
#define IS_SELF(x) MN_IS_SOUND_PLAYER(x)
#define TYPE_SELF MN_TYPE_SOUND_PLAYER
#define SELF_CLASS(x) MN_SOUND_PLAYER_CLASS(x)

#define SELF_GET_CLASS(x) MN_SOUND_PLAYER_GET_CLASS(x)

/* self typedefs */
typedef MNSoundPlayer Self;
typedef MNSoundPlayerClass SelfClass;

/* here are local prototypes */
static void ___object_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
#line 0 "src/mn-sound-player.gob"
static void mn_sound_player_class_init (MNSoundPlayerClass * c) G_GNUC_UNUSED;
#line 54 "mn-sound-player.c"
#line 52 "src/mn-sound-player.gob"
static void mn_sound_player_init (MNSoundPlayer * self) G_GNUC_UNUSED;
#line 57 "mn-sound-player.c"
#line 62 "src/mn-sound-player.gob"
static void mn_sound_player_dispose (MNSoundPlayer * self) G_GNUC_UNUSED;
#line 60 "mn-sound-player.c"
#line 68 "src/mn-sound-player.gob"
static void mn_sound_player_notify_play_command_cb (GConfClient * client, unsigned int cnxn_id, GConfEntry * entry, gpointer user_data) G_GNUC_UNUSED;
#line 63 "mn-sound-player.c"
#line 85 "src/mn-sound-player.gob"
static gboolean mn_sound_player_can_play (MNSoundPlayer * self) G_GNUC_UNUSED;
#line 66 "mn-sound-player.c"
#line 182 "src/mn-sound-player.gob"
static gboolean mn_sound_player_subst_cb (const char * name, char ** value, gpointer data) G_GNUC_UNUSED;
#line 69 "mn-sound-player.c"
#line 202 "src/mn-sound-player.gob"
static void mn_sound_player_stop_real (MNSoundPlayer * self, gboolean notify_can_stop) G_GNUC_UNUSED;
#line 72 "mn-sound-player.c"
#line 225 "src/mn-sound-player.gob"
static void mn_sound_player_watch_abandoned_cb (GPid pid, int status, gpointer data) G_GNUC_UNUSED;
#line 75 "mn-sound-player.c"
#line 231 "src/mn-sound-player.gob"
static void mn_sound_player_watch_cb (GPid pid, int status, gpointer data) G_GNUC_UNUSED;
#line 78 "mn-sound-player.c"

enum {
	PROP_0,
	PROP_CAN_PLAY,
	PROP_CAN_STOP
};

/* pointer to the class of our parent */
static GObjectClass *parent_class = NULL;

/* Short form macros */
#define self_get_can_play mn_sound_player_get_can_play
#define self_get_can_stop mn_sound_player_get_can_stop
#define self_notify_play_command_cb mn_sound_player_notify_play_command_cb
#define self_can_play mn_sound_player_can_play
#define self_play mn_sound_player_play
#define self_subst_cb mn_sound_player_subst_cb
#define self_stop mn_sound_player_stop
#define self_stop_real mn_sound_player_stop_real
#define self_watch_abandoned_cb mn_sound_player_watch_abandoned_cb
#define self_watch_cb mn_sound_player_watch_cb
#define self_new mn_sound_player_new
GType
mn_sound_player_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (MNSoundPlayerClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mn_sound_player_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (MNSoundPlayer),
			0 /* n_preallocs */,
			(GInstanceInitFunc) mn_sound_player_init,
			NULL
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MNSoundPlayer", &info, (GTypeFlags)0);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((MNSoundPlayer *)g_object_new(mn_sound_player_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static MNSoundPlayer * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static MNSoundPlayer *
GET_NEW_VARG (const char *first, ...)
{
	MNSoundPlayer *ret;
	va_list ap;
	va_start (ap, first);
	ret = (MNSoundPlayer *)g_object_new_valist (mn_sound_player_get_type (), first, ap);
	va_end (ap);
	return ret;
}


static void
___dispose (GObject *obj_self)
{
#define __GOB_FUNCTION__ "MN:Sound:Player::dispose"
	MNSoundPlayer *self G_GNUC_UNUSED = MN_SOUND_PLAYER (obj_self);
#line 62 "src/mn-sound-player.gob"
	mn_sound_player_dispose (self);
#line 151 "mn-sound-player.c"
	if (G_OBJECT_CLASS (parent_class)->dispose) \
		(* G_OBJECT_CLASS (parent_class)->dispose) (obj_self);
}
#undef __GOB_FUNCTION__


static void
___finalize(GObject *obj_self)
{
#define __GOB_FUNCTION__ "MN:Sound:Player::finalize"
	MNSoundPlayer *self G_GNUC_UNUSED = MN_SOUND_PLAYER (obj_self);
	gpointer priv G_GNUC_UNUSED = self->_priv;
	if(G_OBJECT_CLASS(parent_class)->finalize) \
		(* G_OBJECT_CLASS(parent_class)->finalize)(obj_self);
}
#undef __GOB_FUNCTION__

static void 
mn_sound_player_class_init (MNSoundPlayerClass * c G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:Sound:Player::class_init"
	GObjectClass *g_object_class G_GNUC_UNUSED = (GObjectClass*) c;

	g_type_class_add_private(c,sizeof(MNSoundPlayerPrivate));

	parent_class = g_type_class_ref (G_TYPE_OBJECT);

	g_object_class->dispose = ___dispose;
	g_object_class->finalize = ___finalize;
	g_object_class->get_property = ___object_get_property;
    {
	GParamSpec   *param_spec;

	param_spec = g_param_spec_boolean
		("can_play" /* name */,
		 NULL /* nick */,
		 NULL /* blurb */,
		 FALSE /* default_value */,
		 (GParamFlags)(G_PARAM_READABLE));
	g_object_class_install_property (g_object_class,
		PROP_CAN_PLAY,
		param_spec);
	param_spec = g_param_spec_boolean
		("can_stop" /* name */,
		 NULL /* nick */,
		 NULL /* blurb */,
		 FALSE /* default_value */,
		 (GParamFlags)(G_PARAM_READABLE));
	g_object_class_install_property (g_object_class,
		PROP_CAN_STOP,
		param_spec);
    }
}
#undef __GOB_FUNCTION__
#line 52 "src/mn-sound-player.gob"
static void 
mn_sound_player_init (MNSoundPlayer * self G_GNUC_UNUSED)
{
#line 210 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::init"
	self->_priv = G_TYPE_INSTANCE_GET_PRIVATE(self,MN_TYPE_SOUND_PLAYER,MNSoundPlayerPrivate);
 {
#line 53 "src/mn-sound-player.gob"

    selfp->can_play = self_can_play(self);

    mn_g_object_gconf_notification_add_gdk_locked(self,
						  MN_CONF_SOUNDS_PLAY_COMMAND,
						  self_notify_play_command_cb,
						  self);
  
#line 223 "mn-sound-player.c"
 }
}
#undef __GOB_FUNCTION__

static void
___object_get_property (GObject *object,
	guint property_id,
	GValue *VAL G_GNUC_UNUSED,
	GParamSpec *pspec G_GNUC_UNUSED)
#define __GOB_FUNCTION__ "MN:Sound:Player::get_property"
{
	MNSoundPlayer *self G_GNUC_UNUSED;

	self = MN_SOUND_PLAYER (object);

	switch (property_id) {
	case PROP_CAN_PLAY:
		{
#line 39 "src/mn-sound-player.gob"

      g_value_set_boolean(VAL, selfp->can_play);
    
#line 246 "mn-sound-player.c"
		}
		break;
	case PROP_CAN_STOP:
		{
#line 45 "src/mn-sound-player.gob"

      g_value_set_boolean(VAL, selfp->pid != 0);
    
#line 255 "mn-sound-player.c"
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


#line 39 "src/mn-sound-player.gob"
gboolean 
mn_sound_player_get_can_play (MNSoundPlayer * self)
{
#line 275 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::get_can_play"
{
#line 37 "src/mn-sound-player.gob"
		gboolean val; g_object_get (G_OBJECT (self), "can_play", &val, NULL); return val;
}}
#line 281 "mn-sound-player.c"
#undef __GOB_FUNCTION__

#line 45 "src/mn-sound-player.gob"
gboolean 
mn_sound_player_get_can_stop (MNSoundPlayer * self)
{
#line 288 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::get_can_stop"
{
#line 43 "src/mn-sound-player.gob"
		gboolean val; g_object_get (G_OBJECT (self), "can_stop", &val, NULL); return val;
}}
#line 294 "mn-sound-player.c"
#undef __GOB_FUNCTION__


#line 62 "src/mn-sound-player.gob"
static void 
mn_sound_player_dispose (MNSoundPlayer * self G_GNUC_UNUSED)
{
#line 302 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::dispose"
{
#line 63 "src/mn-sound-player.gob"
	
    /* do not emit notify::can-stop at finalization time (pass FALSE) */
    self_stop_real(self, FALSE);
  }}
#line 310 "mn-sound-player.c"
#undef __GOB_FUNCTION__

#line 68 "src/mn-sound-player.gob"
static void 
mn_sound_player_notify_play_command_cb (GConfClient * client, unsigned int cnxn_id, GConfEntry * entry, gpointer user_data)
{
#line 317 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::notify_play_command_cb"
{
#line 73 "src/mn-sound-player.gob"
	
    Self *self = user_data;
    gboolean can_play;

    can_play = self_can_play(self);
    if (can_play != selfp->can_play)
      {
	selfp->can_play = can_play;
	g_object_notify(G_OBJECT(self), "can-play");
      }
  }}
#line 332 "mn-sound-player.c"
#undef __GOB_FUNCTION__

#line 85 "src/mn-sound-player.gob"
static gboolean 
mn_sound_player_can_play (MNSoundPlayer * self)
{
#line 339 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::can_play"
#line 85 "src/mn-sound-player.gob"
	g_return_val_if_fail (self != NULL, (gboolean )0);
#line 85 "src/mn-sound-player.gob"
	g_return_val_if_fail (MN_IS_SOUND_PLAYER (self), (gboolean )0);
#line 345 "mn-sound-player.c"
{
#line 87 "src/mn-sound-player.gob"
	
    char *command;
    gboolean can;

    command = mn_conf_get_string(MN_CONF_SOUNDS_PLAY_COMMAND);
    can = command && *command;
    g_free(command);

    return can;
  }}
#line 358 "mn-sound-player.c"
#undef __GOB_FUNCTION__

#line 98 "src/mn-sound-player.gob"
void 
mn_sound_player_play (MNSoundPlayer * self, const char * filename, GtkWindow * parent)
{
#line 365 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::play"
#line 98 "src/mn-sound-player.gob"
	g_return_if_fail (self != NULL);
#line 98 "src/mn-sound-player.gob"
	g_return_if_fail (MN_IS_SOUND_PLAYER (self));
#line 98 "src/mn-sound-player.gob"
	g_return_if_fail (filename != NULL);
#line 373 "mn-sound-player.c"
{
#line 100 "src/mn-sound-player.gob"
	
    const char *error_message;
    char *command = NULL;
    char *subst_command;
    char *shell;
    char *argv[4];
    GError *err = NULL;

    g_return_if_fail(parent == NULL || GTK_IS_WINDOW(parent));
    g_return_if_fail(selfp->can_play);

    self_stop(self);

    /*
     * If a parent window is provided, we are normally called because
     * the user interacted with the UI. Otherwise, we are called because
     * a background event has occurred, and for clarity we should
     * mention the originating application in the error message.
     */
    if (parent)
      error_message = N_("Unable to play sound");
    else
      error_message = N_("A command error has occurred in Mail Notification");

    command = mn_conf_get_string(MN_CONF_SOUNDS_PLAY_COMMAND);
    if (! command || ! *command)
      goto end;

    subst_command = mn_subst_command(command, self_subst_cb, (gpointer) filename, &err);
    if (! subst_command)
      {
	mn_show_error_dialog(parent,
			     _(error_message),
			     _("Unable to execute \"%s\": %s."),
			     command,
			     err->message);
	g_error_free(err);
	goto end;
      }

    /*
     * We cannot use gnome_execute_child() because it does not reap the
     * child, which is needed for GChildWatch.
     */

    shell = gnome_util_user_shell();

    argv[0] = shell;
    argv[1] = "-c";
    argv[2] = subst_command;
    argv[3] = NULL;

    if (g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &selfp->pid, &err))
      {
	GSource *source;

	g_object_notify(G_OBJECT(self), "can-stop");

	g_assert(selfp->watch_source == NULL);

	source = g_child_watch_source_new(selfp->pid);
	g_source_set_callback(source, (GSourceFunc) self_watch_cb, self, NULL);
	g_source_attach(source, NULL);
	selfp->watch_source = source;
      }
    else
      {
	mn_show_error_dialog(parent,
			     _(error_message),
			     _("Unable to execute \"%s\": %s."),
			     command,
			     err->message);
	g_error_free(err);
      }

    g_free(subst_command);
    g_free(shell);

  end:
    g_free(command);
  }}
#line 457 "mn-sound-player.c"
#undef __GOB_FUNCTION__

#line 182 "src/mn-sound-player.gob"
static gboolean 
mn_sound_player_subst_cb (const char * name, char ** value, gpointer data)
{
#line 464 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::subst_cb"
{
#line 184 "src/mn-sound-player.gob"
	
    const char *filename = data;

    if (! strcmp(name, "file"))
      {
	*value = g_strdup(filename);
	return TRUE;
      }

    return FALSE;
  }}
#line 479 "mn-sound-player.c"
#undef __GOB_FUNCTION__

#line 196 "src/mn-sound-player.gob"
void 
mn_sound_player_stop (MNSoundPlayer * self)
{
#line 486 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::stop"
#line 196 "src/mn-sound-player.gob"
	g_return_if_fail (self != NULL);
#line 196 "src/mn-sound-player.gob"
	g_return_if_fail (MN_IS_SOUND_PLAYER (self));
#line 492 "mn-sound-player.c"
{
#line 198 "src/mn-sound-player.gob"
	
    self_stop_real(self, TRUE);
  }}
#line 498 "mn-sound-player.c"
#undef __GOB_FUNCTION__

#line 202 "src/mn-sound-player.gob"
static void 
mn_sound_player_stop_real (MNSoundPlayer * self, gboolean notify_can_stop)
{
#line 505 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::stop_real"
#line 202 "src/mn-sound-player.gob"
	g_return_if_fail (self != NULL);
#line 202 "src/mn-sound-player.gob"
	g_return_if_fail (MN_IS_SOUND_PLAYER (self));
#line 511 "mn-sound-player.c"
{
#line 204 "src/mn-sound-player.gob"
	
    /*
     * If we remove the watch, a zombie will be left since GLib will
     * not have waited for the process, so we just abandon the watch.
     */
    if (selfp->watch_source)
      {
	g_source_set_callback(selfp->watch_source, (GSourceFunc) self_watch_abandoned_cb, NULL, NULL);
	g_source_unref(selfp->watch_source);
	selfp->watch_source = NULL;
      }

    if (selfp->pid)
      {
	kill(selfp->pid, SIGTERM);
	selfp->pid = 0;
	if (notify_can_stop)
	  g_object_notify(G_OBJECT(self), "can-stop");
      }
  }}
#line 534 "mn-sound-player.c"
#undef __GOB_FUNCTION__

#line 225 "src/mn-sound-player.gob"
static void 
mn_sound_player_watch_abandoned_cb (GPid pid, int status, gpointer data)
{
#line 541 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::watch_abandoned_cb"
{
#line 227 "src/mn-sound-player.gob"
	
    /* no op */
  }}
#line 548 "mn-sound-player.c"
#undef __GOB_FUNCTION__

#line 231 "src/mn-sound-player.gob"
static void 
mn_sound_player_watch_cb (GPid pid, int status, gpointer data)
{
#line 555 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::watch_cb"
{
#line 233 "src/mn-sound-player.gob"
	
    Self *self = data;

    GDK_THREADS_ENTER();

    g_assert(selfp->watch_source != NULL);

    g_source_unref(selfp->watch_source);
    selfp->watch_source = NULL;
    selfp->pid = 0;

    g_object_notify(G_OBJECT(self), "can-stop");

    /* do not call gdk_flush(), we're normally in the main thread */
    GDK_THREADS_LEAVE();
  }}
#line 575 "mn-sound-player.c"
#undef __GOB_FUNCTION__

#line 250 "src/mn-sound-player.gob"
MNSoundPlayer * 
mn_sound_player_new (void)
{
#line 582 "mn-sound-player.c"
#define __GOB_FUNCTION__ "MN:Sound:Player::new"
{
#line 252 "src/mn-sound-player.gob"
	
    return GET_NEW;
  }}
#line 589 "mn-sound-player.c"
#undef __GOB_FUNCTION__
