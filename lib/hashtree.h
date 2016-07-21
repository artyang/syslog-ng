/*
 * Copyright (c) 2010-2015 Balabit
 * Copyright (c) 2010-2015 Laszlo Budai <Laszlo.Budai@balabit.com>
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

#ifndef HASHTREE_H_INCLUDED
#define HASHTREE_H_INCLUDED

#include <glib.h>

typedef struct _HNode HNode;

typedef void(*htree_traversal_fn)(HNode *node, gpointer arg);

HNode* htree_new(const gchar *path, const gchar *delim);
void htree_free(HNode *root);
void htree_free_full(HNode *root, GDestroyNotify data_free_fn);

HNode* htree_get_top_root(HNode *root);
HNode* htree_find(HNode *root, const gchar *path);
HNode* htree_find_longest_match(HNode *root, const gchar *path, gchar **remain);
HNode* htree_unlink(HNode *root, const gchar *path);
HNode* htree_join(HNode *root, HNode *subtree);
HNode* htree_insert(HNode *root, const gchar *path);

guint htree_get_size(HNode *root, const gchar *path);
guint htree_get_n_children(HNode *root, const gchar *path);
guint htree_get_height(HNode *root, const gchar *path);

gpointer htree_get_value(HNode *root, const gchar *path);
gboolean htree_set_value(HNode *root, const gchar *path, gpointer value);

HNode* htree_get_parent(HNode *root, const gchar *path);
const gchar* htree_get_key(HNode *root, const gchar *path);

const gchar* hnode_get_key(HNode *node);
gpointer hnode_get_value(HNode *node);
void hnode_set_value(HNode *node, gpointer value);
HNode* hnode_get_parent(HNode *node);
guint hnode_get_size(HNode *node);
guint hnode_get_n_children(HNode *node);
guint hnode_get_height(HNode *node);

gchar *hnode_get_fqdn(HNode *node);

void htree_foreach(HNode *root, htree_traversal_fn, gpointer data);

#endif
