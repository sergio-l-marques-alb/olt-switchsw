/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l7_vrrpinclude.h
*
* @purpose VRRP header file
*
* @component VRRP
*
* @comments 
*
* @create  08/11/2001
*
* @author  TCS
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#ifndef _L7_VRRPINCLUDE_H_
#define _L7_VRRPINCLUDE_H_


/*------------------------------------------------------------------
 *                         COMMON INCLUDES
 *------------------------------------------------------------------
 */
#include "math.h" 
#include <string.h>
#include "l7_common.h"                   /* lvl7 common definitions   */
#include "l7_common_l3.h"

#include "usmdb_mib_vrrp_api.h"
#include "default_cnfgr.h"
#include "nvstoreapi.h"
#include "osapi.h"
#include "l7_packet.h"
#include "nimapi.h"
#include "simapi.h"
#include "l7_ip_api.h"
#include "l3_defaultconfig.h"
#include "log.h"
#include "sysapi.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "l3end_api.h"
#include "osapi_support.h"
#include "trap_layer3_api.h"

#include "l7_cnfgr_api.h"

#include "l7_ip_api.h"
#include "buff_api.h"

/*------------------------------------------------------------------
 *                    APIs and FUNCTION PROTOTYPES
 *------------------------------------------------------------------
 */
#include "l7_vrrp_api.h"                   /* VRRP public methods */

/*------------------------------------------------------------------
 *                       VRRP FACTORY DEFAULTS
 *------------------------------------------------------------------
 */
#include "l3_defaultconfig.h"
#include "l3_default_cnfgr.h"

/*------------------------------------------------------------------
 *                          VRRP HEADER FILES
 *------------------------------------------------------------------
 */
#include "vrrp_config.h"
#include "vrrp_util.h"
#include "vrrptask.h"
#include "vrrp_cnfgr.h"

#endif /* _L7_VRRPINCLUDE_H */
