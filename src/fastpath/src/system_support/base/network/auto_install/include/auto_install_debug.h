/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
* @filename  auto_install_debug.h
*
* @purpose   auto-install debug utilities
*
* @component auto_install
*
* @comments
*
* @create    21/01/2008
*
* @author    ipopov
*
* @end
*
**********************************************************************/
#ifndef AUTO_INSTALL_DEBUG_H_
#define AUTO_INSTALL_DEBUG_H_

#include "datatypes.h"

typedef enum
{
  AUTO_INSTALL_DBG_FLAG_FIRST_ENTRY = 0,        /* 0 */
  
  AUTO_INSTALL_DBG_FLAG_ControlGroup,           /* 1 */
  AUTO_INSTALL_DBG_FLAG_CnfgrGroup,             /* 2 */
  AUTO_INSTALL_DBG_FLAG_Dns_Interact,           /* 3 */
  AUTO_INSTALL_DBG_FLAG_Tftp_Interact,          /* 4 */
  AUTO_INSTALL_DBG_FLAG_Dhcp_Bootp_Interact,    /* 5 */
  AUTO_INSTALL_DBG_FLAG_ConfScript_Interact,    /* 6 */
  AUTO_INSTALL_DBG_FLAG_ApiCroup,               /* 7 */
    
  AUTO_INSTALL_DBG_FLAG_LAST_ENTRY,             /* 8 */

  AUTO_INSTALL_DBG_FLAG_ALL_ENTRIES = 255
} autoInstallDebugTraceFlags_t;


/********** FUNCTION PROTOTYPES **********/

void autoInstallDebugTrace(L7_uint32 event_flag, L7_char8 * format, ...);
void autoInstallDebugTraceSet(L7_uint32 flag);
void autoInstallDebugTraceClear(L7_uint32 flag);

#endif /*AUTO_INSTALL_DEBUG_H_*/
