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

#include "query-commands.h"
#include "nv_property_container.h"
#include "testutils.h"
#include "hds.h"

#define QUERY_TESTCASE(testfunc, ...) { testcase_begin("%s(%s)", #testfunc, #__VA_ARGS__); testfunc(__VA_ARGS__); testcase_end(); }

typedef struct _IntProperty
{
  Property super;
  gint value;
  GString *value_str;
} IntProperty;

static void
int_property_free(Property *p)
{
  IntProperty *self = (IntProperty *)p;
  g_string_free(self->value_str, TRUE);
}

static const gchar*
int_property_to_string(Property *p)
{
  IntProperty *self = (IntProperty *)p;
  g_string_printf(self->value_str, "%d", self->value);

  return self->value_str->str;
}

static void
int_property_set_value(IntProperty *self, gint value)
{
  self->value = value;

  if (!self->value_str)
    self->value_str = g_string_sized_new(64);

  g_string_printf(self->value_str, "%d", self->value);
}

static IntProperty* 
int_property_new(gint value)
{
  IntProperty *self = g_new(IntProperty, 1);
  self->super.to_string = int_property_to_string;
  self->super.free_fn = int_property_free;
  self->value_str = NULL;

  int_property_set_value(self, value);

  return self;
}

static PropertyContainer*
_hds_acquire_nv_property_container(HDSHandle handle)
{
  return hds_acquire_property_container(handle, nv_property_container_new);
}

static void
_hds_set_value(const gchar *fqdn, int number)
{
  gchar *path = g_strdup(fqdn);
  gchar *property;
  gchar *sep_pos = strrchr(path, '.');
  *sep_pos = '\0';
  property = sep_pos;
  ++property;

  HDSHandle handle = hds_register_handle(path);
  IntProperty *int_prop = int_property_new(number);
  PropertyContainer *container = _hds_acquire_nv_property_container(handle);
  property_container_add_property(container, property, &(int_prop->super));
  g_free(path);
}

static void
_fill_hds_with_test_data(void)
{
  _hds_set_value("dst.java.kafka.1", 1);
  _hds_set_value("dst.java.hdfs.1", 3);
  _hds_set_value("src.java.kafka.1", 2);
  _hds_set_value("src.java.jdbc.1", 4);
  _hds_set_value("src.network.tcp.1", 10);
}

static void
_setup(void)
{
  hds_init();
  _fill_hds_with_test_data();
}

static void
_teardown(void)
{
  hds_destroy();
}

static gchar*
_process_query_command(const gchar *cmd)
{
  GString *gcmd = g_string_new(cmd);
  GString *result = process_query_command(gcmd);
  g_string_free(gcmd, TRUE);

  return g_string_free(result, FALSE);
}

void
test_list(void)
{
  gchar *list_dst;

  list_dst = _process_query_command("QUERY LIST somewhere.over.the.rainbow");
  assert_string(list_dst, "", NULL);
  g_free(list_dst);

  list_dst = _process_query_command("QUERY LIST dst.java.hdfs.1.*");
  assert_string(list_dst, "", NULL);
  g_free(list_dst);

  list_dst = _process_query_command("QUERY LIST dst.java.hdfs.1");
  assert_string(list_dst, "dst.java.hdfs.1\n", NULL);
  g_free(list_dst);

  list_dst = _process_query_command("QUERY LIST dst.*");
  assert_string(list_dst, "dst.java.kafka.1\ndst.java.hdfs.1\n", NULL);
  g_free(list_dst);

  list_dst = _process_query_command("QUERY LIST *.network.*");
  assert_string(list_dst, "src.network.tcp.1\n", NULL);
  g_free(list_dst);

  list_dst = _process_query_command("QUERY LIST *");
  assert_string(list_dst,
                "dst.java.kafka.1\n"
                "dst.java.hdfs.1\n"
                "src.network.tcp.1\n"
                "src.java.jdbc.1\n"
                "src.java.kafka.1\n",
                NULL);
  g_free(list_dst);
}

void
test_sum_implicit_glob(void)
{
  gchar *sum_dst_kafka = _process_query_command("QUERY SUM dst.java");
  assert_string(sum_dst_kafka,
                "dst.java.kafka.1: 1\n"
                "dst.java.hdfs.1: 3\n",
                NULL);
  g_free(sum_dst_kafka);

  sum_dst_kafka = _process_query_command("QUERY SUM dst.java.");
  assert_string(sum_dst_kafka,
                "dst.java.kafka.1: 1\n"
                "dst.java.hdfs.1: 3\n",
                NULL);
  g_free(sum_dst_kafka);

  sum_dst_kafka = _process_query_command("QUERY SUM *.java");
  assert_string(sum_dst_kafka, "", "explicit globbing should be consistently used");
  g_free(sum_dst_kafka);
}

void
test_sum_single_node(void)
{
  gchar *sum_dst_kafka = _process_query_command("QUERY SUM dst.java.kafka.1");

  assert_string(sum_dst_kafka, "dst.java.kafka.1: 1\n", NULL);

  g_free(sum_dst_kafka);
}

void
test_sum_subtree(void)
{
  gchar *sum_dst = _process_query_command("QUERY SUM dst.*");

  assert_string(sum_dst, "dst.java.kafka.1: 1\ndst.java.hdfs.1: 3\n", NULL);

  g_free(sum_dst);
}

void
test_sum_aggregate(void)
{
  gchar *sum_dst_kafka = _process_query_command("QUERY SUM_AGGREGATE dst.java.kafka");
  gchar *sum_dst_java = _process_query_command("QUERY SUM_AGGREGATE *.java.*");
  gchar *sum_all = _process_query_command("QUERY SUM_AGGREGATE *");

  assert_string(sum_dst_kafka, "1\n", NULL);
  assert_string(sum_dst_java, "10\n", NULL);
  assert_string(sum_all, "20\n", NULL);

  g_free(sum_dst_kafka);
  g_free(sum_dst_java);
  g_free(sum_all);
}

int main(int argc, char **argv)
{
  _setup();
  QUERY_TESTCASE(test_list);
  QUERY_TESTCASE(test_sum_implicit_glob);
  QUERY_TESTCASE(test_sum_single_node);
  QUERY_TESTCASE(test_sum_subtree);
  QUERY_TESTCASE(test_sum_aggregate);
  _teardown ();

  return 0;
}
