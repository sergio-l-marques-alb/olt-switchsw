/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    snooping_debug.h
* @purpose     Snoop debug functions
* @component   SNOOPING
* @comments    none
* @create      08/22/2006
* @author      ssuvarna
*              drajendra
* @end
*
**********************************************************************/

#ifndef SNOOP_DEBUG_H
#define SNOOP_DEBUG_H

#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "snooping.h"

#define SNOOP_USER_TRACE_TX(__fmt__, __args__... )                              \
           LOG_USER_TRACE(L7_SNOOPING_COMPONENT_ID, __fmt__,##__args__);

#define SNOOP_USER_TRACE_RX(__fmt__, __args__... )                              \
           LOG_USER_TRACE(L7_SNOOPING_COMPONENT_ID, __fmt__,##__args__);

#define SNOOP_DEBUG_PRINTF sysapiPrintf

#define SNOOP_MAX_DEBUG_MSG_SIZE 512

#define SNOOP_TRACE(traceFlag,snoop_family, __fmt__, __args__...)  { \
   if (snoopDebugTraceFlagCheck ((traceFlag),(snoop_family)) == L7_TRUE)\
   {\
     L7_char8  __buf1__[SNOOP_MAX_DEBUG_MSG_SIZE];    \
     L7_char8  __buf2__[SNOOP_MAX_DEBUG_MSG_SIZE];    \
     L7_timespec time;                                \
      osapiUpTime(&time);                             \
     (void)osapiSnprintf (__buf1__, SNOOP_MAX_DEBUG_MSG_SIZE, __fmt__, ## __args__);  \
          (void)osapiSnprintf (__buf2__, SNOOP_MAX_DEBUG_MSG_SIZE,                    \
               "\n%02d:%02d:%02d %s SNOOP: In %s() %d - %s",                             \
       time.hours, time.minutes, time.seconds, ((snoop_family) == L7_AF_INET) ? "IGMP" : (((snoop_family) == L7_AF_INET6) ? "MLD" : "Base"),\
       __FUNCTION__,__LINE__,__buf1__);       \
        SNOOP_DEBUG_PRINTF(__buf2__); \
   }\
 }


typedef enum
{
  SNOOP_DEBUG_TIMER = 0,
  SNOOP_DEBUG_QUERIER,
  SNOOP_DEBUG_PROTO,
  SNOOP_DEBUG_INTERNAL,
  SNOOP_DEBUG_CHECKPOINT,
  SNOOP_DEBUG_CHECKPOINT_VERBOSE,

  SNOOP_DEBUG_FLAGS_BITS
} SNOOP_TRACE_FLAGS_t;

typedef enum
{
  SNOOP_PKT_DROP_NOT_READY = 1,
  SNOOP_PKT_DROP_BAD_VLAN,
  SNOOP_PKT_DROP_BAD_ETYPE,
  SNOOP_PKT_DROP_RX_CPU,
  SNOOP_PKT_DROP_BAD_IPHDR_LEN,
  SNOOP_PKT_DROP_BAD_IGMP_CKSUM,
  SNOOP_PKT_DROP_BAD_IP_CKSUM,
  SNOOP_PKT_DROP_BAD_IPV4_DEST,
  SNOOP_PKT_DROP_BAD_ICMP_CKSUM

} SNOOP_DEBUG_DROP_FLAGS_t;


struct mgmdSnoopControlPkt_s;

L7_RC_t snoopDebugTraceEnable (void);
void    snoopDebugTraceFlagsSetAF(L7_uint32 flag);

/*********************************************************************
*
* @purpose Trace snoop packets received
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   vlanId   @b{(input)} Vlan ID
* @param   *igmpPacket  @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void snoopDebugPacketRxTrace(struct mgmdSnoopControlPkt_s *mcastPacket, L7_uchar8 drop);

/*********************************************************************
*
* @purpose Trace snoop packets transmitted
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   vlanId   @b{(input)} Vlan ID
* @param   *igmpPacket  @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void snoopDebugPacketTxTrace(L7_uint32 intIfNum, L7_uint32 vlanId,
                             L7_uchar8 *payload, L7_uchar8 family);
/*********************************************************************
* @purpose  Copy the trace settings to the debug config file
*
* @param    pSnoopCB   @b{(input)} Control block
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void snoopDebugCfgUpdate(snoop_cb_t *pSnoopCB);
/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void snoopDebugRegister(void);
/*********************************************************************
* @purpose  Read and apply the debug config of a snoop instance
*
* @param    pSnoopCB   @b{(input)} Control block
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void snoopDebugCfgRead(snoop_cb_t *pSnoopCB);
/*********************************************************************
*
* @purpose
*
* @param    flag   {(input)} Trace Level
* @param    family {(input)} Snoop Instance
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
extern L7_BOOL
snoopDebugTraceFlagCheck (L7_uint32 traceFlag, L7_uchar8 family);
#endif /* SNOOP_DEBUG_H */
