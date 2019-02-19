/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
*@filename    chassis_alarmmgr_api.h
*
*@purpose     Externs for Alarm Manager
*
*@component   Alarm Manager
*
*@comments    none
*
*@create      07/15/2005
*
*@author      ytramanjaneyulu
*@end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#ifndef CHASSIS_ALARM_MANAGER_API_H
#define CHASSIS_ALARM_MANAGER_API_H

#include "commdefs.h"
#include "chassis_commdefs.h"
#include "datatypes.h"
#include "l7_cnfgr_api.h"
#include "osapi.h"


/* This defines log table Ids. If some one adds new log table, it requires to 
 * add entry in this enum 
 */ 
typedef enum
{
  L7_ALARM_LOG_CURRENT_TABLE=0,    /* Log Current table */
  L7_ALARM_LOG_HISTORY_TABLE       /* Log History table */
} L7_ALARM_LOG_TABLE_t;

typedef enum
{
  L7_ALARM_DEF_TABLE = 0,    /* Def table */
  L7_ALARM_LOG_TABLE         /* Log table */
} L7_ALARM_TABLE_TYPE_t;

typedef enum
{
  L7_ALARM_DEF_PARAM_DESC       /* Alarm Table Description */
} L7_ALARM_DEF_TABLE_PARAMS_t;    

typedef enum
{
  L7_ALARM_LOG_CURRENT_PARAM_ALARMID = 0,       /* Alarm ID */
  L7_ALARM_LOG_CURRENT_PARAM_COUNT,       /* Alarm Count */
  L7_ALARM_LOG_CURRENT_PARAM_TIMESTAMP,       /* Alarm Time Stamp */
  L7_ALARM_LOG_CURRENT_PARAM_DESC,    /* Alarm Description */
} L7_ALARM_LOG_CURRENT_TABLE_PARAMS_t;

typedef enum
{
  L7_ALARM_LOG_HISTORY_PARAM_ALARMID = 0,       /* Alarm ID */
  L7_ALARM_LOG_HISTORY_PARAM_TIMESTAMP,       /* Alarm Time Stamp */
  L7_ALARM_LOG_HISTORY_PARAM_STATUS,       /* Alarm Time Stamp */
  L7_ALARM_LOG_HISTORY_PARAM_DESC,    /* Alarm Description */
} L7_ALARM_LOG_HISTORY_TABLE_PARAMS_t;

/* The following enum lists all the alarm IDs. Each enum is written in such 
 * way that it is self explanatory.
 */  
typedef enum
{
  /* Download to LM failed */
  L7_ALARM_ID_DOWNLOAD_TO_LM0_FAILED = 1,    
  L7_ALARM_ID_DOWNLOAD_TO_LM1_FAILED, 
  L7_ALARM_ID_DOWNLOAD_TO_LM2_FAILED, 
  L7_ALARM_ID_DOWNLOAD_TO_LM3_FAILED,
  /* LM Failed alarm Ids */
  L7_ALARM_ID_LM0_FAILED, 
  L7_ALARM_ID_LM1_FAILED,
  L7_ALARM_ID_LM2_FAILED, 
  L7_ALARM_ID_LM3_FAILED, 
  /* Download to ACM and standby CM failed alarm IDs */
  L7_ALARM_ID_DOWNLOAD_TO_ACTIVE_CM_FAILED, 
  L7_ALARM_ID_DOWNLOAD_TO_STBY_CM_FAILED, 
  L7_ALARM_ID_STBY_CM_FAILED,
  L7_ALARM_ID_STBY_CM_UNPLUGGED,
  /* Power module failed */
  L7_ALARM_ID_PM0_FAILED,
  L7_ALARM_ID_PM1_FAILED, 
  /* Power module unplugged */
  L7_ALARM_ID_PM0_UNPLUGGED, 
  L7_ALARM_ID_PM1_UNPLUGGED, 
  /* Fan module failed */
  L7_ALARM_ID_FM0_FAILED,
  L7_ALARM_ID_FM1_FAILED, 
  /* Fan Module unplugged */    
  L7_ALARM_ID_FM0_UNPLUGGED,
  L7_ALARM_ID_FM1_UNPLUGGED, 
  L7_ALARM_ID_SWITCH_IMAGE_FAILED,
  /* Board Id mismatch */
  L7_ALARM_ID_LM0_BOARD_ID_MISMATCH,
  L7_ALARM_ID_LM1_BOARD_ID_MISMATCH, 
  L7_ALARM_ID_LM2_BOARD_ID_MISMATCH,
  L7_ALARM_ID_LM3_BOARD_ID_MISMATCH,
  /* LM is inserted in Power OFF slot */
  L7_ALARM_ID_LM0_IS_INSERTED_IN_POWER_OFF_SLOT,
  L7_ALARM_ID_LM1_IS_INSERTED_IN_POWER_OFF_SLOT, 
  L7_ALARM_ID_LM2_IS_INSERTED_IN_POWER_OFF_SLOT,
  L7_ALARM_ID_LM3_IS_INSERTED_IN_POWER_OFF_SLOT,
  /* Inserted stby CM SW version has greater than Active CM version */
  L7_ALARM_ID_STBY_CM_IMAGE_MISMATCH,
  L7_ALARM_ID_STBY_CM_CONFIG_DATABASE_MISMATCH,
  L7_ALARM_ID_LM0_UNPLUGGED,
  L7_ALARM_ID_LM1_UNPLUGGED,
  L7_ALARM_ID_LM2_UNPLUGGED,
  L7_ALARM_ID_LM3_UNPLUGGED,
  /* Unsupported module Alarm IDs */
  L7_ALARM_ID_SLOT0_MODULE_UNSUPPORTED,
  L7_ALARM_ID_SLOT1_MODULE_UNSUPPORTED,
  L7_ALARM_ID_SLOT2_MODULE_UNSUPPORTED,
  L7_ALARM_ID_SLOT3_MODULE_UNSUPPORTED,
  L7_ALARM_ID_SLOT4_MODULE_UNSUPPORTED,
  L7_ALARM_ID_SLOT5_MODULE_UNSUPPORTED,
  /* Alarm ID is not defined */
  L7_ALARM_ID_UNKNOWN
} L7_ALARM_ID_t;

typedef enum
{ 
  L7_ALARM_TYPE_DOWNLOAD_FAILURE = 0, /* Download to LM, CM failed */
  L7_ALARM_TYPE_MODULE_FAILURE,       /* Module (LM/CM/FM/PM) Failure */
  L7_ALARM_TYPE_MODULE_UNPLUG,        /* Module (LM/CM/FM/PM) unplueed */
  L7_ALARM_TYPE_MODULE_UNSUPPORTED,   /* Module (LM/CM) unsupported */
  L7_ALARM_TYPE_SWITCH_IMAGE_FAILURE, 
  L7_ALARM_TYPE_MODULE_MISMATCH, /* Module (LM/CM) Mismatch */
  L7_ALARM_TYPE_SLOT_POWERED_OFF, /* Slot is powered off */
  L7_ALARM_TYPE_PLUGGED_IN_MODULE_HAS_DIFFERENT_VERSION,
  L7_ALARM_TYPE_UNKNOWN    /* Source is unknown */
} L7_ALARM_TYPE_t;

#define ALARM_DEF_TABLE_NAME_MAX_LENGTH 30
#define ALARM_LOG_TABLE_NAME_MAX_LENGTH 30

typedef struct
{
  /* Def Table Name */    
  L7_uchar8 tableName[ALARM_DEF_TABLE_NAME_MAX_LENGTH];

  /* Set the alarm def table parameters in Corresponding definition 
   * table.
   */
  L7_RC_t (*alarmDefTableSet)(L7_ALARM_ID_t, L7_uint32, void *);          
     
  /* Get the alarm information from Corresponding definition table. */
  L7_RC_t (*alarmDefTableGet)(L7_ALARM_ID_t, L7_uint32, void *);
     
  /* Gets the first alarm information from Corresponding definition table. */
  L7_RC_t (*alarmDefTableGetFirst)(L7_ALARM_ID_t *);
     
  /* Gets the Next alarm information to the specified alarm information 
   * from Corresponding definition table.
   */
  L7_RC_t (*alarmDefTableGetNext)(L7_ALARM_ID_t, L7_ALARM_ID_t *);
     
  /* It sets the Log threshold from specified definition table.*/
  L7_RC_t (*alarmDefTableLogThresholdSet)(L7_ALARM_SEVERITY_t);
     
  /* It sets the Report threshold for specified definition table.*/
  L7_RC_t (*alarmDefTableReportThresholdSet)(L7_ALARM_SEVERITY_t);
     
  /* It Gets the Log threshold from specified definition table.*/
  L7_RC_t (*alarmDefTableLogThresholdGet)(L7_ALARM_SEVERITY_t *);
     
  /* It Gets the Report threshold from specified definition table.*/
  L7_RC_t (*alarmDefTableReportThresholdGet)(L7_ALARM_SEVERITY_t *);
     
  /* It returns success, if the specified alarm is defined in corresponding 
   * definition table. Otherwise, it returns not exists. 
   */
  L7_RC_t (*alarmIsIdMatched)( L7_ALARM_ID_t);

  /*It returns success, if the logging criteria matches in corresponding 
   * definition table. Otherwise, it returns false.
   */
  L7_RC_t (*alarmIsLoggingRequired)( L7_ALARM_ID_t);

  /* It returns success, if the reporting criteria matches in corresponding 
   * definition table. Otherwise, it returns false
   */
  L7_RC_t (*alarmIsReportingRequired)( L7_ALARM_ID_t);
} alarmDefinitionTableFunctions_t;

typedef struct
{
  L7_ALARM_DEFINITION_TABLE_t defTableId;
  alarmDefinitionTableFunctions_t defTableFunctions;
} alarmDefTableData_t;

typedef struct
{
  L7_char8 tableName[ALARM_LOG_TABLE_NAME_MAX_LENGTH];
     
  /* Get the Handle from Alarm ID.*/
  L7_RC_t (*alarmLogHandleGet)(L7_ALARM_ID_t, L7_uint32 *); 
     
  /* Checks the alarm Log information exists or not.*/
  L7_RC_t (*alarmLogEntryExists)(L7_uint32);
     
  /* Get the alarm Log information from Corresponding Log table.*/
  L7_RC_t (*alarmLogGet)(L7_uint32, L7_uint32, void *);

  /* Gets the first alarm log information from Corresponding log table.*/
  L7_RC_t (*alarmLogGetFirst)(L7_uint32 *);     

  /* Gets the Next alarm log information to the specified alarm log 
   * information from Corresponding log table.
   */     
  L7_RC_t (*alarmLogGetNext)(L7_uint32, L7_uint32 *);
     
  /* It Clears the corresponding log table*/
  L7_RC_t (*alarmLogClearAll)(void);

  /* It logs the alarm occurrence in corresponding Log table with date 
   * and time.
   */
  L7_RC_t (*alarmLogSet)
  (L7_ALARM_ID_t, L7_ALARM_STATUS_t, L7_clocktime, L7_uchar8*);
     
  /* It Clears the specified log entry in corresponding log table.*/
  L7_RC_t (*alarmLogClear)
  (L7_ALARM_ID_t,  L7_ALARM_STATUS_t, L7_clocktime, L7_uchar8*);
     
  /*It Gets number of alarms logged in corresponding log table.*/
  L7_RC_t (*alarmLogCountGet)(L7_uint32 *);
     
  /*It Gets maximum size of corresponding log table.*/
  L7_RC_t (*alarmLogMaxCountGet)(L7_uint32 *);
     
  /* It sets the what action needs to be taken when corresponding log table 
   * is completly filled. 
   */
  L7_RC_t (*alarmLogFullActionSet)(L7_ALARM_LOG_FULL_ACTION_t);
     
  /* It gets status of the table full action of the corresponding log 
   * table 
   */  
  L7_RC_t (*alarmLogFullActionGet)(L7_ALARM_LOG_FULL_ACTION_t *);

} alarmLogTableFunctions_t;

typedef struct
{
  L7_ALARM_LOG_TABLE_t logTableId;
  alarmLogTableFunctions_t logTableFunctions;
} alarmLogTableData_t;


typedef struct
{
   L7_ALARM_TABLE_TYPE_t alarmTableType;     
   union 
   {   
     alarmDefTableData_t alarmDefTableData;     
     alarmLogTableData_t alarmLogTableData;
   } alarmTableData;
} alarmTableCallbackFuncData_t;


/* Interface functions */

/*********************************************************************
* @purpose  This function checks whether the alarm entry is exists or not 
*           in all def tables.
*
* @param    alarmId  - @b{(input)}  Alarm ID.
*
* @returns  L7_SUCCESS - There were no errors.
* @returns  L7_NOT_EXIST   - The Alarm Id does not exists.
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionTableIsEntryExist(L7_ALARM_ID_t alarmId);


/*********************************************************************
* @purpose  This function gets the one of the parameter which is specified 
*           in 'alarmDefTableParam' from the specified def table. 
*
* @param    alarmId             - @b{(input)}  Alarm ID. 
* @param    alarmDefTableParam  - @b{(input)}  The parameter enum. 
* @param    alarmDeftEntry      - @b{(output)} The void pointer would contains 
*                                              the specified parameter of 
*                                              alarm Entry.
*
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables or 
*                             error in input param.
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionTableGet(L7_ALARM_ID_t alarmId, 
                L7_uint32 alarmDefTableParam,
                void *alarmDefEntry);

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
*                                   supported or error in input param.     
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionTableGetFirst(
                L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
                L7_ALARM_ID_t *alarmId) ;

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
*                                   supported or error in input param.     
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionTableGetNext(
                L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
                L7_ALARM_ID_t alarmId, 
                L7_ALARM_ID_t* nextAlarmId);

/*********************************************************************
* @purpose  This function sets the one of the parameter which is specified 
*           in 'alarmDefTableParam' in specified def table. 
*
* @param    alarmId             - @b{(input)}  Alarm ID. 
* @param    alarmDefTableParam  - @b{(input)}  The parameter enum. 
* @param    alarmDeftEntry      - @b{(input)}  The void pointer contains 
*                                               the specified parameter of 
*                                               alarm Entry.
*
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - The alarm Id does not exist in any def tables or 
*                           invalid input value or error in input param.
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionTableSet(L7_ALARM_ID_t alarmId,
                L7_uint32 alarmDefTableParam,
                void *alarmDefEntry);

/*********************************************************************
* @purpose  This function sets the Log threshold for the specified definition 
*           table.
*
* @param    alarmDefTableId - @b{(input)}  Def table ID. 
* @param    logThreshold    - @b{(input)}  Threshold value.
*
* @returns  L7_SUCCESS      -   There were no errors. 
* @returns  L7_FAILURE      -   The def table Id is greater than max 
*                                   supported or invalid severity value.     
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionLogThresholdSet(
                L7_ALARM_DEFINITION_TABLE_t  alarmDefTableId, 
                L7_ALARM_SEVERITY_t logThreshold);

/*********************************************************************
* @purpose  This function sets the report threshold for the specified 
*           definition table.
*
* @param    alarmDefTableId - @b{(input)}  Def table ID. 
* @param    reportThreshold - @b{(input)}  Threshold value.
*
* @returns  L7_SUCCESS      - There were no errors. 
* @returns  L7_FAILURE      - The def table Id is greater than max 
*                                   supported or invalid severity value.     
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionReportThresholdSet(
                L7_ALARM_DEFINITION_TABLE_t  alarmDefTableId, 
                L7_ALARM_SEVERITY_t reportThreshold);

/*********************************************************************
* @purpose  This function gets the report threshold from the specified 
*           definition table.
*
* @param    alarmDefTableId - @b{(input)}  Def table ID. 
* @param    reportThreshold - @b{(output)}  Threshold value.
*
* @returns  L7_SUCCESS      - There were no errors. 
* @returns  L7_FAILURE      - The def table Id is greater than max 
*                                   supported or error in input param.     
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionReportThresholdGet(
                L7_ALARM_DEFINITION_TABLE_t  alarmDefTableId, 
                L7_ALARM_SEVERITY_t *reportThreshold);

/*********************************************************************
* @purpose  This function gets the Log threshold from the specified 
*           definition table.
*           
* @param    alarmDefTableId - @b{(input)}   Def table ID. 
* @param    logThreshold    - @b{(output)}  Threshold value.
*
* @returns  L7_SUCCESS      - There were no errors. 
* @returns  L7_FAILURE      - The def table Id is greater than max 
*                                   supported or error in input param.     
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionLogThresholdGet(
                L7_ALARM_DEFINITION_TABLE_t  alarmDefTableId, 
                L7_ALARM_SEVERITY_t *logThreshold); 

/*********************************************************************
* @purpose  This function gets the First Def Table ID.
*           
* @param    alarmDefTableId - @b{(output)}  First def table ID. 
*
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - Error in input param. 
* @returns  L7_NOT_EXIST  - If def table is empty.
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionTableIdGetFirst(L7_ALARM_DEFINITION_TABLE_t 
        *alarmDefTableId);

/*********************************************************************
* @purpose  This function gets the next Def Table ID to specified def 
*           table Id.
*           
* @param    alarmCurrentDefTableId - @b{(input)}  Current def table ID. 
* @param    alarmNextDefTableId    - @b{(output)}  Next def table ID. 
*
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_FAILURE    - Error in input param. 
* @returns  L7_NOT_EXIST  - If current def id is the last entry or tabled Id 
*                             does not exist.
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionTableIdGetNext(
        L7_ALARM_DEFINITION_TABLE_t alarmCurrentDefTableId,
        L7_ALARM_DEFINITION_TABLE_t *alarmNextDefTableId); 

/*********************************************************************
* @purpose  This function gets the specified parameter from the def table data.
*           
* @param    alarmDefTableId - @b{(input)}  Def table ID. 
* @param    defTableParam   - @b{(input)}  Def table parameter. 
* @param    defTableData    - @b{(output)} The def table parameter.
*
* @returns  L7_SUCCESS      - There were no errors. 
* @returns  L7_FAILURE      - The def table Id is greater than max 
*                             supported or specified parameter is not exist or 
*                             error in input param.     
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionTableInfoGet(
            L7_ALARM_DEFINITION_TABLE_t alarmDefTableId,
            L7_ALARM_DEF_TABLE_PARAMS_t defTableParam,
            void *defTableData);

/*********************************************************************
* @purpose  This function checks whether the def table id is exists or not.
*
* @param    alarmDefTableId - @b{(input)}  Def table ID. 
*
* @returns  L7_SUCCESS    - There were no errors. 
* @returns  L7_NOT_EXIST  - The Id is not exists.
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmDefinitionTableIsIdExist(
                L7_ALARM_DEFINITION_TABLE_t alarmDefTableId);

/*********************************************************************
* @purpose  This function checks whether the handle is exists or not in 
*           specified log Table.
*
* @param    handle          - @b{(input)}  Handle.
* @param    alarmLogTableId - @b{(input)}  Log table ID.
*
* @returns  L7_SUCCESS          - There were no errors. 
* @returns  L7_NOT_EXIST        - The Entry does not exits in specified table.
* @returns  L7_NOT_SUPPORTED    - The specified table is not supported this 
*                                   operation.
* @returns  L7_FAILURE          - The log table Id is greater than max 
*                                   supported.        
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmLogTableIsEntryExist(L7_uint32 handle,
        L7_ALARM_LOG_TABLE_t  alarmLogTableId); 

/*********************************************************************
* @purpose  This function Gets the Handle associated with AlarmID.
*
* @param    alarmId         - @b{(input)}  AlarmId.
* @param    alarmLogTableId - @b{(input)}  Log table ID.
* @param    handle          - @b{(output)}  Handle.
*
* @returns  L7_SUCCESS         - There were no errors. 
* @returns  L7_NOT_EXIST       - The Entry does not exits in specified table.
* @returns  L7_NOT_SUPPORTED   - The specified table is not supported this 
*                                   operation.
* @returns  L7_FAILURE         - The log table Id is greater than max 
*                                   supported or error in input param.        
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmLogTableHandleGet(L7_ALARM_ID_t alarmId,
        L7_ALARM_LOG_TABLE_t  alarmLogTableId, L7_uint32 *handle);

/*********************************************************************
* @purpose  This function gets the one of the parameter which is specified 
*           in 'alarmLogTableParam' from the specified log table. 
*
* @param    handle             - @b{(input)}   Handle.
* @param    alarmLogTableId    - @b{(input)}   Log table ID.
* @param    alarmLogTableParam - @b{(input)}   Log table Entry parameter.
* @param    alarmLogEntry      - @b{(output)}  The void pointer would contain 
*                                              the specified parameter in 
*                                              specified alarm log table.
*      
*
* @returns  L7_SUCCESS         - There were no errors. 
* @returns  L7_NOT_SUPPORTED   - The specified table is not supported this 
*                                operation.
* @returns  L7_FAILURE         - The log table Id is greater than max 
*                                   supported or error in input param.        
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmLogTableGet(L7_uint32 handle, 
        L7_ALARM_LOG_TABLE_t  alarmLogTableId, 
        L7_uint32 alarmLogTableParam,
        void *alarmLogEntry); 

/*********************************************************************
* @purpose  This function gets the first entry in specified alarm log table.
*
* @param    alarmLogTableId - @b{(input)}   Log table ID.
* @param    handle          - @b{(output)}  The first Handle in the specified 
*                                             Log Table.
*
* @returns  L7_SUCCESS          - There were no errors. 
* @returns  L7_NOT_EXIST        - The Specified table is empty.
* @returns  L7_NOT_SUPPORTED    - The specified table is not supported this 
*                                   operation.
* @returns  L7_FAILURE          - The log table Id is greater than max 
*                                   supported or error in input param.        
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmLogTableGetFirst(
          L7_ALARM_LOG_TABLE_t alarmLogTableId, L7_uint32 *handle);

/*********************************************************************
* @purpose  This function gets the next handle to the specified handle in 
*            specified log table.
* 
* @param    alarmLogTableId - @b{(input)}  Log table ID. 
* @param    handle          - @b{(input)}  The Current handle.
* @param    nextHandle      - @b{(output)} The next handle to the specified 
*                                          handle in specified alarm log table.
*
* @returns  L7_SUCCESS          - There were no errors. 
* @returns  L7_NOT_EXIST        - The Current Handle is the last entry in 
*                                   specified log table.
* @returns  L7_NOT_SUPPORTED    - The specified table is not supported this 
*                                   operation  or handle is not 
*                                   exists in table.
* @returns  L7_FAILURE          - The log table Id is greater than max 
*                                   supported or error in input param .        
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmLogTableGetNext(
                L7_ALARM_LOG_TABLE_t alarmLogTableId, 
                L7_uint32 handle, L7_uint32 *nextHandle);

/*********************************************************************
* @purpose  This function empties the specified alarm log table.
*
* @param    alarmLogTableId - @b{(input)}  Log table ID. 
* 
* @returns  L7_SUCCESS -        There were no errors. 
* @returns  L7_NOT_SUPPORTED   - The specified table is not supported this 
*                                operation.
* @returns  L7_FAILURE         - The log table Id is greater than max 
*                                   supported.        
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmLogTableClearAll(
                L7_ALARM_LOG_TABLE_t alarmLogTableId) ;

/*********************************************************************
* @purpose  This function returns number of entries logged in specified 
*           log table.
*           
* @param    alarmLogTableId - @b{(input)}  Log table ID.
* @param    numberOfEntries - @b{(output)}  Number of Alarms Logged.
* 
* @returns  L7_SUCCESS   - There were no errors. 
* @returns  L7_FAILURE   - The log table Id is greater than max 
*                           supported or error in input param.        
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmLogTableCountGet(
                L7_ALARM_LOG_TABLE_t  alarmLogTableId,
                L7_uint32 *numberOfEntries);

/*********************************************************************
* @purpose  This function returns maximum supported number of entries in 
*           specified log table.
*           
* @param    alarmLogTableId - @b{(input)}  Log table ID.
* @param    maxEntries - @b{(output)}  Max number of Alarm Entries.
* 
* @returns  L7_SUCCESS   - There were no errors. 
* @returns  L7_FAILURE   - The log table Id is greater than max 
*                           supported or error in input param.        
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmLogTableMaxCountGet(
                L7_ALARM_LOG_TABLE_t  alarmLogTableId,
                L7_uint32 *maxEntries);

/*********************************************************************
* @purpose  This function Sets the table full action in specified 
*           Log table. The Table Full action specifies, whether the 
*           Wrapping required, when table is full or not.
*           
* @param    alarmLogTableId - @b{(input)}  Log table ID.
* @param    logFullAction   - @b{(input)}  Log table full action.
*
* @returns  L7_SUCCESS         - There were no errors. 
* @returns  L7_NOT_SUPPORTED   - The specified table is not supported this 
*                                operation.
* @returns  L7_FAILURE         - The log table Id is greater than max 
*                                   supported or error in input param.        
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmLogTableFullSet(
                L7_ALARM_LOG_TABLE_t  alarmLogTableId, 
                L7_ALARM_LOG_FULL_ACTION_t logFullAction);

/*********************************************************************
* @purpose  This function Gets the table full action from specified 
*           Log table. The Table Full action specifies, whether the 
*           Wrapping required, when table is full or not.
*           
* @param    alarmLogTableId - @b{(input)}  Log table ID.
* @param    logFullAction   - @b{(output)}  Log table full action.
*
* @returns  L7_SUCCESS          - There were no errors. 
* @returns  L7_NOT_SUPPORTED    - The specified table is not supported this 
*                                   operation.
* @returns  L7_FAILURE          - The log table Id is greater than max 
*                                   supported or error in input param.        
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmLogTableFullGet(
                L7_ALARM_LOG_TABLE_t  alarmLogTableId, 
                L7_ALARM_LOG_FULL_ACTION_t *logFullAction);

/*********************************************************************
* @purpose  This function processes alarmRaise, which is invoked by 
*           alarm users.
*
* @param    alarmId - @b{(input)}  Alarm ID.
* @param    alarmTimeStamp - @b{(input)}  Time stamp of alarm occurance.
* @param    addInfo - @b{(input)}  Additional information about alarm. This 
*                                  can be passed as L7_NULL, if there is no 
*                                  additional information.
*     
*
* @returns  L7_SUCCESS - There were no errors. 
* @returns  L7_NOT_EXIST   - Alarm ID does not exists in Definition tables.
* @returns  L7_FAILURE   - There were errors in logging the alarm or trap 
*                          processing.  
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmRaise(L7_ALARM_ID_t alarmId,
                  L7_clocktime alarmTimeStamp, L7_uchar8* addInfo);


/*********************************************************************
* @purpose  This function processes alarm Clear, which is invoked by 
*           alarm users or UI.
*
* @param    alarmId - @b{(input)}  Alarm ID.
* @param    alarmTimeStamp - @b{(input)}  Time stamp of alarm clearance.
* @param    addInfo - @b{(input)}  Additional information about alarm. This 
*                                  can be passed as L7_NULL, if there is no 
*                                  additional information.
*     
*
* @returns  L7_SUCCESS - There were no errors. 
* @returns  L7_NOT_EXIST   - Alarm ID does not exists in Definition tables.
* @returns  L7_FAILURE   - There were errors in clearing the log or trap 
*                          processing.  
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmClear(L7_ALARM_ID_t alarmId,
                         L7_clocktime alarmTimeStamp, L7_uchar8* addInfo);

/*********************************************************************
*
* @purpose  Handles messages from the configurator.
*
* @param    pCmdData - @b{(input)}  Indicates the command and request 
*                                   from the configurator
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
extern void alarmApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);


/*********************************************************************
* @purpose  Register a routine to be called when a alarm raise or clear 
*           operations.
*
* @param    registrar_ID - @b{(input)}  routine registrar id  
*                                         (See L7_COMPONENT_ID_t)
* @param    notify  -   @b{(input)}   pointer to a routine to be invoked on 
*                                    alarm operation.
*
* @returns  L7_SUCCESS  : On successful registration.
* @returns  L7_FAILURE  : If Registration ID is greater than Max supported 
*                         component Id or already callback function is 
*                         registered on same component ID.
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmCallbackRegister(L7_uint32 registrar_ID,
                             void (*notify)(L7_uint32 alarmId,
                                            L7_uint32 alarmStatus));

/*********************************************************************
* @purpose  Deregister a routine to be called when a alarm raise or clear 
*           operations.
*
* @param    registrar_ID - @b{(input)}  routine registrar id  
*                                         (See L7_COMPONENT_ID_t)
*
* @returns  L7_SUCCESS  : On successful Deregistration.
* @returns  L7_FAILURE  : If Registration ID is greater than Max supported 
*                         component Id.
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmCallbackDeregister(L7_uint32 registrar_ID);

/*********************************************************************
* @purpose  This function registers the table callback functions with 
*           Alarm Def/Log tables.
*
* @param    registrar_ID - @b{(input)}  routine registrar id  
*                                         (See L7_COMPONENT_ID_t)
* @param    alarmTableCallbackFuncdata  -   @b{(input)}   pointer to a table 
*                                              callback functions and table ID.
*
* @returns  L7_SUCCESS  : On successful registration.
* @returns  L7_FAILURE  : Table Id passed otherthan DEF or LOG Ids.
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmTableCallbackFuncsRegister(L7_uint32 registrar_ID,
                     alarmTableCallbackFuncData_t *alarmTableCallbackFuncdata);

/*********************************************************************
* @purpose  This function Deregisters the table callback functions with 
*           Alarm Def/Log tables.
*
* @param    registrar_ID - @b{(input)}  routine registrar id  
*                                         (See L7_COMPONENT_ID_t)
* @param    alarmTableCallbackFuncdata  -   @b{(input)}   pointer to table ID.
*
* @returns  L7_SUCCESS  : On successful registration.
* @returns  L7_FAILURE  : Table Id passed otherthan DEF or LOG Ids.
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t alarmMgrAlarmTableCallbackFuncsDeregister(L7_uint32 registrar_ID,
                     alarmTableCallbackFuncData_t *alarmTableCallbackFuncdata);

#endif

