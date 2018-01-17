/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename snmp_fileio_api.h
*
* @purpose File Buffer utility to provide access to NVRAM storage for SNMP
*
* @component SNMP
*
* @comments
*
* @create 09/04/2003
*
* @author cpverne
* @end
*
**********************************************************************/

#ifndef SNMP_FILEIO_API_H
#define SNMP_FILEIO_API_H

#include "l7_common.h"

typedef enum snmp_file_status_e
{
  SNMP_FS_OPEN = 0,
  SNMP_FS_CLOSED,
  SNMP_FS_AVAILABLE,
  SNMP_FS_CREATE
} snmp_file_status_t;

typedef enum snmp_file_mode_e
{
  SNMP_FM_READ = 0,
  SNMP_FM_WRITE,
  SNMP_FM_APPEND,
  SNMP_FM_NONE
} snmp_file_mode_t;

#define SNMP_MAX_FILENAME_SIZE 256
typedef struct snmp_file_s
{
  L7_char8 filename[SNMP_MAX_FILENAME_SIZE];
  L7_uint32 file_id;
  snmp_file_status_t status;
  snmp_file_mode_t mode;
  L7_uint32 eof; /* 0 based, offset to first non-valid character*/
  L7_uint32 pos; /* 0 based, offset to next read from/write to character*/
  L7_uint32 buffer_length; /* 1 based, number of characters in the buffer */
  L7_char8 *buffer;
} snmp_file_t;


/* Default configuration filename used by SNMP Agent */
#define SNMP_CONFIGURATION_FILE "snmpd.cnf"


/*********************************************************************
*
* @purpose Initialize File Buffer table and create a MutEx Semaphore
*
* @returns L7_SUCCESS  if successfully initialized
* @returns L7_ERROR    if semaphore could not be created
* @returns L7_FAILURE  if already initialized
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t snmp_fileinit();

/*********************************************************************
*
* @purpose Returns a pointer to the file buffer structure for a given file_id
*
* @param   file_id     @b{(input)} file identifier
*
* @returns pointer to file buffer structure
*
* @comments
*
* @end
*
*********************************************************************/
snmp_file_t *snmp_file_get(L7_uint32 file_id);

/* new file functions to interface to an flash-based file system */
/*********************************************************************
*
* @purpose Copies a string from the file buffer and updates current position
*
* @param   buf         @b{(output)} buffer to receive the string
* @param   size        @b{(input)} max number of characters to copy
* @param   file_id     @b{(input)} file identifier
*
* @returns 1 if a string is copied
* @returns 0 if the file isn't found, or nothing was copied
*
* @comments Stops at first occurance of '\n'.
*
* @end
*
*********************************************************************/
L7_uint32 snmp_fgets(L7_char8 *buf, L7_uint32 size, L7_uint32 file_id);

/*********************************************************************
*
* @purpose Determines if file position is at the end of the file
*
* @param   file_id     @b{(input)} file identifier
*
* @returns 1   if at the end of the file, or if there's an error
* @returns 0   if there are more characters in the file
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 snmp_feof(L7_uint32 file_id);

/*********************************************************************
*
* @purpose Copies a string into the file buffer and updates position
*
* @param   buf         @b{(input)} string buffer
* @param   file_id     @b{(input)} file identifier
*
* @returns number of characters copied
*
* @comments Sets the file buffer's end-of-file to be after the new 
*           string.
*
* @end
*
*********************************************************************/
L7_uint32 snmp_fputs(const L7_char8 *buf, L7_uint32 file_id);

/*********************************************************************
*
* @purpose Opens a file
*
* @param   filename    @b{(input)} file name
* @param   mode        @b{(input)} file mode  ("r"=read, "w"=write, "a"=append)
*
* @returns file_id of the newly opened file
*
* @comments If a file is already open, it will ignore the current file buffer
*
* @end
*
*********************************************************************/
L7_uint32 snmp_fopen(const L7_char8 *filename, const L7_char8 *mode);

/*********************************************************************
*
* @purpose Closes a currently open file buffer
*
* @param   file_id     @b{(input)} file identifier
*
* @returns 1 if the file could be closed
* @returns 0 if the file could not be closed
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 snmp_fclose(L7_uint32 file_id);

/*********************************************************************
*
* @purpose Resets the file buffer to it's default data for the corresponding filename
*
* @param   filename    @b{(input)} filename of buffer to reset
*
* @returns L7_SUCCESS       if the file could be defaulted
* @returns L7_ERROR         if it could not get a semaphore
* @returns L7_FAILURE       if the file buffer doesn't exist for that file
*
* @comments Only defaults the first occurance of the file
*
* @end
*
*********************************************************************/
L7_RC_t snmp_resetToDefault(const L7_char8 *filename);

/*********************************************************************
*
* @purpose Writes default data into the file
*
* @param   file_id     @b{(input)} file identifier
*
* @returns L7_SUCCESS       if the file could be defaulted
* @returns L7_ERROR         if it could not get a semaphore
* @returns L7_FAILURE       if the file buffer doesn't exist for that file
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t snmp_buildDefaultFile(L7_uint32 file_id);

/*********************************************************************
*
* @purpose Returns a copy of the first file buffer corresponding to that filename
*
* @param   filename    @b{(input)} filename
* @param   buffer      @b{(output)} pointer to recieve character buffer
*
* @returns L7_SUCCESS
* @returns L7_ERROR         row does not exist
* @returns L7_FAILURE
*
* @comments Allocates a character buffer to hold the copy, this must be 
*           freed by the caller
*
* @end
*
*********************************************************************/
L7_RC_t snmp_getFileBuffer(const L7_char8 *filename, L7_char8 **buffer, L7_uint32 *buffer_length);


/* DEBUG FUNCTIONS */

/*********************************************************************
*
* @purpose Prints the file corresponding to the filename
*
* @param   filename    @b{(input)} filename
*
* @returns 1 if the file could be printed
* @returns 0 if the file could not be found
*
* @comments Prints the actual file buffer
*
* @end
*
*********************************************************************/
L7_uint32 snmp_printFileRaw(L7_char8 *filename);

/*********************************************************************
*
* @purpose Prints the file corresponding to the filename
*
* @param   filename    @b{(input)} filename
*
* @returns 1 if the file could be printed
* @returns 0 if the file could not be found
*
* @comments File must be open to print, uses snmp_fgets
*
* @end
*
*********************************************************************/
L7_uint32 snmp_printFile(L7_char8 *filename);

/*********************************************************************
*
* @purpose Saves the file corresponding to the filename to the 
*           single-config file
*
* @param   filename    @b{(input)} filename
*
* @returns L7_SUCCESS  if the file could be saved
* @returns L7_ERROR    if the file could not be saved
* @returns L7_FAILURE  if the file is not stored in a buffer
*
* @end
*
*********************************************************************/
L7_RC_t snmp_saveFileBuffer(L7_char8 *filename);

#endif /* SNMP_FILEIO_API_H */

