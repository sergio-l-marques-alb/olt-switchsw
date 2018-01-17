/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dns_client_sid.h
*
* @purpose    Headers for functions returning constants
*
* @component  DNS Client
*
* @comments   none
*
* @create     02/28/05
*
* @author     dfowler
* @end
*
**********************************************************************/
#ifndef INCLUDE_DNS_CLIENT_SID_H
#define INCLUDE_DNS_CLIENT_SID_H

#include "l7_common.h"

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
L7_int32 dnsSidDefaultStackSize();

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
L7_int32 dnsSidDefaultTaskSlice();

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
L7_int32 dnsSidDefaultTaskPriority();


#endif /* INCLUDE_DNS_CLIENT_SID_H */
