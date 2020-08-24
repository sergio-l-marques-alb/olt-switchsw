/** \file appl_ref_sys_device.c
 * $Id$
 *
 * System device level application procedures for DNX. 
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <soc/dnxc/dnxc_verify.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>


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
    int modid_index, modid;
    SHR_FUNC_INIT_VARS(unit);

    /** Init */
    sal_memset(sys_params, 0x0, sizeof(appl_dnx_sys_device_t));

    /** number of device in the system, used for application iteration */
    sys_params->nof_devices = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "nof_devices", 1);
    LOG_VERBOSE_EX(BSL_LOG_MODULE, "   nof devices = %d.\n%s%s%s", sys_params->nof_devices, EMPTY, EMPTY, EMPTY);

    /** number of modids in each device in the system */
    sys_params->nof_modids_per_device =
        soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "nof_modids",
                                                dnx_data_device.general.max_nof_fap_ids_per_core_get(unit) *
                                                dnx_data_device.general.nof_cores_get(unit));
    LOG_VERBOSE_EX(BSL_LOG_MODULE, "   nof modids per device = %d.\n%s%s%s", sys_params->nof_modids_per_device,
                   EMPTY, EMPTY, EMPTY);

    sys_params->nof_modids_per_core = sys_params->nof_modids_per_device / dnx_data_device.general.nof_cores_get(unit);

    /** device index */
    sys_params->device_index = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "device_index", 0);
    LOG_VERBOSE_EX(BSL_LOG_MODULE, "   device index = %d.\n%s%s%s", sys_params->device_index, EMPTY, EMPTY, EMPTY);

    for (modid_index = 0; modid_index < APPL_DNX_NOF_MODIDS; modid_index++)
    {
        sys_params->index_to_modid[modid_index] = -1;
        sys_params->modid_to_index[modid_index] = -1;
    }
    for (modid_index = 0; modid_index < sys_params->nof_devices * sys_params->nof_modids_per_device; modid_index++)
    {
        modid = soc_property_suffix_num_get_only_suffix(unit, modid_index, spn_APPL_PARAM, "modid_map_", modid_index);
        sys_params->index_to_modid[modid_index] = modid;
        sys_params->modid_to_index[modid] = modid_index;
    }

    /** my modid, this id is later configured to HW */
    modid_index = sys_params->device_index * sys_params->nof_modids_per_device;
    sys_params->modid = sys_params->index_to_modid[modid_index];
    LOG_VERBOSE_EX(BSL_LOG_MODULE, "   module id = %d.\n%s%s%s", sys_params->modid, EMPTY, EMPTY, EMPTY);

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
    int modid_index, my_modid_index, local_index;
    int core0_modid_index;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify modid is in range */

    my_modid_index = sys_params->device_index * sys_params->nof_modids_per_device;
    for (local_index = 0; local_index < sys_params->nof_modids_per_core; local_index++)
    {
        core0_modid_index = my_modid_index + local_index * dnx_data_device.general.nof_cores_get(unit);

        for (core = 1; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            modid_index = core0_modid_index + core;
            if (sys_params->index_to_modid[modid_index] != sys_params->index_to_modid[core0_modid_index] + core)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Core %d  modid (%d) and core 0 modid (%d) are not sequentail.\n",
                             core, sys_params->index_to_modid[modid_index],
                             sys_params->index_to_modid[core0_modid_index]);

            }
        }
    }

    for (modid_index = 0; modid_index < sys_params->nof_devices * sys_params->nof_modids_per_device; modid_index++)
    {
        if (sys_params->modid_to_index[sys_params->index_to_modid[modid_index]] != modid_index)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Modid %d is used in more than one index (%d and %d).\n",
                         sys_params->index_to_modid[modid_index], modid_index,
                         sys_params->modid_to_index[sys_params->index_to_modid[modid_index]]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert FAP index + TM port to 
 * modid + ftmh_pp_dsp
 *
 */
shr_error_e
appl_dnx_fap_and_tm_port_to_modport_convert(
    int unit,
    int fap_index,
    int core_id,
    int tm_port,
    int *modid,
    int *ftmh_pp_dsp)
{
    appl_dnx_sys_device_t *sys_params;
    int local_index;
    int nof_fap_ids_per_core = dnx_data_device.general.max_nof_fap_ids_per_core_get(unit);
    int modid_index;

    SHR_FUNC_INIT_VARS(unit);

    local_index = tm_port / APPL_DNX_NOF_PORTS_PER_MODID;
    *ftmh_pp_dsp = tm_port % APPL_DNX_NOF_PORTS_PER_MODID;

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    modid_index = fap_index * APPL_DNX_NOF_MODIDS_PER_DEVICE(sys_params) + nof_fap_ids_per_core * core_id + local_index;
    *modid = sys_params->index_to_modid[modid_index];
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert FAP index + TM port to 
 * modid
 *
 */
shr_error_e
appl_dnx_fap_and_tm_port_to_modid_convert(
    int unit,
    int fap_index,
    int core_id,
    int tm_port,
    int *modid)
{
    int ftmh_pp_dsp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnx_fap_and_tm_port_to_modport_convert(unit, fap_index, core_id, tm_port,
                                                                modid, &ftmh_pp_dsp));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert FAP index + TM port to 
 * modid
 *
 */
shr_error_e
appl_dnx_modid_to_fap_and_core_convert(
    int unit,
    int modid,
    int *fap_index,
    int *core_id)
{
    appl_dnx_sys_device_t *sys_params;
    int nof_fap_ids_per_core = dnx_data_device.general.max_nof_fap_ids_per_core_get(unit);
    int modid_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** get modid index */
    modid_index = sys_params->modid_to_index[modid];

    /** convert modid to device index */
    *fap_index = modid_index / APPL_DNX_NOF_MODIDS_PER_DEVICE(sys_params);

    /** convert modid to core id */
    *core_id = (modid_index % APPL_DNX_NOF_MODIDS_PER_DEVICE(sys_params)) / nof_fap_ids_per_core;

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
    int local_index, modid_index;

    SHR_FUNC_INIT_VARS(unit);


    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** Configure my modid */
    bcm_stk_modid_config_t_init(&modid_config);

    for (local_index = 0; local_index < sys_params->nof_modids_per_core; local_index++)
    {
        modid_index = sys_params->device_index * sys_params->nof_modids_per_device +
            local_index * dnx_data_device.general.nof_cores_get(unit);
        modid_config.modid = sys_params->index_to_modid[modid_index];
        SHR_IF_ERR_EXIT(bcm_stk_modid_config_add(unit, &modid_config));
    }
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

    /*
     * Init the dnxc_verify_allowed flag.
     */
    dnxc_verify_allowed_init(unit);

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

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        /** Get a pointer to the system parameters */
        SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

        if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_is_supported))
        {
            /** Enable fabric control cells */
            SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricControlCellsEnable, 1));
        }

        /** Enable traffic */
        SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, -1, 1));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_vis_port_enable(
    int unit,
    bcm_port_t logical_port)
{
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    uint32 flags;
    uint32 instru_flags;

    SHR_FUNC_INIT_VARS(unit);

    flags = 0;
    sal_memset(&port_interface_info, 0x0, sizeof(bcm_port_interface_info_t));
    sal_memset(&port_mapping_info, 0x0, sizeof(bcm_port_mapping_info_t));

    SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &port_interface_info, &port_mapping_info));
    if (!(flags &
          (BCM_PORT_ADD_KBP_PORT | BCM_PORT_ADD_STIF_PORT | BCM_PORT_ADD_FLEXE_PHY | BCM_PORT_ADD_CROSS_CONNECT)))
    {
        instru_flags = BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS;
        if (port_mapping_info.pp_port != -1)
        {
            instru_flags |= BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS;
        }
        SHR_IF_ERR_EXIT(bcm_instru_gport_control_set(unit, logical_port,
                                                     instru_flags, bcmInstruGportControlVisForce, TRUE));
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
            SHR_IF_ERR_EXIT(appl_dnx_sys_vis_port_enable(unit, logical_port));
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
#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
    SHR_IF_ERR_EXIT(dnx_rollback_journal_appl_init_state_change(unit, TRUE));
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

#ifdef DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_diag_init(unit));
#endif /* DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS */

    DNXC_MTA(SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_application_init_is_done(unit, TRUE)));

    /*
     * Enable verification after init is done.
     */
    dnxc_verify_allowed_set(unit, 1);

    SHR_EXIT();
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

    DNXC_MTA(SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_application_init_is_done(unit, FALSE)));

    /*
     * Notify Error Recovery -  init done
     */
#ifdef DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_diag_deinit(unit));
#endif /* DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS */

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
    SHR_IF_ERR_EXIT(dnx_rollback_journal_appl_init_state_change(unit, FALSE));
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

    SHR_EXIT();
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

        /** filter KBP, STIF ports, FlexE phy and L1 ports */
        if (flags &
            (BCM_PORT_ADD_KBP_PORT | BCM_PORT_ADD_STIF_PORT | BCM_PORT_ADD_FLEXE_PHY | BCM_PORT_ADD_CROSS_CONNECT))
        {
            BCM_PBMP_PORT_REMOVE(*pbmp, logical_port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
