/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot3ad_sid.c
*
* @purpose Structural data for dot3ad
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
#include "l7_resources.h"
#include "platform_config.h"
#include "dot3ad_sid_const.h" 
#include "defaultconfig.h"

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
L7_uint32 dot3adSidTimerTaskPriorityGet(void)
{
  return L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY);    
}

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
L7_uint32 dot3adSidTimerTaskStackSizeGet(void)
{
  return L7_DEFAULT_STACK_SIZE;
}

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
L7_uint32 dot3adSidTimerTaskSliceGet(void)
{
  return L7_DEFAULT_TASK_SLICE;
}

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
L7_uint32 dot3adSidLacTaskPriorityGet(void)
{
  return L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY);    
}

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
L7_uint32 dot3adSidLacTaskStackSizeGet(void)
{
  return L7_DEFAULT_STACK_SIZE;
}

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
L7_uint32 dot3adSidLacTaskSliceGet(void)
{
  return L7_DEFAULT_TASK_SLICE;
}

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
L7_uint32 dot3adSidTimerMsgCountGet(void)
{
  return DOT3AD_SID_MSG_COUNT;
}

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
L7_uint32 dot3adSidLacMsgCountGet(void)
{
  return DOT3AD_SID_MSG_COUNT;
}


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
L7_uint32 dot3adSidMaxFrameSizeGet(void)
{
  return DOT3AD_SID_LAG_MAX_FRAME_SIZE;
}


