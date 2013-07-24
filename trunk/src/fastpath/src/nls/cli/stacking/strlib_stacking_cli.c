/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/cli/stacking/strlib_stacking_cli.c                                                      
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

#include "strlib_stacking_cli.h"

L7_char8 *pStrInfo_stacking_Supervisor = "%u (supervisor)";
L7_char8 *pStrInfo_stacking_StackModePrompt = "(Config-stack)#";
L7_char8 *pStrInfo_stacking_TxRx = "------------TX-------------- ------------RX--------------";
L7_char8 *pStrInfo_stacking_Fromunit = "<fromunit>";
L7_char8 *pStrInfo_stacking_Newunit = "<newunit>";
L7_char8 *pStrInfo_stacking_Tounit = "<tounit>";
L7_char8 *pStrInfo_stacking_CardIdx_1 = "Card Index (CID)";
L7_char8 *pStrInfo_stacking_Code_1 = "Code";
L7_char8 *pStrInfo_stacking_StackPortEtherNet = "Configure a Front Panel Stacking Port to Ethernet Mode.";
L7_char8 *pStrInfo_stacking_StackPortsStacking = "Configure a Front Panel Stacking Port to Stacking Mode.";
L7_char8 *pStrInfo_stacking_StackPort = "Configure a Stack Port on an interface.";
L7_char8 *pStrInfo_stacking_CfgSwitch = "Configure a stack switch.";
L7_char8 *pStrInfo_stacking_CfguredRunning = "Configured  Running";
L7_char8 *pStrInfo_stacking_StackCfgMbr = "Create a Stack Member.";
L7_char8 *pStrInfo_stacking_DataErrorDataError = "Data    Error                Data    Error";
L7_char8 *pStrInfo_stacking_CodeVerDetected = "Detected Code Version";
L7_char8 *pStrInfo_stacking_CodeInFlashDetected = "Detected Code in Flash";
L7_char8 *pStrInfo_stacking_ShowStackPortCounters = "Display Stack Port Counter information.";
L7_char8 *pStrInfo_stacking_ShowStackPortDiag = "Display Stack Port Diagnostic information.";
L7_char8 *pStrInfo_stacking_ShowStackPort = "Display Stack Port information.";
L7_char8 *pStrInfo_stacking_ShowSwitch = "Display stack or switch information.";
L7_char8 *pStrInfo_stacking_QosMode = "Enable or Disable QOS Mode for Front Panel Stacking on all interfaces.";
L7_char8 *pStrInfo_stacking_IntfsStacking = "Enable or Disable interface for stacking.";
L7_char8 *pStrInfo_stacking_SwitchPri = "Enter priority in the range of %d to %d.";
L7_char8 *pStrInfo_stacking_CodeVerExpected = "Expected Code Type";
L7_char8 *pStrInfo_stacking_MgmtPrefHardware = "Hardware Management Preference";
L7_char8 *pStrErr_stacking_InvalidUnitX = "Invalid switch ID %d!";
L7_char8 *pStrErr_stacking_UsrInputUnitType = "Invalid switch index!";
L7_char8 *pStrInfo_stacking_LinkLink = "Link         Link";
L7_char8 *pStrInfo_stacking_UnitMacAddr = "MAC address";
L7_char8 *pStrInfo_stacking_MidModuleModelIdType = "MID          Module Model ID           Type";
L7_char8 *pStrInfo_stacking_MgmtPref = "Management Preference";
L7_char8 *pStrInfo_stacking_MgmtStatus = "Management Status";
L7_char8 *pStrInfo_stacking_MgmtCode = "Mgmt       Code";
L7_char8 *pStrInfo_stacking_ModuleModuleCode = "Module        Module                Code";
L7_char8 *pStrInfo_stacking_StackCfgMoveMgmt = "Move stack management functionality to a different switch.";
L7_char8 *pStrInfo_stacking_VerifyMoveStackMgmt = "Moving stack management will unconfigure entire stack including all interfaces.\r\nAre you sure you want to move stack management? (y/n) ";
L7_char8 *pStrErr_stacking_SwitchRenumberExists = "New switch number already exists!\r\nSpecify a different new switch number.";
L7_char8 *pStrErr_stacking_SwitchRenumberSame = "Old and new switch numbers are the same!\r\nSpecify a different new switch number.";
L7_char8 *pStrInfo_stacking_ModelIdPluggedIn = "Plugged-in Model Identifier";
L7_char8 *pStrInfo_stacking_Lag_2 = "Port-channel";
L7_char8 *pStrInfo_stacking_ModelIdPreconfig = "Preconfigured Model Identifier";
L7_char8 *pStrInfo_stacking_QosMode_1 = "QOS Mode";
L7_char8 *pStrInfo_stacking_RateRateTotalRateRateTotal = "Rate    Rate      Total      Rate    Rate      Total";
L7_char8 *pStrInfo_stacking_ReloadStack = "Reload stack or a switch in the stack.";
L7_char8 *pStrInfo_stacking_CfgSwitchRenumber = "Renumber a stack switch.";
L7_char8 *pStrInfo_stacking_SidSwitchModelIdPrefType = "SID         Switch Model ID              Pref       Type";
L7_char8 *pStrInfo_stacking_CfgSwitchPri = "Set or reset priority of a stack switch.";
L7_char8 *pStrInfo_stacking_SlotPortsModelIdStatusVer = "Slot           Ports Model ID      Status                Version";
L7_char8 *pStrInfo_stacking_SlotIntfStatusSpeedGbS = "Slot    Interface       Status       Speed (Gb/s)";
L7_char8 *pStrInfo_stacking_SlotIntfMbSErrsSErrsMbSErrsSErrs = "Slot   Interface      (Mb/s) (Errors/s)  Errors    (Mb/s) (Errors/s)  Errors";
L7_char8 *pStrInfo_stacking_UnitFromToSwitchId = "Source and Target Switch ID should not be the same.";
L7_char8 *pStrInfo_stacking_Stack = "Stack";
L7_char8 *pStrInfo_stacking_StackStackLinkLink = "Stack      Stack      Link       Link";
L7_char8 *pStrInfo_stacking_StackLinkSpeed2p5 = "2.5";
L7_char8 *pStrInfo_stacking_CmdMoveMgmtNotPerformed = "Stack management move not performed!";
L7_char8 *pStrInfo_stacking_CardSupported = "Supported Cards:";
L7_char8 *pStrInfo_stacking_UnitAlreadyExists = "Switch %d already exists!";
L7_char8 *pStrErr_stacking_UsrInputSwPri = "Switch priority must be in the range of %d to %d!";
L7_char8 *pStrErr_stacking_UsrInputStbySwPriErr = "Priority of configured Standby switch cannot be set to 0 (Management preference disabled)";
L7_char8 *pStrErr_stacking_MovingMgmtSwitch = "The first switch designated must be the current management switch!";
L7_char8 *pStrErr_stacking_MgmtSwitchCmd = "This command can only be used on management switch!";
L7_char8 *pStrInfo_stacking_MsgCfgUnableToSave = "Unable to save configuration!";
L7_char8 *pStrInfo_stacking_UnitIntfModeModeStatusSpeedGbS = "Unit    Interface       Mode       Mode       Status     Speed (Gb/s)";
L7_char8 *pStrInfo_stacking_UnitIntfMbSErrsSErrsMbSErrsSErrs = "Unit   Interface      (Mb/s) (Errors/s)  Errors    (Mb/s) (Errors/s)  Errors";
L7_char8 *pStrErr_stacking_SwitchTypeUnSupported = "Unsupported switch index %d!";
L7_char8 *pStrErr_stacking_CfgIntfsStacking = "Use '[no] stacking'.";
L7_char8 *pStrErr_stacking_CfgMbr = "Use 'member <unit> <switchindex>'.";
L7_char8 *pStrErr_stacking_CfgMoveMgmt = "Use 'movemanagement <fromunit> <tounit>'.";
L7_char8 *pStrErr_stacking_CfgNoMbr = "Use 'no member <unit>'.";
L7_char8 *pStrErr_stacking_CfgQosMode = "Use 'qos-mode'.";
L7_char8 *pStrErr_stacking_CfgStackPort = "Use 'stack-port <%s> {ethernet | stack}'.";
L7_char8 *pStrErr_stacking_CfgSwitchPri_1 = "Use '[no] switch <unit> priority <value>'.";
L7_char8 *pStrErr_stacking_CfgSwitchCmdsSplit = "Use '[no] switch <unit> priority <value>'\r\n             or  Use 'switch <unit> renumber <newunit>'.";
L7_char8 *pStrErr_stacking_CfgSwitchRenumber_1 = "Use 'switch <unit> renumber <newunit>'.";
L7_char8 *pStrErr_stacking_SwitchAdd = "add switch to stack.";
L7_char8 *pStrErr_stacking_IntfStacking = "configure interface for stacking.";
L7_char8 *pStrInfo_stacking_DoesntExist = "does not exist.";
L7_char8 *pStrErr_stacking_MgmtOrUnitId = "get ID of management or current switch.";
L7_char8 *pStrInfo_stacking_Mbr_1 = "member %u %u";
L7_char8 *pStrInfo_stacking_CmdMoveMgmt = "movemanagement";
L7_char8 *pStrErr_stacking_PortsCantBeCfgForStacking = "ports cannot be configured for stacking!";
L7_char8 *pStrInfo_stacking_QosMode_2 = "qos-mode";
L7_char8 *pStrErr_stacking_SwitchRemove = "remove switch from stack.";
L7_char8 *pStrInfo_stacking_SwitchRenumber = "renumber";
L7_char8 *pStrErr_stacking_SwitchRenumber_1 = "renumber switch.";
L7_char8 *pStrInfo_stacking_MgmtUnitRenumberWarning = "All the switches in the stack will be reset to perform Manager unit renumbering and the configuration of Manager switch interfaces will be cleared.\r\nAre you sure you want to renumber? (y/n)";
L7_char8 *pStrInfo_stacking_UnitRenumberWarning =  "The switch will be reset to perform unit renumbering and the configuration of switch interfaces will be cleared.\r\nAre you sure you want to renumber? (y/n)";
L7_char8 *pStrInfo_stacking_CmdRenumberNotPerformed = "Unit renumbering not performed!\r\n";
L7_char8 *pStrErr_stacking_StackPortFailure = "set Front Panel Stacking mode.";
L7_char8 *pStrErr_stacking_QosModeFailure = "set QOS mode.";
L7_char8 *pStrErr_stacking_SwitchPri_2 = "set switch priority.";
L7_char8 *pStrInfo_stacking_StackPort_1 = "stack-port";
L7_char8 *pStrInfo_stacking_StackPort_2 = "stack-port %s %s";
L7_char8 *pStrInfo_stacking_Stacking_1 = "stacking";
L7_char8 *pStrInfo_stacking_SwitchPri_3 = "switch %u priority %u";
L7_char8 *pStrInfo_stacking_Standby = "standby";
L7_char8 *pStrInfo_stacking_StackCfgStandby = "Assign a Standby.";

L7_char8 *pStrErr_stacking_MgmtCannotBeStandby = "Error: Management unit cannot be set as Standby.";
L7_char8 *pStrErr_stacking_UnitProconfigOrOK = "Error: Cannot set unit as standby Unit should have either joined the stack properly or pre-configured.\n";
L7_char8 *pStrErr_stacking_StandbyConfigfailed = "Error: Failed to set the unit as standby.\n";
L7_char8 *pStrErr_stacking_StandbyDoesNotExist = "Error: Standby does not exists.\n";
L7_char8 *pStrInfo_stacking_NoStandbyInfo = "FASTPATH will automatically select a standby.\n";
L7_char8 *pStrInfo_stacking_CfgStandby = "Cfg Stby";
L7_char8 *pStrInfo_stacking_OperStandby = "Oper Stby";
L7_char8 *pStrInfo_stacking_StandbyCmd = "standby %u";

L7_char8 *pStrInfo_stacking_ShowStackFirmwareSync = "Display Auto copy code configuration information";
L7_char8 *pStrInfo_stacking_sfs = "Stack Firmware Synchronization";
L7_char8 *pStrInfo_stacking_synchronization = "Synchronization";
L7_char8 *pStrInfo_stack_autocopysw = "auto-copy-sw";
L7_char8 *pStrInfo_stack_setAutoCopy = "Enable/Disable auto copy of code if there is a version mismatch";
L7_char8* pStrInfo_stack_sfs_trap = "trap";
L7_char8* pStrInfo_stack_sfs_auto_copy_sw_trap = "SNMP Trap status";
L7_char8* pStrInfo_stack_sfs_downgrade= "allow-downgrade";
L7_char8* pStrInfo_stack_show_sfs_downgrade= "Allow Downgrade";
L7_char8* pStrInfo_stack_auto_trap  = "Enable/Disable Stack Firmware Synchronization traps";
L7_char8* pStrInfo_stack_auto_downgrade = "Enable/Disable Downgrade of image on stack member";
L7_char8 *pStrErr_stack_autocopysw_fail = "Error! Unable to set auto copy software mode";
L7_char8 *pStrErr_stack_autocopysw_get_fail = "Error! Unable to get auto copy software mode";
L7_char8 *pStrErr_stack_autocopyswtrap_fail = "Error! Unable to set auto copy software traps";
L7_char8 *pStrErr_stack_autocopyswtrap_get_fail = "Error! Unable to get auto copy software traps mode";
L7_char8 *pStrErr_stack_autocopyswdowngrade_fail = "Error! Unable to set auto copy software allow downgrade";
L7_char8 *pStrErr_stack_autocopyswdowngrade_get_fail = "Error! Unable to get auto copy software allow downgrade mode";
L7_char8 *pStrInfo_stack_runcfg_autocopy = "boot auto-copy-sw";
L7_char8 *pStrInfo_stack_runcfg_autocopytrap = "boot auto-copy-sw trap";
L7_char8 *pStrInfo_stack_runcfg_autocopyallowdowngrade = "boot auto-copy-sw allow-downgrade";
L7_char8* pStrErr_stack_autocopyinProgress = "Error! Request Denied because Stack Firmware Synchronization is in progress";
L7_char8* pStrInfo_stacking_sfs_status = "SFS Status";
L7_char8* pStrInfo_stacking_sfs_last_attempt_status = "SFS Last Attempt Status";



