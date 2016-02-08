/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename sntp_exports.h
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

#ifndef __SNTP_EXPORTS_H_
#define __SNTP_EXPORTS_H_

/* SNTP Component Feature List */
typedef enum
{
  L7_SNTP_FEATURE_ID = 0,            /* general support statement */
  L7_SNTP_UNICAST_FEATURE_ID,
  L7_SNTP_BROADCAST_FEATURE_ID,
  L7_SNTP_MULTICAST_FEATURE_ID,
  L7_SNTP_PROTO_DNS_FEATURE_ID,
  L7_SNTP_PROTO_IPV4_FEATURE_ID,
  L7_SNTP_PROTO_IPV6_FEATURE_ID,
  L7_SNTP_FEATURE_ID_TOTAL           /* total number of enum values */
} L7_SNTP_FEATURE_IDS_t;

/******************************************************************/
/*************       Start SNTP types and defines *********/
/******************************************************************/

#define L7_TIME_SOURCE_NONE 0
#define L7_TIME_SOURCE_SNTP 1

typedef enum
{
  L7_SNTP_STATUS_OTHER = 1,
  L7_SNTP_STATUS_SUCCESS = 2,
  L7_SNTP_STATUS_REQUEST_TIMED_OUT = 3,
  L7_SNTP_STATUS_BAD_DATE_ENCODED = 4,
  L7_SNTP_STATUS_VERSION_NOT_SUPPORTED = 5,
  L7_SNTP_STATUS_SERVER_UNSYNCHRONIZED = 6,
  L7_SNTP_STATUS_KISS_OF_DEATH = 7
} L7_SNTP_PACKET_STATUS_t;

typedef enum
{
  L7_SNTP_ADDRESS_UNKNOWN = 0,
  L7_SNTP_ADDRESS_IPV4 = 1,
  L7_SNTP_ADDRESS_DNS = 16
} L7_SNTP_ADDRESS_TYPE_t;

typedef enum
{
  L7_SNTP_SERVER_MODE_RESERVED = 0,
  L7_SNTP_SERVER_MODE_SYMMETRIC_ACTIVE = 1,
  L7_SNTP_SERVER_MODE_SYMMETRIC_PASSIVE = 2,
  L7_SNTP_SERVER_MODE_CLIENT = 3,
  L7_SNTP_SERVER_MODE_SERVER = 4,
  L7_SNTP_SERVER_MODE_BROADCAST = 5,
  L7_SNTP_SERVER_MODE_RESERVED_CONTROL = 6,
  L7_SNTP_SERVER_MODE_RESERVED_PRIVATE = 7
} L7_SNTP_SERVER_MODE_t;

typedef enum
{
  L7_SNTP_CLIENT_DISABLED = 0,
  L7_SNTP_CLIENT_UNICAST = 1,
  L7_SNTP_CLIENT_BROADCAST = 2,
  L7_SNTP_CLIENT_MULTICAST = 4
} L7_SNTP_CLIENT_MODE_t;

typedef enum
{
 L7_SNTP_SUPPORTED_DISABLED = 1,
 L7_SNTP_SUPPORTED_UNICAST = 2,
 L7_SNTP_SUPPORTED_BROADCAST = 4,
 L7_SNTP_SUPPORTED_MULTICAST = 8,
 L7_SNTP_SUPPORTED_UNICAST_AND_BROADCAST = 16
}L7_SNTP_SUPPORTED_MODE_t;

/* Server table row status - Only ACTIVE, and NOT_IN_SERVICE are ever returned.
   Only ACTIVE, NOT_IN_SERVICE, and DESTROY may be set. */

typedef enum
{
  L7_SNTP_SERVER_STATUS_INVALID = 0,
  L7_SNTP_SERVER_STATUS_ACTIVE = 1,
  L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE = 2,
  L7_SNTP_SERVER_STATUS_NOT_READY = 3,
  L7_SNTP_SERVER_STATUS_CREATE_AND_GO = 4,
  L7_SNTP_SERVER_STATUS_CREATE_AND_WAIT = 5,
  L7_SNTP_SERVER_STATUS_DESTROY = 6
} L7_SNTP_SERVER_STATUS_t;




/******************** conditional Override *****************************/

#ifdef INCLUDE_SNTP_EXPORTS_OVERRIDES
#include "sntp_exports_overrides.h"
#endif

#endif /* __SNTP_EXPORTS_H_*/
