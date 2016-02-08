/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename     hpc_chassis_hw_api.h
*
* @purpose      Prototypes Chassis HPC component's platform-specific function 
*               APIs
*
* @component    hpc
*
* @comments     none
*
* @create       11/16/2005
*
* @author       YTR
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/
#ifndef INCLUDE_HPC_CHASSIS_HW_API
#define INCLUDE_HPC_CHASSIS_HW_API

#include "l7_common.h"
#include "sysapi_hpc.h"
#include "sysapi_hpc_chassis.h"

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
void hpcCurrentSlotGet(L7_uint32 *pSlotNum);

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
void hpcOtherCfmSlotGet(L7_uint32 *pSlotNum);

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
L7_RC_t hpcModuleEventStatusGet(L7_uint32 slotNum, 
                                L7_MODULE_EVENT_DATA_t *pEvData);

/*****************************************************************************/
/*      HPC EEPROM Handler Function Prototypes                               */
/*****************************************************************************/

/******************************************************************************
* @purpose  To read 8 bit data from a EEPROM device located on a module
*
* @param    slotNum - @b{(input)}  Slot Number
*           offset  - @b{(input)}  Offset of memory location
*           pData   - @b{(output)} Board id of the requested module
*
* @returns  L7_SUCCESS,   if the memory location is read successfully
*           L7_FAILURE,   Otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t hpcEepromRead8(L7_uint32 slotNum, L7_ushort16 offset, 
                       L7_uchar8 *pData);

/******************************************************************************
* @purpose  To write a 8 bit data into a specified location in EEPROM device
*           located on a module
*
* @param    slotNum - @b{(input)}  Slot Number
*           offset  - @b{(input)}  Offset of memory location
*           data    - @b{(output)} Data to be written into the memory location
*
* @returns  L7_SUCCESS,   if the data is written successfully
*           L7_FAILURE,   Otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t hpcEepromWrite8(L7_uint32 slotNum, L7_ushort16 offset, L7_uchar8 data);

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
L7_RC_t hpcModuleBoardIdSet(L7_uint32 slotNum, L7_uint32 boardId);

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
L7_RC_t hpcModuleBoardIdGet(L7_uint32 slotNum, L7_uint32 *pBoardId);

/*****************************************************************************/
/*      HPC HeartBeat Handler Function Prototypes                            */
/*****************************************************************************/
/******************************************************************************
* @purpose  To initilize the Heartbeat handler that monitors the health of the 
*           other CFM.

. Heartbeat handler detects the 
*           failure of the other CFM by monitoring it continuosly. It notifies
*           about the failure to Redundancy handler in order to initiate 
*           switchover.
*
* @param    void
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t   hpcHeartBeatHndlrInit(void);

/******************************************************************************
* @purpose  To start the Heartbeat activity with the other CFM.
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
******************************************************************************/
void hpcHeartBeatStart();

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
void hpcHeartBeatStop();

/*****************************************************************************/
/*      HPC HRC Function Prototypes                                          */ 
/*****************************************************************************/

/******************************************************************************
* @purpose  Initilizes the HPC HRC Component
*
* @param    void
*
* @returns  L7_SUCCESS - Component Initialized successfully
* @returns  L7_FAILURE - otherwise
*
* @notes    Placeholder for HPC HRC initialization
*
* @end
******************************************************************************/
L7_RC_t hpcHrcInit(void);

/******************************************************************************
* @purpose  Processes a HeartBeat failure event from the HBT Handler
*
* @param    failureType - @b{(input)}  Type of the HeartBeat failure
*
* @returns  none
*
* @notes
*
* @end
******************************************************************************/
void hpcHrcHeartBeatFailureNotify (L7_HPC_HEARTBEAT_FAILURE_t failureType);

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
void hpcHrcActiveStandbyNotify(SYSAPI_CARD_EVENT_t event);

/*****************************************************************************/
/*      HPC LED Handler Function Prototypes                                  */ 
/*****************************************************************************/
    
/**************************************************************************
*
* @purpose  It creates the semaphore, message queue and tasks. It also 
*           initializes the blinking database.
*
* @param    None.
*
* @returns  None
*
* @notes 
*
* @end
*
*************************************************************************/
void ledHandlerInit(void);

/**************************************************************************
*
* @purpose  It posts a message to LED task for event processing. The message 
*           post depends on event and system state. In case, system is in 
*           standby mode, only stanby and active events will be processed. 
*
* @param    slotNum   Slot Number. This is the source for Event.
* @param    eventId   Event Id.    It indicates the HPC Event.
*
* @returns  L7_SUCCESS  Successful processing of an event.
* @returns  L7_FAILURE  problem encountered in getting slot information or 
*                       event which is not required for LED.
*
* @notes 
*
* @end
*
*************************************************************************/
L7_RC_t ledUpdate(L7_uint32 slotNum, SYSAPI_CARD_EVENT_t eventId);

/**************************************************************************
*
* @purpose  It Gets the status of all LEDs from BSP.
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
L7_RC_t hpcLedStatusGet(L7_CHASSIS_LED_STATS_t *ledStatus);



/*****************************************************************************/
/*      HPC Power Function Prototypes                                        */
/*****************************************************************************/

/******************************************************************************
* @purpose  Initilizes the HPC Power Handler
*
* @param    void
*
* @returns  L7_SUCCESS - Database is created successfully
* @returns  L7_FAILURE - otherwise
*
* @notes    Placeholder for HPC Power Handler initialization
*
* @end
******************************************************************************/
L7_RC_t hpcPwrHndlrInit(void);

/******************************************************************************
* @purpose  To turn on/off power to a line module slot
*
* @param    slotNum - @b{(input)}  Slot Number
*           status  - @b{(input)}  Power status (On/Off) of the LM slot
*                                  the administrator or not.
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
L7_RC_t hpcPwrSlotPowerStatusSet(L7_uint32 phySlot, POWER_STATUS_t status,
                                 POWER_CONTROL_t source);

/******************************************************************************
* @purpose  To reset a control or line module
*
* @param    slotNum - @b{(input)}  Slot Number
*
* @returns  L7_SUCCESS,   if the setting of power status of a slot
*                         is successful
*           L7_FAILURE,   Otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t hpcPwrModuleReset(L7_uint32 slotNum);



/*****************************************************************************/
/*      HPC Sensor Function Prototypes                                       */
/*****************************************************************************/
/******************************************************************************
* @purpose  To initilize the sensor detector and sensor event processor.
*           Detector polls module's state periodically. If there is
*           any change in the status from the previous one, it informs the
*           event processor. The event processor identifies the module event
*           and notifies the registered components.
*
* @param    void
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t   hpcSensorHndlrInit(void);

/******************************************************************************
 * @purpose  To know whether (the other CFM is present && any of the LMs are 
 *           not present ) 
 *
 * @param    void  - @b{(input)}   
 *
 * @returns  L7_TRUE, if the above mentioned condition is true
 *           L7_FFALSE, Otherwise
 *
 * @notes    none
 *
 * @end
 ******************************************************************************/
L7_BOOL hpcIsConnBtwTwoCfmsNotPrsnt(void);

L7_BOOL hpcIsThisCfmOnlyPrsnt(void);
#endif
