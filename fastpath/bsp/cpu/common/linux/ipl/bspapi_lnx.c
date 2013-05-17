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
#include <fcntl.h>

#include "l7_common.h"
#include "bspapi.h"
#include "iplsec.h"
#include "osapi.h"
#include "osapi_support.h"
#include "stk.h"

extern L7_ushort16 file_crc_compute(L7_uchar8 *file_name, L7_uint32 file_size);
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

/**************************************************************************
 *
 * @purpose  Reads the header from STK files and validate it
 *
 * @param    file_name  Operational code file.
 *
 * @returns  none.
 *
 * @comments  This function understands STK files.
 *            STK files have the specified 'tags' in the header.
 *            This function validates the given image.
 *
 * @end
 *
 *************************************************************************/
L7_RC_t bspapiOprHeaderRead(char *fileName, oprHeader_t *opr)
{
  stkFileHeader_t stkHeader;
  stkOprFileInfo_t *oprHead = NULL;
  char buffer[STK_MAX_HEADER_SIZE];
  int filedesc;
  int i;
  int offset;
  L7_ushort16 computed_crc;
  L7_uint32 fileSize, fileSizeOnDisk;

  filedesc = open (fileName, O_RDWR, 0);
  if ( filedesc < 0 )
  {
    return L7_FAILURE;
  }

  /* read first few bytes to determine STK or OPR */

  i = read(filedesc, (void *)&stkHeader, sizeof(stkHeader));

  if (i < sizeof(stkHeader))
  {
    close(filedesc);
    return L7_FAILURE;
  }
  /* check if the file is an STK file */

  offset = 0;

  if((osapiNtohs(stkHeader.tag1) == STK_TAG1) && 
     (osapiNtohl(stkHeader.tag2) == STK_TAG2))
  {
    /* First, validate CRC */
    if (osapiFsFileSizeGet(fileName, &fileSizeOnDisk) != L7_SUCCESS) 
    {
      close(filedesc);
      return L7_FAILURE;
    }
    fileSize = osapiNtohl(stkHeader.file_size);
    if (fileSizeOnDisk < fileSize)
    {
      close(filedesc);
      return L7_FAILURE;
    }

    computed_crc = file_crc_compute(fileName, fileSize);
    if (computed_crc != osapiNtohs(stkHeader.crc))
    {
      close(filedesc);
      return L7_FAILURE;      
    }
    /* Sanity-check sizes */
    if ((osapiNtohl(stkHeader.num_components) > STK_MAX_IMAGES) || 
	(osapiNtohl(stkHeader.stk_header_size) > STK_MAX_HEADER_SIZE))
    {
      close(filedesc);
      return L7_FAILURE;      
    }
    /* rewind to the begining */

    lseek(filedesc, 0, SEEK_SET);

    if(0 > read(filedesc, &buffer[0], osapiNtohl(stkHeader.stk_header_size))){}

    /* check all available opr files to get the correct
     * opr file for this target
     */

    if(osapiNtohl(stkHeader.num_components) == 0)
    {
      close(filedesc);
      return L7_FAILURE;
    }

    for(i = 0; i < osapiNtohl(stkHeader.num_components); i++)
    {
      offset = (sizeof(stkHeader) + (i * sizeof (stkOprFileInfo_t)));
      oprHead = (stkOprFileInfo_t *)&buffer[offset];

      if((osapiNtohl(oprHead->target_device)) == bspapiIplModelGet())
      {
	if(osapiNtohl(oprHead->os) == STK_OS_LINUX)
	{
	  offset = oprHead->offset;
          memcpy(opr, oprHead, sizeof(stkOprFileInfo_t));
	  break;
	}
      }
    }

    if(i == osapiNtohl(stkHeader.num_components))
    {
      close(filedesc);
      return L7_FAILURE;
    }
  } 
  else /* STK tags not present */ 
  {
    close(filedesc);
    return L7_FAILURE;
  }
  close(filedesc);
  return L7_SUCCESS;
}

