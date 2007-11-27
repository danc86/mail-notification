/*
 * Mail Notification
 * Copyright (C) 2003-2007 Jean-Yves Lefort <jylefort@brutele.be>
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

#ifndef _MN_CLIENT_SESSION_H
#define _MN_CLIENT_SESSION_H

#include "config.h"
#include <glib.h>
#if WITH_SASL
#include <sasl/sasl.h>
#endif

typedef struct _MNClientSession MNClientSession;
typedef struct _MNClientSessionPrivate MNClientSessionPrivate;
typedef struct _MNClientSessionResponse MNClientSessionResponse;

#define MN_CLIENT_SESSION_ERROR		(mn_client_session_error_quark())

typedef enum
{
  MN_CLIENT_SESSION_ERROR_OTHER,
  MN_CLIENT_SESSION_ERROR_CONNECTION_LOST
} MNClientSessionError;

enum
{
  /* read the next input line */
  MN_CLIENT_SESSION_RESULT_CONTINUE			= -1,
  /* signal an uncompliant response and disconnect */
  MN_CLIENT_SESSION_RESULT_BAD_RESPONSE_FOR_CONTEXT	= -2,
  /* disconnect */
  MN_CLIENT_SESSION_RESULT_DISCONNECT			= -3
};

/*
 * In all the session callbacks below, @session is the session which
 * is being run, and @private is the opaque data pointer which was
 * passed to mn_client_session_run().
 */

typedef struct
{
  /*
   * The state ID, which must be a positive non-zero integer.
   */
  unsigned int	id;

  /*
   * enter_cb (optional):
   *
   * Enters the state. If set to %NULL, does nothing and returns
   * %MN_CLIENT_SESSION_RESULT_CONTINUE.
   *
   * Return value: must return a state ID to switch to, or one of the
   * MN_CLIENT_SESSION_RESULT_ codes above
   */
  int		(*enter_cb)	(MNClientSession	  *session,
				 MNClientSessionPrivate	  *private);

  /*
   * handle_cb (required):
   * @response: the #MNClientSessionResponse returned by the response_new()
   *            callback.
   *
   * Handles a response received while the state is active.
   *
   * Return value: must return a state ID to switch to, or one of the
   * MN_CLIENT_SESSION_RESULT_ codes above
   */
  int		(*handle_cb)	(MNClientSession	  *session,
				 MNClientSessionResponse  *response,
				 MNClientSessionPrivate	  *private);
} MNClientSessionState;

/* the state to enter after connecting */
#define MN_CLIENT_SESSION_INITIAL_STATE		1
/* the MNClientSessionState array canary */
#define MN_CLIENT_SESSION_STATES_END		{ 0, NULL, NULL }

typedef struct
{
  /*
   * notice (optional):
   * @message: the notice message
   *
   * Handles a notice.
   */
  void      (*notice)		(MNClientSession	  *session,
				 const char		  *message,
				 MNClientSessionPrivate	  *private);

  /*
   * warning (optional):
   * @message: the warning message
   *
   * Handles a warning.
   */
  void      (*warning)		(MNClientSession	  *session,
				 const char		  *message,
				 MNClientSessionPrivate	  *private);

  /*
   * response_new (required):
   * @input: an input line received from the server
   *
   * Parses server input.
   *
   * Return value: must return an opaque data pointer on success, or
   * %NULL on failure
   */
  MNClientSessionResponse *(*response_new) (MNClientSession         *session,
					    const char              *input,
					    MNClientSessionPrivate  *private);

  /*
   * response_free (optional):
   * @response: a response that was returned by response_new()
   *
   * Releases all the memory associated with @response.
   */
  void      (*response_free)	(MNClientSession	  *session,
				 MNClientSessionResponse  *response,
				 MNClientSessionPrivate   *private);

  /*
   * pre_read (optional):
   *
   * Called before calling read() or one of its variants.
   */
  void      (*pre_read)		(MNClientSession          *session,
				 MNClientSessionPrivate   *private);

  /*
   * post_read (optional):
   *
   * Called after calling read() or one of its variants.
   */
  void      (*post_read)	(MNClientSession          *session,
				 MNClientSessionPrivate   *private);

#if WITH_SASL
  /*
   * sasl_get_credentials (required if
   * mn_client_session_sasl_authentication_start() is needed, optional
   * otherwise):
   * @username: a pointer to store the username, or %NULL
   * @password: a pointer to store the password, or %NULL
   *
   * Fills in the requested credentials.
   *
   * Return value: must return %TRUE in case of success, or %FALSE if
   * a requested credential cannot be provided (in such case, the SASL
   * authentication exchange will be aborted)
   */
  gboolean  (*sasl_get_credentials)	(MNClientSession	*session,
					 MNClientSessionPrivate	*priv,
					 const char		**username,
					 const char		**password);
#endif /* WITH_SASL */

#if WITH_SSL
  /*
   * ssl_trust_server (required if SSL support was compiled in):
   *
   * Decides whether to trust the given server after SSL verification
   * failed. If the server provided a certificate, the @fingerprint
   * and @verify_error arguments will be set.
   *
   * Return value: must return %TRUE if the given server should be trusted.
   */
  gboolean  (*ssl_trust_server)		(MNClientSession	*session,
					 const char		*server,
					 int			port,
					 const char		*fingerprint,
					 const char		*verify_error,
					 MNClientSessionPrivate	*priv);
#endif /* WITH_SSL */
} MNClientSessionCallbacks;

gboolean mn_client_session_run (const MNClientSessionState *states,
				const MNClientSessionCallbacks *callbacks,
#if WITH_SSL
				gboolean use_ssl,
#endif
				const char *hostname,
				int port,
				MNClientSessionPrivate *private,
				GError **err);

gconstpointer mn_client_session_read (MNClientSession *session, unsigned int nbytes);
const char *mn_client_session_read_line (MNClientSession *session);

int mn_client_session_write (MNClientSession *session,
			     const char *format,
			     ...) G_GNUC_PRINTF(2, 3);

#if WITH_SSL
gboolean mn_client_session_enable_ssl (MNClientSession *session);
#endif

#if WITH_SASL
gboolean mn_client_session_sasl_authentication_start (MNClientSession *session,
						      const char *service,
						      GSList *mechanisms,
						      const char *forced_mechanism,
						      const char **used_mechanism,
						      const char **initial_clientout,
						      unsigned int *initial_clientoutlen);
int mn_client_session_sasl_authentication_step (MNClientSession *session,
						const char *input);
gboolean mn_client_session_sasl_authentication_done (MNClientSession *session);

void mn_client_session_sasl_dispose (MNClientSession *session);
sasl_ssf_t mn_client_session_sasl_get_ssf (MNClientSession *session);
#endif /* WITH_SASL */

void mn_client_session_notice (MNClientSession *session,
			       const char *format,
			       ...) G_GNUC_PRINTF(2, 3);
void mn_client_session_warning (MNClientSession *session,
				const char *format,
				...);

int mn_client_session_set_error (MNClientSession *session,
				 int code,
				 const char *format,
				 ...) G_GNUC_PRINTF(3, 4);
int mn_client_session_set_error_from_response (MNClientSession *session,
					       int code,
					       const char *response);

GQuark mn_client_session_error_quark (void);

#endif /* _MN_CLIENT_SESSION_H */
