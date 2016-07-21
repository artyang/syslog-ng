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

#include "csvparser/csvparser.h"
#include <stdio.h>
#include <stdlib.h>

#define TEST_ASSERT(cond)                                               \
  if (!(cond))                                                          \
      {                                                                 \
            fprintf(stderr, "Test assertion failed at %d\n", __LINE__); \
            exit(1);                                                    \
      }

static inline guint32
append_flag(const char *flag_name, guint32 flag)
{
  return flag|log_csv_parser_lookup_flag(flag_name);
}

static inline void
assert_csv_parser_flag_equals(LogColumnParser *column_parser, guint32 expected)
{
  guint32 flags = log_csv_parser_get_flags(column_parser);
  TEST_ASSERT(flags == expected);
}

static inline void
assert_csv_parser_escape_flag_equals(LogColumnParser *column_parser,
                                     guint32 expected)
{
  guint32 escape_flags = (log_csv_parser_get_flags(column_parser) &
                         LOG_CSV_PARSER_ESCAPE_MASK);

  TEST_ASSERT(escape_flags == expected);
}

static inline void
assert_csv_parser_non_escape_flag_equals(LogColumnParser *column_parser,
                                     guint32 expected)
{
  guint32 flags = (log_csv_parser_get_flags(column_parser) &
                   ~LOG_CSV_PARSER_ESCAPE_MASK);
  TEST_ASSERT(flags == expected);
}

static inline void
assert_csv_parser_flags_equals(LogColumnParser *column_parser,
                               guint32 expected_escape_flag,
                               guint32 expected_non_escape_flags)
{
  assert_csv_parser_escape_flag_equals(column_parser, expected_escape_flag);

  assert_csv_parser_non_escape_flag_equals(column_parser,
                                           expected_non_escape_flags);

  assert_csv_parser_flag_equals(column_parser,
                                expected_escape_flag |
                                expected_non_escape_flags);
}

void
test_csv_parser_escape_flags()
{
  LogColumnParser *column_parser = log_csv_parser_new();

  guint32 flags = 0;
  flags = append_flag("escape-backslash", flags);
  flags = append_flag("drop-invalid", flags);
  flags = append_flag("greedy", flags);
  log_csv_parser_set_flags(column_parser,
                           log_csv_parser_normalize_escape_flags(column_parser,
                                                                 flags));

  assert_csv_parser_flags_equals(column_parser,
                                 LOG_CSV_PARSER_ESCAPE_BACKSLASH,
                                 LOG_CSV_PARSER_DROP_INVALID |
                                 LOG_CSV_PARSER_GREEDY);

  flags = append_flag("escape-double-char", 0);
  log_csv_parser_set_flags(column_parser,
                           log_csv_parser_normalize_escape_flags(column_parser,
                                                                 flags));

  assert_csv_parser_flags_equals(column_parser,
                                 LOG_CSV_PARSER_ESCAPE_DOUBLE_CHAR,
                                 LOG_CSV_PARSER_DROP_INVALID |
                                 LOG_CSV_PARSER_GREEDY);

  flags = append_flag("strip-whitespace", 0);
  log_csv_parser_set_flags(column_parser,
                           log_csv_parser_normalize_escape_flags(column_parser,
                                                                 flags));

  assert_csv_parser_flags_equals(column_parser,
                                 LOG_CSV_PARSER_ESCAPE_DOUBLE_CHAR,
                                 LOG_CSV_PARSER_STRIP_WHITESPACE);
}

int main(int argc, char **argv)
{
  test_csv_parser_escape_flags();

  return 0;
}
