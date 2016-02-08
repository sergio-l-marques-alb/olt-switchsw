/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   src/nls/common/security/strlib_security_common.c
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

#include "strlib_security_common.h"

L7_char8 *pStrInfo_security_Aborting = "Aborting";
L7_char8 *pStrInfo_security_AccessRejects = "Access Rejects";
L7_char8 *pStrInfo_security_AccessReqs = "Access Requests";
L7_char8 *pStrInfo_security_Authenticating = "Authenticating";
L7_char8 *pStrInfo_security_Authenticator = "Authenticator";
L7_char8 *pStrInfo_security_AuthenticatorPaeState = "Authenticator PAE State";
L7_char8 *pStrInfo_security_Authorized = "Authorized";
L7_char8 *pStrInfo_security_BadAuthenticators = "Bad Authenticators";
L7_char8 *pStrInfo_security_Connecting = "Connecting";
L7_char8 *pStrInfo_security_CntrlDirection = "Control Direction";
L7_char8 *pStrInfo_security_CurrentSrvrHostAddr = "Current Server Host Address";
L7_char8 *pStrInfo_security_Disconnected = "Disconnected";
L7_char8 *pStrInfo_security_EapReqFramesTxed = "EAP Request Frames Transmitted";
L7_char8 *pStrInfo_security_EapReqFramesRcvd = "EAP Request Frames Received";
L7_char8 *pStrInfo_security_EapRespFramesRcvd = "EAP Response Frames Received";
L7_char8 *pStrInfo_security_EapRespFramesTxed = "EAP Response Frames Transmitted";
L7_char8 *pStrInfo_security_EapolFramesRcvd = "EAPOL Frames Received";
L7_char8 *pStrErr_security_EapolLenFramesRcvd = "EAPOL Length Error Frames Received";
L7_char8 *pStrInfo_security_EapolLogoffFramesRcvd = "EAPOL Logoff Frames Received";
L7_char8 *pStrInfo_security_EapolLogoffFramesTxed = "EAPOL Logoff Frames Transmitted";
L7_char8 *pStrInfo_security_ForceAuthorized = "Force Authorized";
L7_char8 *pStrInfo_security_ForceUnAuthorized = "Force Unauthorized";
L7_char8 *pStrInfo_security_Held = "Held";
L7_char8 *pStrErr_security_EapolFramesRcvd_1 = "Invalid EAPOL Frames Received";
L7_char8 *pStrErr_security_SrvrAddres = "Invalid Server Addresses";
L7_char8 *pStrInfo_security_LastEapolFrameSrc = "Last EAPOL Frame Source";
L7_char8 *pStrInfo_security_LastEapolFrameVer = "Last EAPOL Frame Version";
L7_char8 *pStrInfo_security_MalformedAccessResps = "Malformed Access Responses";
L7_char8 *pStrInfo_security_MaxReqs = "Maximum Requests";
L7_char8 *pStrInfo_security_MaxStarts = "Maximum Start Messages";
L7_char8 *pStrInfo_security_NumOfCfguredSrvrs = "Number of Configured Servers";
L7_char8 *pStrInfo_security_PaeCapabilities = "PAE Capabilities";
L7_char8 *pStrInfo_security_ReauthenticationEnbld = "Reauthentication Enabled";
L7_char8 *pStrInfo_security_Req = "Request";
L7_char8 *pStrInfo_security_Resp = "Response";
L7_char8 *pStrInfo_security_SecretCfgured = "Secret Configured";
L7_char8 *pStrInfo_security_SrvrTimeoutSecs = "Server Timeout (secs)";
L7_char8 *pStrInfo_security_Timeouts = "Timeouts";
L7_char8 *pStrInfo_security_UnknownTypes = "Unknown Types";
L7_char8 *pStrInfo_security_ForceAuthorized_1 = "forceauthorized";
L7_char8 *pStrInfo_security_ForceUnAuthorized_1 = "forceunauthorized";
L7_char8 *pStrInfo_security_MacBased_1 = "macbased";
L7_char8 *pStrInfo_security_Scp_1 = "scp";
L7_char8 *pStrInfo_security_Scp_2 = "SCP";
L7_char8 *pStrInfo_security_Sftp_1 = "sftp";
L7_char8 *pStrInfo_security_Sftp_2 = "SFTP";
L7_char8 *pStrInfo_security_AddServer = "Add";
L7_char8 *pStrInfo_security_GuestVlanId = "Guest VLAN ID";
L7_char8 *pStrInfo_security_UnauthenticatedVlanId = "Unauthenticated VLAN ID";
L7_char8 *pStrInfo_security_VlanAssigned = "VLAN Assigned";
L7_char8 *pStrInfo_security_VlanAssignedReason = "VLAN Assigned Reason";
L7_char8 *pStrInfo_security_SessionTimeout = "Session Timeout";
L7_char8 *pStrInfo_security_SessionTerminateAction = "Session Termination Action";
L7_char8 *pStrInfo_security_Dot1xShowClientInterface = "Interface";
L7_char8 *pStrInfo_security_Dot1xShowClientUserName = "User Name";
L7_char8 *pStrInfo_security_Dot1xShowClientMacAddr  = "Supp MAC Address"; 
L7_char8 *pStrInfo_security_Dot1xShowClientSessionTime = "Session Time";
L7_char8 *pStrInfo_security_Dot1xShowClientFilterId = "Filter Id";
L7_char8 *pStrInfo_security_Dot1xShowClientVland = "VLAN Id";
L7_char8 *pStrInfo_security_Dot1xShowClientVlanMode= "VLAN Assigned";
L7_char8 *pStrInfo_security_Dot1xVlanAssigned_Radius = "RADIUS";
L7_char8 *pStrInfo_security_Dot1xVlanAssigned_unauth = "Unauth";
L7_char8 *pStrInfo_security_Dot1xVlanAssigned_Default= "Default";
L7_char8 *pStrInfo_security_Dot1xVlanAssigned_Guest= "Guest";
L7_char8 *pStrInfo_security_Dot1xVlanAssigned_Voice= "Voice";
L7_char8 *pStrInfo_security_Dot1xVlanAssigned_NotAssigned= "Not Assigned";
L7_char8 *pStrInfo_security_Dot1xSessionTerminationAction_Radius= "Reauthenticate";
L7_char8 *pStrInfo_security_Dot1xLogicalInterface = "Logical Interface";
L7_char8 *pStrInfo_security_KeyTransmissionEnbld = "Key Transmission Enabled";
L7_char8 *pStrInfo_security_CntrlMode = "Control Mode";
L7_char8 *pStrInfo_security_QuietPeriodSecs = "Quiet Period (secs)";
L7_char8 *pStrInfo_security_TxPeriodSecs = "Transmit Period (secs)";
L7_char8 *pStrInfo_security_GuestVlanPeriod = "Guest VLAN Period (secs)";
L7_char8 *pStrInfo_security_SupplicantTimeoutSecs = "Supplicant Timeout (secs)";
L7_char8 *pStrInfo_security_ReauthenticationPeriodSecs = "Reauthentication Period (secs)";
L7_char8 *pStrInfo_security_Max_Users = "Maximum Users";
L7_char8 *pStrInfo_security_MsgAuthenticator = "Message Authenticator";
L7_char8 *pStrInfo_security_NumAuthServers = "Number of Configured Authentication Servers";
L7_char8 *pStrInfo_security_NumAcctServers = "Number of Configured Accounting Servers";
L7_char8 *pStrInfo_security_NumAuthServerGrps = "Number of Named Authentication Server Groups";
L7_char8 *pStrInfo_security_NumAcctServerGrps = "Number of Named Accounting Server Groups";
L7_char8 *pStrInfo_security_Empty = "";
L7_char8 *pStrInfo_security_SupplicantPaeState = "Supplicant PAE State";
L7_char8 *pStrInfo_security_StartPeriodSecs = "Start Period (secs)";
L7_char8 *pStrInfo_security_HeldPeriodSecs = "Held Period (secs)";
L7_char8 *pStrInfo_security_AuthPeriodSecs = "Authentication Period (secs)";
L7_char8 *pStrErr_security_AcctSrvrHostAddr_1 = "RADIUS Accounting Server Host Address";
L7_char8 *pStrInfo_security_RadSrvrIPAddr = "RADIUS Server IP Address";
L7_char8 *pStrInfo_security_RadSrvrDNSAddr = "RADIUS Server DNS Address";
L7_char8 *pStrInfo_security_CurrentSrvrIPAddr = "Current Server IP Address";
L7_char8 *pStrInfo_security_CurrentSrvrDNSAddr = "Current Server DNS Address";
L7_char8 *pStrInfo_security_AcctSrvrIPAddr = "RADIUS Accounting Server IP Address";
L7_char8 *pStrInfo_security_AcctSrvrDNSAddr = "RADIUS Accounting Server DNS Address";
L7_char8 *pStrInfo_security_DNSAddrNotResolved = "0.0.0.0 - Not Resolved";


