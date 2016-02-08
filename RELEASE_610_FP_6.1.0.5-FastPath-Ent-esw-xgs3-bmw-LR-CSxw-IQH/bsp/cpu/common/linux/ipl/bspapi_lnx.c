/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   bspapi_lnx.c
*
* @purpose    BSP API
*
* @component  bsp component
*
* @comments   none
*
* @create     12/12/2006
*
* @author     Brady Rutherford
*
* @end
*             
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "l7_common.h"
#include "bspapi.h"

/***************************************************************************
*
* @purpose Return file descriptor for the console port.
*
* @comments Return file descriptor for the console port.
*
* @returns file descriptor for the console port.
*
* @end
*
***************************************************************************/
static int consoleInputHandle = STDIN_FILENO;
static int consoletOutputHandle = STDOUT_FILENO;

int bspapiConsoleFdGet(void)
{
  return consoleInputHandle;
}

int bspapiConsoleFdOutGet(void)
{
  return consoletOutputHandle;
}

/***************************************************************************
*
* @purpose Sets file descriptor for the console port.
*
* @comments Sets file descriptor for the console port.
*
* @returns L7_SUCCESS
*
* @end
*
***************************************************************************/
L7_RC_t bspapiConsoleFdSet(int consoleFd)
{
  consoleInputHandle = consoleFd;
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Read the Real Time clock value
*
* @param    time ptr where to write time values
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes This function actually returns the system time which should have
*        been set based on the RTC when the kernel booted. This avoids
*        the overhead of forking a process to read the RTC.
*
* @end
*
*************************************************************************/
L7_RC_t bspapiRTCRead(struct tm *rtcTime)
{

  L7_RC_t   rc = L7_FAILURE;
  time_t    now;

  time(&now);
  localtime_r(&now, rtcTime);

  rc = L7_SUCCESS;
  return rc;
}

/**************************************************************************
*
* @purpose  This function prints on the terminal VPD of the code stored in FLASH
*
* @param    none.
*
* @returns  none.
*
* @comments none.
*
* @end
*
*************************************************************************/

void print_vpd_in_flash ()
{

   char buf[0x80];

   /* Build system command to print VPD file */
   snprintf(buf, sizeof(buf), "cat %s%s", EXEC_PATH, VPD_FILE);

   if (WEXITSTATUS(system(buf)) != 0) {

      /* indicate failure... */
      printf("Unable to print VPD information!");

   }

}


/**************************************************************************
*
* @purpose  Returns the startType for how this code was started.
*
* @param    
*
* @returns  startType
*
* @end
*
*************************************************************************/
L7_uint32 bspapiStartTypeGet()
{
  return START_OPR_CODE;
}
