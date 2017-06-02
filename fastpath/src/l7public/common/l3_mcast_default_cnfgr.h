/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename l3_pimdm_default_cnfgr.h
*
* @purpose All Factory default settings are in this file
*
* @component cnfgr
*
* @comments none
*
* @create 03/14/2002
*
* @author   vidhumouli H / M Pavan K Chakravarthi
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_L3_MCAST_DEFAULT_CNFGR
#define INCLUDE_L3_MCAST_DEFAULT_CNFGR
#include "cnfgr.h"
#include "l3_default_cnfgr.h"


/* Start of CONFIGURATOR PIMDM Component's Factory Defaults */
/* Start of CONFIGURATOR PIMDM Component's Factory Defaults */
/* Start of CONFIGURATOR PIMDM Component's Factory Defaults */
/* Start of CONFIGURATOR PIMDM Component's Factory Defaults */
/* Start of CONFIGURATOR PIMDM Component's Factory Defaults */

#define FD_CNFGR_PIMDM_DEFAULT_STACK_SIZE       L7_L3_DEFAULT_STACK_SIZE
#define FD_CNFGR_PIMDM_DEFAULT_TASK_SLICE       L7_L3_DEFAULT_TASK_SLICE
#define FD_CNFGR_PIMDM_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)



/* Start of CONFIGURATOR IGMP Component's Factory Defaults */
/* Start of CONFIGURATOR IGMP Component's Factory Defaults */
/* Start of CONFIGURATOR IGMP Component's Factory Defaults */
/* Start of CONFIGURATOR IGMP Component's Factory Defaults */
/* Start of CONFIGURATOR IGMP Component's Factory Defaults */

#define FD_CNFGR_IGMP_DEFAULT_STACK_SIZE       L7_L3_DEFAULT_STACK_SIZE
#define FD_CNFGR_IGMP_DEFAULT_TASK_SLICE       L7_L3_DEFAULT_TASK_SLICE
/*#define FD_CNFGR_IGMP_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)*/
#define FD_CNFGR_IGMP_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_MEDIUM_TASK_PRIORITY)

#define L7_MCAST_MAX_ASYNC_EVENTS              (L7_RTR_MAX_RTR_INTERFACES)
#define L7_MCAST_MAX_ASYNC_EVENT_TIMEOUT       60000      /* milliseconds */

/* Start of CONFIGURATOR PIMSM Component's Factory Defaults */
/* Start of CONFIGURATOR PIMSM Component's Factory Defaults */
/* Start of CONFIGURATOR PIMSM Component's Factory Defaults */
/* Start of CONFIGURATOR PIMSM Component's Factory Defaults */
/* Start of CONFIGURATOR PIMSM Component's Factory Defaults */

#define FD_CNFGR_PIMSM_DEFAULT_STACK_SIZE       L7_L3_DEFAULT_STACK_SIZE
#define FD_CNFGR_PIMSM_DEFAULT_TASK_SLICE       L7_L3_DEFAULT_TASK_SLICE
#define FD_CNFGR_PIMSM_DEFAULT_TASK_PRI         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)


/*--------------------------------------*/
/*  PIMDM MAP TASK INIT WAIT              */
/*--------------------------------------*/
/* Length of time to wait for task initialization */
#define FD_CNFGR_PIMDM_TASK_INIT_WAIT           L7_WAIT_FOREVER


#endif   /*INCLUDE_L3_MCAST_DEFAULT_CNFGR*/

