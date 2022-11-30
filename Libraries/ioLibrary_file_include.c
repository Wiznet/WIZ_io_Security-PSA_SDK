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

#if (_WIZCHIP_ == 6100)
#include "io6Library/Application/loopback/loopback.c"
#include "io6Library/Ethernet/socket.c"
#include "io6Library/Ethernet/wizchip_conf.c"
#include "io6Library/Ethernet/W6100/w6100.c"
#include "io6Library/Internet/DHCP4/dhcpv4.c"
#include "io6Library/Internet/DNS/dns.c"
#else
#include "ioLibrary_Driver/Application/loopback/loopback.c"
#include "ioLibrary_Driver/Ethernet/socket.c"
#include "ioLibrary_Driver/Ethernet/wizchip_conf.c"
#if (_WIZCHIP_ == 5105)
#include "ioLibrary_Driver/Ethernet/W5100S/w5100s.c"
#elif (_WIZCHIP_ == 5500)
#include "ioLibrary_Driver/Ethernet/W5500/w5500.c"
#endif
#include "ioLibrary_Driver/Internet/DHCP/dhcp.c"
#include "ioLibrary_Driver/Internet/DNS/dns.c"
#endif
