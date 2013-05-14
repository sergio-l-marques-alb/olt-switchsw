
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename dai_util.h
*
* @purpose  Working data structures etc.
*
* @component  Dynamic ARP Inspection
*
* @comments 
*
* @create 09/05/2007
*
* @author Kiran Kumar Kella
* @end
*
**********************************************************************/

#ifndef DAI_UTIL_H
#define DAI_UTIL_H


#include "nimapi.h"
#include "simapi.h"
#include "dot1q_api.h"
#include "avl_api.h"
#include "l7_packet.h"
#include "osapi_support.h"
#include "l7_cnfgr_api.h"

#define DAI_PKT_ALLOWED_MAX_SIZE      64

/* incl NULL terminator */
#define DAI_MAC_STR_LEN               18

#define GET_IP_FROM_PKT(addrbyte) \
      ((((L7_uint32) *(addrbyte))       << 24) |  \
       (((L7_uint32) *((addrbyte) + 1)) << 16) |  \
       (((L7_uint32) *((addrbyte) + 2)) <<  8) |  \
        ((L7_uint32) *((addrbyte) + 3)))

typedef enum 
{
    DAI_CNFGR_INIT = 0,
    DAI_VLAN_EVENT,
    DAI_TIMER_EVENT,
    DAI_INTF_EVENT,
    DAI_NIM_STARTUP
} daiMessages_t;

typedef enum
{
  SOURCE_MAC_FAILURE = 0,
  DEST_MAC_FAILURE,
  IP_VALID_FAILURE,
  ACL_MATCH_FAILURE,
  DHCP_SNOOP_DB_MATCH_FAILURE
} daiFailure_t;

typedef enum 
{
  DAI_TRACE_LOG = 0,
  DAI_TRACE_CONSOLE
} daiTraceDest_t;

/* Return code to indicate the result of applying
 * ARP ACL filter or DHCP snooping db match filter
 * on an ARP packet */
typedef enum
{
  DAI_FILTER_NONE = 0,
  DAI_FILTER_PASS,
  DAI_FILTER_FAIL
} daiFilterAction_t;

typedef struct daiVlanEventMsg_s
{
  /* VLAN ID of VLAN changed. */
  dot1qNotifyData_t vlanData;

  /* Internal interface number of port associated with VLAN. 
   * Only set for certain events. */
  L7_uint32 intIfNum;

  /* Type of VLAN event. */
  vlanNotifyEvent_t vlanEventType;

} daiVlanEventMsg_t;

typedef struct daiNimStartup_s
{
  NIM_STARTUP_PHASE_t startupPhase;
} daiNimStartup_t;

typedef struct daiFrameMsg_s
{
  /* Receive interface. Internal interface number. */
  L7_uint32 rxIntf;

  /* VLAN on which message was received */
  L7_ushort16 vlanId;

  /* Frame length */
  L7_uint32 dataLen;

  /* Entire ethernet frame. Don't want to hold mbuf on msg queue. */
  L7_uchar8 frameBuf[DAI_PKT_ALLOWED_MAX_SIZE];

} daiFrameMsg_t;

typedef struct daiIntfEventMsg_s
{
  L7_uint32           event;          /* one of L7_PORT_EVENTS_t from NIM */
  NIM_CORRELATOR_t    correlator;
  L7_uint32 intIfNum;    
} daiIntfEventMsg_t;

typedef struct
{
  daiMessages_t msgType;

  union
  {
    daiVlanEventMsg_t vlanEvent;
    daiIntfEventMsg_t intfEvent;
    L7_CNFGR_CMD_DATA_t cmdData;
    daiNimStartup_t   nimStartupEvent; 
  } daiMsgData;

} daiEventMsg_t;

typedef struct daiDebugStats_t
{
  /* Error receiving from message queues */
  L7_uint32 msgRxError;

  /* Failure putting a message on the frame message queue */
  L7_uint32 frameMsgTxError;

  /* Failure putting a message on the event message queue */
  L7_uint32 eventMsgTxError;

  /* Number of ARP messages that were received before the control
   * plane is ready. Such packets are dropped */
  L7_uint32 msgsDroppedControlPlaneNotReady;

  /* Packets received on port-based routing interface */
  L7_uint32 pktsOnPortRoutingIf;

  /* Packets received on Routing VLAN */
  L7_uint32 pktsOnVlanRoutingIf;

  /* Packets received on Non-Routing VLAN */
  L7_uint32 pktsOnVlanNonRoutingIf;

  /* Packets given to ARP application */
  L7_uint32 pktsToArpAppln;

  /* Packets given to System directly */
  L7_uint32 pktsToSystem;

  /* Packets received but not handled */
  L7_uint32 pktsNotHandled;

  /* ARP packets received with invalid length > 64 bytes */
  L7_uint32 pktsInvalidLength;

  /* Total packets intercepted */
  L7_uint32 pktsIntercepted;

  /* Total packets queued to daiTask */
  L7_uint32 pktsQueued;

  /* Number of Mbuf failures */
  L7_uint32 daiMbufFailures;

  /* Total packets verified to be valid */
  L7_uint32 pktsValidated;

  /* Packets failed to be transmitted in software */
  L7_uint32 pktTxFailures;

} daiDebugStats_t;

typedef struct daiInfo_s
{
  /* L7_TRUE if config data is no longer in sync with persistent storage. */
  L7_BOOL cfgDataChanged;

  /* Statistics for debugging. */
  daiDebugStats_t debugStats;

  /* Bit mask of NIM events that DAI is registered to receive */
  PORTEVENT_MASK_t nimEventMask;

  /* flag to indicate if the hardware update is completed by DAI application */
  L7_BOOL l2HwUpdateComplete;

} daiInfo_t;

typedef struct daiVlanInfo_s
{
  /* Per VLAN statistics for DAI */
  daiVlanStats_t stats;
} daiVlanInfo_t;

typedef struct daiIntfInfo_s
{
  /* A port may be a member of multiple VLANs. For each port, count the 
   * number of VLANs for which it is a member and the VLAN is enabled for
   * DAI. If 0, the port is considered disabled for DAI. */
  L7_ushort16 daiNumVlansEnabled;

  /* Number of packets received on this interface within a burst interval */
  L7_uint32   pktRxCount; 

  /* Currently running consecutive 1-second interval during which incoming
   * packet rate on this interface (if untrusted) has exceeded
   * configured rate limit */
  L7_uint32   consecutiveInterval;
} daiIntfInfo_t;

/* dai_main.c */
void daiTask(void);
L7_RC_t daiVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, 
                              L7_uint32 event);
void daiIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase);
L7_RC_t daiNimCreateStartup(void);
L7_RC_t daiNimActivateStartup(void);
L7_RC_t daiIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
L7_RC_t daiVlanEventProcess(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event);
L7_RC_t daiIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
L7_RC_t daiTimerStart(void);
void daiRateLimitCheck(void);
L7_BOOL rateLimitFilter(sysnet_pdu_info_t *pduInfo);
L7_BOOL macIpAddrFilter(L7_uchar8 *data, L7_uint32 vlanId, L7_uint32 intIfNum);
SYSNET_PDU_RC_t daiArpRecv(L7_uint32 hookId,
                           L7_netBufHandle bufHandle,
                           sysnet_pdu_info_t *pduInfo,
                           L7_FUNCPTR_t continueFunc);
L7_RC_t daiPacketQueue(L7_uchar8 *ethHeader, L7_uint32 dataLen,
                       sysnet_pdu_info_t *pduInfo);
void dtlArpPacketHook(L7_uint32 intIfNum, L7_ushort16 vlanId,
                      L7_uchar8 *frame, L7_uint32 dataLen);
void daiIpMapPacketHook(L7_uint32 intIfNum, L7_ushort16 vlanId,
                     L7_uchar8 *frame, L7_uint32 dataLen);
void daiFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId,
                     L7_uchar8 *frame, L7_uint32 dataLen);
daiFilterAction_t daiFrameARPAclFilter(L7_uint32 intIfNum, L7_ushort16 vlanId,
                                       L7_uchar8 *frame, L7_uint32 dataLen);
L7_BOOL daiFrameDHCPSnoopingDbFilter(L7_uint32 intIfNum, L7_ushort16 vlanId,
                                     L7_uchar8 *frame, L7_uint32 dataLen);
L7_RC_t daiFrameForward(L7_uint32 intIfNum, L7_ushort16 vlanId, 
                        L7_uchar8 *frame, L7_ushort16 frameLen);
L7_RC_t daiFrameUnicast(L7_uint32 outgoingIf, L7_uint32 vlanId,
                        L7_uchar8 *frame, L7_ushort16 frameLen);
L7_RC_t daiFrameFlood(L7_uint32 intIfNum, L7_ushort16 vlanId, 
                      L7_uchar8 *frame, L7_ushort16 frameLen);
L7_RC_t daiFrameSend(L7_uint32 intIfNum, L7_ushort16 vlanId, 
                    L7_uchar8 *frame, L7_ushort16 frameLen);

/* dai_util.c */
void daiConfigDataChange(void);
void daiCfgDataNotChanged(void);
L7_RC_t _daiVlanStatsClear(void);
L7_RC_t daiVlanEnableApply(L7_uint32 vlanId, L7_uint32 vlanCfg);
L7_RC_t daiIntfTrustApply(L7_uint32 intIfNum, L7_BOOL trust);
L7_RC_t daiIntfRateLimitApply(L7_uint32 intIfNum);
L7_BOOL daiIntfIsInspected(L7_uint32 intIfNum);
void daiIntfCreate(L7_uint32 intIfNum);
void daiIntfAttach(L7_uint32 intIfNum);
L7_RC_t daiIntfEnable(L7_uint32 intIfNum);
L7_RC_t daiIntfDisable(L7_uint32 intIfNum);
L7_RC_t daiAclNameStringCheck(L7_uchar8 *aclName);
void daiMacToString(L7_uchar8 *macAddr, L7_uchar8 *macAddrStr);
L7_RC_t daiStringToMac(L7_uchar8 *macStr, L7_uchar8 *macAddr);
void daiLogEthernetHeader(L7_enetHeader_t *ethHdr, daiTraceDest_t traceDest);
void daiLogArpPacket(L7_uchar8 *frame, L7_ushort16 vlanId,
                     L7_uint32 intIfNum, daiTraceDest_t traceDest);
void daiLogAndDropPacket(L7_uchar8 *frame, L7_ushort16 vlanId,
                         L7_uint32 intIfNum, daiFailure_t failure);

/* dai_debug.c */
void daiDebugStats(void);
void daiDebugStatsClear(void);
L7_RC_t daiValidate(void);
L7_RC_t daiTraceWrite(L7_uchar8 *daiTrace);
void daiTraceFlagsSet(L7_uint32 daiTraceFlags);
void daiTraceFlagsDump(void);
void daiDebugQueueStats(void);

/* dai_cfg.c */
void _daiVlanEnable(L7_uint32 vlanId);
void _daiVlanDisable(L7_uint32 vlanId);
L7_BOOL _daiVlanEnableGet(L7_uint32 vlanId);
void _daiIntfTrust(L7_uint32 intIfNum);
void _daiIntfUntrust(L7_uint32 intIfNum);
L7_BOOL _daiIntfTrustGet(L7_uint32 intIfNum);
void _daiVlanLogInvalid(L7_uint32 vlanId);
void _daiVlanDontLogInvalid(L7_uint32 vlanId);
L7_BOOL _daiVlanLogInvalidGet(L7_uint32 vlanId);
void _daiVlanStaticFlagSet(L7_uint32 vlanId);
void _daiVlanStaticFlagClear(L7_uint32 vlanId);
L7_BOOL _daiVlanStaticFlagGet(L7_uint32 vlanId);
void _daiVlanArpAclSet(L7_uint32 vlanId, L7_uchar8 *aclName);
void _daiVlanArpAclClear(L7_uint32 vlanId);
L7_BOOL _daiVlanArpAclGet(L7_uint32 vlanId, L7_uchar8 *aclName);
void daiBuildDefaultConfigData(L7_uint32 ver);
L7_RC_t daiSave(void);
L7_RC_t daiRestore(void);
L7_RC_t daiRestoreProcess(void);
L7_BOOL daiHasDataChanged(void);
void daiResetDataChanged(void);
L7_RC_t daiApplyConfigData(void);

L7_RC_t _arpAclCreate(L7_uchar8 *aclName);
L7_RC_t _arpAclDelete(L7_uchar8 *aclName);
L7_RC_t _arpAclGet(L7_uchar8 *aclName);
L7_RC_t _arpAclNextGet(L7_uchar8 *aclName, L7_uchar8 *nextAclName);
L7_RC_t _arpAclRuleAdd(L7_uchar8 *aclName, L7_uint32 ipAddress,
                       L7_uchar8 *macAddress);
L7_RC_t _arpAclRuleDelete(L7_uchar8 *aclName, L7_uint32 ipAddress,
                          L7_uchar8 *macAddress);
L7_RC_t _arpAclRuleGet(L7_uchar8 *aclName, L7_uint32 ipAddress, L7_uchar8 *macAddress);
L7_RC_t _arpAclRuleNextGet(L7_uchar8 *aclNameIn, L7_uint32 ipAddrIn, L7_uchar8 *macAddrIn,
                           L7_uchar8 *aclNameOut, L7_uint32 *ipAddrOut, L7_uchar8 *macAddrOut);
L7_RC_t _arpAclRuleInAclNextGet(L7_uchar8 *aclName, L7_uint32 ipAddrIn, L7_uchar8 *macAddrIn,
                                L7_uint32 *ipAddrOut, L7_uchar8 *macAddrOut);

/* dai_outcalls.c */
L7_BOOL daiIntfIsValid(L7_uint32 intIfNum);

#endif /* DAI_UTIL_H */
