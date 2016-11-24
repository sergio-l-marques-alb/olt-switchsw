/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  logNv.c
*
* @purpose   Store & Retrive the log NV buffer
*
* @component hardware
*
* @comments  none
*
* @create    09/12/2005
*
* @author    bradyr
*
* @end
*
*********************************************************************/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "l7_common.h"
#include "platform_cpu.h"
#include "bspcpu_api.h"
#include "log_api.h"

/*******************************************************************************
** NAME: logNvStore 
**
**  Platform-specific function to store log entry in NVRAM.
**
*******************************************************************************/
void logNvStore(L7_uchar8 *buf)
{
  int fd;

  /* This sample code stores the error buffer in the clock memory.
  */
  fd = open("/dev/nvram",O_RDWR);
  if(fd < 0)
  {
     printf("Unable to open /dev/nvram (not a fatal error)\n");
     return;
  }
  if(lseek(fd,L7_LOG_BUFFER_OFFSET,SEEK_SET) < 0)
  {
     printf("Cannot seek to addr in /dev/nvram\n");
     close(fd);
     return;
  }
  if(write(fd, (void *)buf, LOG_CALLOUT_BUF_SIZE) < 0)
  {
     printf("Cannot write to /dev/nvram\n");
     close(fd);
     return;
  }
  close(fd);
}

/*******************************************************************************
** NAME: logNvRetrieve 
**
**  Platform-specific function to retrieve log entry from NVRAM.
**  If BSP can't get the data then the buffer should not be touched.
**
*******************************************************************************/
void logNvRetrieve(L7_uchar8 *buf)
{
  int fd;

  /* This sample code stores the error buffer in NVRAM at address 0x0450.
  ** This address is normally preserved accross warm resets. 
  */
  fd = open("/dev/nvram",O_RDONLY);
  if(fd < 0)
  {
     printf("Unable to open /dev/nvram (not a fatal error)\n");
     return;
  }
  if(lseek(fd,L7_LOG_BUFFER_OFFSET,SEEK_SET) < 0)
  {
     printf("Cannot seek to addr in /dev/nvram\n");
     close(fd);
     return;
  }
  if(read(fd, (void *)buf, LOG_CALLOUT_BUF_SIZE) < 0)
  {
     printf("Cannot read from /dev/nvram\n");
     close(fd);
     return;
  }
  close(fd);
}
