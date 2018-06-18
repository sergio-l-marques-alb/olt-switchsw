/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_ip.c
*
* Purpose: This file contains the functions to transform the L3 IP
*          components's driver requests. 
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: Shekhar Kalyanam 3/14/2001 
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/





/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_IP_GLOBALS                /* Enable global space   */    
#include "dtlinclude.h"

#if DTLCTRL_COMPONENT_IP


/*
**********************************************************************
*                           PRIVATE FUNCTIONS PROTOTYPES 
**********************************************************************
*/


/*
**********************************************************************
*                           API FUNCTIONS 
**********************************************************************
*/
/*********************************************************************
* @purpose  Sets the IP Address for the system
*
* @param    ipAddr   @b{(input)} IP Address for the system in 32 bit notation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @notes    
*           
* @end
*********************************************************************/
L7_RC_t dtlIPAddrSystemSet(L7_ulong32 ipAddr)
{

  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.systemIpAddress.ipAddress = ipAddr;
  dapiCmd.cmdData.systemIpAddress.getOrSet = DAPI_CMD_SET;

  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}

L7_RC_t dtlBootPDhcpSystemConfig(DTL_DHCP_CONFIG_t config)
{
#if 0
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  DAPI_DHCP_CONFIG_t dapiConfig;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  switch (config)
  {
  case DTL_DHCP_CONFIG_NONE:
    dapiConfig = DAPI_DHCP_CONFIG_NONE;
    break;
  case DTL_DHCP_CONFIG_DHCP:
    dapiConfig = DAPI_DHCP_CONFIG_DHCP;
    break;
  case DTL_DHCP_CONFIG_BOOTP:
    dapiConfig = DAPI_DHCP_CONFIG_BOOTP;
    break;
  case DTL_DHCP_CONFIG_DHCP_AND_BOOTP:
    dapiConfig = DAPI_DHCP_CONFIG_DHCP_AND_BOOTP;
    break;
  }

  dapiCmd.cmdData.bootpDhcpConfig.config = dapiConfig;
  dapiCmd.cmdData.bootpDhcpConfig.getOrSet = DAPI_CMD_SET;

  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_BOOTP_DHCP_CONFIG, &dapiCmd);

  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
#else
  /* colinw: The above code has been disabled since the driver doesn't support 
             the operation */
  return L7_SUCCESS;
#endif

}
/*
**********************************************************************
*                           PRIVATE FUNCTIONS 
**********************************************************************
*/

#endif /*DTLCTRL_COMPONENT_IP*/
