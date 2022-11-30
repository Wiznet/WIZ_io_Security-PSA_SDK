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

#include "stm32u5xx_hal.h"

#if (_WIZCHIP_ == 6100)
#include "io6Library/Ethernet/wizchip_conf.h"
#else
#include "ioLibrary_Driver/Ethernet/wizchip_conf.h"
#endif

#include "wiznet_spi.h"

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
SPI_HandleTypeDef hspi1;

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
static inline void wizchip_select(void)
{
    HAL_GPIO_WritePin(WIZCHIP_CS_GPIO_Port, WIZCHIP_CS_Pin, GPIO_PIN_RESET);
}

static inline void wizchip_deselect(void)
{
    HAL_GPIO_WritePin(WIZCHIP_CS_GPIO_Port, WIZCHIP_CS_Pin, GPIO_PIN_SET);
}

void wizchip_reset()
{
    HAL_GPIO_WritePin(WIZCHIP_RST_GPIO_Port, WIZCHIP_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(WIZCHIP_RST_GPIO_Port, WIZCHIP_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
}

static uint8_t wizchip_read(void)
{
    uint8_t rx_data = 0;
    uint8_t tx_data = 0xFF;

    HAL_SPI_TransmitReceive(&hspi1, &tx_data, &rx_data, 1, 0xFF);
    return rx_data;
}

static void wizchip_write(uint8_t tx_data)
{
    uint8_t rx_data = 0xFF;
    HAL_SPI_TransmitReceive(&hspi1, &tx_data, &rx_data, 1, 0xFF);
}

void wizchip_spi_initialize(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};    
    SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = {0};
    
    /* USER CODE BEGIN SPI1_Init 1 */
    
    /* USER CODE END SPI1_Init 1 */
    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 0x7;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
      printf(" WIZCHIP SPI initialized fail\r\n");
      while(1);
    }
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
    if (HAL_SPIEx_SetConfigAutonomousMode(&hspi1, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
    {
      printf(" WIZCHIP SPI initialized fail\r\n");
      while(1);
    }
    
    /* GPIO Ports Clock Enable */
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    HAL_PWREx_EnableVddIO2();

    /*Configure GPIO pin Output Level */
    
    /*Configure GPIO pins : WIZCHIP_RST_Pin WIZCHIP_CS_Pin */
    GPIO_InitStruct.Pin = WIZCHIP_RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(WIZCHIP_RST_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = WIZCHIP_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(WIZCHIP_CS_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : WIZCHIP_INT_Pin */
    GPIO_InitStruct.Pin = WIZCHIP_INT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(WIZCHIP_INT_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(WIZCHIP_RST_GPIO_Port, WIZCHIP_RST_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(WIZCHIP_CS_GPIO_Port, WIZCHIP_CS_Pin, GPIO_PIN_SET);
}

void wizchip_initialize(void)
{
    /* Deselect the FLASH : chip select high */
    wizchip_deselect();

    /* CS function register */
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);

    /* SPI function register */
#if (_WIZCHIP_ == W6100)
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write, 0, 0);
#else
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
#endif
#ifdef USE_SPI_DMA
    reg_wizchip_spiburst_cbfunc(wizchip_read_burst, wizchip_write_burst);
#endif

    /* WIZCHIP initialize */
    uint8_t temp;
#if (_WIZCHIP_ == W5100S)
    uint8_t memsize[2][4] = {{2, 2, 2, 2}, {2, 2, 2, 2}};
#elif (_WIZCHIP_ == W5500 || _WIZCHIP_ == W6100)
    uint8_t memsize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
#endif

    if (ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
    {
        printf(" WIZCHIP initialized fail\r\n");
        return;
    }

    /* Check PHY link status */
    do
    {
        if (ctlwizchip(CW_GET_PHYLINK, (void *)&temp) == -1)
        {
            printf(" Unknown PHY link status\r\n");
            return;
        }
    } while (temp == PHY_LINK_OFF);
}

void wizchip_check(void)
{
#if (_WIZCHIP_ == W5100S)
    /* Read version register */
    if (getVER() != 0x51)
    {
        printf(" ACCESS ERR : VERSION != 0x51, read value = 0x%02x\r\n", getVER());
        while (1);
    }
#elif (_WIZCHIP_ == W5500)
    /* Read version register */
    if (getVERSIONR() != 0x04)
    {
        printf(" ACCESS ERR : VERSION != 0x04, read value = 0x%02x\r\n", getVERSIONR());
        while (1);
    }
#elif (_WIZCHIP_ == W6100)
    if (getVER() != 0x4661)
    {
        printf(" ACCESS ERR : VERSION != 0x4661, read value = 0x%02x\r\n", getVER());
        while (1);
    }
#endif
}

void network_initialize(wiz_NetInfo net_info)
{
    ctlnetwork(CN_SET_NETINFO, (void *)&net_info);
}

void print_network_information(wiz_NetInfo net_info)
{

#if ((_WIZCHIP_ == W5500) || (_WIZCHIP_ == W5100S))
  uint8_t tmp_str[8] = {
        0,
    };

    if (net_info.dhcp == NETINFO_DHCP)
    {
        printf("======================================================================\r\n");
        printf(" %s network configuration : DHCP\r\n\r\n", (char *)tmp_str);
    }
    else
    {
        printf("======================================================================\r\n");
        printf(" %s network configuration : static\r\n\r\n", (char *)tmp_str);
    }
   
    ctlnetwork(CN_GET_NETINFO, (void *)&net_info);
    ctlwizchip(CW_GET_ID, (void *)tmp_str);
    
    printf(" MAC         : %02X:%02X:%02X:%02X:%02X:%02X\r\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    printf(" IP          : %d.%d.%d.%d\r\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    printf(" Subnet Mask : %d.%d.%d.%d\r\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
    printf(" Gateway     : %d.%d.%d.%d\r\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    printf(" DNS         : %d.%d.%d.%d\r\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
    printf("======================================================================\r\n\r\n");
#elif (_WIZCHIP_ == W6100)
	wizchip_getnetinfo(&net_info);
	printf("Mac address: %02x:%02x:%02x:%02x:%02x:%02x\n\r",net_info.mac[0],net_info.mac[1],net_info.mac[2],net_info.mac[3],net_info.mac[4],net_info.mac[5]);
	printf("IP address : %d.%d.%d.%d\n\r",net_info.ip[0],net_info.ip[1],net_info.ip[2],net_info.ip[3]);
	printf("SN Mask	   : %d.%d.%d.%d\n\r",net_info.sn[0],net_info.sn[1],net_info.sn[2],net_info.sn[3]);
	printf("Gate way   : %d.%d.%d.%d\n\r",net_info.gw[0],net_info.gw[1],net_info.gw[2],net_info.gw[3]);
	printf("DNS Server : %d.%d.%d.%d\n\r",net_info.dns[0],net_info.dns[1],net_info.dns[2],net_info.dns[3]);
	printf("LLA  : %.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X\r\n",net_info.lla[0],net_info.lla[1],net_info.lla[2],net_info.lla[3],\
									net_info.lla[4],net_info.lla[5],net_info.lla[6],net_info.lla[7],\
									net_info.lla[8],net_info.lla[9],net_info.lla[10],net_info.lla[11],\
									net_info.lla[12],net_info.lla[13],net_info.lla[14],net_info.lla[15]);
	printf("GUA  : %.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X\n\r",net_info.gua[0],net_info.gua[1],net_info.gua[2],net_info.gua[3],\
									net_info.gua[4],net_info.gua[5],net_info.gua[6],net_info.gua[7],\
									net_info.gua[8],net_info.gua[9],net_info.gua[10],net_info.gua[11],\
									net_info.gua[12],net_info.gua[13],net_info.gua[14],net_info.gua[15]);
	printf("SN6  : %.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X\n\r",net_info.sn6[0],net_info.sn6[1],net_info.sn6[2],net_info.sn6[3],\
									net_info.sn6[4],net_info.sn6[5],net_info.sn6[6],net_info.sn6[7],\
									net_info.sn6[8],net_info.sn6[9],net_info.sn6[10],net_info.sn6[11],\
									net_info.sn6[12],net_info.sn6[13],net_info.sn6[14],net_info.sn6[15]);
	printf("GW6  : %.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X\r\n",net_info.gw6[0],net_info.gw6[1],net_info.gw6[2],net_info.gw6[3],\
									net_info.gw6[4],net_info.gw6[5],net_info.gw6[6],net_info.gw6[7],\
									net_info.gw6[8],net_info.gw6[9],net_info.gw6[10],net_info.gw6[11],\
									net_info.gw6[12],net_info.gw6[13],net_info.gw6[14],net_info.gw6[15]);


#endif
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */
  
  /* USER CODE END SPI1_MspInit 0 */
  
  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    PeriphClkInit.Spi1ClockSelection = RCC_SPI1CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      printf("(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)\r\n");
      while(1);
    }
  
    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PE14     ------> SPI1_MISO
    PE13     ------> SPI1_SCK
    PE15     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  /* USER CODE BEGIN SPI1_MspInit 1 */
  
  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_SPI1_CLK_DISABLE();

        /**SPI1 GPIO Configuration
        PE13    ------> SPI1_SCK
        PE14    ------> SPI1_MISO
        PE15    ------> SPI1_MOSI
        */
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    }
}
