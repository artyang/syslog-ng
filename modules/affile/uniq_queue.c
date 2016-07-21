/*
 * Copyright (c) 2014 Balabit
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

#include "uniq_queue.h"
#include <glib.h>

struct _UniqQueue {
  GQueue *queue;
  GHashTable *dict;
};

gchar *
uniq_queue_pop_head(UniqQueue *self)
{
  gchar *element = NULL;
  if (!g_queue_is_empty(self->queue))
  {
    element = g_queue_pop_head(self->queue);
    g_hash_table_remove(self->dict, element);
  }
  return element;
}

gboolean
uniq_queue_is_empty(UniqQueue *self)
{
  return g_queue_is_empty(self->queue);
}

guint
uniq_queue_length(UniqQueue *self)
{
  return g_queue_get_length(self->queue);
}

void
uniq_queue_push_tail(UniqQueue *self, gchar *element)
{
  g_queue_push_head(self->queue, element);
  g_hash_table_insert(self->dict, element, NULL);
}

gboolean
uniq_queue_check_element(UniqQueue *self, const gchar *element)
{
  return g_hash_table_lookup_extended(self->dict, element, NULL, NULL);
}

void
uniq_queue_free(UniqQueue *self)
{
  g_queue_free(self->queue);
  g_hash_table_unref(self->dict);
}

UniqQueue *
uniq_queue_new()
{
  UniqQueue *self = g_new0(UniqQueue,1);
  self->queue = g_queue_new();
  self->dict = g_hash_table_new(g_str_hash, g_str_equal);
  return self;
}
