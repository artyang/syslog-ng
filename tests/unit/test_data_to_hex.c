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
#include "misc.h"
#include <stdio.h>

void
test_data_to_hex()
{
  guint8 input[] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,0x10,0x20,0xff,0x00};
  gchar *expected_output = "01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 20 FF 00";
  gchar *output = data_to_hex_string(input, sizeof(input));
  assert_string(output, expected_output, "Bad working!\n");
  GString *lowercased_string = g_string_ascii_down(g_string_new(expected_output));

  guint32 size = 0;
  GError *error = NULL;
  guint8 *data = hex_string_to_data(output, &size, &error);
  assert_null(error, "There is no error for a good input.");
  assert_guint32(size, sizeof(input), "Decoded data must be the same size.");
  assert_guint8_array_non_fatal(data, size, input, sizeof(input),
          "The decoded data must be identical to the original one") ? 1 : exit(1);
  g_free(data);

  data = hex_string_to_data(lowercased_string->str, &size, &error);
  assert_guint8_array_non_fatal(data, size, input, sizeof(input),
          "The decoded data must be identical to the original one, with lowercase letters") ? 1 : exit(1);
  g_free(data);
  g_free(output);
  g_string_free(lowercased_string, TRUE);
}

void
test_hex_to_data_invalid_data()
{
    guint32 size;
    GError *error;
    guint8 *data;

    error = NULL;
    size = 8;
    data = hex_string_to_data("01 23 4", &size, &error);
    assert_not_null(error, "Error must be filled for invalid length");
    assert_guint32(size, 0, "The size must be 0 if it is not working, invalid length");
    assert_null(data, "The return value is NULL for wrong-formatted string, invalid length.");
    g_error_free(error);

    error = NULL;
    size = 8;
    data = hex_string_to_data("01X23", &size, &error);
    assert_not_null(error, "Error must be filled for non-space separator");
    assert_guint32(size, 0, "The size must be 0 if it is not working, non-space");
    assert_null(data, "The return value is NULL for wrong-formatted string, non-space.");
    g_error_free(error);

    error = NULL;
    size = 8;
    data = hex_string_to_data("01 X3", &size, &error);
    assert_not_null(error, "Error must be filled for non-hex value separator");
    assert_guint32(size, 0, "The size must be 0 if it is not working, non-hex");
    assert_null(data, "The return value is NULL for wrong-formatted string, non-hex.");
    g_error_free(error);

    size = 8;
    data = hex_string_to_data("01 X3", &size, NULL); /* It works without error parameter */
    assert_guint32(size, 0, "The size must be 0 if it is not working, without error parameter");
    assert_null(data, "The return value is NULL for wrong-formatted string, without error parameter.");
}

int
main(int argc G_GNUC_UNUSED, char *argv[] G_GNUC_UNUSED)
{
  test_data_to_hex();
  test_hex_to_data_invalid_data();
  return 0;
}
