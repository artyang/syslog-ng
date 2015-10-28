/*
 * Copyright (c) 2002-2015 BalaBit IT Ltd, Budapest, Hungary
 * Copyright (c) 1998-2015 Viktor Juhász
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

#ifndef HDS_H
#define HDS_H

#include "syslog-ng.h"
#include "property_container.h"

typedef gpointer HDSHandle;
typedef void (*PROP_MATCH)(Property *prop, const gchar *fqdn, gpointer user_data);

void hds_init();
void hds_destroy();

HDSHandle hds_get_root();
const gchar *hds_get_root_name();

HDSHandle hds_register_handle(const gchar *name);
void hds_unregister_handle(HDSHandle handle);
HDSHandle hds_get_handle(const gchar *name);
const gchar *hds_get_value(const gchar *path);

gchar *hds_handle_get_fqdn(HDSHandle handle);
gchar *hds_handle_get_name(HDSHandle handle);

PropertyContainer *hds_acquire_property_container(HDSHandle handle,  PROPERTY_CONTAINER_CONSTRUCTOR constructor);
PropertyContainer *hds_get_property_container(HDSHandle handle);

gpointer hds_query_properties(const gchar *pattern, PROP_MATCH on_match, gpointer user_data);

void hds_lock();
void hds_unlock();

void hds_thread_init();
void hds_thread_deinit();

GString *hds_get_tls_buffer();


#endif
