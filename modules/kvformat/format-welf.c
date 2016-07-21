/*
 * Copyright (c) 2015 Balabit
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

#include "format-welf.h"
#include "utf8utils.h"
#include "value-pairs.h"

static gboolean
tf_format_welf_prepare(LogTemplateFunction *self, LogTemplate *parent,
                       gint argc, gchar *argv[],
                       gpointer *state, GDestroyNotify *state_destroy,
                       GError **error)
{
  ValuePairs *vp;

  vp = value_pairs_new_from_cmdline (parent->cfg, argc, argv, error);
  if (!vp)
    return FALSE;

  *state = vp;
  *state_destroy = (GDestroyNotify) value_pairs_unref;
  return TRUE;
}

static gboolean
tf_format_welf_foreach(const gchar *name, TypeHint type, const gchar *value, gpointer user_data)
{
  GString *result = (GString *) user_data;

  if (result->len > 0)
    g_string_append(result, " ");
  g_string_append(result, name);
  g_string_append_c(result, '=');
  if (strchr(value, ' ') == NULL)
    append_unsafe_utf8_as_escaped_binary(result, value, -1, NULL);
  else
    {
      g_string_append_c(result, '"');
      append_unsafe_utf8_as_escaped_binary(result, value, -1, "\"");
      g_string_append_c(result, '"');
    }

  return FALSE;
}

static gint
tf_format_welf_strcmp(gconstpointer a, gconstpointer b)
{
  gchar *sa = (gchar *)a, *sb = (gchar *)b;
  if (strcmp (sa, "id") == 0)
    return -1;
  return strcmp(sa, sb);
}

static void
tf_format_welf_call(LogTemplateFunction *self, gpointer state, GPtrArray *arg_bufs,
                    LogMessage **messages, gint num_messages, const LogTemplateOptions *opts,
                    gint tz, gint seq_num, const gchar *context_id, GString *result)
{
  ValuePairs *vp = (ValuePairs *)state;
  gint i;

  for (i = 0; i < num_messages; i++)
    {
      value_pairs_foreach_sorted(vp,
                                 tf_format_welf_foreach, (GCompareDataFunc) tf_format_welf_strcmp,
                                 messages[i], 0, tz, opts, result);
    }

}

TEMPLATE_FUNCTION(tf_format_welf, tf_format_welf_prepare, NULL, tf_format_welf_call, NULL);
