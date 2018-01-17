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
#ifndef DOT3AD_SID_H
#define DOT3AD_SID_H

#include "l7_common.h"

/*********************************************************************
* @purpose  Retrieve the Task priority for dot3ad 
*
* @param    void 
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot3adSidTimerTaskPriorityGet(void);

/*********************************************************************
* @purpose  Retrieve the stack size for dot3ad 
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot3adSidTimerTaskStackSizeGet(void);

/*********************************************************************
* @purpose  Retrieve the task slice 
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot3adSidTimerTaskSliceGet(void);


/*********************************************************************
* @purpose  Retrieve the Task priority for dot3ad 
*
* @param    void 
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot3adSidLacTaskPriorityGet(void);

/*********************************************************************
* @purpose  Retrieve the stack size for dot3ad 
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot3adSidLacTaskStackSizeGet(void);


/*********************************************************************
* @purpose  Retrieve the task slice 
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot3adSidLacTaskSliceGet(void);


/*********************************************************************
* @purpose  Retrieve the max number of msgs for the queue
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot3adSidTimerMsgCountGet(void);


/*********************************************************************
* @purpose  Retrieve the max number of msgs for the queue
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot3adSidLacMsgCountGet(void);

/*********************************************************************
* @purpose  Retrieve the maximum frame size for the LAG interface
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot3adSidMaxFrameSizeGet(void);

#endif
