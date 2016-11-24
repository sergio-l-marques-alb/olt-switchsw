/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename     usmdb_file.c
*
* @purpose      Provide interface to File System on the Flash for the
*               MGMT components
*
* @component    Osapi
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

#include "osapi.h"

#include "dim.h"

#include "usmdb_dim_api.h"

#include "usmdb_sim_api.h"

#include "usmdb_file_api.h"

#include "sysapi.h"

#include "usmdb_cda_api.h"
#include "usmdb_unitmgr_api.h"

/*********************************************************************
* @purpose  Renames a given file on the File system
*
* @param    unit            @b{(input)}  Unit for this operation.
* @param    curFileName     @b{(input)}  image to be renamed
* @param    newFileName     @b{(input)}  new name for the image
*
* @returns  L7_SUCCESS      on successful execution
* @returns  L7_NOT_EXIST    if the specified file could not be found
*
* @notes    This function uses the osapi and the dim components to
*           rename the given file.
*
* @end
*********************************************************************/
L7_RC_t usmDbFileRename(L7_uint32 unit, L7_char8 *curFileName,
                        L7_char8 *newFileName)
{
  L7_RC_t rc;

  rc = usmDbFileCopy(unit, curFileName, newFileName, L7_TRUE);
  if(rc == L7_SUCCESS)
  {
    rc = usmDbFileDelete(unit, curFileName);
  }

  return rc;
}

/*********************************************************************
* @purpose  Deletes a given file from the File system
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  file to be deleted
*
* @returns  L7_SUCCESS      on successful execution
* @returns  L7_NOT_EXIST    if the specified file/unit could not be found
* @returns  L7_REQUEST_DENIED if the file could not be deleted
* @return   L7_NOT_SUPPORTED  specified operation is not supported
*
* @notes    This function uses the osapi and the dim components to
*           delete the given file.
*
* @end
*********************************************************************/
L7_RC_t usmDbFileDelete(L7_uint32 unit, L7_char8 *fileName)
{
    L7_RC_t rc;
    L7_RC_t rc2 = L7_SUCCESS;
    L7_uint32 unitId, unitMgrId;
    
#ifdef  L7_STACKING_PACKAGE    
   usmDbUnitMgrStatus_t status;
#endif
   
   usmDbUnitMgrNumberGet(&unitId);

   usmDbUnitMgrMgrNumberGet(&unitMgrId);

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

      rc2 = usmdbCdaImageDelete(unit, fileName);

      if (unit != L7_USMDB_CDA_CDA_ALL_UNITS)
        return rc2;

      /* proceed to execute this command on local unit */
    }
    
#endif


    if ( 0 == strcmp(fileName,DIM_BACKUP_IMAGE))
    {
       if (L7_SUCCESS != dimBackupImageNameGet(fileName))
       {
         return L7_ERROR;
       }
     }

    /* If the specified file is an Image, inform the DIM component */
    if(usmDbIsAnImage(unit, fileName) == L7_TRUE)
    {
      rc = usmDbImageDelete(unit, fileName);

      if(rc != L7_SUCCESS)
      {
        /* round off the error.
         * here, the active/activated image are the same, as 
         * far as errors is concerned
         */ 
        if(rc == L7_ALREADY_CONFIGURED)
          rc = L7_REQUEST_DENIED;

        return rc;
      }
    }

    /* effect this on the file system */

    if(osapiFsDeleteFile(fileName) != L7_SUCCESS)
        return L7_NOT_EXIST;

    return (rc2);
}

/*********************************************************************
* @purpose  copies a given local file locally or remotely
*           
* @param    unit        @b{(input)}  Unit for this operation. 
* @param    srcFileName @b{(input)}  source file name
* @param    dstFileName @b{(input)}  target file name
* @param    blocking    @b{(input)}  If TRUE, wait for copy to finish
*
* @returns  L7_SUCCESS      on successful execution
* @returns  L7_NOT_EXIST    if the specified file/unit could not be found
* @returns  L7_REQUEST_DENIED if the file could not be deleted
* @return   L7_NOT_SUPPORTED  specified operation is not supported
*
* @notes    This function uses the osapi & cda components to
*           copy the given file.
*
* @end
*********************************************************************/
L7_RC_t usmDbFileCopy(L7_uint32 unit, L7_char8 *srcFileName,
    L7_char8 *dstFileName, L7_BOOL blocking)
{
  L7_RC_t rc  =  L7_NOT_EXIST;
  L7_RC_t rc2 = L7_NOT_EXIST;
  L7_uint32 unitId, unitMgrId;
  L7_char8  tmpFileName[DIM_MAX_FILENAME_SIZE + 1];

#ifdef  L7_STACKING_PACKAGE    
  usmDbUnitMgrStatus_t status;
#endif

  usmDbUnitMgrNumberGet(&unitId);

  usmDbUnitMgrMgrNumberGet(&unitMgrId);

  /* first check to see if we have to do a local operation */

  if (unit == unitId)
  {
    /* local copy operation   */

    result_string("Copying image .. "); 

    if ( 0 == strcmp(dstFileName,DIM_BACKUP_IMAGE))
    {
      if (L7_SUCCESS != dimBackupImageNameGet(tmpFileName))
      {
        return rc;
      }
    }
    else if ( 0 == strcmp(dstFileName,DIM_ACTIVE_IMAGE))
    {
      if (L7_SUCCESS != dimActiveImageFileNameGet(tmpFileName))
      {
        return rc;
      }
    }
    else
    {
      OSAPI_STRNCPY_SAFE(tmpFileName, dstFileName);
    } 

    if ( L7_TRUE == blocking )
    {
      rc = osapiFsCopyFile(srcFileName, tmpFileName);

      if (rc == L7_SUCCESS)
      {
        result_string("copy operation successful ");
        rc = dimImageAdd(tmpFileName);

        if (rc == DIM_SUCCESS)
        {
          return rc;
        }
      }
    }
    else
    {
      usmDbTransferFilePathLocalSet(unit, "");
      usmDbTransferFileNameLocalSet(unit, srcFileName);
      usmDbTransferFilePathRemoteSet(unit, "");
      usmDbTransferFileNameRemoteSet(unit, tmpFileName);
      usmDbTransferModeSet(unit, L7_TRANSFER_LOCAL);
      rc = usmDbTransferDownStartSet(unit); /* Direction has no meaning for local copy */
      if ( L7_SUCCESS == rc )
      {
        dimImageAdd(tmpFileName);
        return rc;
      }
    }

    result_string("copy operation failed "); 
    return rc;
  }

#ifdef  L7_STACKING_PACKAGE 

  /* Only unit Managers operate on other nodes */

  result_string("Distributing code to other node(s) .. "); 
  
  if(unitId != unitMgrId)
    return L7_NOT_SUPPORTED;

  /* If not ALL UNITS, check for existance of unit */

  if(unit != L7_USMDB_CDA_CDA_ALL_UNITS)
  {
    if (usmDbUnitMgrUnitStatusGet(unit, &status) != L7_SUCCESS)
      return L7_NOT_EXIST;
  }

  /* effect the operation on all units */

  rc2 = usmdbCdaCodeUpdate(unit, srcFileName, dstFileName);

  if(rc2 == L7_SUCCESS)
  {
    result_string("copy operation successful "); 
  }
  else
  {
    result_string("copy operation failed "); 
  }

#endif

  return rc2;  
}

/*********************************************************************
* @purpose  Displays list of files on FLASH file system.
*
* @param    unitIndex        @b{(input)}  Unit for this operation.
*
* @returns  L7_SUCCESS      on successful execution
* @returns  L7_FAILURE      on fs errors
*
* @notes    This function uses the osapi components to display the files
*           in FLASH file system.
*
* @end
*********************************************************************/
L7_RC_t usmDbDirList(L7_FILE_COMPACT_t *fileList, L7_uint32 *fileEntries)
{
  return osapiFsListCompact(fileList, fileEntries);
}
