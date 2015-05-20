/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2005-2007
 *
 **********************************************************************
 *
 * @filename   dtl_l3_tnnl.c
 *
 * @purpose    Dtl Common Tunnel functions
 *
 * @component  Device Transformation Layer
 *
 * @comments   Provides L3 Protocol Neutral interface management
 *
 * @create     08/15/2005
 *
 * @author     eberge
 * @end
 *
 **********************************************************************/

#include "l3_comm_structs.h"
#include "dtlinclude.h"
#include "dtl_l3_api.h"


/*********************************************************************
* @purpose  Create a tunnel in the driver
*
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    tunnelMode       @b{(input)} Tunnel mode
* @param    localAddr        @b{(input)} Source transport address
* @param    remoteAddr       @b{(input)} Destination transport address
* @param    nextHopAddr      @b{(input)} Transport Nexthop Address
* @param    nextHopIntIfNum  @b{(input)} Transport Nexthop interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlTunnelCreate(L7_uint32 intIfNum, L7_uint32 tunnelMode,
                        L7_sockaddr_union_t *localAddr,
                        L7_sockaddr_union_t *remoteAddr,
                        L7_sockaddr_union_t *nextHopAddr,
                        L7_uint32 nextHopIntIfNum)
{
  L7_uint32 sysIntfType;
  DAPI_TUNNEL_MGMT_CMD_t dapiCmd;
  nimUSP_t usp;
  DAPI_USP_t ddUsp, nhUsp;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FAILURE;

  if (sysIntfType != L7_TUNNEL_INTF)
    return L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  if (nimGetUnitSlotPort(nextHopIntIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  nhUsp.unit = usp.unit;
  nhUsp.slot = usp.slot;
  nhUsp.port = usp.port - 1;

  dapiCmd.cmdData.tunnelCreate.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.tunnelCreate.tunnelMode = tunnelMode;
  dapiCmd.cmdData.tunnelCreate.localAddr = *localAddr;
  dapiCmd.cmdData.tunnelCreate.remoteAddr = *remoteAddr;
  dapiCmd.cmdData.tunnelCreate.nextHopAddr = *nextHopAddr;
  dapiCmd.cmdData.tunnelCreate.nextHopUsp = nhUsp;

  if (dapiCtl(&ddUsp, DAPI_CMD_TUNNEL_CREATE, &dapiCmd) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a tunnel in the driver
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlTunnelDelete(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType;
  DAPI_TUNNEL_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t   usp;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FAILURE;

  if (sysIntfType != L7_TUNNEL_INTF)
    return L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.tunnelDelete.getOrSet = DAPI_CMD_SET;

  if (dapiCtl(&ddUsp, DAPI_CMD_TUNNEL_DELETE, &dapiCmd) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the transport nexhop of a tunnel.
*
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    nextHopAddr      @b{(input)} Transport Nexthop Address
* @param    nextHopIntIfNum  @b{(input)} Transport Nexthop Interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlTunnelNextHopSet(L7_uint32 intIfNum,
                            L7_sockaddr_union_t *nextHopAddr,
                            L7_uint32 nextHopIntIfNum)
{
  L7_uint32 sysIntfType;
  DAPI_TUNNEL_MGMT_CMD_t dapiCmd;
  nimUSP_t usp;
  DAPI_USP_t ddUsp, nhUsp;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FAILURE;

  if (sysIntfType != L7_TUNNEL_INTF)
    return L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  if (nimGetUnitSlotPort(nextHopIntIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  nhUsp.unit = usp.unit;
  nhUsp.slot = usp.slot;
  nhUsp.port = usp.port - 1;

  dapiCmd.cmdData.tunnelNextHopSet.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.tunnelNextHopSet.nextHopAddr = *nextHopAddr;
  dapiCmd.cmdData.tunnelNextHopSet.nextHopUsp = nhUsp;

  if (dapiCtl(&ddUsp, DAPI_CMD_TUNNEL_NEXT_HOP_SET, &dapiCmd) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a tunnel in the driver
*
* @param    tunnelMode       @b{(input)} Tunnel mode
* @param    localAddr        @b{(input)} Source transport address
* @param    remoteAddr       @b{(input)} Destination transport address
* @param    nextHopAddr      @b{(input)} Transport Nexthop Address
* @param    nextHopIntIfNum  @b{(input)} Transport Nexthop interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlL3TunnelAdd(L7_uint32 tunnelMode,
                       L7_sockaddr_union_t *localAddr,
                       L7_sockaddr_union_t *remoteAddr,
                       L7_sockaddr_union_t *nextHopAddr,
                       L7_uint32 nextHopIntIfNum)
{
  DAPI_TUNNEL_MGMT_CMD_t dapiCmd;
  nimUSP_t usp;
  DAPI_USP_t ddUsp, nhUsp;
  
  ddUsp.unit = ddUsp.slot = ddUsp.port = -1;

  if (nimGetUnitSlotPort(nextHopIntIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  nhUsp.unit = usp.unit;
  nhUsp.slot = usp.slot;
  nhUsp.port = usp.port - 1;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  dapiCmd.cmdData.l3TunnelAddDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.l3TunnelAddDelete.tunnelMode = tunnelMode;
  dapiCmd.cmdData.l3TunnelAddDelete.localAddr = *localAddr;
  dapiCmd.cmdData.l3TunnelAddDelete.remoteAddr = *remoteAddr;
  dapiCmd.cmdData.l3TunnelAddDelete.nextHopAddr = *nextHopAddr;
  dapiCmd.cmdData.l3TunnelAddDelete.nextHopUsp = nhUsp;

  if (dapiCtl(&ddUsp, DAPI_CMD_L3_TUNNEL_ADD, &dapiCmd) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a tunnel in the driver
*
* @param    tunnelMode       @b{(input)} Tunnel mode
* @param    localAddr        @b{(input)} Source transport address
* @param    remoteAddr       @b{(input)} Destination transport address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlL3TunnelDelete(L7_uint32 tunnelMode,
                          L7_sockaddr_union_t *localAddr,
                          L7_sockaddr_union_t *remoteAddr)
{
  DAPI_TUNNEL_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  
  ddUsp.unit = ddUsp.slot = ddUsp.port = -1;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  dapiCmd.cmdData.l3TunnelAddDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.l3TunnelAddDelete.tunnelMode = tunnelMode;
  dapiCmd.cmdData.l3TunnelAddDelete.localAddr = *localAddr;
  dapiCmd.cmdData.l3TunnelAddDelete.remoteAddr = *remoteAddr;

  if (dapiCtl(&ddUsp, DAPI_CMD_L3_TUNNEL_DELETE, &dapiCmd) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a tunnel MAC address in the driver
*
* @param    tunnelMode       @b{(input)} Tunnel mode
* @param    localAddr        @b{(input)} Source transport address
* @param    remoteAddr       @b{(input)} Destination transport address
* @param    macAddr          @b{(input)} MAC address
* @param    intIfNum         @b{(input)} Internal interface ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlL3TunnelMacAddressAdd(L7_uint32 tunnelMode,
                                 L7_sockaddr_union_t *localAddr,
                                 L7_sockaddr_union_t *remoteAddr,
                                 L7_uchar8 *macAddr,
                                 L7_uint32 intIfNum)
{
  DAPI_TUNNEL_MGMT_CMD_t dapiCmd;
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  dapiCmd.cmdData.l3TunnelMacAddressAddDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.l3TunnelMacAddressAddDelete.tunnelMode = tunnelMode;
  dapiCmd.cmdData.l3TunnelMacAddressAddDelete.localAddr = *localAddr;
  dapiCmd.cmdData.l3TunnelMacAddressAddDelete.remoteAddr = *remoteAddr;
  memcpy(&dapiCmd.cmdData.l3TunnelMacAddressAddDelete.macAddr, macAddr,
         L7_ENET_MAC_ADDR_LEN);

  if (dapiCtl(&ddUsp, DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD, &dapiCmd)
      != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a tunnel MAC address in the driver
*
* @param    macAddr          @b{(input)} MAC address
* @param    intIfNum         @b{(input)} Internal interface ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlL3TunnelMacAddressDelete(L7_uchar8 *macAddr, L7_uint32 intIfNum)
{
  DAPI_TUNNEL_MGMT_CMD_t dapiCmd;
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  dapiCmd.cmdData.l3TunnelMacAddressAddDelete.getOrSet = DAPI_CMD_SET;
  memcpy(&dapiCmd.cmdData.l3TunnelMacAddressAddDelete.macAddr, macAddr,
         L7_ENET_MAC_ADDR_LEN);

  if (dapiCtl(&ddUsp, DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE, &dapiCmd)
      != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
