/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_cnfgr.c
*
* @purpose   ISDP configurator file
*
* @component isdp
*
* @comments
*
* @create    8/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/

#include "isdp_include.h"
#include "buff_api.h"

isdpCnfgrState_t   isdpCnfgrState    = ISDP_PHASE_INIT_0;
isdpCfg_t          *isdpCfg          = L7_NULLPTR;
L7_uint32          *isdpMapTbl       = L7_NULLPTR;
isdpIntfInfoData_t *isdpIntfTbl      = L7_NULLPTR;
osapiTimerDescr_t  *isdpBaseTimer    = L7_NULLPTR;
isdpStats_t        *isdpStats        = L7_NULLPTR;
isdpEntryTree_t    *isdpEntryTree    = L7_NULLPTR;

extern void        *isdpProcessQueue;
extern L7_uint32   isdpIpAddrPoolId;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for isdp component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the isdp comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void isdpApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc;
  isdpMsg_t msg;

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
    if (isdpInit(pCmdData) != L7_SUCCESS)
    {
      isdpInitUndo();
    }
  }
  else
  {
    memset(&msg, 0x00, (L7_uint32)sizeof(msg));
    msg.data.CmdData = *pCmdData;
    msg.event = ISDP_CNFGR_EVENT;

    rc = isdpProcessMsgQueue(msg);
  }
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
L7_RC_t isdpCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  isdpCfg         = (isdpCfg_t *)osapiMalloc(L7_ISDP_COMPONENT_ID, sizeof(isdpCfg_t));
  isdpMapTbl      = osapiMalloc(L7_ISDP_COMPONENT_ID, sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);
  isdpIntfTbl     = osapiMalloc(L7_ISDP_COMPONENT_ID, sizeof(isdpIntfInfoData_t) * L7_MAX_INTERFACE_COUNT);
  isdpEntryTree   = (isdpEntryTree_t *) osapiMalloc(L7_ISDP_COMPONENT_ID, sizeof(isdpEntryTree_t));
  isdpStats       = (isdpStats_t*) osapiMalloc(L7_ISDP_COMPONENT_ID, sizeof(isdpStats_t));

  if ((isdpCfg         == L7_NULLPTR) ||
      (isdpMapTbl      == L7_NULLPTR) ||
      (isdpIntfTbl     == L7_NULLPTR) ||
      (isdpEntryTree   == L7_NULLPTR) ||
      (isdpStats       == L7_NULLPTR))
  {
    isdpDebugTrace(ISDP_DBG_FLAG_CnfgrGroup, "%s: LINE %d: lack of resourses\n",
        __FUNCTION__, __LINE__);

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  /* Allocate buffer pool for ip addresses */
  if (bufferPoolInit(L7_ISDP_MAX_IPADDRESSES,
                     sizeof(isdpIpAddressBuffer_t) + (sizeof(isdpIpAddressBuffer_t) % 4),
                     ISDP_IP_ADDR_BUF_DESC,
                     &isdpIpAddrPoolId) != L7_SUCCESS)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_CnfgrGroup, "%s: LINE %d: lack of resourses\n",
        __FUNCTION__, __LINE__);

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  /* memset to initialize all data structures to zero */
  memset(isdpCfg,       0, sizeof(isdpCfg_t));
  memset(isdpMapTbl,    0, sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);
  memset(isdpIntfTbl,   0, sizeof(isdpIntfInfoData_t) * L7_MAX_INTERFACE_COUNT);
  memset(isdpStats,     0, sizeof(isdpStats_t));
  memset(isdpEntryTree, 0, sizeof(isdpEntryTree_t));

  avlCreateAvlTree(&isdpEntryTree->treeData,
                   isdpEntryTree->treeHeap,
                   isdpEntryTree->dataHeap,
                   L7_ISDP_MAX_NEIGHBORS,
                   sizeof(isdpEntry_t),
                   0x10,
                   sizeof(isdpEntryKey_t));

  /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
  isdpDebugCfgRead();
  isdpDebugApplyConfigData();

  isdpCnfgrState = ISDP_PHASE_INIT_1;

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
L7_RC_t isdpCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t               isdpRC = L7_SUCCESS;
  nvStoreFunctionList_t isdpNotifyFunctionList;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  memset((void *) &isdpNotifyFunctionList, 0, sizeof(isdpNotifyFunctionList));

  isdpNotifyFunctionList.registrar_ID         = L7_ISDP_COMPONENT_ID;
  isdpNotifyFunctionList.notifySave           = isdpSave;
  isdpNotifyFunctionList.hasDataChanged       = isdpHasDataChanged;
  isdpNotifyFunctionList.resetDataChanged     = isdpResetDataChanged;
  isdpNotifyFunctionList.notifyConfigDump     = isdpDebugCfgDump;

  if (nvStoreRegister(isdpNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    isdpRC     = L7_ERROR;

    isdpDebugTrace(ISDP_DBG_FLAG_CnfgrGroup, "%s: LINE %d: failed to nvStoreRegister\n",
        __FUNCTION__, __LINE__);

    return isdpRC;
  }

  /* register callback with NIM for L7_UPs and L7_DOWNs */
  if (nimRegisterIntfChange(L7_ISDP_COMPONENT_ID, isdpIntfChangeCallback) != L7_SUCCESS)
  {
    LOG_MSG("Unable to register for NIM Interface change callback!\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    isdpRC     = L7_ERROR;
    return isdpRC;
  }

  /* register for debug*/
  isdpDebugRegister();

  isdpCnfgrState = ISDP_PHASE_INIT_2;

  return isdpRC;
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
L7_RC_t isdpCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t isdpRC = L7_SUCCESS;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  if (sysapiCfgFileGet(L7_ISDP_COMPONENT_ID, ISDP_CFG_FILENAME,
                   (L7_char8 *)isdpCfg, sizeof(isdpCfg_t),
                   &isdpCfg->checkSum, ISDP_CFG_VER_CURRENT,
                   isdpBuildDefaultConfigData, L7_NULL) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    isdpRC     = L7_ERROR;

    LOG_MSG("sysapiCfgFileGet failed\n");
    return isdpRC;
  }

  isdpRC = isdpCtlApplyConfigData();
  if(isdpRC != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    isdpRC     = L7_ERROR;

    LOG_MSG("isdpCtlApplyConfigData failed\n");
    return isdpRC;
  }

  isdpCnfgrState = ISDP_PHASE_INIT_3;

  return isdpRC;
}
/*********************************************************************
* @purpose  This function undoes isdpCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void isdpCnfgrFiniPhase1Process()
{
  if (isdpCfg != L7_NULLPTR)
  {
    osapiFree(L7_ISDP_COMPONENT_ID, (void *)isdpCfg);
    isdpCfg = L7_NULLPTR;
  }

  if (isdpMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_ISDP_COMPONENT_ID, isdpMapTbl);
    isdpMapTbl = L7_NULLPTR;
  }

  if (isdpIntfTbl != L7_NULLPTR)
  {
    osapiFree(L7_ISDP_COMPONENT_ID, isdpIntfTbl);
    isdpIntfTbl = L7_NULLPTR;
  }

  if (isdpEntryTree != L7_NULLPTR)
  {
    isdpAllEntryDelete();
    isdpTreeDelete();
    osapiFree(L7_ISDP_COMPONENT_ID, (void *)isdpEntryTree);
    isdpEntryTree = L7_NULLPTR;
  }

  if (isdpStats != L7_NULLPTR)
  {
    osapiFree(L7_ISDP_COMPONENT_ID, (void *)isdpStats);
    isdpStats = L7_NULLPTR;
  }

  if (isdpIpAddrPoolId != 0)
  {
    bufferPoolTerminate(isdpIpAddrPoolId);
    isdpIpAddrPoolId = 0;
  }

  isdpInitUndo();

  isdpCnfgrState = ISDP_PHASE_INIT_0;

  return;
}

/*********************************************************************
* @purpose  This function undoes isdpCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void isdpCnfgrFiniPhase2Process()
{
  nimDeRegisterIntfChange(L7_ISDP_COMPONENT_ID);
  nvStoreDeregister(L7_ISDP_COMPONENT_ID);

  isdpCnfgrState = ISDP_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes isdpCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void isdpCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;
  isdpCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t isdpCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t isdpRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(isdpRC);
}

/*********************************************************************
*
* @purpose  To parse the configurator commands send to isdpTask
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void isdpCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t               isdpRC     = L7_ERROR;
  L7_CNFGR_ERR_RC_t     reason     = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                if ((isdpRC = isdpCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                 isdpCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((isdpRC = isdpCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  isdpCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((isdpRC = isdpCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  isdpCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                isdpRC = isdpCnfgrNoopProccess( &response, &reason );
                isdpCnfgrState = ISDP_PHASE_WMU;
                break;
              default:
                /* invalid command/request pair */
                isdpDebugTrace(ISDP_DBG_FLAG_CnfgrGroup, "%s: LINE %d: invalid command/request pair\n",
                    __FUNCTION__, __LINE__);
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            switch ( request )
            {
              case L7_CNFGR_RQST_E_START:
                isdpCnfgrState = ISDP_PHASE_EXECUTE;
                isdpRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = 0;
                break;

              default:
                /* invalid command/request pair */
                isdpDebugTrace(ISDP_DBG_FLAG_CnfgrGroup, "%s: LINE %d: invalid command/request pair\n",
                    __FUNCTION__, __LINE__);
                break;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                isdpRC = isdpCnfgrNoopProccess( &response, &reason );
                isdpCnfgrState = ISDP_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                isdpRC = isdpCnfgrUconfigPhase2( &response, &reason );
                isdpCnfgrState = ISDP_PHASE_UNCONFIG_2;
                break;

              default:
                /* invalid command/request pair */
                isdpDebugTrace(ISDP_DBG_FLAG_CnfgrGroup, "%s: LINE %d: invalid command/request pair\n",
                    __FUNCTION__, __LINE__);
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            isdpRC = isdpCnfgrNoopProccess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            isdpDebugTrace(ISDP_DBG_FLAG_CnfgrGroup, "%s: LINE %d: invalid command\n",
                __FUNCTION__, __LINE__);
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
  cbData.asyncResponse.rc = isdpRC;
  if (isdpRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);


  return;
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

L7_RC_t isdpCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  memset(isdpCfg,      0, sizeof(isdpCfg_t));

  isdpTxRxDeinit();

  isdpCnfgrState = ISDP_PHASE_WMU;

  return rc;
}

/*********************************************************************
* @purpose  Check whether isdp is ready.
*
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL isdpCnfgrStateCheck (void)
{
  if ((isdpCnfgrState == ISDP_PHASE_EXECUTE) ||
      (isdpCnfgrState == ISDP_PHASE_INIT_3)  ||
      (isdpCnfgrState == ISDP_PHASE_UNCONFIG_1))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

