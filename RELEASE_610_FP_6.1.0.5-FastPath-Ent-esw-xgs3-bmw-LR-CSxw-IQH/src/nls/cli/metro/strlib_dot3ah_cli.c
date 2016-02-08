/**************************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2009
*
***************************************************************************
*
* @filename   strlib_dot3ah_cli.c
*
* @purpose    Strings Library for Dot1ad Component.
*
* @component  Common Strings Library
*
* @comments   none
*
* @create     14/05/2008
*
* @author     Sengottuvelan Srirangan
*
* @end
*
**************************************************************************/

#include "strlib_dot3ah_cli.h"

/* CLI*/
L7_char8 *pStrInfo_dot3ah_EnableMode="enable";
L7_char8 *pStrInfo_dot3ah_DisableMode="disable";

L7_char8 *pStrInfo_dot3ah_EnabledMode="enabled";
L7_char8 *pStrInfo_dot3ah_DisabledMode="disabled";

L7_char8 *pStrInfo_dot3ah_Enabled="Enabled";
L7_char8 *pStrInfo_dot3ah_Disabled="Disabled";

L7_char8 *pStrInfo_dot3ah_ModeType = "mode";
L7_char8 *pStrInfo_dot3ah_CfgModeType = "Configure the Dot3ah interface mode type.";
L7_char8 *pStrInfo_dot3ah_InterfaceMode_Active = "active";
L7_char8 *pStrInfo_dot3ah_InterfaceMode_Passive = "passive";
L7_char8 *pStrInfo_dot3ah_Active = "Active";
L7_char8 *pStrInfo_dot3ah_Passive = "Passive";
L7_char8 *pStrInfo_dot3ah_LclMode = "Local OAM Mode";
L7_char8 *pStrInfo_dot3ah_InterfaceMode_Invalid = "invalid";
L7_char8 *pStrInfo_dot3ah_LinkTimeOut = "peer-timeout";
L7_char8 *pStrInfo_dot3ah_LinkTimeOutSec ="<2-30>";
L7_char8 *pStrInfo_dot3ah_UseLinkTimeOutSec="Use 'ethernet oam client-time-out <2-30>'.";

L7_char8 *pStrInfo_dot3ah_linkTimeOutOfRange="Valid link time lost range is <%d-%d>.";
L7_char8 *pStrInfo_dot3ah_MaxPduRateOutOfRange="Valid max pdu rate range is <%d-%d>.";
L7_char8 *pStrInfo_dot3ah_MinPduRateOutOfRange="Valid min pdu rate range is <%d-%d>.";

L7_char8 *pStrInfo_dot3ah_linkTimeNo= "Use 'no ethernet oam client-time-out'.";
L7_char8 *pStrInfo_dot3ah_MaxPduRateNo="Use 'no ethernet oam max-pdu-rate .";
L7_char8 *pStrInfo_dot3ah_MinPduRateNo="Use 'no ethernet oam min-pdu-rate .";
L7_char8 *pStrErr_dot3ah_CfgureMaxPduRate = "configure maximum pdu rate.";
L7_char8 *pStrErr_dot3ah_CfgureMinPduRate = "configure minimum pdu rate.";
L7_char8 *pStrErr_dot3ah_CfgureLinkTime = "configure Dot3ah Link lost timer.";

L7_char8 *pStrInfo_dot3ah_LinkMonitoring ="link-monitor";
L7_char8 *pStrInfo_dot3ah_LinkMonitoringOn ="on";
L7_char8 *pStrInfo_dot3ah_LinkMonitoringOff ="off";
L7_char8 *pStrInfo_dot3ah_LinkMonitoringSupported ="supported";

L7_char8 *pStrInfo_dot3ah_ErroredFrame="frame";
L7_char8 *pStrInfo_dot3ah_Threshold="threshold";
L7_char8 *pStrInfo_dot3ah_ThresholdHigh="high";
L7_char8 *pStrInfo_dot3ah_ThresholdLow="low";
L7_char8 *pStrInfo_dot3ah_Window="window";
L7_char8 *pStrInfo_dot3ah_FrameThresholdHigh="<1-65535>";
L7_char8 *pStrInfo_dot3ah_FrameThresholdHighNone="none";
L7_char8 *pStrInfo_dot3ah_FrameThresholdHighOutOfRange="Valid Threshold high value <%d-%d> or none";
 
L7_char8 *pStrInfo_dot3ah_FrameThresholdLow="<1-65535>";
L7_char8 *pStrInfo_dot3ah_FrameThresholdLowOutOfRange="Valid Threshold low value <%d-%d>.";
L7_char8 *pStrInfo_dot3ah_FrameWindow="<1-60>";
L7_char8 *pStrInfo_dot3ah_FrameThresholdWindowOutOfRange="Valid Threshold Window value <%d-%d>.";


L7_char8 *pStrInfo_dot3ah_ErroredFramePeriod="frame-period";
L7_char8 *pStrInfo_dot3ah_FramePeriodWindow="<1-60>";

L7_char8 *pStrInfo_dot3ah_ErroredFrameSumSec="frame-seconds";
L7_char8 *pStrInfo_dot3ah_FrameSumSecWindow="<10-900>";

L7_char8 *pStrInfo_dot3ah_MaxPduRate = "max-pdu-rate";
L7_char8 *pStrInfo_dot3ah_MaxPduRateSec ="<1-10>";

L7_char8 *pStrInfo_dot3ah_UseMaxPduRateSec="Use 'ethernet oam max-pdu-rate <1-10>'.";
L7_char8 *pStrInfo_dot3ah_UseMinPduRateSec="Use 'ethernet oam min-pdu-rate <1-10>'.";

L7_char8 *pStrInfo_dot3ah_MinPduRate = "min-pdu-rate";
L7_char8 *pStrInfo_dot3ah_MinPduRateSec ="<1-10>";
L7_char8 *pStrInfo_dot3ah_OrgSpecInfoTlv ="org-spec-tlv";
L7_char8 *pStrInfo_dot3ah_OrgSpecEventPdu ="org-spec-event-pdu";
L7_char8 *pStrInfo_dot3ah_OrgSpecEventPduSend ="send";
L7_char8 *pStrInfo_dot3ah_OrgSpecInfoTlvSupported="supported";
L7_char8 *pStrInfo_dot3ah_RemoteLoopback="remote-loopback";
L7_char8 *pStrInfo_dot3ah_RemoteLoopbackSupported="supported";
L7_char8 *pStrInfo_dot3ah_RemoteLoopbackTimeout="time-out";
L7_char8 *pStrInfo_dot3ah_RemoteLoopbackTimeoutSec="<10-100>";
L7_char8 *pStrInfo_dot3ah_RemoteLoopbackTimeoutOutOfRange="Valid link time lost range is <%d-%d>.";
L7_char8 *pStrInfo_dot3ah_RemoteLoopbackStart="start";
L7_char8 *pStrInfo_dot3ah_RemoteLoopbackStop="stop";
L7_char8 *pStrInfo_dot3ah_Interface="interface";
L7_char8 *pStrInfo_dot3ah_Interface_id=" <unit/slot/port> ";
L7_char8 *pStrInfo_dot3ah_RemoteFailure="remote-failure";
L7_char8 *pStrInfo_dot3ah_DyingGasp="dying-gasp";
L7_char8 *pStrInfo_dot3ah_LinkFault="link-fault";
L7_char8 *pStrInfo_dot3ah_Action="action";
L7_char8 *pStrInfo_dot3ah_ErrorDisableOam= "error-disable-oam";


/* CFG Description*/
L7_char8 *pStrInfo_dot3ah_CfgEnableMode="Enable Ethernet OAM (Dot3ah) on this interface.";
L7_char8 *pStrInfo_dot3ah_CfgDisableMode="Disable Ethernet OAM (Dot3ah) on this interface.";

L7_char8 *pStrInfo_dot3ah_CfgInterfaceMode_Active =
                "Configure the Dot3ah interface mode type as 'active'.";
L7_char8 *pStrInfo_dot3ah_CfgInterfaceMode_Passive =
                "Configure the Dot3ah interface mode type as 'passive'.";

L7_char8 *pStrInfo_dot3ah_CfgOrgSpecInfoTlv = "Configure Dot3ah Org Specific Information TLV support to be included.";

L7_char8 *pStrInfo_dot3ah_CfgOrgSpecInfoTlvSupported ="Configure Dot3ah Org Specific Event Pdu to be send"; 
L7_char8 *pStrInfo_dot3ah_CfgOrgSpecEventPdu ="Execute to send Org Specific Event Pdu to Remote Entity";
L7_char8 *pStrInfo_dot3ah_CfgOrgSpecEventPduSend ="Enter to send the pdu";

L7_char8 *pStrInfo_dot3ah_CfgInterfaceMode_Invalid =
                "Configure the Dot3ah interface mode type as 'Invalid'.";
L7_char8 *pStrInfo_dot3ah_CfgLinkTimeOut = "Configure the Dot3ah Remote entity discovery time-out";
L7_char8 *pStrInfo_dot3ah_CfgLinkTimeOutSec ="Enter timeout period <2-30> secs. ";

L7_char8 *pStrInfo_dot3ah_CfgLinkMonitoring ="Configure Dot3ah link-monitoring params.";
L7_char8 *pStrInfo_dot3ah_CfgLinkMonitoringOn ="Start the Ethernet OAM (Dot3ah) link monitoring .";
L7_char8 *pStrInfo_dot3ah_CfgLinkMonitoringOff ="Stop the Ethernet OAM (Dot3ah) link monitoring off.";
L7_char8 *pStrInfo_dot3ah_CfgLinkMonitoringSupported ="Enter to support link monitoring on this interface .";

L7_char8 *pStrInfo_dot3ah_CfgMaxPduRate = "Configure the maximum pdu rate per secs .";
L7_char8 *pStrInfo_dot3ah_CfgMaxPduRateSec ="Enter max pdu rate <1-10> per secs";

L7_char8 *pStrInfo_dot3ah_CfgMinPduRate = "Configure the minimum pdu rate in secs.";
L7_char8 *pStrInfo_dot3ah_CfgMinPduRateSec ="Enter min pdu rate <1-10> in secs";

L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopback="Configure Remote Loopback params.";
L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopbackSupported="Configure to local entity to respond Remote-Loopback OAMPDU. ";
L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopbackTimeout="Enter Remote-Loopback time-out. ";
L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopbackTimeoutSec="Enter Remote-Loopback time-out period <10-100> Sec. ";
L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopbackStart="Enter to start Remote-Loopback on Ethernet OAM interface.";
L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopbackStop="Enter to stop Remote-Loopback on Ethernet OAM interface.";
L7_char8 *pStrInfo_dot3ah_CfgInterface="Enter Dot3ah Interface";
L7_char8 *pStrInfo_dot3ah_CfgInterface_id="Enter <slot/port>";

L7_char8 *pStrInfo_dot3ah_CfgErroredFrame="Configure Errored Frame Event properties.";
L7_char8 *pStrInfo_dot3ah_CfgThreshold="Enter threshold value for high and low .";
L7_char8 *pStrInfo_dot3ah_CfgThresholdHigh="Enter higher Errored Frame/Frame-Period/Frame-Sum-Seconds Event value.";
L7_char8 *pStrInfo_dot3ah_CfgFrameThresholdHighNone="Enter higher errored Frame threshold as 'none'.";
L7_char8 *pStrInfo_dot3ah_CfgThresholdLow="Enter lower Errored Frame/Frame-Period/Frame-Sum-Seconds Event value.";
L7_char8 *pStrInfo_dot3ah_CfgErroredFramePeriod="Configure Errored Frame-Period Event properties. ";
L7_char8 *pStrInfo_dot3ah_CfgErroredFrameSumSec="Configure Errored Frame-Summary Seconds Event properties. ";
L7_char8 *pStrInfo_dot3ah_CfgFrameThresholdHigh="Enter Errored frame high threshold value <1-65535>.";
L7_char8 *pStrInfo_dot3ah_CfgFrameThresholdLow="Enter Errored frame low threshold value <0-65535>";
L7_char8 *pStrInfo_dot3ah_CfgFrameWindow="Enter Errored frame window value <1-60> in secs.";
L7_char8 *pStrInfo_dot3ah_CfgFramePeriodWindow="Enter Errored frame-period window value <1-60> in secs.";
L7_char8 *pStrInfo_dot3ah_CfgFrameSumSecWindow="Enter Errored frame-summary-seconds window value <10-900> in secs.";

L7_char8 *pStrInfo_dot3ah_CfgRemoteFailure="Enter remote-failure action";
L7_char8 *pStrInfo_dot3ah_CfgDyingGasp="Configure dying-gasp properties.";
L7_char8 *pStrInfo_dot3ah_CfgLinkFault="Configure link-fault properties when remote-entity is timed-out";
L7_char8 *pStrInfo_dot3ah_CfgAction="Enter Action";
L7_char8 *pStrInfo_dot3ah_CfgErrorDisableOam= "Configure Error-disable-oam when Received Critical events.";
L7_char8 *pStrInfo_ClrDot3ahStats ="Clear the Ethernet OAM (Dot3ah) statistics.";
L7_char8 *pStrInfo_dot3ah_VerifyClrDot3ahPortStats = "\r\nAre you sure you want to clear dot3ah port stats? (y/n)";


/* SHOW CLI */
L7_char8 *pStrInfo_dot3ah_ShowInterfaceModeType =
                 "Display the Ethernet OAM (Dot3ah) Mode Type for the interface.";
L7_char8 *pStrInfo_dot3ah_ShowDot3ahInterfaceModeTypeAll =
                 "Display the Dot3ah Mode Type for all interfaces.";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahStatisticsInfoAll =
                  "Display the Dot3ah Statistics Information for all interfaces.";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahDiscoveryInfoAll =
                  "Display the Dot3ah Entity Discovery Information for all interfaces.";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahSummaryInfoAll =
                  "Display the Dot3ah Summary Information for all interfaces.";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahStatusInfoAll =
                  "Display the Dot3ah Status Information for all interfaces.";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahInterfaceInfo=
                  "Display the Dot3ah Interface Information for an interface.";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahLinkMonitorInfo=
                  "Display the Dot3ah Link Monitor Information for an interface.";

L7_char8 *pStrInfo_dot3ah_all="all";
L7_char8 *pStrInfo_dot3ah_Statistics="statistics";
L7_char8 *pStrInfo_dot3ah_Discovery="discovery";
L7_char8 *pStrInfo_dot3ah_Summary="summary";
L7_char8 *pStrInfo_dot3ah_Status="status";
L7_char8 *pStrInfo_dot3ah_ClrDot3ahAll = "Enter 'all' to specify all ports.";
L7_char8 *pStrInfo_dot3ah_ClrDot3ahStats = "clear ethernet oam statistics %s";
L7_char8 *pStrInfo_dot3ah_ClrDot3ahPortStats = "Use 'clear ethernet oam statistics <%s | all>'.";
L7_char8 *pStrInfo_dot3ah_Dot3ahPortStatsNotClred = "Dot3ah port statistics not cleared.";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahIntfModeCommand =
                  "Use 'show ethernet oam intf-mode {all | <unit/slot/port>}'";
L7_char8 *pStrInfo_dot3ah_ShowDot3ahStatisticsCommand =
                 "Use 'show ethernet oam statistics {all | interface <unit/slot/port>}'";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahDiscoveryCommand =
                 "Use 'show ethernet oam discovery {all | interface <unit/slot/port>}'";
L7_char8 *pStrInfo_dot3ah_ShowDot3ahSummaryCommand =
                 "Use 'show ethernet oam summary {all | interface <unit/slot/port>}'";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahStatusCommand =
                 "Use 'show ethernet oam status {all | interface <unit/slot/port>}'";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahInterfaceCommand =
                 "Use 'show ethernet oam interface <slot/port>'";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahLinkMonitorCommand =
                 "Use 'show ethernet oam link-monitor <slot/port>'";

L7_char8 *pStrInfo_dot3ah_ShowDot3ahDiscoveryInfo =
                  "Display the Dot3ah Discovery Information of the interface.";
L7_char8 *pStrInfo_dot3ah_ShowDot3ahStatus =
                  "Display the Dot3ah protocol status Information for valid interfaces.";

L7_char8 *pStrInfo_dot3ah_DisplayDot3ahIntfModeTypeDetails =
                  "Interface    EFM-OAM             ModeType";

L7_char8 *pStrInfo_dot3ah_DisplayDiscoveryDetails1 =
		  "Interface  Rem-MAC-address    Rem-OUI    Rem-Mode   Rem-State   Rem-VSI";
L7_char8 *pStrInfo_dot3ah_DisplayDiscoveryDetails2 =
		  "Interface  Loc-Mode  Loc-State  Disc-State  Rem-Mode   Rem-State  Link-Status";
L7_char8 *pStrInfo_dot3ah_DisplayDiscoveryDetails3 =
                 "Interface    Discovery-State  Remote-Loop-State "; 
L7_char8 *pStrInfo_dot3ah_DisplayDiscoveryDetails4 =
                 "Interface     Local-MAC-address          Mode/Type      Current-State ";
/* Show cli Config description*/

L7_char8 *pStrInfo_dot3ah_Cfgall="Enter for all available interfaces.";
L7_char8 *pStrInfo_dot3ah_CfgStatistics="Display ethernet oam (Dot3ah) statistics information.";
L7_char8 *pStrInfo_dot3ah_CfgDiscovery="Display ethernet oam (Dot3ah) discovery information.";
L7_char8 *pStrInfo_dot3ah_CfgSummary="Display ethernet oam (Dot3ah) summary of the protocol information.";
L7_char8 *pStrInfo_dot3ah_CfgStatus="Display ethernet oam (Dot3ah) status of the protocol on an interface.";

L7_char8 *pStrInfo_dot3ah_IntfIsNotEnbld="The interface is not enabled for Ethernet OAM (Dot3ah) protocol.\r\n";
L7_char8 *pStrInfo_dot3ah_pduInterval = "Pdu Transmit Interval.";
L7_char8 *pStrInfo_dot3ah_linkLostInterval = "Link Lost Interval.";
L7_char8 *pStrInfo_dot3ah_remLbExpiryInterval = "Remote Loopback expiry Interval.";
L7_char8 *pStrInfo_dot3ah_NoLocalIntfsAreEnbldToOAM = "No local interfaces are enabled for OAM protocol.";

L7_char8 *pStrInfo_dot3ah_InterfaceLinkMonitorInfo = "Dot3ah Link Monitoring Information Details";
L7_char8 *pStrInfo_dot3ah_lclLinkMonitorCap = "Local OAM Link Monitoring Capability";
L7_char8 *pStrInfo_dot3ah_lclLinkMonitor = "Local OAM Link Monitoring";
L7_char8 *pStrInfo_dot3ah_lclUniDirCap = "Local OAM Unidirectional Capability";
L7_char8 *pStrInfo_dot3ah_lclRemLbCap = "Local OAM Remote Loopback Capability";
L7_char8 *pStrInfo_dot3ah_lclOrgSpcInfoTlvCap="Local OAM Org Spec Info Tlv Capability";
L7_char8 *pStrInfo_dot3ah_lclOrgSpcEvtTlvCap ="Local OAM Org Spec Event Tlv Capability";
L7_char8 *pStrInfo_dot3ah_lclOrgSpcPduCap = "Local OAM Org Spec Pdu Capability";

L7_char8 *pStrInfo_dot3ah_totLinkFault = "Total Link Fault";
L7_char8 *pStrInfo_dot3ah_totDyingGasp = "Total Dying Gasp";
L7_char8 *pStrInfo_dot3ah_lastLinkFault = "Last Link Fault Timestamp";
L7_char8 *pStrInfo_dot3ah_lastDyingGasp = "Last Dying Gasp Timestamp";

/* Error info */
L7_char8 *pStrErr_dot3ah_InterfaceModeType="Unable to set OAM Mode. Enable OAM or Already this mode is set on this interface";
L7_char8 *pStrErr_dot3ah_Disable="Unable to disable Ethernet OAM on this interface.";
L7_char8 *pStrErr_dot3ah_Enable="Unable to Enable Ethernet OAM on this interface.";
L7_char8 *pStrErr_dot3ah_EnableAlready="Already Ethernet OAM is enabled on this interface.";
L7_char8 *pStrErr_dot3ah_DisableAlready="Already Ethernet OAM is disabled on this interface.";
L7_char8 *pStrErr_dot3ah_UnableToClrAllDot3ahportStats = "Unable to clear Dot3ah port statistics.";
