/**
 * Copyright (c) 2022 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _DNS_CONFIG_H_
#define _DNS_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
#define SOCKET_DNS 3

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
uint8_t dns_server_ip[4] = {8, 8, 8, 8};

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */

#ifdef __cplusplus
}
#endif

#endif /* _DNS_CONFIG_H_ */
