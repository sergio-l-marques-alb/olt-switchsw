/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename nim_exports.h
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

#ifndef __NIM_EXPORTS_H_
#define __NIM_EXPORTS_H_


/* NIM Component Feature List */
typedef enum
{
  L7_NIM_FEATURE_ID = 0,            /* general support statement */
  L7_NIM_JUMBOFRAMES_PER_INTERFACE_FEATURE_ID,
  L7_NIM_JUMBOFRAMES_FEATURE_ID,    /* whether jumbo frames (maximum transmission unit) is configurable */
  L7_NIM_AUTONEG_WITH_SPEED_DUPLEX_FEATURE_ID,/* speed & duplex commands to enable auto-neg for IS-CLI compliance*/
  L7_NIM_FEATURE_ID_TOTAL           /* total number of enum values */
} L7_NIM_FEATURE_IDS_t;

#define L7_NIM_IFDESCRINFO_SET   nimIfDescrInfoSet
#define NIM_AUTONEG_FIXED_DUPLEX_MASK      0x700
#define NIM_AUTONEG_FIXED_SPEED_MASK       0xF800


/******************** conditional Override *****************************/

#ifdef INCLUDE_NIM_EXPORTS_OVERRIDES
#include "nim_exports_overrides.h"
#endif

#endif /* __NIM_EXPORTS_H_*/
