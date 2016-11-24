/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename   cardmgr_cnfgr.c
 *
 * @purpose    The functions that interface to the CNFGR component
 *
 * @component  cardmgr
 *
 * @comments   none
 *
 * @create     06/19/2003
 *
 * @author     avasquez
 * @end
 *
 **********************************************************************/
#include "cardmgr_include.h"
#include "cfg_change_api.h"

extern L7_cardMgrCfgFileData_t cmgrCfgData;

void *cmgrSemaId;  /* cmgr semaphore */
void *cmgrCardInsertRemoveSemaId;
void *cmgrTimerSyncSemaId;
void *cmgrQueue;  /* cmgr queue */
void *cmgrCardInsertRemoveQueue;  /* cmgr queue */
/* current phase of cmgr initialization */
extern L7_cmgr_state_t cmgrState;
extern L7_BOOL boardIdValidate_g;

/* card manager event Handle */
L7_EVENT_HANDLE_t cmgrEventHandle_g;
L7_EVENT_DATA_t cmgrEventInfo_g;

/*********************************************************************
 *
 * @purpose  Receive a Configurator event
 *
 * @param    cmgr_cmpdu_t* cmpdu  Card Manager Protocol packet
 *
 * @returns  None.
 *
 * @notes    This API is called when the Configurator makes a
 *           request to the Card Manager.
 *
 * @end
 *********************************************************************/
void cmgrCnfgrCmdReceive( cmgr_cmpdu_t *cmpdu )
{

  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_CMD_DATA_t  *pCmdData;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;

  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  L7_RC_t               cmgrRC     = L7_ERROR;
  L7_CNFGR_ERR_RC_t     reason     = L7_CNFGR_ERR_RC_INVALID_PAIR;

  do
  {

    if (cmpdu == L7_NULLPTR)
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD;
      break;
    }

    pCmdData = (L7_CNFGR_CMD_DATA_t *) &cmpdu->cmpdu_data.cmdData;

    /* validate command */
    if ( pCmdData == L7_NULL )
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD;
      break;
    } /* endif validate command */

    /* validate command type */
    if (pCmdData->type != L7_CNFGR_RQST)
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
      break;
    } /* endif validate command type */

    /* get command and validate request */
    command    = pCmdData->command;
    request    = pCmdData->u.rqstData.request;
    correlator = pCmdData->correlator;

    if ( request <= L7_CNFGR_RQST_FIRST ||
	request >= L7_CNFGR_RQST_LAST )
    {
      reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      break;

    } /* endif validate request */

    /* validate command/event pair and process command */
    switch ( command )
    {
      case L7_CNFGR_CMD_INITIALIZE:
	switch ( request )
	{
	  case L7_CNFGR_RQST_I_PHASE1_START:
	    cmgrRC = cmgrCnfgrNoopProccess( &response, &reason );
	    break;

	  case L7_CNFGR_RQST_I_PHASE2_START:
	    cmgrRC = cmgrCnfgrInitPhase2Process( &response, &reason );
	    cmgrState = L7_CNFGR_STATE_P2;
	    break;

	  case L7_CNFGR_RQST_I_PHASE3_START:
	    cmgrRC = cmgrCnfgrInitPhase3Process( &response, &reason );
	    cmgrState = L7_CNFGR_STATE_P3;
	    break;

	  case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
	    cmgrRC = cmgrCnfgrNoopProccess( &response, &reason );
	    cmgrState = L7_CNFGR_STATE_WMU;
	    break;

	  default:
	    /* invalid command/request pair */
	    break;
	} /* endswitch initialize requests */
	break;

      case L7_CNFGR_CMD_EXECUTE:
	switch ( request )
	{
	  case L7_CNFGR_RQST_E_START:
	    cmgrState = L7_CNFGR_STATE_E;
	    cmgrRC = cmgrCnfgrExecuteProcess( &response, &reason );
	    break;

	  default:
	    /* invalid command/request pair */
	    break;
	} /* endswitch initialize requests */
	break;

      case L7_CNFGR_CMD_UNCONFIGURE:
	switch ( request )
	{
	  case L7_CNFGR_RQST_U_PHASE1_START:
	    cmgrState = L7_CNFGR_STATE_U1;
	    cmgrRC = cmgrCnfgrUncfgPhase1Proccess( &response, &reason );
	    break;

	  case L7_CNFGR_RQST_U_PHASE2_START:
	    cmgrState = L7_CNFGR_STATE_U2;
	    cmgrRC = cmgrCnfgrUncfgPhase2Proccess( &response, &reason );
	    break;

	  default:
	    /* invalid command/request pair */
	    break;
	} /* endswitch initialize requests */
	break;

      case L7_CNFGR_CMD_TERMINATE:
      case L7_CNFGR_CMD_SUSPEND:
      case L7_CNFGR_CMD_RESUME:
	cmgrRC = cmgrCnfgrNoopProccess( &response, &reason );
	break;

      default:
	reason = L7_CNFGR_ERR_RC_INVALID_CMD;
	break;
    } /* endswitch command/event pair */

  } while ( 0 );

  /* return value to caller -
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = cmgrRC;

  if (cmgrRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason   = reason;
  } /* endif error */

  cnfgrApiCallback(&cbData);

  return;
}

/*********************************************************************
 * @purpose  Phase 2 initialization function.
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t *pReason )

{

  L7_RC_t cmgrRC,
	  cmgrRC1 = L7_ERROR;
  nvStoreFunctionList_t  notifyFunctionList;

  cmgrEventInfo_g.componentId = L7_CARDMGR_COMPONENT_ID;
  cmgrEventInfo_g.notify = cmgrCardReportCallBack;

  do
  {
    /* Register with the Hardware Platform Control (HPC) */
    cmgrRC = sysapiHpcEventCallbackRegister(&cmgrEventHandle_g, &cmgrEventInfo_g);

    if (cmgrRC != L7_SUCCESS)
    {
      *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      LOG_MSG("cmgrCnfgrInitPhase2Process(): Could not register with HPC events callback \n");
      break;
    }
    cmgrRC = sysapiHpcReceiveCallbackRegister(cmgrReceiveCMPDUCallback,
	L7_CARDMGR_COMPONENT_ID);
    if (cmgrRC != L7_SUCCESS)
    {
      *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      LOG_MSG("cmgrCnfgrInitPhase2Process(): Could not register with HPC receive callback \n");
      break;
    }

    if(cnfgrIsChassis() == L7_TRUE)
    {
      boardIdValidate_g = L7_TRUE;
    }

    /* Register with nvStore */
    (void)memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
    notifyFunctionList.registrar_ID   = L7_CARDMGR_COMPONENT_ID;
    notifyFunctionList.notifySave     = cmgrSave;
    notifyFunctionList.hasDataChanged = cmgrHasDataChanged;
    notifyFunctionList.notifyConfigDump = cmgrConfigDump;
    notifyFunctionList.resetDataChanged = cmgrResetDataChanged;

    cmgrRC = nvStoreRegister(notifyFunctionList);
    if (cmgrRC != L7_SUCCESS)
    {
      *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      LOG_MSG("cmgrCnfgrInitPhase2Process(): Could not register with nvStore \n");
      break;
    }

    /* start card manager's timer */
    cmgrTimer(L7_NULL, L7_NULL);
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    cmgrRC1 = L7_SUCCESS;

  } while ( 0 );

  return cmgrRC1;
}

/*********************************************************************
 * @purpose  Phase 3 initialization function.
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    The Card Manager only reaches Phase 3 initialization
 *           when executing on the Management Unit.
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t *pReason )
{
  L7_uint32     maxUnits;
  L7_uint32     maxSlots;
  L7_RC_t       cmgrRC = L7_ERROR;
  L7_uint32     unit, slot;
  L7_RC_t       rc;
  L7_uint32   current_card_id, card_id;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardTypeDb;


  maxUnits = CMGR_MAX_UNITS_f;
  maxSlots = CMGR_MAX_SLOTS_f;

  /* At this point the local unit number is known.
   */
  rc = unitMgrNumberGet (&unit);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  /* For stackable devices the local unit information is stored in unit 0.
   ** Now that the real unit number is known we can copy the unit 0 information
   ** to the real unit number.
   **
   ** We can copy information on stand-alone unit too because the unit 0
   ** record is always up to date with the latest local unit information.
   */
  rc = cmgrCMDBUnitInfoCopy(0, unit);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (unit);
  }


  do
  {
    if(L7_TRUE != cnfgrIsChassis())
    {
      /* Need to handle case where card has already been physically detected.
       ** In that case, the card will already be created, but ports won't yet
       ** be created.
       */
      rc = sysapiCfgFileGet (L7_CARDMGR_COMPONENT_ID, CMGR_CFG_FILENAME,
	  (L7_char8 *)&cmgrCfgData, sizeof(L7_cardMgrCfgFileData_t),
	  &cmgrCfgData.checkSum, CMGR_CFG_VER_CURRENT,
	  cmgrBuildDefaultConfigData, cardMgrMigrateConfigData);

      /* starting with fresh config */
      cmgrCfgData.cfgHdr.dataChanged = L7_FALSE;

      for (unit = 0; unit <= maxUnits; unit++)
      {
	for (slot = 0; slot <= maxSlots; slot++)
	{
	  card_id = cmgrCfgData.cfgParms.configCard[unit][slot].card_id;
	  if (card_id == L7_CMGR_CARD_ID_NONE)
	  {
	    continue;  /* Slot is not configured */
	  }

	  cardTypeDb = sysapiHpcCardDbEntryGet(card_id);
	  if (cardTypeDb == L7_NULL)
	  {
	     LOG_ERROR (card_id); 
	  }

	  /* If this is a logical card, but the unit is not the logical unit then
	   ** skip this card.
	   */
	  if (((cardTypeDb->type == SYSAPI_CARD_TYPE_LOGICAL_CPU) ||
		(cardTypeDb->type == SYSAPI_CARD_TYPE_VLAN_ROUTER) ||
		(cardTypeDb->type == SYSAPI_CARD_TYPE_LOOPBACK) ||
		(cardTypeDb->type == SYSAPI_CARD_TYPE_TUNNEL) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_CAPWAP_TUNNEL) ||
		(cardTypeDb->type == SYSAPI_CARD_TYPE_LAG)) &&
	      (slot != L7_LOGICAL_UNIT))
	  {
	    continue;
	  }

	  /* If a card is already plugged into the slot and it is different from
	   ** the preconfigured card then change the configured card type to
	   ** the new type.
	   */
	  current_card_id = cmgrCMDBConfigCardIdGet (unit, slot);
	  (void) cmgrCMDBCardConfigCardIdSet (unit, slot, card_id);

	  if (current_card_id != L7_CMGR_CARD_ID_NONE)
	  {
	    if (current_card_id != card_id)
	    {
	      cmgrAsyncCardCreateNotify (unit, slot, card_id);
	    }
	    else
	    {
	      /* If card is already inserted then issue plugin
	       ** for the card and ports.
	       */
	      cmgrAsyncCardPluginNotify (unit,slot , card_id);
	    }

	  }
	  else
	  {
	    cmgrAsyncCardCreateNotify (unit, slot, card_id);
	  }


	}
      }
    } 
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    cmgrRC = L7_SUCCESS;
  } while ( 0 );

  return cmgrRC;
}

/*********************************************************************
 *
 * @purpose  Notify Card Manager that initialization is complete, so
 *           "card create", "port create" and "card plugin" can be sent.
 *
 * @param    L7_uint32 phase  Indicates the initialization phase to be
 *                            executed
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes    This API is provided to allow the Configurator to direct
 *           phased initialization for the Card
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCnfgrExecuteProcess( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t *pReason )
{
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 cardId;
  L7_uint32 maxUnits;
  L7_RC_t   cmgrRC = L7_SUCCESS;
  L7_int32  loopCount;
  SYSAPI_HPC_CARD_DESCRIPTOR_t* cardTypeDb;

  *pResponse = L7_CNFGR_CMD_COMPLETE;

  /*if stacking environment, use max units per stack as the maxUnits else it is 1*/
  if (cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
  {
    maxUnits = CMGR_MAX_UNITS_f;
  }
  else
  {
    (void) unitMgrNumberGet(&maxUnits);
  }

  cmgrAsyncCardConfigSlots();

  /* handle pre-configured cards */
  for (unit = 1; unit <= maxUnits; unit++)
  {
    for (slot = 0; slot <= cmgrCMDBUnitMaxSlotsGet(unit); slot++)
    {
      /*card is configured on that slot */
      if (cmgrSlotConfigGet(unit, slot) == L7_SUCCESS)
      {
	cardId = cmgrCMDBConfigCardIdGet(unit,slot);

	cmgrAsyncCardCreateNotify(unit, slot, cardId);
      }
    }
  }

  /* handle all inserted cards
   ** First the logical cards and
   ** second for the physical cards.
   ** (loopCount == 0) means logical  cards
   ** (loopCount == 1) means phyiscal cards
   */
  for (loopCount=0; loopCount < 2; loopCount++)
  {
    for (unit = 0; unit <= maxUnits; unit++)
    {
      for (slot = 0; slot <= cmgrCMDBUnitMaxSlotsGet(unit); slot ++)
      {
	if (cmgrSlotPhysicalGet(unit, slot) == L7_SUCCESS)
	{
	  cardId = cmgrCMDBInsertedCardIdGet(unit, slot);

	  cardTypeDb = sysapiHpcCardDbEntryGet(cardId);

	  if (cardTypeDb == L7_NULL)
	  {
	    continue;
	  }

	  if (((loopCount == 0) &&                            /* Plugin all logical cards in the system */
		(cardTypeDb->type != SYSAPI_CARD_TYPE_LINE))
	      ||
	      ((loopCount == 1) &&                            /* Plugin all Physical Cards not in unit 0 */
	       (cardTypeDb->type == SYSAPI_CARD_TYPE_LINE) && /* because card manager stores all local   */
	       (unit != 0)))                                  /* card info there                         */
	  {

	    /* lock cmgrDb */
	    osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

	    if ((authenticateCardType(unit, slot, cardId) != L7_SUCCESS) &&
		(cmgrCMDBCardStatusGet(unit, slot) != L7_CARD_STATUS_UNSUPPORTED))
	    {
	      cmgrCardUnsupported(unit, slot, cardId);
	      osapiSemaGive(cmgrSemaId);
	    }
	    else
	    {
	      if (cmgrCMDBConfigCardIdGet(unit, slot) == L7_CMGR_CARD_ID_NONE)
	      {
		cmgrCMDBCardConfigCardIdSet(unit,slot, cardId);
		cmgrAsyncCardCreateNotify(unit, slot, cardId);
	      }

	      if (cmgrCMDBConfigCardIdGet(unit, slot) !=
		  cmgrCMDBInsertedCardIdGet(unit, slot))
	      {
		cmgrCardMisMatch(unit, slot,
		    cmgrCMDBConfigCardIdGet(unit, slot),
		    cmgrCMDBInsertedCardIdGet(unit, slot));

		/* unlock cmgrDb */
		osapiSemaGive(cmgrSemaId);
	      }
	      else
	      {
		/* unlock cmgrDb before notifying of plugin */
		osapiSemaGive(cmgrSemaId);
	      }
	    }
	  }
	}
      }
    }
  }

  cmgrAsyncCardConfigPorts();

  for (loopCount=0; loopCount < 2; loopCount++)
  {
    for (unit = 0; unit <= maxUnits; unit++)
    {
      for (slot = 0; slot <= cmgrCMDBUnitMaxSlotsGet(unit); slot ++)
      {
	if (cmgrSlotPhysicalGet(unit, slot) == L7_SUCCESS)
	{
	  cardId = cmgrCMDBInsertedCardIdGet(unit, slot);

	  cardTypeDb = sysapiHpcCardDbEntryGet(cardId);

	  if (cardTypeDb == L7_NULL)
	  {
	    continue;
	  }

	  if (((loopCount == 0) &&                            /* Plugin all logical cards in the system */
		(cardTypeDb->type != SYSAPI_CARD_TYPE_LINE))
	      ||
	      ((loopCount == 1) &&                            /* Plugin all Physical Cards not in unit 0 */
	       (cardTypeDb->type == SYSAPI_CARD_TYPE_LINE) && /* because card manager stores all local   */
	       (unit != 0)))                                  /* card info there                         */
	  {

	    /* lock cmgrDb */
	    osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

	    if ((authenticateCardType(unit, slot, cardId) != L7_SUCCESS) &&
		(cmgrCMDBCardStatusGet(unit, slot) != L7_CARD_STATUS_UNSUPPORTED))
	    {
	    }
	    else
	    {
	      if (cmgrCMDBConfigCardIdGet(unit, slot) !=
		  cmgrCMDBInsertedCardIdGet(unit, slot))
	      {
		/* unlock cmgrDb */
		osapiSemaGive(cmgrSemaId);
	      }
	      else
	      {
		/* unlock cmgrDb before notifying of plugin */
		osapiSemaGive(cmgrSemaId);
		cmgrAsyncCardPluginNotify(unit, slot, cardId);
	      }
	    }
	  }
	}
      }
    }
  }

  return cmgrRC;
}

/*********************************************************************
 *
 * @purpose  Notify Card Manager that it should unconfigure itself.
 *
 * @param    L7_CNFGR_CMD_DATA_t *pCmdData
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes    This API is provided to allow the Configurator to direct
 *           the Card Manager to unconfigure itself.
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCnfgrUncfgPhase1Proccess( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t *pReason )
{
  L7_RC_t   cmgrRC = L7_SUCCESS;

  *pResponse = L7_CNFGR_CMD_COMPLETE;

  cmgrAllCardsUnplugUnconfigure ();

  return cmgrRC;
}

/*********************************************************************
 *
 * @purpose  Notify Card Manager that it should unconfigure itself.
 *
 * @param    L7_CNFGR_CMD_DATA_t *pCmdData
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes    This API is provided to allow the Configurator to direct
 *           the Card Manager to unconfigure itself.
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCnfgrUncfgPhase2Proccess( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t *pReason )
{
  L7_RC_t   cmgrRC = L7_SUCCESS;

  *pResponse = L7_CNFGR_CMD_COMPLETE;

  /* Wipe out card manager configuration. The configuration will
   ** be restored in phase 3.
   */
  memset (&cmgrCfgData, 0, sizeof(L7_cardMgrCfgFileData_t));

  return cmgrRC;
}

/*********************************************************************
 * @purpose  This function process the configurator control commands/request
 *           pair as a NOOP.
 *
 * @param    pResponse - @b{(output)}  Response always command complete.
 *
 * @param    pReason   - @b{(output)}  Always 0
 *
 * @returns  L7_SUCCESS - Always return this value. onse is available.
 *
 *
 * @notes    The following are valid response:
 *           L7_CNFGR_CMD_COMPLETE
 *
 * @notes    The following are valid error reason code:
 *           None.
 *
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t cmgrRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(cmgrRC);
}

/*********************************************************************
 * @purpose  Initialize and start Card Manager Task function.
 *
 * @param    L7_CNFGR_CMD_DATA_t *pCmdData
 *
 * @returns  L7_RC_t  Returns L7_SUCCESS or L7_ERROR
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrStartTask(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_int32 taskId;
  cmgrQueue = (void *)osapiMsgQueueCreate(L7_CMGR_QUEUE_NAME,
      L7_CMGR_MSGQ_COUNT_f,
      L7_CMGR_MSGQ_SIZE_f);
  if (cmgrQueue == L7_NULLPTR)
  {
    LOG_ERROR (0);
  }

  cmgrCardInsertRemoveQueue =
    (void *)osapiMsgQueueCreate(L7_CMGR_CARD_INSERT_REMOVE_QUEUE_NAME,
			 L7_CMGR_CARD_INSERT_REMOVE_QUEUE_MSG_COUNT  	,
				sizeof (cmgrCardInsertRemoveMsg_t));
  if (cmgrCardInsertRemoveQueue == L7_NULLPTR)
  {
    LOG_ERROR (0);
  }

  cmgrSemaId = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (cmgrSemaId == L7_NULLPTR)
  {
    LOG_ERROR (0);
  }

  cmgrCardInsertRemoveSemaId = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_EMPTY);
  if (cmgrCardInsertRemoveSemaId == L7_NULLPTR)
  {
    LOG_ERROR (0);
  }

  cmgrTimerSyncSemaId = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (cmgrTimerSyncSemaId == L7_NULLPTR)
  {
    LOG_ERROR (0);
  }

  cmgrNimSyncSemCreate ();

  taskId = osapiTaskCreate(L7_CMGR_TASK_NAME, cmgrTask, 0, 0,
                           L7_CMGR_TASK_STACK_SIZE_f,
                           L7_CMGR_TASK_PRIORITY_f,
                           L7_CMGR_TASK_SLICE_f);

  if (taskId == L7_ERROR)
  {
    LOG_ERROR (0);
  }

  if (osapiWaitForTaskInit (L7_CMGR_TASK_SYNC_f, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_ERROR (0);
  }

  /* Start Card Insertion/Removal task.
   */
  taskId = osapiTaskCreate(L7_CMGR_CARD_INSERT_REMOVE_TASK_NAME,
                           cmgrCardInsertRemoveTask, 0, 0,
                           L7_CMGR_TASK_STACK_SIZE_f,
                           L7_CMGR_TASK_PRIORITY_f,
                           L7_CMGR_TASK_SLICE_f);

  if (taskId == L7_ERROR)
  {
    LOG_ERROR (0);
  }

  cmgrTraceInit(CMGR_TRACE_ENTRY_MAX, CMGR_TRACE_ENTRY_SIZE_MAX);
  cmgrProfileInit();

  return L7_SUCCESS;
}

