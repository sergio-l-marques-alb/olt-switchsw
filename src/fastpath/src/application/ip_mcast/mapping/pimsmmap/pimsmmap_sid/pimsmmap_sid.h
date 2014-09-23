/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pimsmmap_sid.h
*
* @purpose    Headers for functions returning constants
*
* @component  PIM-SM 
*
* @comments   none
*
* @create     11/12/2003
*
* @author     mfiorito
* @end
*
**********************************************************************/
#ifndef INCLUDE_PIMSMMAP_SID_H
#define INCLUDE_PIMSMMAP_SID_H

#include "commdefs.h"
#include "datatypes.h"
#include "l7_product.h"

/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    none 
*
* @returns  FD_CNFGR_PIMSM_MAP_DEFAULT_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimsmMapSidDefaultStackSize();

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    none 
*
* @returns  FD_CNFGR_PIMSM_MAP_DEFAULT_TASK_SLICE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimsmMapSidDefaultTaskSlice();

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    none 
*
* @returns  FD_CNFGR_PIMSM_MAP_DEFAULT_TASK_PRI 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 pimsmMapSidDefaultTaskPriority();


#endif /* INCLUDE_PIMSMMAP_SID_H */
