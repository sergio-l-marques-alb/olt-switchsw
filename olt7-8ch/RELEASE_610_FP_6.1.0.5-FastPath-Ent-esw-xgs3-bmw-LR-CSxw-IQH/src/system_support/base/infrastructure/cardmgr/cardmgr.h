/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename cardmgr.h
 *
 * @purpose Externs for Card Manager
 *
 * @component Card Manager
 *
 * @comments none
 *
 * @create 11/22/2002
 *
 * @author kmcdowell
 * @end
 *
 **********************************************************************/
#ifndef CARD_MANAGER_H
#define CARD_MANAGER_H

#include "l7_cnfgr_api.h"
#include "trap_inventory_api.h"
#include "cardmgr_api.h"
#include "sysapi_hpc_chassis.h"
#include "chassis_alarmmgr_api.h"

/* Plataform values */
#define CMGR_MAX_SLOTS_f (platSlotTotalMaxPerUnitGet() + \
    platPowerModulesTotalMaxPerUnitGet() + \
    platFanModulesTotalMaxPerUnitGet()) 

#define CMGR_MAX_UNITS_f platUnitTotalMaxPerStackGet()

#define CMGR_MAX_LMS_PER_UNIT_f platLineModulesTotalMaxPerUnitGet()
#define CMGR_MAX_PMS_PER_UNIT_f platPowerModulesTotalMaxPerUnitGet()
#define CMGR_MAX_FMS_PER_UNIT_f platFanModulesTotalMaxPerUnitGet()

/* Structural values */
/* -- Card Manager Protocol */
#define L7_CMGR_TX_SEC_f          cmgrSidTxSecGet()
#define L7_CMGR_INIT_RX_SEC_f     cmgrSidInitRxSecGet()
#define L7_CMGR_TIMER_INTERVAL_f  cmgrSidTimerIntervalGet()
/* -- Card Manager Message Queue */
#define L7_CMGR_MSGQ_COUNT_f      cmgrSidMsgQCountGet()
#define L7_CMGR_MSGQ_SIZE_f       cmgrSidMsgQSizeGet()
/* -- Card Manager Task */
#define L7_CMGR_TASK_SYNC_f       cmgrSidTaskSyncGet()
#define L7_CMGR_TASK_PRIORITY_f   cmgrSidTaskPriorityGet()
#define L7_CMGR_TASK_SLICE_f      cmgrSidTaskSliceGet()
#define L7_CMGR_TASK_STACK_SIZE_f cmgrSidTaskStackSizeGet()


#define L7_CMGR_TASK_NAME "cmgrTask"
#define L7_CMGR_QUEUE_NAME "cmgrQueue"
#define L7_CMGR_CARD_INSERT_REMOVE_TASK_NAME "cmgrInsertTask"
#define L7_CMGR_CARD_INSERT_REMOVE_QUEUE_NAME "cmgrInsertQueue"

/* Configuration file defines.
 */
#define CMGR_CFG_VER_1                  0x1
#define CMGR_CFG_VER_2                  0x2
#define CMGR_CFG_VER_3                  0x3
#define CMGR_CFG_VER_CURRENT    CMGR_CFG_VER_3
#define CMGR_CFG_FILENAME               "cmgr.cfg"

/* Indicates that card ID has not been configured or inserted.
 */
#define L7_CMGR_CARD_ID_NONE            0

#define CMGR_ALARM_DESC_SIZE      100

extern L7_BOOL cmgrLogFlag_g;

#define CMGR_LOG_FLAG       cmgrLogFlag_g

#define CMGR_DEBUG(flags, format, args...)        \
{                                                 \
  if (flags) sysapiPrintf(format, ##args);         \
}

/* Card Manager states */
typedef L7_CNFGR_STATE_t L7_cmgr_state_t;


/* Data Structures */
typedef struct cmgr_slot_s
{   
  /* slot status */
  L7_uint32 slotId;        
  L7_uint32 phySlotId;        
  L7_BOOL   isFull;  /* this slot is filled with a card */
  L7_BOOL   isValid; /* this slot number is valid for this unit type */

  /* Card Manager notified application that port is created.
   */
  L7_BOOL       portCreateNotified; 

  /* Card Manager notified application that port is plugged in.
   */
  L7_BOOL   portInsertNotified;

  /* Card Manager notified applications that card is created.
   */
  L7_BOOL   cardCreateNotified;

  /* Card Manager notified applications that card is plugged in.
   */
  L7_BOOL   cardInsertNotified;

  /* slot configured info */
  L7_uint32 adminMode;
  L7_uint32 powerMode;
  /* card status */
  L7_CARD_STATUS_TYPES_t cardStatus;

  /* module index */
  SYSAPI_CARD_INDEX_t cardIndex;

  /* card type */
  SYSAPI_CARD_TYPE_t cardType;

  /* Card which is currently ionserted in the slot.
   ** 0 - No inserted card.
   */
  L7_uint32 insertedCardId;

  /* Card which is currently configured in the slot.
   ** 0 - Card has not been configured.
   */
  L7_uint32 configuredCardId;

} cmgr_slot_t;

typedef struct cmgr_unit_s
{
  L7_BOOL unitConnected;
  L7_BOOL unitConfigured;
  L7_uint32 unitType;
  L7_uint32 maxSlotsOnUnit;
  cmgr_slot_t* cmSlot;
} cmgr_unit_t;        


/* NVStore data */

typedef struct cmgr_card_config_s
{
  L7_uint32 card_id;   /* Configured card identifier */
  L7_uint32 adminMode; /* Configured administrative mode */
  L7_uint32 powerMode; /* Power mode */
} cmgr_card_config_t;

typedef struct L7_cardMgrCfgParms_s
{
  cmgr_card_config_t   configCard[L7_MAX_UNITS_PER_STACK+1]
    [L7_MAX_SLOTS_PER_UNIT + 
    L7_MAX_POWER_MODULES_PER_UNIT + 
    L7_MAX_FAN_MODULES_PER_UNIT+1];
} L7_cardMgrCfgParms_t;

typedef struct L7_cardMgrCfgFileData_s
{
  L7_fileHdr_t                  cfgHdr;
  L7_cardMgrCfgParms_t          cfgParms;   /* raw TC config parms */
  L7_uint32                     checkSum;
} L7_cardMgrCfgFileData_t;


/****************************************
 *
 *  Card Manager Registrar Data                    
 *
 *****************************************/
typedef struct
{
  L7_COMPONENT_IDS_t registrar_ID; 
  L7_uint32 (*notify_card_create)(L7_uint32 unit, 
      L7_uint32 slot, 
      L7_uint32 cardId);  
  L7_uint32 (*notify_card_clear)(L7_uint32 unit, 
      L7_uint32 slot);  
  L7_RC_t (*notify_card_plugin)(L7_uint32 unit, L7_uint32 slot, L7_uint32 cardId);  
  L7_RC_t (*notify_card_unplug)(L7_uint32 unit, L7_uint32 slot);
  L7_RC_t (*notify_port_change)(L7_uint32 unit, L7_uint32 slot, L7_uint32 port,
      L7_uint32 cardType, L7_PORT_EVENTS_t event, SYSAPI_HPC_PORT_DESCRIPTOR_t *portData);  
  void (*notify_trap_event)(L7_uint32 unit, L7_uint32 slot, 
      L7_uint32 ins_cardTypeId, L7_uint32 cfg_cardTypeId, 
      trapMgrNotifyEvents_t event);
} cmgrNotifyList_t;


/*************
 ** Message between card manager task and Card Insert/Remove task.
 *************/

#define L7_CMGR_CARD_INSERT 1
#define L7_CMGR_CARD_REMOVE 2
#define L7_CMGR_CARD_SYNC 3
#define L7_CMGR_CARD_CREATE 4
#define L7_CMGR_CARD_DELETE 5
#define L7_CMGR_LOCAL_UNIT_IS_MANAGER 6
#define L7_CMGR_LOCAL_UNIT_IS_NOT_MANAGER 7
#define L7_CMGR_CARD_CREATE_COMPLETE 8
#define L7_CMGR_CARD_CONFIG_SLOTS 9
#define L7_CMGR_CARD_CONFIG_PORTS 10
#define L7_CMGR_UNIT_CONNECTED 11
#define L7_CMGR_UNIT_DISCONNECTED 12

typedef struct 
{
  L7_uint32 action; /* Insert/Remove */

  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 cardId;

}cmgrCardInsertRemoveMsg_t;



/*****************************************************************************/
/*  Card Manager Functions                                                    */
/*****************************************************************************/


/*********************************************************************
 *
 * @purpose  Receive a Card Plug in event
 *
 * @param    cmgr_cmpdu_t* cmpdu  Card Manager Protocol packet
 *                            
 * @returns  
 *
 * @notes    This API is called when a card is plugged in to the 
 *           local unit. 
 *
 * @end
 *********************************************************************/
void cmgrReceiveCardPlugIn(cmgr_cmpdu_t *cmpdu);

/*********************************************************************
 *
 * @purpose  Process a Card Plug in event on the Management unit
 *
 * @param    L7_uint32 unit         unit  
 * @param    L7_uint32 slot         slot being plugged in  
 * @param    L7_uint32 cardType     Type of card plugged in  
 * @param    L7_uint32 cardStatus   Status of card plugged in  
 *                            
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes    This API is used to update the Application of a card
 *           plug in event.  It gets called if a local card was plugged
 *           in or if a CMPDU status message from a remote unit indicates
 *           that a card was plugged in on the remote unit. 
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrProcessCardPlugIn(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardTypeId, L7_uint32 cardStatus, 
    SYSAPI_CARD_TYPE_t cardType, 
    SYSAPI_CARD_INDEX_t moduleIndex);

/*********************************************************************
 *
 * @purpose  Receive a Card failure event
 *
 * @param    cmgr_cmpdu_t* cmpdu  Card Manager Protocol packet
 *                            
 * @returns  
 *
 * @notes    This API is called when a card is failed from the 
 *           local unit. 
 *
 * @end
 *********************************************************************/
void cmgrReceiveCardFailure(cmgr_cmpdu_t *cmpdu);

/*********************************************************************
 *
 * @purpose  Process a Card Failure event on the Management unit
 *
 * @param    L7_uint32 unit         unit
 * @param    L7_uint32 slot         slot being unplugged
 * @param    L7_uint32 cardStatus   Status of card unplugged
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes    This API is used to update the Application of a card
 *           failure event.  It gets called if a local card fails
 *           or if a CMPDU status message from a remote unit indicates
 *           that a card has failed on the remote unit.
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrProcessCardFailure(L7_uint32 unit, L7_uint32 slot, L7_uint32 cardStatus,
    SYSAPI_CARD_TYPE_t cardType, SYSAPI_CARD_INDEX_t moduleIndex);

/*********************************************************************
 *
 * @purpose raise alarms related with Card Manager
 *
 * @param  SYSAPI_CARD_TYPE_t cardType,  (input)
 *         L7_ALARM_TYPE_t alarmType,    (input)
 *         SYSAPI_CARD_INDEX_t cardIndex (input)
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Card Manager to
 *        raise an alarm
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrAlarmRaise (SYSAPI_CARD_TYPE_t cardType, 
    L7_ALARM_TYPE_t alarmType,
    SYSAPI_CARD_INDEX_t cardIndex);

/*********************************************************************
 *
 * @purpose clear alarms related with Card Manager
 *
 * @param  SYSAPI_CARD_TYPE_t cardType,  (input)
 *         L7_ALARM_TYPE_t alarmType,    (input)
 *         SYSAPI_CARD_INDEX_t cardIndex (input)
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Card Manager to
 *        clear alarm
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrAlarmClear (SYSAPI_CARD_TYPE_t cardType, 
    L7_ALARM_TYPE_t alarmType, 
    SYSAPI_CARD_INDEX_t cardIndex);

/*********************************************************************
 *
 * @purpose This function triggers Code update through CDA in case of 
 *          Code version mismatch in LM.
 *
 * @param  unit, slot, cardType, cardIndex
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_FAILURE,     if parameters are invalid or other error
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrLMImageVersionUpdate(L7_uint32 unit, L7_uint32 slot, 
    SYSAPI_CARD_TYPE_t cardType, L7_int32 cardIndex);

/*********************************************************************
 * @purpose  Database Access - get card type.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  card type
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBCardTypeGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - get module index.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  module index
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBModuleIndexGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set card type.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   card type
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardTypeSet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardType);

/*********************************************************************
 * @purpose  Database Access - set module index.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   module index
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBModuleIndexSet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 moduleIndex);

/*********************************************************************
 *
 * @purpose  Request a Status Update CMPDU from a unit
 *
 * @param    L7_uint32 unit         unit  
 *                            
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes    This API is used to send a request to a unit to resend
 *           the Status Update Message. 
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrSendRequestForStatusUpdate(L7_uint32 unit);

/*********************************************************************
 *
 * @purpose  Receive a Card unplug event
 *
 * @param    cmgr_cmpdu_t* cmpdu  Card Manager Protocol packet
 *                            
 * @returns  
 *
 * @notes    This API is called when a card is unplugged from the 
 *           local unit. 
 *
 * @end
 *********************************************************************/
void cmgrReceiveCardUnplug(cmgr_cmpdu_t *cmpdu);

/*********************************************************************
 *
 * @purpose  Process a Card unplug event on the Management unit
 *
 * @param    L7_uint32 unit         unit  
 * @param    L7_uint32 slot         slot being unplugged   
 * @param    L7_uint32 cardStatus   Status of card unplugged   
 *                            
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes    This API is used to update the Application of a card
 *           unplug event.  It gets called if a local card was unplugged
 *           or if a CMPDU status message from a remote unit indicates
 *           that a card was unplugged on the remote unit. 
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrProcessCardUnplug(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardStatus, SYSAPI_CARD_TYPE_t cardType,
    SYSAPI_CARD_INDEX_t moduleIndex);

/*********************************************************************
 *
 * @purpose  Receive a status update request event
 *
 * @param    cmgr_cmpdu_t* cmpdu  Card Manager Protocol packet
 *                            
 * @returns  
 *
 * @notes    This API is called when the local unit receives a request
 *           to send a status update. 
 *
 * @end
 *********************************************************************/
void cmgrReceiveStatusUpdateRequest(cmgr_cmpdu_t* cmpdu);

/*********************************************************************
 *
 * @purpose  Send a status update cmpdu
 *
 * @param    L7_cmpdu_packet_t type  Reason for status update.
 *                            
 * @returns  
 *
 * @notes    This API is called when the unit sends a status update. 
 *
 * @end
 *********************************************************************/
void cmgrSendStatusUpdate(L7_cmpdu_packet_t type);

/*********************************************************************
 *
 * @purpose  Receive a status update cmpdu
 *
 * @param    L7_cmpdu_packet_t type  Reason for status update.
 *                            
 * @returns  
 *
 * @notes    This API is called when the unit sends a status update. 
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrReceiveStatusUpdate(cmgr_cmpdu_t* cmpdu);




/*****************************************************************************/
/*  Data Access Functions                                                    */
/*****************************************************************************/

/*********************************************************************
 * @purpose  Database Access - get unit connected.
 *
 * @param    unit    unit
 *
 * @returns  boolean True if unit connected, false otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBUnitConnectedGet(L7_uint32 unit);

/*********************************************************************
 * @purpose  Database Access - set unit connected.
 *
 * @param    unit    unit
 * @param    value   L7_TRUE if unit connected, L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBUnitConnectedSet(L7_uint32 unit, L7_BOOL unitConnected);

/*********************************************************************
 * @purpose  Database Access - get unit configured.
 *
 * @param    unit    unit
 *
 * @returns  L7_BOOL L7_TRUE if unit configured; L7_FALSE otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBUnitConfiguredGet(L7_uint32 unit);

/*********************************************************************
 * @purpose  Database Access - set unit configured.
 *
 * @param    unit    unit
 * @param    value   L7_TRUE if unit configured, L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBUnitConfiguredSet(L7_uint32 unit, L7_BOOL unitConfigured);

/*********************************************************************
 * @purpose  Database Access - get unit type.
 *
 * @param    unit    unit
 *
 * @returns  L7_uint32 unit type.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBUnitTypeGet(L7_uint32 unit);

/*********************************************************************
 * @purpose  Database Access - set unit unit type.
 *
 * @param    unit    unit
 * @param    value   unit type
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBUnitTypeSet(L7_uint32 unit, L7_uint32 unitType);

/*********************************************************************
 * @purpose  Database Access - get unit maximum slots.
 *
 * @param    unit    unit
 *
 * @returns  L7_uint32  max slots for this unit.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBUnitMaxSlotsGet(L7_uint32 unit);

/*********************************************************************
 * @purpose  Database Access - set unit max slots.
 *
 * @param    unit    unit
 * @param    value   maximum number of slots available for this unit
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBUnitMaxSlotsSet(L7_uint32 unit, L7_uint32 maxSlots);

/*********************************************************************
 * @purpose  Database Access - get slot id.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  slot id for this slot on this unit.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBSlotIdGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set slot id.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   slot ID value
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBSlotIdSet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 slotId);

/*********************************************************************
 * @purpose  Database Access - get physical slot id.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  physical slot id for this slot on this unit.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBPhySlotIdGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set physical slot id.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   slot ID value
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBPhySlotIdSet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 slotId);

/*********************************************************************
 * @purpose  Database Access - get slot validity.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if this is a valid slot on this unit;
 *                    L7_FALSE otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBSlotIsValid(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - get slot full status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if a card is inserted in this slot on 
 *                    this unit; L7_FALSE otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBSlotIsFull(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set slot slot full status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   L7_TRUE if slot is full; L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBSlotIsFullSet(L7_uint32 unit, L7_uint32 slot, 
    L7_BOOL isFull);


/*********************************************************************
 * @purpose  Database Access - get portCreateNotified  status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if a card is inserted in this slot on 
 *                    this unit; L7_FALSE otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBPortCreateNotifiedGet(L7_uint32 unit, L7_uint32 slot);


/*********************************************************************
 * @purpose  Database Access - set portCreateNotified status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   L7_TRUE if slot is full; L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBPortCreateNotifiedSet(L7_uint32 unit, L7_uint32 slot, L7_BOOL value);

/*********************************************************************
 * @purpose  Database Access - get portInsertNotified  status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if a card is inserted in this slot on 
 *                    this unit; L7_FALSE otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBPortInsertNotifiedGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set portInsertNotified status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   L7_TRUE if slot is full; L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBPortInsertNotifiedSet(L7_uint32 unit, L7_uint32 slot, L7_BOOL value);

/*********************************************************************
 * @purpose  Database Access - get cardInsertNotified  status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if a card is inserted in this slot on 
 *                    this unit; L7_FALSE otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBCardInsertNotifiedGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set cardInsertNotified status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   L7_TRUE if slot is full; L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardInsertNotifiedSet(L7_uint32 unit, L7_uint32 slot, L7_BOOL value);

/*********************************************************************
 * @purpose  Database Access - get cardCreateNotified  status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if a card is inserted in this slot on 
 *                    this unit; L7_FALSE otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBCardCreateNotifiedGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set cardCreateNotified status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   L7_TRUE if slot is full; L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardCreateNotifiedSet(L7_uint32 unit, L7_uint32 slot, L7_BOOL value);

/*********************************************************************
 * @purpose  Database Access - get slot administrative mode.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  administrative mode.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBConfiguredAdminModeGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set slot admin mode.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   administrative mode value
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBSlotConfiguredAdminModeSet(L7_uint32 unit, 
    L7_uint32 slot, 
    L7_uint32 adminMode);

/*********************************************************************
 * @purpose  Database Access - get slot power mode.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  power mode.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBConfiguredPowerModeGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set slot power mode.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   slot power mode
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBSlotConfiguredPowerModeSet(L7_uint32 unit, 
    L7_uint32 slot, 
    L7_uint32 powerMode);

/*********************************************************************
 * @purpose  Set LM slot power - wrapper function for the HPC API
 *
 * @param    slot    slot
 * @param    status  status to be set
 * @param    souurce source system / administrator
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_FAILURE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrHpcPwrSlotPowerSet(L7_uint32 slotNum, POWER_STATUS_t status, 
    POWER_CONTROL_t source);

/*********************************************************************
 * @purpose  get LM slot power status - wrapper function for the HPC API
 *
 * @param    slot    slot
 * @param    status  status to be set
 * @param    source  source system / administrator
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_FAILURE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrHpcPwrSlotPowerStatusGet(L7_uint32 slotNum, POWER_STATUS_t *pStatus, 
    POWER_CONTROL_t *pSource);

/*********************************************************************
 *
 * @purpose Turn ON / OFF power to given LM slot
 *
 * @param  L7_uint32    slotNum       Slot number
 *         L7_BOOL      enable        L7_TRUE to switch ON
 *                                    L7_FALSE to switch OFF
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Card Manager to switch ON power
 *        to the given LM slot
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPowerSet (L7_uint32 unit, L7_uint32 slot, POWER_STATUS_t enable, 
    POWER_CONTROL_t source);

/*********************************************************************
 * @purpose  Database Access - get card status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  card status
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBCardStatusGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set card status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   card status
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardStatusSet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardStatus);

/*********************************************************************
 * @purpose  Database Access - get slot inserted card type.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  inserted card type
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBInsertedCardIdGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - set inserted card type.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   inserted card type value
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardInsertedCardIdSet(L7_uint32 unit, 
    L7_uint32 slot, 
    L7_uint32 insertedCardType);


/*********************************************************************
 * @purpose  Database Access - get slot configured card type.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  configured card type
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBConfigCardIdGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Database Access - Copy information from one unit record 
 *                                                         to another unit record.
 *
 * @param    src_unit Unit number from which to copy.
 * @param    dst_unit Unit number to which to copy.
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBUnitInfoCopy(L7_uint32 src_unit, L7_uint32 dst_unit);

/*********************************************************************
 * @purpose  Database Access - configured card identifier.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   configured card type value
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardConfigCardIdSet(L7_uint32 unit, 
    L7_uint32 slot, 
    L7_uint32 configuredCardId);

/*********************************************************************
 * @purpose  Database Access - get next slot from given unit and slot.
 *
 * @param    unit         unit
 * @param    slot         slot
 * @param    *nextSlot    next valid slot
 *
 * @returns  L7_RC_t  L7_SUCCESS if next slot is found; L7_ERROR
 *                    otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBSlotNextGet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 *nextSlot);


/*********************************************************************
 *
 * @purpose Get unit and slot of the LAG slot.
 *
 * @param  L7_uint32    unit      Unit ID of the logical unit
 * @param  L7_uint32    slot      Slot ID of the created port
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if parameters are invalid or other error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCMDBLagCardUSPGet (L7_uint32 *unit, L7_uint32 *slot);

/*********************************************************************
 *
 * @purpose Get unit and slot of the vlan card.
 *
 * @param  L7_uint32    unit      Unit ID of the logical unit
 * @param  L7_uint32    slot      Slot ID of the deleted port
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if parameters are invalid or other error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCMDBVlanCardUSPGet (L7_uint32 *unit, L7_uint32 *slot);

/*********************************************************************
 *
 * @purpose Unplug and unconfigure all cards.
 *
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if parameters are invalid or other error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
void cmgrAllCardsUnplugUnconfigure(void);

/*********************************************************************
 *
 * @purpose Send the pending notifications for the given unit and slot
 *           at the time of registration
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if parameters are invalid or other error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCMDBPluginSendPendingNotif( L7_COMPONENT_IDS_t registrar_ID, 
    L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 *
 * @purpose Map the component ID to card type Id.
 *
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if parameters are invalid or other error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrRegistrarId2CardTypeGet( L7_COMPONENT_IDS_t registrar_ID, 
    L7_uint32 *cardType);

/******************/
/* Util functions */
/******************/

/*********************************************************************
 *
 * @purpose  Create sync semaphore for port-create/attach/detach/delete
 *                       events. 
 *
 * @returns none 
 *
 * @notes    Card manager will wait on this semaphore for NIM to notify
 *                       it that all port processing is done.
 *
 * @end
 *********************************************************************/
void cmgrNimSyncSemCreate (void);

/*********************************************************************
 *
 * @purpose  Send Card create and Port create notifications
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardId
 *                            
 * @returns  
 *
 * @notes    This API is called when a card has been created.  It notifies
 *           registrants that the card and ports must be created. 
 *
 * @end
 *********************************************************************/
void cmgrCardCreateNotify(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardId);

/*********************************************************************
 *
 * @purpose  Send Card clear and Port delete notifications
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardId   card identifier
 *                            
 * @returns  
 *
 * @notes    This API is called when a card has been cleared.  It notifies
 *           registrants that the card and ports must be cleared. 
 *
 * @end
 *********************************************************************/
void cmgrAsyncCardClearNotify(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardId);

/*********************************************************************
 *
 * @purpose  Send Card clear and Port delete notifications
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardId   card identifier
 *                            
 * @returns  
 *
 * @notes    This API is called when a card has been cleared.  It notifies
 *           registrants that the card and ports must be cleared. 
 *
 * @end
 *********************************************************************/
void cmgrCardClearNotify(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardId);

/*********************************************************************
 *
 * @purpose  Send Card Create notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 *                            
 * @returns  
 *
 * @notes    This API is called to notify registrants when a card  
 *           has been pluggd. 
 *           The function enqueues the command to the card insert
 *           remove task.
 *
 * @end
 *********************************************************************/
void cmgrAsyncCardCreateNotify(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardId);


/*********************************************************************
 *
 * @purpose  Send Card plug-in notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 *                            
 * @returns  
 *
 * @notes    This API is called to notify registrants when a card  
 *           has been pluggd. 
 *           The function enqueues the command to the card insert
 *           remove task.
 *
 * @end
 *********************************************************************/
void cmgrAsyncCardPluginNotify(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardId);

/*********************************************************************
 *
 * @purpose  Send Card plugin notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardId   card identifier
 *                            
 * @returns  
 *
 * @notes    This API is called to notify registrants when a card  
 *           has been pluggd in. If possible, this should not be 
 *           called within the card manager database semaphore.
 *
 * @end
 *********************************************************************/
void cmgrCardPluginNotify(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardId);

/*********************************************************************
 *
 * @purpose  Send Card unplug notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 *                            
 * @returns  
 *
 * @notes    This API is called to notify registrants when a card  
 *           has been unpluggd. 
 *           The function enqueues the command to the card insert
 *           remove task.
 *
 * @end
 *********************************************************************/
void cmgrAsyncCardUnPlugNotify(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardTypeId);

void cmgrAsyncCardConfigSlots(void);
void cmgrAsyncCardConfigPorts(void);
/*********************************************************************
 *
 * @purpose  Send Card unplug notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 *                            
 * @returns  
 *
 * @notes    This API is called to notify registrants when a card  
 *           has been unpluggd.  If possible, this should not be 
 *           called within the card manager database semaphore.
 *
 * @end
 *********************************************************************/
void cmgrCardUnPlugNotify(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 cardId);

/*********************************************************************
 * @purpose  go through registered users and notify them of port change.
 *
 * @param    unit        unit
 * @param    slot        slot
 * @param    port        port
 * @param    cardType    type of card
 * @param    event       port create, delete, plugin, unplug
 * @param    portType    type of port
 *
 * @returns  L7_SUCCESS 
 * @returns  L7_ERROR
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrDoNotifyPortChange(L7_uint32 unit, L7_uint32 slot, L7_uint32 port, 
    L7_uint32 cardType, L7_PORT_EVENTS_t event,
    SYSAPI_HPC_PORT_DESCRIPTOR_t *portData);

/*********************************************************************
 * @purpose  Issue port create.
 *
 * @param    unit        unit
 * @param    slot        slot
 * @param    port        port
 * @param    cardType    type of card
 * @param    portType    type of port
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrDoNotifyPortCreate(L7_uint32 unit, 
    L7_uint32 slot, 
    L7_uint32 port,
    L7_uint32 cardType, 
    SYSAPI_HPC_PORT_DESCRIPTOR_t *portData);

/*********************************************************************
 * @purpose  go through registered users and notify them of card creation.
 *
 * @param    unit        unit
 * @param    slot        slot
 * @param    cardId
 *
 * @returns  void
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
void cmgrDoNotifyCardCreate(L7_uint32 unit, L7_uint32 slot, L7_uint32 cardId);

/*********************************************************************
 * @purpose  go through registered users and notify them of card clear.
 *
 * @param    unit        unit
 * @param    slot        slot
 *
 * @returns  void
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
void cmgrDoNotifyCardClear(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  go through registered users and notify them of card plugin.
 *
 * @param    unit        unit
 * @param    slot        slot
 * @param    cardId
 *
 * @returns  void
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
void cmgrDoNotifyCardPlugin(L7_uint32 unit, L7_uint32 slot, L7_uint32 cardId);

/*********************************************************************
 * @purpose  go through registered users and notify them of card unplug.
 *
 * @param    unit        unit
 * @param    slot        slot
 *
 * @returns  void
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
void cmgrDoNotifyCardUnplug(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 *
 * @purpose  Authenticate Card Type
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardType  card type
 *                            
 * @returns  
 *
 * @notes    This API is called to authenticate the card type in this
 *           unit in this slot. If additional authentication is 
 *           necessary, it can be added here.
 *
 * @end
 *********************************************************************/
L7_RC_t authenticateCardType(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardType);

/*********************************************************************
 *
 * @purpose  Authenticate Board Id
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardType  Board Id
 *
 * @returns
 *
 * @notes    This API is called to authenticate the board Id in this
 *           unit in this slot. In case of authentication failure
 *           of a Line Module, the power will be turned OFF immediately
 *           to that slot.
 *
 * @end
 *********************************************************************/
L7_RC_t authenticateBoardId(L7_uint32 unit, L7_uint32 phySlot, 
    L7_uint32 boardId, SYSAPI_CARD_TYPE_t cardType, 
    L7_int32 cardIndex);

/*********************************************************************
 *
 * @purpose  Card Unsupported
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardType  card type
 *                            
 * @returns  
 *
 * @notes    This API is sets the card status and calls trap manager if
 *           a card is plugged in whose type is unsupported on this unit.  
 *           If additional action needs to be taken in this event,
 *           it can be added here.
 *
 * @end
 *********************************************************************/
void cmgrCardUnsupported(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 cardType);

/*********************************************************************
 *
 * @purpose  Clear the Card Unsupported alarm
 *
 * @param    L7_uint32 phySlot   physical slot
 *
 * @returns  L7_SUCCESS          if success
 *           L7_FAILURE          if alarm could not be raised
 *
 * @notes    This API clears the cardUnsupported alarm on a successful
 *           card plug-in
 *           
 * @end
 *********************************************************************/
L7_RC_t cmgrCardUnSupportedAlarmClear(L7_uint32 phySlot);

/*********************************************************************
 *
 * @purpose  Card Mismatch
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 configCardType  Configured card type
 * @param    L7_uint32 insertedCardType  Inserted card type
 *                            
 * @returns  
 *
 * @notes    This API is sets the card status and calls trap manager if
 *           a card is plugged in whose type doesn't match the card type
 *           configured in this slot on this unit.  
 *           If additional action needs to be taken in this event,
 *           it can be added here.
 *
 * @end
 *********************************************************************/
void cmgrCardMisMatch(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 configCardType, 
    L7_uint32 insertedCardType);

/*********************************************************************
 *
 * @purpose  Send message using HPC transport
 *
 * @param    L7_uint32 pCmdData      Configurator data
 * @param    L7_uint32 cmpdu         cmpdu to return
 *                            
 * @returns  L7_RC_t L7_SUCCESS or L7_ERROR
 *
 * @notes    This function is used to send a message using HPC.
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrSendCmpdu(cmgr_cmpdu_t *cmpdu, L7_uint32 size);

/*********************************************************************
 *
 * @purpose  Create a CMPDU from Cnfgr data
 *
 * @param    L7_uint32 pCmdData      Configurator data
 * @param    L7_uint32 cmpdu         cmpdu to return
 *                            
 * @returns  L7_RC_t L7_SUCCESS or L7_ERROR
 *
 * @notes    This function is used to create a cmpdu to be sent to the
 *           card manager queue with data from the configuator.
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrMakeCnfgrCmpdu(L7_CNFGR_CMD_DATA_t *pCmdData, 
    cmgr_cmpdu_t *cmpdu);

/*********************************************************************
 * @purpose  Initialize Card Manager Database.
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
void cmgrDatabaseInit();

/*********************************************************************
 * @purpose  Initialize slots for a unit in the Card Manager Database.
 *
 * @param    unit    unit
 *
 * @returns  void
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrInitSlotsOnUnit(L7_uint32 unit);


/*********************************************************************
 * @purpose  Card Manager Timer - handles Card Manager timer events
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    This is called from the timer task and does minimal work.
 *           It forwards the message to the Card Manager task and resets
 *           the timer.
 *       
 * @end
 *********************************************************************/
void cmgrTimer( L7_uint32 arg1, L7_uint32 arg2 );

/*********************************************************************
 * @purpose  Card Manager Task
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
void cmgrCardInsertRemoveTask();

/*********************************************************************
 * @purpose  Card Manager Task
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
void cmgrTask();


/*********************************************************************
 * @purpose  Card Manager Save data function.
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrSave(void);

/*********************************************************************
 * @purpose  Build default card manager configuration.
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes    none 
 *
 * @end
 *********************************************************************/
void cmgrBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
 * @purpose  Is data changed?.
 *
 * @param    none
 *
 * @returns  boolean L7_TRUE if data has changed;  L7_FALSE otherwise.
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_BOOL cmgrHasDataChanged(void);
void cmgrResetDataChanged(void);

/*********************************************************************
 * @purpose  Dump configuration function.
 *
 * @param    none
 *
 * @returns  L7_RC_t 
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
L7_RC_t cmgrConfigDump(void);

/*********************************************************************
 *
 * @purpose  Send Unit Is Manager notification to the card insert task.
 *
 * @param    none
 *
 * @returns
 *
 * @notes    
 *
 * @end
 *********************************************************************/
void cmgrAsyncTaskLocalUnitIsManager(void);

/*********************************************************************
 *
 * @purpose  Send Unit Is Not Manager notification to the card insert task.
 *
 * @param    none
 *
 * @returns
 *
 * @notes  
 *
 * @end
 *********************************************************************/
void cmgrAsyncTaskLocalUnitIsNotManager(void);

/* cardmgr_migrate.c */
/*********************************************************************
 * @purpose  Migrate old config to new
 *
 * @param    oldVer      @b{(input)} version of old config definition
 * @param    ver         @b{(input)} version of current config definition
 * @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
 *
 * @returns  void
 *
 * @notes    This is the callback function provided to the sysapiCfgFileGet
 *           routine to handle cases where the config file is of an older
 *           version.
 *
 * @notes
 *
 * @end
 *********************************************************************/
void cardMgrMigrateConfigData ( L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

/*********************************************************************
 * @purpose  After sending CARD_INSERT, update LED status to indicate
 *           that Initialisation is complete
 * @param    none
 *
 * @returns  L7_RC_t rc
 *
 * @author   bguna
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrLedInitScmLm(L7_uint32 unit, L7_uint32 slot);


#endif /* CARD_MANATER_H */
