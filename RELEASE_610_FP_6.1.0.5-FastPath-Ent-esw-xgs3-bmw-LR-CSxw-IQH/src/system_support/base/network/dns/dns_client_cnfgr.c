/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dns_client_cnfgr.c
*
* @purpose Contains definitions to support the configurator API
*
* @component 
*
* @comments 
*
* @create 02/28/2005
*
* @author dfowler
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "nvstoreapi.h"
#include "cnfgr.h"
#include "buff_api.h"
#include "dns_client.h"
#include "dns_client_util.h"
#include "dns_client_cache.h"

dnsCnfgrState_t                  dnsCnfgrState = DNS_CLIENT_PHASE_INIT_0;
extern void                     *dnsQueue;
extern L7_int32                  dnsTaskId;
extern L7_int32                  dnsRxTaskId;

extern dnsCfgData_t             *dnsCfgData;   
extern dnsOprData_t             *dnsOprData;
extern dnsCacheData_t           *dnsCacheData;
extern dnsNotifyEntry_t         *dnsNotifyTbl;
extern void                     *dnsSemaphore;
extern L7_uint32                 dnsCachePoolId;
extern L7_RC_t dnsCfgDump(void);

/* used to implement blocking dnsNameLookup API */
void *dnsNameLookupSemaphore = L7_NULLPTR;
void *dnsNameLookupQueue     = L7_NULLPTR;


/*********************************************************************
*
* @purpose  CNFGR System Initialization for DNS client component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the component.  This function is re-entrant.
*
* @end
*********************************************************************/
void dnsApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  dnsMgmtMsg_t msg;

/*
 * Let all but PHASE 1 start fall through into an osapiMessageSend.
 * The application task will handle everything.
 * Phase 1 will do a osapiMessageSend after a few pre reqs have been
 * completed
 */

  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) && 
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* this function will invoke the message send for phase 1 */
    if (dnsInit(pCmdData) != L7_SUCCESS)
      dnsInitUndo();
  } else
  {
    memcpy(&msg.u.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.msgId = dnsMsgCnfgr;
    if (osapiMessageSend(dnsQueue, 
                         &msg, 
                         DNS_CLIENT_MSG_SIZE, 
                         L7_NO_WAIT, 
                         L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      LOG_MSG("dnsApiCnfgrCommand(): message send failed\n");
    }
  }
}


/*********************************************************************
*
* @purpose  System Initialization for DNS client component
*
* @param    none
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dnsInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  dnsMgmtMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;

  dnsQueue = (void *)osapiMsgQueueCreate(DNS_CLIENT_QUEUE, 
                                         DNS_CLIENT_MSG_COUNT, 
                                         DNS_CLIENT_MSG_SIZE);
  if (dnsQueue == L7_NULLPTR)
  {
    LOG_MSG("dnsInit: msgQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  dnsNameLookupQueue = (void *)osapiMsgQueueCreate(DNS_NAME_LOOKUP_QUEUE, 
                                                   DNS_NAME_LOOKUP_MSG_COUNT, 
                                                   DNS_NAME_LOOKUP_MSG_SIZE);
  if (dnsNameLookupQueue == L7_NULLPTR)
  {
    LOG_MSG("dnsInit: msgQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (dnsTaskStart() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  memcpy(&msg.u.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = dnsMsgCnfgr;
  if (osapiMessageSend(dnsQueue, 
                       &msg, 
                       DNS_CLIENT_MSG_SIZE, 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("dnsInit(): message send failed\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for DNS client component
*
* @param    none
*                            
* @notes    none
*
* @end
*********************************************************************/
void dnsInitUndo()
{
  if (dnsQueue != L7_NULLPTR)
    osapiMsgQueueDelete(dnsQueue);


  if (dnsTaskId != L7_ERROR)
    osapiTaskDelete(dnsTaskId);

  if (dnsRxTaskId != L7_ERROR)
    osapiTaskDelete(dnsRxTaskId);

  dnsCnfgrState = DNS_CLIENT_PHASE_INIT_0;
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
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t dnsCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse       = L7_CNFGR_CMD_COMPLETE;
  *pReason         = 0;

  dnsCfgData       = osapiMalloc(L7_DNS_CLIENT_COMPONENT_ID, sizeof(dnsCfgData_t));
  dnsOprData       = osapiMalloc(L7_DNS_CLIENT_COMPONENT_ID, sizeof(dnsOprData_t));
  dnsCacheData     = osapiMalloc(L7_DNS_CLIENT_COMPONENT_ID, sizeof(dnsCacheData_t));
  dnsNotifyTbl     = osapiMalloc(L7_DNS_CLIENT_COMPONENT_ID, L7_LAST_COMPONENT_ID * sizeof(dnsNotifyEntry_t));

  if ((dnsCfgData == L7_NULLPTR) ||
      (dnsOprData == L7_NULLPTR) ||
      (dnsCacheData == L7_NULLPTR) ||
      (dnsNotifyTbl == L7_NULLPTR))
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  memset((void *)dnsCfgData, 0, sizeof(dnsCfgData_t));
  memset((void *)dnsOprData, 0, sizeof(dnsOprData_t));
  memset((void *)dnsCacheData, 0, sizeof(dnsCacheData_t));
  memset((void *)dnsNotifyTbl, 0, L7_LAST_COMPONENT_ID * sizeof(dnsNotifyEntry_t));

  /* initialize our internal response function used for blocking dnsNameLookup API */
  dnsNotifyTbl[L7_DNS_CLIENT_COMPONENT_ID].notifyFunction = dnsNameLookupResponseMessageSend;

  /* allocate buffer pools for cache, ip address and cname buffers */
  if (bufferPoolInit(L7_DNS_CACHE_ENTRIES,
                     sizeof(dnsCacheEntry_t),
                     DNS_CACHE_ENTRY_BUF_DESC,
                     &dnsCachePoolId) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  dnsSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (dnsSemaphore == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  dnsNameLookupSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (dnsNameLookupSemaphore == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  dnsCnfgrState = DNS_CLIENT_PHASE_INIT_1;

  return L7_SUCCESS;
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
* @end
*********************************************************************/
L7_RC_t dnsCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason)
{
  nvStoreFunctionList_t dnsNotifyFunctionList;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  memset((void *)&dnsNotifyFunctionList, 0, sizeof(dnsNotifyFunctionList));
  dnsNotifyFunctionList.registrar_ID     = L7_DNS_CLIENT_COMPONENT_ID;
  dnsNotifyFunctionList.notifySave       = dnsSave;
  dnsNotifyFunctionList.hasDataChanged   = dnsHasDataChanged;
  dnsNotifyFunctionList.notifyConfigDump = dnsCfgDump;
  dnsNotifyFunctionList.resetDataChanged = dnsResetDataChanged;

  if (nvStoreRegister(dnsNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  dnsCnfgrState = DNS_CLIENT_PHASE_INIT_2;

  return L7_SUCCESS;
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
* @end
*********************************************************************/
L7_RC_t dnsCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  if (sysapiCfgFileGet(L7_DNS_CLIENT_COMPONENT_ID, DNS_CLIENT_CFG_FILENAME,
                       (L7_char8 *)dnsCfgData, sizeof(dnsCfgData_t),
                       &dnsCfgData->checkSum, DNS_CLIENT_CFG_VER_CURRENT,
                       dnsBuildDefaultConfigData, L7_NULLPTR) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  /* Search List generation is done only if domainList is not configured*/
  if(dnsCfgData->domainList[0][0] != 0) 
  {
    dnsDefaultDomainNameSearchListGenerate();  
  }
  else
  {
    dnsDomainNameListSearchListGenerate();
  }

  if (dnsApplyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  dnsCnfgrState = DNS_CLIENT_PHASE_INIT_3;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This function undoes dnsCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dnsCnfgrFiniPhase1Process()
{

  if (dnsCfgData != L7_NULLPTR)
  {
    osapiFree(L7_DNS_CLIENT_COMPONENT_ID, dnsCfgData);
    dnsCfgData = L7_NULLPTR;
  }

  if (dnsOprData != L7_NULLPTR)
  {
    osapiFree(L7_DNS_CLIENT_COMPONENT_ID, dnsOprData);
    dnsOprData = L7_NULLPTR;
  }

  if (dnsCacheData != L7_NULLPTR)
  {
    osapiFree(L7_DNS_CLIENT_COMPONENT_ID, dnsCacheData);
    dnsCacheData = L7_NULLPTR;
  }

  if (dnsNotifyTbl != L7_NULLPTR)
  {
    osapiFree(L7_DNS_CLIENT_COMPONENT_ID, dnsNotifyTbl);
    dnsNotifyTbl = L7_NULLPTR;
  }

  if (dnsCachePoolId != 0)
  {
    bufferPoolTerminate(dnsCachePoolId);
    dnsCachePoolId = 0;
  }

  if (dnsSemaphore != L7_NULLPTR)
  {
    osapiSemaDelete(dnsSemaphore);
    dnsSemaphore = L7_NULLPTR;
  }

  dnsInitUndo();

  dnsCnfgrState = DNS_CLIENT_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes dnsCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dnsCnfgrFiniPhase2Process()
{
  (void)nvStoreDeregister(L7_DNS_CLIENT_COMPONENT_ID);

  dnsCnfgrState = DNS_CLIENT_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes dnsCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dnsCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place dnsCnfgrState to WMU */
  dnsCnfgrUconfigPhase2(&response, &reason);
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
* @end
*********************************************************************/
L7_RC_t dnsCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason)
{
  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return L7_SUCCESS;
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
* @end
*********************************************************************/
L7_RC_t dnsCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* disable the client */
  dnsClientAdminModeApply(L7_FALSE);

  memset(dnsNotifyTbl, 0, L7_LAST_COMPONENT_ID * sizeof(dnsNotifyEntry_t));
  memset(dnsCfgData, 0, sizeof(dnsCfgData));

  dnsCnfgrState = DNS_CLIENT_PHASE_WMU;

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To parse the configurator commands send to dnsTask
*
* @param    none
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void dnsCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             dnsRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if (pCmdData != L7_NULL)
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if (request > L7_CNFGR_RQST_FIRST && 
          request < L7_CNFGR_RQST_LAST)
      {
        /* validate command/event pair */
        switch (command)
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch (request)
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((dnsRC = dnsCnfgrInitPhase1Process(&response, &reason)) != L7_SUCCESS)
                {
                  dnsCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((dnsRC = dnsCnfgrInitPhase2Process(&response, &reason)) != L7_SUCCESS)
                {
                  dnsCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((dnsRC = dnsCnfgrInitPhase3Process(&response, &reason)) != L7_SUCCESS)
                {
                  dnsCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                dnsRC = dnsCnfgrNoopProccess(&response, &reason);
                dnsCnfgrState = DNS_CLIENT_PHASE_WMU;
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            switch (request)
            {
              case L7_CNFGR_RQST_E_START:
                dnsCnfgrState = DNS_CLIENT_PHASE_EXECUTE;

                dnsRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = 0;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch (request)
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                dnsRC = dnsCnfgrNoopProccess(&response, &reason);
                dnsCnfgrState = DNS_CLIENT_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                dnsRC = dnsCnfgrUconfigPhase2(&response, &reason);
                dnsCnfgrState = DNS_CLIENT_PHASE_UNCONFIG_2;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            dnsRC = dnsCnfgrNoopProccess(&response, &reason);
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
  cbData.asyncResponse.rc = dnsRC;
  if (dnsRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}

