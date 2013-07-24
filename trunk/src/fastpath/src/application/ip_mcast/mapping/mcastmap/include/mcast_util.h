/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mcast_util.h
*
* @purpose    MCAST Mapping layer internal function prototypes
*
* @component  MCAST Mapping Layer
*
* @comments   none
*
* @create     05/17/2002
*
* @author     Vemula & Srusti
*
* @end
**********************************************************************/

/*************************************************************
                    
*************************************************************/
#ifndef _MCAST_UTIL_H_
#define _MCAST_UTIL_H_

#include "l7_common.h"
#include "async_event_api.h"
#include "mcast_config.h"
#include "mcast_cnfgr.h"
#include "l3_mcast_commdefs.h"
#include "mfc_api.h"
#include "pktrcvr.h"
#include "l7apptimer_api.h"
#include "mcast_debug_api.h"
#include "l7_mcast_api.h"

#define MCAST_MASK_STRING_LENGTH  (COMPONENT_INDICES*3)

typedef enum
{
  MCASTMAP_APP_TIMER_Q = 0,
  MCASTMAP_EVENT_Q,
  MCASTMAP_CTRL_PKT_Q,
  MCASTMAP_MAX_Q
}MCASTMAP_QUEUE_ID_t;

/* this enum defines number of control blocks supported for each of the
   address family supported. This is defined even though MCASTMAP currently
   doesnot support control blocks per family as control block enhancement for 
   mcastmap is on cards */
typedef enum
{
  MCAST_MAP_IPV4_CB = 0,
  MCAST_MAP_IPV6_CB,
  MCAST_MAP_CB_MAX
}MCAST_MAP_CB_TYPE_t;

#define MCASTMAP_MGMD_EVENTS_COUNT_LIMIT \
        (((L7_RTR_MAX_RTR_INTERFACES + (L7_MGMD_GROUPS_MAX_ENTRIES/2)) * 60)/100)

typedef struct mcastMapInfo_s
{
  L7_BOOL	mcastV4Initialized;
  L7_BOOL	mcastV6Initialized;
  ASYNC_EVENT_HANDLER_t   eventHandlerId;
  asyncEventHandler_t     eventHandler;
  void                   *eventCompleteSignal;
  COMPONENT_MASK_t        registeredComponents;
} mcastMapInfo_t;

typedef struct mcastMapQueue_s
{
  void        *QPointer;
  L7_uchar8   QName[32];
  L7_uint32   QSize;
  L7_uint32   QCount;
  L7_uint32   QCurrentMsgCnt;
  L7_uint32   QSendFailedCnt;
  L7_uint32   QMaxRx;
  L7_uint32   QRxSuccess;
  void        *QRecvBuffer;

}mcastMapQueue_t;


typedef struct mcastGblVar_s
{
  mcastMapInfo_t        mcastMapInfo;
  mcastCnfgrState_t     mcastCnfgrState;
  L7_BOOL               warmRestart;            /* L7_TRUE if last reboot was a warm restart */
  L7_BOOL               warmRestartInProgress;  /* L7_TRUE during warm restart interval */
  L7_uint32            *pMcastMapCfgMapTbl;
  L7_mcastMapCfg_t      mcastMapCfgData;     /* Configuration for IPv4 MCAST */
  L7_mcastMapCfg_t      *mcastMapV6CfgData;  /* Configuration for IPv6 MCAST */
  L7_uint32             mcastV4CtrlPktPoolId;
  L7_uint32             mcastV6CtrlPktPoolId;
  L7_uint32             mcastV6DataPktPoolId;
  L7_uint32             mcastV4HeapId;
  L7_uint32             mcastV6HeapId;
  L7_uint32             mcastAppTimerBufPoolId;
  void                  *mcastHandleListMemHndl;
  void                  *mcastV6HandleListMemHndl;
  L7_uint32             mcastV4MgmdEventsPoolId;
  L7_uint32             mcastV6MgmdEventsPoolId;
  L7_int32              mcastMapTaskId;
  L7_int32              pktRcvrTaskId;
  L7_PktRcvr_t*         pktRcvr;
 /* Three message queues are created to prioritize the timer events and control msg events ahead of
    data packet reception events so that the operational control events are still
    handled irrespective of packets flowing at high data rate */

   mcastMapQueue_t            mcastMapQueue[MCASTMAP_MAX_Q];


  void                 *mcastMapMsgQSema;
  L7_RC_t (*notifyMcastModeChange[L7_MRP_MAXIMUM])(L7_uint32 event,
                                void *pdata, ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

  void                 *mcastMapAdminScopeSema;
  L7_RC_t              (*notifyAdminScopeChange [L7_MRP_MAXIMUM])
                       (L7_uint32 event_type, 
                        L7_uint32 msg_len, void *adminInfo);

  mcastMapStaticMRouteNotifyFn_t mcastV4StaticMRouteNotifyFn[L7_MRP_MAXIMUM];
  mcastMapStaticMRouteNotifyFn_t mcastV6StaticMRouteNotifyFn[L7_MRP_MAXIMUM];
} mcastGblVar_t;

extern mcastGblVar_t mcastGblVariables_g;


/*  MCAST Debug Trace Info */

#define MCAST_DEBUG_CFG_FILENAME      "mcast_debug.cfg"
#define MCAST_DEBUG_CFG_VER_1          0x1
#define MCAST_DEBUG_CFG_VER_CURRENT    MCAST_DEBUG_CFG_VER_1


#define MCAST_DEBUG_TRACE_FLAG_BYTES     ((MCAST_DEBUG_LAST_TRACE + 7) / 8)
#define MCAST_DEBUG_TRACE_FLAG_BITS_MAX      8
#define MCAST_DEBUG_TRACE_FLAG_VALUE         1

typedef  L7_uchar8 mcastDebugTraceFlags_t[MCAST_DEBUG_TRACE_FLAG_BYTES];

typedef struct mcastDebugCfgData_s 
{
  mcastDebugTraceFlags_t mcastDebugTraceFlag[MCAST_MAP_CB_MAX];
} mcastDebugCfgData_t;

typedef struct mcastDebugCfg_s
{
  L7_fileHdr_t          hdr;
  mcastDebugCfgData_t    cfg;
  L7_uint32             checkSum;
} mcastDebugCfg_t;

/********************************************************************
*                     Function Prototypes
*********************************************************************/
extern L7_RC_t mcastMapMcastAdminModeEnable(L7_uchar8 family);

extern L7_RC_t mcastMapMcastAdminModeDisable(L7_uchar8 family);

extern L7_RC_t mcastMapMcastAdminModeEnableAndNotify(void);

extern L7_RC_t mcastMapMcastAdminModeDisableAndNotify(void);

extern L7_RC_t mcastMapIntfTtlThreshApply(L7_uint32 intIfIndex, L7_uint32 ttl);

extern L7_BOOL mcastMapIntfIsConfigurable(L7_uint32 intIfNum,
                                   L7_mcastMapIfCfgData_t **pCfg);

extern L7_BOOL mcastMapIntfConfigEntryGet(L7_uint32 intIfNum, 
                                   L7_mcastMapIfCfgData_t **pCfg);

extern L7_RC_t mcastMapAdminScopeBoundaryFlush(void);

extern L7_RC_t mcastMapAdminScopeBoundaryAddApply(L7_uint32 intIfNum,
                                       L7_inet_addr_t *ipGroup,
                                       L7_inet_addr_t *ipMask);

extern L7_RC_t mcastMapAdminScopeBoundaryDeleteApply(L7_uint32 intIfNum,
                                          L7_inet_addr_t *ipGroup,
                                          L7_inet_addr_t *ipMask);
extern void mcastMapAdminscopeBoundaryInterfaceModeApply(L7_uint32 intIfNum,
                                                L7_uint32 mode);

extern void mcastMapAsyncEventCompleteNotify(ASYNC_EVENT_COMPLETE_INFO_t *pComplete, 
                                     COMPONENT_MASK_t *pRemainingMask,
                                     COMPONENT_MASK_t *pFailingMask);

extern L7_RC_t mcastMapMessageSend(MCASTMAP_QUEUE_ID_t QIndex,void *msg);

extern L7_RC_t mcastMapQDelete(void);

extern L7_RC_t
mcastMapCfgDataGet (L7_uchar8 addrFamily,
                    L7_mcastMapCfg_t **cfgData);

L7_RC_t
mcastMapStaticMRouteLookup (L7_uchar8 addrFamily,
                            L7_inet_addr_t *srcAddr,
                            L7_inet_addr_t *srcMask,
                            L7_inet_addr_t *rpfAddr,
                            L7_uint32 *intIfNum,
                            L7_uint32 *preference);

L7_RC_t
mcastMapStaticMRouteBestRouteLookup (L7_uchar8 addrFamily,
                                     L7_inet_addr_t *srcAddr,
                                     mcastRPFInfo_t *bestRPFRoute);

L7_RC_t
mcastMapStaticMRouteRegisteredUsersNotify (L7_uchar8 addrFamily,
                                           L7_mcastMapStaticRtsCfgData_t *staticMRouteEntry,
                                           L7_uint32 mRouteStatus);

extern L7_RC_t mcastMapAdminModeEventChangeNotify(L7_uint32 event, void *pData);

#endif /* _MCAST_UTIL_H_ */
