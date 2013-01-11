/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename  osapi_file.h
 *
 * @purpose   OS independent prototype declarations for File System
 *
 * @component osapi
 *
 * @comments  copied from osapi.h
 *
 * @create    12/07/2006
 *
 * @author    Rama Sasthri, Kristipati
 *
 * @end
 *
 *********************************************************************/

#ifndef OSAPI_FILE_H
#define OSAPI_FILE_H

#include "datatypes.h"
#include "commdefs.h"
#include "cli_web_exports.h"

/* enumeration of different file types */

#define OSAPI_REGULAR_FILE    "regular"
#define OSAPI_DIRECTORY       "directory"
#define OSAPI_CONFIGSCR       "config-script"
#define OSAPI_IMAGE           "code-image"
#define OSAPI_CONFIGURATION   "configuration"
#define OSAPI_SYSTEM_FILE     "system"
#define OSAPI_SPECIAL_FILE    "special"
#define OSAPI_UNKNOWN         "unknown"

typedef enum
{
  OSAPI_TYPE_REGULAR_FILE = 1,
  OSAPI_TYPE_DIRECTORY,
  OSAPI_TYPE_CONFIGSCR,
  OSAPI_TYPE_IMAGE,
  OSAPI_TYPE_CONFIGURATION,
  OSAPI_TYPE_SYSTEM_FILE,
  OSAPI_TYPE_SPECIAL_FILE,
  OSAPI_TYPE_UNKNOWN,
  OSAPI_TYPE_MAX
} osapiFileType_t;

/* enumeration of different permission modes */

#define OSAPI_RO              "ro"
#define OSAPI_RW              "rw"
#define OSAPI_WO              "wo"
#include "transfer_exports.h"

#ifdef _L7_OS_ECOS_

#define OSAPI_CODE_FILENAME1  "/dev/flash1"
#define OSAPI_CODE_FILENAME2  "/dev/flash2"
#define OSAPI_CODE_FILENAME3  "/dev/boot.dim"
#define MAX_FILE_SIZE   1572864

#endif

typedef enum
{
  OSAPI_PERM_RO = 1,
  OSAPI_PERM_RW,
  OSAPI_PERM_WO,
  OSAPI_PERM_MAX
} osapiFilePermission_t;

typedef enum
{
  OSAPI_FILE_CREATE = 0,
  OSAPI_FILE_OPEN,
  OSAPI_FILE_READ,
  OSAPI_FILE_WRITE,
  OSAPI_FILE_CLOSE,
  OSAPI_FILE_DELETE,
  OSAPI_FILE_CP_CREATE, /* Create file for Captive Portal Download */
  OSAPI_FILE_NV_CREATE /* Create file for Network Visualization Download */
} osapiFileOperation;

/*********************************************************************
 *
 * @structures L7_FILE_t
 *
 * @purpose  for holding all 'viewable' properties of a file
 *
 * @comments
 *
 *********************************************************************/
typedef struct _file_t
{
  L7_char8 name[L7_MAX_FILENAME];
  L7_char8 type[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 mode[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 modifiedDate[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 size;
  L7_uint32 index;

} L7_FILE_t;

/*********************************************************************
 *
 * @structures L7_FILE_COMPACT_t
 *
 * @purpose  compact structure for holding all 'viewable' properties of a file
 *
 * @comments
 *
 *********************************************************************/
typedef struct _file_compact_t
{
  L7_char8              name[L7_MAX_FILENAME];
  osapiFileType_t       type;
  osapiFilePermission_t mode;
  L7_uint32             size;
} L7_FILE_COMPACT_t;


/*********************************************************************
 *
 * @structures L7_DIR_t
 *
 * @purpose  for holding all 'viewable' properties of a directory
 *
 * @comments
 *
 *********************************************************************/
typedef struct _dir_t
{
  L7_char8 name[L7_MAX_FILENAME];
  L7_uint32 size;
  L7_uint32 totalSize;
  L7_uint32 freeSize;

} L7_DIR_t;

/**************************************************************************
 *
 * @purpose  Development debug file system commands used by utility menu
 *
 * @param    none
 *
 * @returns  none
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API void osapiFsUtils (void);

/**************************************************************************
 *
 * @purpose     Dump all buffered files to the file system.
 *
 * @param       none
 * @returns     none
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API void osapiFsSync (void);

/***************************************************************************
 *
 * @purpose Obtain the directory information from a volume's directory
 *
 * @param  pCB        @b{(input)}  where to put listing
 * @param  max_bytes  @b{(input)}  pCB size
 *
 * @comments  If successful, the output of @b{osapiFsDir()} will be a NULL terminated string
 *         Subdirectories always contain two special entries. The “.” entry refers to
 *         the subdirectory itself, while the “..” entry refers to the subdirectory’s parent
 *         directory. The root directory does not contain these special entries.
 *         An example output will look like this...
 *
 * @table{
 * @row{@cell{@c{ ./ }}        @cell{@c{ subdir }}}
 * @row{@cell{@c{ ../ }}       @cell{@c{ subdir }}}
 * @row{@cell{@c{ subdir1/ }}  @cell{@c{ subdir }}}
 * @row{@cell{@c{ subdir2/ }}  @cell{@c{ subdir }}}
 * @row{@cell{@c{ testfile }}  @cell{@c{ file   }}}}
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @end
 *
 ***************************************************************************/
EXT_API L7_RC_t osapiFsDir (L7_char8 * pCB, L7_uint32 max_bytes);

/***************************************************************************
 *
 * @purpose Obtain the directory information from a volume's directory
 *
 * @param  pFile      @b{(input)}  where to put listing
 * @param  pDir       @b{(input)}  where to put the directory summary
 *                                 contain name of directory that should be read, 
 *                                 if such isn't specified then read current
 * @param  max_files  @b{(input, output)}  Max number of files/ available 
 *
 * @comments  If successful, the @b{osapiFsList()} will return the details of 
 *            various files, subdirectorys etc in a given directory.
 *            max_files is the max number of file that will be read. If directory
 *            contain more then max_files files information about them will be ignored
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @end
 *
 ***************************************************************************/
EXT_API L7_RC_t osapiFsList (L7_FILE_t * pFile, L7_DIR_t * pDir, L7_uint32 * max_files);

/***************************************************************************
*
* @purpose Obtain the directory information from a volume's directory
*
* @param  pFile      @b{(input)}  where to put listing
* @param  max_files  @b{(input, output)}  Max number of files/ available
*
* @comments  If successful, the @b{osapiFsList()} will return the details of
*            various files, subdirectorys etc in a given directory.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @end
*
***************************************************************************/
EXT_API L7_RC_t osapiFsListCompact( L7_FILE_COMPACT_t *pFile, L7_uint32 *max_files );

/**************************************************************************
 *
 * @purpose  Create a file in the current subdirectory of the filesystem
 *
 * @param    filename  @b{(input)}  ptr to a null terminated string
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR No descriptors remaining
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsCreateFile (L7_char8 * filename);

/**************************************************************************
 *
 * @purpose  Init and create a RAM file system.
 *
 * @param    path @b{(input)} ptr to a null terminated string containing
 *                            path to ramdisk
 * @param    name@b{(input)} ptr to a null terminated string containing
 *                            name of ramdisk
 * @param    rblks@b{(input)} requested number of blocks
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments Use this function with CAUTION as it is destructive!
 * @comments It first unmounts and deletes the previous ramdisk (if any).
 * @comments The function proceeds as;
 * @comments - Attempt to un-mount previous/unwanted mount
 * @comments - Attempt to delete previous/unwanted directory
 * @comments - Create and format 1MB file storage.
 * @comments - Create a ext2/3 filesystem
 * @comments - Make new directory
 * @comments - Mount the new RAM file system
 *
 * @notes - mke2fs uses "-F" (force) avoiding "not a special device" query
 * @notes - mount uses "-o loop" for loop back device
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsRamCreate (L7_char8 * path, L7_char8 * name, L7_uint32 rblks);

/**************************************************************************
 *
 * @purpose  Create a file in the current subdirectory of the filesystem
 *
 * @param    filename  @b{(input)}  ptr to a null terminated string
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR No descriptors remaining
 *
 * @comments Will not switch to the RamDiskVolName. In a system with two filesystem
 *           volumes, i.e., a FLASH only system, the larger code download volume
 *           is usually the default. Use this function to stay on the default volume.
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsCurrentVolCreateFile (L7_char8 * filename);

/**************************************************************************
 *
 * @purpose  Delete a file in the current subdirectory of the filesystem
 *
 * @param    filename  @b{(input)}  ptr to a null terminated string
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsDeleteFile (L7_char8 * filename);

/**************************************************************************
 *
 * @purpose  Delete a file in the current subdirectory of the filesystem
 *
 * @param    filename  @b{(input)}  ptr to a null terminated string
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments Will not switch to the RamDiskVolName. In a system with two filesystem
 *           volumes, i.e., a FLASH only system, the larger code download volume
 *           is usually the default. Use this function to stay on the default volume.
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsCurrentVolDeleteFile (L7_char8 * filename);

/**************************************************************************
 *
 * @purpose  Rename a file in the current subdirectory of the filesystem
 *
 * @param    oldfilename  @b{(input)}  old filename - ptr to a null terminated string
 * @param    newfilename  @b{(input)}  new filename - ptr to a null terminated string
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsRenameFile (L7_char8 * oldfilename, L7_char8 * newfilename);

/**************************************************************************
 *
 * @purpose  Rename a file in the current subdirectory of the filesystem
 *
 * @param    oldfilename  @b{(input)}  old filename - ptr to a null terminated string
 * @param    newfilename  @b{(input)}  new filename - ptr to a null terminated string
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsCurrentVolRenameFile (L7_char8 * oldfilename, L7_char8 * newfilename);

/**************************************************************************
 *
 * @purpose  Copies a file
 *
 * @param    infilename   @b{(input)}  in filename - name of file to read
 * @param    outfilename  @b{(input)}  out filename - name of file to write
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsCopyFile (L7_char8 * infilename, L7_char8 * outfilename);

/**************************************************************************
 *
 * @purpose  Copies a file in the current subdirectory of the filesystem
 *
 * @param    infilename   @b{(input)}  in filename - name of file to read
 * @param    outfilename  @b{(input)}  out filename - name of file to write
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsCurrentVolCopyFile (L7_char8 * infilename, L7_char8 * outfilename);

/**************************************************************************
 *
 * @purpose  Write data to a file. Create/Open file if does not exist.
 *
 * @param    filename  @b{(input)}  File to write data to
 * @param    buffer    @b{(input)}  actual data to write to file
 * @param    nbytes    @b{(input)}  number of bytes to write to file
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments    If the WRITE_TO_FLASH flag shows ENABLED, also write contents of
 *           RAM DISK to Flash.
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsWrite (L7_char8 * filename, L7_char8 * buffer, L7_int32 nbytes);

/**************************************************************************
 *
 * @purpose  Write data to a file. Assumes file already opened. Do not close file to allow successive writes
 *
 * @param    filedesc  @b{(input)}  descriptor of file to close
 * @param    buffer    @b{(input)}  actual data to write to file
 * @param    nbytes    @b{(input)}  number of bytes to write to file
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments    If the WRITE_TO_FLASH flag shows ENABLED, also write contents of
 *              RAM DISK to Flash.
 *              This function is used when the file pointer must be moved and not reset
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsWriteNoClose (L7_int32 filedesc, L7_char8 * buffer, L7_int32 nbytes);

/**************************************************************************
 *
 * @purpose  Write data to a file. Create/Open file if does not exist.
 *
 * @param    filename  @b{(input)}  File to write data to
 * @param    buffer    @b{(input)}  actual data to write to file
 * @param    nbytes    @b{(input)}  number of bytes to write to file
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments If the WRITE_TO_FLASH flag shows ENABLED, also write contents of
 *           RAM DISK to Flash.
 *           Will not switch to the RamDiskVolName. In a system with two filesystem
 *           volumes, i.e., a FLASH only system, the larger code download volume
 *           is usually the default. Use this function to stay on the default volume.
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsCurrentVolWrite (L7_char8 * filename, L7_char8 * buffer, L7_int32 nbytes);

/**************************************************************************
 * @purpose  Sets the read/write pointer of an already open file
 *
 * @param    fd        Open file descriptor
 * @param    nbytes    Offset
 * @param    mode      SEEK_SET, pointer set to to offset bytes
 *                     SEEK_CUR, pointer set to current value plus offset bytes
 *                     SEEK_END, pointer set to size of the file plus offset bytes
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments This function opens the file, reads data, and closes the file
 *
 * @end
 *************************************************************************/
EXT_API L7_RC_t osapiFileSeek (L7_int32 fd, L7_int32 nbytes, L7_uint32 mode);

/**************************************************************************
 * @purpose  Read specified number of bytes from the file descriptor.
 *       If file is shorter then specified number of bytes then
 *       return code is still success.
 *
 * @param    fd     Open file descriptor.
 * @param    buffer   where to put data
 * @param    nbytes    amount of data to read
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments
 *
 * @end
 **************************************************************************/
EXT_API L7_RC_t osapiFileRead (L7_int32 fd, L7_char8 * buffer, L7_int32 nbytes);

/**************************************************************************
 * @purpose  Read specified number of bytes from the file descriptor.
 *       If file is shorter then specified number of bytes then
 *       return code is still success.
 *
 * @param    fd       Open file descriptor.
 * @param    buffer   where to put data
 * @param    nbytes   amount of data to read, also return the read length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments
 *
 * @end
 **************************************************************************/
EXT_API L7_RC_t osapiFileReadWithLen (L7_int32 fd, L7_char8 * buffer, L7_int32 * nbytes);

/**************************************************************************
 *
 * @purpose  Read the contents of a file
 *
 * @param    filename  @b{(input)}  File from which to read data
 * @param    buffer    @b{(input)}  where to put data
 * @param    nbytes    @b{(input)}  amount of data to read
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments    This function opens the file, reads data, and closes the file
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsRead (L7_char8 * filename, L7_char8 * buffer, L7_int32 nbytes);

/**************************************************************************
 *
 * @purpose  Read the contents of a file
 *
 * @param    filedesc  @b{(input)}  File descriptor from previous open
 * @param    buffer    @b{(input)}  where to put data
 * @param    nbytes    @b{(input)}  amount of data to read
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @notes    This function reads data from the file, but does not close the file.
 * @notes    osapiFsOpen must first be called.
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsReadCurrentVolNoClose (L7_int32 filedesc, L7_char8 * buffer,
    L7_int32 nbytes);

/**************************************************************************
 *
 * @purpose  Read the contents of a file
 *
 * @param    filename  @b{(input)}  File from which to read data
 * @param    buffer    @b{(input)}  where to put data
 * @param    nbytes    @b{(input)}  amount of data to read
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments This function opens the file, reads data, and closes the file.
 *           Will not switch to the RamDiskVolName. In a system with two filesystem
 *           volumes, i.e., a FLASH only system, the larger code download volume
 *           is usually the default. Use this function to stay on the default volume.
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsCurrentVolRead (L7_char8 * filename, L7_char8 * buffer, L7_int32 nbytes);

/**************************************************************************
 *
 * @purpose  Retrieve file size
 *
 * @param    filename  @b{(input)}  string of file to retrieve size
 * @param    filesize  @b{(output)} integer of file size
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments Will not switch to the RamDiskVolName. In a system with two filesystem
 *           volumes, i.e., a FLASH only system, the larger code download volume
 *           is usually the default. Use this function to stay on the default volume.
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsCurrentVolFileSize (L7_char8 * filename, L7_int32 * filesize);


/**************************************************************************
 *
 * @purpose  Opens a file
 *
 * @param    filename  @b{(input)}  File to Open
 * @param    filedesc  @b{(input)}  Pointer to file descriptor
 *
 * @returns  L7_ERROR if file does not exist
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsOpen (L7_char8 * filename, L7_int32 *filedesc);


/**************************************************************************
 *
 * @purpose  Create a file in the current subdirectory of the filesystem
 *
 * @param    filename @b{(input)} ptr to a null terminated string
 * @param    filedesc @b{(output)} location to store the file descriptor
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE if the file is not created
 *
 * @comments This function creates the file, but does not close the file.
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsFileCreate (L7_char8 * filename, L7_int32 * filedesc);


/**************************************************************************
 *
 * @purpose  Opens a file
 *
 * @param    filename  @b{(input)}  File to Open
 *
 * @returns  A file descriptor
 * @returns  L7_ERROR if file does not exist
 *
 * @comments Will not switch to the RamDiskVolName. In a system with two filesystem
 *           volumes, i.e., a FLASH only system, the larger code download volume
 *           is usually the default. Use this function to stay on the default volume.
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_int32 osapiFsCurrentVolOpen (L7_char8 * filename);

/**************************************************************************
 *
 * @purpose  closes a file opened by osapiFsOpen
 *
 * @param    filedesc  @b{(input)}  descriptor of file to close
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR if file already closed
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsClose (L7_int32 filedesc);

/**************************************************************************
 *
 * @purpose  closes a file opened by osapiFsOpen
 *
 * @param    filedesc  @b{(input)}  descriptor of file to close
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR if file already closed
 *
 * @comments Will not switch to the RamDiskVolName. In a system with two filesystem
 *           volumes, i.e., a FLASH only system, the larger code download volume
 *           is usually the default. Use this function to stay on the default volume.
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsCurrentVolClose (L7_int32 filedesc);

/**************************************************************************
 *
 * @purpose  Retrieve file size
 *
 * @param    filename  @b{(input)}  string of file to retrieve size
 * @param    filesize  @b{(output)} integer of file size
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_RC_t osapiFsFileSizeGet (L7_char8 * filename, L7_uint32 * filesize);


/**************************************************************************
 *
 * @purpose  Check if the file already exists
 *
 * @param    filename  @b{(input)}  file to check
 *
 * @returns  L7_TRUE    if specified file exist
 * @returns  L7_FALSE   if not exist
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
EXT_API L7_BOOL osapiFsFileExist(L7_char8 * filename);

#endif /* OSAPI_FILE_H */
