/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_map.h
*
* @purpose    PIMDM Mapping layer internal function prototypes
*
* @component  PIMDM Mapping Layer
*
* @comments   none
*
* @create     03/28/2006
*
* @author     gkiran
* @end
*
**********************************************************************/
#ifndef _PIMDM_MAP_H_
#define _PIMDM_MAP_H_

#include "l7_cnfgr_api.h"
#include "l7_mgmd_api.h"
#include "mfc_api.h"
#include "mcast_api.h"
#include "pim_defs.h"
#include "pimdm_map_util.h"
#include "pimdm_defs.h"
#include "l7_mcast_api.h"

/*-------------------------------------------------------------*/
/*  Queue-1 : Apptimer                                         */
/*-------------------------------------------------------------*/
#define PIMDMMAP_APP_TMR_QUEUE     "pimdmMapAppTmrQueue" 
#define PIMDMMAP_APP_TMR_MSG_COUNT  MCAST_APPTIMER_QUEUE_MSG_COUNT
#define PIMDMMAP_APP_TMR_MSG_SIZE   sizeof(pimdmMapAppTmrMsg_t)

/*-------------------------------------------------------------*/
/*  Queue-2 : All mapping Events +  MGMD events                */
/*-------------------------------------------------------------*/
#define PIMDMMAP_QUEUE     "pimdmMapEventsQueue" 
#define PIMDMMAP_MSG_COUNT  (L7_RTR_MAX_RTR_INTERFACES + L7_MGMD_GROUPS_MAX_ENTRIES + ((L7_MGMD_GROUPS_MAX_ENTRIES*60)/100))
#define PIMDMMAP_MSG_SIZE   sizeof(pimdmMapMsg_t)

/* Reserving 60% buffer space of the Events Queue for MGMD events */
#define PIMDMMAP_MGMD_EVENTS_COUNT_LIMIT ((PIMDMMAP_MSG_COUNT * 60)/100)

/*-------------------------------------------------------------*/
/*  Queue-3 : All ctrl pkt events + MFC events                 */
/*-------------------------------------------------------------*/
#define PIMDMMAP_CTRL_PKT_QUEUE     "pimdmMapCtrlPktQueue" 
#define PIMDMMAP_CTRL_PKT_MSG_COUNT  (L7_MAX_NETWORK_BUFF_PER_BOX +  L7_MULTICAST_FIB_MAX_ENTRIES)
#define PIMDMMAP_CTRL_PKT_MSG_SIZE   sizeof(pimdmMapCtrlPktAndMfcMsg_t)

/* Reserving 60% buffer space of the Events Queue for MFC events */
#define PIMDMMAP_MFC_EVENTS_COUNT_LIMIT ((PIMDMMAP_CTRL_PKT_MSG_COUNT * 60)/100)


typedef enum
{
  PIMDMMAP_CNFGR_MSG = 1,
  PIMDMMAP_INTF_CHANGE_MSG,
  PIMDMMAP_ROUTING_EVENT_MSG,
  PIMDMMAP_MCAST_EVENT_MSG,
  PIMDMMAP_RTR6_EVENT_MSG,
  PIMDMMAP_PROTOCOL_EVENT_MSG,
  PIMDMMAP_UI_EVENT_MSG
} pimdmMapMessages_t;

typedef enum
{
  PIMDMMAP_ADMIN_MODE_SET = 1,
  PIMDMMAP_INTF_MODE_SET,
  PIMDMMAP_HELLO_INTVL_SET
} PIMDMMAP_UI_EVENT_TYPE_t;

typedef struct pimdmEventMsg_s
{
  /* Union of all Events that PIM-DM can receive */
  union
  {
    mfcEntry_t            mfcInfo;       /* MFC Event Info */
    mgmdMrpEventInfo_t    mgmdInfo;      /* MGMD Event Info */
    mcastAdminMsgInfo_t   asbInfo;       /* AdminScope Boundary Event Info */
    mcastRPFInfo_t        rpfInfo;       /* Static MRoute Event Info */
  }u;
}pimdmEventMsg_t;

typedef struct pimdmMapIntfChangeParms_s 
{
  L7_uint32        event;
  L7_uint32        intIfNum;
  NIM_CORRELATOR_t correlator;
} pimdmMapIntfChangeParms_t;

typedef struct pimdmMapRoutingEventParms_s 
{
  L7_uint32                  event;
  L7_uint32                  intIfNum;
  void                      *pData;
  L7_BOOL                    asyncResponseRequired;
  ASYNC_EVENT_NOTIFY_INFO_t  eventInfo;
} pimdmMapRoutingEventParms_t;

typedef struct pimdmMapMcastEventParms_s 
{
  L7_uint32                  event;
  L7_uint32                  intIfNum;
  void                      *pData;
  ASYNC_EVENT_NOTIFY_INFO_t  eventInfo;
} pimdmMapMcastEventParms_t;

typedef struct pimdmMapProtocolEventParms_s 
{
  L7_uint32                  event;
  L7_uchar8                  familyType;
  pimdmEventMsg_t            msgData;
} pimdmMapProtocolEventParms_t;

typedef struct pimdmMapCtrlPktEventParms_s 
{
  L7_uint32                  event;
  L7_uchar8                  familyType;
  union
  {
  mcastControlPkt_t          msgData;   /*  Control Pkt Reception Event Info */
    mfcEntry_t               mfcInfo;   /* MFC Event Info */
  }u;
} pimdmMapCtrlPktAndMfcEventParms_t;

typedef struct pimdmMapAppTmrEventParms_s 
{
  L7_uint32                  event;
  L7_uchar8                  familyType;
} pimdmMapAppTmrEventParms_t;

typedef struct pimdmMapUiAdminMode_s
{
  L7_uint32 mode;
}pimdmMapUiAdminMode_t;

typedef struct pimdmMapUiIntfMode_s
{
  L7_uint32 intIfNum;
  L7_uint32 mode;
}pimdmMapUiIntfMode_t;

typedef struct pimdmMapUiHelloIntvl_s
{
  L7_uint32 intIfNum;
  L7_uint32 interval;
}pimdmMapUiHelloIntvl_t;

typedef struct pimdmMapUIEventParms_s 
{
  L7_uint32 event;
  MCAST_CB_HNDL_t cbHandle;
  union
  {
    pimdmMapUiAdminMode_t  adminMode;
    pimdmMapUiIntfMode_t   intfMode;
    pimdmMapUiHelloIntvl_t helloIntvl;
  }eventInfo;
} pimdmMapUIEventParms_t;

typedef struct
{
  L7_uint32   msgId;    
  union 
  {
    L7_CNFGR_CMD_DATA_t          cnfgrCmdData;
    pimdmMapIntfChangeParms_t    pimdmMapIntfChangeParms;
    pimdmMapRoutingEventParms_t  pimdmMapRoutingEventParms;
    pimdmMapMcastEventParms_t    pimdmMapMcastEventParms;
    pimdmMapProtocolEventParms_t pimdmMapProtocolEventParms;
    pimdmMapUIEventParms_t       pimdmMapUiEventParms;
  } u;
} pimdmMapMsg_t;

typedef struct
{
  L7_uint32   msgId;
  pimdmMapCtrlPktAndMfcEventParms_t     pimdmMapCtrlPkAndMfctParms;
} pimdmMapCtrlPktAndMfcMsg_t;

typedef struct
{
  L7_uint32   msgId;
  pimdmMapAppTmrEventParms_t     pimdmMapAppTmrParms;
} pimdmMapAppTmrMsg_t;

/*
**********************************************************************
*                    FUNCTION PROTOTYPES
**********************************************************************
*/

/* pimdmmap.c */

/*********************************************************************
*
* @purpose task to handle all PIM-DM Mapping messages
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*
*********************************************************************/
void pimdmMapTask();

/*********************************************************************
* @purpose  Save PIMDM user config file to NVStore
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmSave(void);

/*********************************************************************
* @purpose  Restore PIMDM user config file to factory defaults
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmRestore(void);

/*********************************************************************
* @purpose  Check if PIMDM user config data has changed
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
L7_BOOL pimdmHasDataChanged(void);
void pimdmResetDataChanged(void);
/*********************************************************************
* @purpose  Check if PIMDM V4 user config data has changed
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
L7_BOOL pimdmV4HasDataChanged(void);

/*********************************************************************
* @purpose  Build default PIMDM config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmBuildDefaultConfigData(L7_uint32 ver);
/*********************************************************************
* @purpose  Build default PIMDM config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdm6BuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Apply PIMDM config data
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS  Config data applied, or not a PIMDM interface
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmApplyConfigData(pimdmMapCB_t *pimdmMapCbPtr);

/*********************************************************************
* @purpose  Propogate Routing Event notifications to PIM-DM Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimdmMapRoutingEventChangeCallBack(L7_uint32 intIfNum, 
                                       L7_uint32 event, 
                                       void *pData, 
                                       ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

/*********************************************************************
* @purpose  Propogate Routing Event notifications to PIM-DM6 Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimdm6MapRoutingEventChangeCallBack(L7_uint32 intIfNum, 
                                         L7_uint32 event, 
                                         void *pData, 
                                         ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

/*********************************************************************
* @purpose  Propogate MCAST Event notifications to PIM-DM Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimdmMapMcastEventCallBack(L7_uint32 event, 
                                   void *pData, 
                                   ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
/*********************************************************************
* @purpose  Propogate Interface notifications to PIM-DM Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    correlator  @b{(input)} Correlator for event
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
                                   NIM_CORRELATOR_t correlator);
/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    pimdmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrInitPhase1DynamicProcess (pimdmMapCB_t* pimdmMapCb);
L7_RC_t pimdmMapMemoryInit(pimdmMapCB_t* pimdmMapCb);
L7_RC_t pimdmMapMemoryDeInit(pimdmMapCB_t* pimdmMapCb);
/*********************************************************************
* @purpose  This function De-allocates the memory for the Vendor
*           Protocol upon its Disable.
*
* @param    pimdmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrFiniPhase1DynamicProcess (pimdmMapCB_t* pimdmMapCb);

/*********************************************************************
* @purpose  Build default PIMDM config data for an intf 
*
* @param    pCfg     @b{(output)} pointer to the PIMDM ckt 
*                                 configuration record.
*
* @returns  none
*
* @comments none 
*
* @end
*********************************************************************/
void pimdmIntfBuildDefaultConfigData(pimdmCfgCkt_t *pCfg);

/*********************************************************************
* @purpose  Build default PIMDM config data for an intf 
*
* @param    timerCtrlBlk @b{(input)}  pointer to the PIMDM ckt 
*                                      configuration record.
* @param    param        @b{(input)}  pointer  to param
*
* @returns  void
*
* @comments none 
*
* @end
*********************************************************************/
void pimdmMapAppTimerExpiryHandler (L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, 
                                                          void *param);

/*********************************************************************
* @purpose  To send the event to Message Queue
*
* @param    familyType  @b{(input)} address family.
* @param    eventyType  @b{(input)} Event Type
* @param    msgLen      @b{(input)} Message Length.
* @param    pMsg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapMessageQueueSend(L7_uchar8 familyType, L7_uint32 eventType,
                                 L7_uint32 msgLen, L7_VOIDPTR pMsg);
/*********************************************************************
* @purpose  To Add/Delete the Multicast address in the hardware table.
*
* @param    familyType - L7_AF_INET/L7_AF_INET6
* @param    intIfNum   - Interface Number
* @param    inetAddr   - Address to be added
* @param    actionFlag - TRUE/FALSE
*
* @returns  mode     @b{(output)} L7_SUCCESS or L7_FAILURE
*
* @comments 
*
*
* @end
*********************************************************************/
L7_RC_t pimdmMapLocalMulticastAddrAddOrDelete(L7_uint32 familyType,L7_uint32 intIfNum,
                                      L7_inet_addr_t *inetAddr,L7_uchar8 actionFlag);

L7_RC_t pimdmMapUIEventSend (pimdmMapCB_t *pimdmMapCbPtr,
                             PIMDMMAP_UI_EVENT_TYPE_t eventType, 
                             L7_uint32 mode,
                             L7_uint32 intIfNum,
                             L7_uint32 helloIntvl);

L7_RC_t pimdmMapUIEventChangeProcess (MCAST_CB_HNDL_t cbHandle,
                                      PIMDMMAP_UI_EVENT_TYPE_t event, 
                                      void *pData);


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

L7_RC_t pimdmMessageSend(PIMDM_QUEUE_ID_t QIndex,void *msg);

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

L7_RC_t pimdmMapQDelete(void);


/**********************DEBUG TRACE ROUTINE PROTOTYPES ***************/

/*********************************************************************
 * @purpose  Saves pimdm configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    pimdmDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t pimdmDebugSave(void);
/*********************************************************************
 * @purpose  Restores pimdm debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    pimdmDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t pimdmDebugRestore(void);
/*********************************************************************
 * @purpose  Checks if pimdm debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL pimdmDebugHasDataChanged(void);

/*********************************************************************
 * @purpose  Build default pimdm config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void pimdmDebugBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
 * @purpose  Apply pimdm debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t pimdmApplyDebugConfigData(void);

#endif /* _PIMDM_MAP_H_ */
