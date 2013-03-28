/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cnfgr_sid.h
*
* @purpose    Configurator component initialization header file that
*             contains the constants for the configurator 
*             structural initialization data. 
*
* @component  cnfgr
*
* @comments   none
*
* @create     03/17/2003
*
* @author     avasquez
* @end
*
**********************************************************************/
#ifndef INCLUDE_CNFGR_SID_CONST_H
#define INCLUDE_CNFGR_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

/* ===================================================================
 *  Configurator General--
 * ===================================================================
*/

enum {
       CNFGR_SEMAPHORE_OPTIONS = (OSAPI_SEM_Q_PRIORITY | OSAPI_SEM_INVERSION_SAFE) 
};

/* ===================================================================
 *  Component Control (CCTRL)--
 * ===================================================================
*/
enum {
       CNFGR_CTRL_CALLBACK_MAX     =  64,
       CNFGR_CTRL_CMDRQST_POOL_MAX =  128
};


/* =================================================================== 
 * Message Handler --
 * ===================================================================
*/

enum {
    CNFGR_MSG_LOG_MAX_COUNT             = 4,

/* Message Queues */
    CNFGR_MSG_NOOP_COUNT                = 50,             

/* Message Q1 - Receive messages when !EIP.
 *
 */
    CNFGR_MSG_Q1_COUNT                  = 128,

/* Message Q2 - Receive messages when EIP.
 *
 */
    CNFGR_MSG_Q2_COUNT                  = 128,   

/* Message Handler Thread -
 *
*/
    CNFGR_MSG_HANDLER_THREAD_PRIO       = L7_DEFAULT_TASK_PRIORITY,
    CNFGR_MSG_HANDLER_THREAD_STACK_SIZE = L7_DEFAULT_STACK_SIZE,  
    CNFGR_MSG_HANDLER_THREAD_SLICE      = L7_DEFAULT_TASK_SLICE  
};



/* =================================================================== 
 * Callback Tally --
 * ===================================================================
*/

enum {

/* Tally handle -  
 *
*/
    CNFGR_TALLY_HANDLE_MAX              = 4,            
    CNFGR_TALLY_RSP_LIST_MAX            = L7_LAST_COMPONENT_ID, 

/* Correlator pool -
 *
*/
    CNFGR_TALLY_CORRELATOR_MAX          = (L7_LAST_COMPONENT_ID),


/* Tally Watchdog Timer -  
 *
*/
    CNFGR_TALLY_WATCHDOG_TIME           = 3600, 

    CNFGR_TALLY_WATCHDOG_RETRY          = 1            

};

/* =================================================================== 
 * Component Repository --
 * ===================================================================
*/
enum {

    CNFGR_CR_COMPONENT_TABLE_END = L7_LAST_COMPONENT_ID
};


#endif /* INCLUDE_CNFGR_SID_CONST_H */
