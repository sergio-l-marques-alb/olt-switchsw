/**************************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2009
*
***************************************************************************
*
* @filename   strlib_dot1ag_cli.c
*
* @purpose    Strings Library for Dot1ag Component.
*
* @component  Common Strings Library
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

#include "datatypes.h"

/* CLI*/
L7_char8 *pStrInfo_dot1ag_cache            = "cache";
L7_char8 *pStrInfo_dot1ag_cc               = "cc";
L7_char8 *pStrInfo_dot1ag_cfm              = "cfm";
L7_char8 *pStrInfo_dot1ag_holdtime         = "hold-time";
L7_char8 *pStrInfo_dot1ag_level            = "level";
L7_char8 *pStrInfo_dot1ag_mep              = "mep";
L7_char8 *pStrInfo_dot1ag_mip              = "mip";
L7_char8 *pStrInfo_dot1ag_mep_up           = "mep-up";
L7_char8 *pStrInfo_dot1ag_mep_down         = "mep-down";
L7_char8 *pStrInfo_dot1ag_traceroute       = "traceroute";
L7_char8 *pStrInfo_dot1ag_domain_name      = "<domain-name>";
L7_char8 *pStrInfo_dot1ag_direction        = "direction";
L7_char8 *pStrInfo_dot1ag_up               = "up";
L7_char8 *pStrInfo_dot1ag_down             = "down";
L7_char8 *pStrInfo_dot1ag_mpid             = "mpid";
L7_char8 *pStrInfo_dot1ag_rmpid            = "remote-mpid";
L7_char8 *pStrInfo_dot1ag_maintainance_points  = "maintenance-points";
L7_char8 *pStrInfo_dot1ag_remote           = "remote";
L7_char8 *pStrInfo_dot1ag_traceroute_cache = "traceroute-cache";
L7_char8 *pStrInfo_dot1ag_local            = "local";
L7_char8 *pStrInfo_dot1ag_ttl              = "ttl";
L7_char8 *pStrInfo_dot1ag_sequenceNumberTag= "sequence-num";
L7_char8 *pStrInfo_dot1ag_sequenceNumber   = "<sequence-num>";
L7_char8 *pStrErr_dot1ag_CcmInterval_1     = "1";
L7_char8 *pStrErr_dot1ag_CcmInterval_10    = "10";
L7_char8 *pStrErr_dot1ag_CcmInterval_60    = "60";
L7_char8 *pStrErr_dot1ag_CcmInterval_600   = "600";

L7_char8 *pStrInfo_dot1ag_archive_holdtime             = "archive-hold-time";
L7_char8 *pStrInfo_dot1ag_MaintainanceDomainPrompt             = "(Config-cfm-mdomain)#";
L7_char8 *pStrInfo_dot1ag_PrivUserExecMaintainaceModePrompt   = "(Config-ether-cfm-domain)#";

L7_char8 *pStrErr_dot1ag_StatsOutOfSeqCCMsRx    = "Out-of-sequence CCM's received";
L7_char8 *pStrErr_dot1ag_StatsCCMsTx            = "CCM's transmitted";
L7_char8 *pStrErr_dot1ag_StatsInOrderLBRsRx     = "In-order Loopback Replies received";
L7_char8 *pStrErr_dot1ag_StatsOutofOrderLBRsRx  = "Out-of-order Loopback Replies received";
L7_char8 *pStrErr_dot1ag_StatsBadMsduLBRsRx     = "Bad MSDU Loopback Replies received";
L7_char8 *pStrErr_dot1ag_StatsLBRsTx            = "Loopback Replies transmitted";
L7_char8 *pStrErr_dot1ag_StatsUnexpectedLTRsRx  = "Unexpected LTR's received";

L7_char8 *pStrErr_dot1ag_DomainLevelInvalid  = "Invalid domain level";
L7_char8 *pStrErr_dot1ag_SequenceNumberInvalid  = "Invalid sequence number";

L7_char8 *pStrErr_dot1ag_InvalidDomainName = "Invalid domain name. Enter a name up to 43 alphanumeric characters including -, _, ' '.";
L7_char8 *pStrErr_dot1ag_InvalidServiceName = "Invalid service name. Enter a name up to 45 alphanumeric characters including -, _, ' '.";
L7_char8 *pStrErr_dot1ag_InvalidServiceVlanId = "Invalid service vlan id";
L7_char8 *pStrErr_dot1ag_NonAlphaNumericDomainName = "Invalid domain name. Domain name may only contain alphanumeric characters including -, _, ' '.";
L7_char8 *pStrErr_dot1ag_NonAlphaNumericServiceName = "Invalid service name. Service name may only contain alphanumeric characters including -, _, ' '.";
L7_char8 *pStrErr_dot1ag_InvalidDomainLevel = "Invalid domain level";
L7_char8 *pStrErr_dot1ag_InvalidPingCount = "Invalid Ping Count";
L7_char8 *pStrErr_dot1ag_SystemBusyFail = "System is Busy!! Reissue the command after some time";
L7_char8 *pStrErr_dot1ag_DomainDonotExists = "Specified domain does not exist";
L7_char8 *pStrErr_dot1ag_DomainCreateFail = "Failed to create domain";
L7_char8 *pStrErr_dot1ag_DomainDeleteFail = "Failed to delete domain";
L7_char8 *pStrErr_dot1ag_DomainMaxLimitReached = "Maximum domains limit is reached";
L7_char8 *pStrErr_dot1ag_Enable  = "Failed to enable cfm";
L7_char8 *pStrErr_dot1ag_Disable = "Failed to disable cfm";
L7_char8 *pStrErr_dot1ag_HoldTimeInvalid = "Invalid hold time";
L7_char8 *pStrErr_dot1ag_IntervalInvalid = "Invalid interval";
L7_char8 *pStrErr_dot1ag_MpidInvalid = "Invalid mpid";
L7_char8 *pStrErr_dot1ag_MepArchiveCacheHoldTime = "Failed to set mep archive hold time";
L7_char8 *pStrErr_dot1ag_ServicesMaxLimitReached = "Maximum services limit is reached";
L7_char8 *pStrErr_dot1ag_ServiceCreateFailed = "Failed to create service";
L7_char8 *pStrErr_dot1ag_ServiceDeleteFailed = "Failed to delete service";
L7_char8 *pStrErr_dot1ag_ServiceDoesNotExists = "Specified service does not exists";

L7_char8 *pStrErr_dot1ag_CC_Enable = "Failed to enable transmission of CCMs";
L7_char8 *pStrErr_dot1ag_CCTxInterval = "Failed to set transmission interval of CCMs";
L7_char8 *pStrErr_dot1ag_TraceRouteCacheClear = "Failed to clear traceroute cache";
L7_char8 *pStrErr_dot1ag_MipCreateFail = "Failed to create MIP";
L7_char8 *pStrErr_dot1ag_MipDeleteFail = "Failed to delete MIP";
L7_char8 *pStrErr_dot1ag_MepCreateFail = "Failed to create MEP";
L7_char8 *pStrErr_dot1ag_MepDeleteFail = "Failed to delete MEP";
L7_char8 *pStrErr_dot1ag_MepEnableSetFail = "Failed to Enable or Disable the MEP";
L7_char8 *pStrErr_dot1ag_MepActiveModeSetFail = "Failed to set the MEP active mode";
L7_char8 *pStrErr_dot1ag_RmepClearFail = "Failed to remote MEP data";

L7_char8 *pStrErr_dot1ag_DomainExistsButInvalid = "Domain with a given name or level exists already";
L7_char8 *pStrErr_dot1ag_DomainNameLevelCombinationInvalid = "Failed to find a domain with a given name and level";
L7_char8 *pStrErr_dot1ag_ServiceNameVidCombinationInvalid = "Failed to find a service with a given name and vlan id";
L7_char8 *pStrErr_dot1ag_ServiceAlreadyExists = "Service with a given name or vlan id already exists in a specified domain";
L7_char8 *pStrErr_dot1ag_LtmFail = "L2 Traceroute Failed";


L7_char8 *pStrInfo_dot1ag_cfm_help      = "Configure cfm";
L7_char8 *pStrInfo_dot1ag_cc_help      = "configure cc";
L7_char8 *pStrInfo_dot1ag_level_help      = "Configure level";
L7_char8 *pStrInfo_dot1ag_level_range_help      = "Enter maintenance domain level";
L7_char8 *pStrInfo_dot1ag_enable_help      = "Configure enable";
L7_char8 *pStrInfo_dot1ag_domain_help      = "Configure domain";
L7_char8 *pStrInfo_dot1ag_interval_help      = "Configure CCM transmit interval";
L7_char8 *pStrInfo_dot1ag_interval_1sec_help        = "set CCM interval to 1 second";
L7_char8 *pStrInfo_dot1ag_interval_10secs_help      = "set CCM interval to 10 seconds";
L7_char8 *pStrInfo_dot1ag_interval_60secs_help      = "set CCM interval to 60 seconds";
L7_char8 *pStrInfo_dot1ag_interval_600secs_help     = "set CCM interval to 600 seconds";
L7_char8 *pStrInfo_dot1ag_domain_name_help = "Enter maintenance domain name";
L7_char8 *pStrInfo_dot1ag_domain_name_length_help = "Enter a name up to 43 alphanumeric characters including -, _, ' '.";
L7_char8 *pStrInfo_dot1ag_service_name_length_help = "Enter a name up to 45 alphanumeric characters including -, _, ' '.";
L7_char8 *pStrInfo_dot1ag_interface_show_help = "Enter ethernet cfm interface";
L7_char8 *pStrInfo_dot1ag_dot1ag_enable_help = "Enable dot1ag";
L7_char8 *pStrInfo_dot1ag_traceroute_help = "Configure traceroute";
L7_char8 *pStrInfo_dot1ag_cache_help = "Configure cache";
L7_char8 *pStrInfo_dot1ag_holdtime_help = "Configure hold-time";
L7_char8 *pStrInfo_dot1ag_holdtime_range_help = "Enter hold time in seconds";
L7_char8 *pStrInfo_dot1ag_mep_help = "Configure mep";
L7_char8 *pStrInfo_dot1ag_mepActivate_help = "Set MEP active mode";
L7_char8 *pStrInfo_dot1ag_mip_help = "Configure mip";
L7_char8 *pStrInfo_dot1ag_rempteMepIdEnter_help = "Enter remote MEP Id";
L7_char8 *pStrInfo_dot1ag_remoteMepMacEnter_help = "Enter remote MEP MAC Address";
L7_char8 *pStrInfo_dot1ag_MpMacEnter_help = "Enter maintenance point MAC Address";
L7_char8 *pStrInfo_dot1ag_direction_help = "Configure mep direction";
L7_char8 *pStrInfo_dot1ag_mep_up_direction_help = "Select up direction";
L7_char8 *pStrInfo_dot1ag_mep_down_direction_help = "Select down direction";
L7_char8 *pStrInfo_dot1ag_mpid_help = "Configure mep id";
L7_char8 *pStrInfo_dot1ag_mpid_range_help = "Enter mep id";
L7_char8 *pStrInfo_dot1ag_clear_cfm_errors_help = "Clear cfm errors";
L7_char8 *pStrInfo_dot1ag_maintainance_points_help = "Configure maintenance-points";
L7_char8 *pStrInfo_dot1ag_remote_maintenance_points_help = "Configure remote maintenance-points";
L7_char8 *pStrInfo_dot1ag_SendingLbmMessageWithTimeout = "Sending %d Ethernet CFM loopback messages, timeout is %d seconds:";
L7_char8 *pStrInfo_dot1ag_tracerouteCacheClear_help = "Clear traceroute-cache";
L7_char8 *pStrInfo_dot1ag_sequenceNumberEnter_help = "Enter sequence number";
L7_char8 *pStrInfo_dot1ag_tracerouteCacheShow_help = "Display Trace-route cache";
L7_char8 *pStrInfo_dot1ag_cfmDisplay_help      = "Display ethernet cfm params";
L7_char8 *pStrInfo_dot1ag_cfmStatisticsShowHelp = "Displays ethernet cfm statistics";

L7_char8 *pStrInfo_dot1ag_domain_summary_help = "Display maintenance domains summary";
L7_char8 *pStrInfo_dot1ag_domain_params_help = "Display maintenance domains params";
L7_char8 *pStrInfo_dot1ag_show_cfm_errors_help = "Display cfm errors ";
L7_char8 *pStrInfo_dot1ag_show_mp_params_help = "Display maintenance points params";
L7_char8 *pStrInfo_dot1ag_show_local_mp_help = "Display local maintenance points parms";
L7_char8 *pStrInfo_dot1ag_show_remote_mp_help = "Display remote maintenance points parms";
L7_char8 *pStrInfo_dot1ag_service_help = "Create or Delete a service";
L7_char8 *pStrInfo_dot1ag_archive_holdtime_help = "Configure archive hold time";
L7_char8 *pStrInfo_dot1ag_archive_holdtime_range_help = "Specify archive hold time in seconds";
