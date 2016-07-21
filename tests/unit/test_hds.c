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

#include "testutils.h"
#include "utils.h"
#include "apphook.h"
#include "hds.h"
#include "nv_property_container.h"

typedef struct _TestProperty {
  Property super;
  guint32 value;
  GString *value_str;
} TestProperty;

const gchar *
test_property_get_value(Property *s)
{
  TestProperty *self = (TestProperty *)s;
  g_string_printf(self->value_str, "%d", self->value);
  return self->value_str->str;
}

void
test_property_free_fn(Property *s)
{
  TestProperty *self = (TestProperty *)s;
  g_string_free(self->value_str, TRUE);
}

TestProperty *
test_property_new()
{
  TestProperty *self = g_new0(TestProperty, 1);
  self->value_str = g_string_sized_new(1024);
  self->super.to_string = test_property_get_value;
  self->super.free_fn = test_property_free_fn;
  return self;
}

void
test_register_node()
{
  HDSHandle handle = hds_register_handle("dst.java.1");
  assert_not_null(handle, NULL);

  HDSHandle sub_handle = hds_get_handle("dst.java");
  assert_not_null(sub_handle, NULL);

  gchar *name = hds_handle_get_name(sub_handle);
  assert_string(name, "java", NULL);
  g_free(name);

  name = hds_handle_get_fqdn(handle);
  assert_string(name, "dst.java.1", NULL);
  g_free(name);

  hds_unregister_handle(sub_handle);
  sub_handle = hds_get_handle("dst.java");
  assert_null(sub_handle, NULL);
}

void
_append_prop_key_value(PropertyContainer *self, const gchar *name, Property *prop, gpointer user_data)
{
  GString *concat = (GString *) user_data;
  const gchar *value = property_to_string(prop);
  if (value)
    g_string_append_printf(concat, "%s='%s'\n", name, value);
  else
    g_string_append_printf(concat, "%s=null\n", name);
}

void
test_property_handling()
{
  HDSHandle handle = hds_register_handle("dst.java.1");
  TestProperty *test_prop = test_property_new();
  Property *get_value;
  test_prop->value = 1234;

  PropertyContainer *container = hds_acquire_property_container(handle, nv_property_container_new);
  property_container_add_property(container, "test_value.dropped", &test_prop->super);

  get_value = property_container_get_property(container, "test_value.dropped");
  assert_gpointer(test_prop, get_value, NULL);
  assert_string("1234", property_to_string(get_value), "property_container_get_property() mismatch");

  GString *concat = g_string_new("");
  property_container_foreach(container, _append_prop_key_value, concat);
  assert_string("test_value.dropped='1234'\n", concat->str, "property_container_foreach() mismatch");
  g_string_free(concat, TRUE);

  assert_null(hds_get_value(NULL), NULL);
  assert_null(hds_get_value(""), NULL);
  assert_null(hds_get_value("syslog-ng"), NULL);
  assert_null(hds_get_value("dst"), NULL);
  assert_null(hds_get_value("dst.java"), NULL);
  assert_null(hds_get_value("dst.java.1"), NULL);
  assert_null(hds_get_value("dst.java.1.false_test"), NULL);
  assert_null(hds_get_value("dst.java.1.test_value"), NULL);
  assert_string("1234", hds_get_value("dst.java.1.test_value.dropped"), NULL);

  PropertyContainer *props = hds_acquire_property_container(handle, nv_property_container_new);
  assert_not_null(props, NULL);
  assert_gpointer(props, container, NULL);
  get_value = property_container_get_property(props, "test_value.dropped");
  assert_string("1234", property_to_string(get_value), NULL);
  property_container_remove_property(props, "test_value.dropped");

  get_value = property_container_get_property(props, "test_value.dropped");
  assert_null(get_value, NULL);
}

static void
_property_collector(Property *prop, const gchar *fqdn, gpointer user_data)
{
  GString *result = (GString *) user_data;
  const gchar *value = property_to_string(prop);
  g_string_append_printf(result, "%s=%s\n", fqdn, value);
}

void
test_query_properties()
{
  GString *props;
  HDSHandle handle = hds_register_handle("dst.java.1");
  TestProperty *test_prop = test_property_new();
  test_prop->value = 42;
  PropertyContainer *container = hds_acquire_property_container(handle, nv_property_container_new);
  property_container_add_property(container, "test_value.dropped", &test_prop->super);

  props = hds_query_properties("*", _property_collector, g_string_new(""));
  assert_string(props->str, "dst.java.1.test_value.dropped=42\n", "query result mismatch");
  g_string_free(props, TRUE);

  props = hds_query_properties("dst.java.1.test_value.dropped", _property_collector, g_string_new(""));
  assert_string(props->str, "dst.java.1.test_value.dropped=42\n", "query result mismatch");
  g_string_free(props, TRUE);

  hds_unregister_handle(handle);
}

int
main(int arc, gchar **argv)
{
  hds_init();
  test_register_node();
  test_property_handling();
  test_query_properties();
  hds_destroy();
  return 0;
}
