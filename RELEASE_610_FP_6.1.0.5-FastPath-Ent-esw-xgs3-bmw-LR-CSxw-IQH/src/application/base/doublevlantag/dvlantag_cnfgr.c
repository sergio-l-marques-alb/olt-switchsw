/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvlantag_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component 
*
* @comments 
*
* @create 10/17/2003
*
* @author mfiorito
*
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "dvlantag_include.h"

dvlantagCnfgrState_t dvlantagCnfgrState = DVLANTAG_PHASE_INIT_0;

extern L7_RC_t dvlantagDefaultTpidApply(L7_uint32 etherType);

extern DVLANTAG_CFG_t *dvlantagCfg;
extern L7_uint32 *dvlantagMapTbl;
osapiRWLock_t dvlantagCfgRWLock;
/* Contents of dvlantagGlobalEthertype is to be used only when
 * ethertype is settable and settable on the entire device.
 * If the ethertype is settable on a per interface basis this variable
 * should not be used. This variable is for the sole purpose of
 * accomodating a newly created interface procure the current ethertype 
 * setting on the device.
 */
L7_uint32 dvlantagDeviceEthertype = L7_DVLANTAG_DEFAULT_ETHERTYPE; 
L7_BOOL dvlantagDeviceEthertypePresent = L7_FALSE;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for dvlantag component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the dvlantag comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void dvlantagApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             dvlantagRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason     = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                if ((dvlantagRC = dvlantagCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  dvlantagCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((dvlantagRC = dvlantagCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  dvlantagCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((dvlantagRC = dvlantagCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  dvlantagCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                dvlantagRC = dvlantagCnfgrNoopProcess( &response, &reason );
                dvlantagCnfgrState = DVLANTAG_PHASE_WMU;
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
                dvlantagCnfgrState = DVLANTAG_PHASE_EXECUTE;

                dvlantagRC  = L7_SUCCESS;
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
                dvlantagRC = dvlantagCnfgrNoopProcess( &response, &reason );
                dvlantagCnfgrState = DVLANTAG_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                dvlantagRC = dvlantagCnfgrUconfigPhase2( &response, &reason );
                dvlantagCnfgrState = DVLANTAG_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            dvlantagRC = dvlantagCnfgrNoopProcess( &response, &reason );
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
  cbData.asyncResponse.rc = dvlantagRC;
  if (dvlantagRC == L7_SUCCESS)
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
L7_RC_t dvlantagCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dvlantagRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  dvlantagRC = L7_SUCCESS;

  /* Allocate and initialized memory for global data */
  dvlantagRC = dtagMemoryMalloc();

  /* Make sure that allocation succeded */
  if (dvlantagRC != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    dvlantagRC = L7_ERROR;

    return dvlantagRC;
  }

  if (osapiRWLockCreate(&dvlantagCfgRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    LOG_MSG("dvlantagCnfgrInitPhase1Process: Unable to create r/w lock for dvlantag\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    dvlantagRC = L7_ERROR;

    return dvlantagRC;
  }

  dvlantagCnfgrState = DVLANTAG_PHASE_INIT_1;

  return dvlantagRC;

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
L7_RC_t dvlantagCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dvlantagRC;
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  dvlantagRC = L7_SUCCESS;

  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_DVLANTAG_COMPONENT_ID;
  notifyFunctionList.notifySave     = dvlantagSave;
  notifyFunctionList.hasDataChanged = dvlantagHasDataChanged;
  notifyFunctionList.notifyConfigDump = dvlantagCfgDump;
  notifyFunctionList.resetDataChanged = dvlantagResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    dvlantagRC = L7_ERROR;

    return dvlantagRC;
  }

  /* register NIM callback to support interface changes */
  if ((dvlantagRC = nimRegisterIntfChange(L7_DVLANTAG_COMPONENT_ID,
                                          dvlantagIntfChangeCallBack)) != L7_SUCCESS)
  {
    LOG_MSG("Failed nim registration for Interface Change Callback\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    dvlantagRC = L7_ERROR;

    return dvlantagRC;
  }

  dvlantagCnfgrState = DVLANTAG_PHASE_INIT_2;

  return dvlantagRC;
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
L7_RC_t dvlantagCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dvlantagRC;


  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  dvlantagRC = L7_SUCCESS;

  if (cnfgrIsFeaturePresent(L7_DVLANTAG_COMPONENT_ID, 
							L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_TRUE &&
	  cnfgrIsFeaturePresent(L7_DVLANTAG_COMPONENT_ID, 
							L7_DVLANTAG_ETHERTYPE_PER_INTERFACE_FEATURE_ID) == L7_FALSE)
  {
	dvlantagDeviceEthertypePresent = L7_TRUE;
  }
  else
  {
	dvlantagDeviceEthertypePresent = L7_FALSE;
  }

  if (sysapiCfgFileGet(L7_DVLANTAG_COMPONENT_ID, DVLANTAG_CFG_FILENAME, 
                       (L7_char8 *)dvlantagCfg, sizeof(DVLANTAG_CFG_t), 
                       &dvlantagCfg->checkSum, DVLANTAG_CFG_VER_CURRENT, 
                       dvlantagBuildDefaultConfigData, dvlantagMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    dvlantagRC = L7_ERROR;

    return dvlantagRC;
  }

  if (dvlantagApplyConfigData() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    dvlantagRC = L7_ERROR;

    return dvlantagRC;
  }

  dvlantagCfg->cfgHdr.dataChanged = L7_FALSE;

  dvlantagCnfgrState = DVLANTAG_PHASE_INIT_3;

  return dvlantagRC;
}


/*********************************************************************
* @purpose  This function undoes dvlantagCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dvlantagCnfgrFiniPhase1Process()
{
  /* Deallocate anything that was allocated */
  dtagMemoryFree();

  dvlantagCnfgrState = DVLANTAG_PHASE_INIT_0;
}


/*********************************************************************
* @purpose  This function undoes dvlantagCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dvlantagCnfgrFiniPhase2Process()
{
  nimDeRegisterIntfChange(L7_DVLANTAG_COMPONENT_ID);
  nvStoreDeregister(L7_DVLANTAG_COMPONENT_ID);

  dvlantagCnfgrState = DVLANTAG_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes dvlantagCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dvlantagCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place dvlantagCnfgrState to WMU */
  dvlantagCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t dvlantagCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dvlantagRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(dvlantagRC);
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
L7_RC_t dvlantagCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{

  L7_RC_t dvlantagRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  dvlantagRC  = L7_SUCCESS;

  /* Remove double VLAN tagging from any interface on which it is configured */
  dvlantagCfgRemove();

  /* Clear out the configuration */
  memset(dvlantagCfg, 0, sizeof(DVLANTAG_CFG_t));

  dvlantagDeviceEthertype = L7_DVLANTAG_DEFAULT_ETHERTYPE;
   dvlantagDefaultTpidApply(dvlantagDeviceEthertype);

  dvlantagCnfgrState = DVLANTAG_PHASE_WMU;

  return dvlantagRC;
}


