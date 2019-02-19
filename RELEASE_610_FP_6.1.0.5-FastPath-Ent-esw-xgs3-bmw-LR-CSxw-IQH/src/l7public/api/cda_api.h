/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   cda_api.h
*
* @purpose    Component APIs for the Code Distribution Administrator
*
* @component  cda
*
* @comments   
*
* @create     9/19/2003
*
* @author     djohnson
* @end
*
**********************************************************************/

#ifndef CDA_API_H
#define CDA_API_H

#include "l7_common.h"

#define CDA_ALL_UNITS L7_MAX_UNITS_PER_STACK+1
#define CDA_TIME_SIZE   32

typedef enum
{
  START_DOWNLOAD = 1,
  DELETE_IMAGE,
  ACTIVATE_IMAGE,
  UPDATE_BOOTCODE,
  UPDATE_KERNEL,
  CDA_MAX_OPERATION
} cdaOperations_t;


typedef enum
{
  IN_PROGRESS = 1,
  NOT_IN_PROGRESS,
  FINISHED_WITH_SUCCESS,
  FINISHED_WITH_ERROR
} cdaCodeLoadStatus_t;

/* Do not add any more fields in this structure. It
** breaks backwards compatibility for CDA.
*/
typedef struct
{
  L7_BOOL   valid;
  L7_uchar8 rel;
  L7_uchar8 ver;
  L7_uchar8 maint_level;
  L7_uchar8 build_num;

#if L7_FEAT_STACKING_K_COMPAT
  L7_uchar8 bootTimestamp[CDA_TIME_SIZE];
#endif

} codeVersion_t;

/*********************************************************************
* @purpose  Starts code update on one or all units
*
* @param    unit        Unit number or CDA_ALL_UNITS
* @param    srcFileName File to be downloaded
* @param    dstFileName File to be written
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
L7_RC_t cdaCodeUpdate(L7_uint32 unit, L7_char8 *srcFileName, L7_char8 *dstFileName, L7_BOOL blocking);

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
* @notes    Examines list of remote unit statuses
*       
* @end
*********************************************************************/
L7_RC_t cdaStatusGet(cdaOperations_t oper, cdaCodeLoadStatus_t *status);

/*********************************************************************
* @purpose  Examines the STK file
*
* @param    image
* @param    crc check required or not
*
* @returns
*
* @notes    Tells unit manager to update the detected code version
*
* @end
********************************************************************/
L7_RC_t cdaUnpackSTK(L7_char8 *image, L7_BOOL crcChkNotNeeded);

/*********************************************************************
* @purpose  Gets the version of code currently running
*
* @param    version      version
*
* @returns  L7_SUCCESS, if the version is valid
* @returns  L7_FAILURE, if the version has not been initialized yet
*
* @notes    At boot, the running version and the flash version are the same
*           Copies codeVersion_t bytes into *version
*       
* @end
*********************************************************************/
L7_RC_t cdaCodeVersionRunningGet(codeVersion_t *version);

/*********************************************************************
* @purpose  Gets the version of active image stored in flash
*
* @param    version      version
*
* @returns  L7_SUCCESS, if the version is valid
* @returns  L7_FAILURE, if the version has not been read from the code
*                       file yet
*
* @notes    At boot, the running version and the flash version are the same
*       
* @end
*********************************************************************/
L7_RC_t cdaCodeVersionFlashGet(codeVersion_t *version);

/*********************************************************************
* @purpose  Gets the version of backup image in flash
*
* @param    version      version
*
* @returns  L7_SUCCESS, if the version is valid
* @returns  L7_FAILURE, if the version has not been read from the code
*                       file yet
*
* @notes    At boot, the running version and the flash version are the same
*       
* @end
*********************************************************************/
L7_RC_t cdaCodeVersionBackupGet(codeVersion_t *version);

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
L7_RC_t cdaBootcodeUpdate(L7_uint32 unit);

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
L7_RC_t cdaImageDelete(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  activates an image on one or all units
*
* @param    unit        Unit number or CDA_ALL_UNITS
* @param    fileName    image to be activated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sends message to remote unit(s), poll for remote
*           status until complete
*       
* @end
*********************************************************************/
L7_RC_t cdaImageActivate(L7_uint32 unit, L7_char8 *fileName);

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
L7_RC_t cdaUpdateACFMwithSCFMImage(L7_uint32 unit,
		                           L7_char8 *srcImage, 
								   L7_char8 *dstImage);

/*********************************************************************
* @purpose  Updates the kernel on given unit(s)
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
L7_RC_t cdaKernelUpdate(L7_uint32 unit, L7_char8 *srcFileName,
                        L7_int32 fileSize);

#endif /*CDA_API_H*/

