/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename wireless_commdefs.h
*
* @purpose The purpose of this file is to have a central location for
*          common constants to be used by the wireless system.
*
* @component wireless
*
* @comments none
*
* @create 01/30/06
*
* @author dfowler
* @end
*
**********************************************************************/

#ifndef INCLUDE_WIRELESS_COMMDEFS
#define INCLUDE_WIRELESS_COMMDEFS


/*-------------------------------------*/
/*  Start Wireless Global Constants    */
/*-------------------------------------*/

#define L7_WIRELESS_MAX_AP_PROFILES               16
#define L7_WIRELESS_MAX_AP_RADIOS                  2
#define L7_WIRELESS_KEEP_ALIVE_TIMEOUT            30 /* in seconds */
#define L7_WIRELESS_AP_DOWNLOAD_TIMEOUT           20 /* minutes */
#define L7_WIRELESS_AP_RECONNECT_TIMEOUT          10 /* minutes */
#define L7_WIRELESS_CHANNEL_POWER_REQUEST_TIMEOUT 15 /* seconds */

#define L7_WDM_COMPONENT_NAME_MIN               1
#define L7_WDM_COMPONENT_NAME_MAX               32
#define L7_WDM_COMPONENT_ID_MAX                 16  /* size allows 1-255 */
#define L7_WDM_AGE_TIMEOUT_MIN                  0      /* hours */
#define L7_WDM_AGE_TIMEOUT_MAX                  168    /* hours */
#define L7_WDM_ROAM_TIMEOUT_MIN                 1      /* seconds */
#define L7_WDM_ROAM_TIMEOUT_MAX                 120    /* seconds */
#define L7_WDM_WS_PEER_GROUP_ID_MIN             1
#define L7_WDM_WS_PEER_GROUP_ID_MAX             255
#define L7_WDM_SOFTWARE_VERSION_MAX             32
#define L7_WDM_COUNTRY_CODE_SIZE                2
#define L7_WDM_TUNNEL_MTU_SIZE                  2
#define L7_WDM_COUNTRY_STRING_LENGTH_MAX        48
#define L7_WDM_DOMAIN_STRING_LENGTH_MAX         48
#define L7_WDM_COUNTRY_NMODE_SUPPORTED_SIZE     1

/* #define L7_WDM_MAX_SUPPORTED_CHANNELS           36 -- deprecated */
/* Deprecated - do not use!  Use L7_WDM_REG_ALL_CHANNELS instead. */
#define L7_WDM_MAX_SUPPORTED_CHANNELS           (L7_WDM_REG_ALL_CHANNELS)


#define IS_2_4GHZ_CHANNEL(ch) ( ((ch) >= 1) && ((ch) <= 14) )
/* Used to communicate to AP.  Lots of padding added for future expansion. */
#define L7_WDM_MAX_SUPPORTED_CHANNEL_ENTRIES    64


#define L7_WDM_MAX_IP_LIST_ENTRIES              256
#define L7_WDM_MAX_VLAN_LIST_ENTRIES            16
#define L7_WDM_WS_TUNNEL_MTU_MIN                1500
#define L7_WDM_WS_TUNNEL_MTU_MAX                1520
#define L7_WDM_WS_WIDS_PRIORITY_MIN             0
#define L7_WDM_WS_WIDS_PRIORITY_MAX             255

#if (L7_MAX_NUM_L2TUNNEL_VLANS == 0)
#define L7_WDM_CENT_TNNL_MAX_VLAN_ENTRIES       1
#else
#define L7_WDM_CENT_TNNL_MAX_VLAN_ENTRIES       L7_MAX_NUM_L2TUNNEL_VLANS
#endif

#define L7_WDM_WS_WIDS_WIRED_DETECTION_DISABLED                0
#define L7_WDM_WS_WIDS_WIRED_DETECTION_INTERVAL_MIN            1
#define L7_WDM_WS_WIDS_WIRED_DETECTION_INTERVAL_MAX            3600
#define L7_WDM_WS_WIDS_ROGUE_DETECTED_TRAP_NEVER_SEND          0
#define L7_WDM_WS_WIDS_ROGUE_DETECTED_TRAP_INTERVAL_MIN        60
#define L7_WDM_WS_WIDS_ROGUE_DETECTED_TRAP_INTERVAL_MAX        3600
#define L7_WDM_WS_WIDS_AP_ATTACK_INTERVAL                      10

#define L7_WDM_WS_WIDS_ROGUE_AP_MITIGATION_LIMIT               16

/* WIDS Client security related constants */

#define L7_WDM_WIDS_ROGUE_CLIENT_DETECTED_TRAP_NEVER_SEND       0
#define L7_WDM_WIDS_ROGUE_CLIENT_DETECTED_TRAP_INTERVAL_MAX     3600

#define L7_WDM_WIDS_DEAUTH_THRESHOLD_INTERVAL_MIN               1
#define L7_WDM_WIDS_DEAUTH_THRESHOLD_INTERVAL_MAX               3600

#define L7_WDM_WIDS_DEAUTH_THRESHOLD_VAL_MIN                    1
#define L7_WDM_WIDS_DEAUTH_THRESHOLD_VAL_MAX                    99999

#define L7_WDM_WIDS_AUTH_THRESHOLD_INTERVAL_MIN                 1
#define L7_WDM_WIDS_AUTH_THRESHOLD_INTERVAL_MAX                 3600

#define L7_WDM_WIDS_AUTH_THRESHOLD_VAL_MIN                      1
#define L7_WDM_WIDS_AUTH_THRESHOLD_VAL_MAX                      99999

#define L7_WDM_WIDS_PROBE_THRESHOLD_INTERVAL_MIN                1
#define L7_WDM_WIDS_PROBE_THRESHOLD_INTERVAL_MAX                3600

#define L7_WDM_WIDS_PROBE_THRESHOLD_VAL_MIN                     1
#define L7_WDM_WIDS_PROBE_THRESHOLD_VAL_MAX                     99999

#define L7_WDM_WIDS_AUTH_FAIL_THRESHOLD_VAL_MIN                 1
#define L7_WDM_WIDS_AUTH_FAIL_THRESHOLD_VAL_MAX                 99999


#define L7_WDM_DIST_TUNNEL_IDLE_TIMEOUT_MIN 30
#define L7_WDM_DIST_TUNNEL_IDLE_TIMEOUT_MAX 3600
#define L7_WDM_DIST_TUNNEL_MAX_TIMEOUT_MIN  30
#define L7_WDM_DIST_TUNNEL_MAX_TIMEOUT_MAX  86400
#define L7_WDM_DIST_TUNNEL_MCAST_REPL_MIN   1
#define L7_WDM_DIST_TUNNEL_MCAST_REPL_MAX   1024
#define L7_WDM_DIST_TUNNEL_MAX_CLIENTS_MIN  1
#define L7_WDM_DIST_TUNNEL_MAX_CLIENTS_MAX  8000


/* These parameters may be customized for specific vendors
** and hardware platforms.
*/
#define L7_WDM_LVL7_VENDOR_ID                   0x0001
#define L7_WDM_LVL7_WIRELESS_PROTOCOL_VERSION   0x0002
#define L7_WDM_LVL7_HARDWARE_TYPE               1

#define L7_WDM_AP_HARDWARE_TYPE_MIN             0
#define L7_WDM_AP_HARDWARE_TYPE_MAX             9

typedef enum
{
  L7_WDM_AP_IMAGE_TYPE_1 = 1,
  L7_WDM_AP_IMAGE_TYPE_FIRST = L7_WDM_AP_IMAGE_TYPE_1,
  L7_WDM_AP_IMAGE_TYPE_ENTERPRISE_BROADCOM = L7_WDM_AP_IMAGE_TYPE_FIRST,

  L7_WDM_AP_IMAGE_TYPE_2,
  L7_WDM_AP_IMAGE_TYPE_MJ    =  L7_WDM_AP_IMAGE_TYPE_2,

  L7_WDM_AP_IMAGE_TYPE_3,
  L7_WDM_AP_IMAGE_TYPE_6X00  =  L7_WDM_AP_IMAGE_TYPE_3,
 
  L7_WDM_AP_IMAGE_TYPE_4,
  L7_WDM_AP_IMAGE_TYPE_4748  = L7_WDM_AP_IMAGE_TYPE_4,

  L7_WDM_AP_IMAGE_TYPE_LAST,   /* always keep last */
  L7_WDM_AP_IMAGE_TYPE_ALL  =  L7_WDM_AP_IMAGE_TYPE_LAST

} L7_WDM_AP_IMAGE_TYPE_t;

/* deprecated, do not use -- for backward compatibility only.
 * Plese keep MAX value below in sync with type defined above.
 */
#define L7_WDM_AP_IMAGE_TYPE_MIN   (L7_WDM_AP_IMAGE_TYPE_ENTERPRISE_BROADCOM)
#define L7_WDM_AP_IMAGE_TYPE_MAX   (L7_WDM_AP_IMAGE_TYPE_4748)

#define L7_WDM_RADIUS_SERVER_NAME_MAX           32
#define L7_WDM_RADIUS_SERVER_NAME_MIN           1

#define L7_WDM_MAX_KNOWN_CLIENTS                1000
#define L7_WDM_RADIUS_KNOWN_CLIENTS             128

/* MAC Authentication Mode */
typedef enum
{
    L7_WDM_WS_MACAUTH_WHITELIST = 1,
    L7_WDM_WS_MACAUTH_BLACKLIST
} L7_WDM_WS_MACAUTH_MODE_t;

/* Known Client Authentication Action */
typedef enum
{
    L7_WDM_KNOWN_CLIENT_ACTION_GLOBAL,
    L7_WDM_KNOWN_CLIENT_ACTION_GRANT,
    L7_WDM_KNOWN_CLIENT_ACTION_DENY
} L7_WDM_KNOWN_CLIENT_ACTION_t;

/* Wireless Certificate regenerate action status */
typedef enum
{
  L7_WDM_CERT_GENERATION_NOT_IN_PROGRESS = 0,
  L7_WDM_CERT_GENERATION_START,
  L7_WDM_CERT_GENERATION_IN_PROGRESS
} L7_WDM_CERTIFICATE_GEN_STATUS_t;

/* Wireless switch network mutual auth mode status */
typedef enum
{
  L7_WDM_NW_MUTUAL_AUTH_NOT_STARTED = 0,
  L7_WDM_NW_MUTUAL_AUTH_EXCHANGE_CERTIFICATES_START,
  L7_WDM_NW_MUTUAL_AUTH_IN_PROGRESS,
  L7_WDM_NW_MUTUAL_AUTH_PROVISIONING_IN_PROGRESS,
  L7_WDM_NW_MUTUAL_AUTH_EXCHANGE_CERTIFICATES_IN_PROGRESS,
  L7_WDM_NW_MUTUAL_AUTH_PROVISIONING_COMPLETE,
  L7_WDM_NW_MUTUAL_AUTH_EXCHANGE_CERTIFICATES_COMPLETE,
  L7_WDM_NW_MUTUAL_AUTH_COMPLETE_WITHOUT_ERRORS,
  L7_WDM_NW_MUTUAL_AUTH_COMPLETE_WITH_ERRORS
} L7_WDM_NW_MUTUAL_AUTH_STATUS_t;

/* -----------------------------------------------------------------------------
   Wireless Threat (Rogue) Classification Test ID's
   - Order must match the descriptions in
     widsApRogueClassificationTestDescription
----------------------------------------------------------------------------- */
typedef enum
{
  L7_WIDS_CLASSIFICATION_ADMIN_CONFIGURED_ROGUE_AP = 0,   /* Code assumes a start at 0 and sequential values */
  L7_WIDS_CLASSIFICATION_MANAGED_SSID_FROM_UNKNOWN_AP,
  L7_WIDS_CLASSIFICATION_MANAGED_SSID_FROM_FAKE_MANAGED_AP,
  L7_WIDS_CLASSIFICATION_FAKE_MANAGED_AP_NO_SSID,
  L7_WIDS_CLASSIFICATION_FAKE_MANAGED_AP_INVALID_CHANNEL,
  L7_WIDS_CLASSIFICATION_MANAGED_SSID_INCORRECT_SECURITY,
  L7_WIDS_CLASSIFICATION_INVALID_SSID_FROM_MANAGED_AP,
  L7_WIDS_CLASSIFICATION_AP_ON_ILLEGAL_CHANNEL,
  L7_WIDS_CLASSIFICATION_STANDALONE_AP_UNEXPECTED_CONFIG,
  L7_WIDS_CLASSIFICATION_UNEXPECTED_WDS_DEVICE,
  L7_WIDS_CLASSIFICATION_UNMANAGED_AP_ON_WIRED_NETWORK,
  L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_COUNT
} L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t;

/* -----------------------------------------------------------------------------
   Wireless Client Threat (Rogue) Classification Test ID's
   - Order must match the descriptions in
     widsClientRogueClassificationTestDescription
----------------------------------------------------------------------------- */
typedef enum
{
  L7_WIDS_CLASSIFICATION_NOT_IN_KNOWN_CLIENT_LIST = 0,   /* Code assumes a start at 0 and sequential values */
  L7_WIDS_CLASSIFICATION_EXCEED_CONFIGURED_AUTH_RATE,
  L7_WIDS_CLASSIFICATION_EXCEED_CONFIGURED_PROBE_RATE,
  L7_WIDS_CLASSIFICATION_EXCEED_CONFIGURED_DEAUTH_RATE,
  L7_WIDS_CLASSIFICATION_EXCEED_MAX_FAILING_AUTH_MSGS,
  L7_WIDS_CLASSIFICATION_KNOWN_CLIENT_UNKNOWN_AP,
  L7_WIDS_CLASSIFICATION_NOT_IN_OUI_DATABASE,
  L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_COUNT
} L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t;


typedef enum
{
  L7_WDM_WS_TRAPS             = 0x0001,
  L7_WDM_PEER_WS_TRAPS        = 0x0002,
  L7_WDM_AP_STATE_TRAPS       = 0x0004,
  L7_WDM_AP_FAILURE_TRAPS     = 0x0008,
  L7_WDM_ROGUE_AP_TRAPS       = 0x0010,
  L7_WDM_RF_SCAN_TRAPS        = 0x0020,
  L7_WDM_CLIENT_STATE_TRAPS   = 0x0040,
  L7_WDM_CLIENT_FAILURE_TRAPS = 0x0080,
  L7_WDM_WIDS_TRAPS           = 0x0100,
  L7_WDM_TSPEC_TRAPS          = 0x0200
} L7_WDM_TRAP_FLAGS_t;

typedef enum
{
  L7_WDM_IP_STATUS_NOT_POLLED = 0,
  L7_WDM_IP_STATUS_POLLED,
  L7_WDM_IP_STATUS_DISCOVERED,
  L7_WDM_IP_STATUS_DISCOVERED_FAILED
} L7_WDM_IP_STATUS_t;

typedef enum
{
  L7_WDM_WS_MODE_ENABLE_PENDING = 0,
  L7_WDM_WS_MODE_ENABLED,
  L7_WDM_WS_MODE_DISABLE_PENDING,
  L7_WDM_WS_MODE_DISABLED
} L7_WDM_WS_MODE_STATUS_t;

 /* 'admin' disable could be due to admin initiated or 
    system mac-type (buned-in/local) changed */
typedef enum
{
  L7_WDM_WS_MODE_REASON_NONE = 0,
  L7_WDM_WS_MODE_REASON_ADMIN,
  L7_WDM_WS_MODE_REASON_NO_LOOPBACK_INTERFACE,
  L7_WDM_WS_MODE_REASON_NO_IP_ADDRESS,
  L7_WDM_WS_MODE_REASON_GLOBAL_ROUTING_DISABLED,
  L7_WDM_WS_MODE_REASON_NO_SSL_FILES,
  L7_WDM_WS_MODE_REASON_NO_ACTIVE_INTERFACE,
} L7_WDM_WS_MODE_REASON_t;

typedef enum
{
  L7_WDM_AP_VALIDATION_LOCAL = 1,
  L7_WDM_AP_VALIDATION_RADIUS
} L7_WDM_AP_VALIDATION_MODE_t;

typedef enum
{
  L7_WDM_DISC_IP_POLL_RECVD = 0,
  L7_WDM_DISC_PEER_REDIRECT,
  L7_WDM_DISC_SWITCH_IP_CONFIGURED,
  L7_WDM_DISC_SWITCH_IP_DHCP,
  L7_WDM_DISC_L2_POLL_RECVD
} L7_WDM_DISC_REASON_t;

/* status for configuration and admin requested actions such
     as AP code download, reset, etc. */
typedef enum
{
  L7_WDM_ACTION_NOT_STARTED = 0,
  L7_WDM_ACTION_REQUESTED,
  L7_WDM_ACTION_IN_PROGRESS,
  L7_WDM_ACTION_CODE_TRANSFER_IN_PROGRESS,
  L7_WDM_ACTION_WAITING_FOR_APS_TO_FINISH_DOWNLOAD,
  L7_WDM_ACTION_SUCCESS,
  L7_WDM_ACTION_PARTIAL_SUCCESS,
  L7_WDM_ACTION_ABORTED,
  L7_WDM_ACTION_NVRAM_UPDATE_IN_PROGRESS,
  L7_WDM_ACTION_TIMED_OUT,
  L7_WDM_ACTION_FAILURE
} L7_WDM_ACTION_STATUS_t;

typedef enum
{
  L7_WDM_IP_ADDR_TYPE_V4 = 0,
  L7_WDM_IP_ADDR_TYPE_V6 = 1,
} L7_WDM_IP_ADDR_TYPE_t;

/* RADIUS configuration status */
typedef enum
{
  L7_WDM_RADIUS_STATUS_NONE = 0,
  L7_WDM_RADIUS_STATUS_NOT_CONFIGURED,
  L7_WDM_RADIUS_STATUS_CONFIGURED
} L7_WDM_RADIUS_STATUS_t;

typedef enum
{
  L7_TUNNEL_TYPE_SWITCH = 1,
  L7_TUNNEL_TYPE_AP,
} L7_TUNNEL_TYPE_t;

#define L7_WDM_IPV6_ADDRESS_SIZE            16
/*-------------------------------------*/
/*  End Wireless Global Constants      */
/*-------------------------------------*/

/*-------------------------------------*/
/*  Start Local AP Database Constants  */
/*-------------------------------------*/

#define L7_WDM_MAX_LOCATION_NAME                32
#define L7_WDM_MIN_AP_PASSWORD                  8
#define L7_WDM_MAX_AP_PASSWORD                  63
#define L7_WDM_RADIO_INDEX_MIN                  1
#define L7_WDM_RADIO_INDEX_MAX                  L7_WIRELESS_MAX_AP_RADIOS
#define L7_WDM_RADIO_POWER_MIN                  0
#define L7_WDM_RADIO_POWER_MAX                  100
#define L7_WDM_RADIO_POWER_STEP                 10

#define L7_WDM_CHANNEL_AUTO                     0
#define L7_WDM_POWER_AUTO                       0

/* Managed Mode of the AP */
typedef enum
{
  L7_WDM_AP_WS_MANAGED = 1,
  L7_WDM_AP_STANDALONE,
  L7_WDM_AP_ROGUE
} L7_WDM_AP_MANAGED_MODE_t;

/* Expected WDS Mode of the Stand-alone AP */
typedef enum
{
  L7_WDM_AP_STANDALONE_WDS_MODE_ANY = 0,
  L7_WDM_AP_STANDALONE_WDS_MODE_NORMAL,
  L7_WDM_AP_STANDALONE_WDS_MODE_BRIDGE
} L7_WDM_AP_STANDALONE_WDS_MODE_t;

/* Expected Security Mode of the Stand-alone AP */
typedef enum
{
  L7_WDM_AP_STANDALONE_SECURITY_MODE_ANY = 0,
  L7_WDM_AP_STANDALONE_SECURITY_MODE_OPEN,
  L7_WDM_AP_STANDALONE_SECURITY_MODE_WEP,
  L7_WDM_AP_STANDALONE_SECURITY_MODE_WPA_WPA2
} L7_WDM_AP_STANDALONE_SECURITY_MODE_t;

/* Expected Wired Network Mode of the Stand-alone AP */
typedef enum
{
  L7_WDM_AP_STANDALONE_WIRED_NETWORK_ALLOWED = 0,
  L7_WDM_AP_STANDALONE_WIRED_NETWORK_NOT_ALLOWED
} L7_WDM_AP_STANDALONE_WIRED_NETWORK_MODE_t;

/*-------------------------------------*/
/*  End Local AP Database Constants    */
/*-------------------------------------*/

/*-------------------------------------*/
/*  Start Channel/Power Constants      */
/*-------------------------------------*/

#define L7_WDM_CHANNEL_PLAN_CANDIDATE_MAX       12
#define L7_WDM_CHANNEL_PLAN_INTERVAL_MIN        6
#define L7_WDM_CHANNEL_PLAN_INTERVAL_MAX        24
#define L7_WDM_CHANNEL_PLAN_HIST_DEPTH_MAX      10
#define L7_WDM_CHANNEL_PLAN_TIME_MAX            1440  /* 24 hour time in minutes */
#define L7_WDM_POWER_ADJ_INTERVAL_MIN           15
#define L7_WDM_POWER_ADJ_INTERVAL_MAX           1440
#define L7_WDM_POWER_ADJ_CANDIDATE_MAX          (platWirelessPeerGroupAccessPointsMaxCountGet() * \
                                                 L7_WIRELESS_MAX_AP_RADIOS)  /* TBD ??? */
/* channel plan mode */
typedef enum
{
  L7_WDM_CHANNEL_PLAN_MANUAL = 1,
  L7_WDM_CHANNEL_PLAN_INTERVAL,
  L7_WDM_CHANNEL_PLAN_TIME
} L7_WDM_CHANNEL_PLAN_MODE_t;

/* channel plan type */
typedef enum
{
  L7_WDM_CHANNEL_PLAN_NONE = 0,
  L7_WDM_CHANNEL_PLAN_A,
  L7_WDM_CHANNEL_PLAN_BG,
  L7_WDM_CHANNEL_PLAN_MAX
} L7_WDM_CHANNEL_PLAN_TYPE_t;

/* channel plan operating status */
typedef enum
{
  L7_WDM_CHANNEL_PLAN_INACTIVE = 0,
  L7_WDM_CHANNEL_PLAN_ACTIVE
} L7_WDM_CHANNEL_PLAN_STATUS_t;

/* manual channel adjustment status */
typedef enum
{
  L7_WDM_CHANNEL_ADJ_NONE = 0,
  L7_WDM_CHANNEL_ADJ_ALGO_START,
  L7_WDM_CHANNEL_ADJ_ALGO_PROGRESS,
  L7_WDM_CHANNEL_ADJ_ALGO_COMPLETE,
  L7_WDM_CHANNEL_ADJ_APPLY_START,
  L7_WDM_CHANNEL_ADJ_APPLY_PROGRESS,
  L7_WDM_CHANNEL_ADJ_APPLY_COMPLETE,
  L7_WDM_CHANNEL_ADJ_FAIL_NO_MANUAL_MODE,
  L7_WDM_CHANNEL_ADJ_FAIL_ALREADY_IN_PROGRESS,
  L7_WDM_CHANNEL_ADJ_FAIL_INTERNAL,
  L7_WDM_CHANNEL_ADJ_FAIL_TIMEOUT
} L7_WDM_CHANNEL_ADJ_STATUS_t;

/* power adjustment mode */
typedef enum
{
  L7_WDM_POWER_ADJ_MANUAL = 1,
  L7_WDM_POWER_ADJ_INTERVAL
} L7_WDM_POWER_ADJ_MODE_t;

/* manual power adjustment status */
typedef enum
{
  L7_WDM_POWER_ADJ_NONE = 0,
  L7_WDM_POWER_ADJ_ALGO_START,
  L7_WDM_POWER_ADJ_ALGO_PROGRESS,
  L7_WDM_POWER_ADJ_ALGO_COMPLETE,
  L7_WDM_POWER_ADJ_APPLY_START,
  L7_WDM_POWER_ADJ_APPLY_PROGRESS,
  L7_WDM_POWER_ADJ_APPLY_COMPLETE,
  L7_WDM_POWER_ADJ_FAIL_NO_MANUAL_MODE,
  L7_WDM_POWER_ADJ_FAIL_ALREADY_IN_PROGRESS,
  L7_WDM_POWER_ADJ_FAIL_INTERNAL,
  L7_WDM_POWER_ADJ_FAIL_TIMEOUT
} L7_WDM_POWER_ADJ_STATUS_t;

/* Config Push Global Send Status */
typedef enum
{
  L7_WDM_CONFIG_PUSH_SEND_NOT_STARTED = 0,
  L7_WDM_CONFIG_PUSH_SEND_REQUESTED,
  L7_WDM_CONFIG_PUSH_SEND_SAVING_CONFIG,
  L7_WDM_CONFIG_PUSH_SEND_SENDING_CONFIG,
  L7_WDM_CONFIG_PUSH_SEND_APPLYING_AP_PROFILE_CONFIG,
  L7_WDM_CONFIG_PUSH_SEND_COMPLETE
} L7_WDM_CONFIG_PUSH_SEND_STATUS_t;

/* Config Push Peer switch Send Status */
typedef enum
{
  L7_WDM_PEER_SWITCH_CP_SEND_NOT_STARTED = 0,
  L7_WDM_PEER_SWITCH_CP_SEND_REQUESTED,
  L7_WDM_PEER_SWITCH_CP_IN_PROGRESS,
  L7_WDM_PEER_SWITCH_CP_SEND_FAILURE_INVALID_CODE_VERSION,
  L7_WDM_PEER_SWITCH_CP_SEND_FAILURE_INVALID_HW_VERSION,
  L7_WDM_PEER_SWITCH_CP_SEND_FAILURE_OPERATION_IN_PROGRESS,
  L7_WDM_PEER_SWITCH_CP_SEND_FAILURE_INVALID_CONFIGURATION,
  L7_WDM_PEER_SWITCH_CP_SEND_FAILURE_INVALID_PACKET_FORMAT,
  L7_WDM_PEER_SWITCH_CP_SEND_FAILURE_TIMEOUT,
  L7_WDM_PEER_SWITCH_CP_SEND_FAILURE_GENERIC,
  L7_WDM_PEER_SWITCH_CP_SEND_SUCCESS
} L7_WDM_PEER_SWITCH_CP_SEND_STATUS_t;

/* Peer switch config push global receive status */
typedef enum
{
  L7_WDM_CONFIG_PUSH_RX_NOT_STARTED = 0,
  L7_WDM_CONFIG_PUSH_RX_RECEIVING_CONFIG,
  L7_WDM_CONFIG_PUSH_RX_SAVING_CONFIG,
  L7_WDM_CONFIG_PUSH_RX_APPLYING_AP_PROFILE_CONFIG,
  L7_WDM_CONFIG_PUSH_RX_FAILURE_INVALID_CODE_VERSION,
  L7_WDM_CONFIG_PUSH_RX_FAILURE_INVALID_HW_VERSION,
  L7_WDM_CONFIG_PUSH_RX_FAILURE_INVALID_CONFIGURATION,
  L7_WDM_CONFIG_PUSH_RX_FAILURE_INVALID_PACKET_FORMAT,
  L7_WDM_CONFIG_PUSH_RX_FAILURE_TIMEOUT,
  L7_WDM_CONFIG_PUSH_RX_SUCCESS
} L7_WDM_CONFIG_PUSH_RX_STATUS_t;

typedef enum
{
  L7_WDM_CONFIG_PUSH_GLOBAL_CONFIG_MASK                = 0x00000001,
  L7_WDM_CONFIG_PUSH_DISCOVERY_CONFIG_MASK             = 0x00000002,
  L7_WDM_CONFIG_PUSH_VALID_AP_DATABASE_CONFIG_MASK     = 0x00000004,
  L7_WDM_CONFIG_PUSH_CHNL_PWR_CONFIG_MASK              = 0x00000008,
  L7_WDM_CONFIG_PUSH_AP_PROFILE_CONFIG_MASK            = 0x00000010,
  L7_WDM_CONFIG_PUSH_KNOWN_CLIENT_CONFIG_MASK          = 0x00000020,
  L7_WDM_CONFIG_PUSH_ALL_CLUSTER_APPS_CONFIG_MASK      = 0xFFFF0000
} L7_WDM_CONFIG_PUSH_MASK_t;


/*-------------------------------------*/
/*  End Channel/Power Constants        */
/*-------------------------------------*/


/*-------------------------------------*/
/*  Start Network Constants            */
/*-------------------------------------*/
#define L7_WDM_NETWORK_ID_MIN                   1
#define L7_WDM_NETWORK_ID_MAX                   L7_MAX_NUM_WIRELESS_INTF
#define L7_WDM_NETWORK_ID_GUEST                 1
#define L7_WDM_NETWORK_CLIENT_QOS_BW_NONE       0
#define L7_WDM_NETWORK_CLIENT_QOS_BW_MIN        1
#define L7_WDM_NETWORK_CLIENT_QOS_BW_MAX        4294967295UL

#define L7_WDM_NETWORK_BCAST_KEY_REFRESHRATE_MIN      L7_DISABLE
#define L7_WDM_NETWORK_BCAST_KEY_REFRESHRATE_MAX      86400
#define L7_WDM_NETWORK_SESSION_KEY_REFRESHRATE_MIN    L7_DISABLE
#define L7_WDM_NETWORK_SESSION_KEY_REFRESHRATE_MAX    86400


/*-------------------------------------*/
/*  End Network Constants              */
/*-------------------------------------*/

/*-------------------------------------*/
/*  Start AP Profile Constants         */
/*-------------------------------------*/

#define L7_WDM_PROFILE_ID_MIN                   1
#define L7_WDM_PROFILE_ID_MAX                   L7_WIRELESS_MAX_AP_PROFILES
#define L7_WDM_MAX_PROFILE_NAME                 32
#define L7_WDM_MAX_AP_PROFILE_RADIUS_SECRET     64
#define L7_WDM_MAX_CLIENTS_PER_AP               1024
#define L7_WDM_KEEPALIVE_INTERVAL_MIN           5
#define L7_WDM_KEEPALIVE_INTERVAL_MAX           180
#define L7_WDM_MAX_REDIRECT_URL                 128

/* AP Profile State */
typedef enum
{
  L7_WDM_AP_PROFILE_NONE = 0,  /* not configured, TBD add to WDM, remove inUse */
  L7_WDM_AP_PROFILE_CONFIGURED = 1,
  L7_WDM_AP_PROFILE_APPLY_REQUESTED,
  L7_WDM_AP_PROFILE_APPLY_IN_PROGRESS,
  L7_WDM_AP_PROFILE_ASSOCIATED
} L7_WDM_AP_PROFILE_STATE_t;

#define L7_WDM_MAX_RADIO_NAME                         32
#define L7_WDM_AP_PROFILE_RADIO_SCAN_INTERVAL_MIN     30
#define L7_WDM_AP_PROFILE_RADIO_SCAN_INTERVAL_MAX     120
#define L7_WDM_AP_PROFILE_RADIO_SCAN_DURATION_MIN     10
#define L7_WDM_AP_PROFILE_RADIO_SCAN_DURATION_MAX     2000
#define L7_WDM_AP_PROFILE_RADIO_NORMAL_RATE_LIMIT_MIN 1
#define L7_WDM_AP_PROFILE_RADIO_NORMAL_RATE_LIMIT_MAX 50
#define L7_WDM_AP_PROFILE_RADIO_BURST_RATE_LIMIT_MIN  1
#define L7_WDM_AP_PROFILE_RADIO_BURST_RATE_LIMIT_MAX  75
#define L7_WDM_AP_PROFILE_RADIO_BEACON_INTERVAL_MIN   20
#define L7_WDM_AP_PROFILE_RADIO_BEACON_INTERVAL_MAX   2000
#define L7_WDM_AP_PROFILE_RADIO_DTIM_PERIOD_MIN       1
#define L7_WDM_AP_PROFILE_RADIO_DTIM_PERIOD_MAX       255
#define L7_WDM_AP_PROFILE_RADIO_FRAG_THRESHOLD_MIN    256
#define L7_WDM_AP_PROFILE_RADIO_FRAG_THRESHOLD_MAX    2346
#define L7_WDM_AP_PROFILE_RADIO_RTS_THRESHOLD_MIN     0
#define L7_WDM_AP_PROFILE_RADIO_RTS_THRESHOLD_MAX     2347
#define L7_WDM_AP_PROFILE_RADIO_SHORT_RETRY_LIMIT_MIN 1
#define L7_WDM_AP_PROFILE_RADIO_SHORT_RETRY_LIMIT_MAX 255
#define L7_WDM_AP_PROFILE_RADIO_LONG_RETRY_LIMIT_MIN  1
#define L7_WDM_AP_PROFILE_RADIO_LONG_RETRY_LIMIT_MAX  255
#define L7_WDM_AP_PROFILE_RADIO_MAX_CLIENTS_ALLOWED_MIN 0
#define L7_WDM_AP_PROFILE_RADIO_MAX_CLIENTS_ALLOWED_MAX L7_WIRELESS_MAX_CLIENTS_PER_RADIO
#define L7_WDM_AP_PROFILE_RADIO_DEFAULT_TX_POWER_MIN  1
#define L7_WDM_AP_PROFILE_RADIO_DEFAULT_TX_POWER_MAX  100
#define L7_WDM_LOAD_BALANCING_PERCENT_UTILIZATION_MIN 1
#define L7_WDM_LOAD_BALANCING_PERCENT_UTILIZATION_MAX 100
#define L7_WDM_RADIO_SUPPORTED_RATES_MAX              16
#define L7_WDM_RADIO_ADVERTISED_RATES_MAX             16

/* Radio Scan Frequency */
typedef enum
{
  L7_WDM_RADIO_SCAN_FREQUENCY_A = 1,
  L7_WDM_RADIO_SCAN_FREQUENCY_BG,
  L7_WDM_RADIO_SCAN_FREQUENCY_ALL
} L7_WDM_RADIO_SCAN_FREQUENCY_t;

typedef enum
{
  L7_WDM_RADIO_CHANNEL_BANDWIDTH_NONE = 0,
  L7_WDM_RADIO_CHANNEL_BANDWIDTH_20_MHZ = 1,
  L7_WDM_RADIO_CHANNEL_BANDWIDTH_40_MHZ = 2,
} L7_WDM_RADIO_CHANNEL_BANDWIDTH_t;

typedef enum
{
  L7_WDM_RADIO_PRIMARY_CHANNEL_UPPER = 1,
  L7_WDM_RADIO_PRIMARY_CHANNEL_LOWER = 2,
} L7_WDM_RADIO_PRIMARY_CHANNEL_t;

typedef enum
{
  L7_WDM_RADIO_DOT11N_PROTECTION_AUTO = 1,
  L7_WDM_RADIO_DOT11N_PROTECTION_OFF = 2,
} L7_WDM_RADIO_DOT11N_PROTECTION_t;

typedef enum
{
  L7_WDM_RADIO_GUARD_INTERVAL_LONG = 0,
  L7_WDM_RADIO_GUARD_INTERVAL_SHORT = 1,
} L7_WDM_RADIO_GUARD_INTERVAL_t;

typedef enum
{
  L7_WDM_RADIO_STBC_MODE_DISABLE = 0,
  L7_WDM_RADIO_STBC_MODE_ENABLE  = 1
} L7_WDM_RADIO_STBC_MODE_t;

typedef enum
{
  L7_WDM_RADIO_ANTENNA_LEFTRIGHT = 1,
  L7_WDM_RADIO_ANTENNA_LEFTMIDDLE = 2,
  L7_WDM_RADIO_ANTENNA_MIDDLERIGHT = 3
} L7_WDM_RADIO_ANTENNA_SEL_t;

#define L7_WDM_AP_PROFILE_RATE_100KBPS_MULT         10 /* Mbps rate in 100kbps */
#define L7_WDM_AP_PROFILE_RATE_END                  0x0000

#define L7_WDM_EDCA_QUEUE_MAX                         4
#define L7_WDM_EDCA_AIFS_MIN                          1
#define L7_WDM_EDCA_AIFS_MAX                          255
#define L7_WDM_EDCA_MAX_BURST_LENGTH_MIN              0
#define L7_WDM_EDCA_MAX_BURST_LENGTH_MAX              999900
#define L7_WDM_EDCA_TX_OP_LIMIT_MIN                   0
#define L7_WDM_EDCA_TX_OP_LIMIT_MAX                   65535

typedef enum
{
  L7_WDM_EDCA_CW_1 = 1,
  L7_WDM_EDCA_CW_3 = 3,
  L7_WDM_EDCA_CW_7 = 7,
  L7_WDM_EDCA_CW_15 = 15,
  L7_WDM_EDCA_CW_31 = 31,
  L7_WDM_EDCA_CW_63 = 63,
  L7_WDM_EDCA_CW_127 = 127,
  L7_WDM_EDCA_CW_255 = 255,
  L7_WDM_EDCA_CW_511 = 511,
  L7_WDM_EDCA_CW_1023 = 1023
} L7_WDM_EDCA_CW_VALUES_t;

/* EDCA Queue Types */
typedef enum
{
  L7_WDM_QUEUE_VOICE = 0,
  L7_WDM_QUEUE_VIDEO,
  L7_WDM_QUEUE_BEST_EFFORT,
  L7_WDM_QUEUE_BACKGROUND
} L7_WDM_QOS_QUEUES_t;

/* AP Profile VAP constants - Start */

typedef enum
{
  L7_WDM_TUNNEL_NONE = 0,            /* tunnel feature disabled or tunnel not configured */
  L7_WDM_TUNNEL_CONFIGURED,          /* tunnel routing interface configured for wireless */
  L7_WDM_TUNNEL_ROUTING_DISABLED,    /* global routing is disabled */
  L7_WDM_TUNNEL_MULTICAST_ENABLED,   /* multicast enabled on routing interface */
  L7_WDM_TUNNEL_IPV6_ENABLED,        /* IPv6 enabled on routing interface */
  L7_WDM_TUNNEL_NO_ROUTING_INTERFACE /* no ip interface matching tunnel configuration */
} L7_WDM_TUNNEL_STATUS_t;

#define L7_WDM_WEP_MAX_KEY_LENGTH               32
#define L7_WDM_SSID_MIN_LENGTH                  1
#define L7_WDM_SSID_MAX_LENGTH                  32
#define L7_WDM_WEP_INDEX_MIN                    1
#define L7_WDM_WEP_INDEX_MAX                    4
#define L7_WDM_WPA_MIN_KEY_LENGTH               8
#define L7_WDM_WPA_MAX_KEY_LENGTH               63
#define L7_WDM_WPA_MAX_HEX_KEY_LENGTH           64
#define L7_WDM_VAPID_MIN                        0
#define L7_WDM_VAPID_MAX                        15
#define L7_WDM_TUNNEL_SUBNET_MASK_LENGTH_MIN    1
#define L7_WDM_TUNNEL_SUBNET_MASK_LENGTH_MAX    32
#define L7_WDM_VAP_SESSION_TIMEOUT_MIN          0
#define L7_WDM_VAP_SESSION_TIMEOUT_MAX          1440
#define L7_WDM_WPA2_PRE_AUTHENTICATION_MIN      0
#define L7_WDM_WPA2_PRE_AUTHENTICATION_MAX      192
#define L7_WDM_WPA2_TIMEOUT_MIN                 1
#define L7_WDM_WPA2_TIMEOUT_MAX                 1440
#define L7_WDM_MAX_VAPS_PER_RADIO               16
#define L7_WDM_MAX_VAPS_PER_RADIO_8             8
#define L7_WDM_MAX_VAPS_PER_RADIO_4             4
#define L7_WDM_MAX_CELL_RECOVERY_NBRS           3
#define L7_MIN_VLAN_ID                          1

/* WEP/WPA key type */
typedef enum
{
  L7_WDM_ASCII = 1,
  L7_WDM_HEX
} L7_WDM_WEP_WPA_KEY_TYPE_t;

/* WPA supported versions */
typedef enum
{
  L7_WDM_WPA = 1,
  L7_WDM_WPA2,
  L7_WDM_WPA_WPA2
} L7_WDM_WPA_SUPPORTED_VERSIONS_t;

/* WPA cipher suites */
typedef enum
{
  L7_WDM_TKIP = 1,
  L7_WDM_CCMP,
  L7_WDM_TKIP_CCMP
} L7_WDM_WPA_CIPHER_SUITES_t;

/* MAC authentication modes */
typedef enum
{
  L7_WDM_MAC_AUTH_MODE_LOCAL = 1,
  L7_WDM_MAC_AUTH_MODE_RADIUS,
  L7_WDM_MAC_AUTH_MODE_DISABLE
} L7_WDM_MAC_AUTH_MODE_t;

/* client QoS directions */
typedef enum
{
  L7_WDM_CLIENT_QOS_DIR_DOWN = 1,
  L7_WDM_CLIENT_QOS_DIR_UP
} L7_WDM_CLIENT_QOS_DIR_t;
#define L7_WDM_CLIENT_QOS_DIR_MAX  2

/* client QoS status types */
typedef enum
{
  L7_WDM_CLIENT_QOS_OPER_STATUS = 1,
  L7_WDM_CLIENT_QOS_DOT1X_CACHE
} L7_WDM_CLIENT_QOS_PARAM_TYPE_t;

/* VAP redirect modes*/
typedef enum
{
  L7_WDM_REDIRECT_NONE = 1,
  L7_WDM_REDIRECT_HTTP,
  L7_WDM_REDIRECT_IP
} L7_WDM_REDIRECT_MODE_t;

/* VAP security modes*/
typedef enum
{
  L7_WDM_SECURITY_NONE = 1,
  L7_WDM_SECURITY_STATIC_WEP,
  L7_WDM_SECURITY_WEP_8021X,
  L7_WDM_SECURITY_WPA_PERSONAL,
  L7_WDM_SECURITY_WPA_ENTERPRISE
} L7_WDM_SECURITY_MODE_t;

/* RFScan detected security mode */
typedef enum
{
  L7_WDM_SECURITY_OPEN = 0,
  L7_WDM_SECURITY_WEP,
  L7_WDM_SECURITY_WPA,
  L7_WDM_SECURITY_UNKNOWN
} L7_WDM_SECURITY_DETECTED_t;

/* Shared Key Authentication Types */
typedef enum
{
  L7_WDM_WEP_AUTH_OPENSYSTEM = 1,
  L7_WDM_WEP_AUTH_SHAREDKEY,
  L7_WDM_WEP_AUTH_OPEN_AND_SHARED,
} L7_WDM_WEP_AUTH_TYPE_t;

/* WEP key length*/
typedef enum
{
  L7_WDM_WEP_64 = 64,
  L7_WDM_WEP_128 = 128,
} L7_WDM_WEP_KEY_LENGTH_t;

#define L7_WDM_WEP_KEY_64BIT_ASCII_EQUIVALENT       5
#define L7_WDM_WEP_KEY_128BIT_ASCII_EQUIVALENT      13
#define L7_WDM_WEP_KEY_64BIT_HEX_EQUIVALENT         10
#define L7_WDM_WEP_KEY_128BIT_HEX_EQUIVALENT        26
#define L7_WDM_WEP_KEY_IV_LENGTH                    24

/* AP Profile VAP constants - End */

/*-------------------------------------*/
/*  End AP Profile Constants           */
/*-------------------------------------*/

/*-------------------------------------*/
/*  Start Client Status Constants      */
/*-------------------------------------*/

#define L7_WDM_CLIENT_USERNAME_MAX              32
#define L7_WDM_CLIENT_PMK_SIZE                  32
#define L7_WDM_MAX_AD_HOC_CLIENTS               2048
#define L7_WDM_MAX_CLIENT_FAILURES              2048
#define L7_WDM_CLIENT_NBR_AP_MULT               4
#define L7_WDM_CLIENT_PRE_AUTH_DATA_SIZE        1600
#define L7_WDM_MAX_DETECTED_CLIENTS             (L7_WIRELESS_MAX_CLIENTS * 2)
#define L7_WDM_OUI_MAX                          32

typedef enum
{
  L7_WDM_CLIENT_PEER_NOTIFY                       = 0x00000001,
  L7_WDM_CLIENT_PMK_UPDATE_PEER                   = 0x00000002,
  L7_WDM_CLIENT_PRE_AUTH_UPDATE_PEER              = 0x00000004,
  L7_WDM_CLIENT_MAC_AUTH_UPDATE_PEER              = 0x00000008,
  L7_WDM_CLIENT_FORCE_DISASSOC                    = 0x00000010,
  L7_WDM_CLIENT_PMK_UPDATE_AP                     = 0x00000020,
  L7_WDM_CLIENT_PRE_AUTH_UPDATE_AP                = 0x00000040,
  L7_WDM_CLIENT_MAC_AUTH_UPDATE_AP                = 0x00000080,
  L7_WDM_CLIENT_PMK_PRESENT                       = 0x00000100,
  L7_WDM_CLIENT_MAC_AUTH_PRESENT                  = 0x00000200,
  L7_WDM_CLIENT_WIDS_CLIENT_SEND                  = 0x00000400,
  L7_WDM_CLIENT_TUNNEL_INFO_PRESENT               = 0x00000800,
  L7_WDM_CLIENT_QOS_DOT1X_CFG_CACHED              = 0x00001000,
  L7_WDM_CLIENT_ACCOUNTING_NOTIFY                 = 0x00002000,
  L7_WDM_CLIENT_DIST_TUNNEL_SETUP_MSG             = 0x00004000,
  L7_WDM_CLIENT_DIST_TUNNEL_TERM_MSG_SEND_ASSOCAP = 0x00008000,
  L7_WDM_CLIENT_DIST_TUNNEL_TERM_MSG_SEND_HOMEAP  = 0x00010000,
  L7_WDM_CLIENT_VLAN_ID_CACHED                    = 0x00020000,
  L7_WDM_CLIENT_ROAM_INDICATOR                    = 0x00040000,
  L7_WDM_CLIENT_RRM_PRESENT                       = 0x00080000,
  L7_WDM_CLIENT_FORCE_DISASSOC_FROM_PEER          = 0x00100000
} L7_WDM_CLIENT_FLAG_t;

typedef enum
{
    L7_WDM_CLIENT_PEER_NOTIFICATION               = 0x00000001,
    L7_WDM_CLIENT_WIDS_NOTIFICATION               = 0x00000002
} L7_WDM_CLIENT_NOTIFICATION_TYPE_t;

#define L7_WDM_CLIENT_STATE_NONE_TIMEOUT        60
#define L7_WDM_RSSI_MIN                          1
#define L7_WDM_RSSI_MAX                        100
#define L7_WDM_SIGNAL_STRENGTH_MIN            -128
#define L7_WDM_SIGNAL_STRENGTH_MAX             128
#define L7_WDM_CHANNEL_NOISE_MIN              -128
#define L7_WDM_CHANNEL_NOISE_MAX               128


/* Client Status */
typedef enum
{
  L7_WDM_CLIENT_NONE = 0,
  L7_WDM_CLIENT_ASSOC,
  L7_WDM_CLIENT_AUTH,
  L7_WDM_CLIENT_DISASSOC,
  L7_WDM_CLIENT_DELETED
} L7_WDM_CLIENT_STATUS_t;

/* Client Failure Type */
typedef enum
{
  L7_WDM_CLIENT_FAILURE_ASSOC = 0,
  L7_WDM_CLIENT_FAILURE_AUTH
} L7_WDM_CLIENT_FAILURE_t;

typedef enum
{
  L7_WDM_DETECTED_CLIENT_MODE_DISABLE = 0,
  L7_WDM_DETECTED_CLIENT_MODE_ENABLE
}L7_WDM_DETECTED_CLIENT_MODE_STATUS_t;


typedef enum
{
  L7_WDM_DETECTED_CLIENT_AUTH = 0,
  L7_WDM_DETECTED_CLIENT_DETECTED,
  L7_WDM_DETECTED_CLIENT_KNOWN,
  L7_WDM_DETECTED_CLIENT_BLACKLISTED,
  L7_WDM_DETECTED_CLIENT_ROGUE
}L7_WDM_DETECTED_CLIENT_STATUS_t;

typedef enum
{
  L7_WDM_CLIENT_NOT_AUTHENTICATED = 0,
  L7_WDM_CLIENT_AUTHENTICATED
}L7_WDM_CLIENT_AUTHENTICATION_STATUS_t;

typedef enum
{
  L7_WDM_CLIENT_THREAT_NOT_DETECTED = 0,
  L7_WDM_CLIENT_THREAT_DETECTED
}L7_WDM_CLIENT_THREAT_DETECTION_STATUS_t;

typedef enum
{
  L7_WDM_CLIENT_THREAT_MITIGATION_NOT_DONE = 0,
  L7_WDM_CLIENT_THREAT_MITIGATION_DONE
}L7_WDM_CLIENT_THREAT_MITIGATION_STATUS_t;

typedef enum
{
  L7_WDM_DETECTED_CLIENT_KNOWN_DATABASE_LOCAL= 1,
  L7_WDM_DETECTED_CLIENT_KNOWN_DATABASE_RADIUS
}L7_WDM_DETECTED_CLIENT_KNOWN_DATABASE_t;

/* Ad Hoc Client Detection Mode */
typedef enum
{
  L7_WDM_ADHOC_CLIENT_DETECTION_MODE_BEACON_FRAME,
  L7_WDM_ADHOC_CLIENT_DETECTION_MODE_DATA_FRAME
} L7_WDM_ADHOC_CLIENT_DETECTION_MODE_t;

/*-------------------------------------*/
/*  End Client Status Constants        */
/*-------------------------------------*/

/*-------------------------------------*/
/* Start Access Point Status Constants */
/*-------------------------------------*/

#define L7_WDM_SERIAL_NUM_MAX_LENGTH              32
#define L7_WDM_PART_NUM_MAX_LENGTH                32
#define L7_WDM_CONFIG_FAILURE_MSG_MAX_LENGTH      64
#define L7_WDM_MAX_ELIGIBLE_CHANNELS              13
#define L7_WDM_MAX_AP_NEIGHBORS_PER_RADIO         64
#define L7_WDM_MAX_AP_DEBUG_PASSWORD              32
#define L7_WDM_MIN_AP_DEBUG_PASSWORD              5

/* Managed AP List Status (also for neighbor AP status) */
typedef enum
{
  L7_WDM_AP_STATUS_MANAGED = 1,
  L7_WDM_AP_STATUS_UNKNOWN,
  L7_WDM_AP_STATUS_STANDALONE,
  L7_WDM_AP_STATUS_ROGUE
} L7_WDM_AP_STATUS_t;

/* Managed AP Current Status */
typedef enum
{
  L7_WDM_MANAGED_AP_NONE = 0,
  L7_WDM_MANAGED_AP_DISCOVERED,
  L7_WDM_MANAGED_AP_AUTHENTICATED,
  L7_WDM_MANAGED_AP_UPGRADING,
  L7_WDM_MANAGED_AP_MANAGED,
  L7_WDM_MANAGED_AP_FAILED,
  L7_WDM_MANAGED_AP_DELETED
} L7_WDM_MANAGED_AP_STATUS_t;

/* AP Security Mode */
typedef enum
{
  L7_WDM_AP_SECURITY_OPEN = 0,
  L7_WDM_AP_SECURITY_WEP,
  L7_WDM_AP_SECURITY_WPA
} L7_WDM_AP_SECURITY_MODE_t;

typedef enum
{
  L7_WDM_ROGUE_MITIGATION_NOT_REQD = 0,
  L7_WDM_ROGUE_MITIGATION_IN_PROGRESS,
  L7_WDM_ROGUE_MITIGATION_DISABLED,
  L7_WDM_ROGUE_MITIGATION_TOO_MANY,
  L7_WDM_ROGUE_MITIGATION_ILLEGAL_CHANNEL,
  L7_WDM_ROGUE_MITIGATION_SPOOFING,
  L7_WDM_ROGUE_MITIGATION_AD_HOC
} L7_WDM_ROGUE_MITIGATION_STATUS_t;

typedef enum
{
  L7_WDM_MANAGED_AP_PEER_NOTIFY         = 0x0001,
  L7_WDM_MANAGED_AP_INIT_CHANNEL        = 0x0002,
  L7_WDM_MANAGED_AP_INIT_POWER          = 0x0004,
  L7_WDM_MANAGED_AP_INIT_STAT_INTERVAL  = 0x0008,
  L7_WDM_MANAGED_AP_INIT_RF_SCAN        = 0x0010,
  L7_WDM_MANAGED_AP_RESET               = 0x0020,
  L7_WDM_MANAGED_AP_DEBUG_LOGIN         = 0x0040,
  L7_WDM_MANAGED_AP_KEEP_ALIVE_SEND     = 0x0080,
  L7_WDM_MANAGED_AP_AUTH_INFO_SEND      = 0x0100,
  L7_WDM_MANAGED_AP_DISCONNECT          = 0x0200,
  L7_WDM_MANAGED_AP_WIDS_AP_STATUS_SEND = 0x0400,
  L7_WDM_MANAGED_AP_WIDS_RADIO_VAP_STATUS_SEND = 0x0800,
  L7_WDM_MANAGED_AP_WIDS_FAILED_DELETE_SEND = 0x1000,
  L7_WDM_MANAGED_AP_DIST_TUNNEL_CONFIG_SEND = 0x2000,
  L7_WDM_MANAGED_AP_AUTO_UPGRADE            = 0x4000
} L7_WDM_MANAGED_AP_FLAG_t;

typedef enum
{
  L7_WDM_WS_WIDS_ELECTION_REQUIRED      = 0x0001
} L7_WDM_WS_FLAG_t;


typedef enum
{
  L7_WDM_WIDS_ELECTION_NOT_STARTED = 0,
  L7_WDM_WIDS_ELECTION_STARTED,
  L7_WDM_WIDS_ELECTION_WAITING_FOR_KEEP_ALIVE_TX,
  L7_WDM_WIDS_ELECTION_WAITING_FOR_KEEP_ALIVE_ACK,
  L7_WDM_WIDS_ELECTION_KEEP_ALIVE_ACKNOWLEDGED,
  L7_WDM_WIDS_ELECTION_RETRIGGERED,
  L7_WDM_WIDS_ELECTION_COMPLETED
} L7_WDM_WIDS_ELECTION_STATUS_t;


/* The following managed AP flags are always set when the
   AP transitions to L7_WDM_MANAGED_AP_MANAGED state */
#define L7_WDM_MANAGED_AP_INIT_FLAGS (L7_WDM_MANAGED_AP_INIT_CHANNEL | \
                                      L7_WDM_MANAGED_AP_INIT_POWER | \
                                      L7_WDM_MANAGED_AP_INIT_STAT_INTERVAL | \
                                      L7_WDM_MANAGED_AP_INIT_RF_SCAN | \
                                      L7_WDM_MANAGED_AP_DIST_TUNNEL_CONFIG_SEND)

/* AP Authentication Failure action pending flags */
typedef enum
{
  L7_WDM_AP_FAILURE_WIDS_AP_FAIL_SEND = 0x0001,
} L7_WDM_AP_FAILURE_FLAG_t;

/* Peer Switch action pending flags */
typedef enum
{
  L7_WDM_PEER_SWITCH_NOTIFY                       = 0x0001,
  L7_WDM_PEER_SWITCH_KEEP_ALIVE_ACK_SEND          = 0x0002,
  L7_WDM_PEER_SWITCH_CONFIG_PUSH_SEND             = 0x0004,
  L7_WDM_PEER_SWITCH_CONFIG_PUSH_STATUS_SEND      = 0x0008,
  L7_WDM_PEER_SWITCH_HISTORY_SEND                 = 0x0010,
  L7_WDM_PEER_SWITCH_HISTORY_CLEAR_A_SEND         = 0x0020,
  L7_WDM_PEER_SWITCH_HISTORY_CLEAR_BG_SEND        = 0x0040

} L7_WDM_PEER_SWITCH_FLAG_t;

/* Neighbor Client Status */
typedef enum
{
  L7_WDM_CLIENT_NBR_RF_SCAN        = 0x01,
  L7_WDM_CLIENT_NBR_ASSOC          = 0x02,
  L7_WDM_CLIENT_NBR_ASSOC_THIS_AP  = 0x04,
  L7_WDM_CLIENT_NBR_PROBE_REQUEST  = 0x08,
  L7_WDM_CLIENT_NBR_ADHOC_ROGUE    = 0x10,
  L7_WDM_CLIENT_NBR_ASSOC_PEER_AP  = 0x20
} L7_WDM_CLIENT_NBR_FLAGS_t;

/* Radio Physical Modes */
typedef enum
{
  L7_WDM_RADIO_PHY_MODE_NONE            = 0,
  L7_WDM_RADIO_PHY_MODE_802_11A         = 1,
  L7_WDM_RADIO_PHY_MODE_802_11BG        = 2,
  L7_WDM_RADIO_PHY_MODE_802_11AN        = 3,
  L7_WDM_RADIO_PHY_MODE_802_11BGN       = 4,
  L7_WDM_RADIO_PHY_MODE_5GHZ_N          = 5,
  L7_WDM_RADIO_PHY_MODE_2_4GHZ_N        = 6,
  L7_WDM_RADIO_PHY_MODE_LAST            = 7,
} L7_WDM_RADIO_PHY_MODE_t;

/* Regulatory Domain Mode Mask */
typedef enum
{
  L7_WDM_RADIO_PHY_NONE            = 0x0000,
  L7_WDM_RADIO_PHY_802_11A         = 0x0001,
  L7_WDM_RADIO_PHY_802_11B         = 0x0002,
  L7_WDM_RADIO_PHY_802_11G         = 0x0004,
  L7_WDM_RADIO_PHY_802_11A40U      = 0x0008,
  L7_WDM_RADIO_PHY_802_11A40L      = 0x0010,
  L7_WDM_RADIO_PHY_802_11G40U      = 0x0020,
  L7_WDM_RADIO_PHY_802_11G40L      = 0x0040,
  L7_WDM_RADIO_PHY_PRIME           = 0x0080,
  L7_WDM_RADIO_PHY_TURBO           = 0x0100,
  L7_WDM_RADIO_PHY_PRIMEG          = 0x0200,
  L7_WDM_RADIO_PHY_TURBOG          = 0x0400,
  L7_WDM_RADIO_PHY_LAST            = 0x8000,
} L7_WDM_RADIO_PHY_MODE_MASK_t;

/* AP Failure Type */
typedef enum
{
  L7_WDM_AP_FAILURE_NONE =                       0x00000001,
  L7_WDM_AP_FAILURE_NONE_AP_RELINK =             0x00000002,
  L7_WDM_AP_FAILURE_LOCAL_AUTH =                 0x00000004,
  L7_WDM_AP_FAILURE_RADIUS_AUTH =                0x00000008,
  L7_WDM_AP_FAILURE_RADIUS_UNREACH =             0x00000010,
  L7_WDM_AP_FAILURE_RADIUS_CHALLENGED =          0x00000020,
  L7_WDM_AP_FAILURE_RADIUS_RESP_INVALID =        0x00000040,
  L7_WDM_AP_FAILURE_NO_DB_ENTRY =                0x00000080,
  L7_WDM_AP_FAILURE_NO_SET_MANAGED =             0x00000100,
  L7_WDM_AP_FAILURE_PROFILE_ID_INVALID =         0x00000200,
  L7_WDM_AP_FAILURE_WDM_ERROR =                  0x00000400,
  L7_WDM_AP_FAILURE_TOO_MANY_APS =               0x00000800,
  L7_WDM_AP_FAILURE_AUTH_TIMEOUT =               0x00001000,
  L7_WDM_AP_FAILURE_AP_NOT_MANAGED =             0x00002000,
  L7_WDM_AP_FAILURE_PROFILE_HW_TYPE_MISMATCH =   0x00004000,
  L7_WDM_AP_FAILURE_RADIUS_MSG_SEND_FAILED =     0x00008000,
  L7_WDM_AP_FAILURE_CODE_IMAGE_NOT_AVAILABLE =   0x00010000,
  L7_WDM_AP_FAILURE_LAST =                       0x00020000
} L7_WDM_AP_FAILURE_t;

#define L7_WDM_AP_FAILURE_VALIDATION            (L7_WDM_AP_FAILURE_NO_DB_ENTRY | \
                                                 L7_WDM_AP_FAILURE_NO_SET_MANAGED | \
                                                 L7_WDM_AP_FAILURE_AP_NOT_MANAGED | \
                                                 L7_WDM_AP_FAILURE_WDM_ERROR | \
                                                 L7_WDM_AP_FAILURE_TOO_MANY_APS | \
                                                 L7_WDM_AP_FAILURE_PROFILE_ID_INVALID)

#define L7_WDM_AP_FAILURE_AUTHENTICATION_RADIUS (L7_WDM_AP_FAILURE_RADIUS_AUTH | \
                                                 L7_WDM_AP_FAILURE_RADIUS_UNREACH | \
                                                 L7_WDM_AP_FAILURE_RADIUS_CHALLENGED | \
                                                 L7_WDM_AP_FAILURE_RADIUS_MSG_SEND_FAILED | \
                                                 L7_WDM_AP_FAILURE_RADIUS_RESP_INVALID)

#define L7_WDM_AP_FAILURE_AUTHENTICATION        (L7_WDM_AP_FAILURE_AUTHENTICATION_RADIUS | \
                                                 L7_WDM_AP_FAILURE_AUTH_TIMEOUT | \
                                                 L7_WDM_AP_FAILURE_LOCAL_AUTH)

/*------------------------------------*/
/*  End Access Point Status Constants */
/*------------------------------------*/

/*----------------------------------------*/
/*  Start Network Visualization Constants */
/*----------------------------------------*/

#define L7_WDM_MAX_NV_GRAPHS                  12
#define L7_WDM_MAX_NV_AP_COORD                (L7_WIRELESS_MAX_PEER_GROUP_ACCESS_POINTS+128)
#define L7_WDM_MAX_NV_PEER_COORD              (L7_WIRELESS_MAX_PEER_SWITCHES+2)
#define L7_WDM_MAX_NV_GRAPH_NAME              16
#define L7_WDM_MAX_NV_FILE_NAME               32 /* use L7_MAX_FILE_NAME? */

#define L7_WNV_EVENT_QUEUE_SIZE               1024

/*----------------------------------------*/
/*  End Network Visualization Constants   */
/*----------------------------------------*/

#define L7_WIRELESS_BASE_PORT                FD_WIRELESS_BASE_PORT

#define L7_WIRELESS_IP_PORT_PRIVATE_MIN      1
#define L7_WIRELESS_IP_BASE_PORT_PRIVATE_MAX 65000
#define L7_WIRELESS_IP_PORTS_RESERVED_NUM    10
#define L7_WIRELESS_IP_PORT_PRIVATE_MAX      (L7_WIRELESS_IP_BASE_PORT_PRIVATE_MAX - L7_WIRELESS_IP_PORTS_RESERVED_NUM)

typedef enum {
  L7_WIRELESS_TNL_UDP_PORT_OFFSET = 0,
  L7_WIRELESS_UDP_PORT_OFFSET,
  L7_WIRELESS_SSL_SECURE_PORT_OFFSET,
  L7_WIRELESS_CAPWAP_DATA_UDP_SRC_PORT_OFFSET,
  L7_WIRELESS_CAPWAP_DATA_UDP_DST_PORT_OFFSET	
} L7_WIRELESS_PORT_OFFSET;

/* PEM-encoded certificate files */
#define L7_WIRELESS_SSL_PEM_DIR              FD_WIRELESS_SSL_PEM_DIR
#define L7_WIRELESS_SSL_ROOT_PEM             FD_WIRELESS_SSL_ROOT_PEM
#define L7_WIRELESS_SSL_SERVER_PEM           FD_WIRELESS_SSL_SERVER_PEM
#define L7_WIRELESS_SSL_DHWEAK_PEM           FD_WIRELESS_SSL_DHWEAK_PEM
#define L7_WIRELESS_SSL_DHSTRONG_PEM         FD_WIRELESS_SSL_DHSTRONG_PEM

/* Configurable SSL protocol Mode's */
#define L7_WIRELESS_SSL_KEY_LENGTH_MIN            512
#define L7_WIRELESS_SSL_KEY_LENGTH_MAX            1024

#define L7_WIRELESS_SSL_SUBJECT_DN_FIELD_MIN      1
#define L7_WIRELESS_SSL_SUBJECT_DN_FIELD_MAX      64
#define L7_WIRELESS_SSL_SUBJECT_DN_COUNTRY_SIZE   2
#define L7_WIRELESS_SSL_SUBJECT_LINE_MAX          256
#define L7_WIRELESS_SSL_PEM_BUFFER_SIZE_MAX       1024  /* max for one cert or one cert request */
#define L7_WIRELESS_SSL_PEM_FILE_SIZE_MAX         8192  /* max for a PEM file, could include cert chain */
#define L7_WIRELESS_SSL_FINGERPRINT_MD5_SIZE_MAX  36    /* MD5 + spaces + NULL byte */
#define L7_WIRELESS_SSL_CERT_DATE_SIZE_MAX        32

#define L7_WIRELESS_SSL_CERT_VALID_DAYS           FD_WIRELESS_SSL_CERT_VALID_DAYS
#define L7_WIRELESS_SSL_CERT_VALID_DAYS_MIN       1
#define L7_WIRELESS_SSL_CERT_VALID_DAYS_MAX       1
#define L7_WIRELESS_SSL_CERT_X509_VERSION         2     /* X509v3 */
#define L7_WIRELESS_SSL_ISSUER_COMMON_NAME        "self-signed"

#define L7_WIRELESS_SSL_SERVER_CERT_PEM           "wssl2_cert.pem"
#define L7_WIRELESS_SSL_SERVER_KEY_PEM            "wssl2_key.pem"
#define L7_WIRELESS_X509_CERTS_PEM_FILE           "wlan.pem"

#define L7_WIRELESS_UDP_MSG_SIZE_MAX        8192
#define L7_WIRELESS_TCP_MSG_SIZE_MAX        8192
#define L7_WIRELESS_L2_MSG_SIZE_MAX         1024
#define L7_ETHER_TYPE_WIRELESS              0x88e8 /* L2 message ether type */
#define L7_ETHER_PROTOCOL_ID_WIRELESS       0x0001 /* Wireless discovery protocol ID */
#define L7_WIRELESS_AUTH_PASSPHRASE_LEN     L7_WDM_MAX_AP_PASSWORD+1

#define L7_WIRELESS_CLIENT_NETBIOS_NAME_LENGTH       15

/* Config Push Byte Order Check Value */
#define L7_WIRELESS_CONFIG_PUSH_BYTE_ORDER_VALUE     0x05050A0A
#define L7_WIRELESS_CONFIG_PUSH_HANDSHAKE_TIMEOUT    900

/* AP Hardware Capability table Values */
#define L7_WDM_MAX_AP_HW_TYPE_DESCRIPTION             48
#define L7_WDM_MAX_AP_RADIO_TYPE_DESCRIPTION          32
#define L7_WDM_MAX_AP_IMAGE_TYPE_DESCRIPTION          96

/* Maximum outstanding client radius request messages on the switch */
#define L7_WIRELESS_CLIENT_RADIUS_MSG_MAX        256
#define L7_WIRELESS_CLIENT_RADIUS_MSG_SIZE       4096


/*-----------------------------------------------*/
/* Start Wireless Auto AP Image Download defines */
/*-----------------------------------------------*/
typedef enum
{
  L7_WSAP_IMAGE_MODE_NONE = 0,
  L7_WSAP_IMAGE_MODE_INTEGRATED,
  L7_WSAP_IMAGE_MODE_INDEPENDENT,
  L7_WSAP_IMAGE_MODE_BOTH
} L7_WSAP_IMAGE_MODE_t;

#define L7_WSAP_AUTO_UPD_CMDBUF_SIZE      54
#define L7_WSAP_AUTO_UPD_IMAGE_VER_CMD    "(cd /usr/local/ptin/log/fastpath/; tar -x version -f apimage-%d.tar)"
#define L7_WSAP_AUTO_UPD_IMAGE_VER_PATH   "/usr/local/ptin/log/fastpath/version"

#define L7_WSAP_AUTO_UPD_NAMEBUF_SIZE     15
#define L7_WSAP_AUTO_UPD_IMAGE_NAME       "\"apimage*.tar\""
#define L7_WSAP_AUTO_UPD_IMAGE_NAME_FILE  "apimagenames"
#define L7_WSAP_AUTO_UPD_IMAGE_NAME_CMD   "(cd /usr/local/ptin/log/fastpath/; find . -name %s > %s)"
#define L7_WSAP_AUTO_UPD_IMAGE_NAME_PATH  "(cd /usr/local/ptin/log/fastpath/)"

#define L7_WSAP_AUTO_UPD_BUF_SIZE         128
#define L7_WSAP_AUTO_UPD_IMAGE_LIST_SIZE  (L7_WDM_AP_IMAGE_TYPE_LAST-1)

/*-----------------------------------------------*/
/* End Wireless Auto AP Image Download defines   */
/*-----------------------------------------------*/


/*----------------------------------------*/
/*  Start Wireless RF Scan defines        */
/*----------------------------------------*/

/*****************************************************************
   Key to reading the names of the many, many constants below: 
   
   Mnemonic  |  Meaning
   ----------+-----------------------------
   ADM          admission
   AVG          average
   BCN          beacon
   CAT          category
   CH           channel
   COND(S)      condition(s) 
   DUR          duration
   INFO         information
   MEAS         measurement 
   NBR          neighbor
   OPER         operating
   TX           transmit | transmission

 *******************************************************************
*/

/* Macros and types for Interpreting the "cif" field */
typedef enum {
  ESS                 = 0,
  IBSS                = 1,
  CF_POLLABLE         = 2,
  CF_POLL_REQUEST     = 3,
  PRIVACY             = 4,
  SHORT_PREAMBLE      = 5,
  PBCC                = 6,
  CHANNEL_AGILITY     = 7,
  SPECTRUM_MGMT       = 8,
  QOS                 = 9,
  SHORT_SLOT_TIME     = 10,
  APSD                = 11,
  RADIO_MEAS          = 12, /* RRM */
  DSSS_OFDM           = 13,
  DELAYED_BLOCK_ACK   = 14,
  IMMEDIATE_BLOCK_ACK = 15
} rfscan_cif_bit_idx;

#define CIF_BIT_GET(cif, bit)  ((0 == ((cif) & (1 << (bit)))) ? 0 : 1)


/* Macros and types for Interpreting the "rrmEnabCapab" field  
 *
 */ 

typedef enum {
   LINK_MEAS                    = 0,
   NBR_REPORT                   = 1,
   PARALLEL_MEAS                = 2,
   REPEATED_MEAS                = 3,
   BCN_PASSIVE_MEAS             = 4,
   BCN_ACTIVE_MEAS              = 5,
   BCN_TABLE_MEAS               = 6,
   BCN_MEAS_REPORTING_CONDS     = 7,
   FRAME_MEAS                   = 8,
   CH_LOAD_MEAS                 = 9,
   NOISE_HISTOGRAM_MEAS         = 10,
   STATS_MEAS                   = 11,
   LCI_MEAS                     = 12,
   LCI_AZIMUTH                  = 13,
   TX_STREAM_CAT_MEAS           = 14,
   TRIGGERED_TX_STREAM_CAT_MEAS = 15,
   AP_CH_REPORT                 = 16,
   RRM_MIB                      = 17,
   OPER_CH_MAX_MEAS_DUR_1       = 18,
   OPER_CH_MAX_MEAS_DUR_2       = 19,
   OPER_CH_MAX_MEAS_DUR_3       = 20,
   NON_OPER_CH_MAX_MEAS_DUR_1   = 21,
   NON_OPER_CH_MAX_MEAS_DUR_2   = 22,
   NON_OPER_CH_MAX_MEAS_DUR_3   = 23,
   MEAS_PILOT_1                 = 24,
   MEAS_PILOT_2                 = 25,
   MEAS_PILOT_3                 = 26,
   MEAS_PILOT_TX_INFO           = 27,
   NBR_REPORT_TSF_OFFSET        = 28,
   RCPI_MEAS                    = 29,
   RSNI_MEAS                    = 30,
   BSS_AVG_ACCESS_DELAY         = 31,
   BSS_AVAILABLE_ADM_CAPACITY   = 32,
   ANTENNA_INFO                 = 33,
   RESERVED1                    = 34,
   RESERVED2                    = 35,
   RESERVED3                    = 36,
   RESERVED4                    = 37,
   RESERVED5                    = 38,
   RESERVED6                    = 39
}  rfscan_rrm_ec_bit_idx;

/* for extracting multiple bits */

#define CAPAB_BIT_GET(rrmec, bit) \
  ( (0 == ( ((rrmec).data[((bit)/8)]) & (1 << ((bit)%8)))) ? 0 : 1 )

/* for extracting multiple-bit fields */

#define OPER_CH_MAX_MEAS_DUR(rrmec)     (((rrmec).data[2] & 0x1c) >> 2)
#define NON_OPER_CH_MAX_MEAS_DUR(rrmec) (((rrmec).data[2] & 0xe0) >> 5)
#define MEAS_PILOT(rrmec)                ((rrmec).data[3] & 0x07)

typedef struct rrmNbrReportInput_s
{
  L7_uchar8       channel, phyType, regClass;
  L7_ushort16     cif;
} rrmNbrReportInput_t;

/*-----------------------------------------------*/
/*  End Wireless RF Scan defines                 */
/*-----------------------------------------------*/

/*-----------------------------------------------------------------*/
/*  Start Wireless Radio Resource Measurement (RRM) defines        */
/*-----------------------------------------------------------------*/

#define WDM_RRM_NBR_LIST_UPDATE_INTVL           60       /* seconds */
#define WDM_RRM_NBR_ENTRY_TIMEOUT        (60 *  15)      /* seconds */

#define RRM_CH_LOAD_DURATION_MIN                  1   /* TUs */
#define RRM_CH_LOAD_DURATION_MAX              65535  /* TUs */

#define RRM_CH_LOAD_CHANNEL_MIN                   1  
#define RRM_CH_LOAD_CHANNEL_MAX                 255


typedef enum {
  REQ_NONE = 0,         /* nothing happening yet */
  REQ_SUCCESS,
  REQ_IN_PROGRESS,
  REQ_TIMED_OUT,
  REQ_ABORTED,
  REQ_INCAPABLE,
  REQ_REFUSED
} rrmChLoadRequestStatus_t;

typedef enum {
  REQ_FAIL_NONE = 0,           /* nothing to complain about               */
  REQ_FAIL_STILL_IN_PROGRESS,  /* request is currently pending            */
  REQ_FAIL_RRM_MODE_DISABLE,   /* RRM mode is disabled                    */
  REQ_FAIL_BAD_DURATION,       /* bogus request duration                  */
  REQ_FAIL_NO_SEND_TO_AP,      /* can't find AP, or sending of msg failed */
  REQ_FAIL_CLIENT_DISASSOC     /* client not found among managed APs      */
} rrmChLoadRequestFailure_t;

typedef enum {
  REPORT_MODE_BIT_SUCCESS = 0,
  REPORT_MODE_BIT_LATE,
  REPORT_MODE_BIT_INCAPABLE,  
  REPORT_MODE_BIT_REFUSED  
} rrmChLoadReportModeBits_t;

/*---------------------------------------------------------------*/
/*  End Wireless Radio Resource Measurement (RRM) defines        */
/*---------------------------------------------------------------*/


/*----------------------------------------*/
/*  Start Provisioning and Mutual Auth    */
/*----------------------------------------*/
#define L7_WDM_AP_PROVISIONING_AGE_TIMEOUT_MIN  0
#define L7_WDM_AP_PROVISIONING_AGE_TIMEOUT_MAX  240

/* Switch Provisioning Status */
typedef enum
{
  L7_WDM_WS_PROVISIONING_NOT_STARTED = 1,
  L7_WDM_WS_PROVISIONING_REQUESTED,
  L7_WDM_WS_PROVISIONING_CERT_REQUESTED,
  L7_WDM_WS_PROVISIONING_IN_PROGRESS,
  L7_WDM_WS_PROVISIONING_FAILED,
  L7_WDM_WS_PROVISIONING_CONNECTION_FAILED,
  L7_WDM_WS_PROVISIONING_SUCCESS
} L7_WDM_WS_PROVISIONING_STATUS_t;

/* Cluster Switch Provisioning Info Status */
typedef enum
{
  L7_WDM_WS_CLUSTER_PROV_NOT_STARTED = 1,
  L7_WDM_WS_CLUSTER_PROV_IN_PROGRESS,
  L7_WDM_WS_CLUSTER_PROV_FAILED,
  L7_WDM_WS_CLUSTER_PROV_SUCCESS
} L7_WDM_WS_CLUSTER_PROV_STATUS_t;
  
/* Switch X.509 Certificate Request Status */
typedef enum
{
  L7_WDM_WS_X509_CERTIFICATE_REQ_NOT_STARTED = 1,
  L7_WDM_WS_X509_CERTIFICATE_REQ_REQUESTED,
  L7_WDM_WS_X509_CERTIFICATE_REQ_IN_PROGRESS,
  L7_WDM_WS_X509_CERTIFICATE_REQ_INVALID_IP,
  L7_WDM_WS_X509_CERTIFICATE_REQ_TIMED_OUT,
  L7_WDM_WS_X509_CERTIFICATE_REQ_SUCCESS
} L7_WDM_WS_X509_CERTIFICATE_REQ_STATUS_t;


/* AP Provisioning Status */
typedef enum
{
  L7_WDM_AP_PROVISIONING_NOT_STARTED = 1,
  L7_WDM_AP_PROVISIONING_REQUESTED,
  L7_WDM_AP_PROVISIONING_IN_PROGRESS,
  L7_WDM_AP_PROVISIONING_INVALID_SW_IP_ADDR,
  L7_WDM_AP_PROVISIONING_REJECTED,
  L7_WDM_AP_PROVISIONING_TIMEDOUT,
  L7_WDM_AP_PROVISIONING_FAILURE_UNKNOWN,
  L7_WDM_AP_PROVISIONING_SUCCESS
} L7_WDM_AP_PROVISIONING_STATUS_t;

/* AP Provisioning Start flag */
typedef enum
{
  L7_WDM_AP_PROVISIONING_FLAG_START = 1
} L7_WDM_AP_PROVISIONING_FLAG_t;

/* Global AP Provisioning Start command status */
typedef enum
{
  L7_WDM_AP_PROVISIONING_GLOBAL_NOT_STARTED = 1,
  L7_WDM_AP_PROVISIONING_GLOBAL_STARTED,
  L7_WDM_AP_PROVISIONING_GLOBAL_IN_PROGRESS,
  L7_WDM_AP_PROVISIONING_GLOBAL_SUCCESS
} L7_WDM_AP_PROVISIONING_GLOBAL_STATUS_t;

/* AP Provisioning Status */
typedef enum
{
  L7_WDM_AP_CRED_TX_NOT_STARTED = 1,
  L7_WDM_AP_CRED_TX_FAILED,
  L7_WDM_AP_CRED_TX_SUCCESS
} L7_WDM_AP_CRED_TX_STATUS_t;

/*----------------------------------------*/
/*  End Provisioning and Mutual Auth      */
/*----------------------------------------*/

/*----------------------------------------*/
/*  Start Wireless System Logging Options */
/*----------------------------------------*/

#define WIRELESS_DLOG(wd_log_level, __fmt__, __args__...) \
 if (wirelessDebugLevelCheck(wd_log_level) == L7_TRUE) \
 { \
   L7_LOGF (L7_LOG_SEVERITY_DEBUG, L7_FLEX_WIRELESS_COMPONENT_ID, __fmt__, ## __args__); \
 }

#define L7_WIRELESS_PKT_LOG(__sev__, __comp__, file, line,  __fmt__,__args__...)                         \
          l7_logf(__sev__, __comp__, file, line, __fmt__, ## __args__)

L7_BOOL wirelessDebugLevelCheck(L7_uint32 level);

typedef enum
{
  WD_LEVEL_TRACE = 0,    /* Wireless codepath Tracing */
  WD_LEVEL_ERROR,        /* Non-fatal errors */

  /* Messages Transmitted by the Wireless Switch.
  */
  WD_LEVEL_TX_FIRST = 10,  /* Debug flags for TX */

  WD_LEVEL_AUTH_TX,      /* Authentication transmit */
  WD_LEVEL_DISCOVERY_IP_TX, /* IP Discovery Message */
  WD_LEVEL_DISCOVERY_L2_TX, /* IP Discovery Message */
  WD_LEVEL_KEEP_ALIVE_TX,   /* Keep-Alive Message */
  WD_LEVEL_KEEP_ALIVE_ACK_TX,   /* Keep-Alive Ack Message */
  WD_LEVEL_RELINK_TX,       /* Relink Message */
  WD_LEVEL_PEER_AP_DISC_TX,      /* Peer AP Discover Message */
  WD_LEVEL_PEER_AP_REMOVE_TX,      /* Peer AP Discover Message */
  WD_LEVEL_PEER_CLIENT_DISC_TX,    /* Peer Client Discover Message */
  WD_LEVEL_PEER_CLIENT_REMOVE_TX,  /* Peer Client Remove Message */
  WD_LEVEL_INITIAL_ID_TX,        /* Initial AP Identification Message */
  WD_LEVEL_CLIENT_MAC_AUTH_TX,   /* Client Mac-Authentication Message */
  WD_LEVEL_CLIENT_PRE_AUTH_TX,   /* Client Pre-Authentication Message */
  WD_LEVEL_CLIENT_PMK_AUTH_TX,   /* Client PMK Authentication Message */
  WD_LEVEL_DISS_CLIENT_TX,       /* Dissasociate Client Message */

  WD_LEVEL_AP_CONFIG_TX,       /* AP Configuration Message */
  WD_LEVEL_STATS_INTVL_TX,     /* Status Interval Message */
  WD_LEVEL_SCAN_INTVL_TX,     /* Scan Report Interval Message */
  WD_LEVEL_STATS_CLEAR_TX,     /* Clear Stats Message */
  WD_LEVEL_RF_SCAN_REPORT_INTVL_TX,     /* Scan Report Interval Message */
  WD_LEVEL_AP_AUTH_REQ_TX,     /* AP Authentication Required Message */
  WD_LEVEL_AP_AUTH_SUCCEEDED_TX,     /* AP Authentication Succeeded Message */
  WD_LEVEL_NEW_ARP_TX,     /* New ARP Message */
  WD_LEVEL_DEBUG_LOGIN_TX,     /* Debug Login */
  WD_LEVEL_START_AUTH_TX,     /* Start Authentication Message */
  WD_LEVEL_NETWORK_PACKET_TX, /* Network Packet Message */
  WD_LEVEL_RESEND_CLIENT_INFO_TX,  /* Resend client info command Message */
  WD_LEVEL_CONFIG_PUSH_TX,              /* Configuration Push Config Message */
  WD_LEVEL_L2DT_SETUP_TX,
  WD_LEVEL_L2DT_TERMINATE_TX,
  WD_LEVEL_L2DT_CONFIG_TX,
  WD_LEVEL_L2DT_PACKET_TX,
  WD_LEVEL_L2DT_ASSOC_REPLY_TX,
  WD_LEVEL_TSPEC_PKT_TX,      /* TSPEC Admission/Deletion Messages */
  WS_LEVEL_PROVISIONING_TX,

  WD_LEVEL_TX_LAST,  /* End of TX List. */

  /* Messages Received by the Wireless Switch.
  */
  WD_LEVEL_RX_FIRST = 50,  /* Debug flags for RX */

  WD_LEVEL_AUTH_RX,      /* Authentication receive */
  WD_LEVEL_DISCOVERY_IP_RX, /* IP Discovery Message */
  WD_LEVEL_DISCOVERY_L2_RX, /* IP Discovery Message */
  WD_LEVEL_KEEP_ALIVE_RX,   /* Keep-Alive Message */
  WD_LEVEL_KEEP_ALIVE_ACK_RX,   /* Keep-Alive Ack Message */
  WD_LEVEL_RELINK_RX,       /* Relink Message */
  WD_LEVEL_CONFIG_STATUS_RX, /* Config Status Message */
  WD_LEVEL_SCAN_REPORT_RX,   /* Scan Report Message */
  WD_LEVEL_PROBE_REQ_RX,     /* Probe Request Message */
  WD_LEVEL_CLIENT_STATS_RX,  /* Client Statistics Message */
  WD_LEVEL_CLIENT_ASSOC_RX,  /* Client Association Message */
  WD_LEVEL_CLIENT_DISS_RX,   /* Client Disassociation Message */
  WD_LEVEL_CLIENT_ASSOC_FAILED_RX,  /* Client Association Failed Message */
  WD_LEVEL_PEER_AP_DISC_RX,  /* Peer AP Discover Message */
  WD_LEVEL_PEER_AP_REMOVE_RX,  /* Peer AP Discover Message */
  WD_LEVEL_PEER_CLIENT_DISC_RX,  /* Peer Client Discover Message */
  WD_LEVEL_PEER_CLIENT_REMOVE_RX,  /* Peer Client Remove Message */
  WD_LEVEL_INITIAL_ID_RX,        /* Initial AP Identification Message */
  WD_LEVEL_CLIENT_MAC_AUTH_RX,   /* Client Mac-Authentication Message */
  WD_LEVEL_CLIENT_PRE_AUTH_RX,   /* Client Pre-Authentication Message */
  WD_LEVEL_CLIENT_PMK_AUTH_RX,   /* Client PMK Authentication Message */
  WD_LEVEL_CLIENT_AUTH_NOTIFY_RX,   /* Client Authentication Notify Message */
  WD_LEVEL_DISS_CLIENT_RX,       /* Dissasociate Client Message */
  WD_LEVEL_NETWORK_PACKET_RX,    /* Network Packet Message */
  WD_LEVEL_UPDATE_STATUS_RX,  /* Code Download Status Message */
  WD_LEVEL_AP_STATS_RX,       /* AP statistics report */
  WD_LEVEL_CONFIG_PUSH_RX,        /* Configuration Push Config Message */
  WD_LEVEL_CLIENT_CONN_RX,   /* Client Connected Message */
  WD_LEVEL_L2DT_SETUP_RX,
  WD_LEVEL_L2DT_TERMINATE_RX,
  WD_LEVEL_L2DT_PACKET_RX,
  WD_LEVEL_L2DT_ASSOC_RX,
  WD_LEVEL_L2DT_AUTH_RX,
  WD_LEVEL_L2DT_PEER_DISC_RX,
  WD_LEVEL_TSPEC_PKT_RX,      /* TSPEC Admission/Deletion Messages */
  WD_LEVEL_PROVISIONING_RX,

  WD_LEVEL_RX_LAST,  /* End of RX List. */

  /* WDM Database Access Flags.
  */
  WD_LEVEL_WDM_FIRST = 100,  /* WDM Events */
  WD_LEVEL_WDM,          /* Wireless Database Manager */
  WD_LEVEL_WDM_DATA_ERROR, /* logs for non-fatal invalid data */
  WD_LEVEL_AUTH_WDM,     /* Authentication calls to WDM */

  WD_LEVEL_WDM_LAST,     /* The Last Database Event */

  /* Component-level debug flags.
  */
  WD_LEVEL_COMPONENT_FIRST = 110, /* Start of component-level debug flags. */

  WD_LEVEL_AUTH_TRACE,   /* Authentication tracing */
  WD_LEVEL_AUTH_QUEUE_TRACE,   /* Authentication queue tracing */
  WD_LEVEL_AUTH_ERROR,   /* Authentication errors */
  WD_LEVEL_AUTH_PKT,     /* Authentication packet dumps */
  WD_LEVEL_AUTH_RADIUS,  /* Authentication RADIUS AP debug */
  WD_LEVEL_AUTH_RADIUS_OTHER,  /* Authentication RADIUS other user debug */

  WD_LEVEL_WSAP_CONN,    /* WS to AP Connection events */

  WD_LEVEL_WSAP_COMM_TCP_PKT_TX,/* Peer communication TCP packet Transmit  dumps */
  WD_LEVEL_WSAP_COMM_TCP_PKT_RX,/* Peer communication TCP packet Receive  dumps */
  WD_LEVEL_WSAP_COMM_UDP_PKT_TX,/* Peer communication UDP packet Transmit dumps */
  WD_LEVEL_WSAP_COMM_UDP_PKT_RX,/* Peer communication UDP packet Receive dumps */

  WD_LEVEL_WSAP_COMM_TNL_UDP_PKT_TX,/* Peer communication TNL UDP packet Transmit dumps */
  WD_LEVEL_WSAP_COMM_TNL_UDP_PKT_RX,/* Peer communication TNL UDP packet Receive dumps */

  WD_LEVEL_WSAP_COMM_L2_PKT_TX,/* Peer communication L2 packet Transmit dumps */
  WD_LEVEL_WSAP_COMM_L2_PKT_RX,/* Peer communication L2 packet Receive dumps */
  WD_LEVEL_WSAP_COMM_INFO,/* Peer communication Informational */

  WD_LEVEL_DISC_PKT,     /* Discovery packet dumps */
  WD_LEVEL_DISC_INFO,    /* Discovery Informational */

  WD_LEVEL_WSAP_STATS_INFO,   /* Statistics Collector informational */
  WD_LEVEL_WSAP_STATS_PKT,    /* Statistics Collector packet dump */

  WD_LEVEL_CLIENTASSOCLB_PKT,/* Client association and load balancing packet dumps */
  WD_LEVEL_CLIENTASSOCLB_INFO,/* Client association Load Balancing informational */

  WD_LEVEL_RF_SCAN_CACHED_RADIUS, /* RF Scan Switch Cached Radius db events */
  WD_LEVEL_KNOWN_CLIENT_CACHED_RADIUS, /* Known Client Switch Cached Radius db events */

  WD_LEVEL_WSAP_CONF_INFO,  /* AP Configurator informational */
  WD_LEVEL_WSAP_CONF_PKT,   /* AP Configurator packet dump */

  WD_LEVEL_WSAP_UPD_INFO,  /* AP Update informational */
  WD_LEVEL_WSAP_RESET_INFO,  /* AP Reset informational */

  WD_LEVEL_SSL_TRACE,    /* SSL tracing */

  WD_LEVEL_SSL_INFO,     /* SSL Informational */
  WD_LEVEL_SSL_ERROR,    /* SSL Errors */
  WD_LEVEL_SSL_LIB,      /* SSL OpenSSL Library */
  WD_LEVEL_SSL_LIBSEM,   /* SSL OpenSSL Library semaphore locking */
  WD_LEVEL_SSL_CONN,     /* SSL Connection events */
  WD_LEVEL_SSL_RX,       /* SSL Receive events */
  WD_LEVEL_SSL_TX,       /* SSL Transmit events */
  WD_LEVEL_SSL_TMR_INFO, /* SSL Timer events */
  WD_LEVEL_WTNNL_INFO,   /* WTNNL informational events */
  WD_LEVEL_WTNNL_ERROR,  /* WTNNL errors */
  WD_LEVEL_WTNNL_ARP,    /* WTNNL ARP packet processing */
  WD_LEVEL_WTNNL_DHCP,   /* WTNNL DHCP packet processing */
  WD_LEVEL_WTNNL_MCAST,  /* WTNNL Multicast packet processing */

  WD_LEVEL_NV_INFO,      /* NV informational events */
  WD_LEVEL_NV_ERROR,     /* NV errors */

  WD_LEVEL_CP_INFO,      /* Channel-Power info msgs */
  WD_LEVEL_CP_INFO2,     /* Channel-Power info-lvl2 msgs */
  WD_LEVEL_CP_POWER_EVENT,/* Channel-Power info msgs */
  WD_LEVEL_CP_ERROR,     /* Channel-Power component error msgs */
  WD_LEVEL_CP_WDM_ERROR, /* Failures to WDM calls */
  WD_LEVEL_CR_INFO,      /* Cell recovery msgs */
  WD_LEVEL_CONFIG_PUSH_PKT_RX,     /* Configuration Push dumps */
  WD_LEVEL_CONFIG_PUSH_PKT_TX,     /* Configuration Push dumps */
  WD_LEVEL_CONFIG_PUSH_INFO,     /* Configuration Push Information */
  WD_LEVEL_WIDS_PKT_RX,            /* WIDS Rx Packet Dump */
  WD_LEVEL_WIDS_PKT_TX,            /* WIDS Tx Packet Dump */
  WD_LEVEL_WIDS_INFO,              /* WIDS informational */
  WD_LEVEL_WIDS_AP_INFO,           /* WIDS AP informational */
  WD_LEVEL_WIDS_RADIO_VAP_INFO,    /* WIDS Radio VAP informational */
  WD_LEVEL_WIDS_AP_FAIL_INFO,      /* WIDS AP failure informational */
  WD_LEVEL_WIDS_CLIENT_INFO,       /* WIDS client informational */
  WD_LEVEL_WIDS_PURGE_AP_INFO,     /* WIDS Purge AP Db informational */

  WD_LEVEL_THREAT_CLASSIFICATION,   /* Threat classification informational */
  WD_LEVEL_CLIENT_SECURITY,         /* Client Security informational */
  WD_LEVEL_WIDS_CONFIG_MSG_TX,      /* WIDS Configuration Message informational */
  WD_LEVEL_CLN_THREAT_MITIGATION,   /* Client Threat Mitigation informational */

  WD_LEVEL_RADIUS_PROXY_CMD_INFO, /* Radius proxy command informational */
  WD_LEVEL_RADIUS_PROXY_CMD_TX,   /* Radius proxy command send */
  WD_LEVEL_RADIUS_PROXY_CMD_RX,   /* Radius proxy command receive */
  WD_LEVEL_RADIUS_PROXY_CMD_PKT,  /* Radius proxy command packet dump */

  WD_LEVEL_CENTTNNL_INFO,   /* CENTTNNL informational events */
  WD_LEVEL_CENTTNNL_ERROR,  /* CENTTNNL errors */

  WD_LEVEL_WSAP_CONF_CLTQOS_INFO,                     /* AP Client QoS informational */
  WD_LEVEL_WSAP_CONF_CLTQOS_ERROR,                    /* AP Client QoS errors */
  WD_LEVEL_WSAP_CONF_CLTQOS_CALLBACK,                 /* AP Client QoS callbacks */
  WD_LEVEL_WSAP_CONF_CLTQOS_FORMATTED_MSG,            /* AP Client QoS display formatted message contents */
  WD_LEVEL_WSAP_CONF_CLTQOS_LIST_UPDATE_PKT_TX,       /* AP Client QoS Tx List Update packet dump */
  WD_LEVEL_WSAP_CONF_CLTQOS_LIST_RENAME_PKT_TX,       /* AP Client QoS Tx List Rename packet dump */
  WD_LEVEL_WSAP_CONF_CLTQOS_LIST_DELETE_PKT_TX,       /* AP Client QoS Tx List Delete packet dump */
  WD_LEVEL_WSAP_CONF_CLTQOS_DEF_REQ_PKT_RX,           /* AP Client QoS Rx Definition Request packet dump */
  WD_LEVEL_WSAP_CONF_CLTQOS_ACL_DEF_REPLY_PKT_TX,     /* AP Client QoS Tx ACL Definition Reply packet dump */
  WD_LEVEL_WSAP_CONF_CLTQOS_POLICY_DEF_REPLY_PKT_TX,  /* AP Client QoS Tx Diffserv Policy Definition Reply packet dump */
  WD_LEVEL_WSAP_CONF_CLTQOS_MODE_UPDATE_PKT_TX,       /* AP Client QoS Tx Mode Update packet dump */

  WD_LEVEL_TSPEC_INFO,                                /* TSPEC informational */
  WD_LEVEL_TSPEC_ERROR,                               /* TSPEC errors */
  WD_LEVEL_TSPEC_PKT,                                 /* TSPEC packet dumps (miscellaneous) */
  WD_LEVEL_TSPEC_DETAIL,                              /* TSPEC detailed information */
  WD_LEVEL_TSPEC_TASK_INFO,                           /* TSPEC task informational */
  WD_LEVEL_TSPEC_MGMT_ACTION_INFO,                    /* TSPEC Admission Request/Reply and Deletion informational */
  WD_LEVEL_TSPEC_MGMT_ACTION_PKT,                     /* TSPEC Admission Request/Reply and Deletion packet dumps */
  WD_LEVEL_TSPEC_MGMT_ACTION_DETAIL,                  /* TSPEC Admission Request/Reply and Deletion detailed information */
  WD_LEVEL_TSPEC_ACU_INFO,                            /* TSPEC Admission Control Unit informational */
  WD_LEVEL_TSPEC_ACU_DETAIL,                          /* TSPEC Admission Control Unit detailed information */

  WD_LEVEL_WSAP_MUTUAL_AUTH_INFO,                     /* Mutual authentication informational */
  WD_LEVEL_WSAP_MUTUAL_AUTH_ERROR,                    /* Mutual authentication errors */
  WD_LEVEL_WSAP_MUTUAL_AUTH_PKT,                      /* Mutual authentication pkt dumps */
  WD_LEVEL_WS_PROVISION_INFO,                         /* Switch Provisioning information */
  WD_LEVEL_WS_PROVISION_ERROR,                        /* Switch Provisioning errors */
  WD_LEVEL_WS_PROVISION_PKT,                          /* Switch Provisioning pkt dumps */
  WD_LEVEL_COMPONENT_LAST,     /* The Last component-level Event */


  /* This must be the last element in the list.
  */
  WD_LEVEL_LAST
} wireless_log_level_t;

/*===============================================*/
/* NOTE: Remember to add an entry below for each */
/*       name added to the wireless_log_level_t  */
/*       enum above.                             */
/*                                               */
/*       Order doesn't strictly matter, but      */
/*       maintenance is easier if entries are    */
/*       kept in the same order as the enum.     */
/*===============================================*/

typedef struct
{
  L7_uint32     id;
  L7_char8      *name;
} wireless_log_level_enum_display_t;

#define WD_LEVEL_ENUM_ENTRY_NAMES \
{ \
  { WD_LEVEL_TRACE, "WD_LEVEL_TRACE" }, \
  { WD_LEVEL_ERROR, "WD_LEVEL_ERROR" }, \
  { WD_LEVEL_TX_FIRST, "WD_LEVEL_TX_FIRST" }, \
  { WD_LEVEL_AUTH_TX, "WD_LEVEL_AUTH_TX" }, \
  { WD_LEVEL_DISCOVERY_IP_TX, "WD_LEVEL_DISCOVERY_IP_TX" }, \
  { WD_LEVEL_DISCOVERY_L2_TX, "WD_LEVEL_DISCOVERY_L2_TX" }, \
  { WD_LEVEL_KEEP_ALIVE_TX, "WD_LEVEL_KEEP_ALIVE_TX" }, \
  { WD_LEVEL_KEEP_ALIVE_ACK_TX, "WD_LEVEL_KEEP_ALIVE_ACK_TX" }, \
  { WD_LEVEL_RELINK_TX, "WD_LEVEL_RELINK_TX" }, \
  { WD_LEVEL_PEER_AP_DISC_TX, "WD_LEVEL_PEER_AP_DISC_TX" }, \
  { WD_LEVEL_PEER_AP_REMOVE_TX, "WD_LEVEL_PEER_AP_REMOVE_TX" }, \
  { WD_LEVEL_PEER_CLIENT_DISC_TX, "WD_LEVEL_PEER_CLIENT_DISC_TX" }, \
  { WD_LEVEL_PEER_CLIENT_REMOVE_TX, "WD_LEVEL_PEER_CLIENT_REMOVE_TX" }, \
  { WD_LEVEL_INITIAL_ID_TX, "WD_LEVEL_INITIAL_ID_TX" }, \
  { WD_LEVEL_CLIENT_PRE_AUTH_TX, "WD_LEVEL_CLIENT_PRE_AUTH_TX" }, \
  { WD_LEVEL_CLIENT_PMK_AUTH_TX, "WD_LEVEL_CLIENT_PMK_AUTH_TX" }, \
  { WD_LEVEL_DISS_CLIENT_TX, "WD_LEVEL_DISS_CLIENT_TX" }, \
  { WD_LEVEL_AP_CONFIG_TX, "WD_LEVEL_AP_CONFIG_TX" }, \
  { WD_LEVEL_STATS_INTVL_TX, "WD_LEVEL_STATS_INTVL_TX" }, \
  { WD_LEVEL_SCAN_INTVL_TX, "WD_LEVEL_SCAN_INTVL_TX" }, \
  { WD_LEVEL_STATS_CLEAR_TX, "WD_LEVEL_STATS_CLEAR_TX" }, \
  { WD_LEVEL_RF_SCAN_REPORT_INTVL_TX, "WD_LEVEL_RF_SCAN_REPORT_INTVL_TX" }, \
  { WD_LEVEL_AP_AUTH_REQ_TX, "WD_LEVEL_AP_AUTH_REQ_TX" }, \
  { WD_LEVEL_AP_AUTH_SUCCEEDED_TX, "WD_LEVEL_AP_AUTH_SUCCEEDED_TX" }, \
  { WD_LEVEL_NEW_ARP_TX, "WD_LEVEL_NEW_ARP_TX" }, \
  { WD_LEVEL_DEBUG_LOGIN_TX, "WD_LEVEL_DEBUG_LOGIN_TX" }, \
  { WD_LEVEL_START_AUTH_TX, "WD_LEVEL_START_AUTH_TX" }, \
  { WD_LEVEL_NETWORK_PACKET_TX, "WD_LEVEL_NETWORK_PACKET_TX" }, \
  { WD_LEVEL_RESEND_CLIENT_INFO_TX, "WD_LEVEL_RESEND_CLIENT_INFO_TX" }, \
  { WD_LEVEL_CONFIG_PUSH_TX, "WD_LEVEL_CONFIG_PUSH_TX" }, \
  { WD_LEVEL_L2DT_SETUP_TX, "WD_LEVEL_L2DT_SETUP_TX" }, \
  { WD_LEVEL_L2DT_TERMINATE_TX, "WD_LEVEL_L2DT_TERMINATE_TX" }, \
  { WD_LEVEL_L2DT_CONFIG_TX, "WD_LEVEL_L2DT_CONFIG_TX" }, \
  { WD_LEVEL_L2DT_PACKET_TX, "WD_LEVEL_L2DT_PACKET_TX" }, \
  { WD_LEVEL_L2DT_ASSOC_REPLY_TX, "WD_LEVEL_L2DT_ASSOC_REPLY_TX" }, \
  { WD_LEVEL_TSPEC_PKT_TX, "WD_LEVEL_TSPEC_PKT_TX" }, \
  { WS_LEVEL_PROVISIONING_TX, "WS_LEVEL_PROVISIONING_TX" }, \
  { WD_LEVEL_TX_LAST, "WD_LEVEL_TX_LAST" }, \
  { WD_LEVEL_RX_FIRST, "WD_LEVEL_RX_FIRST" }, \
  { WD_LEVEL_AUTH_RX, "WD_LEVEL_AUTH_RX" }, \
  { WD_LEVEL_DISCOVERY_IP_RX, "WD_LEVEL_DISCOVERY_IP_RX" }, \
  { WD_LEVEL_DISCOVERY_L2_RX, "WD_LEVEL_DISCOVERY_L2_RX" }, \
  { WD_LEVEL_KEEP_ALIVE_RX, "WD_LEVEL_KEEP_ALIVE_RX" }, \
  { WD_LEVEL_KEEP_ALIVE_ACK_RX, "WD_LEVEL_KEEP_ALIVE_ACK_RX" }, \
  { WD_LEVEL_RELINK_RX, "WD_LEVEL_RELINK_RX" }, \
  { WD_LEVEL_CONFIG_STATUS_RX, "WD_LEVEL_CONFIG_STATUS_RX" }, \
  { WD_LEVEL_SCAN_REPORT_RX, "WD_LEVEL_SCAN_REPORT_RX" }, \
  { WD_LEVEL_PROBE_REQ_RX, "WD_LEVEL_PROBE_REQ_RX" }, \
  { WD_LEVEL_CLIENT_STATS_RX, "WD_LEVEL_CLIENT_STATS_RX" }, \
  { WD_LEVEL_CLIENT_ASSOC_RX, "WD_LEVEL_CLIENT_ASSOC_RX" }, \
  { WD_LEVEL_CLIENT_DISS_RX, "WD_LEVEL_CLIENT_DISS_RX" }, \
  { WD_LEVEL_CLIENT_ASSOC_FAILED_RX, "WD_LEVEL_CLIENT_ASSOC_FAILED_RX" }, \
  { WD_LEVEL_PEER_AP_DISC_RX, "WD_LEVEL_PEER_AP_DISC_RX" }, \
  { WD_LEVEL_PEER_AP_REMOVE_RX, "WD_LEVEL_PEER_AP_REMOVE_RX" }, \
  { WD_LEVEL_PEER_CLIENT_DISC_RX, "WD_LEVEL_PEER_CLIENT_DISC_RX" }, \
  { WD_LEVEL_PEER_CLIENT_REMOVE_RX, "WD_LEVEL_PEER_CLIENT_REMOVE_RX" }, \
  { WD_LEVEL_INITIAL_ID_RX, "WD_LEVEL_INITIAL_ID_RX" }, \
  { WD_LEVEL_CLIENT_PRE_AUTH_RX, "WD_LEVEL_CLIENT_PRE_AUTH_RX" }, \
  { WD_LEVEL_CLIENT_PMK_AUTH_RX, "WD_LEVEL_CLIENT_PMK_AUTH_RX" }, \
  { WD_LEVEL_CLIENT_AUTH_NOTIFY_RX, "WD_LEVEL_CLIENT_AUTH_NOTIFY_RX" }, \
  { WD_LEVEL_DISS_CLIENT_RX, "WD_LEVEL_DISS_CLIENT_RX" }, \
  { WD_LEVEL_NETWORK_PACKET_RX, "WD_LEVEL_NETWORK_PACKET_RX" }, \
  { WD_LEVEL_UPDATE_STATUS_RX, "WD_LEVEL_UPDATE_STATUS_RX" }, \
  { WD_LEVEL_AP_STATS_RX, "WD_LEVEL_AP_STATS_RX" }, \
  { WD_LEVEL_CONFIG_PUSH_RX, "WD_LEVEL_CONFIG_PUSH_RX" }, \
  { WD_LEVEL_CLIENT_CONN_RX, "WD_LEVEL_CLIENT_CONN_RX" }, \
  { WD_LEVEL_L2DT_SETUP_RX, "WD_LEVEL_L2DT_SETUP_RX" }, \
  { WD_LEVEL_L2DT_TERMINATE_RX, "WD_LEVEL_L2DT_TERMINATE_RX" }, \
  { WD_LEVEL_L2DT_PACKET_RX, "WD_LEVEL_L2DT_PACKET_RX" }, \
  { WD_LEVEL_L2DT_ASSOC_RX, "WD_LEVEL_L2DT_ASSOC_RX" }, \
  { WD_LEVEL_L2DT_AUTH_RX, "WD_LEVEL_L2DT_AUTH_RX" }, \
  { WD_LEVEL_L2DT_PEER_DISC_RX, "WD_LEVEL_L2DT_PEER_DISC_RX" }, \
  { WD_LEVEL_TSPEC_PKT_RX, "WD_LEVEL_TSPEC_PKT_RX" }, \
  { WD_LEVEL_PROVISIONING_RX, "WD_LEVEL_PROVISIONING_RX" }, \
  { WD_LEVEL_RX_LAST, "WD_LEVEL_RX_LAST" }, \
  { WD_LEVEL_WDM_FIRST, "WD_LEVEL_WDM_FIRST" }, \
  { WD_LEVEL_WDM, "WD_LEVEL_WDM" }, \
  { WD_LEVEL_WDM_DATA_ERROR, "WD_LEVEL_WDM_DATA_ERROR" }, \
  { WD_LEVEL_AUTH_WDM, "WD_LEVEL_AUTH_WDM" }, \
  { WD_LEVEL_WDM_LAST, "WD_LEVEL_WDM_LAST" }, \
  { WD_LEVEL_COMPONENT_FIRST, "WD_LEVEL_COMPONENT_FIRST" }, \
  { WD_LEVEL_AUTH_TRACE, "WD_LEVEL_AUTH_TRACE" }, \
  { WD_LEVEL_AUTH_QUEUE_TRACE, "WD_LEVEL_AUTH_QUEUE_TRACE" }, \
  { WD_LEVEL_AUTH_ERROR, "WD_LEVEL_AUTH_ERROR" }, \
  { WD_LEVEL_AUTH_PKT, "WD_LEVEL_AUTH_PKT" }, \
  { WD_LEVEL_AUTH_RADIUS, "WD_LEVEL_AUTH_RADIUS" }, \
  { WD_LEVEL_AUTH_RADIUS_OTHER, "WD_LEVEL_AUTH_RADIUS_OTHER" }, \
  { WD_LEVEL_WSAP_CONN, "WD_LEVEL_WSAP_CONN" }, \
  { WD_LEVEL_WSAP_COMM_TCP_PKT_TX, "WD_LEVEL_WSAP_COMM_TCP_PKT_TX" }, \
  { WD_LEVEL_WSAP_COMM_TCP_PKT_RX, "WD_LEVEL_WSAP_COMM_TCP_PKT_RX" }, \
  { WD_LEVEL_WSAP_COMM_UDP_PKT_TX, "WD_LEVEL_WSAP_COMM_UDP_PKT_TX" }, \
  { WD_LEVEL_WSAP_COMM_UDP_PKT_RX, "WD_LEVEL_WSAP_COMM_UDP_PKT_RX" }, \
  { WD_LEVEL_WSAP_COMM_TNL_UDP_PKT_TX, "WD_LEVEL_WSAP_COMM_TNL_UDP_PKT_TX" }, \
  { WD_LEVEL_WSAP_COMM_TNL_UDP_PKT_RX, "WD_LEVEL_WSAP_COMM_TNL_UDP_PKT_RX" }, \
  { WD_LEVEL_WSAP_COMM_L2_PKT_TX, "WD_LEVEL_WSAP_COMM_L2_PKT_TX" }, \
  { WD_LEVEL_WSAP_COMM_L2_PKT_RX, "WD_LEVEL_WSAP_COMM_L2_PKT_RX" }, \
  { WD_LEVEL_WSAP_COMM_INFO, "WD_LEVEL_WSAP_COMM_INFO" }, \
  { WD_LEVEL_DISC_PKT, "WD_LEVEL_DISC_PKT" }, \
  { WD_LEVEL_DISC_INFO, "WD_LEVEL_DISC_INFO" }, \
  { WD_LEVEL_WSAP_STATS_INFO, "WD_LEVEL_WSAP_STATS_INFO" }, \
  { WD_LEVEL_WSAP_STATS_PKT, "WD_LEVEL_WSAP_STATS_PKT" }, \
  { WD_LEVEL_CLIENTASSOCLB_PKT, "WD_LEVEL_CLIENTASSOCLB_PKT" }, \
  { WD_LEVEL_CLIENTASSOCLB_INFO, "WD_LEVEL_CLIENTASSOCLB_INFO" }, \
  { WD_LEVEL_RF_SCAN_CACHED_RADIUS, "WD_LEVEL_RF_SCAN_CACHED_RADIUS" }, \
  { WD_LEVEL_KNOWN_CLIENT_CACHED_RADIUS, "WD_LEVEL_KNOWN_CLIENT_CACHED_RADIUS" }, \
  { WD_LEVEL_WSAP_CONF_INFO, "WD_LEVEL_WSAP_CONF_INFO" }, \
  { WD_LEVEL_WSAP_CONF_PKT, "WD_LEVEL_WSAP_CONF_PKT" }, \
  { WD_LEVEL_WSAP_UPD_INFO, "WD_LEVEL_WSAP_UPD_INFO" }, \
  { WD_LEVEL_WSAP_RESET_INFO, "WD_LEVEL_WSAP_RESET_INFO" }, \
  { WD_LEVEL_SSL_TRACE, "WD_LEVEL_SSL_TRACE" }, \
  { WD_LEVEL_SSL_INFO, "WD_LEVEL_SSL_INFO" }, \
  { WD_LEVEL_SSL_ERROR, "WD_LEVEL_SSL_ERROR" }, \
  { WD_LEVEL_SSL_LIB, "WD_LEVEL_SSL_LIB" }, \
  { WD_LEVEL_SSL_LIBSEM, "WD_LEVEL_SSL_LIBSEM" }, \
  { WD_LEVEL_SSL_CONN, "WD_LEVEL_SSL_CONN" }, \
  { WD_LEVEL_SSL_RX, "WD_LEVEL_SSL_RX" }, \
  { WD_LEVEL_SSL_TX, "WD_LEVEL_SSL_TX" }, \
  { WD_LEVEL_SSL_TMR_INFO, "WD_LEVEL_SSL_TMR_INFO" }, \
  { WD_LEVEL_WTNNL_INFO, "WD_LEVEL_WTNNL_INFO" }, \
  { WD_LEVEL_WTNNL_ERROR, "WD_LEVEL_WTNNL_ERROR" }, \
  { WD_LEVEL_WTNNL_ARP, "WD_LEVEL_WTNNL_ARP" }, \
  { WD_LEVEL_WTNNL_DHCP, "WD_LEVEL_WTNNL_DHCP" }, \
  { WD_LEVEL_WTNNL_MCAST, "WD_LEVEL_WTNNL_MCAST" }, \
  { WD_LEVEL_NV_INFO, "WD_LEVEL_NV_INFO" }, \
  { WD_LEVEL_NV_ERROR, "WD_LEVEL_NV_ERROR" }, \
  { WD_LEVEL_CP_INFO, "WD_LEVEL_CP_INFO" }, \
  { WD_LEVEL_CP_INFO2, "WD_LEVEL_CP_INFO2" }, \
  { WD_LEVEL_CP_POWER_EVENT, "WD_LEVEL_CP_POWER_EVENT" }, \
  { WD_LEVEL_CP_ERROR, "WD_LEVEL_CP_ERROR" }, \
  { WD_LEVEL_CP_WDM_ERROR, "WD_LEVEL_CP_WDM_ERROR" }, \
  { WD_LEVEL_CR_INFO, "WD_LEVEL_CR_INFO" }, \
  { WD_LEVEL_CONFIG_PUSH_PKT_RX, "WD_LEVEL_CONFIG_PUSH_PKT_RX" }, \
  { WD_LEVEL_CONFIG_PUSH_PKT_TX, "WD_LEVEL_CONFIG_PUSH_PKT_TX" }, \
  { WD_LEVEL_CONFIG_PUSH_INFO, "WD_LEVEL_CONFIG_PUSH_INFO" }, \
  { WD_LEVEL_WIDS_PKT_RX, "WD_LEVEL_WIDS_PKT_RX" }, \
  { WD_LEVEL_WIDS_PKT_TX, "WD_LEVEL_WIDS_PKT_TX" }, \
  { WD_LEVEL_WIDS_INFO, "WD_LEVEL_WIDS_INFO" }, \
  { WD_LEVEL_WIDS_AP_INFO, "WD_LEVEL_WIDS_AP_INFO" }, \
  { WD_LEVEL_WIDS_RADIO_VAP_INFO, "WD_LEVEL_WIDS_RADIO_VAP_INFO" }, \
  { WD_LEVEL_WIDS_AP_FAIL_INFO, "WD_LEVEL_WIDS_AP_FAIL_INFO" }, \
  { WD_LEVEL_WIDS_CLIENT_INFO, "WD_LEVEL_WIDS_CLIENT_INFO" }, \
  { WD_LEVEL_WIDS_PURGE_AP_INFO, "WD_LEVEL_WIDS_PURGE_AP_INFO" }, \
  { WD_LEVEL_THREAT_CLASSIFICATION, "WD_LEVEL_THREAT_CLASSIFICATION" }, \
  { WD_LEVEL_CLIENT_SECURITY, "WD_LEVEL_CLIENT_SECURITY" }, \
  { WD_LEVEL_WIDS_CONFIG_MSG_TX, "WD_LEVEL_WIDS_CONFIG_MSG_TX" }, \
  { WD_LEVEL_CLN_THREAT_MITIGATION, "WD_LEVEL_CLN_THREAT_MITIGATION" }, \
  { WD_LEVEL_RADIUS_PROXY_CMD_INFO, "WD_LEVEL_RADIUS_PROXY_CMD_INFO" }, \
  { WD_LEVEL_RADIUS_PROXY_CMD_TX, "WD_LEVEL_RADIUS_PROXY_CMD_TX" }, \
  { WD_LEVEL_RADIUS_PROXY_CMD_RX, "WD_LEVEL_RADIUS_PROXY_CMD_RX" }, \
  { WD_LEVEL_RADIUS_PROXY_CMD_PKT, "WD_LEVEL_RADIUS_PROXY_CMD_PKT" }, \
  { WD_LEVEL_CENTTNNL_INFO, "WD_LEVEL_CENTTNNL_INFO" }, \
  { WD_LEVEL_CENTTNNL_ERROR, "WD_LEVEL_CENTTNNL_ERROR" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_INFO, "WD_LEVEL_WSAP_CONF_CLTQOS_INFO" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_ERROR, "WD_LEVEL_WSAP_CONF_CLTQOS_ERROR" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_CALLBACK, "WD_LEVEL_WSAP_CONF_CLTQOS_CALLBACK" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_FORMATTED_MSG, "WD_LEVEL_WSAP_CONF_CLTQOS_FORMATTED_MSG" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_LIST_UPDATE_PKT_TX, "WD_LEVEL_WSAP_CONF_CLTQOS_LIST_UPDATE_PKT_TX" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_LIST_RENAME_PKT_TX, "WD_LEVEL_WSAP_CONF_CLTQOS_LIST_RENAME_PKT_TX" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_LIST_DELETE_PKT_TX, "WD_LEVEL_WSAP_CONF_CLTQOS_LIST_DELETE_PKT_TX" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_DEF_REQ_PKT_RX, "WD_LEVEL_WSAP_CONF_CLTQOS_DEF_REQ_PKT_RX" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_ACL_DEF_REPLY_PKT_TX, "WD_LEVEL_WSAP_CONF_CLTQOS_ACL_DEF_REPLY_PKT_TX" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_POLICY_DEF_REPLY_PKT_TX, "WD_LEVEL_WSAP_CONF_CLTQOS_POLICY_DEF_REPLY_PKT_TX" }, \
  { WD_LEVEL_WSAP_CONF_CLTQOS_MODE_UPDATE_PKT_TX, "WD_LEVEL_WSAP_CONF_CLTQOS_MODE_UPDATE_PKT_TX" }, \
  { WD_LEVEL_TSPEC_INFO, "WD_LEVEL_TSPEC_INFO" }, \
  { WD_LEVEL_TSPEC_ERROR, "WD_LEVEL_TSPEC_ERROR" }, \
  { WD_LEVEL_TSPEC_PKT, "WD_LEVEL_TSPEC_PKT" }, \
  { WD_LEVEL_TSPEC_DETAIL, "WD_LEVEL_TSPEC_DETAIL" }, \
  { WD_LEVEL_TSPEC_TASK_INFO, "WD_LEVEL_TSPEC_TASK_INFO" }, \
  { WD_LEVEL_TSPEC_MGMT_ACTION_INFO, "WD_LEVEL_TSPEC_MGMT_ACTION_INFO" }, \
  { WD_LEVEL_TSPEC_MGMT_ACTION_PKT, "WD_LEVEL_TSPEC_MGMT_ACTION_PKT" }, \
  { WD_LEVEL_TSPEC_MGMT_ACTION_DETAIL, "WD_LEVEL_TSPEC_MGMT_ACTION_DETAIL" }, \
  { WD_LEVEL_TSPEC_ACU_INFO, "WD_LEVEL_TSPEC_ACU_INFO" }, \
  { WD_LEVEL_TSPEC_ACU_DETAIL, "WD_LEVEL_TSPEC_ACU_DETAIL" }, \
  { WD_LEVEL_WSAP_MUTUAL_AUTH_INFO, "WD_LEVEL_WSAP_MUTUAL_AUTH_INFO" }, \
  { WD_LEVEL_WSAP_MUTUAL_AUTH_ERROR, "WD_LEVEL_WSAP_MUTUAL_AUTH_ERROR" }, \
  { WD_LEVEL_WSAP_MUTUAL_AUTH_PKT, "WD_LEVEL_WSAP_MUTUAL_AUTH_PKT" }, \
  { WD_LEVEL_WS_PROVISION_INFO, "WD_LEVEL_WS_PROVISION_INFO" }, \
  { WD_LEVEL_WS_PROVISION_ERROR, "WD_LEVEL_WS_PROVISION_ERROR" }, \
  { WD_LEVEL_WS_PROVISION_PKT, "WD_LEVEL_WS_PROVISION_PKT" }, \
  { WD_LEVEL_COMPONENT_LAST, "WD_LEVEL_COMPONENT_LAST" }, \
  { WD_LEVEL_LAST, "WD_LEVEL_LAST" } \
}

/*----------------------------------------*/
/*  End Wireless System Logging Options   */
/*----------------------------------------*/
#define L7_WDM_RATE_STRING_MAX  9 /* 8+NULL */
 /*XUI : Added for Compilation. Confirm and change later(Radha K) */

#define L7_DATA_RATE_MULTIPLIER 5

#define L7_MINUTES    (60)
#define L7_HOURS      ((60) * (60))
#define L7_DAYS       (((60) * (60)) * (24))

#define L7_TEN        10
#define L7_HUNDRED    100
#define L7_THOUSAND   1000
#define L7_TENTHOUSAND 10000

typedef enum
{
  L7_WDM_MANUAL_ACTION_NONE = 0,
  L7_WDM_MANUAL_ACTION_START,
  L7_WDM_MANUAL_ACTION_APPLY,
  L7_WDM_MANUAL_ACTION_CLEAR
}L7_WDM_MANUAL_ACTION_t;

typedef enum
{
  L7_WDM_SWITCH_LOCAL = 0,
  L7_WDM_SWITCH_PEER
}L7_WDM_SWITCH_TYPE_t;

typedef enum
{
  L7_WDM_ROAM_STATUS_ASSOCIATED = 0,
  L7_WDM_ROAM_STATUS_HOME
}L7_WDM_ROAM_STATUS_t;

typedef enum
{
  L7_WDM_NO_ACTION = 0,
  L7_WDM_ACK
}L7_WDM_ACK_t;

typedef enum
{
  L7_WDM_ROGUE_NOT_REPORTED = 0,
  L7_WDM_ROGUE_REPORTED
}L7_WDM_ROGUE_REPORT_t;

typedef enum
{
  L7_HW_BROADCOM = 0,
  L7_HW_OTHERS  
}L7_HW_TYPE_t;

typedef enum
{
  L7_NOT_DETECTED = 0,
  L7_DETECTED
}L7_DETECTION_STATE_t;

typedef enum
{
  L7_VENDOR_BROADCOM = 0,
  L7_VENDOR_OTHERS
}L7_VENDOR_ID_t;

typedef enum
{
  L7_UNSUPPORTED = 0,
  L7_SUPPORTED
}L7_SUPPORT_t;

typedef enum
{
  L7_NOT_ADHOC = 0,
  L7_ADHOC
}L7_ADHOC_t;


#endif /* INCLUDE_WIRELESS_COMMDEFS */
