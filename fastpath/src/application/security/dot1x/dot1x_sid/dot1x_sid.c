/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1x_sid.c
*
* @purpose dot1x Structural Initialization Database
*
* @component dot1x
*
* @comments none
*
* @create 08/07/2003
*
* @author mfiorito
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "dot1x_sid.h"
#include "dot1x_sid_const.h"


/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 dot1xSidDefaultStackSize()
{
  return ( FD_CNFGR_DOT1X_DEFAULT_STACK_SIZE );
}

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 dot1xSidDefaultTaskSlice()
{
  return ( FD_CNFGR_DOT1X_DEFAULT_TASK_SLICE );
}

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @comments none 
*       
* @end
*********************************************************************/
L7_int32 dot1xSidDefaultTaskPriority()
{
  return ( FD_CNFGR_DOT1X_DEFAULT_TASK_PRI );
}

