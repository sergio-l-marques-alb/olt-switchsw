/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dos_sid.c
*
* @purpose DOS Structural Initialization Database
*
* @component DENIAL OF SERVICE
*
* @comments none
*
* @create 04/13/2007
*
* @author aprashant
*
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "dos_sid.h"
#include "dos_sid_const.h"

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
L7_int32 dosSidDefaultStackSize()
{
  return ( FD_CNFGR_DOS_DEFAULT_STACK_SIZE );
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
L7_int32 dosSidDefaultTaskSlice()
{
  return ( FD_CNFGR_DOS_DEFAULT_TASK_SLICE );
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
L7_int32 dosSidDefaultTaskPriority()
{
  return ( FD_CNFGR_DOS_DEFAULT_TASK_PRI );
}

