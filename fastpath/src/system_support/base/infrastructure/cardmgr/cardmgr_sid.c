/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename cardmgr_sid.c
*
* @purpose Card Manager Structural Initialization Database
*
* @component card manager
*
* @comments none
*
* @create 07/10/2003
*
* @author avasquez
*
* @end
*             
**********************************************************************/

#include "cardmgr_sid.h"
#include "cardmgr_sid_const.h"
#include "cardmgr_api.h"

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
L7_int32 cmgrSidTxSecGet()
{
  return (L7_CMGR_TX_SEC);
}

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
L7_int32 cmgrSidInitRxSecGet()
{
  return (L7_CMGR_INIT_RX_SEC);
}

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
L7_int32 cmgrSidTimerIntervalGet()
{
  return (L7_CMGR_TIMER_INTERVAL);
}


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
L7_int32 cmgrSidMsgQCountGet()
{
  return (L7_CMGR_MSGQ_COUNT);
}

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
L7_int32 cmgrSidMsgQSizeGet()
{
  return sizeof(cmgr_cmpdu_t);
}

/* Card Manager Task */

/*********************************************************************
* @purpose  Get transmit message interval
*
* @param    void    
*
* @returns  L7_CMGR_TASK_SYNC 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_int32 cmgrSidTaskSyncGet()
{
  return (L7_CMGR_TASK_SYNC);
}

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
L7_int32 cmgrSidTaskPriorityGet()
{
    return (L7_CMGR_TASK_PRIORITY);
}

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
L7_int32 cmgrSidTaskStackSizeGet()
{ 
    return (L7_CMGR_TASK_STACK_SIZE);
}

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
L7_int32 cmgrSidTaskSliceGet()
{ 
    return (L7_CMGR_TASK_SLICE);
}
