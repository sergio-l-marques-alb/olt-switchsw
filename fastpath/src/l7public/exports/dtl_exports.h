/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename dtl_exports.h
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

#ifndef __DTL_EXPORTS_H_
#define __DTL_EXPORTS_H_


/* DTL Component Feature List */
typedef enum
{
  L7_DTL_CUSTOM_MMU_FEATURE_ID = 0,           /* Custom MMU settings         */
  L7_DTL_STRICT_POLICY_LIMIT_FEATURE_ID,
  L7_DTL_CUSTOM_LED_BLINK_FEATURE_ID,         /* LED Processor ENABLING/DISABLING */
  L7_DTL_FEATURE_ID_TOTAL                     /* total number of enum values */
} L7_DTL_FEATURE_IDS_t;



/******************** conditional Override *****************************/

#ifdef INCLUDE_DTL_EXPORTS_OVERRIDES
#include "dtl_exports_overrides.h"
#endif

#endif /* __DTL_EXPORTS_H_*/
