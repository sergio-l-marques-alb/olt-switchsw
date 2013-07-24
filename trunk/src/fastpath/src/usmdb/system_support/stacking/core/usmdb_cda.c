/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename usmdb_cda.c
*
* @purpose USMDB Code Distribution Administrator
*
* @component unitmgr
*
* @comments none
*
* @create 10/23/2003
*
* @author djohnson
*
* @end
*             
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "usmdb_cda_api.h"
#include "cda_api.h"
#include "usmdb_unitmgr_api.h"
#include "osapi.h"
#include "bspapi.h"

#include <stdio.h>
/*********************************************************************
* @purpose  Starts code update on one or all units
*
* @param    unit        Unit number or L7_USMDB_CDA_CDA_ALL_UNITS
* @param    srcImage    Image to be copied
* @param    dstImage    Image to be updated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends start message to remote unit(s)
*       
* @end
*********************************************************************/
L7_RC_t usmdbCdaCodeUpdate(L7_uint32 unit, L7_char8 *srcImage, L7_char8 *dstImage)
{
  return cdaCodeUpdate(unit, srcImage, dstImage, L7_TRUE);  
}

/*********************************************************************
* @purpose  Starts code update on one or all units but doesn't block
*
* @param    unit        Unit number or L7_USMDB_CDA_CDA_ALL_UNITS
* @param    srcImage    Image to be copied
* @param    dstImage    Image to be updated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends start message to remote unit(s)
*       
* @end
*********************************************************************/
L7_RC_t usmdbCdaCodeUpdateNoBlock(L7_uint32 unit, L7_char8 *srcImage, L7_char8 *dstImage)
{
  return cdaCodeUpdate(unit, srcImage, dstImage, L7_FALSE);  
}

/*********************************************************************
* @purpose  Gets the overall code update status
*
* @param    operation       
* @param    status       
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, device not management or doesn't support code
*                       transfer function
*
* @notes    Examines list of remote unit statuses to determine the
*           overall update status
*       
* @end
*********************************************************************/
L7_RC_t usmdbCdaUpdateStatusGet(cdaOperations_t operation,
                                usmDbCdaCodeLoadStatus_t *status)
{
  L7_RC_t rc;
  cdaCodeLoadStatus_t cdaStatus;

  rc = cdaStatusGet(operation, &cdaStatus);
  switch (cdaStatus)
  {
  case IN_PROGRESS:
    *status = L7_USMDB_CDA_IN_PROGRESS;
    break;
  case NOT_IN_PROGRESS:
    *status = L7_USMDB_CDA_NOT_IN_PROGRESS;
    break;
  case FINISHED_WITH_SUCCESS:
    *status = L7_USMDB_CDA_FINISHED_WITH_SUCCESS;
    break;
  case FINISHED_WITH_ERROR:
    *status = L7_USMDB_CDA_FINISHED_WITH_ERROR;
    break;
  default:
    rc = L7_FAILURE;
  }
  
  return rc;
}

/*********************************************************************
* @purpose Deletes an image on one or all units
*
* @param    unit        Unit number or L7_USMDB_CDA_CDA_ALL_UNITS
* @param    image       Image to be deleted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends start message to remote unit(s)
*       
* @end
*********************************************************************/
L7_RC_t usmdbCdaImageDelete(L7_uint32 unit, L7_char8 *image)
{
  return cdaImageDelete(unit, image);
}

/*********************************************************************
* @purpose Activates an image on one or all units
*
* @param    unit            Unit number or L7_USMDB_CDA_CDA_ALL_UNITS
* @param    image           Image to be activated
* @param    updateBootCode  Flag to command a boot code update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends start message to remote unit(s)
*       
* @end
*********************************************************************/
L7_RC_t usmdbCdaImageActivate(L7_uint32 unit, L7_char8 *image,
                              L7_BOOL updateBootCode)
{
  return cdaImageActivate(unit, image, updateBootCode);
}

/*********************************************************************
* @purpose updates the bootcode on one or all units
*
* @param    unit        Unit number or L7_USMDB_CDA_CDA_ALL_UNITS
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends start message to remote unit(s)
*       
* @end
*********************************************************************/
L7_RC_t usmdbCdaBootcodeUpdate(L7_uint32 unit)
{
  return cdaBootcodeUpdate(unit);
}

/*********************************************************************
* @purpose  Starts kernel update on one or all units
*
* @param    unit        Unit number or L7_USMDB_CDA_CDA_ALL_UNITS
* @param    srcImage    Image to be copied
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends start message to remote unit(s)
*       
* @end
*********************************************************************/
L7_RC_t usmdbCdaKernelUpdate(L7_uint32 unit, L7_char8 *srcImage,
                             L7_int32 fileSize)
{
  return cdaKernelUpdate(unit, srcImage, fileSize);  
}

