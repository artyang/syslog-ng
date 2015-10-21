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

#include "nv_property_container.h"

struct _NVPropertyContainer {
  PropertyContainer super;
  GMutex *mutex;
  GHashTable *table;
};


static void
__add_property(PropertyContainer *s, const gchar *key, Property *property)
{
  NVPropertyContainer *self = (NVPropertyContainer *)s;
  g_mutex_lock(self->mutex);
  g_hash_table_insert(self->table, g_strdup(key), property);
  g_mutex_unlock(self->mutex);
}

static void
__remove_property(PropertyContainer *s, const gchar *key)
{
  NVPropertyContainer *self = (NVPropertyContainer *)s;
  g_mutex_lock(self->mutex);
  g_hash_table_remove(self->table, key);
  g_mutex_unlock(self->mutex);
}

static Property *
__get_property(PropertyContainer *s, const gchar *key)
{
  NVPropertyContainer *self = (NVPropertyContainer *)s;
  Property *result = NULL;
  g_mutex_lock(self->mutex);
  result = g_hash_table_lookup(self->table, key);
  g_mutex_unlock(self->mutex);
  return result;
}

static void
__foreach_callback(gpointer key, gpointer value, gpointer user_data)
{
  gpointer **args = user_data;
  NVPropertyContainer *self = (NVPropertyContainer *)args[0];
  PROPERTIES_CALLBACK func = (PROPERTIES_CALLBACK)args[1];
  gpointer arg = args[2];

  func(&self->super, key, value, arg);
}

static void
__foreach(PropertyContainer *s, PROPERTIES_CALLBACK func, gpointer user_data)
{
  NVPropertyContainer *self = (NVPropertyContainer *)s;
  gpointer args[] = {self, func, user_data};
  g_mutex_lock(self->mutex);
  g_hash_table_foreach(self->table, __foreach_callback, args);
  g_mutex_unlock(self->mutex);
}

static void
__free_fn(PropertyContainer *s)
{
  NVPropertyContainer *self = (NVPropertyContainer *)s;
  g_mutex_free(self->mutex);
  g_hash_table_unref(self->table);
}

PropertyContainer *
nv_property_container_new(gpointer owner)
{
  NVPropertyContainer *self = g_new0(NVPropertyContainer, 1);
  property_container_init_instance(&self->super, owner);
  self->mutex = g_mutex_new();
  self->table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)property_free);

  self->super.add_property = __add_property;
  self->super.foreach = __foreach;
  self->super.free_fn = __free_fn;
  self->super.get_property = __get_property;
  self->super.remove_property = __remove_property;
  return &self->super;
}


