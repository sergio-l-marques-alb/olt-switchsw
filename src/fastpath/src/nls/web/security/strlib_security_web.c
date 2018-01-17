/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   src/nls/web/security/strlib_security_web.c
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

#include "strlib_security_web.h"

L7_char8 *pStrInfo_security_AccessAccepts = "Access Accepts";
L7_char8 *pStrInfo_security_AccessChallenges = "Access Challenges";
L7_char8 *pStrInfo_security_AccessRetransmissions = "Access Retransmissions";
L7_char8 *pStrInfo_security_AcctMode = "Accounting Mode";
L7_char8 *pStrInfo_security_AcctReqs = "Accounting Requests";
L7_char8 *pStrInfo_security_AcctResps = "Accounting Responses";
L7_char8 *pStrInfo_security_AcctRetransmissions = "Accounting Retransmissions";
L7_char8 *pStrInfo_security_AcctSrvrHostAddr = "Accounting Server Host Address";
L7_char8 *pStrInfo_security_BackendState = "Backend State";
L7_char8 *pStrInfo_security_BothShldNotBeSimultaneouslyDsbld = "Both these should not be simultaneously disabled.";
L7_char8 *pStrInfo_security_Certificate = "Certificate";
L7_char8 *pStrInfo_security_ClrAllRadiusStats = "Clear All RADIUS Statistics";
L7_char8 *pStrInfo_security_ConnTimeout = "Connection Timeout";
L7_char8 *pStrInfo_security_Current = "Current";
L7_char8 *pStrInfo_security_DownloadCertificates = "Download Certificates";
L7_char8 *pStrInfo_security_DownloadHostKeys = "Download Host Keys";
L7_char8 *pStrInfo_security_EapReqIdFramesTxed = "EAP Request/ID Frames Transmitted";
L7_char8 *pStrInfo_security_EapRespIdFramesRcvd = "EAP Response/ID Frames Received";
L7_char8 *pStrInfo_security_FailedToGenerateCertificate = "Failed to Generate Certificate.";
L7_char8 *pStrErr_security_FailedToGetRadiusSrvrIpAddrSpecified = "Failed to get radius server ip address specified.";
L7_char8 *pStrInfo_security_GuestVlanSupplicantMode = "Guest VLAN Supplicant Mode";
L7_char8 *pStrInfo_security_HttpsAdminMode = "HTTPS Admin Mode";
L7_char8 *pStrInfo_security_HttpsHardTimeout = "HTTPS Hard Timeout";
L7_char8 *pStrInfo_security_HttpsPort = "HTTPS Port";
L7_char8 *pStrInfo_security_HttpsSoftTimeout = "HTTPS Soft Timeout";
L7_char8 *pStrInfo_security_KeyString_1 = "Key String";
L7_char8 *pStrInfo_security_LenOfKeyStringIsGreaterThan128Chars = "Length of Key String is greater than 128 characters";
L7_char8 *pStrInfo_security_MalformedAcctResps = "Malformed Accounting Responses";
L7_char8 *pStrInfo_security_MaxNumOfReTxs = "Max Number of Retransmits";
L7_char8 *pStrInfo_security_MaxSshSession = "Max SSH Session";
L7_char8 *pStrInfo_security_MaxNumOfHttpsSessions = "Maximum Number of HTTPS Sessions";
/*L7_char8 *pStrInfo_security_MsgAuthenticator = "Message Authenticator";*/
L7_char8 *pStrInfo_security_OperatingCntrlMode = "Operating Control Mode";
L7_char8 *pStrInfo_security_PktsDropped = "Packets Dropped";
L7_char8 *pStrInfo_security_PendingReqs = "Pending Requests";
L7_char8 *pStrInfo_security_PortAccessCntrlCfg = "Port Access Control Configuration";
L7_char8 *pStrInfo_security_PortAccessCntrlPortCfg = "Port Access Control Port Configuration";
L7_char8 *pStrInfo_security_PortAccessCntrlSupplicantPortCfg = "Port Access Control Port Configuration";
L7_char8 *pStrInfo_security_PortAccessCntrlPortSummary = "Port Access Control Port Summary";
L7_char8 *pStrInfo_security_PortAccessClientSummary = "Port Access Client Summary";
L7_char8 *pStrInfo_security_PortAccessClientDetail = "Port Access Client Detail";
L7_char8 *pStrInfo_security_PortAccessCntrlStats = "Port Access Control Statistics";
L7_char8 *pStrInfo_security_SupplicantPortAccessCntrlStats = "Supplicant Port Access Control Statistics";
L7_char8 *pStrInfo_security_PortAccessCntrlStatus = "Port Access Control Status";
L7_char8 *pStrInfo_security_PortAccessCntrlUsrLoginCfg = "Port Access Control User Login Configuration";
L7_char8 *pStrInfo_security_PortAccessPrivileges = "Port Access Privileges";
L7_char8 *pStrInfo_security_PortAccessSummary = "Port Access Summary";
L7_char8 *pStrInfo_security_PortStatus = "Port Status";
L7_char8 *pStrInfo_security_PrimarySrvr = "Primary Server";
L7_char8 *pStrInfo_security_ProtoLvl = "Protocol Level";
L7_char8 *pStrInfo_security_RadiusAcctSrvrCfg = "RADIUS Accounting Server Configuration";
L7_char8 *pStrInfo_security_RadiusAcctSrvrStats = "RADIUS Accounting Server Statistics";
L7_char8 *pStrInfo_security_RadiusClrStats = "RADIUS Clear Statistics";
L7_char8 *pStrInfo_security_RadiusCfg = "RADIUS Configuration";
L7_char8 *pStrInfo_security_RadiusSrvrCfg = "RADIUS Server Configuration";
L7_char8 *pStrInfo_security_RadiusSrvrStats = "RADIUS Server Statistics";
L7_char8 *pStrInfo_security_RadiusAttr4NasIpAddr = "RADIUS Attribute 4 (NAS-IP Address)";
L7_char8 *pStrInfo_security_Reauthenticate = "Reauthenticate";
L7_char8 *pStrInfo_security_RoundTripTimeSecs = "Round Trip Time (secs)";
L7_char8 *pStrInfo_security_SshSessionIdleTimeout = "SSH Session idle timeout";
L7_char8 *pStrInfo_security_SslVer3Mode = "SSL Version 3 Mode";
L7_char8 *pStrInfo_security_Secret = "Secret";
L7_char8 *pStrInfo_security_SecureHttpCfg = "Secure HTTP Configuration";
L7_char8 *pStrInfo_security_SecureShellCfg = "Secure Shell Configuration";
L7_char8 *pStrInfo_security_Supplicant_1 = "Supplicant";
L7_char8 *pStrInfo_security_TacacsCfg = "TACACS+ Configuration";
L7_char8 *pStrInfo_security_TacacsSrvr = "TACACS+ Server";
L7_char8 *pStrInfo_security_TacacsSrvrAddr = "Server Address";
L7_char8 *pStrInfo_security_TacacsSrvrCfg = "TACACS+ Server Configuration";
L7_char8 *pStrInfo_security_TlsVer1Mode = "TLS Version 1 Mode";
L7_char8 *pStrInfo_security_TlsVer1ModeAndSslVer3Mode = "TLS Version 1 Mode and SSL Version 3 Mode";
L7_char8 *pStrInfo_security_TimeoutSecs = "Timeout (secs)";
L7_char8 *pStrInfo_security_TimeoutDurationSecs = "Timeout Duration (secs)";
L7_char8 *pStrInfo_security_VlanAssignmentMode = "VLAN Assignment Mode";
L7_char8 *pStrInfo_security_Ver1OrVer2MustBeEnbld = "Version 1 or Version 2 must be enabled";
L7_char8 *pStrInfo_security_AddRadiusSrvr = "RADIUS server";
L7_char8 *pStrInfo_security_HtmlFileDot1xAplCfg = "dot1x_apl_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileDot1xCfg = "dot1x_config.html";
L7_char8 *pStrInfo_security_HtmlFileDot1xPortCfg = "dot1x_port_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileDot1xSuppPortCfg = "dot1x_supp_port_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileDot1xSupplicantPortCfg = "dot1x_supplicant_port_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileDot1xPortSumm = "dot1x_port_summ.html";
L7_char8 *pStrInfo_security_HtmlFileDot1xStats = "dot1x_stats.html";
L7_char8 *pStrInfo_security_HtmlFileDot1xStatus = "dot1x_status.html";
L7_char8 *pStrInfo_security_HtmlFileDot1xSumm = "dot1x_summ.html";
L7_char8 *pStrInfo_security_HtmlFileDot1xUsrPorts = "dot1x_user_ports.html";
L7_char8 *pStrInfo_security_HtmlFileDot1xClientSumm = "dot1x_client_summ.html";
L7_char8 *pStrInfo_security_False_1 = "false";
L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xcfg = "href=\"/security/dot1x/help_dot1x.html#dot1xcfg\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xloginconfig = "href=\"/security/dot1x/help_dot1x.html#dot1xloginconfig\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportaccesscfg = "href=\"/security/dot1x/help_dot1x.html#dot1xportaccesscfg\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportaccesssummary = "href=\"/security/dot1x/help_dot1x.html#dot1xportaccesssummary\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportcfg = "href=\"/security/dot1x/help_dot1x.html#dot1xportcfg\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xportstatus = "href=\"/security/dot1x/help_dot1x.html#dot1xportstatus\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xClientSummary = "href=\"/security/dot1x/help_dot1x.html#dot1xclientSummary\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xClientDetail = "href=\"/security/dot1x/help_dot1x.html#dot1xclientDetail\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xstats = "href=\"/security/dot1x/help_dot1x.html#dot1xstats\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityDot1xHelpDot1xDot1xstatus = "href=\"/security/dot1x/help_dot1x.html#dot1xstatus\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusAcctcfg = "href=\"/security/radius/help/radius_acct_config.html\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadacctstats = "href=\"/security/radius/help/radius_acct_stats.html\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadcfg = "href=\"/security/radius/help/radius_configuration.html\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadclearstats = "href=\"/security/radius/help/radius_clear.html\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadservcfg = "href=\"/security/radius/help/radius_server_config.html\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusRadservstats = "href=\"/security/radius/help/radius_stats.html\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusStatus = "href=\"/security/radius/help/radius_status.html\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityRadiusHelpRadiusAcctStatus = "href=\"/security/radius/help/radius_acct_status.html\"";
L7_char8 *pStrInfo_security_HtmlFileSecuritySshHelpSsh = "href=\"/security/ssh/help_ssh.html\"";
L7_char8 *pStrInfo_security_HtmlFileSecuritySslHelpHttps = "href=\"/security/ssl/help_https.html\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityTacacsHelpTacacsTacacscfg = "href=\"/security/tacacs/help_tacacs.html#tacacscfg\"";
L7_char8 *pStrInfo_security_HtmlLinkSecurityTacacsHelpTacacsTacacsservercfg = "href=\"/security/tacacs/help_tacacs.html#tacacsservercfg\"";
L7_char8 *pStrInfo_security_HtmlFileRadiusAcctSrvr = "radius_acct_server.html";
L7_char8 *pStrInfo_security_HtmlFileRadiusAcctStats = "radius_acct_stats.html";
L7_char8 *pStrInfo_security_HtmlFileRadiusClr = "radius_clear.html";
L7_char8 *pStrInfo_security_HtmlFileRadiusCfg = "radius_config.html";
L7_char8 *pStrInfo_security_HtmlFileRadiusServStats = "radius_serv_stats.html";
L7_char8 *pStrInfo_security_HtmlFileRadiusSrvr = "radius_server.html";
L7_char8 *pStrInfo_security_RemoveAcctSrvr = "remove accounting server";
L7_char8 *pStrInfo_security_RemoveRadiusSrvr = "remove radius server";
L7_char8 *pStrInfo_security_SetBridgePri = "set bridge priority";
L7_char8 *pStrInfo_security_HtmlFileSshCfg = "ssh_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileSslCfg = "ssl_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileTacacsCfg = "tacacs_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileTacacsSrvrCfgRo = "tacacs_server_cfg_ro.html";
L7_char8 *pStrInfo_security_HtmlFileTacacsSrvrCfgRw = "tacacs_server_cfg_rw.html";
L7_char8 *pStrInfo_security_True_1 = "true";
L7_char8 *pStrInfo_security_WhileTryingToInitializePort = "while trying to initialize port";
L7_char8 *pStrInfo_security_WhileTryingToReauthenticatePort = "while trying to reauthenticate port";
L7_char8 *pStrInfo_security_MacBasedDot1xAuth = "MAC Based";
L7_char8 *pStrInfo_security_HtmlFileDot1xClientsDetail = "dot1x_client_detail.html";
/*------------------------------------------------------------------*/
/*             CAPTIVE PORTAL STRINGS                               */
/*------------------------------------------------------------------*/

/* html files */
L7_char8 *pStrInfo_security_HtmlFileCPGlobalCfg = "cp_global_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileCPCfg = "cp_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileCPCfgMgmt = "cp_cfg_mgmt.html";
L7_char8 *pStrInfo_security_HtmlFileCPWebCustom = "cp_web_custom.html";
L7_char8 *pStrInfo_security_HtmlFileCPUserLocal = "cp_user_local.html";
L7_char8 *pStrInfo_security_HtmlFileCPUserLocalCfg = "cp_user_local_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileCPIntfAssocCfg = "cp_intf_assoc_cfg.html";
L7_char8 *pStrInfo_security_HtmlFileCPGlobalStatus = "cp_global_status.html";
L7_char8 *pStrInfo_security_HtmlFileCPStatus = "cp_status.html";
L7_char8 *pStrInfo_security_HtmlFileCPIntfStatus = "cp_intf_status.html";
L7_char8 *pStrInfo_security_HtmlFileCPIntfCapStatus = "cp_intf_capability_status.html";
L7_char8 *pStrInfo_security_HtmlFileCPClients = "cp_clients.html";
L7_char8 *pStrInfo_security_HtmlFileCPClientStatus = "cp_client_status.html";
L7_char8 *pStrInfo_security_HtmlFileCPClientIntfStatus = "cp_client_intf_status.html";
L7_char8 *pStrInfo_security_HtmlFileCPClientCPStatus = "cp_client_cp_status.html";
L7_char8 *pStrInfo_security_HtmlFileCPSessionStats = "cp_session_statistics.html";
L7_char8 *pStrInfo_security_HtmlFileCPTrapCfg = "cp_trap_cfg.html";

/* page titles */
L7_char8 *pStrInfo_security_TitleCPGlobalCfg = "Global Configuration";
L7_char8 *pStrInfo_security_TitleCPCfg = "CP Summary";
L7_char8 *pStrInfo_security_TitleCPCfgMgmt = "CP Configuration";
L7_char8 *pStrInfo_security_TitleCPWebCustom = "CP WEB Customization";
L7_char8 *pStrInfo_security_TitleCPUserLocal = "Local User Summary";
L7_char8 *pStrInfo_security_TitleCPUserLocalCfg = "Local User Configuration";
L7_char8 *pStrInfo_security_TitleCPIntfAssocCfg = "Interface Association";
L7_char8 *pStrInfo_security_TitleCPGlobalStatus = "Global Status";
L7_char8 *pStrInfo_security_TitleCPStatus = "CP Activation and Activity Status";
L7_char8 *pStrInfo_security_TitleCPIntfStatus = "Interface Activation Status";
L7_char8 *pStrInfo_security_TitleCPIntfCapStatus = "Interface Capability Status";
L7_char8 *pStrInfo_security_TitleCPClients = "Client Summary";
L7_char8 *pStrInfo_security_TitleCPClientStatus = "Client Detail";
L7_char8 *pStrInfo_security_TitleCPSessionStats = "Client Statistics";
L7_char8 *pStrInfo_security_TitleCPClientIntfStatus = "Interface - Client Status";
L7_char8 *pStrInfo_security_TitleCPClientCPStatus = "CP - Client Status";
L7_char8 *pStrInfo_security_TitleCPTrapCfg = "SNMP Trap Configuration";
L7_char8 *pStrInfo_security_TitlePAECfg = "PAE Capability Configuration";
L7_char8 *pStrInfo_security_TitleSuppCfg = "Supplicant Port Configuration";
 
/* help links */
L7_char8 *pStrInfo_security_HtmlLinkHelpCPGlobalCfg = "href=\"help_cp_global_config.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPCfg = "href=\"help_cp_summary.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPCfgMgmt = "href=\"help_cp_config.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPWebCustom = "href=\"help_cp_web_custom.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPUserLocal = "href=\"help_cp_user_local.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPUserLocalAdd = "href=\"help_cp_add_local.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPUserLocalCfg = "href=\"help_cp_user_local_config.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPIntfAssocCfg = "href=\"help_cp_if_assoc.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPGlobalStatus = "href=\"help_cp_global_status.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPStatus = "href=\"help_cp_act_status.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPIntfStatus = "href=\"help_cp_if_act_status.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPIntfCapStatus = "href=\"help_cp_if_cap_status.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPClients = "href=\"help_cp_client_summary.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPClientStatus = "href=\"help_cp_client_detail.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPSessionStats = "href=\"help_cp_client_stats.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPClientIntfStatus = "href=\"help_cp_client_if_status.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPClientCPStatus = "href=\"help_cp_client_assoc_status.html\"";
L7_char8 *pStrInfo_security_HtmlLinkHelpCPTrapCfg = "href=\"help_cp_snmp_trap_config.html\"";

/* global config */
L7_char8 *pStrInfo_security_EnableCP = "Enable Captive Portal";
L7_char8 *pStrInfo_security_HTTPPort = "Additional HTTP Port";
L7_char8 *pStrInfo_security_HTTPSecurePort = "Additional HTTP Secure Port";
L7_char8 *pStrInfo_security_PeerSwStatReportInterval = "Peer Switch Statistics Reporting Interval (secs)";
L7_char8 *pStrInfo_security_AuthSessionTimeout = "Authentication Timeout (secs)";

/* cp config */
L7_char8 *pStrInfo_security_Configuration = "Configuration";
L7_char8 *pStrInfo_security_CPName = "CP Name";
L7_char8 *pStrInfo_security_Mode = "Mode";
L7_char8 *pStrInfo_security_Protocol = "Protocol";
L7_char8 *pStrInfo_security_Verification = "Verification";
L7_char8 *pStrInfo_security_Language = "Language";
L7_char8 *pStrInfo_security_Languages = "Languages";
L7_char8 *pStrInfo_security_ProtocolHTTP = "HTTP";
L7_char8 *pStrInfo_security_ProtocolHTTPS = "HTTPS";
L7_char8 *pStrInfo_security_VerificationModeGuest = "Guest";
L7_char8 *pStrInfo_security_VerificationModeLocal = "Local";
L7_char8 *pStrInfo_security_VerificationModeRadius = "RADIUS";
L7_char8 *pStrInfo_security_ConfigurationName = "Configuration Name";
L7_char8 *pStrInfo_security_ProtocolMode = "Protocol Mode";
L7_char8 *pStrInfo_security_VerificationMode = "Verification Mode";
L7_char8 *pStrInfo_security_CfgURLRedirectMode = "Enable Redirect Mode";
L7_char8 *pStrInfo_security_CfgURL = "Redirect URL";
L7_char8 *pStrInfo_security_RadiusAccounting = "RADIUS Accounting";
L7_char8 *pStrInfo_security_RadiusAuthServer = "RADIUS Auth Server";
L7_char8 *pStrInfo_security_UserDownRate = "n/a";
L7_char8 *pStrInfo_security_IntrusionThreshold = "Intrusion Threshold (secs)";
L7_char8 *pStrInfo_security_AcceptanceUsePolicy = "Acceptance Use Policy";
L7_char8 *pStrErr_security_RadiusSrvrMustBeAlphaNumericPlus = "RADIUS server name may only contain alphanumeric plus “-“, “_”, and space characters.";
L7_char8 *pStrInfo_security_UserLogoutMode = "User Logout Mode";
L7_char8 *pStrInfo_security_ConfiguredLocalUsers = "Configured Local Users";
/* cp web customization */

/* user local */
L7_char8 *pStrInfo_security_UserList = "User List";
L7_char8 *pStrInfo_security_UserName = "User Name";
L7_char8 *pStrInfo_security_Password = "Password";
L7_char8 *pStrInfo_security_PasswordEncrypted = "******";
L7_char8 *pStrInfo_security_UserGroup = "User Group";
L7_char8 *pStrInfo_security_SessionTimeoutSecs = "Session Timeout (secs)";
L7_char8 *pStrInfo_security_IdleTimeout = "Idle Timeout (secs)";
L7_char8 *pStrInfo_security_MaxBwUp = "Max Up Rate (bytes/sec)";
L7_char8 *pStrInfo_security_MaxBwDown = "Max Down Rate (bytes/sec)";
L7_char8 *pStrInfo_security_MaxInputOctets = "Max Receive (bytes)";
L7_char8 *pStrInfo_security_MaxOutputOctets = "Max Transmit (bytes)";
L7_char8 *pStrInfo_security_MaxTotalOctets = "Max Total (bytes)";
L7_char8 *pStrInfo_security_BrowserTitle = "Browser Title";
L7_char8 *pStrInfo_security_PageTitle = "Page Title";
L7_char8 *pStrInfo_security_AccountLabel = "Account title";
L7_char8 *pStrInfo_security_UserLabel = "User label";
L7_char8 *pStrInfo_security_PasswordLabel = "Password label";
L7_char8 *pStrInfo_security_ButtonLabel = "Button label";
L7_char8 *pStrInfo_security_AUPText = "Acceptance Use Policy";
L7_char8 *pStrInfo_security_AcceptText = "Acceptance acknowledgement text";
L7_char8 *pStrInfo_security_InstructionalText = "Instructional text";
L7_char8 *pStrInfo_security_DeniedMsg = "Denied message";
L7_char8 *pStrInfo_security_ResourceMsg = "Resource message";
L7_char8 *pStrInfo_security_TimeoutMsg = "Timeout message";
L7_char8 *pStrInfo_security_NoAcceptMsg = "No Accept message";
L7_char8 *pStrInfo_security_WIPMsg = "Busy message";
L7_char8 *pStrInfo_security_WelcomeTitle = "Welcome title";
L7_char8 *pStrInfo_security_WelcomeText = "Welcome text";
L7_char8 *pStrInfo_security_ScriptText = "No script text";
L7_char8 *pStrInfo_security_PopupText = "No popup text";
L7_char8 *pStrInfo_security_LogoutBrowserTitleText = "Logout browser title";
L7_char8 *pStrInfo_security_LogoutPageTitleText = "Logout title";
L7_char8 *pStrInfo_security_LogoutInstructText = "Logout content text";
L7_char8 *pStrInfo_security_LogoutButtonLabel = "Logout button label";
L7_char8 *pStrInfo_security_LogoutConfirmText = "Logout Confirmation text";
L7_char8 *pStrInfo_security_LogoutSuccessBrowserTitleText = "Logout success browser title";
L7_char8 *pStrInfo_security_LogoutSuccessPageTitleText = "Logout success title";
L7_char8 *pStrInfo_security_LogoutSuccessInstructText = "Logout success content text";
L7_char8 *pStrInfo_security_MaxNumOfChars = "Maximum number of characters allowed is %d.\r\n";

/* interface association */
L7_char8 *pStrInfo_security_Interface = "Interface";
L7_char8 *pStrInfo_security_InterfaceList = "Interface List";
L7_char8 *pStrInfo_security_AssocInterfaces = "Associated Interfaces";
L7_char8 *pStrInfo_security_CPConfig = "CP Configuration";
L7_char8 *pStrInfo_security_Physical = "Physical";

/* cp global status */
L7_char8 *pStrInfo_security_CPGlobalOperStatus = "CP Global Operational Status";
L7_char8 *pStrInfo_security_CPGlobalDisableReason = "CP Global Disable Reason";
L7_char8 *pStrInfo_security_CPIPAddress = "CP IP Address";
L7_char8 *pStrInfo_security_SupportedCPs = "Supported Captive Portals";
L7_char8 *pStrInfo_security_ConfiguredCPs = "Configured Captive Portals";
L7_char8 *pStrInfo_security_ActiveCPs = "Active Captive Portals";
L7_char8 *pStrInfo_security_SupportedUsersInSystem = "System Supported Users";
L7_char8 *pStrInfo_security_LocalUsersInSystem = "Supported Local Users";
L7_char8 *pStrInfo_security_AuthenticatedUsers = "Authenticated Users";
L7_char8 *pStrInfo_security_AuthFailureTableSize = "Authorization Failure Table Size";
L7_char8 *pStrInfo_security_UsersInAuthFailureTable = "Authorization Failed Users";
L7_char8 *pStrInfo_security_ActivityLogTableSize = "Activity Log Table Size";
L7_char8 *pStrInfo_security_ActivityEntriesInLog = "Activity Entries In Log";
L7_char8 *pStrInfo_security_CPModeEnablePending = "Enable - Pending";
L7_char8 *pStrInfo_security_CPModeDisablePending = "Disable - Pending";
L7_char8 *pStrInfo_security_CPModeReasonAdmin = "Administrator Disabled";
L7_char8 *pStrInfo_security_CPModeReasonNoIpAddr = "IP Address Not Configured";
L7_char8 *pStrInfo_security_CPModeReasonRoutingNoIpRoutingIntf = "No IP Routing Interface";
L7_char8 *pStrInfo_security_CPModeReasonRoutingDisabled = "Routing Disabled";

/* cp activation/activity status */
L7_char8 *pStrInfo_security_OperStatus = "Operational Status";
L7_char8 *pStrInfo_security_ActStatus = "Activation Status";
L7_char8 *pStrInfo_security_DisableReason = "Disable Reason";
L7_char8 *pStrInfo_security_BlockedStatus = "Blocked Status";
L7_char8 *pStrInfo_security_NoRadiusServer = "No RADIUS Server";
L7_char8 *pStrInfo_security_NoAcctServer = "No Accounting Server";
L7_char8 *pStrInfo_security_NoAssocWithIntf = "Not Associated With Interfaces";
L7_char8 *pStrInfo_security_NoValidActiveIntf = "No Valid Active Interfaces";
L7_char8 *pStrInfo_security_NoValidCert = "No HTTPS Certificate";
L7_char8 *pStrInfo_security_Blocked = "Blocked";
L7_char8 *pStrInfo_security_NotBlocked = "Not Blocked";
L7_char8 *pStrInfo_security_BlockedPending = "Blocking";
L7_char8 *pStrInfo_security_NotBlockedPending = "Unblocking";

/* interface activation status */
L7_char8 *pStrInfo_security_IntfNotAttached = "Interface Not Attached";

/* interface capability status */
L7_char8 *pStrInfo_security_CPMode = "CP Mode";
L7_char8 *pStrInfo_security_RoamingSupport = "Roaming Support";
L7_char8 *pStrInfo_security_MaxInputOctetsMonitor = "Max Input Octets Monitor";
L7_char8 *pStrInfo_security_MaxOutputOctetsMonitor = "Max Output Octets Monitor";
L7_char8 *pStrInfo_security_BytesRxCounter = "Bytes Received Counter";
L7_char8 *pStrInfo_security_BytesTxCounter = "Bytes Transmitted Counter";
L7_char8 *pStrInfo_security_PktsRxCounter = "Packets Received Counter";
L7_char8 *pStrInfo_security_PktsTxCounter = "Packets Transmitted Counter";
L7_char8 *pStrInfo_security_Session_Timeout = "Session Timeout";
L7_char8 *pStrInfo_security_Idle_Timeout = "Idle Timeout";

/* client status */
L7_char8 *pStrInfo_security_ClientIPAddr = "Client IP Address";
L7_char8 *pStrInfo_security_SwitchMACAddr = "Switch MAC Address";
L7_char8 *pStrInfo_security_SwitchIPAddr = "Switch IP Address";
L7_char8 *pStrInfo_security_SwitchType = "Switch Type";
L7_char8 *pStrInfo_security_SessionTime = "Session Time";
L7_char8 *pStrInfo_security_TimeSinceLastStatReport = "Time Since Last Statistics Report";
L7_char8 *pStrInfo_security_Local = "Local";
L7_char8 *pStrInfo_security_Peer = "Peer";

/* client interface association status */

/* client cp association status */

/* session statistics */
L7_char8 *pStrInfo_security_ClientBytesRx = "Bytes Received";
L7_char8 *pStrInfo_security_ClientBytesTx = "Bytes Transmitted";
L7_char8 *pStrInfo_security_ClientPktsRx = "Packets Received";
L7_char8 *pStrInfo_security_ClientPktsTx = "Packets Transmitted";
L7_char8 *pStrInfo_security_Statistics = "Session Statistics";

/* snmp traps */
L7_char8 *pStrInfo_security_Enable = "Enable";
L7_char8 *pStrInfo_security_Disable = "Disable";
L7_char8 *pStrInfo_security_CPTrapMode = "Captive Portal Trap Mode";
L7_char8 *pStrInfo_security_CPCltAuthFailureTrapMode = "Client Authentication Failure Traps";
L7_char8 *pStrInfo_security_CPCltConnectionTrapMode = "Client Connection Traps";
L7_char8 *pStrInfo_security_CPCltDBFullTrapMode = "Client Database Full Traps";
L7_char8 *pStrInfo_security_CPCltDisconnectionTrapMode = "Client Disconnection Traps";
L7_char8 *pStrInfo_security_EnableCPTrapMode = "enable Captive Portal trap mode.";
L7_char8 *pStrInfo_security_EnableCPClientAuthFailureTrap = "enable client authentication failure trap mode.";
L7_char8 *pStrInfo_security_EnableCPClientConnectionTrap = "enable client connection trap mode.";
L7_char8 *pStrInfo_security_EnableCPClientDBFullTrap = "enable client database full trap mode.";
L7_char8 *pStrInfo_security_EnableCPClientDisconnectionTrap = "enable client disconnection trap mode.";

/* messages */
L7_char8 *pStrInfo_security_NoUsersExist = "No users exist.";
L7_char8 *pStrInfo_security_UserEntries = "user entries.";
L7_char8 *pStrInfo_security_ClientConnectionsAllDeauth = "Successfully deauthenticated all clients for this CP configuration.";
L7_char8 *pStrInfo_security_UserEntriesAllDeleted = "Successfully deleted all user entries.";
L7_char8 *pStrInfo_security_SelectCheckBox = "Please make a selection by clicking on a check box.";
L7_char8 *pStrInfo_security_DeleteUserFailed = "delete the user.";
L7_char8 *pStrInfo_security_DeletedUsers = "Successfully deleted following users:";
L7_char8 *pStrInfo_security_DeletedUser = "Successfully deleted user ";
L7_char8 *pStrInfo_security_AddedUserGroup = "Successfully added user group %s ";
L7_char8 *pStrInfo_security_FailedToAddUser = "Failed to add user %s.";
L7_char8 *pStrInfo_security_FailedToAddUserGroup = "Failed to add user group %s.";
L7_char8 *pStrInfo_security_UserExists = "User %s already exists.";
L7_char8 *pStrInfo_security_UserGroupInUse = "This user group is currently in use!";
L7_char8 *pStrInfo_security_UserGroupExists = "User group %s already exists.\r\n";
L7_char8 *pStrInfo_security_FailedToDelUser = "Failed to delete user %s.";
L7_char8 *pStrInfo_security_FailedToDeauthClients = "Failed to deauthenticate clients.";
L7_char8 *pStrInfo_security_FailedToDelUserGroup = "Failed to delete user group %s.";
L7_char8 *pStrInfo_security_UserNameMustContainAlnumCharsOnly = "User name must contain alphaNumeric characters only.";
L7_char8 *pStrInfo_security_UserGroupMustContainAlnumCharsOnly = "User group must contain alphaNumeric characters only.";
L7_char8 *pStrInfo_security_UserNameLengthInvalid = "User name must be %d to %d characters long.";
L7_char8 *pStrInfo_security_PasswordLengthInvalid = "Password must be %d to %d characters long.";
L7_char8 *pStrInfo_security_CPNameMustContainAlnumCharsOnly = "CP Name must contain alphaNumeric characters only.";
L7_char8 *pStrInfo_security_NoInterfacesExist = "No Interfaces exist.";
L7_char8 *pStrInfo_security_SelectCPConfig = "Select a CP Configuration.";
L7_char8 *pStrInfo_security_AssocIntfOneCPOnly = "An interface can be associated to only one CP Configuration.";
L7_char8 *pStrInfo_security_IntfCPAlreadyAssociated = " is associated to CP Configuration ";
L7_char8 *pStrInfo_security_NoCPClientsExist = "No Captive Portal clients exist.";
L7_char8 *pStrInfo_security_DeauthClientFailed = "deauthenticate the client.";
L7_char8 *pStrInfo_security_DeauthClients = "Successfully deauthenticated following clients:";
L7_char8 *pStrInfo_security_Clients = "clients.";
L7_char8 *pStrInfo_security_ClientsAllDeauth = "Successfully deauthenticated all clients.";
L7_char8 *pStrInfo_security_IntfNotAssociatedToCPConfig = "Interface (%s) is not associated to CP Configuration %u.";
L7_char8 *pStrInfo_security_UnableToAddInterfacesToCP = "Error! Unable to associate the following interfaces:";
L7_char8 *pStrInfo_security_UnableToDelInterfacesFromCP = "Error! Following interfaces are not associated to CP Configuration %u:";
L7_char8 *pStrInfo_security_UnableToSetAUP = "Error! Unable to set Acceptance Use policy for the following language code(s):\r\n";
L7_char8 *pStrInfo_security_UnableToSetLanguage = "Failed to set language code %s.";
L7_char8 *pStrInfo_security_NoIntfAssocCPInstance = "No interfaces associated to this CP configuration.";
L7_char8 *pStrInfo_security_NoCPClientsExistOnThisIntf = "No Captive Portal clients connected on this interface.";
L7_char8 *pStrInfo_security_NoCPClientsExistOnThisCP = "No Captive Portal clients connected to this CP configuration.";
L7_char8 *pStrErr_security_FileTypeInvalidOrErrorProcessingFile = "File type invalid or error processing file.";
L7_char8 *pStrErr_security_FileTransferPleaseVerifyInputFile = "Invalid file transfer! Please verify input file.";
/*L7_char8 *pStrInfo_security_CaptivePortal = "Captive Portal";*/

/* other */
L7_char8 *pStrInfo_security_JsdataUser = "jsData[%d] = {user:\"%s\",";
L7_char8 *pStrInfo_security_Group = "group:\"%s\",";
L7_char8 *pStrInfo_security_GroupError = "group:\"error\",";
L7_char8 *pStrInfo_security_Session = "sessionTimeout:\"%u\",";
L7_char8 *pStrInfo_security_Session_Blank = "sessionTimeout:\"%s\",";
L7_char8 *pStrInfo_security_SessionError = "sessionTimeout:\"error\",";
L7_char8 *pStrInfo_security_Idle = "idleTimeout:\"%u\"}";
L7_char8 *pStrInfo_security_Idle_Blank = "idleTimeout:\"%s\"}";
L7_char8 *pStrInfo_security_IdleError = "idleTimeout:\"error\"}";
L7_char8 *pStrInfo_security_JsdataPeerClient = "jsData[%d] = {peerClient:\"%s\",";
L7_char8 *pStrInfo_security_JsdataClientMAC = "jsData[%d] = {mac:\"%02x:%02x:%02x:%02x:%02x:%02x\",";
L7_char8 *pStrInfo_security_JsdataClientMAC2 = "mac:\"%02x:%02x:%02x:%02x:%02x:%02x\",";
L7_char8 *pStrInfo_security_ClientIP = "ip:\"%s\",";
L7_char8 *pStrInfo_security_ClientIPError = "ip:\"error\",";
L7_char8 *pStrInfo_security_ClientIntf = "intf:\"%s\",";
L7_char8 *pStrInfo_security_ClientIntfError = "intf:\"error\",";
L7_char8 *pStrInfo_security_ClientCpid = "cpid:\"%s\",";
L7_char8 *pStrInfo_security_ClientCpidError = "cpid:\"error\",";
L7_char8 *pStrInfo_security_SwMAC = "swMac:\"%02x:%02x:%02x:%02x:%02x:%02x\",";
L7_char8 *pStrInfo_security_SwMACError = "swMac:\"error\",";
L7_char8 *pStrInfo_security_SwIP = "swIp:\"%s\",";
L7_char8 *pStrInfo_security_SwIPError = "swIp:\"error\",";
L7_char8 *pStrInfo_security_Protocol_1 = "protocol:\"%s\",";
L7_char8 *pStrInfo_security_ProtocolError = "protocol:\"error\",";
L7_char8 *pStrInfo_security_User = "user:\"%s\",";
L7_char8 *pStrInfo_security_UserError = "user:\"error\"}";
L7_char8 *pStrInfo_security_Verification_1 = "verification:\"%s\"}";
L7_char8 *pStrInfo_security_VerificationError = "verification:\"error\"}";
L7_char8 *pStrInfo_security_Comma = ", ";
L7_char8 *pStrInfo_security_FieldInvalidFmt = " format is invalid.";
L7_char8 *pStrInfo_security_RadiusNamedStatus = "RADIUS Named Server Status";
L7_char8 *pStrInfo_security_RadiusNamedAcctStatus = "RADIUS Named Accounting Server Status";
L7_char8 *pStrInfo_security_NoRadNamedServersExist = "No configured RADIUS named servers exist.";
L7_char8 *pStrInfo_security_NoRadNamedAcctServersExist = "No configured RADIUS named Accounting servers exist.";
L7_char8 *pStrInfo_security_HtmlFileRadiusNamedAcctSrvr= "radius_named_acct_server.html";
L7_char8 *pStrInfo_security_HtmlFileRadiusNamedSrvr = "radius_named_server.html";
L7_char8 *pStrInfo_security_RadiusAuthAcctSrvrExists = "Server Entry with specified IP address already exists!";
L7_char8 *pStrInfo_security_AddRadiusAcctSrvr = "RADIUS Accounting Server";
L7_char8 *pStrInfo_security_SrvrType = "Server Type";
L7_char8 *pStrInfo_security_PortNum_1 = "Port Number";
L7_char8 *pStrInfo_security_Current1 = "Current<BR>(*)";
