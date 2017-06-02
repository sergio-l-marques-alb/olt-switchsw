/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pimsm_map.h
*
* @purpose    PIM-SM Mapping layer internal function prototypes
*
* @component  PIM-SM Mapping Layer
*
* @comments   none
*
* @create     13/11/2005
*
* @author     gkiran/dsatyanarayana
* @end
*
**********************************************************************/
#ifndef _PIMSM_MAP_H_
#define _PIMSM_MAP_H_

#include "mcast_wrap.h"
#include "mfc_api.h"
#include "pimsmdefs.h"

typedef enum
{
  PIMSMMAP_CNFGR_MSG = 1,
  PIMSMMAP_ROUTING_EVENT_MSG,
  PIMSMMAP_RTR6_EVENT_MSG,
  PIMSMMAP_MCAST_EVENT_MSG,
  PIMSMMAP_PROTOCOL_EVENT_MSG,
  PIMSMMAP_CFG_SET_EVENT_MSG,
} pimsmMapMessages_t;

typedef enum
{
  PIMSMMAP_ADMIN_MODE_EVENT = 1,
  PIMSMMAP_SSM_EVENT,
  PIMSMMAP_STATIC_RP_EVENT,
  PIMSMMAP_CAND_RP_EVENT,
  PIMSMMAP_CAND_BSR_EVENT,
  PIMSMMAP_SPT_THRESHOLD_EVENT,
  PIMSMMAP_REGISTER_THRESHOLD_EVENT,
  PIMSMMAP_INTF_MODE_EVENT,
  PIMSMMAP_INTF_HELLO_INTV_EVENT,
  PIMSMMAP_INTF_JP_INTV_EVENT,
  PIMSMMAP_INTF_DR_PRIORITY_EVENT,
  PIMSMMAP_INTF_BSR_BORDER_EVENT,
  PIMSMMAP_RESTART_MODE_EXIT_EVENT,
  PIMSMMAP_CFG_SET_LAST_EVENT
} PIMSMMAP_CFG_SET_EVENT_ID_t;

typedef struct pimsmEventMsg_s
{
  union
  {
    mfcEntry_t            mfcEntry;       /* MFC Event Info */
    mgmdMrpEventInfo_t    mgmdGrpInfo;    /* MGMD Event Info */
    mcastAdminMsgInfo_t   adminInfo;      /* AdminScope Boundary Event Info */
    mcastRPFInfo_t        rpfInfo;        /* RPF Info */
  } u;
} pimsmEventMsg_t;

typedef struct pimsmMapRoutingEventParms_s
{
  L7_uint32                  event;
  L7_uint32                  intIfNum;
  void                      *pData;
  ASYNC_EVENT_NOTIFY_INFO_t  eventInfo;
} pimsmMapRoutingEventParms_t;

typedef struct pimsmMapMcastEventParms_s
{
  L7_uint32                  event;
  L7_uint32                  intIfNum;
  void                      *pData;
  ASYNC_EVENT_NOTIFY_INFO_t  eventInfo;
} pimsmMapMcastEventParms_t;

typedef struct pimsmMapProtocolEventParms_s 
{
  L7_uint32                  event;
  /*MCAST_CB_HNDL_t            cbHandle;*/
  L7_uchar8                  familyType;
  pimsmEventMsg_t            msgData;
} pimsmMapProtocolEventParms_t;

typedef struct pimsmMapCtrlPktEventParms_s 
{
  L7_uint32                  event;
  /*MCAST_CB_HNDL_t            cbHandle;*/
  L7_uchar8                  familyType;
  union
  {
    mfcEntry_t            mfcEntry;       /* MFC Event Info */
  mcastControlPkt_t          msgData;        /* IPv4 Control Pkt Reception Info */
  }u;
} pimsmMapCtrlPktAndMfcEventParms_t;


typedef struct pimsmMapDataPktEventParms_s 
{
  L7_uint32                  event;
  /*MCAST_CB_HNDL_t            cbHandle;*/
  L7_uchar8                  familyType;
  union {
     mfcEntry_t            mfcEntry;       /* MFC Event Info */
     mcastControlPkt_t     msgData;        /* IPv4 Control Pkt Reception Info */
  } u;
} pimsmMapDataPktEventParms_t;

typedef struct pimsmMapAppTmrEventParms_s 
{
  L7_uint32                  event;
  /*MCAST_CB_HNDL_t            cbHandle;*/
  L7_uchar8                  familyType;
} pimsmMapAppTmrEventParms_t;


typedef struct pimsmMapIntfData_s {
   L7_uint32 intIfNum;
   L7_uint32  opaqueData;   
} pimsmMapIntfData_t;

/* eventdata w.r.t event MCAST_EVENT_PIMSM_STATIC_RP_SET */
typedef struct pimsmMapStaticRPdata_s {
   L7_uint32 mode; 
   L7_inet_addr_t rpAddr;
   L7_uchar8 prefixLen;
   L7_inet_addr_t grpAddr;
   L7_BOOL overrideFlag;
   L7_uint32 index;   
} pimsmMapStaticRPdata_t;

/* eventdata w.r.t event PIMSM_CAND_RP_SET */
typedef struct pimsmMapCandRPdata_s {
   L7_uint32        mode; 
   L7_uint32 intIfNum;
   L7_inet_addr_t rpAddr;
   L7_uchar8 prefixLen;
   L7_inet_addr_t grpAddr;
   L7_uint32 index;
} pimsmMapCandRPdata_t;

/* eventdata w.r.t event PIMSM_CAND_BSR_SET */
typedef struct pimsmMapCandBSRdata_s {
   L7_uint32 mode; 
   L7_inet_addr_t bsrAddr;   
   L7_uint32 intIfNum;
   L7_uint32      bsrPriority;       
   L7_uint32      bsrHashMaskLength;
   L7_uint32      bsrScope; 
} pimsmMapCandBSRdata_t;

typedef struct pimsmMapSSMdata_s {
   L7_uint32        operMode; 
   L7_uchar8 prefixLen;
   L7_inet_addr_t grpAddr;
} pimsmMapSSMdata_t;
/*
 * event: PIMSMMAP_ADMIN_MODE_EVENT,
 * data : mode
 *
 * event: PIMSMMAP_SSM_EVENT,
 * data : grpAddr, prefixLen, operMode
 *
 * event: PIMSMMAP_STATIC_RP_EVENT,
 * data : rpAddr, grpAddr, prefixLen, overrideFlag, operMode
 *
 * event: PIMSMMAP_CAND_RP_EVENT,
 * data : rpAddr, grpAddr, prefixLen, operMode
 *
 * event: PIMSMMAP_CAND_BSR_EVENT,
 * data : bsrAddr, bsrPriority, bsrHashMaskLength, bsrScope, operMode
 *
 * event: PIMSMMAP_SPT_THRESHOLD_EVENT,
 * data : threshold
 *
 * event: PIMSMMAP_REGISTER_THRESHOLD_EVENT,
 * data : threshold
 *
 * event: PIMSMMAP_INTF_MODE_EVENT,
 * data : intfNum, operMode
 *
 * event: PIMSMMAP_INTF_HELLO_INTV_EVENT,
 * data : intfNum, interval
 *
 * event: PIMSMMAP_INTF_JP_INTV_EVENT,
 * data : intfNum, interval
 *
 * event: PIMSMMAP_DR_PRIORITY_EVENT,
 * data : intfNum, drPriority
 *
 * event: PIMSMMAP_BSR_BORDER_EVENT,
 * data : intfNum, bsrBorder
 *
 */

typedef struct pimsmMapCfgSetEventInfo_s 
{
  L7_uint32                  event;
  pimsmMapCB_t              *pimsmMapCbPtr;
  union {
     pimsmMapIntfData_t     intfData;
     pimsmMapStaticRPdata_t staticRPData;
     pimsmMapCandRPdata_t   candRPData;
     pimsmMapCandBSRdata_t  candBSRData;
     pimsmMapSSMdata_t      ssmData;
     L7_uint32 mode;
     L7_uint32 threshold;
  } u;
} pimsmMapCfgSetEventInfo_t;

typedef struct
{
  L7_uint32   msgId;
  union
  {
    L7_CNFGR_CMD_DATA_t          cnfgrCmdData;
    pimsmMapRoutingEventParms_t  pimsmMapRoutingEventParms;
    pimsmMapProtocolEventParms_t pimsmMapProtocolEventParms;
    pimsmMapMcastEventParms_t     pimsmMapMcastEventParms;
    pimsmMapCfgSetEventInfo_t    pimsmMapCfgSetEventInfo;
  } u;
} pimsmMapMsg_t;

typedef struct
{
  L7_uint32   msgId;
  pimsmMapCtrlPktAndMfcEventParms_t     pimsmMapCtrlPktAndMfcParms;
} pimsmMapCtrlPktAndMfcMsg_t;

typedef struct
{
  L7_uint32   msgId;
  pimsmMapAppTmrEventParms_t     pimsmMapAppTmrParms;
} pimsmMapAppTmrMsg_t;

typedef struct
{
  L7_uint32   msgId;
  pimsmMapDataPktEventParms_t pimsmMapDataPktEventParms;
} pimsmMapDataPktMsg_t;

#define PIMSM_WARM_RESTART_INTERVAL_MSEC  60*1000     /* time to wait before declaring the restart process complete in milliseconds */

/*-------------------------------------------------------------*/
/*  Queue-1 : Apptimer                                         */
/*-------------------------------------------------------------*/
#define PIMSMMAP_APP_TMR_QUEUE     "pimsmMapAppTmrQueue"
#define PIMSMMAP_APP_TMR_MSG_COUNT  MCAST_APPTIMER_QUEUE_MSG_COUNT
#define PIMSMMAP_APP_TMR_MSG_SIZE   sizeof(pimsmMapAppTmrMsg_t)

/*-------------------------------------------------------------*/
/*  Queue-2 : All mapping Events +  MGMD events                */
/*-------------------------------------------------------------*/
#define PIMSMMAP_QUEUE     "pimsmMapEventsQueue"
#define PIMSMMAP_MSG_COUNT  (L7_IPMAP_INTF_MAX_COUNT + (L7_MGMD_GROUPS_MAX_ENTRIES*2))
#define PIMSMMAP_MSG_SIZE   sizeof(pimsmMapMsg_t)

/* Reserving 60% buffer space of the Events Queue for MGMD events */
#define PIMSMMAP_MGMD_EVENTS_COUNT_LIMIT ((PIMSMMAP_MSG_COUNT * 60)/100)

/*-------------------------------------------------------------*/
/*  Queue-3 : All ctrl pkt events + MFC events                 */
/*-------------------------------------------------------------*/
#define PIMSMMAP_CTRL_PKT_QUEUE     "pimsmMapCtrlPktQueue"
#define PIMSMMAP_CTRL_PKT_MSG_COUNT  (L7_MAX_NETWORK_BUFF_PER_BOX +  L7_MULTICAST_FIB_MAX_ENTRIES)
#define PIMSMMAP_CTRL_PKT_MSG_SIZE   sizeof(pimsmMapCtrlPktAndMfcMsg_t)

/* Reserving 60% buffer space of the Events Queue for MFC events */
#define PIMSMMAP_MFC_EVENTS_COUNT_LIMIT ((PIMSMMAP_CTRL_PKT_MSG_COUNT * 60)/100)


/*-------------------------------------------------------------*/
/*  Queue-4 : All data pkt events + MFC (whole-pkt) events + Register-pkt    */
/*-------------------------------------------------------------*/
#define PIMSMMAP_DATA_PKT_QUEUE     "pimsmMapDataPktQueue"
#define PIMSMMAP_DATA_PKT_MSG_COUNT  (L7_MAX_NETWORK_BUFF_PER_BOX +  L7_MULTICAST_FIB_MAX_ENTRIES)
#define PIMSMMAP_DATA_PKT_MSG_SIZE   sizeof(pimsmMapDataPktMsg_t)


/*
**********************************************************************
*                    FUNCTION PROTOTYPES
**********************************************************************
*/
/* pimsm_map.c */
/*********************************************************************
*
* @purpose task to handle all PIM-DM Mapping messages
*
* @param   none
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void pimsmMapTask();

/*********************************************************************
* @purpose  Save PIM-SM user config file to NVStore
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmSave(void);

/*********************************************************************
* @purpose  Restore PIM-SM user config file to factory defaults
*
* @param   none 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmRestore(void);

/*********************************************************************
* @purpose  Check if PIM-SM user config data has changed
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimsmHasDataChanged(void);
void pimsmResetDataChanged(void);
/*********************************************************************
* @purpose  Check if PIM-SM V4 user config data has changed
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimsmV4HasDataChanged(void);

/*********************************************************************
* @purpose  Build default PIM-SM config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void pimsm6BuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Build default PIM-SM config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void pimsmBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Apply PIM-SM config data
*
* @param    pimsmMapCbPtr     @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS       Config data applied
* @returns  L7_FAILURE       Unexpected condition encountered
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmApplyConfigData(pimsmMapCB_t *pimsmMapCbPtr);

/*********************************************************************
* @purpose  Propogate Routing Event notifications to PIM-SM Map task
*
* @param    intIfNum          @b{(input)} Interface number
* @param    event             @b{(input)} Event type
* @param    pData             @b{(input)} Data
* @param    pEventInfo        @b{(input)} EventInfo.
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRoutingEventChangeCallBack(L7_uint32 intIfNum,
                                        L7_uint32 event,
                                        void *pData,
                                        ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
/*********************************************************************
* @purpose  Propogate Routing Event notifications to PIM-DM6 Map task
*
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event type
* @param    pData      @b{(input)} Data
* @param    pEventInfo @b{(input)} Event Info.
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsm6MapRoutingEventChangeCallBack(L7_uint32 intIfNum,
                                      L7_uint32 event,
                                      void *pData,
                                      ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
/*********************************************************************
* @purpose  Propogate MCAST Event notifications to PIM-SM Map task
*
* @param    intIfNum   @b{(input)}   Interface number
* @param    event      @b{(input)}   Event type
* @param    pData      @b{(input)}   Data
* @param    pEventInfo @b{(input)}   EventInfo.
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapMcastEventCallBack(L7_uint32 event,
                                   void *pData,
                                   ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
/*********************************************************************
* @purpose  Propogate MCAST Admin-scope Event notifications to PIM-SM Map task
*

* @param    event      @b{(input)} Event type
* @param    pData      @b{(input)} Data
* @param    pEventInfo @b{(input)} EventInfo.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapAdminScopeEventChangeCallback(L7_uint32 eventType, 
                                              L7_uint32 msgLen,
                                              void *eventMsg);

/* MCAST_PHASE_II_WRAPPERS_START */

/*********************************************************************
* @purpose  Build default PIMSM config data for an intf
*
* @param    pCfg     @b{(input)}  pointer to the DVMRP ckt configuration record
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void pimsmIntfBuildDefaultConfigData(pimsmCfgCkt_t *pCfg);



/*********************************************************************
* @purpose  To get the mapping layer CB based on family Type
*
* @param    familyType    @b{(input)} Address Family type.
* @param    pimsmMapCbPtr @b{(inout)} Mapping Control Block.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCtrlBlockGet(L7_uchar8 familyType, 
                             pimsmMapCB_t **pimsmMapCbPtr);

L7_RC_t pimsmMapCfgSetEventQueue(pimsmMapCB_t *pimsmMapCbPtr, 
            PIMSMMAP_CFG_SET_EVENT_ID_t event, L7_VOIDPTR pMsg, L7_uint32 msgLen);

/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    pimsmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimsmCnfgrInitPhase1DynamicProcess (pimsmMapCB_t* pimsmMapCb);
L7_RC_t pimsmMapMemoryInit (pimsmMapCB_t* pimsmMapCb);
L7_RC_t pimsmMapMemoryDeInit (pimsmMapCB_t* pimsmMapCb);

/*********************************************************************
* @purpose  This function De-allocates the memory for the Vendor
*           Protocol upon its Disable.
*
* @param    pimsmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimsmCnfgrFiniPhase1DynamicProcess (pimsmMapCB_t* pimsmMapCb);



/*********************************************************************
* @purpose  Wrapper function to send the the message on the appropriate Q
*
* @param    QIndex    @b{(input)} Index to the Queue on which message is to be sent.
* @param    msg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
**********************************************************************/


L7_RC_t pimsmMessageSend(PIMSM_QUEUE_ID_t QIndex,void *msg);


/*********************************************************************
* @purpose  Function to delete the Queue
*
* @param    None
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
**********************************************************************/
L7_RC_t pimsmMapQDelete(void);


/**********************DEBUG TRACE ROUTINE PROTOTYPES ***************/

/*********************************************************************
 * @purpose  Saves pimsm configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    pimsmDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t pimsmDebugSave(void);
/*********************************************************************
 * @purpose  Restores pimsm debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    pimsmDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t pimsmDebugRestore(void);
/*********************************************************************
 * @purpose  Checks if pimsm debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL pimsmDebugHasDataChanged(void);

/*********************************************************************
 * @purpose  Build default pimsm config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void pimsmDebugBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
 * @purpose  Apply pimsm debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t pimsmApplyDebugConfigData(void);
                      
#endif /* _PIMSM_MAP_H_ */
