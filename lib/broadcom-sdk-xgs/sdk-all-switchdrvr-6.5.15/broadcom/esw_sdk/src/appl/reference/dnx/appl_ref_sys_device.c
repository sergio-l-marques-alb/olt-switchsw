/** \file appl_ref_sys_device.c
 * $Id$
 *
 * System device level application procedures for DNX. 
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
#include <bcm/fabric.h>
#include <bcm/instru.h>

#include <appl/diag/shell.h>
#include "appl_ref_sys_device.h"
#include "appl_ref_rx_init.h"


#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnxc/dnxc_ha.h>


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

appl_dnx_sys_device_t appl_dnx_sys_params[SOC_MAX_NUM_DEVICES];

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

/**
 * \brief - parse device level system parameters (nof devices, base modid, etc...) 
 * These attributes are later used by following applications
 */
static shr_error_e
appl_dnx_sys_device_params_parse(
    int unit,
    appl_dnx_sys_device_t * sys_params)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Init */
    sal_memset(sys_params, 0x0, sizeof(appl_dnx_sys_device_t));

    /** number of device in the system, used for application iteration */
    sys_params->nof_devices = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "nof_devices", 1);
    LOG_INFO_EX(BSL_LOG_MODULE, "   nof devices = %d.\n%s%s%s", sys_params->nof_devices, EMPTY, EMPTY, EMPTY);

    /** base modid, used for application iteration */
    sys_params->base_modid = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "base_module_id", 0);
    LOG_INFO_EX(BSL_LOG_MODULE, "   base module id = %d.\n%s%s%s", sys_params->base_modid, EMPTY, EMPTY, EMPTY);

    /** my modid, this id is later configured to HW */
    sys_params->modid = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "module_id", 0);
    LOG_INFO_EX(BSL_LOG_MODULE, "   module id = %d.\n%s%s%s", sys_params->modid, EMPTY, EMPTY, EMPTY);

    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parsed system parameters
 */
static shr_error_e
appl_dnx_sys_device_params_verify(
    int unit,
    appl_dnx_sys_device_t * sys_params)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify modid is in range */
    if ((sys_params->modid < sys_params->base_modid) ||
        (sys_params->modid >= (sys_params->base_modid + APPL_DNX_NOF_MODIDS_PER_DEVICE * sys_params->nof_devices)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Modid %d is out of range of valid modids.\n", sys_params->modid);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_sys_device_init(
    int unit)
{
    appl_dnx_sys_device_t *sys_params;
    bcm_stk_modid_config_t modid_config;

    SHR_FUNC_INIT_VARS(unit);


    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** Configure my modid */
    bcm_stk_modid_config_t_init(&modid_config);
    modid_config.modid = sys_params->modid;
    SHR_IF_ERR_EXIT(bcm_stk_modid_config_set(unit, &modid_config));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_device_params_get(
    int unit,
    appl_dnx_sys_device_t ** sys_params)
{
    SHR_FUNC_INIT_VARS(unit);

    /** point to the units system parameters */
    *sys_params = &(appl_dnx_sys_params[unit]);

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_params_parse(
    int unit)
{
    appl_dnx_sys_device_t *sys_params;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** Parse and fill the parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_parse(unit, sys_params));

    /** Verify parsed system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_verify(unit, sys_params));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_traffic_enable(
    int unit)
{
    appl_dnx_sys_device_t *sys_params;
    bcm_port_config_t port_config;
    int cpu_port;
    int cpu_port_exist = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        /** Get a pointer to the system parameters */
        SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));
        
        if (!SOC_IS_J2C(unit))
        {
            /** Enable fabric control cells */
            SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricControlCellsEnable, 1));

            /** Enable traffic */
            SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, sys_params->modid, -1, 1));
        }
    }

    /** Enable CPU RX */

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    /*
     * Finding first CPU port
     */
    BCM_PBMP_ITER(port_config.cpu, cpu_port)
    {
        cpu_port_exist = 1;
        break;
    }

    if (cpu_port_exist)
    {
        SHR_IF_ERR_EXIT(dnx_rx_appl_active(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_vis_enable(
    int unit)
{
    bcm_port_config_t port_config;
    bcm_gport_t logical_port;
    bcm_pbmp_t pbmp;
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_instru_control_set(unit, 0, bcmInstruControlVisMode, bcmInstruVisModeSelective));

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

        BCM_PBMP_ASSIGN(pbmp, port_config.all);
        BCM_PBMP_REMOVE(pbmp, port_config.sfi);
        BCM_PBMP_REMOVE(pbmp, port_config.olp);

        BCM_PBMP_ITER(pbmp, logical_port)
        {
            flags = 0;
            sal_memset(&port_interface_info, 0x0, sizeof(bcm_port_interface_info_t));
            sal_memset(&port_mapping_info, 0x0, sizeof(bcm_port_mapping_info_t));

            SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &port_interface_info, &port_mapping_info));
            if (flags & (BCM_PORT_ADD_KBP_PORT | BCM_PORT_ADD_STIF_PORT))
            {
                continue;
            }
            /*
             * if (port_interface_info.interface == BCM_PORT_IF_ERP) { continue; } 
             */
            SHR_IF_ERR_EXIT(bcm_instru_gport_control_set(unit, logical_port,
                                                         BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS |
                                                         BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                         bcmInstruGportControlVisForce, TRUE));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_init_done_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Notify Error Recovery -  init done
     */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_appl_init_state_change(unit, TRUE));
    DNXC_MTA(SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_application_init_is_done(unit, TRUE)));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_init_done_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Notify Error Recovery -  init done
     */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_appl_init_state_change(unit, FALSE));
    DNXC_MTA(SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_application_init_is_done(unit, FALSE)));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_device_ports_get(
    int unit,
    bcm_pbmp_t * pbmp)
{
    bcm_port_config_t port_config;
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    bcm_port_t logical_port;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    /** Create a bitmap of all ports */
    BCM_PBMP_CLEAR(*pbmp);
    BCM_PBMP_OR(*pbmp, port_config.nif);
    BCM_PBMP_OR(*pbmp, port_config.cpu);
    BCM_PBMP_OR(*pbmp, port_config.rcy);
    BCM_PBMP_OR(*pbmp, port_config.sat);
    BCM_PBMP_OR(*pbmp, port_config.ipsec);
    BCM_PBMP_OR(*pbmp, port_config.erp);
    BCM_PBMP_OR(*pbmp, port_config.oamp);
    BCM_PBMP_OR(*pbmp, port_config.olp);
    BCM_PBMP_OR(*pbmp, port_config.eventor);

    BCM_PBMP_ITER(*pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &port_interface_info, &port_mapping_info));

        /** filter KBP and STIF ports */
        if (flags & (BCM_PORT_ADD_KBP_PORT | BCM_PORT_ADD_STIF_PORT))
        {
            BCM_PBMP_PORT_REMOVE(*pbmp, logical_port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
