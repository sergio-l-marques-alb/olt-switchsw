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
* @comments adapted from fdb_sid.h
*
* @create 07/08/2003
*
* @author cpverne
*
* @end
*             
**********************************************************************/
#ifndef SNMP_SID_H
#define SNMP_SID_H

#include "l7_common.h"
#include "platform_config.h"

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
L7_uint32 snmpSidTaskPriorityGet(void);

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
L7_uint32 snmpSidTaskStackSizeGet(void);

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
L7_uint32 snmpSidTaskSliceGet(void);

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
L7_uint32 snmpSidMsgCountGet(void);

#endif
