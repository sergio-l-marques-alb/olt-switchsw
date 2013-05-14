/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename pml_sid.c
*
* @purpose Port MAC Locking Structural Initialization Database
*
* @component Port MAC Locking
*
* @comments none
*
* @create 05/21/2004
*
* @author colinw
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "pml_sid.h"
#include "pml_sid_const.h"


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
L7_int32 pmlSidDefaultStackSize()
{
  return ( FD_CNFGR_PML_DEFAULT_STACK_SIZE );
}

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_SLICE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 pmlSidDefaultTaskSlice()
{
  return ( FD_CNFGR_PML_DEFAULT_TASK_SLICE );
}

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_PRIO 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 pmlSidDefaultTaskPriority()
{
  return ( FD_CNFGR_PML_DEFAULT_TASK_PRI );
}

