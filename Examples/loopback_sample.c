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
#include "wiznet_interface.h"

#include "samples.h"
#if (_WIZCHIP_ == 6100)
#include "io6Library/Application/loopback/loopback.h"
#include "io6Library/Ethernet/wizchip_conf.h"
#else
#include "ioLibrary_Driver/Application/loopback/loopback.h"
#include "ioLibrary_Driver/Ethernet/wizchip_conf.h"
#endif

#if WIZCHIP_TEST_LOOPBACK

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
#define SOCKET_LOOPBACK 0
#define PORT_LOOPBACK 5000

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
static uint8_t g_loopback_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
void wizchip_test_loopback(void)
{
    int retval = 0;

    /* Infinite loop */
    while (1)
    {
        /* TCP server loopback test */
#if (_WIZCHIP_ == W6100)
        if ((retval = loopback_tcps(SOCKET_LOOPBACK, g_loopback_buf, PORT_LOOPBACK, AS_IPV4)) < 0)
#else
        if ((retval = loopback_tcps(SOCKET_LOOPBACK, g_loopback_buf, PORT_LOOPBACK)) < 0)
#endif
        {
            printf(" Loopback error : %d\r\n", retval);
            while (1);
        }
    }
}

#endif

