/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dtl_sid.c
*
* @purpose Structural data for DTL
*
* @component dtl
*
* @comments none
*
* @create 06/20/2003
*
* @author mbaucom
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "default_cnfgr.h"

/*********************************************************************
* @purpose  Retrieve the Task priority for DTL  
*
* @param    void 
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/

L7_uint32 dtlSidTaskPriorityGet(void)
{
  return(FD_CNFGR_DTL_DEFAULT_TASK_PRI);
}

/*********************************************************************
* @purpose  Retrieve the stack size for DTL task  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dtlSidTaskStackSizeGet(void)
{
  return(FD_CNFGR_DTL_DEFAULT_STACK_SIZE);
}

/*********************************************************************
* @purpose  Retrieve the task slice for DTL task  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dtlSidTaskSliceGet(void)
{
  return(FD_CNFGR_DTL_DEFAULT_TASK_SLICE);
}
