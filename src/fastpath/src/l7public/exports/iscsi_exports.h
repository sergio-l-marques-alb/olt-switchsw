/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename iscsi_exports.h
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
* @created 04/26/2008
*
* @end
*
**********************************************************************/

#ifndef __ISCSI_EXPORTS_H_
#define __ISCSI_EXPORTS_H_

/* iSCSI Feature IDs */
typedef enum
{
  /* iSCSI Flow Acceleration */
  L7_ISCSI_FEATURE_SUPPORTED = 0,            /* general support statement */

  L7_ISCSI_DOT1P_MARKING_FEATURE_ID,         /* support of simultaneous marking of dot1p priority and CoS queue assignment */

  L7_ISCSI_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_ISCSI_FEATURE_IDS_t;

/*--------------------------------------*/
/*  iSCSI Constants                     */
/*--------------------------------------*/
#define ISCSI_NAME_LENGTH_MAX            223  /* per RFC 3720 section 3.2.6.1 */
#define ISCSI_ISID_FIELD_LENGTH          6  /* per RFC 3720 section 3.2.6.1 */


#define ISCSI_SESSION_TIME_OUT_MINUTES_MIN 1     /* 1 minute */
#define ISCSI_SESSION_TIME_OUT_MINUTES_MAX 43200 /* 43,200 minutes */
#define ISCSI_SESSION_TIME_OUT_SECONDS_MIN ISCSI_SESSION_TIME_OUT_MINUTES_MIN*60  /* value in seconds */
#define ISCSI_SESSION_TIME_OUT_SECONDS_MAX ISCSI_SESSION_TIME_OUT_MINUTES_MAX*60  /* value in seconds */

/******************** conditional Override *****************************/

#ifdef INCLUDE_ISCSI_EXPORTS_OVERRIDES
  #include "iscsi_exports_overrides.h"
#endif

#endif /* __ISCSI_EXPORTS_H_*/
