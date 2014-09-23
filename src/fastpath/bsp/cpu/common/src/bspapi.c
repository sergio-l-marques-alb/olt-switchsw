/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   bspapi.c
*
* @purpose    BSP API
*
* @component  bsp component
*
* @comments   none
*
* @create     05/02/2006
*
* @author     Brady Rutherford
*
* @end
*             
**********************************************************************/

#include <stdio.h>
#include <fcntl.h>
#ifdef _L7_OS_VXWORKS_
#include <ioLib.h>
#endif

#ifdef _L7_OS_LINUX_
#include <unistd.h>
#endif
#include <netinet/in.h>
#include <sys/stat.h>

#include "l7_common.h"
#include "platform_cpu.h"
#include "bspapi.h"
#include "bspcpu_api.h"
#include "dim.h"

extern void print_vpd_in_flash ();
extern L7_ushort16 file_crc_compute(L7_uchar8 *file_name, L7_uint32 file_size);

/*********************************************************************
* @purpose  Returns the IPL Model Tag for this unit
*
* @returns  IPL_MODEL_TAG
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 bspapiIplModelGet(void)
{
  return IPL_MODEL_TAG;
}

/*********************************************************************
* @purpose  Returns the Default Baud Rate for this unit
*
* @returns  L7_DEFAULT_BAUDRATE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 bspapiDefaultBaudRateGet(void)
{
  return L7_DEFAULT_BAUDRATE;
}

/*********************************************************************
* @purpose  Returns the Service Port Name
*
* @returns  L7_FAILURE, no service port
* @returns  L7_SUCCESS, name of service port
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_char8 *bspapiServicePortNameGet(void)
{
  return SERVICE_PORT_NAME;
}

/*********************************************************************
* @purpose  Returns the Service Port unit number
*
* @returns  L7_FAILURE, no service port
* @returns  L7_SUCCESS, name of service port
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 bspapiServicePortUnitGet(void)
{
  return SERVICE_PORT_UNIT;
}

/**************************************************************************
*
* @purpose  Retrieve a serial port setting
*
* @param    parm specifies which serial port parameter to retrieve
* @param    result ptr where to place parameter setting
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE if parm is not supported or recognized
*
* @end
*
*************************************************************************/
L7_RC_t bspapiSerialPortParmGet ( L7_uint32 parm, L7_uint32 *result )
{
  L7_RC_t rc = L7_SUCCESS;

  switch ( parm )
  {
    case SYSAPISERIALPORT_CHARSIZE :
      *result = 8;
      break;

    case SYSAPISERIALPORT_FLOWCONTROL :
      *result = ( L7_uint32 )(L7_DISABLE);
      break;

    case SYSAPISERIALPORT_PARITYTYPE :
      *result = ( L7_uint32 )(L7_PARITY_NONE);
      break;

    case SYSAPISERIALPORT_STOPBITS :
      *result = ( L7_uint32 )(L7_STOP_BIT_1);  
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/**************************************************************************
*
* @purpose  Enables the Hardware Watchdog
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @end
*
*************************************************************************/
L7_RC_t bspapiWatchdogEnable( void )
{
  SYS_WATCHDOG_ENABLE();

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Disables the Hardware Watchdog
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @end
*
*************************************************************************/
L7_RC_t bspapiWatchdogDisable( void )
{
  SYS_WATCHDOG_RESET();

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Services the Hardware Watchdog
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @end
*
*************************************************************************/
L7_RC_t bspapiWatchdogService( void )
{
  SYS_WATCHDOG_SERVICE();

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the Hardware Watchdog Service Interval
*
* @param    void
*
* @returns  Service interval in milliseconds
*
* @end
*
*************************************************************************/
int bspapiWatchdogInterval( void )
{
  return WATCHDOG_TIMER_INTERVAL;
}

/*********************************************************************
* @purpose  Registered support debug dump routine for general system issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
L7_RC_t bspapiDebugRegisteredSysInfoDump(void)
{
  printf("\r\n");
  printf("/*=====================================================================*/\n");
  printf("/*                  BSP  INFORMATION                                   */\n");
  printf("/*=====================================================================*/\n");


  printf("\r\n");
  printf("\n");

  print_vpd_in_flash();

  printf("\r\n");
  printf("\n");

  return L7_SUCCESS;
}

/***************************************************************************
*
* @purpose Returns the Board ID for the CPU.
*
* @param    none.
*
* @returns Board ID
*
* @end
*
***************************************************************************/
extern L7_uint32 cpu_card_id;
L7_uint32 bspapiCpuBoardIdGet(void)
{
  return cpu_card_id;
}

/***************************************************************************
*
* @purpose Determine if the MAC address can be assigned to this switch
*
* @param   macAddr - pointer to MAC address
*
* @returns L7_TRUE  - Mac address is valid
* @returns L7_FALSE - Mac address is invalid
*
* @end
*
***************************************************************************/
typedef struct invalidMac_s
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
} invalidMac_t;

static invalidMac_t invalidMacAddrTable [] = {
  {{0x00,0x00,0x00,0x00,0x00,0x00}},
};

L7_BOOL bspapiSwitchMacAddrValid(L7_char8 *macAddr)
{
  int ctr;

  /* Check for Multicast bit */
  if ((macAddr[0] & 0x01) == 0x01)
  {
    return L7_FALSE;
  }

  for (ctr = 0; ctr < sizeof(invalidMacAddrTable)/sizeof(invalidMac_t); ctr++)
  {
     if (memcmp(macAddr, invalidMacAddrTable[ctr].macAddr,
                L7_MAC_ADDR_LEN) == 0)
     {
       return L7_FALSE;
     }
  }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Determines if the file name is a valid image file
*           
* @param    fileName    @b{(input)}  File name to validate
* @param    imageId     @b{(input)}  Pointer to returned image ID.
*
* @returns  L7_TRUE   if filename is a valid image
* @returns  L7_FALSE  if filename is invalid 
*
* @end
*********************************************************************/
L7_BOOL bspapiImageFileNameValid(char *fileName, L7_uint32 *imageId)
{
  if (strcmp(DIM_IMAGE1_NAME, fileName) == 0)
  {
    *imageId = 0;
    return L7_TRUE;
  }
  if (strcmp(DIM_IMAGE2_NAME, fileName) == 0)
  {
    *imageId = 1;
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
 * @purpose  Retrieves the file name for the current active image 
 *           
 * @param    fileName    @b{(output)}  Location to copy the  
 *                                     file name for the active image
 *
 * @returns  L7_SUCCESS  on successful execution
 * @returns  L7_FAILURE  if no image is marked as active yet
 *
 * @notes    File name is copied.
 *
 * @end
 *********************************************************************/
L7_RC_t bspapiActiveImageFileNameGet(char *fileName)
{
  return dimActiveImageFileNameGet(fileName);
}

/*********************************************************************
 * @purpose  Retrieves the file name for the current backup image 
 *           
 * @param    fileName    @b{(output)}  Location to copy file name of the  
 *                                     backup image
 *
 * @returns  L7_SUCCESS  on successful execution
 * @returns  L7_FAILURE  if there is no backup image
 *
 * @notes    File name is copied.
 *
 * @end
 *********************************************************************/
L7_RC_t bspapiBackupImageFileNameGet(char *fileName)
{
  return dimBackupImageFileNameGet(fileName);
}

#ifdef DIM_BACKUP_IMAGE
/*********************************************************************
 * @purpose  Retrieves the name for the currently configured backup
 *           image even if the image is not present on the box
 *
 * @param    fileName    @b{(output)}  Location to copy name of the
 *                                     backup image
 *
 * @returns  L7_SUCCESS   on successful execution
 *
 * @notes    File name is copied.
 *
 * @end
 *********************************************************************/
static L7_RC_t backupFileNameGet(char *fileName)
{
  if (0 != dimActiveImageFileNameGet(fileName))
  {
    strcpy(fileName, DIM_IMAGE1_NAME);
    return L7_SUCCESS;
  }

  if (0 == strcmp(DIM_IMAGE2_NAME, fileName))
  {
    strcpy(fileName, DIM_IMAGE1_NAME);
  }
  else                  /* image2 must be backup */
  {
    strcpy(fileName, DIM_IMAGE2_NAME);
  }
  return L7_SUCCESS;
}
#endif

/*********************************************************************
 * @purpose  Determines if the image name passed is valid and maps it
 *           to the correct file name
 *
 * @param    imageName   @b{(input)}   Image name to validate/resolve
 * @param    fileName    @b{(output)}  Location to copy name of the
 *                                     file
 *
 * @returns  L7_TRUE      if valid image name is passed
 * @returns  L7_FALSE     if image name is invalid
 *
 * @notes    File name is copied.
 *
 * @end
 *********************************************************************/
L7_BOOL bspapiImageFileNameResolve(char *imageName, char *fileName)
{
  L7_uint32 imageId;

  strcpy(fileName, imageName);
  /* If the backup image keyword is the desitination, use */
  /* the current backup image as the destination file.    */
  if (strcmp(DIM_BACKUP_IMAGE, imageName) == 0)
  {
    if (backupFileNameGet(fileName) != L7_SUCCESS)
    {
      strcpy(fileName, DIM_IMAGE2_NAME);
    }
  }
  if (strcmp(DIM_ACTIVE_IMAGE, imageName) == 0)
  {
    if (dimActiveImageFileNameGet(fileName) != L7_SUCCESS)
    {
      strcpy(fileName, DIM_IMAGE1_NAME);
    }
  }

  /* The image names are fixed.
   * check if this name is one of the allowed names
   */
  return bspapiImageFileNameValid(fileName, &imageId);
}

/*********************************************************************
 * @purpose  Sets the supplied image as the active image for the 
 *           subsequent re-boots
 *           
 * @param    imageName       @b{(input)}  image to be activated
 * @param    updateBootCode  @b{(input)}  flag to command a boot code update
 *
 * @returns  L7_SUCCESS              on successful execution
 * @returns  L7_NOT_EXIST            if the specified image could not be found
 * @return   L7_ALREADY_CONFIGURED   if the backup image is already active
 * @return   L7_DEPENDENCY_NOT_MET   if the bootrom cannot boot image
 * @return   L7_FAILURE              on internal error
 *
 * @notes    Internal data structures are updated with the information.
 *           Image currently active is marked as backup and the specified
 *           image is marked as active.
 *
 * @end
 *********************************************************************/
L7_RC_t bspapiImageActivate(char *imageName, L7_BOOL updateBootCode)
{
  L7_char8     fileName[DIM_MAX_FILENAME_SIZE];
  oprHeader_t  oprHeader;
  L7_RC_t      retCode;

  if (bspapiImageFileNameResolve(imageName, fileName) != L7_TRUE)
  {
    return L7_FAILURE;
  }
  /* Verify that the image name is valid an the image exists.
     Otherwise return error */
  retCode = bspapiOprHeaderRead(fileName, &oprHeader);
  if (retCode != L7_SUCCESS)
  {
    return retCode;
  }

#ifdef BOOTCODE_MIN_VERSION
  if (updateBootCode != L7_TRUE)
  {
    L7_uint32    bootCodeVersion = 0;
    if ((bspCpuBootVersionGet(&bootCodeVersion) != L7_SUCCESS) ||
       (oprHeader.minBootVersion > bootCodeVersion))
    {
#ifdef BOOTCODE_VERSION
      retCode = bspapiBootCodeUpdate(fileName);
      if (retCode != L7_SUCCESS)
      {
        return retCode;
      }
#else
      return L7_DEPENDENCY_NOT_MET;
#endif
    }
  }
#endif

  retCode =  dimImageActivate(fileName);

#if L7_FEAT_BOOTCODE_UPDATE
  if (updateBootCode == L7_TRUE)
  {
    retCode = bspapiBootCodeUpdate(fileName);
  }
#endif

  return retCode;
}

/*********************************************************************
 * @purpose  Adds an image to the list of images 
 *           
 * @param    srcFileName   @b{(input)}  image file to add
 * @param    dstImageName  @b{(input)}  destination image name
 *
 * @returns  L7_SUCCESS   on successful execution
 * @returns  L7_FAILURE   if image cannot be added
 * 
 * @end
 *********************************************************************/
L7_RC_t bspapiImageAdd(char *srcFileName, char *dstImageName)
{
  L7_char8 fileName[DIM_MAX_FILENAME_SIZE];

  if (bspapiImageFileNameResolve(dstImageName, fileName) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  return dimImageAdd(srcFileName, fileName);
}

/*********************************************************************
 * @purpose  Deletes an image from the list of boot images 
 *           
 * @param    imageName    @b{(input)}  name of the image to be deleted
 *
 * @returns  L7_SUCCESS        on successful execution
 * @returns  L7_NOT_EXIST      if the image could not be found.
 * @returns  L7_REQUEST_DENIED if the specified image is active/activated.
 *
 * @notes    current active image is never deleted.
 *
 * @end
 *********************************************************************/
L7_RC_t bspapiImageDelete(char *imageName)
{
  L7_char8 fileName[DIM_MAX_FILENAME_SIZE];

  if (bspapiImageFileNameResolve(imageName, fileName) != L7_TRUE)
  {
    return L7_FAILURE;
  }
  return dimImageDelete(fileName);
}

/*********************************************************************
* @purpose  Checks if the backup image is activated 
*           
* @param    none
*
* @returns  L7_TRUE  if backup image is valid & activated
*           L7_FALSE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL bspapiIsBackupImageActivated(void)
{
  return dimIsBackupImageActivated();
}

/*********************************************************************
 * @purpose  Indicates if the image exists in the file system.
 *
 * @param    name    @b{(input)}  Name of the file containing the image
 *
 * @returns  L7_TRUE  when the image exists
 * @returns  L7_FALSE when the image does not exist
 *
 * @notes
 * @end
 *********************************************************************/
L7_BOOL bspapiImageExists(char *name)
{
  int      filedesc;
  L7_char8 fileName[DIM_MAX_FILENAME_SIZE];
  if (bspapiImageFileNameResolve(name, fileName) != L7_TRUE)
  {
    return L7_FALSE;
  }

  /* Verify the file name is a valid image file name */
  if ((strcmp(fileName, DIM_IMAGE1_NAME) != 0) &&
      (strcmp(fileName, DIM_IMAGE2_NAME) != 0))
  {
    return L7_FALSE;
  }

  filedesc = open(fileName, O_RDONLY, 0);
  if (-1 == filedesc)
  {
    return L7_FALSE;
  }

  close(filedesc);
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Returns the image file name for the given ID.
*
* @param    imageId     @b{(input)}  image ID to retrieve file name
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @end
*********************************************************************/
L7_RC_t bspapiImageFileNameGet(L7_uint32 imageId, L7_char8 *fileName)
{
  L7_RC_t rc = L7_FAILURE;
  
  switch (imageId)
  {
    case 0:
      strcpy(fileName, DIM_IMAGE1_NAME);
      rc = L7_SUCCESS;
      break;
    case 1:
      strcpy(fileName, DIM_IMAGE2_NAME);
      rc = L7_SUCCESS;
      break;
    default:
      break;
  }
  return rc;
}

/*********************************************************************
* @purpose  Associates a given text description for an image
*           
* @param    image         @b{(input)}  image name
* @param    descr         @b{(input)}  text to be associated
* 
* @returns  L7_SUCCESS   on successful execution
* @returns  L7_NOT_EXIST if the specified image could not be found
*
* @end
*********************************************************************/
L7_RC_t bspapiImageDescrSet(char *image, char *descr)
{
  L7_char8 fileName[DIM_MAX_FILENAME_SIZE];

  if (bspapiImageFileNameResolve(image, fileName) != L7_TRUE)
  {
    return L7_FAILURE;
  }
  return dimImageDescrSet(fileName, descr);
}

/*********************************************************************
* @purpose  retrieves the text description for an image
*           
* @param    image         @b{(input)}  image name
* @param    descr         @b{(input)}  pointer to copy the text to
* 
* @returns  L7_SUCCESS   on successful execution
* @returns  L7_NOT_EXIST if the specified image could not be found
*
* @end
*********************************************************************/
L7_RC_t bspapiImageDescrGet(char *image, char *descr)
{
  L7_char8 fileName[DIM_MAX_FILENAME_SIZE];

  if (bspapiImageFileNameResolve(image, fileName) != L7_TRUE)
  {
    return L7_FAILURE;
  }
  return dimImageDescrGet(fileName, descr);
}

/*********************************************************************
 * @purpose  Returns the name for the backup image
 *           
 * @param    imageName    @b{(input)}  buffer to copy the image name
 *
 * @returns  none.
 * 
 * @end
 *********************************************************************/
L7_RC_t bspapiBackupImageNameGet(char *imageName)
{
#ifdef DIM_BACKUP_IMAGE
  strcpy(imageName, DIM_BACKUP_IMAGE);
  return L7_SUCCESS;
#else
  return L7_FAILURE;
#endif
}

/*********************************************************************
 * @purpose  Returns the name for the active image
 *           
 * @param    imageName    @b{(input)}  buffer to copy the image name
 *
 * @returns  none.
 * 
 * @end
 *********************************************************************/
L7_RC_t bspapiActiveImageNameGet(char *imageName)
{
#ifdef DIM_ACTIVE_IMAGE
  strcpy(imageName, DIM_ACTIVE_IMAGE);
  return L7_SUCCESS;
#else
  return L7_FAILURE;
#endif
}

/*********************************************************************
* @purpose  Determines if the image name is a valid
*           
* @param    imageName   @b{(input)}  Image name to validate
*
* @returns  L7_TRUE   if image name is valid
* @returns  L7_FALSE  if image name is not supported
*
* @end
*********************************************************************/
L7_BOOL bspapiImageNameValid(char *imageName)
{
#ifdef DIM_BACKUP_IMAGE
  if (strcmp(imageName, DIM_BACKUP_IMAGE) == 0)
    return L7_TRUE;
#endif

#ifdef DIM_ACTIVE_IMAGE
  if (strcmp(imageName, DIM_ACTIVE_IMAGE) == 0)
    return L7_TRUE;
#endif

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Persistently save the next active SDM template ID.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  L7_SUCCESS
*           L7_ERROR if file save fails
*
* @notes    Initial implementation saves as a file on the flash file 
*           system. Could be overridden on a platform to save somewhere
*           else.
*
* @end
*********************************************************************/
L7_RC_t bspapiNextSdmTemplateSave(L7_uint32 templateId)
{
  bspSdmTemplateData_t sdmTemplateData;
  L7_uint32 nbytes = sizeof(sdmTemplateData);

  sdmTemplateData.version = SDM_TEMPLATE_FILE_VERSION;
  sdmTemplateData.sdm_template_id = templateId;
  sdmTemplateData.pad = 0;
  sdmTemplateData.crc = bspapiCalcCrc((L7_uint32) &sdmTemplateData, 
                                      (L7_uint32)(sizeof (sdmTemplateData) - 
                                                  sizeof (sdmTemplateData.crc)));

  return bspCpuFsWrite(SDM_TEMPLATE_FILENAME, (L7_char8*) &sdmTemplateData, nbytes);
}

/*********************************************************************
* @purpose  Retrieve the next active SDM template ID from persistent storage.
*
* @param    nextTemplateId @b{(output)} Template ID read from persistent storage
*
* @returns  L7_SUCCESS
*           L7_NOT_EXIST if file not read
*           L7_ERROR if file corrupted
*
* @notes    Initial implementation reads from a file on the flash file 
*           system. Could be overridden on a platform to retrieve from 
*           somewhere else.
*
* @end
*********************************************************************/
L7_RC_t bspapiNextSdmTemplateGet(L7_uint32 *nextTemplateId)
{
  bspSdmTemplateData_t sdmTemplateData;
  L7_uint32 nbytes = sizeof(sdmTemplateData);

  if (bspCpuFsRead(SDM_TEMPLATE_FILENAME, (L7_char8*) &sdmTemplateData, nbytes) != L7_SUCCESS)
  {
    return L7_NOT_EXIST;
  }

  /* Verify CRC */
  if (sdmTemplateData.crc != bspapiCalcCrc((L7_uint32) &sdmTemplateData, 
                                           (L7_uint32)(sizeof (sdmTemplateData) - 
                                                       sizeof (sdmTemplateData.crc))))
  {
    return L7_ERROR;
  }
  /* Only one version of this file has been defined. So if file version doesn't 
   * match, declare an error. If future releases change file format and update version
   * number, logic may need to change. */
  if (sdmTemplateData.version != SDM_TEMPLATE_FILE_VERSION)
  {
    return L7_ERROR;
  }
  *nextTemplateId = sdmTemplateData.sdm_template_id;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove the next active SDM template ID from persistent storage.
*
* @param    void
*
* @returns  L7_SUCCESS
*           L7_ERROR if file save fails
*
* @notes    Default implementation deletes SDM template file.
*
* @end
*********************************************************************/
L7_RC_t bspapiNextSdmTemplateClear(void)
{
  return bspCpuFsDeleteFile(SDM_TEMPLATE_FILENAME);
}

/*********************************************************************
* @purpose  Write a file to the flash file system.
*
* @param    filename @b{(input)}  name of the file to be written
* @param    data     @b{(input)}  pointer to start of data to be written to file
* @param    length   @b{(input)}  number of bytes to be written
*
* @returns  L7_SUCCESS
*           L7_ERROR if file save fails
*
* @notes    If file does not already exist, it is created.
*
*           Function is intended to be called from within the BSP and 
*           avoids the need to include osapi to write to the FS. 
*
* @end
*********************************************************************/
L7_RC_t bspCpuFsWrite(L7_uchar8 *filename, L7_char8* data, L7_uint32 length)
{
  L7_int32 filedesc;
  L7_int32 rc;

  if ((filedesc = open(filename, O_RDWR | O_CREAT, 0)) == -1)
  {
    return L7_ERROR;
  }

  rc = write(filedesc, data, length);
  if ((rc == -1) || (rc != length))
  {
    return L7_ERROR;
  }

  close(filedesc);

  return L7_SUCCESS;
}

/**************************************************************************
 *
 * @purpose  Read the contents of a file
 *
 * @param    filename  @b{(input)}  File from which to read data
 * @param    buffer    @b{(input)}  where to put data
 * @param    nbytes    @b{(input)}  amount of data to read
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments    This function opens the file, reads data, and closes the file
 *
 *              Function is intended to be called from within the BSP and 
 *              avoids the need to include osapi to write to the FS. 
 *
 * @end
 *
 *************************************************************************/
L7_RC_t bspCpuFsRead(L7_char8 *filename, L7_char8 *buffer, L7_int32 nbytes)
{
  L7_int32 filedesc, rc;

  filedesc = open(filename, O_RDWR, 0);
  if (filedesc < 0)
  {
    return L7_ERROR;
  }

  rc = read(filedesc, buffer, nbytes);
  if ((rc < 0) || (rc != nbytes))
  {
    return L7_ERROR;
  }

  close(filedesc);
  return L7_SUCCESS;
}

/**************************************************************************
 *
 * @purpose  Delete a file from the flash file system.
 *
 * @param    filename  @b{(input)}  Name of file to delete
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments    Function is intended to be called from within the BSP and 
 *              avoids the need to include osapi to write to the FS. 
 *
 * @end
 *
 *************************************************************************/
L7_RC_t bspCpuFsDeleteFile(L7_char8 *filename)
{
  L7_int32 rc = remove(filename);
  if (rc == -1)
  {
    return L7_ERROR;
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  retrieves the code version of the image
 *
 * @param    image         @b{(input)}  image name
 * @param    version       @b{(output)}  pointer to the code version
 *
 * @returns  L7_SUCCESS   on successful execution
 * @returns  L7_FAILURE   if unable to read version
 *
 * @end
 *********************************************************************/
L7_RC_t bspapiImageVersionGet(L7_char8* filename, codeVersion_t* version)
{
  struct ipl_image_header image_header;
  stkFileHeader_t *stkHdrPtr = L7_NULL;
  L7_int32 rc;
  L7_int32 fd;
  
  if (filename == L7_NULL || version == L7_NULL)
  {
    return L7_FAILURE;
  }
  fd = open(filename,O_RDONLY,0);
  if (fd < 0)
  {
    return L7_FAILURE;
  }
  stkHdrPtr = (stkFileHeader_t *)&image_header;
  rc  = read(fd, (L7_uchar8*)&image_header, sizeof(struct ipl_image_header));
  if ((stkHdrPtr->tag1 == STK_TAG1) && (stkHdrPtr->tag2 == STK_TAG2))
  {
    /* This is an .stk file, pull version from .stk header */
    version->rel = stkHdrPtr->rel;
    version->ver = stkHdrPtr->ver;
    version->maint_level = stkHdrPtr->maint_level;
    version->build_num   = stkHdrPtr->build_num;
  }
  else
  {
    /* Otherwise assume a .opr file, pull version from .vpd header */
    version->rel = image_header.vpd.rel;
    version->ver = image_header.vpd.ver;
    version->maint_level = image_header.vpd.maint_level;
    version->build_num   = image_header.vpd.build_num;
  }
  version->valid = L7_TRUE;
  close(fd);
  return L7_SUCCESS;
}

/**************************************************************************
 *
 * @purpose  Reads the header from STK files and validate it
 *
 * @param    file_name  Operational code file on RAM disk.
 *
 * @returns  none.
 *
 * @comments  This function understands STK files.
 *            STK files have the specified 'tags' in the header.
 *            This function validates the given image.
 *
 * @end
 *
 *************************************************************************/
STK_RC_t bspapiValidateImage(L7_uchar8 *fileName, L7_BOOL validate_crc)
{
  stkFileHeader_t stkHeader;
  stkOprFileInfo_t *oprHead = NULL;
  char buffer[STK_MAX_HEADER_SIZE];
  int filedesc;
  int i;
  int offset;
  L7_ushort16 computed_crc;
  L7_uint32 fileSize, fileSizeOnDisk;
  struct stat sbuf;

  filedesc = open (fileName, O_RDONLY, 0);
  if ( filedesc < 0 )
  {
    return STK_IMAGE_DOESNOT_EXIST;
  }

  /* read first few bytes to determine STK or OPR */

  i = read(filedesc, (void *)&stkHeader, sizeof(stkHeader));

  if (i < sizeof(stkHeader))
  {
    close(filedesc);
    return STK_INVALID_IMAGE;
  }
  /* check if the file is an STK file */

  offset = 0;

  if((ntohs(stkHeader.tag1) == STK_TAG1) &&
     (ntohl(stkHeader.tag2) == STK_TAG2))
  {
    /* First, validate CRC */
    if (fstat (filedesc, &sbuf) < 0)
    {
      close(filedesc);
      return STK_FILE_SIZE_FAILURE;
    }
    fileSizeOnDisk = sbuf.st_size;
    
    fileSize = ntohl(stkHeader.file_size);
   
    if (fileSizeOnDisk < fileSize)
    {
      close(filedesc);
      return STK_FILE_SIZE_MISMATCH;
    }
  if (fileSizeOnDisk > fileSize)
    {
      if ((fileSizeOnDisk - fileSize) <= 1023)
      {
    /* Xmodem download has probably padded this file to an integer
       number of blocks. Remove padding. If we're wrong then CRC check
       will fail. */

#ifdef _L7_OS_LINUX_
    truncate(fileName, fileSize);
#elif _L7_OS_VXWORKS_
    ftruncate(filedesc, fileSize);
#endif   
      }
    }

    /* Sanity-check sizes */
    if ((ntohl(stkHeader.num_components) > STK_MAX_IMAGES) ||
    (ntohl(stkHeader.stk_header_size) > STK_MAX_HEADER_SIZE))
    {
      close(filedesc);
      return STK_TOO_MANY_IMAGES_IN_STK;
    }
    /* rewind to the begining */

    lseek(filedesc, 0, SEEK_SET);

    if(0 > read(filedesc, &buffer[0], ntohl(stkHeader.stk_header_size))){}

    /* check all available opr files to get the correct
     * opr file for this target
     */

    if(ntohl(stkHeader.num_components) == 0)
    {
      close(filedesc);
      return STK_STK_EMPTY;
    }

    for(i = 0; i < ntohl(stkHeader.num_components); i++)
    {
      offset = (sizeof(stkHeader) + (i * sizeof (stkOprFileInfo_t)));
      oprHead = (stkOprFileInfo_t *)&buffer[offset];

      if((ntohl(oprHead->target_device)) == bspapiIplModelGet())
      {
#ifdef _L7_OS_LINUX_
        if(ntohl(oprHead->os) == STK_OS_LINUX)
#elif _L7_OS_VXWORKS_
        if(ntohl(oprHead->os) == STK_OS_VXWORKS)
#endif        
        {
          offset = oprHead->offset;
          break;
        }
      }
    }

    if(i == ntohl(stkHeader.num_components))
    {
      close(filedesc);
      return STK_PLATFORM_MISMATCH;
    }
  }
  else /* STK tags not present */
  {
    close(filedesc);
    return STK_INVALID_IMAGE_FORMAT;
  }
  close(filedesc);
  
  /*Doing at the end as crc check is time consuming*/
  if (validate_crc == L7_TRUE)
  {
    computed_crc = file_crc_compute(fileName, fileSize);
    if (computed_crc != ntohs(stkHeader.crc))
    {
      close(filedesc);
      return STK_INVALID_IMAGE;
    }
  }
  return STK_SUCCESS;
}



