/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename dhcp_snooping_exports.h
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

#ifndef __DHCP_SNOOPING_EXPORTS_H_
#define __DHCP_SNOOPING_EXPORTS_H_


/*-----------------------------------*/
/* DHCP SNOOPING Constants           */
/*-----------------------------------*/
#define L7_DS_DB_STORE_INTERVAL 300
#define L7_DS_DB_MIN_STORE_INTERVAL 15 
#define L7_DS_DB_MAX_STORE_INTERVAL 86400 

/*--------------------------*/
/* DHCP SNOOPING CONSTANTS  */
/*--------------------------*/
#define L7_DS_RATE_LIMIT_NONE -1
#define L7_DS_RATE_LIMIT_MIN                0
#define L7_DS_RATE_LIMIT_MAX              300
#define L7_DS_BURST_INTERVAL_MIN            1
#define L7_DS_BURST_INTERVAL_MAX           15




/******************** conditional Override *****************************/

#ifdef INCLUDE_DHCP_SNOOPING_EXPORTS_OVERRIDES
#include "dhcp_snooping_exports_overrides.h"
#endif

#endif /* __DHCP_SNOOPING_EXPORTS_H_*/
