/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename lldp_sid.c
*
* @purpose 802.1AB Structural Initialization Database
*
* @component 802.1AB
*
* @comments none
*
* @create 02/01/2005
*
* @author dfowler
*
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "lldp_sid.h"
#include "lldp_sid_const.h"


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
L7_int32 lldpSidDefaultStackSize()
{
  return ( FD_CNFGR_LLDP_DEFAULT_STACK_SIZE );
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
L7_int32 lldpSidDefaultTaskSlice()
{
  return ( FD_CNFGR_LLDP_DEFAULT_TASK_SLICE );
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
L7_int32 lldpSidDefaultTaskPriority()
{
  return ( FD_CNFGR_LLDP_DEFAULT_TASK_PRI );
}
