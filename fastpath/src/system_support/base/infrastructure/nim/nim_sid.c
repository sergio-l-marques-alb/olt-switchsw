/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename fdb_sid.c
*
* @purpose Structural data for fdb
*
* @component fdb
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
#include "platform_config.h"
#include "nim_sid_const.h" 
#include "defaultconfig.h"

/*********************************************************************
* @purpose  Retrieve the Task priority for nim 
*
* @param    void 
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 nimSidTaskPriorityGet(void)
{
  return NIM_SID_DEFAULT_TASK_PRI;    
}

/*********************************************************************
* @purpose  Retrieve the stack size for nimtask  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 nimSidTaskStackSizeGet(void)
{
  return NIM_SID_DEFAULT_STACK_SIZE;
}

/*********************************************************************
* @purpose  Retrieve the task slice for nimtask  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 nimSidTaskSliceGet(void)
{
  return NIM_SID_DEFAULT_TASK_SLICE;
}

/*********************************************************************
* @purpose  Retrieve the task slice for nimtask  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 nimSidMsgCountGet(void)
{
    return (5 * platVlanTotalMaxEntriesGet() + 5 * platIntfTotalMaxCountGet());

/*  return (platVlanTotalMaxEntriesGet() * platIntfTotalMaxCountGet()); */
}

