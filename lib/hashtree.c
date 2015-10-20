/*
 * Copyright (c) 2010-2015 BalaBit IT Ltd, Budapest, Hungary
 * Copyright (c) 2010-2015 Laszlo Budai <Laszlo.Budai@balabit.com>
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

#include "hashtree.h"
#include <string.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

struct _HNode
{
  HNode *parent;
  gpointer value;
  gchar *key;
  GHashTable *children;
  const gchar *delim;
};

static gchar **
_path_to_array(const gchar *path, const gchar *delim)
{
  return g_strsplit(path, delim, -1);
}

static HNode *
_hnode_create(const gchar *key, const gchar *delim)
{
  HNode *node = g_new(HNode, 1);

  node->parent = NULL;
  node->value = NULL;
  node->children = NULL;
  node->key = g_strdup(key);
  node->delim = delim;

  return node;
}

static inline gboolean
_hnode_is_leaf(HNode *node)
{
  return (node->children == NULL);
}

static inline gboolean
_hnode_is_root(HNode *node)
{
  return (node->parent == NULL);
}

static void
_hnode_append(HNode *root, HNode *node)
{
   if (!node)
     return;

   node->parent = root;

   if (!root->children)
     root->children = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);

   g_hash_table_insert(root->children, node->key, node);
}

static HNode*
_insert_key(HNode *parent_node, const gchar *key)
{
  HNode *node = _hnode_create(key, parent_node->delim);
  node->parent = parent_node;

  if (_hnode_is_root(node))
    return node;

  if (_hnode_is_leaf(parent_node))
    parent_node->children = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);

  g_hash_table_insert(parent_node->children, node->key, node);

  return node;
}

static inline gboolean
_is_single_path(const gchar *path, const gchar *delim)
{
  const gchar *it;
  gboolean contains_delim = FALSE;

  for (it = delim; *it && !contains_delim; ++it)
    contains_delim = (strchr(path, *it) != NULL);

  return !contains_delim;
}

static HNode*
_htree_insert_keys(HNode *root, gchar **keys, const gchar *delim)
{
  HNode *node;
  gchar **it = keys;

  if (!root)
    {
      root = _hnode_create(*it, delim);
      ++it;
    }

  for (node = root; it && *it; it++)
    {
      node = _insert_key(node, *it);
    }

  return node;
}

static HNode*
_htree_insert(HNode *root, const gchar *path, const gchar *delim)
{
  HNode *node = NULL;
  gchar **keys = _path_to_array(path, delim);

  node = _htree_insert_keys(root, keys, delim);

  g_strfreev(keys);

  return node;
}

static HNode*
_longest_match_key_array(HNode *root, gchar ***keys)
{
  HNode *node, *res = NULL;
  gboolean matching = TRUE;

  for (node = root; matching && *keys && **keys; (*keys)++)
    {
      if (_hnode_is_leaf(node))
        {
          res = node;
          node = NULL;
        }
      else
        {
          node = g_hash_table_lookup(node->children, **keys);
        }

      if (node)
        {
          res = node;
        }
      else
        {
          matching = FALSE;
        }
   }

  if (!matching)
    (*keys)--;

  return res;
}

static inline HNode*
_hnode_get_child_by_key(HNode *node, const gchar *key)
{
  if (!node || _hnode_is_leaf(node))
    return NULL;

  return g_hash_table_lookup(node->children, key);
}

static HNode*
_htree_find_longest_match(HNode *root, gchar **keys, gchar ***remain)
{
  HNode *res = NULL;
  gchar **it = keys;

  if (!root)
    {
      *remain = keys;
      return NULL;
    }

  if (g_strv_length(keys) == 1)
    {
      res = _hnode_get_child_by_key(root, keys[0]);

      if (!res)
        *remain = keys;

      return res;
    }

  res = _longest_match_key_array(root, &it);

  if (*it)
    *remain = it;

  return res;
}

HNode *
htree_get_top_root(HNode *node)
{
  HNode *root = node;

  while (node != NULL)
    {
      root = node;
      node = node->parent;
    }

  return root;
}

HNode*
htree_new(const gchar *path, const gchar *delim)
{
  if (!path)
    return NULL;

  if (_is_single_path(path, delim))
    {
      return _hnode_create(path, delim);
    }

  return htree_get_top_root(_htree_insert(NULL, path, delim));
}

HNode*
htree_insert(HNode *root, const gchar *path)
{
  HNode *node = NULL;
  gchar **keys;
  gchar **remaining_keys = NULL;

  if (!root)
    return NULL;

  keys = _path_to_array(path, root->delim);

  node = _htree_find_longest_match(root, keys, &remaining_keys);

  if (node)
    root = node;

  node = _htree_insert_keys(root, remaining_keys, root->delim);

  g_strfreev(keys);

  return node;
}

void
htree_free(HNode *root)
{
  htree_free_full(root, NULL);
}

static void
_hnode_free(HNode *node, GDestroyNotify data_free_fn)
{
  if (data_free_fn && node->value)
    data_free_fn(node->value);

  if (!_hnode_is_leaf(node))
    g_hash_table_unref(node->children);

  g_free(node->key);
  g_free(node);
}

void
htree_free_full(HNode *root, GDestroyNotify data_free_fn)
{
  GList *nodes, *it;

  if (!root)
    return;

  if (_hnode_is_leaf(root))
    {
      _hnode_free(root, data_free_fn);
      return;
    }

  nodes = g_hash_table_get_values(root->children);
  for (it = nodes; it; it = it->next)
    {
      HNode *n = (HNode *)it->data;
      htree_free_full(n, data_free_fn);
    }
  g_list_free(nodes);
  _hnode_free(root, data_free_fn);

}

HNode*
htree_find(HNode *root, const gchar *path)
{
  gchar **remain = NULL;
  HNode *node;
  gchar **keys;

  if (!root || path == NULL)
    return root;

  keys = _path_to_array(path, root->delim);
  node = _htree_find_longest_match(root, keys, &remain);

  if (remain)
    node = NULL;

  g_strfreev(keys);

  return node;
}

HNode*
htree_find_longest_match(HNode *root, const gchar *path, gchar **remain)
{
  HNode *res;
  gchar **keys;
  gchar **_remain = NULL;

  if (!root || !path)
    return root;

  keys = _path_to_array(path, root->delim);
  res = _htree_find_longest_match(root, keys, &_remain);

  if (_remain)
    {
      if (_remain == keys)
        *remain = g_strdup(path);
      else
        *remain = g_strjoinv(root->delim, _remain);
    }

  g_strfreev(keys);

  return res;
}

HNode*
htree_unlink(HNode *root, const gchar *path)
{
  HNode *node = htree_find(root, path);

  if (node && node->parent)
    {
      g_hash_table_remove(node->parent->children, node->key);
      node->parent = NULL;
    }

  return node;
}

HNode*
htree_join(HNode *root, HNode *node)
{
  if (!root || !node)
    return root;

  _hnode_append(root, node);

   return root;
}

guint
htree_get_size(HNode *root, const gchar *path)
{
  HNode *node = htree_find(root, path);

  if (!node)
    return 0;

  return hnode_get_size(node);
}

guint
htree_get_n_children(HNode *root, const gchar *path)
{
  if (!root)
    return 0;

  return htree_get_size(root, path) - 1;
}

guint
htree_get_height(HNode *root, const gchar *path)
{
  HNode *node = htree_find(root, path);

  if (!node)
    return 0;

  return hnode_get_height(node);
}

gpointer
htree_get_value(HNode *root, const gchar *path)
{
  HNode *node = htree_find(root, path);

  if (!node)
    return NULL;

  return node->value;
}

gboolean
htree_set_value(HNode *root, const gchar *path, gpointer value)
{
  HNode *node = htree_find(root, path);

  if (!node)
    return FALSE;

  hnode_set_value(node, value);

  return TRUE;
}

HNode*
htree_get_parent(HNode *root, const gchar *path)
{
  HNode *node = htree_find(root, path);

  if (!node)
    return NULL;

  if (node)
    {
      return node->parent;
    }

  return NULL;
}

const gchar*
htree_get_key(HNode *root, const gchar *path)
{
  HNode *node = htree_find(root, path);

  if (!node)
    return NULL;

  if (node)
    {
      return node->key;
    }

  return NULL;
}

const gchar *
hnode_get_key(HNode *node)
{
  return node->key;
}

gpointer
hnode_get_value(HNode *node)
{
  return node->value;
}

void
hnode_set_value(HNode *node, gpointer value)
{
  node->value = value;
}

HNode *
hnode_get_parent(HNode *node)
{
  return node->parent;
}

static void
_count(HNode *node, gpointer arg)
{
  guint *n = (guint *)arg;
  ++(*n);
}

guint
hnode_get_size(HNode *node)
{
  guint n = 0;

  htree_foreach(node, _count, &n);
  return n;
}

guint
hnode_get_n_children(HNode *node)
{
  return hnode_get_size(node) - 1;
}

guint
hnode_get_height(HNode *node)
{
  guint height = 0;

  if (!node || !node->children)
    return 0;

  if (node->children)
    {
      GList *children = g_hash_table_get_values(node->children);
      GList *it;
      for (it = children; it; it = it->next)
        height = MAX(hnode_get_height(it->data), height);
      g_list_free(children);
    }

  return height + 1;
}

gchar *
hnode_get_fqdn(HNode *node)
{
  GString *fqdn = g_string_new(node->key);
  HNode *parent = node->parent;
  while (parent)
    {
      fqdn = g_string_prepend_c(fqdn, '.');
      fqdn = g_string_prepend(fqdn, parent->key);
      parent = parent->parent;
    }
  return g_string_free(fqdn, FALSE);
}

static void
_hashtable_foreach(gpointer key, gpointer value, gpointer data)
{
  gpointer *args = (gpointer *)data;
  HNode *node = (HNode *)value;
  htree_traversal_fn traversal_fn = (htree_traversal_fn) args[0];

  htree_foreach(node, traversal_fn, args[1]);
}

void
htree_foreach(HNode *root, htree_traversal_fn fn, gpointer arg)
{
  if (!root)
    return;

  gpointer args[] = {fn, arg};
  fn(root, arg);

  if (!_hnode_is_leaf(root))
    g_hash_table_foreach(root->children, _hashtable_foreach, args);
}
