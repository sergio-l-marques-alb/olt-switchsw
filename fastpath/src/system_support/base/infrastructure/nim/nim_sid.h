/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename nim_sid.h
*
* @purpose Structural data for nim
*
* @component nim
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
#ifndef NIM_SID_H
#define NIM_SID_H

#include "l7_common.h"
#include "platform_config.h"

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
L7_uint32 nimSidTaskPriorityGet(void);

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
L7_uint32 nimSidTaskStackSizeGet(void);

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
L7_uint32 nimSidTaskSliceGet(void);

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
L7_uint32 nimSidMsgCountGet(void);


#endif
