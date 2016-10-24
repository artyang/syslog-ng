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

static inline void
_raise_read_permissions(FileMonitor *self)
{
  if (self->privileged)
    raise_syslog_privileged_read_permissions();
  else
    raise_read_permissions();
}

cap_t
file_monitor_unix_raise_caps(FileMonitor *self)
{
  cap_t old = g_process_cap_save();
  _raise_read_permissions(self);
  return old;
}
