/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dvmrpmap_sid.c
*
* @purpose Dvmrp Mapping Structural Initialization Database
*
* @component DVMRP
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
#include "dvmrpmap_sid.h"
#include "dvmrpmap_sid_const.h"


/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    void    
*
* @returns  FD_CNFGR_DVMRP_MAP_DEFAULT_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 dvmrpMapSidDefaultStackSize()
{
  return ( FD_CNFGR_DVMRP_MAP_DEFAULT_STACK_SIZE );
}

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  FD_CNFGR_DVMRP_MAP_DEFAULT_TASK_SLICE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 dvmrpMapSidDefaultTaskSlice()
{
  return ( FD_CNFGR_DVMRP_MAP_DEFAULT_TASK_SLICE );
}                                                      

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  FD_CNFGR_DVMRP_MAP_DEFAULT_TASK_PRI 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 dvmrpMapSidDefaultTaskPriority()
{
  return ( FD_CNFGR_DVMRP_MAP_DEFAULT_TASK_PRI );
}

