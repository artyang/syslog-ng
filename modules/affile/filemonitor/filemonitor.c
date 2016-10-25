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

#include "messages.h"
#include "mainloop.h"
#include "timeutils.h"
#include "misc.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <iv.h>
#include <stdlib.h>
#include <sys/stat.h>

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

/**
 *  Problem: g_file_test(filename, G_FILE_TEST_EXISTS) invokes access(),
 *  that would check against real UID, not the effective UID.
 */
static gboolean
_file_is_regular(const gchar *filename)
{
  struct stat st;

  if (stat(filename, &st) < 0)
    return FALSE;

  return S_ISREG (st.st_mode) && !S_ISDIR (st.st_mode);
}

/**
 * file_monitor_chk_file:
 *
 * This function checks if the given filename matches the filters.
 **/
gboolean
file_monitor_chk_file(FileMonitor * monitor, MonitorBase *source, const gchar *filename)
{
  gboolean ret = FALSE;
  gchar *path = g_build_filename(source->base_dir, filename, NULL);

  if (g_pattern_match_string(monitor->compiled_pattern, filename) &&
      monitor->file_callback != NULL &&
      _file_is_regular(path))
    {
      /* FIXME: resolve symlink */
      /* callback to affile */
      if (G_LIKELY(filename != END_OF_LIST))
        msg_trace("file_monitor_chk_file filter passed", evt_tag_str("file",path),NULL);
      monitor->file_callback(path, monitor->user_data, ACTION_NONE);
      ret = TRUE;
    }
  g_free(path);
  return ret;
}

/**
 *
 * This function performs the initial iteration of a monitored directory.
 * Once this finishes we closely watch all events that change the directory
 * contents.
 **/
gboolean
file_monitor_list_directory(FileMonitor *self, MonitorBase *source, const gchar *basedir)
{
  GDir *dir = NULL;
  GError *error = NULL;
  const gchar *file_name = NULL;
  guint files_count = 0;
  cap_t caps;

  caps = file_monitor_raise_caps(self);
  /* try to open diretory */
  dir = g_dir_open(basedir, 0, &error);
  if (dir == NULL)
    {
      g_clear_error(&error);
      g_process_cap_restore(caps);
      return FALSE;
    }

  while ((file_name = g_dir_read_name(dir)) != NULL)
    {
      gchar * path = resolve_to_absolute_path(file_name, basedir);
      if (g_file_test(path, G_FILE_TEST_IS_DIR))
        {
          /* Recursion is enabled */
          if (self->options->recursion)
            file_monitor_watch_directory(self, path); /* construct a new source to monitor the directory */
        }
      else
        {
          /* if file or symlink, match with the filter pattern */
          file_monitor_chk_file(self, source, file_name);
        }
      files_count++;
      g_free(path);
    }
  msg_trace("file_monitor_list_directory directory scanning has been finished", evt_tag_int("Sum of file(s) found in directory", files_count), NULL);
  g_dir_close(dir);
  if (self->file_callback != NULL)
    self->file_callback(END_OF_LIST, self->user_data, ACTION_NONE);
  g_process_cap_restore(caps);

  return TRUE;
}


/**
 * file_monitor_is_monitored:
 *
 * Check if the directory specified in filename is already in the monitored
 * list.
 **/
gboolean
file_monitor_is_dir_monitored(FileMonitor *self, const gchar *filename)
{
  GSList *source;

  source = self->sources;
  while (source != NULL)
    {
      const gchar *chk_dir = ((MonitorBase*) source->data)->base_dir;
      if (strcmp(filename, chk_dir) == 0)
        return TRUE;
      else
        source = g_slist_next(source);
    }
  return FALSE;
}

static FileMonitor *
_create_platform_specific_async_file_monitor(FileMonitorOptions *options)
{
#if ENABLE_MONITOR_INOTIFY
  return file_monitor_inotify_new(options);
#endif

#ifdef G_OS_WIN32
  return file_monitor_windows_new(options);
#endif

  return NULL;
}

FileMonitor *
file_monitor_create_instance(FileMonitorOptions *options)
{
  FileMonitor *file_monitor = NULL;

  if (!options->force_directory_polling)
    file_monitor = _create_platform_specific_async_file_monitor(options);

  if (!file_monitor)
    file_monitor = file_monitor_poll_new(options);

  g_assert(file_monitor != NULL);

  return file_monitor;
}
