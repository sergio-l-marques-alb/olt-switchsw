/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   cda_api.c
*
* @purpose    API stubs for the Code Distribution Administrator
*
* @component  cda
*
* @comments   
*
* @create     1/23/2004
*
* @author     djohnson
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "cda_api.h"
#include <string.h>
#include "unitmgr_api.h"

/*********************************************************************
* @purpose  Starts code update on one or all units
*
* @param    unit        Unit number or CDA_ALL_UNITS
* @param    fileName    File to be downloaded
* @param    blocking    indicates that the update should block until
*                       completed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends start message to remote unit(s), poll for remote
*           status until complete
*       
* @end
*********************************************************************/
L7_RC_t cdaCodeUpdate(L7_uint32 unit, L7_char8 *srcFileName, L7_char8 *dstFileName, L7_BOOL blocking)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the overall code update status
*
* @param    oper
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
L7_RC_t cdaStatusGet(cdaOperations_t oper, cdaCodeLoadStatus_t *status)
{
  *status = NOT_IN_PROGRESS;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Updates the bootcode on given unit(s)
*
* @param    unit        Unit number or CDA_ALL_UNITS
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends message to remote unit(s), poll for remote
*           status until complete
*       
* @end
*********************************************************************/
L7_RC_t cdaBootcodeUpdate(L7_uint32 unit)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deletes an image on one or all units
*
* @param    unit        Unit number or CDA_ALL_UNITS
* @param    fileName    File to be deleted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends message to remote unit(s), poll for remote
*           status until complete
*       
* @end
*********************************************************************/
L7_RC_t cdaImageDelete(L7_uint32 unit, L7_char8 *fileName)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  activates an image on one or all units
*
* @param    unit        Unit number or CDA_ALL_UNITS
* @param    fileName    image to be activated
* @param    updateBootCode  Flag to command a boot code update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends message to remote unit(s), poll for remote
*           status until complete
*       
* @end
*********************************************************************/
L7_RC_t cdaImageActivate(L7_uint32 unit, L7_char8 *fileName,
                         L7_BOOL updateBootCode)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update Standby CFM with Active CFM code version
*
* @param    unit        unit number of standby CFM
* @param    srcImage    Source Image on standby CFM
* @param    dstImage    Destination Image on Active CFM
*
* @returns  L7_SUCCESS  
*           L7_FAILURE  
*
* @notes
*
* @end
********************************************************************/
L7_RC_t cdaUpdateACFMwithSCFMImage(L7_uint32 unit, L7_char8 *srcImage,
                                   L7_char8 *dstImage)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Updates the kernel on given unit(s)
*
* @param    unit        Unit number or CDA_ALL_UNITS
* @param    srcFileName File to be downloaded
* @param    fileSize    Size of file to be downloaded
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends message to remote unit(s), poll for remote
*           status until complete
*
* @end
*********************************************************************/
L7_RC_t cdaKernelUpdate(L7_uint32 unit, L7_char8 *srcFileName,
                        L7_int32 fileSize)
{
  return L7_SUCCESS;
}



