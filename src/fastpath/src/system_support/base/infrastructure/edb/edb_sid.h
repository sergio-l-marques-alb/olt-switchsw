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
* @comments adapted from fdb_sid.h
*
* @create 07/08/2003
*
* @author cpverne
*
* @end
*             
**********************************************************************/
#ifndef EDB_SID_H
#define EDB_SID_H

#include "l7_common.h"
#include "platform_config.h"

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
L7_uint32 edbSidTaskPriorityGet(void);

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
L7_uint32 edbSidTaskStackSizeGet(void);

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
L7_uint32 edbSidTaskSliceGet(void);

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
L7_uint32 edbSidMsgCountGet(void);

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
L7_uint32 edbSidMsgCountGet(void);

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
L7_uint32 edbSidTrapThrottleTimeGet(void);
#endif
