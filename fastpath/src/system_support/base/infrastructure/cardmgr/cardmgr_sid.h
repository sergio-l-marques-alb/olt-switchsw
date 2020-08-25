/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cardmgr_sid.h
*
* @purpose    Card Manager component initialization header file that
*             contains the structurat initialization data for the
*             Card Manager.
*
* @component  card manager
*
* @comments   none
*
* @create     07/10/2003
*
* @author     avasquez
* @end
*
**********************************************************************/
#ifndef INCLUDE_CARD_MANAGER_SID_H
#define INCLUDE_CARD_MANAGER_SID_H

#include "commdefs.h"
#include "datatypes.h"
#include "l7_product.h"

/*
***********************************************************************
*                          Function Prototypes
***********************************************************************
*/

/* Card Manager Protocol */
 
/*********************************************************************
* @purpose  Get transmit message interval in seconds
*
* @param    void    
*
* @returns  L7_CMGR_TX_SEC 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cmgrSidTxSecGet();

/*********************************************************************
* @purpose  Get init value for receive message inteval in seconds
*
* @param    void    
*
* @returns  L7_CMGR_INIT_RX_SEC 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cmgrSidInitRxSecGet();

/*********************************************************************
* @purpose  Get transmit message interval
*
* @param    void    
*
* @returns  L7_CMGR_TIMER_INTERVAL 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cmgrSidTimerIntervalGet();

/* Card Manager Message Queue */

/*********************************************************************
* @purpose  Get message count 
*
* @param    void    
*
* @returns  L7_CMGR_MSGQ_COUNT 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cmgrSidMsgQCountGet();

/*********************************************************************
* @purpose  Get message size interval
*
* @param    void    
*
* @returns  L7_CMGR_MSGQ_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cmgrSidMsgQSizeGet();

/* Card Manager Task */

/*********************************************************************
* @purpose  Get task syncronization
*
* @param    void    
*
* @returns  L7_CMGR_TASK_SYNC 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cmgrSidTaskSyncGet();

/*********************************************************************
* @purpose  Get task priority level
*
* @param    void    
*
* @returns  L7_CMGR_TASK_PRIORITY 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cmgrSidTaskPriorityGet();

/*********************************************************************
* @purpose  Get task stack size    
*
* @param    void    
*
* @returns  L7_CMGR_TASK_STACK_SIZE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cmgrSidTaskStackSizeGet();

/*********************************************************************
* @purpose  Get task slice value    
*
* @param    void    
*
* @returns  L7_CMGR_TASK_SLICE 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cmgrSidTaskSliceGet();

#endif /* INCLUDE_CARD_MANAGER_SID_H */
