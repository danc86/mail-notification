/* 
 * Copyright (C) 2004, 2005 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _MN_CLIENT_SESSION_H
#define _MN_CLIENT_SESSION_H

#include "config.h"
#include <glib.h>

typedef struct _MNClientSession MNClientSession;
typedef struct _MNClientSessionPrivate MNClientSessionPrivate;
typedef struct _MNClientSessionResponse MNClientSessionResponse;

enum
{
  MN_CLIENT_SESSION_RESULT_CONTINUE			= -1001,
  MN_CLIENT_SESSION_RESULT_BAD_RESPONSE_FOR_CONTEXT	= -1002,
  MN_CLIENT_SESSION_RESULT_END				= -1003,
};
    
typedef struct
{
  unsigned int	id;
  int		(*enter_cb)	(MNClientSession	  *session,
				 MNClientSessionPrivate	  *private);
  int		(*handle_cb)	(MNClientSession	  *session,
				 MNClientSessionResponse  *response,
				 MNClientSessionPrivate	  *private);
} MNClientSessionState;

#define MN_CLIENT_SESSION_INITIAL_STATE		1
#define MN_CLIENT_SESSION_STATES_END		{ 0, NULL, NULL }

typedef struct
{
  void      (*notice)		(MNClientSession	  *session,
				 const char		  *str,
				 MNClientSessionPrivate	  *private);
  void      (*warning)		(MNClientSession	  *session,
				 const char		  *str,
				 MNClientSessionPrivate	  *private);

  MNClientSessionResponse *(*response_new) (MNClientSession         *session,
					    const char              *input,
					    MNClientSessionPrivate  *private);
  void      (*response_free)	(MNClientSession	  *session,
				 MNClientSessionResponse  *response,
				 MNClientSessionPrivate   *private);

  int       (*custom_handler)	(MNClientSession	  *session,
				 MNClientSessionResponse  *response,
				 int			  result,
				 MNClientSessionPrivate   *private);

  void      (*pre_read)		(MNClientSession          *session,
				 MNClientSessionPrivate   *private);
  void      (*post_read)	(MNClientSession          *session,
				 MNClientSessionPrivate   *private);

#ifdef WITH_SASL
  const char *	(*sasl_get_username)	(MNClientSession	*session,
					 MNClientSessionPrivate	*priv);
  const char *	(*sasl_get_password)	(MNClientSession	*session,
					 MNClientSessionPrivate	*priv);
#endif /* WITH_SASL */
} MNClientSessionCallbacks;

gboolean mn_client_session_run (MNClientSessionState *states,
				MNClientSessionCallbacks *callbacks,
#ifdef WITH_SSL
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

#ifdef WITH_SSL
gboolean mn_client_session_enable_ssl (MNClientSession *session);
#endif

#ifdef WITH_SASL
gboolean mn_client_session_sasl_authentication_start (MNClientSession *session,
						      const char *service,
						      GSList *mechanisms,
						      const char *forced_mechanism,
						      const char **used_mechanism,
						      gboolean initial_response);
int mn_client_session_sasl_authentication_step (MNClientSession *session,
						const char *input);
gboolean mn_client_session_sasl_authentication_done (MNClientSession *session);
int mn_client_session_sasl_get_ssf (MNClientSession *session);
#endif /* WITH_SASL */

void mn_client_session_notice (MNClientSession *session,
			       const char *format,
			       ...) G_GNUC_PRINTF(2, 3);
void mn_client_session_warning (MNClientSession *session,
				const char *format,
				...);
int mn_client_session_error (MNClientSession *session,
			     const char *format,
			     ...) G_GNUC_PRINTF(2, 3);

#endif /* _MN_CLIENT_SESSION_H */
