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
#include "logmsg.h"
#include "apphook.h"
#include "logpipe.h"

struct _AckRecord
{
  LogMessage *original;
  LogPathOptions path_options;
  gboolean acked;
  void (*init)(AckRecord *);
  void (*deinit)(AckRecord *);
  void (*ack_message)(LogMessage *lm, AckType ack_type);
};

static void
_init(AckRecord *self)
{
  self->acked = FALSE;
  log_msg_ref(self->original);

  log_msg_refcache_start_producer(self->original);
  log_msg_add_ack(self->original, &self->path_options);
  log_msg_ref(self->original);
}

static void
_deinit(AckRecord *self)
{
  log_msg_drop(self->original, &self->path_options, AT_PROCESSED);
  log_msg_refcache_stop();
}

static void
_ack_message(LogMessage *msg, AckType type)
{
  AckRecord *self = msg->ack_record;
  self->acked = TRUE;
}

static void
ack_record_free(AckRecord *self)
{
  log_msg_unref(self->original);
  g_free(self);
}

static AckRecord *
ack_record_new()
{
  AckRecord *self = g_new0(AckRecord, 1);
  self->init = _init;
  self->deinit = _deinit;
  self->ack_message = _ack_message;
  self->original = log_msg_new_empty();
  self->original->ack_func = self->ack_message;
  self->original->ack_record = self;
  self->path_options.ack_needed = TRUE;
  return self;
}

static LogMessage *
create_clone(LogMessage *msg, LogPathOptions *path_options)
{
  LogMessage *cloned = log_msg_ref(msg);
  cloned = log_msg_clone_cow(msg, path_options);
  log_msg_add_ack(msg, path_options);
  return cloned;
}

void
test_normal_ack()
{
  AckRecord *t = ack_record_new();
  t->init(t);
  t->deinit(t);
  assert_true(t->acked, NULL);
  ack_record_free(t);
}

void
test_clone_ack()
{
  AckRecord *t = ack_record_new();
  t->init(t);

  LogMessage *clone = create_clone(t->original, &t->path_options);

  log_msg_drop(clone, &t->path_options, AT_PROCESSED);
  assert_false(t->acked, NULL);
  
  t->deinit(t);
  assert_true(t->acked, NULL);
  ack_record_free(t);
}

struct nv_pair
{
  const gchar *name;
  const gchar *value;
};

static void
test_cloned_clone_ack(struct nv_pair param)
{
  AckRecord *t = ack_record_new();
  t->init(t);

  LogMessage *cloned = create_clone(t->original, &t->path_options);

  log_msg_set_value_by_name(cloned, param.name, param.value, -1);

  LogMessage *cloned_clone1 = create_clone(cloned,  &t->path_options);
  LogMessage *cloned_clone2 = create_clone(cloned,  &t->path_options);

  log_msg_drop(cloned_clone1, &t->path_options, AT_PROCESSED);
  assert_false(t->acked, NULL);

  log_msg_drop(cloned_clone2, &t->path_options, AT_PROCESSED);
  assert_false(t->acked, NULL);

  log_msg_drop(cloned, &t->path_options, AT_PROCESSED);
  assert_false(t->acked, NULL);

  t->deinit(t);
  assert_true(t->acked, NULL);
  ack_record_free(t);
}

int
main(int argc, gchar **argv)
{
  app_startup();
  test_normal_ack();
  test_clone_ack();
  test_cloned_clone_ack((struct nv_pair){.name = "test", .value="value"});
  test_cloned_clone_ack((struct nv_pair){.name = "", .value=""});
  app_shutdown();
  return 0;
}
