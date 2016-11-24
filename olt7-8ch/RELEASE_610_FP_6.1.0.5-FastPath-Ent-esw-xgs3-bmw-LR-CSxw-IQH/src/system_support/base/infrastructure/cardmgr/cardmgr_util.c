/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 * @filename cardmgr_util.c
 *
 * @purpose Card Manager utility functions
 *
 * @component cardmgr
 *
 * @comments none
 *
 * @create 01/06/2003
 *
 * @author kmcdowell
 *
 * @end
 *
 **********************************************************************/

#define L7_MAC_ENET_BCAST
#include "cardmgr_include.h"
#include "log.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_cardmgr_api.h"
#include "trap_inventory_api.h"
#include "spm_api.h"
#include "l7_cnfgr_api.h"
#include "sysapi_hpc.h"
#include "cfg_change_api.h"
#include "sysapi_hpc_chassis.h"
#include "chassis_alarmmgr_api.h"
#include "unitmgr_api.h"
#include "sysapi_hpc_slotmapper.h"

L7_cardMgrCfgFileData_t cmgrCfgData;

cmgr_unit_t *cmUnit;
extern cmgrNotifyList_t cmgrNotifyList[L7_LAST_COMPONENT_ID];
extern void (*cmgrCardConfigCallback)(L7_CMGR_CONFIG_EVENT_TYPES_t ev);
extern void *cmgrSemaId;
extern void *cmgrCardInsertRemoveSemaId;
extern void *cmgrTimerSyncSemaId;
extern void  *cmgrQueue;
extern void *cmgrCardInsertRemoveQueue;

extern L7_cmgr_state_t cmgrState;

osapiTimerDescr_t *pCmgrTimerHolder;
static void *cmgrNimSyncSem = L7_NULLPTR;
static void cmgrCardInsertRemoveWait(void);

L7_BOOL boardIdValidate_g = L7_FALSE;

/* char strings for card types to be used in debug messages */
L7_char8 cardTypeStr[(SYSAPI_CARD_TYPE_LAST+1)][30] =
{
  "CARD_TYPE_NOT_PRESENT",
  "CARD_TYPE_LINE",
  "CARD_TYPE_LAG",
  "CARD_TYPE_VLAN_ROUTER",
  "CARD_TYPE_CPU",
  "CARD_TYPE_LOGICAL_CPU",
  "CARD_TYPE_FAN",
  "CARD_TYPE_POWER_SUPPLY",
  "CARD_TYPE_CONTROL",
  "CARD_TYPE_CONTROL_STANDBY",
  "CARD_TYPE_LOOPBACK",
  "CARD_TYPE_TUNNEL",
  "CARD_TYPE_CAPWAP_TUNNEL",
  "CARD_TYPE_LAST"
};

L7_char8 cardStatusStr[L7_CARD_STATUS_TOTAL][30] =
{
  "CARD_STATUS_INVALID",
  "CARD_STATUS_UNKNOWN",
  "CARD_STATUS_UNPLUG",
  "CARD_STATUS_PLUG",
  "CARD_STATUS_WORKING",
  "CARD_STATUS_DIAGFAILED",
  "CARD_STATUS_DOWNLOADING",
  "CARD_STATUS_MISMATCH",
  "CARD_STATUS_UNSUPPORTED",
  "CARD_STATUS_FAILED"
};

L7_char8 cardEventStr[SYSAPI_CARD_EVENT_LAST][30] =
{
  "SYSAPI_CARD_EVENT_NONE",
  "SYSAPI_CARD_EVENT_PLUG",
  "SYSAPI_CARD_EVENT_UNPLUG",
  "SYSAPI_CARD_EVENT_FAILURE",
  "SYSAPI_CARD_EVENT_HEALTHY",
  "SYSAPI_CARD_EVENT_INIT_DONE",
  "SYSAPI_CARD_EVENT_ACTIVE",
  "SYSAPI_CARD_EVENT_STANDBY",
  "SYSAPI_CARD_EVENT_VER_MISMATCH"
};

/* Handle to the Configurator Callback function for initialisation done LED */
static L7_CNFGR_CB_HANDLE_t cmgrInitCompleteHandle_g = L7_CNFGR_NO_HANDLE;
static void cmgrInitCompleteCallBack(void);

/*****************************************************************************/
/*  Helper Functions                                                         */
/*****************************************************************************/

/*********************************************************************
 *
 * @purpose  Create sync semaphore for port-create/attach/detach/delete
 *           events.
 *
 * @returns none
 *
 * @notes    Card manager will wait on this semaphore for NIM to notify
 *           it that all port processing is done.
 *
 * @end
 *********************************************************************/
void cmgrNimSyncSemCreate (void)
{
  cmgrNimSyncSem = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_EMPTY);
  if (cmgrNimSyncSem == L7_NULLPTR)
  {
    LOG_ERROR (0);
  }
}

/*********************************************************************
 *
 * @purpose  Wait for the NIM sync semaphore.
 *
 * @returns none
 *
 * @notes    Card manager will wait on this semaphore for NIM to notify
 *           it that all port processing is done.
 *
 * @end
 *********************************************************************/
static void cmgrNimSyncSemGet (void)
{
  L7_RC_t rc;

  rc = osapiSemaTake (cmgrNimSyncSem, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
}

/*********************************************************************
 *
 * @purpose  NIM Callback function.
 *
 * @returns none
 *
 * @notes    Card manager will wait on this semaphore for NIM to notify
 *           it that all port processing is done.
 *
 * @end
 *********************************************************************/
void cmgrNimSyncSemGive (NIM_NOTIFY_CB_INFO_t retVal)
{
  L7_RC_t rc;

  rc = osapiSemaGive (cmgrNimSyncSem);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
}

/*********************************************************************
 *
 * @purpose  Send Card Create notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 *
 * @returns
 *
 * @notes    This API is called to notify registrants when a card
 *           has been pluggd.
 *           The function enqueues the command to the card insert
 *           remove task.
 *
 * @end
 *********************************************************************/
void cmgrAsyncCardCreateNotify(L7_uint32 unit, L7_uint32 slot,
                               L7_uint32 cardId)
{
  cmgrCardInsertRemoveMsg_t cmd;
  L7_RC_t rc;

  cmd.action = L7_CMGR_CARD_CREATE;
  cmd.unit = unit;
  cmd.slot = slot;
  cmd.cardId = cardId;

  rc = osapiMessageSend(cmgrCardInsertRemoveQueue,
                        (void *)&cmd,
                        sizeof(cmgrCardInsertRemoveMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_HI);
  /* We are using the high priority to get the slots created before the plugins */
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
}

void cmgrAsyncCardConfigSlots(void)
{
  cmgrCardInsertRemoveMsg_t cmd;
  L7_RC_t rc;

  cmd.action = L7_CMGR_CARD_CONFIG_SLOTS;
  cmd.unit = 0;
  cmd.slot = 0;
  cmd.cardId = 0;

  rc = osapiMessageSend(cmgrCardInsertRemoveQueue,
                        (void *)&cmd,
                        sizeof(cmgrCardInsertRemoveMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
}

void cmgrAsyncCardConfigPorts(void)
{
  cmgrCardInsertRemoveMsg_t cmd;
  L7_RC_t rc;

  cmd.action = L7_CMGR_CARD_CONFIG_PORTS;
  cmd.unit = 0;
  cmd.slot = 0;
  cmd.cardId = 0;

  rc = osapiMessageSend(cmgrCardInsertRemoveQueue,
                        (void *)&cmd,
                        sizeof(cmgrCardInsertRemoveMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
}

/*********************************************************************
 *
 * @purpose  Send Card create and Port create notifications
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardId
 *
 * @returns
 *
 * @notes    This API is called when a card has been created.  It notifies
 *           registrants that the card and ports must be created.
 *
 * @end
 *********************************************************************/
void cmgrCardCreateNotify(L7_uint32 unit, L7_uint32 slot,
                          L7_uint32 cardId)
{
  L7_uint32 port = 0;
  SYSAPI_HPC_CARD_DESCRIPTOR_t* cardTypeDb;

  cardTypeDb = sysapiHpcCardDbEntryGet(cardId);

  if (cardTypeDb != L7_NULLPTR)
  {
    cmgrCfgData.cfgParms.configCard[unit][slot].card_id = cardId;
    cmgrCfgData.cfgHdr.dataChanged = L7_TRUE;

    if (cmgrCMDBCardCreateNotifiedGet (unit,slot ) == L7_FALSE)
    {
      /* send "card create" to registrants */
      cmgrDoNotifyCardCreate(unit, slot, cardId);

      osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
      cmgrCMDBCardCreateNotifiedSet (unit,slot , L7_TRUE);
      osapiSemaGive(cmgrSemaId);
    }

    /* Only send port change messages if in steady state */
    if (cmgrState == L7_CNFGR_STATE_E)
    {
      if (cmgrCMDBPortCreateNotifiedGet (unit,slot ) == L7_FALSE)
      {
        CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_CREATE, L7_TRUE, unit, slot);
        for (port = 1; port <= cardTypeDb->numOfNiPorts; port ++)
        {
          if (cmgrState != L7_CNFGR_STATE_E)
          {
            break;
          }

          (void)cmgrDoNotifyPortChange(unit, slot, port,
                                       cardId, L7_CREATE,
                                       &cardTypeDb->portInfo[port-1]);
        }
        CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_CREATE, L7_FALSE, unit, slot);

        osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
        cmgrCMDBPortCreateNotifiedSet (unit,slot , L7_TRUE);
        osapiSemaGive(cmgrSemaId);
      }
    }
  }
  return;
}

/*********************************************************************
 *
 * @purpose  Send Card Create notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 *
 * @returns
 *
 * @notes    This API is called to notify registrants when a card
 *           has been pluggd.
 *           The function enqueues the command to the card insert
 *           remove task.
 *
 * @end
 *********************************************************************/
void cmgrAsyncCardClearNotify(L7_uint32 unit, L7_uint32 slot,
                              L7_uint32 cardId)
{
  cmgrCardInsertRemoveMsg_t cmd;
  L7_RC_t rc;

  cmd.action = L7_CMGR_CARD_DELETE;
  cmd.unit = unit;
  cmd.slot = slot;
  cmd.cardId = cardId;

  rc = osapiMessageSend(cmgrCardInsertRemoveQueue,
                        (void *)&cmd,
                        sizeof(cmgrCardInsertRemoveMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
}

/*********************************************************************
 *
 * @purpose  Send Card clear and Port delete notifications
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardId   card identifier
 *
 * @returns
 *
 * @notes    This API is called when a card has been cleared.  It notifies
 *           registrants that the card and ports must be cleared.
 *
 * @end
 *********************************************************************/
void cmgrCardClearNotify(L7_uint32 unit, L7_uint32 slot,
                         L7_uint32 cardId)
{
  L7_uint32 port = 0;
  SYSAPI_HPC_CARD_DESCRIPTOR_t* cardTypeDb;
  /*    L7_RC_t rc;  */

  cardTypeDb = sysapiHpcCardDbEntryGet(cardId);

  if (cardTypeDb != L7_NULLPTR)
  {
    /* Only send port change messages if in steady state */
    if ((cmgrState == L7_CNFGR_STATE_E) ||
        (cmgrState == L7_CNFGR_STATE_U1))
    {
      if (cmgrCMDBPortCreateNotifiedGet (unit,slot ) == L7_TRUE)
      {
        for (port = 1; port <= cardTypeDb->numOfNiPorts; port ++)
          (void)cmgrDoNotifyPortChange(unit, slot, port,
                                       cardId, L7_DELETE,
                                       &cardTypeDb->portInfo[port-1]);

        cmgrCMDBPortCreateNotifiedSet (unit,slot , L7_FALSE);
      }
    }

    cmgrCfgData.cfgParms.configCard[unit][slot].card_id = L7_CMGR_CARD_ID_NONE;
    cmgrCfgData.cfgHdr.dataChanged = L7_FALSE;


    if (cmgrCMDBCardCreateNotifiedGet (unit,slot ) == L7_TRUE)
    {
      cmgrDoNotifyCardClear(unit, slot);

      cmgrCMDBCardCreateNotifiedSet (unit,slot , L7_FALSE);
    }
  }
  return;
}

/*********************************************************************
 *
 * @purpose  Send Card plug-in notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 *
 * @returns
 *
 * @notes    This API is called to notify registrants when a card
 *           has been pluggd.
 *           The function enqueues the command to the card insert
 *           remove task.
 *
 * @end
 *********************************************************************/
void cmgrAsyncCardPluginNotify(L7_uint32 unit, L7_uint32 slot,
                               L7_uint32 cardId)
{
  cmgrCardInsertRemoveMsg_t cmd;
  L7_RC_t rc;

  cmd.action = L7_CMGR_CARD_INSERT;
  cmd.unit = unit;
  cmd.slot = slot;
  cmd.cardId = cardId;

  rc = osapiMessageSend(cmgrCardInsertRemoveQueue,
                        (void *)&cmd,
                        sizeof(cmgrCardInsertRemoveMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
}

/*********************************************************************
 *
 * @purpose  Send Card plugin notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_CARD_TYPES_t cardType   card type
 *
 * @returns
 *
 * @notes    This API is called to notify registrants when a card
 *           has been pluggd in. If possible, this should not be
 *           called within the card manager database semaphore.
 *
 * @end
 *********************************************************************/
void cmgrCardPluginNotify(L7_uint32 unit, L7_uint32 slot,
                          L7_uint32 cardId)
{
  L7_uint32 mode;
  L7_uint32 port = 0;
  SYSAPI_HPC_CARD_DESCRIPTOR_t* cardTypeDb;
  L7_RC_t rc;
  L7_BOOL stacking_present;

  stacking_present = cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                                           L7_STACKING_FEATURE_SUPPORTED);

  cardTypeDb = sysapiHpcCardDbEntryGet(cardId);

  if (cardTypeDb != L7_NULLPTR)
  {
    /* issue plug in only if in steady state ---
     *
     * NOTE: The following code has redundant check
     *       for steady state.  This is done in the
     *       case that optimization is done and the
     *       plug in can be sent before entering
     *       steady state.
     */

    if ((cmgrState == L7_CNFGR_STATE_E ||
         stacking_present == L7_FALSE))
    {
      if ((cardTypeDb->type == SYSAPI_CARD_TYPE_LINE) &&
          (cmgrCMDBPortCreateNotifiedGet (unit,slot) == L7_FALSE))
      {
        return;
      }
      if (cmgrCMDBCardInsertNotifiedGet (unit,slot) == L7_FALSE )
      {
        osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
        cmgrCMDBCardStatusSet(unit,slot, L7_CARD_STATUS_WORKING);
        osapiSemaGive(cmgrSemaId);

        if((cardTypeDb->type != SYSAPI_CARD_TYPE_FAN) &&
           (cardTypeDb->type != SYSAPI_CARD_TYPE_POWER_SUPPLY))
        {
          /* insert events for all card types other than line cards were done during phase 2 */
          CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_INSERT, L7_TRUE, unit, slot);
          rc = dtlCardCmd((L7_ushort16)unit, (L7_ushort16)slot,
                          DTL_CARD_INSERT, (L7_ulong32)cardId);
          CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_INSERT, L7_FALSE, unit, slot);

          if (rc != L7_SUCCESS)
          {
            CMGR_DEBUG(CMGR_LOG_FLAG,
                       "cmgrCardPluginNotify: error sending DTL_CARD_INSERT\n");
            LOG_MSG("cmgrCardPluginNotify: error sending DTL_CARD_INSERT\n");

            /* clean up CMDB due to failed card plugin */
            osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
            cmgrProcessCardUnplug(unit,slot, L7_CARD_STATUS_UNKNOWN,
                                  SYSAPI_CARD_TYPE_INVALID, SYSAPI_CARD_INDEX_INVALID);
            osapiSemaGive(cmgrSemaId);
            return;
          }

          /* Update Led status */
          (void)cmgrLedInitScmLm(unit, cmgrCMDBPhySlotIdGet(unit, slot));
        }
        osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
        (void)cmgrCMDBSlotConfiguredAdminModeSet(unit, slot, L7_ENABLE);
        (void)cmgrCMDBSlotConfiguredPowerModeSet(unit, slot, L7_ENABLE);
        osapiSemaGive(cmgrSemaId);

        /* Don't tell anybody about logical cards.
         */
        if ((cardTypeDb->type == SYSAPI_CARD_TYPE_LINE) ||
            (cardTypeDb->type == SYSAPI_CARD_TYPE_FAN) ||
            (cardTypeDb->type == SYSAPI_CARD_TYPE_POWER_SUPPLY))
        {
          cmgrDoNotifyCardPlugin(unit, slot, cardId);
        }

        osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
        cmgrCMDBCardInsertNotifiedSet (unit,slot , L7_TRUE);
        osapiSemaGive(cmgrSemaId);
      }
    }

    /* NOTE: For the disabled card when a new card-plugin is detected by CM,
     *       don't issue port-plugin events for the ports on the card.
     */
    mode = cmgrCMDBConfiguredAdminModeGet(unit, slot);
    if (mode == L7_ENABLE)
    {
      /* Only send line port change messages if in steady state */
      if (cmgrState == L7_CNFGR_STATE_E &&
          cardTypeDb->type == SYSAPI_CARD_TYPE_LINE)
      {
        if ((cmgrCMDBPortInsertNotifiedGet (unit,slot) == L7_FALSE) &&
            (cmgrCMDBPortCreateNotifiedGet (unit,slot ) == L7_TRUE))
        {
          CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_ATTACH, L7_TRUE, unit, slot);
          (void) dtlCardCmd((L7_ushort16)unit, (L7_ushort16)slot,
                            DTL_CARD_PLUGIN_START, (L7_ulong32)cardId);
          for (port = 1; port <= cardTypeDb->numOfNiPorts; port ++)
          {
            /* In case we start the unconfigure cycle while inserting a card
            ** skip the remaining ports.
            */
            if (cmgrState != L7_CNFGR_STATE_E)
            {
              break;
            }

            cmgrDoNotifyPortChange(unit, slot, port,
                                   cardId, L7_ATTACH,
                                   &cardTypeDb->portInfo[port-1]);
          }

          (void) dtlCardCmd((L7_ushort16)unit, (L7_ushort16)slot,
                            DTL_CARD_PLUGIN_FINISH, (L7_ulong32)cardId);
          CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_ATTACH, L7_FALSE, unit, slot);
          osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
          cmgrCMDBPortInsertNotifiedSet (unit,slot , L7_TRUE);
          osapiSemaGive(cmgrSemaId);
        }
      }
    }

    if (cmgrState == L7_CNFGR_STATE_E &&
        cardTypeDb->type == SYSAPI_CARD_TYPE_LOGICAL_CPU)
    {
      if (cmgrCMDBPortInsertNotifiedGet (unit,slot) == L7_FALSE)
      {
        CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_ATTACH, L7_TRUE, unit, slot);
        cmgrDoNotifyPortChange(unit, slot, 1,
                               cardId, L7_ATTACH,
                               &cardTypeDb->portInfo[0]);
        CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_ATTACH, L7_FALSE, unit, slot);
        osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
        cmgrCMDBPortInsertNotifiedSet (unit,slot , L7_TRUE);
        osapiSemaGive(cmgrSemaId);
      }
    }
  }
  return;
}

/*********************************************************************
 * @purpose  Database Access - get card type.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  card type
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBCardTypeGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->cardType;
}

/*********************************************************************
 * @purpose  Database Access - get module index.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  module index
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBModuleIndexGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->cardIndex;
}

/*********************************************************************
 * @purpose  Database Access - set card type.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   card status
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardTypeSet(L7_uint32 unit, L7_uint32 slot, L7_uint32 value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->cardType = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - set card type.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   card status
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBModuleIndexSet(L7_uint32 unit, L7_uint32 slot, L7_uint32 value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->cardIndex = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Set LM slot power - wrapper function for the HPC API
 *
 * @param    slot    slot
 * @param    status  status to be set
 * @param    souurce source system / administrator
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_FAILURE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrHpcPwrSlotPowerSet(L7_uint32 slotNum,
                               POWER_STATUS_t status, POWER_CONTROL_t source)
{
  L7_RC_t rc = L7_FAILURE;
  rc = sysapiHpcSlotPowerStatusSet(slotNum, status, source);
  return rc;
}

/*********************************************************************
 * @purpose  get LM slot power status - wrapper function for the HPC API
 *
 * @param    slot    slot
 * @param    status  status to be set
 * @param    source  source system / administrator
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_FAILURE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrHpcPwrSlotPowerStatusGet(L7_uint32 slotNum,
                                     POWER_STATUS_t *pStatus,
                                     POWER_CONTROL_t *pSource)
{
  L7_RC_t rc = L7_FAILURE;
  rc = sysapiHpcSlotPowerStatusGet(slotNum, pStatus, pSource);
  return rc;
}

/*********************************************************************
 *
 * @purpose Turn ON / OFF power to given LM slot
 *
 * @param  L7_uint32    slotNum       Slot number
 *         L7_BOOL      enable        L7_TRUE to switch ON
 *                                    L7_FALSE to switch OFF
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Card Manager to switch ON power
 *        to the given LM slot
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrSlotPowerSet (L7_uint32 unit, L7_uint32 slot, POWER_STATUS_t enable,
                          POWER_CONTROL_t source)
{
  POWER_STATUS_t statusConfigured;
  POWER_CONTROL_t sourceConfigured;
  SYSAPI_CARD_TYPE_t moduleType;
  SYSAPI_CARD_INDEX_t moduleIndex;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL module_present = L7_FALSE;

  if (L7_SUCCESS != (sysapiHpcSlotMapMPModuleInfoGet(slot, &moduleType, &moduleIndex)))
  {
    LOG_MSG("cmgrSlotPowerSet: current module info get for slot %d - failed\n", slot);
    return L7_FAILURE;
  }

  if(moduleType == SYSAPI_CARD_TYPE_LINE)
  {
    if(L7_SUCCESS !=
       (cmgrHpcPwrSlotPowerStatusGet(slot, &statusConfigured,
                                     &sourceConfigured)))
    {
      LOG_MSG("cmgrSlotPowerSet: current status/source get for slot %d - failed\n", slot);
      return L7_FAILURE;
    }

    if((enable == statusConfigured) && (source == sourceConfigured))
    {
      return L7_ALREADY_CONFIGURED;
    }

    switch(sourceConfigured)
    {
      case POWER_ADMIN:
        {
          switch(statusConfigured)
          {
            case POWER_ON:
              rc = cmgrHpcPwrSlotPowerSet(slot, enable, source);
              break;

            case POWER_OFF:
              if(source != POWER_ADMIN)
              {
                cmgrAlarmRaise (moduleType, L7_ALARM_TYPE_SLOT_POWERED_OFF, moduleIndex);
                rc = L7_FAILURE;
              }
              else
              {
                rc = cmgrHpcPwrSlotPowerSet(slot, enable, source);
                /* Update LM LED status, if the slot is full */
                sysapiHpcIsModulePresent(slot, &module_present);
                if(module_present == L7_TRUE)
                {
                  sysapiHpcLedUpdate (slot, SYSAPI_CARD_EVENT_PLUG);
                }
              }
              break;

            default:
              LOG_MSG("cmgrSlotPowerSet: statis %d config state - unknown\n", statusConfigured);
              break;
          }
        }
        break;

      case POWER_SYSTEM:
        rc = cmgrHpcPwrSlotPowerSet(slot, enable, source);
        break;

      default:
        LOG_MSG("cmgrSlotPowerSet: source %d config state - unknown\n", sourceConfigured);
        break;
    }
  }
  else
  {
    LOG_MSG("cmgrSlotPowerSet: moduleType %d invalid\n", moduleType);
    return L7_FAILURE;
  }

  CMGR_DEBUG(CMGR_LOG_FLAG,
             "cmgrSlotPowerSet: unit:%d slot:%d enable:%d source:%d rc:%d\n",
             unit, slot, enable, source, rc);
  return rc;
}

/*********************************************************************
 *
 * @purpose Reset the Module present in the given slot
 *
 * @param  L7_uint32    slotNum              (input)   Slot number
 *
 * @returns L7_SUCCESS,     if success
 * @returns L7_ERROR,       if parameters are invalid or other error
 *
 * @notes This function is called by the Card Manager to
 *        reset a module
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrModuleReset(L7_uint32 unit, L7_uint32 slot)
{
  L7_RC_t rc = L7_FAILURE;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  if ((rc == sysapiHpcPwrModuleReset (slot)))
  {
    (void)cmgrCMDBSlotIsFullSet (unit, slot, L7_FALSE);
    (void)cmgrCMDBCardInsertedCardIdSet (unit, slot, L7_CMGR_CARD_ID_NONE);
    (void)cmgrCMDBCardStatusSet (unit, slot, L7_CARD_STATUS_UNKNOWN);
    (void)cmgrCMDBCardTypeSet (unit, slot, SYSAPI_CARD_TYPE_INVALID);
    (void)cmgrCMDBModuleIndexSet (unit, slot, SYSAPI_CARD_INDEX_INVALID);
  }
  return rc;
}

/*********************************************************************
 *
 * @purpose  Send Card unplug notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 *
 * @returns
 *
 * @notes    This API is called to notify registrants when a card
 *           has been unpluggd.
 *           The function enqueues the command to the card insert
 *           remove task.
 *
 * @end
 *********************************************************************/
void cmgrAsyncCardUnPlugNotify(L7_uint32 unit, L7_uint32 slot,
                               L7_uint32 cardTypeId)
{
  cmgrCardInsertRemoveMsg_t cmd;
  L7_RC_t rc;

  cmd.action = L7_CMGR_CARD_REMOVE;
  cmd.unit = unit;
  cmd.slot = slot;
  cmd.cardId = cardTypeId;

  rc = osapiMessageSend(cmgrCardInsertRemoveQueue,
                        (void *)&cmd,
                        sizeof(cmgrCardInsertRemoveMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
}

/*********************************************************************
 *
 * @purpose  Send Card unplug notification
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 *
 * @returns
 *
 * @notes    This API is called to notify registrants when a card
 *           has been unpluggd.  If possible, this should not be
 *           called within the card manager database semaphore.
 *
 * @end
 *********************************************************************/
void cmgrCardUnPlugNotify(L7_uint32 unit, L7_uint32 slot,
                          L7_uint32 cardTypeId)
{
  L7_uint32 port = 0;
  SYSAPI_HPC_CARD_DESCRIPTOR_t* cardTypeDb;
  L7_RC_t rc;

  cardTypeDb = sysapiHpcCardDbEntryGet(cardTypeId);

  if (cardTypeDb != L7_NULLPTR)
  {
    /* Only send port change messages if in steady state */
    if (((cmgrState == L7_CNFGR_STATE_E) ||
         (cmgrState == L7_CNFGR_STATE_U1)) &&
        (cardTypeDb->type == SYSAPI_CARD_TYPE_LINE))
    {
      if (cmgrCMDBPortInsertNotifiedGet (unit,slot) == L7_TRUE)
      {
        (void) dtlCardCmd((L7_ushort16)unit, (L7_ushort16)slot,
                          DTL_CARD_UNPLUG_START, (L7_ulong32)cardTypeId);
        for (port = 1; port <= cardTypeDb->numOfNiPorts; port ++)
          (void)cmgrDoNotifyPortChange(unit, slot, port,
                                       cardTypeId, L7_DETACH,
                                       &cardTypeDb->portInfo[port-1]);
        (void) dtlCardCmd((L7_ushort16)unit, (L7_ushort16)slot,
                          DTL_CARD_UNPLUG_FINISH, (L7_ulong32)cardTypeId);

        cmgrCMDBPortInsertNotifiedSet (unit,slot , L7_FALSE);
      }
    }

    if (((cmgrState == L7_CNFGR_STATE_E) ||
         (cmgrState == L7_CNFGR_STATE_U1)) &&
        (cardTypeDb->type == SYSAPI_CARD_TYPE_LOGICAL_CPU))
    {
      if (cmgrCMDBPortInsertNotifiedGet (unit,slot) == L7_TRUE)
      {
        cmgrDoNotifyPortChange(unit, slot, 1,
                               cardTypeId, L7_DETACH,
                               &cardTypeDb->portInfo[0]);
        osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);
        cmgrCMDBPortInsertNotifiedSet (unit,slot , L7_FALSE);
        osapiSemaGive(cmgrSemaId);
      }
    }

    if (cmgrCMDBCardInsertNotifiedGet (unit,slot) == L7_TRUE)
    {
      /* Don't tell anybody about logical cards.
       */
      if ((cardTypeDb->type == SYSAPI_CARD_TYPE_LINE) ||
          (cardTypeDb->type == SYSAPI_CARD_TYPE_FAN) ||
          (cardTypeDb->type == SYSAPI_CARD_TYPE_POWER_SUPPLY))
      {
        cmgrDoNotifyCardUnplug(unit, slot);
      }

      /* Only physical cards are removed from the hardware in this path.
      ** The logical cards are removed in cmgrLocalUnitIsNotManager ()
      */
      if (cardTypeDb->type == SYSAPI_CARD_TYPE_LINE)
      {
        rc = dtlCardCmd((L7_ushort16)unit, (L7_ushort16)slot, DTL_CARD_REMOVE,
                        (L7_ulong32)cardTypeId);
        if(rc != L7_SUCCESS)
        {
          CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardUnPlugNotify: error sending DTL_CARD_REMOVE for unit %d\n",unit);
          LOG_MSG("Error sending DTL_CARD_REMOVE for unit %d\n", unit);
        }
      }

      cmgrCMDBCardStatusSet(unit,slot, L7_CARD_STATUS_UNPLUG);
      cmgrCMDBCardInsertNotifiedSet (unit,slot , L7_FALSE);
    }
  }
  return;
}

/*********************************************************************
 *
 * @purpose  Send Unit Is Manager notification to the card insert task.
 *
 * @param    none
 *
 * @returns
 *
 * @notes
 *
 * @end
 *********************************************************************/
void cmgrAsyncTaskLocalUnitIsManager(void)
{
  cmgrCardInsertRemoveMsg_t cmd;
  L7_RC_t rc;

  cmd.action = L7_CMGR_LOCAL_UNIT_IS_MANAGER;
  cmd.unit = 0;
  cmd.slot = 0;
  cmd.cardId = 0;

  rc = osapiMessageSend(cmgrCardInsertRemoveQueue,
                        (void *)&cmd,
                        sizeof(cmgrCardInsertRemoveMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
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
 *        Since we need to wait until the logical cards are inserted
 *        before telling configurator to proceed to the next state,
 *        the configurator command is issued here.
 *
 * @end
 *
 *********************************************************************/
void cmgrTaskLocalUnitIsManager(void)
{
  L7_uint32 slot;
  L7_uint32 maxSlots;
  L7_uint32 card_id;
  L7_uint32 unit;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardTypeDb = L7_NULL;
  L7_RC_t   rc;
  L7_CNFGR_CMD_DATA_t cmdData;
  L7_CNFGR_CMD_DATA_t *pCmdData;

  pCmdData = &cmdData;

  maxSlots = CMGR_MAX_SLOTS_f;

  /* Insert only logical cards and local cards to the driver */
  for (slot = 0 ; slot <= maxSlots; slot++ )
  {
    /* Retreive info about the logical card from the local unit.
     */
    card_id = cmgrCMDBInsertedCardIdGet (L7_LOGICAL_UNIT, slot);
    if (card_id == L7_CMGR_CARD_ID_NONE)
    {
      continue;
    }

    cardTypeDb = sysapiHpcCardDbEntryGet(card_id);
    if (cardTypeDb == L7_NULL)
    {
      LOG_ERROR (card_id);
    }

    if ((cardTypeDb->type == SYSAPI_CARD_TYPE_LOGICAL_CPU) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_VLAN_ROUTER) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_LOOPBACK) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_TUNNEL) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_CAPWAP_TUNNEL) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_LAG))
    {
      CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_INSERT, L7_TRUE, L7_LOGICAL_UNIT, slot);
      rc = dtlCardCmd(L7_LOGICAL_UNIT, slot, DTL_CARD_INSERT, card_id);
      if (rc != L7_SUCCESS)
      {
        LOG_ERROR(card_id);
      }
      CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_INSERT, L7_FALSE, L7_LOGICAL_UNIT, slot);
    }
  }

  /* The unit number of the stack should be known by now */
  if (unitMgrNumberGet(&unit) != L7_SUCCESS)
  {
    LOG_ERROR (0);
  }

  for (slot = 0; slot <= maxSlots; slot++ )
  {
    /* Retreive local card information from unit 0
    ** The information will be copied to the appropriate
    ** unit in card manager's database in phase 3
    */
    card_id = cmgrCMDBInsertedCardIdGet (0, slot);
    if (card_id == L7_CMGR_CARD_ID_NONE)
    {
      continue;
    }

    cardTypeDb = sysapiHpcCardDbEntryGet(card_id);
    if (cardTypeDb == L7_NULL)
    {
      LOG_ERROR (card_id);
    }

    if (cardTypeDb->type == SYSAPI_CARD_TYPE_LINE)
    {
      /* Plug it in the stack unit */
      CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_INSERT, L7_TRUE, unit, slot);
      rc = dtlCardCmd(unit, slot, DTL_CARD_INSERT, card_id);
      if (rc != L7_SUCCESS)
      {
        LOG_ERROR(card_id);
      }
      CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_INSERT, L7_FALSE, unit, slot);
    }
  }

  cnfgrApiRegister((L7_VOIDFUNCPTR_t)cmgrInitCompleteCallBack, &cmgrInitCompleteHandle_g);
  pCmdData->cbHandle   = cmgrInitCompleteHandle_g;
  pCmdData->command    = L7_CNFGR_CMD_INITIALIZE;
  pCmdData->correlator = 0;
  pCmdData->type       = L7_CNFGR_EVNT;
  pCmdData->u.evntData.event = L7_CNFGR_EVNT_I_MGMT_UNIT;
  pCmdData->u.evntData.data  = L7_LAST_COMPONENT_ID;
  cnfgrApiCommand(pCmdData);
}

/*********************************************************************
 *
 * @purpose  Send Unit Is Not Manager notification to the card insert task.
 *
 * @param    none
 *
 * @returns
 *
 * @notes
 *
 * @end
 *********************************************************************/
void cmgrAsyncTaskLocalUnitIsNotManager(void)
{
  cmgrCardInsertRemoveMsg_t cmd;
  L7_RC_t rc;

  cmd.action = L7_CMGR_LOCAL_UNIT_IS_NOT_MANAGER;
  cmd.unit = 0;
  cmd.slot = 0;
  cmd.cardId = 0;

  rc = osapiMessageSend(cmgrCardInsertRemoveQueue,
                        (void *)&cmd,
                        sizeof(cmgrCardInsertRemoveMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
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
void cmgrTaskLocalUnitIsNotManager(void)
{
  L7_uint32 slot;
  L7_uint32 maxSlots;
  L7_uint32 card_id;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardTypeDb;

  maxSlots = CMGR_MAX_SLOTS_f;

  /* Remove only logical cards from the driver */
  for (slot = 0 ; slot <= maxSlots; slot++ )
  {
    /* Logical cards always live in unit 1.
     */
    card_id = cmgrCMDBInsertedCardIdGet (L7_LOGICAL_UNIT, slot);
    if (card_id == L7_CMGR_CARD_ID_NONE)
    {
      continue;
    }

    cardTypeDb = sysapiHpcCardDbEntryGet(card_id);
    if (cardTypeDb == L7_NULL)
    {
      LOG_ERROR (card_id);
    }

    if ((cardTypeDb->type == SYSAPI_CARD_TYPE_LOGICAL_CPU) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_VLAN_ROUTER) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_LOOPBACK) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_TUNNEL) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_CAPWAP_TUNNEL) ||
        (cardTypeDb->type == SYSAPI_CARD_TYPE_LAG))
    {
      (void)dtlCardCmd(L7_LOGICAL_UNIT, slot, DTL_CARD_REMOVE, card_id);
    }
  }
}

/*********************************************************************
 *
 * @purpose  Authenticate Card Type
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardType  card type
 *
 * @returns
 *
 * @notes    This API is called to authenticate the card type in this
 *           unit in this slot. If additional authentication is
 *           necessary, it can be added here.
 *
 * @end
 *********************************************************************/
L7_RC_t authenticateCardType(L7_uint32 unit, L7_uint32 slot, L7_uint32 cardType)
{
  L7_RC_t  rc;

  L7_uint32 unit_id;
  L7_uint32 supported_unit_index;
  L7_uint32 supported_unit_id;

  L7_uint32 supported_slot_index;
  L7_uint32 supported_slot;
  L7_uint32 ux;

  L7_uint32 supported_card_index;
  L7_uint32 supported_card_id;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *card_desc;

  /* Determine whether specified card is in the card database.
   */
  supported_card_index = 0;
  rc = usmDbCardSupportedIndexNextGet(&supported_card_index);
  while (rc == L7_SUCCESS)
  {
    rc = usmDbCardSupportedTypeIdGet(supported_card_index, &supported_card_id);
    if (rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (supported_card_id == cardType)
    {
      break;
    }
    rc = usmDbCardSupportedIndexNextGet(&supported_card_index);
  }
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* If this is a logical card then approve it.
   */
  card_desc = sysapiHpcCardDbEntryByIndexGet(supported_card_index);
  if ((card_desc != L7_NULL) && (card_desc->type != SYSAPI_CARD_TYPE_LINE))
  {
    return L7_SUCCESS;
  }

  /* Determine what type of unit the card is plugged in.
   */
  rc = unitMgrUnitTypeGet(unit, &unit_id);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Find the supported-unit index for this unit.
   */
  supported_unit_index = 0;
  rc = usmDbUnitMgrSupportedUnitIdNextGet(&supported_unit_index, &supported_unit_id);
  while (rc == L7_SUCCESS)
  {
    if (supported_unit_id == unit_id)
    {
      break;
    }
    rc = usmDbUnitMgrSupportedUnitIdNextGet(&supported_unit_index, &supported_unit_id);
  }
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Find supported-slot index for the specified slot.
   */
  supported_slot_index = 0;
  ux = supported_unit_index;
  rc = usmDbSlotSupportedSlotNumberNextGet(&ux, &supported_slot_index, &supported_slot);
  while (rc == L7_SUCCESS)
  {
    if (ux != supported_unit_index)
    {
      return L7_FAILURE;
    }

    if (supported_slot == slot)
    {
      break;
    }

    rc = usmDbSlotSupportedSlotNumberNextGet(&ux, &supported_slot_index, &supported_slot);
  }
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* We now have the unit index, slot index, and card index.
  ** Last step is to verify whether the specified card is supported
  ** in the specified unit and specified slot.
  */
  rc = usmDbCardInSlotSupportedGet(supported_unit_index,
                                   supported_slot_index,
                                   supported_card_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  Authenticate Board Id
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 phySlot   physical slot
 * @param    L7_uint32 cardType  Board Id
 *
 * @returns
 *
 * @notes    This API is called to authenticate the board Id in this
 *           unit in this slot. In case of authentication failure
 *           of a Line Module, the power will be turned OFF immediately
 *           to that slot.
 *
 * @end
 *********************************************************************/
L7_RC_t authenticateBoardId(L7_uint32 unit, L7_uint32 phySlot,
                            L7_uint32 boardId, SYSAPI_CARD_TYPE_t cardType,
                            L7_int32 cardIndex)
{
  L7_RC_t  rc = L7_FAILURE;
  L7_uint32 supported_card_index;
  L7_uint32 supported_card_id;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *pCardDescriptor;

  /* Determine whether specified card is in the card database.
   */
  supported_card_index = 0;

  rc = sysapiHpcCardDbEntryIndexNextGet (&supported_card_index);

  while (rc == L7_SUCCESS)
  {
    if ((pCardDescriptor = sysapiHpcCardDbEntryByIndexGet(supported_card_index)) != L7_NULLPTR)
    {
      supported_card_id = pCardDescriptor->cardTypeId;
      rc = L7_SUCCESS;
    }
    else
    {
      CMGR_DEBUG(CMGR_LOG_FLAG,
                 "authenticateBoardId: unit:%d phySlot:%d Card Descriptor Failure\n",
                 unit, phySlot);
      LOG_MSG("Card Descriptor Failure: unit:%d phySlot:%d\n", unit, phySlot);
      return L7_FAILURE;
    }

    if (supported_card_id == boardId)
    {
      break;
    }
    rc = sysapiHpcCardDbEntryIndexNextGet (&supported_card_index);
  }

  if (rc != L7_SUCCESS)
  {
    CMGR_DEBUG(CMGR_LOG_FLAG,
               "authenticateBoardId: unit:%d phySlot:%d boardId:0x%x Board Id mismatch !!!\n",
               unit, phySlot, boardId);

    /* In case of Board Id mismatch, turn OFF power to the LM slot */
    if(cardType == SYSAPI_CARD_TYPE_LINE)
    {
      CMGR_DEBUG(CMGR_LOG_FLAG, "Turning OFF the slot power...\n");
      cmgrSlotPowerSet(unit, phySlot, POWER_OFF, POWER_SYSTEM);
    }

    /* raise module mismatch alarm */
    cmgrAlarmRaise(cardType, L7_ALARM_TYPE_MODULE_MISMATCH, cardIndex);

    /* update Led to indicate board Id mismatch */
    sysapiHpcLedUpdate(phySlot, SYSAPI_CARD_EVENT_VER_MISMATCH);

    return L7_ERROR;
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose  Card Unsupported
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 cardType  card type
 *
 * @returns
 *
 * @notes    This API is sets the card status and calls trap manager if
 *           a card is plugged in whose type is unsupported on this unit.
 *           If additional action needs to be taken in this event,
 *           it can be added here.
 *
 * @end
 *********************************************************************/
void cmgrCardUnsupported(L7_uint32 unit, L7_uint32 slot, L7_uint32 cardTypeId)
{
  (void)cmgrCMDBCardStatusSet(unit, slot, L7_CARD_STATUS_UNSUPPORTED);

  cmgrNotifyRegisteredUsers(unit, slot, cardTypeId, 0, TRAPMGR_CARD_UNSUPPORTED_EVENT);

  cmgrAlarmRaise(cmgrCMDBCardTypeGet(unit,slot),
                 L7_ALARM_TYPE_MODULE_UNSUPPORTED,
                 cmgrCMDBModuleIndexGet(unit,slot));

  return;
}

/*********************************************************************
 *
 * @purpose  Clear the Card Unsupported alarm
 *
 * @param    L7_uint32 phySlot   physical slot
 *
 * @returns  L7_SUCCESS          if success
 *           L7_FAILURE          if alarm could not be raised
 *
 * @notes    This API clears the cardUnsupported alarm on a successful
 *           card plug-in
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCardUnSupportedAlarmClear(L7_uint32 phySlot)
{
  L7_uchar8 addInfo[CMGR_ALARM_DESC_SIZE];
  L7_RC_t rc = L7_SUCCESS;

  /* time at which this alarm is being raised */
  L7_clocktime alarmTimeStamp;

  /* alarm Id not known yet */
  L7_ALARM_ID_t alarmId = L7_ALARM_ID_UNKNOWN;

  bzero(addInfo, CMGR_ALARM_DESC_SIZE);

  switch (phySlot)
  {
    case 0:
      alarmId = L7_ALARM_ID_SLOT0_MODULE_UNSUPPORTED;
      OSAPI_STRNCPY_SAFE (addInfo, "Unsupported Module in slot 0");
      break;
    case 1:
      alarmId = L7_ALARM_ID_SLOT1_MODULE_UNSUPPORTED;
      OSAPI_STRNCPY_SAFE (addInfo, "Unsupported Module in slot 1");
      break;
    case 2:
      alarmId = L7_ALARM_ID_SLOT2_MODULE_UNSUPPORTED;
      OSAPI_STRNCPY_SAFE (addInfo, "Unsupported Module in slot 2");
      break;
    case 3:
      alarmId = L7_ALARM_ID_SLOT3_MODULE_UNSUPPORTED;
      OSAPI_STRNCPY_SAFE (addInfo, "Unsupported Module in slot 3");
      break;
    case 4:
      alarmId = L7_ALARM_ID_SLOT4_MODULE_UNSUPPORTED;
      OSAPI_STRNCPY_SAFE (addInfo, "Unsupported Module in slot 4");
      break;
    case 5:
      alarmId = L7_ALARM_ID_SLOT5_MODULE_UNSUPPORTED;
      OSAPI_STRNCPY_SAFE (addInfo, "Unsupported Module in slot 5");
      break;
    default:
      CMGR_DEBUG (CMGR_LOG_FLAG, "cmgrCardUnSupportedAlarmClear: Unexpected slot %d\n",
                  phySlot);
      LOG_MSG("cmgrCardUnSupportedAlarmClear: Unexpected slot %d\n", phySlot);
      break;
  }

  if (alarmId != L7_ALARM_ID_UNKNOWN)
  {
    /* Get current timestamp */
    osapiUTCTimeGet (&alarmTimeStamp);
    /* raise alarm */
    rc = alarmMgrAlarmClear(alarmId, alarmTimeStamp, addInfo);
  }

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardUnSupportedAlarmClear: alarmId:%d Info:%s\n", alarmId, addInfo);
  return rc;
}

/*********************************************************************
 *
 * @purpose  Card Mismatch
 *
 * @param    L7_uint32 unit      Unit
 * @param    L7_uint32 slot      slot
 * @param    L7_uint32 configCardType  Configured card type
 * @param    L7_uint32 insertedCardType  Inserted card type
 *
 * @returns
 *
 * @notes    This API is sets the card status and calls trap manager if
 *           a card is plugged in whose type doesn't match the card type
 *           configured in this slot on this unit.
 *           If additional action needs to be taken in this event,
 *           it can be added here.
 *
 * @end
 *********************************************************************/
void cmgrCardMisMatch(L7_uint32 unit, L7_uint32 slot, L7_uint32 configCardType,
                      L7_uint32 insertedCardType)
{
  (void)cmgrCMDBCardStatusSet(unit, slot, L7_CARD_STATUS_MISMATCH);

  cmgrNotifyRegisteredUsers(unit, slot, insertedCardType,
                            configCardType, TRAPMGR_CARD_MISMATCH_EVENT);
  return;
}

/*********************************************************************
 *
 * @purpose  Create a CMPDU from Cnfgr data
 *
 * @param    L7_uint32 pCmdData      Configurator data
 * @param    L7_uint32 cmpdu         cmpdu to return
 *
 * @returns  L7_RC_t L7_SUCCESS or L7_ERROR
 *
 * @notes    This function is used to create a cmpdu to be sent to the
 *           card manager queue with data from the configuator.
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrMakeCnfgrCmpdu(L7_CNFGR_CMD_DATA_t *pCmdData,
                           cmgr_cmpdu_t *cmpdu)
{
  if ( cmpdu == L7_NULLPTR )
  {
    return L7_ERROR;
  }
  (void)unitMgrNumberGet(&cmpdu->unit);
  cmpdu->cmgrPacketType = L7_CMPDU_CNFGR_REQUEST;
  (void)memcpy(&(cmpdu->cmpdu_data.cmdData), pCmdData, sizeof (L7_CNFGR_CMD_DATA_t));

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  Send message using HPC transport
 *
 * @param    L7_uint32 pCmdData      Configurator data
 * @param    L7_uint32 cmpdu         cmpdu to return
 *
 * @returns  L7_RC_t L7_SUCCESS or L7_ERROR
 *
 * @notes    This function is used to send a message using HPC.
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrSendCmpdu(cmgr_cmpdu_t *cmpdu, L7_uint32 size)
{
  if (cmpdu == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  /* send cmpdu to HSSL */
  if (sysapiHpcBcastTransportIsReady() == L7_TRUE)
  {
    return sysapiHpcBroadcastMessageSend(L7_CARDMGR_COMPONENT_ID, size, (L7_uchar8 *)cmpdu);
  }
  return L7_ERROR;
}

/*****************************************************************************/
/* Callback Notify APIs                                                      */
/*****************************************************************************/

/*********************************************************************
 * @purpose  go through registered users and notify them of card creation.
 *
 * @param    unit        unit
 * @param    slot        slot
 * @param    cardId
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cmgrDoNotifyCardCreate(L7_uint32 unit, L7_uint32 slot,
                            L7_uint32 cardId)
{
  L7_RC_t   rc;
  L7_uint32 i;

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if ((cmgrNotifyList[i].registrar_ID != L7_NULL) &&
        (cmgrNotifyList[i].notify_card_create != L7_NULLPTR))
    {
      rc = (*cmgrNotifyList[i].notify_card_create)(unit, slot, cardId);
    }
  }
}

/*********************************************************************
 * @purpose  go through registered users and notify them of card clear.
 *
 * @param    unit        unit
 * @param    slot        slot
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cmgrDoNotifyCardClear(L7_uint32 unit, L7_uint32 slot)
{
  L7_RC_t   rc;
  L7_uint32 i;

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if ((cmgrNotifyList[i].registrar_ID != L7_NULL) &&
        (cmgrNotifyList[i].notify_card_clear != L7_NULLPTR))
    {
      rc = (*cmgrNotifyList[i].notify_card_clear)(unit, slot);
    }
  }
}

/*********************************************************************
 * @purpose  go through registered users and notify them of card plugin.
 *
 * @param    unit        unit
 * @param    slot        slot
 * @param    cardId
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cmgrDoNotifyCardPlugin(L7_uint32 unit, L7_uint32 slot,
                            L7_uint32 cardId)
{
  L7_RC_t   rc;
  L7_uint32 i;

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if ((cmgrNotifyList[i].registrar_ID != L7_NULL) &&
        (cmgrNotifyList[i].notify_card_plugin != L7_NULLPTR))
    {
      rc = (*cmgrNotifyList[i].notify_card_plugin)(unit, slot, cardId);

      if(rc != L7_SUCCESS)
        LOG_MSG("cmgrDoNotifyCardPlugin: notify unit:%d slot:%d - failed\n", unit, slot);
      else
        CMGR_DEBUG(CMGR_LOG_FLAG,
                   "cmgrDoNotifyCardPlugin: Notification sent to unit:%d slot:%d\n",
                   unit, slot);
    }
  }
}

/*********************************************************************
 * @purpose  go through registered users and notify them of card unplug.
 *
 * @param    unit        unit
 * @param    slot        slot
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cmgrDoNotifyCardUnplug(L7_uint32 unit, L7_uint32 slot)
{
  L7_RC_t   rc;
  L7_uint32 i;

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if ((cmgrNotifyList[i].registrar_ID != L7_NULL) &&
        (cmgrNotifyList[i].notify_card_unplug != L7_NULLPTR))
    {
      rc = (*cmgrNotifyList[i].notify_card_unplug)(unit, slot);
    }
  }
}

/*********************************************************************
 * @purpose  go through registered users and notify them of port change.
 *
 * @param    unit        unit
 * @param    slot        slot
 * @param    port        port
 * @param    cardType    type of card
 * @param    event       port create, attach, detach, delete
 * @param    portType    type of port
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrDoNotifyPortChange(L7_uint32 unit, L7_uint32 slot, L7_uint32 port,
                               L7_uint32 cardType, L7_PORT_EVENTS_t event,
                               SYSAPI_HPC_PORT_DESCRIPTOR_t *portData)
{
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t handle;
  nimUSP_t usp;
  L7_uint32 intIfNum;
  L7_RC_t rc = L7_ERROR;

  switch ( event )
  {
    case L7_CREATE:
      rc = cmgrDoNotifyPortCreate(unit,slot,port, cardType, portData);
      break;

    case L7_ATTACH:
    case L7_DETACH:
    case L7_DELETE:

      /* Don't attach  or detach ports that are operating in stack mode.
       */
      if (((event == L7_ATTACH) || (event == L7_DETACH)) &&
          (spmFpsPortStackingModeCheck (unit, slot, port) == L7_TRUE))
      {
        rc = L7_SUCCESS;
        break;
      }

      usp.unit = unit;
      usp.slot = slot;
      usp.port = port;

      if ((rc = nimGetIntIfNumFromUSP(&usp, &intIfNum)) == L7_SUCCESS)
      {
        eventInfo.component = L7_CARDMGR_COMPONENT_ID;
        eventInfo.event = event;
        eventInfo.intIfNum = intIfNum;
        eventInfo.pCbFunc = cmgrNimSyncSemGive;
        rc = nimEventIntfNotify(eventInfo,&handle);

        /* Wait for NIM to notify everybody about the the event.
         */
        cmgrNimSyncSemGet ();
      }

      break;

    default:
      break;
  }

  if (rc != L7_SUCCESS)
    rc = L7_ERROR;

  return rc;

#if 0
  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if ((cmgrNotifyList[i].registrar_ID != L7_NULL) &&
        (cmgrNotifyList[i].notify_port_change != L7_NULLPTR))
    {
      rc = (*cmgrNotifyList[i].notify_port_change)(unit, slot, port,
                                                   cardType, event,
                                                   portData);
    }
  }
#endif
}

/*********************************************************************
 * @purpose  Issue port create.
 *
 * @param    unit        unit
 * @param    slot        slot
 * @param    port        port
 * @param    cardType    type of card
 * @param    portType    type of port
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrDoNotifyPortCreate(L7_uint32 unit,
                               L7_uint32 slot,
                               L7_uint32 port,
                               L7_uint32 cardType,
                               SYSAPI_HPC_PORT_DESCRIPTOR_t *portData)
{
  L7_RC_t  rc = L7_ERROR;
  nimIntfCreateRequest_t pRequest;
  L7_uint32   intIfNum;
  NIM_HANDLE_t  handle;
  NIM_INTF_CREATE_INFO_t eventInfo;
  NIM_EVENT_NOTIFY_INFO_t  notifyEventInfo;

  nimConfigID_t          pIntfIdInfo;  /* unique interface specification info */
  nimIntfDescr_t         pIntfDescr;   /* unique interface descriptor  info */
  nimIntfCreateOutput_t  output;

  CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrDoNotifyPortCreate: unit:%d slot:%d port:%d\n", unit, slot, port);

  output.handle =   &handle;
  output.intIfNum = &intIfNum;

  eventInfo.component = L7_CARDMGR_COMPONENT_ID;
  eventInfo.pCbFunc   = cmgrNimSyncSemGive;

  /* setup the config ID */
  pIntfIdInfo.configSpecifier.usp.unit = unit;
  pIntfIdInfo.configSpecifier.usp.slot = slot;
  pIntfIdInfo.configSpecifier.usp.port = port;

  pRequest.pDefaultCfg  = L7_NULLPTR;
  pRequest.pIntfDescr   = &pIntfDescr;
  pRequest.pIntfIdInfo  = &pIntfIdInfo;
  pRequest.pCreateInfo  = &eventInfo;

  switch (portData->type)
  {
    case  L7_IANA_OTHER_CPU:
      pIntfIdInfo.type = L7_CPU_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                                L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
                 " CPU Interface for Unit:", unit, "Slot:", slot, "Port:", port);

        sprintf (pIntfDescr.ifName, "%s %d/%d/%d",
                 "CPU Interface: ", unit, slot, port);
      }
      else
      {
        sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s %d %s %d",
                 " CPU Interface for", "Slot:", slot, "Port:", port);

        sprintf (pIntfDescr.ifName, "%s %d/%d",
                 "CPU Interface: ", slot, port);
      }
      break;

    case L7_IANA_LAG_DESC:
      pIntfIdInfo.type = L7_LAG_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                                L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                       unit, slot, port);
      }
      else
      {
        (void)sprintf (pIntfDescr.ifName, "%d/%d",
                       slot, port);
      }
      break;

    case  L7_IANA_ETHERNET:
    case  L7_IANA_FAST_ETHERNET:
    case  L7_IANA_FAST_ETHERNET_FX:
      pIntfIdInfo.type = L7_PHYSICAL_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                                L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
                       "Unit:", unit, "Slot:", slot, "Port:", port);

        (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                       unit, slot, port);
      }
      else
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d",
                       "Slot:", slot, "Port:", port);

        (void)sprintf (pIntfDescr.ifName, "%d/%d",
                       slot, port);
      }
      (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s",
                     (L7_char8 *)(pIntfDescr.ifDescr),
                     IANA_FAST_ETHERNET_DESC);
      break;

    case  L7_IANA_GIGABIT_ETHERNET:
    case  L7_IANA_2G5BIT_ETHERNET:
      pIntfIdInfo.type = L7_PHYSICAL_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                                L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
                       "Unit:", unit, "Slot:", slot, "Port:", port);

        (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                       unit, slot, port);
      }
      else
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d",
                       "Slot:", slot, "Port:", port);

        (void)sprintf (pIntfDescr.ifName, "%d/%d",
                       slot, port);
      }
      (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s",
                     (L7_char8 *)(pIntfDescr.ifDescr),
                     IANA_GIGABIT_ETHERNET_DESC);
      break;

    case  L7_IANA_10G_ETHERNET:
      pIntfIdInfo.type = L7_PHYSICAL_INTF;
      if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                                L7_STACKING_FEATURE_SUPPORTED) == L7_TRUE)
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d %s %d",
                       "Unit:", unit, "Slot:", slot, "Port:", port);

        (void)sprintf (pIntfDescr.ifName, "%d/%d/%d",
                       unit, slot, port);
      }
      else
      {
        (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %d %s %d",
                       "Slot:", slot, "Port:", port);

        (void)sprintf (pIntfDescr.ifName, "%d/%d",
                       slot, port);
      }
      (void)sprintf ((L7_char8 *)(pIntfDescr.ifDescr), "%s %s",
                     (L7_char8 *)(pIntfDescr.ifDescr),
                     IANA_10G_ETHERNET_DESC);
      break;

    case L7_IANA_L2_VLAN:
      pIntfIdInfo.type = L7_LOGICAL_VLAN_INTF;
      break;
    default:
      return L7_ERROR;
  }

  if ((rc = cmgrIfaceMacGet((L7_INTF_TYPES_t)pIntfIdInfo.type, (L7_uint32)unit,(L7_uint32)slot,port,pIntfDescr.macAddr.addr)) != L7_SUCCESS)
    return  rc;

  pIntfDescr.configurable   = L7_TRUE;
  pIntfDescr.settableParms  = L7_INTF_PARM_ADMINSTATE | L7_INTF_PARM_MTU |
    L7_INTF_PARM_MACADDR | L7_INTF_PARM_LINKTRAP |
    L7_INTF_PARM_LOOPBACKMODE |
    L7_INTF_PARM_MACROPORT | L7_INTF_PARM_ENCAPTYPE;
  switch (pIntfIdInfo.type)
  {
    case L7_PHYSICAL_INTF:
      pIntfDescr.settableParms |= L7_INTF_PARM_AUTONEG | L7_INTF_PARM_SPEED |
        L7_INTF_PARM_FRAMESIZE ;
      break;

    case L7_LAG_INTF:
      pIntfDescr.settableParms |= L7_INTF_PARM_FRAMESIZE;
      break;

    default:
      /* do nothing */
      break;
  }

  pIntfDescr.connectorType  = portData->connectorType;
  pIntfDescr.defaultSpeed   =  portData->defaultSpeed;
  pIntfDescr.frameSize.largestFrameSize = L7_MAX_FRAME_SIZE;
  pIntfDescr.ianaType       = portData->type;
  pIntfDescr.internal       = L7_FALSE;
  pIntfDescr.phyCapability  = portData->phyCapabilities;
  memset((void*)&pIntfDescr.macroPort,0,sizeof(nimMacroPort_t));
  memcpy (&pIntfDescr.bcastMacAddr,  &L7_ENET_BCAST_MAC_ADDR, 6);

  if (nimIntfCreate(&pRequest,&output) != L7_SUCCESS)
  {
    rc = L7_ERROR;
  }
  else
  {
    notifyEventInfo.component = L7_CARDMGR_COMPONENT_ID;
    notifyEventInfo.pCbFunc   = cmgrNimSyncSemGive;
    notifyEventInfo.event = L7_CREATE;
    notifyEventInfo.intIfNum = intIfNum;
    rc = nimEventIntfNotify(notifyEventInfo,&handle);
    if (rc != L7_SUCCESS)
    {
      LOG_ERROR (rc);
    }

    /* Wait for NIM to notify everybody about the the event.
     */
    cmgrNimSyncSemGet ();
  }

  return rc;
}

/*****************************************************************************/
/*  Data Access Functions - Initialize Data                                  */
/*****************************************************************************/

/*********************************************************************
 * @purpose  Initialize Card Manager Database.
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cmgrDatabaseInit()
{
  L7_uint32 unit = 0;
  L7_uint32 maxUnits = 0;
  L7_uint32 maxSlots = 0;
  L7_RC_t rc;
  L7_uint32   tempsize;
  cmgr_unit_t * unitDb;

  L7_uint32 local_unit_index;
  L7_uint32 local_unit_id;
  L7_uint32 local_unit_number;

  maxUnits = CMGR_MAX_UNITS_f;
  maxSlots = CMGR_MAX_SLOTS_f;

  cmUnit = (cmgr_unit_t *)osapiMalloc(L7_CARDMGR_COMPONENT_ID, sizeof (cmgr_unit_t) * (maxUnits + 1));

  for (unit=0; unit <= maxUnits; unit++)
  {
    unitDb = &cmUnit[unit];
    if (unitDb == NULL)
      continue;

    unitDb->unitConnected = L7_FALSE;
    unitDb->unitConfigured = L7_FALSE;
    unitDb->maxSlotsOnUnit = 0;
    tempsize = sizeof (cmgr_slot_t) * (maxSlots + 1);
    unitDb->cmSlot =
      (cmgr_slot_t *)osapiMalloc(L7_CARDMGR_COMPONENT_ID, tempsize);
  }

  /* Set up local unit.
  ** The local unit configuration is always stored in unit 0 record.
  */
  unitDb = &cmUnit[0];
  rc = sysapiHpcUnitDbEntryLocalUnitIndexGet (&local_unit_index);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);  /* Local unit must always be defined */
  }

  rc = sysapiHpcUnitDbUnitIdGet (local_unit_index, &local_unit_id);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);  /* Local unit must always be defined */
  }

  /* Configure the local unit. The local unit is always configured
  ** and is always connected.
  */

  /* Unit number 0 is always configured because this is where
  ** slot information is stored for non-management unit.
  */
  local_unit_number = 0;
  (void) cmgrUnitConfig (local_unit_number, local_unit_id);
  (void) cmgrUnitConnected (local_unit_number);

  /* Check whether local unit number is already known. On stand-alone
  ** devices the local unit number is always defined.
  ** On stackable devices the local unit number is set to 0. The local
  ** unit will be set later by the unit manager.
  */
  rc = unitMgrNumberGet (&local_unit_number);
  if (rc == L7_SUCCESS)
  {
    (void) cmgrUnitConfig (local_unit_number, local_unit_id);
    (void) cmgrUnitConnected (local_unit_number);
  }

  return;
}

/*********************************************************************
 * @purpose  Initialize slots for a unit in the Card Manager Database.
 *
 * @param    unit    unit
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrInitSlotsOnUnit(L7_uint32 unit)
{
  L7_uint32 slot = 0;
  cmgr_unit_t * unitDb;
  cmgr_slot_t* slotDb;
  L7_uint32 i, sx;
  L7_uint32 supported_card = L7_CMGR_CARD_ID_NONE;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t unit_entry;
  L7_uint32 phySlot = 0;
  SYSAPI_CARD_TYPE_t moduleType;
  SYSAPI_CARD_INDEX_t moduleIndex;

  unitDb = &cmUnit[unit];
  if (unitDb == NULL)
  {
    LOG_ERROR (unit);
  }

  if(L7_SUCCESS != (sysapiHpcSlotMapUSPhysicalSlotGet(unit, &phySlot)))
  {
    LOG_MSG("cmgrInitSlotsOnUnit: phySlot get for unit %d - failed\n", unit);
    return L7_FAILURE;
  }

  if(L7_SUCCESS != (sysapiHpcSlotMapMPModuleInfoGet(phySlot, &moduleType,
                                                    &moduleIndex)))
  {
    LOG_MSG("cmgrInitSlotsOnUnit: module type/index get for phySlot %d - failed\n", phySlot);
    return L7_FAILURE;
  }

  for (slot = 0; slot <= cmgrCMDBUnitMaxSlotsGet(unit); slot++)
  {
    slotDb = &unitDb->cmSlot[slot];
    if (slotDb == NULL)
    {
      LOG_ERROR (slot);
    }

    slotDb->slotId = slot;
    slotDb->isFull = L7_FALSE;
    slotDb->isValid = L7_FALSE;
    slotDb->adminMode = L7_ENABLE;
    slotDb->powerMode = L7_ENABLE;
    slotDb->cardStatus = L7_CARD_STATUS_UNKNOWN;
    slotDb->insertedCardId = L7_CMGR_CARD_ID_NONE;
    slotDb->cardType = SYSAPI_CARD_TYPE_INVALID;
    slotDb->cardIndex = SYSAPI_CARD_INDEX_INVALID;

    memset(&unit_entry, 0, sizeof(SYSAPI_HPC_UNIT_DESCRIPTOR_t));
    if (cmgrUnitEntryAndSlotIndexGet(unit, slot, &sx, &unit_entry) != L7_SUCCESS)
    {
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

      (void)cmgrCMDBCardConfigCardIdSet(unit, slot, supported_card);

      if (unit > 0)
      {
        cmgrAsyncCardCreateNotify(unit, slot, supported_card);
      }
    }
    else
    {
      slotDb->configuredCardId = L7_CMGR_CARD_ID_NONE;
    }
  }
  return L7_SUCCESS;
}

/*****************************************************************************/
/*  Data Access Functions - Raw Data Access                                  */
/*****************************************************************************/


/*********************************************************************
 * @purpose  Database Access - get unit connected.
 *
 * @param    unit    unit
 *
 * @returns  boolean True if unit connected, false otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBUnitConnectedGet(L7_uint32 unit)
{
  if (unit > CMGR_MAX_UNITS_f)
    return L7_FALSE;

  return(&cmUnit[unit])->unitConnected;
}

/*********************************************************************
 * @purpose  Database Access - set unit connected.
 *
 * @param    unit    unit
 * @param    value   L7_TRUE if unit connected, L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBUnitConnectedSet(L7_uint32 unit, L7_BOOL value)
{
  if (unit > CMGR_MAX_UNITS_f)
    return L7_ERROR;

  (&cmUnit[unit])->unitConnected = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get unit configured.
 *
 * @param    unit    unit
 *
 * @returns  L7_BOOL L7_TRUE if unit configured; L7_FALSE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBUnitConfiguredGet(L7_uint32 unit)
{
  if (unit > CMGR_MAX_UNITS_f)
    return L7_FALSE;

  return(&cmUnit[unit])->unitConfigured;
}

/*********************************************************************
 * @purpose  Database Access - set unit configured.
 *
 * @param    unit    unit
 * @param    value   L7_TRUE if unit configured, L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBUnitConfiguredSet(L7_uint32 unit, L7_BOOL value)
{
  if (unit > CMGR_MAX_UNITS_f)
    return L7_ERROR;

  (&cmUnit[unit])->unitConfigured = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get unit type.
 *
 * @param    unit    unit
 *
 * @returns  L7_uint32 unit type.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBUnitTypeGet(L7_uint32 unit)
{
  if (unit > CMGR_MAX_UNITS_f)
    return L7_FALSE;

  return(&cmUnit[unit])->unitType;
}

/*********************************************************************
 * @purpose  Database Access - set unit unit type.
 *
 * @param    unit    unit
 * @param    value   unit type
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBUnitTypeSet(L7_uint32 unit, L7_uint32 value)
{
  if (unit > CMGR_MAX_UNITS_f)
    return L7_ERROR;

  (&cmUnit[unit])->unitType = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get unit maximum slots.
 *
 * @param    unit    unit
 *
 * @returns  L7_uint32  max slots for this unit.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBUnitMaxSlotsGet(L7_uint32 unit)
{
  if (unit > CMGR_MAX_UNITS_f)
    return L7_ERROR;

  return(&cmUnit[unit])->maxSlotsOnUnit;
}

/*********************************************************************
 * @purpose  Database Access - set unit max slots.
 *
 * @param    unit    unit
 * @param    value   maximum number of slots available for this unit
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBUnitMaxSlotsSet(L7_uint32 unit, L7_uint32 value)
{
  if (unit > CMGR_MAX_UNITS_f)
    return L7_ERROR;

  (&cmUnit[unit])->maxSlotsOnUnit = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get slot id.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  slot id for this slot on this unit.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBSlotIdGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return 0;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->slotId;
}

/*********************************************************************
 * @purpose  Database Access - set slot id.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   slot ID value
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBSlotIdSet(L7_uint32 unit, L7_uint32 slot, L7_uint32 value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  if (value > cmgrCMDBUnitMaxSlotsGet(unit))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->slotId = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get slot id.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  slot id for this slot on this unit.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBPhySlotIdGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return 0;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->phySlotId;
}

/*********************************************************************
 * @purpose  Database Access - set slot id.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   slot ID value
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBPhySlotIdSet(L7_uint32 unit, L7_uint32 slot, L7_uint32 value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  if (value > cmgrCMDBUnitMaxSlotsGet(unit))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->phySlotId = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get slot validity.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if this is a valid slot on this unit;
 *                    L7_FALSE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBSlotIsValid(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->isValid;
}

/*********************************************************************
 * @purpose  Database Access - get slot full status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if a card is inserted in this slot on
 *                    this unit; L7_FALSE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBSlotIsFull(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->isFull;
}

/*********************************************************************
 * @purpose  Database Access - set slot slot full status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   L7_TRUE if slot is full; L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBSlotIsFullSet(L7_uint32 unit, L7_uint32 slot, L7_BOOL value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->isFull = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get portCreateNotified  status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if a card is inserted in this slot on
 *                    this unit; L7_FALSE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBPortCreateNotifiedGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->portCreateNotified;
}

/*********************************************************************
 * @purpose  Database Access - set portCreateNotified status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   L7_TRUE if slot is full; L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBPortCreateNotifiedSet(L7_uint32 unit, L7_uint32 slot, L7_BOOL value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->portCreateNotified = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get portInsertNotified  status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if a card is inserted in this slot on
 *                    this unit; L7_FALSE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBPortInsertNotifiedGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->portInsertNotified;
}

/*********************************************************************
 * @purpose  Database Access - set portInsertNotified status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   L7_TRUE if slot is full; L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBPortInsertNotifiedSet(L7_uint32 unit, L7_uint32 slot, L7_BOOL value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->portInsertNotified = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get cardCreateNotified  status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if a card is inserted in this slot on
 *                    this unit; L7_FALSE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBCardCreateNotifiedGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->cardCreateNotified;
}

/*********************************************************************
 * @purpose  Database Access - set cardCreateNotified status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   L7_TRUE if slot is full; L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardCreateNotifiedSet(L7_uint32 unit, L7_uint32 slot, L7_BOOL value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->cardCreateNotified = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get cardInsertNotified  status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_BOOL  L7_TRUE if a card is inserted in this slot on
 *                    this unit; L7_FALSE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrCMDBCardInsertNotifiedGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->cardInsertNotified;
}

/*********************************************************************
 * @purpose  Database Access - set cardInsertNotified status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   L7_TRUE if slot is full; L7_FALSE otherwise
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardInsertNotifiedSet(L7_uint32 unit, L7_uint32 slot, L7_BOOL value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->cardInsertNotified = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get slot administrative mode.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  administrative mode.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBConfiguredAdminModeGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->adminMode;
}

/*********************************************************************
 * @purpose  Database Access - set slot admin mode.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   administrative mode value
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBSlotConfiguredAdminModeSet(L7_uint32 unit, L7_uint32 slot, L7_uint32 value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  if ((value != L7_ENABLE) && (value != L7_DISABLE))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->adminMode = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get slot power mode.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  power mode.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBConfiguredPowerModeGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->powerMode;
}

/*********************************************************************
 * @purpose  Database Access - set slot power mode.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   slot power mode
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBSlotConfiguredPowerModeSet(L7_uint32 unit, L7_uint32 slot,
                                           L7_uint32 value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  if ((value != L7_ENABLE) && (value != L7_DISABLE))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->powerMode = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get card status.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  card status
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBCardStatusGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->cardStatus;
}

/*********************************************************************
 * @purpose  Database Access - set card status.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   card status
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardStatusSet(L7_uint32 unit, L7_uint32 slot, L7_uint32 value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->cardStatus = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get slot inserted card identifier.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  inserted card type
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBInsertedCardIdGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->insertedCardId;
}

/*********************************************************************
 * @purpose  Database Access - set inserted card type.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   inserted card type value
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardInsertedCardIdSet(L7_uint32 unit, L7_uint32 slot, L7_uint32 value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->insertedCardId = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get slot configured card type.
 *
 * @param    unit    unit
 * @param    slot    slot
 *
 * @returns  L7_uint32  configured card type
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cmgrCMDBConfigCardIdGet(L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_FALSE;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  return slotDb->configuredCardId;
}

/*********************************************************************
 * @purpose  Database Access - configured card ID.
 *
 * @param    unit    unit
 * @param    slot    slot
 * @param    value   configured card type value
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBCardConfigCardIdSet(L7_uint32 unit, L7_uint32 slot,
                                    L7_uint32 value)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];
  slotDb->configuredCardId = value;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - Copy information from one unit record
 *                             to another unit record.
 *
 * @param    src_unit Unit number from which to copy.
 * @param    dst_unit Unit number to which to copy.
 *
 * @returns  L7_RC_t L7_SUCCESS if value was set, L7_ERROR otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBUnitInfoCopy(L7_uint32 src_unit, L7_uint32 dst_unit)
{
  L7_uint32 slot;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardTypeDb;

  if ((src_unit > CMGR_MAX_UNITS_f) ||
      (dst_unit > CMGR_MAX_UNITS_f))
  {
    return L7_ERROR;
  }

  cmUnit[dst_unit].maxSlotsOnUnit = cmUnit[src_unit].maxSlotsOnUnit;
  cmUnit[dst_unit].unitConfigured = cmUnit[src_unit].unitConfigured;
  cmUnit[dst_unit].unitConnected = cmUnit[src_unit].unitConnected;
  cmUnit[dst_unit].unitType = cmUnit[src_unit].unitType;

  /* Note that we only copy real time information from
  ** the slot record. We don't copy any configuration fields.
  */
  for (slot = 0; slot <= CMGR_MAX_SLOTS_f; slot++)
  {
    cardTypeDb = sysapiHpcCardDbEntryGet(cmUnit[src_unit].cmSlot[slot].insertedCardId);

    /* Only Copy Physical Cards */
    if ((cardTypeDb != L7_NULL) && (cardTypeDb->type == SYSAPI_CARD_TYPE_LINE))
    {
      memset (&cmUnit[dst_unit].cmSlot[slot], 0, sizeof (cmgr_slot_t));
      cmUnit[dst_unit].cmSlot[slot].cardStatus = cmUnit[src_unit].cmSlot[slot].cardStatus;
      cmUnit[dst_unit].cmSlot[slot].insertedCardId = cmUnit[src_unit].cmSlot[slot].insertedCardId;
      cmUnit[dst_unit].cmSlot[slot].isFull = cmUnit[src_unit].cmSlot[slot].isFull;
      cmUnit[dst_unit].cmSlot[slot].isValid = cmUnit[src_unit].cmSlot[slot].isValid;
      cmUnit[dst_unit].cmSlot[slot].slotId = cmUnit[src_unit].cmSlot[slot].slotId;
      cmUnit[dst_unit].cmSlot[slot].phySlotId = cmUnit[src_unit].cmSlot[slot].phySlotId;
      cmUnit[dst_unit].cmSlot[slot].adminMode = cmUnit[src_unit].cmSlot[slot].adminMode;
      cmUnit[dst_unit].cmSlot[slot].powerMode = cmUnit[src_unit].cmSlot[slot].powerMode;
      cmUnit[dst_unit].cmSlot[slot].cardType = cmUnit[src_unit].cmSlot[slot].cardType;
      cmUnit[dst_unit].cmSlot[slot].cardIndex = cmUnit[src_unit].cmSlot[slot].cardIndex;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Database Access - get next slot from given unit and slot.
 *
 * @param    unit         unit
 * @param    slot         slot
 * @param    *nextSlot    next valid slot
 *
 * @returns  L7_RC_t  L7_SUCCESS if next slot is found; L7_ERROR
 *                    otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCMDBSlotNextGet(L7_uint32 unit, L7_uint32 slot, L7_uint32 *nextSlot)
{
  L7_uint32 x = 0, y = 0;
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;

  for (x = unit; x < CMGR_MAX_UNITS_f; x++)
  {
    unitDb = &cmUnit[x];

    if ((unitDb != NULL) && (unitDb->unitConfigured == L7_TRUE))
    {
      for (y = slot; y < cmgrCMDBUnitMaxSlotsGet(unit); y++)
      {
        slotDb = &unitDb->cmSlot[y];
        if (slotDb->isValid == L7_TRUE)
        {
          *nextSlot = y;
          return L7_SUCCESS;
        }
      }
    }
  }
  *nextSlot = 0;
  return L7_ERROR;
}

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
L7_RC_t cmgrCMDBLagCardUSPGet (L7_uint32 *unit, L7_uint32 *slot)
{
  L7_uint32 i, j;
  L7_uint32 maxUnits;
  L7_uint32 maxSlots;
  SYSAPI_HPC_CARD_DESCRIPTOR_t* cardTypeDb;
  L7_uint32 card_id;

  maxUnits = CMGR_MAX_UNITS_f;
  maxSlots = CMGR_MAX_SLOTS_f;

  for (i = 0; i < maxUnits ;i++)
  {
    if (cmUnit[i].unitConfigured != L7_TRUE)
    {
      continue;
    }
    for (j = 0; j < maxSlots; j++)
    {
      if (cmUnit[i].cmSlot[j].isFull != L7_TRUE)
      {
        continue;
      }
      card_id = cmUnit[i].cmSlot[j].insertedCardId;

      cardTypeDb = sysapiHpcCardDbEntryGet(card_id);
      if ((cardTypeDb != L7_NULL) && (cardTypeDb->type == SYSAPI_CARD_TYPE_LAG))
      {
        *unit = i;
        *slot = cmUnit[i].cmSlot[j].slotId;
        return L7_SUCCESS;
      }
    }
  }

  return L7_ERROR;
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
L7_RC_t cmgrCMDBVlanCardUSPGet (L7_uint32 *unit, L7_uint32 *slot)
{
  L7_uint32 i, j;
  L7_uint32 maxUnits;
  L7_uint32 maxSlots;
  SYSAPI_HPC_CARD_DESCRIPTOR_t* cardTypeDb;
  L7_uint32 card_id;

  maxUnits = CMGR_MAX_UNITS_f;
  maxSlots = CMGR_MAX_SLOTS_f;

  for (i = 0; i < maxUnits ;i++)
  {
    if (cmUnit[i].unitConfigured != L7_TRUE)
    {
      continue;
    }
    for (j = 0; j < maxSlots; j++)
    {
      if (cmUnit[i].cmSlot[j].isFull != L7_TRUE)
      {
        continue;
      }
      card_id = cmUnit[i].cmSlot[j].insertedCardId;

      cardTypeDb = sysapiHpcCardDbEntryGet(card_id);
      if ((cardTypeDb != L7_NULL) && (cardTypeDb->type == SYSAPI_CARD_TYPE_VLAN_ROUTER))
      {
        *unit = i;
        *slot = cmUnit[i].cmSlot[j].slotId;
        return L7_SUCCESS;
      }
    }
  }

  return L7_ERROR;
}

/*********************************************************************
 *
 * @purpose Send the pending notifications for the given unit and slot
 *           at the time of registration
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if parameters are invalid or other error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrCMDBPluginSendPendingNotif( L7_COMPONENT_IDS_t registrar_ID,
                                        L7_uint32 unit, L7_uint32 slot)
{
  cmgr_unit_t* unitDb;
  cmgr_slot_t* slotDb;
  L7_uint32 cardType;

  if ((unit > CMGR_MAX_UNITS_f) ||
      (slot > cmgrCMDBUnitMaxSlotsGet(unit)))
    return L7_ERROR;

  unitDb = &cmUnit[unit];
  slotDb = &unitDb->cmSlot[slot];

  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("CMGR registrar_ID %d greater then CMGR_USER_LAST\n", registrar_ID);
    return(L7_FAILURE);
  }

  if(L7_SUCCESS == (cmgrRegistrarId2CardTypeGet(registrar_ID, &cardType)))
  {
    if((unitDb->cmSlot[slot].cardType == cardType) &&
       (cmUnit[unit].cmSlot[slot].cardStatus == L7_CARD_STATUS_PLUG) &&
       (cmUnit[unit].cmSlot[slot].cardInsertNotified == L7_FALSE))
    {
      cmgrDoNotifyCardPlugin(unit, slot, cmUnit[unit].cmSlot[slot].insertedCardId);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Map the component ID to card type Id.
 *
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if parameters are invalid or other error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cmgrRegistrarId2CardTypeGet( L7_COMPONENT_IDS_t registrar_ID, L7_uint32 *cardType)
{
  L7_RC_t rc = L7_FAILURE;

  switch(registrar_ID)
  {
    case L7_POWER_UNIT_MGR_COMPONENT_ID:
      *cardType = SYSAPI_CARD_TYPE_POWER_SUPPLY;
      rc = L7_SUCCESS;
      break;

    case L7_FAN_UNIT_MGR_COMPONENT_ID:
      *cardType = SYSAPI_CARD_TYPE_FAN;
      rc = L7_SUCCESS;
      break;

    case L7_EDB_COMPONENT_ID:
    case L7_UNITMGR_COMPONENT_ID:
      break;

    default:
      CMGR_DEBUG(CMGR_LOG_FLAG,
                 "cmgrRegistrarId2CardTypeGet: Unknown registrar ID:%d\n", registrar_ID);
      LOG_MSG("cmgrRegistrarId2CardTypeGet: Unknown registrar ID:%d\n", registrar_ID);
      break;
  }

  return rc;
}

/*********************************************************************
 * @purpose  After sending CARD_INSERT, update LED status to indicate
 *           that Initialisation is complete
 * @param    none
 *
 * @returns  L7_RC_t rc
 *
 * @author   bguna
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrLedInitScmLm(L7_uint32 unit, L7_uint32 slot)
{
  SYSAPI_CARD_TYPE_t moduleType;
  L7_int32 phySlot;
  SYSAPI_CARD_INDEX_t moduleIndex;
  L7_RC_t rc = L7_FAILURE;

  /* Do not send led events for fan, power, and logical cards */
  if(unit == 0)
    return L7_SUCCESS;

  if(L7_SUCCESS != (sysapiHpcSlotMapUSPhysicalSlotGet(unit, &phySlot)))
    return rc;

  if(L7_SUCCESS != (sysapiHpcSlotMapMPModuleInfoGet(phySlot, &moduleType, &moduleIndex)))
    return rc;

  if(slot != phySlot)
    return rc;

  if((moduleType == SYSAPI_CARD_TYPE_CONTROL) ||
     (moduleType == SYSAPI_CARD_TYPE_LINE))
  {
    if((L7_SUCCESS != (rc = sysapiHpcLedUpdate(phySlot, SYSAPI_CARD_EVENT_INIT_DONE))))
    {
      CMGR_DEBUG(CMGR_LOG_FLAG, "Initialisation failed for %s %d\n",
                 cardTypeStr[moduleType], moduleIndex);
      LOG_MSG("Initialisation failed for %s %d\n", cardTypeStr[moduleType], moduleIndex);
    }
    else
    {
      CMGR_DEBUG(CMGR_LOG_FLAG, "Initialisation complete for %s %d\n",
                 cardTypeStr[moduleType], moduleIndex);
    }
  }
  return rc;
}

/*********************************************************************
 *
 * @purpose Unplug and unconfigure all cards.
 *
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_ERROR,   if parameters are invalid or other error
 *
 * @notes None.
 *
 * @end
 *
 *********************************************************************/
void cmgrAllCardsUnplugUnconfigure(void)
{
  L7_uint32 slot;
  L7_uint32 unit;
  L7_uint32 config_card_id;
  L7_uint32 inserted_card_id;
  L7_uint32 loopCount;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardTypeDb;
  L7_uint32 phySlot = 0;

  /* handle all inserted cards
  ** First the physical cards and
  ** second for the logical cards.
  ** (loopCount == 0) means physical cards
  ** (loopCount == 1) means logical  cards
  */
  for (loopCount=0;loopCount<2;loopCount++)
  {
    for (unit = 0; unit <= CMGR_MAX_UNITS_f; unit++)
    {
      if(L7_SUCCESS == (sysapiHpcSlotMapUSPhysicalSlotGet(unit, &phySlot)))
      {
        /* Set slot power status to default */
        (void)cmgrHpcPwrSlotPowerSet(phySlot, POWER_ON, POWER_ADMIN);
      }

      for (slot=0; slot <= cmgrCMDBUnitMaxSlotsGet(unit); slot++)
      {
        config_card_id = cmgrCMDBConfigCardIdGet(unit,slot);

        inserted_card_id = cmgrCMDBInsertedCardIdGet(unit,slot);

        cardTypeDb = sysapiHpcCardDbEntryGet(config_card_id);

        if (cardTypeDb == L7_NULL)
          continue;

        /* Do not unplug any physical cards in unit 0 */
        /* Any physical cards in unit 0 are only for storage of the local system */
        if (((loopCount == 1) &&                            /* unPlug all logical cards in the system */
             (cardTypeDb->type != SYSAPI_CARD_TYPE_LINE))
            ||
            ((loopCount == 0) &&                            /* unPlug all Physical Cards not in unit 0 */
             (cardTypeDb->type == SYSAPI_CARD_TYPE_LINE) && /* because card manager stores all local   */
             (unit != 0)))                                  /* card info there                         */
        {
          if((cardTypeDb->type != SYSAPI_CARD_TYPE_FAN) &&
             (cardTypeDb->type != SYSAPI_CARD_TYPE_POWER_SUPPLY))
          {
            if (inserted_card_id != L7_CMGR_CARD_ID_NONE)
            {
              (void)cmgrAsyncCardUnPlugNotify(unit, slot, inserted_card_id);
            }

            if (config_card_id != L7_CMGR_CARD_ID_NONE)
            {
              (void)cmgrAsyncCardClearNotify(unit, slot, config_card_id);
            }

            osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

            (void)cmgrCMDBCardStatusSet(unit, slot, L7_CARD_STATUS_UNKNOWN);
            (void)cmgrCMDBSlotConfiguredAdminModeSet(unit, slot, L7_ENABLE);
            (void)cmgrCMDBSlotConfiguredPowerModeSet(unit, slot, L7_ENABLE);
            (void)cmgrCMDBCardStatusSet(unit, slot, L7_CARD_STATUS_UNKNOWN);
            (void)cmgrCMDBCardConfigCardIdSet(unit, slot, L7_CMGR_CARD_ID_NONE);

            osapiSemaGive(cmgrSemaId);
          }
        }
      }
    }
  }
  /* Wait until all pending card insertions and removals are done.
   */
  cmgrCardInsertRemoveWait();
}

/*****************************************************************************/
/*  Initialization Functions                                                 */
/*****************************************************************************/

/*********************************************************************
 * @purpose  Card Manager Timer - handles Card Manager timer events
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    This is called from the timer task and does minimal work.
 *           It forwards the message to the Card Manager task and resets
 *           the timer.
 *
 * @end
 *********************************************************************/
void cmgrTimer(L7_uint32 arg1, L7_uint32 arg2)
{
  L7_int32 cmgrRC;
  cmgr_cmpdu_t cmpdu;

  osapiSemaTake(cmgrTimerSyncSemaId, L7_WAIT_FOREVER);

  pCmgrTimerHolder = L7_NULLPTR;

  do
  {
    if (cmgrQueue == L7_NULLPTR)
    {
      LOG_MSG("cmgrTimer(): cmgrQueue has not been created \n");
      break;
    }

    if (unitMgrNumberGet(&cmpdu.unit) != L7_SUCCESS)
    {
      /* no unit number available at this time... try again later */

      osapiTimerAdd ((void*)cmgrTimer, arg1, arg2, L7_CMGR_TIMER_INTERVAL_f,
                     &pCmgrTimerHolder );
      break;
    }

    cmpdu.cmgrPacketType = L7_CMPDU_TIMER_TIMEOUT;

    cmgrRC = osapiMessageSend(cmgrQueue, &cmpdu, sizeof(cmgr_cmpdu_t), 0, L7_MSG_PRIORITY_NORM );
    if (cmgrRC != L7_SUCCESS)
    {
      LOG_MSG("cmgrTimer(): Card Manager timer tick send failed\n");
    }

    osapiTimerAdd ((void*)cmgrTimer, arg1, arg2, L7_CMGR_TIMER_INTERVAL_f,
                   &pCmgrTimerHolder );
  } while (0);

  osapiSemaGive(cmgrTimerSyncSemaId);
}

/*********************************************************************
 * @purpose  Restart Card Manager Timers
 *
 * @param    none
 *
 * @notes    Due to lack of synchonization between CMGR/UM/SPM, it
 *           is possible that the manager can receive card status update
 *           before it has received updates from spm. This causes the
 *           the stack ports to be treated as fron-panel ports and get
 *           attached at NIM.
 *           To avoid this, CM timer is restarted whenever a unit
 *           moves to isolated state from a conn_unit/conn_mgr state.
 *           This gives ample opportunity for the manager to get an
 *           stacking port update from the SPM.
 *
 * @returns  void
*********************************************************************/
void cmgrResetTimers(void)
{
  osapiSemaTake(cmgrTimerSyncSemaId, L7_WAIT_FOREVER);

  if (pCmgrTimerHolder)
  {
    osapiChangeUserTimer(pCmgrTimerHolder, L7_CMGR_TIMER_INTERVAL_f);
  }

  osapiSemaGive(cmgrTimerSyncSemaId);
}

/*********************************************************************
 * @purpose  Wait until all pending card insertions and removals are
 *           done.
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
static void cmgrCardInsertRemoveWait(void)
{
  cmgrCardInsertRemoveMsg_t cmd;
  L7_RC_t rc;

  cmd.action = L7_CMGR_CARD_SYNC;
  cmd.unit = 0;
  cmd.slot = 0;
  cmd.cardId = 0;

  rc = osapiMessageSend(cmgrCardInsertRemoveQueue,
                        (void *)&cmd,
                        sizeof(cmgrCardInsertRemoveMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  /* Wait until Card Insert/Remove task is done processing all
  ** pending card insertions and removals.
  */
  rc = osapiSemaTake (cmgrCardInsertRemoveSemaId, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
}

/*********************************************************************
 * @purpose  Card Manager Card Insertion/Removal Task.
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    This task adds and removes cards. The task may block for
 *           extended periods of time while the device driver is
 *           processing card insertions and while the applications
 *           are processing port insertions.
 *
 * @end
 *********************************************************************/
void cmgrCardInsertRemoveTask()
{
  cmgrCardInsertRemoveMsg_t cmd;
  L7_RC_t rc;

  do
  {
    rc = osapiMessageReceive(cmgrCardInsertRemoveQueue, &cmd,
                             sizeof(cmgrCardInsertRemoveMsg_t),
                             L7_WAIT_FOREVER);
    if (rc != L7_SUCCESS)
    {
      LOG_ERROR (rc);
    }

    CMGR_TRACE_CARD_EVENT(cmd.action, L7_TRUE, cmd.unit, cmd.slot);

    switch (cmd.action)
    {
      case L7_CMGR_CARD_INSERT:
        CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardInsertRemoveTask: INSERT unit:%d slot:%d cardId:0x%x\n", cmd.unit, cmd.slot, cmd.cardId);
        cmgrCardPluginNotify(cmd.unit, cmd.slot, cmd.cardId);
        break;

      case L7_CMGR_CARD_REMOVE:
        CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardInsertRemoveTask: REMOVE unit:%d slot:%d cardId:0x%x\n", cmd.unit, cmd.slot, cmd.cardId);
        cmgrCardUnPlugNotify(cmd.unit, cmd.slot, cmd.cardId);
        break;
      case L7_CMGR_CARD_SYNC:
        CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardInsertRemoveTask: SYNC unit:%d slot:%d cardId:0x%x\n", cmd.unit, cmd.slot, cmd.cardId);
        rc = osapiSemaGive(cmgrCardInsertRemoveSemaId);
        if (rc != L7_SUCCESS)
        {
          LOG_ERROR (rc);
        }
        break;

      case L7_CMGR_CARD_CREATE:
        CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardInsertRemoveTask: CREATE unit:%d slot:%d cardId:0x%x\n", cmd.unit, cmd.slot, cmd.cardId);
        cmgrCardCreateNotify(cmd.unit, cmd.slot, cmd.cardId);
        break;

      case L7_CMGR_CARD_CONFIG_SLOTS:
        if(cmgrCardConfigCallback != L7_NULLPTR)
        {
          CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardInsertRemoveTask: CONFIG Phase 0 Start\n");
          CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_CONFIG_SLOTS, L7_TRUE, cmd.unit, cmd.slot);
          cmgrCardConfigCallback(L7_CMGR_CONFIG_EVENT_CONFIG_SLOTS);
          CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_CONFIG_SLOTS, L7_FALSE, cmd.unit, cmd.slot);
          CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardInsertRemoveTask: CONFIG Phase 0 End\n");
        }
        break;

      case L7_CMGR_CARD_CONFIG_PORTS:
        if(cmgrCardConfigCallback != L7_NULLPTR)
        {
          CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardInsertRemoveTask: CONFIG Phase 1 Start\n");
          CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_CONFIG_PORTS, L7_TRUE, cmd.unit, cmd.slot);
          cmgrCardConfigCallback(L7_CMGR_CONFIG_EVENT_CONFIG_PORTS);
          CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_CONFIG_PORTS, L7_FALSE, cmd.unit, cmd.slot);
          CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardInsertRemoveTask: CONFIG Phase 1 End\n");
        }
        break;

      case L7_CMGR_CARD_DELETE:
        CMGR_DEBUG(CMGR_LOG_FLAG, "cmgrCardInsertRemoveTask: DELETE unit:%d slot:%d cardId:0x%x\n", cmd.unit, cmd.slot, cmd.cardId);
        cmgrCardClearNotify(cmd.unit, cmd.slot, cmd.cardId);
        break;

      case L7_CMGR_LOCAL_UNIT_IS_MANAGER:
        CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_LOCAL_UNIT_MGR, L7_TRUE, cmd.unit, cmd.slot);
        cmgrTaskLocalUnitIsManager();
        CMGR_PROFILE_EVENT(CMGR_PROFILE_EVENT_CARD_LOCAL_UNIT_MGR, L7_FALSE, cmd.unit, cmd.slot);
        break;

      case L7_CMGR_LOCAL_UNIT_IS_NOT_MANAGER:
        cmgrTaskLocalUnitIsNotManager();
        break;

      default:
        LOG_ERROR (cmd.action);
        break;
    }

    CMGR_TRACE_CARD_EVENT(cmd.action, L7_FALSE, cmd.unit, cmd.slot);

  } while (1);
}

/*********************************************************************
 * @purpose  Card Manager Task
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cmgrTask()
{
  cmgr_cmpdu_t cmpdu;
  L7_RC_t rc;

  /* create and initialize the Card Manager Database */
  cmgrDatabaseInit();

  (void)osapiTaskInitDone(L7_CMGR_TASK_SYNC_f);

  do
  {
    if (cmgrQueue == L7_NULLPTR)
      continue;

    rc = osapiMessageReceive(cmgrQueue, &cmpdu,
                             sizeof(cmgr_cmpdu_t), L7_WAIT_FOREVER);
    if (rc == L7_SUCCESS)
    {
      CMGR_TRACE_CMPDU_EVENT(cmpdu.cmgrPacketType,
                             L7_TRUE,
                             cmpdu.unit
                            );
      switch (cmpdu.cmgrPacketType)
      {
        case L7_CMPDU_CNFGR_REQUEST:
          cmgrCnfgrCmdReceive(&cmpdu);
          break;

        case L7_CMPDU_TIMER_TIMEOUT:
          cmgrSendStatusUpdate(cmpdu.cmgrPacketType);
          break;

        case L7_CMPDU_CARD_REPORT_CARD_PLUGIN:
          cmgrReceiveCardPlugIn(&cmpdu);
          break;

        case L7_CMPDU_CARD_REPORT_CARD_UNPLUG:
          cmgrReceiveCardUnplug(&cmpdu);
          break;

        case L7_CMPDU_REQUEST_ALL_INFO:
          /*if (unitMgrNumberGet(&cmpdu.unit) == L7_SUCCESS)*/
          {
            cmgrReceiveStatusUpdateRequest(&cmpdu);
          }
          break;

        case L7_CMPDU_STATUS_UPDATE:
          cmgrReceiveStatusUpdate(&cmpdu);
          break;

        case L7_CMPDU_STATUS_REQUEST:
          if (L7_SUCCESS != cmgrSendRequestForStatusUpdate(cmpdu.unit))
      	  {
      	    LOG_MSG("CMGR Req status update to unit-%d- failed\n", cmpdu.unit);
      	  }
          break;

        case L7_CMPDU_CARD_REPORT_CARD_FAILURE:
          cmgrReceiveCardFailure(&cmpdu);
          break;

        default:
          LOG_MSG("cmgrTask(): invalid message type:%d. %s:%d\n",
                  cmpdu.cmgrPacketType, __FILE__, __LINE__);
          break;
      }

      CMGR_TRACE_CMPDU_EVENT(cmpdu.cmgrPacketType,
                             L7_FALSE,
                             cmpdu.unit
                            );
    }
  } while (1);
  return;
}

/**************************************************************************/
/* NVStore code                                                           */

/**************************************************************************/

/*********************************************************************
 * @purpose  Build default card manager configuration.
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cmgrBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 unit, slot;
  L7_uint32 maxUnits, maxSlots;

  maxUnits = CMGR_MAX_UNITS_f;
  maxSlots = CMGR_MAX_SLOTS_f;

  memset(( void * )&cmgrCfgData, 0, sizeof(L7_cardMgrCfgFileData_t));
  OSAPI_STRNCPY_SAFE(cmgrCfgData.cfgHdr.filename, CMGR_CFG_FILENAME);
  cmgrCfgData.cfgHdr.version = ver;
  cmgrCfgData.cfgHdr.componentID = L7_CARDMGR_COMPONENT_ID;
  cmgrCfgData.cfgHdr.type = L7_CFG_DATA;
  cmgrCfgData.cfgHdr.length = sizeof(L7_cardMgrCfgFileData_t);
  cmgrCfgData.cfgHdr.dataChanged = L7_FALSE;
  cmgrCfgData.cfgHdr.version = ver;

  for (unit = 0; unit <= maxUnits; unit++)
  {
    for (slot = 0; slot <= maxSlots; slot++)
    {
      cmgrCfgData.cfgParms.configCard[unit][slot].card_id = L7_CMGR_CARD_ID_NONE;
      cmgrCfgData.cfgParms.configCard[unit][slot].adminMode = L7_ENABLE;
      cmgrCfgData.cfgParms.configCard[unit][slot].powerMode = L7_ENABLE;
    }
  }

  return;
}

/*********************************************************************
 * @purpose  Card Manager Save data function.
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrSave(void)
{
  if (cmgrCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    cmgrCfgData.cfgHdr.dataChanged = L7_FALSE;
    cmgrCfgData.checkSum = nvStoreCrc32((L7_char8 *)&cmgrCfgData,
                                        sizeof(cmgrCfgData) - sizeof(cmgrCfgData.checkSum));

    if (sysapiCfgFileWrite(L7_CARDMGR_COMPONENT_ID, CMGR_CFG_FILENAME, (L7_char8 *)&cmgrCfgData,
                           sizeof (cmgrCfgData)) == L7_ERROR)
    {
      LOG_MSG("Error on call to sysapiCfgFileWrite routine on config file %s\n",CMGR_CFG_FILENAME);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Is data changed?.
 *
 * @param    none
 *
 * @returns  boolean L7_TRUE if data has changed;  L7_FALSE otherwise.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrHasDataChanged(void)
{
  /* The location of the dataChanged flag in card manager will always cause
     a false report of changed configuration.  The event of an end user
     pre-configuring a card is expected to be rare.  Thus, the workaround
     is such that card manager will never report the value of its dataChanged
     flag.  However, the internal usage of the dataChanged flag will remain
     unchanged so that the configuration of card manager will always be saved. */
  return L7_FALSE;
}

void cmgrResetDataChanged(void)
{
  cmgrCfgData.cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
 * @purpose  Dump configuration function.
 *
 * @param    none
 *
 * @returns  L7_RC_t
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrConfigDump(void)
{
  return L7_SUCCESS;
}

/**************************************************************************/
/* Card Manager Utility Functions                                         */
/**************************************************************************/

/*********************************************************************
 * @purpose  Helper routine to get card index from card id
 *
 * @param  card_id  @b{(input)) the card id
 * @param  cx       @b{(output)) the card index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrCardIndexFromIDGet(L7_uint32 card_id, L7_uint32 *cx)
{
  L7_uint32 cid, cidx = 0;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *pCardDescriptor;
  L7_RC_t rc;

  if (card_id != 0)
  {
    rc = sysapiHpcCardDbEntryIndexNextGet(&cidx);

    while (rc == L7_SUCCESS)
    {
      if ((pCardDescriptor = sysapiHpcCardDbEntryByIndexGet(cidx)) != L7_NULLPTR)
      {
        cid = pCardDescriptor->cardTypeId;
        if (cid == card_id)
        {
          *cx = cidx;
          return L7_SUCCESS;
        }
      }

      rc = sysapiHpcCardDbEntryIndexNextGet(&cidx);
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Helper routine to get slot number from unit-slot index
 *
 * @param  ux        @b{(input)) the unit index
 * @param  sx        @b{(input)) the slot index
 * @param  slot_num  @b{(output)) the slot number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrSlotNumFromIndexGet(L7_uint32 ux, L7_uint32 sx, L7_uint32 *slot_number)
{
  L7_uint32 uidx, sidx, num;
  L7_RC_t rc;

  if (sx != 0)
  {
    uidx = sidx = 0;
    rc = sysapiHpcSlotDbEntryIndexNextGet(&uidx, &sidx);

    while (rc == L7_SUCCESS)
    {
      if ((uidx == ux) && (sidx == sx))
      {
        if (sysapiHpcSlotDbEntrySlotNumberGet(uidx, sidx, &num) == L7_SUCCESS)
        {
          *slot_number = num;
          return L7_SUCCESS;
        }
      }

      rc = sysapiHpcSlotDbEntryIndexNextGet(&uidx, &sidx);
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Helper routine to get unit index from unit id
 *
 * @param  unit_id  @b{(input)) the unit id
 * @param  cx       @b{(output)) the unit index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrUnitIndexFromIDGet(L7_uint32 unit_id, L7_uint32 *ux)
{
  L7_uint32 uid, uidx = 0;
  L7_RC_t rc;

  if (unit_id != 0)
  {
    rc = sysapiHpcUnitDbEntryIndexNextGet(&uidx);

    while (rc == L7_SUCCESS)
    {
      rc = sysapiHpcUnitDbUnitIdGet(uidx, &uid);
      if ((rc == L7_SUCCESS) && (uid == unit_id))
      {
        *ux = uidx;
        return L7_SUCCESS;
      }

      rc = sysapiHpcUnitDbEntryIndexNextGet(&uidx);
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Helper routine to get slot index from slot number
 *           for a given unit index
 *
 * @param  ux    @b{(input)) the unit index
 * @param  slot  @b{(input)) the slot number
 * @param  sx    @b{(output)) the slot index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrSlotIndexFromNumGet(L7_uint32 ux, L7_uint32 slot, L7_uint32 *sx)
{
  L7_uint32 uidx, sidx, num;
  L7_RC_t rc;

  if (ux != 0)
  {
    uidx = sidx = 0;
    rc = sysapiHpcSlotDbEntryIndexNextGet(&uidx, &sidx);

    while (rc == L7_SUCCESS)
    {
      if (uidx == ux)
      {
        if (sysapiHpcSlotDbEntrySlotNumberGet(uidx, sidx, &num) == L7_SUCCESS)
        {
          if (num == slot)
          {
            *sx = sidx;
            return L7_SUCCESS;
          }
        }
      }

      rc = sysapiHpcSlotDbEntryIndexNextGet(&uidx, &sidx);
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Helper routine to get unit entry and slot index
 *           for a given unit-slot
 *
 * @param  unit        @b{(input)) the unit number
 * @param  slot        @b{(input)) the slot number
 * @param  sx          @b{(output)) the slot index
 * @param  unit_entry  @b{(output)) pointer to the unit entry structure
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t cmgrUnitEntryAndSlotIndexGet(L7_uint32 unit, L7_uint32 slot, L7_uint32 *sx,
                                     SYSAPI_HPC_UNIT_DESCRIPTOR_t *unit_entry)
{
  L7_uint32 unit_id, slot_index;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *u_entry;

  if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID,
                            L7_STACKING_FEATURE_SUPPORTED) != L7_TRUE)
  {
    u_entry = sysapiHpcLocalUnitDbEntryGet();
  }
  else
  {
    unit_id = cmgrCMDBUnitTypeGet(unit);
    u_entry = sysapiHpcUnitDbEntryGet(unit_id);
  }

  if (L7_NULLPTR == u_entry)
    return L7_FAILURE;

  for (slot_index = 0; slot_index < L7_MAX_PHYSICAL_SLOTS_PER_UNIT; slot_index++)
  {
    if (u_entry->physSlot[slot_index].slot_number == slot)
    {
      *sx = slot_index;
      memcpy(unit_entry, u_entry, sizeof(SYSAPI_HPC_UNIT_DESCRIPTOR_t));
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*****************************************************************************
 * @purpose  This is essentially a callback function called by configurator
 *           after all the intialization is completed related to
 *           transformation to stack manager. This function gives the ACTIVE
 *           event to HPC for chassis configuration. Any other callbacks can
 *           be inserted here.
 *
 * @param  None
 *
 * end
 ******************************************************************************/
static void cmgrInitCompleteCallBack(void)
{
  sysapiHpcHrcActiveStandbyNotify(SYSAPI_CARD_EVENT_ACTIVE);
}

/*********************************************************************
 * @purpose  Check if there are any more cards in the CM database
 *           except the currUnit/currSlot which are yet to be plugged-in
 *
 * @param  currUnit        @b{(input)) the unit number of the card to ignore
 * @param  currSlot        @b{(input)) the slot number of the card to ignore
 * @param  nextUnit        @b{(output)) the unit number of the card not yet
 *                                      plugged
 * @param  nextSlot        @b{(output)) the slot number of the card not yet
 *                                      plugged
 *
 * @returns  L7_FALSE        No more pluggable cards found.
 * @returns  L7_TRUE         A pluggable card found.
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_BOOL cmgrCardPendingPluginCheck(L7_uint32 currUnit, L7_uint32 currSlot,
                                   L7_uint32 *nextUnit, L7_uint32 *nextSlot)
{
  L7_RC_t              result = L7_FAILURE;
  unitMgrStatus_t      unitStatus;
  L7_uint32            slotIdx, unitIdx;
  L7_BOOL              found = L7_FALSE;

  *nextUnit = 0;
  *nextSlot = 0;

  osapiSemaTake(cmgrSemaId, L7_WAIT_FOREVER);

  for (unitIdx = 0; unitIdx <= L7_MAX_UNITS_PER_STACK; unitIdx++)
  {
    /* Check the unit status */
    result = unitMgrUnitStatusGet(unitIdx, &unitStatus);
    if ((result != L7_SUCCESS) || (unitStatus != L7_UNITMGR_UNIT_OK))
    {
      continue;
    }

    /* Loop through all the physical slots */
    for (slotIdx = 0; slotIdx < CMGR_MAX_SLOTS_f; slotIdx++)
    {
      if ((unitIdx == currUnit) && (slotIdx == currSlot))
        continue;

      if (cmgrCMDBCardTypeGet(unitIdx, slotIdx) != SYSAPI_CARD_TYPE_LINE)
        continue;

      /* If the slot is in plug status and has not been inserted yet, then we have a pending physical slot */
      if ((cmgrCMDBCardStatusGet(unitIdx, slotIdx) == L7_CARD_STATUS_PLUG)  &&
          (cmgrCMDBCardInsertNotifiedGet(unitIdx, slotIdx) == L7_FALSE))
      {
        found = L7_TRUE;
        *nextUnit = unitIdx;
        *nextSlot = slotIdx;
        break;
      }
    }
  }

  osapiSemaGive(cmgrSemaId);

  return found;
}
