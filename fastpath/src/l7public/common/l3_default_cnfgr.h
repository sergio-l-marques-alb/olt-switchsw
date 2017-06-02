/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename default_configurator.h
*
* @purpose All Factory default settings are in this file
*
* @component cnfgr
*
* @comments none
*
* @create 09/15/2000
*
* @author bmutz
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#ifndef INCLUDE_L3_DEFAULT_CNFGR
#define INCLUDE_L3_DEFAULT_CNFGR
#include "cnfgr.h"


#define L7_L3_DEFAULT_STACK_SIZE  L7_DEFAULT_STACK_SIZE
#define L7_L3_DEFAULT_TASK_SLICE   L7_DEFAULT_TASK_SLICE

/* Start of CONFIGURATOR IP Component's Factory Defaults */
/* Start of CONFIGURATOR IP Component's Factory Defaults */
/* Start of CONFIGURATOR IP Component's Factory Defaults */
/* Start of CONFIGURATOR IP Component's Factory Defaults */
/* Start of CONFIGURATOR IP Component's Factory Defaults */

#define FD_CNFGR_IP_DEFAULT_STACK_SIZE     (L7_L3_DEFAULT_STACK_SIZE * 2)
#define FD_CNFGR_IP_DEFAULT_TASK_SLICE     L7_L3_DEFAULT_TASK_SLICE
#define FD_CNFGR_IP_DEFAULT_TASK_PRI       L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define FD_CNFGR_IP_ARP_DEFAULT_STACK_SIZE L7_L3_DEFAULT_STACK_SIZE
#define FD_CNFGR_IP_ARP_DEFAULT_TASK_SLICE L7_L3_DEFAULT_TASK_SLICE
#define FD_CNFGR_IP_ARP_DEFAULT_TASK_PRI   L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

/* Maximum number of messages serviced by the IP Forwarding task before giving other tasks
   a chance to run */
#define FD_CNFGR_IP_MAX_MSGS_SERVICED      512

/* Port types accecpted in ipMapIsValidIntfType() */
#define L7_IPMAP_MAX_ASYNC_EVENTS      L7_IPMAP_INTF_MAX_COUNT

/* This default has been changed from 10 to 60 seconds. Timeouts
 * occurred at 10 seconds when deleting an IP address on an interface
 * where > 2000 routes had been learned.
 */
#define L7_IPMAP_MAX_ASYNC_EVENT_TIMEOUT    60000      /* milliseconds */

/* How long IP MAP waits from the time RTO notifies it of best route changes
 * until IP MAP asks RTO for those changes. The delay allows RTO to process
 * several changes and send them all to IP MAP at one time and also allows
 * flaps to damp out. Shorter than delay for BGP, RIP, or OSPF so forwarding
 * table gets updated first. */
#define L7_IPMAP_BEST_ROUTE_DELAY    10   /*ms*/ 

/* Max number of best route changes IP MAP can get from RTO at once. */
#define L7_IPMAP_MAX_BEST_ROUTE_CHANGES 100

/* Max msgs serviced by the ARP Reissue task before yielding to other tasks */
#define FD_CNFGR_ARP_REISSUE_MSG_SERVICE_MAX    128
/* Max ARP Reissue task message queue messages. If hapi reports an error
 * inserting an ARP entry in hw, a message is queued to the ARP reissue task.
 * Since hapi now handles ARP changes asynchronously, failures are unlikely.
 * A small queue is probably satisfactory. */
#define FD_CNFGR_ARP_REISSUE_MSG_Q_COUNT        100
/* Sleep duration (in seconds) between task iterations */
#define FD_CNFGR_ARP_REISSUE_SLEEP_TIME         20
/* Number of device ARP add attempts before giving up (the +1 is for initial add) */
#define FD_CNFGR_ARP_REISSUE_DEV_FAIL_MAX       (3 +1)

/* Max msgs serviced by the ARP Callback task before yielding to other tasks */
#define FD_CNFGR_ARP_CALLBACK_MSG_SERVICE_MAX   128
/* Max timer ticks to sleep when ARP Reissue task yields to other tasks */
#define FD_CNFGR_ARP_CALLBACK_MSG_TASK_DELAY    2

/* Max ARP callback task message queue messages. Used to notify clients 
 * that ARP resolution has completed. Used during forwarding. This use limited
 * by the number of mbufs. */
#define FD_CNFGR_ARP_CALLBACK_MSG_Q_COUNT  L7_MAX_NETWORK_BUFF_PER_BOX

/* Max timer ticks to sleep when ARP Reissue task yields to other tasks */
#define FD_CNFGR_ARP_TIMEREXP_MSG_TASK_DELAY    2

/* End of CONFIGURATOR IP Component's Factory Defaults */


/* Start of CONFIGURATOR OSPF Component's Factory Defaults */
/* Start of CONFIGURATOR OSPF Component's Factory Defaults */
/* Start of CONFIGURATOR OSPF Component's Factory Defaults */
/* Start of CONFIGURATOR OSPF Component's Factory Defaults */
/* Start of CONFIGURATOR OSPF Component's Factory Defaults */

#define FD_CNFGR_OSPF_DEFAULT_STACK_SIZE      L7_L3_DEFAULT_STACK_SIZE
#define FD_CNFGR_OSPF_DEFAULT_TASK_SLICE      L7_L3_DEFAULT_TASK_SLICE
#define FD_CNFGR_OSPF_DEFAULT_TASK_PRI        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)


/* Maximum number of messages serviced by the OSPF task before giving other tasks
a chance to run */
#define FD_CNFGR_OSPF_MAX_MSGS_SERVICED      512

/* Maximum number of timer ticks to sleep when OSPF task has serviced its max msgs */
#define FD_CNFGR_OSPF_MAX_MSGS_TASK_DELAY     4


/*--------------------------------------*/
/*  OSPF MAP TIMERS                     */
/*--------------------------------------*/
/* Task "wake-up" timer needed by some operating systems */
#define FD_CNFGR_OSPF_TASK_TIMER   1000  /*ms*/

/*--------------------------------------*/
/*  OSPF MAP TASK INIT WAIT             */
/*--------------------------------------*/
/* Length of time to wait for task initialization */
#define FD_CNFGR_OSPF_TASK_INIT_WAIT  L7_WAIT_FOREVER

#define OSPF_PROC_TASK "OSPF mapping Task"
#define OSPF_PROC_QUEUE "OSPF task queue"
#define OSPF_REDIST_QUEUE "OSPF redist queue"

#define OSPFV3_PROC_TASK "OSPFV3 mapping Task"
#define OSPFV3_PROC_QUEUE "OSPFV3 task queue"
#define OSPFV3_REDIST_QUEUE "OSPFV3 redist queue"

/* The number of messages that can be queued in the OSPF queue.
 * This queue contains NIM events, router events, configurator events, etc. */
#define OSPF_PROC_MSG_COUNT (L7_IPMAP_INTF_MAX_COUNT * 2)

#define OSPF_REDIST_MSG_COUNT 10

/* How long OSPF waits from the time RTO notifies it of best route changes
 * until OSPF asks RTO for those changes. The delay allows RTO to process
 * several changes and send them all to OSPF at one time and also allows
 * flaps to damp out. */
#define L7_OSPF_BEST_ROUTE_DELAY    4000   /*ms*/ 

/* Max number of best route changes OSPF can get from RTO at once. */
#define L7_OSPF_MAX_BEST_ROUTE_CHANGES 50

/* End of CONFIGURATOR OSPF Component's Factory Defaults */


/* Start of CONFIGURATOR RIP Component's Factory Defaults */
/* Start of CONFIGURATOR RIP Component's Factory Defaults */
/* Start of CONFIGURATOR RIP Component's Factory Defaults */
/* Start of CONFIGURATOR RIP Component's Factory Defaults */
/* Start of CONFIGURATOR RIP Component's Factory Defaults */

#define FD_CNFGR_RIP_DEFAULT_STACK_SIZE       L7_L3_DEFAULT_STACK_SIZE
#define FD_CNFGR_RIP_DEFAULT_TASK_SLICE       L7_L3_DEFAULT_TASK_SLICE
#define FD_CNFGR_RIP_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define RIPMAP_PROCESS_QUEUE "ripMapProcessQueue"
#define RIPMAP_REDIST_QUEUE "ripMapRedistQueue"

/* The size of the main RIP processing message queue. This queue contains
 * all messages other than those for route redistribution. */
#define RIPMAP_PROC_MSG_COUNT (L7_IPMAP_INTF_MAX_COUNT * 2)

/* The size of the RIP redistribution message queue. */
#define RIPMAP_REDIST_MSG_COUNT 10

#define FD_CNFGR_RIP_TASK_INIT_WAIT           L7_WAIT_FOREVER

/* How long RIP waits from the time RTO notifies it of best route changes
 * until RIP asks RTO for those changes. The delay allows RTO to process
 * several changes and send them all to RIP at one time and also allows
 * flaps to damp out. */
#define L7_RIP_BEST_ROUTE_DELAY    5000   /*ms*/ 

/* Max number of best route changes OSPF can get from RTO at once. */
#define L7_RIP_MAX_BEST_ROUTE_CHANGES 50

/* End of CONFIGURATOR RIP Component's Factory Defaults */


/* Start of CONFIGURATOR Router Discovery Component's Factory Defaults */
/* Maximum number of messages serviced by the rtr_disc processing task before giving other
   tasks a chance to run */
#define FD_CNFGR_RTR_DISC_MAX_MSGS_SERVICED      512

#define RTR_DISC_PROCESS_QUEUE "rtrDiscProcessQueue"
#define RTR_DISC_PROC_MSG_COUNT (L7_IPMAP_INTF_MAX_COUNT * 2)


/* End of CONFIGURATOR Router Discovery Component's Factory Defaults */

/* Start of CONFIGURATOR VRRP Component's Factory Defaults */
/* Start of CONFIGURATOR VRRP Component's Factory Defaults */
/* Start of CONFIGURATOR VRRP Component's Factory Defaults */
/* Start of CONFIGURATOR VRRP Component's Factory Defaults */
/* Start of CONFIGURATOR VRRP Component's Factory Defaults */

#define FD_CNFGR_VRRP_DEFAULT_STACK_SIZE       L7_L3_DEFAULT_STACK_SIZE
#define FD_CNFGR_VRRP_DEFAULT_TASK_SLICE       L7_L3_DEFAULT_TASK_SLICE
#define FD_CNFGR_VRRP_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

#define L7_RTR_MAX_VRRP_PER_INTERFACE          255

/* How long VRRP waits from the time RTO notifies it of best route changes
 * until VRRP asks RTO for those changes. The delay allows RTO to process
 * several changes and send them all to VRRP at one time and also allows
 * flaps to damp out.*/
#define L7_VRRP_BEST_ROUTE_DELAY    1000   /*ms*/

/* Max number of best route changes VRRP can get from RTO at once. */
#define L7_VRRP_MAX_BEST_ROUTE_CHANGES 50

/* VRRP msgQ size. Can get a router interface event (e.g., L7_RTR_INTF_CREATE) 
 * for every interface. Leave space for other events, too. */
#define VRRP_MSG_COUNT      (L7_VRRP_MAX_INTERFACES * 2)

/*--------------------------------------*/
/*  VRRP MAP TASK INIT WAIT              */
/*--------------------------------------*/
/* Length of time to wait for task initialization */
#define FD_CNFGR_VRRP_TASK_INIT_WAIT           L7_WAIT_FOREVER

/* End of CONFIGURATOR VRRP Component's Factory Defaults */




/* Start of CONFIGURATOR RLIM Component's Factory Defaults */
/* Start of CONFIGURATOR RLIM Component's Factory Defaults */
/* Start of CONFIGURATOR RLIM Component's Factory Defaults */
/* Start of CONFIGURATOR RLIM Component's Factory Defaults */
/* Start of CONFIGURATOR RLIM Component's Factory Defaults */

#define RLIM_PROC_TASK "RLIM task"
#define RLIM_PROC_QUEUE "RLIM task queue"

/* The number of messages that can be queued in the RLIM queue.
 * This queue contains nim and router events plus we allow
 * for a single restore and best-route event plus some slop */
#define RLIM_PROC_MSG_COUNT ((L7_IPMAP_INTF_MAX_COUNT * 3) + 16)

#define RLIM_CNFGR_TASK "RLIM cnfgr task"
#define RLIM_CNFGR_QUEUE "RLIM cnfgr queue"

/* The number of messages that can be queued in the RLIM queue.
 * This queue contains nim and router events plus we allow
 * for a single restore and best-route event plus some slop */
#define RLIM_CNFGR_MSG_COUNT 16

/* End of CONFIGURATOR RLIM Component's Factory Defaults */



/* Start of CONFIGURATOR RELAY Component's Factory Defaults */
/* Start of CONFIGURATOR RELAY Component's Factory Defaults */
/* Start of CONFIGURATOR RELAY Component's Factory Defaults */
/* Start of CONFIGURATOR RELAY Component's Factory Defaults */
/* Start of CONFIGURATOR RELAY Component's Factory Defaults */

#define FD_CNFGR_RELAY_DEFAULT_STACK_SIZE       L7_L3_DEFAULT_STACK_SIZE
#define FD_CNFGR_RELAY_DEFAULT_TASK_SLICE       L7_L3_DEFAULT_TASK_SLICE
#define FD_CNFGR_RELAY_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)

/* Port types accecpted in ipMapIsValidIntfType() */
#define L7_IPMAP_INTF_MAX_COUNT     L7_MAX_PORT_COUNT          \
                                       + L7_MAX_NUM_VLAN_INTF  \
                                       + L7_MAX_NUM_LOOPBACK_INTF

#define L7_OSPF_INTF_MAX_COUNT  L7_IPMAP_INTF_MAX_COUNT

#endif
