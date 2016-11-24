/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  rip_map_cnfgr.c
*
* @purpose   Router Discovery API functions
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
#include "l7_packet.h"
#include "defaultconfig.h"
#include "nvstoreapi.h"
#include "sysnet_api_ipv4.h"
#include "nimapi.h"
#include "l7_ip_api.h"
#include "l7_rip_api.h"
#include "l3_default_cnfgr.h"
#include "rip_map_cnfgr.h"
#include "rip_config.h"
#include "rip_util.h"
#include "rip_vend_exten.h"
#include "rip_map_table.h"
#include "rto_api.h"

ripMapCnfgrState_t ripMapCnfgrState = RIPMAP_PHASE_INIT_0;

extern void               *ripMapSemaphore;
extern ripMapCfg_t        *pRipMapCfgData;
extern L7_uint32          *pRipMapMapTbl;
extern ripInfo_t          *pRipInfo;

extern ripMapDebugCfg_t ripMapDebugCfg;    /* Debug Configuration File Overlay */
extern void ripMapDebugRegister(void);
extern void ripMapDebugCfgRead(void);

/* This points to the first element in an array. Elements in the 
* array are indexed on internal interface number. The first valid
* index is 1 and the last is L7_RIP_MAP_INTF_MAX_COUNT.
*/
extern ripIntfInfo_t      *pRipIntfInfo;
extern rtoRouteChange_t *ripRouteChangeBuf;

L7_RC_t ripMapCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t ripMapCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

static void ripFuncTableInit(void);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for IpMap component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the ipMap comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void ripMapApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  ripMapMsg_t   msg;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_RC_t               ripMapRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t     reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

/*
 * Let all but PHASE 1 start fall through into an osapiMessageSend.
 * The application task will handle everything.
 * Phase 1 may do a osapiMessageSend after a few pre reqs have been
 * completed
 */

  if ( pCmdData == L7_NULL)
  {
    cbData.correlator               = L7_NULL;
    cbData.asyncResponse.rc         = L7_ERROR; 
    cbData.asyncResponse.u.reason   = L7_CNFGR_ERR_RC_INVALID_CMD;

    cnfgrApiCallback(&cbData);
    return;
  }

  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) && 
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
      if ((ripMapRC = ripMapCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
      {
        ripMapCnfgrFiniPhase1Process();
      }

      cbData.correlator = pCmdData->correlator;
      /* return value to caller - 
       * <prepare complesion response>
       * <callback the configurator>
       */
      cbData.asyncResponse.rc = ripMapRC;
      if (ripMapRC == L7_SUCCESS)
        cbData.asyncResponse.u.response = response;
      else
        cbData.asyncResponse.u.reason   = reason;

      cnfgrApiCallback(&cbData);

  }
  else
  {
    L7_RC_t rc;
    memcpy(&msg.type.cnfgrData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.msgId = RIPMAP_CNFGR;
    rc = osapiMessageSend(pRipInfo->ripMapProcessQueue, &msg, sizeof(ripMapMsg_t), 
                     L7_NO_WAIT, L7_MSG_PRIORITY_NORM); 

    if (rc == L7_SUCCESS)
    {
        osapiSemaGive(pRipInfo->msgQSema);
    }
    else
    {
        L7_LOG(L7_LOG_SEVERITY_ALERT, L7_RIP_MAP_COMPONENT_ID, 
               "Failed to send configurator message to RIP thread.");
    }
  }
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
static void ripFuncTableInit(void)
{
    ripFuncTable.ripMapCfgDataShow = ripMapCfgDataShow;
}

/*********************************************************************
* @purpose  This function processes the configurator control commands/request
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
L7_RC_t ripMapCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ripMapRC = L7_SUCCESS;

  do
  {
    /*-----------------------------------------------------------*/
    /* malloc space for the ripMap configuration data structure  */
    /*-----------------------------------------------------------*/
    pRipMapCfgData = (ripMapCfg_t *)osapiMalloc(L7_RIP_MAP_COMPONENT_ID, sizeof(ripMapCfg_t));
    if (pRipMapCfgData == L7_NULL)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      ripMapRC   = L7_ERROR;
      break;
    }

    /*-----------------------------------------------------------*/
    /* malloc space for the ripMap configuration data structure  */
    /*-----------------------------------------------------------*/
    pRipMapMapTbl = osapiMalloc(L7_RIP_MAP_COMPONENT_ID, sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);

    if (pRipMapMapTbl == L7_NULL)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      ripMapRC   = L7_ERROR;
      break;
    }

    /*----------------------------------------------*/
    /* malloc space for various RIP info structures */
    /*----------------------------------------------*/
    pRipInfo = (ripInfo_t *)osapiMalloc(L7_RIP_MAP_COMPONENT_ID, sizeof(ripInfo_t));
    if (pRipInfo == L7_NULL)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      ripMapRC   = L7_ERROR;
      break;
    }
    memset(pRipInfo, 0, sizeof(ripInfo_t));  

    pRipIntfInfo = (ripIntfInfo_t *)osapiMalloc(L7_RIP_MAP_COMPONENT_ID, sizeof(ripIntfInfo_t) * 
                                                (L7_RIP_MAP_INTF_MAX_COUNT + 1));
    if (pRipIntfInfo == L7_NULLPTR)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      ripMapRC   = L7_ERROR;
      break;
    }
    memset(pRipIntfInfo, 0, sizeof(ripIntfInfo_t)*(L7_RIP_MAP_INTF_MAX_COUNT + 1));

    /* Create buffer for best route notification */
    ripRouteChangeBuf = (rtoRouteChange_t*) osapiMalloc(L7_RIP_MAP_COMPONENT_ID,
                                                         L7_RIP_MAX_BEST_ROUTE_CHANGES * 
                                                         sizeof(rtoRouteChange_t));
    if (ripRouteChangeBuf == NULL)
    {
      L7_LOG(L7_LOG_SEVERITY_ALERT, L7_RIP_MAP_COMPONENT_ID,
             "Unable to allocate RIP route change buffer.");
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      return L7_ERROR;
    } 
    memset((void*) ripRouteChangeBuf, 0, 
           L7_RIP_MAX_BEST_ROUTE_CHANGES * sizeof(rtoRouteChange_t));


    ripMapRouteTableInit();

    /* The signal for all RIP conditions are blocking.  
    
       The vendor code has only one point of completion for all interface events, and
       that point of completion is not specific to any one interface. Thus, the completion 
       signal if defined in the ripInfo structure.
       
       RIP has no need for a callback routine for its signal as 
        1. all of its signal starts/ends are being handled between two specific functions
           between two specific tasks.
        2. all points of signal start are on the same thread.
     */   

    if ( asyncEventSignalCreate( &(pRipInfo->ripVendIfCompletionSignal), 
                                 ASYNC_EVENT_SIGNAL_FLAG_SEMAB_EMPTY) != L7_SUCCESS)  
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
        ripMapRC   = L7_ERROR;
        break;
    }


    /* initialize global control variables */
    ripMapVendCtrlCtxInit();

    /*------------------------------------------*/
    /* initialize the RIP stack and start tasks */
    /*------------------------------------------*/
    if (pRipInfo->ripInitialized != L7_TRUE)
    {
      if (ripMapVendInit() != L7_SUCCESS)
      {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        ripMapRC   = L7_ERROR;
        break;
      }
      pRipInfo->ripInitialized = L7_TRUE;
    }

    /*------------------------------------------*/
    /* create global configuration semaphore    */
    /*------------------------------------------*/
    ripMapSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
    if (ripMapSemaphore == L7_NULLPTR)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      ripMapRC   = L7_ERROR;
      break;
    }

    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;

    /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
    ripMapDebugCfgRead();
    ripMapApplyDebugConfigData();

    ripMapCnfgrState = RIPMAP_PHASE_INIT_1;

  } while (0);

  ripFuncTableInit();

  return(ripMapRC);
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
L7_RC_t ripMapCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ripMapRC = L7_SUCCESS;
  nvStoreFunctionList_t notifyFunctionList;
  sysnetPduIntercept_t sysnetPduIntercept;

  /*----------------------*/
  /* nvStore registration */
  /*----------------------*/
  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_RIP_MAP_COMPONENT_ID;
  notifyFunctionList.notifySave     = ripMapSave;
  notifyFunctionList.hasDataChanged = ripMapHasDataChanged;
  notifyFunctionList.resetDataChanged = ripMapResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ripMapRC = L7_ERROR;
  }

  /*------------------------------------------*/
  /* register callbacks with IP Mapping layer */
  /*------------------------------------------*/
  /* register RIP to receive a callback when an interface is enabled
   * or disabled for routing, or when the IP address of a router 
   * interface changes
   */
  else if (ipMapRegisterRoutingEventChange(L7_IPRT_RIP,"ripMapRoutingEventChangeCallBack", 
                                           ripMapRoutingEventChangeCallBack) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ripMapRC = L7_ERROR;
  }

  /*--------------------------------------------------*/
  /* register callback with NIM for interface changes */
  /*--------------------------------------------------*/
  else if (nimRegisterIntfChange(L7_RIP_MAP_COMPONENT_ID, ripMapIntfChangeCallback) 
           != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ripMapRC = L7_ERROR;
  }
  else
  {

    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_LOCAL_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_RIP_FILTER_PRECEDENCE;
    sysnetPduIntercept.interceptFunc = ripMapFrameFilter;
    strcpy(sysnetPduIntercept.interceptFuncName, "ripMapFrameFilter");
    /* Register with sysnet */
    if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_FATAL;
      ripMapRC = L7_ERROR;
    }
    else
    {
      *pResponse = L7_CNFGR_CMD_COMPLETE;
      *pReason   = 0;
    }
  }    

  /* register for debug */
  ripMapDebugRegister();

  if (ripMapRC == L7_SUCCESS) 
  {
      ripMapCnfgrState = RIPMAP_PHASE_INIT_2;
  }
  /* Return Value to caller */
  return(ripMapRC);
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
L7_RC_t ripMapCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ripMapRC = L7_SUCCESS;

  ripMapRC = sysapiCfgFileGet(L7_RIP_MAP_COMPONENT_ID, L7_RIP_MAP_CFG_FILENAME, (L7_char8 *)pRipMapCfgData,
                              sizeof(ripMapCfg_t), &pRipMapCfgData->checkSum, L7_RIP_MAP_CFG_VER_CURRENT,
                              ripMapBuildDefaultConfigData, ripMapMigrateConfigData);

  do
  {
      if ( ripMapRC != L7_SUCCESS )
      {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        ripMapRC = L7_ERROR;
        break;
      }

      /*---------------------------------*/
      /* Initialize the RIP application  */
      /*---------------------------------*/


      if (ripMapAppsInit() != L7_SUCCESS)
      {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        ripMapRC   = L7_ERROR;
        break;
      }
      pRipMapCfgData->cfgHdr.dataChanged = L7_FALSE;


    /*---------------------------------*/
    /* Successful return               */
    /*---------------------------------*/
     *pResponse = L7_CNFGR_CMD_COMPLETE;
     *pReason   = 0;

     ripMapCnfgrState = RIPMAP_PHASE_INIT_3;

  } while (0);

  /* Return Value to caller */
  return(ripMapRC);
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
L7_RC_t ripMapCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ripMapRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(ripMapRC);
}



/*********************************************************************
* @purpose  This function undoes policyCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ripMapCnfgrFiniPhase1Process()
{
   (void)ripMapVendDeinit();
   ripMapVendCtrlCtxInit();
   (void)asyncEventSignalDelete(&(pRipInfo->ripVendIfCompletionSignal));
   osapiFree(L7_RIP_MAP_COMPONENT_ID, pRipIntfInfo);
   osapiFree(L7_RIP_MAP_COMPONENT_ID, pRipInfo);
   osapiFree(L7_RIP_MAP_COMPONENT_ID, pRipMapMapTbl);
   osapiFree(L7_RIP_MAP_COMPONENT_ID, pRipMapCfgData);
   
   if (ripMapSemaphore != L7_NULLPTR)
   {
     osapiSemaDelete(ripMapSemaphore);
     ripMapSemaphore = L7_NULLPTR;
   }

   ripMapCnfgrState = RIPMAP_PHASE_INIT_0;
}
/*********************************************************************
* @purpose  This function undoes policyCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ripMapCnfgrFiniPhase2Process()
{

    sysnetPduIntercept_t sysnetPduIntercept;
                     
    /* Deregister with sysnet */

    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_LOCAL_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_RIP_FILTER_PRECEDENCE;
    sysnetPduIntercept.interceptFunc = ripMapFrameFilter;
    strcpy(sysnetPduIntercept.interceptFuncName, "ripMapFrameFilter");
    (void)sysNetPduInterceptRegister(&sysnetPduIntercept);

    /* Deregister remaining registrations */

    (void)nimDeRegisterIntfChange(L7_RIP_MAP_COMPONENT_ID);
    (void)ipMapDeregisterRoutingEventChange( L7_RIP_MAP_COMPONENT_ID );
    (void) nvStoreDeregister(L7_IPRT_RIP);


   ripMapCnfgrState = RIPMAP_PHASE_INIT_1;

   return;
}
/*********************************************************************
* @purpose  This function undoes policyCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ripMapCnfgrFiniPhase3Process()
{    
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;
  
  /* this func will place ripMapCnfgrState to WMU */
  ripMapCnfgrUnconfigPhase2(&response, &reason);
  
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
L7_RC_t ripMapCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{

  L7_RC_t ipMapRC;
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  ipMapRC     = L7_SUCCESS; 

  /* this just disables RIP processing, 
     the default config will be rebuilt in Phase 3 */
  if (pRipMapCfgData->ripAdminMode != L7_DISABLE) 
  {
    pRipMapCfgData->ripAdminMode = L7_DISABLE;
    (void)ripMapAdminModeDisable();
  }

  ripMapCnfgrState = RIPMAP_PHASE_WMU;

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To parse the configurator commands send to l7_proccess task
*
* @param    none
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void ripMapCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             ripMapRC = L7_ERROR;
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
#if 0  /* Done on configurator thread */
            case L7_CNFGR_RQST_I_PHASE1_START:
              if ((ripMapRC = ripMapCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
              {
                ripMapCnfgrFiniPhase1Process();
              }
              break;
#endif
            case L7_CNFGR_RQST_I_PHASE2_START:
              if ((ripMapRC = ripMapCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
              {
                ripMapCnfgrFiniPhase2Process();
              }
              break;
            case L7_CNFGR_RQST_I_PHASE3_START:
              if ((ripMapRC = ripMapCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
              {
                ripMapCnfgrFiniPhase3Process();
              }
              break;
            case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
              ripMapRC = ripMapCnfgrNoopProcess( &response, &reason );
              ripMapCnfgrState = RIPMAP_PHASE_WMU;
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
              ripMapCnfgrState = RIPMAP_PHASE_EXECUTE;

              ripMapRC = L7_SUCCESS;
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
              ripMapRC = ripMapCnfgrNoopProcess( &response, &reason );
              ripMapCnfgrState = RIPMAP_PHASE_UNCONFIG_1;
              break;

            case L7_CNFGR_RQST_U_PHASE2_START:
              ripMapRC = ripMapCnfgrUnconfigPhase2( &response, &reason );
              ripMapCnfgrState = RIPMAP_PHASE_UNCONFIG_2;
              break;

            default:
              /* invalid command/request pair */
              break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            ripMapRC = ripMapCnfgrNoopProcess( &response, &reason );
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
  cbData.asyncResponse.rc = ripMapRC;
  if (ripMapRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}


