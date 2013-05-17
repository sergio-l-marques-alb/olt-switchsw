/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   lldp_sid.h
*
* @purpose    Headers for functions returning constants
*
* @component  802.1AB
*
* @comments   none
*
* @create     02/01/2005
*
* @author     dfowler
* @end
*
**********************************************************************/
#ifndef INCLUDE_LLDP_SID_H
#define INCLUDE_LLDP_SID_H

#include "l7_common.h"

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
L7_int32 lldpSidDefaultStackSize();

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
L7_int32 lldpSidDefaultTaskSlice();

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
L7_int32 lldpSidDefaultTaskPriority();

#endif /* INCLUDE_LLDP_SID_H */
