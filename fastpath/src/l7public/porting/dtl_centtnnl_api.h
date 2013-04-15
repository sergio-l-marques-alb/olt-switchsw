
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename   dtl_centtnnl_api.h
 *
 * @purpose    This files contains the the necessary prototypes,
 *             for Centralized L2 Tunnel Dtl calls
 *
 * @component  Device Transformation Layer
 *
 * @comments   none
 *
 * @create     12/12/2007
 *
 * @author     jshaw
 * @end
 *
 **********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DTL_CENTTNNL_API_H
#define INCLUDE_DTL_CENTTNNL_API_H

#include "dapi.h"
#include "datatypes.h"
#include "l7_packet.h"

/*
  /------------------------------------------------------------------\
  *           Protocol independent APIs                              *
  \------------------------------------------------------------------/
*/

/*********************************************************************
* @purpose  Create a tunnel in the driver
*
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    tunnelType       @b{(input)} Tunnel mode
* @param    localAddr        @b{(input)} Source transport address
* @param    remoteAddr       @b{(input)} Destination transport address
* @param    remoteMacAddr    @b{(input)} Destination MAC Address
* @param    vlanId           @b{(input)} Tunnel VLAN
* @param    nextHopIpAddr    @b{(input)} Next hop IP address
* @param    nextHopIntIfNum  @b{(input)} Internal interface of next hop
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL2TunnelCreate(L7_uint32 intIfNum, L7_uint32 tunnelType,
                                  L7_sockaddr_union_t *localIpAddr,
                                  L7_enetMacAddr_t *localMacAddr,
                                  L7_sockaddr_union_t *remoteIpAddr,
                                  L7_enetMacAddr_t *remoteMacAddr,
                                  L7_ushort16 vlanId,
                                  L7_sockaddr_union_t *nextHopIpAddr, 
                                  L7_uint32 nextHopIntIfNum);

/*********************************************************************
* @purpose  Modify a tunnel in the driver
*
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    tunnelType       @b{(input)} Tunnel mode
* @param    localAddr        @b{(input)} Source transport address
* @param    remoteAddr       @b{(input)} Destination transport address
* @param    remoteMacAddr    @b{(input)} Destination MAC Address
* @param    vlanId           @b{(input)} Tunnel VLAN
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL2TunnelModify(L7_uint32 intIfNum, L7_uint32 tunnelType,
                                  L7_sockaddr_union_t *localIpAddr,
                                  L7_enetMacAddr_t *localMacAddr,
                                  L7_sockaddr_union_t *remoteIpAddr,
                                  L7_enetMacAddr_t *remoteMacAddr,
                                  L7_ushort16 vlanId);

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
EXT_API L7_RC_t dtlL2TunnelDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Add a tunneled vlan in the driver
*
* @param    vlanId    @b{(input)} vlan to add or remove
* @param    addflag   @b{(input)} flag for add or remove
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL2TunnelVlanAddRemove(L7_ushort16 vlanId, L7_BOOL addflag);

/*********************************************************************
* @purpose  Set the UDP ports to use for CAPWAP data traffic
*
* @param    udpSrcPort    @b{(input)} source udp port for capwap
* @param    udpDstPort   @b{(input)} destination udp port for capwap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL2TunnelUdpPortsSet(L7_ushort16 udpSrcPort, L7_ushort16 udpDstPort);

#endif /* INCLUDE_DTL_CENTTNNL_API_H */
