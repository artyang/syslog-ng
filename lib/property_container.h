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

#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include "syslog-ng.h"
#include "property.h"

typedef struct _PropertyContainer PropertyContainer;

typedef void (*PROPERTIES_CALLBACK)(PropertyContainer *self, const gchar *name, Property *prop, gpointer user_data);

typedef PropertyContainer *(*PROPERTY_CONTAINER_CONSTRUCTOR)(gpointer owner);

/*
 * If the add_property and remove_property are not implemented,
 * it means that container is a "static/containing preallocated properties"
 */

struct _PropertyContainer {
  gpointer owner;
  void (*add_property)(PropertyContainer *self, const gchar *key, Property *property);
  void (*remove_property)(PropertyContainer *self, const gchar *key);
  Property *(*get_property)(PropertyContainer *self, const gchar *key);
  void (*foreach)(PropertyContainer *self, PROPERTIES_CALLBACK func, gpointer user_data);
  void (*free_fn)(PropertyContainer *self);
};

static inline void
property_container_init_instance(PropertyContainer *self, gpointer owner)
{
  self->owner = owner;
}

static inline void property_container_free(PropertyContainer *self)
{
  if (self && self->free_fn)
    {
      self->free_fn(self);
    }
  g_free(self);
}

static inline void
property_container_add_property(PropertyContainer *self, const gchar *key, Property *property)
{
  if (self && self->add_property)
    {
      self->add_property(self, key, property);
    }
}

static inline void
property_container_remove_property(PropertyContainer *self, const gchar *key)
{
  if (self && self->remove_property)
    {
      self->remove_property(self, key);
    }
}

static inline Property *
property_container_get_property(PropertyContainer *self, const gchar *key)
{
  if (self && self->get_property)
    {
      return self->get_property(self, key);
    }
  return NULL;
}

static inline gpointer
property_container_get_owner(PropertyContainer *self)
{
  if (self)
    return self->owner;
  return NULL;
}

static inline void
property_container_foreach(PropertyContainer *self, PROPERTIES_CALLBACK func, gpointer user_data)
{
  if (self && self->foreach)
    {
      self->foreach(self, func, user_data);
    }
}

#endif /* PROPERTIES_H_ */
