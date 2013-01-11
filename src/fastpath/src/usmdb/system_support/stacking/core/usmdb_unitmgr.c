/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename usmdb_unitmgr.c
*
* @purpose USMDB Unit manager API implementation
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

#include <string.h>
#include "l7_common.h"
#include "usmdb_unitmgr_api.h"
#include "unitmgr_api.h"
#include "usmdb_slotmapper.h"

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
L7_RC_t usmDbUnitMgrSupportedLocalUnitIndexGet (L7_uint32 *unitTypeIdIndex)
{

 return(sysapiHpcUnitDbEntryLocalUnitIndexGet(unitTypeIdIndex));
}

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
L7_RC_t usmDbUnitMgrSupportedIndexNextGet (L7_uint32 *unitTypeIdIndex)
{
 return(sysapiHpcUnitDbEntryIndexNextGet(unitTypeIdIndex));
}


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
L7_RC_t usmDbUnitMgrSupportedUnitIdGet (L7_uint32 unitTypeIdIndex, L7_uint32 *unitTypeId)
{
  return(sysapiHpcUnitDbUnitIdGet(unitTypeIdIndex, unitTypeId));
}

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
L7_RC_t usmDbUnitMgrSupportedUnitIdNextGet (L7_uint32 *unitTypeIdIndex, L7_uint32 *unitTypeId)
{
  L7_RC_t rc;

  rc = sysapiHpcUnitDbEntryIndexNextGet(unitTypeIdIndex);
  if (rc != L7_SUCCESS)
  {
      return rc;
  }

  return(sysapiHpcUnitDbUnitIdGet(*unitTypeIdIndex, unitTypeId));
}


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
L7_RC_t usmDbUnitMgrUnitTypeIdIndexGet(L7_uint32 unit_number, L7_uint32 *unitTypeIdIndex)
{
  L7_RC_t     rc = L7_FAILURE;
  L7_uint32   unitType;
  L7_uint32   typeIndex;
  L7_uint32   uxType;

  if (usmDbUnitMgrUnitTypeGet(unit_number, &unitType) != L7_SUCCESS)
    return L7_FAILURE;

  typeIndex = 0;
  rc = usmDbUnitMgrSupportedUnitIdNextGet(&typeIndex, &uxType);
  while ((rc == L7_SUCCESS) && (unitType != uxType))
  {
    rc = usmDbUnitMgrSupportedUnitIdNextGet(&typeIndex, &uxType);
  }
  if (unitType == uxType)
  {
    *unitTypeIdIndex = typeIndex;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}



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
L7_RC_t usmDbUnitMgrSupportedModelGet (L7_uint32 unitTypeIdIndex, L7_uchar8 *unitModel)
{
  return(sysapiHpcUnitDbModelGet(unitTypeIdIndex, unitModel));
}

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
L7_RC_t usmDbUnitMgrSupportedModelNextGet (L7_uint32 *unitTypeIdIndex, L7_uchar8 *unitModel)
{
  L7_RC_t rc;

  rc = sysapiHpcUnitDbEntryIndexNextGet(unitTypeIdIndex);
  if (rc != L7_SUCCESS)
  {
       return rc;
  }

  return(sysapiHpcUnitDbModelGet(*unitTypeIdIndex, unitModel));
}

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
L7_RC_t usmDbUnitMgrSupportedDescriptionGet (L7_uint32 unitTypeIdIndex, L7_uchar8 *unitDescription)
{
  return(sysapiHpcUnitDbDescriptionGet(unitTypeIdIndex, unitDescription));
}

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
L7_RC_t usmDbUnitMgrSupportedDescriptionNextGet (L7_uint32 *unitTypeIdIndex, L7_uchar8 *unitDescription)
{
   L7_RC_t rc;

  rc = sysapiHpcUnitDbEntryIndexNextGet(unitTypeIdIndex);
  if (rc != L7_SUCCESS)
  {
        return rc;
  }

  return(sysapiHpcUnitDbDescriptionGet(*unitTypeIdIndex, unitDescription));
}

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
L7_RC_t usmDbUnitMgrSupportedMgmtPrefGet (L7_uint32 unitTypeIdIndex, L7_uint32 *unitMgmtPref)
{
  return(sysapiHpcUnitDbMgmtPrefGet(unitTypeIdIndex, unitMgmtPref));
}

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
L7_RC_t usmDbUnitMgrSupportedMgmtPrefNextGet (L7_uint32 *unitTypeIdIndex, L7_uint32 *unitMgmtPref)
{
   L7_RC_t rc;

   rc = sysapiHpcUnitDbEntryIndexNextGet(unitTypeIdIndex);
   if (rc != L7_SUCCESS)
   {
         return rc;
   }

  return(sysapiHpcUnitDbMgmtPrefGet(*unitTypeIdIndex, unitMgmtPref));
}

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
L7_RC_t usmDbUnitMgrSupportedCodeTargetIdGet (L7_uint32 unitTypeIdIndex, L7_uint32 *unitCodeLoadTargetId)
{
  return(sysapiHpcUnitDbCodeTargetIdGet(unitTypeIdIndex, unitCodeLoadTargetId));
}

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
L7_RC_t usmDbUnitMgrSupportedCodeTargetIdNextGet (L7_uint32 *unitTypeIdIndex, L7_uint32 *unitCodeLoadTargetId)
{
   L7_RC_t rc;

   rc = sysapiHpcUnitDbEntryIndexNextGet(unitTypeIdIndex);
   if (rc != L7_SUCCESS)
   {
         return rc;
   }

  return(sysapiHpcUnitDbCodeTargetIdGet(*unitTypeIdIndex, unitCodeLoadTargetId));
}


/*********************************************************************
**********************************************************************
** Run-Time Unit Table Access Functions.
**********************************************************************
*********************************************************************/

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
L7_RC_t usmDbUnitMgrStackMemberGetFirst(L7_uint32 *first_unit)
{
  return unitMgrStackMemberGetFirst(first_unit);
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
L7_RC_t usmDbUnitMgrStackMemberGetNext(L7_uint32 start_unit, L7_uint32 *next_unit)
{
  return unitMgrStackMemberGetNext(start_unit, next_unit);
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
L7_RC_t usmDbUnitMgrUnitTypeGet(L7_uint32 unit_number, L7_uint32 *unit_type)
{
  return unitMgrUnitTypeGet(unit_number, unit_type);
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
L7_RC_t usmDbUnitMgrModelIdentifierGet(L7_uint32 unit_number, L7_uchar8 *model_id)
{
  return unitMgrModelIdentifierHPCGet(unit_number, model_id);
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
L7_RC_t usmDbUnitMgrModelIdentiferPreCfgGet(L7_uint32 unit_number, L7_uchar8 *model_id)
{
  return unitMgrModelIdentifierPreCfgGet(unit_number, model_id);
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
* @notes    status is one of usmDbUnitMgrStatus_t
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrUnitStatusGet(L7_uint32 unit_number, usmDbUnitMgrStatus_t *status)
{
  L7_RC_t rc;
  unitMgrStatus_t um_status;

  rc = unitMgrUnitStatusGet(unit_number, &um_status);

  if(rc == L7_SUCCESS)
  {
    switch (um_status)
    {
      case L7_UNITMGR_UNIT_OK:
        *status = L7_USMDB_UNITMGR_UNIT_OK;
        break;

      case L7_UNITMGR_UNIT_UNSUPPORTED:
        *status = L7_USMDB_UNITMGR_UNIT_UNSUPPORTED;
        break;

      case L7_UNITMGR_CODE_MISMATCH:
        *status = L7_USMDB_UNITMGR_CODE_MISMATCH;
        break;

      case L7_UNITMGR_CFG_MISMATCH:
        *status = L7_USMDB_UNITMGR_CFG_MISMATCH;
        break;

      case L7_UNITMGR_SDM_MISMATCH:
        *status = L7_USMDB_UNITMGR_SDM_MISMATCH;
        break;

      case L7_UNITMGR_UNIT_NOT_PRESENT:
        *status = L7_USMDB_UNITMGR_UNIT_NOT_PRESENT;
        break;

      case L7_UNITMGR_CODE_UPDATE:
        *status = L7_USMDB_UNITMGR_CODE_UPDATE;
        break;

      default:
        *status = 0;
        rc = L7_FAILURE;
    }
  }
  else
    *status = L7_USMDB_UNITMGR_UNIT_UNSUPPORTED;

  return rc;
}

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
L7_RC_t usmDbUnitMgrIsMgmtUnit(L7_uint32 unit_number, usmDbUnitMgrMgmtFlag_t *mgmt_flag)
{
  L7_RC_t rc;
  unitMgrMgmtFlag_t um_mgmt_flag;

  rc = unitMgrIsMgmtUnit(unit_number,&um_mgmt_flag);
  switch (um_mgmt_flag)
  {
    case L7_UNITMGR_MGMT_ENABLED:
      *mgmt_flag = L7_USMDB_UNITMGR_MGMT_ENABLED;
      break;

    case L7_UNITMGR_MGMT_DISABLED:
      *mgmt_flag = L7_USMDB_UNITMGR_MGMT_DISABLED;
      break;

    case L7_UNITMGR_MGMT_UNASSIGNED:
      *mgmt_flag = L7_USMDB_UNITMGR_MGMT_UNASSIGNED;
      break;

    default:
      *mgmt_flag = 0;
      rc = L7_FAILURE;
  }

  return rc;
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
L7_RC_t usmDbUnitMgrDeviceDescriptionGet(L7_uint32 unit_number, L7_uchar8 *dev_desc)
{
  return unitMgrDeviceDescriptionGet(unit_number,dev_desc);
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
* @notes    will copy L7_MAC_ADDR_LEN bytes into *key
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrUnitIdKeyGet(L7_uint32 unit_number, L7_enetMacAddr_t *key)
{
  return unitMgrUnitIdKeyGet(unit_number, key);
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
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrExpectedCodeVerGet(L7_uint32 unit_number, usmDbCodeVersion_t *ver)
{
  codeVersion_t cdaVersion;

  if (unitMgrExpectedCodeVerGet(unit_number, &cdaVersion) != L7_SUCCESS)
    return L7_FAILURE;

  ver->rel = cdaVersion.rel;
  ver->ver = cdaVersion.ver;
  ver->maint_level = cdaVersion.maint_level;
  ver->build_num = cdaVersion.build_num;

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
L7_RC_t usmDbUnitMgrUnitUpTimeGet(L7_uint32 unit_number, L7_uint32 *uptime)
{
  return unitMgrUnitUpTimeGet(unit_number, uptime);
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
* @notes    *hw_pref is one of usmDbUnitMgrMgmtPref_t
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrHwMgmtPrefGet(L7_uint32 unit_number, usmDbUnitMgrMgmtPref_t *hw_pref)
{
  L7_RC_t rc;
  unitMgrMgmtPref_t um_hw_pref;

  rc = unitMgrHwMgmtPrefGet(unit_number, &um_hw_pref);
  switch (um_hw_pref)
  {
    case L7_UNITMGR_MGMTPREF_DISABLED:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_DISABLED;
      break;

    case L7_UNITMGR_MGMTPREF_1:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_1;
      break;

    case L7_UNITMGR_MGMTPREF_2:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_2;
      break;

    case L7_UNITMGR_MGMTPREF_3:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_3;
      break;

    case L7_UNITMGR_MGMTPREF_4:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_4;
      break;

    case L7_UNITMGR_MGMTPREF_5:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_5;
      break;

    case L7_UNITMGR_MGMTPREF_6:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_6;
      break;

    case L7_UNITMGR_MGMTPREF_7:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_7;
      break;

    case L7_UNITMGR_MGMTPREF_8:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_8;
      break;

    case L7_UNITMGR_MGMTPREF_9:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_9;
      break;

    case L7_UNITMGR_MGMTPREF_10:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_10;
      break;

    case L7_UNITMGR_MGMTPREF_11:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_11;
      break;

    case L7_UNITMGR_MGMTPREF_12:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_12;
      break;

    case L7_UNITMGR_MGMTPREF_13:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_13;
      break;

    case L7_UNITMGR_MGMTPREF_14:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_14;
      break;

    case L7_UNITMGR_MGMTPREF_15:
      *hw_pref = L7_USMDB_UNITMGR_MGMTPREF_15;
      break;

    case L7_UNITMGR_MGMTFUNC_UNASSIGNED:
      *hw_pref = L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED;
      break;


    default:
      *hw_pref = 0;
  }

  return rc;
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
* @notes    *admin_pref is one of unitmgr_mgmt_pref_t
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrAdminMgmtPrefGet(L7_uint32 unit_number, usmDbUnitMgrMgmtPref_t *admin_pref)
{
  L7_RC_t rc;
  unitMgrMgmtPref_t um_admin_pref;

  rc = unitMgrAdminMgmtPrefGet(unit_number, &um_admin_pref);
  switch (um_admin_pref)
  {
    case L7_UNITMGR_MGMTPREF_DISABLED:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_DISABLED;
      break;

    case L7_UNITMGR_MGMTPREF_1:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_1;
      break;

    case L7_UNITMGR_MGMTPREF_2:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_2;
      break;

    case L7_UNITMGR_MGMTPREF_3:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_3;
      break;

    case L7_UNITMGR_MGMTPREF_4:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_4;
      break;

    case L7_UNITMGR_MGMTPREF_5:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_5;
      break;

    case L7_UNITMGR_MGMTPREF_6:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_6;
      break;

    case L7_UNITMGR_MGMTPREF_7:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_7;
      break;

    case L7_UNITMGR_MGMTPREF_8:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_8;
      break;

    case L7_UNITMGR_MGMTPREF_9:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_9;
      break;

    case L7_UNITMGR_MGMTPREF_10:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_10;
      break;

    case L7_UNITMGR_MGMTPREF_11:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_11;
      break;

    case L7_UNITMGR_MGMTPREF_12:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_12;
      break;

    case L7_UNITMGR_MGMTPREF_13:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_13;
      break;

    case L7_UNITMGR_MGMTPREF_14:
     *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_14;
      break;

    case L7_UNITMGR_MGMTPREF_15:
      *admin_pref = L7_USMDB_UNITMGR_MGMTPREF_15;
      break;

    case L7_UNITMGR_MGMTFUNC_UNASSIGNED:
      *admin_pref = L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED;
      break;

    default:
      *admin_pref = 0;
  }

  return rc;
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
L7_RC_t usmDbUnitMgrDetectCodeVerRunningGet(L7_uint32 unit_number, usmDbCodeVersion_t *ver)
{
  codeVersion_t cdaVersion;

  if (unitMgrDetectCodeVerRunningGet(unit_number, &cdaVersion) != L7_SUCCESS)
    return L7_FAILURE;

  ver->rel = cdaVersion.rel;
  ver->ver = cdaVersion.ver;
  ver->maint_level = cdaVersion.maint_level;
  ver->build_num = cdaVersion.build_num;

  return L7_SUCCESS;
}

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
L7_RC_t usmDbUnitMgrSerialNumberGet(L7_uint32 unit_number, L7_uchar8 *serial_number)
{
  L7_RC_t rc;

  rc = unitMgrSerialNumberGet(unit_number, serial_number);

  return rc;
}

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
L7_RC_t usmDbUnitMgrAssetTagSet(L7_uint32 unit_number, L7_uchar8 *unit_assettag)
{
  if ((unit_number < 1) || (unit_number > L7_MAX_UNITS_PER_STACK))
  {
    return L7_FAILURE;
  }

  if (strlen(unit_assettag) > L7_SYSMGMT_ASSETTAG_MAX)
  {
    return L7_FAILURE;
  }

  return unitMgrDeviceAssetTagSet(unit_number, unit_assettag);
}

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
L7_RC_t usmDbUnitMgrAssetTagGet(L7_uint32 unit_number, L7_uchar8 *unit_assettag)
{
  if ((unit_number < 1) || (unit_number > L7_MAX_UNITS_PER_STACK))
  {
    return L7_FAILURE;
  }

  if (unit_assettag == L7_NULL)
  {
    return L7_FAILURE;
  }

  return unitMgrDeviceAssetTagGet(unit_number, unit_assettag);
}

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
L7_RC_t usmDbUnitMgrServiceTagGet(L7_uint32 unit_number, L7_uchar8 *unit_servicetag)
{
  if ((unit_number < 1) || (unit_number > L7_MAX_UNITS_PER_STACK))
  {
    return L7_FAILURE;
  }

  if (unit_servicetag == L7_NULL)
  {
    return L7_FAILURE;
  }

  return unitMgrDeviceServiceTagGet(unit_number, unit_servicetag);
}

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
L7_RC_t usmDbUnitMgrOsDescriptionGet(L7_uint32 unit_number, L7_uchar8 *os_descr)
{
  L7_RC_t rc;

  rc = unitMgrOsDescriptionGet(unit_number, os_descr);

  return rc;
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
L7_RC_t usmDbUnitMgrDetectCodeVerFlashGet(L7_uint32 unit_number, usmDbCodeVersion_t *ver)
{
  codeVersion_t cdaVersion;

  if (unitMgrDetectCodeVerFlashGet(unit_number, &cdaVersion) != L7_SUCCESS)
    return L7_FAILURE;

  ver->rel = cdaVersion.rel;
  ver->ver = cdaVersion.ver;
  ver->maint_level = cdaVersion.maint_level;
  ver->build_num = cdaVersion.build_num;

  return L7_SUCCESS;
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
L7_RC_t usmDbUnitMgrUnitTypeClear(void)
{
  return unitMgrUnitTypeClear();
}

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
L7_RC_t usmDbUnitMgrUnitNumberAssign(L7_uint32 unit_number)
{
  return unitMgrUnitNumberAssign(unit_number);
}

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
L7_RC_t usmDbUnitMgrAdminMgmtPrefSet(L7_uint32 unit_number, usmDbUnitMgrMgmtPref_t admin_pref)
{
  unitMgrMgmtPref_t um_admin_pref;

  switch (admin_pref)
  {
    case L7_USMDB_UNITMGR_MGMTPREF_DISABLED:
      um_admin_pref = L7_UNITMGR_MGMTPREF_DISABLED;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_1:
      um_admin_pref = L7_UNITMGR_MGMTPREF_1;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_2:
      um_admin_pref = L7_UNITMGR_MGMTPREF_2;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_3:
      um_admin_pref = L7_UNITMGR_MGMTPREF_3;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_4:
      um_admin_pref = L7_UNITMGR_MGMTPREF_4;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_5:
      um_admin_pref = L7_UNITMGR_MGMTPREF_5;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_6:
      um_admin_pref = L7_UNITMGR_MGMTPREF_6;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_7:
      um_admin_pref = L7_UNITMGR_MGMTPREF_7;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_8:
      um_admin_pref = L7_UNITMGR_MGMTPREF_8;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_9:
      um_admin_pref = L7_UNITMGR_MGMTPREF_9;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_10:
      um_admin_pref = L7_UNITMGR_MGMTPREF_10;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_11:
     um_admin_pref = L7_UNITMGR_MGMTPREF_11;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_12:
      um_admin_pref = L7_UNITMGR_MGMTPREF_12;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_13:
      um_admin_pref = L7_UNITMGR_MGMTPREF_13;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_14:
      um_admin_pref = L7_UNITMGR_MGMTPREF_14;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_15:
      um_admin_pref = L7_UNITMGR_MGMTPREF_15;
      break;
    case L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED:
      um_admin_pref = L7_UNITMGR_MGMTFUNC_UNASSIGNED;
      break;
    default:
      um_admin_pref = L7_UNITMGR_MGMTPREF_DISABLED;
      break;
  }

  return unitMgrAdminMgmtPrefSet(unit_number, um_admin_pref);
}

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
L7_RC_t usmDbUnitMgrTransferMgmt(L7_uint32 target_unit)
{
  return unitMgrTransferMgmt(target_unit);
}

/*********************************************************************
* @purpose  Initiate a warm failover to the backup unit
*
* @returns  If call is successful, this function never returns
*           L7_FAILURE  if an error
*           L7_REQUEST_DENIED if the system is not ready for a warm restart
*
* @notes   The management unit is rebooted.
*
* @end
*********************************************************************/
L7_RC_t usmDbFailoverInitiate(void)
{
  return unitMgrFailoverInitiate();
}

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
L7_RC_t usmDbUnitMgrNsfAdminSet(L7_uint32 nsf)
{
  if ((nsf == L7_ENABLE) || (nsf == L7_DISABLE))
  {
    return unitMgrNsfAdminSet(nsf);
  }
  return L7_ERROR;
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
L7_uint32 usmDbUnitMgrNsfAdminGet(void)
{
  return unitMgrNsfAdminGet();
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
L7_uint32 usmDbUnitMgrNsfOperStatusGet(void)
{
  return unitMgrNsfOperStatusGet();
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
L7_LAST_STARTUP_REASON_t usmDbUnitMgrStartupReasonGet(void)
{
  return unitMgrStartupReasonGet();
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
L7_RC_t usmDbUnitMgrStartupReasonStrGet(L7_LAST_STARTUP_REASON_t reason,
                                        L7_uchar8 *reasonStr)
{
  return unitMgrStartupReasonStrGet(reason, reasonStr);
}

/*********************************************************************
* @purpose  Convert a run config status to a string.
*
* @param    runCfgStatus - Status of running configuration on backup unit
* @param    runCfgStatusStr - text description of the status
*
* @returns  L7_SUCCESS
*
* @notes    runCfgStatusStr buffer must be at least L7_RUN_CFG_STATUS_STR_LEN.
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrRunCfgStatusString(L7_RUN_CFG_STATUS_t runCfgStatus, 
                                       L7_uchar8 *runCfgStatusStr)
{
  if (runCfgStatusStr == NULL)
    return L7_ERROR;

  switch (runCfgStatus)
  {
    case L7_RUN_CFG_NONE: 
      strncpy(runCfgStatusStr, "Not Yet Copied", L7_RUN_CFG_STATUS_STR_LEN - 1);
      break;

    case L7_RUN_CFG_STALE:
      strncpy(runCfgStatusStr, "Stale", L7_RUN_CFG_STATUS_STR_LEN - 1);
      break;

    case L7_RUN_CFG_IN_PROGRESS: 
      strncpy(runCfgStatusStr, "In Progress", L7_RUN_CFG_STATUS_STR_LEN - 1);
      break;

    case L7_RUN_CFG_CURRENT:
      strncpy(runCfgStatusStr, "Current", L7_RUN_CFG_STATUS_STR_LEN - 1);
      break;

    case L7_RUN_CFG_NO_BACKUP:
      strncpy(runCfgStatusStr, "No Backup Unit", L7_RUN_CFG_STATUS_STR_LEN - 1);
      break;

    default:
      strncpy(runCfgStatusStr, "Unknown", L7_RUN_CFG_STATUS_STR_LEN - 1);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of seconds since the management unit took over
*           as manager.
*
* @param    void
*
* @returns  number of seconds since mgr became mgr
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 usmDbUnitMgrTimeSinceLastRestart(void)
{
  return unitMgrTimeSinceLastRestart();
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
L7_BOOL usmDbUnitMgrRestartInProgress(void)
{
  return unitMgrRestartInProgress();
}

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
L7_RC_t usmDbUnitMgrConfigAgeOnBackup(L7_uint32 *age)
{
  return unitMgrConfigAgeOnBackup(age);
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
L7_RUN_CFG_STATUS_t usmDbUnitMgrRunCfgStatusGet(void)
{
    return unitMgrRunCfgStatusGet();
}


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
L7_RC_t usmDbUnitMgrNextConfigCopyTime(L7_uint32 *wait)
{
    return unitMgrNextConfigCopyTime(wait);
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
L7_RC_t usmDbUnitMgrNsfSupportGet(L7_uint32 unit, L7_BOOL *nsfSupport)
{
  return unitMgrNsfSupportGet(unit, nsfSupport);
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
L7_RC_t usmDbUnitMgrReplicateCfg(L7_uint32 target_unit)
{
  return unitMgrReplicateCfg(target_unit);
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
L7_RC_t usmDbUnitMgrReplicateOpr(L7_uint32 target_unit)
{
  return unitMgrReplicateOpr(target_unit);
}

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
L7_RC_t usmDbUnitMgrResetSystem(void)
{
  return unitMgrResetSystem(UNITMGR_USER_RESET_EVENT);
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
L7_RC_t usmDbUnitMgrResetUnit(L7_uint32 unit_number)
{
  return unitMgrResetUnit(unit_number);
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
L7_RC_t usmDbUnitMgrResetUnitCfg(void)
{
  return unitMgrResetUnitCfg();
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
L7_RC_t usmDbUnitMgrMgrNumberGet(L7_uint32 *unit_number)
{
  return unitMgrMgrNumberGet(unit_number);
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
L7_RC_t usmDbUnitMgrNumberGet(L7_uint32 *unit_number)
{
  return unitMgrNumberGet(unit_number);
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
L7_RC_t usmDbUnitMgrUnitNumberChange(L7_uint32 unit_number, L7_uint32 new_unit_number)
{
  return unitMgrUnitNumberChange(unit_number, new_unit_number);
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
L7_RC_t usmDbUnitMgrDeviceDescriptionSet(L7_uint32 unit_number, L7_uchar8 *unit_description)
{
  return unitMgrDeviceDescriptionSet(unit_number, unit_description);
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
L7_RC_t usmDbUnitMgrCreateUnit(L7_uint32 unit_number, L7_uint32 unit_index)
{
  return unitMgrPreConfigUnitNumber(unit_number, unit_index);
}

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
L7_RC_t usmDbUnitMgrDeleteUnit(L7_uint32 unit_number)
{
  L7_RC_t              rc;
  usmDbUnitMgrStatus_t unitStatus;

  rc = usmDbUnitMgrUnitStatusGet(unit_number, &unitStatus);
  if (rc != L7_SUCCESS)
  {
    return rc;    
  }
   
  /* Unit Delete on an active member of the stack is not allowed */
  if (unitStatus == L7_USMDB_UNITMGR_UNIT_OK)
  {
    rc = L7_ERROR;
    return rc;    
  }

  return unitMgrDeleteUnit(unit_number);
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
L7_RC_t usmDbUnitMgrEventLogGet(L7_uint32 unit)
{
  return unitMgrEventLogGet(unit);
}


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
                                 L7_uint32 *numSlots, L7_uint32 *ptrSlots)
{
  L7_uint32 i = 0;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *entry = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  entry = sysapiHpcUnitDbEntryGet(unitTypeId);
  if (entry != L7_NULLPTR)
  {
    for (i = 0; i < entry->numPhysSlots; i++)
      *(ptrSlots + i) = entry->physSlot[i].slot_number;

    rc = L7_SUCCESS;
  }

  *numSlots = i;
  return rc;
}

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
    L7_char8 *version)
{
  return unitMgrImageVersionGet(unit, image, version);
}

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
                                        L7_char8 *version)
{
  return unitMgrBootImageVersionGet(unit, image, version);
}

/*********************************************************************
* @purpose  Returns the active image file name
*
* @param    unit        @b{(input)}  Unit for this operation.
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around unitMgrActiveImageNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrActiveImageNameGet(L7_uint32 unit, L7_char8 *fileName)
{
  return unitMgrActiveImageNameGet(unit, fileName);
}

/*********************************************************************
* @purpose  Returns the backup image file name
*
* @param    unit        @b{(input)}  Unit for this operation.
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around unitMgrBackupImageNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrBackupImageNameGet(L7_uint32 unit, L7_char8 *fileName)
{
  return unitMgrBackupImageNameGet(unit, fileName);
}

/*********************************************************************
* @purpose  Returns the activated image file name
*
* @param    unit        @b{(input)}  Unit for this operation.
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around unitMgrActiveImageNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbUnitMgrActivatedImageNameGet(L7_uint32 unit, L7_char8 *fileName)
{
  return unitMgrActivatedImageNameGet(unit, fileName);
}

/*********************************************************************
**********************************************************************
**
** Debug Functions
**
**********************************************************************
*********************************************************************/

/*******************************************************
** List all supported units
*******************************************************/

L7_uint32 usmdbDebugSupUnitsNextGetTest (void)
{
   L7_uint32 unit_index;
   L7_RC_t   rc;

   L7_uint32 unit_id;
   L7_uchar8 unit_model [SYSAPI_HPC_UNIT_MODEL_SIZE];
   L7_uchar8 unit_descr [SYSAPI_HPC_UNIT_DESCRIPTION_SIZE];
   L7_uint32 mgmt_pref;
   L7_uint32 code_load_target_id;


    /* The unit table starts with index 1, so passing 0 as the current index
    ** returns the first entry in the supported cards table.
    */
    unit_index = 0;
    rc =  usmDbUnitMgrSupportedUnitIdNextGet (&unit_index, &unit_id);
    while (rc == L7_SUCCESS)
    {
        printf("Index: %d : ", unit_index);
        printf("Unit ID: 0x%x\n", unit_id);

        rc =  usmDbUnitMgrSupportedUnitIdNextGet (&unit_index, &unit_id);
    }

    unit_index = 0;
    rc =  usmDbUnitMgrSupportedModelNextGet (&unit_index, unit_model);
    while (rc == L7_SUCCESS)
    {
        printf("Index: %d : ", unit_index);
        printf("Unit Model: %s\n", unit_model);

        rc =  usmDbUnitMgrSupportedModelNextGet (&unit_index, unit_model);
    }

    unit_index = 0;
    rc =  usmDbUnitMgrSupportedDescriptionNextGet (&unit_index, unit_descr);
    while (rc == L7_SUCCESS)
    {
        printf("Index: %d : ", unit_index);
        printf("Unit Description: %s\n", unit_descr);

        rc =  usmDbUnitMgrSupportedDescriptionNextGet (&unit_index, unit_descr);
    }

    unit_index = 0;
    rc =  usmDbUnitMgrSupportedMgmtPrefNextGet (&unit_index, &mgmt_pref);
    while (rc == L7_SUCCESS)
    {
        printf("Index: %d : ", unit_index);
        printf("Unit Management Preference: %d\n", mgmt_pref);

        rc =  usmDbUnitMgrSupportedMgmtPrefNextGet (&unit_index, &mgmt_pref);
    }


    unit_index = 0;
    rc =  usmDbUnitMgrSupportedCodeTargetIdNextGet (&unit_index, &code_load_target_id);
    while (rc == L7_SUCCESS)
    {
        printf("Index: %d : ", unit_index);
        printf("Unit Code Load target ID: 0x%x\n", code_load_target_id);

        rc =  usmDbUnitMgrSupportedCodeTargetIdNextGet (&unit_index, &code_load_target_id);
    }


    return 0;
}

/**************************************************************************
* @purpose  copy the config file from supervisor to standby
*
* @param    unit             @b((output)) pointer to secondary CFMs unitid
*
* @returns  L7_RC_t  L7_SUCCESS if configuration copied successfully
* @returns           L7_FAILURE if operation fails
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t usmDbUnitMgrPropagateCfg()
{
  return(unitMgrPropagateCfg(UNITMGR_CFG_TYPE_SYSTEM));
}
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
L7_RC_t usmDbUnitMgrEraseStartupConfig()
{
  return L7_SUCCESS;
  /* TBD : This function needs to be completed when task_75719 is done.
     This is to Erase Startup config for stacking Builds. The task branch
     is not pulled until yet as it is not tested by now (20/4/08). But
     the other changes for this functionality are obtained by merging
     autoInstall changes from task branch 70144. */
  /*return unitMgrEraseStartupConfig(); */
}

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
L7_RC_t usmDbUnitMgrStandbySet(L7_uint32 standby_unit, L7_BOOL mode)
{
  return unitMgrStandbySet(standby_unit, mode);
}

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
L7_RC_t usmDbUnitMgrStandbyGet(L7_uint32 *standby_unit, L7_BOOL *admin_mode)
{
  return unitMgrStandbyGet(standby_unit, admin_mode);
}
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
L7_RC_t usmDbUnitMgrCfgStandbyGet(L7_uint32 *cfg_standby_unit)
{
  return unitMgrCfgStandbyGet(cfg_standby_unit);
}


#define MAX_UNITMGR_USER_DESCRIPTION_SIZE 81 /* CLI spec allows 80 chars, add one for null terminator */
L7_uchar8 usmDbUnitMgrUserDescription[L7_MAX_UNITS_PER_STACK][MAX_UNITMGR_USER_DESCRIPTION_SIZE] = {{0}};
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
L7_RC_t usmDbUnitMgrUserDescriptionSet(L7_uint32 unit, L7_uchar8 *descr)
{
  if ((unit < 1) || (unit > L7_MAX_UNITS_PER_STACK))
  {
    return L7_FAILURE;
  }

  if (strlen(descr) >= MAX_UNITMGR_USER_DESCRIPTION_SIZE)
  {
    return L7_FAILURE;
  }

  strcpy(usmDbUnitMgrUserDescription[unit - 1], descr);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbUnitMgrUserDescriptionGet(L7_uint32 unit, L7_uchar8 *descr)
{
  if ((unit < 1) || (unit > L7_MAX_UNITS_PER_STACK))
  {
    return L7_FAILURE;
  }

  if (descr == L7_NULL)
  {
    return L7_FAILURE;
  }

  strcpy(descr, usmDbUnitMgrUserDescription[unit - 1]);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbUnitMgrChassisServiceTagGet(L7_uint32 unit_number, L7_uchar8 *unit_serialno)
{
  if ((unit_number < 1) || (unit_number > L7_MAX_UNITS_PER_STACK))
  {
    return L7_FAILURE;
  }

  if (unit_serialno == L7_NULL)
  {
    return L7_FAILURE;
  }

  return UnitMgrDeviceChassisServiceTagGet(unit_number, unit_serialno);
}
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
L7_RC_t usmDbUnitMgrDeviceChassisSlotIdGet(L7_uint32 unit_number, L7_uchar8 *slotId)
{
  if ((unit_number < 1) || (unit_number > L7_MAX_UNITS_PER_STACK))
  {
    return L7_FAILURE;
  }

  if (L7_NULLPTR == slotId)
  {
    return L7_FAILURE;
  }

  return UnitMgrDeviceChassisSlotIdGet(unit_number, slotId);
}

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
L7_RC_t usmDbUnitMgrSlotIdGet(L7_uint32 unit_number, L7_uint32 slotNum, L7_BOOL *cardPresent)
{
  if ((unit_number < 1) || (unit_number > L7_MAX_UNITS_PER_STACK))
  {
    return L7_FAILURE;
  }

  if (cardPresent == L7_NULL)
  {
    return L7_FAILURE;
  }

  return UnitMgrDeviceSlotIdGet(unit_number, slotNum, cardPresent);
}
#endif  /* L7_FEAT_DNI8541_BLADESERVER */

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
 **********************************************************************/
L7_RC_t usmDbUnitMgrStackFirmwareSyncModeGet(L7_uint32 *status)
{
  return unitMgrStackFirmwareSyncModeGet(status);
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
 **********************************************************************/

L7_RC_t usmDbUnitMgrStackFirmwareSyncModeSet(L7_uint32 mode)
{
  return unitMgrStackFirmwareSyncModeSet(mode);
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
 **********************************************************************/
L7_RC_t usmDbUnitMgrStackFirmwareSyncTrapModeGet(L7_uint32* status)
{
  return unitMgrStackFirmwareSyncTrapModeGet(status);
}

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
 **********************************************************************/
L7_RC_t usmDbUnitMgrStackFirmwareSyncTrapModeSet(L7_uint32 mode)
{
  return unitMgrStackFirmwareSyncTrapModeSet(mode);
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
 **********************************************************************/
L7_RC_t usmDbUnitMgrStackFirmwareSyncAllowDowngradeModeGet(L7_uint32* status)
{
  return unitMgrStackFirmwareSyncAllowDowngradeModeGet(status);
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
 **********************************************************************/
L7_RC_t usmDbUnitMgrStackFirmwareSyncAllowDowngradeModeSet(L7_uint32 mode)
{
  return unitMgrStackFirmwareSyncAllowDowngradeModeSet(mode);
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
 **********************************************************************/
L7_RC_t usmDbUnitMgrStackFirmwareSyncMemberStatusGet(L7_uint32 unit,L7_uint32 *status)
{
  return unitMgrStackFirmwareSyncMemberStatusGet(unit,status);
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
 **********************************************************************/
L7_RC_t usmDbUnitMgrStackFirmwareSyncStatusGet(L7_uint32 *status)
{
  return unitMgrStackFirmwareSyncStatusGet(status);
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
 **********************************************************************/
L7_RC_t usmDbUnitMgrStackFirmwareSyncMemberLastAttemptStatusGet(L7_uint32 unit,L7_uint32 *status)
{
  return unitMgrStackFirmwareSyncMemberLastAttemptStatusGet(unit,status);
}


