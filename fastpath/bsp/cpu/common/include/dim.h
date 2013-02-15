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
#include "bspapi.h"
#include "commdefs.h"
#include "cli_web_exports.h"


/* =====================    Macros       ===================== */


#define MAX_BOOTCFG_SIZE                (1024)

#define DIM_MAX_BOOTCFG_LINE_SIZE       L7_CLI_MAX_STRING_LENGTH

#define DIM_MAX_ERROR_SIZE               4 
#define DIM_MAX_STATE_SIZE              14 

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
* @returns  STK_SUCCESS  on successful initialization
* @returns  STK_IMAGE_DOESNOT_EXIST  if file doesn't exist or no 
*																		 images canbe found
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
int dimInitialize(void);

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
L7_RC_t dimActiveImageFileNameGet(char *fileName);

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
L7_RC_t dimBackupImageFileNameGet(char *fileName);

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
L7_RC_t dimImageActivate(char *fileName);

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
L7_RC_t dimImageAdd(char *srcFileName, char *fileName);

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
L7_RC_t dimImageDelete(char *fileName);

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
* @returns  L7_TRUE  if backup image is valid & activated
*           L7_FALSE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL dimIsBackupImageActivated(void);

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
L7_RC_t dimImageDescrSet(char *image, char *descr);

/*********************************************************************
* @purpose  retrieves the text description for an image
*           
* @param    image         @b{(input)}  image name
* @param    descr         @b{(input)}  pointer to copy the text to
* 
* @returns  L7_SUCCESS   on successful execution
* @returns  L7_NOT_EXIST if the specified image could not be found
*
* @notes    Internal data structures and the boot configuration file
*           are updated with the information. 
*
* @end
*********************************************************************/
L7_RC_t dimImageDescrGet(char *image, char *descr);

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

#endif /* INCLUDE_DIM */

