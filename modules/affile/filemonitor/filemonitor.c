/*
 * Copyright (c) 2016 Balabit
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

#include "filemonitor.h"
#include "filemonitor_inotify.h"
#include "filemonitor_poll.h"
#include "filemonitor_win.h"

void
file_monitor_set_file_callback(FileMonitor *self, FileMonitorCallbackFunc file_callback, gpointer user_data)
{
  self->file_callback = file_callback;
  self->user_data = user_data;
}

void
file_monitor_set_destroy_callback(FileMonitor *self, GSourceFunc destroy_callback, gpointer user_data)
{
  self->destroy_callback = destroy_callback;
  self->user_data = user_data;
}

FileMonitor *
file_monitor_create_instance(gint poll_freq, gboolean force_poll)
{
#if ENABLE_MONITOR_INOTIFY
  if (!force_poll)
    return file_monitor_inotify_new();
#endif

#ifdef G_OS_WIN32
  if (!force_poll)
    return file_monitor_windows_new();
#endif

  return file_monitor_poll_new(poll_freq);
}
