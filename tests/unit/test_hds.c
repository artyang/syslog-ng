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
  assert_string(name, "syslog-ng.dst.java.1", NULL);
  g_free(name);

  hds_unregister_handle(sub_handle);
  sub_handle = hds_get_handle("dst.java");
  assert_null(sub_handle, NULL);
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
  assert_string("1234", property_to_string(get_value), NULL);

  assert_null(hds_get_value("dst.java"), NULL);
  assert_null(hds_get_value("dst.java.1"), NULL);
  assert_null(hds_get_value("dst.java.1.false_test"), NULL);
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

void
test_query_keys()
{
  GList *result = NULL;
  GList *iterator = NULL;

  hds_register_handle("dst.java.1");
  hds_register_handle("dst.python.something");
  hds_register_handle("dst.java.something");

  result = hds_query_keys("dst.java.*", result);
  assert_not_null(result, NULL);
  assert_guint32(g_list_length(result), 2, NULL);

  iterator = result;
  while(iterator)
    {
      gchar *name = iterator->data;
      assert_false(strcmp(name, "syslog-ng.dst.python.something") == 0, NULL);
      assert_nstring(name, strlen("syslog-ng.dst.java"), "syslog-ng.dst.java", strlen("syslog-ng.dst.java"), NULL);
      iterator = iterator->next;
    }
  g_list_free_full(result, g_free);
}

void
test_query_nodes()
{
  GList *result = NULL;
  GList *iterator = NULL;
  HDSHandle java_1 = hds_register_handle("dst.java.1");
  HDSHandle python_1  = hds_register_handle("dst.python.something");
  HDSHandle java_2 = hds_register_handle("dst.java.something");

  result = hds_query_handles("dst.java.*", result);
  iterator = result;
  while(iterator)
    {
      HDSHandle handle = iterator->data;
      assert_true(handle != python_1, NULL);
      assert_true(handle == java_1 || handle == java_2, NULL);
      iterator = iterator->next;
    }
  assert_not_null(result, NULL);
  assert_guint32(g_list_length(result), 2, NULL);

  g_list_free(result);
}

int
main(int arc, gchar **argv)
{
  hds_init();
  test_register_node();
  test_property_handling();
  test_query_keys();
  test_query_nodes();
  hds_destroy();
}
