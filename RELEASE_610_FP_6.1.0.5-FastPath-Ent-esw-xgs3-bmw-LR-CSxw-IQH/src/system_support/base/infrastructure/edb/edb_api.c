/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   edb_api.c
*
* @purpose    API code for Entity MIB - RFC 2737
*
* @component  SNMP
*
* @comments   Provides access to the database supporting the 
*             entityPhysicalTable.
*
* @create     6/06/2003
*
* @author     cpverne
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "usmdb_cardmgr_api.h"
#include "usmdb_registry_api.h"
#include "edb.h"
#include "edb_sid.h"
#include "unitmgr_api.h"
#include "cardmgr_api.h"
#include <ctype.h> /* for isalpha */

L7_uint32 L7_edb_task_id = 0;
L7_uint32 L7_edbTrap_task_id = 0;

/* Begin Function Definitions: edb_api.h */

/***************************************************************************************/
/***** entPhysicalTable ****************************************************************/
/***************************************************************************************/

/*********************************************************************
*
* @purpose Checks to see if the given entityPhysicalEntry exists.
*
* @param physicalIndex @b((input))  index of the entityPhysicalEntry to check
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalEntryGet(L7_uint32 physicalIndex)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 maxIndex;

  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  if (edbTreeMaxIndex(&maxIndex) == L7_SUCCESS &&
      maxIndex > 0 &&
      physicalIndex > 0 &&
      physicalIndex <= maxIndex)
  {
    rc = L7_SUCCESS;
  }

  (void)osapiSemaGive(edbSemaId);

  return rc;
}

/*********************************************************************
*
* @purpose Retrieves the next valid entityPhysicalEntry.
*
* @param physicalIndex @b((input,output)) pointer to receive next index
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @notes Uses the current value of physicalIndex and finds the next valid index
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalEntryNextGet(L7_uint32 *physicalIndex)
{
  /* physicalIndex will always be a contiguous index */
  *physicalIndex += 1;

  return edbPhysicalEntryGet(*physicalIndex);
}

/*********************************************************************
*
* @purpose Returns the Description value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param physicalDesc @b((output))  pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalDescrGet(L7_uint32 physicalIndex, L7_char8 *physicalDescr)
{
  L7_uint32 unit_type = 0;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_descriptor = L7_NULLPTR;

  L7_EDB_OBJECT_t *object_p;
  L7_RC_t rc = L7_FAILURE;
  
  L7_uint32 intIfNum=0;
  nimUSP_t usp;

  L7_uint32 card_id=0;
  L7_uint32 card_index=0;

  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  if (edbObjectFind(physicalIndex, &object_p) == L7_SUCCESS)
  {
    switch (object_p->objectType)
    {
    case L7_EDB_OBJECT_TYPE_UNIT:
      if (unitMgrUnitTypeGet(object_p->relPos, &unit_type) == L7_SUCCESS)
      {
        unit_descriptor = sysapiHpcUnitDbEntryGet(unit_type);

        if (unit_descriptor != L7_NULLPTR)
        {
          (void)strcpy(physicalDescr, unit_descriptor->unitDescription);
          rc = L7_SUCCESS;
        }
      }
      break;
      
    case L7_EDB_OBJECT_TYPE_PORT:
      usp.unit = object_p->unit;
      usp.slot = object_p->slot;
      usp.port = object_p->relPos;

      if (nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
      {
        rc = nimGetIfDescr(intIfNum, physicalDescr);
      }
      break;

    case L7_EDB_OBJECT_TYPE_SLOT:
    case L7_EDB_OBJECT_TYPE_CARD:
      if (object_p->objectType == L7_EDB_OBJECT_TYPE_SLOT)
      {
        /* check to see if this slot contains cards */
        if (object_p->child != L7_NULLPTR &&
            object_p->child->objectType == L7_EDB_OBJECT_TYPE_CARD)
        {
          /* carded slots don't have descriptions */
          strcpy(physicalDescr, "");
          rc = L7_SUCCESS;
          break;
        }
      }
  
      if (usmDbCardInsertedCardTypeGet(object_p->unit, object_p->slot, &card_id) == L7_SUCCESS)
      {
        if (usmDbCardIndexFromIDGet(card_id, &card_index) == L7_SUCCESS)
        {
          if (usmDbCardSupportedCardDescriptionGet(card_index, physicalDescr) == L7_SUCCESS)
          {
            rc = L7_SUCCESS;
            break;
          }
        }
      }
      break;

    case L7_EDB_OBJECT_TYPE_UNKNOWN:
    case L7_EDB_OBJECT_TYPE_STACK:
    case L7_EDB_OBJECT_TYPE_POWERSUPPLY:
    case L7_EDB_OBJECT_TYPE_FAN:
    default:
      strcpy(physicalDescr, "");
      rc = L7_SUCCESS;
      break;
    }
  }

  (void)osapiSemaGive(edbSemaId);

  return rc;
}

/*********************************************************************
*
* @purpose Returns the VendorType value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param vendorType @b((output))    pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalVendorTypeGet(L7_uint32 physicalIndex, L7_char8 *vendorType)
{
  strcpy(vendorType, "0.0");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Returns the ContainedIn value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param containedIn @b((output))   pointer to unsigned integer to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalContainedInGet(L7_uint32 physicalIndex, L7_uint32 *containedIn)
{
  L7_EDB_OBJECT_t *object_p;
  L7_RC_t rc = L7_FAILURE;

  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  if (edbObjectFind(physicalIndex, &object_p) == L7_SUCCESS)
  {
    if (object_p->parent != L7_NULLPTR)
    {
      *containedIn = object_p->parent->physicalIndex;
    }
    else
    {
      *containedIn = 0;
    }

    rc = L7_SUCCESS;
  }

  (void)osapiSemaGive(edbSemaId);

  return rc;
}

/*********************************************************************
*
* @purpose Returns the Class value of a physical object
*
* @param physicalIndex @b((input))   index of the physical object
* @param physicalClass @b((output))  pointer to unsigned integer to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalClassGet(L7_uint32 physicalIndex, L7_uint32 *physicalClass)
{
  L7_EDB_OBJECT_t *object_p;
  L7_RC_t rc = L7_FAILURE;

  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  if (edbObjectFind(physicalIndex, &object_p) == L7_SUCCESS)
  {
    *physicalClass = object_p->objectClass;
    rc = L7_SUCCESS;
  }

  (void)osapiSemaGive(edbSemaId);

  return rc;
}

/*********************************************************************
*
* @purpose Returns the ParentRelPos value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param parentRelPos @b((output))  pointer to integer to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalParentRelPosGet(L7_uint32 physicalIndex, L7_int32 *parentRelPos)
{
  L7_EDB_OBJECT_t *object_p;
  L7_RC_t rc = L7_FAILURE;

  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  if (edbObjectFind(physicalIndex, &object_p) == L7_SUCCESS)
  {
    *parentRelPos = object_p->relPos;
    rc = L7_SUCCESS;
  }

  (void)osapiSemaGive(edbSemaId);

  return rc;
}

/*********************************************************************
*
* @purpose Returns the Name value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param name @b((output))          pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalNameGet(L7_uint32 physicalIndex, L7_char8 *name)
{
  L7_uint32 card_type = 0;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *card_descriptor = L7_NULLPTR;

  L7_EDB_OBJECT_t *object_p;
  L7_RC_t rc = L7_FAILURE;

  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  if (edbObjectFind(physicalIndex, &object_p) == L7_SUCCESS)
  {
    switch (object_p->objectType)
    {
    case L7_EDB_OBJECT_TYPE_UNKNOWN:
      (void)strcpy(name, "Unknown");
      rc = L7_SUCCESS;
      break;

    case L7_EDB_OBJECT_TYPE_STACK:
      (void)strcpy(name, "Stack");
      rc = L7_SUCCESS;
      break;

    case L7_EDB_OBJECT_TYPE_UNIT:
      (void)sprintf(name, "Unit %d", object_p->relPos);
      rc = L7_SUCCESS;
      break;

    case L7_EDB_OBJECT_TYPE_SLOT:
      (void)sprintf(name, "Slot %d.%d", object_p->unit, object_p->relPos);
      rc = L7_SUCCESS;
      break;

    case L7_EDB_OBJECT_TYPE_CARD:
      if (cmgrCardInsertedCardTypeGet(object_p->unit, object_p->slot, &card_type) == L7_SUCCESS)
      {
        card_descriptor = sysapiHpcCardDbEntryGet(card_type);

        if (card_descriptor != L7_NULLPTR)
        {
          switch (card_descriptor->type)
          {
          case SYSAPI_CARD_TYPE_NOT_PRESENT:
            (void)strcpy(name, "Not Present");
            rc = L7_SUCCESS;
            break;
          case SYSAPI_CARD_TYPE_LINE:
            (void)strcpy(name, "Line Card");
            rc = L7_SUCCESS;
            break;
          case SYSAPI_CARD_TYPE_LAG:
            (void)strcpy(name, "Lag Card");
            rc = L7_SUCCESS;
            break;
          case SYSAPI_CARD_TYPE_VLAN_ROUTER:
            (void)strcpy(name, "VLAN Router Card");
            rc = L7_SUCCESS;
            break;
          case SYSAPI_CARD_TYPE_CPU:
            (void)strcpy(name, "CPU Card");
            rc = L7_SUCCESS;
            break;
          case SYSAPI_CARD_TYPE_FAN:
            (void)strcpy(name, "Fan Card");
            rc = L7_SUCCESS;
            break;
          case SYSAPI_CARD_TYPE_POWER_SUPPLY:
            (void)strcpy(name, "Power Supply Card");
            rc = L7_SUCCESS;
            break;
          case SYSAPI_CARD_TYPE_LOOPBACK:
            (void)strcpy(name, "Loopback Card");
            rc = L7_SUCCESS;
            break;
          case SYSAPI_CARD_TYPE_TUNNEL:
            (void)strcpy(name, "Tunnel Card");
            rc = L7_SUCCESS;
            break;
          case SYSAPI_CARD_TYPE_CAPWAP_TUNNEL:
            (void)strcpy(name, "CAPWAP Tunnel Card");
            rc = L7_SUCCESS;
            break;
          default:
            /* unknown card type */
            (void)strcpy(name, "");
            rc = L7_SUCCESS;
            break;
          }
        }
      }
      break;

    case L7_EDB_OBJECT_TYPE_PORT:
      (void)sprintf(name, "Port %d.%d.%d", object_p->unit, object_p->slot, object_p->relPos);
      rc = L7_SUCCESS;
      break;

    case L7_EDB_OBJECT_TYPE_POWERSUPPLY:
      (void)sprintf(name, "Power Supply %d", object_p->relPos);
      rc = L7_SUCCESS;
      break;

    case L7_EDB_OBJECT_TYPE_FAN:
      (void)sprintf(name, "Fan %d", object_p->relPos);
      rc = L7_SUCCESS;
      break;

    default:
      (void)strcpy(name, "");
      rc = L7_SUCCESS;
      break;

    }
  }

  (void)osapiSemaGive(edbSemaId);
  return rc;
}

/*********************************************************************
*
* @purpose Returns the HardwareRev value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param hardwareRev @b((output))   pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalHardwareRevGet(L7_uint32 physicalIndex, L7_char8 *hardwareRev)
{
  /* Not Implemented Yet */
  strcpy(hardwareRev, "");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Returns the FirmwareRev value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param firmwareRev @b((output))   pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalFirmwareRevGet(L7_uint32 physicalIndex, L7_char8 *firmwareRev)
{
  /* Not Implemented Yet */
  strcpy(firmwareRev, "");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Returns the SoftwareRev value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param softwareRev @b((output))   pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalSoftwareRevGet(L7_uint32 physicalIndex, L7_char8 *softwareRev)
{
  codeVersion_t ver;
  L7_EDB_OBJECT_t *object_p;
  L7_RC_t rc = L7_FAILURE;
  
  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  if (edbObjectFind(physicalIndex, &object_p) == L7_SUCCESS)
  {
    switch (object_p->objectType)
    {
    case L7_EDB_OBJECT_TYPE_UNIT:
      /* only check unit manager if stacking is present */
      if (edbStackPresent() == L7_TRUE)
      {
        if (unitMgrDetectCodeVerRunningGet(object_p->relPos, &ver) == L7_SUCCESS)
        {
          if (isalpha(ver.rel))
          {
            sprintf(softwareRev, "%c.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
          }
          else
          {
            sprintf(softwareRev, "%d.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
          }
          rc = L7_SUCCESS;
        }
      }
      else
      {
        rc = usmDbSwVersionGet(object_p->relPos, softwareRev);
      }
      break;
      
    case L7_EDB_OBJECT_TYPE_UNKNOWN:
    case L7_EDB_OBJECT_TYPE_STACK:
    case L7_EDB_OBJECT_TYPE_SLOT:
    case L7_EDB_OBJECT_TYPE_CARD:
    case L7_EDB_OBJECT_TYPE_PORT:
    case L7_EDB_OBJECT_TYPE_POWERSUPPLY:
    case L7_EDB_OBJECT_TYPE_FAN:
    default:
      /* Not Implemented Yet */
      strcpy(softwareRev, "");
      rc = L7_SUCCESS;
      break;
    }
  }

  (void)osapiSemaGive(edbSemaId);

  return rc;
}

/*********************************************************************
*
* @purpose Returns the SerialNum value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param serialNum @b((output))     pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalSerialNumGet(L7_uint32 physicalIndex, L7_char8 *serialNum)
{
  L7_EDB_OBJECT_t *object_p;
  L7_RC_t rc = L7_FAILURE;
  
  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  if (edbObjectFind(physicalIndex, &object_p) == L7_SUCCESS)
  {
    switch (object_p->objectType)
    {
    case L7_EDB_OBJECT_TYPE_UNIT:
      rc = usmDbSwDevInfoSerialNumGet(object_p->relPos, serialNum);
      break;
      
    case L7_EDB_OBJECT_TYPE_UNKNOWN:
    case L7_EDB_OBJECT_TYPE_STACK:
    case L7_EDB_OBJECT_TYPE_SLOT:
    case L7_EDB_OBJECT_TYPE_CARD:
    case L7_EDB_OBJECT_TYPE_PORT:
    case L7_EDB_OBJECT_TYPE_POWERSUPPLY:
    case L7_EDB_OBJECT_TYPE_FAN:
    default:
      /* Not Implemented Yet */
      strcpy(serialNum, "");
      rc = L7_SUCCESS;
      break;
    }
  }

  (void)osapiSemaGive(edbSemaId);

  return rc;
}

/*********************************************************************
*
* @purpose Sets the SerialNum value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param serialNum @b((input))      pointer to string that contains value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalSerialNumSet(L7_uint32 physicalIndex, L7_char8 *serialNum)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
* @purpose Returns the MfgName value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param mfgName @b((output))       pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalMfgNameGet(L7_uint32 physicalIndex, L7_char8 *mfgName)
{
  /* Not Implemented Yet */
  strcpy(mfgName, "");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Returns the ModelName value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param modelName @b((output))     pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalModelNameGet(L7_uint32 physicalIndex, L7_char8 *modelName)
{
  L7_EDB_OBJECT_t *object_p=L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 card_id=0;
  L7_uint32 card_index=0;

  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  if (edbObjectFind(physicalIndex, &object_p) == L7_SUCCESS)
  {
    switch (object_p->objectType)
    {
    case L7_EDB_OBJECT_TYPE_UNIT:
      rc = unitMgrModelIdentifierHPCGet(object_p->relPos, modelName);
      break;
      
    case L7_EDB_OBJECT_TYPE_SLOT:
    case L7_EDB_OBJECT_TYPE_CARD:
      if (object_p->objectType == L7_EDB_OBJECT_TYPE_SLOT)
      {
        /* check to see if this slot contains cards */
        if (object_p->child != L7_NULLPTR &&
            object_p->child->objectType == L7_EDB_OBJECT_TYPE_CARD)
        {
          /* carded slots don't have model names */
          strcpy(modelName, "");
          rc = L7_SUCCESS;
          break;
        }
      }

      if (usmDbCardInsertedCardTypeGet(object_p->unit, object_p->slot, &card_id) == L7_SUCCESS)
      {
        if (usmDbCardIndexFromIDGet(card_id, &card_index) == L7_SUCCESS)
        {
          if (usmDbCardSupportedCardModelIdGet(card_index, modelName) == L7_SUCCESS)
          {
            rc = L7_SUCCESS;
            break;
          }
        }
      }
      break;

    case L7_EDB_OBJECT_TYPE_UNKNOWN:
    case L7_EDB_OBJECT_TYPE_STACK:
    case L7_EDB_OBJECT_TYPE_PORT:
    case L7_EDB_OBJECT_TYPE_POWERSUPPLY:
    case L7_EDB_OBJECT_TYPE_FAN:
    default:
      /* Not Implemented Yet */
      strcpy(modelName, "");
      rc = L7_SUCCESS;
      break;
    }
  }

  (void)osapiSemaGive(edbSemaId);

  return rc;
}

/*********************************************************************
*
* @purpose Returns the Alias value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param alias @b((output))         pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalAliasGet(L7_uint32 physicalIndex, L7_char8 *alias)
{
  /* Not Implemented Yet */
  strcpy(alias, "");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Sets the Alias value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param alias @b((output))         pointer to string that contains value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalAliasSet(L7_uint32 physicalIndex, L7_char8 *alias)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
* @purpose Returns the AssetID value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param assetID @b((output))       pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalAssetIDGet(L7_uint32 physicalIndex, L7_char8 *assetID)
{
  /* Not Implemented Yet */
  strcpy(assetID, "");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Sets the AssetID value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param assetID @b((output))       pointer to string that contains value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalAssetIDSet(L7_uint32 physicalIndex, L7_char8 *assetID)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
*
* @purpose Returns the IsFRU value of a physical object
*
* @param physicalIndex @b((input))  index of the physical object
* @param isFRU @b((output))         pointer to string to receive value
*
* @returns L7_SUCCESS  if the object exists
* @returns L7_FAILURE  if the object does not exist
*
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalIsFRUGet(L7_uint32 physicalIndex, L7_BOOL *isFRU)
{
  L7_EDB_OBJECT_t *object_p;
  L7_RC_t rc = L7_FAILURE;

  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  if (edbObjectFind(physicalIndex, &object_p) == L7_SUCCESS)
  {
    if (object_p->parent != L7_NULLPTR &&
        (object_p->objectType == L7_EDB_OBJECT_TYPE_CARD ||
         object_p->objectType == L7_EDB_OBJECT_TYPE_UNIT))
    {
      *isFRU = L7_TRUE;
    }
    else
    {
      *isFRU = L7_FALSE;
    }

    rc = L7_SUCCESS;
  }

  (void)osapiSemaGive(edbSemaId);

  return rc;
}

/***************************************************************************************/
/***** entContainsTable ****************************************************************/
/***************************************************************************************/

/*********************************************************************
*
* @purpose Checks to see if the given physical object contains the child object
*
* @param physicalIndex @b((input))  index of the physical object
* @param childIndex @b((input))     index of the child object
*
* @returns L7_SUCCESS  if the physical object contains the child object
* @returns L7_FAILURE  otherwise
*
* @notes Performs check by looking up the child index, and checking it's parent
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalContainsEntryGet(L7_uint32 physicalIndex, L7_uint32 childIndex)
{
  L7_EDB_OBJECT_t *object_p;
  L7_RC_t rc = L7_FAILURE;

  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  /* find the child and see if the parent matches the physicalIndex */
  if (edbObjectFind(childIndex, &object_p) == L7_SUCCESS &&
      object_p->parent != L7_NULLPTR &&
      object_p->parent->physicalIndex == physicalIndex)
  {
    rc = L7_SUCCESS;
  }

  (void)osapiSemaGive(edbSemaId);

  return rc;
}

/*********************************************************************
*
* @purpose Returns the next valid parent/child pair
*
* @param physicalIndex @b((input,output))  index of the physical object
* @param childIndex @b((input,output))     index of the child object
*
* @returns L7_SUCCESS  if there is a next valid parent/child pair
* @returns L7_FAILURE  otherwise
*
* @notes Performs lookup by finding the current physical index, and incrementing through 
*        it's children until it gets a child greater than the child index, or the first
*        child if it has to look on annother physical object.
* 
* @end
*
*********************************************************************/
L7_RC_t edbPhysicalContainsEntryNextGet(L7_uint32 *physicalIndex, L7_uint32 *childIndex)
{
  L7_uint32 maxIndex;
  L7_uint32 parentIndex = *physicalIndex;
  L7_EDB_OBJECT_t *object_p;
  L7_EDB_OBJECT_t *child_p;

  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  /* find the last object number to look through */
  if (edbTreeMaxIndex(&maxIndex) == L7_SUCCESS)
  {
    if (parentIndex <= 0)
    {
      /* set the indicies to the first possible index */
      parentIndex = 1;
      *childIndex = 0;
    }

    /* iterate through the parents */
    while (parentIndex <= maxIndex)
    {
      /* pull the parent object for the given index */
      if (edbObjectFind(parentIndex, &object_p) == L7_SUCCESS)
      {
        /* iterate through the children to find the next child (if any) */
        for (child_p = object_p->child; child_p != L7_NULLPTR; child_p = child_p->nextSibling)
        {
          /* if we had to go to a next parent, or if the child index is greater */
          if ((parentIndex > *physicalIndex) ||
              (parentIndex == *physicalIndex && child_p->physicalIndex > *childIndex))
          {
            *physicalIndex = parentIndex;
            *childIndex = child_p->physicalIndex;

            (void)osapiSemaGive(edbSemaId);

            return L7_SUCCESS;
          }
        }
      }
      else
      {
        break;
      }

      /* go on to the next parent */
      parentIndex += 1;
      *childIndex = 0;
    }
  }

  (void)osapiSemaGive(edbSemaId);

  return L7_FAILURE;
}

/***************************************************************************************/
/***** entGeneralGroup *****************************************************************/
/***************************************************************************************/

/*********************************************************************
*
* @purpose Returns the sysUpTime at the time of the last table change
*
* @param lastChangeTime @b((output))  pointer to receive sysUpTime value
*
* @returns L7_SUCCESS  if the value exists
* @returns L7_FAILURE  otherwise
*
* @end
*
*********************************************************************/
L7_RC_t edbLastChangeTimeGet(L7_uint32 *lastChangeTime)
{
  return edbConfigChangeTime(lastChangeTime);
}

/***************************************************************************************/
/***** cnfgr Support Functions *********************************************************/
/***************************************************************************************/

/*********************************************************************
* @purpose Initialize edb for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.  If this fails, it is due to an inability to 
*         to acquire resources.
*
* @end
*********************************************************************/
L7_RC_t edbPhaseOneInit()
{
  L7_RC_t rc = L7_SUCCESS;

  rc = edbInitialize();

  if (rc == L7_SUCCESS)
  {
    L7_edb_task_id = osapiTaskCreate( "EDB", 
                                      edbTask, 0, 0, 
                                      edbSidTaskStackSizeGet(),
                                      edbSidTaskPriorityGet(),
                                      edbSidTaskSliceGet());

    if (L7_edb_task_id == L7_ERROR)
      LOG_MSG("Failed to Create EDB Task.\n");

    L7_edbTrap_task_id = osapiTaskCreate( "EDB Trap", 
                                          edbTrapTask, 0, 0, 
                                          edbSidTaskStackSizeGet(),
                                          edbSidTaskPriorityGet(),
                                          edbSidTaskSliceGet());

    if (L7_edbTrap_task_id == L7_ERROR)
      LOG_MSG("Failed to Create EDB Trap Task.\n");
  }

  return(rc);
}

/*********************************************************************
* @purpose Initialize edb for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t edbPhaseTwoInit()
{
  L7_RC_t rc = L7_SUCCESS;

  rc = edbRegister();

  if (rc == L7_SUCCESS)
    rc = edbCreateDefaults();

  return rc;
}

/*********************************************************************
* @purpose Initialize edb for Phase 3
*
* @param   void
*
* @returns L7_SUCCESS  Phase 3 completed
* @returns L7_FAILURE  Phase 3 incomplete
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t edbPhaseThreeInit()
{
  L7_RC_t rc = L7_SUCCESS;

  /*rc = edbCreateDefaults();*/

  return(rc);
}

/*********************************************************************
* @purpose  Release all resources collected during phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void edbPhaseOneFini()
{
  edbUninitialize();
}

/* End Function Definitions */

void edbWalk()
{
  L7_uint32 nominator = 0;
  L7_uint32 physicalIndex = 0;
  L7_uint32 childIndex = 0;
  L7_char8 buffer[256];
  L7_uint32 val = 0;
  L7_BOOL value = L7_FALSE;
  if (edbLastChangeTimeGet(&val) == L7_SUCCESS)
  {
    sysapiPrintf("entLastChangeTime.0 = %d\n", val);
  }
  else
  {
    /*sysapiPrintf("entLastChangeTime.0 = no such instance\n");*/
  }

  for (nominator = 0; nominator < 19; nominator += 1)
  {
    physicalIndex = 0;
    while (edbPhysicalEntryNextGet(&physicalIndex) == L7_SUCCESS)
    {
      switch (nominator)
      {
      case 0:
        sysapiPrintf("entPhysicalIndex.%d = %d\n", physicalIndex, physicalIndex);
        break;
  
      case 1:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalDescrGet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalDescr.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalDescr.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 2:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalVendorTypeGet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalVendorType.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalVendorType.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 3:
        if (edbPhysicalContainedInGet(physicalIndex, &val) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalContainedIn.%d = %d\n", physicalIndex, val);
        }
        else
        {
          /*sysapiPrintf("entPhysicalContainedIn.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 4:
        if (edbPhysicalClassGet(physicalIndex, &val) == L7_SUCCESS)
        {
          switch (val)
          {
          case L7_EDB_PHYSICAL_CLASS_OTHER:
            sysapiPrintf("entPhysicalClass.%d = %d (other)\n", physicalIndex, val);
            break;
      
          case L7_EDB_PHYSICAL_CLASS_UNKNOWN:
            sysapiPrintf("entPhysicalClass.%d = %d (unknown)\n", physicalIndex, val);
            break;
      
          case L7_EDB_PHYSICAL_CLASS_CHASSIS:
            sysapiPrintf("entPhysicalClass.%d = %d (chassis)\n", physicalIndex, val);
            break;
      
          case L7_EDB_PHYSICAL_CLASS_BACKPLANE:
            sysapiPrintf("entPhysicalClass.%d = %d (backplane)\n", physicalIndex, val);
            break;
      
          case L7_EDB_PHYSICAL_CLASS_CONTAINER:
            sysapiPrintf("entPhysicalClass.%d = %d (container)\n", physicalIndex, val);
            break;
      
          case L7_EDB_PHYSICAL_CLASS_POWERSUPPLY:
            sysapiPrintf("entPhysicalClass.%d = %d (powerSupply)\n", physicalIndex, val);
            break;
      
          case L7_EDB_PHYSICAL_CLASS_FAN:
            sysapiPrintf("entPhysicalClass.%d = %d (fan)\n", physicalIndex, val);
            break;
      
          case L7_EDB_PHYSICAL_CLASS_SENSOR:
            sysapiPrintf("entPhysicalClass.%d = %d (sensor)\n", physicalIndex, val);
            break;
      
          case L7_EDB_PHYSICAL_CLASS_MODULE:
            sysapiPrintf("entPhysicalClass.%d = %d (module)\n", physicalIndex, val);
            break;
      
          case L7_EDB_PHYSICAL_CLASS_PORT:
            sysapiPrintf("entPhysicalClass.%d = %d (port)\n", physicalIndex, val);
            break;
      
          case L7_EDB_PHYSICAL_CLASS_STACK:
            sysapiPrintf("entPhysicalClass.%d = %d (stack)\n", physicalIndex, val);
            break;
          default:
            sysapiPrintf("entPhysicalClass.%d = %d (?)\n", physicalIndex, val);            
            break;
          }
        }
        else
        {
          /*sysapiPrintf("entPhysicalClass.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 5:
        if (edbPhysicalParentRelPosGet(physicalIndex, &val) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalParentRelPos.%d = %d\n", physicalIndex, val);
        }
        else
        {
          /*sysapiPrintf("entPhysicalParentRelPos.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 6:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalNameGet(physicalIndex, buffer) == L7_SUCCESS)
        {
         sysapiPrintf("entPhysicalName.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalName.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 7:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalHardwareRevGet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalHardwareRev.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalHardwareRev.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 8:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalFirmwareRevGet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalFirmwareRev.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalFirmwareRev.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 9:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalSoftwareRevGet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalSoftwareRev.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalSoftwareRev.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 10:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalSerialNumGet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalSerialNum.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalSerialNum.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 11:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalSerialNumSet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalSerialNum.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalSerialNum.%d = no such instance\n", physicalIndex);*/
        }
        break;

      case 12:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalMfgNameGet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalMfgName.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalMfgName.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 13:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalModelNameGet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalModelName.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalModelName.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 14:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalAliasGet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalAlias.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalAlias.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 15:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalAliasSet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalAlias.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalAlias.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 16:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalAssetIDGet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalAssetID.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalAssetID.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 17:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalAssetIDSet(physicalIndex, buffer) == L7_SUCCESS)
        {
          sysapiPrintf("entPhysicalAssetID.%d = \"%s\"\n", physicalIndex, buffer);
        }
        else
        {
          /*sysapiPrintf("entPhysicalAssetID.%d = no such instance\n", physicalIndex);*/
        }
        break;
  
      case 18:
        bzero(buffer, sizeof(buffer));
        if (edbPhysicalIsFRUGet(physicalIndex, &value) == L7_SUCCESS)
        {
          switch (value)
          {
          case L7_TRUE:
            sysapiPrintf("entPhysicalIsFRU.%d = 1 (true)\n", physicalIndex);
            break;

          case L7_FALSE:
            sysapiPrintf("entPhysicalIsFRU.%d = 2 (false)\n", physicalIndex);
            break;

          default:
            sysapiPrintf("entPhysicalIsFRU.%d = %d (?)\n", physicalIndex, value);
            break;
          }
        }
        else
        {
          /*sysapiPrintf("entPhysicalIsFRU.%d = no such instance\n", physicalIndex);*/
        }
        break;

      default:
        break;
      }
    }
  }

  physicalIndex = 0;
  childIndex = 0;
  while (edbPhysicalContainsEntryNextGet(&physicalIndex, &childIndex) == L7_SUCCESS)
  {
    sysapiPrintf("entPhysicalIndex.%d.%d = %d\n", physicalIndex, childIndex, physicalIndex);
  }
  physicalIndex = 0;
  childIndex = 0;
  while (edbPhysicalContainsEntryNextGet(&physicalIndex, &childIndex) == L7_SUCCESS)
  {
    sysapiPrintf("entChildIndex.%d.%d = %d\n", physicalIndex, childIndex, childIndex);
  }

  return;
}

