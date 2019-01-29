/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename iscsi_sid.c
*
* @purpose ISCSI Structural Initialization Database
*
* @component ISCSI
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
#include "iscsi_sid.h"
#include "iscsi_sid_const.h"

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
L7_int32 iscsiSidDefaultStackSize()
{
  return ( FD_CNFGR_ISCSI_DEFAULT_STACK_SIZE );
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
L7_int32 iscsiSidDefaultTaskSlice()
{
  return ( FD_CNFGR_ISCSI_DEFAULT_TASK_SLICE );
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
L7_int32 iscsiSidDefaultTaskPriority()
{
  return ( FD_CNFGR_ISCSI_DEFAULT_TASK_PRI );
}

