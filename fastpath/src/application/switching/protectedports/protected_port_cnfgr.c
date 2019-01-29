/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_cnfgr.c
*
* @purpose   Protected port configurator file
*
* @component protectedPort
*
* @comments
*
* @create    6/6/2005
*
* @author    ryadagiri
*
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"

#include "protected_port_include.h"

osapiRWLock_t protectedPortCfgRWLock;
protectedPortCfg_t *protectedPortCfg = L7_NULLPTR;
protectedPortCnfgrState_t protectedPortCnfgrState = PROTECTED_PORT_PHASE_INIT_0;
extern PORTEVENT_MASK_t protectedPortEventMask_g;

/* Begin Function Declarations: protected_port_cnfgr.h */

/*********************************************************************
*
* @purpose  CNFGR System Initialization for Protected Port component
*
* @param    *pCmdData    @b{(input)} Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the Protected Port comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void protectedPortApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t command;
  L7_CNFGR_RQST_t request;
  L7_CNFGR_CB_DATA_t cbData;
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t protectedPortRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if (pCmdData != L7_NULL)
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command = pCmdData->command;
      request = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if ( ( request > L7_CNFGR_RQST_FIRST ) &&
           ( request < L7_CNFGR_RQST_LAST ) )
      {
        /* validate command/event pair */
        switch (command)
        {
        case L7_CNFGR_CMD_INITIALIZE:
          switch (request)
          {
          case L7_CNFGR_RQST_I_PHASE1_START:
            protectedPortRC = protectedPortCnfgrInitPhase1Process(&response, &reason);
            if (protectedPortRC != L7_SUCCESS)
            {
              protectedPortCnfgrFiniPhase1Process();
            }
            break;

          case L7_CNFGR_RQST_I_PHASE2_START:
            protectedPortRC = protectedPortCnfgrInitPhase2Process(&response, &reason);
            if (protectedPortRC != L7_SUCCESS)
            {
              protectedPortCnfgrFiniPhase2Process();
            }
            break;

          case L7_CNFGR_RQST_I_PHASE3_START:
            protectedPortRC = protectedPortCnfgrInitPhase3Process(&response, &reason);
            if (protectedPortRC != L7_SUCCESS)
            {
              protectedPortCnfgrFiniPhase3Process();
            }
            break;

          case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
            protectedPortRC = protectedPortCnfgrNoopProcess(&response, &reason);
            protectedPortCnfgrState = PROTECTED_PORT_PHASE_WMU;
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
            protectedPortCnfgrState = PROTECTED_PORT_PHASE_EXECUTE;

            protectedPortRC = L7_SUCCESS;
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
            protectedPortRC         = protectedPortCnfgrNoopProcess(&response, &reason);
            protectedPortCnfgrState = PROTECTED_PORT_PHASE_UNCONFIG_1;
            break;

          case L7_CNFGR_RQST_U_PHASE2_START:
            protectedPortRC         = protectedPortCnfgrUconfigPhase2(&response, &reason);
            protectedPortCnfgrState = PROTECTED_PORT_PHASE_UNCONFIG_2;
            break;

          default:
            /* invalid command/request pair */
            break;
          }
          break;

        case L7_CNFGR_CMD_TERMINATE:
        case L7_CNFGR_CMD_SUSPEND:
          protectedPortRC = protectedPortCnfgrNoopProcess(&response, &reason);
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
  cbData.asyncResponse.rc = protectedPortRC;
  if (protectedPortRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason = reason;
  }

  cnfgrApiCallback(&cbData);
}

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
L7_RC_t protectedPortCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t * pResponse,
                                            L7_CNFGR_ERR_RC_t * pReason)
{
  L7_RC_t protectedPortRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  protectedPortRC = L7_SUCCESS;
  /* Allocate and initialized memory for global data */

  protectedPortCfg =
  (protectedPortCfg_t *) osapiMalloc(L7_PROTECTED_PORT_COMPONENT_ID,
                                     (L7_uint32) sizeof(protectedPortCfg_t));

  if (protectedPortCfg == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    protectedPortRC = L7_ERROR;

    return protectedPortRC;
  }

  /* memset all memory to zero */
  memset((void *)protectedPortCfg, 0x00,
         (L7_uint32) sizeof(protectedPortCfg_t));

  if (osapiRWLockCreate(&protectedPortCfgRWLock,
                        OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "protectedPortCnfgrInitPhase1Process: Unable "
           "to create r/w lock for protectedPort. This appears when protectedPortCfgRWLock Fails\n");
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    protectedPortRC = L7_ERROR;

    return protectedPortRC;
  }

  protectedPortCnfgrState = PROTECTED_PORT_PHASE_INIT_1;
  return protectedPortRC;
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
L7_RC_t protectedPortCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t * pResponse,
                                            L7_CNFGR_ERR_RC_t * pReason)
{
    L7_RC_t protectedPortRC;
    nvStoreFunctionList_t notifyFunctionList;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
    protectedPortRC = L7_SUCCESS;
    memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
    notifyFunctionList.registrar_ID = L7_PROTECTED_PORT_COMPONENT_ID;
    notifyFunctionList.notifySave = protectedPortSave;
    notifyFunctionList.hasDataChanged = protectedPortHasDataChanged;
    notifyFunctionList.notifyConfigDump = protectedPortCfgDump;
    notifyFunctionList.resetDataChanged = protectedPortResetDataChanged;

    if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason = L7_CNFGR_ERR_RC_FATAL;
        protectedPortRC = L7_ERROR;

        return protectedPortRC;
    }

    /* register callback with NIM for interface changes */
    if (nimRegisterIntfChange(L7_PROTECTED_PORT_COMPONENT_ID,
                              protectedPortIntfChangeCallback,
                              protectedPortStartupNotifyCallback,
                              NIM_STARTUP_PRIO_PROTECTED_PORT) != L7_SUCCESS)
    {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "protectedPortCnfgrInitPhase2Process: Unable "
                "to register for VLAN change callback! This appears when nimRegisterIntfChange with VLAN fails\n");
        *pResponse = 0;
        *pReason = L7_CNFGR_ERR_RC_FATAL;
        protectedPortRC = L7_ERROR;

        return protectedPortRC;
    }

    protectedPortCnfgrState = PROTECTED_PORT_PHASE_INIT_2;
    return protectedPortRC;
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
L7_RC_t protectedPortCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t * pResponse,
                                            L7_CNFGR_ERR_RC_t * pReason)
{
    L7_RC_t protectedPortRC;

    /* Clear the NIM port event mask until ready to receive events */
    memset(&protectedPortEventMask_g, 0, sizeof(protectedPortEventMask_g));
    nimRegisterIntfEvents(L7_PROTECTED_PORT_COMPONENT_ID, protectedPortEventMask_g);

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
    protectedPortRC = L7_SUCCESS;
    if (sysapiCfgFileGet(L7_PROTECTED_PORT_COMPONENT_ID,
                       PROTECTED_PORT_CFG_FILENAME,
                       (L7_char8 *)protectedPortCfg,
                       sizeof(protectedPortCfg_t),
                       &protectedPortCfg->checkSum,
                       PROTECTED_PORT_CFG_VER_CURRENT,
                       protectedPortBuildDefaultConfigData,
                       L7_NULL) != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason = L7_CNFGR_ERR_RC_FATAL;
        protectedPortRC = L7_ERROR;

        return protectedPortRC;
    }
    if (protectedPortApplyConfigData() != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason = L7_CNFGR_ERR_RC_FATAL;
        protectedPortRC = L7_ERROR;

        return protectedPortRC;
    }

    protectedPortCfg->cfgHdr.dataChanged = L7_FALSE;
    protectedPortCnfgrState = PROTECTED_PORT_PHASE_INIT_3;

    return protectedPortRC;
}


/*********************************************************************
* @purpose  This function undoes protectedPortCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void protectedPortCnfgrFiniPhase1Process()
{
    /* Deallocate anything that was allocated */
    if (protectedPortCfg != L7_NULLPTR)
    {
        osapiFree(L7_PROTECTED_PORT_COMPONENT_ID, protectedPortCfg);
        protectedPortCfg = L7_NULLPTR;
    }

    protectedPortCnfgrState = PROTECTED_PORT_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes protectedPortCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void protectedPortCnfgrFiniPhase2Process()
{
    nimDeRegisterIntfChange(L7_PROTECTED_PORT_COMPONENT_ID);
    nvStoreDeregister(L7_PROTECTED_PORT_COMPONENT_ID);
    protectedPortCnfgrState = PROTECTED_PORT_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes protectedPortCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void protectedPortCnfgrFiniPhase3Process()
{
    L7_CNFGR_RESPONSE_t response;
    L7_CNFGR_ERR_RC_t reason;

    /* this func will place protectedPortCnfgrState to WMU */
    protectedPortCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t protectedPortCnfgrNoopProcess(L7_CNFGR_RESPONSE_t * pResponse,
                                      L7_CNFGR_ERR_RC_t * pReason)
{
    L7_RC_t protectedPortRC = L7_SUCCESS;

    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
    return(protectedPortRC);
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
L7_RC_t protectedPortCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t * pResponse,
                                        L7_CNFGR_ERR_RC_t * pReason)
{
    L7_RC_t protectedPortRC;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
    protectedPortRC = L7_SUCCESS;

    /* Restore any configuration */
    protectedPortRestore();

    /* Clear out the configuration */
    memset(protectedPortCfg, 0, sizeof(protectedPortCfg_t));

    protectedPortCnfgrState = PROTECTED_PORT_PHASE_WMU;

    return protectedPortRC;
}

/* End Function Declarations */
