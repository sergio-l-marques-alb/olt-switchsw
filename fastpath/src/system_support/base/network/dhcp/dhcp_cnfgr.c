/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
* @filename  dhcp_cnfgr.c
*
* @purpose   dhcp configurator API
*
* @component DHCP client
*
* @comments
*
* @create    1/02/2008
*
* @author    ipopov
*
* @end
*
**********************************************************************/

#include "defaultconfig.h"
#include "l3_commdefs.h"
#include "dhcp_cnfgr.h"
#include "l7_ip_api.h"
#include "osapi.h"
#include "l7_dhcp.h"
#include "string.h"
#include "dhcp_bootp_api.h"
#include "dhcp_map.h"
#include "dhcp_client_api.h"
#include "usmdb_util_api.h"
#include "dhcp_debug.h"

dhcpClientCB_t dhcpClientCB;

extern void *servPortSema;
extern void *networkPortSema;

/* L7_TRUE if Configurator reported last restart to be warm */
extern L7_BOOL dhcpcWarmRestart;


/*********************************************************************
 * @purpose  Process configurator commands.
 *
 * @param    pCmdData  - @b{(input)}  command to be processed
 *
 * @returns  None
 *
 * @notes    This function completes synchronously. The return value
 *           is presented to the configurator by calling the cnfgrApiCallback().
 *           The following are the possible return codes:
 *           L7_SUCCESS - There were no errors. Response is available.
 *           L7_ERROR   - There were errors. Reason code is available.
 *
 * @notes    This function runs in the configurator's thread.
 *
 * @end
 *********************************************************************/
void dhcpClientApiCnfgrCommand (L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t rc = L7_ERROR;
  L7_CNFGR_ERR_RC_t reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

  if (pCmdData == L7_NULL)
  {
    cbData.correlator               = L7_NULL;
    cbData.asyncResponse.rc         = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
    return;
  }

  if (pCmdData->type != L7_CNFGR_RQST)
  {
    cbData.correlator               = L7_NULL;
    cbData.asyncResponse.rc         = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
    cnfgrApiCallback(&cbData);
    return;
  }

  command = pCmdData->command;
  request = pCmdData->u.rqstData.request;
  correlator = pCmdData->correlator;

  if ((request <= L7_CNFGR_RQST_FIRST) || (request >= L7_CNFGR_RQST_LAST))
  {
    cbData.correlator               = L7_NULL;
    cbData.asyncResponse.rc         = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
    cnfgrApiCallback(&cbData);
    return;
  }

  switch (command)
  {
    case L7_CNFGR_CMD_INITIALIZE:
      switch (request)
      {
        case L7_CNFGR_RQST_I_PHASE1_START:
          rc = dhcpCnfgrInitPhase1Process(&response, &reason);
          break;
        case L7_CNFGR_RQST_I_PHASE2_START:
          rc = dhcpCnfgrInitPhase2Process(&response, &reason);
          break;
        case L7_CNFGR_RQST_I_PHASE3_START:
          dhcpcWarmRestart = (pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM);
          rc = dhcpCnfgrInitPhase3Process(&response, &reason);
          break;
        case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
          rc = dhcpCnfgrNoopProccess(&response, &reason);
          break;
        default:
          /* invalid command/request pair */
          break;
      }
      break;

    case L7_CNFGR_CMD_UNCONFIGURE:
      switch (pCmdData->u.rqstData.request)
      {
        case L7_CNFGR_RQST_U_PHASE1_START:
          rc = dhcpCnfgrUnconfigPhase1(&response, &reason);
          break;

        case L7_CNFGR_RQST_U_PHASE2_START:
          rc = dhcpCnfgrUnconfigPhase2(&response, &reason);
          break;

        default:
          rc = L7_ERROR;
          reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
          break;
      }
      break;

    case L7_CNFGR_CMD_EXECUTE:
    case L7_CNFGR_CMD_TERMINATE:
    case L7_CNFGR_CMD_SUSPEND:
    case L7_CNFGR_CMD_RESUME:
      rc = dhcpCnfgrNoopProccess(&response, &reason);
      break;

    default:
      reason = L7_CNFGR_ERR_RC_INVALID_CMD;
      break;
  }

  cbData.correlator = correlator;
  cbData.asyncResponse.rc = rc;
  if (rc == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason = reason;

  cnfgrApiCallback(&cbData);

  return;
}

/*********************************************************************
 * @purpose  Phase 1 initialization.
 *
 * @param    response - @b{(output)}  Response if L7_SUCCESS
 * @param    reason   - @b{(output)}  Reason if L7_ERROR
 *
 * @returns  L7_SUCCESS if all goes well
 *           L7_ERROR if there is a problem
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *response,
                                   L7_CNFGR_ERR_RC_t *reason)
{
  L7_uint32 size = 0;
  L7_uint32 index = 0;
  L7_uint32 unit = 0;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  memset (&dhcpClientCB, 0, sizeof(dhcpClientCB_t));

  /* DHCP Client Notify List Memory */
  size = sizeof (dhcpNotifyList_t) * L7_LAST_COMPONENT_ID;
  if ((dhcpClientCB.dhcpNotifyList = (dhcpNotifyList_t*)
                             osapiMalloc (L7_DHCP_CLIENT_COMPONENT_ID, size))
                          == L7_NULLPTR)
  {
    *response = 0;
    *reason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Notify List Malloc Failed");
    return L7_FAILURE;
  }
  memset (dhcpClientCB.dhcpNotifyList, 0, size);

  /* DHCP Client Notify List Semaphore */
  if ((dhcpClientCB.notifyListSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO,
                                                      OSAPI_SEM_FULL))
                          == L7_NULLPTR)
  {
    *response = 0;
    *reason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Notify List Sem Create Failed");
    return L7_FAILURE;
  }

  /* DHCP Client per Interface Information Memory */
  /* If Routing is present initialize upto 128 */
  /* If Network port is present initialize 129 */
  /* If Service port is present initialize 130 */

  /* Allocate Memory for Router Interfaces */
#ifdef L7_ROUTING_PACKAGE
  for (index = 0; index < DHCP_CLIENT_MAX_RTR_INTERFACES; index++)
  {
    if ((dhcpClientCB.clientIntfInfo[index] = (dhcpClientInfo_t*)
                               osapiMalloc (L7_DHCP_CLIENT_COMPONENT_ID,
                                            sizeof(dhcpClientInfo_t)))
                            == L7_NULLPTR)
    {
      *response = 0;
      *reason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Malloc Failed");
      return L7_FAILURE;
    }
    memset (dhcpClientCB.clientIntfInfo[index], 0, sizeof(dhcpClientInfo_t));
  }
#endif

  /* Allocate Memory for Network Port */
  if (usmDbFeaturePresentCheck (unit, L7_SIM_COMPONENT_ID,
                                L7_SIM_NETWORK_PORT_FEATURE_ID)
                             == L7_TRUE)
  {
    index = DHCP_CLIENT_NETWORK_PORT_INTF_NUM-1;
    if ((dhcpClientCB.clientIntfInfo[index] = (dhcpClientInfo_t*)
                               osapiMalloc (L7_DHCP_CLIENT_COMPONENT_ID,
                                            sizeof(dhcpClientInfo_t)))
                            == L7_NULLPTR)
    {
      *response = 0;
      *reason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Malloc Failed");
      return L7_FAILURE;
    }
    memset (dhcpClientCB.clientIntfInfo[index], 0, sizeof(dhcpClientInfo_t));
  }

  /* Allocate Memory for Service Port */
  if (cnfgrIsComponentPresent (L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE)
  {
    index = DHCP_CLIENT_SERVICE_PORT_INTF_NUM-1;
    if ((dhcpClientCB.clientIntfInfo[index] = (dhcpClientInfo_t*)
                               osapiMalloc (L7_DHCP_CLIENT_COMPONENT_ID,
                                            sizeof(dhcpClientInfo_t)))
                            == L7_NULLPTR)
    {
      *response = 0;
      *reason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Malloc Failed");
      return L7_FAILURE;
    }
    memset (dhcpClientCB.clientIntfInfo[index], 0, sizeof(dhcpClientInfo_t));
  }

  /* DHCP Client per Interface Information Semaphore */
  if ((dhcpClientCB.clientIntfInfoSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO,
                                                          OSAPI_SEM_FULL))
                          == L7_NULLPTR)
  {
    *response = 0;
    *reason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Sem Create Failed");
    return L7_FAILURE;
  }

  /* DHCP Client Control Block Members Protection Semaphore */
  if ((dhcpClientCB.cbMembersProtectionSem= osapiSemaBCreate (OSAPI_SEM_Q_FIFO,
                                                              OSAPI_SEM_FULL))
                          == L7_NULLPTR)
  {
    *response = 0;
    *reason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "cbMembersProtectionSem Create Failed");
    return L7_FAILURE;
  }

  /* DHCP Client Message Queue Counters Semaphore */
  if ((dhcpClientCB.dhcpClientMsgQ.queueCountersSem =
                    osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL))
                 == L7_NULLPTR)
  {
    *response = 0;
    *reason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Q Counters Sem Creation Failed");
    return L7_FAILURE;
  }

  /* DHCP Client Message Queue */
  memcpy (dhcpClientCB.dhcpClientMsgQ.queueName, DHCP_CLIENT_QUEUE_NAME,
          sizeof(DHCP_CLIENT_QUEUE_NAME));
  dhcpClientCB.dhcpClientMsgQ.queueSize  = DHCP_CLIENT_QUEUE_SIZE;
  dhcpClientCB.dhcpClientMsgQ.queueMsgSize = DHCP_CLIENT_QUEUE_MSG_SIZE;
  dhcpClientCB.dhcpClientMsgQ.queuePtr =
        (void*) osapiMsgQueueCreate(dhcpClientCB.dhcpClientMsgQ.queueName,
                                    dhcpClientCB.dhcpClientMsgQ.queueSize,
                                    dhcpClientCB.dhcpClientMsgQ.queueMsgSize);

  if (dhcpClientCB.dhcpClientMsgQ.queuePtr == L7_NULLPTR)
  {
    *response = 0;
    *reason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Q Creation Failed");
    return L7_FAILURE;
  }

  if ((dhcpClientCB.dhcpClientTaskId = osapiTaskCreate ("DHCPv4 Client Task",
                                dhcpClientTask, 0, 0,
                                L7_DEFAULT_STACK_SIZE, L7_DEFAULT_TASK_PRIORITY,
                                L7_DEFAULT_TASK_SLICE))
                             == L7_ERROR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Dhcp Client Task Creation Failed");
    L7_LOGF (L7_LOG_SEVERITY_CRITICAL, L7_DHCP_CLIENT_COMPONENT_ID,
             "Dhcp Client Task Creation Failed");
    *response = 0;
    *reason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  *response = L7_CNFGR_CMD_COMPLETE;
  *reason = 0;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Phase-1 Initialization Successful");
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Phase 2 initialization.
 *
 * @param    response - @b{(output)}  Response if L7_SUCCESS
 * @param    reason   - @b{(output)}  Reason if L7_ERROR
 *
 * @returns  L7_SUCCESS if all goes well
 *           L7_ERROR if there is a problem
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *response,
                                   L7_CNFGR_ERR_RC_t *reason)
{
  /* Add any callback registrations here */

  *response = L7_CNFGR_CMD_COMPLETE;
  *reason = 0;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Phase 3 initialization.
 *
 * @param    response - @b{(output)}  Response if L7_SUCCESS
 * @param    reason   - @b{(output)}  Reason if L7_ERROR
 *
 * @returns  L7_SUCCESS if all goes well
 *           L7_ERROR if there is a problem
 *
 * @notes    Builds the default configuration
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *response,
                                   L7_CNFGR_ERR_RC_t *reason)
{
  if (dhcpBuildDefaultConfig() == L7_SUCCESS)
  {
    *response = L7_CNFGR_CMD_COMPLETE;
    *reason = 0;

    return L7_SUCCESS;
  }
  else
  {
    *response = 0;
    *reason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }
}

/*********************************************************************
 * @purpose  Return success to Configurator commands that are a no-op
 *           for the DHCP client.
 *
 * @param    response - @b{(output)}  Always command complete.
 * @param    reason   - @b{(output)}  Always 0
 *
 * @returns  L7_SUCCESS
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *response,
                              L7_CNFGR_ERR_RC_t *reason)
{
  *response = L7_CNFGR_CMD_COMPLETE;
  *reason   = 0;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Take actions required for DHCP client when the system enters the
 *           unconfig phase 1 state.
 *
 * @param    response - @b{(output)}  Always command complete.
 * @param    reason   - @b{(output)}  Always 0
 *
 * @returns  L7_SUCCESS
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpCnfgrUnconfigPhase1(L7_CNFGR_RESPONSE_t *response,
                                L7_CNFGR_ERR_RC_t *reason)
{
  /* NOTE: If an address is acquired by a Inband interface, IPMAP component
   * will take care of releasing it.
   */

  /* Release the address on the Network Port, if acquired */
  if (usmDbFeaturePresentCheck (0, L7_SIM_COMPONENT_ID,
                                L7_SIM_NETWORK_PORT_FEATURE_ID)
                             == L7_TRUE)
  {
    dhcpClientReleaseProcess (0, L7_MGMT_NETWORKPORT);
  }

  /* Release the address on the Service Port, if acquired */
  if (cnfgrIsComponentPresent (L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE)
  {
    dhcpClientReleaseProcess (0, L7_MGMT_SERVICEPORT);
  }

#ifdef L7_ROUTING_PACKAGE
  dhcpClientAllIntfInfoCleanup();
#endif

  *response = L7_CNFGR_CMD_COMPLETE;
  *reason = 0;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Take actions required for DHCP client when the system enters the
 *           unconfig phase 2 state.
 *
 * @param    response - @b{(output)}  Always command complete.
 * @param    reason   - @b{(output)}  Always 0
 *
 * @returns  L7_SUCCESS
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpCnfgrUnconfigPhase2(L7_CNFGR_RESPONSE_t *response,
                                L7_CNFGR_ERR_RC_t *reason)
{
  return dhcpCnfgrNoopProccess(response, reason);
}

/*********************************************************************
* @purpose  This function undoes dhcpCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpCnfgrFiniPhase1Process()
{
  if (dhcpClientCB.dhcpNotifyList != L7_NULLPTR)
  {
    osapiFree(L7_DHCP_CLIENT_COMPONENT_ID, dhcpClientCB.dhcpNotifyList);
    dhcpClientCB.dhcpNotifyList = L7_NULLPTR;
  }

  if (dhcpClientCB.notifyListSem != L7_NULLPTR)
  {
    osapiSemaDelete(dhcpClientCB.notifyListSem);
    dhcpClientCB.notifyListSem = L7_NULLPTR;
  }

  if (dhcpClientCB.cbMembersProtectionSem != L7_NULLPTR)
  {
    osapiSemaDelete(dhcpClientCB.cbMembersProtectionSem);
    dhcpClientCB.cbMembersProtectionSem = L7_NULLPTR;
  }
}

/*********************************************************************
* @purpose  This function Builds the default configuration for the DHCP Client
*
* @returns  L7_SUCCESS - There were no errors.
*
* @end
*********************************************************************/
L7_RC_t dhcpBuildDefaultConfig()
{
  if (osapiSemaTake (dhcpClientCB.cbMembersProtectionSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Release Sem");
    return L7_FAILURE;
  }

  /* Initialise to default configurtion.*/
  dhcpClientCB.dhcpOptions.vendorClassOption.optionMode = FD_DHCP_CLIENT_VENDOR_CLASS_OPTION_MODE;
  osapiStrncpy(dhcpClientCB.dhcpOptions.vendorClassOption.vendorClassString,
               FD_DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING,
               DHCP_VENDOR_CLASS_STRING_MAX);

  osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);

  return L7_SUCCESS;
}

