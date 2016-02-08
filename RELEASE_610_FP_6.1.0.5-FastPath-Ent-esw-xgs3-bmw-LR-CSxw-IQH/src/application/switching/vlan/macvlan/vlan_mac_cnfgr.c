/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_cnfgr.c
*
* @purpose   Mac Vlan Configurator file
*
* @component vlanMac
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

#include "vlan_mac_include.h"

osapiRWLock_t macVlanCfgRWLock;
avlTree_t *macVlanTreeData;
avlTreeTables_t *macVlanTreeHeap;
vlanMacOperData_t *macVlanDataHeap;
vlanMacCnfgrState_t vlanMacCnfgrState = VLAN_MAC_PHASE_INIT_0;

/* Begin Function Declarations: vlan_mac_cnfgr.h */

/*********************************************************************
*
* @purpose  CNFGR System Initialization for vlanMac component
*
* @param    *pCmdData    {(input)} Data structure for this
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the vlanMac comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void vlanMacApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
    L7_CNFGR_CMD_t command;
    L7_CNFGR_RQST_t request;
    L7_CNFGR_CB_DATA_t cbData;
    L7_CNFGR_RESPONSE_t response = L7_SUCCESS;
    L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
    L7_RC_t vlanMacRC = L7_ERROR;
    L7_CNFGR_ERR_RC_t reason = L7_CNFGR_ERR_RC_INVALID_PAIR;


    /* validate command type */
    if (pCmdData != L7_NULL)
    {
        if (pCmdData->type == L7_CNFGR_RQST)
        {
            command = pCmdData->command;
            request = pCmdData->u.rqstData.request;
            correlator = pCmdData->correlator;
            if (request > L7_CNFGR_RQST_FIRST && request < L7_CNFGR_RQST_LAST)
            {
                /* validate command/event pair */
                switch (command)
                {
                case L7_CNFGR_CMD_INITIALIZE:
                    switch (request)
                    {
                    case L7_CNFGR_RQST_I_PHASE1_START:
                        if ((vlanMacRC =
                             vlanMacCnfgrInitPhase1Process(&response,
                                                           &reason)) != L7_SUCCESS)
                        {
                            vlanMacCnfgrFiniPhase1Process();
                        }
                        break;

                    case L7_CNFGR_RQST_I_PHASE2_START:
                        if ((vlanMacRC =
                             vlanMacCnfgrInitPhase2Process(&response,
                                                           &reason)) != L7_SUCCESS)
                        {
                            vlanMacCnfgrFiniPhase2Process();
                        }
                        break;

                    case L7_CNFGR_RQST_I_PHASE3_START:
                        if ((vlanMacRC =
                             vlanMacCnfgrInitPhase3Process(&response,
                                                           &reason)) != L7_SUCCESS)
                        {
                            vlanMacCnfgrFiniPhase3Process();
                        }
                        break;

                    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                        vlanMacRC =
                        vlanMacCnfgrNoopProcess(&response, &reason);
                        vlanMacCnfgrState = VLAN_MAC_PHASE_WMU;
                        break;

                    default:
                        /* invalid command/request pair */
                        break;
                    }
                    break;

                case L7_CNFGR_CMD_EXECUTE:
                    switch (request)
                    {
                    case L7_CNFGR_RQST_E_START:
                        vlanMacCnfgrState = VLAN_MAC_PHASE_EXECUTE;
                        vlanMacRC = L7_SUCCESS;
                        response = L7_CNFGR_CMD_COMPLETE;
                        reason = 0;
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
                        vlanMacRC =
                        vlanMacCnfgrNoopProcess(&response, &reason);
                        vlanMacCnfgrState = VLAN_MAC_PHASE_UNCONFIG_1;
                        break;

                    case L7_CNFGR_RQST_U_PHASE2_START:
                        vlanMacRC =
                        vlanMacCnfgrUconfigPhase2(&response, &reason);
                        vlanMacCnfgrState = VLAN_MAC_PHASE_UNCONFIG_2;
                        break;

                    default:
                        /* invalid command/request pair */
                        break;
                    }
                    break;

                case L7_CNFGR_CMD_TERMINATE:
                case L7_CNFGR_CMD_SUSPEND:
                    vlanMacRC =
                    vlanMacCnfgrNoopProcess(&response, &reason);
                    break;

                default:
                    reason = L7_CNFGR_ERR_RC_INVALID_CMD;
                    break;
                }
            }
            else
            {
                reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            }
        }
        else
        {
            reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
        }
    }
    else
    {
        correlator = L7_NULL;
        reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    }

    cbData.correlator = correlator;
    cbData.asyncResponse.rc = vlanMacRC;
    if (vlanMacRC == L7_SUCCESS)
        cbData.asyncResponse.u.response = response;
    else
        cbData.asyncResponse.u.reason = reason;

    cnfgrApiCallback(&cbData);
    return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
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
L7_RC_t vlanMacCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t *pReason)
{
    L7_RC_t vlanMacRC;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
    vlanMacRC = L7_SUCCESS;


    /* Allocate and initialized memory for global data */
    vlanMacCfg = (vlanMacCfg_t *) osapiMalloc(L7_VLAN_MAC_COMPONENT_ID, 
                                              (L7_uint32)sizeof(vlanMacCfg_t));
    macVlanTreeData = (avlTree_t *) osapiMalloc(L7_VLAN_MAC_COMPONENT_ID, sizeof(avlTree_t)); 
    macVlanTreeHeap = (avlTreeTables_t *) osapiMalloc(L7_VLAN_MAC_COMPONENT_ID, 
                                          sizeof(avlTreeTables_t)*L7_VLAN_MAC_MAX_VLANS);
    macVlanDataHeap = (vlanMacOperData_t *) osapiMalloc(L7_VLAN_MAC_COMPONENT_ID, 
                                          sizeof(vlanMacOperData_t)*L7_VLAN_MAC_MAX_VLANS);

    if (vlanMacCfg == L7_NULLPTR || macVlanTreeData == L7_NULLPTR ||
        macVlanTreeHeap == L7_NULLPTR || macVlanDataHeap == L7_NULLPTR)
    {
        if (vlanMacCfg)
            (void) osapiFree(L7_VLAN_MAC_COMPONENT_ID, vlanMacCfg);
        if (macVlanTreeHeap)
            (void) osapiFree(L7_VLAN_MAC_COMPONENT_ID, macVlanTreeHeap);
        if (macVlanDataHeap)
            (void) osapiFree(L7_VLAN_MAC_COMPONENT_ID, macVlanDataHeap);
        if (macVlanTreeData)
            (void) osapiFree(L7_VLAN_MAC_COMPONENT_ID, macVlanTreeData);

        *pResponse = 0;
        *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
        vlanMacRC = L7_ERROR;
        return vlanMacRC;
    }

    /* memset all memory to zero */
    memset((void *) vlanMacCfg, 0x00, (L7_uint32) sizeof(vlanMacCfg_t));
    memset((void *) macVlanTreeData, 0x00, (L7_uint32) sizeof(avlTree_t));
    memset((void *) macVlanTreeHeap, 0x00, sizeof(avlTreeTables_t)*L7_VLAN_MAC_MAX_VLANS);
    memset((void *) macVlanDataHeap, 0x00, sizeof(vlanMacOperData_t)*L7_VLAN_MAC_MAX_VLANS);

    if (osapiRWLockCreate(&macVlanCfgRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
    {
		L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_VLAN_MAC_COMPONENT_ID, "vlanMacCnfgrInitPhase1Process: Unable to "
					   "create r/w lock for vlanMac. This appears when a read/write lock creations fails.\n");
        *pResponse = 0;
        *pReason = L7_CNFGR_ERR_RC_FATAL;
        vlanMacRC = L7_ERROR;
        return vlanMacRC;
    }

    avlCreateAvlTree(macVlanTreeData, macVlanTreeHeap, macVlanDataHeap, L7_VLAN_MAC_MAX_VLANS, 
                     (L7_uint32)sizeof(vlanMacOperData_t), 0x10, sizeof(L7_enetMacAddr_t));

    vlanMacCnfgrState = VLAN_MAC_PHASE_INIT_1;
    return vlanMacRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
* 
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
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
L7_RC_t vlanMacCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t * pResponse,
                                      L7_CNFGR_ERR_RC_t * pReason)
{
    L7_RC_t vlanMacRC;
    nvStoreFunctionList_t notifyFunctionList;
    L7_uint32 eventMask = 0;


    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
    vlanMacRC = L7_SUCCESS;

    memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
    notifyFunctionList.registrar_ID = L7_VLAN_MAC_COMPONENT_ID;
    notifyFunctionList.notifySave = vlanMacSave;
    notifyFunctionList.hasDataChanged = vlanMacHasDataChanged;
    notifyFunctionList.notifyConfigDump = vlanMacCfgDump;
    notifyFunctionList.notifyDebugDump =  vlanMacInfoDump;
    notifyFunctionList.resetDataChanged = vlanMacResetDataChanged;

    if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason = L7_CNFGR_ERR_RC_FATAL;
        vlanMacRC = L7_ERROR;
        return vlanMacRC;
    }
    eventMask = VLAN_ADD_NOTIFY | VLAN_DELETE_NOTIFY;
    /* register callback with DOT1Q for VLAN changes */
    if (vlanRegisterForChange(vlanMacVlanChangeCallback, L7_VLAN_MAC_COMPONENT_ID, eventMask) != L7_SUCCESS)
    {
		L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_VLAN_MAC_COMPONENT_ID, "Unable to register for VLAN change callback! "
						  "This appears when this component unable to register for vlan change notifications \n");
        *pResponse = 0;
        *pReason = L7_CNFGR_ERR_RC_FATAL;
        vlanMacRC = L7_ERROR;

        return vlanMacRC;
    }

    vlanMacCnfgrState = VLAN_MAC_PHASE_INIT_2;
    return vlanMacRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
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
L7_RC_t vlanMacCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t *pReason)
{
    L7_RC_t vlanMacRC;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
    vlanMacRC = L7_SUCCESS;

    if (sysapiCfgFileGet(L7_VLAN_MAC_COMPONENT_ID, VLAN_MAC_CFG_FILENAME, (L7_char8 *) vlanMacCfg, 
                         sizeof(vlanMacCfg_t), &vlanMacCfg->checkSum, VLAN_MAC_CFG_VER_CURRENT, 
                         vlanMacBuildDefaultConfigData, vlanMacMigrateConfigData) != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason = L7_CNFGR_ERR_RC_FATAL;
        vlanMacRC = L7_ERROR;
        return vlanMacRC;
    }

    if (vlanMacApplyConfigData() != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason = L7_CNFGR_ERR_RC_FATAL;
        vlanMacRC = L7_ERROR;
        return vlanMacRC;
    }

    vlanMacCfg->cfgHdr.dataChanged = L7_FALSE;
    vlanMacCnfgrState = VLAN_MAC_PHASE_INIT_3;
    return vlanMacRC;
}

/*********************************************************************
* @purpose  This function undoes vlanMacCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void  vlanMacCnfgrFiniPhase1Process()
{
    /* Deallocate anything that was allocated */
    if (vlanMacCfg != L7_NULLPTR)
    {
        memset((void *) vlanMacCfg, 0x00, (L7_uint32) sizeof(vlanMacCfg_t));
        osapiFree(L7_VLAN_MAC_COMPONENT_ID, vlanMacCfg);
        vlanMacCfg = L7_NULLPTR;
    }
    if (macVlanTreeData != L7_NULLPTR)
    {
        avlPurgeAvlTree(macVlanTreeData, L7_VLAN_MAC_MAX_VLANS);
        if (macVlanTreeData->semId != L7_NULLPTR &&
            osapiSemaDelete((void *) macVlanTreeData->semId) != L7_SUCCESS)
        {
			L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_VLAN_MAC_COMPONENT_ID, "vlanMacCnfgrFiniPhase1Process: could not "
					   "delete avl semaphore. This appears when a semaphore deletion of this component fails.\n");
        }
        memset((void *) macVlanTreeData, 0x00, (L7_uint32) sizeof(avlTree_t));
        osapiFree(L7_VLAN_MAC_COMPONENT_ID, macVlanTreeData);
        macVlanTreeData = L7_NULLPTR;
    }
    if (macVlanTreeHeap != L7_NULLPTR)
    {
        memset((void *) macVlanTreeHeap, 0x00, sizeof(avlTreeTables_t) * L7_VLAN_MAC_MAX_VLANS);
        osapiFree(L7_VLAN_MAC_COMPONENT_ID, macVlanTreeHeap);
        macVlanTreeHeap = L7_NULLPTR;
    }
    if (macVlanDataHeap != L7_NULLPTR)
    {
        memset((void *) macVlanDataHeap, 0x00, sizeof(vlanMacOperData_t) * L7_VLAN_MAC_MAX_VLANS);
        osapiFree(L7_VLAN_MAC_COMPONENT_ID, macVlanDataHeap);
        macVlanDataHeap = L7_NULLPTR;
    }

    vlanMacCnfgrState = VLAN_MAC_PHASE_INIT_0;
}


/*********************************************************************
* @purpose  This function undoes vlanMacCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void  vlanMacCnfgrFiniPhase2Process()
{
    vlanDeregisterForChange(L7_VLAN_MAC_COMPONENT_ID);
    nvStoreDeregister(L7_VLAN_MAC_COMPONENT_ID);
    vlanMacCnfgrState = VLAN_MAC_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes vlanMacCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void  vlanMacCnfgrFiniPhase3Process()
{
    L7_CNFGR_RESPONSE_t response;
    L7_CNFGR_ERR_RC_t reason;

    /* this func will place vlanMacCnfgrState to WMU */
    vlanMacCnfgrUconfigPhase2(&response, &reason);
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse  @b{(output)}  Response always command complete.
*     
* @param    pReason    @b{(output)}  Always 0                    
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
L7_RC_t vlanMacCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t *pReason)
{
    L7_RC_t vlanMacRC = L7_SUCCESS;

    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
    return(vlanMacRC);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
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
L7_RC_t vlanMacCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t *pReason)
{
    L7_RC_t vlanMacRC;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
    vlanMacRC = L7_SUCCESS;

    /* Restore any configuration */
    vlanMacRestore();
    /* Clear out the configuration */
    memset(vlanMacCfg, 0, sizeof(vlanMacCfg_t));
    vlanMacCnfgrState = VLAN_MAC_PHASE_WMU;

    return vlanMacRC;
}

/* End Function Declarations */
