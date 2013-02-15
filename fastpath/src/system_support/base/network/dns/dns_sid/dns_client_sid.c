/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dns_client_sid.c
*
* @purpose DNS Client Structural Initialization Database
*
* @component DNS Client
*
* @comments none
*
* @create 02/28/2005
*
* @author dfowler
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "dns_client_sid.h"
#include "dns_client_sid_const.h"


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
L7_int32 dnsSidDefaultStackSize()
{
  return ( FD_CNFGR_DNS_CLIENT_DEFAULT_STACK_SIZE );
}

/*********************************************************************
* @purpose  Get thread default task slice
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_SLICE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 dnsSidDefaultTaskSlice()
{
  return ( FD_CNFGR_DNS_CLIENT_DEFAULT_TASK_SLICE );
}

/*********************************************************************
* @purpose  Get thread default task priority
*
* @param    void    
*
* @returns  CNFGR_MSG_HANDLER_THREAD_PRIO 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 dnsSidDefaultTaskPriority()
{
  return ( FD_CNFGR_DNS_CLIENT_DEFAULT_TASK_PRI );
}

