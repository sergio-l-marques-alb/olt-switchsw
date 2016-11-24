/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename wireless_exports.h
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

#ifndef __WIRELESS_EXPORTS_H_
#define __WIRELESS_EXPORTS_H_

#include "wireless_defaultconfig.h"

typedef enum
{
  L7_WIRELESS_FEATURE_ID = 0,                   /* general support statement */
  L7_WIRELESS_TUNNELING_FEATURE_ID,
  L7_WIRELESS_NETWORK_VISUALIZATION_FEATURE_ID,
  L7_WIRELESS_L2_CENTTNNL_FEATURE_ID,
  L7_WIRELESS_DIST_TUNNELING_FEATURE_ID,
  L7_WIRELESS_AP_IMAGE_MODE_INDEPENDENT_FEATURE_ID,
  L7_WIRELESS_AP_IMAGE_MODE_INTEGRATED_FEATURE_ID,
  L7_WIRELESS_TSPEC_FEATURE_ID,
  L7_WIRELESS_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_WIRELESS_FEATURE_IDS_t;


/* TSPEC Access Category List */
typedef enum
{
  L7_WIRELESS_TSPEC_AC_BE = 0,                  /* Best-Effort */
  L7_WIRELESS_TSPEC_AC_BK = 1,                  /* Background */
  L7_WIRELESS_TSPEC_AC_VI = 2,                  /* Video */
  L7_WIRELESS_TSPEC_AC_VO = 3,                  /* Voice */
  L7_WIRELESS_TSPEC_AC_TOTAL                    /* total number of ACs defined */
} L7_WIRELESS_TSPEC_AC_t;

#define L7_WIRELESS_TSPEC_AC_NAME_STRINGS \
  { "Best-Effort", "Background", "Video", "Voice" }

#define L7_WIRELESS_TSPEC_AC_QUEUE_NAME_STRINGS \
  { "AC_BE", "AC_BK", "AC_VI", "AC_VO" }

/* TSPEC Direction */
typedef enum
{
  L7_WIRELESS_TSPEC_DIR_UP   = 0,               /* Uplink */
  L7_WIRELESS_TSPEC_DIR_DOWN = 1,               /* Downlink */
  L7_WIRELESS_TSPEC_DIR_RSVD = 2,               /* (reserved) */
  L7_WIRELESS_TSPEC_DIR_BIDI = 3,               /* Bidirectional */
  L7_WIRELESS_TSPEC_DIR_TOTAL                   /* total number of directions defined */
} L7_WIRELESS_TSPEC_DIR_t;

#define L7_WIRELESS_TSPEC_DIR_NAME_STRINGS \
  { "Uplink", "Downlink", "(Reserved)", "Bidirectional" }

/* TSPEC Min/Max traffic stream identifier (TID) values used for EDCA */
#define L7_WIRELESS_TSPEC_TID_MIN                               0
#define L7_WIRELESS_TSPEC_TID_MAX                               7

/* TSPEC medium time scaling factor 
 * (use for converting to/from units of 32 usec-per-second)
 */
#define L7_WIRELESS_TSPEC_MEDIUM_TIME_SCALE                     32
#define L7_WIRELESS_TSPEC_MEDIUM_TIME_MAX                       (1000000 / L7_WIRELESS_TSPEC_MEDIUM_TIME_SCALE)

/* TSPEC client violation report interval (seconds) */
#define L7_WIRELESS_TSPEC_VIOLATE_REPORT_IVAL_MIN               0
#define L7_WIRELESS_TSPEC_VIOLATE_REPORT_IVAL_MAX               900

/* TSPEC ACM bandwidth limit (percent)*/
#define L7_WIRELESS_TSPEC_ACM_LIMIT_MIN                         0
#define L7_WIRELESS_TSPEC_ACM_LIMIT_MAX                         FD_WDM_DEFAULT_WS_TSPEC_ACM_BW_MAX

#define L7_WIRELESS_TSPEC_ACM_LIMITS_COMBINED_MAX               FD_WDM_DEFAULT_WS_TSPEC_ACM_BW_MAX

/* TSPEC AP TS inactivity timeout (seconds) */
#define L7_WIRELESS_TSPEC_AP_TS_INACT_TIMEOUT_MIN               0
#define L7_WIRELESS_TSPEC_AP_TS_INACT_TIMEOUT_MAX               120

/* TSPEC STA TS inactivity timeout (seconds) */
#define L7_WIRELESS_TSPEC_STA_TS_INACT_TIMEOUT_MIN              0
#define L7_WIRELESS_TSPEC_STA_TS_INACT_TIMEOUT_MAX              120

/******************** conditional Override *****************************/

#ifdef INCLUDE_WIRELESS_EXPORTS_OVERRIDES
#include "wireless_exports_overrides.h"
#endif

#endif /* __WIRELESS_EXPORTS_H_*/
