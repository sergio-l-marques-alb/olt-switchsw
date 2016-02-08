/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   fanmgr_binds.c
*
* @purpose    Binds for fan manager functionality
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
L7_RC_t fumgrAllTrapsRegister( L7_COMPONENT_IDS_t registrar_ID,
                              void (*notify)(L7_uint32 unit,
                                             L7_uint32 slot,
                                             L7_uint32 ins_cardTypeId,
                                             L7_uint32 cfg_cardTypeId,
                                             trapMgrNotifyEvents_t event))
{
	return L7_SUCCESS;
}
