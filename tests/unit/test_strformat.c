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

#include <stdio.h>

#include "testutils.h"
#include "str-format.h"

void
assert_str_rtrim(gchar * const string, const gchar *characters, const gchar *expected_string)
{
  gchar *rtrimmed_string = g_strdup(string);

  str_rtrim(rtrimmed_string, characters);
  assert_string(rtrimmed_string, expected_string, "Right trimmed string is not expected");
  g_free(rtrimmed_string);
}

gint
main(gint argc G_GNUC_UNUSED, gchar *argv[] G_GNUC_UNUSED)
{
  assert_str_rtrim("", "", "");
  assert_str_rtrim("apple", "", "apple");
  assert_str_rtrim("apple ", " ", "apple");
  assert_str_rtrim("apple  ", " ", "apple");
  assert_str_rtrim("apple  ", "z ", "apple");
  assert_str_rtrim("asdqweasd", "qweasd", "");

  return 0;
}
