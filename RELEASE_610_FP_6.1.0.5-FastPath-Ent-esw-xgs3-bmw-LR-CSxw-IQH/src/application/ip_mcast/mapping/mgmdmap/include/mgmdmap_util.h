/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   mgmd_util.h
*
* @purpose    MGMD Mapping layer internal function prototypes
*
* @component  MGMD Mapping Layer
*
* @comments   none
*
* @create     02/06/2002
*
* @author     ramakrishna
* @end
*
**********************************************************************/

#ifndef _INCLUDE_MGMD_MAP_UTIL_H
#define _INCLUDE_MGMD_MAP_UTIL_H
#include "mfc_api.h"
#include "l7_mgmd_api.h"
#include "l7apptimer_api.h"
#include "mgmd_debug_api.h"

/* Enumeration for the Maximum number of Control Blocks that
 * MGMD supports.
 */
typedef enum
{
  MGMD_MAP_IPV4_CB = 0,
  MGMD_MAP_IPV6_CB,
  MGMD_MAP_CB_MAX
}MGMD_MAP_CB_TYPE_t;

typedef enum
{
  MGMD_APP_TIMER_Q = 0,
  MGMD_EVENT_Q,
  MGMD_CTRL_PKT_Q, 
  MGMD_MAX_Q
}MGMD_QUEUE_ID_t;


                          
/*--------------------------------------*/
/*  MGMD MAP INFO STRUCTURES            */
/*--------------------------------------*/

typedef struct membershipEventsNotifyList_s
{
  L7_RC_t (*notifyMembershipChange)(L7_uchar8 family, 
                                      mcastEventTypes_t event,
                                      L7_uint32 msgLen, void *pMsg);
}mgmdMapEventsNotifyList_t;

typedef struct mgmdIntfInfo_s
{
    L7_BOOL           mgmdInitialized;
    AcquiredMask      acquiredList; /* Mask of components "acquiring"
                                       an interface */
} mgmdIntfInfo_t;


typedef struct mgmdInfo_s
{
    L7_BOOL           initialized;
    mgmdIntfInfo_t    mgmdIntfInfo[MAX_INTERFACES];
    COMPONENT_MASK_t  registeredComponents;
    mgmdMapEventsNotifyList_t  mgmdGrpInfoNotifyList[L7_MRP_MAXIMUM]; /* Callback fn */
    L7_BOOL           mgmdProxyEnabled;
} mgmdInfo_t;

typedef struct mgmdEventData_s
{
  L7_uint32           event;
  L7_uint32           intIfNum;
} mgmdEventData_t;

typedef struct mgmdQueue_s
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
  
}mgmdQueue_t;


typedef struct mgmdMapGblVars_s 
{
  /* MGMD processing thread reads from three message queues. There is a separate
     queue for packet events and seperate thread for timer events and seperate 
     thread for rest other events */
  mgmdQueue_t     mgmdQueue[MGMD_MAX_Q];

  /* Counting semaphore. Semaphore is given every time a message is added to the
     mgmdMapMsgQueue or mgmdMapPktQueue or Apptimer Queue. The processing task waits
     on this semaphore before reading from the queues. */
  void            *mgmdMapMsgQSema;
  L7_int32         mgmdMapTaskId;
  mgmdCnfgrState_t mgmdCnfgrState;
} mgmdMapGblVars_t;




typedef struct mgmdMapCB_s 
{
  mgmdInfo_t       mgmdInfo;
  L7_uint32        mgmdMapCfgMapTbl[L7_MAX_INTERFACE_COUNT];
  L7_mgmdMapCfg_t  mgmdMapCfgData;
  
  mgmdInfo_t       *pMgmdInfo;
  L7_uint32        *pMgmdMapCfgMapTbl;
  L7_mgmdMapCfg_t  *pMgmdMapCfgData;
  L7_uchar8         familyType;
  MCAST_CB_HNDL_t   cbHandle;
  MCAST_CB_HNDL_t   proxyCbHandle;
  L7_uint32         mgmdHeapId;
  mgmdMapGblVars_t *gblVars;
} mgmdMapCB_t;

extern mgmdMapGblVars_t mgmdMapGblVariables_g;
extern mgmdMapCB_t     *mgmdMapCB_g;

/*  MGMD Debug Trace Info */

#define MGMD_DEBUG_CFG_FILENAME      "mgmd_debug.cfg"
#define MGMD_DEBUG_CFG_VER_1          0x1
#define MGMD_DEBUG_CFG_VER_CURRENT    MGMD_DEBUG_CFG_VER_1


#define MGMD_DEBUG_TRACE_FLAG_BYTES     ((MGMD_DEBUG_LAST_TRACE + 7) / 8)
#define MGMD_DEBUG_TRACE_FLAG_BITS_MAX      8
#define MGMD_DEBUG_TRACE_FLAG_VALUE         1

typedef  L7_uchar8 mgmdDebugTraceFlags_t[MGMD_DEBUG_TRACE_FLAG_BYTES];

typedef struct mgmdDebugCfgData_s 
{
  mgmdDebugTraceFlags_t mgmdDebugTraceFlag[MGMD_MAP_CB_MAX];
} mgmdDebugCfgData_t;

typedef struct mgmdDebugCfg_s
{
  L7_fileHdr_t          hdr;
  mgmdDebugCfgData_t    cfg;
  L7_uint32             checkSum;
} mgmdDebugCfg_t;

/*********************************************************************
* @purpose  Enable the Mgmd on the router
*
* @param    mgmdMapCbPtr   @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMgmdAdminModeEnable(mgmdMapCB_t *mgmdMapCB, L7_BOOL doInit);

/*********************************************************************
* @purpose  Disable the Mgmd on router
*
* @param    mgmdMapCB   @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMgmdAdminModeDisable(mgmdMapCB_t *mgmdMapCB, L7_BOOL doInit);

/*********************************************************************
* @purpose  Enables the MGMD for the specified interface
*
* @param    mgmdMapCbPtr   @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    intfType       @b{(input)} interface type (L7_MGMD_INTF_MODE_t)
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfModeEnable(mgmdMapCB_t *mgmdMapCB, L7_uint32 intIfNum,
                              L7_MGMD_INTF_MODE_t intfType);

/*********************************************************************
* @purpose  Disables the MGMD for the specified interface
*
* @param    mgmdMapCB   @b{(input)} Mapping Control Block.
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    intfType       @b{(input)} interface type (L7_MGMD_INTF_MODE_t)
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfModeDisable(mgmdMapCB_t *mgmdMapCB, L7_uint32 intIfNum,
                               L7_MGMD_INTF_MODE_t intfType);

/*********************************************************************
* @purpose  Sets the MGMD Proxy version for the specified interface
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    node            @b{(input)} Router-alert check optional 
*                                       or mandatory
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMgmdRouterAlertCheckApply(mgmdMapCB_t *mgmdMapCbPtr,
                                         L7_BOOL mode);

/*********************************************************************
* @purpose  Updates Proxy MRT table for interface down event
*
* @param    mgmdMapCB   @b{(input)} Mapping Control Block.
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMgmdProxyIntfDownMRTApply(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Sets the MGMD Proxy version for the specified interface
*
* @param    mgmdMapCbPtr   @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    version        @b{(input)} MGMD version
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfVersionApply(mgmdMapCB_t *mgmdMapCbPtr, 
                        L7_uint32 intIfNum, L7_uint32 version);
/*********************************************************************
* @purpose  Updates vendor with configured robustness.
*
* @param    mgmdMapCB   @b{(input)} Mapping Control Block.
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    robustness  @b{(input)} configured robustness
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapIntfRobustnessApply(mgmdMapCB_t *mgmdMapCbPtr,
                                    L7_uint32 intIfNum,
                                    L7_uint32 robustness);
/*********************************************************************
* @purpose  Updates vendor with configured queryInterval
*
* @param    mgmdMapCB      @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    queryInterval  @b{(input)} configured query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapIntfQueryIntervalApply(mgmdMapCB_t *mgmdMapCbPtr,
                                       L7_uint32 intIfNum,
                                       L7_uint32 queryInterval);
/*********************************************************************
* @purpose  Updates vendor with configured query Maximum response time
*
* @param    mgmdMapCB         @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    queryMaxRespTime  @b{(input)} configured query Maximum
*                                         response time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapIntfQueryMaxRespTimeApply(mgmdMapCB_t *mgmdMapCbPtr,
                                          L7_uint32 intIfNum,
                                          L7_uint32 queryMaxRespTime);
/*********************************************************************
* @purpose  Updates vendor with configured startup Query Interval
*
* @param    mgmdMapCB            @b{(input)} Mapping Control Block.
* @param    intIfNum             @b{(input)} Internal Interface Number
* @param    startupQueryInterval @b{(input)} configured startup queryInterval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapIntfStartupQueryIntervalApply(mgmdMapCB_t *mgmdMapCbPtr,
                                              L7_uint32 intIfNum,
                                              L7_uint32 startupQueryInterval);
/*********************************************************************
* @purpose  Updates vendor with configured startup Query count
*
* @param    mgmdMapCB         @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    startupQueryCount @b{(input)} configured startup query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfStartupQueryCountApply(mgmdMapCB_t *mgmdMapCbPtr,
                                          L7_uint32 intIfNum,
                                          L7_uint32 startupQueryCount);
/*********************************************************************
* @purpose  Updates vendor with configured Last member query interval
*
* @param    mgmdMapCB            @b{(input)} Mapping Control Block.
* @param    intIfNum             @b{(input)} Internal Interface Number
* @param    lastMemQueryInterval @b{(input)} configured last member
*                                            query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfLastMemberQueryIntervalApply(mgmdMapCB_t *mgmdMapCbPtr,
                                                L7_uint32 intIfNum,
                                                L7_uint32 lastMemQueryInterval);
/*********************************************************************
* @purpose  Updates vendor with configured Last member query count
*
* @param    mgmdMapCB          @b{(input)} Mapping Control Block.
* @param    intIfNum           @b{(input)} Internal Interface Number
* @param    lastMemQueryCount  @b{(input)} configured last member
*                                          query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfLastMemberQueryCountApply(mgmdMapCB_t *mgmdMapCbPtr, 
                                             L7_uint32 intIfNum,
                                             L7_uint32 lastMemQueryCount);
/*********************************************************************
* @purpose  Updates vendor with configured unsolicited report interval
*
* @param    mgmdMapCB                 @b{(input)} Mapping Control Block.
* @param    intIfNum                  @b{(input)} Internal Interface Number
* @param    unsolicitedReportInterval @b{(input)} configured unsolicited
*                                                 report interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is specific to Proxy interface and hence applied only if the
*           proxy is enabled on the given interface 
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfUnsolicitedReportIntervalApply(mgmdMapCB_t *mgmdMapCbPtr,
                                                  L7_uint32 intIfNum,
                                                  L7_uint32 unsolicitedReportInterval);

/*********************************************************************
* @purpose  Determines if the specified internal interface is 
*           currently active, meaning the link is up and
*           in forwarding state
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL mgmdMapIntfIsActive(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  This is the callback function MGMD registers with IP6 MAP
*           for notification of routing events.
*
* @param    intIfnum    @b{(input)}  internal interface number
* @param    event       @b{(input)}  an event listed in l7_ip6_api.h
* @param    pData       @b{(input)}  unused
* @param    pEventInfo  @b{(input)}  provides parameters used to acknowledge
*                                    processing of the event
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmd6MapRoutingEventCallback (L7_uint32 intIfNum,
                                      L7_uint32 event,
                                      void *pData,
                                      ASYNC_EVENT_NOTIFY_INFO_t *response);
/*********************************************************************
* @purpose  Determine if the MGMD has been initialized
*
* @param    mgmdMapCbPtr   @b{(input)} Mapping Control Block.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL mgmdMapMgmdInitialized(mgmdMapCB_t *mgmdMapCbPtr);

/*********************************************************************
* @purpose  Determine if IP interface has been created and initialized
*           in the sub-system.
*
* @param    familyType     @b{(input)  Address Family type
* @param    intIfNum       @b{(input)  Internal Interface Number
*
* @returns  @b{(output)} L7_ENABLE
* @returns  @b{(output)} L7_ENABLE
*
* @comments This routine returns the user-configured state of the router.
*
*
* @end
*********************************************************************/
L7_BOOL  mgmdMapSubsystemIpIntfExists(L7_uchar8 familyType, 
                                      L7_uint32 intIfNum);

/*********************************************************************
* @purpose  To get the mapping layer CB based on family Type
*
* @param    familyType    @b{(input)} Address Family type.
* @param    mgmdMapCbPtr  @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCtrlBlockGet(L7_uchar8 familyType, 
                            mgmdMapCB_t **mgmdMapCbPtr);
#endif /* End of MGMD_UTIL.H */


