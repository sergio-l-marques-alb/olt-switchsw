/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  sysapi_support.c
*
* @purpose   Basic support/debug infrastructure
*
* @component sysapi_support
*
* @create    08/03/2006
*
* @author    wjacobs
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
#include "zlib.h"
#include "sysapi_util.h"
#include "sysnet_api.h"
#include "osapi.h"


#define  SYSAPI_FILE_BUF_SIZE   1024

extern int  atoi (const L7_char8 *__s);
extern void sysapiDebugSysInfoDumpRegister(void);

/*  System config mgmt parms */
SYSAPI_DIRECTORY_CFG_t *pSysapiSupportCfgTree[SYSAPI_MAX_FILE_ENTRIES];
void * sysapiSupportCfgSema = L7_NULL;

/* Static Declarations */
typedef struct sysapiDebugSupport_s
{
  L7_uint32 printfDebugMode;
}sysapiDebugSupport_t;

static sysapiDebugSupport_t sysapiDebugSupport;

static L7_RC_t sysapiSupportCfgFileSeparate(L7_char8 *cfgFile);
static L7_RC_t sysapiSupportCfgTreeUpdate( L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
                                          L7_char8 *buffer, L7_uint32 nbytes,
                                          SYSAPI_DIRECTORY_CFG_t *pCfgTreeEntry);

static L7_RC_t sysapiSupportCfgFileSeparateFilesV2(L7_int32 file_desc, L7_int32 total_length);

L7_RC_t sysapiDebugSupportInit(void);

L7_RC_t sysapiSupportCfgFileIndexGet(const L7_char8 *fileName, L7_uint32 *pFileIndex);


extern L7_RC_t supportDebugCategoryListInit(void);

/**************************************************************************
*
* @purpose  Enable or disable sysapi debug display mode
*
* @param    mode    L7_ENABLE or L7_DISABLE
*
* @returns  void
*
* @notes   The setting of this mode controls whether certain types of messages
*          (e.g. internal trace messages, debug dumps) may be displayed on
*          CLI login sessions which are enabled for such display.

*
*          This mode must be set to enabled if there are any sessions enabled
*          for debug display.  Otherwise, set to disabled.
*
*
* @end
*
*************************************************************************/
void sysapiDebugSupportDisplayModeSet(L7_uint32 mode)
{
  sysapiDebugSupport.printfDebugMode = mode;
}


/**************************************************************************
*
* @purpose  Get sysapi debug display mode
*
* @param    void
*
* @returns  mode    L7_ENABLE or L7_DISABLE
*
* @notes   The setting of this mode controls whether certain types of messages
*          (e.g. internal trace messages, debug dumps) may be displayed on
*          CLI login sessions which are enabled for such display.
*
* @end
*
*************************************************************************/
L7_uint32 sysapiDebugSupportDisplayModeGet(void)
{
  return(sysapiDebugSupport.printfDebugMode);
}

/**************************************************************************
*
* @purpose  Initialize the debug support infrastructure
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiDebugSupportInit(void)
{
  memset((L7_char8 *)&sysapiDebugSupport, 0, sizeof(sysapiDebugSupport));

  if ( supportDebugCategoryListInit() != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* The infrastructure for debugging has been initialized.
     Do our own registration. */

  sysapiDebugSysInfoDumpRegister();

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
void sysapiSupportCfgFileReload(void)
{
  L7_RC_t rc;

  rc = sysapiSupportCfgFileSeparate(SYSAPI_SUPPORT_CONFIG_FILENAME);

  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }
}


/*********************************************************************
*
* @purpose Read the Big Config file from the ramdisk and parse it out
*          into all the component config files on the ramdisk
*
* @param   bigCfn   @b{(input)} Big Config file
*
* @returns L7_SUCCESS
* @returns L7_ERROR  there were problems
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t sysapiSupportCfgFileDeploy(L7_char8 *bigCfn)
{

  return L7_NOT_SUPPORTED;

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
L7_RC_t sysapiSupportCfgFileSysInit(void)
{
  L7_uint32 i;
  L7_uint32 rc;

  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    pSysapiSupportCfgTree[i] = (SYSAPI_DIRECTORY_CFG_t *)osapiMalloc(L7_SIM_COMPONENT_ID, (L7_uint32)sizeof(SYSAPI_DIRECTORY_CFG_t));
    if (pSysapiSupportCfgTree[i] == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Failed to malloc pSysapiSupportCfgTree[%u]\n", i);
      L7_LOG_ERROR(1);
      return L7_ERROR;
    }
    else
    {
      memset(pSysapiSupportCfgTree[i], 0x00, sizeof(SYSAPI_DIRECTORY_CFG_t));

      pSysapiSupportCfgTree[i]->fileInfo.filename = osapiMalloc(L7_SIM_COMPONENT_ID, L7_MAX_FILENAME);
      if ( pSysapiSupportCfgTree[i]->fileInfo.filename == L7_NULLPTR)
      {
        L7_LOG_ERROR (0);
      }
      pSysapiSupportCfgTree[i]->fileInfo.filename[0] = 0;
    }
  }


  sysapiSupportCfgSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  rc = sysapiSupportCfgFileSeparate(SYSAPI_SUPPORT_CONFIG_FILENAME);
  if (L7_SUCCESS == rc)
    return rc;

/* Message is not of interest to normal users.
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
         "could not separate %s\n", SYSAPI_SUPPORT_CONFIG_FILENAME); */
  return L7_FAILURE;

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
static L7_RC_t sysapiSupportCfgFileSeparate(L7_char8 *cfgFile)
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

  (void)osapiSemaTake(sysapiSupportCfgSema, L7_WAIT_FOREVER);


  /*-----------------------------------------------*/
  /* get the size of the data area, if file exists */
  /*-----------------------------------------------*/

  if (osapiFsFileSizeGet(cfgFile, &total_length) != L7_SUCCESS)
  {
    /* No log mesage needed here as the debug config file
       does not necessarily exist */
    (void)osapiSemaGive(sysapiSupportCfgSema);
    return L7_FAILURE;
  }

  if (total_length < file_header_size)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Support configuration file size %u is toos small.\n",
             total_length);
    (void)osapiSemaGive(sysapiSupportCfgSema);
    return L7_ERROR;
  }

  memset(&cfgFileHdr, 0, sizeof(cfgFileHdr));
  rc = osapiFsRead (cfgFile, (L7_char8 *) &cfgFileHdr, file_header_size);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to read support config file %s header\n", cfgFile);
    (void)osapiSemaGive(sysapiSupportCfgSema);
    return L7_ERROR;
  }

  if ((0 != cfgFileHdr.fileHdrVersion) &&
      (L7_FILE_HDR_VER_CURRENT != cfgFileHdr.fileHdrVersion))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Version Check Failed\n");
    (void)osapiSemaGive (sysapiSupportCfgSema);
    return L7_ERROR;
  }

  /* It is possible that file size on disk is larger than expected file
  ** size. For CRC computation and file parsing we must use the expected
  ** file size.
  */
  if (total_length < cfgFileHdr.length || cfgFileHdr.length < file_header_size)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Support configuration file on disk size %u is less than expected size %u.\n",
             total_length, cfgFileHdr.length);
    (void)osapiSemaGive(sysapiSupportCfgSema);
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
      (void)osapiSemaGive (sysapiSupportCfgSema);
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
        (void)osapiSemaGive (sysapiSupportCfgSema);
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
      (void)osapiSemaGive (sysapiSupportCfgSema);
      return L7_ERROR;
    }

    (void)osapiFsClose (file_desc);
  }

  if (checksum != read_checksum)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "sysapiSupportCfgFileSeparate: CRC check failed. 0x%x read and 0x%x calculated",
            read_checksum, checksum);
    (void)osapiSemaGive(sysapiSupportCfgSema);
    return L7_ERROR;
  }


  /* Read the file header from the big config file.
  */
  rc = osapiFsOpen (cfgFile, &file_desc);
  if (rc != L7_SUCCESS)
  {
    (void)osapiSemaGive (sysapiSupportCfgSema);
    return L7_ERROR;
  }
  rc = osapiFileRead (file_desc, (L7_char8 *) &cfgFileHdr, file_header_size);
  if (rc != L7_SUCCESS)
  {
    (void)osapiSemaGive (sysapiSupportCfgSema);
    return L7_ERROR;
  }

  /* Make sure that file name is what we expect.
  */
  if (strcmp(cfgFileHdr.filename, SYSAPI_SUPPORT_CONFIG_FILENAME) != 0)
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
             "Unexpected file name:  %s\n",cfgFileHdr.filename);
    (void)osapiFsClose (file_desc);
    (void)osapiSemaGive(sysapiSupportCfgSema);
    return L7_ERROR;
  }

  /* Make sure that the configuration file is correct for the target device.
  */
  if (cfgFileHdr.targetDevice != hpc_unit->configLoadTargetId)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Unexpected target device:  %s\n",cfgFileHdr.targetDevice);
    (void)osapiFsClose (file_desc);
    (void)osapiSemaGive(sysapiSupportCfgSema);
    return L7_ERROR;
  }


  /*--------------------------*/
  /* read the big config file */
  /*--------------------------*/
  if (cfgFileHdr.version == SYSAPI_CFG_VER_2)
  {
    sysapiSupportCfgFileSeparateFilesV2(file_desc, total_length);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Unrecognized big support config file version %d\n", cfgFileHdr.fileHdrVersion);
    (void)osapiSemaGive(sysapiSupportCfgSema);
    return L7_ERROR;
  }


  /*---------------------*/
  /* give semaphore back */
  /*---------------------*/

  (void)osapiSemaGive(sysapiSupportCfgSema);


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
void sysapiSupportCfgFileInvalidate (void)
{
  L7_uint32 i;

    /* Set file type to something other than L7_CFG_DATA.
    ** This forces the osapiCfgFileGet() function to
    ** create default configuration.
    */
  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    pSysapiSupportCfgTree[i]->fileInfo.filetype = L7_INIT_PARMS;
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
L7_RC_t sysapiSupportCfgFileWrite(L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
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

  (void)osapiSemaTake(sysapiSupportCfgSema, L7_WAIT_FOREVER);


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
    (void)osapiSemaGive(sysapiSupportCfgSema);
    return L7_ERROR;
  }

  /* Make sure that component file name is not NULL.
  */
  if (cfgFileHdr->filename[0] == 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "File %s hdr name = %s is invalid\n", filename, (L7_char8 *)cfgFileHdr->filename);
    (void)osapiSemaGive(sysapiSupportCfgSema);
    return L7_ERROR;
  }

  rc = sysapiSupportCfgFileIndexGet(filename, &fileindex);
  if (rc == L7_ERROR)
  {
    (void)osapiSemaGive(sysapiSupportCfgSema);
    return rc;
  }


  if (sysapiSupportCfgTreeUpdate(component_id, filename, buffer, nbytes,
                                 pSysapiSupportCfgTree[fileindex])  != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Failed to update support cfg tree for component %d, %s and file %s\n",
            component_id, name, cfgFileHdr->filename);

  }

  (void)osapiSemaGive(sysapiSupportCfgSema);
  return L7_SUCCESS;
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
L7_RC_t sysapiSupportCfgFileSave(void)
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

  (void)osapiSemaTake(sysapiSupportCfgSema, L7_WAIT_FOREVER);

  /* Compute total configuration file size.
  */
  total_length = sizeof(L7_fileHdr_t);

  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    if (pSysapiSupportCfgTree[i]->fileInfo.file_location == L7_NULLPTR)
      continue;

    /* Each file location points to a configuration file overlay which contains
    ** an L7_fileHdr_t structure, the configuration data, and a crc
    */

    total_length += sizeof(sysapiCfgFileDescr_t);
    total_length += pSysapiSupportCfgTree[i]->cfgFileDescr.compressedFileSize;

  }


  /* Add in the size crc for the combined file
  */
  total_length = total_length + sizeof(L7_uint32);

  /*-----------------------------------------*/
  /* create the header for the combined file */
  /*-----------------------------------------*/

  memset((L7_char8 *)&cfgFileHdr, 0, sizeof(L7_fileHdr_t));

  cfgFileHdr.length = total_length;
  osapiStrncpySafe((char*)cfgFileHdr.filename, SYSAPI_SUPPORT_CONFIG_FILENAME, L7_MAX_FILENAME);
  cfgFileHdr.version = SYSAPI_CFG_VER_CURRENT;
  cfgFileHdr.type = L7_CFG_DATA;
  cfgFileHdr.targetDevice = hpc_unit->configLoadTargetId;

  /* Set the Header Version */
  cfgFileHdr.fileHdrVersion = L7_FILE_HDR_VER_CURRENT;

  /* Save the file header
  */
  rc = osapiFsFileCreate (SYSAPI_SUPPORT_CONFIG_FILENAME, &file_desc);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
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
    L7_LOG_ERROR (rc);
  }

  /*--------------------------------------------*/
  /* Iterate through the tree to copy all files */
  /*--------------------------------------------*/

  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    if (pSysapiSupportCfgTree[i]->fileInfo.file_location == L7_NULLPTR)
      continue;

    /* Always place the sysapiCfgFileDescr_t structure immediately before
       the cfg file in big combined cfg file */

    size_to_copy    =  sizeof(sysapiCfgFileDescr_t);
    source_location =  (L7_uchar8 *)&pSysapiSupportCfgTree[i]->cfgFileDescr;
    rc = osapiFsWriteNoClose (file_desc, source_location, size_to_copy);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR (rc);
    }

    octet = source_location;
    for (j = 0; j < size_to_copy; j++)
    {
      checksum = nvStoreCrcUpdate (checksum, *octet);
      octet++;
    }

    /* Each file location points to a configuration file overlay which contains
       an L7_fileHdr_t structure, the configuration data, and a crc. */

    size_to_copy    =  pSysapiSupportCfgTree[i]->cfgFileDescr.compressedFileSize;
    source_location =  pSysapiSupportCfgTree[i]->fileInfo.file_location;

    rc = osapiFsWriteNoClose (file_desc, source_location, size_to_copy);
    if (rc != L7_SUCCESS)
    {
      L7_LOG_ERROR (rc);
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
    L7_LOG_ERROR (rc);
  }

  (void)osapiFsClose (file_desc);

  /*---------------------*/
  /* give semaphore back */
  /*---------------------*/
  (void)osapiSemaGive(sysapiSupportCfgSema);

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
L7_RC_t sysapiSupportCfgFileGet(L7_COMPONENT_IDS_t component_id, L7_char8 *fileName,
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

  /* Obtain the index */

  rc = sysapiSupportCfgFileIndexGet(fileName, &fileIndex);
  if (rc != L7_SUCCESS)
  {
    fileSize = 0;
  }
  else
  {

    buildDefaultFile = L7_FALSE;
    pCfg = pSysapiSupportCfgTree[fileIndex]->fileInfo.file_location;

      /* Get the fileSize */
    fileSize = 0;
    if ((pCfg != L7_NULLPTR) &&
        (pSysapiSupportCfgTree[fileIndex]->cfgFileDescr.marker == SYSAPI_SUPPORT_CFG_DESCR_MARKER))
    {
      pFileHdr = (L7_fileHdr_t *)pCfg;

      fileSize = pFileHdr->length;
    }
  }

  /* If this is not a configuration file then it is not valid.
  */
  if ((fileSize != 0) &&
      (pSysapiSupportCfgTree[fileIndex]->fileInfo.filetype != L7_CFG_DATA))
    fileSize = 0;

  /* If file does not exist or file size does not equal to the buffer size
     available to store the file, migrate the file */
  if (fileSize < sizeof(L7_fileHdr_t) || (pCfg == L7_NULLPTR))
    buildDefaultFile = L7_TRUE;
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
          if (savedVersion == version)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                    "File %s: same version (%u) but the sizes (%u->%u) differ\n",
                     fileName, version, fileSize, bufferSize);
          }
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                  "Migrating support config file %s from version %u to %u\n", fileName, savedVersion, version);

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

          return(L7_SUCCESS);
        }
      }
      else
      {

          /*----------------------------------------------*/
          /* Uncompress the config data for the component */
          /*----------------------------------------------*/

        pCompressedData    = pSysapiSupportCfgTree[fileIndex]->fileInfo.file_location + sizeof(L7_fileHdr_t);
        compressedDataSize = pSysapiSupportCfgTree[fileIndex]->cfgFileDescr.compressedFileSize - sizeof(L7_fileHdr_t);

          /* Need to pass in the data size that is needed for full uncompression */
        uncompressedBufLen = fileSize;

        if (sysapiUncompress(pCompressedData, compressedDataSize,
                             (L7_char8 **)&pUncompressedData, &uncompressedBufLen) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                  "Failed to uncompress config data for component %d, %s\n", component_id, name);
          return L7_ERROR;
        }

          /*----------------------------*/
          /* Read the configuration file
          */
          /*----------------------------*/

        /* Copy the file header */
        memcpy(buffer, pSysapiSupportCfgTree[fileIndex]->fileInfo.file_location, sizeof(L7_fileHdr_t));

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
    /* do default build */
    if ((L7_uint32)defaultBuild == L7_NULL)
      return(L7_FAILURE);

    /*  No debug message is needed for the debug cfg file */

    /*

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
           "Building defaults for file %s version %u\n",fileName, version);

    */

    (*defaultBuild)(version);

    pFileHdr = (L7_fileHdr_t *)buffer;
    pFileHdr->fileHdrVersion = L7_FILE_HDR_VER_CURRENT;
    pFileHdr->componentID = component_id;
    *checkSum = nvStoreCrc32((L7_uchar8 *)buffer, bufferSize-sizeof(L7_uint32));

    if (sysapiSupportCfgFileWrite(component_id, fileName, buffer, bufferSize) == L7_ERROR)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Error on call to sysapiSupportCfgFileWrite routine on file %s\n", fileName);
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
L7_RC_t sysapiSupportCfgFileSizeGet(L7_COMPONENT_IDS_t component_id,
                                    L7_char8 *fileName, L7_uint32 *bufferSize)
{
  L7_uint32 fileIndex=0;
  L7_RC_t rc;

  rc = sysapiSupportCfgFileIndexGet(fileName, &fileIndex);
  if (rc != L7_SUCCESS)
  {
    *bufferSize = 0;
    return L7_FAILURE;
  }
  else
  {
    /* Get the fileSize */
    *bufferSize = pSysapiSupportCfgTree[fileIndex]->cfgFileDescr.uncompressedLength - sizeof(L7_fileHdr_t);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Find the index of a filename in pSysapiSupportCfgTree
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
L7_RC_t sysapiSupportCfgFileIndexGet(const L7_char8 *fileName, L7_uint32 *pFileIndex)
{
  L7_uint32 i;
  L7_uint32 first_empty_index = 0;
  L7_uint32 first_empty_index_is_set = 0;

  for (i = 0; i < SYSAPI_MAX_FILE_ENTRIES; i++)
  {
    if (pSysapiSupportCfgTree[i] == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "pSysapiSupportCfgTree[%u] is NULL", i);
      return L7_ERROR;
    }
    else
    {
      if (strcmp(fileName, pSysapiSupportCfgTree[i]->fileInfo.filename) == 0)
      {
        *pFileIndex = i;
        return L7_SUCCESS;
      }
      else
      {
        if ((first_empty_index_is_set == 0) &&
            (pSysapiSupportCfgTree[i]->fileInfo.filename[0] == 0))
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
static L7_RC_t sysapiSupportCfgFileSeparateFilesV2(L7_int32 file_desc, L7_int32 total_length)
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
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Failure to process cfg file descriptor: index %d\n", i);
      return L7_ERROR;
    }

    if (cfgFileDescr.marker != SYSAPI_SUPPORT_CFG_DESCR_MARKER)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Invalid cfgFileDescr Marker: %d\n", cfgFileDescr.marker);
      (void)osapiFsClose (file_desc);
      return L7_ERROR;
    }

    file_size = cfgFileDescr.compressedFileSize;

      /* Make sure that we don't have unexpected file format change.
      */
    if ((file_size == 0) || (file_size > bytes_left))
    {
      (void)osapiFsClose (file_desc);
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
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Failure to read config file header: index %d\n", i);
      return L7_ERROR;
    }

    {
      L7_uint32 fileindex = 0;
      L7_uchar8    *data_area = L7_NULL;

      rc = sysapiSupportCfgFileIndexGet (cfgFileHdr.filename, &fileindex);
      if (rc == L7_ERROR)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "Failure to cfg file index for file %s\n", cfgFileHdr.filename);
        (void)osapiFsClose (file_desc);
        return L7_ERROR;
      }

         /* Always free any allocation of a pre-existing data area */
      if (pSysapiSupportCfgTree[fileindex]->fileInfo.file_location != L7_NULLPTR)
      {
        osapiFree (L7_SIM_COMPONENT_ID, pSysapiSupportCfgTree[fileindex]->fileInfo.file_location);
        data_area = L7_NULL;
      }

      if (data_area == L7_NULL)
      {
        data_area = osapiMalloc (L7_SIM_COMPONENT_ID, file_size);
        if (data_area == L7_NULL)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                  "Failure to allocate memory for cfg for file %s\n", cfgFileHdr.filename);
          (void)osapiFsClose (file_desc);
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
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
                "Failure closing file %s\n", cfgFileHdr.filename);
        return L7_ERROR;
      }

         /*----------------------------------------------------------*/
         /*
              Update the pSysapiTree information
         */
         /*----------------------------------------------------------*/


         /* Update the file Info */
      pSysapiSupportCfgTree[fileindex]->fileInfo.file_location = data_area;
      osapiStrncpySafe(pSysapiSupportCfgTree[fileindex]->fileInfo.filename, cfgFileHdr.filename, L7_MAX_FILENAME);
      pSysapiSupportCfgTree[fileindex]->fileInfo.filetype = L7_CFG_DATA;
      pSysapiSupportCfgTree[fileindex]->fileInfo.filesize = file_size;

         /* Update the config file descriptor */
      memcpy( (void *)&pSysapiSupportCfgTree[fileindex]->cfgFileDescr,
              (void *)&cfgFileDescr, file_descr_size);

    }

    bytes_left -= file_size;

      /* Terminate the loop after we reach the end of file.
      */
    if (bytes_left < file_header_size)
    {
      break;
    }
  }

  (void)osapiFsClose (file_desc);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Compress the config data and update the sysapiSupportCfgTree
*
* @param    component_id   component ID of the file owner.
* @param    fileName       name of file to load
* @param    buffer         pointer to uncompressed config data
* @param    nbytes         number of bytes of uncompressed data
* @param    *pCfgTreeEntry pointer to pSysapiSupportCfgTree entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t sysapiSupportCfgTreeUpdate(L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
                                          L7_char8 *buffer, L7_uint32 nbytes,
                                          SYSAPI_DIRECTORY_CFG_t *pCfgTreeEntry)
{
  void          *dataArea;
  L7_uchar8     *pCompressedData;
  L7_int32      compressedBufLen;
  L7_int32      compressedFileSize;
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if ((rc = cnfgrApiComponentNameGet(component_id, name)) != L7_SUCCESS)
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
            "Failed to compress support config file for component %d, %s\n", component_id, name);
    return L7_ERROR;
  }

    /* ------------------------------------------------------------ */
    /* Update the pSysapiSupportCfgTree for this component's configuration
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
              "Failed to allocate %ld for support config file %s\n", compressedFileSize, filename);
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
  osapiStrncpySafe(pCfgTreeEntry->fileInfo.filename, filename, L7_MAX_FILENAME);
  pCfgTreeEntry->fileInfo.filetype = L7_CFG_DATA;
  pCfgTreeEntry->fileInfo.filesize = nbytes;


    /* Update the config file descriptor */
  memset( (L7_char8 *)&(pCfgTreeEntry->cfgFileDescr), 0, sizeof(sysapiCfgFileDescr_t));
  pCfgTreeEntry->cfgFileDescr.version            = SYSAPI_CFG_DESCR_VER_CURRENT;
  pCfgTreeEntry->cfgFileDescr.marker             = SYSAPI_SUPPORT_CFG_DESCR_MARKER;
  pCfgTreeEntry->cfgFileDescr.compressedFileSize = compressedFileSize;
  pCfgTreeEntry->cfgFileDescr.uncompressedLength = nbytes;

  return L7_SUCCESS;

}
