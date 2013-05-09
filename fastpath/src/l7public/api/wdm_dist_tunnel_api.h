
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wdm_dist_tunnel.c
*
* @purpose      Distributed L2 tunneling control functions
*
* @component    Wireless
*
* @comments     none
*
* @create       12/12/2007
*
* @author       pmurali
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_WDM_DIST_TNNL_API_H
#define INCLUDE_WDM_DIST_TNNL_API_H

#include "datatypes.h"
#include "wireless_commdefs.h"

/*********************************************************************
*
* @purpose  Get the Distributed tunneling infomation for associated client
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
* @param    L7_uint32        intfNum @b{(output)} Network Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t wdmAssocClientL2DistTunnelInfoGet(L7_enetMacAddr_t clientMacAddr,
                                          L7_enetMacAddr_t apMacAddr,
                                          L7_enetMacAddr_t vapMacAddr,
                                          L7_uchar8 radio_if,
                                          L7_uchar8 *homeAP,
                                          L7_enetMacAddr_t *homeAPMacAddr,
                                          L7_in6_addr_t    *homeAPIPv6Addr);

/*********************************************************************
* @purpose  update WDM flags to send Terminate-L2-Distributed-Tunnel
*           message to associate AP.
*
* @param    macAddr   @b{(input)} Client MAC address.
*
*
* @returns  none
*
* @notes
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmL2DistTunnelUpdateTerminateMsgFlags(L7_enetMacAddr_t clientMacAddr);

/*********************************************************************
* @purpose update WDM flags to send Setup-L2-Distributed-Tunneling
*          message to Home AP.
*
* @param    macAddr   @b{(input)} Client MAC address.
*
*
* @returns  none
*
* @notes
*
*
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmL2DistTunnelUpdateSetupMsgFlags(L7_enetMacAddr_t clientMacAddr);

/*********************************************************************
*
* @purpose  Get the dist tunneled client next entry from associated
*           client for all switches.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t *nextMacAddr @b{(output)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr 00:00:00:00:00:00 returns first entry.
*
* @end
*
*********************************************************************/
L7_RC_t wdmL2DistTunnelClientNextGet(L7_enetMacAddr_t  clientMac,
                                     L7_enetMacAddr_t  *nextClientMac);

/*********************************************************************
* @purpose  Handle Client Disassociation
*
*
* @param    macAddr   @b{(input)} Client MAC address.
*
*
* @returns  none
*
* @notes   If the Home AP is managed by the switch then send
*          Terminate-L2-Distributed-Tunnel message to the AP.
*          If the AP is managed by the peer switch then when the peer
*          switch receives a Peer-Client-Remove message that affects
*          a Home AP that it manages then it sends
*          the Terminate-L2-Distributed-Tunnel message to the Home AP
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmClientL2DistTunnelDisassociateHandle(L7_enetMacAddr_t macAddr);

/*********************************************************************
* @purpose  Handle peer client remove message
*
* @param    macAddr   @b{(input)} Client MAC Address.
*
*
* @returns  none
*
* @notes    if client Home AP is managed by the switch then send
*           terminate-l2-distributed-tunnel message to home AP
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmPeerClientL2DistTunnelDisassociateHandle(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Function to process L2 Distributed Tunneling Global Config
*           that require to apply on all AP's managed by switch whenever
*           any config parameter changed by the administratot
* @returns  void
*
* @comments The wdmSema WDM_WRITE_LOCK has already been acquired by
*           the caller.  Do not invoke any API function that attempts
*           to take either the read or write version of this lock.
*
* @end
*
*********************************************************************/
void wdmL2DistTunnelConfigProcess(void);

#endif
