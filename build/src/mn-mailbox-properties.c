/* Generated by GOB (v2.0.17)   (do not edit directly) */

/* End world hunger, donate to the World Food Programme, http://www.wfp.org */

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 17

#define selfp (self->_priv)

#include <string.h> /* memset() */

#include "mn-mailbox-properties.h"

#include "mn-mailbox-properties-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */

#line 25 "src/mn-mailbox-properties.gob"

#include "mn-util.h"
#include "mn-mailbox-properties-dialog.h"

#line 31 "mn-mailbox-properties.c"
/* self casting macros */
#define SELF(x) MN_MAILBOX_PROPERTIES(x)
#define SELF_CONST(x) MN_MAILBOX_PROPERTIES_CONST(x)
#define IS_SELF(x) MN_IS_MAILBOX_PROPERTIES(x)
#define TYPE_SELF MN_TYPE_MAILBOX_PROPERTIES
#define SELF_CLASS(x) MN_MAILBOX_PROPERTIES_CLASS(x)

#define SELF_GET_CLASS(x) MN_MAILBOX_PROPERTIES_GET_CLASS(x)

/* self typedefs */
typedef MNMailboxProperties Self;
typedef MNMailboxPropertiesClass SelfClass;

/* here are local prototypes */
static void ___object_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void ___object_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
#line 0 "src/mn-mailbox-properties.gob"
static void mn_mailbox_properties_init (MNMailboxProperties * o) G_GNUC_UNUSED;
#line 50 "mn-mailbox-properties.c"
#line 0 "src/mn-mailbox-properties.gob"
static void mn_mailbox_properties_class_init (MNMailboxPropertiesClass * c) G_GNUC_UNUSED;
#line 53 "mn-mailbox-properties.c"
static MNMailbox * ___real_mn_mailbox_properties_get_mailbox (MNMailboxProperties * self);
#line 91 "src/mn-mailbox-properties.gob"
static void ___6_mn_mailbox_properties_activate (MNMailboxProperties * properties) G_GNUC_UNUSED;
#line 57 "mn-mailbox-properties.c"
#line 101 "src/mn-mailbox-properties.gob"
static void ___7_mn_mailbox_properties_deactivate (MNMailboxProperties * properties) G_GNUC_UNUSED;
#line 60 "mn-mailbox-properties.c"

enum {
	PROP_0,
	PROP_DIALOG,
	PROP_COMPLETE,
	PROP_DEFAULT_NAME,
	PROP_DEFAULT_CHECK_DELAY
};

/* pointer to the class of our parent */
static GObjectClass *parent_class = NULL;

/* Short form macros */
#define self_activate mn_mailbox_properties_activate
#define self_deactivate mn_mailbox_properties_deactivate
#define self_set_mailbox mn_mailbox_properties_set_mailbox
#define self_get_mailbox mn_mailbox_properties_get_mailbox
#define self_add_general_section mn_mailbox_properties_add_general_section
#define self_notify_complete mn_mailbox_properties_notify_complete
#define self_notify_default_name mn_mailbox_properties_notify_default_name
#define self_notify_default_check_delay mn_mailbox_properties_notify_default_check_delay
GType
mn_mailbox_properties_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (MNMailboxPropertiesClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mn_mailbox_properties_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (MNMailboxProperties),
			0 /* n_preallocs */,
			(GInstanceInitFunc) mn_mailbox_properties_init,
			NULL
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MNMailboxProperties", &info, (GTypeFlags)G_TYPE_FLAG_ABSTRACT);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((MNMailboxProperties *)g_object_new(mn_mailbox_properties_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static MNMailboxProperties * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static MNMailboxProperties *
GET_NEW_VARG (const char *first, ...)
{
	MNMailboxProperties *ret;
	va_list ap;
	va_start (ap, first);
	ret = (MNMailboxProperties *)g_object_new_valist (mn_mailbox_properties_get_type (), first, ap);
	va_end (ap);
	return ret;
}


static void
___dispose (GObject *obj_self)
{
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::dispose"
	MNMailboxProperties *self G_GNUC_UNUSED = MN_MAILBOX_PROPERTIES (obj_self);
	if (G_OBJECT_CLASS (parent_class)->dispose) \
		(* G_OBJECT_CLASS (parent_class)->dispose) (obj_self);
#line 40 "src/mn-mailbox-properties.gob"
	if(self->label_size_group) { g_object_unref ((gpointer) self->label_size_group); self->label_size_group = NULL; }
#line 134 "mn-mailbox-properties.c"
}
#undef __GOB_FUNCTION__


static void
___finalize(GObject *obj_self)
{
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::finalize"
	MNMailboxProperties *self G_GNUC_UNUSED = MN_MAILBOX_PROPERTIES (obj_self);
	gpointer priv G_GNUC_UNUSED = self->_priv;
	if(G_OBJECT_CLASS(parent_class)->finalize) \
		(* G_OBJECT_CLASS(parent_class)->finalize)(obj_self);
#line 42 "src/mn-mailbox-properties.gob"
	if(self->entries) { g_slist_free ((gpointer) self->entries); self->entries = NULL; }
#line 149 "mn-mailbox-properties.c"
#line 44 "src/mn-mailbox-properties.gob"
	if(self->_priv->general_sections) { mn_g_object_slist_free ((gpointer) self->_priv->general_sections); self->_priv->general_sections = NULL; }
#line 152 "mn-mailbox-properties.c"
}
#undef __GOB_FUNCTION__

static void 
mn_mailbox_properties_init (MNMailboxProperties * o G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::init"
	o->_priv = G_TYPE_INSTANCE_GET_PRIVATE(o,MN_TYPE_MAILBOX_PROPERTIES,MNMailboxPropertiesPrivate);
#line 40 "src/mn-mailbox-properties.gob"
	o->label_size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
#line 163 "mn-mailbox-properties.c"
}
#undef __GOB_FUNCTION__
static void 
mn_mailbox_properties_class_init (MNMailboxPropertiesClass * c G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::class_init"
	GObjectClass *g_object_class G_GNUC_UNUSED = (GObjectClass*) c;
	MNMailboxPropertiesClass *mn_mailbox_properties_class = (MNMailboxPropertiesClass *)c;

	g_type_class_add_private(c,sizeof(MNMailboxPropertiesPrivate));
#line 25 "src/mn-mailbox-properties.gob"
	c->enable_check_delay = TRUE;
#line 176 "mn-mailbox-properties.c"

	parent_class = g_type_class_ref (G_TYPE_OBJECT);

	c->activate = NULL;
	c->deactivate = NULL;
	c->set_mailbox = NULL;
#line 71 "src/mn-mailbox-properties.gob"
	c->get_mailbox = ___real_mn_mailbox_properties_get_mailbox;
#line 91 "src/mn-mailbox-properties.gob"
	mn_mailbox_properties_class->activate = ___6_mn_mailbox_properties_activate;
#line 101 "src/mn-mailbox-properties.gob"
	mn_mailbox_properties_class->deactivate = ___7_mn_mailbox_properties_deactivate;
#line 189 "mn-mailbox-properties.c"
	g_object_class->dispose = ___dispose;
	g_object_class->finalize = ___finalize;
	g_object_class->get_property = ___object_get_property;
	g_object_class->set_property = ___object_set_property;
    {
	GParamSpec   *param_spec;

	param_spec = g_param_spec_pointer
		("dialog" /* name */,
		 NULL /* nick */,
		 NULL /* blurb */,
		 (GParamFlags)(G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (g_object_class,
		PROP_DIALOG,
		param_spec);
	param_spec = g_param_spec_boolean
		("complete" /* name */,
		 NULL /* nick */,
		 NULL /* blurb */,
		 FALSE /* default_value */,
		 (GParamFlags)(G_PARAM_READABLE));
	g_object_class_install_property (g_object_class,
		PROP_COMPLETE,
		param_spec);
	param_spec = g_param_spec_string
		("default_name" /* name */,
		 NULL /* nick */,
		 NULL /* blurb */,
		 NULL /* default_value */,
		 (GParamFlags)(G_PARAM_READABLE));
	g_object_class_install_property (g_object_class,
		PROP_DEFAULT_NAME,
		param_spec);
	param_spec = g_param_spec_int
		("default_check_delay" /* name */,
		 NULL /* nick */,
		 NULL /* blurb */,
		 G_MININT /* minimum */,
		 G_MAXINT /* maximum */,
		 0 /* default_value */,
		 (GParamFlags)(G_PARAM_READABLE));
	g_object_class_install_property (g_object_class,
		PROP_DEFAULT_CHECK_DELAY,
		param_spec);
    }
}
#undef __GOB_FUNCTION__

static void
___object_set_property (GObject *object,
	guint property_id,
	const GValue *VAL G_GNUC_UNUSED,
	GParamSpec *pspec G_GNUC_UNUSED)
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::set_property"
{
	MNMailboxProperties *self G_GNUC_UNUSED;

	self = MN_MAILBOX_PROPERTIES (object);

	switch (property_id) {
	case PROP_DIALOG:
		{
#line 38 "src/mn-mailbox-properties.gob"
self->dialog = g_value_get_pointer (VAL);
#line 254 "mn-mailbox-properties.c"
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
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::get_property"
{
	MNMailboxProperties *self G_GNUC_UNUSED;

	self = MN_MAILBOX_PROPERTIES (object);

	switch (property_id) {
	case PROP_DIALOG:
		{
#line 38 "src/mn-mailbox-properties.gob"
g_value_set_pointer (VAL, self->dialog);
#line 285 "mn-mailbox-properties.c"
		}
		break;
	case PROP_COMPLETE:
		{
#line 47 "src/mn-mailbox-properties.gob"
 g_value_set_boolean(VAL, TRUE); 
#line 292 "mn-mailbox-properties.c"
		}
		break;
	case PROP_DEFAULT_NAME:
		{
#line 50 "src/mn-mailbox-properties.gob"
 g_value_set_string(VAL, NULL); 
#line 299 "mn-mailbox-properties.c"
		}
		break;
	case PROP_DEFAULT_CHECK_DELAY:
		{
#line 54 "src/mn-mailbox-properties.gob"

      MNMailboxClass *class;

      class = mn_mailbox_get_class_from_name(SELF_GET_CLASS(self)->type);
      g_value_set_int(VAL, class->default_check_delay);
      g_type_class_unref(class);
    
#line 312 "mn-mailbox-properties.c"
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



#line 62 "src/mn-mailbox-properties.gob"
void 
mn_mailbox_properties_activate (MNMailboxProperties * self)
{
#line 333 "mn-mailbox-properties.c"
	MNMailboxPropertiesClass *klass;
#line 62 "src/mn-mailbox-properties.gob"
	g_return_if_fail (self != NULL);
#line 62 "src/mn-mailbox-properties.gob"
	g_return_if_fail (MN_IS_MAILBOX_PROPERTIES (self));
#line 339 "mn-mailbox-properties.c"
	klass = MN_MAILBOX_PROPERTIES_GET_CLASS(self);

	if(klass->activate)
		(*klass->activate)(self);
}

#line 65 "src/mn-mailbox-properties.gob"
void 
mn_mailbox_properties_deactivate (MNMailboxProperties * self)
{
#line 350 "mn-mailbox-properties.c"
	MNMailboxPropertiesClass *klass;
#line 65 "src/mn-mailbox-properties.gob"
	g_return_if_fail (self != NULL);
#line 65 "src/mn-mailbox-properties.gob"
	g_return_if_fail (MN_IS_MAILBOX_PROPERTIES (self));
#line 356 "mn-mailbox-properties.c"
	klass = MN_MAILBOX_PROPERTIES_GET_CLASS(self);

	if(klass->deactivate)
		(*klass->deactivate)(self);
}

#line 68 "src/mn-mailbox-properties.gob"
void 
mn_mailbox_properties_set_mailbox (MNMailboxProperties * self, MNMailbox * mailbox)
{
#line 367 "mn-mailbox-properties.c"
	MNMailboxPropertiesClass *klass;
#line 68 "src/mn-mailbox-properties.gob"
	g_return_if_fail (self != NULL);
#line 68 "src/mn-mailbox-properties.gob"
	g_return_if_fail (MN_IS_MAILBOX_PROPERTIES (self));
#line 68 "src/mn-mailbox-properties.gob"
	g_return_if_fail (mailbox != NULL);
#line 68 "src/mn-mailbox-properties.gob"
	g_return_if_fail (MN_IS_MAILBOX (mailbox));
#line 377 "mn-mailbox-properties.c"
	klass = MN_MAILBOX_PROPERTIES_GET_CLASS(self);

	if(klass->set_mailbox)
		(*klass->set_mailbox)(self,mailbox);
}

#line 71 "src/mn-mailbox-properties.gob"
MNMailbox * 
mn_mailbox_properties_get_mailbox (MNMailboxProperties * self)
{
#line 388 "mn-mailbox-properties.c"
	MNMailboxPropertiesClass *klass;
#line 71 "src/mn-mailbox-properties.gob"
	g_return_val_if_fail (self != NULL, (MNMailbox * )0);
#line 71 "src/mn-mailbox-properties.gob"
	g_return_val_if_fail (MN_IS_MAILBOX_PROPERTIES (self), (MNMailbox * )0);
#line 394 "mn-mailbox-properties.c"
	klass = MN_MAILBOX_PROPERTIES_GET_CLASS(self);

	if(klass->get_mailbox)
		return (*klass->get_mailbox)(self);
	else
		return (MNMailbox * )(0);
}
#line 71 "src/mn-mailbox-properties.gob"
static MNMailbox * 
___real_mn_mailbox_properties_get_mailbox (MNMailboxProperties * self G_GNUC_UNUSED)
{
#line 406 "mn-mailbox-properties.c"
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::get_mailbox"
{
#line 73 "src/mn-mailbox-properties.gob"
	
    return mn_mailbox_new(SELF_GET_CLASS(self)->type, NULL);
  }}
#line 413 "mn-mailbox-properties.c"
#undef __GOB_FUNCTION__

#line 77 "src/mn-mailbox-properties.gob"
GtkWidget * 
mn_mailbox_properties_add_general_section (MNMailboxProperties * self, const char * title)
{
#line 420 "mn-mailbox-properties.c"
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::add_general_section"
#line 77 "src/mn-mailbox-properties.gob"
	g_return_val_if_fail (self != NULL, (GtkWidget * )0);
#line 77 "src/mn-mailbox-properties.gob"
	g_return_val_if_fail (MN_IS_MAILBOX_PROPERTIES (self), (GtkWidget * )0);
#line 77 "src/mn-mailbox-properties.gob"
	g_return_val_if_fail (title != NULL, (GtkWidget * )0);
#line 428 "mn-mailbox-properties.c"
{
#line 79 "src/mn-mailbox-properties.gob"
	
    GtkWidget *section;
    GtkWidget *vbox;

    section = mn_hig_section_new_with_box(title, NULL, &vbox);
    g_object_ref_sink(section);
    gtk_widget_show(section);

    selfp->general_sections = g_slist_append(selfp->general_sections, section);
    return vbox;
  }}
#line 442 "mn-mailbox-properties.c"
#undef __GOB_FUNCTION__

#line 91 "src/mn-mailbox-properties.gob"
static void 
___6_mn_mailbox_properties_activate (MNMailboxProperties * properties G_GNUC_UNUSED)
#line 448 "mn-mailbox-properties.c"
#define PARENT_HANDLER(___properties) \
	{ if(MN_MAILBOX_PROPERTIES_CLASS(parent_class)->activate) \
		(* MN_MAILBOX_PROPERTIES_CLASS(parent_class)->activate)(___properties); }
{
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::activate"
{
#line 93 "src/mn-mailbox-properties.gob"
	
    Self *self = SELF(properties);
    GSList *l;

    MN_LIST_FOREACH(l, selfp->general_sections)
      gtk_box_pack_start(GTK_BOX(MN_MAILBOX_PROPERTIES_DIALOG(properties->dialog)->general_vbox), l->data, FALSE, FALSE, 0);
  }}
#line 463 "mn-mailbox-properties.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

#line 101 "src/mn-mailbox-properties.gob"
static void 
___7_mn_mailbox_properties_deactivate (MNMailboxProperties * properties G_GNUC_UNUSED)
#line 470 "mn-mailbox-properties.c"
#define PARENT_HANDLER(___properties) \
	{ if(MN_MAILBOX_PROPERTIES_CLASS(parent_class)->deactivate) \
		(* MN_MAILBOX_PROPERTIES_CLASS(parent_class)->deactivate)(___properties); }
{
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::deactivate"
{
#line 103 "src/mn-mailbox-properties.gob"
	
    Self *self = SELF(properties);
    GSList *l;

    MN_LIST_FOREACH(l, selfp->general_sections)
      gtk_container_remove(GTK_CONTAINER(MN_MAILBOX_PROPERTIES_DIALOG(properties->dialog)->general_vbox), l->data);
  }}
#line 485 "mn-mailbox-properties.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

#line 111 "src/mn-mailbox-properties.gob"
void 
mn_mailbox_properties_notify_complete (MNMailboxProperties * self)
{
#line 493 "mn-mailbox-properties.c"
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::notify_complete"
#line 111 "src/mn-mailbox-properties.gob"
	g_return_if_fail (self != NULL);
#line 111 "src/mn-mailbox-properties.gob"
	g_return_if_fail (MN_IS_MAILBOX_PROPERTIES (self));
#line 499 "mn-mailbox-properties.c"
{
#line 113 "src/mn-mailbox-properties.gob"
	
    g_object_notify(G_OBJECT(self), "complete");
  }}
#line 505 "mn-mailbox-properties.c"
#undef __GOB_FUNCTION__

#line 117 "src/mn-mailbox-properties.gob"
void 
mn_mailbox_properties_notify_default_name (MNMailboxProperties * self)
{
#line 512 "mn-mailbox-properties.c"
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::notify_default_name"
#line 117 "src/mn-mailbox-properties.gob"
	g_return_if_fail (self != NULL);
#line 117 "src/mn-mailbox-properties.gob"
	g_return_if_fail (MN_IS_MAILBOX_PROPERTIES (self));
#line 518 "mn-mailbox-properties.c"
{
#line 119 "src/mn-mailbox-properties.gob"
	
    g_object_notify(G_OBJECT(self), "default-name");
  }}
#line 524 "mn-mailbox-properties.c"
#undef __GOB_FUNCTION__

#line 123 "src/mn-mailbox-properties.gob"
void 
mn_mailbox_properties_notify_default_check_delay (MNMailboxProperties * self)
{
#line 531 "mn-mailbox-properties.c"
#define __GOB_FUNCTION__ "MN:Mailbox:Properties::notify_default_check_delay"
#line 123 "src/mn-mailbox-properties.gob"
	g_return_if_fail (self != NULL);
#line 123 "src/mn-mailbox-properties.gob"
	g_return_if_fail (MN_IS_MAILBOX_PROPERTIES (self));
#line 537 "mn-mailbox-properties.c"
{
#line 125 "src/mn-mailbox-properties.gob"
	
    g_object_notify(G_OBJECT(self), "default-check-delay");
  }}
#line 543 "mn-mailbox-properties.c"
#undef __GOB_FUNCTION__
