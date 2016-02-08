/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename voip_sid.c
*
* @purpose VOIP Structural Initialization Database
*
* @component VOIP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
*
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "voip_sid.h"
#include "voip_sid_const.h"

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
L7_int32 voipSidDefaultStackSize()
{
  return ( FD_CNFGR_VOIP_DEFAULT_STACK_SIZE );
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
L7_int32 voipSidDefaultTaskSlice()
{
  return ( FD_CNFGR_VOIP_DEFAULT_TASK_SLICE );
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
L7_int32 voipSidDefaultTaskPriority()
{
  return ( FD_CNFGR_VOIP_DEFAULT_TASK_PRI );
}

