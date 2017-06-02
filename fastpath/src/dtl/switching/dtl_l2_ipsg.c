/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  dtl_l2_ipsg.c
*
* @purpose   This file contains the functions to transform layer 2
*            IP Source Guard requests into driver requests.
*
* @component Device Transformation Layer (DTL)
*
* @comments  none
*
* @create    03/30/2007
*
* @author    colinw
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
* @purpose  Configure IP Source Guard on an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    enabled     @b{(input)} Intf is enabled for IP Source Guard
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlIpsgConfig(L7_uint32 intIfNum,
                      L7_BOOL   enabled)
{
  DAPI_INTF_MGMT_CMD_t cmd;
  nimUSP_t             usp;
  DAPI_USP_t           ddUsp;
  L7_RC_t              rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.ipsgEnable.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.ipsgEnable.enabled = enabled;

    rc = dapiCtl(&ddUsp, DAPI_CMD_INTF_IPSG_CONFIG, &cmd);
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the number of dropped packets due to IPSG on an interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    *droppedPkts @b{(input)} pointer to dropped packets counter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlIpsgDroppedPktsGet(L7_uint32  intIfNum,
                              L7_uint32 *droppedPkts)
{
  DAPI_INTF_MGMT_CMD_t cmd;
  nimUSP_t             usp;
  DAPI_USP_t           ddUsp;
  L7_RC_t              rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.ipsgStats.getOrSet = DAPI_CMD_GET;
    cmd.cmdData.ipsgStats.droppedPackets = droppedPkts;

    rc = dapiCtl(&ddUsp, DAPI_CMD_INTF_IPSG_STATS_GET, &cmd);
  }

  return rc;
}

/*********************************************************************
* @purpose  Configure an allowable IP/MAC pair for IP Source Guard on an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipv4Addr    @b{(input)} IPv4 address of allowable client
* @param    macAddr     @b{(input)} Optional MAC address of allowable client
*                                   Set to all zero's if the MAC is not used.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlIpsgIpv4ClientAdd(L7_uint32        intIfNum,
                             L7_in_addr_t     ipv4Addr,
                             L7_enetMacAddr_t macAddr)
{
  DAPI_INTF_MGMT_CMD_t cmd;
  nimUSP_t             usp;
  DAPI_USP_t           ddUsp;
  L7_RC_t              rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.ipsgClientAdd.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.ipsgClientAdd.ip4Addr = ipv4Addr.s_addr;
    memset(&cmd.cmdData.ipsgClientAdd.ip6Addr, 0, sizeof(L7_in6_addr_t));
    cmd.cmdData.ipsgClientAdd.macAddr = macAddr;

    rc = dapiCtl(&ddUsp, DAPI_CMD_INTF_IPSG_CLIENT_ADD, &cmd);
  }

  return rc;
}

/*********************************************************************
* @purpose  Configure an allowable IP/MAC pair for IP Source Guard on an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipv4Addr    @b{(input)} IPv6 address of allowable client
* @param    macAddr     @b{(input)} Optional MAC address of allowable client
*                                   Set to all zero's if the MAC is not used.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlIpsgIpv6ClientAdd(L7_uint32        intIfNum,
                             L7_in6_addr_t    ipv6Addr,
                             L7_enetMacAddr_t macAddr)
{
  DAPI_INTF_MGMT_CMD_t cmd;
  nimUSP_t             usp;
  DAPI_USP_t           ddUsp;
  L7_RC_t              rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.ipsgClientAdd.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.ipsgClientAdd.ip4Addr = 0;
    cmd.cmdData.ipsgClientAdd.ip6Addr = ipv6Addr;
    cmd.cmdData.ipsgClientAdd.macAddr = macAddr;

    rc = dapiCtl(&ddUsp, DAPI_CMD_INTF_IPSG_CLIENT_ADD, &cmd);
  }

  return rc;
}

/*********************************************************************
* @purpose  Remove an allowable IP/MAC pair for IP Source Guard from an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipv4Addr    @b{(input)} IPv4 address of allowable client
* @param    macAddr     @b{(input)} Optional MAC address of allowable client
*                                   Set to all zero's if the MAC is not used.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlIpsgIpv4ClientRemove(L7_uint32        intIfNum,
                                L7_in_addr_t     ipv4Addr)
{
  DAPI_INTF_MGMT_CMD_t cmd;
  nimUSP_t             usp;
  DAPI_USP_t           ddUsp;
  L7_RC_t              rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.ipsgClientDelete.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.ipsgClientDelete.ip4Addr = ipv4Addr.s_addr;
    memset(&cmd.cmdData.ipsgClientDelete.ip6Addr, 0, sizeof(L7_in6_addr_t));

    rc = dapiCtl(&ddUsp, DAPI_CMD_INTF_IPSG_CLIENT_DELETE, &cmd);
  }

  return rc;
}

/*********************************************************************
* @purpose  Remove an allowable IP/MAC pair for IP Source Guard from an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipv4Addr    @b{(input)} IPv6 address of allowable client
* @param    macAddr     @b{(input)} Optional MAC address of allowable client
*                                   Set to all zero's if the MAC is not used.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlIpsgIpv6ClientRemove(L7_uint32        intIfNum,
                                L7_in6_addr_t    ipv6Addr)
{
  DAPI_INTF_MGMT_CMD_t cmd;
  nimUSP_t             usp;
  DAPI_USP_t           ddUsp;
  L7_RC_t              rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.ipsgClientDelete.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.ipsgClientDelete.ip4Addr = 0;
    cmd.cmdData.ipsgClientDelete.ip6Addr = ipv6Addr;

    rc = dapiCtl(&ddUsp, DAPI_CMD_INTF_IPSG_CLIENT_DELETE, &cmd);
  }

  return rc;
}

/*
 * *******************************************************************
 *                           PRIVATE FUNCTIONS
 * *******************************************************************
 */
