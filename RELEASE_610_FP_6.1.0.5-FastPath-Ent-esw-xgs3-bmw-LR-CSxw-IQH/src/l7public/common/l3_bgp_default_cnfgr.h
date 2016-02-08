/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l3_bgp_default_cnfgr.h
*
* @purpose All Factory default settings are in this file
*
* @component cnfgr
*
* @comments none
*
* @create 02/18/2002
*
* @author
* @end
*
**********************************************************************/

#ifndef INCLUDE_L3_BGP_DEFAULT_CNFGR
#define INCLUDE_L3_BGP_DEFAULT_CNFGR

#include "cnfgr.h"
#include "l3_default_cnfgr.h"

/* Start of CONFIGURATOR BGP Component's Factory Defaults */
/* Start of CONFIGURATOR BGP Component's Factory Defaults */
/* Start of CONFIGURATOR BGP Component's Factory Defaults */
/* Start of CONFIGURATOR BGP Component's Factory Defaults */
/* Start of CONFIGURATOR BGP Component's Factory Defaults */

#define FD_CNFGR_BGP_DEFAULT_STACK_SIZE      L7_L3_DEFAULT_STACK_SIZE
#define FD_CNFGR_BGP_DEFAULT_TASK_SLICE      L7_L3_DEFAULT_TASK_SLICE
#define FD_CNFGR_BGP_DEFAULT_TASK_PRI        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define L7_BGP_MAX_NUMBER_OF_ADDR_FAMILIES   10
#define L7_BGP_MAX_NUMBER_OF_SNPA            10
#define L7_BGP_MAX_NUMBER_OF_PEERS           10
#define L7_BGP_MAX_ADDR_AGG_ENTRIES          10
#define L7_BGP_MAX_NUMBER_OF_ROUTES          10
#define L7_BGP_MAX_NUM_OF_POL_ENTRIES        20
#define L7_BGP_MAX_NUM_OF_POL_ARGS           128

#define L7_BGP_MAX_NUM_OF_RANGE_ENTRIES      6
#define L7_BGP_MAX_NUM_OF_ACTION_ENTRIES     L7_BGP_MAX_NUM_OF_POL_ENTRIES

/* Maximum number of messages serviced by the BGP task before giving other tasks
a chance to run */

#define FD_CNFGR_BGP_MAX_MSGS_SERVICED      512

/* Maximum number of timer ticks to sleep when BGP task has serviced its max msgs */
#define FD_CNFGR_BGP_MAX_MSGS_TASK_DELAY     4

/*--------------------------------------*/
/*  BGP MAP TIMERS                      */
/*--------------------------------------*/
/* Task "wake-up" timer needed by some operating systems */
#define FD_CNFGR_BGP_TASK_TIMER   1000  /*ms*/

#define BGP_TIMER_TASK			"bgpTimer_t"
#define BGP_TIMER_STACK_SIZE_DEFAULT	(platTaskStackDefaultGet())
#define BGP_TIMER_TASK_PRI		L7_TASK_PRIORITY_LEVEL(L7_MEDIUM_TASK_PRIORITY)
#define BGP_TIMER_DEFAULT_TASK_SLICE	L7_DEFAULT_TASK_SLICE

#define BGP_TIMER_QUEUE		"Bgp-Timer-Q"
#define BGP_MSG_COUNT		256

/* How long BGP waits from the time RTO notifies it of best route changes
 * until BGP asks RTO for those changes. The delay allows RTO to process
 * several changes and send them all to BGP at one time and also allows
 * flaps to damp out. Make delay longer for BGP than for others. Slower
 * is always better for BGP! However 10 seconds is too slow for ANVL tests
 * 24.3, 24.5.  Changing to 5.9 secs. */
#define L7_BGP_BEST_ROUTE_DELAY    5900   /*ms*/ 

/* The number of best route changes that BGP requests from RTO. This controls
 * the size of a buffer allocated at init time and is not configurable at
 * run time. */
#define L7_BGP_MAX_BEST_ROUTE_CHANGES 50

/*--------------------------------------*/
/*  BGP MAP TASK INIT WAIT             */
/*--------------------------------------*/
/* Length of time to wait for task initialization */
#define FD_CNFGR_BGP_TASK_INIT_WAIT  L7_WAIT_FOREVER

/* At startup, BGP starts a short-lived task whose responsibility is
 * to register BGP with the ACL component to receive notification of
 * changes to access list configurations. The task quits as soon as
 * registration takes place.
 */
#define BGP_ACL_REGISTER_TASK			     "BGP ACL Registration Task"
#define BGP_ACL_REGISTER_STACK_SIZE_DEFAULT	 (platTaskStackDefaultGet())
#define BGP_ACL_REGISTER_TASK_PRI		      L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
#define BGP_ACL_REGISTER_DEFAULT_TASK_SLICE	  L7_DEFAULT_TASK_SLICE

/* End of CONFIGURATOR BGP Component's Factory Defaults */


#endif
