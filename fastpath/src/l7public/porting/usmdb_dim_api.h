/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename     usmdb_dim_api.h
*
* @purpose      externs for USMDB DIM layer
*
* @component    DIM
*
* @comments     none
*
* @create       03/06/2005
*
* @author       bviswanath
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/
#ifndef USMDB_DIM_API_H
#define USMDB_DIM_API_H

#define RUNNING_IMAGE                   "running_image"
/*********************************************************************
* @purpose  Sets the supplied image as the active image for the 
*           subsequent re-boots
*           
* @param    unit            @b{(input)}  Unit for this operation.           
* @param    fileName        @b{(input)}  image to be activated
* @param    updateBootCode  @b{(input)}  flag to command a boot code update
*
* @returns  L7_SUCCESS          on successful execution
* @returns  L7_NOT_EXIST        if the specified image could not be found
* @returns  L7_REQUEST_DENIED   if the specified image is not the backup image
* @return   L7_ALREADY_CONFIGURED if the image is already activated
*
* @notes    This is a wrapper around the dimImageActivate API
*
* @end
*********************************************************************/
L7_RC_t usmDbImageActivate(L7_uint32 unit, L7_char8 *fileName,
                           L7_BOOL updateBootCode);

/*********************************************************************
* @purpose  Validates the supplied fileName for code downloads.
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  image for download
*
* @returns  L7_SUCCESS          on successful execution
* @returns  L7_REQUEST_DENIED   if the specified image is the active image
* @return   L7_ALREADY_CONFIGURED if the backup image is already activated
*
* @notes    This function checks to see if the supplied image can be
*           downloaded.
*
* @end
*********************************************************************/
L7_RC_t usmDbImageDownloadValidate(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Returns the active image file name
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around dimActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbActiveImageNameGet(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Returns the backup image file name
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around dimActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbBackupImageNameGet(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Returns the activated image file name
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around dimActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbActivatedImageNameGet(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Checks to see if the supplied filename belongs to an Image
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  file name to be verified
*
* @returns  L7_TRUE     If the file name belongs to an image
* @return   L7_FALSE    otherwise
*
* @notes    This function is a wrapper around dimIsAnImage
*
* @end
*********************************************************************/
L7_BOOL usmDbIsAnImage(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Deletes a given Image
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  file to be deleted
*
* @returns  L7_SUCCESS      on successful execution
* @returns  L7_NOT_EXIST    if the specified image could not be found
* @returns  L7_REQUEST_DENIED if the image is currently active
* @returns  L7_ALREADY_CONFIGURED if the image is currently activated
*
* @notes    This function is a wrapper around dimImageDelete
*
* @end
*********************************************************************/
L7_RC_t usmDbImageDelete(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Updates the Bootcode from the currently activated 
*           Image
*           
* @param    unit        @b{(input)}  Unit for this operation.           
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_FAILURE    on error
*
* @notes    This function is a wrapper around osapiBootCodeUpdate
*
* @end
*********************************************************************/
L7_RC_t usmDbBootCodeUpdate(L7_uint32 unit);

/*********************************************************************
* @purpose  Associated text description with a given image
*           
* @param    image        @b{(input)}  image to associate the text
* @param    descr        @b{(input)}  text to be associated
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_NOT_EXIST  if the specified image could not be found
*
* @notes    This function is a wrapper around dimImageDescrSet
*
* @end
*********************************************************************/
L7_RC_t usmDbImageDescrSet(L7_char8 *image, L7_char8 *descr);


/*********************************************************************
* @purpose  Retrieves text description with a given image
*           
* @param    image        @b{(input)}  image to associate the text
* @param    descr        @b{(output)}  text associated
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_NOT_EXIST  if the specified image could not be found
*
* @notes    This function is a wrapper around dimImageDescrGet
*
* @end
*********************************************************************/
L7_RC_t usmDbImageDescrGet(L7_char8 *image, L7_char8 *descr);

/*********************************************************************
* @purpose  Retrieves the version information for a given image
*           
* @param    unit         @b{(input)}  Unit number for this request
* @param    image        @b{(input)}  image to associate the text
* @param    descr        @b{(output)} version string
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_NOT_EXIST  if the specified image could not be found
* @returns  L7_FAILURE    on FS errors
*
* @notes    This function uses osapi API to get the information
*
* @end
*********************************************************************/
L7_RC_t usmDbImageVersionGet(L7_uint32 unit, L7_char8 *image, 
    L7_char8 *version);

/*********************************************************************
* @purpose  Retrieves the boot image version details for a given image
*
* @param    unit         @b{(input)}  Unit number for this request
* @param    image        @b{(input)}  image to associate the text
* @param    descr        @b{(output)} build date and time string
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_NOT_EXIST  if the specified image could not be found
* @returns  L7_FAILURE    on FS errors
*
* @notes    This function uses osapi API to get the information
*
* @end
*********************************************************************/
L7_RC_t usmDbBootImageVersionGet(L7_uint32 unit, L7_char8 *image,
                                 L7_char8 *bootVersion);

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
L7_BOOL usmDbImageExists(char *name);

/*********************************************************************
* @purpose  Returns the image file name for the given ID.
*
* @param    imageId     @b{(input)}  image ID to retrieve file name
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around dimActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbImageFileNameGet(L7_uint32 imageId, L7_char8 *fileName);

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
L7_BOOL usmDbImageFileNameValid(char *fileName, L7_uint32 *imageId);

/*********************************************************************
 * @purpose  Returns the name for the backup image
 *           
 * @param    imageName    @b{(input)}  buffer to copy the image name
 *
 * @returns  none.
 * 
 * @end
 *********************************************************************/
L7_RC_t usmDbBackupNameGet(char *imageName);

/*********************************************************************
 * @purpose  Returns the name for the active image
 *           
 * @param    imageName    @b{(input)}  buffer to copy the image name
 *
 * @returns  none.
 * 
 * @end
 *********************************************************************/
L7_RC_t usmDbActiveNameGet(char *imageName);

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
L7_BOOL usmDbImageNameValid(char *imageName);

#endif /* USMDB_DIM_API_H */
