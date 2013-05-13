/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename ds_util.h
*
* @purpose  Working data structures etc.
*
* @component  DHCP Snooping
*
* @comments 
*
* @create 3/15/2007
*
* @author Rob Rice (rrice)
* @end
*
**********************************************************************/

#ifndef DS_UTIL_H
#define DS_UTIL_H


//#include "nimapi.h"
//#include "simapi.h"
#include "dot1q_api.h"
#include "cli_txt_cfg_api.h"
#include "avl_api.h"
#include "l7_packet.h"
#include "osapi_support.h"
#include "l7_cnfgr_api.h"
//
///* incl NULL terminator */
//#define DS_MAC_STR_LEN 18
//
//#define DS_LEASE_TIMER_INTERVAL   1000
///* lease time option */
//#define DHO_DHCP_LEASE_TIME      51
///* message type option */
//#define DHO_DHCP_MESSAGE_TYPE             53
//#define DHCP_RELAY_AGENT_INFO_OPTION      82
//#define DHCP_OPTION_LENGTH_OFFSET         1
//#define DHCP_OPTION_CONTENT_OFFSET        2
//#define DHCP_RELAY_AGENT_CIRCUIT_ID_SUBOPTION  1
//#define DHCP_RELAY_AGENT_REMOTE_ID_SUBOPTION   2
//#define DHCP_RELAY_AGENT_CIRCUIT_ID_LENGTH     3  /* i.e. 3 bytes for inserting unit, slot, port.*/
//#define DHO_PAD                       0
//#define DHO_END                 255
//
//#if 1 /* PTin added: DHCPv6 */
//#define PTIN_ENTERPRISE_NUMBER  4746
//#endif
//
//#define DS_IFACE_INFO_STR_SIZE   L7_NIM_IFNAME_SIZE + 25
#define PPPOE_PACKET_SIZE_MAX  1518
typedef enum 
{
    PPPOE_CNFGR_INIT = 0
} pppoeMessages_t;
//
//typedef enum
//{
//  DS_ENTRY_ADD = 0,
//  DS_ENTRY_MODIFY,
//  DS_ENTRY_DELETE,
//  DS_ENTRY_FLUSH_ALL,
//  DS_ENTRY_EVENT_LAST
//} dsCkptEventType_t;
//
//typedef enum
//{
//  DS_CKPT_DONE = 0,
//  DS_CKPT_ADD,
//  DS_CKPT_DELETE,
//  DS_CKPT_LAST
//} dsCkptFlag_t;
//
//typedef enum
//{
//  DS_TRACE_LOG = 0,
//  DS_TRACE_CONSOLE
//} dsTraceDest_t;
//
//typedef struct dsDhcpsEventMsg_s
//{
//  L7_enetMacAddr_t chAddr;
//  L7_uint32 ipAddr;
//  L7_uint32 leaseTime;
//}dsDhcpsEventMsg_t;
//
//
//typedef struct dsVlanEventMsg_s
//{
//  /* VLAN ID of VLAN changed. */
//  dot1qNotifyData_t vlanData;
//
//  /* Internal interface number of port associated with VLAN.
//   * Only set for certain events. */
//  L7_uint32 intIfNum;
//
//  /* Type of VLAN event. */
//  vlanNotifyEvent_t vlanEventType;
//
//} dsVlanEventMsg_t;
//
//typedef struct dsCfgApplyEventMsg_s
//{
//  txtCfgApplyEvent_t txtCfgEventType;
//}dsCfgApplyEventMsg_t;
//
//typedef struct dsTftpEventMsg_s
//{
//  L7_BOOL dsDbDownloadStatus;
//}dsTftpEventMsg_t;
//
//typedef struct dsIntfEventMsg_s
//{
//  L7_uint32 intIfNum;
//  L7_uint32           event;          /* one of L7_PORT_EVENTS_t from NIM */
//  NIM_CORRELATOR_t    correlator;
//}dsIntfEventMsg_t;
//
//typedef struct dsNimStartup_s
//{
//  NIM_STARTUP_PHASE_t startupPhase;
//} dsNimStartup_t;
//
typedef struct pppoeFrameMsg_s
{
  /* Receive interface. Internal interface number. */
  L7_uint32 rxIntf;

  /* VLAN on which message was received */
  L7_ushort16 vlanId;

  /* Inner VLAN on which DHCP packet was received,
     if the received packet is double-tagged. */
  L7_ushort16 innerVlanId;

  /* Entire ethernet frame. Don't want to hold mbuf on msg queue. */
  L7_uchar8 frameBuf[PPPOE_PACKET_SIZE_MAX];

  /* Frame Len rx'ed fron sysnet.*/
  L7_uint32 frameLen;

  /* PTin added: DHCP snooping */
  L7_uint   client_idx;

} pppoeFrameMsg_t;
//
//
/* dsMsg_t only used for VLAN and configuration events. Don't want to embed
 * DHCP messages here, because that would cause queue elements to be much
 * larger. */
typedef struct
{
  pppoeMessages_t msgType;

  union
  {
    L7_CNFGR_CMD_DATA_t cmdData;
  } pppoeMsgData;

} pppoeEventMsg_t;
//
//typedef struct dsBindingsTable_s
//{
//  L7_uint32           maxBindings;
//  L7_uint32           staticBindings;
//  avlTreeTables_t     *treeHeap;
//  L7_uint32           treeHeapSize;
//  void                *dataHeap;   /* dsBindingTreeNode_t* */
//  L7_uint32           dataHeapSize;
//  avlTree_t           treeData;
//} dsBindingsTable_t;
//
//#ifdef L7_IPSG_PACKAGE
//
//typedef struct ipsgEntryTable_s
//{
//  L7_uint32           maxBindings;
//  L7_uint32           maxStaticBindings;
//  L7_uint32           currentStaticBindings;
//  avlTreeTables_t     *treeHeap;
//  L7_uint32           treeHeapSize;
//  void                *dataHeap;   /* ipsgEntryNode_t* */
//  L7_uint32           dataHeapSize;
//  avlTree_t           treeData;
//} ipsgEntryTable_t;
//
//#endif
//
//typedef struct dsDebugStats_t
//{
//  /* Error receiving from message queues */
//  L7_uint32 msgRxError;
//
//  /* Failure putting a message on the frame message queue */
//  L7_uint32 frameMsgTxError;
//
//  /* Failure putting a message on the event message queue */
//  L7_uint32 eventMsgTxError;
//
//  /* Illegal source IP address in snooped packet */
//  L7_uint32 badSrcAddr;
//
//  /* Number of DHCP messages that were received before the control
//   * plane is ready. Such packets are dropped */
//  L7_uint32 msgsDroppedControlPlaneNotReady;
//
//  /* Number of DHCP messages intercepted. This count is updated on the
//   * thread that processes the sysnet intercept. Compare to msgsReceived
//   * to see if packets get lost in transfer to DHCP snooping thread. */
//  L7_uint32 msgsIntercepted;
//
//  /* Number of DHCP messages processed */
//  L7_uint32 msgsReceived;
//
//  /* Number of DHCP messages filtered */
//  L7_uint32 msgsFiltered;
//
//  /* Number of DHCP messages forwarded */
//  L7_uint32 msgsForwarded;
//
//  /* Number of messages where forwarding attempt failed. Can be incremented
//   * for multiple ports for a single msg. */
//  L7_uint32 txFail;
//
//  /* Received a RELEASE or DECLINE from a client not in the bindings table */
//  L7_uint32 releaseDeclineUnkClient;
//
//  /* Failure adding binding to bindings table */
//  L7_uint32 bindingAddFail;
//
//  /* Failure updating an existing binding */
//  L7_uint32 bindingUpdateFail;
//
//  /* Number of bindings added to bindings table */
//  L7_uint32 bindingsAdded;
//
//  /* Number of bindings removed from bindings table */
//  L7_uint32 bindingsRemoved;
//
//  /* Number of DHCP client requests received with Option-82 */
//  L7_uint32 clientOption82Rx;
//
//  /* Number of DHCP client requests transmitted with Option-82 */
//  L7_uint32 clientOption82Tx;
//
//  /* Number of DHCP server replies received with Option-82 */
//  L7_uint32 serverOption82Rx;
//
//  /* Number of DHCP server replies transmitted with Option-82 */
//  L7_uint32 serverOption82Tx;
//
//  /* Number of messages intercepted on each interface. Indexed by
//   * internal interface number. */
//  L7_uint32 msgsInterceptedIntf[DS_MAX_INTF_COUNT + 1];
//
//  /* Number of DHCP messages dropped as static binding already exists */
//  L7_uint32 msgDroppedBindingExists;
//
//} dsDebugStats_t;
//
//typedef struct dsIntfStats_s
//{
//  /* Number of DHCP client packets whose source MAC address didn't
//   * match the client hardware address. */
//  L7_uint32 macVerify;
//
//  /* Number of DHCP client packets received on an interface different from the
//   * interface where the DHCP snooping binding says the client should be.
//   * Only DHCPRELEASE or DHCPDECLINE. */
//  L7_uint32 intfMismatch;
//
//  /* Number of DHCP server messages received on an untrusted port. */
//  L7_uint32 untrustedSvrMsg;
//
//  /* The following statistics are applicable for DHCP L2 Relay.*/
//  /* No. of server messages received with Option82 on untrusted interface */
//  L7_uint32 untrustedServerFramesWithOption82;
//  /* No. of server messages received without Option82 on trusted interface */
//  L7_uint32 trustedServerFramesWithoutOption82;
//  /* No. of client messages received without Option82 on trusted interface */
//  L7_uint32 trustedClientFramesWithoutOption82;
//  /* No. of client messages received with Option82 on untrusted interface */
//  L7_uint32 untrustedClientFramesWithOption82;
//
//} dsIntfStats_t;
//
//typedef struct dsRelayAgentInfoOption_s
//{
//  L7_BOOL circuitIdFlag;
//  nimUSP_t  usp;          /* Circuit-id in USP format.*/
//  L7_uint32 intIfNum;     /* Circuit-id in internal interface format.*/
//  L7_uint8  board_slot;   /* own-board slot */
//  L7_BOOL remoteIdFlag;
//  L7_uchar8 remoteId[DS_MAX_REMOTE_ID_STRING];
//}dsRelayAgentInfo_t;
//
//typedef struct dsInfo_s
//{
//  /* L7_TRUE if config data is no longer in sync with persistent storage. */
//  L7_BOOL cfgDataChanged;
//
//  /* L7_TRUE if config data is no longer in sync with persistent storage. */
//  L7_BOOL dsDbDataChanged;
//
//  /* Set of authorized clients learned by DHCP snooping. */
//  dsBindingsTable_t bindingsTable;
//
//  L7_uchar8  pktBuff[DS_DHCP_PACKET_SIZE_MAX];
//
//  /* Statistics for debugging. */
//  dsDebugStats_t debugStats;
//
//  /* Bit mask of NIM events that DHCP Snooping is registered to receive */
//  PORTEVENT_MASK_t nimEventMask;
//
//  /* Checkpointing information used for NSF */
//
//  /* DHCP Snooping component holds this semaphore when a checkpoint is pending */
//  void *ckptPendingSema;
//
//  /* would be set in DHCP Snooping control block of the active manager
//   * when a backup manager is recognized */
//  L7_BOOL backupMgrKnown;
//
//  /* If the component went into phase 3 during warm restart or not */
//  L7_BOOL warmRestart;
//
//  /* flag to indicate if the hardware update is completed by DHCP Snooping application */
//  L7_BOOL l2HwUpdateComplete;
//
//} dsInfo_t;
//
//#ifdef L7_IPSG_PACKAGE
//
//typedef struct ipsgInfo_s
//{
//  /* L7_TRUE if config data is no longer in sync with persistent storage. */
//  L7_BOOL cfgDataChanged;
//
//  /* Set of authorized clients learned by DHCP snooping and IPSG static entries. */
//  ipsgEntryTable_t ipsgEntryTable;
//
//  /* Statistics for debugging.
//  dsDebugStats_t debugStats; */
//
//} ipsgInfo_t;
//
//#endif
//
//typedef struct dsIntfInfo_s
//{
//  /* A port may be a member of multiple VLANs. For each port, count the
//   * number of VLANs for which it is a member and the VLAN is enabled for
//   * DHCP snooping. If 0, the port is considered disabled for DHCP snooping. */
//  L7_ushort16 dsNumVlansEnabled;
//
//  dsIntfStats_t dsIntfStats;
//
//  /* Number of packets received on this interface within a burst interval */
//  L7_uint32   pktRxCount;
//
//  /* Currently running consecutive 1-second interval during which incoming
//   * packet rate on this interface (if untrusted) has exceeded
//   * configured rate limit */
//  L7_uint32   consecutiveInterval;
//
// /* VLAN Mask to handle the multiple Include & Exclude events */
//  L7_VLAN_MASK_t dsVlans;
//
//} dsIntfInfo_t;
//
///* Structure to represent a DHCP snooping binding in each tree node.
// * Key is the MAC address. */
//typedef struct dsBindingTreeNode_s
//{
//  /* A MAC address uniquely identifies a node in the bindings tree. */
//  L7_enetMacAddr_t macAddr;
//
//  /* VLAN station is in. */
//  L7_ushort16 vlanId;
//
//  /* PTin added: DHCP snooping */
//  #if 1
//  /* Inner vlan */
//  L7_ushort16 innerVlanId;
//  #endif
//
//  /* IP address assigned to the station */
//#if 1 /* PTin modified: DHCPv6 snooping */
//  L7_inet_addr_t ipAddr;
//#else
//  L7_uint32 ipAddr;
//#endif
//  /* physical port where client is attached. */
//  L7_uint32 intIfNum;
//
//  /* Time when binding learned (seconds since boot) */
//  L7_uint32 leaseStart;
//
//  /* How long the lease is valid (seconds) */
//  L7_uint32 leaseTime;
//
//  dsBindingType_t bindingType;
//
//#ifdef L7_NSF_PACKAGE
//  dsCkptFlag_t    ckptFlag;  /* Flag to indicate whether to checkpoint
//                              * Add / Delete of this binding entry */
//#endif
//
//  void *next;                /* AVL next */
//
//} dsBindingTreeNode_t;
//
//typedef struct dsDbBindingTreeNode_s
//{
//
//  /* A MAC address uniquely identifies a node in the bindings tree. */
//  L7_enetMacAddr_t macAddr;
//
//  /* VLAN station is in. */
//  L7_ushort16 vlanId;
//
//  /* IP address assigned to the station */
//  L7_uint32 ipAddr;
//
//  /* physical port where client is attached. */
//  L7_uint32 intIfNum;
//
//  /* Time when binding learned (seconds since boot) */
//  L7_uint32 remLease;
//
//} dsDbBindingTreeNode_t;
//
//typedef struct dsDbCfgData_s
//{
//  L7_fileHdr_t          cfgHdr;
//#if (!L7_FEAT_DHCP_SNOOPING_DB_PERSISTENCY)
//  dsDbBindingTreeNode_t dsBindingDb[1];
//#else
//  dsDbBindingTreeNode_t dsBindingDb[L7_DHCP_SNOOP_MAX_BINDINGS];
//#endif
//
//  L7_uint32             checkSum;
//} dsDbCfgData_t;
//
//
//
//#define DHCP_L2RELAY_LOG(str,intIfNum, vlanId, ethHdr, ipHdr, dhcpPacket, traceDest) \
//{  \
//  static L7_uint32 lastMsg = 0; \
//  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1]; \
//  L7_uchar8 ifInfo[DS_IFACE_INFO_STR_SIZE]; \
//  if (osapiUpTimeRaw() > lastMsg) \
//  { \
//    lastMsg = osapiUpTimeRaw(); \
//    nimGetIntfName(intIfNum, L7_SYSNAME, ifName); \
//    osapiSnprintf(ifInfo, DS_IFACE_INFO_STR_SIZE, "interface %s vlanId %d",\
//                  ifName, vlanId);\
//    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID, str);\
//    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID, ifInfo);\
//    dsLogEthernetHeader(ethHdr, DS_TRACE_LOG);\
//    dsLogIpHeader(ipHdr, DS_TRACE_LOG);\
//    dsLogDhcpPacket(dhcpPacket, DS_TRACE_LOG);\
//  } \
//}
//
//
//
/* pppoe_main.c */
void pppoeTask(void);
//L7_RC_t dsTimerStart(void);
//void dsRateLimitCheck(void);
//L7_BOOL dsRateLimitFilter(sysnet_pdu_info_t *pduInfo);
//
//L7_RC_t dsVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum,
//                             L7_uint32 event);
//
//void dsIntfCreate(L7_uint32 intIfNum);
//void dsIntfAttach(L7_uint32 intIfNum);
//L7_RC_t dsIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
SYSNET_PDU_RC_t pppoePacketIntercept(L7_uint32 hookId,
                                     L7_netBufHandle bufHandle,
                                     sysnet_pdu_info_t *pduInfo,
                                     L7_FUNCPTR_t continueFunc);
L7_RC_t pppoePacketQueue(L7_uchar8 *ethHeader, L7_uint32 dataLen,
                         L7_ushort16 vlanId, L7_uint32 intIfNum,
                         L7_ushort16 innerVlanId, L7_uint *client_idx);           /* PTin modified: DHCP snooping */
L7_RC_t pppoeFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId,
                          L7_uchar8 *frame, L7_uint32 frameLen,
                          L7_ushort16 innerVlanId, L7_uint client_idx);          /* PTin modified: DHCP snooping */
//L7_RC_t dsDHCPv4FrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId,
//                             L7_uchar8 *frame, L7_uint32 frameLen,
//                             L7_ushort16 innerVlanId, L7_uint client_idx);          /* PTin modified: DHCP snooping */
//L7_RC_t dsDHCPv6FrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId,
//                             L7_uchar8 *frame, L7_uint32 frameLen,
//                             L7_ushort16 innerVlanId, L7_uint client_idx);          /* PTin modified: DHCP snooping */
//L7_BOOL dsFrameFilter(L7_uint32 intIfNum, L7_ushort16 vlanId,
//                      L7_uchar8 *frame, L7_ipHeader_t *ipHeader,
//                      L7_ushort16 innerVlanId, L7_uint *client_idx);          /* PTin modified: DHCP snooping */
//L7_BOOL dsFilterServerMessage(L7_uint32 intIfNum, L7_ushort16 vlanId,
//                              L7_uchar8 *frame, L7_ipHeader_t *ipHeader,
//                              L7_ushort16 innerVlanId, L7_uint *client_idx);   /* PTin modified: DHCP snooping */
//L7_BOOL dsFilterClientMessage(L7_uint32 intIfNum, L7_ushort16 vlanId,
//                              L7_uchar8 *frame, L7_ipHeader_t *ipHeader,
//                              L7_ushort16 innerVlanId, L7_uint *client_idx);   /* PTin modified: DHCP snooping */
//L7_BOOL dsFilterVerifyMac(L7_uint32 intIfNum, L7_ushort16 vlanId,
//                          L7_uchar8 *frame, L7_ipHeader_t *ipHeader);
//L7_RC_t dsBindingExtract(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId, /* PTin modified: DHCP */
//                         L7_dhcp_packet_t *dhcpPacket, L7_uint32 pktLen);
//L7_RC_t dsFrameForward(L7_uint32 intIfNum, L7_ushort16 vlanId,
//                       L7_uchar8 *frame, L7_ushort16 frameLen,
//                       L7_ushort16 innerVlanId, L7_uint client_idx,           /* PTin modified: DHCP snooping */
//                       L7_uint32 relayOptIntIfNum);
//L7_RC_t dsFrameFlood(L7_uint32 intIfNum, L7_ushort16 vlanId,
//                     L7_uchar8 *frame, L7_ushort16 frameLen,
//                     L7_BOOL requestFlag,
//                     L7_ushort16 innerVlanId, L7_uint client_idx);            /* PTin modified: DHCP snooping */
//L7_RC_t dsFrameSend(L7_uint32 intIfNum, L7_ushort16 vlanId,
//                    L7_ushort16 innerVlanId, L7_uint client_idx,              /* PTin modified: DHCP snooping */
//                    L7_uchar8 *frame, L7_ushort16 frameLen);
//L7_RC_t dsBindingNthEntryGet (dhcpSnoopBinding_t *dsBinding,
//                               L7_uint32 n,
//                               dsBindingType_t type );
//
//
//
///* ds_util.c */
//void dsConfigDataChange(void);
//void dsCfgDataNotChanged(void);
//L7_RC_t dsTraceWrite(L7_uchar8 *dsTrace);
L7_RC_t pppoeAdminModeApply(L7_uint32 adminMode);
L7_RC_t pppoeAdminModeEnable(void);
L7_RC_t pppoeAdminModeDisable(L7_BOOL clearBindings);
//L7_RC_t _dsBindingClear(L7_uint32 intIfNum, L7_BOOL clrFlag);
//L7_RC_t _dsIntfStatsClear(void);
//L7_RC_t dsVlanEnableApply(L7_uint32 vlanId, L7_uint32 vlanCfg);
//L7_RC_t dsIntfTrustApply(L7_uint32 intIfNum, L7_BOOL trust);
//
//L7_RC_t dsIntfEnable(L7_uint32 intIfNum);
//L7_RC_t dsIntfDisable(L7_uint32 intIfNum);
//
//#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
//L7_RC_t dsL2RelaySubscriptionEntryGet(L7_uint32 intIfNum,
//                                      L7_uchar8 *subscriptionName,
//                                      L7_BOOL exactFlag,
//                                      dsL2RelaySubscriptionCfg_t **subsCfg);
//L7_RC_t dsL2RelaySubscriptionEntryFree(dsL2RelaySubscriptionCfg_t *subsCfg);
//#endif
//L7_RC_t dsIntfChangeCallback(L7_uint32 intIfNum,
//                             L7_uint32 event,
//                             NIM_CORRELATOR_t correlator);
//void dsIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase);
//L7_RC_t dsNimCreateStartup(void);
//L7_RC_t dsNimActivateStartup(void);
//L7_BOOL dsIntfIsSnooping(L7_uint32 intIfNum);
//L7_RC_t dsIntfSnoopingEnable(L7_uint32 intIfNum);
//L7_RC_t dsIntfSnoopingDisable(L7_uint32 intIfNum);
//L7_RC_t dsIntfRateLimitApply(L7_uint32 intIfNum);
//void dsMacToString(L7_uchar8 *macAddr, L7_uchar8 *macAddrStr);
//L7_RC_t dsStringToMac(L7_uchar8 *macStr, L7_enetMacAddr_t *macAddr);
//L7_dhcp_pkt_type_t dsPacketType(L7_dhcp_packet_t *packet, L7_uint32 pktLen);
//L7_uint32 dsLeaseTimeGet(L7_dhcp_packet_t *packet, L7_uint32 pktLen);
//L7_ushort16 dsIpHdrLen(L7_ipHeader_t *ipHeader);
//void dsLogEthernetHeader(L7_enetHeader_t *ethHdr, dsTraceDest_t traceDest);
//void dsLogIpHeader(L7_ipHeader_t *ipHdr, dsTraceDest_t traceDest);
//void dsLogDhcpPacket(L7_dhcp_packet_t *dhcpPacket, dsTraceDest_t traceDest);
//void dsL2RelayDetailLog(L7_uchar8 *msgTypeStr, L7_uint32 intfIfNum, L7_ushort16 vlanId,
//                        L7_enetHeader_t *ethHdr,L7_ipHeader_t *ipHdr,
//                        L7_dhcp_packet_t *dhcpPacket, dsTraceDest_t traceDest);
//
//L7_RC_t dsDbRemoteSave();
//L7_RC_t dsDbRemoteRestore();
//void dsTftpUploadInit();
//void dsTftpDownloadInit();
//void dsDbLocalSave();
//void dsDbLocalRestore();
//
//L7_RC_t dsIntfEnable(L7_uint32 intIfNum);
//L7_RC_t dsIntfDisable(L7_uint32 intIfNum);
//
//
//
///* ds_debug.c */
//void dsDebugStatsClear(void);
//
///* ds_cfg.c */
//void _dsVlanEnable(L7_uint32 vlanId);
//void _dsVlanDisable(L7_uint32 vlanId);
//L7_BOOL _dsVlanEnableGet(L7_uint32 vlanId);
//
///*********************************************************************
//* @purpose  Sets the L2Relay for a given interface in the config
//*
//* @param    intIfNum   @b((input)) internal interface number
//*
//* @returns  none
//*
//* @end
//*********************************************************************/
//void _dsIntfL2RelayEnable(L7_uint32 intIfNum);
///*********************************************************************
//* @purpose  Resets the L2Relay for a given interface in the config
//*
//* @param    intIfNum   @b((input)) internal interface number
//*
//* @returns  none
//*
//* @end
//*********************************************************************/
//void _dsIntfL2RelayDisable(L7_uint32 intIfNum);
///*********************************************************************
//* @purpose  Determines if an interface is enabled for DHCP L2Relay
//*
//* @param    intIfNum   @b((input)) internal interface number
//*
//* @returns  L7_TRUE if interface is L2 Realy enabled.
//*
//* @end
//*********************************************************************/
//L7_BOOL _dsIntfL2RelayGet(L7_uint32 intIfNum);
///*********************************************************************
//* @purpose  Set the L2 Relay trust bit for a given interface in the config
//*
//* @param    intIfNum   internal interface number
//*
//* @returns  none
//*
//* @end
//*********************************************************************/
//
///* PTin added: DHCP snooping */
///**
// * Verify if a particular (internal) vlan+interface is
// * part of a DHCP active EVC.
// *
// * @param vlanId : internal vlan
// * @param intIfNum : interface reference
// *
// * @return L7_BOOL : L7_TRUE/L7_FALSE
// */
//L7_BOOL dsVlanIntfIsSnooping(L7_uint16 vlanId, L7_uint32 intIfNum);
//
///**
// * Verify if a particular (internal) vlan+interface is
// * part of a DHCP active EVC.
// *
// * @param vlanId : internal vlan
// * @param intIfNum : interface reference
// *
// * @return L7_BOOL : L7_TRUE/L7_FALSE
// */
//L7_BOOL _dsVlanIntfL2RelayGet(L7_uint16 vlanId, L7_uint32 intIfNum);
///* PTin end */
//
//void _dsIntfL2RelayTrust(L7_uint32 intIfNum);
///*********************************************************************
//* @purpose  Reset the L2 Relay trust bit for a given interface in the config
//*
//* @param    intIfNum   internal interface number
//*
//* @returns  none
//*
//* @end
//*********************************************************************/
//void _dsIntfL2RelayDistrust(L7_uint32 intIfNum);
///*********************************************************************
//* @purpose  Determine whether a given interface is trusted for DHCP L2 Relay.
//*
//* @param    intIfNum   internal interface number
//*
//* @returns  L7_TRUE if interface is trusted.
//*
//* @end
//*********************************************************************/
//L7_BOOL _dsIntfL2RelayTrustGet(L7_uint32 intIfNum);
//
///* PTin added: DHCP snooping */
///**
// * Check if a particular (internal) vlan+interface is part of a DHCP
// * active EVC, with its interface as root (trusted interface)
// *
// * @param vlanId : internal vlan
// * @param intIfNum : interface
// *
// * @return L7_BOOL : L7_TRUE/L7_FALSE
// */
//L7_BOOL _dsVlanIntfTrustGet(L7_uint16 vlanId, L7_uint32 intIfNum);
//
///**
// * Check if a particular (internal) vlan+interface is part of a DHCP
// * active EVC, with its interface as root (trusted interface)
// *
// * @param vlanId : internal vlan
// * @param intIfNum : interface
// *
// * @return L7_BOOL : L7_TRUE/L7_FALSE
// */
//L7_BOOL _dsVlanIntfL2RelayTrustGet(L7_uint16 vlanId, L7_uint32 intIfNum);
//
///*********************************************************************
//* @purpose  Sets the L2Relay for a given vlan in the config
//*
//* @param    vlanNum   @b((input)) VLAN id
//*
//* @returns  none
//*
//* @end
//*********************************************************************/
//void _dsVlanL2RelayEnable(L7_uint32 vlanNum);
///*********************************************************************
//* @purpose  Resets the L2Relay for a given vlan in the config
//*
//* @param    vlanNum   @b((input)) VLAN id
//*
//* @returns  none
//*
//* @end
//*********************************************************************/
//void _dsVlanL2RelayDisable(L7_uint32 intIfNum);
///*********************************************************************
//* @purpose  Determines if a vlan is enabled for DHCP L2Relay
//*
//* @param    vlanNum   @b((input)) VLAN id
//*
//* @returns  L7_TRUE if interface is L2 Realy enabled.
//*
//* @end
//*********************************************************************/
//L7_BOOL _dsVlanL2RelayGet(L7_uint32 vlanNum);
//
///*********************************************************************
//* @purpose  Sets the L2Relay Circuit-Id for a given vlan in the config
//*
//* @param    vlanNum   @b((input)) VLAN Id
//*
//* @returns  none
//*
//* @end
//*********************************************************************/
//void _dsVlanL2RelayCircuitIdEnable(L7_uint32 vlanNum);
///*********************************************************************
//* @purpose  Resets the L2Relay Circuit-Id for a given vlan in the config
//*
//* @param    vlanNum   @b((input)) VLAN Id
//*
//* @returns  none
//*
//* @end
//*********************************************************************/
//void _dsVlanL2RelayCircuitIdDisable(L7_uint32 vlanNum);
///*********************************************************************
//* @purpose  Determines if a vlan is enabled for DHCP L2Relay Circuit-Id
//*
//* @param    intIfNum   @b((input)) VLAN Id
//*
//* @returns  L7_TRUE if interface is L2 Realy enabled.
//*
//* @end
//*********************************************************************/
//L7_BOOL _dsVlanL2RelayCircuitIdGet(L7_uint32 vlanNum);
///*********************************************************************
//* @purpose  Determines if a vlan is enabled for DHCP L2Relay Remote-Id
//*
//* @param    vlanNum   @b((input)) VLAN Id
//*
//* @returns  L7_TRUE if vlan is L2 Realy enabled.
//*
//* @end
//*********************************************************************/
//L7_BOOL _dsVlanL2RelayRemoteIdGet(L7_uint32 vlanNum);
//
///*********************************************************************
//* @purpose  Apply a change to the global DHCP L2Relay config state
//*
//* @param    adminMode   @b((input)) DHCP L2 Relay admin mode
//*
//* @returns  L7_SUCCESS
//* @returns  L7_FAILURE
//*
//* @notes    none
//*
//* @end
//*********************************************************************/
//L7_RC_t dsL2RelayAdminModeApply(L7_uint32 adminMode);
///*********************************************************************
//* @purpose  Apply a change to the global DHCP L2Relay config state
//*
//* @param    intIfNum   @b((input)) internal interface number
//* @param    intfMode   @b((input)) DHCP L2 Relay interface mode
//*
//* @returns  L7_SUCCESS
//* @returns  L7_FAILURE
//*
//* @notes    none
//*
//* @end
//*********************************************************************/
//L7_RC_t dsL2RelayIntfModeApply(L7_uint32 intfIfNum, L7_uint32 intfMode);
//
///*********************************************************************
//* @purpose  Enable DHCP snooping on a port.
//*
//* @param    intIfNum   @b((input)) internal interface number
//* @param    intfMode   @b((input)) DHCP Snooping interface mode
//*
//* @returns  L7_SUCCESS
//*
//* @notes
//*
//* @end
//*********************************************************************/
//L7_RC_t dsIntfSnoopingApply(L7_uint32 intIfNum, L7_uint32 intfMode);
//
//
//
//void _dsIntfTrust(L7_uint32 intIfNum);
//void _dsIntfDistrust(L7_uint32 intIfNum);
//L7_BOOL _dsIntfTrustGet(L7_uint32 intIfNum);
//void _dsIntfLogInvalid(L7_uint32 intIfNum);
//void _dsIntfDontLogInvalid(L7_uint32 intIfNum);
//L7_BOOL _dsIntfLogInvalidGet(L7_uint32 intIfNum);
//void dsBuildDefaultConfigData(L7_uint32 ver);
//L7_RC_t dsSave(void);
//L7_RC_t dsRestore(void);
//L7_RC_t dsRestoreProcess(void);
//L7_BOOL dsHasDataChanged(void);
//void dsResetDataChanged(void);
//L7_RC_t dsApplyConfigData(void);
//L7_RC_t dsApplyConfigCompleteCb(L7_uint32 event);
//L7_RC_t dsTftpStatusCb(L7_uint32 direction,L7_BOOL downloadStatus);
//
//#ifdef L7_DHCPS_PACKAGE
//
//L7_RC_t dsLocalServerCb(L7_uint32 sendMsgType,
//                        L7_enetMacAddr_t chAddr,
//                        L7_uint32 ipAddr,
//                        L7_uint32 leaseTime);
//#endif
//
//
///* ds_binding.c */
//L7_RC_t dsBindingsTableCreate(void);
//L7_RC_t dsBindingsTableDelete(void);
//L7_RC_t dsBindingAdd(dsBindingType_t bindingType, L7_enetMacAddr_t *macAddr,
//                     L7_uint32 ipAddr,
//                     L7_ushort16 vlanId, L7_ushort16 innerVlanId /*PTin modified: DHCP */, L7_uint32 intIfNum);
//L7_RC_t dsv6BindingAdd(dsBindingType_t bindingType, L7_enetMacAddr_t *macAddr,
//                       L7_inet_addr_t ipAddr, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint32 intIfNum);
//L7_RC_t dsBindingRemove(L7_enetMacAddr_t *macAddr);
//L7_BOOL dsBindingExists(L7_enetMacAddr_t *macAddr, L7_uint32 ipAddr,
//                        L7_ushort16 vlanId);
//L7_RC_t dsBindingFind(dhcpSnoopBinding_t *dsBinding, L7_uint32 matchType);
//L7_RC_t dsBindingIpAddrSet(L7_enetMacAddr_t *macAddr, L7_uint32 ipAddr);
//L7_RC_t dsv6BindingIpAddrSet(L7_enetMacAddr_t *macAddr, L7_inet_addr_t ipAddr);
//L7_RC_t dsBindingLeaseSet(L7_enetMacAddr_t *macAddr, L7_uint32 leaseTime);
//L7_uint32 _dsBindingsCount(void);
//L7_uint32 _dsStaticBindingsCount(void);
//L7_RC_t dsTimerStop(void);
//L7_RC_t dsLeaseCheck(void);
//L7_RC_t dstxtCfgEventProcess(L7_uint32);
//void dsTimerCallback(void);
//L7_RC_t dsBindingsValidate(void);
//
///* ds_outcalls.c */
//L7_BOOL dsIntfTypeIsValid(L7_uint32 intIfNum, L7_uint32 sysIntfType);
//L7_BOOL dsIntfIsValid(L7_uint32 intIfNum);
//
///* TBD : This function is external to DHCP.
//   This API is need to be provided by DOT1AD.
//   This API and its prototype needs to be
//   removed from here when DOT1AD provides equivalent. */
//L7_uint32 dot1adSVlanIdGet(L7_uint32 cVlanId);
//L7_uint32 dot1adCVlanIdGet(L7_uint32 sVlanId);

#endif
