/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename dhcp_debug.h
*
* @purpose  debug and statistics related structs and APIs.
*
* @component DHCP Client
*
* @comments
*
* @create 01/01/2010
*
* @author ckrishna
* @end
*
**********************************************************************/

#ifndef _DHCPC_DEBUG_H_
#define _DHCPC_DEBUG_H_

#include "l7_common.h"
#include "l3_addrdefs.h"
#include "sysapi.h"

#define DHCPC_DEBUG_PRINTF sysapiPrintf

#define DHCPC_TRACE(traceLevel, __fmt__, __args__...)                          \
{                                                                              \
  if (dhcpClientDebugFlagCheck(traceLevel) == L7_TRUE)                         \
  {                                                                            \
    L7_char8  __buf1__[DHCP_CLIENT_DBG_MSG_SIZE_MAX];                          \
    L7_char8  __buf2__[DHCP_CLIENT_DBG_MSG_SIZE_MAX];                          \
                                                                               \
    osapiSnprintf (__buf1__, DHCP_CLIENT_DBG_MSG_SIZE_MAX, __fmt__, ## __args__); \
    osapiSnprintf (__buf2__, DHCP_CLIENT_DBG_MSG_SIZE_MAX,                 \
                   "\nDHCP_CLIENT[%s-%d]: %s", __FUNCTION__, __LINE__, __buf1__); \
    DHCPC_DEBUG_PRINTF (__buf2__);                                             \
  }                                                                            \
}                                                                              \

extern L7_uchar8 *dhcpDebugMgmtPortType[];

#define DHCPC_USER_TRACE(__fmt__, __args__... )                              \
    LOG_USER_TRACE(L7_DHCP_CLIENT_COMPONENT_ID, __fmt__,##__args__);        \

/*********************************************************************
*                   Trace Level Definition
*********************************************************************/
typedef enum
{
  DHCPC_DEBUG_INIT = 0,          /* 0  - Initialization */
  DHCPC_DEBUG_APIS,              /* 1  - APIs */
  DHCPC_DEBUG_TASK,              /* 2  - Task flow */
  DHCPC_DEBUG_EVENTS,            /* 3  - Events */
  DHCPC_DEBUG_STATE,             /* 4  - DHCP States */
  DHCPC_DEBUG_PKT_TX,            /* 5  - Tx Packet Flow */
  DHCPC_DEBUG_PKT_RX,            /* 6  - Rx Packet Flow */
  DHCPC_DEBUG_UPDATES,           /* 7  - Updates to Other components */
  DHCPC_DEBUG_INIT_DEINIT,       /* 8  - Initialization and De-initialization */
  DHCPC_DEBUG_GENERAL,           /* 9  - General Debug */
  DHCPC_DEBUG_FAILURE,           /* 10 - All Failures */
  DHCPC_DEBUG_TIMER,             /* 11 - Timers */
  DHCPC_DEBUG_PKT_SRVC_TX,       /* 12  - Tx Packet Flow for Serviceability*/
  DHCPC_DEBUG_PKT_SRVC_RX,       /* 13  - Rx Packet Flow for Serviceability*/
  DHCPC_DEBUG_LAST

}DHCPC_TRACE_LVL_t; /* DHCPC_DEBUG_PKT_SRVC_TX and DHCPC_DEBUG_PKT_SRVC_RX
                       always have to be at the end */

L7_BOOL
dhcpClientDebugFlagCheck (DHCPC_TRACE_LVL_t traceLevel);

L7_RC_t
dhcpClientDebugFlagSet (DHCPC_TRACE_LVL_t traceLevel);

L7_RC_t
dhcpClientDebugFlagReset (DHCPC_TRACE_LVL_t traceLevel);

L7_RC_t
dhcpClientDebugEnable (void);

L7_RC_t
dhcpClientDebugDisable (void);

L7_uchar8*
dhcpClientDebugAddrPrint (L7_uint32 addr,
                          L7_uchar8 *buf);
L7_BOOL
dhcpClientDebugServiceabilityFlagCheck (DHCPC_TRACE_LVL_t traceLevel);


#endif /*_DHCPC_DEBUG_H_ */

