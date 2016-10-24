/*
 * Copyright (c) 2011-2015 Balabit
 * Copyright (c) 2011 Balázs Scheidler
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

#ifndef FILEMONITOR_H_INCLUDED
#define FILEMONITOR_H_INCLUDED
#include "syslog-ng.h"
#include "gprocess.h"

#define END_OF_LIST (gchar*)file_monitor_set_file_callback

typedef enum { ACTION_NONE, ACTION_CREATED, ACTION_MODIFIED, ACTION_DELETED} FileActionType;

typedef gboolean (*FileMonitorCallbackFunc)(const gchar *filename, gpointer user_data, FileActionType action_type);

typedef struct _FileMonitor FileMonitor;

struct _FileMonitor
{
  GSList *sources;
  GPatternSpec *compiled_pattern;
  FileMonitorCallbackFunc file_callback;
  GSourceFunc destroy_callback;
  gpointer user_data;
  gboolean recursion;
  gboolean privileged;

  gboolean (*watch_directory)(FileMonitor *self, const gchar *filename);
  gboolean (*stop)(FileMonitor *self);
  void (*deinit)(FileMonitor *self);
  void (*free_fn)(FileMonitor *self);
};

typedef struct _MonitorBase
{
  gchar *base_dir;
  FileMonitor *file_monitor;
  gboolean (* callback)(FileMonitor *,struct _MonitorBase *);
} MonitorBase;

static inline gboolean
file_monitor_watch_directory(FileMonitor *self, const gchar *filename)
{
  if (self->watch_directory)
    return self->watch_directory(self, filename);

  return FALSE;
}

static inline gboolean
file_monitor_stop(FileMonitor *self)
{
  if (self->stop)
    return self->stop(self);

  return FALSE;
}

static inline void
file_monitor_deinit(FileMonitor *self)
{
  if (self->deinit)
    self->deinit(self);
}

static inline void
file_monitor_free(FileMonitor *self)
{
  if (self->free_fn)
    self->free_fn(self);

  g_free(self);
}

FileMonitor *file_monitor_create_instance(gint poll_freq, gboolean force_poll);

void file_monitor_set_file_callback(FileMonitor *self, FileMonitorCallbackFunc file_callback, gpointer user_data);
void file_monitor_set_destroy_callback(FileMonitor *self, GSourceFunc destroy_callback, gpointer user_data);
cap_t file_monitor_raise_caps(FileMonitor *self);
#endif
