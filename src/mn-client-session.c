/*
 * mn-client-session.c - a state machine for handling POP3 and IMAP
 * client sessions.
 *
 * The MNClientSession interface provides an abstract POP3 and IMAP
 * protocol client. The module handles the low-level client
 * functionality, such as connecting to a server, setting up SSL/TLS,
 * reading and writing data, and conducting a SASL authentication
 * exchange.
 *
 * MNClientSession contains no code which is specific to either POP3
 * or IMAP. It is the responsability of the caller to manage the POP3
 * or IMAP session, by parsing responses and switching to the
 * appropriate state depending on the context.
 *
 *
 *
 * Mail Notification
 * Copyright (C) 2003-2008 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"
#include <stdio.h>		/* required by stdlib.h on Darwin */
#include <stdlib.h>		/* required by sys/socket.h on Darwin */
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <eel/eel.h>
#if WITH_SSL
#include <openssl/err.h>
#include <openssl/x509v3.h>
#include "mn-ssl.h"
#endif /* WITH_SSL */
#if WITH_SASL
#include <sasl/sasl.h>
#include <sasl/saslutil.h>
#include "mn-sasl.h"
#endif /* WITH_SASL */
#include "mn-util.h"
#include "mn-client-session.h"

/*** cpp *********************************************************************/

#define READ_BUFSIZE			2048

/*** types *******************************************************************/

struct _MNClientSession
{
  const MNClientSessionState		*states;
  const MNClientSessionCallbacks	*callbacks;
  const char				*server;
  int					port;
  int					s;
  const MNClientSessionState		*state;
  GError				*error;
  MNClientSessionPrivate		*private;
  GByteArray				*input_buffer;
  unsigned int				bytes_to_remove;

#if WITH_SSL
  SSL					*ssl;
#endif

#if WITH_SASL
  sasl_conn_t				*sasl_conn;
  sasl_ssf_t				sasl_ssf;
  unsigned int				sasl_maxoutbuf;
#endif /* WITH_SASL */
};

/*** variables ***************************************************************/

#if WITH_SASL
static sasl_callback_t sasl_callbacks[] = {
  { SASL_CB_USER, NULL, NULL },
  { SASL_CB_AUTHNAME, NULL, NULL },
  { SASL_CB_PASS, NULL, NULL },

  { SASL_CB_LIST_END, NULL, NULL }
};
#endif /* WITH_SASL */

#ifndef HAVE_REENTRANT_RESOLVER
G_LOCK_DEFINE_STATIC(resolver);
#endif

/*** functions ***************************************************************/

static struct addrinfo *mn_client_session_resolve (MNClientSession *session);
static int mn_client_session_connect (MNClientSession *session, struct addrinfo *addrinfo);

#if WITH_SSL
static GSList *mn_client_session_ssl_get_certificate_servers (X509 *cert);
static gboolean mn_client_session_ssl_check_server_name (const char *user_name,
							 const char *cert_name);
static gboolean mn_client_session_ssl_check_server_name_from_list (const char *user_name,
								   const GSList *cert_names);
static char *mn_client_session_ssl_get_verify_error (MNClientSession *session,
						     X509 *cert);
static gboolean mn_client_session_ssl_verify (MNClientSession *session);
#endif

static int mn_client_session_enter_state (MNClientSession *session, int id);
static gboolean mn_client_session_handle_input (MNClientSession *session, const char *input);

static void mn_client_session_prepare_input_buffer (MNClientSession *session);

#if WITH_SASL
static int mn_client_session_write_base64 (MNClientSession *session,
					   const char *buf,
					   unsigned int len);
static gboolean mn_client_session_sasl_fill_interact (MNClientSession *session,
						      sasl_interact_t *interact,
						      const char *unknown_warning);
static char *mn_client_session_sasl_get_ip_port (const struct sockaddr *addr);
#endif /* WITH_SASL */

/*** implementation **********************************************************/

/**
 * mn_client_session_run:
 * @states: a %MN_CLIENT_SESSION_STATES_END-terminated array of
 *          %MNClientSessionState structures. One of the states must
 *          have the %MN_CLIENT_SESSION_INITIAL_STATE id.
 * @callbacks: a pointer to a %MNClientSessionCallbacks structure
 * @use_ssl: whether to establish a SSL/TLS connection or not
 * @server: the hostname, IPv4 address or IPv6 address to connect to
 * @port: the port to connect to
 * @private: an opaque pointer which will be passed to callbacks, or %NULL
 * @err: a location to report errors, or %NULL
 *
 * Runs the client session. After connecting to the server, the
 * %MN_CLIENT_SESSION_INITIAL_STATE state is entered.
 *
 * Return value: %TRUE on success, or %FALSE on failure (in such case
 * @err is set)
 **/
gboolean
mn_client_session_run (const MNClientSessionState *states,
		       const MNClientSessionCallbacks *callbacks,
#if WITH_SSL
		       gboolean use_ssl,
#endif
		       const char *server,
		       int port,
		       MNClientSessionPrivate *private,
		       GError **err)
{
  MNClientSession session;
  struct addrinfo *addrinfo;
  const char *line;

  g_return_val_if_fail(states != NULL, FALSE);
  g_return_val_if_fail(callbacks != NULL, FALSE);
  g_return_val_if_fail(callbacks->response_new != NULL, FALSE);
#if WITH_SSL
  g_return_val_if_fail(callbacks->ssl_trust_server != NULL, FALSE);
#endif
  g_return_val_if_fail(server != NULL, FALSE);

  memset(&session, 0, sizeof(session));
  session.states = states;
  session.callbacks = callbacks;
  session.server = server;
  session.port = port;
  session.private = private;

  addrinfo = mn_client_session_resolve(&session);
  if (! addrinfo)
    goto end;

  session.s = mn_client_session_connect(&session, addrinfo);
  freeaddrinfo(addrinfo);
  if (session.s < 0)
    goto end;

#if WITH_SSL
  if (use_ssl)
    {
      if (! mn_client_session_enable_ssl(&session))
	goto end;
    }
#endif /* WITH_SSL */

  mn_client_session_enter_state(&session, MN_CLIENT_SESSION_INITIAL_STATE);

  session.input_buffer = g_byte_array_new();
  while ((line = mn_client_session_read_line(&session)))
    if (! mn_client_session_handle_input(&session, line))
      break;
  g_byte_array_free(session.input_buffer, TRUE);

 end:
  if (session.s >= 0)
    while (close(session.s) < 0 && errno == EINTR);
#if WITH_SSL
  if (session.ssl)
    SSL_free(session.ssl);
#endif /* WITH_SSL */
#if WITH_SASL
  if (session.sasl_conn)
    sasl_dispose(&session.sasl_conn);
#endif /* WITH_SASL */
  if (session.error)
    {
      g_propagate_error(err, session.error);
      return FALSE;
    }
  else
    return TRUE;
}

static struct addrinfo *
mn_client_session_resolve (MNClientSession *session)
{
  char *servname;
  struct addrinfo hints;
  struct addrinfo *addrinfo;
  int status;

  g_return_val_if_fail(session != NULL, NULL);

  memset(&hints, 0, sizeof(hints));
#if WITH_IPV6
  hints.ai_family = PF_UNSPEC;
#else
  hints.ai_family = PF_INET;
#endif /* WITH_IPV6 */
  hints.ai_socktype = SOCK_STREAM;

  mn_client_session_notice(session, _("resolving %s"), session->server);

  servname = g_strdup_printf("%i", session->port);
#ifndef HAVE_REENTRANT_RESOLVER
  G_LOCK(resolver);
#endif
  status = getaddrinfo(session->server, servname, &hints, &addrinfo);
#ifndef HAVE_REENTRANT_RESOLVER
  G_UNLOCK(resolver);
#endif
  g_free(servname);

  if (status == 0)
    return addrinfo;
  else
    {
      mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to resolve %s: %s"), session->server, gai_strerror(status));
      return NULL;
    }
}

static int
mn_client_session_connect (MNClientSession *session, struct addrinfo *addrinfo)
{
  struct addrinfo *a;
  int n;

  g_return_val_if_fail(session != NULL, -1);
  g_return_val_if_fail(addrinfo != NULL, -1);

  /* iterate over addrinfo to find a working address (RFC 3484) */
  for (a = addrinfo, n = 1; a; a = a->ai_next, n++)
    {
      int status;
      int s;
      char buf[NI_MAXHOST];
      char *fail_str = NULL;
      const char *ip;

#ifndef HAVE_REENTRANT_RESOLVER
      G_LOCK(resolver);
#endif
      status = getnameinfo(a->ai_addr,
			   a->ai_addrlen,
			   buf,
			   sizeof(buf),
			   NULL,
			   0,
			   NI_NUMERICHOST);
#ifndef HAVE_REENTRANT_RESOLVER
      G_UNLOCK(resolver);
#endif

      if (status == 0)
	ip = buf;
      else
	{
	  fail_str = g_strdup_printf(_("network address #%i"), n);
	  ip = fail_str;

	  mn_client_session_warning(session, _("unable to convert network address #%i into textual form: %s"), n, gai_strerror(status));
	}

      if (a->ai_family == AF_INET)
	((struct sockaddr_in *) a->ai_addr)->sin_port = g_htons(session->port);
#if WITH_IPV6
      else if (a->ai_family == AF_INET6)
	((struct sockaddr_in6 *) a->ai_addr)->sin6_port = g_htons(session->port);
#endif /* WITH_IPV6 */
      else
	{
	  mn_client_session_notice(session, _("%s: unsupported address family"), ip);
	  goto failure;
	}

      s = socket(a->ai_family, SOCK_STREAM, 0);
      if (s < 0)
	{
	  mn_client_session_notice(session, _("%s: unable to create socket: %s"), ip, g_strerror(errno));
	  goto failure;
	}

      mn_client_session_notice(session, _("connecting to %s (%s) port %i"), session->server, ip, session->port);
      if (connect(s, a->ai_addr, a->ai_addrlen) < 0)
	{
	  mn_client_session_notice(session, _("unable to connect: %s"), g_strerror(errno));
	  while (close(s) < 0 && errno == EINTR);
	}
      else
	{
	  mn_client_session_notice(session, _("connected successfully"));
	  goto success;
	}

    failure:
      g_free(fail_str);
      continue;

    success:
      g_free(fail_str);
      return s;
    }

  /* if reached, we couldn't find a working address */
  mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to connect to %s"), session->server);
  return -1;
}

#if WITH_SSL
/**
 * mn_client_session_enable_ssl:
 * @session: a #MNClientSession
 *
 * Enables in-band SSL/TLS. Must not be used if the @use_ssl
 * mn_client_session_run() argument was %TRUE. If an error occurs,
 * mn_client_session_set_error() will be called on @session.
 *
 * Return value: %TRUE on success
 **/
gboolean
mn_client_session_enable_ssl (MNClientSession *session)
{
  SSL_CTX *ctx;
  GError *err = NULL;

  g_return_val_if_fail(session != NULL, FALSE);
  g_return_val_if_fail(session->ssl == NULL, FALSE);

  ctx = mn_ssl_init(&err);
  if (! ctx)
    {
      mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to initialize the OpenSSL library: %s"), err->message);
      g_error_free(err);
      return FALSE;
    }

  session->ssl = SSL_new(ctx);
  if (! session->ssl)
    {
      mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to create a SSL/TLS object: %s"), mn_ssl_get_error());
      return FALSE;
    }

  if (! SSL_set_fd(session->ssl, session->s))
    {
      mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to set the SSL/TLS file descriptor: %s"), mn_ssl_get_error());
      return FALSE;
    }

  if (SSL_connect(session->ssl) != 1)
    {
      mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to perform the SSL/TLS handshake: %s"), mn_ssl_get_error());
      return FALSE;
    }

  if (! mn_client_session_ssl_verify(session))
    {
      mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("untrusted server"));
      return FALSE;
    }

  mn_client_session_notice(session, _("a SSL/TLS layer is now active (%s, %s %i-bit)"),
			   SSL_get_version(session->ssl),
			   SSL_get_cipher(session->ssl),
			   SSL_get_cipher_bits(session->ssl, NULL));

  return TRUE;
}

/**
 * mn_client_session_ssl_get_certificate_servers():
 * @cert: the server certificate
 *
 * Returns the list of server names (commonName and subjectAltName)
 * contained in @cert.
 *
 * Return value: a newly-allocated list of UTF-8 server names. When no
 * longer used, the list must be freed with eel_g_slist_free_deep().
 **/
static GSList *
mn_client_session_ssl_get_certificate_servers (X509 *cert)
{
  GSList *servers = NULL;
  X509_NAME *subject;
  void *ext;

  g_return_val_if_fail(cert != NULL, NULL);

  /* append the commonName entries */

  subject = X509_get_subject_name(cert);
  if (subject)
    {
      int pos = -1;

      while (TRUE)
	{
	  X509_NAME_ENTRY *entry;
	  ASN1_STRING *data;
	  int len;
	  unsigned char *str;

	  pos = X509_NAME_get_index_by_NID(subject, NID_commonName, pos);
	  if (pos == -1)
	    break;

	  entry = X509_NAME_get_entry(subject, pos);
	  if (! entry)
	    continue;

	  data = X509_NAME_ENTRY_get_data(entry);
	  if (! data)
	    continue;

	  len = ASN1_STRING_to_UTF8(&str, data);
	  if (len < 0)
	    continue;

	  g_assert(g_utf8_validate(str, len, NULL));

	  servers = g_slist_append(servers, g_strndup(str, len));
	  OPENSSL_free(str);
	}
    }

  /*
   * RFC 3501 11.1: "If a subjectAltName extension of type dNSName is
   * present in the certificate, it SHOULD be used as the source of
   * the server's identity."
   */

  ext = X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL);
  if (ext)
    {
      int count;
      int i;

      count = sk_GENERAL_NAME_num(ext);
      for (i = 0; i < count; i++)
	{
	  GENERAL_NAME *name;

	  name = sk_GENERAL_NAME_value(ext, i);
	  if (name
	      && name->type == GEN_DNS
	      && name->d.ia5->data
	      && g_utf8_validate(name->d.ia5->data, -1, NULL))
	    servers = g_slist_append(servers, g_strdup(name->d.ia5->data));
	}
    }

  return servers;
}

static gboolean
mn_client_session_ssl_check_server_name (const char *user_name,
					 const char *cert_name)
{
  g_return_val_if_fail(user_name != NULL, FALSE);
  g_return_val_if_fail(cert_name != NULL, FALSE);

  /*
   * RFC 3501 11.1: "A "*" wildcard character MAY be used as the
   * left-most name component in the certificate. For example,
   * *.example.com would match a.example.com, foo.example.com,
   * etc. but would not match example.com."
   */

  if (g_str_has_prefix(cert_name, "*."))
    {
      const char *domain = cert_name + 1;

      return mn_utf8_str_case_has_suffix(user_name, domain) && strlen(user_name) > strlen(domain);
    }
  else
    return ! mn_utf8_strcasecmp(user_name, cert_name);
}

static gboolean
mn_client_session_ssl_check_server_name_from_list (const char *user_name,
						   const GSList *cert_names)
{
  const GSList *l;

  g_return_val_if_fail(user_name != NULL, FALSE);

  MN_LIST_FOREACH(l, cert_names)
    {
      const char *cert_name = l->data;

      if (mn_client_session_ssl_check_server_name(user_name, cert_name))
	return TRUE;
    }

  return FALSE;
}

static char *
mn_client_session_ssl_get_verify_error (MNClientSession *session, X509 *cert)
{
  long verify_result;
  GSList *servers;
  char *error = NULL;

  g_return_val_if_fail(session != NULL, NULL);
  g_return_val_if_fail(session->ssl != NULL, NULL);
  g_return_val_if_fail(cert != NULL, NULL);

  /* check the result of the OpenSSL verification */

  verify_result = SSL_get_verify_result(session->ssl);
  if (verify_result != X509_V_OK)
    {
      /*
       * X509_verify_cert_error_string() is thread-unsafe (it can
       * return a pointer to a temporary static buffer).
       */
      G_LOCK(mn_ssl);
      error = g_strdup(X509_verify_cert_error_string(verify_result));
      G_UNLOCK(mn_ssl);

      return error;
    }

  /*
   * Check if the user-provided server name matches one of the
   * certificate-provided server names. This is required for IMAP (RFC
   * 3501 11.1) and cannot hurt for POP3.
   */

  servers = mn_client_session_ssl_get_certificate_servers(cert);
  if (! servers)
    return g_strdup(_("server name not found in certificate"));

  if (! mn_client_session_ssl_check_server_name_from_list(session->server, servers))
    {
      if (g_slist_length(servers) == 1)
	error = g_strdup_printf(_("user-provided server name \"%s\" does not match certificate-provided server name \"%s\""),
				session->server, (char *) servers->data);
      else
	{
	  GString *servers_comma_list;
	  GSList *l;

	  servers_comma_list = g_string_new(NULL);

	  MN_LIST_FOREACH(l, servers)
	    {
	      char *server = l->data;

	      if (l->next)
		g_string_append_printf(servers_comma_list, _("\"%s\", "), server);
	      else
		g_string_append_printf(servers_comma_list, _("\"%s\""), server);
	    }

	  error = g_strdup_printf(_("user-provided server name \"%s\" matches none of the certificate-provided server names %s"),
				  session->server, servers_comma_list->str);

	  g_string_free(servers_comma_list, TRUE);
	}
    }

  eel_g_slist_free_deep(servers);

  return error;
}

static gboolean
mn_client_session_ssl_verify (MNClientSession *session)
{
  X509 *cert;

  g_return_val_if_fail(session != NULL, FALSE);
  g_return_val_if_fail(session->ssl != NULL, FALSE);

  cert = SSL_get_peer_certificate(session->ssl);
  if (cert)
    {
      char *error;
      gboolean status = FALSE;

      error = mn_client_session_ssl_get_verify_error(session, cert);
      if (! error)
	status = TRUE;
      else
	{
	  unsigned char md5sum[16];
	  unsigned char fingerprint[40];
	  int md5len;
	  int i;
	  unsigned char *f;

	  /* calculate the MD5 hash of the raw certificate */
	  md5len = sizeof(md5sum);
	  X509_digest(cert, EVP_md5(), md5sum, &md5len);
	  for (i = 0, f = fingerprint; i < 16; i++, f += 3)
	    sprintf(f, "%.2x%c", md5sum[i], i != 15 ? ':' : '\0');

	  if (session->callbacks->ssl_trust_server(session,
						   session->server,
						   session->port,
						   fingerprint,
						   error,
						   session->private))
	    status = TRUE;

	  g_free(error);
	}

      X509_free(cert);

      return status;
    }
  else
    return session->callbacks->ssl_trust_server(session,
						session->server,
						session->port,
						NULL,
						NULL,
						session->private);
}
#endif /* WITH_SSL */

static int
mn_client_session_enter_state (MNClientSession *session, int id)
{
  int i;

  g_return_val_if_fail(session != NULL, 0);

  for (i = 0; session->states[i].id; i++)
    if (session->states[i].id == id)
      {
	session->state = &session->states[i];
	return session->state->enter_cb
	  ? session->state->enter_cb(session, session->private)
	  : MN_CLIENT_SESSION_RESULT_CONTINUE;
      }

  g_assert_not_reached();
  return 0;
}

static gboolean
mn_client_session_handle_input (MNClientSession *session, const char *input)
{
  MNClientSessionResponse *response;
  gboolean cont = TRUE;

  g_return_val_if_fail(session != NULL, FALSE);
  g_return_val_if_fail(input != NULL, FALSE);

  response = session->callbacks->response_new(session, input, session->private);
  if (response)
    {
      int result;

      g_assert(session->state->handle_cb != NULL);
      result = session->state->handle_cb(session, response, session->private);

    loop:
      switch (result)
	{
	case MN_CLIENT_SESSION_RESULT_CONTINUE:
	  break;

	case MN_CLIENT_SESSION_RESULT_BAD_RESPONSE_FOR_CONTEXT:
	  {
	    char *escaped;

	    escaped = mn_utf8_escape(input);
	    mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("response \"%s\" is not valid in current context"), escaped);
	    g_free(escaped);

	    cont = FALSE;
	  }
	  break;

	case MN_CLIENT_SESSION_RESULT_DISCONNECT:
	  cont = FALSE;
	  break;

	case 0:			/* assertion failed somewhere */
	  g_assert_not_reached();
	  break;

	default:
	  g_assert(result > 0);
	  result = mn_client_session_enter_state(session, result);
	  goto loop;
	}

      if (session->callbacks->response_free)
	session->callbacks->response_free(session, response, session->private);
    }
  else
    {
      char *escaped;

      escaped = mn_utf8_escape(input);
      mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to parse response \"%s\""), escaped);
      g_free(escaped);

      cont = FALSE;
    }

  return cont;
}

static void
mn_client_session_prepare_input_buffer (MNClientSession *session)
{
  g_return_if_fail(session != NULL);

  if (session->bytes_to_remove)
    {
      g_byte_array_remove_range(session->input_buffer, 0, session->bytes_to_remove);
      session->bytes_to_remove = 0;
    }
}

static gboolean
mn_client_session_fill_input_buffer (MNClientSession *session)
{
  char buf[READ_BUFSIZE];
  ssize_t bytes_read;
  const char *in = NULL;
  unsigned int inlen;

  g_return_val_if_fail(session != NULL, FALSE);

  if (session->callbacks->pre_read)
    session->callbacks->pre_read(session, session->private);

#if WITH_SSL
  if (session->ssl)
    bytes_read = SSL_read(session->ssl, buf, sizeof(buf));
  else
#endif /* WITH_SSL */
    do
      bytes_read = read(session->s, buf, sizeof(buf));
    while (bytes_read < 0 && errno == EINTR);

  if (session->callbacks->post_read)
    session->callbacks->post_read(session, session->private);

  if (bytes_read <= 0)
    {
#if WITH_SSL
      if (session->ssl)
	mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_CONNECTION_LOST, _("unable to read from server: %s"), mn_ssl_get_error());
      else
#endif /* WITH_SSL */
	{
	  if (bytes_read == 0)
	    mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_CONNECTION_LOST, _("unable to read from server: EOF"));
	  else
	    mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_CONNECTION_LOST, _("unable to read from server: %s"), g_strerror(errno));
	}
      return FALSE;
    }

#if WITH_SASL
  if (session->sasl_ssf)
    {
      if (sasl_decode(session->sasl_conn, buf, bytes_read, &in, &inlen) != SASL_OK)
	{
	  mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to decode data using SASL: %s"), sasl_errdetail(session->sasl_conn));
	  return FALSE;
	}
    }
#endif /* WITH_SASL */

  if (! in)
    {
      in = buf;
      inlen = bytes_read;
    }

  g_byte_array_append(session->input_buffer, in, inlen);
  return TRUE;
}

/**
 * mn_client_session_read:
 * @session: a #MNClientSession object to read from
 * @nbytes: the number of bytes to read
 *
 * Reads exactly @nbytes from @session. If an error occurs,
 * mn_client_session_set_error() will be called on @session.
 *
 * Return value: a pointer to a buffer containing @nbytes on success,
 * %NULL on failure. The pointer will be valid until the next call to
 * mn_client_session_read() or mn_client_session_read_line().
 **/
gconstpointer
mn_client_session_read (MNClientSession *session, unsigned int nbytes)
{
  char *str;

  g_return_val_if_fail(session != NULL, FALSE);
  g_return_val_if_fail(session->input_buffer != NULL, FALSE);
  g_return_val_if_fail(nbytes >= 0, FALSE);

  mn_client_session_prepare_input_buffer(session);

  while (session->input_buffer->len < nbytes)
    if (! mn_client_session_fill_input_buffer(session))
      return FALSE;

  session->bytes_to_remove = nbytes;

  str = g_strndup(session->input_buffer->data, nbytes);
  /* g_log() escapes unsafe and non UTF-8 characters, so this is safe */
  mn_client_session_notice(session, "< %s", str);
  g_free(str);

  return session->input_buffer->data;
}

/**
 * mn_client_session_read_line:
 * @session: a #MNClientSession object to read from
 *
 * Reads a crlf-terminated line from @session. If an error occurs,
 * mn_client_session_set_error() will be called on @session.
 *
 * Return value: the line read on success, %NULL on failure. The
 * pointer will be valid until the next call to
 * mn_client_session_read() or mn_client_session_read_line().
 **/
const char *
mn_client_session_read_line (MNClientSession *session)
{
  char *terminator;
  const char *line;

  g_return_val_if_fail(session != NULL, NULL);
  g_return_val_if_fail(session->input_buffer != NULL, NULL);

  mn_client_session_prepare_input_buffer(session);

  while (! (session->input_buffer->data
	    && (terminator = g_strstr_len(session->input_buffer->data,
					  session->input_buffer->len,
					  "\r\n"))))
    if (! mn_client_session_fill_input_buffer(session))
      return NULL;

  *terminator = 0;
  session->bytes_to_remove = terminator - (char *) session->input_buffer->data + 2;

  line = session->input_buffer->data;

  /* g_log() escapes unsafe and non UTF-8 characters, so this is safe */
  mn_client_session_notice(session, "< %s", line);

  return line;
}

/**
 * mn_client_session_write:
 * @session: a #MNClientSession object to write to
 * @format: a printf() format string
 * @...: the arguments to the format string
 *
 * Writes a formatted crlf-terminated line to @session. If an error
 * occurs, mn_client_session_set_error() will be called on @session.
 *
 * Return value: %MN_CLIENT_SESSION_RESULT_CONTINUE on success, or the
 * return value of mn_client_session_set_error() on failure
 **/
int
mn_client_session_write (MNClientSession *session,
			 const char *format,
			 ...)
{
  va_list args;
  char *str;
  char *full;
  unsigned int len;
  GByteArray *array = NULL;
  ssize_t bytes_written;
  int result = MN_CLIENT_SESSION_RESULT_CONTINUE;

  g_return_val_if_fail(session != NULL, 0);
  g_return_val_if_fail(format != NULL, 0);

  va_start(args, format);
  str = g_strdup_vprintf(format, args);
  va_end(args);

  mn_client_session_notice(session, "> %s", str);
  full = g_strconcat(str, "\r\n", NULL);
  g_free(str);
  len = strlen(full);

#if WITH_SASL
  if (session->sasl_ssf)
    {
      unsigned int start = 0;

      array = g_byte_array_new();
      while (len > 0)
	{
	  unsigned int chunk_len;
	  const char *out;
	  unsigned int outlen;

	  chunk_len = MIN(len, session->sasl_maxoutbuf);
	  if (sasl_encode(session->sasl_conn, full + start, chunk_len, &out, &outlen) != SASL_OK)
	    {
	      result = mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to encode data using SASL: %s"), sasl_errdetail(session->sasl_conn));
	      goto end;
	    }

	  g_byte_array_append(array, out, outlen);

	  start += chunk_len;
	  len -= chunk_len;
	}
    }
#endif /* WITH_SASL */

  if (! array)
    {
      array = g_byte_array_sized_new(len);
      g_byte_array_append(array, full, len);
    }

#if WITH_SSL
  if (session->ssl)
    bytes_written = SSL_write(session->ssl, array->data, array->len);
  else
#endif /* WITH_SSL */
    do
      bytes_written = write(session->s, array->data, array->len);
    while (bytes_written < 0 && errno == EINTR);

  if (bytes_written <= 0)
    {
#if WITH_SSL
      if (session->ssl)
	result = mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_CONNECTION_LOST, _("unable to write to server: %s"), mn_ssl_get_error());
      else
#endif /* WITH_SSL */
	{
	  if (bytes_written == 0)
	    result = mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_CONNECTION_LOST, _("unable to write to server: EOF"));
	  else
	    result = mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_CONNECTION_LOST, _("unable to write to server: %s"), g_strerror(errno));
	}
    }

#if WITH_SASL
 end:
#endif
  g_free(full);
  g_byte_array_free(array, TRUE);

  return result;
}

#if WITH_SASL
static int
mn_client_session_write_base64 (MNClientSession *session,
				const char *buf,
				unsigned int len)
{
  char buf64[len * 2];		/* Base64 is 33% larger than the data it encodes */
  unsigned int outlen;
  int result;
  char *str;

  g_return_val_if_fail(session != NULL, 0);
  g_return_val_if_fail(buf != NULL, 0);

  result = sasl_encode64(buf, len, buf64, sizeof(buf64), &outlen);
  if (result != SASL_OK)
    return mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to encode Base64: %s"), sasl_errstring(result, NULL, NULL));

  str = g_strndup(buf64, outlen);
  result = mn_client_session_write(session, "%s", str);
  g_free(str);

  return result;
}

static gboolean
mn_client_session_sasl_fill_interact (MNClientSession *session,
				      sasl_interact_t *interact,
				      const char *unknown_warning)
{
  sasl_interact_t *i;
  gboolean need_username = FALSE;
  gboolean need_password = FALSE;
  const char *username = NULL;
  const char *password = NULL;

  g_return_val_if_fail(session != NULL, FALSE);
  g_return_val_if_fail(interact != NULL, FALSE);

  for (i = interact; i->id; i++)
    switch (i->id)
      {
      case SASL_CB_USER:
      case SASL_CB_AUTHNAME:
	need_username = TRUE;
	break;

      case SASL_CB_PASS:
	need_password = TRUE;
	break;

      default:
	mn_client_session_warning(session, unknown_warning);
	return FALSE;
      };

  if (need_username || need_password)
    {
      if (! session->callbacks->sasl_get_credentials(session,
						     session->private,
						     need_username ? &username : NULL,
						     need_password ? &password : NULL))
	return FALSE;
    }

  for (i = interact; i->id; i++)
    {
      const char *data;

      switch (i->id)
	{
	case SASL_CB_USER:
	case SASL_CB_AUTHNAME:
	  data = username;
	  break;

	case SASL_CB_PASS:
	  data = password;
	  break;

	default:
	  g_assert_not_reached();
	  break;
	};

      g_assert(data != NULL);

      i->result = data;
      i->len = strlen(data);
    }

  return TRUE;
}

static char *
mn_client_session_sasl_get_ip_port (const struct sockaddr *addr)
{
#if WITH_IPV6
  char buf[INET6_ADDRSTRLEN];
#else
  char buf[INET_ADDRSTRLEN];
#endif /* WITH_IPV6 */
  int port;

  g_return_val_if_fail(addr != NULL, NULL);

  if (addr->sa_family == AF_INET)
    {
      struct sockaddr_in *in = (struct sockaddr_in *) addr;

      if (! inet_ntop(addr->sa_family, &in->sin_addr, buf, sizeof(buf)))
	return NULL;
      port = g_ntohs(in->sin_port);
    }
#if WITH_IPV6
  else if (addr->sa_family == AF_INET6)
    {
      struct sockaddr_in6 *in6 = (struct sockaddr_in6 *) addr;

      if (! inet_ntop(addr->sa_family, &in6->sin6_addr, buf, sizeof(buf)))
	return NULL;
      port = g_ntohs(in6->sin6_port);
    }
#endif
  else
    return NULL;

  return g_strdup_printf("%s;%i", buf, port);
}

/**
 * mn_client_session_sasl_authentication_start:
 * @session: a #MNClientSession
 * @service: the SASL service identifier (normally "pop" or "imap")
 * @mechanisms: the list of available mechanisms, or %NULL
 * @forced_mechanism: a mechanism to force usage of, or %NULL
 * @used_mechanism: a location to store the name of the mechanism that was
 *                  selected by the SASL library
 * @initial_clientout: a location to store the initial client response,
 *                     or %NULL
 * @initial_clientoutlen: a location to store the length of the initial
 *                        client response, or %NULL
 *
 * Starts a SASL authentication exchange. @initial_clientout and
 * @initial_clientoutlen must be both set or both %NULL.
 *
 * If @forced_mechanism is provided, authentication is attempted using
 * that mechanism only. Otherwise, @mechanisms must point to a
 * non-empty list of available mechanism names, and the SASL library
 * will select an appropriate mechanism automatically.
 *
 * On success, the selected mechanism is stored at @used_mechanism.
 *
 * On failure, if a mechanism could be selected, it is stored at
 * @used_mechanism (the caller might want to remove that mechanism
 * from the list and try again). Otherwise, %NULL is stored at
 * @used_mechanism.
 *
 * On success, if @initial_clientout and @initial_clientoutlen were
 * set, they point to the initial client response (which is not
 * necessarily NUL-terminated) and its length, respectively. If there
 * is no initial client response, they point to %NULL and 0,
 * respectively.
 *
 * The function may be called multiple times.
 *
 * Return value: %TRUE on success
 **/
gboolean
mn_client_session_sasl_authentication_start (MNClientSession *session,
					     const char *service,
					     GSList *mechanisms,
					     const char *forced_mechanism,
					     const char **used_mechanism,
					     const char **initial_clientout,
					     unsigned int *initial_clientoutlen)
{
  GError *err = NULL;
  int result;
  struct sockaddr name;
  socklen_t namelen;
  char *local_ip_port = NULL;
  char *remote_ip_port = NULL;

  g_return_val_if_fail(session != NULL, FALSE);
  g_return_val_if_fail(session->callbacks->sasl_get_credentials != NULL, FALSE);
  g_return_val_if_fail(service != NULL, FALSE);
  g_return_val_if_fail(mechanisms != NULL || forced_mechanism != NULL, FALSE);
  g_return_val_if_fail((initial_clientout == NULL && initial_clientoutlen == NULL)
		       || (initial_clientout != NULL && initial_clientoutlen != NULL), FALSE);

  if (! mn_sasl_init(&err))
    {
      mn_client_session_warning(session, _("unable to initialize the SASL library: %s"), err->message);
      g_error_free(err);
      return FALSE;
    }

  /* make sure we do not leak the previous sasl_conn if any */
  mn_client_session_sasl_dispose(session);

  namelen = sizeof(name);
  if (getsockname(session->s, &name, &namelen) >= 0)
    local_ip_port = mn_client_session_sasl_get_ip_port(&name);
  else
    mn_client_session_warning(session, _("unable to retrieve local address of socket: %s"), g_strerror(errno));

  namelen = sizeof(name);
  if (getpeername(session->s, &name, &namelen) >= 0)
    remote_ip_port = mn_client_session_sasl_get_ip_port(&name);
  else
    mn_client_session_warning(session, _("unable to retrieve remote address of socket: %s"), g_strerror(errno));

  result = sasl_client_new(service,
			   session->server,
			   local_ip_port,
			   remote_ip_port,
			   sasl_callbacks,
			   0,
			   &session->sasl_conn);

  g_free(local_ip_port);
  g_free(remote_ip_port);

  if (result == SASL_OK)
    {
      sasl_security_properties_t security;
      sasl_interact_t *interact = NULL;
      GString *mechanisms_string;
      GSList *l;

      security.min_ssf = 0;
      security.max_ssf = 256;
      security.maxbufsize = READ_BUFSIZE;
      /* only permit plaintext mechanisms if SSL is in use */
#if WITH_SSL
      if (session->ssl)
	security.security_flags = 0;
      else
#endif /* WITH_SSL */
	security.security_flags = SASL_SEC_NOPLAINTEXT;
      security.property_names = NULL;
      security.property_values = NULL;

      if (sasl_setprop(session->sasl_conn, SASL_SEC_PROPS, &security) != SASL_OK)
	mn_client_session_warning(session, _("unable to set SASL security properties: %s"), sasl_errdetail(session->sasl_conn));

      mechanisms_string = g_string_new(NULL);
      if (forced_mechanism)
	g_string_append(mechanisms_string, forced_mechanism);
      else
	MN_LIST_FOREACH(l, mechanisms)
          {
	    if (*mechanisms_string->str)
	      g_string_append_c(mechanisms_string, ' ');
	    g_string_append(mechanisms_string, l->data);
	  }

      do
	{
	  result = sasl_client_start(session->sasl_conn,
				     mechanisms_string->str,
				     &interact,
				     initial_clientout,
				     initial_clientoutlen,
				     used_mechanism);

	  if (result == SASL_INTERACT)
	    {
	      if (! mn_client_session_sasl_fill_interact(session, interact, _("unable to start SASL authentication: SASL asked for something we did not know")))
		break;
	    }
	}
      while (result == SASL_INTERACT);

      g_string_free(mechanisms_string, TRUE);

      switch (result)
	{
	case SASL_OK:
	case SASL_CONTINUE:
	  return TRUE;

	case SASL_INTERACT:
	  /* could not fill interaction, nop */
	  break;

	default:
	  mn_client_session_warning(session, _("unable to start SASL authentication: %s"), sasl_errdetail(session->sasl_conn));
	}
    }
  else
    mn_client_session_warning(session, _("unable to create a SASL connection: %s"), sasl_errdetail(session->sasl_conn));

  return FALSE;
}

/**
 * mn_client_session_sasl_authentication_step:
 * @session: a #MNClientSession
 * @input: the last server challenge received
 *
 * Continues a SASL authentication exchange successfully initiated
 * with mn_client_session_sasl_authentication_start().
 *
 * Return value: the state to switch to
 **/
int
mn_client_session_sasl_authentication_step (MNClientSession *session,
					    const char *input)
{
  g_return_val_if_fail(session != NULL, 0);
  g_return_val_if_fail(session->sasl_conn != NULL, 0);
  g_return_val_if_fail(input != NULL, 0);

  {
    unsigned int inlen = strlen(input);
    char buf[inlen];
    unsigned int outlen;
    int result;

    result = sasl_decode64(input, inlen, buf, inlen, &outlen);
    if (result == SASL_OK)
      {
	sasl_interact_t *interact = NULL;
	const char *clientout;
	unsigned int clientoutlen;

	do
	  {
	    result = sasl_client_step(session->sasl_conn,
				      buf,
				      outlen,
				      &interact,
				      &clientout,
				      &clientoutlen);

	    if (result == SASL_INTERACT)
	      {
		if (! mn_client_session_sasl_fill_interact(session, interact, _("SASL asked for something we did not know, aborting SASL authentication")))
		  break;
	      }
	  }
	while (result == SASL_INTERACT);

	switch (result)
	  {
	  case SASL_OK:
	  case SASL_CONTINUE:
	    return mn_client_session_write_base64(session, clientout, clientoutlen);

	  case SASL_INTERACT:
	    /* could not fill interaction, abort */
	    return mn_client_session_write(session, "*");

	  default:
	    mn_client_session_warning(session, _("%s, aborting SASL authentication"), sasl_errdetail(session->sasl_conn));
	    return mn_client_session_write(session, "*");
	  }
      }
    else			/* compliance error */
      return mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to decode Base64 input from server: %s"), sasl_errstring(result, NULL, NULL));
  }
}

/**
 * mn_client_session_sasl_authentication_done:
 * @session: a #MNClientSession
 *
 * Completes a successful SASL authentication exchange. Must only be
 * used if the server has terminated the exchange with a positive
 * response.
 *
 * Return value: %TRUE on success
 **/
gboolean
mn_client_session_sasl_authentication_done (MNClientSession *session)
{
  gconstpointer ptr;

  g_return_val_if_fail(session != NULL, FALSE);
  g_return_val_if_fail(session->sasl_conn != NULL, 0);

  if (sasl_getprop(session->sasl_conn, SASL_SSF, &ptr) == SASL_OK)
    {
      const sasl_ssf_t *ssf = ptr;

      if (*ssf)
	{
	  if (sasl_getprop(session->sasl_conn, SASL_MAXOUTBUF, &ptr) == SASL_OK)
	    {
	      const unsigned int *maxoutbuf = ptr;

	      session->sasl_ssf = *ssf;
	      session->sasl_maxoutbuf = *maxoutbuf;

	      if (session->sasl_ssf)
		mn_client_session_notice(session, _("a SASL security layer of strength factor %i is now active"), session->sasl_ssf);
	    }
	  else
	    {
	      /* a security layer is active but we can't retrieve maxoutbuf -> fatal */
	      mn_client_session_set_error(session, MN_CLIENT_SESSION_ERROR_OTHER, _("unable to get SASL_MAXOUTBUF property: %s"), sasl_errdetail(session->sasl_conn));
	      return FALSE;
	    }
	}
    }
  else
    mn_client_session_warning(session, _("warning: unable to get SASL_SSF property: %s"), sasl_errdetail(session->sasl_conn));

  return TRUE;
}

/**
 * mn_client_session_sasl_dispose:
 * @session: a #MNClientSession
 *
 * Destroys the SASL connection of @session, or, if no SASL connection
 * is active, does nothing.
 *
 * Since the SASL connection is always destroyed before
 * mn_client_session_run() returns, omitting to call this function
 * will not leak the object away. However, in some situations (eg. if
 * SASL authentication fails but the session continues nevertheless)
 * it might be desirable to get rid of the object, in order to free
 * memory for the rest of the session duration.
 **/
void
mn_client_session_sasl_dispose (MNClientSession *session)
{
  g_return_if_fail(session != NULL);

  if (session->sasl_conn)
    {
      sasl_dispose(&session->sasl_conn);
      session->sasl_conn = NULL;
    }
}

/**
 * mn_client_session_sasl_get_ssf:
 * @session: a #MNClientSession
 *
 * Gets the SASL security strength factor. Must not be used unless
 * mn_client_session_sasl_authentication_done() has returned %TRUE.
 *
 * Return value: 0 if no security layer is active, or an approximation
 * of the encryption key length otherwise
 **/
sasl_ssf_t
mn_client_session_sasl_get_ssf (MNClientSession *session)
{
  g_return_val_if_fail(session != NULL, 0);
  g_return_val_if_fail(session->sasl_conn != NULL, 0);

  return session->sasl_ssf;
}
#endif /* WITH_SASL */

/**
 * mn_client_session_notice:
 * @session: a #MNClientSession
 * @format: a printf() format string
 * @...: the arguments to the format string
 *
 * If the notice callback of @session is defined, calls it with the
 * given message as argument. Otherwise, does nothing.
 **/
void
mn_client_session_notice (MNClientSession *session,
			  const char *format,
			  ...)
{
  g_return_if_fail(session != NULL);
  g_return_if_fail(format != NULL);

  if (session->callbacks->notice)
    {
      va_list args;
      char *message;

      va_start(args, format);
      message = g_strdup_vprintf(format, args);
      va_end(args);

      session->callbacks->notice(session, message, session->private);
      g_free(message);
    }
}

/**
 * mn_client_session_warning:
 * @session: a #MNClientSession
 * @format: a printf() format string
 * @...: the arguments to the format string
 *
 * If the warning callback of @session is defined, calls it with the
 * given message as argument. Otherwise, does nothing.
 **/
void
mn_client_session_warning (MNClientSession *session,
			   const char *format,
			   ...)
{
  g_return_if_fail(session != NULL);
  g_return_if_fail(format != NULL);

  if (session->callbacks->warning)
    {
      va_list args;
      char *message;

      va_start(args, format);
      message = g_strdup_vprintf(format, args);
      va_end(args);

      session->callbacks->warning(session, message, session->private);
      g_free(message);
    }
}

/**
 * mn_client_session_set_error:
 * @session: a #MNClientSession
 * @code: a #MNClientSessionError code
 * @format: a printf() format string
 * @...: the arguments to the format string
 *
 * If @session has no error yet, sets the given error. Otherwise, does
 * nothing.
 *
 * Return value: %MN_CLIENT_SESSION_RESULT_DISCONNECT
 **/
int
mn_client_session_set_error (MNClientSession *session,
			     int code,
			     const char *format,
			     ...)
{
  g_return_val_if_fail(session != NULL, 0);
  g_return_val_if_fail(format != NULL, 0);

  if (! session->error)
    {
      va_list args;
      char *message;

      va_start(args, format);
      message = g_strdup_vprintf(format, args);
      va_end(args);

      session->error = g_error_new_literal(MN_CLIENT_SESSION_ERROR, code, message);
      g_free(message);
    }

  return MN_CLIENT_SESSION_RESULT_DISCONNECT;
}

int
mn_client_session_set_error_from_response (MNClientSession *session,
					   int code,
					   const char *response)
{
  g_return_val_if_fail(session != NULL, 0);

  return response
    ? mn_client_session_set_error(session, code, _("\"%s\""), response)
    : mn_client_session_set_error(session, code, _("unknown server error"));
}

GQuark
mn_client_session_error_quark (void)
{
  return g_quark_from_static_string("mn-client-session-error");
}
