/*
 * Copyright (c) 2016 Balabit
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */

#include "cache.h"
#include "testutils.h"

gint fetch_count;
gint free_fn_count;

void *
fetch(Provider *self, gchar *key)
{
  fetch_count++;
  return g_strdup_printf("almafa_%s", key);
}

void
free_fn(Provider *self)
{
  free_fn_count++;
  return;
}

Provider *
moc_provider_new()
{
  Provider *self = g_new0(Provider,1);
  self->fetch = fetch;
  self->free_fn = free_fn;
  return self;
}

void
test_cache_write_and_read()
{
  fetch_count = 0;
  free_fn_count = 0;
  gchar *key = "key";
  gchar *key2 = "key2";
  Cache *c = NULL;

  c = cache_new(moc_provider_new(), g_free);

  gchar *result = cache_lookup(c, key);
  assert_string(result, "almafa_key","Value error for \"%s\" key", key);
  assert_gint(fetch_count, 1, "Fetch call error for \"%s\" key", key);

  result = cache_lookup(c, key);
  assert_string(result, "almafa_key","Wrong value in the cache for the \"%s\" key", key);
  assert_gint(fetch_count, 1, "cache_lookup call the fetch too many times for \"%s\" key", key);

  result = cache_lookup(c, key2);
  assert_string(result, "almafa_key2","Value error for \"%s\" key", key2);
  assert_gint(fetch_count, 2, "Fetch call error for \"%s\" key", key2);

  result = cache_lookup(c, key2);
  assert_string(result, "almafa_key2","Wrong value in the cache for the \"%s\" key", key2);
  assert_gint(fetch_count, 2, "cache_lookup call the fetch too many times for \"%s\" key", key2);

  result = cache_lookup(c, key);
  assert_string(result, "almafa_key","Wrong value in the cache for the \"%s\" key", key);
  assert_gint(fetch_count, 2, "cache_lookup call the fetch too many times for \"%s\" key", key);

  cache_free(c);
  assert_gint(fetch_count, 2, "cache_free call the fetch");
  assert_gint(free_fn_count, 1, "cache_free call the free_fn %d times", free_fn_count);
}

int
main(int argc G_GNUC_UNUSED, char *argv[] G_GNUC_UNUSED)
{
  test_cache_write_and_read();
  return 0;
}
