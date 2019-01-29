/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/include/strlib_stacking_cli.h                                                      
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

#ifndef STRLIB_STACKING_CLI_C
#define STRLIB_STACKING_CLI_C

#include "datatypes.h"
extern L7_char8 *pStrInfo_stacking_Supervisor;
extern L7_char8 *pStrInfo_stacking_StackModePrompt;
extern L7_char8 *pStrInfo_stacking_TxRx;
extern L7_char8 *pStrInfo_stacking_Fromunit;
extern L7_char8 *pStrInfo_stacking_Newunit;
extern L7_char8 *pStrInfo_stacking_Tounit;
extern L7_char8 *pStrInfo_stacking_CardIdx_1;
extern L7_char8 *pStrInfo_stacking_Code_1;
extern L7_char8 *pStrInfo_stacking_StackPortEtherNet;
extern L7_char8 *pStrInfo_stacking_StackPortsStacking;
extern L7_char8 *pStrInfo_stacking_StackPort;
extern L7_char8 *pStrInfo_stacking_CfgSwitch;
extern L7_char8 *pStrInfo_stacking_CfguredRunning;
extern L7_char8 *pStrInfo_stacking_StackCfgMbr;
extern L7_char8 *pStrInfo_stacking_DataErrorDataError;
extern L7_char8 *pStrInfo_stacking_CodeVerDetected;
extern L7_char8 *pStrInfo_stacking_CodeInFlashDetected;
extern L7_char8 *pStrInfo_stacking_ShowStackPortCounters;
extern L7_char8 *pStrInfo_stacking_ShowStackPortDiag;
extern L7_char8 *pStrInfo_stacking_ShowStackPort;
extern L7_char8 *pStrInfo_stacking_ShowSwitch;
extern L7_char8 *pStrInfo_stacking_QosMode;
extern L7_char8 *pStrInfo_stacking_IntfsStacking;
extern L7_char8 *pStrInfo_stacking_SwitchPri;
extern L7_char8 *pStrInfo_stacking_CodeVerExpected;
extern L7_char8 *pStrInfo_stacking_MgmtPrefHardware;
extern L7_char8 *pStrErr_stacking_InvalidUnitX;
extern L7_char8 *pStrErr_stacking_UsrInputUnitType;
extern L7_char8 *pStrInfo_stacking_LinkLink;
extern L7_char8 *pStrInfo_stacking_UnitMacAddr;
extern L7_char8 *pStrInfo_stacking_MidModuleModelIdType;
extern L7_char8 *pStrInfo_stacking_MgmtPref;
extern L7_char8 *pStrInfo_stacking_MgmtStatus;
extern L7_char8 *pStrInfo_stacking_MgmtCode;
extern L7_char8 *pStrInfo_stacking_ModuleModuleCode;
extern L7_char8 *pStrInfo_stacking_StackCfgMoveMgmt;
extern L7_char8 *pStrInfo_stacking_VerifyMoveStackMgmt;
extern L7_char8 *pStrErr_stacking_SwitchRenumberExists;
extern L7_char8 *pStrErr_stacking_SwitchRenumberSame;
extern L7_char8 *pStrInfo_stacking_ModelIdPluggedIn;
extern L7_char8 *pStrInfo_stacking_Lag_2;
extern L7_char8 *pStrInfo_stacking_ModelIdPreconfig;
extern L7_char8 *pStrInfo_stacking_QosMode_1;
extern L7_char8 *pStrInfo_stacking_RateRateTotalRateRateTotal;
extern L7_char8 *pStrInfo_stacking_ReloadStack;
extern L7_char8 *pStrInfo_stacking_CfgSwitchRenumber;
extern L7_char8 *pStrInfo_stacking_SidSwitchModelIdPrefType;
extern L7_char8 *pStrInfo_stacking_CfgSwitchPri;
extern L7_char8 *pStrInfo_stacking_SlotPortsModelIdStatusVer;
extern L7_char8 *pStrInfo_stacking_SlotIntfStatusSpeedGbS;
extern L7_char8 *pStrInfo_stacking_SlotIntfMbSErrsSErrsMbSErrsSErrs;
extern L7_char8 *pStrInfo_stacking_UnitFromToSwitchId;
extern L7_char8 *pStrInfo_stacking_Stack;
extern L7_char8 *pStrInfo_stacking_StackStackLinkLink;
extern L7_char8 *pStrInfo_stacking_StackLinkSpeed2p5;
extern L7_char8 *pStrInfo_stacking_CmdMoveMgmtNotPerformed;
extern L7_char8 *pStrInfo_stacking_CardSupported;
extern L7_char8 *pStrInfo_stacking_UnitAlreadyExists;
extern L7_char8 *pStrErr_stacking_UsrInputSwPri;
extern L7_char8 *pStrErr_stacking_UsrInputStbySwPriErr;
extern L7_char8 *pStrErr_stacking_MovingMgmtSwitch;
extern L7_char8 *pStrErr_stacking_MgmtSwitchCmd;
extern L7_char8 *pStrInfo_stacking_MsgCfgUnableToSave;
extern L7_char8 *pStrInfo_stacking_UnitIntfModeModeStatusSpeedGbS;
extern L7_char8 *pStrInfo_stacking_UnitIntfMbSErrsSErrsMbSErrsSErrs;
extern L7_char8 *pStrErr_stacking_SwitchTypeUnSupported;
extern L7_char8 *pStrErr_stacking_CfgIntfsStacking;
extern L7_char8 *pStrErr_stacking_CfgMbr;
extern L7_char8 *pStrErr_stacking_CfgMoveMgmt;
extern L7_char8 *pStrErr_stacking_CfgNoMbr;
extern L7_char8 *pStrErr_stacking_CfgQosMode;
extern L7_char8 *pStrErr_stacking_CfgStackPort;
extern L7_char8 *pStrErr_stacking_CfgSwitchPri_1;
extern L7_char8 *pStrErr_stacking_CfgSwitchCmdsSplit;
extern L7_char8 *pStrErr_stacking_CfgSwitchRenumber_1;
extern L7_char8 *pStrErr_stacking_SwitchAdd;
extern L7_char8 *pStrErr_stacking_IntfStacking;
extern L7_char8 *pStrInfo_stacking_DoesntExist;
extern L7_char8 *pStrErr_stacking_MgmtOrUnitId;
extern L7_char8 *pStrInfo_stacking_Mbr_1;
extern L7_char8 *pStrInfo_stacking_CmdMoveMgmt;
extern L7_char8 *pStrErr_stacking_PortsCantBeCfgForStacking;
extern L7_char8 *pStrInfo_stacking_QosMode_2;
extern L7_char8 *pStrErr_stacking_SwitchRemove;
extern L7_char8 *pStrInfo_stacking_SwitchRenumber;
extern L7_char8 *pStrErr_stacking_SwitchRenumber_1;
extern L7_char8 *pStrInfo_stacking_MgmtUnitRenumberWarning;
extern L7_char8 *pStrInfo_stacking_UnitRenumberWarning;
extern L7_char8 *pStrInfo_stacking_CmdRenumberNotPerformed;
extern L7_char8 *pStrErr_stacking_StackPortFailure;
extern L7_char8 *pStrErr_stacking_QosModeFailure;
extern L7_char8 *pStrErr_stacking_SwitchPri_2;
extern L7_char8 *pStrInfo_stacking_StackPort_1;
extern L7_char8 *pStrInfo_stacking_StackPort_2;
extern L7_char8 *pStrInfo_stacking_Stacking_1;
extern L7_char8 *pStrInfo_stacking_SwitchPri_3;
extern L7_char8 *pStrInfo_stacking_Standby;
extern L7_char8 *pStrInfo_stacking_StackCfgStandby;
extern L7_char8 *pStrErr_stacking_MgmtCannotBeStandby;
extern L7_char8 *pStrErr_stacking_UnitProconfigOrOK;
extern L7_char8 *pStrErr_stacking_StandbyConfigfailed;
extern L7_char8 *pStrErr_stacking_StandbyDoesNotExist;
extern L7_char8 *pStrInfo_stacking_NoStandbyInfo;
extern L7_char8 *pStrInfo_stacking_CfgStandby;
extern L7_char8 *pStrInfo_stacking_OperStandby;
extern L7_char8 *pStrInfo_stacking_StandbyCmd;
extern L7_char8 *pStrErr_ipaddr_Zero;
extern L7_char8 *pStrInfo_stacking_ShowStackFirmwareSync;
extern L7_char8 *pStrInfo_stacking_sfs;
extern L7_char8 *pStrInfo_stacking_synchronization;
extern L7_char8 *pStrInfo_stack_autocopysw;
extern L7_char8 *pStrInfo_stack_setAutoCopy;
extern L7_char8* pStrInfo_stack_sfs_trap;
extern L7_char8* pStrInfo_stack_sfs_auto_copy_sw_trap;
extern L7_char8* pStrInfo_stack_sfs_downgrade;
extern L7_char8* pStrInfo_stack_show_sfs_downgrade;
extern L7_char8* pStrInfo_stack_auto_trap;
extern L7_char8* pStrInfo_stack_auto_downgrade;
extern L7_char8 *pStrErr_stack_autocopysw_fail;
extern L7_char8 *pStrErr_stack_autocopysw_get_fail;
extern L7_char8 *pStrErr_stack_autocopyswtrap_fail;
extern L7_char8 *pStrErr_stack_autocopyswtrap_get_fail;
extern L7_char8 *pStrErr_stack_autocopyswdowngrade_fail;
extern L7_char8 *pStrErr_stack_autocopyswdowngrade_get_fail;
extern L7_char8 *pStrInfo_stack_runcfg_autocopy ;
extern L7_char8 *pStrInfo_stack_runcfg_autocopytrap ;
extern L7_char8 *pStrInfo_stack_runcfg_autocopyallowdowngrade ;
extern L7_char8 *pStrErr_stack_autocopyinProgress;
extern L7_char8* pStrInfo_stacking_sfs_status;
extern L7_char8* pStrInfo_stacking_sfs_last_attempt_status;

#endif

