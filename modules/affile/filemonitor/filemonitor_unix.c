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

static gchar*
build_filename(const gchar *basedir, const gchar *path)
{
  gchar *result;

  if (!path)
    return NULL;

  result = (gchar *)g_malloc(strlen(basedir) + strlen(path) + 2);
  sprintf(result, "%s/%s", basedir, path);

  return result;
}

static inline long
get_path_max()
{
  long path_max;
#ifdef PATH_MAX
  path_max = PATH_MAX;
#else
  path_max = pathconf(path, _PC_PATH_MAX);
  if (path_max <= 0)
  path_max = 4096;
#endif
  return path_max;
}

/*
 Resolve . and ..
 Resolve symlinks
 Resolve tricki symlinks like a -> ../a/../a/./b
*/
gchar*
resolve_to_absolute_path(const gchar *path, const gchar *basedir)
{
  long path_max = get_path_max();
  gchar *res;
  gchar *w_name;

  w_name = build_filename(basedir, path);
  res = (char *)g_malloc(path_max);

  if (!realpath(w_name, res))
    {
      g_free(res);
      if (errno == ENOENT)
        {
          res = g_strdup(path);
        }
      else
        {
          msg_error("Can't resolve to absolute path", evt_tag_str("path", path), evt_tag_errno("error", errno), NULL);
          res = NULL;
        }
    }
  g_free(w_name);
  return res;
}


/**************************************************************************/

static gboolean file_monitor_chk_file(FileMonitor * monitor, MonitorBase *source, const gchar *filename);

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

static inline void
_raise_read_permissions(FileMonitor *self)
{
  if (self->privileged)
    raise_syslog_privileged_read_permissions();
  else
    raise_read_permissions();
}

cap_t
file_monitor_raise_caps(FileMonitor *self)
{
  cap_t old = g_process_cap_save();
  _raise_read_permissions(self);
  return old;
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
          if (self->recursion)
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

void
file_monitor_start(FileMonitor *self, MonitorBase *source, const gchar *base_dir)
{
  switch(self->monitor_type)
    {
#if ENABLE_MONITOR_INOTIFY
      case MONITOR_INOTIFY:
         file_monitor_start_inotify(self, source, base_dir);
         break;
#endif
       case MONITOR_POLL:
         file_monitor_start_poll(self, source, base_dir);
         break;
       default:
         g_assert_not_reached();
    }
}

gboolean
file_monitor_watch_directory(FileMonitor *self, const gchar *filename)
{
  MonitorBase *source = NULL;
  gchar *base_dir;
  g_assert(self);
  g_assert(filename);

  if (!g_file_test(filename, G_FILE_TEST_IS_DIR))
    {
      /* if the filename is not a directory then remove the file part and try only the directory part */
      gchar *dir_part = g_path_get_dirname (filename);

      if (g_path_is_absolute (dir_part))
        {
          base_dir = resolve_to_absolute_path(dir_part, G_DIR_SEPARATOR_S);
        }
      else
        {
          gchar *wd = g_get_current_dir();
          base_dir = resolve_to_absolute_path(dir_part, wd);
          g_free(wd);
        }

      g_free(dir_part);

      if (!self->compiled_pattern)
        {
          gchar *pattern = g_path_get_basename(filename);
          self->compiled_pattern = g_pattern_spec_new(pattern);
          g_free(pattern);
        }
    }
  else
    {
       base_dir = g_strdup(filename);
    }

  if (base_dir == NULL || !g_path_is_absolute (base_dir))
    {
      msg_error("Can't monitor directory, because it can't be resolved as absolute path", evt_tag_str("base_dir", base_dir), NULL);
      return FALSE;
    }

  if (file_monitor_is_dir_monitored(self, base_dir))
    {
      /* We are monitoring this directory already, so ignore it */
      g_free(base_dir);
      return TRUE;
    }
  else
    {
      msg_debug("Monitoring new directory", evt_tag_str("basedir", base_dir), NULL);
    }
  switch(self->monitor_type)
    {
      /* Setup inotify, if supported */
#if ENABLE_MONITOR_INOTIFY
      case MONITOR_INOTIFY:
        source = file_monitor_create_inotify(self, base_dir);
        break;
#endif
      case MONITOR_POLL:
        source = file_monitor_create_poll(self, base_dir);
        break;
      default:
#if ENABLE_MONITOR_INOTIFY
        self->monitor_type = MONITOR_INOTIFY;
        source = file_monitor_create_inotify(self, base_dir);
#endif
        if (!source)
          {
            self->monitor_type = MONITOR_POLL;
            source = file_monitor_create_poll(self, base_dir);
            if (!source)
              {
                self->monitor_type = MONITOR_NONE;
              }
          }
    }

  if (source == NULL)
    {
      msg_error("Failed to initialize file monitoring", evt_tag_str("basedir", base_dir), NULL);
      g_free(base_dir);
      return FALSE;
    }

  self->sources = g_slist_append(self->sources, source);
  file_monitor_start(self, source, base_dir);
  g_free(base_dir);
  return TRUE;
}


/**
 * file_monitor_new:
 *
 * This function constructs a new FileMonitor object.
 **/
FileMonitor *
file_monitor_new(void)
{
  FileMonitor *self = g_new0(FileMonitor, 1);

  self->sources = NULL;
  self->file_callback = NULL;
  self->recursion = FALSE;
  self->monitor_type = MONITOR_NONE;
  self->poll_freq = 1000; /* 1 sec is the default*/
  self->privileged = FALSE;
  return self;
}


void
file_monitor_deinit(FileMonitor *self)
{
  if (self->sources) {
    if (self->monitor_type == MONITOR_POLL)
      {
        g_slist_foreach(self->sources, file_monitor_poll_destroy, NULL);
      }
    else
      {
#if ENABLE_MONITOR_INOTIFY
        g_slist_foreach(self->sources, file_monitor_inotify_destroy, NULL);
#else
        abort();
#endif
      }
  }
}

void
file_monitor_free(FileMonitor *self)
{
  if (self->compiled_pattern)
    {
      g_pattern_spec_free(self->compiled_pattern);
      self->compiled_pattern = NULL;
    }
  if (self->sources)
    {
      GSList *source_list = self->sources;
      while(source_list)
        {
          g_free(source_list->data);
          source_list = source_list->next;
        }
      g_slist_free(self->sources);
      self->sources = NULL;
    }
  g_free(self);
}

cap_t
file_monitor_raise_caps(FileMonitor *self)
{
  cap_t old = g_process_cap_save();
  _raise_read_permissions(self);
  return old;
}
