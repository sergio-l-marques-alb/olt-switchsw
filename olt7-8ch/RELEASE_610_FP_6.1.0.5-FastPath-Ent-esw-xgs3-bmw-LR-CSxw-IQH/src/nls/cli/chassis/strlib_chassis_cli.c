/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*                                                                     
* @filename   src/nls/cli/chassis/strlib_chassis_cli.c                                                      
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

#include "strlib_chassis_cli.h"

L7_char8 *pStrInfo_chassis_AlarmId = "<alarm-id>";
L7_char8 *pStrInfo_chassis_AlarmTblId = "<alarm-table-id>";
L7_char8 *pStrInfo_chassis_Severity = "<severity>";
L7_char8 *pStrInfo_chassis_AdminPowerInsertedCard = "Admin   Power          Inserted Card";
L7_char8 *pStrErr_chassis_ChassisCardDsbl2 = "Admin mode not disabled on slot %d as it is not supported on this slot.";
L7_char8 *pStrInfo_chassis_Alarm = "Alarm";
L7_char8 *pStrInfo_chassis_AlarmReportLog = "Alarm                           Report          Log";
L7_char8 *pStrInfo_chassis_AlarmLogReportService = "Alarm              Log      Report   Service";
L7_char8 *pStrInfo_chassis_ActiveAlarmClred = "Alarm Cleared.";
L7_char8 *pStrInfo_chassis_AlarmDesc = "Alarm Description";
L7_char8 *pStrErr_chassis_AlarmHistoryClred = "Alarm history Cleared.";
L7_char8 *pStrErr_chassis_AlarmHistoryNotClred = "Alarm history could not Cleared.";
L7_char8 *pStrInfo_chassis_ActiveAlarmNotClred = "Alarm not cleared.";
L7_char8 *pStrInfo_chassis_AlarmAlarmId = "AlarmId";
L7_char8 *pStrErr_chassis_ActiveAlarmNotClred_1 = "Alarms Not Cleared.";
L7_char8 *pStrInfo_chassis_AllActiveAlarmsNotClred = "Alarms could not cleared.";
L7_char8 *pStrInfo_chassis_AllActiveAlarmsClred = "All alarms cleared.";
L7_char8 *pStrInfo_chassis_Alarm_1 = "Category/Condition";
L7_char8 *pStrInfo_chassis_ClrAlarm = "Clear the active alarm.";
L7_char8 *pStrInfo_chassis_ClrAlarmHistory = "Clear the alarm history.";
L7_char8 *pStrInfo_chassis_Clred = "Cleared";
L7_char8 *pStrInfo_chassis_CodeTransferCompleted = "Code transfer completed successfully.";
L7_char8 *pStrInfo_chassis_Alarm_2 = "Configure alarm parameters.";
L7_char8 *pStrInfo_chassis_AlarmTbl = "Configure alarm table specific paramaeters.";
L7_char8 *pStrInfo_chassis_LoggingThresh = "Configure logging threshold of the alarm table.";
L7_char8 *pStrInfo_chassis_ReportInThresh = "Configure reporting threshold of the alarm table.";
L7_char8 *pStrInfo_chassis_AlarmSeverity = "Configure the alarm severity level.";
L7_char8 *pStrInfo_chassis_DataPathIntegrity = "Data path integrity";
L7_char8 *pStrInfo_chassis_ShowAlarm = "Display Active alarms .";
L7_char8 *pStrInfo_chassis_ShowBackPlan = "Display Backplane Counters information.";
L7_char8 *pStrInfo_chassis_ShowBackPlan_1 = "Display Backplane Diagnostic information.";
L7_char8 *pStrInfo_chassis_ShowBackPlan_2 = "Display Backplane information.";
L7_char8 *pStrInfo_chassis_ShowDefAlarm = "Display alarm specific information.";
L7_char8 *pStrInfo_chassis_ShowAlarmTbl = "Display alarm table.";
L7_char8 *pStrInfo_chassis_ShowAlarmHistory = "Display alarms history.";
L7_char8 *pStrInfo_chassis_ShowEnv = "Display chassis environment information .";
L7_char8 *pStrInfo_chassis_ShowModule = "Display module information.";
L7_char8 *pStrInfo_chassis_ShowFan = "Display the Fan Information.";
L7_char8 *pStrInfo_chassis_ShowPowerSupp = "Display the power supply information.";
L7_char8 *pStrInfo_chassis_AlarmLogMode = "Enable/Disable the alarm log mode.";
L7_char8 *pStrInfo_chassis_AlarmReportMode = "Enable/Disable the alarm report-mode.";
L7_char8 *pStrInfo_chassis_AlarmActiveAll = "Enter 'all' for all Active alarms.";
L7_char8 *pStrInfo_chassis_AlarmTblAll = "Enter 'all' for all alarm tables.";
L7_char8 *pStrInfo_chassis_AlarmAll = "Enter 'all' for all alarms.";
L7_char8 *pStrInfo_chassis_AlarmSeverityVal = "Enter alarm severity level (Critical|4, Major|3, Minor|2, Notification|1).";
L7_char8 *pStrInfo_chassis_AlarmIdVal = "Enter valid alarm id.";
L7_char8 *pStrInfo_chassis_AlarmTblIdVal = "Enter valid alarm table id.";
L7_char8 *pStrErr_chassis_Severity_1 = "Enter valid severity.";
L7_char8 *pStrInfo_chassis_EquipmentFailure = "Equipment failure";
L7_char8 *pStrInfo_chassis_FanStatusAlarmsPending = "Fan     Status         Alarms pending";
L7_char8 *pStrInfo_chassis_FanFailedUnplugged = "Fan failed/unplugged";
L7_char8 *pStrInfo_chassis_Fan = "Fan%u";
L7_char8 *pStrInfo_chassis_Fan0 = "Fan0";
L7_char8 *pStrInfo_chassis_Fan1 = "Fan1";
L7_char8 *pStrInfo_chassis_Fan2 = "Fan2";
L7_char8 *pStrInfo_chassis_HistorySize = "History size ...............................%u";
L7_char8 *pStrInfo_chassis_IdCountTimeAdditionalInfo = "Id    Count Time                  Additional Information";
L7_char8 *pStrInfo_chassis_IdSeverityModeModeAffectingSrcDesc = "Id    Severity     Mode     Mode     Affecting  Source      Description";
L7_char8 *pStrInfo_chassis_IdStateTimeAdditionalInfo = "Id    State    Time                  Additional Information";
L7_char8 *pStrInfo_chassis_ImageDownloadFailed = "Image download failed";
L7_char8 *pStrInfo_chassis_ImageFailure = "Image failure";
L7_char8 *pStrErr_chassis_CodeCopy = "Image transfer failed.";
L7_char8 *pStrInfo_chassis_ImageVerMismatch = "Image version mismatch";
L7_char8 *pStrErr_chassis_InvalidAlarmTbl = "Invalid alarm table.";
L7_char8 *pStrErr_chassis_AlarmId_1 = "Invalid alarm.";
L7_char8 *pStrInfo_chassis_Lm0 = "LM0";
L7_char8 *pStrInfo_chassis_Lm1 = "LM1";
L7_char8 *pStrInfo_chassis_Lm2 = "LM2";
L7_char8 *pStrInfo_chassis_Lm3 = "LM3";
L7_char8 *pStrInfo_chassis_AlarmLogMode_1 = "Log-mode";
L7_char8 *pStrInfo_chassis_AlarmsSeverityMajor = "Major";
L7_char8 *pStrInfo_chassis_Alarms = "Minor";
L7_char8 *pStrInfo_chassis_ModuleFailure = "Module Failure";
L7_char8 *pStrInfo_chassis_ModuleMismatch = "Module mismatch";
L7_char8 *pStrInfo_chassis_ModuleUnplugged = "Module unplugged";
L7_char8 *pStrInfo_chassis_Nsa = "NSA";
L7_char8 *pStrErr_chassis_NoAlarmTbl = "No alarm tables present in the system";
L7_char8 *pStrInfo_chassis_AlarmsSeverityNotification = "Notification";
L7_char8 *pStrInfo_chassis_NumOfEntriesInHistory = "Number of entries in the history ...........%u";
L7_char8 *pStrInfo_chassis_Ps = "PS%-6u";
L7_char8 *pStrInfo_chassis_Ps0 = "PS0";
L7_char8 *pStrInfo_chassis_Ps1 = "PS1";
L7_char8 *pStrInfo_chassis_Ps2 = "PS2";
L7_char8 *pStrErr_chassis_ChassisCardNoPower2 = "Power mode not disabled on slot %d as it cannot be powered down.";
L7_char8 *pStrInfo_chassis_ProcessingError = "Processing error";
L7_char8 *pStrInfo_chassis_ReloadChassis = "Reload all modules or a module.";
L7_char8 *pStrInfo_chassis_AlarmReportMode_1 = "Report-mode";
L7_char8 *pStrInfo_chassis_Sa = "SA";
L7_char8 *pStrInfo_chassis_Alarm_3 = "Service Affecting";
L7_char8 *pStrInfo_chassis_SlotStatusStateStateModelIdPluggable = "Slot  Status  State   State             Model ID              Pluggable";
L7_char8 *pStrInfo_chassis_SlotIsPoweredOff = "Slot is powered off";
L7_char8 *pStrInfo_chassis_SoftwareBootFailure = "Software boot failure";
L7_char8 *pStrErr_chassis_InvalidOptions = "Source and destination should not be same.";
L7_char8 *pStrErr_chassis_ChassisDownLoadCodeFromSta = "Standby CFM is not present";
L7_char8 *pStrInfo_chassis_SupplyStatusAlarmsPending = "Supply  Status         Alarms Pending";
L7_char8 *pStrInfo_chassis_TblDescThreshThresh = "Table Description               Threshold       Threshold";
L7_char8 *pStrErr_chassis_ClrAlarmHistory_1 = "The alarm history could not be cleared.";
L7_char8 *pStrInfo_chassis_AlarmsSeverityUn = "Undetermined";
L7_char8 *pStrErr_chassis_CfgLoggingThresh = "Use 'alarm log-threshold <alarm-table-id> <severity>'.";
L7_char8 *pStrErr_chassis_CfgReportIn = "Use 'alarm rep-threshold <alarm-table-id> <severity>'.";
L7_char8 *pStrErr_chassis_AlarmsSeverity = "Use 'alarm severity <alarm-table-id> {<alarm-id> | all} <severity>'.";
L7_char8 *pStrErr_chassis_NoAlarmsSeverity = "Use 'alarm severity <alarm-table-id> {<alarm-id> | all}'.";
L7_char8 *pStrErr_chassis_ClrAlarmHistory_2 = "Use 'clear alarm history'.";
L7_char8 *pStrErr_chassis_ClrAlarms = "Use 'clear alarms {<alarm-id> | all}'.";
L7_char8 *pStrErr_chassis_CopyCfg = "Use 'copy <nvram:startup-config | standby:config>   <standby:config |\r\nnvram:startup-config>'.";
L7_char8 *pStrErr_chassis_CfgCopyImage = "Use 'copy {standby:image1 | standy:image2} {system:image1 | system:image2}'.";
L7_char8 *pStrErr_chassis_NoCfgLoggingThresh = "Use 'no alarm log-threshold <alarm-table-id>'.";
L7_char8 *pStrErr_chassis_NoCfgReportIn = "Use 'no alarm rep-threshold <alarm-table-id>'.";
L7_char8 *pStrErr_chassis_NoCfgLogMode = "Use 'no set alarm log-mode <alarm-table-id> {<alarm-id> | all}'.";
L7_char8 *pStrErr_chassis_NoCfgReportMode = "Use 'no set alarm report-mode <alarm-table-id> {<alarm-id> | all}'.";
L7_char8 *pStrErr_chassis_ChassisNoSlotDsbl = "Use 'no set slot disable {slot | all}.'";
L7_char8 *pStrErr_chassis_ChassisNoSlotPower = "Use 'no set slot power {slot | all}.'";
L7_char8 *pStrErr_chassis_CfgLogMode = "Use 'set alarm log-mode <alarm-table-id> {<alarm-id> | all}'.";
L7_char8 *pStrErr_chassis_CfgReportMode = "Use 'set alarm report-mode <alarm-table-id> {<alarm-id> | all}'.";
L7_char8 *pStrErr_chassis_ChassisSlotDsbl = "Use 'set slot disable {slot | all}.'";
L7_char8 *pStrErr_chassis_ChassisSlotPower = "Use 'set slot power {slot | all}.'";
L7_char8 *pStrErr_chassis_EnvAlarms = "Use 'show environment alarm {<alarm-id> | all | history}'.";
L7_char8 *pStrErr_chassis_ShowAlarmTbl_1 = "Use 'show environment alarm-table {<alarm-table-id> [alarm <alarm-id>] | all}'.";
L7_char8 *pStrErr_chassis_ShowAlarmHistory_1 = "Use 'show environment alarms history'.";
L7_char8 *pStrErr_chassis_FanStatus = "Use 'show environment fan'.";
L7_char8 *pStrErr_chassis_PowerSupp = "Use'show environment power.'";
L7_char8 *pStrErr_chassis_ConfirmClrAlarmHistory = "\r\nAre you sure you want to clear alarm history? (y/n) \0";
L7_char8 *pStrErr_chassis_ConfirmClrActiveAlarmsAll = "\r\nAre you sure you want to clear all Active alarms ? (y/n) \0";
L7_char8 *pStrErr_chassis_ConfirmClrActiveAlarm = "\r\nAre you sure you want to clear the alarm ? (y/n) \0";
L7_char8 *pStrInfo_chassis_Alarm_4 = "alarm";
L7_char8 *pStrInfo_chassis_AlarmLogMode_2 = "alarm log-mode %u %u";
L7_char8 *pStrInfo_chassis_AlarmReportMode_2 = "alarm report-mode %u %u";
L7_char8 *pStrInfo_chassis_AlarmSeverity_2 = "alarm severity %u %u %u";
L7_char8 *pStrInfo_chassis_AlarmTbl_1 = "alarm-table";
L7_char8 *pStrInfo_chassis_AlarmTblLogThresh = "alarm-table log-threshold";
L7_char8 *pStrInfo_chassis_AlarmTblReportThresh = "alarm-table report-threshold";
L7_char8 *pStrInfo_chassis_Backplane = "backplane";
L7_char8 *pStrInfo_chassis_ClrAlarm_1 = "clear alarm %s";
L7_char8 *pStrInfo_chassis_ClrAlarmHistory_3 = "clear alarm history";
L7_char8 *pStrErr_chassis_CodeTransferFailed = "code transfer failed.";
L7_char8 *pStrErr_chassis_CopyNvramTosSta = "copy nvram:startup-config standby:config";
L7_char8 *pStrErr_chassis_CopySta = "copy standby:config nvram:startup-config";
L7_char8 *pStrInfo_chassis_Environment = "environment";
L7_char8 *pStrInfo_chassis_Fan_1 = "fan";
L7_char8 *pStrInfo_chassis_LogMode = "log-mode";
L7_char8 *pStrInfo_chassis_LogThresh = "log-threshold";
L7_char8 *pStrInfo_chassis_Module = "module";
L7_char8 *pStrInfo_chassis_NoAlarmLogMode = "no alarm log-mode %u %u";
L7_char8 *pStrInfo_chassis_NoAlarmReportMode = "no alarm report-mode %u %u";
L7_char8 *pStrInfo_chassis_NoSetSlotDsbl = "no set slot disable %d";
L7_char8 *pStrInfo_chassis_RepThresh = "rep-threshold";
L7_char8 *pStrInfo_chassis_ReportMode = "report-mode";
L7_char8 *pStrErr_chassis_LogModeSet = "set alarm log-mode.";
L7_char8 *pStrErr_chassis_ReportMode_1 = "set alarm report-mode.";
L7_char8 *pStrErr_chassis_SeveritySet = "set alarm severity.";
L7_char8 *pStrErr_chassis_LogThresh_1 = "set log-threshold.";
L7_char8 *pStrErr_chassis_ReportThresh = "set report-threshold.";
L7_char8 *pStrInfo_chassis_SetSlotDsbl = "set slot disable %d";
L7_char8 *pStrInfo_chassis_SetSlotPower = "set slot power %d";
L7_char8 *pStrInfo_chassis_Severity_2 = "severity";
L7_char8 *pStrInfo_chassis_ShowActiveAlarmsCmd = "show environment alarm all";
L7_char8 *pStrInfo_chassis_ShowAlarmsHistoryCmd = "show environment alarm history";
L7_char8 *pStrInfo_chassis_ShowEnvAlarmTblCmd = "show environment alarm-table %s";
L7_char8 *pStrInfo_chassis_Standby = "standby";
L7_char8 *pStrInfo_chassis_Supervisor_1 = "supervisor";

