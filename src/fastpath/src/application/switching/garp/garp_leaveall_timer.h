/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garp_leaveall_timer.h
* @purpose     Header file for GARP Leave All Timer task
* @component   GARP
* @comments    none
* @create      11/02/2005
* @author      vrynkov
* @author      
* @end
*             
**********************************************************************/

#ifndef _INCLUDE_GARP_LEAVEALL_TIMER_H_
#define _INCLUDE_GARP_LEAVEALL_TIMER_H_


#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "garpapi.h"
#include "dot1dgarp.h"
#include "garp_dot1q_api.h"
#include "avl_api.h"
#include "garp_debug.h"



/* name of the Garp Leave All Timer Task */
#define GARP_LEAVEALL_TIMER_TASK "Garp_LeaveAll_Timer_Task"


/* GARP_TIMER_GRANULARITY is time slice for the Garp Leave All Timer task.
 * GARP_TIMER_GRANULARITY is in centiseconds (cs), 1 cs = 0.01 sec */
#define GARP_TIMER_GRANULARITY      50      /* 0.50 sec */
 
/* GARP_TIMER_GRANULARITY in milliseconds.  All calculations 
 * use GARP_TIMER_GRANULARITY_MS. */
#define GARP_TIMER_GRANULARITY_MS   (10 * GARP_TIMER_GRANULARITY)

/* time to sleep in milliseconds */
#define GARP_TIMER_SLEEP_MS         1000 


/* Number of ports/interfaces on the device */
/*#define GARP_INTF_MAX_COUNT  (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + 1)*/
#define GARP_INTF_MAX_COUNT  L7_MAX_INTERFACE_COUNT


/* used to generate a random value for GARP Leave All timer in the range
 * leaveall_timeout .. (GARP_LEAVE_ALL_TIMER_RANGE * leaveall_timeout) */
#define GARP_LEAVE_ALL_TIMER_RANGE   1.5


/* structure describing the GARP timers running on a port */
typedef struct
{
   L7_uint32 port_no;
   L7_uint32 leaveall_timeout;   /* user-set timeout value */

   L7_BOOL   gvrp_enabled;       /* flags showing what application is used: */
   L7_BOOL   gmrp_enabled;       /* GVRP, GMRP, both GMRP and GVRP, or none */

   L7_uint32 gvrp_leaveall_left; /* time left till GVRP Leave All timer expires */

   L7_ushort16 gmrp_instances;     

} garpTimerStruct_t;


/* commands used to control the GARP timers */
typedef enum
{
   ADD_LEAVEALL_TIMER = 1,  /* add timer when interfaces/port is created */

   UPDATE_LEAVEALL_TIMER,   /* change the value of Leave All timer for 
                             * both GVRP and GMRP */

   DELETE_TIMER,            /* delete timer when interface/port is disabled */

   RESTART_LEAVEALL_TIMER,  /* we have to restart LeaveAll timer running on the 
                             * current port.  This command is issued if the 
                             * current port received GVRP or GMRP Leave All PDU */
} garpTimerCommand_t;

/* Externs */
extern void *garpTaskSyncSema;

/* interface function between the Garp Timer task and the 
 * "outer world".  This function is used to access/change/delete 
 * the fields of garpTimerCommand_t */
extern void garpUpdateLeaveAllTimer(garpTimerCommand_t, 
                                    L7_uint32       port_no,
                                    GARPApplication app, 
                                    L7_ushort16     vlan_id, 
                                    L7_uint32       time, 
                                    L7_uint32       leaveall_timeout);
extern L7_RC_t gmrpInstanceIndexFind(L7_uint32, L7_uint32 *);
extern L7_RC_t dot1qNextVlanGet(L7_uint32, L7_uint32 *);
extern L7_BOOL garpIsValidIntf(L7_uint32);


/* Prototypes */
L7_RC_t garpLeaveAllTimerStructInit();
void garpLeaveAllTimerProcess();
void garpLeaveAllTimerCallback(void);

#endif  /* #ifndef _INCLUDE_GARP_LEAVEALL_TIMER_H_ */
