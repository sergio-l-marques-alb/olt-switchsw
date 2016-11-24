/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   wireless_api.h
*
* @purpose    Genric wireless APIs accessed outside of the wireless code.
*
* @component  Wireless
*
* @comments   none
*
* @create     7/9/2006
*
* @author     atsigler
*
* @end
*
*********************************************************************/

#ifndef INCLUDE_WIRELESS_API_H
#define INCLUDE_WIRELESS_API_H

#include "datatypes.h"
#include "commdefs.h"
#include "comm_structs.h"
#include "l7_packet.h"
#include "l3_comm_structs.h"
#include "l7_cnfgr_api.h"

/*********************************************************************
* Tunneling API table containing function pointers for external APIs 
* called from other components.
*********************************************************************/
typedef struct wtnnlFuncTable_s
{
  L7_RC_t (*wtnnlIntIfNumGet)(L7_IP_ADDR_t clientIp4Addr, L7_uint32 *intIfNum);
  L7_RC_t (*wtnnlVlanIsTnnl)(L7_uint32 vlanId, L7_uint32 *intIfNum);
  L7_RC_t (*wtnnlL2McastWiredPktProcess)(L7_netBufHandle bufHandle, L7_uint32 intIfNum);
  L7_RC_t (*wtnnlPhase1Process)(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
  L7_RC_t (*wtnnlPhase2Process)(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
  L7_RC_t (*wtnnlPhase3Process)(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
  void (*wtnnlUconfigPhase2Process)(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
  void (*wtnnlNetworkPktMsgHandle)(L7_IP_ADDR_t wsapIp4Addr, L7_uchar8 *msg, L7_uint32 msgLen);
  void (*wtnnlWSAPAddHandle)(L7_uint32 peerType, L7_enetMacAddr_t peerMac);
  void (*wtnnlWSAPDeleteHandle)(L7_uint32 peerType);
  void (*wtnnlClientAPAssociateHandle)(L7_enetMacAddr_t clientMacAddr,
                                       L7_enetMacAddr_t apMacAddr,
                                       L7_rtrIntfIpAddr_t ip4Subnet);
  void (*wtnnlClientWSAssociateHandle)(L7_enetMacAddr_t clientMacAddr,
                                       L7_IP_ADDR_t wsIp4Addr,
                                       L7_rtrIntfIpAddr_t ip4Subnet);
} wtnnlFuncTable_t;

extern wtnnlFuncTable_t wtnnlFuncTable;

/*********************************************************************
* L2 Tunneling API table containing function pointers for external APIs 
* called from other components.
*********************************************************************/
typedef struct centtnnlFuncTable_s
{
  L7_RC_t (*centtnnlIntIfNumGet)(L7_IP_ADDR_t tunnelIp4Addr, L7_uint32 *intIfNum);
  L7_RC_t (*centtnnlPhase1Process)(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
  L7_RC_t (*centtnnlPhase2Process)(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
  L7_RC_t (*centtnnlPhase3Process)(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
  void (*centtnnlUconfigPhase2Process)(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
  void (*centtnnlWSAPAddHandle)(L7_uint32 peerType, L7_enetMacAddr_t peerMac);
  void (*centtnnlWSAPDeleteHandle)(L7_uint32 peerType);
  void (*centtnnlClientAssociateHandle)(L7_enetMacAddr_t clientMacAddr, L7_enetMacAddr_t peerMac,
                                        L7_IP_ADDR_t peerIp4Addr);
} centtnnlFuncTable_t;

extern centtnnlFuncTable_t centtnnlFuncTable;

/*********************************************************************
* @purpose  Initialize the function pointers in the function table
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void wtnnlFuncTableInit(void);

/*********************************************************************
* @purpose  Get Wireless Interface Number based on the DHCP Client's IP
*
* @param    clientIp4Addr @b{(input)}  IPv4 address of the DHCP Client
* @param    intIfNum      @b{(output)} Internal interface ID of wireless network
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wtnnlIntIfNumGet(L7_IP_ADDR_t clientIp4Addr, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Check if the VLAN is wireless tunnel VLAN
*
* @param    vlnaId        @b{(input)}  vlan id
* @param    intIfNum      @b{(output)} Internal interface ID of wireless network
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wtnnlVlanIsTnnl(L7_uint32 vlanId, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Process a L2 multicast packet received from a wired network
*
* @param    bufHandle     @b{(input)} Buffer handle to the frame
* @param    intIfNum      @b{(input)} Internal interface of received packet
*
* @returns  void
*
* @notes    This function must free bufHandle.
*
* @end
*********************************************************************/
L7_RC_t wtnnlL2McastWiredPktProcess(L7_netBufHandle bufHandle, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Process an NETWORK_PACKET msg received from an AP or Peer WS
*
* @param    wsapIp4Addr   @b{(input)} IPv4 address of AP or Peer WS
* @param    msg           @b{(input)} Message data
* @param    msgLen        @b{(input)} Message length
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void wtnnlNetworkPktMsgHandle(L7_IP_ADDR_t wsapIp4Addr, L7_uchar8 *msg, L7_uint32 msgLen);

/*********************************************************************
* @purpose  Handle the addition of an AP or Peer Switch.
*
* @param    type      @b{(input)} Type of peer (WS_AP or WS_PEER_SWITCH)
*
* @returns  void
*
* @notes    This is called from outside the tunneling subcomponent
*           to indicate the addition of a new AP or Peer Switch.
*
* @end
*********************************************************************/
void wtnnlWSAPAddHandle(L7_uint32 type, L7_enetMacAddr_t peerMac);

/*********************************************************************
* @purpose  Handle the deletion of an AP or Peer WS.
*
* @param    type      @b{(input)} Type of peer (WS_AP or WS_PEER_SWITCH)
*
* @returns  void
*
* @notes    This is called from outside the tunneling subcomponent
*           to indicate the deletion of an AP or Peer Switch.
*
* @end
*********************************************************************/
void wtnnlWSAPDeleteHandle(L7_uint32 type);

/*********************************************************************
* @purpose  Handle an association of a client with an AP.
*
* @param    clientMacAddr   @b{(input)} Client MAC address
* @param    apMacAddr       @b{(input)} AP MAC address
* @param    apMacAddr       @b{(input)} VAP tunnel subnet address
*
* @returns  void
*
* @notes    This is called from outside the tunneling subcomponent
*           to indicate an associate change for a given client.
*
* @end
*********************************************************************/
void wtnnlClientAPAssociateHandle(L7_enetMacAddr_t clientMacAddr,
                                  L7_enetMacAddr_t apMacAddr,
                                  L7_rtrIntfIpAddr_t ip4Subnet);

/*********************************************************************
* @purpose  Handle an association of a client with a Peer Switch.
*
* @param    clientMacAddr   @b{(input)} Client MAC address
* @param    wsIp4Addr       @b{(input)} Peer Switch IPv4 address
*
* @returns  void
*
* @notes    This is called from outside the tunneling subcomponent
*           to indicate an associate change for a given client.
*
* @end
*********************************************************************/
void wtnnlClientWSAssociateHandle(L7_enetMacAddr_t clientMacAddr,
                                  L7_IP_ADDR_t wsIp4Addr,
                                  L7_rtrIntfIpAddr_t ip4Subnet);

/*********************************************************************
* @purpose  This function processes the configurator Phase 1.
*
* @param    L7_CNFGR_RESPONSE_t *pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    L7_CNFGR_ERR_RC_t   *pReason    @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid responses:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t wtnnlPhase1Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

/*********************************************************************
* @purpose  This function processes the configurator Phase 2.
*
* @param    L7_CNFGR_RESPONSE_t *pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    L7_CNFGR_ERR_RC_t   *pReason    @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid responses:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t wtnnlPhase2Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

/*********************************************************************
* @purpose  This function processes the configurator Phase 3.
*
* @param    L7_CNFGR_RESPONSE_t *pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    L7_CNFGR_ERR_RC_t   *pReason    @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid responses:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t wtnnlPhase3Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

/*********************************************************************
* @purpose  This function processes the configurator Uconfig Phase2,
*             it also undoes discoverPhase3Process.
*
* @param    L7_CNFGR_RESPONSE_t *pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    L7_CNFGR_ERR_RC_t   *pReason    @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid responses:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
void wtnnlUconfigPhase2Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);


/*********************************************************************
* @purpose  Initialize the function pointers in the function table
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void centtnnlFuncTableInit(void);

/*********************************************************************
* @purpose  Handle the addition of an AP or Peer Switch.
*
* @param    type      @b{(input)} Type of peer (WS_AP or WS_PEER_SWITCH)
*
* @returns  void
*
* @notes    This is called from outside the tunneling subcomponent
*           to indicate the addition of a new AP or Peer Switch.
*
* @end
*********************************************************************/
void centtnnlWSAPAddHandle(L7_uint32 type, L7_enetMacAddr_t peerMac);

/*********************************************************************
* @purpose  Handle the deletion of an AP or Peer WS.
*
* @param    type      @b{(input)} Type of peer (WS_AP or WS_PEER_SWITCH)
*
* @returns  void
*
* @notes    This is called from outside the tunneling subcomponent
*           to indicate the deletion of an AP or Peer Switch.
*
* @end
*********************************************************************/
void centtnnlWSAPDeleteHandle(L7_uint32 type);

/*********************************************************************
* @purpose  Handle an association of a client with an AP.
*
* @param    clientMacAddr   @b{(input)} Client MAC address
* @param    apMacAddr       @b{(input)} AP MAC address
*
* @returns  void
*
* @notes    This is called from outside the tunneling subcomponent
*           to indicate an associate change for a given client.
*
* @end
*********************************************************************/
void centtnnlClientAssociateHandle(L7_enetMacAddr_t clientMacAddr,
                                   L7_enetMacAddr_t peerMac,
                                   L7_IP_ADDR_t peerIp4Addr);

/*********************************************************************
* @purpose  This function processes the configurator Phase 1.
*
* @param    L7_CNFGR_RESPONSE_t *pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    L7_CNFGR_ERR_RC_t   *pReason    @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid responses:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t centtnnlPhase1Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

/*********************************************************************
* @purpose  This function processes the configurator Phase 2.
*
* @param    L7_CNFGR_RESPONSE_t *pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    L7_CNFGR_ERR_RC_t   *pReason    @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid responses:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t centtnnlPhase2Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

/*********************************************************************
* @purpose  This function processes the configurator Phase 3.
*
* @param    L7_CNFGR_RESPONSE_t *pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    L7_CNFGR_ERR_RC_t   *pReason    @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid responses:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t centtnnlPhase3Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

/*********************************************************************
* @purpose  This function processes the configurator Uconfig Phase2,
*             it also undoes discoverPhase3Process.
*
* @param    L7_CNFGR_RESPONSE_t *pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    L7_CNFGR_ERR_RC_t   *pReason    @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid responses:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
void centtnnlUconfigPhase2Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

/*********************************************************************
* @purpose  Get Wireless Interface Number based on the DHCP Client's IP
*
* @param    tunnelIp4Addr @b{(input)}  IPv4 address of the DHCP Client
* @param    intIfNum      @b{(output)} Internal interface ID of wireless network
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t centtnnlIntIfNumGet(L7_IP_ADDR_t tunnelIp4Addr, L7_uint32 *intIfNum);

#endif /* INCLUDE_WIRELESS_API_H */




