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

#include "hds.h"
#include "hashtree.h"
#include "property_container.h"
#include "nv_property_container.h"

#define ROOT_NAME "syslog-ng"

typedef struct _HDS HDS;

struct _HDS {
  HNode *root;
  GMutex *mutex;
};

HDS *global_hds;

static HDS *
hds_new()
{
  HDS *self = g_new0(HDS, 1);
  self->root = htree_new(ROOT_NAME, ".");
  if (!g_threads_got_initialized)
    {
      g_thread_init(NULL);
    }
  self->mutex = g_mutex_new();
  hds_acquire_property_container(self->root, nv_property_container_new);
  return self;
}

static void
hds_free(HDS *self)
{
  htree_free_full(self->root, (GDestroyNotify)property_container_free);
  g_mutex_free(self->mutex);
  g_free(self);
}


void hds_init()
{
  if (global_hds == NULL)
    global_hds = hds_new();
}

void hds_destroy()
{
  if (global_hds)
    {
      hds_free(global_hds);
      global_hds = NULL;
    }
}

HDSHandle
hds_get_root()
{
  return global_hds->root;
}

const gchar *
hds_get_root_name()
{
  return ROOT_NAME;
}

void hds_lock()
{
  g_mutex_lock(global_hds->mutex);
}

void hds_unlock()
{
  g_mutex_unlock(global_hds->mutex);
}

HDSHandle
hds_register_handle(const gchar *name)
{
  return htree_insert(global_hds->root, name);
}

void
hds_unregister_handle(HDSHandle handle)
{
  HNode *parent = hnode_get_parent(handle);
  HNode *node = htree_unlink(parent, hnode_get_key(handle));
  htree_free_full(node, (GDestroyNotify)property_container_free);
}

HDSHandle
hds_get_handle(const gchar *name)
{
  return htree_find(global_hds->root, name);
}

PropertyContainer *hds_acquire_property_container(HDSHandle handle, PROPERTY_CONTAINER_CONSTRUCTOR constructor)
{
  PropertyContainer *container = (PropertyContainer *)hnode_get_value(handle);
  if (!container && constructor)
    {
      container = constructor(handle);
      hnode_set_value(handle, container);
    }
  return container;
}

PropertyContainer *
hds_get_property_container(HDSHandle handle)
{
  return (PropertyContainer *)hnode_get_value(handle);
}

const gchar *
hds_get_value(const gchar *path)
{
  gchar *remain = NULL;
  const gchar *result = NULL;
  HNode *node = htree_find_longest_match(global_hds->root, path, &remain);
  if (remain != NULL)
    {
      result = property_to_string(property_container_get_property(hnode_get_value(node), remain));
    }
  g_free(remain);
  return result;
}

typedef GList *(*ON_MATCH)(HNode *node, const gchar *fqdn, GList **list);

static GList *
__on_match_collect_name(HNode *node, const gchar *fqdn, GList **list)
{
  *list = g_list_append(*list, g_strdup(fqdn));
  return *list;
}

static GList *
__on_match_collect_node(HNode *node, const gchar *fqdn, GList **list)
{
  *list = g_list_append(*list, node);
  return *list;
}


static void
__matching_pattern(HNode *node, gpointer args)
{
  gpointer *data = (gpointer *)args;
  const gchar *pattern = data[0];
  GList **list = data[1];
  ON_MATCH on_match = data[2];
  gchar *key_name = hnode_get_fqdn(node);

  if (g_pattern_match_simple(pattern, key_name))
    {
      *list = on_match(node, key_name, list);
    }
  g_free(key_name);
}

gchar *
hds_handle_get_fqdn(HDSHandle handle)
{
  return hnode_get_fqdn(handle);
}

gchar *hds_handle_get_name(HDSHandle handle)
{
  return g_strdup(hnode_get_key(handle));
}

static GList *
__hds_query_nodes(const gchar *pattern, GList **list, ON_MATCH on_match)
{
  gchar *remain = NULL;
  gchar *path = g_strdup_printf("%s.%s", hnode_get_key(global_hds->root), pattern);
  HNode *node = htree_find_longest_match(global_hds->root, path, &remain);
  gpointer args[] = {(gpointer)remain, list, on_match};

  htree_foreach(node ? node : global_hds->root, __matching_pattern, args);

  g_free(remain);
  g_free(path);
  return *list;
}

GList*
hds_query_keys(const gchar *pattern, GList *list)
{
  return __hds_query_nodes(pattern, &list, __on_match_collect_name);
}

GList* hds_query_handles(const gchar *pattern, GList *list)
{
  return __hds_query_nodes(pattern, &list, __on_match_collect_node);
}
