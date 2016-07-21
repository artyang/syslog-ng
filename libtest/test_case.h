/*
 * Copyright (c) 2013 Balabit
 * Copyright (c) 2013 Viktor Juhasz <viktor.juhasz@balabit.com>
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

#ifndef TEST_CASE_H_
#define TEST_CASE_H_

typedef struct _TestCase TestCase;

struct _TestCase {
  void (*setup)(TestCase *self);
  void (*run)(TestCase *self);
  void (*teardown)(TestCase *self);
};


static inline void
run_test_case(TestCase *tc)
{
  if (tc->setup) tc->setup(tc);
  tc->run(tc);
  if (tc->teardown) tc->teardown(tc);
}

#define RUN_TESTCASE(class_name, test_case_prefix, test_function) { \
  class_name *tc = g_new0(class_name, 1); \
  tc->super.setup = test_case_prefix ## _setup; \
  tc->super.teardown = test_case_prefix ## _teardown; \
  tc->super.run = test_function;\
  run_test_case(&tc->super); \
  g_free(tc); \
}

#endif /* TEST_CASE_H_ */
