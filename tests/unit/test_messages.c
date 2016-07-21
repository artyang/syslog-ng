/*
 * Copyright (c) 2016 Balabit
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

#include "testutils.h"
#include "messages.h"
#include "logmsg.h"

void
test_messages()
{
  LogMessage *msg = NULL;
  const gchar *message = NULL;
  msg = msg_event_create(EVT_PRI_ERR, "Test Message", evt_tag_str("name1", "value1"), NULL);
  message = log_msg_get_value(msg, LM_V_MESSAGE, NULL);
  assert_string(message, "Test Message; name1='value1'", "Bad message");
  log_msg_unref(msg);

  msg = msg_event_create(EVT_PRI_ERR, "Test Message", evt_tag_str("name1", "value1"), evt_tag_str("name2", "value2"), NULL);
  message = log_msg_get_value(msg, LM_V_MESSAGE, NULL);
  assert_string(message, "Test Message; name1='value1', name2='value2'", "Bad message");
  log_msg_unref(msg);

  msg = msg_event_create(EVT_PRI_ERR, "Test Message", evt_tag_str("name1", "value1"), evt_tag_str("name2", "value2"), evt_tag_id(125), NULL);
  message = log_msg_get_value(msg, LM_V_MESSAGE, NULL);
  assert_string(message, "Test Message; name1='value1', name2='value2'", "Bad message");
  assert_string(log_msg_get_value(msg, LM_V_MSGID, NULL), "125", "Bad message id");
  log_msg_unref(msg);

  msg = msg_event_create(EVT_PRI_ERR, "Test Message", evt_tag_str("name1", "value1"), evt_tag_id(125), evt_tag_str("name2", "value2"), NULL);
  message = log_msg_get_value(msg, LM_V_MESSAGE, NULL);
  assert_string(message, "Test Message; name1='value1', name2='value2'", "Bad message");
  assert_string(log_msg_get_value(msg, LM_V_MSGID, NULL), "125", "Bad message id");
  log_msg_unref(msg);

  msg = msg_event_create(EVT_PRI_ERR, "Test Message", evt_tag_id(125), evt_tag_str("name1", "value1"), evt_tag_str("name2", "value2"), NULL);
  message = log_msg_get_value(msg, LM_V_MESSAGE, NULL);
  assert_string(message, "Test Message; name1='value1', name2='value2'", "Bad message");
  assert_string(log_msg_get_value(msg, LM_V_MSGID, NULL), "125", "Bad message id");
  log_msg_unref(msg);

  msg = msg_event_create(EVT_PRI_ERR, "Test Message", evt_tag_id(125), NULL);
  message = log_msg_get_value(msg, LM_V_MESSAGE, NULL);
  assert_string(message, "Test Message;", "Bad message");
  assert_string(log_msg_get_value(msg, LM_V_MSGID, NULL), "125", "Bad message id");
  log_msg_unref(msg);

  return;
}

int
main(int argc G_GNUC_UNUSED, char *argv[] G_GNUC_UNUSED)
{
  log_msg_registry_init();
  test_messages();
  log_msg_registry_deinit();
  return 0;
}
