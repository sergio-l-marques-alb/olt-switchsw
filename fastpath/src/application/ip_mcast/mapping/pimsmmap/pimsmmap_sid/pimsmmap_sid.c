/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  pimsmmap_sid.c
*
* @purpose   PIM-SM Mapping Structural Initialization Database
*
* @component PIM-SM
*
* @comments  none
*
* @create    11/12/2003
*
* @author    mfiorito
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "pimsmmap_sid.h"
#include "pimsmmap_sid_const.h"


/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    none 
*
* @returns  FD_CNFGR_PIMSM_MAP_DEFAULT_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimsmMapSidDefaultStackSize()
{
  return ( FD_CNFGR_PIMSM_MAP_DEFAULT_STACK_SIZE );
}

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    none 
*
* @returns  FD_CNFGR_PIMSM_MAP_DEFAULT_TASK_SLICE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimsmMapSidDefaultTaskSlice()
{
  return ( FD_CNFGR_PIMSM_MAP_DEFAULT_TASK_SLICE );
}                                                      

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    none 
*
* @returns  FD_CNFGR_PIMSM_MAP_DEFAULT_TASK_PRI 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimsmMapSidDefaultTaskPriority()
{
  return ( FD_CNFGR_PIMSM_MAP_DEFAULT_TASK_PRI );
}

