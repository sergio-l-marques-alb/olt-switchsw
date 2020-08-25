/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q_sid.c
*
* @purpose Structural data for dot1q
*
* @component dot1q
*
* @comments none
*
* @create 08/25/2005
*
* @author Amit Kulkarni
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "platform_config.h"
#include "defaultconfig.h"
#include "dot1q.h"

/*********************************************************************
* @purpose  Retrieve the Task priority for dot1q 
*
* @param    void 
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot1qSidTaskPriorityGet(void)
{
  return L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY);    
}

/*********************************************************************
* @purpose  Retrieve the stack size for dot1q 
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 dot1qSidTaskStackSizeGet(void)
{
#ifdef L7_PRODUCT_SMARTPATH 
  return (2*L7_DEFAULT_STACK_SIZE);
#else
  return (L7_DEFAULT_STACK_SIZE);
#endif
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
L7_uint32 dot1qSidTaskSliceGet(void)
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
L7_uint32 dot1qSidMsgCountGet(void)
{
  return DOT1Q_MSG_COUNT;
}



