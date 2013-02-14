/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename unitmgr_api.h
*
* @purpose Unit manager public defines
*
* @component unitmgr
*
* @comments none
*
* @create 12/06/2002
*
* @author djohnson
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_UNITMGR_API_H
#define INCLUDE_UNITMGR_API_H

#include "l7_common.h"
#include "nimapi.h"
#include "cda_api.h"
#include "system_exports.h"

#define L7_UNITMGR_UNITNUM_UNASSIGNED 0
#define L7_UNITMGR_MIN_UNIT_NUMBER 1
#define L7_UNITMGR_MAX_UNIT_NUMBER L7_MAX_UNITS_PER_STACK

#define L7_UNITMGR_UNIT_DESC_LEN L7_CLI_MAX_STRING_LENGTH-1

typedef enum
{
  L7_UNITMGR_MGMTPREF_DISABLED = 0,  /* can't support mgmt function */
  L7_UNITMGR_MGMTFUNC_UNASSIGNED,
  L7_UNITMGR_MGMTPREF_1,             /* supports it with pref 1 */
  L7_UNITMGR_MGMTPREF_2,
  L7_UNITMGR_MGMTPREF_3,
  L7_UNITMGR_MGMTPREF_4,
  L7_UNITMGR_MGMTPREF_5,
  L7_UNITMGR_MGMTPREF_6,
  L7_UNITMGR_MGMTPREF_7,
  L7_UNITMGR_MGMTPREF_8,
  L7_UNITMGR_MGMTPREF_9,
  L7_UNITMGR_MGMTPREF_10,
  L7_UNITMGR_MGMTPREF_11,
  L7_UNITMGR_MGMTPREF_12,
  L7_UNITMGR_MGMTPREF_13,
  L7_UNITMGR_MGMTPREF_14,
  L7_UNITMGR_MGMTPREF_15
} unitMgrMgmtPref_t;

/* Unit statuses */
typedef enum
{
  L7_UNITMGR_UNIT_OK = 1,
  L7_UNITMGR_UNIT_UNSUPPORTED,
  L7_UNITMGR_CODE_MISMATCH,
  L7_UNITMGR_CFG_MISMATCH,
  L7_UNITMGR_SDM_MISMATCH,
  L7_UNITMGR_UNIT_NOT_PRESENT,
  L7_UNITMGR_CODE_UPDATE
} unitMgrStatus_t;

/* Management flags */
typedef enum
{
  L7_UNITMGR_MGMT_ENABLED = 1, /* this unit IS the mgmt unit */
  L7_UNITMGR_MGMT_DISABLED,    /* this unit IS NOT the mgmt unit */
  L7_UNITMGR_MGMT_UNASSIGNED
} unitMgrMgmtFlag_t;

typedef enum
{
  UNITMGR_CFG_TYPE_SYSTEM = 0,      /* General end user config file */
  UNITMGR_CFG_TYPE_SUPPORT,         /* support config file */
  UNITMGR_CFG_TYPE_RUN_SYSTEM,
  UNITMGR_CFG_TYPE_LAST
} unitMgrCfgType_t;

typedef enum
{
  UNITMGR_UNIT_EVENT_JOIN,
  UNITMGR_UNIT_EVENT_LEAVE,
  UNITMGR_UNIT_EVENT_LAST
} unitMgrUnitEvents_t;

typedef enum
{
  UNITMGR_NSF_EVENT_NONE,
  UNITMGR_NSF_EVENT_ENABLE,
  UNITMGR_NSF_EVENT_DISABLE,
  UNITMGR_NSF_EVENT_LAST
} unitMgrNsfEvents_t;

typedef enum
{
  UNITMGR_USER_RESET_EVENT = 0,
  UNITMGR_STACK_MGR_CONFLICT_RESET_EVENT,
  UNITMGR_SFS_RESET_EVENT,
  UNITMGR_SDM_MISMATCH_RESET_EVENT,
  UNITMGR_RESET_EVENT_LAST
} unitMgrResetReasons_t;

typedef void (*unitNotify)(L7_uint32 unit, unitMgrUnitEvents_t event);
typedef void (*nsfNotify)(unitMgrNsfEvents_t event);

typedef struct
{
  L7_COMPONENT_IDS_t registrar_ID;
  unitNotify         notify_unit_change;
  nsfNotify          notify_nsf_change;
} unitMgrNotifyList_t;

typedef enum
{
	L7_SFS_SWITCH_STATUS_NO_ACTION,
	L7_SFS_SWITCH_STATUS_IN_PROGRESS	
}unitMgrSfsSwitchStatus_t;

typedef enum
{
	L7_SFS_NONE,
	L7_SFS_SUCCESS,
	L7_SFS_FAIL,
  L7_SFS_MIN_BOOTCODE_VERSION_NOT_PRESENT
}unitMgrSfsLastAttemptStatus_t;

/*********************************************************************
* @purpose  Gets the first member of the current stack
*
* @param    *first_unit  first unit in stack (unit number)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure or no units exist
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrStackMemberGetFirst(L7_uint32 *first_unit);

/*********************************************************************
* @purpose  Given a unit, gets the next member of the current stack
*
* @param    start_unit  initial unit number
* @param    *next_unit  next unit in stack
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if start_unit invalid or no next unit
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrStackMemberGetNext(L7_uint32 start_unit, L7_uint32 *next_unit);

/*********************************************************************
* @purpose  Gets the first preconfigured unit
*
* @param    *first_unit  first preconfigured unit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure or no units exist
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrPreCfgUnitsGetFirst(L7_uint32 *first_unit);

/*********************************************************************
* @purpose  Given a preconfigured unit number, gets the next
*           preconfigured unit
*
* @param    start_unit  initial unit number
* @param    *next_unit  next unit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if start_unit invalid or no next unit
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrPreCfgUnitsGetNext(L7_uint32 start_unit, L7_uint32 *next_unit);

/*********************************************************************
* @purpose  Given a unit, gets the unit's type
*
* @param    unit_number unit number
* @param    *unit_type  type of unit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitTypeGet(L7_uint32 unit_number, L7_uint32 *unit_type);

/*********************************************************************
* @purpose  Given a unit, gets the unit's model identifier string
*           assigned by HPC
*
* @param    unit_number unit number
* @param    *model_id   pointer to model id string storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy SYSAPI_HPC_UNIT_MODEL_SIZE bytes into *model_id
*
* @end
*********************************************************************/
L7_RC_t unitMgrModelIdentifierHPCGet(L7_uint32 unit_number, L7_uchar8 *model_id);

/*********************************************************************
* @purpose  Given a unit, gets the unit's model identifier string,
*           if any, that was preconfigured
*
* @param    unit_number unit number
* @param    *model_id   pointer to model id string storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy SYSAPI_HPC_UNIT_MODEL_SIZE bytes into *model_id
*
* @end
*********************************************************************/
L7_RC_t unitMgrModelIdentifierPreCfgGet(L7_uint32 unit_number, L7_uchar8 *model_id);

/*********************************************************************
* @purpose  Given a unit, gets the unit's status
*
* @param    unit_number unit number
* @param    *status     unit status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    status is one of unit_status_t
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitStatusGet(L7_uint32 unit_number, unitMgrStatus_t *status);

/*********************************************************************
* @purpose  Given a unit, gets the unit's management status
*
* @param    unit_number unit number
* @param    *is_mgmt    mgmt status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    status is one of unit_mgmt_status_t
*
* @end
*********************************************************************/
L7_RC_t unitMgrIsMgmtUnit(L7_uint32 unit_number, unitMgrMgmtFlag_t *is_mgmt);

/*********************************************************************
* @purpose  Given a unit, gets the unit's description string
*
* @param    unit_number unit number
* @param    *dev_desc   pointer to model id string storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy SYSAPI_HPC_UNIT_DESCRIPTION_SIZE bytes into *dev_desc
*
* @end
*********************************************************************/
L7_RC_t unitMgrDeviceDescriptionGet(L7_uint32 unit_number, L7_uchar8 *dev_desc);

/*********************************************************************
* @purpose  Given a unit, gets the unit's system identifier key
*
* @param    unit_number unit number
* @param    *key   pointer to unit key storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy L7_HPC_UNIT_ID_KEY_LEN bytes into *key
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitIdKeyGet(L7_uint32 unit_number, L7_enetMacAddr_t *key);


/*********************************************************************
* @purpose  Given a key, gets the unit number 
*
* @param    unit_key     {(input)}   Mac-address of the unit
* @param    *unit_number {(output)}  Unit number 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_key is not found in UM record
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t unitMgrKeyUnitIdGet(L7_enetMacAddr_t unit_key, L7_uint32 *unit_number);

/*********************************************************************
* @purpose  Given a unit, gets the unit's expected code version
*
* @param    unit_number unit number
* @param    *ver        version
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    Copies version information to caller
*
* @end
*********************************************************************/
L7_RC_t unitMgrExpectedCodeVerGet(L7_uint32 unit_number, codeVersion_t *ver);

/*********************************************************************
* @purpose  Given a unit, gets the unit's uptime in seconds
*
* @param    unit_number unit number
* @param    *uptime     uptime in seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitUpTimeGet(L7_uint32 unit_number, L7_uint32 *uptime);

/*********************************************************************
* @purpose  Given a unit, gets the unit's hardware management preference
*
* @param    unit_number unit number
* @param    *hw_pref    preference to become a management unit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    *hw_pref is one of unitMgrMgmtPref_t
*
* @end
*********************************************************************/
L7_RC_t unitMgrHwMgmtPrefGet(L7_uint32 unit_number, unitMgrMgmtPref_t *hw_pref);

/*********************************************************************
* @purpose  Given a unit, gets the unit's admin management preference
*
* @param    unit_number unit number
* @param    *admin_pref preference to become a management unit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    *admin_pref is one of unitmgr_mgmt_pref_t
*
* @end
*********************************************************************/
L7_RC_t unitMgrAdminMgmtPrefGet(L7_uint32 unit_number, unitMgrMgmtPref_t *admin_pref);

/*********************************************************************
* @purpose  Given a unit, gets the unit's running version of code
*
* @param    unit_number unit number
* @param    *ver        version
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrDetectCodeVerRunningGet(L7_uint32 unit_number, codeVersion_t *ver);

/*********************************************************************
* @purpose  Given a unit, gets the unit's serial number
*
* @param    unit_number unit number
* @param    *serial_number  Serial Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrSerialNumberGet(L7_uint32 unit_number, L7_uchar8 *serial_number);

/*********************************************************************
* @purpose  Given a unit, gets the unit's Asset-tag
*
* @param    unit_number unit number
* @param    *asset_tag  Unit's Asset-tag
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t unitMgrDeviceAssetTagGet(L7_uint32 unit_number, L7_uchar8 *asset_tag);

/*********************************************************************
* @purpose  Given a unit, sets the unit's Asset-Tag
*
* @param    unit_number        unit number
* @param    *asset_tag   Pointer to unit's Asset-tag
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy up to L7_SYSMGMT_ASSETTAG_MAX bytes into *asset_Tag
*
* @end
*********************************************************************/
L7_RC_t unitMgrDeviceAssetTagSet(L7_uint32 unit_number, L7_uchar8 *asset_tag);

/*********************************************************************
* @purpose  Given a unit, gets the service tag
*
* @param    unit_number        unit number
* @param    *service_tag   Pointer to unit's service-tag
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t unitMgrDeviceServiceTagGet(L7_uint32 unit_number, L7_uchar8 *service_tag);

/*********************************************************************
* @purpose  Given a unit, gets the unit's operating system description
*
* @param    unit_number unit number
* @param    *os_descr   Operating System Description
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrOsDescriptionGet(L7_uint32 unit_number, L7_uchar8 *os_descr);

/*********************************************************************
* @purpose  Utility to expand the version number to a more readable form
*
* @param    unit_number  unit number
* @param    *rel         release
* @param    *ver         version
* @param    *maint_level maint_level
* @param    *build_num   build number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void unitMgrExpandVersion(L7_uint32 version, L7_uint32 *rel, L7_uint32 *ver, L7_uint32 *maint_level, L7_uint32 *build_num);

/*********************************************************************
* @purpose  Given a unit, gets the unit's version of code in flash
*
* @param    unit_number unit number
* @param    *ver        version
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrDetectCodeVerFlashGet(L7_uint32 unit_number, codeVersion_t *ver);

/*********************************************************************
* @purpose  Clears config of current unit
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    disassociate a unit type from a specific unit number
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitTypeClear(void);

L7_RC_t unitMgrUnitNumberAssign(L7_uint32 unit_number);

/*********************************************************************
* @purpose  Will create a new unit record for unit unit_number
*
* @param    unit_number     unit number
* @param    unit_index      hpc unit index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    If there are unconfigured unit openings in the stack
*           this will grab one of them and assign it unit_number
*
* @end
*********************************************************************/
L7_RC_t unitMgrPreConfigUnitNumber(L7_uint32 unit_number, L7_uint32 unit_index);

/*********************************************************************
* @purpose  Changes a unit's number
*
* @param    unit_number     unit number to change
* @param    new_unit_number new unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    The unit could be currently in the stack or a preconfigured
*           unit, manager or not
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitNumberChange(L7_uint32 unit_number, L7_uint32 new_unit_number);

/*********************************************************************
* @purpose  Set the admin management preference
*
* @param    unit_number unit number
* @param    admin_pref  admin preference
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrAdminMgmtPrefSet(L7_uint32 unit_number, unitMgrMgmtPref_t admin_pref);

/*********************************************************************
* @purpose  Allows the user to manually enable or disable the management
*           function on this unit
*
* @param    mgmt_func  L7_UNITMGR_MGMT_ENABLED or DISABLED
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrMgmtFuncSet(unitMgrMgmtFlag_t mgmt_func);

/*********************************************************************
* @purpose  Transfers the management function from the current unit
*           to another
*
* @param    target_unit destination unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrTransferMgmt(L7_uint32 target_unit);

/*********************************************************************
* @purpose  Administratively failover to the backup unit.
*
* @param    none
*
* @returns  If call is successful, this function never returns
*           L7_FAILURE  if an error
*           L7_REQUEST_DENIED if the system is not ready for a warm restart
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrFailoverInitiate(void);

/*********************************************************************
* @purpose  Administratively enable or disable nonstop forwarding.
*
* @param    nsf - L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if value of nsf is not L7_ENABLE or L7_DISABLE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrNsfAdminSet(L7_uint32 nsf);

/*********************************************************************
* @purpose  Gets the administrative status of nonstop forwarding
*
* @param    void
*
* @returns  L7_ENABLE or L7_DISABLE
*
* @notes    This value does not take into account the operational status of NSF.
*
* @end
*********************************************************************/
L7_uint32 unitMgrNsfAdminGet(void);

/*********************************************************************
* @purpose  Gets the operational status of nonstop forwarding
*
* @param    void
*
* @returns  L7_ENABLE or L7_DISABLE
*
* @notes    NSF can only be operationally enabled if it is administratively
*           enabled and every unit in the stack supports NSF.
*
* @end
*********************************************************************/
L7_uint32 unitMgrNsfOperStatusGet(void);

/*********************************************************************
* @purpose  Get the reason for the last restart.
*
* @param    void
*
* @returns  One of L7_LAST_STARTUP_REASON_t
*
* @notes    The type of activation that caused the software to start the last
*           time.
*
* @end
*********************************************************************/
L7_LAST_STARTUP_REASON_t unitMgrStartupReasonGet(void);

/*********************************************************************
* @purpose  Convert a startup reason to a string.
*
* @param    reason - a startup reason
* @param    reasonStr - text description of the reason
*
* @returns  L7_SUCCESS
*
* @notes    reasonStr buffer must be at least L7_STARTUP_REASON_STR_LEN.
*
* @end
*********************************************************************/
L7_RC_t unitMgrStartupReasonStrGet(L7_LAST_STARTUP_REASON_t reason,
                                   L7_uchar8 *reasonStr);

/*********************************************************************
* @purpose  Get the number of seconds since the management unit took over
*           as manager.
*
* @param    void
*
* @returns  number of seconds since mgr became mgr.
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 unitMgrTimeSinceLastRestart(void);

/*********************************************************************
* @purpose  Determine whether a restart is in progress.
*
* @param    void
*
* @returns  L7_TRUE if a restart is in progress
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL unitMgrRestartInProgress(void);

/*********************************************************************
* @purpose  Get the age of the configuration on the backup unit.
*
* @param    age - output - the number of seconds since the management unit last
*                          copied the configuration to the backup unit.
*
* @returns  L7_SUCCESS if age set
*           L7_NOT_EXIST if running configuration has not yet been copied to
*                        the backup unit. In this case, age set to 2^32 - 1.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t unitMgrConfigAgeOnBackup(L7_uint32 *age);

/*********************************************************************
* @purpose  Get how long before the running config will next be
*           copied to the backup unit, if a copy is scheduled.
*
* @param    wait - Number of seconds until config will be copied.
*
* @returns  L7_SUCCESS if a config copy is pending.
*           L7_FAILURE otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t unitMgrNextConfigCopyTime(L7_uint32 *wait);

/*********************************************************************
* @purpose  Get the status of copying the running config to the backup unit.
*
* @param    void
*
* @returns  running config status on backup
*
* @notes
*
* @end
*********************************************************************/
L7_RUN_CFG_STATUS_t unitMgrRunCfgStatusGet(void);

/*********************************************************************
* @purpose  Determine whether a given unit supports nonstop forwarding.
*
* @param    unit - unit number
* @param    nsfSupport - (output)  set to L7_TRUE if unit supports NSF
*
* @returns  L7_SUCCESS if unit found
*           L7_FAILURE otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t unitMgrNsfSupportGet(L7_uint32 unit, L7_BOOL *nsfSupport);

/*********************************************************************
* @purpose  Replicate configuration from mangement unit to target unit
*
* @param    target_unit destination unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrReplicateCfg(L7_uint32 target_unit);

/*********************************************************************
* @purpose  Trigger copy of running configuration from management unit
*           to the backup unit.
*
* @param    immediate - L7_TRUE if the running configuration should be saved
*                 immediately, regardless of the delay and hold timers.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrSendRunCfg(L7_BOOL immediate);

/*********************************************************************
* @purpose  Replicate operational code from mangement unit to target unit
*
* @param    target_unit destination unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrReplicateOpr(L7_uint32 target_unit);

/*********************************************************************
* @purpose  Reset all units in the stack
*
* @param    reset_reason {(input)} Reason for initiating the reset
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrResetSystem(unitMgrResetReasons_t reset_reason);

/*********************************************************************
* @purpose  Reset a specified unit in the stack
*
* @param    unit_number unit to reset
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrResetUnit(L7_uint32 unit_number);

/*********************************************************************
* @purpose  Clear the local unit's configuration
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrResetUnitCfg(void);


/*********************************************************************
* @purpose  Get the manager's unit number
*
* @param    *unit_number manager's unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no manager exists
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrMgrNumberGet(L7_uint32 *unit_number);


/*********************************************************************
* @purpose  Get the manager's key
*
* @param    *unit_number manager's key
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no manager exists
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrMgrKeyGet(L7_enetMacAddr_t *mgmt_key);

/*********************************************************************
* @purpose  Obtain the unit number for the backup manager
*
* @param    *backup_unit_number  pointer to return unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrBackupNumberGet(L7_uint32 *backup_unit_number);

/*********************************************************************
* @purpose  Get this unit's number
*
* @param    *unit_number this unit's number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrNumberGet(L7_uint32 *unit_number);

/*********************************************************************
* @purpose  Given a unit, sets the unit's description string
*
* @param    unit_number        unit number
* @param    *unit_description  pointer to model id string storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy up to L7_UNITMGR_UNIT_DESC_LEN bytes into *dev_desc
*
* @end
*********************************************************************/
L7_RC_t unitMgrDeviceDescriptionSet(L7_uint32 unit_number, L7_uchar8 *unit_description);

/*********************************************************************
* @purpose  Will create a new unit record for unit unit_number
*
* @param    unit_number     unit number to change
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    If there are unconfigured unit openings in the stack
*           this will grab one of them and assign it unit_number
*
* @end
*********************************************************************/
L7_RC_t unitMgrCreateUnit(L7_uint32 unit_number);

/*********************************************************************
* @purpose  Will remove a unit's preconfiged record or remove it from the stack
*
* @param    unit_number     unit number to remove
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t unitMgrDeleteUnit(L7_uint32 unit_number);

/*********************************************************************
* @purpose  Gets the MAC address for a port
*
* @param  type        @b{(input)}     interface type of the interface
* @param  unit        @b{(input)}     unit id of the interface
* @param  slot        @b{(input)}     slot id of the interface
* @param  port        @b{(input)}     port id of the interface
* @param  l2_mac_addr @b{(output)}    L2 MAC address of the interface
* @param  l3_mac_addr @b{(output)}    L3 MAC address of the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, failure in getting the remote info or
*                       a bad slot.port
*
* @notes    Could be a local or remote unit
*
* @end
*********************************************************************/
L7_RC_t unitMgrIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 unit,L7_uint32 slot,L7_uint32 port, L7_uchar8 *l2_mac_addr, L7_uchar8 *l3_mac_addr);

void umUpdateLocalUnit(void);

/*********************************************************************
* @purpose  Generate a unit update
*
* @param    void
*
* @returns  void
*
* @notes    Invoked after CDA transfers code or when code version
*           information needs updated
*
* @end
*
*********************************************************************/
L7_RC_t unitMgrGenUpdate(void);



/*********************************************************************
* @purpose  Logs a message to the management unit
*
* @param   *file        @b{(input)} pointer to file name
* @param   line         @b{(input)} line number
* @param   severity     @b{(input)} message severity
* @param   component    @b{(input)} component ID
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void unitMgrLogMsg(L7_char8 *file, L7_ulong32 line, L7_char8 *log_buf,
                   L7_LOG_SEVERITY_t severity, L7_COMPONENT_IDS_t component);


/*********************************************************************
* @purpose  Attempts to get the eventlog from another unit
*
* @param    unit  unit number
*
* @returns  L7_SUCCESS, if log retreived
*
* @notes    Saves the eventlog locally in flash as log-#.bin
*
* @end
*********************************************************************/
L7_RC_t unitMgrEventLogGet(L7_uint32 unit);



/*********************************************************************
* @purpose      Gets the next entry from the event log
*
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 ptr       @b{(input)} Index to the next event log entry or NULL if requesting the first
* @param L7_uint32 *pstring  @b{(input)} Ptr to place the formatted event log requested
*
* @returns      index to the next event log entry or NULL if last
*
* @notes none
*
* @end
*
*********************************************************************/
L7_uint32 unitMgrEventLogEntryNextGet(L7_uint32 UnitIndex, L7_uint32 ptr, L7_char8 *pstring);

/*********************************************************************
* @purpose  Will propagate configuration files to other connected units
*
* @param    cfg_type        Type of config file being propagated
*
* @returns  L7_SUCCESS, if config transfer was successfull
*           L7_FAILURE, if config transfer timed out
*
* @notes    Broadcasts a start-config-download message to all units
*           in the stack.  Connected units will respond by FFTPing
*           a list of config files from the management unit.  Config
*           files consist of fastpath.cfg and security key files.
*
* @end
*********************************************************************/
L7_RC_t unitMgrPropagateCfg(unitMgrCfgType_t cfg_type);

/*********************************************************************
* @purpose  Generates event to update this units record in the avl tree
*
* @param    void
*
* @returns  void
*
* @notes    Called after CDA update to get new version info
*
* @end
*********************************************************************/
void unitMgrUpdateUnit(void);

/*********************************************************************
* @purpose  Retrieves the version information for a given image
*
* @param    unit         @b{(input)}  Unit number for this request
* @param    image        @b{(input)}  image to associate the text
* @param    descr        @b{(output)} version string
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_NOT_EXIST  if the specified image could not be found
* @returns  L7_FAILURE    on FS errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  unitMgrImageVersionGet(L7_uint32 unit, L7_char8 *image,
    L7_char8 *version);

/*********************************************************************
* @purpose  Retrieves the boot image version information for a given image
*
* @param    unit         @b{(input)}  Unit number for this request
* @param    image        @b{(input)}  image to associate the text
* @param    descr        @b{(output)} modification time string
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_FAILURE    if the specified unit/image could not be found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t unitMgrBootImageVersionGet(L7_uint32 unit, L7_char8 *image,
                                   L7_char8 *descr);

/*********************************************************************
* @purpose  Returns the active image file name
*
* @param    unit        @b{(input)}  Unit for this operation.
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t unitMgrActiveImageNameGet(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Returns the backup image file name
*
* @param    unit        @b{(input)}  Unit for this operation.
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t unitMgrBackupImageNameGet(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Returns the activated image file name
*
* @param    unit        @b{(input)}  Unit for this operation.
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t unitMgrActivatedImageNameGet(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Will propagate configuration files from standby CFM to
*           active CFM
*
* @param    void
*
* @returns  L7_SUCCESS, if config transfer was successfull
*           L7_FAILURE, if config transfer timed out
*
* @notes    Broadcasts a start-config-download message to all units
*           in the stack.  Connected units will respond by FFTPing
*           a list of config files from the management unit.  Config
*           files consist of fastpath.cfg and security key files.
*
* @end
*********************************************************************/
L7_RC_t unitMgrPropagateCfg_SCFM2ACFM(void);

/*********************************************************************
* @purpose  Examines two code versions for differences
*
* @param    ver1    first version
* @param    ver2    second version
*
* @returns  L7_TRUE, if the versions don't match
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL unitMgrVersionMismatch(codeVersion_t ver1, codeVersion_t ver2);

/*********************************************************************
* @purpose  Sets the unit as the standby of the stack
*
* @param    standy_unit standby unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if target_unit invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrStandbySet(L7_uint32 standby_unit, L7_BOOL mode);

/*********************************************************************
* @purpose  Gets the unit number of the standby in the stack
*
* @param    *standy_unit  Pointer to the standby unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if standby invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrStandbyGet(L7_uint32 *standby_unit, L7_BOOL *admin_mode);
L7_RC_t unitMgrCfgStandbyGet(L7_uint32 *cfg_standby_unit);

L7_BOOL unitMgrSwitchUnconfigInProgress(void);
L7_BOOL umSwitchoverConditionGet();


/*********************************************************************
 * @purpose  Get the Unit's UM protocol version
 *
 * @param    unit_number {(input)} Id of the unit
 * @param    version     {(output)} UM version
 *
 * @returns  L7_SUCCESS - Unit could be found
 *           L7_FAILURE - Unit record does not exist
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrUnitUMVersionGet(L7_uint32 unit_number, L7_uint32 *version);

extern  L7_BOOL standby_debug_enable;
#define STANDBY_DEBUG(format, args...) if (standby_debug_enable)SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "<ts=%d:tm=%d>%s[%d]: "format, osapiUpTimeRaw(),osapiTimeMillisecondsGet(),__FUNCTION__,__LINE__,##args)
#define STANDBY_DEBUG_D(format, args...) if (standby_debug_enable) SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "<ts=%d:tm=%d>%s[%d]: "format, osapiUpTimeRaw(),osapiTimeMillisecondsGet(),__FUNCTION__,__LINE__,##args);osapiSleepMSec(20)

/*********************************************************************
* @purpose  Register a routine to be called when a unit state changes.
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
* @param    *notify        pointer to a routine to be invoked for unit state
*                          changes.  Each routine has the following parameters:
*                          (unit number, event(    etc.)).
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitChangeRegister(L7_COMPONENT_IDS_t registrar_ID, unitNotify notify);

/*********************************************************************
* @purpose  De-Register a routine to be called when a unit state changes.
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitChangeDeRegister(L7_COMPONENT_IDS_t registrar_ID);

/*********************************************************************
* @purpose  Set the reason for starting the management software
*
* @param    startupReason {(input)} This is set by CM/Driver when
*                                   they fail to do warm start
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void unitMgrSystemStartupReasonSet(L7_LAST_STARTUP_REASON_t startupReason);

/*********************************************************************
* @purpose  Notify the unitMgr that the driver restart has completed
*
* @param    reason         Indicates cold/warm restart
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrStartupCompleteNotify(L7_LAST_STARTUP_REASON_t reason);

#if L7_FEAT_CHASSIS_SERVICE_TAG
/*********************************************************************
* @purpose  Given a unit, gets the chassis service tag
*
* @param    unit_number      unit number
* @param    *unit_assettag   Pointer to unit's service-tag
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t UnitMgrDeviceChassisServiceTagGet(L7_uint32 unit_number, L7_uchar8 *service_tag);
#endif

#if (L7_FEAT_DNI8541_BLADESERVER==1)
/*********************************************************************
* @purpose  Given a unit, gets the chassis slot id
*
* @param    unit_number      unit number
* @param    *slotId          Pointer to unit's slot id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t UnitMgrDeviceChassisSlotIdGet(L7_uint32 unit_number, L7_uchar8 *slotId);

/*********************************************************************
* @purpose  Given a unit and slot number, gets the card presence
*
* @param    unit_number      unit number
* @param    slotNum          slot number
* @param    *cardPresent     Pointer to card presence in a slot
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t UnitMgrDeviceSlotIdGet(L7_uint32 unit_number, L7_uint32 slotNum,
                               L7_BOOL *cardPresent);
#endif


#ifdef L7_NSF_PACKAGE
/*********************************************************************
* @purpose  Register a routine to be called when the NSF state changes.
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
* @param    *notify        pointer to a routine to be invoked for NSF state
*                          changes.  Each routine has the following parameters:
*                          (unit number, event(    etc.)).
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrNsfChangeRegister(L7_COMPONENT_IDS_t registrar_ID, nsfNotify notify);

/*********************************************************************
* @purpose  De-Register a routine to be called when the nsf state changes.
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrNsfChangeDeRegister(L7_COMPONENT_IDS_t registrar_ID);
#endif


/*********************************************************************
 * @purpose  Get Stack Firmware synchronication status
 *
 * @param    *status    Pointer to the SFS status
 *
 * @returns  L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncModeGet(L7_uint32 *status);

/*********************************************************************
 * @purpose  Set Stack Firmware synchronication status
 *
 * @param    status    SFS status to be set.Valid values are
 *                     L7_ENABLE/L7_DISABLE
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Get Stack Firmware synchronication Trap status
 *
 * @param    *status    Pointer to the SFS Trap status
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncTrapModeGet(L7_uint32* status);

/*********************************************************************
 * @purpose  Set Stack Firmware synchronication Trap status
 *
 * @param    status    SFS Trap status to be set.Valid values are
 *                     L7_ENABLE/L7_DISABLE
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncTrapModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Get Stack Firmware synchronication Allow downgrade mode
 *
 * @param    *status    Pointer to the SFS Allow downgrade mode
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncAllowDowngradeModeGet(L7_uint32* status);

/*********************************************************************
 * @purpose  Set Stack Firmware Synchronization downgrade mode
 *
 * @param    mode    Allow downgrade mode.Possible values are
 *                     L7_ENABLE/L7_DISABLE
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncAllowDowngradeModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Get Stack Firmware synchronication switch status for a unit in stack
 *
 * @param    unit    unit for which the status needs to be get
 *
 * @param    *status Status pointer  
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncMemberStatusGet(L7_uint32 unit,L7_uint32* status);

/*********************************************************************
 * @purpose  Get Stack Firmware synchronication switch status for the whole stack
 *
 * @param    *status    Pointer to the SFS switch status
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
 L7_RC_t unitMgrStackFirmwareSyncStatusGet(L7_uint32* status);


/*********************************************************************
 * @purpose  Get the SFS last attempt status for a given unit
 *
 * @param    unit    unit for which the status needs to be get
 *
 * @param    *status Status pointer   
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncMemberLastAttemptStatusGet(L7_uint32 unit,
                                                           L7_uint32* status);

#endif  /* INCLUDE_UNITMGR_API_H */
