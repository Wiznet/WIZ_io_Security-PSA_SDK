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
#include "wiznet_spi.h"
#include "wiznet_interface.h"
   
#if (_WIZCHIP_ == 6100)
#include "io6Library/Ethernet/wizchip_conf.h"
#include "io6Library/Ethernet/socket.h"
#include "dhcpv4.h"
#else
#include "io6Library/Ethernet/wizchip_conf.h"
#include "ioLibrary_Driver/Ethernet/socket.h"
#include "dhcp.h"
#endif

#include "common.h"
#include "ymodem.h"

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
#define SOCKET_DHCP 1

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */

#if (_WIZCHIP_ == W6100)
wiz_NetInfo g_net_info = { .mac = {0x00, 0x08, 0xdc, 0x12, 0x34, 0x56},
                           .ip = {192,168,2,100},
                           .sn = {255, 255, 255, 0},
                           .gw = {192, 168, 2, 1},
                           .dns = {8, 8, 8, 8},
                           .lla = {0xfe,0x80, 0x00,0x00,
                                   0x00,0x00, 0x00,0x00,
                                   0x02,0x08, 0xdc,0xff,
                                   0xfe,0xff, 0xff,0xff},
                           .gua={0x20,0x01,0x02,0xb8,
                                 0x00,0x10,0x00,0x01,
                                 0x02,0x08,0xdc,0xff,
                                 0xfe,0xff,0xff,0xff},
                           .sn6={0xff,0xff,0xff,0xff,
                                 0xff,0xff,0xff,0xff,
                                 0x00,0x00,0x00, 0x00,
                                 0x00,0x00,0x00,0x00},
                           .gw6={0xfe, 0x80, 0x00,0x00,
                                 0x00,0x00,0x00,0x00,
                                 0x02,0x00, 0x87,0xff,
                                 0xfe,0x08, 0x4c,0x81},
                           .ipmode=NETINFO_DHCP_V4
                            };
#else
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 2, 100},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 2, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_DHCP                         // DHCP
};
#endif

static uint8_t g_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
static void wizchip_dhcp_conflict(void)
{
    printf(" Conflict IP from DHCP\r\n");

    // halt or reset or any...
    while (1)
        ; // this example is halt.
}

static void wizchip_dhcp_assign(void)
{

#if (_WIZCHIP_ == W6100)
    getIPfromDHCPv4(g_net_info.ip);
    getGWfromDHCPv4(g_net_info.gw);
    getSNfromDHCPv4(g_net_info.sn);
    getDNSfromDHCPv4(g_net_info.dns);
#else
    getIPfromDHCP(g_net_info.ip);
    getGWfromDHCP(g_net_info.gw);
    getSNfromDHCP(g_net_info.sn);
    getDNSfromDHCP(g_net_info.dns);

    g_net_info.dhcp = NETINFO_DHCP;
#endif
    /* Network initialize */
    network_initialize(g_net_info); // apply from DHCP

    /* Print network information */
    print_network_information(g_net_info);
#if (_WIZCHIP_ == W6100)
    printf(" DHCP leased time : %u seconds\r\n", getDHCPv4Leasetime());
#else    
    printf(" DHCP leased time : %u seconds\r\n", getDHCPLeasetime());
#endif
}

static void wizchip_dhcp_init(void)
{
    printf(" DHCP client running...\r\n\r\n");

#if (_WIZCHIP_ == W6100)
    DHCPv4_init(SOCKET_DHCP, g_ethernet_buf);
    reg_dhcpv4_cbfunc(wizchip_dhcp_assign, wizchip_dhcp_assign, wizchip_dhcp_conflict);
#else
    DHCP_init(SOCKET_DHCP, g_ethernet_buf);
    reg_dhcp_cbfunc(wizchip_dhcp_assign, wizchip_dhcp_assign, wizchip_dhcp_conflict);
#endif
    
}

void wizchip_net_init(void)
{
    /* Variable initialize */
    int retval = 0;

    /* SPI initialize */
    wizchip_spi_initialize();

    /* WIZCHIP initialize */
    wizchip_reset();
    wizchip_initialize();
    wizchip_check();

    /* Network initialize */
#if (_WIZCHIP_ == W6100)
    uint8_t syslock = SYS_NET_LOCK;
    ctlwizchip(CW_SYS_UNLOCK, &syslock);
    ctlnetwork(CN_SET_NETINFO, &g_net_info);

    if (g_net_info.ipmode == NETINFO_DHCP_V4) // DHCP
#else
    if (g_net_info.dhcp == NETINFO_DHCP) // DHCP
#endif
    {
        wizchip_dhcp_init();

        while (1)
        {

#if (_WIZCHIP_ == W6100)
            retval = DHCPv4_run();
            if (retval == DHCP_IPV4_LEASED)
#else
            retval = DHCP_run();
            if (retval == DHCP_IP_LEASED)
#endif
            {
                break;
            }

            HAL_Delay(1000);
        }
    }
    else // static
    {
        /* Network initialize */
        network_initialize(g_net_info);
        /* Print network information */
        print_network_information(g_net_info);
    }
}

/**
  * @brief Transmit Data.
  * @param uDataLength: Data pointer to the Data to transmit.
  * @param uTimeout: Timeout duration.
  * @retval Status of the Transmit operation.
  */
HAL_StatusTypeDef wizchip_Transmit(uint8_t *Data, uint16_t uDataLength, uint32_t uTimeout)
{
  int ret;
  ret = send(DOWNLOAD_SOCKET, Data, uDataLength);

  switch (ret) {
    case SOCK_BUSY:
      return HAL_BUSY;
      
    case SOCKERR_TIMEOUT:
      return HAL_TIMEOUT;

    default :
    {
      if (ret == uDataLength)
        return HAL_OK;
      else
        return HAL_ERROR;
    }
  }
}

/**
  * @brief Receive Data.
  * @param uDataLength: Data pointer to the Data to receive.
  * @param uTimeout: Timeout duration.
  * @retval Status of the Receive operation.
  */

HAL_StatusTypeDef wizchip_Receive(uint8_t *Data, uint16_t uDataLength, uint32_t uTimeout)
{
  int size = 0;
  int total_size = 0;
  uint32_t tickStart = HAL_GetTick();

  do{
    if (getSn_RX_RSR(DOWNLOAD_SOCKET) > 0){
      size = recv(DOWNLOAD_SOCKET, Data, uDataLength);
      if (size <= 0)
        return HAL_ERROR;
      else{
        if (uDataLength != size)
          printf("wizchip_Receive dataLen = %d, size = %d\r\n", uDataLength, size);
        return HAL_OK;
      }
    }
  } while ((HAL_GetTick() - tickStart) <= uTimeout);  
  return HAL_TIMEOUT;
}

HAL_StatusTypeDef wizchip_Receive_Packet(uint8_t *Data, uint16_t uDataLength, uint32_t *packet_size, uint32_t uTimeout)
{
  int size = 0;
  int total_size = 0;
  uint32_t tickStart = HAL_GetTick();

  do{
    if (getSn_RX_RSR(DOWNLOAD_SOCKET) > 0){
      size = recv(DOWNLOAD_SOCKET, Data, uDataLength);
      if (size <= 0)
        return HAL_ERROR;
      else{
        *packet_size = size;
        return HAL_OK;
      }
    }
  } while ((HAL_GetTick() - tickStart) <= uTimeout);  
  return HAL_TIMEOUT;
}

/**
  * @brief  Receive a packet from sender
  * @param  pData
  * @param  puLength
  *     0: end of transmission
  *     2: abort by sender
  *    >0: packet length
  * @param  uTimeout
  * @retval HAL_OK: normally return
  *         HAL_BUSY: abort by user
  */
static HAL_StatusTypeDef ReceivePacket_ETH(uint8_t *pData, uint32_t *puLength, uint32_t uTimeout)
{
  uint32_t packet_size = 0U;
  HAL_StatusTypeDef status;

  status = wizchip_Receive_Packet(pData, PACKET_1K_SIZE, &packet_size, uTimeout);
  /* Simple packet sanity check */
  if (status != HAL_OK)
    packet_size = 0U;

  *puLength = packet_size;
  return status;
}


/**
  * @brief  Receive a file using the tcp server.
  * @param  puSize The uSize of the file.
  * @param  uFlashDestination where the file has to be downloaded.
  * @retval COM_StatusTypeDef result of reception/programming
  */
HAL_StatusTypeDef Eth_Receive(uint32_t *puSize, uint32_t uFlashDestination)
{
  uint32_t packet_length, file_done = 0;
  uint32_t filesize = 0;
  uint32_t time_start;
  uint32_t buf_len = 0, remain_len = 0;
  HAL_StatusTypeDef e_result = HAL_OK;
  uint8_t data_buf[PACKET_1K_SIZE];
  uint8_t aPacketData[PACKET_1K_SIZE];

  memset(data_buf, 0x00, PACKET_1K_SIZE);
  memset(aPacketData, 0x00, PACKET_1K_SIZE);
  
  while ((file_done == 0U) && (e_result == HAL_OK))
  {
    //switch (ReceivePacket(m_aPacketData, &packet_length, DOWNLOAD_TIMEOUT))
    switch (ReceivePacket_ETH(aPacketData, &packet_length, DOWNLOAD_TIMEOUT))
    {
      case HAL_OK:
        /* Data packet received callback call*/
        if(packet_length > 0)
        {
          time_start = HAL_GetTick();
          if ((buf_len + packet_length) < PACKET_1K_SIZE)
          {
            memcpy(data_buf + buf_len, aPacketData, packet_length);
            buf_len += packet_length;
          }
          else
          {
            remain_len = (buf_len + packet_length) - PACKET_1K_SIZE;
            memcpy(data_buf + buf_len, aPacketData, packet_length - remain_len);
            if(Eth_DataPktRxCpltCallback(data_buf, uFlashDestination, PACKET_1K_SIZE) == HAL_OK)
              uFlashDestination += (PACKET_1K_SIZE);
            else /* An error occurred while writing to Flash memory */
            {
              printf("Eth_DataPktRxCpltCallback Error\r\n");
              e_result = HAL_ERROR;
              /* End session */
            }
            memset(data_buf, 0xFF, PACKET_1K_SIZE);
            memcpy(data_buf, aPacketData + (packet_length - remain_len), remain_len);
            buf_len = remain_len;
          }
          filesize += packet_length;
          packet_length = 0;
        }
        break;
        
      case HAL_BUSY: /* Abort actually */
        e_result = HAL_BUSY;
        break;
    }
    if ((filesize != 0) && (HAL_GetTick() >= (time_start + DOWNLOAD_TIMEOUT)))
      file_done = 1;
  }

  if(file_done)
  {
    if (buf_len > 0)
    {
      if(Eth_DataPktRxCpltCallback(data_buf, uFlashDestination, PACKET_1K_SIZE) != HAL_OK)
        e_result = HAL_ERROR;
    }   
  }
  *puSize = filesize;
  printf("filesize = %d\r\n", filesize);
#if defined(__ARMCC_VERSION)
  printf("e_result = %x\n", e_result);
#else
  printf("e_result = %x\n", e_result);
#endif /* __ARMCC_VERSION */
  return e_result;
}


