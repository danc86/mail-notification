/* Generated by GOB (v2.0.11) on Mon Nov 15 16:14:35 2004
   (do not edit directly) */

/* End world hunger, donate to the World Food Programme, http://www.wfp.org */

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 11

#define selfp (self->_priv)

#include "mn-gmail-mailbox.h"

#include "mn-gmail-mailbox-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */

#line 29 "mn-gmail-mailbox.gob"

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#endif
#include <glib/gi18n.h>
#include <eel/eel.h>
#include "mn-mailbox-private.h"
#include "mn-util.h"
#include "mn-stock.h"
#include "mn-sgml-ref.h"

  typedef enum
  {
    JS_ARG_STRING,
    JS_ARG_LIST
  } JSArgType;
  
  typedef struct
  {
    JSArgType type;
    union 
    {
      char	*string;
      GSList	*list;
    } value;
  } JSArg;

#line 58 "mn-gmail-mailbox.c"
/* self casting macros */
#define SELF(x) MN_GMAIL_MAILBOX(x)
#define SELF_CONST(x) MN_GMAIL_MAILBOX_CONST(x)
#define IS_SELF(x) MN_IS_GMAIL_MAILBOX(x)
#define TYPE_SELF MN_TYPE_GMAIL_MAILBOX
#define SELF_CLASS(x) MN_GMAIL_MAILBOX_CLASS(x)

#define SELF_GET_CLASS(x) MN_GMAIL_MAILBOX_GET_CLASS(x)

/* self typedefs */
typedef MNGmailMailbox Self;
typedef MNGmailMailboxClass SelfClass;

/* here are local prototypes */
static void mn_gmail_mailbox_init (MNGmailMailbox * o) G_GNUC_UNUSED;
static void mn_gmail_mailbox_class_init (MNGmailMailboxClass * class) G_GNUC_UNUSED;
static GObject * ___2_mn_gmail_mailbox_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_params) G_GNUC_UNUSED;
static gboolean ___3_mn_gmail_mailbox_impl_is (MNMailbox * dummy, MNURI * uri) G_GNUC_UNUSED;
static MNAuthenticatedMailboxResult ___4_mn_gmail_mailbox_impl_authenticated_check (MNAuthenticatedMailbox * authenticated_mailbox, GError ** err) G_GNUC_UNUSED;
static SoupUri * mn_gmail_mailbox_soup_uri_new (const char * text_uri, GError ** err) G_GNUC_UNUSED;
static char * mn_gmail_mailbox_get (MNGmailMailbox * self, SoupSession * session, const char * text_uri, GError ** err) G_GNUC_UNUSED;
static char * mn_gmail_mailbox_get_from_uri (MNGmailMailbox * self, SoupSession * session, SoupUri * uri, GError ** err) G_GNUC_UNUSED;
static void mn_gmail_mailbox_dump_request (MNGmailMailbox * self, SoupMessage * message) G_GNUC_UNUSED;
static void mn_gmail_mailbox_dump_request_cb (gpointer key, gpointer value, gpointer user_data) G_GNUC_UNUSED;
static void mn_gmail_mailbox_dump_response (MNGmailMailbox * self, SoupMessage * message, const char * body) G_GNUC_UNUSED;
static void mn_gmail_mailbox_dump_response_cb (gpointer key, gpointer value, gpointer user_data) G_GNUC_UNUSED;
static char * mn_gmail_mailbox_get_substring (const char * str, const char * pre, const char * post, gboolean include_pre, gboolean include_post) G_GNUC_UNUSED;
static gboolean mn_gmail_mailbox_login (MNGmailMailbox * self, SoupSession * session, gboolean * authentication_failed, GError ** err) G_GNUC_UNUSED;
static void mn_gmail_mailbox_update_cookies (MNGmailMailbox * self, SoupMessage * message) G_GNUC_UNUSED;
static char * mn_gmail_mailbox_build_cookie (MNGmailMailbox * self) G_GNUC_UNUSED;
static void mn_gmail_mailbox_build_cookie_cb (gpointer key, gpointer value, gpointer user_data) G_GNUC_UNUSED;
static gboolean mn_gmail_mailbox_check (MNGmailMailbox * self, SoupSession * session, GError ** err) G_GNUC_UNUSED;
static void mn_gmail_mailbox_append_message (MNGmailMailbox * self, GSList ** messages, JSArg * arg) G_GNUC_UNUSED;
static gboolean mn_gmail_mailbox_js_get_arg_list (char ** str, GSList ** list) G_GNUC_UNUSED;
static gboolean mn_gmail_mailbox_js_arg_list_scan (GSList * list, ...) G_GNUC_UNUSED;
static void mn_gmail_mailbox_js_arg_list_free (GSList * list) G_GNUC_UNUSED;
static JSArg * mn_gmail_mailbox_js_arg_new_string (const char * string) G_GNUC_UNUSED;
static JSArg * mn_gmail_mailbox_js_arg_new_list (GSList * list) G_GNUC_UNUSED;
static void mn_gmail_mailbox_js_arg_free (JSArg * arg) G_GNUC_UNUSED;
static time_t mn_gmail_mailbox_parse_date (const char * date) G_GNUC_UNUSED;

/* pointer to the class of our parent */
static MNAuthenticatedMailboxClass *parent_class = NULL;

/* Short form macros */
#define self_soup_uri_new mn_gmail_mailbox_soup_uri_new
#define self_get mn_gmail_mailbox_get
#define self_get_from_uri mn_gmail_mailbox_get_from_uri
#define self_dump_request mn_gmail_mailbox_dump_request
#define self_dump_request_cb mn_gmail_mailbox_dump_request_cb
#define self_dump_response mn_gmail_mailbox_dump_response
#define self_dump_response_cb mn_gmail_mailbox_dump_response_cb
#define self_get_substring mn_gmail_mailbox_get_substring
#define self_login mn_gmail_mailbox_login
#define self_update_cookies mn_gmail_mailbox_update_cookies
#define self_build_cookie mn_gmail_mailbox_build_cookie
#define self_build_cookie_cb mn_gmail_mailbox_build_cookie_cb
#define self_check mn_gmail_mailbox_check
#define self_append_message mn_gmail_mailbox_append_message
#define self_js_get_arg_list mn_gmail_mailbox_js_get_arg_list
#define self_js_arg_list_scan mn_gmail_mailbox_js_arg_list_scan
#define self_js_arg_list_free mn_gmail_mailbox_js_arg_list_free
#define self_js_arg_new_string mn_gmail_mailbox_js_arg_new_string
#define self_js_arg_new_list mn_gmail_mailbox_js_arg_new_list
#define self_js_arg_free mn_gmail_mailbox_js_arg_free
#define self_parse_date mn_gmail_mailbox_parse_date
GType
mn_gmail_mailbox_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (MNGmailMailboxClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mn_gmail_mailbox_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (MNGmailMailbox),
			0 /* n_preallocs */,
			(GInstanceInitFunc) mn_gmail_mailbox_init,
			NULL
		};

		type = g_type_register_static (MN_TYPE_AUTHENTICATED_MAILBOX, "MNGmailMailbox", &info, (GTypeFlags)0);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((MNGmailMailbox *)g_object_new(mn_gmail_mailbox_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static MNGmailMailbox * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static MNGmailMailbox *
GET_NEW_VARG (const char *first, ...)
{
	MNGmailMailbox *ret;
	va_list ap;
	va_start (ap, first);
	ret = (MNGmailMailbox *)g_object_new_valist (mn_gmail_mailbox_get_type (), first, ap);
	va_end (ap);
	return ret;
}


static void
___finalize(GObject *obj_self)
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::finalize"
	MNGmailMailbox *self G_GNUC_UNUSED = MN_GMAIL_MAILBOX (obj_self);
	gpointer priv G_GNUC_UNUSED = self->_priv;
	if(G_OBJECT_CLASS(parent_class)->finalize) \
		(* G_OBJECT_CLASS(parent_class)->finalize)(obj_self);
#line 64 "mn-gmail-mailbox.gob"
	if(self->_priv->cookies) { g_hash_table_destroy ((gpointer) self->_priv->cookies); self->_priv->cookies = NULL; }
#line 178 "mn-gmail-mailbox.c"
}
#undef __GOB_FUNCTION__

static void 
mn_gmail_mailbox_init (MNGmailMailbox * o G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::init"
	o->_priv = G_TYPE_INSTANCE_GET_PRIVATE(o,TYPE_SELF,MNGmailMailboxPrivate);
}
#undef __GOB_FUNCTION__
#line 67 "mn-gmail-mailbox.gob"
static void 
mn_gmail_mailbox_class_init (MNGmailMailboxClass * class G_GNUC_UNUSED)
#line 192 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::class_init"
	GObjectClass *g_object_class G_GNUC_UNUSED = (GObjectClass*) class;
	MNMailboxClass *mn_mailbox_class = (MNMailboxClass *)class;
	MNAuthenticatedMailboxClass *mn_authenticated_mailbox_class = (MNAuthenticatedMailboxClass *)class;

	g_type_class_add_private(class,sizeof(MNGmailMailboxPrivate));

	parent_class = g_type_class_ref (MN_TYPE_AUTHENTICATED_MAILBOX);

#line 73 "mn-gmail-mailbox.gob"
	g_object_class->constructor = ___2_mn_gmail_mailbox_constructor;
#line 86 "mn-gmail-mailbox.gob"
	mn_mailbox_class->impl_is = ___3_mn_gmail_mailbox_impl_is;
#line 92 "mn-gmail-mailbox.gob"
	mn_authenticated_mailbox_class->impl_authenticated_check = ___4_mn_gmail_mailbox_impl_authenticated_check;
#line 209 "mn-gmail-mailbox.c"
	g_object_class->finalize = ___finalize;
 {
#line 68 "mn-gmail-mailbox.gob"

    MN_MAILBOX_CLASS(class)->stock_id = MN_STOCK_GMAIL;
    MN_MAILBOX_CLASS(class)->format = "Gmail";
  
#line 217 "mn-gmail-mailbox.c"
 }
}
#undef __GOB_FUNCTION__



#line 73 "mn-gmail-mailbox.gob"
static GObject * 
___2_mn_gmail_mailbox_constructor (GType type G_GNUC_UNUSED, guint n_construct_properties, GObjectConstructParam * construct_params)
#line 227 "mn-gmail-mailbox.c"
#define PARENT_HANDLER(___type,___n_construct_properties,___construct_params) \
	((G_OBJECT_CLASS(parent_class)->constructor)? \
		(* G_OBJECT_CLASS(parent_class)->constructor)(___type,___n_construct_properties,___construct_params): \
		((GObject * )0))
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::constructor"
{
#line 75 "mn-gmail-mailbox.gob"
	
    GObject *object;

    object = PARENT_HANDLER(type, n_construct_properties, construct_params);
    
    if (! soup_ssl_supported)
      mn_mailbox_set_init_error(MN_MAILBOX(object), _("libsoup has not been compiled with SSL/TLS support"));

    return object;
  }}
#line 246 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

#line 86 "mn-gmail-mailbox.gob"
static gboolean 
___3_mn_gmail_mailbox_impl_is (MNMailbox * dummy G_GNUC_UNUSED, MNURI * uri)
#line 253 "mn-gmail-mailbox.c"
#define PARENT_HANDLER(___dummy,___uri) \
	((MN_MAILBOX_CLASS(parent_class)->impl_is)? \
		(* MN_MAILBOX_CLASS(parent_class)->impl_is)(___dummy,___uri): \
		((gboolean )0))
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::impl_is"
#line 86 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (uri != NULL, (gboolean )0);
#line 86 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (MN_IS_URI (uri), (gboolean )0);
#line 264 "mn-gmail-mailbox.c"
{
#line 88 "mn-gmail-mailbox.gob"
	
    return MN_URI_IS_GMAIL(uri);
  }}
#line 270 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

#line 92 "mn-gmail-mailbox.gob"
static MNAuthenticatedMailboxResult 
___4_mn_gmail_mailbox_impl_authenticated_check (MNAuthenticatedMailbox * authenticated_mailbox G_GNUC_UNUSED, GError ** err)
#line 277 "mn-gmail-mailbox.c"
#define PARENT_HANDLER(___authenticated_mailbox,___err) \
	((MN_AUTHENTICATED_MAILBOX_CLASS(parent_class)->impl_authenticated_check)? \
		(* MN_AUTHENTICATED_MAILBOX_CLASS(parent_class)->impl_authenticated_check)(___authenticated_mailbox,___err): \
		((MNAuthenticatedMailboxResult )0))
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::impl_authenticated_check"
#line 92 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (authenticated_mailbox != NULL, (MNAuthenticatedMailboxResult )0);
#line 92 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (MN_IS_AUTHENTICATED_MAILBOX (authenticated_mailbox), (MNAuthenticatedMailboxResult )0);
#line 288 "mn-gmail-mailbox.c"
{
#line 95 "mn-gmail-mailbox.gob"
	
    Self *self = SELF(authenticated_mailbox);
    SoupSession *session;
    gboolean authentication_failed;
    MNAuthenticatedMailboxResult result;

    session = mn_soup_session_sync_new();

    if (! selfp->logged_in)
      selfp->logged_in = self_login(self, session, &authentication_failed, err);

    if (selfp->logged_in)
      result = self_check(self, session, err) ? MN_AUTHENTICATED_MAILBOX_OK : MN_AUTHENTICATED_MAILBOX_ERROR_OTHER;
    else
      result = authentication_failed ? MN_AUTHENTICATED_MAILBOX_ERROR_AUTHENTICATION : MN_AUTHENTICATED_MAILBOX_ERROR_OTHER;
    
    g_object_unref(session);

    return result;
  }}
#line 311 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

#line 116 "mn-gmail-mailbox.gob"
static SoupUri * 
mn_gmail_mailbox_soup_uri_new (const char * text_uri, GError ** err)
#line 318 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::soup_uri_new"
#line 116 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (text_uri != NULL, (SoupUri * )0);
#line 323 "mn-gmail-mailbox.c"
{
#line 118 "mn-gmail-mailbox.gob"
	
    SoupUri *uri;

    uri = soup_uri_new(text_uri);
    if (! uri)
      g_set_error(err, 0, 0, _("unable to parse URI \"%s\""), text_uri);

    return uri;
  }}
#line 335 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 128 "mn-gmail-mailbox.gob"
static char * 
mn_gmail_mailbox_get (MNGmailMailbox * self, SoupSession * session, const char * text_uri, GError ** err)
#line 341 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::get"
#line 128 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (self != NULL, (char * )0);
#line 128 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (MN_IS_GMAIL_MAILBOX (self), (char * )0);
#line 128 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (session != NULL, (char * )0);
#line 128 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (SOUP_IS_SESSION (session), (char * )0);
#line 128 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (text_uri != NULL, (char * )0);
#line 354 "mn-gmail-mailbox.c"
{
#line 133 "mn-gmail-mailbox.gob"
	
    SoupUri *uri;
    char *body;

    uri = self_soup_uri_new(text_uri, err);
    if (! uri)
      return NULL;

    body = self_get_from_uri(self, session, uri, err);
    soup_uri_free(uri);

    return body;
  }}
#line 370 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 147 "mn-gmail-mailbox.gob"
static char * 
mn_gmail_mailbox_get_from_uri (MNGmailMailbox * self, SoupSession * session, SoupUri * uri, GError ** err)
#line 376 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::get_from_uri"
#line 147 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (self != NULL, (char * )0);
#line 147 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (MN_IS_GMAIL_MAILBOX (self), (char * )0);
#line 147 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (session != NULL, (char * )0);
#line 147 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (SOUP_IS_SESSION (session), (char * )0);
#line 147 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (uri != NULL, (char * )0);
#line 389 "mn-gmail-mailbox.c"
{
#line 152 "mn-gmail-mailbox.gob"
	
    SoupMessage *message;
    char *cookie;
    char *body = NULL;

    message = soup_message_new_from_uri(SOUP_METHOD_GET, uri);
    soup_message_add_header(message->request_headers, "User-Agent", "Mail Notification");

    cookie = self_build_cookie(self);
    if (cookie)
      {
	soup_message_add_header(message->request_headers, "Cookie", cookie);
	g_free(cookie);
      }
    
    self_dump_request(self, message);
    soup_session_send_message(session, message);

    if (SOUP_STATUS_IS_SUCCESSFUL(message->status_code))
      {
	body = g_strndup(message->response.body, message->response.length);
	self_dump_response(self, message, body);
	self_update_cookies(self, message);
      }
    else
      g_set_error(err, 0, 0, _("unable to transfer data: %s"), message->reason_phrase);

    g_object_unref(message);
    
    return body;
  }}
#line 423 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 184 "mn-gmail-mailbox.gob"
static void 
mn_gmail_mailbox_dump_request (MNGmailMailbox * self, SoupMessage * message)
#line 429 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::dump_request"
#line 184 "mn-gmail-mailbox.gob"
	g_return_if_fail (self != NULL);
#line 184 "mn-gmail-mailbox.gob"
	g_return_if_fail (MN_IS_GMAIL_MAILBOX (self));
#line 184 "mn-gmail-mailbox.gob"
	g_return_if_fail (message != NULL);
#line 184 "mn-gmail-mailbox.gob"
	g_return_if_fail (SOUP_IS_MESSAGE (message));
#line 440 "mn-gmail-mailbox.c"
{
#line 186 "mn-gmail-mailbox.gob"
	
    const SoupUri *suri;
    char *uri;

    suri = soup_message_get_uri(message);
    uri = soup_uri_to_string(suri, FALSE);
    mn_mailbox_notice(MN_MAILBOX(self), "> GET %s", uri);
    g_free(uri);

    soup_message_foreach_header(message->request_headers, self_dump_request_cb, self);
  }}
#line 454 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 198 "mn-gmail-mailbox.gob"
static void 
mn_gmail_mailbox_dump_request_cb (gpointer key, gpointer value, gpointer user_data)
#line 460 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::dump_request_cb"
#line 198 "mn-gmail-mailbox.gob"
	g_return_if_fail (key != NULL);
#line 198 "mn-gmail-mailbox.gob"
	g_return_if_fail (value != NULL);
#line 198 "mn-gmail-mailbox.gob"
	g_return_if_fail (user_data != NULL);
#line 469 "mn-gmail-mailbox.c"
{
#line 202 "mn-gmail-mailbox.gob"
	
    Self *self = user_data;
    const char *header_name = key;
    const char *header_value = value;

    mn_mailbox_notice(MN_MAILBOX(self), "> %s: %s", header_name, header_value);
  }}
#line 479 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 210 "mn-gmail-mailbox.gob"
static void 
mn_gmail_mailbox_dump_response (MNGmailMailbox * self, SoupMessage * message, const char * body)
#line 485 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::dump_response"
#line 210 "mn-gmail-mailbox.gob"
	g_return_if_fail (self != NULL);
#line 210 "mn-gmail-mailbox.gob"
	g_return_if_fail (MN_IS_GMAIL_MAILBOX (self));
#line 210 "mn-gmail-mailbox.gob"
	g_return_if_fail (message != NULL);
#line 210 "mn-gmail-mailbox.gob"
	g_return_if_fail (SOUP_IS_MESSAGE (message));
#line 210 "mn-gmail-mailbox.gob"
	g_return_if_fail (body != NULL);
#line 498 "mn-gmail-mailbox.c"
{
#line 214 "mn-gmail-mailbox.gob"
	
    char **lines;
    int i;
    
    soup_message_foreach_header(message->response_headers, self_dump_response_cb, self);

    lines = g_strsplit(body, "\n", 0);
    for (i = 0; lines[i]; i++)
      mn_mailbox_notice(MN_MAILBOX(self), "< %s", lines[i]);
    g_strfreev(lines);
  }}
#line 512 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 226 "mn-gmail-mailbox.gob"
static void 
mn_gmail_mailbox_dump_response_cb (gpointer key, gpointer value, gpointer user_data)
#line 518 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::dump_response_cb"
#line 226 "mn-gmail-mailbox.gob"
	g_return_if_fail (key != NULL);
#line 226 "mn-gmail-mailbox.gob"
	g_return_if_fail (value != NULL);
#line 226 "mn-gmail-mailbox.gob"
	g_return_if_fail (user_data != NULL);
#line 527 "mn-gmail-mailbox.c"
{
#line 230 "mn-gmail-mailbox.gob"
	
    Self *self = user_data;
    const char *header_name = key;
    const char *header_value = value;

    mn_mailbox_notice(MN_MAILBOX(self), "< %s: %s", header_name, header_value);
  }}
#line 537 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

/**
 * mn_gmail_mailbox_get_substring:
 * @str: the string to search
 * @pre: the start of the substring
 * @post: the end of the substring
 * @include_pre: whether to include @pre in the result or not
 * @include_post: whether to include @post in the result or not
 *
 * Gets the first substring of @str included between @pre and @post.
 *
 * Return value: a newly allocated string containing the substring
 *               or %NULL if not found.
 **/
#line 251 "mn-gmail-mailbox.gob"
static char * 
mn_gmail_mailbox_get_substring (const char * str, const char * pre, const char * post, gboolean include_pre, gboolean include_post)
#line 556 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::get_substring"
#line 251 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (str != NULL, (char * )0);
#line 251 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (pre != NULL, (char * )0);
#line 251 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (post != NULL, (char * )0);
#line 565 "mn-gmail-mailbox.c"
{
#line 257 "mn-gmail-mailbox.gob"
	
    char *sub = NULL;
    char *pre_loc;
    
    pre_loc = strstr(str, pre);
    if (pre_loc)
      {
	char *after_pre_loc;
	char *post_loc;
	
	after_pre_loc = pre_loc + strlen(pre);
	post_loc = strstr(after_pre_loc, post);
	if (post_loc)
	  {
	    char *start = include_pre ? pre_loc : after_pre_loc;
	    char *end = include_post ? post_loc + strlen(post) : post_loc;
	    
	    sub = g_strndup(start, end - start);
	  }
      }
    
    return sub;
  }}
#line 591 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 281 "mn-gmail-mailbox.gob"
static gboolean 
mn_gmail_mailbox_login (MNGmailMailbox * self, SoupSession * session, gboolean * authentication_failed, GError ** err)
#line 597 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::login"
#line 281 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (self != NULL, (gboolean )0);
#line 281 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (MN_IS_GMAIL_MAILBOX (self), (gboolean )0);
#line 281 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (session != NULL, (gboolean )0);
#line 281 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (SOUP_IS_SESSION (session), (gboolean )0);
#line 281 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (authentication_failed != NULL, (gboolean )0);
#line 610 "mn-gmail-mailbox.c"
{
#line 286 "mn-gmail-mailbox.gob"
	
    char *text_uri;
    char *escaped_username;
    char *escaped_password;
    char *body;
    char *next_location;
    char *query;
    SoupUri *uri;
    gboolean success;

    g_return_val_if_fail(selfp->logged_in == FALSE, FALSE);
    
    mn_mailbox_notice(MN_MAILBOX(self), _("logging in"));
    *authentication_failed = FALSE;

    if (selfp->cookies)
      g_hash_table_destroy(selfp->cookies);
    selfp->cookies = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

#define EXTRA_CHARS ";/?:@&=+$,"/* taken from RFC 2396 2.2 */
    escaped_username = soup_uri_encode(MN_MAILBOX(self)->uri->username, EXTRA_CHARS);
    escaped_password = soup_uri_encode(MN_AUTHENTICATED_MAILBOX(self)->password, EXTRA_CHARS);
    text_uri = g_strdup_printf("https://www.google.com/accounts/ServiceLoginBoxAuth?service=mail&continue=http://gmail.google.com/gmail&Email=%s&Passwd=%s", escaped_username, escaped_password);
    g_free(escaped_username);
    g_free(escaped_password);

    body = self_get(self, session, text_uri, err);
    g_free(text_uri);

    if (! body)
      return FALSE;

    next_location = self_get_substring(body, "top.location = \"", "\"", FALSE, FALSE);
    g_free(body);

    if (! next_location)
      {
	*authentication_failed = TRUE;
	goto failed;
      }

    /*
     * The following kludges work around
     * http://bugzilla.ximian.com/show_bug.cgi?id=66516.
     *
     * FIXME: remove it and bump libsoup version requirement when the
     * bug is fixed.
     */

    query = strchr(next_location, '?');
    if (query)
      *query = 0;

    text_uri = g_strdup_printf("https://www.google.com/accounts/%s", next_location);
    g_free(next_location);

    uri = self_soup_uri_new(text_uri, err);
    g_free(text_uri);

    if (! uri)
      return FALSE;

    if (query)
      {
	uri->query = g_strdup(query + 1);
	uri->broken_encoding = TRUE;
      }

    body = self_get_from_uri(self, session, uri, err);
    soup_uri_free(uri);

    if (! body)
      return FALSE;

    next_location = self_get_substring(body, "location.replace(\"", "\")", FALSE, FALSE);
    g_free(body);

    if (! next_location)
      goto failed;

    body = self_get(self, session, next_location, err);
    g_free(next_location);

    success = strstr(body, "frame name=js src=/gmail") != NULL;
    g_free(body);

    if (success)
      return TRUE;
    
  failed:
    g_set_error(err, 0, 0, _("login failed"));
    return FALSE;
  }}
#line 706 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 380 "mn-gmail-mailbox.gob"
static void 
mn_gmail_mailbox_update_cookies (MNGmailMailbox * self, SoupMessage * message)
#line 712 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::update_cookies"
#line 380 "mn-gmail-mailbox.gob"
	g_return_if_fail (self != NULL);
#line 380 "mn-gmail-mailbox.gob"
	g_return_if_fail (MN_IS_GMAIL_MAILBOX (self));
#line 380 "mn-gmail-mailbox.gob"
	g_return_if_fail (message != NULL);
#line 380 "mn-gmail-mailbox.gob"
	g_return_if_fail (SOUP_IS_MESSAGE (message));
#line 723 "mn-gmail-mailbox.c"
{
#line 382 "mn-gmail-mailbox.gob"
	
    const GSList *set_cookie_headers;
    const GSList *l;
    
    set_cookie_headers = soup_message_get_header_list(message->response_headers, "Set-Cookie");
    MN_LIST_FOREACH(l, set_cookie_headers)
      {
	const char *value = l->data;
	char *equal;

	equal = strchr(value, '=');
	if (equal && equal - value > 0)
	  {
	    char *end;
	    char *cookie_name;
	    char *cookie_value;

	    end = strchr(equal, ';');

	    cookie_name = g_strndup(value, equal - value);
	    cookie_value = end
	      ? g_strndup(equal + 1, end - equal - 1)
	      : g_strdup(equal + 1);

	    g_hash_table_insert(selfp->cookies, cookie_name, cookie_value);
	  }
      }
  }}
#line 754 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 411 "mn-gmail-mailbox.gob"
static char * 
mn_gmail_mailbox_build_cookie (MNGmailMailbox * self)
#line 760 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::build_cookie"
#line 411 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (self != NULL, (char * )0);
#line 411 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (MN_IS_GMAIL_MAILBOX (self), (char * )0);
#line 767 "mn-gmail-mailbox.c"
{
#line 413 "mn-gmail-mailbox.gob"
	
    GString *cookie;
    char *str;

    cookie = g_string_new(NULL);
    g_hash_table_foreach(selfp->cookies, self_build_cookie_cb, cookie);

    if (*cookie->str)
      str = g_string_free(cookie, FALSE);
    else
      {
	str = NULL;
	g_string_free(cookie, TRUE);
      }

    return str;
  }}
#line 787 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 431 "mn-gmail-mailbox.gob"
static void 
mn_gmail_mailbox_build_cookie_cb (gpointer key, gpointer value, gpointer user_data)
#line 793 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::build_cookie_cb"
{
#line 433 "mn-gmail-mailbox.gob"
	
    GString *cookie = user_data;

    if (*cookie->str)
      g_string_append(cookie, "; ");

    g_string_append_printf(cookie, "%s=%s", (const char *) key, (const char *) value);
  }}
#line 806 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 442 "mn-gmail-mailbox.gob"
static gboolean 
mn_gmail_mailbox_check (MNGmailMailbox * self, SoupSession * session, GError ** err)
#line 812 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::check"
#line 442 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (self != NULL, (gboolean )0);
#line 442 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (MN_IS_GMAIL_MAILBOX (self), (gboolean )0);
#line 442 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (session != NULL, (gboolean )0);
#line 442 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (SOUP_IS_SESSION (session), (gboolean )0);
#line 823 "mn-gmail-mailbox.c"
{
#line 444 "mn-gmail-mailbox.gob"
	
    char *body;
    char *work;
    gboolean status = FALSE;

    g_return_val_if_fail(selfp->logged_in == TRUE, FALSE);

    mn_mailbox_notice(MN_MAILBOX(self), _("searching for unread mail"));

    body = self_get(self, session, "https://gmail.google.com/gmail?search=adv&as_subset=unread&view=tl&start=0", err);
    if (! body)
      return FALSE;

    work = strstr(body, "Search results for: is:unread\",\"");
    if (work)
      {
	GSList *messages = NULL;

	work += 32;

	while ((work = strstr(work, "D([")))
	  {
	    GSList *arg_list;

	    work += 2;
	    if (self_js_get_arg_list(&work, &arg_list) && arg_list)
	      {
		GSList *l;

		MN_LIST_FOREACH(l, arg_list->next)
		  self_append_message(self, &messages, l->data);

		self_js_arg_list_free(arg_list);
	      }
	  }
	
	GDK_THREADS_ENTER();
	mn_mailbox_set_has_new(MN_MAILBOX(self), messages != NULL);
	mn_mailbox_set_messages(MN_MAILBOX(self), messages);
	gdk_flush();
	GDK_THREADS_LEAVE();

	mn_g_object_slist_free(messages);
	status = TRUE;
      }
    else
      g_set_error(err, 0, 0, _("unable to parse Gmail data"));

    g_free(body);

    return status;
  }}
#line 878 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 497 "mn-gmail-mailbox.gob"
static void 
mn_gmail_mailbox_append_message (MNGmailMailbox * self, GSList ** messages, JSArg * arg)
#line 884 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::append_message"
#line 497 "mn-gmail-mailbox.gob"
	g_return_if_fail (self != NULL);
#line 497 "mn-gmail-mailbox.gob"
	g_return_if_fail (MN_IS_GMAIL_MAILBOX (self));
#line 497 "mn-gmail-mailbox.gob"
	g_return_if_fail (messages != NULL);
#line 497 "mn-gmail-mailbox.gob"
	g_return_if_fail (arg != NULL);
#line 895 "mn-gmail-mailbox.c"
{
#line 501 "mn-gmail-mailbox.gob"
	
    char *date_field;
    char *sender_field;
    char *subject_field;
    GSList *labels_list;
    GSList *l;
    gboolean in_inbox = FALSE;

    char *date;
    char *sender_email;
    char *sender_name;
    char *subject;

    time_t sent_time = 0;
    char *from = NULL;
    char *expanded_subject = NULL;

    if (arg->type != JS_ARG_LIST
	|| ! self_js_arg_list_scan(arg->value.list,
				   3, JS_ARG_STRING, &date_field,
				   4, JS_ARG_STRING, &sender_field,
				   6, JS_ARG_STRING, &subject_field,
				   8, JS_ARG_LIST, &labels_list,
				   -1))
      return;
				  
    MN_LIST_FOREACH(l, labels_list)
      {
	JSArg *label = l->data;

	if (label->type == JS_ARG_STRING && ! strcmp(label->value.string, "^i"))
	  {
	    in_inbox = TRUE;
	    break;
	  }
      }

    if (! in_inbox)
      return;
    
    date = self_get_substring(date_field, "<b>", "</b>", FALSE, FALSE);
    sender_email = self_get_substring(sender_field, "<span id='_user_", "'", FALSE, FALSE);
    sender_name = self_get_substring(sender_field, "'><b>", "</b>", FALSE, FALSE);
    subject = self_get_substring(subject_field, "<b>", "</b>", FALSE, FALSE);
		    
    if (date)
      {
	sent_time = self_parse_date(date);
	g_free(date);
      }
    
    if (sender_name && sender_email)
      {
	char *tmp;
	
	tmp = g_strdup_printf("%s <%s>", sender_name, sender_email);
	from = mn_sgml_ref_expand(tmp);
	g_free(tmp);
      }
    else if (sender_name)
      from = mn_sgml_ref_expand(sender_name);
    else if (sender_email)
      from = mn_sgml_ref_expand(sender_email);
    
    g_free(sender_email);
    g_free(sender_name);
    
    if (subject)
      {
	if (strcmp(subject, "(no subject)"))
	  expanded_subject = mn_sgml_ref_expand(subject);
	g_free(subject);
      }
    
    *messages = g_slist_append(*messages, mn_message_new(MN_MAILBOX(self)->uri,
							 NULL,
							 sent_time,
							 NULL,
							 from,
							 expanded_subject));
    
    g_free(from);
    g_free(expanded_subject);
  }}
#line 982 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 586 "mn-gmail-mailbox.gob"
static gboolean 
mn_gmail_mailbox_js_get_arg_list (char ** str, GSList ** list)
#line 988 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::js_get_arg_list"
#line 586 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (str != NULL, (gboolean )0);
#line 586 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (list != NULL, (gboolean )0);
#line 995 "mn-gmail-mailbox.c"
{
#line 588 "mn-gmail-mailbox.gob"
	
    GSList *our_list = NULL;
    gboolean done = FALSE;
    gboolean in_literal = FALSE;	/* a string enclosed in double quotes */
    gboolean quoted = FALSE;		/* the next character is quoted with a backslash */
    JSArg *arg = NULL;
    GString *string;

    g_return_val_if_fail(*str != NULL, FALSE);
    g_return_val_if_fail(**str == '[', FALSE);

    (*str)++;			/* skip the initial open bracket */
    string = g_string_new(NULL);

    while (**str && ! done)
      {
	gboolean next = TRUE;

	if (in_literal)
	  switch (**str)
	    {
	    case '\\':
	      quoted = TRUE;
	      break;

	    case '"':
	      if (! quoted)
		{
		  in_literal = FALSE;
		  break;
		}
	      /* else pass through */
	      
	    default:
	      quoted = FALSE;
	      g_string_append_c(string, **str);
	    }
	else
	  switch (**str)
	    {
	    case '[':
	      {
		GSList *sub_list;

		if (arg || ! self_js_get_arg_list(str, &sub_list))
		  goto end;	/* parse error */

		arg = self_js_arg_new_list(sub_list);
		next = FALSE;
		break;
	      }

	    case ']':
	      done = TRUE;
	      /* fall through */
	    
	    case ',':
	      if (! arg)
		arg = self_js_arg_new_string(string->str);
	      
	      our_list = g_slist_append(our_list, arg);

	      g_string_truncate(string, 0);
	      arg = NULL;
	      break;

	    case '"':
	      in_literal = TRUE;
	      break;
	      
	    default:
	      g_string_append_c(string, **str);
	    }

	if (next)
	  (*str)++;
      }

  end:
    if (arg)
      self_js_arg_free(arg);
    g_string_free(string, TRUE);

    if (done)
      {
	*list = our_list;
	return TRUE;
      }
    else
      {
	self_js_arg_list_free(our_list);
	return FALSE;
      }
  }}
#line 1092 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 683 "mn-gmail-mailbox.gob"
static gboolean 
mn_gmail_mailbox_js_arg_list_scan (GSList * list, ...)
#line 1098 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::js_arg_list_scan"
{
#line 685 "mn-gmail-mailbox.gob"
	
    va_list args;
    int position;
    gboolean status = TRUE;

    va_start(args, list);

    while ((position = va_arg(args, int)) != -1)
      {
	JSArgType type = va_arg(args, JSArgType);
	JSArg *arg = g_slist_nth_data(list, position);
	
	if (! arg)
	  {
	    status = FALSE;
	    break;
	  }

	switch (type)
	  {
	  case JS_ARG_STRING:
	    {
	      char **str = va_arg(args, char **);
	      g_return_val_if_fail(str != NULL, FALSE);

	      *str = arg->value.string;
	      break;
	    }

	  case JS_ARG_LIST:
	    {
	      GSList **elem_list = va_arg(args, GSList **);
	      g_return_val_if_fail(elem_list != NULL, FALSE);

	      *elem_list = arg->value.list;
	      break;
	    }

	  default:
	    g_return_val_if_reached(FALSE);
	  }
      }

    va_end(args);

    return status;
  }}
#line 1150 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 733 "mn-gmail-mailbox.gob"
static void 
mn_gmail_mailbox_js_arg_list_free (GSList * list)
#line 1156 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::js_arg_list_free"
{
#line 735 "mn-gmail-mailbox.gob"
	
    eel_g_slist_free_deep_custom(list, (GFunc) self_js_arg_free, NULL);
  }}
#line 1164 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 739 "mn-gmail-mailbox.gob"
static JSArg * 
mn_gmail_mailbox_js_arg_new_string (const char * string)
#line 1170 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::js_arg_new_string"
#line 739 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (string != NULL, (JSArg * )0);
#line 1175 "mn-gmail-mailbox.c"
{
#line 741 "mn-gmail-mailbox.gob"
	
    JSArg *arg;

    arg = g_new(JSArg, 1);
    arg->type = JS_ARG_STRING;
    arg->value.string = g_strdup(string);

    return arg;
  }}
#line 1187 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 751 "mn-gmail-mailbox.gob"
static JSArg * 
mn_gmail_mailbox_js_arg_new_list (GSList * list)
#line 1193 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::js_arg_new_list"
{
#line 753 "mn-gmail-mailbox.gob"
	
    JSArg *arg;

    arg = g_new(JSArg, 1);
    arg->type = JS_ARG_LIST;
    arg->value.list = list;

    return arg;
  }}
#line 1207 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 763 "mn-gmail-mailbox.gob"
static void 
mn_gmail_mailbox_js_arg_free (JSArg * arg)
#line 1213 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::js_arg_free"
#line 763 "mn-gmail-mailbox.gob"
	g_return_if_fail (arg != NULL);
#line 1218 "mn-gmail-mailbox.c"
{
#line 765 "mn-gmail-mailbox.gob"
	
    switch (arg->type)
      {
      case JS_ARG_STRING:
	g_free(arg->value.string);
	break;

      case JS_ARG_LIST:
	self_js_arg_list_free(arg->value.list);
	break;

      default:
	g_return_if_reached();
      }

    g_free(arg);
  }}
#line 1238 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__

#line 783 "mn-gmail-mailbox.gob"
static time_t 
mn_gmail_mailbox_parse_date (const char * date)
#line 1244 "mn-gmail-mailbox.c"
{
#define __GOB_FUNCTION__ "MN:Gmail:Mailbox::parse_date"
#line 783 "mn-gmail-mailbox.gob"
	g_return_val_if_fail (date != NULL, (time_t )0);
#line 1249 "mn-gmail-mailbox.c"
{
#line 785 "mn-gmail-mailbox.gob"
	
    time_t t = 0;
#ifdef HAVE_TIMEGM
    time_t now;

    now = mn_time();
    if (now > 0)
      {
	int hours;
	int minutes;
	char ampm[3];
	char month_str[4];
	int day;
	struct tm *tm = NULL;
	
	/* convert now to UTC-7 (Gmail time) */
	now -= (7 * 3600);
	
	if (sscanf(date, "%d:%d%2s", &hours, &minutes, ampm) == 3)
	  {
	    tm = gmtime(&now);
	    tm->tm_hour = hours;
	    if (ampm[0] == 'p')
	      tm->tm_hour += 12;
	    tm->tm_min = minutes;
	    tm->tm_sec = 0;
	  }
	else if (sscanf(date, "%3s %d", month_str, &day) == 2)
	  {
	    const char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	    int i;
	    
	    for (i = 0; i < G_N_ELEMENTS(months); i++)
	      if (! strcmp(month_str, months[i]))
		{
		  tm = gmtime(&now);
		  tm->tm_mon = i;
		  tm->tm_mday = day;
		  tm->tm_hour = 0;
		  tm->tm_min = 0;
		  tm->tm_sec = 0;
		  
		  break;
		}
	  }

	if (tm)
	  t = timegm(tm) + (7 * 3600); /* add 7 hours, because Gmail time is UTC-7 */
      }
#endif /* HAVE_TIMEGM */

    return t;
  }}
#line 1305 "mn-gmail-mailbox.c"
#undef __GOB_FUNCTION__
