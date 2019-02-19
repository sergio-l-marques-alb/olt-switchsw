/**************************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2009
*
***************************************************************************
*
* @filename   strlib_dot1ag_cli.h
*
* @purpose    Strings Library for dot1ag component
*
* @component  Strings Library
*
* @comments   none
*
* @create     02/20/2009
*
* @author     Siva Mukka
*
* @end
*
**************************************************************************/

#ifndef STRLIB_DOT1AG_CLI_H
#define STRLIB_DOT1AG_CLI_H

#include "datatypes.h"

extern L7_char8 *pStrInfo_dot1ag_cache;
extern L7_char8 *pStrInfo_dot1ag_cc;
extern L7_char8 *pStrInfo_dot1ag_cfm;
extern L7_char8 *pStrInfo_dot1ag_holdtime;
extern L7_char8 *pStrInfo_dot1ag_level;
extern L7_char8 *pStrInfo_dot1ag_mep;
extern L7_char8 *pStrInfo_dot1ag_mep_up;
extern L7_char8 *pStrInfo_dot1ag_mep_down;
extern L7_char8 *pStrInfo_dot1ag_mip;
extern L7_char8 *pStrInfo_dot1ag_traceroute;
extern L7_char8 *pStrInfo_dot1ag_domain_name;
extern L7_char8 *pStrInfo_dot1ag_direction;
extern L7_char8 *pStrInfo_dot1ag_up;
extern L7_char8 *pStrInfo_dot1ag_down;
extern L7_char8 *pStrInfo_dot1ag_mpid;
extern L7_char8 *pStrInfo_dot1ag_rmpid;
extern L7_char8 *pStrInfo_dot1ag_maintainance_points;
extern L7_char8 *pStrInfo_dot1ag_remote;
extern L7_char8 *pStrInfo_dot1ag_traceroute_cache;
extern L7_char8 *pStrInfo_dot1ag_local;
extern L7_char8 *pStrInfo_dot1ag_ttl;
extern L7_char8 *pStrInfo_dot1ag_sequenceNumberTag;
extern L7_char8 *pStrInfo_dot1ag_sequenceNumber;
extern L7_char8 *pStrErr_dot1ag_CcmInterval_1;
extern L7_char8 *pStrErr_dot1ag_CcmInterval_10;
extern L7_char8 *pStrErr_dot1ag_CcmInterval_60;
extern L7_char8 *pStrErr_dot1ag_CcmInterval_600;
extern L7_char8 *pStrInfo_dot1ag_archive_holdtime;
extern L7_char8 *pStrInfo_dot1ag_MaintainanceDomainPrompt;
extern L7_char8 *pStrInfo_dot1ag_PrivUserExecMaintainaceModePrompt;

extern L7_char8 *pStrErr_dot1ag_StatsOutOfSeqCCMsRx;
extern L7_char8 *pStrErr_dot1ag_StatsCCMsTx;
extern L7_char8 *pStrErr_dot1ag_StatsInOrderLBRsRx;
extern L7_char8 *pStrErr_dot1ag_StatsOutofOrderLBRsRx;
extern L7_char8 *pStrErr_dot1ag_StatsBadMsduLBRsRx;
extern L7_char8 *pStrErr_dot1ag_StatsLBRsTx;
extern L7_char8 *pStrErr_dot1ag_StatsUnexpectedLTRsRx;

extern L7_char8 *pStrErr_dot1ag_DomainLevelInvalid;
extern L7_char8 *pStrErr_dot1ag_SequenceNumberInvalid;
extern L7_char8 *pStrErr_dot1ag_CCEnableSyntax;


extern L7_char8 *pStrErr_dot1ag_InvalidDomainName;
extern L7_char8 *pStrErr_dot1ag_InvalidServiceName;
extern L7_char8 *pStrErr_dot1ag_InvalidServiceVlanId;
extern L7_char8 *pStrErr_dot1ag_NonAlphaNumericDomainName;
extern L7_char8 *pStrErr_dot1ag_NonAlphaNumericServiceName;
extern L7_char8 *pStrErr_dot1ag_InvalidDomainLevel;
extern L7_char8 *pStrErr_dot1ag_InvalidPingCount;
extern L7_char8 *pStrErr_dot1ag_SystemBusyFail;
extern L7_char8 *pStrErr_dot1ag_DomainDonotExists;
extern L7_char8 *pStrErr_dot1ag_DomainCreateFail;
extern L7_char8 *pStrErr_dot1ag_DomainDeleteFail;
extern L7_char8 *pStrErr_dot1ag_DomainMaxLimitReached;
extern L7_char8 *pStrErr_dot1ag_Enable;
extern L7_char8 *pStrErr_dot1ag_Disable;
extern L7_char8 *pStrErr_dot1ag_HoldTimeInvalid;
extern L7_char8 *pStrErr_dot1ag_IntervalInvalid;
extern L7_char8 *pStrErr_dot1ag_MpidInvalid;
extern L7_char8 *pStrErr_dot1ag_MepArchiveCacheHoldTime;
extern L7_char8 *pStrErr_dot1ag_ServicesMaxLimitReached;
extern L7_char8 *pStrErr_dot1ag_ServiceCreateFailed;
extern L7_char8 *pStrErr_dot1ag_ServiceDeleteFailed;
extern L7_char8 *pStrErr_dot1ag_ServiceDoesNotExists;



extern L7_char8 *pStrErr_dot1ag_CC_Enable;
extern L7_char8 *pStrErr_dot1ag_CCTxInterval;
extern L7_char8 *pStrErr_dot1ag_TraceRouteCacheClear;
extern L7_char8 *pStrErr_dot1ag_MipCreateFail;
extern L7_char8 *pStrErr_dot1ag_MipDeleteFail;
extern L7_char8 *pStrErr_dot1ag_MepCreateFail;
extern L7_char8 *pStrErr_dot1ag_MepDeleteFail;
extern L7_char8 *pStrErr_dot1ag_MepEnableSetFail;
extern L7_char8 *pStrErr_dot1ag_MepActiveModeSetFail;
extern L7_char8 *pStrErr_dot1ag_RmepClearFail;

extern L7_char8 *pStrErr_dot1ag_DomainExistsButInvalid;
extern L7_char8 *pStrErr_dot1ag_DomainNameLevelCombinationInvalid;
extern L7_char8 *pStrErr_dot1ag_ServiceNameVidCombinationInvalid;
extern L7_char8 *pStrErr_dot1ag_ServiceAlreadyExists;
extern L7_char8 *pStrErr_dot1ag_LtmFail;


extern L7_char8 *pStrInfo_dot1ag_cfm_help;
extern L7_char8 *pStrInfo_dot1ag_cc_help;
extern L7_char8 *pStrInfo_dot1ag_level_help;
extern L7_char8 *pStrInfo_dot1ag_level_range_help;
extern L7_char8 *pStrInfo_dot1ag_enable_help;
extern L7_char8 *pStrInfo_dot1ag_domain_help;
extern L7_char8 *pStrInfo_dot1ag_interval_help;
extern L7_char8 *pStrInfo_dot1ag_interval_1sec_help;
extern L7_char8 *pStrInfo_dot1ag_interval_10secs_help;
extern L7_char8 *pStrInfo_dot1ag_interval_60secs_help;
extern L7_char8 *pStrInfo_dot1ag_interval_600secs_help;
extern L7_char8 *pStrInfo_dot1ag_domain_name_help;
extern L7_char8 *pStrInfo_dot1ag_domain_name_length_help;
extern L7_char8 *pStrInfo_dot1ag_service_name_length_help;
extern L7_char8 *pStrInfo_dot1ag_interface_show_help;
extern L7_char8 *pStrInfo_dot1ag_dot1ag_enable_help;
extern L7_char8 *pStrInfo_dot1ag_traceroute_help;
extern L7_char8 *pStrInfo_dot1ag_cache_help;
extern L7_char8 *pStrInfo_dot1ag_sequnceNumber_help;
extern L7_char8 *pStrInfo_dot1ag_holdtime_help;
extern L7_char8 *pStrInfo_dot1ag_holdtime_range_help;
extern L7_char8 *pStrInfo_dot1ag_mep_help;
extern L7_char8 *pStrInfo_dot1ag_mepActivate_help;
extern L7_char8 *pStrInfo_dot1ag_mip_help;
extern L7_char8 *pStrInfo_dot1ag_rempteMepIdEnter_help;
extern L7_char8 *pStrInfo_dot1ag_remoteMepMacEnter_help;
extern L7_char8 *pStrInfo_dot1ag_MpMacEnter_help;
extern L7_char8 *pStrInfo_dot1ag_direction_help;
extern L7_char8 *pStrInfo_dot1ag_mep_up_direction_help;
extern L7_char8 *pStrInfo_dot1ag_mep_down_direction_help;
extern L7_char8 *pStrInfo_dot1ag_mpid_help;
extern L7_char8 *pStrInfo_dot1ag_mpid_range_help;
extern L7_char8 *pStrInfo_dot1ag_clear_cfm_errors_help;
extern L7_char8 *pStrInfo_dot1ag_maintainance_points_help;
extern L7_char8 *pStrInfo_dot1ag_remote_maintenance_points_help;
extern L7_char8 *pStrInfo_dot1ag_SendingLbmMessageWithTimeout;
extern L7_char8 *pStrInfo_dot1ag_tracerouteCacheClear_help;
extern L7_char8 *pStrInfo_dot1ag_sequenceNumberEnter_help;
extern L7_char8 *pStrInfo_dot1ag_tracerouteCacheShow_help;
extern L7_char8 *pStrInfo_dot1ag_cfmDisplay_help;
extern L7_char8 *pStrInfo_dot1ag_cfmStatisticsShowHelp;
extern L7_char8 *pStrInfo_dot1ag_domain_summary_help;
extern L7_char8 *pStrInfo_dot1ag_domain_params_help;
extern L7_char8 *pStrInfo_dot1ag_show_cfm_errors_help;
extern L7_char8 *pStrInfo_dot1ag_show_mp_params_help;
extern L7_char8 *pStrInfo_dot1ag_show_local_mp_help;
extern L7_char8 *pStrInfo_dot1ag_show_remote_mp_help;
extern L7_char8 *pStrInfo_dot1ag_service_help;
extern L7_char8 *pStrInfo_dot1ag_archive_holdtime_help;
extern L7_char8 *pStrInfo_dot1ag_archive_holdtime_range_help;



#endif
