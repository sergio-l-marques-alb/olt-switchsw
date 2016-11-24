/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename openssl_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component openssl
*
* @comments none
*
* @create 11/09/2006
*
* @author jshaw
*
* @end
*
**********************************************************************/
#include "l7_cnfgr_api.h"
#include "nvstoreapi.h"
#include "cnfgr.h"
#include "openssl_cnfgr.h"
#include "openssl_util.h"
#include "openssl_cfg.h"
#include "osapi.h"

opensslCnfgrState_t opensslCnfgrState = OPENSSL_PHASE_INIT_0;
void *opensslDhSema = L7_NULLPTR;

/*********************************************************************
*
* @purpose  CNFGR System Initialization
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the OPENSSL comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void opensslApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /*
     Here we don't spawn a separate task since the only possible function
     it would serve is to initialize the library and semaphores.
   */
  opensslCnfgrParse(pCmdData);
}

/*********************************************************************
*
* @purpose  System Init Undo for OPENSSL component
*
* @comments none
*
* @end
*********************************************************************/
void opensslInitUndo()
{
  opensslSemaCleanup();
  ERR_free_strings();
  EVP_cleanup();
  CRYPTO_cleanup_all_ex_data();
  opensslCnfgrState = OPENSSL_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t opensslCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t opensslRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = L7_NULL;
  opensslRC      = L7_SUCCESS;

  opensslDhSema = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (opensslDhSema == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  if (opensslInit() != L7_SUCCESS)
  {
    opensslRC = L7_ERROR;
    return opensslRC;
  }

  opensslCnfgrState = OPENSSL_PHASE_INIT_1;

  return opensslRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t opensslCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t opensslRC;
  nvStoreFunctionList_t opensslNotifyFunctionList;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = L7_NULL;
  opensslRC      = L7_SUCCESS;

  memset((void *) &opensslNotifyFunctionList, L7_NULL, sizeof(opensslNotifyFunctionList));
  opensslNotifyFunctionList.registrar_ID   = L7_FLEX_OPENSSL_COMPONENT_ID;
  opensslNotifyFunctionList.notifySave     = opensslSave;
  opensslNotifyFunctionList.hasDataChanged = opensslHasDataChanged;
  opensslNotifyFunctionList.resetDataChanged = opensslResetDataChanged;

  if (nvStoreRegister(opensslNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = L7_NULL;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    opensslRC      = L7_ERROR;

    return opensslRC;

  }
  opensslCnfgrState = OPENSSL_PHASE_INIT_2;

  return opensslRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t opensslCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t opensslRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = L7_NULL;
  opensslRC      = L7_SUCCESS;
  opensslCnfgrState = OPENSSL_PHASE_INIT_3;

  return opensslRC;
}

/*********************************************************************
* @purpose  This function undoes opensslCnfgrInitPhase1Process
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void opensslCnfgrFiniPhase1Process()
{
  opensslInitUndo();

  if (opensslDhSema != L7_NULLPTR)
  {
    osapiSemaDelete(opensslDhSema);
    opensslDhSema = L7_NULLPTR;
  }

  opensslCnfgrState = OPENSSL_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes opensslCnfgrInitPhase2Process
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void opensslCnfgrFiniPhase2Process()
{
  opensslCnfgrState = OPENSSL_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes opensslCnfgrInitPhase3Process
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void opensslCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t reason;

  /* this func will place opensslCnfgrState to WMU */
  opensslCnfgrUconfigPhase2(&response, &reason);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @comments The following are valid error reason code:
*           None.
*
* @end
*********************************************************************/
L7_RC_t opensslCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t opensslRC = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;

  return(opensslRC);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @end
*********************************************************************/
L7_RC_t opensslCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t opensslRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = L7_NULL;
  opensslRC      = L7_SUCCESS;
  opensslCnfgrState = OPENSSL_PHASE_WMU;

  return opensslRC;
}

/*********************************************************************
*
* @purpose  To parse the configurator commands send to opensslEvTask
*
* @returns
*
* @comments none
*
* @end
*********************************************************************/
void opensslCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             opensslRC = L7_ERROR;
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
                if ((opensslRC = opensslCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  opensslCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((opensslRC = opensslCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  opensslCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((opensslRC = opensslCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  opensslCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                opensslRC = opensslCnfgrNoopProccess( &response, &reason );
                opensslCnfgrState = OPENSSL_PHASE_WMU;
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
                opensslCnfgrState = OPENSSL_PHASE_EXECUTE;

                opensslRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = L7_NULL;
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
                opensslRC = opensslCnfgrNoopProccess( &response, &reason );
                opensslCnfgrState = OPENSSL_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                opensslRC = opensslCnfgrUconfigPhase2( &response, &reason );
                opensslCnfgrState = OPENSSL_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            opensslRC = opensslCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = opensslRC;
  if (opensslRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}

