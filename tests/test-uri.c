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

#include "mn-uri.h"

int
main (int argc, char **argv)
{
  /*
   * The parser is very lax (unsuitable for handling untrusted URIs),
   * we are only interested in making sure it can handle all the URIs
   * we'll have to deal with.
   */
  const char *pop_uri_list[] = {
    "pop://invalid",

    "pop://usern%40me:password@hostname",
    "pop://usern%40me:password@hostname:110",
    "pop://usern%40me:password@hostname:555",

    "pop://usern%40me:password@[::1]",
    "pop://usern%40me:password@[::1]:110",
    "pop://usern%40me:password@[::1]:555",

    "pop://usern%40me:password;auth=MECH@hostname",
    "pop://usern%40me:password;auth=MECH@hostname:110",
    "pop://usern%40me:password;auth=MECH@hostname:555",

    "pop://usern%40me:password;auth=MECH@[::1]",
    "pop://usern%40me:password;auth=MECH@[::1]:110",
    "pop://usern%40me:password;auth=MECH@[::1]:555",
      
    "pops://invalid",

    "pops://usern%40me:password@hostname",
    "pops://usern%40me:password@hostname:995",
    "pops://usern%40me:password@hostname:555",

    "pops://usern%40me:password@[::1]",
    "pops://usern%40me:password@[::1]:995",
    "pops://usern%40me:password@[::1]:555",

    "pops://usern%40me:password;auth=MECH@hostname",
    "pops://usern%40me:password;auth=MECH@hostname:995",
    "pops://usern%40me:password;auth=MECH@hostname:555",

    "pops://usern%40me:password;auth=MECH@[::1]",
    "pops://usern%40me:password;auth=MECH@[::1]:995",
    "pops://usern%40me:password;auth=MECH@[::1]:555",
  };
  const char *imap_uri_list[] = {
    "imap://invalid",

    "imap://usern%40me:password@hostname",
    "imap://usern%40me:password@hostname/mailbox",
    "imap://usern%40me:password@hostname:143",
    "imap://usern%40me:password@hostname:143/mailbox",
    "imap://usern%40me:password@hostname:555",
    "imap://usern%40me:password@hostname:555/mailbox",
    
    "imap://usern%40me:password@[::1]",
    "imap://usern%40me:password@[::1]/mailbox",
    "imap://usern%40me:password@[::1]:143",
    "imap://usern%40me:password@[::1]:143/mailbox",
    "imap://usern%40me:password@[::1]:555",
    "imap://usern%40me:password@[::1]:555/mailbox",

    "imap://usern%40me:password;auth=MECH@hostname",
    "imap://usern%40me:password;auth=MECH@hostname/mailbox",
    "imap://usern%40me:password;auth=MECH@hostname:143",
    "imap://usern%40me:password;auth=MECH@hostname:143/mailbox",
    "imap://usern%40me:password;auth=MECH@hostname:555",
    "imap://usern%40me:password;auth=MECH@hostname:555/mailbox",
      
    "imap://usern%40me:password;auth=MECH@[::1]",
    "imap://usern%40me:password;auth=MECH@[::1]/mailbox",
    "imap://usern%40me:password;auth=MECH@[::1]:143",
    "imap://usern%40me:password;auth=MECH@[::1]:143/mailbox",
    "imap://usern%40me:password;auth=MECH@[::1]:555",
    "imap://usern%40me:password;auth=MECH@[::1]:555/mailbox",

    "imaps://invalid",

    "imaps://usern%40me:password@hostname",
    "imaps://usern%40me:password@hostname/mailbox",
    "imaps://usern%40me:password@hostname:993",
    "imaps://usern%40me:password@hostname:993/mailbox",
    "imaps://usern%40me:password@hostname:555",
    "imaps://usern%40me:password@hostname:555/mailbox",
    
    "imaps://usern%40me:password@[::1]",
    "imaps://usern%40me:password@[::1]/mailbox",
    "imaps://usern%40me:password@[::1]:993",
    "imaps://usern%40me:password@[::1]:993/mailbox",
    "imaps://usern%40me:password@[::1]:555",
    "imaps://usern%40me:password@[::1]:555/mailbox",

    "imaps://usern%40me:password;auth=MECH@hostname",
    "imaps://usern%40me:password;auth=MECH@hostname/mailbox",
    "imaps://usern%40me:password;auth=MECH@hostname:993",
    "imaps://usern%40me:password;auth=MECH@hostname:993/mailbox",
    "imaps://usern%40me:password;auth=MECH@hostname:555",
    "imaps://usern%40me:password;auth=MECH@hostname:555/mailbox",
      
    "imaps://usern%40me:password;auth=MECH@[::1]",
    "imaps://usern%40me:password;auth=MECH@[::1]/mailbox",
    "imaps://usern%40me:password;auth=MECH@[::1]:993",
    "imaps://usern%40me:password;auth=MECH@[::1]:993/mailbox",
    "imaps://usern%40me:password;auth=MECH@[::1]:555",
    "imaps://usern%40me:password;auth=MECH@[::1]:555/mailbox",
  };
  const char *gmail_uri_list[] = {
    "gmail://invalid",

    "gmail://usern%40me:password"
  };
  int i;
      
  for (i = 0; i < G_N_ELEMENTS(pop_uri_list); i++)
    {
      gboolean ssl = FALSE;
      char *username = NULL;
      char *password = NULL;
      char *authmech = NULL;
      char *hostname = NULL;
      int port = -1;

      g_print("pop %i: ", i);
      if (mn_uri_parse_pop(pop_uri_list[i], &ssl, &username, &password, &authmech, &hostname, &port))
	{
	  char *uri;
	  char *canonicalized;
	  
	  uri = mn_uri_build_pop(ssl, username, password, authmech, hostname, port);
	  g_assert(uri != NULL);
	  
	  canonicalized = mn_uri_canonicalize(uri);
	  g_assert(strcmp(canonicalized, uri) == 0);
	  g_free(canonicalized);

	  g_free(username);
	  g_free(password);
	  g_free(authmech);
	  g_free(hostname);

	  g_print("%s\n", uri);
	  g_free(uri);
	}
      else
	{
	  g_assert(ssl == FALSE
		   && username == NULL
		   && password == NULL
		   && authmech == NULL
		   && hostname == NULL
		   && port == -1);
	  g_print("invalid\n");
	}
    }
  
  for (i = 0; i < G_N_ELEMENTS(imap_uri_list); i++)
    {
      gboolean ssl = FALSE;
      char *username = NULL;
      char *password = NULL;
      char *authmech = NULL;
      char *hostname = NULL;
      int port = -1;
      char *mailbox = NULL;

      g_print("imap %i: ", i);
      if (mn_uri_parse_imap(imap_uri_list[i], &ssl, &username, &password, &authmech, &hostname, &port, &mailbox))
	{
	  char *uri;
	  char *canonicalized;
	  
	  uri = mn_uri_build_imap(ssl, username, password, authmech, hostname, port, mailbox);
	  g_assert(uri != NULL);
	  
	  canonicalized = mn_uri_canonicalize(uri);
	  g_assert(strcmp(canonicalized, uri) == 0);
	  g_free(canonicalized);

	  g_free(username);
	  g_free(password);
	  g_free(authmech);
	  g_free(hostname);
	  g_free(mailbox);

	  g_print("%s\n", uri);
	  g_free(uri);
	}
      else
	{
	  g_assert(ssl == FALSE
		   && username == NULL
		   && password == NULL
		   && authmech == NULL
		   && hostname == NULL
		   && port == -1
		   && mailbox == NULL);
	  g_print("invalid\n");
	}
    }

  for (i = 0; i < G_N_ELEMENTS(gmail_uri_list); i++)
    {
      char *username = NULL;
      char *password = NULL;

      g_print("gmail %i: ", i);
      if (mn_uri_parse_gmail(gmail_uri_list[i], &username, &password))
	{
	  char *uri;
	  char *canonicalized;
	  
	  uri = mn_uri_build_gmail(username, password);
	  g_assert(uri != NULL);
	  
	  canonicalized = mn_uri_canonicalize(uri);
	  g_assert(strcmp(canonicalized, uri) == 0);
	  g_free(canonicalized);

	  g_free(username);
	  g_free(password);

	  g_print("%s\n", uri);
	  g_free(uri);
	}
      else
	{
	  g_assert(username == NULL && password == NULL);
	  g_print("invalid\n");
	}
    }

  return 0;
}
