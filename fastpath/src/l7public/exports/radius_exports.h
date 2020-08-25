/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename radius_exports.h
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
* @author akulkarn
* @end
*
**********************************************************************/

#ifndef __RADIUS_EXPORTS_H_
#define __RADIUS_EXPORTS_H_

#define ALL_RADIUS_SERVERS        (L7_uint32)0x00000000

/* RADIUS Component Feature List */
typedef enum
{
  L7_RADIUS_FEATURE_ID = 0,                   /* general support statement */
  L7_RADIUS_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_RADIUS_FEATURE_IDS_t;


/*----------------------------------*/
/*  RADIUS Public Constants         */
/*----------------------------------*/
/* Server Entry Types */
typedef enum
{
  L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY = 1,
  L7_RADIUS_SERVER_ENTRY_TYPE_SECONDARY

} L7_RADIUS_SERVER_ENTRY_TYPE_t;

#define L7_RADIUS_MAX_SECRET 16

/* Retransmission Interval */
#define L7_RADIUS_MIN_RETRANS             1
#define L7_RADIUS_MAX_RETRANS             15
#define L7_RADIUS_DEFAULT_RETRANS         4

/* Timers in seconds */
#define L7_RADIUS_MIN_TIMEOUTVAL             1
#define L7_RADIUS_MAX_TIMEOUTVAL            30
#define L7_RADIUS_DEFAULT_TIMEOUTDURATION    5
#define L7_RADIUS_DEFAULT_RETRYPRIMARYTIME 300

#define L7_RADIUS_MIN_PORT                1
#define L7_RADIUS_MAX_PORT                65535

#define L7_RADIUS_NAS_IDENTIFIER_SIZE 17

/* Usage type for the radius server */
typedef enum
{
  L7_RADIUS_SERVER_USAGE_UNKNOWN = 0,
  L7_RADIUS_SERVER_USAGE_ALL,
  L7_RADIUS_SERVER_USAGE_FIRST = L7_RADIUS_SERVER_USAGE_ALL,
  L7_RADIUS_SERVER_USAGE_LOGIN,
  L7_RADIUS_SERVER_USAGE_DOT1X,
  L7_RADIUS_SERVER_USAGE_MAX,
  L7_RADIUS_SERVER_USAGE_ANY = L7_RADIUS_SERVER_USAGE_MAX

} L7_RADIUS_SERVER_USAGE_TYPE_t;

typedef enum
{
  L7_RADIUSGLOBAL = 0, /* Setting applies to all RADIUS servers. */
  L7_RADIUSLOCAL       /* Setting applies to a specific RADIUS server. */
} L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t;


/* Lowest number is highest priority */
#define L7_RADIUS_MAX_PRIORITY               65535
#define L7_RADIUS_MIN_PRIORITY               0
#define L7_RADIUS_DEFAULT_PRIORITY           0

/* Timers in seconds */
#define L7_RADIUS_MIN_DEADTIME               0
#define L7_RADIUS_MAX_DEADTIME               2000*60
#define L7_RADIUS_MAX_DEADTIME_MINUTES       2000
#define L7_RADIUS_DEFAULT_DEADTIME           0


/* default source-ip to be used in default source-ip parameter */
#define L7_DEFAULT_SOURCEIP    "0.0.0.0"
#define L7_RADIUS_FD_DEFAULT_USAGEYPE         "all"
#define L7_RADIUS_FD_LOGIN_USAGEYPE           "login"
#define L7_RADIUS_FD_DOT1X_USAGEYPE           "802.1x"

typedef enum
{
  RADIUS_SERVER_TYPE_UNKNOWN = 0,
  RADIUS_SERVER_TYPE_AUTH,
  RADIUS_SERVER_TYPE_ACCT

} radiusServerType_t;

#define L7_RADIUS_SERVER_NAME_MIN           1
#define L7_RADIUS_SERVER_NAME_MAX           32
#define L7_RADIUS_MAX_AUTH_SERVERS    FD_RADIUS_MAX_AUTH_SERVERS
#define L7_RADIUS_MAX_ACCT_SERVERS    FD_RADIUS_MAX_ACCT_SERVERS

/******************** conditional Override *****************************/

#ifdef INCLUDE_RADIUS_EXPORTS_OVERRIDES
#include "radius_exports_overrides.h"
#endif

#endif /* __RADIUS_EXPORTS_H_*/
