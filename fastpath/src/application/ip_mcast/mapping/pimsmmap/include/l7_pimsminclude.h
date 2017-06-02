/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  l7_pimsminclude.h
*
* @purpose   Contains all common PIM-SM header includes
*
* @component PIM-SM Mapping Layer
*
* @comments  none
*
* @create    03/07/2002
*
* @author    Ratnakar
* @end
*
**********************************************************************/


#ifndef _L7_PIMSMINCLUDE_H_
#define _L7_PIMSMINCLUDE_H_


/*------------------------------------------------------------------
 *                         COMMON INCLUDES
 *------------------------------------------------------------------
 */
#include "l7_common.h"                   /* lvl7 common definitions   */
#include "l7_common_l3.h"
#include "l3_comm_structs.h"
#include "log.h"
#include "sysapi.h"
#include "osapi.h"
/*------------------------------------------------------------------
 *                       PIM-SM FACTORY DEFAULTS
 *------------------------------------------------------------------
 */
#include "l3_mcast_defaultconfig.h"
#include "l3_mcast_commdefs.h"
#include "l3_addrdefs.h"

/*------------------------------------------------------------------
 *                    APIs and FUNCTION PROTOTYPES
 *------------------------------------------------------------------
 */
#include "l7_ip_api.h"                   /* IP MAP public methods */


/*------------------------------------------------------------------
 *                       PIM-SM HEADER FILES
 *------------------------------------------------------------------
 */
#include "mcast_api.h"
#include "pimsm_map_cfg.h"
#include "pimsm_map_util.h"
#include "pimsm_map_debug.h"
#endif /* _L7_PIMSMINCLUDE_H_ */

