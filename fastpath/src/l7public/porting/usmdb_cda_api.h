/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename usmdb_cda_api.h
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

#ifndef USMDB_CDA_API_H
#define USMDB_CDA_API_H

#include "cda_api.h"

#define L7_USMDB_CDA_CDA_ALL_UNITS CDA_ALL_UNITS

typedef enum
{
  L7_USMDB_CDA_IN_PROGRESS = 1,
  L7_USMDB_CDA_NOT_IN_PROGRESS,
  L7_USMDB_CDA_FINISHED_WITH_SUCCESS,
  L7_USMDB_CDA_FINISHED_WITH_ERROR
} usmDbCdaCodeLoadStatus_t;

/*********************************************************************
* @purpose  Starts code update on one or all units
*
* @param    unit        Unit number or L7_USMDB_CDA_CDA_ALL_UNITS
* @param    image       Image to be downloaded
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends start message to remote unit(s)
*       
* @end
*********************************************************************/
L7_RC_t usmdbCdaCodeUpdate(L7_uint32 unit, L7_char8 *srcImage, L7_char8 *dstImage);

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
L7_RC_t usmdbCdaCodeUpdateNoBlock(L7_uint32 unit, L7_char8 *srcImage, L7_char8 *dstImage);

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
L7_RC_t usmdbCdaImageDelete(L7_uint32 unit, L7_char8 *image);

/*********************************************************************
* @purpose Activates an image on one or all units
*
* @param    unit           Unit number or L7_USMDB_CDA_CDA_ALL_UNITS
* @param    image          Image to be activated
* @param    updateBootCode Flag to command a boot code update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends start message to remote unit(s)
*       
* @end
*********************************************************************/
L7_RC_t usmdbCdaImageActivate(L7_uint32 unit, L7_char8 *image,
                              L7_BOOL updateBootCode);

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
L7_RC_t usmdbCdaBootcodeUpdate(L7_uint32 unit);

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
                                usmDbCdaCodeLoadStatus_t *status);

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
L7_RC_t usmDbCdaUpdateACFMwithSCFMImage(L7_uint32 unit,L7_char8 *srcImage, L7_char8 *dstImage);

/*********************************************************************
* @purpose  Starts kernel update on one or all units
*
* @param    unit        Unit number or L7_USMDB_CDA_CDA_ALL_UNITS
* @param    image       Image to be downloaded
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends start message to remote unit(s)
*       
* @end
*********************************************************************/
L7_RC_t usmdbCdaKernelUpdate(L7_uint32 unit, L7_char8 *srcImage,
                             L7_int32 fileSize);

#endif

