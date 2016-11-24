/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   src/nls/include/strlib_security_web.h
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

#ifndef STRLIB_SECURITY_WEB_C
#define STRLIB_SECURITY_WEB_C

#include "datatypes.h"
extern L7_char8 *pStrInfo_security_AccessAccepts;
extern L7_char8 *pStrInfo_security_AccessChallenges;
extern L7_char8 *pStrInfo_security_AccessRetransmissions;
extern L7_char8 *pStrInfo_security_AcctMode;
extern L7_char8 *pStrInfo_security_AcctReqs;
extern L7_char8 *pStrInfo_security_AcctResps;
extern L7_char8 *pStrInfo_security_AcctRetransmissions;
extern L7_char8 *pStrInfo_security_AcctSrvrHostAddr;
extern L7_char8 *pStrInfo_security_BackendState;
extern L7_char8 *pStrInfo_security_BothShldNotBeSimultaneouslyDsbld;
extern L7_char8 *pStrInfo_security_Certificate;
extern L7_char8 *pStrInfo_security_ClrAllRadiusStats;
extern L7_char8 *pStrInfo_security_ConnTimeout;
extern L7_char8 *pStrInfo_security_Current;
extern L7_char8 *pStrInfo_security_DownloadCertificates;
extern L7_char8 *pStrInfo_security_DownloadHostKeys;
extern L7_char8 *pStrInfo_security_EapReqIdFramesTxed;
extern L7_char8 *pStrInfo_security_EapRespIdFramesRcvd;
extern L7_char8 *pStrInfo_security_FailedToGenerateCertificate;
extern L7_char8 *pStrErr_security_FailedToGetRadiusSrvrIpAddrSpecified;
extern L7_char8 *pStrInfo_security_GuestVlanSupplicantMode;
extern L7_char8 *pStrInfo_security_HttpsAdminMode;
extern L7_char8 *pStrInfo_security_HttpsHardTimeout;
extern L7_char8 *pStrInfo_security_HttpsPort;
extern L7_char8 *pStrInfo_security_HttpsSoftTimeout;
extern L7_char8 *pStrInfo_security_KeyString_1;
extern L7_char8 *pStrInfo_security_LenOfKeyStringIsGreaterThan128Chars;
extern L7_char8 *pStrInfo_security_MalformedAcctResps;
extern L7_char8 *pStrInfo_security_MaxNumOfReTxs;
extern L7_char8 *pStrInfo_security_MaxSshSession;
extern L7_char8 *pStrInfo_security_MaxNumOfHttpsSessions;
/*extern L7_char8 *pStrInfo_security_MsgAuthenticator;*/
extern L7_char8 *pStrInfo_security_OperatingCntrlMode;
extern L7_char8 *pStrInfo_security_PktsDropped;
extern L7_char8 *pStrInfo_security_PendingReqs;
extern L7_char8 *pStrInfo_security_PortAccessCntrlCfg;
extern L7_char8 *pStrInfo_security_PortAccessCntrlPortCfg;
extern L7_char8 *pStrInfo_security_PortAccessCntrlSupplicantPortCfg;
extern L7_char8 *pStrInfo_security_PortAccessCntrlSupplicantPortCfg;
extern L7_char8 *pStrInfo_security_PortAccessCntrlPortSummary;
extern L7_char8 *pStrInfo_security_PortAccessClientSummary;
extern L7_char8 *pStrInfo_security_PortAccessClientDetail;
extern L7_char8 *pStrInfo_security_PortAccessCntrlStats;
extern L7_char8 *pStrInfo_security_SupplicantPortAccessCntrlStats;
extern L7_char8 *pStrInfo_security_PortAccessCntrlStatus;
extern L7_char8 *pStrInfo_security_PortAccessCntrlUsrLoginCfg;
extern L7_char8 *pStrInfo_security_PortAccessPrivileges;
extern L7_char8 *pStrInfo_security_PortAccessSummary;
extern L7_char8 *pStrInfo_security_PortStatus;
extern L7_char8 *pStrInfo_security_PrimarySrvr;
extern L7_char8 *pStrInfo_security_ProtoLvl;
extern L7_char8 *pStrInfo_security_RadiusAcctSrvrCfg;
extern L7_char8 *pStrInfo_security_RadiusAcctSrvrStats;
extern L7_char8 *pStrInfo_security_RadiusClrStats;
extern L7_char8 *pStrInfo_security_RadiusCfg;
extern L7_char8 *pStrInfo_security_RadiusSrvrCfg;
extern L7_char8 *pStrInfo_security_RadiusSrvrStats;
extern L7_char8 *pStrInfo_security_RadiusAttr4NasIpAddr;
extern L7_char8 *pStrInfo_security_Reauthenticate;
extern L7_char8 *pStrInfo_security_RoundTripTimeSecs;
extern L7_char8 *pStrInfo_security_SshSessionIdleTimeout;
extern L7_char8 *pStrInfo_security_SslVer3Mode;
extern L7_char8 *pStrInfo_security_Secret;
extern L7_char8 *pStrInfo_security_SecureHttpCfg;
extern L7_char8 *pStrInfo_security_SecureShellCfg;
extern L7_char8 *pStrInfo_security_Supplicant_1;
extern L7_char8 *pStrInfo_security_TacacsCfg;
extern L7_char8 *pStrInfo_security_TacacsSrvr;
extern L7_char8 *pStrInfo_security_TacacsSrvrAddr;
extern L7_char8 *pStrInfo_security_TacacsSrvrCfg;
extern L7_char8 *pStrInfo_security_TlsVer1Mode;
extern L7_char8 *pStrInfo_security_TlsVer1ModeAndSslVer3Mode;
extern L7_char8 *pStrInfo_security_TimeoutSecs;
extern L7_char8 *pStrInfo_security_TimeoutDurationSecs;
extern L7_char8 *pStrInfo_security_VlanAssignmentMode;
extern L7_char8 *pStrInfo_security_Ver1OrVer2MustBeEnbld;
extern L7_char8 *pStrInfo_security_AddRadiusSrvr;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xAplCfg;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xCfg;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xPortCfg;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xSupplicantPortCfg;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xSuppPortCfg;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xPortSumm;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xStats;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xStatus;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xSumm;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xUsrPorts;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xClientSumm;
extern L7_char8 *pStrInfo_security_False_1;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xcfg;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xloginconfig;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportaccesscfg;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportaccesssummary;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportcfg;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportstatus;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xClientSummary;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xClientDetail;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xstats;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xstatus;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusAcctcfg;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadacctstats;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadcfg;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadclearstats;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadservcfg;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadservstats;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusStatus;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusAcctStatus;
extern L7_char8 *pStrInfo_security_HtmlFileSecuritySshHelpSsh;
extern L7_char8 *pStrInfo_security_HtmlFileSecuritySslHelpHttps;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityTacacsHelpTacacsTacacscfg;
extern L7_char8 *pStrInfo_security_HtmlLinkSecurityTacacsHelpTacacsTacacsservercfg;
extern L7_char8 *pStrInfo_security_HtmlFileRadiusAcctSrvr;
extern L7_char8 *pStrInfo_security_HtmlFileRadiusAcctStats;
extern L7_char8 *pStrInfo_security_HtmlFileRadiusClr;
extern L7_char8 *pStrInfo_security_HtmlFileRadiusCfg;
extern L7_char8 *pStrInfo_security_HtmlFileRadiusServStats;
extern L7_char8 *pStrInfo_security_HtmlFileRadiusSrvr;
extern L7_char8 *pStrInfo_security_RemoveAcctSrvr;
extern L7_char8 *pStrInfo_security_RemoveRadiusSrvr;
extern L7_char8 *pStrInfo_security_SetBridgePri;
extern L7_char8 *pStrInfo_security_HtmlFileSshCfg;
extern L7_char8 *pStrInfo_security_HtmlFileSslCfg;
extern L7_char8 *pStrInfo_security_HtmlFileTacacsCfg;
extern L7_char8 *pStrInfo_security_HtmlFileTacacsSrvrCfgRo;
extern L7_char8 *pStrInfo_security_HtmlFileTacacsSrvrCfgRw;
extern L7_char8 *pStrInfo_security_True_1;
extern L7_char8 *pStrInfo_security_WhileTryingToInitializePort;
extern L7_char8 *pStrInfo_security_WhileTryingToReauthenticatePort;
extern L7_char8 *pStrInfo_security_MacBasedDot1xAuth;
extern L7_char8 *pStrInfo_security_HtmlFileDot1xClientsDetail;

/* CAPTIVE PORTAL STRINGS */

/* html files */
extern L7_char8 *pStrInfo_security_HtmlFileCPGlobalCfg;
extern L7_char8 *pStrInfo_security_HtmlFileCPCfg;
extern L7_char8 *pStrInfo_security_HtmlFileCPCfgMgmt;
extern L7_char8 *pStrInfo_security_HtmlFileCPWebCustom;
extern L7_char8 *pStrInfo_security_HtmlFileCPUserLocal;
extern L7_char8 *pStrInfo_security_HtmlFileCPUserLocalCfg;
extern L7_char8 *pStrInfo_security_HtmlFileCPIntfAssocCfg;
extern L7_char8 *pStrInfo_security_HtmlFileCPGlobalStatus;
extern L7_char8 *pStrInfo_security_HtmlFileCPStatus;
extern L7_char8 *pStrInfo_security_HtmlFileCPIntfStatus;
extern L7_char8 *pStrInfo_security_HtmlFileCPIntfCapStatus;
extern L7_char8 *pStrInfo_security_HtmlFileCPClients;
extern L7_char8 *pStrInfo_security_HtmlFileCPClientStatus;
extern L7_char8 *pStrInfo_security_HtmlFileCPClientIntfStatus;
extern L7_char8 *pStrInfo_security_HtmlFileCPClientCPStatus;
extern L7_char8 *pStrInfo_security_HtmlFileCPSessionStats;
extern L7_char8 *pStrInfo_security_HtmlFileCPTrapCfg;

/* page titles */
extern L7_char8 *pStrInfo_security_TitleCPGlobalCfg;
extern L7_char8 *pStrInfo_security_TitleCPCfg;
extern L7_char8 *pStrInfo_security_TitleCPCfgMgmt;
extern L7_char8 *pStrInfo_security_TitleCPWebCustom;
extern L7_char8 *pStrInfo_security_TitleCPUserLocal;
extern L7_char8 *pStrInfo_security_TitleCPUserLocalCfg;
extern L7_char8 *pStrInfo_security_TitleCPIntfAssocCfg;
extern L7_char8 *pStrInfo_security_TitleCPGlobalStatus;
extern L7_char8 *pStrInfo_security_TitleCPStatus;
extern L7_char8 *pStrInfo_security_TitleCPIntfStatus;
extern L7_char8 *pStrInfo_security_TitleCPIntfCapStatus;
extern L7_char8 *pStrInfo_security_TitleCPClients;
extern L7_char8 *pStrInfo_security_TitleCPClientStatus;
extern L7_char8 *pStrInfo_security_TitleCPClientIntfStatus;
extern L7_char8 *pStrInfo_security_TitleCPClientCPStatus;
extern L7_char8 *pStrInfo_security_TitleCPSessionStats;
extern L7_char8 *pStrInfo_security_TitleCPTrapCfg;
extern L7_char8 *pStrInfo_security_TitlePAECfg;
extern L7_char8 *pStrInfo_security_TitleSuppCfg;

/* help links */
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPGlobalCfg;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPCfg;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPCfgMgmt;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPWebCustom;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPUserLocal;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPUserLocalAdd;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPUserLocalCfg;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPIntfAssocCfg;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPGlobalStatus;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPStatus;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPIntfStatus;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPIntfCapStatus;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPClients;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPClientStatus;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPSessionStats;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPClientIntfStatus;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPClientCPStatus;
extern L7_char8 *pStrInfo_security_HtmlLinkHelpCPTrapCfg;

/* global config */
extern L7_char8 *pStrInfo_security_EnableCP;
extern L7_char8 *pStrInfo_security_HTTPPort;
extern L7_char8 *pStrInfo_security_HTTPSecurePort;
extern L7_char8 *pStrInfo_security_PeerSwStatReportInterval;
extern L7_char8 *pStrInfo_security_AuthSessionTimeout;

/* cp config */
extern L7_char8 *pStrInfo_security_Configuration;
extern L7_char8 *pStrInfo_security_CPName;
extern L7_char8 *pStrInfo_security_Mode;
extern L7_char8 *pStrInfo_security_Protocol;
extern L7_char8 *pStrInfo_security_Verification;
extern L7_char8 *pStrInfo_security_Language;
extern L7_char8 *pStrInfo_security_Languages;
extern L7_char8 *pStrInfo_security_ProtocolHTTP;
extern L7_char8 *pStrInfo_security_ProtocolHTTPS;
extern L7_char8 *pStrInfo_security_VerificationModeGuest;
extern L7_char8 *pStrInfo_security_VerificationModeLocal;
extern L7_char8 *pStrInfo_security_VerificationModeRadius;
extern L7_char8 *pStrInfo_security_ConfigurationName;
extern L7_char8 *pStrInfo_security_ProtocolMode;
extern L7_char8 *pStrInfo_security_VerificationMode;
extern L7_char8 *pStrInfo_security_RadiusAccounting;
extern L7_char8 *pStrInfo_security_CfgURLRedirectMode;
extern L7_char8 *pStrInfo_security_CfgURL;
extern L7_char8 *pStrInfo_security_RadiusAuthServer;
extern L7_char8 *pStrInfo_security_UserDownRate;
extern L7_char8 *pStrInfo_security_IntrusionThreshold;
extern L7_char8 *pStrInfo_security_AcceptanceUsePolicy;
extern L7_char8 *pStrErr_security_RadiusSrvrMustBeAlphaNumericPlus;
extern L7_char8 *pStrInfo_security_UserLogoutMode;
extern L7_char8 *pStrInfo_security_ConfiguredLocalUsers;
/* cp web customization */

/* user local */
extern L7_char8 *pStrInfo_security_UserList;
extern L7_char8 *pStrInfo_security_UserName;
extern L7_char8 *pStrInfo_security_Password;
extern L7_char8 *pStrInfo_security_PasswordEncrypted;
extern L7_char8 *pStrInfo_security_UserGroup;
extern L7_char8 *pStrInfo_security_SessionTimeoutSecs;
extern L7_char8 *pStrInfo_security_IdleTimeout;
extern L7_char8 *pStrInfo_security_MaxBwUp;
extern L7_char8 *pStrInfo_security_MaxBwDown;
extern L7_char8 *pStrInfo_security_MaxInputOctets;
extern L7_char8 *pStrInfo_security_MaxOutputOctets;
extern L7_char8 *pStrInfo_security_MaxTotalOctets;
extern L7_char8 *pStrInfo_security_BrowserTitle;
extern L7_char8 *pStrInfo_security_PageTitle;
extern L7_char8 *pStrInfo_security_AccountLabel;
extern L7_char8 *pStrInfo_security_UserLabel;
extern L7_char8 *pStrInfo_security_PasswordLabel;
extern L7_char8 *pStrInfo_security_ButtonLabel;
extern L7_char8 *pStrInfo_security_AUPText;
extern L7_char8 *pStrInfo_security_AcceptText;
extern L7_char8 *pStrInfo_security_InstructionalText;
extern L7_char8 *pStrInfo_security_DeniedMsg;
extern L7_char8 *pStrInfo_security_ResourceMsg;
extern L7_char8 *pStrInfo_security_TimeoutMsg;
extern L7_char8 *pStrInfo_security_NoAcceptMsg;
extern L7_char8 *pStrInfo_security_WIPMsg;
extern L7_char8 *pStrInfo_security_WelcomeTitle;
extern L7_char8 *pStrInfo_security_WelcomeText;
extern L7_char8 *pStrInfo_security_ScriptText;
extern L7_char8 *pStrInfo_security_PopupText;
extern L7_char8 *pStrInfo_security_LogoutBrowserTitleText;
extern L7_char8 *pStrInfo_security_LogoutPageTitleText;
extern L7_char8 *pStrInfo_security_LogoutInstructText;
extern L7_char8 *pStrInfo_security_LogoutButtonLabel;
extern L7_char8 *pStrInfo_security_LogoutConfirmText;
extern L7_char8 *pStrInfo_security_LogoutSuccessBrowserTitleText;
extern L7_char8 *pStrInfo_security_LogoutSuccessPageTitleText;
extern L7_char8 *pStrInfo_security_LogoutSuccessInstructText;
extern L7_char8 *pStrInfo_security_MaxNumOfChars;

/* interface association */
extern L7_char8 *pStrInfo_security_Interface;
extern L7_char8 *pStrInfo_security_InterfaceList;
extern L7_char8 *pStrInfo_security_AssocInterfaces;
extern L7_char8 *pStrInfo_security_CPConfig;
extern L7_char8 *pStrInfo_security_Physical;

/* cp global status */
extern L7_char8 *pStrInfo_security_CPGlobalOperStatus;
extern L7_char8 *pStrInfo_security_CPGlobalDisableReason;
extern L7_char8 *pStrInfo_security_CPIPAddress;
extern L7_char8 *pStrInfo_security_SupportedCPs;
extern L7_char8 *pStrInfo_security_ConfiguredCPs;
extern L7_char8 *pStrInfo_security_ActiveCPs;
extern L7_char8 *pStrInfo_security_SupportedUsersInSystem;
extern L7_char8 *pStrInfo_security_LocalUsersInSystem;
extern L7_char8 *pStrInfo_security_AuthenticatedUsers;
extern L7_char8 *pStrInfo_security_AuthFailureTableSize;
extern L7_char8 *pStrInfo_security_UsersInAuthFailureTable;
extern L7_char8 *pStrInfo_security_ActivityLogTableSize;
extern L7_char8 *pStrInfo_security_ActivityEntriesInLog;
extern L7_char8 *pStrInfo_security_CPModeEnablePending;
extern L7_char8 *pStrInfo_security_CPModeDisablePending;
extern L7_char8 *pStrInfo_security_CPModeReasonAdmin;
extern L7_char8 *pStrInfo_security_CPModeReasonNoIpAddr;
extern L7_char8 *pStrInfo_security_CPModeReasonRoutingNoIpRoutingIntf;
extern L7_char8 *pStrInfo_security_CPModeReasonRoutingDisabled;

/* cp activation/activity status */
extern L7_char8 *pStrInfo_security_OperStatus;
extern L7_char8 *pStrInfo_security_DisableReason;
extern L7_char8 *pStrInfo_security_BlockedStatus;
extern L7_char8 *pStrInfo_security_NoRadiusServer;
extern L7_char8 *pStrInfo_security_NoAcctServer;
extern L7_char8 *pStrInfo_security_NoAssocWithIntf;
extern L7_char8 *pStrInfo_security_NoValidActiveIntf;
extern L7_char8 *pStrInfo_security_NoValidCert;
extern L7_char8 *pStrInfo_security_Blocked;
extern L7_char8 *pStrInfo_security_NotBlocked;
extern L7_char8 *pStrInfo_security_BlockedPending;
extern L7_char8 *pStrInfo_security_NotBlockedPending;

/* interface activation status */
extern L7_char8 *pStrInfo_security_IntfNotAttached;

/* interface capability status */
extern L7_char8 *pStrInfo_security_CPMode;
extern L7_char8 *pStrInfo_security_RoamingSupport;
extern L7_char8 *pStrInfo_security_MaxInputOctetsMonitor;
extern L7_char8 *pStrInfo_security_MaxOutputOctetsMonitor;
extern L7_char8 *pStrInfo_security_BytesRxCounter;
extern L7_char8 *pStrInfo_security_BytesTxCounter;
extern L7_char8 *pStrInfo_security_PktsRxCounter;
extern L7_char8 *pStrInfo_security_PktsTxCounter;
extern L7_char8 *pStrInfo_security_Session_Timeout;
extern L7_char8 *pStrInfo_security_Idle_Timeout;

/* client status */
extern L7_char8 *pStrInfo_security_ClientIPAddr;
extern L7_char8 *pStrInfo_security_SwitchMACAddr;
extern L7_char8 *pStrInfo_security_SwitchIPAddr;
extern L7_char8 *pStrInfo_security_SwitchType;
extern L7_char8 *pStrInfo_security_SessionTime;
extern L7_char8 *pStrInfo_security_TimeSinceLastStatReport;
extern L7_char8 *pStrInfo_security_Local;
extern L7_char8 *pStrInfo_security_Peer;

/* client interface association status */

/* client cp association status */

/* session statistics */
extern L7_char8 *pStrInfo_security_ClientBytesTx;
extern L7_char8 *pStrInfo_security_ClientPktsTx;
extern L7_char8 *pStrInfo_security_ClientBytesRx;
extern L7_char8 *pStrInfo_security_ClientPktsRx;
extern L7_char8 *pStrInfo_security_Statistics;

/* snmp traps */
extern L7_char8 *pStrInfo_security_Enable;
extern L7_char8 *pStrInfo_security_Disable;
extern L7_char8 *pStrInfo_security_CPTrapMode;
extern L7_char8 *pStrInfo_security_CPCltAuthFailureTrapMode;
extern L7_char8 *pStrInfo_security_CPCltConnectionTrapMode;
extern L7_char8 *pStrInfo_security_CPCltDBFullTrapMode;
extern L7_char8 *pStrInfo_security_CPCltDisconnectionTrapMode;
extern L7_char8 *pStrInfo_security_EnableCPTrapMode;
extern L7_char8 *pStrInfo_security_EnableCPClientAuthFailureTrap;
extern L7_char8 *pStrInfo_security_EnableCPClientConnectionTrap;
extern L7_char8 *pStrInfo_security_EnableCPClientDBFullTrap;
extern L7_char8 *pStrInfo_security_EnableCPClientDisconnectionTrap;

/* messages */
extern L7_char8 *pStrInfo_security_NoUsersExist;
extern L7_char8 *pStrInfo_security_UserEntries;
extern L7_char8* pStrInfo_security_ClientConnectionsAllDeauth;
extern L7_char8 *pStrInfo_security_UserEntriesAllDeleted;
extern L7_char8 *pStrInfo_security_SelectCheckBox;
extern L7_char8 *pStrInfo_security_DeleteUserFailed;
extern L7_char8 *pStrInfo_security_DeletedUsers;
extern L7_char8 *pStrInfo_security_DeletedUser;
extern L7_char8 *pStrInfo_security_AddedUserGroup;
extern L7_char8 *pStrInfo_security_FailedToAddUser;
extern L7_char8 *pStrInfo_security_FailedToAddUserGroup;
extern L7_char8 *pStrInfo_security_UserExists;
extern L7_char8 *pStrInfo_security_UserGroupInUse;
extern L7_char8 *pStrInfo_security_UserGroupExists;
extern L7_char8 *pStrInfo_security_FailedToDeauthClients;
extern L7_char8 *pStrInfo_security_FailedToDelUser;
extern L7_char8 *pStrInfo_security_FailedToDelUserGroup;
extern L7_char8 *pStrInfo_security_UserNameMustContainAlnumCharsOnly;
extern L7_char8 *pStrInfo_security_UserGroupMustContainAlnumCharsOnly;
extern L7_char8 *pStrInfo_security_UserNameLengthInvalid;
extern L7_char8 *pStrInfo_security_PasswordLengthInvalid;
extern L7_char8 *pStrInfo_security_CPNameMustContainAlnumCharsOnly;
extern L7_char8 *pStrInfo_security_NoInterfacesExist;
extern L7_char8 *pStrInfo_security_SelectCPConfig;
extern L7_char8 *pStrInfo_security_AssocIntfOneCPOnly;
extern L7_char8 *pStrInfo_security_IntfCPAlreadyAssociated;
extern L7_char8 *pStrInfo_security_NoCPClientsExist;
extern L7_char8 *pStrInfo_security_DeauthClientFailed;
extern L7_char8 *pStrInfo_security_DeauthClients;
extern L7_char8 *pStrInfo_security_Clients;
extern L7_char8 *pStrInfo_security_ClientsAllDeauth;
extern L7_char8 *pStrInfo_security_IntfNotAssociatedToCPConfig;
extern L7_char8 *pStrInfo_security_UnableToAddInterfacesToCP;
extern L7_char8 *pStrInfo_security_UnableToDelInterfacesFromCP;
extern L7_char8 *pStrInfo_security_UnableToSetAUP;
extern L7_char8 *pStrInfo_security_UnableToSetLanguage;
extern L7_char8 *pStrInfo_security_NoIntfAssocCPInstance;
extern L7_char8 *pStrInfo_security_NoCPClientsExistOnThisIntf;
extern L7_char8 *pStrInfo_security_NoCPClientsExistOnThisCP;
extern L7_char8 *pStrErr_security_FileTypeInvalidOrErrorProcessingFile;
extern L7_char8 *pStrErr_security_FileTransferPleaseVerifyInputFile;
/*extern L7_char8 *pStrInfo_security_CaptivePortal;*/

/* other */
extern L7_char8 *pStrInfo_security_JsdataUser;
extern L7_char8 *pStrInfo_security_Group;
extern L7_char8 *pStrInfo_security_GroupError;
extern L7_char8 *pStrInfo_security_Session;
extern L7_char8 *pStrInfo_security_Session_Blank;
extern L7_char8 *pStrInfo_security_SessionError;
extern L7_char8 *pStrInfo_security_Idle;
extern L7_char8 *pStrInfo_security_Idle_Blank;
extern L7_char8 *pStrInfo_security_IdleError;
extern L7_char8 *pStrInfo_security_JsdataPeerClient;
extern L7_char8 *pStrInfo_security_JsdataClientMAC;
extern L7_char8 *pStrInfo_security_JsdataClientMAC2;
extern L7_char8 *pStrInfo_security_ClientIP;
extern L7_char8 *pStrInfo_security_ClientIPError;
extern L7_char8 *pStrInfo_security_ClientIntf;
extern L7_char8 *pStrInfo_security_ClientIntfError;
extern L7_char8 *pStrInfo_security_ClientCpid;
extern L7_char8 *pStrInfo_security_ClientCpidError;
extern L7_char8 *pStrInfo_security_SwMAC;
extern L7_char8 *pStrInfo_security_SwMACError;
extern L7_char8 *pStrInfo_security_SwIP;
extern L7_char8 *pStrInfo_security_SwIPError;
extern L7_char8 *pStrInfo_security_Protocol_1;
extern L7_char8 *pStrInfo_security_ProtocolError;
extern L7_char8 *pStrInfo_security_User;
extern L7_char8 *pStrInfo_security_UserError;
extern L7_char8 *pStrInfo_security_Verification_1;
extern L7_char8 *pStrInfo_security_VerificationError;
extern L7_char8 *pStrInfo_security_Comma;
extern L7_char8 *pStrInfo_security_FieldInvalidFmt;
extern L7_char8 *pStrInfo_security_RadiusNamedStatus; 
extern L7_char8 *pStrInfo_security_RadiusNamedAcctStatus; 
extern L7_char8 *pStrInfo_security_NoRadNamedServersExist; 
extern L7_char8 *pStrInfo_security_NoRadNamedAcctServersExist; 
extern L7_char8 *pStrInfo_security_HtmlFileRadiusNamedAcctSrvr;
extern L7_char8 *pStrInfo_security_HtmlFileRadiusNamedSrvr; 
extern L7_char8 *pStrInfo_security_RadiusAuthAcctSrvrExists;
extern L7_char8 *pStrInfo_security_AddRadiusAcctSrvr;
extern L7_char8 *pStrInfo_security_SrvrType; 
extern L7_char8 *pStrInfo_security_PortNum_1;
extern L7_char8 *pStrInfo_security_Current1;



/* Supplicant strings are placed here */
extern L7_char8 *pStrInfo_security_PaeSupplicant;
extern L7_char8 *pStrInfo_security_PaeAuthenticator;

#endif

