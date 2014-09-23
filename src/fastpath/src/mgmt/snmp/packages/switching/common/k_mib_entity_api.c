/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_entity_api.c
*
* @purpose    Wrapper functions for Entity MIB - RFC 2737
*
* @component  SNMP
*
* @comments
*
* @create     6/10/2003
*
* @author     cpverne
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_entity_api.h"
#include "snmpapi.h"

#include "commdefs.h"
#include "usmdb_edb_api.h"
#include "edb_exports.h"

/* Begin Function Definitions: k_mib_entity_api.h */

L7_RC_t
snmpEntPhysicalEntryGet ( entPhysicalEntry_t *entPhysicalEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (usmDbEdbPhysicalEntryGet(entPhysicalEntryData->entPhysicalIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, entPhysicalEntryData->valid);

    bzero(snmp_buffer, SNMP_BUFFER_LEN);

    switch (nominator)
    {
    case -1:
    case I_entPhysicalIndex:
      rc = L7_SUCCESS;
    if (nominator != -1) break;
    /* else pass through */

    case I_entPhysicalDescr:
      if (usmDbEdbPhysicalDescrGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&entPhysicalEntryData->entPhysicalDescr, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
    if (nominator != -1) break;
    /* else pass through */

    case I_entPhysicalVendorType:
      if (usmDbEdbPhysicalVendorTypeGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOIDFromDot(&entPhysicalEntryData->entPhysicalVendorType, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalContainedIn:
      rc = usmDbEdbPhysicalContainedInGet(entPhysicalEntryData->entPhysicalIndex, &entPhysicalEntryData->entPhysicalContainedIn);
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalClass:
      rc = snmpEntPhysicalClassGet(entPhysicalEntryData->entPhysicalIndex, &entPhysicalEntryData->entPhysicalClass);
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalParentRelPos:
      rc = usmDbEdbPhysicalParentRelPosGet(entPhysicalEntryData->entPhysicalIndex, &entPhysicalEntryData->entPhysicalParentRelPos);
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalName:
      if (usmDbEdbPhysicalNameGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&entPhysicalEntryData->entPhysicalName, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalHardwareRev:
      if (usmDbEdbPhysicalHardwareRevGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&entPhysicalEntryData->entPhysicalHardwareRev, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalFirmwareRev:
      if (usmDbEdbPhysicalFirmwareRevGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&entPhysicalEntryData->entPhysicalFirmwareRev, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalSoftwareRev:
      if (usmDbEdbPhysicalSoftwareRevGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&entPhysicalEntryData->entPhysicalSoftwareRev, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalSerialNum:
      if (usmDbEdbPhysicalSerialNumGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&entPhysicalEntryData->entPhysicalSerialNum, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalMfgName:
      if (usmDbEdbPhysicalMfgNameGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&entPhysicalEntryData->entPhysicalMfgName, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalModelName:
      if (usmDbEdbPhysicalModelNameGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&entPhysicalEntryData->entPhysicalModelName, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalAlias:
      if (usmDbEdbPhysicalAliasGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&entPhysicalEntryData->entPhysicalAlias, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalAssetID:
      if (usmDbEdbPhysicalAssetIDGet(entPhysicalEntryData->entPhysicalIndex, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&entPhysicalEntryData->entPhysicalAssetID, snmp_buffer) == L7_TRUE)
        rc = L7_SUCCESS;
      if (nominator != -1) break;
      /* else pass through */

    case I_entPhysicalIsFRU:
      rc = snmpEntPhysicalIsFRUGet(entPhysicalEntryData->entPhysicalIndex, &entPhysicalEntryData->entPhysicalIsFRU);
      break;

    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, entPhysicalEntryData->valid);
  }

  return rc;
}

L7_RC_t
snmpEntPhysicalEntryNextGet ( entPhysicalEntry_t *entPhysicalEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (usmDbEdbPhysicalEntryNextGet(&entPhysicalEntryData->entPhysicalIndex) == L7_SUCCESS)
  {
    if (snmpEntPhysicalEntryGet(entPhysicalEntryData, nominator) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}


L7_RC_t snmpEntPhysicalClassGet(L7_uint32 physicalIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbEdbPhysicalClassGet(physicalIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_EDB_PHYSICAL_CLASS_OTHER:
      *val = D_entPhysicalClass_other;
      break;

    case L7_EDB_PHYSICAL_CLASS_UNKNOWN:
      *val = D_entPhysicalClass_unknown;
      break;

    case L7_EDB_PHYSICAL_CLASS_CHASSIS:
      *val = D_entPhysicalClass_chassis;
      break;

    case L7_EDB_PHYSICAL_CLASS_BACKPLANE:
      *val = D_entPhysicalClass_backplane;
      break;

    case L7_EDB_PHYSICAL_CLASS_CONTAINER:
      *val = D_entPhysicalClass_container;
      break;

    case L7_EDB_PHYSICAL_CLASS_POWERSUPPLY:
      *val = D_entPhysicalClass_powerSupply;
      break;

    case L7_EDB_PHYSICAL_CLASS_FAN:
      *val = D_entPhysicalClass_fan;
      break;

    case L7_EDB_PHYSICAL_CLASS_SENSOR:
      *val = D_entPhysicalClass_sensor;
      break;

    case L7_EDB_PHYSICAL_CLASS_MODULE:
      *val = D_entPhysicalClass_module;
      break;

    case L7_EDB_PHYSICAL_CLASS_PORT:
      *val = D_entPhysicalClass_port;
      break;

    case L7_EDB_PHYSICAL_CLASS_STACK:
      *val = D_entPhysicalClass_stack;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t snmpEntPhysicalIsFRUGet(L7_uint32 physicalIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbEdbPhysicalIsFRUGet(physicalIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_entPhysicalIsFRU_true;
      break;

    case L7_FALSE:
      *val = D_entPhysicalIsFRU_false;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t snmpEntLastChangeTimeGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbEdbLastChangeTimeGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    /* convert sysUpTime to TimeTicks */
    *val = temp_val * SNMP_HUNDRED;
  }

  return rc;
}

/* End Function Definitions */
