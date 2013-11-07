/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/wireless/wireless_web.h
 *
 * @purpose Support of the EmWeb code
 *
 * @component unitmgr
 *
 * @comments tba
 *
 * @create 06/12/2000
 *
 * @author tgaunce
 * @end
 *
 **********************************************************************/

#ifndef WIRELESS_WEB_H__
#define WIRELESS_WEB_H__
#include "wireless_comm_structs.h"
#include "wireless_commdefs.h"

#define USMWEB_WIRELESS_CHANNEL_SIZE  6

/* wireless_cfg.c */
L7_char8 *usmWebWidsPriorityEntryHelp(L7_int32 min, L7_int32 max);

/* network_client_qos_cfg.c */
L7_char8 *usmWebBandwidthLimitEntryHelp(L7_uint32 min, L7_uint32 max);
void usmWebNetworkClientQosCfgGet(EwsContext context,
                                  EwaForm_ws_network_cfgP form,
                                  L7_uint32 networkId,
                                  usmWeb_AppInfo_t *pAppInfo);
void usmWebNetworkClientQosCfgSet(EwsContext context,
                                  EwaForm_ws_network_cfgP form,
                                  L7_uint32 networkId,
                                  usmWeb_AppInfo_t *pAppInfo);

typedef enum
{
  USMWEB_APPINFO_APPROFILE_ADVANCED = 0,
  USMWEB_APPINFO_APPROFILE_REFRESH,
  USMWEB_APPINFO_APPROFILE_ID,
  USMWEB_APPINFO_APPROFILE_RADIO,
  USMWEB_APPINFO_APPROFILE_VAP,
  USMWEB_APPINFO_NETWORK_ID,
  USMWEB_APPINFO_NETWORK_SECURITY,
  USMWEB_APPINFO_AP_RESET,
  USMWEB_APPINFO_AP_MANAGED_MODE,
  USMWEB_APPINFO_AP_HW_TYPE_ID,
  USMWEB_APPINFO_NETWORK_CLIENT_QOS_USE_APPDATA,
  USMWEB_APPINFO_NETWORK_CLIENT_QOS_MODE,
  USMWEB_APPINFO_NETWORK_CLIENT_QOS_BW_DOWN,
  USMWEB_APPINFO_NETWORK_CLIENT_QOS_BW_UP,
  USMWEB_APPINFO_NETWORK_CLIENT_QOS_ACL_DOWN_ID,
  USMWEB_APPINFO_NETWORK_CLIENT_QOS_ACL_UP_ID,
  USMWEB_APPINFO_NETWORK_CLIENT_QOS_DS_POLICY_DOWN_ID,
  USMWEB_APPINFO_NETWORK_CLIENT_QOS_DS_POLICY_UP_ID,
  USMWEB_APPINFO_NETWORK_CLIENT_QOS_DISP_TYPE
} usmWebAppInfoAPProfileData_t;

typedef enum
{
  USMWEB_APPINFO_DEVLOC_BUILDING_NUM = 0,
  USMWEB_APPINFO_DEVLOC_FLOOR_NUM,
  USMWEB_APPINFO_DEVLOC_DEVICE_TYPE,
  USMWEB_APPINFO_DEVLOC_DETECTED_MAC_LIST,
  USMWEB_APPINFO_DEVLOC_USE_RADIOS,
  USMWEB_APPINFO_DEVLOC_SUBMIT_CODE
} usmWebAppInfoLocationTriggerData_t;

typedef enum
{
  USMWEB_AP_STATUS_TAB_AP_SUMMARY = 0,
  USMWEB_AP_STATUS_TAB_AP_DETAIL,
  USMWEB_AP_STATUS_TAB_AP_RADIO_SUMMARY,
  USMWEB_AP_STATUS_TAB_AP_RADIO_DETAIL,
  USMWEB_AP_STATUS_TAB_AP_NEIGHBOR_AP,
  USMWEB_AP_STATUS_TAB_AP_NEIGHBOR_CLIENTS,
  USMWEB_AP_STATUS_TAB_AP_VAP,
  USMWEB_AP_STATUS_TAB_AP_DIST_TUNNEL
} usmWebAPStatusTabs_t;

typedef enum
{
  USMWEB_AP_STATISTICS_TAB_AP_WLAN = 0,
  USMWEB_AP_STATISTICS_TAB_AP_ENET,
  USMWEB_AP_STATISTICS_TAB_AP_DETAIL,
  USMWEB_AP_STATISTICS_TAB_AP_RADIO,
  USMWEB_AP_STATISTICS_TAB_AP_VAP,
  USMWEB_AP_STATISTICS_TAB_AP_DIST_TUNNEL
} usmWebAPStatisticsTabs_t;

typedef enum
{
  USMWEB_CLIENT_STATISTICS_TAB_ASSOC = 0,
  USMWEB_CLIENT_STATISTICS_TAB_SESSION,
  USMWEB_CLIENT_STATISTICS_TAB_CLIENT_ASSOC,
  USMWEB_CLIENT_STATISTICS_TAB_CLIENT_SESSION
} usmWebClientStatisticsTabs_t;

typedef enum {
  USMWEB_CHPWR_HISTORY_SUMMARY=0,
  USMWEB_CHPWR_HISTORY_DETAIL
}usmWebHistoryTabs_t;

typedef enum {
  USMWEB_APPINFO_CHPWR_CH_PLAN=0,
  USMWEB_APPINFO_CHPWR_SW_IP
} usmWebAppInfoChPwrData_t;


typedef enum {
  USMWEB_AP_HW_TYPE_GLOBAL=0,
  USMWEB_AP_HW_TYPE_RADIO_DETAIL
}usmWebAPHWTypeTabs_t;

typedef enum {
  USMWEB_APPINFO_APHW_TYPE_RADIO_HWID=0,
  USMWEB_APPINFO_APHW_TYPE_RADIO_RADIOID
}usmWebAppInfoAPHWTypeRadioData_t;

typedef struct UsmDbWdmAPProfileMACData_s
{
  L7_uint32 profileId;
  L7_enetMacAddr_t macAddr;
} usmDbWdmAPProfileMACData_t;

typedef struct
{
  L7_BOOL advanced;
  L7_uint32 profileId;
  L7_uint32 radio;
  L7_uint32 vapId;
} usmWebAPProfileVAPEntry_t;

typedef struct
{
  L7_enetMacAddr_t macAddr;
  L7_uchar8 rif;
} usmWebManagedAPRadioEntry_t;

typedef struct
{
  L7_enetMacAddr_t macAddr;
  L7_enetMacAddr_t mac;
  L7_uchar8 rif;
} usmWeb_clientMap_t;

typedef struct
{
  L7_enetMacAddr_t macAddr;
  L7_char8 ssid[L7_WDM_SSID_MAX_LENGTH+1];
} usmWeb_clientSSID_t;

typedef struct
{
  L7_enetMacAddr_t macAddr;
  L7_enetMacAddr_t mac;
  L7_uchar8 rif;
  L7_int32 vapId;
  L7_char8 ssid[L7_WDM_SSID_MAX_LENGTH+1];
  L7_BOOL same;
  L7_BOOL filter;
} usmWeb_wirelessMap_t;

typedef struct
{
  L7_WDM_CHANNEL_PLAN_TYPE_t plan;
  L7_enetMacAddr_t macAddr;
  L7_uchar8 radio;
} usmWebChannelPlanEntry_t;

typedef struct
{
  L7_enetMacAddr_t macAddr;
  L7_uchar8 radio;
} usmWebPowerPlanEntry_t;


typedef struct
{
  L7_uint32 index;
  L7_enetMacAddr_t macAddr;
} adHocIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_char8 ssid[L7_WDM_SSID_MAX_LENGTH+1];
  L7_enetMacAddr_t macAddr;
  L7_uint32 channel;
  L7_uint32 status;
} ssidClientIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_OUIVal_t ouiVal;
  L7_char8    oui[L7_WDM_OUI_MAX+1];
}ouiIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_enetMacAddr_t vapMAC;
  L7_enetMacAddr_t apMAC;
  L7_char8 loc[L7_WDM_MAX_LOCATION_NAME+1];
  L7_uchar8 radio;
  L7_char8 radioMode[32];
  L7_enetMacAddr_t clientMAC;
} vapClientIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_enetMacAddr_t macAddr;
  L7_uint64 pktsRx;
  L7_uint64 bytesRx;
  L7_uint64 pktsTx;
  L7_uint64 bytesTx;
} clientStatsIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_enetMacAddr_t nbrMAC;
  L7_uchar8 radio;
  L7_WDM_CLIENT_STATUS_t status;
} clientNbrApIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_enetMacAddr_t nbrMAC;
  L7_char8 ssid[L7_WDM_SSID_MAX_LENGTH+1];
  L7_uint32 rssi;
  L7_WDM_CLIENT_STATUS_t status;
  L7_uint32 age;
} apNbrApIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_enetMacAddr_t nbrMAC;
  L7_uchar8 channel;
  L7_uint32 rssi;
  L7_WDM_CLIENT_STATUS_t status;
  L7_uint32 age;
} apNbrClientIterator_t;

typedef struct
{
  L7_enetMacAddr_t macAddr;
  L7_uchar8 radio;
  L7_uchar8 channel;
} usmWebRadioRadarEntry_t;

typedef struct
{
  L7_uchar8 channel;
  L7_uint32 phyModesMask;
  L7_BOOL rdrDetRequired;
} usmWebRadarChannelEntry_t;

typedef struct
{
  L7_IP_ADDR_t               ipAddr;
  L7_enetMacAddr_t           macAddr;
  L7_BOOL                    allPeerAP;
} usmWebPeerSwitchAPEntry_t;

typedef struct
{
  L7_uint32 index;
  L7_IP_ADDR_t switchIPAddr;
  L7_enetMacAddr_t macAddr;
  L7_uint32 channel;
  L7_uint32 status;
} switchClientIterator_t;

typedef struct
{
  L7_uint32 sentryMode;
  wdmDetectedClientTriangulationEntry_t entry;
} clientTriangulationIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_enetMacAddr_t clientMac;
  L7_uint32 appIndex;
} roamHistoryIterator_t;

typedef struct
{
  L7_uint32 index;
  L7_enetMacAddr_t clientMac;
  L7_uint32 appIndex;
} preAuthIterator_t;

typedef struct
{
   apVapPair_t                avp;
   L7_enetMacAddr_t           nbrMac;
   L7_BOOL                    allAP;
} usmWebRrmNbrAPEntry_t;


typedef struct
{
  L7_uchar8                  groupId;
  L7_enetMacAddr_t           macAddr;
  L7_BOOL                    allGroupId;
} usmWebWdsAPStatusEntry_t;

typedef struct
{
  L7_uint32             bldngNum;
  L7_uint32             flrNum;
  L7_enetMacAddr_t      macAddr;
  L7_BOOL               allBldng;
  L7_BOOL               allFlr;
} usmWebDevLocManagedAPEntry_t;



#endif

