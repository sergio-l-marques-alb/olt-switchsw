/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename ipmap_sid.c
*
* @purpose IP MAP Structural Initialization Database
*
* @component IP Map
*
* @comments none
*
* @create 07/13/2003
*
* @author markl
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "l3_default_cnfgr.h"
#include "ipmap_sid.h"
#include "ipmap_sid_const.h"


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
L7_int32 ipMapProcessingSidDefaultStackSize()
{
  return ( FD_IP_PROCESSING_DEFAULT_STACK_SIZE );
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
L7_int32 ipMapProcessingSidDefaultTaskSlice()
{
  return ( FD_IP_PROCESSING_DEFAULT_TASK_SLICE );
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
L7_int32 ipMapProcessingSidDefaultTaskPriority()
{
  return ( FD_IP_PROCESSING_DEFAULT_TASK_PRI );
}

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
L7_int32 ipMapForwardingSidDefaultStackSize()
{
  return ( FD_IP_FORWARDING_DEFAULT_STACK_SIZE );
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
L7_int32 ipMapForwardingSidDefaultTaskSlice()
{
  return ( FD_IP_FORWARDING_DEFAULT_TASK_SLICE );
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
L7_int32 ipMapForwardingSidDefaultTaskPriority()
{
  return ( FD_IP_FORWARDING_DEFAULT_TASK_PRI );
}

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
L7_int32 ipMapArpSidDefaultStackSize()
{
  return ( FD_IP_ARP_DEFAULT_STACK_SIZE );
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
L7_int32 ipMapArpSidDefaultTaskSlice()
{
  return ( FD_IP_ARP_DEFAULT_TASK_SLICE );
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
L7_int32 ipMapArpSidDefaultTaskPriority()
{
  return ( FD_IP_ARP_DEFAULT_TASK_PRI );
}

