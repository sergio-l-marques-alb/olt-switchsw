/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename dos_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component DOSCONTROL
*
* @comments
*
* @create 04/04/2005
*
* @author esmiley
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "dos.h"
#include "dos_api.h"
#include "dos_util.h"

extern osapiRWLock_t       doSCfgRWLock;
extern doSCfgData_t  *doSCfgData;
doSCnfgrState_t doSCnfgrState = DOS_PHASE_INIT_0;
extern void *dosQueue;
extern L7_int32                   dosTaskId;
extern dosIntfInfo_t              *dosIntfInfo;
extern L7_uint32                  *dosMapTbl;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for DoS component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the DoS comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void doSApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             doSRC = L7_ERROR;
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
                if ((doSRC = doSCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  doSCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((doSRC = doSCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  doSCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((doSRC = doSCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  doSCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                doSRC = doSCnfgrNoopProcess( &response, &reason );
                doSCnfgrState = DOS_PHASE_WMU;
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
                doSCnfgrState = DOS_PHASE_EXECUTE;

                doSRC  = L7_SUCCESS;
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
                doSRC = doSCnfgrNoopProcess( &response, &reason );
                doSCnfgrState = DOS_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                doSRC = doSCnfgrUconfigPhase2( &response, &reason );
                doSCnfgrState = DOS_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            doSRC = doSCnfgrNoopProcess( &response, &reason );
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
  cbData.asyncResponse.rc = doSRC;
  if (doSRC == L7_SUCCESS)
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
L7_RC_t doSCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t doSRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  doSRC   = L7_SUCCESS;


  /* Allocate memory for global data */
  doSCfgData = osapiMalloc(L7_DOSCONTROL_COMPONENT_ID, sizeof(doSCfgData_t));

  /* Make sure that allocation succeded */
  if (doSCfgData == L7_NULL)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    doSRC   = L7_ERROR;

    return doSRC;
  }

  memset(doSCfgData, 0, sizeof(doSCfgData_t));

  if (osapiRWLockCreate(&doSCfgRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
    LOG_MSG("doSCnfgrInitPhase1Process: Unable to create r/w lock for DOSCONTROL\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    doSRC = L7_ERROR;

    return doSRC;
  }

  dosQueue = (void *)osapiMsgQueueCreate(DOS_QUEUE, DOS_MSG_COUNT, DOS_MSG_SIZE);
  if (dosQueue == L7_NULLPTR)
  {
    LOG_MSG("doSCnfgrInitPhase1Process: msgQueue creation error.\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    doSRC = L7_ERROR;

    return doSRC;
  }
  if (dosStartTask() != L7_SUCCESS)
  {
    LOG_MSG("doSCnfgrInitPhase1Process: Task could not be started.\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    doSRC = L7_ERROR;

    return doSRC;
  }

  dosIntfInfo  = osapiMalloc(L7_DOSCONTROL_COMPONENT_ID, sizeof(dosIntfInfo_t) * ( platIntfPhysicalIntfMaxCountGet()+ 1 ) );

  if(dosIntfInfo == L7_NULLPTR)
  {
    LOG_MSG("doSCnfgrInitPhase1Process: could not allocate memory to dosIntfInfo.\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    doSRC   = L7_ERROR;

    return doSRC;
  }
  memset((void *)dosIntfInfo, 0, sizeof(dosIntfInfo_t) * (platIntfPhysicalIntfMaxCountGet() + 1) );

  dosMapTbl     = osapiMalloc(L7_DOSCONTROL_COMPONENT_ID, sizeof(L7_uint32) * (platIntfPhysicalIntfMaxCountGet() + 1));
  if(dosMapTbl == L7_NULLPTR)
  {
    LOG_MSG("doSCnfgrInitPhase1Process: could not allocate memory to dosMapTbl.\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    doSRC   = L7_ERROR;

    return doSRC;
  }
  memset((void *)dosMapTbl, 0, sizeof(L7_uint32) * (platIntfPhysicalIntfMaxCountGet() + 1));

  doSCnfgrState = DOS_PHASE_INIT_1;

  return doSRC;

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
L7_RC_t doSCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t doSRC;
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  doSRC   = L7_SUCCESS;

  memset(&notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_DOSCONTROL_COMPONENT_ID;
  notifyFunctionList.notifySave     = doSSave;
  notifyFunctionList.hasDataChanged = doSHasDataChanged;
  notifyFunctionList.resetDataChanged = doSResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    doSRC   = L7_ERROR;

    return doSRC;
  }

  if (nimRegisterIntfChange(L7_DOSCONTROL_COMPONENT_ID, dosIntfChangeCallback) != L7_SUCCESS)
  {
    LOG_MSG("L7_DOSCONTROL_COMPONENT_ID: Unable to register with NIM\n");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    doSRC     = L7_ERROR;

    return doSRC;
  }

  doSCnfgrState = DOS_PHASE_INIT_2;

  return doSRC;
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
L7_RC_t doSCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t doSRC;


  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  doSRC   = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_DOSCONTROL_COMPONENT_ID, DOSCONTROL_CFG_FILENAME,
                       (L7_char8 *)doSCfgData, sizeof(doSCfgData_t),
                       &doSCfgData->checkSum, DOSCONTROL_CFG_VER_CURRENT,
                       doSBuildDefaultConfigData, doSMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    doSRC      = L7_ERROR;

    return doSRC;
  }

  if (doSApplyConfigData() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    doSRC      = L7_ERROR;

    return doSRC;
  }

  doSCfgData->cfgHdr.dataChanged = L7_FALSE;

  doSCnfgrState = DOS_PHASE_INIT_3;

  return doSRC;
}


/*********************************************************************
* @purpose  This function undoes doSCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void doSCnfgrFiniPhase1Process()
{
  /* Deallocate anything that was allocated */

  if (doSCfgData != L7_NULL)
  {
    osapiFree(L7_DOSCONTROL_COMPONENT_ID, doSCfgData);
    doSCfgData = L7_NULL;
  }

  if (dosIntfInfo != L7_NULLPTR)
  {
    osapiFree(L7_DOSCONTROL_COMPONENT_ID, dosIntfInfo);
    dosIntfInfo = L7_NULLPTR;
  }

  if (dosQueue != L7_NULLPTR)
    osapiMsgQueueDelete(dosQueue);

  if (dosTaskId != L7_ERROR)
    osapiTaskDelete(dosTaskId);

  if (dosMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_DOSCONTROL_COMPONENT_ID, dosMapTbl);
    dosMapTbl = L7_NULLPTR;
  }

  doSCnfgrState = DOS_PHASE_INIT_0;
}


/*********************************************************************
* @purpose  This function undoes doSCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void doSCnfgrFiniPhase2Process()
{

  /*
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now
   * a temporary solution is set in place, where in the registered
   * function if we can execute the callback only if it's corresponding
   * member in the doSDeregister is set to L7_FALSE;
   */

  (void)nimDeRegisterIntfChange(L7_DOSCONTROL_COMPONENT_ID);
  nvStoreDeregister(L7_DOSCONTROL_COMPONENT_ID);
  doSCnfgrState = DOS_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes doSCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void doSCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place doSCnfgrState to WMU */
  doSCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t doSCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t doSRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(doSRC);
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

L7_RC_t doSCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason )
{

  L7_RC_t doSRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  doSRC    = L7_SUCCESS;

  /* Clear out the configuration */
  memset(doSCfgData, 0, sizeof(doSCfgData_t));

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_SIPDIP_FEATURE_ID) == L7_TRUE)
  {
      if (doSSIPDIPSet(FD_DOS_DEFAULT_SIPDIP_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_SMACDMAC_FEATURE_ID) == L7_TRUE)
  {
      if (doSSMACDMACSet(FD_DOS_DEFAULT_SMACDMAC_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID) == L7_TRUE)
  {
      if (doSFirstFragModeSet(FD_DOS_DEFAULT_FIRSTFRAG_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }

      if (doSFirstFragSizeSet(FD_DOS_DEFAULT_MINTCPHDR_SIZE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPFRAG_FEATURE_ID) == L7_TRUE)
  {
      if (doSTCPFragSet(FD_DOS_DEFAULT_TCPFRAG_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPFLAG_FEATURE_ID) == L7_TRUE)
  {
      if (doSTCPFlagSet(FD_DOS_DEFAULT_TCPFLAG_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID) == L7_TRUE)
  {
      if (doSTCPFlagSeqSet(FD_DOS_DEFAULT_TCPFLAGSEQ_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_L4PORT_FEATURE_ID) == L7_TRUE)
  {
      if (doSL4PortSet(FD_DOS_DEFAULT_L4PORT_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
  {
      if (doSICMPModeSet(FD_DOS_DEFAULT_ICMP_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }

      if (doSICMPSizeSet(FD_DOS_DEFAULT_MAXICMP_SIZE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_ICMPV4_FEATURE_ID) == L7_TRUE)
  {
      if (doSICMPModeSet(FD_DOS_DEFAULT_ICMP_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }

      if (doSICMPSizeSet(FD_DOS_DEFAULT_MAXICMP_SIZE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_ICMPV6_FEATURE_ID) == L7_TRUE)
  {
      if (doSICMPModeSet(FD_DOS_DEFAULT_ICMP_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }

      if (doSICMPv6SizeSet(FD_DOS_DEFAULT_MAXICMP_SIZE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_ICMPFRAG_FEATURE_ID) == L7_TRUE)
  {
      if (doSICMPFragModeSet(FD_DOS_DEFAULT_ICMPFRAG_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  } 

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPFINURGPSH_FEATURE_ID) == L7_TRUE)
  {
      if (doSTCPFinUrgPshSet(FD_DOS_DEFAULT_TCPFINURGPSH_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  } 

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPOFFSET_FEATURE_ID) == L7_TRUE)
  {
      if (doSTCPOffsetSet(FD_DOS_DEFAULT_TCPOFFSET_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  } 

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPSYN_FEATURE_ID) == L7_TRUE)
  {
      if (doSTCPSynSet(FD_DOS_DEFAULT_TCPSYN_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPSYNFIN_FEATURE_ID) == L7_TRUE)
  {
      if (doSTCPSynFinSet(FD_DOS_DEFAULT_TCPSYNFIN_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_TCPPORT_FEATURE_ID) == L7_TRUE)
  {
      if (doSTCPPortSet(FD_DOS_DEFAULT_TCPPORT_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }

  if (cnfgrIsFeaturePresent(L7_DOSCONTROL_COMPONENT_ID,
                            L7_DOSCONTROL_UDPPORT_FEATURE_ID) == L7_TRUE)
  {
      if (doSUDPPortSet(FD_DOS_DEFAULT_UDPPORT_MODE) != L7_SUCCESS) 
      {
          *pResponse = 0;
          *pReason   = L7_CNFGR_ERR_RC_FATAL;
          doSRC   = L7_ERROR;
      }
  }        
          
  doSCnfgrState = DOS_PHASE_WMU;

  return doSRC;
}
             

