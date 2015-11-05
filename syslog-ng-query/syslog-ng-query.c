/*
 * Copyright (c) 2002-2015 BalaBit
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

#include "syslog-ng.h"
#include "gsocket.h"
#include "control_client.h"
#include "cfg.h"
#include "reloc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if HAVE_GETOPT_H
#include <getopt.h>
#endif

static gchar *control_name = PATH_CONTROL_SOCKET;
static ControlClient *control_client;

void usage(const gchar *bin_name);

static gboolean
slng_send_cmd(const gchar * const cmd)
{
  if (!control_client_connect(control_client))
    {
      return FALSE;
    }

  if (control_client_send_command(control_client,cmd) < 0)
    {
      return FALSE;
    }

  return TRUE;
}

static GString *
slng_run_command(const gchar * const command)
{
  if (slng_send_cmd(command))
    return control_client_read_reply(control_client);

  return NULL;
}

static gboolean sum_aggr = FALSE;

static GOptionEntry sum_options[] =
{
  { "aggregate", 'a', 0, G_OPTION_ARG_NONE, &sum_aggr,
      "aggregate all values to a single number", NULL },
  { NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL }
};

static GString *_sum_command_builder(const gchar *path) {
  GString *cmd;
  const gchar *subcmd;

  if (sum_aggr)
    subcmd = "SUM_AGGREGATE";
  else
    subcmd = "SUM";

  cmd = g_string_new("");
  g_string_printf(cmd, "QUERY %s %s\n", subcmd, path);
  return cmd;
}

static gint
slng_sum (int argc, char *argv[], const gchar *mode)
{
  GString *rsp, *cmd;

  if (argc != 2)
    {
      fprintf (stderr, "error: need a path argument\n");
      usage (argv[0]);
      return 1;
    }

  cmd = _sum_command_builder(argv[1]);
  rsp = slng_run_command (cmd->str);
  g_string_free(cmd, TRUE);
  if (rsp == NULL)
    return 1;

  printf("%s", rsp->str);
  if (rsp->len > 0)
    printf("\n");

  g_string_free(rsp, TRUE);

  return 0;
}

static gint
_slng_list (const gchar * path)
{
  GString *cmd = g_string_new("");
  g_string_printf(cmd, "QUERY LIST %s\n", path);
  GString *rsp = slng_run_command (cmd->str);
  g_string_free(cmd, TRUE);

  if (rsp == NULL)
    return 1;

  printf ("%s", rsp->str);
  if (rsp->len > 0)
    printf("\n");

  g_string_free (rsp, TRUE);

  return 0;
}

static gint
slng_list (int argc, char *argv[], const gchar *mode)
{
  if (argc == 1) {
      return _slng_list("*");
  } else if (argc == 2) {
      return _slng_list(argv[1]);
  } else {
      fprintf (stderr, "error: need a path argument or default to *\n");
      usage (argv[0]);
      return 1;
    }
}

static GOptionEntry no_options[] =
{
  { NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL }
};

const gchar *
get_mode(int *argc, char **argv[])
{
  gint i;
  const gchar *mode;

  for (i = 1; i < (*argc); i++)
    {
      if ((*argv)[i][0] != '-')
        {
          mode = (*argv)[i];
          memmove(&(*argv)[i], &(*argv)[i+1], ((*argc) - i) * sizeof(gchar *));
          (*argc)--;
          return mode;
        }
    }
  return NULL;
}

static GOptionEntry slng_options[] =
{
  { "control", 'c', 0, G_OPTION_ARG_STRING, &control_name,
    "syslog-ng control socket", "<socket>" },
  { NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL }
};

static struct
{
  const gchar *mode;
  const GOptionEntry *options;
  const gchar *description;
  gint (*main)(gint argc, gchar *argv[], const gchar *mode);
} modes[] =
  {
    { "list", no_options, "lists the available property names with "
        "full path/FQDN, but without the top root node", slng_list },
    { "sum", sum_options, "summarize values stored in nodes that are matched "
        "to the filter/query result", slng_sum },
    { NULL, NULL },
  };

static struct
{
  const gchar *query;
  const gchar *description;
} queries[] =
  {
    { "sum \"destination.tcp.d_network#0.10.50.0.111.514.stored\"",
        "number of messages stored in the given message queue" },
    { "sum 'destination.tcp.d_network#?.*.stored'",
        "messages stored in each message queue and a sum" },
    { "sum --aggregate 'destination.tcp.d_network#0.10.50.0.111.514.stored'",
        "only output total sum as a single number" },
    { NULL, NULL }
  };

void
usage(const gchar *bin_name)
{
  gint mode, query;

  fprintf(stderr, "Syntax: %s <command> [options]\nPossible commands are:\n", bin_name);
  for (mode = 0; modes[mode].mode; mode++)
    {
      fprintf(stderr, "    %-20s %s\n", modes[mode].mode, modes[mode].description);
    }

  fprintf(stderr, "Query examples:\n");
  for (query = 0; queries[query].query; query++)
    {
      fprintf(stderr, "    %s\n            %s\n", queries[query].query, queries[query].description);
    }
  exit(1);
}

#include "reloc.c"

int
main(int argc, char *argv[])
{
  const gchar *mode_string;
  GOptionContext *ctx;
  gint mode;
  GError *error = NULL;
  int result;

  control_name = get_reloc_string(PATH_CONTROL_SOCKET);

  mode_string = get_mode(&argc, &argv);
  if (!mode_string)
    {
      usage(argv[0]);
    }

  ctx = NULL;
  for (mode = 0; modes[mode].mode; mode++)
    {
      if (strcmp(modes[mode].mode, mode_string) == 0)
        {
          ctx = g_option_context_new(mode_string);
          #if GLIB_CHECK_VERSION (2, 12, 0)
          g_option_context_set_summary(ctx, modes[mode].description);
          #endif
          g_option_context_add_main_entries(ctx, modes[mode].options, NULL);
          g_option_context_add_main_entries(ctx, slng_options, NULL);
          break;
        }
    }
  if (!ctx)
    {
      fprintf(stderr, "Unknown command\n");
      usage(argv[0]);
    }

  if (!g_option_context_parse(ctx, &argc, &argv, &error))
    {
      fprintf(stderr, "Error parsing command line arguments: %s\n", error ? error->message : "Invalid arguments");
      g_clear_error(&error);
      g_option_context_free(ctx);
      return 1;
    }
  g_option_context_free(ctx);

  control_client = control_client_new(control_name);

  result = modes[mode].main(argc, argv, modes[mode].mode);
  control_client_free(control_client);
  return result;
}
