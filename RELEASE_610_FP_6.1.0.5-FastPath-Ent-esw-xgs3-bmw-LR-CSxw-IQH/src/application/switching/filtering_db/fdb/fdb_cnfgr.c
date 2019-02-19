

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   fdb_cnfgr.c
*
* @purpose    The functions that interface to the CNFGR component
*
* @component  FDB
*
* @comments   none
*
* @create     05/07/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include <string.h>
#include "fdb.h"

/* The last phase that was completed */
static L7_CNFGR_STATE_t fdbState = L7_CNFGR_STATE_NULL;


/* Prototypes for this module only */
L7_RC_t fdbCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

void    fdbCnfgrFiniPhase1Process(void);

L7_RC_t fdbCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

void    fdbCnfgrFiniPhase2Process(void);

L7_RC_t fdbCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

void    fdbCnfgrFiniPhase3Process(void);

L7_RC_t fdbCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t fdbCnfgrInitPhaseExecProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );

void fdbCnfgrFiniPhaseExecProcess(void);

L7_CNFGR_STATE_t fdbCnfgrPhaseGet(void);

L7_RC_t fdbCnfgrUnconfigureProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );

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
void fdbApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             fdbRC    = L7_ERROR;
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
                if ((fdbRC = fdbCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  fdbCnfgrFiniPhase1Process();
                }
                else
                {
                  fdbState = L7_CNFGR_STATE_P1;
                }

                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((fdbRC = fdbCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  fdbCnfgrFiniPhase2Process();
                }
                else
                {
                  fdbState = L7_CNFGR_STATE_P2;
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:  /* no configuration data to be read */
                if ((fdbRC = fdbCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  fdbCnfgrFiniPhase3Process();
                }
                else
                {
                  fdbState = L7_CNFGR_STATE_P3;
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                fdbRC = fdbCnfgrNoopProccess( &response, &reason);
                fdbState = L7_CNFGR_STATE_WMU;
                break;
                                             
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            if ((fdbRC = fdbCnfgrInitPhaseExecProcess( &response, &reason )) != L7_SUCCESS)
            {
              fdbCnfgrFiniPhaseExecProcess();
            }
            else 
            {
              fdbState = L7_CNFGR_STATE_E;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch (request)
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                if ((fdbRC = fdbCnfgrNoopProccess( &response, &reason)) == L7_SUCCESS)
                  fdbState = L7_CNFGR_STATE_U1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                if ((fdbRC = fdbCnfgrUnconfigureProcess( &response, &reason)) == L7_SUCCESS)
                  fdbState = L7_CNFGR_STATE_U2;
                break;

              default:
                break;
            }            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            fdbRC = fdbCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = fdbRC;

  if (fdbRC == L7_SUCCESS)
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
 *             FDB cnfgr Internal Function Calls
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
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t fdbCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t fdbRC = L7_SUCCESS;

  fdbRC = fdbPhaseOneInit();

  if (fdbRC != L7_SUCCESS)
  {
    fdbRC = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(fdbRC);
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
void fdbCnfgrFiniPhase1Process(void)
{
  fdbPhaseOneFini();
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
L7_RC_t fdbCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t fdbRC = L7_SUCCESS;

  fdbRC = fdbPhaseTwoInit();

  if (fdbRC != L7_SUCCESS)
  {
    fdbRC = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return fdbRC;
}

/*********************************************************************
* @purpose  Release all resources collected during phase 2
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void fdbCnfgrFiniPhase2Process(void)
{
  fdbPhaseTwoFini();

  return;
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
L7_RC_t fdbCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t fdbRC = L7_SUCCESS;

  fdbRC = fdbPhaseThreeInit();

  if (fdbRC != L7_SUCCESS)
  {
    fdbRC = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return fdbRC;

}

/*********************************************************************
* @purpose  Release all resources collected during phase 3
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void fdbCnfgrFiniPhase3Process(void)
{
  (void)fdbPhaseThreeFini();

  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase Exec.
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
L7_RC_t fdbCnfgrInitPhaseExecProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t fdbRC = L7_SUCCESS;

  fdbRC = fdbPhaseExecInit();

  if (fdbRC != L7_SUCCESS)
  {
    fdbRC = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return fdbRC;

}

/*********************************************************************
* @purpose  Release all resources collected during phase Exec
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void fdbCnfgrFiniPhaseExecProcess(void)
{
  fdbPhaseExecFini();

  return;
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
L7_RC_t fdbCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t fdbRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(fdbRC);
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
L7_RC_t fdbCnfgrUnconfigureProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t fdbRC = L7_SUCCESS;

  fdbRC = fdbPhaseUnconfig();

  if (fdbRC != L7_SUCCESS)
  {
    fdbRC = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return fdbRC;
}
/*********************************************************************
* @purpose  This function returns the phase that Fdb believes it has completed
*
* @param    void
*
* @returns  L7_CNFGR_RQST_t - the last phase completed
*
* @notes    Until FDB has completed phase 1, the value will be L7_CNFGR_RQST_FIRST
*
* @end
*********************************************************************/
L7_CNFGR_STATE_t fdbCnfgrPhaseGet(void)
{
  return(fdbState);
}


