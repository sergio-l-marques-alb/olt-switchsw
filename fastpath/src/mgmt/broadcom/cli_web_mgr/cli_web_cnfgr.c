/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename   cli_web_cnfgr.c
 *
 * @purpose    The functions that interface to the CNFGR component
 *
 * @component  cliWeb
 *
 * @comments   none
 *
 * @create     11/02/2003
 *
 * @author     smathew
 * @end
 *
 **********************************************************************/

#include <string.h>

#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "cli_web_cnfgr.h"
#include "cli_web_include.h"
#include "cli_web_util.h"

/* The last phase that was completed */
static L7_CNFGR_RQST_t cliWebPhase = L7_CNFGR_RQST_FIRST;
cliWebCnfgrState_t cliWebCnfgrState = CLI_WEB_PHASE_INIT_0;

extern L7_RC_t cliTxtCfgInit (L7_uint32 phase);

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
void cliWebApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request = 0;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             cliWebRC    = L7_ERROR;
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
		if ((cliWebRC = cliWebCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
		{
		  cliWebPhaseOneFini();
		}
		break;
	      case L7_CNFGR_RQST_I_PHASE2_START:
		if ((cliWebRC = cliWebCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
		{
		  cliWebPhaseTwoFini();  
		}
		break;
	      case L7_CNFGR_RQST_I_PHASE3_START:  /* no configuration data to be read */
		if ((cliWebRC = cliWebCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
		{
		  cliWebPhaseThreeFini(); 
		}
		break;
	      case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
		cliWebRC = cliWebCnfgrNoopProccess( &response, &reason );
		cliWebCnfgrState = CLI_WEB_PHASE_WMU;
		break;
	      default:
		/* invalid command/request pair */
		break;
	    } /* endswitch initialize requests */
	    break;

	  case L7_CNFGR_CMD_EXECUTE:
	    cliWebRC = cliWebCnfgrNoopProccess( &response, &reason );
	    cliWebCnfgrState = CLI_WEB_PHASE_EXECUTE;
	    break;
	  case L7_CNFGR_CMD_UNCONFIGURE:
	    switch ( request )
	    {
	      case L7_CNFGR_RQST_U_PHASE1_START:
		cliWebRC = cliWebCnfgrUnconfigureProcess(&response,&reason); 
		cliWebCnfgrState = CLI_WEB_PHASE_UNCONFIG_1;
		break;

	      case L7_CNFGR_RQST_U_PHASE2_START:
		cliWebRC = cliWebCnfgrNoopProccess( &response, &reason );
		cliWebCnfgrState = CLI_WEB_PHASE_UNCONFIG_2;
		break;

	      default:
		/* invalid command/request pair */
		break;
	    } /* endswitch initialize requests */
	    break;
	  case L7_CNFGR_CMD_TERMINATE:
	  case L7_CNFGR_CMD_SUSPEND:
	    cliWebRC = cliWebCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = cliWebRC;

  if (cliWebRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
    cliWebPhase = request;
  }
  else
  {
    cbData.asyncResponse.u.reason   = reason;
  }

  cnfgrApiCallback(&cbData);

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
L7_RC_t cliWebCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t cliWebRC = L7_SUCCESS;

  cliWebRC = cliWebPhaseOneInit();

  if (cliWebRC != L7_SUCCESS)
  {
    cliWebRC   = L7_ERROR;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  = 0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  cliWebCnfgrState = CLI_WEB_PHASE_INIT_1;

  cliTxtCfgInit(1);
  return(cliWebRC);
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
L7_RC_t cliWebCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = cliWebPhaseTwoInit();

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

  cliWebCnfgrState = CLI_WEB_PHASE_INIT_2;

  cliTxtCfgInit(2);
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
L7_RC_t cliWebCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = cliWebPhaseThreeInit();

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

  cliWebCnfgrState = CLI_WEB_PHASE_INIT_3;

  cliTxtCfgInit(3);
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
L7_RC_t cliWebCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t cliWebRC = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(cliWebRC);
}

/*********************************************************************
 * @purpose  Transitions the cliWeb to configurator state WMU from PHASE 3
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
L7_RC_t cliWebCnfgrUnconfigureProcess( L7_CNFGR_RESPONSE_t *pResponse,
    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t cliWebRC = L7_SUCCESS;

  cliWebPhaseThreeFini();

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(cliWebRC);
}

/*********************************************************************
 * @purpose  This function returns the phase that cliWeb believes it has completed
 *
 * @param    void
 *
 * @returns  L7_CNFGR_RQST_t - the last phase completed
 *
 * @notes    Until cliWeb has completed phase 1, the value will be L7_CNFGR_RQST_FIRST
 *
 * @end
 *********************************************************************/
L7_CNFGR_RQST_t cliWebCnfgrPhaseGet(void)
{
  return(cliWebPhase);
}

/*********************************************************************
 * @purpose  This function checks whether the CLIWEB component is ready
 *
 * @param    void
 *
 * @returns  L7_SUCCESS if ready
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebIsReady(void)
{
  if (cliWebCnfgrState == CLI_WEB_PHASE_INIT_0 || 
      cliWebCnfgrState == CLI_WEB_PHASE_INIT_1)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
