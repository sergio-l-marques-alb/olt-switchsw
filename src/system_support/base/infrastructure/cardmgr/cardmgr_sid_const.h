/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cardmgr_sid.h
*
* @purpose    Card Manager component initialization header file that
*             contains the constants for the Card Manager 
*             structural initialization data. 
*
* @component  card manager
*
* @comments   none
*
* @create     06/10/2003
*
* @author     avasquez
* @end
*
**********************************************************************/
#ifndef INCLUDE_CARD_MANAGER_SID_CONST_H
#define INCLUDE_CARD_MANAGER_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

/* Card Manager Protocol */
enum
{
L7_CMGR_TX_SEC          = 15,
L7_CMGR_INIT_RX_SEC     = 3,
L7_CMGR_TIMER_INTERVAL  = 15000 /* 15 seconds in milliseconds */
};

/* Card Manager Message Queue */
#define L7_CMGR_MSGQ_COUNT 32

/* Card Manager Task */
enum
{
    L7_CMGR_TASK_SYNC       = 3, 
    L7_CMGR_TASK_STACK_SIZE = L7_DEFAULT_STACK_SIZE,
    L7_CMGR_TASK_PRIORITY   = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
    L7_CMGR_TASK_SLICE      = L7_DEFAULT_TASK_SLICE
};

#endif /* INCLUDE_CARD_MANAGER_SID_CONST_H */
