/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pml_sid.h
*
* @purpose    Headers for functions returning constants
*
* @component  Port MAC Locking
*
* @comments   none
*
* @create     05/21/2004
*
* @author     colinw
* @end
*
**********************************************************************/
#ifndef INCLUDE_PML_SID_H
#define INCLUDE_PML_SID_H

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
L7_int32 pmlSidDefaultStackSize();

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
L7_int32 pmlSidDefaultTaskSlice();

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
L7_int32 pmlSidDefaultTaskPriority();


#endif /* INCLUDE_PML_SID_H */
