/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename log_ext.h
*
* @purpose Contains definitions used by the extended logging implementation
*
* @component
*
* @comments  Not intended to be used by casual log users (e.g. LOG_MSG)
*
* @create
*
* @author various
*
* @end
*
**********************************************************************/


#ifndef LOG_EXT_H_INCLUDED
#define LOG_EXT_H_INCLUDED

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "l7_common.h"

/* Maximum size of file name stored in the log entry. Keep the size word aligned.
*/
#define LOG_FILE_NAME_SIZE  32 

/*-------------------------------------------------------------------*/
/*
 *  EXTENDED LOG STRUCTURES
 *
 */
/*-------------------------------------------------------------------*/
                           
/* This structure is used for errors reported in-line.
*/
  struct logformat_event
  {
    unsigned long entry_format;
    unsigned char file_name[LOG_FILE_NAME_SIZE];
    unsigned long line_number;
    unsigned long task_id;
    unsigned long error_code;
    time_t        time_stamp; /* Number of seconds since IPL */
  };

/* This structure is used to report processor faults.
*/
  struct logformat_fault
  {
    unsigned long entry_format;
    unsigned long srr0;
    unsigned long srr1;
    unsigned long msr;
    unsigned long dsisr;
    unsigned long dar;
    time_t        time_stamp;
  };



/*-------------------------------------------------------------------*/
/*
 *  EXTENDED LOG FUNCTION PROTOTYPES  
 *
 */
/*-------------------------------------------------------------------*/
/**************************************************************************
*
* @purpose  Initialize the logging facility
*
* @param event_log_size   Number of bytes reserved for log.
* @param box_reset        Pointer to user defined function that resets hardware.
* @param log_to_flash     LOG_ENABLE_FLASH - Log events to FLASH LOG_DISABLE_FLASH - Don't access FLASH
* @param log_to_tty       LOG_ENABLE_TTY - Display events on TTY LOG_DISABLE_TTY - Don't log to tty
*
* @comments If fatal error caused this re-IPL then store this error in FLASH event log.
*
* @returns  none.
*
* @end
*
*************************************************************************/
extern void Log_Init ( unsigned long flash_event_log_size,
                       void (*box_reset)(void),
                       unsigned long log_to_flash,
                       unsigned long log_to_tty,
                       unsigned long log_to_ram);


/**************************************************************************
*
* @purpose	Retrieves the next event log entry
*
* @param    index   @b{(input)} index to the next event log entry or NULL 
*                               if requesting the first
* @param    pstring @b{(input)} ptr to place the formatted entry requested
*
* @returns  next index or NULL if last
*
* @comments for debugging purposes
*     
* @end
*
*************************************************************************/
extern L7_uint32 L7_event_log_get_next (struct logformat_event *logPtr, 
                                        L7_uint32 index, L7_char8 *pstring);




#ifdef __cplusplus
}
#endif


#endif
