/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename traceroute_exports.h
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

#ifndef __TRACEROUTE_EXPORTS_H_
#define __TRACEROUTE_EXPORTS_H_


/*--------------------------*/
/* Trace Route constants*/
/*--------------------------*/
/* Default values */
#define TRACEROUTE_DEFAULT_PROBE_PER_HOP  3
#define TRACEROUTE_DEFAULT_PROBE_INTERVAL 3
#define TRACEROUTE_DEFAULT_PROBE_SIZE     0
#define TRACEROUTE_DEFAULT_PORT           33434
#define TRACEROUTE_DEFAULT_INIT_TTL       1
#define TRACEROUTE_DEFAULT_MAX_TTL        30
/*#define TRACEROUTE_DEFAULT_MAX_FAIL       \
  (TRACEROUTE_DEFAULT_PROBE_PER_HOP * TRACEROUTE_DEFAULT_MAX_TTL)*/
#define TRACEROUTE_DEFAULT_MAX_FAIL       5


/* Min and Max values */
#define TRACEROUTE_MAX_INDEX_LEN      32   /* Max size for index field */
#define TRACEROUTE_MAX_NAME_LEN       32   /* Max size for name field */
#define TRACEROUTE_MAX_PROBE_PER_HOP  10   /* Max probes per  hop */
#define TRACEROUTE_MIN_PROBE_PER_HOP  1    /* Min probes per  hop */
#define TRACEROUTE_MAX_TTL            255  /* Max value of maxTtl */
#define TRACEROUTE_MIN_TTL            1     /* Min value of minTtl */
#define TRACEROUTE_MAX_INIT_TTL       255  /* Max value of Init TTL */
#define TRACEROUTE_MIN_INIT_TTL       0    /* Min value of Init TTL */
#define TRACEROUTE_MAX_FAIL           255 /* Max consecutive fails allowed */
#define TRACEROUTE_MIN_FAIL           0    /* Min consecutive fails allowed */
#define TRACEROUTE_MAX_INTERVAL       60 /* Max probe interval in seconds */
#define TRACEROUTE_MIN_INTERVAL       1    /* Min probe interval in seconds */
#define TRACEROUTE_MIN_PORT           1    /* Min value for UDP port */
#define TRACEROUTE_MAX_PORT           65535 /* Max value for UDP port */
#define TRACEROUTE_MIN_PROBE_SIZE     0    /* Min size of PDU transmitted */
#define TRACEROUTE_MAX_PROBE_SIZE     65507  /* Max size of PDU transmitted */

/* To convert usec to msec */
#define TRACEROUTE_RTT_MULTIPLIER     1000

/*-------------------------------------------*/
/* Old Trace Route constants not used anymore*/
/*-------------------------------------------*/
#define L7_TRACEROUTE_MAXHOSTNAMELEN                64
#define L7_TRACEROUTE_MAXHOPS                       20
#define L7_TRACEROUTE_NUM_PROBE_PACKETS              3
#define L7_TRACEROUTE_MIN_PORT                       0
#define L7_TRACEROUTE_MAX_PORT                   65535




/******************** conditional Override *****************************/

#ifdef INCLUDE_TRACEROUTE_EXPORTS_OVERRIDES
#include "traceroute_exports_overrides.h"
#endif

#endif /* __TRACEROUTE_EXPORTS_H_*/
