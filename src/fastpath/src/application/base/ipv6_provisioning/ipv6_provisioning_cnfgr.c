/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename ipv6_provisioning_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component
*
* @comments
*
* @create 10/21/2003
*
* @author mfiorito
*
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "nimapi.h"
#include "sysnet_api.h"
#include "ipv6_provisioning_config.h"
#include "ipv6_provisioning_api.h"
#include "ipv6_provisioning_debug.h"

ipv6ProvCnfgrState_t ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_INIT_0;

extern ipv6ProvCfgData_t *ipv6ProvCfgData;
extern L7_uint32 *ipv6ProvMapTbl;
extern L7_uint32 numIPv6Pkts;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for IPv6 Provisioning component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the IPv6 Provisioning comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void ipv6ProvApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             ipv6ProvRC = L7_ERROR;
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
                if ((ipv6ProvRC = ipv6ProvCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  ipv6ProvCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((ipv6ProvRC = ipv6ProvCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  ipv6ProvCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((ipv6ProvRC = ipv6ProvCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  ipv6ProvCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                ipv6ProvRC = ipv6ProvCnfgrNoopProcess( &response, &reason );
                ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_WMU;
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
                ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_EXECUTE;

                ipv6ProvRC  = L7_SUCCESS;
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
                ipv6ProvRC = ipv6ProvCnfgrNoopProcess( &response, &reason );
                ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                ipv6ProvRC = ipv6ProvCnfgrUconfigPhase2( &response, &reason );
                ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            ipv6ProvRC = ipv6ProvCnfgrNoopProcess( &response, &reason );
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
  cbData.asyncResponse.rc = ipv6ProvRC;
  if (ipv6ProvRC == L7_SUCCESS)
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
L7_RC_t ipv6ProvCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ipv6ProvRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  ipv6ProvRC = L7_SUCCESS;

  ipv6ProvCfgData = osapiMalloc(L7_IPV6_PROVISIONING_COMPONENT_ID, sizeof(ipv6ProvCfgData_t));
  ipv6ProvMapTbl  = osapiMalloc(L7_IPV6_PROVISIONING_COMPONENT_ID, sizeof(L7_uint32) * platIntfMaxCountGet());

  /* Make sure that allocation succeded */
  if ((ipv6ProvCfgData   == L7_NULLPTR) ||
      (ipv6ProvMapTbl    == L7_NULLPTR))
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ipv6ProvRC = L7_ERROR;

    return ipv6ProvRC;
  }

  memset(ipv6ProvCfgData, 0, sizeof(ipv6ProvCfgData_t));
  memset(ipv6ProvMapTbl, 0, sizeof(L7_uint32) * platIntfMaxCountGet());

  ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_INIT_1;

  return ipv6ProvRC;

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
L7_RC_t ipv6ProvCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ipv6ProvRC;
  nvStoreFunctionList_t notifyFunctionList;
  sysnetNotifyEntry_t snEntry;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  ipv6ProvRC = L7_SUCCESS;

  memset(&notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID     = L7_IPV6_PROVISIONING_COMPONENT_ID;
  notifyFunctionList.notifySave       = ipv6ProvSave;
  notifyFunctionList.hasDataChanged   = ipv6ProvHasDataChanged;
  notifyFunctionList.notifyConfigDump = ipv6ProvCfgDump;
  notifyFunctionList.resetDataChanged = ipv6ProvResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipv6ProvRC = L7_ERROR;

    return ipv6ProvRC;
  }

  /* register NIM callback to support interface changes */
  if ((ipv6ProvRC = nimRegisterIntfChange(L7_IPV6_PROVISIONING_COMPONENT_ID,
                                          ipv6ProvIntfChangeCallback)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IPV6_PROVISIONING_COMPONENT_ID,
            "ipv6ProvCnfgrInitPhase2Process: Failed nim registration for Interface Change Callback\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipv6ProvRC = L7_ERROR;

    return ipv6ProvRC;
  }

  LOG_INFO(LOG_CTX_STARTUP,"Going to register ipv6ProvRecvPkt related to type=%u, protocol_type=%u: 0x%08x",
           SYSNET_ETHERTYPE_ENTRY, L7_ETYPE_IPV6, (L7_uint32) ipv6ProvRecvPkt);

  strcpy(snEntry.funcName, "ipv6ProvRecvPkt");
  snEntry.notify_pdu_receive = ipv6ProvRecvPkt;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_IPV6;
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IPV6_PROVISIONING_COMPONENT_ID,
            "ipv6ProvCnfgrInitPhase2Process: Failure registering for IPv6 packets\n");

    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipv6ProvRC = L7_ERROR;

    return ipv6ProvRC;
  }

  ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_INIT_2;

  return ipv6ProvRC;
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
L7_RC_t ipv6ProvCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ipv6ProvRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  ipv6ProvRC = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_IPV6_PROVISIONING_COMPONENT_ID, IPV6_PROVISIONING_CFG_FILENAME,
                       (L7_char8 *)ipv6ProvCfgData, sizeof(ipv6ProvCfgData_t),
                       &ipv6ProvCfgData->checkSum, IPV6_PROVISIONING_CFG_VER_CURRENT,
                       ipv6ProvBuildDefaultConfigData, ipv6ProvMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipv6ProvRC = L7_ERROR;

    return ipv6ProvRC;
  }

  if (ipv6ProvApplyConfigData() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    ipv6ProvRC = L7_ERROR;

    return ipv6ProvRC;
  }

  /* Initialize global data */
  numIPv6Pkts = 0;

  ipv6ProvDebugInit();

  ipv6ProvCfgData->cfgHdr.dataChanged = L7_FALSE;

  ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_INIT_3;

  return ipv6ProvRC;
}


/*********************************************************************
* @purpose  This function undoes ipv6ProvCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipv6ProvCnfgrFiniPhase1Process()
{
  /* Deallocate anything that was allocated */
  if (ipv6ProvMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_IPV6_PROVISIONING_COMPONENT_ID, ipv6ProvMapTbl);
    ipv6ProvMapTbl = L7_NULLPTR;
  }

  if (ipv6ProvCfgData != L7_NULLPTR)
  {
    osapiFree(L7_IPV6_PROVISIONING_COMPONENT_ID, ipv6ProvCfgData);
    ipv6ProvCfgData = L7_NULLPTR;
  }

  ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_INIT_0;
}


/*********************************************************************
* @purpose  This function undoes ipv6ProvCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipv6ProvCnfgrFiniPhase2Process()
{
  sysnetNotifyEntry_t snEntry;

  strcpy(snEntry.funcName, "ipv6ProvRecvPkt");
  snEntry.notify_pdu_receive = ipv6ProvRecvPkt;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_IPV6;
  (void)sysNetDeregisterPduReceive(&snEntry);

  (void)nimDeRegisterIntfChange(L7_IPV6_PROVISIONING_COMPONENT_ID);
  (void)nvStoreDeregister(L7_IPV6_PROVISIONING_COMPONENT_ID);

  ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes ipv6ProvCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipv6ProvCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place ipv6ProvCnfgrState to WMU */
  ipv6ProvCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t ipv6ProvCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t ipv6ProvRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(ipv6ProvRC);
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
L7_RC_t ipv6ProvCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_uint32 cfgIndex, intIfNum, dstIfNum;
  L7_RC_t ipv6ProvRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  ipv6ProvRC  = L7_SUCCESS;

  for (cfgIndex = 1; cfgIndex < L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT; cfgIndex++)
  {
    if (nimIntIfFromConfigIDGet(&(ipv6ProvCfgData->ipv6PortMappings[cfgIndex].configId),
                                &intIfNum) != L7_SUCCESS)
      continue;

    if (ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6Mode == L7_ENABLE)
    {
      if (nimIntIfFromConfigIDGet(&(ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6DstId),
                                  &dstIfNum) == L7_SUCCESS)
      {
        (void)ipv6ProvBridgeSrcIfNumDstIfNum(intIfNum, dstIfNum, L7_DISABLE,
                                             ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6copyToCpu);
      }
    }
  }

  ipv6ProvCnfgrState = IPV6_PROVISIONING_PHASE_WMU;

  return ipv6ProvRC;
}


