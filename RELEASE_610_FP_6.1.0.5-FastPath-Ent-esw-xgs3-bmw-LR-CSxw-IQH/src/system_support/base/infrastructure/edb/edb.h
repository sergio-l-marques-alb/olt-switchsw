/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   edb.h
*
* @purpose    System-Specific code for Entity MIB - RFC 2737
*
* @component  SNMP
*
* @comments
*
* @create     6/04/2003
*
* @author     cpverne
* @end
*
**********************************************************************/

#ifndef EDB_H
#define EDB_H

#include "l7_common.h"
#include "edb_api.h"
#include "sysapi_hpc.h"
#include "nimapi.h"

/* Default relative position of objects which are contained in other objects */
#define L7_EDB_PHYSICAL_RELPOS_DEFAULT 1

/* Relative position of objects which are not contained in other objects */
#define L7_EDB_PHYSICAL_RELPOS_NOPARENT 0

/* Semaphore object for database interaction */
extern void *edbSemaId;

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose Indicates the presence of a stack entity
*
* @returns L7_TRUE   if the build should contain a stack entity
* @returns L7_FALSE  otherwise
*
* @end
*
*********************************************************************/
L7_BOOL edbStackPresent();

/*********************************************************************
*
* @purpose Initialize the entity component.
*
* @returns L7_SUCCESS  if all initialization is done
* @returns L7_FAILURE  if something fails during initialization
*
* @end
*
*********************************************************************/
L7_RC_t edbInitialize();

/*********************************************************************
*
* @purpose Removes anything allocated during initialization
*
* @returns L7_SUCCESS  if all uninitialization is done
* @returns L7_FAILURE  if something fails during uninitialization
*
* @end
*
*********************************************************************/
void edbUninitialize();


/*********************************************************************
*
* @purpose Initialize the entity tree.
*
* @returns L7_SUCCESS  if all initialization is done
* @returns L7_FAILURE  if something fails during initialization
*
* @end
*
*********************************************************************/
L7_RC_t edbCreateDefaults();

/*********************************************************************
*
* @purpose Register for callbacks.
*
* @returns L7_SUCCESS  if all registration is done
* @returns L7_FAILURE  if something fails during registration
*
* @end
*
*********************************************************************/
L7_RC_t edbRegister();

/*********************************************************************
*
* @purpose Main task for performing Entity Table Management
*
* @end
*
*********************************************************************/
void edbTask();

/*********************************************************************
*
* @purpose Main task for performing Entity Config Change Traps
*
* @end
*
*********************************************************************/
void edbTrapTask();

/***************************************************************************************/
/***** Callback Functions **************************************************************/
/***************************************************************************************/

/*********************************************************************
*
* @purpose Signals creation of a new Unit object
*
* @param unit @b((input))  Unit index to create an object for
*
* @returns L7_SUCCESS  if the object is created
* @returns L7_FAILURE  if the object could not be created
* @returns L7_ERROR    if the object couldn't be inserted, and then couldn't be destroyed
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectUnitJoinCallback(L7_uint32 unit);

/*********************************************************************
*
* @purpose Signals deletion of a Unit object
*
* @param unit @b((input))  Unit index to delete
*
* @returns L7_SUCCESS  if the object is deleted
* @returns L7_FAILURE  if the object could not be deleted
*
* @notes  Removes all slots/cards/ports under this unit
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectUnitLeaveCallback(L7_uint32 unit);

/*********************************************************************
*
* @purpose Signals creation of a new Slot object
*
* @param unit @b((input))  Unit index the slot is on
* @param slot @b((input))  Slot index to create an object for
*
* @returns L7_SUCCESS  if the object is created
* @returns L7_FAILURE  if the object could not be created
* @returns L7_ERROR    if the object couldn't be inserted, and then couldn't be destroyed
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectSlotCreateCallback(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
*
* @purpose Signals deletion of a Slot object
*
* @param unit @b((input))  Unit index slot is on
* @param slot @b((input))  Slot index to delete
*
* @returns L7_SUCCESS  if the object is deleted
* @returns L7_FAILURE  if the object could not be found or removed
* @returns L7_ERROR    if the object could be removed, but not deleted
*
* @notes  Removes all cards/ports under this slot
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectSlotDeleteCallback(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
*
* @purpose Signals creation of a new Card object
*
* @param unit @b((input))  Unit index the slot is on
* @param slot @b((input))  Slot index to create a Card object under
* @param cardType @b((input))  Card type this object represents (SYSAPI_CARD_TYPE_t)
*
* @returns L7_SUCCESS  if the object is created
* @returns L7_FAILURE  if the object could not be created
* @returns L7_ERROR    if the object couldn't be inserted, and then couldn't be destroyed
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectCardPluginCallback(L7_uint32 unit, L7_uint32 slot, L7_uint32 cardType);

/*********************************************************************
*
* @purpose Signals deletion of a Card object
*
* @param unit @b((input))  Unit index slot is on
* @param slot @b((input))  Slot index card to delete is under
*
* @returns L7_SUCCESS  if the object is deleted
* @returns L7_FAILURE  if the object could not be found or removed
* @returns L7_ERROR    if the object could be removed, but not deleted
*
* @notes  Removes all ports under this slot
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectCardUnplugCallback(L7_uint32 unit, L7_uint32 slot);


/*********************************************************************
* @purpose  Determine if the interface is valid
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none  TODO- Move to edb_outcalls.c
*
* @end
*********************************************************************/
L7_BOOL edbIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Signals creation/deletion of a new Port object
*
* @param intIfNum @b((input))  internal interface number of the port
* @param event    @b((input))  Interface event type
*
* @returns L7_SUCCESS  if the object is created
* @returns L7_FAILURE  if the object could not be created
* @returns L7_ERROR    if the object couldn't be inserted, and then couldn't be destroyed
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectPortChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);

/***************************************************************************************/
/***** Callback Operators **************************************************************/
/***************************************************************************************/

/*********************************************************************
*
* @purpose Initates creation of a new Unit object
*
* @param unit @b((input))  Unit index to create an object for
*
* @returns L7_SUCCESS  if the object is created
* @returns L7_FAILURE  if the object could not be created
* @returns L7_ERROR    if the object couldn't be inserted, and then couldn't be destroyed
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectUnitJoin(L7_uint32 unit);

/*********************************************************************
*
* @purpose Initates deletion of a Unit object
*
* @param unit @b((input))  Unit index to delete
*
* @returns L7_SUCCESS  if the object is deleted
* @returns L7_FAILURE  if the object could not be deleted
*
* @notes  Removes all slots/cards/ports under this unit
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectUnitLeave(L7_uint32 unit);

/*********************************************************************
*
* @purpose Initates creation of a new Slot object
*
* @param unit @b((input))  Unit index the slot is on
* @param slot @b((input))  Slot index to create an object for
* @param slotType @b((input))  Slot type this object represents
*
* @returns L7_SUCCESS  if the object is created
* @returns L7_FAILURE  if the object could not be created
* @returns L7_ERROR    if the object couldn't be inserted, and then couldn't be destroyed
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectSlotCreate(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
*
* @purpose Initates deletion of a Slot object
*
* @param unit @b((input))  Unit index slot is on
* @param slot @b((input))  Slot index to delete
*
* @returns L7_SUCCESS  if the object is deleted
* @returns L7_FAILURE  if the object could not be found or removed
* @returns L7_ERROR    if the object could be removed, but not deleted
*
* @notes  Removes all cards/ports under this slot
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectSlotDelete(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
*
* @purpose Initates creation of a new Card object
*
* @param unit @b((input))  Unit index the slot is on
* @param slot @b((input))  Slot index to create a Card object under
*
* @returns L7_SUCCESS  if the object is created
* @returns L7_FAILURE  if the object could not be created
* @returns L7_ERROR    if the object couldn't be inserted, and then couldn't be destroyed
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectCardPlugin(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
*
* @purpose Initates deletion of a Card object
*
* @param unit @b((input))  Unit index slot is on
* @param slot @b((input))  Slot index card to delete is under
*
* @returns L7_SUCCESS  if the object is deleted
* @returns L7_FAILURE  if the object could not be found or removed
* @returns L7_ERROR    if the object could be removed, but not deleted
*
* @notes  Removes all ports under this slot
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectCardUnplug(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
*
* @purpose Initates creation of a new Port object
*
* @param unit @b((input))  Unit index the slot is on
* @param slot @b((input))  Slot index the port is on
* @param port @b((input))  Port index to create an object for
*
* @returns L7_SUCCESS  if the object is created
* @returns L7_FAILURE  if the object could not be created
* @returns L7_ERROR    if the object couldn't be inserted, and then couldn't be destroyed
*
* @notes  Detects if there is a card in the slot, and adds the port to that card
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectPortCreate(L7_uint32 unit, L7_uint32 slot, L7_uint32 port);

/*********************************************************************
*
* @purpose Initates deletion of a Port object
*
* @param unit @b((input))  Unit index the slot is on
* @param slot @b((input))  Slot index the port is on
* @param port @b((input))  Port index to create an object for
*
* @returns L7_SUCCESS  if the object is deleted
* @returns L7_FAILURE  if the object could not be deleted
* @returns L7_ERROR    if the object could be removed, but not destroyed
*
* @notes  Detects if there is a card under the slot, and removes the port from that card
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectPortDelete(L7_uint32 unit, L7_uint32 slot, L7_uint32 port);


/***************************************************************************************/
/***** Entity Object Functions *********************************************************/
/***************************************************************************************/

/*********************************************************************
*
* @purpose Creates a new Entity Object and initializes values
*
* @param **object_p @b((input))  pointer to receive L7_ENTITY_OBJECT_t pointer
*
* @returns L7_SUCCESS  if the object is created successfully
* @returns L7_FAILURE  if the object pointer provided is null
* @returns L7_ERROR    if memory could not be allocated for the new object
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectCreate(L7_EDB_OBJECT_t **object_p);

/*********************************************************************
*
* @purpose Frees the memory associated with this object
*
* @param *object_p @b((input))  pointer to object to delete
*
* @returns L7_SUCCESS  if the object is freed
* @returns L7_FAILURE  if the object pointer is null, or the object has siblings
* @returns L7_ERROR    if removing any child object fails
*
* @notes object should first be removed before destroying
* @notes does not set the pointer to NULL
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectDestroy(L7_EDB_OBJECT_t *object_p);

/*********************************************************************
*
* @purpose Checks if the object can be set as a child of the parent object
*
* @param *object_p @b((input))   pointer to object to be added
* @param *parent_p @b((output))  pointer to parent object will be added as a child of
*
* @returns L7_SUCCESS  if object is a correct match
* @returns L7_FAILURE  if object pointer is null, or parent pointer is null and there is an
*                      object in the tree, or if the child can't be added under the parent
*
* @notes Stack and Chassis objects can only be created under Stack objects.  All other
*        combinations are allowed.
* @notes If there are no objects in the tree, then any object can be created as the first
*        object.
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectParentChildMatch(L7_EDB_OBJECT_t *object_p, L7_EDB_OBJECT_t *parent_p);

/*********************************************************************
*
* @purpose Inserts an object into the tree under the parent object
*
* @param *object_p @b((input))  pointer to object to be inserted
* @param *parent_p @b((input))  pointer to object to insert under
*
* @returns L7_SUCCESS  if the object is inserted
* @returns L7_FAILURE  if the object pointer is null, or the parent pointer is null
*                      and there are objects in the tree.
*
* @notes If there are no objects in the tree, then by specifing L7_NULLPTR as the
*        parent will add the object as the first object in the tree.
* @notes It will check that there aren't any direct parent loops with the new object.
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectChildInsert(L7_EDB_OBJECT_t *object_p, L7_EDB_OBJECT_t *parent_p);

/*********************************************************************
*
* @purpose Removes an object from the tree
*
* @param *object_p @b((input))  Object to remove from the tree
*
* @returns L7_SUCCESS  if the object is removed
* @returns L7_FAILURE  if the object pointer is null, or object has child objects
*
* @notes Sets child, parent, and sibling pointers to L7_NULLPTR.
* @notes If the object is the first object in the tree, sets edbTree_g to L7_NULLPTR.
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectRemove(L7_EDB_OBJECT_t *object_p);

/*********************************************************************
*
* @purpose Initiates reindexing the entire tree.
*
* @returns L7_SUCCESS if the reindexing completes sucessfully
* @returns L7_FAILURE if something prevents it from cmpleting
*
* @end
*
*********************************************************************/
L7_RC_t edbTreeReindex();

/*********************************************************************
*
* @purpose Initiates reindexing starting at an object
*
* @param *object_p @b((input)) pointer to object to start indexing
* @param *index @b((input))    pointer to index to start indexing from
*
* @returns L7_SUCCESS if indexing from this object succeeds
* @returns L7_FAILURE if object pointer is null, or if indexing child objects fails
*
* @notes Sets index to next sibling (or parent's sibling) index suitable for recursive indexing
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectReindex(L7_EDB_OBJECT_t *object_p, L7_uint32 *index);

/*********************************************************************
*
* @purpose Returns the last physicalIndex value after reindexing.
*
* @param *maxIndex @b((input))    variable to receive last physicalIndex value
*
* @returns L7_SUCCESS if the value is returned sucessfully
* @returns L7_FAILURE if the variable pointer points to L7_NULLPTR
*
* @end
*
*********************************************************************/
L7_RC_t edbTreeMaxIndex(L7_uint32 *maxIndex);


/*********************************************************************
*
* @purpose Searches tree for an object with the given physicalIndex
*
* @param physicalIndex @b((input))  physicalIndex of object to find
* @param **object_p @b((output))    pointer to receive object pointer
*
* @returns L7_SUCCESS  if object is found
* @returns L7_FAILURE  if objectpointer is null, or index out of bounds, or not found
*
* @comments An AVL tree is not necessary as the tree maintains it's indexing, this allows
*           searches to find the right path down the tree and prevent O(n) searches.
* @end
*
*********************************************************************/
L7_RC_t edbObjectFind(L7_int32 physicalIndex, L7_EDB_OBJECT_t **object_p);


/*********************************************************************
*
* @purpose Searches the stack for a Unit object with the given Index
*
* @param unitIndex @b((input))    Unit Index of object to find
* @param *stack_p @b((input))     object pointer of the stack object
* @param **object_p @b((output))  pointer to receive object pointer
*
* @returns L7_SUCCESS  if object is found
* @returns L7_FAILURE  if objectpointer is null, or stack pointer points to an object
*                      which is not a stack object, or the unit is not found
*
* @notes A unit is defined as a child of a stack object
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectUnitFind(L7_uint32 unitIndex, L7_EDB_OBJECT_t *stack_p, L7_EDB_OBJECT_t **object_p);


/*********************************************************************
*
* @purpose Searches the unit for a Slot object with the given Index
*
* @param slotIndex @b((input))    Slot Index of object to find
* @param *unit_p @b((input))      object pointer of the unit object
* @param **object_p @b((output))  pointer to receive object pointer
*
* @returns L7_SUCCESS  if object is found
* @returns L7_FAILURE  if objectpointer is null, or unit pointer points to an object
*                      which is not a unit object, or the slot is not found
*
* @notes A slot is defined as a child of a unit object
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectSlotFind(L7_uint32 slotIndex, L7_EDB_OBJECT_t *unit_p, L7_EDB_OBJECT_t **object_p);

/*********************************************************************
*
* @purpose Searches the slot for a Port object with the given Index
*
* @param portIndex @b((input))    Port Index of object to find
* @param *slot_p @b((input))      object pointer of the slot object
* @param **object_p @b((output))  pointer to receive object pointer
*
* @returns L7_SUCCESS  if object is found
* @returns L7_FAILURE  if objectpointer is null, or slot pointer points to an object
*                      which is not a slot object, or the port is not found
*
* @notes A port is defined as a child of a slot object or slot & card object pair
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectPortFind(L7_uint32 portIndex, L7_EDB_OBJECT_t *slot_p, L7_EDB_OBJECT_t **object_p);

/*********************************************************************
*
* @purpose Returns the last sysUpTime of when there was a change
*
* @param *maxIndex @b((input))    variable to receive value of edbConfigChangeTime_g
*
* @returns L7_SUCCESS
*
* @end
*
*********************************************************************/
L7_RC_t edbConfigChangeTime(L7_uint32 *configChangeTime);

/* End Function Prototypes */

#endif /* EDB_H */
