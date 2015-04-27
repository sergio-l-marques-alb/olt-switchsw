/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_cnfgr.c
*
* @purpose   IP Subnet Vlan configurator
*
* @component vlanIpSubnet
*
* @comments
*
* @create    5/5/2005
*
* @author    tsrikanth
*
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#ifdef L7_CLI_PACKAGE
#include "cli_txt_cfg_api.h"
#endif

#include "vlan_ipsubnet_include.h"

extern osapiRWLock_t vlanIpSubnetCfgRWLock;
avlTree_t *vlanIpSubnetTreeData;
avlTreeTables_t *vlanIpSubnetTreeHeap;
vlanIpSubnetOperData_t *vlanIpSubnetDataHeap;
vlanIpSubnetCnfgrState_t vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_INIT_0;

/* Begin Function Declarations: vlan_ipsubnet_cnfgr.h */
/*********************************************************************
*
* @purpose  CNFGR System Initialization for vlanIpSubnet component
*
* @param    *pCmdData    @b{(input)} Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the vlanIpSubnet comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void vlanIpSubnetApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
    /* set up variables and structures */
    L7_CNFGR_CMD_t        command;
    L7_CNFGR_RQST_t       request;
    L7_CNFGR_CB_DATA_t    cbData;
    L7_CNFGR_RESPONSE_t   response;
    L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

    L7_RC_t               vlanIpSubnetRC = L7_ERROR;
    L7_CNFGR_ERR_RC_t     reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                        if ((vlanIpSubnetRC = vlanIpSubnetCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                        {
                            vlanIpSubnetCnfgrFiniPhase1Process();
                        }
                        break;
                    case L7_CNFGR_RQST_I_PHASE2_START:
                        if ((vlanIpSubnetRC = vlanIpSubnetCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                        {
                            vlanIpSubnetCnfgrFiniPhase2Process();
                        }
                        break;
                    case L7_CNFGR_RQST_I_PHASE3_START:
                        if ((vlanIpSubnetRC = vlanIpSubnetCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                        {
                            vlanIpSubnetCnfgrFiniPhase3Process();
                        }
                        break;
                    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                        vlanIpSubnetRC = vlanIpSubnetCnfgrNoopProcess( &response, &reason );
                        vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_WMU;
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
                        vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_EXECUTE;

                        vlanIpSubnetRC  = L7_SUCCESS;
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
                        vlanIpSubnetRC = vlanIpSubnetCnfgrNoopProcess( &response, &reason );
                        vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_UNCONFIG_1;
                        break;

                    case L7_CNFGR_RQST_U_PHASE2_START:
                        vlanIpSubnetRC = vlanIpSubnetCnfgrUconfigPhase2( &response, &reason );
                        vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_UNCONFIG_2;
                        break;

                    default:
                        /* invalid command/request pair */
                        break;
                    }
                    break;

                case L7_CNFGR_CMD_TERMINATE:
                case L7_CNFGR_CMD_SUSPEND:
                    vlanIpSubnetRC = vlanIpSubnetCnfgrNoopProcess( &response, &reason );
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
     * <prepare complesion response>
     * <callback the configurator>
     */
    cbData.correlator       = correlator;
    cbData.asyncResponse.rc = vlanIpSubnetRC;
    if (vlanIpSubnetRC == L7_SUCCESS)
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
L7_RC_t vlanIpSubnetCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t vlanIpSubnetRC;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    vlanIpSubnetRC = L7_SUCCESS;

    /* Allocate and initialized memory for global data */

    vlanIpSubnetCfg = (vlanIpSubnetCfg_t *) osapiMalloc(L7_VLAN_IPSUBNET_COMPONENT_ID,
                                                        (L7_uint32)sizeof(vlanIpSubnetCfg_t));
    /* START OF AVL HOOKS THAT NEED TO BE CONVERTED FOR VLANS */
    vlanIpSubnetTreeData = (avlTree_t *) osapiMalloc(L7_VLAN_IPSUBNET_COMPONENT_ID,
                                                     sizeof(avlTree_t));

    vlanIpSubnetTreeHeap = (avlTreeTables_t *) osapiMalloc(L7_VLAN_IPSUBNET_COMPONENT_ID,
                                    sizeof(avlTreeTables_t) * L7_VLAN_IPSUBNET_MAX_VLANS);

    vlanIpSubnetDataHeap = (vlanIpSubnetOperData_t *) osapiMalloc(L7_VLAN_IPSUBNET_COMPONENT_ID,
                             sizeof(vlanIpSubnetOperData_t) * L7_VLAN_IPSUBNET_MAX_VLANS);

    if (vlanIpSubnetCfg == L7_NULLPTR ||
        vlanIpSubnetTreeData == L7_NULLPTR ||
        vlanIpSubnetTreeHeap == L7_NULLPTR ||
        vlanIpSubnetDataHeap == L7_NULLPTR)
    {
        if (vlanIpSubnetCfg)
            (void) osapiFree(L7_VLAN_IPSUBNET_COMPONENT_ID, vlanIpSubnetCfg);
        if (vlanIpSubnetTreeHeap)
            (void) osapiFree(L7_VLAN_IPSUBNET_COMPONENT_ID, vlanIpSubnetTreeHeap);
        if (vlanIpSubnetDataHeap)
            (void) osapiFree(L7_VLAN_IPSUBNET_COMPONENT_ID, vlanIpSubnetDataHeap);

        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
        vlanIpSubnetRC = L7_ERROR;

        return vlanIpSubnetRC;
    }
    /* END OF AVL HOOKS THAT NEED TO BE CONVERTED FOR VLANS */

    /* memset all memory to zero */
    memset((void *) vlanIpSubnetCfg, 0x00,(L7_uint32) sizeof(vlanIpSubnetCfg_t));
    memset((void *) vlanIpSubnetTreeData, 0x00, (L7_uint32) sizeof(avlTree_t));
    memset((void *) vlanIpSubnetTreeHeap, 0x00, sizeof(avlTreeTables_t) * L7_VLAN_IPSUBNET_MAX_VLANS);
    memset((void *) vlanIpSubnetDataHeap, 0x00, sizeof(vlanIpSubnetOperData_t) * L7_VLAN_IPSUBNET_MAX_VLANS);

    if (osapiRWLockCreate(&vlanIpSubnetCfgRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_IPSUBNET_COMPONENT_ID, "vlanIpSubnetCnfgrInitPhase1Process: Unable "
                      "to create r/w lock for vlanIpSubnet. This appears when a read/write lock creations fails.\n");

        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        vlanIpSubnetRC = L7_ERROR;

        return vlanIpSubnetRC;
    }


    avlCreateAvlTree(vlanIpSubnetTreeData, vlanIpSubnetTreeHeap,
                     vlanIpSubnetDataHeap, L7_VLAN_IPSUBNET_MAX_VLANS,
                     (L7_uint32) sizeof(vlanIpSubnetOperData_t), 0x10, VLAN_IPSUBNET_KEYSIZE);


    vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_INIT_1;
    return vlanIpSubnetRC;

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
L7_RC_t vlanIpSubnetCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t vlanIpSubnetRC;
    nvStoreFunctionList_t notifyFunctionList;
    L7_uint32 eventMask = 0;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    vlanIpSubnetRC = L7_SUCCESS;

    memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
    notifyFunctionList.registrar_ID   = L7_VLAN_IPSUBNET_COMPONENT_ID;
    notifyFunctionList.notifySave     = vlanIpSubnetSave;
    notifyFunctionList.hasDataChanged = vlanIpSubnetHasDataChanged;
    notifyFunctionList.notifyConfigDump = vlanIpSubnetCfgDump;
    notifyFunctionList.notifyDebugDump = vlanIpSubnetInfoDump;
    notifyFunctionList.resetDataChanged = vlanIpSubnetResetDataChanged;

    if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        vlanIpSubnetRC = L7_ERROR;

        return vlanIpSubnetRC;
    }
    eventMask = VLAN_ADD_NOTIFY | VLAN_DELETE_NOTIFY;
    /* register callback with DOT1Q for VLAN changes */
    if (vlanRegisterForChange(vlanIpSubnetVlanChangeCallback, L7_VLAN_IPSUBNET_COMPONENT_ID, eventMask) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_IPSUBNET_COMPONENT_ID, "vlanIpSubnetCnfgrInitPhase2Process: "
           "Unable to register for VLAN change callback! This appears when this component unable to register"
           " for vlan change notifications.\n");
        *pResponse  = 0;
        *pReason    = L7_CNFGR_ERR_RC_FATAL;
        vlanIpSubnetRC     = L7_ERROR;

        return vlanIpSubnetRC;
    }

#ifdef L7_CLI_PACKAGE
  /* Register with cli text config componet to get notifications after
    apply config completion */
  if (txtCfgApplyCompletionNotifyRegister(L7_VLAN_IPSUBNET_COMPONENT_ID,
                                          vlanIpTxtCfgApplyCompletionCallback))
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_VLAN_IPSUBNET_COMPONENT_ID,
           "IP Subnet VLAN failed to register for apply config completions.");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    vlanIpSubnetRC  = L7_ERROR;
    return vlanIpSubnetRC;
  }
#else
  /* signal configurator that this component is ready for driver sync. */
  cnfgrApiComponentHwUpdateDone(L7_VLAN_IPSUBNET_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);
#endif

    vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_INIT_2;

    return vlanIpSubnetRC;
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
L7_RC_t vlanIpSubnetCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t vlanIpSubnetRC;


    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    vlanIpSubnetRC = L7_SUCCESS;

    if (sysapiCfgFileGet(L7_VLAN_IPSUBNET_COMPONENT_ID, VLAN_IPSUBNET_CFG_FILENAME,
                         (L7_char8 *)vlanIpSubnetCfg, sizeof(vlanIpSubnetCfg_t),
                         &vlanIpSubnetCfg->checkSum, VLAN_IPSUBNET_CFG_VER_CURRENT,
                         vlanIpSubnetBuildDefaultConfigData, vlanIpSubnetMigrateConfigData) != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        vlanIpSubnetRC = L7_ERROR;

        return vlanIpSubnetRC;
    }

    if (vlanIpSubnetApplyConfigData() != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        vlanIpSubnetRC = L7_ERROR;

        return vlanIpSubnetRC;
    }

    vlanIpSubnetCfg->cfgHdr.dataChanged = L7_FALSE;

    vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_INIT_3;

    return vlanIpSubnetRC;
}


/*********************************************************************
* @purpose  This function undoes vlanIpSubnetCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void vlanIpSubnetCnfgrFiniPhase1Process()
{
    /* Deallocate anything that was allocated */
    if (vlanIpSubnetCfg  != L7_NULLPTR)
    {
        osapiFree(L7_VLAN_IPSUBNET_COMPONENT_ID, vlanIpSubnetCfg);
        vlanIpSubnetCfg = L7_NULLPTR;
    }

    if (vlanIpSubnetTreeData != L7_NULLPTR)
    {
        avlPurgeAvlTree(vlanIpSubnetTreeData, L7_VLAN_IPSUBNET_MAX_VLANS);
        if (vlanIpSubnetTreeData->semId != L7_NULLPTR &&
            osapiSemaDelete((void *) vlanIpSubnetTreeData->semId) != L7_SUCCESS)
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_IPSUBNET_COMPONENT_ID, "vlanIpSubnetCnfgrFiniPhase1Process: "
             "could not delete avl semaphore. This appears when a semaphore deletion of this component fails.\n");
        memset((void *) vlanIpSubnetTreeData, 0x00, (L7_uint32) sizeof(avlTree_t));
        osapiFree(L7_VLAN_IPSUBNET_COMPONENT_ID, vlanIpSubnetTreeData);
        vlanIpSubnetTreeData = L7_NULLPTR;
    }

    if (vlanIpSubnetTreeHeap != L7_NULLPTR)
    {
        memset((void *) vlanIpSubnetTreeHeap, 0x00,
               sizeof(avlTreeTables_t) * L7_VLAN_IPSUBNET_MAX_VLANS);
        osapiFree(L7_VLAN_IPSUBNET_COMPONENT_ID, vlanIpSubnetTreeHeap);
        vlanIpSubnetTreeHeap = L7_NULLPTR;
    }

    if (vlanIpSubnetDataHeap != L7_NULLPTR)
    {
        memset((void *) vlanIpSubnetDataHeap, 0x00,
               sizeof(vlanIpSubnetOperData_t) * L7_VLAN_IPSUBNET_MAX_VLANS);
        osapiFree(L7_VLAN_IPSUBNET_COMPONENT_ID, vlanIpSubnetDataHeap);
        vlanIpSubnetDataHeap = L7_NULLPTR;
    }

    /* END OF AVL HOOKS THAT NEED TO BE CONVERTED FOR VLANS */
    vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_INIT_0;
}


/*********************************************************************
* @purpose  This function undoes vlanIpSubnetCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void vlanIpSubnetCnfgrFiniPhase2Process()
{
    vlanDeregisterForChange(L7_VLAN_IPSUBNET_COMPONENT_ID);
    nvStoreDeregister(L7_VLAN_IPSUBNET_COMPONENT_ID);


    vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes vlanIpSubnetCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void vlanIpSubnetCnfgrFiniPhase3Process()
{
    L7_CNFGR_RESPONSE_t response;
    L7_CNFGR_ERR_RC_t   reason;

    /* this func will place vlanIpSubnetCnfgrState to WMU */
    vlanIpSubnetCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t vlanIpSubnetCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t vlanIpSubnetRC = L7_SUCCESS;


    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    return(vlanIpSubnetRC);
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
L7_RC_t vlanIpSubnetCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t vlanIpSubnetRC;

    *pResponse  = L7_CNFGR_CMD_COMPLETE;
    *pReason    = 0;
    vlanIpSubnetRC  = L7_SUCCESS;

    /* Remove any configuration */
    vlanIpSubnetRestore();

    /* Clear out the configuration */
    memset(vlanIpSubnetCfg, 0, sizeof(vlanIpSubnetCfg_t));

    vlanIpSubnetCnfgrState = VLAN_IPSUBNET_PHASE_WMU;

    return vlanIpSubnetRC;
}

/* End Function Declarations */
