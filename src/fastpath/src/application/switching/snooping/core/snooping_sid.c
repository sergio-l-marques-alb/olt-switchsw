/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename snooping_sid.c
*
* @purpose Snooping Structural Initialization Database
*
* @component Snooping
*
* @comments none
*
* @create 07/2/2003
*
* @author markl
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "snooping_sid.h"
#include "snooping_sid_const.h"


/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 snoopSidDefaultStackSize()
{
  return ( FD_CNFGR_SNOOPING_DEFAULT_STACK_SIZE);
}

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 snoopSidDefaultTaskSlice()
{
  return ( FD_CNFGR_SNOOPING_DEFAULT_TASK_SLICE );
}

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 snoopSidDefaultTaskPriority()
{
  return ( FD_CNFGR_SNOOPING_DEFAULT_TASK_PRI );
}

