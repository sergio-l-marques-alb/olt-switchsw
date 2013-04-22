/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pimdmmap_sid.h
*
* @purpose    Headers for functions returning constants
*
* @component  PIM-DM 
*
* @comments   none
*
* @create     11/12/2003
*
* @author     mfiorito
* @end
*
**********************************************************************/
#ifndef INCLUDE_PIMDMMAP_SID_H
#define INCLUDE_PIMDMMAP_SID_H

#include "commdefs.h"
#include "datatypes.h"
#include "l7_product.h"

/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    void    
*
* @returns  FD_CNFGR_PIMDM_MAP_DEFAULT_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimdmMapSidDefaultStackSize();

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  FD_CNFGR_PIMDM_MAP_DEFAULT_TASK_SLICE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimdmMapSidDefaultTaskSlice();

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  FD_CNFGR_PIMDM_MAP_DEFAULT_TASK_PRI 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimdmMapSidDefaultTaskPriority();


#endif /* INCLUDE_PIMDMMAP_SID_H */
