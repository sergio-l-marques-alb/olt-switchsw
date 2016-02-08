/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dvmrp_util.h
*
* @purpose    DVMRP Mapping layer internal function prototypes
*
* @component  DVMRP Mapping Layer
*
* @comments   none
*
* @create     02/08/2002
*
* @author     M Pavan K Chakravarthi
* @end
*
**********************************************************************/

#ifndef _DVMRP_UTIL_H_
#define _DVMRP_UTIL_H_
#include "mcast_api.h"
#include "l7apptimer_api.h"
#include "dvmrp_debug_api.h"
#include "dvmrp_cache_table.h"

/*-------------------------------------*/
/*  DVMRP MAP INFO STRUCTURES          */
/*-------------------------------------*/

typedef enum
{
  DVMRP_APP_TIMER_Q = 0,
  DVMRP_EVENT_Q,
  DVMRP_CTRL_PKT_Q, 
  DVMRP_MAX_Q
}DVMRP_QUEUE_ID_t;


typedef struct dvmrpInfo_s
{
  L7_BOOL             dvmrpEnabled;/* indicates vendor DVMRP stack init'd */
} dvmrpInfo_t;

typedef struct dvmrpIntfInfo_s
{
  L7_BOOL             dvmrpEnabled;
  L7_uint32           intIfNum;
  AcquiredMask        acquiredList; /* Mask of components "acquiring"
                                       an interface */
} dvmrpIntfInfo_t;

typedef struct dvmrpQueue_s
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
  
}dvmrpQueue_t;



/*-------------------------------------*/
/*  DVMRP GLOBAL STRUCTURES          */
/*-------------------------------------*/
typedef struct dvmrpGblVaribles_s 
{
  dvmrpInfo_t               DvmrpInfo;
  dvmrpIntfInfo_t           DvmrpIntfInfo[MAX_INTERFACES+1];
  L7_dvmrpMapCfg_t          DvmrpMapCfgData;
  L7_uint32                 DvmrpMapCfgMapTbl[L7_MAX_INTERFACE_COUNT];

  dvmrpQueue_t              dvmrpQueue[DVMRP_MAX_Q];
  void                      *msgQSema;
  L7_int32                  dvmrpMapTaskId;
  dvmrpCnfgrState_t         dvmrpCnfgrState;
  MCAST_CB_HNDL_t           dvmrpCb;
  L7_uchar8                 family;
  L7_BOOL                   heapAlloc; 
} dvmrpGblVaribles_t;

extern dvmrpGblVaribles_t dvmrpGblVar_g;


/*  DVMRP Debug Trace Info */

#define DVMRP_DEBUG_CFG_FILENAME      "dvmrp_debug.cfg"
#define DVMRP_DEBUG_CFG_VER_1          0x1
#define DVMRP_DEBUG_CFG_VER_CURRENT    DVMRP_DEBUG_CFG_VER_1


#define DVMRP_DEBUG_TRACE_FLAG_BYTES     ((DVMRP_DEBUG_LAST_TRACE + 7) / 8)
#define DVMRP_DEBUG_TRACE_FLAG_BITS_MAX      8
#define DVMRP_DEBUG_TRACE_FLAG_VALUE         1

typedef  L7_uchar8 dvmrpDebugTraceFlags_t[DVMRP_DEBUG_TRACE_FLAG_BYTES];

typedef struct dvmrpDebugCfgData_s 
{
  dvmrpDebugTraceFlags_t dvmrpDebugTraceFlag;
} dvmrpDebugCfgData_t;

typedef struct dvmrpDebugCfg_s
{
  L7_fileHdr_t          hdr;
  dvmrpDebugCfgData_t   cfg;
  L7_uint32             checkSum;
} dvmrpDebugCfg_t;










/*--------------------------------------*/
/*  dvmrp Map Task Messaging             */
/*--------------------------------------*/

/*-------------------------------------------------------------*/
/*  Queue-1 : Apptimer                                         */
/*-------------------------------------------------------------*/
#define DVMRPMAPAPPTIMER_QUEUE     "dvmrpMapAppTimerQueue" 
#define DVMRPMAP_APPTIMER_COUNT  MCAST_APPTIMER_QUEUE_MSG_COUNT  
#define DVMRPMAP_APPTIMER_SIZE   sizeof(dvmrpAppTimerMsg_t)

/*-------------------------------------------------------------*/
/*  Queue-2 : All mapping Events +  MGMD events                */
/*-------------------------------------------------------------*/
#define DVMRPMAPMSG_QUEUE     "dvmrpMapMsgQueue" 
#define DVMRPMAP_MSG_COUNT  (L7_RTR_MAX_RTR_INTERFACES + L7_MGMD_GROUPS_MAX_ENTRIES)
#define DVMRPMAP_MSG_SIZE   sizeof(dvmrpMapMsg_t)

/*-------------------------------------------------------------*/
/*  Queue-3 : All ctrl pkt events + MFC events                 */
/*-------------------------------------------------------------*/
#define DVMRPMAPPKT_QUEUE     "dvmrpMapPktQueue" 
#define DVMRPMAP_PKT_COUNT  (L7_MAX_NETWORK_BUFF_PER_BOX +  DVMRP_MRT_MAX_IPV4_ROUTE_ENTRIES)  
#define DVMRPMAP_PKT_SIZE   sizeof(dvmrpMapPktMsg_t)



typedef enum
{
  DVMRPMAP_CNFGR_MSG = 1,
  DVMRPMAP_INTF_CHANGE_MSG,
  DVMRPMAP_ROUTING_EVENT_MSG,
  DVMRPMAP_MCAST_EVENT_MSG,
  DVMRPMAP_PROTOCOL_EVENT_MSG,
  DVMRPMAP_UI_EVENT_MSG  
} dvmrpMapMessages_t;

typedef struct dvmrpEventMsg_s
{
  /* Union of all Events that DVMRP can receive */
  union
  {
    mfcEntry_t            mfcInfo;       /* MFC Event Info */
    mgmdMrpEventInfo_t    mgmdInfo;      /* MGMD Event Info */
    mcastAdminMsgInfo_t   asbInfo;       /* AdminScope Boundary Event Info */
    L7_int32              intfIndex;     /* interface index (MGMD Qurier) */
    dvmrp_timer_event_t   timerBlock;    /* Application Timer Event Info */
  }u;
}dvmrpEventMsg_t;

typedef struct dvmrpMapIntfChangeParms_s 
{
  L7_uint32        event;
  L7_uint32        intIfNum;
  NIM_CORRELATOR_t correlator;
} dvmrpMapIntfChangeParms_t;

typedef struct dvmrpMapRoutingEventParms_s 
{
  L7_uint32                  event;
  L7_uint32                  intIfNum;
  void                      *pData;
  L7_BOOL                    asyncResponseRequired;
  ASYNC_EVENT_NOTIFY_INFO_t  eventInfo;
} dvmrpMapRoutingEventParms_t;

typedef struct dvmrpMapIgmpEventParms_s 
{
  L7_uint32                  event;
  L7_uint32                  intIfNum;
  void                      *pData;
  ASYNC_EVENT_NOTIFY_INFO_t  eventInfo;
} dvmrpMapIgmpEventParms_t;

typedef struct dvmrpMapProtocolEventParms_s 
{
  L7_uint32                  event;
  L7_uchar8                  familyType;
  dvmrpEventMsg_t            msgData;
} dvmrpMapProtocolEventParms_t;
typedef struct dvmrpMapUiAdminMode_s
{
  L7_uint32 mode;
}dvmrpMapUiAdminMode_t;

typedef struct dvmrpMapUiIntfMode_s
{
  L7_uint32 intIfNum;
  L7_uint32 mode;
}dvmrpMapUiIntfMode_t;

typedef struct dvmrpMapUiIntfMetric_s
{
  L7_uint32 intIfNum;
  L7_uint32 metric;
}dvmrpMapUiIntfMetric_t;


typedef struct dvmrpMapUIEventParms_s 
{
  L7_uint32 event;
  MCAST_CB_HNDL_t cbHandle;
  union
  {
    dvmrpMapUiAdminMode_t  adminMode;
    dvmrpMapUiIntfMode_t   intfMode;
    dvmrpMapUiIntfMetric_t intfMetric;
  }eventInfo;
} dvmrpMapUIEventParms_t;

typedef struct
{
  L7_uint32   msgId;    
  union 
  {
    L7_CNFGR_CMD_DATA_t          cnfgrCmdData;
    dvmrpMapIntfChangeParms_t    dvmrpMapIntfChangeParms;
    dvmrpMapRoutingEventParms_t  dvmrpMapRoutingEventParms;
    dvmrpMapIgmpEventParms_t     dvmrpMapIgmpEventParms;
    dvmrpMapProtocolEventParms_t dvmrpMapProtocolEventParms;
    dvmrpMapUIEventParms_t       dvmrpMapUiEventParms;    
  } u;
} dvmrpMapMsg_t;


typedef struct dvmrpMapAppTimerEventParms_s 
{
  L7_uchar8                     familyType;
  L7_uint32                     event;
  L7_APP_TMR_CTRL_BLK_t   timerCtrlBlk;  /* Application Timer Event Info */
} dvmrpMapAppTimerEventParms_t;


typedef struct dvmrpMapCtrlPktEventParms_s 
{
  L7_uchar8                  familyType;
  L7_uint32                  event;
  mcastControlPkt_t          ctrlPkt; 
} dvmrpMapCtrlPktEventParms_t;


typedef struct
{
  L7_uint32   msgId;    
  dvmrpMapCtrlPktEventParms_t dvmrpMapCtrlPktEventParms;
} dvmrpMapPktMsg_t;

typedef struct
{
  L7_uint32   msgId;    
  dvmrpMapAppTimerEventParms_t dvmrpMapAppTimerEventParms;
} dvmrpAppTimerMsg_t;



/*
**********************************************************************
*                    FUNCTION PROTOTYPES
**********************************************************************
*/


/* dvmrp_util.c */
/*********************************************************************
* @purpose  Enable the DVMRP Routing Function
*
* @param    dvmrpDoInit      @b{(input)} Flag to check whether DVMRP
*                                        Memory can be Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpAdminModeEnable(L7_BOOL dvmrpDoInit);

/*********************************************************************
* @purpose  Disable the DVMRP Routing Function
*
* @param    dvmrpDoInit      @b{(input)} Flag to check whether DVMRP
*                                        Memory can be De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpAdminModeDisable(L7_BOOL dvmrpDoInit);

/*********************************************************************
* @purpose  Set the DVMRP admin mode for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} Administrative mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes:
*           1. DVMRP is enabled
*           2. Interface has been created and configured for DVMRP at
*              the DVMRP vendor layer
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfAdminModeApply(L7_uint32 intIfNum,L7_uint32 mode);

/*********************************************************************
* @purpose  Set the DVMRP Metric for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    metric      @b{(input)} Administrative mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes:
*           1. DVMRP is enabled
*           2. Interface has been created and configured for DVMRP at
*              the DVMRP vendor layer
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfMetricApply(L7_uint32 intIfNum, L7_ushort16 metric);

/*********************************************************************
* @purpose  Configure DVMRP router interfaces
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouterIntfConfigure(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Propogate Routing Event notifications to DVMRP Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                            L7_uint32 event,
                                            void *pData, 
                                            ASYNC_EVENT_NOTIFY_INFO_t *response);

/*********************************************************************
* @purpose  Propogate MCAST Event notifications to Dvmrp Map task
*
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event type
* @param    pData      @b{(input)} data
* @param    pEventInfo @b{(input)} event info
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapMcastEventCallBack(L7_uint32 event, 
                                   void *pData, 
                                   ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

/*********************************************************************
* @purpose  Propogate Interface notifications to DVMRP Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    correlator  @b{(input)} Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
                                   NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Sets the interface Configuration parameters to default
*
* @param    pCfg   @b{(input)} pointer to the DVMRP ckt configuration record
*
* @returns  Void
*
* @notes
*
* @end
*********************************************************************/
void dvmrpMapIntfBuildDefaultCfg(dvmrpCfgCkt_t *pCfg);

/*********************************************************************
* @purpose  Determine if the interface is valid for DVMRP configuration
*           and optionally output a pointer to the configuration structure
*           entry for this interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    pCfg        @b{(input)} Output pointer location, or L7_NULL if not needed
*                       @b{(output)} Pointer to DVMRP interface config structure
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the
*           interface config data structure.
*
* @end
*********************************************************************/
L7_BOOL dvmrpMapIntfIsConfigurable(L7_uint32 intIfNum, dvmrpCfgCkt_t **pCfg);

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dvmrpIntfCreate(L7_uint32 intIfNum); 
/*********************************************************************
* @purpose  Retrieve a pointer to an empty configuration data for a given
*           interface if found.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    pCfg        @b{(input)} Output pointer location
*                       @b{(output)} Pointer to DVMRP interface config structure
*
* @returns  L7_TRUE     an empty interface config structure was found
* @returns  L7_FALSE    no empty interface config structure entry is available
*
* @notes    Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the
*           interface config data structure.
*
* @end
*********************************************************************/
L7_BOOL dvmrpMapIntfConfigEntryGet(L7_uint32 intIfNum, dvmrpCfgCkt_t **pCfg);

/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    dvmrpGblVar_g  @b{(input)}   DVMRP Global Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCnfgrInitPhase1DynamicProcess (dvmrpGblVaribles_t *pDvmrpGblVar);
L7_RC_t dvmrpMapMemoryInit (dvmrpGblVaribles_t *pDvmrpGblVar);
L7_RC_t dvmrpMapMemoryDeInit (dvmrpGblVaribles_t *pDvmrpGblVar);

/*********************************************************************
* @purpose  This function De-allocates the memory for the Vendor
*           Protocol upon its Disable.
*
* @param    dvmrpGblVar_g  @b{(input)}   DVMRP Global Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCnfgrFiniPhase1DynamicProcess (dvmrpGblVaribles_t *pDvmrpGblVar);

#endif /* _DVMRP_MAP_UTIL_H_ */
