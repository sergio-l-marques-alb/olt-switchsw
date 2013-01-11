/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   rip_map_util.h
*
* @purpose    RIP Mapping layer internal function prototypes
*
* @component  RIP Mapping Layer
*
* @comments   none
*
* @create     05/21/2001
*
* @author     gpaussa
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef _RIP_MAP_UTIL_H_
#define _RIP_MAP_UTIL_H_

                                     
#include  "l7_common.h"
#include  "l7_cnfgr_api.h"
#include  "sysnet_api.h"
#include  "async_event_api.h"
#include  "nimapi.h"
#include  "rip_config.h"
#include  "rto_api.h"
#include  "acl_api.h"

                          
#define RIP_MAP_RTR_INTF_FIRST  1       /* index number of first routing intf */
#define RIP_MAP_IPADDR_STR_LEN ((4*3) + 3 + 1)   /* "xxx.xxx.xxx.xxx\0" */

#define L7_RIP_METRIC_INVALID                  0xffffffff
#define L7_RIP_ROUTE_REDIST_FILTER_INVALID     0xffffffff
#define L7_RIP_ROUTE_REDIST_MATCHTYPE_INVALID  0
#define L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG 1           /* If default metric is not
                                                              confgiured, a metric of 1
                                                              should be used to advertise
                                                              the default route.
                                                           */

#define RIPMAP_DEBUG_CFG_FILENAME "ripmap_debug.cfg"
#define RIPMAP_DEBUG_CFG_VER_1          0x1
#define RIPMAP_DEBUG_CFG_VER_CURRENT    RIPMAP_DEBUG_CFG_VER_1


/*-------------------------------------*/
/*  RIP MAP INFO STRUCTURES            */
/*-------------------------------------*/

typedef struct ripInfo_s     
{
  L7_BOOL         ripInitialized;           /* indicates vendor RIP stack init'd */
  L7_uint32       ripTaskId;                /* RIP vendor code task ID */
  L7_uint32       ripMapProcessingTaskId;   /* RIP mapping layer task ID */

  /* The RIP processing thread reads from two message queues. There is a separate
   * queue for redistribution events so that we can give priority to other events,
   * such as interface, router, and configuration events. */
  void           *ripMapProcessQueue;       /* main RIP mapping layer processing queue */
  void           *ripMapRedistQueue;        /* Queue for redistribution events */

  /* Counting semaphore. Semaphore is given every time a message is added to the
   * ripMapProcessQueue or ripMapRedistQueue. The processing task waits on this 
   * semaphore before reading from the queues. */
  void           *msgQSema;

  L7_uint32       ripAdminMode;             /* indicates applied admin mode */
  L7_BOOL         ripVendIfWaitForCompletionSignal; /* indicates completion signal wait occurred */
  asyncEventSignalDescr_t   ripVendIfCompletionSignal; /* Completion signal for 
                                                         interface-related events.
                                                         For the RIP vendor code, interface-related
                                                         events include both IP address and
                                                         router-interface mode events */
  L7_uint32       ripDefRtAdv;          /* indicates applied def rt adv mode */
  L7_uint32       ripRtRedistMode[REDIST_RT_LAST];   /* indicates applied redist mode */

  /* Timer that tells RIP to request best route changes from RTO. */
  osapiTimerDescr_t *bestRouteTimer;

  /* L7_TRUE if RIP is registered with RTO for best route changes. */
  L7_BOOL registeredWithRto;

} ripInfo_t;


/* RIP interface state */
typedef enum
{
  RIP_MAP_INTF_STATE_NONE = 0,          /* not created */
  RIP_MAP_INTF_STATE_DOWN,              /* not operational from RIP perspective */
  RIP_MAP_INTF_STATE_UP                 /* usable for RIP */
} ripMapIntfState_t;


typedef struct ripIntfInfo_s     
{
  /* internal interface number of the interface whose state is described
   * by this structure.
   */
  L7_uint32       intIfNum;

  /* RIP interface state. Used to tell the protocol engine which interfaces
   * are up.
   */
  ripMapIntfState_t state;

} ripIntfInfo_t;

extern ripInfo_t          *pRipInfo;
extern ripIntfInfo_t      *pRipIntfInfo;




/*--------------------------------------*/
/*  RIP MAP MESSAGES                    */
/*--------------------------------------*/

/*  IP Map Message IDs */
typedef enum {
    RIPMAP_RTR_EVENT,
    RIPMAP_IP_ADDR_EVENT,               /* WPJ_TBD */
    RIPMAP_ADMIN_MODE_EVENT,   
    RIPMAP_INTF_MODE_EVENT,    
    RIPMAP_RESTORE_EVENT,                /* Restore configuration */
    RIPMAP_CNFGR,
    RIPMAP_RTO_EVENT,
    RIPMAP_LAST_MSG
 }RIPMAP_MSG_ID_t;
      


typedef struct ripMapRtrEventMsg_s
{
    L7_uint32                   intIfNum;       
    L7_uint32                   event;          /* one of L7_RTR_EVENT_CHANGE_t */
    void                        *pData;         /* event-specific data */
    L7_BOOL                     asyncResponseNeeded;
    ASYNC_EVENT_NOTIFY_INFO_t   response;     /* Asynchronous response info */
} ripMapRtrEventMsg_t;


typedef struct ripMapAdminModeEventMsg_s
{
    L7_uint32           intIfNum;       
    L7_uint32           mode;          /* L7_ENABLE or L7_DISABLE */
} ripMapAdminModeEventMsg_t;

 
typedef struct ripMapMsg_s
{
    L7_uint32       msgId;            
    union 
    {
      ripMapRtrEventMsg_t       rtrEvent;
      ripMapAdminModeEventMsg_t modeEvent;
      L7_CNFGR_CMD_DATA_t   cnfgrData;
    } type;
} ripMapMsg_t;

typedef struct ripMapDebugCfgData_s 
{
  L7_BOOL ripMapDebugPacketTraceFlag;
} ripMapDebugCfgData_t;

typedef struct ripMapDebugCfg_s
{
  L7_fileHdr_t          hdr;
  ripMapDebugCfgData_t  cfg;
  L7_uint32             checkSum;
} ripMapDebugCfg_t;

#define  RIP_MAP_MSG_SIZE  (sizeof(ripMapMsg_t))


      

/*-------------------------------------------------*/
/*              FUNCTION PROTOTYPES                */
/*-------------------------------------------------*/

/* rip_map.c */

L7_RC_t ripMapStartTasks(void);
L7_RC_t ripMapFiniTasks(void);
L7_RC_t ripMapAppsInit(void);
L7_RC_t ripMapSave(void);
void ripMapProcessingTask(void);
L7_BOOL ripMapHasDataChanged(void);
void ripMapResetDataChanged(void);
void    ripMapBuildDefaultConfigData(L7_uint32 ver);
void ripMapIntfBuildDefaultConfigData(nimConfigID_t *configId, ripMapCfgIntf_t *pCfg);
L7_RC_t ripMapApplyConfigData(void);
L7_RC_t ripMapRouterIntfConfigure(L7_uint32 intIfNum);
L7_RC_t ripMapIntfCreate(L7_uint32 intIfNum); 
L7_RC_t ripMapIntfDelete(L7_uint32 intIfNum); 
L7_RC_t ripMapAdminModeQueue(L7_uint32 mode);
L7_RC_t ripMapIntfAdminModeQueue( L7_uint32 intIfNum, L7_uint32 mode);
void    ripMapIntfAdminModeProcess(L7_uint32 intIfNum, L7_uint32 mode);
L7_RC_t ripMapIntfAdminModeEnable(L7_uint32 intIfNum);
L7_RC_t ripMapIntfAdminModeDisable(L7_uint32 intIfNum);
void    ripMapAdminModeProcess(L7_uint32 mode);
L7_RC_t ripMapRoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                          L7_uint32 event,
                                          void *pData, 
                                          ASYNC_EVENT_NOTIFY_INFO_t *pResponse);
L7_RC_t ripMapRoutingEventChangeProcess (L7_uint32 intIfNum, 
                                          L7_uint32 event,
                                          void *pData, 
                                          ASYNC_EVENT_NOTIFY_INFO_t *pResponse);
void    ripMapRoutingIfEventCompletion (ASYNC_RESPONSE_t *pResponse, L7_char8 *eventData);
void    ripMapRestoreProcess(void);    
void ripMapAclCallback(L7_uint32 aclNum, aclRouteFilter_t event);
L7_RC_t ripMapIpAddressAdd(L7_uint32 intIfNum);
L7_RC_t ripMapIpAddressChangeCallBack(L7_uint32 intIfNum);
L7_RC_t ripMapIpAddressDeleteCallBack(L7_uint32 intIfNum);
void ripMapBestRouteChangeCallback(void);
void ripMapBestRouteTimerCb(void);
L7_RC_t ripMapBestRoutesGet(void);
L7_RC_t ripBestRouteChange(L7_routeEntry_t *routeEntry, 
                           RTO_ROUTE_EVENT_t route_status);
void    ripMapIpAddrToStr(L7_uint32 ipAddr, L7_uchar8 *pBuf);
void    ripMapMsgLvlSet(L7_uint32 msgLvl);
SYSNET_PDU_RC_t ripMapFrameFilter(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc);


/* rip_map_util.c */
void    ripMapSemaTake(void);
void    ripMapSemaGive(void);
L7_RC_t ripMapAdminModeEnable(void);
L7_RC_t ripMapAdminModeDisable(void);
L7_RC_t ripMapSplitHorizonModeApply(L7_uint32 val);
L7_RC_t ripMapAutoSummarizationModeApply(L7_uint32 val);
L7_RC_t ripMapHostRoutesAcceptModeApply(L7_uint32 val);
L7_RC_t ripMapDefaultMetricApply(L7_uint32 val);
L7_RC_t ripMapDefaultRouteAdvertiseModeApply(L7_uint32 val);
L7_RC_t ripMapDefaultRouteAdvertiseModeEnable();
L7_RC_t ripMapDefaultRouteAdvertiseModeDisable();
L7_RC_t ripMapRouteRedistributeFilterApply(L7_REDIST_RT_INDICES_t protocol, L7_uint32 val);
L7_RC_t ripMapRouteRedistributeParmsApply(L7_REDIST_RT_INDICES_t protocol, L7_uint32 metric, L7_OSPF_METRIC_TYPES_t matchType);
L7_RC_t ripMapRouteRedistributeMetricApply(L7_REDIST_RT_INDICES_t protocol, L7_uint32 metric);
L7_RC_t ripMapRouteRedistributeMatchTypeApply(L7_REDIST_RT_INDICES_t protocol, L7_OSPF_METRIC_TYPES_t matchType);
L7_RC_t ripMapRouteRedistributeModeApply(L7_REDIST_RT_INDICES_t protocol, L7_uint32 mode);
L7_RC_t ripMapIntfAdminModeApply(L7_uint32 intIfNum, L7_uint32 mode);
L7_RC_t ripMapIntfAuthTypeApply(L7_uint32 intIfNum, L7_uint32 authType);
L7_RC_t ripMapIntfAuthKeyApply(L7_uint32 intIfNum, L7_uchar8 *key);
L7_RC_t ripMapIntfAuthKeyIdApply(L7_uint32 intIfNum, L7_uint32 authKeyId);
L7_RC_t ripMapIntfVerSendApply(L7_uint32 intIfNum, L7_uint32 version);
L7_RC_t ripMapIntfVerRecvApply(L7_uint32 intIfNum, L7_uint32 version);
L7_BOOL ripMapIsInitialized(void);
L7_BOOL ripMapIntfIsConfigurable(L7_uint32 intIfNum, ripMapCfgIntf_t **pCfg);
L7_BOOL ripMapIntfIsActive(L7_uint32 intIfNum);
L7_BOOL ripMapRtrIntfIsOperable(L7_uint32 rtrIfNum, L7_uint32 *pIntIfNum);
L7_BOOL ripMapProtocolMayParticipate(void);
L7_BOOL ripMapIntfMayParticipate(L7_uint32 intIfNum);
L7_BOOL ripMapIntfIsConfigurable(L7_uint32 intIfNum, ripMapCfgIntf_t **pCfg);
L7_BOOL ripMapIntfConfigEntryGet(L7_uint32 intIfNum, ripMapCfgIntf_t **pCfg);
void ripIntfStateSet(L7_uint32 intIfNum, ripMapIntfState_t state);
L7_RC_t ripRtoRegisterUpdate(void);
L7_BOOL ripShouldListenToBestRoutes(void);
L7_BOOL ripRedistributes(void);
L7_BOOL ripIsRegisteredWithRto(void);

/* rip_map_api.c - internal versions of external API that assume semaphore is locked */
L7_RC_t ripMapAdminModeGet(L7_uint32 *mode);
L7_RC_t ripMapDefaultMetricGet(L7_uint32 *val);
L7_RC_t ripMapRouteRedistributeFilterGet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 *val);
L7_RC_t ripMapRouteRedistributeMatchTypeGet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 *matchType);
L7_RC_t ripMapRouteRedistributeMetricGet(L7_uint32 protocol, L7_uint32 *metric);
L7_RC_t ripMapHostRoutesAcceptModeGet(L7_uint32 *val);
L7_RC_t ripMapDefaultRouteAdvertiseModeGet(L7_uint32 *val);


/* WPJ_TBD: Create a debug file for this */

/* rip_map_outcalls.c */
L7_BOOL ripMapIsAclPresent(void);
L7_RC_t ripMapAclCallbackRegister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr, L7_uchar8 *funcNameStr);
L7_RC_t ripMapAclCallbackUnregister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr);
L7_BOOL ripMapAclCallbackIsRegistered(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr);
L7_RC_t ripMapAclRouteFilter(L7_uint32 aclnum,
                              L7_uint32 routePrefix,
                              L7_uint32 routeMask);
void ripMapDebugShow(void);
void ripMapCfgDataShow(void);

L7_RC_t ripMapDebugSave(void);
L7_RC_t ripMapDebugRestore(void);
L7_BOOL ripMapDebugHasDataChanged(void);
void ripMapDebugBuildDefaultConfigData(L7_uint32 ver);
L7_RC_t ripMapApplyDebugConfigData(void);


#endif /* _RIP_MAP_UTIL_H_ */

