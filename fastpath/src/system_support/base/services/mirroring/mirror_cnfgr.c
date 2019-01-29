/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename mirror_cnfgr.c
*
* @purpose Provide the Configurator hooks for Initialization and Teardown
*
* @component mirroring
*
* @comments
*
* @create 06/03/2003
*
* @author mbaucom
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "sysapi.h"
#include "defaultconfig.h"
#include "l7_cnfgr_api.h"
#include "mirror_cnfgr.h"
#include "nvstoreapi.h"
#include "mirror.h"

extern L7_RC_t mirrorConfigDump(void);
extern mirrorCfgData_t   *mirrorCfgData;
/* semaphore */
extern void *intfListSema;

extern mirrorSessionInfo_t mirrorInfo[];
extern L7_BOOL firstSourcePort[L7_MIRRORING_MAX_SESSIONS];
extern PORTEVENT_MASK_t mirrorPortEventMask_g;
/* The last phase that was completed */
mirrorPhases_t mirrorState = MIRROR_PHASE_INIT_0;

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.
*
* @returns  None
*
* @notes    This function completes synchronously. The return value
*           is presented to the configurator by calling the cnfgrApiCallback().
*           The following are the possible return codes:
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
void mirrorApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response = L7_CNFGR_INVALID_RESPONSE;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             mirrorRC    = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if ( request > L7_CNFGR_RQST_FIRST &&
           request < L7_CNFGR_RQST_LAST )
      {
        /* validate command/event pair */
        switch ( command )
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch ( request )
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((mirrorRC = mirrorCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  mirrorPhaseOneFini();
                }
                else
                {
                  mirrorState = MIRROR_PHASE_INIT_1;
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((mirrorRC = mirrorCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  mirrorPhaseTwoFini();
                }
                else
                {
                  mirrorState = MIRROR_PHASE_INIT_2;
                }
                break;

              case L7_CNFGR_RQST_I_PHASE3_START:  /* no configuration data to be read */
                if ((mirrorRC = mirrorCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  mirrorPhaseThreeFini();
                }
                else
                {
                  mirrorState = MIRROR_PHASE_INIT_3;
                }
                break;

              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                mirrorRC = mirrorCnfgrNoopProccess( &response, &reason );
                mirrorState = MIRROR_PHASE_WMU;
                break;

              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            mirrorRC = mirrorCnfgrNoopProccess( &response, &reason );
            mirrorState = MIRROR_PHASE_EXECUTE;
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:

            switch (request)
            {
            case L7_CNFGR_RQST_U_PHASE1_START:
                mirrorRC = mirrorCnfgrNoopProccess( &response, &reason );
                mirrorState = MIRROR_PHASE_UNCONFIG_1;
                break;
            case L7_CNFGR_RQST_U_PHASE2_START:
                if ((mirrorRC = mirrorCnfgrUnconfigureProcess(&response,&reason)) == L7_SUCCESS)
                {
                  mirrorState = MIRROR_PHASE_UNCONFIG_2;
                }
                break;
            default:
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            mirrorRC = mirrorCnfgrNoopProccess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */

      }
      else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;

      } /* endif validate request */

    }
    else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;

    } /* endif validate command type */

  }
  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;

  } /* check for command valid pointer */

  /* return value to caller -
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = mirrorRC;

  if (mirrorRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason   = reason;
  }

  cnfgrApiCallback(&cbData);

  return;
}

/*
 *********************************************************************
 *             mirror cnfgr Internal Function Calls
 *********************************************************************
*/

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
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t mirrorCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t mirrorRC = L7_SUCCESS;

  mirrorRC = mirrorPhaseOneInit();

  if (mirrorRC != L7_SUCCESS)
  {
    mirrorRC   = L7_ERROR;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  = 0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(mirrorRC);
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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t mirrorCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = mirrorPhaseTwoInit();

  if (rc != L7_SUCCESS)
  {
    rc = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(rc);
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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t mirrorCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = mirrorPhaseThreeInit();

  if (rc != L7_SUCCESS)
  {
    rc = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(rc);
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
L7_RC_t mirrorCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t mirrorRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(mirrorRC);
}

/*********************************************************************
* @purpose  Transitions the mirror to configurator state WMU from PHASE 3
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t mirrorCnfgrUnconfigureProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t mirrorRC = L7_SUCCESS;

  mirrorPhaseThreeFini();

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(mirrorRC);
}

/*********************************************************************
* @purpose  This function returns the phase that mirror believes it has completed
*
* @param    void
*
* @returns  L7_CNFGR_RQST_t - the last phase completed
*
* @notes    Until mirror has completed phase 1, the value will be L7_CNFGR_RQST_FIRST
*
* @end
*********************************************************************/
L7_CNFGR_STATE_t mirrorCnfgrPhaseGet(void)
{
  return(mirrorState);
}

/*********************************************************************
* @purpose  phase 1 to Initialize mirroring
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   mirrorPhaseOneInit(void)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;

  /* create semaphores */
  /* Semaphore to regulate reads and writes to interface list array*/
  intfListSema = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);


  mirrorCfgData = osapiMalloc(L7_PORT_MIRROR_COMPONENT_ID, sizeof(mirrorCfgData_t));
  if( mirrorCfgData != L7_NULLPTR)
  {
    memset((void *)mirrorCfgData,0,sizeof(mirrorCfgData_t));
    rc = L7_SUCCESS;
  }
  memset((void *)mirrorInfo, 0, sizeof(mirrorSessionInfo_t));
  for (i = 0; i < L7_MIRRORING_MAX_SESSIONS; i++)
  {
	firstSourcePort[i] = L7_TRUE;
  }

  return rc;
}

/*********************************************************************
* @purpose  phase 2 to Initialize mirror component
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   mirrorPhaseTwoInit(void)
{
  L7_RC_t rc = L7_SUCCESS;
  nvStoreFunctionList_t mirrorNotifyFunctionList;

  /* Register Mirrorings NVRAM routines with the system */
  memset((void *) &mirrorNotifyFunctionList, 0, sizeof(mirrorNotifyFunctionList));
  mirrorNotifyFunctionList.registrar_ID   = L7_PORT_MIRROR_COMPONENT_ID;
  mirrorNotifyFunctionList.notifySave     = mirrorSave;
  mirrorNotifyFunctionList.hasDataChanged = mirrorHasDataChanged;
  mirrorNotifyFunctionList.notifyConfigDump     = mirrorConfigDump;
  mirrorNotifyFunctionList.resetDataChanged = mirrorResetDataChanged;

  if ((rc = nvStoreRegister(mirrorNotifyFunctionList)) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  /* register callbacks to NIM for port create/delete/insert */
  else if ((rc = nimRegisterIntfChange(L7_PORT_MIRROR_COMPONENT_ID,
                                       mirrorIntfChangeCallback,
                                       mirrorStartupNotifyCallback, 
                                       NIM_STARTUP_PRIO_MIRROR)) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    rc = L7_SUCCESS;
  }

  return(rc);
}

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 1 
*
* @param    void
*
* @returns  void
*
* @notes    none.
*
* @end
*********************************************************************/
void mirrorPhaseOneFini(void)
{
  /* interface list sema */
  if (intfListSema != L7_NULLPTR)
    osapiSemaDelete(intfListSema);


  if(mirrorCfgData != L7_NULLPTR)
    osapiFree(L7_PORT_MIRROR_COMPONENT_ID, mirrorCfgData);
  mirrorState = MIRROR_PHASE_INIT_0;
  return;
}

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 2
*
* @param    void
*
* @returns  void
*
* @notes    none.
*
* @end
*********************************************************************/
void mirrorPhaseTwoFini(void)
{
  (void)nimDeRegisterIntfChange(L7_PORT_MIRROR_COMPONENT_ID);
  (void)nvStoreDeregister(L7_PORT_MIRROR_COMPONENT_ID);
  mirrorState = MIRROR_PHASE_INIT_1;
  return;
}

/*********************************************************************
* @purpose  phase 3 to Initialize mirror component
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Read the configuration during phase 3
*
* @end
*********************************************************************/
L7_RC_t   mirrorPhaseThreeInit(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Clear the NIM port event mask until ready to receive events */
  memset(&mirrorPortEventMask_g, 0, sizeof(mirrorPortEventMask_g));
  nimRegisterIntfEvents(L7_PORT_MIRROR_COMPONENT_ID, mirrorPortEventMask_g);

  rc = sysapiCfgFileGet(L7_PORT_MIRROR_COMPONENT_ID, MIRROR_CFG_FILENAME, 
                        (L7_char8 *)mirrorCfgData, sizeof(mirrorCfgData_t), 
                        &mirrorCfgData->checkSum, MIRROR_CFG_VER_CURRENT, 
                        mirrorBuildDefaultConfigData, mirrorMigrateConfigData);
  mirrorCfgData->cfgHdr.dataChanged = L7_FALSE;
  mirrorApplyConfigData();

  rc = L7_SUCCESS;
  return(rc);
}


/*********************************************************************
* @purpose  Remove all resources acquired during Phase 3
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void mirrorPhaseThreeFini(void)
{
  L7_uint32 sessionCount, sessionIndex;
 
  sessionCount = L7_MIRRORING_MAX_SESSIONS; 

  /* configuration should not be accidently changed, so only disable the session */
  for(sessionIndex = 1; sessionIndex <= sessionCount; sessionIndex++)
  {
    mirrorConfigRemove(sessionIndex);
    memset((void *)&mirrorCfgData->sessionData[sessionIndex-1], 0,
           sizeof(mirrorSession_t));
  }
  mirrorCfgData->cfgHdr.dataChanged = L7_FALSE;
  memset((void *)mirrorInfo, 0, sizeof(mirrorSessionInfo_t)); 
  mirrorState = MIRROR_PHASE_WMU;  
  return;
}
