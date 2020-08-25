/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  l7_ripinclude.h
*
* @purpose   Contains all common RIP header includes.
*
* @component RIP Mapping Layer
*
* @comments  none
*
* @create    05/21/2001
*
* @author    gpaussa
* @end
*
**********************************************************************/

/**************************@null{*************************************
                     
 *******************************}*************************************}
 **********************************************************************/

#ifndef _L7_RIPINCLUDE_H_
#define _L7_RIPINCLUDE_H_
 

/*------------------------------------------------------------------
 *                         COMMON INCLUDES
 *------------------------------------------------------------------
 */
#include <string.h>
#include <time.h>
#include "l7_common.h"                   /* lvl7 common definitions   */
#include "l7_common_l3.h"

#include "nvstoreapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "l7_packet.h"
#include "nimapi.h"
#include "simapi.h"
#include "l7_ip_api.h"
#include "l3_defaultconfig.h"
#include "ip_config.h"
#include "log.h"
#include "sysapi.h"
#include "l3end_api.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "l3_default_cnfgr.h"

/*------------------------------------------------------------------
 *                    APIs and FUNCTION PROTOTYPES
 *------------------------------------------------------------------
 */
#include "l7_rip_api.h"                   /* rip public methods */

/*------------------------------------------------------------------
 *                       RIP FACTORY DEFAULTS
 *------------------------------------------------------------------
 */
#include "l3_defaultconfig.h"
#include "l3_default_cnfgr.h"

/*------------------------------------------------------------------
 *                          RIP HEADER FILES
 *------------------------------------------------------------------
 */
 
#include "rip_config.h"
#include "rip_map_cnfgr.h"
#include "rip_util.h"
#include "rip_vend_ctrl.h"
#include "rip_vend_exten.h"
#include "rip_debug.h"
#include "rip_map_debug.h"


#endif /* _L7_RIPINCLUDE_H */
