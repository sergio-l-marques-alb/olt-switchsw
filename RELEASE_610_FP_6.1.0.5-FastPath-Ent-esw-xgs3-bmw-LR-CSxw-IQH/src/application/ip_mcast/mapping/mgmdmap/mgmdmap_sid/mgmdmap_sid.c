/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename mgmdmap_sid.c
*
* @purpose Mgmd Mapping Structural Initialization Database
*
* @component MGMD
*
* @comments none
*
* @create 09/10/2003
*
* @author jeffr
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "mgmdmap_sid.h"
#include "mgmdmap_sid_const.h"


/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    void    
*
* @returns  FD_CNFGR_MGMD_MAP_DEFAULT_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 mgmdMapSidDefaultStackSize()
{
  return ( FD_CNFGR_MGMD_MAP_DEFAULT_STACK_SIZE );
}

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  FD_CNFGR_MGMD_MAP_DEFAULT_TASK_SLICE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 mgmdMapSidDefaultTaskSlice()
{
  return ( FD_CNFGR_MGMD_MAP_DEFAULT_TASK_SLICE );
}                                                      

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  FD_CNFGR_MGMD_MAP_DEFAULT_TASK_PRI 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 mgmdMapSidDefaultTaskPriority()
{
  return ( FD_CNFGR_MGMD_MAP_DEFAULT_TASK_PRI );
}

