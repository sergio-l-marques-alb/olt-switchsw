/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename     usmdb_alarmmgr_api.h
*
* @purpose      Usmdb externs for Alarm Manager
*
* @component    Alarm Manager
*
* @comments     none
*
* @create       08/08/2005
*
* @author       ytramanjaneyulu
* 
* @end
*
**********************************************************************/

#ifndef USMDB_ALARM_MANAGER_API_H
#define USMDB_ALARM_MANAGER_API_H

#include "chassis_alarmmgr_api.h"
#include "osapi.h"

/*********************************************************************
*
* @purpose Sets the Log threshold severity for the def table.
*
* @param    alarmDefTableId        - @b{(input)}   Def Table index. 
* @param    logSeverity            - @b{(input)}   Log threshold Severity
*         Use FD_ALARM_DEF_TABLE_DEFAULT_SEVERITY for setting default value. 
* 
* @returns L7_SUCCESS      On success
* @returns L7_FAILURE      If Threshold is invalid.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableLogThresholdSet(
        L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
            L7_ALARM_SEVERITY_t logSeverity);

/*********************************************************************
*
* @purpose Gets the Log threshold severity for the def table.
*
* @param    alarmDefTableId        - @b{(input)}   Def Table index. 
* @param    logSeverity            - @b{(output)}   Log threshold Severity
* 
* @returns L7_SUCCESS      On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableLogThresholdGet(
        L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
            L7_ALARM_SEVERITY_t *logSeverity);  


/*********************************************************************
*
* @purpose Sets the Report threshold severity for the def table.
*
* @param    alarmDefTableId        - @b{(input)}   Def Table index. 
* @param    reportSeverity         - @b{(input)}   Report threshold Severity
*         Use FD_ALARM_DEF_TABLE_DEFAULT_SEVERITY for setting default value. 
* 
* @returns L7_SUCCESS      On success
* @returns L7_FAILURE      If Threshold is invalid.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableReportThresholdSet(
        L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
            L7_ALARM_SEVERITY_t reportSeverity);


/*********************************************************************
*
* @purpose Gets the Report threshold severity for the def table.
*
* @param    alarmDefTableId        - @b{(input)}   Def Table index. 
* @param    reportSeverity         - @b{(output)}   Report threshold Severity
* 
* @returns L7_SUCCESS      On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableReportThresholdGet(
        L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
            L7_ALARM_SEVERITY_t *reportSeverity);


/*********************************************************************
*
* @purpose Sets the Alarm Severity for the specified alarm.
*
* @param    alarmId                - @b{(input)}   Alarm ID. 
* @param    severity               - @b{(input)}  Alarm Severity.
*         Use FD_ALARM_DEF_TABLE_DEFAULT_SEVERITY for setting default value. 
* 
* @returns  L7_SUCCESS   - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables or 
*                           severity value is invalid.
*
* @end
* 
*********************************************************************/

L7_RC_t usmDbAlarmDefTableSeveritySet(L7_ALARM_ID_t alarmId, 
                L7_ALARM_SEVERITY_t severity);


/*********************************************************************
*
* @purpose Sets the Alarm Logging mode for the specified alarm.
*
* @param    alarmId                - @b{(input)}   Alarm ID. 
* @param    logMode                - @b{(input)}   Alarm Log mode.
* 
* @returns  L7_SUCCESS   - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables or 
*                           log mode value is invalid.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableLogModeSet(L7_ALARM_ID_t alarmId, 
                L7_ALARM_LOG_MODE_t logMode);


/*********************************************************************
*
* @purpose Sets the Alarm Logging mode for the specified alarm.
*
* @param    alarmId                - @b{(input)}   Alarm ID. 
* @param    reportMode             - @b{(input)}   Alarm Report mode.
* 
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables or 
*                           report mode value is invalid.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableReportModeSet(L7_ALARM_ID_t alarmId, 
                L7_ALARM_REPORT_MODE_t reportMode);


/*********************************************************************
*
* @purpose Gets the Alarm Type for the specified alarm.
*
* @param    alarmId                - @b{(input)}   Alarm ID. 
* @param    alarmType              - @b{(output)}   Alarm Type.
* 
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableAlarmTypeGet(L7_ALARM_ID_t alarmId, 
                L7_ALARM_TYPE_t *alarmType);


/*********************************************************************
*
* @purpose Gets the Alarm Source for the specified alarm.
*
* @param    alarmId                - @b{(input)}    Alarm ID. 
* @param    alarmSource            - @b{(output)}   Alarm Source.
* 
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableAlarmSourceGet(L7_ALARM_ID_t alarmId, 
                L7_ALARM_SOURCE_t *alarmSource);


/*********************************************************************
*
* @purpose Gets the Alarm Category for the specified alarm.
*
* @param    alarmId                - @b{(input)}    Alarm ID. 
* @param    alarmCategory          - @b{(output)}   Alarm Category.
* 
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableAlarmCategoryGet(L7_ALARM_ID_t alarmId, 
                L7_ALARM_CATEGORY_t *alarmCategory);


/*********************************************************************
*
* @purpose Gets the Alarm Severity for the specified alarm.
*
* @param    alarmId                - @b{(input)}    Alarm ID. 
* @param    severity               - @b{(output)}   Alarm Severity.
* 
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableSeverityGet(L7_ALARM_ID_t alarmId, 
                L7_ALARM_SEVERITY_t *severity);

/*********************************************************************
*
* @purpose Gets the Service affects or not for the specified alarm.
*
* @param    alarmId                - @b{(input)}    Alarm ID. 
* @param    serviceAffect          - @b{(output)}   it indicates whether 
*                                                   Service affects or not.
* 
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableServiceAffectGet(L7_ALARM_ID_t alarmId, 
                L7_ALARM_SERVICE_AFFECT_t *serviceAffect);


/*********************************************************************
*
* @purpose Gets the Alarm Report mode for the specified alarm.
*
* @param    alarmId                - @b{(input)}    Alarm ID. 
* @param    reportMode             - @b{(output)}   Report Mode.
* 
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableReportModeGet(L7_ALARM_ID_t alarmId, 
                L7_ALARM_REPORT_MODE_t *reportMode);


/*********************************************************************
*
* @purpose Gets the Alarm Report mode for the specified alarm.
*
* @param    alarmId                - @b{(input)}    Alarm ID. 
* @param    logMode                - @b{(output)}   Log Mode.
* 
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableLogModeGet(L7_ALARM_ID_t alarmId, 
                L7_ALARM_LOG_MODE_t *logMode);


/*********************************************************************
*
* @purpose Gets the Alarm Description for the specified alarm.
*
* @param    alarmId                - @b{(input)}    Alarm ID. 
* @param    alarmDesc              - @b{(output)}   Alarm Description.
* 
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableAlarmDescGet(L7_ALARM_ID_t alarmId, 
                L7_char8 *alarmDesc);


/*********************************************************************
* @purpose  This function checks whether the alarm entry is exists or not 
*           in all def tables.
*
* @param    alarmId  - @b{(input)}  Alarm ID.
*
* @returns  L7_SUCCESS     - There were no errors.
* @returns  L7_NOT_EXIST   - The Alarm Id does not exists.
* @end
*********************************************************************/
L7_RC_t usmDbAlarmDefTableEntryExists(L7_ALARM_ID_t alarmId);


/*********************************************************************
* @purpose  This function gets the first alarm ID in specified definition 
*             table.
* 
* @param    alarmDefTableId - @b{(input)}  Def table ID. 
* @param    alarmId         - @b{(output)}  The Alarm ID.
*
* @returns  L7_SUCCESS      -  There were no errors. 
* @returns  L7_NOT_EXIST    -  If the specified Def table is empty. 
* @returns  L7_FAILURE      -  The def table Id is greater than max 
*                                   supported.     
* @end
*********************************************************************/
L7_RC_t usmDbAlarmDefTableEntryGetFirst(
                L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
                L7_ALARM_ID_t *alarmId);


/*********************************************************************
* @purpose  This function gets the next alarm Id to the specified alarm Id in 
*            specified definition table.
*
* @param    alarmDefTableId     - @b{(input)}   Def table ID. 
* @param    alarmId             - @b{(input)}   Current Alalrm ID.
* @param    nextAlarmId         - @b{(output)}  Next alarm Id to the specified 
*                                               alarm Id in specified alarm 
*                                               def table.
*
* @returns  L7_SUCCESS      - There were no errors. 
* @returns  L7_NOT_EXIST    - If the Current alarm Id is the last entry in the 
*                                 specified Def table or specified alarm Id 
*                                 not exists in def table. 
* @returns  L7_FAILURE      - The def table Id is greater than max 
*                                   supported.     
* @end
*********************************************************************/
L7_RC_t usmDbAlarmDefTableEntryGetNext(
                L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
                L7_ALARM_ID_t alarmId, L7_ALARM_ID_t *nextAlarmId );


/*********************************************************************
*
* @purpose Checks whether the table ID is exists or not for def table.
*
* @param    alarmDefTableId - @b{(input)}  Def table ID. 
*
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_NOT_EXIST  - The Id is not exists.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefTableIdExists(L7_ALARM_DEFINITION_TABLE_t alarmDefTableId);


/*********************************************************************
*
* @purpose Gets the Description about specified alarm Table ID.
*
* @param    alarmDefTableId - @b{(input)}   Def table ID. 
* @param    tableName       - @b{(output)}  Table Name. 
*
* @returns  L7_SUCCESS      - There were no errors. 
* @returns  L7_FAILURE      - The def table Id is greater than max 
*                             supported.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmDefinitionTableDescGet(
                L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
                L7_uchar8 *tableName);


/*********************************************************************
* @purpose  This function gets the First Def Table ID.
*           
* @param    alarmDefTableId - @b{(output)}  First def table ID. 
*
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_NOT_EXIST  - If def table is empty.
* @end
*********************************************************************/
L7_RC_t usmDbAlarmDefinitionTableGetFirst(L7_ALARM_DEFINITION_TABLE_t 
        *alarmDefTableId);


/*********************************************************************
* @purpose  This function gets the next Def Table ID to specified def 
*           table Id.
*           
* @param    alarmCurrentDefTableId - @b{(input)}  Current def table ID. 
* @param    alarmNextDefTableId    - @b{(output)}  Next def table ID. 
*
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_NOT_EXIST  - If current def id is the last entry or tabled Id 
*                             does not exist.
* @end
*********************************************************************/
L7_RC_t usmDbAlarmDefinitionTableGetNext(
                L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
                L7_ALARM_DEFINITION_TABLE_t *nextAlarmDefTableId);


/*********************************************************************
*
* @purpose Gets the Alarm ID for the specified handle from Log Current Table. 
*
* @param    handle        - @b{(input)}   Current Table handle. 
* @param    alarmId       - @b{(output)}  Alarm ID. 
*
* @returns  L7_SUCCESS         - There were no errors. 
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableAlarmIdGet(L7_uint32 handle, 
                        L7_ALARM_ID_t *alarmId);


/*********************************************************************
*
* @purpose Gets the Alarm Count for the specified handle from Log Current 
*            Table. 
*
* @param  L7_uint32 handle   Handle. Index to the table.
* @param  L7_uint32 *alarmCount Alarm Count.
*
* @returns L7_SUCCESS,     if success
* @returns L7_FAILURES,    if parameters are invalid or other error
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableAlarmCountGet(L7_uint32 handle, 
                        L7_uint32 *alarmCount);


/*********************************************************************
*
* @purpose Gets the Alarm Time stamp for the specified handle from Log Current 
*            Table. 
*
* @param  L7_uint32 handle   Handle. Index to the table.
* @param  L7_clocktime *alarmTimeStamp  Alarm Time Stamp.
*
* @returns L7_SUCCESS,     if success
* @returns L7_FAILURES,    if parameters are invalid or other error
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableAlarmTimeStampGet(L7_uint32 handle, 
                        L7_clocktime *alarmTimeStamp);

/*********************************************************************
*
* @purpose  Gets the Alarm additional information the specified handle from 
*           Log Current Table. 
*
* 
* @param    handle           - @b{(input)}   Current Table handle. 
* @param    alarmDesc        - @b{(output)}  Additional information about 
*                                            alarm in Log Current table. 
*
* @returns L7_SUCCESS     - On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableAlarmAdditionalInfoGet(L7_uint32 handle, 
                        L7_char8 *alarmDesc);


/*********************************************************************
*
* @purpose Checks whether the Entry is exists or not in Log Current Table.
*
* @param    handle          - @b{(input)}  Handle.
*
* @returns  L7_SUCCESS          - There were no errors. 
* @returns  L7_NOT_EXIST        - The Entry does not exits.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableEntryExists(L7_uint32 handle);


/*********************************************************************
*
* @purpose Returns the handle associated with alarm ID in Log Current Table.
*
* @param    alarmId         - @b{(input)}  AlarmId.
* @param    handle          - @b{(output)}  Handle.
*
* @returns  L7_SUCCESS         - There were no errors. 
* @returns  L7_NOT_EXIST       - The Entry does not exits.
*
* @end 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableHandleGet(L7_ALARM_ID_t alarmId, 
        L7_uint32 *handle);



/*********************************************************************
*
* @purpose Gets the first entry from the Log Current Table.
*
* @param    handle          - @b{(output)}  The first Handle in the Log 
*                                           Current Table.
*
* @returns  L7_SUCCESS          - There were no errors. 
* @returns  L7_NOT_EXIST        - The log current table is empty.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableEntryGetFirst(L7_uint32 *handle);


/*********************************************************************
*
* @purpose Gets the next entry to the specified index from Log Current Table.
*
* @param    handle          - @b{(input)}  The Current handle.
* @param    nextHandle      - @b{(output)} The next handle to the specified 
*                                          handle in log current table.
*
* @returns  L7_SUCCESS          - There were no errors. 
* @returns  L7_NOT_EXIST        - The Current Handle is the last entry in 
*                                   log current table or handle is not 
*                                   exists in log current table.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableEntryGetNext(L7_uint32 handle, 
                L7_uint32 *nextHandle);


/*********************************************************************
*
* @purpose Clears the specified alarm from Log Current Table.
*
* @param    alarmId         - @b{(input)}  Alarm ID.
*  
* @returns  L7_SUCCESS      - There were no errors. 
* @returns  L7_NOT_EXIST    - Alarm ID does not exists in Definition tables.
* @returns  L7_FAILURE      - There were errors in clearing the log or trap 
*                               processing.  
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmClear(L7_ALARM_ID_t alarmId);


/*********************************************************************
*
* @purpose Clears the Log Current Table.
*  
* @returns L7_SUCCESS     On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableClearAll(void);


/*********************************************************************
*
* @purpose Gets the number of alarms logged in Log Current table.
*  
* @param    numberOfEntries - @b{(output)} Number of Alarms logged in Log 
*                                          Current Table. 
*
* @returns L7_SUCCESS     On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableCountGet(L7_uint32 *numberOfEntries);


/*********************************************************************
*
* @purpose Gets the maximum number of entries supported Log Current table.
*  
* @param    maxEntries - @b{(output)} Maximum number of entries supported Log
*                                          Current Table. 
*
* @returns L7_SUCCESS     On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogCurrentTableMaxCountGet(L7_uint32 *maxEntries);


/*********************************************************************
*
* @purpose Gets the Alarm ID for the specified handle from Log History Table. 
*
* @param    handle        - @b{(input)}   History Table handle. 
* @param    alarmId       - @b{(output)}  Alarm ID. 
*
* @returns  L7_SUCCESS         - There were no errors. 
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogHistoryTableAlarmIdGet(L7_uint32 handle, 
                        L7_ALARM_ID_t *alarmId);


/*********************************************************************
*
* @purpose Gets the Alarm Status for the specified handle from Log History 
*            Table. 
*
* @param    handle        - @b{(input)}   History Table handle. 
* @param    alarmStatus   - @b{(output)}  Alarm Status. 
*
* @returns L7_SUCCESS     - On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogHistoryTableAlarmStatusGet(L7_uint32 handle, 
                        L7_ALARM_STATUS_t *alarmStatus);


/*********************************************************************
*
* @purpose  Gets the Alarm Time stamp for the specified handle from Log 
*           History Table. 
*
* @param    handle           - @b{(input)}   History Table handle. 
* @param    alarmTimeStamp   - @b{(output)}  Alarm Time Stamp. 
*
* @returns L7_SUCCESS     - On success
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogHistoryTableAlarmTimeStampGet(L7_uint32 handle, 
                        L7_clocktime *alarmTimeStamp);


/*********************************************************************
*
* @purpose  Gets the Alarm additional information for the specified handle from 
*           Log History Table. 
*
* @param    handle           - @b{(input)}   History Table handle. 
* @param    alarmDesc        - @b{(output)}  Additional information about 
*                                            alarm in Log History table. 
*
* @returns L7_SUCCESS     - On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogHistoryTableAlarmAdditionalInfoGet(L7_uint32 handle, 
                        L7_char8 *alarmDesc);


/*********************************************************************
*
* @purpose Checks whether the Entry is exists or not in Log History Table.
*
* @param    handle          - @b{(input)}  Handle.
*
* @returns  L7_SUCCESS          - There were no errors. 
* @returns  L7_NOT_EXIST        - The Entry does not exits.
*
* @end
*
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogHistoryTableEntryExists(L7_uint32 handle);


/*********************************************************************
*
* @purpose Gets the first entry from the Log History Table.
*
* @param    handle          - @b{(output)}  The first Handle in the Log 
*                                           History Table.
*
* @returns  L7_SUCCESS          - There were no errors. 
* @returns  L7_NOT_EXIST        - The log history table is empty.
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogHistoryTableEntryGetFirst(L7_uint32 *handle);


/*********************************************************************
*
* @purpose Gets the next entry to the specified index from Log History Table.
*
* @param    handle          - @b{(input)}  The Current handle.
* @param    nextHandle      - @b{(output)} The next handle to the specified 
*                                          handle in log history table.
*
* @returns  L7_SUCCESS          - There were no errors. 
* @returns  L7_NOT_EXIST        - The Current Handle is the last entry in 
*                                   log history table or handle is not 
*                                   exists in log history table.
*
* @end
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogHistoryTableEntryGetNext(L7_uint32 handle, 
                L7_uint32 *nextHandle);


/*********************************************************************
*
* @purpose Clears the Log History Table.
*  
* @returns L7_SUCCESS      On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogHistoryTableClearAll(void);


/*********************************************************************
*
* @purpose Gets the number of alarms logged in Log history table.
*  
* @param    numberOfEntries - @b{(output)} Number of Alarms logged in Log 
*                                          History Table. 
*
* @returns L7_SUCCESS     On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogHistoryTableCountGet(L7_uint32 *numberOfEntries);

/*********************************************************************
*
* @purpose Gets the maximum number of entries supported Log History table.
*  
* @param    maxEntries - @b{(output)} Maximum number of entries supported Log
*                                          History Table. 
*
* @returns L7_SUCCESS     On success
*
* @end
* 
*********************************************************************/
L7_RC_t usmDbAlarmLogHistoryTableMaxCountGet(L7_uint32 *maxEntries);

#endif /* USMDB_ALARM_MANAGER_API_H */

