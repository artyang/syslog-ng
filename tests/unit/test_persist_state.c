/*
 * Copyright (c) 2010-2014 Balabit
 * Copyright (c) 2010-2014 Balázs Scheidler
 * Copyright (c) 2014 Viktor Tusa
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

#include "persist-state.h"
#include "apphook.h"

#include <stdio.h>
#include <stdlib.h>

void
test_values(void)
{
  PersistState *state;
  gint i, j;
  gchar *data;

  unlink("test_values.persist");
  state = persist_state_new("test_values.persist");
  if (!persist_state_start(state))
    {
      fprintf(stderr, "Error starting persist_state object\n");
      exit(1);
    }
  for (i = 0; i < 1000; i++)
    {
      gchar buf[16];
      PersistEntryHandle handle;

      g_snprintf(buf, sizeof(buf), "testkey%d", i);
      if (!(handle = persist_state_alloc_entry(state, buf, 128)))
        {
          fprintf(stderr, "Error allocating value in the persist file: %s\n", buf);
          exit(1);
        }
      data = persist_state_map_entry(state, handle);
      for (j = 0; j < 128; j++)
        {
          data[j] = (i % 26) + 'A';
        }
      persist_state_unmap_entry(state, handle);
    }
  for (i = 0; i < 1000; i++)
    {
      gchar buf[16];
      PersistEntryHandle handle;
      gsize size;
      guint8 version;

      g_snprintf(buf, sizeof(buf), "testkey%d", i);
      if (!(handle = persist_state_lookup_entry(state, buf, &size, &version)))
        {
          fprintf(stderr, "Error retrieving value from the persist file: %s\n", buf);
          exit(1);
        }
      data = persist_state_map_entry(state, handle);
      for (j = 0; j < 128; j++)
        {
          if (data[j] != (i % 26) + 'A')
            {
              fprintf(stderr, "Invalid data in persistent entry\n");
              exit(1);
            }
        }
      persist_state_unmap_entry(state, handle);
    }
  persist_state_commit(state);
  persist_state_free(state);

  /* reopen */
  state = persist_state_new("test_values.persist");
  if (!persist_state_start(state))
    {
      fprintf(stderr, "Error starting persist_state object\n");
      exit(1);
    }
  for (i = 0; i < 1000; i++)
    {
      gchar buf[16];
      PersistEntryHandle handle;
      gsize size;
      guint8 version;

      g_snprintf(buf, sizeof(buf), "testkey%d", i);
      if (!(handle = persist_state_lookup_entry(state, buf, &size, &version)))
        {
          fprintf(stderr, "Error retrieving value from the persist file: %s\n", buf);
          exit(1);
        }
      if (size != 128 || version != 4)
        {
          fprintf(stderr, "Error retrieving value from the persist file: %s, invalid size (%d) or version (%d)\n", buf, (gint) size, version);
          exit(1);
        }
      data = persist_state_map_entry(state, handle);
      for (j = 0; j < 128; j++)
        {
          if (data[j] != (i % 26) + 'A')
            {
              fprintf(stderr, "Invalid data in persistent entry\n");
              exit(1);
            }
        }
      persist_state_unmap_entry(state, handle);
    }
  persist_state_free(state);
}

int
main(int argc, char *argv[])
{
#if __hpux__
  return 0;
#endif
  app_startup();
  test_values();
  return 0;
}
