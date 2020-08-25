/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename sslt_sid.c
*
* @purpose SSL Tunnel Structural Initialization Database
*
* @component sslt
*
* @comments none
*
* @create 10/22/2003
*
* @author spetriccione
*
* @end
*             
**********************************************************************/
#include "sslt_include.h"

/*********************************************************************
* @purpose Get thread default Stack Size
*
* @returns CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 ssltSidDefaultStackSize()
{
  return (FD_CNFGR_SSLT_DEFAULT_STACK_SIZE);
}

/*********************************************************************
* @purpose Get thread default task slice
*
* @returns CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 ssltSidDefaultTaskSlice()
{
  return (FD_CNFGR_SSLT_DEFAULT_TASK_SLICE);
}

/*********************************************************************
* @purpose Get thread default task priority
*
* @returns CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 ssltSidDefaultTaskPriority()
{
  return (FD_CNFGR_SSLT_DEFAULT_TASK_PRI);
}

