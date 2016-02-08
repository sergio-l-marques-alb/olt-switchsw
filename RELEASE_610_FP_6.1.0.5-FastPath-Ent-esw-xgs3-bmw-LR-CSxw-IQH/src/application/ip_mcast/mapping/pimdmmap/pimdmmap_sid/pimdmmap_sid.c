/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  pimdmmap_sid.c
*
* @purpose   PIM-DM Mapping Structural Initialization Database
*
* @component PIM-DM
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
#include "pimdmmap_sid.h"
#include "pimdmmap_sid_const.h"


/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    void    
*
* @returns  FD_CNFGR_PIMDM_MAP_DEFAULT_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimdmMapSidDefaultStackSize()
{
  return ( FD_CNFGR_PIMDM_MAP_DEFAULT_STACK_SIZE );
}

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  FD_CNFGR_PIMDM_MAP_DEFAULT_TASK_SLICE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimdmMapSidDefaultTaskSlice()
{
  return ( FD_CNFGR_PIMDM_MAP_DEFAULT_TASK_SLICE );
}                                                      

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  FD_CNFGR_PIMDM_MAP_DEFAULT_TASK_PRI 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimdmMapSidDefaultTaskPriority()
{
  return ( FD_CNFGR_PIMDM_MAP_DEFAULT_TASK_PRI );
}

