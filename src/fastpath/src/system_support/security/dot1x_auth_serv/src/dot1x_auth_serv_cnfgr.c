/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dot1x_auth_serv_cnfgr.c
*
* @purpose    The functions that interface to the CNFGR component
*
* @component  Dot1x Authentication Server component
*
* @comments   none
*
* @create     11/17/2009
*
* @author     pradeepk
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "dot1x_auth_serv_cnfgr.h"
#include "dot1x_auth_serv.h"
#include "dot1x_auth_serv_cfg.h"

/* The last phase that was completed */
static L7_CNFGR_RQST_t dot1xAuthServPhase = L7_CNFGR_RQST_FIRST;

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
void dot1xAuthServApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request = 0;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             rc    = L7_ERROR;
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
                if ((rc = dot1xAuthServCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  dot1xAuthServPhaseOneFini();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((rc = dot1xAuthServCnfgrInitPhase2Process(&response, &reason)) != L7_SUCCESS)
                {
                  dot1xAuthServPhaseTwoFini();
                }
                rc = dot1xAuthServCnfgrNoopProccess( &response, &reason );
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:  /* no configuration data to be read */
                if ((rc = dot1xAuthServCnfgrInitPhase3Process(&response, &reason)) != L7_SUCCESS)
                {
                  dot1xAuthServPhaseThreeFini();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rc = dot1xAuthServCnfgrNoopProccess( &response, &reason );
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            rc = dot1xAuthServCnfgrNoopProccess( &response, &reason );
            break;
          case L7_CNFGR_CMD_UNCONFIGURE:
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                rc = dot1xAuthServCnfgrNoopProccess( &response, &reason );
                break;
              case L7_CNFGR_RQST_U_PHASE2_START:
                rc = dot1xAuthServCnfgrUnConfigPhase1Process( &response, &reason );
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;
          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rc = dot1xAuthServCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = rc;

  if (rc == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
    dot1xAuthServPhase = request;
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
 *             Dot1x auth serv cnfgr Internal Function Calls
 *********************************************************************
*/ 

/*********************************************************************
 * @purpose Initialize the Dot1x user db for Phase 1
 *
 * @param   void
 *
 * @returns L7_SUCCESS  Phase 1 completed
 * @returns L7_FAILURE  Phase 1 incomplete
 *
 * @notes  If phase 1 is incomplete, it is up to the caller to call the fini
 *         function if desired.  If this fails, it is due to an inability to
 *         to acquire resources.
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServPhaseOneInit(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* config data initialized to zero */
  memset(&dot1xAuthServCfgData, 0, sizeof(dot1xAuthServCfgData));

  /* Create Dot1x User DB Semaphore*/
  /* Read write lock for controlling User additions and Deletions */
  if (osapiRWLockCreate(&dot1xAuthServUserDBRWLock,
                        OSAPI_RWLOCK_Q_FIFO) == L7_FAILURE)
  {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1X_AUTH_SERV_COMPONENT_ID, 
              "Error creating dot1xAuthServUserDBRWLock semaphore \n");
      return L7_FAILURE;
  }

  return(rc);
}

/*********************************************************************
 * @purpose  Release all resources collected during phase 1
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void dot1xAuthServPhaseOneFini(void)
{
  /* config data initialized to zero */
  memset(&dot1xAuthServCfgData, 0, sizeof(dot1xAuthServCfgData));

  /* Delete the Dot1x User DB Semaphore */
  (void)osapiRWLockDelete(dot1xAuthServUserDBRWLock);
}

/*********************************************************************
* @purpose  This function undoes dot1xAuthServCnfgrInitPhase2Process 
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dot1xAuthServPhaseTwoFini()
{
  nvStoreDeregister(L7_DOT1X_AUTH_SERV_COMPONENT_ID);
}
/*********************************************************************
* @purpose  This function undoes dot1xAuthServCnfgrInitPhase3Process 
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dot1xAuthServPhaseThreeFini()
{
  dot1xAuthServUnconfigure();
}

/*********************************************************************
 * @purpose  Phase userMgr to configurator phase unconfigurator
 *
 * @returns  void
 *
 * @notes    Nothing to apply for config, only reset the memory
 *
 * @end
 *********************************************************************/
void dot1xAuthServUnconfigure(void)
{
  /* config data initialized to zero */
  memset(&dot1xAuthServCfgData, 0, sizeof(dot1xAuthServCfgData));

  return;
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
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t dot1xAuthServCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = dot1xAuthServPhaseOneInit();

  if (rc != L7_SUCCESS)
  {
    rc   = L7_ERROR;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  = 0;
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
L7_RC_t dot1xAuthServCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc;
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  rc     = L7_SUCCESS;

  memset(&notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_DOT1X_AUTH_SERV_COMPONENT_ID;
  notifyFunctionList.notifySave     = (void *)dot1xAuthServSave;
  notifyFunctionList.hasDataChanged = (void *)dot1xAuthServHasDataChanged;
  notifyFunctionList.resetDataChanged = (void *)dot1xAuthServResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    rc     = L7_ERROR;

    return rc;
  }

  return rc;
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
L7_RC_t dot1xAuthServCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{

  L7_RC_t rc;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  rc     = L7_SUCCESS;

  dot1xAuthServBuildDefaultConfigData(DOT1X_AUTH_SERV_CFG_VER_CURRENT);

  /* Implement a Apply Default Config Data if needed */
  dot1xAuthServCfgData.cfgHdr.dataChanged = L7_FALSE;

  return rc;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 1.
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
L7_RC_t dot1xAuthServCnfgrUnConfigPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                           L7_CNFGR_ERR_RC_t   *pReason )
{
  dot1xAuthServUnconfigure();

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  return(L7_SUCCESS);
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
L7_RC_t dot1xAuthServCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(rc);
}

