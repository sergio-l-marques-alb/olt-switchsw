/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   sshd_sid.h
*
* @purpose    Headers for functions returning constants
*
* @component  sshd
*
* @comments   none
*
* @create     10/13/2003
*
* @author     mfiorito
* @end
*
**********************************************************************/
#ifndef INCLUDE_SSHD_SID_H
#define INCLUDE_SSHD_SID_H

#include "datatypes.h"

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
L7_int32 sshdSidDefaultStackSize();

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
L7_int32 sshdSidDefaultTaskSlice();

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
L7_int32 sshdSidDefaultTaskPriority();


#endif /* INCLUDE_SSHD_SID_H */
