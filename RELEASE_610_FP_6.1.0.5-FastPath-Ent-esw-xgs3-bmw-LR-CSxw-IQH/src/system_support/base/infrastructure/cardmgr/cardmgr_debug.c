/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename cardmgr_debug.c
*
* @purpose Card Manager debug functions
*
* @component cardmgr
*
* @comments none
*
* @create 30/05/2006
*
* @author asingh
*
* @end
*
**********************************************************************/
#include "sysapi_hpc.h"
#include "cardmgr_api.h"
#include "cardmgr.h"

extern L7_char8 cardTypeStr[(SYSAPI_CARD_TYPE_LAST+1)][30];
extern L7_char8 cardStatusStr[L7_CARD_STATUS_TOTAL][30];
extern L7_char8 cardEventStr[SYSAPI_CARD_EVENT_LAST][30];
extern L7_BOOL boardIdValidate_g;
extern void *cmgrQueue;
extern L7_int32 auto_synchronize;
L7_BOOL cmgrLogFlag_g = L7_FALSE;

void cmgrDebugPopulateUnit(L7_uint32 unit)
{
  L7_uint32 slot;

  if (unit > CMGR_MAX_UNITS_f)
  {
    printf("\r\ninvalid unit or slot\r\n");
    return;
  }
  cmgrCMDBUnitConnectedSet(unit, L7_TRUE);
  cmgrCMDBUnitConfiguredSet(unit, L7_TRUE);
  cmgrCMDBUnitTypeSet(unit, 1);
  cmgrCMDBUnitMaxSlotsSet(unit, 4);
  for (slot = 1; slot < 4; slot++)
  {
    cmgrCMDBSlotIdSet(unit, slot , slot);
    cmgrCMDBPhySlotIdSet(unit, slot, slot);
    cmgrCMDBSlotIsFullSet(unit,slot , L7_TRUE);
    cmgrCMDBSlotConfiguredAdminModeSet(unit,slot,L7_ENABLE);
    cmgrCMDBSlotConfiguredPowerModeSet(unit,slot , L7_ENABLE);
    cmgrCMDBCardStatusSet(unit,slot,1);
    cmgrCMDBCardInsertedCardIdSet(unit,slot,L7_CMGR_CARD_ID_NONE);
    cmgrCMDBCardConfigCardIdSet(unit,slot,L7_CMGR_CARD_ID_NONE);
    cmgrCMDBCardTypeSet(unit,slot,SYSAPI_CARD_TYPE_LINE);
    cmgrCMDBModuleIndexSet(unit,slot,SYSAPI_CARD_INDEX_1);
  }
  printf("\r\nDatabase is populated\r\n");
  return;
}

void cmgrDebugDisplaySlot(L7_uint32 unit, L7_uint32 slot)
{
  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    printf("\r\ninvalid unit or slot\r\n");
  else
  {
    printf("\r\nUnit=%d, Slot=%d\r\n", unit, slot);
    if (cmgrCMDBUnitConnectedGet(unit) == L7_TRUE)
      printf("Unit connected; ");
    else
      printf("Unit disconnected; ");
    if (cmgrCMDBUnitConfiguredGet(unit) == L7_TRUE)
      printf("Unit configured; ");
    else
      printf("Unit unconfigured; ");

    printf("UnitType= 0x%x; ", cmgrCMDBUnitTypeGet(unit));
    printf("MaxSlots= %d \r\n",cmgrCMDBUnitMaxSlotsGet(unit));

    printf("SlotId=%d; ",cmgrCMDBSlotIdGet(unit,slot));
    printf("PhySlotId=%d; ",cmgrCMDBPhySlotIdGet(unit,slot));
    if (cmgrCMDBSlotIsFull(unit,slot) == L7_TRUE)
    {
      printf("Slot Full; ");
      printf("InsertedCardType=%s ;",
             cardTypeStr[cmgrCMDBCardTypeGet(unit,slot)]);
      printf("CardStatus=%s ;",
              cardStatusStr[cmgrCMDBCardStatusGet(unit,slot)]);
    }
    else
    {
      printf("Slot Empty; ");
      printf("InsertedCardType=%d ;", cmgrCMDBCardTypeGet(unit,slot));
      printf("CardStatus=%d ;",
              cmgrCMDBCardStatusGet(unit,slot));
    }

    printf("AdminMode=%d ;",
            cmgrCMDBConfiguredAdminModeGet(unit,slot));
    printf("PowerMode=%d \r\n",
            cmgrCMDBConfiguredPowerModeGet(unit,slot));
    printf("InsertedModuleIndex=%d ;",
           cmgrCMDBModuleIndexGet(unit,slot));
    printf("InsertedCardId=0x%x ;",
            cmgrCMDBInsertedCardIdGet(unit,slot));
    printf("ConfigCardId=0x%x \r\n",
            cmgrCMDBConfigCardIdGet(unit,slot));
  }
  return;
}

void cmgrDebugDisplayUnit(L7_uint32 unit)
{
  L7_uint32 slot;

  printf("\r\nUnit=%d\r\n", unit);
  if (cmgrCMDBUnitConnectedGet(unit) == L7_TRUE)
    printf("Unit connected; ");
  else
    printf("Unit disconnected; ");
  if (cmgrCMDBUnitConfiguredGet(unit) == L7_TRUE)
    printf("Unit configured; ");
  else
    printf("Unit unconfigured; ");

  printf("UnitNumber = %d; ", unit);
  printf("UnitType = 0x%x; ", cmgrCMDBUnitTypeGet(unit));
  printf("MaxSlots = %d \r\n",cmgrCMDBUnitMaxSlotsGet(unit));

  for (slot=0; slot <= cmgrCMDBUnitMaxSlotsGet(unit); slot++)
  {
    printf("\nSlotId=%d; ",cmgrCMDBSlotIdGet(unit,slot));
    printf("phySlotId=%d; ", cmgrCMDBPhySlotIdGet(unit, slot));
    if (cmgrCMDBSlotIsFull(unit,slot) == L7_TRUE)
    {
      printf("Slot Full; ");
      printf("InsertedCardType=%s ;",
             cardTypeStr[cmgrCMDBCardTypeGet(unit,slot)]);
      printf ("CardStatus = %s ;",
              cardStatusStr[cmgrCMDBCardStatusGet(unit,slot)]);
    }
    else
    {
      printf("Slot Empty; ");
      printf("InsertedCardType=%d ;", cmgrCMDBCardTypeGet(unit,slot));
      printf ("CardStatus = %d ;", cmgrCMDBCardStatusGet(unit,slot));
    }

    printf ("AdminMode= %d ;",
            cmgrCMDBConfiguredAdminModeGet(unit,slot));

    printf ("PowerMode = %d \r\n",
            cmgrCMDBConfiguredPowerModeGet(unit,slot));

	printf("ModuleIndex=%d ;",
			cmgrCMDBModuleIndexGet(unit,slot));

    printf ("CardStatus = %d ;",
            cmgrCMDBCardStatusGet(unit,slot));

    printf ("InsertedCardId = 0x%x ;",
            cmgrCMDBInsertedCardIdGet(unit,slot));

    printf ("ConfigCardId= 0x%x \r\n",
            cmgrCMDBConfigCardIdGet(unit,slot));
  }
  return;
}

void cmgrDebugStatusUpdate()
{
  cmgr_cmpdu_t hpcCmpdu;

  hpcCmpdu.cmgrPacketType = L7_CMPDU_STATUS_UPDATE;
  hpcCmpdu.unit = 1;
  hpcCmpdu.cmpdu_data.statusUpdate.slotId = 1;
  hpcCmpdu.cmpdu_data.statusUpdate.slotStatus = L7_TRUE;
  hpcCmpdu.cmpdu_data.statusUpdate.insertedCardTypeId = 2;
  hpcCmpdu.cmpdu_data.statusUpdate.cardStatus = 3;
  hpcCmpdu.cmpdu_data.statusUpdate.cardType = SYSAPI_CARD_TYPE_FAN;
  hpcCmpdu.cmpdu_data.statusUpdate.cardIndex = SYSAPI_CARD_INDEX_2;

  cmgrReceiveCMPDUCallback((L7_uchar8*)&hpcCmpdu,6);
  return;
}

void cmgrDebugUnit2StatusUpdate()
{
  cmgr_cmpdu_statusUpdate_t * smCmpdu;
  cmgr_cmpdu_t cmpdu;
  L7_uint32 unit = 0;
  L7_RC_t rc;


  unit = 2;
  smCmpdu = (cmgr_cmpdu_statusUpdate_t*) &cmpdu.cmpdu_data.statusUpdate;

  cmpdu.cmgrPacketType = L7_CMPDU_STATUS_UPDATE;
  cmpdu.unit = unit;

  smCmpdu->slotId = 0;
  smCmpdu->slotStatus = L7_TRUE;
  smCmpdu->insertedCardTypeId = 2;
  smCmpdu->cardStatus = L7_CARD_STATUS_WORKING;
  smCmpdu->cardType = SYSAPI_CARD_TYPE_POWER_SUPPLY;
  smCmpdu->cardIndex = SYSAPI_CARD_INDEX_1;

  rc = osapiMessageSend(cmgrQueue, &cmpdu, sizeof(cmgr_cmpdu_t), 0, L7_MSG_PRIORITY_NORM );
}

void cmgrDebugCardPlugIn(L7_uint32 slot, L7_uint32 cardTypeIndex)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardDesc;
  hpcEvent_t event;
  event.event = SYSAPI_CARD_EVENT_PLUG;
  event.moduleIndex = SYSAPI_CARD_INDEX_1;
  event.moduleType = SYSAPI_CARD_TYPE_LINE;

  cardDesc = sysapiHpcCardDbEntryByIndexGet(cardTypeIndex);
  if (cardDesc != L7_NULLPTR)
  {
    cmgrCardReportCallBack(slot, cardDesc->cardTypeId, event);
  }
  else
  {
    printf("Invalid cardTypeIndex.\n");
  }
  return;
}

void cmgrDebugCardUnPlug(L7_uint32 slot, L7_uint32 cardTypeIndex)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardDesc;
  hpcEvent_t event;
  event.event = SYSAPI_CARD_EVENT_UNPLUG;
  event.moduleIndex = SYSAPI_CARD_INDEX_1;
  event.moduleType = SYSAPI_CARD_TYPE_LINE;

  cardDesc = sysapiHpcCardDbEntryByIndexGet(cardTypeIndex);
  if (cardDesc != L7_NULLPTR)
  {
    cmgrCardReportCallBack(slot, cardDesc->cardTypeId, event);
  }
  else
  {
    printf("Invalid cardTypeIndex.\n");
  }
  return;
}

void cmgrDebugCardFailure(L7_uint32 slot, L7_uint32 cardTypeIndex)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardDesc;
  hpcEvent_t event;
  event.event = SYSAPI_CARD_EVENT_FAILURE;
  event.moduleIndex = SYSAPI_CARD_INDEX_1;
  event.moduleType = SYSAPI_CARD_TYPE_LINE;

  cardDesc = sysapiHpcCardDbEntryByIndexGet(cardTypeIndex);
  if (cardDesc != L7_NULLPTR)
  {
    cmgrCardReportCallBack(slot, cardDesc->cardTypeId, event);
  }
  else
  {
    printf("Invalid cardTypeIndex.\n");
  }
  return;
}

/* This is a temporary debug function, To be removed later */
void cmgrDebugIsBoardIdValid(L7_BOOL value)
{
  if((value != L7_TRUE) && (value != L7_FALSE))
  {
    printf("\nInvalid Input value %d\n", value);
    return;
  }
/* set board Id validation flag in card manager */
  boardIdValidate_g = value;
}

void cmgrDebugSupportedCardsList()
{
  L7_uint32 usmdbDebugSupCardsList (void);

  (void) usmdbDebugSupCardsList ();
}

void cmgrDebugCardCPU()
{
  L7_uint32 cardTypeId = 0;
  hpcEvent_t event;
  event.event = SYSAPI_CARD_EVENT_PLUG;
  event.moduleIndex = SYSAPI_CARD_INDEX_1;
  event.moduleType = SYSAPI_CARD_TYPE_LOGICAL_CPU;

  sysapiHpcCardDbCardIdGet(5, &cardTypeId);
  cmgrCardReportCallBack(L7_CPU_SLOT_NUM, cardTypeId, event);
    return;
}

void cmgrDebugCardLAG()
{
  L7_uint32 cardTypeId = 0;
  hpcEvent_t event;
  event.event = SYSAPI_CARD_EVENT_PLUG;
  event.moduleIndex = SYSAPI_CARD_INDEX_1;
  event.moduleType = SYSAPI_CARD_TYPE_LAG;

  /* L7_LOGICAL_CARD_LAG_ID */
  sysapiHpcCardDbCardIdGet(6, &cardTypeId);
  cmgrCardReportCallBack(L7_LAG_SLOT_NUM, cardTypeId, event);
    return;
}

void cmgrDebugCardRouter()
{
  L7_uint32 cardTypeId = 0;
  hpcEvent_t event;
  event.event = SYSAPI_CARD_EVENT_PLUG;
  event.moduleIndex = SYSAPI_CARD_INDEX_1;
  event.moduleType = SYSAPI_CARD_TYPE_VLAN_ROUTER;

  /* L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID */
  sysapiHpcCardDbCardIdGet(7, &cardTypeId);
  cmgrCardReportCallBack(L7_VLAN_SLOT_NUM, cardTypeId, event);
    return;
}

void cmgrDebugCardConfig()
{
    cmgrSlotCardConfigSet(1,2,7);
    return;
}

void cmgrDebugCardClear()
{
    cmgrSlotCardConfigClear(1,2);
    return;
}

void cmgrDebugLagUspGet (void)
{
  L7_uint32 unit, slot;
  L7_RC_t rc;

  rc = cmgrLagCardUSPGet (&unit, &slot);
  if (rc == L7_SUCCESS)
  {
    printf("LAG Unit = %d, Slot = %d\n", unit, slot);
  }
  else
  {
    printf("LAG card does not exist.\n");
  }
}

void cmgrDebugVlanUspGet (void)
{
  L7_uint32 unit, slot;
  L7_RC_t rc;

  rc = cmgrVlanCardUSPGet (&unit, &slot);
  if (rc == L7_SUCCESS)
  {
    printf("VLAN Unit = %d, Slot = %d\n", unit, slot);
  }
  else
  {
    printf("VLAN card does not exist.\n");
  }

}

void cmgrDebugDatabaseDisplay()
{
  L7_uint32 unit = 0;

  for (unit = 0; unit <= CMGR_MAX_UNITS_f; unit++)
  {
    cmgrDebugDisplayUnit(unit);
  }

  return;
}

void cmgrDebugEnable(L7_BOOL debugLevel)
{
  cmgrLogFlag_g = debugLevel;
}

void cmgrDebugSetAutoSynchronize(int autoSyn)
{
  auto_synchronize = autoSyn;
}
