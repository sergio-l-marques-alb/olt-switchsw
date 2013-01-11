/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename bxs_exports.h
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

#ifndef __BXS_EXPORTS_H_
#define __BXS_EXPORTS_H_


#define L7_MAX_TEMP_SENSORS_PER_UNIT           1
#define L7_MAX_PW_MODS_PER_UNIT                2
#define L7_MAX_FANS_PER_UNIT                   5
#define L7_MAX_SFPS_PER_UNIT                   24
#define L7_MAX_XFPS_PER_UNIT                   4

#define BOXS_TEMPERATURE_MARGIN_INSIDE          5
#define BOXS_TEMPERATURE_MARGIN_OUTSIDE         5

/* Box Services Component Feature List */
typedef enum
{
  L7_BXS_FEATURE_ID = 0,            /* general support statement */
  L7_BXS_FAN_STATUS_FEATURE_ID,
  L7_BXS_PWR_SUPPLY_STATUS_FEATURE_ID,
  L7_BXS_TEMPERATURE_FEATURE_ID,
  L7_BXS_TEMPERATURE_STATUS_FEATURE_ID,
  L7_BXS_SFP_STATUS_FEATURE_ID,
  L7_BXS_XFP_STATUS_FEATURE_ID,
  L7_BXS_FEATURE_ID_TOTAL           /* total number of enum values */
} L7_BXS_FEATURE_IDS_t;


/******************** conditional Override *****************************/

#ifdef INCLUDE_BXS_EXPORTS_OVERRIDES
#include "bxs_exports_overrides.h"
#endif

#endif /* __BXS_EXPORTS_H_*/
