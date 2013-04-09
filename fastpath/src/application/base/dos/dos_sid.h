/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dos_sid.h
*
* @purpose    Headers for functions returning constants
*
* @component  DENIAL OF SERVICE
*
* @comments   none
*
* @create     04/13/2007
*
* @author     aprashant
* @end
*
**********************************************************************/
#ifndef INCLUDE_DOS_SID_H
#define INCLUDE_DOS_SID_H

#include "commdefs.h"
#include "datatypes.h"
#include "l7_product.h"

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
L7_int32 dosSidDefaultStackSize();

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
L7_int32 dosSidDefaultTaskSlice();

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
L7_int32 dosSidDefaultTaskPriority();

#endif
