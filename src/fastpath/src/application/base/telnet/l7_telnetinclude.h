/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   telnet_config.h
*
* @purpose    Outbound Telnet Implementation
*
* @component  Outbound Telnet Component
*
* @comments   none
*
* @create     02/27/2003
*
* @author     anindya
*
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#ifndef _L7_TELNETINCLUDE_H_
#define _L7_TELNETINCLUDE_H_


/*------------------------------------------------------------------
 *                         COMMON INCLUDES
 *------------------------------------------------------------------
 */
#ifdef _L7_OS_VXWORKS_
  #include "telnetLib.h"
  #include "vxWorks.h"
  #include "sockLib.h"
  #include "inetLib.h"
  #include "ioLib.h"
  #include "hostLib.h"
#endif


#ifdef _L7_OS_LINUX_
  #include <termios.h>
  #include <arpa/telnet.h>
#endif


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


#include "l7_common.h"     /* lvl7 common definitions */
#include "nvstoreapi.h"
#include "osapi.h"
#include "log.h"
#include "osapi_support.h"
#include "defaultconfig.h"


/*------------------------------------------------------------------
 *                    APIs and FUNCTION PROTOTYPES
 *------------------------------------------------------------------
 */
#include "l7_telnet_api.h"     /* Outbound Telnet public methods */

/*------------------------------------------------------------------
 *                          Outbound Telnet HEADER FILES
 *------------------------------------------------------------------
 */
#include "telnet_config.h"
#include "telnet_util.h"
#include "telnet_cnfgr.h"

#endif /* _L7_TELNETINCLUDE_H */

