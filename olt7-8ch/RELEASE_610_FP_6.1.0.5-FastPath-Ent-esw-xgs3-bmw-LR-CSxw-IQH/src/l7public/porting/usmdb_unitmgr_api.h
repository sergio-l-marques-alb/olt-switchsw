
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename usmdb_unitmgr_api.h
*
* @purpose USMDB Unit manager API defs
*
* @component unitmgr
*
* @comments none
*
* @create 1/15/2003
*
* @author djohnson
*
* @end
*
**********************************************************************/

#ifndef USMDB_UNITMGR_API_H
#define USMDB_UNITMGR_API_H

typedef enum
{
  L7_USMDB_UNITMGR_MGMTPREF_DISABLED = 0,
  L7_USMDB_UNITMGR_MGMTPREF_1,
  L7_USMDB_UNITMGR_MGMTPREF_2,
  L7_USMDB_UNITMGR_MGMTPREF_3,
  L7_USMDB_UNITMGR_MGMTPREF_4,
  L7_USMDB_UNITMGR_MGMTPREF_5,
  L7_USMDB_UNITMGR_MGMTPREF_6,
  L7_USMDB_UNITMGR_MGMTPREF_7,
  L7_USMDB_UNITMGR_MGMTPREF_8,
  L7_USMDB_UNITMGR_MGMTPREF_9,
  L7_USMDB_UNITMGR_MGMTPREF_10,
  L7_USMDB_UNITMGR_MGMTPREF_11,
  L7_USMDB_UNITMGR_MGMTPREF_12,
  L7_USMDB_UNITMGR_MGMTPREF_13,
  L7_USMDB_UNITMGR_MGMTPREF_14,
  L7_USMDB_UNITMGR_MGMTPREF_15,
  L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED
} usmDbUnitMgrMgmtPref_t;

/* Unit statuses */
typedef enum
{
  L7_USMDB_UNITMGR_UNIT_OK = 1,
  L7_USMDB_UNITMGR_UNIT_UNSUPPORTED,
  L7_USMDB_UNITMGR_CODE_MISMATCH,
  L7_USMDB_UNITMGR_CFG_MISMATCH,
  L7_USMDB_UNITMGR_UNIT_NOT_PRESENT
} usmDbUnitMgrStatus_t;

/* Management flag */
typedef enum
{
  L7_USMDB_UNITMGR_MGMT_ENABLED = 1,
  L7_USMDB_UNITMGR_MGMT_DISABLED,
  L7_USMDB_UNITMGR_MGMT_UNASSIGNED
} usmDbUnitMgrMgmtFlag_t;

typedef struct
{
  L7_uchar8 rel;
  L7_uchar8 ver;
  L7_uchar8 maint_level;
  L7_uchar8 build_num;
} usmDbCodeVersion_t;

/*********************************************************************
**********************************************************************
** Supported-Unit Table Access Functions
**********************************************************************
*********************************************************************/

/**************************************************************************
*
* @purpose  Returns unit index of the local unit.
*
* @param    unitTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedLocalUnitIndexGet (L7_uint32 *unitTypeIdIndex);


/**************************************************************************
*
* @purpose  Returns the next valid unitTypeIdIndex of greater value
*           than the one provided if it exists.
*
* @param    unitTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedIndexNextGet (L7_uint32 *unitTypeIdIndex);

/**************************************************************************
*
* @purpose  Returns unit type identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitTypeId
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedUnitIdGet (L7_uint32 unitTypeIdIndex, L7_uint32 *unitTypeId);


/**************************************************************************
*
* @purpose  Returns unit type identifier that corresponds to
*           the next index.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitTypeId
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedUnitIdNextGet (L7_uint32 *unitTypeIdIndex, L7_uint32 *unitTypeId);

/**************************************************************************
*
* @purpose  Returns unit model identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitModel
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedModelGet (L7_uint32 unitTypeIdIndex, L7_uchar8 *unitModel);

/*********************************************************************
* @purpose  Given a unit, gets the unit type index
*
* @param    unit_number       unit number
* @param    *unitTypeIdIndex  unit type type index for this unit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrUnitTypeIdIndexGet(L7_uint32 unit_number, L7_uint32 *unitTypeIdIndex);

/**************************************************************************
*
* @purpose  Returns unit model identifier that corresponds to
*           the next index.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitModel
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedModelNextGet (L7_uint32 *unitTypeIdIndex, L7_uchar8 *unitModel);

/**************************************************************************
*
* @purpose  Returns unit description that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitDescription
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedDescriptionGet (L7_uint32 unitTypeIdIndex, L7_uchar8 *unitDescription);

/**************************************************************************
*
* @purpose  Returns unit description that corresponds to
*           the next index.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitDescription
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedDescriptionNextGet (L7_uint32 *unitTypeIdIndex, L7_uchar8 *unitDescription);

/**************************************************************************
*
* @purpose  Returns unit management preference that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitMgmtPref
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedMgmtPrefGet (L7_uint32 unitTypeIdIndex, L7_uint32 *unitMgmtPref);

/**************************************************************************
*
* @purpose  Returns unit management preference that corresponds to
*           the next index.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitMgmtPref
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedMgmtPrefNextGet (L7_uint32 *unitTypeIdIndex, L7_uint32 *unitMgmtPref);

/**************************************************************************
*
* @purpose  Returns unit code target ID that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitCodeLoadTargetId
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedCodeTargetIdGet (L7_uint32 unitTypeIdIndex, L7_uint32 *unitCodeLoadTargetId);

/**************************************************************************
*
* @purpose  Returns unit code target ID that corresponds to
*           the next index.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitCodeLoadTargetId
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t usmDbUnitMgrSupportedCodeTargetIdNextGet (L7_uint32 *unitTypeIdIndex, L7_uint32 *unitCodeLoadTargetId);

/*********************************************************************
**********************************************************************
** Run-Time Unit Table Access Functions.
**********************************************************************
*********************************************************************/














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
L7_RC_t usmDbUnitMgrStackMemberGetFirst(L7_uint32 *first_unit);

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
L7_RC_t usmDbUnitMgrStackMemberGetNext(L7_uint32 start_unit, L7_uint32 *next_unit);


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
L7_RC_t usmDbUnitMgrUnitTypeGet(L7_uint32 unit_number, L7_uint32 *unit_type);

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
L7_RC_t usmDbUnitMgrModelIdentifierGet(L7_uint32 unit_number, L7_uchar8 *model_id);

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
L7_RC_t usmDbUnitMgrModelIdentiferPreCfgGet(L7_uint32 unit_number, L7_uchar8 *model_id);

/*********************************************************************
* @purpose  Given a unit, gets the unit's status
*
* @param    unit_number unit number
* @param    *status     unit status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    status is one of usmDbUnitMgrStatus_t
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrUnitStatusGet(L7_uint32 unit_number, usmDbUnitMgrStatus_t *status);

/*********************************************************************
* @purpose  Given a unit, gets the unit's management status
*
* @param    unit_number unit number
* @param    *mgmt_flag    mgmt status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    status is one of usmDbUnitMgrMgmtFlag_t
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrIsMgmtUnit(L7_uint32 unit_number, usmDbUnitMgrMgmtFlag_t *mgmt_flag);

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
L7_RC_t usmDbUnitMgrDeviceDescriptionGet(L7_uint32 unit_number, L7_uchar8 *dev_desc);

/*********************************************************************
* @purpose  Given a unit, gets the unit's system identifier key
*
* @param    unit_number unit number
* @param    *key   pointer to unit key storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy L7_MAC_ADDR_LEN bytes into *key
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrUnitIdKeyGet(L7_uint32 unit_number, L7_enetMacAddr_t *key);

/*********************************************************************
* @purpose  Given a unit, gets the unit's expected code version
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
L7_RC_t usmDbUnitMgrExpectedCodeVerGet(L7_uint32 unit_number, usmDbCodeVersion_t *ver);

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
L7_RC_t usmDbUnitMgrUnitUpTimeGet(L7_uint32 unit_number, L7_uint32 *uptime);

/*********************************************************************
* @purpose  Given a unit, gets the unit's hardware management preference
*
* @param    unit_number unit number
* @param    *hw_pref    preference to become a management unit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    *hw_pref is one of unitmgr_mgmt_pref_t
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrHwMgmtPrefGet(L7_uint32 unit_number, usmDbUnitMgrMgmtPref_t *hw_pref);

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
L7_RC_t usmDbUnitMgrAdminMgmtPrefGet(L7_uint32 unit_number, usmDbUnitMgrMgmtPref_t *admin_pref);

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
L7_RC_t usmDbUnitMgrDetectCodeVerRunningGet(L7_uint32 unit_number, usmDbCodeVersion_t *ver);

/*********************************************************************
* @purpose  Given a unit, gets the unit's serial number
*
* @param    unit_number unit number
* @param    *serial_number  Unit serial number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrSerialNumberGet(L7_uint32 unit_number, L7_uchar8 *serial_number);

/*********************************************************************
* @purpose  Given a unit, gets the unit's Asset-tag
*
* @param    unit_number unit number
* @param    *unit_assettag  Unit's Asset-tag
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrAssetTagSet(L7_uint32 unit_number, L7_uchar8 *unit_assettag);

/*********************************************************************
* @purpose  Given a unit, gets the unit's Asset-Tag
*
* @param    unit_number        unit number
* @param    *unit_assettag   Pointer to unit's Asset-tag
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrAssetTagGet(L7_uint32 unit_number, L7_uchar8 *unit_assettag);

/*********************************************************************
* @purpose  Given a unit, gets the service tag
*
* @param    unit_number        unit number
* @param    *unit_assettag   Pointer to unit's service-tag
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrServiceTagGet(L7_uint32 unit_number, L7_uchar8 *unit_servicetag);

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
L7_RC_t usmDbUnitMgrChassisServiceTagGet(L7_uint32 unit_number, L7_uchar8 *unit_chassisServtag);
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
L7_RC_t usmDbUnitMgrDeviceChassisSlotIdGet(L7_uint32 unit_number, L7_uchar8 *slotId);

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
L7_RC_t usmDbUnitMgrSlotIdGet(L7_uint32 unit_number, L7_uint32 slotNum, L7_BOOL *cardPresent);
#endif

/*********************************************************************
* @purpose  Given a unit, gets the unit's operating system description
*
* @param    unit_number unit number
* @param    *os_descr  Operating system description.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrOsDescriptionGet(L7_uint32 unit_number, L7_uchar8 *os_descr);

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
L7_RC_t usmDbUnitMgrDetectCodeVerFlashGet(L7_uint32 unit_number, usmDbCodeVersion_t *ver);

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
L7_RC_t usmDbUnitMgrUnitTypeClear(void);

/*********************************************************************
* @purpose  Assigns the local unit a new unit number
*
* @param    unit_number new unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrUnitNumberAssign(L7_uint32 unit_number);

/*********************************************************************
* @purpose  Set the admin management preference
*
* @param    unit_number unit number, local or remote
* @param    admin_pref  admin_pref
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrAdminMgmtPrefSet(L7_uint32 unit_number, usmDbUnitMgrMgmtPref_t admin_pref);

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
L7_RC_t usmDbUnitMgrTransferMgmt(L7_uint32 target_unit);

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
L7_RC_t usmDbUnitMgrReplicateCfg(L7_uint32 target_unit);

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
L7_RC_t usmDbUnitMgrReplicateOpr(L7_uint32 target_unit);

/*********************************************************************
* @purpose  Reset all units in the stack
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
L7_RC_t usmDbUnitMgrResetSystem(void);

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
L7_RC_t usmDbUnitMgrResetUnit(L7_uint32 unit_number);

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
L7_RC_t usmDbUnitMgrResetUnitCfg(void);

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
L7_RC_t usmDbUnitMgrMgrNumberGet(L7_uint32 *unit_number);

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
L7_RC_t usmDbUnitMgrNumberGet(L7_uint32 *unit_number);

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
L7_RC_t usmDbUnitMgrUnitNumberChange(L7_uint32 unit_number, L7_uint32 new_unit_number);


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
L7_RC_t usmDbUnitMgrDeviceDescriptionSet(L7_uint32 unit_number, L7_uchar8 *unit_description);

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
L7_RC_t usmDbUnitMgrCreateUnit(L7_uint32 unit_number, L7_uint32 unit_index);

/*********************************************************************
* @purpose  Will remove a unit only if it is not an active member of
*           the stack
*
* @param    unit_number     unit number to remove
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if remove failed
* @returns  L7_ERROR    if unit is an active member of the stack
*
* @notes    If the unit status is L7_USMDB_UNITMGR_UNIT_OK, then
*           the stack member cannot be deleted.
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrDeleteUnit(L7_uint32 unit_number);

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
L7_RC_t usmDbUnitMgrEventLogGet(L7_uint32 unit);



/**************************************************************************
* @purpose  Retrieve information from the unit descriptor database for the
*           given unit type identifier.
*
* @param    unitTypeId   @b((input)) unit type identifier
* @param    numSlots     @b((output)) pointer to number of slots in array
* @param    ptrSlots     @b((output)) pointer to slots array
*
* @returns  pointer to unit descriptor record if unitTypeId found in database
* @returns  L7_NULLPTR if unitTypeId not found in database
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t usmDbUnitDbEntrySlotsGet(L7_uint32 unitTypeId,
                                 L7_uint32 *numSlots, L7_uint32 *ptrSlots);

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
L7_RC_t  usmDbUnitMgrImageVersionGet(L7_uint32 unit, L7_char8 *image,
    L7_char8 *version);

/*********************************************************************
* @purpose  Retrieves the boot image version information for a given image
*
* @param    unit         @b{(input)}  Unit number for this request
* @param    image        @b{(input)}  image to associate the text
* @param    descr        @b{(output)} modification time string
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_NOT_EXIST  if the specified image could not be found
* @returns  L7_FAILURE    on FS errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrBootImageVersionGet(L7_uint32 unit, L7_char8 *image,
                                        L7_char8 *version);

/*********************************************************************
* @purpose  Returns the active image file name
*
* @param    unit        @b{(input)}  Unit for this operation.
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around unitMgrActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrActiveImageNameGet(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Returns the backup image file name
*
* @param    unit        @b{(input)}  Unit for this operation.
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around unitMgrActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrBackupImageNameGet(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Returns the activated image file name
*
* @param    unit        @b{(input)}  Unit for this operation.
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around unitMgrActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrActivatedImageNameGet(L7_uint32 unit, L7_char8 *fileName);

/**************************************************************************
* @purpose  copy the config file from standby to supervisor
*
* @param    unit             @b((output)) pointer to secondary CFMs unitid
*
* @returns  L7_RC_t  L7_SUCCESS if a secondary CFM is inserted.
* @returns           L7_FAILURE IF a secondary CFM is not inserted.
*
* @comments  This is a chassis specific function call.
*
* @end
*************************************************************************/
L7_RC_t usmDbUnitMgrPropagateCfg_SCFM2ACFM();
/**************************************************************************
* @purpose  get the physical slot number of standby
*
* @param    unit             @b((output)) pointer to secondary CFMs unitid
*
* @returns  L7_RC_t  L7_SUCCESS if a secondary CFM is inserted.
* @returns           L7_FAILURE IF a secondary CFM is not inserted.
*
* @comments  This is a chassis specific function call.
*
* @end
*************************************************************************/
void usmDbStandbyCfmSlotGet(L7_uint32 *slot);
/**************************************************************************
* @purpose  copy the config file from supervisor to standby
*
* @param    unit             @b((output)) pointer to secondary CFMs unitid
*
* @returns  L7_RC_t  L7_SUCCESS if a secondary CFM is inserted.
* @returns           L7_FAILURE IF a secondary CFM is not inserted.
*
* @comments  This is a chassis specific function call.
*
* @end
*************************************************************************/
L7_RC_t usmDbUnitMgrPropagateCfg();
/**************************************************************************
* @purpose  will erase the startup-config file on other connected units
*
* @param    none
*
* @returns  L7_RC_t  L7_SUCCESS if startup-config successfully erased
* @returns           L7_FAILURE failed while trying to erase startup-config
*
* @comments  none
*
* @end
*************************************************************************/
L7_RC_t usmDbUnitMgrEraseStartupConfig();
/**************************************************************************
* @purpose  Retrieve the unit id of the secondary CFM in chassis
*
* @param    unit             @b((output)) pointer to secondary CFMs unitid
*
* @returns  L7_RC_t  L7_SUCCESS if a secondary CFM is inserted.
* @returns           L7_FAILURE IF a secondary CFM is not inserted.
*
* @comments  This is a chassis specific function call.
*
* @end
*************************************************************************/
L7_RC_t usmdbChassisStandbyCMUnitGet(L7_uint32 *unit);
/*********************************************************************
*
* @purpose set unit user description
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrUserDescriptionSet(L7_uint32 unit, L7_uchar8 *descr);

/*********************************************************************
*
* @purpose get unit user description
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrUserDescriptionGet(L7_uint32 unit, L7_uchar8 *descr);

/*********************************************************************
* @purpose  Sets the unit as standby in the stack.
*
* @param    standby_unit destination unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrStandbySet(L7_uint32 standby_unit, L7_BOOL mode);

/*********************************************************************
* @purpose  Gets the standby unit number in the stack.
*
* @param    *standby_unit Pointer to the standby unit number
* @param    *admin_mode   Pointer to the admin mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrStandbyGet(L7_uint32 *standby_unit, L7_BOOL *admin_mode);


/*********************************************************************
* @purpose  To set a given unit is standby of the stack
*
* @param    unit_number        unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrStandbySet(L7_uint32 standby_unit, L7_BOOL mode);

/*********************************************************************
* @purpose  To Get the current standby unit number of the stack
*
* @param    *unit_number        Pointer to the standby unit number
* @param    *mode               Pointer to the admin mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrStandbyGet(L7_uint32 *standby_unit, L7_BOOL *mode);
L7_RC_t usmDbUnitMgrCfgStandbyGet(L7_uint32 *cfg_standby_unit);

#endif

