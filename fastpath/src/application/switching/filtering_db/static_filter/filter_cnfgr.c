/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename filter_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component filter
*
* @comments 
*
* @create 07/26/2003
*
* @author msmith
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "filter.h"
#include "filter_cnfgr.h"

filterCnfgrState_t filterCnfgrState = FILTER_PHASE_INIT_0;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for filter component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the filter comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void filterApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             filterRC = L7_ERROR;
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
                if ((filterRC = filterCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  filterCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((filterRC = filterCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  filterCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((filterRC = filterCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  filterCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                filterRC = filterCnfgrNoopProcess( &response, &reason );
                filterCnfgrState = FILTER_PHASE_WMU;
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
                filterCnfgrState = FILTER_PHASE_EXECUTE;

                filterRC  = L7_SUCCESS;
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
                filterCnfgrState = FILTER_PHASE_UNCONFIG_1;
                filterRC = filterCnfgrUconfigPhase1( &response, &reason );
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                filterRC = filterCnfgrNoopProcess( &response, &reason );                
                filterCnfgrState = FILTER_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                filterRC = L7_ERROR;
                reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
                break;
            }
            break;

        case L7_CNFGR_CMD_TERMINATE:
        case L7_CNFGR_CMD_SUSPEND:
            filterRC = filterCnfgrNoopProcess( &response, &reason );
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
  cbData.asyncResponse.rc = filterRC;
  if (filterRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);


  return;
}

