/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename rlim.h
*
* @purpose Contains prototypes and Data Structures
*          for managing routing logical interfaces
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 02/16/2005
*
* @author eberge
* @end
*
**********************************************************************/

#ifndef INCLUDE_RLIM_H
#define INCLUDE_RLIM_H

#include <string.h>
#include "l7_socket.h"
#include "osapi_support.h"
#include "osapi_sockdefs.h"
#include "async_event_api.h"
#include "rlim_api.h"
#include "rlim_cfg.h"
#include "rlim_cnfgr.h"
#include "rlim_mask.h"
#include "l3_defaultconfig.h"
#include "l7utils_api.h"
#include "avl_api.h"

#define RLIM_INTF_LOOPBACK_MTU  1500

/* RFC 4213, Section 3.2.1 
 * A node using static tunnel MTU treats the tunnel interface as having
 * a fixed-interface MTU.  By default, the MTU MUST be between 1280 and
 * 1480 bytes (inclusive), but it SHOULD be 1280 bytes.  If the default
 * is not 1280 bytes, the implementation MUST have a configuration knob
 * that can be used to change the MTU value.
 * NOTE: configurability of tunnel MTU is not supported as of now */

#define RLIM_INTF_TUNNEL_MTU    1280

/*
 * How long to wait before processing best route events (in milliseconds)
 */
#define RLIM_NHOP_CHANGE_DELAY   1000

/*
 * Periodic age check for tunnel dest (in milliseconds)
 */
#define RLIM_TUNNEL_DEST_AGE_TIME   (60 *1000)
#define RLIM_TUNNEL_DEST_AGE_MAX    (60 * 60)         /* (in seconds) */

/*
 * Operational defines and data structures
 */
typedef struct {
  void *processQueue;
  L7_uint32 procTaskHandle;
  void *cnfgrQueue;
  L7_uint32 cnfgrTaskHandle;
  osapiTimerDescr_t *ip4NhopChangeTimer;
  osapiTimerDescr_t *tunnelDestAgeTimer;
} rlimOpData_t;

typedef struct {
  L7_uint32 loopbackIntIfNum;
  void *deleteSema;
} rlimLoopbackOpData_t;

typedef struct {
  L7_uint32 tunnelIntIfNum;
  L7_uint32 tunnelActiveMode;
  L7_uint32 flags;
  L7_uint32 localIp4Addr;
  L7_in6_addr_t localIp6Addr;
  L7_uint32 localIntIfNum;
  L7_uint32 nextHopIp4Addr;
  L7_uint32 nextHopIp4IntIfNum;
  L7_in6_addr_t nextHopIp6Addr;
  L7_uint32 nextHopIp6IntIfNum;
  void *deleteSema;
} rlimTunnelOpData_t;


typedef struct rlim6to4Dest_s{
  L7_uint32                 tunnelId;
  L7_uint32                 remoteAddr;
  L7_uint32                 flags;
  L7_uint32                 nextHopIp4Addr;
  /* the last time this destination is created in stack */
  L7_uint32                 timeStamp;
  /* ipv4 route metric to the remoteAddr */
  L7_uint32                 metric;
  /* 6to4 neighbor ipv6 address */
  L7_in6_addr_t             nbr;
} rlim6to4Dest_t;

typedef enum {
  RLIM_TNNLOP_FLAG_ENABLED = (1<<0),
  RLIM_TNNLOP_FLAG_IP4_ROUTING = (1<<1),
  RLIM_TNNLOP_FLAG_IP6_ROUTING = (1<<2),
  RLIM_TNNLOP_FLAG_REACHABLE = (1<<3),
  RLIM_TNNLOP_FLAG_LOCALIP4UP = (1<<4),
  RLIM_TNNLOP_FLAG_LOCALIP6UP = (1<<5),
  RLIM_TNNLOP_FLAG_OSAPI_INTF_CREATED = (1<<6),
  RLIM_TNNLOP_FLAG_DTL_INTF_CREATED = (1<<7),
  RLIM_TNNLOP_FLAG_DISABLE_IN_PROGRESS = (1<<8),
  RLIM_TNNLOP_FLAG_DELETE_IN_PROGRESS = (1<<9),
  RLIM_TNNLOP_FLAG_HAS_DUPLICATE = (1<<10),
  RLIM_TNNLOP_FLAG_IS_DUPLICATE = (1<<11),
  RLIM_TNNLOP_FLAG_PORT_ENABLED = (1<<12),
  RLIM_TNNLOP_FLAG_DTL_ROUTE_CREATED = (1<<13),
} rlimTunnelOpFlag_t;

extern rlimOpData_t *rlimOpData;
extern rlimLoopbackOpData_t *rlimLoopbackOpData;
extern rlimTunnelOpData_t *rlimTunnelOpData;

extern osapiRWLock_t rlimRWLock;
extern void *rlimCreateDeleteMutex;
extern void *rlimNhopChangeEventMutex;
extern L7_uint32 rlimIntIfNumToTunnelId[];

extern L7_BOOL rlimDebug;
extern L7_uint32 rlimDebugTraceFlags;
extern avlTree_t         rlim6to4TunnelDestTree; 

/*
 * Debug macros
 */
#define RLIM_TRACE_LEN_MAX 255

#define RLIM_TRACE_EVENTS             0x2
#define RLIM_TRACE_6TO4_ROUTES        0x4
#define RLIM_TRACE_NEXT_HOP_CHANGES   0x8

#define RLIM_DEBUG_PRINT(cond, format, args...) \
        if (cond) { \
          L7_char8 * _filename = __FILE__; \
          l7utilsFilenameStrip(&_filename); \
          sysapiPrintf("\nRLIM [%s:%u]: " format, \
                       _filename, __LINE__ , ##args); \
        }

#define RLIM_SEMA_TAKE(a,b) { \
        RLIM_DEBUG_PRINT(rlimDebug, "sema take %p\n", (a)); \
        osapiSemaTake((a),(b)); \
        RLIM_DEBUG_PRINT(rlimDebug, "sema have %p\n", (a)); \
}

#define RLIM_SEMA_GIVE(a) { \
        RLIM_DEBUG_PRINT(rlimDebug, "sema give %p\n", (a)); \
        osapiSemaGive((a)); \
}

#define RLIM_WRITE_LOCK_TAKE(a,b) { \
        RLIM_DEBUG_PRINT(rlimDebug, "writelock take %p\n", (a)); \
        osapiWriteLockTake((a),(b)); \
        RLIM_DEBUG_PRINT(rlimDebug, "writelock have %p\n", (a)); \
}

#define RLIM_WRITE_LOCK_GIVE(a) { \
        RLIM_DEBUG_PRINT(rlimDebug, "writelock give %p\n", (a)); \
        osapiWriteLockGive((a)); \
}

#define RLIM_READ_LOCK_TAKE(a,b) { \
        RLIM_DEBUG_PRINT(rlimDebug, "readlock take %p\n", (a)); \
        osapiReadLockTake((a),(b)); \
        RLIM_DEBUG_PRINT(rlimDebug, "readlock have %p\n", (a)); \
}

#define RLIM_READ_LOCK_GIVE(a) { \
        RLIM_DEBUG_PRINT(rlimDebug, "readlock give %p\n", (a)); \
        osapiReadLockGive((a)); \
}

/*
 * Messaging data structures
 */
typedef enum
{
  RLIM_NIM_EVENT,
  RLIM_RESTORE_EVENT,
  RLIM_IP4_RTR_EVENT,
  RLIM_IP4_NHOP_CHANGE_EVENT,
  RLIM_CNFGR_EVENT,
  RLIM_TUNNEL_DEST_AGE_EVENT
} RIPMAP_MSG_ID_t;

typedef struct
{
  L7_uint32 intIfNum;
  L7_uint32 event;
  NIM_CORRELATOR_t correlator;
} rlimNimEventMsg_t;

typedef struct
{
  L7_uint32 intIfNum;
  L7_uint32 event;
  void *pData;
  L7_BOOL asyncResponseNeeded;
  ASYNC_EVENT_NOTIFY_INFO_t response;
} rlimIp4RtrEventMsg_t;

typedef struct
{
  L7_uint32 intIfNum;
  L7_uint32 releaserFlag;
} rlimTunnelReleaseMsg_t;

typedef struct
{
  L7_uint32 msgId;
  union
  {
    L7_CNFGR_CMD_DATA_t cmdData;
    rlimNimEventMsg_t nimEvent;
    rlimIp4RtrEventMsg_t ip4RtrEvent;
    rlimTunnelReleaseMsg_t tunnelRelease;
  } type;
} rlimMsg_t;

/*
 * Determine if a given tunnel mode allows configuration as
 * a IP4 or IP6 interface.
 */
#define RLIM_TUNNEL_CANBE_IP4_INTF(mode) \
          ((mode) == L7_TUNNEL_MODE_IP6)

#define RLIM_TUNNEL_CANBE_IP6_INTF(mode) \
          ((mode) == L7_TUNNEL_MODE_6OVER4 || \
           (mode) == L7_TUNNEL_MODE_6TO4 || \
           (mode) == L7_TUNNEL_MODE_ISATAP || \
           (mode) == L7_TUNNEL_MODE_IP6)

/*
 * Address validity checking macros
 */
#define RLIM_VALID_IP4_ADDR(addr) \
          ((addr) != 0 && (addr) != 0xFFFFFFFF && \
           (((addr) >> 24) & 0xff) != IN_LOOPBACKNET)

#define RLIM_VALID_IP6_ADDR(addr) \
          (!L7_IP6_IS_ADDR_UNSPECIFIED(addr) && \
           !L7_IP6_IS_ADDR_LOOPBACK(addr) && \
           !L7_IP6_IS_ADDR_LINK_LOCAL(addr) && \
           !L7_IP6_IS_ADDR_MULTICAST(addr))

/*
 * L7_sockaddr_union_t setting macros
 */
#define RLIM_IP4_ADDR_PUT(from, to) { \
          memset((to), 0, sizeof(*(to))); \
          (to)->u.sa4.sin_family = L7_AF_INET; \
          (to)->u.sa4.sin_len = sizeof((to)->u.sa4); \
          (to)->u.sa4.sin_addr.s_addr = osapiHtonl(from); \
}
#define RLIM_IP6_ADDR_PUT(from, to) { \
          memset((to), 0, sizeof(*(to))); \
          (to)->u.sa6.sin6_family = L7_AF_INET6; \
          (to)->u.sa6.sin6_len = sizeof((to)->u.sa6); \
          (to)->u.sa6.sin6_addr = *(from); \
}

typedef enum
{
  RLIM_RT_6TO4_ADD,
  RLIM_RT_6TO4_DEL,
  RLIM_RT_6TO4_MOD,
} RLIM_RT_6TO4_TYPE_t;

/*
 * Function prototypes
 */
void rlimDebugInit();

L7_RC_t rlimIntfCreateInNim(L7_uint32 intfType, L7_uint32 intfId,
                            L7_uint32 *pIntIfNum);
L7_RC_t rlimIntfDeleteInNim(L7_uint32 intIfNum);
void rlimNimEventCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal);
void rlimNimEventSend(L7_uint32 intIfNum, L7_uint32 event);
void rlimDebugTraceWrite(L7_char8 *traceMsg);
void rlimIntfNameFromTypeIdGet(L7_uint32 intfType, L7_uint32 intfId,
                               L7_uchar8 *pIntName);
void rlimIntfLogMsg(L7_uchar8 *fmtString, L7_uint32 intIfNum);
L7_RC_t rlimIntfTypeIdGet(L7_uint32 intIfNum, L7_uint32 *pIfType,
                          L7_uint32 *pIfId);
L7_RC_t rlimLoopbackIdGetNoLock(L7_uint32 intIfNum, L7_uint32 *pLoopbackId);
L7_RC_t rlimTunnelIdGetNoLock(L7_uint32 intIfNum, L7_uint32 *pTunnelId);
L7_BOOL rlimAddrIsEqual(rlimAddr_t *pAddr1, rlimAddr_t *pAddr2);

void rlimLoopbackOpDataInit(rlimLoopbackOpData_t *lop);
L7_RC_t rlimLoopbackOpCreate(L7_uint32 loopbackId);
void rlimLoopbackOpDelete(L7_uint32 intIfNum, L7_uint32 loopbackId);
void rlimLoopbackOpDeleteWait(L7_uint32 loopbackId);
void rlimLoopbackNimDeleteDone(NIM_NOTIFY_CB_INFO_t retVal);
void rlimTunnelOpDataInit(rlimTunnelOpData_t *top);
L7_RC_t rlimTunnelOpCreate(L7_uint32 tunnelId);
void rlimTunnelOpDelete(L7_uint32 intIfNum, L7_uint32 tunnelId);
void rlimTunnelOpDeleteWait(L7_uint32 tunnelId);
void rlimTunnelNimDeleteDone(NIM_NOTIFY_CB_INFO_t retVal);
void rlimTunnelOpUpdate(L7_uint32 tunnelId);
L7_BOOL rlimTunnelCanEnable(L7_uint32 tunnelId);
L7_RC_t rlimTunnelEnable(L7_uint32 tunnelId);
void rlimTunnelDisable(L7_uint32 tunnelId);
void rlimTunnelOpLocalAddrUpdate(L7_uint32 tunnelId);
void rlimTunnelOpRemoteAddrUpdate(L7_uint32 tunnelId, rlimAddr_t *newAddr);
void rlimTunnelOpLocalAddrTypeGet(L7_uint32 tunnelId, L7_uint32 *pAddrType);
L7_RC_t rlimTunnelOpLocalIp4AddrGet(L7_uint32 tunnelId, L7_uint32 *addr);
L7_RC_t rlimTunnelOpLocalIp6AddrGet(L7_uint32 tunnelId, L7_in6_addr_t *addr);
L7_RC_t rlimTunnelOpLocalIntfGet(L7_uint32 tunnelId, L7_uint32 *pIntIfNum);

L7_RC_t rlimNimIntfChangeCallBack(L7_uint32 intIfNum, L7_uint32 event,
                                  NIM_CORRELATOR_t correlator);
L7_RC_t rlimIp4RoutingEventChangeCallBack(L7_uint32 intIfNum,
                                          L7_uint32 event,
                                          void *pData,
                                          ASYNC_EVENT_NOTIFY_INFO_t *pResponse);
L7_RC_t rlimThreadCreate(void);
void rlimCnfgrTask(void);
void rlimProcTask(void);
L7_RC_t rlimNimEventProcess(L7_uint32 intIfNum, L7_uint32 event,
                            NIM_CORRELATOR_t correlator);
L7_RC_t rlimIp4RoutingEventProcess(L7_uint32 intIfNum, L7_uint32 event,
                                   void *pData,
                                   ASYNC_EVENT_NOTIFY_INFO_t *response);
void rlimTunnelIp4IntfUpProcess(L7_uint32 eventIntIfNum);
void rlimTunnelIp4IntfDownProcess(L7_uint32 eventIntIfNum);
void rlimTunnelIp4SecondaryAddrAddProcess(L7_uint32 eventIntIfNum);
void rlimTunnelIp4SecondaryAddrDeleteProcess(L7_uint32 eventIntIfNum);
void rlimTunnelIntfDeleteProcess(L7_uint32 eventIntIfNum);
void rlimTunnelForceIp4Up(L7_uint32 tunnelId, L7_uint32 localIp4Addr,
                          L7_uint32 localIntIfNum);
void rlimTunnelForceIp4Down(L7_uint32 tunnelId);

void rlimTunnelIp4NhopChangeCallback(L7_uint32 addr, void *tid);
void rlimTunnelIp4NhopChangeTimerCallback(void);
void rlimTunnelIp4NhopChangeEventProcess(void);
void rlimTunnelIp4NhopChangeProcess(void);
void rlimTunnel6to4NhopChangeProcess(void);
void rlimTunnelIp4ReachabilityCheck(L7_uint32 tunnelId);
L7_BOOL rlimTunnelIsDuplicate(L7_uint32 tunnelId);
void rlimTunnelDuplicateCheck(void);

L7_RC_t rlimTunnelAddrsGet(L7_uint32 tunnelId, L7_sockaddr_union_t *pSrcAddr,
                           L7_sockaddr_union_t *pDstAddr);
L7_RC_t rlimTunnelNextHopGet(L7_uint32 tunnelId,
                             L7_sockaddr_union_t *pNextHopAddr,
                             L7_uint32 *pNextHopIntIfnum);

void rlimLoopbackPortRoutingDisableProcess(L7_uint32 intIfNum);
void rlimTunnelPortRoutingDisableProcess(L7_uint32 intIfNum);

L7_RC_t rlim6to4TunnelDestListCreate(void);
L7_RC_t rlim6to4TunnelDestListDestroy(void);
L7_RC_t rlim6to4TunnelDestListAdd(L7_uint32 destAddr, L7_uint32 tunnelId);
L7_RC_t rlim6to4TunnelDestListDelete(rlim6to4Dest_t *tdest);
rlim6to4Dest_t *rlim6to4TunnelDestListFind(L7_uint32 destAddr, L7_uint32 tunnelId);
void rlimTunnel6to4ReachabilityCheck(L7_uint32 tunnelId);
void rlim6to4TunnelDestPurge(L7_uint32 tunnelId);
void rlim6to4TunnelDestAgeEventProcess(void);
void rlimTunnelDestAgeTimerCallback(void);
L7_RC_t  rlim6to4TunnelNetworkSet(L7_uint32 intIfNum, rlim6to4Dest_t *tdest, RLIM_RT_6TO4_TYPE_t rtype );

#endif /* !defined(INCLUDE_RLIM_H) */
