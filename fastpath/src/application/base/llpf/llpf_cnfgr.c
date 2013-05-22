/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
* @filename  llpf_cnfgr.c
*
* @purpose   Link Local Protocol Filtering Configurator file
*
* @component LLPF
*
* @comments  None
*
* @create    05/10/2009
*
* @author    Vijayanand K (kvijayan)
*
* @end
*
**********************************************************************/
/* Standard header file includes */
#include <flex.h>
#include <string.h>

/* Common header file includes */
#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "osapi.h"

/* Component related header file includes */
#include "llpf_cnfgr.h"
#include "llpf_cfg.h"
#include "llpf_util.h"

/* Semaphore Lock for Config Critical Section synchronization */
osapiRWLock_t llpfCfgRWLock;

llpfCnfgrState_t llpfCnfgrState = LLPF_PHASE_INIT_0;

/* Bit mask of NIM events that LLPF is registered to receive */
PORTEVENT_MASK_t llpfPortEventMask_g;

llpfCfgData_t *llpfCfgData = L7_NULLPTR;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for LLPF component
*
* @param    *pCmdData   @b{(input)} Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the LLPF comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void llpfApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{

 /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  
  L7_RC_t             llpfRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                if ((llpfRC = llpfCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  llpfCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((llpfRC = llpfCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  llpfCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((llpfRC = llpfCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  llpfCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                llpfRC = llpfCnfgrNoopProcess( &response, &reason );
                llpfCnfgrState = LLPF_PHASE_WMU;
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
                llpfCnfgrState = LLPF_PHASE_EXECUTE;

                llpfRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = 0;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                llpfRC = llpfCnfgrNoopProcess( &response, &reason );
                llpfCnfgrState = LLPF_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                llpfRC = llpfCnfgrUconfigPhase2( &response, &reason );
                llpfCnfgrState = LLPF_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            llpfRC = llpfCnfgrNoopProcess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */

      } else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;

      } /* endif validate request */
    } else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;

    } /* endif validate command type */

  } else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;

  } /* check for command valid pointer */

  /* return value to caller -
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = llpfRC;
  if (llpfRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);


  return; 
}

/*********************************************************************
* C@purpose  This function process the configurator control commands/request
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
L7_RC_t llpfCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
   L7_RC_t llpfRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  llpfRC     = L7_SUCCESS;


  /* Allocate memory for global data */
  llpfCfgData = osapiMalloc(L7_LLPF_COMPONENT_ID, sizeof(llpfCfgData_t));

  /* Make sure that allocation succeded */
  if (llpfCfgData == L7_NULL)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    llpfRC     = L7_ERROR;

    return llpfRC;
  }

  memset(llpfCfgData, 0, sizeof(llpfCfgData_t));

  if (osapiRWLockCreate(&llpfCfgRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLPF_COMPONENT_ID,
            "llpfCnfgrInitPhase1Process: Unable to create r/w lock for Protocol Filtering\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    llpfRC     = L7_ERROR;

    return llpfRC;
  }

  llpfCnfgrState = LLPF_PHASE_INIT_1;

  return llpfRC;

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
L7_RC_t llpfCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t llpfRC;
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  llpfRC     = L7_SUCCESS;

  memset(&notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_LLPF_COMPONENT_ID;
  notifyFunctionList.notifySave     = (void *)llpfSave;
  notifyFunctionList.hasDataChanged = (void *)llpfHasDataChanged;
  notifyFunctionList.resetDataChanged = (void *)llpfResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    llpfRC     = L7_ERROR;

    return llpfRC;
  }

  /* TBD the priority of this need to be discussed */
  if (nimRegisterIntfChange(L7_LLPF_COMPONENT_ID,
                            llpfIntfChangeCallback,
                            llpfStartupNotifyCallback,
                            NIM_STARTUP_PRIO_DEFAULT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLPF_COMPONENT_ID,
            "L7_LLPF_COMPONENT_ID: Unable to register with NIM\n");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    llpfRC     = L7_ERROR;

    return llpfRC;
  }

  llpfCnfgrState = LLPF_PHASE_INIT_2;

  return llpfRC;

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
L7_RC_t llpfCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  
  L7_RC_t llpfRC;

  /* Clear the NIM port event mask until ready to receive events */
  memset(&llpfPortEventMask_g, 0, sizeof(llpfPortEventMask_g));
  nimRegisterIntfEvents(L7_LLPF_COMPONENT_ID, llpfPortEventMask_g);

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  llpfRC     = L7_SUCCESS;

  llpfBuildDefaultConfigData(LLPF_CFG_VER_CURRENT);

  /* Implement a Apply Default Config Data if needed */ 
  llpfCfgData->cfgHdr.dataChanged = L7_FALSE;

  llpfCnfgrState = LLPF_PHASE_INIT_3;

  return llpfRC;
  
}

/*********************************************************************
* @purpose  This function undoes llpfCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void llpfCnfgrFiniPhase1Process()
{
  /* Deallocate anything that was allocated */

  if (llpfCfgData != L7_NULL)
  {
    osapiFree(L7_LLPF_COMPONENT_ID, llpfCfgData);
    osapiRWLockDelete(llpfCfgRWLock);
    llpfCfgData = L7_NULL;
  }

  llpfCnfgrState = LLPF_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes llpfCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void llpfCnfgrFiniPhase2Process()
{

  /*
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now
   * a temporary solution is set in place, where in the registered
   * function if we can execute the callback only if it's corresponding
   * member in the llpfDeregister is set to L7_FALSE;
   */

  (void)nimDeRegisterIntfChange(L7_LLPF_COMPONENT_ID);
  nvStoreDeregister(L7_LLPF_COMPONENT_ID);
  llpfCnfgrState = LLPF_PHASE_INIT_1;
}
/*********************************************************************
* @purpose  This function undoes llpfCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void llpfCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place llpfCnfgrState to WMU */
  llpfCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t llpfCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t llpfRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(llpfRC);
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

L7_RC_t llpfCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason )
{

  L7_RC_t llpfRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  llpfRC      = L7_SUCCESS;
 
  llpfBuildDefaultConfigData(0);
 
  /* TBD */ 
  llpfCnfgrState = LLPF_PHASE_WMU;   
  
  return L7_SUCCESS;
}
