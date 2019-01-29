/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2008
*
**********************************************************************
*
* @filename     cp_cluster_msg.h
*
* @purpose      Captive Portal (CP) Messages header
*
* @component    CP
*
* @comments     none
*
* @create       01/16/2008
*
* @author       rjindal
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_CP_CLUSTER_MSG_H
#define INCLUDE_CP_CLUSTER_MSG_H

#include "datatypes.h"
#include "osapi_support.h"

/* CP Cluster support messages */
typedef enum cpClusterMsgType_s
{
  /* 0x1000-0x10ff reserved for CP messages */
  CP_CLUSTER_CP_CLIENT_NOTIFICATION_MSG       = 0x1000,
  CP_CLUSTER_CP_CLIENT_AUTH_REQUEST_MSG       = 0x1001,
  CP_CLUSTER_CP_CLIENT_AUTH_REPLY_MSG         = 0x1002,
  CP_CLUSTER_CP_CONTROLLER_COMMAND_MSG        = 0x1003,
  CP_CLUSTER_CP_CONNECTED_CLIENTS_UPDATE_MSG  = 0x1004,
  CP_CLUSTER_CP_INSTANCE_INTERFACE_UPDATE_MSG = 0x1005,
  CP_CLUSTER_CP_STATISTICS_UPDATE_MSG         = 0x1006,
  CP_CLUSTER_CP_CONFIG_SYNC_MSG               = 0x1007,
  CP_CLUSTER_CP_RESEND_MSG                    = 0x1008
} cpClusterMsgType_t;

/* CP Cluster support element IDs */
typedef enum cpClusterElementID_s
{
  CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_TYPE       = 0x0001,
  CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_TYPE       = 0x0002,
  CP_CLUSTER_IE_AUTHENTICATION_DATA_TYPE      = 0x0003,
  CP_CLUSTER_IE_CP_INTERFACE_INDEX_TYPE       = 0x0004,
  CP_CLUSTER_IE_CP_DEAUTHENTICATE_ALL_TYPE    = 0x0005,
  CP_CLUSTER_IE_BLOCK_CP_INSTANCE_TYPE        = 0x0006,
  CP_CLUSTER_IE_UNBLOCK_CP_INSTANCE_TYPE      = 0x0007,
  CP_CLUSTER_IE_CLIENT_STATUS_TYPE            = 0x0008,
  CP_CLUSTER_IE_CP_INST_TYPE                  = 0x0009,
  CP_CLUSTER_IE_CP_INST_STATUS_TYPE           = 0x000a,
  CP_CLUSTER_IE_CP_INST_AUTH_USERS_TYPE       = 0x000b,
  CP_CLUSTER_IE_CP_INST_INTF_STATUS_TYPE      = 0x000c,
  CP_CLUSTER_IE_CP_INST_INTF_AUTH_USERS_TYPE  = 0x000d,
  CP_CLUSTER_IE_CLIENT_IP_TYPE                = 0x000e,
  CP_CLUSTER_IE_USERNAME_TYPE                 = 0x000f,
  CP_CLUSTER_IE_PROTOCOL_MODE_TYPE            = 0x0010,
  CP_CLUSTER_IE_VERIFY_MODE_TYPE              = 0x0011,
  CP_CLUSTER_IE_STATS_DATA_TYPE               = 0x0012,

  CP_CLUSTER_IE_CP_GLOBAL_MODE_TYPE           = 0x0013,
  CP_CLUSTER_IE_HTTP_PORT_TYPE                = 0x0014,
  CP_CLUSTER_IE_PEER_STATS_REPORT_INTVL_TYPE  = 0x0015,
  CP_CLUSTER_IE_AUTH_SESSION_TIMEOUT_TYPE     = 0x0016,
  CP_CLUSTER_IE_CP_TRAP_MODE_TYPE             = 0x0017,
  CP_CLUSTER_IE_CLIENT_AUTH_FAILURE_TRAP_TYPE = 0x0018,
  CP_CLUSTER_IE_CLIENT_CONN_TRAP_TYPE         = 0x0019,
  CP_CLUSTER_IE_CLIENT_DB_FULL_TRAP_TYPE      = 0x001a,
  CP_CLUSTER_IE_CLIENT_DISCONN_TRAP_TYPE      = 0x001b,

  CP_CLUSTER_IE_CP_ID_TYPE                    = 0x001c,
  CP_CLUSTER_IE_CP_NAME_TYPE                  = 0x001d,
  CP_CLUSTER_IE_CP_MODE_TYPE                  = 0x001e,
  CP_CLUSTER_IE_CP_PROTOCOL_TYPE              = 0x001f,
  CP_CLUSTER_IE_CP_VERIFY_TYPE                = 0x0020,
  CP_CLUSTER_IE_CP_REDIRECT_MODE_TYPE         = 0x0021,
  CP_CLUSTER_IE_CP_REDIRECT_URL_TYPE          = 0x0022,
  CP_CLUSTER_IE_CP_SESSION_TIMEOUT_TYPE       = 0x0023,
  CP_CLUSTER_IE_CP_IDLE_TIMEOUT_TYPE          = 0x0024,
  CP_CLUSTER_IE_CP_BAND_UP_TYPE               = 0x0025,
  CP_CLUSTER_IE_CP_BAND_DOWN_TYPE             = 0x0026,
  CP_CLUSTER_IE_CP_INPUT_OCTET_TYPE           = 0x0027,
  CP_CLUSTER_IE_CP_OUTPUT_OCTET_TYPE          = 0x0028,
  CP_CLUSTER_IE_CP_TOTAL_OCTET_TYPE           = 0x0029,
  CP_CLUSTER_IE_CP_NUM_LANGUAGES_TYPE         = 0x002a,
  CP_CLUSTER_IE_CP_WEB_ID_TYPE                = 0x002b,
  CP_CLUSTER_IE_CP_WEB_CODE_TYPE              = 0x002c,
  CP_CLUSTER_IE_CP_WEB_LINK_TYPE              = 0x002d,
  CP_CLUSTER_IE_CP_NUM_INTF_TYPE              = 0x002e,
  CP_CLUSTER_IE_CP_INTF_ID_TYPE               = 0x002f,

  CP_CLUSTER_IE_GROUP_ID_TYPE                 = 0x0030,
  CP_CLUSTER_IE_GROUP_NAME_TYPE               = 0x0031,

  CP_CLUSTER_IE_USER_ID_TYPE                  = 0x0032,
  CP_CLUSTER_IE_USER_NAME_TYPE                = 0x0033,

  CP_CLUSTER_IE_USER_PWD_TYPE                 = 0x0034,
  CP_CLUSTER_IE_USER_SESSION_TIMEOUT_TYPE     = 0x0035,
  CP_CLUSTER_IE_USER_IDLE_TIMEOUT_TYPE        = 0x0036,
  CP_CLUSTER_IE_USER_BAND_UP_TYPE             = 0x0037,
  CP_CLUSTER_IE_USER_BAND_DOWN_TYPE           = 0x0038,
  CP_CLUSTER_IE_USER_INPUT_OCTET_TYPE         = 0x0039,
  CP_CLUSTER_IE_USER_OUTPUT_OCTET_TYPE        = 0x003a,
  CP_CLUSTER_IE_USER_TOTAL_OCTET_TYPE         = 0x003b,
  CP_CLUSTER_IE_USER_NUM_GROUP_TYPE           = 0x003c,
  CP_CLUSTER_IE_USER_GROUP_ID_TYPE            = 0x003d,

  CP_CLUSTER_IE_RESEND_TYPE                   = 0x003e,
  CP_CLUSTER_IE_IP_ADDRESS_TYPE               = 0x003f,

  CP_CLUSTER_IE_RADIUS_AUTH_SERVER_TYPE       = 0x0040,
  CP_CLUSTER_IE_FOREGROUND_COLOR_TYPE         = 0x0041,
  CP_CLUSTER_IE_BACKGROUND_COLOR_TYPE         = 0x0042,
  CP_CLUSTER_IE_SEPARATOR_COLOR_TYPE          = 0x0043,

  CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT1_TYPE    = 0x0044,
  CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT2_TYPE    = 0x0045

} cpClusterElementID_t;

/* CP Cluster support element size in bytes */
#define CP_CLUSTER_IE_SWITCH_MAC_ADDRESS_SIZE       6
#define CP_CLUSTER_IE_CLIENT_MAC_ADDRESS_SIZE       6
#define CP_CLUSTER_IE_AUTHENTICATION_DATA_SIZE      148
#define CP_CLUSTER_IE_CP_INTERFACE_INDEX_SIZE       4
#define CP_CLUSTER_IE_CP_DEAUTHENTICATE_ALL_SIZE    0
#define CP_CLUSTER_IE_BLOCK_CP_INSTANCE_SIZE        4
#define CP_CLUSTER_IE_UNBLOCK_CP_INSTANCE_SIZE      4
#define CP_CLUSTER_IE_CLIENT_STATUS_SIZE            1
#define CP_CLUSTER_IE_CP_INST_SIZE                  4
#define CP_CLUSTER_IE_CP_INST_STATUS_SIZE           1
#define CP_CLUSTER_IE_CP_INST_AUTH_USERS_SIZE       4
#define CP_CLUSTER_IE_CP_INST_INTF_STATUS_SIZE      1
#define CP_CLUSTER_IE_CP_INST_INTF_AUTH_USERS_SIZE  4
#define CP_CLUSTER_IE_CLIENT_IP_SIZE                4
#define CP_CLUSTER_IE_USERNAME_SIZE                 32
#define CP_CLUSTER_IE_PROTOCOL_MODE_SIZE            4
#define CP_CLUSTER_IE_VERIFY_MODE_SIZE              4
#define CP_CLUSTER_IE_STATS_DATA_SIZE               32
#define CP_CLUSTER_IE_COUNTER64_SIZE                8
#define CP_CLUSTER_IE_COUNTER32_SIZE                4

#define CP_CLUSTER_IE_CP_GLOBAL_MODE_SIZE           1
#define CP_CLUSTER_IE_HTTP_PORT_SIZE                4
#define CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT1_SIZE    4
#define CP_CLUSTER_IE_HTTP_AUX_SECURE_PORT2_SIZE    4
#define CP_CLUSTER_IE_PEER_STATS_REPORT_INTVL_SIZE  4
#define CP_CLUSTER_IE_AUTH_SESSION_TIMEOUT_SIZE     4
#define CP_CLUSTER_IE_CP_TRAP_MODE_SIZE             1
#define CP_CLUSTER_IE_CLIENT_AUTH_FAILURE_TRAP_SIZE 1
#define CP_CLUSTER_IE_CLIENT_CONN_TRAP_SIZE         1
#define CP_CLUSTER_IE_CLIENT_DB_FULL_TRAP_SIZE      1
#define CP_CLUSTER_IE_CLIENT_DISCONN_TRAP_SIZE      1

#define CP_CLUSTER_IE_CP_ID_SIZE                    1
#define CP_CLUSTER_IE_CP_NAME_SIZE                  32
#define CP_CLUSTER_IE_CP_MODE_SIZE                  1
#define CP_CLUSTER_IE_CP_PROTOCOL_SIZE              1
#define CP_CLUSTER_IE_CP_VERIFY_SIZE                1
#define CP_CLUSTER_IE_CP_REDIRECT_MODE_SIZE         1
#define CP_CLUSTER_IE_CP_REDIRECT_URL_SIZE          512
#define CP_CLUSTER_IE_CP_SESSION_TIMEOUT_SIZE       4
#define CP_CLUSTER_IE_CP_IDLE_TIMEOUT_SIZE          4
#define CP_CLUSTER_IE_CP_BAND_UP_SIZE               4
#define CP_CLUSTER_IE_CP_BAND_DOWN_SIZE             4
#define CP_CLUSTER_IE_CP_INPUT_OCTET_SIZE           4
#define CP_CLUSTER_IE_CP_OUTPUT_OCTET_SIZE          4
#define CP_CLUSTER_IE_CP_TOTAL_OCTET_SIZE           4
#define CP_CLUSTER_IE_CP_NUM_LANGUAGES_SIZE         1
#define CP_CLUSTER_IE_CP_WEB_ID_SIZE                1
#define CP_CLUSTER_IE_CP_WEB_CODE_SIZE              32
#define CP_CLUSTER_IE_CP_WEB_LINK_SIZE              512
#define CP_CLUSTER_IE_CP_NUM_INTF_SIZE              4
#define CP_CLUSTER_IE_CP_INTF_ID_SIZE               4

#define CP_CLUSTER_IE_GROUP_ID_SIZE                 1
#define CP_CLUSTER_IE_GROUP_NAME_SIZE               32

#define CP_CLUSTER_IE_USER_ID_SIZE                  1
#define CP_CLUSTER_IE_USER_NAME_SIZE                32
#define CP_CLUSTER_IE_USER_PWD_SIZE                 128
#define CP_CLUSTER_IE_USER_SESSION_TIMEOUT_SIZE     4
#define CP_CLUSTER_IE_USER_IDLE_TIMEOUT_SIZE        4
#define CP_CLUSTER_IE_USER_BAND_UP_SIZE             4
#define CP_CLUSTER_IE_USER_BAND_DOWN_SIZE           4
#define CP_CLUSTER_IE_USER_INPUT_OCTET_SIZE         4
#define CP_CLUSTER_IE_USER_OUTPUT_OCTET_SIZE        4
#define CP_CLUSTER_IE_USER_TOTAL_OCTET_SIZE         4
#define CP_CLUSTER_IE_USER_NUM_GROUP_SIZE           1
#define CP_CLUSTER_IE_USER_GROUP_ID_SIZE            1

#define CP_CLUSTER_IE_RESEND_SIZE                   1
#define CP_CLUSTER_IE_IP_ADDRESS_SIZE               4

#define CP_CLUSTER_IE_RADIUS_AUTH_SERVER_SIZE       32
#define CP_CLUSTER_IE_FOREGROUND_COLOR_SIZE         32
#define CP_CLUSTER_IE_BACKGROUND_COLOR_SIZE         32
#define CP_CLUSTER_IE_SEPARATOR_COLOR_SIZE          32

/* PDU Handle */
typedef struct cpClusterPduHdl_s
{
  L7_uchar8    *bufPtr;
  L7_uint32    offset;
} cpClusterPduHdl_t;

/* CP Cluster support message (TCP/UDP) header */
typedef struct cpClusterMsgHdr_s
{
  L7_ushort16  msgType;
  L7_ushort16  msgLen;
} cpClusterMsgHdr_t;

/* Message header size in bytes */
#define CP_CLUSTER_MSG_TYPE_SIZE      2
#define CP_CLUSTER_MSG_LEN_SIZE       2
#define CP_CLUSTER_MSG_HDR_SIZE       (CP_CLUSTER_MSG_TYPE_SIZE + CP_CLUSTER_MSG_LEN_SIZE)

/* CP Cluster support message element header */
typedef struct cpClusterMsgElementHdr_s
{
  L7_ushort16  elementId;
  L7_ushort16  elementLen;
} cpClusterMsgElementHdr_t;

/* IE header size in bytes */
#define CP_CLUSTER_IE_ID_SIZE         CP_CLUSTER_MSG_TYPE_SIZE
#define CP_CLUSTER_IE_LEN_SIZE        CP_CLUSTER_MSG_LEN_SIZE
#define CP_CLUSTER_IE_HDR_SIZE        CP_CLUSTER_MSG_HDR_SIZE

/* Maximum number of clients per client notification message based on message size:
 * (4 bytes) header + (10 bytes) switch + (31 bytes) per client
 */
#define CP_NUM_CLIENT_PER_TCP_MSG_MAX   263

/* Maximum number of clients per statistics update message based on message size:
 * (4 bytes) header + (10 bytes) switch + (46 bytes) per client
 */
#define CP_NUM_CLIENT_PER_UDP_MSG_MAX   66

/* CP-Client-Notification IE type enumeration */
typedef enum
{
  CP_CLIENT_NOTIFICATION_IE_TYPE_SWITCH_MAC_ADDRESS = 0x0001,
  CP_CLIENT_NOTIFICATION_IE_TYPE_CLIENT_MAC_ADDRESS = 0x0002,
  CP_CLIENT_NOTIFICATION_IE_TYPE_CLIENT_IP_ADDRESS  = 0x003f,
  CP_CLIENT_NOTIFICATION_IE_TYPE_CLIENT_CPID        = 0x0009,
  CP_CLIENT_NOTIFICATION_IE_TYPE_CLIENT_STATUS      = 0x0008
} cpClientNotificationIEType_t;

/* CP-Client-Notification message IE data size in bytes */
#define CP_CLIENT_NOTIFICATION_IE_SWITCH_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN
#define CP_CLIENT_NOTIFICATION_IE_CLIENT_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN
#define CP_CLIENT_NOTIFICATION_IE_CLIENT_IP_ADDRESS_SIZE    4
#define CP_CLIENT_NOTIFICATION_IE_CLIENT_CPID_SIZE          4
#define CP_CLIENT_NOTIFICATION_IE_CLIENT_STATUS_SIZE        1

#define CP_CLIENT_NOTIFICATION_MSG_TOTAL_IE_COUNT   5

/* CP-Client-Notification message IE total size in bytes */
#define CP_CLIENT_NOTIFICATION_MSG_TOTAL_IE_SIZE    (((CP_CLUSTER_IE_HDR_SIZE)*(CP_CLIENT_NOTIFICATION_MSG_TOTAL_IE_COUNT)) + \
                                                     (CP_CLIENT_NOTIFICATION_IE_SWITCH_MAC_ADDRESS_SIZE) + \
                                                     (CP_CLIENT_NOTIFICATION_IE_CLIENT_MAC_ADDRESS_SIZE) + \
                                                     (CP_CLIENT_NOTIFICATION_IE_CLIENT_IP_ADDRESS_SIZE) + \
                                                     (CP_CLIENT_NOTIFICATION_IE_CLIENT_CPID_SIZE) + \
                                                     (CP_CLIENT_NOTIFICATION_IE_CLIENT_STATUS_SIZE))
/* CP-Client-Notification message size in bytes */
#define CP_CLIENT_NOTIFICATION_MSG_SIZE             ((CP_CLUSTER_MSG_HDR_SIZE)+\
                                                     (CP_CLIENT_NOTIFICATION_MSG_TOTAL_IE_SIZE))


/* CP-Client-Auth-Request IE type enumeration */
typedef enum
{
  CP_CLIENT_AUTH_REQUEST_IE_TYPE_SWITCH_MAC_ADDRESS = 0x0001,
  CP_CLIENT_AUTH_REQUEST_IE_TYPE_CLIENT_MAC_ADDRESS = 0x0002
} cpClientAuthReqIEType_t;

/* CP-Client-Auth-Request message IE data size in bytes */
#define CP_CLIENT_AUTH_REQUEST_IE_SWITCH_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN
#define CP_CLIENT_AUTH_REQUEST_IE_CLIENT_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN

#define CP_CLIENT_AUTH_REQUEST_MSG_TOTAL_IE_COUNT   2

/* CP-Client-Auth-Request message IE total size in bytes */
#define CP_CLIENT_AUTH_REQUEST_MSG_TOTAL_IE_SIZE    (((CP_CLUSTER_IE_HDR_SIZE)*(CP_CLIENT_AUTH_REQUEST_MSG_TOTAL_IE_COUNT)) + \
                                                     (CP_CLIENT_AUTH_REQUEST_IE_SWITCH_MAC_ADDRESS_SIZE) + \
                                                     (CP_CLIENT_AUTH_REQUEST_IE_CLIENT_MAC_ADDRESS_SIZE))
/* CP-Client-Auth-Request message size in bytes */
#define CP_CLIENT_AUTH_REQUEST_MSG_SIZE             ((CP_CLUSTER_MSG_HDR_SIZE)+\
                                                     (CP_CLIENT_AUTH_REQUEST_MSG_TOTAL_IE_SIZE))


/* CP-Client-Auth-Reply IE type enumeration */
typedef enum
{
  CP_CLIENT_AUTH_REPLY_IE_TYPE_SWITCH_MAC_ADDRESS = 0x0001,
  CP_CLIENT_AUTH_REPLY_IE_TYPE_CLIENT_MAC_ADDRESS = 0x0002,
  CP_CLIENT_AUTH_REPLY_IE_TYPE_AUTH_DATA          = 0x0003
} cpClientAuthReplyIEType_t;

/* CP-Client-Auth-Reply message IE data size in bytes */
#define CP_CLIENT_AUTH_REPLY_IE_SWITCH_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN
#define CP_CLIENT_AUTH_REPLY_IE_CLIENT_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN
#define CP_CLIENT_AUTH_REPLY_IE_AUTH_DATA_SIZE            148

#define CP_CLIENT_AUTH_REPLY_MSG_TOTAL_IE_COUNT   3

/* CP-Client-Auth-Reply message IE total size in bytes */
#define CP_CLIENT_AUTH_REPLY_MSG_TOTAL_IE_SIZE    (((CP_CLUSTER_IE_HDR_SIZE)*(CP_CLIENT_AUTH_REPLY_MSG_TOTAL_IE_COUNT)) + \
                                                   (CP_CLIENT_AUTH_REPLY_IE_SWITCH_MAC_ADDRESS_SIZE) + \
                                                   (CP_CLIENT_AUTH_REPLY_IE_CLIENT_MAC_ADDRESS_SIZE) + \
                                                   (CP_CLIENT_AUTH_REPLY_IE_AUTH_DATA_SIZE))
/* CP-Client-Auth-Request message size in bytes */
#define CP_CLIENT_AUTH_REPLY_MSG_SIZE             ((CP_CLUSTER_MSG_HDR_SIZE)+\
                                                   (CP_CLIENT_AUTH_REPLY_MSG_TOTAL_IE_SIZE))


/* CP-Controller-Command IE type enumeration */
typedef enum
{
  CP_CTRL_CMD_IE_TYPE_SWITCH_MAC_ADDRESS = 0x0001,
  CP_CTRL_CMD_IE_TYPE_CLIENT_MAC_ADDRESS = 0x0002,
  CP_CTRL_CMD_IE_TYPE_INST_INDEX         = 0x0009,
  CP_CTRL_CMD_IE_TYPE_DEAUTH_ALL_CLIENTS = 0x0005,
  CP_CTRL_CMD_IE_TYPE_INST_BLOCK         = 0x0006,
  CP_CTRL_CMD_IE_TYPE_INST_UNBLOCK       = 0x0007
} cpCtrlCmdIEType_t;

/* CP-Controller-Command message IE data size in bytes */
#define CP_CTRL_CMD_IE_SWITCH_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN
#define CP_CTRL_CMD_IE_CLIENT_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN
#define CP_CTRL_CMD_IE_INST_INDEX_SIZE           4
#define CP_CTRL_CMD_IE_DEAUTH_ALL_CLIENTS_SIZE   0
#define CP_CTRL_CMD_IE_INST_BLOCK_SIZE           4
#define CP_CTRL_CMD_IE_INST_UNBLOCK_SIZE         4

#define CP_CTRL_CMD_MSG_TOTAL_IE_COUNT    6

/* CP-Controller-Command message IE total size in bytes */
#define CP_CTRL_CMD_MSG_TOTAL_IE_SIZE     (((CP_CLUSTER_IE_HDR_SIZE)*(CP_CTRL_CMD_MSG_TOTAL_IE_COUNT)) + \
                                           (CP_CTRL_CMD_IE_SWITCH_MAC_ADDRESS_SIZE) + \
                                           (CP_CTRL_CMD_IE_CLIENT_MAC_ADDRESS_SIZE) + \
                                           (CP_CTRL_CMD_IE_INST_INDEX_SIZE) + \
                                           (CP_CTRL_CMD_IE_DEAUTH_CLIENT_SIZE) + \
                                           (CP_CTRL_CMD_IE_INST_BLOCK_SIZE) + \
                                           (CP_CTRL_CMD_IE_INST_UNBLOCK_SIZE))
/* CP-Controller-Command message size in bytes */
#define CP_CTRL_CMD_MSG_SIZE              ((CP_CLUSTER_MSG_HDR_SIZE)+\
                                           (CP_CTRL_CMD_MSG_TOTAL_IE_SIZE))


/* CP-Connected-Clients-Update IE type enumeration */
typedef enum
{
  CP_CONN_CLIENTS_UPDATE_IE_TYPE_SWITCH_MAC_ADDRESS = 0x0001,
  CP_CONN_CLIENTS_UPDATE_IE_TYPE_CLIENT_MAC_ADDRESS = 0x0002,
  CP_CONN_CLIENTS_UPDATE_IE_TYPE_CLIENT_STATUS      = 0x0008,
  CP_CONN_CLIENTS_UPDATE_IE_TYPE_CLIENT_IP          = 0x000e,
  CP_CONN_CLIENTS_UPDATE_IE_TYPE_USERNAME           = 0x000f,
  CP_CONN_CLIENTS_UPDATE_IE_TYPE_PROTOCOL           = 0x0010,
  CP_CONN_CLIENTS_UPDATE_IE_TYPE_VERIFY             = 0x0011,
  CP_CONN_CLIENTS_UPDATE_IE_TYPE_INTF               = 0x0004,
  CP_CONN_CLIENTS_UPDATE_IE_TYPE_CPID               = 0x0009,
  CP_CONN_CLIENTS_UPDATE_IE_TYPE_STATS_DATA         = 0x0012
} cpConnClientsUpdateIEType_t;

/* CP-Connected-Clients-Update message IE data size in bytes */
#define CP_CONN_CLIENTS_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN
#define CP_CONN_CLIENTS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN
#define CP_CONN_CLIENTS_UPDATE_IE_CLIENT_STATUS_SIZE        1
#define CP_CONN_CLIENTS_UPDATE_IE_CLIENT_IP_SIZE            4
#define CP_CONN_CLIENTS_UPDATE_IE_USERNAME_SIZE             32
#define CP_CONN_CLIENTS_UPDATE_IE_PROTOCOL_SIZE             4
#define CP_CONN_CLIENTS_UPDATE_IE_VERIFY_SIZE               4
#define CP_CONN_CLIENTS_UPDATE_IE_INTF_SIZE                 4
#define CP_CONN_CLIENTS_UPDATE_IE_CPID_SIZE                 4
#define CP_CONN_CLIENTS_UPDATE_IE_STATS_DATA_SIZE           32

#define CP_CONN_CLIENTS_UPDATE_MSG_TOTAL_IE_COUNT   10

/* CP-Connected-Clients-Update message IE total size in bytes */
#define CP_CONN_CLIENTS_UPDATE_MSG_TOTAL_IE_SIZE    (((CP_CLUSTER_IE_HDR_SIZE)*(CP_CONN_CLIENTS_UPDATE_MSG_TOTAL_IE_COUNT)) + \
                                                     (CP_CONN_CLIENTS_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE) + \
                                                     (CP_CONN_CLIENTS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE) + \
                                                     (CP_CONN_CLIENTS_UPDATE_IE_CLIENT_STATUS_SIZE) + \
                                                     (CP_CONN_CLIENTS_UPDATE_IE_CLIENT_IP_SIZE) + \
                                                     (CP_CONN_CLIENTS_UPDATE_IE_USERNAME_SIZE) + \
                                                     (CP_CONN_CLIENTS_UPDATE_IE_PROTOCOL_SIZE) + \
                                                     (CP_CONN_CLIENTS_UPDATE_IE_VERIFY_SIZE) + \
                                                     (CP_CONN_CLIENTS_UPDATE_IE_INTF_SIZE) + \
                                                     (CP_CONN_CLIENTS_UPDATE_IE_CPID_SIZE) + \
                                                     (CP_CONN_CLIENTS_UPDATE_IE_STATS_DATA_SIZE))
/* CP-Connected-Clients-Update message size in bytes */
#define CP_CONN_CLIENTS_UPDATE_MSG_SIZE             ((CP_CLUSTER_MSG_HDR_SIZE)+\
                                                     (CP_CONN_CLIENTS_UPDATE_MSG_TOTAL_IE_SIZE))


/* CP-Instance-Interface-Update IE type enumeration */
typedef enum
{
  CP_INST_INTF_UPDATE_IE_TYPE_SWITCH_MAC_ADDRESS      = 0x0001,
  CP_INST_INTF_UPDATE_IE_TYPE_CP_INST                 = 0x0009,
  CP_INST_INTF_UPDATE_IE_TYPE_CP_INST_STATUS          = 0x000a,
  CP_INST_INTF_UPDATE_IE_TYPE_CP_INST_AUTH_USERS      = 0x000b,
  CP_INST_INTF_UPDATE_IE_TYPE_CP_INTF_INDEX           = 0x0004,
  CP_INST_INTF_UPDATE_IE_TYPE_CP_INST_INTF_STATUS     = 0x000c,
  CP_INST_INTF_UPDATE_IE_TYPE_CP_INST_INTF_AUTH_USERS = 0x000d
} cpInstIntfUpdateIEType_t;

/* CP-Instance-Interface-Update message IE data size in bytes */
#define CP_INST_INTF_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE       L7_ENET_MAC_ADDR_LEN
#define CP_INST_INTF_UPDATE_IE_CP_INST_SIZE                  4
#define CP_INST_INTF_UPDATE_IE_CP_INST_STATUS_SIZE           1
#define CP_INST_INTF_UPDATE_IE_CP_INST_AUTH_USERS_SIZE       4
#define CP_INST_INTF_UPDATE_IE_CP_INTF_INDEX_SIZE            4
#define CP_INST_INTF_UPDATE_IE_CP_INST_INTF_STATUS_SIZE      1
#define CP_INST_INTF_UPDATE_IE_CP_INST_INTF_AUTH_USERS_SIZE  4

#define CP_INST_INTF_UPDATE_MSG_TOTAL_IE_COUNT    7

/* CP-Instance-Interface-Update message IE total size in bytes */
#define CP_INST_INTF_UPDATE_MSG_TOTAL_IE_SIZE     (((CP_CLUSTER_IE_HDR_SIZE)*(CP_INST_INTF_UPDATE_MSG_TOTAL_IE_COUNT)) + \
                                                   (CP_INST_INTF_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE) + \
                                                   (CP_INST_INTF_UPDATE_IE_CP_INST_SIZE) + \
                                                   (CP_INST_INTF_UPDATE_IE_CP_INST_STATUS_SIZE) + \
                                                   (CP_INST_INTF_UPDATE_IE_CP_INST_AUTH_USERS_SIZE) + \
                                                   (CP_INST_INTF_UPDATE_IE_CP_INTF_INDEX_SIZE) + \
                                                   (CP_INST_INTF_UPDATE_IE_CP_INST_INTF_STATUS_SIZE) + \
                                                   (CP_INST_INTF_UPDATE_IE_CP_INST_INTF_AUTH_USERS_SIZE))
/* CP-Instance-Interface-Update message size in bytes */
#define CP_INST_INTF_UPDATE_MSG_SIZE              ((CP_CLUSTER_MSG_HDR_SIZE)+\
                                                   (CP_INST_INTF_UPDATE_MSG_TOTAL_IE_SIZE))


/* CP-Statistics-Update IE type enumeration */
typedef enum
{
  CP_STATISTICS_UPDATE_IE_TYPE_SWITCH_MAC_ADDRESS = 0x0001,
  CP_STATISTICS_UPDATE_IE_TYPE_CLIENT_MAC_ADDRESS = 0x0002,
  CP_STATISTICS_UPDATE_IE_TYPE_STATS_DATA         = 0x0012
} cpStatisticsUpdateIEType_t;

/* CP-Statistics-Update message IE data size in bytes */
#define CP_STATISTICS_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE      L7_ENET_MAC_ADDR_LEN
#define CP_STATISTICS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE      L7_ENET_MAC_ADDR_LEN
#define CP_STATISTICS_UPDATE_IE_STATS_DATA_SIZE              32

#define CP_STATISTICS_UPDATE_MSG_TOTAL_IE_COUNT   3

/* CP-Statistics-Update message IE total size in bytes */
#define CP_STATISTICS_UPDATE_MSG_TOTAL_IE_SIZE    (((CP_CLUSTER_IE_HDR_SIZE)*(CP_STATISTICS_UPDATE_MSG_TOTAL_IE_COUNT)) + \
                                                   (CP_STATISTICS_UPDATE_IE_SWITCH_MAC_ADDRESS_SIZE) + \
                                                   (CP_STATISTICS_UPDATE_IE_CLIENT_MAC_ADDRESS_SIZE) + \
                                                   (CP_STATISTICS_UPDATE_IE_STATS_DATA_SIZE))
/* CP-Statistics-Update message size in bytes */
#define CP_STATISTICS_UPDATE_MSG_SIZE             ((CP_CLUSTER_MSG_HDR_SIZE)+\
                                                   (CP_STATISTICS_UPDATE_MSG_TOTAL_IE_SIZE))


/* CP-Config-Sync IE type enumeration */
typedef enum
{
  CP_CLUSTER_CONFIG_MSG_IE_GLOBAL_TYPE = 1,
  CP_CLUSTER_CONFIG_MSG_IE_CP_TYPE,
  CP_CLUSTER_CONFIG_MSG_IE_GROUP_TYPE,
  CP_CLUSTER_CONFIG_MSG_IE_USER_TYPE
} CP_CLUSTER_CONFIG_MSG_IE_t;

/* CP-Config-Sync message IE data size in bytes */
#define CP_CLUSTER_CONFIG_MSG_IE_GLOBAL_SIZE  0
#define CP_CLUSTER_CONFIG_MSG_IE_CP_SIZE      0
#define CP_CLUSTER_CONFIG_MSG_IE_GROUP_SIZE   0
#define CP_CLUSTER_CONFIG_MSG_IE_USER_SIZE    0


/* CP-Resend IE type enumeration */
typedef enum
{
  CP_RESEND_IE_TYPE_SWITCH_MAC_ADDRESS = 0x0001,
  CP_RESEND_IE_TYPE_MSG_TYPE           = 0x003e
} cpResendIEType_t;

/* CP-Resend message IE data size in bytes */
#define CP_RESEND_IE_SWITCH_MAC_ADDRESS_SIZE   L7_ENET_MAC_ADDR_LEN
#define CP_RESEND_IE_MSG_TYPE_SIZE             1

#define CP_RESEND_MSG_TOTAL_IE_COUNT   2

/* CP-Resend message IE total size in bytes */
#define CP_RESEND_MSG_TOTAL_IE_SIZE    (((CP_CLUSTER_IE_HDR_SIZE)*(CP_RESEND_MSG_TOTAL_IE_COUNT)) + \
                                        (CP_RESEND_IE_SWITCH_MAC_ADDRESS_SIZE) + \
                                        (CP_RESEND_IE_MSG_TYPE_SIZE))
/* CP-Resend message size in bytes */
#define CP_RESEND_MSG_SIZE             ((CP_CLUSTER_MSG_HDR_SIZE)+\
                                        (CP_RESEND_MSG_TOTAL_IE_SIZE))


/* CP element and message header macros */

#define CP_PKT_ELEMENT_HDR_SET(element, type, len, buffer, disp)   \
                do { \
                  element.elementId = osapiHtons(type); \
                  element.elementLen = osapiHtons(len); \
                  memcpy((buffer+disp), &element, sizeof(cpClusterMsgElementHdr_t)); \
                  disp += CP_CLUSTER_IE_HDR_SIZE; \
                } while (0);

#define CP_PKT_MAC_ADDR_SET(value, buffer, disp)     \
                do { \
                  memcpy((buffer+disp), &value, L7_ENET_MAC_ADDR_LEN); \
                  disp += L7_ENET_MAC_ADDR_LEN; \
                } while (0);

#define CP_PKT_INT8_SET(value, buffer, disp)     \
                do { \
                  memcpy((buffer+disp), &value, sizeof(value)); \
                  disp += sizeof(value); \
                } while (0);

#define CP_PKT_INT16_SET(value, buffer, disp)    \
                do { \
                  value = osapiHtons(value); \
                  memcpy((buffer+disp), &value, sizeof(value)); \
                  disp += sizeof(value); \
                } while (0);

#define CP_PKT_INT32_SET(value, buffer, disp)    \
                do { \
                  value = osapiHtonl(value); \
                  memcpy((buffer+disp), &value, sizeof(value)); \
                  disp += sizeof(value); \
                } while (0);

#define CP_PKT_INT64_SET(value, buffer, disp)    \
                do { \
                  value = osapiHtonll(value); \
                  memcpy((buffer+disp), &value, sizeof(value)); \
                  disp += sizeof(value); \
                } while (0);

#define CP_PKT_STRING_SET(string, length, buffer, disp)    \
                do { \
                  memcpy((buffer+disp), string, length); \
                  disp += length; \
                } while (0);

#endif /* INCLUDE_CP_CLUSTER_MSG_H */

