
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
#include "fdb_sid_const.h"
#include "fdb_exports.h"

/*********************************************************************
* @purpose  Retrieve the Task priority for fdb  
*
* @param    void 
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/

L7_uint32 fdbSidTaskPriorityGet(void)
{
  return(FDB_SID_DEFAULT_TASK_PRI);
}

/*********************************************************************
* @purpose  Retrieve the stack size for fdb task  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 fdbSidTaskStackSizeGet(void)
{
  return(FDB_SID_DEFAULT_STACK_SIZE);
}

/*********************************************************************
* @purpose  Retrieve the task slice for fdb task  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 fdbSidTaskSliceGet(void)
{
  return(FDB_SID_DEFAULT_TASK_SLICE);
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
L7_uint32 fdbSidFdbMsgCountGet(void)
{
  return(platFdbTotalMacEntriesGet()*2);
}

/*********************************************************************
* @purpose  Retrieve the max Vlans per bridge
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 fdbSidMaxVlanPerBridgeGet(void)
{
  return(FDB_SID_MAX_VLAN_PER_BRIDGE);
}

/*********************************************************************
* @purpose  Retrieve default aging time
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 fdbSidDefaultAgingTimeoutGet(void)
{
  return(FDB_SID_DEFAULT_AGING_TIMEOUT);
}

/*********************************************************************
* @purpose  Retrieve max FDBs
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 fdbSidMaxFdbGet(void)
{
  return(FDB_SID_MAX_FILTERING_DATABASES);
}

/*********************************************************************
* @purpose  Retrieve max VLAN Learning Type Get
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_FDB_TYPE_t fdbSidVlanLearningTypeGet(void)
{
  return(FDB_SID_VLAN_LEARNING_TYPE);
}
