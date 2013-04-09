/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename snooping_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 03/26/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef __DHCPS_EXPORTS_H_
#define __DHCPS_EXPORTS_H_
/*------------------------------------*/
/* DHCP Server Parameter Limits  */
/*------------------------------------*/

#define L7_DHCPS_POOL_NAME_MINLEN          1
#define L7_DHCPS_POOL_NAME_MAXLEN          31
#define L7_DHCPS_DEFAULT_ROUTER_MAX        8
#define L7_DHCPS_DNS_SERVER_MAX            8
#define L7_DHCPS_POOL_OPTION_BUFFER        1024
#define L7_DHCPS_MAX_EXCLUDE_NUMBER        128
#define L7_DHCPS_CLIENT_ID_MAXLEN          255
#define L7_DHCPS_HOST_NAME_MAXLEN          255
#define L7_DHCPS_HARDWARE_ADDR_MAXLEN      16
#define L7_DHCPS_CLIENT_NAME_MAXLEN        31
#define L7_DHCPS_NETBIOS_NAME_SERVER_MAX    8
#define L7_DHCPS_DOMAIN_NAME_MAXLEN         255
#define L7_DHCPS_BOOT_FILE_NAME_MAXLEN      128
#define L7_DHCPS_POOL_OPTION_LENGTH         441

#define L7_DHCPS_OPTION_CODE_MIN            0
#define L7_DHCPS_OPTION_CODE_MAX            255

#define L7_DHCPS_OPTION_CONFIGURE_MIN       L7_DHCPS_OPTION_CODE_MIN + 1
#define L7_DHCPS_OPTION_CONFIGURE_MAX       L7_DHCPS_OPTION_CODE_MAX - 1


#define L7_DHCPS_LEASETIME_MIN              1     /* 1 minute */
#define L7_DHCPS_LEASETIME_MAX              86400 /* 60 days */

#define L7_DHCPS_PINGPKTNO_MIN              0
#define L7_DHCPS_PINGPKTNO_MAX              10
#define L7_DHCPS_PINGPKTNOSET_MIN           2
#define L7_DHCPS_MAX_OPTION_CODE            256
#define L7_DHCPS_DEFAULT_MILLISECONDS_BETWEEN_PINGS 200

/******DHCP Lease Time Min Max for Days and Hours*****/

#define L7_DHCPS_LEASETIME_DAY_MIN        0
#define L7_DHCPS_LEASETIME_DAY_MAX        60
#define L7_DHCPS_LEASETIME_HRS_MIN        0
#define L7_DHCPS_LEASETIME_HRS_MAX        23
#define L7_DHCPS_LEASETIME_MINS_MIN       1     /* 1 minute */
#define L7_DHCPS_LEASETIME_MINS_MAX       59

/*****DHCP Prefix Min Max ********************/

#define L7_DHCPS_PREFIX_MIN               0
#define L7_DHCPS_PREFIX_MAX               32

/* DHCP Option row statrus*/
#define L7_DHCPS_NOT_READY 0
#define L7_DHCPS_ACTIVE 1

/* Defines for DHCP POOL Type */

#define L7_DHCPS_INACTIVE_POOL       0
#define L7_DHCPS_DYNAMIC_POOL        1
#define L7_DHCPS_MANUAL_POOL         2

/* To support SNMP create operation */
#define L7_DHCPS_NOT_READY_POOL      3


/* Defines for DHCP Lease Type */

#define L7_DHCPS_FREE_LEASE       0
#define L7_DHCPS_ACTIVE_LEASE     1
#define L7_DHCPS_OFFERED_LEASE    2
#define L7_DHCPS_EXPIRED_LEASE    3
#define L7_DHCPS_ABANDONED_LEASE  4

#define L7_DHCPS_ETHERNET           1
#define L7_DHCPS_IEEE_802           6

#define  L7_DHCPv6_RELAYOPT_MIN    32
#define  L7_DHCPv6_RELAYOPT_MAX    65535
#define  L7_DHCPv6_REMOTEID_MIN    1
#define  L7_DHCPv6_REMOTEID_MAX    65535

#define  L7_DHCPv6_RELAYOPT_MIN    32
#define  L7_DHCPv6_RELAYOPT_MAX    65535
#define  L7_DHCPv6_REMOTEID_MIN    1
#define  L7_DHCPv6_REMOTEID_MAX    65535

/* Definition for Netbios over TCP/IP Type option */
#define L7_DHCPS_B_NODE    0x1
#define L7_DHCPS_P_NODE    0x2
#define L7_DHCPS_M_NODE    0x4
#define L7_DHCPS_H_NODE    0x8

#define L7_DHCPS_PING           0
#define L7_DHCPS_GRATUITOUS_ARP 1

#define L7_DHCPS_NONE           0
#define L7_DHCPS_ASCII          1
#define L7_DHCPS_HEX            2
#define L7_DHCPS_IP             3

/*------------------------------------*/
/*  END  DHCP Server Parameter Limits */
/*------------------------------------*/

/******************** conditional Override *****************************/

#ifdef INCLUDE_DHCPS_EXPORTS_OVERRIDES
#include "dhcps_exports_overrides.h"
#endif

#endif /* __DHCPS_EXPORTS_H_*/
