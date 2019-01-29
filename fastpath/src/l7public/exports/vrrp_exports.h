/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
*
* @filename vrrp_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by 
*           management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 12/14/2009
*
* @author rrice    
* @end
*
**********************************************************************/

#ifndef __VRRP_EXPORTS_H_
#define __VRRP_EXPORTS_H_

#include "l3_commdefs.h"

/* Minimum amount by which the VRRP priority can be reduced when a 
 * tracked route is not in the routing table. Don't allow this to be 
 * set to 0, since a decrement of 0 is a no-op. */
#define L7_VRRP_TRACK_ROUTE_DECREMENT_MIN      1
#define L7_VRRP_TRACK_ROUTE_DECREMENT_MAX      (L7_VRRP_INTF_PRIORITY_MAX - 1)

#define FD_VRRP_DEFAULT_TIMER_LEARN_MODE       L7_DISABLE
#define FD_VRRP_DEFAULT_PREEMPT_DELAY          0
#define FD_VRRP_DEFAULT_PREEMPT_DELAY_CONFIG   L7_DISABLE
#define L7_VRRP_MAX_DESCRIPTION                80
#define L7_VRRP_MIN_DESCRIPTION                0

/******************** conditional Override *****************************/

#ifdef INCLUDE_VRRP_EXPORTS_OVERRIDES
#include "vrrp_exports_overrides.h"
#endif

#endif /* __VRRP_EXPORTS_H_*/
