/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  vrrp_cnfgr.c
*
* @purpose   VRRP Configurator functions
*
* @component 
*
* @comments  
*
* @create    11/19/2001
*
* @author    ksdesai
*
* @end
*             
**********************************************************************/

/*************************************************************
        
*************************************************************/

#include "l7_common.h"
#include "l7_common_l3.h"
#include "l7_cnfgr_api.h"
#include "defaultconfig.h"
#include "nvstoreapi.h"
#include "sysnet_api_ipv4.h"
#include "nimapi.h"
#include "l7_ip_api.h"
#include "l3_default_cnfgr.h"
#include "buff_api.h"
#include "l7_vrrp_api.h"
#include "vrrp_config.h"
#include "vrrp_cnfgr.h"
#include "vrrp_util.h"
#include "vrrptask.h"
#include "rto_api.h"


vrrpCnfgrState_t vrrpCnfgrState = VRRP_PHASE_INIT_0;

extern L7_vrrpMapCfg_t   *pVrrpMapCfgData; 
extern L7_uint32         *vrrpIntfMapTbl;
extern vrrpRouterBasicConfiguration_t  vrrpBasicConfig_g;
extern void  *vrrp_Queue;   
extern void     *VrrpConfigSema;
extern L7_int32  vrrpTaskDaemonId;
extern vrrpList_t * listMemPool_g[L7_VRRP_INTF_MAX_COUNT + 1];
extern vrrpRouterInfo_t * routersArray_g[L7_VRRP_INTF_MAX_COUNT + 1];
extern vrrpIntfInformation_t * vrrpIntfInfoTbl;
extern L7_uchar8 *  vrrpVRIdToIndexMapTbl[L7_IPMAP_INTF_MAX_COUNT];
extern osapiTimerDescr_t   *pVRRPAdTimerHolder;
extern osapiTimerDescr_t   *pVRRPMasterTimerHolder;
extern vrrpMapInfo_t *pVrrpMapInfo;
extern L7_uint32 vrrpBufferPoolId;

/* array of rtoRouteChange_t objects for best route notification. */
extern rtoRouteChange_t  *vrrpMapRouteChangeBuf;

extern void vrrpDebugRegister(void);
extern void vrrpDebugCfgRead(void);

extern void vrrpVRIdToIndexMapTblInit();
L7_RC_t vrrpCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t vrrpCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

static void vrrpFuncTableInit(void);

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
void vrrpApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             vrrpRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason    = L7_CNFGR_ERR_RC_INVALID_PAIR;

  vrrpMsg_t msg;

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
          if( (command == L7_CNFGR_CMD_INITIALIZE) && (request == L7_CNFGR_RQST_I_PHASE1_START) )
          {
              if ((vrrpRC = vrrpCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
              {
                  vrrpCnfgrFiniPhase1Process();
              }

          }
          else
          {
              /* Put a msg into the msgQ. The VRRP daemon task will handle it */
              msg.msg_id = vrrpMsgCnfgr;
              memcpy(&msg.type.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
              
              vrrpRC = osapiMessageSend( vrrp_Queue, &msg, VRRP_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM );
              
              if (vrrpRC != L7_SUCCESS)
              {
                  LOG_MSG("%s: %d: vrrpApiCnfgrCommand: osapiMessageSend failed\n",__FILE__, __LINE__);
              }

              /* The VRRP daemon task will take care of letting the cnfgr know when the command has been handled */
              return;
          }
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
  cbData.asyncResponse.rc = vrrpRC;
  if (vrrpRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}

void vrrpCnfgrParse( L7_CNFGR_CMD_DATA_t *pCmdData )
{
    
    /* set up variables and structures */
    L7_CNFGR_CMD_t        command;
    L7_CNFGR_RQST_t       request;
    L7_CNFGR_CB_DATA_t    cbData;
    L7_CNFGR_RESPONSE_t   response;
    L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

    L7_RC_t             vrrpRC = L7_ERROR;
    L7_CNFGR_ERR_RC_t   reason    = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                        if ((vrrpRC = vrrpCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                        {
                            vrrpCnfgrFiniPhase1Process();
                        }break;
                    case L7_CNFGR_RQST_I_PHASE2_START:
                        if ((vrrpRC = vrrpCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                        {
                            vrrpCnfgrFiniPhase2Process();
                        }
                        break;
                    case L7_CNFGR_RQST_I_PHASE3_START:
                        if ((vrrpRC = vrrpCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                        {
                            vrrpCnfgrFiniPhase3Process();
                        }
                        break;
                    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                        vrrpRC = vrrpCnfgrNoopProcess( &response, &reason );
                        vrrpCnfgrState = VRRP_PHASE_WMU;
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
                        vrrpCnfgrState = VRRP_PHASE_EXECUTE;
                        vrrpRC = L7_SUCCESS;
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
                        vrrpRC = vrrpCnfgrNoopProcess( &response, &reason );
                        vrrpCnfgrState = VRRP_PHASE_UNCONFIG_1;
                        break;

                    case L7_CNFGR_RQST_U_PHASE2_START:
                        vrrpRC = vrrpCnfgrUnconfigPhase2( &response, &reason );
                        vrrpCnfgrState = VRRP_PHASE_UNCONFIG_2;
                        /* we need to do something with the stats in the future */
                        break;

                    default:
                        /* invalid command/request pair */
                        break;
                    }
                    break;

                case L7_CNFGR_CMD_TERMINATE:
                case L7_CNFGR_CMD_SUSPEND:
                    vrrpRC = vrrpCnfgrNoopProcess( &response, &reason );
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
    cbData.asyncResponse.rc = vrrpRC;
    if (vrrpRC == L7_SUCCESS)
        cbData.asyncResponse.u.response = response;
    else
        cbData.asyncResponse.u.reason   = reason;

    cnfgrApiCallback(&cbData);

    return;
}

/*********************************************************************
*
* @purpose  Initialize the function pointers in the function table
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void vrrpFuncTableInit(void)
{
    vrrpFuncTable.L7_vrrpIsActive              = L7_vrrpIsActive;
    vrrpFuncTable.vrrpMapGetVMac               = vrrpMapGetVMac;
    vrrpFuncTable.L7_vrrpAdminModeGet          = L7_vrrpAdminModeGet;
    vrrpFuncTable.L7_vrrpOperVirtualMacAddrGet = L7_vrrpOperVirtualMacAddrGet;
    vrrpFuncTable.L7_vrrpIpGet                 = L7_vrrpIpGet;
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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t vrrpCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t vrrpRC = L7_SUCCESS;

  L7_uint32 i;

  /* use single-pass loop for exception control */
  do
  {
    /*-----------------------------------------------------------*/
    /* malloc space for the vrrpMap configuration data structure  */
    /*-----------------------------------------------------------*/
    pVrrpMapCfgData = (L7_vrrpMapCfg_t *)osapiMalloc(L7_VRRP_MAP_COMPONENT_ID, sizeof(L7_vrrpMapCfg_t));

    if (pVrrpMapCfgData == L7_NULLPTR)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      vrrpRC   = L7_ERROR;
      break;
    }
    memset(pVrrpMapCfgData, 0, sizeof(L7_vrrpMapCfg_t));

    vrrpIntfMapTbl  = osapiMalloc(L7_VRRP_MAP_COMPONENT_ID, sizeof(L7_uint32) * L7_VRRP_INTF_MAX_COUNT);

    if (vrrpIntfMapTbl == L7_NULL)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      vrrpRC   = L7_ERROR;
      break;
    }
    memset(vrrpIntfMapTbl, 0, sizeof(L7_uint32) * L7_VRRP_INTF_MAX_COUNT);
    
    for(i=1; i<=L7_VRRP_INTF_MAX_COUNT; i++)
    {
        listMemPool_g[i] = osapiMalloc(L7_VRRP_MAP_COMPONENT_ID, sizeof(vrrpList_t) * L7_VRRP_MAX_VIRT_ROUTERS);
        routersArray_g[i] = osapiMalloc(L7_VRRP_MAP_COMPONENT_ID, sizeof(vrrpRouterInfo_t) * L7_VRRP_MAX_VIRT_ROUTERS);
    }

    for(i=1; i<=L7_IPMAP_INTF_MAX_COUNT; i++)
    {
        vrrpVRIdToIndexMapTbl[i-1] = osapiMalloc(L7_VRRP_MAP_COMPONENT_ID, sizeof(L7_uchar8) * L7_RTR_MAX_VRRP_PER_INTERFACE);
    }
    vrrpVRIdToIndexMapTblInit();

    vrrpIntfInfoTbl = osapiMalloc(L7_VRRP_MAP_COMPONENT_ID, sizeof(vrrpIntfInformation_t) * (L7_VRRP_INTF_MAX_COUNT + 1));
    memset(vrrpIntfInfoTbl, 0, sizeof(vrrpIntfInformation_t) * (L7_VRRP_INTF_MAX_COUNT + 1));
    
    pVrrpMapInfo              = (vrrpMapInfo_t *)osapiMalloc(L7_VRRP_MAP_COMPONENT_ID, sizeof(vrrpMapInfo_t));
    /* Create buffer for best route notification */
    vrrpMapRouteChangeBuf = (rtoRouteChange_t*) osapiMalloc(L7_VRRP_MAP_COMPONENT_ID, L7_VRRP_MAX_BEST_ROUTE_CHANGES *
                              sizeof(rtoRouteChange_t));
    if (vrrpMapRouteChangeBuf == NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_VRRP_MAP_COMPONENT_ID,
          "Unable to allocate VRRP route change buffer.");
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      vrrpRC      = L7_ERROR;
      return vrrpRC;
    }
    memset((void*) vrrpMapRouteChangeBuf, 0,
        L7_VRRP_MAX_BEST_ROUTE_CHANGES * sizeof(rtoRouteChange_t));


    /*--------------------------------*/
    /* Initializing global parameters */
    /*--------------------------------*/
    bzero((L7_uchar8 *) &vrrpBasicConfig_g, sizeof(vrrpBasicConfig_g));
    memset(pVrrpMapInfo,              0,  sizeof(vrrpMapInfo_t));
    vrrpBasicConfig_g.vrrp_version = 2;
    vrrpBasicConfig_g.notif_flag =  L7_ENABLE;

    if (bufferPoolInit(50, VRRP_PACKET_LENGTH, "VRRP Bufs", &vrrpBufferPoolId) != L7_SUCCESS)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      vrrpRC   = L7_ERROR;
      return vrrpRC;
    }

    VrrpConfigSema = (void *)osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    if (VrrpConfigSema == L7_NULLPTR)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      vrrpRC   = L7_ERROR;
      break;
    }

    vrrp_Queue = (void *)osapiMsgQueueCreate(VRRP_QUEUE, VRRP_MSG_COUNT, VRRP_MSG_SIZE);
    if (vrrp_Queue == L7_NULLPTR)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      vrrpRC   = L7_ERROR;
      break;
    }

    /* JASON TBD:  INITIALIZATION OF STATSMGR CALLS HERE!  */
    if (vrrpStartTasks() != L7_SUCCESS)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      vrrpRC   = L7_ERROR;
      break;
    }

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;

    vrrpCnfgrState = VRRP_PHASE_INIT_1;
    
    /* Read and apply trace configuration at phase 1.
       This allows for tracing during system initialization and
       during clear config */
    vrrpDebugCfgRead();
    vrrpApplyDebugConfigData();

    vrrpFuncTableInit();

    /* success path returns here */
    return(vrrpRC);

  } while (0);

  /* NOTE:  Put any component cleanup here in the event of an init failure */

  pVrrpMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  return(vrrpRC);
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
L7_RC_t vrrpCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  nvStoreFunctionList_t   notifyFunctionList;
  L7_RC_t vrrpRC = L7_SUCCESS;

  /* Register for needed notifications */
  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_VRRP_MAP_COMPONENT_ID;
  notifyFunctionList.notifySave     = vrrpSave;
  notifyFunctionList.hasDataChanged = vrrpHasDataChanged;
  notifyFunctionList.resetDataChanged = vrrpResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    vrrpRC = L7_ERROR;
  }
  else if (nimRegisterIntfChange(L7_VRRP_MAP_COMPONENT_ID, vrrpMapIntfChangeCallback) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    vrrpRC = L7_ERROR;
  }
  if (ipMapRegisterRoutingEventChange(L7_IPRT_VRRP, "vrrpMapRoutingEventChangeCallBack",
                                      vrrpMapRoutingEventChangeCallBack) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    vrrpRC = L7_ERROR;
  }
  
  /* register for best route changes */
  if ((rtoBestRouteClientRegister ("VRRP", vrrpMapRouteCallback)) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    vrrpRC    = L7_ERROR;
  }

  else
  {
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    vrrpCnfgrState = VRRP_PHASE_INIT_2;
  }

  /* register for debug */
    vrrpDebugRegister();
    
  /* Return Value to caller */
  return(vrrpRC);
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
L7_RC_t vrrpCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t vrrpRC = L7_SUCCESS;

  vrrpRC = sysapiCfgFileGet(L7_VRRP_MAP_COMPONENT_ID, L7_VRRP_CFG_FILENAME, (L7_char8 *)pVrrpMapCfgData,
                            sizeof(L7_vrrpMapCfg_t), &pVrrpMapCfgData->checkSum, L7_VRRP_CFG_VER_CURRENT,
                            vrrpBuildDefaultConfigData, vrrpMigrateConfigData);

  pVrrpMapCfgData->cfgHdr.dataChanged = L7_FALSE;

  if ( vrrpRC != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    vrrpRC = L7_ERROR;

  }
  else
  {
      /* Task has been created. Queue has ben created. Now kick off the initial timers. */
      VRRPAdvertTimerCheck();
      VRRPMasterCheck();

      if( vrrpApplyConfigData() != L7_SUCCESS)
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          vrrpRC = L7_ERROR;
      }
      else
      {
          *pResponse = L7_CNFGR_CMD_COMPLETE;
          *pReason   = 0;
          vrrpCnfgrState = VRRP_PHASE_INIT_3;
      }
  }

  return(vrrpRC);
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
L7_RC_t vrrpCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t vrrpRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(vrrpRC);
}



/*********************************************************************
* @purpose  This function undoes vrrpCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void vrrpCnfgrFiniPhase1Process()
{
    L7_uint32   i;

  /* Free Configuration Structure */
  if (pVrrpMapCfgData != L7_NULLPTR )
    osapiFree(L7_VRRP_MAP_COMPONENT_ID, pVrrpMapCfgData);

  /* Clean up global data */
  bzero((L7_uchar8 *) &vrrpBasicConfig_g, sizeof(vrrpBasicConfig_g));

  if(vrrpIntfMapTbl != L7_NULLPTR)
      osapiFree(L7_VRRP_MAP_COMPONENT_ID, vrrpIntfMapTbl);

  for(i=1; i<=L7_VRRP_INTF_MAX_COUNT; i++)
  {
      if(listMemPool_g[i] != L7_NULLPTR)
      {
          osapiFree(L7_VRRP_MAP_COMPONENT_ID, listMemPool_g[i]);
      }

      if(routersArray_g[i] != L7_NULLPTR)
      {
          osapiFree(L7_VRRP_MAP_COMPONENT_ID, routersArray_g[i]);
      }
  }

  if(vrrpIntfInfoTbl != L7_NULLPTR)
  {
      osapiFree(L7_VRRP_MAP_COMPONENT_ID, vrrpIntfInfoTbl);
  }

  if (vrrpBufferPoolId != 0)
  {
    bufferPoolDelete(vrrpBufferPoolId);
    vrrpBufferPoolId = 0;
  }

  if (vrrp_Queue  != L7_NULLPTR )
    osapiMsgQueueDelete(vrrp_Queue);

  if (VrrpConfigSema  != L7_NULLPTR )
    osapiSemaDelete(VrrpConfigSema);

  if (vrrpTaskDaemonId  != L7_ERROR )
    osapiTaskDelete(vrrpTaskDaemonId);
  
  if (vrrpMapRouteChangeBuf)
  {
    osapiFree(L7_VRRP_MAP_COMPONENT_ID, vrrpMapRouteChangeBuf);
    vrrpMapRouteChangeBuf = NULL;
  }


  vrrpCnfgrState = VRRP_PHASE_INIT_0;

}
/*********************************************************************
* @purpose  This function undoes vrrpCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void vrrpCnfgrFiniPhase2Process()
{
    (void)nimDeRegisterIntfChange(L7_VRRP_MAP_COMPONENT_ID);
    (void)ipMapDeregisterRoutingEventChange( L7_VRRP_MAP_COMPONENT_ID );
    (void) nvStoreDeregister(L7_IPRT_VRRP);
    
    vrrpCnfgrState = VRRP_PHASE_INIT_1;
    
    return;
}
/*********************************************************************
* @purpose  This function undoes vrrpCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void vrrpCnfgrFiniPhase3Process()
{
    
    L7_CNFGR_RESPONSE_t response;
    L7_CNFGR_ERR_RC_t   reason;

    vrrpCnfgrUnconfigPhase2(&response, &reason);
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t vrrpCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t rc;

    osapiTimerFree(pVRRPAdTimerHolder);
    osapiTimerFree(pVRRPMasterTimerHolder);

    rc = vrrpRestoreProcess();
/*     memset((void *)vrrpIntfMapTbl, 0, sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT); */
    memset(pVrrpMapCfgData, 0, sizeof(L7_vrrpMapCfg_t));

    vrrpCnfgrState = VRRP_PHASE_WMU;
    return rc;
}

