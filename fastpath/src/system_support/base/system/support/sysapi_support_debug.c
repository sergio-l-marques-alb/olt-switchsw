/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  sysapi_support_debug.c
*
* @purpose sysapi support debug routines
*
* @component sysapi
*
* @create       8/15/2006
*
* @author       W. Jacobs
*
* @end
*
**********************************************************************/

#include "log_api.h"
#include "sysapi.h"
#include "sysapi_util.h"
#include "support_api.h"
#include "osapi.h"
#include "dtlapi.h"
#include "l7_cnfgr_api.h"

extern SYSAPI_DIRECTORY_CFG_t *pSysapiSupportCfgTree[SYSAPI_MAX_FILE_ENTRIES];


extern L7_RC_t sysapiSupportCfgFileDeploy(L7_char8 *bigCfn);

void sysapiSupportCfgFileDump (void);
void sysapiCfgFileDebugConfigDeploy (L7_char8 * cfgFile);



/*********************************************************************
* @purpose  displays the configuration file information
*
* @param    NONE
*
* @returns  NONE
*
* @notes
*
* @end
*********************************************************************/
void sysapiSupportCfgFileDump (void)
{
  L7_uint32 i;
  L7_uint32  filesize = 0;
  L7_fileHdr_t *pFileHdr;
  L7_uint32  compressedFileSize = 0;

  sysapiPrintf("\n                                     Cfg Descr                                            Cfg Descr");
  sysapiPrintf("\n                          Original   Compressed                                  Uncompressed");
  sysapiPrintf("\n                 File       Size        Size     CompID  Ver  Changed    Len         Len         \n");
  sysapiPrintf("\n                 ----     --------    ---------  ------  ---  -------    ---     --------------  \n");
  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    if (pSysapiSupportCfgTree[i]->fileInfo.filename[0] != 0)
    {
      pFileHdr = (L7_fileHdr_t *)(pSysapiSupportCfgTree[i]->fileInfo.file_location);
      sysapiPrintf("%24s  %6u        %6u",
                   pSysapiSupportCfgTree[i]->fileInfo.filename,
                   pSysapiSupportCfgTree[i]->fileInfo.filesize,
                   pSysapiSupportCfgTree[i]->cfgFileDescr.compressedFileSize);
      compressedFileSize += pSysapiSupportCfgTree[i]->cfgFileDescr.compressedFileSize + sizeof(sysapiCfgFileDescr_t);

      if (pSysapiSupportCfgTree[i]->fileInfo.file_location != L7_NULLPTR)
      {
        sysapiPrintf("    %3u    %2u      %1u   %6u   %6u\n", pFileHdr->componentID,
                     pFileHdr->version, pFileHdr->dataChanged, pFileHdr->length, 
                     pSysapiSupportCfgTree[i]->cfgFileDescr.uncompressedLength);
      }
      else
      {
        sysapiPrintf("\n");
      }
      filesize += pSysapiSupportCfgTree[i]->fileInfo.filesize;
    }
  }


  sysapiPrintf("\n");
  sysapiPrintf("%24s  %6u        %6u\n",
               "Summation", filesize, compressedFileSize);


  sysapiPrintf("\n");
  sysapiPrintf("%s version:       %d \n", SYSAPI_CONFIG_FILENAME, SYSAPI_CFG_VER_CURRENT);
  sysapiPrintf("sysapiCfgDescriptor ersion:     %d \n", SYSAPI_CFG_DESCR_VER_CURRENT);
}


/*********************************************************************
* @purpose  deploys configuration from bigc file if it is already in flash
*
* @param    cfgFile configuration file name
*
* @returns  NONE
*
* @notes
*
* @end
*********************************************************************/
void sysapiSupportCfgFileDebugConfigDeploy (L7_char8 * cfgFile)
{
  if (cfgFile == NULL)
    cfgFile = "bigc";

  if (sysapiSupportCfgFileDeploy (cfgFile) != L7_SUCCESS)
  {
    SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_ALWAYS, "%s", "FAIL: sysapiCfgFileDebugConfigDeploy\n");
  }
  else
  {
    SYSAPI_PRINTF (SYSAPI_APPLICATION_LOGGING_ALWAYS, "%s", "PASS: sysapiCfgFileDebugConfigDeploy\n");
  }
}         


/*********************************************************************
* @purpose  displays the configuration file header information
*
* @param    pFileHdr    Pointer to an L7_fileHdr_t structure
*
* @returns  NONE
*
* @notes
*
* @end
*********************************************************************/
void sysapiSupportDebugCategoryHelp (void)
{
  SUPPORT_CATEGORIES_t category;
  L7_COMPONENT_IDS_t componentId;
  void* supportRoutine;
  supportDebugCategory_t supportDebugCategory;
  L7_RC_t  rc;
  L7_char8 categoryName[SUPPORT_CATEGORY_NAME_MAX_LEN];
  L7_char8 componentName[L7_COMPONENT_NAME_MAX_LEN];

  sysapiPrintf("\n                                                           ");
  sysapiPrintf("\n                                                         Routine ");
  sysapiPrintf("\n Category   Component      Routine Name                  Address  ");
  sysapiPrintf("\n --------  -----------     ------------                  ---------");
  sysapiPrintf("\n");

  category = 0;
  componentId = 0;
  supportRoutine = L7_NULLPTR;
  memset(&supportDebugCategory, 0x00, sizeof(supportDebugCategory));

  while ( supportDebugCategoryGetNext( category, componentId, supportRoutine,
                                       &supportDebugCategory) == L7_SUCCESS)
  {
    category = supportDebugCategory.category;
    componentId = supportDebugCategory.componentId;
    supportRoutine = supportDebugCategory.info.supportRoutine;

    rc = supportDebugCategoryNameGet(category,categoryName);
    if (rc != L7_SUCCESS)
    {
      osapiStrncpySafe(categoryName,"UNKNOWN",SUPPORT_CATEGORY_NAME_MAX_LEN);
    }

    rc = cnfgrApiComponentNameGet(componentId, componentName);
    if (rc != L7_SUCCESS)
    {
      osapiStrncpySafe(componentName,"UNKNOWN",L7_COMPONENT_NAME_MAX_LEN);

    }

    sysapiPrintf("  %-8.7s %-13.12s %-26.25s %-11.10x\n", 
                 categoryName,componentName, 
                 supportDebugCategory.info.supportRoutineName,
                 supportDebugCategory.info.supportRoutine);

  } /* while supportDebugCategoryGetNext() */

}

/*********************************************************************
* @purpose  Print descriptions of all registered help routines
*
* @param    void      
*
* @returns  void
*
* @notes 
*                                 
* @end
*********************************************************************/
void supportDebugDevshellHelp(void)
{
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];
  L7_COMPONENT_IDS_t componentId;
  void* debugHelp;
  L7_uchar8  debugHelpRoutineName[SUPPORT_DEBUG_HELP_NAME_SIZE];
  L7_uchar8  debugHelpDescr[SUPPORT_DEBUG_HELP_DESCR_SIZE]; 

  /* Initialization */
  componentId = 0;
  debugHelp = L7_NULLPTR;
  memset(&debugHelpRoutineName,0, SUPPORT_DEBUG_HELP_NAME_SIZE);
  memset(&debugHelpDescr,0, SUPPORT_DEBUG_HELP_DESCR_SIZE);


  sysapiPrintf("\n");
  sysapiPrintf("Component  DebugHelp Routine              Description      \n");
  sysapiPrintf("---------  -----------------              -----------      \n");
  sysapiPrintf("\n");
  /* Keep looping while there are entries in the arp table */
  while (supportDebugHelpGetNext(&componentId,&debugHelp, 
                                 debugHelpRoutineName,debugHelpDescr)== L7_SUCCESS)
  {

    bzero(name, L7_COMPONENT_NAME_MAX_LEN); 
    if (cnfgrApiComponentNameGet(componentId, name) != L7_SUCCESS)
    {
      osapiStrncpySafe(name, "UNKNOWN",L7_COMPONENT_NAME_MAX_LEN );
    }
    sysapiPrintf(" %-9.8s %-30.29s %-44.43s\n", 
                 name,debugHelpRoutineName, debugHelpDescr);
  } /* while (supportDebugDevshellHelpGetNext) */

}

/*********************************************************************
* @purpose  Execute commands on a driver shell
*
* @param    str    string needed to be executed on the debug shell
*
* @returns 
*
* @notes
*
* @end
*********************************************************************/
void sysapiDebugDriverShell(const L7_uchar8 str[])
{
    dtlDriverShell(str);
    return;
}

