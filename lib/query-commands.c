/*
 * Copyright (c) 2015 Balabit
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

#include "query-commands.h"
#include "hds.h"
#include "messages.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h> // TODO

enum QueryCommands {
  QUERY_SUM = 0,
  QUERY_SUM_AGGREGATE,
  QUERY_LIST,
  QUERY_CMD_MAX
};

typedef GString *(*QUERY_CMD)(const gchar *hds_path);


static void
_on_match_collect_property_key_value(Property *prop, const gchar *fqdn, gpointer user_data)
{
  GString *result = (GString *) user_data;
  const gchar *value = property_to_string(prop);
  if (value)
    g_string_append_printf(result, "%s: %s\n", fqdn, value);
  else
    msg_debug("Skipping property value from query because its value is null",
              evt_tag_str("path", fqdn),
              NULL);
}

static GString*
_query_sum(const gchar *hds_path)
{
  return hds_query_properties(hds_path, (PROP_MATCH)_on_match_collect_property_key_value, g_string_new(""));
}

static void
_on_match_collect_property_key(Property *prop, const gchar *fqdn, gpointer user_data)
{
  GString *result = (GString *) user_data;
  g_string_append_printf(result, "%s\n", fqdn);
}

static GString*
_query_list(const gchar *hds_path)
{
  return hds_query_properties(hds_path, (PROP_MATCH)_on_match_collect_property_key, g_string_new(""));
}

static void
_on_match_collect_property_aggr(Property *prop, const gchar *fqdn, gpointer user_data)
{
  long int *sum = (long int *) user_data;
  const gchar *value = property_to_string(prop);
  if (value)
    {
      errno = 0;
      long int number = strtol (value, NULL, 10);
      if (errno != 0)
        {
          msg_error("Internal error while converting property to number",
                    evt_tag_str("path", fqdn),
                    evt_tag_str("value", value),
                    evt_tag_str("error", g_strerror (errno)),
                    NULL);
        }
      else
        {
          *sum += number;
        }
    }
}

static GString*
_query_sum_aggregate(const gchar *hds_path)
{
  long int sum = 0;
  hds_query_properties(hds_path, (PROP_MATCH)_on_match_collect_property_aggr, &sum);

  GString *result = g_string_new("");
  g_string_printf(result, "%ld\n", sum);
  return result;
}

static gint
_command_str_to_id(const gchar *cmd)
{
  if (g_str_equal(cmd, "SUM_AGGREGATE"))
    return QUERY_SUM_AGGREGATE;

  if (g_str_equal(cmd, "SUM"))
    return QUERY_SUM;

  if (g_str_equal(cmd, "LIST"))
    return QUERY_LIST;

  return QUERY_CMD_MAX;
}

static QUERY_CMD QUERY_CMDS[] = {
  _query_sum,
  _query_sum_aggregate,
  _query_list
};

static GString*
_dispatch_query(gint cmd_id, const gchar *hds_path)
{
  g_assert(cmd_id >= QUERY_SUM && cmd_id < QUERY_CMD_MAX);

  return QUERY_CMDS[cmd_id](hds_path);
}

GString*
process_query_command(GString *command)
{
  GString *result;
  gchar **cmds = g_strsplit(command->str, " ", 3);

  g_assert(g_str_equal(cmds[0], "QUERY"));

  result = _dispatch_query(_command_str_to_id(cmds[1]), cmds[2]);

  g_strfreev(cmds);
  return result;
}
