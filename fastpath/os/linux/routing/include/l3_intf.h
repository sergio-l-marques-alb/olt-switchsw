/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename l3_intf.h
*
* @purpose Linux network adaptation for the router interfaces.
*
* @component Linux IPM
*
* @comments none
*
* @create 02/08/2002
*
* @author JWL
* @end
*
**********************************************************************/
/*********************************************************************
 *                
 *********************************************************************/
#ifndef L3_INTF_H
#define L3_INTF_H

#define L3INTF_USE_TAP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>

#include <l3_commdefs.h>
#include "l7_product.h"


typedef enum {
   L3INTF_INVALID = 0,
   L3INTF_VALID
} L3INTF_PORT_STATES;

typedef struct {

   L7_uint32 intIfNum;
   L7_uint32 ip_addr[L7_L3_NUM_IP_ADDRS];
   L7_uint32 ip_netmask[L7_L3_NUM_IP_ADDRS];
   L7_uint32 ip_gateway[L7_L3_NUM_IP_ADDRS];
   L7_uchar8 enetAddr[L7_ENET_MAC_ADDR_LEN];
   L3INTF_PORT_STATES port_state;
   L7_BOOL   port_mapped;
#ifdef L3INTF_USE_TAP
   int tap_fd;
#endif
} L3INTF_INFO;

extern L7_uint32 ipmIfCheck(L7_uint32 ifIndex);

#endif /* L3_INTF_H */
