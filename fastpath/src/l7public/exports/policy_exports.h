/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename policy_exports.h
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

#ifndef __POLICY_EXPORTS_H_
#define __POLICY_EXPORTS_H_


/* Policy Component Feature List */
typedef enum
{
  L7_POLICY_FEATURE_SUPPORTED = 0,            /* general support statement */
  L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID,
  L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID,
  L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID,
  L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID,
  L7_POLICY_BCAST_CONTROL_FEATURE_ID,
  L7_POLICY_MCAST_CONTROL_FEATURE_ID,
  L7_POLICY_UCAST_CONTROL_FEATURE_ID,
  L7_POLICY_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID,
  L7_POLICY_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_POLICY_FEATURE_IDS_t;

#define L7_BCAST_STORM_THRESHOLD_MIN  0
#define L7_BCAST_STORM_THRESHOLD_MAX  100
#define L7_UCAST_STORM_THRESHOLD_MIN  0
#define L7_UCAST_STORM_THRESHOLD_MAX  100
#define L7_MCAST_STORM_THRESHOLD_MIN  0
#define L7_MCAST_STORM_THRESHOLD_MAX  100


typedef enum
{
    STORMCONTROL_BCAST,
    STORMCONTROL_MCAST,
    STORMCONTROL_UCAST,
} L7_STORMCONTROL_TYPE;



/******************** conditional Override *****************************/

#ifdef INCLUDE_POLICY_EXPORTS_OVERRIDES
#include "policy_exports_overrides.h"
#endif

#endif /* __POLICY_EXPORTS_H_*/
