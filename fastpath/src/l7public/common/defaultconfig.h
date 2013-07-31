/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename defaultconfig.h
*
* @purpose All Factory default settings are in this file
*
* @component none
*
* @comments This file contains default values for end user configuration
*           parameters.  These values are used to build default configuration
*           files for components.
*
* @create 08/30/2000
*
* @author bmutz
* @end
*
**********************************************************************/

#ifndef DEFAULTCONFIG_H
#define DEFAULTCONFIG_H

#include <string.h>                /* for memcpy() etc... */
#include "datatypes.h"
#include "l7_product.h"
#include "registry.h"
#include "commdefs.h"
#include "defaultconfig_pkg.h"
#include "default_cnfgr.h"
#include "nimapi.h"
#include "sim_exports.h"
#include "snmp_util_api.h"
#include "log.h"
#include "bspapi.h"
#include "chassis_commdefs.h"
#include "pfc_api.h"
#include "platform_config.h"
#include "sdm_exports.h"

#define L7_DEFAULT_SYSTEM_DESCRIPTION       "Ethernet Switch"

/*****************************************************************
*
*     SIM Configuration data
*
******************************************************************/

#define L7_AUTH_METHOD_DEFAULT  L7_AUTH_METHOD_LOCAL

/**Spanning Tree Defaults****/
#define FD_DOT1S_STMST_PRIORITY   32768

/* Start of SIM Component's Factory Defaults */

#define FD_SIM_MACTYPE                      L7_SYSMAC_BIA
#define FD_SIM_ADDR_CONFLICT_DETECT_MODE    L7_ENABLE
#ifdef L7_PRODUCT_SMARTPATH
#define FD_SIM_NETWORK_CONFIG_MODE          L7_SYSCONFIG_MODE_NONE
#define FD_SIM_SERVPORT_CONFIG_MODE         L7_SYSCONFIG_MODE_NONE

#define FD_SIM_SERVPORT_ADMIN_STATE         L7_ENABLE

#define FD_SIM_NETWORK_IPV6_CONFIG_MODE     L7_SYSCONFIG_MODE_NONE
#define FD_SIM_SERVPORT_IPV6_CONFIG_MODE    L7_SYSCONFIG_MODE_NONE

#define FD_SIM_NETWORK_IPV6_AUTOCONFIG_MODE   L7_DISABLE
#define FD_SIM_SERVPORT_IPV6_AUTOCONFIG_MODE  L7_DISABLE
#else
#define FD_SIM_NETWORK_CONFIG_MODE          L7_SYSCONFIG_MODE_NONE
#define FD_SIM_SERVPORT_CONFIG_MODE         L7_SYSCONFIG_MODE_DHCP

#define FD_SIM_SERVPORT_ADMIN_STATE         L7_ENABLE

#define FD_SIM_NETWORK_IPV6_CONFIG_MODE     L7_SYSCONFIG_MODE_NONE
#define FD_SIM_SERVPORT_IPV6_CONFIG_MODE    L7_SYSCONFIG_MODE_NONE

#define FD_SIM_NETWORK_IPV6_AUTOCONFIG_MODE   L7_DISABLE
#define FD_SIM_SERVPORT_IPV6_AUTOCONFIG_MODE  L7_DISABLE
#endif
#define FD_SIM_PORT_MONITOR_MODE            L7_DISABLE
#define FD_SIM_DEFAULT_STACK_ID             0
#define FD_SIM_DEFAULT_SYSTEM_OID_NAME      "broadcom"
#define FD_SIM_DEFAULT_SERIAL_PORT_TIMEOUT  15      /* PTin modified: session timeout (5) */
#define FD_SIM_DEFAULT_SYSTEM_BAUD_RATE          bspapiDefaultBaudRateGet()
#define FD_SIM_DEFAULT_SERIAL_PORT_PARITY_TYPE   L7_PARITY_NONE
#define FD_SIM_DEFAULT_SERIAL_PORT_STOPBITS      L7_STOP_BIT_1
#define FD_SIM_DEFAULT_SERIAL_PORT_FLOW_CONTROL  L7_DISABLE
#define FD_SIM_DEFAULT_SERIAL_PORT_CHAR_SIZE     8

#if L7_FEAT_NETWORK_PORT
#define FD_SIM_DEFAULT_MGMT_VLAN_ID 1
#else
#define FD_SIM_DEFAULT_MGMT_VLAN_ID 0
#endif
#define FD_SIM_DEFAULT_MGMT_PORT                 0
#define FD_SIM_DEFAULT_SERVPORT_IPV6_ADMIN_MODE  L7_ENABLE
#define FD_SIM_DEFAULT_SYSTEM_IPV6_ADMIN_MODE    L7_ENABLE
#define FD_SIM_MAX_INTF_IPV6_ADDRS          4

#define FD_SIM_DEFAULT_TRANSFER_MODE        L7_TRANSFER_TFTP
#define FD_SIM_DEFAULT_TRANSFER_SERVER_ADDR_TYPE     L7_AF_INET
#define FD_SIM_DEFAULT_TRANSFER_SERVER_IP   0
#define FD_SIM_DEFAULT_UPLOAD_FILE_TYPE     L7_FILE_TYPE_ERRORLOG
#define FD_SIM_DEFAULT_DOWNLOAD_FILE_TYPE   L7_FILE_TYPE_CODE
#define FD_SIM_DEFAULT_MAC_ADR              ""
#define FD_SIM_DEFAULT_MGMT_IP_ADDR              0x00000000  /* 0.0.0.0 */
#define FD_SIM_DEFAULT_MGMT_IP_MASK              0x00000000  /* 0.0.0.0 */


/* Define separate default address for network and service port 
 * and distinguish these by product */
#define FD_SIM_DEFAULT_SERVPORT_IP_ADDR    PROD_SIM_DEFAULT_SERVPORT_IP_ADDR  
#define FD_SIM_DEFAULT_SERVPORT_MASK       PROD_SIM_DEFAULT_SERVPORT_MASK  
#define FD_SIM_DEFAULT_SERVPORT_GW         PROD_SIM_DEFAULT_SERVPORT_GW  

#define FD_SIM_DEFAULT_NETWORK_IP_ADDR     PROD_SIM_DEFAULT_NETWORK_IP_ADDR  
#define FD_SIM_DEFAULT_NETWORK_MASK        PROD_SIM_DEFAULT_NETWORK_MASK  
#define FD_SIM_DEFAULT_NETWORK_GW          PROD_SIM_DEFAULT_NETWORK_GW  


/* Begin SNMP component's factory defaults */
#define FD_SNMP_SVR_DEFAULT_SYSNAME                           ""
#define FD_SNMP_SVR_DEFAULT_SYSLOC                            ""
#define FD_SNMP_SVR_DEFAULT_CONTACT                           ""
#define FD_SNMP_DEFAULT_ACCESS_MODE                           L7_LOGIN_ACCESS_READ_ONLY
#define FD_SNMP_DEFAULT_ADMIN_ACCESS_MODE                     L7_LOGIN_ACCESS_READ_WRITE
#define FD_SNMP_DEFAULT_GUEST_ACCESS_MODE                     L7_LOGIN_ACCESS_READ_ONLY
#define FD_SNMPV3_DEFAULT_ACCESS_MODE                           L7_SNMP_USER_ACCESS_LEVEL_READ_ONLY
#define FD_SNMPV3_DEFAULT_ADMIN_ACCESS_MODE                   L7_SNMP_USER_ACCESS_LEVEL_READ_WRITE
#define FD_SNMPV3_DEFAULT_GUEST_ACCESS_MODE                   L7_SNMP_USER_ACCESS_LEVEL_READ_ONLY
#define FD_SNMP_DEFAULT_USER_AUTH                             L7_SNMP_USER_AUTH_PROTO_NONE
#define FD_SNMP_DEFAULT_PRIV_PROTO                            L7_SNMP_USER_PRIV_PROTO_NONE

#define FD_SNMP_DEFAULT_TRAP_VERSION                          L7_SNMP_TRAP_VER_SNMPV2C

#define FD_DEFAULT_SNMP_RETRIES_COUNT      3
#define FD_DEFAULT_SNMP_TIMEOUT_COUNT      15
#define FD_DEFAULT_SNMP_UDP_PORT           1162     /* PTin modified: port connections (162) */
#define FD_DEFAULT_SNMP_VERSION            1
#define FD_DEFAULT_SNMP_SECURITY           1

#ifdef SNMP_FACTORY_DEFAULT
#if SNMP_FACTORY_DEFAULT == FACTORY_DEFAULT_DEFINE
snmpComm_t FD_snmpComm[L7_MAX_SNMP_COMM] =
{
  {                                                                       /* table entry 0 */
    "public",                                                             /* communityName        */
    "default",                                                            /* communityOwner       */
    0x0,                                                                  /* communityIpAddr      */
    0x0,                                                                  /* communityIpMask      */
    L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY,                             /* communityAccessLevel */
    L7_SNMP_COMMUNITY_STATUS_VALID                                        /* communityStatus      */
  },
  {                                                                       /* table entry 1 */
    "private",
    "default",
    0x0,
    0x0,
    L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE,
    L7_SNMP_COMMUNITY_STATUS_VALID
  },
  {                                                                       /* table entry 2 */
    "",
    "",
    0x0,
    0x0,
    L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY,
    L7_SNMP_COMMUNITY_STATUS_DELETE
  },
  {                                                                       /* table entry 3 */
    "",
    "",
    0x0,
    0x0,
    L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY,
    L7_SNMP_COMMUNITY_STATUS_DELETE
  },
  {                                                                       /* table entry 4 */
    "",
    "",
    0x0,
    0x0,
    L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY,
    L7_SNMP_COMMUNITY_STATUS_DELETE
  },
  {                                                                       /* table entry 5 */
    "",
    "",
    0x0,
    0x0,
    L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY,
    L7_SNMP_COMMUNITY_STATUS_DELETE
  }
};

snmpTrapMgr_t FD_snmpTrapMgr[L7_MAX_SNMP_COMM] =
{
  {                                                                       /* table entry 0 */
    "",
    0x0,
    {{{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}}},
    L7_SNMP_TRAP_MGR_STATUS_DELETE,
    FD_SNMP_DEFAULT_TRAP_VERSION
  },
  {                                                                       /* table entry 1 */
    "",
    0x0,
    {{{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}}},
    L7_SNMP_TRAP_MGR_STATUS_DELETE,
    FD_SNMP_DEFAULT_TRAP_VERSION
  },
  {                                                                       /* table entry 2 */
    "",
    0x0,
    {{{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}}},
    L7_SNMP_TRAP_MGR_STATUS_DELETE,
    FD_SNMP_DEFAULT_TRAP_VERSION
  },
  {                                                                       /* table entry 3 */
    "",
    0x0,
    {{{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}}},
    L7_SNMP_TRAP_MGR_STATUS_DELETE,
    FD_SNMP_DEFAULT_TRAP_VERSION
  },
  {                                                                       /* table entry 4 */
    "",
    0x0,
    {{{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}}},
    L7_SNMP_TRAP_MGR_STATUS_DELETE,
    FD_SNMP_DEFAULT_TRAP_VERSION
  },
  {                                                                       /* table entry 5 */
    "",
    0x0,
    {{{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}}},
    L7_SNMP_TRAP_MGR_STATUS_DELETE,
    FD_SNMP_DEFAULT_TRAP_VERSION
  }
};

#else
extern snmpComm_t FD_snmpComm[L7_MAX_SNMP_COMM];
extern snmpTrapMgr_t FD_snmpTrapMgr[L7_MAX_SNMP_COMM];
#endif
#endif /* SNMP_FACTORY_DEFAULT */

/* Start of Cli Web Mgr Factory Defaults */

#define FD_USER_MGR_PASSWD_MIN_LENGTH   8
#define FD_USER_MGR_PASSWD_HIST_LENGTH  0
#define FD_USER_MGR_PASSWD_AGING_PERIOD 0
#define FD_USER_MGR_PASSWD_USER_LOCKOUT 0

#define FD_USER_MGR_LOGINS_SIZE   7
#define FD_NONCONFIGUREDAUTH_SIZE   2
#define FD_APL_SIZE   10

#define FD_USER_MGR_ADMIN_LOGIN_NAME        "admin"
#define FD_USER_MGR_GUEST_LOGIN_NAME        "guest"
#define FD_USER_MGR_DEFAULT_PASSWORD        ""
#define FD_USER_MGR_DEFAULT_LOGIN_STATUS    L7_ENABLE

/* STRONG PASSWORD */
#define FD_USER_MGR_PASSWD_MIN_UPPERCASE_CHARS    2
#define FD_USER_MGR_PASSWD_MIN_LOWERCASE_CHARS    2
#define FD_USER_MGR_PASSWD_MIN_NUMERIC_CHARS      2
#define FD_USER_MGR_PASSWD_MIN_SPL_CHARS          2
#define FD_USER_MGR_PASSWD_MAX_CONSECUTIVE_CHARS  0
#define FD_USER_MGR_PASSWD_MAX_REPEATED_CHARS     0
#define FD_USER_MGR_PASSWD_MIN_CHARACTER_CLASSES  4

#define FD_USER_MGR_PASSWD_CHECK_FLAGS  0x3FF


#define FD_LOGINS                               \
  {                                             \
    {                                           \
      /* table entry 0 */ /* Admin User */      \
      FD_USER_MGR_ADMIN_LOGIN_NAME,             \
      FD_USER_MGR_DEFAULT_PASSWORD,             \
      L7_FALSE,                                 \
      FD_SNMP_DEFAULT_ADMIN_ACCESS_MODE,        \
      FD_USER_MGR_DEFAULT_LOGIN_STATUS,         \
      L7_FALSE,                                 \
      0,                                        \
      0,                                        \
      0,                                        \
      0,                                        \
      {{0}},                                    \
      0,                                        \
      L7_DISABLE,                               \
      L7_SNMP_USER_AUTH_PROTO_NONE,             \
      L7_SNMP_USER_PRIV_PROTO_NONE,             \
      "",                                       \
      FD_SNMPV3_DEFAULT_ADMIN_ACCESS_MODE,      \
                                                \
      {                                         \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_USER_MGR_COMPONENT_ID              \
        },                                      \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_DOT1X_COMPONENT_ID                 \
        }                                       \
      }                                         \
                                                \
    },                                          \
    {                                           \
      /* table entry 1 */ /* Guest User */      \
      FD_USER_MGR_GUEST_LOGIN_NAME,             \
      FD_USER_MGR_DEFAULT_PASSWORD,             \
      L7_TRUE,                                  \
      FD_SNMP_DEFAULT_GUEST_ACCESS_MODE,        \
      FD_USER_MGR_DEFAULT_LOGIN_STATUS,         \
      L7_FALSE,                                 \
      0,                                        \
      0,                                        \
      0,                                        \
      0,                                        \
      {{0}},                                    \
      0,                                        \
      L7_DISABLE,                               \
      L7_SNMP_USER_AUTH_PROTO_NONE,             \
      L7_SNMP_USER_PRIV_PROTO_NONE,             \
      "",                                       \
      FD_SNMPV3_DEFAULT_GUEST_ACCESS_MODE,      \
                                                \
      {                                         \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_USER_MGR_COMPONENT_ID              \
        },                                      \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_DOT1X_COMPONENT_ID                 \
        }                                       \
      }                                         \
                                                \
    },                                          \
    {                                           \
      /* table entry 2 */                       \
      "",                                       \
      "",                                       \
      L7_TRUE,                                  \
      FD_SNMP_DEFAULT_ACCESS_MODE,              \
      L7_DISABLE,                               \
      L7_FALSE,                                 \
      0,                                        \
      0,                                        \
      0,                                        \
      0,                                        \
      {{0}},                                    \
      0,                                        \
      L7_DISABLE,                               \
      FD_SNMP_DEFAULT_USER_AUTH,                \
      FD_SNMP_DEFAULT_PRIV_PROTO,               \
      "",                                       \
      FD_SNMPV3_DEFAULT_ACCESS_MODE,            \
                                                \
      {                                         \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_USER_MGR_COMPONENT_ID              \
        },                                      \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_DOT1X_COMPONENT_ID                 \
        }                                       \
      }                                         \
                                                \
    },                                          \
    {                                           \
      /* table entry 3 */                       \
      "",                                       \
      "",                                       \
      L7_TRUE,                                  \
      FD_SNMP_DEFAULT_ACCESS_MODE,              \
      L7_DISABLE,                               \
      L7_FALSE,                                 \
      0,                                        \
      0,                                        \
      0,                                        \
      0,                                        \
      {{0}},                                    \
      0,                                        \
      L7_DISABLE,                               \
      FD_SNMP_DEFAULT_USER_AUTH,                \
      FD_SNMP_DEFAULT_PRIV_PROTO,               \
      "",                                       \
      FD_SNMPV3_DEFAULT_ACCESS_MODE,            \
                                                \
      {                                         \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_USER_MGR_COMPONENT_ID              \
        },                                      \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_DOT1X_COMPONENT_ID                 \
        }                                       \
      }                                         \
                                                \
    },                                          \
    {                                           \
      /* table entry 4 */                       \
      "",                                       \
      "",                                       \
      L7_TRUE,                                  \
      FD_SNMP_DEFAULT_ACCESS_MODE,              \
      L7_DISABLE,                               \
      L7_FALSE,                                 \
      0,                                        \
      0,                                        \
      0,                                        \
      0,                                        \
      {{0}},                                    \
      0,                                        \
      L7_DISABLE,                               \
      FD_SNMP_DEFAULT_USER_AUTH,                \
      FD_SNMP_DEFAULT_PRIV_PROTO,               \
      "",                                       \
      FD_SNMPV3_DEFAULT_ACCESS_MODE,            \
                                                \
      {                                         \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_USER_MGR_COMPONENT_ID              \
        },                                      \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_DOT1X_COMPONENT_ID                 \
        }                                       \
      }                                         \
                                                \
    },                                          \
    {                                           \
      /* table entry 5 */                       \
      "",                                       \
      "",                                       \
      L7_TRUE,                                  \
      FD_SNMP_DEFAULT_ACCESS_MODE,              \
      L7_DISABLE,                               \
      L7_FALSE,                                 \
      0,                                        \
      0,                                        \
      0,                                        \
      0,                                        \
      {{0}},                                    \
      0,                                        \
      L7_DISABLE,                               \
      FD_SNMP_DEFAULT_USER_AUTH,                \
      FD_SNMP_DEFAULT_PRIV_PROTO,               \
      "",                                       \
      FD_SNMPV3_DEFAULT_ACCESS_MODE,            \
                                                \
      {                                         \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_USER_MGR_COMPONENT_ID              \
        },                                      \
        {                                       \
          L7_DEFAULT_APL_NAME,                  \
          L7_DOT1X_COMPONENT_ID                 \
        }                                       \
      }                                         \
                                                \
                                                \
    }                                           \
  }

#define FD_LOGIN_APLS                           \
  {                                             \
    {                                           \
      L7_DEFAULT_APL_NAME,                      \
      {                                         \
        L7_AUTH_METHOD_LOCAL,                    \
        L7_AUTH_METHOD_UNDEFINED,               \
        L7_AUTH_METHOD_UNDEFINED                \
      },                                        \
      L7_TRUE                                   \
    },                                          \
    {                                           \
      L7_DEFAULT_NETWORK_APL_NAME,              \
      {                                         \
        L7_AUTH_METHOD_LOCAL,                   \
        L7_AUTH_METHOD_UNDEFINED,               \
        L7_AUTH_METHOD_UNDEFINED                \
      },                                        \
      L7_TRUE                                   \
    }                                           \
  }


#define FD_ENABLE_APLS                          \
  {                                             \
    L7_ENABLE_APL_NAME,                         \
    {                                           \
      L7_AUTH_METHOD_ENABLE,                      \
      L7_AUTH_METHOD_UNDEFINED,                 \
      L7_AUTH_METHOD_UNDEFINED                  \
    },                                          \
    L7_TRUE                                     \
  }

#define FD_WEB_APLS                             \
  {                                             \
    L7_DEFAULT_HTTP_APL_NAME,                   \
    {                                           \
      L7_AUTH_METHOD_LOCAL,                     \
      L7_AUTH_METHOD_UNDEFINED,                 \
      L7_AUTH_METHOD_UNDEFINED                  \
    },                                          \
    L7_TRUE                                     \
  }

#define FD_DOT1X_APLS                           \
  {                                             \
    L7_DEFAULT_HTTP_APL_NAME,                   \
    {                                           \
      L7_AUTH_METHOD_UNDEFINED,                 \
      L7_AUTH_METHOD_UNDEFINED,                 \
      L7_AUTH_METHOD_UNDEFINED                  \
    },                                          \
    L7_TRUE                                     \
  }

#define FD_CLI_DEFAULT_MAX_CONNECTIONS      (FD_SSHD_MAX_SESSIONS + \
                                            FD_TELNET_DEFAULT_MAX_SESSIONS + 1)
#define FD_APL_DEFAULT_LIST_NAME           "defaultList"
#define FD_CLI_WEB_JAVA_MODE                 L7_ENABLE
#define FD_CLI_WEB_MODE                      L7_ENABLE
#define FD_CLI_WEB_TELNET_NEW_SESSIONS       L7_ENABLE
#define FD_CLI_WEB_TELNET_DEFAULT_ADMIN_MODE L7_ENABLE
#define FD_CLI_WEB_DEFAULT_REMOTECON_TIMEOUT     5
#define FD_CLI_WEB_DEFAULT_LOGIN_NAME            ""
#define FD_WEB_DEFAULT_MAX_CONNECTIONS           FD_HTTP_DEFAULT_MAX_CONNECTIONS+FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS
#define FD_UI_DEFAULT_MAX_CONNECTIONS            FD_CLI_DEFAULT_MAX_CONNECTIONS+FD_WEB_DEFAULT_MAX_CONNECTIONS
#define FD_CP_WEB_DEFAULT_MAX_CONNECTIONS        FD_CP_HTTP_DEFAULT_MAX_CONNECTIONS+FD_CP_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS
#define FD_CLI_WEB_DEFAULT_REMOTECON_MIN_TIMEOUT     1
#define FD_CLI_WEB_DEFAULT_REMOTECON_MAX_TIMEOUT     160


#define FD_HTTP_SESSION_SOFT_TIMEOUT_MIN            1       /* HTTP Soft timeout range start at 1 (instead of 0).*/
#define FD_HTTP_SESSION_SOFT_TIMEOUT_MAX            60      /* 60 minutes */
#define FD_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT        60      /* PTin modified: session timeout (5) */ /* 5 minutes */
#define FD_HTTP_SESSION_HARD_TIMEOUT_MIN            1       /* HTTP Hard timeout range start at 1 (instead of 0).*/
#define FD_HTTP_SESSION_HARD_TIMEOUT_MAX           (24*7)   /* 1 week */
#define FD_HTTP_SESSION_HARD_TIMEOUT_DEFAULT        24      /* 1 day */
#define FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_MIN     1       /* 1 minute */
#define FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_MAX     60      /* 1 hour */
#define FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT 15      /* PTin modified: session timeout (5) */ /* 5 minutes */
#define FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_MIN     1       /* 1 hour */
#define FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_MAX     (24*7)  /* 1 week */
#define FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_DEFAULT (24)    /* 1 day */

#define FD_MILLISECONDS_BETWEEN_PINGS        100
#define FD_CLI_LINES_FOR_PAGINATION          24

#define FD_TXT_RUN_CFG_SUPPORTED          L7_FALSE
/* End of Cli Web Mgr Factory Defaults */

/* Start of NIM Component's Factory Defaults */

#define FD_NIM_MACTYPE                 L7_SYSMAC_BIA
#define FD_NIM_IF_ALIAS                ""
#define FD_NIM_ADMIN_STATE             L7_ENABLE
#define FD_NIM_TRAP_STATE              L7_ENABLE
#define FD_NIM_NAME_TYPE               L7_SYSNAME
#define FD_NIM_AUTONEG_ENABLE          L7_ENABLE
#define FD_NIM_AUTONEG_DISABLE         L7_DISABLE
#define FD_NIM_FAST_ENET_SPEED         L7_PORTCTRL_PORTSPEED_FULL_100TX
#define FD_NIM_FAST_ENET_FX_SPEED      L7_PORTCTRL_PORTSPEED_FULL_100FX
#define FD_NIM_GIG_ENET_SPEED          L7_PORTCTRL_PORTSPEED_FULL_1000SX    /* PTin changed from 100TX to 1000SX */
#define FD_NIM_2G5_ENET_SPEED          L7_PORTCTRL_PORTSPEED_FULL_2P5FX     /* PTin added: Speed 2.5G */
#define FD_NIM_10G_ENET_SPEED          L7_PORTCTRL_PORTSPEED_FULL_10GSX
#define FD_NIM_40G_ENET_SPEED          L7_PORTCTRL_PORTSPEED_FULL_40G_KR4   /* PTin added: Speed 40G */
#define FD_NIM_100G_ENET_SPEED         L7_PORTCTRL_PORTSPEED_FULL_100G_BKP  /* PTin added: Speed 100G */
#define FD_NIM_OTHER_PORT_TYPE_SPEED   L7_PORTCTRL_PORTSPEED_UNKNOWN
#define FD_NIM_ENCAPSULATION_TYPE      L7_ENCAP_ETHERNET
#define FD_NIM_DEFAULT_MTU_SIZE        L7_PORT_ENET_ENCAP_MAX_MTU
#define FD_NIM_DEFAULT_MAX_FRAME_SIZE  L7_MIN_FRAME_SIZE
#define FD_NIM_DEFAULT_UNIT_NUM        0

/* End of NIM Component's Factory Defaults */

/* Start of TRAP Component's Factory Defaults */

#define FD_TRAP_AUTH          L7_ENABLE
#define FD_TRAP_LINK_STATUS   L7_ENABLE
#define FD_TRAP_MULTI_USERS   L7_ENABLE
#define FD_TRAP_SPAN_TREE     L7_ENABLE
#define FD_TRAP_DOT1Q         L7_ENABLE
#define FD_TRAP_VRRP          L7_VRRP_TRAP_ALL
#define FD_TRAP_RELAY         L7_DISABLE
#define FD_TRAP_PIM           L7_DISABLE
#define FD_TRAP_DVMRP         L7_DISABLE
#define FD_TRAP_POE           L7_ENABLE
#define FD_TRAP_MACLOCK_VIOLATION L7_ENABLE
#define FD_TRAP_ACL           L7_DISABLE
#define FD_TRAP_WIRELESS      L7_ENABLE
#define FD_TRAP_CAPTIVE_PORTAL L7_DISABLE

/* This is the default configuration of OSPF trapflags (all disabled) */
#define FD_TRAP_OSPF_TRAP     L7_DISABLE
#define FD_TRAP_OSPFV3_TRAP   L7_DISABLE

/* End of TRAP Component's Factory Defaults */

/*GARP TIMERS */
#define DEFAULT_GVRP_VLAN_CREATION_FORBID L7_FALSE
#define DEFAULT_GVRP_REGISTRATION_FORBID L7_FALSE
/*--------------------------------------*/
/*  GARP Constants                      */
/*--------------------------------------*/
#define DEFAULT_GARPCONFIGJOINTIME          200
#define DEFAULT_GARPCONFIGLEAVETIME         600
#define DEAFULT_GARPCONFIGLEAVEALLTIME      10000




/* Start of DOT1Q Component's Factory Defaults */

/* End user configurable parameters */
#define FD_DOT1Q_DEFAULT_JOIN_TIME      20
#define FD_DOT1Q_DEFAULT_LEAVE_TIME     60
#define FD_DOT1Q_DEFAULT_LEAVEALL_TIME  100
#define FD_DOT1Q_DEFAULT_VLAN                        1
#define FD_DOT1Q_DEFAULT_VLAN_NAME                  "Default"
#define FD_DOT1Q_DEFAULT_VLAN_INTF_SPEED            L7_PORTCTRL_PORTSPEED_HALF_10T
#define FD_DOT1Q_DEFAULT_VLAN_INTF_ADMIN_STATE      L7_ENABLE
#define FD_DOT1Q_DEFAULT_VLAN_INTF_TRAP_STATE       L7_ENABLE
#define FD_DOT1Q_DEFAULT_VLAN_INTF_IP_MTU           L7_PORT_ENET_ENCAP_MAX_MTU
#define FD_DOT1Q_DEFAULT_VLAN_INTF_ENCAP_TYPE       L7_ENCAP_ETHERNET
#define FD_DOT1Q_DEFAULT_VLAN_INTF_CFG_MAX_FRAME_SIZE   L7_MIN_FRAME_SIZE
#define FD_DOT1Q_DEFAULT_INGRESS_FILTER_PORT        L7_ENABLE                         /* PTin modified: default */
#define FD_DOT1Q_DEFAULT_ACCEPT_FRAME_TYPE_ADMITALL L7_TRUE
/*#define FD_DOT1Q_DEFAULT_ACCEPT_FRAME_TYPE          L7_DOT1Q_ADMIT_ONLY_VLAN_UNTAGGED*/
#define FD_DOT1Q_DEFAULT_ACCEPT_FRAME_TYPE          L7_DOT1Q_ADMIT_ALL
#define FD_DOT1Q_DEFAULT_SWPORT_MODE                DOT1Q_SWPORT_MODE_GENERAL
/* ACCESS_VLAN is valid only when swport_mode is access*/
#define FD_DOT1Q_DEFAULT_ACCESS_VLAN                FD_DOT1Q_DEFAULT_VLAN
#define FD_DOT1Q_DEFAULT_PVID                       FD_DOT1Q_DEFAULT_VLAN
#define FD_DOT1Q_DEFAULT_TAGGING                    L7_DOT1Q_UNTAGGED

/* By default, assign VLAN IDs to VLAN routing interface starting from
 * the highest VLAN ID and working down. */
#define FD_DOT1Q_BASE_INTERNAL_VLAN                 L7_PLATFORM_MAX_VLAN_ID
#define FD_DOT1Q_INTERNAL_VLAN_POLICY               DOT1Q_INT_VLAN_POLICY_DESCENDING

/* Porting parameters */

/* VLAN interface descriptions */
#define DOT1Q_VLAN_INTF_BCAST_MAC_ADDR              L7_ENET_BCAST_MAC_ADDR
#define DOT1Q_VLAN_INTF_MAX_FRAME_SIZE_LIMIT        L7_MAX_FRAME_SIZE
#define DOT1Q_VLAN_INTF_IFNAME_PREFIX               "VLAN-"

/* End of DOT1Q Component's Factory Defaults */

/* Start of DOT1P Component's Factory Defaults */

#ifdef DOT1D_PRIORITY_TABLE
#if DOT1D_PRIORITY_TABLE == FACTORY_DEFAULT_DEFINE
L7_ushort16 dot1dUserPriorityRegenTable[8][8] = {
/*                      1 */  {0,0,0,0,0,0,0,0},
/*                      2 */  {0,0,0,0,1,1,1,1},
/*                      3 */  {0,0,0,0,1,1,2,2},
/*                      4 */  {1,0,0,1,2,2,3,3},
/*                      5 */  {1,0,0,1,2,3,4,4},
/*                      6 */  {1,0,0,2,3,4,5,5},
/*                      7 */  {1,0,0,2,3,4,5,6},
/*                      8 */  {2,0,1,3,4,5,6,7}
};
#else
extern L7_ushort16 dot1dUserPriorityRegenTable[8][8];
#endif
#endif

#define FD_DOT1P_DEFAULT_USER_PRIORITY                L7_DOT1P_DEFAULT_USER_PRIORITY
#define FD_DOT1P_NUM_TRAFFIC_CLASSES        L7_DOT1P_NUM_TRAFFIC_CLASSES

/* End of DOT1P Component's Factory Defaults */

/* Start of FDB Component's Factory Defaults */

#define FD_FDB_DEFAULT_AGING_TIMEOUT   300 /* seconds */

/* End of FDB Component's Factory Defaults */

/* Start of PoE Component's Factory Defaults */
#ifdef L7_PRODUCT_SMARTPATH
#define FD_POE_POWER_LIMIT_TYPE L7_POE_LIMIT_TYPE_NONE
#define FD_POE_POWER_MGMT       L7_POE_POWER_MGMT_STATIC_PRI
#else
#define FD_POE_POWER_LIMIT_TYPE L7_POE_LIMIT_TYPE_USER
#define FD_POE_POWER_MGMT       L7_POE_POWER_MGMT_DYNAMIC
#endif
#define FD_POE_DETECTION        L7_POE_DETECTION_4PT_DOT3AF
#define FD_POE_HIGH_POWER       L7_DISABLE
#define FD_POE_DISCONNECT_TYPE  L7_POE_POWER_DISC_AC
#define FD_POE_AUTO_RESET       L7_ENABLE
#define FD_POE_LOGICAL_MAP_MODE L7_ENABLE

#define FD_POE_ADMIN_ENABLE                  L7_ENABLE
#define FD_POE_PORT_ADMIN_ENABLE             L7_ENABLE
#define FD_POE_NOTIFICATION_CONTROL_ENABLE   L7_DISABLE
#define FD_POE_POWER_PAIRS                   L7_POE_PORT_POWER_PAIRS_SIGNAL
#define FD_POE_POWER_PRIORITY                L7_POE_PRIORITY_LOW
#define FD_POE_POWER_LIMIT                   15400 /* mW */
#define FD_POE_USAGETHRESHOLD                90

/* End of PoE Component's Factory Defaults */

/* Spanning Tree Instance Constants

   L7_MAX_STP_INSTANCES defines the maximum number of Spanning Tree Instances.
   L7_NO_STP_INSTANCE is used for interfaces which are not members of a
   spannng tree instance. This is the default at startup.

*/
#define FD_STATIC_FILTERING_ENABLED L7_TRUE
#define FD_VLAN_FILTERING_MODE L7_FILTER_FORWARD_UNREGISTERED

#define L7_MAX_STP_INSTANCES    1
#define L7_NO_STP_INSTANCE      L7_MAX_STP_INSTANCES+1

#define L7_MAX_FDB_STATIC_MAC_ENTRIES    (200 + L7_MAX_FDB_STATIC_FILTER_ENTRIES)
#define L7_MAX_FDB_DYNAMIC_MAC_ENTRIES  (L7_MAX_FDB_MAC_ENTRIES-L7_MAX_FDB_STATIC_MAC_ENTRIES)

#define  L7_MIN_VLANS       1

/*Number of GMRP ,IGMP Snooping and Static filtering for multicast addresses
 * and destination ports Group Registration Entries*/
#define L7_MAX_GROUP_REGISTRATION_ENTRIES L7_MFDB_MAX_MAC_ENTRIES

/* GARP/GMRP default status of the switch */
#define FD_GMRP_GARP_DEFAULT_SWITCH_STATUS       L7_DISABLE

/* GARP/GMRP default status of the Port */
#define FD_GMRP_GARP_DEFAULT_PORT_STATUS         L7_DISABLE
#define FD_GVRP_GARP_DEFAULT_SWITCH_STATUS       L7_DISABLE
#define FD_GVRP_DEFAULT_VLAN_CREATE_FORBID       L7_DISABLE
#define FD_GVRP_DEFAULT_REGISTER_FORBID          L7_DISABLE

/* GVRP default interface mode */
#define FD_GVRP_DEFAULT_INTERFACE_MODE           L7_DISABLE

/* Start of PBVLAN specific define statements                                 */

#define L7_PBVLAN_MAX_NUM_GROUPS                 128
#define L7_PBVLAN_MIN_NUM_GROUPS                 1
#define L7_PBVLAN_ETYPE_MIN              0x00000600  /* 1536  */
#define L7_PBVLAN_ETYPE_MAX              0x0000FFFF  /* 65535 */
#define L7_PBVLAN_DEFAULT_GROUP_NAME            ""

#define L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS    16 
#define FD_PBVLAN_DEFAULT_GROUP_VLAN_ID          0

/* End of PBVLAN specific define statements                                   */

/* Start of Port mirroring specific define statements                         */

#define  FD_MIRRORING_MODE                    L7_DISABLE
#define  FD_MIRRORING_DIRECTION               L7_MIRROR_BIDIRECTIONAL
#define  FD_MIRRORING_TRACE_MODE              L7_DISABLE

/* End of Port mirroring specific define statements                         */

/* Number of filtering databases supported  */
#define L7_MAX_FILTERING_DATABASES L7_MAX_VLAN_PER_BRIDGE

/* Start of POLICY Component's Factory Defaults */

#define FD_POLICY_DEFAULT_FLOW_CONTROL_MODE           L7_DISABLE
#define FD_POLICY_DEFAULT_BCAST_STORM_MODE            L7_DISABLE
#define FD_POLICY_DEFAULT_MCAST_STORM_MODE            L7_DISABLE
#define FD_POLICY_DEFAULT_UCAST_STORM_MODE            L7_DISABLE
#define FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD       5
#define FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD       5
#define FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD       5
#define FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD_UNIT  L7_RATE_UNIT_PERCENT
#define FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD_UNIT  L7_RATE_UNIT_PERCENT
#define FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD_UNIT  L7_RATE_UNIT_PERCENT

/* End of POLICY Component's Factory Defaults */

/* Start of DOT3AD Component's Factory Defaults */

#define FD_DOT3AD_NAME                    ""
#define FD_DOT3AD_MEMBER_UNIT             0
#define FD_DOT3AD_ADMIN_MODE              L7_ENABLE

#if L7_FEAT_LAG_PRECREATE
#define FD_DOT3AD_LINK_TRAP_MODE          L7_DISABLE
#define FD_DOT3AD_STATIC_MODE             L7_TRUE
#else
#define FD_DOT3AD_LINK_TRAP_MODE          L7_ENABLE
#define FD_DOT3AD_STATIC_MODE             L7_FALSE
#endif

#define FD_DOT3AD_DEFAULT_LACP_MAXDELAY   0
#define FD_DOT3AD_DEFAULT_PRIORITY        0x8000
#define FD_DOT3AD_DEFAULT_PORT_PRIORITY   0x80
#define FD_DOT3AD_DEFAULT_LACP_ENABLED    L7_TRUE

/* Valid only for lag pre create feature*/
#define FD_DOT3AD_DEFAULT_LAG_NAME        "ch"


#define FD_DOT3AD_ACTOR_DEFAULT_SYS_PRIORITY    32768
#define FD_DOT3AD_ACTOR_DEFAULT_PORT_PRIORITY    128

#define FD_DOT3AD_PARTNER_DEFAULT_ADMINKEY    0
#define FD_DOT3AD_PARTNER_DEFAULT_PORTID    0
#define FD_DOT3AD_PARTNER_DEFAULT_PORT_PRIORITY    0
#define FD_DOT3AD_PARTNER_DEFAULT_SYS_PRIORITY    0x00
/* Activity and aggregation bit are set while timeout bit is not set indicating long timeout*/
#define FD_DOT3AD_ACTOR_ADMIN_PORT_STATE          0x5
#define FD_DOT3AD_PARTNER_ADMIN_PORT_STATE    0x5


/* End of DOT3AD Component's Factory Defaults */

/* Start of IGMP Snooping Component's Factory Defaults */

#define FD_IGMP_SNOOPING_ADMIN_MODE                L7_DISABLE
#define FD_IGMP_SNOOPING_INTF_MODE                 L7_DISABLE
#define FD_IGMP_SNOOPING_MCASTRTR_STATUS           L7_DISABLE
#define FD_IGMP_SNOOPING_INTF_FAST_LEAVE_ADMIN_MODE  L7_DISABLE
#define FD_IGMP_SNOOPING_SRC_ADDR_PROXY            L7_ENABLE
#define FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL 260  /* seconds */
#define FD_IGMP_SNOOPING_MAX_RESPONSE_TIME         10   /* seconds */
#define FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME     0    /* seconds...Infinite timeout */

#define FD_IGMP_SNOOPING_QUERIER_ADMIN_MODE         L7_DISABLE
#define FD_IGMP_SNOOPING_QUERIER_VLAN_MODE          L7_DISABLE
#define FD_IGMP_SNOOPING_QUERIER_VLAN_ELECTION_MODE L7_DISABLE
#define FD_IGMP_SNOOPING_QUERIER_VERSION            2
#define FD_IGMP_SNOOPING_QUERIER_QUERY_INTERVAL     60
#define FD_IGMP_SNOOPING_QUERIER_EXPIRY_INTERVAL    60
#define FD_IGMP_SNOOPING_ROUTER_ALERT_CHECK         L7_FALSE

/* End of IGMP Snooping Component's Factory Defaults */

/* Start of MLD Snooping Component's Factory Defaults */

#define FD_MLD_SNOOPING_ADMIN_MODE                L7_DISABLE
#define FD_MLD_SNOOPING_INTF_MODE                 L7_DISABLE
#define FD_MLD_SNOOPING_MCASTRTR_STATUS           L7_DISABLE
#define FD_MLD_SNOOPING_INTF_FAST_LEAVE_ADMIN_MODE  L7_DISABLE
#define FD_MLD_SNOOPING_SRC_ADDR_PROXY            L7_ENABLE
#define FD_MLD_SNOOPING_GROUP_MEMBERSHIP_INTERVAL 260  /* seconds */
#define FD_MLD_SNOOPING_MAX_RESPONSE_TIME         10   /* seconds */
#define FD_MLD_SNOOPING_MCAST_RTR_EXPIRY_TIME     0    /* seconds...Infinite timeout */

#define FD_MLD_SNOOPING_QUERIER_ADMIN_MODE         L7_DISABLE
#define FD_MLD_SNOOPING_QUERIER_VLAN_MODE          L7_DISABLE
#define FD_MLD_SNOOPING_QUERIER_VLAN_ELECTION_MODE L7_DISABLE
#define FD_MLD_SNOOPING_QUERIER_VERSION            1
#define FD_MLD_SNOOPING_QUERIER_QUERY_INTERVAL     60
#define FD_MLD_SNOOPING_QUERIER_EXPIRY_INTERVAL    60

/* Start of Port MAC Locking Component's Factory Defaults */

#define FD_PML_ADMIN_MODE                L7_DISABLE
#define FD_PML_INTF_LOCK_MODE            L7_DISABLE
#define FD_PML_INTF_VIOLATION_TRAP_MODE  L7_DISABLE
#define FD_PML_INTF_VIOLATION_TRAP_SECS  30

/* End of Port MAC Locking Component's Factory Defaults */

/* Start of DHCP Filtering Component's Factory Defaults */

#define FD_DHCP_FILTERING_INTF_TRUSTED    L7_DISABLE
#define FD_DHCP_FILTERING_ADMIN_MODE      L7_DISABLE

/* End of DHCP Filtering Component's Factory Defaults */

/* DHCP Client VENDOR OPTIONS */
#define FD_DHCP_CLIENT_VENDOR_CLASS_OPTION_MODE     L7_DISABLE
#define FD_DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING   ""

/* DHCP Snooping */
#define FD_DHCP_SNOOP_ADMIN_MODE   L7_DISABLE
#define FD_DHCP_SNOOP_VERIFY_MAC   L7_TRUE
#define FD_DHCP_SNOOP_VLAN         L7_DISABLE
#define FD_DHCP_SNOOP_TRUST        L7_FALSE
#define FD_DHCP_SNOOP_LOG_INVALID_MSG L7_FALSE
#define FD_DHCP_SNOOPING_RATE_LIMIT          -1 /* rate-limit disabled */
#define FD_DHCP_SNOOPING_BURST_INTERVAL       1 /* 1 second */

/* DHCP L2 Relay */
#define FD_DHCP_L2RELAY_ADMIN_MODE           L7_DISABLE
#define FD_DHCP_L2RELAY_INTERFACE_MODE       L7_DISABLE
#define FD_DHCP_L2RELAY_INTERFACE_TRUST_MODE L7_FALSE
#define FD_DHCP_L2RELAY_VLAN_MODE            L7_DISABLE
#define FD_DHCP_L2RELAY_CIRCUIT_ID_VLAN_MODE L7_DISABLE
#define FD_DHCP_L2RELAY_REMOTE_ID_VLAN_MODE  L7_EOS


#define FD_DHCP_L2RELAY_SUBSCRIPTION_MODE            L7_DISABLE
#define FD_DHCP_L2RELAY_CIRCUIT_ID_SUBSCRIPTION_MODE L7_DISABLE
#define FD_DHCP_L2RELAY_REMOTE_ID_SUBSCRIPTION_MODE  ""

#define FD_DS_MAX_REMOTE_ID_STRING    64 /* PTin modified: DHCP snooping */ // 33 /* Actually 33, but one char is reserved for EOS('\0').*/
/* Dynamic ARP Inspection */
#define FD_DAI_VERIFY_SOURCE_MAC   L7_DISABLE
#define FD_DAI_VERIFY_DEST_MAC     L7_DISABLE
#define FD_DAI_VERIFY_IP           L7_DISABLE
#define FD_DAI_RATE_LIMIT          15 /* packets per second */
#define FD_DAI_BURST_INTERVAL       1 /* 1 second */
#define FD_DAI_VLAN                L7_DISABLE
#define FD_DAI_LOG_INVALID         L7_ENABLE
#define FD_DAI_TRUST               L7_DISABLE

/* IP Source Guard */
#define FD_IPSG_VERIFY_IP  L7_FALSE
#define FD_IPSG_VERIFY_MAC L7_FALSE

/* PFC defaults */
#define FD_PFC_DEFAULT_MODE   (PFC_MODE_DISABLE)
#define FD_PFC_NODROP_PRI_BMP (0)

/* Start of QOS ACL Component's Factory Defaults */

#define FD_QOS_ACL_ADMIN_MODE             L7_DISABLE

/* Defining minimal factory defaults for platforms that do not support ACLs.
 * A platform that supports ACLs indicates its upper limits in platform.h,
 * which overrides these defaults.
 */
#define FD_QOS_ACL_MAX_LISTS                1
#define FD_QOS_ACL_MAX_RULES_PER_LIST       1

/* Default for platforms that do not support ACL rule logging */
#define FD_QOS_ACL_LOG_RULE_LIMIT           0

/* Default ACL logging report interval */
#define FD_QOS_ACL_LOGGING_INTERVAL         5   /* minutes */
#define FD_QOS_ACL_TASK_SLEEP_SECS          10  /* seconds */

/* End of QOS ACL Component's Factory Defaults */

/* Start of QOS COS Component's Factory Defaults */

/* Precedence to COS queue mapping table initializer
 *  - a separate initializer is defined per the number of supported COS queues
 *  - value arrangement same as 802.1p specification
 *  - IP Precedence uses 1:1 mapping
 *  - IP DSCP uses 8:1 mapping (every 8 DSCP values mapped to a different COS)
 */
#define FD_QOS_COS_MAP_TABLE_INIT_1             {0,0,0,0,0,0,0,0} /* 1 queue  */
#define FD_QOS_COS_MAP_TABLE_INIT_2             {0,0,0,0,1,1,1,1} /* 2 queues */
#define FD_QOS_COS_MAP_TABLE_INIT_3             {0,0,0,0,1,1,2,2} /* 3 queues */
#define FD_QOS_COS_MAP_TABLE_INIT_4             {1,0,0,1,2,2,3,3} /* 4 queues */
#define FD_QOS_COS_MAP_TABLE_INIT_5             {1,0,0,1,2,3,4,4} /* 5 queues */
#define FD_QOS_COS_MAP_TABLE_INIT_6             {1,0,0,2,3,4,5,5} /* 6 queues */
#define FD_QOS_COS_MAP_TABLE_INIT_7             {1,0,0,2,3,4,5,6} /* 7 queues */
#define FD_QOS_COS_MAP_TABLE_INIT_8             {2,0,1,3,4,5,6,7} /* 8 queues */

#define FD_QOS_COS_MAP_TABLE_DSCP_MAPPING_RATIO 8  /* 8:1 mapping DSCP to COS */

/* the following two defines set a presumed upper limit for config data arrays*/
#define FD_QOS_COS_MAX_QUEUES_PER_INTF          8     /* platform independent */
#define FD_QOS_COS_MAX_DROP_PREC_PER_QUEUE      3     /* platform independent */

/* defaults for various COS config parms */
#define FD_QOS_COS_MSG_LVL                      0     /* off */
#define FD_QOS_COS_MAP_INTF_TRUST_MODE          L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P
#define FD_QOS_COS_QCFG_INTF_SHAPING_RATE       0     /* unlimited */
#define FD_QOS_COS_QCFG_MIN_BANDWIDTH           0     /* no guarantee */
#define FD_QOS_COS_QCFG_MAX_BANDWIDTH           0     /* unlimited */
#define FD_QOS_COS_QCFG_SCHED_TYPE              L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED
#define FD_QOS_COS_QCFG_MGMT_TYPE               L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP
#define FD_QOS_COS_QCFG_WRED_DECAY_EXP          9
#define FD_QOS_COS_QCFG_WRED_DROP_PROB          10

/* tail drop thresholds for up to 8 queues, 3 drop precedence levels each */
#define FD_QOS_COS_QCFG_TDROP_THRESH_Q0_DP1     100
#define FD_QOS_COS_QCFG_TDROP_THRESH_Q0_DP2     90
#define FD_QOS_COS_QCFG_TDROP_THRESH_Q0_DP3     80
#define FD_QOS_COS_QCFG_TDROP_THRESH_Q0_NONTCP  100

/* WRED minimum thresholds for up to 8 queues, 3 drop precedence levels each */
#define FD_QOS_COS_QCFG_WRED_MIN_THRESH_Q0_DP1  40
#define FD_QOS_COS_QCFG_WRED_MIN_THRESH_Q0_DP2  30
#define FD_QOS_COS_QCFG_WRED_MIN_THRESH_Q0_DP3  20
#define FD_QOS_COS_QCFG_WRED_MIN_THRESH_Q0_NONTCP 100

/* WRED maximum thresholds for up to 8 queues, 3 drop precedence levels each */
#define FD_QOS_COS_QCFG_WRED_MAX_THRESH_Q0_DP1  100
#define FD_QOS_COS_QCFG_WRED_MAX_THRESH_Q0_DP2  90
#define FD_QOS_COS_QCFG_WRED_MAX_THRESH_Q0_DP3  80
#define FD_QOS_COS_QCFG_WRED_MAX_THRESH_Q0_NONTCP 100

/* End of QOS COS Component's Factory Defaults */

/* Start of QOS DiffServ Component's Factory Defaults */

#define FD_QOS_DIFFSERV_ADMIN_MODE                  L7_ENABLE
#define FD_QOS_DIFFSERV_TRACE_MODE                  L7_ENABLE
#define FD_QOS_DIFFSERV_MSG_LVL                     0 /* off */
#define FD_QOS_DIFFSERV_STORAGE_TYPE                3 /* non-volatile(3)*/
#define FD_QOS_DIFFSERV_DEFAULTCLASS_NAME           "default"
#define FD_QOS_DIFFSERV_EXPEDITE_CBURST             4 /* Kbytes */
#define FD_QOS_DIFFSERV_POLICE_CONFORM_ACT          6 /* send(6) */
#define FD_QOS_DIFFSERV_POLICE_EXCEED_ACT           1 /* drop(1) */
#define FD_QOS_DIFFSERV_POLICE_NONCONFORM_ACT       1 /* drop(1) */
#define FD_QOS_DIFFSERV_POLICE_COLOR_CONFORM_INDEX  0 /* none */
#define FD_QOS_DIFFSERV_POLICE_COLOR_CONFORM_MODE   1 /* blind(1) */
#define FD_QOS_DIFFSERV_POLICE_COLOR_EXCEED_INDEX   0 /* none */
#define FD_QOS_DIFFSERV_POLICE_COLOR_EXCEED_MODE    1 /* blind(1) */
#define FD_QOS_DIFFSERV_RANDOMDROP_SAMPLING_RATE    0
#define FD_QOS_DIFFSERV_RANDOMDROP_DECAY_EXPONENT   9

/* End of QoS DiffServ Component's Factory Defaults */

/* Start of QOS iSCSI Component's Factory Defaults */
#define FD_QOS_ISCSI_ADMIN_MODE                     L7_DISABLE
#define FD_QOS_ISCSI_SESSION_TIME_OUT_INTERVAL      600  /* seconds */
#define FD_QOS_ISCSI_TAG_SELECTOR                   L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P
#define FD_QOS_ISCSI_VPT_VALUE                      5
#define FD_QOS_ISCSI_DSCP_VALUE                     46
#define FD_QOS_ISCSI_PREC_VALUE                     7
#define FD_QOS_ISCSI_MARKING_ENABLED                L7_FALSE
#define FD_QOS_ISCSI_TARGET_TABLE_CONTENTS          {{L7_TRUE,860,0,"",L7_TRUE,L7_FALSE}, \
                                                     {L7_TRUE,3260,0,"",L7_TRUE}}
/* End of QoS iSCSI Component's Factory Defaults */

/* Start of MFDB Component's Factory Defaults */

/* If the value of this constant is modified,
   L7_MFDB_PROTOCOL_TYPE_t has to be accordingly
   updated in commdefs.h
 */
#define L7_MFDB_MAX_USERS   4

/* End of MFDB Component's Factory Defaults */

/* Start of RMON specific define statements                                   */
#define L7_RMON_ETHER_STAT_ENTRY_MAX_NUM                L7_MAX_RMON_INTERFACE_COUNT
#define L7_RMON_HISTORY_CONTROL_ENTRY_MAX_NUM           (L7_RMON_ENTRY_NUM     \
                                                        * L7_MAX_RMON_INTERFACE_COUNT)
#define L7_RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED      10
#define L7_RMON_ALARM_ENTRY_MAX_NUM                     (L7_RMON_ENTRY_NUM     \
                                                        * L7_MAX_RMON_INTERFACE_COUNT)
#define L7_RMON_EVENT_ENTRY_MAX_NUM                     (L7_RMON_ENTRY_NUM     \
                                                        * L7_MAX_RMON_INTERFACE_COUNT)
#define L7_RMON_LOG_QUEUE_INDEX_MAX_NUM                 10

#ifdef L7_PRODUCT_SMARTPATH
#define FD_RMON_ALARM_VARIABLE_NAME  "1.3.6.1.2.1.16.3.1.1.3.1"
#else
#define FD_RMON_ALARM_VARIABLE_NAME  "alarmInterval.1"
#endif

#define FD_RMON_ALARM_OWNER_NAME     "monitorAlarm"

#define FD_RMON_EVENT_DESCRIPTION    "alarmEvent"
#define FD_RMON_EVENT_COMMUNITY      "public"
#define FD_RMON_EVENT_OWNER          "monitorEvent"

#define FD_RMON_HISTORY_CTRL_OWNER   "monitorHistoryControl"

#define FD_RMON_STATS_OWNER          "monitorEtherStats"

/* End of RMON specific define statements                                     */

#define FD_ASF_DEFAULT_MODE L7_DISABLE

#ifndef SERVICE_PORT_PRESENT
  #define SERVICE_PORT_PRESENT 1
#endif

/*****************************************
 * Sysnet PDU Intercept Factory Defaults *
 *****************************************/

/* Maximum number of address families supported by SYSNET PDU hooks.  This value would
** need to be increased if more address family hooks are supported.
*/
#define FD_SYSNET_PDU_MAX_AF    5

/* AF = AF_INET, Hook ID = SYSNET_INET_RECV_IN */
#define FD_SYSNET_HOOK_CP_WIO_IN_PRECEDENCE        L7_SYSNET_HOOK_PRECEDENCE_1
#define FD_SYSNET_HOOK_ISCSI_PRECEDENCE            L7_SYSNET_HOOK_PRECEDENCE_2
#define FD_SYSNET_HOOK_IGMP_SNOOPING_PRECEDENCE    L7_SYSNET_HOOK_PRECEDENCE_3
#define FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE    L7_SYSNET_HOOK_PRECEDENCE_4
#define FD_SYSNET_HOOK_VOIP_PRECEDENCE             L7_SYSNET_HOOK_PRECEDENCE_5
#define FD_SYSNET_HOOK_VOICE_VLAN_PRECEDENCE       L7_SYSNET_HOOK_PRECEDENCE_6
//#define FD_SYSNET_HOOK_PPPOE_SNOOPING_PRECEDENCE   L7_SYSNET_HOOK_PRECEDENCE_7

/* AF = AF_INET, Hook ID = SYSNET_INET_RECV_ARP_IN */
#define FD_SYSNET_HOOK_DAI_ARP_IN_PRECEDENCE       L7_SYSNET_HOOK_PRECEDENCE_1
#define FD_SYSNET_HOOK_CP_WIO_ARP_IN_PRECEDENCE    L7_SYSNET_HOOK_PRECEDENCE_2
#define FD_SYSNET_HOOK_PTIN_ARP_IN_PRECEDENCE      L7_SYSNET_HOOK_PRECEDENCE_3  /* PTin added: ARP processor */

/* AF = AF_INET, Hook ID = SYSNET_INET_IN */
#define FD_SYSNET_HOOK_VRRP_FILTER_PRECEDENCE      L7_SYSNET_HOOK_PRECEDENCE_1
#define FD_SYSNET_HOOK_WTNNL_INET_IN_PRECEDENCE    L7_SYSNET_HOOK_PRECEDENCE_2
#define FD_SYSNET_HOOK_MCAST_PRECEDENCE            L7_SYSNET_HOOK_PRECEDENCE_3
#define FD_SYSNET_HOOK_UDP_RELAY_PRECEDENCE        L7_SYSNET_HOOK_PRECEDENCE_5

/* AF = AF_INET, Hook ID = SYSNET_INET_VALID_IN */
#define FD_SYSNET_HOOK_RTR_DISC_PRECEDENCE         L7_SYSNET_HOOK_PRECEDENCE_3
#define FD_SYSNET_HOOK_VRRP_ADV_PRECEDENCE         L7_SYSNET_HOOK_PRECEDENCE_5

/* AF = AF_INET, Hook ID = SYSNET_INET_MCAST_IN */
#define FD_SYSNET_HOOK_OSPF_FILTER_PRECEDENCE      L7_SYSNET_HOOK_PRECEDENCE_2
#define FD_SYSNET_HOOK_IGMP_PRECEDENCE             L7_SYSNET_HOOK_PRECEDENCE_3

/* AF = AF_INET, Hook ID = SYSNET_INET_LOCAL_IN */
#define FD_SYSNET_HOOK_RIP_FILTER_PRECEDENCE       L7_SYSNET_HOOK_PRECEDENCE_1
#define FD_SYSNET_HOOK_DHCPS_FILTER_PRECEDENCE     L7_SYSNET_HOOK_PRECEDENCE_2

/* AF = AF_INET, Hook ID = SYSNET_INET_POSTCAP_OUT */
#define FD_SYSNET_HOOK_CP_WIO_OUT_PRECEDENCE        L7_SYSNET_HOOK_PRECEDENCE_1
#define FD_SYSNET_HOOK_WTNNL_OUT_PRECEDENCE         L7_SYSNET_HOOK_PRECEDENCE_3

/* AF = AF_INET, Hook ID = SYSNET_INET_ARP_IN */
#define FD_SYSNET_HOOK_WTNNL_ARP_IN_PRECEDENCE      L7_SYSNET_HOOK_PRECEDENCE_2

/* AF = AF_INET6, Hook ID = SYSNET_INET_MCAST_IN */
#define FD_SYSNET_HOOK_MFC_FILTER_PRECEDENCE      L7_SYSNET_HOOK_PRECEDENCE_0

#define FD_SYSNET_HOOK_MGMT_ACAL_FILTER_PRECEDENCE  L7_SYSNET_HOOK_PRECEDENCE_2

/* Start of DOT1S Component's Factory Defaults */

#define FD_DOT1S_PORT_PRIORITY         128
#define FD_DOT1S_LAG_PORT_PRIORITY     96
#define FD_DOT1S_BRIDGE_PRIORITY       0x8000
#define FD_DOT1S_BRIDGE_FWD_DELAY      15
#define FD_DOT1S_BRIDGE_HELLO_TIME     2
#define FD_DOT1S_BRIDGE_MAX_AGE        20
#define FD_DOT1S_BRIDGE_MAX_HOP        20
#define FD_DOT1S_BRIDGE_TX_HOLD_COUNT  6
#define FD_DOT1S_MODE                  L7_DOT1S_MODE
#define FD_DOT1S_PORT_MODE             FD_DOT1S_MODE
#define FD_DOT1S_FORCE_VERSION         DOT1S_FORCE_VERSION_DOT1S
#define FD_DOT1S_PORT_PATH_COST        L7_DOT1S_AUTO_CALC_PATH_COST
#define FD_DOT1S_PORT_PATH_COST_100FX  FD_DOT1S_PORT_PATH_COST
#define FD_DOT1S_PORT_PATH_COST_1000SX 20000
#define FD_DOT1S_DEFAULT_CONFIG_NAME   ""
#define FD_DOT1S_DEFAULT_CONFIG_REVISION   0
#define FD_DOT1S_DEFAULT_ADMIN_EDGE    L7_FALSE
#define FD_DOT1S_AUTO_EDGE    L7_TRUE
#define FD_DOT1S_ROOTGUARD    L7_FALSE
#define FD_DOT1S_LOOPGUARD    L7_FALSE
#define FD_DOT1S_TCNGUARD     L7_FALSE
#define FD_DOT1S_BPDUFILTER   L7_FALSE
#define FD_DOT1S_BPDUFLOOD    L7_DOT1S_BPDUFLOOD_ACTION_AUTO  

/* End of DOT1S Component's Factory Defaults */

/* Start of RADIUS Client Component's Factory Defaults */

#define FD_RADIUS_AUTH_OPER_MODE        L7_ENABLE
#define FD_RADIUS_ACCT_ADMIN_MODE       L7_DISABLE
#define FD_RADIUS_AUTH_PORT             1812
#define FD_RADIUS_ACCT_PORT             1813
#define FD_RADIUS_MAX_RETRANS           L7_RADIUS_DEFAULT_RETRANS
#define FD_RADIUS_TIMEOUT_DURATION      L7_RADIUS_DEFAULT_TIMEOUTDURATION
#define FD_RADIUS_RETRY_PRIMARY_TIME    L7_RADIUS_DEFAULT_RETRYPRIMARYTIME
#define FD_RADIUS_SOCKET_INACTIVE_TIME  180 /*seconds*/
#ifdef  L7_PRODUCT_SMARTPATH
#define FD_RADIUS_MAX_AUTH_SERVERS      3
#define FD_RADIUS_MAX_ACCT_SERVERS      1
#else
#define FD_RADIUS_MAX_AUTH_SERVERS      32
#define FD_RADIUS_MAX_ACCT_SERVERS      32
#endif
#define FD_RADIUS_SERVER_ENTRY_TYPE     L7_RADIUS_SERVER_ENTRY_TYPE_SECONDARY
#define FD_RADIUS_INC_MSG_AUTH_MODE     L7_ENABLE
#define FD_RADIUS_SERVER_DEFAULT_SECRET_CONFIG     L7_FALSE
#define FD_RADIUS_NAS_IP_MODE           L7_FALSE
#define FD_RADIUS_DEADTIME              L7_RADIUS_DEFAULT_DEADTIME
#define FD_RADIUS_PRIORITY              L7_RADIUS_DEFAULT_PRIORITY
#define FD_RADIUS_USAGE                 L7_RADIUS_SERVER_USAGE_ALL

/* End of RADIUS Client Component's Factory Defaults */

/* Start of TACACS+ Client Component's Factory Defaults */
#define FD_TACACS_PLUS_TIMEOUT_VALUE      5
#define FD_TACACS_PLUS_SRC_IP_ADDR        0
#define FD_TACACS_PLUS_PORT_NUM           49
#define FD_TACACS_PLUS_PRIORITY           0
#define FD_TACACS_PLUS_SINGLE_CONNECTION  L7_FALSE /* default is connection per session */
/* End of TACACS+ Client Component's Factory Defaults */

/* Start of DOT1X Component's Factory Defaults */
#define FD_DOT1X_ADMIN_MODE                       L7_DISABLE
#define FD_DOT1X_PORT_MODE                        L7_DOT1X_PORT_AUTO
#define FD_DOT1X_LOG_TRACE_MODE                L7_DISABLE
#define FD_DOT1X_VLAN_ASSIGN_MODE              L7_DISABLE
#define FD_DOT1X_DYNAMIC_VLAN_CREATION_MODE    L7_DISABLE
#define FD_DOT1X_PORT_PAE_CAPABILITIES         L7_DOT1X_PAE_PORT_AUTH_CAPABLE
#define FD_DOT1X_PORT_DIRECTION                   L7_DOT1X_PORT_DIRECTION_BOTH
#define FD_DOT1X_PORT_QUIET_PERIOD                60 /* seconds */
#define FD_DOT1X_PORT_TX_PERIOD                   30 /* seconds */
#define FD_DOT1X_PORT_SUPP_TIMEOUT                30 /* seconds */
#define FD_DOT1X_PORT_SERVER_TIMEOUT              30 /* seconds */
#define FD_DOT1X_PORT_MAX_REQ                     2
#define FD_DOT1X_PORT_GUEST_VLAN                     0
#define FD_DOT1X_PORT_REAUTH_PERIOD               3600 /* seconds */
#define FD_DOT1X_PORT_REAUTH_ENABLED              L7_FALSE
#define FD_DOT1X_PORT_KEY_TRANSMISSION_ENABLED    L7_FALSE
#define FD_DOT1X_PORT_GUEST_PERIOD                90
#define FD_DOT1X_PORT_MAX_USERS                   16
#define FD_DOT1X_PORT_UNAUTHENTICATED_VLAN        0
#define FD_DOT1X_PORT_MAB_ENABLED                 L7_DISABLE
#define FD_DOT1X_PASS_TEXT                        " "
#define FD_DOT1X_MONITOR_MODE                     L7_DISABLE

/* Start of DOT1x Supplicant's Factory Defaults */
#define FD_DOT1X_SUPPLICANT_PORT_MODE           L7_DOT1X_PORT_AUTO
#define FD_DOT1X_SUPPLICANT_PORT_MAX_START      3
#define FD_DOT1X_SUPPLICANT_PORT_START_PERIOD   30 /*Seconds*/
#define FD_DOT1X_SUPPLICANT_PORT_HELD_PERIOD    60 /*Seconds*/
#define FD_DOT1X_SUPPLICANT_PORT_AUTH_PERIOD    30 /*Seconds*/

/* End of DOT1X Component's Factory Defaults */
/* Start of Voice Vlan Component's Factory Defaults */

#define  FD_VOICE_VLAN_ADMIN_MODE   L7_DISABLE
#define  FD_VOICE_VLAN_TRACE_MODE   L7_DISABLE
#define  FD_VOICE_VLAN_VLAN_ID      0
#define  FD_VOICE_VLAN_PRIORITY     255
#define  FD_VOICE_VLAN_TAGGING      L7_FALSE
#define  FD_VOICE_VLAN_OVERRIDE     L7_FALSE
#define  FD_VOICE_VLAN_NONE_MODE    L7_FALSE
#define  FD_VOICE_VLAN_AUTH_STATE   L7_ENABLE
#define  FD_VOICE_VLAN_DSCP         0

/* End of Voice Vlan Component's Factory Defaults */

/* Start of dvlantag Component's Factory Defaults */
#define FD_DVLANTAG_MAX_ETHERTYPE           0xFFFF
#define FD_DVLANTAG_MIN_ETHERTYPE           0x0001
#define FD_DVLANTAG_VMAN_ETHERTYPE          0x88A8
#define FD_DVLANTAG_802_1Q_ETHERTYPE        0x8100
#define FD_DVLANTAG_DEFAULT_ETHERTYPE       platDVLANTagDefaultEthTypeGet()
#define FD_DVLANTAG_PER_PORT_TPID_MASK      0x0001
#define FD_DVLANTAG_DEFAULT_CUSTID          1
#define FD_DVLANTAG_MAX_CUSTID              4095
#define FD_DVLANTAG_MIN_CUSTID              1
#define FD_DVLANTAG_DEFAULT_PORT_MODE       L7_DISABLE
#define FD_DVLANTAG_DEFAULT_MODE            L7_DOT1Q_UNTAGGED
/* End of dvlantag Component's Factory Defaults */

/* Start of DHCP Server Component's Factory Defaults */

/* dhcpsCfgData_t default values */
#define FD_DHCPS_DEFAULT_ADMIN_MODE                     L7_DISABLE
#define FD_DHCPS_DEFAULT_PINGPKTNO                      2
#define FD_DHCPS_DEFAULT_CONFLICTLOGGING                L7_TRUE
#define FD_DHCPS_DEFAULT_BOOTPAUTO                      L7_FALSE

/*  poolCfgData_t default values */
#define FD_DHCPS_DEFAULT_POOLCFG_LEASETIME              1440 /* 1 DAY */
#define FD_DHCPS_DEFAULT_POOLCFG_TYPEOFBINDING          INACTIVE_POOL
#define FD_DHCPS_DEFAULT_NETWORKIP_ADDR                 L7_NULL_IP_ADDR
#define FD_DHCPS_DEFAULT_DNS_SERVER                     L7_NULL_IP_ADDR
#define FD_DHCPS_DEFAULT_ROUTER                         L7_NULL_IP_ADDR
#define FD_DHCPS_DEFAULT_CLIENT_NAME                    ""
#define FD_DHCPS_DEFAULT_CLIENT_ID                      ""
#define FD_DHCPS_DEFAULT_HOST_ADDRESS                   L7_NULL_IP_ADDR
#define FD_DHCPS_DEFAULT_HARDWARE_ADDRESS               ""
#define FD_DHCPS_DEFAULT_HARDWARE_TYPE                  L7_DHCPS_ETHERNET
#define FD_DHCPS_DEFAULT_DOMAIN_NAME                    ""
#define FD_DHCPS_DEFAULT_BOOTFILE_NAME                  ""
#define FD_DHCPS_DEFAULT_NEXTSERVERIP_ADDR              L7_NULL_IP_ADDR
#define FD_DHCPS_DEFAULT_NBNAMESERVERIP_ADDR            L7_NULL_IP_ADDR
#define FD_DHCPS_DEFAULT_NBNODETYPE                     0

#if L7_FEAT_BANNER_MOTD
/* Start of CLI Banner Component's Factory Defaults */
#define FD_CLI_BANNER_DEFAULT_STATE                     L7_TRUE
#define FD_CLI_BANNER_DEFAULT_MESSAGE                   ""
#endif/*L7_FEAT_BANNER_MOTD*/

/* Start of LOG Server Component's Factory Defaults */

        /*--- End user configuration values--- */

/* Default console admin status (locked means no messages emitted) */
#define FD_LOG_DEFAULT_ALL_ADMIN_STATUS           L7_ADMIN_MODE_ENABLE
#define FD_LOG_DEFAULT_GLOBAL_ADMIN_STATUS        L7_ADMIN_MODE_ENABLE
#define FD_LOG_DEFAULT_CONSOLE_ADMIN_STATUS       L7_ADMIN_MODE_ENABLE
/* Default console component filter (all means all component messages) */
#define FD_LOG_DEFAULT_CONSOLE_COMPONENT_FILTER   L7_ALL_COMPONENTS
/* Defaults console severity filter */
#define FD_LOG_DEFAULT_CONSOLE_SEVERITY_FILTER    L7_LOG_SEVERITY_ERROR

/* Default in memory admin status (locked means no messages emitted) */
#define FD_LOG_DEFAULT_IN_MEMORY_ADMIN_STATUS     L7_ADMIN_MODE_ENABLE
/* Default in memory behvior */
#define FD_LOG_DEFAULT_IN_MEMORY_BEHAVIOR         L7_LOG_WRAP_BEHAVIOR
/* Default in memory component filter (all mean all component messages are logged) */
#define FD_LOG_DEFAULT_IN_MEMORY_COMPONENT_FILTER L7_ALL_COMPONENTS
/* Defaults in memory severity filter */
#define FD_LOG_DEFAULT_IN_MEMORY_SEVERITY_FILTER  L7_LOG_SEVERITY_INFO

/* Persistent log defaults. */
#define FD_LOG_DEFAULT_PERSISTENT_ADMIN_STATUS    L7_ADMIN_MODE_DISABLE
#define FD_LOG_DEFAULT_PERSISTENT_SEVERITY_FILTER L7_LOG_SEVERITY_ALERT

#define FD_LOG_DEFAULT_IN_MEMORY_SIZE             200
/* Default in memory admin status (locked means no messages emitted) */
#define FD_LOG_DEFAULT_FACILITY                   L7_LOG_FACILITY_LOCAL7
/* The default description for syslog server */
#define FD_LOG_DEFAULT_SERVER_DESCRIPTION         ""

/* Syslog defaults. */
#define FD_LOG_DEFAULT_SYSLOG_ADMIN_STATUS        L7_ADMIN_MODE_DISABLE
#define FD_LOG_DEFAULT_SYSLOG_SEVERITY_FILTER     L7_LOG_SEVERITY_CRITICAL

/* The port number for the local syslog client */
#define FD_LOG_DEFAULT_CLIENT_PORT                514
/* The port number for the remote syslog collector/relay */
#define FD_LOG_DEFAULT_SERVER_PORT                514

/* The default facility (used when not specified by caller) */
#define FD_LOG_DEFAULT_LOG_FACILITY               L7_LOG_FACILITY_USER

/* Command Logging Defaults */
#define FD_CMD_LOGGER_ADMIN_MODE    L7_DISABLE
#define FD_CMD_LOGGER_SEVERITY      L7_LOG_SEVERITY_NOTICE

/* Web Sessions*/
#define FD_CMD_LOGGER_WEB_ADMIN_MODE    L7_DISABLE
#define FD_CMD_LOGGER_WEB_SEVERITY      L7_LOG_SEVERITY_NOTICE

/* SNMP Set commands */
#define FD_CMD_LOGGER_SNMP_ADMIN_MODE    L7_DISABLE
#define FD_CMD_LOGGER_SNMP_SEVERITY      L7_LOG_SEVERITY_NOTICE

/* Audit parameters */
#define FD_CMD_LOGGER_AUDIT_ADMIN_MODE   L7_DISABLE

          /*--- Porting parameters --- */
/* How many historical persistent logs to keep (includes the current set) */
#define L7_LOG_MAX_PERSISTENT_LOGS                      3
/* Minimum length of a syslog host name */
#define L7_LOG_HOST_MIN_ADDRESS_LEN                     1
/* Maximum length of a syslog host name */
#define L7_LOG_HOST_MAX_ADDRESS_LEN                     64
/* Maximum length of a syslog host description */
#define L7_LOG_HOST_DESCRIPTION_LEN                     64
/* Maximum number of syslog hosts. */
#define L7_LOG_MAX_HOSTS                                8
/* Maximum length of a persistent log message (includes the null terminator) */
/* This should be long enough to hold the syslog information which precedes the message
   plus long enough to hold the message (LOG_MSG_MAX_MSG_SIZE) without truncation */
#define L7_LOG_MESSAGE_LENGTH                   (80 +LOG_MSG_MAX_MSG_SIZE)
/* Maximum length of a buffered log message. (Includes null termination). */
/* These two define the size (number of records) of their respective logs */

#define L7_LOG_IN_MEMORY_LOG_MIN_COUNT                  20
#ifdef L7_PRODUCT_SMARTPATH
#define L7_LOG_IN_MEMORY_LOG_MAX_COUNT                  200
#define L7_LOG_IN_MEMORY_LOG_DEFAULT_COUNT              100
#else
#define L7_LOG_IN_MEMORY_LOG_MAX_COUNT                  400
#define L7_LOG_IN_MEMORY_LOG_DEFAULT_COUNT              200
#endif

#define L7_SYSLOG_MIN_PORT 1
#define L7_SYSLOG_MAX_PORT 65535
#define L7_SYSLOG_MAX_DESCRIPTION_LENGTH 64
#define L7_SYSLOG_MIN_DESCRIPTION_LENGTH 1

#define L7_LOG_PERSISTENT_STARTUP_LOG_COUNT             32
#define L7_LOG_PERSISTENT_OPERATION_LOG_COUNT           32

/* Maximum length of a persistent log message (includes the null terminator) */
/* This should be set equal to L7_LOG_MESSAGE_LENGTH to ensure message is not truncated */
#define L7_LOG_PERSISTENT_MESSAGE_LENGTH                L7_LOG_MESSAGE_LENGTH
/* Maximum length of the persistent log file name (including null termination) */
#define L7_LOG_MAX_FILENAME_LEN                         32
/* Maximum size of the formatted buffer part of a log message
* (not including the fixed format part)
*/
#define FD_LOG_EMAILALERT_DEFAULT_DURATION 30
#define FD_LOG_EMAILALERT_DEFAULT_SEVERITY 2
#define FD_LOG_EMAILALERT_DEFAULT_PORT 465
#define FD_LOG_EMAILALERT_DEFAULT_SECURITY_PROTOCOL L7_LOG_EMAIL_ALERT_NONE
/* These two define the file name templates for the respective persistent logs. */
#define L7_LOG_PERSISTENT_STARTUP_FILE_MASK             "slog%d.txt"
#define L7_LOG_PERSISTENT_OPERATION_FILE_MASK           "olog%d.txt"
#define PERSISTENT_LOG_FILE_NAME "persist.txt"

/* These two define the file name templates for the remote persistent logs. */
#define L7_LOG_REMOTE_PERSISTENT_STARTUP_FILE_MASK      "slog%d%02d.txt"
#define L7_LOG_REMOTE_PERSISTENT_OPERATION_FILE_MASK    "olog%d%02d.txt"
#define PERSISTENT_LOG_FILE_NAME "persist.txt"

#define L7_SCRIPTS_STARTUP_CONFIG_FILENAME "startup-config"
/* End of Log Component's Factory Defaults */

/* Start of IPv6 Provisioning Component's Factory Defaults */

#define FD_IPV6_PROVISIONING_DEFAULT_MODE         L7_DISABLE

/* End of IPv6 Provisioning Component's Factory Defaults */

/* Start of SSLT Component's Factory Defaults */

#define FD_SSLT_ADMIN_MODE           L7_DISABLE
#define FD_SSLT_PEM_PASS_PHRASE      ""         /* No encryption used in PEM files */
#define FD_SSLT_SECURE_PORT          443        /* https */
#define FD_SSLT_UNSECURE_PORT        80         /* httpd */
#define FD_SSLT_UNSECURE_SERVER_ADDR 0x7F000001 /* 127.0.0.1 */
#define FD_SSLT_SSL30_MODE           L7_ENABLE  /* SSL Version 3.0 */
#define FD_SSLT_TLS10_MODE           L7_ENABLE  /* TLS Version 1.0 */

#define FD_SSLT_PEM_DIR              "./"
#define FD_SSLT_ROOT_PEM             "rootcert.pem"
#define FD_SSLT_SERVER_PEM           "server.pem"
#define FD_SSLT_DHWEAK_PEM           "dh512.pem"
#define FD_SSLT_DHSTRONG_PEM         "dh1024.pem"

#define FD_SSLT_CERT_NUMBER_ACTIVE   1
#define FD_SSLT_CERT_VALID_DAYS      365

/* End of SSLT Component's Factory Defaults */

/* Start of security cipher/encryption factory defaults */
#define L7_SECURITY_SUPPORT_DES  1
#define L7_SECURITY_SUPPORT_3DES 1
#define L7_SECURITY_SUPPORT_AES  1
#define L7_SECURITY_SUPPORT_RC4  1
#define L7_SECURITY_SUPPORT_RC2  1
#define L7_SECURITY_SUPPORT_IDEA 0 /* patented */

/* following items apply only to ssl */
#define L7_SECURITY_SUPPORT_SSLV2 0 /* not secure, use is discouraged */
#define L7_SECURITY_SUPPORT_SSLV3 1
#define L7_SECURITY_SUPPORT_TLSV1 1

/* following items apply only to ssh */
#define L7_SECURITY_SUPPORT_BLOWFISH 1
#define L7_SECURITY_SUPPORT_CAST 1


/* End of security cipher/encryption factory defaults */
/* Start of SSHD Component's Factory Defaults */

#define FD_SSHD_ADMIN_MODE           L7_DISABLE
#define FD_SSHD_PROTO_LEVEL_V1_ONLY  0x01
#define FD_SSHD_PROTO_LEVEL_V2_ONLY  0x02
#define FD_SSHD_PROTO_LEVEL          (FD_SSHD_PROTO_LEVEL_V2_ONLY | FD_SSHD_PROTO_LEVEL_V1_ONLY)
#define FD_SSHD_EMWEB_PORT           2222
#define FD_SSHD_UNSECURE_SERVER_ADDR 0x7F000001 /* 127.0.0.1 */
#define FD_SSHD_TIMEOUT              0x5
#define FD_SSHC_EMWEB_PORT           3333
#define FD_SSHD_PORT                 22
#define FD_SSHD_PUBKEY_AUTH_MODE     L7_DISABLE

#ifdef _L7_OS_LINUX_
#define FD_SSHD_PATH_SSH_USER_DIR    "/usr/local/ptin/log/fastpath/"    /* PTin modified: paths */
#endif /* _L7_OS_LINUX_ */
#ifdef _L7_OS_VXWORKS_
#define FD_SSHD_PATH_SSH_USER_DIR    ""
#endif /* _L7_OS_VXWORKS_ */

#define FD_SSHD_NAME_SERVER_PRIVKEY        "ssh_host_key"
#define FD_SSHD_NAME_SERVER_PRIVKEY_FRESSH "sshd.key"
#define FD_SSHD_NAME_SERVER_PRIVKEY_DSA    "ssh_host_dsa_key"
#define FD_SSHD_NAME_SERVER_PRIVKEY_RSA    "ssh_host_rsa_key"

#define FD_SSHD_PATH_SERVER_PRIVKEY        FD_SSHD_PATH_SSH_USER_DIR FD_SSHD_NAME_SERVER_PRIVKEY
#define FD_SSHD_PATH_SERVER_PRIVKEY_FRESSH FD_SSHD_PATH_SSH_USER_DIR FD_SSHD_NAME_SERVER_PRIVKEY_FRESSH
#define FD_SSHD_PATH_SERVER_PRIVKEY_DSA    FD_SSHD_PATH_SSH_USER_DIR FD_SSHD_NAME_SERVER_PRIVKEY_DSA
#define FD_SSHD_PATH_SERVER_PRIVKEY_RSA    FD_SSHD_PATH_SSH_USER_DIR FD_SSHD_NAME_SERVER_PRIVKEY_RSA

/* End of SSHD Component's Factory Defaults */


/* 802.1P Constants */
#define L7_DOT1P_MIN_PRIORITY             0
#define L7_DOT1P_MAX_PRIORITY             7

#define L7_DOT1P_MIN_TRAFFIC_CLASS        0
#define L7_DOT1P_MAX_TRAFFIC_CLASS        (L7_DOT1P_NUM_TRAFFIC_CLASSES-1)

/* file system path */
#ifdef _L7_OS_LINUX_
#define FD_LOGO_IMAGE_FILE_DIR    "/usr/local/ptin/log/fastpath/"       /* PTin modified: paths */
#endif /* _L7_OS_LINUX_ */
#ifdef _L7_OS_VXWORKS_
#define FD_LOGO_IMAGE_FILE_DIR    ""
#endif /* _L7_OS_VXWORKS_ */
/* end file system path */

/* Start of SNTP factory defaults. */

/*
* The following can be one of:
*
*  L7_SNTP_CLIENT_DISABLED
*  L7_SNTP_CLIENT_UNICAST
*  L7_SNTP_CLIENT_BROADCAST
*  L7_SNTP_CLIENT_MULTICAST
*
*  Only broadcast, multicast, or disabled are of much use
*  since default unicast servers are not supported.
*
*  Before writing code to configure a default NTP server,
*  please read one (or all) of the following links:
*
*   http://netsecurity.about.com/b/a/021894.htm
*   http://news.com.com/2100-1002-5068035.html?tag=macintouch
*   http://news.zdnet.co.uk/internet/security/0,39020375,39115911,00.htm
*
* Configuring an IP address as a default NTP server is
* very much frowned upon. It is highly recommended that
* only DNS names be used as default servers, should
* one choose to place a default server in a build.
*
*/

#define FD_SNTP_DEFAULT_CLIENT_MODE L7_SNTP_CLIENT_DISABLED

/* The default local client port to use. */
#define FD_SNTP_DEFAULT_CLIENT_PORT      123

/* The default local client port to use. */
#define FD_SNTP_DEFAULT_UNICAST_CLIENT_PORT 0

/* The default remote server port to use. */
#define FD_SNTP_DEFAULT_SERVER_PORT      123

/* The default NTP version on the polled servers */
#define FD_SNTP_DEFAULT_VERSION           4

/* The default poll interval (2^6 seconds) */
#define FD_SNTP_DEFAULT_POLL_INTERVAL     6
/* The default poll response timeout (5 seconds) */
#define FD_SNTP_DEFAULT_POLL_TIMEOUT      5
/* The default poll retry count */
#define FD_SNTP_DEFAULT_POLL_RETRY        1
/* The default priority for servers */
#define FD_SNTP_DEFAULT_SERVER_PRIORITY   1

/* The maximum number of packets received from
* non-polled hosts before failing a poll
* in unicast mode.
*/

#define FD_SNTP_MAX_SPOOFED_PACKETS       3

/* Dates before this value are silently discarded by the SNTP client. */
/* This is Jan. 2003 expressed as an offset in seconds from the NTP epoch. */
#define FD_SNTP_DISCARD_TIME 3250368000UL

#define FD_SNTP_MAX_AUTH_KEYS       5

/* Maximum number of servers supported. Three is plenty. */
#define FD_SNTP_MAX_SERVERS         3

/* Maximum length of an address string - string is null terminated */
#define FD_SNTP_MAX_ADDRESS_LEN     64

#define FD_SNTP_DEFAULT_TRUSTEDKEY           L7_CLI_ZERO
#define FD_SNTP_DEFAULT_AUTH_MODE            L7_DISABLE

/* The default priority for servers */
#define FD_SNTP_DEFAULT_SERVER_POLL       L7_ENABLE

#define FD_TIMEZONE_HRS             0
#define FD_TIMEZONE_MINS            0
#define FD_TIMEZONE_ACRONYM         ""
/* End of SNTP Factory Defaults */

#define FD_TIMEZONE_SUMMERTIME      L7_NO_SUMMER_TIME


/* Start of Outbound Telnet Component's Factory Defaults */

/* telnetCfgData_t default values */
#define FD_TELNET_DEFAULT_ADMIN_MODE             L7_ENABLE
#define FD_TELNET_DEFAULT_TIMEOUT                30             /* PTin modified: session timeout (5) */

/* End of Outbound Telnet Component's Factory Defaults */

/* Start of Front Panel Stacking Factory Defaults. */
#define FD_FPS_DEFAULT_STACK_PORT_MODE           L7_ENABLE
#define FD_FPS_DEFAULT_QOS_MODE                  L7_ENABLE
/* End of Front Panel Stacking Factory Defaults */

#define FD_NSF_DEFAULT_ADMIN_MODE                L7_ENABLE

/* Start of 802.1AB Component's Factory Defaults */

#define FD_LLDP_PORT_TX_ADMIN_MODE                L7_DISABLE
#define FD_LLDP_PORT_RX_ADMIN_MODE                L7_DISABLE
#define FD_LLDP_PORT_NOTIFICATION_ADMIN_MODE      L7_DISABLE
#define FD_LLDP_PORT_TRANSMIT_MGMT_ADDR_ENABLE    L7_FALSE
#define FD_LLDP_PORT_TRANSMIT_TLVS_ENABLE         0   /* bit mask, no optional TLVs enabled */
#define FD_LLDP_TX_INTERVAL                       30  /* seconds */
#define FD_LLDP_TX_HOLD_MULTIPLIER                4
#define FD_LLDP_REINIT_DELAY                      2   /* seconds */
#define FD_LLDP_NOTIFICATION_INTERVAL             5   /* seconds */

/* End of 802.1AB Component's Factory Defaults */

/* Start of Denial of Service Factory Defaults */

#define FD_DOS_DEFAULT_FIRSTFRAG_MODE  L7_DISABLE
#define FD_DOS_DEFAULT_ICMP_MODE  L7_DISABLE
#define FD_DOS_DEFAULT_ICMPFRAG_MODE L7_DISABLE
#define FD_DOS_DEFAULT_L4PORT_MODE  L7_DISABLE
#define FD_DOS_DEFAULT_TCPPORT_MODE L7_DISABLE
#define FD_DOS_DEFAULT_UDPPORT_MODE L7_DISABLE
#define FD_DOS_DEFAULT_SIPDIP_MODE  L7_DISABLE
#define FD_DOS_DEFAULT_SMACDMAC_MODE L7_DISABLE
#define FD_DOS_DEFAULT_TCPFLAG_MODE  L7_DISABLE
#define FD_DOS_DEFAULT_TCPFLAGSEQ_MODE  L7_DISABLE
#define FD_DOS_DEFAULT_TCPFRAG_MODE  L7_DISABLE
#define FD_DOS_DEFAULT_TCPFINURGPSH_MODE L7_DISABLE
#define FD_DOS_DEFAULT_TCPSYN_MODE L7_DISABLE
#define FD_DOS_DEFAULT_TCPSYNFIN_MODE L7_DISABLE
#define FD_DOS_DEFAULT_TCPOFFSET_MODE L7_DISABLE
#define FD_DOS_DEFAULT_MAXICMP_SIZE  512
#define FD_DOS_DEFAULT_MINTCPHDR_SIZE  20
#define FD_DOS_DEFAULT_PINGFLOODING_MODE    L7_DISABLE
#define FD_DOS_DEFAULT_SMURFATTACK_MODE     L7_DISABLE
#define FD_DOS_DEFAULT_SYNACK_MODE      L7_DISABLE
#define FD_DOS_DEFAULT_PING_PARAM            1000
#define FD_DOS_DEFAULT_SYNACK_PARAM          1000



/* Start of DNS Client Factory Defaults */

#define FD_DNS_CLIENT_ADMIN_MODE_ENABLED           L7_TRUE
#define FD_DNS_CLIENT_QUERY_TIMEOUT                3   /* seconds */
#define FD_DNS_NO_OF_RETRIES                       2
#define FD_DNS_CLIENT_REQUEST_TIMEOUT              ((L7_DNS_NAME_SERVER_ENTRIES)*(FD_DNS_CLIENT_QUERY_TIMEOUT*(FD_DNS_NO_OF_RETRIES+1))) /* 72   seconds  - must accomodate 8 server timeout failovers */

/* End of DNS Client Factory Defaults */


/* End of Denial of Service Factory Defaults */

/* Start of Alarm manager Factory Default */
enum
{
    FD_ALARM_DEF_TABLE_DEFAULT_SEVERITY    =  9999,
};

/* Chassis tables Factory Defaults */
enum
{
    FD_CHASSIS_ALARM_DEF_TABLE_DEFAULT_SEVERITY =  L7_ALARM_SEVERITY_MINOR,
    FD_CHASSIS_ALARM_DEF_TABLE_DEFAULT_LOG_MODE =  L7_ALARM_LOGGING_ENABLED,
    FD_CHASSIS_ALARM_DEF_TABLE_DEFAULT_REPORT_MODE =  L7_ALARM_REPORT_ENABLED
};

/* Start of LLDP-MED Factory Defaults */
#define FD_LLDP_TX_DELAY                    2
#define FD_LLDP_MED_FASTSTART_REPEAT_COUNT  3
#define FD_LLDP_MED_PORT_ADMIN_MODE         L7_DISABLE
#define FD_LLDP_MED_PORT_NOTIFY_MODE        L7_FALSE
#define FD_LLDP_MED_PORT_TVL_TX             0x3
/* The bits for capability and Network policy will be set. Location, ExtendedPSE, extendedPD and
 * Inventory are disabled. With Location and Inventory unsupported.
 */
#define FD_LLDP_MED_LOCAL_DEVICE_CLASS      4 /* Maps to lldpXMedDeviceClass_t networkConnectivity */
/* End of LLDP-MED Factory Defaults */

#define FD_DEFAULT_PASSWORD                 ""
#define CLI_SUPPORTCONSOLE_DEFAULTBREAKIN "3%condbg4&"

/* This is for Password Recovery */
extern L7_RC_t passwdRecoveryFlagSet(L7_uint32 value);
extern L7_RC_t passwdRecoveryFlagGet(L7_int32 *envPtr);

typedef struct envVar_s
{
  L7_uint32 defaultPasswordFlag;
}envVar_t;

#define FD_DEFAULT_PASSWORD               ""

/* VOIP FACTORY DEFAULTS */
#define FD_VOIP_INTF_PROFILE_MODE L7_DISABLE
#define FD_VOIP_INTF_PROFILE_BANDWIDTH 0
#define FD_VOIP_COS_QUEUE L7_MAX_CFG_QUEUES_PER_PORT -1

/* PFC FACTORY DEFAULTS */
#define FD_PFC_INTF_ADMIN_MODE L7_DISABLE
#define FD_PFC_PRIORITY_ACTION L7_PFC_ACTION_DROP

/* sFlow Factory defaults */
#define FD_SFLOW_PROTOCOL_VERSION       5
#define FD_SFLOW_RCVR_TIMEOUT           0
#define FD_SFLOW_MAX_DGRAM_SIZE         1400
#define FD_SFLOW_RCVR_PORT              6343
#define FD_SFLOW_RCVR_ADDR_TYPE         L7_AF_INET
#define FD_SFLOW_INSTANCE               1
#define FD_SFLOW_SAMPLING_RATE          0
#define FD_SFLOW_DEFAULT_HEADER_SIZE    128
#define FD_SFLOW_POLL_INTERVAL          0
#define FD_SFLOW_DEFAULT_IP             "0.0.0.0"
#define FD_SFLOW_SAMPLE_RANDOM_SEED     0x11223344
#define FD_SFLOW_CPU_SAMPLE_PRIORITY    0
#define FD_SFLOW_RCVR_SNMP_MODE         L7_ENABLE
#define FD_SFLOW_OWNER_LENGTH           128 /* 127 characters + 1 null char */
#define FD_SFLOW_DEFAULT_SAMPLING_RATE  400

#define L7_RADIUS_SERVER_NAME_LENGTH  32
#define L7_RADIUS_SERVER_DEFAULT_NAME_AUTH "Default-RADIUS-Server"
#define L7_RADIUS_SERVER_DEFAULT_NAME_ACCT "Default-RADIUS-Server"

/*-------------------------------------*/
/* Start Isdp Global Defaults          */
/*-------------------------------------*/

#define FD_ISDP_DEFAULT_MODE                      L7_DISABLE  /* PTin modified: ISDP off */
#define FD_ISDP_DEFAULT_TIMER                     30
#define FD_ISDP_DEFAULT_HOLDTIME                  180
#define FD_ISDP_DEFAULT_V2MODE                    L7_ENABLE

#define FD_ISDP_DEFAULT_TX_TRACE_MODE             L7_DISABLE
#define FD_ISDP_DEFAULT_RX_TRACE_MODE             L7_DISABLE
#define FD_ISDP_DEFAULT_EVENTS_TRACE_MODE         L7_DISABLE

/*-------------------------------------*/
/* End Isdp Global Defaults            */
/*-------------------------------------*/


/* AUTO-INSTALL FACTORY DEFAULTS */
#define FD_AUTO_INSTALL_START_STOP_DEFAULT           L7_FALSE    /* disable auto-install by default */
#define FD_AUTO_INSTALL_AUTO_SAVE_DEFAULT            L7_FALSE    /* disable auto-save by default */
#define FD_AUTO_INSTALL_UNICAST_RETRY_COUNT_DEFAULT  3
#define AUTO_INSTALL_NETWORK_CONFIG_FILE_DEFAULT     "fastpath-net.cfg"

/*-------------------------------------*/
/* Start Dot1ad Factory Defaults       */
/*-------------------------------------*/

#define FD_DOT1AD_CPE_INTFERFACE_TYPE_DEFAULT        DOT1AD_INTFERFACE_TYPE_UNI
#define FD_DOT1AD_DEFAULT_L2PT_ACTION                DOT1AD_TERMINATE
#define FD_DOT1AD_PB_INTFERFACE_TYPE_DEFAULT         DOT1AD_INTFERFACE_TYPE_SWITCHPORT

#define DOT1AD_L2TP_NNI_CONFIG_ALLOW L7_TRUE

/*-------------------------------------*/
/* End Dot1ad Factory Defaults         */
/*-------------------------------------*/

/* TR-069 factory defaults */
#define FD_TR069_ACS_URL                        ""
#define FD_TR069_ACS_USER_NAME                  "acs-admin"
#define FD_TR069_ACS_PASSWD                     ""
#define FD_TR069_PERIODIC_INFORM_MODE           L7_DISABLE
#define FD_TR069_PERIODIC_INFORM_INTERVAL       0
#define FD_TR069_PERIODIC_INFORM_TIME           "0000-00-00T00:00:00"
#define FD_TR069_CONN_REQUEST_USER_NAME         "acs-admin"
#define FD_TR069_CONN_REQUEST_PASSWD            "#default#"
#define FD_TR069_UPGRADES_MANAGED               L7_FALSE

#define FD_TR069_ACS_SSLT_ROOT_PEM             "acscert.cacert"
#define FD_TR069_CLIENT_SSL_PRIV_KEY           "cpecert.priv"
#define FD_TR069_CLIENT_SSL_CERT               "cpecert.cert"

/* dot3ah Defaults */
#define FD_DOT3AH_INTF_ADMIN_MODE                              L7_FALSE
#define FD_DOT3AH_PDU_TIMER_DEFAULT                            1000 /* msec */
#define FD_DOT3AH_DEFAULT_PDU_TRANS_RATE                       1
#define FD_DOT3AH_DEFAULT_LINK_TIME_OUT                       10
#define FD_DOT3AH_REM_LB_EXPIRY_TIMER_DEFAULT                  50
#define FD_DOT3AH_PDU_DEFAULT_COUNT                           10
#define FD_DOT3AH_DEFAULT_ERR_FRAME_THRESHOLD_HIGH             1
#define FD_DOT3AH_DEFAULT_ERR_FRAME_THRESHOLD_LOW              1
#define FD_DOT3AH_DEFAULT_ERR_FRAME_THRESHOLD_WINDOW           1
#define FD_DOT3AH_DEFAULT_ERR_FRAME_PERIOD_THRESHOLD_WINDOW    1
#define FD_DOT3AH_DEFAULT_ERR_FRAME_SUM_SEC_THRESHOLD_WINDOW  60
#define FD_DOT3AH_MAX_OAMPDU_SIZE                             1518
#define FD_DOT3AH_MIN_OAMPDU_SIZE                             64

#define FD_SERIAL_HISTORY_DEFAULT_ADMIN_MODE     L7_ENABLE
#define FD_TELNET_HISTORY_DEFAULT_ADMIN_MODE     L7_ENABLE
#define FD_SSH_HISTORY_DEFAULT_ADMIN_MODE        L7_ENABLE

#define FD_SIM_DEFAULT_SYSTEM_HISTORY_SIZE    10

#define FD_SIM_DEFAULT_FREE_MEM_THRESHOLD      0 
#define FD_SIM_DEFAULT_CPU_UTIL_THRESHOLD      0
#define FD_SIM_DEFAULT_CPU_UTIL_PERIOD         0
#define FD_SIM_DEFAULT_CPU_UTIL_MAX_PERIOD     86400 /* secs */
#define FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD     5     /* secs */
#define FD_SIM_DEFAULT_CPU_UTIL_MIN_THRESHOLD  1
#define FD_SIM_DEFAULT_CPU_UTIL_MAX_THRESHOLD  100

#define HISTORYBUFFER_SIZE_MIN   0
#define HISTORYBUFFER_SIZE_MAX   216  /* be sure to change EWS_CLI_MAX_HISTORY if you change this */

#define FD_HTTP_PORT_NUM    8080        /* PTin modified: port connections (80) */
#define FD_TELNET_PORT_NUM  8023        /* PTin modified: port connections (23) */

#define FD_USER_MGR_BLCKD_ACCESS_LEVEL  0
#define FD_USER_MGR_GUEST_ACCESS_LEVEL  1
#define FD_USER_MGR_ADMIN_ACCESS_LEVEL  15

/* Auto-install Defaults */
#define FD_AUTO_INSTALL_NETWORK_CONFIG_FILE     "net.cfg"

#define FD_DOT1X_WAN_PORT                         0

#define FD_UNITMGR_DEFAULT_SFS_MODE              L7_DISABLE
#define FD_UNITMGR_DEFAULT_SFS_TRAP_MODE         L7_ENABLE
#define FD_UNITMGR_DEFAULT_SFS_TARGET_IMAGE      L7_SFS_BACKUP
#define FD_UNITMGR_DEFAULT_ALLOW_DOWNGRADE_MODE  L7_ENABLE


/* LLPF Defaults */

#define FD_LLPF_DEFAULT_ISDP_MODE                   L7_DISABLE 
#define FD_LLPF_DEFAULT_UDLD_MODE                   L7_DISABLE 
#define FD_LLPF_DEFAULT_PAGP_MODE                   L7_DISABLE 
#define FD_LLPF_DEFAULT_SSTP_MODE                   L7_DISABLE 
#define FD_LLPF_DEFAULT_VTP_MODE                    L7_DISABLE 
#define FD_LLPF_DEFAULT_DTP_MODE                    L7_DISABLE 
#define FD_LLPF_DEFAULT_ALL_MODE                    L7_DISABLE 

/* SDM template defaults */
/* Default template in builds with IPv6 routing package */
#define FD_SDM_DEFAULT_DUAL                  SDM_TEMPLATE_DUAL_DEFAULT
/* Default template in routing builds w/o IPv6 routing package */
#define FD_SDM_DEFAULT_V4ONLY                SDM_TEMPLATE_V4_DEFAULT

#ifdef INCLUDE_DEFAULTCONFIG_OVERRIDES
#include "defaultconfig_overrides.h"
#endif

#endif

