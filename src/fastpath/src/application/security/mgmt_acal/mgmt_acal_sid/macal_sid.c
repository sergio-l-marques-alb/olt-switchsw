/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2000-2006
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
* @filename macal_sid.c
*
* @purpose Management Access Control and Administration List Structural Initialization Database
*
* @component Management Access Control and Administration List
*
* @comments none
*
* @create 05/05/2005
*
* @author stamboli
*
* @end
*             
**********************************************************************/
#include "l7_common.h"
#include "macal_sid.h"
#include "macal_sid_const.h"


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
L7_int32 macalSidDefaultStackSize()
{
  return ( FD_CNFGR_MACAL_DEFAULT_STACK_SIZE );
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
L7_int32 macalSidDefaultTaskSlice()
{
  return ( FD_CNFGR_MACAL_DEFAULT_TASK_SLICE );
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
L7_int32 macalSidDefaultTaskPriority()
{
  return ( FD_CNFGR_MACAL_DEFAULT_TASK_PRI );
}

