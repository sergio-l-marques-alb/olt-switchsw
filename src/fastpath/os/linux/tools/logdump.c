/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    lvl7_bin_to_asc.c
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
#define _L7_OS_LINUX_

#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <commdefs.h>
#include <datatypes.h>
#include "l7_product.h"
#include <log_api.h>

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
      {
        size += write(ofd, buf, strlen(buf));
      }
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
  L7_char8  buf[1024];  /* This must be large to accomodate 1024 byte padding */
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
    write (fd, buf, pad);
  }
  close(fd);
  return L7_SUCCESS;
}

L7_int32 main(int argc, char *argv[])
{
  if(logWritePersistentLogsToFile(ASCII_LOG_FILE_NAME) != L7_SUCCESS)
  {
    printf("Error in copy log file to ascii file\r\n");
    return(L7_ERROR);
  }
  printf("Log contents written to %s\r\n", ASCII_LOG_FILE_NAME);
  return(L7_SUCCESS);
}
