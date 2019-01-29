/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/include/strlib_chassis_cli.h                                                      
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

#ifndef STRLIB_CHASSIS_CLI_C
#define STRLIB_CHASSIS_CLI_C

#include "datatypes.h"
extern L7_char8 *pStrInfo_chassis_AlarmId;
extern L7_char8 *pStrInfo_chassis_AlarmTblId;
extern L7_char8 *pStrInfo_chassis_Severity;
extern L7_char8 *pStrInfo_chassis_AdminPowerInsertedCard;
extern L7_char8 *pStrErr_chassis_ChassisCardDsbl2;
extern L7_char8 *pStrInfo_chassis_Alarm;
extern L7_char8 *pStrInfo_chassis_AlarmReportLog;
extern L7_char8 *pStrInfo_chassis_AlarmLogReportService;
extern L7_char8 *pStrInfo_chassis_ActiveAlarmClred;
extern L7_char8 *pStrInfo_chassis_AlarmDesc;
extern L7_char8 *pStrErr_chassis_AlarmHistoryClred;
extern L7_char8 *pStrErr_chassis_AlarmHistoryNotClred;
extern L7_char8 *pStrInfo_chassis_ActiveAlarmNotClred;
extern L7_char8 *pStrInfo_chassis_AlarmAlarmId;
extern L7_char8 *pStrErr_chassis_ActiveAlarmNotClred_1;
extern L7_char8 *pStrInfo_chassis_AllActiveAlarmsNotClred;
extern L7_char8 *pStrInfo_chassis_AllActiveAlarmsClred;
extern L7_char8 *pStrInfo_chassis_Alarm_1;
extern L7_char8 *pStrInfo_chassis_ClrAlarm;
extern L7_char8 *pStrInfo_chassis_ClrAlarmHistory;
extern L7_char8 *pStrInfo_chassis_Clred;
extern L7_char8 *pStrInfo_chassis_CodeTransferCompleted;
extern L7_char8 *pStrInfo_chassis_Alarm_2;
extern L7_char8 *pStrInfo_chassis_AlarmTbl;
extern L7_char8 *pStrInfo_chassis_LoggingThresh;
extern L7_char8 *pStrInfo_chassis_ReportInThresh;
extern L7_char8 *pStrInfo_chassis_AlarmSeverity;
extern L7_char8 *pStrInfo_chassis_DataPathIntegrity;
extern L7_char8 *pStrInfo_chassis_ShowAlarm;
extern L7_char8 *pStrInfo_chassis_ShowBackPlan;
extern L7_char8 *pStrInfo_chassis_ShowBackPlan_1;
extern L7_char8 *pStrInfo_chassis_ShowBackPlan_2;
extern L7_char8 *pStrInfo_chassis_ShowDefAlarm;
extern L7_char8 *pStrInfo_chassis_ShowAlarmTbl;
extern L7_char8 *pStrInfo_chassis_ShowAlarmHistory;
extern L7_char8 *pStrInfo_chassis_ShowEnv;
extern L7_char8 *pStrInfo_chassis_ShowModule;
extern L7_char8 *pStrInfo_chassis_ShowFan;
extern L7_char8 *pStrInfo_chassis_ShowPowerSupp;
extern L7_char8 *pStrInfo_chassis_AlarmLogMode;
extern L7_char8 *pStrInfo_chassis_AlarmReportMode;
extern L7_char8 *pStrInfo_chassis_AlarmActiveAll;
extern L7_char8 *pStrInfo_chassis_AlarmTblAll;
extern L7_char8 *pStrInfo_chassis_AlarmAll;
extern L7_char8 *pStrInfo_chassis_AlarmSeverityVal;
extern L7_char8 *pStrInfo_chassis_AlarmIdVal;
extern L7_char8 *pStrInfo_chassis_AlarmTblIdVal;
extern L7_char8 *pStrErr_chassis_Severity_1;
extern L7_char8 *pStrInfo_chassis_EquipmentFailure;
extern L7_char8 *pStrInfo_chassis_FanStatusAlarmsPending;
extern L7_char8 *pStrInfo_chassis_FanFailedUnplugged;
extern L7_char8 *pStrInfo_chassis_Fan;
extern L7_char8 *pStrInfo_chassis_Fan0;
extern L7_char8 *pStrInfo_chassis_Fan1;
extern L7_char8 *pStrInfo_chassis_Fan2;
extern L7_char8 *pStrInfo_chassis_HistorySize;
extern L7_char8 *pStrInfo_chassis_IdCountTimeAdditionalInfo;
extern L7_char8 *pStrInfo_chassis_IdSeverityModeModeAffectingSrcDesc;
extern L7_char8 *pStrInfo_chassis_IdStateTimeAdditionalInfo;
extern L7_char8 *pStrInfo_chassis_ImageDownloadFailed;
extern L7_char8 *pStrInfo_chassis_ImageFailure;
extern L7_char8 *pStrErr_chassis_CodeCopy;
extern L7_char8 *pStrInfo_chassis_ImageVerMismatch;
extern L7_char8 *pStrErr_chassis_InvalidAlarmTbl;
extern L7_char8 *pStrErr_chassis_AlarmId_1;
extern L7_char8 *pStrInfo_chassis_Lm0;
extern L7_char8 *pStrInfo_chassis_Lm1;
extern L7_char8 *pStrInfo_chassis_Lm2;
extern L7_char8 *pStrInfo_chassis_Lm3;
extern L7_char8 *pStrInfo_chassis_AlarmLogMode_1;
extern L7_char8 *pStrInfo_chassis_AlarmsSeverityMajor;
extern L7_char8 *pStrInfo_chassis_Alarms;
extern L7_char8 *pStrInfo_chassis_ModuleFailure;
extern L7_char8 *pStrInfo_chassis_ModuleMismatch;
extern L7_char8 *pStrInfo_chassis_ModuleUnplugged;
extern L7_char8 *pStrInfo_chassis_Nsa;
extern L7_char8 *pStrErr_chassis_NoAlarmTbl;
extern L7_char8 *pStrInfo_chassis_AlarmsSeverityNotification;
extern L7_char8 *pStrInfo_chassis_NumOfEntriesInHistory;
extern L7_char8 *pStrInfo_chassis_Ps;
extern L7_char8 *pStrInfo_chassis_Ps0;
extern L7_char8 *pStrInfo_chassis_Ps1;
extern L7_char8 *pStrInfo_chassis_Ps2;
extern L7_char8 *pStrErr_chassis_ChassisCardNoPower2;
extern L7_char8 *pStrInfo_chassis_ProcessingError;
extern L7_char8 *pStrInfo_chassis_ReloadChassis;
extern L7_char8 *pStrInfo_chassis_AlarmReportMode_1;
extern L7_char8 *pStrInfo_chassis_Sa;
extern L7_char8 *pStrInfo_chassis_Alarm_3;
extern L7_char8 *pStrInfo_chassis_SlotStatusStateStateModelIdPluggable;
extern L7_char8 *pStrInfo_chassis_SlotIsPoweredOff;
extern L7_char8 *pStrInfo_chassis_SoftwareBootFailure;
extern L7_char8 *pStrErr_chassis_InvalidOptions;
extern L7_char8 *pStrErr_chassis_ChassisDownLoadCodeFromSta;
extern L7_char8 *pStrInfo_chassis_SupplyStatusAlarmsPending;
extern L7_char8 *pStrInfo_chassis_TblDescThreshThresh;
extern L7_char8 *pStrErr_chassis_ClrAlarmHistory_1;
extern L7_char8 *pStrInfo_chassis_AlarmsSeverityUn;
extern L7_char8 *pStrErr_chassis_CfgLoggingThresh;
extern L7_char8 *pStrErr_chassis_CfgReportIn;
extern L7_char8 *pStrErr_chassis_AlarmsSeverity;
extern L7_char8 *pStrErr_chassis_NoAlarmsSeverity;
extern L7_char8 *pStrErr_chassis_ClrAlarmHistory_2;
extern L7_char8 *pStrErr_chassis_ClrAlarms;
extern L7_char8 *pStrErr_chassis_CopyCfg;
extern L7_char8 *pStrErr_chassis_CfgCopyImage;
extern L7_char8 *pStrErr_chassis_NoCfgLoggingThresh;
extern L7_char8 *pStrErr_chassis_NoCfgReportIn;
extern L7_char8 *pStrErr_chassis_NoCfgLogMode;
extern L7_char8 *pStrErr_chassis_NoCfgReportMode;
extern L7_char8 *pStrErr_chassis_ChassisNoSlotDsbl;
extern L7_char8 *pStrErr_chassis_ChassisNoSlotPower;
extern L7_char8 *pStrErr_chassis_CfgLogMode;
extern L7_char8 *pStrErr_chassis_CfgReportMode;
extern L7_char8 *pStrErr_chassis_ChassisSlotDsbl;
extern L7_char8 *pStrErr_chassis_ChassisSlotPower;
extern L7_char8 *pStrErr_chassis_EnvAlarms;
extern L7_char8 *pStrErr_chassis_ShowAlarmTbl_1;
extern L7_char8 *pStrErr_chassis_ShowAlarmHistory_1;
extern L7_char8 *pStrErr_chassis_FanStatus;
extern L7_char8 *pStrErr_chassis_PowerSupp;
extern L7_char8 *pStrErr_chassis_ConfirmClrAlarmHistory;
extern L7_char8 *pStrErr_chassis_ConfirmClrActiveAlarmsAll;
extern L7_char8 *pStrErr_chassis_ConfirmClrActiveAlarm;
extern L7_char8 *pStrInfo_chassis_Alarm_4;
extern L7_char8 *pStrInfo_chassis_AlarmLogMode_2;
extern L7_char8 *pStrInfo_chassis_AlarmReportMode_2;
extern L7_char8 *pStrInfo_chassis_AlarmSeverity_2;
extern L7_char8 *pStrInfo_chassis_AlarmTbl_1;
extern L7_char8 *pStrInfo_chassis_AlarmTblLogThresh;
extern L7_char8 *pStrInfo_chassis_AlarmTblReportThresh;
extern L7_char8 *pStrInfo_chassis_Backplane;
extern L7_char8 *pStrInfo_chassis_ClrAlarm_1;
extern L7_char8 *pStrInfo_chassis_ClrAlarmHistory_3;
extern L7_char8 *pStrErr_chassis_CodeTransferFailed;
extern L7_char8 *pStrErr_chassis_CopyNvramTosSta;
extern L7_char8 *pStrErr_chassis_CopySta;
extern L7_char8 *pStrInfo_chassis_Environment;
extern L7_char8 *pStrInfo_chassis_Fan_1;
extern L7_char8 *pStrInfo_chassis_LogMode;
extern L7_char8 *pStrInfo_chassis_LogThresh;
extern L7_char8 *pStrInfo_chassis_Module;
extern L7_char8 *pStrInfo_chassis_NoAlarmLogMode;
extern L7_char8 *pStrInfo_chassis_NoAlarmReportMode;
extern L7_char8 *pStrInfo_chassis_NoSetSlotDsbl;
extern L7_char8 *pStrInfo_chassis_RepThresh;
extern L7_char8 *pStrInfo_chassis_ReportMode;
extern L7_char8 *pStrErr_chassis_LogModeSet;
extern L7_char8 *pStrErr_chassis_ReportMode_1;
extern L7_char8 *pStrErr_chassis_SeveritySet;
extern L7_char8 *pStrErr_chassis_LogThresh_1;
extern L7_char8 *pStrErr_chassis_ReportThresh;
extern L7_char8 *pStrInfo_chassis_SetSlotDsbl;
extern L7_char8 *pStrInfo_chassis_SetSlotPower;
extern L7_char8 *pStrInfo_chassis_Severity_2;
extern L7_char8 *pStrInfo_chassis_ShowActiveAlarmsCmd;
extern L7_char8 *pStrInfo_chassis_ShowAlarmsHistoryCmd;
extern L7_char8 *pStrInfo_chassis_ShowEnvAlarmTblCmd;
extern L7_char8 *pStrInfo_chassis_Standby;
extern L7_char8 *pStrInfo_chassis_Supervisor_1;

#endif

