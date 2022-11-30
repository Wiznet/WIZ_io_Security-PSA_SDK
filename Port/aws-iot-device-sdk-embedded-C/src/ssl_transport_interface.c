/**
 * Copyright (c) 2022 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#if (_WIZCHIP_ == 6100)
#include "io6Library/Ethernet/socket.h"
#else
#include "ioLibrary_Driver/Ethernet/socket.h"
#endif
#include "wiznet_time.h"

#include "ssl_transport_interface.h"
//#include "sample_certificates.h"

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
static uint8_t g_ssl_debug_buf[DEBUG_BUF_MAX_SIZE] = {
    0,
};

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
#ifdef SECURE_PRIVATE_KEY

#define SHA256_DIGEST_LENGTH         32

#define ECDSA_P256_SIGNATURE_LENGTH  64

#define RSA_2048_MODULUS_BITS        2048
#define RSA_2048_SIGNATURE_LENGTH    ( RSA_2048_MODULUS_BITS / 8 )

#define PSA_DEVICE_PRIVATE_KEY_ID 3U

/**
 * @brief Sign a cryptographic hash with the private key.
 *
 * @param[in] pvContext Crypto context.
 * @param[in] xMdAlg Unused.
 * @param[in] pucHash Byte array of hash to be signed.
 * @param[in] uiHashLen  Length in bytes of hash to be signed.
 * @param[out] pucSig RSA signature bytes.
 * @param[in] pxSigLen Length in bytes of signature buffer.
 * @param[in] piRng Unused.
 * @param[in] pvRng Unused.
 *
 * @return Zero on success.
 */
static int _PrivateKeySigningCallback( void * pvContext,
                                       mbedtls_md_type_t xMdAlg,
                                       const unsigned char * pucHash,
                                       size_t xHashLen,
                                       unsigned char * pucSig,
                                       size_t * pxSigLen,
                                       int ( * piRng )( void *, unsigned char *, size_t ),
                                       void * pvRng )
{
    /* Unreferenced parameters. */
    ( void ) ( piRng );
    ( void ) ( pvRng );
    ( void ) ( xMdAlg );
    
    tls_context_t *tls_context = ( tls_context_t * )pvContext;
    const unsigned char * pucDataToBeSigned = pucHash;
    size_t xDataLengthToBeSigned = xHashLen;
    size_t xExpectedInputLength;
    size_t xSignatureLength;
    psa_key_attributes_t sKeyAttributes;
    psa_algorithm_t xSignAlgorithm;
    psa_status_t uxStatus;
    int lResult = 0;

    if( ( pucHash == NULL ) || ( pucSig == NULL ) || ( pxSigLen == NULL ) )
    {
        lResult = -1;
    }

    if( lResult == 0 )
    {
        /* Get the key's algorithm by getting the key attributes. */
        uxStatus = psa_get_key_attributes( tls_context->uxSignKey, &sKeyAttributes );
        if( uxStatus == PSA_SUCCESS)
        {
            xSignAlgorithm = psa_get_key_algorithm (&sKeyAttributes);
        }
        else
        {
            lResult = -1;
        }
    }

    if( lResult == 0 )
    { 
        /* Update the signature length. */
        if( ( ( PSA_ALG_IS_RSA_PKCS1V15_SIGN( xSignAlgorithm ) ) ) ||
            ( ( PSA_ALG_IS_RSA_PSS( xSignAlgorithm ) ) ) )
        {
            xSignatureLength = RSA_2048_SIGNATURE_LENGTH;
            xExpectedInputLength = SHA256_DIGEST_LENGTH;
        }
        else if ( PSA_ALG_IS_ECDSA( xSignAlgorithm ) )
        {
            xSignatureLength = ECDSA_P256_SIGNATURE_LENGTH;
            xExpectedInputLength = SHA256_DIGEST_LENGTH;
        }
        else
        {
            lResult = -1;
        }
    }


    if( lResult == 0 )
    {
        /* Check that input data to be signed is the expected length. */
        if( xDataLengthToBeSigned == xExpectedInputLength )
        {
            *pxSigLen = xSignatureLength;

            /* Sign the data.*/
            uxStatus = psa_sign_hash( tls_context->uxSignKey,
                                      xSignAlgorithm,
                                      pucDataToBeSigned,
                                      xDataLengthToBeSigned,
                                      pucSig,
                                      *pxSigLen,
                                      pxSigLen );
            if( uxStatus == PSA_SUCCESS )
            {
                if( PSA_ALG_IS_ECDSA( xSignAlgorithm ) )
                {
                    /* PKCS #11 for P256 returns a 64-byte signature with 32 bytes for R and 32 bytes for S.
                     * This must be converted to an ASN.1 encoded array. */
                    if( *pxSigLen == ECDSA_P256_SIGNATURE_LENGTH )
                    {
                        PKI_pkcs11SignatureTombedTLSSignature( pucSig, pxSigLen );
                    }
                    else
                    {
                        lResult = -1;
                    }
                }
            }
            else
            {
                printf("Failure in signing callback.\r\n");
                lResult = -1;
            }
        }
        else
        {
            lResult = -1;
        }
    }

    return lResult;
}

#endif


/* Shell for mbedtls debug function.
 * DEBUG_LEBEL can be changed from 0 to 3 */
#if defined(MBEDTLS_DEBUG_C)
static void debug_callback(void *ctx, int level, const char *file, int line, const char *str)
{
    if (level <= DEBUG_LEVEL)
    {
        printf("%s\r\n", str);
    }
}
#endif /* MBEDTLS_DEBUG_C */

int ssl_recv_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
    uint32_t start_ms = millis();

    do
    {
        if (getSn_RX_RSR((uint8_t)ctx) > 0)
        {
            return recv((uint8_t)ctx, (uint8_t *)buf, (uint16_t)len);
        }
    } while ((millis() - start_ms) < timeout);

    return 0;
}

int ssl_send(void *ctx, const unsigned char *buf, size_t len)
{
  return send((uint8_t)ctx, (uint8_t *)buf, (uint16_t)len);

}


static int ssl_random_callback(void *p_rng, unsigned char *output, size_t output_len)
{
    int i;

    if (output_len <= 0)
    {
        return 1;
    }

    for (i = 0; i < output_len; i++)
    {
        *output++ = rand() % 0xff;
    }

    srand(rand());
    return 0;
}

/* SSL context initialization
 * */
int ssl_transport_init(tls_context_t *tls_context, int *socket_fd, const char *host)
{
    int ret = 1;
#if defined(MBEDTLS_ENTROPY_C)
    const unsigned char *pers = {"ssl_client1"};
#endif
#if defined(MBEDTLS_ERROR_C)
    char error_buf[100];
#endif /* MBEDTLS_ERROR_C */

#if defined(MBEDTLS_DEBUG_C)
    debug_set_threshold(DEBUG_LEVEL);
#endif /* MBEDTLS_DEBUG_C */

    /*
        Initialize session data
    */
    mbedtls_ssl_config_init(&tls_context->conf);
    mbedtls_x509_crt_init(&tls_context->cacert);
    mbedtls_x509_crt_init(&tls_context->clicert);
    mbedtls_pk_init(&tls_context->pkey);
    mbedtls_ssl_init(&tls_context->ssl);

#if defined(MBEDTLS_ENTROPY_C)
    mbedtls_entropy_init(&tls_context->entropy);
#endif /* MBEDTLS_ENTROPY_C */
    mbedtls_ctr_drbg_init(&tls_context->ctr_drbg);

    /*
        Initialize certificates
    */
#if defined(MBEDTLS_ENTROPY_C)
    if ((ret = mbedtls_ctr_drbg_seed(&tls_context->ctr_drbg, mbedtls_entropy_func, &tls_context->entropy,
                                     (const unsigned char *)pers, strlen(pers))) != 0)
    {
        printf(" failed\r\n  ! mbedtls_ctr_drbg_seed returned -0x%x\r\n", -ret);
        return -1;
    }
#endif /* MBEDTLS_ENTROPY_C */

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_ssl_conf_dbg(&tls_context->conf, debug_callback, stdout);
#endif /* MBEDTLS_DEBUG_C */

    printf("Root CA verify option %d\r\n", tls_context->rootca_option);
    
#if defined(SECURE_PRIVATE_KEY)
    psa_status_t psa_status;
    psa_status = psa_open_key( PSA_DEVICE_PRIVATE_KEY_ID, &( tls_context->uxSignKey ) );
    printf("PSA_DEVICE_PRIVATE_KEY_ID = %d\r\n", PSA_DEVICE_PRIVATE_KEY_ID);
    printf("psa_open_key = %d\r\n", psa_status);
#endif

    if ((ret = mbedtls_ssl_config_defaults(&tls_context->conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        printf(" failed mbedtls_ssl_config_defaults returned %d\r\n", ret);
        return -1;
    }

    mbedtls_ssl_conf_authmode(&tls_context->conf, tls_context->rootca_option);
    mbedtls_ssl_conf_rng(&tls_context->conf, mbedtls_ctr_drbg_random, &tls_context->ctr_drbg);

    /*
        Parse certificate
    */
    if (tls_context->rootca_option != MBEDTLS_SSL_VERIFY_NONE)
    {
        ret = mbedtls_x509_crt_parse(&tls_context->cacert, (const unsigned char *)tls_context->root_ca, strlen((const char *)tls_context->root_ca) + 1);

        if (ret < 0)
        {
            printf(" failed\r\n  !  mbedtls_x509_crt_parse returned -0x%x while parsing root cert\r\n", -ret);

            return -1;
        }
        printf("ok! mbedtls_x509_crt_parse returned -0x%x while parsing root cert\r\n", -ret);
    }

    if (tls_context->clica_option == true)
    {
        ret = mbedtls_x509_crt_parse((&tls_context->clicert), (const unsigned char *)tls_context->client_cert, strlen((const char *)tls_context->client_cert) + 1);
        if (ret != 0)
        {
            printf(" failed\r\n  !  mbedtls_x509_crt_parse returned -0x%x while parsing device cert\r\n", -ret);

            return -1;
        }
        printf("ok! mbedtls_x509_crt_parse returned -0x%x while parsing device cert\r\n", -ret);
        mbedtls_ssl_conf_ca_chain(&tls_context->conf, &tls_context->cacert, NULL);
    }

#ifdef SECURE_PRIVATE_KEY
    {
        mbedtls_pk_type_t pk_type = MBEDTLS_PK_NONE;
        psa_key_attributes_t sKeyAttributes;
        psa_algorithm_t xSignAlgorithm;
        psa_status_t uxStatus;

        uxStatus = psa_get_key_attributes( tls_context->uxSignKey, &sKeyAttributes );
        if( uxStatus == PSA_SUCCESS )
        {
            xSignAlgorithm = psa_get_key_algorithm (&sKeyAttributes);
            if( PSA_ALG_IS_RSA_PKCS1V15_SIGN( xSignAlgorithm ) ||
                PSA_ALG_IS_RSA_PSS( xSignAlgorithm ) )
            {
                pk_type = MBEDTLS_PK_RSA;
            }
            else if( PSA_ALG_IS_ECDSA( xSignAlgorithm ) )
            {
                pk_type = MBEDTLS_PK_ECKEY;
            }
        }
        //pk_type = MBEDTLS_PK_RSA;
        printf("pk_type = %d\r\n", pk_type);
        if( pk_type != MBEDTLS_PK_NONE )
        {
            memcpy( &( tls_context->privateKeyInfo ), mbedtls_pk_info_from_type( pk_type ), sizeof( mbedtls_pk_info_t ) );
            tls_context->privateKeyInfo.sign_func = _PrivateKeySigningCallback;
            tls_context->pkey.pk_info = &tls_context->privateKeyInfo;
            tls_context->pkey.pk_ctx = tls_context;
        }
        else
        {
            ret = -1;
            printf("pk_type == MBEDTLS_PK_NONE\r\n");
            while(1);
        }

    }
#else
    /* Parse the client certificate private key. */
    ret = mbedtls_pk_parse_key( &( tls_context->pkey ),
                                         ( const unsigned char * )tls_context->private_key,
                                         strlen((const char *)tls_context->private_key) + 1,
                                         NULL,
                                         0 );
#endif
  
    if (tls_context->clica_option == true)
    {
        if ((ret = mbedtls_ssl_conf_own_cert(&tls_context->conf, &tls_context->clicert, &tls_context->pkey)) != 0)
        {
            printf("failed! mbedtls_ssl_conf_own_cert returned %d\r\n", ret);
            return -1;
        }
        printf("ok! mbedtls_ssl_conf_own_cert returned %d\r\n", ret);
    }

    if ((ret = mbedtls_ssl_setup(&tls_context->ssl, &tls_context->conf)) != 0)
    {
        printf(" failed mbedtls_ssl_setup returned -0x%x\r\n", -ret);
        return -1;
    }
    mbedtls_ssl_set_bio(&tls_context->ssl, socket_fd, ssl_send, NULL, ssl_recv_timeout);
    mbedtls_ssl_conf_read_timeout(&tls_context->conf, SSL_RECV_TIMEOUT);

    if ((ret = mbedtls_ssl_set_hostname(&tls_context->ssl, host)) != 0)
    {
        printf(" failed mbedtls_ssl_set_hostname returned %d\r\n", ret);
        return -1;
    }
    printf("ok! mbedtls_ssl_set_hostname returned %d\r\n", ret);
    return 0;
}

/* Free the memory for SSL context */
void ssl_transport_deinit(tls_context_t *tls_context)
{
    /* Free SSL context memory */
    mbedtls_ctr_drbg_free(&tls_context->ctr_drbg);
#if defined(SECURE_PRIVATE_KEY)
    psa_close_key( tls_context->uxSignKey );
    memset( &( tls_context->privateKeyInfo ), 0, sizeof( mbedtls_pk_info_t ));
    tls_context->pkey.pk_info = NULL;
#endif
    mbedtls_ssl_free(&tls_context->ssl);
    mbedtls_pk_free(&tls_context->pkey);
    mbedtls_x509_crt_free(&tls_context->clicert);
    mbedtls_x509_crt_free(&tls_context->cacert);
    mbedtls_ssl_config_free(&tls_context->conf);
#if defined(MBEDTLS_ENTROPY_C)
    mbedtls_entropy_free(&tls_context->entropy);
#endif /* MBEDTLS_ENTROPY_C */
}

int ssl_socket_connect_timeout(tls_context_t *tls_context, char *addr, unsigned int port, unsigned int local_port, uint32_t timeout)
{
    int ret;
    uint32_t start_ms = millis();

    uint8_t sock = (uint8_t)(tls_context->ssl.p_bio);

    /* Socket open */
    ret = socket(sock, Sn_MR_TCP, local_port, 0x00);
    if (ret != sock)
        return ret;

    /* Connect to the target */
#if (_WIZCHIP_ == W6100)
    ret = connect(sock, (uint8_t *)addr, port, 4);
#else
    ret = connect(sock, (uint8_t *)addr, port);
#endif
    if (ret != SOCK_OK)
        return ret;

    printf(" Performing the SSL/TLS handshake...\r\n");

    while ((ret = mbedtls_ssl_handshake(&tls_context->ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
#if defined(MBEDTLS_ERROR_C)
            memset(g_ssl_debug_buf, 0, DEBUG_BUF_MAX_SIZE);
            mbedtls_strerror(ret, (char *)g_ssl_debug_buf, DEBUG_BUF_MAX_SIZE);
            printf(" failed\r\n  ! mbedtls_ssl_handshake returned %d: %s\n\r", ret, g_ssl_debug_buf);
#endif /* MBEDTLS_ERROR_C */
            printf(" failed\r\n  ! mbedtls_ssl_handshake returned -0x%x\n\r", -ret);
            return (-1);
        }

        if ((millis() - start_ms) > timeout) // timeout
        {
#if defined(MBEDTLS_ERROR_C)
            memset(g_ssl_debug_buf, 0, DEBUG_BUF_MAX_SIZE);
            mbedtls_strerror(ret, (char *)g_ssl_debug_buf, DEBUG_BUF_MAX_SIZE);
            printf(" timeout\r\n  ! mbedtls_ssl_handshake returned %d: %s\n\r", ret, g_ssl_debug_buf);
#endif /* MBEDTLS_ERROR_C */
            printf(" timeout\r\n  ! mbedtls_ssl_handshake returned -0x%x\n\r", ret);
            return (-1);
        }
    }

#if defined(MBEDTLS_DEBUG_C)
    printf(" ok\n\r    [ Ciphersuite is %s ]\n\r",
           mbedtls_ssl_get_ciphersuite(&tls_context->ssl));
#endif /* MBEDTLS_DEBUG_C */
    printf(" ok\n\r    [ Ciphersuite is %s ]\n\r",
           mbedtls_ssl_get_ciphersuite(&tls_context->ssl));
    return (0);
}

int ssl_transport_read(tls_context_t *tls_context, unsigned char *readbuf, unsigned int len)
{
    return mbedtls_ssl_read(&tls_context->ssl, readbuf, len);
}

int ssl_transport_write(tls_context_t *tls_context, unsigned char *writebuf, unsigned int len)
{
    return mbedtls_ssl_write(&tls_context->ssl, writebuf, len);
}

int ssl_transport_disconnect(tls_context_t *tls_context, uint32_t timeout)
{
    int ret = 0;
    uint8_t sock = (uint8_t)(tls_context->ssl.p_bio);
    uint32_t tickStart = millis();

    do
    {
        ret = disconnect(sock);
        if ((ret == SOCK_OK) || (ret == SOCKERR_TIMEOUT))
            break;
    } while ((millis() - tickStart) < timeout);

    if (ret == SOCK_OK)
        ret = sock; // socket number
    return ret;
}

/* SSL close notify */
unsigned int ssl_transport_close_notify(tls_context_t *tls_context)
{
    uint32_t rc;
    do
        rc = mbedtls_ssl_close_notify(&tls_context->ssl);
    while (rc == MBEDTLS_ERR_SSL_WANT_WRITE);
    return rc;
}

/* SSL session reset */
int ssl_transport_session_reset(tls_context_t *tls_context)
{
    return mbedtls_ssl_session_reset(&tls_context->ssl);
}

int ssl_transport_check_ca(uint8_t *ca_data, uint32_t ca_len)
{
    int ret;

    mbedtls_x509_crt ca_cert;
    mbedtls_x509_crt_init(&ca_cert);

    ret = mbedtls_x509_crt_parse(&ca_cert, ca_data, ca_len + 1);
    if (ret < 0)
    {
        printf(" failed\r\n  !  mbedtls_x509_crt_parse returned -0x%x while parsing root cert\r\n", -ret);
    }
    else
        printf("ok! mbedtls_x509_crt_parse returned -0x%x while parsing root cert\r\n", -ret);

    mbedtls_x509_crt_free(&ca_cert);
    return ret;
}

int ssl_transport_check_pkey(uint8_t *pkey_data, uint32_t pkey_len)
{
    int ret;

    mbedtls_pk_context pk_cert;
    mbedtls_pk_init(&pk_cert);

    ret = mbedtls_pk_parse_key(&pk_cert, pkey_data, pkey_len + 1, NULL, 0);
    if (ret != 0)
    {
        printf(" failed\r\n  !  mbedtls_pk_parse_key returned -0x%x while parsing private key\r\n", -ret);
    }
    else
    {
        printf(" ok !  mbedtls_pk_parse_key returned -0x%x while parsing private key\r\n", -ret);
    }

    mbedtls_pk_free(&pk_cert);
    return ret;
}
