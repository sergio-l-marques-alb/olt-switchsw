/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename     usmdb_file_api.h
*
* @purpose      externs for USMDB FILE layer
*
* @component    osapi
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
#ifndef USMDB_FILE_API_H
#define USMDB_FILE_API_H


/* ===================== Function prototypes ===================== */ 

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
                        L7_char8 *newFileName);

/*********************************************************************
* @purpose  Deletes a given file from the File system
*           
* @param    unit        @b{(input)}  Unit for this operation.           
* @param    fileName    @b{(input)}  file to be deleted
*
* @returns  L7_SUCCESS      on successful execution
* @returns  L7_NOT_EXIST    if the specified file could not be found
* @returns  L7_REQUEST_DENIED if the file could not be deleted
*
* @notes    This function uses the osapi and the dim components to
*           delete the given file.
*
* @end
*********************************************************************/
L7_RC_t usmDbFileDelete(L7_uint32 unit, L7_char8 *fileName);

/*********************************************************************
* @purpose  Retrieves a directory listing from the Flash
*           
* @param    unit        @b{(input)}     Unit for this operation.           
* @param    file        @b{(output)}    Place holder for the listing
* @param    dir         @b{(output)}    Place holder for the summary
* @param    maxFiles    @b{(input, output)}    number of files
*
* @returns  L7_SUCCESS      on successful execution
* @returns  L7_FAILURE      on fs errors
*
* @notes    This function uses the osapi and the dim components to
*           retrieve and modify the listing.
*
* @end
*********************************************************************/
L7_RC_t usmDbFileDir(L7_uint32 unit, L7_FILE_t *file,
        L7_DIR_t *dir, L7_uint32 *maxFiles);

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
        L7_char8 *dstFileName, L7_BOOL blocking);

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
L7_RC_t usmDbDirList(L7_FILE_COMPACT_t *fileList, L7_uint32 *fileEntries);


#endif /* USMDB_FILE_API_H */
