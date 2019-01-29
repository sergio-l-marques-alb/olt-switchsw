/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  snmp_util.c
*
* @purpose   snmp utility functions
*
* @component snmp component
*
* @comments  none
*
* @create    02/26/2001
*
* @author    asuthan
*
* @end
*
**********************************************************************/

#include "commdefs.h"
#define SNMP_FACTORY_DEFAULT FACTORY_DEFAULT_DEFINE_SNMP
#include "l7_common.h"
#include "snmp_exports.h"
#include "usmdb_common.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_util_api.h"
#include "defaultconfig.h"

#include "osapi.h"
#include "snmp_util.h"
#include "nvstoreapi.h"
#include "snmptask_ct.h"
#include "user_mgr_api.h"
#include "snmp_api.h"
#include "snmp_trap_api.h"
#include "snmp_sid.h"
#include "snmp_fileio_api.h"

L7_int32 L7_snmp_task_id;
L7_int32 L7_snmp_save_cfg_id;
L7_int32 L7_snmp_cfg_task_id;
L7_int32 L7_snmp_mon_task_id;
L7_int32 L7_snmp_ct_task_id;
L7_int32 L7_snmp_trap_task_id;

static L7_BOOL SnmpResetTrapTable = L7_TRUE;
static L7_BOOL SnmpCommTableApply = L7_TRUE;

void              *snmpSema;
void        *snmpTrapMessageQueue = L7_NULLPTR;
snmpState_t        snmpState;
snmpCfgData_t snmpCfgData;
snmpSupportedMib_t supportedMibs[L7_SNMP_SUPPORTED_MIB_COUNT];

/* SNMP Agent Functions to activate loading and saving the configuration file */
extern void SnmpSaveConfiguration();
extern void SnmpLoadConfiguration();

L7_RC_t snmpConfigDump(void);
void snmpConfigDataTestShow(void);

/*********************************************************************
* @purpose  Initialize snmp tasks
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void
snmpStartTasks(void)
{
  snmp_fileinit();

  L7_snmp_task_id = osapiTaskCreate( L7_SNMP_TASK_NAME, L7_snmp_task, 0, 0,
                                     snmpSidTaskStackSizeGet(),
                                     snmpSidTaskPriorityGet(),
                                     snmpSidTaskSliceGet());
  if (L7_snmp_task_id == L7_ERROR)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNMP_COMPONENT_ID,
            "Failed to Create SNMP Task.\n");

  L7_snmp_save_cfg_id = osapiTaskCreate( "SNMPSaveCfgTask",
                                         (void *)sysapiWriteConfigToFlashTask, 0, 0,
                                         4*snmpSidTaskStackSizeGet(),
                                         snmpSidTaskPriorityGet(),
                                         snmpSidTaskSliceGet());

  if (L7_snmp_save_cfg_id == L7_ERROR)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNMP_COMPONENT_ID,
            "Failed to Create SNMP Task.\n");

#ifdef _L7_OS_LINUX_
  /*if we are running linux*/
  /*start a task to monitor proc*/
  L7_snmp_mon_task_id = osapiTaskCreate("SNMPProcMon", L7_snmp_proc_monitor, 0, 0,
                                        snmpSidTaskStackSizeGet(),
                                        snmpSidTaskPriorityGet(),
                                        snmpSidTaskSliceGet());
  if (L7_snmp_mon_task_id == L7_ERROR)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNMP_COMPONENT_ID,
            "Failed to create SNMP proc monitoring task\n");
#endif


  L7_snmp_ct_task_id = osapiTaskCreate( "SNMPCTTask", L7_snmp_ct_task, 0, 0,
                                        snmpSidTaskStackSizeGet(),
                                        snmpSidTaskPriorityGet(),
                                        snmpSidTaskSliceGet());

  if (L7_snmp_ct_task_id == L7_ERROR)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNMP_COMPONENT_ID,
            "Failed to Create SNMP Cable Test Task.\n");

  L7_snmp_cfg_task_id = osapiTaskCreate( "SNMPCfgTask", L7_snmp_cfg_task, 0, 0,
                                         snmpSidTaskStackSizeGet(),
                                         snmpSidTaskPriorityGet(),
                                         snmpSidTaskSliceGet());

  if (L7_snmp_cfg_task_id == L7_ERROR)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNMP_COMPONENT_ID,
            "Failed to Create SNMP Config Task.\n");

  L7_snmp_trap_task_id = osapiTaskCreate( "SNMPTrapTask", L7_snmp_trap_task, 0, 0,
                                         snmpSidTaskStackSizeGet(),
                                         snmpSidTaskPriorityGet(),
                                         snmpSidTaskSliceGet());

  if (L7_snmp_trap_task_id == L7_ERROR)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNMP_COMPONENT_ID,
            "Failed to Create SNMP Trap Task.\n");
}

/*********************************************************************
* @purpose  Checks if snmp user config data has changed
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
snmpHasDataChanged(void)
{
  return snmpCfgData.cfgHdr.dataChanged;
}
void snmpResetDataChanged(void)
{
  snmpCfgData.cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Print the current SNMP config values to serial port
*
* @param    table 1 - community, 2 - trap receivers, 3 - users
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpConfigDumpTable(L7_uint32 table)
{
  L7_RC_t rc=L7_SUCCESS;
  L7_char8 buf[64];
  L7_uint32 i=0;

  if (table == 1)
  {
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "L7_MAX_SNMP_COMM  #%d\n", L7_MAX_SNMP_COMM);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "L7_SNMP_SIZE  #%d\n", L7_SNMP_SIZE);

  for (i=0; i<L7_MAX_SNMP_COMM; i++)
  {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "SNMP Community Table Entry #%d\n---------------------------\n", i);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "\nSNMP Community Name - %s\n", snmpCfgData.snmpComm[i].agentCommunityName);

      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "SNMP Community Owner - %s\n", snmpCfgData.snmpComm[i].agentCommunityOwner);

    osapiInetNtoa (snmpCfgData.snmpComm[i].agentCommunityClientIpAddr,buf);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "SNMP Client IP Address - %s\n",buf);

    osapiInetNtoa (snmpCfgData.snmpComm[i].agentCommunityClientIpMask,buf);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "SNMP Client IP Mask - %s\n",buf);

      switch (snmpCfgData.snmpComm[i].agentCommunityAccessLevel)
      {
      case L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE:
        sprintf(buf,"L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE");
        break;
      case L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY:
        sprintf(buf,"L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY");
        break;
      default:
        sprintf(buf,"? %d ?", snmpCfgData.snmpComm[i].agentCommunityAccessLevel);
        break;
      }
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "SNMP Community Access Level - %s\n",buf);

    switch (snmpCfgData.snmpComm[i].agentCommunityStatus)
    {
    case L7_SNMP_COMMUNITY_STATUS_VALID:
        sprintf(buf,"L7_SNMP_COMMUNITY_STATUS_VALID");
      break;
    case L7_SNMP_COMMUNITY_STATUS_INVALID:
        sprintf(buf,"L7_SNMP_COMMUNITY_STATUS_INVALID");
      break;
    case L7_SNMP_COMMUNITY_STATUS_DELETE:
        sprintf(buf,"L7_SNMP_COMMUNITY_STATUS_DELETE");
      break;
    case L7_SNMP_COMMUNITY_STATUS_CONFIG:
        sprintf(buf,"L7_SNMP_COMMUNITY_STATUS_CONFIG");
      break;
      default:
        sprintf(buf,"? %d ?", snmpCfgData.snmpComm[i].agentCommunityStatus);
      }
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "SNMP Community Status - %s\n\n",buf);
    }
    }
  else if (table == 2)
  {
    for (i=0; i<L7_MAX_SNMP_COMM; i++)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "SNMP Trap Table Entry #%d\n-----------------------\n", i);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "SNMP Trap Manager Community Name - %s\n",snmpCfgData.snmpTrapMgr[i].agentTrapMgrCommunityName);

    osapiInetNtoa (snmpCfgData.snmpTrapMgr[i].agentTrapMgrIpAddr,buf);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "SNMP Trap Manager IP Address - %s\n",buf);

    if (snmpCfgData.snmpTrapMgr[i].agentTrapMgrStatus == L7_ENABLE)
      sprintf(buf,"%s","L7_ENABLE");
    else
      sprintf(buf,"%s","L7_DISABLE");
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "SNMP Trap Manager Status - %s\n\n",buf);
    }
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "%s", "1 - SNMP Community Table");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_MASK, "%s", "2 - SNMP Trap Receiver Table");
    rc = L7_FAILURE;
  }

  return rc;
}





/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void snmpBuildTestConfigData(void)
{

    L7_uint32 i;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/


    for (i=0; i<L7_MAX_SNMP_COMM; i++)
    {

        strncpy( snmpCfgData.snmpComm[i].agentCommunityName, "TESTNAME", L7_SNMP_NAME_SIZE);
        strncpy( snmpCfgData.snmpComm[i].agentCommunityName, "TESTOWNER", L7_SNMP_NAME_SIZE);
        snmpCfgData.snmpComm[i].agentCommunityClientIpAddr = 0x0A0A0A01 + i;
        snmpCfgData.snmpComm[i].agentCommunityClientIpMask = 0xFFFF0000;
        snmpCfgData.snmpComm[i].agentCommunityAccessLevel = L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE;
        snmpCfgData.snmpComm[i].agentCommunityStatus = L7_SNMP_COMMUNITY_STATUS_CONFIG;

        strncpy( snmpCfgData.snmpTrapMgr[i].agentTrapMgrCommunityName, "TEST_TRAP_NAME", L7_SNMP_NAME_SIZE);
        snmpCfgData.snmpTrapMgr[i].agentTrapMgrIpAddr = 0x0A0A0A02;
        snmpCfgData.snmpTrapMgr[i].agentTrapMgrStatus = L7_SNMP_TRAP_MGR_STATUS_CONFIG;
    }

 /* End of Component's Test Non-default configuration Data */



  /* Force write of config file */
  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;
  sysapiPrintf("Built test config data\n");

}


/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void snmpConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;
    L7_uint32     i;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &snmpCfgData.cfgHdr;
    sysapiPrintf("pFileHdr->filename    : %s\n", (char *)pFileHdr->filename);
    sysapiPrintf("pFileHdr->version     : %u\n", pFileHdr->version);
    sysapiPrintf("pFileHdr->componentID : %u\n", pFileHdr->componentID);
    sysapiPrintf("pFileHdr->type        : %u\n", pFileHdr->type);
    sysapiPrintf("pFileHdr->length      : %u\n", pFileHdr->length);
    sysapiPrintf("pFileHdr->dataChanged : %u\n", pFileHdr->dataChanged);

    /* Start of release I file header changes */

    sysapiPrintf("pFileHdr->savePointId : %u\n", pFileHdr->savePointId);
    sysapiPrintf("pFileHdr->targetDevice: %u\n", pFileHdr->targetDevice);
    for (i = 0; i < L7_FILE_HDR_PAD_LEN; i++)
    {
        sysapiPrintf("pFileHdr->pad[i]      : %u\n", pFileHdr->pad[i]);
    }

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    snmpConfigDump();


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("snmpCfgData.checkSum : %u\n", snmpCfgData.checkSum);


}




/*********************************************************************
* @purpose  Print the current SNMP config values to serial port
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpConfigDump(void)
{
  snmpConfigDumpTable(1);
  snmpConfigDumpTable(2);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Saves SNMP user config file to NVStore
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
snmpSave(void)
{

  if (snmpCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    snmpCfgData.cfgHdr.dataChanged = L7_FALSE;
    snmpCfgData.checkSum = nvStoreCrc32((L7_char8 *)&snmpCfgData,
                                        sizeof(snmpCfgData) - sizeof(snmpCfgData.checkSum));

    if (sysapiCfgFileWrite(L7_SNMP_COMPONENT_ID, SNMP_CFG_FILENAME, (L7_char8 *)&snmpCfgData,
                           sizeof (snmpCfgData)) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNMP_COMPONENT_ID,
              "Error on call to sysapiCfgFileWrite routine on config file %s\n",SNMP_CFG_FILENAME);
    }
  }

#if (!L7_FEAT_SNMP_CONFAPI)
  /* Force the SNMP Agent to save it's data to a file buffer */
  SnmpSaveConfiguration();

  if (snmp_saveFileBuffer(SNMP_CONFIGURATION_FILE) == L7_SUCCESS)
  {
    /* Do nothing for now */
  }
#endif

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Restores SNMP user config file to factory defaults
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
snmpRestore(void)
{
  /* reload default agent base configuration */
  snmp_resetToDefault(SNMP_CONFIGURATION_FILE);
  SnmpLoadConfiguration();

  snmpBuildDefaultConfigData(SNMP_CFG_VER_CURRENT);
  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;
  snmpInitData();

  SnmpResetConfig = L7_ENABLE;
  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Build default snmp config data
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
snmpBuildDefaultConfigData(L7_uint32 ver)
{
  memset(( void * )&snmpCfgData, 0, sizeof( snmpCfgData_t));
  strcpy(snmpCfgData.cfgHdr.filename, SNMP_CFG_FILENAME);
  snmpCfgData.cfgHdr.version = ver;
  snmpCfgData.cfgHdr.componentID = L7_SNMP_COMPONENT_ID;
  snmpCfgData.cfgHdr.type = L7_CFG_DATA;
  snmpCfgData.cfgHdr.length = sizeof(snmpCfgData);
  snmpCfgData.cfgHdr.dataChanged = L7_FALSE;

  snmpInitData ();
}

/*********************************************************************
* @purpose  SNMP set to factory defaults
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
snmpInitData()
{
#if (!L7_FEAT_SNMP_CONFAPI)
  memcpy((snmpComm_t *)&snmpCfgData.snmpComm, (snmpComm_t *)&FD_snmpComm,
         (sizeof (snmpComm_t) * L7_MAX_SNMP_COMM));
  memcpy((snmpTrapMgr_t *)&snmpCfgData.snmpTrapMgr, (snmpTrapMgr_t *)&FD_snmpTrapMgr,
         (sizeof (snmpTrapMgr_t) * L7_MAX_SNMP_COMM));

  /* create default Community table entries */
  SnmpCommunityTableUpdate();

  /* create default Trap Receiver table entries. */
  SnmpTrapMgrTableUpdate();
#endif

  /* initialize SNMP Agent traps */
  SnmpInitializeTraps();

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;
}


/*********************************************************************
* @purpose  Check to see if SNMP is present.
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL
snmpPresent()
{
  return L7_TRUE;
}


/*********************************************************************
* @purpose Allow SNMP to access USMDB
*
* @param   void
*
* @returns nothing
*
* @notes  none
*
* @end
*********************************************************************/
void snmpUsmDbReadyCallback()
{
  SnmpSetState(L7_SNMP_STATE_CONFIG);
}


/*********************************************************************
* @purpose Initialize SNMP for Phase 1
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
L7_RC_t snmpPhaseOneInit()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 index;

  /* config data initialized to zero */
  memset((void *) &snmpCfgData, 0, sizeof(snmpCfgData_t));

  /* clear the supported mibs table */
  for (index = 0; index<L7_SNMP_SUPPORTED_MIB_COUNT; index++)
  {
    bzero(supportedMibs[index].mibName, L7_SNMP_SUPPORTED_MIB_NAME_SIZE);
    bzero(supportedMibs[index].mibDescription, L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE);
    bzero(supportedMibs[index].mibOID, L7_SNMP_SUPPORTED_MIB_OID_SIZE);
    supportedMibs[index].mibEntryCreationTime = 0;
    supportedMibs[index].mibStatus = L7_DISABLE;
  }

  snmpState = L7_SNMP_STATE_INIT;

  /* create Mutual Exclusion semaphore for SNMP configuration */
  snmpSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);

  if (snmpSema == L7_NULLPTR)
    rc = L7_FAILURE;


  /* create Message Queue SNMP Traps */
  snmpTrapMessageQueue = osapiMsgQueueCreate("snmp trap queue", snmpSidMsgCountGet(), sizeof(L7_SNMP_TRAP_MESSAGE_t));

  if (snmpSema == L7_NULLPTR)
    rc = L7_FAILURE;

  if (rc == L7_SUCCESS)
  {
    /* start all SNMP tasks */
    snmpStartTasks();
  }

  return(rc);
}

/*********************************************************************
* @purpose Initialize the snmp for Phase 2
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
L7_RC_t snmpPhaseTwoInit()
{
  L7_RC_t rc = L7_SUCCESS;

  nvStoreFunctionList_t notifyFunctionList;

  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID     = L7_SNMP_COMPONENT_ID;
  notifyFunctionList.notifySave       = snmpSave;
  notifyFunctionList.hasDataChanged   = snmpHasDataChanged;
  notifyFunctionList.notifyConfigDump = snmpConfigDump;
  notifyFunctionList.resetDataChanged = snmpResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }

  SnmpTrapRegistrationBase();
  SnmpTrapRegistrationCustomer();
  SnmpTrapRegistrationSwitching();

  return rc;
}

/*********************************************************************
* @purpose Initialize the snmp for Phase 3
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
L7_RC_t snmpPhaseThreeInit()
{
  L7_RC_t rc = L7_SUCCESS;

  rc = sysapiCfgFileGet(L7_SNMP_COMPONENT_ID, SNMP_CFG_FILENAME, (L7_char8 *)&snmpCfgData, sizeof(snmpCfgData),
                 &snmpCfgData.checkSum, SNMP_CFG_VER_CURRENT, snmpBuildDefaultConfigData, snmpMigrateConfigData);

  if (rc != L7_SUCCESS )
  {
    rc = L7_FAILURE;
  }
  else
  {
    snmpCfgData.cfgHdr.dataChanged = L7_FALSE;
  }

 SnmpResetTrapTable = L7_TRUE;
 SnmpCommTableApply = L7_TRUE;

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
void snmpPhaseOneFini(void)
{
  if (snmpSema != L7_NULLPTR)
  {
    /* try to delete the Mutual Exclusion semaphore for SNMP */
    (void)osapiSemaDelete(snmpSema);
  }

  /* config data initialized to zero */
  memset((void *) &snmpCfgData, 0, sizeof(snmpCfgData_t));

}

/*********************************************************************
* @purpose Unconfigure SNMP for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.
*
* @end
*********************************************************************/
L7_RC_t snmpPhaseOneUnConfig()
{
  /* nothing to do */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Unconfigure SNMP for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.
*
* @end
*********************************************************************/
L7_RC_t snmpPhaseTwoUnConfig()
{
  /* return to default configuration */

  return snmpRestore();
}

/*********************************************************************
*
* @purpose  Waits for SNMP agent to be ready before applying configuration
*
* @notes    none
*
* @end
*********************************************************************/
void
L7_snmp_cfg_task ()
{
  L7_uint32 reset_type=0;
  L7_uint32 sleepCount=0;

  SnmpWaitUntilState(L7_SNMP_STATE_CONFIG);

#if (!L7_FEAT_SNMP_CONFAPI)
  /* create default Community table entries */
  SnmpCommunityTableUpdate();

  /* create default Trap Receiver table entries. */
  SnmpTrapMgrTableUpdate();
#endif

  SnmpInitializeTraps();

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_SUPPORTED) == L7_TRUE)
  {
      /* wait for user manager to be ready */
      while (userMgrReady() != L7_TRUE)
      {
        sleepCount += 1;
        osapiSleep(1);
      }

      if (userMgrSnmpUsersRestore() != L7_SUCCESS)
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNMP_COMPONENT_ID,
                "SNMP User restore returned failure.");
  }

  /* wait atleast a minute after starting to allow IP address to be allocated*/
  if (sleepCount < 60)
  {
    osapiSleep(60 - sleepCount);
  }

  /* allow snmp activities to occur */
  SnmpSetState(L7_SNMP_STATE_RUN);

  /* retrieve the type of reset and send out the trap */
  sysapiRegistryGet (RESET_TYPE, U32_ENTRY, &reset_type);
  if ( reset_type == L7_WARMRESET )
    usmDbTrapMgrWarmStartTrapSend(USMDB_UNIT_CURRENT, simGetThisUnit());
  else
    usmDbTrapMgrColdStartTrapSend(USMDB_UNIT_CURRENT, simGetThisUnit());

#if (!L7_FEAT_SNMP_CONFAPI)
  while (1)
  {

    /* When SNMP needs to reconfigure Users */
    if (SnmpResetConfig == L7_ENABLE)
    {
      /* wait for user manager to be ready */
      while (userMgrReady() != L7_TRUE)
      {
        osapiSleep(1);
      }

      if (userMgrSnmpConfigSet() != L7_SUCCESS)
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNMP_COMPONENT_ID,
                "SNMP User Config returned failure.");

      SnmpResetConfig = L7_DISABLE;
    }
    if (SnmpResetTrapTable == L7_TRUE)
    {
      SnmpTrapMgrTableUpdate();
      SnmpResetTrapTable = L7_FALSE;
    }

    osapiSleep(1);
  }
#endif
}

/*********************************************************************
*
* @purpose  Takes SNMP Semaphore
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpSemaTake()
{
/*  L7_uint32 temp_int;
  osapiTaskIDSelfGet(&temp_int);

  sysapiPrintf("SNMP (%x): sema TAKE\n", temp_int);*/
  return osapiSemaTake(snmpSema, L7_WAIT_FOREVER);
}

/*********************************************************************
*
* @purpose  Takes SNMP Semaphore
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpSemaGive()
{
/*  L7_uint32 temp_int;
  osapiTaskIDSelfGet(&temp_int);

  sysapiPrintf("SNMP (%x): sema GIVE\n", temp_int);*/
  return osapiSemaGive(snmpSema);
}


/*********************************************************************
*
* @purpose  SNMP Trap Task
*
* @notes    This task services SNMP Trap requests to offload work from
*           component tasks.  Will also wait until SNMP agent is ready
*           to send traps, preventing traps from not being sent.
*
* @end
*********************************************************************/
void L7_snmp_trap_task()
{
  L7_SNMP_TRAP_MESSAGE_t msg;

  /* Wait until the SNMP task is ready to send traps */
  SnmpWaitUntilRun();

  while (osapiMessageReceive(snmpTrapMessageQueue, &msg, sizeof(msg), L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    (void)snmpSemaTake();
    if (msg.func_ptr != L7_NULLPTR)
      (msg.func_ptr)(msg.trap_id, msg.pdata);

    if (msg.pdata != L7_NULLPTR)
      osapiFree(L7_SNMP_COMPONENT_ID, msg.pdata);
    (void)snmpSemaGive();
  }
}


/*********************************************************************
*
* @purpose  Puts a SNMP Trap Message on the queue
*
* @param    trap_id  - Index of trap
* @param    pdata    - pointer to trap data structure
* @param    func_ptr - function pointer of trap handler
*
* @notes    If message can not be sent, this function will free
*           the memory allocated in pdata (if not NULL) to allow
*           fire-and-forget trap sending
*
* @end
*********************************************************************/
L7_RC_t snmpTrapSend(L7_uint32 trap_id, void *pdata, void(*func_ptr)(L7_uint32, void*))
{
  L7_SNMP_TRAP_MESSAGE_t msg;
  L7_uint32 flag;

  /* ignore if traps are disabled */
  if ((SnmpEnableTrapsGet(&flag) == L7_SUCCESS) && (flag != L7_ENABLE))
  {
    return L7_SUCCESS;
  }

  /* Check for null pointers in arguments */
  if (func_ptr == L7_NULLPTR)
    return L7_FAILURE;

  /* clear the message */
  memset(&msg, 0, sizeof(msg));

  msg.trap_id = trap_id;
  msg.pdata = pdata;
  msg.func_ptr = func_ptr;

  if (osapiMessageSend(snmpTrapMessageQueue, (void*)&msg, sizeof(msg), L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    /* if message couldn't be sent, free the data allocated */
    if (pdata != L7_NULLPTR)
      osapiFree(L7_SNMP_COMPONENT_ID, pdata);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNMP_COMPONENT_ID,
            "SNMP Could not deliver Trap: %d", trap_id);
    return L7_ERROR;
  }

  return L7_SUCCESS;
}
