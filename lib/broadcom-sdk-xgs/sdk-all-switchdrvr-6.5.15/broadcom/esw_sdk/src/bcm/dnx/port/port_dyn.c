/**
 * \file port_dyn.c
 * 
 *  Dynamicaly adding and removing ports
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lane_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/counter.h>

#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/tune/tune.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include <bcm_int/dnx/port/port_ingress.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/cosq/egq_ps_db.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <bcm_int/dnx/mirror/mirror_rcy.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <bcm_int/dnx/link/link.h>

#include <soc/dnx/swstate/auto_generated/access/vlan_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_algo_port_types.h>
#include <soc/sand/sand_aux_access.h>
#include "port_utils.h"
#include "src/bcm/dnx/algo/port/algo_port_internal.h"
#include "src/bcm/dnx/trunk/trunk_utils.h"
#include "src/bcm/dnx/trunk/trunk_sw_db.h"
#include <soc/dnxc/dnxc_wb_test_utils.h>

/*
 * }
 */

/*
 * Macros.
 * {
 */
/**
 * \brief - supported flags for API bcm_dnx_port_add()
 */
#define DNX_PORT_ADD_SUPPORTED_FLAGS (BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID | BCM_PORT_ADD_CGM_AUTO_ADJUST_DISABLE | \
                                      BCM_PORT_ADD_CONFIG_CHANNELIZED | BCM_PORT_ADD_STIF_PORT | \
                                      BCM_PORT_ADD_KBP_PORT | BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT)
/*
 * }
 */

/*
 * Typedefs.
 * {  
 */
/**
 * \brief - relevant information required for each step in port add / remove sequence
 */
typedef struct dnx_port_add_remove_step_s dnx_port_add_remove_step_t;
struct dnx_port_add_remove_step_s
{
    /**
     * Step name
     */
    char *name;
    /**
     * Callback to add function
     */
    utilex_seq_cb_f add_func;
    /**
     * Callback to remove function
     */
    utilex_seq_cb_f remove_func;
    /**
     * List of sub-steps (optional - might be null if not required)
     */
    const dnx_port_add_remove_step_t *sub_list;

    /** 
     * Flag function CB that will run prior to the init function of 
     * this step to determine which flags are needed to the step 
     * according to the CB logic - could be looking for certain SOC 
     * properties for example. 
     */
    dnx_step_flag_cb flag_function;
};

/*
 * }
 */

/*
 * Local Functions
 * {
 */
static int dnx_port_init_ilkn_lane_order_set(
    int unit);
/*
 * }
 */

/**
 * \brief - callback function which skips J2C 
 * 
 */
static shr_error_e
j2c_skip(
    int unit,
    int *dynamic_flags)
{
    if (SOC_IS_J2C(unit))
    {
        *dynamic_flags = DNX_INIT_STEP_F_SKIP;
    }
    else
    {
        *dynamic_flags = 0;
    }
    return _SHR_E_NONE;
}

/**
 * \brief - Verifying 'bcm_dnx_port_add()' 
 *          For additional info goto 'bcm_dnx_port_add()'  definition.
 */
static shr_error_e
dnx_port_add_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    int is_init_done;
    const dnx_data_nif_phys_core_phys_map_t *nif_core_phys_map;
    bcm_pbmp_t phy_pbmp;
    int is_stif = 0, is_elk = 0, is_boot_sim = 0;
    dnx_algo_port_type_e port_type;
    int supported_core;
    SHR_FUNC_INIT_VARS(unit);

    /** null checks */
    SHR_NULL_CHECK(interface_info, _SHR_E_PARAM, "inteface_info");
    SHR_NULL_CHECK(mapping_info, _SHR_E_PARAM, "mapping_info");

    /** flags */
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_USE_PHY_PBMP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_PORT_ADD_USE_PHY_PBMP flag is not supported. phy_pbmp is used only for BCM_PORT_IF_ILKN and BCM_PORT_IF_NIF_ETH interface types.\n");
    }

    if ((interface_info->interface != BCM_PORT_IF_ILKN) && _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT flag is supported only for BCM_PORT_IF_ILKN interface type.\n");
    }

    SHR_MASK_VERIFY(flags, DNX_PORT_ADD_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");

    /** valid and free logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_free_verify(unit, port));

    /** setting pp_port is not supported - make sure that pp port wasn't set explicitly*/
    if (mapping_info->pp_port != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Setting PP port is not supported (only getting). pp_port must be set to -1\n");
    }

    /** validate core/phys mapping (if any) */
    /*
     * if ELK port, update flag
     */
    if (flags & BCM_PORT_ADD_KBP_PORT)
    {
        is_elk = 1;
    }
    /*
     * add indication for STIF port
     */
    if (flags & BCM_PORT_ADD_STIF_PORT)
    {
        is_stif = 1;
    }

    /*
     * Convert interface type to port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_type_get
                    (unit, interface_info->interface, is_elk, is_stif, &port_type));
    nif_core_phys_map = dnx_data_nif.phys.core_phys_map_get(unit, mapping_info->core);
    BCM_PBMP_CLEAR(phy_pbmp);
#ifdef PLISIM
    if (SAL_BOOT_PLISIM)
    {
        is_boot_sim = 1;
    }
#endif /* PLISIM */

    if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_type) && is_boot_sim == 0)
    {
        if ((interface_info->interface == BCM_PORT_IF_ILKN) && BCM_PBMP_IS_NULL(interface_info->phy_pbmp))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ILKN port (%d) cannot be assigned with empty bitmap\n", port);
        }

        if (BCM_PBMP_IS_NULL(interface_info->phy_pbmp))
        {
            /*
             * phy_port is one based.
             * Decrease 1 to be aligned with zero based phy pbmp
             */
            BCM_PBMP_PORT_ADD(phy_pbmp, (interface_info->phy_port - 1));
        }
        else
        {
            BCM_PBMP_ASSIGN(phy_pbmp, interface_info->phy_pbmp);
        }

        if ((interface_info->interface != BCM_PORT_IF_ILKN) && BCM_PBMP_NOT_NULL(phy_pbmp))
        {
            BCM_PBMP_AND(phy_pbmp, nif_core_phys_map->phys);
            if (BCM_PBMP_IS_NULL(phy_pbmp))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Should not assign port %d to core %d\n", port, mapping_info->core);
            }
        }

        if (interface_info->interface == BCM_PORT_IF_ILKN)
        {
            supported_core = dnx_data_nif.ilkn.supported_core_get(unit, interface_info->interface_id)->core;
            if (supported_core != mapping_info->core)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "ILKN port (%d) mapped to unsupported core (%d)\n", port,
                             mapping_info->core);
            }
        }
    }

    /** Adding ERP port dynamically is not supported */
    is_init_done = dnx_init_is_init_done_get(unit);
    if (is_init_done)
    {
        if (interface_info->interface == BCM_PORT_IF_ERP)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - adding ERP port dynamically is not supported, use soc property ucode_port instead.\n",
                         port);
        }
    }

    /** additional verification will be done by port mgmt when adding the new port to database*/

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verifying 'bcm_dnx_port_remove()' 
 *          For additional info goto 'bcm_dnx_port_remove()'  definition.
 */
static shr_error_e
dnx_port_remove_verify(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    uint32 supported_flags;
    dnx_algo_port_type_e port_type;
    uint32 port_speed, min_speed, dummy_flags = 0;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);

    /** valid port verify */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cannot remove STIF port. port=%d\n", port);
    }
    /** flags verify */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "flags are not supported 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /** Removing ERP port is not supported */
    if (DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_type))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - removing ERP port is not supported.\n", port);
    }

    /*
     * Port can be removed only if its egr and sch shapers were set to zero
     */
    /*
     * Checking speed of egr port shaper
     */
    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
    {
        BCM_GPORT_LOCAL_SET(gport, port);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_get(unit, gport, 0, &min_speed, &port_speed, &dummy_flags));
        if (port_speed > 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - port can't be removed if egr shapers were not set to 0. Current egr port shaper speed is %d!\n",
                         port, port_speed);
        }
    }

    /*
     * Checking speed of sch port shaper
     */
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_type))
    {
        BCM_COSQ_GPORT_E2E_PORT_SET(gport, port);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_bandwidth_get(unit, gport, 0, &min_speed, &port_speed, &dummy_flags));
        if (port_speed > 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d - port can't be removed if sch shapers were not set to 0. Current sch port shaper speed is %d!\n",
                         port, port_speed);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Adding new logical port info to port mgmt db (algo_port_*) 
 *          For additional info about params goto 'bcm_dnx_port_add()'  definition.
 */
static shr_error_e
dnx_port_mgmt_db_add(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    dnx_algo_port_type_e port_type;
    dnx_algo_port_if_add_t if_info;
    dnx_algo_port_tm_add_t *tm_info;
    dnx_algo_port_special_interface_add_t special_info;
    int lane, nof_lanes, first_phy_port;
    int is_not_init_sequence = 0;
    bcm_port_t master_port;
    int interface_id;
    int is_stif = 0, is_elk = 0;
    int is_ilkn_over_fabric = 0;
    SHR_FUNC_INIT_VARS(unit);

    is_not_init_sequence = (dnx_init_is_init_done_get(unit));

    /*
     * init IF info to 0
     */
    sal_memset(&if_info, 0, sizeof(dnx_algo_port_if_add_t));

    /*
     * if ELK port, update flag
     */
    if (flags & BCM_PORT_ADD_KBP_PORT)
    {
        is_elk = 1;
        if (is_not_init_sequence)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "KBP/ELK port is not supported dynamically");
        }
    }
    /*
     * add indication for STIF port
     */
    if (flags & BCM_PORT_ADD_STIF_PORT)
    {
        is_stif = 1;
        if (is_not_init_sequence)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "STIF port is not supported dynamically");
        }
    }

    if ((interface_info->interface == BCM_PORT_IF_ILKN) && (flags & BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT))
    {
        is_ilkn_over_fabric = 1;
    }

    /*
     * Convert interface type to port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_type_get
                    (unit, interface_info->interface, is_elk, is_stif, &port_type));

    /*
     * Add to port per port type 
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, TRUE, TRUE))
    {
        if_info.interface = interface_info->interface;
        if_info.core = mapping_info->core;
        if_info.ilkn_info.is_elk_if = is_elk;
        if_info.eth_info.is_stif = is_stif;
        if_info.ilkn_info.is_ilkn_over_fabric = is_ilkn_over_fabric;

        /*
         * Set relevant parameters to nif port. 
         */
        /*
         * Interface parameters
         */
        if_info.interface_offset = interface_info->interface_id;
        if (interface_info->interface == BCM_PORT_IF_ILKN)
        {
            /*
             * Set NIF phy bitmap (0-95) out of ilkn bitmap 
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lanes_to_nif_phys_get
                            (unit, interface_info->interface_id, &interface_info->phy_pbmp, &if_info.phys));

            /*
             * Set ILKN bitmap (0-23) 
             */
            BCM_PBMP_ASSIGN(if_info.ilkn_info.ilkn_lanes, interface_info->phy_pbmp);
        }
        else if (interface_info->interface == BCM_PORT_IF_NIF_ETH)
        {
            BCM_PBMP_ASSIGN(if_info.phys, interface_info->phy_pbmp);
        }
        else
        {
            /*
             * legacy type, when using type that is NOT BCM_PORT_IF_NIF_ETH
             * phy_port is 1-based in legacy calls, thus we need to do minus one
             */
            first_phy_port = interface_info->phy_port - 1;

                /** get number of lanes according to legacy type */
            SHR_IF_ERR_EXIT(dnx_algo_port_legacy_interface_to_nof_lanes_get
                            (unit, interface_info->interface, &nof_lanes));
            for (lane = 0; lane < nof_lanes; ++lane)
            {
                BCM_PBMP_PORT_ADD(if_info.phys, first_phy_port + lane);
            }
        }

        tm_info = &if_info.tm_info;

        /** get DB interface ID */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_interface_handle_allocate
                        (unit, port, if_info.interface, if_info.phys, if_info.interface_offset, &interface_id));

    }
    /** TM Non-NIF ports */
    else if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
    {
        /*
         * set special tm interface params
         */
        sal_memset(&special_info, 0, sizeof(dnx_algo_port_special_interface_add_t));
        special_info.interface = interface_info->interface;
        special_info.core = mapping_info->core;

        /*
         * Set interface offset (used by recycle ports and expected to be -1 for every other port)
         */
        special_info.interface_offset = interface_info->interface_id;

        tm_info = &special_info.tm_info;

        /** get DB interface ID */
        SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_id_get
                        (unit, port_type, mapping_info->core, interface_info->interface_id, &interface_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "port type is not supported within this function %d", port_type);
    }

    /** For all TM ports */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
    {
        /*
         * Set Switch port params
         */
        tm_info->tm_port = mapping_info->tm_port;
        tm_info->is_channelized = ((flags & BCM_PORT_ADD_CONFIG_CHANNELIZED) ? 1 : 0);
        tm_info->channel = tm_info->is_channelized ? mapping_info->channel : 0;
        tm_info->num_priorities = mapping_info->num_priorities;

        /**
         * Allocate Egress interface and qpairs
         * Egress queuing ports only
         */
        
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type) &&
            (!dnx_data_dev_init.general.access_only_get(unit)) && (!SOC_IS_J2C(unit)))
        {
            /** allocate egress interface*/
            SHR_IF_ERR_EXIT(dnx_algo_port_if_id_to_master_get(unit, interface_id, &master_port));
            SHR_IF_ERR_EXIT(dnx_egr_queuing_interface_alloc(unit,
                                                            port,
                                                            master_port,
                                                            mapping_info->core,
                                                            interface_info->interface, &(tm_info->egress_interface)));

            /** allocate qpair */
            if (flags & BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID)
            {
                SHR_IF_ERR_EXIT(dnx_ps_db_base_qpair_alloc_with_id
                                (unit, port, tm_info->egress_interface, tm_info->is_channelized, mapping_info->core,
                                 tm_info->num_priorities, mapping_info->base_q_pair));
                tm_info->base_q_pair = mapping_info->base_q_pair;
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_ps_db_base_qpair_alloc
                                (unit, port, tm_info->egress_interface, tm_info->is_channelized, mapping_info->core,
                                 tm_info->num_priorities, &tm_info->base_q_pair));
            }
        }

        /**
         * Allocate HRs
         * E2E SCH ports only
         */
        
        if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_type) &&
            (!dnx_data_dev_init.general.access_only_get(unit)) && (!SOC_IS_J2C(unit)))
        {
            if (DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_type))
            {
                /*
                 * For ERP use fixed HRs
                 */
                tm_info->base_hr = dnx_data_sch.flow.erp_hr_get(unit);
                tm_info->sch_priorities = mapping_info->num_priorities;
            }
            else
            {
                
                tm_info->base_hr = tm_info->base_q_pair;
                tm_info->sch_priorities = mapping_info->num_priorities;
            }
        }
    }
    /*
     * Add to port per port type 
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, TRUE, TRUE))
    {
        /** add nif port */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_add(unit, port, &if_info));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
    {
        /** add special tm port */
        SHR_IF_ERR_EXIT(dnx_algo_port_special_interface_add(unit, port, &special_info));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "port type is not supported within this function %d", port_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verifying 'bcm_dnx_port_get()' 
 *          For additional info goto 'bcm_dnx_port_get()'  definition.
 */
static shr_error_e
dnx_port_get_verify(
    int unit,
    bcm_port_t port,
    uint32 *flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** NULL check */
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "NULL flags parameter");
    SHR_NULL_CHECK(interface_info, _SHR_E_PARAM, "NULL interface_info parameter");
    SHR_NULL_CHECK(mapping_info, _SHR_E_PARAM, "NULL mapping_info parameter");

    /** valid port verify */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get logical port info from port mgmt db (algo_port_*) 
 *          For additional info about params goto 'bcm_dnx_port_get()'  definition.
 */
static shr_error_e
dnx_port_mgmt_db_get(
    int unit,
    bcm_port_t port,
    uint32 *flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    dnx_algo_port_type_e port_type;
    int is_channelized;
    dnx_algo_port_tdm_mode_e tdm_mode;
    int is_elk, is_stif;

    SHR_FUNC_INIT_VARS(unit);

    /** init vars */
    sal_memset(interface_info, 0, sizeof(bcm_port_interface_info_t));
    bcm_port_mapping_info_t_init(mapping_info);
    *flags = 0;

    /** get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    /** get interface type */
    SHR_IF_ERR_EXIT(dnx_algo_port_to_interface_type_get
                    (unit, port_type, &is_elk, &is_stif, &(interface_info->interface)));

    /** get TM info */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
    {
        /** Get tm port and core */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &(mapping_info->core), &(mapping_info->tm_port)));

        /** Get channel_id */
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, (int *) &(mapping_info->channel)));

        /** get flags */
        {
            /** is channelized flag */
            SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &is_channelized));
            if (is_channelized)
            {
                *flags |= BCM_PORT_ADD_CONFIG_CHANNELIZED;
            }
            /** is tdm flag */
            SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));
            if (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
            {
                *flags |= BCM_PORT_ADD_TDM_PORT;
            }
        }

        /** Get EGR Queuing related attributes */
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
        {
            /** Get base_q_pair */
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, (int *) &(mapping_info->base_q_pair)));
            /** Get number of port priorities */
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, (int *) &(mapping_info->num_priorities)));

        }
        else if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_type))
        {
            /** Get number of port priorities */
            SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, port, (int *) &(mapping_info->num_priorities)));
        }

    }

    /** Get PP related attributes */
    if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_type))
    {
        /** Get pp port and core */
        int port_in_lag;
        SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port, &port_in_lag));
        /** get PP port only if port is not part of lag */
        if (!port_in_lag)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, port, &(mapping_info->core), &(mapping_info->pp_port)));
        }
        else
        {
            /*
             * Only fill core ID 
             */
            mapping_info->pp_port = -1;
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &(mapping_info->core)));
        }
    }

    /** Get NIF related attributes */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, TRUE, TRUE))
    {
        /** Get number of lanes */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, (int *) &(interface_info->num_lanes)));

        /** Get phys bitmap */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, TRUE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_lanes_get(unit, port, &(interface_info->phy_pbmp)));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &(interface_info->phy_pbmp)));
        }

        /** Get first phy port */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, (int *) &(interface_info->phy_port)));
        /** Get interface ID */
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, (int *) &(interface_info->interface_id)));
    }

    /** is ELK port flag */
    if (is_elk)
    {
        *flags |= BCM_PORT_ADD_KBP_PORT;
    }

    /** is STIF port */
    if (is_stif)
    {
        *flags |= BCM_PORT_ADD_STIF_PORT;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - counts NOF members in step list
 * 
 * \param [in] unit - unit #
 * \param [in] step_list - pointer to step list
 * \param [out] nof_steps - returned result
 * \return
 *   See shr_error_e
 * \remark
 *   list MUST contain at least one member (last member) with
 *   name  == NULL.
 * \see
 *   * None
 */
static shr_error_e
dnx_port_add_remove_step_list_count_steps(
    int unit,
    const dnx_port_add_remove_step_t * step_list,
    int *nof_steps)
{
    int step_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * all lists must have at least one member - the last one - with name = NULL
     */
    for (step_index = 0; step_list[step_index].name != NULL; ++step_index)
    {
        /*
         * Do nothing 
         */
    }

    *nof_steps = step_index;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Deep free of utilex_seq_step_t
 *          Should be called after 'dnx_port_add_remove_step_list_convert()'
 */
static shr_error_e
dnx_port_add_remove_step_list_destory(
    int unit,
    utilex_seq_step_t * step_list)
{
    int step_index;
    SHR_FUNC_INIT_VARS(unit);

    /** recursive destroy */
    for (step_index = 0; step_list[step_index].step_id != UTILEX_SEQ_STEP_LAST; step_index++)
    {
        if (step_list[step_index].step_sub_array != NULL)
        {
            SHR_IF_ERR_CONT(dnx_port_add_remove_step_list_destory(unit, step_list[step_index].step_sub_array));
        }
    }

    /** destroy current */
    SHR_FREE(step_list);

    SHR_FUNC_EXIT;
}
/**
 * \brief - Deep conversion of dnx_port_add_remove_step_t to utilex_seq_step_t
 *          The conversion allocate memory that should be freed using 'dnx_step_list_destroy()'
 */
static shr_error_e
dnx_port_add_remove_step_list_convert(
    int unit,
    const dnx_port_add_remove_step_t * dnx_step_list,
    utilex_seq_step_t ** step_list)
{
    int list_size;
    int step_index;
    utilex_seq_step_t *step;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get list size
     */
    SHR_IF_ERR_EXIT(dnx_port_add_remove_step_list_count_steps(unit, dnx_step_list, &list_size));
    list_size++; /** Count the last step too */

    /*
     * Allocate memory for list
     */
    *step_list =
        (utilex_seq_step_t *) sal_alloc(sizeof(utilex_seq_step_t) * list_size, "dnx port add remove sequence list");
    SHR_NULL_CHECK(*step_list, _SHR_E_MEMORY, "failed to allocate memory for step list");

    /*
     * Convert each step
     */
    for (step_index = 0; step_index < list_size; step_index++)
    {
        step = &((*step_list)[step_index]);
        SHR_IF_ERR_EXIT(utilex_seq_step_t_init(unit, step));

        step->step_id = dnx_step_list[step_index].name != NULL ? 1 /** won't be used */ : UTILEX_SEQ_STEP_LAST;
        step->step_name = dnx_step_list[step_index].name;
        step->forward = dnx_step_list[step_index].add_func;
        step->backward = dnx_step_list[step_index].remove_func;
        step->dyn_flags = dnx_step_list[step_index].flag_function;
        if (dnx_step_list[step_index].sub_list != NULL)
        {
            SHR_IF_ERR_EXIT(dnx_port_add_remove_step_list_convert
                            (unit, dnx_step_list[step_index].sub_list, &(step->step_sub_array)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Converting port add \ remove sequnce to utilex_seq
 */
static shr_error_e
dnx_port_add_remove_seq_convert(
    int unit,
    const dnx_port_add_remove_step_t * step_list,
    utilex_seq_t * seq)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init seq structure
     */
    SHR_IF_ERR_EXIT(utilex_seq_t_init(unit, seq));

    /*
     * Configure sequence 
     */
    /** Logging */
    seq->bsl_flags = BSL_LOG_MODULE;
    sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "PORT:");
    seq->log_severity = bslSeverityVerbose;

    /** sequence */
    SHR_IF_ERR_EXIT(dnx_port_add_remove_step_list_convert(unit, step_list, &seq->step_list));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Running port add remove sequence forward or backward
 *          The function returns the last step id passed with no errors - to be used by error recovery
 *          And allows to start from a specific step (also to be used by error recovery
 */
static shr_error_e
dnx_port_add_remove_run(
    int unit,
    const dnx_port_add_remove_step_t * step_list,
    int forward,
    int first_step_id,
    int *last_step_id_passed)
{
    utilex_seq_t seq;
    SHR_FUNC_INIT_VARS(unit);

    /** Convert step list to general utilex_seq list */
    SHR_IF_ERR_EXIT(dnx_port_add_remove_seq_convert(unit, step_list, &seq));
    /** set first step id */
    if (first_step_id != -1)
    {
        seq.first_step = first_step_id;
    }
    /** Run list forward, Check error just after utilex seq list destroy */
    SHR_IF_ERR_EXIT(utilex_seq_run(unit, &seq, forward));

exit:
    /** returns the last step id passed with no errors - to be used by error recovery */
    *last_step_id_passed = seq.last_passed_step;
        /** Destroy step list (allocated in 'dnx_port_add_remove_seq_convert()') */
    dnx_port_add_remove_step_list_destory(unit, seq.step_list);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief port add / remove sequence 
 *        * Each step in port add / remove step should be added here
 *        * Both add and remove callbacks gets just unit as parameter
 *          The information about the added port can be extracted from port mgmt,
 *          Using function: 'dnx_algo_port_added_port_get()' or 'dnx_algo_port_removed_port_get()'
 *          In addition each step should check if the port is indeed relevant:
 *          For example: NIF step should filter CPU ports
 *        * The last step must be all NULLs
 */
static const dnx_port_add_remove_step_t port_add_remove_sequence[] = {
    /*NAME                ADD_FUNC                                  REMOVE_FUNC                                  SUB_LIST    FLAG_CB  */   
    /** IngressCongestion must come after PortIngress */
    {"IngressCongestion", dnx_cosq_ingress_port_add_handle,         dnx_cosq_ingress_port_remove_handle,         NULL,        NULL},
    /** MirrorRcy must come after IngressCongestion */
    {"MirrorRcy",         dnx_mirror_rcy_port_add_handle,           dnx_mirror_rcy_port_remove_handle,           NULL,        NULL},
    {"EgqQueuing",        dnx_egr_queuing_port_add_handle,          dnx_egr_queuing_port_remove_handle,          NULL,    j2c_skip},
    {"PortIngress",       dnx_port_ingress_port_add_handle,         dnx_port_ingress_port_remove_handle,         NULL,    j2c_skip},
    {"PortEcgm",          dnx_ecgm_port_add,                        dnx_ecgm_port_remove,                        NULL,    j2c_skip},
    {"ImbPath",           dnx_port_imb_path_port_add_handle,        dnx_port_imb_path_port_remove_handle,        NULL,    j2c_skip},
    /** SchPort must come after EgqQueuing */
    {"SchPort",           dnx_sch_port_add_handle,                  dnx_sch_port_remove_handle,                  NULL,    j2c_skip},
    {"PortTune",          dnx_tune_port_add,                        dnx_tune_port_remove,                        NULL,    j2c_skip},
    {"Egress Visibility", dnx_visibility_port_egress_enable,        dnx_visibility_port_egress_disable,          NULL,    j2c_skip},
    {"Disable&Flash",     NULL,                                     dnx_egr_queuing_port_disable,                NULL,    j2c_skip},
    {"Linkscan",          dnx_linkscan_port_init,                   dnx_linkscan_port_remove,                    NULL,    j2c_skip},
    /** CounterUpdate must be the last step */
    {"CounterUpdate",     dnx_counter_port_add,                     dnx_counter_port_remove,                     NULL,    j2c_skip},
    {NULL,                NULL,                                     NULL,                                        NULL} /* Last step must be all NULLs*/
};
/* *INDENT-ON* */


int
bcm_dnx_port_add(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    int last_step_passed = -1;
    int first_phy = -1;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_add_verify(unit, port, flags, interface_info, mapping_info));

    /*
     * Add to port mgmt db
     */
    if (interface_info->interface == BCM_PORT_IF_NIF_ETH)
    {
        BCM_PBMP_ITER(interface_info->phy_pbmp, first_phy)
        {
            break;
        }
        BCM_PBMP_COUNT(interface_info->phy_pbmp, interface_info->num_lanes);
        interface_info->interface_id = first_phy / interface_info->num_lanes;
    }

    SHR_IF_ERR_EXIT(dnx_port_mgmt_db_add(unit, port, flags, interface_info, mapping_info));
    /*
     * Run 'port_add_remove_sequence' defined above 
     * Skip in access only init, 
     * (in order to allow access per port - just need to add the port to DB)
     */
    if (!dnx_data_dev_init.general.access_only_get(unit))
    {

        SHR_IF_ERR_EXIT(dnx_port_add_remove_run(unit, port_add_remove_sequence, 1, -1, &last_step_passed));
    }

    /*
     * Port add done
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_add_process_done(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 *  Remove and kind of logical port. 
 *  The function will: 
 *  * Free the logical port 
 *  * If the logical port mapped to physical port, and it is only one - will remove the physical port instance.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - Valid logical port # 
 * \param [in] flags - see BCM_PORT_ADD_F_* (currently there are no supported flags)
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_remove(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int last_step_passed = -1;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 port_flags;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Port remove verify
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_remove_verify(unit, port, flags));

    /** gets port info - to be used for error recovery */
    SHR_IF_ERR_EXIT(dnx_port_mgmt_db_get(unit, port, &port_flags, &interface_info, &mapping_info));

    /*
     * Mark as removed port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_remove_process_start(unit, port));

    /*
     * Run 'port_add_remove_sequence' defined above
     */
    SHR_IF_ERR_EXIT(dnx_port_add_remove_run(unit, port_add_remove_sequence, 0, -1, &last_step_passed));

    /*
     * Remove from port mgmt
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_remove(unit, port));

exit:
    if (SHR_FUNC_ERR() && last_step_passed != -1)
    {
        dnx_port_mgmt_db_add(unit, port, port_flags, &interface_info, &mapping_info);
        dnx_port_add_remove_run(unit, port_add_remove_sequence, 1, last_step_passed, &last_step_passed);
        dnx_algo_port_add_process_done(unit, port);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ports attributes
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - Logical port
 * \param [in] flags - Port add flags (BCM_PORT_ADD_F_...)
 * \param [in] interface_info - Interface related Information (for more info \see bcm_dnx_port_add)
 * \param [in] mapping_info - Logical related information (for more info \see bcm_dnx_port_add)
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_port_add API
 */
int
bcm_dnx_port_get(
    int unit,
    bcm_port_t port,
    uint32 *flags,
    bcm_port_interface_info_t * interface_info,
    bcm_port_mapping_info_t * mapping_info)
{
    bcm_port_t local_port;
    int phy_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Convert gport to logical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    local_port = gport_info.local_port;

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_port_get_verify(unit, local_port, flags, interface_info, mapping_info));

    /** retrieve information from port db */
    SHR_IF_ERR_EXIT(dnx_port_mgmt_db_get(unit, local_port, flags, interface_info, mapping_info));

    /** legacy compatibility - need to convert phy to be 1 base*/
    if (interface_info->interface != BCM_PORT_IF_NIF_ETH && interface_info->interface != BCM_PORT_IF_ILKN)
    {
        interface_info->phy_port += 1;
        BCM_PBMP_CLEAR(interface_info->phy_pbmp);
        for (phy_id = interface_info->phy_port; phy_id < interface_info->phy_port + interface_info->num_lanes; phy_id++)
        {
            BCM_PBMP_PORT_ADD(interface_info->phy_pbmp, phy_id);
        }
    }

exit:
    /** Warmboot skipped, because of a usage between pkt_send and pkt_verify.*/
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_init_port_internal_add(
    int unit,
    int init_db_only)
{
    bcm_port_t logical_port;
    bcm_port_interface_info_t if_info;
    bcm_port_mapping_info_t mapping_info;
    const dnx_data_port_static_add_ucode_port_t *ucode_port;
    uint32 flags;
    int lane;
    const dnx_data_nif_ilkn_phys_t *ilkn_phys;
    int is_over_fabric = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
    {
        /*
         * Call 'bcm_port_add' for each port defined by soc property 'ucode_port'
         */
        ucode_port = dnx_data_port.static_add.ucode_port_get(unit, logical_port);
        if (ucode_port->interface != BCM_PORT_IF_NULL)
        {
            /** init vars */
            sal_memset(&if_info, 0, sizeof(bcm_port_interface_info_t));
            bcm_port_mapping_info_t_init(&mapping_info);
            flags = 0;

            /** Set relevant input vars for 'bcm_port_add'*/
            /** interface */
            if_info.interface = ucode_port->interface;
            if_info.interface_id = ucode_port->interface_offset;

            /** interlaken phy bitmap, set use_pbmp_flag */
            if (ucode_port->interface == BCM_PORT_IF_ILKN)
            {
                ilkn_phys = dnx_data_nif.ilkn.phys_get(unit, ucode_port->interface_offset);
                SHR_NULL_CHECK(ilkn_phys, _SHR_E_CONFIG, "interface offset is out of bound");
                BCM_PBMP_ASSIGN(if_info.phy_pbmp, ilkn_phys->bitmap);
            }
            else if (ucode_port->interface == BCM_PORT_IF_NIF_ETH)
            {
                BCM_PBMP_CLEAR(if_info.phy_pbmp);
                for (lane = 0; lane < ucode_port->nof_lanes; ++lane)
                {
                    /*
                     * phy is zero based, calculate each phy and add to PBMP
                     */
                    BCM_PBMP_PORT_ADD(if_info.phy_pbmp, (ucode_port->interface_offset * ucode_port->nof_lanes) + lane);
                }
            }
            else
            {
                /** first PHY one based legacy support */
                if_info.phy_port = ucode_port->interface_offset * ucode_port->nof_lanes + 1;
            }

            if (ucode_port->interface == BCM_PORT_IF_ILKN)
            {
                is_over_fabric = dnx_data_nif.ilkn.properties_get(unit, ucode_port->interface_offset)->is_over_fabric;
                if (is_over_fabric)
                {
                    flags |= BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT;
                }
            }

            /** switch port */
            mapping_info.core = ucode_port->core;
            mapping_info.tm_port = ucode_port->tm_port;
            mapping_info.channel = ucode_port->channel;
            mapping_info.num_priorities = ucode_port->num_priorities;
            /** base queue pair with id flag */
            if (ucode_port->base_q_pair != -1)
            {
                mapping_info.base_q_pair = ucode_port->base_q_pair;
                flags |= BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID;
            }

            /** attributes: kbp, stif, tdm */
            if (ucode_port->is_stif)
            {
                flags |= BCM_PORT_ADD_STIF_PORT;
            }
            if (ucode_port->is_kbp)
            {
                flags |= BCM_PORT_ADD_KBP_PORT;
            }
            if (ucode_port->tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
            {
                flags |= BCM_PORT_ADD_TDM_PORT;
            }
            else if (ucode_port->tdm_mode == DNX_ALGO_PORT_TDM_MODE_PACKET)
            {
                flags |= BCM_PORT_ADD_TDM_QUEUING_ON;
            }

            /** If ILKN or channelized set flag is_channelized */
            if (ucode_port->channel != -1)
            {
                flags |= BCM_PORT_ADD_CONFIG_CHANNELIZED;
            }

            /** add the port */
            if (init_db_only)
            {
                SHR_IF_ERR_EXIT(dnx_port_mgmt_db_add(unit, logical_port, flags, &if_info, &mapping_info));
            }
            else
            {
                if (!SOC_WARM_BOOT(unit))
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(bcm_dnx_port_add(unit, logical_port, flags, &if_info, &mapping_info),
                                             "Adding port %d failed.\n%s%s", logical_port, EMPTY, EMPTY);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_init_port_add(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_init_port_internal_add(unit, 0));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_init_port_add_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /** nothing to do */
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the padding size for every new
 *        port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_port_init_pad_size_config(
    int unit,
    bcm_port_t port)
{
    int padding_size;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, TRUE))
    {
        /** check if there is a new value from soc property */
        padding_size = dnx_data_port.static_add.eth_padding_get(unit, port)->pad_size;

        /** use API to set new value */
        if (padding_size != 0)
        {
            bcm_dnx_port_control_set(unit, port, bcmPortControlPadToSize, padding_size);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the stat oversize for every new
 *        port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_port_init_stat_oversize_config(
    int unit,
    bcm_port_t port)
{
    int stat_jumbo;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, TRUE))
    {
        /** check if there is a new value from soc property */
        stat_jumbo = dnx_data_mib.general.stat_jumbo_get(unit);
        SHR_IF_ERR_EXIT(bcm_dnx_port_control_set(unit, port, bcmPortControlStatOversize, stat_jumbo));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert per port properties to BCM API calls (excluding NIF modules)
 */
static shr_error_e
dnx_port_init_special_ifs_properties_set(
    int unit)
{
    bcm_pbmp_t nif_ports_bitmap;
    bcm_pbmp_t tm_ports_bitmap;
    bcm_port_t logical_port;
    int port_speed;
    bcm_port_resource_t *resource = NULL;
    int port_index = 0;
    int nof_ports;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get special interfaces ports bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &nif_ports_bitmap));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH, 0, &tm_ports_bitmap));
    BCM_PBMP_REMOVE(tm_ports_bitmap, nif_ports_bitmap);

    BCM_PBMP_COUNT(tm_ports_bitmap, nof_ports);
    SHR_ALLOC(resource, nof_ports * sizeof(bcm_port_resource_t), "special ifs resource", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /*
     * Speed
     */
    BCM_PBMP_ITER(tm_ports_bitmap, logical_port)
    {

        port_speed = dnx_data_port.static_add.speed_get(unit, logical_port)->val;
        if (port_speed <= 0)
        {
            port_speed = dnx_data_port.static_add.default_speed_for_special_if_get(unit);
        }
        bcm_port_resource_t_init(&resource[port_index]);

        resource[port_index].port = logical_port;
        resource[port_index].speed = port_speed;
        port_index++;
    }

    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_multi_set(unit, nof_ports, resource));

exit:
    SHR_FREE(resource);
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_port_init_serdes_tx_taps_config(
    int unit,
    bcm_port_t port)
{
    bcm_port_phy_tx_t tx;
    SHR_FUNC_INIT_VARS(unit);

    tx.main = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->main;
    tx.pre = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->pre;
    tx.post = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->post;
    tx.pre2 = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->pre2;
    tx.post2 = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->post2;
    tx.post3 = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->post3;
    tx.tx_tap_mode = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->tx_tap_mode;
    tx.signalling_mode = dnx_data_port.static_add.serdes_tx_taps_get(unit, port)->signalling_mode;

    SHR_IF_ERR_EXIT(bcm_dnx_port_phy_tx_set(unit, port, &tx));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert per port properties to BCM API calls (Only NIF module)
 */
static shr_error_e
dnx_port_init_nif_properties_set(
    int unit)
{
    bcm_pbmp_t nif_ports_bitmap;
    bcm_port_t nif_port;
    int nof_ports;
    int port_index = 0;
    int tx_pam4_precoder;
    int lp_tx_precoder;
    const dnx_data_port_static_add_speed_t *port_speed;
    bcm_port_resource_t *resource = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &nif_ports_bitmap));

    BCM_PBMP_COUNT(nif_ports_bitmap, nof_ports);
    SHR_ALLOC(resource, nof_ports * sizeof(bcm_port_resource_t), "NIF resource", "%s%s%s", EMPTY, EMPTY, EMPTY);

    BCM_PBMP_ITER(nif_ports_bitmap, nif_port)
    {
        port_speed = dnx_data_port.static_add.speed_get(unit, nif_port);
        if (port_speed->val == DNXC_PORT_INVALID_SPEED)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "NIF port MUST have speed");
        }

        bcm_port_resource_t_init(&resource[port_index]);
        resource[port_index].port = nif_port;
        resource[port_index].speed = port_speed->val;
        resource[port_index].fec_type = dnx_data_port.static_add.fec_type_get(unit, nif_port)->val;
        resource[port_index].link_training = dnx_data_port.static_add.link_training_get(unit, nif_port)->val;
        resource[port_index].phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;

        SHR_IF_ERR_EXIT(bcm_dnx_port_resource_default_get(unit, nif_port, 0, &resource[port_index]));

        /** override phy lane config bits with values from SoC property */
        SHR_IF_ERR_EXIT(dnx_port_resource_lane_config_soc_properties_get(unit, nif_port, &resource[port_index]));

        port_index++;
    }

    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_multi_set(unit, nof_ports, resource));

    for (port_index = 0; port_index < nof_ports; port_index++)
    {
        /*
         * padding size config
         */
        SHR_IF_ERR_EXIT(dnx_port_init_pad_size_config(unit, resource[port_index].port));
        /*
         * statistics oversize config
         */
        SHR_IF_ERR_EXIT(dnx_port_init_stat_oversize_config(unit, resource[port_index].port));
        /** set precoder according to SoC property */
        tx_pam4_precoder = dnx_data_port.static_add.tx_pam4_precoder_get(unit, resource[port_index].port)->val;

        if (resource[port_index].link_training == 0)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_phy_control_set
                            (unit, resource[port_index].port, BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE,
                             tx_pam4_precoder));
        }
        else if ((resource[port_index].link_training == 1) && (tx_pam4_precoder == 1))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: if link training is enabled, enabling tx_pam4_precoder is not allowed.",
                         resource[port_index].port);
        }

        /** set lp precoder according to SoC property */
        lp_tx_precoder = dnx_data_port.static_add.lp_tx_precoder_get(unit, resource[port_index].port)->val;

        if (resource[port_index].link_training == 0)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_phy_control_set
                            (unit, resource[port_index].port, BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE,
                             lp_tx_precoder));
        }
        else if ((resource[port_index].link_training == 1) && (lp_tx_precoder == 1))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: if link training is enabled, enabling lp_tx_precoder is not allowed.",
                         resource[port_index].port);
        }

        /*
         * config TX FIR TAPs
         */
        if (resource[port_index].link_training == 0)
        {
            SHR_IF_ERR_EXIT(dnx_port_init_serdes_tx_taps_config(unit, resource[port_index].port));
        }
    }

    /*
     * ILKN lane order remap
     */
    SHR_IF_ERR_EXIT(dnx_port_init_ilkn_lane_order_set(unit));

    /*
     * Enable NIF ports
     */
    BCM_PBMP_ITER(nif_ports_bitmap, nif_port)
    {
        port_speed = dnx_data_port.static_add.speed_get(unit, nif_port);
        if (port_speed->val > 0)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_enable_set(unit, nif_port, 1));
        }
    }

exit:
    SHR_FREE(resource);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Take PP port soc-properties and call the right BCM PP port APIs accordingly.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_init_packet_processing_properties_set(
    int unit)
{
    bcm_port_t port;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    int header_type_in;
    int header_type_out;
    bcm_pbmp_t logical_ports;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &logical_ports));
    BCM_PBMP_ITER(logical_ports, port)
    {

        if (dnx_data_port.static_add.header_type_get(unit, port)->header_type_in == BCM_SWITCH_PORT_HEADER_TYPE_NONE)
        {
            header_type_in = dnx_data_port.static_add.header_type_get(unit, port)->header_type;
        }
        else
        {
            header_type_in = dnx_data_port.static_add.header_type_get(unit, port)->header_type_in;
        }

        key.type = bcmSwitchPortHeaderType;
        key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN;
        value.value = header_type_in;

        /** Set the incoming header type */
        SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, port, key, value));

        if (dnx_data_port.static_add.header_type_get(unit, port)->header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_NONE)
        {
            header_type_out = dnx_data_port.static_add.header_type_get(unit, port)->header_type;
        }
        else
        {
            header_type_out = dnx_data_port.static_add.header_type_get(unit, port)->header_type_out;
        }

        if (header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2 is not supported on egress!");
        }

        key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT;
        value.value = header_type_out;

        /** Get the port`s type */
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
        if (DNX_ALGO_PORT_TYPE_IS_EGR_PP(unit, port_type))
        {
            /** Set the outgoing header type */
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, port, key, value));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h */
shr_error_e
dnx_port_packet_processing_properties_remove_internal(
    int unit,
    bcm_port_t pp_port,
    bcm_core_t core_id)
{
    uint32 entry_handle_id;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    flags = 0;

    SHR_IF_ERR_EXIT(dnx_port_untagged_vlan_set_internal(unit, pp_port, core_id, BCM_VLAN_DEFAULT));
    SHR_IF_ERR_EXIT(dnx_port_pp_learn_set(unit, core_id, pp_port, flags));

    /*
     * Configure the INGRESS_LLR_CONTEXT_PROPERTIES table:
     * Disable LLVP and Initial VID per port.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IRPP_LLR_CONTEXT_PROPERTIES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_VID_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLR_CONTEXT_ID, INST_SINGLE,
                                 DBAL_ENUM_FVAL_LLR_CONTEXT_ID_GENERAL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PORT_SEL_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLR_PTC_PROFILE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FILTER_SA_EQ_DA_ENABLE, INST_SINGLE, 0);
    /*
     * Disable acceptable frame type filter
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_ENABLE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Configure the INGRESS_PP_PORT table: disable learn-lif.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_PP_PORT, entry_handle_id));

    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LEARN_LIF_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT,
                                 INST_SINGLE, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ITMH_A1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PORT_SAME_INTERFACE_FILTER_PROFILE,
                                 INST_SINGLE, DBAL_ENUM_FVAL_IN_PORT_SAME_INTERFACE_FILTER_PROFILE_DISABLE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See h. for reference
 */
shr_error_e
dnx_port_packet_processing_properties_remove(
    int unit,
    bcm_port_t port)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_port_packet_processing_properties_remove_internal
                        (unit, gport_info.internal_port_pp_info.pp_port[pp_port_index],
                         gport_info.internal_port_pp_info.core_id[pp_port_index]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remap the ILKN logical lane order.
 *    Lane order remapping feature is only relevant for ILKN
 *    and ELK ports.
 * 
 * \param [in] unit - chip unit id
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_init_ilkn_lane_order_set(
    int unit)
{
    int ilkn_id;
    int phy_iter;
    int ilkn_lane_id[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF] = { 0 };
    int count = 0;
    bcm_pbmp_t nif_phys;
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &logical_ports));

    BCM_PBMP_ITER(logical_ports, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

        /*
         * This feature is only relevant for ILKN and ELK ports 
         */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1 /* include elk */ ))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &nif_phys));

            count = 0;
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
            BCM_PBMP_ITER(nif_phys, phy_iter)
            {
                /*
                 * Get the ILKN lane re-mapping configuration from Soc property 
                 */
                ilkn_lane_id[count] = dnx_data_lane_map.ilkn.remapping_get(unit, phy_iter, ilkn_id)->ilkn_lane_id;
                count++;
            }
            SHR_IF_ERR_EXIT(bcm_dnx_port_ilkn_lane_id_map_set(unit, 0, port, count, ilkn_lane_id));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_init_port_properties_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (!SOC_IS_J2C(unit))
    {
        /*
         * NIF
         */
        SHR_IF_ERR_EXIT(dnx_port_init_nif_properties_set(unit));

        /*
         * Special interfaces (TM ports excluding NIF)
         */
        SHR_IF_ERR_EXIT(dnx_port_init_special_ifs_properties_set(unit));
    }

    /*
     * PP
     */
    SHR_IF_ERR_EXIT(dnx_port_init_packet_processing_properties_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_init_port_properties_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /** nothing to do */

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_init_port_lane_map_set(
    int unit)
{
    int flags = 0, lane_id;
    int map_size = dnx_data_nif.phys.nof_phys_get(unit);
    bcm_port_lane_to_serdes_map_t lane2serdes[DNX_DATA_MAX_NIF_PHYS_NOF_PHYS];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set lane mapping for NIF phys
     */
    /** Get lane mapping info from dnx-data */
    for (lane_id = 0; lane_id < map_size; lane_id++)
    {
        lane2serdes[lane_id].serdes_rx_id = dnx_data_lane_map.nif.mapping_get(unit, lane_id)->serdes_rx_id;
        lane2serdes[lane_id].serdes_tx_id = dnx_data_lane_map.nif.mapping_get(unit, lane_id)->serdes_tx_id;
    }
    /** Store lane mapping info to swstate */
    SHR_IF_ERR_EXIT(bcm_dnx_port_lane_to_serdes_map_set(unit, flags, map_size, lane2serdes));

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_port_init_port_lane_map_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /** nothing to do */
    SHR_FUNC_EXIT;
}
