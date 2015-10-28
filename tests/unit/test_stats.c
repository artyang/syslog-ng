/*
 * Copyright (c) 2002-2015 BalaBit IT Ltd, Budapest, Hungary
 * Copyright (c) 2009-2015 Viktor Juhász
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */
#include "testutils.h"
#include "stats.c"
#include "apphook.h"

StatsCounterItem *counter;

static inline void
init_test(gint stats_level)
{
  counter = NULL;
  stats_init();
  stats_set_stats_level(stats_level);
}

static inline void
clean_test()
{
  stats_destroy();
}

void
test_register_counter()
{
  init_test(3);
  stats_register_counter(2, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &counter);
  assert_not_null(counter, NULL);
  stats_counter_inc(counter);
  assert_gint(counter->value, 1, NULL);
  stats_unregister_counter(SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &counter);
  assert_null(counter, NULL);
  clean_test();
}

void
test_stats_level()
{
  init_test(1);
  stats_register_counter(2, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &counter);
  assert_null(counter, NULL);
  clean_test();
}

void
test_empty_keys()
{
  init_test(1);
  stats_register_counter(1, SCS_TCP | SCS_SOURCE, NULL, NULL, SC_TYPE_PROCESSED, &counter);
  assert_not_null(counter, NULL);
  clean_test();
}

void
test_dynamic_instant_inc()
{
  init_test(1);

  gboolean new;
  stats_instant_inc_dynamic_counter(1, SCS_TCP | SCS_DESTINATION, "d_tcp#0", "1.1.1.1", (time_t)123456789);
  stats_register_dynamic_counter(1, SCS_TCP | SCS_DESTINATION, "d_tcp#0", "1.1.1.1", SC_TYPE_PROCESSED, &counter, &new);
  assert_false(new, NULL);
  assert_gint(counter->value, 1, NULL);

  stats_register_dynamic_counter(1, SCS_TCP | SCS_DESTINATION, "d_tcp#0", "1.1.1.1", SC_TYPE_STAMP, &counter, &new);
  assert_true(new, NULL);
  assert_gint(counter->value, 123456789, NULL);

  clean_test();
}

void
test_register_new_dynamic_counter()
{
  init_test(1);
  gboolean new = FALSE;
  StatsCounter *sc = stats_register_dynamic_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &counter, &new);
  assert_not_null(counter, NULL);
  assert_true(new, NULL);
  assert_true(sc->dynamic, NULL);
  clean_test();
}

void
test_register_already_registered_dynamic_counter()
{
  init_test(1);
  gboolean new = FALSE;
  stats_register_dynamic_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &counter, &new);
  stats_register_dynamic_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &counter, &new);
  assert_not_null(counter, NULL);
  assert_false(new, NULL);
  clean_test();
}

void
test_register_same_dynamic_counter_but_another_counter_item()
{
  init_test(1);
  gboolean new = FALSE;
  StatsCounter *sc = stats_register_dynamic_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &counter, &new);
  StatsCounter *new_sc = stats_register_dynamic_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_DROPPED, &counter, &new);
  assert_not_null(counter, NULL);
  assert_false(new, NULL);
  assert_gpointer(sc, new_sc, NULL);
  clean_test();
}

void
test_cleanup_orphans()
{
  init_test(1);
  gboolean new;
  StatsCounterItem *static_counter = NULL;
  StatsCounter *sc_processed = stats_register_dynamic_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &counter, &new);
  StatsCounter *sc_dropped = stats_register_dynamic_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_DROPPED, &counter, &new);
  stats_register_counter(1, SCS_TCP | SCS_DESTINATION, "d_tcp#0", "127.0.0.1", SC_TYPE_STORED, &static_counter);

  assert_gpointer(sc_dropped, sc_processed, NULL);

  stats_unregister_counter( SCS_TCP | SCS_DESTINATION, "d_tcp#0", "127.0.0.1", SC_TYPE_STORED, &static_counter);
  stats_unregister_dynamic_counter(sc_dropped, SC_TYPE_DROPPED, &counter);
  stats_cleanup_orphans();

  stats_register_dynamic_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_DROPPED, &counter, &new);
  assert_false(new, NULL);
  clean_test();
}

void
test_stats_reinit()
{
  init_test(1);
  gint pri = 12;
  gint fac = LOG_FAC(pri);
  gint sev = LOG_PRI(pri);
  GlobalConfig *cfg = cfg_new(0x0505);

  stats_counter_inc_pri(pri);
  assert_null(facility_counters[fac], NULL);
  assert_null(severity_counters[sev], NULL);

  stats_set_stats_level(3);
  stats_reinit(cfg);
  stats_counter_inc_pri(pri);
  assert_gint32(facility_counters[fac]->value, 1, NULL);
  assert_gint32(severity_counters[sev]->value, 1, NULL);
  cfg_free(cfg);
  clean_test();
}

static void
__callback(PropertyContainer *self, const gchar *name, Property *prop, gpointer user_data)
{
  GHashTable *table = (GHashTable *)user_data;
  g_hash_table_insert(table, (gchar *)name, prop);
}

void
test_hds_extension()
{
  init_test(1);
  gboolean new;

  GHashTable *table = g_hash_table_new(g_str_hash, g_str_equal);
  StatsCounter *sc_processed = stats_register_dynamic_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &counter, &new);
  HDSHandle handle = sc_processed->super.owner;
  StatsCounter *sc_hds = (StatsCounter *)hds_acquire_property_container(handle, NULL);
  assert_gpointer(sc_processed, sc_hds, NULL);
  assert_gpointer(&sc_processed->counters[SC_TYPE_PROCESSED], counter, NULL);
  property_container_foreach(&sc_hds->super, __callback, table);

  StatsCounterItem *item = (StatsCounterItem *)g_hash_table_lookup(table, "processed");
  assert_gpointer(item, counter, NULL);
  g_hash_table_unref(table);

  StatsCounterItem *static_counter = NULL;
  StatsCounter test_key;

  stats_register_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &static_counter);
  stats_counter_init_instance(&test_key, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1");
  const gchar *key_hds = __build_hds_path(SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1");
  handle = hds_get_handle(key_hds);
  assert_not_null(handle, NULL);
  sc_hds = (StatsCounter *)hds_acquire_property_container(handle, NULL);
  assert_not_null(sc_hds, NULL);
  assert_gpointer(&sc_hds->counters[SC_TYPE_PROCESSED], static_counter, NULL);

  clean_test();
}

void
test_hds_key()
{
  init_test(1);
  gboolean new = FALSE;
  StatsCounter *sc_processed = stats_register_dynamic_counter(1, SCS_TCP | SCS_SOURCE, "s_tcp#0", "127.0.0.1", SC_TYPE_PROCESSED, &counter, &new);
  HDSHandle handle = sc_processed->super.owner;
  gchar *key_hds = hds_handle_get_fqdn(handle);
  assert_string(key_hds, "source.tcp.s_tcp#0.127.0.0.1.stats", NULL);
  g_free(key_hds);

  sc_processed = stats_register_dynamic_counter(1, SCS_JAVA | SCS_SOURCE, NULL, NULL, SC_TYPE_PROCESSED, &counter, &new);
  handle = sc_processed->super.owner;
  key_hds = hds_handle_get_fqdn(handle);
  assert_string(key_hds, "source.java.stats", NULL);
  g_free(key_hds);

  sc_processed = stats_register_dynamic_counter(1, SCS_GROUP | SCS_SOURCE, "s_java", NULL, SC_TYPE_PROCESSED, &counter, &new);
  handle = sc_processed->super.owner;
  key_hds = hds_handle_get_fqdn(handle);
  assert_string(key_hds, "source.s_java.stats", NULL);
  g_free(key_hds);

  sc_processed = stats_register_dynamic_counter(1, SCS_GROUP | SCS_DESTINATION, "d_something", NULL, SC_TYPE_PROCESSED, &counter, &new);
  handle = sc_processed->super.owner;
  key_hds = hds_handle_get_fqdn(handle);
  assert_string(key_hds, "destination.d_something.stats", NULL);
  g_free(key_hds);
  clean_test();
}

gint
main(gint argc, gchar **argv)
{
  test_register_counter();
  test_stats_level();
  test_empty_keys();
  test_register_new_dynamic_counter();
  test_register_already_registered_dynamic_counter();
  test_register_same_dynamic_counter_but_another_counter_item();
  test_cleanup_orphans();
  test_stats_reinit();
  test_hds_extension();
  test_hds_key();
  return 0;
}
