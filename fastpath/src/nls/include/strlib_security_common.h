/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   src/nls/include/strlib_security_common.h
*
* @purpose    Strings Library
*
* @component  Common Strings Library
*
* @comments   none
*
* @create     01/10/2007
*
* @author     Rama Sasthri, Kristipati
*
* @end
*
*********************************************************************/

#ifndef STRLIB_SECURITY_COMMON_C
#define STRLIB_SECURITY_COMMON_C

#include "datatypes.h"
extern L7_char8 *pStrInfo_security_Aborting;
extern L7_char8 *pStrInfo_security_AccessRejects;
extern L7_char8 *pStrInfo_security_AccessReqs;
extern L7_char8 *pStrInfo_security_Authenticating;
extern L7_char8 *pStrInfo_security_Authenticator;
extern L7_char8 *pStrInfo_security_AuthenticatorPaeState;
extern L7_char8 *pStrInfo_security_Authorized;
extern L7_char8 *pStrInfo_security_BadAuthenticators;
extern L7_char8 *pStrInfo_security_Connecting;
extern L7_char8 *pStrInfo_security_CntrlDirection;
extern L7_char8 *pStrInfo_security_CurrentSrvrHostAddr;
extern L7_char8 *pStrInfo_security_Disconnected;
extern L7_char8 *pStrInfo_security_EapReqFramesTxed;
extern L7_char8 *pStrInfo_security_EapReqFramesRcvd;
extern L7_char8 *pStrInfo_security_EapRespFramesRcvd;
extern L7_char8 *pStrInfo_security_EapRespFramesTxed;
extern L7_char8 *pStrInfo_security_EapolFramesRcvd;
extern L7_char8 *pStrErr_security_EapolLenFramesRcvd;
extern L7_char8 *pStrInfo_security_EapolLogoffFramesRcvd;
extern L7_char8 *pStrInfo_security_EapolLogoffFramesTxed;
extern L7_char8 *pStrInfo_security_ForceAuthorized;
extern L7_char8 *pStrInfo_security_ForceUnAuthorized;
extern L7_char8 *pStrInfo_security_Held;
extern L7_char8 *pStrErr_security_EapolFramesRcvd_1;
extern L7_char8 *pStrErr_security_SrvrAddres;
extern L7_char8 *pStrInfo_security_LastEapolFrameSrc;
extern L7_char8 *pStrInfo_security_LastEapolFrameVer;
extern L7_char8 *pStrInfo_security_MalformedAccessResps;
extern L7_char8 *pStrInfo_security_MaxReqs;
extern L7_char8 *pStrInfo_security_NumOfCfguredSrvrs;
extern L7_char8 *pStrInfo_security_MaxStarts;
extern L7_char8 *pStrInfo_security_PaeCapabilities;
extern L7_char8 *pStrInfo_security_ReauthenticationEnbld;
extern L7_char8 *pStrInfo_security_Req;
extern L7_char8 *pStrInfo_security_Resp;
extern L7_char8 *pStrInfo_security_SecretCfgured;
extern L7_char8 *pStrInfo_security_SrvrTimeoutSecs;
extern L7_char8 *pStrInfo_security_Timeouts;
extern L7_char8 *pStrInfo_security_UnknownTypes;
extern L7_char8 *pStrInfo_security_ForceAuthorized_1;
extern L7_char8 *pStrInfo_security_ForceUnAuthorized_1;
extern L7_char8 *pStrInfo_security_MacBased_1;
extern L7_char8 *pStrInfo_security_Scp_1;
extern L7_char8 *pStrInfo_security_Scp_2;
extern L7_char8 *pStrInfo_security_Sftp_1;
extern L7_char8 *pStrInfo_security_Sftp_2;
extern L7_char8 *pStrInfo_security_AddServer;
extern L7_char8 *pStrInfo_security_GuestVlanId;
extern L7_char8 *pStrInfo_security_UnauthenticatedVlanId;
extern L7_char8 *pStrInfo_security_VlanAssigned;
extern L7_char8 *pStrInfo_security_VlanAssignedReason;
extern L7_char8 *pStrInfo_security_SessionTimeout;
extern L7_char8 *pStrInfo_security_SessionTerminateAction;
extern L7_char8 *pStrInfo_security_Dot1xShowClientInterface;
extern L7_char8 *pStrInfo_security_Dot1xLogicalInterface;
extern L7_char8 *pStrInfo_security_Dot1xShowClientUserName;
extern L7_char8 *pStrInfo_security_Dot1xShowClientMacAddr; 
extern L7_char8 *pStrInfo_security_Dot1xShowClientSessionTime;
extern L7_char8 *pStrInfo_security_Dot1xShowClientFilterId;
extern L7_char8 *pStrInfo_security_Dot1xShowClientVland;
extern L7_char8 *pStrInfo_security_Dot1xShowClientVlanMode;
extern L7_char8 *pStrInfo_security_Dot1xVlanAssigned_Radius;
extern L7_char8 *pStrInfo_security_Dot1xVlanAssigned_unauth;
extern L7_char8 *pStrInfo_security_Dot1xVlanAssigned_Default;
extern L7_char8 *pStrInfo_security_Dot1xVlanAssigned_Guest;
extern L7_char8 *pStrInfo_security_Dot1xVlanAssigned_NotAssigned;
extern L7_char8 *pStrInfo_security_Dot1xVlanAssigned_Voice;
extern L7_char8 *pStrInfo_security_Dot1xVlanAssigned_Monitor;
extern L7_char8 *pStrInfo_security_Dot1xSessionTerminationAction_Radius;
extern L7_char8 *pStrInfo_security_KeyTransmissionEnbld;
extern L7_char8 *pStrInfo_security_CntrlMode;
extern L7_char8 *pStrInfo_security_QuietPeriodSecs;
extern L7_char8 *pStrInfo_security_TxPeriodSecs;
extern L7_char8 *pStrInfo_security_GuestVlanPeriod;
extern L7_char8 *pStrInfo_security_SupplicantTimeoutSecs;
extern L7_char8 *pStrInfo_security_ReauthenticationPeriodSecs;
extern L7_char8 *pStrInfo_security_Max_Users;
extern L7_char8 *pStrInfo_security_MsgAuthenticator; 
extern L7_char8 *pStrInfo_security_NumAuthServers;
extern L7_char8 *pStrInfo_security_NumAcctServers;
extern L7_char8 *pStrInfo_security_NumAuthServerGrps;
extern L7_char8 *pStrInfo_security_NumAcctServerGrps;
extern L7_char8 *pStrInfo_security_Empty;
extern L7_char8 *pStrErr_security_AcctSrvrHostAddr_1;
extern L7_char8 *pStrInfo_security_RadSrvrIPAddr;
extern L7_char8 *pStrInfo_security_RadSrvrDNSAddr;
extern L7_char8 *pStrInfo_security_CurrentSrvrIPAddr;
extern L7_char8 *pStrInfo_security_CurrentSrvrDNSAddr;
extern L7_char8 *pStrInfo_security_AcctSrvrDNSAddr;
extern L7_char8 *pStrInfo_security_AcctSrvrIPAddr;
extern L7_char8 *pStrInfo_security_DNSAddrNotResolved;
extern L7_char8 *pStrInfo_security_Dot1xVlanAssigned_MonitorMode;
extern L7_char8 *pStrInfo_security_Dot1xShowNumMonitorModeClients;
extern L7_char8 *pStrInfo_security_Dot1xShowNumClients;
/* Supplicant Strings */
extern L7_char8 *pStrInfo_security_SupplicantPaeState;
extern L7_char8 *pStrInfo_security_StartPeriodSecs;
extern L7_char8 *pStrInfo_security_HeldPeriodSecs;
extern L7_char8 *pStrInfo_security_AuthPeriodSecs;






#endif

