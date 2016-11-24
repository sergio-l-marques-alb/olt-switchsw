/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename edb_sid.c
*
* @purpose Structural data for edb component
*
* @component edb
*
* @comments adapted from fdb_sid.c
*
* @create 07/08/2003
*
* @author cpverne
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "platform_config.h"
#include "edb_sid_const.h"

/*********************************************************************
* @purpose  Retrieve the Task priority for edb  
*
* @param    void 
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/

L7_uint32 edbSidTaskPriorityGet(void)
{
  return(EDB_SID_DEFAULT_TASK_PRI);
}

/*********************************************************************
* @purpose  Retrieve the stack size for edb task  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 edbSidTaskStackSizeGet(void)
{
  return(EDB_SID_DEFAULT_STACK_SIZE);
}

/*********************************************************************
* @purpose  Retrieve the task slice for edb task  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 edbSidTaskSliceGet(void)
{
  return(EDB_SID_DEFAULT_TASK_SLICE);
}

/*********************************************************************
* @purpose  Retrieve the number of Messages allowed in the queue
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 edbSidMsgCountGet(void)
{
  return(EDB_SID_DEFAULT_MSG_SIZE);
}

/*********************************************************************
* @purpose  Retrieve the number of seconds between entChangeTraps
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 edbSidTrapThrottleTimeGet(void)
{
  return(EDB_SID_DEFAULT_TRAP_THROTTLE_TIME);
}

