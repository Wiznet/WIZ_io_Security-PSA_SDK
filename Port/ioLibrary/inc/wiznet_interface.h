/**
 * Copyright (c) 2022 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _WIZCHIP_NETWORK_H_
#define _WIZCHIP_NETWORK_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include "stm32u5xx_hal.h"

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

#define DOWNLOAD_SOCKET 0
#define TCPS_PORT   8000

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
void wizchip_net_init(void);
HAL_StatusTypeDef wizchip_Transmit(uint8_t *Data, uint16_t uDataLength, uint32_t uTimeout);
HAL_StatusTypeDef wizchip_Receive(uint8_t *Data, uint16_t uDataLength, uint32_t uTimeout);
HAL_StatusTypeDef Eth_Receive(uint32_t *puSize, uint32_t uFlashDestination);

#ifdef __cplusplus
}
#endif

#endif /* _WIZCHIP_NETWORK_H_ */
