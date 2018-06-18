/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename ping_exports.h
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

#ifndef __PING_EXPORTS_H_
#define __PING_EXPORTS_H_



/*--------------------------------------*/
/*  PING Constants                      */
/*--------------------------------------*/
/* To display RTT in msec */
#define PING_RTT_MULTIPLIER 1000
/* Default values */
#define PING_DEFAULT_PROBE_INTERVAL 3
#define PING_DEFAULT_PROBE_SIZE     0
#define PING_DEFAULT_PROBE_COUNT    1
#define PING_DEFAULT_SRC_IP_ADDR    0
#define PING_DEFAULT_DSCP           0
/* Min and Max values */
#define PING_MAX_NAME_LEN       32     /* Max length of name */
#define PING_MAX_INDEX_LEN      32     /* Max length of name */
#define PING_MAX_SIZE           65507    /* Max size of ICMP payload */
#define PING_MIN_SIZE           0      /* Min size of ICMP payload */
#define PING_MAX_PROBES         15  /* Max probes supported per session */
#define PING_MIN_PROBES         1      /* Min probes supported per session */
#define PING_MAX_INTERVAL       60   /* Max probe interval in seconds */
#define PING_MIN_INTERVAL       1      /* Min probe interval in seconds */
#define L7_PING_PACKET_SIZE     "size"
#define L7_PING_PACKET_COUNT    "count"
#define L7_PING_INTERVAL        "interval"
#define L7_PING_SOURCE          "source"
#define PING_PROBE_RESP_TYPE_UNREACH   3

typedef enum {
    L7_PING_SESSION_STATE_FREE,            /* Free entry */
    L7_PING_SESSION_STATE_ALLOCATED,       /* Entry allocated and being built.
                                           Ping not yet started */
    L7_PING_SESSION_STATE_IN_PROGRESS,     /* Ping in progress */
    L7_PING_SESSION_STATE_DONE             /* Ping done - resp already read */
} L7_pingSessionState_t;

/*--------------------------------------*/
/*  Old PING Constants not used anymore */
/*--------------------------------------*/
#define L7_PING_COUNT_MAX  20
#define L7_PING_COUNT_DEF  3

#define L7_WEBFORM_PING_IPADDR_DEF "0.0.0.0"
#define L7_WEBFORM_PING_COUNT_DEF "3"



/******************** conditional Override *****************************/

#ifdef INCLUDE_PING_EXPORTS_OVERRIDES
#include "ping_exports_overrides.h"
#endif

#endif /* __PING_EXPORTS_H_*/
