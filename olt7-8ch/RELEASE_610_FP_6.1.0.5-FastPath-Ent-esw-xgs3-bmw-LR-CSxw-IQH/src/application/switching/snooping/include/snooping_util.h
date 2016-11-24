/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  snooping_util.h
*
* @purpose   Snooping utility function prototypes
*
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/

#ifndef SNOOPING_UTIL_H
#define SNOOPING_UTIL_H

#include "snooping.h"

// PTin added
#define SNOOP_IGMP_NONE   0
#define SNOOP_IGMP_JOIN   1
#define SNOOP_IGMP_LEAVE  2
#define SNOOP_IGMP_QUERY  3

#define SNOOP_IGMP_V3_RECORD_TYPE_INCLUDE 1
#define SNOOP_IGMP_V3_RECORD_TYPE_EXCLUDE 2

#define SNOOP_IGMP_QUERY_GENERAL  0
#define SNOOP_IGMP_QUERY_GROUP    1

extern volatile int igmp_intercept_debug;
// PTin end

struct snoop_cb_s;
struct mgmdSnoopControlPkt_s;

L7_RC_t snoop_dot1qOperVlanMemberGet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 *mode);
L7_RC_t snoop_dot1qVlanMemberGet(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 *mode);
L7_RC_t snoop_dot1qVlanIntfIntIfNumToVlanId(L7_uint32 intIfNum, L7_uint32 *vlanId);
L7_RC_t snoop_dot1qVIDListMaskGet(L7_uint32 intIfNum, L7_VLAN_MASK_t *data);

L7_RC_t snoopIGMPAdminModeApply(L7_uint32 adminMode, L7_uint32 intIfNum, L7_uint16 vlanId, L7_uint8 prio);
L7_RC_t snoopMLDAdminModeApply(L7_uint32 adminMode);

L7_RC_t snoopEntriesDoFlush(snoop_cb_t *pSnoopCB);
/* PTin added: IGMP */
#if 1
L7_RC_t snoopEntryDoFlush(L7_uint16 vlan_id, snoop_cb_t *pSnoopCB);
#endif

/* Mrouter interface routines */
L7_RC_t snoopMcastRtrListAdd(L7_uint32 vlanId, L7_uint32 intIfNum, 
                             snoop_requestor_t req, struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopMcastRtrListRemove(L7_uint32 vlanId, L7_uint32 intIfNum, 
                                snoop_requestor_t req, struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopAllMcastRtrIntfsAdd(L7_uchar8 *macAddr, L7_uint32 vlanId, 
                                 L7_uint32 intIfNum, struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopIntfFromVlanRemove(L7_uint32 intIfNum, L7_uint32 vlanId, 
                                struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopIntfListGet(L7_uchar8 *macaddr,L7_uint32 vlanId,
                         L7_INTF_MASK_t *snoopMemberList, L7_uchar8 family);
void    snoopAllVlansGet(void);

L7_BOOL snoopMapIntfIsConfigurable(L7_uint32 intIfNum, 
                                   snoopIntfCfgData_t **pCfg, 
                                   struct snoop_cb_s *pSnoopCB);


/* Interface event processing routines */
L7_RC_t snoopIntfCheck(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uchar8 family);
L7_BOOL snoopMapIntfConfigEntryGet(L7_uint32 intIfNum, 
                                   snoopIntfCfgData_t **pCfg, 
                                   struct snoop_cb_s *pSnoopCB);

/******************************************************************************
  PDU Processing Routines
*******************************************************************************/
void    snoopFrameTransmit(L7_uint32 intIfNum, L7_uint32 vlanId, 
                           L7_netBufHandle bufHandle, snoop_send_type_t type);
L7_RC_t snoopPacketFlood(struct mgmdSnoopControlPkt_s *mcastPacket);
L7_RC_t snoopPacketClientIntfsForward(struct mgmdSnoopControlPkt_s *mcastPacket, L7_uint8 type);  // PTin added
L7_RC_t snoopPacketRtrIntfsForward(struct mgmdSnoopControlPkt_s *mcastPacket, L7_uint8 type);
L7_RC_t snoopPacketModifyInnerTag(L7_uint32 intIfNum, L7_uint32 vlanId,
                                  L7_uint32 innerVIDUntagged,
                                  L7_netBufHandle bufHandle);
L7_RC_t snoopBuildInsertInnerTagFrameTransmit(L7_uint32 intIfNum,  L7_uint32 vlanId,
                                              L7_netBufHandle orgBufHandle);
L7_RC_t snoopPacketFloodInsertInnerTag(L7_uint32 vlanId,
                                       L7_netBufHandle bufHandle);
/******************************************************************************
  Helper Routines
*******************************************************************************/
L7_ushort16 snoopCheckSum(L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum);
L7_uint32   snoopCheckPrecedenceParamGet(L7_uint32 vlanId, L7_uint32 intIfNum, 
                                         snoop_params_t param, L7_uchar8 family);
void snoopMulticastMacFromIpAddr(L7_inet_addr_t *ipAddr, L7_uchar8 *macAddr);
void snoopInetHtoP(L7_inet_addr_t *addr, L7_uchar8 *string);
L7_RC_t snoopMacAddrCheck(L7_uchar8 *macAddr, L7_uchar8 family);
L7_RC_t snoopCfgDump(void);
void snoopFPDecode(L7_uchar8 charCode, L7_short16 *val);
void snoopMLDFPDecode(L7_ushort16 shortCode, L7_uint32 *val);
void snoopNotifyL3Mcast(L7_uchar8 *destMac, L7_uint32 vlanId, L7_INTF_MASK_t *outIntIfMask);
L7_uchar8 snoopGetEndianess(void);
/*********************************************************************
  Timer Routines
*********************************************************************/
void snoopTimerProcess(L7_uint32 timerCBHandle, snoop_eb_t *pSnoopEB);
void snoopTimerUpdate(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum, 
                         L7_uint32 vlanId, snoop_interface_type_t timerType,
                         L7_uint32 timerValue, struct snoop_cb_s *pSnoopCB);
void     snoopTimerStop(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum, 
                         L7_uint32 vlanId, snoop_interface_type_t timerType,
                         struct snoop_cb_s *pSnoopCB);
L7_RC_t  snoopTimerStart(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum, 
                         L7_uint32 vlanId,snoop_interface_type_t timerType,
                         struct snoop_cb_s *pSnoopCB);
void     snoopGroupMembershipExpiry(void *param);
void     snoopMrtrExpiry(void *param);
L7_RC_t  snoopTimerDataDestroy(L7_sll_member_t *ll_member);
L7_int32 snoopTimerDataCmp(void *p, void *q, L7_uint32 key);
L7_RC_t  snoopTimerDestroy(L7_APP_TMR_CTRL_BLK_t timerCB, L7_APP_TMR_HNDL_t *timer, 
                           L7_uint32 *handle);

/***************************************************************
  Utility routines
****************************************************************/
#define SNOOP_GET_BYTE(val, cp) ((val) = *(L7_uchar8 *)(cp)++)

#define SNOOP_GET_LONG(val, cp) \
  do { \
       memcpy(&(val),(cp), sizeof(L7_uint32));\
       (val) = osapiNtohl((val));\
       (cp) += sizeof(L7_uint32);\
     } while (0)

#define SNOOP_GET_SHORT(val, cp) \
  do { \
       memcpy(&(val),(cp), sizeof(L7_ushort16));\
       (val) = osapiNtohs((val));\
       (cp) += sizeof(L7_ushort16);\
     } while (0)

#define SNOOP_GET_ADDR(addr, cp) \
  do { \
       memcpy((addr),(cp), sizeof(L7_uint32));\
       (*addr) = osapiNtohl((*addr));\
       (cp) += sizeof(L7_uint32);\
     } while (0)
     
#define SNOOP_GET_ADDR6(addr, cp)\
  do { \
       register L7_uchar8 *Xap; \
       register int i; \
       Xap = (L7_uchar8 *)(addr); \
       for (i = 0; i < 16; i++) \
         *Xap++ = *(cp)++; \
     } while (0)
 
#define SNOOP_PUT_BYTE(val, cp) (*(cp)++ = (L7_uchar8)(val))
  
#define SNOOP_PUT_SHORT(val, cp) \
  do { \
       (val) = osapiHtons((val));\
       memcpy((cp), &(val), sizeof(L7_ushort16));\
       (cp) += sizeof(L7_ushort16); \
     } while (0)

#define SNOOP_PUT_DATA(data, len, cp) (memcpy (cp, data, len), (cp) += (len))

#define SNOOP_UNUSED_PARAM(x) ((void)(x))
#define SNOOP_INTERVAL_ROUND(x, y) (((x) % (y) != 0) ? ((x)/(y))+1 : (x)/(y))
/* The below macro,compared to the above one is for a buffer of 1 in the case when x is equal to y,
   which actually is to take care of processing time as in the case of max response time which is 1 sec */
#define SNOOP_MAXRESP_INTVL_ROUND(x, y) (((x)/(y))+1)
#define SNOOP_L3_INGRESS_VLAN_MASK_PRESENT 0x01
#define SNOOP_L3_EGRESS_VLAN_MASK_PRESENT  0x10
#endif /* SNOOPING_UTIL_H */
