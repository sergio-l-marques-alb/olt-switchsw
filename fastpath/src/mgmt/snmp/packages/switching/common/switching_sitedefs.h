/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename switching_sitedefs.h
*
* @purpose The purpose of this file is to include definitions from
*          switching package
*
* @component SNMP
*
* @comments none
*
* @create 12/12/2003
*
* @author ragrawal
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef SWITCHING_SITEDEFS_H
#define SWITCHING_SITEDEFS_H

#ifdef __cplusplus
extern "C" {
#endif


/*
 * #defines for the Undo methods of different Tabular and Scalar MIB Families in
 * switching package.
*/
#define SR_agentTelnetConfigGroup_UNDO
#define SR_agentUserConfigGroup_UNDO
#define SR_agentUserConfigEntry_UNDO
#define SR_agentSerialGroup_UNDO
#define SR_agentLagConfigGroup_UNDO
#define SR_agentLagSummaryConfigEntry_UNDO
#define SR_agentNetworkConfigGroup_UNDO
#define SR_agentServicePortConfigGroup_UNDO
#define SR_agentSnmpConfigGroup_UNDO
#define SR_agentSnmpCommunityConfigEntry_UNDO
#define SR_agentSnmpTrapReceiverConfigEntry_UNDO	
#define SR_agentSnmpTrapFlagsConfigGroup_UNDO
#define SR_agentSpanningTreeConfigGroup_UNDO
#define SR_agentStpSwitchConfigGroup_UNDO
#define SR_agentStpPortEntry_UNDO
#define SR_agentStpCstConfigGroup_UNDO
#define SR_agentStpCstPortEntry_UNDO
#define SR_agentStpMstEntry_UNDO
#define SR_agentStpMstPortEntry_UNDO
#define SR_agentStpMstVlanEntry_UNDO
#define SR_agentSwitchConfigGroup_UNDO
#define SR_agentSwitchDenialOfServiceGroup_UNDO
#define SR_agentSwitchStormControlGroup_UNDO
#define SR_agentSwitchAddressAgingTimeoutEntry_UNDO
#define SR_agentSwitchStaticMacFilteringEntry_UNDO
#define SR_agentSwitchIGMPSnoopingGroup_UNDO
#define SR_agentSwitchIGMPSnoopingVlanEntry_UNDO
#define SR_agentSwitchIGMPVlanStaticMrouterEntry_UNDO
#define SR_agentTransferUploadGroup_UNDO
#define SR_agentTransferDownloadGroup_UNDO
#define SR_agentPortMirroringGroup_UNDO
#define SR_agentPortMirrorEntry_UNDO
#define SR_agentPortMirrorTypeEntry_UNDO
#define SR_agentDot3adAggPortEntry_UNDO
#define SR_agentPortConfigEntry_UNDO
#define SR_agentProtocolConfigGroup_UNDO
#define SR_agentProtocolGroupEntry_UNDO
#define SR_agentProtocolGroupPortEntry_UNDO
#define SR_agentAuthenticationGroup_UNDO
#define SR_agentAuthenticationListEntry_UNDO
#define SR_agentSystemGroup_UNDO
#define SR_agentCableTesterGroup_UNDO
#define SR_ifEntry_UNDO
#define SR_ifStackEntry_UNDO
#define SR_ifRcvAddressEntry_UNDO
#define SR_dot3adAggEntry_UNDO
#define SR_dot3adAggPortEntry_UNDO
#define SR_dot1dBasePortEntry_UNDO
#define SR_dot1dStp_UNDO
#define SR_dot1dStpPortEntry_UNDO
#define SR_dot1dTp_UNDO
#define SR_dot1dStaticEntry_UNDO
#define SR_dot1dExtBase_UNDO
#define SR_dot1dUserPriorityRegenEntry_UNDO
#define SR_dot1dTrafficClassEntry_UNDO
#define SR_dot1qBase_UNDO
#define SR_dot1qForwardAllEntry_UNDO
#define SR_dot1qForwardUnregisteredEntry_UNDO
#define SR_dot1qStaticUnicastEntry_UNDO
#define SR_dot1qStaticMulticastEntry_UNDO
#define SR_dot1qVlanStaticEntry_UNDO
#define SR_dot1qLearningConstraintsEntry_UNDO

#define SR_entPhysicalEntry_UNDO

#define SR_agentLogInMemoryConfigGroup_UNDO
#define SR_agentLogConsoleConfigGroup_UNDO
#define SR_agentLogPersistantConfigGroup_UNDO
#define SR_agentLogSysLogConfigGroup_UNDO
#define SR_agentLogSyslogHostEntry_UNDO

#ifdef __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* SWITCHING_SITEDEFS_H */
