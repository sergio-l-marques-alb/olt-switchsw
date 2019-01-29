/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename cardmgr_api.h
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

#ifndef CARD_MANAGER_API_H
#define CARD_MANAGER_API_H

#include "sysnet_api.h"
#include "l7_cnfgr_api.h" 
#include "nimapi.h" 
#include "sysapi_hpc.h" 
#include "trap_inventory_api.h"

typedef enum {
  L7_CARD_STATUS_UNKNOWN = 1,
  L7_CARD_STATUS_UNPLUG,
  L7_CARD_STATUS_PLUG,
  L7_CARD_STATUS_WORKING,
  L7_CARD_STATUS_DIAGFAILED,
  L7_CARD_STATUS_DOWNLOADING,
  L7_CARD_STATUS_MISMATCH,
  L7_CARD_STATUS_UNSUPPORTED,
  L7_CARD_STATUS_FAILED,

  L7_CARD_STATUS_TOTAL /* number of enum entries */

} L7_CARD_STATUS_TYPES_t;

/* Protocol structures */
typedef enum
{
  L7_CMPDU_UNKNOWN = 0,
  L7_CMPDU_STATUS_UPDATE,
  L7_CMPDU_STATUS_REQUEST,
  L7_CMPDU_REQUEST_ALL_INFO,
  L7_CMPDU_CARD_REPORT_CARD_PLUGIN,
  L7_CMPDU_CARD_REPORT_CARD_UNPLUG,
  L7_CMPDU_CNFGR_REQUEST,
  L7_CMPDU_TIMER_TIMEOUT,
  L7_CMPDU_CARD_REPORT_CARD_FAILURE
} L7_cmpdu_packet_t;

/* Card Configuration Event Types */
typedef enum
{
  L7_CMGR_CONFIG_EVENT_CONFIG_SLOTS = 0,
  L7_CMGR_CONFIG_EVENT_CONFIG_PORTS,

  L7_CMGR_CONFIG_EVENT_TOTAL   /* number of enum entries */

} L7_CMGR_CONFIG_EVENT_TYPES_t;

typedef struct cmgr_cmpdu_statusUpdate_s
{
  L7_uint32 slotId;
  L7_uint32 phySlotId;
  L7_uint32 slotStatus;  /* empty/full */
  L7_uint32 insertedCardTypeId;
  L7_CARD_STATUS_TYPES_t cardStatus;
  SYSAPI_CARD_INDEX_t cardIndex;
  SYSAPI_CARD_TYPE_t cardType;

} cmgr_cmpdu_statusUpdate_t;

typedef struct cmgr_cmpdu_requestAllInfo_s
{
  L7_uint32 requestedUnit;
} cmgr_requestCmpdu_t;

typedef struct cmgr_cmpdu_s
{
 L7_COMPONENT_IDS_t componentId;
  L7_cmpdu_packet_t cmgrPacketType;   
  L7_BOOL       local;  /* This PDU is not sent on the wire */
  L7_uint32 unit;
  union
  {
    cmgr_cmpdu_statusUpdate_t statusUpdate;
    cmgr_requestCmpdu_t requestAll;
    L7_CNFGR_CMD_DATA_t cmdData;
  } cmpdu_data;
} cmgr_cmpdu_t;

/* Cmpdu db that is active on all the units in the stack */
typedef struct
{
  /* All the Slot information for a unit */
  cmgr_cmpdu_statusUpdate_t *slotInfo;

  /* This record is valid or not. */
  L7_BOOL                    valid;

} cmgr_unit_cmpdu_db_t;

/*********************************************************************
 * Initialization APIs
 *
 *********************************************************************/


/*********************************************************************
 *
 * @purpose  CNFGR Initialization for Card Manager component
 *
 * @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
 *                                             CNFGR request
 *                            
 * @returns  None
 *
 * @notes    This API is provided to allow the Configurator to issue a
 *           request to the Card Manager.  This function is re-entrant.
 *           The CNFGR request should be transfered to the Card Manager 
 *           task as quickly as possible to avoid processing in the 
 *           Configurator's task. 
 *
 * @notes    This function completes Configurator requests asynchronously. 
 *           If there is a failure, this function returns synchronously.
 *           The return value is presented to the configurator by calling the 
 *           cnfgrApiCallback(). The following are the possible return codes:   
 *           L7_SUCCESS - There were no errors. Response is available.
 *           L7_ERROR   - There were errors. Reason code is available.
 *
 * @notes    The following are valid response: 
 *           L7_CNFGR_CMD_COMPLETE 
 *
 * @notes    The following are valid error reason code:
 *           L7_CNFGR_CB_ERR_RC_INVALID_CMD
 *           L7_CNFGR_ERR_RC_FATAL
 *
 * @notes    This function runs in the configurator's thread. Care should be
 *           taken when blocking this thread.!
 *
 * @end
 *********************************************************************/
void cmgrApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);


/*********************************************************************
 * Register/Deregister for Callback APIs
 *
 *********************************************************************/

/*********************************************************************
 * @purpose  Register a routine to be called when a trap occurs.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)      
 * @param    notify         pointer to a routine to be invoked for card 
 *                          creation.  Each routine has the following parameters:
 *                          (unit, slot, ins_cardTypeId, cfg_cardTypeId, event). 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    One callback per registrar-ID.  
 *                                 
 * @end
 *********************************************************************/
L7_RC_t cmgrAllTrapsRegister( L7_COMPONENT_IDS_t registrar_ID, 
    void (*notify)(L7_uint32 unit,
      L7_uint32 slot,
      L7_uint32 ins_cardTypeId,
      L7_uint32 cfg_cardTypeId,
      trapMgrNotifyEvents_t event));

/*********************************************************************
 * @purpose  Notify registered users of a trap event
 *
 * @param    unit            the unit id
 * @param    slot            the slot id
 * @param    ins_cardTypeId  id of inserted card
 * @param    cfg_cardTypeId  id of configured card
 * @param    event           notification event (see trapMgrNotifyEvents_t)
 *                                       
 * @returns  Void
 *
 * @notes    none
 *       
 * @end
 *********************************************************************/
void cmgrNotifyRegisteredUsers(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 ins_cardTypeId, 
    L7_uint32 cfg_cardTypeId, 
    trapMgrNotifyEvents_t event);

/*********************************************************************
 * @purpose  deregister a routine to be called when a trap occurrs.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)      
 *
 * @returns  none 
 *
 * @notes    none
 *                                 
 * @end
 *********************************************************************/
void cmgrAllTrapsDeregister( L7_COMPONENT_IDS_t registrar_ID );

/*********************************************************************
 * @purpose  Register a routine to be called when a card is created.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)      
 * @param    *notify        pointer to a routine to be invoked for card 
 *                          creation.  Each routine has the following parameters:
 *                          (unit, slot, cardId). 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *                                 
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterCardCreate( L7_COMPONENT_IDS_t registrar_ID, 
    L7_uint32 (*notify)(L7_uint32 unit,
      L7_uint32 slot, 
      L7_uint32 cardId));

/*********************************************************************
 * @purpose  Register a routine to be called before and after creating slots
 *           to apply configuration
 *
 * @param    *notify        pointer to a routine to be invoked
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *                                 
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterCardConfig(void (*notify)(L7_CMGR_CONFIG_EVENT_TYPES_t ev));

/*********************************************************************
 * @purpose  Register a routine to be called when a card is cleared.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)      
 * @param    *notify        pointer to a routine to be invoked for card 
 *                          clear.  Each routine has the following parameters:
 *                          (unit, slot). 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *                                 
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterCardClear( L7_COMPONENT_IDS_t registrar_ID, 
                               L7_uint32 (*notify)(L7_uint32 unit, L7_uint32 slot));

/*********************************************************************
 * @purpose  Register a routine to be called when a card is plugged in.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)      
 * @param    *notify        pointer to a routine to be invoked for card 
 *                          plugin.  Each routine has the following parameters:
 *                          (unit, slot, cardType). 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *                                 
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterCardPlugin( L7_COMPONENT_IDS_t registrar_ID, 
    L7_RC_t (*notify)(L7_uint32 unit,
      L7_uint32 slot, 
      L7_uint32 cardId));

/*********************************************************************
 * @purpose  Register a routine to be called when a card is unplugged.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)      
 * @param    *notify        pointer to a routine to be invoked for card 
 *                          unplug.  Each routine has the following parameters:
 *                          (unit, slot). 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *                                 
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterCardUnplug( L7_COMPONENT_IDS_t registrar_ID, 
    L7_RC_t (*notify)(L7_uint32 unit,
      L7_uint32 slot));

/*********************************************************************
 * @purpose  Register a routine to be called when a port is created.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)      
 * @param    *notify        pointer to a routine to be invoked for port 
 *                          creation.  Each routine has the following parameters:
 *                          (unit, slot, port, cardType). 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *                                 
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterPortChange( L7_COMPONENT_IDS_t registrar_ID, 
    L7_RC_t (*notify)(L7_uint32 unit,
      L7_uint32 slot, 
      L7_uint32 port,
      L7_uint32 cardType,
      L7_PORT_EVENTS_t event,
      SYSAPI_HPC_PORT_DESCRIPTOR_t *portData));


/*********************************************************************
 * Callback APIs
 *
 *********************************************************************/

/*********************************************************************
 *
 * @purpose Callback function to handle card plug in and unplug
 *          messages from HPCL.
 *
 * @param  L7_uint32    slot              Slot ID of the changed card
 * @param  L7_uint32    *sysapiCardInfo   Card info structure
 * @param  L7_uint32    event             Card plugin or unplug
 *                                                  
 * @returns None. 
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void cmgrCardReportCallBack(L7_uint32 slot, 
    L7_uint32 cardTypeId, 
    hpcEvent_t event);

/*********************************************************************
 *
 * @purpose Callback function to handle Card Manager Protocol messages
 *
 * @param    src_key {(input)}  Key (mac-address) of the unit that sent the msg
 * @param    msg     {(input)}  The buffer holding the message
 * @param    msg_len {(input)}  The buffer length of the message
 *
 * @returns None.
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
void cmgrReceiveCMPDUCallback(L7_enetMacAddr_t src_key,
                              L7_uchar8* hpcCmpdu, L7_uint32 msgLen);


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
L7_RC_t cmgrLagCardUSPGet (L7_uint32 *unit, L7_uint32 *slot);

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
L7_RC_t cmgrVlanCardUSPGet (L7_uint32 *unit, L7_uint32 *slot);



/*********************************************************************
 * Unit Manager APIs
 *
 *********************************************************************/

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that a new unit has been configured
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    unitType   Unit Type for the unit
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Unit Manager to notify the 
 *        Card Manager that a unit has been configured.  This causes
 *        the unit and slots on the unit to be initialized.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrUnitConfig (L7_uint32 unit, L7_uint32 unitType);

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that a unit has been un-configured
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Unit Manager to notify the 
 *        Card Manager that a unit has been unconfigured.  This causes
 *        the unit and slots on the unit to be deleted.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrUnitUnconfig (L7_uint32 unit);

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that a unit has joined the stack.
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    unitType   Unit Type of the unit
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Unit Manager to notify the 
 *        Card Manager that a unit joined the stack.  This could happen
 *        if the communication link is established.    
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrUnitConnected (L7_uint32 unit);


/*********************************************************************
 *
 * @purpose On the Mgmt unit, Unit manager notifies driver that a
 *          unit has left the stack. This is done before the Card
 *          Remove events are issued and allows driver an opportunity
 *          to do some cleanup. 
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
void cmgrUnitFailureNotify (L7_uint32 unit);

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that a unit has left the stack.
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Unit Manager to notify the 
 *        Card Manager that a unit left the stack.  This could happen
 *        if the communication link is lost.  This causes the Card
 *        Manager to set all cards on the unit to be disabled.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrUnitDisconnected (L7_uint32 unit);

/*********************************************************************
 *
 * @purpose Tell CM to start/stop collecting cmpdu's in cmpdu db
 *
 * @param  unit     {(input)}   Unit ID of the unit
 * @param  collect  {(input)}   L7_TRUE: Start collecting cmpdu's.
 *                              L7_FALSE: Stop collecting cmpdu's.
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes UM notifies CM to start collecting cmpdu's when a unit has joined
 *        the stack. The stop collection is done when a unit has left 
 *        the stack.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrUnitCmpduCollect(L7_uint32 unit, L7_BOOL collect);

/*********************************************************************
 *
 * @purpose Retrieve the MAC address for an interface
 *
 * @param  type        @b{(input)}     interface type of the interface
 * @param  unit        @b{(input)}     unit id of the interface
 * @param  slot        @b{(input)}     slot id of the interface
 * @param  port        @b{(input)}     port id of the interface
 * @param  l2_mac_addr @b{(output)}    L2 MAC address for the interface
 * @param  l3_mac_addr @b{(output)}    L3 router MAC address for the interface

 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 unit, L7_uint32 slot, L7_uint32 port, L7_uchar8 *l2_mac_addr, L7_uchar8 *l3_mac_addr);

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that this unit is a management
 *              unit.
 *
 * @params  systemStartupreason {(input)} Type of startup
 *          mgrFailover         {(input)} Indicates that the unit is
 *                                        becoming mgr after a failover
 *          lastMgrUnitId       {(input)} Unit number of the last mgr
 *          lastMgrKey          {(input)} Key of the last mgr
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This notification is given to CM just before the configurator
 *        is told to start phase 3. This gives CM the apportunity to
 *        create logical cards.
 *
 * @end
 *
 *********************************************************************/
void cmgrLocalUnitIsManager (L7_LAST_STARTUP_REASON_t systemStartupReason,
                             L7_BOOL                  mgrFailover,
                             L7_uint32                lastMgrUnitId,
                             L7_enetMacAddr_t         lastMgrKey);

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that this unit is no longer
 *          a management unit.
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This notification is given to CM just as the system transitions
 *        into the "wait for management unit" state. This gives CM the 
 *        opportunity to delete logical cards from the driver.
 *
 * @end
 *
 *********************************************************************/
void cmgrLocalUnitIsNotManager (void);

/*********************************************************************
 *
 * @purpose Cnfgr notifies cmgr about completion of a hw apply phase
 *
 * @params  phase {(input)} Hw apply phase completed
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This notification is passed by CM to the driver through the
 *        cardInsertRemoveTask.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrHwApplyNotify (L7_CNFGR_HW_APPLY_t phase);

/*********************************************************************
 * USMDB APIs
 *
 *********************************************************************/

/*********************************************************************
 *
 * @purpose Configure a card in a slot
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot
 * @param  L7_uint32   cardType    Configured Card Type
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This configures a card with the given card type in a slot.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotCardConfigSet(L7_uint32 unit, 
    L7_uint32 slot, 
    L7_uint32 cardType);

/*********************************************************************
 *
 * @purpose Clear configured card information from a slot
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit and slot does not exist
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This clears the configured card information from a slot.  
 *        If a card is physically located in the slot, it will use
 *        the default card values.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotCardConfigClear(L7_uint32 unit, 
    L7_uint32 slot);


/*********************************************************************
 *
 * @purpose Get inserted card type
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32   *cardType  Inserted Card Type
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist or there is no
 *                          card currently plugged into that slot
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the card type of the card that is physically
 *        plugged in to this slot.  If no card is plugged in this slot,
 *         L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCardInsertedCardTypeGet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 *cardType);

/**************************************************************************
 *
 * @purpose  Returns the next valid cardTypeIdIndex of greater value
 *           than the one provided if it exists.
 *
 * @param    cardTypeIdIndex   index into the card descriptor table
 *
 * @returns  L7_SUCCESS  an entry with a greater index value exists
 * @returns  L7_FAILURE  an entry with a greater index value does not exist
 * @returns  cardTypeIdIndex value of next index if found
 *
 * @comments 
 *
 * @end
 *
 *************************************************************************/
L7_RC_t cmgrCardSupportedCardIndexNextGet(L7_uint32 *cardTypeIdIndex);



/*********************************************************************
 *
 * @purpose Get inserted card status
 *
 * @param  L7_uint32     unit       Unit ID of the unit
 * @param  L7_uint32     slot       Slot ID of the slot port
 * @param  L7_uint32    *cardStatus  Inserted Card Status
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or no card is
 *                          currently plubbed into that slot
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the card status of the card that is physically
 *        plugged in to this slot.  If no card is plugged in this slot,
 *         L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cmgrCardStatusGet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 *cardStatus);

/*********************************************************************
 *
 * @purpose Get the number of ports on this card
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32    *numPorts  Number of ports on this card
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist or no card is
 *                          configured in that slot
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the number of ports on the card that is in
 *        this slot.  If no card is configured in this slot,
 *         L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCardNumPortsGet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 *numPorts);

/*********************************************************************
 *
 * @purpose Get configured card type
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32    *cardType  Configured Card Type
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist or no card is
 *                          configured in that slot
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the card type of the card that is configured
 *         in to this slot.  If no card is configured in this slot,
 *         L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCardConfiguredCardTypeGet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 *cardType);

/*********************************************************************
 *
 * @purpose Get slot administrative mode
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32   *adminMode  Configured Administrative mode
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or is not 
 *                          configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the administrative mode of the slot.  If this
 *        slot is not configured, L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotConfiguredAdminModeGet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 *adminMode);

/*********************************************************************
 *
 * @purpose Set slot administrative mode
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32    adminMode  Configured Administrative mode
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This sets the administrative mode of the slot.    
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotAdminModeSet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 adminMode);

#ifndef L7_CHASSIS
/*********************************************************************
 *
 * @purpose Get slot power mode
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32   *powerMode  Configured Power mode
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or is not 
 *                          configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the power mode of the slot.  If this
 *        slot is not configured, L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPowerModeGet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 *powerMode);

/*********************************************************************
 *
 * @purpose Set slot power mode
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32    powerMode  Power mode to set
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or is not 
 *                          configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This configures the power mode of the slot.  If this
 *        slot is not configured, L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPowerModeSet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 powerMode);
#else
/*********************************************************************
 *
 * @purpose Set slot power status
 *
 * @param  L7_uint32    unit         Unit ID of the unit
 * @param  L7_uint32    slot         Slot ID of the slot port
 * @param  L7_uint32    powerStatus  Power status to set
 * @param  L7_uint32    source       source Admin/System
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_FAILURE,     if parameters are invalid or other error
 *
 * @notes This configures the power status of the slot.  If this
 *        slot is not configured, L7_FAILURE will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPowerStatusSet(L7_uint32 unit, L7_uint32 slot,
    POWER_STATUS_t powerStatus, 
    POWER_CONTROL_t source);

/*********************************************************************
 *
 * @purpose Get slot status
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32   *slotPower  Configured Power status
 * @param  L7_uint32   *pSource    source that has set the power status
 *                                 (Admin / System)
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or is not 
 *                          configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the power mode of the slot.  If this
 *        slot is not configured, L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPowerStatusGet (L7_uint32 unit, L7_uint32 slot, 
    POWER_STATUS_t *slotPower, 
    POWER_CONTROL_t *pSource);
#endif

/*********************************************************************
 *
 * @purpose Retrieve slot full/empty status
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_BOOL     *isFull     True if slot is full; false otherwise
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or is not 
 *                          configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns true if the slot is full.  Returns false if the
 *        slot is empty.  If this slot is not configured, L7_NOT_EXIST 
 *        will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotIsFullGet(L7_uint32 unit, L7_uint32 slot, 
    L7_BOOL *isFull);

/*********************************************************************
 *
 * @purpose Retrieve first slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    *slot      Slot ID of the first slot 
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the first slot in this unit.  If this unit is 
 *        not configured, L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotFirstGet(L7_uint32 unit, L7_uint32 *slotId);

/*********************************************************************
 *
 * @purpose Retrieve the next slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot 
 * @param  L7_uint32    *nextSlot      Slot ID of the next slot 
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the slot following the input slot in this unit.  
 *        If this unit is not configured, L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotNextGet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 *nextSlot);

/*********************************************************************
 *
 * @purpose Retrieve the slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot      Slot ID of the  slot 
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit and slot does not exist,
 *                          or is not configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns L7_SUCCESS if the slot exists in this unit.  
 *        If this unit/slot combination is not configured, 
 *        L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 *
 * @purpose Retrieve first ocnfigured slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    *slot      Slot ID of the first slot 
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the first configured slot in this unit.  If this unit is 
 *        not configured, L7_NOT_EXIST will be returned.  
 *                                            
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotFirstConfigGet(L7_uint32 unit, L7_uint32 *slotId);

/*********************************************************************
 *
 * @purpose Retrieve the next configured slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot 
 * @param  L7_uint32    *nextSlot      Slot ID of the next slot 
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns configured the slot following the input slot 
 *        in this unit.  
 *        If this unit is not configured, L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotConfigNextGet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 *nextSlot);

/*********************************************************************
 *
 * @purpose Retrieve the configured slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot      Slot ID of the  slot 
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit and slot does not exist,
 *                          or is not configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns L7_SUCCESS if the slot exists in this unit.  
 *        If this unit/slot combination is not configured, 
 *        L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotConfigGet(L7_uint32 unit, L7_uint32 slot);


/*********************************************************************
 *
 * @purpose Retrieve first physically inserted slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    *slot      Slot ID of the first slot 
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the first slot in this unit.  If this unit is 
 *        not configured, L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotFirstPhysicalGet(L7_uint32 unit, L7_uint32 *slotId);

/*********************************************************************
 *
 * @purpose Retrieve the next slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot 
 * @param  L7_uint32    *nextSlot      Slot ID of the next slot 
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the slot following the input slot in this unit.  
 *        If this unit is not configured, L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPhysicalNextGet(L7_uint32 unit, L7_uint32 slot, 
    L7_uint32 *nextSlot);

/*********************************************************************
 *
 * @purpose Retrieve the slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot      Slot ID of the  slot 
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit and slot does not exist,
 *                          or is not configured 
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns L7_SUCCESS if the slot exists in this unit.  
 *        If this unit/slot combination is not configured, 
 *        L7_NOT_EXIST will be returned.  
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPhysicalGet(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
 * @purpose  Helper routine to get card index from card id
 *
 * @param  card_id  @b{(input)) the card id
 * @param  cx       @b{(output)) the card index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCardIndexFromIDGet(L7_uint32 card_id, L7_uint32 *cx);

/*********************************************************************
 * @purpose  Helper routine to get slot number from unit-slot index
 *
 * @param  ux        @b{(input)) the unit index
 * @param  sx        @b{(input)) the slot index
 * @param  slot_num  @b{(output)) the slot number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrSlotNumFromIndexGet(L7_uint32 ux, L7_uint32 sx, L7_uint32 *slot_number);

/*********************************************************************
 * @purpose  Helper routine to get unit index from unit id
 *
 * @param  unit_id  @b{(input)) the unit id
 * @param  cx       @b{(output)) the unit index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrUnitIndexFromIDGet(L7_uint32 unit_id, L7_uint32 *ux);

/*********************************************************************
 * @purpose  Helper routine to get slot index from slot number 
 *           for a given unit index
 *
 * @param  ux    @b{(input)) the unit index
 * @param  slot  @b{(input)) the slot number
 * @param  sx    @b{(output)) the slot index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrSlotIndexFromNumGet(L7_uint32 ux, L7_uint32 slot, L7_uint32 *sx);

/*********************************************************************
 * @purpose  Helper routine to get unit entry and slot index 
 *           for a given unit-slot
 *
 * @param  unit        @b{(input)) the unit number
 * @param  slot        @b{(input)) the slot number
 * @param  sx          @b{(output)) the slot index
 * @param  unit_entry  @b{(output)) pointer to the unit entry structure
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrUnitEntryAndSlotIndexGet(L7_uint32 unit, L7_uint32 slot, L7_uint32 *sx, 
    SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_entry);

/*********************************************************************
 *
 * @purpose Reset the Module present in the given slot
 *
 * @param  L7_uint32    slotNum              (input)   Slot number
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Card Manager to
 *        reset a module
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrModuleReset(L7_uint32 unit, L7_uint32 slotNum);

/*********************************************************************
 * @purpose  Check if there are any more cards in the CM database 
 *           except the currUnit/currSlot which are yet to be plugged-in 
 *
 * @param  currUnit        @b{(input)) the unit number of the card to ignore
 * @param  currSlot        @b{(input)) the slot number of the card to ignore
 * @param  nextUnit        @b{(output)) the unit number of the card not yet
 *                                      plugged
 * @param  nextSlot        @b{(output)) the slot number of the card not yet
 *                                      plugged
 *
 * @returns  L7_FALSE        No more pluggable cards found.
 * @returns  L7_TRUE         A pluggable card found.
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrCardPendingPluginCheck(L7_uint32 currUnit, L7_uint32 currSlot, 
                                   L7_uint32 *nextUnit, L7_uint32 *nextSlot);


/*********************************************************************
 * @purpose  Restart Card Manager Timers
 *
 * @param    none
 *
 * @notes    Due to lack of synchonization between CMGR/UM/SPM, it
 *           is possible that the manager can receive card status update 
 *           before it has received updates from spm. This causes the
 *           the stack ports to be treated as fron-panel ports and get 
 *           attached at NIM.
 *           To avoid this, CM timer is restarted whenever a unit 
 *           moves to isolated state from a conn_unit/conn_mgr state.
 *           This gives ample opportunity for the manager to get an
 *           stacking port update from the SPM.
 *
 * @returns  void
*********************************************************************/
void cmgrResetTimers(void);
#endif /* CARD_MANAGER_API_H */
