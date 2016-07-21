/*
 * Copyright (c) 2002-2010 Balabit
 * Copyright (c) 1998-2010 Balázs Scheidler
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

#include "children.h"
#include <atomic.h>
#include <sys/types.h>
#include <signal.h>


typedef struct _ChildEntry
{
  pid_t pid;
  gpointer callback_data;
  GDestroyNotify callback_data_destroy;
  void (*exit_callback)(pid_t pid, int status, gpointer user_data);
} ChildEntry;

GHashTable *child_hash;

/* This variable will be used concurrently from the main loop and signal handlers. */
GAtomicCounter child_count;

static void
child_manager_child_entry_free(ChildEntry *ce)
{
  if (ce->callback_data_destroy)
    ce->callback_data_destroy(ce->callback_data);
  g_free(ce);
}

void
child_manager_register(pid_t pid, void (*callback)(pid_t, int, gpointer), gpointer user_data, GDestroyNotify callback_data_destroy)
{
  ChildEntry *ce = g_new0(ChildEntry, 1);

  ce->pid = pid;
  ce->exit_callback = callback;
  ce->callback_data = user_data;
  ce->callback_data_destroy = callback_data_destroy;

  g_hash_table_insert(child_hash, &ce->pid, ce);
  g_atomic_counter_inc(&child_count);
}

void
child_manager_unregister(pid_t pid)
{
  if (g_hash_table_lookup(child_hash, &pid))
    {
      g_hash_table_remove(child_hash, &pid);
      (void)g_atomic_counter_dec_and_test(&child_count);
    }
}

void
child_manager_sigchild(pid_t pid, int status)
{
  ChildEntry *ce;

  ce = g_hash_table_lookup(child_hash, &pid);
  if (ce)
    {
      ce->exit_callback(pid, status, ce->callback_data);
      g_hash_table_remove(child_hash, &pid);
      (void)g_atomic_counter_dec_and_test(&child_count);
    }
}

static void
_kill_all_helper(gpointer key, gpointer value, gpointer user_data)
{
  pid_t pid = *((pid_t*)key);
  kill(pid, SIGKILL);
}

void
child_manager_kill_all()
{
  g_hash_table_foreach(child_hash, _kill_all_helper, NULL);
}

void
child_manager_init(void)
{
  child_hash = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, (GDestroyNotify) child_manager_child_entry_free);
  g_atomic_counter_set(&child_count, 0);
}

void
child_manager_deinit(void)
{
  g_hash_table_destroy(child_hash);
}

/* We expect that there are no more child process starts when we started the shutdown.
 * So if we read 0, it stays 0 */
gboolean
child_manager_is_empty(void)
{
  return (g_atomic_counter_get(&child_count) == 0);
}
