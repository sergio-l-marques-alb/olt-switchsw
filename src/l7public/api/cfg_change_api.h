
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename cfg_change_api.h
*
* @purpose Detect Hardware Configuration Changes API
*
* @component cfgChange
*
* @comments none
*
* @create 09/15/2000
*
* @author bmutz
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef INCLUDE_CFG_CHANGE_API
#define INCLUDE_CFG_CHANGE_API

#include "commdefs.h"
#include "datatypes.h"
#include "registry.h"

typedef enum
{
  CONTROL_MODULE =1,
  LINE_MODULE
} 
CHASSIS_MODULETYPE_t;

/****************************************
*
*  CFGCHANGE Functions                   
*
*****************************************/
L7_uint32 cfgChangeRegister(L7_uint32 registrar_ID, void (*notifyCfgChange)(L7_uint32 slotNum, slot_registry *pSR));

/*********************************************************************
* @purpose  return SystemInitComplete status.
*
* @param    void    
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL cnfgrIsSystemInitComplete();

/*********************************************************************
* @purpose  Indicates if the system is a chassis system.
*
* @param    void    
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL cnfgrIsChassis(void);

/*********************************************************************
* @purpose  Indicates if the system is a chassis CFM or LM system.
*
* @param    CONTROL_MODULE/LINE_MODULE
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
**********************************************************************/
L7_BOOL cnfgrIsChassisModule(CHASSIS_MODULETYPE_t moduleType);

#endif
