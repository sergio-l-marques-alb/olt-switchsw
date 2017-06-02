/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename  telnet_map.c
*
* @purpose   Outbound Telnet Mapping system infrastructure
*
* @component Outbound Telnet Mapping Layer
*
* @comments  none
*
* @create    02/27/2004
*
* @author    anindya
*
* @end
*
**********************************************************************/

#include "l7_telnetinclude.h"

/* external function prototypes */
extern void *   telnetProcQueue;         /* Outbound telnet Processing queue */

/* Outbound Telnet mapping component globals */
telnetMapCtrl_t         telnetMapCtrl_g;
L7_telnetMapCfg_t       *pTelnetMapCfgData;

void *telnetMap_Queue = L7_NULL;


/*********************************************************************
* @purpose  Initialize the Outbound Telnet layer application
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t telnetMapAppsInit(void)
{
    static const char *routine_name = "telnetMapAppsInit()";

    /*--------------------------------------*/
    /* initialize the Outbound Telnet stack */
    /*--------------------------------------*/

    /* create a semaphore to control mutual exclusion access to the Outbound
     * Telnet configuration and dyanamic data structure
     */
    telnetMapCtrl_g.telnetDataSemId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    if (telnetMapCtrl_g.telnetDataSemId == L7_NULL)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
                "%s %d: %s: Unable to create Outbound Telnet data control semaphore\n",
                __FILE__, __LINE__, routine_name);
        L7_assert(1);
        return L7_FAILURE;
    }

    telnetSessionRemoveAll();

    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Save Outbound Telnet user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t telnetSave(void)
{
  if (pTelnetMapCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    pTelnetMapCfgData->cfgHdr.dataChanged = L7_FALSE;
    pTelnetMapCfgData->checkSum = nvStoreCrc32((L7_char8 *)pTelnetMapCfgData,
                                               sizeof(L7_telnetMapCfg_t) - sizeof(pTelnetMapCfgData->checkSum));

    if (sysapiCfgFileWrite(L7_TELNET_MAP_COMPONENT_ID, L7_TELNET_CFG_FILENAME,
                           (L7_char8 *)pTelnetMapCfgData, sizeof(L7_telnetMapCfg_t)) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
              "telnetSave: Error during osapiFsWrite for config file %s\n",
              L7_TELNET_CFG_FILENAME);
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if Outbound Telnet user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL telnetHasDataChanged(void)
{
  return pTelnetMapCfgData->cfgHdr.dataChanged;
}
void telnetResetDataChanged(void)
{
  pTelnetMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Build default Outbound Telnet config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void telnetBuildDefaultConfigData(L7_uint32 ver)
{
  /*--------------------------*/
  /* build config file header */
  /*--------------------------*/

  memset(pTelnetMapCfgData, 0, sizeof(L7_telnetMapCfg_t));
  strcpy(pTelnetMapCfgData->cfgHdr.filename, L7_TELNET_CFG_FILENAME);
  pTelnetMapCfgData->cfgHdr.version       = ver;
  pTelnetMapCfgData->cfgHdr.componentID   = L7_TELNET_MAP_COMPONENT_ID;
  pTelnetMapCfgData->cfgHdr.type          = L7_CFG_DATA;
  pTelnetMapCfgData->cfgHdr.length        = sizeof(L7_telnetMapCfg_t);
  pTelnetMapCfgData->cfgHdr.dataChanged   = L7_FALSE;

  /*---------------------------*/
  /* build default config data */
  /*---------------------------*/

  /* generic Outbound Telnet cfg */
  pTelnetMapCfgData->telnet.telnetAdminMode           = FD_TELNET_DEFAULT_ADMIN_MODE;
  pTelnetMapCfgData->telnet.telnetMaxSessions         = FD_TELNET_DEFAULT_MAX_SESSIONS;
  pTelnetMapCfgData->telnet.telnetNoOfActiveSessions  = 0;
  pTelnetMapCfgData->telnet.telnetTimeout             = FD_TELNET_DEFAULT_TIMEOUT;
}


/*********************************************************************
* @purpose  Apply Outbound Telnet config data
*
* @param    void
*
* @returns  L7_SUCCESS  Config data applied
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t telnetApplyConfigData(void)
{
  /* check validity of admin mode */
  if (! (pTelnetMapCfgData->telnet.telnetAdminMode == L7_ENABLE ||
         pTelnetMapCfgData->telnet.telnetAdminMode == L7_DISABLE) )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
            "telnetApplyConfigData: Error invalid admin mode\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Purge Outbound Telnet config data
*
* @param    void
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    void
*
* @end
*********************************************************************/
L7_RC_t telnetPurgeData(void)
{
  if(pTelnetMapCfgData == L7_NULL || telnetMapCtrl_g.telnetInitialized != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(telnetMapCtrl_g.telnetDataSemId, L7_WAIT_FOREVER);

  memset(pTelnetMapCfgData, 0, sizeof(L7_telnetMapCfg_t));

  osapiSemaGive(telnetMapCtrl_g.telnetDataSemId) ;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  The Outbound Telnet processing task uses this function to
*           process messages from the message queue, performing the tasks
*           specified in each message. The initial motivation for this
*           task is to offload initialization processing from the
*           configurator.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void telnetProcTask( void )
{
    L7_uint32 status;
    telnetProcMsg_t message;


    osapiTaskInitDone(L7_TELNET_PROC_TASK_SYNC);

    /* Loop forever, processing incoming messages */
    while (L7_TRUE)
    {
        memset(&message, 0x00, sizeof(telnetProcMsg_t));
        status = osapiMessageReceive(telnetProcQueue, &message,
                                     sizeof(telnetProcMsg_t), L7_WAIT_FOREVER);

        if (status == L7_SUCCESS)
        {
            switch (message.msgType)
            {
            case TELNET_CNFGR_CMD:
                telnetCnfgrHandle(&message.type.cmdData);
                break;

            case TELNET_RESTORE_EVENT:
                telnetRestoreProcess();
                break;

            default:
                break;
            }
        }
        else
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
                    "telnetProcTask: Bad status on receive message from proc queue %08x\n", status);
        }
    } /* endwhile */

    return;
}

/*********************************************************************
* @purpose  Restores Outbound Telnet configuration to factory defaults.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t telnetRestoreProcess(void)
{
    telnetPurgeData();
    telnetBuildDefaultConfigData(L7_TELNET_CFG_VER_CURRENT);
    pTelnetMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    return(telnetApplyConfigData());
}
