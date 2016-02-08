/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sysapi_hpc_chassis.h
*
* @purpose Externs for Chassis HPC features
*
* @component HPC
*
* @comments none
*
* @create 09/05/2005
*
* @author msiva
* @end
*
******************************************************************************/

#ifndef SYSAPI_HPC_CHASSIS_H
#define SYSAPI_HPC_CHASSIS_H

#include "sysapi_hpc.h"

/******************************************************************************
*       Chassis HPC API Constants                                             *
******************************************************************************/

#define HPC_CONFIG_PREFERRED_SLOT         0 /* First CFM Slot */

/******************************************************************************
*       Chassis HPC API enumerations and data structures                      *
******************************************************************************/
/* Following enums are used for slot Power On and Power Off */

typedef enum
{
  POWER_OFF = 1,
  POWER_ON
} POWER_STATUS_t;

typedef enum
{
  POWER_ADMIN = 1,
  POWER_SYSTEM
} POWER_CONTROL_t;

typedef struct
{
  POWER_STATUS_t  status;
  POWER_CONTROL_t control;
} slotPower_t;
typedef struct
{
  L7_uint32 componentId;  /* Component ID */
  void(*notify)(L7_uint32 slotNum, L7_uint32 cardTypeId,
                hpcEvent_t eventInfo); /* Callback function */
} L7_EVENT_DATA_t;

typedef struct
{
  L7_BOOL isModPlugIn;  /* Module status as Plugin or Unplug */
  L7_BOOL isModHealthy; /* Module status as healthy or failure */
} L7_MODULE_EVENT_DATA_t;

typedef struct
{
  L7_LED_COLOR_t masterLedStatus;
  L7_LED_COLOR_t powerLedStatus;
  L7_LED_COLOR_t cfmLedStatus;
  L7_LED_COLOR_t fanLedStatus;
  L7_LED_COLOR_t poeLedStatus;
  L7_LED_COLOR_t lm1LedStatus;
  L7_LED_COLOR_t lm2LedStatus;
  L7_LED_COLOR_t lm3LedStatus;
  L7_LED_COLOR_t lm4LedStatus;
} L7_CHASSIS_LED_STATS_t;

typedef enum
{
  L7_HPC_HEARTBEAT_COMM_NOT_ESTABLISHED = 100,
  L7_HPC_HEARTBEAT_COMM_LOST,
  L7_HPC_HEARTBEAT_COMM_LAST

} L7_HPC_HEARTBEAT_FAILURE_t;

/******************************************************************************
*       Chassis HPC API Externs                                               *
******************************************************************************/

/*****************************************************************************/
/*      Chassis HPC API Function Prototypes                                  */
/*****************************************************************************/

/******************************************************************************
*
* @purpose  HPC Sensor handler initialization. Sensor Handler is responsible
*           for detecting module events.
*
* @param    None.
*
* @returns  L7_SUCCESS  Successful initialization of Sensor Handler.
* @returns  L7_FAILURE  problem encountered in Sensor Handler initialization
*
* @notes
*
* @end
*
******************************************************************************/
L7_RC_t sysapiHpcSensorHndlrInit(void);

/******************************************************************************
* @purpose  To set the board id for a module
*
* @param    slotNum - @b{(input)}  Slot Number
*           boardId - @b{(input)}  Board id to be written into the module
*
* @returns  L7_SUCCESS,   if board id write is successful
*           L7_FAILURE,   Otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t sysapiHpcModuleBoardIdSet(L7_uint32 slotNum, L7_uint32 boardId);

/******************************************************************************
* @purpose  To get the board id of a module
*
* @param    slotNum  - @b{(input)}  Slot Number
*           pBoardId - @b{(output)} Board id of the requested module
*
* @returns  L7_SUCCESS,   if board id is read successfully
*           L7_FAILURE,   Otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t sysapiHpcModuleBoardIdGet(L7_uint32 slotNum, L7_uint32 *pBoardId);

/******************************************************************************
*
* @purpose  HPC Power Handler initialization.
*
* @param    None.
*
* @returns  L7_SUCCESS  Successful initialization of Power Handler.
* @returns  L7_FAILURE  problem encountered in Power Handler initialization
*
* @notes
*
* @end
*
******************************************************************************/
L7_RC_t sysapiHpcPwrHndlrInit(void);

/******************************************************************************
* @purpose  To turn on/off power to a line module slot
*
* @param    slotNum - @b{(input)}  Slot Number
*           status  - @b{(input)}  Power status (On/Off) of the LM slot
*           source  - @b((input))  Initiator of this request
*
* @returns  L7_SUCCESS,   if the setting of power status of a slot
*                         is successful
*           L7_FAILURE,   Otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t sysapiHpcSlotPowerStatusSet(L7_uint32 phySlot,
                                    POWER_STATUS_t status, POWER_CONTROL_t source);

/******************************************************************************
* @purpose  To get the power status of a line module slot
*
* @param    slotNum - @b{(input)}  Slot Number
*           pStatus - @b{(output)} Power status (On/Off) of the LM slot
*           pSource - @b((output)) To store the source responsible for
*                                  current power status of a slot
*
* @returns  L7_SUCCESS,   if power status is retrived successfully
*           L7_FAILURE,   Otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t sysapiHpcSlotPowerStatusGet(L7_uint32 phySlot,
                                    POWER_STATUS_t *pStatus, POWER_CONTROL_t *pSource);

/******************************************************************************
* @purpose  To reset a control or line module
*
* @param    slotNum - @b{(input)}  Slot Number
*
* @returns  L7_SUCCESS,   if the resetting of a module is successful
*           L7_FAILURE,   Otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t sysapiHpcPwrModuleReset(L7_uint32 slotNum);

/******************************************************************************
* @purpose  To get the CFM's own slot number
*
* @param    pSlotNum - @b{(input)}  Slot Number
*
* @returns  none
*
* @notes    none
*
* @end
******************************************************************************/
void sysapiHpcCurrentSlotGet(L7_uint32 *pSlotNum);

/******************************************************************************
* @purpose  To get the Other CFM's slot number
*
* @param    pSlotNum - @b{(input)}  Slot Number
*
* @returns  none
*
* @notes    none
*
* @end
******************************************************************************/
void sysapiHpcOtherCfmSlotGet(L7_uint32 *pSlotNum);

/******************************************************************************
* @purpose  To get the Module's event status based on the slot number
*
* @param    slotNum - @b{(input)}   Slot Number
* @param    pEvData - @b{(output)}  Pointer to a structure to stores the
*                                   given module event status
*
* @returns  L7_SUCCESS, if the event status of a given slot is handled
*                       successfully
*           L7_FAILURE, Otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t sysapiHpcModuleEventStatusGet(L7_uint32 slotNum,
                                      L7_MODULE_EVENT_DATA_t *pEvData);

/******************************************************************************
* @purpose  To get the module presence in a given slot
*
* @param    slotNum      - @b{(input)}   Slot Number
* @param    pModPresence - @b{(output)}  Pointer to a variable to store the
*                                        presence of the module
*
* @returns  L7_SUCCESS, if the module presence status in a given slot is handled
*                       successfully
*           L7_FAILURE, Otherwise
*
* @notes    Currently this function checks the presence of remote CFM only
*
* @end
******************************************************************************/
L7_RC_t sysapiHpcIsModulePresent(L7_uint32 slotNum, L7_BOOL *pModPresence);

/******************************************************************************
* @purpose  To get the health status of a module in the given slot
*
* @param    slotNum     - @b{(input)}   Slot Number
* @param    pModHealthy - @b{(output)}  Pointer to a variable to store the
*                                       healthy status of the module
*
* @returns  L7_SUCCESS, if the healthy status of a module present in a given
*                       slot is handled successfully
*           L7_FAILURE, Otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t sysapiHpcIsModuleHealthy(L7_uint32 slotNum, L7_BOOL *pModHealthy);

/******************************************************************************
* @purpose  To initilize the Heartbeat handler that monitors the health of the
*           other CFM.
*
* @param    None.
*
* @returns  L7_SUCCESS  Successful initialization of HeartBeat Handler.
* @returns  L7_FAILURE  problem encountered in HeartBeat Handler initialization
*
* @notes
*
* @end
*
******************************************************************************/
L7_RC_t sysapiHpcHeartBeatHndlrInit(void);

/******************************************************************************
* @purpose  To stop the Heartbeat activity with the other CFM.
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
******************************************************************************/
void sysapiHpcHeartBeatStop(void);

/******************************************************************************
* @purpose  Initilizes the HPC HRC Component
*
* @param    void
*
* @returns  L7_SUCCESS - Component Initialized successfully
* @returns  L7_FAILURE - otherwise
*
* @notes    This function initializes the HRC Component.
*
*           It uses the "Master Discovery Process" to determine if this
*           CFM should become the Master CFM. It uses the following data
*           for this process
*           1. EEPROM Signature from previous boot
*           2. EEPROM signature from other CFM
*           3. Presence of other CFM
*           4. CFM Slot Number
*
*           If this function determines that this CFM should become the
*           active CFM, it initiates the activation sequence, most
*           importantly,
*           1. the Sensor Handler event detection process is kicked-off
*           2. the eeprom signature is written
*           3. the admin preference for the unit manager is enabled.
*
*           If this function determines that this CFM should become the
*           standby CFM, it initiates the HeartBeat communication.
*
* @end
******************************************************************************/
L7_RC_t sysapiHpcHrcInit(void);

/******************************************************************************
* @purpose  Processes a Active or Standby event
*
* @param    event - @b{(input)}  event type
*
* @returns  none
*
* @notes
*
* @end
******************************************************************************/
void sysapiHpcHrcActiveStandbyNotify(SYSAPI_CARD_EVENT_t event);

/**************************************************************************
*
* @purpose This function provides registration of callback function for HPC
*           events.
*
* @param    handle        The Event Handler.
* @param    eventInfo     The Event Information. Which has component ID and
*                         callback function.
*
* @returns  L7_SUCCESS  On successful registration.
* @returns  L7_FAILURE  Memory allocation failure or invalid input arguments.
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcEventCallbackRegister(L7_EVENT_HANDLE_t *handle,
                                       L7_EVENT_DATA_t *eventInfo);

/**************************************************************************
*
* @purpose This function deregistration of callback function for HPC
*           events based on event handler.
*
* @param    handle        The Event Handler.
*
* @returns  L7_SUCCESS  On successful deregistration.
* @returns  L7_FAILURE  If handler is not present in event database.
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcEventCallbackDeregister(L7_EVENT_HANDLE_t handle);

/**************************************************************************
*
* @purpose This function invokes all callback function  which are registered on
*           to notify the event occurance.
*
* @param    eventInfo        Event Details. Which has event ID and source.
*
* @returns  L7_SUCCESS  On success.
*
* @comments
*
* @end
*
*************************************************************************/
void hpcHevEventNotify(hpcEvent_t eventInfo);

/**************************************************************************
*
* @purpose  It informs the HPC LED event to LED handler.
*
* @param    slotNum   Slot Number. This is the source for Event.
* @param    eventId   Event Id.    It indicates the HPC Event.
*
* @returns  L7_SUCCESS  Successful processing of an LED event.
* @returns  L7_FAILURE  problem encountered in getting slot information or
*           event which is not required for LED.
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLedUpdate(L7_uint32 slotNum, SYSAPI_CARD_EVENT_t eventId);

/**************************************************************************
*
* @purpose  It Gets the status of all LEDs in the system.
*
* @param    ledStatus   Led Status Array.  LED status information.
*
* @returns  L7_SUCCESS  Successful getting LED status.
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLedStatusGet(L7_CHASSIS_LED_STATS_t *ledStatus);

/**************************************************************************
*
* @purpose This function Allocates memory and initializes the Event database
*           and semaphore.
*
* @param    None
*
* @returns  L7_SUCCESS  On successful initialization.
* @returns  L7_FAILURE  Memory allocation/Semaphore creation failure.
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcEventHandlerInit(void);

/**************************************************************************
*
* @purpose  HPC LED handler initialization. It registeres with HPC event
*           handler for events and invokes board LED init function.
*
* @param    None.
*
* @returns  L7_SUCCESS  Successful initialization of LED handler.
* @returns  L7_FAILURE  problem encountered in registration with HPC event
*                       handler.
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLedHandlerInit(void);

/******************************************************************************
 * @purpose  To know whether (the other CFM is presen && none of the LMs are
 *           not present )
 *
 * @param    void  - @b{(input)}
 *
 * @returns  L7_TRUE, if the above mentioned condition is true
 *           L7_FALSE, Otherwise
 *
 * @notes    none
 *
 * @end
 *******************************************************************************/
L7_BOOL sysapiHpcIsConnBtwTwoCfmsNotPrsnt(void);

L7_BOOL sysapiHpcIsThisCfmOnlyPrsnt(void);

#endif /* SYSAPI_HPC_CHASSIS_H */
