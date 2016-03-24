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

#ifndef UNIQ_QUEUE_H
#define UNIQ_QUEUE_H

#include <glib.h>

typedef struct _UniqQueue UniqQueue;

UniqQueue *uniq_queue_new();
void uniq_queue_free(UniqQueue *self);

gchar *uniq_queue_pop_head(UniqQueue *self);
void uniq_queue_push_tail(UniqQueue *self, gchar *element);
gboolean uniq_queue_check_element(UniqQueue *self, const gchar *element);
guint uniq_queue_length(UniqQueue *self);
gboolean uniq_queue_is_empty(UniqQueue *self);

#endif
