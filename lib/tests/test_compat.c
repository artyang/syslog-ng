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

#include "syslog-ng.h"
#include "testutils.h"
#include "compat.h"

#ifndef G_GNUC_UNUSED
#define G_GNUC_UNUSED
#endif

void
_test_inet_ntop(void)
{
  char *dst = g_strdup_printf("xxx.xxx.xxx.xxx...................");
  socklen_t len = strlen(dst) + 1;
  struct in_addr sa4;
  memset((void *)&sa4, 0xFF, sizeof(sa4));

  const char *res = compat_inet_ntop(-1, &sa4, dst, len);
  expect_true(res == NULL, "inet_ntop() should fail on invalid family");

  res = compat_inet_ntop(AF_INET, &sa4, dst, 1);
#ifndef _AIX
  expect_true(res == NULL, "inet_ntop() should fail on small buffer");
#endif

  res = compat_inet_ntop(AF_INET, &sa4, dst, len);
  expect_false(res == NULL, "inet_ntop() should succeed on IPv4");
  expect_nstring(dst, -1, "255.255.255.255", -1, "inet_ntop(): wrong IPv4 address");

#if ENABLE_IPV6
  const char sa6[] = { 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4 };
  res = compat_inet_ntop(AF_INET6, (const struct in6_addr *)&sa6, dst, len);
  expect_false(res == NULL, "inet_ntop() should succeed on IPv6");
  expect_nstring(dst, -1,
                 "101:101:202:202:303:303:404:404", -1,
                 "inet_ntop(): wrong IPv6 address");
#endif
  g_free(dst);
}

void
_test_inet_pton(void)
{
  struct in_addr sa4;

  int res = compat_inet_pton(-1, "255.255.255.255", &sa4);
  expect_true(res != 1, "inet_pton() should fail on invalid family");

  res = compat_inet_pton(AF_INET, "-", &sa4);
  expect_true(res != 1, "inet_pton() should fail on invalid string");

  res = compat_inet_pton(AF_INET, "255.255.255.255", &sa4);
  expect_true(res == 1, "inet_pton() should succeed on IPv4");
  assert_guint64_non_fatal(sa4.s_addr, 0xffffffff, "inet_pton(): wrong IPv4 address");

#if ENABLE_IPV6
  struct in6_addr sa6;
  res = compat_inet_pton(AF_INET6, "101:101:202:202:303:303:404:404", &sa6);
  expect_true(res == 1, "inet_pton() should succeed on IPv6");
  const char expected_sa6[] = { 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4 };
  gsize len = sizeof(expected_sa6) / sizeof(expected_sa6[0]);
  assert_guint8_array_non_fatal((const guint8 *)&sa6, len,
                                expected_sa6, len,
                                "inet_pton(): wrong IPv6 address");
#endif
}

int
main(int argc G_GNUC_UNUSED, char *argv[] G_GNUC_UNUSED)
{
  _test_inet_ntop();
  _test_inet_pton();
  return !testutils_deinit();
}
