/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dhcps_config.h
*
* @purpose    DHCP Server Implementation
*
* @component  DHCP Server Component
*
* @comments   none
*
* @create     09/15/2003
*
* @author     athakur
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#ifndef _L7_DHCPSINCLUDE_H_
#define _L7_DHCPSINCLUDE_H_


/*------------------------------------------------------------------
 *                         COMMON INCLUDES
 *------------------------------------------------------------------
 */
#include <string.h>
#include <stdlib.h>
#include "l7_common.h"                   /* lvl7 common definitions   */
#include "nvstoreapi.h"
#include "osapi.h"
#include "l7_packet.h"
#include "nimapi.h"
#include "simapi.h"
#include "l7_ip_api.h"
#include "log.h"
#include "sysapi.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "dtlapi.h"
#include "osapi_support.h"
#include "defaultconfig.h"


/*------------------------------------------------------------------
 *                    APIs and FUNCTION PROTOTYPES
 *------------------------------------------------------------------
 */
#include "l7_dhcps_api.h"                   /* dhcps public methods */


/*------------------------------------------------------------------
 *                          DHCP Server HEADER FILES
 *------------------------------------------------------------------
 */
#include "dhcps_config.h"
#include "dhcps_util.h"
#include "dhcps_cnfgr.h"


#endif /* _L7_DHCPSINCLUDE_H */
