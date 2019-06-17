/** \file appl_ref_sys_ports.c
 * $Id$
 *
 * System ports level application procedures for DNX. 
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/property.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/port.h>
#include "appl_ref_sys_device.h"
#include <appl/reference/dnx/appl_ref_sys_ports.h>

#include <appl/reference/sand/appl_ref_sand.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/*
 * See .h files
 */
int
appl_dnx_logical_port_to_sysport_get(
    int unit,
    bcm_module_t modid,
    bcm_port_t port,
    int *sysport)
{
    int modid_index;
    appl_dnx_sys_device_t *sys_params;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** convert modid to modid index (fap index) */
    modid_index = (modid - sys_params->base_modid) / APPL_DNX_NOF_MODIDS_PER_DEVICE;

    SHR_IF_ERR_EXIT(appl_sand_modid_to_sysport_convert(unit, modid_index, port, sysport));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_sys_ports_port_create(
    int unit,
    int port)
{
    appl_dnx_sys_device_t *sys_params;
    int modid_idx;
    int sys_port;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_gport_t modport_gport, sysport_gport;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** iterate over all devices and configure a system port */
    for (modid_idx = 0; modid_idx < sys_params->nof_devices; modid_idx++)
    {
        /** Convert modid and logical port to system port id */
        SHR_IF_ERR_EXIT(appl_sand_modid_to_sysport_convert(unit, modid_idx, port, &sys_port));

        /** Map system port to physical port (modport) */
        {
            SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));

            BCM_GPORT_MODPORT_SET(modport_gport, sys_params->base_modid + modid_idx * APPL_DNX_NOF_MODIDS_PER_DEVICE +
                                  mapping_info.core, mapping_info.tm_port);

            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);

            SHR_IF_ERR_EXIT(bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_sys_ports_init(
    int unit)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    /** get ports */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_ports_get(unit, &pbmp));

    /** create system port for each port */
    BCM_PBMP_ITER(pbmp, port)
    {
        SHR_IF_ERR_EXIT(appl_dnx_sys_ports_port_create(unit, port));
    }

exit:
    SHR_FUNC_EXIT;
}
