/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename mcastmap_sid.c
*
* @purpose Mcast Mapping Structural Initialization Database
*
* @component MCAST
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
#include "mcastmap_sid.h"
#include "mcastmap_sid_const.h"


/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    void    
*
* @returns  FD_CNFGR_MCAST_MAP_DEFAULT_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 mcastMapSidDefaultStackSize()
{
  return ( FD_CNFGR_MCAST_MAP_DEFAULT_STACK_SIZE );
}

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  FD_CNFGR_MCAST_MAP_DEFAULT_TASK_SLICE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 mcastMapSidDefaultTaskSlice()
{
  return ( FD_CNFGR_MCAST_MAP_DEFAULT_TASK_SLICE );
}                                                      

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  FD_CNFGR_MCAST_MAP_DEFAULT_TASK_PRI 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 mcastMapSidDefaultTaskPriority()
{
  return ( FD_CNFGR_MCAST_MAP_DEFAULT_TASK_PRI );
}

