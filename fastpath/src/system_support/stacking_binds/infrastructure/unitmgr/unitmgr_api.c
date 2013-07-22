
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename unitmgr_api.c
*
* @purpose Unit manager APIs
*      This UM is used for the stand-alone version of the build.
*
* @component unitmgr
*
* @comments none
*
* @create 7/15/2003
*
* @author djohnson, atsigler
*
* @end
*
**********************************************************************/
#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "unitmgr_api.h"
#include "unitmgr.h"
#include "log.h"
#include "log_ext.h"
#include "bspapi.h"
#include "sysapi_hpc.h"
#include "system_exports.h"
#include "defaultconfig.h"

#define UM_DEFAULT_UNIT   1

/*********************************************************************
* @purpose  Gets the first member of the current stack
*
* @param    *first_unit  first unit in stack
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure or no units exist
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrStackMemberGetFirst(L7_uint32 *first_unit)
{
  *first_unit = UM_DEFAULT_UNIT;
  return L7_SUCCESS;
}

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
L7_RC_t unitMgrStackMemberGetNext(L7_uint32 start_unit, L7_uint32 *next_unit)
{
  if (start_unit < UM_DEFAULT_UNIT)
  {
    *next_unit = UM_DEFAULT_UNIT;
    return L7_SUCCESS;
  } else
  {
    return L7_FAILURE;
  }
}


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
L7_RC_t unitMgrUnitTypeGet(L7_uint32 unit_number, L7_uint32 *unit_type)
{
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *hpc;

  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }
  hpc = sysapiHpcLocalUnitDbEntryGet ();
  *unit_type = hpc->unitTypeId;

  return L7_SUCCESS;
}

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
L7_RC_t unitMgrModelIdentifierHPCGet(L7_uint32 unit_number, L7_uchar8 *model_id)
{
  L7_RC_t rc;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *p;

  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }
  p = sysapiHpcLocalUnitDbEntryGet();
  if (p != L7_NULLPTR)
  {
    memcpy(model_id, p->unitModel, SYSAPI_HPC_UNIT_MODEL_SIZE);
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

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
L7_RC_t unitMgrModelIdentifierPreCfgGet(L7_uint32 unit_number, L7_uchar8 *model_id)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrUnitStatusGet(L7_uint32 unit_number, unitMgrStatus_t *status)
{
  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }

  *status = L7_UNITMGR_UNIT_OK;

  return L7_SUCCESS;
}

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
L7_RC_t unitMgrIsMgmtUnit(L7_uint32 unit_number, unitMgrMgmtFlag_t *is_mgmt)
{
  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }
  *is_mgmt = L7_UNITMGR_MGMT_ENABLED;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a unit, gets the unit's description string
*
* @param    unit_number unit number
* @param    *dev_desc   pointer to model id string storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy L7_UNITMGR_UNIT_DESC_LEN bytes into *dev_desc
*
* @end
*********************************************************************/
L7_RC_t unitMgrDeviceDescriptionGet(L7_uint32 unit_number, L7_uchar8 *dev_desc)
{
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *hpc;

  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }
  hpc = sysapiHpcLocalUnitDbEntryGet ();

  memcpy((void*)dev_desc, hpc->unitDescription, L7_CLI_MAX_STRING_LENGTH-1);

  return L7_SUCCESS;
}

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
L7_RC_t unitMgrUnitIdKeyGet(L7_uint32 unit_number, L7_enetMacAddr_t *key)
{
  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }
  if (sysapiHpcLocalUnitIdentifierMacGet(key) == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}


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
L7_RC_t unitMgrKeyUnitIdGet(L7_enetMacAddr_t unit_key, L7_uint32 *unit_number)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrExpectedCodeVerGet(L7_uint32 unit_number, codeVersion_t *ver)
{
  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }

  ver->rel = 0;
  ver->ver = 0;
  ver->maint_level = 0;
  ver->build_num = 0;
  ver->valid = L7_TRUE;

  return L7_SUCCESS;
}

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
L7_RC_t unitMgrUnitUpTimeGet(L7_uint32 unit_number, L7_uint32 *uptime)
{
  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }

  *uptime = osapiUpTimeRaw();

  return L7_SUCCESS;
}

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
L7_RC_t unitMgrHwMgmtPrefGet(L7_uint32 unit_number, unitMgrMgmtPref_t *hw_pref)
{
  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }

  *hw_pref = L7_UNITMGR_MGMTPREF_8;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a unit, gets the unit's admin management preference
*
* @param    unit_number unit number
* @param    *admin_pref preference to become a management unit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    *admin_pref is one of unitMgrMgmtPref_t
*
* @end
*********************************************************************/
L7_RC_t unitMgrAdminMgmtPrefGet(L7_uint32 unit_number, unitMgrMgmtPref_t *admin_pref)
{
  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }

  *admin_pref = L7_UNITMGR_MGMTPREF_8;

  return L7_SUCCESS;
}

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
L7_RC_t unitMgrDetectCodeVerRunningGet(L7_uint32 unit_number, codeVersion_t *ver)
{
  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }

  ver->rel = 0;
  ver->ver = 0;
  ver->maint_level = 0;
  ver->build_num = 0;
  ver->valid = L7_TRUE;

  return L7_SUCCESS;
}

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
L7_RC_t unitMgrSerialNumberGet(L7_uint32 unit_number, L7_uchar8 *serial_number)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrDeviceAssetTagGet(L7_uint32 unit_number, L7_uchar8 *asset_tag)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrDeviceAssetTagSet(L7_uint32 unit_number, L7_uchar8 *asset_tag)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrDeviceServiceTagGet(L7_uint32 unit_number, L7_uchar8 *service_tag)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrOsDescriptionGet(L7_uint32 unit_number, L7_uchar8 *os_descr)
{
  return L7_FAILURE;
}

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
void unitMgrExpandVersion(L7_uint32 version, L7_uint32 *rel, L7_uint32 *ver, L7_uint32 *maint_level, L7_uint32 *build_num)
{
  *rel = (version & 0xff000000) >> 24;
  *ver = (version & 0x00ff0000) >> 16;
  *maint_level = (version & 0x0000ff00) >> 8;
  *build_num = (version & 0x000000ff);
}

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
L7_RC_t unitMgrDetectCodeVerFlashGet(L7_uint32 unit_number, codeVersion_t *ver)
{
  if (unit_number != UM_DEFAULT_UNIT)
  {
    return L7_FAILURE;
  }

  ver->rel = 0;
  ver->ver = 0;
  ver->maint_level = 0;
  ver->build_num = 0;
  ver->valid = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Assigns or changes a unit type to the current unit or
*           a preconfigured unit
*
* @param    unit_number unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid or type unsupported
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitTypeAssign(L7_uint32 unit_number, L7_uint32 unit_type)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrUnitTypeClear(void)
{
  return L7_FAILURE;
}

L7_RC_t unitMgrUnitNumberAssign(L7_uint32 unit_number)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrPreConfigUnitNumber(L7_uint32 unit_number, L7_uint32 unit_index)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrUnitNumberChange(L7_uint32 unit_number, L7_uint32 new_unit_number)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrAdminMgmtPrefSet(L7_uint32 unit_number, unitMgrMgmtPref_t admin_pref)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrMgmtFuncSet(unitMgrMgmtFlag_t mgmt_func)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Transfers the management function from the current unit
*           to another
*
* @param    target_unit destination unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if target_unit invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrTransferMgmt(L7_uint32 target_unit)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrReplicateCfg(L7_uint32 target_unit)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrReplicateOpr(L7_uint32 target_unit)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrResetSystem(unitMgrResetReasons_t reset_reason)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrResetUnit(L7_uint32 unit_number)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrResetUnitCfg(void)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrMgrNumberGet(L7_uint32 *unit_number)
{
  *unit_number = UM_DEFAULT_UNIT;
  return L7_SUCCESS;
}

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
L7_RC_t unitMgrNumberGet(L7_uint32 *unit_number)
{
  *unit_number = UM_DEFAULT_UNIT;

  return L7_SUCCESS;
}

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
L7_RC_t unitMgrDeviceDescriptionSet(L7_uint32 unit_number, L7_uchar8 *unit_description)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrCreateUnit(L7_uint32 unit_number)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrDeleteUnit(L7_uint32 unit_number)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 unit,L7_uint32 slot,L7_uint32 port, L7_uchar8 *l2_mac_addr, L7_uchar8 *l3_mac_addr)
{
  L7_RC_t rc;

  rc = sysapiHpcIfaceMacGet(type, slot, port, l2_mac_addr, l3_mac_addr);
  return rc;
}

/*********************************************************************
* @purpose  Logs a message to the management unit
*
* @param  msg         @b{(input)}     message to be logged
*
* @returns  none
*
* @notes    Could be a local or remote unit
*
* @end
*********************************************************************/
void unitMgrLogMsg(char *file, unsigned long line, char *log_buf,
                   L7_LOG_SEVERITY_t severity, L7_COMPONENT_IDS_t component)
{
  return;
}


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
L7_RC_t unitMgrEventLogGet(L7_uint32 unit)
{
  return L7_SUCCESS;
}


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
L7_uint32 unitMgrEventLogEntryNextGet(L7_uint32 UnitIndex, L7_uint32 ptr, L7_char8 *pstring)
{
  return L7_event_log_get_next(L7_NULLPTR, ptr, pstring);
}


/*********************************************************************
* @purpose  Will propagate configuration files to other connected units
*
* @param    cfg_type        Type of config file being propagated
* 
* @returns  L7_SUCCESS, if config transfer was successful
*           L7_FAILURE, if config transfer timed out
*
* @notes    Broadcasts a start-config-download message to all units
*           in the stack.  Connected units will respond by FFTPing
*           a list of config files from the management unit.  Config
*           files consist of fastpath.cfg and security key files.
*
* @end
*********************************************************************/
L7_RC_t unitMgrPropagateCfg(unitMgrCfgType_t cfg_type)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the version information for a given image
*           
* @param    unit         @b{(input)}  Unit number for this request
* @param    image        @b{(input)}  image to associate the text
* @param    descr        @b{(output)} version string
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_FAILURE    if the specified unit/image could not be found
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t  unitMgrImageVersionGet(L7_uint32 unit, L7_char8 *image, 
    L7_char8 *descr)
{
  sprintf(descr, "%d.%d.%d.%d", 0,0,0,0);

  return L7_SUCCESS;
  
}

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
                                   L7_char8 *descr)
{
	sprintf(descr, "%s", "Not Available");
	return L7_SUCCESS;
}

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
L7_RC_t unitMgrActiveImageNameGet(L7_uint32 unit, L7_char8 *fileName)
{
  return bspapiActiveImageFileNameGet(fileName);
}

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
L7_RC_t unitMgrBackupImageNameGet(L7_uint32 unit, L7_char8 *fileName)
{
  return bspapiBackupImageFileNameGet(fileName);
}

/*********************************************************************
* @purpose  Returns the activated image file name
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_FAILURE  
* @notes    
*
* @end
*********************************************************************/
L7_RC_t unitMgrActivatedImageNameGet(L7_uint32 unit, L7_char8 *fileName)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Generates and broadcasts a message to signal the start of
*           config file transfer from Standby CFM to active CFM
*
* @param    void
*
* @returns  void
*
* @notes    Only invoked on management unit, waits for completion or timeout
*
* @end
*********************************************************************/
L7_RC_t unitMgrPropagateCfg_SCFM2ACFM(void)
{
  return L7_FAILURE;
}

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
L7_RC_t unitMgrStandbySet(L7_uint32 standby_unit, L7_BOOL mode)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the unit number of the standby in the stack
*
* @param    *standy_unit Pointer to the standby unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if standby unit is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrStandbyGet(L7_uint32 *standby_unit, L7_BOOL *admin_mode)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the unit number of the user configured standby in the stack
*
* @param    *cfg_standy_unit Pointer to the user cfg standby unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if standby unit is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrCfgStandbyGet(L7_uint32 *cfg_standby_unit)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  If a standby has become a master, then a swicthover condition exisits
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    This rotuine will return TRUE for a brief period after swictover.
*           After 10 discovery pdu reception this variable will be reset.
*
* @end
*********************************************************************/
L7_BOOL umSwitchoverConditionGet()
{
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Administratively failover to the backup unit.
*
* @param    none
*
* @returns  If call is successful, this function never returns
*           L7_FAILURE  if an error
*           L7_REQUEST_DENIED if the system is not ready for a warm restart
*
* @notes    stubbed out for non-stacking build.
*
* @end
*********************************************************************/
L7_RC_t unitMgrFailoverInitiate(void)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Administratively enable or disable nonstop forwarding.
*
* @param    nsf - L7_ENABLE or L7_DISABLE
*
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrNsfAdminSet(L7_uint32 nsf)
{
  return L7_FAILURE;
}

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
L7_uint32 unitMgrNsfAdminGet(void)
{
  return L7_DISABLE;
}

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
L7_uint32 unitMgrNsfOperStatusGet(void)
{
  return L7_DISABLE;
}

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
L7_LAST_STARTUP_REASON_t unitMgrStartupReasonGet(void)
{
  return L7_STARTUP_POWER_ON;
}

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
                                   L7_uchar8 *reasonStr)
{
  if (reasonStr == NULL)
    return L7_ERROR;

  switch (reason)
  {
    case L7_STARTUP_POWER_ON: 
      strncpy(reasonStr, "Power On", L7_STARTUP_REASON_STR_LEN - 1);
      break;

    case L7_STARTUP_COLD_ADMIN_MOVE:
      strncpy(reasonStr, "Cold Administrative Move", L7_STARTUP_REASON_STR_LEN - 1);
      break;

    case L7_STARTUP_WARM_ADMIN_MOVE: 
      strncpy(reasonStr, "Warm Administrative Move", L7_STARTUP_REASON_STR_LEN - 1);
      break;

    case L7_STARTUP_AUTO_WARM:
      strncpy(reasonStr, "Warm Auto-Restart", L7_STARTUP_REASON_STR_LEN - 1);
      break;

    case L7_STARTUP_AUTO_COLD:
      strncpy(reasonStr, "Cold Auto-Restart", L7_STARTUP_REASON_STR_LEN - 1);
      break;

    default:
      strncpy(reasonStr, "Unknown", L7_STARTUP_REASON_STR_LEN - 1);
  }
  return L7_SUCCESS;
}

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
L7_uint32 unitMgrTimeSinceLastRestart(void)
{
  return 0;
}

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
L7_BOOL unitMgrRestartInProgress(void)
{
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Get the age of the configuration on the backup unit.
*
* @param    void
*
* @returns  age in seconds
*
* @notes    This is the number of seconds since the management unit last
*           copied the configuration to the backup unit.
*
* @end
*********************************************************************/
L7_RC_t unitMgrConfigAgeOnBackup(L7_uint32 *age)
{
  return L7_NOT_EXIST;
}

/*********************************************************************
* @purpose  Get how long before the running config will next be
*           copied to the backup unit, if a copy is scheduled.
*
* @param    wait - Number of seconds until config will be copied. 
*
* @returns  L7_SUCCESS if a config copy is pending.
*           L7_NOT_EXIST if no config copy is pending
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t unitMgrNextConfigCopyTime(L7_uint32 *wait)
{
    return L7_NOT_SUPPORTED;
}

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
L7_RUN_CFG_STATUS_t unitMgrRunCfgStatusGet(void)
{
    return L7_RUN_CFG_NONE;
}

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
L7_RC_t unitMgrNsfSupportGet(L7_uint32 unit, L7_BOOL *nsfSupport)
{
  *nsfSupport = L7_FALSE;
  return L7_SUCCESS;
}

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
L7_RC_t unitMgrStackFirmwareSyncModeGet(L7_uint32 *status)
{
  if (L7_NULL == status)
  {
    return L7_ERROR;
  }
  *status = FD_UNITMGR_DEFAULT_SFS_MODE; 
  return L7_SUCCESS;
}

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
L7_RC_t unitMgrStackFirmwareSyncModeSet(L7_uint32 mode)
{
  if ((L7_ENABLE != mode) && (L7_DISABLE != mode))
  {
    return L7_ERROR;
  }
  return L7_FAILURE;
}
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
L7_RC_t unitMgrStackFirmwareSyncTrapModeGet(L7_uint32* status)
{
  if (L7_NULL == status)
  {
    return L7_ERROR;
  }
  *status = FD_UNITMGR_DEFAULT_SFS_TRAP_MODE;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Set Stack Firmware synchronication Trap status
 *
 * @param    status    SFS Trap status to be set.Valid values are
 *                     L7_ENABLE/L7_DISABLE
 *
 * @returns  L7_SUCCESS/L7_FAILURE/L7_ERROR
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncTrapModeSet(L7_uint32 mode)
{
  if ((L7_ENABLE != mode) && (L7_DISABLE != mode))
  {
    return L7_ERROR;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get Stack Firmware synchronication downgrade mode status
 *
 * @param    *status    Pointer to the SFS Trap status
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncAllowDowngradeModeGet(L7_uint32* status)
{
  if (L7_NULL == status)
  {
    return L7_ERROR;
  }
  *status = FD_UNITMGR_DEFAULT_ALLOW_DOWNGRADE_MODE;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Set Stack Firmware Synchronization downgrade mode
 *
 * @param    status    downgrade mode to be set.Valid values are
 *                     L7_ENABLE/L7_DISABLE
 *
 * @returns  L7_SUCCESS/L7_FAILURE/L7_ERROR
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncAllowDowngradeModeSet(L7_uint32 mode)
{
  if ((L7_ENABLE != mode) && (L7_DISABLE != mode))
  {
    return L7_ERROR;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the SFS status for a particular member
 *
 * @param    unit      member unit number
 * @param    status    downgrade mode to be set.Valid values are
 *                     L7_ENABLE/L7_DISABLE
 *
 * @returns  L7_SUCCESS/L7_FAILURE/L7_ERROR
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncMemberStatusGet(L7_uint32 unit,L7_uint32* status)
{
  if (NULL == status)
  {
    return L7_ERROR;
  }
  *status = L7_SFS_SWITCH_STATUS_NO_ACTION;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get Stack Firmware synchronication switch status
 *           for the whole stack
 *
 * @param    *status    Pointer to the SFS switch status
 *
 * @returns  L7_SUCCESS/L7_FAILURE/L7_ERROR
 *
 * @notes
 *
 * @end
 *********************************************************************/
 L7_RC_t unitMgrStackFirmwareSyncStatusGet(L7_uint32* status)
 {
   if (NULL == status)
   {
     return L7_ERROR;
   }
   *status = L7_SFS_SWITCH_STATUS_NO_ACTION;
   return L7_SUCCESS;
 }

/*********************************************************************
 * @purpose  Set Stack Firmware Synchronization downgrade mode
 *
 * @param    status    downgrade mode to be set.Valid values are
 *                     L7_ENABLE/L7_DISABLE
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t unitMgrStackFirmwareSyncMemberLastAttemptStatusGet(L7_uint32 unit,L7_uint32* status)
{
  if (NULL == status)
  {
    return L7_ERROR;
  }
  *status = L7_SFS_NONE;
  return L7_SUCCESS;
}

