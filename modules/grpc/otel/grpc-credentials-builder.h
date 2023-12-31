/*
 * Copyright (c) 2023 Attila Szakacs
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

#ifndef GRPC_CREDENTIALS_BUILDER_H
#define GRPC_CREDENTIALS_BUILDER_H

#include "compat/cpp-start.h"

#include "compat/glib.h"

typedef enum
{
  GSAM_INSECURE,
  GSAM_TLS,
  GSAM_ALTS,
} GrpcServerAuthMode;

typedef enum
{
  GSTPV_OPTIONAL_UNTRUSTED,
  GSTPV_OPTIONAL_TRUSTED,
  GSTPV_REQUIRED_UNTRUSTED,
  GSTPV_REQUIRED_TRUSTED,
} GrpcServerTlsPeerVerify;

typedef struct GrpcServerCredentialsBuilderW_ GrpcServerCredentialsBuilderW; // Wrapper struct

void grpc_server_credentials_builder_set_mode(GrpcServerCredentialsBuilderW *s, GrpcServerAuthMode mode);
gboolean grpc_server_credentials_builder_set_tls_ca_path(GrpcServerCredentialsBuilderW *s, const gchar *ca_path);
gboolean grpc_server_credentials_builder_set_tls_key_path(GrpcServerCredentialsBuilderW *s, const gchar *key_path);
gboolean grpc_server_credentials_builder_set_tls_cert_path(GrpcServerCredentialsBuilderW *s, const gchar *cert_path);
void grpc_server_credentials_builder_set_tls_peer_verify(GrpcServerCredentialsBuilderW *s,
                                                         GrpcServerTlsPeerVerify peer_verify);

#include "compat/cpp-end.h"

#endif
