/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename voiceVlan_exports.h
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
* @created 03/26/2008
*
* @author skoundin    
* @end
*
**********************************************************************/

#ifndef __VOICEVLAN_EXPORTS_H_
#define __VOICEVLAN_EXPORTS_H_

#define L7_VOICE_VLAN_MAX_VOIP_DEVICE_COUNT 64 /*Maximum number of devices supported on the system*/

#define L7_VOICE_VLAN_DSCP_MIN              0
#define L7_VOICE_VLAN_DSCP_MAX          64 /* Maximum range for DSCP value*/

#define VOICE_VLAN_MIN_DOT1P_PRIORITY  L7_DOT1P_MIN_PRIORITY
#define VOICE_VLAN_MAX_DOT1P_PRIORITY  L7_DOT1P_MAX_PRIORITY
#define VOICE_VLAN_MAX_OVERIDE_PORTS   8


/******************** conditional Override *****************************/

#ifdef INCLUDE_VOICEVLAN_EXPORTS_OVERRIDES
#include "voicevlan_exports_overrides.h"
#endif

#endif /* __DOT1X_EXPORTS_H_*/
