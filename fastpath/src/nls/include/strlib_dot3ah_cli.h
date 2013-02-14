/**************************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2009
*
***************************************************************************
*
* @filename   strlib_dot3ah_cli.h
*
* @purpose    Strings Library for dot3ah component
*
* @component  Strings Library
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

#ifndef STRLIB_DOT3AH_CLI_H
#define STRLIB_DOT3AH_CLI_H

#include "datatypes.h"




extern L7_char8 *pStrInfo_dot3ah_EnableMode;
extern L7_char8 *pStrInfo_dot3ah_DisableMode;

extern L7_char8 *pStrInfo_dot3ah_EnabledMode;
extern L7_char8 *pStrInfo_dot3ah_DisabledMode;
extern L7_char8 *pStrInfo_dot3ah_Enabled;
extern L7_char8 *pStrInfo_dot3ah_Disabled;
extern L7_char8 *pStrInfo_dot3ah_ModeType;
extern L7_char8 *pStrInfo_dot3ah_LclMode;
extern L7_char8 *pStrInfo_dot3ah_CfgModeType;
extern L7_char8 *pStrInfo_dot3ah_InterfaceMode_Active;
extern L7_char8 *pStrInfo_dot3ah_InterfaceMode_Passive;
extern L7_char8 *pStrInfo_dot3ah_Active;
extern L7_char8 *pStrInfo_dot3ah_Passive;
extern L7_char8 *pStrInfo_dot3ah_InterfaceMode_Invalid;
extern L7_char8 *pStrInfo_dot3ah_LinkTimeOut;
extern L7_char8 *pStrInfo_dot3ah_LinkTimeOutSec;
extern L7_char8 *pStrInfo_dot3ah_UseLinkTimeOutSec;
extern L7_char8 *pStrInfo_dot3ah_linkTimeOutOfRange;
extern L7_char8 *pStrInfo_dot3ah_MaxPduRateOutOfRange;
extern L7_char8 *pStrInfo_dot3ah_MinPduRateOutOfRange;
extern L7_char8 *pStrInfo_dot3ah_linkTimeNo;
extern L7_char8 *pStrInfo_dot3ah_MaxPduRateNo;
extern L7_char8 *pStrInfo_dot3ah_MinPduRateNo;
extern L7_char8 *pStrErr_dot3ah_CfgureMaxPduRate;
extern L7_char8 *pStrErr_dot3ah_CfgureMinPduRate;
extern L7_char8 *pStrErr_dot3ah_CfgureLinkTime;
extern L7_char8 *pStrInfo_dot3ah_LinkMonitoring;
extern L7_char8 *pStrInfo_dot3ah_LinkMonitoringOn;
extern L7_char8 *pStrInfo_dot3ah_LinkMonitoringOff;
extern L7_char8 *pStrInfo_dot3ah_LinkMonitoringSupported;
extern L7_char8 *pStrInfo_dot3ah_ErroredFrame;
extern L7_char8 *pStrInfo_dot3ah_Threshold;
extern L7_char8 *pStrInfo_dot3ah_ThresholdHigh;
extern L7_char8 *pStrInfo_dot3ah_ThresholdLow;
extern L7_char8 *pStrInfo_dot3ah_Window;
extern L7_char8 *pStrInfo_dot3ah_FrameThresholdHigh;
extern L7_char8 *pStrInfo_dot3ah_FrameThresholdHighNone;
extern L7_char8 *pStrInfo_dot3ah_FrameThresholdHighOutOfRange;

extern L7_char8 *pStrInfo_dot3ah_FrameThresholdLow;
extern L7_char8 *pStrInfo_dot3ah_FrameThresholdLowOutOfRange;
extern L7_char8 *pStrInfo_dot3ah_FrameWindow;
extern L7_char8 *pStrInfo_dot3ah_FrameThresholdWindowOutOfRange;
extern L7_char8 *pStrInfo_dot3ah_ErroredFramePeriod;
extern L7_char8 *pStrInfo_dot3ah_FramePeriodWindow;
extern L7_char8 *pStrInfo_dot3ah_OrgSpecInfoTlvSupported;
extern L7_char8 *pStrInfo_dot3ah_ErroredFrameSumSec;
extern L7_char8 *pStrInfo_dot3ah_FrameSumSecWindow;

extern L7_char8 *pStrInfo_dot3ah_MaxPduRate;
extern L7_char8 *pStrInfo_dot3ah_MaxPduRateSec;

extern L7_char8 *pStrInfo_dot3ah_UseMaxPduRateSec;
extern L7_char8 *pStrInfo_dot3ah_UseMinPduRateSec;

extern L7_char8 *pStrInfo_dot3ah_MinPduRate;
extern L7_char8 *pStrInfo_dot3ah_MinPduRateSec;
extern L7_char8 *pStrInfo_dot3ah_RemoteLoopback;
extern L7_char8 *pStrInfo_dot3ah_RemoteLoopbackSupported;
extern L7_char8 *pStrInfo_dot3ah_RemoteLoopbackTimeout;
extern L7_char8 *pStrInfo_dot3ah_RemoteLoopbackTimeoutSec;
extern L7_char8 *pStrInfo_dot3ah_RemoteLoopbackTimeoutOutOfRange;
extern L7_char8 *pStrInfo_dot3ah_RemoteLoopbackStart;
extern L7_char8 *pStrInfo_dot3ah_RemoteLoopbackStop;
extern L7_char8 *pStrInfo_dot3ah_Interface;
extern L7_char8 *pStrInfo_dot3ah_Interface_id;
extern L7_char8 *pStrInfo_dot3ah_RemoteFailure;
extern L7_char8 *pStrInfo_dot3ah_DyingGasp;
extern L7_char8 *pStrInfo_dot3ah_LinkFault;
extern L7_char8 *pStrInfo_dot3ah_Action;
extern L7_char8 *pStrInfo_dot3ah_ErrorDisableOam;

extern L7_char8 *pStrErr_dot3ah_EnableAlready;
extern L7_char8 *pStrErr_dot3ah_DisableAlready;

extern L7_char8 *pStrInfo_dot3ah_OrgSpecEventPduSend;
extern L7_char8 *pStrInfo_dot3ah_OrgSpecInfoTlv;
extern L7_char8 *pStrInfo_dot3ah_OrgSpecEventPdu;
extern L7_char8 *pStrInfo_dot3ah_CfgOrgSpecEventPdu;

/* CFG Description*/
extern L7_char8 *pStrInfo_dot3ah_CfgEnableMode;
extern L7_char8 *pStrInfo_dot3ah_CfgDisableMode;
extern L7_char8 *pStrInfo_dot3ah_CfgOrgSpecEventPduSend;
extern L7_char8 *pStrInfo_dot3ah_CfgOrgSpecInfoTlv;
extern L7_char8 *pStrInfo_dot3ah_CfgOrgSpecInfoTlvSupported;

extern L7_char8 *pStrInfo_dot3ah_CfgInterfaceMode_Active;
extern L7_char8 *pStrInfo_dot3ah_CfgInterfaceMode_Passive;

extern L7_char8 *pStrInfo_dot3ah_CfgInterfaceMode_Invalid;
extern L7_char8 *pStrInfo_dot3ah_CfgLinkTimeOut;
extern L7_char8 *pStrInfo_dot3ah_CfgLinkTimeOutSec;

extern L7_char8 *pStrInfo_dot3ah_CfgLinkMonitoring;
extern L7_char8 *pStrInfo_dot3ah_CfgLinkMonitoringOn;
extern L7_char8 *pStrInfo_dot3ah_CfgLinkMonitoringOff;
extern L7_char8 *pStrInfo_dot3ah_CfgLinkMonitoringSupported;

extern L7_char8 *pStrInfo_dot3ah_CfgMaxPduRate;
extern L7_char8 *pStrInfo_dot3ah_CfgMaxPduRateSec;

extern L7_char8 *pStrInfo_dot3ah_CfgMinPduRate;
extern L7_char8 *pStrInfo_dot3ah_CfgMinPduRateSec;

extern L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopback;
extern L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopbackSupported;
extern L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopbackTimeout;
extern L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopbackTimeoutSec;
extern L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopbackStart;
extern L7_char8 *pStrInfo_dot3ah_CfgRemoteLoopbackStop;
extern L7_char8 *pStrInfo_dot3ah_CfgInterface;
extern L7_char8 *pStrInfo_dot3ah_CfgInterface_id;

extern L7_char8 *pStrInfo_dot3ah_CfgErroredFrame;
extern L7_char8 *pStrInfo_dot3ah_CfgThreshold;
extern L7_char8 *pStrInfo_dot3ah_CfgThresholdHigh;
extern L7_char8 *pStrInfo_dot3ah_FrameThresholdHighNone;
extern L7_char8 *pStrInfo_dot3ah_CfgThresholdLow;
extern L7_char8 *pStrInfo_dot3ah_CfgFrameWindow;
extern L7_char8 *pStrInfo_dot3ah_CfgErroredFramePeriod;
extern L7_char8 *pStrInfo_dot3ah_CfgErroredFrameSumSec;
extern L7_char8 *pStrInfo_dot3ah_CfgFrameThresholdHigh;
extern L7_char8 *pStrInfo_dot3ah_CfgFrameThresholdLow;
extern L7_char8 *pStrInfo_dot3ah_CfgFrameWindow;
extern L7_char8 *pStrInfo_dot3ah_CfgFramePeriodWindow;
extern L7_char8 *pStrInfo_dot3ah_CfgFrameSumSecWindow;

extern L7_char8 *pStrInfo_dot3ah_CfgRemoteFailure;
extern L7_char8 *pStrInfo_dot3ah_CfgDyingGasp;
extern L7_char8 *pStrInfo_dot3ah_CfgLinkFault;
extern L7_char8 *pStrInfo_dot3ah_CfgAction;
extern L7_char8 *pStrInfo_dot3ah_CfgErrorDisableOam;
extern L7_char8 *pStrInfo_dot3ah_CfgFrameThresholdHighNone;
/* SHOW CLI */
extern L7_char8 *pStrInfo_dot3ah_ShowInterfaceModeType;
extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahInterfaceModeTypeAll;

extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahStatisticsInfoAll;

extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahDiscoveryInfoAll;

extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahSummaryInfoAll;

extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahStatusInfoAll;


extern L7_char8 *pStrInfo_dot3ah_all;
extern L7_char8 *pStrInfo_dot3ah_Statistics;
extern L7_char8 *pStrInfo_dot3ah_Discovery;
extern L7_char8 *pStrInfo_dot3ah_Summary;
extern L7_char8 *pStrInfo_dot3ah_Status;

extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahIntfModeCommand;
extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahStatisticsCommand;

extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahDiscoveryCommand;
extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahSummaryCommand;

extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahStatusCommand;

extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahDiscoveryInfo;
extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahStatus;

extern L7_char8 *pStrInfo_dot3ah_DisplayDot3ahIntfModeTypeDetails;
extern L7_char8 *pStrInfo_dot3ah_DisplayDiscoveryDetails1;
extern L7_char8 *pStrInfo_dot3ah_DisplayDiscoveryDetails2;
extern L7_char8 *pStrInfo_dot3ah_DisplayDiscoveryDetails3;
extern L7_char8 *pStrInfo_dot3ah_DisplayDiscoveryDetails4;
/* Show cli Config description*/

extern L7_char8 *pStrInfo_dot3ah_Cfgall;
extern L7_char8 *pStrInfo_dot3ah_CfgStatistics;
extern L7_char8 *pStrInfo_dot3ah_CfgDiscovery;
extern L7_char8 *pStrInfo_dot3ah_CfgSummary;
extern L7_char8 *pStrInfo_dot3ah_CfgStatus;

extern L7_char8 *pStrInfo_dot3ah_IntfIsNotEnbld;
extern L7_char8 *pStrInfo_dot3ah_pduInterval;
extern L7_char8 *pStrInfo_dot3ah_linkLostInterval;
extern L7_char8 *pStrInfo_dot3ah_remLbExpiryInterval;
extern L7_char8 *pStrInfo_dot3ah_NoLocalIntfsAreEnbldToOAM;
extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahInterfaceInfo;
extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahLinkMonitorInfo;
extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahInterfaceCommand;
extern L7_char8 *pStrInfo_dot3ah_ShowDot3ahLinkMonitorCommand;
extern L7_char8 *pStrInfo_dot3ah_InterfaceLinkMonitorInfo;
extern L7_char8 *pStrInfo_dot3ah_lclLinkMonitorCap; 
extern L7_char8 *pStrInfo_dot3ah_lclLinkMonitor; 
extern L7_char8 *pStrInfo_dot3ah_lclUniDirCap; 
extern L7_char8 *pStrInfo_dot3ah_lclRemLbCap; 
extern L7_char8 *pStrInfo_dot3ah_lclOrgSpcInfoTlvCap;
extern L7_char8 *pStrInfo_dot3ah_lclOrgSpcEvtTlvCap; 
extern L7_char8 *pStrInfo_dot3ah_lclOrgSpcPduCap;
extern L7_char8 *pStrInfo_dot3ah_totLinkFault;
extern L7_char8 *pStrInfo_dot3ah_totDyingGasp; 
extern L7_char8 *pStrInfo_dot3ah_lastLinkFault; 
extern L7_char8 *pStrInfo_dot3ah_lastDyingGasp;
extern L7_char8 *pStrInfo_dot3ah_VerifyClrDot3ahPortStats;
extern L7_char8 *pStrInfo_ClrDot3ahStats;
extern L7_char8 *pStrInfo_dot3ah_ClrDot3ahAll;
extern L7_char8 *pStrInfo_dot3ah_ClrDot3ahStats;
extern L7_char8 *pStrInfo_dot3ah_Dot3ahPortStatsNotClred;
extern L7_char8 *pStrInfo_dot3ah_ClrDot3ahPortStats;


/* Error info */
extern L7_char8 *pStrErr_dot3ah_InterfaceModeType;
extern L7_char8 *pStrErr_dot3ah_Disable;
extern L7_char8 *pStrErr_dot3ah_Enable;
extern L7_char8 *pStrErr_dot3ah_UnableToClrAllDot3ahportStats;
                   

#endif
