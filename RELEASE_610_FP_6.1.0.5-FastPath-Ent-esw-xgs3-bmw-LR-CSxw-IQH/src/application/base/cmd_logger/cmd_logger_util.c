
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  cmd_logger_util.c
*
* @purpose   command logger utility functions
*
* @component cmdLogger component
*
* @comments  none
*
* @create    25-Oct-2004
*
* @author    gaunce
*
* @end
*             
**********************************************************************/

#include "cmd_logger_include.h"
#include "default_cnfgr.h"

cmdLoggerCfgData_t    cmdLoggerCfgData;

extern  cmdLoggerCnfgrState_t cmdLoggerCnfgrState;



/*********************************************************************
* @purpose  Checks if cmdLogger user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL 
cmdLoggerHasDataChanged(void)
{
  return cmdLoggerCfgData.cfgHdr.dataChanged;
}
void cmdLoggerResetDataChanged(void)
{
  cmdLoggerCfgData.cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Print the current cmdLogger config values to 
*           serial port
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t 
cmdLoggerConfigDump(void)
{
  L7_RC_t rc=L7_SUCCESS;
  L7_char8 buf[66];

  if (cmdLoggerCfgData.adminMode == L7_ENABLE)
    sprintf(buf,"%s","L7_ENABLE");
  else 
    sprintf(buf,"%s","L7_DISABLE");
  printf("Command Logger Admin Mode - %s\n",buf);
  printf("Command Logger Severity - %d\n", (L7_uint32)(cmdLoggerCfgData.severity));

  return rc;
}


/*********************************************************************
* @purpose  Saves cmdLogger config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
cmdLoggerSave(void)
{
  
  if (cmdLoggerCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    cmdLoggerCfgData.cfgHdr.dataChanged = L7_FALSE;
    cmdLoggerCfgData.checkSum = nvStoreCrc32((L7_char8 *)&cmdLoggerCfgData,
                                          sizeof(cmdLoggerCfgData) - sizeof(cmdLoggerCfgData.checkSum));
    /* call save NVStore routine */
    if (sysapiCfgFileWrite(L7_CMD_LOGGER_COMPONENT_ID, CMD_LOGGER_CFG_FILENAME, 
                           (L7_char8 *)&cmdLoggerCfgData, sizeof (cmdLoggerCfgData)) == L7_ERROR)
    {
      LOG_MSG("Error on call to osapiFsWrite routine on config file %s\n", CMD_LOGGER_CFG_FILENAME);
    }
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Restores cmdLogger config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
cmdLoggerRestore(void)
{
  cmdLoggerBuildDefaultConfigData(CMD_LOGGER_CFG_VER_CURRENT);
  cmdLoggerCfgData.cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);                                                     /* TBD: RC checking */
}


/*********************************************************************
* @purpose  Build default cmdLogger config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void 
cmdLoggerBuildDefaultConfigData(L7_uint32 ver)
{
  memset(( void * )&cmdLoggerCfgData, 0, sizeof( cmdLoggerCfgData_t));  
  strcpy(cmdLoggerCfgData.cfgHdr.filename, CMD_LOGGER_CFG_FILENAME);
  cmdLoggerCfgData.cfgHdr.version = ver;
  cmdLoggerCfgData.cfgHdr.componentID = L7_CMD_LOGGER_COMPONENT_ID;
  cmdLoggerCfgData.cfgHdr.type = L7_CFG_DATA;
  cmdLoggerCfgData.cfgHdr.length = sizeof(cmdLoggerCfgData);
  cmdLoggerCfgData.cfgHdr.dataChanged = L7_FALSE;
  cmdLoggerCfgData.cfgHdr.version = ver;

  cmdLoggerCfgData.adminMode = FD_CMD_LOGGER_ADMIN_MODE;
  cmdLoggerCfgData.severity = FD_CMD_LOGGER_SEVERITY;

  cmdLoggerCfgData.webAdminMode = FD_CMD_LOGGER_WEB_ADMIN_MODE;
  cmdLoggerCfgData.webSeverity = FD_CMD_LOGGER_WEB_SEVERITY;

  cmdLoggerCfgData.snmpAdminMode =  FD_CMD_LOGGER_SNMP_ADMIN_MODE;
  cmdLoggerCfgData.snmpSeverity = FD_CMD_LOGGER_SNMP_SEVERITY;
}

/*********************************************************************
* @purpose  cmdLogger set to factory defaults  
*
* @param    none
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void 
cmdLoggerInitData()
{
  cmdLoggerCfgData.adminMode = FD_CMD_LOGGER_ADMIN_MODE;
  cmdLoggerCfgData.severity = FD_CMD_LOGGER_SEVERITY;

  cmdLoggerCfgData.webAdminMode = FD_CMD_LOGGER_WEB_ADMIN_MODE;
  cmdLoggerCfgData.webSeverity = FD_CMD_LOGGER_WEB_SEVERITY;

  cmdLoggerCfgData.snmpAdminMode =  FD_CMD_LOGGER_SNMP_ADMIN_MODE;
  cmdLoggerCfgData.snmpSeverity = FD_CMD_LOGGER_SNMP_SEVERITY;
}

 /*********************************************************************
* @purpose Initialize the cmdLogger for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.  If this fails, it is due to an inability to 
*         to acquire resources.
*
* @end
*********************************************************************/
L7_RC_t cmdLoggerPhaseOneInit(void)
{
  L7_RC_t rc = L7_SUCCESS;

  return(rc);
}

/*********************************************************************
* @purpose  Release all resources collected during phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void cmdLoggerPhaseOneFini(void)
{
    cmdLoggerCnfgrState = CMD_LOGGER_PHASE_INIT_0;
}

/*********************************************************************
* @purpose Initialize the cmdLogger for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t cmdLoggerPhaseTwoInit(void)
{
  L7_RC_t rc = L7_SUCCESS;
  nvStoreFunctionList_t notifyFunctionList; /* nvstore Functions' Data Structure */

  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID     = L7_CMD_LOGGER_COMPONENT_ID;
  notifyFunctionList.notifySave       = cmdLoggerSave;
  notifyFunctionList.hasDataChanged   = cmdLoggerHasDataChanged;
  notifyFunctionList.notifyConfigDump = cmdLoggerConfigDump;
  notifyFunctionList.resetDataChanged = cmdLoggerResetDataChanged;
  if ((rc = nvStoreRegister(notifyFunctionList)) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose Free the resource for phase 2
*
* @param   void
*
* @returns void
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
void cmdLoggerPhaseTwoFini(void)
{
  nvStoreFunctionList_t notifyFunctionList; /* nvstore Functions' Data Structure */

  /* nvstore Functions' Data Structure initialized to zero */
  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));

  /* de-register by setting the values to zero and registering again */
  (void)nvStoreRegister(notifyFunctionList);

  cmdLoggerCnfgrState = CMD_LOGGER_PHASE_INIT_1;
  return;
}

/*********************************************************************
* @purpose Initialize the cmdLogger for Phase 3
*
* @param   void
*
* @returns L7_SUCCESS  Phase 3 completed
* @returns L7_FAILURE  Phase 3 incomplete
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t cmdLoggerPhaseThreeInit(void)
{
  L7_RC_t rc = L7_SUCCESS;

  rc = sysapiCfgFileGet(L7_CMD_LOGGER_COMPONENT_ID, CMD_LOGGER_CFG_FILENAME, 
                        (L7_char8 *)&cmdLoggerCfgData, sizeof(cmdLoggerCfgData), 
                        &cmdLoggerCfgData.checkSum, CMD_LOGGER_CFG_VER_CURRENT, 
                        cmdLoggerBuildDefaultConfigData, cmdLoggerMigrateConfigData);  
  if (rc != L7_SUCCESS )
  {
    rc = L7_FAILURE;
    cmdLoggerInitData();
  }
  else 
  {
    cmdLoggerCfgData.cfgHdr.dataChanged = L7_FALSE;
  }

  return(rc);
}

/*********************************************************************
* @purpose Reset the cmdLogger to prior to phase 3
*
* @param   void
*
* @returns void
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
void cmdLoggerPhaseThreeFini(void)
{
  /* config data initialized to zero */
  cmdLoggerRestore();
  
  cmdLoggerCnfgrState = CMD_LOGGER_PHASE_INIT_2;
}


