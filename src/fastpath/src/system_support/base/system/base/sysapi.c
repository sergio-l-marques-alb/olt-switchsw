/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename  sysapi.c
 *
 * @purpose   Provide system-wide support routines
 *
 * @component sysapi
 *
 * @create    08/03/2000
 *
 * @author    paulq
 * @end
 *
 *********************************************************************/
#include <string.h>                /* for memcpy() etc... */
#include <stdarg.h>                /* for va_start, etc... */
#include "l7_common.h"
#include "l7_product.h"
#include "registry.h"
#include "osapi.h"
#include "log_api.h"
#include "log.h"
#include "log_ext.h"
#include "sysapi.h"
#include "cnfgr.h"
#include "default_cnfgr.h"
#include "nvstoreapi.h"
#include "async_event_api.h"
#include "statsapi.h"
#include "sysapi_hpc.h"
#include "l7_cnfgr_api.h"
#include "bspapi.h"
#include "zlib.h"
#include "sysapi_util.h"
#include "sysnet_api.h"
#include "cli_web_mgr_api.h"
#include "usmdb_util_api.h"
#include "trace_api.h"
#include "cli_web_user_mgmt.h"
#include "sdm_api.h"

#define  SYSAPI_FILE_BUF_SIZE   1024

extern L7_RC_t maskUtilInit (void);
extern L7_RC_t sysapiCfgFileRel4_0Separate (L7_char8 * bigCfn);
extern L7_RC_t sysapiCfgFileUncombinedToCombinedCfgFileConvert (void);
extern int  atoi (const L7_char8 *__s);
extern L7_RC_t sysapiSupportCfgFileSysInit(void);
extern L7_RC_t sysapiDebugSupportInit(void);

#ifdef L7_ROUTING_PACKAGE
extern L7_RC_t routingStart(void);
#endif

/*
 * Text Configuration Support
 */
static void sysapiTxtCfgInit(void);
static L7_BOOL sysapiValidateCfgFileName(L7_char8 *filename);

/* Binary Config Files */
static L7_char8 sysapiBinCfgFileList[][L7_MAX_FILENAME] =
{
  "dhcpsLeases.cfg",
  "wireless_nv.cfg",
  "dsDb.cfg",
  "cp_images.cfg"
};

/* Text Config Flow Controls */
static L7_BOOL sysapiTxtCfgExists = L7_FALSE;
static L7_BOOL sysapiTxtCfgEnable = L7_FALSE;
static L7_BOOL sysapiTxtCfgValid = L7_FALSE;

/*************************************
 * Mbuf Queue declarations
 *************************************/
L7_uint32 *pMbufQTop;      /* top of queue */
L7_uint32 *pMbufQBot;      /* bottom of queue */
L7_uint32 *MbufQHead;
L7_uint32 *MbufQTail;
L7_uint32 MbufsFree;
L7_uint32 MbufsRxUsed;
L7_uint32 MbufsMaxFree;
void      *MbufSema;
void      *pMbufPool;

#ifdef MBUF_HISTORY
  mbuf_history_t *mbufHistory;
  L7_uint32 historyIndex = 0;
  L7_uint32 mbuf_history_size = 0;
#endif

/*  System config mgmt parms */
SYSAPI_DIRECTORY_CFG_t *pSysapiCfgTree[SYSAPI_MAX_FILE_ENTRIES];
void * sysapiCfgSema = NULL;

/* Static Declarations */

static L7_int32 sysapiTimerTaskID = L7_NULL;
static void *sysapiWriteToFlashSema = L7_NULL;
static L7_SAVE_CONFIG_CODE_t savingConfigToFlashStatus=L7_SAVE_CONFIG_CODE_NONE;

static L7_RC_t sysapiCfgFileSeparate(L7_char8 *cfgFile);
static L7_RC_t sysapiCfgFileSeparateFilesV1(L7_int32 file_desc, L7_int32 total_length);
static L7_RC_t sysapiCfgFileSeparateFilesV2(L7_int32 file_desc, L7_int32 total_length);
static L7_RC_t sysapiCfgTreeUpdate(L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
    L7_char8 *buffer, L7_uint32 nbytes,
    SYSAPI_DIRECTORY_CFG_t *pCfgTreeEntry);

#ifdef MBUF_HISTORY
void mbufHistoryIndexInc(void)
{
  historyIndex++;
  if (historyIndex >= mbuf_history_size)
    historyIndex = 0;
}
#endif
L7_BOOL clearConfig = L7_FALSE;

/**************************************************************************
 *
 * @purpose  Start various system services.
 *
 * @param    none
 *
 * @returns  none.
 *
 * @notes This function must be envoked from bootos.c before
 *        the configurator is started.
 *
 * @end
 *
 *************************************************************************/
void sysapiInit (void (*box_reset)(void))
{
  L7_RC_t rc;

  /* Register SW & OS versions */
  osapiSwVersionRegister();
  osapiOSVersionRegister();

  /* Initialize SDM templates and read next active template ID from 
   * persistent storage. Needs to be done after the file system is
   * initialized. */
  if (sdmTemplateMgrInit() != L7_SUCCESS)
  {
    printf("Failed to initialize the SDM template manager.\n\n");
  }

  (void)sysapiSystemInit();
  rc = asyncEventHandlerInit();
  if (rc != L7_SUCCESS)
  {
    printf("%s", "Event Handler Initialization failed.\n\n");
  }

  /*------------------------------------------------------------------*/
  /* Initialize logging subsystem.
     Log_Init() - Initializes logging extensions, saving a limited
     set of event data across reboots.

     logIPLPersistentInit() - allows writes to syslog persistent log
     loggingInit() - initializes the syslog infrastructure
   */
  /*------------------------------------------------------------------*/

  /* Initialize extended logging */
  Log_Init (EVENTLOG_MAX_SIZE, box_reset, LOG_ENABLE_FLASH, LOG_DISABLE_TTY, LOG_ENABLE_RAM);

  /* Allow writes to persistent log at this point. Logging need not be up yet. */
  loggingInit();

  (void)sysapiCfgFileSysInit();

  if (statsCfgInit() != L7_SUCCESS)
  {
    printf("Stats initialization failed\n");
    LOG_ERROR(876);
  }

  if ( sysapiIfNetInit() != L7_SUCCESS )
    printf("%s:%u: sysapiIfNetInit() failed\n", __FILE__, __LINE__);

  /*
   * This needs to happen before the system starts running since sysNet is not
   * a component and is not initialized by cnfgr.
   */
  if ( sysNetPduHooksInit() != L7_SUCCESS )
  {
    printf( "%s:%u: sysNetPduHooksInit() failed\n", __FILE__, __LINE__ );
  }

  /* initialize the debugging subsystem infrastructure support */
  (void)sysapiDebugSupportInit();
  (void)sysapiSupportCfgFileSysInit();

#ifdef L7_ROUTING_PACKAGE
  /* Need a single place to initialize the routing infrastructure before
   * phase 1 init of the routing components. This includes allocation of
   * the routing heap. */
  if (routingStart() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_IP_MAP_COMPONENT_ID,
        "Failed to initialize the routing infrastructure.");
    LOG_ERROR(0);
  }
#endif

  osapiMiscInit();

  sysTraceProfileMgrInit();
}

/**************************************************************************
 *
 * @purpose  Task that creates the application timer function
 *
 * @param    none
 *
 * @returns  none.
 *
 * @notes If the task is already created, just return.
 *
 * @end
 *
 *************************************************************************/
void sysapiTimerTaskStart(void)
{
  if ( sysapiTimerTaskID != L7_NULL )
  {
    return;
  }
  sysapiTimerTaskID = osapiTaskCreate(   "osapiTimer",
      (void *)osapiTimerHandler,
      0,
      L7_NULLPTR,
      L7_DEFAULT_STACK_SIZE,
#ifdef _L7_OS_LINUX_
      L7_MEDIUM_TASK_PRIORITY,
#else
      L7_MEDIUM_TASK_PRIORITY - 1 /* Slightly lower than nim and dtl */,
#endif
      L7_DEFAULT_TASK_SLICE);

  /* Wait for osapiTimer task to finish initialization.*/
  if (osapiWaitForTaskInit(L7_OSAPI_TIMER_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "In routine %s line %d, osapiWaitForTaskInit failed\n",
        __FUNCTION__, __LINE__);
    LOG_ERROR(0);
  }
  return;
}

/**************************************************************************
 *
 * @purpose  Print to console and enabled sessions
 *
 * @param    standard "printf" format string
 *
 * @returns  L7_SUCCESS
 *
 * @notes
 *
 * @end
 *
 *************************************************************************/
void sysapiPrintf(const L7_char8 *format, ...)
{
  va_list arg;

  if ( osapiIntContext() == L7_FALSE ) {

    va_start (arg, format);
    vprintf(format, arg);
    va_end (arg);
  }

  return;
}


/**************************************************************************
 * @purpose  Initialize the sysapi component
 *
 * @param    none
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @notes
 *
 * @end
 *
 *************************************************************************/
L7_RC_t sysapiSystemInit(void)
{
  L7_int32 i;
  L7_uint32 temp32;
  L7_uint32 phy_size = 0;
  L7_uint32 mtu_size = 0;

  /* initialize system timers */
  sysapiTimerTaskStart();

  /* Initialize HPC
   */
  (void)sysapiHpcInit();

  /***********************************
   * Allocate the "mbuf" memory pool
   ***********************************/
  if (sysapiRegistryGet(MTU_SIZE, U32_ENTRY,  (void *) &mtu_size) != L7_SUCCESS )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "sysapiRegistryGet failed");
  }
  if ( sysapiRegistryGet(PHYS_SIZE, U32_ENTRY, (void *) &phy_size) != L7_SUCCESS )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "sysapiRegistryGet failed");
  }

  /* Reserve extra space for control overhead. */
  phy_size += (sizeof(SYSAPI_NET_MBUF_HEADER_t) + NET_MBUF_START_OFFSET + 64);

  temp32 = phy_size + mtu_size + SYSAPI_PKT_BUF_ALIGN_LEN;

  if (sysapiRegistryPut( L7_MBUF_SIZE,  U32_ENTRY, (void *) &temp32) != L7_SUCCESS )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "sysapiRegistryPut failed");
    return(L7_ERROR);
  }

  pMbufPool = osapiMalloc ( L7_SIM_COMPONENT_ID, L7_MAX_NETWORK_BUFF_PER_BOX * ( temp32 ) );
  if ( pMbufPool == L7_NULLPTR )
    return(L7_ERROR);

#ifdef MBUF_HISTORY
  mbufHistory = NULL;    /* Allocated on demand */
#endif

  /********************************************************
   * Allocate the "mbuf" Queue. Each entry is a 32 bit ptr
   *********************************************************/
  pMbufQTop = ( L7_uint32 * )osapiMalloc ( L7_SIM_COMPONENT_ID, L7_MAX_NETWORK_BUFF_PER_BOX * 4 );
  if ( pMbufQTop == L7_NULLPTR )
    return(L7_ERROR);

  /***************************************************
   * Initialize the "mbuf" Queue and counter.
   ****************************************************/
  MbufQHead = pMbufQTop;
  MbufQTail = pMbufQTop;
  MbufsMaxFree = L7_MAX_NETWORK_BUFF_PER_BOX;
  MbufsFree = MbufsMaxFree;
  MbufsRxUsed = 0;

  for ( i=0;i<(L7_int32)MbufsFree;i++ )
  {
    *MbufQHead = ( L7_uint32 ) ( (L7_uchar8 *)pMbufPool + i * ( temp32 ));
    MbufQHead++;
  }
  pMbufQBot = --MbufQHead;            /* set bottom of queue ptr */
  MbufQHead = pMbufQTop;              /* reset head ptr to top */

  /***********************************************************************
   * Create a binary semaphore that is initially full for controlling
   * access to global mbuf vars. Tasks blocked on semaphore wait
   * in priority order.
   ***********************************************************************/
  MbufSema = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY,
      OSAPI_SEM_FULL);

#ifndef L7_TRACE_ENABLED
  /* Start the CPU Utilization monitor task, if tracing is not enabled. */
  /* This will avoid problems with the tools attempting to use the Aux  */
  /* clock while the CPU utilization is also using it.                  */
  sysapiCpuUtilTaskStart();
#endif

  return(L7_SUCCESS);
}


/**************************************************************************
 *
 * @purpose  Task to save configuration data when request is from SNMP
 *
 * @param    None
 *
 * @returns  Never returns
 *
 * @notes
 *
 * @end
 *
 *************************************************************************/
void sysapiWriteConfigToFlashTask()
{
  sysapiWriteToFlashSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);

  while(L7_TRUE)
  {
    if (osapiSemaTake(sysapiWriteToFlashSema, L7_WAIT_FOREVER) == L7_SUCCESS)
    {
      savingConfigToFlashStatus = L7_SAVE_CONFIG_CODE_STARTING;

      if (nvStoreSave(L7_SAVE_CONFIG_TYPE_STARTUP) == L7_SUCCESS)
      {
        savingConfigToFlashStatus = L7_SAVE_CONFIG_CODE_SUCCESS;
      }
      else
      {
        savingConfigToFlashStatus = L7_SAVE_CONFIG_CODE_FAILURE;
      }
    }
  }
}

/**************************************************************************
 *
 * @purpose  Status for save configuration command when request is from SNMP
 *
 * @param    None
 *
 * @returns  Returns enum from L7_SAVE_CONFIG_CODE_t
 *
 * @notes
 *
 * @end
 *
 *************************************************************************/
L7_SAVE_CONFIG_CODE_t sysapiWriteConfigToFlashStatus()
{
  return savingConfigToFlashStatus;
}

/**************************************************************************
 *
 * @purpose  Function to start save configuration command when request is from SNMP
 *
 * @param    None
 *
 * @returns  None
 *
 * @notes
 *
 * @end
 *
 *************************************************************************/
void sysapiWriteConfigToFlashStart()
{
  if (sysapiWriteToFlashSema != L7_NULL)
    (void)osapiSemaGive(sysapiWriteToFlashSema);

  return;
}

/*********************************************************************
 *
 * @purpose Malloc space for all the filenames in the SYSAPI_DIRECTORY_t tree
 *
 * @param *tree Pointer to the directory listing structure
 *
 * @returns L7_SUCCESS
 * @returns L7_ERROR
 *
 * @end
 *
 *********************************************************************/
L7_RC_t sysapiTreeMalloc(SYSAPI_DIRECTORY_t *tree)
{
  L7_uint32 i;
  L7_RC_t   rc = L7_SUCCESS;

  for ( i = 0; i < (L7_uint32)SYSAPI_MAX_FILE_ENTRIES; i++ )
  {
    tree[i].filename = osapiMalloc(L7_SIM_COMPONENT_ID, L7_MAX_FILENAME);
    if ( tree[i].filename != L7_NULLPTR )
    {
      memset(tree[i].filename, 0, L7_MAX_FILENAME);
    }
    else
    {
      rc = L7_ERROR;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "osapiMalloc() failed");
      break;
    }
  }
  return(rc);
}


/*********************************************************************
 *
 * @purpose Free space of all the filenames in the SYSAPI_DIRECTORY_t tree
 *
 * @param *tree Pointer to the directory listing structure
 *
 * @returns nothing
 *
 * @end
 *
 *********************************************************************/
void sysapiTreeFree(SYSAPI_DIRECTORY_t *tree)
{
  L7_uint32 i;

  for ( i = 0; i < (L7_uint32)SYSAPI_MAX_FILE_ENTRIES; i++ )
  {
    osapiFree(L7_SIM_COMPONENT_ID, tree[i].filename);
  }
}

/*********************************************************************
 *
 * @purpose Retrieve the directory listing and store in usable form
 *
 * @param tree Pointer to the directory listing structure
 * @param pFileNum Pointer to the number of files in the directory
 *
 * @returns L7_SUCCESS
 * @returns L7_ERROR
 *
 * @end
 *
 *********************************************************************/
L7_RC_t sysapiFileListGet(SYSAPI_DIRECTORY_t *tree, L7_uint32 *pFileNum)
{
  L7_char8 *pbuf, *ptok;
  *pFileNum = 0;

  if ((pbuf = osapiMalloc(L7_SIM_COMPONENT_ID, SYSAPI_CFGFILE_LIST_BUF_SIZE)) == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "osapiMalloc() failed");
    return L7_ERROR;
  }
  /* Get the directory listing */
  if ( osapiFsDir(pbuf, SYSAPI_CFGFILE_LIST_BUF_SIZE) != L7_SUCCESS )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "osapiFsDir() failed");
    osapiFree(L7_SIM_COMPONENT_ID, (void *)pbuf);
    return L7_ERROR;
  }

  ptok = strtok(pbuf, "\n");     /* Toss the '.' and '..' entries */
  ptok = strtok((L7_char8 *)'\0', "\n");

  do
  {
    ptok = strtok((L7_char8 *)'\0', " ");    /* Pull the filenames, one at a time */
    if ( ptok != NULL )
    {
      memset(tree[*pFileNum].filename, 0x00, L7_MAX_FILENAME);
      strncpy(tree[*pFileNum].filename,  ptok, L7_MAX_FILENAME-1);

      /* Assign the file type */
      if ( strstr(tree[*pFileNum].filename, SYSAPI_BIG_CONFIG_FILENAME) != L7_NULL )
      {
        tree[*pFileNum].filetype = SYSAPI_FILETYPE_BIGC;
      }
      else if ( strstr(tree[*pFileNum].filename, ".cfg") != L7_NULL )
      {
        tree[*pFileNum].filetype = SYSAPI_FILETYPE_CFG;
      }
      else
      {
        tree[*pFileNum].filetype = SYSAPI_FILETYPE_OTHER;
      }

      /* Get the file size */
      ptok = strtok((L7_char8 *)'\0', "\n");
      if ( ptok != L7_NULL)
      {
        while ( strncmp(ptok, " ", 1) == L7_NULL )  /* Strip off leading blanks */
        {
          ptok++;
        }
        tree[*pFileNum].filesize = (L7_uint32)atoi(ptok);
        (*pFileNum)++;
      }
    }

  } while ( ptok != L7_NULL );

  osapiFree(L7_SIM_COMPONENT_ID, (void *)pbuf);

  return L7_SUCCESS;
}


/*********************************************************************
 *
 * @purpose Combine all the '.cfg' files into one Big Config file
 *          and save it as SYSAPI_BIG_CONFIG_FILENAME on the ramdisk
 *
 * @returns L7_SUCCESS
 * @returns L7_ERROR  there were problems
 *
 * @end
 *
 *********************************************************************/
L7_RC_t sysapiCfgFileCombine(void)
{
  return L7_SUCCESS;

}

/*********************************************************************
 *
 * @purpose Re-read configuration from NVRAM into run-time storage.
 *
 * @returns none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t sysapiCfgFileReload(L7_uchar8* filename)
{
  L7_RC_t rc;

  rc = sysapiCfgFileSeparate(filename);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to separate component config from %s\n", filename);
  }
  return rc;
}

/*********************************************************************
 *
 * @purpose Initiate system unconfig
 *
 * @returns L7_SUCCESS
 * @returns L7_ERROR  there were problems
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t sysapiUnconfig(void)
{
  L7_RC_t rc;
  L7_CNFGR_CMD_DATA_t cmdData, *pCmdData = &cmdData;

  /* Initiate the unconfiguration cycle.
   */
  pCmdData->cbHandle         = L7_CNFGR_NO_HANDLE;
  pCmdData->command          = L7_CNFGR_CMD_UNCONFIGURE;
  pCmdData->correlator       = L7_NULL;
  pCmdData->type             = L7_CNFGR_EVNT;
  pCmdData->u.evntData.event = L7_CNFGR_EVNT_U_START;
  pCmdData->u.evntData.data  = L7_LAST_COMPONENT_ID;
  rc = cnfgrApiScheduleCommand(pCmdData, (L7_uint32)5000);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }
  clearConfig = L7_TRUE;
  return(rc);
}

/*********************************************************************
* @purpose  Sets the Clear Config Flag.
*
* @param    none
*
* @returns
*
* @notes The clearConfig flag will be set to TRUE when user clears the configuration
         and will be set to FALSE when user saves the configuration to startup-config.
*
* @end
*
*********************************************************************/
void sysapiClearConfigFlagSet(L7_BOOL val)
{
  clearConfig = val;
}

/*********************************************************************
* @purpose  Gets the Clear Config Flag.
*
* @param    none
*
* @returns
*
* @notes The clearConfig flag will be having TRUE when user clears the configuration
         and will be having FALSE when user saves the configuration to startup-config.
*
* @end
*
*********************************************************************/
void sysapiClearConfigFlagGet(L7_BOOL *val)
{
  *val = clearConfig;
}

/*********************************************************************
 *
 * @purpose Read the Big Config file from the ramdisk and parse it out
 *          into all the component config files on the ramdisk
 *
 * @param   bigCfn @b{(input)} Big Config file
 *
 * @returns L7_SUCCESS
 * @returns L7_ERROR  there were problems
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t sysapiCfgFileDeploy(L7_char8 *bigCfn)
{
  L7_RC_t rc;

  bigCfn = bigCfn ? bigCfn : SYSAPI_CONFIG_FILENAME;
  rc = sysapiCfgFileSeparate(bigCfn);

  if (rc != L7_SUCCESS)
  {
    rc = sysapiCfgFileRel4_0Separate (bigCfn);
  }

  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  return sysapiUnconfig();
}
/*********************************************************************
 * @purpose  Set the MAC address for the specified interface
 *
 * @param    portMac     port mac address to be set
 * @param    sysMac      system base mac address
 * @param    portNumber  port number
 *
 *
 * @returns  None
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void sysapiSetMacAddress(L7_uchar8  *portMac,L7_uchar8 *sysMac, L7_uint32 portNumber)
{
  L7_uint32     portMacLs4b, sysMacLs4b;
  L7_ushort16   portMacMs2b, sysMacMs2b;

  portMacMs2b = (portMac[0] << 8) | portMac[1];
  portMacLs4b = (portMac[2] << 24) | (portMac[3] << 16) | (portMac[4] << 8) | (portMac[5]);

  sysMacMs2b = (sysMac[0] << 8) | sysMac[1];
  sysMacLs4b = (sysMac[2] << 24) | (sysMac[3] << 16) | (sysMac[4] << 8) | (sysMac[5]);

  portMacLs4b = sysMacLs4b + portNumber;

  /* if there was a carry */
  if (portMacLs4b < sysMacLs4b)
  {
    portMacMs2b = sysMacMs2b + 1;
  }
  else
  {
    portMacMs2b = sysMacMs2b;
  }

  portMac[0] = portMacMs2b >> 8;
  portMac[1] = portMacMs2b & 0x00ff;

  portMac[2] = (portMacLs4b >> 24) & 0x000000ff;
  portMac[3] = (portMacLs4b >> 16) & 0x000000ff;
  portMac[4] = (portMacLs4b >> 8)  & 0x000000ff;
  portMac[5] = (portMacLs4b )      & 0x000000ff;

}

/*********************************************************************
 *
 * @purpose Allocate memory to store individual component config files
 *
 * param    Void
 *
 * @returns L7_SUCCESS
 * @returns L7_ERROR
 *
 * @end
 *
 *********************************************************************/
L7_RC_t sysapiCfgFileSysInit(void)
{
  L7_uint32 i;
  L7_uint32 rc;
  L7_uint32       unit;

  unit = 1; /* This unit number just for the sake of place holder */

  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) == L7_TRUE)
  {
    sysapiTxtCfgInit();
  }

  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    pSysapiCfgTree[i] = (SYSAPI_DIRECTORY_CFG_t *)osapiMalloc(L7_SIM_COMPONENT_ID, (L7_uint32)sizeof(SYSAPI_DIRECTORY_CFG_t));
    if (pSysapiCfgTree[i] == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Failed to malloc pSysapiCfgTree[%u]\n", i);
      LOG_ERROR(1);
      return L7_ERROR;
    }
    else
    {
      memset(pSysapiCfgTree[i], 0x00, sizeof(SYSAPI_DIRECTORY_CFG_t));

      pSysapiCfgTree[i]->fileInfo.filename = osapiMalloc(L7_SIM_COMPONENT_ID, L7_MAX_FILENAME);
      if ( pSysapiCfgTree[i]->fileInfo.filename == L7_NULLPTR)
      {
        LOG_ERROR (0);
      }
      pSysapiCfgTree[i]->fileInfo.filename[0] = 0;
    }
  }

  maskUtilInit();

  sysapiCfgSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  rc = sysapiCfgFileSeparate(SYSAPI_CONFIG_FILENAME);
  if (L7_SUCCESS == rc)
  {
    return rc;
  }

  if (L7_TRUE != usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "could not separate %s. The configuration file "
            "could not be read.  This message may occur on a system for which no configuration has ever "
            "been saved or for which configuration has been erased.", SYSAPI_CONFIG_FILENAME);
  }

  /* check if we need to convert uncombined individual files to combined file */
  if (L7_ERROR == rc)
  {
    return rc;
  }

  return sysapiCfgFileUncombinedToCombinedCfgFileConvert ();
}

/*********************************************************************
 *
 * @purpose Read the Big Config file from the ramdisk and distribute
 *          the `*.cfg` files into a set of memory areas on the ramdisk
 *
 * param    cfgFile  the big config file
 *
 * @returns L7_SUCCESS
 * @returns L7_ERROR
 * @returns L7_FAILURE
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t sysapiCfgFileSeparate(L7_char8 *cfgFile)
{
  L7_uint32 total_length = 0;
  L7_uint32 checksum;
  L7_uint32 read_checksum;
  L7_fileHdr_t cfgFileHdr;
  L7_RC_t   rc;
  L7_int32  file_desc;
  L7_uint32 file_header_size = sizeof (L7_fileHdr_t);
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *hpc_unit;

  hpc_unit = sysapiHpcLocalUnitDbEntryGet();

  /*-----------------------------------------------------------------------------*/
  /* take semaphore to ensure that this operation completes without interruption */
  /*-----------------------------------------------------------------------------*/

  (void)osapiSemaTake(sysapiCfgSema, L7_WAIT_FOREVER);


  /*-----------------------------------------------*/
  /* get the size of the data area, if file exists */
  /*-----------------------------------------------*/

  if (osapiFsFileSizeGet(cfgFile, &total_length) != L7_SUCCESS)
  {
    if (L7_TRUE != usmDbFeaturePresentCheck(1, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "Configuration file %s size is 0 (zero) bytes. "
              "The configuration file could not be read.  This message may occur on a system for which no"
              " configuration has ever been saved or for which configuration has been erased.", cfgFile);
    }

    (void)osapiSemaGive(sysapiCfgSema);
    return L7_FAILURE;
  }

  if (total_length < file_header_size)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Configuration file size %u is toos small.\n",
             total_length);
    (void)osapiSemaGive(sysapiCfgSema);
    return L7_ERROR;
  }

  memset(&cfgFileHdr, 0, sizeof(cfgFileHdr));
  rc = osapiFsRead (cfgFile, (L7_char8 *) &cfgFileHdr, file_header_size);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to read config file %s header\n", cfgFile);
    (void)osapiSemaGive(sysapiCfgSema);
    return L7_ERROR;
  }

  if ((0 != cfgFileHdr.fileHdrVersion) &&
      (L7_FILE_HDR_VER_CURRENT != cfgFileHdr.fileHdrVersion))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Version Check Failed\n");
    (void)osapiSemaGive (sysapiCfgSema);
    return L7_ERROR;
  }

  /* It is possible that file size on disk is larger than expected file
  ** size. For CRC computation and file parsing we must use the expected
  ** file size.
  */
  if (total_length < cfgFileHdr.length || cfgFileHdr.length < file_header_size)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Configuration file on disk size %u is less than expected size %u.\n",
             total_length, cfgFileHdr.length);
    (void)osapiSemaGive(sysapiCfgSema);
    return L7_ERROR;
  }

  total_length = cfgFileHdr.length;

  /*------------------------------*/
  /* calculate and verify the crc */
  /*------------------------------*/
  {
    L7_uint32 i, j;
    L7_uchar8 file_buf [SYSAPI_FILE_BUF_SIZE];
    L7_uint32 buf_size;

    rc = osapiFsOpen (cfgFile, &file_desc);
    if (rc != L7_SUCCESS)
    {
      (void)osapiSemaGive (sysapiCfgSema);
      return L7_ERROR;
    }

    checksum = nvStoreCrcInit ();

    memset(file_buf, 0, sizeof(file_buf));
    i = 0;
    while (i < (total_length - 4))
    {
      buf_size = (total_length - 4) - i;
      if (buf_size > SYSAPI_FILE_BUF_SIZE)
      {
        buf_size = SYSAPI_FILE_BUF_SIZE;
      }

      rc = osapiFileRead (file_desc, file_buf, buf_size);
      if (rc != L7_SUCCESS)
      {
        (void)osapiSemaGive (sysapiCfgSema);
        return L7_ERROR;
      }

      for (j = 0; j < buf_size; j++)
      {
        checksum = nvStoreCrcUpdate (checksum, file_buf[j]);
      }

      i += buf_size;
    }


    checksum = nvStoreCrcFinish (checksum);

    read_checksum = 0;
    rc = osapiFileRead (file_desc, (L7_char8 *) &read_checksum, 4);
    if (rc != L7_SUCCESS)
    {
      (void)osapiSemaGive (sysapiCfgSema);
      return L7_ERROR;
    }

    (void)osapiFsClose (file_desc);
  }

  if (checksum != read_checksum)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "sysapiCfgFilesSeparate: CRC check failed. 0x%x read and 0x%x calculated",
            read_checksum, checksum);
    (void)osapiSemaGive(sysapiCfgSema);
    return L7_ERROR;
  }

  /* Read the file header from the big config file.
   */
  rc = osapiFsOpen (cfgFile, &file_desc);
  if (rc != L7_SUCCESS)
  {
    (void)osapiSemaGive (sysapiCfgSema);
    return L7_ERROR;
  }

  rc = osapiFileRead (file_desc, (L7_char8 *) &cfgFileHdr, file_header_size);
  if (rc != L7_SUCCESS)
  {
    (void)osapiSemaGive (sysapiCfgSema);
    return L7_ERROR;
  }

  /* Make sure that file name is what we expect.
   */
  if (strcmp(cfgFileHdr.filename, SYSAPI_CONFIG_FILENAME) != 0 &&
      strcmp(cfgFileHdr.filename, SYSAPI_CONFIG_RUNING_FILENAME) != 0)
  {
    (void)osapiFsClose (file_desc);
    (void)osapiSemaGive(sysapiCfgSema);
    return L7_ERROR;
  }

  /* Make sure that the configuration file is correct for the target device.
   */
  if (cfgFileHdr.targetDevice != hpc_unit->configLoadTargetId)
  {
    (void)osapiFsClose (file_desc);
    (void)osapiSemaGive(sysapiCfgSema);
    return L7_ERROR;
  }


  /*--------------------------*/
  /* read the big config file */
  /*--------------------------*/

  if (cfgFileHdr.version == SYSAPI_CFG_VER_1)
  {
    sysapiCfgFileSeparateFilesV1(file_desc, total_length);
  }
  else if (cfgFileHdr.version == SYSAPI_CFG_VER_2)
  {
    sysapiCfgFileSeparateFilesV2(file_desc, total_length);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Unrecognized big config file version %d\n", cfgFileHdr.fileHdrVersion);
    (void)osapiSemaGive(sysapiCfgSema);
    return L7_ERROR;
  }

  osapiFsClose(file_desc);

  /*---------------------*/
  /* give semaphore back */
  /*---------------------*/

  (void)osapiSemaGive(sysapiCfgSema);

  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Invalidate run-time configuration.
 *
 * @param    none
 *
 * @returns  none
 *
 * @notes
 *
 * @end
 *********************************************************************/
void sysapiCfgFileInvalidate (void)
{
  L7_uint32 i;

  /* Set file type to something other than L7_CFG_DATA.
   ** This forces the osapiCfgFileGet() function to
   ** create default configuration.
   */
  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    pSysapiCfgTree[i]->fileInfo.filetype = L7_INIT_PARMS;
  }
}

/*********************************************************************
 * @purpose  Write data to the file. Create/Open file if does not exist.
 *
 * @param    component_id  Component ID of the file owner.
 * @param    filename      file to write data to
 * @param    buffer        actual data to write to file
 * @param    nbytes        number of bytes to write to file
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t sysapiCfgFileWrite( L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
    L7_char8 *buffer, L7_uint32 nbytes)
{
#if L7_FEAT_TEXT_BASED_CONFIG
  return L7_SUCCESS;
#else
  return sysapiBinaryCfgFileWrite(component_id, filename, buffer, nbytes);
#endif
}

/*********************************************************************
 * @purpose  Write binary data to the file. Create/Open file if does not exist.
 *
 * @param    component_id  Component ID of the file owner.
 * @param    filename      file to write data to
 * @param    buffer        actual data to write to file
 * @param    nbytes        number of bytes to write to file
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t sysapiBinaryCfgFileWrite( L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
    L7_char8 *buffer, L7_uint32 nbytes)
{
  L7_RC_t       rc;
  L7_uint32     fileindex;
  L7_fileHdr_t  *cfgFileHdr;
  L7_RC_t rc2;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc2 = cnfgrApiComponentNameGet(component_id, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  /*-----------------------------------------------------------------------------*/
  /* take semaphore to ensure that this operation completes without interruption */
  /*-----------------------------------------------------------------------------*/

  (void)osapiSemaTake(sysapiCfgSema, L7_WAIT_FOREVER);

  if(sysapiValidateCfgFileName(filename) != L7_TRUE)
  {
    (void)osapiSemaGive(sysapiCfgSema);
    return L7_SUCCESS;
  }

  /* Initialize parms */
  fileindex = 0;

  /* Verify that file size in the file header is the same as
   ** file size passed as parameter to this function.
   */
  cfgFileHdr = (L7_fileHdr_t *) buffer;
  if (cfgFileHdr->length != nbytes)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "File %s hdr length = %u is invalid expect %u\n", filename,
        cfgFileHdr->length, nbytes);
    (void)osapiSemaGive(sysapiCfgSema);
    return L7_ERROR;
  }

  /* Make sure that component file name is not NULL.
   */
  if (cfgFileHdr->filename[0] == 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "File %s hdr name = %s is invalid\n", filename, (L7_char8 *)cfgFileHdr->filename);
    (void)osapiSemaGive(sysapiCfgSema);
    return L7_ERROR;
  }

  rc = sysapiCfgFileIndexGet(filename, &fileindex);
  if (rc == L7_ERROR)
  {
    (void)osapiSemaGive(sysapiCfgSema);
    return rc;
  }


  if (sysapiCfgTreeUpdate(component_id, filename, buffer, nbytes,
        pSysapiCfgTree[fileindex])  != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to update cfg tree for component %d, %s and file %s\n",
        component_id, name, cfgFileHdr->filename);

  }

  (void)osapiSemaGive(sysapiCfgSema);
  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Write raw binary data to the file. Create/Open file if does not exist.
 *
 * @param    component_id  Component ID of the file owner.
 * @param    filename      file to write data to
 * @param    buffer        actual data to write to file
 * @param    nbytes        number of bytes to write to file
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @notes  Creates a temp fileHdr_t structure and passes it to
 *
 * @end
 *********************************************************************/
L7_RC_t sysapiBinaryCfgFileRawWrite(L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
    L7_char8 *buffer, L7_uint32 buffer_length)
{
  L7_RC_t rc;
  L7_uint32 nbytes, chkSum;
  L7_char8 *dataArea = L7_NULL;
  L7_fileHdr_t *cfgFileHdr = L7_NULLPTR;

  nbytes = buffer_length + sizeof(L7_fileHdr_t) + sizeof(L7_uint32);

  /* Make sure that file name is not NULL.
   */
  if (filename == L7_NULLPTR ||
      filename[0] == 0 ||
      buffer == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  dataArea = osapiMalloc (component_id, nbytes);

  if (dataArea == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to allocate %u for config file %s\n", nbytes, filename);
    return L7_ERROR;
  }

  /* Create a dummy configuration file header for this raw file */
  cfgFileHdr = (L7_fileHdr_t *)(dataArea);

  memset((L7_char8 *)cfgFileHdr, 0, sizeof(L7_fileHdr_t));

  cfgFileHdr->componentID = component_id;
  cfgFileHdr->length = nbytes;
  osapiStrncpySafe(cfgFileHdr->filename, filename,sizeof(cfgFileHdr->filename));

  /* copy cfg data into this new data area */
  memcpy(dataArea + sizeof(L7_fileHdr_t), (void *)buffer, buffer_length);

  /* calculate checksum for file */
  chkSum = nvStoreCrc32(dataArea, nbytes - sizeof(L7_uint32));
  memcpy(dataArea + sizeof(L7_fileHdr_t) + buffer_length, &chkSum, sizeof(chkSum));

  rc = sysapiBinaryCfgFileWrite(component_id, filename, dataArea, nbytes);

  osapiFree(component_id, dataArea);

  return rc;
}

/*********************************************************************
 *
 * @purpose Combine all `*.cfg` files into one Big Config file and
 *          save it as SYSAPI_CONFIG_FILENAME on the ramdisk
 *
 * @param   Void
 *
 * @returns L7_SUCCESS
 * @returns L7_ERROR
 *
 * @end
 *
 *********************************************************************/
static L7_BOOL sysapiCfgTreeEntrySaveCheck(SYSAPI_DIRECTORY_CFG_t *pCfgTreeEntry)
{
  if (pCfgTreeEntry->fileInfo.file_location == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  if (pCfgTreeEntry->fileInfo.filetype != L7_CFG_DATA)
  {
    return L7_FALSE;
  }

  if(sysapiValidateCfgFileName(pCfgTreeEntry->fileInfo.filename) != L7_FALSE)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

L7_RC_t sysapiCfgFileSave(L7_uchar8* filename)
{
  L7_uint32 i, j;
  L7_uint32 total_length;
  L7_uint32 size_to_copy;
  L7_uint32 checksum;
  L7_fileHdr_t cfgFileHdr;
  L7_int32      file_desc=0;
  L7_uchar8     *octet;
  L7_uchar8     *source_location;
  L7_RC_t       rc;
  SYSAPI_HPC_UNIT_DESCRIPTOR_t *hpc_unit;

  hpc_unit = sysapiHpcLocalUnitDbEntryGet();

  /*-----------------------------------------------------------------------------*/
  /* take semaphore to ensure that this operation completes without interruption */
  /*-----------------------------------------------------------------------------*/

  (void)osapiSemaTake(sysapiCfgSema, L7_WAIT_FOREVER);

  /* Compute total configuration file size.
   */
  total_length = sizeof(L7_fileHdr_t);

  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    if(sysapiCfgTreeEntrySaveCheck(pSysapiCfgTree[i]) == L7_FALSE)
    {
      continue;
    }


    /* Each file location points to a configuration file overlay which contains
     ** an L7_fileHdr_t structure, the configuration data, and a crc
     */

    total_length += sizeof(sysapiCfgFileDescr_t);
    total_length += pSysapiCfgTree[i]->cfgFileDescr.compressedFileSize;

  }


  /* Add in the size crc for the combined file
   */
  total_length = total_length + sizeof(L7_uint32);

  /*-----------------------------------------*/
  /* create the header for the combined file */
  /*-----------------------------------------*/

  memset((L7_char8 *)&cfgFileHdr, 0, sizeof(L7_fileHdr_t));

  cfgFileHdr.length = total_length;
  strncpy((char*)cfgFileHdr.filename, filename, L7_MAX_FILENAME);
  cfgFileHdr.version = SYSAPI_CFG_VER_CURRENT;
  cfgFileHdr.type = L7_CFG_DATA;
  cfgFileHdr.targetDevice = hpc_unit->configLoadTargetId;

  /* Set the Header Version */
  cfgFileHdr.fileHdrVersion = L7_FILE_HDR_VER_CURRENT;

  /* Save the file header
   */
  rc = osapiFsFileCreate (filename, &file_desc);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  checksum = nvStoreCrcInit ();

  octet = (L7_uchar8 *) &cfgFileHdr;
  for (i = 0; i < sizeof (cfgFileHdr); i++)
  {
    checksum = nvStoreCrcUpdate (checksum, *octet);
    octet++;
  }

  rc = osapiFsWriteNoClose (file_desc, (L7_char8 *) &cfgFileHdr, sizeof (cfgFileHdr));
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  /*--------------------------------------------*/
  /* Iterate through the tree to copy all files */
  /*--------------------------------------------*/

  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    if(sysapiCfgTreeEntrySaveCheck(pSysapiCfgTree[i]) == L7_FALSE)
    {
      continue;
    }

    /* Always place the sysapiCfgFileDescr_t structure immediately before
       the cfg file in big combined cfg file */

    size_to_copy    =  sizeof(sysapiCfgFileDescr_t);
    source_location =  (L7_uchar8 *)&pSysapiCfgTree[i]->cfgFileDescr;
    rc = osapiFsWriteNoClose (file_desc, source_location, size_to_copy);
    if (rc != L7_SUCCESS)
    {
      LOG_ERROR (rc);
    }

    octet = source_location;
    for (j = 0; j < size_to_copy; j++)
    {
      checksum = nvStoreCrcUpdate (checksum, *octet);
      octet++;
    }

    /* Each file location points to a configuration file overlay which contains
       an L7_fileHdr_t structure, the configuration data, and a crc. */

    size_to_copy    =  pSysapiCfgTree[i]->cfgFileDescr.compressedFileSize;
    source_location =  pSysapiCfgTree[i]->fileInfo.file_location;

    rc = osapiFsWriteNoClose (file_desc, source_location, size_to_copy);
    if (rc != L7_SUCCESS)
    {
      LOG_ERROR (rc);
    }

    octet = source_location;
    for (j = 0; j < size_to_copy; j++)
    {
      checksum = nvStoreCrcUpdate (checksum, *octet);
      octet++;
    }
  }

  /*-----------------------------------------*/
  /*  Finish up and close the file           */
  /*-----------------------------------------*/
  checksum = nvStoreCrcFinish (checksum);

  /* Store CRC in the last four bytes of the file.
   */
  rc = osapiFsWriteNoClose (file_desc, (L7_uchar8 *) &checksum, 4);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  (void)osapiFsClose (file_desc);

  /*---------------------*/
  /* give semaphore back */
  /*---------------------*/
  (void)osapiSemaGive(sysapiCfgSema);

  return L7_SUCCESS;
}

/*
 * Helper function to build default configuration
 */
static L7_RC_t sysapiCfgFileBuildDefault(L7_COMPONENT_IDS_t component_id, L7_char8 *fileName,
    L7_char8 *buffer, L7_uint32 bufferSize,
    L7_uint32 *checkSum, L7_uint32 version,
    void (*defaultBuild)(L7_uint32))
{
  L7_fileHdr_t *pFileHdr;

  if ((L7_uint32)defaultBuild == L7_NULL)
  {
    return L7_FAILURE;
  }

  if(sysapiTxtCfgEnable == L7_FALSE)
  {
	L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "Building defaults for file %s version %u. "
			"Configuration did not exist or could not be read for the specified feature or file.  Default "
			"configuration values will be used.   The file name and version are indicated..\n",fileName, version);
  }

  (*defaultBuild)(version);
  pFileHdr = (L7_fileHdr_t *)buffer;
  pFileHdr->fileHdrVersion = L7_FILE_HDR_VER_CURRENT;
  pFileHdr->componentID = component_id;
  *checkSum = nvStoreCrc32((L7_uchar8 *)buffer, bufferSize-sizeof(L7_uint32));

  if (sysapiCfgFileWrite(component_id, fileName, buffer, bufferSize) == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Error on call to sysapiCfgFileWrite routine on file %s\n", fileName);
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Read configuration data from the data location pointer to
 *           by the config file management system
 *
 * @param    component_id   component ID of the file owner.
 * @param    fileName       name of file to load
 * @param    buffer         pointer to location file will be placed
 * @param    bufferSize     size of buffer
 * @param    checkSum       pointer to location of checksum in buffer
 * @param    version        expected software version of file (current version)
 * @param    defaultBuild   function ptr to factory default build routine
 *                          if L7_NULL do not build default file
 * @param    migrateBuild   function ptr to config migration routine
 *                          if config is outdated
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    Routine assumes checkSum is last 4 bytes of buffer.
 *           Routine assumes first structure in the buffer is L7_fileHdr_t
 *
 * @end
 *********************************************************************/
L7_RC_t sysapiCfgFileGetImpl(L7_COMPONENT_IDS_t component_id, L7_char8 *fileName,
    L7_char8 *buffer, L7_uint32 bufferSize,
    L7_uint32 *checkSum, L7_uint32 version,
    void (*defaultBuild)(L7_uint32),
    void (*migrateBuild)(L7_uint32, L7_uint32, L7_char8 *))
{
  L7_uint32 chkSum;
  L7_uint32 fileSize;
  L7_uint32 savedVersion;
  L7_BOOL   buildDefaultFile;
  void      *pCfg;
  L7_fileHdr_t *pFileHdr;
  L7_uint32 fileIndex;
  L7_RC_t rc;
  L7_char8 *pCompressedData;
  L7_uint32 compressedDataSize;
  L7_char8 *pUncompressedData;
  L7_uint32 uncompressedBufLen;
  L7_RC_t rc2;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc2 = cnfgrApiComponentNameGet(component_id, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }


  /* Initialize parms */
  buildDefaultFile  = L7_FALSE;
  pCfg              = L7_NULL;
  fileIndex         =   0;
  pUncompressedData = L7_NULL;
  pFileHdr          = L7_NULL;

  if(sysapiTxtCfgExists == L7_TRUE && sysapiValidateCfgFileName(fileName) != L7_TRUE)
  {
    return sysapiCfgFileBuildDefault(component_id, fileName, buffer, bufferSize,
        checkSum, version, defaultBuild);
  }

  /* Obtain the index */
  rc = sysapiCfgFileIndexGet(fileName, &fileIndex);
  if (rc != L7_SUCCESS)
  {
    fileSize = 0;
  }
  else
  {
    buildDefaultFile = L7_FALSE;
    pCfg = pSysapiCfgTree[fileIndex]->fileInfo.file_location;

    /* Get the fileSize */
    fileSize = 0;
    if ((pCfg != L7_NULLPTR) &&
        (pSysapiCfgTree[fileIndex]->cfgFileDescr.marker == SYSAPI_CFG_DESCR_MARKER))
    {
      pFileHdr = (L7_fileHdr_t *)pCfg;

      fileSize = pFileHdr->length;
    }
  }

  /* If this is not a configuration file then it is not valid.
   */
  if ((fileSize != 0) &&
      (pSysapiCfgTree[fileIndex]->fileInfo.filetype != L7_CFG_DATA))
  {
    fileSize = 0;
  }

  /* If file does not exist or file size does not equal to the buffer size
     available to store the file, migrate the file */
  if (fileSize < sizeof(L7_fileHdr_t) || (pCfg == L7_NULLPTR))
  {
    buildDefaultFile = L7_TRUE;
  }
  else
  {
    /* Read the file header to validate the version info */
    memcpy(buffer, pFileHdr, sizeof(L7_fileHdr_t));

    savedVersion = ((L7_fileHdr_t *)buffer)->version;
    /*check for version mismatch*/
    if ((savedVersion != version) || (fileSize != bufferSize))
    {
      /*call migrate function*/
      if ((L7_uint32)migrateBuild == L7_NULL)
      {
        buildDefaultFile = L7_TRUE;
      }
      else
      {
        if(savedVersion == version)
        {
		  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "File %s: same version (%u) but the sizes "
					"(%u->%u) differ. The configuration file which was loaded was of a different size than "
					"expected for the version number.  This message indicates the configuration file needed"
					" to be migrated to the version number appropriate for the code image. This message may"
					" appear after upgrading the code image to a more current release.\n", fileName, version,
				    fileSize, bufferSize);
        }
		L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "Migrating config file %s from version %u "
					"to %u. A configuration file version mismatch was detected so a configuration file "
					"migration has started.\n", fileName, savedVersion, version);

        /* Read old configuration into the buffer.
         */
        if (bufferSize < fileSize)
        {
          memcpy(buffer, pCfg, bufferSize);
        }
        else
        {
          memcpy(buffer, pCfg, fileSize);
        }

        (*migrateBuild)(savedVersion, version, buffer);

        return (L7_SUCCESS);
      }
    }
    else
    {
      /*----------------------------------------------*/
      /* Uncompress the config data for the component */
      /*----------------------------------------------*/

      pCompressedData    = (L7_char8 *)pSysapiCfgTree[fileIndex]->fileInfo.file_location + sizeof(L7_fileHdr_t);
      compressedDataSize = pSysapiCfgTree[fileIndex]->cfgFileDescr.compressedFileSize - sizeof(L7_fileHdr_t);

      /* Need to pass in the data size that is needed for full uncompression */
      uncompressedBufLen = fileSize;

      if (sysapiUncompress(pCompressedData, compressedDataSize,
            (L7_char8 **)&pUncompressedData, &uncompressedBufLen) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "Failed to uncompress config data for component %d, %s\n", component_id, name);
        return L7_ERROR;
      }

      /*-----------------------------*/
      /* Read the configuration file */
      /*-----------------------------*/

      /* Copy the file header */
      memcpy(buffer, pSysapiCfgTree[fileIndex]->fileInfo.file_location, sizeof(L7_fileHdr_t));

      /* Copy the uncompressed  data */
      memcpy(buffer + sizeof(L7_fileHdr_t), pUncompressedData, uncompressedBufLen);

      /* Must free the uncompression buffer */
      osapiFree(L7_SIM_COMPONENT_ID, pUncompressedData);

      /* Validate the file */
      chkSum = nvStoreCrc32((L7_uchar8 *)buffer, bufferSize-sizeof(L7_uint32));
      *checkSum = *(L7_uint32 *) (buffer + (bufferSize - sizeof (L7_uint32)));
      if (*checkSum != chkSum)
      {
		L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "File %s corrupted from file system.  "
					"Checksum mismatch.The calculated checksum of a component’s configuration file "
					"in the file system did not match the checksum of the file in memory.\n", fileName);
        buildDefaultFile = L7_TRUE;
      }
      else
      {
        pFileHdr = (L7_fileHdr_t *)buffer;
        pFileHdr->componentID = component_id;
        pFileHdr->fileHdrVersion = L7_FILE_HDR_VER_CURRENT;
        *checkSum = nvStoreCrc32((L7_uchar8 *)buffer, bufferSize-sizeof(L7_uint32));
      }
    }
  }

  if (buildDefaultFile == L7_TRUE)
  {
    return sysapiCfgFileBuildDefault(component_id, fileName, buffer, bufferSize,
        checkSum, version, defaultBuild);
  }
  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Read configuration data from the data location pointer to
 *           by the config file management system and return the filesize
 *
 * @param    component_id  Component ID of the file owner.
 * @param    fileName       name of file to return buffer size of
 * @param    bufferSize     pointer to location of buffer size
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t sysapiCfgFileSizeGet( L7_COMPONENT_IDS_t component_id,
    L7_char8 *fileName, L7_uint32 *bufferSize)
{
  L7_uint32 fileIndex=0;
  L7_RC_t rc;

  rc = sysapiCfgFileIndexGet(fileName, &fileIndex);
  if (rc != L7_SUCCESS)
  {
    *bufferSize = 0;
    return L7_FAILURE;
  } else
  {
    /* Get the fileSize */
    *bufferSize = pSysapiCfgTree[fileIndex]->cfgFileDescr.uncompressedLength - sizeof(L7_fileHdr_t);
  }
  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Read configuration data from the data location pointer to
 *           by the config file management system
 *
 * @param    component_id   Component ID of the file owner.
 * @param    fileName       name of file to load
 * @param    buffer         pointer to location file will be placed
 * @param    bufferSize     size of buffer read from file
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t sysapiCfgFileRawGet(L7_COMPONENT_IDS_t component_id, L7_char8 *fileName,
    L7_char8 *buffer, L7_uint32 *bufferSize)
{
  L7_RC_t rc;
  L7_uint32 nbytes;
  L7_char8 *dataArea = L7_NULL;
  L7_uint32 fileIndex=0;
  L7_uint32 checkSum;

  nbytes = *bufferSize + sizeof(L7_fileHdr_t);

  /* Make sure that file name is not NULL.
   */
  if (fileName == L7_NULLPTR ||
      fileName[0] == 0 ||
      buffer == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  dataArea = osapiMalloc (component_id, nbytes);

  if (dataArea == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to allocate %u for config file %s\n", nbytes, fileName);
    return L7_ERROR;
  }

  rc = sysapiCfgFileIndexGet(fileName, &fileIndex);

  if (rc != L7_SUCCESS)
    return rc;

  rc = sysapiCfgFileGet(fileIndex, fileName, dataArea, nbytes, &checkSum, 0, L7_NULLPTR, L7_NULLPTR);

  if (rc == L7_SUCCESS)
  {
    /* Copy just the data from the the config file to the buffer */
    memcpy(buffer, dataArea + sizeof(L7_fileHdr_t), *bufferSize);
    *bufferSize = nbytes - sizeof(L7_fileHdr_t);
  }

  osapiFree(component_id, dataArea);

  return rc;
}

/*********************************************************************
 *
 * @purpose Find the index of a filename in pSysapiCfgTree
 *          If file index is not found then the first empty file
 *          index is returned.
 *
 * @param    fileName       name of file to load
 * @param    pFileIndex     address of file's index if function returns
 *                          L7_SUCCESS
 *
 * @returns L7_SUCCESS  - Match is found.
 * @returns L7_FAILURE  - Did not find match, but return pointer to empty index.
 * @returns L7_ERROR - Match is not found, error.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t sysapiCfgFileIndexGet(const L7_char8 *fileName, L7_uint32 *pFileIndex)
{
  L7_uint32 i;
  L7_uint32 first_empty_index = 0;
  L7_uint32 first_empty_index_is_set = 0;

  for(i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    if (pSysapiCfgTree[i] == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "pSysapiCfgTree[%u] is NULL", i);
      return L7_ERROR;
    }
    else
    {
      if (strcmp(fileName, pSysapiCfgTree[i]->fileInfo.filename) == 0)
      {
        *pFileIndex = i;
        return L7_SUCCESS;
      } else
      {
        if ((first_empty_index_is_set == 0) &&
            (pSysapiCfgTree[i]->fileInfo.filename[0] == 0))
        {
          first_empty_index_is_set = 1;
          first_empty_index = i;
        }
      }
    }
  }

  if (first_empty_index_is_set != 0)
  {
    *pFileIndex = first_empty_index;
    return L7_FAILURE;
  }

  return L7_ERROR;
}



/*********************************************************************
 *
 * @purpose Uncompress the data
 *
 * @param    pCompressedData     (@input) pointer to compressed data
 * @param    compressedDataSize  (@input) length of compresed data
 * @param    pUncompressedData   (@output) pointer to location to store uncompressed data
 * @param    *uncompressedBufLen  (@input/@output) length of buffer storing uncompressed data
 *
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE  -  value passed by uncompressedBufLen indicates buffer is too small.
 *                         Returned value in *uncompressedBufLen will identify correct buffer size
 * @returns L7_ERROR
 *
 * @notes    pUncompressedData should not point to previously allocated memory
 *
 * @notes    On input, the value of *uncompressedBufLen must be non-null.
 *
 * @notes    The uncompression algorithm will fail if the value for *uncompressedBufLen
 *           is too small.
 *
 * @notes    On output, the value of *uncompressedBufLen is modified to be the actual length of
 *           the uncompression buffer.
 *
 * @notes    Caller is responsible for freeing memory pointed to by pCompressedData
 *
 * @end
 *
 *********************************************************************/
L7_RC_t sysapiUncompress(L7_char8 *pCompressedData, L7_uint32 compressedDataSize,
    L7_char8 **pUncompressedData, L7_uint32 *uncompressedBufLen)
{
  L7_ulong32  compressionBufferSize;
  L7_ulong32  uncompressionBufferSize;
  L7_uint32   retCode;
  L7_BOOL     bufferAllocated;
  L7_BOOL     bufferAllocationNeeded;
  L7_char8    logBuffer[128];
  L7_char8    *pBuffer;

  bufferAllocated         = L7_FALSE;
  bufferAllocationNeeded  = L7_FALSE;


  /* compression buffer is required to be 0.1% + 12 bytes larger than the source */
  uncompressionBufferSize =  *uncompressedBufLen;
  compressionBufferSize = uncompressionBufferSize + uncompressionBufferSize/1000 +12;

  pBuffer = *pUncompressedData;


  /*  Allocate buffer if needed */
  if (L7_NULL == pBuffer)
    bufferAllocationNeeded = L7_TRUE;

  if (bufferAllocationNeeded == L7_TRUE )
  {

    /* Null pointer - Now get a buffer of sufficient size */
    pBuffer     = osapiMalloc (L7_SIM_COMPONENT_ID, compressionBufferSize);
    if (L7_NULLPTR == pCompressedData)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Failed to allocate %ld for config file compression buffer\n", compressionBufferSize);
      return L7_ERROR;
    }

    bufferAllocated = L7_TRUE;
  }


  retCode = uncompress((L7_uchar8 *)pBuffer, &compressionBufferSize,
      pCompressedData, compressedDataSize);


  /*
   * uncompress returns Z_OK if success,
   * Z_MEM_ERROR if there was not enough memory,
   * Z_BUF_ERROR if there was not enough room in the output buffer, or
   * Z_DATA_ERROR if the input data was corrupted.
   */
  if (Z_OK != retCode)
  {
    sprintf(logBuffer, "Failed to uncompress data buffer - rc = %u ", retCode);
    switch(retCode)
    {
      case Z_MEM_ERROR:
        strcat(logBuffer, "Insufficient memory");
        break;

      case Z_BUF_ERROR:
        strcat(logBuffer, "Insufficient target buffer");
        break;

      case Z_DATA_ERROR:
        strcat(logBuffer, "File corrupt");
        break;

      default:
        break;
    }
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID, logBuffer);
    if (bufferAllocated == L7_TRUE)
      osapiFree(L7_SIM_COMPONENT_ID, (void *) pBuffer);

    return L7_ERROR;

  }


  /* For informational purposes, return the size of the calculated
     Output includes pointer to compression buffer
     Return required buffer length for this operation if it was malloc'ed

   */
  if (bufferAllocated == L7_TRUE)
    *uncompressedBufLen   =  compressionBufferSize;

  *pUncompressedData = pBuffer;


  return L7_SUCCESS;

}



/*********************************************************************
 *
 * @purpose Compress the data
 *
 * @param    pUncompressedData     (@input) pointer to uncompressed data
 * @param    uncompressedDataSize  (@input) length of uncompresed data
 * @param    pCompressedData       (@output) pointer to location to store compressed data
 * @param    *compressedBufLen     (@output) length of buffer storing compressed data
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 * @returns L7_ERROR
 *
 * @notes    pCompressedData should not point to previously allocated memory
 *
 * @notes    Caller is responsible for freeing memory pointed to by pCompressedData
 *
 * @notes    On output, the value of *compressedBufLen is modified
 *
 *
 *
 * @end
 *
 *********************************************************************/
L7_RC_t sysapiCompress(L7_char8 *pUncompressedData, L7_uint32 uncompressedDataSize,
    L7_char8 **pCompressedData,  L7_uint32 *compressedBufLen)
{
  L7_ulong32  compressionBufferSize;
  L7_uint32   retCode;
  L7_BOOL     bufferAllocated;
  L7_BOOL     bufferAllocationNeeded;
  L7_char8    *pBuffer;

  bufferAllocated         = L7_FALSE;
  bufferAllocationNeeded  = L7_FALSE;

  /* compression buffer is required to be 0.1% + 12 bytes larger than the source */
  compressionBufferSize = uncompressedDataSize + uncompressedDataSize/1000 +12;

  pBuffer = *pCompressedData;

  if (L7_NULL == pBuffer)
    bufferAllocationNeeded = L7_TRUE;

  if (bufferAllocationNeeded == L7_TRUE )
  {

    /* Null pointer - Now get a buffer of sufficient size */
    pBuffer     = osapiMalloc (L7_SIM_COMPONENT_ID, compressionBufferSize);
    if (L7_NULLPTR == pBuffer)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Failed to allocate %ld for config file compression buffer\n", compressionBufferSize);
      return L7_ERROR;
    }

    bufferAllocated = L7_TRUE;
  }

  /* a buffer of sufficient size is acquired, now compress the data */
  retCode = compress( (L7_uchar8 *)pBuffer, &compressionBufferSize,pUncompressedData,
      uncompressedDataSize);

  /*
   * compress returns Z_OK if success, Z_MEM_ERROR if there was not enough memory,
   * Z_BUF_ERROR if there was not enough room in the output buffer.
   */
  if (Z_OK != retCode)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to compress data - rc = %u (%s)\n", retCode,
        (Z_MEM_ERROR==retCode)?"Insufficient memory":"Insufficient target buffer");

    if (bufferAllocated == L7_TRUE)
      osapiFree(L7_SIM_COMPONENT_ID, (void *)pBuffer);

    return L7_ERROR;
  }

  *compressedBufLen = compressionBufferSize;
  *pCompressedData = pBuffer;

  return L7_SUCCESS;

}



/*********************************************************************
 *
 * @purpose Separate sysapi cfg file version 1
 *
 * @param    file_desc     (@input) pointer to uncompressed data
 * @param    total_length  (@input) length of compresed data
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 * @returns L7_ERROR
 *
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t sysapiCfgFileSeparateFilesV1(L7_int32 file_desc, L7_int32 total_length)
{

  L7_uint32 i;
  L7_fileHdr_t cfgFileHdr;
  L7_int32 bytes_left;
  L7_int32 file_header_size;
  L7_int32 file_size;
  L7_RC_t  rc;
  void *stored_location;
  L7_uint32 size_to_copy;

  /* Init */

  file_header_size = sizeof (L7_fileHdr_t);


  /*---------------------------------------------------*/
  /* copy component data info over to the memory areas */
  /*---------------------------------------------------*/

  bytes_left = total_length - file_header_size;

  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    /* Read first component config file header.
     */
    rc = osapiFileRead (file_desc, (L7_char8 *) &cfgFileHdr, file_header_size);
    if (rc != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    file_size = cfgFileHdr.length;

    /* Make sure that we don't have unexpected file format change.
     */
    if ((file_size == 0) || (file_size > bytes_left))
    {
      return L7_ERROR;
    }

    {
      L7_uint32 fileindex = 0;
      L7_uchar8    *data_area = L7_NULL;

      rc = sysapiCfgFileIndexGet (cfgFileHdr.filename, &fileindex);
      if (rc == L7_ERROR)
      {
        return L7_ERROR;
      }

      if (pSysapiCfgTree[fileindex]->fileInfo.file_location != L7_NULLPTR)
      {
        osapiFree (cfgFileHdr.componentID,pSysapiCfgTree[fileindex]->fileInfo.file_location);
        pSysapiCfgTree[fileindex]->fileInfo.file_location = L7_NULL;
        data_area = L7_NULL;
      }

      if (data_area == L7_NULL)
      {
        data_area = osapiMalloc (L7_SIM_COMPONENT_ID, file_size);
        if (data_area == L7_NULL)
        {
          return L7_ERROR;
        }
      }

      /* copy cfg data into this new data area
       */
      memcpy (data_area, &cfgFileHdr, file_header_size);

      /*=============================================================*/
      /* Version 1 of the big config file stored data uncompressed.
         Version 2 of the big config file stores data compressed.
         Read the file as version 1 but store as version 2.
       */
      /*=============================================================*/

      stored_location = &data_area[file_header_size];
      size_to_copy    = file_size - file_header_size;

      rc = osapiFileRead (file_desc, stored_location, size_to_copy);
      if (rc != L7_SUCCESS)
      {
        return L7_ERROR;
      }

      /* Version 1 of the big config file was saved as uncompressed.
         It must be stored in the cfg tree as compressed */
      if (sysapiCfgTreeUpdate(cfgFileHdr.componentID, cfgFileHdr.filename, data_area,
            cfgFileHdr.length, pSysapiCfgTree[fileindex])  != L7_SUCCESS)
      {
        L7_RC_t rc2;
        L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

        if ((rc2 = cnfgrApiComponentNameGet(cfgFileHdr.componentID, name)) != L7_SUCCESS)
        {
          osapiStrncpySafe(name, "Unknown", 8);
        }

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "Failed to update cfg tree for component %d, %s and file %s\n",
            cfgFileHdr.componentID, name, cfgFileHdr.filename);

      }

      /* Free the temporary data area */
      osapiFree (L7_SIM_COMPONENT_ID, data_area);
    }

    bytes_left -= file_size;

    /* Terminate the loop after we reach the end of file.
     */
    if (bytes_left < file_header_size)
    {
      break;
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
 *
 * @purpose Separate sysapi cfg file version 2
 *
 * @param    file_desc     (@input) pointer to uncompressed data
 * @param    total_length  (@input) length of compresed data
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 * @returns L7_ERROR
 *
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t sysapiCfgFileSeparateFilesV2(L7_int32 file_desc, L7_int32 total_length)
{
  L7_uint32 i;
  L7_fileHdr_t cfgFileHdr;
  sysapiCfgFileDescr_t  cfgFileDescr;
  L7_int32 bytes_left;
  L7_int32 file_header_size;
  L7_int32 file_descr_size;
  L7_int32 file_size;
  void *stored_location;
  L7_uint32 size_to_copy;
  L7_RC_t  rc;
  L7_uint32 fileindex = 0;
  L7_uchar8 *data_area = L7_NULL;

  /* Init */

  file_header_size = sizeof (L7_fileHdr_t);
  file_descr_size  = sizeof (sysapiCfgFileDescr_t);

  /*---------------------------------------------------*/
  /* copy component data info over to the memory areas */
  /*---------------------------------------------------*/

  bytes_left = total_length - file_header_size;

  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    /*----------------------------------------------------------*/
    /*
       Process the config file descriptor
     */
    /*----------------------------------------------------------*/

    rc = osapiFileRead (file_desc, (L7_char8 *) &cfgFileDescr,
        file_descr_size);
    if (rc != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    if (cfgFileDescr.marker != SYSAPI_CFG_DESCR_MARKER)
    {
      return L7_ERROR;
    }

    file_size = cfgFileDescr.compressedFileSize;

    /* Make sure that we don't have unexpected file format change.
     */
    if ((file_size == 0) || (file_size > bytes_left))
    {
      return L7_ERROR;
    }


    /*----------------------------------------------------------*/
    /*
       Process the component configuration data
     */
    /*----------------------------------------------------------*/


    /* Read component config file header.
     */
    rc = osapiFileRead (file_desc, (L7_char8 *) &cfgFileHdr, file_header_size);
    if (rc != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    if(sysapiTxtCfgExists == L7_TRUE && sysapiValidateCfgFileName(cfgFileHdr.filename) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Ignoring the config '%s'\n", cfgFileHdr.filename);
      osapiFileSeek(file_desc, file_size - file_header_size, SEEK_CUR);
      bytes_left -= file_size;
      continue;
    }

    fileindex =0;
    data_area = L7_NULL;

    rc = sysapiCfgFileIndexGet (cfgFileHdr.filename, &fileindex);
    if (rc == L7_ERROR)
    {
      return L7_ERROR;
    }

    /* Always free any allocation of a pre-existing data area */
    if (pSysapiCfgTree[fileindex]->fileInfo.file_location != L7_NULLPTR)
    {
      osapiFree (L7_SIM_COMPONENT_ID, pSysapiCfgTree[fileindex]->fileInfo.file_location);
      pSysapiCfgTree[fileindex]->fileInfo.file_location = L7_NULLPTR;
      data_area = L7_NULL;
    }

    if (data_area == L7_NULL)
    {
      data_area = osapiMalloc (L7_SIM_COMPONENT_ID, file_size);
      if (data_area == L7_NULL)
      {
        return L7_ERROR;
      }
    }

    /*=============================================================*/
    /* Version 2 is the latest version.  No conversion of data
       needs to be done.  Simply read the config file data into
       the specified data area.
     */
    /*=============================================================*/

    memcpy (data_area, &cfgFileHdr, file_header_size);

    stored_location = &data_area[file_header_size];
    size_to_copy    = file_size - file_header_size;

    rc = osapiFileRead (file_desc, stored_location, size_to_copy);
    if (rc != L7_SUCCESS)
    {
      osapiFree(L7_SIM_COMPONENT_ID, data_area);
      return L7_ERROR;
    }

    /*----------------------------------------------------------*/
    /*
       Update the pSysapiTree information
     */
    /*----------------------------------------------------------*/

    /* Update the file Info */
    pSysapiCfgTree[fileindex]->fileInfo.file_location = data_area;
    strncpy(pSysapiCfgTree[fileindex]->fileInfo.filename, cfgFileHdr.filename, L7_MAX_FILENAME);
    pSysapiCfgTree[fileindex]->fileInfo.filetype = L7_CFG_DATA;
    pSysapiCfgTree[fileindex]->fileInfo.filesize = file_size;

    /* Update the config file descriptor */
    memcpy( (void *)&pSysapiCfgTree[fileindex]->cfgFileDescr,
        (void *)&cfgFileDescr, file_descr_size);

    bytes_left -= file_size;

    /* Terminate the loop after we reach the end of file.
     */
    if (bytes_left < file_header_size)
    {
      break;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Compress the config data and update the sysapiCfgTree
 *
 * @param    component_id   component ID of the file owner.
 * @param    fileName       name of file to load
 * @param    buffer         pointer to uncompressed config data
 * @param    nbytes         number of bytes of uncompressed data
 * @param    *pCfgTreeEntry pointer to pSysapiCfgTree entry
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t sysapiCfgTreeUpdate( L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
    L7_char8 *buffer, L7_uint32 nbytes,
    SYSAPI_DIRECTORY_CFG_t *pCfgTreeEntry)
{
  L7_char8      *dataArea;
  L7_uchar8     *pCompressedData;
  L7_int32      compressedBufLen;
  L7_int32      compressedFileSize;
  L7_RC_t rc2;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc2 = cnfgrApiComponentNameGet(component_id, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }


  /* Initialize info */

  dataArea = L7_NULL;

  /* ----------------------------------------------------------------------- */
  /*
     Compress the component config data
   */
  /* ----------------------------------------------------------------------- */

  pCompressedData            = L7_NULL;
  compressedBufLen            = 0;

  /* Compress the actual data associated with the component. Do not compress the
     file header */
  if ( sysapiCompress( (buffer + sizeof(L7_fileHdr_t) ), nbytes-sizeof(L7_fileHdr_t),
        (L7_char8 **)&pCompressedData, &compressedBufLen) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to compress config file for component %d, %s\n", component_id, name);
    return L7_ERROR;
  }

  /* ------------------------------------------------------------ */
  /* Update the pSysapiCfgTree for this component's configuration
   */
  /* ------------------------------------------------------------ */

  compressedFileSize =  compressedBufLen + sizeof(L7_fileHdr_t);
  if (pCfgTreeEntry->fileInfo.file_location != L7_NULLPTR)
  {
    if (pCfgTreeEntry->cfgFileDescr.compressedFileSize == compressedFileSize)
    {
      dataArea = pCfgTreeEntry->fileInfo.file_location;
    }
    else
    {
      osapiFree (component_id, pCfgTreeEntry->fileInfo.file_location);
      dataArea = L7_NULL;
    }
  }

  if (dataArea == L7_NULL)
  {
    dataArea = osapiMalloc (component_id, compressedFileSize);
    if (dataArea == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Failed to allocate %ld for config file %s\n", compressedFileSize, filename);
      return L7_ERROR;
    }
  }

  /*
   * copy cfg data into this new data area
   * preserve file header
   */
  memcpy(dataArea, (void *)buffer, sizeof(L7_fileHdr_t));
  memcpy(dataArea + sizeof(L7_fileHdr_t), (void *)pCompressedData, compressedBufLen);


  /* Free the memory malloc,ed for temporary compression area */
  osapiFree (L7_SIM_COMPONENT_ID, pCompressedData);

  /* Update the file Info */
  pCfgTreeEntry->fileInfo.file_location = dataArea;
  strncpy(pCfgTreeEntry->fileInfo.filename, filename, L7_MAX_FILENAME);
  pCfgTreeEntry->fileInfo.filetype = L7_CFG_DATA;
  pCfgTreeEntry->fileInfo.filesize = nbytes;


  /* Update the config file descriptor */
  memset( (L7_char8 *)&(pCfgTreeEntry->cfgFileDescr), 0, sizeof(sysapiCfgFileDescr_t));
  pCfgTreeEntry->cfgFileDescr.version            = SYSAPI_CFG_DESCR_VER_CURRENT;
  pCfgTreeEntry->cfgFileDescr.marker             = SYSAPI_CFG_DESCR_MARKER;
  pCfgTreeEntry->cfgFileDescr.compressedFileSize = compressedFileSize;
  pCfgTreeEntry->cfgFileDescr.uncompressedLength = nbytes;

  return L7_SUCCESS;
}

L7_BOOL sysapiTxtCfgEnableGet(void)
{
  return sysapiTxtCfgEnable;
}

void sysapiTxtCfgEnableSet(L7_BOOL val)
{
  sysapiTxtCfgEnable = val;
}

L7_BOOL sysapiTxtCfgValidGet(void)
{
  return sysapiTxtCfgValid;
}

void sysapiTxtCfgValidSet(L7_BOOL val)
{
  sysapiTxtCfgValid = val;
}

L7_char8 *sysapiTxtCfgFileGet(void)
{
  if(sysapiTxtCfgEnable == L7_TRUE)
  {
    return "startup-config";
  }
  return "TxtCfgDsbld.err";
}
L7_char8 *sysapiTxtRunCfgFileGet(void)
{
  if(sysapiTxtCfgEnable == L7_TRUE)
  {
    return "running-config";
  }
  return "TxtRunCfgDsbld.err";
}

static void sysapiTxtCfgInit(void)
{
  L7_uint32 startupFileLen;

  sysapiTxtCfgEnable = L7_TRUE;
  sysapiTxtCfgValid = L7_TRUE;

  if (osapiFsFileSizeGet("startup-config", &startupFileLen) == L7_SUCCESS)
  {
    sysapiTxtCfgExists = L7_TRUE;
  }
}

static L7_BOOL sysapiValidateCfgFileName(L7_char8 *filename)
{
  L7_BOOL valid = L7_TRUE;
  L7_uint32 fileindex;
  L7_uint32 maxCount;

  if (sysapiTxtCfgEnable == L7_TRUE)
  {
    valid = L7_FALSE;
    maxCount = sizeof(sysapiBinCfgFileList)/sizeof(sysapiBinCfgFileList[0]);
    for (fileindex = 0; fileindex < maxCount; fileindex++)
    {
      if(strcmp(filename,sysapiBinCfgFileList[fileindex]) == 0 )
      {
        valid = L7_TRUE;
        break;
      }
    }
  }

  return valid;
}






