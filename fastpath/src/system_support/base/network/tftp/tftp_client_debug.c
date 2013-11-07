/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
* @filename  tftp_client_debug.c
*
* @purpose   tftp client debug file
*
* @comments
*
* @create    30/01/2008
*
* @author    ipopov
*
* @end
*
**********************************************************************/

#include "tftp_client_debug.h"
#include "commdefs.h"
#include <stdarg.h>
#include <stdio.h>

static L7_uint32 tftpDebugFlag = L7_DISABLE;

/*********************************************************************
* @purpose  Set tftp client tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 tftpClientDebugModeSet(L7_uint32 mode)
{
  if(mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;

  tftpDebugFlag = mode;
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Record a tftp client event trace if trace configuration
*           permits 
*
* @param    format      @b{(input)} format string
* @param    ...         @b{(input)} additional arguments (per format
*           string)
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void tftpDebugTrace(L7_char8 * format, ...)
{
  va_list arg;
  
  if (tftpDebugFlag == L7_DISABLE)
  {
    return;
  }

  va_start (arg, format);
  vprintf(format, arg);
  va_end (arg);
}
