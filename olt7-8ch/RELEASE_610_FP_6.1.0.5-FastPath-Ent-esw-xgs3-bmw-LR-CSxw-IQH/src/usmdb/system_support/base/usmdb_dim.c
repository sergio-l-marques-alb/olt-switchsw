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

#include "dim.h"

#include "usmdb_dim_api.h"

#include "osapi.h"

#include "usmdb_cda_api.h"
#include "usmdb_unitmgr_api.h"

/*********************************************************************
* @purpose  Sets the supplied image as the active image for the 
*           subsequent re-boots
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  image to be activated
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
L7_RC_t usmDbImageActivate(L7_uint32 unit, L7_char8 *fileName)
{
    L7_RC_t rc;
    int retVal;
    L7_RC_t rc2 = L7_SUCCESS;

#ifdef  L7_STACKING_PACKAGE

    L7_uint32 unitId, unitMgrId;
    usmDbUnitMgrStatus_t status;

    usmDbUnitMgrNumberGet(&unitId);

    usmDbUnitMgrMgrNumberGet(&unitMgrId);

    if(unit != unitId)
    {
      /* Only unit Managers operate on other nodes */
      
      if(unitId != unitMgrId)
        return L7_NOT_SUPPORTED;
      
      /* If not ALL UNITS, check for existance of unit */
      
      if(unit != L7_USMDB_CDA_CDA_ALL_UNITS)
      {
        if (usmDbUnitMgrUnitStatusGet(unit, &status) != L7_SUCCESS)
          return L7_NOT_EXIST;
      }

      /* effect the operation on all units */

      rc2 = usmdbCdaImageActivate(unit, fileName);

      if (unit != L7_USMDB_CDA_CDA_ALL_UNITS)
        return rc2;

      /* proceed to execute this command on local unit */
    }
    
#endif

    retVal = dimImageActivate(fileName);

    switch(retVal)
    {
      case    DIM_SUCCESS : 
        rc = L7_SUCCESS; 
        break;

      case    DIM_IMAGE_DOESNOT_EXIST:
        rc = L7_NOT_EXIST;
        break;

      case    DIM_INVALID_IMAGE : 
        rc = L7_REQUEST_DENIED; 
        break;
      case    DIM_IMAGE_ACTIVE:
        rc = L7_ALREADY_CONFIGURED;
        break;

      default :
        rc = L7_FAILURE;
        break;
    }

    if(rc != L7_SUCCESS)
      return rc;

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

    (void)dimActiveImageFileNameGet(&activeImageName[0]);

    if(strcmp(activeImageName, fileName) == 0)
        return L7_REQUEST_DENIED;

    rc = dimIsBackupImageActivated();

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
* @notes    This function is a wrapper around dimActiveImageFileNameGet
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

    (void)dimActiveImageFileNameGet(fileName);

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
* @notes    This function is a wrapper around dimActiveImageFileNameGet
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

    if (dimBackupImageFileNameGet(fileName) != 0)
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
* @notes    This function is a wrapper around dimActiveImageFileNameGet
*
* @end
*********************************************************************/
L7_RC_t usmDbActivatedImageNameGet(L7_uint32 unit, L7_char8 *fileName)
{
    int rc;
    
    L7_char8 active[128];
    L7_char8 backup[128];

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

    (void)dimActiveImageFileNameGet(&active[0]);

   
    if (dimBackupImageFileNameGet(&backup[0]) != 0)
    {
        strcpy(backup, "<none>");
    }

    rc = dimIsBackupImageActivated();

    if(rc == DIM_ACTIVATED)
    {
        strcpy(fileName, backup);
    }
    else
    {
        strcpy(fileName, active);
    }

    return L7_SUCCESS;
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
* @notes    This function is a wrapper around dimIsAnImage
*
* @end
*********************************************************************/
L7_RC_t usmDbIsAnImage(L7_uint32 unit, L7_char8 *fileName)
{
    int answer = 0;

    (void) dimIsAnImage(fileName, &answer);

    if(answer == DIM_IMAGE_DOESNOT_EXIST)
        return L7_FALSE;

    return L7_TRUE;
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
* @notes    This function is a wrapper around dimImageDelete
*
* @end
*********************************************************************/
L7_RC_t usmDbImageDelete(L7_uint32 unit, L7_char8 *fileName)
{
    int retVal = 0;
    L7_RC_t rc;

    retVal = dimImageDelete(fileName);

    switch(retVal)
    {
        case    DIM_SUCCESS : 
            rc = L7_SUCCESS; 
            break;
            
        case    DIM_IMAGE_DOESNOT_EXIST:
            rc = L7_NOT_EXIST;
            break;
            
        case    DIM_INVALID_IMAGE : 
            rc = L7_REQUEST_DENIED; 
            break;
        case    DIM_IMAGE_ACTIVE:
            rc = L7_ALREADY_CONFIGURED;
            break;

        default :
            rc = L7_FAILURE;
            break;
    }

    return rc;
}

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

    char fileName[20];

    L7_uint32 unitId, unitMgrId;

#ifdef  L7_STACKING_PACKAGE
    L7_RC_t rc2 = L7_SUCCESS; 
#endif

#ifdef  L7_STACKING_PACKAGE
    usmDbUnitMgrStatus_t status;
#endif

    usmDbUnitMgrNumberGet(&unitId);
    usmDbUnitMgrMgrNumberGet(&unitMgrId);

    usmDbActivatedImageNameGet(unitId, &fileName[0]);
    

#ifdef  L7_STACKING_PACKAGE

    if(unit != unitId)
    {
      /* Only unit Managers operate on other nodes */
      
      if(unitId != unitMgrId)
        return L7_NOT_SUPPORTED;
      
      /* If not ALL UNITS, check for existance of unit */
      
      if(unit != L7_USMDB_CDA_CDA_ALL_UNITS)
      {
        if (usmDbUnitMgrUnitStatusGet(unit, &status) != L7_SUCCESS)
          return L7_NOT_EXIST;
      }

      /* effect the operation on all units */

      rc2 = usmdbCdaBootcodeUpdate(unit);

      if (unit != L7_USMDB_CDA_CDA_ALL_UNITS)
        return rc2;

      /* proceed to execute this command on local unit */
    }
    
#endif    

    retVal = osapiBootCodeUpdate(fileName);

    if(retVal != DIM_SUCCESS)
        return L7_FAILURE;

    return L7_SUCCESS;
}


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
    int rc = 0;

    rc = dimImageDescrSet(image, descr);

    if(rc == DIM_IMAGE_DOESNOT_EXIST)
        return L7_NOT_EXIST;

    return L7_SUCCESS;
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
    int rc = 0;

    rc = dimImageDescrGet(image, descr);

    if(rc == DIM_IMAGE_DOESNOT_EXIST)
        return L7_NOT_EXIST;

    return L7_SUCCESS;
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
    int answer = 0;

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

    (void) dimIsAnImage(image, &answer);

    if(answer == DIM_IMAGE_DOESNOT_EXIST)
        return L7_NOT_EXIST;

    return osapiImageVersionGet(image, version);
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
  return dimImageExists(name);
}

