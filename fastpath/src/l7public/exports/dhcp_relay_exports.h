/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename dhcp_relay_exports.h
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
* @created 10/06/2009
*
* @author rkanduri
* @end
*
**********************************************************************/

#ifndef __DHCP_RELAY_EXPORTS_H_
#define __DHCP_RELAY_EXPORTS_H_

/* RELAY Component Feature List */
typedef enum
{
  L7_RELAY_FEATURE_ID = 0,           
  L7_RELAY_OPTION82_INTF_FEATURE_ID,         /* relay option-82 check and insertion and interface level*/
  L7_RELAY_FEATURE_ID_TOTAL                     /* total number of enum values */
} L7_RELAY_FEATURE_IDS_t;

typedef enum
{
  L7_RELAY_INTF_DISABLE = 0,
  L7_RELAY_INTF_ENABLE,
  L7_RELAY_INTF_NOT_CONFIGURED
} dhcpRelayIntfValue_t;

#define FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_CIDOPTIONCHECK L7_DISABLE

/******************** conditional Override *****************************/

#ifdef INCLUDE_DHCP_RELAY_EXPORTS_OVERRIDES
#include "dhcp_relay_exports_overrides.h"
#endif

#endif /* __DHCP_RELAY_EXPORTS_H_*/


