/* Generated by GOB (v2.0.9) on Thu Sep  9 01:42:01 2004
   (do not edit directly) */

/* End world hunger, donate to the World Food Programme, http://www.wfp.org */

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 9

#define selfp (self->_priv)

#include "mn-pop3-mailbox-properties.h"

#include "mn-pop3-mailbox-properties-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */

#line 22 "mn-pop3-mailbox-properties.gob"

#include "config.h"
#include <glib/gi18n-lib.h>
#include "mn-mailbox-properties.h"
#include "mn-mailbox-properties-util.h"
#include "mn-auth-combo-box.h"
#include "mn-util.h"

#line 34 "mn-pop3-mailbox-properties.c"
/* self casting macros */
#define SELF(x) MN_POP3_MAILBOX_PROPERTIES(x)
#define SELF_CONST(x) MN_POP3_MAILBOX_PROPERTIES_CONST(x)
#define IS_SELF(x) MN_IS_POP3_MAILBOX_PROPERTIES(x)
#define TYPE_SELF MN_TYPE_POP3_MAILBOX_PROPERTIES
#define SELF_CLASS(x) MN_POP3_MAILBOX_PROPERTIES_CLASS(x)

#define SELF_GET_CLASS(x) MN_POP3_MAILBOX_PROPERTIES_GET_CLASS(x)

/* self typedefs */
typedef MNPOP3MailboxProperties Self;
typedef MNPOP3MailboxPropertiesClass SelfClass;

/* here are local prototypes */
static void ___object_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void mn_pop3_mailbox_properties_init (MNPOP3MailboxProperties * o) G_GNUC_UNUSED;
static void mn_pop3_mailbox_properties_class_init (MNPOP3MailboxPropertiesClass * class) G_GNUC_UNUSED;
static GObject * ___2_mn_pop3_mailbox_properties_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_params) G_GNUC_UNUSED;
static gboolean mn_pop3_mailbox_properties_set_uri (MNMailboxProperties * properties, MNURI * uri) G_GNUC_UNUSED;
static MNURI * mn_pop3_mailbox_properties_get_uri (MNMailboxProperties * properties) G_GNUC_UNUSED;

enum {
	PROP_0,
	PROP_COMPLETE
};

/* pointer to the class of our parent */
static MNPIMailboxPropertiesClass *parent_class = NULL;

/* Short form macros */
#define self_set_uri mn_pop3_mailbox_properties_set_uri
#define self_get_uri mn_pop3_mailbox_properties_get_uri


static void
___MN_Mailbox_Properties_init (MNMailboxPropertiesIface *iface)
{
#line 94 "mn-pop3-mailbox-properties.gob"
	iface->set_uri = self_set_uri;
#line 114 "mn-pop3-mailbox-properties.gob"
	iface->get_uri = self_get_uri;
#line 76 "mn-pop3-mailbox-properties.c"
}

GType
mn_pop3_mailbox_properties_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (MNPOP3MailboxPropertiesClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mn_pop3_mailbox_properties_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (MNPOP3MailboxProperties),
			0 /* n_preallocs */,
			(GInstanceInitFunc) mn_pop3_mailbox_properties_init,
			NULL
		};

		static const GInterfaceInfo MN_Mailbox_Properties_info = {
			(GInterfaceInitFunc) ___MN_Mailbox_Properties_init,
			NULL,
			NULL
		};

		type = g_type_register_static (MN_TYPE_PI_MAILBOX_PROPERTIES, "MNPOP3MailboxProperties", &info, (GTypeFlags)0);
		g_type_add_interface_static (type,
			MN_TYPE_MAILBOX_PROPERTIES,
			&MN_Mailbox_Properties_info);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((MNPOP3MailboxProperties *)g_object_new(mn_pop3_mailbox_properties_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static MNPOP3MailboxProperties * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static MNPOP3MailboxProperties *
GET_NEW_VARG (const char *first, ...)
{
	MNPOP3MailboxProperties *ret;
	va_list ap;
	va_start (ap, first);
	ret = (MNPOP3MailboxProperties *)g_object_new_valist (mn_pop3_mailbox_properties_get_type (), first, ap);
	va_end (ap);
	return ret;
}

static void 
mn_pop3_mailbox_properties_init (MNPOP3MailboxProperties * o G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:POP3:Mailbox:Properties::init"
}
#undef __GOB_FUNCTION__
#line 60 "mn-pop3-mailbox-properties.gob"
static void 
mn_pop3_mailbox_properties_class_init (MNPOP3MailboxPropertiesClass * class G_GNUC_UNUSED)
#line 139 "mn-pop3-mailbox-properties.c"
{
#define __GOB_FUNCTION__ "MN:POP3:Mailbox:Properties::class_init"
	GObjectClass *g_object_class G_GNUC_UNUSED = (GObjectClass*) class;

	parent_class = g_type_class_ref (MN_TYPE_PI_MAILBOX_PROPERTIES);

#line 69 "mn-pop3-mailbox-properties.gob"
	g_object_class->constructor = ___2_mn_pop3_mailbox_properties_constructor;
#line 148 "mn-pop3-mailbox-properties.c"
	g_object_class->get_property = ___object_get_property;
    {
	g_object_class_override_property (g_object_class,
		PROP_COMPLETE,
		"complete");
    }
 {
#line 61 "mn-pop3-mailbox-properties.gob"

    MNPIMailboxPropertiesClass *pi_class = MN_PI_MAILBOX_PROPERTIES_CLASS(class);

    pi_class->label = "POP3";
    pi_class->default_port[0] = mn_uri_get_default_port("pop");
    pi_class->default_port[1] = mn_uri_get_default_port("pops");
  
#line 164 "mn-pop3-mailbox-properties.c"
 }
}
#undef __GOB_FUNCTION__

static void
___object_get_property (GObject *object,
	guint property_id,
	GValue *VAL G_GNUC_UNUSED,
	GParamSpec *pspec G_GNUC_UNUSED)
#define __GOB_FUNCTION__ "MN:POP3:Mailbox:Properties::get_property"
{
	MNPOP3MailboxProperties *self G_GNUC_UNUSED;

	self = MN_POP3_MAILBOX_PROPERTIES (object);

	switch (property_id) {
	case PROP_COMPLETE:
		{
#line 35 "mn-pop3-mailbox-properties.gob"

      MNPIMailboxProperties *pi = MN_PI_MAILBOX_PROPERTIES(self);
      gboolean complete;
      const char *username;
      const char *password;
      const char *hostname;
      
      mn_pi_mailbox_properties_get_contents(pi,
					    NULL,
					    NULL,
					    &username,
					    &password,
					    NULL,
					    &hostname,
					    NULL);

      complete = *username && *password && *hostname;
#ifndef WITH_SSL
      if (complete)
	complete = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pi->conn_radio[0]));
#endif /* WITH_SSL */

      g_value_set_boolean(VAL, complete);
    
#line 208 "mn-pop3-mailbox-properties.c"
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



#line 69 "mn-pop3-mailbox-properties.gob"
static GObject * 
___2_mn_pop3_mailbox_properties_constructor (GType type G_GNUC_UNUSED, guint n_construct_properties, GObjectConstructParam * construct_params)
#line 228 "mn-pop3-mailbox-properties.c"
#define PARENT_HANDLER(___type,___n_construct_properties,___construct_params) \
	((G_OBJECT_CLASS(parent_class)->constructor)? \
		(* G_OBJECT_CLASS(parent_class)->constructor)(___type,___n_construct_properties,___construct_params): \
		((GObject * )0))
{
#define __GOB_FUNCTION__ "MN:POP3:Mailbox:Properties::constructor"
{
#line 71 "mn-pop3-mailbox-properties.gob"
	
    GObject *object;
    Self *self;
    MNPIMailboxProperties *pi;

    object = PARENT_HANDLER(type, n_construct_properties, construct_params);
    self = SELF(object);
    pi = MN_PI_MAILBOX_PROPERTIES(object);

    mn_auth_combo_box_append(MN_AUTH_COMBO_BOX(pi->auth_combo), "+APOP", "APOP");
    mn_auth_combo_box_append(MN_AUTH_COMBO_BOX(pi->auth_combo), "+USERPASS", "USER/PASS");

    mn_tooltips_set_tips(pi->tooltips,
			 pi->hostname_entry, _("The hostname or IP address of the POP3 server"),
			 pi->username_entry, _("Your username on the POP3 server"),
			 pi->password_entry, _("Your password on the POP3 server"),
			 pi->port_spin[0], _("The port number of the POP3 server"),
			 pi->port_spin[1], _("The port number of the POP3 server"),
			 NULL);

    return object;
  }}
#line 259 "mn-pop3-mailbox-properties.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

#line 94 "mn-pop3-mailbox-properties.gob"
static gboolean 
mn_pop3_mailbox_properties_set_uri (MNMailboxProperties * properties, MNURI * uri)
#line 266 "mn-pop3-mailbox-properties.c"
{
#define __GOB_FUNCTION__ "MN:POP3:Mailbox:Properties::set_uri"
#line 94 "mn-pop3-mailbox-properties.gob"
	g_return_val_if_fail (properties != NULL, (gboolean )0);
#line 94 "mn-pop3-mailbox-properties.gob"
	g_return_val_if_fail (MN_IS_MAILBOX_PROPERTIES (properties), (gboolean )0);
#line 94 "mn-pop3-mailbox-properties.gob"
	g_return_val_if_fail (uri != NULL, (gboolean )0);
#line 94 "mn-pop3-mailbox-properties.gob"
	g_return_val_if_fail (MN_IS_URI (uri), (gboolean )0);
#line 277 "mn-pop3-mailbox-properties.c"
{
#line 97 "mn-pop3-mailbox-properties.gob"
	
    if (MN_URI_IS_POP(uri))
      {
	mn_pi_mailbox_properties_set_contents(MN_PI_MAILBOX_PROPERTIES(properties),
					      MN_URI_IS_SSL(uri),
					      MN_URI_IS_INBAND_SSL(uri),
					      uri->username,
					      uri->password,
					      uri->authmech,
					      uri->hostname,
					      uri->port);
	return TRUE;
      }
    else
      return FALSE;
  }}
#line 296 "mn-pop3-mailbox-properties.c"
#undef __GOB_FUNCTION__

#line 114 "mn-pop3-mailbox-properties.gob"
static MNURI * 
mn_pop3_mailbox_properties_get_uri (MNMailboxProperties * properties)
#line 302 "mn-pop3-mailbox-properties.c"
{
#define __GOB_FUNCTION__ "MN:POP3:Mailbox:Properties::get_uri"
#line 114 "mn-pop3-mailbox-properties.gob"
	g_return_val_if_fail (properties != NULL, (MNURI * )0);
#line 114 "mn-pop3-mailbox-properties.gob"
	g_return_val_if_fail (MN_IS_MAILBOX_PROPERTIES (properties), (MNURI * )0);
#line 309 "mn-pop3-mailbox-properties.c"
{
#line 116 "mn-pop3-mailbox-properties.gob"
	
    gboolean ssl;
    gboolean inband_ssl;
    const char *username;
    const char *password;
    char *authmech;
    const char *hostname;
    int port;
    MNURI *uri;

    mn_pi_mailbox_properties_get_contents(MN_PI_MAILBOX_PROPERTIES(properties),
					  &ssl,
					  &inband_ssl,
					  &username,
					  &password,
					  &authmech,
					  &hostname,
					  &port);

    uri = mn_uri_new_pop(ssl, inband_ssl, username, password, authmech, hostname, port);
    g_free(authmech);

    return uri;
  }}
#line 336 "mn-pop3-mailbox-properties.c"
#undef __GOB_FUNCTION__
