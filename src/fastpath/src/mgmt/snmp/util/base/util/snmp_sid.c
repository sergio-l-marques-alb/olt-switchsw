/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename snmp_sid.c
*
* @purpose Structural data for snmp component
*
* @component snmp
*
* @comments adapted from fdb_sid.c
*
* @create 07/09/2003
*
* @author cpverne
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "platform_config.h"
#include "snmp_sid_const.h"

/*********************************************************************
* @purpose  Retrieve the Task priority for snmp  
*
* @param    void 
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/

L7_uint32 snmpSidTaskPriorityGet(void)
{
  return(SNMP_SID_DEFAULT_TASK_PRI);
}

/*********************************************************************
* @purpose  Retrieve the stack size for snmp task  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 snmpSidTaskStackSizeGet(void)
{
  return(SNMP_SID_DEFAULT_STACK_SIZE);
}

/*********************************************************************
* @purpose  Retrieve the task slice for snmp task  
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 snmpSidTaskSliceGet(void)
{
  return(SNMP_SID_DEFAULT_TASK_SLICE);
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
L7_uint32 snmpSidMsgCountGet(void)
{
  return(SNMP_SID_DEFAULT_MSG_SIZE);
}

