/*
 * $Id: gport_mgmt.c,v 1.247 Broadcom SDK $
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * The GPORT Management
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <shared/bsl.h>

#include "bcm_int/common/debug.h"

#include <sal/core/libc.h>
#include <sal/core/alloc.h>

#include <soc/drv.h>
#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/legacy/mbcm.h>
#include <soc/dnx/legacy/cosq.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/types.h>
#include <shared/hash_tbl.h>
#include <bcm_int/common/multicast.h>

#include <bcm_int/dnx/legacy/utils.h>
#include <bcm_int/dnx/legacy/gport_mgmt.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/common/multicast.h>

#include <soc/dnx/legacy/TMC/tmc_api_ports.h>
#include <soc/dnx/legacy/TMC/tmc_api_ingress_packet_queuing.h>
#include <shared/shrextend/shrextend_debug.h>


/*
 * Functions
 */

/*
 * Function:
 *    _bcm_dnx_gport_to_tm_dest_info
 * Description:
 *    convert gport from TM dest information
 * Parameters:
 *  unit -           [IN] DNX device unit number (driver internal).
 *  gport -          [OUT] general port
 *  soc_dnx_dest_info - [OUT] Soc_dnx destination info
 * Returns:
 *    _SHR_E_XXX
 */
int 
_bcm_dnx_gport_from_tm_dest_info(int unit, bcm_gport_t *gport, SOC_DNX_DEST_INFO  *soc_dnx_dest_info)
{
    int dummy;
    SHR_FUNC_INIT_VARS(unit);
    /* verify input parameters */
    SHR_NULL_CHECK(soc_dnx_dest_info, _SHR_E_PARAM, "soc_dnx_dest_info");
    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");
    
    switch(soc_dnx_dest_info->dbal_type) {
    case DBAL_FIELD_PORT_ID: /** SOC_DNX_DEST_TYPE_SYS_PHY_PORT */
        BCM_GPORT_SYSTEM_PORT_ID_SET(*gport, soc_dnx_dest_info->id);
        break;
    case DBAL_FIELD_MC_ID: /** SOC_DNX_DEST_TYPE_MULTICAST */
        BCM_GPORT_MCAST_SET(*gport,soc_dnx_dest_info->id);
        break;
    case DBAL_FIELD_LAG_ID: /** SOC_DNX_DEST_TYPE_LAG */
        SHR_IF_ERR_EXIT(dnx_trunk_spa_id_to_gport(unit, soc_dnx_dest_info->id, &dummy, gport));
        break;
    case DBAL_FIELD_FLOW_ID: /** SOC_DNX_DEST_TYPE_QUEUE */
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(*gport,soc_dnx_dest_info->id);
        break;
    default:
        SHR_ERR_EXIT(_SHR_E_PARAM, "unknown destination type");

    }

exit:
    SHR_FUNC_EXIT;
}
