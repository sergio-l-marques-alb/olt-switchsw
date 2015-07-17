
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_cnfgr.c
*
* @purpose    The functions that interface to the CNFGR component
*
* @component  NIM
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
#include "sysapi_hpc.h"
#include "dtlapi.h"
#include "dtl_cnfgr.h"
#include "dtlinclude.h"
#include "dtl_pkg.h"
#include "nvstoreapi.h"
#include "bspapi.h"
#include <string.h>

/* The last phase that was completed */
static L7_CNFGR_RQST_t dtlPhase = L7_CNFGR_RQST_FIRST;
static L7_uint32  dtlTaskID;

static L7_BOOL dtl_started = L7_FALSE;


/*********************************************************************
* @purpose  Determine whether DTL has been started.
*
*********************************************************************/
L7_BOOL dtlCnfgrStarted (void)
{
  return dtl_started;
}


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
void dtlApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request = 0;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             dtlRC    = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  dtl_started = L7_TRUE;

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
                if ((dtlRC = dtlCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  dtlCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                dtlRC = dtlCnfgrInitPhase2Process( &response, &reason );

                break;
              case L7_CNFGR_RQST_I_PHASE3_START:  /* no configuration data to be read */
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                dtlRC = dtlCnfgrNoopProccess( &response, &reason );
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
          case L7_CNFGR_CMD_UNCONFIGURE:
          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            dtlRC = dtlCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = dtlRC;

  if (dtlRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
    dtlPhase = request;
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
 *             DTL cnfgr Internal Function Calls
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
L7_RC_t dtlCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dtlRC = L7_SUCCESS;
  L7_ulong32 cpuBoardID;
  L7_uint32 i;

  /* retrieve the base CPU board ID */
  cpuBoardID = bspapiCpuBoardIdGet();

  if ((dtlRC = dtlStartTask(&dtlTaskID)) == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
            "DTL: Task failed to start\n");
  }
  else if ((dtlRC = dapiInit(cpuBoardID)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
            "DTL: Failed to initialize DAPI\n");
  }
  else
  {
    for (i=0; i<DTL_NUM_EVENT; i++)
    {
      dtlEventHandle[i] = dtlRegistryCtrlCreate();
      if (dtlEventHandle[i] == L7_NULLPTR)
      {
        dtlRC       =  L7_ERROR;
        break;
      }
    }
  }

  if (dtlRC != L7_SUCCESS)
  {
    dtlRC = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(dtlRC);
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
L7_RC_t dtlCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dtlRC = L7_SUCCESS;

  /* Register with outside components during phase 2 */
  do
  {
    if ((dtlRC = dtlIpRegPkg()) != L7_SUCCESS)
      break;

    LOG_INFO(LOG_CTX_STARTUP, "Going to registering function 0x%08x to family DTL_FAMILY_INTF_MGMT (%u)", (L7_uint32) dtlLinkChangeCallback, DTL_FAMILY_INTF_MGMT);
    if ((dtlRC = dtlCallbackRegistration(DTL_FAMILY_INTF_MGMT,dtlLinkChangeCallback)) != L7_SUCCESS)
      break;

    LOG_INFO(LOG_CTX_STARTUP, "Going to register function 0x%08x to family DTL_FAMILY_FRAME (%u)", (L7_uint32) dtlPduReceiveCallback, DTL_FAMILY_FRAME);
    if ((dtlRC = dtlCallbackRegistration(DTL_FAMILY_FRAME,dtlPduReceiveCallback)) != L7_SUCCESS)
      break;

    LOG_INFO(LOG_CTX_STARTUP, "Going to register function 0x%08x to family DTL_FAMILY_QVLAN_MGMT (%u)", (L7_uint32) dtlGenericCallback, DTL_FAMILY_QVLAN_MGMT);
    if ((dtlRC = dtlCallbackRegistration(DTL_FAMILY_QVLAN_MGMT,dtlGenericCallback)) != L7_SUCCESS)
      break;

    LOG_INFO(LOG_CTX_STARTUP, "Going to register function 0x%08x to family DTL_FAMILY_SERVICES (%u)", (L7_uint32) dtlGenericCallback, DTL_FAMILY_SERVICES);
    if ((dtlRC = dtlCallbackRegistration(DTL_FAMILY_SERVICES,dtlGenericCallback)) != L7_SUCCESS)
     break;

  } while ( 0 );


  if ( dtlRC != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    dtlRC      = L7_ERROR;
  }
  else
  {
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  /* Return Value to caller */
  return(dtlRC);
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
L7_RC_t dtlCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dtlRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(dtlRC);
}

/*********************************************************************
* @purpose  This function returns the phase that Dtl believes it has completed
*
* @param    void
*
* @returns  L7_CNFGR_RQST_t - the last phase completed
*
* @notes    Until DTL has completed phase 1, the value will be L7_CNFGR_RQST_FIRST
*
* @end
*********************************************************************/
L7_CNFGR_RQST_t dtlCnfgrPhaseGet(void)
{
  return(dtlPhase);
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
void dtlCnfgrFiniPhase1Process(void)
{
  L7_uint32 i;

  /* kill the task if it was created */
  if (dtlTaskID != L7_ERROR)
  {
    osapiTaskDelete(dtlTaskID);
  }

  /* kill the queues if they are created */
  if (dtlMsgQueue != L7_NULL)
  {
    osapiMsgQueueDelete(dtlMsgQueue);
  }


  /* get rid of these allocations */
  for (i=0; i<DTL_NUM_EVENT; i++)
  {
    if (dtlEventHandle[i] != L7_NULLPTR)
    {
      dtlRegistryCtrlDelete(dtlEventHandle[i]);
    }
  }

}

/*********************************************************************
* @purpose  To Verify the ready state of dtl
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dtlIsReady(void)
{
  return (((dtlPhase == L7_CNFGR_RQST_I_PHASE3_START) ||
          (dtlPhase == L7_CNFGR_RQST_E_START) ||
          (dtlPhase == L7_CNFGR_RQST_U_PHASE1_START)) ? (L7_TRUE) : (L7_FALSE));

}



