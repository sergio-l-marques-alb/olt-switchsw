/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename wireless_defaultconfig.h
*
* @purpose All wireless system default configurations are in this file.
*
* @component none
*
* @comments This file contains default values for end user configuration
*           parameters.  These values are used to build default configuration
*           files for components.
*
* @create 01/30/2006
*
* @author dfowler
* @end
*
**********************************************************************/
#ifndef INCLUDE_WIRELESS_CONFIG_DEFAULTS
#define INCLUDE_WIRELESS_CONFIG_DEFAULTS

#include "datatypes.h"
#include "l7_ip_priv_port.h"


/*-------------------------------------*/
/* Start Wireless Global Defaults      */
/*-------------------------------------*/

#define FD_WDM_DEFAULT_WS_MODE                    L7_DISABLE
#define FD_WDM_DEFAULT_WS_COUNTRY_CODE            "US"
#define FD_WDM_DEFAULT_WS_PEER_GROUP_ID           1
#define FD_WDM_DEFAULT_WS_TUNNEL_MTU              1500
#define FD_WDM_DEFAULT_WS_IP_DISCOVERY            L7_ENABLE
#define FD_WDM_DEFAULT_WS_L2_DISCOVERY            L7_ENABLE
#define FD_WDM_DEFAULT_WS_RF_DISCOVERY            L7_ENABLE
#define FD_WDM_DEFAULT_WS_AP_VALIDATION           L7_WDM_AP_VALIDATION_LOCAL
#define FD_WDM_DEFAULT_WS_AP_AUTHENTICATION       L7_DISABLE
#define FD_WDM_DEFAULT_WS_AP_AUTO_UPGRADE         (((cnfgrIsFeaturePresent(L7_FLEX_WIRELESS_COMPONENT_ID,                \
                                                          L7_WIRELESS_AP_IMAGE_MODE_INTEGRATED_FEATURE_ID)==L7_TRUE) &&  \
                                                    (cnfgrIsFeaturePresent(L7_FLEX_WIRELESS_COMPONENT_ID,                \
                                                          L7_WIRELESS_AP_IMAGE_MODE_INDEPENDENT_FEATURE_ID)!=L7_TRUE))   \
                                                   ? (L7_ENABLE) : (L7_DISABLE))
#define FD_WDM_DEFAULT_WS_AUTO_IP_ASSIGN_MODE     L7_ENABLE
#define FD_WDM_DEFAULT_WS_CERT_GENERATE_MODE      L7_FALSE;
#define FD_WDM_DEFAULT_WS_SWITCH_PROVISIONING     L7_ENABLE 
#define FD_WDM_DEFAULT_WS_AP_PROVISIONING_ENTRY_TIMEOUT 72
#define FD_WDM_DEFAULT_WS_NETWORK_MUTUAL_AUTHENTICATION_MODE L7_DISABLE
#define FD_WDM_DEFAULT_WS_AP_REPROVISIONING_MODE  L7_ENABLE
#define FD_WDM_DEFAULT_WS_MUT_AUTH_EXCHANGE_CERTIFICATES L7_DISABLE

#define FD_WDM_DEFAULT_WS_AP_FAILURE_TRAP         L7_DISABLE
#define FD_WDM_DEFAULT_WS_TRAP                    L7_DISABLE
#define FD_WDM_DEFAULT_PEER_WS_TRAP               L7_DISABLE
#define FD_WDM_DEFAULT_WS_ROGUE_AP_TRAP           L7_DISABLE
#define FD_WDM_DEFAULT_WS_AP_STATE_TRAP           L7_DISABLE
#define FD_WDM_DEFAULT_WS_CLIENT_STATE_TRAP       L7_DISABLE
#define FD_WDM_DEFAULT_RF_SCAN_TRAP               L7_DISABLE
#define FD_WDM_DEFAULT_WS_TSPEC_TRAP              L7_DISABLE
#define FD_WDM_DEFAULT_WS_WIDS_TRAP               L7_DISABLE
#define FD_WDM_DEFAULT_WS_CLIENT_FAILURE_TRAP     L7_DISABLE
#define FD_WDM_DEFAULT_WS_IMAGE_IP_ADDRESS        0
#define FD_WDM_DEFAULT_WS_IMAGE_FILENAME          ""
#define FD_WDM_DEFAULT_WS_AD_HOC_AGE              24
#define FD_WDM_DEFAULT_WS_AP_FAILURE_AGE          24
#define FD_WDM_DEFAULT_WS_CLIENT_FAILURE_AGE      24
#define FD_WDM_DEFAULT_WS_CLIENT_ROAM_AGE         30
#define FD_WDM_DEFAULT_WS_RF_SCAN_AGE             24
#define FD_WDM_DEFAULT_WS_DETECTED_CLIENTS_AGE    24
#define FD_WDM_AP_CODE_DOWNLOAD_GROUP_SIZE        10
#define FD_WDM_CHANNEL_PLAN_MODE                  L7_WDM_CHANNEL_PLAN_MANUAL
#define FD_WDM_CHANNEL_PLAN_INTERVAL              6
#define FD_WDM_CHANNEL_PLAN_TIME                  0  /* 24 hour time, midnight */
#define FD_WDM_CHANNEL_PLAN_HIST_DEPTH            5
#define FD_WDM_POWER_ADJ_MODE                     L7_WDM_POWER_ADJ_MANUAL
#define FD_WDM_POWER_ADJ_INTERVAL                 L7_WDM_POWER_ADJ_INTERVAL_MIN
#define FD_WDM_DEFAULT_NV_SHOW_POWER              L7_DISABLE
#define FD_WDM_DEFAULT_NV_SHOW_TAB_VIEW           L7_DISABLE
#define FD_WDM_DEFAULT_NV_SHOW_MAPS               L7_ENABLE
#define FD_WDM_DEFAULT_NV_SHOW_RAPS               L7_ENABLE
#define FD_WDM_DEFAULT_NV_SHOW_MAP_CLIENTS        L7_ENABLE
#define FD_WDM_DEFAULT_NV_SHOW_DETECTED_CLIENTS   L7_ENABLE
#define FD_WDM_DEFAULT_NV_SHOW_LOCATION_FLAG      L7_ENABLE
#define FD_WDM_DEFAULT_NV_CFG_MIGRATE             L7_FALSE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_GLOBAL_CONFIG               L7_ENABLE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_DISCOVERY_CONFIG            L7_DISABLE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_VALID_AP_DATABASE_CONFIG    L7_ENABLE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_CHNL_PWR_CONFIG             L7_ENABLE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_AP_PROFILE_CONFIG           L7_ENABLE
#define FD_WDM_DEFAULT_WS_AP_CLIENT_QOS_MODE                      L7_DISABLE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_KNOWN_CLIENT                L7_ENABLE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_WDS_GROUP                   L7_ENABLE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_DEV_LOC_CONFIG              L7_ENABLE
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_CAPTIVE_PORTAL_CONFIG       L7_ENABLE
#endif
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_RADIUS_CONFIG               L7_ENABLE
#ifdef L7_ACL_PACKAGE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_QOS_ACL                     L7_ENABLE
#endif
#ifdef L7_DIFFSERV_PACKAGE
#define FD_WDM_DEFAULT_WS_CONFIG_PUSH_QOS_DIFFSERV                L7_ENABLE
#endif

#define FD_WDM_DEFAULT_WS_WIDS_PRIORITY                                     1
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_ADMIN_CONFIGURED                L7_ENABLE        
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_MANAGED_SSID_UNKNOWN_AP         L7_ENABLE 
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_MANAGED_SSID_FAKE_MANAGED_AP    L7_ENABLE
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_FAKE_MANAGED_NO_SSID            L7_ENABLE 
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_FAKE_MANAGED_CHAN_INVALID       L7_ENABLE        
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_MANAGED_SSID_SECURITY_INVALID   L7_ENABLE        
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_MANAGED_AP_SSID_INVALID         L7_ENABLE        
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_CHANNEL_ILLEGAL                 L7_ENABLE        
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_STANDALONE_CONFIG_INVALID       L7_ENABLE        
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_WDS_DEVICE_INVALID              L7_ENABLE        
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECT_WIRED_UNMANAGED_AP              L7_ENABLE        
#define FD_WDM_DEFAULT_WS_WIDS_WIRED_DETECTION_INTERVAL                     60   
#define FD_WDM_DEFAULT_WS_WIDS_ROGUE_DETECTION_TRAP_INTERVAL                300         
#define FD_WDM_DEFAULT_WS_WIDS_DE_AUTHENTICATION_ATTACK                     L7_DISABLE        

/* Client Security Default configurations */
#define FD_WDM_DEFAULT_WS_WIDS_CLIENT_ROGUE_DET_TRAP_INTERVAL               300
#define FD_WDM_DEFAULT_WS_WIDS_CLIENT_NOT_IN_KNOWN_DB_CONFIGURED            L7_DISABLE
#define FD_WDM_DEFAULT_WS_WIDS_CLIENT_NOT_IN_OUI_DB_CONFIGURED            L7_DISABLE
#define FD_WDM_DEFAULT_WS_WIDS_CLIENT_AUTH_RATE_CONFIGURED                  L7_ENABLE
#define FD_WDM_DEFAULT_WS_WIDS_CLIENT_PROBE_RATE_CONFIGURED                 L7_ENABLE
#define FD_WDM_DEFAULT_WS_WIDS_CLIENT_DEAUTH_RATE_CONFIGURED                L7_ENABLE
#define FD_WDM_DEFAULT_WS_WIDS_CLIENT_AUTH_FAILURES_CONFIGURED              L7_ENABLE
#define FD_WDM_DEFAULT_WS_WIDS_CLIENT_AUTH_TO_UNKNOWN_AP_CONFIGURED         L7_DISABLE
#define FD_WDM_DEFAULT_WS_WIDS_CLIENT_THREAT_MITIGATION_CONFIGURED          L7_DISABLE
#define FD_WDM_DEFAULT_WS_WIDS_DEAUTH_THRESHOLD_INTERVAL                    60
#define FD_WDM_DEFAULT_WS_WIDS_DEAUTH_THRESHOLD_VAL                         10
#define FD_WDM_DEFAULT_WS_WIDS_AUTH_THRESHOLD_INTERVAL                      60
#define FD_WDM_DEFAULT_WS_WIDS_AUTH_THRESHOLD_VAL                           10
#define FD_WDM_DEFAULT_WS_WIDS_PROBE_THRESHOLD_INTERVAL                     60
#define FD_WDM_DEFAULT_WS_WIDS_PROBE_THRESHOLD_VAL                          120
#define FD_WDM_DEFAULT_WS_WIDS_AUTH_FAIL_THRESHOLD_VAL                      5
#define FD_WDM_DEFAULT_WS_WIDS_KNOWN_DB_LOCATION                            L7_WDM_DETECTED_CLIENT_KNOWN_DATABASE_LOCAL

#define FD_WDM_DEFAULT_WS_RADIUS_SERVER_NAME            "Default-RADIUS-Server"
#define FD_WDM_DEFAULT_WS_RADIUS_ACCT_MODE              L7_DISABLE
#define FD_WDM_DEFAULT_WS_USE_NETWORK_RADIUS            L7_ENABLE

#define FD_WDM_DIST_TUNNEL_IDLE_TIMEOUT  120
#define FD_WDM_DIST_TUNNEL_MAX_TIMEOUT   7200
#define FD_WDM_DIST_TUNNEL_MCAST_REPL    128
#define FD_WDM_DIST_TUNNEL_MAX_CLIENTS   128
#define FD_WDM_DIST_TUNNEL_MODE          L7_DISABLE

#define FD_WDM_NETWORK_BCAST_KEY_REFRESH_RATE      300
#define FD_WDM_NETWORK_SESSION_KEY_REFRESH_RATE    L7_WDM_NETWORK_SESSION_KEY_REFRESHRATE_DISABLE
#define FD_WDM_WIRELESS_ARP_SUPP_MODE    L7_DISABLE

/* TSPEC global config parm defaults */
#define FD_WDM_DEFAULT_WS_TSPEC_VIOLATE_REPORT_IVAL         300         /* seconds */

/* TSPEC build constant defaults */
#define FD_WDM_DEFAULT_WS_TSPEC_VIOLATE_THRESHOLD           5           /* consecutive averaging intervals */
#define FD_WDM_DEFAULT_WS_TSPEC_ACM_BW_MAX                  70          /* percent */

/* 802.11k RRM  build constant defaults */
#define FD_WDM_DEFAULT_RRM_CH_LOAD_REQ_TIME_MAX             60          /* seconds */

/*-------------------------------------*/
/* End Wireless Global Defaults        */
/*-------------------------------------*/

/*-------------------------------------*/
/* Start Local AP Database Defaults    */
/*-------------------------------------*/

#define FD_WDM_DEFAULT_AP_LOCATION                                ""
#define FD_WDM_DEFAULT_AP_MANAGED_MODE                            L7_WDM_AP_WS_MANAGED
#define FD_WDM_DEFAULT_AP_PASSWORD                                ""
#define FD_WDM_DEFAULT_AP_RADIO_CHANNEL                           L7_WDM_CHANNEL_AUTO
#define FD_WDM_DEFAULT_AP_RADIO_POWER                             L7_WDM_POWER_AUTO
#define FD_WDM_DEFAULT_AP_STANDALONE_EXPECTED_SSID                ""
#define FD_WDM_DEFAULT_AP_STANDALONE_EXPECTED_WDS_MODE            L7_WDM_AP_STANDALONE_WDS_MODE_ANY
#define FD_WDM_DEFAULT_AP_STANDALONE_EXPECTED_SECURITY_MODE       L7_WDM_AP_STANDALONE_SECURITY_MODE_ANY
#define FD_WDM_DEFAULT_AP_STANDALONE_EXPECTED_CHANNEL             L7_WDM_CHANNEL_AUTO
#define FD_WDM_DEFAULT_AP_STANDALONE_EXPECTED_WIRED_NETWORK_MODE  L7_WDM_AP_STANDALONE_WIRED_NETWORK_ALLOWED
#define FD_WDM_DEFAULT_WS_MAC_AUTH_MODE                           L7_WDM_WS_MACAUTH_WHITELIST   

/*--------------------------------------*/
/* Start Known Client Database Defaults */
/*--------------------------------------*/

#define FD_WDM_DEFAULT_KONWN_CLIENT_AUTH_ACTION		              L7_WDM_KNOWN_CLIENT_ACTION_GLOBAL
#define FD_WDM_DEFAULT_KNOWN_CLIENT_NICKNAME                      ""

/*-------------------------------------*/
/* End Local AP Database Defaults      */
/*-------------------------------------*/

/*-------------------------------------*/
/* Start Network Defaults              */
/*-------------------------------------*/

#define FD_WDM_DEFAULT_NETWORK_ID_MIN                               L7_WDM_VAPID_MIN+1  /* VAPs are 0 based */
#define FD_WDM_DEFAULT_NETWORK_ID_MAX                               L7_WDM_VAPID_MAX+1
#define FD_WDM_DEFAULT_NETWORK_GUEST_SSID                           "Guest Network"
#define FD_WDM_DEFAULT_NETWORK_SSID                                 "Managed SSID %d"
#define FD_WDM_DEFAULT_NETWORK_HIDE_SSID_MODE                       L7_DISABLE
#define FD_WDM_DEFAULT_NETWORK_DENY_BCAST_MODE                      L7_DISABLE
#define FD_WDM_DEFAULT_NETWORK_TUNNEL_MODE                          L7_DISABLE
#define FD_WDM_DEFAULT_NETWORK_TUNNEL_MASK                          0xFFFFFF00 /* 255.255.255.0 */
#define FD_WDM_DEFAULT_NETWORK_SECURITY_MODE                        L7_WDM_SECURITY_NONE
#define FD_WDM_DEFAULT_NETWORK_REDIRECT_MODE                        L7_WDM_REDIRECT_NONE
#define FD_WDM_DEFAULT_NETWORK_REDIRECT_URL                         ""
#define FD_WDM_DEFAULT_NETWORK_REDIRECT_IPADDR                      0
#define FD_WDM_DEFAULT_NETWORK_WPA_VERSIONS_SUPPORTED               L7_WDM_WPA_WPA2
#define FD_WDM_DEFAULT_NETWORK_WPA_CIPHER_SUITES                    L7_WDM_TKIP_CCMP
#define FD_WDM_DEFAULT_NETWORK_MAC_AUTH_MODE                        L7_WDM_MAC_AUTH_MODE_DISABLE
#define FD_WDM_DEFAULT_NETWORK_USE_AP_RADIUS_CONFIG                 L7_ENABLE
#define FD_WDM_DEFAULT_NETWORK_WEP_AUTH_MODE                        L7_WDM_WEP_AUTH_OPENSYSTEM
#define FD_WDM_DEFAULT_NETWORK_WEP_KEY_TYPE                         L7_WDM_HEX
#define FD_WDM_DEFAULT_NETWORK_WEP_KEY_LENGTH                       L7_WDM_WEP_128
#define FD_WDM_DEFAULT_NETWORK_WEP_KEY_TX_INDEX                     1
#define FD_WDM_DEFAULT_NETWORK_WPA_KEY_TYPE                         L7_WDM_ASCII
#define FD_WDM_DEFAULT_NETWORK_WPA2_PRE_AUTH_MODE                   L7_ENABLE
#define FD_WDM_DEFAULT_NETWORK_WPA2_PRE_AUTH_LIMIT                  0
#define FD_WDM_DEFAULT_NETWORK_WPA2_DY_KEY_FW_MODE                  L7_ENABLE
#define FD_WDM_DEFAULT_NETWORK_WPA2_ROAM_KEY_CACHE_HOLDTIME         10
#define FD_WDM_DEFAULT_NETWORK_CLIENT_QOS_MODE                      L7_DISABLE
#define FD_WDM_DEFAULT_NETWORK_CLIENT_BW_LIMIT                      L7_WDM_NETWORK_CLIENT_QOS_BW_NONE
#define FD_WDM_DEFAULT_NETWORK_CLIENT_ACL_TYPE                      L7_ACL_TYPE_NONE
#define FD_WDM_DEFAULT_NETWORK_CLIENT_ACL_NAME                      ""
#define FD_WDM_DEFAULT_NETWORK_CLIENT_DS_POLICY_TYPE                L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE
#define FD_WDM_DEFAULT_NETWORK_CLIENT_DS_POLICY_NAME                ""

/*-------------------------------------*/
/* End Network Defaults                */
/*-------------------------------------*/

/*-------------------------------------*/
/* Start AP Profile Defaults           */
/*-------------------------------------*/

#define FD_WDM_DEFAULT_AP_PROFILE_ID                          1
#define FD_WDM_DEFAULT_AP_PROFILE_NAME                        "Default"
#define FD_WDM_DEFAULT_AP_PROFILE_KEEP_ALIVE_INTERVAL         30
#define FD_WDM_DEFAULT_AP_PROFILE_DISCONN_AP_FWDING_MODE      L7_DISABLE
#define FD_WDM_DEFAULT_AP_PROFILE_DISCONN_AP_MGMT_MODE        L7_ENABLE
#define FD_WDM_DEFAULT_AP_PROFILE_RADIUS_SERVER_SECRET        ""
#define FD_WDM_DEFAULT_AP_PROFILE_RADIUS_ACCT_MODE            L7_DISABLE
#define FD_WDM_DEFAULT_AP_HW_TYPE                             0
#ifdef L7_AE_PROTOCOL_PACKAGE
#define FD_WDM_DEFAULT_AP_PROFILE_AE_MODE                     L7_DISABLE
#endif
/*-------------------------------------*/
/* Start WDS Managed AP Defaults       */
/*-------------------------------------*/
#define FD_WDM_DEFAULT_WDS_GROUP_ID_MAX     8
#define FD_WDM_DEFAULT_WDSAPGROUP_NAME "WDS-AP-Group-%d"
#define FD_WDM_DEFAULT_WDSAPGROUP_PASSWD "AP-Group-%d"

/*-------------------------------------*/
/* End WDS Managed AP Group Defaults   */
/*-------------------------------------*/


/* AP Profile Radio - Start */
#define FD_WDM_AP_PROFILE_RADIO_ADMIN_MODE                  L7_ENABLE
#define FD_WDM_AP_PROFILE_RADIO_1_PHY_N_MODE                L7_WDM_RADIO_PHY_MODE_802_11BGN
#define FD_WDM_AP_PROFILE_RADIO_2_PHY_N_MODE                L7_WDM_RADIO_PHY_MODE_802_11AN
#define FD_WDM_AP_PROFILE_RADIO_1_PHY                       L7_WDM_RADIO_PHY_MODE_802_11BG
#define FD_WDM_AP_PROFILE_RADIO_2_PHY                       L7_WDM_RADIO_PHY_MODE_802_11A
#define FD_WDM_AP_PROFILE_RADIO_RF_SENTRY_SCAN_MODE         L7_DISABLE
#define FD_WDM_AP_PROFILE_RADIO_RF_OTHER_CHANNEL_SCAN_MODE  L7_ENABLE
#define FD_WDM_AP_PROFILE_RADIO_RF_SCAN_FREQUENCY           L7_WDM_RADIO_SCAN_FREQUENCY_ALL
#define FD_WDM_AP_PROFILE_RADIO_RF_SCAN_INTERVAL            60
#define FD_WDM_AP_PROFILE_RADIO_RF_SCAN_DURATION_ACTIVE     10
#define FD_WDM_AP_PROFILE_RADIO_RF_SCAN_DURATION_SENTRY     2000
#define FD_WDM_AP_PROFILE_RADIO_RATE_LIMIT                  L7_DISABLE
#define FD_WDM_AP_PROFILE_RADIO_RATE_LIMIT_NORMAL           50
#define FD_WDM_AP_PROFILE_RADIO_RATE_LIMIT_BURST            75
#define FD_WDM_AP_PROFILE_RADIO_BEACON_INTERVAL             100
#define FD_WDM_AP_PROFILE_RADIO_DTIM_PERIOD                 10
#define FD_WDM_AP_PROFILE_RADIO_FRAGMENTATION_THRESHOLD     2346
#define FD_WDM_AP_PROFILE_RADIO_RTS_THRESHOLD               2347
#define FD_WDM_AP_PROFILE_RADIO_SHORT_RETRY_LIMIT           7
#define FD_WDM_AP_PROFILE_RADIO_LONG_RETRY_LIMIT            4
#define FD_WDM_AP_PROFILE_RADIO_MAX_TRANSMIT_LIFETIME       512
#define FD_WDM_AP_PROFILE_RADIO_MAX_RECEIVE_LIFETIME        512
#define FD_WDM_AP_PROFILE_RADIO_CHANNEL_BANDWIDTH_DOT11A    L7_WDM_RADIO_CHANNEL_BANDWIDTH_40_MHZ
#define FD_WDM_AP_PROFILE_RADIO_CHANNEL_BANDWIDTH_DOT11BG   L7_WDM_RADIO_CHANNEL_BANDWIDTH_20_MHZ
#define FD_WDM_AP_PROFILE_RADIO_PRIMARY_CHANNEL             L7_WDM_RADIO_PRIMARY_CHANNEL_LOWER
#define FD_WDM_AP_PROFILE_RADIO_PROTECTION                  L7_WDM_RADIO_DOT11N_PROTECTION_AUTO
#define FD_WDM_AP_PROFILE_RADIO_GUARD_INTERVAL              L7_WDM_RADIO_GUARD_INTERVAL_SHORT
#define FD_WDM_AP_PROFILE_RADIO_STBC_MODE                   L7_WDM_RADIO_STBC_MODE_ENABLE
#define FD_WDM_AP_PROFILE_RADIO_MCAST_TX_RATE               0
#define FD_WDM_AP_PROFILE_RADIO_APSD                        L7_ENABLE
#define FD_WDM_AP_PROFILE_RADIO_NO_ACK                      L7_DISABLE
#define FD_WDM_AP_PROFILE_RADIO_MAX_CLIENTS                 L7_WIRELESS_MAX_CLIENTS_PER_RADIO
#define FD_WDM_AP_PROFILE_RADIO_CHANNEL_AUTO_ADJUST         L7_ENABLE

#if 1 
#define FD_WDM_AP_PROFILE_RADIO_LIMIT_CHANNEL_SELECTION     L7_DISABLE
#endif

#define FD_WDM_AP_PROFILE_RADIO_POWER_AUTO_ADJUST           L7_ENABLE
#define FD_WDM_AP_PROFILE_RADIO_TX_POWER_DEFAULT            100
#define FD_WDM_AP_PROFILE_RADIO_WMM_MODE                    L7_ENABLE
#define FD_WDM_AP_PROFILE_RADIO_RRM_ENABLED                 L7_ENABLE
#define FD_WDM_AP_PROFILE_RADIO_LOAD_BALANCING_MODE         L7_DISABLE
#define FD_WDM_AP_PROFILE_RADIO_DATA_SNOOPING_MODE          L7_DISABLE
#define FD_WDM_LOAD_BALANCING_UTILIZATION                   60
#define FD_WDM_AP_PROFILE_RADIO_STATION_ISOLATION_MODE      L7_DISABLE
#define FD_WDM_AP_PROFILE_RADIO_ANTENNA_DIVERSITY_MODE      L7_ENABLE
#define FD_WDM_AP_PROFILE_RADIO_ANTENNA_SELECT              L7_WDM_RADIO_ANTENNA_LEFTRIGHT
/* AP Profile Radio - End */

/* AP Profile Radio QoS - Start */
#define FD_WDM_EDCA_TEMPLATE_TYPE                           L7_WDM_EDCA_TEMPLATE_CUSTOM

#define FD_WDM_AP_PROFILE_AP_EDCA_VOICE_AIFS                1
#define FD_WDM_AP_PROFILE_AP_EDCA_VIDEO_AIFS                1
#define FD_WDM_AP_PROFILE_AP_EDCA_BEST_EFFORT_AIFS          3
#define FD_WDM_AP_PROFILE_AP_EDCA_BACKGROUND_AIFS           7
#define FD_WDM_AP_PROFILE_AP_EDCA_VOICE_CWMIN               3
#define FD_WDM_AP_PROFILE_AP_EDCA_VIDEO_CWMIN               7
#define FD_WDM_AP_PROFILE_AP_EDCA_BEST_EFFORT_CWMIN         15
#define FD_WDM_AP_PROFILE_AP_EDCA_BACKGROUND_CWMIN          15
#define FD_WDM_AP_PROFILE_AP_EDCA_VOICE_CWMAX               7
#define FD_WDM_AP_PROFILE_AP_EDCA_VIDEO_CWMAX               15
#define FD_WDM_AP_PROFILE_AP_EDCA_BEST_EFFORT_CWMAX         63
#define FD_WDM_AP_PROFILE_AP_EDCA_BACKGROUND_CWMAX          1023
#define FD_WDM_AP_PROFILE_AP_EDCA_VOICE_BURST               1500
#define FD_WDM_AP_PROFILE_AP_EDCA_VIDEO_BURST               3000
#define FD_WDM_AP_PROFILE_AP_EDCA_BEST_EFFORT_BURST         0
#define FD_WDM_AP_PROFILE_AP_EDCA_BACKGROUND_BURST          0
#define FD_WDM_AP_PROFILE_STATION_EDCA_VOICE_AIFS           2
#define FD_WDM_AP_PROFILE_STATION_EDCA_VIDEO_AIFS           2
#define FD_WDM_AP_PROFILE_STATION_EDCA_BEST_EFFORT_AIFS     3
#define FD_WDM_AP_PROFILE_STATION_EDCA_BACKGROUND_AIFS      7
#define FD_WDM_AP_PROFILE_STATION_EDCA_VOICE_CWMIN          3
#define FD_WDM_AP_PROFILE_STATION_EDCA_VIDEO_CWMIN          7
#define FD_WDM_AP_PROFILE_STATION_EDCA_BEST_EFFORT_CWMIN    15
#define FD_WDM_AP_PROFILE_STATION_EDCA_BACKGROUND_CWMIN     15
#define FD_WDM_AP_PROFILE_STATION_EDCA_VOICE_CWMAX          7
#define FD_WDM_AP_PROFILE_STATION_EDCA_VIDEO_CWMAX          15
#define FD_WDM_AP_PROFILE_STATION_EDCA_BEST_EFFORT_CWMAX    1023
#define FD_WDM_AP_PROFILE_STATION_EDCA_BACKGROUND_CWMAX     1023
#define FD_WDM_AP_PROFILE_STATION_EDCA_VOICE_TXOP_LIMIT     47
#define FD_WDM_AP_PROFILE_STATION_EDCA_VIDEO_TXOP_LIMIT     94
#define FD_WDM_AP_PROFILE_STATION_EDCA_BEST_EFFORT_TXOP_LIMIT     0
#define FD_WDM_AP_PROFILE_STATION_EDCA_BACKGROUND_TXOP_LIMIT      0
/* AP Profile Radio QoS - End */

/* AP Profile Radio TSPEC - Start */
#define FD_WDM_AP_PROFILE_RADIO_TSPEC_VOICE_ACM_MODE              L7_DISABLE
#define FD_WDM_AP_PROFILE_RADIO_TSPEC_VIDEO_ACM_MODE              L7_DISABLE
#define FD_WDM_AP_PROFILE_RADIO_TSPEC_VOICE_ACM_LIMIT             20          /* percent */
#define FD_WDM_AP_PROFILE_RADIO_TSPEC_VIDEO_ACM_LIMIT             15          /* percent */
#define FD_WDM_AP_PROFILE_RADIO_TSPEC_ROAM_RES_LIMIT              5           /* percent */
#define FD_WDM_AP_PROFILE_RADIO_TSPEC_AP_INACT_TIMEOUT            30          /* seconds */
#define FD_WDM_AP_PROFILE_RADIO_TSPEC_STA_INACT_TIMEOUT           30          /* seconds */
#define FD_WDM_AP_PROFILE_RADIO_TSPEC_LEGACY_WMM_Q_MAP_MODE       L7_DISABLE
#define FD_WDM_AP_PROFILE_RADIO_TSPEC_MODE                        L7_DISABLE
/* AP Profile Radio TSPEC - End */

/* AP Profile VAP - Start */
#define FD_WDM_AP_PROFILE_VAP0_MODE         L7_ENABLE
#define FD_WDM_AP_PROFILE_VAP_MODE          L7_DISABLE
/* AP Profile VAP - End */

/*-------------------------------------*/
/* End AP Profile Defaults             */
/*-------------------------------------*/

/*-------------------------------------*/
/* Start WDS Managed AP Defaults       */
/*-------------------------------------*/
#define FD_WDM_WDS_AP_STP_PRIORITY          36864
#define FD_WDM_WDS_LINK_STP_COST            40

/*-------------------------------------*/
/* End WDS Managed AP Defaults         */
/*-------------------------------------*/

/*-------------------------------------*/
/* Start Device Location Defaults      */
/*-------------------------------------*/

#define FD_WDM_DEFAULT_BLDG_DESC "Building-%d"
#define FD_WDM_DEFAULT_WS_DEVICE_LOCATION_RF_SCAN_INTERVAL 60
#define FD_WDM_DEFAULT_WS_DEV_LOC_MEASUREMENT_SYS "Metric"
#define FD_WDM_DEFAULT_WS_DEV_LOC_RFSCAN          L7_ENABLE
/*-------------------------------------*/
/* End Device Location Defaults       */
/*-------------------------------------*/
#define FD_WIRELESS_RRM_CHANNEL_LOAD_NEW_REQUEST_DURATION  /* in TUs */  1000

/* Start of Wireless Authentication Component's Factory Defaults */
#define FD_WIRELESS_CAPWAP_MTU               1518
#define FD_WIRELESS_CAPWAP_TTL               255

#define FD_WIRELESS_BASE_PORT                L7_IP_PORT_57775      

#define FD_WIRELESS_SSL_CERT_VALID_DAYS      1

#define FD_WIRELESS_SSL_PEM_DIR              "./"
#define FD_WIRELESS_SSL_ROOT_PEM             "wsroot.pem"
#define FD_WIRELESS_SSL_SERVER_PEM           "wsserver.pem"
#define FD_WIRELESS_SSL_DHWEAK_PEM           "wsdh512.pem"
#define FD_WIRELESS_SSL_DHSTRONG_PEM         "wsdh1024.pem"

#define FD_WIRELESS_DEV_LOC_ACCEPTABLE_SIGMA               2  /* meters */
#define FD_WIRELESS_DEV_LOC_RADIUS_FUDGE_LIMIT             5  /* meters */
#define FD_WIRELESS_DEV_LOC_HIGH_SIGNAL_THRESHOLD_2_4GHZ    (-46) /* dbm */
#define FD_WIRELESS_DEV_LOC_HIGH_SIGNAL_THRESHOLD_5GHZ     (-55)  /* dbm */
#define FD_WIRELESS_DEV_LOC_DEFAULT_TRIANGULATION_APS      6

extern L7_uint32 numDistMaps; /* defined in ws_dev_loc.c */
#define FD_WS_DEV_LOC_MAP_REF_SIG_2_4GHZ(index)  \
                    ( (index <= numDistMaps/4) ? (-23) : (-34) )

#define FD_WS_DEV_LOC_MAP_REF_SIG_5_0GHZ(__index)  \
                    ( (index <= numDistMaps/4) ? (-36) : (-40) )

#define FD_WS_DEV_LOC_MAP_REF_SIG(band,index)                           \
                    ( band ? FD_WS_DEV_LOC_MAP_REF_SIG_5_0GHZ(index)    \
                           : FD_WS_DEV_LOC_MAP_REF_SIG_2_4GHZ(index)    \
                    )

#define FD_WS_DEV_LOC_MAP_REF_DISTANCE         1 /* meters */                         

#define FD_WS_DEV_LOC_MAP_REF_DEGRADATION_2_4GHZ(index)                               \
                    ( ( index <= (numDistMaps/4) ) ?                                  \
                      (float)( 3.5 - ( (float)( (numDistMaps/4) - index )) *(0.5) ) : \
                      (float)( 3.5 - ( (float)( (numDistMaps/2) - index )) *(0.5) )   \
                    )     

#define FD_WS_DEV_LOC_MAP_REF_DEGRADATION_5_0GHZ(index)                               \
                    ( ( index <= (numDistMaps/4) ) ?                                  \
                      (float)( 3.7 - ( (float)( (numDistMaps/4) - index )) *(0.5) ) : \
                      (float)( 3.7 - ( (float)( (numDistMaps/2) - index )) *(0.5) )   \
                    )     

#define FD_WS_DEV_LOC_MAP_REF_DEGRADATION(_band,_index)                         \
                    ( _band ? FD_WS_DEV_LOC_MAP_REF_DEGRADATION_5_0GHZ(_index)  \
                            : FD_WS_DEV_LOC_MAP_REF_DEGRADATION_2_4GHZ(_index)  \
                    )

#define FD_WS_DEV_LOC_MAP_REF_OBSTACLE_DIST         8 /* meters */

/* End of Wireless Authentication Component's Factory Defaults */

#endif /* INCLUDE_WIRELESS_CONFIG_DEFAULTS */
