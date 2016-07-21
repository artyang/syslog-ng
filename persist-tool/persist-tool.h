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

#ifndef PERSIST_TOOL_H
#define PERSIST_TOOL_H 1

#include "cfg.h"
#include "persist-state.h"
#include "state.h"

#define DEFAULT_PERSIST_FILE "syslog-ng.persist"

typedef struct _PersistTool
{
  PersistState *state;
  PersistStateMode mode;
  GlobalConfig *cfg;
  gchar *persist_filename;
}PersistTool;

PersistTool *persist_tool_new(gchar *persist_filename, PersistStateMode open_mode);

void persist_tool_free(PersistTool *self);

StateHandler *persist_tool_get_state_handler(PersistTool *self, gchar *name);

void persist_tool_revert_changes(PersistTool *self);


#endif
