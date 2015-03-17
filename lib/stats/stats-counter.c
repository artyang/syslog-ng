/*
 * Copyright (c) 2015 BalaBit
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

#include "stats.h"

static void
_reset_counters(gpointer key, gpointer value, gpointer user_data)
{
  StatsCounter     *sc = (StatsCounter*)value;
  StatsCounterType type;

  for (type = 0; type < SC_TYPE_MAX; ++type)
    {
      stats_counter_set(&sc->counters[type], 0);
    }
}

void
stats_reset_counters(GHashTable *counter_hash)
{
  stats_lock();
  g_hash_table_foreach(counter_hash, _reset_counters, NULL);
  stats_unlock();
}

