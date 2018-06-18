
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename wireless_comm_strcusts.h
*
* @purpose The purpose of this file is to have a central location for
*          wireless common structures to be used by the entire system.
*
* @component wireless
*
* @comments none
*
* @create 30-aug-2007
*
* @author tgaunce
* @end
*
**********************************************************************/


#ifndef INCLUDE_WIRELESS_COMM_STRUCTS
#define INCLUDE_WIRELESS_COMM_STRUCTS

#include "datatypes.h"
#include "l7_packet.h"
#include "wireless_commdefs.h"


/* cell recovery neighbor info */
typedef struct wdmCellRecoveryNbrInfo_s
{
  L7_enetMacAddr_t              macAddr;
  L7_uchar8                     radioIf;
} wdmCellRecoveryNbrInfo_t;

/****************************************
*
*  RF-SCAN STRUCTURES AND DEFINES
*
*****************************************/

#define WDM_RFSCAN_TRIANGULATION_ENTRIES_TOTAL    6   /* 3 sentry and 3 non-sentry rf-scan triangulation entries */
#define WDM_RFSCAN_TRIANGULATION_ENTRIES        3   /* 3 sentry and 3 non-sentry rf-scan triangulation entries */

typedef struct apVapPair_s
{
  L7_enetMacAddr_t ap;
  L7_enetMacAddr_t vap;
} apVapPair_t;

#define RRM_ENABLED_CAPAB_LEN  5
typedef struct rrmEnabCapab_s
{
  L7_uchar8 data[RRM_ENABLED_CAPAB_LEN];
} rrmEnabCapab_t;


typedef struct wdmWsRFScanAPData_s
{
  L7_enetMacAddr_t  vapMac;        /* Key, Sender VAP MAC of Neighbor AP */  
  L7_uchar8         ssid[L7_WDM_SSID_MAX_LENGTH+1]; /* SS ID */
  L7_enetMacAddr_t  bssid;
  L7_uchar8         channelNumber;  /* beacon Tx Channel) */
  L7_ushort16       rate;           /* in 100Kbps units */
  L7_ushort16       beaconInt;      /* in milliseconds */
  L7_enetMacAddr_t  apMacAddr;      /* Sender AP MAC address (part of vendor 
				       info in LVL7 VAP beacons) */
  L7_uchar8         adhoc;
  L7_uchar8         rssi;           /* 0 - 100 */
  L7_uchar8         security;    /* Security used by the AP */
  L7_ushort16       highRate;    /* Highest supported rate in 100Kbps units */
  L7_int8           strength;    /* Signal strength in dBm */
  L7_int8           noise;       /* Channel noise in dBm */
  L7_uchar8         dot11n;      /* 802.11n mode */
  L7_ushort16       cif;          /* Capability info field, as per IEEE 802.11
				   * spec, section 7.3.1.4 */
  L7_uchar8         phyType;      /* PHY Type 
                                   * (see 802.11k spec, sec. 7.3.2.22.6) */
  L7_uchar8         regClass;     /* Reg Class (also in 7.3.2.22.6) */
  rrmEnabCapab_t    rrmEnabCapab; /* RRM Enabled Capabilities 
                                   * (see 802.11k spec, sec. 7.3.2.45) */
} wdmWsRFScanAPData_t;

/* RF Scan Access Point Signal Triangulation entry information */
typedef struct wdmRFScanTriangulationEntry_s
{
  L7_BOOL               valid;      /* L7_TRUE if this entry is valid, L7_FALSE otherwise */
  L7_BOOL               sentry;     /* L7_TRUE for sentry AP or L7_FALSE for non-sentry AP */
  L7_enetMacAddr_t      mac;        /* Mac Address of the AP which detected the device */
  L7_uchar8             radio;      /* Radio of the AP which detected the device */
  L7_uint32             rssi;       /* Received signal strength indicator in percent (0 to 100%) */
  L7_int32              strength;   /* Received signal strength in dBm (-127 to 127) */
  L7_int32              noise;      /* Noise reported on the channel byt the AP in dBm (-127 to 127) */
  L7_uint32             time;       /* Time in seconds when this AP detected the signal */
} wdmRFScanTriangulationEntry_t;

/* RF Scan Access Point Signal Triangulation status - all entries */
typedef struct wdmRFScanTriangulationStatus_s
{
  wdmRFScanTriangulationEntry_t     sentry[WDM_RFSCAN_TRIANGULATION_ENTRIES];  
  wdmRFScanTriangulationEntry_t     nonSentry[WDM_RFSCAN_TRIANGULATION_ENTRIES];  
} wdmRFScanTriangulationStatus_t;

/* WIDS AP Rogue Classification Test Results */
typedef struct wdmWidsRogueClassificationTestResults_s
{
  L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t testId;
  L7_BOOL             detected;       /* Flag indicating whether this test detected the condition that 
                                         it is designed to detect, L7_TRUE or L7_FLASE */
  L7_enetMacAddr_t    mac;            /* Managed AP MAC address that last reported detecting this condition */
  L7_uchar8           radio;          /* Radio of the managed AP that last reported detecting this condition, 
                                         0 - No report avail, 1-2 */
  L7_uchar8           testEnabled;    /* Indicated that this test is configured to report rogues, L7_ENABLE or L7_DISABLE */
  L7_BOOL             rogueReported;  /* Indication that this test reported the device as rogue, L7_TRUE or L7_FALSE */
  L7_uint32           firstDetected;  /* Time stamp indicating when this test first detected the condition */
  L7_uint32           lastDetected;   /* Time stamp indicating when this test last detected the condition */
} wdmWidsRogueClassificationTestResults_t;

typedef struct wdmWidsWirelessThreatResults_s
{
  L7_BOOL                                 rogueReported;
  L7_BOOL                                 threatDetected;
  wdmWidsRogueClassificationTestResults_t threat[L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_COUNT];
} wdmWidsWirelessThreatResults_t;



#define WDM_AP_DE_AUTHENTICATION_ATTACK_MAX    16   /* Maximum number of APs the wireless system can 
                                                       conduct the de-authentication attack against */
/* WIDS AP De-Authentication Attack Status */
typedef struct wdmWidsDeAuthenticationAttackStatus_s
{
  L7_BOOL             valid;    /* L7_TRUE if this entry is valid, L7_FALSE otherwise */
  L7_enetMacAddr_t    bssid;    /* BSSID of the AP against which the attack is launched */
  L7_uchar8           chan;     /* Channel on which the rogue AP is operating */
  L7_uint32           tStart;   /* Time since the attack started on this AP */
  L7_uint32           age;      /* Time since the RF Scan report about this AP */
} wdmWidsDeAuthenticationAttackStatus_t;

/* WIDS Client Rogue Classification Test Results */
typedef struct wdmWidsClientRogueClassificationTestResults_s
{
  L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId;
  L7_BOOL             detected;       /* Flag indicating whether this test detected the condition that
                                         it is designed to detect, L7_TRUE or L7_FLASE */
  L7_enetMacAddr_t    apMac;            /* Managed AP MAC address that last reported detecting this condition */
  L7_uchar8           radio;          /* Radio of the managed AP that last reported detecting this condition,
                                         0 - No report avail, 1-2 */
  L7_uchar8           testEnabled;    /* Indicated that this test is configured to report rogues, L7_ENABLE or L7_DISABLE */
  L7_BOOL             rogueReported;  /* Indication that this test reported the device as rogue, L7_TRUE or L7_FALSE */
  L7_uint32           firstDetected;  /* Time stamp indicating when this test first detected the condition */
  L7_uint32           lastDetected;   /* Time stamp indicating when this test last detected the condition */
} wdmWidsClientRogueClassificationTestResults_t;

/*  Global Maximum number of roaming history entries and pre-authentication 
**  history entries respectively.
*/
#define L7_WDM_DETECTED_CLIENT_PREAUTH_HISTORY_ENTRIES_MAX             500 
#define L7_WDM_DETECTED_CLIENT_ROAM_HISTORY_ENTRIES_MAX                500 

/*  Maximum number of roaming history entries and pre-authentication 
**  history entries respectively per detected client.
*/
#define L7_WDM_DETECTED_CLIENT_PREAUTH_HISTORY_ENTRIES_PER_CLIENT_MAX  10
#define L7_WDM_DETECTED_CLIENT_ROAM_HISTORY_ENTRIES_PER_CLIENT_MAX     10



/* Detected client's roaming history entry event type  */
typedef enum
{
  L7_WDM_DETECTED_CLIENT_EVENT_NEW_AUTHENTICATION = 1,
  L7_WDM_DETECTED_CLIENT_EVENT_ROAMING,
  L7_WDM_DETECTED_CLIENT_EVENT_TYPE_MAX
}L7_WDM_DETECTED_CLIENT_HISTORY_EVENT_t;

/* Detected Client Signal Triangulation entry information */
typedef struct wdmDetectedClientTriangulationEntry_s
{
  L7_enetMacAddr_t      mac;        /* AP which has reported the detected client */
  L7_int32              strength;   /* Received signal strength in dBm (-127 to 127) */
  L7_int32              noise;      /* Noise reported on the channel by the AP in dBm (-127 to 127) */
  L7_uint32             timeStamp;  /* Last updated time in seconds when this event is recorded at switch */
  L7_int8               rssi;       /* Received signal strength indicator in percent (0 to 100%) */
  L7_uint8              radio;      /* Radio of the AP which detected the client */
} wdmDetectedClientTriangulationEntry_t;

/* Detected Client Signal Triangulation status - all entries */
typedef struct wdmDetectedClientTriangulationStatus_s
{
  wdmDetectedClientTriangulationEntry_t sentry[WDM_RFSCAN_TRIANGULATION_ENTRIES];  
  wdmDetectedClientTriangulationEntry_t nonSentry[WDM_RFSCAN_TRIANGULATION_ENTRIES];  
} wdmDetectedClientTriangulationStatus_t;

/* TSPEC summary statistics */
typedef struct wdmTspecTrafficSumStats_s
{
  L7_uint64                  tsPktsRx;
  L7_uint64                  tsPktsTx;
  L7_uint64                  tsBytesRx;
  L7_uint64                  tsBytesTx;
} wdmTspecTrafficSumStats_t;

/* TSPEC general statistics */
typedef struct wdmTspecGenSumStats_s
{
  L7_uint32                   totTspecAccepted;
  L7_uint32                   totTspecRejected;
  L7_uint32                   totRoamTspecAccepted;
  L7_uint32                   totRoamTspecRejected;
} wdmTspecGenSumStats_t;

typedef struct WdmWDSLinkKey_s
{
  L7_uchar8                 groupId;
  L7_enetMacAddr_t          sourceMacAddr;
  L7_uchar8                 sourceRadio;
  L7_enetMacAddr_t          destMacAddr;
  L7_uchar8                 destRadio;
} wdmWDSLinkKey_t;

typedef struct wdmWDSAPKey_s
{
  L7_uchar8                 groupId;
  L7_enetMacAddr_t          apMac;
} wdmWDSAPKey_t;

typedef struct wdmDevLocBdgFlrKey_s
{
  L7_uint32                 bldngNum;
  L7_uint32                 flrNum;
} wdmDevLocBdgFlrKey_t;

typedef struct wdmDevLocApKey_s
{
  L7_uint32                 bldngNum;
  L7_uint32                 flrNum;
  L7_enetMacAddr_t          mac;
} wdmDevLocApKey_t;

typedef struct wdmDevLocXYCoord_s
{
  L7_int32 x;
  L7_int32 y;
}wdmDevLocXYCoord_t;

typedef struct devLocCoord_s
{
  L7_int32 x;
  L7_int32 y;
}devLocCoord_t;
 
#define WS_DEV_LOC_DIST_MAP_MEASUREMENT_RANGE_MAX     100

typedef struct sigToDist_s
{
  L7_int32 distance;
  L7_int32 signal;
}sigToDist_t;

typedef struct signalToDistanceMap_s
{
  /* Reference signal strength in dBm.
  */
  L7_int32 refSignal;

  /* Distance at which the reference signal is measured in meters.
  */
  L7_int32 refDistance;

  /* In an indoor ennvironment there are obstacles, such as walls that absorb the 
  ** wireless signal. This value represents the average absorption value in 
  ** dbm for each obstacle.
  */
  float degradFactor;

  /* Average distance in meters between obstacles in the indoor environment.
  */
  L7_int32 obstDistance;

  /* Number of distance/signal pairs.
  */
  L7_int32 numEntries;
  sigToDist_t sigToDistTable[WS_DEV_LOC_DIST_MAP_MEASUREMENT_RANGE_MAX];
} signalToDistanceMap_t;

#endif 
