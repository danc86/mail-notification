/* Generated by GOB (v2.0.11) on Sun Oct 31 18:41:23 2004
   (do not edit directly) */

/* End world hunger, donate to the World Food Programme, http://www.wfp.org */

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 11

#define selfp (self->_priv)

#include "mn-uri.h"

#include "mn-uri-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */

#line 54 "mn-uri.gob"

#include "config.h"
#include <stdio.h>		/* required by stdlib.h on Darwin */
#include <stdlib.h>		/* required by sys/socket.h on Darwin */
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <glib/gi18n.h>
#include <libgnomevfs/gnome-vfs-utils.h>
#include <eel/eel.h>
#include "mn-util.h"

  typedef struct
  {
    const char		*name;

    MNURIType		type;
    MNURIFlags		flags;

    int			default_port;
    const char		*default_path;
  } SchemeInfo;
    
  static const SchemeInfo our_schemes[] = {
    { "pop",	MN_URI_POP,	0,		110,	NULL },
    { "pops",	MN_URI_POP,	MN_URI_SSL,	995,	NULL },
    { "imap",	MN_URI_IMAP,	0,		143,	"INBOX" },
    { "imaps",	MN_URI_IMAP,	MN_URI_SSL,	993,	"INBOX" },
    { "gmail",	MN_URI_GMAIL,	0,		-1,	NULL }
  };

#line 60 "mn-uri.c"
/* self casting macros */
#define SELF(x) MN_URI(x)
#define SELF_CONST(x) MN_URI_CONST(x)
#define IS_SELF(x) MN_IS_URI(x)
#define TYPE_SELF MN_TYPE_URI
#define SELF_CLASS(x) MN_URI_CLASS(x)

#define SELF_GET_CLASS(x) MN_URI_GET_CLASS(x)

/* self typedefs */
typedef MNURI Self;
typedef MNURIClass SelfClass;

/* here are local prototypes */
static void mn_uri_init (MNURI * o) G_GNUC_UNUSED;
static void mn_uri_class_init (MNURIClass * c) G_GNUC_UNUSED;
static gboolean mn_uri_is_ipv6_address (const char * hostname) G_GNUC_UNUSED;
static const SchemeInfo * mn_uri_get_scheme_info (const char * scheme) G_GNUC_UNUSED;
static void mn_uri_parse (MNURI * self) G_GNUC_UNUSED;
static void mn_uri_build (MNURI * self) G_GNUC_UNUSED;
static void mn_uri_set_human_readable (MNURI * self) G_GNUC_UNUSED;
static void mn_uri_post_init (MNURI * self) G_GNUC_UNUSED;

/* pointer to the class of our parent */
static GObjectClass *parent_class = NULL;

/* Short form macros */
#define self_is_ipv6_address mn_uri_is_ipv6_address
#define self_has_query mn_uri_has_query
#define self_get_system_mailbox mn_uri_get_system_mailbox
#define self_get_scheme_info mn_uri_get_scheme_info
#define self_get_default_port mn_uri_get_default_port
#define self_get_default_path mn_uri_get_default_path
#define self_parse mn_uri_parse
#define self_build mn_uri_build
#define self_set_human_readable mn_uri_set_human_readable
#define self_post_init mn_uri_post_init
#define self_new mn_uri_new
#define self_new_pop mn_uri_new_pop
#define self_new_imap mn_uri_new_imap
#define self_new_gmail mn_uri_new_gmail
GType
mn_uri_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (MNURIClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mn_uri_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (MNURI),
			0 /* n_preallocs */,
			(GInstanceInitFunc) mn_uri_init,
			NULL
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MNURI", &info, (GTypeFlags)0);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((MNURI *)g_object_new(mn_uri_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static MNURI * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static MNURI *
GET_NEW_VARG (const char *first, ...)
{
	MNURI *ret;
	va_list ap;
	va_start (ap, first);
	ret = (MNURI *)g_object_new_valist (mn_uri_get_type (), first, ap);
	va_end (ap);
	return ret;
}


static void
___dispose (GObject *obj_self)
{
#define __GOB_FUNCTION__ "MN:URI::dispose"
	MNURI *self G_GNUC_UNUSED = MN_URI (obj_self);
	if (G_OBJECT_CLASS (parent_class)->dispose) \
		(* G_OBJECT_CLASS (parent_class)->dispose) (obj_self);
#line 92 "mn-uri.gob"
	if(self->vfs) { gnome_vfs_uri_unref ((gpointer) self->vfs); self->vfs = NULL; }
#line 154 "mn-uri.c"
}
#undef __GOB_FUNCTION__


static void
___finalize(GObject *obj_self)
{
#define __GOB_FUNCTION__ "MN:URI::finalize"
	MNURI *self G_GNUC_UNUSED = MN_URI (obj_self);
	if(G_OBJECT_CLASS(parent_class)->finalize) \
		(* G_OBJECT_CLASS(parent_class)->finalize)(obj_self);
#line 91 "mn-uri.gob"
	if(self->text) { g_free ((gpointer) self->text); self->text = NULL; }
#line 168 "mn-uri.c"
#line 93 "mn-uri.gob"
	if(self->human_readable) { g_free ((gpointer) self->human_readable); self->human_readable = NULL; }
#line 171 "mn-uri.c"
#line 95 "mn-uri.gob"
	if(self->scheme) { g_free ((gpointer) self->scheme); self->scheme = NULL; }
#line 174 "mn-uri.c"
#line 96 "mn-uri.gob"
	if(self->username) { g_free ((gpointer) self->username); self->username = NULL; }
#line 177 "mn-uri.c"
#line 97 "mn-uri.gob"
	if(self->password) { g_free ((gpointer) self->password); self->password = NULL; }
#line 180 "mn-uri.c"
#line 98 "mn-uri.gob"
	if(self->authmech) { g_free ((gpointer) self->authmech); self->authmech = NULL; }
#line 183 "mn-uri.c"
#line 99 "mn-uri.gob"
	if(self->hostname) { g_free ((gpointer) self->hostname); self->hostname = NULL; }
#line 186 "mn-uri.c"
#line 101 "mn-uri.gob"
	if(self->path) { g_free ((gpointer) self->path); self->path = NULL; }
#line 189 "mn-uri.c"
#line 102 "mn-uri.gob"
	if(self->queries) { g_strfreev ((gpointer) self->queries); self->queries = NULL; }
#line 192 "mn-uri.c"
}
#undef __GOB_FUNCTION__

static void 
mn_uri_init (MNURI * o G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:URI::init"
#line 54 "mn-uri.gob"
	o->text = NULL;
#line 202 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->vfs = NULL;
#line 205 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->human_readable = NULL;
#line 208 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->scheme = NULL;
#line 211 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->username = NULL;
#line 214 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->password = NULL;
#line 217 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->authmech = NULL;
#line 220 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->hostname = NULL;
#line 223 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->port = -1;
#line 226 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->path = NULL;
#line 229 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->queries = NULL;
#line 232 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->type = MN_URI_UNKNOWN;
#line 235 "mn-uri.c"
#line 54 "mn-uri.gob"
	o->flags = 0;
#line 238 "mn-uri.c"
}
#undef __GOB_FUNCTION__
static void 
mn_uri_class_init (MNURIClass * c G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "MN:URI::class_init"
	GObjectClass *g_object_class G_GNUC_UNUSED = (GObjectClass*) c;

	parent_class = g_type_class_ref (G_TYPE_OBJECT);

	g_object_class->dispose = ___dispose;
	g_object_class->finalize = ___finalize;
}
#undef __GOB_FUNCTION__



#line 107 "mn-uri.gob"
static gboolean 
mn_uri_is_ipv6_address (const char * hostname)
#line 259 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::is_ipv6_address"
{
#line 109 "mn-uri.gob"
	
    char dummy[16];		/* sizeof(struct in6_addr) */

    g_return_val_if_fail(hostname != NULL, FALSE);

    return inet_pton(AF_INET6, hostname, dummy) == 1;
  }}
#line 271 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 117 "mn-uri.gob"
gboolean 
mn_uri_has_query (MNURI * self, const char * query)
#line 277 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::has_query"
#line 117 "mn-uri.gob"
	g_return_val_if_fail (self != NULL, (gboolean )0);
#line 117 "mn-uri.gob"
	g_return_val_if_fail (MN_IS_URI (self), (gboolean )0);
#line 117 "mn-uri.gob"
	g_return_val_if_fail (query != NULL, (gboolean )0);
#line 286 "mn-uri.c"
{
#line 119 "mn-uri.gob"
	
    return self->queries && eel_g_strv_find(self->queries, query) != -1;
  }}
#line 292 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 123 "mn-uri.gob"
const char * 
mn_uri_get_system_mailbox (void)
#line 298 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::get_system_mailbox"
{
#line 125 "mn-uri.gob"
	
    static char *global_uri = NULL;
    G_LOCK_DEFINE_STATIC(global_uri);
    const char *uri;

    G_LOCK(global_uri);
    if (! global_uri)
      {
	const char *mail = g_getenv("MAIL");
	if (mail)
	  global_uri = gnome_vfs_get_uri_from_local_path(mail);
      }
    uri = global_uri;
    G_UNLOCK(global_uri);

    return uri;
  }}
#line 320 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 143 "mn-uri.gob"
static const SchemeInfo * 
mn_uri_get_scheme_info (const char * scheme)
#line 326 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::get_scheme_info"
{
#line 145 "mn-uri.gob"
	
    int i;

    for (i = 0; i < G_N_ELEMENTS(our_schemes); i++)
      if (! strcmp(scheme, our_schemes[i].name))
	return &our_schemes[i];

    return NULL;
  }}
#line 340 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 155 "mn-uri.gob"
int 
mn_uri_get_default_port (const char * scheme)
#line 346 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::get_default_port"
#line 155 "mn-uri.gob"
	g_return_val_if_fail (scheme != NULL, (int )0);
#line 351 "mn-uri.c"
{
#line 157 "mn-uri.gob"
	
    const SchemeInfo *scheme_info;

    scheme_info = self_get_scheme_info(scheme);
    return scheme_info ? scheme_info->default_port : -1;
  }}
#line 360 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 164 "mn-uri.gob"
const char * 
mn_uri_get_default_path (const char * scheme)
#line 366 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::get_default_path"
#line 164 "mn-uri.gob"
	g_return_val_if_fail (scheme != NULL, (const char * )0);
#line 371 "mn-uri.c"
{
#line 166 "mn-uri.gob"
	
    const SchemeInfo *scheme_info;

    scheme_info = self_get_scheme_info(scheme);
    return scheme_info ? scheme_info->default_path : NULL;
  }}
#line 380 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 173 "mn-uri.gob"
static void 
mn_uri_parse (MNURI * self)
#line 386 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::parse"
#line 173 "mn-uri.gob"
	g_return_if_fail (self != NULL);
#line 173 "mn-uri.gob"
	g_return_if_fail (MN_IS_URI (self));
#line 393 "mn-uri.c"
{
#line 175 "mn-uri.gob"
	
    int len;
    int buflen;

    if (self->vfs || ! self->text) /* no need to parse, or already parsed */
      return;

    len = strlen(self->text);
    buflen = len + 1;

    {
      char *pat;
      char scheme_buf[buflen];
      char auth_buf[buflen];
      char location_buf[buflen];
      char username_buf[buflen];
      char password_buf[buflen];
      char authmech_buf[buflen];
      char hostname_buf[buflen];
      int _port;
      char path_buf[buflen];
      char queries_buf[buflen];
      gboolean has_location = FALSE;
      gboolean has_password = FALSE;
      gboolean has_authmech = FALSE;
      gboolean has_port = FALSE;
      gboolean has_path = FALSE;
      gboolean has_queries = FALSE;
      int n;

      /* split URI in 3 parts: scheme, auth and location */

      pat = g_strdup_printf("%%%i[^:]://%%%i[^@]@%%%is", len, len, len);
      n = sscanf(self->text, pat, scheme_buf, auth_buf, location_buf);
      g_free(pat);

      if (n >= 2)
	{
	  if (n == 3)
	    has_location = TRUE;
	}
      else
	return;			/* unparsable */

      /* split auth part in 3 subparts: username, password and authmech */

      /*
       * For backward compatibility with previous versions of Mail
       * Notification, we also support ;auth= (in lowercase).
       */
      
      pat = g_strdup_printf("%%%i[^:]:%%%i[^;];%%*1[aA]%%*1[uU]%%*1[tT]%%*1[hH]=%%%is", len, len, len);
      n = sscanf(auth_buf, pat, username_buf, password_buf, authmech_buf);
      g_free(pat);

      if (n >= 2)
	{
	  has_password = TRUE;
	  if (n == 3)
	    has_authmech = TRUE;
	}
      else
	{
	  pat = g_strdup_printf("%%%i[^;];%%*1[aA]%%*1[uU]%%*1[tT]%%*1[hH]=%%%is", len, len);
	  n = sscanf(auth_buf, pat, username_buf, authmech_buf);
	  g_free(pat);

	  if (n >= 1)
	    {
	      if (n == 2)
		has_authmech = TRUE;
	    }
	  else
	    return;		/* unparsable */
	}

      if (has_location)
	{
	  char hostport_buf[buflen];

	  /* split location part in 3 subparts: hostport, path and queries */

	  pat = g_strdup_printf("%%%i[^/]/%%%i[^?]?%%%is", len, len, len);
	  n = sscanf(location_buf, pat, hostport_buf, path_buf, queries_buf);
	  g_free(pat);

	  if (n >= 2)
	    {
	      has_path = TRUE;
	      if (n == 3)
		has_queries = TRUE;
	    }
	  else
	    {
	      pat = g_strdup_printf("%%%i[^?]?%%%is", len, len);
	      n = sscanf(location_buf, pat, hostport_buf, queries_buf);
	      g_free(pat);

	      if (n == 2)
		has_queries = TRUE;
	    }

	  /* split hostport in 2 subparts: host and port */

	  pat = g_strdup_printf("[%%%i[^]]]:%%u", len);
	  n = sscanf(hostport_buf, pat, hostname_buf, &_port);
	  g_free(pat);
	
	  if (n < 1)
	    {
	      pat = g_strdup_printf("%%%i[^:]:%%u", len);
	      n = sscanf(hostport_buf, pat, hostname_buf, &_port);
	      g_free(pat);
	    }
	
	  if (n == 2)
	    has_port = TRUE;
	}

      self->scheme = gnome_vfs_unescape_string(scheme_buf, NULL);
      self->username = gnome_vfs_unescape_string(username_buf, NULL);
      if (has_password)
	self->password = gnome_vfs_unescape_string(password_buf, NULL);
      if (has_authmech)
	self->authmech = gnome_vfs_unescape_string(authmech_buf, NULL);
      if (has_location)
	self->hostname = gnome_vfs_unescape_string(hostname_buf, NULL);
      self->port = has_port ? _port : self_get_default_port(self->scheme);
      self->path = has_path ? gnome_vfs_unescape_string(path_buf, NULL) : g_strdup(self_get_default_path(self->scheme));
      if (has_queries)
	{
	  int i;
	    
	  self->queries = g_strsplit(queries_buf, "&", 0);
	  for (i = 0; self->queries[i]; i++)
	    {
	      char *unescaped;

	      unescaped = gnome_vfs_unescape_string(self->queries[i], NULL);

	      g_free(self->queries[i]);
	      self->queries[i] = unescaped;
	    }
	}
    }
  }}
#line 542 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 322 "mn-uri.gob"
static void 
mn_uri_build (MNURI * self)
#line 548 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::build"
#line 322 "mn-uri.gob"
	g_return_if_fail (self != NULL);
#line 322 "mn-uri.gob"
	g_return_if_fail (MN_IS_URI (self));
#line 555 "mn-uri.c"
{
#line 324 "mn-uri.gob"
	
    char *canonical = NULL;

    if (self->vfs)
      {
	g_return_if_fail(self->text != NULL);
	canonical = gnome_vfs_make_uri_canonical(self->text);
      }
    else if (self->scheme)
      {
	GString *string;
	char *escaped;
	int i;
	
	g_return_if_fail(self->username != NULL);

	escaped = gnome_vfs_escape_string(self->scheme);
	string = g_string_new(escaped);
	g_free(escaped);

	escaped = gnome_vfs_escape_string(self->username);
	g_string_append_printf(string, "://%s", escaped);
	g_free(escaped);

	if (self->password)
	  {
	    escaped = gnome_vfs_escape_string(self->password);
	    g_string_append_printf(string, ":%s", escaped);
	    g_free(escaped);
	  }

	if (self->authmech)
	  {
	    escaped = gnome_vfs_escape_string(self->authmech);
	    g_string_append_printf(string, ";AUTH=%s", escaped);
	    g_free(escaped);
	  }

	if (self->hostname)
	  {
	    escaped = gnome_vfs_escape_host_and_path_string(self->hostname);
	    g_string_append_printf(string, self_is_ipv6_address(self->hostname) ? "@[%s]" : "@%s", escaped);
	    g_free(escaped);
	  }

	if (self->port >= 0 && self->port != self_get_default_port(self->scheme))
	  g_string_append_printf(string, ":%i", self->port);

	if (self->path)
	  {
	    const char *default_path;

	    default_path = self_get_default_path(self->scheme);
	    if (! default_path || strcmp(self->path, default_path))
	      {
		escaped = gnome_vfs_escape_path_string(self->path);
		g_string_append_printf(string, "/%s", escaped);
		g_free(escaped);
	      }
	  }

	if (self->queries)
	  for (i = 0; self->queries[i]; i++)
	    {
	      g_string_append_c(string, i == 0 ? '?' : '&');

	      escaped = gnome_vfs_escape_string(self->queries[i]);
	      g_string_append(string, escaped);
	      g_free(escaped);
	    }

	canonical = g_string_free(string, FALSE);
      }

    if (canonical)
      {
	g_free(self->text);
	self->text = canonical;
      }
  }}
#line 638 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 405 "mn-uri.gob"
static void 
mn_uri_set_human_readable (MNURI * self)
#line 644 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::set_human_readable"
#line 405 "mn-uri.gob"
	g_return_if_fail (self != NULL);
#line 405 "mn-uri.gob"
	g_return_if_fail (MN_IS_URI (self));
#line 651 "mn-uri.c"
{
#line 407 "mn-uri.gob"
	
    g_return_if_fail(self->human_readable == NULL);

    if (MN_URI_IS_SYSTEM_MAILBOX(self))
      self->human_readable = g_strdup(_("System Mailbox"));
    else if (self->vfs)
      {
	char *path;
	
	path = gnome_vfs_get_local_path_from_uri(self->text);
	if (path)
	  {
	    self->human_readable = g_filename_to_utf8(path, -1, NULL, NULL, NULL);
	    g_free(path);
	  }
	else
	  self->human_readable = gnome_vfs_uri_to_string(self->vfs, GNOME_VFS_URI_HIDE_PASSWORD);
      }
    else if (MN_URI_IS_POP(self))
      self->human_readable = g_strdup_printf("%s@%s", self->username, self->hostname);
    else if (MN_URI_IS_IMAP(self))
      {
	const char *default_path = self_get_default_path(self->scheme);
	self->human_readable = (self->path && (! default_path || strcmp(self->path, default_path)))
	  ? g_strdup_printf("%s@%s/%s", self->username, self->hostname, self->path)
	  : g_strdup_printf("%s@%s", self->username, self->hostname);
      }
    else if (MN_URI_IS_GMAIL(self))
      self->human_readable = g_strdup_printf("%s@gmail.com", self->username);
    else
      self->human_readable = g_strdup(self->text);
  }}
#line 686 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 440 "mn-uri.gob"
static void 
mn_uri_post_init (MNURI * self)
#line 692 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::post_init"
#line 440 "mn-uri.gob"
	g_return_if_fail (self != NULL);
#line 440 "mn-uri.gob"
	g_return_if_fail (MN_IS_URI (self));
#line 699 "mn-uri.c"
{
#line 442 "mn-uri.gob"
	
    const char *system_uri;

    self_parse(self);
    self_build(self);
    
    if (self->vfs)
      {
	self->type = MN_URI_VFS;
	if (gnome_vfs_uri_is_local(self->vfs))
	  self->flags |= MN_URI_LOCAL;
      }
    else if (self->scheme)
      {
	const SchemeInfo *scheme_info;

	scheme_info = self_get_scheme_info(self->scheme);
	if (scheme_info)
	  {
	    self->type = scheme_info->type;
	    self->flags |= scheme_info->flags;
	  }

	if ((MN_URI_IS_POP(self) && self_has_query(self, "STLS"))
	    || (MN_URI_IS_IMAP(self) && self_has_query(self, "STARTTLS")))
	  self->flags |= MN_URI_INBAND_SSL;
      }

    system_uri = self_get_system_mailbox();
    if (system_uri && ! strcmp(system_uri, self->text))
      self->flags |= MN_URI_SYSTEM_MAILBOX;

    self_set_human_readable(self);
  }}
#line 736 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 477 "mn-uri.gob"
MNURI * 
mn_uri_new (const char * text_uri)
#line 742 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::new"
#line 477 "mn-uri.gob"
	g_return_val_if_fail (text_uri != NULL, (MNURI * )0);
#line 747 "mn-uri.c"
{
#line 479 "mn-uri.gob"
	
    Self *self;
    char *scheme;

    self = GET_NEW;
    self->text = g_str_has_prefix(text_uri, "pop3:") /* also handle obsolete pop3 locators */
      ? g_strconcat("pop://", text_uri + 5, NULL)
      : g_strdup(text_uri);

    /* only use GnomeVFS if we can't handle it ourselves */
    scheme = gnome_vfs_get_uri_scheme(self->text);
    if (! scheme || ! self_get_scheme_info(scheme))
      self->vfs = gnome_vfs_uri_new(self->text);
    g_free(scheme);
    
    self_post_init(self);
    return self;
  }}
#line 768 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 498 "mn-uri.gob"
MNURI * 
mn_uri_new_pop (gboolean ssl, gboolean stls, const char * username, const char * password, const char * authmech, const char * hostname, int port)
#line 774 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::new_pop"
#line 498 "mn-uri.gob"
	g_return_val_if_fail (username != NULL, (MNURI * )0);
#line 498 "mn-uri.gob"
	g_return_val_if_fail (hostname != NULL, (MNURI * )0);
#line 781 "mn-uri.c"
{
#line 506 "mn-uri.gob"
	
    Self *self;

    self = GET_NEW;

    self->scheme = g_strdup(ssl ? "pops" : "pop");
    self->username = g_strdup(username);
    self->password = g_strdup(password);
    self->authmech = g_strdup(authmech);
    self->hostname = g_strdup(hostname);
    self->port = port;
    if (stls)
      self->queries = g_strsplit("STLS", " ", 0);
    
    self_post_init(self);
    return self;
  }}
#line 801 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 524 "mn-uri.gob"
MNURI * 
mn_uri_new_imap (gboolean ssl, gboolean starttls, const char * username, const char * password, const char * authmech, const char * hostname, int port, const char * mailbox)
#line 807 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::new_imap"
#line 524 "mn-uri.gob"
	g_return_val_if_fail (username != NULL, (MNURI * )0);
#line 524 "mn-uri.gob"
	g_return_val_if_fail (hostname != NULL, (MNURI * )0);
#line 524 "mn-uri.gob"
	g_return_val_if_fail (mailbox != NULL, (MNURI * )0);
#line 816 "mn-uri.c"
{
#line 533 "mn-uri.gob"
	
    Self *self;

    self = GET_NEW;

    self->scheme = g_strdup(ssl ? "imaps" : "imap");
    self->username = g_strdup(username);
    self->password = g_strdup(password);
    self->authmech = g_strdup(authmech);
    self->hostname = g_strdup(hostname);
    self->port = port;
    self->path = g_strdup(mailbox);
    if (starttls)
      self->queries = g_strsplit("STARTTLS", " ", 0);
    
    self_post_init(self);
    return self;
  }}
#line 837 "mn-uri.c"
#undef __GOB_FUNCTION__

#line 552 "mn-uri.gob"
MNURI * 
mn_uri_new_gmail (const char * username, const char * password)
#line 843 "mn-uri.c"
{
#define __GOB_FUNCTION__ "MN:URI::new_gmail"
#line 552 "mn-uri.gob"
	g_return_val_if_fail (username != NULL, (MNURI * )0);
#line 848 "mn-uri.c"
{
#line 555 "mn-uri.gob"
	
    Self *self;

    self = GET_NEW;

    self->scheme = g_strdup("gmail");
    self->username = g_strdup(username);
    self->password = g_strdup(password);
    
    self_post_init(self);
    return self;
  }}
#line 863 "mn-uri.c"
#undef __GOB_FUNCTION__
