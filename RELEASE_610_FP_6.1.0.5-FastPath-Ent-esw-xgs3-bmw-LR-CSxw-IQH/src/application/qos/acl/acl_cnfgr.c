
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename acl_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component
*
* @comments
*
* @create 06/09/2003
*
* @author markl
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "acl.h"
#include "acl_api.h"
#include "tlv_api.h"
#include "nvstoreapi.h"
#include "dtlapi.h"
#include "dtl_acl.h"
#include "clustering_api.h"
#include "acl_cluster.h"

aclCnfgrState_t aclCnfgrState = ACL_PHASE_INIT_0;
L7_int32        aclLogTaskId_g = L7_ERROR;      /* this is what osapi returns */

extern L7_BOOL           aclCompInitialized;
extern aclCfgFileData_t  *aclCfgFileData;
extern L7_uint32         *aclMapTbl;
extern aclIntfInfo_t     *pAclIntfInfo;
extern aclDeregister_t   aclDeregister;
extern L7_tlvHandle_t    aclTlvHandle;
extern L7_uint32         acl_curr_entries_g;    /* total current list entries */
extern avlTree_t         *pAclTree;
extern avlTree_t         *pAclMacTree;
extern aclVlanInfo_t     *pAclVlanInfo;
extern L7_uint32         *aclVlanMapTbl;

/* ACL data can be accessed by multiple threads.
 * Management threads may read or write this data. Routing protocol
 * threads may read the config data when using an ACL as a route
 * filter. For this reason, we protect ACL data with
 * a read/write lock. The advantage of the read/write lock is that
 * multiple routing protocols can apply ACLs as route filters
 * simultaneously. The data protected includes
 *
 *   - all data in the AVL trees
 *   - routeFilterCallbacks[]
 *   - notifyEventCallbacks[]
 *   - deleteApprovalCallbacks[]
 *
 * The lock does not protect the configuration data structures (aclCfgFileData),
 * since that data is only accessed by a management thread (and because there
 * is some interaction among the functions in this file making it non-trivial
 * to protect the configuration data).
 *
 * The lock is created during system initialization
 * and never deleted.
 *
 * The semaphore associated with the AVL tree predates the read/write lock
 * and is only used during creation and deletion of an ACL. It may no longer
 * be necessary to take this semaphore at all.
 */
osapiRWLock_t     aclRwLock;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for Policy component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the acl comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void aclApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             aclRC = L7_ERROR;
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
                if ((aclRC = aclCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  aclCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((aclRC = aclCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  aclCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((aclRC = aclCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  aclCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                aclRC = aclCnfgrNoopProccess( &response, &reason );
                aclCnfgrState = ACL_PHASE_WMU;
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
                aclCnfgrState = ACL_PHASE_EXECUTE;

                aclRC  = L7_SUCCESS;
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
                aclRC = aclCnfgrNoopProccess( &response, &reason );
                aclVlanConfigRestore();
                aclCnfgrState = ACL_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                aclRC = aclCnfgrUconfigPhase2( &response, &reason );
                aclCnfgrState = ACL_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            aclRC = aclCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = aclRC;
  if (aclRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

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
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t aclCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t aclRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  aclRC       = L7_SUCCESS;


  if (osapiRWLockCreate(&aclRwLock, OSAPI_RWLOCK_Q_PRIORITY) != L7_SUCCESS)
  {
      aclRC = L7_FAILURE;
  }


  /* Allocate memory for global data */
  aclCfgFileData  = osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, (L7_uint32)sizeof(aclCfgFileData_t));
  aclMapTbl       = osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, (L7_uint32)sizeof(L7_uint32) * platIntfMaxCountGet());
  pAclIntfInfo    = osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, (L7_uint32)sizeof(aclIntfInfo_t) * platIntfMaxCountGet());
  pAclVlanInfo    = osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, (L7_uint32)sizeof(aclVlanInfo_t) * (L7_ACL_VLAN_MAX_COUNT + 1));
  aclVlanMapTbl   = osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, (L7_uint32)sizeof(L7_uint32) * (L7_ACL_MAX_VLAN_ID + 1));

  /* Make sure that allocation succeded */
  if ((aclMapTbl == L7_NULLPTR) ||
      (aclCfgFileData == L7_NULLPTR) ||
      (L7_NULLPTR == pAclVlanInfo ) ||
      (L7_NULLPTR == aclVlanMapTbl ) ||
      (pAclIntfInfo == L7_NULLPTR))
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    aclRC       = L7_ERROR;

    return aclRC;
  }

  memset(aclCfgFileData, 0, sizeof(aclCfgFileData_t));
  memset(aclMapTbl, 0, sizeof(L7_uint32) * platIntfMaxCountGet());
  memset(pAclIntfInfo, 0, sizeof(aclIntfInfo_t) * platIntfMaxCountGet());
  memset(pAclVlanInfo,  0, sizeof(aclVlanInfo_t) * (L7_ACL_VLAN_MAX_COUNT + 1));
  memset(aclVlanMapTbl, 0, sizeof(L7_uint32) * (L7_ACL_MAX_VLAN_ID + 1));

  /* Allocate memory for callback table control struct */
  if (aclCallbackInit() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    aclRC      = L7_ERROR;

    return aclRC;
  }

  if (aclCorrTableCreate(L7_ACL_LOG_RULE_LIMIT) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    aclRC      = L7_ERROR;

    return aclRC;
  }

  aclCreateTree(L7_ACL_MAX_LISTS);
  aclMacCreateTree(L7_ACL_MAX_LISTS);
  acl_curr_entries_g = 0;

  /* create the ACL log task (only if hardware supports ACL rule logging) */
  if (L7_ACL_LOG_RULE_LIMIT > 0)
  {
    aclLogTaskId_g = osapiTaskCreate("aclLogTask", (void *)aclLogTask,
                                     0, 0, L7_DEFAULT_STACK_SIZE,
                                     L7_DEFAULT_TASK_PRIORITY,
                                     L7_DEFAULT_TASK_SLICE);

    if ((aclLogTaskId_g == L7_ERROR) ||
        (osapiWaitForTaskInit (L7_QOS_ACL_LOG_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS))
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_FATAL;
      aclRC      = L7_ERROR;

      return aclRC;
    }
  }

  if (clusterSupportGet() == L7_SUCCESS)
  {
    if (aclClusterPhase1Init() != L7_SUCCESS)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_FATAL;
      aclRC      = L7_ERROR;

      return aclRC;
    }
  }

  aclCnfgrState = ACL_PHASE_INIT_1;

  return aclRC;
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
L7_RC_t aclCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t aclRC;
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  aclRC       = L7_SUCCESS;

  memset(&notifyFunctionList, 0x00, sizeof(nvStoreFunctionList_t));

  notifyFunctionList.registrar_ID   = L7_FLEX_QOS_ACL_COMPONENT_ID;
  notifyFunctionList.notifySave     = aclSave;
  notifyFunctionList.hasDataChanged = aclHasDataChanged;
  notifyFunctionList.notifyConfigDump = aclConfigDataShow;
  notifyFunctionList.resetDataChanged = aclResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    aclRC     = L7_ERROR;

    return aclRC;
  }
  aclDeregister.aclSave = L7_FALSE;
  aclDeregister.aclRestore = L7_FALSE;
  aclDeregister.aclHasDataChanged = L7_FALSE;

  /* register NIM callback to support interface changes */
  if (nimRegisterIntfChange(L7_FLEX_QOS_ACL_COMPONENT_ID, aclIntfChangeCallback) != L7_SUCCESS)
  {
    LOG_MSG("Failed NIM registration for Interface Change callback\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    aclRC      = L7_ERROR;

    return aclRC;
  }

  if (tlvRegister(L7_QOS_ACL_TLV_MAX_SIZE, L7_FLEX_QOS_ACL_COMPONENT_ID,
                  (L7_uchar8*)"ACL", &aclTlvHandle) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    aclRC      = L7_ERROR;

    return aclRC;
  }

  aclDeregister.aclIntfChangeCallback = L7_FALSE;

  if (clusterSupportGet() == L7_SUCCESS)
  {
    if (aclClusterPhase2Init() != L7_SUCCESS)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_FATAL;
      aclRC      = L7_ERROR;

      return aclRC;
    }
  }

  aclCnfgrState = ACL_PHASE_INIT_2;

  return aclRC;
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
L7_RC_t aclCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t aclRC;


  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  aclRC   = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_FLEX_QOS_ACL_COMPONENT_ID, ACL_CFG_FILENAME,
                   (L7_char8 *)aclCfgFileData, (L7_uint32)sizeof(aclCfgFileData_t),
                   &aclCfgFileData->checkSum, ACL_CFG_VER_CURRENT,
                   aclBuildDefaultConfigData, aclMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    aclRC      = L7_ERROR;

    return aclRC;
  }

  /* ACL COMPONENT IS NOW INITIALIZED */
  aclCompInitialized = L7_TRUE;

  if (aclApplyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    aclRC       = L7_ERROR;

    return aclRC;
  }
#ifdef L7_PRODUCT_SMARTPATH
  aclConfigSyncWithEasyACL();
#endif
  
/* no status stuff for now
  if (aclStatsCreate() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    aclRC       = L7_ERROR;

    return aclRC;
  }
*/
  aclCfgFileData->cfgHdr.dataChanged = L7_FALSE;

  aclCnfgrState = ACL_PHASE_INIT_3;

  return aclRC;
}


/*********************************************************************
* @purpose  This function undoes aclCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void aclCnfgrFiniPhase1Process()
{
  if (aclLogTaskId_g != L7_ERROR)
    osapiTaskDelete(aclLogTaskId_g);

  aclCallbackFini();

  aclCorrTableDelete();

  aclDeleteTree();
  (void)osapiSemaDelete(pAclTree->semId);

  aclMacDeleteTree();
  (void)osapiSemaDelete(pAclMacTree->semId);

  acl_curr_entries_g = 0;

  if (aclCfgFileData != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, aclCfgFileData);
    aclCfgFileData = L7_NULLPTR;
  }

  if (aclMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, aclMapTbl);
    aclMapTbl = L7_NULLPTR;
  }

  if (pAclIntfInfo != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pAclIntfInfo);
    pAclIntfInfo = L7_NULLPTR;
  }

  if (pAclVlanInfo != L7_NULLPTR)
  {
    osapiFree( L7_FLEX_QOS_ACL_COMPONENT_ID, pAclVlanInfo );
    pAclVlanInfo = L7_NULLPTR;
  }

  if (aclVlanMapTbl != L7_NULLPTR)
  {
    osapiFree( L7_FLEX_QOS_ACL_COMPONENT_ID, aclVlanMapTbl );
    aclVlanMapTbl = L7_NULLPTR;
  }
}


/*********************************************************************
* @purpose  This function undoes aclCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void aclCnfgrFiniPhase2Process()
{
  /*
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now
   * a temporary solution is set in place, where in the registered
   * function if we can execute the callback only if it's corresponding
   * member in the aclDeregister is set to L7_FALSE;
   */

  aclDeregister.aclSave = L7_TRUE;
  aclDeregister.aclRestore = L7_TRUE;
  aclDeregister.aclHasDataChanged = L7_TRUE;
  aclDeregister.aclIntfChangeCallback = L7_TRUE;

  (void)tlvUnregister(aclTlvHandle);
  aclTlvHandle = L7_NULL;

  aclCnfgrState = ACL_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes aclCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void aclCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place aclCnfgrState to WMU */
  (void)aclCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t aclCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t aclRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(aclRC);
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/

L7_RC_t aclCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t aclRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  aclRC       = L7_SUCCESS;

  /* clear out the applied configuration */
  (void)aclRestore();

  if (aclCfgFileData != L7_NULLPTR)
    memset(aclCfgFileData, 0, sizeof(aclCfgFileData_t));

  aclCnfgrState = ACL_PHASE_WMU;

  return aclRC;
}


