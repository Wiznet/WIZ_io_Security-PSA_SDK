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
#include <time.h>
#include "stm32u5xx_hal.h"

#if (_WIZCHIP_ == 6100)
#include "io6Library/Ethernet/wizchip_conf.h"
#include "io6Library/Internet/dns/dns.h"
#include "dhcpv4.h"
#else
#include "ioLibrary_Driver/Ethernet/wizchip_conf.h"
#include "ioLibrary_Driver/Internet/dns/dns.h"
#include "dhcp.h"
#endif

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
TIM_HandleTypeDef htim2;

volatile unsigned int g_devtime_msec = 0;
volatile unsigned int g_msec_cnt = 0;

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
time_t millis(void)
{
    return g_devtime_msec;
}

void delay_ms(unsigned int ms)
{
    uint32_t end_time = millis() + ms;
    while(end_time > millis());
}

void sleep_ms(unsigned int ms)
{
    uint32_t end_time = millis() + ms;
    while(end_time > millis());
}

void wizchip_timer_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 160;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        printf(" WIZCHIP base initialized fail\r\n");
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        printf(" WIZCHIP configure clock source initialized fail\r\n");
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        printf(" WIZCHIP master configure synchronization initialized fail\r\n");
    }

    HAL_TIM_Base_Start_IT(&htim2);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        g_devtime_msec++;
        g_msec_cnt++;

        if (g_msec_cnt >= 1000)
        {
            g_msec_cnt = 0;
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_7);
#if (_WIZCHIP_ == W6100)
            DHCPv4_time_handler();
#else
            DHCP_time_handler();
#endif
            DNS_time_handler();
        }
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base)
{
    if (htim_base->Instance == TIM2)
    {
        /* Peripheral clock enable */
        __HAL_RCC_TIM2_CLK_ENABLE();

        /* TIM2 interrupt Init */
        HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim_base)
{
    if (htim_base->Instance == TIM2)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM2_CLK_DISABLE();

        /* TIM2 interrupt DeInit */
        HAL_NVIC_DisableIRQ(TIM2_IRQn);
    }
}

