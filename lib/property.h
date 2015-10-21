/*
 * Copyright (c) 2002-2015 BalaBit IT Ltd, Budapest, Hungary
 * Copyright (c) 1998-2015 Viktor Juhász
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
#ifndef PROPERTY_H
#define PROPERTY_H

typedef struct _Property Property;

struct _Property {
  const gchar *(*to_string)(Property *self);
  gpointer (*get_object)(Property *self);
  void (*free_fn)(Property *self);
};

static inline const gchar *
property_to_string(Property *self)
{
  const gchar *result = NULL;
  if (self && self->to_string)
    result = self->to_string(self);
  return result;
}

static inline gpointer
property_get_object(Property *self)
{
  gpointer result = NULL;
  if (self && self->get_object)
      result = self->get_object(self);
  return result;
}

static inline void
property_free(Property *self)
{
  if (self && self->free_fn)
    {
      self->free_fn(self);
    }
  g_free(self);
}

#endif
