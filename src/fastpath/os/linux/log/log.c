
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    log.c
*
* @purpose
*
* @component   hardware
*
* @create      03/20/98
*
* @author      ALT
*
* @end
*
*********************************************************************/

#include <l7_common.h>
#include <osapi.h>
#include <../osapi/osapi_priv.h>
#include <registry.h>
#include <sysapi.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <osapi_sem.h>

#include <log.h>
#include <log_api.h>
#include "log_persistent.h"
#include "l7_resources.h"

/* Terminator character for persistent logs messages */
#define TERMINATOR_LEN          3
static L7_char8 terminator[] = "\r\n\0";

/* The current startup and operation log file names */
/* These are initialized by logPersistentInit. */
L7_char8 startupLogName[L7_LOG_MAX_FILENAME_LEN];
L7_char8 operationLogName[L7_LOG_MAX_FILENAME_LEN];
/*
 *HANDY #DEFS
 */
#define PRINTMSG_SCREEN 1
#define PRINTMSG_FILE   2
#define PRINTMSG_BOTH   3
#define PRINTMSG_NONE   4

#define PRINTMSG_LOC PRINTMSG_BOTH

/*
 *MACROS for our stack trace
 */
#ifdef __GNUC__
#if (PRINTMSG_LOC == PRINTMSG_SCREEN)

#define PRINTMSG(f,s,a...) printf(s, ## a)

#elif (PRINTMSG_LOC == PRINTMSG_FILE)

#define PRINTMSG(f,s,a...) \
if(f != NULL) fprintf(f,s,## a)

#elif (PRINTMSG_LOC == PRINTMSG_BOTH)
#define PRINTMSG(f,s,a...) \
printf(s, ## a);\
if(f != NULL) fprintf(f,s,## a)

#elif (PRINTMSG_LOC == PRINTMSG_NONE)
#define PRINTMSG
#else
#error "Please select a valid value for PRINTMSG_LOC in log.c!"
#endif

#else
/*this is what we use if we are not using gnu C (__GNUC__)*/
/*other compilers may not support the ... construct in macros*/
#define PRINTMSG(f,s,a)
#endif


/* The count of messages logged into the current persistent logs
* (sum of startup and operation logs).
*/
static L7_uint32  logPersistentLogMessageCount = 0;

/*
 */
struct stack_frame_intro_s
{
  unsigned int *backchain;
  unsigned int last_link_register;
};


/**************************************************************************
*
* @purpose Write a message to the persistent storage
*
* @param buf @b {(input)} the input buffer
* @param len @b {(input)} length of the input buffer
*
* @notes  Updates the persistent message count.
*         This routine is thread safe and reentrant.
*
*
* @end
*
*************************************************************************/

L7_RC_t logWriteMsgToFlash(L7_char8 * buf, L7_int32 len)
{
  /* We are relying on the compile to put NULLs in here */
  static L7_char8 blank[L7_LOG_PERSISTENT_MESSAGE_LENGTH];
  L7_char8 * fileName;
  L7_uint32 offset;
  /* Open will create if necessary. */
  L7_int32  fd;
  L7_uint32 msgIndex;

  if ((buf == L7_NULL) || (len == 0))
    return L7_ERROR;

  /* TODO - grab a semaphore here */
  msgIndex = logPersistentLogMessageCount++;

  if (msgIndex < L7_LOG_PERSISTENT_STARTUP_LOG_COUNT)
  {
    fileName = startupLogName;
    offset = msgIndex * L7_LOG_PERSISTENT_MESSAGE_LENGTH;
  }
  else
  {
    fileName = operationLogName;
    offset = ((msgIndex - L7_LOG_PERSISTENT_STARTUP_LOG_COUNT) %
      L7_LOG_PERSISTENT_OPERATION_LOG_COUNT) *
      L7_LOG_PERSISTENT_MESSAGE_LENGTH;
  }
  fd = open(fileName, O_CREAT | O_WRONLY, 0644);
  if (fd != L7_ERROR)
  {
    if (len >= L7_LOG_PERSISTENT_MESSAGE_LENGTH)
      len = L7_LOG_PERSISTENT_MESSAGE_LENGTH - 3;
    lseek(fd, offset, SEEK_SET);
    /* Write the message */
    if(0 > write(fd, buf, len)){}
    /* Write the null terminator */
    if(0 > write(fd, &terminator, TERMINATOR_LEN)){}
    /* Write nulls to complete the record */
    if (L7_LOG_PERSISTENT_MESSAGE_LENGTH - (len + TERMINATOR_LEN) > 0)
    {
      if(0 > write(fd, blank, L7_LOG_PERSISTENT_MESSAGE_LENGTH - (len + TERMINATOR_LEN))){}
    }
    close(fd);
  }
  else
  {
    return L7_ERROR;
  }
  return L7_SUCCESS;
}

/****************************************************************
** @purpose: This function will dump information regarding the  call stack
** of the offending pid which received a fatal signal
*
* @param @b{(input)} pointer to stack frame
*
*
*****************************************************************/
void log_backtrace(unsigned int * raw_stack_pointer)
{
  struct stack_frame_intro_s *frame_info, *old_frame_info;
  char buf[128];
  int len = 0;

  frame_info = (struct stack_frame_intro_s *)raw_stack_pointer;
  old_frame_info = NULL;

  len = sprintf(buf, "<0> Process id %d", getpid());
  logWriteMsgToFlash(buf, len);
  for(;frame_info != NULL;frame_info = (struct stack_frame_intro_s *)frame_info->backchain)
  {
    len = sprintf(buf, "<0> last link register = 0x%x",(int)frame_info->last_link_register);
    logWriteMsgToFlash(buf, len);
    len = sprintf(buf, "<0> next stack frame located at 0x%x",(int)frame_info->backchain);
    logWriteMsgToFlash(buf, len);
  }
}

/**********************************************************************
* @purpose  get persistent log message count
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logServerLogPersistentLogCountGet(L7_uint32 * count)
{
  if (count == L7_NULL)
    return L7_ERROR;
  *count = logPersistentLogMessageCount;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  get startup log message count - utility function
*
* @returns  count of persistent messages
*
*
* @end
*********************************************************************/

L7_uint32 logServerPersistentLogMessageCount()
{
  return logPersistentLogMessageCount;
}

/**********************************************************************
* @purpose  clear persistent log stats - utility function
*
*
* @end
*********************************************************************/

void logPersistentStatsClear()
{
  logPersistentLogMessageCount = 0;
}

/**********************************************************************
* @purpose  clear persistent log buffer
*
* @end
*********************************************************************/

void logServerLogPersistentClear( void )
{
  if ( logPersistentLogMessageCount > L7_LOG_PERSISTENT_STARTUP_LOG_COUNT )
  {
    logPersistentLogMessageCount=0;
    remove(startupLogName );
    remove( operationLogName );
  }
  else
  {
      /* only startup log has been created */
      logPersistentLogMessageCount=0;
      remove(startupLogName );
  }
}

/**************************************************************************
*
* @purpose Write the contents of a named log files using the specified file
*           descriptor.
*
* @param   fd @b{(input)} The output file descriptor
* @param   fileName @b{(input)} The file name containing the contents to write.
* @param   numRecords @b{(input)} The number of records to print.
*
* @comments It writes out file contents. No attempt is made to locate the
*           "first" record for fifo log files. The records are printed
*           based on file order. Records are checked for sanity.
*           Invalid records are silently ignored.
*
* @returns  none.
*
* @end
*
*************************************************************************/


static L7_uint32 logWriteFileContents(L7_int32 ofd, L7_char8 * fileName,
                                        L7_uint32 numRecords)
{
  L7_int32  ndx;
  L7_int32  rc;
  L7_uint32 size = 0;
  L7_char8  buf[L7_LOG_PERSISTENT_MESSAGE_LENGTH];

  L7_int32  fd = open(fileName, O_RDONLY, 0);
  if (fd)
  {
    for (ndx = 0; ndx < numRecords; ndx++)
    {
      rc = read(fd, buf, L7_LOG_PERSISTENT_MESSAGE_LENGTH);
      /* If we got the whole file - quit */
      if (rc <= 0)
        break;
      if (buf[0] == '<')
        size += write(ofd, buf, strlen(buf));
    }
    close(fd);
    return size;
  }
  return 0;
}


/**************************************************************************
*
* @purpose Write the contents of the persistent log files to an ASCII file.
*
* @comments It writes files - what can I say.
*
* @returns  none.
*
* @end
*
*************************************************************************/
L7_RC_t logWritePersistentLogsToFile(L7_char8 * outFileName)
{
  L7_uint32 ndx;
  L7_uint32 pad;
  L7_uint32 len = 0;
  L7_char8  fileName[L7_LOG_MAX_FILENAME_LEN];
  L7_char8  buf[1024];
  L7_int32  fd;
  L7_uint32 fileSize = 0;

  if (L7_LOG_MAX_PERSISTENT_LOGS == 0)
    return L7_SUCCESS;

  fd = open(outFileName, O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if (fd < 0)
    return L7_ERROR;

  /* Write the oldest log first. */
  for (ndx = L7_LOG_MAX_PERSISTENT_LOGS - 1; ndx >= 0; ndx--)
  {
    sprintf(fileName, L7_LOG_PERSISTENT_STARTUP_FILE_MASK, ndx);

    len = sprintf(buf, "- Startup Log N-%d - %s\r\n", ndx, fileName);
    fileSize += write(fd, buf, len);
    fileSize += logWriteFileContents(fd, fileName, L7_LOG_PERSISTENT_STARTUP_LOG_COUNT);
    fileSize += write(fd, "\r\n\r\n", 4);

    sprintf(fileName, L7_LOG_PERSISTENT_OPERATION_FILE_MASK, ndx);
    len = sprintf(buf, "- Operation Log N-%d - %s\r\n", ndx, fileName);
    fileSize += write(fd, buf, len);
    fileSize += logWriteFileContents(fd, fileName, L7_LOG_PERSISTENT_OPERATION_LOG_COUNT);
    fileSize += write(fd, "\r\n\r\n", 4);
  }
    /* Now pad to 1024 byte boundary for xmodem. If tftp'ing, so what. */
  pad = 1024 - ( fileSize % 1024 );
  if (pad != 1024)
  {
    memset(buf, ' ', sizeof(buf));
    if(0 > write (fd, buf, pad)){}
  }
  close(fd);
  return L7_SUCCESS;
}



/**************************************************************************
*
* @purpose Shuffle the log files on system startup.
*
* @comments It munges files - what can I say.
*
* @returns  L7_SUCCESS is persistent log files created. L7_ERROR otherwise.
*
* @end
*
*************************************************************************/
L7_RC_t logShuffleFiles()
{
  static L7_BOOL logFilesShuffled = L7_FALSE;
  L7_int32  fd;
  L7_uint32 ndx;
  L7_char8  fileName[L7_LOG_MAX_FILENAME_LEN];
  L7_char8  fromFile[L7_LOG_MAX_FILENAME_LEN];
  L7_char8  toFile[L7_LOG_MAX_FILENAME_LEN];

  if ((L7_LOG_MAX_PERSISTENT_LOGS == 0) || (logFilesShuffled == L7_TRUE))
    return L7_SUCCESS;
  logFilesShuffled = L7_TRUE;

  /* Delete the oldest log */
  /* It's ok if these calls fail. These files may not exist. */
  sprintf(fileName, L7_LOG_PERSISTENT_STARTUP_FILE_MASK,
        L7_LOG_MAX_PERSISTENT_LOGS-1);
  remove(fileName);

  sprintf(fileName, L7_LOG_PERSISTENT_OPERATION_FILE_MASK,
        L7_LOG_MAX_PERSISTENT_LOGS-1);
  remove(fileName);

  /* Shuffle up the logs from the oldest to the newest. */
  for (ndx = (L7_LOG_MAX_PERSISTENT_LOGS - 1); ndx > 0; ndx--)
  {
    sprintf(fromFile, L7_LOG_PERSISTENT_STARTUP_FILE_MASK, ndx - 1);
    sprintf(toFile, L7_LOG_PERSISTENT_STARTUP_FILE_MASK, ndx);
    rename(fromFile, toFile);

    sprintf(fromFile, L7_LOG_PERSISTENT_OPERATION_FILE_MASK, ndx - 1);
    sprintf(toFile, L7_LOG_PERSISTENT_OPERATION_FILE_MASK, ndx);
    rename(fromFile, toFile);
  }

  /* Create the newest logs. These files will be written to in this instantiation.
  * These files MUST be created. Failure is not an option.
   */
  fd = open(startupLogName, O_CREAT | O_TRUNC | O_RDWR, 0644);
  if (fd < 0)
  {
    return L7_ERROR;
  }
  else
  {
    close(fd);
  }

  fd = open(operationLogName, O_CREAT | O_TRUNC | O_RDWR, 0644);
  if (fd < 0)
  {
    return L7_ERROR;
  }
  else
  {
  close(fd);
  }
  return L7_SUCCESS;
}


#ifdef _USE_PTIN_LOG_MESSAGE_
/*Used to convert FP verbosity to PTIN verbosity*/
log_severity_t fp_to_ptin_logger_verbosity(L7_LOG_SEVERITY_t fp_verbosity)
{
  switch (fp_verbosity)
  {
  case L7_LOG_SEVERITY_EMERGENCY: return LOG_SEV_CRITICAL;
  case L7_LOG_SEVERITY_ALERT:     return LOG_SEV_CRITICAL;
  case L7_LOG_SEVERITY_CRITICAL:  return LOG_SEV_CRITICAL;
  case L7_LOG_SEVERITY_ERROR:     return LOG_SEV_ERROR;
  case L7_LOG_SEVERITY_WARNING:   return LOG_SEV_WARNING;
  case L7_LOG_SEVERITY_NOTICE:    return LOG_SEV_NOTICE;
  case L7_LOG_SEVERITY_INFO:      return LOG_SEV_INFO;
  case L7_LOG_SEVERITY_DEBUG:     return LOG_SEV_DEBUG;
  default: return LOG_OFF;
  }
}

/*Used to convert FP Component Id to PTIN. If the component is not defined by PTIN considered MISC*/
log_context_t fp_to_ptin_component(L7_COMPONENT_IDS_t fp_component)
{
  switch (fp_component)
  {
  case L7_DHCP_SNOOPING_COMPONENT_ID: return LOG_CTX_DHCP;
  case L7_SNOOPING_COMPONENT_ID:      return LOG_CTX_IGMP;  
  case L7_DAI_COMPONENT_ID:           return LOG_CTX_DAI;
  default: return LOG_CTX_MISC;
  }
}
#endif
