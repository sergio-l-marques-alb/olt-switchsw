/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 **********************************************************************
 *
 * @filename     dim.c
 *
 * @purpose      dual image Manager - Implementation
 *
 * @component    dim
 *
 * @comments     none
 *
 * @create       03/04/2005
 *
 * @author       bviswanath
 * @end
 *
 **********************************************************************/

/* ===================== Include Headers ===================== */

#include <stdlib.h>
#include <fcntl.h>

#ifdef _L7_OS_VXWORKS_
#include <ioLib.h>
#endif
#ifdef _L7_OS_LINUX_
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef _L7_OS_ECOS_
#include <unistd.h>
#include <cyg/hal/bcmnvram.h>
#endif

#include <string.h>
#include <stdio.h>

#include "dim.h"
#include "bspapi.h"
#include "osapi.h"

#ifdef _L7_OS_ECOS_
L7_int32 osapiBootCfgOpen(L7_char8 *localfilename);
void osapicloseBootCfg(void);
int osapireadBootCfg(  int filedesc,L7_uchar8 *buffer,L7_uint32 size);
int osapiwriteBootCfg(void);
L7_int32 osapiTransferOpen(L7_uint32 direction, L7_uint32 fileType,L7_char8*filelocal);
extern char* nvram_get(const char *name);
extern int nvram_set (const char *name, const char *value);
extern int nvram_commit (void);
#endif
/* ===================== Global Variables ==================== */

dimImageInfo_t bootImage_g[MAX_BOOT_IMAGES];
int numImages_g = 0;

/* ===================== Function Definitions ================ */

/*********************************************************************
 * @purpose  Returns a string ending with a newline character from the
 *           buffer.
 *
 * @param    src     @b{(input)}  Name of the file containing the image
 * @param    line    @b{(output)} Location to fill in the pointer to a
 *                                String ending with a newline
 *
 * @returns  number where the next string begins in the buffer
 * @returns  -1  if no newline character could be found with in limit
 *
 * @notes
 * @end
 *********************************************************************/
static int readline(char *src, char **line)
{
  int i = 0;

  *line = NULL;

  while ((src[i] != '\n') && (i < DIM_MAX_BOOTCFG_LINE_SIZE))
    i++;

  if (i == DIM_MAX_BOOTCFG_LINE_SIZE)
    return -1;

  *line = src;    /* point to the begining of the null terminated string */

  src[i] = '\0';  /* string is to be null terminated */

  return(i + 1);  /* where to begin the next string */
}

#ifdef BOOTENV_SUPPORT
/*********************************************************************
 * @purpose  Read the active image file name from the environment variables
 *
 * @param    fileName - pointer to location to write the file name
 *
 * @returns  0  valid configuration file name returned.
 * @returns  -1 unable to determine active image file name
 *
 * @end
 *********************************************************************/
static int dimActiveEnvRead(char *fileName)
{
  char buf[50];
  int  fd, rd;

  /* U-boot specific environment variable setting */
  sprintf(buf, "bootenv -d active >" ACTIVE_CFG_FILE);
  if (WEXITSTATUS(system(buf)) != 0)
    return(DIM_FAILURE);

  /* open the boot.dim file from the flash */
  fd = open(ACTIVE_CFG_FILE, O_RDONLY, 0);

  if ( fd == -1)
  {
    printf(" Unable to open active configuration file\n");
    return -1;
  }

  /* read the file into the buffer */  
  rd = read(fd, fileName, CFG_FILE_NAME_LEN);
  if (rd == -1)
  {
    printf(" Unable to read active configuration file\n");
    close(fd);
    return -1;
  }
  fileName[rd] = '\0';
  return 0;
}
#endif  /* BOOTENV_SUPPORT */

/*********************************************************************
 * @purpose  Reconstructs the boot config file 
 *
 * @param    none
 * 
 * @returns  void *
 * @notes    A power-off during writeBootCfg() can leave the boot.dim 
 *           file in a state where it can be opened but not read. In 
 *           that case, recostruct it to the extent possible. 
 *
 * @end
 *********************************************************************/
#ifndef _L7_OS_ECOS_
static void reconstructBootCfg(void) {
  char buf[50];
  L7_int32 fd;

  /* When we see this condition we won't have the DIM globals set. 
     Set them up now. */
#ifdef BOOTENV_SUPPORT
  /* If we have bootenv, we can figure out which image is active */
  if (dimActiveEnvRead(buf) != 0) {
    strcpy(buf, DIM_IMAGE1_NAME);
  }
#else
  /* If we don't know, assume image1 is the active one */
  strcpy(buf, DIM_IMAGE1_NAME);
#endif
  (void)osapiFsDeleteFile(DIM_CFG_FILENAME);

  /* Add the image we consider active, if it exists. */
  fd = osapiFsOpen(buf);
  if (fd >= 0) {
    close(fd);
    (void)dimImageAdd(buf);
  }
  /* Add the other one */
  if (strcmp(buf, DIM_IMAGE1_NAME) == 0) {
    fd = osapiFsOpen(DIM_IMAGE2_NAME);
    if (fd >= 0) {
      close(fd);
      (void)dimImageAdd(DIM_IMAGE2_NAME);
    }
  } else {
    fd = osapiFsOpen(DIM_IMAGE1_NAME);
    if (fd >= 0) {
      close(fd);
      (void)dimImageAdd(DIM_IMAGE1_NAME);
    }
  }
}
#endif /* not eCos */

/*********************************************************************
 * @purpose  Updates the boot config file 
 *
 * @param    none
 * 
 * @returns  0   if the file is successfully updated
 * @returns  -1  if the file could not be opened for writing
 * @returns  -2  if the file could not be written
 *
 * @notes    A buffer is filled with data from each of the image desc. and 
 *           is written to the file. Note that the file is not "updated'
 *           but written again. This is because the typical file size 
 *           being small (100-200 bytes) and the number of times the
 *           file is updated is very few. Therefore the additional logic
 *           required to "update' is not preferred.
 * @end
 *********************************************************************/
#ifndef _L7_OS_ECOS_
static int writeBootCfg(void)
{
  int filedesc;
  int act_size = 0;
  int j = 0, images=0;
  char *pBuf;

  char buffer[MAX_BOOTCFG_SIZE];
  dimImageInfo_t *pImage;

  /* open the boot.dim file from the flash */

  (void)osapiFsDeleteFile(DIM_CFG_FILENAME);

  filedesc = open(DIM_CFG_FILENAME, (O_WRONLY | O_CREAT), 0);

  if (filedesc < 0)
  {
    return -1;
  }

  pBuf = &buffer[0];
  strcpy(pBuf, "");

  /* fill the buffer with Image details */

  memset(pBuf, 0, MAX_BOOTCFG_SIZE);

  for (j = 0; j < MAX_BOOT_IMAGES; j++)
  {
    if (images == numImages_g)
      break;

    pImage = &bootImage_g[j];

    /* if not a valid image , go to the next image*/

    if (strcmp(pImage->fileName, "") == 0x0)
      continue;

    images++;

    pBuf = strcat(pBuf, "---IMAGE---");
    pBuf = strcat(pBuf, "\n");
    pBuf = strcat(pBuf, pImage->currentState);
    pBuf = strcat(pBuf, "\n");
    pBuf = strcat(pBuf, pImage->nextState);
    pBuf = strcat(pBuf, "\n");
    pBuf = strcat(pBuf, pImage->fileName);
    pBuf = strcat(pBuf, "\n");
    pBuf = strcat(pBuf, pImage->numErrors);
    pBuf = strcat(pBuf, "\n");
    pBuf = strcat(pBuf, pImage->descr);
    pBuf = strcat(pBuf, "\n");

  } /* end for */

  act_size = strlen(pBuf);

  /* write the file and close it */

  j = write(filedesc, &buffer[0], act_size);

  (void)close(filedesc);

  return j;
}
#endif

/*********************************************************************
 * @purpose  Initializes the dual boot image manager
 *
 * @param    none
 *
 * @returns  DIM_SUCCESS  on successful initialization
 * @returns  DIM_IMAGE_DOESNOT_EXIST  if file doesn't exist or no 
 *                                    images canbe found
 * @return   DIM_FAILURE  on file system errors
 *
 * @notes    Reads any existing boot config file and updates internals.
 *           If no such file exists, it simply returns after initializing
 *           the number of images to be 0. File will be added when an
 *           image is added to the list.
 *           When called from a bootloader context (isBoot != 0), this 
 *           function copies the nextStates of each of the images to to
 *           their currentStates.Thus, the activated image becomes the 
 *           active image for the new session.
 *
 * @end
 *********************************************************************/
int dimInitialize(void)
{
  int filedesc;
  int act_size = 0;
  int i = 0, image_index = 0, j = 0;
  char *line;
  char *pBuf;

  char buffer[MAX_BOOTCFG_SIZE];
  dimImageInfo_t *pImage;
#ifdef BOOTENV_SUPPORT
  char activeEnv[CFG_FILE_NAME_LEN+1];
  int  rc;
#endif

  /* clear the existing information */

  for (i = 0; i < MAX_BOOT_IMAGES; i++)
  {
    memset(&bootImage_g[i], 0, sizeof(dimImageInfo_t));
  }

  numImages_g = 0;

  /* open the boot.dim file from the flash */
#ifdef _L7_OS_ECOS_
  filedesc = osapiBootCfgOpen(DIM_CFG_FILENAME);
  if ( filedesc == L7_ERROR || filedesc == L7_NULL )
  {
    dimImageAdd(DIM_IMAGE1_NAME);
    /* the file is empty.return now */
    return DIM_IMAGE_DOESNOT_EXIST;
  }

#else
  filedesc = open (DIM_CFG_FILENAME, O_RDONLY, 0);
  if (filedesc < 0)
  {
    reconstructBootCfg();
    /* the file is empty.return now */
    return DIM_SUCCESS;
  }
#endif
  memset(buffer, 0, MAX_BOOTCFG_SIZE);

  /* read the file into the buffer */
#ifdef _L7_OS_ECOS_
  act_size = osapireadBootCfg(filedesc,(L7_uchar8*)buffer,MAX_BOOTCFG_SIZE);
  if(act_size < 0)
  {
    osapicloseBootCfg();
    return DIM_FAILURE;
  }
  if(act_size==0)
  {
    osapicloseBootCfg();
    return DIM_IMAGE_DOESNOT_EXIST;
  }
#else
  act_size =  (int)read(filedesc, buffer, MAX_BOOTCFG_SIZE);

  if (act_size < 0)
  {
    close(filedesc);
    reconstructBootCfg();
    return DIM_SUCCESS;
  }

  if (act_size == 0)
  {
    /* the file is empty. We need to formulate this */
    close(filedesc);
    reconstructBootCfg();
    return DIM_SUCCESS;
  }
#endif
  /* parse the buffer into the internal data structures */

  i = j = 0;

  pBuf = &buffer[0];
  image_index = 0;

  while(( i = readline(pBuf, &line)) > 0)
  {
    pBuf += i;

    /* check if this line indicates the begining of a new image description */

    if (strcmp(line, "---IMAGE---") == 0)
    {
      /* Go to the next image and continue with its details */

      image_index++;

      if (image_index > MAX_BOOT_IMAGES)
        break;

      numImages_g++;

      j = 0;

      continue;
    }

    /* Populate the Image details with the data from boot.dim */
    /* the image's next state will now be the current state for the 
     * image. the images next state will also has to be set
     * accordingly
     * */

    pImage = &bootImage_g[image_index - 1];

    switch (j)
    {
      case 0:
        memset(pImage->currentState, 0, sizeof(pImage->currentState));
        strncpy(pImage->currentState, line, sizeof(pImage->currentState) - 1);
        break;
      case 1:
        memset(pImage->nextState, 0, sizeof(pImage->nextState));
        strncpy(pImage->nextState, line, sizeof(pImage->nextState) - 1);
        break;
      case 2:
        memset(pImage->fileName, 0, sizeof(pImage->fileName));
        strncpy(pImage->fileName, line, sizeof(pImage->fileName) - 1);
        break;
      case 3:
        memset(pImage->numErrors, 0, sizeof(pImage->numErrors));
        strncpy(pImage->numErrors, line, sizeof(pImage->numErrors) - 1);
        break;
      case 4:
        memset(pImage->descr, 0, sizeof(pImage->descr));
        strncpy(pImage->descr, line, sizeof(pImage->descr) - 1);
        break;
      default:
        break;
    }

    j++;
  }

  /* all the description is read into the memory */



#ifdef _L7_OS_ECOS_
  osapicloseBootCfg();
#else
  (void)close (filedesc);
#endif
#ifdef BOOTENV_SUPPORT
  /* For Linux, read the environment variable in case it has been changed */
  /* by u-boot or the Utility menu.                                       */
  memset(activeEnv, 0, sizeof(activeEnv));
  if (dimActiveEnvRead(activeEnv) >= 0)
  {
    rc = dimImageActivate(activeEnv);
  }
#endif

  /* adjust the current and next states for the images 
   * if the next state and the current state of the image is not the same
   * it indicates that there is a re-boot since an image is made
   * active. 
   */  

  for (image_index = 0; image_index < numImages_g; image_index++)
  {
    pImage = &bootImage_g[image_index];

    memset(pImage->currentState, 0, sizeof(pImage->currentState));
    strncpy(pImage->currentState, pImage->nextState, sizeof(pImage->currentState) - 1);
  }
#ifdef _L7_OS_ECOS_
  char *str_ptr=nvram_get("STARTUP");
  int index=0;
  if(str_ptr!=NULL)
  {
    if(strcmp(str_ptr,ACTIVATE_IMAGE1)==0)
    {
      if(strcmp( bootImage_g[index].nextState,"backup")==0)
      {
        memset(bootImage_g[index].nextState,0x0,sizeof(bootImage_g[index].nextState));
        strcpy( bootImage_g[index].nextState,"active");
        strcpy(bootImage_g[index].currentState, bootImage_g[index].nextState);
        memset(bootImage_g[index+1].nextState,0x0,sizeof(bootImage_g[index+1].nextState));
        strcpy( bootImage_g[index+1].nextState,"backup");
        strcpy(bootImage_g[index+1].currentState, bootImage_g[index+1].nextState);
      }
    }
    else if (strcmp(str_ptr,ACTIVATE_IMAGE2)==0)
    {
      if(strcmp( bootImage_g[index+1].nextState,"backup")==0)
      {
        memset(bootImage_g[index+1].nextState,0x0,sizeof(bootImage_g[index+1].nextState));
        strcpy( bootImage_g[index+1].nextState,"active");
        strcpy(bootImage_g[index+1].currentState, bootImage_g[index+1].nextState);
        memset(bootImage_g[index].nextState,0x0,sizeof(bootImage_g[index].nextState));
        strcpy( bootImage_g[index].nextState,"backup");
        strcpy(bootImage_g[index].currentState, bootImage_g[index].nextState);
      }
    }
    else
    {
      printf(" Invalid Flash Image activated \n");
      return DIM_FAILURE;
    }
  }
#endif     


  /* we made some modifications, write back the configuration file */
#ifdef _L7_OS_ECOS_
  osapiwriteBootCfg();
#else
  writeBootCfg();
#endif
  return DIM_SUCCESS;
}

/*********************************************************************
 * @purpose  Returns the number of boot errors for the active Image 
 *
 * @param    pErrors     @b{(output)} Location to fill in the number of 
 *                                    boot erros for the active image
 *                                    
 * @returns  0   on successful execution
 * @returns  -1  if no image is marked as active
 *
 * @notes    Data is read from the internal data structures and is 
 *           returned.
 *           It is envisaged that the bootloader increments the error count
 *           every time it loads an image. FP on successful loading of 
 *           the image, clears the error count.
 *           If the number of errors are too-high, bootloader may choose
 *           to boot an alternate image, if any, from the flash.
 *
 * @end
 *********************************************************************/
int dimActiveImageErrorsGet(int *pErrors)
{
  int index;

  /* If no images are available, return error */

  if (numImages_g == 0)
    return -1;

  /* check if there is an active image */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].currentState, "active") == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return -1;

#ifndef _L7_OS_ECOS_
  if (L7_FALSE == dimImageExists(bootImage_g[index].fileName))
  {
    return -1;
  }
#endif
  /* get the number of errors, convert the string to an integer
   * and return */

  *pErrors = atoi(bootImage_g[index].numErrors);

  return 0;
}

/*********************************************************************
 * @purpose  Sets the number of boot errors for the active Image 
 *
 * @param    errors      @b{(input)}  Number of boot erros for the image
 *
 * @returns  0   on successful execution
 * @returns  -1  if no image is marked as active yet
 *
 * @notes    Internal data structures are updated with this information.
 *           the boot config file is also updated.
 *           It is envisaged that the bootloader increments the error count
 *           every time it loads an image. FP on successful loading of 
 *           the image, clears the error count.
 *           If the number of errors are too-high, bootloader may choose
 *           to boot an alternate image, if any, from the flash.
 *
 * @end
 *********************************************************************/
int dimActiveImageErrorsSet( int errors)
{
  int index;

  /* If no images are available, return error */

  if (numImages_g == 0)
    return -1;

  /* check if there is an active image */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].currentState, "active") == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return -1;
#ifndef _L7_OS_ECOS_
  if (L7_FALSE == dimImageExists(bootImage_g[index].fileName))
  {
    return -1;
  }
#endif
  /* get the number of errors, convert the integer to a string */

  sprintf(bootImage_g[index].numErrors,"%d",errors);

  /* Update the bootconfig file */
#ifdef _L7_OS_ECOS_
  osapiwriteBootCfg();
#else    
  writeBootCfg();
#endif
  return 0;
}

/*********************************************************************
 * @purpose  Retrieves the file name for the current active image 
 *           
 * @param    fileName    @b{(output)}  Location to copy the  
 *                                     file name for the active image
 *
 * @returns  0   on successful execution
 * @returns  -1  if no image is marked as active yet
 *
 * @notes    File name is copied.
 *
 * @end
 *********************************************************************/
int dimActiveImageFileNameGet(char *fileName)
{
  int index = 0;

  /* If no images are added, return error */

  if (numImages_g == 0)
    return -1;

  /* If no image is marked as active, return error */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].currentState, "active") == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return -1;

  /* copy the file name */

  strcpy(fileName, bootImage_g[index].fileName);
  return 0;
}

/*********************************************************************
 * @purpose  Retrieves the file name for the current backup image 
 *           
 * @param    fileName    @b{(output)}  Location to copy file name of the  
 *                                     backup image
 *
 * @returns  0   on successful execution
 * @returns  -1  if there is no backup image yet
 *
 * @notes    File name is copied.
 *
 * @end
 *********************************************************************/
int dimBackupImageFileNameGet(char *fileName)
{
  int index = 0;

  /* If no images are added, return error */

  if (numImages_g == 0)
    return -1;

  /* If no image is marked as active, return error */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].currentState, "backup") == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return -1;

  /* copy the file name */

  strcpy(fileName, bootImage_g[index].fileName);
  return 0;
}

/*********************************************************************
 * @purpose  Retrieves the name for the currently configured backup
 *           image even if the image is not present on the box
 *
 * @param    fileName    @b{(output)}  Location to copy name of the
 *                                     backup image
 *
 * @returns  L7_SUCCESS   on successful execution
 * @returns  L7_ERROR     if there is no active image yet
 *
 * @notes    File name is copied.
 *
 * @end
 *********************************************************************/
int dimBackupImageNameGet(char *fileName)
{
  if (0 != dimActiveImageFileNameGet(fileName))
  {
    return L7_ERROR;
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

/*********************************************************************
 * @purpose  Sets the supplied image as the active image for the 
 *           subsequent re-boots
 *           
 * @param    fileName    @b{(input)}  image to be activated
 *
 * @returns  DIM_SUCCESS   on successful execution
 * @returns  DIM_IMAGE_DOESNOT_EXIST  if the specified image could not be found
 * @returns  DIM_INVALID_IMAGE  if the specified image is not the backup image
 * @return   DIM_IMAGE_ACTIVE   if the backup image is already active
 * @return   DIM_FAILURE        on internal error
 *
 * @notes    Internal data structures and the boot configuration file
 *           are updated with the information. Image currently  
 *           active is marked as backup and the specified image is 
 *           marked as active. the number of errors for the backup 
 *           image are cleared to 0. 
 *
 * @end
 *********************************************************************/
int dimImageActivate(char *fileName)
{
  int index = 0, def_index = 0;

#ifdef _L7_OS_LINUX_
  char buf[100];
  int rc;
  struct stat st;
#endif

  /* If there are no images, return error */

#ifndef _L7_OS_ECOS_
  if ((numImages_g == 0) ||
      (L7_FALSE == dimImageExists(fileName)))
#else
    if(numImages_g == 0)
#endif
    {
      return DIM_IMAGE_DOESNOT_EXIST;
    }



  /* Verify that the image exists. Otherwise return error */

  if ((strcmp(fileName, DIM_IMAGE1_NAME) != 0) &&
      (strcmp(fileName, DIM_IMAGE2_NAME) != 0))
  {
    return DIM_IMAGE_DOESNOT_EXIST;
  }

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, fileName) == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return DIM_IMAGE_DOESNOT_EXIST;

  /* verify that the image is a backup image */

  /*if(strcmp(bootImage_g[index].currentState, "backup") != 0x0)
    return DIM_INVALID_IMAGE; */

  /* verify that the image is not activated */

  if (strcmp(bootImage_g[index].nextState, "active") == 0x0)
    return DIM_IMAGE_ACTIVE;


  /* check if there is an image currently marked active */
  /* if so, mark it backup                              */
  for (def_index = 0; def_index < MAX_BOOT_IMAGES; def_index++)
  {
    if (strcmp(bootImage_g[def_index].nextState, "active") == 0x0)
      break;
  }

  if (def_index <  MAX_BOOT_IMAGES)
  {
    strcpy(bootImage_g[def_index].nextState, "backup");
    strcpy(bootImage_g[def_index].numErrors, "0");

#ifdef BOOTENV_SUPPORT
    /* U-boot specific environment variable setting */
    sprintf(buf,"bootenv -s backup %s", bootImage_g[def_index].fileName);
    if (WEXITSTATUS(system(buf)) != 0)
      return(DIM_FAILURE);
  }
  else
  {
    sprintf(buf,"bootenv -s backup %s", "");
    if (WEXITSTATUS(system(buf)) != 0)
      return(DIM_FAILURE);
#endif
  }

  /* Activate the current backup image */

  strcpy(bootImage_g[index].numErrors, "0");
  strcpy(bootImage_g[index].nextState, "active");
#ifdef _L7_OS_ECOS_
  if(strcmp("image1",fileName)==0)
  { 
    nvram_set("STARTUP","boot -z -elf flash0.os:");
    nvram_commit();
  }
  else if(strcmp("image2",fileName)==0)
  {
    nvram_set("STARTUP","boot -z -elf flash0.os1:");
    nvram_commit();
  }
  else
  {
    printf("\n Not a valid image");
    return DIM_FAILURE;
  }
#endif
#ifdef BOOTENV_SUPPORT
  /* U-boot specific environment variable setting */
  sprintf(buf,"bootenv -s active %s", bootImage_g[index].fileName);
  if (WEXITSTATUS(system(buf)) != 0)
    return(DIM_FAILURE);
#endif
#ifdef _L7_OS_LINUX_
  /* A script is included in the .stk file to handle any necessary */
  /* hardware specific requirements to activate the new image.     */
  if (stat("/usr/sbin/extimage", &st) == 0)
  {
    snprintf(buf, sizeof(buf), "cd %s ; extimage -i %s -o %sACTIVATE -n 2",
        CONFIG_PATH, bootImage_g[index].fileName, DOWNLOAD_PATH);
    if (WEXITSTATUS(system(buf)) != 0)
      return(DIM_FAILURE);

    snprintf(buf, sizeof(buf), "cd %s ; sh ACTIVATE -q %s", DOWNLOAD_PATH,
        bootImage_g[index].fileName);
    rc = WEXITSTATUS(system(buf));
    if ((rc != 0) && (rc != 1))
      return(DIM_FAILURE);
  }
#endif

  /* update the current boot configuration file */
#ifdef _L7_OS_ECOS_
  osapiwriteBootCfg();
#else
  writeBootCfg();
#endif
  return DIM_SUCCESS;
}

/*********************************************************************
 * @purpose  Adds an image to the list of images 
 *           
 * @param    fileName    @b{(input)}  file for the backup image 
 *
 * @returns  0   on successful execution
 * @returns  -1  if the allowed max number of images reached already
 * 
 * @notes    Internal data structures and the boot configuration file
 *           are updated with the information. 
 *
 *           if there are no images existing, this will be added as the
 *           active image.
 *           if there are already images, this will be added as the 
 *           backup image.
 *           If a backup image is actiavted, it will not be overwritten.
 *
 * @end
 *********************************************************************/
int dimImageAdd(char *fileName)
{
  int index = 0;

#ifdef _L7_OS_LINUX_
  char buf[100];
  int rc;
  struct stat st;
#endif

  /* The image names are fixed.
   * check if this name is one of the allowed names
   */

  if ((strcmp(fileName, DIM_IMAGE1_NAME) != 0) &&
      (strcmp(fileName, DIM_IMAGE2_NAME) != 0))
  {
    return -3;
  }

#ifndef _L7_OS_ECOS_
  if (L7_FALSE == dimImageExists(fileName))
  {
    return -2;
  }
#endif

  if (numImages_g == 0)
  {
    numImages_g++;

    /* copy the image details */

    memset(bootImage_g[0].fileName, 0, sizeof(bootImage_g[0].fileName));
    strncpy(bootImage_g[0].fileName, fileName, sizeof(bootImage_g[0].fileName) - 1);
    strcpy(bootImage_g[0].numErrors, "0");
    strcpy(bootImage_g[0].currentState, "active");
    strcpy(bootImage_g[0].nextState, "active");
    strcpy(bootImage_g[0].descr, "default image");

#ifdef BOOTENV_SUPPORT
    /* U-boot specific environment variable setting */
    sprintf(buf,"bootenv -s active %s", fileName);
    if (WEXITSTATUS(system(buf)) != 0)
      return(DIM_FAILURE);
#endif
#ifdef _L7_OS_LINUX_
    /* A script is included in the .stk file to handle any necessary */
    /* hardware specific requirements to activate the new image.     */
    if (stat("/usr/sbin/extimage", &st) == 0)
    {
      snprintf(buf, sizeof(buf), "cd %s ; extimage -i %s -o %sACTIVATE -n 2",
          CONFIG_PATH, bootImage_g[index].fileName, DOWNLOAD_PATH);
      if (WEXITSTATUS(system(buf)) != 0)
        return(DIM_FAILURE);

      snprintf(buf, sizeof(buf), "cd %s ; sh ACTIVATE -q %s", DOWNLOAD_PATH,
          bootImage_g[index].fileName);
      rc = WEXITSTATUS(system(buf));
      if ((rc != 0) && (rc != 1))
        return(DIM_FAILURE);
    }
#endif

    /* update the bootcfg */
#ifdef _L7_OS_ECOS_
    if(strcmp("image1",fileName)==0)
    {
      if(nvram_set("STARTUP",ACTIVATE_IMAGE1) == 0)
      {
        nvram_commit();
      }
    }
    else if(strcmp("image2",fileName)==0)
    {
      if(nvram_set("STARTUP",ACTIVATE_IMAGE2) == 0)
      {
        nvram_commit();
      }
    }
    else
    {
      printf(" Not a valid image\n ");
      return -1;
    }
#endif
#ifdef _L7_OS_ECOS_
    osapiwriteBootCfg();
#else     
    writeBootCfg();
#endif
    return 0;
  }

  /* check if any file exists with the same name */
  /* if so return */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, fileName) == 0x0)
    {
#ifdef _L7_OS_LINUX_
      /* If we are over-writing the active image then run the 
         ACTIVATE script */
      if (strcmp(bootImage_g[index].nextState, "active") == 0)
      {
        if (stat("/usr/sbin/extimage", &st) == 0) {
          snprintf(buf, sizeof(buf), "cd %s ; extimage -i %s -o %sACTIVATE -n 2", 
              CONFIG_PATH, bootImage_g[index].fileName, DOWNLOAD_PATH);
          if (WEXITSTATUS(system(buf)) != 0)
            return(DIM_FAILURE);

          snprintf(buf, sizeof(buf), "cd %s ; sh ACTIVATE -q %s", DOWNLOAD_PATH, 
              bootImage_g[index].fileName);
          rc = WEXITSTATUS(system(buf));
          if ((rc != 0) && (rc != 1))
            return(DIM_FAILURE);
        }
      }
#endif
      return 0;
    }
  }

  /* look for an empty slot and mark the image as backup */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, "") == 0x0)
    {
      numImages_g++;

      /* copy the image details */

      memset(bootImage_g[index].fileName, 0, sizeof(bootImage_g[index].fileName));
      strncpy(bootImage_g[index].fileName, fileName, sizeof(bootImage_g[index].fileName) - 1);
      strcpy(bootImage_g[index].numErrors, "0");
      strcpy(bootImage_g[index].currentState, "backup");
      strcpy(bootImage_g[index].nextState, "backup");
      strcpy(bootImage_g[index].descr, "");

      /* update the bootcfg */
#ifdef _L7_OS_ECOS_
      osapiwriteBootCfg();
#else
      writeBootCfg();
#endif
      return 0;
    }
  }

  return -1;
}

/*********************************************************************
 * @purpose  Deletes an image from the list of boot images 
 *           
 * @param    fileName    @b{(input)}  file containing the image to be 
 *                                deleted from the list 
 *
 * @returns  DIM_SUCCESS   on successful execution
 * @returns  DIM_IMAGE_DOESNOT_EXIST  if the image could not be found.
 * @returns  DIM_INVALID_IMAGE  if the image is currently active.
 * @returns  DIM_IMAGE_ACTIVE if the specified backup image is activated.
 *
 * @notes    Internal data structures and the boot configuration file
 *           are updated with the information. The actual file is not 
 *           deleted from the filesystem.
 *
 *           current active image is never deleted.
 *           activated backup image is never deleted.
 *           
 *
 * @end
 *********************************************************************/
int dimImageDelete(char *fileName)
{
  int index = 0;

#ifdef BOOTENV_SUPPORT

  char buf[100];

#endif

#ifndef _L7_OS_ECOS_
  if ((numImages_g == 0) ||
      (L7_FALSE == dimImageExists(fileName)))
#else
    if(numImages_g == 0)
#endif
    {
      return DIM_IMAGE_DOESNOT_EXIST;
    }


  /* check for the image in the list */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, fileName) == 0x0)
      break;
  }

  /* if the image doesn't exist, return error */

  if (index == MAX_BOOT_IMAGES)
    return DIM_IMAGE_DOESNOT_EXIST;

  /* if this is the default image, return error */

  if (strcmp(bootImage_g[index].currentState, "active") == 0x0)
    return DIM_INVALID_IMAGE;

  /* if this is the default image, return error */

  if (strcmp(bootImage_g[index].nextState, "active") == 0x0)
    return DIM_IMAGE_ACTIVE;

  /* delete the internal details */

  memset(&bootImage_g[index], 0, sizeof(dimImageInfo_t));

  numImages_g--;

#ifdef BOOTENV_SUPPORT
  /* U-boot specific environment variable setting */
  sprintf(buf,"bootenv -u backup");
  if (WEXITSTATUS(system(buf)) != 0)
    return(DIM_FAILURE);
#endif

  /* effect this on the flash */
#ifdef _L7_OS_ECOS_
  osapiwriteBootCfg();
#else
  writeBootCfg();
#endif
  return DIM_SUCCESS;
}

/*********************************************************************
 * @purpose  Retrieves the Number of images in the list 
 *           
 * @param    pNum    @b{(output)}  Location to copy the number of images
 *
 * @returns  0   always
 * 
 * @notes    
 *
 * @end
 *********************************************************************/
int dimNumImagesGet(int *pNum)
{
  *pNum = numImages_g;
  return 0;
}

/*********************************************************************
 * @purpose  provides formatted text, giving information about 
 *           the current lis of images 
 *           
 * @param    pImage  @b{(output)}  Location to copy the text
 * @returns  0   always
 * 
 * @notes    care must be taken to provide sufficienty big buffer for
 *           holding the text.
 *
 * @end
 *********************************************************************/
int dimImagesInfoGet(char *pImage)
{
  return 0;
}

/*********************************************************************
 * @purpose  Checks if the supplied filename belongs an image
 *           
 * @param    fileName @b{(input)}  File name to be verified
 * @param    answer @b{(output)}   Loction to place the output
 *                                 The following are valid values -
 *                                 DIM_IMAGE_DOESNOT_EXIST
 *                                 DIM_VALID_IMAGE
 *
 * @returns  0   on success
 * 
 * @notes    
 *
 * @end
 *********************************************************************/
int dimIsAnImage(char *fileName, int *answer)
{
  int index = 0;

  *answer = DIM_IMAGE_DOESNOT_EXIST;

#ifndef _L7_OS_ECOS_
  if ((numImages_g == 0) ||
      (L7_FALSE == dimImageExists(fileName)))
#else
    if(numImages_g == 0)
#endif 
    {
      return 0;
    }

  /* check for the image in the list */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, fileName) == 0x0)
      break;
  }

  /* if the image doesn't exist, return error */

  if (index != MAX_BOOT_IMAGES)
    *answer = DIM_VALID_IMAGE;

  return 0;
}

/*********************************************************************
 * @purpose  Checks if the backup image is activated 
 *           
 * @param    none
 *
 * @returns  DIM_NOT_ACTIVATED
 *           DIM_ACTIVATED
 *           DIM_IMAGE_DOESNOT_EXIST
 *           DIM_FAILURE
 *
 *
 * @notes    
 *
 * @end
 *********************************************************************/
DIM_RC_t dimIsBackupImageActivated(void)
{
  int index = 0;

  if (numImages_g == 0)
    return DIM_FAILURE;

  /* check for the image in the list */
  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].currentState, "backup") == 0x0)
      break;
  }

  /* if the image doesn't exist, return error */
  if (index == MAX_BOOT_IMAGES)
  {
    return DIM_IMAGE_DOESNOT_EXIST;
  }

  if (strcmp(bootImage_g[index].nextState, "active") == 0x0)
  {
    return DIM_ACTIVATED;
  }
  return DIM_NOT_ACTIVATED;
}

/*********************************************************************
 * @purpose  Displays the boot configuration file 
 *           
 * @param    none
 *
 * @returns  0   on success
 *           -1  on file system errors
 *
 * @notes    
 *
 * @end
 *********************************************************************/
int dimShowBootCfg(void)
{
  int filedesc;
  int act_size = 0;
  int i = 0, j = 0;
  char *line;
  char *pBuf;

  char buffer[1024] = { 0 };

  memset(buffer, 0, sizeof(buffer));

  /* open the boot.dim file from the flash */

  filedesc = open(DIM_CFG_FILENAME, O_RDONLY, 0);

  if ( filedesc == -1)
  {
    /* the file is empty. We need to formulate this */
    printf(" boot.dim doesn't exist on the disk \n");
    return -1;
  }

  /* read the file into the buffer */

  act_size =  (int)read(filedesc, buffer, MAX_BOOTCFG_SIZE);

  if (act_size == -1)
  {
    printf(" boot.dim read error\n");
    close(filedesc);
    return -2;
  }

  printf("\nsize is %d \n", act_size);

  if (act_size == 0)
  {
    printf(" boot.dim empty\n");
    close(filedesc);
    return -3;
  }

  pBuf = &buffer[0];
  j = 0;

  while ((i = readline(pBuf, &line)) > 0)
  {
    j++;
    printf("boot.dim %d :\t\t %s \n",j,line);
    pBuf += i;
  }

  /* all the description is read into the memory */

  (void)close(filedesc);

  return 0;
}

/*********************************************************************
 * @purpose  Renames supplied image with the given name
 *           
 * @param    curFileName    @b{(input)}  image to be renamed
 * @param    newFileName    @b{(input)}  New name for the image
 * 
 * @returns  DIM_SUCCESS   on successful execution
 * @returns  DIM_IMAGE_DOESNOT_EXIST  if the specified image could not be found
 *
 * @notes    Internal data structures and the boot configuration file
 *           are updated with the information. 
 *
 * @end
 *********************************************************************/
int dimImageRename(char *curFileName, char *newFileName)
{
  int index = 0;

#ifdef BOOTENV_SUPPORT

  char buf[100];

#endif

  /* If there are no images, return error */

#ifndef _L7_OS_ECOS_
  if ((numImages_g == 0) ||
      (L7_FALSE == dimImageExists(curFileName)))
#else
    if(numImages_g == 0)
#endif
    {
      return DIM_IMAGE_DOESNOT_EXIST;
    }


  /* Verify that the image exists. Otherwise return error */
  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, curFileName) == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return DIM_IMAGE_DOESNOT_EXIST;

  /* rename the image */

  memset(bootImage_g[index].fileName, 0, sizeof(bootImage_g[index].fileName));
  strncpy(bootImage_g[index].fileName, newFileName, sizeof(bootImage_g[index].fileName) - 1);

#ifdef BOOTENV_SUPPORT

  /* update the environment variables */

  if (strcmp(bootImage_g[index].nextState, "active") == 0)
  {
    sprintf(buf, "bootenv -s active %s", newFileName);
  }
  else
  {
    sprintf(buf, "bootenv -s backup %s", newFileName);
  }

  if (WEXITSTATUS(system(buf)) != 0)
    return(DIM_FAILURE);

#endif

  /* effect this on the flash */
#ifdef _L7_OS_ECOS_
  osapiwriteBootCfg();
#else
  writeBootCfg();
#endif
  return DIM_SUCCESS;
}

/*********************************************************************
 * @purpose  Associates a given text description for an image
 *           
 * @param    image         @b{(input)}  image name
 * @param    descr         @b{(input)}  text to be associated
 * 
 * @returns  DIM_SUCCESS   on successful execution
 * @returns  DIM_IMAGE_DOESNOT_EXIST  if the specified image could not be found
 *
 * @notes    Internal data structures and the boot configuration file
 *           are updated with the information. 
 *
 * @end
 *********************************************************************/
int dimImageDescrSet(char *image, char *descr)
{
  int index = 0;

  /* If there are no images, return error */

#ifndef _L7_OS_ECOS_
  if ((numImages_g == 0) ||
      (L7_FALSE == dimImageExists(image)))
#else
    if(numImages_g == 0)
#endif
    {
      return DIM_IMAGE_DOESNOT_EXIST;
    }

  /* Verify that the image exists. Otherwise return error */
  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, image) == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return DIM_IMAGE_DOESNOT_EXIST;

  /* truncate incoming string if it's too long */
  strncpy(bootImage_g[index].descr, descr, sizeof(bootImage_g[index].descr));
#ifdef _L7_OS_ECOS_
  osapiwriteBootCfg();
#else
  writeBootCfg();
#endif  
  return DIM_SUCCESS;
}

/*********************************************************************
 * @purpose  retrieves the text description for an image
 *           
 * @param    image         @b{(input)}  image name
 * @param    descr         @b{(input)}  pointer to copy the text to
 * 
 * @returns  DIM_SUCCESS   on successful execution
 * @returns  DIM_IMAGE_DOESNOT_EXIST  if the specified image could not be found
 *
 * @notes    Internal data structures and the boot configuration file
 *           are updated with the information. 
 *
 * @end
 *********************************************************************/
int dimImageDescrGet(char *image, char *descr)
{
  int index = 0;

  /* If there are no images, return error */
#ifndef _L7_OS_ECOS_
  if ((numImages_g == 0) ||
      (L7_FALSE == dimImageExists(image)))
#else
    if(numImages_g == 0)
#endif
    {
      return DIM_IMAGE_DOESNOT_EXIST;
    }

  /* Verify that the image exists. Otherwise return error */
  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, image) == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return DIM_IMAGE_DOESNOT_EXIST;

  strcpy(descr, bootImage_g[index].descr);

  return DIM_SUCCESS;
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
L7_BOOL dimImageExists(char *name)
{
  int     filedesc;

  filedesc = open(name, O_RDONLY, 0);
  if (-1 == filedesc)
  {
    return L7_FALSE;
  }

  close(filedesc);
  return L7_TRUE;
}
