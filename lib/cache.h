/*
 * Copyright (c) 2012 Balabit
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

#ifndef __CACHE_H
#define __CACHE_H

#include <glib.h>

typedef struct _Provider Provider;
typedef struct _Cache Cache;

struct _Provider
{
  void *(*fetch)(Provider *self, gchar *key);
  void (*free_fn)(Provider *self);
};

static inline void*
provider_fetch(Provider *self, gchar *key)
{
  if (self->fetch)
    {
      return self->fetch(self, key);
    }
  return NULL;
}

static inline void
provider_free(Provider *self)
{
  if (self->free_fn)
    {
      self->free_fn(self);
    }
  g_free(self);
}

Cache *cache_new(Provider *provider, GDestroyNotify value_destroy_func);
void *cache_lookup(Cache *self, gchar *key);
void cache_free(Cache *self);

#endif /*__CACHE_H*/
