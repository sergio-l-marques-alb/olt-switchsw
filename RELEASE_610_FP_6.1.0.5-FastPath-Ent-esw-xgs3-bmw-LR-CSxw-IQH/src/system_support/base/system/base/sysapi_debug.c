/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  sysapi_debug.c
*
* @purpose sysapi debug routines
*
* @component sysapi
*
* @create       8/23/2004
*
* @author       Rama Sasthri, Kristipati
*
* @end
*
**********************************************************************/

#include "log_api.h"
#include "sysapi.h"
#include "sysapi_util.h"

extern SYSAPI_DIRECTORY_CFG_t *pSysapiCfgTree[SYSAPI_MAX_FILE_ENTRIES];
extern void *sysapiCfgSema;

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
void sysapiCfgFileDump (void)
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
        if (pSysapiCfgTree[i]->fileInfo.filename[0] != 0)
        {
            pFileHdr = (L7_fileHdr_t *)(pSysapiCfgTree[i]->fileInfo.file_location);
            sysapiPrintf("%24s  %6u        %6u",
                   pSysapiCfgTree[i]->fileInfo.filename,
                   pSysapiCfgTree[i]->fileInfo.filesize,
                   pSysapiCfgTree[i]->cfgFileDescr.compressedFileSize);
            compressedFileSize += pSysapiCfgTree[i]->cfgFileDescr.compressedFileSize + sizeof(sysapiCfgFileDescr_t);
            
            if (pSysapiCfgTree[i]->fileInfo.file_location != L7_NULLPTR)
            {
              sysapiPrintf("    %3u    %2u      %1u   %6u   %6u\n", pFileHdr->componentID,
                           pFileHdr->version, pFileHdr->dataChanged, pFileHdr->length, 
                           pSysapiCfgTree[i]->cfgFileDescr.uncompressedLength);
            }
            else
            {
              sysapiPrintf("\n");
            }
            filesize += pSysapiCfgTree[i]->fileInfo.filesize;
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
void sysapiCfgFileDebugConfigDeploy (L7_char8 * cfgFile)
{
  if(cfgFile == NULL)
	cfgFile = "bigc";

  if (sysapiCfgFileDeploy (cfgFile) != L7_SUCCESS)
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
void sysapiCfgFileHeaderDump (L7_fileHdr_t  *pFileHdr)
{

    L7_uint32     i;


    if (pFileHdr == L7_NULLPTR) 
    {
        sysapiPrintf("Error: pFileHdr is NULL\n");
        return;
    }

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
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

    return;

}
