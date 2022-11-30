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
#include <string.h>

#include "wiznet_interface.h"
#include "wiznet_time.h"

#include "mqtt_transport_interface.h"
#include "ssl_transport_interface.h"

#include "samples.h"
#include "sample_certificates.h"

#if WIZCHIP_TEST_AWS_IOT
/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
#define SOCKET_AWS_IOT 0
#define PORT_AWS_IOT 8883

#define MQTT_PUB_PERIOD (1000 * 10) // 10 seconds
#define MQTT_DOMAIN "aqzlwsttrwzrm-ats.iot.ap-northeast-2.amazonaws.com"
#define MQTT_PUB_TOPIC "$aws/things/my_aws_iot_thing/shadow/update"
#define MQTT_SUB_TOPIC "$aws/things/my_aws_iot_thing/shadow/update/accepted"

#define MQTT_USERNAME NULL
#define MQTT_PASSWORD NULL
#define MQTT_CLIENT_ID "my_aws_iot_thing"

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
static uint8_t g_mqtt_buf[MQTT_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_mqtt_publish_messsage_buf[MQTT_BUF_MAX_SIZE] = {
    0,
};

static tls_context_t g_mqtt_tls_context;

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
void wizchip_test_aws_iot(void)
{
    int retval = 0;
    uint32_t start_ms = 0;
    uint32_t end_ms = 0;
    uint32_t publish_cnt = 0;

    /* Setup certificate */
    g_mqtt_tls_context.rootca_option = MBEDTLS_SSL_VERIFY_REQUIRED; // use Root CA verify
    g_mqtt_tls_context.clica_option = 1;                            // use client certificate
    g_mqtt_tls_context.root_ca = aws_iot_root_ca;
    g_mqtt_tls_context.client_cert = aws_iot_client_cert;
    g_mqtt_tls_context.private_key = aws_iot_private_key;

    retval = mqtt_transport_init(true, (uint8_t *)MQTT_CLIENT_ID, NULL, NULL, MQTT_DEFAULT_KEEP_ALIVE);

    if (retval != 0)
    {
        printf(" Failed, mqtt_transport_init returned %d\r\n", retval);
        while (1);
    }

    retval = mqtt_transport_connect(SOCKET_AWS_IOT, 1, g_mqtt_buf, MQTT_BUF_MAX_SIZE, (uint8_t *)MQTT_DOMAIN, PORT_AWS_IOT, &g_mqtt_tls_context);
    if (retval != 0)
    {
        printf(" Failed, mqtt_transport_connect returned %d\r\n", retval);
        while (1);
    }

    retval = mqtt_transport_subscribe(0, MQTT_SUB_TOPIC);

    if (retval != 0)
    {
        printf(" Failed, mqtt_transport_subscribe returned %d\r\n", retval);
        while (1);
    }

    start_ms = millis();

    /* Infinite loop */
    while (1)
    {
        /* AWS IoT send & receive test */
        retval = mqtt_transport_yield(MQTT_DEFAULT_YIELD_TIMEOUT);

        if (retval != 0)
        {
            printf(" Failed, mqtt_transport_yield returned %d\r\n", retval);
            while (1);
        }

        end_ms = millis();

        if (end_ms > start_ms + MQTT_PUB_PERIOD)
        {
            start_ms = millis();
            sprintf((char *)g_mqtt_publish_messsage_buf, "{\"message\":\"Hello, World!\", \"publish count\":\"%d\"}\r\n", publish_cnt++);
            mqtt_transport_publish((uint8_t *)MQTT_PUB_TOPIC, g_mqtt_publish_messsage_buf, strlen((const char *)g_mqtt_publish_messsage_buf), 0);
        }
    }
}

#endif

