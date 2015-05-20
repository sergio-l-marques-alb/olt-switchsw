/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename card_manager_api.c
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
#include "usmdb_unitmgr_api.h"
#include "usmdb_cda_api.h"
#include "cfg_change_api.h"

/* In case of code version mismatch, automatically download the correct
 * code to the reporting unit  */
L7_int32 auto_synchronize = L7_FALSE;
L7_int32 autoSyncSuccess  = L7_FALSE ;
/* nvstore Functions' Data Structure  */
extern nvStoreFunctionList_t  notifyFunctionList;
extern void *cmgrSemaId;  /* cmgr semaphore */
extern void *cmgrQueue;  /* cmgr queue */
extern L7_cmgr_state_t cmgrState; /* current phase of cmgr initialization */
extern L7_char8 cardTypeStr[(SYSAPI_CARD_TYPE_LAST+1)][30];
extern L7_char8 cardStatusStr[L7_CARD_STATUS_TOTAL][30];

/*********************************************************************
*
* @purpose  Receive a Card Plug in event
*
* @param    cmgr_cmpdu_t* cmpdu  Card Manager Protocol packet
*
* @returns
*
* @notes    This API is called when a card is plugged in to the
*           local unit.
*
* @end
*********************************************************************/
void cmgrReceiveCardPlugIn(cmgr_cmpdu_t *cmpdu)
{
  cmgr_cmpdu_statusUpdate_t *cdcmpdu;
  L7_RC_t rc;

  cdcmpdu = (cmgr_cmpdu_statusUpdate_t*) &cmpdu->cmpdu_data.statusUpdate;

  CMGR_DEBUG(CMGR_LOG_FLAG,"\ncmgrReceiveCardPlugIn: Entry unit:%d physlot:%d slotId:%d\n",
         cmpdu->unit, cdcmpdu->phySlotId, cdcmpdu->slotId);

  /* lock cmgrDb */
  (void)osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

  /* check for valid slot */
  if ((cdcmpdu->slotId >= 0) &&
      (cdcmpdu->slotId <= cmgrCMDBUnitMaxSlotsGet(cmpdu->unit)))
  {
    /* verify that the slot was empty */
    if (cmgrCMDBSlotIsFull(cmpdu->unit, cdcmpdu->slotId) == L7_FALSE)
    {
      if (cmgrState == L7_CNFGR_STATE_P3 ||
          cmgrState == L7_CNFGR_STATE_E)
      {
        cmgrCMDBPhySlotIdSet(cmpdu->unit, cdcmpdu->slotId, cdcmpdu->phySlotId);

        rc = cmgrProcessCardPlugIn(cmpdu->unit, cdcmpdu->slotId,
                                   cdcmpdu->insertedCardTypeId,
                                   cdcmpdu->cardStatus, cdcmpdu->cardType,
                                   cdcmpdu->cardIndex);

        /* Copy unit info into unit 0 record. Unit 0 always contains
        ** local card information.
        */
        if ( (cmpdu->unit != 0 ) && (cmpdu->local == L7_TRUE ))
        {
          rc = cmgrCMDBUnitInfoCopy (cmpdu->unit, 0);
          if (rc != L7_SUCCESS)
          {
            L7_LOG_ERROR (cmpdu->unit);
          }
        }

        (void)osapiSemaGive(cmgrSemaId);

        if (rc == L7_SUCCESS)
        {
          cmgrAsyncCardPluginNotify(cmpdu->unit, cdcmpdu->slotId,
                                                      cdcmpdu->insertedCardTypeId);
           CMGR_DEBUG(CMGR_LOG_FLAG,
             "\ncmgrReceiveCardPlugIn: PlugIn Notify Issued unit:%d physlot:%d slotId:%d\n",
             cmpdu->unit, cdcmpdu->phySlotId, cdcmpdu->slotId);
        }
      }
      else   /* Stack Unit */
      {
        CMGR_DEBUG(CMGR_LOG_FLAG,
          "\ncmgrReceiveCardPlugIn: stack plugin unit %d, slot %d, type 0x%x\n",
          cmpdu->unit, cdcmpdu->slotId, cdcmpdu->insertedCardTypeId);

      if (cmpdu->unit != 0 && cmgrCMDBUnitConfiguredGet(cmpdu->unit) == L7_FALSE)
      {
        /*
        Only unit 0 cards have been setup correctly.  Other units
        need to do everything here, or when the values are written
        back to the unit 0 record, there will be incorrect data
        present.  Depending on timing the same card can either be
        inserted with unit 0 or the local unit number.  Without
        this fix, the code only works when 0 is the unit number of
        the plugged-in "card" because of the copy back to unit 0

        Note that this only needs to be done when the unit is not
        yet configured
        */

        (void) cmgrCMDBUnitMaxSlotsSet(cmpdu->unit, CMGR_MAX_SLOTS_f);
        (void) cmgrCMDBUnitConfiguredSet(cmpdu->unit, L7_TRUE);
        (void) cmgrCMDBUnitConnectedSet(cmpdu->unit, L7_TRUE);
        (void) cmgrCMDBUnitTypeSet (cmpdu->unit, cdcmpdu->insertedCardTypeId);

        /*
        Set slots to default values
        */

        cmgrInitSlotsOnUnit(cmpdu->unit);
      }

        (void)cmgrCMDBPhySlotIdSet(cmpdu->unit, cdcmpdu->slotId, cdcmpdu->phySlotId);
        (void)cmgrCMDBSlotIsFullSet (cmpdu->unit, cdcmpdu->slotId, L7_TRUE);
        (void)cmgrCMDBCardInsertedCardIdSet(cmpdu->unit, cdcmpdu->slotId,
                                            cdcmpdu->insertedCardTypeId);
        (void)cmgrCMDBCardStatusSet(cmpdu->unit, cdcmpdu->slotId,
                                    cdcmpdu->cardStatus);
        (void)cmgrCMDBSlotConfiguredAdminModeSet(cmpdu->unit,
                                                 cdcmpdu->slotId, L7_ENABLE);
        (void)cmgrCMDBSlotConfiguredPowerModeSet(cmpdu->unit,
                                                 cdcmpdu->slotId, L7_ENABLE);
        (void)cmgrCMDBCardTypeSet(cmpdu->unit, cdcmpdu->slotId,
                                    cdcmpdu->cardType);
        (void)cmgrCMDBModuleIndexSet(cmpdu->unit, cdcmpdu->slotId,
                                    cdcmpdu->cardIndex);

        /* Copy unit info into unit 0 record. Unit 0 always contains
        ** local card information.
        */
        if ((cmpdu->unit != 0) && (cmpdu->local == L7_TRUE))
        {
          rc = cmgrCMDBUnitInfoCopy (cmpdu->unit, 0);
          if (rc != L7_SUCCESS)
          {
            L7_LOG_ERROR (cmpdu->unit);
          }
        }

        (void)osapiSemaGive(cmgrSemaId);
      }

      /* Send Status update only if completed phase 2*/
      if (cmgrState != L7_CNFGR_STATE_IDLE &&
          cmgrState != L7_CNFGR_STATE_P1)
      {
        cmgrSendStatusUpdate(L7_CMPDU_CARD_REPORT_CARD_PLUGIN);
      }
    }
    else
    {
      /* Find out if we have already process this card. If so, NOOP, else
       * log message
      */
      if (cdcmpdu->insertedCardTypeId != cmgrCMDBInsertedCardIdGet(cmpdu->unit,
                                                                   cdcmpdu->slotId))
      {
        /* Should not get here, Log message. */
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CARDMGR_COMPONENT_ID,
                "cmgrReceiveCardPlugIn(): Card mismatch \n");
      }

      /* unlock the cmgrDb */
      (void)osapiSemaGive(cmgrSemaId);
    }
  }
  else
  {
    /* unlock the cmgrDb */
    (void)osapiSemaGive(cmgrSemaId);
  }

  return;
}

/*********************************************************************
*
* @purpose  Process a Card Plug in event on the Management unit
*
* @param    L7_uint32 unit         unit
* @param    L7_uint32 slot         slot being plugged in
* @param    L7_uint32 cardType     Type of card plugged in
* @param    L7_uint32 cardStatus   Status of card plugged in
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    This API is used to update the Application of a card
*           plug in event.  It gets called if a local card was plugged
*           in or if a CMPDU status message from a remote unit indicates
*           that a card was plugged in on the remote unit.
*
* @end
*********************************************************************/
L7_RC_t cmgrProcessCardPlugIn(L7_uint32 unit, L7_uint32 slot,
                              L7_uint32 cardTypeId, L7_uint32 cardStatus,
                              SYSAPI_CARD_TYPE_t cardType,
                              SYSAPI_CARD_INDEX_t cardIndex)
{
  L7_RC_t rc;
  L7_uint32 phySlot;

  phySlot = cmgrCMDBPhySlotIdGet(unit, slot);

  /* update CMDB with card info */
  rc = cmgrCMDBCardInsertedCardIdSet(unit, slot, cardTypeId);

  (void) cmgrCMDBCardStatusSet(unit, slot, cardStatus);

  (void) cmgrCMDBSlotConfiguredAdminModeSet(unit, slot, L7_ENABLE);
  (void) cmgrCMDBSlotConfiguredPowerModeSet(unit, slot, L7_ENABLE);
  (void) cmgrCMDBCardTypeSet (unit, slot, cardType);
  (void) cmgrCMDBModuleIndexSet (unit, slot, cardIndex);

  rc = authenticateCardType(unit, slot, cardTypeId);
  if (rc != L7_SUCCESS)
  {
    cmgrCardUnsupported(unit, slot, cardTypeId);
    return rc;
  }

  cmgrCMDBSlotIsFullSet (unit, slot, L7_TRUE);

  /* check if pre-configured or not */
  if (cmgrCMDBConfigCardIdGet(unit,slot) == L7_CMGR_CARD_ID_NONE)
  {
    /* This card has not been preconfigured, so assume that
    ** the card is OK and try to make it usable.
    */
    (void) cmgrCMDBSlotConfiguredAdminModeSet(unit, slot, L7_ENABLE);
    (void) cmgrCMDBSlotConfiguredPowerModeSet(unit, slot, L7_ENABLE);


    cmgrCMDBCardConfigCardIdSet(unit, slot, cardTypeId);
    cmgrAsyncCardCreateNotify(unit, slot, cardTypeId);
  }

  if (cmgrCMDBConfigCardIdGet(unit, slot) !=
      cardTypeId)
  {
    cmgrCardMisMatch(unit, slot, cmgrCMDBConfigCardIdGet(unit, slot),
                     cardTypeId);
    return L7_ERROR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Request a Status Update CMPDU from a unit
*
* @param    L7_uint32 unit         unit
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    This API is used to send a request to a unit to resend
*           the Status Update Message.
*
* @end
*********************************************************************/
L7_RC_t cmgrSendRequestForStatusUpdate(L7_uint32 unit)
{
  cmgr_cmpdu_t cmpdu;

  cmpdu.componentId = L7_CARDMGR_COMPONENT_ID;
  cmpdu.cmgrPacketType = L7_CMPDU_REQUEST_ALL_INFO;
  (void)unitMgrNumberGet(&cmpdu.unit);
  cmpdu.cmpdu_data.requestAll.requestedUnit = unit;

  /* send cmpdu to HPC */
  return cmgrSendCmpdu(&cmpdu, sizeof(cmpdu));
}

/*********************************************************************
*
* @purpose  Receive a Card unplug event
*
* @param    cmgr_cmpdu_t* cmpdu  Card Manager Protocol packet
*
* @returns
*
* @notes    This API is called when a card is unplugged from the
*           local unit.
*
* @end
*********************************************************************/
void cmgrReceiveCardUnplug(cmgr_cmpdu_t *cmpdu)
{
  cmgr_cmpdu_statusUpdate_t *cdcmpdu;
  L7_uint32   cardId;
  L7_RC_t   rc;


  cdcmpdu = (cmgr_cmpdu_statusUpdate_t*) &cmpdu->cmpdu_data.statusUpdate;
  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

 if ((cdcmpdu->slotId < 0) ||
      (cdcmpdu->slotId > cmgrCMDBUnitMaxSlotsGet(cmpdu->unit)))
  {
    osapiSemaGive(cmgrSemaId);
    return;  /* invalid slot */
  }

  /* verify that the slot was full before */
  if (cmgrCMDBSlotIsFull(cmpdu->unit, cdcmpdu->slotId) != L7_TRUE)
  {
    osapiSemaGive(cmgrSemaId);
    return;   /* no new info */
  }

  if (cmgrState == L7_CNFGR_STATE_P3 ||
      cmgrState == L7_CNFGR_STATE_E)
  {
    cardId = cmgrCMDBInsertedCardIdGet (cmpdu->unit, cdcmpdu->slotId);

    cmgrProcessCardUnplug(cmpdu->unit, cdcmpdu->slotId, cdcmpdu->cardStatus,
                          cdcmpdu->cardType, cdcmpdu->cardIndex);

    /* Copy unit info into unit 0 record. Unit 0 always contains
    ** local card information.
    */
    if ( (cmpdu->unit != 0 ) && (cmpdu->local == L7_TRUE ))
    {
      rc = cmgrCMDBUnitInfoCopy (cmpdu->unit, 0);
      if (rc != L7_SUCCESS)
      {
        L7_LOG_ERROR (cmpdu->unit);
      }
    }

    osapiSemaGive(cmgrSemaId);

    cmgrAsyncCardUnPlugNotify(cmpdu->unit, cdcmpdu->slotId, cardId);
  }
  else   /* Stack Unit */
  {
    (void)cmgrCMDBSlotIsFullSet(cmpdu->unit, cdcmpdu->slotId, L7_FALSE);
    (void)cmgrCMDBCardInsertedCardIdSet(cmpdu->unit, cdcmpdu->slotId,
                                        L7_CMGR_CARD_ID_NONE);
    (void)cmgrCMDBCardStatusSet(cmpdu->unit, cdcmpdu->slotId,
                                L7_CARD_STATUS_UNKNOWN);
    (void)cmgrCMDBCardTypeSet(cmpdu->unit, cdcmpdu->slotId,
                                SYSAPI_CARD_TYPE_INVALID);
    (void)cmgrCMDBModuleIndexSet(cmpdu->unit, cdcmpdu->slotId,
                                SYSAPI_CARD_INDEX_INVALID);

    /* Copy unit info into unit 0 record. Unit 0 always contains
    ** local card information.
    */
    if ((cmpdu->local == L7_TRUE) && (cmpdu->unit != 0))
    {
      rc = cmgrCMDBUnitInfoCopy (cmpdu->unit, 0);
      if (rc != L7_SUCCESS)
      {
        L7_LOG_ERROR (cmpdu->unit);
      }
    }
    osapiSemaGive(cmgrSemaId);
  }

  /* Send Status update only if completed phase 2*/
  if (cmgrState != L7_CNFGR_STATE_IDLE &&
      cmgrState != L7_CNFGR_STATE_P1)
  {
    cmgrSendStatusUpdate(L7_CMPDU_CARD_REPORT_CARD_UNPLUG);
  }
  return;
}

/*********************************************************************
*
* @purpose  Process a Card unplug event on the Management unit
*
* @param    L7_uint32 unit         unit
* @param    L7_uint32 slot         slot being unplugged
* @param    L7_uint32 cardStatus   Status of card unplugged
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    This API is used to update the Application of a card
*           unplug event.  It gets called if a local card was unplugged
*           or if a CMPDU status message from a remote unit indicates
*           that a card was unplugged on the remote unit.
*
* @end
*********************************************************************/
L7_RC_t cmgrProcessCardUnplug(L7_uint32 unit, L7_uint32 slot, L7_uint32 cardStatus,
                              SYSAPI_CARD_TYPE_t cardType, SYSAPI_CARD_INDEX_t cardIndex)
{
  L7_uint32 rc;
  L7_uint32 phySlot;

  rc = cmgrCMDBSlotIsFullSet(unit, slot, L7_FALSE);
  rc = cmgrCMDBCardInsertedCardIdSet(unit, slot, L7_CMGR_CARD_ID_NONE);
  rc = cmgrCMDBCardStatusSet(unit, slot, cardStatus);

  phySlot = cmgrCMDBPhySlotIdGet(unit, slot);

  rc = cmgrCMDBCardTypeSet(unit, slot, SYSAPI_CARD_TYPE_INVALID);
  rc = cmgrCMDBModuleIndexSet(unit, slot, SYSAPI_CARD_INDEX_INVALID);

  return rc;
}

/*********************************************************************
*
* @purpose  Receive a Card failure event
*
* @param    cmgr_cmpdu_t* cmpdu  Card Manager Protocol packet
*
* @returns
*
* @notes    This API is called when a card is failed from the
*           local unit.
*
* @end
*********************************************************************/
void cmgrReceiveCardFailure(cmgr_cmpdu_t *cmpdu)
{
  cmgr_cmpdu_statusUpdate_t *cdcmpdu;
  L7_uint32   cardId;
  L7_RC_t   rc;


  cdcmpdu = (cmgr_cmpdu_statusUpdate_t*) &cmpdu->cmpdu_data.statusUpdate;
  /* lock cmgrDb */
  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

  /* verify that the slot was full before */
  if (cmgrCMDBSlotIsFull(cmpdu->unit, cdcmpdu->slotId) != L7_TRUE)
  {
    osapiSemaGive(cmgrSemaId);
    return;   /* no new info */
  }

  if (cmgrState == L7_CNFGR_STATE_P3 ||
      cmgrState == L7_CNFGR_STATE_E)
  {
    cardId = cmgrCMDBInsertedCardIdGet (cmpdu->unit, cdcmpdu->slotId);

    cmgrProcessCardFailure(cmpdu->unit, cdcmpdu->slotId, cdcmpdu->cardStatus,
                           cdcmpdu->cardType, cdcmpdu->cardIndex);

    /* Copy unit info into unit 0 record. Unit 0 always contains
    ** local card information.
    */

    if ( (cmpdu->unit != 0 ) && (cmpdu->local == L7_TRUE ))
    {
      rc = cmgrCMDBUnitInfoCopy (cmpdu->unit, 0);
      if (rc != L7_SUCCESS)
      {
        L7_LOG_ERROR (cmpdu->unit);
      }
    }

    osapiSemaGive(cmgrSemaId);
  }
  else   /* Stack Unit */
  {
    (void)cmgrCMDBSlotIsFullSet(cmpdu->unit, cdcmpdu->slotId, L7_FALSE);
    (void)cmgrCMDBCardInsertedCardIdSet(cmpdu->unit, cdcmpdu->slotId,
                                        L7_CMGR_CARD_ID_NONE);
    (void)cmgrCMDBCardStatusSet(cmpdu->unit, cdcmpdu->slotId,
                                L7_CARD_STATUS_FAILED);
    (void)cmgrCMDBCardTypeSet(cmpdu->unit, cdcmpdu->slotId,
                                SYSAPI_CARD_TYPE_INVALID);
    (void)cmgrCMDBModuleIndexSet(cmpdu->unit, cdcmpdu->slotId,
                                SYSAPI_CARD_INDEX_INVALID);

    /* Copy unit info into unit 0 record. Unit 0 always contains
    ** local card information.
    */
    if ((cmpdu->local == L7_TRUE) && (cmpdu->unit != 0))
    {
      rc = cmgrCMDBUnitInfoCopy (cmpdu->unit, 0);
      if (rc != L7_SUCCESS)
      {
        L7_LOG_ERROR (cmpdu->unit);
      }
    }
    osapiSemaGive(cmgrSemaId);
  }

  /* Send Status update only if completed phase 2*/
  if (cmgrState != L7_CNFGR_STATE_IDLE &&
      cmgrState != L7_CNFGR_STATE_P1)
  {
    cmgrSendStatusUpdate(L7_CMPDU_CARD_REPORT_CARD_FAILURE);
  }
  return;
}

/*********************************************************************
*
* @purpose  Process a Card unplug event on the Management unit
*
* @param    L7_uint32 unit         unit
* @param    L7_uint32 slot         slot being unplugged
* @param    L7_uint32 cardStatus   Status of card unplugged
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    This API is used to update the Application of a card
*           unplug event.  It gets called if a local card was unplugged
*           or if a CMPDU status message from a remote unit indicates
*           that a card was unplugged on the remote unit.
*
* @end
*********************************************************************/
L7_RC_t cmgrProcessCardFailure(L7_uint32 unit, L7_uint32 slot, L7_uint32 cardStatus,
                               SYSAPI_CARD_TYPE_t cardType, SYSAPI_CARD_INDEX_t cardIndex)
{
  (void) cmgrCMDBSlotIsFullSet(unit, slot, L7_FALSE);
  (void) cmgrCMDBCardInsertedCardIdSet(unit, slot, L7_CMGR_CARD_ID_NONE);
  (void) cmgrCMDBCardStatusSet(unit, slot, cardStatus);

  (void) cmgrCMDBCardTypeSet(unit, slot, cardType);
  (void) cmgrCMDBModuleIndexSet(unit, slot, cardIndex);

  (void) cmgrCMDBCardTypeSet(unit, slot, SYSAPI_CARD_TYPE_INVALID);
  (void) cmgrCMDBModuleIndexSet(unit, slot, SYSAPI_CARD_INDEX_INVALID);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Receive a status update request event
*
* @param    cmgr_cmpdu_t* cmpdu  Card Manager Protocol packet
*
* @returns
*
* @notes    This API is called when the local unit receives a request
*           to send a status update.
*
* @end
*********************************************************************/
void cmgrReceiveStatusUpdateRequest(cmgr_cmpdu_t* cmpdu)
{
  L7_uint32 local_unit_number;

  /* verify this is the requested unit */
  if (unitMgrNumberGet(&local_unit_number) != L7_SUCCESS)
    return;

  if (cmpdu->cmpdu_data.requestAll.requestedUnit != local_unit_number)
    return;

  /* Send the new Status Update */
  cmgrSendStatusUpdate(L7_CMPDU_REQUEST_ALL_INFO);

  return;
}

/*****************************************************************************/
/* Status Update Message                                                     */
/*****************************************************************************/
/*********************************************************************
*
* @purpose  Send a status update cmpdu
*
* @param    L7_cmpdu_packet_t type  Reason for status update.
*
* @returns
*
* @notes    This API is called when the unit sends a status update.
*
* @end
*********************************************************************/
void cmgrSendStatusUpdate(L7_cmpdu_packet_t type)
{
  cmgr_cmpdu_statusUpdate_t * smCmpdu;
  cmgr_cmpdu_t cmpdu;
  L7_uint32 unit = 0; /* Note that local unit info is in unit 0 */
  L7_uint32 slot = 0;
  L7_RC_t   rc;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardTypeDb;
  L7_uint32 mgr_unit_num;


  if (sysapiHpcBcastTransportIsReady() == L7_FALSE)
  {
    return;
  }

  /* Don't send card update messages until we know that this unit
  ** is the management unit or another management unit is present.
  */
  rc = usmDbUnitMgrMgrNumberGet(&mgr_unit_num);
  if (rc != L7_SUCCESS)
  {
    return;
  }

  /* lock db */
  (void)osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

  cmpdu.local            = 0;
  cmpdu.componentId      = L7_CARDMGR_COMPONENT_ID;
  cmpdu.cmgrPacketType   = L7_CMPDU_STATUS_UPDATE;

  rc = unitMgrNumberGet (&cmpdu.unit);
  if (rc != L7_SUCCESS)
  {
    /* Unit number is not assigned to this unit, so we
    ** should not send any update messages.
    */
    /* unlock db */
    osapiSemaGive(cmgrSemaId);
    return;
  }

  for (slot = 0; slot <= cmgrCMDBUnitMaxSlotsGet(unit); slot ++)
  {
    if (cmgrCMDBSlotIsFull(unit,slot) == L7_FALSE)
    {
      continue;
    }
    smCmpdu = &cmpdu.cmpdu_data.statusUpdate;
    smCmpdu->slotId             = cmgrCMDBSlotIdGet(unit, slot);
    smCmpdu->phySlotId          = cmgrCMDBPhySlotIdGet(unit, slot);
    smCmpdu->slotStatus         = cmgrCMDBSlotIsFull(unit, slot);
    smCmpdu->insertedCardTypeId = cmgrCMDBInsertedCardIdGet(unit, slot);
    smCmpdu->cardStatus         = cmgrCMDBCardStatusGet(unit, slot);
    smCmpdu->cardType           = cmgrCMDBCardTypeGet(unit, slot);
    smCmpdu->cardIndex          = cmgrCMDBModuleIndexGet(unit, slot);

    cardTypeDb = sysapiHpcCardDbEntryGet(smCmpdu->insertedCardTypeId);

   if ((cardTypeDb == L7_NULLPTR) || (cardTypeDb->type != SYSAPI_CARD_TYPE_LINE))
   {
     /* Send information only about line cards.
     */
     continue;
   }


    (void)cmgrSendCmpdu(&cmpdu, sizeof(cmpdu));

    if ((type == L7_CMPDU_CARD_REPORT_CARD_PLUGIN) ||
        (type == L7_CMPDU_CARD_REPORT_CARD_UNPLUG) ||
        (type == L7_CMPDU_CARD_REPORT_CARD_FAILURE))
    {
      osapiSleep(0);
      (void)cmgrSendCmpdu(&cmpdu, sizeof(cmpdu));

    }
  }

  /* unlock db */
  osapiSemaGive(cmgrSemaId);

}

/*********************************************************************
*
* @purpose  Receive a status update cmpdu
*
* @param    L7_cmpdu_packet_t type  Reason for status update.
*
* @returns
*
* @notes    This API is called when the unit sends a status update.
*
* @end
*********************************************************************/
L7_RC_t cmgrReceiveStatusUpdate(cmgr_cmpdu_t* cmpdu)
{
  cmgr_cmpdu_statusUpdate_t * smCmpdu;
  L7_RC_t   rc;
  L7_uint32 thisUnit,mgmtUnit;

  smCmpdu = (cmgr_cmpdu_statusUpdate_t *) &cmpdu->cmpdu_data.statusUpdate;

  /* Only process cmpdus from units that are within range. */
  if ((cmpdu->unit < 0) ||
      (cmpdu->unit > CMGR_MAX_UNITS_f))
  {
    CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrReceiveStatusUpdate: unit:%d Out of range\n",
               cmpdu->unit);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CARDMGR_COMPONENT_ID,
            "cmgrReceiveStatusUpdate: unit:%d Out of range\n", cmpdu->unit);
    return L7_ERROR;
  }

  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

  /* Update the Cmpdu db */
  cmgrUnitCmpduDbUpdate(cmpdu->unit, &(cmpdu->cmpdu_data.statusUpdate));

  if (cmgrCMDBUnitConnectedGet(cmpdu->unit) == L7_FALSE)
  {
    osapiSemaGive(cmgrSemaId);
    if ((unitMgrNumberGet(&thisUnit) == L7_SUCCESS) &&
        (unitMgrMgrNumberGet(&mgmtUnit) == L7_SUCCESS) &&
        (thisUnit == mgmtUnit))
    {
      CMGR_DEBUG(CMGR_LOG_FLAG,
                 "cmgrReceiveStatusUpdate: unit:%d Unconnected\n",
                 cmpdu->unit);
    }
    return L7_ERROR;
  }

  /* card inserted or removed */
  else if (smCmpdu->slotStatus !=
           cmgrCMDBSlotIsFull(cmpdu->unit, smCmpdu->slotId))
  {
    if (smCmpdu->slotStatus == L7_TRUE)
    {
      (void)cmgrCMDBPhySlotIdSet(cmpdu->unit, smCmpdu->slotId, smCmpdu->phySlotId);
      rc = cmgrProcessCardPlugIn(cmpdu->unit, smCmpdu->slotId,
                                 smCmpdu->insertedCardTypeId, smCmpdu->cardStatus,
                                 smCmpdu->cardType, smCmpdu->cardIndex);

      osapiSemaGive(cmgrSemaId);
      if (rc == L7_SUCCESS)
      {
        CMGR_DEBUG(CMGR_LOG_FLAG,
          "cmgrReceiveStatusUpdate: unit:%d phySlotId:%d slotId:%d plugin successful\n",
          cmpdu->unit, smCmpdu->phySlotId, smCmpdu->slotId);
        cmgrAsyncCardPluginNotify(cmpdu->unit, smCmpdu->slotId,
                                  smCmpdu->insertedCardTypeId);
      }
      else
      {
        CMGR_DEBUG(CMGR_LOG_FLAG,
                   "cmgrReceiveStatusUpdate: Card plugin for unit:%d phySlotId:%d slotId:%d- FAILED\n",
                   cmpdu->unit, smCmpdu->phySlotId, smCmpdu->slotId);
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CARDMGR_COMPONENT_ID,
                "cmgrReceiveStatusUpdate: Card plugin for unit:%d phySlotId:%d slotId:%d- FAILED\n",
                cmpdu->unit, smCmpdu->phySlotId, smCmpdu->slotId);
      }
    }
    else
    {
      cmgrProcessCardUnplug(cmpdu->unit, smCmpdu->slotId, smCmpdu->cardStatus,
                            smCmpdu->cardType, smCmpdu->cardIndex);

      osapiSemaGive(cmgrSemaId);

      cmgrAsyncCardUnPlugNotify(cmpdu->unit, smCmpdu->phySlotId,
                                cmgrCMDBInsertedCardIdGet(cmpdu->unit,
                                                          smCmpdu->slotId));
    }
  }
  /* card status changed */
  else if (smCmpdu->cardStatus !=
           cmgrCMDBCardStatusGet(cmpdu->unit, smCmpdu->slotId))
  {
    cmgrCMDBCardStatusSet(cmpdu->unit, smCmpdu->slotId,
                          smCmpdu->cardStatus);
    /* QSCAN_KMM: may want to issue a trap??? */
    osapiSemaGive(cmgrSemaId);
    CMGR_DEBUG(CMGR_LOG_FLAG,
               "cmgrReceiveStatusUpdate: Card status changed for unit:%d phySlotId:%d slotId:%d cardstatus:%s\n",
               cmpdu->unit, smCmpdu->phySlotId, smCmpdu->slotId,
               cardStatusStr[smCmpdu->cardStatus]);
  }
  /* card type changed */
  else if (smCmpdu->insertedCardTypeId !=
           cmgrCMDBInsertedCardIdGet(cmpdu->unit, smCmpdu->slotId))
  {
    /* QSCAN_KMM: This shouldn't happen...log message */
    osapiSemaGive(cmgrSemaId);
  }
  else
  {
    osapiSemaGive(cmgrSemaId);
    CMGR_DEBUG(CMGR_LOG_FLAG,
      "cmgrReceiveStatusUpdate: unit:%d phySlotId:%d slotId:%d PacketType:%d\n",
      cmpdu->unit, smCmpdu->phySlotId, smCmpdu->slotId, cmpdu->cmgrPacketType);

    /*
     * Since this is a heartbeat message, only log it if tracing is enabled.
     */
    if (CMGR_LOG_FLAG)
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG , L7_CARDMGR_COMPONENT_ID,
              "cmgrReceiveStatusUpdate: unit:%d phySlotId:%d slotId:%d PacketType:%d\n",
              cmpdu->unit, smCmpdu->phySlotId, smCmpdu->slotId, cmpdu->cmgrPacketType);
    }
  }

  return L7_SUCCESS;
}

