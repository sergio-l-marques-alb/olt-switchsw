#ifndef TFTP_CLIENT_DEBUG_H_
#define TFTP_CLIENT_DEBUG_H_

#include "datatypes.h"

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
L7_uint32 tftpClientDebugModeSet(L7_uint32 mode);

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
void tftpDebugTrace(L7_char8 * format, ...);

#endif /*TFTP_CLIENT_DEBUG_H_*/
