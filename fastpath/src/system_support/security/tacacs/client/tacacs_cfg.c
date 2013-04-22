/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_cfg.c
*
* @purpose tacacs+ configuration file
*
* @component tacacs+
*
* @comments none
*
* @create 03/08/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "sysapi.h"
#include "osapi_support.h"
#include "tacacs_cfg.h"
#include "tacacs_aaa.h"
#include "tacacs_control.h"
#include "tacacs_cnfgr.h"
#include "tacacs_api.h"

extern L7_uint32          tacacsTaskId;
extern L7_uint32          tacacsRxTaskId;
extern tacacsCnfgrState_t tacacsCnfgrState;

tacacsCfg_t         *tacacsCfg            = L7_NULLPTR;
void                *tacacsQueue          = L7_NULLPTR;
void                *tacacsSemaphore      = L7_NULLPTR;
tacacsNotifyEntry_t *tacacsNotifyList     = L7_NULLPTR;
tacacsOprData_t     *tacacsOprData        = L7_NULLPTR;


/*********************************************************************
*
* @purpose System Initialization for tacacs+ component
*
* @param    pCmdData - @b{(input)}
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CB_DATA_t   cbData;
  tacacsMsg_t          msg;

  /* create the task queue  */
  tacacsQueue = (void *)osapiMsgQueueCreate("tacacs_queue",
                                            FD_CNFGR_TACACS_MSG_COUNT,
                                            (L7_uint32)sizeof(tacacsMsg_t));
  if (tacacsQueue == L7_NULLPTR)
  {
    TACACS_ERROR_SEVERE("TACACS+: Severe error, unable to create msg queue!\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  /* semaphore creation for task protection over the common data*/
  tacacsSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (tacacsSemaphore == L7_NULLPTR)
  {
    TACACS_ERROR_SEVERE
      ("TACACS+: Severe error, unable to create the task semaphore\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (tacacsStartTasks() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
            "TACACS+: Start Tasks Failed\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  /* Sending Message to Configurator Init*/
  memcpy(&msg.data.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.event=TACACS_EVENT_CNFGR_INIT;
  (void)osapiMessageSend(tacacsQueue,
                         &msg,
                         sizeof(tacacsMsg_t),
                         L7_NO_WAIT,
                         L7_MSG_PRIORITY_NORM);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose System Init Undo for tacacs+ component
*
* @param   none
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void tacacsInitUndo(void)
{
  if (tacacsQueue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(tacacsQueue);
    tacacsQueue = L7_NULLPTR;
  }

  if (tacacsTaskId != L7_ERROR)
  {
    osapiTaskDelete(tacacsTaskId);
    tacacsTaskId = L7_ERROR;
  }

  if (tacacsRxTaskId != L7_ERROR)
  {
    osapiTaskDelete(tacacsRxTaskId);
    tacacsRxTaskId = L7_ERROR;
  }

  if (tacacsSemaphore != L7_NULLPTR)
  {
    (void)osapiSemaDelete(tacacsSemaphore);
    tacacsSemaphore = L7_NULLPTR;
  }

  tacacsCnfgrState = TACACS_PHASE_INIT_0;
}

/*********************************************************************
*
* @purpose Saves the TACACS+ Client configuration
*
* @param   none
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t tacacsSave(void)
{
  if (tacacsHasDataChanged() == L7_TRUE)
  {
    tacacsCfg->header.dataChanged = L7_FALSE;
    tacacsCfg->checkSum = nvStoreCrc32((L7_uchar8 *)tacacsCfg,
                                       (L7_uint32)(sizeof(tacacsCfg_t) - sizeof(tacacsCfg->checkSum)));

    if (sysapiCfgFileWrite(L7_TACACS_COMPONENT_ID,
                           L7_TACACS_CFG_FILE_NAME,
                           (L7_uchar8 *)tacacsCfg,
                           (L7_uint32)sizeof(tacacsCfg_t)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
              "TACACS+: Failed to save configuration file.\n");
      return L7_FAILURE;
    }
  } /* end of if data changed */

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Restores the TACACS+ Client configuration
*
* @param   none
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments tacacsCfg is the overlay
*
* @end
*
*********************************************************************/
L7_RC_t tacacsRestore(void)
{

#if 0  /* single-connection not implemented yet */
  /* close open sockets to single-connection servers, sockets open
     for a session are always closed on session delete */
  L7_uint32 i = 0;
  for (i=0; i < L7_TACACS_MAX_SERVERS; i++)
  {
    if (tacacsOprData->servers[i].status == TACACS_SERVER_STATUS_CONNECTED)
    {
      tacacsOprData->servers[i].status = TACACS_SERVER_STATUS_DISCONNECTED;
      osapiSocketClose(tacacsOprData->servers[i].socket);
      tacacsOprData->servers[i].socket = 0;
    }
  }
#endif

  /* build the default configuration */
  tacacsBuildDefaultConfigData(L7_TACACS_CFG_VER_CURRENT);
  /* apply configuration */
  tacacsApplyConfigData();

  tacacsCfg->header.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To build Tacacs+ default config data for a server
*
* @param   L7_uint32         version     @b{(input)} software version of Config Data.
* @param   tacacsServerCfg_t *serverCfg  @((output)) server configuration
*
* @returns none
*
* @comments
*
* @end
*
*********************************************************************/
void tacacsBuildDefaultServerConfigData(L7_uint32 version,
                                        tacacsServerCfg_t *serverCfg)
{
  memset(serverCfg, 0, sizeof(tacacsServerCfg_t));
  serverCfg->port = FD_TACACS_PLUS_PORT_NUM;
  serverCfg->priority = FD_TACACS_PLUS_PRIORITY;
  serverCfg->singleConnection = FD_TACACS_PLUS_SINGLE_CONNECTION;
  serverCfg->sourceIP = FD_TACACS_PLUS_SRC_IP_ADDR;  /* 0.0.0.0, placeholder to support mult interfaces L3 */
  serverCfg->timeout = FD_TACACS_PLUS_TIMEOUT_VALUE;
  /* the following default to the global configuration */
  serverCfg->useGblKey = L7_TRUE;
  serverCfg->useGblSourceIP = L7_TRUE;
  serverCfg->useGblTimeout = L7_TRUE;
  return;
}

/*********************************************************************
*
* @purpose To build Tacacs+ default config Data.
*
* @param   version @b{(input)} software version of Config Data.
*
* @returns none
*
* @comments
*
* @end
*
*********************************************************************/
void tacacsBuildDefaultConfigData(L7_uint32 version)
{
  /* clear configuration */
  memset(tacacsCfg, 0, sizeof(tacacsCfg_t));

  /* set up file header */
  tacacsCfg->header.version = version;
  tacacsCfg->header.componentID = L7_TACACS_COMPONENT_ID;
  tacacsCfg->header.type = L7_CFG_DATA;
  tacacsCfg->header.length = (L7_uint32)sizeof(tacacsCfg_t);
  strcpy((L7_char8 *)tacacsCfg->header.filename, L7_TACACS_CFG_FILE_NAME);
  tacacsCfg->header.dataChanged = L7_FALSE;

  /* set global defaults, there is no default secret key, and no servers configured */
  tacacsCfg->timeout = FD_TACACS_PLUS_TIMEOUT_VALUE;
  tacacsCfg->sourceIP = FD_TACACS_PLUS_SRC_IP_ADDR;  /* 0.0.0.0, placeholder to support mult interfaces L3 */

  return;
}

/*********************************************************************
*
* @purpose To apply tacacs+ config data.
*
* @param   none.
*
* @returns none
*
* @comments
*
* @end
*
*********************************************************************/
void tacacsApplyConfigData(void)
{
  /* there really isn't anything to apply, sockets are opened when an
     authentication session is processed */

  return;
}

/*********************************************************************
*
* @purpose Checks if TACACS+ Client user config data has changed
*
* @param   none
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL tacacsHasDataChanged(void)
{
  return tacacsCfg->header.dataChanged;
}
void tacacsResetDataChanged(void)
{
  tacacsCfg->header.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Print current TACACS client configuration values to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t tacacsInfoShow(void)
{
  L7_uint32 i = 0;
  sysapiPrintf("TACACS+ Client Configuration\n");
  sysapiPrintf("============================\n\n");

  sysapiPrintf("Encryption: %s\n", tacacsOprData->unencrypted ? "Off (until reset)" : "On");
  sysapiPrintf("Global Timeout: %i\n", tacacsCfg->timeout);
  sysapiPrintf("Global Source IP: %s\n", osapiInet_ntoa(tacacsCfg->sourceIP));
  sysapiPrintf("Global Key: %s\n\n", tacacsCfg->key);

  sysapiPrintf("Configured Servers\n");
  sysapiPrintf("==================\n\n");

  for (i=0; i < L7_TACACS_MAX_SERVERS; i++)
  {
    if (tacacsCfg->servers[i].ip != 0)
    {
      sysapiPrintf("Server [%i]\n", i);
      sysapiPrintf("-----------\n");
      sysapiPrintf("IP: %s\n", osapiInet_ntoa(tacacsCfg->servers[i].ip));
      sysapiPrintf("Priority: %u\n", tacacsCfg->servers[i].priority);
      sysapiPrintf("Port: %u\n", tacacsCfg->servers[i].port);
      sysapiPrintf("Single Connection: %s\n", tacacsCfg->servers[i].singleConnection ? "Yes" : "No");
      if (tacacsCfg->servers[i].useGblTimeout == L7_TRUE)
      {
        sysapiPrintf("Timeout: Global\n");
      } else
      {
        sysapiPrintf("Timeout: %u\n", tacacsCfg->servers[i].timeout);
      }
      if (tacacsCfg->servers[i].useGblSourceIP == L7_TRUE)
      {
        sysapiPrintf("Source IP: Global\n");
      } else
      {
        sysapiPrintf("Source IP: %u\n", osapiInet_ntoa(tacacsCfg->servers[i].sourceIP));
      }
      if (tacacsCfg->servers[i].useGblKey == L7_TRUE)
      {
        sysapiPrintf("Key: Global\n");
      } else
      {
        sysapiPrintf("Key: %s\n", tacacsCfg->servers[i].key);
      }
      sysapiPrintf("\n");
    }
  }
  return L7_SUCCESS;
}

