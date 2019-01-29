/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename lldp_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 
*
* @author 
* @end
*
**********************************************************************/

#ifndef __LLDP_EXPORTS_H_
#define __LLDP_EXPORTS_H_
#include "lldp_api.h"

#define L7_LLDP_MIN_TIME_INTERVAL           1
#define L7_LLDP_MAX_TIME_INTERVAL           32768
#define L7_LLDP_MIN_HOLD_MULTIPLIER         2
#define L7_LLDP_MAX_HOLD_MULTIPLIER         10
#define L7_LLDP_MIN_REINIT_DELAY            1
#define L7_LLDP_MAX_REINIT_DELAY            10
#define L7_LLDP_MIN_NOTIFICATION_INTERVAL   5
#define L7_LLDP_MAX_NOTIFICATION_INTERVAL   3600
#define L7_LLDP_TX_INTERVAL_MIN             5
#define L7_LLDP_TX_INTERVAL_MAX             32768
#define L7_LLDP_TX_DELAY_MIN             1
#define L7_LLDP_TX_DELAY_MAX             8192


/* LLDP-MED Defines */
#define LLDP_MED_FASTSTART_REPEAT_COUNT_MIN 1
#define LLDP_MED_FASTSTART_REPEAT_COUNT_MAX 10
#define LLDP_PORTID_SUBTYPE_SUPPORTED                    LLDP_PORT_ID_SUBTYPE_MAC_ADDR
/******************** conditional Override *****************************/

#ifdef INCLUDE_LLDP_EXPORTS_OVERRIDES
#include "lldp_exports_overrides.h"
#endif

#endif /* __LLDP_EXPORTS_H_*/
