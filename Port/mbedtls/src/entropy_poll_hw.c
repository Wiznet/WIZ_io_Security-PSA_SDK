/*
 * Entropy hardware poll function
 *
 *  Copyright (C) 2006-2021, Arm Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
 #include "mbedtls/config.h"
#else
 #include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

#include <string.h>
#include "entropy.h"
#include "entropy_poll.h"
#include "stm32u5xx_hal.h"

/**
 * Entropy poll callback for a hardware source
 */
__WEAK int mbedtls_hardware_poll (void *data, unsigned char *output,
                                  size_t len, size_t *olen) {
  (void)data;
  uint32_t timer;

  if (len < sizeof(uint32_t)) {
    *olen = 0;
    return (0);
  }
  timer = HAL_GetTick();

  memcpy (output, &timer, sizeof(timer));
  *olen = sizeof (timer);
  return (0);
}

#endif /* MBEDTLS_ENTROPY_HARDWARE_ALT */
