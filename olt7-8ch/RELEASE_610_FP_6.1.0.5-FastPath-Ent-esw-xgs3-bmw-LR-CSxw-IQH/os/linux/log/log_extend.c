
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    log_extend.c
*
* @purpose     To manage FASTPATH event logs, and event log variations
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
#include <registry.h>
#include <sysapi.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <log.h>
#include <log_ext.h>
#include <sys/types.h>
#include <unistd.h>
#include "l7utils_api.h"

#define LOG_BLANK_ENTRY  0xFFFFFFFF 
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


/* FLASH log definitions.
*/
static struct logformat_event *Log_Ptr = L7_NULLPTR;

static long Log_Index;     /* index into flash log */

/* User function to reset hardware.
*/
static void (* Box_Reset)();


/* Parameters passed on initialization call.
*/
static unsigned long Log_to_FLASH;
static unsigned long Log_to_TTY;
static unsigned long Log_to_RAM;

/* These variable control logging a single event to TTY.
*/

/* Maximum number of entries in event log.
*/
static unsigned long Log_Max_Entries;
static unsigned long EventLogSize;
static unsigned long LogInitialized = 0;


/* forward declaration.
*/
static void create_event_log_entry (unsigned long entry_format,
                                    char * file_name, unsigned long line,
                                    unsigned long error_code,
                                    struct logformat_event * event_record);


/* Event number transmitted to workstation.
** This information helps the workstation to detect missing events.
*/
static unsigned long Xmit_Event_Number;

/* Array for storing trace messages in RAM */

/* Format for a log entry is as follows:
 * +-----------------------------------------+
 * | Timestamp (4 bytes ) | Data (124 bytes) |
 * +-----------------------------------------+
 */


void *logMutex = 0;


L7_int32 create_ascii_event_log(L7_char8 *event_log_file_name);


/**************************************************************************
*
* @purpose  This routine erases the event (flash) log.
*
* @param    none.
*
* @returns  Index to start of FLASH log
*
* @end
*
*************************************************************************/

long
erase_flash_log(void)
{
	/*
	Set log file to be all empty entries
	*/

	memset ((void *)Log_Ptr, 0xFF, (Log_Max_Entries * sizeof (struct logformat_event)) - 1);
	osapiFsWrite (LOG_FILE_NAME, (L7_char8 *)Log_Ptr,
				  (L7_int32)(Log_Max_Entries * sizeof (struct logformat_event)));

	/*
	Return the pointer to the start of the log
	*/

	return(0);

} /* erase_flash_log */


/**************************************************************************
*
* @purpose  This routine adds an entry to the log
*
* @param    log_entry    Pointer to log entry
* @param    int_context  0-called from a thread 1-Called from an interrupt
*
* @comments This routine inserts new entry into FLASH log, and updates
* @comments counters. If log is full  the it is erased, however if function is
* @comments called from an interrupt then the log is not erased and new entry
* @comments is not added.
*
* @returns  none.
*
* @end
*
*************************************************************************/

void
add_flash_log_entry(struct logformat_event *log_entry,
					unsigned long int_context)
{
	if (Log_to_FLASH == LOG_DISABLE_FLASH)
	{
		return;
	}

	/*
	If we are inside an interrupt handler and the log is full then
	don't log. Erasing FLASH takes too much time.
	*/

	if (Log_Index == Log_Max_Entries)
	{
		if (int_context == L7_TRUE)
		{
			return;
		}

		Log_Index = erase_flash_log();
	}

	if (int_context == L7_TRUE)
	{
		/*
		Don't write log file entry if in interrupt context
		*/

		return;
	}

	memcpy((void *)&Log_Ptr[Log_Index], (void *)log_entry, sizeof(struct logformat_event));

        osapiFsWrite (LOG_FILE_NAME, (L7_char8 *)Log_Ptr,
          (L7_int32)(Log_Max_Entries * sizeof(struct logformat_event)));
        
	Log_Index++;

	return;

} /* add_flash_log_entry */


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

void
Log_Init(unsigned long event_log_size,
		 void (*box_reset)(void),
		 unsigned long log_to_flash,
		 unsigned long log_to_tty,
		 unsigned long log_to_ram)
{
	long		i, j;
	L7_int32	fd;

	if (Log_Ptr != L7_NULLPTR)
	{
		LOG_MSG("Log_Init: Re-initializing logs, old size %d, new size %d\n",
				EventLogSize, event_log_size);

		if (event_log_size != EventLogSize)
		{
			osapiFree(L7_LOG_COMPONENT_ID, Log_Ptr);
			Log_Ptr = L7_NULLPTR;
		}
	}

	if (Log_Ptr == L7_NULLPTR)
	{
		Log_Ptr = (struct logformat_event *) osapiMalloc(L7_LOG_COMPONENT_ID, event_log_size);

		if (Log_Ptr == L7_NULLPTR)
		{
			LOG_MSG("Log_Init: Cannot malloc enough memory to create log\n");
			return;
		}
	}

	Log_Max_Entries = event_log_size / sizeof (struct logformat_event);
	EventLogSize    = event_log_size;
	Box_Reset       = (void(*)) box_reset;

	Log_to_FLASH    = log_to_flash;
	Log_to_TTY      = log_to_tty;
	Log_to_RAM      = log_to_ram;

	/*
	If a previously saved flash log exists, try to read it in
	*/

	if (osapiFsRead(LOG_FILE_NAME, (L7_char8 *)Log_Ptr,
					(L7_int32) event_log_size) == L7_ERROR)
	{
		L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
            "Log_Init: log file error - creating new log file."
            " This pertains to the “event log” persistent file "
            "in flash. Either it did not exist, or had a bad checksum.");

		if (osapiFsFileCreate(LOG_FILE_NAME, &fd) == L7_FAILURE)
		{
			/*
			If a new log file can't be created, reset the box
			*/

			Box_Reset();
		}

		(void)osapiFsClose(fd);
	}

	/*
	Set counter of transmitted events to 0.
	*/

	Xmit_Event_Number = 0;

	if (Log_to_FLASH != LOG_DISABLE_FLASH)
	{
		/*
		Find first empty log record in FLASH log.
		*/

		for (i = 0; (i < Log_Max_Entries) &&
			  (Log_Ptr[i].entry_format != LOG_BLANK_ENTRY); i++);

		/*
		Verify that we don't have an uninitalized log or a log
		that is completely used. If so, erase it.
		*/

		if (i == Log_Max_Entries)
		{
			L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
                "Log_Init: Flash (event) log full; erasing."
                " Event log file has been cleared; happens at boot time.");
			Log_Index = erase_flash_log ();
		}
		else
		{
			/*
			Verify that we don't have any used entries after last unused record.
			If such entries are found then we have a corrupt event log. The
			whole event log is erased.
			*/

			for (j = i; (j < Log_Max_Entries) &&
				   (Log_Ptr[j].entry_format == LOG_BLANK_ENTRY); j++);

			if (j != Log_Max_Entries)
			{
				L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
                    "Log_Init: Corrupt event log; erasing."
                    " Event log file had a non-blank entry "
                    "after a blank entry; therefore, something was messed up.");
				Log_Index = erase_flash_log();
			}
			else
			{
				Log_Index = i;
			}
		}

	}

	LogInitialized = 1;

	return;

} /* Log_Init */


/**************************************************************************
** NAME: create_event_log_entry
**
** This function initializes fields of an event log entry.
**
** INPUT:
**  entry_format - EVENT/ERROR
**  file_name - Source file name where error occured.
**      line - Line number
**      error_code - 4-byte error code.
**
** INPUT/OUTPUT:
**  event_record - Pointer to memory which will hold the event record.
**
**************************************************************************/

static void
create_event_log_entry(unsigned long entry_format,
								   char *file_name,
								   unsigned long line,
								   unsigned long error_code,
								   struct logformat_event *event_record)
{
	long			i;
	unsigned long	file_name_size;

	l7utilsFilenameStrip(&file_name);
	file_name_size = strlen (file_name);

	/*
	File name is stored in the error log as a non-terminated array, and
	is truncated (or padded with blanks) to fit in the available space.
	*/

	for (i = 0; i < LOG_FILE_NAME_SIZE; i++)
	{
		if (i < file_name_size)
		{
			event_record->file_name[i] = file_name[i];
		}
		else
		{
			event_record->file_name[i]  = (unsigned char)' ';
		}
	}

	event_record->entry_format = entry_format;
	event_record->line_number = line;
	event_record->error_code = error_code;

	event_record->time_stamp = osapiUpTimeRaw();

	event_record->task_id =  osapiTaskIdSelf ();

	return;

} /* create_event_log_entry */


/**************************************************************************
** NAME: log_event
**
** This function records events in an error log.
** The function may NOT be called from an interrupt handler.
**
** INPUT:
**    file_name - String file identifier
**    line - Line number
**    error_code - 4-byte error code.
**
**************************************************************************/

void
log_event(char *file_name, unsigned long line, unsigned long error_code)
{
	struct logformat_event	event_record;

	/*
	Make sure that log_event is not called from an interrupt handler.
	*/

	if (osapiIntContext() == L7_TRUE)
	{
		Box_Reset();
	}

	/*
	Initialize log entry.
	*/

	create_event_log_entry(LOGFMT_EVENT, file_name,
						   line, error_code, &event_record);

	/*
	If error code indicates that event should be logged in FLASH then
	log it in FLASH.
	*/

	add_flash_log_entry (&event_record, L7_FALSE);

	return;

} /* log_event */


/**************************************************************************
** NAME: log_event_int
**
** This function records events in an error log.
** This function may be called from an interrupt handler or from an
** exception handler. If FLASH access semaphore is locked or event log
** is full then the function does not log an event.
**
** The function never sends events to the terminal.
**
** INPUT:
**    file_name - String file identifier
**    line - Line number
**    error_code - 4-byte error code.
**
**************************************************************************/

void
log_event_int(char * file_name, unsigned long line, unsigned long error_code)
{
	struct logformat_event	event_record;

	/*
	Initialize log entry.
	*/

	create_event_log_entry(LOGFMT_EVENT, file_name, line, error_code,
						   &event_record);

	/*
	If error code indicates that event should be logged in FLASH then
	log it in FLASH.
	*/

	add_flash_log_entry(&event_record, L7_TRUE);

	return;

} /* log_event_int */


/**************************************************************************
*
* @purpose  Retrieves the next event log entry
*
* @param    logPtr  pointer to an eventlog, usually Log_Ptr or remoteEventLog
* @param    index   @b{(input)} index to the next event log entry or NULL if requesting the first
* @param    pstring @b{(input)} ptr to place the formatted entry requested
*
* @returns  next index or NULL if last
*
* @comments for debugging purposes
*
* @end
*
*************************************************************************/

L7_uint32
L7_event_log_get_next(struct logformat_event *logPtr, L7_uint32 index,
					  L7_char8 *pstring)
{
	L7_ulong32	w32;
	L7_uint32	*pw32;
	L7_uint32	days;
	L7_ushort16	hours, minutes, seconds;
	L7_uchar8	*pc8;
	L7_int32	i;                        /* generic loop counter                 */
	L7_char8	buf[100];

	if (logPtr == L7_NULLPTR)
	{
		logPtr = Log_Ptr;
	}

	if (Log_to_FLASH == LOG_DISABLE_FLASH)
	{
		return (L7_NULL);
	}

	if (index == L7_NULL)
	{
		/* Find first empty log record in FLASH log. */

		for (i = 0; (i < (L7_int32) Log_Max_Entries) &&
			   (logPtr[i].entry_format != (L7_ulong32) LOG_BLANK_ENTRY); i++);

		if (i == 0)
		{
			return L7_NULL;
		}

		pw32 = (L7_uint32 *) (&logPtr[i-1]);
		index = (L7_uint32) (&logPtr[i-1]);
	}
	else
	{
		pw32 = (L7_uint32 *) index;
	}

	if (index == (L7_uint32) logPtr)
	{
		index = (L7_uint32) (&logPtr[Log_Max_Entries-1]);
	}
	else
	{
		index -= sizeof(struct logformat_event);
	}

	/*
	* get ENTRY type
	*/

	w32 = *pw32++;

	/*
	* valid or last entry?
	*/

	switch (w32)
	{
		/*
		"EVENT" entry, read it and
		send out formatted.
		*/

		case LOGFMT_EVENT:
		case LOGFMT_ERROR:
			if (w32 == LOGFMT_EVENT)
			{
				sprintf(pstring, "EVENT> ");           /* tell "event" */
			}
			else
			{
				sprintf(pstring, "ERROR> ");           /* tell "error" */
			}

			pc8 = (L7_uchar8 *) pw32;
			/* Note: file name (per pc8) is NOT null-terminated here! */
			memcpy(buf, pc8, LOG_FILE_NAME_SIZE);    /* copy name to buf */
			buf[LOG_FILE_NAME_SIZE] = L7_EOS;
			strncat(pstring, buf, LOG_FILE_NAME_SIZE);  /* write name */
			pc8 += (L7_uchar8)LOG_FILE_NAME_SIZE;

			pw32 = (L7_uint32 *) pc8;
			w32 = *pw32++;  /* Line number */
			sprintf(buf, " %4ld ", (L7_long32) w32);
			strcat(pstring, buf);
			w32 = *pw32++;   /* taskID */
			sprintf(buf, "%08lX ", w32);
			strcat(pstring, buf);
			w32 = *pw32++;   /* error code */
			sprintf(buf, "%08lX", w32);
			strcat(pstring, buf);

			w32 = *pw32++;                              /* time           */

			days    = w32 / SECONDS_PER_DAY;
			w32     = w32 % SECONDS_PER_DAY;
			hours   = (L7_ushort16) (w32 / SECONDS_PER_HOUR);
			w32     = w32 % SECONDS_PER_HOUR;
			minutes = (L7_ushort16) (w32 / SECONDS_PER_MINUTE);
			w32     = w32 % SECONDS_PER_MINUTE;
			seconds = (L7_ushort16) w32;

			sprintf(buf, "      %4d %2d %2d %2d\r\n", (L7_int32)days,
					(L7_int32)hours, (L7_int32)minutes, (L7_int32)seconds);
			strcat(pstring, buf);

			break;

		/*
		"FAULT" entry, read it and
		send out formatted.
		*/

		case LOGFMT_MACH_CHECK:
		case LOGFMT_WATCHDOG:
			if (w32 == LOGFMT_MACH_CHECK)
			{
				sprintf(pstring, "Machine Check > ");
			}
			else
			{
				sprintf(pstring, "Watchdog > ");
			}

			w32 = *pw32++;
			sprintf(buf, "srr0:%08lX ", w32);
			strcat (pstring, buf);
			w32 = *pw32++;
			/* sprintf(buf, "srr1: %08lX ", w32);
			strcat (pstring, buf); */
			w32 = *pw32++;
			/* sprintf(buf, "msr:%08lX ", w32);
			strcat (pstring, buf); */
			w32 = *pw32++;
			/* sprintf(buf, "dsisr:%08lX ", w32);
			strcat (pstring, buf); */
			w32 = *pw32++;
			sprintf(buf, "dar:%08lX\r\n", w32);
			strcat (pstring, buf);
			w32 = *pw32++;                              /* extra garabage */

			break;

		case LOG_BLANK_ENTRY:
			return (L7_NULL);
			break;

		default:
			sprintf(pstring, "CORRUPT ENTRY >>>>>>>");
			sprintf(buf, "%08lX ", w32);
			strcat (pstring, buf);
			w32 = *pw32++;
			sprintf(buf, "%08lX ", w32);
			strcat (pstring, buf);
			w32 = *pw32++;
			sprintf(buf, "%08lX ", w32);
			strcat (pstring, buf);
			w32 = *pw32++;
			sprintf(buf, "%08lX ", w32);
			strcat (pstring, buf);
			w32 = *pw32++;
			sprintf(buf, "%08lX ", w32);
			strcat (pstring, buf);
			w32 = *pw32++;
			sprintf(buf, "%08lX\r\n", w32);
			strcat (pstring, buf);
			w32 = *pw32++;                              /* extra garabage */
	}

  return (index);

} /* L7_event_log_get_next */


