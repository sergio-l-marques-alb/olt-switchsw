/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename sim_exports.h
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

#ifndef __SIM_EXPORTS_H_
#define __SIM_EXPORTS_H_

#include "timezone_exports.h"

/* SIM Component Feature List */
typedef enum
{
  L7_SIM_FEATURE_ID = 0,            /* general support statement */
  L7_SIM_FEATURE_KEYING_FEATURE_ID,
  L7_SIM_DRIVER_SHELL_FEATURE_ID,
  L7_SIM_DNI8541_BLADESERVER,
  L7_SIM_ALLOW_LOCAL_MAC_USAGE_FEATURE_ID,
  L7_SIM_FEATURE_ID_TOTAL           /* total number of enum values */
} L7_SIM_FEATURE_IDS_t;

/*********************************************************************
*
* @structures simTimeZone_t
*
* @purpose
*
* @notes none
*
*********************************************************************/
typedef struct
{
  L7_uchar8 tzName[L7_TIMEZONE_ACRONYM_LEN+1];
  L7_int32  hour;
  L7_uint32 minutes;

}  simTimeZone_t;


typedef struct
{
  L7_uint32 ipAddr;
  L7_uint32 netMask;
  L7_uint32 gateway;
} simRouteStorage_t;

typedef enum
{
  SIM_ROUTE_ADD = 1,
  SIM_ROUTE_DELETE,
  SIM_ROUTE_MODIFY,
  SIM_ROUTE_CHANGE_LAST
}simRouteCmd_e;

/* SIM Address change event notification */
typedef enum
{
  SIM_ADDR_CHANGE_TYPE_NETWORK = 0,
  SIM_ADDR_CHANGE_TYPE_SERVICEPORT,
  SIM_ADDR_CHANGE_TYPE_TOTAL
} SIM_ADDR_CHANGE_TYPE_t;

typedef L7_RC_t (*SIM_ADDR_CHANGE_CB)(L7_COMPONENT_IDS_t registrar_ID, 
                                      SIM_ADDR_CHANGE_TYPE_t addrType, 
                                      L7_uchar8 family);

typedef struct simAddrChangeNotifyList_s
{
  L7_COMPONENT_IDS_t      registrar_ID;
  SIM_ADDR_CHANGE_TYPE_t  addrType;
  L7_uchar8               addrFamily;
  SIM_ADDR_CHANGE_CB      addrChangeCB;

} simAddrChangeNotifyList_t;

#define SIM_ADDR_CHANGE_REGISTRATIONS_MAX 4

/******************** conditional Override *****************************/

#ifdef INCLUDE_SIM_EXPORTS_OVERRIDES
#include "sim_exports_overrides.h"
#endif

#endif /* __SIM_EXPORTS_H_*/
