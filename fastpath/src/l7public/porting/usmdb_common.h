/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/l7public/porting/usmdb_common.h
*
* @purpose  USMDB layer Common Include
*
* @component unitmgr
*
* @comments tba
*
* @create 08/16/2000
*
* @author cpverne
* @end
*
**********************************************************************/

#ifndef USMDB_COMMON_H
#define USMDB_COMMON_H

#include "l7_common.h"
#include "fdb_exports.h"
#include "comm_mask.h"
#include "nimapi.h"
#include "simapi.h"
#include "usmdb_counters_api.h"
#include "usmdb_pkg.h"
#include "fdb_api.h"
#include "usmdb_routing_stub_api.h"
#include "defaultconfig.h"
#include "compdefs.h"
#include "mfdb_exports.h"

#define USMDB_UNIT_CURRENT simGetThisUnit()
#define USMDB_SYSWEB_MODE_DISABLE 1
#define USMDB_SYSWEB_MODE_ENABLE  2

#define USMDB_DISABLE   0
#define USMDB_ENABLE    1

#define USMDB_SERIAL_TIMEOUT_MIN 0
#define USMDB_SERIAL_TIMEOUT_MAX 160   /*Minutes*/

#define USMDB_TELNET_TIMEOUT_MIN 1
#define USMDB_TELNET_TIMEOUT_MAX 160

#define USMDB_USER_NONE     0
#define USMDB_USER_DISPLAY  1
#define USMDB_USER_TFTP     2
#define USMDB_TABLE_SIZE    256
#define USMDB_TIME_STR_SIZE 25
#define USMDB_MSG_SIZE      275

/*********************************************************************
*
* @structures usmDbTrapLogEntry_t
*
* @purpose
*
* @notes none
*
*********************************************************************/
typedef struct
{
  L7_ushort16 gen;
  L7_ushort16 spec;
  L7_char8    timestamp[USMDB_TIME_STR_SIZE];
  L7_char8    message[USMDB_MSG_SIZE];
} usmDbTrapLogEntry_t;

/*********************************************************************
*
* @structures  usmdbFdbEntry_t
*
* @purpose
*
* @notes none
*
*********************************************************************/
typedef struct
{
  L7_uchar8   usmdbFdbMac[L7_FDB_KEY_SIZE];
  L7_uint32   usmdbFdbIntIfNum;
  L7_ushort16 usmdbFdbEntryType;
} usmdbFdbEntry_t;

/*********************************************************************
*
* @structures  usmdbArpEntry_t
*
* @purpose
*
* @notes none
*
*********************************************************************/
typedef struct
{
  L7_uchar8   usmdbArpMac[L7_MAC_ADDR_LEN];
  L7_uint32   ipAddr;
  L7_long32  arpIndex;
} usmdbArpEntry_t;

/*********************************************************************
*
* @structures usmDbTimeSpec_t
*
* @purpose
*
* @notes none
*
*********************************************************************/
typedef struct
{
  L7_uint32 days;
  L7_uint32 hours;
  L7_uint32 minutes;
  L7_uint32 seconds;
}  usmDbTimeSpec_t;


/*********************************************************************
*
* @structures  usmSlotInfo_t
*
* @purpose
*
* @notes none
*
*********************************************************************/
typedef struct
{
  L7_uint32 num_ports;
  L7_uint32 port_type;
} usmSlotInfo_t;


/*********************************************************************
*
* @structures  usmSlotInfo_t
*
* @purpose
*
* @notes none       TBD_POST_INTEROP: Move to ESA300 project
*
*********************************************************************/
typedef struct usmRouteInfo_s
{
  /* The network, protocol, router, and type of service uniqely identify
  ** each route.
  */
  L7_uint32 network;                                                           /* Destination Network */
  L7_uint32 protocol;                                                          /* Which protocol added this route */
  L7_uint32 router;                                                            /* Destination router */
  L7_uint32 type_of_service;                                                   /* Type of service for this route */

  L7_uint32 ifNum;                                                             /* IP Interface index of the router */
  L7_uint32 net_mask;                                                          /* Network mask for the route */

} usmRouteInfoType_t;


/*********************************************************************
*
* @structures  usmdbMfdbUserInfo_t
*
* @purpose
*
* @notes       none
*
*********************************************************************/
typedef struct
{
  L7_uint32         usmdbMfdbCompId;                                           /* user component's ID          */
  L7_uint32         usmdbMfdbType;                                             /* user component's type        */
  L7_uchar8         usmdbMfdbDescr[L7_MFDB_COMPONENT_DESCR_STRING_LEN];        /* user component's description */
  L7_INTF_MASK_t    usmdbMfdbFwdMask;                                          /* forwarding ports mask        */
  L7_INTF_MASK_t    usmdbMfdbFltMask;                                          /* filtering ports mask         */
} usmdbMfdbUserInfo_t;

/*********************************************************************
*
* @structures  usmdbMfdbEntry_t
*
* @purpose
*
* @notes       none
*
*********************************************************************/
typedef struct
{
  L7_uchar8           usmdbMfdbVidMac[L7_MFDB_KEY_SIZE];                       /* vlan id + mac address combo */
  usmdbMfdbUserInfo_t usmdbMfdbUser[L7_MFDB_MAX_USERS];                        /* entry-user's information    */
} usmdbMfdbEntry_t;

/*********************************************************************
*
* @structures  usmdbIsdpNeighborsTableIndex_t
*
* @purpose
*
* @notes none
*
*********************************************************************/
typedef struct
{
  L7_uchar8   usmdbDeviceId[L7_ISDP_DEVICE_ID_LEN];
  L7_uint32   usmdbIntIfnum;
} usmdbIsdpNeighborsTableIndex_t;

/*********************************************************************
*
* @purpose  Creates the set of user statistics for the specified interface
*           category
*
* @param    unitIndex   Unit Index
* @param    cat         USMDB user statistics category specifier
* @param    intf        internal interface number
*
* @returns  L7_SUCCESS  Counter set was successfully created
* @returns  L7_FAILURE  Function failed
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbUserStatsCreate(L7_uint32 unitIndex, usmdbUserStatsCat_t cat, L7_uint32 intf);

/*********************************************************************
*
* @purpose Deletes the set of user statistics for the specified
*          interface category.
*
* @param unitIndex    Unit Index
* @param cat          USMDB user statistics category specifier
* @param intf         internal interface number
*
* @returns L7_SUCCESS, if counter set was successfully created
* @returns L7_FAILURE, if function failed
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbUserStatsDelete(L7_uint32 unitIndex, usmdbUserStatsCat_t cat, L7_uint32 intf);

/*********************************************************************
*
* @purpose  Get a single statistic
*
* @param    unitIndex   Unit Index
* @param    uid         USMDB counter identifier for this statistic
* @param    intf        internal interface number for per-interface statistic
* @param    *val        pointer to output location
*                       (@b{Output:} returned value of statistic)
*
* @returns  L7_SUCCESS  Counter was successfully retrieved
* @returns  L7_ERROR    Requested counter does not exist
* @returns  L7_FAILURE  Function failed (output not valid)
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbStatGet(L7_uint32 unitIndex, L7_uint32 Id, L7_uint32 intf, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Reset a single statistic
*
* @param    unitIndex   Unit Index
* @param    uid         USMDB counter identifier for this statistic
* @param    intf        internal interface number for per-interface statistic
*
* @returns  L7_SUCCESS  Counter was successfully reset
* @returns  L7_ERROR    Requested counter does not exist or not resettable
* @returns  L7_FAILURE  Function failed
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbStatReset(L7_uint32 unitIndex, L7_uint32 uid, L7_uint32 intf);

/*********************************************************************
*
* @purpose  Reset all statistics on the specified interface
*
* @param    unitIndex   Unit Index
* @param    intf        internal interface number for per-interface statistic
*
* @returns  L7_SUCCESS  Counters were successfully reset
* @returns  L7_FAILURE  Function failed
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIntfStatReset(L7_uint32 unitIndex, L7_uint32 intf);

/*********************************************************************
*
* @purpose  Get a single 64-bit statistic
*
* @param    unitIndex   Unit Index
* @param    uid         USMDB counter identifier for this statistic
* @param    intf        internal interface number for per-interface statistic
* @param    *high       pointer to output location for high-order 32-bits
*                       (@b{Output:} returned value of statistic high)
* @param    *low        pointer to output location for low-order 32-bits
*                       (@b{Output:} returned value of statistic low)
*
* @returns  L7_SUCCESS  Counter was successfully retrieved
* @returns  L7_ERROR    Requested counter does not exist
* @returns  L7_FAILURE  Function failed (output not valid)
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbStatGet64(L7_uint32 unitIndex, L7_uint32 Id, L7_uint32 intf, L7_uint32 *high, L7_uint32 *low);


/*********************************************************************
*
* @purpose  Reset a single interface's statistic reset time
*
* @param    unitIndex   Unit Index
* @param    uid         USMDB counter identifier for this statistic
* @param    intf        internal interface number for per-interface statistic
*
* @returns  L7_SUCCESS  Counter was successfully reset
* @returns  L7_ERROR    Requested counter does not exist or not resettable
* @returns  L7_FAILURE  Function failed
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbStatTimeReset(L7_uint32 unitIndex, L7_uint32 intf);


/*********************************************************************
*
* @purpose Returns the number of non-translatable discards
*
* @param   L7_uint32 UnitIndex
* @param   L7_uint32 interface
* @param   L7_uint32 *val
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbNontranslatableDiscardsGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

#endif /* USMDB_COMMON_H */

