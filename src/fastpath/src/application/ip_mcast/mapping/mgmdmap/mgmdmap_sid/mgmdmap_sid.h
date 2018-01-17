/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   mgmdmap_sid.h
*
* @purpose    Headers for functions returning constants
*
* @component  mgmd 
*
* @comments   none
*
* @create     09/10/2003
*
* @author     jeffr
* @end
*
**********************************************************************/
#ifndef INCLUDE_MGMDMAP_SID_H
#define INCLUDE_MGMDMAP_SID_H

#include "commdefs.h"
#include "datatypes.h"
#include "l7_product.h"

/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    void    
*
* @returns  FD_CNFGR_MGMD_MAP_DEFAULT_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 mgmdMapSidDefaultStackSize();

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  FD_CNFGR_MGMD_MAP_DEFAULT_TASK_SLICE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 mgmdMapSidDefaultTaskSlice();

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  FD_CNFGR_MGMD_MAP_DEFAULT_TASK_PRI 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 mgmdMapSidDefaultTaskPriority();


#endif /* INCLUDE_MGMDMAP_SID_H */
