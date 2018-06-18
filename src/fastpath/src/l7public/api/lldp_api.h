/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename lldp_api.h
*
* @purpose 802.1AB definitions
*
* @component 802.1AB(LLDP)
*
* @comments none
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#ifndef LLDP_API_H
#define LLDP_API_H

#include "system_exports.h"

typedef enum {
  LLDP_CHASSIS_ID_SUBTYPE_CHASSIS_COMP = 1,
  LLDP_CHASSIS_ID_SUBTYPE_INTF_ALIAS = 2,
  LLDP_CHASSIS_ID_SUBTYPE_PORT_COMP = 3,
  LLDP_CHASSIS_ID_SUBTYPE_MAC_ADDR = 4,
  LLDP_CHASSIS_ID_SUBTYPE_NET_ADDR = 5,
  LLDP_CHASSIS_ID_SUBTYPE_INTF_NAME = 6,
  LLDP_CHASSIS_ID_SUBTYPE_LOCAL = 7
} lldpChassisIdSubtype_t;

typedef enum {
  LLDP_CHASSIS_ID_SUBTYPE_NET_OTHER = 0,
  LLDP_CHASSIS_ID_SUBTYPE_NET_IPV4 =1, 
  LLDP_CHASSIS_ID_SUBTYPE_NET_IPV6 = 2,
  LLDP_CHASSIS_ID_SUBTYPE_NET_NSAP = 3,
  LLDP_CHASSIS_ID_SUBTYPE_NET_HDLC = 4,
  LLDP_CHASSIS_ID_SUBTYPE_NET_BBN1822 = 5,
  LLDP_CHASSIS_ID_SUBTYPE_NET_ALL802 = 6,
  LLDP_CHASSIS_ID_SUBTYPE_NET_E163 = 7,
  LLDP_CHASSIS_ID_SUBTYPE_NET_E164 = 8,
  LLDP_CHASSIS_ID_SUBTYPE_NET_F69 = 9,
  LLDP_CHASSIS_ID_SUBTYPE_NET_X121 = 10,
  LLDP_CHASSIS_ID_SUBTYPE_NET_IPX = 11,
  LLDP_CHASSIS_ID_SUBTYPE_NET_APPLETALK = 12,
  LLDP_CHASSIS_ID_SUBTYPE_NET_DECNETIV = 13,
  LLDP_CHASSIS_ID_SUBTYPE_NET_BANYANVINES = 14,
  LLDP_CHASSIS_ID_SUBTYPE_NET_E164WITHNSAP = 15,
  LLDP_CHASSIS_ID_SUBTYPE_NET_DNS = 16,
  LLDP_CHASSIS_ID_SUBTYPE_NET_DISTINGUISHEDNAME = 17,
  LLDP_CHASSIS_ID_SUBTYPE_NET_ASNUMBER = 18,
  LLDP_CHASSIS_ID_SUBTYPE_NET_XTPOVERIPV4 = 19,
  LLDP_CHASSIS_ID_SUBTYPE_NET_XTPOVERIPV6 = 20,
  LLDP_CHASSIS_ID_SUBTYPE_NET_XTPNATIVEMODEXTP = 21,
  LLDP_CHASSIS_ID_SUBTYPE_NET_FIBRECHANNELWWPN = 22,
  LLDP_CHASSIS_ID_SUBTYPE_NET_FIBRECHANNELWWNN = 23,
  LLDP_CHASSIS_ID_SUBTYPE_NET_GWID = 24,
  LLDP_CHASSIS_ID_SUBTYPE_NET_AFI = 25,
} lldpChassisIdSubtypeNetAddrType_t;

typedef enum {
  LLDP_PORT_ID_SUBTYPE_INTF_ALIAS = 1,
  LLDP_PORT_ID_SUBTYPE_PORT_COMP = 2,
  LLDP_PORT_ID_SUBTYPE_MAC_ADDR = 3,
  LLDP_PORT_ID_SUBTYPE_NET_ADDR = 4,
  LLDP_PORT_ID_SUBTYPE_INTF_NAME = 5,
  LLDP_PORT_ID_SUBTYPE_AGENT_ID = 6,
  LLDP_PORT_ID_SUBTYPE_LOCAL = 7
} lldpPortIdSubtype_t;

typedef enum {
  LLDP_MAN_ADDR_IF_SUBTYPE_UNKNOWN = 1,
  LLDP_MAN_ADDR_IF_SUBTYPE_IF_INDEX = 2,
  LLDP_MAN_ADDR_IF_SUBTYPE_PORT_NUMBER = 3
} lldpManAddrIfSubtype_t;

typedef enum {
  LLDP_IANA_ADDR_FAMILY_NUMBER_RESERVED = 0,
  LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4 = 1,
  LLDP_IANA_ADDR_FAMILY_NUMBER_IPV6 = 2,
  LLDP_IANA_ADDR_FAMILY_NUMBER_802 = 6
} lldpIANAAddrFamilyNumber_t;

typedef enum {
  LLDP_SYS_CAP_MASK_OTHER      = 0x0001,
  LLDP_SYS_CAP_MASK_REPEATER   = 0x0002,
  LLDP_SYS_CAP_MASK_BRIDGE     = 0x0004,
  LLDP_SYS_CAP_MASK_WLAN       = 0x0008,
  LLDP_SYS_CAP_MASK_ROUTER     = 0x0010,
  LLDP_SYS_CAP_MASK_TELEPHONE  = 0x0020,
  LLDP_SYS_CAP_MASK_CABLE      = 0x0040,
  LLDP_SYS_CAP_MASK_STATION    = 0x0080,
  LLDP_SYS_CAP_MASK_RESERVED   = 0xFF00
} lldpSysCapMask_t;

#define LLDP_MGMT_STRING_SIZE_MAX       (255+1)  /* max size + NULL byte */
#define LLDP_MGMT_ADDR_SIZE_MAX         (31)
#define LLDP_MGMT_ADDR_OID_SIZE_MAX     (128+1)  /* max size + NULL byte */
#define LLDP_UNKNOWN_TLV_INFO_SIZE_MAX  (511)
#define LLDP_ORG_DEF_INFO_SIZE_MAX      (507)

/*********************************************************************
 *                LLDP-MED defines - Start
 *********************************************************************/



typedef enum
{
  notDefined = 0,
  endpointClass1,
  endpointClass2,
  endpointClass3,
  networkConnectivity

}lldpXMedDeviceClass_t;

#define LLDP_MED_CAP_CAPABILITIES_BITMASK   0x01
#define LLDP_MED_CAP_NETWORKPOLICY_BITMASK  0x02
#define LLDP_MED_CAP_LOCATION_BITMASK     0x04
#define LLDP_MED_CAP_EXT_PSE_BITMASK    0x08
#define LLDP_MED_CAP_EXT_PD_BITMASK     0x10
#define LLDP_MED_CAP_INVENTORY_BITMASK    0x20
typedef enum
{
  capabilities = 0,
  networkPolicy,
  location,
  extendedPSE,
  extendedPD,
  inventory



}lldpXMedCapabilitiesValue_t;

typedef struct
{
  L7_uchar8 bitmap[2];
}lldpXMedCapabilities_t;

typedef enum
{
  unknown_locsubtype = 0,
  coordinateBased_locsubtype,
  civicAddress_locsubtype,
  elin_locsubtype

}lldpXMedLocationSubtype_t;

#define LLDP_MED_POLICYAPPTYPE_UNKNOWN          0x0001
#define LLDP_MED_POLICYAPPTYPE_VOICE          0x0002
#define LLDP_MED_POLICYAPPTYPE_VOICE_SIGNALING      0x0004
#define LLDP_MED_POLICYAPPTYPE_GUEST_VOICE        0x0008
#define LLDP_MED_POLICYAPPTYPE_GUEST_VOICE_SIGNALING  0x0010
#define LLDP_MED_POLICYAPPTYPE_SOFT_PHONE_VOICE     0x0020
#define LLDP_MED_POLICYAPPTYPE_VIDEO_CONFERENCING     0x0040
#define LLDP_MED_POLICYAPPTYPE_STREAM_VIDEO       0x0080
#define LLDP_MED_POLICYAPPTYPE_VIDEO_SIGNALLING     0x0100
typedef enum
{
  unknown_policyapptype = 0,
  voice_policyapptype,
  voiceSignaling_policyapptype,
  guestVoice_policyapptype,
  guestVoiceSignaling_policyapptype,
  softPhoneVoice_policyapptype,
  videoConferencing_policyapptype,
  streamingVideo_policyapptype,
  videoSignaling_policyapptype

}lldpXMedPolicyAppTypeValue_t;

typedef struct
{
  L7_uchar8 bitmap[2];
}lldpXMedPolicyAppType_t;

typedef enum
{
  pseDevice=0,
  pdDevice,
  none_poedevicetype,
  unknown_poedevicetype
}lldpXMedPoeDeviceType_t;
typedef enum
{
  unknown_pwpri = 0,
  critical_pwpri,
  high_pwpri,
  low_pwpri
}lldpXMedPoePowerPriority_t;
typedef enum
{
  unknown_psesrc = 0,
  primary_psesrc,
  backup_psesrc
}lldpXMedPsePowerSource_t;
typedef enum
{
  unknown_pdsrc = 0,
  fromPSE_pdsrc,
  local_pdsrc,
  localAndPSE_pdsrc
}lldpXMedPdPowerSource_t;

typedef enum
{
  port_class_pd = 0,
  power_type_pse_type2=0,
  port_class_pse =1,
  power_type_pd_type2=1,
  power_type_pse_type1=2,
  power_type_pd_type1=3
}lldpX8023PoeMdiPowerType_t;

typedef enum
{
  LLDP_MED_ADD_EVENT = 1,
  LLDP_MED_REMOVE_EVENT,
  LLDP_POE_ADD_EVENT,
  LLDP_POE_MOD_EVENT,
  LLDP_POE_REMOVE_EVENT
}lldpXMedNotifyEvent_t;

typedef struct
{
  /*L7_uchar8 mac[L7_MAC_ADDR_LEN];*/
  L7_uchar8   remLocPortClass;
  L7_uchar8   remLocPSEMdiSupport;
  L7_uchar8   remLocPSEMdiPowerState;
  L7_uchar8   remLocPSEPairControlAbility;
  L7_uchar8   remLocPowerPair;
  L7_uchar8   remLocPowerClass;
  L7_uchar8   remLocPowerType;
  L7_uchar8   remLocPowerSource;
  L7_uchar8   remLocPowerPriority;
  L7_ushort16 remLocPDReqPowerValue;
  L7_ushort16 remLocPSEAllocPowerValue;
}lldpPoeData_t;

typedef struct
{
  lldpXMedNotifyEvent_t event;
  union
  {
    struct
    {
      L7_uchar8   mac[L7_MAC_ADDR_LEN];
      lldpXMedDeviceClass_t deviceType;
    }med;

    lldpPoeData_t poe;
  }evData;
}lldpXMedNotify_t;

typedef struct
{
  L7_COMPONENT_IDS_t registrar_ID;
  L7_RC_t (*notify_med_intf)(L7_uint32 intIfNum, lldpXMedNotify_t *data);
} lldpXMedNotifyList_t;

typedef struct
{
  L7_uint32   intIfNum;
  L7_uint32   type;
  L7_ushort16   vlanId;
  L7_uchar8   U;
  L7_uchar8   T;
  L7_uchar8   priority;
  L7_uchar8   dscp;

}lldpXMedPolicyInfoParms_t;


typedef enum
{
  LLDP_DEVICE_INFO_VOIP_MAC = 1,
  LLDP_DEVICE_INFO_MAX
}lldpRemDeviceInfoEvent_t;

typedef struct
{
  L7_uint32   intIfNum;
  L7_uint32   remIndex;
  lldpRemDeviceInfoEvent_t   info;
  union
  {
     L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  }data;
}lldpRemDeviceInfoParms_t;

typedef struct lldpCkptInfo_s
{
    L7_uchar8               macAddr[L7_MAC_ADDR_LEN];
    lldpXMedNotifyEvent_t   event;
    lldpXMedDeviceClass_t   deviceType;
    L7_uint32               intIfNum;

    void                    *next;

} lldpCkptInfo_t;

/*********************************************************************
 *                LLDP-MED defines - End
 *********************************************************************/
/*********************************************************************
* @purpose  Determine if the interface is valid for 802.1AB
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL lldpIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit interval
*
* @param    L7_uint32  interval  @b((input))  interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxIntervalSet(L7_uint32 interval);

/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit interval
*
* @param    L7_uint32  *interval @b((output)) interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxIntervalGet(L7_uint32 *interval);

/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit hold muliplier
*
* @param    L7_uint32  hold      @b((input))  hold multiplier
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxHoldSet(L7_uint32 hold);

/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit hold muliplier
*
* @param    L7_uint32  *hold     @b((output)) hold multiplier
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxHoldGet(L7_uint32 *hold);

/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit reinit delay
*
* @param    L7_uint32  delay     @b((input))  delay in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxReinitDelaySet(L7_uint32 delay);

/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit reinit delay
*
* @param    L7_uint32  *delay     @b((output)) delay in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxReinitDelayGet(L7_uint32 *delay);

/*********************************************************************
*
* @purpose  Sets the 802.1AB global notification interval limit
*
* @param    L7_uint32  interval  @b((input))  interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpNotificationIntervalSet(L7_uint32 interval);

/*********************************************************************
*
* @purpose  Gets the 802.1AB global notification interval limit
*
* @param    L7_uint32  *interval @b((output)) interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpNotificationIntervalGet(L7_uint32 *interval);

/*********************************************************************
*
* @purpose  Sets the 802.1AB transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_uint32  mode      @b((input))  802.1AB transmit mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets the 802.1AB transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  *mode     @b((output)) configured 802.1AB intf mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Sets the 802.1AB receive mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  mode      @b((input))  intf receive mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfRxModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets the 802.1AB receive mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  *mode     @b((output)) configured intf receive mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfRxModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Sets the 802.1AB notify mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_uint32  mode      @b((input)) intf notify mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfNotificationModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets the 802.1AB notify mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  *mode     @b((output)) intf notify mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfNotificationModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Sets the 802.1AB optional transmit TLVs for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_BOOL    portDesc  @b((input))  transmit port desc TLV
* @param    L7_BOOL    sysName   @b((input))  transmit system name TLV
* @param    L7_BOOL    sysDesc   @b((input))  transmit system desc TLV
* @param    L7_BOOL    sysCap    @b((input))  transmit system capability TLV
*
* @returns  L7_SUCCESS, if optional TLVs are set successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxTLVsSet(L7_uint32 intIfNum,
                          L7_BOOL   portDesc,
                          L7_BOOL   sysName,
                          L7_BOOL   sysDesc,
                          L7_BOOL   sysCap);

/*********************************************************************
*
* @purpose  Gets the 802.1AB optional transmit TLVs for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_BOOL    portDesc  @b((output))  transmit port desc TLV
* @param    L7_BOOL    sysName   @b((output))  transmit system name TLV
* @param    L7_BOOL    sysDesc   @b((output))  transmit system desc TLV
* @param    L7_BOOL    sysCap    @b((output))  transmit system capability TLV
*
* @returns  L7_SUCCESS, if values retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxTLVsGet(L7_uint32 intIfNum,
                          L7_BOOL   *portDesc,
                          L7_BOOL   *sysName,
                          L7_BOOL   *sysDesc,
                          L7_BOOL   *sysCap);

/*********************************************************************
*
* @purpose  Sets the 802.1AB mgmt addr transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_BOOL    enabled   @b((input))  transmit mgmt addr TLV
*
* @returns  L7_SUCCESS, if mgmt addr transmit is set successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxMgmtAddrSet(L7_uint32 intIfNum, L7_BOOL enabled);

/*********************************************************************
*
* @purpose  Gets the 802.1AB mgmt addr transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_BOOL    enabled   @b((output))  transmit mgmt addr TLV
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxMgmtAddrGet(L7_uint32 intIfNum, L7_BOOL *enabled);

/*********************************************************************
 *                802.1AB Statistics APIs
 *********************************************************************/

/*********************************************************************
*
* @purpose  Clears all 802.1AB statistics
*
* @param    none
*
* @returns  L7_SUCCESS, if statistics cleared successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsClear(void);

/*********************************************************************
*
* @purpose  Gets the last update time for 802.1AB remote data table
*
* @param    L7_uint32    lastUpate   @b((output))  time of last update
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRemoteLastUpdateGet(L7_uint32 *lastUpdate);

/*********************************************************************
*
* @purpose  Gets the insert count for 802.1AB remote data table
*
* @param    L7_uint32   inserts   @b((output))  insert count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRemoteInsertsGet(L7_uint32 *inserts);

/*********************************************************************
*
* @purpose  Gets the delete count for 802.1AB remote data table
*
* @param    L7_uint32   deletes   @b((output))  delete count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRemoteDeletesGet(L7_uint32 *inserts);

/*********************************************************************
*
* @purpose  Gets the drop count for 802.1AB remote data table
*
* @param    L7_uint32   drops   @b((output))  drop count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRemoteDropsGet(L7_uint32 *drops);

/*********************************************************************
*
* @purpose  Gets the ageout count for 802.1AB remote data table
*
* @param    L7_uint32   ageouts   @b((output))  ageout count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRemoteAgeoutsGet(L7_uint32 *ageouts);

/*********************************************************************
*
* @purpose  Gets the LLDP transmit frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  transmit frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsTxPortFramesTotalGet(L7_uint32 intIfNum, L7_uint32 *total);

/*********************************************************************
*
* @purpose  Gets the LLDP receive frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  receive frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortFramesTotalGet(L7_uint32 intIfNum, L7_uint32 *total);

/*********************************************************************
*
* @purpose  Gets the LLDP discarded frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  discarded frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortFramesDiscardedGet(L7_uint32 intIfNum, L7_uint32 *total);

/*********************************************************************
*
* @purpose  Gets the LLDP errors frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  errors frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortFramesErrorsGet(L7_uint32 intIfNum, L7_uint32 *total);

/*********************************************************************
*
* @purpose  Gets the LLDP ageouts count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  ageouts count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortAgeoutsGet(L7_uint32 intIfNum, L7_uint32 *total);

/*********************************************************************
*
* @purpose  Gets the LLDP TLV discarded count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  discarded count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortTLVsDiscardedGet(L7_uint32 intIfNum, L7_uint32 *total);

/*********************************************************************
*
* @purpose  Gets the LLDP TLV unrecognized count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortTLVsUnrecognizedGet(L7_uint32 intIfNum, L7_uint32 *total);

/*********************************************************************
 *                802.1AB Local Data APIs
 *********************************************************************/

/*********************************************************************
*
* @purpose  Gets the 802.1AB local chassis id subtype
*
* @param    lldpChassisIdSubtype_t  subtype  @b((output))  Chassis ID Subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocChassisIdSubtypeGet(lldpChassisIdSubtype_t *subtype);

/*********************************************************************
*
* @purpose  Gets the 802.1AB local chassis id
*
* @param    L7_uchar8   *chassisId  @b((output))  Chassis ID
* @param    L7_ushort16 *length     @b((output))  Chassis ID length
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    chassisId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           chassisId is encoded based on the ChassisIdSubtype
*
* @end
*********************************************************************/
L7_RC_t lldpLocChassisIdGet(L7_uchar8 *chassisId, L7_ushort16 *length);

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system name
*
* @param    L7_char8  sysName  @b((output))  system name
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    sysName buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpLocSysNameGet(L7_char8 *sysName);

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system description
*
* @param    L7_char8  sysDesc  @b((output))  system description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    sysDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpLocSysDescGet(L7_char8 *sysDesc);

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system capabilities supported
*
* @param    L7_ushort16  sysCap  @b((output))  bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocSysCapSupportedGet(L7_ushort16 *sysCap);

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system capabilities enabled
*
* @param    L7_ushort16  sysCap  @b((output))  bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocSysCapEnabledGet(L7_ushort16 *sysCap);

/*********************************************************************
*
* @purpose  Gets the 802.1AB local port subtype for the specified interface
*
* @param    L7_uint32            intIfNum @b((input))   internal interface number
* @param    lldpPortIdSubtype_t  subtype  @b((output))  Port ID Subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocPortIdSubtypeGet(L7_uint32 intIfNum,
                                lldpPortIdSubtype_t *subtype);

/*********************************************************************
*
* @purpose  Gets the 802.1AB local port id for the specified interface
*
* @param    L7_uint32   intIfNum  @b((input))   internal interface number
* @param    L7_uchar8   *portId   @b((output))  Port ID
* @param    L7_ushort16 *length   @b((output))  Port ID length
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    portId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           portId is encoded based on the ChassisIdSubtype
*
* @end
*********************************************************************/
L7_RC_t lldpLocPortIdGet(L7_uint32   intIfNum,
                         L7_uchar8   *portId,
                         L7_ushort16 *length);

/*********************************************************************
*
* @purpose  Gets the 802.1AB local port description for the specified interface
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_char8  *portDesc  @b((output))  Port Description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    portDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpLocPortDescGet(L7_uint32 intIfNum,
                           L7_char8 *portDesc);

/*********************************************************************
*
* @purpose  Gets the 802.1AB management address
*
* @param    lldpIANAAddrFamilyNumber_t *family   @b((output))   IANA address family
* @param    L7_uchar8                  *address  @b((output))   management address
* @param    L7_uchar8                  *length   @b((output))   address length
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    address is encoded based on family
*
* @end
*********************************************************************/
L7_RC_t lldpLocManAddrGet(lldpIANAAddrFamilyNumber_t *family,
                          L7_uchar8                  *address,
                          L7_uchar8                  *length);

/*********************************************************************
*
* @purpose  Gets the 802.1AB management address interface subtype
*
* @param    lldpManAddrIfSubtype_t *subtype @b((output))  interface subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocManAddrIfSubtypeGet(lldpManAddrIfSubtype_t *subtype);

/*********************************************************************
*
* @purpose  Gets the 802.1AB management address interface id
*
* @param    L7_uint32  *ifId @b((output))  interface id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocManAddrIfIdGet(L7_uint32 *ifId);

/*********************************************************************
*
* @purpose  Gets the 802.1AB management address OID
*
* @param    L7_char8  *addrOID @b((output))  management address OID
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    addrOID buffer should be at least LLDP_MGMT_ADDR_OID_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpLocManAddrOIDGet(L7_char8 *addrOID);


/*********************************************************************
 *                802.1AB Remote Data APIs
 *********************************************************************/


/*********************************************************************
*
* @purpose  Clears all 802.1AB remote data
*
* @param    none
*
* @returns  L7_SUCCESS, if data cleared successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemTableClear(void);

/*********************************************************************
*
* @purpose  Get the unique values to identify an interface remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 *remIndex  @b((output))  remote data index
* @param    L7_uint32 *timestamp @b((output))  remote data timestamp
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    each interface can only have one remote entry.
*
* @end
*********************************************************************/
L7_RC_t lldpRemEntryGet(L7_uint32 intIfNum,
                        L7_uint32 *remIndex,
                        L7_uint32 *timestamp);

/*********************************************************************
*
* @purpose  Get the Next unique values to identify an interface remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 *remIndex  @b((output))  remote data index
* @param    L7_uint32 *timestamp @b((output))  remote data timestamp
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    each interface can only have one remote entry.
*
* @end
*********************************************************************/
L7_RC_t lldpRemEntryGetNext(L7_uint32 intIfNum,
                            L7_uint32 *remIndex,
                            L7_uint32 *timestamp);

/*********************************************************************
*
* @purpose  Get the unique values to identify an interface remote entry
*
* @param    L7_uint32 timestamp @b((input))  remote data timestamp
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex  @b((input))  remote data index
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemTimestampIndexEntryGet(L7_uint32 timestamp,
                                      L7_uint32 intIfNum,
                                      L7_uint32 remIndex);

/*********************************************************************
*
* @purpose  Get the unique values to identify an interface remote entry
*
* @param    L7_uint32 timestamp @b((output))  remote data timestamp
* @param    L7_uint32 intIfNum   @b((output))   internal interface number
* @param    L7_uint32 remIndex  @b((output))  remote data index
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemTimestampIndexEntryGetNext(L7_uint32 *timestamp,
                                          L7_uint32 *intIfNum,
                                          L7_uint32 *remIndex);

/*********************************************************************
*
* @purpose  Get the time to live for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 *ttl       @b((output)) Time to Live seconds
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemTTLGet(L7_uint32 intIfNum,
                      L7_uint32 remIndex,
                      L7_uint32 timestamp,
                      L7_uint32 *ttl);

/*********************************************************************
*
* @purpose  Get the chassis subtype ID for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    lldpChassisIdSubtype_t subtype  @b((output))  remote chassis subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemChassisIdSubtypeGet(L7_uint32 intIfNum,
                                   L7_uint32 remIndex,
                                   L7_uint32 timestamp,
                                   lldpChassisIdSubtype_t *subtype);

/*********************************************************************
*
* @purpose  Get the chassis ID for a remote entry
*
* @param    L7_uint32    intIfNum    @b((input))   internal interface number
* @param    L7_uint32    remIndex    @b((input))   remote data index
* @param    L7_uint32    timestamp   @b((input))   remote data timestamp
* @param    L7_uchar8    *chassisId  @b((output))  buffer to store chassis id
* @param    L7_ushort16  *length     @b((output))  length of chassis id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    chassisId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           chassisId is encoded based on ChassisIdSubtype
*
* @end
*********************************************************************/
L7_RC_t lldpRemChassisIdGet(L7_uint32    intIfNum,
                            L7_uint32    remIndex,
                            L7_uint32    timestamp,
                            L7_uchar8   *chassisId,
                            L7_ushort16 *length);

/*********************************************************************
*
* @purpose  Get the port id subtype for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   remote data index
* @param    L7_uint32 timestamp  @b((input))   remote data timestamp
* @param    lldpPortIdSubtype_t *subtype @b((output))  remote port id subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemPortIdSubtypeGet(L7_uint32 intIfNum,
                                L7_uint32 remIndex,
                                L7_uint32 timestamp,
                                lldpPortIdSubtype_t *subtype);

/*********************************************************************
*
* @purpose  Get the port ID for a remote entry
*
* @param    L7_uint32    intIfNum   @b((input))   internal interface number
* @param    L7_uint32    remIndex   @b((input))   remote data index
* @param    L7_uint32    timestamp  @b((input))   remote data timestamp
* @param    L7_uchar8    *portId    @b((output))  remote port id
* @param    L7_ushort16  *length    @b((output))  length of port id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    portId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           portId buffer is encoded based on portIdSubtype
*
* @end
*********************************************************************/
L7_RC_t lldpRemPortIdGet(L7_uint32    intIfNum,
                         L7_uint32    remIndex,
                         L7_uint32    timestamp,
                         L7_uchar8   *portId,
                         L7_ushort16 *length);

/*********************************************************************
*
* @purpose  Get the port description for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_char8  *portDesc  @b((output)) remote port description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    portDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemPortDescGet(L7_uint32 intIfNum,
                           L7_uint32 remIndex,
                           L7_uint32 timestamp,
                           L7_char8 *portDesc);

/*********************************************************************
*
* @purpose  Get the system name for the remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_char8 *sysName   @b((output)) remote system name
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    sysName buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemSysNameGet(L7_uint32 intIfNum,
                          L7_uint32 remIndex,
                          L7_uint32 timestamp,
                          L7_char8 *sysName);

/*********************************************************************
*
* @purpose  Get the system description for the remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   remote data index
* @param    L7_uint32 timestamp  @b((input))   remote data timestamp
* @param    L7_char8 *sysDesc   @b((output))  remote system description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    sysDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemSysDescGet(L7_uint32 intIfNum,
                          L7_uint32 remIndex,
                          L7_uint32 timestamp,
                          L7_char8 *sysDesc);

/*********************************************************************
*
* @purpose  Get the system capabilities supported for a remote entry
*
* @param    L7_uint32   intIfNum   @b((input))   internal interface number
* @param    L7_uint32   remIndex   @b((input))   remote data index
* @param    L7_uint32   timestamp  @b((input))   remote data timestamp
* @param    L7_ushort16 sysCap     @b((output))  bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemSysCapSupportedGet(L7_uint32    intIfNum,
                                  L7_uint32    remIndex,
                                  L7_uint32    timestamp,
                                  L7_ushort16 *sysCap);

/*********************************************************************
*
* @purpose  Get the system capabilities enabled for a remote entry
*
* @param    L7_uint32   intIfNum   @b((input))  internal interface number
* @param    L7_uint32   remIndex   @b((input))  remote data index
* @param    L7_uint32   timestamp  @b((input))  remote data timestamp
* @param    L7_ushort16 sysCap     @b((output)) bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemSysCapEnabledGet(L7_uint32    intIfNum,
                                L7_uint32    remIndex,
                                L7_uint32    timestamp,
                                L7_ushort16 *sysCap);

/*********************************************************************
*
* @purpose  Get the next valid management address entry for the
*           indicated remote data entry.
*
* @param    L7_uint32                   intIfNum   @b((input))  internal interface number
* @param    L7_uint32                   remIndex   @b((input))  remote data index
* @param    L7_uint32                   timestamp  @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t *family     @b((input/output))   IANA address family
* @param    L7_uchar8                  *addr       @b((input/output))   remote mgmt address
* @param    L7_uchar8                  *length     @b((input/output))   address length
*
* @returns  L7_SUCCESS, entry found, addrIndex is valid
* @returns  L7_FAILURE, no more entries
*
* @notes    if *length == 0, this will return first entry.
*           addr buffer must be at least LLDP_MGMT_ADDR_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemManAddrEntryNextGet(L7_uint32                    intIfNum,
                                   L7_uint32                    remIndex,
                                   L7_uint32                    timestamp,
                                   lldpIANAAddrFamilyNumber_t  *family,
                                   L7_uchar8                   *addr,
                                   L7_uchar8                   *length);

/*********************************************************************
*
* @purpose  Determine if a remote management address entry exists
*
* @param    L7_uint32                  intIfNum  @b((input))  internal interface number
* @param    L7_uint32                  remIndex  @b((input))  remote data index
* @param    L7_uint32                  timestamp @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family    @b((input))   IANA address family
* @param    L7_uchar8                  *addr     @b((input))   remote mgmt address
* @param    L7_uchar8                  length    @b((input))   address length
*
* @returns  L7_SUCCESS, entry exists
* @returns  L7_FAILURE, entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemManAddrEntryGet(L7_uint32                    intIfNum,
                               L7_uint32                    remIndex,
                               L7_uint32                    timestamp,
                               lldpIANAAddrFamilyNumber_t   family,
                               L7_uchar8                   *addr,
                               L7_uchar8                    length);

/*********************************************************************
*
* @purpose  Get interface subtype for a remote management address entry
*
* @param    L7_uint32                  intIfNum   @b((input))  internal interface number
* @param    L7_uint32                  remIndex   @b((input))  remote data index
* @param    L7_uint32                  timestamp  @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family     @b((input))   IANA address family
* @param    L7_uchar8                  *addr      @b((input))   remote mgmt address
* @param    L7_uchar8                  length     @b((input))   address length
* @param    lldpManAddrIfSubtype_t     *subtype   @b((output))  interface subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemManAddrIfSubtypeGet(L7_uint32                    intIfNum,
                                   L7_uint32                    remIndex,
                                   L7_uint32                    timestamp,
                                   lldpIANAAddrFamilyNumber_t   family,
                                   L7_uchar8                   *addr,
                                   L7_uchar8                    length,
                                   lldpManAddrIfSubtype_t      *subtype);

/*********************************************************************
*
* @purpose  Get interface id for a remote management address entry
*
* @param    L7_uint32                  intIfNum   @b((input))  internal interface number
* @param    L7_uint32                  remIndex   @b((input))  remote data index
* @param    L7_uint32                  timestamp  @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family     @b((input))   IANA address family
* @param    L7_uchar8                  *addr      @b((input))   remote mgmt address
* @param    L7_uchar8                  length     @b((input))   address length
* @param    L7_uint32                  *id        @b((output)) interface id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemManAddrIfIdGet(L7_uint32                    intIfNum,
                              L7_uint32                    remIndex,
                              L7_uint32                    timestamp,
                              lldpIANAAddrFamilyNumber_t   family,
                              L7_uchar8                   *addr,
                              L7_uchar8                    length,
                              L7_uint32                   *id);

/*********************************************************************
*
* @purpose  Get OID for the remote management address entry
*
* @param    L7_uint32                  intIfNum   @b((input))   internal interface number
* @param    L7_uint32                  remIndex   @b((input))   remote data index
* @param    L7_uint32                  timestamp  @b((input))   remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family     @b((input))   IANA address family
* @param    L7_uchar8                  *addr      @b((input))   remote mgmt address
* @param    L7_uchar8                  length     @b((input))   address length
* @param    L7_char8                   *oid       @b((output))  management address OID
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    oid buffer must be at least LLDP_MGMT_ADDR_OID_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemManAddrOIDGet(L7_uint32                    intIfNum,
                             L7_uint32                    remIndex,
                             L7_uint32                    timestamp,
                             lldpIANAAddrFamilyNumber_t   family,
                             L7_uchar8                   *addr,
                             L7_uchar8                    length,
                             L7_char8                    *oid);

/*********************************************************************
*
* @purpose  Get the next unknown TLV entry for the indicated remote data entry.
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 *tlvType   @b((input/output)) tlv type
*
* @returns  L7_SUCCESS, next entry found, tlvType is valid
* @returns  L7_FAILURE, no more entries
*
* @notes    if *tlvType == 0, this will return first entry.
*
* @end
*********************************************************************/
L7_RC_t lldpRemUnknownTLVEntryNextGet(L7_uint32  intIfNum,
                                      L7_uint32  remIndex,
                                      L7_uint32  timestamp,
                                      L7_uint32 *tlvType);

/*********************************************************************
*
* @purpose  Determine if unknown TLV entry exists.
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 tlvType    @b((input))  tlv type
*
* @returns  L7_SUCCESS, entry valid
* @returns  L7_FAILURE, entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemUnknownTLVEntryGet(L7_uint32  intIfNum,
                                  L7_uint32  remIndex,
                                  L7_uint32  timestamp,
                                  L7_uint32  tlvType);

/*********************************************************************
*
* @purpose  Get the TLV info for the specified remote entry
*
* @param    L7_uint32   intIfNum   @b((input))  internal interface number
* @param    L7_uint32   remIndex   @b((input))  remote data index
* @param    L7_uint32   timestamp  @b((input))  remote data timestamp
* @param    L7_uint32   tlvType    @b((input))  tlv type
* @param    L7_uchar8   *info      @b((output))  organization defined info
* @param    L7_ushort16 *length    @b((output))  length of info field
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    tlvInfo buffer must be at least LLDP_UNKNOWN_TLV_INFO_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemUnknownTLVInfoGet(L7_uint32    intIfNum,
                                 L7_uint32    remIndex,
                                 L7_uint32    timestamp,
                                 L7_uint32    tlvType,
                                 L7_uchar8   *info,
                                 L7_ushort16 *length);

/*********************************************************************
*
* @purpose  Get the next organizationally defined info entry for
*           the indicated remote data entry.
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 *infoIndex @b((output)) info index
*
* @returns  L7_SUCCESS, entry found, tlvIndex is valid
* @returns  L7_FAILURE, no more entries
*
* @notes    if *infoIndex = 0, this will return first index.
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefInfoEntryNextGet(L7_uint32  intIfNum,
                                      L7_uint32  remIndex,
                                      L7_uint32  timestamp,
                                      L7_uint32 *infoIndex);

/*********************************************************************
*
* @purpose  Determine if an organizationally defined info entry exists.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefInfoEntryGet(L7_uint32    intIfNum,
                                  L7_uint32    remIndex,
                                  L7_uint32    timestamp,
                                  L7_uint32    infoIndex);

/*********************************************************************
*
* @purpose  Get the organizationally defined info for specified entry.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
* @param    L7_uchar8   *info       @b((output))  organization defined info
* @param    L7_ushort16 *length     @b((output))  length of info field
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    orgDefInfo buffer must be at least LLDP_ORG_DEF_INFO_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefInfoGet(L7_uint32    intIfNum,
                             L7_uint32    remIndex,
                             L7_uint32    timestamp,
                             L7_uint32    infoIndex,
                             L7_uchar8   *info,
                             L7_ushort16 *length);
/*********************************************************************
*
* @purpose  Get the organizationally defined subtype for specified entry.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
* @param    L7_uchar8   *subtype    @b((output))  organization defined info
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    subtype buffer must be at least 1 char size
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefSubtypeGet(L7_uint32    intIfNum,
                             L7_uint32    remIndex,
                             L7_uint32    timestamp,
                             L7_uint32    infoIndex,
                             L7_uint32   *subtype);
/*********************************************************************
*
* @purpose  Get the organizationally defined OUI for specified entry.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
* @param    L7_uchar8   *oui       @b((output))  organization defined info
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    oui buffer must be at least LLDP_ORG_DEF_INFO_OUI_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefOUIGet(L7_uint32    intIfNum,
                             L7_uint32    remIndex,
                             L7_uint32    timestamp,
                             L7_uint32    infoIndex,
                             L7_uchar8   *oui);

/*********************************************************************
 *                LLDP-MED APIs
 *********************************************************************/
/*********************************************************************
*
* @purpose  Notification Registration
*
* @param    L7_uint32                  component_ID @b((input)) componant id
* @param    L7_uint32                  L7_uint32 (*notify)
*                   (L7_uint32 intIfNum, lldpXMedNotify_t *data)
*                   @b((input))  function to callback
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedNotificationRegister( L7_COMPONENT_IDS_t component_ID, L7_RC_t (*notify)(L7_uint32 intIfNum, lldpXMedNotify_t *data));
/*********************************************************************
*
* @purpose  Notification DeRegistration
*
* @param    L7_uint32                  component_ID @b((input)) componant id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedNotificationDeregister( L7_COMPONENT_IDS_t component_ID);

/*********************************************************************
 *                LLDP-MED Configuration APIs
 *********************************************************************/
/*********************************************************************
*
* @purpose  Sets the MED admin mode for this interface
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  value @b((input))  admin mode (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedPortAdminModeSet(L7_uint32 intIfNum, L7_uint32 value);
/*********************************************************************
*
* @purpose  Gets the MED admin mode for this interface
*
* @param    L7_uint32                  intIfNum @b((input)) Internal Interface Number
* @param    L7_uint32                  *value @b((output)) configured admin mode (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedPortAdminModeGet(L7_uint32 intIfNum, L7_uint32 *value);
/*********************************************************************
*
* @purpose  Gets whether we have seen a MED TLV and we are transmitting MED TLVs
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                   *status@b((input))  (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The port will transmit only if it has received an MED TLV
*     So even if we are admin mode enabled, we may not neccessarily transmit MED TLVs
*
* @end
*********************************************************************/
L7_RC_t lldpXMedPortTransmitEnabledGet(L7_uint32 intIfNum, L7_uint32 *status);
/*********************************************************************
*
* @purpose  Gets the local device's (This device) MED Classification
*
* @param    L7_uint32                *deviceClass @b((output))  endpoint C I, CII, CII or network connectivity
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocDeviceClassGet(L7_uint32 *deviceClass);
/*********************************************************************
*
* @purpose  Gets the PSE allocated power value
*
* @param    L7_uint32                  *allocPowerValue @b((output))  Allocated power value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocPSEAllocatedPowerValueGet(L7_uint32 intIfNum,L7_uint32 *allocPowerValue);

/*********************************************************************
*
* @purpose  Gets the supported capabilities that could be tranamitted in MED TLVs
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    lldpXMedCapabilities_t     *capSup @b((output))  bit mask of the capabilities supported
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedPortCapSupportedGet(L7_uint32 intIfNum, lldpXMedCapabilities_t *capSup);
/*********************************************************************
*
* @purpose  Sets the TLVs that need to be transmitted in the LLDP PDU
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    lldpXMedCapabilities_t     *capSet @b((output))  Bit mask of the TLVs that will be transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedPortConfigTLVsEnabledSet(L7_uint32 intIfNum, lldpXMedCapabilities_t *capSet);
/*********************************************************************
*
* @purpose  Gets the TLVs that need to be transmitted in the LLDP PDU
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    lldpXMedCapabilities_t     *capGet@b((input))  Bit mask of TLVs that are set for transmision
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedPortConfigTLVsEnabledGet(L7_uint32 intIfNum, lldpXMedCapabilities_t *capGet);
/*********************************************************************
*
* @purpose  Sets the Topology Change notification for this interface
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL                   notif @b((output))  Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedPortConfigNotifEnableSet(L7_uint32 intIfNum, L7_BOOL notif);
/*********************************************************************
*
* @purpose  Gets the Topology Change notification for this interface
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL                   *notif @b((input))  Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedPortConfigNotifEnableGet(L7_uint32 intIfNum, L7_BOOL *notif);
/*********************************************************************
*
* @purpose  Sets the fast start repeat count
*
* @param    L7_uint32                  count @b((input))  fast start repeat count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedFastStartRepeatCountSet(L7_uint32 count);
/*********************************************************************
*
* @purpose  Gets the fast start repeat count
*
* @param    L7_uint32                   *count @b((output)) fast start repeat count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedFastStartRepeatCountGet(L7_uint32 *count);
/*********************************************************************
 *                LLDP-MED Local Device APIs
 *********************************************************************/
/*********************************************************************
*
* @purpose  Gets the vlan id associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param  L7_uint32          policyAppType @b((input)) type application policy
* @param    L7_uint32                  *vlanId @b((output))  Vlan ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocMediaPolicyVlanIDGet(L7_uint32 intIfNum,
                       L7_uint32 policyAppType,
                       L7_uint32 *vlanId);
/*********************************************************************
*
* @purpose  Gets the priority associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param  L7_uint32          policyAppType @b((input)) type application policy
* @param    L7_uint32                  *priority @b((output))  Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocMediaPolicyPriorityGet(L7_uint32 intIfNum,
                         L7_uint32 policyAppType,
                         L7_uint32 *priority );
/*********************************************************************
*
* @purpose  Gets the DSCP associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param  L7_uint32          policyAppType @b((input)) type application policy
* @param    L7_uint32                  *dscp @b((output))  dscp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocMediaPolicyDscpGet(L7_uint32 intIfNum,
                       L7_uint32 policyAppType,
                       L7_uint32 *dscp );
/*********************************************************************
*
* @purpose  Gets the Unknown bit associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param  L7_uint32          policyAppType @b((input)) type application policy
* @param    L7_BOOL                  *unknown @b((output))  unknown bit value (0/1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocMediaPolicyUnknownGet(L7_uint32 intIfNum,
                        L7_uint32 policyAppType,
                        L7_BOOL *unknown );
/*********************************************************************
*
* @purpose  Gets the tagged bit associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param  L7_uint32          policyAppType @b((input)) type application policy
* @param    L7_BOOL                  *tagged @b((output))  tagged bit value (0/1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocMediaPolicyTaggedGet(L7_uint32 intIfNum,
                       L7_uint32 policyAppType,
                       L7_BOOL *tagged );
/*********************************************************************
*
* @purpose  Gets the Hardware revision
*
* @param    L7_uchar8                    *hardwareRev @b((output)) Hardware revision as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocHardwareRevGet(L7_uchar8 *hardwareRev);
/*********************************************************************
*
* @purpose  Gets the Firmware revision
*
* @param    L7_uchar8                    *firmwareRev @b((output)) Firmware revision as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocFirmwareRevGet(L7_uchar8 *firmwareRev);
/*********************************************************************
*
* @purpose  Gets the Software revision
*
* @param    L7_uchar8                    *SoftwareRev @b((output)) Software revision as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocSoftwareRevGet(L7_uchar8 *softwareRev);
/*********************************************************************
*
* @purpose  Gets the Serial Number
*
* @param    L7_uchar8                    *serialNum @b((output)) Serial Number as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocSerialNumGet(L7_uchar8 *serialNum);
/*********************************************************************
*
* @purpose  Gets the Manufacturer's Name
*
* @param    L7_uchar8                    *mfgName @b((output)) Manufacturer's Name as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocMfgNameGet(L7_uchar8 *mfgName);
/*********************************************************************
*
* @purpose  Gets the Model Name
*
* @param    L7_uchar8                    *modelName @b((output)) Model Name as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocModelNameGet(L7_uchar8 *modelName);
/*********************************************************************
*
* @purpose  Gets the Asset ID
*
* @param    L7_uchar8                    *assetID @b((output)) Asset ID as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocAssetIDGet(L7_uchar8 *assetID);
/*********************************************************************
*
* @purpose  Sets the location information as a string for given type of Location ID
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  subType  @b((input)) Type of location information (unkbown, ELIN, Co-ord, civic address)
* @param    L7_uint32                  *info @b((input)) Actual location information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocLocationInfoSet(L7_uint32 intIfNum,
                    L7_uint32 subType,
                    L7_uchar8 *info);
/*********************************************************************
*
* @purpose  Gets the location information as a string for given type of Location ID
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  subType  @b((input)) Type of location information (unkbown, ELIN, Co-ord, civic address)
* @param    L7_uint32                  *info @b((output)) Actual location information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocLocationInfoGet(L7_uint32 intIfNum,
                    L7_uint32 subType,
                    L7_uchar8 *info);
/*********************************************************************
*
* @purpose  Gets the PoE device type.
*
* @param    L7_uint32                *deviceType   @b((output))  Type of power device
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocXPoeDeviceTypeGet(L7_uint32 intIfNum,L7_uint32 *deviceType);
/*********************************************************************
*
* @purpose  Gets the PSE power value in tenths of watts
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  *powerSource @b((output)) type of power source 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is the amount of power supply can give.
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocXPoePSEPowerSourceGet( L7_uint32 *powerSource);
/*********************************************************************
*
* @purpose  Gets the Power source of the PD Device
*
* @param    L7_uint32                  *powerSource @b((output))  Source of the PD
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocXPoePDPowerSourceGet(L7_uint32 intIfNum,L7_uint32 *powerSource);
/*********************************************************************
*
* @purpose  Gets this device's PD priority
*
* @param    L7_uint32                  *priority @b((output))  Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocXPoePDPowerPriorityGet(L7_uint32 intIfNum,L7_uint32 *priority);
/*********************************************************************
*
* @purpose  Gets the PSE port power priority
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  *priority @b((output)) priority 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocXPoePSEPortPriorityGet(L7_uint32 intIfNum, L7_uint32 *priority);
/*********************************************************************
*
* @purpose  Gets the PSE power value in tenths of watts
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  *powerAv @b((output))  Power value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is the amount of power available by the power source device.
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocXPoePSEPortPowerAvGet(L7_uint32 intIfNum, L7_uint32 *powerAv);
/*********************************************************************
*
* @purpose  Gets this devices PD power requirement
*
* @param    L7_uint32                  *pdPowerReq @b((output))  If this device is a PD then its requirement
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocXPoePDPowerReqGet(L7_uint32 intIfNum,L7_uint32 *pdPowerReq);
/*********************************************************************
*
* @purpose  Gets this port's auto nego capabilities
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL                   *support @b((output))  Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPortAutoNegSupportedGet(L7_uint32 intIfNum, L7_BOOL *support);
/*********************************************************************
*
* @purpose   Gets this port's auto nego status
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL                  *enabled @b((output))  Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPortAutoNegEnabledGet(L7_uint32 intIfNum, L7_BOOL *enabled);
/*********************************************************************
*
* @purpose  Gets this port's auto nego advertized capability
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                   *advrtCap @b((output))  String representation of the capability
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPortAutoNegAdvertizedCapGet(L7_uint32 intIfNum, L7_uchar8 *advrtCap);
/*********************************************************************
*
* @purpose  Gets this port's auto nego operational MAU type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  *type @b((output))  MAU Type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPortAutoNegOperMauTypeGet(L7_uint32 intIfNum, L7_uint32 *type);


/* 802.3 Power MDI Local Data API */
/*********************************************************************
*
* @purpose  Gets Local Device Power Class 
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL*                   portClass @b((input)) Pointer to Port Class
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPowerPortClassGet(L7_uint32 intIfNum, L7_BOOL *portClass);

/*********************************************************************
*
* @purpose  Gets Local Device MDISupported value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL*                 mdiSupported @b((input)) Pointer to MDI Supported 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPowerMDISupportedGet(L7_uint32 intIfNum, L7_BOOL *mdiSupported);

/*********************************************************************
*
* @purpose  Gets Local Device MDI Enable value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL*                 mdiEnabled @b((input)) Pointer to MDI Enabled 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPowerMDIEnabledGet(L7_uint32 intIfNum, L7_BOOL *mdiEnabled);

/*********************************************************************
*
* @purpose  Gets Local Device Power Pair contrallable value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL*                 pairControlable @b((input)) pairControlable 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPowerPairControlableGet(L7_uint32 intIfNum, L7_BOOL *pairControlable);

/*********************************************************************
*
* @purpose  Gets Local Device Power Pairs Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               pairs@b((input)) Pointer to power pairs 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPowerPairsGet(L7_uint32 intIfNum, L7_uint32 *pairs);

/*********************************************************************
*
* @purpose  Gets Local Device Detected Power Class Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerClass @b((input)) Pointer to powerClass 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPowerClassGet(L7_uint32 intIfNum, L7_uint32 *powerClass);

/*********************************************************************
*
* @purpose  Gets Local Device Power Type Value
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerType @b((input)) Pointer to power type 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPowerTypeGet(L7_uint32 intIfNum, L7_uint32 *powerType);

/*********************************************************************
*
* @purpose  Gets Local Device Power Source Value
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerSource @b((input)) Pointer to powerSource 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPowerSourceGet(L7_uint32 intIfNum, L7_uint32 *powerSource);

/*********************************************************************
*
* @purpose  Gets Local Device Power Priroty value  
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerPriority @b((input)) Pointer to power priority 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPowerPriorityGet(L7_uint32 intIfNum, L7_uint32 *powerPriority);

/*********************************************************************
*
* @purpose  Gets Local Device PD Requested power value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerValue @b((input)) Pointer to PD Requested power value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPDRequestedPowerValueGet(L7_uint32 intIfNum, L7_uint32 *powerValue);

/*********************************************************************
*
* @purpose  Gets Local Device PSE Allocated Power Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               powerValue @b((input)) Pointer to PSE allocated power value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPSEAllocatedPowerValueGet(L7_uint32 intIfNum, L7_uint32 *powerValue);

/*********************************************************************
*
* @purpose  Gets Local Device Response Time 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               responseTime @b((input)) Pointer to Response time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocResponseTime(L7_uint32 intIfNum, L7_uint32 *responseTime);

/*********************************************************************
*
* @purpose  Gets Local device readiness 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_BOOL*                 ready @b((input)) Pointer to readiness 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocReadyGet(L7_uint32 intIfNum, L7_BOOL *ready);

/*********************************************************************
*
* @purpose  Gets Local Device Reduced Operation Power Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32*               reducedPowerValue @b((input)) Pointer to reduced power value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocReducedOperationPowerValueGet(L7_uint32 intIfNum, L7_uint32 *reducedPowerValue);

/*********************************************************************
*
* @purpose  Sets the Local Device Power Priority 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uchar8                powerPriority@b((input)) Power priority  value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3LocPowerPrioritySet(L7_uint32 intIfNum, L7_uint32 powerPriority);

L7_RC_t lldpXMedRemXPoeParamsGet(L7_uint32 intIfNum, L7_uint32 index, 
                                 lldpXMedNotify_t   *notify);


/*****************************************************************************
******************************************************************************/

/*********************************************************************
 *                LLDP-MED Remote Device APIs
 *********************************************************************/
/*********************************************************************
*
* @purpose  Gets the supported capabilities that was received in MED TLV on this port
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *cap@b((output))  Bit mask of the capabilities
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemCapSupportedGet(L7_uint32 intIfNum,
                    L7_uint32 remIndex,
                    L7_uint32 timestamp,
                    lldpXMedCapabilities_t *cap);
/*********************************************************************
*
* @purpose  Gets the enabled capabilities that was received in MED TLV on this port
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *cap@b((output))  Bit mask of capabilities
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemCapCurrentGet(L7_uint32 intIfNum,
                    L7_uint32 remIndex,
                    L7_uint32 timestamp,
                    lldpXMedCapabilities_t *cap);
/*********************************************************************
*
* @purpose  Gets the remote devices MED class
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *deviceClass @b((output))  MED class of the remote device
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemDeviceClassGet(L7_uint32 intIfNum,
                     L7_uint32 remIndex,
                     L7_uint32 timestamp,
                     L7_uint32 *deviceClass);
/*********************************************************************
*
* @purpose  Gets the vlan id associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param  L7_uint32          policyAppType @b((input)) type application policy
* @param    L7_uint32                  *vlanId @b((output))  Vlan ID
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemMediaPolicyVlanIdGet(L7_uint32 intIfNum,
                       L7_uint32 remIndex,
                       L7_uint32 timestamp,
                       L7_uint32 policyAppType,
                       L7_uint32 *vlanId);
/*********************************************************************
*
* @purpose  Gets the priority associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param  L7_uint32          policyAppType @b((input)) type application policy
* @param    L7_uint32                  *priority @b((output))  Priority
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemMediaPolicyPriorityGet(L7_uint32 intIfNum,
                         L7_uint32 remIndex,
                         L7_uint32 timestamp,
                         L7_uint32 policyAppType,
                         L7_uint32 *priority);
/*********************************************************************
*
* @purpose  Gets the DSCP associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param  L7_uint32          policyAppType @b((input)) type application policy
* @param    L7_uint32                  *dscp @b((output))  dscp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemMediaPolicyDscpGet(L7_uint32 intIfNum,
                       L7_uint32 remIndex,
                       L7_uint32 timestamp,
                       L7_uint32 policyAppType,
                       L7_uint32 *dscp);
/*********************************************************************
*
* @purpose  Gets the Unknown bit associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param  L7_uint32          policyAppType @b((input)) type application policy
* @param    L7_BOOL                  *unknown @b((output))  unknown bit value (0/1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemMediaPolicyUnknownGet(L7_uint32 intIfNum,
                        L7_uint32 remIndex,
                        L7_uint32 timestamp,
                        L7_uint32 policyAppType,
                        L7_BOOL *unknown);
/*********************************************************************
*
* @purpose  Gets the tagged bit associated with a particular policy type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param  L7_uint32          policyAppType @b((input)) type application policy
* @param    L7_BOOL                  *tagged @b((output))  tagged bit value (0/1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemMediaPolicyTaggedGet(L7_uint32 intIfNum,
                       L7_uint32 remIndex,
                       L7_uint32 timestamp,
                       L7_uint32 policyAppType,
                       L7_BOOL *tagged);
/*********************************************************************
*
* @purpose  Gets the remote device's Hardware Revision
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *hardwareRev @b((output))  Hardware revision as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemHardwareRevGet(L7_uint32 intIfNum,
                     L7_uint32 remIndex,
                     L7_uint32 timestamp,
                     L7_uchar8 *hardwareRev);
/*********************************************************************
*
* @purpose  Gets the remote device's Firmware Revision
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *firmwareRev @b((output))  Firmware revision as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemFirmwareRevGet(L7_uint32 intIfNum,
                     L7_uint32 remIndex,
                     L7_uint32 timestamp,
                     L7_uchar8 *firmwareRev);
/*********************************************************************
*
* @purpose  Gets the remote device's Softdware Revision
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *softwareRev @b((output))  Software revision as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemSoftwareRevGet(L7_uint32 intIfNum,
                     L7_uint32 remIndex,
                     L7_uint32 timestamp,
                     L7_uchar8 *softwareRev);
/*********************************************************************
*
* @purpose  Gets the remote device's Hardware Revision
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *serialNum @b((output))  Serial Number as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemSerialNumGet(L7_uint32 intIfNum,
                   L7_uint32 remIndex,
                   L7_uint32 timestamp,
                   L7_uchar8 *serialNum);
/*********************************************************************
*
* @purpose  Gets the remote device's Manufacturer's Name
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *mfgName @b((output))  Manufacturer's name as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemMfgNameGet(L7_uint32 intIfNum,
                   L7_uint32 remIndex,
                   L7_uint32 timestamp,
                   L7_uchar8 *mfgName);
/*********************************************************************
*
* @purpose  Gets the remote device's Model Name
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *modelName @b((output))  Model Name as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemModelNameGet(L7_uint32 intIfNum,
                   L7_uint32 remIndex,
                   L7_uint32 timestamp,
                   L7_uchar8 *modelName);
/*********************************************************************
*
* @purpose  Gets the remote device's Asset ID
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *assetID @b((output))  Asset ID as a string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemAssetIdGet(L7_uint32 intIfNum,
                   L7_uint32 remIndex,
                   L7_uint32 timestamp,
                   L7_uchar8 *assetId);
/*********************************************************************
*
* @purpose  Gets the location info received on this port for the given location subtype
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   subType @b((input))  Loaction subtype
* @param    L7_uchar8                  *info @b((output)) Location information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemLocationInfoGet(L7_uint32 intIfNum,
                    L7_uint32 remIndex,
                    L7_uint32 timestamp,
                    L7_uint32 subType,
                    L7_uchar8 *info);
/*********************************************************************
*
* @purpose  Gets the remote device's Poe device type connected to this port
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *deviceType @b((output))  Poe device type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemXPoeDeviceTypeGet(L7_uint32 intIfNum,
                      L7_uint32 remIndex,
                      L7_uint32 timestamp,
                      L7_uint32 *deviceType);
/*********************************************************************
*
* @purpose  Gets the remote ports PSE power value in tenths of watts
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *powerSrc@b((output)) Power Soruce type 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemXPoePSEPowerAvGet(L7_uint32 intIfNum,
                      L7_uint32 remIndex,
                      L7_uint32 timestamp,
                      L7_uint32 *powerSrc);
/*********************************************************************
*
* @purpose  Gets the remote port's PD power Source
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *powerSrc @b((output))  Power source type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemXPoePSEPowerSrcGet(L7_uint32 intIfNum,
                       L7_uint32 remIndex,
                       L7_uint32 timestamp,
                       L7_uint32 *powerSrc);
/*********************************************************************
*
* @purpose  Gets the remote ports PSE power priority
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *powerPri @b((output)) priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemXPoePSEPowerPriGet(L7_uint32 intIfNum,
                       L7_uint32 remIndex,
                       L7_uint32 timestamp,
                       L7_uint32 *powerPri);
/*********************************************************************
*
* @purpose  Gets the remote port's PD power priority
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *powerPri @b((output))  power priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemXPoePDPowerReqGet(L7_uint32 intIfNum,
                      L7_uint32 remIndex,
                      L7_uint32 timestamp,
                      L7_uint32 *powerPri);
/*********************************************************************
*
* @purpose  Gets the remote ports PSE power value in tenths of watts
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *powerAv @b((output))  Power Available
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemXPoePDPowerSrcGet(L7_uint32 intIfNum,
                      L7_uint32 remIndex,
                      L7_uint32 timestamp,
                      L7_uint32 *powerAv);
/*********************************************************************
*
* @purpose  Gets the remote port's PD power requirement 
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                   *powerReq @b((output)) Power requirement 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedRemXPoePDPowerPriGet(L7_uint32 intIfNum,
                      L7_uint32 remIndex,
                      L7_uint32 timestamp,
                      L7_uint32 *powerReq);
/*********************************************************************
 *                LLDP 802.3 Extension Configuration APIs
 *********************************************************************/
/*********************************************************************
*
* @purpose  Gets remote port's auto nego capability
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL                   *support@b((output)) Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPortAutoNegSupportedGet(L7_uint32 intIfNum,
                         L7_uint32 remIndex,
                         L7_uint32 timestamp,
                         L7_BOOL *support);
/*********************************************************************
*
* @purpose  Gets remote port's auto nego enabled status
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL                   *enabled@b((output)) Truth value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPortAutoNegEnabledGet(L7_uint32 intIfNum,
                         L7_uint32 remIndex,
                         L7_uint32 timestamp,
                         L7_BOOL *enabled);
/*********************************************************************
*
* @purpose  Gets remote ports auto nego advertized capability as a string
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uchar8                  *advrtCap @b((output))  Advertized capability
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPortAutoNegAdvertizedCapGet(L7_uint32 intIfNum,
                           L7_uint32 remIndex,
                           L7_uint32 timestamp,
                           L7_uchar8 *advrtCap);
/*********************************************************************
*
* @purpose  Gets the remote port's Operational MAU Type
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                  remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                  timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32                  *type @b((output))  MAU Type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPortAutoNegOperMauTypeGet(L7_uint32 intIfNum,
                           L7_uint32 remIndex,
                           L7_uint32 timestamp,
                           L7_uint32 *type);

/* 802.3 Power MDI Remote Data API */
/*********************************************************************
*
* @purpose  Gets Remote Device Power Class 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL*                 portClass @b((input)) Pointer to Port Class
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPowerPortClassGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp,L7_BOOL *portClass);

/*********************************************************************
*
* @purpose  Gets Remote Device MDISupported value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL*                 mdiSupported @b((input)) Pointer to MDI Supported 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPowerMDISupportedGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp, L7_BOOL *mdiSupported);

/*********************************************************************
*
* @purpose  Gets Remote Device MDI Enable value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL*                 mdiEnabled @b((input)) Pointer to MDI Enabled 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPowerMDIEnabledGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp, L7_BOOL *mdiEnabled);

/*********************************************************************
*
* @purpose  Gets Remote Device Power Pair contrallable value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_BOOL*                 pairControlable @b((input)) pairControlable 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPowerPairControlableGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp, L7_BOOL *pairControlable);

/*********************************************************************
*
* @purpose  Gets Remote Device Power Pairs Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               pairs@b((input)) Pointer to power pairs 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPowerPairsGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp, L7_uint32 *pairs);

/*********************************************************************
*
* @purpose  Gets Remote Device Detected Power Class Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerClass @b((input)) Pointer to powerClass 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPowerClassGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp, L7_uint32 *powerClass);

/*********************************************************************
*
* @purpose  Gets Remote Device Power Type Value
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerType @b((input)) Pointer to power type 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPowerTypeGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                                 L7_uint32 timestamp,L7_uint32 *powerType);

/*********************************************************************
*
* @purpose  Gets Remote Device Power Source Value
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerSource @b((input)) Pointer to powerSource 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPowerSourceGet(L7_uint32 intIfNum,L7_uint32 remIndex,
                                 L7_uint32 timestamp, L7_uint32 *powerSource);

/*********************************************************************
*
* @purpose  Gets Remote Device Power Priority Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerPriority @b((input)) Pointer to power priority 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPowerPriorityGet(L7_uint32 intIfNum, L7_uint32 remIndex,
                           L7_uint32 timestamp,  L7_uint32 *powerPriority);

/*********************************************************************
*
* @purpose  Gets Remote Device PD Requested power value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerValue @b((input)) Pointer to PD Requested power value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPDRequestedPowerValueGet(L7_uint32 intIfNum,L7_uint32 remIndex,
                                 L7_uint32 timestamp, L7_uint32 *powerValue);

/*********************************************************************
*
* @purpose  Gets Remote Device PSE Allocated Power Value 
*
* @param    L7_uint32                intIfNum  @b((input)) Internal Interface Number
* @param    L7_uint32                remIndex  @b((input)) Index associated with this remote entry
* @param    L7_uint32                timestamp @b((input)) Time Stamp associated with this entry
* @param    L7_uint32*               powerValue @b((input)) Pointer to PSE allocated power value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXdot3RemPSEAllocatedPowerValueGet(L7_uint32 intIfNum,L7_uint32 remIndex,
                                 L7_uint32 timestamp, L7_uint32 *powerValue);

/*********************************************************************
*
* @purpose  Send a message to the LLDP Queue for MED to send network policy parameters in its TLV
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param    lldpXMedPolicyInfoParms_t  *policyInfo @b((input)) policy information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpXMedPolicyEventCallback(L7_uint32 intIfNum, lldpXMedPolicyInfoParms_t *policyInfo);
/*********************************************************************
*
* @purpose  Gets whether a particular policy type is supported on this interface
*
* @param    L7_uint32                  intIfNum  @b((input)) Internal Interface Number
* @param  L7_uint32          policyAppType @b((input)) type application policy
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL lldpXMedLocMediaPolicyAppSupportedGet(L7_uint32 intIfNum,
                        L7_uint32 policyAppType);
/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit delay
*
* @param    L7_uint32  delay  @b((input))  delay in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxDelaySet(L7_uint32 delay);
/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit delay
*
* @param    L7_uint32  *interval @b((output)) interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxDelayGet(L7_uint32 *delay);

/*********************************************************************
*
* @purpose  Gets the LLDP TLV 8021 count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortTLVs8021Get(L7_uint32 intIfNum, L7_uint32 *total);
/*********************************************************************
*
* @purpose  Gets the LLDP TLV 8023 count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortTLVs8023Get(L7_uint32 intIfNum, L7_uint32 *total);
/*********************************************************************
*
* @purpose  Gets the LLDP TLV MED count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortTLVsMEDGet(L7_uint32 intIfNum, L7_uint32 *total);

/*********************************************************************
*
* @purpose  Get the Next Remote device address on an interface
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    This is call that should be called within semaphore unless
*           it is called in the lldp's context.
*
* @end
*********************************************************************/
L7_RC_t lldpRemoteDeviceGetNext(lldpRemDeviceInfoParms_t *parm);
/*********************************************************************
* @purpose  Determine if the interface is valid and capable of supporting poe
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t lldpPoeIsValidIntf(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Determine if the interface is valid and capable of supporting poe PSE
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t lldpPoeIsValidPSEIntf(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Determine if the interface is capable of supporting poe PD.
*
* @param    intIfNum    Internal    Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpPoeIsValidPDIntf(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Sets a flag indicating a change in the Local 802.31AB Database.
*
* @param    intIfNum    Internal    Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocDbChangeSet(L7_uint32 intIfNum);
/*********************************************************************
*
* @purpose  Get Info associated with this port with a specific OUI and Subtype 
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   Remote Index
* @param    L7_uint32 timestamp  @b((input))   Timestamp for this entry
* @param    L7_uint32 infoIndex  @b((input))   InfoIndex for this entry
* @param    L7_uchar8     *oui   @b((input))   Pointer to the OUI
* @param    L7_uint32 *subtype   @b((input))   Pointer to the Subtype
* @param    L7_uchar8    *info   @b((output))   Pointer To the Info
* @param    L7_uint32 *length    @b((output))   Pointer To the Length
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    If incoming OUI and subtype is zero then this routine gets the 
*           numerically smallest OUI/Subtype combination.  
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefEntryInfoGet(L7_uint32    intIfNum,
                                  L7_uint32    remIndex,
                                  L7_uint32    timestamp,
                                  L7_uint32    infoIndex,
                                  L7_uchar8     *oui,
                                  L7_uint32    *subtype,
                                  L7_uchar8    *info,
                                  L7_uint32  *length);
/*********************************************************************
*
* @purpose  Get next Info associated with this port given a specific OUI and Subtype 
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   Remote Index
* @param    L7_uint32 timestamp  @b((input))   Timestamp for this entry
* @param    L7_uint32 *infoIndex  @b((input))  Pointer to InfoIndex for this entry
* @param    L7_uchar8     *oui   @b((input))   Pointer to the OUI
* @param    L7_uint32 *subtype   @b((input))   Pointer to the Subtype
* @param    L7_uchar8    *info   @b((output))   Pointer To the Info
* @param    L7_uint32 *length    @b((output))   Pointer To the Length
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    Starting with the incoming OUI and Subtype this routine returns the next 
*           numerically largest OUI/Subtype combination.  
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefEntryInfoGetNext(L7_uint32    intIfNum,
                                      L7_uint32    remIndex,
                                      L7_uint32    timestamp,
                                      L7_uint32    *infoIndex,
                                      L7_uchar8     *oui,
                                      L7_uint32    *subtype,
                                      L7_uchar8   *info,
                                      L7_uint32 *length);
/*********************************************************************
*
* @purpose  Gets this port power class
*
* @param    L7_uint32                  *portPowerClass @b((output))  Port Power Class
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocXPoePowerPortClassGet(L7_uint32 intIfNum,L7_uint32 *portPowerClass);
/*********************************************************************
*
* @purpose  Gets status of the LLDP 
*
* @param    L7_uint32                  *isReady @b((output)) LLDP Running State 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpXMedLocReadyGet(L7_uint32 unitNum, L7_BOOL *isReady);

#endif /* LLDP_API_H */
