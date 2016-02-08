/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename nvstore.c
 *
 * @purpose Global Save and Restore Manager
 *
 * @component nvStore
 *
 * @comments none
 *
 * @create 08/16/2000
 *
 * @author bmutz
 * @end
 *
 **********************************************************************/

#include <string.h>                /* for memcpy() etc... */
#include "l7_common.h"
#include "nvstoreapi.h"
#include "nvstore.h"
#include "l7_product.h"
#include "registry.h"
#include "sysapi.h"
#include "osapi.h"
#include "log.h"
#include "unitmgr_api.h"
#include "transfer_exports.h"

/* for nvStoreDebugDataChanged function*/
#include "usmdb_cnfgr_api.h"

extern void sysapiUncombinedCfgRemove(void);

extern L7_BOOL sysapiGlobalDataChangedFlag;
extern L7_char8 *nimDebugCompStringGet(L7_COMPONENT_IDS_t cid);
static nvStoreFunctionList_t   nvStoreNotifyList[L7_LAST_COMPONENT_ID];

static void (*nvStoreTxtCfgNotifySave)(L7_char8 *filename) = L7_NULLPTR;


/*********************************************************************
 * @purpose  Register a components non-volitale save routine and
 *           factory defaults restore routine
 *
 * @param    notifyFunctionList   pointer to structure with function pointers
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_RC_t nvStoreRegister(nvStoreFunctionList_t notifyFunctionList)
{
  if (notifyFunctionList.registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("nvStore registrar_ID %d greater then NVSTORE_USER_LAST\n", 
           (L7_int32)notifyFunctionList.registrar_ID);
    return(L7_FAILURE);
  }

  if (nvStoreNotifyList[notifyFunctionList.registrar_ID].registrar_ID != 0)
  {
    LOG_MSG("nvStore registrar_ID %d already registered\n", (L7_int32)notifyFunctionList.registrar_ID);
    return(L7_FAILURE);
  }

  memcpy(&nvStoreNotifyList[notifyFunctionList.registrar_ID], &notifyFunctionList, 
         sizeof(nvStoreFunctionList_t));

  return(L7_SUCCESS);

}  

/*********************************************************************
 * @purpose  Register a components non-volitale save routine and
 *           factory defaults restore routine
 *
 * @param    *notifySave    pointer to a routine to be invoked for saves     
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_RC_t nvStoreTxtCfgRegister(void (*notifySave)( L7_char8 *filename ))
{
  nvStoreTxtCfgNotifySave = notifySave;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Deregister a component from nvStore
 *
 * @param    componentId     one of L7_COMPONENT_IDS_t
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_RC_t nvStoreDeregister(L7_COMPONENT_IDS_t componentId)
{
  if (componentId >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("nvStore registrar_ID %d greater then NVSTORE_USER_LAST\n", componentId);
    return(L7_FAILURE);
  }

  if (nvStoreNotifyList[componentId].registrar_ID == 0)
  {
    LOG_MSG("nvStore registrar_ID %d not registered\n", componentId);
    return(L7_FAILURE);
  }

  memset(&nvStoreNotifyList[componentId],0, sizeof(nvStoreFunctionList_t));

  return(L7_SUCCESS);

}  

/*********************************************************************
 * @purpose  Call all components non-volitale save routine
 *
 * @param    generateConfig      type of config to save
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_RC_t nvStoreSave(L7_SAVE_CONFIG_TYPE_t generateConfig)
{
  L7_uint32 i;
  L7_RC_t   rc = L7_SUCCESS;

  if ((L7_SAVE_CONFIG_TYPE_STARTUP == generateConfig) ||
      (L7_SAVE_CONFIG_TYPE_RUNNING == generateConfig))
  {
    for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
    {
      if (nvStoreNotifyList[i].registrar_ID != 0)
      {
        if (nvStoreNotifyList[i].notifySave != L7_NULLPTR)
        {
          if ((*nvStoreNotifyList[i].notifySave)() != L7_SUCCESS)
          {
            LOG_MSG("bad rc on Save call to registrar_ID %d\n",(L7_int32)nvStoreNotifyList[i].registrar_ID);
            LOG_EVENT(nvStoreNotifyList[i].registrar_ID);
          }
        }
      }
    }
  }

#ifndef PROD_USE_BINARY_CONFIGURATION  
  if (nvStoreTxtCfgNotifySave != L7_NULLPTR)
  {
    /* Generate text-based configuration file. */
    switch (generateConfig) {
    case L7_SAVE_CONFIG_TYPE_STARTUP:
      sysapiGlobalDataChangedFlag = L7_FALSE;
      sysapiClearConfigFlagSet(L7_FALSE);
      /* remove the old configuration files if any present */
      sysapiUncombinedCfgRemove();
      nvStoreTxtCfgNotifySave(L7_SCRIPTS_STARTUP_CONFIG_FILENAME);
      if (sysapiCfgFileSave(SYSAPI_CONFIG_FILENAME) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
      }
      else
      {
        osapiFsSync (); /* Wait until all config files are saved to the file system */
        /* propagate configuration files to connected units incase of failover */
        unitMgrPropagateCfg(UNITMGR_CFG_TYPE_SYSTEM);
      }
      break;
    case L7_SAVE_CONFIG_TYPE_BACKUP:
      nvStoreTxtCfgNotifySave(L7_SCRIPTS_BACKUP_CONFIG_FILENAME);
      break;
    case L7_SAVE_CONFIG_TYPE_TEMP:
      nvStoreTxtCfgNotifySave(L7_SCRIPTS_TEMP_CONFIG_FILENAME);
      break;
    case L7_SAVE_CONFIG_TYPE_RUNNING:
      nvStoreTxtCfgNotifySave(sysapiTxtRunCfgFileGet());
      if (sysapiCfgFileSave(SYSAPI_CONFIG_RUNING_FILENAME) != L7_SUCCESS)
        rc = L7_FAILURE;
      break;
    default:
      break;
    }
  }
#else
  sysapiGlobalDataChangedFlag = L7_FALSE;
  /* remove the old configuration files if any present */
  sysapiUncombinedCfgRemove();
  if (sysapiCfgFileSave(SYSAPI_CONFIG_FILENAME) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  osapiFsSync (); /* Wait until all config files are saved to the file system */
#endif /* L7_PRODUCT_SMARTPATH */
  return(rc);
}

/*********************************************************************
 * @purpose  Check if all components user config data has changed
 *
 * @param    void      
 *
 * @returns  L7_BOOL  L7_TRUE or L7_FALSE
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_BOOL nvStoreHasDataChanged(void)
{
  L7_uint32 i;
  L7_BOOL hasChanged = L7_FALSE;
  
  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (nvStoreNotifyList[i].registrar_ID != 0)
    {
      if (nvStoreNotifyList[i].hasDataChanged != L7_NULLPTR)
      {
        if ((*nvStoreNotifyList[i].hasDataChanged)() == L7_TRUE)
        {
          hasChanged = L7_TRUE;
          break;
        }
      }
    }
  }
  if (hasChanged == L7_FALSE && sysapiGlobalDataChangedFlag == L7_TRUE)
  {
    hasChanged = L7_TRUE;
  }
  return hasChanged;
}

L7_BOOL nvStoreHasDataChangedSinceLastRunCfg(void)
{
  L7_uint32 i;
    
  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (nvStoreNotifyList[i].registrar_ID != 0)
    {
      if (nvStoreNotifyList[i].hasDataChanged != L7_NULLPTR)
      {
        if ((*nvStoreNotifyList[i].hasDataChanged)() == L7_TRUE)
        {
          return L7_TRUE;
        }
      }
    }
  }
  return L7_FALSE;
}
void nvStoreResetDataChanged(void)
{
  L7_uint32 i;

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (nvStoreNotifyList[i].registrar_ID != 0)
    {
      if (nvStoreNotifyList[i].resetDataChanged != L7_NULLPTR)
      {
        (*nvStoreNotifyList[i].resetDataChanged)();
      }
    }
  }
  return;
}

void nvStoreResetCompDataChanged(L7_COMPONENT_IDS_t componentId)
{
  if (componentId <=0 || componentId >= L7_LAST_COMPONENT_ID)
  {
    return;
  }
  if (nvStoreNotifyList[componentId].registrar_ID == componentId)
  {
    if (nvStoreNotifyList[componentId].resetDataChanged != L7_NULLPTR)
      {
        (*nvStoreNotifyList[componentId].resetDataChanged)();
      }
  }
  return;
}

/*********************************************************************
 * @purpose  Print all components where user config data has changed
 *
 * @param    void      
 *
 * @returns  void
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
void nvStoreDebugDataChanged(void)
{
  L7_uint32 i;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];
  L7_COMPONENT_IDS_t component_id;
  L7_RC_t rc= L7_FAILURE;

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (nvStoreNotifyList[i].registrar_ID != 0 && nvStoreNotifyList[i].hasDataChanged != L7_NULLPTR)
    {
      if ((*nvStoreNotifyList[i].hasDataChanged)() == L7_TRUE)
      {
        bzero(name, L7_COMPONENT_NAME_MAX_LEN); 
        component_id = nvStoreNotifyList[i].registrar_ID;
        if ((rc=usmDbComponentNameGet(component_id, name)) == L7_SUCCESS)
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "\n%25s (%d) has data changed flag set.",name, 
                        (L7_int32)component_id);
        }
        else
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "\nComponent %d has data changed flag set.", 
                        (L7_int32)component_id);
        }
      }
    }
  }
}

/*********************************************************************
 * @purpose  Call all components dump config routine
 *
 * @param    void      
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_RC_t nvStoreConfigDump(void)
{
  L7_uint32 i, count = 0;

  for (i = 0; i < (L7_uint32)L7_LAST_COMPONENT_ID; i++)
  {
    if (nvStoreNotifyList[i].registrar_ID != 0)
    {
      if ((L7_uint32)nvStoreNotifyList[i].notifyConfigDump != L7_NULL)
      {
        if ((*nvStoreNotifyList[i].notifyConfigDump)() != L7_SUCCESS)
        {
          count++;
          LOG_MSG("bad rc on config dump call to registrar_ID %d\n",
                  (L7_int32)nvStoreNotifyList[i].registrar_ID);
          LOG_EVENT(nvStoreNotifyList[i].registrar_ID);
        }
      }
    }
  }

  return count ? L7_FAILURE : L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Call all components dump debug routine
 *
 * @param    void      
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_RC_t nvStoreDebugDump(void)
{
  L7_uint32 i, count = 0;

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (nvStoreNotifyList[i].registrar_ID != 0)
    {
      if ((L7_uint32)nvStoreNotifyList[i].notifyDebugDump != L7_NULL)
      {
        if ((*nvStoreNotifyList[i].notifyDebugDump)() != L7_SUCCESS)
        {
          count++;
          LOG_MSG("bad rc on debug dump call to registrar_ID %d\n",
                  (L7_int32)nvStoreNotifyList[i].registrar_ID);
          LOG_EVENT(nvStoreNotifyList[i].registrar_ID);
        }
      }
    }
  }
  return count ? L7_FAILURE : L7_SUCCESS;
}

/*********************************************************************
 * @purpose  generate crc32 value
 *
 * @param    blk_adr  address of block of data      
 * @param    blk_len  length of block of data      
 *
 * @returns  crc32Value
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_uint32 nvStoreCrc32( L7_uchar8 * blk_adr, L7_uint32 blk_len )
{
  L7_uint32 crc = (L7_uint32)INIT_REFLECTED;

  while (blk_len--)
  {
    crc = crctable[(crc ^ *blk_adr++) & 0xFFL] ^ (crc >> 8);
  }
  return crc ^ XOROT;
}

/*********************************************************************
 * @purpose  Set starting value for the cumulative CRC computation.
 *
 * @returns  crc32Value
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_uint32 nvStoreCrcInit (void)
{
  return (L7_uint32) INIT_REFLECTED;
}

/*********************************************************************
 * @purpose  Update cumulative CRC.
 *
 * @param    old_crc  Previous CRC value.
 * @param    octet    New data byte.
 *
 * @returns  New CRC value.
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_uint32 nvStoreCrcUpdate (L7_uint32 old_crc, L7_uchar8 octet)
{
  L7_uint32 crc;

  crc = crctable [(old_crc ^ octet) & 0xFFL] ^ (old_crc >> 8);

  return crc;
}

/*********************************************************************
 * @purpose  Finish cumulative CRC computation.
 *
 * @param    old_crc  Previous CRC value.
 *
 * @returns  Final CRC for the data block.
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_uint32 nvStoreCrcFinish (L7_uint32 old_crc)
{
  return old_crc ^ XOROT;
}

/*********************************************************************
 * @purpose  Get's requested file form file system. If file does not 
 *           exists or is corrupt,  will build Factory Default file
 *
 * @param    fileName       name of file requesting
 * @param    buffer         pointer to location file will be placed
 * @param    bufferSize     size of buffer
 * @param    checkSum       pointer to location of checksun in buffer
 * @param    version        software version of the file      
 * @param    defaultBuild   function ptr to factory default build routine
 *                          if L7_NULL do not build default file      
 *
 * @returns  L7_RC_t        L7_SUCCESS ot L7_FAILURE
 *
 * @notes    routine assumes checkSum is last 4 bytes of buffer
 *                                 
 * @end
 *********************************************************************/
L7_RC_t nvStoreGetFile(L7_char8 *fileName, L7_char8 *buffer, L7_uint32 bufferSize, 
    L7_uint32 *checkSum, L7_uint32 version, void (*defaultBuild)(L7_uint32))
{
  L7_uint32 chkSum;

  if (osapiFsRead(fileName, buffer, (L7_int32)bufferSize) == L7_ERROR)
  {
    if ((L7_uint32)defaultBuild == L7_NULL)
    {
      return(L7_FAILURE);
    }

	L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "Building defaults for file %s."
			" A component’s configuration file does not exist or the file’s checksum is incorrect"
			" so the component’s default configuration file is built.\n",fileName);

    (*defaultBuild)(version);

    *checkSum = nvStoreCrc32((L7_uchar8 *)buffer, bufferSize-sizeof(L7_uint32));

    if (osapiFsWrite(fileName, buffer, (L7_int32)bufferSize) == L7_ERROR)
    {
	  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "Error on call to osapiFsWrite routine "
							 "on file %s. Either the file can not be opened or the OS’s file I/O returned"
							 " an error trying to write to the file.\n",fileName);
    }
  }
  else
  {
    chkSum = nvStoreCrc32((L7_uchar8 *)buffer, bufferSize-sizeof(L7_uint32));

    if (chkSum != *checkSum)
    {
	  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "File %s corrupted from file system.  "
					"Checksum mismatch. The calculated checksum of a component’s configuration file in "
					"the file system did not match the checksum of the file in memory.\n", fileName);


      if ((L7_uint32)defaultBuild == L7_NULL)
      {
        return(L7_FAILURE);
      }

	  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "Building defaults for file %s. A component’s "
							"configuration file does not exist or the file’s checksum is incorrect so the "
							 "component’s default configuration file is built.\n",fileName);

      (*defaultBuild)(version);

      *checkSum = nvStoreCrc32((L7_uchar8 *)buffer, bufferSize-sizeof(L7_uint32));

      if (osapiFsWrite(fileName, buffer, (L7_int32)bufferSize) == L7_ERROR)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID, "Error on call to osapiFsWrite routine "
                "on file %s. Either the file can not be opened or the OS’s file I/O returned "
                "an error trying to write to the file.\n",fileName);
      }
    }
  }
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Erase startup-config file
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nvStoreEraseStartupConfig(void)
{
  if (osapiFsDeleteFile(SYSAPI_TXTCFG_FILENAME) != L7_SUCCESS)
  {
    LOG_MSG("%s: LINE %d: Could not delete file startup-config\n",
            __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*****************************************************************************
  se  Call a specified components non-volitale save routine
 *
 * @param    void
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t nvStoreComponentSave(L7_COMPONENT_IDS_t componentId)
{
  L7_uint32 count = 0;

  if ( componentId  >=  L7_LAST_COMPONENT_ID)
  {
    return L7_FAILURE;
  }

  componentId = componentId - L7_FIRST_COMPONENT_ID;

  if (nvStoreNotifyList[componentId].registrar_ID != 0)
  {
    if (nvStoreNotifyList[componentId].notifySave != L7_NULLPTR)
    {
      if ((*nvStoreNotifyList[componentId].notifySave)() != L7_SUCCESS)
      {
        count++;
        LOG_MSG("bad rc on Save call to registrar_ID %d\n",
                (L7_int32)nvStoreNotifyList[componentId].registrar_ID);
        LOG_EVENT(nvStoreNotifyList[componentId].registrar_ID);
      }
    }
  }

  osapiFsSync (); /* Wait until all config files are saved to the file system */

  return count ? L7_FAILURE : L7_SUCCESS;
}

L7_RC_t nvStoreComponentSaveAll(void)
{
  L7_uint32 i;

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    nvStoreComponentSave (i);
  }
  return L7_SUCCESS;
}

L7_BOOL nvStoreHasComponentDataChanged(L7_COMPONENT_IDS_t componentId )
{
  if (( componentId < L7_LAST_COMPONENT_ID) &&
      ( 0 != nvStoreNotifyList[componentId].registrar_ID) &&
      ( L7_NULLPTR != nvStoreNotifyList[componentId].hasDataChanged))
  {
    return((*nvStoreNotifyList[componentId].hasDataChanged)());
  }

  return( L7_FALSE );
}
/* Check if any registered component has failed to register a 
 * non null function pointer to reset data
 * this routine is useful when new components are added but
 * the developer fails to code the reset data changed flag function
 */
void nvStoreDebugResetDataRegistrationCheck(void)
{
  L7_uint32 i;
  
  printf("\nnvStoreDebugResetDataRegistrationCheck\n");
  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (nvStoreNotifyList[i].registrar_ID != 0)
    {
      if (nvStoreNotifyList[i].resetDataChanged == L7_NULLPTR)
      {
        printf("CompID %d Comp %s has not registerd a resetDataChanged function.\n", 
               nvStoreNotifyList[i].registrar_ID, 
               nimDebugCompStringGet(nvStoreNotifyList[i].registrar_ID));
      }
    }
  }

  return;

}

