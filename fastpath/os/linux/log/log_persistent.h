/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @file     log_persistent.h
*
* @purpose   Log server persistent file access and
*            manipulation declarations
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

#ifndef __LOG_PERSISTENT_H__
#define __LOG_PERSISTENT_H__


/* Utility routine to show the current persistent log contents. */
extern void logPersistentShow();

/* Clear the persistent log stat. */
extern void logPersistentStatsClear();

/* Get the persistent log stat. */
extern L7_RC_t logServerLogPersistentLogCountGet(L7_uint32 * count);

/* Get the persistent log stat. Utility */
extern L7_uint32 logServerPersistentLogMessageCount();

/* Write the contents of the persistent log files to an ASCII file. */
extern L7_RC_t logWritePersistentLogsToFile(L7_char8 * outFileName);

/* Shuffle the files, i.e. remove the oldest, rename the next oldest
* to oldest, etc. This function is located in log.c.
*/
extern L7_RC_t logShuffleFiles();

/* Write a log message to flash */
extern L7_RC_t logWriteMsgToFlash(L7_char8 * buf, L7_int32 len);

/* Write the contents of the persistent log files
* (all of them) onto the console
*/
extern  L7_RC_t logWritePersistentLogsToConsole();

/* Initialize the persistent log module. */
extern void logPersistentInit();

/* Get the specified row from the persistent log. */
L7_RC_t logServerLogMsgPersistentGet(L7_uint32 unitIndex,
                            L7_uint32 ndx, L7_char8 * buf);

/* Get the next row after the specified row from the persistent log. */
L7_RC_t logServerLogMsgPersistentGetNext(L7_uint32 unitIndex,
                                        L7_uint32 ndx, L7_char8 * buf,
                                        L7_uint32 * bufNdx);

/* Copy the remote logs from the specified unit to the local unit. */
L7_RC_t logServerLogMsgPersistentRemoteRetrieve(L7_uint32 unit);

#endif
