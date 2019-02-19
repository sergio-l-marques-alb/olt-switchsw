/*
 * $Id: dnxf_fabric.c,v 1.6 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXF FABRIC
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

#ifdef BCM_DNXF_SUPPORT
#include <shared/bsl.h>
#include <soc/dnxc/legacy/error.h>
#include <bcm/fabric.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <bcm/error.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/mbcm.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

#include <bcm_int/control.h>

/*
 * Function:
 *      soc_dnxf_fabric_link_status_all_get
 * Purpose:
 *      Get all links status
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      links_array_max_size - (IN)  max szie of link_status array
 *      link_status          - (OUT) array of link status per link
 *      errored_token_count  - (OUT) array error token count per link
 *      links_array_count    - (OUT) array actual size
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e
soc_dnxf_fabric_link_status_all_get(int unit, int links_array_max_size, uint32* link_status, uint32* errored_token_count, int* links_array_count)
{
    int rc, nof_links = 0;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }
    
    nof_links = dnxf_data_port.general.nof_links_get(unit);
    if(links_array_max_size < nof_links) {
         SHR_ERR_EXIT(_SHR_E_FULL, "Array is too small");
    }

    SHR_NULL_CHECK(link_status, _SHR_E_PARAM, "link_status");
    SHR_NULL_CHECK(errored_token_count, _SHR_E_PARAM, "errored_token_count");
    SHR_NULL_CHECK(links_array_count, _SHR_E_PARAM, "links_array_count");

    DNXF_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_status_all_get,(unit, links_array_max_size, link_status, errored_token_count, links_array_count));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

#endif /* BCM_DNXF_SUPPORT */

#undef BSL_LOG_MODULE

