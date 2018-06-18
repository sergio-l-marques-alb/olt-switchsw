/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename sslt_sid.h
*
* @purpose Headers for functions returning constants
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
#ifndef INCLUDE_SSLT_SID_H
#define INCLUDE_SSLT_SID_H

#include "commdefs.h"
#include "datatypes.h"
#include "l7_product.h"

/*********************************************************************
* @purpose Get thread default Stack Size
*
* @returns CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 ssltSidDefaultStackSize();

/*********************************************************************
* @purpose Get thread default task slice
*
* @returns CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 ssltSidDefaultTaskSlice();

/*********************************************************************
* @purpose Get thread default task priority
*
* @returns CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 ssltSidDefaultTaskPriority();

#endif /* INCLUDE_SSLT_SID_H */

