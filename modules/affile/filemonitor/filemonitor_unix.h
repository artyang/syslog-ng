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

#ifndef FILEMONITOR_UNIX_H_INCLUDED
#define FILEMONITOR_UNIX_H_INCLUDED

gchar *resolve_to_absolute_path(const gchar *path, const gchar *basedir);
cap_t file_monitor_raise_caps(FileMonitor *self);
gboolean file_monitor_chk_file(FileMonitor * monitor, MonitorBase *source, const gchar *filename);
gboolean file_monitor_list_directory(FileMonitor *self, MonitorBase *source, const gchar *basedir);
gboolean file_monitor_is_dir_monitored(FileMonitor *self, const gchar *filename);

#endif
