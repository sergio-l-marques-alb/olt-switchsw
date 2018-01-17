/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   edb_cnfgr.c
*
* @purpose    The functions that interface to the CNFGR component
*
* @component  edb
*
* @comments   adapted from usrMgr_cnfgr.c
*
* @create     06/30/2003
*
* @author     cpverne
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "edb.h"
#include <string.h>

/* The last phase that was completed */
static L7_CNFGR_STATE_t edbState = L7_CNFGR_STATE_NULL;


/* Prototypes for this module only */
L7_RC_t edbCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

void    edbCnfgrFiniPhase1Process(void);

L7_RC_t edbCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

void    edbCnfgrFiniPhase2Process(void);

L7_RC_t edbCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

void    edbCnfgrFiniPhase3Process(void);

L7_RC_t edbCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );

L7_CNFGR_STATE_t edbCnfgrPhaseGet(void);

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
void edbApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response = L7_CNFGR_INVALID_RESPONSE;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             edbRC    = L7_ERROR;
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
                if ((edbRC = edbCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  edbCnfgrFiniPhase1Process();
                }
                else
                {
                  edbState = L7_CNFGR_STATE_P1;
                }
  
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((edbRC = edbCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  edbCnfgrFiniPhase2Process();
                }
                else
                {
                  edbState = L7_CNFGR_STATE_P2;
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:  /* no configuration data to be read */
                if ((edbRC = edbCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  edbCnfgrFiniPhase3Process();
                }
                else
                {
                  edbState = L7_CNFGR_STATE_P3;
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                edbRC = edbCnfgrNoopProccess( &response, &reason);
                edbState = L7_CNFGR_STATE_WMU;
                break;
  
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            edbRC = edbCnfgrNoopProccess( &response, &reason );
            edbState = L7_CNFGR_STATE_E;
            break;
          case L7_CNFGR_CMD_UNCONFIGURE:
            edbRC = edbCnfgrNoopProccess( &response, &reason );
            edbState = L7_CNFGR_STATE_U2;
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            edbRC = edbCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = edbRC;

  if (edbRC == L7_SUCCESS)
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
 *             edb cnfgr Internal Function Calls
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
L7_RC_t edbCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t edbRC = L7_SUCCESS;

  edbRC = edbPhaseOneInit();

  if (edbRC != L7_SUCCESS)
  {
    edbRC   = L7_ERROR;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  = 0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(edbRC);
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
L7_RC_t edbCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = edbPhaseTwoInit();

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
L7_RC_t edbCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = edbPhaseThreeInit();

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
L7_RC_t edbCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t edbRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(edbRC);
}

/*********************************************************************
* @purpose  This function returns the phase that edb believes it has completed
*
* @param    void
*
* @returns  L7_CNFGR_RQST_t - the last phase completed
*
* @notes    Until edb has completed phase 1, the value will be L7_CNFGR_RQST_FIRST
*
* @end
*********************************************************************/
L7_CNFGR_STATE_t edbCnfgrPhaseGet(void)
{
  return(edbState);
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
void edbCnfgrFiniPhase1Process(void)
{
  edbPhaseOneFini();
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
void edbCnfgrFiniPhase2Process(void)
{
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
void edbCnfgrFiniPhase3Process(void)
{
}


