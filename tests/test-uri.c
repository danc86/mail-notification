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

#include <stdlib.h>
#include <string.h>
#include "mn-uri.h"

static char *
build_uri (const char *scheme,
	   const char *password,
	   const char *authmech,
	   const char *hostname,
	   int port,
	   const char *path,
	   const char *queries)
{
  GString *uri;

  uri = g_string_new(scheme);
  g_string_append(uri, "://username");

  if (password)
    g_string_append_printf(uri, ":%s", password);
  if (authmech)
    g_string_append_printf(uri, ";AUTH=%s", authmech);
  if (hostname)
    g_string_append_printf(uri, "@%s", hostname);
  if (port != -1)
    g_string_append_printf(uri, ":%i", port);
  if (path)
    g_string_append_printf(uri, "/%s", path);
  if (queries)
    g_string_append_printf(uri, "?%s", queries);

  return g_string_free(uri, FALSE);
}

static char *
build_canonical_uri (const char *scheme,
		     const char *password,
		     const char *authmech,
		     const char *hostname,
		     int port,
		     const char *path,
		     const char *queries)
{
  int default_port = -1;
  const char *default_path = NULL;
  
  if (! strcmp(scheme, "pop") || ! strcmp(scheme, "pops"))
    {
      if (! hostname || path)
	return NULL;
    }
  else if (! strcmp(scheme, "imap") || ! strcmp(scheme, "imaps"))
    {
      if (! hostname)
	return NULL;
    }
  else if (! strcmp(scheme, "gmail"))
    {
      if (authmech || hostname || port != -1 || path || queries)
	return NULL;
    }
  
  if (! strcmp(scheme, "pop"))
    default_port = 110;
  else if (! strcmp(scheme, "pops"))
    default_port = 995;
  else if (! strcmp(scheme, "imap"))
    default_port = 143;
  else if (! strcmp(scheme, "imaps"))
    default_port = 993;

  if (! strcmp(scheme, "imap") || ! strcmp(scheme, "imaps"))
    default_path = "INBOX";

  return build_uri(scheme,
		   password,
		   authmech,
		   hostname,
		   port != default_port ? port : -1,
		   (! path || ! default_path || strcmp(path, default_path)) ? path : NULL,
		   queries);
}

static void
assert_strequal (const char *uri, const char *str1, const char *str2)
{
  if (! ((! str1 && ! str2) || (str1 && str2 && ! strcmp(str1, str2))))
    {
      g_print("%s and %s differ\n", str1, str2);
      g_print("uri was %s\n", uri);
      exit(1);
    }
}

int
main (int argc, char **argv)
{
#define FOR(iterator, var) \
  for (iterator = 0; iterator < G_N_ELEMENTS(var); iterator++)

  int a, b, c, d, e, f, g;
  const char *schemes[] = { "pop", "pops", "imap", "imaps", "gmail" };
  const char *passwords[] = { NULL, "password" };
  const char *authmechs[] = { NULL, "CRAM-MD5" };
  const char *hostnames[] = { NULL, "hostname", "[::1]" };
  int ports[] = { -1, 110, 995, 143, 993, 555 };
  const char *paths[] = { NULL, "INBOX", "foo-path" };
  const char *queries[] = { NULL, "STARTTLS", "STLS", "foo-query", "noidle" };
  int n_uri = 0;
  int n_canonical = 0;
  
  g_type_init();

  FOR(a, schemes)
    FOR(b, passwords)
    FOR(c, authmechs)
    FOR(d, hostnames)
    FOR(e, ports)
    FOR(f, paths)
    FOR(g, queries)
  {
    char *uri;
    char *canonical_uri;
    MNURI *obj;

    n_uri++;

    uri = build_uri(schemes[a], passwords[b], authmechs[c], hostnames[d], ports[e], paths[f], queries[g]);
    canonical_uri = build_canonical_uri(schemes[a], passwords[b], authmechs[c], hostnames[d], ports[e], paths[f], queries[g]);

    obj = mn_uri_new(uri);
    if (canonical_uri)
      {
	n_canonical++;
	assert_strequal(uri, obj->text, canonical_uri);
      }

    g_free(uri);
    g_free(canonical_uri);
    g_object_unref(obj);
  }

  g_print("+++ URI TEST PASSED (%i URI tested, %i could be canonicalized)\n", n_uri, n_canonical);

  return 0;
}
