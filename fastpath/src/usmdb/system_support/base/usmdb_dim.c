/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename     usmdb_dim.c
*
* @purpose      Provide interface to DIM API's 
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

#include <string.h>

#include "l7_common.h"

#include "bspapi.h"

#include "usmdb_dim_api.h"

#include "osapi.h"

#include "usmdb_cda_api.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"


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
* @notes    This is a wrapper around the bspapiImageActivate API
*
* @end
*********************************************************************/
L7_RC_t usmDbImageActivate(L7_uint32 unit, L7_char8 *fileName,
                           L7_BOOL updateBootCode)
{
  L7_RC_t retVal;
  L7_RC_t rc2 = L7_SUCCESS;
#ifdef  L7_STACKING_PACKAGE
  L7_uint32 unitId, unitMgrId;
  usmDbUnitMgrStatus_t status;
#endif

  if (usmDbImageSemaTake(L7_NO_WAIT)!= L7_SUCCESS)
  {
    return L7_IMAGE_IN_USE;
  }
#ifdef L7_STACKING_PACKAGE
  usmDbUnitMgrNumberGet(&unitId);
  usmDbUnitMgrMgrNumberGet(&unitMgrId);

  if(unit != unitId)
  {
    /* Only unit Managers operate on other nodes */
    if(unitId != unitMgrId)
    {
      usmDbImageSemaGive();
      return L7_NOT_SUPPORTED;
    }
    /* If not ALL UNITS, check for existance of unit */
      
    if(unit != L7_USMDB_CDA_CDA_ALL_UNITS)
    {
      if (usmDbUnitMgrUnitStatusGet(unit, &status) != L7_SUCCESS)
      {
        usmDbImageSemaGive();
        return L7_NOT_EXIST;
      }  
    }
    /* effect the operation on all units */
    rc2 = usmdbCdaImageActivate(unit, fileName, updateBootCode);

    if (unit != L7_USMDB_CDA_CDA_ALL_UNITS)
    {
      usmDbImageSemaGive();
      return rc2;
    } 
    /* proceed to execute this command on local unit */
  }
#endif
  retVal = bspapiImageActivate(fileName, updateBootCode);
  usmDbImageSemaGive(); 
   
  if (retVal != L7_SUCCESS)
  {
    return retVal;
  }

  return rc2;
}
            
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
L7_RC_t usmDbImageDownloadValidate(L7_uint32 unit, L7_char8 *fileName)
{

#if 0  
    L7_char8 activeImageName[20];
    int rc = 0;
    
    /* on image downloads, check that
     * 1. the active image is not being overwritten
     * 2. the backup image, if any is not activated
     **/

    (void)bspapiActiveImageFileNameGet(&activeImageName[0]);

    if(strcmp(activeImageName, fileName) == 0)
        return L7_REQUEST_DENIED;

    rc = bspapiIsBackupImageActivated();

    if(rc == DIM_ACTIVATED)
        return L7_ALREADY_CONFIGURED;
#endif
    
    return L7_SUCCESS;
}

            
/*********************************************************************
* @purpose  Returns the active image file name
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around bspapiActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbActiveImageNameGet(L7_uint32 unit, L7_char8 *fileName)
{

#ifdef  L7_STACKING_PACKAGE

    L7_uint32 unitId, unitMgrId;

    usmDbUnitMgrNumberGet(&unitId);

    usmDbUnitMgrMgrNumberGet(&unitMgrId);

    if(unit != unitId)
    {
      /* Only unit Managers operate on other nodes */
      
      if(unitId != unitMgrId)
        return L7_NOT_SUPPORTED;
      
      if(unit == L7_USMDB_CDA_CDA_ALL_UNITS)
           return L7_NOT_SUPPORTED;

      /* effect the operation on all units */

      return usmDbUnitMgrActiveImageNameGet(unit, fileName);

    }
    
#endif

    (void)bspapiActiveImageFileNameGet(fileName);

    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Returns the backup image file name
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around bspapiActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbBackupImageNameGet(L7_uint32 unit, L7_char8 *fileName)
{

#ifdef  L7_STACKING_PACKAGE

    L7_uint32 unitId, unitMgrId;

    usmDbUnitMgrNumberGet(&unitId);

    usmDbUnitMgrMgrNumberGet(&unitMgrId);

    if(unit != unitId)
    {
      /* Only unit Managers operate on other nodes */
      
      if(unitId != unitMgrId)
        return L7_NOT_SUPPORTED;
      
      if(unit == L7_USMDB_CDA_CDA_ALL_UNITS)
           return L7_NOT_SUPPORTED;

      /* effect the operation on all units */

      return usmDbUnitMgrBackupImageNameGet(unit, fileName);

    }
    
#endif 

    if (bspapiBackupImageFileNameGet(fileName) != 0)
    {
        strcpy(fileName, "<none>");
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns the activated image file name
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @notes    This function is a wrapper around bspapiActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbActivatedImageNameGet(L7_uint32 unit, L7_char8 *fileName)
{

#ifdef  L7_STACKING_PACKAGE

    L7_uint32 unitId, unitMgrId;

    usmDbUnitMgrNumberGet(&unitId);

    usmDbUnitMgrMgrNumberGet(&unitMgrId);

    if(unit != unitId)
    {
      /* Only unit Managers operate on other nodes */
      
      if(unitId != unitMgrId)
        return L7_NOT_SUPPORTED;
      
      if(unit == L7_USMDB_CDA_CDA_ALL_UNITS)
           return L7_NOT_SUPPORTED;

      /* effect the operation on all units */

      return usmDbUnitMgrActivatedImageNameGet(unit, fileName);

    }
    
#endif     
    if (bspapiIsBackupImageActivated() == L7_TRUE)
    {
      return (bspapiBackupImageFileNameGet(fileName));
    }
    return (bspapiActiveImageFileNameGet(fileName));
}

/*********************************************************************
* @purpose  Checks to see if the supplied filename belongs to an Image
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  file name to be verified
*
* @returns  L7_TRUE     If the file name belongs to an image
* @return   L7_FALSE    otherwise
*
* @notes    This function is a wrapper around bspapiIsAnImage
*
* @end
*********************************************************************/
L7_BOOL usmDbIsAnImage(L7_uint32 unit, L7_char8 *fileName)
{
  return bspapiImageExists(fileName);
}

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
* @notes    This function is a wrapper around bspapiImageDelete
*
* @end
*********************************************************************/
L7_RC_t usmDbImageDelete(L7_uint32 unit, L7_char8 *fileName)
{
  L7_RC_t rc;

  if (usmDbImageSemaTake(L7_NO_WAIT)!= L7_SUCCESS)
  {
    return L7_IMAGE_IN_USE;
  }
  rc = bspapiImageDelete(fileName);
  usmDbImageSemaGive();

  return rc;
}

#if L7_FEAT_BOOTCODE_UPDATE
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
L7_RC_t usmDbBootCodeUpdate(L7_uint32 unit)
{
  int retVal;
  char fileName[DIM_MAX_FILENAME_SIZE];
  L7_uint32 unitId, unitMgrId;
#ifdef  L7_STACKING_PACKAGE
  L7_RC_t rc2 = L7_SUCCESS; 
  usmDbUnitMgrStatus_t status;
#endif

  if (usmDbImageSemaTake(L7_NO_WAIT)!= L7_SUCCESS)
  {
    return L7_IMAGE_IN_USE;
  }
  usmDbUnitMgrNumberGet(&unitId);
  usmDbUnitMgrMgrNumberGet(&unitMgrId);

  if (usmDbActivatedImageNameGet(unitId, fileName) != L7_SUCCESS)
    return L7_FAILURE;

#ifdef  L7_STACKING_PACKAGE
  if (unit != unitId)
  {
    /* Only unit Managers operate on other nodes */
    if (unitId != unitMgrId)
    {
      usmDbImageSemaGive();
      return L7_NOT_SUPPORTED;
    }  
    /* If not ALL UNITS, check for existance of unit */
    if (unit != L7_USMDB_CDA_CDA_ALL_UNITS)
    {
      if (usmDbUnitMgrUnitStatusGet(unit, &status) != L7_SUCCESS)
      {
        usmDbImageSemaGive();
        return L7_NOT_EXIST;
      }  
    }

    /* effect the operation on all units */
    rc2 = usmdbCdaBootcodeUpdate(unit);

    if (unit != L7_USMDB_CDA_CDA_ALL_UNITS)
    {
      usmDbImageSemaGive();
      return rc2;
    }  
    /* proceed to execute this command on local unit */
  }
#endif    

  retVal = bspapiBootCodeUpdate(fileName);
  usmDbImageSemaGive();
    
  return retVal;
}
#endif


/*********************************************************************
* @purpose  Associated text description with a given image
*           
* @param    image        @b{(input)}  image to associate the text
* @param    descr        @b{(input)}  text to be associated
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_NOT_EXIST  if the specified image could not be found
*
* @notes    This function is a wrapper around osapiBootCodeUpdate
*
* @end
*********************************************************************/
L7_RC_t usmDbImageDescrSet(L7_char8 *image, L7_char8 *descr)
{
  L7_RC_t rc;
    
  if (usmDbImageSemaTake(L7_NO_WAIT)!= L7_SUCCESS)
  {
    return L7_IMAGE_IN_USE;
  }
  rc = bspapiImageDescrSet(image, descr);
  usmDbImageSemaGive();
   
  return rc;
}


/*********************************************************************
* @purpose  Retrieves text description with a given image
*           
* @param    image        @b{(input)}  image to associate the text
* @param    descr        @b{(output)}  text associated
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_NOT_EXIST  if the specified image could not be found
*
* @notes    This function is a wrapper around osapiBootCodeUpdate
*
* @end
*********************************************************************/
L7_RC_t usmDbImageDescrGet(L7_char8 *image, L7_char8 *descr)
{
    return bspapiImageDescrGet(image, descr);
}

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
    L7_char8 *version)
{
   L7_RC_t rc;
   codeVersion_t code_version;

#ifdef  L7_STACKING_PACKAGE

    L7_uint32 unitId, unitMgrId;

    usmDbUnitMgrNumberGet(&unitId);

    usmDbUnitMgrMgrNumberGet(&unitMgrId);

    if(unit != unitId)
    {
      /* Only unit Managers operate on other nodes */
      
      if(unitId != unitMgrId)
        return L7_NOT_SUPPORTED;
      
      if(unit == L7_USMDB_CDA_CDA_ALL_UNITS)
      {
        return L7_NOT_SUPPORTED;
      }

      /* Get the image version for the target */

      return usmDbUnitMgrImageVersionGet(unit, image, version);

    }
    
#endif        

    if (bspapiImageExists(image) != L7_TRUE)
        return L7_NOT_EXIST;
    
    rc = bspapiImageVersionGet(image,&code_version);
    if (rc != L7_SUCCESS)
    {
      return rc;
    }
    return usmDbImageVersionToString(code_version,version);      
}

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
                                 L7_char8 *bootVersion)
{
  L7_uint32 unitId, unitMgrId;

  usmDbUnitMgrNumberGet(&unitId);

  usmDbUnitMgrMgrNumberGet(&unitMgrId);

  if (unitId != unitMgrId)
    return L7_NOT_SUPPORTED;

  if (unit == L7_USMDB_CDA_CDA_ALL_UNITS)
  {
    return L7_NOT_SUPPORTED;
  }

  /* Get the boot image version for the target */

  return usmDbUnitMgrBootImageVersionGet(unit, image, bootVersion);
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
L7_BOOL usmDbImageExists(char *name)
{
  return bspapiImageExists(name);
}

/*********************************************************************
* @purpose  Retrieves the file name for a given image ID
*           
* @param    imageId      @b{(input)}  image to retrieve filename
* @param    fileName     @b{(output)} returned file name
*
* @returns  L7_SUCCESS    on successful execution
* @returns  L7_NOT_EXIST  if the specified image could not be found
*
* @end
*********************************************************************/
L7_RC_t usmDbImageFileNameGet(L7_uint32 imageId, L7_char8 *fileName)
{
  return bspapiImageFileNameGet(imageId, fileName);
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
L7_BOOL usmDbImageFileNameValid(char *fileName, L7_uint32 *imageId)
{
  return bspapiImageFileNameValid(fileName, imageId);
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
L7_RC_t usmDbBackupNameGet(char *imageName)
{
  return bspapiBackupImageNameGet(imageName);
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
L7_RC_t usmDbActiveNameGet(char *imageName)
{
  return bspapiActiveImageNameGet(imageName);
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
L7_BOOL usmDbImageNameValid(char *imageName)
{
  return bspapiImageNameValid(imageName);
}
