/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2010
 *
 **********************************************************************
 **********************************************************************
 *
 * @filename     bspcpu_dim.c
 *
 * @purpose      Linux specific Dual Image Manager functions
 *
 * @component    bsp
 *
 * @comments     none
 *
 * @create       01/25/2010
 *
 * @author       bradyr
 * @end
 *
 **********************************************************************/

/* ===================== Include Headers ===================== */

#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "l7_common.h"
#include "cli_web_exports.h"
#include "bspapi.h"
#include "osapi.h"
#include "osapi_file.h"
#include "platform_cpu.h"
#include "stk.h"
#include "bspcpu_api.h"

#ifndef DIM_CFG_FILENAME

#define CFG_FILE_NAME_LEN              (sizeof(DIM_IMAGE1_NAME) - 1)

static char activeImageFileName[DIM_MAX_FILENAME_SIZE];
static char backupImageFileName[DIM_MAX_FILENAME_SIZE];
static L7_BOOL activeImageValid = L7_FALSE;
static L7_BOOL backupImageValid = L7_FALSE;
static L7_BOOL backupImageActivated = L7_FALSE;

#ifdef BOOTENV_SUPPORT
#define ACTIVE_CFG_FILE                "/usr/local/ptin/sbin/active.cfg"
/*********************************************************************
 * @purpose  Read the active image file name from the environment variables
 *
 * @returns  L7_SUCCESS  valid image file name detected.
 * @returns  L7_FAILURE  unable to determine active image file name
 *
 * @end
 *********************************************************************/
static L7_RC_t activeEnvRead(void)
{
  char buf[50];
  char tmpFileName[CFG_FILE_NAME_LEN+1];
  int  fd, rd;

  /* U-boot specific environment variable setting */
  sprintf(buf, "bootenv -d active >" ACTIVE_CFG_FILE);
  if (WEXITSTATUS(system(buf)) != 0)
    return(L7_FAILURE);

  /* open the boot.dim file from the flash */
  fd = open(ACTIVE_CFG_FILE, O_RDONLY, 0);

  if ( fd == -1)
  {
    printf(" Unable to open active configuration file\n");
    return L7_FAILURE;
  }

  /* read the file into the buffer */  
  rd = read(fd, tmpFileName, CFG_FILE_NAME_LEN);
  close(fd);
  if (rd == -1)
  {
    printf(" Unable to read active configuration file\n");
    return L7_FAILURE;
  }
  tmpFileName[rd] = '\0';
  if ((strncmp(tmpFileName, DIM_IMAGE1_NAME, DIM_MAX_FILENAME_SIZE) != 0) &&
      (strncmp(tmpFileName, DIM_IMAGE2_NAME, DIM_MAX_FILENAME_SIZE) != 0))
  {
    printf(" Invalid image name - %s\n", tmpFileName);
    return L7_FAILURE;
  }
  snprintf(activeImageFileName, sizeof(activeImageFileName), "%s", tmpFileName);
  return L7_SUCCESS;
}
#else
#define ACTIVE_IMG_FILE                "dim.active"
/*********************************************************************
 * @purpose  Read the active image file name from the environment variables
 *
 * @returns  L7_SUCCESS  valid image file name detected.
 * @returns  L7_FAILURE  unable to determine active image file name
 *
 * @end
 *********************************************************************/
static L7_RC_t activeEnvRead(void)
{
  char tmpFileName[DIM_MAX_FILENAME_SIZE+1];
  memset(tmpFileName, 0, sizeof(tmpFileName));

  if (osapiFsRead(ACTIVE_IMG_FILE, tmpFileName,
                  DIM_MAX_FILENAME_SIZE) == L7_ERROR)
  {
    strcpy(tmpFileName, DIM_IMAGE1_NAME);
    osapiFsWrite(ACTIVE_IMG_FILE, DIM_IMAGE1_NAME, sizeof(DIM_IMAGE1_NAME));
  }
  else
  {
    if ((strncmp(tmpFileName, DIM_IMAGE1_NAME, DIM_MAX_FILENAME_SIZE) != 0) &&
        (strncmp(tmpFileName, DIM_IMAGE2_NAME, DIM_MAX_FILENAME_SIZE) != 0))
    {
      printf("Invalid image name - %s\n", tmpFileName);
      strcpy(tmpFileName, DIM_IMAGE1_NAME);
    }
  }
  snprintf(activeImageFileName, sizeof(activeImageFileName), "%s", tmpFileName);
  return L7_SUCCESS;
}
#endif

/*********************************************************************
 * @purpose  Sets the supplied image as the active image for the 
 *           subsequent re-boots
 *           
 * @param    fileName    @b{(input)}  image to be activated
 *
 * @returns  L7_SUCCESS              on successful execution
 * @returns  L7_NOT_EXIST            if the specified image could not be found
 * @return   L7_ALREADY_CONFIGURED   if the backup image is already active
 * @return   L7_FAILURE              on internal error
 *
 * @end
 *********************************************************************/
L7_RC_t dimImageActivate(char *fileName)
{
#ifdef _L7_OS_LINUX_
  char buf[100];
  int rc;
  struct stat st;
#endif
  char newbackupImageFileName[DIM_MAX_FILENAME_SIZE];
  L7_BOOL newBackupStatus;

  /* The active image is still active */
  if ((strncmp(fileName, activeImageFileName, DIM_MAX_FILENAME_SIZE) == 0) &&
     (backupImageActivated == L7_FALSE))
  {
    return L7_ALREADY_CONFIGURED;
  }

  /* The backup image has already been activated */
  if ((strncmp(fileName, backupImageFileName, DIM_MAX_FILENAME_SIZE) == 0) &&
     (backupImageActivated == L7_TRUE))
  {
    return L7_ALREADY_CONFIGURED;
  }

  /* Activate the current backup image */
  if (strncmp(fileName, backupImageFileName, DIM_MAX_FILENAME_SIZE) == 0)
  {
    snprintf(newbackupImageFileName, sizeof(newbackupImageFileName),
             "%s", activeImageFileName);
    newBackupStatus = L7_TRUE;
  }
  else /* Reactivate the original active image */
  {
    snprintf(newbackupImageFileName, sizeof(newbackupImageFileName),
             "%s", backupImageFileName);
    newBackupStatus = L7_FALSE;
  }

#ifdef BOOTENV_SUPPORT
  /* U-boot specific environment variable setting */
  snprintf(buf, sizeof(buf), "bootenv -s backup %s", newbackupImageFileName);
  if (WEXITSTATUS(system(buf)) != 0)
    return(L7_FAILURE);

  /* U-boot specific environment variable setting */
  snprintf(buf, sizeof(buf), "bootenv -s active %s", fileName);
  if (WEXITSTATUS(system(buf)) != 0)
    return(L7_FAILURE);
#else
  if (osapiFsWrite(ACTIVE_IMG_FILE, fileName, strlen(fileName)) == L7_ERROR)
  {
    return(L7_FAILURE);
  }
#endif

#ifdef _L7_OS_LINUX_
  /* A script is included in the .stk file to handle any necessary */
  /* hardware specific requirements to activate the new image.     */
  if (stat("/usr/sbin/extimage", &st) == 0)
  {
    snprintf(buf, sizeof(buf), "cd %s ; extimage -i %s -o %sACTIVATE -n 2",
        CONFIG_PATH, fileName, DOWNLOAD_PATH);
    if (WEXITSTATUS(system(buf)) != 0)
      return(L7_FAILURE);

    snprintf(buf, sizeof(buf), "cd %s ; sh ACTIVATE -q %s", DOWNLOAD_PATH,
        fileName);
    rc = WEXITSTATUS(system(buf));
    if ((rc != 0) && (rc != 1))
      return(L7_FAILURE);
  }
#endif
  backupImageActivated = newBackupStatus;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Deletes an image from the list of boot images 
 *           
 * @param    fileName    @b{(input)}  file name of the image to be deleted
 *
 * @returns  L7_SUCCESS        on successful execution
 * @returns  L7_NOT_EXIST      if the image could not be found.
 * @returns  L7_REQUEST_DENIED if the specified image is active/activated.
 *
 * @notes    current active image is never deleted.
 *
 * @end
 *********************************************************************/
L7_RC_t dimImageDelete(char *fileName)
{
  char buf[100];

  if (bspapiImageExists(fileName) == L7_FALSE)
  {
    return L7_NOT_EXIST;
  }

  /* if this is the active image, return error */
  if ((strncmp(activeImageFileName, fileName, DIM_MAX_FILENAME_SIZE) == 0x0) ||
    (backupImageActivated == L7_TRUE))
  {
    return L7_REQUEST_DENIED;
  }

  /* Only delete a back up image */
  if (backupImageValid == L7_TRUE)
  {
    /* delete the internal details */
    backupImageValid = L7_FALSE;
    memset(backupImageFileName, 0, sizeof(backupImageFileName));
    remove(fileName);
    snprintf(buf, sizeof(buf), "%s.dsc", fileName);
    remove(buf);
    
#ifdef BOOTENV_SUPPORT
    /* U-boot specific environment variable setting */
    snprintf(buf, sizeof(buf), "bootenv -u backup");
    if (WEXITSTATUS(system(buf)) != 0)
      printf("%s: error updating backup environment variable\n", __FUNCTION__);
#endif
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Initializes the dual boot image manager
 *
 * @param    none
 *
 * @returns  L7_SUCCESS  on successful initialization
 * @returns  L7_FAILURE  if file doesn't exist or no image can be found
 *
 * @notes    Reads the active environment variable and updates internals.
 *
 * @end
 *********************************************************************/
int dimInitialize(void)
{
  L7_RC_t retCode = L7_FAILURE;
 
  memset(activeImageFileName, 0, sizeof(activeImageFileName));
  memset(backupImageFileName, 0, sizeof(backupImageFileName));
  backupImageActivated = L7_FALSE;
  activeImageValid = L7_FALSE;
  backupImageValid = L7_FALSE;

  /* Initialize active image from persistant storage. */
  if (activeEnvRead() == L7_SUCCESS)
  {
    retCode = L7_SUCCESS;
    if (bspapiImageExists(activeImageFileName) == L7_TRUE)
    {
      activeImageValid = L7_TRUE;
    }

    if (strncmp(activeImageFileName, DIM_IMAGE1_NAME,
                DIM_MAX_FILENAME_SIZE) == 0)
    {
      if (bspapiImageExists(DIM_IMAGE2_NAME) == L7_TRUE)
      {
        strcpy(backupImageFileName, DIM_IMAGE2_NAME);
      }
    }
    else
    {
      if (bspapiImageExists(DIM_IMAGE1_NAME) == L7_TRUE)
      {
        strcpy(backupImageFileName, DIM_IMAGE1_NAME);
      }
    }
    /*
      By this time backup image name is resolved.
      Validate backup image.Validation of active image is not required. 
    */
    retCode = bspapiValidateImage(backupImageFileName,L7_TRUE);
    if (retCode != STK_SUCCESS)
    {
      printf("\nValidating the backup image %s failed rc = %d",
             backupImageFileName,retCode);
      dimImageDelete(backupImageFileName);
     
    }
    else
    {
      backupImageValid = L7_TRUE;
    }
    /* Has active image has been deleted & a valid backup exist */
    if ((activeImageValid == L7_FALSE) && (backupImageValid == L7_TRUE))
    {
      dimImageActivate(backupImageFileName);
    }
  }
  return retCode;
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
L7_RC_t dimActiveImageFileNameGet(char *fileName)
{
  L7_RC_t retCode = L7_FAILURE;

  if (activeImageValid == L7_TRUE)
  {
    strncpy(fileName, activeImageFileName, DIM_MAX_FILENAME_SIZE);
    retCode = L7_SUCCESS;
  }
  return retCode;
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
L7_RC_t dimBackupImageFileNameGet(char *fileName)
{
  L7_RC_t retCode = L7_FAILURE;

  if (backupImageValid == L7_TRUE)
  {
    strncpy(fileName, backupImageFileName, DIM_MAX_FILENAME_SIZE);
    retCode = L7_SUCCESS;
  }
  return retCode;
}

/*********************************************************************
 * @purpose  Adds an image to the list of images 
 *           
 * @param    srcFileName  @b{(input)}  source file name of image to add
 * @param    dstFileName  @b{(input)}  destination file name of new image
 *
 * @returns  L7_SUCCESS   on successful execution
 * @returns  L7_FAILURE   if unable to add the image
 * 
 * @end
 *********************************************************************/
L7_RC_t dimImageAdd(char *srcFileName, char *dstFileName)
{
#ifdef BOOTENV_SUPPORT
  char buf[100];
#endif

  /* If srcFileName == dstFileName, assume the file is already present & */
  /* valid on the file system, otherwise copy to the dstFileName.        */
  if (strncmp(srcFileName, dstFileName, DIM_MAX_FILENAME_SIZE) != 0)
  {
    remove(dstFileName);

    /* copy the file. */
    if (osapiFsCopyFile(srcFileName,dstFileName) != L7_SUCCESS)
    {
      return (L7_FAILURE);
    }
  }

  /* If the dstFileName is the backup image & there was no previous */
  /* backup, then update internal data indicating that there is     */
  /* now a valid backup image present.                              */
  if ((backupImageValid == L7_FALSE) &&
      (strncmp(dstFileName, activeImageFileName, DIM_MAX_FILENAME_SIZE) != 0))
  {
    backupImageValid = L7_TRUE;
    strncpy(backupImageFileName, dstFileName, DIM_MAX_FILENAME_SIZE-1);

#ifdef BOOTENV_SUPPORT
    /* U-boot specific environment variable setting */
    snprintf(buf, sizeof(buf), "bootenv -s backup %s", dstFileName);
    if (WEXITSTATUS(system(buf)) != 0)
      printf("%s: error updating backup environment variable\n", __FUNCTION__);
#endif
  }

  return L7_SUCCESS;
}



/*********************************************************************
 * @purpose  Deletes an image from the list of boot images 
 *           
 * @param    fileName    @b{(input)}  file name of the image to be deleted
 *
 * @returns  L7_SUCCESS        on successful execution
 * @returns  L7_NOT_EXIST      if the image could not be found.
 *
 * @notes    This function should only be called from the bootrom. It
 *           simply deletes the image without performing any sanity checks.
 *
 * @end
 *********************************************************************/
L7_RC_t dimBootromImageDelete(L7_uchar8 *fileName)
{
  char buf[100];
  L7_RC_t rc = L7_NOT_EXIST;

  if (strncmp(fileName, activeImageFileName, DIM_MAX_FILENAME_SIZE) == 0)
  {
    /* delete the internal details */
    activeImageValid = L7_FALSE;
    remove(activeImageFileName);
    snprintf(buf, sizeof(buf), "%s.dsc", activeImageFileName);
    remove(buf);
    memset(activeImageFileName, 0, sizeof(activeImageFileName));
    rc = L7_SUCCESS;
  }
    
  if (strncmp(fileName, backupImageFileName, DIM_MAX_FILENAME_SIZE) == 0)
  {
    /* delete the internal details */
    backupImageValid = L7_FALSE;
    remove(backupImageFileName);
    snprintf(buf, sizeof(buf), "%s.dsc", backupImageFileName);
    remove(buf);
    memset(backupImageFileName, 0, sizeof(backupImageFileName));
    rc = L7_SUCCESS;
  }

  return rc;
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
L7_BOOL dimIsBackupImageActivated(void)
{
  return backupImageActivated;
}

/*********************************************************************
 * @purpose  Associates a given text description for an image
 *           
 * @param    image         @b{(input)}  image name
 * @param    descr         @b{(input)}  text to be associated
 * 
 * @returns  L7_SUCCESS   on successful execution
 * @returns  L7_FAILURE   if unable to write the text description
 *
 * @end
 *********************************************************************/
L7_RC_t dimImageDescrSet(char *image, char *descr)
{
  char desrFile[DIM_MAX_FILENAME_SIZE+4];
  int  descFd;
  L7_RC_t retCode = L7_FAILURE;

  snprintf(desrFile, sizeof(desrFile), "%s.dsc", image);
  descFd = creat(desrFile, O_WRONLY);
  if (descFd >= 0)
  {
    if (write(descFd, descr, strlen(descr)) > 0)
    {
      retCode = L7_SUCCESS;
    }
    close(descFd);
  }
  return retCode;
}

/*********************************************************************
 * @purpose  retrieves the text description for an image
 *           
 * @param    image         @b{(input)}  image name
 * @param    descr         @b{(input)}  pointer to copy the text to
 * 
 * @returns  L7_SUCCESS   on successful execution
 * @returns  L7_FAILURE   if unable to read the text description
 *
 * @end
 *********************************************************************/
L7_RC_t dimImageDescrGet(char *image, char *descr)
{
  char desrFile[DIM_MAX_FILENAME_SIZE+4];
  int  descFd;
  int  descLen;
  L7_RC_t retCode = L7_NOT_EXIST;

  snprintf(desrFile, sizeof(desrFile), "%s.dsc", image);
  if (osapiFsOpen(desrFile, &descFd) == L7_SUCCESS)
  {
    descLen = read(descFd, descr, L7_CLI_MAX_STRING_LENGTH);
    if (descLen > 0)
    {
      descr[descLen] = '\0';
      retCode = L7_SUCCESS;
    }
    close(descFd);
  }
  return retCode;
}

/*********************************************************************
 * @purpose  Displays the boot configuration file 
 *           
 * @param    none
 *
 * @notes    
 *
 * @end
 *********************************************************************/
int dimShowBootCfg(void)
{
  char imageDescr[L7_CLI_MAX_STRING_LENGTH+1];

  if (dimImageDescrGet(activeImageFileName, imageDescr) != L7_SUCCESS)
  {
    imageDescr[0] = '\0';
  }
  printf("\nDim Configuration:\n\n");
  printf("  Active image - %s, %s\n", activeImageFileName, imageDescr);
  if (backupImageValid)
  {
    if (dimImageDescrGet(backupImageFileName, imageDescr) != L7_SUCCESS)
    {
      imageDescr[0] = '\0';
    }
    printf("  Backup image - %s, %s", backupImageFileName, imageDescr);
    if (backupImageActivated)
      printf(" - ACTIVATED");
    printf("\n");
  }
  return 0;
}

#endif /* DIM_CFG_FILENAME */


