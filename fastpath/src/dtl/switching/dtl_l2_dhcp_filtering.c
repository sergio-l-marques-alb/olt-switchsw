/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  dtl_l2_dhcp_filtering.c
*
* @purpose   This file contains the functions to transform layer 2
*            DHCP filtering requests into driver requests.
*
* @component Device Transformation Layer (DTL)
*
* @comments  none
*
* @create    07/21/2005
*
* @author    sbasu
*
* @end
*
*********************************************************************/

/*
 * *******************************************************************
 *                           HEADER FILES
 * *******************************************************************
 */

#include "dtlinclude.h"

/*
 * *******************************************************************
 *                           PRIVATE FUNCTIONS PROTOTYPES
 * *******************************************************************
 */

/*
 * *******************************************************************
 *                           API FUNCTIONS
 * *******************************************************************
 */

/*********************************************************************
 * @purpose  Configures DHCP Filtering on an interface
 *
 * @param    intfNum          @b{(input)} Internal intf number
 * @param    trusted          @b{(input)} TRUE if the port is trusted
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @end
 *********************************************************************/
L7_RC_t dtlDhcpFilterIntfModeSet( L7_uint32 intfNum, L7_BOOL trusted )
{
/* Deprecated... replaced w/ DHCP Snooping */
#if 0
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t dr;
  L7_RC_t rc = L7_FAILURE;

  if ( nimGetUnitSlotPort( intfNum, &usp ) != L7_SUCCESS )
  {
    return( L7_FAILURE );
  }
  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.dhcpFilterConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.dhcpFilterConfig.trusted  = trusted;

  dr = dapiCtl( &ddUsp, DAPI_CMD_INTF_DHCP_FILTER_CONFIG, &dapiCmd );

  if ( L7_SUCCESS == dr )
  {
    rc = L7_SUCCESS;
  }

  return( rc );
#else
  return L7_SUCCESS;
#endif
}

/*********************************************************************
 * @purpose  Configures DHCP Filtering 
 *
 * @param    enabe          @b{(input)} TRUE if the admin mode is enable
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @end
 *********************************************************************/
L7_RC_t dtlDhcpFilterModeSet(L7_uint32 adminMode)
{
/* Deprecated... replaced w/ DHCP Snooping */
#if 0
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  /* ddUsp must be a valid usp so set it to zeros */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.dhcpFilterSystemConfig.getOrSet = DAPI_CMD_SET;
  if(adminMode)
	  dapiCmd.cmdData.dhcpFilterSystemConfig.enable = L7_TRUE;
  else
	  dapiCmd.cmdData.dhcpFilterSystemConfig.enable = L7_FALSE;

  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DHCP_FILTER_CONFIG, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
#else
  return L7_SUCCESS;
#endif
}

/*
 * *******************************************************************
 *                           PRIVATE FUNCTIONS
 * *******************************************************************
 */
