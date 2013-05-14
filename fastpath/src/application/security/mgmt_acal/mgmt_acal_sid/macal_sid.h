/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2006
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename   macal_sid.h
*
* @purpose    Headers for functions returning constants
*
* @component  Management Access Control and Administration List
*
* @comments   none
*
* @create     05/05/2005
*
* @author     stamboli
* @end
*
**********************************************************************/
#ifndef INCLUDE_MACAL_SID_H
#define INCLUDE_MACAL_SID_H

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
L7_int32 macalSidDefaultStackSize();

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
L7_int32 macalSidDefaultTaskSlice();

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
L7_int32 macalSidDefaultTaskPriority();


#endif /* INCLUDE_MACAL_SID_H */
