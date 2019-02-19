
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename cardmgr_api.c
 *
 * @purpose Card Manager API functions
 *
 * @component Card Manager
 *
 * @comments none
 *
 * @create 12/20/2002
 *
 * @author kmcdowell
 * @end
 *
 **********************************************************************/
#include "cardmgr_include.h"
#include "sysapi_hpc_slotmapper.h"
#include "sysapi_hpc_chassis.h"
#include "cfg_change_api.h"

/*****************************************************************************/
/* CMGR Global Data Structures                                               */
/*****************************************************************************/
cmgrNotifyList_t cmgrNotifyList[L7_LAST_COMPONENT_ID];
void (*cmgrCardConfigCallback)(L7_CMGR_CONFIG_EVENT_TYPES_t ev) = L7_NULLPTR;
L7_cmgr_state_t cmgrState = L7_CNFGR_STATE_IDLE;

extern L7_cardMgrCfgFileData_t cmgrCfgData;

extern void *cmgrSemaId;
extern void  *cmgrQueue;
extern L7_char8 cardTypeStr[(SYSAPI_CARD_TYPE_LAST+1)][30];
extern L7_char8 cardStatusStr[L7_CARD_STATUS_TOTAL][30];
extern L7_char8 cardEventStr[SYSAPI_CARD_EVENT_LAST][30];
/*****************************************************************************/
/* CMGR Local  Data Structures                                               */
/*****************************************************************************/
static L7_BOOL initPhase1Done = L7_FALSE;


/*****************************************************************************/
/* Initialization APIs                                                       */
/*****************************************************************************/

/*********************************************************************
 *
 * @purpose  CNFGR Initialization for Card Manager component
 *
 * @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
 *                                             CNFGR request
 *
 * @returns  None
 *
 * @notes    This API is provided to allow the Configurator to issue a
 *           request to the Card Manager.  This function is re-entrant.
 *           The CNFGR request should be transfered to the Card Manager
 *           task as quickly as possible to avoid processing in the
 *           Configurator's task.
 *
 * @notes    This function completes Configurator requests asynchronously.
 *           If there is a failure, this function returns synchronously.
 *           The return value is presented to the configurator by calling the
 *           cnfgrApiCallback(). The following are the possible return codes:
 *           L7_SUCCESS - There were no errors. Response is available.
 *           L7_ERROR   - There were errors. Reason code is available.
 *
 * @notes    The following are valid response:
 *           L7_CNFGR_CMD_COMPLETE
 *
 * @notes    The following are valid error reason code:
 *           L7_CNFGR_CB_ERR_RC_INVALID_CMD
 *           L7_CNFGR_ERR_RC_FATAL
 *           L7_CNFGR_ERR_RC_OUT_OF_SEQUENCE
 *
 * @notes    This function runs in the configurator's thread. Care should be
 *           taken when blocking this thread.!
 *
 * @end
 *********************************************************************/
void cmgrApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  cmgr_cmpdu_t cmpdu;
  L7_CNFGR_CB_DATA_t cbData;
  L7_BOOL commandComplete = L7_TRUE;

  do
  {

    if (pCmdData == L7_NULLPTR)
    {
      cbData.correlator = 0;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
      cbData.asyncResponse.rc = L7_ERROR;

      LOG_MSG("cmgrApiCnfgrCommand: Invalid command \n");
      break;   /* goto EXIT: */
    }

    /* If request is for Initialize Phase 1, call function to create
     * component's task, queues, etc.  All other command/requests get put on
     * component's queue to be processed in component's thread.
     *
     * NOTE: All the phase 1 initialization is done here. Upon completion
     *       the cardMgr is ready to interface with other components, e.g.
     *       registration, etc.
     */
    if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
	(pCmdData->type == L7_CNFGR_RQST) &&
	(pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
    {
      cmgrState = L7_CNFGR_STATE_P1;
      cbData.asyncResponse.rc = cmgrStartTask(pCmdData);
      if ( cbData.asyncResponse.rc != L7_SUCCESS )
      {
	cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_FATAL;
	cbData.correlator = pCmdData->correlator;
	cbData.asyncResponse.rc = L7_ERROR;

	/* NOTE: cmgrStartTask issued LOG_MSG */
	break; /* goto EXIT */
      }

      initPhase1Done = L7_TRUE;
      cbData.asyncResponse.u.response = L7_CNFGR_CMD_COMPLETE;
      cbData.correlator = pCmdData->correlator;
    }
    else
    {
      if ( initPhase1Done == L7_FALSE )
      {
	cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_OUT_OF_SEQUENCE;
	cbData.correlator = pCmdData->correlator;
	cbData.asyncResponse.rc = L7_ERROR;

	LOG_MSG("cmgrApiCnfgrCommand: CardMgr is not initialized... Warning \n");
	break;   /* goto EXIT: */

      }

      cbData.asyncResponse.rc = cmgrMakeCnfgrCmpdu(pCmdData, &cmpdu);

      if ((cmgrQueue == L7_NULLPTR) || (cbData.asyncResponse.rc != L7_SUCCESS))
      {
	cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_FATAL;
	cbData.correlator = pCmdData->correlator;
	cbData.asyncResponse.rc = L7_ERROR;

	LOG_MSG("cmgrApiCnfgrCommand: Internal fatal error \n");
	break;   /* goto EXIT: */
      }

      /* Send the message to cmgrTask --
       *
       * NOTE: At this point there is a msgQ and cmgrTask operational.
       */
      cbData.asyncResponse.rc = osapiMessageSend(cmgrQueue, &cmpdu, sizeof(cmgr_cmpdu_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM );

      if ( cbData.asyncResponse.rc != L7_SUCCESS )
      {
	cbData.correlator = pCmdData->correlator;
	cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_FATAL;
	cbData.asyncResponse.rc = L7_ERROR;

	LOG_MSG("cmgrApiCnfgrCommand: Failed to send message...fatal error \n");
	break; /* goto EXIT */
      }

      /* a message has been sent to the cmgrTask. cmgrTask will issue completion response
       * to the Configurator when done.
       */
      commandComplete = L7_FALSE;
    }

  } while ( 0 );

  /* EXIT:  */
  if ( commandComplete == L7_TRUE )
    cnfgrApiCallback( &cbData );

  return;
}

/*****************************************************************************/
/* HPCL callback APIs                                                        */
/*****************************************************************************/


/*********************************************************************
 *
 * @purpose Callback function to handle card plug in and unplug
 *          messages from HPC.
 *
 * @param  L7_uint32    slot              Slot ID of the changed card
 * @param  L7_uint32    cardTypeId        Card type Identifier
 * @param  L7_uint32    event             Card plugin or unplug
 *
 * @returns L7_SUCCESS,   if success
 * @returns L7_FAILURE,   if parameters are invalid or other error
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void cmgrCardReportCallBack(L7_uint32 slot, L7_uint32 cardTypeId,
    hpcEvent_t event)
{
  cmgr_cmpdu_t cmpdu;
  L7_uint32 rc;
  SYSAPI_CARD_TYPE_t myModuleType;
  L7_int32  mySlot, phySlotId=0;
  SYSAPI_CARD_INDEX_t myModuleIndex;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardTypeDb;

  sysapiHpcCurrentSlotGet(&mySlot);
  (void)sysapiHpcSlotMapMPModuleInfoGet(mySlot,&myModuleType, &myModuleIndex);
  (void)sysapiHpcSlotMapMPPhysicalSlotGet(event.moduleType, event.moduleIndex,
					  &phySlotId);

  cmpdu.local = L7_TRUE;
  if(myModuleType == SYSAPI_CARD_TYPE_CONTROL)
  {
    switch(event.moduleType)
    {
      case SYSAPI_CARD_TYPE_CONTROL:
	if(event.moduleIndex != myModuleIndex)
	  cmpdu.local = L7_FALSE;
	break;
      case SYSAPI_CARD_TYPE_LINE:
	cmpdu.local = L7_FALSE;
	if(event.event == SYSAPI_CARD_EVENT_PLUG)
	{
	  /* set power ON if it is an LM slot */
	  cmgrSlotPowerSet (cmpdu.unit, phySlotId, POWER_ON, POWER_SYSTEM);
	}
	break;
      default:
	cmpdu.local = L7_TRUE;
	break;
    }
  }

  if(cmpdu.local == L7_FALSE)
    return;

  /* When unit is not acting as the management unit
   ** the card information is always stored in
   ** unit zero area.
   */
  rc = unitMgrNumberGet (&cmpdu.unit);
  if (rc != L7_SUCCESS)
  {
    cmpdu.unit = 0;
  }

  cardTypeDb = sysapiHpcCardDbEntryGet(cardTypeId);

  if ((cardTypeDb != L7_NULLPTR) && (cardTypeDb->type != SYSAPI_CARD_TYPE_LINE))
  {
    cmpdu.unit = L7_LOGICAL_UNIT;
  }

  cmpdu.cmpdu_data.statusUpdate.slotId = slot;
  cmpdu.cmpdu_data.statusUpdate.phySlotId =  phySlotId;
  cmpdu.cmpdu_data.statusUpdate.slotStatus = L7_TRUE;
  cmpdu.cmpdu_data.statusUpdate.insertedCardTypeId = cardTypeId;
  cmpdu.cmpdu_data.statusUpdate.cardType = event.moduleType;
  cmpdu.cmpdu_data.statusUpdate.cardIndex = event.moduleIndex;

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardReportCallBack:  slot:%d cardTypeId:0x%x unit:%d\n",
      slot, cardTypeId, cmpdu.unit);

  CMGR_DEBUG(CMGR_LOG_FLAG, "event:%s cardType:%s cardIndex:%d\n", cardEventStr[event.event],
      cardTypeStr[event.moduleType], event.moduleIndex);

  if (event.event == SYSAPI_CARD_EVENT_PLUG)
  {
    cmpdu.cmgrPacketType = L7_CMPDU_CARD_REPORT_CARD_PLUGIN;
    cmpdu.cmpdu_data.statusUpdate.cardStatus = L7_CARD_STATUS_PLUG;
  }
  else if (event.event == SYSAPI_CARD_EVENT_UNPLUG)
  {
    cmpdu.cmgrPacketType = L7_CMPDU_CARD_REPORT_CARD_UNPLUG;
    cmpdu.cmpdu_data.statusUpdate.cardStatus = L7_CARD_STATUS_UNPLUG;
  }
  else if (event.event == SYSAPI_CARD_EVENT_FAILURE)
  {
    cmpdu.cmgrPacketType = L7_CMPDU_CARD_REPORT_CARD_FAILURE;
    cmpdu.cmpdu_data.statusUpdate.cardStatus = L7_CARD_STATUS_FAILED;
  }

  /* put message on CM queue */
  if (cmgrQueue == L7_NULLPTR)
  {
    LOG_MSG("cardMgr(): cmgrQueue has not been created \n");
  }
  rc = osapiMessageSend(cmgrQueue, &cmpdu, sizeof(cmgr_cmpdu_t), 0, L7_MSG_PRIORITY_NORM );
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("cardMgr(): cardReportCallback send failed\n");
  }

  return;
}



/*****************************************************************************/
/* CMPDU API - puts msg in CM queue                                          */
/*****************************************************************************/

/*********************************************************************
 *
 * @purpose Callback function to handle Card Manager Protocol messages.
 *          This function gets called when a CMPDU is received from
 *          another unit.
 *
 * @param  cmgr_cmpdu_t *cmpdu   Card Manager Protocol Data unit
 =*
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
void cmgrReceiveCMPDUCallback(L7_uchar8* hpcCmpdu, L7_uint32 msgLen)
{
  L7_uint32 rc;
  cmgr_cmpdu_t cmpdu;

  /* unpack message */
  if (hpcCmpdu == L7_NULLPTR)
  {
    return;
  }
  memcpy(&cmpdu, hpcCmpdu, sizeof (cmpdu));

  /* put message on CM queue */
  if (cmgrQueue == L7_NULLPTR)
  {
    LOG_MSG("cardMgr(): cmgrQueue has not been created \n");
  }
  rc = osapiMessageSend(cmgrQueue, &cmpdu, sizeof(cmgr_cmpdu_t), 0, L7_MSG_PRIORITY_NORM );
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("cardMgr(): cardReportCallback send failed\n");
  }
  return;
}

/*****************************************************************************/
/* Logical Card -- APIs                                                      */
/*****************************************************************************/

/*********************************************************************
 *
 * @purpose Get unit and slot of the LAG slot.
 *
 * @param  L7_uint32    unit      Unit ID of the logical unit
 * @param  L7_uint32    slot      Slot ID of the created port
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if parameters are invalid or other error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrLagCardUSPGet (L7_uint32 *unit, L7_uint32 *slot)
{

  return cmgrCMDBLagCardUSPGet (unit, slot);
}

/*********************************************************************
 *
 * @purpose Get unit and slot of the vlan card.
 *
 * @param  L7_uint32    unit      Unit ID of the logical unit
 * @param  L7_uint32    slot      Slot ID of the deleted port
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if parameters are invalid or other error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrVlanCardUSPGet (L7_uint32 *unit, L7_uint32 *slot)
{

  return cmgrCMDBVlanCardUSPGet (unit, slot);
}

/*****************************************************************************/
/* Callback Registration APIs                                                */
/*****************************************************************************/


/*********************************************************************
 * @purpose  Register a routine to be called when a trap occurrs.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
 * @param    notify         pointer to a routine to be invoked for card
 *                          creation.  Each routine has the following parameters:
 *                          (unit, slot, ins_cardTypeId, cfg_cardTypeId, event).
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrAllTrapsRegister(L7_COMPONENT_IDS_t registrar_ID,
    void (*notify)(L7_uint32 unit,
      L7_uint32 slot,
      L7_uint32 ins_cardTypeId,
      L7_uint32 cfg_cardTypeId,
      trapMgrNotifyEvents_t event))
{
  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("CMGR registrar_ID %d greater than CMGR_USER_LAST\n", registrar_ID);
    return L7_FAILURE;
  }

  if (cmgrNotifyList[registrar_ID].notify_trap_event != L7_NULLPTR)
  {
    LOG_MSG("CMGR registrar_ID %d already registered\n", registrar_ID);
    return L7_FAILURE;
  }

  cmgrNotifyList[registrar_ID].registrar_ID = registrar_ID;
  cmgrNotifyList[registrar_ID].notify_trap_event = notify;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Notify registered users of a trap event
 *
 * @param    unit            the unit id
 * @param    slot            the slot id
 * @param    ins_cardTypeId  id of inserted card
 * @param    cfg_cardTypeId  id of configured card
 * @param    event           notification event (see trapMgrNotifyEvents_t)
 *
 * @returns  Void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cmgrNotifyRegisteredUsers(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 ins_cardTypeId, L7_uint32 cfg_cardTypeId,
    trapMgrNotifyEvents_t event)
{
  L7_uint32 i;

  if ((event >= TRAPMGR_CARD_MISMATCH_EVENT) &&
      (event < TRAPMGR_CARD_MAX_EVENTS))
  {
    for (i = 1; i < L7_LAST_COMPONENT_ID; i++)
    {
      if (cmgrNotifyList[i].registrar_ID != L7_NULL  &&
	  (i == L7_TRAPMGR_COMPONENT_ID))
      {
	cmgrNotifyList[i].notify_trap_event(unit, slot, ins_cardTypeId,
	    cfg_cardTypeId, event);
      }
    }
  }

  return;
}

/*********************************************************************
 * @purpose  deregister a routine to be called when a trap occurrs.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
 *
 * @returns  none
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cmgrAllTrapsDeregister( L7_COMPONENT_IDS_t registrar_ID )
{
  return;
}

/*********************************************************************
 * @purpose  Register a routine to be called when a card is created.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
 * @param    *notify        pointer to a routine to be invoked for card
 *                          creation.  Each routine has the following parameters:
 *                          (unit, slot, cardId).
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterCardCreate( L7_COMPONENT_IDS_t registrar_ID,
    L7_uint32 (*notify)(L7_uint32 unit,
      L7_uint32 slot,
      L7_uint32 cardId))
{
  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("CMGR registrar_ID %d greater then CMGR_USER_LAST\n", registrar_ID);
    return(L7_FAILURE);
  }

  if (cmgrNotifyList[registrar_ID].notify_card_create != L7_NULL)
  {
    LOG_MSG("CMGR registrar_ID %d already registered\n",registrar_ID);
    return(L7_FAILURE);
  }

  cmgrNotifyList[registrar_ID].registrar_ID = registrar_ID;
  cmgrNotifyList[registrar_ID].notify_card_create = notify;
  return(L7_SUCCESS);

}
/*********************************************************************
 * @purpose  Register a routine to be called before and after creating slots
 *           to apply configuration
 *
 * @param    *notify        pointer to a routine to be invoked for 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *                                 
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterCardConfig(void (*notify)(L7_CMGR_CONFIG_EVENT_TYPES_t ev))
{
  cmgrCardConfigCallback = notify;
  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Register a routine to be called when a card is cleared.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
 * @param    *notify        pointer to a routine to be invoked for card
 *                          clear.  Each routine has the following parameters:
 *                          (unit, slot).
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterCardClear( L7_COMPONENT_IDS_t registrar_ID,
    L7_uint32 (*notify)(L7_uint32 unit,
      L7_uint32 slot))
{
  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("CMGR registrar_ID %d greater then CMGR_USER_LAST\n", registrar_ID);
    return(L7_FAILURE);
  }

  if (cmgrNotifyList[registrar_ID].notify_card_clear != L7_NULL)
  {
    LOG_MSG("CMGR registrar_ID %d already registered\n",registrar_ID);
    return(L7_FAILURE);
  }

  cmgrNotifyList[registrar_ID].registrar_ID = registrar_ID;
  cmgrNotifyList[registrar_ID].notify_card_clear = notify;
  return(L7_SUCCESS);

}

/*********************************************************************
 * @purpose  Register a routine to be called when a card is plugged in.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
 * @param    *notify        pointer to a routine to be invoked for card
 *                          plug in.  Each routine has the following parameters:
 *                          (unit, slot, cardId).
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterCardPlugin( L7_COMPONENT_IDS_t registrar_ID,
    L7_RC_t  (*notify)(L7_uint32 unit,
      L7_uint32 slot,
      L7_uint32 cardId))
{
  L7_uint32 unit, slot;
  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("CMGR registrar_ID %d greater then CMGR_USER_LAST\n", registrar_ID);
    return(L7_FAILURE);
  }

  if (cmgrNotifyList[registrar_ID].notify_card_plugin != L7_NULL)
  {
    LOG_MSG("CMGR registrar_ID %d already registered\n",registrar_ID);
    return(L7_FAILURE);
  }

  cmgrNotifyList[registrar_ID].registrar_ID = registrar_ID;
  cmgrNotifyList[registrar_ID].notify_card_plugin = notify;

  for(unit = 0; unit < CMGR_MAX_UNITS_f; unit++)
  {
    for(slot = 0; slot < cmgrCMDBUnitMaxSlotsGet(unit); slot++)
    {
      cmgrCMDBPluginSendPendingNotif(registrar_ID, unit, slot);
    }
  }

  return(L7_SUCCESS);

}

/*********************************************************************
 * @purpose  Register a routine to be called when a card is unplugged.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
 * @param    *notify        pointer to a routine to be invoked for card
 *                          unplug.  Each routine has the following parameters:
 *                          (unit, slot).
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterCardUnplug( L7_COMPONENT_IDS_t registrar_ID,
    L7_RC_t (*notify)(L7_uint32 unit,
      L7_uint32 slot))
{
  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("CMGR registrar_ID %d greater then CMGR_USER_LAST\n", registrar_ID);
    return(L7_FAILURE);
  }

  if (cmgrNotifyList[registrar_ID].notify_card_unplug != L7_NULL)
  {
    LOG_MSG("CMGR registrar_ID %d already registered\n",registrar_ID);
    return(L7_FAILURE);
  }

  cmgrNotifyList[registrar_ID].registrar_ID = registrar_ID;
  cmgrNotifyList[registrar_ID].notify_card_unplug = notify;
  return(L7_SUCCESS);

}

/*********************************************************************
 * @purpose  Register a routine to be called when a port is changed.
 *
 * @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
 * @param    *notify        pointer to a routine to be invoked for port
 *                          change.  Each routine has the following
 *                          parameters: (unit, slot, port, cardType,
 *                          event, portData).
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrRegisterPortChange( L7_COMPONENT_IDS_t registrar_ID,
    L7_RC_t (*notify)(L7_uint32 unit,
      L7_uint32 slot,
      L7_uint32 port,
      L7_uint32 cardType,
      L7_PORT_EVENTS_t event,
      SYSAPI_HPC_PORT_DESCRIPTOR_t *portData))
{
  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("CMGR registrar_ID %d greater then CMGR_USER_LAST\n", registrar_ID);
    return(L7_FAILURE);
  }

  if (cmgrNotifyList[registrar_ID].notify_port_change != L7_NULL)
  {
    LOG_MSG("CMGR registrar_ID %d already registered\n",registrar_ID);
    return(L7_FAILURE);
  }

  cmgrNotifyList[registrar_ID].registrar_ID = registrar_ID;
  cmgrNotifyList[registrar_ID].notify_port_change = notify;
  return(L7_SUCCESS);

}


/*****************************************************************************/
/* Management/USMDB APIs                                                     */
/*****************************************************************************/

/*********************************************************************
 *
 * @purpose Configure a card in a slot
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot
 * @param  L7_uint32   cardId    Configured Card ID
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This configures a card with the given card type in a slot.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotCardConfigSet(L7_uint32 unit,
    L7_uint32 slot,
    L7_uint32 cardId)
{
  L7_RC_t rc;

  /* NOTE: This check is performed to prevent slot re-configuration.
   *
   * If cards are not pluggable, the slot is automatically
   * configured when a unit is preconfigured. Also, the ports
   * are created for the slot supported card.
   */
  if (cmgrCMDBConfigCardIdGet(unit, slot) == cardId)
  {
    /* card type is same as the one already configured for the slot */
    return L7_ALREADY_CONFIGURED;
  }

  if (authenticateCardType(unit, slot, cardId) != L7_SUCCESS)
  {
    /* card type is unsupported */
    return L7_FAILURE;
  }

  else if (cmgrCMDBConfigCardIdGet(unit, slot) == L7_CMGR_CARD_ID_NONE)
  {
    /* lock cmgrDb */
    osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
    rc = cmgrCMDBCardConfigCardIdSet(unit, slot, cardId);

    /* unlock cmgrDb */
    osapiSemaGive(cmgrSemaId);

    if (rc != L7_SUCCESS)
      return L7_FAILURE;

    cmgrAsyncCardCreateNotify(unit, slot, cardId);
  }

  else if (cmgrCMDBConfigCardIdGet(unit, slot) != cardId)
  {
    /* cardType is different from the one currently
       configured for that slot */
    return L7_FAILURE;
  }

  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrSlotCardConfigSet: unit:%d slot:%d cardId:0x%x configured\n",
      unit, slot, cardId);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Clear configured card information from a slot
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit and slot does not exist
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This clears the configured card information from a slot.
 *        If a card is physically located in the slot, it will use
 *        the default card values.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotCardConfigClear(L7_uint32 unit,
    L7_uint32 slot)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_CARD_STATUS_TYPES_t cardStatus = L7_CARD_STATUS_UNKNOWN;
  L7_uint32  card_id, sx;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t unit_entry;

  /* NOTE: Following check is performed to prevent ports from
   *       being deleted when slot configuration is cleared.
   *
   * If cards are non-pluggable, the slot is automatically
   * configured when a unit is preconfigured. Also, the ports
   * are created for the slot supported card. So, clearing slot
   * configuration would result in deletion of ports for the
   * slot supported card, which is not desired.
   *
   * If cards are not pluggable, clearing the slot configuration
   * and deletion of ports for the slot supported card should be
   * done when unit is deleted.
   */
  memset(&unit_entry, 0, sizeof(SYSAPI_HPC_UNIT_DESCRIPTOR_t));
  if (cmgrUnitEntryAndSlotIndexGet(unit, slot, &sx, &unit_entry) != L7_SUCCESS)
    return L7_ERROR;

  if (unit_entry.physSlot[sx].pluggable != L7_TRUE)
    return L7_NOT_SUPPORTED;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

  card_id = cmgrCMDBConfigCardIdGet(unit, slot);

  cmgrAsyncCardUnPlugNotify(unit, slot, cmgrCMDBInsertedCardIdGet(unit,slot));
  rc = cmgrCMDBCardStatusSet(unit, slot, L7_CARD_STATUS_UNKNOWN);

  if (rc == L7_SUCCESS)
    rc = cmgrCMDBSlotConfiguredAdminModeSet(unit, slot, L7_ENABLE);
  if (rc == L7_SUCCESS)
    rc = cmgrCMDBSlotConfiguredPowerModeSet(unit, slot, L7_ENABLE);
  if (rc == L7_SUCCESS)
    rc = cmgrCMDBCardStatusSet(unit, slot, L7_CARD_STATUS_UNKNOWN);
  if (rc == L7_SUCCESS)
    rc = cmgrCMDBCardConfigCardIdSet(unit, slot, L7_CMGR_CARD_ID_NONE);
  if (rc == L7_SUCCESS)
    rc = cmgrCMDBCardTypeSet(unit, slot, SYSAPI_CARD_TYPE_INVALID);
  if (rc == L7_SUCCESS)
    rc = cmgrCMDBModuleIndexSet(unit, slot, SYSAPI_CARD_INDEX_INVALID);
  if (rc != L7_SUCCESS)
  {
    osapiSemaGive(cmgrSemaId);
    return rc;
  }

  cmgrAsyncCardClearNotify(unit, slot, card_id);

  if (cmgrCMDBSlotIsFull(unit, slot) == L7_TRUE)
  {
    rc = cmgrProcessCardPlugIn(unit,slot, cmgrCMDBInsertedCardIdGet(unit,slot), 
	cardStatus, SYSAPI_CARD_TYPE_LINE, 
	SYSAPI_CARD_INDEX_INVALID);
  }
  else
  {
    rc = cmgrCMDBCardConfigCardIdSet(unit, slot, L7_CMGR_CARD_ID_NONE);
  }

  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrSlotCardConfigClear: unit:%d slot:%d config cleared\n", unit, slot);


  return rc;
}


/*********************************************************************
 *
 * @purpose Get slot administrative mode
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32   *adminMode  Configured Administrative mode
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or is not
 *                          configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the administrative mode of the slot.  If this
 *        slot is not configured, L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotConfiguredAdminModeGet(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 *adminMode)
{
  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  *adminMode = cmgrCMDBConfiguredAdminModeGet(unit,slot);
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrSlotConfiguredAdminModeGet: unit:%d slot:%d admin mode:%d\n", 
      unit,cmgrCMDBPhySlotIdGet(unit, slot), *adminMode);

  return L7_SUCCESS;
}


/*********************************************************************
 *
 * @purpose Set slot administrative mode
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32    adminMode  Configured Administrative mode
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This sets the administrative mode of the slot.
 * @notes If the "pluggable" flag in supported card database is FALSE
 *        then the card is always enabled and cannot be disabled.
 * @notes Default administrative state of a card is "enabled".
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotAdminModeSet(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 adminMode)
{
  L7_uint32 card_id_ins, mode, port = 0;
  L7_uint32 sx;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t unit_entry;
  SYSAPI_CARD_TYPE_t cardType;
  SYSAPI_CARD_INDEX_t cardIndex;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardTypeDb;
  L7_uint32 phySlot, mySlot, pluggable;
  L7_RC_t rc = L7_SUCCESS;

  if ((unit > CMGR_MAX_UNITS_f) || (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
  {
    LOG_MSG("cmgrSlotAdminModeSet: unit %d slot %d out of range\n", unit, slot);
    return L7_ERROR;
  }

  if ((adminMode != L7_ENABLE) && (adminMode != L7_DISABLE))
  {
    LOG_MSG("cmgrSlotAdminModeSet: adminMode %d invalid mode\n", adminMode);
    return L7_ERROR;
  }

  memset(&unit_entry, 0, sizeof(SYSAPI_HPC_UNIT_DESCRIPTOR_t));

  sysapiHpcCurrentSlotGet(&mySlot);
  sysapiHpcSlotMapMPModuleInfoGet(mySlot,&cardType, &cardIndex);
  phySlot = 0;
  if(cardType == SYSAPI_CARD_TYPE_CONTROL)
  {
    phySlot = cmgrCMDBPhySlotIdGet(unit, slot);
    if(L7_SUCCESS != (sysapiHpcSlotMapSlotPluggableGet(phySlot, &pluggable)))
    {
      LOG_MSG("cmgrSlotAdminModeSet: get pluggable status for phySlot %d - failed\n", phySlot);
      return L7_FAILURE;
    }
  }
  else
  {
    if (cmgrUnitEntryAndSlotIndexGet(unit, slot, &sx, &unit_entry) != L7_SUCCESS)
    {
      LOG_MSG("cmgrSlotAdminModeSet: get unit entry and slot index for unit %d slot %d - failed\n", unit, slot);
      return L7_ERROR;
    }
    pluggable = unit_entry.physSlot[sx].pluggable;
  }

  if(pluggable != L7_TRUE)
  {
    if (adminMode == L7_ENABLE)
      return L7_ALREADY_CONFIGURED;
    else
    {
      LOG_MSG("cmgrSlotAdminModeSet: pluggable cannot be set when admin mode is disable - not supported\n");
      return L7_NOT_SUPPORTED;
    }
  }

  if(L7_SUCCESS != (sysapiHpcSlotMapMPModuleInfoGet(phySlot, &cardType, &cardIndex)))
  {
    LOG_MSG("cmgrSlotAdminModeSet: module info get for phySlot %d - failed\n", phySlot);
    return L7_FAILURE;
  }

  if(cardType != SYSAPI_CARD_TYPE_LINE)
  {
    LOG_MSG("cmgrSlotAdminModeSet: cardType %d - not supported\n", cardType);
    return L7_NOT_SUPPORTED;
  }

  mode = cmgrCMDBConfiguredAdminModeGet(unit, slot);
  if (((adminMode == L7_ENABLE) && (mode == L7_ENABLE)) ||
      ((adminMode == L7_DISABLE) && (mode == L7_DISABLE)))
    return L7_ALREADY_CONFIGURED;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  rc = cmgrCMDBSlotConfiguredAdminModeSet(unit, slot, adminMode);

  cmgrCfgData.cfgParms.configCard[unit][slot].adminMode = adminMode;
  cmgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);

  card_id_ins = cmgrCMDBInsertedCardIdGet(unit, slot);
  if ((rc == L7_SUCCESS) && (card_id_ins != 0))
  {
    cardTypeDb = sysapiHpcCardDbEntryGet(card_id_ins);

    if (adminMode == L7_ENABLE)
    {
      /* NOTE: If card-enable is issued for a disabled card,
       *       issue port-attach events.
       *
       * Only send port change messages if in steady state.
       */
      if (cmgrState == L7_CNFGR_STATE_E)
      {
        CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_ATTACH, L7_TRUE, unit, slot);
	    for (port = 1; port <= cardTypeDb->numOfNiPorts; port++)
	      (void)cmgrDoNotifyPortChange(unit, slot, port, card_id_ins,
				                       L7_ATTACH, &cardTypeDb->portInfo[port-1]);
        CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_ATTACH, L7_FALSE, unit, slot);
      }
    }
    else if (adminMode == L7_DISABLE)
    {
      /* NOTE: If card-disable is issued for a card that is already plugged in,
       *       issue port-detach events for all ports.
       *
       * Only send port change messages if in steady state.
       */
      if (cmgrState == L7_CNFGR_STATE_E)
      {
	for (port = 1; port <= cardTypeDb->numOfNiPorts; port++)
	  (void)cmgrDoNotifyPortChange(unit, slot, port, card_id_ins,
				       L7_DETACH, &cardTypeDb->portInfo[port-1]);
      }
    }
  }

  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrSlotAdminModeSet: unit:%d slot:%d admin mode:%d\n", 
      unit, phySlot, adminMode);
  return rc;
}

#ifndef L7_CHASSIS
/*********************************************************************
 *
 * @purpose Get slot power mode
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32   *powerMode  Configured Power mode
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or is not
 *                          configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the power mode of the slot.  If this
 *        slot is not configured, L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPowerModeGet(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 *powerMode)
{
  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  *powerMode = cmgrCMDBConfiguredPowerModeGet(unit,slot);
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);

  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrSlotPowerModeGet: unit:%d slot:%d power mode:%d\n", 
      unit, slot, *powerMode);
  return L7_SUCCESS;
}
#else
/*********************************************************************
 *
 * @purpose Get Power status of given LM slot
 *
 * @param  L7_uint32    slotNum              (input)   Slot number
 *         hpcSlotPowerDatabase_t *slotPower (output)  Power Status
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Card Manager to get the 
 *        power status of the given LM slot
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPowerStatusGet (L7_uint32 unit, L7_uint32 slot, 
    POWER_STATUS_t *slotPower, POWER_CONTROL_t *pSource)
{
  L7_RC_t rc = L7_FAILURE;
  SYSAPI_CARD_TYPE_t moduleType;
  SYSAPI_CARD_INDEX_t moduleIndex, phySlot;

  if ((unit > CMGR_MAX_UNITS_f) || (slot > CMGR_MAX_SLOTS_f))
  {
    LOG_MSG("cmgrSlotPowerStatusGet: unit %d or slot %d - out of range\n", unit, slot);
    return L7_ERROR;
  }

  phySlot = cmgrCMDBPhySlotIdGet(unit,slot);
  if(L7_SUCCESS != 
      (sysapiHpcSlotMapMPModuleInfoGet(phySlot, &moduleType, &moduleIndex)))
  {
    LOG_MSG("cmgrSlotPowerStatusGet: module info get for phySlot %d - failed\n", phySlot);
    return L7_FAILURE;
  }

  if(moduleType == SYSAPI_CARD_TYPE_LINE)
  {
    rc = cmgrHpcPwrSlotPowerStatusGet (phySlot, slotPower, pSource);
  }

  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrSlotPowerStatusGet: unit:%d slot:%d slotPower:%d psource:%d\n", 
      unit, phySlot, *slotPower, *pSource);

  return rc;
}
#endif

/*********************************************************************
 *
 * @purpose Get inserted card status
 *
 * @param  L7_uint32     unit       Unit ID of the unit
 * @param  L7_uint32     slot       Slot ID of the slot port
 * @param  L7_uint32    *cardStatus  Inserted Card Status
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or no card is
 *                          currently plubbed into that slot
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the card status of the card that is physically
 *        plugged in to this slot.  If no card is plugged in this slot,
 *         L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCardStatusGet(L7_uint32 unit, L7_uint32 slot, L7_uint32 *cardStatus)
{
  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  *cardStatus = cmgrCMDBCardStatusGet(unit, slot);
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);

  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrCardStatusGet: unit:%d slot:%d card status:%s\n", 
      unit, slot, cardStatusStr[*cardStatus]);
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Get inserted card type
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32   *cardType  Inserted Card Type
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist or there is no
 *                          card currently plugged into that slot
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the card type of the card that is physically
 *        plugged in to this slot.  If no card is plugged in this slot,
 *         L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCardInsertedCardTypeGet(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 *cardType)
{
  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  *cardType = cmgrCMDBInsertedCardIdGet(unit, slot);
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);

  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrCardInsertedCardTypeGet: unit:%d phySlot:%d card type:0x%x\n", 
      unit, cmgrCMDBPhySlotIdGet(unit,slot), *cardType);
  return L7_SUCCESS;
}


/*********************************************************************
 *
 * @purpose Get the number of ports on this card
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32    *numPorts  Number of ports on this card
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist or no card is
 *                          configured in that slot
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the number of ports on the card that is in
 *        this slot.  If no card is configured in this slot,
 *         L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCardNumPortsGet(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 *numPorts)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t* cardTypeDb;
  L7_uint32 cardId;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  cardId = cmgrCMDBConfigCardIdGet(unit, slot);
  cardTypeDb = sysapiHpcCardDbEntryGet(cardId);
  if (cardTypeDb == L7_NULLPTR)
    return L7_ERROR;

  *numPorts = cardTypeDb->numOfNiPorts;

  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrCardNumPortsGet: unit:%d slot:%d numPorts:%d\n", 
      unit, slot, *numPorts);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Get configured card type
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32    *cardType  Configured Card Type
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist or no card is
 *                          configured in that slot
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the card type of the card that is configured
 *         in to this slot.  If no card is configured in this slot,
 *         L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCardConfiguredCardTypeGet(L7_uint32 unit, L7_uint32 slot, L7_uint32 *cardType)
{
  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  *cardType = cmgrCMDBConfigCardIdGet(unit, slot);
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);

  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrCardConfiguredCardTypeGet: unit:%d phySlot:%d cardType:0x%x\n", 
      unit, cmgrCMDBPhySlotIdGet(unit,slot), *cardType);
  return L7_SUCCESS;
}

#ifndef L7_CHASSIS
/*********************************************************************
 *
 * @purpose Set slot power mode
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32    powerMode  Power mode to set
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or is not
 *                          configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This configures the power mode of the slot.  If this
 *        slot is not configured, L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPowerModeSet(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 powerMode)
{
  L7_uint32 sx, mode;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t unit_entry;
  L7_uint32 rc = L7_SUCCESS;

  if ((unit > CMGR_MAX_UNITS_f) || (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  if ((powerMode != L7_ENABLE) && (powerMode != L7_DISABLE))
    return L7_ERROR;

  memset(&unit_entry, 0, sizeof(SYSAPI_HPC_UNIT_DESCRIPTOR_t));
  if (cmgrUnitEntryAndSlotIndexGet(unit, slot, &sx, &unit_entry) != L7_SUCCESS)
    return L7_ERROR;

  if (unit_entry.physSlot[sx].power_down != L7_TRUE)
  {
    if (powerMode == L7_ENABLE)
      return L7_ALREADY_CONFIGURED;
    else
      return L7_NOT_SUPPORTED;
  }

  mode = cmgrCMDBConfiguredPowerModeGet(unit, slot);
  if (((powerMode == L7_ENABLE) && (mode == L7_ENABLE)) ||
      ((powerMode == L7_DISABLE) && (mode == L7_DISABLE)))
    return L7_ALREADY_CONFIGURED;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  rc = cmgrCMDBSlotConfiguredPowerModeSet(unit, slot, powerMode);

  cmgrCfgData.cfgParms.configCard[unit][slot].powerMode = powerMode;
  cmgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrSlotPowerModeSet: unit:%d slot:%d powerMode:%d\n", 
      unit, slot, powerMode);

  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  return rc;
}
#else
/*********************************************************************
 *
 * @purpose Set slot power mode
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_uint32    powerMode  Power mode to set
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or is not
 *                          configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This configures the power mode of the slot.  If this
 *        slot is not configured, L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPowerStatusSet(L7_uint32 unit, L7_uint32 slot,
    POWER_STATUS_t powerStatus, POWER_CONTROL_t source)
{
  L7_uint32 powerDown;
  L7_uint32 phySlot;

  phySlot = cmgrCMDBPhySlotIdGet(unit, slot);
  if(L7_SUCCESS != (sysapiHpcSlotMapSlotPowerdownGet(phySlot, &powerDown)))
  {
    LOG_MSG("cmgrSlotPowerStatusSet: current power status get for phySlot %d - failed\n", phySlot);
    return L7_ERROR;
  }

  /* if power down capability is not enabled, the slots are always powered ON */
  if (powerDown != L7_TRUE)
  {
    if (powerStatus == POWER_ON)
      return L7_ALREADY_CONFIGURED;
    else
      return L7_NOT_SUPPORTED;
  }

  if((powerStatus != POWER_OFF) && (powerStatus != POWER_ON))
  {
    LOG_MSG("cmgrSlotPowerStatusSet: powerStatus %d is invalid\n", powerStatus);
    return L7_FAILURE;
  }

  if((source != POWER_ADMIN) && (source != POWER_SYSTEM))
  {
    LOG_MSG("cmgrSlotPowerStatusSet: source %d is invalid\n", source);
    return L7_FAILURE;
  }

  /* invalid unit */
  if (unit > CMGR_MAX_UNITS_f)
  {
    LOG_MSG("cmgrSlotPowerStatusSet: unit %d is invalid\n", unit);
    return L7_ERROR;
  }
  /* invalid slot number for any unit */
  else if (phySlot > CMGR_MAX_SLOTS_f)
  {
    LOG_MSG("cmgrSlotPowerStatusSet: phySlot %d is invalid\n", phySlot);
    return L7_ERROR;
  }

  /* case of an empty / occupied slot in chassis */
  if(phySlot >= 0) 
  {
    return cmgrSlotPowerSet(unit, phySlot, powerStatus, source);
  }

  CMGR_DEBUG(CMGR_LOG_FLAG, 
      "cmgrSlotPowerStatusSet: unit:%d phySlot:%d powerStatus:%d source:%d Successful\n", 
      unit, phySlot, powerStatus, source);

  return L7_SUCCESS;
}
#endif
/*********************************************************************
 *
 * @purpose Retrieve slot full/empty status
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot port
 * @param  L7_BOOL     *isFull     True if slot is full; false otherwise
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that slot does not exist, or is not
 *                          configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns true if the slot is full.  Returns false if the
 *        slot is empty.  If this slot is not configured, L7_NOT_EXIST
 *        will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotIsFullGet(L7_uint32 unit, L7_uint32 slot,
    L7_BOOL *isFull)
{
  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  *isFull = cmgrCMDBSlotIsFull(unit, slot);
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrSlotIsFullGet: unit: %d phySlot:%d isFull:%d\n", 
      unit,cmgrCMDBPhySlotIdGet(unit, slot) , *isFull);

  return L7_SUCCESS;
}
/*********************************************************************
 *
 * @purpose Retrieve first slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    *slot      Slot ID of the first slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the first slot in this unit.  If this unit is
 *        not configured, L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotFirstGet(L7_uint32 unit, L7_uint32 *slotId)
{
  L7_uint32 rc = L7_SUCCESS;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  rc = cmgrCMDBSlotNextGet(unit, 0, slotId);
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrSlotFirstGet: unit: %d slot:%d\n", 
      unit, *slotId);
  return rc;
}

/*********************************************************************
 *
 * @purpose Retrieve the next slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot
 * @param  L7_uint32    *nextSlot      Slot ID of the next slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the slot following the input slot in this unit.
 *        If this unit is not configured, L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotNextGet(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 *nextSlot)

{
  L7_RC_t rc = L7_SUCCESS;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  rc = cmgrCMDBSlotNextGet(unit, slot, nextSlot);
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrSlotNextGet: unit: %d slot:%d nextSlot:%d\n", 
      unit, slot, *nextSlot);
  return rc;
}

/*********************************************************************
 *
 * @purpose Retrieve the slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot      Slot ID of the  slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit and slot does not exist,
 *                          or is not configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns L7_SUCCESS if the slot exists in this unit.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotGet(L7_uint32 unit, L7_uint32 slot)
{
  L7_uint32 rc = L7_SUCCESS;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    rc = L7_ERROR;

  else if (cmgrCMDBSlotIdGet(unit, slot) != slot)
    rc = L7_ERROR;
  else
    rc = L7_SUCCESS;
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrSlotGet: unit: %d slot:%d\n", unit, slot);
  return rc;
}


/*********************************************************************
 *
 * @purpose Retrieve first ocnfigured slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    *slot      Slot ID of the first slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the first configured slot in this unit.  If this unit is
 *        not configured, L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotFirstConfigGet(L7_uint32 unit, L7_uint32 *slotId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 slot = 0;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  do
  {
    rc = cmgrCMDBSlotNextGet(unit, slot, &slot);
    if (rc != L7_SUCCESS)
    {
      /* unlock cmgrDb */
      osapiSemaGive(cmgrSemaId);
      return rc;
    }
  } while (cmgrCMDBConfigCardIdGet(unit, slot) == L7_CMGR_CARD_ID_NONE);

  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  *slotId = slot;

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrSlotFirstConfigGet: unit: %d slot:%d\n", 
      unit, *slotId);

  return rc;
}

/*********************************************************************
 *
 * @purpose Retrieve the next configured slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot
 * @param  L7_uint32    *nextSlot      Slot ID of the next slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns configured the slot following the input slot
 *        in this unit.
 *        If this unit is not configured, L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotConfigNextGet(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 *nextSlot)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tempSlot = slot;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  do
  {
    rc = cmgrCMDBSlotNextGet(unit, tempSlot, &tempSlot);
    if (rc != L7_SUCCESS)
    {
      /* unlock cmgrDb */
      osapiSemaGive(cmgrSemaId);
      return rc;
    }
  } while (cmgrCMDBConfigCardIdGet(unit, tempSlot) ==
      L7_CMGR_CARD_ID_NONE);

  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrSlotConfigNextGet: unit: %d slot:%d nextSlot:%d\n", 
      unit, slot, *nextSlot);
  *nextSlot = tempSlot;
  return rc;
}

/*********************************************************************
 *
 * @purpose Retrieve the configured slot for a unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot      Slot ID of the  slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit and slot does not exist,
 *                          or is not configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns L7_SUCCESS if the slot exists in this unit.
 *        If this unit/slot combination is not configured,
 *        L7_FAILURE will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotConfigGet(L7_uint32 unit, L7_uint32 slot)
{
  L7_uint32 rc = L7_SUCCESS;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  if (cmgrCMDBSlotIdGet(unit, slot) != slot)
    rc = L7_FAILURE;

  if (cmgrCMDBConfigCardIdGet(unit, slot) == L7_CMGR_CARD_ID_NONE)
    rc = L7_FAILURE;

  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  return rc;
}


/*********************************************************************
 *
 * @purpose Retrieve first physically inserted slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    *slot      Slot ID of the first slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the first slot in this unit.  If this unit is
 *        not configured, L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotFirstPhysicalGet(L7_uint32 unit, L7_uint32 *slotId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 slot = 0;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  do
  {
    rc = cmgrCMDBSlotNextGet(unit, slot, &slot);
    if (rc != L7_SUCCESS)
    {
      /* unlock cmgrDb */
      osapiSemaGive(cmgrSemaId);
      return rc;
    }
  } while (cmgrCMDBSlotIsFull(unit, slot) == L7_FALSE);

  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  *slotId = slot;

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrSlotFirstPhysicalGet: unit: %d slot:%d\n", 
      unit, *slotId);

  return rc;
}

/*********************************************************************
 *
 * @purpose Retrieve the next slot in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot       Slot ID of the slot
 * @param  L7_uint32    *nextSlot      Slot ID of the next slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit does not exist, has no slots,
 *                          or is not configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns the slot following the input slot in this unit.
 *        If this unit is not configured, L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPhysicalNextGet(L7_uint32 unit, L7_uint32 slot,
    L7_uint32 *nextSlot)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tempSlot = slot;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  do
  {
    rc = cmgrCMDBSlotNextGet(unit, tempSlot, &tempSlot);
    if (rc != L7_SUCCESS)
    {
      /* unlock cmgrDb */
      osapiSemaGive(cmgrSemaId);
      return rc;
    }
  } while (cmgrCMDBSlotIsFull(unit, tempSlot) == L7_FALSE);

  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  *nextSlot = tempSlot;

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrSlotFirstPhysicalGet: unit: %d slot:%d nextSlot:%d\n", 
      unit, slot, *nextSlot);

  return rc;
}

/*********************************************************************
 *
 * @purpose Check if the slot exist in the unit
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    slot      Slot ID of the  slot
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_NOT_EXIST,   if that unit and slot does not exist,
 *                          or is not configured
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This returns L7_SUCCESS if the slot exists in this unit.
 *        If this unit/slot combination is not configured,
 *        L7_NOT_EXIST will be returned.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPhysicalGet(L7_uint32 unit, L7_uint32 slot)
{
  L7_uint32 rc = L7_SUCCESS;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

  if (cmgrCMDBSlotIsFull(unit, slot) == L7_FALSE)
    rc = L7_FAILURE;

  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  return rc;
}



/*****************************************************************************/
/* Unit Manager APIs                                                         */
/*****************************************************************************/

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that a new unit has been configured
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    unitType   Unit Type for the unit
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Unit Manager to notify the
 *        Card Manager that a unit has been configured.  This causes
 *        the unit and slots on the unit to be initialized.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrUnitConfig (L7_uint32 unit, L7_uint32 unitType)
{
  L7_uint32 maxSlots = 0;

  maxSlots = CMGR_MAX_SLOTS_f;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

  if (cmgrCMDBUnitConfiguredGet (unit) == L7_TRUE)
  {
    /* Unit is already configured. Can't do it again.
     */
    osapiSemaGive(cmgrSemaId);
    return L7_FAILURE;
  }

  (void) cmgrCMDBUnitMaxSlotsSet(unit, maxSlots);
  (void) cmgrCMDBUnitConnectedSet(unit, L7_FALSE);
  (void) cmgrCMDBUnitConfiguredSet(unit, L7_TRUE);
  (void) cmgrCMDBUnitTypeSet (unit, unitType);

  /* set slots to default values*/
  cmgrInitSlotsOnUnit(unit);

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrUnitConfig: unit: %d unitType:0x%x\n", 
      unit, unitType);

  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that a unit has been un-configured
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Unit Manager to notify the
 *        Card Manager that a unit has been unconfigured.  This causes
 *        the unit and slots on the unit to be deleted.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrUnitUnconfig (L7_uint32 unit)
{
  L7_uint32 i, slot, supported_card=0, sx;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t unit_entry;
  L7_uint32 rc = L7_SUCCESS;

  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

  for (slot = 0; slot <= cmgrCMDBUnitMaxSlotsGet(unit); slot++)
  {
    memset(&unit_entry, 0, sizeof(SYSAPI_HPC_UNIT_DESCRIPTOR_t));
    if (cmgrUnitEntryAndSlotIndexGet(unit, slot, &sx, &unit_entry) != L7_SUCCESS)
    {
      osapiSemaGive(cmgrSemaId);
      continue;
    }

    if (unit_entry.physSlot[sx].pluggable != L7_TRUE)
    {
      for (i = 0; i < unit_entry.physSlot[sx].num_supported_card_types; i++)
      {
	if (unit_entry.physSlot[sx].supported_cards[i] != L7_CMGR_CARD_ID_NONE)
	{
	  supported_card = unit_entry.physSlot[sx].supported_cards[i];
	  break; /* as only one cardtype is supported on a non-pluggble slot */
	}
      }

      rc = cmgrCMDBCardStatusSet(unit, slot, L7_CARD_STATUS_UNKNOWN);
      if (rc == L7_SUCCESS)
	rc = cmgrCMDBSlotConfiguredAdminModeSet(unit, slot, L7_ENABLE);
      if (rc == L7_SUCCESS)
	rc = cmgrCMDBSlotConfiguredPowerModeSet(unit, slot, L7_ENABLE);
      if (rc == L7_SUCCESS)
	rc = cmgrCMDBCardStatusSet(unit, slot, L7_CARD_STATUS_UNKNOWN);
      if (rc == L7_SUCCESS)
	rc = cmgrCMDBCardConfigCardIdSet(unit, slot, L7_CMGR_CARD_ID_NONE);
      if (rc != L7_SUCCESS)
      {
	osapiSemaGive(cmgrSemaId);
	return rc;
      }

      cmgrAsyncCardClearNotify(unit, slot, supported_card);
    }
  }

  rc = cmgrCMDBUnitMaxSlotsSet(unit, 0);
  rc = cmgrCMDBUnitConnectedSet(unit, L7_FALSE);
  rc = cmgrCMDBUnitConfiguredSet(unit, L7_FALSE);
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrUnitUnconfig: unit: %d\n", unit);
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that a unit has joined the stack.
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 * @param  L7_uint32    unitType   Unit Type of the unit
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Unit Manager to notify the
 *        Card Manager that a unit joined the stack.  This could happen
 *        if the communication link is established.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrUnitConnected (L7_uint32 unit)
{
  L7_uint32 rc;

  CMGR_TRACE_CARD_EVENT(L7_CMGR_UNIT_CONNECTED, L7_TRUE, unit, 0);
  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

  if (cmgrCMDBUnitConfiguredGet(unit) ==L7_FALSE)
  {
    /* unit must be configured before connected */
    osapiSemaGive(cmgrSemaId);
    return L7_ERROR;
  }
  rc = cmgrCMDBUnitConnectedSet(unit, L7_TRUE);
  osapiSemaGive(cmgrSemaId);

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrUnitConnected: unit: %d\n", unit);

  /* QSCAN_KMM: if we get a unit connected, We will set a timer and
     if it times out, then we will send a CMPDU request
     to get all slot information. */
  CMGR_TRACE_CARD_EVENT(L7_CMGR_UNIT_CONNECTED, L7_FALSE, unit, 0);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that a unit has left the stack.
 *
 * @param  L7_uint32    unit       Unit ID of the unit
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Unit Manager to notify the
 *        Card Manager that a unit left the stack.  This could happen
 *        if the communication link is lost.  This causes the Card
 *        Manager to set all cards on the unit to be disabled.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrUnitDisconnected (L7_uint32 unit)
{
  L7_uint32 rc;
  L7_uint32 maxSlots = 0;
  L7_uint32 slot = 0;

  CMGR_TRACE_CARD_EVENT(L7_CMGR_UNIT_DISCONNECTED, L7_TRUE, unit, 0);
  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
  rc = cmgrCMDBUnitConnectedSet(unit, L7_FALSE);

  /* for each card on unit, send "card unplug" event */
  maxSlots = cmgrCMDBUnitMaxSlotsGet(unit);

  for (slot= 0; slot < maxSlots; slot++)
  {
    cmgrAsyncCardUnPlugNotify(unit,slot, cmgrCMDBInsertedCardIdGet(unit,slot));
    cmgrProcessCardUnplug(unit, slot, L7_CARD_STATUS_UNPLUG, 
	cmgrCMDBCardTypeGet(unit,slot), 
	cmgrCMDBModuleIndexGet(unit,slot));
  }
  /* unlock cmgrDb */
  osapiSemaGive(cmgrSemaId);
  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrUnitDisconnected: unit: %d\n", unit);
  CMGR_TRACE_CARD_EVENT(L7_CMGR_UNIT_DISCONNECTED, L7_FALSE, unit, 0);
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Retrieve the MAC address for an interface
 *
 * @param  type        @b{(input)}     interface type of the interface
 * @param  unit        @b{(input)}     unit id of the interface
 * @param  slot        @b{(input)}     slot id of the interface
 * @param  port        @b{(input)}     port id of the interface
 * @param  mac_addr    @b{(output)}    The MAC address of the interface
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 unit, L7_uint32 slot, L7_uint32 port, L7_uchar8 *mac_addr)
{
  L7_RC_t rc = L7_ERROR;
  L7_uint32   thisUnit;
  SYSAPI_MAC_POLICY_t mac_allocation_policy;

  mac_allocation_policy = sysapiHpcMacAllocationPolicyGet();

  switch (mac_allocation_policy)
  {
    case SYSAPI_MAC_POLICY_MGMT_UNIT_POOL:
      /* if using this policy, local HPC will determine MAC address based on local unit's pool and the interface type */
      rc = sysapiHpcIfaceMacGet(type,slot,port,mac_addr);
      break;
    case SYSAPI_MAC_POLICY_REMOTE_UNIT_POOL:
      /*
       * Get the local unit number from unitMgr
       * If this is not a stacking build, unitMgr will still return the local unit
       * number
       */

      if ((rc = unitMgrNumberGet(&thisUnit)) != L7_SUCCESS)
      {
	memset((void *)mac_addr,0,sizeof(L7_uchar8)*6);
      }
      else
      {
	if (unit == thisUnit || unit == 0)
	{
	  /* local unit, call the HPC directly */
	  rc = sysapiHpcIfaceMacGet(type,slot,port,mac_addr);
	}
	else
	{
	  /*
	   * not local, let unit mgr figure it out
	   * Should never get here if we are not a Stacking build
	   */
	  rc = unitMgrIfaceMacGet(type, unit, slot, port, mac_addr);
	}
      }
      break;
    default:
      LOG_MSG("Unknown value returned from sysapiHpcMacAllocationPolicyGet() in cmgrIfaceMacGet().\n");
  }


  return(rc);
}

/*********************************************************************
 *
 * @purpose Unit manager notifies CM that this unit is a management
 *              unit.
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This notification is given to CM just before the configurator
 *        is told to start phase 3. This gives CM the apportunity to
 *        create logical cards.
 *
 * @end
 *
 *********************************************************************/
void cmgrLocalUnitIsManager (void)
{
  cmgrAsyncTaskLocalUnitIsManager();
}
/*********************************************************************
 *
 * @purpose Unit manager notifies CM that this unit is no longer
 *          a management unit.
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This notification is given to CM just as the system transitions
 *        into the "wait for management unit" state. This gives CM the
 *        opportunity to delete logical cards from the driver.
 *
 * @end
 *
 *********************************************************************/
void cmgrLocalUnitIsNotManager (void)
{
  cmgrAsyncTaskLocalUnitIsNotManager ();
}
