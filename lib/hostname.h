/*
 * Copyright (c) 2013 Balabit
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
#include "gsockaddr.h"

#include <sys/types.h>
G_LOCK_EXTERN(resolv_lock);

void reset_cached_hostname(void);
void getlonghostname(gchar *buf, gsize buflen);

gchar *get_dnsname();
void apply_custom_domain(gchar *fqdn, int length, gchar *domain);

void set_custom_domain(const gchar *new_custom_domain);
const gchar *get_custom_domain();
void normalize_hostname(gchar *result, gsize *result_len, const gchar *hostname);
gchar *format_hostname(const gchar *hostname,gchar *domain,GlobalConfig *cfg);

const char *get_cached_longhostname();
const char *get_cached_shorthostname();
