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
#ifndef FDB_SID_H
#define FDB_SID_H

#include "l7_common.h"
#include "platform_config.h"

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
L7_uint32 fdbSidTaskPriorityGet(void);

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
L7_uint32 fdbSidTaskStackSizeGet(void);

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
L7_uint32 fdbSidTaskSliceGet(void);

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
L7_uint32 fdbSidFdbMsgCountGet(void);

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
L7_uint32 fdbSidMaxVlanPerBridgeGet(void);

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
L7_uint32 fdbSidDefaultAgingTimeoutGet(void);

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
L7_uint32 fdbSidMaxFdbGet(void);

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
L7_FDB_TYPE_t fdbSidVlanLearningTypeGet(void);

#endif
