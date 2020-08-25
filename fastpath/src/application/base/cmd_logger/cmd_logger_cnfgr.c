


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cmd_logger_cnfgr.c
*
* @purpose    The functions that interface to the CNFGR component
*
* @component  cmdLogger
*
* @comments   none
*
* @create     25-Oct-2004
*
* @author     gaunce
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "cmd_logger_cnfgr.h"
#include "cmd_logger_include.h"
#include "cmd_logger_util.h"

/* The last phase that was completed */
static L7_CNFGR_RQST_t cmdLoggerPhase = L7_CNFGR_RQST_FIRST;
cmdLoggerCnfgrState_t cmdLoggerCnfgrState = CMD_LOGGER_PHASE_INIT_0;

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
void cmdLoggerApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request = 0;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             cmdLoggerRC    = L7_ERROR;
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
                if ((cmdLoggerRC = cmdLoggerCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  cmdLoggerPhaseOneFini();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((cmdLoggerRC = cmdLoggerCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  cmdLoggerPhaseTwoFini();  
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:  /* no configuration data to be read */
                if ((cmdLoggerRC = cmdLoggerCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  cmdLoggerPhaseThreeFini(); 
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                cmdLoggerRC = cmdLoggerCnfgrNoopProccess( &response, &reason );
                cmdLoggerCnfgrState = CMD_LOGGER_PHASE_WMU;
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

		case L7_CNFGR_CMD_EXECUTE:
            cmdLoggerRC = cmdLoggerCnfgrNoopProccess( &response, &reason );
            cmdLoggerCnfgrState = CMD_LOGGER_PHASE_EXECUTE;
            break;
          case L7_CNFGR_CMD_UNCONFIGURE:
              switch ( request )
              {
              case L7_CNFGR_RQST_U_PHASE1_START:
                  cmdLoggerRC = cmdLoggerCnfgrUnconfigureProcess(&response,&reason); 
                      cmdLoggerCnfgrState = CMD_LOGGER_PHASE_UNCONFIG_1;
                  break;

                case L7_CNFGR_RQST_U_PHASE2_START:
                  cmdLoggerRC = cmdLoggerCnfgrNoopProccess( &response, &reason );
                  cmdLoggerCnfgrState = CMD_LOGGER_PHASE_UNCONFIG_2;
                  break;

                default:
                  /* invalid command/request pair */
                  break;
              } /* endswitch initialize requests */
              break;
          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            cmdLoggerRC = cmdLoggerCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = cmdLoggerRC;

  if (cmdLoggerRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
    cmdLoggerPhase = request;
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
 *             cmdLogger cnfgr Internal Function Calls
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
L7_RC_t cmdLoggerCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t cmdLoggerRC = L7_SUCCESS;

  cmdLoggerRC = cmdLoggerPhaseOneInit();

  if (cmdLoggerRC != L7_SUCCESS)
  {
    cmdLoggerRC   = L7_ERROR;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  = 0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  cmdLoggerCnfgrState = CMD_LOGGER_PHASE_INIT_1;
  return(cmdLoggerRC);
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
L7_RC_t cmdLoggerCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = cmdLoggerPhaseTwoInit();

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

  cmdLoggerCnfgrState = CMD_LOGGER_PHASE_INIT_2;
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
L7_RC_t cmdLoggerCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = cmdLoggerPhaseThreeInit();

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

  cmdLoggerCnfgrState = CMD_LOGGER_PHASE_INIT_3;
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
L7_RC_t cmdLoggerCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t cmdLoggerRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(cmdLoggerRC);
}

/*********************************************************************
* @purpose  Transitions the cmdLogger to configurator state WMU from PHASE 3
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
L7_RC_t cmdLoggerCnfgrUnconfigureProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t cmdLoggerRC = L7_SUCCESS;

  cmdLoggerPhaseThreeFini();

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(cmdLoggerRC);
}

/*********************************************************************
* @purpose  This function returns the phase that cmdLogger believes it has completed
*
* @param    void
*
* @returns  L7_CNFGR_RQST_t - the last phase completed
*
* @notes    Until cmdLogger has completed phase 1, the value will be L7_CNFGR_RQST_FIRST
*
* @end
*********************************************************************/
L7_CNFGR_RQST_t cmdLoggerCnfgrPhaseGet(void)
{
  return(cmdLoggerPhase);
}


