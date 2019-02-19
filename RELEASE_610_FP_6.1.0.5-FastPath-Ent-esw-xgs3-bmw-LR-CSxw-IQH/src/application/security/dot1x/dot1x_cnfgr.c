/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dot1x_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component 
*
* @comments 
*
* @create 08/07/2003
*
* @author mfiorito
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "nvstoreapi.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "dot1x_include.h"
#include "dot1q_api.h"
#include "dot1x_debug.h"
#include "tlv_api.h"

extern void *dot1xQueue;
dot1xCnfgrState_t dot1xCnfgrState = DOT1X_PHASE_INIT_0;
L7_tlvHandle_t    dot1xTlvHandle;

extern L7_int32 dot1xTaskId;
extern dot1xCfg_t *dot1xCfg;
L7_VLAN_MASK_t dot1xVlanMask;
L7_ushort16 *dot1xVlanIntfCount;
L7_VLAN_MASK_t dot1xGuestVlanMask;
L7_ushort16 *dot1xGuestVlanIntfCount;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for ACL component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @comments  This API is provided to allow the Configurator to issue a
*           request to the dot1x comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void dot1xApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
/*
 * Let all but PHASE 1 start fall through into an dot1xIssueCmd.
 * The application task will handle everything.
 * Phase 1 will do a dot1xIssueCmd after a few pre reqs have been
 * completed
 */

  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) && 
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* this function will invoke the message send for phase 1 */
    if (dot1xInit(pCmdData) != L7_SUCCESS)
      dot1xInitUndo();
  }
  else
  {
    if (dot1xIssueCmd(dot1xCnfgr, L7_NULL, pCmdData) != L7_SUCCESS)
	  L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID, "dot1xApiCnfgrCommand: Failed calling dot1xIssueCmd. "
															"802.1X message queue is full\n");
  }
}


/*********************************************************************
*
* @purpose  System Initialization for dot1x component
*
* @param    none
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t dot1xInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CB_DATA_t cbData;

  dot1xQueue = (void *)osapiMsgQueueCreate("dot1xQueue", DOT1X_MSG_COUNT, (L7_uint32)sizeof(dot1xMsg_t));
  if (dot1xQueue == L7_NULLPTR)
  {
    LOG_MSG("dot1xInit: msgQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (dot1xStartTasks() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (dot1xIssueCmd(dot1xCnfgr, L7_NULL, pCmdData) != L7_SUCCESS)
    L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID, "dot1xInit: Failed calling dot1xIssueCmd. "
															"802.1X message queue is full\n");

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for dot1x component
*
* @param    none
*                            
* @comments  none
*
* @end
*********************************************************************/
void dot1xInitUndo()
{
  if (dot1xQueue != L7_NULLPTR)
    osapiMsgQueueDelete(dot1xQueue);


  if (dot1xTaskId != L7_ERROR)
    osapiTaskDelete(dot1xTaskId);

  dot1xCnfgrState = DOT1X_PHASE_INIT_0;
}

/*********************************************************************
*
* @purpose  Determine memory to be allocated for logical ports depending 
*           on if mac based mode is supported.
*
* @param    max_users (@b output) maximum users supported on the current platform 
*                                 for the current product  
*                            
* @comments  none
*
* @end
*********************************************************************/
static L7_uint32 dot1xMaxAllocMemGet()
{
  L7_uint32 max_node=0; 

  if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)== L7_TRUE) 
  {
      max_node = L7_DOT1X_MAX_USERS;
  }
  else
  {
      max_node = L7_MAX_PORT_COUNT;
  }

  return max_node;
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
* @comments  The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @commets  The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t dot1xCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dot1xRC,rc;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  dot1xRC     = L7_SUCCESS;


  dot1xCfg        = osapiMalloc(L7_DOT1X_COMPONENT_ID, sizeof(dot1xCfg_t));
  dot1xPortInfo   = osapiMalloc(L7_DOT1X_COMPONENT_ID, sizeof(dot1xPortInfo_t) * platIntfMaxCountGet());
  dot1xSupplicantPortInfo  = osapiMalloc(L7_DOT1X_COMPONENT_ID, 
                              sizeof(dot1xSupplicantPortInfo_t) * platIntfMaxCountGet());
  
  rc = dot1xLogicalPortInfoDBInit(dot1xMaxAllocMemGet());
  dot1xPortSessionStats = osapiMalloc(L7_DOT1X_COMPONENT_ID, 
                                     (sizeof(dot1xPortSessionStats_t) * (platIntfMaxCountGet() + 1)));
  dot1xPortStats  = osapiMalloc(L7_DOT1X_COMPONENT_ID, sizeof(dot1xPortStats_t) * platIntfMaxCountGet());
  dot1xMapTbl     = osapiMalloc(L7_DOT1X_COMPONENT_ID, sizeof(L7_uint32) * platIntfMaxCountGet());
  dot1xVlanIntfCount = (L7_ushort16 *)osapiMalloc(L7_DOT1X_COMPONENT_ID, 
                                (L7_uint32)(sizeof(L7_ushort16) * (platVlanVlanIdMaxGet()+1)));
  dot1xGuestVlanIntfCount = (L7_ushort16 *)osapiMalloc(L7_DOT1X_COMPONENT_ID, 
                                (L7_uint32)(sizeof(L7_ushort16) * (platVlanVlanIdMaxGet()+1)));

  /* initialize Mac address database*/
  
  rc = dot1xMacAddrInfoDBInit(dot1xMaxAllocMemGet());


  if ((dot1xCfg       == L7_NULLPTR) ||
      (dot1xPortInfo  == L7_NULLPTR) ||
      (dot1xSupplicantPortInfo  == L7_NULLPTR) ||
      (rc == L7_FAILURE) ||
      (dot1xPortStats == L7_NULLPTR) ||
      (dot1xMapTbl    == L7_NULLPTR) ||
      (dot1xVlanIntfCount == L7_NULLPTR) ||
      (dot1xGuestVlanIntfCount == L7_NULLPTR) ||
      (dot1xPortSessionStats == L7_NULLPTR) )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    dot1xRC   = L7_ERROR;

    return dot1xRC;
  }

  /* Zero bitmasks to indicate no interfaces are enabled */
  memset((void *)dot1xCfg, 0, sizeof(dot1xCfg_t));
  memset((void *)dot1xPortInfo, 0, sizeof(dot1xPortInfo_t) * platIntfMaxCountGet());
  memset((void *)dot1xSupplicantPortInfo, 0, sizeof(dot1xSupplicantPortInfo_t) * platIntfMaxCountGet());
  memset((void *)dot1xPortStats, 0, sizeof(dot1xPortStats_t) * platIntfMaxCountGet());
  memset((void *)dot1xMapTbl, 0, sizeof(L7_uint32) * platIntfMaxCountGet());
  memset((void *)dot1xVlanIntfCount, 0, (sizeof(L7_ushort16) * (platVlanVlanIdMaxGet()+1)));
  memset((void *)dot1xGuestVlanIntfCount, 0, (sizeof(L7_ushort16) * (platVlanVlanIdMaxGet()+1)));
  memset((void *)&dot1xVlanMask, 0, sizeof(L7_VLAN_MASK_t));
  memset((void *)&dot1xGuestVlanMask, 0, sizeof(L7_VLAN_MASK_t));
  memset((void *)dot1xPortSessionStats , 0 ,sizeof(dot1xPortSessionStats_t) * (L7_MAX_INTERFACE_COUNT + 1));

  if (dot1xOutcallsSysInit() != L7_SUCCESS)
  {
      LOG_MSG("Failed to initialize dot1x outcalls\n");
      return(L7_FAILURE);
  }
  
  /* Trace create for debugging. This trace has 256 entries, each
   * entry is 16 byte long. Actual user data available is 15
   * bytes as the first byte always records the trace id
   */

  if (traceBlockCreate( DOT1X_TRACE_ENTRY_MAX, DOT1X_TRACE_ENTRY_SIZE_MAX,
              (L7_uchar8 *)"dot1xTrace", &dot1xInfo.traceId) != L7_SUCCESS)
  {
    LOG_MSG("Unable to create dot1xTrace block\n");
  }

  /* enable tracing */
  if (traceBlockStart(dot1xInfo.traceId) != L7_SUCCESS)
    LOG_MSG("Failed to enabling dot1x tracing\n");

  /* initilize debug */
  if (dot1xDebugInit() != L7_SUCCESS)
    LOG_MSG("Unable to initialize dot1x debug\n");

  /* register function with sysnet to receive PDUs */
  if (dot1xTxRxInit() != L7_SUCCESS)
  {
      LOG_MSG("Failure in dot1xTxRxInit\n");
      return L7_FAILURE;
  }

  /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
  dot1xDebugCfgRead();
  dot1xApplyDebugConfigData();

  dot1xCnfgrState = DOT1X_PHASE_INIT_1;

  return dot1xRC;
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
* @comments  The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments  The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t dot1xCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dot1xRC;
  nvStoreFunctionList_t dot1xNotifyFunctionList;
  L7_uint32 eventMask = 0;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  dot1xRC     = L7_SUCCESS;


  memset((void *) &dot1xNotifyFunctionList, 0, sizeof(dot1xNotifyFunctionList));
  dot1xNotifyFunctionList.registrar_ID     = L7_DOT1X_COMPONENT_ID;
  dot1xNotifyFunctionList.notifySave       = dot1xSave;
  dot1xNotifyFunctionList.hasDataChanged   = dot1xHasDataChanged;
  dot1xNotifyFunctionList.notifyConfigDump = dot1xCfgDump;
  dot1xNotifyFunctionList.resetDataChanged = dot1xResetDataChanged;

  if (nvStoreRegister(dot1xNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1xRC     = L7_ERROR;

    return dot1xRC;

  }

  if (nimRegisterIntfChange(L7_DOT1X_COMPONENT_ID, dot1xIntfChangeCallback) != L7_SUCCESS)
  {
    LOG_MSG("dot1x: Unable to register with NIM\n");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1xRC     = L7_ERROR;

    return dot1xRC;
  }

  if (radiusResponseRegister(L7_DOT1X_COMPONENT_ID, dot1xRadiusResponseCallback) != L7_SUCCESS)
  {
    LOG_MSG("dot1x: Unable to register for RADIUS response callback\n");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1xRC     = L7_ERROR;

    return dot1xRC;
  }
  eventMask = VLAN_DELETE_PENDING_NOTIFY | VLAN_ADD_NOTIFY | VLAN_PVID_CHANGE_NOTIFY | VLAN_ADD_PORT_NOTIFY | VLAN_DELETE_PORT_NOTIFY;
  /*register with vlan to get notifications for vlan create/delete events */
  if(vlanRegisterForChange(dot1xVlanChangeCallback, L7_DOT1X_COMPONENT_ID, eventMask) != L7_SUCCESS)
  {
    LOG_MSG("dot1x: Unable to register for Vlan callback\n");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1xRC     = L7_ERROR;

    return dot1xRC;
  }

  if (tlvRegister(65536, L7_DOT1X_COMPONENT_ID,
                  (L7_uchar8*)"DOT1x", &dot1xTlvHandle) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    dot1xRC      = L7_ERROR;

    return dot1xRC;
  }
  
  /* register for debug*/
  dot1xDebugRegister();
  dot1xCnfgrState = DOT1X_PHASE_INIT_2;

  return dot1xRC;
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
* @comments  The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments  The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t dot1xCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dot1xRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  dot1xRC     = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_DOT1X_COMPONENT_ID, DOT1X_CFG_FILENAME,
                   (L7_char8 *)dot1xCfg, (L7_uint32)sizeof(dot1xCfg_t),
                   &dot1xCfg->checkSum, DOT1X_CFG_VER_CURRENT,
                   dot1xBuildDefaultConfigData, dot1xMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1xRC     = L7_ERROR;

    return dot1xRC;
  }
  dot1xCnfgrState = DOT1X_PHASE_INIT_3;
  if (dot1xCtlApplyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1xRC     = L7_ERROR;

    return dot1xRC;
  }
  return dot1xRC;
}


/*********************************************************************
* @purpose  This function undoes dot1xCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @comments  none
*
* @end
*********************************************************************/
void dot1xCnfgrFiniPhase1Process()
{

  if (dot1xCfg != L7_NULLPTR)
  {
    osapiFree(L7_DOT1X_COMPONENT_ID, dot1xCfg);
    dot1xCfg = L7_NULLPTR;
  }

  if (dot1xPortInfo != L7_NULLPTR)
  {
    osapiFree(L7_DOT1X_COMPONENT_ID, dot1xPortInfo);
    dot1xPortInfo = L7_NULLPTR;
  }

  if (dot1xSupplicantPortInfo != L7_NULLPTR)
  {
    osapiFree(L7_DOT1X_COMPONENT_ID, dot1xSupplicantPortInfo);
    dot1xSupplicantPortInfo = L7_NULLPTR;
  }

  dot1xLogicalPortInfoDBDeInit();

  if (dot1xPortStats != L7_NULLPTR)
  {
    osapiFree(L7_DOT1X_COMPONENT_ID, dot1xPortStats);
    dot1xPortStats = L7_NULLPTR;
  }

  if (dot1xMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_DOT1X_COMPONENT_ID, dot1xMapTbl);
    dot1xMapTbl = L7_NULLPTR;
  }

  dot1xMacAddrInfoDBDeInit();

  dot1xInitUndo();

  dot1xCnfgrState = DOT1X_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes dot1xCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @comments  none
*
* @end
*********************************************************************/
void dot1xCnfgrFiniPhase2Process()
{
  (void)radiusResponseDeregister(L7_DOT1X_COMPONENT_ID);
  (void)nimDeRegisterIntfChange(L7_DOT1X_COMPONENT_ID);
  (void)nvStoreDeregister(L7_DOT1X_COMPONENT_ID);
  (void)vlanDeregisterForChange(L7_DOT1X_COMPONENT_ID);
  (void)tlvUnregister(dot1xTlvHandle);
  dot1xTlvHandle = L7_NULL;

  dot1xCnfgrState = DOT1X_PHASE_INIT_1;

}

/*********************************************************************
* @purpose  This function undoes dot1xCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @comments  none
*
* @end
*********************************************************************/
void dot1xCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place dot1xCnfgrState to WMU */
  dot1xCnfgrUconfigPhase2(&response, &reason);

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
* @comments  The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments  The following are valid error reason code:
*           None.
*
* @end
*********************************************************************/
L7_RC_t dot1xCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dot1xRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(dot1xRC);
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
* @comments  The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments  The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @end
*********************************************************************/

L7_RC_t dot1xCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dot1xRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  dot1xRC     = L7_SUCCESS;


  if (dtlDot1xModeSet(L7_DISABLE) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1xRC     = L7_ERROR;

    return dot1xRC;
  }

  memset(dot1xCfg, 0, sizeof(dot1xCfg_t));

  dot1xCnfgrState = DOT1X_PHASE_WMU;

  return dot1xRC;
}


/*********************************************************************
*
* @purpose  To parse the configurator commands send to dot1xTask
*
* @param    none
*                            
* @returns  
*
* @comments  none
*
* @end
*********************************************************************/
void dot1xCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             dot1xRC = L7_ERROR;
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
                if ((dot1xRC = dot1xCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  dot1xCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((dot1xRC = dot1xCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  dot1xCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((dot1xRC = dot1xCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  dot1xCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                dot1xRC = dot1xCnfgrNoopProccess( &response, &reason );
                dot1xCnfgrState = DOT1X_PHASE_WMU;
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
                dot1xCnfgrState = DOT1X_PHASE_EXECUTE;

                dot1xRC  = L7_SUCCESS;
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
                dot1xRC = dot1xCnfgrNoopProccess( &response, &reason );
                dot1xCnfgrState = DOT1X_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                dot1xRC = dot1xCnfgrUconfigPhase2( &response, &reason );
                dot1xCnfgrState = DOT1X_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            dot1xRC = dot1xCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = dot1xRC;
  if (dot1xRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);


  return;
}

