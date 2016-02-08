/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   edb_api.h
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

#ifndef EDB_API_H
#define EDB_API_H

#include "l7_common.h"
#include "edb_exports.h"
#include "nimapi.h"
#include "edb_exports.h"

typedef struct
{
  L7_EDB_MESSAGE_TYPE_t messageType;
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 intIfNum;
  L7_PORT_EVENTS_t port_event;
  NIM_CORRELATOR_t correlator;
} L7_EDB_MESSAGE_t;

typedef struct L7_EDB_OBJECT_s
{
  /* index of this object in the entPhysicalTable */
  L7_int32 physicalIndex;

  /* pointer to object that this object is contained in */
  struct L7_EDB_OBJECT_s *parent;

  /* pointer to first object that this object contains */
  struct L7_EDB_OBJECT_s *child;

  /* pointer to the next logical sibling object */
  struct L7_EDB_OBJECT_s *nextSibling;

  /* pointer to the previous logical sibling object */
  struct L7_EDB_OBJECT_s *prevSibling;

  /* Physical class of this object */
  L7_EDB_PHYSICAL_CLASS_t objectClass;

  /* Type of device this object represents */
  L7_EDB_OBJECT_TYPE_t objectType;

  /* unit object is on if it's a slot, card or port */
  L7_uint32 unit;

  /* slot object is on if it's a card or port */
  L7_uint32 slot;

  /* Relative position of this 'child' object to it's 'sibling' object */
  /* Corresponds with Unit/Slot/Port of this object if Object Type is unit, slot, or port */
  L7_int32 relPos;

} L7_EDB_OBJECT_t;
                                
/* Begin Function Prototypes */

void L7_entityTask();

void L7_entityTrapTask();

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
L7_RC_t edbPhysicalEntryGet(L7_uint32 physicalIndex);

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
L7_RC_t edbPhysicalEntryNextGet(L7_uint32 *physicalIndex);

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
L7_RC_t edbPhysicalDescrGet(L7_uint32 physicalIndex, L7_char8 *physicalDescr);

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
L7_RC_t edbPhysicalVendorTypeGet(L7_uint32 physicalIndex, L7_char8 *vendorType);

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
L7_RC_t edbPhysicalContainedInGet(L7_uint32 physicalIndex, L7_uint32 *containedIn);

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
L7_RC_t edbPhysicalClassGet(L7_uint32 physicalIndex, L7_uint32 *physicalClass);

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
L7_RC_t edbPhysicalParentRelPosGet(L7_uint32 physicalIndex, L7_int32 *parentRelPos);

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
L7_RC_t edbPhysicalNameGet(L7_uint32 physicalIndex, L7_char8 *name);

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
L7_RC_t edbPhysicalHardwareRevGet(L7_uint32 physicalIndex, L7_char8 *hardwareRev);

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
L7_RC_t edbPhysicalFirmwareRevGet(L7_uint32 physicalIndex, L7_char8 *firmwareRev);

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
L7_RC_t edbPhysicalSoftwareRevGet(L7_uint32 physicalIndex, L7_char8 *softwareRev);

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
L7_RC_t edbPhysicalSerialNumGet(L7_uint32 physicalIndex, L7_char8 *serialNum);

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
L7_RC_t edbPhysicalSerialNumSet(L7_uint32 physicalIndex, L7_char8 *serialNum);

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
L7_RC_t edbPhysicalMfgNameGet(L7_uint32 physicalIndex, L7_char8 *mfgName);

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
L7_RC_t edbPhysicalModelNameGet(L7_uint32 physicalIndex, L7_char8 *modelName);

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
L7_RC_t edbPhysicalAliasGet(L7_uint32 physicalIndex, L7_char8 *alias);

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
L7_RC_t edbPhysicalAliasSet(L7_uint32 physicalIndex, L7_char8 *alias);

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
L7_RC_t edbPhysicalAssetIDGet(L7_uint32 physicalIndex, L7_char8 *assetID);

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
L7_RC_t edbPhysicalAssetIDSet(L7_uint32 physicalIndex, L7_char8 *assetID);

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
L7_RC_t edbPhysicalIsFRUGet(L7_uint32 physicalIndex, L7_BOOL *isFRU);

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
L7_RC_t edbPhysicalContainsEntryGet(L7_uint32 physicalIndex, L7_uint32 childIndex);

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
L7_RC_t edbPhysicalContainsEntryNextGet(L7_uint32 *physicalIndex, L7_uint32 *childIndex);

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
L7_RC_t edbLastChangeTimeGet(L7_uint32 *lastChangeTime);

/***************************************************************************************/
/***** cnfgr Support Functions *********************************************************/
/***************************************************************************************/

/*********************************************************************
* @purpose Initialize entity for Phase 1
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
L7_RC_t edbPhaseOneInit();

/*********************************************************************
* @purpose Initialize entity for Phase 2
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
L7_RC_t edbPhaseTwoInit();

/*********************************************************************
* @purpose Initialize entity for Phase 3
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
L7_RC_t edbPhaseThreeInit();

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
void edbPhaseOneFini(void);

/* End Function Prototypes */

#endif /* EDB_API_H */

