/* 
 * Copyright (c) 2004 Jean-Yves Lefort <jylefort@brutele.be>
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
} MNClientSessionCallbacks;

gboolean mn_client_session_run (MNClientSessionState *states,
				MNClientSessionCallbacks *callbacks,
#ifdef WITH_SSL
				gboolean use_ssl,
#endif
				const char *hostname,
				int port,
				const char *username,
				const char *password,
				MNClientSessionPrivate *private,
				GError **err);

int mn_client_session_write (MNClientSession *session,
			     const char *format,
			     ...);

#ifdef WITH_SASL
gboolean mn_client_session_sasl_authentication_start (MNClientSession *session,
						      const char *service,
						      GSList *mechanisms,
						      const char *forced_mechanism,
						      const char **used_mechanism);
int mn_client_session_sasl_authentication_step (MNClientSession *session,
						const char *input);
gboolean mn_client_session_sasl_authentication_done (MNClientSession *session);
#endif /* WITH_SASL */

void mn_client_session_notice (MNClientSession *session,
			       const char *format,
			       ...);
int mn_client_session_error (MNClientSession *session,
			     const char *format,
			     ...);

#endif /* _MN_CLIENT_SESSION_H */
