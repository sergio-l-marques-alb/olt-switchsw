/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/security/oem_security.c
*
* @purpose    OEM String Helper Functions
*
* @component  WEB
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

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_security_common.h"
#include "strlib_security_web.h"
#include "strlib_base_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebSecurityGetPageHdr1(L7_int32 token);
L7_char8 *usmWebSecurityGetPageHdr2(L7_int32 token);
static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebSecurityGetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 999:
    errMsg = pStrInfo_common_Usrs;
    break;
  case 1000:
    errMsg = pStrInfo_security_UserName;
    break;
  case 1001:
    errMsg = pStrInfo_security_Password;
    break;
  case 1005:
    errMsg = pStrInfo_common_ApIpAddr;
    break;
  case 1006:
    errMsg = pStrInfo_common_ApHostAddr;
    break;
  case 1015:
    errMsg = pStrInfo_common_Yes;
    break;
  case 1016:
    errMsg = pStrInfo_common_No;
    break;
  case 1101:
    errMsg = pStrInfo_common_Port_2;
    break;
  case 1103:
    errMsg = pStrInfo_common_LogicalPort;
    break;
  case 1104:
    errMsg = pStrInfo_common_SuplicantMacAddress;
    break;
  case 1105:
    errMsg = pStrInfo_common_VlanId_1;
    break;
  case 1320:
    errMsg = pStrInfo_security_ClientIPAddr;
    break;
  case 1350:
    errMsg = pStrInfo_security_Configuration;
    break;
  case 1461:
    errMsg = pStrInfo_security_SessionTime;
    break;
  case 1474:
    errMsg = pStrInfo_security_VerificationModeLocal;
    break;
  case 1548:
    errMsg = pStrInfo_security_Protocol;
    break;
  case 1551:
    errMsg = pStrInfo_security_PortStatus;
    break;
  case 1639:
    errMsg = pStrInfo_security_Interface;
    break;
  case 1872:
    errMsg = pStrInfo_security_OperStatus;
    break;
  case 4001:
    errMsg = pStrInfo_common_Dsbl_1;
    break;
  case 4002:
    errMsg = pStrInfo_common_Enbl_1;
    break;
  case 5101:
    errMsg = pStrInfo_common_Auto;
    break;
  case 8808:
    errMsg = pStrInfo_common_Pri_1;
    break;
  case 8847:
    errMsg = pStrInfo_common_True2;
    break;
  case 8848:
    errMsg = pStrInfo_common_False2;
    break;
  case 11122:
    errMsg = pStrInfo_security_Mode;
    break;
  case 12401:
    errMsg = pStrInfo_security_ConfigurationName;
    break;
  case 14983:
    errMsg = pStrInfo_security_PrimarySrvr;
    break;
  case 14996:
    errMsg = pStrInfo_security_CntrlDirection;
    break;
  case 15004:
    errMsg = pStrInfo_common_AdministrativeMode;
    break;
  case 15007:
    errMsg = pStrInfo_common_ApProtoVer;
    break;
  case 15008:
    errMsg = pStrInfo_security_PaeCapabilities;
    break;
  case 15009:
    errMsg = pStrInfo_security_AuthenticatorPaeState;
    break;
  case 15016:
    errMsg = pStrInfo_security_ForceAuthorized;
    break;
  case 15017:
    errMsg = pStrInfo_security_ForceUnAuthorized;
    break;
  case 15018:
    errMsg = pStrInfo_security_MacBasedDot1xAuth;
    break;
  case 15025:
    errMsg = pStrInfo_security_QuietPeriodSecs;
    break;
  case 15026:
    errMsg = pStrInfo_security_TxPeriodSecs;
    break;
  case 15027:
    errMsg = pStrInfo_security_SupplicantTimeoutSecs;
    break;
  case 15028:
    errMsg = pStrInfo_security_MaxReqs;
    break;
  case 15029:
    errMsg = pStrInfo_security_ReauthenticationPeriodSecs;
    break;
  case 15030:
    errMsg = pStrInfo_security_ReauthenticationEnbld;
    break;
  case 15031:
    errMsg = pStrInfo_security_Max_Users;
    break;
  case 15032:
    errMsg = pStrInfo_security_EapolFramesRcvd;
    break;
  case 15033:
    errMsg = pStrInfo_common_EapolFramesTxed;
    break;
  case 15034:
    errMsg = pStrInfo_common_EapolStartFramesRcvd;
    break;
  case 15035:
    errMsg = pStrInfo_security_EapolLogoffFramesRcvd;
    break;
  case 15036:
    errMsg = pStrInfo_security_LastEapolFrameVer;
    break;
  case 15037:
    errMsg = pStrInfo_security_LastEapolFrameSrc;
    break;
  case 15038:
    errMsg = pStrInfo_security_EapRespIdFramesRcvd;
    break;
  case 15039:
    errMsg = pStrInfo_security_EapRespFramesRcvd;
    break;
  case 15040:
    errMsg = pStrInfo_security_EapReqIdFramesTxed;
    break;
  case 15041:
    errMsg = pStrInfo_security_EapReqFramesTxed;
    break;
  case 15042:
    errMsg = pStrErr_security_EapolFramesRcvd_1;
    break;
  case 15043:
    errMsg = pStrErr_security_EapolLenFramesRcvd;
    break;
  case 15051:
    errMsg = pStrInfo_security_BackendState;
    break;
  case 15055:
    errMsg = pStrInfo_security_CntrlMode;
    break;
  case 15056:
    errMsg = pStrInfo_security_OperatingCntrlMode;
    break;
  case 15059:
    errMsg = pStrInfo_security_SrvrTimeoutSecs;
    break;
  case 15075:
    errMsg = pStrInfo_security_VlanAssignmentMode;
    break;
  case 15076:
    errMsg = pStrInfo_security_GuestVlanSupplicantMode;
    break;
  case 15077:
    errMsg = pStrInfo_security_KeyTransmissionEnbld;
    break;
  case 15951:
    errMsg = pStrInfo_security_Secret;
    break;
  case 15955:
    errMsg = pStrInfo_common_ApProfileRadiusSrvrHost_1;
    break;
  case 15961:
    errMsg = pStrInfo_security_ClrAllRadiusStats;
    break;
  case 15963:
    errMsg = pStrInfo_security_VerificationModeRadius;
    break;
  case 15974:
    errMsg = pStrInfo_security_RadiusAttr4NasIpAddr;
    break;
  case 16000:
    errMsg = pStrInfo_security_CurrentSrvrHostAddr;
    break;
  case 16002:
    errMsg = pStrInfo_security_MaxNumOfReTxs;
    break;
  case 16003:
    errMsg = pStrInfo_security_TimeoutDurationSecs;
    break;
  case 16004:
    errMsg = pStrInfo_security_AcctMode;
    break;
  case 16008:
    errMsg = pStrInfo_security_RoundTripTimeSecs;
    break;
  case 16009:
    errMsg = pStrInfo_security_AccessReqs;
    break;
  case 16010:
    errMsg = pStrInfo_security_AccessRetransmissions;
    break;
  case 16011:
    errMsg = pStrInfo_security_AccessAccepts;
    break;
  case 16012:
    errMsg = pStrInfo_security_AccessChallenges;
    break;
  case 16013:
    errMsg = pStrInfo_security_MalformedAccessResps;
    break;
  case 16014:
    errMsg = pStrInfo_security_BadAuthenticators;
    break;
  case 16015:
    errMsg = pStrInfo_security_PendingReqs;
    break;
  case 16016:
    errMsg = pStrInfo_security_Timeouts;
    break;
  case 16017:
    errMsg = pStrInfo_security_UnknownTypes;
    break;
  case 16018:
    errMsg = pStrInfo_security_PktsDropped;
    break;
  case 16019:
    errMsg = pStrInfo_security_AcctSrvrHostAddr;
    break;
  case 16020:
    errMsg = pStrInfo_security_SecretCfgured;
    break;
  case 16021:
    errMsg = pStrInfo_security_AcctReqs;
    break;
  case 16022:
    errMsg = pStrInfo_security_AcctRetransmissions;
    break;
  case 16023:
    errMsg = pStrInfo_security_AcctResps;
    break;
  case 16024:
    errMsg = pStrInfo_security_MalformedAcctResps;
    break;
  case 16025:
    errMsg = pStrErr_security_SrvrAddres;
    break;
  case 16043:
    errMsg = pStrInfo_security_AccessRejects;
    break;
  case 16100:
    errMsg = pStrInfo_common_Login;
    break;
  case 16101:
    errMsg = pStrInfo_common_ClientAuth;
    break;
  case 16150:
    errMsg = pStrInfo_security_Current;
    break;
  case 16170:
    errMsg = pStrInfo_security_MsgAuthenticator;
    break;
  case 21152:
    errMsg = pStrInfo_security_Session_Timeout;
    break;
  case 22070:
    errMsg = pStrInfo_security_KeyString_1;
    break;
  case 22071:
    errMsg = pStrInfo_security_ConnTimeout;
    break;
  case 22072:
    errMsg = pStrInfo_security_TacacsSrvr;
    break;
  case 24098:
    errMsg = pStrInfo_security_SwitchMACAddr;
    break;
  case 24516:
    errMsg = pStrInfo_security_TacacsSrvrAddr;
    break;
  case 25002:
    errMsg = pStrInfo_security_SwitchIPAddr;
    break;
  case 25216:
    errMsg = pStrInfo_security_ClientPktsRx;
    break;
  case 25217:
    errMsg = pStrInfo_security_ClientBytesRx;
    break;
  case 25218:
    errMsg = pStrInfo_security_ClientPktsTx;
    break;
  case 25219:
    errMsg = pStrInfo_security_ClientBytesTx;
    break;
  case 25520:
    errMsg = pStrInfo_security_RadiusAccounting;
    break;
  case 25567:
    errMsg = pStrInfo_security_ProtocolHTTP;
    break;
  case 25938:
    errMsg = pStrInfo_security_GuestVlanId;
    break;
  case 25939:
    errMsg = pStrInfo_security_GuestVlanPeriod;
    break;
  case 25950:
    errMsg = pStrInfo_security_GuestVlanId;
    break;
  case 25951:
    errMsg = pStrInfo_security_UnauthenticatedVlanId;
    break;
  case 25952:
    errMsg = pStrInfo_security_VlanAssigned;
    break;
  case 25953:
    errMsg = pStrInfo_security_VlanAssignedReason;
    break;
  case 25954:
    errMsg = pStrInfo_security_SessionTimeout;
    break;
  case 25955:
    errMsg = pStrInfo_security_SessionTerminateAction;
    break;
  case 26001:
    errMsg =  pStrInfo_security_Dot1xShowClientInterface;
    break;
  case 26002:
    errMsg =  pStrInfo_security_Dot1xShowClientUserName;
    break;
  case 26003:
    errMsg =  pStrInfo_security_Dot1xShowClientMacAddr;
    break;
  case 26004:
    errMsg = pStrInfo_security_Dot1xShowClientSessionTime;
    break;
  case 26005:
    errMsg = pStrInfo_security_Dot1xShowClientFilterId;
    break;
  case 26006:
    errMsg = pStrInfo_security_Dot1xShowClientVland;
    break;
  case 27101:
    errMsg = pStrInfo_security_EnableCP;
    break;
  case 27102:
    errMsg = pStrInfo_security_PeerSwStatReportInterval;
    break;
  case 27103:
    errMsg = pStrInfo_security_AuthSessionTimeout;
    break;
  case 27104:
    errMsg = pStrInfo_security_CPConfig;
    break;
  case 27105:
    errMsg = pStrInfo_security_UserList;
    break;
  case 27106:
    errMsg = pStrInfo_security_UserGroup;
    break;
  case 27107:
    errMsg = pStrInfo_security_SessionTimeoutSecs;
    break;
  case 27108:
    errMsg = pStrInfo_security_IdleTimeout;
    break;
  case 27109:
    errMsg = pStrInfo_security_MaxBwUp;
    break;
  case 27110:
    errMsg = pStrInfo_security_MaxBwDown;
    break;
  case 27111:
    errMsg = pStrInfo_security_MaxInputOctets;
    break;
  case 27112:
    errMsg = pStrInfo_security_MaxOutputOctets;
    break;
  case 27113:
    errMsg = pStrInfo_security_MaxTotalOctets;
    break;
  case 27114:
    errMsg = pStrInfo_security_CPGlobalOperStatus;
    break;
  case 27115:
    errMsg = pStrInfo_security_CPGlobalDisableReason;
    break;
  case 27116:
    errMsg = pStrInfo_security_CPIPAddress;
    break;
  case 27117:
    errMsg = pStrInfo_security_SupportedCPs;
    break;
  case 27118:
    errMsg = pStrInfo_security_ConfiguredCPs;
    break;
  case 27119:
    errMsg = pStrInfo_security_ActiveCPs;
    break;
  case 27120:
    errMsg = pStrInfo_security_SupportedUsersInSystem;
    break;
  case 27121:
    errMsg = pStrInfo_security_AuthenticatedUsers;
    break;
  case 27122:
    errMsg = pStrInfo_security_AuthFailureTableSize;
    break;
  case 27123:
    errMsg = pStrInfo_security_UsersInAuthFailureTable;
    break;
  case 27124:
    errMsg = pStrInfo_security_ActivityLogTableSize;
    break;
  case 27125:
    errMsg = pStrInfo_security_ActivityEntriesInLog;
    break;
  case 27126:
    errMsg = pStrInfo_security_HTTPPort;
    break;
  case 27127:
    errMsg = pStrInfo_security_DisableReason;
    break;
  case 27128:
    errMsg = pStrInfo_security_BlockedStatus;
    break;
  case 27129:
    errMsg = pStrInfo_security_Verification;
    break;
  case 27130:
    errMsg = pStrInfo_security_Languages;
    break;
  case 27131:
    errMsg = pStrInfo_security_SwitchType;
    break;
  case 27132:
    errMsg = pStrInfo_security_TimeSinceLastStatReport;
    break;
  case 27133:
    errMsg = pStrInfo_security_NoUsersExist;
    break;
  case 27134:
    errMsg = pStrInfo_security_ClientBytesRx;
    break;
  case 27135:
    errMsg = pStrInfo_security_ClientPktsRx;
    break;
  case 27136:
    errMsg = pStrInfo_security_ClientBytesTx;
    break;
  case 27137:
    errMsg = pStrInfo_security_ClientPktsTx;
    break;
  case 27138:
    errMsg = pStrInfo_security_NoCPClientsExist;
    break;
  case 27139:
    errMsg = pStrInfo_security_CPMode;
    break;
  case 27140:
    errMsg = pStrInfo_security_RoamingSupport;
    break;
  case 27141:
    errMsg = pStrInfo_security_MaxInputOctetsMonitor;
    break;
  case 27142:
    errMsg = pStrInfo_security_MaxOutputOctetsMonitor;
    break;
  case 27143:
    errMsg = pStrInfo_security_BytesRxCounter;
    break;
  case 27144:
    errMsg = pStrInfo_security_BytesTxCounter;
    break;
  case 27145:
    errMsg = pStrInfo_security_PktsRxCounter;
    break;
  case 27146:
    errMsg = pStrInfo_security_PktsTxCounter;
    break;
  case 27147:
    errMsg = pStrInfo_security_Idle_Timeout;
    break;
  case 27148:
    errMsg = pStrInfo_security_ProtocolMode;
    break;
  case 27149:
    errMsg = pStrInfo_security_VerificationMode;
    break;
  case 27150:
    errMsg = pStrInfo_security_CfgURLRedirectMode;
    break;
  case 27151:
    errMsg = pStrInfo_security_CfgURL;
    break;
  case 27152:
    errMsg = pStrInfo_security_RadiusAuthServer;
    break;
  case 27153:
    errMsg = pStrInfo_security_UserDownRate;
    break;
  case 27154:
    errMsg = pStrInfo_security_IntrusionThreshold;
    break;
  case 27155:
    errMsg = pStrInfo_security_AcceptanceUsePolicy;
    break;
  case 27156:
    errMsg = pStrInfo_security_ProtocolHTTPS;
    break;
  case 27157:
    errMsg = pStrInfo_security_VerificationModeGuest;
    break;
  case 27158:
    errMsg = pStrInfo_security_AssocInterfaces;
    break;
  case 27159:
    errMsg = pStrInfo_security_InterfaceList;
    break;
  case 27160:
    errMsg = pStrInfo_security_NoIntfAssocCPInstance;
    break;
  case 27161:
    errMsg = pStrInfo_security_NoCPClientsExistOnThisIntf;
    break;
  case 27162:
    errMsg = pStrInfo_security_NoCPClientsExistOnThisCP;
    break;
  /*case 27164:
    errMsg = pStrInfo_security_CaptivePortal;
    break;*/
  case 27165:
    errMsg = pStrInfo_security_CPTrapMode;
    break;
  case 27166:
    errMsg = pStrInfo_security_CPCltAuthFailureTrapMode;
    break;
  case 27167:
    errMsg = pStrInfo_security_CPCltConnectionTrapMode;
    break;
  case 27168:
    errMsg = pStrInfo_security_CPCltDBFullTrapMode;
    break;
  case 27169:
    errMsg = pStrInfo_security_CPCltDisconnectionTrapMode;
    break;

  case 27170:
    errMsg = pStrInfo_common_RadiusSrvrName;
    break;
  case 27171:
    errMsg = pStrInfo_security_NoRadNamedServersExist;
    break;
  case 27172:
    errMsg = pStrInfo_security_NoRadNamedAcctServersExist;
    break;
  case 27173:
    errMsg = pStrErr_security_AcctSrvrHostAddr_1;
    break;
  case 27174:
    errMsg = pStrInfo_wireless_WsRadiusAcctSrvrName;
    break;
  case 27175:
    errMsg = pStrInfo_base_DnsIPAddr;
    break;
  case 27176:
    errMsg = pStrInfo_security_SrvrType;
    break;
  case 27177:
    errMsg = pStrInfo_security_PortNum_1;
    break;
  case 27178:
    errMsg = pStrInfo_security_LocalUsersInSystem;
    break;
  case 27179:
    errMsg = pStrInfo_security_NumAuthServers;
    break;
  case 27180:
    errMsg = pStrInfo_security_NumAcctServers;
    break;
  case 27181:
    errMsg = pStrInfo_security_NumAuthServerGrps;
    break;
  case 27182:
    errMsg = pStrInfo_security_NumAcctServerGrps;
    break;
  case 27183:
    errMsg = pStrInfo_security_Current1;
    break;
  case 27185:
    errMsg = pStrInfo_security_PaeCapabilities;
    break;
  case 27186:
    errMsg = pStrInfo_security_PaeSupplicant;
    break;
  case 27187:
    errMsg = pStrInfo_security_PaeAuthenticator;
    break;
  case 27188:
  errMsg = pStrInfo_security_StartPeriodSecs;
  break;
  case 27189:
  errMsg = pStrInfo_security_HeldPeriodSecs;
  break;
  case 27190:
  errMsg = pStrInfo_security_AuthPeriodSecs;
  break;
  case 27191:
  errMsg = pStrInfo_security_MaxStarts;
  break;
  case 27192:
  errMsg = pStrInfo_security_SupplicantPaeState;
  break;
  case 27193:
  errMsg = pStrInfo_security_SupplicantPortAccessCntrlStats;
  break;
  case 27194:
  errMsg = pStrInfo_common_EapolStartFramesTxed;
  break;
  case 27195:
  errMsg = pStrInfo_security_EapRespIdFramesTxed_1;
  break;
  case 27196:
  errMsg = pStrInfo_security_EapRespFramesTxed;
  break;
  case 27197:
  errMsg = pStrInfo_security_EapReqIdFramesRcvd_1;
  break;
  case 27198:
  errMsg = pStrInfo_security_EapReqFramesRcvd;
  break;

  case 27199:
    errMsg = pStrInfo_security_RadSrvrIPAddr;
    break;
  case 27200:
    errMsg = pStrInfo_security_AcctSrvrIPAddr;
    break;
  case 27201:
    errMsg = pStrInfo_security_HTTPSecurePort;
    break;
  case 27202:
    errMsg = pStrInfo_security_UserLogoutMode;
    break;
  case 27203:
    errMsg = pStrInfo_security_ConfiguredLocalUsers;
    break;
	case 27204:
    errMsg = pStrInfo_security_ActStatus;
    break;

  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebSecurityGetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    local_call=L7_TRUE;
    errMsg = usmWebSecurityGetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebSecurityGetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1039:
    errMsg = pStrInfo_security_PortAccessSummary;
    break;
  case 1042:
    errMsg = pStrInfo_security_PortAccessCntrlUsrLoginCfg;
    break;
  case 1129:
    errMsg = pStrInfo_security_PortAccessPrivileges;
    break;
  case 1130:
    errMsg = pStrInfo_security_RadiusAcctSrvrCfg;
    break;
  case 1131:
    errMsg = pStrInfo_security_RadiusAcctSrvrStats;
    break;
  case 1132:
    errMsg = pStrInfo_security_RadiusCfg;
    break;
  case 1133:
    errMsg = pStrInfo_security_RadiusClrStats;
    break;
  case 1134:
    errMsg = pStrInfo_security_RadiusSrvrStats;
    break;
  case 1135:
    errMsg = pStrInfo_security_RadiusSrvrCfg;
    break;
  case 1137:
    errMsg = pStrInfo_security_PortAccessCntrlCfg;
    break;
  case 1138:
    errMsg = pStrInfo_security_PortAccessCntrlPortCfg;
    break;
  case 1139:
    errMsg = pStrInfo_security_PortAccessCntrlStats;
    break;
  case 1140:
    errMsg = pStrInfo_security_PortAccessCntrlStatus;
    break;
  case 1141:
    errMsg = pStrInfo_security_PortAccessCntrlPortSummary;
    break;
  case 1142:
    errMsg = pStrInfo_security_PortAccessClientSummary;
    break;
  case 1143:
    errMsg = pStrInfo_security_PortAccessClientDetail;
    break;
  case 1145:
    errMsg = pStrInfo_security_SecureShellCfg;
    break;
  case 1146:
    errMsg = pStrInfo_security_SecureHttpCfg;
    break;
  case 1182:
    errMsg = pStrInfo_security_TacacsCfg;
    break;
  case 1183:
    errMsg = pStrInfo_security_TacacsSrvrCfg;
    break;

  case 1951:
    errMsg = pStrInfo_security_TitleCPGlobalCfg;
    break;
  case 1952:
    errMsg = pStrInfo_security_TitleCPCfg;
    break;
  case 1953:
    errMsg = pStrInfo_security_TitleCPWebCustom;
    break;
  case 1954:
    errMsg = pStrInfo_security_TitleCPUserLocalCfg;
    break;
  case 1955:
    errMsg = pStrInfo_security_TitleCPIntfAssocCfg;
    break;
  case 1956:
    errMsg = pStrInfo_security_TitleCPGlobalStatus;
    break;
  case 1957:
    errMsg = pStrInfo_security_TitleCPStatus;
    break;
  case 1958:
    errMsg = pStrInfo_security_TitleCPIntfStatus;
    break;
  case 1959:
    errMsg = pStrInfo_security_TitleCPIntfCapStatus;
    break;
  case 1960:
    errMsg = pStrInfo_security_TitleCPClientStatus;
    break;
  case 1961:
    errMsg = pStrInfo_security_TitleCPClientIntfStatus;
    break;
  case 1962:
    errMsg = pStrInfo_security_TitleCPClientCPStatus;
    break;
  case 1963:
    errMsg = pStrInfo_security_TitleCPSessionStats;
    break;
  case 1964:
    errMsg = pStrInfo_security_TitleCPUserLocal;
    break;
  case 1965:
    errMsg = pStrInfo_security_TitleCPCfgMgmt;
    break;
  case 1966:
    errMsg = pStrInfo_security_TitleCPClients;
    break;
  case 1967:
    errMsg = pStrInfo_security_TitleCPTrapCfg;
    break;
  case 1968:
    errMsg = pStrInfo_security_RadiusNamedStatus;
    break;
  case 1969:
    errMsg = pStrInfo_security_RadiusNamedAcctStatus;
    break;
  case 1970:
    errMsg = pStrInfo_security_TitlePAECfg;
    break;
  case 1971:
    errMsg = pStrInfo_security_TitleSuppCfg;
    break;

  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  if( local_call == L7_TRUE )
     return errMsg;
  else
     return usmWebPageHeader1stGet(errMsg);
}

L7_char8 *usmWebSecurityGetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2039:
    errMsg = pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportaccesssummary;
    break;
  case 2042:
    errMsg = pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xloginconfig;
    break;
  case 2100:
    errMsg = pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusAcctStatus;
    break;
  case 2129:
    errMsg = pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportaccesscfg;
    break;
  case 2130:
    errMsg = pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusAcctcfg;
    break;
  case 2131:
    errMsg = pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadacctstats;
    break;
  case 2132:
    errMsg = pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadcfg;
    break;
  case 2133:
    errMsg = pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadclearstats;
    break;
  case 2134:
    errMsg = pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadservstats;
    break;
  case 2135:
    errMsg = pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadservcfg;
    break;
  case 2136:
    errMsg = pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusStatus;
    break;
  case 2137:
    errMsg = pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xcfg;
    break;
  case 2138:
    errMsg = pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportcfg;
    break;
  case 2139:
    errMsg = pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xstats;
    break;
  case 2140:
    errMsg = pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xstatus;
    break;
  case 2141:
    errMsg = pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportstatus;
    break;
  case 2142:
    errMsg = pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xClientSummary;
    break;
  case 2143:
    errMsg = pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xClientDetail;
    break;
  case 2145:
    errMsg = pStrInfo_security_HtmlFileSecuritySshHelpSsh;
    break;
  case 2146:
    errMsg = pStrInfo_security_HtmlFileSecuritySslHelpHttps;
    break;
  case 2182:
    errMsg = pStrInfo_security_HtmlLinkSecurityTacacsHelpTacacsTacacscfg;
    break;
  case 2183:
    errMsg = pStrInfo_security_HtmlLinkSecurityTacacsHelpTacacsTacacsservercfg;
    break;

  case 2751:
    errMsg = pStrInfo_security_HtmlLinkHelpCPGlobalCfg;
    break;
  case 2752:
    errMsg = pStrInfo_security_HtmlLinkHelpCPCfg;
    break;
  case 2753:
    errMsg = pStrInfo_security_HtmlLinkHelpCPWebCustom;
    break;
  case 2754:
    errMsg = pStrInfo_security_HtmlLinkHelpCPUserLocalCfg;
    break;
  case 2755:
    errMsg = pStrInfo_security_HtmlLinkHelpCPIntfAssocCfg;
    break;
  case 2756:
    errMsg = pStrInfo_security_HtmlLinkHelpCPGlobalStatus;
    break;
  case 2757:
    errMsg = pStrInfo_security_HtmlLinkHelpCPStatus;
    break;
  case 2758:
    errMsg = pStrInfo_security_HtmlLinkHelpCPIntfStatus;
    break;
  case 2759:
    errMsg = pStrInfo_security_HtmlLinkHelpCPIntfCapStatus;
    break;
  case 2760:
    errMsg = pStrInfo_security_HtmlLinkHelpCPClientStatus;
    break;
  case 2761:
    errMsg = pStrInfo_security_HtmlLinkHelpCPClientIntfStatus;
    break;
  case 2762:
    errMsg = pStrInfo_security_HtmlLinkHelpCPClientCPStatus;
    break;
  case 2763:
    errMsg = pStrInfo_security_HtmlLinkHelpCPSessionStats;
    break;
  case 2764:
    errMsg = pStrInfo_security_HtmlLinkHelpCPUserLocal;
    break;
  case 2765:
    errMsg = pStrInfo_security_HtmlLinkHelpCPCfgMgmt;
    break;
  case 2766:
    errMsg = pStrInfo_security_HtmlLinkHelpCPClients;
    break;
  case 2767:
    errMsg = pStrInfo_security_HtmlLinkHelpCPUserLocalAdd;
    break;
  case 2768:
    errMsg = pStrInfo_security_HtmlLinkHelpCPTrapCfg;
    break;

  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}

