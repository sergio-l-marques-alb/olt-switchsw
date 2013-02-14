/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename util_strings.c
 *
 * @purpose string utilites
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/01/2007
 *
 * @author  Rama Shastri kristipati
 * @end
 *
 **********************************************************************/

#include "lldp_api.h"
#ifndef UTIL_ENUMSTR_H
#define UTIL_ENUMSTR_H

#include "l7_common.h"
#include "acl_exports.h"
#include "defaultconfig.h"
#include "l3_commdefs.h"
#include "wireless_commdefs.h"
#include "util_enumstr.h"

#include "usmdb_unitmgr_api.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "l3_mcast_commdefs.h"
#include "l3_bgp_commdefs.h"
#include "l3_comm_structs.h"
#include "ipv6_commdefs.h"
#include "commdefs.h"
#include "dot1s_exports.h"
#include "fdb_exports.h"
#include "mfdb_exports.h"
#include "chassis_exports.h"

L7_char8 *strUtilEnableDisableGet(L7_int32 val, L7_char8 *def);
L7_char8 *strUtilEnabledDisabledGet(L7_int32 val, L7_char8 *def);
L7_char8 *strUtilActiveInactiveGet(L7_int32 val, L7_char8 *def);
L7_char8 *strUtilEncapTypeGet(L7_int32 val, L7_char8 *def);
L7_char8 *strUtilDvlanTagEtherTypeGet(L7_int32 val, L7_char8 *def);
L7_uchar8 *strUtilOspfNetworkTypeGet(L7_uint32 ospfNetworkType, L7_char8 *def);
L7_char8 *strUtilAuthTypeGet(L7_int32 val, L7_char8 *def);
L7_char8 *strUtilRip2ConfSendTypeGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilRip2ConfReceiveTypeGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilWdmAPFailureTypeGet(L7_uint32 type, L7_char8 *def);
L7_char8 *strWdmRFScanStatusGet(L7_uint32 status, L7_char8 *def);
L7_char8 *strWdmDetectedClientStatusGet(L7_uint32 status, L7_char8 *def);
L7_char8 *strClientStatusEntryGet(L7_uint32 status, L7_char8 *def);
L7_char8 *strUtilLinkStateGet(L7_int32 val, L7_char8 *def);
L7_char8 *strUtilTransferModeGet(L7_TRANSFER_TYPES_t val, L7_char8 *def);
L7_char8 *strUtilFileTypeGet(L7_FILE_TYPES_t val, L7_char8 *def);
L7_char8 *strUtilSntpStatusGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilDot1qTypeGet(L7_int32 val, L7_char8 *def);
L7_char8 *strUtilUnitMgrMgmtPrefGet(usmDbUnitMgrMgmtPref_t val, L7_char8 *def);
L7_char8 *strUtilDiffservPolicyTypeGet(L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t val, L7_char8 *def);
L7_char8 *strUtilYesNoGet(L7_BOOL val, L7_char8 *def);
L7_char8 *strUtilTrueFalseGet(L7_BOOL val, L7_char8 *def);
L7_char8 *strUtilQosCosQueueMgmtTypeGet(L7_QOS_COS_QUEUE_MGMT_TYPE_t val, L7_char8 *def);
L7_char8 *strUtilSpecialPortTypeGet(L7_SPECIAL_PORT_TYPE_t val, L7_char8 *def);
L7_char8 *strUtilOspfVirtNbrTypeGet(L7_OSPF_VIRT_NBR_STATE_t val, L7_char8 *def);
L7_char8 *strUtilOspfNssaTransStateGet(L7_OSPF_NSSA_TRANSLATOR_STATE_t val, L7_char8 *def);
L7_char8 *strUtilospfNssaTranslatorTypeGet(L7_OSPF_NSSA_TRANSLATOR_ROLE_t val, L7_char8 *def);
L7_char8 *strUtilOspfMetricTypeGet(L7_OSPF_EXT_METRIC_TYPES_t val, L7_char8 *def);
L7_char8 *strUtilOspfIntfTypeGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilOspfIntfTypeStringGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilOspfMessageGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilOspfMessageTypeGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilOspfAreaSummaryGet(L7_OSPF_AREA_SUMMARY_t val, L7_char8 *def);
L7_char8 *strUtilOspfAreaImportTypeGet(L7_OSPF_AREA_EXT_ROUTING_t val);
L7_char8 *strUtilMcastProtocolTypeGet(L7_MCAST_IANA_PROTO_ID_t val, L7_char8 *def);
L7_char8 *strUtilIntfStateGet(L7_PORT_EVENTS_t val, L7_char8 *def);
L7_char8 *strUtilDot1sPortRoleStateGet(L7_DOT1S_PORT_ROLE_t val, L7_char8 *def);
L7_char8 *strUtilDot1sPortRoleGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilEnableDisableGet1(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilAclProtoGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilPortSpeedGet(L7_PORT_SPEEDS_t val, L7_char8 *def);
L7_char8 *strUtilWdmClientStateGet(L7_WDM_CLIENT_STATUS_t status, L7_char8 *def);
L7_char8 *strUtilApStatusGet(L7_WDM_AP_STATUS_t status, L7_char8 *def);
L7_char8 *strUtilWdmActionStatusGet(L7_WDM_ACTION_STATUS_t status, L7_char8 *def);
L7_char8 *strUtilBgpStatusGet(L7_BGP_PEER_STATE_t status, L7_char8 *def);
L7_char8 *strUtilAlarmStatusGet(L7_MODULE_ALARM_STATUS_t val, L7_char8 *def);
L7_char8 *strUtilMfdbTypeGet(L7_MFDB_ENTRY_TYPE_t val, L7_char8 *def);
L7_char8 *strUtilAclDirectionGet(L7_ACL_DIRECTION_t val, L7_char8 *def);
L7_char8 *strUtilMgmdErrCodeGet(L7_MGMD_ERROR_CODE_t val, L7_char8 *def);
L7_char8 *strUtilOspfVirtualLinkGet(L7_OSPF_AREA_EXT_ROUTING_t val, L7_char8 *def);
L7_char8 *strUtilOspfAreaImportGet(L7_OSPF_AREA_EXT_ROUTING_t val, L7_char8 *def);
L7_char8 *strUtilFdbAddrEntryTypeGet(L7_FDB_ADDR_FLAG_t val, L7_char8 *def);
L7_char8 *strUtilMcastPimsmTypeGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilApHealthStatusGet(L7_MODULE_HEALTH_STATUS_t val, L7_char8 *def);
L7_char8 *strUtilOspfV3EntryTypeGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilOspfAreaAggLsdbTypeGet(L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t val, L7_char8 *def);
L7_char8 *strUtilIpv6NbrStateGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilDhcp6PrefixTypeGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilReDistRouteTypeGet(L7_REDIST_RT_INDICES_t val, L7_char8 *def);
L7_char8 *strUtilWdmQosQueueTypeGet(L7_WDM_QOS_QUEUES_t val, L7_char8 *def);
L7_char8 *strUtilDelAreaError(L7_RC_t val, L7_char8 *def);
L7_char8 *strUtilRtoEntryTypeGet(L7_RTO_PROTOCOL_INDICES_t val, L7_char8 *def);
L7_char8 *strUtilOspfPathTypeGet(L7_OSPF_PATH_TYPE_t val, L7_char8 *def);
L7_char8 *strUtilRouteTypeInfoGet(L7_uint32 val, L7_char8 *def);
L7_char8 *strUtilUpDownGet(L7_int32 val, L7_char8 * def);
L7_char8 *strUtilLLdpMedCapabilityGet(lldpXMedCapabilities_t medCap, L7_char8 *buf);
L7_char8 *strUtilHashModeGet(L7_int32 hashMode);
L7_RC_t strUtilUdpPortToString(L7_ushort16 udpPort, L7_uchar8 *portString);
L7_ushort16 strUtilUdpPortNameToNumber(const L7_char8 *portString);

#endif  /* UTIL_ENUMSTR_H */
