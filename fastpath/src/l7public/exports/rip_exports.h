/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename rip_exports.h
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
* @created 7/21/2008
*
* @author rrice    
* @end
*
**********************************************************************/

#ifndef __RIP_EXPORTS_H_
#define __RIP_EXPORTS_H_

/*  RIP MACROS  */
#define L7_RIP_SENDING_UPDATES_TIME      30
#define L7_RIP_INVALID_PARAMETER         180
#define L7_RIP_HOLD_DOWN_PARAMETER       120
#define L7_RIP_FLUSHED_TIME              300


/******************** conditional Override *****************************/

#ifdef INCLUDE_RIP_EXPORTS_OVERRIDES
#include "rip_exports_overrides.h"
#endif

#endif /* __RIP_EXPORTS_H_*/
