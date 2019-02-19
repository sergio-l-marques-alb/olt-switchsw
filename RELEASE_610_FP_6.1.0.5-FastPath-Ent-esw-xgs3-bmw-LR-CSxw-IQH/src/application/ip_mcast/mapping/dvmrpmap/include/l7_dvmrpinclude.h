/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  l7_dvmrpinclude.h
*
* @purpose   Contains all common DVMRP header includes.
*
* @component DVMRP Mapping Layer
*
* @comments  none
*
* @create    02/12/2002
*
* @author    M Pavan K Chakravarthi
* @end
*
**********************************************************************/

#ifndef _L7_DVMRPINCLUDE_H_
#define _L7_DVMRPINCLUDE_H_


/*------------------------------------------------------------------
 *                         COMMON INCLUDES
 *------------------------------------------------------------------
 */
#include <string.h>
#include "l7_common.h"                   /* lvl7 common definitions   */
#include "l7_common_l3.h"
#include "nvstoreapi.h"
#include "osapi.h"
#include "l7_packet.h"
#include "nimapi.h"
#include "simapi.h"
#include "l3_defaultconfig.h"
#include "l3_default_cnfgr.h"
#include "log.h"
#include "sysapi.h"
#include "l3end_api.h"

/******************************************************************
                FLEX header files
******************************************************************/
#include "l3_mcast_commdefs.h"
#include "l3_mcast_defaultconfig.h"

/*------------------------------------------------------------------
 *                    APIs and FUNCTION PROTOTYPES
 *------------------------------------------------------------------
 */
#include "l7_dvmrp_api.h"                /* dvmrpmap public methods */
#include "dvmrp_api.h"                   /* dvmrp protocol public methods */
#include "l7_ip_api.h"                   /* ip public methods */
#include "trap_layer3_mcast_api.h"

/*------------------------------------------------------------------
 *                       DVMRP FACTORY DEFAULTS
 *------------------------------------------------------------------
 */
#include "l3_mcast_defaultconfig.h"
#include "l3_mcast_default_cnfgr.h"

/*------------------------------------------------------------------
 *                          DVMRP HEADER FILES
 *------------------------------------------------------------------
 */
#include "dvmrp_cnfgr.h"
#include "dvmrp_config.h"
#include "dvmrp_map_util.h"
#include "dvmrp_map.h"
#include "dvmrp_vend_exten.h"
#include "dvmrp_vend_ctrl.h"
#include "dvmrp_map_debug.h"
#endif /* _L7_DVMRPINCLUDE_H */
