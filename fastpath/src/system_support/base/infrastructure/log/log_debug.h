/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @file     log_debug.h
*
* @component LOG
*
* @comments
*
* create 19/10/2004
*
* author hkumar
*
**********************************************************************/

#ifndef __LOG_DEBUG__
#define __LOG_DEBUG__ 


/* It writes out file contents. No attempt is made to locate the
*  "first" record for fifo log files. The records are printed
*  based on file order. Records are checked for sanity.
*  Invalid records are silently ignored.
*
void logPrintFileContents(L7_char8 * fileName);*/

/*
* Restores log configuration to defaults
*/
L7_RC_t logRestoreDefaults();

/*
*  Read LOG config data
*/
L7_RC_t logConfigRead();

/*
* Saves log configuration
*/
L7_RC_t logSave();

#endif
