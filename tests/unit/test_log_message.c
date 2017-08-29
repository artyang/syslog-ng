/*
 * Copyright (c) 2002-2015 Balabit
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
#include "msg_parse_lib.h"
#include "apphook.h"

#include <stdlib.h>

LogMessage *
construct_log_message(void)
{
  const gchar *raw_msg = "foo";
  LogMessage *msg;

  msg = log_msg_new(raw_msg, strlen(raw_msg), NULL, &parse_options);
  log_msg_set_value(msg, LM_V_HOST, raw_msg, -1);
  return msg;
}

NVHandle nv_handle;
NVHandle sd_handle;
const gchar *tag_name = "tag";

LogMessage *
construct_log_message_with_all_bells_and_whistles(void)
{
  LogMessage *msg = construct_log_message();

  nv_handle = log_msg_get_value_handle("foo");
  sd_handle = log_msg_get_value_handle(".SDATA.foo.bar");

  log_msg_set_value(msg, nv_handle, "value", -1);
  log_msg_set_value(msg, sd_handle, "value", -1);
  msg->saddr = g_sockaddr_inet_new("1.2.3.4", 5050);
  log_msg_set_tag_by_name(msg, tag_name);
  return msg;
}

static void
test_log_msg_set_value_indirect_with_self_referencing_handle_results_in_a_nonindirect_value(void)
{
  LogMessage *msg;
  gssize value_len;
  NVHandle handle;

  msg = construct_log_message_with_all_bells_and_whistles();
  log_msg_set_value_indirect(msg, nv_handle, nv_handle, 0, 0, 5);
  assert_string(log_msg_get_value(msg, nv_handle, &value_len), "value", "indirect self-reference value doesn't match");
  log_msg_unref(msg);
}

static void
test_misc_stuff(void)
{
  MSG_TESTCASE(test_log_msg_set_value_indirect_with_self_referencing_handle_results_in_a_nonindirect_value);
}

static void
assert_sdata_value_equals(LogMessage *msg, const gchar *expected)
{
  GString *result = g_string_sized_new(0);

  log_msg_append_format_sdata(msg, result, 0);
  assert_string(result->str, expected, "SDATA value does not match, '%s' vs '%s'", expected, result->str);
  g_string_free(result, TRUE);
}


void
test_sdata_keys_are_sanitized(void)
{
  LogMessage *msg;
  /* These keys looks strange, but JSON object can be parsed to SDATA,
   * so the key could contain any character, while the specification
   * does not declare any way to encode the the keys, just the values.
   * The goal is to have a syntactically valid syslog message. */

  msg = log_msg_new_empty();
  log_msg_set_value_by_name(msg, ".SDATA.foo.bar[0]", "value[0]", -1);
  assert_sdata_value_equals(msg, "[foo bar%5B0%5D=\"value[0\\]\"]");
  log_msg_unref(msg);

  msg = log_msg_new_empty();
  log_msg_set_value_by_name(msg, ".SDATA.foo.bácsi", "bácsi", -1);
  assert_sdata_value_equals(msg, "[foo b%C3%A1csi=\"bácsi\"]");
  log_msg_unref(msg);

  msg = log_msg_new_empty();
  log_msg_set_value_by_name(msg, ".SDATA.foo.sp ace", "sp ace", -1);
  assert_sdata_value_equals(msg, "[foo sp%20ace=\"sp ace\"]");
  log_msg_unref(msg);

  msg = log_msg_new_empty();
  log_msg_set_value_by_name(msg, ".SDATA.foo.eq=al", "eq=al", -1);
  assert_sdata_value_equals(msg, "[foo eq%3Dal=\"eq=al\"]");
  log_msg_unref(msg);

  msg = log_msg_new_empty();
  log_msg_set_value_by_name(msg, ".SDATA.foo.quo\"te", "quo\"te", -1);
  assert_sdata_value_equals(msg, "[foo quo%22te=\"quo\\\"te\"]");
  log_msg_unref(msg);
}

int
main(int argc G_GNUC_UNUSED, char *argv[] G_GNUC_UNUSED)
{
  app_startup();

  init_and_load_syslogformat_module();

  test_misc_stuff();
  test_sdata_keys_are_sanitized();

  app_shutdown();
  return 0;
}
