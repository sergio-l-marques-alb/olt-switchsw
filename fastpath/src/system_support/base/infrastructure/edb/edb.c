/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   edb.c
*
* @purpose    Component code for Entity MIB - RFC 2737
*
* @component  SNMP
*
* @comments   Handles maintaining the database of units/slots/cards/ports for
*             the entityPhysicalTable.
*
* @create     6/04/2003
*
* @author     cpverne
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_snmp_trap_api_stacking.h"

#include "edb.h"
#include "unitmgr_api.h"
#include "cardmgr_api.h"
#include "log.h"
#include "edb_sid.h"
#include "usmdb_snmp_trap_api.h"
#include "nimapi.h"
#include "trapapi.h"

/* Global stack pointer */
L7_EDB_OBJECT_t *edbTree_g = L7_NULLPTR;
void *edbSemaId = L7_NULLPTR;
void *edbMessageQueue = L7_NULLPTR;
L7_uint32 edbTreeCount_g = 0;

/* Bit mask of NIM events that EDB is registered to receive */
PORTEVENT_MASK_t edbNimEventMask;

/* indicates an entConfigChange trap should be sent */
L7_BOOL edbConfigChange = L7_FALSE;
/* sysUpTime of last config change event */
L7_uint32 edbConfigChangeTime_g = 0;

/* Begin Function Definitions: edb.h */

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
L7_BOOL edbStackPresent()
{
  L7_BOOL rc;

  rc = cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                             L7_STACKING_FEATURE_SUPPORTED);

  return rc;
}

/*********************************************************************
*
* @purpose Initialize the edb component.
*
* @returns L7_SUCCESS  if all initialization is done
* @returns L7_FAILURE  if something fails during initialization
*
* @end
*
*********************************************************************/
L7_RC_t edbInitialize(void)
{
  edbSemaId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if (edbSemaId == NULL)
    return L7_FAILURE;

  edbMessageQueue = osapiMsgQueueCreate("edb queue", edbSidMsgCountGet(), sizeof(L7_EDB_MESSAGE_t));

  if (edbMessageQueue == L7_NULLPTR)
    return L7_FAILURE;

  return L7_SUCCESS;
}

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
void edbUninitialize(void)
{
  if (edbSemaId != L7_NULLPTR)
  {
    if (osapiSemaDelete(edbSemaId) == L7_SUCCESS)
      edbSemaId = L7_NULLPTR;
  }

  if (edbMessageQueue != L7_NULLPTR)
  {
    if (osapiMsgQueueDelete(edbMessageQueue) == L7_SUCCESS)
      edbMessageQueue = L7_NULLPTR;
  }
}

/*********************************************************************
*
* @purpose Initialize the edb tree.
*
* @returns L7_SUCCESS  if all initialization is done
* @returns L7_FAILURE  if something fails during initialization
*
* @end
*
*********************************************************************/
L7_RC_t edbCreateDefaults(void)
{
  L7_EDB_OBJECT_t *stack_p;
  L7_uint32 temp_unit;

  /* only create the stack component if this is a stacking build */
  if (edbStackPresent() == L7_TRUE)
  {
    if (edbObjectCreate(&stack_p) == L7_SUCCESS)
    {
      stack_p->objectType = L7_EDB_OBJECT_TYPE_STACK;
      stack_p->objectClass = L7_EDB_PHYSICAL_CLASS_STACK;

      if (edbObjectChildInsert(stack_p, L7_NULLPTR) != L7_SUCCESS)
      {
        /* remove the stack */
        (void)edbObjectDestroy(stack_p);
        return L7_FAILURE;
      }

      /* reindex the table */
      return edbTreeReindex();
    }

    return L7_FAILURE;
  }
  else
  {
    /* add the current unit to the stack */
    if (unitMgrNumberGet(&temp_unit) == L7_SUCCESS)
      edbObjectUnitJoinCallback(temp_unit);
  }

  return L7_SUCCESS;
}

void edbObjectUnitChangeCallback(L7_uint32 unit, unitMgrUnitEvents_t event)
{
  switch (event)
  {
    case UNITMGR_UNIT_EVENT_JOIN:
      edbObjectUnitJoinCallback(unit);
      break;

    case UNITMGR_UNIT_EVENT_LEAVE:
      edbObjectUnitLeaveCallback(unit);
      break;

    default:
      break;
  }
}

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
L7_RC_t edbRegister(void)
{
  /* register to unit manager for unit join/leave notifications if in a stacking build */
  if (L7_SUCCESS != unitMgrUnitChangeRegister(L7_EDB_COMPONENT_ID, edbObjectUnitChangeCallback))
  {
    return L7_FAILURE;
  }

  /* register to cardmanager for card insertion/removal notifications */
  if (cmgrRegisterCardPlugin(L7_EDB_COMPONENT_ID, edbObjectCardPluginCallback) != L7_SUCCESS)
    return L7_FAILURE;

  if (cmgrRegisterCardUnplug(L7_EDB_COMPONENT_ID, edbObjectCardUnplugCallback) != L7_SUCCESS)
    return L7_FAILURE;

  /* register to NIM for port creation/deletion notifications */
  if (nimRegisterIntfChange(L7_EDB_COMPONENT_ID, edbObjectPortChangeCallback,
                            edbNimStartupCallback, NIM_STARTUP_PRIO_DEFAULT) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Main task for performing Edb Table Management
*
* @end
*
*********************************************************************/
void edbTask(void)
{
  L7_RC_t rc;
  L7_EDB_MESSAGE_t temp_msg;
  NIM_EVENT_COMPLETE_INFO_t temp_response;

  while (osapiMessageReceive(edbMessageQueue, (void*)&temp_msg, sizeof(L7_EDB_MESSAGE_t), L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    /*sysapiPrintf("EDB Task: TAKE - Type=%d, Unit=%d, Slot=%d, Port=%d", temp_msg.messageType, temp_msg.unit, temp_msg.slot, temp_msg.port);*/
    (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);
    /*sysapiPrintf(" [done]\n");*/

    rc = L7_FAILURE;
    switch (temp_msg.messageType)
    {
      case L7_EDB_MESSAGE_TYPE_UNIT_JOIN:
        rc = edbObjectUnitJoin(temp_msg.unit);
        break;
      case L7_EDB_MESSAGE_TYPE_UNIT_LEAVE:
        rc = edbObjectUnitLeave(temp_msg.unit);
        break;
      case L7_EDB_MESSAGE_TYPE_SLOT_CREATE:
        rc = edbObjectSlotCreate(temp_msg.unit, temp_msg.slot);
        break;
      case L7_EDB_MESSAGE_TYPE_SLOT_DELETE:
        rc = edbObjectSlotDelete(temp_msg.unit, temp_msg.slot);
        break;
      case L7_EDB_MESSAGE_TYPE_CARD_PLUGIN:
        rc = edbObjectCardPlugin(temp_msg.unit, temp_msg.slot);
        break;
      case L7_EDB_MESSAGE_TYPE_CARD_UNPLUG:
        rc = edbObjectCardUnplug(temp_msg.unit, temp_msg.slot);
        break;
      case L7_EDB_MESSAGE_TYPE_PORT_CREATE:
        rc = edbObjectPortCreate(temp_msg.unit, temp_msg.slot, temp_msg.port);
        temp_response.component = L7_EDB_COMPONENT_ID;
        temp_response.correlator = temp_msg.correlator;
        temp_response.event = temp_msg.port_event;
        temp_response.intIfNum = temp_msg.intIfNum;
        temp_response.response.rc = rc;
        if (rc != L7_ERROR)
        {
          /* hide failures from nim, only return L7_ERROR */
          temp_response.response.rc = L7_SUCCESS;
        }
        temp_response.response.reason = NIM_ERR_RC_UNUSED;
        nimEventStatusCallback(temp_response);
        break;
      case L7_EDB_MESSAGE_TYPE_PORT_DELETE:
        rc = edbObjectPortDelete(temp_msg.unit, temp_msg.slot, temp_msg.port);
        temp_response.component = L7_EDB_COMPONENT_ID;
        temp_response.correlator = temp_msg.correlator;
        temp_response.event = temp_msg.port_event;
        temp_response.intIfNum = temp_msg.intIfNum;
        temp_response.response.rc = rc;
        if (rc != L7_ERROR)
        {
          /* hide failures from nim, only return L7_ERROR */
          temp_response.response.rc = L7_SUCCESS;
        }
        temp_response.response.reason = NIM_ERR_RC_UNUSED;
        nimEventStatusCallback(temp_response);
        break;

        case L7_EDB_MESSAGE_ACTIVATE_STARTUP:
          edbNimActivateStartup();
          break;
      default:
        /* unknown message */
        break;
    }

    if (rc == L7_SUCCESS)
    {
      /* update the changed time */
      edbConfigChangeTime_g = osapiUpTimeRaw();
      edbConfigChange = L7_TRUE;
/*      sysapiPrintf("EDB Success: Type=%d, Unit=%d, Slot=%d, Port=%d, rc=%d\n", temp_msg.messageType, temp_msg.unit, temp_msg.slot, temp_msg.port, rc);*/
    }
    else if (rc == L7_ERROR)
    {
      sysapiPrintf("EDB Error: Type=%d, Unit=%d, Slot=%d, Port=%d, rc=L7_ERROR\n", temp_msg.messageType, temp_msg.unit, temp_msg.slot, temp_msg.port);
    }

    /*sysapiPrintf("EDB Task: GIVE - rc=%d", rc);*/
    (void)osapiSemaGive(edbSemaId);
    /*sysapiPrintf(" [done]\n");*/
  }
  return;
}

/*********************************************************************
*
* @purpose Main task for performing Edb Config Change Traps
*
* @end
*
*********************************************************************/
void edbTrapTask(void)
{
  while (1)
  {
    /* sleep for one second */
    osapiSleep(1);

    (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

    /* if a trap is pending, and not in a throttle period */
    if (edbConfigChange == L7_TRUE)
    {
      if (osapiUpTimeRaw() > (edbConfigChangeTime_g + edbSidTrapThrottleTimeGet()))
      {
        /* TODO: change call to go to trap manager */
        (void)trapMgrEntConfigChangeLogTrap();

        edbConfigChange = L7_FALSE;
      }
    }

    (void)osapiSemaGive(edbSemaId);
  }

  return;
}

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
L7_RC_t edbObjectUnitJoinCallback(L7_uint32 unit)
{
  L7_EDB_MESSAGE_t temp_message;
  L7_uint32 unit_type = 0;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_descriptor = L7_NULLPTR;
#if 0
  L7_uint32 slot_index = 0;
#endif

  temp_message.messageType = L7_EDB_MESSAGE_TYPE_UNIT_JOIN;
  temp_message.unit = unit;
  temp_message.slot = 0;
  temp_message.port = 0;
  temp_message.correlator = 0;

  if (unitMgrUnitTypeGet(unit, &unit_type) == L7_SUCCESS)
  {
    unit_descriptor = sysapiHpcUnitDbEntryGet(unit_type);

    if (unit_descriptor != L7_NULLPTR)
    {
	  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNMP_COMPONENT_ID, 
              "EDB Callback: Unit Join: %d. "
                                              "A new unit has joined the stack.\n", unit);

      /* send the unit creation message */
      if (osapiMessageSend(edbMessageQueue, (void*)&temp_message, sizeof(temp_message),
                           L7_NO_WAIT, L7_MSG_PRIORITY_NORM ) == L7_SUCCESS)
      {
        /* for each physical slot, create slot objects for it */
#if 0
        for (slot_index = 0; slot_index < unit_descriptor->numPhysSlots; slot_index+= 1)
        {
          temp_message.slot = slot_index;

          if (edbObjectSlotCreateCallback(unit, slot_index) != L7_SUCCESS)
          {
            return L7_FAILURE;
          }
        }
#endif
        return L7_SUCCESS;
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNMP_COMPONENT_ID,
                "Failed to send unit %d join message to Edb\n",
                unit);

      }
    }
  }

  return L7_FAILURE;
}

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
L7_RC_t edbObjectUnitLeaveCallback(L7_uint32 unit)
{
  L7_RC_t          rc;
  L7_EDB_MESSAGE_t temp_message;

  temp_message.messageType = L7_EDB_MESSAGE_TYPE_UNIT_LEAVE;
  temp_message.unit = unit;
  temp_message.slot = 0;
  temp_message.port = 0;
  temp_message.correlator = 0;

  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNMP_COMPONENT_ID, 
          "EDB Callback: Unit %d has left the stack.\n", unit);

  rc = osapiMessageSend(edbMessageQueue, (void*)&temp_message, sizeof(temp_message),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNMP_COMPONENT_ID,
            "Failed to send unit %d leave message to Edb\n",
            unit);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Signals creation of a new Slot object
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
L7_RC_t edbObjectSlotCreateCallback(L7_uint32 unit, L7_uint32 slot)
{
  L7_EDB_MESSAGE_t temp_message;
  L7_RC_t rc;

  temp_message.messageType = L7_EDB_MESSAGE_TYPE_SLOT_CREATE;
  temp_message.unit = unit;
  temp_message.slot = slot;
  temp_message.port = 0;
  temp_message.correlator = 0;

  rc = osapiMessageSend(edbMessageQueue, (void*)&temp_message, sizeof(temp_message), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM );

  return rc;
}

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
L7_RC_t edbObjectSlotDeleteCallback(L7_uint32 unit, L7_uint32 slot)
{
  L7_EDB_MESSAGE_t temp_message;

  temp_message.messageType = L7_EDB_MESSAGE_TYPE_SLOT_DELETE;
  temp_message.unit = unit;
  temp_message.slot = slot;
  temp_message.port = 0;
  temp_message.correlator = 0;

  return osapiMessageSend(edbMessageQueue, (void*)&temp_message, sizeof(temp_message), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM );
}

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
L7_RC_t edbObjectCardPluginCallback(L7_uint32 unit, L7_uint32 slot, L7_uint32 cardType)
{
/*  L7_EDB_MESSAGE_t temp_message;

  temp_message.messageType = L7_EDB_MESSAGE_TYPE_CARD_PLUGIN;
  temp_message.unit = unit;
  temp_message.slot = slot;
  temp_message.port = 0;

  return osapiMessageSend(edbMessageQueue, (void*)&temp_message, sizeof(temp_message), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM );*/
  return edbObjectSlotCreateCallback(unit,slot);
}

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
L7_RC_t edbObjectCardUnplugCallback(L7_uint32 unit, L7_uint32 slot)
{
/*  L7_EDB_MESSAGE_t temp_message;

  temp_message.messageType = L7_EDB_MESSAGE_TYPE_CARD_UNPLUG;
  temp_message.unit = unit;
  temp_message.slot = slot;

  return osapiMessageSend(edbMessageQueue, (void*)&temp_message, sizeof(temp_message), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM );*/
  return edbObjectSlotDeleteCallback(unit,slot);
}

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
L7_BOOL edbIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    switch (sysIntfType)
    {
      case L7_PHYSICAL_INTF:
        return L7_TRUE;
        break;

      default:
        return L7_FALSE;
    }
  }
  return L7_FALSE;
}

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
L7_RC_t edbObjectPortChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_EDB_MESSAGE_t temp_message;
  nimUSP_t usp;
  NIM_EVENT_COMPLETE_INFO_t status;

  /* unknown event, ignore */
  status.intIfNum     = intIfNum;
  status.component    = L7_EDB_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.event        = event;
  status.correlator   = correlator;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    status.response.rc = L7_FAILURE;
    nimEventStatusCallback(status);
    /* unknown internal interface */
    return L7_FAILURE;
  }
  else if ( edbIsValidIntf(intIfNum) != L7_TRUE)
  {
    status.response.rc = L7_SUCCESS;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }
  else
  {
    temp_message.unit = usp.unit;
    temp_message.slot = usp.slot;
    temp_message.port = usp.port;
    temp_message.intIfNum = intIfNum;
    temp_message.port_event = event;
    temp_message.correlator = correlator;

    switch (event)
    {
      case L7_ATTACH:
        temp_message.messageType = L7_EDB_MESSAGE_TYPE_PORT_CREATE;
        break;

      case L7_DETACH:
        temp_message.messageType = L7_EDB_MESSAGE_TYPE_PORT_DELETE;
        break;

      default:

        nimEventStatusCallback(status);

        return L7_SUCCESS;
        break;
    }
  }
  return osapiMessageSend(edbMessageQueue, (void*)&temp_message, sizeof(temp_message), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM );
}

/*********************************************************************
* @purpose  Callback registered with NIM for startup.
*
* @param    startupPhase - CREATE or ACTIVATE startup
*
* @returns void
*
* @comments  Takes no action on CREATE startup.
*
* @end
*
*********************************************************************/
void edbNimStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
    L7_EDB_MESSAGE_t startup_message;

    if (startupPhase == NIM_INTERFACE_CREATE_STARTUP)
    {
        nimStartupEventDone(L7_EDB_COMPONENT_ID);
        return;
    }

    if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
    {
        startup_message.messageType = L7_EDB_MESSAGE_ACTIVATE_STARTUP;

        osapiMessageSend(edbMessageQueue, (void*)&startup_message,
                         sizeof(startup_message), L7_WAIT_FOREVER,
                         L7_MSG_PRIORITY_NORM);
        return;
    }

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_EDB_COMPONENT_ID,
            "EDB NIM startup callback invoked with invalid phase %d",
            startupPhase);
}

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
L7_RC_t edbObjectUnitJoin(L7_uint32 unit)
{
  L7_EDB_OBJECT_t *unit_p = L7_NULLPTR;

  if (edbObjectCreate(&unit_p) == L7_SUCCESS)
  {
    unit_p->objectType = L7_EDB_OBJECT_TYPE_UNIT;
    unit_p->objectClass = L7_EDB_PHYSICAL_CLASS_CHASSIS;
    unit_p->relPos = unit;

    if (edbObjectChildInsert(unit_p, edbTree_g) == L7_SUCCESS)
    {
      if (edbTreeReindex() == L7_SUCCESS)
        return L7_SUCCESS;

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_EDB_COMPONENT_ID,
              "EDB Failure: Error renumbering tree after adding Unit %d\n", unit);
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNMP_COMPONENT_ID, 
              "EDB Failure: Error adding Unit %d\n", unit);
      /* remove the unit */
      if (edbObjectDestroy(unit_p) != L7_SUCCESS)
        return L7_ERROR;
    }
  }

  /* failure case */
  return L7_FAILURE;
}

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
L7_RC_t edbObjectUnitLeave(L7_uint32 unit)
{
  L7_EDB_OBJECT_t *unit_p = L7_NULLPTR;

  if (edbObjectUnitFind(unit, edbTree_g, &unit_p) == L7_SUCCESS)
  {
    if (edbObjectRemove(unit_p) == L7_SUCCESS &&
        edbObjectDestroy(unit_p) == L7_SUCCESS &&
        edbTreeReindex() == L7_SUCCESS)
      return L7_SUCCESS;
  }
  else
  {
    
	L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNMP_COMPONENT_ID, 
            "EDB Failure: Could not find Unit %d to remove.\n", unit);
    /* If the Unit was removed, then ignore this removal request */
    return L7_SUCCESS;
  }

  /* failure case */
  return L7_FAILURE;
}

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
L7_RC_t edbObjectSlotCreate(L7_uint32 unit, L7_uint32 slot)
{
  L7_EDB_OBJECT_t *unit_p = L7_NULLPTR;
  L7_EDB_OBJECT_t *slot_p = L7_NULLPTR;

  if (edbObjectUnitFind(unit, edbTree_g, &unit_p) == L7_SUCCESS &&
      edbObjectCreate(&slot_p) == L7_SUCCESS)
  {
    slot_p->objectType = L7_EDB_OBJECT_TYPE_SLOT;
    slot_p->objectClass = L7_EDB_PHYSICAL_CLASS_MODULE;
    slot_p->unit = unit;
    slot_p->relPos = slot;

    if (edbObjectChildInsert(slot_p, unit_p) == L7_SUCCESS)
    {
      if (edbTreeReindex() == L7_SUCCESS)
        return L7_SUCCESS;
    }
    else
    {
      /* remove the slot */
      if (edbObjectDestroy(slot_p) != L7_SUCCESS)
        return L7_ERROR;
    }
  }

  /* failure case */
  return L7_FAILURE;
}

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
L7_RC_t edbObjectSlotDelete(L7_uint32 unit, L7_uint32 slot)
{
  L7_EDB_OBJECT_t *unit_p = L7_NULLPTR;
  L7_EDB_OBJECT_t *slot_p = L7_NULLPTR;

  if (edbObjectUnitFind(unit, edbTree_g, &unit_p) == L7_SUCCESS &&
      edbObjectSlotFind(slot, unit_p, &slot_p) == L7_SUCCESS)
  {
    if (edbObjectRemove(slot_p) == L7_SUCCESS &&
        edbObjectDestroy(slot_p) == L7_SUCCESS &&
        edbTreeReindex() == L7_SUCCESS)
      return L7_SUCCESS;
  }
  else
  {
    /* If the Unit or Slot were removed, then ignore this removal request */
    return L7_SUCCESS;
  }

  /* failure case */
  return L7_FAILURE;
}

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
L7_RC_t edbObjectCardPlugin(L7_uint32 unit, L7_uint32 slot)
{
  L7_EDB_OBJECT_t *unit_p = L7_NULLPTR;
  L7_EDB_OBJECT_t *slot_p = L7_NULLPTR;
  L7_EDB_OBJECT_t *card_p = L7_NULLPTR;

  if (edbObjectUnitFind(unit, edbTree_g, &unit_p) == L7_SUCCESS &&
      edbObjectSlotFind(slot, unit_p, &slot_p) == L7_SUCCESS &&
      edbObjectCreate(&card_p) == L7_SUCCESS)
  {
    card_p->objectType = L7_EDB_OBJECT_TYPE_CARD;
    card_p->objectClass = L7_EDB_PHYSICAL_CLASS_MODULE;
    card_p->unit = unit;
    card_p->slot = slot;
    card_p->relPos = 1;

    if (edbObjectChildInsert(card_p, slot_p) == L7_SUCCESS)
    {
      if (edbTreeReindex() == L7_SUCCESS)
        return L7_SUCCESS;
    }
    else
    {
      /* remove the card */
      if (edbObjectDestroy(card_p) != L7_SUCCESS)
        return L7_ERROR;
    }
  }

  /* failure case */
  return L7_FAILURE;
}

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
L7_RC_t edbObjectCardUnplug(L7_uint32 unit, L7_uint32 slot)
{
  L7_EDB_OBJECT_t *unit_p = L7_NULLPTR;
  L7_EDB_OBJECT_t *slot_p = L7_NULLPTR;
  L7_EDB_OBJECT_t *card_p = L7_NULLPTR;

  if (edbObjectUnitFind(unit, edbTree_g, &unit_p) == L7_SUCCESS &&
      edbObjectSlotFind(slot, unit_p, &slot_p) == L7_SUCCESS)
  {
    if (slot_p->child != L7_NULLPTR &&
        slot_p->child->objectType == L7_EDB_OBJECT_TYPE_CARD)
    {
      card_p = slot_p->child;

      if (edbObjectRemove(card_p) == L7_SUCCESS &&
          edbObjectDestroy(card_p) == L7_SUCCESS &&
          edbTreeReindex() == L7_SUCCESS)
        return L7_SUCCESS;
    }
  }
  else
  {
    /* If the Unit or Slot were removed, then ignore this removal request */
    return L7_SUCCESS;
  }

  /* failure case */
  return L7_FAILURE;
}

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
L7_RC_t edbObjectPortCreate(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  L7_EDB_OBJECT_t *unit_p = L7_NULLPTR;
  L7_EDB_OBJECT_t *slot_p = L7_NULLPTR;
  L7_EDB_OBJECT_t *port_p = L7_NULLPTR;

  if (edbObjectUnitFind(unit, edbTree_g, &unit_p) == L7_SUCCESS &&
      edbObjectSlotFind(slot, unit_p, &slot_p) == L7_SUCCESS)
  {
    if (slot_p->child != L7_NULLPTR &&
        slot_p->child->objectType == L7_EDB_OBJECT_TYPE_CARD)
    {
      /* use the card instead */
      slot_p = slot_p->child;
    }

    if (edbObjectCreate(&port_p) == L7_SUCCESS)
    {
      port_p->objectType = L7_EDB_OBJECT_TYPE_PORT;
      port_p->objectClass = L7_EDB_PHYSICAL_CLASS_PORT;
      port_p->unit = unit;
      port_p->slot = slot;
      port_p->relPos = port;

      if (edbObjectChildInsert(port_p, slot_p) == L7_SUCCESS)
      {
        if (edbTreeReindex() == L7_SUCCESS)
          return L7_SUCCESS;
      }
      else
      {
        /* insertion failed, destroy object */
        if (edbObjectDestroy(port_p) != L7_SUCCESS)
          return L7_ERROR;
      }
    }
  }

  /* failure case */
  return L7_FAILURE;
}

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
L7_RC_t edbObjectPortDelete(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  L7_EDB_OBJECT_t *unit_p = L7_NULLPTR;
  L7_EDB_OBJECT_t *slot_p = L7_NULLPTR;
  L7_EDB_OBJECT_t *port_p = L7_NULLPTR;

  if (edbObjectUnitFind(unit, edbTree_g, &unit_p) == L7_SUCCESS &&
      edbObjectSlotFind(slot, unit_p, &slot_p) == L7_SUCCESS)
  {
    if (slot_p->child != L7_NULLPTR &&
        slot_p->child->objectType == L7_EDB_OBJECT_TYPE_CARD)
    {
      /* If this slot contains a card object, look on it for the port */
      slot_p = slot_p->child;
    }

    if (edbObjectPortFind(port, slot_p, &port_p) == L7_SUCCESS)
    {
      if (edbObjectRemove(port_p) == L7_SUCCESS &&
          edbTreeReindex() == L7_SUCCESS)
      {
        if (edbObjectDestroy(port_p) == L7_SUCCESS)
          return L7_SUCCESS;
        return L7_ERROR;
      }
    }
  }
  else
  {
    /* If the Unit, Slot or Port were removed, then ignore this removal request */
    return L7_SUCCESS;
  }

  /* failure case */
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Determine whether an interface is in the attached state.
*
* @param    intIfNum  @b{(input)}  internal interface number
*
* @returns  L7_TRUE if the interface is attached.
*           L7_FALSE otherwise.
*
* @notes    The interface is considered to be attached if the state is
*           either L7_INTF_ATTACHING or L7_INTF_ATTACHED.
*
* @end
*********************************************************************/
static L7_BOOL edbIntfIsAttached(L7_uint32 intIfNum)
{
    L7_NIM_QUERY_DATA_t queryData;

    /* get the interface state */
    queryData.intIfNum = intIfNum;
    queryData.request = L7_NIM_QRY_RQST_STATE;
    if (nimIntfQuery(&queryData) != L7_SUCCESS)
    {
        return L7_FALSE;
    }

    if ((queryData.data.state == L7_INTF_ATTACHING) ||
        (queryData.data.state == L7_INTF_ATTACHED))
        return L7_TRUE;
    else
        return L7_FALSE;
}

/*********************************************************************
*
* @purpose Handle NIM activate startup
*
* @param void
*
* @returns L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t edbNimActivateStartup(void)
{
    L7_uint32 intIfNum;
    nimUSP_t usp;
    L7_RC_t rc;

    rc = nimFirstValidIntfNumber(&intIfNum);
    while (rc == L7_SUCCESS)
    {
      if (edbIsValidIntf(intIfNum) && edbIntfIsAttached(intIfNum))
      {
          if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
          {
              edbObjectPortCreate(usp.unit, usp.slot, usp.port);
          }
      }
      rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
    }

    /* Now ask NIM to send any future changes for these event types */
    PORTEVENT_SETMASKBIT(edbNimEventMask, L7_ATTACH);
    PORTEVENT_SETMASKBIT(edbNimEventMask, L7_DETACH);
    nimRegisterIntfEvents(L7_EDB_COMPONENT_ID, edbNimEventMask);

    nimStartupEventDone(L7_EDB_COMPONENT_ID);

    return L7_SUCCESS;
}

/***************************************************************************************/
/***** Edb Object Functions *********************************************************/
/***************************************************************************************/

/*********************************************************************
*
* @purpose Creates a new Edb Object and initializes values
*
* @param **object_p @b((input))  pointer to receive L7_EDB_OBJECT_t pointer
*
* @returns L7_SUCCESS  if the object is created successfully
* @returns L7_FAILURE  if the object pointer provided is null
* @returns L7_ERROR    if memory could not be allocated for the new object
*
* @end
*
*********************************************************************/
L7_RC_t edbObjectCreate(L7_EDB_OBJECT_t **object_p)
{
  /* verify arguments, pointer to object pointer can't be null */
  if (object_p == L7_NULLPTR)
    return L7_FAILURE;

  /* allocate memory for this object */
  *object_p = (L7_EDB_OBJECT_t*)osapiMalloc(L7_EDB_COMPONENT_ID, sizeof(L7_EDB_OBJECT_t));

  if (*object_p == L7_NULLPTR)
    return L7_ERROR;

  (*object_p)->parent = L7_NULLPTR;
  (*object_p)->child = L7_NULLPTR;
  (*object_p)->nextSibling = L7_NULLPTR;
  (*object_p)->prevSibling = L7_NULLPTR;
  (*object_p)->physicalIndex = 0;
  (*object_p)->relPos = -1;
  (*object_p)->objectClass = L7_EDB_PHYSICAL_CLASS_UNKNOWN;
  (*object_p)->objectType = L7_EDB_OBJECT_TYPE_UNKNOWN;

  return L7_SUCCESS;
}

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
L7_RC_t edbObjectDestroy(L7_EDB_OBJECT_t *object_p)
{
  L7_EDB_OBJECT_t *temp_p = L7_NULLPTR;

  /* verify arguments, pointer to object pointer can't be null,
     object can't be connected to any other object */

  if (object_p == L7_NULLPTR ||
      object_p->parent != L7_NULLPTR ||
      object_p->nextSibling != L7_NULLPTR ||
      object_p->prevSibling != L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_EDB_COMPONENT_ID,
            "EDB:edbObjectDestroy failure: arguments are NULL\n");
    return L7_FAILURE;
  }

  /* first remove any children */
  while (object_p->child != L7_NULLPTR)
  {
    temp_p = object_p->child;
    if (edbObjectRemove(temp_p) != L7_SUCCESS ||
        edbObjectDestroy(temp_p) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_EDB_COMPONENT_ID,
              "EDB:edbObjectDestroy failure: Could not remove children.\n");
      return L7_ERROR;
    }
  }

  /* free this object */
  osapiFree(L7_EDB_COMPONENT_ID, object_p);

  return L7_SUCCESS;
}

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
L7_RC_t edbObjectParentChildMatch(L7_EDB_OBJECT_t *object_p, L7_EDB_OBJECT_t *parent_p)
{
  if (object_p == L7_NULLPTR ||
      (parent_p == L7_NULLPTR && edbTree_g != L7_NULLPTR))
    return L7_FAILURE;

  if (parent_p == L7_NULLPTR)
    return L7_SUCCESS;

  /* stack and chassis objects must exist in a stack */
  if ((object_p->objectClass == L7_EDB_PHYSICAL_CLASS_CHASSIS ||
       object_p->objectClass == L7_EDB_PHYSICAL_CLASS_STACK) &&
      parent_p->objectClass != L7_EDB_PHYSICAL_CLASS_STACK)
    return L7_FAILURE;

  return L7_SUCCESS;
}

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
L7_RC_t edbObjectChildInsert(L7_EDB_OBJECT_t *object_p, L7_EDB_OBJECT_t *parent_p)
{
  L7_EDB_OBJECT_t *temp_p = L7_NULLPTR;

  /* check arguments for null pointers */
  if (object_p == L7_NULLPTR ||
      (parent_p == L7_NULLPTR && edbTree_g != L7_NULLPTR))
    return L7_FAILURE;

  /* check for loops */
  for (temp_p = parent_p; temp_p != L7_NULLPTR; temp_p = temp_p->parent)
  {
    /* if the object contains the parent object at some depth */
    if (temp_p == object_p)
      return L7_ERROR;
  }

  /* check to see if the object is contained or attached to any other objects */
  if (object_p->parent != L7_NULLPTR ||
      object_p->child != L7_NULLPTR ||
      object_p->nextSibling != L7_NULLPTR ||
      object_p->prevSibling != L7_NULLPTR)
    return L7_FAILURE;

  /* check to see if the parent/object match is allowed */
  if (edbObjectParentChildMatch(object_p, parent_p) != L7_SUCCESS)
    return L7_FAILURE;

  /*** ADD the child ***/

  /* check to see if this object is being added as the first object */
  if (parent_p == L7_NULLPTR)
  {
    if (edbTree_g != L7_NULLPTR)
    {
      /* attempt to add second 'first' object */
      return L7_ERROR;
    }

    edbTree_g = object_p;
    return L7_SUCCESS;
  }

  /* check to see if the parent has any child objects */
  if (parent_p->child != L7_NULLPTR)
  {
    if (object_p->relPos < 0)
    {
      /* order isn't infered by index, so add to the end of the list */
      temp_p = parent_p->child;
      while (temp_p->nextSibling != L7_NULLPTR)
        temp_p = temp_p->nextSibling;

      /* insert after temp_p */
      temp_p->nextSibling = object_p;
      object_p->prevSibling = temp_p;
      object_p->parent = parent_p;

      return L7_SUCCESS;
    }
    else
    {
      /* find where to insert this child object */
      for (temp_p = parent_p->child; temp_p != L7_NULLPTR; temp_p = temp_p->nextSibling)
      {
        if (temp_p->relPos == object_p->relPos)
        {
          /* object already exists with the same index */
          return L7_FAILURE;
        }
        else if (temp_p->relPos > object_p->relPos)
        {
          /* insrt before temp_p */
          if (temp_p->prevSibling != L7_NULLPTR)
          {
            object_p->prevSibling = temp_p->prevSibling;
            temp_p->prevSibling->nextSibling = object_p;
          }

          object_p->nextSibling = temp_p;
          temp_p->prevSibling = object_p;
          object_p->parent = parent_p;

          /* if inserting at the beginning, update the parent */
          if (parent_p->child == temp_p)
            parent_p->child = object_p;

          return L7_SUCCESS;
        }
        else if (temp_p->nextSibling == L7_NULLPTR)
        {
          /* insert at the end */
          temp_p->nextSibling = object_p;
          object_p->prevSibling = temp_p;
          object_p->parent = parent_p;
          return L7_SUCCESS;
        }
      }
    }

    /* should not get here */
    return L7_ERROR;
  }

  /* simple addition of child object */
  parent_p->child = object_p;
  object_p->parent = parent_p;

  return L7_SUCCESS;
}

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
L7_RC_t edbObjectRemove(L7_EDB_OBJECT_t *object_p)
{
  /* check argument for null pointer */
  if (object_p == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_EDB_COMPONENT_ID,
            "EDB:edbObjectRemove failure: object_p == L7_NULLPTR\n");
    return L7_FAILURE;
  }

  if (object_p->prevSibling != L7_NULLPTR)
  {
    /* point previous sibling to next if any */
    (object_p->prevSibling)->nextSibling = object_p->nextSibling;
  }

  if (object_p->nextSibling != L7_NULLPTR)
  {
    /* point next sibling to previous if any */
    (object_p->nextSibling)->prevSibling = object_p->prevSibling;
  }

  /* if containing object points to this object */
  if (object_p->parent != L7_NULLPTR &&
      (object_p->parent)->child == object_p)
  {
    /* point containing object to next sibling */
    (object_p->parent)->child = object_p->nextSibling;
  }

  /* check to see if this is the first object */
  if (object_p == edbTree_g)
  {
    edbTree_g = L7_NULLPTR;
  }

  /* clear pointers */
  object_p->parent = L7_NULLPTR;
  object_p->prevSibling = L7_NULLPTR;
  object_p->nextSibling = L7_NULLPTR;

  return L7_SUCCESS;
}

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
L7_RC_t edbTreeReindex(void)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 maxIndex = 0;

  if (edbTree_g != L7_NULLPTR)
  {
    rc = edbObjectReindex(edbTree_g, &maxIndex);
  }

  edbTreeCount_g = maxIndex;

  return rc;
}

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
L7_RC_t edbObjectReindex(L7_EDB_OBJECT_t *object_p, L7_uint32 *index)
{
  L7_RC_t rc;
  L7_EDB_OBJECT_t *temp_p = L7_NULLPTR;

  if (object_p == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_EDB_COMPONENT_ID,
            "EDB:edbObjectReindex failure: arguments are NULL\n");
    return L7_FAILURE;
  }

  *index += 1;

  /* set current object's enterprisePhysicalIndex value */
  object_p ->physicalIndex = *index;

  /* if this object contains other objects, reindex the child objects as well */
  if (object_p->child != L7_NULLPTR)
  {
    temp_p = object_p->child;
    while (temp_p != L7_NULLPTR)
    {
      rc = edbObjectReindex(temp_p, index);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_EDB_COMPONENT_ID,
                "EDB:edbObjectReindex failure: recursive call failed\n");
        return rc;
      }
      temp_p = temp_p->nextSibling;
    }
  }

  return L7_SUCCESS;
}

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
L7_RC_t edbTreeMaxIndex(L7_uint32 *maxIndex)
{
  if (maxIndex == L7_NULLPTR)
    return L7_FAILURE;

  *maxIndex = edbTreeCount_g;
  return L7_SUCCESS;
}

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
L7_RC_t edbObjectFind(L7_int32 physicalIndex, L7_EDB_OBJECT_t **object_p)
{
  L7_EDB_OBJECT_t *tempObject_p = L7_NULLPTR;

  /* check arguments for known out of range errors */
  if (physicalIndex < 0 ||
      physicalIndex > edbTreeCount_g ||
      object_p == L7_NULLPTR)
    return L7_FAILURE;

  tempObject_p = edbTree_g;
  while (tempObject_p != L7_NULLPTR)
  {
    /* if the current object is what we're looking for, return */
    if (tempObject_p->physicalIndex == physicalIndex)
    {
      *object_p = tempObject_p;
      return L7_SUCCESS;
    }
    /* else, if this object has siblings, check the next sibling */
    else if ((tempObject_p->nextSibling != L7_NULLPTR) &&
             (tempObject_p->nextSibling->physicalIndex <= physicalIndex))
    {
      /* if the next sibling is less than target index, no child of this
         object would have that index, so look on the next sibling's subtree */
      tempObject_p = tempObject_p->nextSibling;
    }
    else
    {
      /* must then be under this object */
      tempObject_p = tempObject_p->child;
    }
    /* look at new tempObject_p */
  }

  if (tempObject_p == L7_NULLPTR)
    return L7_FAILURE;

  *object_p = tempObject_p;
  return L7_SUCCESS;
}

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
L7_RC_t edbObjectUnitFind(L7_uint32 unitIndex, L7_EDB_OBJECT_t *stack_p, L7_EDB_OBJECT_t **object_p)
{
  L7_EDB_OBJECT_t *temp_p = L7_NULLPTR;

  /* check arguments */
  if (object_p == L7_NULLPTR ||
      stack_p == L7_NULLPTR)
    return L7_FAILURE;

  /* if in a stack environment */
  if (edbStackPresent() == L7_TRUE)
  {
    if (stack_p->objectType != L7_EDB_OBJECT_TYPE_STACK)
      return L7_FAILURE;

    temp_p = stack_p->child;
    while (temp_p != L7_NULLPTR)
    {
      if (temp_p->objectType == L7_EDB_OBJECT_TYPE_UNIT &&
          temp_p->relPos == unitIndex)
      {
        *object_p = temp_p;
        return L7_SUCCESS;
      }
      temp_p = temp_p->nextSibling;
    }
  }
  else
  {
    if (stack_p->objectType != L7_EDB_OBJECT_TYPE_UNIT)
      return L7_FAILURE;

    if (stack_p->relPos == unitIndex)
    {
      *object_p = stack_p;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

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
L7_RC_t edbObjectSlotFind(L7_uint32 slotIndex, L7_EDB_OBJECT_t *unit_p, L7_EDB_OBJECT_t **object_p)
{
  L7_EDB_OBJECT_t *temp_p = L7_NULLPTR;

  if (object_p == L7_NULLPTR ||
      unit_p == L7_NULLPTR ||
      unit_p->objectType != L7_EDB_OBJECT_TYPE_UNIT)
    return L7_FAILURE;

  temp_p = unit_p->child;
  while (temp_p != L7_NULLPTR)
  {
    if (temp_p->objectType == L7_EDB_OBJECT_TYPE_SLOT &&
        temp_p->relPos == slotIndex)
    {
      *object_p = temp_p;
      return L7_SUCCESS;
    }
    temp_p = temp_p->nextSibling;
  }

  return L7_FAILURE;
}

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
L7_RC_t edbObjectPortFind(L7_uint32 portIndex, L7_EDB_OBJECT_t *slot_p, L7_EDB_OBJECT_t **object_p)
{
  L7_EDB_OBJECT_t *temp_p = L7_NULLPTR;

  if (object_p == L7_NULLPTR ||
      slot_p == L7_NULLPTR ||
      slot_p->objectType != L7_EDB_OBJECT_TYPE_SLOT)
    return L7_FAILURE;

  /* if this slot is a container, then it should contain a card with the ports on it */
  if (slot_p->objectClass == L7_EDB_PHYSICAL_CLASS_CONTAINER)
  {
    if (slot_p->child != L7_NULLPTR &&
        slot_p->child->objectType == L7_EDB_OBJECT_TYPE_CARD)
    {
      temp_p = slot_p->child->child;
    }
    else
    {
      /* container slot with no card in it */
      return L7_FAILURE;
    }
  }
  else
  {
    temp_p = slot_p->child;
  }

  /* look through the ports on this slot/card */
  while (temp_p != L7_NULLPTR)
  {
    if (temp_p->objectType == L7_EDB_OBJECT_TYPE_PORT &&
        temp_p->relPos == portIndex)
    {
      *object_p = temp_p;
      return L7_SUCCESS;
    }
    temp_p = temp_p->nextSibling;
  }

  return L7_FAILURE;
}

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
L7_RC_t edbConfigChangeTime(L7_uint32 *configChangeTime)
{
  (void)osapiSemaTake(edbSemaId, L7_WAIT_FOREVER);

  *configChangeTime = edbConfigChangeTime_g;

  (void)osapiSemaGive(edbSemaId);

  return L7_SUCCESS;
}

/* End Function Definitions */

/***************************************************************************************/
/***** Test Functions ******************************************************************/
/***************************************************************************************/

/*********************************************************************
*
* @purpose Test function to print out an object by index
*
* @returns L7_SUCCESS
*
* @end
*
*********************************************************************/
void edbPrintIndex(L7_uint32 physicalIndex)
{
  L7_EDB_OBJECT_t *object_p = L7_NULLPTR;

  if (osapiSemaTake(edbSemaId, 1000) != L7_SUCCESS)
  {
    sysapiPrintf("edbPrintIndex(%d): Could not get semaphore\n", physicalIndex);
    return;
  }

  if (edbObjectFind(physicalIndex, &object_p) == L7_SUCCESS)
  {
    sysapiPrintf("%d: %p\n", physicalIndex, object_p);
    sysapiPrintf(" |- parent = %d (%p)\n", object_p->parent == L7_NULLPTR ? 0 : object_p->parent->physicalIndex, object_p->parent);
    sysapiPrintf(" |- child = %d (%p)\n", object_p->child == L7_NULLPTR ? 0 : object_p->child->physicalIndex, object_p->child);
    sysapiPrintf(" |- nextSibling = %d (%p)\n", object_p->nextSibling == L7_NULLPTR ? 0 : object_p->nextSibling->physicalIndex, object_p->nextSibling);
    sysapiPrintf(" |- prevSibling = %d (%p)\n", object_p->prevSibling == L7_NULLPTR ? 0 : object_p->prevSibling->physicalIndex, object_p->prevSibling);

    switch (object_p->objectClass)
    {
      case L7_EDB_PHYSICAL_CLASS_OTHER:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_OTHER)\n", object_p->objectClass);
        break;

      case L7_EDB_PHYSICAL_CLASS_UNKNOWN:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_UNKNOWN)\n", object_p->objectClass);
        break;

      case L7_EDB_PHYSICAL_CLASS_CHASSIS:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_CHASSIS)\n", object_p->objectClass);
        break;

      case L7_EDB_PHYSICAL_CLASS_BACKPLANE:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_BACKPLANE)\n", object_p->objectClass);
        break;

      case L7_EDB_PHYSICAL_CLASS_CONTAINER:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_CONTAINER)\n", object_p->objectClass);
        break;

      case L7_EDB_PHYSICAL_CLASS_POWERSUPPLY:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_POWERSUPPLY)\n", object_p->objectClass);
        break;

      case L7_EDB_PHYSICAL_CLASS_FAN:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_FAN)\n", object_p->objectClass);
        break;

      case L7_EDB_PHYSICAL_CLASS_SENSOR:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_SENSOR)\n", object_p->objectClass);
        break;

      case L7_EDB_PHYSICAL_CLASS_MODULE:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_MODULE)\n", object_p->objectClass);
        break;

      case L7_EDB_PHYSICAL_CLASS_PORT:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_PORT)\n", object_p->objectClass);
        break;

      case L7_EDB_PHYSICAL_CLASS_STACK:
        sysapiPrintf(" |- objectClass = %d (L7_EDB_PHYSICAL_CLASS_STACK)\n", object_p->objectClass);
        break;

      default:
        sysapiPrintf(" |- objectClass = %d (unknown)\n", object_p->objectClass);
        break;
    }

    switch (object_p->objectType)
    {
      case L7_EDB_OBJECT_TYPE_UNKNOWN:
        sysapiPrintf(" |- objectType = %d (L7_EDB_OBJECT_TYPE_UNKNOWN)\n", object_p->objectType);
        break;

      case L7_EDB_OBJECT_TYPE_STACK:
        sysapiPrintf(" |- objectType = %d (L7_EDB_OBJECT_TYPE_STACK)\n", object_p->objectType);
        break;

      case L7_EDB_OBJECT_TYPE_UNIT:
        sysapiPrintf(" |- objectType = %d (L7_EDB_OBJECT_TYPE_UNIT)\n", object_p->objectType);
        break;

      case L7_EDB_OBJECT_TYPE_SLOT:
        sysapiPrintf(" |- objectType = %d (L7_EDB_OBJECT_TYPE_SLOT)\n", object_p->objectType);
        break;

      case L7_EDB_OBJECT_TYPE_CARD:
        sysapiPrintf(" |- objectType = %d (L7_EDB_OBJECT_TYPE_CARD)\n", object_p->objectType);
        break;

      case L7_EDB_OBJECT_TYPE_PORT:
        sysapiPrintf(" |- objectType = %d (L7_EDB_OBJECT_TYPE_PORT)\n", object_p->objectType);
        break;

      case L7_EDB_OBJECT_TYPE_POWERSUPPLY:
        sysapiPrintf(" |- objectType = %d (L7_EDB_OBJECT_TYPE_POWERSUPPLY)\n", object_p->objectType);
        break;

      case L7_EDB_OBJECT_TYPE_FAN:
        sysapiPrintf(" |- objectType = %d (L7_EDB_OBJECT_TYPE_FAN)\n", object_p->objectType);
        break;

      default:
        sysapiPrintf(" |- objectType = %d (unknown)\n", object_p->objectType);
        break;
    }

    sysapiPrintf(" |- relpos = %d\n", object_p->relPos);
  }
  else
  {
    sysapiPrintf("%d: NOT FOUND\n", physicalIndex);
  }

  (void)osapiSemaGive(edbSemaId);
}

/*********************************************************************
*
* @purpose Test function to print out the edb tree.
*
* @returns L7_SUCCESS
*
* @end
*
*********************************************************************/
void edbPrint(void)
{
  L7_uint32 physicalIndex;

  if (osapiSemaTake(edbSemaId, 1000) != L7_SUCCESS)
  {
    sysapiPrintf("edbPrint: Could not get semaphore\n");
    return;
  }

  sysapiPrintf("Global Edb MIB Values:\n");
  sysapiPrintf("-------------------------\n");
  sysapiPrintf("edbSemaId = %p\n", edbSemaId);
  sysapiPrintf("edbMessageQueue = %p\n", edbMessageQueue);
  sysapiPrintf("edbTreeCount_g = %d\n", edbTreeCount_g);
  sysapiPrintf("edbConfigChange = %s\n", edbConfigChange == L7_TRUE ? "L7_TRUE" : "L7_FALSE");
  sysapiPrintf("edbConfigChangeTime_g = %d\n", edbConfigChangeTime_g);
  sysapiPrintf("edbStackPresent() = %s\n", edbStackPresent() == L7_TRUE ? "L7_TRUE" : "L7_FALSE");
  sysapiPrintf("\n");
  sysapiPrintf("Printout of Edb MIB Objects:\n");
  sysapiPrintf("-------------------------------\n");

  (void)osapiSemaGive(edbSemaId);

  for (physicalIndex = 1; physicalIndex <= edbTreeCount_g; physicalIndex++)
  {
    edbPrintIndex(physicalIndex);
  }

  sysapiPrintf("-------------------------------\n");
}
