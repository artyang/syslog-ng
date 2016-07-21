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

#include "misc.c"
#include "testutils.h"

static void
test_replace_string(const gchar *string, const gchar *pattern, const gchar *replacement, const gchar *expected)
{
  gchar *result = replace_string(string, pattern, replacement);
  assert_string(result, expected, "Failed replacement string: %s; pattern: %s; replacement: %s", string, pattern, replacement);
  g_free(result);
}

int main(int argc, char **argv)
{
  test_replace_string("hello", "xxx", "zzz", "hello");
  test_replace_string("hello", "llo" ,"xx", "hexx");
  test_replace_string("hello",  "hel", "xx", "xxlo");
  test_replace_string("c:\\var\\c.txt", "\\", "\\\\", "c:\\\\var\\\\c.txt");
  test_replace_string("c:\\xxx\\xxx", "xxx", "var", "c:\\var\\var");
  return 0;
}
