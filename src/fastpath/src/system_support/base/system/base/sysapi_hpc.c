/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  sysapi_hpc.c
*
* @purpose   Provide system-wide support routines
*
* @component sysapi
*
* @create    02/12/2003
*
* @author    jeffr
* @end
*
*********************************************************************/
/*********************************************************************
 *
 ********************************************************************/
#include <l7_common.h>
#include "l7_product.h"
#include <registry.h>
#include <default_cnfgr.h>
#include <nvstoreapi.h>
#include "string.h"                /* for memcpy() etc... */
#include "stdarg.h"                /* for va_start, etc... */
#include "osapi.h"
#include "log.h"
#include "cnfgr.h"

#include "sysapi.h"
#include "sysapi_hpc.h"
#include "hpc_hw_api.h"
#include "hpc.h"
#include "compdefs.h"
#include "bspapi.h"

extern HPC_UNIT_DESCRIPTOR_t hpc_unit_descriptor_db[];
extern HPC_CARD_DESCRIPTOR_t hpc_card_descriptor_db[];
static L7_uint32             hpcSlotPortCount[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];

/*
** unit/card catalog database api
*/
/**************************************************************************
*
* @purpose  Initialize the Hardware Platform Control facility of the
*           sysapi component.
*
* @param    none
*
* @returns  L7_SUCCESS if initialization completes with no problems
* @returns  L7_FAILURE if problem was found during initialization
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcInit(void)
{
  L7_RC_t result = L7_SUCCESS;

  if ((result = hpcDescriptorDbInit()) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "hpcDescriptorDbInit: bad return code from sysapiHpcDescriptorDbInit call.\n");
    return(result);
  }

  PT_LOG_INFO(LOG_CTX_STARTUP,"Starting sysapiHpcInit...");
  result = hpcInit();
  PT_LOG_INFO(LOG_CTX_STARTUP,"sysapiHpcInit finished");

  if (result != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "hpcInit: bad return code from hpcInit call.\n");
    return(result);
  }

  return(result);
}

/**************************************************************************
*
* @purpose  Retrieve information from the unit descriptor database for the
*           given unit type identifier.
*
* @param    unitTypeId   @b((in)) unit type identifier the caller wants
*                        information about
*
* @returns  pointer to the unit descriptor record if unitTypeId
*           was found in the database
* @returns  L7_NULLPTR if unitTypeId was not found in the database
*
* @comments
*
* @end
*
*************************************************************************/
SYSAPI_HPC_UNIT_DESCRIPTOR_t *sysapiHpcUnitDbEntryGet(L7_uint32 unitTypeId)
{
  return((SYSAPI_HPC_UNIT_DESCRIPTOR_t *)hpcUnitDbLookup(unitTypeId));
}

/**************************************************************************
*
* @purpose  Retrieve information from the unit descriptor database for the
*           given unit index.
*
* @param    unitTypeIdIndex  Index in the unit table.
*
* @returns  pointer to the unit descriptor record if unitTypeIdIndex
*           is valid.
* @returns  L7_NULLPTR if unitTypeIdIndex is invalid.
*
* @comments
*
* @end
*
*************************************************************************/
SYSAPI_HPC_UNIT_DESCRIPTOR_t *sysapiHpcUnitDbEntryByIndexGet(L7_uint32 unitTypeIdIndex)
{
  /* Check that index is valid... index is 1 based */
  if (unitTypeIdIndex > 0 && unitTypeIdIndex <= hpcSupportedUnitsNumGet())
  {
    return  &hpc_unit_descriptor_db[unitTypeIdIndex-1].unitTypeDescriptor;
  }

  return L7_NULLPTR;
}

/**************************************************************************
*
* @purpose  Validates whether the provided index is a valid unitTypeIdIndex
*           value.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
*
* @returns  L7_SUCCESS  index is valid
* @returns  L7_FAILURE  index is invalid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbEntryIndexCheck(L7_uint32 unitTypeIdIndex)
{
  L7_RC_t rc = L7_FAILURE;

  /* Check that index is valid... index is 1 based */
  if (unitTypeIdIndex > 0 && unitTypeIdIndex <= hpcSupportedUnitsNumGet())
  {
    rc = L7_SUCCESS;
  }
  return(rc);
}

/**************************************************************************
*
* @purpose  Return the unit index of the local unit.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbEntryLocalUnitIndexGet(L7_uint32 *unitTypeIdIndex)
{
  L7_uint32 i;
  HPC_UNIT_DESCRIPTOR_t *unit_desc;

  unit_desc = hpcLocalUnitDescriptorGet();

  for (i = 0; i < hpcSupportedUnitsNumGet(); i++)
  {
    if (unit_desc->unitTypeDescriptor.unitTypeId ==
        hpc_unit_descriptor_db[i].unitTypeDescriptor.unitTypeId)
    {
      *unitTypeIdIndex = i+1;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
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
L7_RC_t sysapiHpcUnitDbEntryIndexNextGet(L7_uint32 *unitTypeIdIndex)
{
  L7_RC_t rc = L7_FAILURE;

  /* Check that index is in range after being incremented... index is 1 based
   */
  if ((*unitTypeIdIndex == 0) ||
      (*unitTypeIdIndex < hpcSupportedUnitsNumGet()))
  {
    /* we are in range of table, so increment index value to get to next valid */
    (*unitTypeIdIndex)++;
    rc = L7_SUCCESS;
  }

  return(rc);
}

/**************************************************************************
*
* @purpose  Returns unit type index that corresponds to
*           the unit type identifier provided.
*
* @param    unitTypeId        unit type identifier
* @param    unitTypeIdIndex   index into the unit descriptor table
*
* @returns  L7_SUCCESS  the ID is valid and the index retrieved
* @returns  L7_FAILURE  the ID was not valid
* @returns  *unitTypeIndex  pointer to the index
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbUnitTypeIndexGet(L7_uint32 unitTypeId, L7_uint32 *unitTypeIndex)
{
  L7_uint32 i;

  for (i = 0; i < hpcSupportedUnitsNumGet(); i++)
  {
    if (unitTypeId == hpc_unit_descriptor_db[i].unitTypeDescriptor.unitTypeId)
    {
      *unitTypeIndex = i + 1;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  Returns unit type identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
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
L7_RC_t sysapiHpcUnitDbUnitIdGet(L7_uint32 unitTypeIdIndex, L7_uint32 *unitTypeId)
{
 L7_RC_t rc = L7_FAILURE;

 /* Check that index is valid... index is 1 based */
 if (unitTypeIdIndex > 0 && unitTypeIdIndex <= hpcSupportedUnitsNumGet())
 {
   *unitTypeId = hpc_unit_descriptor_db[unitTypeIdIndex-1].unitTypeDescriptor.unitTypeId;
   rc = L7_SUCCESS;
 }
 return(rc);
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
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbModelGet(L7_uint32 unitTypeIdIndex, L7_uchar8 *unitModel)
{
 L7_RC_t rc = L7_FAILURE;

 /* Check that index is valid... index is 1 based */
 if (unitTypeIdIndex > 0 && unitTypeIdIndex <= hpcSupportedUnitsNumGet())
 {
   strcpy (unitModel, hpc_unit_descriptor_db[unitTypeIdIndex-1].unitTypeDescriptor.unitModel);
   rc = L7_SUCCESS;
 }
 return(rc);
}

/**************************************************************************
*
* @purpose  Returns unit description identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitDescription
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbDescriptionGet(L7_uint32 unitTypeIdIndex, L7_uchar8 *unitDescription)
{
 L7_RC_t rc = L7_FAILURE;

 /* Check that index is valid... index is 1 based */
 if (unitTypeIdIndex > 0 && unitTypeIdIndex <= hpcSupportedUnitsNumGet())
 {
   strcpy (unitDescription, hpc_unit_descriptor_db[unitTypeIdIndex-1].unitTypeDescriptor.unitDescription);
   rc = L7_SUCCESS;
 }
 return(rc);
}

/**************************************************************************
*
* @purpose  Returns unit management preference identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitMgmtPref
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbMgmtPrefGet(L7_uint32 unitTypeIdIndex, L7_uint32 *unitMgmtPref)
{
 L7_RC_t rc = L7_FAILURE;

 /* Check that index is valid... index is 1 based */
 if (unitTypeIdIndex > 0 && unitTypeIdIndex <= hpcSupportedUnitsNumGet())
 {
   *unitMgmtPref = hpc_unit_descriptor_db[unitTypeIdIndex-1].unitTypeDescriptor.managementPreference;
   rc = L7_SUCCESS;
 }
 return(rc);
}

/**************************************************************************
*
* @purpose  Returns unit OID that corresponds to the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitOid
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbOidGet(L7_uint32 unitTypeIdIndex, L7_uchar8 *unitOid)
{
  L7_RC_t rc = L7_FAILURE;

  /* Check that index is valid... index is 1 based */
  if (unitTypeIdIndex > 0 && unitTypeIdIndex <= hpcSupportedUnitsNumGet())
  {
    strncpy(unitOid, hpc_unit_descriptor_db[unitTypeIdIndex-1].unitTypeDescriptor.systemOID,
            SYSAPI_HPC_SYSTEM_OID_SIZE);
    rc = L7_SUCCESS;
  }

  return(rc);
}

/**************************************************************************
*
* @purpose  Returns PoE is supported on the unit type that
*           corresponds to the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitPoeSupported
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbPoeSupportGet(L7_uint32 unitTypeIdIndex, L7_BOOL *unitPoeSupported)
{
  L7_RC_t rc = L7_FAILURE;

  /* Check that index is valid... index is 1 based */
  if (unitTypeIdIndex > 0 && unitTypeIdIndex <= hpcSupportedUnitsNumGet())
  {
    if (0 == hpc_unit_descriptor_db[unitTypeIdIndex-1].unitTypeDescriptor.poeSupport)
    {
      *unitPoeSupported = L7_FALSE;
    }
    else
    {
      *unitPoeSupported = L7_TRUE;
    }
    rc = L7_SUCCESS;
  }

  return(rc);
}

/**************************************************************************
*
* @purpose  Returns unit code load target identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitCodeLoadTargetId
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbCodeTargetIdGet(L7_uint32 unitTypeIdIndex, L7_uint32 *unitCodeLoadTargetId)
{
 L7_RC_t rc = L7_FAILURE;

 /* Check that index is valid... index is 1 based */
 if (unitTypeIdIndex > 0 && unitTypeIdIndex <= hpcSupportedUnitsNumGet())
 {
   *unitCodeLoadTargetId = hpc_unit_descriptor_db[unitTypeIdIndex-1].unitTypeDescriptor.codeLoadTargetId;
   rc = L7_SUCCESS;
 }
 return(rc);
}

/**************************************************************************
*
* @purpose  Return a pointer to the unit descriptor for the local unit.
*
* @param    none
*
* @returns  pointer to the unit descriptor record for the local unit
* @returns  L7_NULLPTR if some problem was encountered in retrieving
*                      the descriptor pointer
*
* @comments
*
* @end
*
*************************************************************************/
SYSAPI_HPC_UNIT_DESCRIPTOR_t *sysapiHpcLocalUnitDbEntryGet(void)
{
  return((SYSAPI_HPC_UNIT_DESCRIPTOR_t *)hpcLocalUnitDescriptorGet());
}

/**************************************************************************
*
* @purpose  Retrieve information from the card descriptor database for the
*           given card type identifier.
*
* @param    cardTypeId   @b((in)) card type identifier the caller wants
*                        information about
*
* @returns  pointer to the card descriptor record if cardTypeId
*           was found in the database
* @returns  L7_NULLPTR if cardTypeId was not found in the database
*
* @comments
*
* @end
*
*************************************************************************/
SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardDbEntryGet(L7_uint32 cardTypeId)
{
  return((SYSAPI_HPC_CARD_DESCRIPTOR_t *)hpcCardDbLookup(cardTypeId));
}

/**************************************************************************
*
* @purpose  Retrieve information from the card descriptor database for the
*           given card index.
*
* @param    cardTypeIdIndex
*
* @returns  pointer to the card descriptor record if cardTypeId
*           was found in the database
* @returns  L7_NULLPTR if card index is not valid.
*
* @comments
*
* @end
*
*************************************************************************/
SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardDbEntryByIndexGet(L7_uint32 cardTypeIdIndex)
{
  /* Check that index is valid... index is 1 based */
  if (cardTypeIdIndex > 0 && cardTypeIdIndex <= hpcSupportedCardsNumGet())
  {
    return (&hpc_card_descriptor_db[cardTypeIdIndex-1].cardTypeDescriptor);
  }

  return L7_NULLPTR;
}

/**************************************************************************
*
* @purpose  Validates whether the provided index is a valid cardTypeIdIndex
*           value.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  index is valid
* @returns  L7_FAILURE  index is invalid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcCardDbEntryIndexCheck(L7_uint32 cardTypeIdIndex)
{
  return(hpcCardDbEntryIndexCheck(cardTypeIdIndex));
}

/**************************************************************************
*
* @purpose  Returns the next valid cardTypeIdIndex of greater value
*           than the one provided if it exists.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
* @returns  cardTypeIdIndex value of next index if found
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcCardDbEntryIndexNextGet(L7_uint32 *cardTypeIdIndex)
{
  return(hpcCardDbEntryIndexNextGet(cardTypeIdIndex));
}

/*****************************************
** Supported-Slot Table Access Functions
*****************************************/

/**************************************************************************
*
* @purpose  Returns the next valid slotTypeIdIndex of greater value
*           than the one provided if it exists.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcSlotDbEntryIndexNextGet(L7_uint32 * unitIndex, L7_uint32 *slotIndex)
{
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_entry;

  /* If both indexes are seto to zero then point at the beginning of the table.
   */
  if ((*unitIndex == 0) && (*slotIndex == 0))
  {
    *unitIndex = 1;
    *slotIndex = 1;

    return L7_SUCCESS;
  }

  /* If unit index is out of range then return error.
   */
  if (*unitIndex > hpcSupportedUnitsNumGet())
  {
    return L7_NOT_EXIST;
  }

  unit_entry =  sysapiHpcUnitDbEntryByIndexGet(*unitIndex);
  if (unit_entry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* If we already past the last slot then reset slot to one and
  ** go to the next unit.
  */
  if (*slotIndex >= unit_entry->numPhysSlots)
  {
    (*unitIndex)++;
    if (*unitIndex > hpcSupportedUnitsNumGet())
    {
      return L7_NOT_EXIST;
    }
    *slotIndex = 1;
  }
  else
  {
    (*slotIndex)++;
  }

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Validates whether the provided index is a valid for the slot.
*
* @param    unitIndex
* @param    slotIndex
*
* @returns  L7_SUCCESS  index is valid
* @returns  L7_FAILURE  index is invalid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcSlotDbEntryIndexCheck(L7_uint32 unitIndex, L7_uint32 slotIndex)
{
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_entry;

  /* If unit index is out of range then return error.
   */
  if (unitIndex > hpcSupportedUnitsNumGet() || unitIndex == 0)
  {
    return L7_NOT_EXIST;
  }

  unit_entry =  sysapiHpcUnitDbEntryByIndexGet(unitIndex);
  if (unit_entry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* If slot index is out of range then return an error.
   */
  if (slotIndex > unit_entry->numPhysSlots || slotIndex == 0)
  {
    return L7_NOT_EXIST;
  }

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Returns the slot number of the specified unit/slot.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
* @param    slotNumber  Slot number of this slot.
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcSlotDbEntrySlotNumberGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *slotNumber)
{
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_entry;
  L7_RC_t  rc;

  rc = sysapiHpcSlotDbEntryIndexCheck(unitIndex, slotIndex);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  unit_entry =  sysapiHpcUnitDbEntryByIndexGet(unitIndex);
  if(unit_entry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *slotNumber = unit_entry->physSlot[slotIndex - 1].slot_number;

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Returns the pluggable indicator of the specified unit/slot.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
* @param    pluggable  Slot number of this slot.
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcSlotDbEntryPluggableGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *pluggable)
{
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_entry;
  L7_RC_t  rc;

  rc = sysapiHpcSlotDbEntryIndexCheck(unitIndex, slotIndex);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  unit_entry =  sysapiHpcUnitDbEntryByIndexGet(unitIndex);
  if(unit_entry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *pluggable = unit_entry->physSlot[slotIndex - 1].pluggable;

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Returns the pluggable indicator of the specified unit/slot.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
* @param    powerdown  Slot number of this slot.
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcSlotDbEntryPowerdownGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *powerdown)
{
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_entry;
  L7_RC_t  rc;

  rc = sysapiHpcSlotDbEntryIndexCheck(unitIndex, slotIndex);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  unit_entry =  sysapiHpcUnitDbEntryByIndexGet(unitIndex);
  if(unit_entry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *powerdown = unit_entry->physSlot[slotIndex - 1].power_down;

  return(L7_SUCCESS);
}

/*************************************************
** Supported-Card-In-slot Table Access Functions
*************************************************/

/**************************************************************************
*
* @purpose  Indicates whether specified card is supported in
*           the specified unit and specified slot.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
* @param    cardIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  Card is supported
* @returns  L7_FAILURE  Card is not supported
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcCardInSlotDbEntryGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 cardIndex)
{
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_entry;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *pCardDescriptor;
  L7_RC_t rc;
  L7_uint32 card_type;
  L7_uint32 i;

  /* Make sure that specified slot is valid.
   */
  rc = sysapiHpcSlotDbEntryIndexCheck(unitIndex, slotIndex);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Make sure that specified card index is valid.
   */
  rc = sysapiHpcCardDbEntryIndexCheck(cardIndex);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get card type for the specified card index.
   */
  if ((pCardDescriptor = sysapiHpcCardDbEntryByIndexGet(cardIndex)) != L7_NULLPTR)
  {
    card_type = pCardDescriptor->cardTypeId;
  }
  else
  {
    return L7_FAILURE;
  }

  unit_entry =  sysapiHpcUnitDbEntryByIndexGet(unitIndex);
  if(unit_entry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Check whether specified card type is supported in the specified slot.
   */
  for (i = 0; i < unit_entry->physSlot[slotIndex - 1].num_supported_card_types; i++)
  {
    if (unit_entry->physSlot[slotIndex - 1].supported_cards[i] == card_type)
    {
      return L7_SUCCESS;  /* Card is supported! */
    }
  }

  return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  Returns the next valid CardInSlotIndex.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
* @param    cardIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcCardInSlotDbEntryIndexNextGet(L7_uint32 * unitIndex, L7_uint32 *slotIndex, L7_uint32 *cardIndex)
{
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_entry;
  L7_RC_t rc;

  do
  {
    /* If all indexes are seto to zero then point at the beginning of the table.
     */
    if ((*unitIndex == 0) && (*slotIndex == 0) && (*cardIndex == 0))
    {
      *unitIndex = 1;
      *slotIndex = 1;
      *cardIndex = 1;

    }
    else
    {
      /* If unit index is out of range then return error.
       */
      if (*unitIndex > hpcSupportedUnitsNumGet())
      {
        return L7_NOT_EXIST;
      }

      unit_entry =  sysapiHpcUnitDbEntryByIndexGet(*unitIndex);
      if (unit_entry == L7_NULLPTR)
      {
        return L7_FAILURE;
      }

      /* If we already past the last slot then reset slot to one and
      ** go to the next unit.
      */
      if (*slotIndex > unit_entry->numPhysSlots)
      {
        (*unitIndex)++;
        if (*unitIndex > hpcSupportedUnitsNumGet())
        {
          return L7_NOT_EXIST;
        }
        *slotIndex = 1;
      }
      else
      {
        /* If we are past the last card then reset card to 1 and increment slot.
         */
        if (*cardIndex >= hpcSupportedCardsNumGet())
        {
          (*slotIndex)++;
          if (*slotIndex > unit_entry->numPhysSlots)
          {
            (*unitIndex)++;
            if (*unitIndex > hpcSupportedUnitsNumGet())
            {
              return L7_NOT_EXIST;
            }
            *slotIndex = 1;
          }
          *cardIndex = 1;
        }
        else
        {
          (*cardIndex)++;
        }
      }
    }

    /* We have a valid index. Now check whether card is supported.
     */
    rc = sysapiHpcCardInSlotDbEntryGet(*unitIndex, *slotIndex, *cardIndex);

  } while (rc != L7_SUCCESS); /* loop until we find supported card or return */

  return(L7_SUCCESS);
}

/**********************************************************
** Support-Card Table Access Functions
**********************************************************/

/**************************************************************************
*
* @purpose  Returns card type identifier that corresponds to
*           the index provided.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *cardTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcCardDbCardIdGet(L7_uint32 cardTypeIdIndex, L7_uint32 *cardTypeId)
{
  return(hpcCardDbCardIdGet(cardTypeIdIndex, cardTypeId));
}

/*********************************************************************
* @purpose  Register a routine to be called when a card is plugged
*           or unplugged in the local unit.
*
* @param    *notify      Notification routine with the following parm
*                        @param    slotNum             internal interface number
*                        @param    cardTypeId          card type identifier
*                        @param    event               SYSAPI_CARD_PLUG_EVENT or
*                                                      SYSAPI_CARD_UNPLUG_EVENT
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcCardEventCallbackRegister(void (*notify)(L7_uint32 slotNum, L7_uint32 cardTypeId, L7_uint32 event),
                                           L7_uint32 registrarID)
{
  return(hpcCardEventCallbackRegister(notify, registrarID));
}

/**************************************************************************
*
* @purpose  Reports the local unit's MAC address for use in identifying
*           this unit across the distributed system.
*
* @param    mac   pointer to storage inwhich to store the mac address
*
* @returns  L7_SUCCESS  address retrieved with no problems
* @returns  L7_ERROR  problem encountered in platform specific retrieval function
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalUnitIdentifierMacGet(L7_enetMacAddr_t *mac)
{
  return(hpcLocalUnitIdentifierMacGet(mac));
}

/*********************************************************************
* @purpose  In forms caller if stack is formed and HPC is ready
*           to accept messages for transport amongst stack units.
*
* @param    None
*
* @returns  L7_TRUE      stack is formed, transport is ready
* @returns  L7_FALSE     stack is not formed, transport is not ready
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL sysapiHpcBcastTransportIsReady(void)
{
  return(hpcBcastTransportIsReady());
}

/*********************************************************************
* @purpose  Sets the unit number assigned to the local system.
*
* @param    unit  Unit number to be assigned to the local system
*
* @returns  Nothing
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcUnitNumberSet(L7_uint32 unit)
{
  return(hpcUnitNumberSet(unit));
}

/*********************************************************************
* @purpose  Gets the unit number assigned to the local system. Returns
*           0 if not set.
*
* @param    None
*
* @returns  Unit number assigned to local system (0 if unassigned)
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 sysapiHpcUnitNumberGet(void)
{
  return(hpcUnitNumberGet());
}

/*********************************************************************
* @purpose  Sets the management preference
*
* @param    admin_pref Preference
*
* @returns  Nothing
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcAdminPrefSet(L7_uint32 admin_pref)
{
  return (hpcAdminPrefSet(admin_pref));
}

/*********************************************************************
* @purpose  Gets the configured management preference
*
* @param    None
*
* @returns  Configured management preference
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 sysapiHpcAdminPrefGet(void)
{
  return (hpcAdminPrefGet());
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
L7_RC_t sysapiHpcUnitIdentifierKeyGet(L7_uint32 unit, L7_enetMacAddr_t *key)
{
  return(hpcUnitIdentifierKeyGet(unit, key));
}

/**************************************************************************
*
* @purpose  Gets the MAC address allocation policy for the platform.  The
*           policy returned is one of the ENUM SYSAPI_MAC_POLICY_t.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems getting policy
* @returns  L7_ERROR    (slot, port) data invalid, or other error in MAC
*                       address retrieval
*
* @notes
*
* @end
*
*************************************************************************/
SYSAPI_MAC_POLICY_t sysapiHpcMacAllocationPolicyGet(void)
{
  return(hpcMacAllocationPolicyGet());
}

/*********************************************************************
* @purpose  Gets the MAC address for the port in the local unit
*           for the specified slot and port.
*
* @param    type     interface type of the interface
* @param    slot     slot number of interface for requested MAC address
* @param    port     port number of interface for requested MAC address
* @param    l2_mac_addr  pointer to storage for the retrieved L2 MAC address
* @param    l3_mac_addr  pointer to storage for the retrieved L3 MAC address
*
* @returns  L7_SUCCESS   the MAC for the slot,port successfully found
* @returns  L7_ERROR     problem encountered in getting MAC for slot,port
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 slot, L7_uint32 port, L7_uchar8 *l2_mac_addr, L7_uchar8 *l3_mac_addr)
{
  return(hpcIfaceMacGet(type, slot, port, l2_mac_addr, l3_mac_addr));
}

/*********************************************************************
* @purpose  Sets the local system's status with respect to top of stack
*           role.
*
* @param    role  one of the enum SYSAPI_HPC_TOP_OF_STACK_t
*
* @returns  L7_SUCCESS - if all goes well
* @returns  L7_FAILURE - if problem occurs
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcTopOfStackSet(SYSAPI_STACK_ROLE_t role)
{
  return(hpcTopOfStackSet(role));
}

/*********************************************************************
* @purpose  Returns the role of this system in the stack.
*
* @param    None
*
* @returns  SYSAPI_STACK_ROLE_UNASSIGNED - stand alone or stack not resolved
* @returns  SYSAPI_STACK_ROLE_MANAGEMENT_UNIT - the local system is the management unit
* @returns  SYSAPI_STACK_ROLE_STACK_UNIT - the local system is a stack member but not
*                                the management unit
*
* @comments
*
* @end
*********************************************************************/
SYSAPI_STACK_ROLE_t sysapiHpcTopOfStackGet(void)
{
  return(hpcTopOfStackGet());
}

/*********************************************************************
* @purpose  Gets whether network port is used for stacking.
*
* @param    slot - slot number of port be configured for stacking
* @param    port - port number to be configured for stacking
* @param    stacking_enable - OUT: boolean to enable (L7_TRUE) or disable (L7_FALSE)
*
* @returns  L7_SUCCESS - if all goes well
* @returns  L7_FAILURE - if specified port is not found in the database.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcStackPortGet(L7_uint32 slot, L7_uint32 port, L7_BOOL *stacking_enable)
{
  return hpcStackPortGet(slot, port, stacking_enable);
}

/*********************************************************************
* @purpose  Enables or disables a network port for use in stacking.
*           This information is saved in the HPC component's persistent
*           storage configuration so it is available after reboots.
*
* @param    slot - slot number of port be configured for stacking
* @param    port - port number to be configured for stacking
* @param    stacking_enable - boolean to enable (L7_TRUE) or disable (L7_FALSE)
*
* @returns  L7_SUCCESS - if all goes well
* @returns  L7_FAILURE - if problem occurs
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcStackPortSet(L7_uint32 slot, L7_uint32 port, L7_BOOL stacking_enable)
{
  return(hpcStackPortSet(slot, port, stacking_enable));
}

/*********************************************************************
* @purpose  Queries the platform component as to the largest message
*           size in bytes that the transport can handle.
*
* @param    none
*
* @returns  the max size of message payload in bytes
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 sysapiHpcTransportMaxMessageLengthGet(void)
{
  /* report number of bytes available for callers payload (i.e. max transport can support
  ** minus bytes needed for hpc header
  */
  return(hpcTransportMaxMessageLengthGet());
}

/*********************************************************************
* @purpose  Register a routine to be called when an intrastack message
*           is received for the registrar.
*
* @param    *notify      Notification routine with the following parm
*                        @param    src_key {(input)}   Key (mac-address) of the
                                                       unit that sent the msg
*                        @param    *buffer             pointer to buffer
                                                       containing message
*                        @param    msg_len             length in bytes of message
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcReceiveCallbackRegister(void (*notify)(L7_enetMacAddr_t src_key,
                                                        L7_uchar8* buffer,
                                                        L7_uint32 msg_len),
                                         L7_uint32 registrarID)

{
  return(hpcReceiveCallbackRegister(notify, registrarID));
}


/*********************************************************************
* @purpose  Set the flags for a HPC registrar
*
* @param    registrarID {(input)} routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    flags       {(input)} Flag values
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID
*
* @comments Caller must be registered with HPC
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcRegistrarFlagsSet(L7_uint32 registrarID,
                                   HPC_REGISTRAR_FLAGS_t flags)

{
  return(hpcRegistrarFlagsSet(registrarID, flags));
}


/**************************************************************************
*
* @purpose  Function registered with HPC to get stack event callbacks.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    event        Stack event to register
* @param    *notify      Notification routine with the hpcStackEventMsg_t parm
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID or invalid eventId
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcStackEventRegisterCallback(L7_uint32 registrarId,
                                            HPC_EVENT_t event,
                                            void (*notify)(hpcStackEventMsg_t eventMsg))
{
  return (hpcStackEventRegisterCallback(registrarId, event, notify));
}
/*********************************************************************
* @purpose  Send a message to all other units in the stack.  Message is delivered
*           to corresponding registrarID in remote system.  If there is
*           no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in any of the
*           remote systems, the message is silently discarded there and
*           no notification is sent to the sending caller.
*
*           If the message payload exceeds the platform transport's
*           maximum message size, this function will return an error
*           without sending anything.
*
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    msg_length  number of bytes in buffer
* @param    buffer  pointer to the payload to be sent
*
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport
* @returns  L7_ERROR registrar is not known to HPC message transport
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcBroadcastMessageSend(L7_uint32 registrarID, L7_uint32 msg_length, L7_uchar8* buffer)
{
  return(hpcBroadcastMessageSend(registrarID, msg_length, buffer));
}

/*********************************************************************
* @purpose  Send a message to a specific unit in the stack.  Message is delivered
*           to corresponding registrarID in remote system.  If there is
*           no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in the
*           remote systems, the message is silently discarded there and
*           an error is returned to the sending caller.  This function will
*           hold the caller's thread until the message is either successfully
*           acknowledged or the send times out.
*
*           If the message payload exceeds the platform transport's
*           maximum message size, this function will return an error
*           without sending anything.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    unit         destination unit ID.
* @param    msg_length   number of bytes in payload buffer
* @param    buffer       pointer to the payload to be sent
*
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport, message not acknowledged
* @returns  L7_ERROR registrar is not known to HPC message transport
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcMessageSend(L7_uint32 registrarID, L7_uint32 unit,
                             L7_uint32 msg_length,  L7_uchar8* buffer)
{
  return(hpcMessageSend(registrarID,unit,msg_length,buffer));
}

/*********************************************************************
* @purpose  Resets the Driver to a know state
*
* @param    void
*
* @returns  L7_SUCCESS successfully reset the driver
* @returns  L7_FAILURE problem occurred while resetting the driver
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDriverReset(void)
{
  return(hpcDriverReset());
}

/*********************************************************************
* @purpose  Determine whether the driver is responsible for resyncing new devices
*
* @param    void
*
* @returns  L7_TRUE   if the driver is responsible
* @returns  L7_FALSE  if the driver is not responsible
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL sysapiHpcDriverSyncSupported(void)
{
  return(hpcDriverSyncSupported());
}

/*********************************************************************
* @purpose  Set (Enable/Disable) the ASF mode.
*
* @param    mode        @b{(input)} mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcAsfModeSet(L7_uint32 mode)
{
  L7_RC_t rc;

  rc = hpcDriverAsfModeSet(mode);

  return rc;
}

/*********************************************************************
* @purpose  Get (Enable/Disable) the ASF mode.
*
* @param    currMode    @b{(input)} Current mode L7_ENABLE/L7_DISABLE
* @param    configMode  @b{(input)} Configured mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcAsfModeGet(L7_uint32 *currMode, L7_uint32 *configMode)
{
  return hpcDriverAsfModeGet(currMode, configMode);
}

/**************************************************************************
  Box Services
****************************************************************************/

/**************************************************************************
*
* @purpose  Get total count of temperature sensors, which could be present in the system
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalTempSensorCountGet(void)
{
  L7_uint32 count = 0;

  count = HPC_LOCAL_TEMP_SENSOR_COUNT_GET();

  return count;
}

/**************************************************************************
*
* @purpose  Get total count of power supplies, which could be present in the system
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalPowerSupplyCountGet(void)
{
  L7_uint32 count = 0;
  HPC_UNIT_DESCRIPTOR_t *unit_desc;

  unit_desc = hpcLocalUnitDescriptorGet();
  if (unit_desc != NULL)
  {
    count = unit_desc->unitTypeDescriptor.numPowerSupply;
  }

  return count;
}

/**************************************************************************
*
* @purpose  Get total count of fans, which could be present in the system
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalFanCountGet(void)
{
  L7_uint32 count = 0;
  HPC_UNIT_DESCRIPTOR_t *unit_desc;

  unit_desc = hpcLocalUnitDescriptorGet();
  if (unit_desc != NULL)
  {
    count = unit_desc->unitTypeDescriptor.numFans;
  }

  return count;
}

/**************************************************************************
*
* @purpose  Get total count of SFPs which could be present in the system
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalSfpCountGet(void)
{
  L7_uint32 count = 0;

  count = HPC_LOCAL_SFP_COUNT_GET();

  return count;
}

/**************************************************************************
*
* @purpose  Get total count of XFPs which could be present in the system
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalXfpCountGet(void)
{
  L7_uint32 count = 0;

  count = HPC_LOCAL_XFP_COUNT_GET();

  return count;
}

/**************************************************************************
*
* @purpose  Get temperature sensor status data
*
* @param    in - @b{(input)}        Number of sensor
* @param    data - @b{(output)}     Temperature sensor status data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalTempSensorDataGet(L7_uint32 id, HPC_TEMP_SENSOR_DATA_t* data)
{
  if (id >= sysapiHpcLocalTempSensorCountGet())
    return L7_FAILURE;

  return HPC_LOCAL_TEMP_SENSOR_DATA_GET(id, data);
}

/**************************************************************************
*
* @purpose  Get power supply status data
*
* @param    in - @b{(input)}        Number of power supply
* @param    data - @b{(output)}     Power supply status data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalPowerSupplyDataGet(L7_uint32 id, HPC_POWER_SUPPLY_DATA_t* data)
{
  L7_RC_t rc;

  if (id >= sysapiHpcLocalPowerSupplyCountGet())
    return L7_FAILURE;

  rc = HPC_LOCAL_POWER_SUPPLY_DATA_GET(id, data);
  if (rc == L7_NOT_SUPPORTED)
  {
    /* If the platform doesn't support reading status, assume everything is working. */
    data->itemType = HPC_TYPE_FIXED;
    data->itemState = HPC_ITEMSTATE_OPERATIONAL;
    rc = L7_SUCCESS;
  }
  return rc;
}

/**************************************************************************
*
* @purpose  Get fan status data
*
* @param    in - @b{(input)}        Number of fan
* @param    data - @b{(output)}     fan status data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalFanDataGet(L7_uint32 id, HPC_FAN_DATA_t* data)
{
  L7_RC_t rc;

  if (id >= sysapiHpcLocalFanCountGet())
    return L7_FAILURE;

  rc = HPC_LOCAL_FAN_DATA_GET(id, data);
  if (rc == L7_NOT_SUPPORTED)
  {
    /* If the platform doesn't support reading status, assume everything is working. */
    data->itemType = HPC_TYPE_FIXED;
    data->itemState = HPC_ITEMSTATE_OPERATIONAL;
    rc = L7_SUCCESS;
  }
  return rc;
}

/**************************************************************************
*
* @purpose  Get SFP status data
*
* @param    in - @b{(input)}        SFP index
* @param    data - @b{(output)}     SFP status data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalSfpDataGet(L7_uint32 id, HPC_SFP_DATA_t *data)
{
  if (id >= sysapiHpcLocalSfpCountGet())
    return L7_FAILURE;

  return HPC_LOCAL_SFP_DATA_GET(id, data);
}

/**************************************************************************
*
* @purpose  Get XFP status data
*
* @param    in - @b{(input)}        XFP index
* @param    data - @b{(output)}     XFP status data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalXfpDataGet(L7_uint32 id, HPC_XFP_DATA_t *data)
{
  if (id >= sysapiHpcLocalXfpCountGet())
    return L7_FAILURE;

  return HPC_LOCAL_XFP_DATA_GET(id, data);
}

/**************************************************************************
*
* @purpose  Returns value indicating if POE is supported by this platfrom.
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalPoeSupported(void)
{
  L7_uint32 poeSupport = 0;
  HPC_UNIT_DESCRIPTOR_t *unit_desc;

  unit_desc = hpcLocalUnitDescriptorGet();
  if (unit_desc != NULL)
  {
    poeSupport = unit_desc->unitTypeDescriptor.poeSupport;
  }

  return poeSupport;
}

/**************************************************************************
*
* @purpose  Returns the system OID for this platform.
*
* @param    *sysOID pointer to where the system OID should be copied.
*
* @returns  L7_SUCCESS - valid system OID returned.
* @returns  L7_FAILURE - unable to determine system OID.
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalSystemOidGet(L7_uchar8 *sysOID)
{
  HPC_UNIT_DESCRIPTOR_t *unit_desc;

  unit_desc = hpcLocalUnitDescriptorGet();
  if (unit_desc == NULL)
  {
    return L7_FAILURE;
  }

  strncpy(sysOID, unit_desc->unitTypeDescriptor.systemOID,
          SYSAPI_HPC_SYSTEM_OID_SIZE);
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Returns value indicating if non-stop forwarding is supported
*           by this platfrom.
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalNsfSupported(void)
{
  L7_uint32 nsfSupport = 0;
  HPC_UNIT_DESCRIPTOR_t *unit_desc;

  unit_desc = hpcLocalUnitDescriptorGet();
  if (unit_desc != NULL)
  {
    nsfSupport = unit_desc->unitTypeDescriptor.nsfSupport;
  }

  return nsfSupport;
}

/*********************************************************************
* @purpose  Reads the temperature of the SFP.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    temperature - @b{(output)}  temperature value
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    temperature is returned in degrees Celsius
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagTempRead(L7_uint32 unit, L7_uint32 port, L7_int32 *temperature)
{
  return HPC_DIAG_TEMP_READ(unit, port, temperature);
}

/*********************************************************************
* @purpose  Reads the voltage of the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    voltage  - @b{(output)}  voltage value
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    voltage is returned in millivolts
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagVoltageRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *voltage)
{
  return HPC_DIAG_VOLTAGE_READ(unit, port, voltage);
}

/*********************************************************************
* @purpose  Reads the current of the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    current  - @b{(output)}  current value
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    temperature is returned in degrees Celsius
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagCurrentRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *current)
{
  return HPC_DIAG_CURRENT_READ(unit, port, current);
}

/*********************************************************************
* @purpose  Reads the tx Power of the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    txPower  - @b{(output)}  Transmit Power value
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    Power is returned in degrees microWatts
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagTxPwrRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *txPower)
{
  return HPC_DIAG_TX_POWER_READ(unit, port, txPower);
}

/*********************************************************************
* @purpose  Reads the rx Power of the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    rxPower  - @b{(output)}  Receive Power value
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    Power is returned in degrees microWatts
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagRxPwrRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *rxPower)
{
  return HPC_DIAG_RX_POWER_READ(unit, port, rxPower);
}

/*********************************************************************
* @purpose  Reads the tx fault status of the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    txFault  - @b{(output)}  Transmit fault status
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    Power is returned in degrees microWatts
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagTxFaultRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *txFault)
{
  return HPC_DIAG_TX_FAULT_READ(unit, port, txFault);
}

/*********************************************************************
* @purpose  Reads the signal LOS on the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    los      - @b{(output)}  los
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    'los' is returned as 1 if LOS is asserted.
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagLosRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *los)
{
  return HPC_DIAG_LOS_READ(unit, port, los);
}

/*********************************************************************
* @purpose  Reads the cable diagnostics for a copper SFP.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    cd       - @b{(output)}  cable diagnostics
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagCopperSfpRead(L7_uint32 unit, L7_uint32 port, void *cd)
{
  return HPC_DIAG_COPPER_SFP_READ(unit, port, cd);
}

/*********************************************************************
* @purpose  SFP power control through CPLD/or other means
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port  - @b{(input)}  port
*
* @returns  void
*
* @end
*********************************************************************/
void sysapiHpcDiagPowerCtrl(L7_uint32 unit, L7_uint32 port)
{
  if ( L7_NOT_SUPPORTED == HPC_DIAG_POWER_CTRL(unit, port))
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "sysapiHpcSfpPowerCtrl: Pwr ctrl is not supported\n");
}

/*********************************************************************
* @purpose  Custom LED control
*
* @param    unit  - @b{(input)}   bcm unit Number.
* @param    port  - @b{(input)}   bcm port Number.
* @param    link  - @b{(input)}   link status.
*
* @returns  void
*
* @end
*********************************************************************/
void sysapiHpcLedModeCustomization(int unit, int port, int link)
{
#ifdef LVL7_DNI8541
  hpcLedCustomization(unit, port, link);
#else
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID, "%s: LED Customization is not supported\r\n", __FUNCTION__);
#endif
}

/*********************************************************************
* @purpose  Notify power controller of presence of 10GBASE-T

* @param    unit     - @b{(input)}   bcm unit Number.
* @param    xfpPort  - @b{(input)}   bcm Port Number.
* @param    present  - @b{(input)}   present or not.
*
* @returns  void
*
* @end
*********************************************************************/
void sysapiHpcNotify10GBASETPresence(L7_uint32 unit, L7_uint32 xfpPort,
                                     L7_BOOL present)
{
  if ( L7_NOT_SUPPORTED == HPC_SENSOR_10GBASET_PRSNCE(unit, xfpPort, present))
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "sysapiHpcNotify10GBASETPresence: 10GBASET ctrl is not supported\n");
}

/*********************************************************************
* @purpose  Notify power controller of presence of SFP+

* @param    unit     - @b{(input)}   bcm unit Number.
* @param    xfpPort  - @b{(input)}   bcm Port Number.
* @param    present  - @b{(input)}   present or not.
*
* @returns  void
*
* @end
*********************************************************************/
void sysapiHpcNotifySfpPlusPresence(L7_uint32 unit, L7_uint32 sfpPort)
{
  if ( L7_NOT_SUPPORTED == HPC_SENSOR_SFPPLUS_PRSNCE(unit, sfpPort))
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "sysapiHpcNotifySFP+Presence: SFP+ ctrl is not supported\n");

}

#ifdef L7_POE_PACKAGE
/**************************************************************************
*
* @purpose  Send a PoE message to the HPC layer.
*
* @param    PoE Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE or other error code returned from controller
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcPoeMsgSend(SYSAPI_POE_MSG_t *msg)
{
  msg->rc = hpcPoeControllerMsg(msg);
  msg->direction = SYSAPI_HPC_POE_CMD_RESPONSE;

  return L7_SUCCESS;
}

extern void *poeReqQ;                   /* PoE Request Message Queue */

/**************************************************************************
*
* @purpose  Send a PoE trap to the application layer.
*
* @param    L7_uint   port number
* @param    enum      trapType
* @param    void      pointer to trap data
*
* @returns  L7_SUCCESS  if trap successfully sent
* @returns  L7_FAILURE  otherwise
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcPoeTrapSend(L7_uint port, SYSAPI_POE_CONTROLLER_MSG_t trapType, void *data)
{
  SYSAPI_POE_MSG_t msg;

  switch (trapType)
  {
    case SYSAPI_HPC_POE_TRAP_MAIN_THRESHOLD_CROSSED:
      msg.cmdData.trapMainThresholdCrossed = *(SYSAPI_POE_TRAP_DATA_THRESHOLD_CROSSED_t *)data;
      break;

    case SYSAPI_HPC_POE_TRAP_PORT_CHANGE:
      msg.cmdData.trapPortChange = *(SYSAPI_POE_TRAP_DATA_PORT_CHANGE_t *)data;
      break;

    case SYSAPI_HPC_POE_TRAP_MAIN_INIT_COMPLETE:
      msg.cmdData.mainInit = *(SYSAPI_POE_TRAP_DATA_MAIN_INIT_t *)data;
      break;

    default:
      return L7_NOT_EXIST;
  }

  msg.msgId     = trapType;
  msg.cmdType   = SYSAPI_HPC_POE_CMD_TRAP;
  msg.direction = SYSAPI_HPC_POE_CMD_REQUEST;
  msg.rc        = L7_SUCCESS;
  msg.usp.unit  = sysapiHpcUnitNumberGet();
  msg.usp.slot  = 0;
  msg.usp.port  = port;

  return osapiMessageSend(poeReqQ, &msg, sizeof(msg), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);
}
#endif  /* L7_POE_PACKAGE */

/*********************************************************************
* @purpose  This routine determines if there are any stack ports.
*           If no stack ports, then unit manager can use this
*           info to speed up it's state machines.
*
* @returns  L7_BOOL
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL sysapiHpcNoStackPorts()
{
  return hpcNoStackPorts();
}

/*********************************************************************
* @purpose  Inform the driver that this unit is the standby of the stack
*
* @param    stby_present {(input)} L7_TRUE: There is a valid stby
*                                L7_FALSE: There is no valid stby
* @param
* @param    stby_key {(input)} Key of the stby unit
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDriverNotifyStandbyStatus(L7_BOOL stby_present,
                                           L7_enetMacAddr_t stby_key)
{
  return hpcDriverNotifyStandbyStatus(stby_present, stby_key);
}

/*********************************************************************
* @purpose  UM on each unit informs the driver that the current manager
*           of the stack is no longer present. Driver tries to remove
*           the manager key from ATP/Next-hop transport layers so that
*           pending/future communication with the failed manager is
*           prevented.
*
* @param    managerKey: CPU key of the Manager
*
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void sysapiHpcDriverRemoveStackManager(L7_enetMacAddr_t managerKey)
{
  return hpcDriverRemoveStackManager(managerKey);
}

/*********************************************************************
* @purpose  Instruct the driver to shutdown all the front-panel ports
*           for a unit.
*
* @param    unitNumber {(input)} Unit for which the ports are to be shut.
*                                L7_ALL_UNITS indicates all the valid
*                                stack members.
*
*
* @returns  L7_RC_t
*
* @comments Called by UM before move-management
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDriverShutdownPorts(L7_uint32 unitNumber)
{
  return hpcDriverShutdownPorts(unitNumber);
}

/**************************************************************************
*
* @purpose  Return the physical port count for a given card ID.
*
* @returns  physicalPortCount   Number of physical ports on given card ID.
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 hpcPhysPortsOnCardGet(L7_uint32 cardId)
{
  L7_uint32 physPortCnt = 0;
  HPC_CARD_DESCRIPTOR_t *card_descriptor_ptr;

  card_descriptor_ptr = hpcCardDbLookup(cardId);
  if (card_descriptor_ptr != NULL)
    physPortCnt = card_descriptor_ptr->cardTypeDescriptor.numOfNiPorts;

  return physPortCnt;
}

void sysapiHpcDebugSlotPortTableShow()
{
  L7_uint32 i;

  for (i = 0; i < L7_MAX_PHYSICAL_SLOTS_PER_UNIT; i++)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "slot %d Max Port count %d \n", i, hpcSlotPortCount [i]);
  }

  return;
}


/**************************************************************************
*
* @purpose  Return the number of physical ports, given a slot number.
*
* @param    slotNum      slot number for requested card ID.
*
* @returns  portCount    Number of physical ports in slot number
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcPhysPortsInSlotGet(int slotNum)
{
  L7_uint32 portCount = 0;

  if (slotNum < L7_MAX_PHYSICAL_SLOTS_PER_UNIT)
  {
    portCount =  hpcSlotPortCount[slotNum];
  }

  return(portCount);
}

/**************************************************************************
*
* @purpose  From the HPC database, populate hpcSlotPortCount array. This array
*           contains the count of maximum number of physical ports supported
*           for each slot.
*
* @param    none
*
* @returns  none
*
*
* @end
*
*************************************************************************/
void sysapiHpcSlotPortCountPopulate(void)
{
  L7_uint32                      slot, idx;
  L7_uint32                      unitTypeIdIndex, cardId;
  L7_uint32                      numPorts;
  L7_RC_t                        rc;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t   *unitDesc = L7_NULLPTR;
  SYSAPI_PHYS_SLOT_MAP_t         *slotMap = L7_NULLPTR;

  /* For each slot, walk through all the supported unit type and calculate the max number of
  ** ports.
  */
  for (slot = 0; slot < L7_MAX_PHYSICAL_SLOTS_PER_UNIT; slot++)
  {
    unitTypeIdIndex = 0;
    while ((rc = sysapiHpcUnitDbEntryIndexNextGet(&unitTypeIdIndex)) == L7_SUCCESS)
    {
      unitDesc =  sysapiHpcUnitDbEntryByIndexGet(unitTypeIdIndex);
      if (L7_NULLPTR == unitDesc)
      {
        continue;
      }

      slotMap = L7_NULLPTR;

      /* Get the entry for the given slot number in Unit Descriptor */
      for (idx = 0; idx < unitDesc->numPhysSlots; idx++)
      {
        if (unitDesc->physSlot[idx].slot_number == slot)
        {
          slotMap = &(unitDesc->physSlot[idx]);
          break;
        }
      }

      if (L7_NULLPTR == slotMap)
      {
        continue;
      }

      /* Walk through the supported card-types in this unit/slot */
      for (idx = 0; idx < slotMap->num_supported_card_types; idx++)
      {
        cardId = slotMap->supported_cards[idx];
        numPorts =  hpcPhysPortsOnCardGet(cardId);
        if (numPorts > hpcSlotPortCount[slot])
        {
          hpcSlotPortCount[slot] = numPorts;
        }
      }

    } /* End while */

  } /* for (slot = 0; slot < L7_MAX_PHYSICAL_SLOTS_PER_UNIT; slot++)*/

  return;

}

/**************************************************************************
*
* @purpose  This function provides registration of callback
*           function for HPC events.
*
* @param    handle        The Event Handler handle.
* @param    eventInfo     The Event Information. Which has component ID and
*                         callback function.
*
* @returns  L7_SUCCESS  On successful registration.
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcEventCallbackRegister(L7_EVENT_HANDLE_t *handle,
                L7_EVENT_DATA_t *eventInfo)
{
    if (L7_NULL == eventInfo)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "%s: %d: sysapiHpcEventHandlerCallbackRegister:"
                "Event info pasessed as NULL\n",
          __FILE__, __LINE__);
      return L7_FAILURE;
    }

    /* Invoke callback function for local card and logical card plug-ins.*/
    (void)hpcHardwareCardEventCallbackNotify(eventInfo->notify);

    *handle = 0;
    return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Retrieves the Module Type and the module index for a give
 *           Physical Slot
 *
 * @param    physicallot     @b{(input)}  Physical Slot Number
 *           moduleType      @b{(output)} Module Type
 *           moduleIndex     @b{(output)} Module Index
 *
 *
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if specified physical slot could not be found
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t sysapiHpcSlotMapMPModuleInfoGet(L7_int32 physicalSlot,
        SYSAPI_CARD_TYPE_t *moduleType,
        SYSAPI_CARD_INDEX_t *moduleIndex)
{
  switch(physicalSlot)
  {
    case(L7_LAG_SLOT_NUM):
      *moduleType = SYSAPI_CARD_TYPE_LAG;
      break;
    case(L7_VLAN_SLOT_NUM):
      *moduleType = SYSAPI_CARD_TYPE_VLAN_ROUTER;
      break;
    case(L7_CPU_SLOT_NUM):
      *moduleType = SYSAPI_CARD_TYPE_LOGICAL_CPU;
      break;
    case(L7_LOOPBACK_SLOT_NUM):
      *moduleType = SYSAPI_CARD_TYPE_LOOPBACK;
      break;
    case(L7_TUNNEL_SLOT_NUM):
      *moduleType = SYSAPI_CARD_TYPE_TUNNEL;
      break;
    case(L7_CAPWAP_TUNNEL_SLOT_NUM):
      *moduleType = SYSAPI_CARD_TYPE_CAPWAP_TUNNEL;
      break;
    /* PTin added: virtual port */
    case(L7_VLAN_PORT_SLOT_NUM):
      *moduleType = SYSAPI_CARD_TYPE_VLAN_PORT;
      break;
    default:
      *moduleType = SYSAPI_CARD_TYPE_LINE;
      break;
  }

  *moduleIndex = SYSAPI_CARD_INDEX_INVALID;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Routine for application helpers to send packets on the local
*           unit.
*
* @param    frame              @b{(input)}
* @param    frameSize          @b{(input)}
* @param    priority           @b{(input)}
* @param    slot               @b{(input)}
* @param    port               @b{(input)}
* @param    ignoreEgressRules  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcHelperSend(L7_uchar8   *frame,
                            L7_uint32    frameSize,
                            L7_uint8     priority,
                            L7_uint8     slot,
                            L7_ushort16  port,
                            L7_BOOL      ignoreEgressRules)
{
  return hpcDriverHelperSend(frame, frameSize, priority, slot, port, ignoreEgressRules);
}

/*********************************************************************
*
* @purpose Allocate memory for the CPU send stats for HPC helper.
*          Zero out the stat values.
*
* @returns
*
* @notes   none
*
* @end
*
*********************************************************************/
void sysapiHpcHelperStatsInit()
{
  hpcDriverHelperStatsInit();
}

/*********************************************************************
* @purpose  Returns a value indicating if the driver is performing a restart
*
* @param    void
*
* @returns  L7_TRUE    the driver performing a restart
* @returns  L7_FALSE   the driver is not performing a restart
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL sysapiHpcDriverInRestart(void)
{
  return hpcDriverInRestart();
}
