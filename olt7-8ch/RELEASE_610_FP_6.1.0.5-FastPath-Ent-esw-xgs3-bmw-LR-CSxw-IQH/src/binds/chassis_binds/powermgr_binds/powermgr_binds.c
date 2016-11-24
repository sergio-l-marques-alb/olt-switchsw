/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   powermgr_binds.c
*
* @purpose    Binds for power manager functionality
*
* @component  fan manager
*
* @create     13/07/2006
*
* @author     siva
* @end
*
******************************************************************************/

#include "trap_inventory_api.h"
#include "commdefs.h"
#include "datatypes.h"
#include "sysapi_hpc.h"
#include "sysapi_hpc_chassis.h"

/******************************************************************************
* @purpose  provides a bind for pumgrAllTrapsRegister function for non-chassis 
* platforms
*
* @param    registrar_ID - @b{(input)}   routine registrar id  (See
*                                          L7_COMPONENT_ID_t)
* @param    notify       - @b{(input)}   pointer to a routine to be invoked
*                                        for card creation.  Each routine has
*                                        the following parameters:
*                          (unit, slot, ins_cardTypeId, cfg_cardTypeId, event).
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t pumgrAllTrapsRegister ( L7_COMPONENT_IDS_t  registrar_ID,
                              void (*notify)(L7_uint32 unit,
                                             L7_uint32 slot,
                                             L7_uint32 ins_cardTypeId,
                                             L7_uint32 cfg_cardTypeId,
                                             trapMgrNotifyEvents_t event))
{
	return L7_SUCCESS;
}

#ifndef L7_CHASSIS
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
L7_RC_t hpcPwrSlotPowerStatusSet(L7_uint32 phySlot, POWER_STATUS_t status,
                           POWER_CONTROL_t source)
{
  return L7_FAILURE;
}
#endif
