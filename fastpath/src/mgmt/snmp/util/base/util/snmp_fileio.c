/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename snmp_fileio.c
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

#include <stdio.h>
#include <string.h>

#include "l7_common.h"
#include "osapi.h"
#include "snmp_fileio_api.h"
#include "sysapi.h"

L7_uint32 new_file_id = 1; /* must not be 0, as that is the error value */
void *snmp_fileio_sema = L7_NULLPTR;

#define SNMP_MAX_FILE_BUFFERS 10
#define SNMP_DEFAULT_FILE_BUFFER_SIZE 1000
#define SNMP_DEFAULT_FILE_WAIT_TIMEOUT 10

snmp_file_t snmpFileBuffers[SNMP_MAX_FILE_BUFFERS];

L7_BOOL snmp_fileio_debug = 0;

#define SNMP_FILEIO_LOG(__fmt__, __args__... ) \
        { if (snmp_fileio_debug) { sysapiPrintf(__fmt__, ## __args__); } }
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
L7_RC_t snmp_fileinit()
{
  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32 i;

  if (firstTime == L7_TRUE)
  {
    for (i = 0; i < SNMP_MAX_FILE_BUFFERS; i++)
    {
      snmpFileBuffers[i].status = SNMP_FS_AVAILABLE;
      snmpFileBuffers[i].buffer = L7_NULLPTR;
      snmpFileBuffers[i].filename[0] = 0;
      snmpFileBuffers[i].eof = 0;
      snmpFileBuffers[i].pos = 0;
      snmpFileBuffers[i].mode = SNMP_FM_NONE;
    }

    /* Mutual exclusion semaphore */
    snmp_fileio_sema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);

    if (snmp_fileio_sema == L7_NULLPTR)
    {
/*      printf("SNMP file: could not create semaphore !!!!\n");*/
      return L7_ERROR;  /* Could not create semaphore for some reason. */
    }

    firstTime = L7_FALSE;

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

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
snmp_file_t *snmp_file_get(L7_uint32 file_id)
{
  L7_uint32 i;

/*  printf("SNMP file: get %d ", file_id);*/

/*  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return L7_NULLPTR;*/

  for (i = 0; i < SNMP_MAX_FILE_BUFFERS; i++)
  {
    if (snmpFileBuffers[i].status == SNMP_FS_OPEN && snmpFileBuffers[i].file_id == file_id)
    {
/*      printf("succeed=%s\n", snmpFileBuffers[i].filename);*/
      (void)osapiSemaGive(snmp_fileio_sema);
      return &snmpFileBuffers[i];
    }
  }
/*  printf("fail\n");*/
/*  (void)osapiSemaGive(snmp_fileio_sema);*/
  return L7_NULLPTR;
}

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
L7_uint32 snmp_fgets(L7_char8 *buf, L7_uint32 size, L7_uint32 file_id)
{
  L7_uint32 len;
  L7_char8 *start;
  snmp_file_t *file = L7_NULLPTR;

  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return 0;

  file = snmp_file_get(file_id);

  /* if file doesn't exist, or at end of file */
  if (file == L7_NULLPTR || snmp_feof(file_id))
  {
    (void)osapiSemaGive(snmp_fileio_sema);
    return 0;
  }

  start = &(file->buffer[file->pos]);

  /* search for the next end of line */
  for (len=0; len < (size - 1) && len < (file->eof - file->pos) ; len++)
  {
    if (start[len] == '\n')
      break;
  }

  /* copy the new data */
  memcpy(buf, start, len+1);
  buf[len+1] = 0;

  /* update the current file position */
  file->pos += len+1;

  (void)osapiSemaGive(snmp_fileio_sema);
  return 1;
}

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
L7_uint32 snmp_feof(L7_uint32 file_id)
{
  snmp_file_t *file = L7_NULLPTR;
  L7_uint32 ret_val = 1;

  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return ret_val;

  file = snmp_file_get(file_id);

  if (file != L7_NULLPTR && file->status == SNMP_FS_OPEN)
  {
    ret_val = (file->pos >= file->eof);
  }

  (void)osapiSemaGive(snmp_fileio_sema);
  return ret_val;
}


/*********************************************************************
*
* @purpose Underlying function to copy a string into the file buffer and 
*          update the position
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
L7_uint32 _snmp_fputs(const L7_char8 *buf, L7_uint32 file_id)
{
  snmp_file_t *file = L7_NULLPTR;
  L7_uint32 sz = strlen(buf);
  L7_char8 *temp_buffer = L7_NULLPTR;
  L7_uint32 temp_buffer_length;

  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return 0;

  file = snmp_file_get(file_id);

  if (file == L7_NULLPTR)
  {
    (void)osapiSemaGive(snmp_fileio_sema);
    return 0;
  }


  /* check to see if the buffer is large enough (allow for trailing \0 character) */
  if (file->pos + sz >= file->buffer_length)
  {
    /* create a new buffer large enough to hold the current file, plus the default size */

    temp_buffer_length = file->buffer_length + sz + SNMP_DEFAULT_FILE_BUFFER_SIZE;
    temp_buffer = (char*) osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(char) * temp_buffer_length);

    /* could not create additional space needed */
    if (temp_buffer == L7_NULLPTR)
    {
      (void)osapiSemaGive(snmp_fileio_sema);
      return 0;
    }

    /* initialize the new buffer */
    bzero(temp_buffer, temp_buffer_length);
    /* copy the old buffer to the new buffer space */
    memcpy(temp_buffer, file->buffer, file->buffer_length);
    /* free the allocated memory */
    osapiFree(L7_SNMP_COMPONENT_ID, file->buffer);
    file->buffer = temp_buffer;
    file->buffer_length = temp_buffer_length;
  }
  strcpy(&file->buffer[file->pos], buf);
  file->pos += sz;

  /* update the end of the file to be after the end of the last write */
  file->eof = file->pos;

  (void)osapiSemaGive(snmp_fileio_sema);
  return sz;
}

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
L7_uint32 snmp_fputs(const L7_char8 *buf, L7_uint32 file_id)
{
  L7_uint32 rval;

  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return 0;

  rval = _snmp_fputs(buf,file_id);

  (void)osapiSemaGive(snmp_fileio_sema);
  return rval;
}

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
L7_uint32 snmp_fopen(const L7_char8 *filename, const L7_char8 *mode)
{
  L7_uint32 i;
  L7_uint32 file_buffer_length = 0;
  L7_uint32 temp_buffer_length = 0;

  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return 0;

  SNMP_FILEIO_LOG("SNMP open: %s '%s'\n", filename, mode);

  /* search for a preexisting copy of this file */
  for (i = 0; i < SNMP_MAX_FILE_BUFFERS; i++)
  {
    if (snmpFileBuffers[i].status == SNMP_FS_CLOSED &&
        strcmp(filename, snmpFileBuffers[i].filename) == 0)
    {
      snmpFileBuffers[i].file_id = new_file_id;
      new_file_id += 1;

      if (strcmp(mode, "r") == 0)
      {
        snmpFileBuffers[i].mode = SNMP_FM_READ;
        snmpFileBuffers[i].pos = 0;
      }
      else if (strcmp(mode, "a") == 0)
      {
        snmpFileBuffers[i].mode = SNMP_FM_APPEND;
        snmpFileBuffers[i].pos = snmpFileBuffers[i].eof;
      }
      else if (strcmp(mode, "w") == 0)
      {
        snmpFileBuffers[i].mode = SNMP_FM_WRITE;
        snmpFileBuffers[i].pos = 0;
      }
      else
      {
        /* unknown mode */
        (void)osapiSemaGive(snmp_fileio_sema);
        SNMP_FILEIO_LOG("SNMP open: unknown mode1\n");
        return 0;
      }

      snmpFileBuffers[i].status = SNMP_FS_OPEN;

      SNMP_FILEIO_LOG("SNMP open: reopen buffer\n");

      (void)osapiSemaGive(snmp_fileio_sema);
      return snmpFileBuffers[i].file_id;
    }
  }

  /* now search for an empty file buffer to use */
  for (i = 0; i < SNMP_MAX_FILE_BUFFERS; i++)
  {
    if (snmpFileBuffers[i].status == SNMP_FS_AVAILABLE)
    {
      SNMP_FILEIO_LOG("SNMP open: looking for an empty buffer.\n");
      strncpy(snmpFileBuffers[i].filename, filename, SNMP_MAX_FILENAME_SIZE-1);
      snmpFileBuffers[i].status = SNMP_FS_CREATE;
      snmpFileBuffers[i].buffer_length = 0;
      snmpFileBuffers[i].eof = 0;
      snmpFileBuffers[i].pos = 0;
      snmpFileBuffers[i].file_id = new_file_id;
      new_file_id += 1;


      if (strcmp(mode, "r") == 0)
      {
        snmpFileBuffers[i].mode = SNMP_FM_READ;
      }
      else if (strcmp(mode, "a") == 0)
      {
        snmpFileBuffers[i].mode = SNMP_FM_APPEND;
      }
      else if (strcmp(mode, "w") == 0)
      {
        snmpFileBuffers[i].mode = SNMP_FM_WRITE;
      }
      else
      {
        /* unknown mode, return entry to the pool and return */
        snmpFileBuffers[i].status = SNMP_FS_AVAILABLE;

        SNMP_FILEIO_LOG("SNMP open: unknown mode2\n");

        (void)osapiSemaGive(snmp_fileio_sema);
        return 0;
      }

      if (snmpFileBuffers[i].mode == SNMP_FM_READ ||
          snmpFileBuffers[i].mode == SNMP_FM_APPEND)
      {
        /* file should exist, try opening it */
        if (sysapiCfgFileSizeGet(L7_SNMP_COMPONENT_ID, snmpFileBuffers[i].filename, &file_buffer_length) == L7_SUCCESS)
        {
          SNMP_FILEIO_LOG("SNMP open: existing file (%d)\n", file_buffer_length);
          temp_buffer_length = file_buffer_length;

          /* if opening the file for appending, add an additional buffer to the end */
          if (snmpFileBuffers[i].mode == SNMP_FM_APPEND)
          {
            temp_buffer_length += SNMP_DEFAULT_FILE_BUFFER_SIZE;
          }

          snmpFileBuffers[i].buffer = (char*) osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(char) * temp_buffer_length);
          snmpFileBuffers[i].buffer_length = temp_buffer_length;
          bzero(snmpFileBuffers[i].buffer, snmpFileBuffers[i].buffer_length);

          if (sysapiCfgFileRawGet(L7_SNMP_COMPONENT_ID, snmpFileBuffers[i].filename, 
                                  snmpFileBuffers[i].buffer, &file_buffer_length) == L7_SUCCESS)
          {
            SNMP_FILEIO_LOG("SNMP open: existing file read\n");
            snmpFileBuffers[i].status = SNMP_FS_OPEN;

  
            /* set the end of file */
            snmpFileBuffers[i].eof = snmpFileBuffers[i].buffer_length-1;
          }
          else
          {
            SNMP_FILEIO_LOG("SNMP open: existing file could not be read!\n");
            snmpFileBuffers[i].status = SNMP_FS_CREATE;
            osapiFree(L7_SNMP_COMPONENT_ID, snmpFileBuffers[i].buffer);
            snmpFileBuffers[i].buffer = 0;
            snmpFileBuffers[i].buffer_length = 0;
          }
        }

        /* If file found, but couldn't be read, or if no file found load defaults */
        if (snmpFileBuffers[i].status == SNMP_FS_CREATE)
        {
          temp_buffer_length = SNMP_DEFAULT_FILE_BUFFER_SIZE;

          SNMP_FILEIO_LOG("SNMP open: creating default file (%d)\n", file_buffer_length);
          /* create an empty buffer of default size */
          snmpFileBuffers[i].buffer = (char*) osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(char) * temp_buffer_length);
          snmpFileBuffers[i].buffer_length = temp_buffer_length;
          bzero(snmpFileBuffers[i].buffer, snmpFileBuffers[i].buffer_length);
          snmpFileBuffers[i].status = SNMP_FS_OPEN;
          snmpFileBuffers[i].eof = 0;
          snmpFileBuffers[i].pos = 0;

          /* as we couldn't open the file, see if we can create a default version */

          if (snmp_buildDefaultFile(snmpFileBuffers[i].file_id) != L7_SUCCESS)
          {
            snmpFileBuffers[i].mode = SNMP_FS_AVAILABLE;
            osapiFree(L7_SNMP_COMPONENT_ID, snmpFileBuffers[i].buffer);
            snmpFileBuffers[i].buffer = L7_NULLPTR;

/*              printf("SNMP open: couldn't build defaults1\n");*/

            (void)osapiSemaGive(snmp_fileio_sema);
            return 0;
          }
        }
  
        /* set the correct file positions */
        if (snmpFileBuffers[i].mode == SNMP_FM_READ)
        {
          snmpFileBuffers[i].pos = 0;
        }
        else
        {
          snmpFileBuffers[i].pos = snmpFileBuffers[i].eof;
        }
      }
      else
      {
        SNMP_FILEIO_LOG("SNMP open: writing to an empty file\n");

        snmpFileBuffers[i].buffer = (char*) osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(char) * SNMP_DEFAULT_FILE_BUFFER_SIZE);
        snmpFileBuffers[i].buffer_length = SNMP_DEFAULT_FILE_BUFFER_SIZE;
        bzero(snmpFileBuffers[i].buffer, snmpFileBuffers[i].buffer_length);
      }


      /* file loaded, return */
      snmpFileBuffers[i].status = SNMP_FS_OPEN;

      (void)osapiSemaGive(snmp_fileio_sema);
      SNMP_FILEIO_LOG("SNMP open: made file: %d\n", snmpFileBuffers[i].file_id);
      return snmpFileBuffers[i].file_id;
    }
  }

SNMP_FILEIO_LOG("SNMP open: no space available\n");

  /* no space available */
  (void)osapiSemaGive(snmp_fileio_sema);
  return 0;
}

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
L7_uint32 snmp_fclose(L7_uint32 file_id)
{
  snmp_file_t *file = L7_NULLPTR;

  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return 0;

  file = snmp_file_get(file_id);

  if (file == L7_NULLPTR)
  {
    (void)osapiSemaGive(snmp_fileio_sema);
    return 0;
  }

  SNMP_FILEIO_LOG("SNMP close: %d %s\n", file_id, file->filename);

  /* Close the buffer */
  file->status = SNMP_FS_CLOSED;

  (void)osapiSemaGive(snmp_fileio_sema);
  return 1;
}

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
L7_RC_t snmp_resetToDefault(const L7_char8 *filename)
{
  L7_uint32 i;
  L7_RC_t rc;
  snmp_file_status_t temp_status;

  SNMP_FILEIO_LOG("SNMP reset to default: %s\n", filename);

  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return L7_ERROR;

  for (i = 0; i < SNMP_MAX_FILE_BUFFERS; i++)
  {
    if (strcmp(filename, snmpFileBuffers[i].filename) == 0)
    {
      /* seek to beginning of file */
      snmpFileBuffers[i].eof = 0;
      snmpFileBuffers[i].pos = 0;

      /* save old status */
      temp_status = snmpFileBuffers[i].status;

      /* temporarily open file to allow BuildDefaultFile to operate */
      snmpFileBuffers[i].status = SNMP_FS_OPEN;

      /* build up defaults */
      rc = snmp_buildDefaultFile(snmpFileBuffers[i].file_id);


      /* return file to old status */
      snmpFileBuffers[i].status = temp_status;
      
      (void)osapiSemaGive(snmp_fileio_sema);
      return rc;
    }
  }

  (void)osapiSemaGive(snmp_fileio_sema);
  return L7_FAILURE;
}

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
L7_RC_t snmp_buildDefaultFile(L7_uint32 file_id)
{
  snmp_file_t *file = L7_NULLPTR;

/*  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
  {
    printf("SNMP defaults: no sema\n");

    return L7_ERROR;
  }*/

  file = snmp_file_get(file_id);

  if (file == L7_NULLPTR)
  {
/*    (void)osapiSemaGive(snmp_fileio_sema);*/
    return L7_ERROR;
  }

/*  printf("Generating defaults(%d:%s)\n", file_id, file->filename);*/

  if (strcmp(file->filename, SNMP_CONFIGURATION_FILE) == 0)
  {
    /*********************************************************************************************************************/
    /* NOTE: Maximum line length is 80 characters, use \\ to split lines                                                 */
    /***********|*******************************************************************************|*************************/

#if L7_FEAT_SNMP_CONFAPI
    /* Views */
    _snmp_fputs("vacmViewTreeFamilyEntry  DefaultSuper iso - included readOnly\n", file_id);
    _snmp_fputs("vacmViewTreeFamilyEntry  Default  iso - included readOnly\n", file_id);
    _snmp_fputs("vacmViewTreeFamilyEntry  Default  snmpVacmMIB - excluded readOnly\n", file_id);
    _snmp_fputs("vacmViewTreeFamilyEntry  Default  usmUser     - excluded readOnly\n", file_id);
    _snmp_fputs("vacmViewTreeFamilyEntry  Default  snmpCommunityTable - excluded readOnly\n", file_id);
    _snmp_fputs("vacmViewTreeFamilyEntry  Default  snmpVacmMIB - excluded readOnly\n", file_id);

    /* Groups: RO */
    _snmp_fputs("vacmAccessEntry DefaultRead - snmpv1  noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("Default - Default readOnly\n", file_id);
    _snmp_fputs("vacmAccessEntry DefaultRead - snmpv2c noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("Default - Default readOnly\n", file_id);

    /* Groups: RW */
    _snmp_fputs("vacmAccessEntry DefaultWrite - snmpv1  noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("Default Default Default readOnly\n", file_id);
    _snmp_fputs("vacmAccessEntry DefaultWrite - snmpv2c noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("Default Default Default readOnly\n", file_id);

    /* Groups: SU */
    _snmp_fputs("vacmAccessEntry DefaultSuper - snmpv1  noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("DefaultSuper DefaultSuper DefaultSuper readOnly\n", file_id);
    _snmp_fputs("vacmAccessEntry DefaultSuper - snmpv2c noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("DefaultSuper DefaultSuper DefaultSuper readOnly\n", file_id);

    /* notification entries */ /* static */
    _snmp_fputs("snmpNotifyEntry TrapNotify   TrapNotify   trap   readOnly\n", file_id);
    _snmp_fputs("snmpNotifyEntry InformNotify InformNotify inform readOnly\n", file_id);

#else /* L7_FEAT_SNMP_CONFAPI */

    /* create a view */ /* STATIC */
    /* <VIEW NAME> <SUBTREE> <FAMILY MASK> <TYPE> <STORAGE> */
    _snmp_fputs("vacmViewTreeFamilyEntry  All iso - included nonVolatile\n", file_id);

    _snmp_fputs("vacmViewTreeFamilyEntry  Unsecure iso \\\n", file_id);
    _snmp_fputs("- included nonVolatile\n", file_id);
    _snmp_fputs("vacmViewTreeFamilyEntry  Unsecure agentLoginSessionTable \\\n", file_id);
    _snmp_fputs("- excluded nonVolatile\n", file_id);
    _snmp_fputs("vacmViewTreeFamilyEntry  Unsecure agentUserConfigGroup \\\n", file_id);
    _snmp_fputs("- excluded nonVolatile\n", file_id);
    _snmp_fputs("vacmViewTreeFamilyEntry  Unsecure agentSnmpConfigGroup \\\n", file_id);
    _snmp_fputs("- excluded nonVolatile\n", file_id);
    _snmp_fputs("vacmViewTreeFamilyEntry  Unsecure agentSnmpTrapFlagsConfigGroup \\\n", file_id);
    _snmp_fputs("- included nonVolatile\n", file_id);
    /* SNMP configuration tables */
    _snmp_fputs("vacmViewTreeFamilyEntry  Unsecure 1.3.6.1.6.3 \\\n", file_id);
    _snmp_fputs("- excluded nonVolatile\n", file_id);
   /*  TACACS Server Shared Keys  */
    _snmp_fputs("vacmViewTreeFamilyEntry  Unsecure agentTacacsGlobalKey \\\n", file_id);
    _snmp_fputs("- excluded nonVolatile\n", file_id);
    _snmp_fputs("vacmViewTreeFamilyEntry  Unsecure agentTacacsKey \\\n", file_id);
    _snmp_fputs("- excluded nonVolatile\n", file_id);

    /* create access entries*/ /* STATIC */
    /* <ACCESS NAME> <CONTEXT> <VERSION> <SECURITY> <CONTEXT TYPE> <READ VIEW> <WRITE VIEW> <NOTIFY VIEW> <STORAGE TYPE>*/
    _snmp_fputs("vacmAccessEntry READ - snmpv1  noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("Unsecure - - nonVolatile\n", file_id);
    _snmp_fputs("vacmAccessEntry READ - snmpv2c noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("Unsecure - - nonVolatile\n", file_id);

    _snmp_fputs("vacmAccessEntry READ_noAuthNoPriv - usm noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("Unsecure - - nonVolatile\n", file_id);
    _snmp_fputs("vacmAccessEntry READ_authNoPriv   - usm authNoPriv   exact \\\n", file_id);
    _snmp_fputs("Unsecure - - nonVolatile\n", file_id);
    _snmp_fputs("vacmAccessEntry READ_authPriv     - usm authPriv     exact \\\n", file_id);
    _snmp_fputs("Unsecure - - nonVolatile\n", file_id);

    _snmp_fputs("vacmAccessEntry WRITE - snmpv1  noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("All All - nonVolatile\n", file_id);
    _snmp_fputs("vacmAccessEntry WRITE - snmpv2c noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("All All - nonVolatile\n", file_id);

    _snmp_fputs("vacmAccessEntry WRITE_noAuthNoPriv - usm noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("All All - nonVolatile\n", file_id);
    _snmp_fputs("vacmAccessEntry WRITE_authNoPriv   - usm authNoPriv   exact \\\n", file_id);
    _snmp_fputs("All All - nonVolatile\n", file_id);
    _snmp_fputs("vacmAccessEntry WRITE_authPriv     - usm authPriv     exact \\\n", file_id);
    _snmp_fputs("All All - nonVolatile\n", file_id);

    _snmp_fputs("vacmAccessEntry TRAP - snmpv1  noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("- - All nonVolatile\n", file_id);
    _snmp_fputs("vacmAccessEntry TRAP - snmpv2c noAuthNoPriv exact \\\n", file_id);
    _snmp_fputs("- - All nonVolatile\n", file_id);

    /* notification entries */ /* static */
    _snmp_fputs("snmpNotifyEntry TrapEntry   TrapNotify   trap   nonVolatile\n", file_id);
    _snmp_fputs("snmpNotifyEntry InformEntry InformNotify inform nonVolatile\n", file_id);
#endif /* L7_FEAT_SNMP_CONFAPI */
/*    (void)osapiSemaGive(snmp_fileio_sema);*/
    return L7_SUCCESS;
  }

  /* unknown file, defaults to empty file */
/*  (void)osapiSemaGive(snmp_fileio_sema);*/
  return L7_SUCCESS;
}

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
L7_RC_t snmp_getFileBuffer(const L7_char8 *filename, L7_char8 **buffer, L7_uint32 *buffer_length)
{
  L7_uint32 i;

  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return L7_ERROR;

  for (i = 0; i < SNMP_MAX_FILE_BUFFERS; i++)
  {
    if (strcmp(filename, snmpFileBuffers[i].filename) == 0)
    {
      if ((*buffer = osapiMalloc(L7_SNMP_COMPONENT_ID, snmpFileBuffers[i].eof+1)) != L7_NULLPTR)
      {
        memcpy(*buffer, snmpFileBuffers[i].buffer, snmpFileBuffers[i].eof+1);
        *buffer_length = snmpFileBuffers[i].eof+1;

        (void)osapiSemaGive(snmp_fileio_sema);
        return L7_SUCCESS;
      }

      (void)osapiSemaGive(snmp_fileio_sema);
      return L7_ERROR;
    }
  }

  (void)osapiSemaGive(snmp_fileio_sema);
  return L7_FAILURE;
}



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
L7_uint32 snmp_printFile(L7_char8 *filename)
{
  L7_uint32 i;

  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return 0;

  for (i = 0; i < SNMP_MAX_FILE_BUFFERS; i++)
  {
    if (strcmp(filename, snmpFileBuffers[i].filename) == 0)
    {
      sysapiPrintf("%s", snmpFileBuffers[i].buffer);

      (void)osapiSemaGive(snmp_fileio_sema);
      return 1;
    }
  }

  (void)osapiSemaGive(snmp_fileio_sema);
  return 0;
}

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
L7_RC_t snmp_saveFileBuffer(L7_char8 *filename)
{
  L7_uint32 i;

  if (osapiSemaTake(snmp_fileio_sema, SNMP_DEFAULT_FILE_WAIT_TIMEOUT) != L7_SUCCESS)
    return L7_ERROR;

  for (i = 0; i < SNMP_MAX_FILE_BUFFERS; i++)
  {
    if (strcmp(filename, snmpFileBuffers[i].filename) == 0)
    {
      if (sysapiBinaryCfgFileRawWrite(L7_SNMP_COMPONENT_ID, filename, 
                                      snmpFileBuffers[i].buffer, 
                                      snmpFileBuffers[i].eof+1) == L7_SUCCESS)
      {
        SNMP_FILEIO_LOG("SNMP File saved.");
        (void)osapiSemaGive(snmp_fileio_sema);
        return L7_SUCCESS;
      }

      SNMP_FILEIO_LOG("SNMP File NOT saved.");
      (void)osapiSemaGive(snmp_fileio_sema);
      return L7_ERROR;
    }
  }

  (void)osapiSemaGive(snmp_fileio_sema);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Turns on/off fileio debug messages
*
* @param   val    @b{(input)} new value of snmp_fileio_debug flag
*
* @returns none
*
* @end
*
*********************************************************************/
void snmp_fileioDebug(L7_BOOL val)
{
  snmp_fileio_debug = val;
  return;
}


