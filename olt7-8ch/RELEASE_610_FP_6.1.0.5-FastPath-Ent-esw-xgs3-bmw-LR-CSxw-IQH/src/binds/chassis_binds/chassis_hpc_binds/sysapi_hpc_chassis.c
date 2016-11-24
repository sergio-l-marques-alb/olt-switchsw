
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   sysapi_hpc_chassis.c
*
* @purpose    Binds for Chassis HPC features
*
* @component  sysapi
*
* @create     09/05/2005
*
* @author     msiva
* @end
*
******************************************************************************/
#include "log.h"
#include "sysapi_hpc.h"
#include "sysapi_hpc_chassis.h"
#include "hpc_hw_api.h"
#include "hpc_chassis_hw_api.h"

/*****************************************************************************/
/*      Chassis HPC Global Data Structures                                   */
/*****************************************************************************/

/*****************************************************************************/
/*      Chassis HPC Function Prototypes                                      */
/*****************************************************************************/

/*****************************************************************************/
/*      Chassis HPC Functions                                                */
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
L7_RC_t sysapiHpcSensorHndlrInit(void)
{
  return hpcSensorHndlrInit();
}

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
L7_RC_t sysapiHpcPwrHndlrInit(void)
{
  return L7_SUCCESS;
}

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
                           POWER_STATUS_t status, POWER_CONTROL_t source)
{
  return L7_SUCCESS;
}

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
                           POWER_STATUS_t *pStatus, POWER_CONTROL_t *pSource)
{
  *pStatus = POWER_ON;
  *pSource = POWER_ADMIN;
  return L7_SUCCESS;
}

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
L7_RC_t sysapiHpcPwrModuleReset(L7_uint32 slotNum)
{
  return L7_SUCCESS;
}

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
void sysapiHpcCurrentSlotGet(L7_uint32 *pSlotNum)
{
  return hpcCurrentSlotGet(pSlotNum);
}

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
void sysapiHpcOtherCfmSlotGet(L7_uint32 *pSlotNum)
{
  *pSlotNum = 0;
}

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
                                      L7_MODULE_EVENT_DATA_t *pEvData)
{
  /* Sensor Detector is not active on non-CFM modules */
  pEvData->isModPlugIn  = L7_FALSE;
  pEvData->isModHealthy = L7_FALSE;
  return L7_SUCCESS;
}

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
L7_RC_t sysapiHpcModuleBoardIdSet(L7_uint32 slotNum, L7_uint32 boardId)
{
  return L7_SUCCESS;
}

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
L7_RC_t sysapiHpcModuleBoardIdGet(L7_uint32 slotNum, L7_uint32 *pBoardId)
{
  *pBoardId = 0;
  return L7_SUCCESS;
}

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
L7_RC_t sysapiHpcIsModulePresent(L7_uint32 slotNum, L7_BOOL *pModPresence)
{
  *pModPresence = L7_FALSE;
  return L7_SUCCESS;
}

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
L7_RC_t sysapiHpcIsModuleHealthy(L7_uint32 slotNum, L7_BOOL *pModHealthy)
{
  *pModHealthy = L7_FALSE;
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  This is a binding for a function that initialized the HPC
*           Heartbeat Handler.
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
L7_RC_t sysapiHpcHeartBeatHndlrInit(void)
{
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  This is a binding for a function that stops the heartbeat activity 
*           with the remote CFM.
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
******************************************************************************/
void sysapiHpcHeartBeatStop(void)
{
  return;
}

/******************************************************************************
* @purpose  This is a binding for a function that initializes the redundancy 
*           controller(HRC)
*
* @param    void
*
* @returns  L7_SUCCESS - always
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t sysapiHpcHrcInit(void)
{
  return L7_SUCCESS;
}

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
void sysapiHpcHrcActiveStandbyNotify(SYSAPI_CARD_EVENT_t event)
{
  return;
}

/**************************************************************************
*
* @purpose  This is bind function. It just returns success always.
*
* @param    None
*
* @returns  L7_SUCCESS  On successful initialization.
*
* @comments 
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcEventHandlerInit(void)
{

    return L7_SUCCESS;   
}


/**************************************************************************
*
* @purpose  This is the dummy function provides registration of callback 
*           function for HPC events.
*
* @param    handle        The Event Handler handle. 
* @param    eventInfo     The Event Information. Which has component ID and 
*                         callback function. 
*
* @returns  L7_SUCCESS  On successful registration.
*
* @comments 
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcEventCallbackRegister(L7_EVENT_HANDLE_t *handle, 
                L7_EVENT_DATA_t *eventInfo) 
{
    if (L7_NULL == eventInfo)
    {
      LOG_MSG("%s: %d: sysapiHpcEventHandlerCallbackRegister:"
                "Event info pasessed as NULL\n", 
          __FILE__, __LINE__);
      return L7_FAILURE;
    }
    
    /* Invoke callback function for local card and logical card plug-ins.*/
    (void)hpcHardwareCardEventCallbackNotify(eventInfo->notify);
    
    *handle = 0;        
    return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  This dummy function provides deregistration of callback function 
*           for HPC events based on event handler handle.
*
* @param    handle        The Event Handler. 
*
* @returns  L7_SUCCESS  On successful deregistration.
*
* @comments 
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcEventCallbackDeregister(L7_EVENT_HANDLE_t handle) 
{      
    return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  HPC LED handler initialization. This is a bind function.
*
* @param    None.
*
* @returns  L7_SUCCESS  Successful initialization of LED handler.
*
* @notes 
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLedHandlerInit(void)
{
    return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  This is a dummy function for LED update.
*
* @param    slotNum   Slot Number. This is the source for Event.
* @param    eventId   Event Id.    It indicates the HPC Event.
*
* @returns  L7_SUCCESS  Successful processing of an LED event.
* @returns  L7_FAILURE  problem encountered in getting slot information or 
*                       event which is not required for LED.
*
* @notes 
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLedUpdate(L7_uint32 slotNum, SYSAPI_CARD_EVENT_t eventId)
{  
  return L7_SUCCESS;
}

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
L7_RC_t sysapiHpcLedStatusGet(L7_CHASSIS_LED_STATS_t *ledStatus)
{
  return L7_FAILURE;
}

/******************************************************************************
 * @purpose  To know whether (the other CFM is present && none of the LMs are 
 *                     present ) 
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
L7_BOOL sysapiHpcIsConnBtwTwoCfmsNotPrsnt(void)
{
  return L7_FALSE;

}

L7_BOOL sysapiHpcIsThisCfmOnlyPrsnt(void)
{
  return L7_FALSE;
}

