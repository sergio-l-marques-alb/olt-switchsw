
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   fanunitmgr_api.h
*
* @purpose    Component APIs for the Fan Unit Manager
*
* @component  Fan Unit Manager
*
* @comments   
*
* @create     07/20/2005
*
* @author     msiva
* @end
*
******************************************************************************/

#ifndef FAN_UNIT_MANAGER_API_H
#define FAN_UNIT_MANAGER_API_H

#include "l7_cnfgr_api.h"
#include "trap_inventory_api.h"
#include "chassis_exports.h"

/******************************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.       
*
* @returns  None
*
* @notes    This function completes synchronously. The return value 
*           is presented to the configurator by calling the cnfgrApiCallback().
*           The following are the possible return codes:   
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following is valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread
*           MUST NOT be blocked!
*       
* @end
******************************************************************************/
extern void fumgrApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

/******************************************************************************
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
******************************************************************************/
extern L7_RC_t fumgrAllTrapsRegister(L7_COMPONENT_IDS_t  registrar_ID,
                              void (*notify)(L7_uint32 unit,
                                             L7_uint32 slot,
                                             L7_uint32 ins_cardTypeId,
                                             L7_uint32 cfg_cardTypeId,
                                             trapMgrNotifyEvents_t event));
/******************************************************************************
* @purpose  This function gets the handle for first entry in the fan
*           unit manager module database.
*
* @param    pHandle - @b{(output)}  handle to the first entry in 
*                                   the module database
*
* @returns  L7_SUCCESS - There were no errors.
* @returns  L7_FAILURE - There were errors.
* @end
******************************************************************************/
extern L7_RC_t fumgrModuleStatusFirstGet(L7_uint32 *pHandle);

/******************************************************************************
* @purpose  This function gets the handle to the next entry in the 
*           fan unit manager module database, based on the 
*           specified  handle
*
* @param    handle       - @b{(input)}   handle to the current entry
* @param    pNextHandle  - @b{(output)}  handle to the next entry
*
* @returns  L7_SUCCESS   - There were no errors.
* @returns  L7_FAILURE   - There were errors.
* @returns  L7_NOT_EXIST - If there is no next entry after
*                          current entry
*
* @end
******************************************************************************/
extern L7_RC_t fumgrModuleStatusNextGet(L7_uint32 handle,
                                        L7_uint32 *pNextHandle);


/******************************************************************************
* @purpose  This function gets the handle for the entry in the fan
*           unit manager module database, based on the slot number.
*
* @param    slotNum    - @b{(input)}   Slot number
* @param    pHandle    - @b{(output)}  handle to the entry in the 
*                                      module database
*
* @returns  L7_SUCCESS - There were no errors.
* @returns  L7_FAILURE - There were errors.
* @end
******************************************************************************/
extern L7_RC_t fumgrModuleStatusSlotGet(L7_uint32 slotNum,
                                        L7_uint32 *pHandle);

/******************************************************************************
* @purpose  This function gets the value of a module database field, 
*           identified by the parameter and handle.
*
* @param    handle  - @b{(input)}   handle to the database entry
* @param    param   - @b{(input)}   Field identifier of a module
*                                   database entry
* @param    pEntry  - @b{(output)}  Pointer to a parameter that stores 
*                                   the returned database field value
*
* @returns  L7_SUCCESS  - There were no errors.
* @returns  L7_FAILURE  - There were errors.
* @end
******************************************************************************/
extern L7_RC_t fumgrModuleStatusGet(L7_uint32 handle, 
                                    L7_MODULE_DATABASE_PARAMS_t param, 
                                    void *pEntry);

#endif /* FAN_UNIT_MANAGER_API_H */

