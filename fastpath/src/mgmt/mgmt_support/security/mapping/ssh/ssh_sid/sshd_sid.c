/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename sshd_sid.c
*
* @purpose SSHD Structural Initialization Database
*
* @component sshd
*
* @comments none
*
* @create 10/13/2003
*
* @author mfiorito
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "sshd_sid.h"
#include "sshd_sid_const.h"

/*********************************************************************
* @purpose  Get thread default Stack Size
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 sshdSidDefaultStackSize()
{
  return ( FD_CNFGR_SSHD_DEFAULT_STACK_SIZE );
}

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 sshdSidDefaultTaskSlice()
{
  return ( FD_CNFGR_SSHD_DEFAULT_TASK_SLICE );
}

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 sshdSidDefaultTaskPriority()
{
  return ( FD_CNFGR_SSHD_DEFAULT_TASK_PRI );
}
