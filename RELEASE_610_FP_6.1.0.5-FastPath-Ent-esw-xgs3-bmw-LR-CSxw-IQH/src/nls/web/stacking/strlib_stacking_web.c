/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/web/stacking/strlib_stacking_web.c                                                      
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

#include "strlib_stacking_web.h"

L7_char8 *pStrInfo_stacking_CodeMismatch = "Code Mismatch";
L7_char8 *pStrInfo_stacking_Codemismatch = "CodeMismatch";
L7_char8 *pStrInfo_stacking_CfgMismatch = "Config Mismatch";
L7_char8 *pStrInfo_stacking_CopyFailed = "Copy Failed";
L7_char8 *pStrInfo_stacking_DstImage = "Destination Image";
L7_char8 *pStrInfo_stacking_DstUnit = "Destination Unit";
L7_char8 *pStrInfo_stacking_DownloadFirmware = "Download Firmware";
L7_char8 *pStrInfo_stacking_FirmwareUpdate = "Firmware Update";
L7_char8 *pStrInfo_stacking_FrontPanelStackingNotSupported = "Front panel stacking not supported";
L7_char8 *pStrInfo_stacking_FrontPanelStackingQosMode = "Front panel stacking qos mode";
L7_char8 *pStrErr_stacking_SwitchType = "Invalid Switch Type";
L7_char8 *pStrErr_stacking_UnitId = "Invalid Unit ID";
L7_char8 *pStrInfo_stacking_Notpresent = "NotPresent";
L7_char8 *pStrInfo_stacking_Pref1 = "Preference 1";
L7_char8 *pStrInfo_stacking_Pref10 = "Preference 10";
L7_char8 *pStrInfo_stacking_Pref11 = "Preference 11";
L7_char8 *pStrInfo_stacking_Pref12 = "Preference 12";
L7_char8 *pStrInfo_stacking_Pref13 = "Preference 13";
L7_char8 *pStrInfo_stacking_Pref14 = "Preference 14";
L7_char8 *pStrInfo_stacking_Pref15 = "Preference 15";
L7_char8 *pStrInfo_stacking_Pref2 = "Preference 2";
L7_char8 *pStrInfo_stacking_Pref3 = "Preference 3";
L7_char8 *pStrInfo_stacking_Pref4 = "Preference 4";
L7_char8 *pStrInfo_stacking_Pref5 = "Preference 5";
L7_char8 *pStrInfo_stacking_Pref6 = "Preference 6";
L7_char8 *pStrInfo_stacking_Pref7 = "Preference 7";
L7_char8 *pStrInfo_stacking_Pref8 = "Preference 8";
L7_char8 *pStrInfo_stacking_Pref9 = "Preference 9";
L7_char8 *pStrInfo_stacking_SrcImage = "Source Image";
L7_char8 *pStrInfo_stacking_StackPortCfg = "Stack Port Configuration";
L7_char8 *pStrInfo_stacking_StackPortCounters = "Stack Port Counters";
L7_char8 *pStrInfo_stacking_StackPortDiagnostics = "Stack Port Diagnostics";
L7_char8 *pStrInfo_stacking_StackPortSummary = "Stack Port Summary";
L7_char8 *pStrInfo_stacking_StackSummary = "Stack Summary";
L7_char8 *pStrInfo_stacking_StackingMbr = "Stacking Member";
L7_char8 *pStrInfo_stacking_SupportedSwitches = "Supported Switches";
L7_char8 *pStrInfo_stacking_SwitchId = "Switch ID";
L7_char8 *pStrInfo_stacking_TransferFailed = "Transfer Failed";
L7_char8 *pStrInfo_stacking_TransferSuccessful = "Transfer Successful";
L7_char8 *pStrInfo_stacking_TransferInProgress = "Transfer in progress...";
L7_char8 *pStrInfo_stacking_UnitCfg = "Unit Configuration";
L7_char8 *pStrInfo_stacking_HtmlFileArchiveCopy = "archive_copy.html";
L7_char8 *pStrInfo_stacking_HtmlLinkStackingHelpStackingFirmupdt = "href=\"/stacking/help_stacking.html#firmupdt\"";
L7_char8 *pStrInfo_stacking_HtmlLinkStackingHelpStackingStackportconfig = "href=\"/stacking/help_stacking.html#stackPortConfig\"";
L7_char8 *pStrInfo_stacking_HtmlLinkStackingHelpStackingStackportcounters = "href=\"/stacking/help_stacking.html#stackPortCounters\"";
L7_char8 *pStrInfo_stacking_HtmlLinkStackingHelpStackingStackportdiag = "href=\"/stacking/help_stacking.html#stackPortDiag\"";
L7_char8 *pStrInfo_stacking_HtmlLinkStackingHelpStackingStackport = "href=\"/stacking/help_stacking.html#stackPort\"";
L7_char8 *pStrInfo_stacking_HtmlLinkStackingHelpStackingStacksumm = "href=\"/stacking/help_stacking.html#stackSumm\"";
L7_char8 *pStrInfo_stacking_HtmlLinkStackingHelpStackingSuppunit = "href=\"/stacking/help_stacking.html#suppunit\"";
L7_char8 *pStrInfo_stacking_HtmlLinkStackingHelpStackingUnitcfg = "href=\"/stacking/help_stacking.html#unitcfg\"";
L7_char8 *pStrInfo_stacking_HtmlFilePortCounters = "port_counters.html";
L7_char8 *pStrInfo_stacking_HtmlFilePortDiag = "port_diag.html";
L7_char8 *pStrInfo_stacking_HtmlFileStackPort = "stack_port.html";
L7_char8 *pStrInfo_stacking_HtmlFileStackPortCfg = "stack_port_config.html";
L7_char8 *pStrInfo_stacking_HtmlFileStackSummary = "stack_summary.html";
L7_char8 *pStrInfo_stacking_HtmlFileSuppUnit = "supp_unit.html";
L7_char8 *pStrInfo_stacking_HtmlFileUnitCfg = "unit_config.html";

