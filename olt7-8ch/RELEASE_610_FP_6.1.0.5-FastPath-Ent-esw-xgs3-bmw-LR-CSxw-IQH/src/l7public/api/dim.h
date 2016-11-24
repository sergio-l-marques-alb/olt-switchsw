/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
**********************************************************************
*
* @filename     dim.h
*
* @purpose      dual boot image Manager Constants and Data Structures
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

#ifndef INCLUDE_DIM
#define INCLUDE_DIM

/* ===================== Include Headers ===================== */
#include "l7_common.h"
#include "commdefs.h"
#include "cli_web_exports.h"

/* =====================    Macros       ===================== */


#define DIM_CFG_FILENAME                "boot.dim"  

#define MAX_FAILED_BOOT_TRIES           3

#define MAX_BOOT_IMAGES                 2

#define MAX_BOOTCFG_SIZE                (1024)

#define DIM_MAX_BOOTCFG_LINE_SIZE       L7_CLI_MAX_STRING_LENGTH

#define DIM_MAX_FILENAME_SIZE           39 
#define DIM_MAX_ERROR_SIZE               4 
#define DIM_MAX_STATE_SIZE              14 

#define DIM_IMAGE1_NAME	       	        "image1"

#define DIM_IMAGE2_NAME			"image2"
#define DIM_ACTIVE_IMAGE		"active"
#define DIM_BACKUP_IMAGE		"backup"
#define RUNNING_IMAGE         	"running_image"

#define ACTIVE_CFG_FILE                "/usr/local/ptin/sbin/active.cfg"
#define CFG_FILE_NAME_LEN              (sizeof(DIM_IMAGE1_NAME) - 1)

#ifdef _L7_OS_ECOS_
#define ACTIVATE_IMAGE1 "boot -z -elf flash0.os:" 
#define ACTIVATE_IMAGE2 "boot -z -elf flash0.os1:"
#endif
/* =====================    enumerations  ===================== */

typedef enum
{

  DIM_SUCCESS = 0,
  DIM_FAILURE,
  DIM_VALID_IMAGE,
  DIM_IMAGE_DOESNOT_EXIST,
  DIM_NOT_ACTIVATED,
  DIM_ACTIVATED,
  DIM_TABLE_IS_FULL,
  DIM_IMAGE_ACTIVE,
  DIM_INVALID_IMAGE

} DIM_RC_t;


/* =====================    Data Types    ===================== */ 

/* boot Image descriptor. Each Image is described as below */

typedef struct _dim_image
{
    /* Mandatory information */
    
    char    currentState[DIM_MAX_STATE_SIZE+1];   /* active / backup */
    char    nextState[DIM_MAX_STATE_SIZE+1];      /* active / backup */
    char    fileName[DIM_MAX_FILENAME_SIZE+1];    /* on the fs */
    char    numErrors[DIM_MAX_ERROR_SIZE+1];      /* Num of tries w/ errors    */
    char    descr[DIM_MAX_BOOTCFG_LINE_SIZE];   /* text description */

} dimImageInfo_t;


/* ===================== Function prototypes ===================== */ 

/*********************************************************************
* @purpose  Initializes the dual boot image manager
*
* @param    none
*
* @returns  DIM_SUCCESS  on successful initialization
* @returns  DIM_IMAGE_DOESNOT_EXIST  if file doesn't exist or no 
*																		 images canbe found
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
int dimInitialize(void);

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
int dimActiveImageErrorsGet(int *pErrors);

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
int dimActiveImageErrorsSet( int errors);

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
int dimActiveImageFileNameGet(char *fileName);

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
int dimBackupImageFileNameGet(char *fileName);

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
int dimBackupImageNameGet(char *fileName);

/*********************************************************************
* @purpose  Sets the supplied image as the active image for the 
*           subsequent re-boots
*           
* @param    fileName    @b{(input)}  image to be activated
*
* @returns  0   on successful execution
* @returns  -1  if the specified image could not be found
* @returns  -2  if the specified image is not the backup image
* @returns  -3  if the specified image is already activated.
*
* @notes    Internal data structures and the boot configuration file
*           are updated with the information. Image currently  
*           active is marked as backup and the specified image is 
*           marked as active. the number of errors for the backup 
*           image are cleared to 0. 
*
* @end
*********************************************************************/
int dimImageActivate(char *fileName);


/*********************************************************************
* @purpose  Adds an image to the list of images 
*           
* @param    fileName    @b{(input)}  file for the backup image 
*
* @returns  0   on successful execution
* @returns  -1  if the allowed max number of images reached already
* 
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
int dimImageAdd(char *fileName);

/*********************************************************************
* @purpose  Deletes an image from the list of boot images 
*           
* @param    fileName    @b{(input)}  file containing the image to be 
*                                deleted from the list 
*
* @returns  0   on successful execution
* @returns  -1  if the specified file could not be found.
* @returns  -2  if the specified image is currently active.
* @returns  -3  if the specified backup image is activated.
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
int dimImageDelete(char *fileName);

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
int dimNumImagesGet(int *pNum);

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
int dimImagesInfoGet(char *pImage);

/*********************************************************************
* @purpose  Checks if the supplied filename belongs an image
*           
* @param    fileName @b{(input)}  File name to be verified
* @param    answer @b{(output)}   Loction to place the output
*                                 The following are valid values -
*                                 0 - file is not am image
*                                 1 - file is an image
*
* @returns  0   on success
*           -1  if there are no images
* 
* @notes    
*
* @end
*********************************************************************/
int dimIsAnImage(char *fileName, int *answer);

/*********************************************************************
* @purpose  Checks if the backup image is activated 
*           
* @param    none
*
* @returns  0 - backup image is activated
*           2 - No images present
*           1 - backup image is not activated
*           3 - backup image doesn't exist
*
* @notes    
*
* @end
*********************************************************************/
DIM_RC_t dimIsBackupImageActivated(void);

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
int dimImageRename(char *curFileName, char *newFileName);


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
int dimImageDescrSet(char *image, char *descr);

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
int dimImageDescrGet(char *image, char *descr);

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
int dimShowBootCfg(void);

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
L7_BOOL dimImageExists(char *name);

#endif /* INCLUDE_DIM */

