/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename traputil.c
*
* @purpose Trap Manager Utilities File
*
* @component trapmgr
*
* @comments none
*
* @create 08/30/2000
*
* @author bmutz
*
* @end
*
**********************************************************************/

#include <string.h>                /* for memcpy() etc... */
#include "l7_common.h"
#include "usmdb_sim_api.h"
#include "usmdb_unitmgr_api.h"
#include "osapi.h"
#include "nimapi.h"
#include "sysapi.h"
#include "log.h"
#include "nvstoreapi.h"
#include "trapapi.h"
#include "defaultconfig.h"
#include "trap.h"
#include "trapstr.h"

#include "simapi.h"
#include "usmdb_snmp_api.h"
#include "usmdb_snmp_trap_api.h"
#include "usmdb_snmp_trap_api_base.h"
#include "dtlapi.h"
#include "cardmgr_api.h"
#include "powerunitmgr_api.h"
#include "fanunitmgr_api.h"
#include "trap_inventory.h"
#include "trap_inventory_api.h"
#include "event_manager.h"
#include "lldp_api.h"
#include "usmdb_lldp_api.h"
#include "trapmgr_exports.h"
#include "snmp_api.h"
#include "dot1s_api.h"
#include "unitmgr_api.h"

trapMgrTrapData_t trapMgrTrapData;
void *trapMgrSemId;

trapMgrNotifyRequest_t *pTrapMgrNotifyRequest = L7_NULLPTR;

extern trapMgrCfgData_t trapMgrCfgData;

L7_uint32 warmStartSetTrapNeeded;
L7_uint32 coldStartSetTrapNeeded;

L7_uint32 stackNewMasterElected_oldUnit    = 0;
L7_BOOL   stackNewMasterElected_trapNeeded = L7_FALSE;
L7_uint32 stackMasterFailed_oldUnit        = 0;
L7_BOOL   stackMasterFailed_trapNeeded     = L7_FALSE;

void *trapMsgQueue;
L7_int32 trapTaskId;
L7_uint32 trapMsgQueueLostMsgs = 0;

static trapMgrFunctionList_t   trapMgrNotifyList[L7_LAST_TRAP_ID];

void trapTask();

void debugTrapDump()
{
  printf("trapMsgQueueLostMsgs == %d\n", trapMsgQueueLostMsgs);
}

/*********************************************************************
* @purpose  Saves trapmgr user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrSave(void)
{

  L7_RC_t rc=L7_SUCCESS;

  if (trapMgrCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    trapMgrCfgData.cfgHdr.dataChanged = L7_FALSE;

    trapMgrCfgData.checkSum = nvStoreCrc32((L7_char8 *)&trapMgrCfgData,
                                           sizeof(trapMgrCfgData) - sizeof(trapMgrCfgData.checkSum));
    /* call save NVStore routine */
    if (sysapiCfgFileWrite(L7_TRAPMGR_COMPONENT_ID, TRAP_CFG_FILENAME,
                           (L7_char8 *)&trapMgrCfgData,
                           sizeof(trapMgrCfgData_t)) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TRAPMGR_COMPONENT_ID,
              "Error on call to osapiFsWrite routine on config file %s\n", TRAP_CFG_FILENAME);
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Restores trapmgr user config file to factore defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrRestore(void)
{
  trapBuildDefaultConfigData(TRAP_CFG_VER_1);

  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Checks if trapmgr user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL trapMgrHasDataChanged(void)
{
  return trapMgrCfgData.cfgHdr.dataChanged;
}
void trapMgrResetDataChanged(void)
{
  trapMgrCfgData.cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Print the current Trap Manager config values to
*           serial port
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrConfigDump(void)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 buf[32];

  if (trapMgrCfgData.trapAuth == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager Authentication Flag - %s\n", buf);

  if (trapMgrCfgData.trapLink == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager Link Up/Down Flag - %s\n", buf);

  if (trapMgrCfgData.trapMultiUsers == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager Multiple Users Flag - %s\n", buf);

  if (trapMgrCfgData.trapSpanningTree == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager Spanning Tree Flag - %s\n", buf);

  if (trapMgrCfgData.trapGeneralSystem == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager General System State - %s\n", buf);

  if (trapMgrCfgData.trapDot1q == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager Dot1q State - %s\n", buf);

  if (trapMgrCfgData.trapOspf == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager OSPF State - %s\n", buf);

  if (trapMgrCfgData.trapVrrp == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager VRRP State - %s\n", buf);

  if (trapMgrCfgData.trapBgp == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager BGP State - %s\n", buf);

  if (trapMgrCfgData.trapPim == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager PIM State - %s\n", buf);

  if (trapMgrCfgData.trapDvmrp == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager DVMRP State - %s\n", buf);

  if (trapMgrCfgData.trapInventory == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager Inventory State - %s\n", buf);
  if (trapMgrCfgData.trapMacLockViolation == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager MAC Lock Violation State - %s\n", buf);

  if (trapMgrCfgData.trapAcl == L7_ENABLE)
    sprintf(buf, "%s", "L7_ENABLE");
  else
    sprintf(buf, "%s", "L7_DISABLE");
  printf("Trap Manager ACL State - %s\n", buf);

  if (trapMgrCfgData.trapCaptivePortal == L7_ENABLE)
    sprintf(buf,"%s","L7_ENABLE");
  else
    sprintf(buf,"%s","L7_DISABLE");
  printf("Trap Manager Captive Portal State - %s\n",buf);

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
void trapMgrBuildTestConfigData(void)
{
  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

  trapMgrCfgData.trapAuth = L7_ENABLE;
  trapMgrCfgData.trapLink = L7_DISABLE;
  trapMgrCfgData.trapMultiUsers = L7_ENABLE;
  trapMgrCfgData.trapSpanningTree = L7_DISABLE;
  trapMgrCfgData.trapGeneralSystem = L7_DISABLE;
  trapMgrCfgData.trapDot1q = L7_ENABLE;
  trapMgrCfgData.trapOspf = L7_DISABLE;
  trapMgrCfgData.trapVrrp = L7_ENABLE;
  trapMgrCfgData.trapBgp = L7_DISABLE;
  trapMgrCfgData.trapPim = L7_ENABLE;
  trapMgrCfgData.trapDvmrp = L7_DISABLE;
  trapMgrCfgData.trapInventory = L7_DISABLE;
  trapMgrCfgData.trapMacLockViolation = L7_ENABLE;
  trapMgrCfgData.trapAcl = L7_ENABLE;
  trapMgrCfgData.trapCaptivePortal = L7_ENABLE;
  /* End of Component's Test Non-default configuration Data */

  /* Force write of config file */
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
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
void trapMgrConfigDataTestShow(void)
{
  L7_fileHdr_t  *pFileHdr;
  L7_uint32     i;

  /*-----------------------------*/
  /* Config File Header Contents */
  /*-----------------------------*/
  pFileHdr = &trapMgrCfgData.cfgHdr;
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

  trapMgrConfigDump();

  /*-----------------------------*/
  /* Checksum                    */
  /*-----------------------------*/
  sysapiPrintf("trapMgrCfgData.checkSum : %u\n", trapMgrCfgData.checkSum);
}

/*********************************************************************
* @purpose Initialize trapMgr for Phase 1
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
L7_RC_t trapMgrPhaseOneInit()
{
  L7_RC_t rc = L7_SUCCESS;

  /* config data initialized to zero */
  memset((void *)&trapMgrCfgData, 0, sizeof(trapMgrCfgData_t));

  coldStartSetTrapNeeded = L7_FALSE;
  warmStartSetTrapNeeded = L7_FALSE;

  pTrapMgrNotifyRequest =
    (trapMgrNotifyRequest_t *)osapiMalloc(L7_TRAPMGR_COMPONENT_ID, (L7_uint32)((sizeof(trapMgrNotifyRequest_t)) *
                                                                               L7_LAST_COMPONENT_ID));
  if (pTrapMgrNotifyRequest == L7_NULLPTR)
    return L7_FAILURE;

  memset((void *)pTrapMgrNotifyRequest, 0x00,
         sizeof(trapMgrNotifyRequest_t) * L7_LAST_COMPONENT_ID);

  trapMgrSemId = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (trapMgrSemId == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_TRAPMGR_COMPONENT_ID, "Error allocating trapMgrSemId");
    return L7_FAILURE;
  }

  osapiSemaTake(trapMgrSemId, L7_WAIT_FOREVER);
  trapMgrTrapData.currentAmountOfTraps   = 0;
  trapMgrTrapData.lastReceivedTrap            = 0;
  trapMgrTrapData.amountOfUnDisplayedTraps = 0;
  trapMgrTrapData.totalAmountOfTraps   = 0;
  osapiSemaGive(trapMgrSemId);

  trapMsgQueue = osapiMsgQueueCreate("trapQueue", L7_TRAP_MSG_QUEUE_SIZE, sizeof(L7_TRAP_t));
  trapTaskId = osapiTaskCreate("trapTask", (void *)trapTask,
                                   0, 0, L7_DEFAULT_STACK_SIZE,
                                   L7_DEFAULT_TASK_PRIORITY,
                                   L7_DEFAULT_TASK_SLICE);

  return(rc);
}

/*********************************************************************
* @purpose Initialize the trapMgr for Phase 2
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
L7_RC_t trapMgrPhaseTwoInit()
{
  L7_RC_t rc;
  nvStoreFunctionList_t notifyFunctionList;

  memset((void *)&notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_TRAPMGR_COMPONENT_ID;
  notifyFunctionList.notifySave     = trapMgrSave;
  notifyFunctionList.hasDataChanged = trapMgrHasDataChanged;
  notifyFunctionList.notifyConfigDump     = trapMgrConfigDump;
  notifyFunctionList.resetDataChanged = trapMgrResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TRAPMGR_COMPONENT_ID,
            "trapMgrPhaseTwoInit(): Could not register with nvStore\n");
  }

  /* register with card manager for card event change */
  rc = cmgrAllTrapsRegister(L7_TRAPMGR_COMPONENT_ID, trapMgrCardEventCallBack);
  if (rc == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TRAPMGR_COMPONENT_ID,
            "TRAPMGR: Failed to register with CARDMGR\n");
  }

  return rc;
}

/*********************************************************************
* @purpose Initialize the trapMgr for Phase 3
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
L7_RC_t trapMgrPhaseThreeInit()
{
  L7_RC_t rc;

  rc = sysapiCfgFileGet(L7_TRAPMGR_COMPONENT_ID, TRAP_CFG_FILENAME,
                        (L7_char8 *)&trapMgrCfgData, sizeof(trapMgrCfgData),
                        &trapMgrCfgData.checkSum, TRAP_CFG_VER_CURRENT,
                        trapBuildDefaultConfigData, trapMigrateConfigData);
  if (rc != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    rc = trapMgrApplyConfigData();
    trapMgrCfgData.cfgHdr.dataChanged = L7_FALSE;
  }

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
void trapMgrPhaseOneFini(void)
{
  if (trapMgrSemId != L7_NULLPTR)
  {
    /* try to delete the Mutual Exclusion semaphore for trapMgr */
    (void)osapiSemaDelete(trapMgrSemId);
  }

  /* config data initialized to zero */
  memset((void *)&trapMgrCfgData, 0, sizeof(trapMgrCfgData_t));
}

/*********************************************************************
* @purpose Unconfigure trapMgr for Phase 1
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
L7_RC_t trapMgrPhaseOneUnConfig()
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Unconfigure trapMgr for Phase 1
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
L7_RC_t trapMgrPhaseTwoUnConfig()
{
  /* return to stored configuration */
  return trapMgrRestore();
}

/*********************************************************************
* @purpose Called when trapMgr goes into MGMT Ready State
*
* @param   void
*
* @returns nothing
*
* @notes  none
*
* @end
*********************************************************************/
void trapMgrUsmDbReadyCallback()
{
  L7_uint32 unit = usmDbThisUnitGet();

  /* push AuthenTrap configuration to SNMP agent */
  usmDbSnmpEnableAuthenTrapsSet(unit, trapMgrCfgData.trapAuth);
}

/*********************************************************************
* @purpose  Build trap system uptime string
*
* @param    time        system time
* @param    buf         buffer to build system uptime string
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrGetTimeString(L7_timespec time, L7_char8 *buf)
{
  sprintf(buf, "%d days %02d:%02d:%02d", time.days, time.hours, time.minutes, time.seconds);
}

/*********************************************************************
* @purpose  Add a trap to the trap log
*
* @param    trapStringBuf    Message to log
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrLogTrapToLocalLog(L7_char8 *trapStringBuf)
{
  L7_timespec ts;
  L7_uchar8 timeStringBuf[TRAPMGR_TIME_STR_SIZE];

  osapiUpTime(&ts);
  trapMgrGetTimeString(ts, timeStringBuf);

  osapiSemaTake(trapMgrSemId, L7_WAIT_FOREVER);

  trapMgrTrapData.amountOfUnDisplayedTraps += 1;
  trapMgrTrapData.totalAmountOfTraps   += 1;

  if (trapMgrTrapData.currentAmountOfTraps > 0)
    trapMgrTrapData.lastReceivedTrap = (trapMgrTrapData.lastReceivedTrap + 1) % TRAPMGR_TABLE_SIZE;
  trapMgrTrapData.currentAmountOfTraps   += 1;

  strcpy(trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].timestamp, timeStringBuf);
  osapiStrncpySafe(trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].message,
                   trapStringBuf,
                   TRAPMGR_MSG_SIZE);

  /* put trap message onto the log */
  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_TRAPMGR_COMPONENT_ID, trapStringBuf);

  osapiSemaGive(trapMgrSemId);
}

/*********************************************************************
 * @purpose  Register a trap SNMP send routine
 *
 * @param    trapMgrFunctionList   pointer to structure with function pointer
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t trapMgrRegister(trapMgrFunctionList_t *trapMgrFunctionList) {

  if (trapMgrFunctionList->registrar_ID >= L7_LAST_TRAP_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TRAPMGR_COMPONENT_ID,
            "trapMgr registrar_ID %d greater then L7_LAST_TRAP_ID\n",
           (L7_int32)trapMgrFunctionList->registrar_ID);
    return(L7_FAILURE);
  }

  if (trapMgrNotifyList[trapMgrFunctionList->registrar_ID].registrar_ID != 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_TRAPMGR_COMPONENT_ID,
            "trapMgr registrar_ID %d already registered. Re-registering\n", 
            (L7_int32)trapMgrFunctionList->registrar_ID);
  }

  memcpy(&trapMgrNotifyList[trapMgrFunctionList->registrar_ID], trapMgrFunctionList,
         sizeof(trapMgrFunctionList_t));

  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Deregister a trap from trapMgr
 *
 * @param    registrar_ID     one of L7_TRAP_ID_t
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t trapMgrDeregister(L7_TRAP_ID_t registrar_ID)
{
  if (registrar_ID >= L7_LAST_TRAP_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TRAPMGR_COMPONENT_ID,
            "trapMgr registrar_ID %d greater then L7_LAST_TRAP_ID\n", registrar_ID);
    return(L7_FAILURE);
  }

  if (trapMgrNotifyList[registrar_ID].registrar_ID == 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TRAPMGR_COMPONENT_ID,
            "trapMgr registrar_ID %d not registered\n", registrar_ID);
    return(L7_FAILURE);
  }

  memset(&trapMgrNotifyList[registrar_ID],0, sizeof(trapMgrFunctionList_t));

  return(L7_SUCCESS);
}

void trapTask()
{
  L7_TRAP_t trap;
  L7_RC_t rc;
/*  osapiTimerDescr_t *pTimerHolder;*/
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uchar8 ifTrunkName[L7_NIM_IFNAME_SIZE + 1];
  L7_uint32 trapDelay = 0;

#ifdef L7_SNMP_PACKAGE
  /* wait until SNMP is ready */
  SnmpWaitUntilState(L7_SNMP_STATE_RUN);
#endif

  while (1)
  {
    rc = osapiMessageReceive(trapMsgQueue, (void *)&trap, sizeof(trap), L7_WAIT_FOREVER);
    if (rc == L7_SUCCESS)
    {
#ifdef L7_SNMP_PACKAGE
      if (trapMgrNotifyList[trap.trapId].registrar_ID != 0 &&
          trapMgrNotifyList[trap.trapId].sendTrap != L7_NULLPTR)
      {
        snmpSemaTake();
        if (SnmpStatusGet() == L7_ENABLE &&
            (*trapMgrNotifyList[trap.trapId].sendTrap)(&trap) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TRAPMGR_COMPONENT_ID,
                  "bad rc on Send Trap call to registrar_ID %d\n", (L7_int32)trapMgrNotifyList[trap.trapId].registrar_ID);
        }
        snmpSemaGive();
      }
      else {
        /* If SNMP is present and doesn't define a trap handler for this trap,
           don't display it in the trap log. */
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_TRAPMGR_COMPONENT_ID, "Unhandled Trap (%d): (gen=%d spec=%d)", 
                trap.trapId, trap.gen, trap.spec);
        continue;
      }
#endif
      trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = trap.gen;
      trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = trap.spec;

      trapDelay = 0;
      memset(trapStringBuf, 0, sizeof(trapStringBuf));
      memset(ifName, 0, sizeof(ifName));
      memset(ifTrunkName, 0, sizeof(ifTrunkName));
      switch (trap.trapId)
      {
        case TRAP_CONFIG_CHANGED:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), configChanged_str);
          break;
        case TRAP_END_TFTP:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), tftpEnd_str, trap.u.endTftp.fName, trap.u.endTftp.exitCode);
          break;
        case TRAP_ABORT_TFTP:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), tftpAbort_str, trap.u.abortTftp.fName, trap.u.abortTftp.exitCode);
          break;
        case TRAP_START_TFTP:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), tftpStart_str, trap.u.startTftp.fName);
          break;
        case TRAP_LINK_FAILURE:
          nimGetIntfName(trap.u.linkStatus.intIfNum, L7_SYSNAME, ifName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), linkFailure_str, ifName);
          break;
        case TRAP_VLAN_DYN_PORT_ADDED:
          nimGetIntfName(trap.u.vlanDynPortAdded.port, L7_SYSNAME, ifName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), vlanDynPortAdded_str, ifName, trap.u.vlanDynPortAdded.dot1qVlanIndex);
          break;
        case TRAP_VLAN_DYN_PORT_REMOVED:
          nimGetIntfName(trap.u.vlanDynPortRemoved.port, L7_SYSNAME, ifName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), vlanDynPortRemoved_str, ifName, trap.u.vlanDynPortRemoved.dot1qVlanIndex);
          break;
        case TRAP_STACK_MASTER_FAILED:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), stackMasterFailed_str,
                        trap.u.stackMasterFailed.oldUnitNumber);
          break;
        case TRAP_STACK_NEW_MASTER_ELECTED:
          if (usmDbUnitMgrNumberGet(&unit) != L7_SUCCESS)
            unit = 0;
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), stackNewMasterElected_str, unit, trap.u.stackNewMasterElected.oldUnitNumber);
          /* for this trap, wait 60 seconds to give the system a chance to get network
             connectivity to the trap receiver */
/*          osapiTimerAdd(trapMgrStackNewMasterElectedLogTrapSend, trap.u.stackNewMasterElected.oldUnitNumber, L7_NULL, 60000, &pTimerHolder);*/
          break;
        case TRAP_STACK_NEW_MEMBER_UNIT_ADDED:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), stackMemberUnitAdded_str, trap.u.stackNewMemberUnitAdded.newUnitNumber);
          break;
        case TRAP_STACK_MEMBER_UNIT_REMOVED:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), stackMemberUnitRemoved_str, trap.u.stackMemberUnitRemoved.removedUnitNumber);
          break;
        case TRAP_STACK_LINK_FAILED:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), stackLinkFailed_str, trap.u.stackLinkFailed.identifiedUnit, trap.u.stackLinkFailed.info);
          break;
#ifdef L7_STACKING_PACKAGE
        case TRAP_STACK_RESTART_COMPLETE:
          {
            L7_uchar8 resTxt[L7_STARTUP_REASON_STR_LEN];
            unitMgrStartupReasonStrGet(trap.u.stackRestartComplete.reason,
                                       resTxt);
            osapiSnprintf(trapStringBuf, sizeof(trapStringBuf),
                          stackRestartComplete_str, resTxt,
                          trap.u.stackRestartComplete.unitId);
          }
          break;
#endif
        case TRAP_DOT1D_STP_PORTSTATE_FWD:
          nimGetIntfName(trap.u.dot1dStpPortStateForwarding.port, L7_SYSNAME, ifName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), dot1dStpPortStateForwarding_str, ifName, trap.u.dot1dStpPortStateForwarding.instanceIdx);
          break;
        case TRAP_DOT1D_STP_PORTSTATE_NOT_FWD:
          nimGetIntfName(trap.u.dot1dStpPortStateNotForwarding.port, L7_SYSNAME, ifName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), dot1dStpPortStateNotForwarding_str, ifName, trap.u.dot1dStpPortStateNotForwarding.instanceIdx);
          break;
        case TRAP_TRUNK_PORT_ADDED:
          nimGetIntfName(trap.u.trunkPortAddedTrap.port, L7_SYSNAME, ifName);
          nimGetIntfName(trap.u.trunkPortAddedTrap.trunkIfIndex, L7_SYSNAME, ifTrunkName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), trunkPortAdded_str, ifName, ifTrunkName);
          break;
        case TRAP_TRUNK_PORT_REMOVED:
          nimGetIntfName(trap.u.trunkPortRemovedTrap.port, L7_SYSNAME, ifName);
          nimGetIntfName(trap.u.trunkPortRemovedTrap.trunkIfIndex, L7_SYSNAME, ifTrunkName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), trunkPortRemoved_str, ifName, ifTrunkName);
          break;
        case TRAP_LOCK_PORT:
          nimGetIntfName(trap.u.lockPortTrap.port, L7_SYSNAME, ifName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), lockPort_str, ifName,
                        trap.u.lockPortTrap.macAddr.addr[0],trap.u.lockPortTrap.macAddr.addr[1],trap.u.lockPortTrap.macAddr.addr[2],
                        trap.u.lockPortTrap.macAddr.addr[3],trap.u.lockPortTrap.macAddr.addr[4],trap.u.lockPortTrap.macAddr.addr[5]);
          break;
        case TRAP_VLAN_DYN_VLAN_ADDED:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), vlanDynVlanAdded_str, trap.u.vlanDynVlanAdded.dot1qVlanIndex);
          break;
        case TRAP_VLAN_DYN_VLAN_REMOVED:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), vlanDynVlanRemoved_str, trap.u.vlanDynVlanRemoved.dot1qVlanIndex);
          break;

        case TRAP_ENV_MON_FAN_STATE_CHANGE:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), envMonFanStateChange_str, trap.u.envMonFanStateChange.fanIndex,
                        trap.u.envMonFanStateChange.trap_unit, trap.u.envMonFanStateChange.status);
          break;

        case TRAP_ENV_MON_POWER_SUPPLY_STATE_CHANGE:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), envMonPowerSupplyStateChange_str,
                        trap.u.envMonPowerSupplyStateChange.envMonSupplyIndex, trap.u.envMonPowerSupplyStateChange.trap_unit,
                        trap.u.envMonPowerSupplyStateChange.status);
          break;
        case TRAP_ENV_MON_TEMP_RISING:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), envMonTemperatureRisingAlarm_str, trap.u.envMonTemperatureRisingAlarm.unit);
          break;
        case TRAP_COPY_FINISHED:
          if (usmDbUnitMgrNumberGet(&unit) != L7_SUCCESS)
            unit = 0;
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), copyFinished_str, unit);
          break;
        case TRAP_COPY_FAILED:
          if (usmDbUnitMgrNumberGet(&unit) != L7_SUCCESS)
            unit = 0;
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), copyFailed_str, unit);
          break;
        case TRAP_DOT1X_PORT_STATUS_AUTHORIZED:
          nimGetIntfName(trap.u.dot1xPortStatusAuthorizedTrap.port, L7_SYSNAME, ifName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), dot1xPortStatusAuthorized_str, ifName);
          break;
        case TRAP_DOT1X_PORT_STATUS_UNAUTHORIZED:
          nimGetIntfName(trap.u.dot1xPortStatusUnauthorizedTrap.port, L7_SYSNAME, ifName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), dot1xPortStatusUnauthorized_str, ifName);
          break;
        case TRAP_STP_ELECTED_AS_ROOT:
          if (usmDbUnitMgrNumberGet(&unit) != L7_SUCCESS)
            unit = 0;
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), stpElectedAsRoot_str, unit);
          break;
        case TRAP_STP_NEW_ROOT_ELECTED:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), stpNewRootElected_str, trap.u.stpNewRootElected.instanceID,
            *((L7_ushort16 *)&trap.u.stpNewRootElected.rootId[0]),
            *((L7_ushort16 *)&trap.u.stpNewRootElected.rootId[2]),
            *((L7_ushort16 *)&trap.u.stpNewRootElected.rootId[4]),
            *((L7_ushort16 *)&trap.u.stpNewRootElected.rootId[6]));
          break;
        case TRAP_INVALID_USER_LOGIN_ATTEMPTED:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), invalidUserLoginAttempted_str, trap.u.invalidUserLoginAttempted.uiMode,
                  trap.u.invalidUserLoginAttempted.fromIpAddress);
          break;
        case TRAP_MGMT_ACL_VIOLATION:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), managementACLViolation_str, trap.u.managementACLViolation.uiMode,
                        trap.u.managementACLViolation.fromIpAddress);
          break;
        case TRAP_SFP_INSERTION_REMOVAL:
          if (trap.u.sfpInsertionRemoval.status == L7_TRUE)
          {
            osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), sfpInserted_str, trap.u.sfpInsertionRemoval.Unit, trap.u.sfpInsertionRemoval.intIfNum);
          }
          else
          {
            osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), sfpRemoved_str, trap.u.sfpInsertionRemoval.Unit, trap.u.sfpInsertionRemoval.intIfNum);
          }
          break;
        case TRAP_XFP_INSERTION_REMOVAL:
          if (trap.u.xfpInsertionRemoval.status == L7_TRUE)
          {
            osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), xfpInserted_str, trap.u.xfpInsertionRemoval.Unit, trap.u.xfpInsertionRemoval.intIfNum);
          }
          else
          {
            osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), xfpRemoved_str, trap.u.xfpInsertionRemoval.Unit, trap.u.xfpInsertionRemoval.intIfNum);
          }
          break;
        case TRAP_EDB_CONFIG_CHANGE:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), entConfigChanged_str);
          break;
        case TRAP_LINK_DOWN:
          nimGetIntfName(trap.u.linkStatus.intIfNum, L7_SYSNAME, ifName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), linkdown_str, ifName);
          break;
        case TRAP_LINK_UP:
          nimGetIntfName(trap.u.linkStatus.intIfNum, L7_SYSNAME, ifName);
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), linkup_str, ifName);
          break;
        case TRAP_CPU_RISING_THRESHOLD:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), cpuRisingThreshold_str, trap.u.cpuUtil.threshold, trap.u.cpuUtil.buf);
          break;
        case TRAP_CPU_FALLING_THRESHOLD:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), cpuFallingThreshold_str, trap.u.cpuUtil.threshold);
          break;
        case TRAP_STACK_FIRMWARE_SYNC_START:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), sfsSyncStart_str, trap.u.stackFirmwareSync.stackMember);
          break;
        case TRAP_STACK_FIRMWARE_SYNC_FAIL:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), sfsSyncFailed_str, trap.u.stackFirmwareSync.stackMember);
          break;
        case TRAP_STACK_FIRMWARE_SYNC_FINISH:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), sfsSyncFinish_str, trap.u.stackFirmwareSync.stackMember);
          break;
        default:
          osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), "Unknown Trap %d", trap.trapId);
          break;
      }
      trapMgrLogTrapToLocalLog(trapStringBuf);
    }
  }
}


/*********************************************************************
*
* @purpose  This function process the temperature event trap
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    itemNum  @b((input)) temperature sensor index
* @param    tempEventType @b((input)) type of temperature event
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrTemperatureChange(L7_uint32 UnitIndex, L7_uint32 itemNum, L7_uint32 tempEventType)
{
    L7_RC_t rc = L7_SUCCESS;
    L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
    L7_uint32 unit;

    unit = usmDbThisUnitGet();

    osapiSnprintf(trapStringBuf,TRAPMGR_MSG_SIZE, temperature_change_alarm_str, itemNum, tempEventType);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
       rc = usmDbSnmpTemperatureChangeTrapSend(itemNum,tempEventType);
    return rc;
}


/*********************************************************************
*
* @purpose  This function process the fan state change event trap
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    itemNum  @b((input)) fan index
* @param    fanEventType @b((input)) type of fan event
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrFanStateChange(L7_uint32 UnitIndex, L7_uint32 itemNum, L7_uint32 fanEventType)
{
    L7_RC_t rc = L7_SUCCESS;
    L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
    L7_uint32 unit;

    unit = usmDbThisUnitGet();

    osapiSnprintf(trapStringBuf,TRAPMGR_MSG_SIZE, fan_state_change_alarm_str, itemNum, fanEventType);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
       rc = usmDbSnmpFanStateChangeTrapSend(itemNum,fanEventType);
    return rc;
}


/*********************************************************************
*
* @purpose  This function process the power supply state change event trap
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    itemNum  @b((input)) power supply  index
* @param    powSupplyEventType @b((input)) type of power supply event
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrPowSupplyStateChange(L7_uint32 UnitIndex, L7_uint32 itemNum, L7_uint32 powSupplyEventType)
{
    L7_RC_t rc = L7_SUCCESS;
    L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
    L7_uint32 unit;

    unit = usmDbThisUnitGet();

    osapiSnprintf(trapStringBuf,TRAPMGR_MSG_SIZE,powsupply_state_change_alarm_str, itemNum, powSupplyEventType);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
       rc = usmDbSnmpPowSupplyStateChangeTrapSend(itemNum,powSupplyEventType);
    return rc;
}
#ifdef FEAT_METRO_CPE_V1_0
/*********************************************************************
*
* @purpose  Issue No Startup Config trap
*
* @param    none
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrNoStartupConfigLogTrap()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];

  sprintf(trapStringBuf, "%s", no_startup_config_str);

  trapMgrLogTrapToLocalLog(trapStringBuf);

  unit = usmDbThisUnitGet();
  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    rc = usmDbSnmpNoStartupConfigTrapSend(unit);

  return rc;
}
#endif

