/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   mgmd_map.h
*
* @purpose    MGMD Mapping layer internal function prototypes
*
* @component  MGMD Mapping Layer
*
* @comments   none
*
* @create     03/12/2006
*
* @author     gkiran
* @end
*
**********************************************************************/        

#ifndef _INCLUDE_MGMD_MAP_H
#define _INCLUDE_MGMD_MAP_H

#include "mcast_api.h"
#include "mgmdmap_util.h"
/*--------------------------------------*/
/*  MGMD Map Task Messaging             */
/*--------------------------------------*/


/*-------------------------------------------------------------*/
/*  Queue-1 : Apptimer                                         */
/*-------------------------------------------------------------*/
#define MGMDMAP_APPTIMER_QUEUE    "mgmdMapAppTimerQueue"  
#define MGMDMAP_APPTIMER_Q_COUNT  MCAST_APPTIMER_QUEUE_MSG_COUNT  
#define MGMDMAP_APPTIMER_Q_SIZE   sizeof(mgmdAppTimerMsg_t)

#define MGMD_V4_HEAP_SIZE     L7_MGMD_V4_HEAP_SIZE
#define MGMD_V6_HEAP_SIZE     L7_MGMD_V6_HEAP_SIZE

#define MGMDMAP_HEAP_SIZE_GET(family)  ((family == L7_AF_INET) ? (MGMD_V4_HEAP_SIZE):(MGMD_V6_HEAP_SIZE))

#define MGMDMAP_MSG_QUEUE    "mgmdMapMsgQueue" 
#define MGMDMAP_MSG_Q_COUNT  (L7_RTR_MAX_RTR_INTERFACES + L7_MULTICAST_FIB_MAX_ENTRIES)
#define MGMDMAP_MSG_Q_SIZE   sizeof(mgmdMapMsg_t)

/*-------------------------------------------------------------*/
/*  Queue-3 : All ctrl pkt events                              */
/*-------------------------------------------------------------*/
#define MGMDMAP_PKT_QUEUE    "mgmdMapPktQueue" 
#define MGMDMAP_PKT_Q_COUNT  (L7_MGMD_GROUPS_MAX_ENTRIES)
#define MGMDMAP_PKT_Q_SIZE   sizeof(mgmdMapPktMsg_t)

#define MGMD_WARM_RESTART_INTERVAL_MSEC  5*1000     /* time to wait before declaring the restart process complete in milliseconds */

typedef enum
{
  MGMDMAP_CNFGR_MSG = 1,
  MGMDMAP_MCAST_EVENT_MSG,
  MGMDMAP_ROUTING_EVENT_MSG,
  MGMDMAP_RTR6_EVENT_MSG,
  MGMDMAP_PROTOCOL_EVENT_MSG,
  MGMDMAP_UI_EVENT_MSG
} mgmdMapMessages_t;

typedef enum
{
  MGMD_ADMIN_MODE_SET = 1,
  MGMD_INTF_MODE_SET,
  MGMD_VERSION_SET,
  MGMD_ROUTER_ALERT_CHECK_SET,
  MGMD_ROBUSTNESS_SET,
  MGMD_QUERY_INTERVAL_SET,
  MGMD_QUERY_MAX_RESPONSE_TIME_SET,
  MGMD_STARTUP_QUERY_INTERVAL_SET,
  MGMD_STARTUP_QUERY_COUNT_SET,
  MGMD_LASTMEMBER_QUERY_INTERVAL_SET,
  MGMD_LASTMEMBER_QUERY_COUNT_SET,
  MGMD_UNSOLICITED_REPORT_INTERVAL_SET
} MGMD_UI_EVENT_TYPE_t;

typedef struct mgmdMapProtocolEventParms_s 
{
  L7_uchar8                    familyType;
  L7_uint32                  event;
  union
  {
    mfcEntry_t            mfcInfo;       /* MFC Event Info */
    mcastAdminMsgInfo_t   asbInfo;       /* AdminScope Boundary Event Info */
    L7_uint32             rtrIfNum;      /* for MGMD Group Info Get */
  }u;
} mgmdMapProtocolEventParms_t;
typedef struct mgmdMapCtrlPktEventParms_s 
{
  MCAST_CB_HNDL_t            cbHandle;
  L7_uint32                  event;
  mcastControlPkt_t          ctrlPkt; 
} mgmdMapCtrlPktEventParms_t;

typedef struct mgmdMapRoutingEventParms_s 
{
  L7_uint32                  event;
  L7_uint32                  intIfNum;
  void                      *pData;
  L7_BOOL                    asyncResponseRequired;
  ASYNC_EVENT_NOTIFY_INFO_t  eventInfo;
} mgmdMapRoutingEventParms_t;


typedef struct mgmdMapUIEventParms_s 
{
  L7_uint32 event;
  MCAST_CB_HNDL_t cbHandle;
  L7_uint32  eventInfo;
  L7_uint32  intIfNum;
  L7_uint32  intfType;
  L7_uint32  version;
} mgmdMapUIEventParms_t;

typedef struct
{
  L7_uint32   msgId;    
  union 
  {
    L7_CNFGR_CMD_DATA_t         cnfgrCmdData;
    mgmdMapRoutingEventParms_t  mgmdMapRoutingEventParms;
    mgmdMapUIEventParms_t       mgmdMapUiEventParms;
    mgmdMapProtocolEventParms_t mgmdMapProtocolEvenetParms;   
  } u;
} mgmdMapMsg_t;

typedef struct
{
  L7_uint32   msgId;    
  L7_uchar8                  familyType;
  L7_uint32                  event;
  mcastControlPkt_t          ctrlPkt; 
} mgmdMapPktMsg_t;

typedef struct
{
  L7_uint32   msgId;    
  L7_uint32                     event;
  L7_APP_TMR_CTRL_BLK_t   timerCtrlBlk;
} mgmdAppTimerMsg_t;

/*********************************************************************
*
* @purpose task to handle all MGMD Mapping messages
*
* @param void
*
* @returns void
*
* @comments none
*
* @end
*
*********************************************************************/
void mgmdMapTask();

/*********************************************************************
* @purpose  Saves mgmd user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdSave(void);

/*********************************************************************
* @purpose  Restores MGMD user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdRestore(void);

/*********************************************************************
* @purpose  Checks if mgmd user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL mgmdHasDataChanged(void);
void mgmdResetdataChanged(void);

/*********************************************************************
* @purpose  Checks if Ipv4 mgmd user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL mgmdV4HasDataChanged(void);

/*********************************************************************
* @purpose  Build default mgmd config data
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void    mgmdBuildDefaultConfigData(L7_uint32 ver);
/*********************************************************************
* @purpose  Build default mgmd config data
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    ver             @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void mgmdCommonBuildDefaultConfigData(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 ver);
/*********************************************************************
* @purpose  Build default mgmd6 config data
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void mgmd6BuildDefaultConfigData(L7_uint32 ver);
/*********************************************************************
* @purpose  Applies mgmd config data
*
* @param    mgmdMapCbPtr   @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdApplyConfigData(mgmdMapCB_t *mgmdMapCbPtr);

/*********************************************************************
* @purpose  Determine if the interface is valid for MGMD configuration 
*           and optionally output a pointer to the configuration structure
*           entry for this interface
*
* @param    mgmdMapCB   @b{(input)} Mapping Control Block.
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    pCfg        @b{(inout)} Output pointer location, or L7_NULL if not needed
*                                   Pointer to MGMD interface config structure
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the  
*           interface config data structure.
*
* @end
*********************************************************************/
L7_BOOL mgmdMapIntfIsConfigurable(mgmdMapCB_t *mgmdMapCB, L7_uint32 intIfNum, 
                                  mgmdIntfCfgData_t **pCfg);

/*********************************************************************
* @purpose  Retrieve a pointer to an empty configuration data for a given
*           interface if found.
*
* @param    mgmdMapCB   @b{(input)} Mapping Control Block.
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    pCfg        @b{(inout)} Output pointer location
*                                   Pointer to MGMD interface config structure
*
* @returns  L7_TRUE     an empty interface config structure was found
* @returns  L7_FALSE    no empty interface config structure entry is available
*
* @comments Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the  
*           interface config data structure.
*
* @end
*********************************************************************/
L7_BOOL mgmdMapIntfConfigEntryGet(mgmdMapCB_t *mgmdMapCB, L7_uint32 intIfNum, 
                                  mgmdIntfCfgData_t **pCfg);

/*********************************************************************
* @purpose  Build default MGMD config data for an intf 
*
* @param    mgmdMapCbPtr  @b{(input)} Mapping Control Block.
* @param    pCfg         @b{(output)} pointer to the MGMD 
*                                     config data for the interface
*
* @returns  L7_SUCCESS   default configuration applied
* @returns  L7_FAILURE   could not build default config data for interface
*
* @comments none 
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfBuildDefaultConfigData(mgmdMapCB_t *mgmdMapCbPtr, 
                                       mgmdIntfCfgData_t *pCfg);

/*********************************************************************
* @purpose  Propogate MCAST Event notifications to Mgmd Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info
*
* @returns  L7_SUCCESS  
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMcastEventCallback(L7_uint32 event, 
                                  void *pData, 
                                  ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);


/*********************************************************************
* @purpose  Send AdminScope Boundary event to MGMD (Proxy ) Vendor
*
* @param    eventType    @b{(input)} Event Type 
* @param    msgLen       @b{(input)} Message Length
* @param    eventMsg     @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is an IPv4 specific function
*
* @end
*********************************************************************/
L7_RC_t mgmdMapAdminScopeEventChangeCallback (L7_uint32 eventType, 
                                               L7_uint32 msgLen, 
                                               void *eventMsg);

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
* @purpose  Propogate Routing Event notifications to MGMD Map task
*
* @param    intIfNum     @b{(input)} Interface number
* @param    event        @b{(input)} Event type
* @param    pData        @b{(input)} Data
* @param    pEventInfo   @b{(input)} Event Info.
*
* @returns  L7_SUCCESS  
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapRoutingEventCallback(L7_uint32 intIfNum, 
                                    L7_uint32 event, 
                                    void *pData, 
                                    ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
/*********************************************************************
* @purpose  Apply configuration data to an MGMD Interface
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdApplyIntfConfigData(mgmdMapCB_t *mgmdMapCB, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Configure MGMD Interface
*
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    mgmdMapCBPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceConfigure(L7_uint32 intIfNum, mgmdMapCB_t *mgmdMapCB);

/*********************************************************************
* @purpose  Send event to MGMD Vendor
*
* @param    familyType    @b{(input)}  Address Family type
* @param    eventType     @b{(input)}  Event Type
* @param    msgLen        @b{(input)}  Message Length
* @param    eventMsg      @b{(input)}  Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapComponentCallback(L7_uchar8 familyType, L7_uint32  eventType, 
                  L7_uint32  msgLen, void *eventMsg);

/*********************************************************************
* @purpose  Post UI event to the Mapping layer
*
* @param    addrFamily  @b{(input)} Address Family Identifier
* @param    event       @b{(input)} UI Event
* @param    pData       @b{(input)} UI Event Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapUIEventSend (mgmdMapCB_t *mgmdMapCbPtr,
                            MGMD_UI_EVENT_TYPE_t eventType, 
                            L7_uint32 mode,
                            L7_uint32 intIfNum,
                            L7_MGMD_INTF_MODE_t intfType);  

/*********************************************************************
* @purpose  To post a message to the corresponding Queue
*
* @param    QIndex  @b{(input)}   Queue Index
* @param    msg       @b{(input)} Msg pointer
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/

L7_RC_t mgmdMessageSend(MGMD_QUEUE_ID_t QIndex,void *msg);

/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    mgmdMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrMgmdInitPhase1DynamicProcess (mgmdMapCB_t* mgmdMapCb);

/*********************************************************************
* @purpose  This function De-allocates the memory for the Vendor
*           Protocol upon its Disable.
*
* @param    mgmdMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrMgmdFiniPhase1DynamicProcess (mgmdMapCB_t* mgmdMapCb);

/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    mgmdMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrProxyInitPhase1DynamicProcess (mgmdMapCB_t* mgmdMapCb);

/*********************************************************************
* @purpose  This function De-allocates the memory for the Vendor
*           Protocol upon its Disable.
*
* @param    mgmdMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrProxyFiniPhase1DynamicProcess (mgmdMapCB_t* mgmdMapCb);

/**********************DEBUG TRACE ROUTINE PROTOTYPES ***************/

/*********************************************************************
 * @purpose  Saves mgmd configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    mgmdDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t mgmdDebugSave(void);
/*********************************************************************
 * @purpose  Restores mgmd debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    mgmdDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t mgmdDebugRestore(void);
/*********************************************************************
 * @purpose  Checks if mgmd debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL mgmdDebugHasDataChanged(void);

/*********************************************************************
 * @purpose  Build default mgmd config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void mgmdDebugBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
 * @purpose  Apply mgmd debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t mgmdApplyDebugConfigData(void);

L7_RC_t mgmdMapMemoryInit(mgmdMapCB_t* mgmdMapCb);

L7_RC_t mgmdMapMemoryDeInit(mgmdMapCB_t* mgmdMapCb);

#endif /* #ifndef _INCLUDE_MGMD_MAP_H */
