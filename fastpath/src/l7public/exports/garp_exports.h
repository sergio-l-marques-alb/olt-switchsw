/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename garp_exports.h
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
* @created 06/20/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef __GARP_EXPORTS_H_
#define __GARP_EXPORTS_H_


/* GARP Component Feature List */



/*--------------------------------------*/
/*   GARP Constants                     */
/*--------------------------------------*/
/* join timer */
#define L7_GARP_MIN_JOIN_TIME      10      /* centiseconds */
#define L7_GARP_MAX_JOIN_TIME      100     /* centiseconds */
#define L7_GARP_DEF_JOIN_TIME      20      /* centiseconds */

/* leave timer */
#define L7_GARP_MIN_LEAVE_TIME     20      /* centiseconds */
#define L7_GARP_MAX_LEAVE_TIME     600     /* centiseconds */
#define L7_GARP_DEF_LEAVE_TIME     60      /* centiseconds */

/* leave all timer */
#define L7_GARP_MIN_LEAVE_ALL_TIME 200     /* centiseconds */
#define L7_GARP_MAX_LEAVE_ALL_TIME  6000    /* centiseconds */
#define L7_GARP_DEF_LEAVE_ALL_TIME 1000    /* centiseconds */

typedef enum
{
  GARP_JOIN_EMPTY_RECEIVED = 1,
  GARP_JOIN_IN_RECEIVED,
  GARP_EMPTY_RECEIVED,
  GARP_LEAVE_IN_RECEIVED,
  GARP_LEAVE_EMPTY_RECEIVED,
  GARP_LEAVEALL_RECEIVED,
  GARP_JOIN_EMPTY_SENT,
  GARP_JOIN_IN_SENT,
  GARP_EMPTY_SENT,
  GARP_LEAVE_IN_SENT,
  GARP_LEAVE_EMPTY_SENT,
  GARP_LEAVEALL_SENT
} GARP_STAT_COUNTER_TYPE_t;

typedef enum
{
  L7_GARP_JOIN_EMPTY = 1,
  L7_GARP_EMPTY,
  L7_GARP_LEAVE_EMPTY,
  L7_GARP_JOIN_IN,
  L7_GARP_LEAVE_IN,
  L7_GARP_LEAVEALL
} L7_GARP_ATTRIBUTE_TYPE_t;

typedef enum
{
  GARP_INVALID_PROTOCOL_ID = 1,
  GARP_INVALID_ATTR_TYPE,
  GARP_INVALID_ATTR_VALUE,
  GARP_INVALID_ATTR_LENGTH,
  GARP_INVALID_ATTR_EVENT
} GARP_ERROR_STAT_COUNTER_TYPE_t;

/******************** conditional Override *****************************/

#ifdef INCLUDE_GARP_EXPORTS_OVERRIDES
#include "garp_exports_overrides.h"
#endif


#endif /* __GARP_EXPORTS_H_*/

