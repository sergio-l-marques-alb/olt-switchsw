/*
 * $Id: fabric.c Exp $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/dnxc/legacy/fabric.h>
#include <soc/dnxc/legacy/error.h>
#include <shared/shrextend/shrextend_debug.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/fabric/fabric_mesh.h>
#include <soc/dnx/legacy/mbcm.h>
#endif

int
soc_dnxc_fabric_mesh_topology_diag_get(int unit, soc_dnxc_fabric_mesh_topology_diag_t *mesh_topology_diag)
{
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {

        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_diag_mesh_topology_get, (unit, mesh_topology_diag)));
    } else
#endif /*BCM_DNXF_SUPPORT*/
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_is_supported)) {
            SHR_IF_ERR_EXIT(dnx_fabric_mesh_topology_get(unit, mesh_topology_diag));
        }
    } else
#endif /*BCM_DNX_SUPPORT*/
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "feature unavail");
    }

exit:
    SHR_FUNC_EXIT;
}

