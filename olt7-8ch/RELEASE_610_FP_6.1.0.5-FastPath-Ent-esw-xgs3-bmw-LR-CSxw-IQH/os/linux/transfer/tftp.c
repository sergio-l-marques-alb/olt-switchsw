/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2008
 *
 **********************************************************************
 *
 * @filename tftp.c
 *
 * @purpose
 *
 * @component Transfer
 *
 * @comments
 *
 * @create 22-Jun-2008
 *
 * @author Rama Sasthri, Kristipati
 * @end
 *
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include "osapi.h"
#include "osapi_support.h"
#include "l7_tftp.h"
#include "sysapi.h"

L7_int32 osapiTransferOpen (L7_uint32 direction, L7_uint32 fileType)
{
  char *filename = L7_NULLPTR;
  L7_int32 file_desc;

  if (fileType == L7_FILE_TYPE_CODE)
  {
    filename = "/tmp/code";
  }
  else if (fileType == L7_FILE_TYPE_CONFIG)
  {
    filename = "/tmp/config";
  }
  else
  {
    sysapiPrintf ("Unknown file type\n");
    return -1;
  }

  if (direction == L7_TRANSFER_DIRECTION_DOWN)
  {
    osapiFsDeleteFile (filename);
    if (L7_SUCCESS != osapiFsFileCreate (filename, &file_desc))
    {
      file_desc = L7_ERROR;
    }
    return file_desc;
  }
  return osapiFsOpen (filename);
}

void osapiTransferClose (L7_int32 fd)
{
  osapiFsClose (fd);
}

L7_uint32 osapiTransferRead (L7_uint32 fd, L7_char8 * buffer, size_t maxbytes)
{
  return osapiRead (fd, buffer, maxbytes);
}

L7_uint32 osapiTransferWrite (L7_uint32 fd, L7_char8 * buffer, size_t maxbytes)
{
  return osapiWrite (fd, buffer, maxbytes);
}

int osapiTftp (L7_uint32 ipAddr, L7_uint32 fileType, L7_uint32 direction)
{
  char ipAddrStr[128];
  L7_int32 file_desc;
  L7_inet_addr_t inet_address;
  L7_TRANSFER_STATUS_t tftpStatus;
  L7_char8 *tftpOp = (L7_TRANSFER_DIRECTION_DOWN == direction) ? "get" : "put";
  void *func = (L7_TRANSFER_DIRECTION_DOWN == direction) ? osapiTransferWrite : osapiTransferRead;

  file_desc = osapiTransferOpen (direction, fileType);
  if (file_desc == L7_ERROR || file_desc == L7_NULL)
  {
    sysapiPrintf ("osapiFsFileCreate failed\n");
    return 1;
  }

  inet_address.family = L7_AF_INET;
  inet_address.addr.ipv4.s_addr = ipAddr;

  osapiInetNtop (L7_AF_INET, (L7_uchar8 *) & ipAddr, (L7_uchar8 *) ipAddrStr, sizeof (ipAddrStr));

  sysapiPrintf ("executing L7_TFTP_COPY %s %s 0x%x\n", tftpOp, ipAddrStr, ipAddr);
  tftpStatus =
    L7_TFTP_COPY (&inet_address, "switchdrvr", tftpOp, "octet", -1, TFTP_TIMEOUT, func, (void *) file_desc);
  sysapiPrintf ("Transfer completed status = %d\n", tftpStatus);

  osapiTransferClose (file_desc);
  return tftpStatus;
}

int osapiDebugTftpGet (L7_uint32 ipaddr)
{
  return osapiTftp (ipaddr, 0, L7_TRANSFER_DIRECTION_DOWN);
}

int osapiDebugTftpPut (L7_uint32 ipaddr)
{
  return osapiTftp (ipaddr, 0, L7_TRANSFER_DIRECTION_UP);
}
