/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dot1x_sid.h
*
* @purpose    Headers for functions returning constants
*
* @component  dot1x
*
* @comments   none
*
* @create     08/07/2003
*
* @author     mfiorito
* @end
*
**********************************************************************/
#ifndef INCLUDE_DOT1X_SID_H
#define INCLUDE_DOT1X_SID_H

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
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 dot1xSidDefaultStackSize();

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 dot1xSidDefaultTaskSlice();

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 dot1xSidDefaultTaskPriority();


#endif /* INCLUDE_DOT1X_SID_H */
