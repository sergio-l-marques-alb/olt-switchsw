
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename usmdb_edb.c
*
* @purpose USMDB Entity API implementation
*
* @component edb
*
* @comments none
*
* @create 07/08/2003
*
* @author cpverne
*
* @end
*             
**********************************************************************/

#ifndef USMDB_EDB_API_H
#define USMDB_EDB_API_H

#include "l7_common.h"
#include "usmdb_edb_api.h"
#include "edb_api.h"

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
L7_RC_t usmDbEdbPhysicalEntryGet(L7_uint32 physicalIndex);

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
L7_RC_t usmDbEdbPhysicalEntryNextGet(L7_uint32 *physicalIndex);

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
L7_RC_t usmDbEdbPhysicalDescrGet(L7_uint32 physicalIndex, L7_char8 *physicalDescr);

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
L7_RC_t usmDbEdbPhysicalVendorTypeGet(L7_uint32 physicalIndex, L7_char8 *vendorType);

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
L7_RC_t usmDbEdbPhysicalContainedInGet(L7_uint32 physicalIndex, L7_uint32 *containedIn);

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
L7_RC_t usmDbEdbPhysicalClassGet(L7_uint32 physicalIndex, L7_uint32 *physicalClass);

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
L7_RC_t usmDbEdbPhysicalParentRelPosGet(L7_uint32 physicalIndex, L7_int32 *parentRelPos);

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
L7_RC_t usmDbEdbPhysicalNameGet(L7_uint32 physicalIndex, L7_char8 *name);

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
L7_RC_t usmDbEdbPhysicalHardwareRevGet(L7_uint32 physicalIndex, L7_char8 *hardwareRev);

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
L7_RC_t usmDbEdbPhysicalFirmwareRevGet(L7_uint32 physicalIndex, L7_char8 *firmwareRev);

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
L7_RC_t usmDbEdbPhysicalSoftwareRevGet(L7_uint32 physicalIndex, L7_char8 *softwareRev);

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
L7_RC_t usmDbEdbPhysicalSerialNumGet(L7_uint32 physicalIndex, L7_char8 *serialNum);

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
L7_RC_t usmDbEdbPhysicalSerialNumSet(L7_uint32 physicalIndex, L7_char8 *serialNum);

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
L7_RC_t usmDbEdbPhysicalMfgNameGet(L7_uint32 physicalIndex, L7_char8 *mfgName);

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
L7_RC_t usmDbEdbPhysicalModelNameGet(L7_uint32 physicalIndex, L7_char8 *modelName);

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
L7_RC_t usmDbEdbPhysicalAliasGet(L7_uint32 physicalIndex, L7_char8 *alias);

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
L7_RC_t usmDbEdbPhysicalAliasSet(L7_uint32 physicalIndex, L7_char8 *alias);

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
L7_RC_t usmDbEdbPhysicalAssetIDGet(L7_uint32 physicalIndex, L7_char8 *assetID);

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
L7_RC_t usmDbEdbPhysicalAssetIDSet(L7_uint32 physicalIndex, L7_char8 *assetID);

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
L7_RC_t usmDbEdbPhysicalIsFRUGet(L7_uint32 physicalIndex, L7_BOOL *isFRU);

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
L7_RC_t usmDbEdbPhysicalContainsEntryGet(L7_uint32 physicalIndex, L7_uint32 childIndex);

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
L7_RC_t usmDbEdbPhysicalContainsEntryNextGet(L7_uint32 *physicalIndex, L7_uint32 *childIndex);

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
L7_RC_t usmDbEdbLastChangeTimeGet(L7_uint32 *lastChangeTime);

#endif
