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

#include <string.h>
#include <stdio.h>

#include "dim.h"
#include "osapi.h"
#include "platform_cpu.h"
#include "stk.h"

/* ===================== Global Variables ==================== */

dimImageInfo_t bootImage_g[MAX_BOOT_IMAGES];
int numImages_g = 0;

/* ===================== Function Definitions ================ */

#ifdef DIM_CFG_FILENAME 
static L7_BOOL dimImageExists(char *name);
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
static void reconstructBootCfg(void) {
  char buf[50];
  L7_int32 fd;

  /* When we see this condition we won't have the DIM globals set. 
     Set them up now. */

  /* If we don't know, assume image1 is the active one */
  strcpy(buf, DIM_IMAGE1_NAME);

  (void)osapiFsDeleteFile(DIM_CFG_FILENAME);

  /* Add the image we consider active, if it exists. */
  if (osapiFsOpen(buf, &fd) != L7_ERROR)
  {
    close(fd);
    (void)dimImageAdd(buf, buf);
  }
  /* Add the other one */
  if (strcmp(buf, DIM_IMAGE1_NAME) == 0) {
    if (osapiFsOpen(DIM_IMAGE2_NAME, &fd) != L7_ERROR)
    {
      close(fd);
      (void)dimImageAdd(DIM_IMAGE2_NAME, DIM_IMAGE2_NAME);
    }
  } else {
    if (osapiFsOpen(DIM_IMAGE1_NAME, &fd) != L7_ERROR)
    {
      close(fd);
      (void)dimImageAdd(DIM_IMAGE1_NAME, DIM_IMAGE1_NAME);
    }
  }
}

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

/*********************************************************************
 * @purpose  Initializes the dual boot image manager
 *
 * @param    none
 *
 * @returns  STK_SUCCESS  on successful initialization
 * @returns  STK_IMAGE_DOESNOT_EXIST  if file doesn't exist or no 
 *                                    images canbe found
 * @return   STK_FAILURE  on file system errors
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
  char backup_filename[DIM_MAX_FILENAME_SIZE+1];
  int retCode;
    
  /* clear the existing information */

  for (i = 0; i < MAX_BOOT_IMAGES; i++)
  {
    memset(&bootImage_g[i], 0, sizeof(dimImageInfo_t));
  }

  numImages_g = 0;

  /* open the boot.dim file from the flash */
  filedesc = open (DIM_CFG_FILENAME, O_RDONLY, 0);
  if (filedesc < 0)
  {
    reconstructBootCfg();
    /* the file is empty.return now */
    return STK_SUCCESS;
  }
  memset(buffer, 0, MAX_BOOTCFG_SIZE);

  /* read the file into the buffer */
  act_size =  (int)read(filedesc, buffer, MAX_BOOTCFG_SIZE);

  if (act_size < 0)
  {
    close(filedesc);
    reconstructBootCfg();
    return STK_SUCCESS;
  }

  if (act_size == 0)
  {
    /* the file is empty. We need to formulate this */
    close(filedesc);
    reconstructBootCfg();
    return STK_SUCCESS;
  }
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



  (void)close (filedesc);

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


  /* we made some modifications, write back the configuration file */
  writeBootCfg();
  /*Validate the backup image*/
  memset(backup_filename,0x0,sizeof(backup_filename)); 
  dimBackupImageFileNameGet(backup_filename);
  if (dimImageExists(backup_filename) == L7_TRUE)
  {
    retCode = bspapiValidateImage(backup_filename,L7_TRUE);
    if (retCode != STK_SUCCESS)
    {
      printf("\nValidating the backup image %s failed\n",backup_filename);
      dimImageDelete(backup_filename);
    }
  }
  return STK_SUCCESS;
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
  int index = 0;

  /* If no images are added, return error */

  if (numImages_g == 0)
    return L7_FAILURE;

  /* If no image is marked as active, return error */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].currentState, "active") == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return L7_FAILURE;

  /* copy the file name */

  strcpy(fileName, bootImage_g[index].fileName);
  return L7_SUCCESS;
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
  int index = 0;

  /* If no images are added, return error */

  if (numImages_g == 0)
    return L7_FAILURE;

  /* If no image is marked as active, return error */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].currentState, "backup") == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return L7_FAILURE;

  /* copy the file name */

  strcpy(fileName, bootImage_g[index].fileName);
  return L7_SUCCESS;
}

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
 * @notes    Internal data structures and the boot configuration file
 *           are updated with the information. Image currently  
 *           active is marked as backup and the specified image is 
 *           marked as active. the number of errors for the backup 
 *           image are cleared to 0. 
 *
 * @end
 *********************************************************************/
L7_RC_t dimImageActivate(char *fileName)
{
  int index = 0, def_index = 0;

#ifdef _L7_OS_LINUX_
  char buf[100];
  int rc;
  struct stat st;
#endif

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, fileName) == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return L7_NOT_EXIST;

  /* verify that the image is a backup image */

  /*if(strcmp(bootImage_g[index].currentState, "backup") != 0x0)
    return DIM_INVALID_IMAGE; */

  /* verify that the image is not activated */

  if (strcmp(bootImage_g[index].nextState, "active") == 0x0)
    return L7_ALREADY_CONFIGURED;


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
  }

  /* Activate the current backup image */

  strcpy(bootImage_g[index].numErrors, "0");
  strcpy(bootImage_g[index].nextState, "active");
#ifdef _L7_OS_LINUX_
  /* A script is included in the .stk file to handle any necessary */
  /* hardware specific requirements to activate the new image.     */
  if (stat("/usr/sbin/extimage", &st) == 0)
  {
    snprintf(buf, sizeof(buf), "cd %s ; extimage -i %s -o %sACTIVATE -n 2",
        CONFIG_PATH, bootImage_g[index].fileName, DOWNLOAD_PATH);
    if (WEXITSTATUS(system(buf)) != 0)
      return(L7_FAILURE);

    snprintf(buf, sizeof(buf), "cd %s ; sh ACTIVATE -q %s", DOWNLOAD_PATH,
        bootImage_g[index].fileName);
    rc = WEXITSTATUS(system(buf));
    if ((rc != 0) && (rc != 1))
      return(L7_FAILURE);
  }
#endif

  /* update the current boot configuration file */
  writeBootCfg();
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Adds an image to the list of images 
 *           
 * @param    fileName    @b{(input)}  file for the backup image 
 *
 * @returns  L7_SUCCESS   on successful execution
 * @returns  L7_FAILURE   if unable to add the image
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
L7_RC_t dimImageAdd(char *srcFileName, char *fileName)
{
  int index = 0;

#ifdef _L7_OS_LINUX_
  char buf[100];
  int rc;
  struct stat st;
#endif

  /* If srcFileName == dstFileName, assume the file is already present & */
  /* valid on the file system, otherwise copy to fileName.               */
  if (strcmp(srcFileName, fileName) != 0)
  {
    remove(fileName);
    /* Use copy here since rename fails if files are no on the same volume */
    if (osapiFsCopyFile(srcFileName, fileName) < 0)
       return L7_FAILURE;
  }

  if (L7_FALSE == dimImageExists(fileName))
  {
    return L7_FAILURE;
  }

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

#ifdef _L7_OS_LINUX_
    /* A script is included in the .stk file to handle any necessary */
    /* hardware specific requirements to activate the new image.     */
    if (stat("/usr/sbin/extimage", &st) == 0)
    {
      snprintf(buf, sizeof(buf), "cd %s ; extimage -i %s -o %sACTIVATE -n 2",
          CONFIG_PATH, bootImage_g[index].fileName, DOWNLOAD_PATH);
      if (WEXITSTATUS(system(buf)) != 0)
        return(L7_FAILURE);

      snprintf(buf, sizeof(buf), "cd %s ; sh ACTIVATE -q %s", DOWNLOAD_PATH,
          bootImage_g[index].fileName);
      rc = WEXITSTATUS(system(buf));
      if ((rc != 0) && (rc != 1))
        return(L7_FAILURE);
    }
#endif

    /* update the bootcfg */
    writeBootCfg();
    return L7_SUCCESS;
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
            return(L7_FAILURE);

          snprintf(buf, sizeof(buf), "cd %s ; sh ACTIVATE -q %s", DOWNLOAD_PATH, 
              bootImage_g[index].fileName);
          rc = WEXITSTATUS(system(buf));
          if ((rc != 0) && (rc != 1))
            return(L7_FAILURE);
        }
      }
#endif
      return L7_SUCCESS;
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
      writeBootCfg();
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Deletes an image from the list of boot images 
 *           
 * @param    fileName    @b{(input)}  file containing the image to be 
 *                                deleted from the list 
 *
 * @returns  L7_SUCCESS        on successful execution
 * @returns  L7_NOT_EXIST      if the image could not be found.
 * @returns  L7_REQUEST_DENIED if the specified image is active/activated.
 *
 * @notes    Internal data structures and the boot configuration file
 *           are updated with the information.
 *
 *           current active image is never deleted.
 *           activated backup image is never deleted.
 *           
 *
 * @end
 *********************************************************************/
L7_RC_t dimImageDelete(char *fileName)
{
  int index = 0;

  if ((numImages_g == 0) ||
      (L7_FALSE == dimImageExists(fileName)))
    {
      return L7_NOT_EXIST;
    }


  /* check for the image in the list */

  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, fileName) == 0x0)
      break;
  }

  /* if the image doesn't exist, return error */

  if (index == MAX_BOOT_IMAGES)
    return L7_NOT_EXIST;

  /* if this is the default image, return error */

  if (strcmp(bootImage_g[index].currentState, "active") == 0x0)
    return L7_REQUEST_DENIED;

  /* if this is the default image, return error */

  if (strcmp(bootImage_g[index].nextState, "active") == 0x0)
    return L7_REQUEST_DENIED;

  /* delete the file & update the internal details */
  remove(fileName);

  memset(&bootImage_g[index], 0, sizeof(dimImageInfo_t));

  numImages_g--;


  /* effect this on the flash */
  writeBootCfg();
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Checks if the supplied filename belongs an image
 *           
 * @param    fileName @b{(input)}  File name to be verified
 * @param    answer @b{(output)}   Loction to place the output
 *                                 The following are valid values -
 *                                 STK_IMAGE_DOESNOT_EXIST
 *                                 STK_VALID_IMAGE
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

  *answer = STK_IMAGE_DOESNOT_EXIST;

  if ((numImages_g == 0) ||
      (L7_FALSE == dimImageExists(fileName)))
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
    *answer = STK_VALID_IMAGE;

  return 0;
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
  int index = 0;

  if (numImages_g == 0)
    return L7_FALSE;

  /* check for the image in the list */
  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].currentState, "backup") == 0x0)
      break;
  }

  /* if the image doesn't exist, return error */
  if (index == MAX_BOOT_IMAGES)
  {
    return L7_FALSE;
  }

  if (strcmp(bootImage_g[index].nextState, "active") == 0x0)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
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
 * @purpose  Associates a given text description for an image
 *           
 * @param    image         @b{(input)}  image name
 * @param    descr         @b{(input)}  text to be associated
 * 
 * @returns  L7_SUCCESS   on successful execution
 * @returns  L7_NOT_EXIST if the specified image could not be found
 *
 * @notes    Internal data structures and the boot configuration file
 *           are updated with the information. 
 *
 * @end
 *********************************************************************/
L7_RC_t dimImageDescrSet(char *image, char *descr)
{
  int index = 0;

  /* If there are no images, return error */

  if ((numImages_g == 0) ||
      (L7_FALSE == dimImageExists(image)))
    {
      return L7_NOT_EXIST;
    }

  /* Verify that the image exists. Otherwise return error */
  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, image) == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return L7_NOT_EXIST;

  /* truncate incoming string if it's too long */
  strncpy(bootImage_g[index].descr, descr, sizeof(bootImage_g[index].descr)-1);

  writeBootCfg();
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  retrieves the text description for an image
 *           
 * @param    image         @b{(input)}  image name
 * @param    descr         @b{(input)}  pointer to copy the text to
 * 
 * @returns  L7_SUCCESS    on successful execution
 * @returns  L7_NOT_EXIST  if the specified image could not be found
 *
 * @notes    Internal data structures and the boot configuration file
 *           are updated with the information. 
 *
 * @end
 *********************************************************************/
L7_RC_t dimImageDescrGet(char *image, char *descr)
{
  int index = 0;

  /* If there are no images, return error */
  if ((numImages_g == 0) ||
      (L7_FALSE == dimImageExists(image)))
    {
      return L7_NOT_EXIST;
    }

  /* Verify that the image exists. Otherwise return error */
  for (index = 0; index < MAX_BOOT_IMAGES; index++)
  {
    if (strcmp(bootImage_g[index].fileName, image) == 0x0)
      break;
  }

  if (index == MAX_BOOT_IMAGES)
    return L7_NOT_EXIST;

  strcpy(descr, bootImage_g[index].descr);

  return L7_SUCCESS;
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
static L7_BOOL dimImageExists(char *name)
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


#endif
