/**
 * Copyright (c) 2022 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SSL_TRANSPORT_INTERFACE_H_
#define _SSL_TRANSPORT_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/x509_crt.h"
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#ifdef SECURE_PRIVATE_KEY
#include <mbedtls/pk_internal.h>
/* PSA includes. */
#include "psa/crypto.h"

/* Amazon FreeRTOS includes. */
#include "core_pki_utils.h"

#endif


/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
#define DEBUG_LEVEL 3
#define DEBUG_BUF_MAX_SIZE 1024

#define SSL_RECV_TIMEOUT (1000 * 10)


/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
typedef struct
{
#if defined(MBEDTLS_ENTROPY_C)
    mbedtls_entropy_context entropy;
#endif /* MBEDTLS_ENTROPY_C */
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
#if defined(SECURE_PRIVATE_KEY)
    mbedtls_pk_info_t privateKeyInfo;
    psa_key_handle_t uxSignKey;
#endif
    uint8_t *root_ca;
    uint8_t rootca_option;
    uint8_t *client_cert;
    uint8_t clica_option;
    uint8_t *private_key;
} tls_context_t;

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */

#if defined(MBEDTLS_DEBUG_C)
static void debug_callback(void *ctx, int level, const char *file, int line, const char *str);
#endif /* MBEDTLS_DEBUG_C */
int ssl_recv_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t timeout);
int ssl_send(void *ctx, const unsigned char *buf, size_t len);
static int ssl_random_callback(void *p_rng, unsigned char *output, size_t output_len);
int ssl_transport_init(tls_context_t *tls_context, int *socket_fd, const char *host);
void ssl_transport_deinit(tls_context_t *tls_context);
int ssl_socket_connect_timeout(tls_context_t *tls_context, char *addr, unsigned int port, unsigned int local_port, uint32_t timeout);
int ssl_transport_read(tls_context_t *tls_context, unsigned char *readbuf, unsigned int len);
int ssl_transport_write(tls_context_t *tls_context, unsigned char *writebuf, unsigned int len);
int ssl_transport_disconnect(tls_context_t *tls_context, uint32_t timeout);
unsigned int ssl_transport_close_notify(tls_context_t *tls_context);
int ssl_transport_session_reset(tls_context_t *tls_context);
int ssl_transport_check_ca(uint8_t *ca_data, uint32_t ca_len);
int ssl_transport_check_pkey(uint8_t *pkey_data, uint32_t pkey_len);

#ifdef __cplusplus
}
#endif

#endif /* _SSL_TRANSPORT_INTERFACE_H_ */
