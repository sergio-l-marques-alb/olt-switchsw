/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @file     log_persistent.c
*
* @purpose   Log server persistent file access and manipulation
*
* @component LOG
*
* @comments
*
* create 2/3/2004
*
* author Mclendon
*
**********************************************************************/


#include "l7_common.h"
#include "l7_resources.h"
#include "osapi.h"
#include "osapi_support.h"
#include "flex.h"
#include "unitmgr_api.h"
#include "log_api.h"
#include "log_persistent.h"
#include "fftp_api.h"


#if (L7_PERSISTENT_LOG_SUPPORTED == L7_TRUE)

/* The current startup and operation log file names */
/* These are initialized by logPersistentInit. */
extern L7_char8 startupLogName[L7_LOG_MAX_FILENAME_LEN];
extern L7_char8 operationLogName[L7_LOG_MAX_FILENAME_LEN];

static struct remoteLog_s
{
  L7_char8    remoteStartupLogName[L7_LOG_MAX_FILENAME_LEN];
  L7_char8    remoteOperationLogName[L7_LOG_MAX_FILENAME_LEN];
  L7_uint32   startupSize;
  L7_uint32   operationSize;
} remoteLog[L7_MAX_UNITS_PER_STACK + 1];


/**********************************************************************
* @purpose  Get the "next" row in the persistent log table.
*
* @param    ndx @b{(input)} the putative index of a log table entry.
* @param    buf @b{(output)} a buffer of at least
*                 L7_LOG_MESSAGE_LENGTH bytes into which the message is
*                 written.
* @param    bufNdx @b{(output)} the true index in the log
*                               table of the returned buffer.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
* @returns  L7_NOT_EXIST if ndx parameter is equal to or grater than
*           the index of the last entry of the log.
*
* @notes    The indices are continuously updated as messages are logged.
*           Therefore, a slow reader and a fast writer (lot's of logging)
*           may continuously retrieve the oldest message (which will not be
*           the same).
*           Use an ndx parameter of 0 to retrieve the oldest message
*           in the log. The startup log and the operation log are
*           treated as one log by this routine. Thus there will likely
*           be a gap in the timestamps and the indices for logged
*           messages on systems that have been running for some
*           period of time.
*
* @end
*********************************************************************/


L7_RC_t logServerLogMsgPersistentGetNext(L7_uint32 unitIndex, L7_uint32 ndx,
                      L7_char8 * buf, L7_uint32 * bufNdx)
{
  L7_char8 *  fileName = L7_NULL;
  L7_uint32   offset;
  L7_uint32   msgNdx;
  L7_uint32   mc = logServerPersistentLogMessageCount();
  L7_int32    fd;
  L7_uint32   unit;

  if (buf == L7_NULL)
    return L7_ERROR;

#ifdef L7_STACKING_PACKAGE
  if (unitIndex == L7_ALL_UNITS)
  {
    unit = unitIndex;
  }
  else
  {
    if (unitMgrNumberGet(&unit) != L7_SUCCESS)
      return L7_ERROR;

    if (unitIndex != unit)
    {
      mc = (remoteLog[unit].startupSize + remoteLog[unit].operationSize) /
                        L7_LOG_PERSISTENT_MESSAGE_LENGTH;
    }
  }
#else
  unit = unitIndex;
#endif

  /* We do not increment index since ndx is "off by one" */

  /* Handle messages in the startup log - these are easy. */
  if (ndx < L7_LOG_PERSISTENT_STARTUP_LOG_COUNT)
  {
    if (ndx >= mc)
    {
      return L7_NOT_EXIST;
    }
    else
    {
      *bufNdx = ndx + 1;
    }
    fileName = (unitIndex != unit) ? remoteLog[unit].remoteStartupLogName : startupLogName;
    offset = ndx * L7_LOG_PERSISTENT_MESSAGE_LENGTH;
  }
  else
  {
    L7_uint32 currentPos = (mc - L7_LOG_PERSISTENT_STARTUP_LOG_COUNT)
                      % L7_LOG_PERSISTENT_OPERATION_LOG_COUNT;

    /* Can't be the last message or past the last message. */
    if (ndx >= mc)
      return L7_NOT_EXIST;

    /* Adjust indices less than our lowest index to the lowest index. */
    /* Adjust indices within our index range to the next index. */
    if ((ndx + L7_LOG_PERSISTENT_OPERATION_LOG_COUNT) <= mc)
    {
      /* Set to first message. */
      ndx = (mc - L7_LOG_PERSISTENT_OPERATION_LOG_COUNT) + 1;
    }
    else
    {
      /* Within range - increment. */
      ndx++;
    }

    /* We scale the logMessagesReceived to be mc - 1, i.e. the last message */
    if (mc <= L7_LOG_PERSISTENT_OPERATION_LOG_COUNT)
      msgNdx = ndx - L7_LOG_PERSISTENT_STARTUP_LOG_COUNT - 1;
    else
      msgNdx = (currentPos + (L7_LOG_PERSISTENT_OPERATION_LOG_COUNT - 1))
                        - (mc - ndx);

    if (msgNdx >= L7_LOG_PERSISTENT_OPERATION_LOG_COUNT)
      msgNdx -= L7_LOG_PERSISTENT_OPERATION_LOG_COUNT;

    fileName = (unitIndex != unit) ? remoteLog[unit].remoteOperationLogName : operationLogName;

    offset = msgNdx * L7_LOG_PERSISTENT_MESSAGE_LENGTH;
    *bufNdx = ndx;
  }
  fd = osapiFsOpen(fileName);
  if (fd != L7_ERROR)
  {
    osapiFileSeek(fd, offset, SEEK_SET);
    osapiFileRead(fd, buf, L7_LOG_PERSISTENT_MESSAGE_LENGTH);
    osapiFsClose(fd);
  }
  else
  {
    return L7_ERROR;
  }
  /* We could check that *buf == '<' here */
  return L7_SUCCESS;

}
/**********************************************************************
* @purpose  Get the specified row in the persistent log table.
*
* @param    ndx @b{(input)} - the putative index of a log table entry.
* @param    buf @b{(output)} - a buffer of at least
*                 L7_LOG_MESSAGE_LENGTH bytes into which the
*                 message is written.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
* @returns  L7_NOT_EXIST if ndx parameter does not identify an existing
*           row in the table(s).
*
* @notes    The indices are continuously updated as messages are logged.
*           On a system that has been operating for an extended
*           period of time, there will likely be a gap in the
*           indices of the startup table
*           (always [1..L7_LOG_PERSISTENT_STARTUP_LOG_COUNT]) and the
*           indices of the operation table.
*
* @end
*********************************************************************/

L7_RC_t logServerLogMsgPersistentGet(L7_uint32 unitIndex, L7_uint32 ndx,
                                    L7_char8 * buf)
{
  L7_char8 *  fileName = L7_NULL;
  L7_uint32   offset;
  L7_uint32   msgNdx;
  L7_int32    fd;
  L7_uint32   unit;

  L7_uint32   mc = logServerPersistentLogMessageCount();

  if (buf == L7_NULL)
    return L7_ERROR;

  if (ndx == 0)
    return L7_NOT_EXIST;

#ifdef L7_STACKING_PACKAGE
  if (unitIndex == L7_ALL_UNITS)
  {
    unit = unitIndex;
  }
  else
  {
    if (unitMgrNumberGet(&unit) != L7_SUCCESS)
      return L7_ERROR;

    if (unitIndex != unit)
    {
      mc = (remoteLog[unit].startupSize + remoteLog[unit].operationSize) /
                                      L7_LOG_PERSISTENT_MESSAGE_LENGTH;
    }
  }
#else
  unit = unitIndex;
#endif

  /* Convert to internal indexing */
  if (ndx <= L7_LOG_PERSISTENT_STARTUP_LOG_COUNT)
  {
    if (ndx > mc)
      return L7_NOT_EXIST;
    offset = (ndx - 1) * L7_LOG_PERSISTENT_MESSAGE_LENGTH;
    fileName = (unitIndex != unit) ? remoteLog[unit].remoteStartupLogName : startupLogName;
  }
  else
  {
    L7_uint32 currentPos = (mc - L7_LOG_PERSISTENT_STARTUP_LOG_COUNT)
                        % L7_LOG_PERSISTENT_OPERATION_LOG_COUNT;

    /* Can't be the last message or past the last message. */
    if ((ndx > mc) ||
        ((ndx + L7_LOG_PERSISTENT_OPERATION_LOG_COUNT) <= mc))
      return L7_NOT_EXIST;

    /* We scale the logMessagesReceived to be mc-1, i.e. the last message */
    msgNdx = (currentPos + (L7_LOG_PERSISTENT_OPERATION_LOG_COUNT - 1))
          - (mc - ndx);

    if (msgNdx >= L7_LOG_PERSISTENT_OPERATION_LOG_COUNT)
      msgNdx -= L7_LOG_PERSISTENT_OPERATION_LOG_COUNT;

    offset = msgNdx * L7_LOG_PERSISTENT_MESSAGE_LENGTH;
    fileName = (unitIndex != unit) ? remoteLog[unit].remoteOperationLogName : operationLogName;
  }
  fd = osapiFsOpen(fileName);
  if (fd != L7_ERROR)
  {
    osapiFileSeek(fd, offset, SEEK_SET);
    osapiFileRead(fd, buf, L7_LOG_PERSISTENT_MESSAGE_LENGTH);
    osapiFsClose(fd);
  }
  else
  {
    return L7_ERROR;
  }
  /* We could check that *buf == '<' here */
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Attempts to get the persistent logs from another unit
*
* @param    unit  @b {(input)} unit number
*
* @returns  L7_SUCCESS, if any log retreived
*
* @notes    Copies the remote persistent logs to the local maxchine
*
* @end
*********************************************************************/
L7_RC_t logServerLogMsgPersistentRemoteRetrieve(L7_uint32 unit)
{
  L7_RC_t rc = L7_SUCCESS;
#if defined (L7_STACKING_PACKAGE)
  L7_uchar8 localFileName[L7_LOG_MAX_FILENAME_LEN];
  L7_uchar8 remoteFileName[L7_LOG_MAX_FILENAME_LEN];

  L7_uint32 ndx;

  for (ndx = 0; ndx < L7_LOG_MAX_PERSISTENT_LOGS; ndx++)
  {
    sprintf(remoteFileName, L7_LOG_REMOTE_PERSISTENT_STARTUP_FILE_MASK, ndx, unit);
    sprintf(localFileName, L7_LOG_REMOTE_PERSISTENT_STARTUP_FILE_MASK, ndx, unit);

    rc = fftpFileRequest(remoteFileName, unit, 0, 0, FFTP_ENTIRE_FILE,
                            500, localFileName);

    if ((rc != L7_SUCCESS) && (ndx == 0))
      return L7_FAILURE;

    sprintf(remoteFileName, L7_LOG_REMOTE_PERSISTENT_OPERATION_FILE_MASK, ndx, unit);
    sprintf(localFileName, L7_LOG_REMOTE_PERSISTENT_OPERATION_FILE_MASK, ndx, unit);

    rc = fftpFileRequest(remoteFileName, unit, 0, 0, FFTP_ENTIRE_FILE,
                            500, localFileName);

    if ((rc != L7_SUCCESS) && (ndx == 0))
      return L7_FAILURE;

    /* Store the name and sizes away. */
    sprintf(remoteLog[unit].remoteStartupLogName, L7_LOG_REMOTE_PERSISTENT_STARTUP_FILE_MASK, 0, unit);
    sprintf(remoteLog[unit].remoteOperationLogName, L7_LOG_REMOTE_PERSISTENT_OPERATION_FILE_MASK, 0, unit);

    /* Presumably, the remote files have been gotten. */
    if (osapiFsFileSizeGet(remoteLog[unit].remoteStartupLogName,
              &remoteLog[unit].startupSize) != L7_SUCCESS)
    {
      remoteLog[unit].operationSize = remoteLog[unit].startupSize = 0;
      return L7_ERROR;
    }
    if (osapiFsFileSizeGet(remoteLog[unit].remoteOperationLogName,
              &remoteLog[unit].operationSize) != L7_SUCCESS)
    {
      remoteLog[unit].operationSize = remoteLog[unit].startupSize = 0;
      return L7_ERROR;
    }
  }
#endif
  return rc;
}

/**********************************************************************
* @purpose  Initialize the persistent log utilities.
*
* @notes    Sets the "current" startup and operations log file names.
*           Rotates (or "shuffles") persistent log file versions
*
* @end
*********************************************************************/
void logPersistentInit()
{
  /* Write the persistent log names. */
  sprintf(startupLogName, L7_LOG_PERSISTENT_STARTUP_FILE_MASK, 0);
  sprintf(operationLogName, L7_LOG_PERSISTENT_OPERATION_FILE_MASK, 0);
  logShuffleFiles();
}




/**************************************************************************
*
* @purpose Write the contents of a named log files using sysapiPrintf.
*
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
static void logPrintFileContents(L7_char8 * fileName)
{
  L7_RC_t   rc;
  L7_char8  buf[L7_LOG_PERSISTENT_MESSAGE_LENGTH];

  L7_int32  fd = osapiFsOpen(fileName);
  if (fd)
  {
    while (1)
    {
      rc = osapiFileRead(fd, buf, L7_LOG_PERSISTENT_MESSAGE_LENGTH);
      if (rc == L7_ERROR)
        break;
      if (buf[0] == '<')
      {
        sysapiPrintf(buf);
      }
    }
    osapiFsClose(fd);
  }
  else
  {
    sysapiPrintf("logPrintfFileContents: File open for %s returned errno %d\n",
              fileName, osapiErrnoGet());
  }
}

/*================= PERSISTENT LOG DEBUG ROUTINES ========================*/

/**************************************************************************
*
* @purpose Print the messages in the persistent log
*
* @param   void
*
* @comments none.
*
* @returns  none.
*
* @end
*
*************************************************************************/
void logPersistentShow()
{
  sysapiPrintf("\r\nStartup Log\r\n");
  logPrintFileContents(startupLogName);
  sysapiPrintf("\r\nOperation Log\r\n");
  logPrintFileContents(operationLogName);
}

         
/**************************************************************************
*
* @purpose Write the contents of the persistent log files.
*
* @comments It writes files - what can I say.
*
* @returns  none.
*
* @end
*
*************************************************************************/
L7_RC_t logWritePersistentLogsToConsole()
{
  L7_uint32 ndx;
  L7_char8  fileName[L7_LOG_MAX_FILENAME_LEN];

  if (L7_LOG_MAX_PERSISTENT_LOGS == 0)
    return L7_SUCCESS;

  /* Write the oldest log first. */
  for (ndx = L7_LOG_MAX_PERSISTENT_LOGS - 1; ndx >= 0; ndx--)
  {
    sprintf(fileName, L7_LOG_PERSISTENT_STARTUP_FILE_MASK, ndx);
    sysapiPrintf("Startup file N-%d - %s\r\n", ndx, fileName);


    logPrintFileContents(fileName);
    sysapiPrintf("\r\n\r\n\r\n");


    sprintf(fileName, L7_LOG_PERSISTENT_OPERATION_FILE_MASK, ndx);
    sysapiPrintf("Operation file N-%d - %s\r\n", ndx, fileName);

    logPrintFileContents(fileName);
    sysapiPrintf("\r\n\r\n\r\n");
  }
}


#else
/* No persistent log implemented - See above for comments for below functions. */


L7_RC_t logShuffleFiles()
{
  return L7_SUCCESS;
}
L7_RC_t logWriteMsgToFlash(L7_char8 * buf, L7_int32 len)
{
  return L7_SUCCESS;
}
L7_RC_t logWritePersistentLogsToConsole()
{
  return L7_SUCCESS;
}

void logPersistentInit() {}


L7_RC_t logServerLogMsgPersistentGet(L7_uint32 unitIndex, L7_uint32 ndx, L7_char8 * buf)
{
  return L7_NOT_EXIST;
}

L7_RC_t logServerLogMsgPersistentGetNext(L7_uint32 unitIndex, L7_uint32 ndx,
                      L7_char8 * buf, L7_uint32 * bufNdx)
{
  return L7_NOT_EXIST;
}


#endif
