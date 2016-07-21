/*
 * Copyright (c) 2014 Balabit
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

#ifndef TLSWINCRYPT_H
#define TLSWINCRYPT_H 1

#include "messages.h"
#include "syslog-ng.h"
#include <openssl/ssl.h>
#include <openssl/engine.h>
#include <openssl/conf.h>
#include <glib.h>

#ifdef _WIN32
gboolean load_certificate(SSL_CTX *ctx,const gchar *cert_subject);
gboolean load_all_trusted_ca_certificates(SSL_CTX *ctx);
gboolean load_all_crls(SSL_CTX *ctx);
#else
static inline gboolean init_windows_crypt(SSL_CTX *ctx, const gchar *cert_subject)
{
  msg_error("This function shouldn't be called under non-Windows platform",evt_tag_str("function_name",__FUNCTION__));
  abort();
}

gboolean load_all_trusted_ca_certificates(SSL_CTX *ctx)
{
  msg_error("This function shouldn't be called under non-Windows platform",evt_tag_str("function_name",__FUNCTION__));
  abort();
}

gboolean  load_all_crls(SSL_CTX *ctx)
{
  msg_error("This function shouldn't be called under non-Windows platform",evt_tag_str("function_name",__FUNCTION__));
  abort();
}
#endif
#endif
