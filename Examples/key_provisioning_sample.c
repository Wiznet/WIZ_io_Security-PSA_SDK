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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "key_provisioning.h"
#include "samples.h"


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
extern uint8_t aws_iot_private_key[];

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
void wizchip_test_key_provisioning(void)
{
  int status;
  printf("Key Provisioning \r\n");
  status = xProvisionPrivateKey((const uint8_t *)aws_iot_private_key,
                                 strlen(aws_iot_private_key) + 1U);
  if (status == 0) {
    printf("Key Provisioning Done. %d\r\n", status);
  } else {
    printf("Key Provisioning Failed! %d\r\n", status);
  }
}
