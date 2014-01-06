/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  cpu_utils.c
*
* @purpose   Provide cpu utility functions for the GTO CPU
*
* @component BSP
*
* @create    11/26/2007
*
* @author    Brady Rutherford
*
* @end
*
*********************************************************************/
#include <stdio.h>
#include <string.h>

#include "l7_common.h"
#include "platform_cpu.h"
#include "bspcpu_api.h"
#include "log_api.h"
#include "bspapi.h"
#ifdef _L7_OS_VXWORKS_
#include "sysNet.h"
#endif
#ifdef _L7_OS_LINUX_
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <stdlib.h>
#endif

#include "logger.h"
#include "readenv.h"  // PTin added: global vars

/**************************************************************************
* @purpose  Read the CPU's serial number. If the serial number cannot be read,
*           do not update serialString.
*
* @param    serialString - pointer to serial number string
*
* @returns  L7_SUCCESS - serial number read.
* @returns  L7_FAILURE - Unable to read the serial number.
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuReadSerialNum(L7_char8 *serialString)
{
  #if 0
  /* PTin added: global vars */
  open_bootenv();
  read_bootenv("serial#",serialString);
  close_bootenv();

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Serial# = \"%s\"",serialString);
  #endif

  return(L7_FAILURE);
}

/**************************************************************************
* @purpose  Read the CPU's MAC address. If MAC address cannot be read,
*           do not update macString.
*
* @param    macString - pointer to MAC address string
*
* @returns  L7_SUCCESS - MAC address read.
* @returns  L7_FAILURE - Unable to read the MAC address
*
* @comments This function returns the local MAC address stored in 
*           non-volatile storage. Most applications should call the
*           sim API simMacAddrGet to get the system MAC address for
*           the stack.
*
* @end
**************************************************************************/
L7_RC_t bspapiMacAddrGet(L7_char8 *macString)
{
  L7_RC_t  rc = L7_FAILURE;

#ifdef _L7_OS_LINUX_
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  if (fd > 0)
  {
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1)
    {
      /* Since FastPath assigns the service port the Base MAC     */
      /* address +1, subtract 1 here so it is assigned corrrectly */

      /* PTin removed: we don't know why, but the MAC was actually set to -1 in the end... */
//      ifr.ifr_hwaddr.sa_data[5]--;

      /* return result */
      memcpy(macString, ifr.ifr_hwaddr.sa_data, 6);
      rc = L7_SUCCESS;

      LOG_TRACE(LOG_CTX_PTIN_HAPI, "MACAddr = %02X:%02X:%02X:%02X:%02X:%02X",
                macString[0], macString[1], macString[2], macString[3], macString[4], macString[5]);
    }
    close(fd);
  }
#else
  rc = sysNetMacNVRamAddrGet(bspapiServicePortNameGet(), bspapiServicePortUnitGet(), macString, 6);
  {
    L7_uchar8 temp8;

    /* Need to byte swap the MAC address. */
    temp8 = macString[0];
    macString[0] = macString[5];
    macString[5] = temp8 - 1;
    temp8 = macString[1];
    macString[1] = macString[4];
    macString[4] = temp8;
    temp8 = macString[2];
    macString[2] = macString[3];
    macString[3] = temp8;
  }
#endif

  return(rc);

}

/*********************************************************************
* @purpose  Sets the stack ID LEDs
* 
* @comments 
* 
* @end
*********************************************************************/
void sysStackLedSet(L7_BOOL master, L7_uint32 unit_number)
{ 
} 
    


/*********************************************************************
* @purpose  Returns the card index of the slot requested
*
* @param    slotNum @b{(input)} requested slot ID
*
* @returns  card index of slot requested. (-1 if no card present)
*
* @end
*********************************************************************/
int bspCpuReadCardID(L7_uint32 slotNum)
{
  int cardIndex = -1;

  switch (slotNum)
  {
    /* The GTO has one non-pluggable card in slot 0 */
    case 0:
      cardIndex = 0;
      break;

   default:
     break;
   }

   return cardIndex;
}

/*******************************************************************************
** NAME: logNvStore 
**
**  CPU-specific function to store log entry in NVRAM.
**
*******************************************************************************/
void logNvStore(L7_uchar8 *buf)
{
  /* Normally the log should be written to non-volatile storage.
  ** On VxWorks this function may be called in an interrupt handler,
  ** so we can't use OS drivers.
  */

  osapiFsWrite (LOG_NVM_FILE, buf, LOG_CALLOUT_BUF_SIZE);
}

/*******************************************************************************
** NAME: logNvRetrieve 
**
**  CPU-specific function to retrieve log entry from NVRAM.
**  If BSP can't get the data then the buffer should not be touched.
**
*******************************************************************************/
void logNvRetrieve(L7_uchar8 *buf)
{
  osapiFsRead (LOG_NVM_FILE, buf, LOG_CALLOUT_BUF_SIZE);
}

