/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  voice_vlan_port_cnfgr.c
*
* @purpose   Voice Vlan  port configurator file
*
* @component voicevlan
*
* @comments
*
* @create
*
* @author
*
* @end
*
**********************************************************************/

#include <string.h>

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "voice_vlan_cnfgr.h"
#include "voice_vlan.h"
#include "voice_vlan_cfg.h"
#include "voice_vlan_debug.h"
#include "dot1q_api.h"
#include "voicevlan_exports.h"

#ifdef L7_NSF_PACKAGE
#include "voice_vlan_ckpt.h"
#endif /* L7_NSF_PACKAGE */


voiceVlanPortCnfgrState_t voiceVlanPortCnfgrState = VOICE_VLAN_INIT_0;
L7_BOOL                   voiceVlanWarmRestart = L7_FALSE;
L7_BOOL                   voiceVlanActivateStartupDone = L7_FALSE;
L7_BOOL                   voiceVlanBackupElected = L7_FALSE;
void                      *voiceVlanCkptReqQ;
avlTree_t                 voiceVlanCkptInfoAvlTree;
avlTreeTables_t           *voiceVlanCkptInfoTreeHeap;
voiceVlanDeviceInfo_t     *voiceVlanCkptInfoDataHeap;

extern voiceVlanInfo_t    *voiceVlanInfo;
extern voiceVlanCfg_t     *voiceVlanCfg;
extern L7_uint32          *voiceVlanMapTbl;
extern L7_VLAN_MASK_t     voiceVlanMask;
extern L7_ushort16        *voiceVlanIntfCount;

extern PORTEVENT_MASK_t   voiceVlanNimEventMask;

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
static
L7_RC_t voiceVlanCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t * pResponse,
                                        L7_CNFGR_ERR_RC_t * pReason)
{
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  L7_RC_t rc = L7_FAILURE;

  /* Allocate and initialized memory for global data */
  voiceVlanCfg = (voiceVlanCfg_t *) osapiMalloc(L7_VOICE_VLAN_COMPONENT_ID,
                                                sizeof(voiceVlanCfg_t));

  voiceVlanInfo  = ( voiceVlanInfo_t *) osapiMalloc(L7_VOICE_VLAN_COMPONENT_ID,
                                       sizeof(voiceVlanInfo_t));

  voiceVlanMapTbl = (L7_uint32 *)osapiMalloc(L7_VOICE_VLAN_COMPONENT_ID,
                                (L7_uint32)sizeof(L7_uint32) * (platIntfMaxCountGet()+1));

  voiceVlanIntfCount = (L7_ushort16 *)osapiMalloc(L7_VOICE_VLAN_COMPONENT_ID,
                                (L7_uint32)(sizeof(L7_ushort16) * (platVlanVlanIdMaxGet()+1)));

  voiceVlanInfo->voiceVlanLock = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  memset((void *)voiceVlanIntfCount, 0, (sizeof(L7_ushort16) * (platVlanVlanIdMaxGet()+1)));
  memset((void *)&voiceVlanMask, 0, sizeof(L7_VLAN_MASK_t));

  rc = voiceVlanDeviceInfoDBInit(L7_VOICE_VLAN_MAX_VOIP_DEVICE_COUNT);

  if((voiceVlanInfo == L7_NULLPTR) || (voiceVlanCfg == L7_NULLPTR) ||
     (voiceVlanMapTbl == L7_NULLPTR) || (voiceVlanInfo->voiceVlanLock == L7_NULLPTR)||(rc == L7_FAILURE) || (voiceVlanIntfCount == L7_NULLPTR))
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

#ifdef L7_NSF_PACKAGE
  /*
  voiceVlanDebugTraceFlagsSet(VOICE_VLAN_TRACE_CHECKPOINT);
  voiceVlanDebugTraceFlagsSet(VOICE_VLAN_TRACE_HIGH);
  voiceVlanDebugTraceFlagsSet(VOICE_VLAN_TRACE_MEDIUM);
  voiceVlanDebugTraceFlagsSet(VOICE_VLAN_TRACE_DEBUG);
  voiceVlanDebugTraceEnable();
  */

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_CHECKPOINT, "Allocating checkpoint info");
  if (voiceVlanCheckpointInfoAlloc() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VOICE_VLAN_COMPONENT_ID,
            "voiceVlanCnfgrInitPhase1Process: Error allocating data for NSF checkpointing");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }
  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_CHECKPOINT, "Checkpoint info allocated");
#endif /* L7_NSF_PACKAGE */

  voiceVlanPortCnfgrState = VOICE_VLAN_PHASE_INIT_1;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
static
L7_RC_t voiceVlanCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t * pResponse,
                                        L7_CNFGR_ERR_RC_t * pReason)
{
  L7_RC_t voiceVlanRC= L7_SUCCESS;
  nvStoreFunctionList_t notifyFunctionList;
  L7_uint32 eventMask = 0;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;

  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID = L7_VOICE_VLAN_COMPONENT_ID;
  notifyFunctionList.notifySave = voiceVlanSave;
  notifyFunctionList.hasDataChanged = voiceVlanHasDataChanged;
  notifyFunctionList.notifyConfigDump = voiceVlanCfgDump;
  notifyFunctionList.resetDataChanged = voiceVlanResetDataChanged;

  if(nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    voiceVlanRC = L7_ERROR;
    sysapiPrintf("error in nvstore \n");
    return voiceVlanRC;
  }

  /* register callback with NIM for interface changes */
  if(nimRegisterIntfChange(L7_VOICE_VLAN_COMPONENT_ID,
                           voiceVlanIntfChangeCallback,
                           voiceVlanStartupCallback, NIM_STARTUP_PRIO_VOICE_VLAN) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "voiceVlanCnfgrInitPhase2Process: Unable to register for Intf change callback!\n");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    voiceVlanRC = L7_ERROR;
    sysapiPrintf("voiceVlanCnfgrInitPhase2Process: Unable to register for Intf change callback!\n");
    return voiceVlanRC;
  }
  eventMask = VLAN_DELETE_PENDING_NOTIFY | VLAN_ADD_NOTIFY | VLAN_DELETE_PORT_NOTIFY;
  /* register callback with vlan to get notifications for vlan create/delete events */
  if(vlanRegisterForChange( voiceVlanDot1QNotificationCallback , L7_VOICE_VLAN_COMPONENT_ID, eventMask) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "voiceVlanCnfgrInitPhase2Process: Unable to register for Vlan callback\n");
    sysapiPrintf("voiceVlanCnfgrInitPhase2Process: Unable to register for Vlan callback\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    voiceVlanRC  = L7_ERROR;

    return voiceVlanRC;
  }


  /* register callback with LLDP-MED to get notification for availability/Non_availability of the voip device */
  if(lldpXMedNotificationRegister( L7_VOICE_VLAN_COMPONENT_ID, voiceVlanLLDPNotificationCallback ) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "voiceVlanCnfgrInitPhase2Process: Unable to register for lldp_med callback\n");
    sysapiPrintf("voiceVlanCnfgrInitPhase2Process: Unable to register for lldp_med callback\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    voiceVlanRC  = L7_ERROR;

    return voiceVlanRC;

  }

#ifdef L7_NSF_PACKAGE
  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_CHECKPOINT, "Registering for NSF callbacks");
  if (voiceVlanCheckpointCallbacksRegister() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_VOICE_VLAN_COMPONENT_ID,
           "Voice VLAN failed to register for checkpoint service callbacks.");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    voiceVlanRC  = L7_ERROR;

    return voiceVlanRC;
  }
#endif /* L7_NSF_PACKAGE */

  voiceVlanPortCnfgrState = VOICE_VLAN_PHASE_INIT_2;
  return voiceVlanRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @comments  The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @comments  The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
static
L7_RC_t voiceVlanCnfgrInitPhase3Process(L7_BOOL warmRestart,
                                        L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t dot1xRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  dot1xRC     = L7_SUCCESS;

  /*
  Clear the NIM port event mask until ready to receive events
  This is necessary because Phase3 can be re-entered without
  re-initializing voiceVlanNimEventMask
  */

  memset(&voiceVlanNimEventMask, 0, sizeof(voiceVlanNimEventMask));
  nimRegisterIntfEvents(L7_VOICE_VLAN_COMPONENT_ID, voiceVlanNimEventMask);

  if (sysapiCfgFileGet(L7_VOICE_VLAN_COMPONENT_ID, VOICE_VLAN_CFG_FILENAME,
                   (L7_char8 *)voiceVlanCfg, (L7_uint32)sizeof(voiceVlanCfg_t),
                   &voiceVlanCfg->checkSum, VOICE_VLAN_CFG_VER_CURRENT,
                   voiceVlanBuildDefaultConfigData, L7_NULLPTR) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1xRC     = L7_ERROR;

    return dot1xRC;
  }

  voiceVlanWarmRestart = warmRestart;

  /*
  If not a warm restart, clear Voice VLAN Checkpoint Data
  */

  if (!warmRestart)
  {
#ifdef L7_NSF_PACKAGE
    /* Clear any checkpointed data */
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_CHECKPOINT, "Not warm restart");
    voiceVlanCheckpointFlushAll();
#endif /* L7_NSF_PACKAGE */
  }

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_CHECKPOINT, "Applying Config Data");
  if (voiceVlanApplyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1xRC     = L7_ERROR;

    return dot1xRC;
  }
  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_CHECKPOINT, "Apply of Config Data done");
  voiceVlanPortCnfgrState = VOICE_VLAN_PHASE_INIT_3;
  return dot1xRC;
}


/*********************************************************************
* @purpose  This function undoes voiceVlanCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static
void voiceVlanCnfgrFiniPhase1Process()
{
  /* free the NULL pointer */
  if(voiceVlanInfo != L7_NULLPTR && voiceVlanInfo->voiceVlanLock != L7_NULLPTR)
  {
    osapiSemaDelete(voiceVlanInfo->voiceVlanLock);
  }

  /* Deallocate anything that was allocated */
  if(voiceVlanCfg != L7_NULLPTR)
  {
    osapiFree(L7_VOICE_VLAN_COMPONENT_ID, voiceVlanCfg);
    voiceVlanCfg = L7_NULLPTR;
  }

  if(voiceVlanInfo != L7_NULLPTR)
  {
    osapiFree(L7_VOICE_VLAN_COMPONENT_ID, voiceVlanInfo);
    voiceVlanInfo = L7_NULLPTR;
  }

  if(voiceVlanMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_VOICE_VLAN_COMPONENT_ID, voiceVlanMapTbl);
    voiceVlanMapTbl = L7_NULLPTR;
  }

  /*free voice vlan Database*/
  voiceVlanDeviceInfoDBDeInit();

  voiceVlanPortCnfgrState = VOICE_VLAN_PHASE_INIT_0;
}

/*********************************************************************
 * @purpose  This function undoes voiceVlanCnfgrInitPhase2Process
 *
 * @param    none
 *
 * @returns  none
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
static void voiceVlanCnfgrFiniPhase2Process()
{
  nimDeRegisterIntfChange(L7_VOICE_VLAN_COMPONENT_ID);
  nvStoreDeregister(L7_VOICE_VLAN_COMPONENT_ID);
  vlanDeregisterForChange(L7_VOICE_VLAN_COMPONENT_ID);
  lldpXMedNotificationDeregister(L7_VOICE_VLAN_COMPONENT_ID);

  voiceVlanPortCnfgrState = VOICE_VLAN_PHASE_INIT_1;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
static
L7_RC_t voiceVlanCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t * pResponse,
                                    L7_CNFGR_ERR_RC_t * pReason)
{
  L7_RC_t voiceVlanRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  voiceVlanRC = L7_SUCCESS;

  /* Restore any configuration */
  voiceVlanRestore();

  voiceVlanPortCnfgrState = VOICE_VLAN_PHASE_WMU;

  return voiceVlanRC;
}


/*********************************************************************
 * @purpose  This function undoes voiceVlanCnfgrInitPhase3Process
 *
 * @param    none
 *
 * @returns  none
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
static void voiceVlanCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t reason;

  /* this func will place voiceVlanCnfgrState to WMU */
  voiceVlanCnfgrUconfigPhase2(&response, &reason);
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
static
L7_RC_t voiceVlanCnfgrNoopProcess(L7_CNFGR_RESPONSE_t * pResponse,
                                  L7_CNFGR_ERR_RC_t * pReason)
{
  L7_RC_t voiceVlanRC = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return(voiceVlanRC);
}


/*********************************************************************
*
* @purpose  CNFGR System Initialization for Voice VLAN component
*
* @param    *pCmdData    @b{(input)} Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the  Voice Vlan comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void voiceVlanApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t command;
  L7_CNFGR_RQST_t request;
  L7_CNFGR_CB_DATA_t cbData;
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  L7_RC_t voiceVlanRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t reason = L7_CNFGR_ERR_RC_INVALID_PAIR;
  L7_BOOL warmRestart = L7_FALSE;

  /* validate command type */
  if (pCmdData != L7_NULL)
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command = pCmdData->command;
      request = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if (( request > L7_CNFGR_RQST_FIRST ) &&
          ( request < L7_CNFGR_RQST_LAST ))
      {
        /* validate command/event pair */
        switch (command)
        {
        case L7_CNFGR_CMD_INITIALIZE:
          switch (request)
          {
          case L7_CNFGR_RQST_I_PHASE1_START:
            voiceVlanRC = voiceVlanCnfgrInitPhase1Process(&response, &reason);
            if (voiceVlanRC != L7_SUCCESS)
            {
              voiceVlanCnfgrFiniPhase1Process();
            }
            break;

          case L7_CNFGR_RQST_I_PHASE2_START:
            voiceVlanRC = voiceVlanCnfgrInitPhase2Process(&response, &reason);
            if (voiceVlanRC != L7_SUCCESS)
            {
              voiceVlanCnfgrFiniPhase2Process();
            }
            break;

          case L7_CNFGR_RQST_I_PHASE3_START:
            warmRestart = ((pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM) ? L7_TRUE : L7_FALSE);
            voiceVlanRC = voiceVlanCnfgrInitPhase3Process(warmRestart, &response, &reason);
            if (voiceVlanRC != L7_SUCCESS)
            {
              voiceVlanCnfgrFiniPhase3Process();
            }
            break;

          case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
            voiceVlanRC = voiceVlanCnfgrNoopProcess(&response, &reason);
            voiceVlanPortCnfgrState = VOICE_VLAN_PHASE_WMU;
            break;
          default:
            /* invalid command/request pair */
            break;
          }           /* endswitch initialize requests */
          break;

        case L7_CNFGR_CMD_EXECUTE:
          switch (request)
          {
          case L7_CNFGR_RQST_E_START:
            voiceVlanPortCnfgrState = VOICE_VLAN_PHASE_EXECUTE;

            voiceVlanRC = L7_SUCCESS;
            response = L7_CNFGR_CMD_COMPLETE;
            reason = 0;
            break;

          default:
            /* invalid command/request pair */
            break;
          }
          break;

        case L7_CNFGR_CMD_UNCONFIGURE:
          switch (request)
          {
          case L7_CNFGR_RQST_U_PHASE1_START:
            voiceVlanRC         = voiceVlanCnfgrNoopProcess(&response, &reason);
            voiceVlanPortCnfgrState = VOICE_VLAN_PHASE_UNCONFIG_1;
            break;

          case L7_CNFGR_RQST_U_PHASE2_START:
            voiceVlanRC         = voiceVlanCnfgrUconfigPhase2(&response, &reason);
            voiceVlanPortCnfgrState = VOICE_VLAN_PHASE_UNCONFIG_2;
            break;

          default:
            /* invalid command/request pair */
            break;
          }
          break;

        case L7_CNFGR_CMD_TERMINATE:
        case L7_CNFGR_CMD_SUSPEND:
          voiceVlanRC = voiceVlanCnfgrNoopProcess(&response, &reason);
          break;

        default:
          reason = L7_CNFGR_ERR_RC_INVALID_CMD;
          break;
        }               /* endswitch command/event pair */
      }
      else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      }                   /* endif validate request */
    }
    else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
    }                       /* endif validate command type */
  }
  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;
  }                           /* check for command valid pointer */

  /* return value to caller -
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator = correlator;
  cbData.asyncResponse.rc = voiceVlanRC;
  if (voiceVlanRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason = reason;
  }
  cnfgrApiCallback(&cbData);

}

