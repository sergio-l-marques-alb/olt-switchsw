/** \file imb_cdu.c
 *
 *  Ethernet ports procedures for DNX.
 *
 */
/*
 *
 * $Id:$
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 *
 */

#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_cdu.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/common/link.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/phy/phymod_sim.h>
#include <soc/dnx/pll/pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/legacy/SAND/SAND_FM/sand_pp_general.h>
#include <src/bcm/dnx/port/port_utils.h>
#include "imb_cdu_internal.h"
#include "imb_utils.h"
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/stat/stif/stif_mgmt.h>
#include <soc/dnxc/legacy/dnxc_port.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/switch/switch.h>
#include <soc/sand/sand_signals.h>

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/* Static function declerations */
static int imb_cdu_portmod_add_info_config(
    int unit,
    bcm_port_t port,
    portmod_port_add_info_t * add_info);

static int imb_cdu_port_priority_config_db_update(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config);

static int imb_cdu_port_remove_rmc_lane_unmap(
    int unit,
    bcm_port_t port);

static int imb_cdu_port_priority_config_enable_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc_arr,
    int nof_rmcs,
    int enable);

static int imb_cdu_port_macro_config(
    int unit,
    int pm_in_cdu_index,
    const imb_create_info_t * imb_info,
    portmod_pm_create_info_t * pm_info);

/**
 * \brief - call back function to portmod to reset port credit
 *
 */
int
imb_cdu_portmod_soft_reset(
    int unit,
    soc_port_t port,
    portmod_call_back_action_type_t action)
{
    uint32 reg[1];
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));

    switch (action)
    {
        case portmodCallBackActionTypeDuring:
            SHR_IF_ERR_EXIT(imb_cdu_port_credit_tx_reset(unit, port));
            break;
        case portmodCallBackActionTypePre:
            
            if (cdu_info.inner_cdu_id == 0)
            {
                if (cdu_info.mac_id == 0)
                {
                    SHR_IF_ERR_EXIT(READ_CDUM_MAC_0_RESETr(unit, cdu_info.core, reg));
                    SHR_BITCLR(reg, (cdu_info.first_lane_in_cdu % 4));
                    SHR_IF_ERR_EXIT(WRITE_CDUM_MAC_0_RESETr(unit, cdu_info.core, reg[0]));
                }
                else
                {
                    SHR_IF_ERR_EXIT(READ_CDUM_MAC_1_RESETr(unit, cdu_info.core, reg));
                    SHR_BITCLR(reg, (cdu_info.first_lane_in_cdu % 4));
                    SHR_IF_ERR_EXIT(WRITE_CDUM_MAC_1_RESETr(unit, cdu_info.core, reg[0]));
                }
            }
            else
            {
                if (cdu_info.mac_id == 0)
                {
                    SHR_IF_ERR_EXIT(soc_reg32_get
                                    (unit, CDU_MAC_0_RESETr, cdu_info.core * 5 + cdu_info.inner_cdu_id - 1, 0, reg));
                    SHR_BITCLR(reg, (cdu_info.first_lane_in_cdu % 4));
                    SHR_IF_ERR_EXIT(soc_reg32_set
                                    (unit, CDU_MAC_0_RESETr, cdu_info.core * 5 + cdu_info.inner_cdu_id - 1, 0, reg[0]));
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_reg32_get
                                    (unit, CDU_MAC_1_RESETr, cdu_info.core * 5 + cdu_info.inner_cdu_id - 1, 0, reg));
                    SHR_BITCLR(reg, (cdu_info.first_lane_in_cdu % 4));
                    SHR_IF_ERR_EXIT(soc_reg32_set
                                    (unit, CDU_MAC_1_RESETr, cdu_info.core * 5 + cdu_info.inner_cdu_id - 1, 0, reg[0]));
                }
            }
            break;

        case portmodCallBackActionTypePost:
            
            if (cdu_info.inner_cdu_id == 0)
            {
                if (cdu_info.mac_id == 0)
                {
                    SHR_IF_ERR_EXIT(READ_CDUM_MAC_0_RESETr(unit, cdu_info.core, reg));
                    SHR_BITSET(reg, (cdu_info.first_lane_in_cdu % 4));
                    SHR_IF_ERR_EXIT(WRITE_CDUM_MAC_0_RESETr(unit, cdu_info.core, reg[0]));
                }
                else
                {
                    SHR_IF_ERR_EXIT(READ_CDUM_MAC_1_RESETr(unit, cdu_info.core, reg));
                    SHR_BITSET(reg, (cdu_info.first_lane_in_cdu % 4));
                    SHR_IF_ERR_EXIT(WRITE_CDUM_MAC_1_RESETr(unit, cdu_info.core, reg[0]));
                }
            }
            else
            {
                if (cdu_info.mac_id == 0)
                {
                    SHR_IF_ERR_EXIT(soc_reg32_get
                                    (unit, CDU_MAC_0_RESETr, cdu_info.core * 5 + cdu_info.inner_cdu_id - 1, 0, reg));
                    SHR_BITSET(reg, (cdu_info.first_lane_in_cdu % 4));
                    SHR_IF_ERR_EXIT(soc_reg32_set
                                    (unit, CDU_MAC_0_RESETr, cdu_info.core * 5 + cdu_info.inner_cdu_id - 1, 0, reg[0]));
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_reg32_get
                                    (unit, CDU_MAC_1_RESETr, cdu_info.core * 5 + cdu_info.inner_cdu_id - 1, 0, reg));
                    SHR_BITSET(reg, (cdu_info.first_lane_in_cdu % 4));
                    SHR_IF_ERR_EXIT(soc_reg32_set
                                    (unit, CDU_MAC_1_RESETr, cdu_info.core * 5 + cdu_info.inner_cdu_id - 1, 0, reg[0]));
                }
            }
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize CDU:
 * 1. Intialize CDU Database.
 * 2. Set some Reg values to get to desired SW start point. 3.
 * 3. Initialize PM init info for PM below and call
 * port_macro_add()
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info required for
 *        initalization.
 * \param [out] imb_specific_info - IMB specific info to be
 *        initialized.
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
imb_cdu_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    int pm_i, rmc, nof_rmcs, nof_cdus_per_core, cdu_id;
    bcm_core_t core;
    portmod_pm_create_info_t pm_info;
    portmod_default_user_access_t *user_acc;
    uint64 pm_cfg;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * 1. Initialize CDU DB
     */
    user_acc = sal_alloc(sizeof(portmod_default_user_access_t), "CDU user access");
    SHR_NULL_CHECK(user_acc, _SHR_E_MEMORY, "user_access");
    SHR_IF_ERR_EXIT(portmod_default_user_access_t_init(unit, user_acc));
    PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET(user_acc);
    user_acc->unit = unit;
    user_acc->blk_id = CDPORT_BLOCK(unit, imb_info->inst_id);
    user_acc->mutex = sal_mutex_create("pm mutex");
    SHR_NULL_CHECK(user_acc->mutex, _SHR_E_MEMORY, "user_access->mutex");
    imb_specific_info->cdu.user_acc = user_acc;

    /**
     * 2. initialize register values
     */
    if (!SOC_WARM_BOOT(unit))
    {
        nof_rmcs = dnx_data_nif.eth.cdu_logical_fifo_nof_get(unit);
        nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
        core = imb_info->inst_id >= nof_cdus_per_core ? 1 : 0;
        cdu_id = imb_info->inst_id % nof_cdus_per_core;
        /*
         * Disable data to Portmacro
         */
        SHR_IF_ERR_EXIT(imb_cdu_tx_data_to_pm_enable_set(unit, core, cdu_id, IMB_CDU_ALL_LANES, 0));
        /*
         * Clear all Schedulers
         */
        SHR_IF_ERR_EXIT(imb_cdu_scheduler_config_set(unit, core, cdu_id, bcmPortNifSchedulerLow, 0));
        SHR_IF_ERR_EXIT(imb_cdu_scheduler_config_set(unit, core, cdu_id, bcmPortNifSchedulerHigh, 0));
        SHR_IF_ERR_EXIT(imb_cdu_scheduler_config_set(unit, core, cdu_id, bcmPortNifSchedulerTDM, 0));
        /*
         * Set all RMCs thrshold after overflow to 0.
         */
        for (rmc = 0; rmc < nof_rmcs; ++rmc)
        {
            SHR_IF_ERR_EXIT(imb_cdu_port_rmc_thr_after_ovf_set(unit, core, cdu_id, rmc, 0));
        }
        /*
         * Enable EEE for the CDU - this does not actually enable the EEE for the ports, but if this bit is not set,
         * EEE will not work
         */
        SHR_IF_ERR_EXIT(imb_cdu_eee_enable_set(unit, core, cdu_id, 1));

        if (cdu_id == 0)
        {
            
            SHR_IF_ERR_EXIT(READ_CDUM_AUTO_DOC_NAME_1r(unit, core, &pm_cfg));
            soc_reg64_field32_set(unit, CDUM_AUTO_DOC_NAME_1r, &pm_cfg, FIELD_43_43f, 0);
            SHR_IF_ERR_EXIT(WRITE_CDUM_AUTO_DOC_NAME_1r(unit, core, pm_cfg));
        }
        else
        {
            
            SHR_IF_ERR_EXIT(soc_reg64_get(unit, CDU_AUTO_DOC_NAME_1r, core * 5 + cdu_id - 1, 0, &pm_cfg));
            soc_reg64_field32_set(unit, CDU_AUTO_DOC_NAME_1r, &pm_cfg, FIELD_43_43f, 0);
            SHR_IF_ERR_EXIT(soc_reg64_set(unit, CDU_AUTO_DOC_NAME_1r, core * 5 + cdu_id - 1, 0, pm_cfg));
        }
        /*
         * Set PRD bypass mode to 0.
         * This is a debug feature to bypass the PRD parser, and it is turned on by default.
         * it should not be exposed as API.
         * this will enable working with the PRD later on.
         */
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_bypass_set(unit, core, cdu_id, 0));
    }

    /**
     * 3. initialize portmod_pm_create_info_t
     */
    for (pm_i = 0; pm_i < dnx_data_nif.eth.cdu_pms_nof_get(unit); pm_i++)
    {
        SHR_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
        SHR_IF_ERR_EXIT(imb_cdu_port_macro_config(unit, pm_i, imb_info, &pm_info));

        /*
         * initalize specific pm8x50 info - after pm8x50 is implemented
         */
        SHR_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - De-initialize the CDU. main task is to free
 *        dynamically allocated memory.
 *
 * \param [in] unit - chip unit id.
 * \param [in] imb_info - IMB info IMB info required for
 *        de-init.
 * \param [out] imb_specific_info - IMB specific info to be
 *        de-initialized.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * Should only be called during de-init process.
 * \see
 *   * None
 */
int
imb_cdu_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    portmod_default_user_access_t *user_acc;
    SHR_FUNC_INIT_VARS(unit);

    user_acc = imb_specific_info->cdu.user_acc;

    if (user_acc != NULL)
    {
        if (user_acc->mutex != NULL)
        {
            sal_mutex_destroy(user_acc->mutex);
        }

        sal_free(user_acc);
        imb_specific_info->cdu.user_acc = NULL;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize Portmod add_info before calling
 *        portmod_port_add
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] add_info - portmod add info
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
imb_cdu_portmod_add_info_config(
    int unit,
    bcm_port_t port,
    portmod_port_add_info_t * add_info)
{
    int i, nof_phys;
    int pm_lower_bound, pm_upper_bound;
    soc_dnxc_lane_map_db_map_t lane2serdes[DNX_DATA_MAX_NIF_ETH_CDU_LANES_NOF];

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, add_info));

    /*
     * Configure lane map info. Get the lane map info from swstate 
     */
    SHR_IF_ERR_EXIT(imb_cdu_port_pm_boundary_get(unit, port, &pm_lower_bound, &pm_upper_bound));
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_pm_lane_to_serdes_map_get
                    (unit, DNX_ALGO_LANE_MAP_NIF_SIDE, pm_lower_bound, pm_upper_bound, lane2serdes));

    for (i = 0; i < dnx_data_nif.eth.cdu_lanes_nof_get(unit); i++)
    {
        add_info->init_config.lane_map[0].lane_map_rx[i] = lane2serdes[i].rx_id;
        add_info->init_config.lane_map[0].lane_map_tx[i] = lane2serdes[i].tx_id;
    }
    add_info->init_config.lane_map[0].num_of_lanes = dnx_data_nif.eth.cdu_lanes_nof_get(unit);
    add_info->init_config.lane_map_overwrite = 1; /** we always overwrite lane map*/
    add_info->init_config.polarity_overwrite = 0; /** same as in pm create, we don't need to overwrite*/
    add_info->init_config.fw_load_method_overwrite = 0; /** same as in pm create, we don't need to overwrite*/
    add_info->init_config.ref_clk_overwrite = 0; /** same as in pm create, we don't need to overwrite*/
    /** get port phys */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, (bcm_pbmp_t *) & (add_info->phys)));
    /** get Max speed */
    PORTMOD_PBMP_COUNT(add_info->phys, nof_phys);
    add_info->interface_config.max_speed = dnx_data_nif.eth.max_speed_get(unit, nof_phys)->speed;

    PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_SET(add_info);
    PORTMOD_PORT_ADD_F_AUTONEG_CONFIG_SKIP_SET(add_info);
    PORTMOD_PORT_ADD_F_RX_SRIP_CRC_SET(add_info);

    add_info->interface_config.interface = SOC_PORT_IF_NIF_ETH;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get port macro id by logical port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] pm_id - port macro id
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
imb_cdu_port_pm_id_get(
    int unit,
    bcm_port_t port,
    int *pm_id)
{
    int first_phy_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0x0, &first_phy_port));
    SHR_IF_ERR_EXIT(portmod_phy_pm_id_get(unit, first_phy_port, pm_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - this function will configure a struct of type portmod_pm_create_info_t
 *          before it is sent to portmod_port_macro_add
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - imb info to retrive CDU id
 * \param [in] pm_in_cdu_index - pm index inside cdu
 * \param [in] pm_info - pm configuration sturct
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
imb_cdu_port_macro_config(
    int unit,
    int pm_in_cdu_index,
    const imb_create_info_t * imb_info,
    portmod_pm_create_info_t * pm_info)
{
    bcm_port_t phy;
    bcm_pbmp_t pm_pbmp;
    int is_sim;
    int pm_index;
    int lane;
    uint32 rx_polarity, tx_polarity;
    phymod_lane_map_t lane_map = { 0 };
    DNX_SERDES_REF_CLOCK_TYPE ref_clk = DNX_SERDES_NOF_REF_CLOCKS;
    int lcpll;

    SHR_FUNC_INIT_VARS(unit);

    lcpll = imb_info->inst_id >= dnx_data_nif.eth.cdu_nof_per_core_get(unit) ? 1 : 0;
    /** update type and phys */
    pm_info->type = portmodDispatchTypePm8x50;
    pm_index = dnx_data_nif.eth.cdu_pm_map_get(unit, imb_info->inst_id)->pms[pm_in_cdu_index];
    pm_pbmp = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;
    BCM_PBMP_ITER(pm_pbmp, phy)
    {
        BCM_PBMP_PORT_ADD(pm_info->phys, phy);
        /** lane polarity get */
        rx_polarity = dnx_data_nif.phys.polarity_get(unit, (int) phy)->rx_polarity;
        tx_polarity = dnx_data_nif.phys.polarity_get(unit, (int) phy)->tx_polarity;
        lane = (int) phy % dnx_data_nif.eth.cdu_lanes_nof_get(unit);
        pm_info->pm_specific_info.pm8x50.polarity.rx_polarity |= ((rx_polarity & 0x1) << lane);
        pm_info->pm_specific_info.pm8x50.polarity.tx_polarity |= ((tx_polarity & 0x1) << lane);
    }

    ref_clk = dnx_data_pll.general.nif_pll_cfg_get(unit, lcpll)->out_freq;
    if (ref_clk == DNX_SERDES_REF_CLOCK_BYPASS)
    {
        ref_clk = dnx_data_pll.general.nif_pll_cfg_get(unit, lcpll)->in_freq;
    }
    SHR_IF_ERR_EXIT(dnx_to_phymod_ref_clk(unit, ref_clk, &(pm_info->pm_specific_info.pm8x50.ref_clk)));

    /** As we do not know the speeds of the ports on the PM yet,we set vco to null */
    pm_info->pm_specific_info.pm8x50.tvco = portmodVCOInvalid;
    pm_info->pm_specific_info.pm8x50.ovco = portmodVCOInvalid;

    /** update access struct with cdu info */
    SHR_IF_ERR_EXIT(phymod_access_t_init(&(pm_info->pm_specific_info.pm8x50.access.access)));
    pm_info->pm_specific_info.pm8x50.access.access.user_acc = imb_info->imb_specific_info.cdu.user_acc;
    pm_info->pm_specific_info.pm8x50.access.access.addr = imb_info->inst_id * dnx_data_nif.eth.cdu_lanes_nof_get(unit); /* PHY
                                                                                                                         * addresses
                                                                                                                         * are
                                                                                                                         * 0,8,16... */
    pm_info->pm_specific_info.pm8x50.access.access.bus = NULL;  /* Use default bus */
    pm_info->pm_specific_info.pm8x50.access.access.tvco_pll_index =
        dnx_data_nif.eth.pm_properties_get(unit, pm_index)->tvco_pll_index;
    SHR_IF_ERR_EXIT(soc_physim_check_sim
                    (unit, phymodDispatchTypeTscbh, &(pm_info->pm_specific_info.pm8x50.access.access), 0, &is_sim));

    if (is_sim)
    {
        pm_info->pm_specific_info.pm8x50.fw_load_method = phymodFirmwareLoadMethodNone;
        /** enable clause45 just for sim - used by phy sim */
        PHYMOD_ACC_F_CLAUSE45_SET(&pm_info->pm_specific_info.pm8x50.access.access);
        PHYMOD_ACC_F_PHYSIM_SET(&pm_info->pm_specific_info.pm8x50.access.access);
    }
    else
    {
        pm_info->pm_specific_info.pm8x50.fw_load_method = dnx_data_port.static_add.nif_fw_load_method_get(unit);
    }

    pm_info->pm_specific_info.pm8x50.lane_map = lane_map; /** lane map soc property will be read at a later stage, we pass an empty lane map for now*/

    pm_info->pm_specific_info.pm8x50.external_fw_loader = NULL; /**Tells Portmod to use default external loader */
    pm_info->pm_specific_info.pm8x50.portmod_mac_soft_reset = imb_cdu_portmod_soft_reset;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - cdu_port_remove helper function to unmap the RMCs
 *        from the physical lanes.
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
static int
imb_cdu_port_remove_rmc_lane_unmap(
    int unit,
    bcm_port_t port)
{
    int ii, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc_arr[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF];
    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (ii = 0; ii < nof_priority_groups; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc_arr[ii]));
        if (rmc_arr[ii].rmc_id == IMB_CDU_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        /*
         * set the RMC to invalid values
         */
        rmc_arr[ii].first_entry = 0;
        rmc_arr[ii].last_entry = 0;
        rmc_arr[ii].prd_priority = IMB_PRD_PRIORITY_ALL;
        rmc_arr[ii].thr_after_ovf = 0;
    }

    SHR_IF_ERR_EXIT(imb_cdu_port_priority_config_enable_set(unit, port, rmc_arr, nof_priority_groups, 0));

    for (ii = 0; ii < nof_priority_groups; ++ii)
    {
        rmc_arr[ii].rmc_id = IMB_CDU_RMC_INVALID;
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_set(unit, port, 0, ii, &rmc_arr[ii]));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get all CDU ports on the same MAC
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] mac_ports - bitmap of ports on the same mac
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
imb_cdu_mac_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * mac_ports)
{
    bcm_port_t port_i;
    bcm_pbmp_t cdu_ports;
    dnx_algo_port_cdu_access_info_t cdu_info, port_i_cdu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * clear output pbmp
     */
    BCM_PBMP_CLEAR(*mac_ports);

    /*
     * get port and CDU info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(imb_cdu_master_ports_get(unit, port, &cdu_ports));

    /*
     * iterate over all CDU ports and check if MAC is is the same as given port
     */
    BCM_PBMP_ITER(cdu_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port_i, &port_i_cdu_info));
        if (port_i_cdu_info.mac_id == cdu_info.mac_id)
        {
            BCM_PBMP_PORT_ADD(*mac_ports, port_i);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static int
imb_cdu_mac_nof_ports_get(
    int unit,
    bcm_port_t port,
    int *mac_nof_ports)
{
    bcm_pbmp_t mac_ports;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_mac_ports_get(unit, port, &mac_ports));
    BCM_PBMP_COUNT(mac_ports, *mac_nof_ports);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the appropriate MAC operating mode from all
 *        enabled ports on the MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] mac_mode - MAC operating mode
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
imb_cdu_port_mac_mode_get(
    int unit,
    bcm_port_t port,
    int *mac_mode)
{
    int nof_phys, port_i_nof_phys, check_3_port_mode, nof_phys_for_3_ports_check;
    bcm_port_t port_i;
    bcm_pbmp_t mac_ports;
    dnx_algo_port_cdu_access_info_t cdu_info, port_i_cdu_info;
    SHR_FUNC_INIT_VARS(unit);

    check_3_port_mode = 0;

    /*
     * get port nof phys
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_phys));

    switch (nof_phys)
    {
        case 8:
            *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_ONE_PORT_FOR_TWO_MACS;
            break;
        case 4:
            *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_ONE_PORT;
            break;
        case 2:
            /*
             * set the MAC mode to 2 ports mode, and mark to check for 3 ports mode
             */
            *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_TWO_PORTS;
            check_3_port_mode = 1;
            /*
             * 3 port mode means on the same quad we have ports with 2 lanes ad ports with single lane
             */
            nof_phys_for_3_ports_check = 1;
            break;
        case 1:
            /*
             * set the MAC mode to 4 ports mode, and mark to check for 3 ports mode
             */
            *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_FOUR_PORTS;
            check_3_port_mode = 1;
            /*
             * 3 port mode means on the same quad we have ports with 2 lanes ad ports with single lane
             */
            nof_phys_for_3_ports_check = 2;
            break;
    }

    if (check_3_port_mode)
    {
        /**check if we are in 3 ports mode*/
        /*
         * get port info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
        SHR_IF_ERR_EXIT(imb_cdu_mac_ports_get(unit, port, &mac_ports));
        /*
         * iterate over all ports on the MAC to see if we are in three ports mode
         */
        BCM_PBMP_PORT_REMOVE(mac_ports, port);
        BCM_PBMP_ITER(mac_ports, port_i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port_i, &port_i_nof_phys));
            /*
             * check if port_i number of phys matches 3 ports mode
             */
            if (port_i_nof_phys == nof_phys_for_3_ports_check)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port_i, &port_i_cdu_info));

                if (((nof_phys > port_i_nof_phys) && (cdu_info.first_lane_in_cdu > port_i_cdu_info.first_lane_in_cdu))
                    || ((nof_phys < port_i_nof_phys)
                        && (cdu_info.first_lane_in_cdu < port_i_cdu_info.first_lane_in_cdu)))
                {
                    *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2;
                    break;
                }
                else
                {
                    *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_2_3;
                    break;
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map NIF port to EGQ interface
 *
 * \param [in] unit   - chip unit id
 * \param [in] port   - logical port
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_cdu_port_egq_interface_map(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    dnx_algo_port_type_e port_type;
    int first_phy;
    int egq_if;
    int nof_cdus_per_core;
    int nof_phys_per_cdu;
    int nof_phys_per_core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
    {
        /*
         * Calc phys per core
         */
        nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
        nof_phys_per_cdu = dnx_data_nif.eth.cdu_lanes_nof_get(unit);
        nof_phys_per_core = nof_cdus_per_core * nof_phys_per_cdu;

        /*
         * Get CDU access info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));

        /*
         * get port first PHY.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

        /*
         * get port egress interface
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));

        /*
         * Call internal function
         */
        SHR_IF_ERR_EXIT(imb_cdu_internal_nif_egq_interface_map
                        (unit, cdu_info.core, first_phy % nof_phys_per_core, egq_if));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new port to the CDU. config all CDU settings and
 *         call Portmod API for PM configurations
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port to be attched to the CDU
 * \param [in] flags - not used
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
imb_cdu_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int mac_mode = 0;
    int pm_id;
    portmod_port_add_info_t add_info;
    portmod_pause_control_t pause_control;
    portmod_rx_control_t rx_ctrl;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * 1.  Enable TX data to PM
     */
    SHR_IF_ERR_EXIT(imb_cdu_port_tx_data_to_pm_enable_set(unit, port, 1));
    /*
     * 5. Set CDU port mode
     */
    SHR_IF_ERR_EXIT(imb_cdu_port_mac_mode_get(unit, port, &mac_mode));
    SHR_IF_ERR_EXIT(imb_cdu_port_mac_enable_set(unit, port, mac_mode, 1));
    /*
     * 6. Reset Async unit
     */
    SHR_IF_ERR_EXIT(imb_cdu_port_async_unit_reset(unit, port, IMB_CDU_OUT_OF_RESET));
    

    /*
     * 12.  Port bond option
     */
    SHR_IF_ERR_EXIT(imb_cdu_port_pm_reset(unit, port, IMB_CDU_OUT_OF_RESET));

    /*
     * 13.  Call Portmod API
     */
    SHR_IF_ERR_EXIT(imb_cdu_portmod_add_info_config(unit, port, &add_info));

    /*
     * TBD - make sure the PM is not enbaled in this function (i.e portmod_port_enable registers are not activated
     * here)
     */

    SHR_IF_ERR_EXIT(imb_cdu_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_EXIT(portmod_core_add(unit, pm_id, &add_info));
    SHR_IF_ERR_EXIT(portmod_port_add(unit, port, &add_info));
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, 0));

    /*
     * Pause - to be consistent with legacy disable TX and enable RX 
     */
    SHR_IF_ERR_EXIT(portmod_pause_control_t_init(unit, &pause_control));
    pause_control.tx_enable = FALSE;
    pause_control.rx_enable = TRUE;
    SHR_IF_ERR_EXIT(portmod_port_pause_control_set(unit, port, &pause_control));

    /*
     * Control Frames - to be consistent with legacy control frames (
     * including PFC frames wih ether type 0x8808) should be dropped in CDMAC  
     */
    rx_ctrl.flags = PORTMOD_MAC_PASS_CONTROL_FRAME;
    rx_ctrl.pass_control_frames = FALSE;
    SHR_IF_ERR_EXIT(portmod_port_rx_control_set(unit, port, &rx_ctrl));

    SHR_IF_ERR_EXIT(imb_cdu_port_egq_interface_map(unit, port));

    /*
     * 2. Configure the SCH
     * 7. Map RMC to physical port
     * 8. Map physical port to RMC
     * 9. Set RX threshold after overflow
     * 4. Reset ports in NMG
     * 11. Reset ports in CDU
     *
     * --> These stages are done during port level tuning.
     * it is important all tuning will be done before the port is enabled!
     *
     *
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get number of ports on a port's CDU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] cdu_nof_ports - number of ports on the CDU
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
imb_cdu_nof_ports_get(
    int unit,
    bcm_port_t port,
    int *cdu_nof_ports)
{
    bcm_pbmp_t cdu_ports;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_master_ports_get(unit, port, &cdu_ports));
    BCM_PBMP_COUNT(cdu_ports, *cdu_nof_ports);
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - get number of ilkn ports on a port's CDU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] cdu_nof_ilkn_ports - number of ilkn ports on the CDU
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
imb_cdu_nof_ilkn_ports_get(
    int unit,
    bcm_port_t port,
    int *cdu_nof_ilkn_ports)
{
    bcm_pbmp_t cdu_ilkn_ports;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_ilkn_master_ports_get(unit, port, &cdu_ilkn_ports));
    BCM_PBMP_COUNT(cdu_ilkn_ports, *cdu_nof_ilkn_ports);
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Remove a port from the CDU. first calls Portmos API
 *        to remove from portmod DB, then configures all CDU
 *        settings.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_detach(
    int unit,
    bcm_port_t port)
{
    int cdu_nof_eth_ports = 0, cdu_nof_ilkn_ports = 0, mac_nof_ports = 0, nof_channels = 0;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * 13.  Call Portmod API
     */
    SHR_IF_ERR_EXIT(portmod_port_remove(unit, port));
    /*
     * get number of eth ports on CDU before disabling anything common to all ports in CDU
     */
    SHR_IF_ERR_EXIT(imb_cdu_nof_ports_get(unit, port, &cdu_nof_eth_ports));
    /*
     * get number of ilkn ports on CDU before disabling PM or before reseting PM's lanes
     */
    SHR_IF_ERR_EXIT(imb_cdu_nof_ilkn_ports_get(unit, port, &cdu_nof_ilkn_ports));
    /*
     * get number of ports on the MAC
     */
    SHR_IF_ERR_EXIT(imb_cdu_mac_nof_ports_get(unit, port, &mac_nof_ports));
    /*
     * get number of channels
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));

    if (((cdu_nof_eth_ports + cdu_nof_ilkn_ports) == 1) && (nof_channels == 1))
    {
        /*
         * 12.  Port bond option
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_pm_reset(unit, port, IMB_CDU_IN_RESET));
    }

    
    if (nof_channels == 1)
    {
        /*
         * 4. Reset ports in NMG
         * 11. Reset ports in CDU
         * 2. Configure the SCH
         * 9. Set RX threshold after overflow
         * 8. Map physical port to RMC
         * 7. Map RMC to physical port
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_remove_rmc_lane_unmap(unit, port));
    }

    if ((mac_nof_ports == 1) && (nof_channels == 1))
    {
        /*
         * 6. Reset Async unit
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_async_unit_reset(unit, port, IMB_CDU_IN_RESET));
        /*
         * 5. Set CDU port mode
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_mac_enable_set(unit, port, 0, 0));
    }
    
    if (nof_channels == 1)
    {
        /*
         * 1.  Disable TX data to PM
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_tx_data_to_pm_enable_set(unit, port, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
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
imb_cdu_port_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int port_type;
    int loopback;
    int sif_instance_core, sif_instance_id;
    uint8 is_connected = FALSE;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, port, &port_type));

    if (enable)
    {
        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type))
        {
            /*
             * stop the egr flush
             */
            SHR_IF_ERR_EXIT(imb_cdu_port_tx_egress_flush_set(unit, port, 0));
        }

        /*
         * reset the port in CDU TX
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_tx_nmg_reset(unit, port, IMB_CDU_IN_RESET));
        SHR_IF_ERR_EXIT(imb_cdu_port_tx_reset(unit, port, IMB_CDU_IN_RESET));

        /*
         * take the port out of reset in CDU TX
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_tx_nmg_reset(unit, port, IMB_CDU_OUT_OF_RESET));
        SHR_IF_ERR_EXIT(imb_cdu_port_tx_reset(unit, port, IMB_CDU_OUT_OF_RESET));

        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type))
        {
            /*
             * take the port out of reset in CDU RX
             */
            SHR_IF_ERR_EXIT(imb_cdu_port_rx_nmg_reset(unit, port, IMB_CDU_ALL_RMCS, IMB_CDU_OUT_OF_RESET));
            SHR_IF_ERR_EXIT(imb_cdu_port_rx_reset(unit, port, IMB_CDU_ALL_RMCS, IMB_CDU_OUT_OF_RESET));

            /*
             * Wait for NIF to finish transmitting initial credits
             */
            sal_usleep(10);

            /*
             * override the egress credits
             */
            SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_default_set(unit, port));
        }

        /*
         * enable the port in Portmod
         */
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, enable));

        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type))
        {
            /*
             * Enable Qpairs
             */
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port, enable, FALSE));
        }
    }
    else
    {
        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type))
        {
            /*
             * Disable Qpairs
             */
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port, enable, FALSE));

            /*
             * reset the port in CDU (NMG)
             */
            SHR_IF_ERR_EXIT(imb_cdu_port_rx_nmg_reset(unit, port, IMB_CDU_ALL_RMCS, IMB_CDU_IN_RESET));
            /*
             * reset the port in CDU (RMC)
             */
            SHR_IF_ERR_EXIT(imb_cdu_port_rx_reset(unit, port, IMB_CDU_ALL_RMCS, IMB_CDU_IN_RESET));

            /*
             * flush the egress
             */
            SHR_IF_ERR_EXIT(imb_cdu_port_tx_egress_flush_set(unit, port, 1));
        }
        else
        {
            /*
             * reset the port in CDU TX
             */
            SHR_IF_ERR_EXIT(imb_cdu_port_tx_nmg_reset(unit, port, IMB_CDU_IN_RESET));
            SHR_IF_ERR_EXIT(imb_cdu_port_tx_reset(unit, port, IMB_CDU_IN_RESET));

            /*
             * Check if the port is already connected to SIF, if yes, the credits in the sif controller should be reset
             */
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_logical_port_is_connected
                            (unit, port, &sif_instance_core, &sif_instance_id, &is_connected));

            if (is_connected)
            {
                SHR_IF_ERR_EXIT(dnx_stif_mgmt_credit_reset(unit, sif_instance_core, sif_instance_id));
            }
        }

        /*
         * disable the port in portmod
         */
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, enable));
    }

    SHR_IF_ERR_EXIT(imb_port_loopback_get(unit, port, &loopback));
    rv = _bcm_linkscan_available(unit);
    if ((loopback == BCM_PORT_LOOPBACK_MAC) && (rv == BCM_E_NONE))
    {
        SHR_IF_ERR_EXIT(_bcm_link_force(unit, port, TRUE, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port enable indication
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * it is enough to get the indication from Portmod
     */

    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable port loopback
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] loopback - lopoback type
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_loopback_set(
    int unit,
    bcm_port_t port,
    int loopback)
{
    portmod_loopback_mode_t portmod_loopback;
    int curr_loopback, is_symmetric;
    int rv;
    int link, is_linkscan_available;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Since the lane map for PM8x50 has been moved to PMD, PHY/REMOTE
     * loopback cannot be configured when lane map is not symmetric.
     */
    if (loopback == BCM_PORT_LOOPBACK_PHY || loopback == BCM_PORT_LOOPBACK_PHY_REMOTE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_is_symmetric_get(unit, port, &is_symmetric));
        if (!is_symmetric)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Lane Map for port %d is not symmetric. Configuring PHY/REMOTE loopback on non-symmetric lane swapped port is forbidden, please adjust the lane map or use MAC loopback",
                         port);
        }
    }

    SHR_IF_ERR_EXIT(imb_port_loopback_get(unit, port, &curr_loopback));
    if (curr_loopback == loopback)
    {
        /*
         * Nothing to do
         */
        SHR_EXIT();
    }

    rv = _bcm_linkscan_available(unit);
    if (rv == BCM_E_NONE)
    {
        is_linkscan_available = TRUE;
    }
    else
    {
        is_linkscan_available = FALSE;
    }

    if (curr_loopback != BCM_PORT_LOOPBACK_NONE)
    {
        /*
         * Open current loopback
         */
        SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, curr_loopback, &portmod_loopback));
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmod_loopback, 0));
        if ((is_linkscan_available == TRUE) && (curr_loopback == BCM_PORT_LOOPBACK_MAC))
        {
            SHR_IF_ERR_EXIT(_bcm_link_force(unit, port, FALSE, FALSE));
        }

    }
    if (loopback != BCM_PORT_LOOPBACK_NONE)
    {
        /*
         * Close requested loopback
         */
        SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, loopback, &portmod_loopback));
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmod_loopback, 1));
        if ((is_linkscan_available == TRUE) && (loopback == BCM_PORT_LOOPBACK_MAC))
        {
            SHR_IF_ERR_EXIT(imb_port_enable_get(unit, port, &link));
            SHR_IF_ERR_EXIT(_bcm_link_force(unit, port, TRUE, link));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get loopback type on the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] loopback - loopback type
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
imb_cdu_port_loopback_get(
    int unit,
    bcm_port_t port,
    int *loopback)
{
    int rv = 0, lb_enabled = 0;
    portmod_loopback_mode_t portmod_loopback;
    SHR_FUNC_INIT_VARS(unit);

    *loopback = 0;
    /*
     * Iterate over all portmod loopback types
     */
    for (portmod_loopback = portmodLoopbackMacOuter; portmod_loopback != portmodLoopbackCount; ++portmod_loopback)
    {
        rv = portmod_port_loopback_get(unit, port, portmod_loopback, &lb_enabled);
        if (rv == _SHR_E_UNAVAIL)
        {
            /*
             * Portmod loopback type is not supported for PM type
             */
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        if (lb_enabled)
        {
            SHR_IF_ERR_EXIT(imb_bcm_loopback_from_portmod_loopback_get(unit, port, portmod_loopback, loopback));
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port link state
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] clear_status - indication to clear latch down
 *        status
 * \param [out] link_state - link state info
 *
 * \return
 *   int - see_SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_link_state_get(
    int unit,
    bcm_port_t port,
    int clear_status,
    bcm_port_link_state_t * link_state)
{
    int sw_latch_down, hw_latch_down;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * link up indication
     */
    SHR_IF_ERR_EXIT(portmod_port_link_get(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, &link_state->status));
    /*
     * get latch down from DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_get(unit, port, &sw_latch_down));
    /*
     * latch down indication
     */
    SHR_IF_ERR_EXIT(portmod_port_link_latch_down_get(unit,
                                                     port,
                                                     clear_status ? PORTMOD_PORT_LINK_LATCH_DOWN_F_CLEAR : 0,
                                                     &hw_latch_down));

    link_state->latch_down = sw_latch_down || hw_latch_down;

    if (clear_status)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_set(unit, port, 0));
    }
    else
    {
        /*
         * dont count on the register to keep the value. save in SW DB just in case
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_set(unit, port, link_state->latch_down));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/ disable eee for the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_eee_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    portmod_eee_t eee;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * enable EEE in Portmod
     */
    SHR_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    eee.enable = enable;
    SHR_IF_ERR_EXIT(portmod_port_eee_set(unit, port, &eee));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get EEE enable indication for the port_update_control
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_eee_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    portmod_eee_t eee;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * we can take the indication directly from Pormod
     */
    SHR_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    *enable = eee.enable;
exit:SHR_FUNC_EXIT;

}

/**
 * \brief - Set the priority group configuratoin into SW db.
 * priority group translate to a RMC (logical fifo)
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - lofical id
 * \param [in] priority_config - priority group specification
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * bcm_port_prio_config_t
 */
static int
imb_cdu_port_priority_config_db_update(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int ii, nof_prio_groups, prev_last_entry, rmc_fifo_size;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    nof_prio_groups = priority_config->nof_priority_groups;

    prev_last_entry = -1;
    for (ii = 0; ii < nof_prio_groups; ++ii)
    {
        /*
         * copy logical fifo priorities from user configuration
         */
        rmc.sch_priority = priority_config->priority_groups[ii].sch_priority;
        rmc.prd_priority = priority_config->priority_groups[ii].source_priority;
        /*
         * allocate RMC id
         */
        SHR_IF_ERR_EXIT(dnx_port_algo_imb_cdu_port_rmc_id_get(unit, port, rmc.sch_priority, &rmc.rmc_id));
        /*
         * get first end last entry of the RMC
         */
        SHR_IF_ERR_EXIT(dnx_port_algo_imb_rmc_entry_range_get
                        (unit, port, priority_config->priority_groups[ii].num_of_entries, prev_last_entry,
                         nof_prio_groups, &rmc.first_entry, &rmc.last_entry));
        prev_last_entry = rmc.last_entry;

        /*
         * get threshold after ovf
         */
        rmc_fifo_size = rmc.last_entry - rmc.first_entry + 1;
        SHR_IF_ERR_EXIT(dnx_algo_port_imb_threshold_after_ovf_get(unit, port, rmc_fifo_size, &rmc.thr_after_ovf));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_set(unit, port, 0, ii, &rmc));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - polling to make sure RMC FIFO is empty before
 *        reseting it.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_id - RMC id to poll for empty
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
imb_cdu_rmc_empty_wait(
    int unit,
    bcm_port_t port,
    uint32 rmc_id)
{
    uint32 fifo_level;
    soc_timeout_t to;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_rmc_level_get(unit, port, rmc_id, &fifo_level));

    soc_timeout_init(&to, 10000 /* 10 millisec */ , 10 /* min polls */ );
    while (fifo_level)
    {
        if (soc_timeout_check(&to))
        {
            /*
             * Note that even after timeout expires, the status should be checked
             * one more time.  Otherwise there is a race condition where an
             * ill-placed O/S task reschedule could cause a false timeout.
             */
            SHR_IF_ERR_EXIT(imb_cdu_rmc_level_get(unit, port, rmc_id, &fifo_level));
            if (fifo_level)
            {
                SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Timeout while waiting for port %d FIFO %d to clear", port, rmc_id);
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
imb_cdu_port_priority_config_enable_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc_arr,
    int nof_rmcs,
    int enable)
{
    int ii;
    int port_has_speed;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * before we start the sequence, we need to make sure all the RMC FIFOs are empty, and reset them
     */
    for (ii = 0; ii < nof_rmcs; ++ii)
    {
        if (rmc_arr[ii].rmc_id == IMB_CDU_RMC_INVALID)
        {
            continue;
        }
        /*
         *  wait for the FIFO to clear before reseting it
         */
        SHR_IF_ERR_EXIT(imb_cdu_rmc_empty_wait(unit, port, rmc_arr[ii].rmc_id));
        /*
         * Reset the RMCs in the CDU and NMG
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_rx_nmg_reset(unit, port, rmc_arr[ii].rmc_id, IMB_CDU_IN_RESET));
        SHR_IF_ERR_EXIT(imb_cdu_port_rx_reset(unit, port, rmc_arr[ii].rmc_id, IMB_CDU_IN_RESET));
    }

    /*
     * configure scheduler
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, &port_has_speed));
    if (port_has_speed)
    {
        SHR_IF_ERR_EXIT(imb_cdu_port_scheduler_config_set(unit, port, enable));
    }

    /*
     * change the priority configuration
     */
    for (ii = 0; ii < nof_rmcs; ++ii)
    {
        if (rmc_arr[ii].rmc_id == IMB_CDU_RMC_INVALID)
        {
            continue;
        }

        /*
         * Map RMC to physical port
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_rmc_to_lane_map(unit, port, &rmc_arr[ii], enable));
        /*
         * Map physical port to RMC
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_lane_to_rmc_map(unit, port, &rmc_arr[ii], enable));
        /*
         * Set RX threshold after overflow
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_thr_after_ovf_set(unit, port, &rmc_arr[ii]));

    }
    if (enable)
    {
        /*
         * take the RMCs out of reset
         */
        for (ii = 0; ii < nof_rmcs; ++ii)
        {
            if (rmc_arr[ii].rmc_id == IMB_CDU_RMC_INVALID)
            {
                continue;
            }
            /*
             * Take RMCs out of reset in the CDU and NMG
             */
            SHR_IF_ERR_EXIT(imb_cdu_port_rx_nmg_reset(unit, port, rmc_arr[ii].rmc_id, IMB_CDU_OUT_OF_RESET));
            SHR_IF_ERR_EXIT(imb_cdu_port_rx_reset(unit, port, rmc_arr[ii].rmc_id, IMB_CDU_OUT_OF_RESET));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static int
imb_cdu_port_priority_config_verify(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int nof_prio_groups_supported, ii, nof_entries, prd_priority_mask;
    int prio_found[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF] = { 0 };
    bcm_port_nif_scheduler_t sch_prio;
    SHR_FUNC_INIT_VARS(unit);

    nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);

    if (priority_config->nof_priority_groups > nof_prio_groups_supported)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of priority groups %d is not supported\n",
                     priority_config->nof_priority_groups);
    }
    for (ii = 0; ii < priority_config->nof_priority_groups; ++ii)
    {
        nof_entries = priority_config->priority_groups[ii].num_of_entries;
        sch_prio = priority_config->priority_groups[ii].sch_priority;
        prd_priority_mask = priority_config->priority_groups[ii].source_priority;

        /*
         * Supported values for num_of_entries is -1 or > dnx_data_nif.eth.priority_group_nof_entries_min
         */
        if (nof_entries < dnx_data_nif.eth.priority_group_nof_entries_min_get(unit) && nof_entries != -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Number of entries %d is invalid\n", nof_entries);
        }
        if (prio_found[sch_prio])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Having two priority groups with the same scheduler priority is not supported\n");
        }
        prio_found[sch_prio] = 1;
        if (prd_priority_mask == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Priority group %d is not mapped to any source priority\n", ii);
        }
        else if (prd_priority_mask & (~IMB_PRD_PRIORITY_ALL))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Priority group %d source priority is invalid\n", ii);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the given priority groups configration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority_config - priority groups configuration
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
imb_cdu_port_priority_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int ii;
    dnx_algo_port_rmc_info_t rmc_arr[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify data is correct
     */
    SHR_IF_ERR_EXIT(imb_cdu_port_priority_config_verify(unit, port, priority_config));

    /*
     * update new configuration on SW DB
     */
    SHR_IF_ERR_EXIT(imb_cdu_port_priority_config_db_update(unit, port, priority_config));

    /*
     * read from DB in order to call configuration function with RMC array
     */
    for (ii = 0; ii < priority_config->nof_priority_groups; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc_arr[ii]));
    }
    SHR_IF_ERR_EXIT(imb_cdu_port_priority_config_enable_set
                    (unit, port, rmc_arr, priority_config->nof_priority_groups, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the given priority groups configration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] priority_config - priority groups configuration
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
imb_cdu_port_priority_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config)
{
    int ii, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);

    priority_config->nof_priority_groups = 0;

    for (ii = 0; ii < nof_priority_groups; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc));
        if (rmc.rmc_id == IMB_CDU_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        priority_config->priority_groups[ii].sch_priority = rmc.sch_priority;
        priority_config->priority_groups[ii].source_priority = rmc.prd_priority;
        priority_config->priority_groups[ii].num_of_entries = rmc.last_entry - rmc.first_entry + 1;
        ++priority_config->nof_priority_groups;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get CDU current bandwidth
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_port_include - indication wether to include /
 *        exclude specified port in the BW calculation
 * \param [out] bandwidth - BW calculated
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
imb_cdu_bandwidth_get(
    int unit,
    bcm_port_t port,
    int is_port_include,
    int *bandwidth)
{
    bcm_pbmp_t cdu_ports;
    bcm_port_t port_i;
    int bw_sum, port_speed;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_master_ports_get(unit, port, &cdu_ports));

    bw_sum = 0;
    if (!is_port_include)
    {
        /*
         * don't include the current port in the calculation (means port is in process of being removed)
         */
        BCM_PBMP_PORT_REMOVE(cdu_ports, port);
    }
    BCM_PBMP_ITER(cdu_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port_i, 0, &port_speed));
        bw_sum += port_speed;
    }
    *bandwidth = bw_sum;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the TX start threshold for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed - port interface rate
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_tx_start_thr_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    int start_thr = -1;
    int idx;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get global TX threshold definition
     */
    start_thr = dnx_data_nif.global.start_tx_threshold_global_get(
    unit);
    if (start_thr < 0)
    {
        /*
         * Global TX threshold not defined.
         * Lookup TX threshold in table.
         */
        for (idx = 0; idx < dnx_data_nif.global.start_tx_threshold_table_info_get(unit)->key_size[0]; idx++)
        {
            if (speed == dnx_data_nif.global.start_tx_threshold_table_get(unit, idx)->speed)
            {
                start_thr = dnx_data_nif.global.start_tx_threshold_table_get(
    unit,
    idx)->start_thr;
                break;
            }
        }

        if (start_thr < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported speed %d (port %d)\n", speed, port);
        }
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(imb_cdu_internal_tx_start_thr_set
                    (unit, cdu_info.core, cdu_info.inner_cdu_id, cdu_info.first_lane_in_cdu, start_thr));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable PRD Hard stage per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
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
imb_cdu_prd_hard_stage_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_CDU_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_hard_stage_enable_set(unit, port, rmc.rmc_id, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication per port for the PRD hard
 *        stage
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_cdu_prd_hard_stage_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_CDU_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_hard_stage_enable_get(unit, port, rmc.rmc_id, enable));
        break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get PRD port type from port header type. in addition
 *        get TM/Eth port indication, depending on the header
 *        type of the port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] prd_port_type - PRD port type. see
 *        NIF_PRD_PORT_TYPE in DBAL
 * \param [in] is_eth_port - is eth port type
 * \param [in] is_tm_port - is TM port type
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
imb_cdu_prd_port_type_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_port_type,
    uint32 *is_eth_port,
    uint32 *is_tm_port)
{
    int header_type;
    SHR_FUNC_INIT_VARS(unit);

    *is_eth_port = 0;
    *is_tm_port = 0;

    SHR_IF_ERR_EXIT(dnx_switch_header_type_get(unit, port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN, &header_type));

    switch (header_type)
    {
        case BCM_SWITCH_PORT_HEADER_TYPE_ETH:
            *prd_port_type = DBAL_ENUM_FVAL_NIF_PRD_PORT_TYPE_ETH;
            *is_eth_port = 1;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_TM:
            *prd_port_type = DBAL_ENUM_FVAL_NIF_PRD_PORT_TYPE_ITMH;
            *is_tm_port = 1;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_STACKING:
            *prd_port_type = DBAL_ENUM_FVAL_NIF_PRD_PORT_TYPE_FTMH;
            *is_tm_port = 1;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP:
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2:
            *prd_port_type = DBAL_ENUM_FVAL_NIF_PRD_PORT_TYPE_PTCH_2;
            *is_eth_port = 1;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported header type %d\n", header_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get outer tag size for the port. depending if the
 *        port is defined as port extender. if the port is port
 *        extender, the outer tag size is 8B, otherwise 4B
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] outer_tag_size - outer tag size. see
 *        VLAN_OUTER_TAG_SIZE in DBAL.
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
imb_cdu_prd_outer_tag_size_get(
    int unit,
    bcm_port_t port,
    uint32 *outer_tag_size)
{
    int port_ext_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_port_control_get(unit, port, bcmPortControlExtenderType, &port_ext_type));

    switch (port_ext_type)
    {
        case BCM_PORT_EXTENDER_TYPE_SWITCH:
            *outer_tag_size = DBAL_ENUM_FVAL_VLAN_OUTER_TAG_SIZE_8B;
            break;
        case BCM_PORT_EXTENDER_TYPE_NONE:
            *outer_tag_size = DBAL_ENUM_FVAL_VLAN_OUTER_TAG_SIZE_4B;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported extender type %d\n", port_ext_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get outer ITMH header TC (Traffic Class) and DP (Drop Precedence) field offsets
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] tc_offset - TC (Traffic Class) field offset
 * \param [out] dp_offset - DP (Drop Precedence) field offset
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
imb_cdu_prd_itmh_offsets_get(
    int unit,
    bcm_port_t port,
    int *tc_offset,
    int *dp_offset)
{
    int size, offset;
    int itmh_size, itmh_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "ITMH_Base", &itmh_offset, &itmh_size));

    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "ITMH_Base.TC", &offset, &size));
    *tc_offset = itmh_size - offset - size;
    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "ITMH_Base.DP", &offset, &size));
    *dp_offset = itmh_size - offset - size;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get outer FTMH header TC (Traffic Class) and DP (Drop Precedence) field offsets
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] tc_offset - TC (Traffic Class) field offset
 * \param [out] dp_offset - DP (Drop Precedence) field offset
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
imb_cdu_prd_ftmh_offsets_get(
    int unit,
    bcm_port_t port,
    int *tc_offset,
    int *dp_offset)
{
    int size, offset;
    int ftmh_size, ftmh_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "FTMH_Base", &ftmh_offset, &ftmh_size));

    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "FTMH_Base.FTMH_PP_TC", &offset, &size));
    *tc_offset = ftmh_size - offset - size;
    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "FTMH_Base.FTMH_DP", &offset, &size));
    *dp_offset = ftmh_size - offset - size;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set all fixed ether types sizes. (in bytes). this
 *        function is called only once per port, when the PRD is
 *        enabled (fixed ether types should always have the
 *        same size). the configurable ether types sizes are set
 *        by the user, see API
 *        imb_cdu_prd_flex_key_construct_set
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
static int
imb_cdu_prd_ether_type_size_set(
    int unit,
    bcm_port_t port)
{
    const dnxc_data_table_info_t *ether_type_table_info;
    uint32 ether_type_code, nof_ether_types, ether_type_size;
    SHR_FUNC_INIT_VARS(unit);

    ether_type_table_info = dnx_data_nif.prd.ether_type_info_get(unit);
    nof_ether_types = ether_type_table_info->key_size[0];
    /*
     * Set Ether type size for the fixed ether types. the configurable ether types sizes should be defined by the user
     * in API imb_cdu_prd_flex_key_construct_set in using offset base of EndOfHeaderAfterEthHeader
     */
    for (ether_type_code = 0; ether_type_code < nof_ether_types; ++ether_type_code)
    {
        ether_type_size = dnx_data_nif.prd.ether_type_get(unit, ether_type_code)->ether_type_size;
        if (ether_type_size > 0)
        {
            SHR_IF_ERR_EXIT(imb_cdu_internal_prd_ether_type_size_set(unit, port, ether_type_code, ether_type_size));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable PRD for the port. this call should be
 *        last, after the PRD is already configured.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - enable modes for the PRD:
 * bcmCosqIngressPortDropDisable
 * bcmCosqIngressPortDropEnableHardStage
 * bcmCosqIngressPortDropEnableHardAndSoftStage
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * it is required to call header type set before enabling
 *     the PRD.
 * \see
 *   * None
 */
int
imb_cdu_prd_enable_set(
    int unit,
    bcm_port_t port,
    int enable_mode)
{
    uint32 en_soft_stage_eth, en_soft_stage_tm;
    uint32 is_eth_port = 0, is_tm_port = 0;
    uint32 port_type = 0;
    uint32 outer_tag_size = 0;
    int tc_offset = 0;
    int dp_offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (enable_mode == bcmCosqIngressPortDropDisable)
    {
        /*
         * Disable Hard stage
         */
        SHR_IF_ERR_EXIT(imb_cdu_prd_hard_stage_enable_set(unit, port, 0));

        /*
         * Disable Soft Stage
         */
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_soft_stage_enable_set(unit, port, 0, 0));
    }
    else
    {
        /*
         * set TC/DP offsets for ITMH/FTMH
         */
        SHR_IF_ERR_EXIT(imb_cdu_prd_itmh_offsets_get(unit, port, &tc_offset, &dp_offset));
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_itmh_offsets_set(unit, port, tc_offset, dp_offset));

        SHR_IF_ERR_EXIT(imb_cdu_prd_ftmh_offsets_get(unit, port, &tc_offset, &dp_offset));
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_ftmh_offsets_set(unit, port, tc_offset, dp_offset));

        /*
         * Get is port extender
         */
        SHR_IF_ERR_EXIT(imb_cdu_prd_outer_tag_size_get(unit, port, &outer_tag_size));
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_outer_tag_size_set(unit, port, outer_tag_size));

        /*
         * Set Ether type sizes for fixed Ether types
         */
        SHR_IF_ERR_EXIT(imb_cdu_prd_ether_type_size_set(unit, port));

        /*
         * Set prd port type
         */
        SHR_IF_ERR_EXIT(imb_cdu_prd_port_type_get(unit, port, &port_type, &is_eth_port, &is_tm_port));
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_port_type_set(unit, port, port_type));

        /*
         * Enable soft stage
         */
        en_soft_stage_eth = ((enable_mode == bcmCosqIngressPortDropEnableHardAndSoftStage) && (is_eth_port)) ? 1 : 0;
        en_soft_stage_tm = ((enable_mode == bcmCosqIngressPortDropEnableHardAndSoftStage) && (is_tm_port)) ? 1 : 0;
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_soft_stage_enable_set(unit, port, en_soft_stage_eth, en_soft_stage_tm));

        /*
         * Enable hard stage
         */
        SHR_IF_ERR_EXIT(imb_cdu_prd_hard_stage_enable_set(unit, port, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get PRD enable mode per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - PRD enable modes:
 *  bcmCosqIngressPortDropDisable
 *  bcmCosqIngressPortDropEnableHardStage
 *  bcmCosqIngressPortDropEnableHardAndSoftStage
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
imb_cdu_prd_enable_get(
    int unit,
    bcm_port_t port,
    int *enable_mode)
{
    uint32 is_eth_port = 0, is_tm_port = 0, port_type, en_hard_stage, en_soft_stage_eth, en_soft_stage_tm;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_prd_hard_stage_enable_get(unit, port, &en_hard_stage));

    if (en_hard_stage)
    {
        /*
         * The expectation is that eth soft stage will only be set for eth ports, and tm soft stage will only be set
         * for tm ports. but we check for the port type just in case
         */
        SHR_IF_ERR_EXIT(imb_cdu_prd_port_type_get(unit, port, &port_type, &is_eth_port, &is_tm_port));

        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_soft_stage_enable_get(unit, port, &en_soft_stage_eth, &en_soft_stage_tm));

        if ((en_soft_stage_eth && is_eth_port) || (en_soft_stage_tm && is_tm_port))
        {
            *enable_mode = bcmCosqIngressPortDropEnableHardAndSoftStage;
        }
        else
        {
            *enable_mode = bcmCosqIngressPortDropEnableHardStage;
        }
    }
    else
    {
        *enable_mode = bcmCosqIngressPortDropDisable;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set different PRD properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to set:
 *  see enum imb_prd_properties_t.
 * \param [in] val - value of the property to set.
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
imb_cdu_prd_properties_set(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (property)
    {
        case imbImbPrdIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_cdu_prd_ignore_ip_dscp_set(unit, port, val));
            break;
        case imbImbPrdIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_cdu_prd_ignore_mpls_exp_set(unit, port, val));
            break;
        case imbImbPrdIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_cdu_prd_ignore_inner_tag_set(unit, port, val));
            break;
        case imbImbPrdIgnoreOuterTag:
            SHR_IF_ERR_EXIT(imb_cdu_prd_ignore_outer_tag_set(unit, port, val));
            break;
        case imbImbPrdDefaultPriority:
            SHR_IF_ERR_EXIT(imb_cdu_prd_default_priority_set(unit, port, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD property %d\n", property);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get PRD property value
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to get.
 * see enum imb_prd_properties_t.
 * \param [out] val - returned property value
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
imb_cdu_prd_properties_get(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (property)
    {
        case imbImbPrdIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_cdu_prd_ignore_ip_dscp_get(unit, port, val));
            break;
        case imbImbPrdIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_cdu_prd_ignore_mpls_exp_get(unit, port, val));
            break;
        case imbImbPrdIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_cdu_prd_ignore_inner_tag_get(unit, port, val));
            break;
        case imbImbPrdIgnoreOuterTag:
            SHR_IF_ERR_EXIT(imb_cdu_prd_ignore_outer_tag_get(unit, port, val));
            break;
        case imbImbPrdDefaultPriority:
            SHR_IF_ERR_EXIT(imb_cdu_prd_default_priority_get(unit, port, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD property %d\n", property);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set PRD hard stage priority map.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - map type:
 *  see bcm_cosq_ingress_port_drop_map_t.
 * \param [in] key - key index for the map (dependes on the map
 *        type)
 * \param [in] priority - priority value to set to the key in
 *        the map.
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
imb_cdu_prd_map_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 priority)
{
    uint32 dp, tc, dscp, pcp, dei, exp;
    SHR_FUNC_INIT_VARS(unit);

    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            IMB_CDU_PRD_TM_KEY_TC_GET(key, tc);
            IMB_CDU_PRD_TM_KEY_DP_GET(key, dp);
            SHR_IF_ERR_EXIT(imb_cdu_internal_prd_map_tm_tc_dp_set(unit, port, tc, dp, priority));
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            IMB_CDU_PRD_IP_KEY_DSCP_GET(key, dscp);
            SHR_IF_ERR_EXIT(imb_cdu_internal_prd_map_ip_dscp_set(unit, port, dscp, priority));
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            IMB_CDU_PRD_ETH_KEY_PCP_GET(key, pcp);
            IMB_CDU_PRD_ETH_KEY_DEI_GET(key, dei);
            SHR_IF_ERR_EXIT(imb_cdu_internal_prd_map_eth_pcp_dei_set(unit, port, pcp, dei, priority));
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            IMB_CDU_PRD_MPLS_KEY_EXP_GET(key, exp);
            SHR_IF_ERR_EXIT(imb_cdu_internal_prd_map_mpls_exp_set(unit, port, exp, priority));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD Map %d\n", map);
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get priority value mapped to a key in one of the PRD
 *        prioroity tables
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - type of map. see
 *        bcm_cosq_ingress_port_drop_map_t
 * \param [in] key - key index for the map (dependes on the map
 *        type)
 * \param [out] priority - returned priority vlaue
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_prd_map_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 *priority)
{
    uint32 dp, tc, dscp, pcp, dei, exp;
    SHR_FUNC_INIT_VARS(unit);

    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            IMB_CDU_PRD_TM_KEY_TC_GET(key, tc);
            IMB_CDU_PRD_TM_KEY_DP_GET(key, dp);
            SHR_IF_ERR_EXIT(imb_cdu_internal_prd_map_tm_tc_dp_get(unit, port, tc, dp, priority));
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            IMB_CDU_PRD_IP_KEY_DSCP_GET(key, dscp);
            SHR_IF_ERR_EXIT(imb_cdu_internal_prd_map_ip_dscp_get(unit, port, dscp, priority));
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            IMB_CDU_PRD_ETH_KEY_PCP_GET(key, pcp);
            IMB_CDU_PRD_ETH_KEY_DEI_GET(key, dei);
            SHR_IF_ERR_EXIT(imb_cdu_internal_prd_map_eth_pcp_dei_get(unit, port, pcp, dei, priority));
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            IMB_CDU_PRD_MPLS_KEY_EXP_GET(key, exp);
            SHR_IF_ERR_EXIT(imb_cdu_internal_prd_map_mpls_exp_get(unit, port, exp, priority));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD Map %d\n", map);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get the correct RMC of the port to which the PRD
 *        prioroity is mapped
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority to look for
 * \param [in] rmc - returned RMC for the Priority
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
imb_cdu_prd_threshold_rmc_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    dnx_algo_port_rmc_info_t * rmc)
{
    uint32 priority_flag = 0, max_rmcs_per_port, rmc_found = 0;
    int rmc_index;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    switch (priority)
    {
        case 0:
            priority_flag = BCM_PORT_F_PRIORITY_0;
            break;
        case 1:
            priority_flag = BCM_PORT_F_PRIORITY_1;
            break;
        case 2:
            priority_flag = BCM_PORT_F_PRIORITY_2;
            break;
        case 3:
            priority_flag = BCM_PORT_F_PRIORITY_3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Priority %d\n", priority);
    }

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, rmc));
        if ((rmc->rmc_id != IMB_CDU_RMC_INVALID) && (rmc->prd_priority & priority_flag))
        {
            rmc_found = 1;
            break;
        }
    }

    if (!rmc_found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Priority %d is not mapped to any of the port's priority groups\n", priority);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD threshold value per priority per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority for the threshold
 * \param [in] threshold - threshold value
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
imb_cdu_prd_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 threshold)
{
    dnx_algo_port_rmc_info_t rmc;
    uint32 rmc_fifo_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_prd_threshold_rmc_get(unit, port, priority, &rmc));

    rmc_fifo_size = rmc.last_entry - rmc.first_entry + 1;

    if (threshold > rmc_fifo_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Threshold %d for priority %d of port %d is higher than the fifo size\n", threshold,
                     priority, port);
    }
    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_threshold_set(unit, port, rmc.rmc_id, priority, threshold));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get Threshold value for the given priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority to get threshold
 * \param [in] threshold - returned threshold
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
imb_cdu_prd_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *threshold)
{
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_prd_threshold_rmc_get(unit, port, priority, &rmc));

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_threshold_get(unit, port, rmc.rmc_id, priority, threshold));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set TPID value for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
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
imb_cdu_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_tpid_set(unit, port, tpid_index, tpid_value));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get TPID value for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index to get
 * \param [in] tpid_value - returned TPID value
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
imb_cdu_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_tpid_get(unit, port, tpid_index, tpid_value));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get PRD drop counter per sch_priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_priority - scheduler prioirity. the PRD
 *        counter is per RMC, so the user of this function gives
 *        the scheduler priority in order to find which RMC to
 *        read the counter from
 * \param [out] count - counter value
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
imb_cdu_prd_drop_count_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    uint64 *count)
{
    dnx_algo_port_rmc_info_t rmc;
    uint32 max_rmcs_per_port, rmc_found = 0;
    int rmc_index;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if ((rmc.rmc_id != IMB_CDU_RMC_INVALID) && (rmc.sch_priority == sch_priority))
        {
            SHR_IF_ERR_EXIT(imb_cdu_internal_prd_drop_count_get(unit, port, rmc.rmc_id, count));
            rmc_found = 1;
            break;
        }
    }

    if (!rmc_found)
    {
        COMPILER_64_SET(*count, 0, 0);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set configurable ether type to be recognized by the
 *        PRD Parser
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to recognize
 *        the new ether type (1-6)
 * \param [in] ether_type_val - ether type value
 *
 * \return
 *   int  -see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_prd_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 ether_type_code_max;
    SHR_FUNC_INIT_VARS(unit);

    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_custom_ether_type_set(unit, port, ether_type_code - 1, ether_type_val));

    if (ether_type_code == ether_type_code_max)
    {
        /*
         * The last configurable ether type (with ether type code 7) is saved for TM port types. the way to save it is
         * to always configure ether ype code 6 and ether type code 7 to the same value. so if the user want to
         * configure ether type code 6, we copy the same value to ether type code 7
         */
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_custom_ether_type_set(unit, port, ether_type_code, ether_type_val));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get ether type mapped to a specific configurable
 *        ether type  code
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - configurable ether type code to
 *        get (1-6)
 * \param [in] ether_type_val - eter type value mapped to the
 *        given ether type code.
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
imb_cdu_prd_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_custom_ether_type_get(unit, port, ether_type_code - 1, ether_type_val));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set an entry to the TCAM table in the PRD soft stage
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] key_index - index in the TCAM
 * \param [in] flex_key_info - info of the TCAM entry
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
imb_cdu_prd_flex_key_entry_set(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    const bcm_cosq_ingress_drop_flex_key_entry_t * flex_key_info)
{
    imb_cdu_internal_prd_tcam_entry_info_t entry_info;
    int offset_idx;
    SHR_FUNC_INIT_VARS(unit);

    entry_info.ether_code_val = flex_key_info->ether_code.value;
    entry_info.ether_code_mask = flex_key_info->ether_code.mask;
    entry_info.priority = flex_key_info->priority;
    entry_info.is_entry_valid = 1;
    entry_info.offset_array_val = 0;
    entry_info.offset_array_mask = 0;

    for (offset_idx = 0; offset_idx < flex_key_info->num_key_fields; ++offset_idx)
    {
        IMB_CDU_PRD_TCAM_ENTRY_OFFSET_SET(flex_key_info->key_fields[offset_idx].value, offset_idx,
                                          entry_info.offset_array_val);
        IMB_CDU_PRD_TCAM_ENTRY_OFFSET_SET(flex_key_info->key_fields[offset_idx].mask, offset_idx,
                                          entry_info.offset_array_mask);
    }
    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_tcam_entry_set(unit, port, key_index, &entry_info));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get an entry from the PRD soft stage TCAM table
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] key_index - index in the TCAM table
 * \param [in] flex_key_info - returned entry info.
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
imb_cdu_prd_flex_key_entry_get(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    bcm_cosq_ingress_drop_flex_key_entry_t * flex_key_info)
{
    imb_cdu_internal_prd_tcam_entry_info_t entry_info;
    int offset_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_tcam_entry_get(unit, port, key_index, &entry_info));

    flex_key_info->ether_code.value = entry_info.ether_code_val;
    flex_key_info->ether_code.mask = entry_info.ether_code_mask;
    flex_key_info->priority = entry_info.priority;

    for (offset_idx = 0; offset_idx < BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS; ++offset_idx)
    {
        /*
         * zero the values first - just in case
         */
        flex_key_info->key_fields[offset_idx].value = 0;
        flex_key_info->key_fields[offset_idx].mask = 0;

        IMB_CDU_PRD_TCAM_ENTRY_OFFSET_GET(entry_info.offset_array_val, offset_idx,
                                          flex_key_info->key_fields[offset_idx].value);
        IMB_CDU_PRD_TCAM_ENTRY_OFFSET_GET(entry_info.offset_array_mask, offset_idx,
                                          flex_key_info->key_fields[offset_idx].mask);
    }
    flex_key_info->num_key_fields = BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS;
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - build the key to the TCAM table. the key is composed
 *        of 36 bit:
 *  0-31: 4 offsets from the packet header given by the user.
 *        from each offset 8 bits are taken. the offsets start
 *        from a configurable offset base:
 *        - start of oacket
 *        - end of eth header
 *        - end of header after eth header
 *  32-35: ether type code. a different key can be
 *        composed per each ether type code.
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to build the
 *        key.
 * \param [in] flex_key_config - key info to config (array of 4
 *        offsets and offsets base).
 *        in case offset base is "end of header after eth
 *        header" and the ether type code is of configurable
 *        type (1-6) it is also required to provide ether type
 *        size (in bytes)
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
imb_cdu_prd_flex_key_construct_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    const bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{
    uint32 ether_type_code_min, ether_type_code_max;
    int offset_index, is_configurable_ether_type;
    SHR_FUNC_INIT_VARS(unit);

    for (offset_index = 0; offset_index < flex_key_config->array_size; ++offset_index)
    {
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_tcam_entry_key_offset_set
                        (unit, port, ether_type_code, offset_index, flex_key_config->offset_array[offset_index]));
    }

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_tcam_entry_key_offset_base_set
                    (unit, port, ether_type_code, flex_key_config->offset_base));

    ether_type_code_min = dnx_data_nif.prd.custom_ether_type_code_min_get(unit);
    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    is_configurable_ether_type = (ether_type_code >= ether_type_code_min) && (ether_type_code <= ether_type_code_max);

    /*
     * only set the ether type size for configurable ether types with offset base of "end of header after eth"
     */
    if (is_configurable_ether_type
        && flex_key_config->offset_base == bcmCosqIngressPortDropFlexKeyBaseEndOfHeaderAfterEthHeader)
    {
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_ether_type_size_set
                        (unit, port, ether_type_code, flex_key_config->ether_type_header_size));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get key constructed per ether type code
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code of the key to
 *        retrieve
 * \param [in] flex_key_config - returned key info
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
imb_cdu_prd_flex_key_construct_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{
    uint32 offset_index, is_mpls;
    SHR_FUNC_INIT_VARS(unit);

    for (offset_index = 0; offset_index < BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS; ++offset_index)
    {
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_tcam_entry_key_offset_get
                        (unit, port, ether_type_code, offset_index, &flex_key_config->offset_array[offset_index]));
    }
    flex_key_config->array_size = BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS;

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_tcam_entry_key_offset_base_get
                    (unit, port, ether_type_code, &flex_key_config->offset_base));

    /*
     * MPLS protocol has no size in HW register, since it is automatically calculated,
     * so before accessing the DBAL, we need to make sure we dont have MPLS
     */
    is_mpls = sal_strcmp(dnx_data_nif.prd.ether_type_get(unit, ether_type_code)->ether_type_name, "MPLS") ? 0 : 1;
    if (!is_mpls)
    {
        SHR_IF_ERR_EXIT(imb_cdu_internal_prd_ether_type_size_get
                        (unit, port, ether_type_code, &flex_key_config->ether_type_header_size));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set control frame properties to be recognized by the
 *        PRD. if a control packet is recognized (each packet is
 *        compared against all control planes defined for the
 *        CDU), it gets the highest priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        configure (0-3)
 * \param [in] control_frame_config - control frame properties
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
imb_cdu_prd_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_control_frame_set(unit, port, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get Control frame properties recognized by the PRD
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        get
 * \param [out] control_frame_config - returned control frame
 *        properties
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
imb_cdu_prd_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_control_frame_get(unit, port, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set MPLS special label properties. the special
 *        labels are compared globaly against all the MPLS ports
 *        of the same CDU.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] label_index - label index to set (0-3)
 * \param [in] label_config - special label configuration.
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
imb_cdu_prd_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_mpls_special_label_set(unit, port, label_index, label_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get MPLS special label properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] label_index - label index to get (0-3)
 * \param [out] label_config - label properties
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_prd_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_prd_mpls_special_label_get(unit, port, label_index, label_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - return true if the ether type code is configurable
 *        to the user. the PRD parser recognizes several ether
 *        types - some are fixed values, and some are open for
 *        user prefernce.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to check
 * \param [in] is_custom_ether_type - returned indication
 *        whether the ether type is configurable
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_prd_is_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *is_custom_ether_type)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_custom_ether_type = (ether_type_code >= dnx_data_nif.prd.custom_ether_type_code_min_get(unit) &&
                             ether_type_code <= dnx_data_nif.prd.custom_ether_type_code_max_get(unit));

    SHR_FUNC_EXIT;

}

/**
 * \brief - Set local fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable value (1 for enabled)
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
imb_cdu_port_local_fault_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    portmod_local_fault_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_local_fault_control_get(unit, port, &control));
    control.enable = enable;

    SHR_IF_ERR_EXIT(portmod_port_local_fault_control_set(unit, port, &control));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get local fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable value (1 for enabled)
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
imb_cdu_port_local_fault_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    portmod_local_fault_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_local_fault_control_get(unit, port, &control));
    *enable = control.enable;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Set remote fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable value (1 for enabled)
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
imb_cdu_port_remote_fault_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    portmod_remote_fault_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_remote_fault_control_get(unit, port, &control));
    control.enable = enable;

    SHR_IF_ERR_EXIT(portmod_port_remote_fault_control_set(unit, port, &control));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get remote fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable value (1 for enabled)
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
imb_cdu_port_remote_fault_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    portmod_remote_fault_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_remote_fault_control_get(unit, port, &control));
    *enable = control.enable;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get NIF RX fifo status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_priority - nif schedule priority
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
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
imb_cdu_port_rx_fifo_status_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    uint32 *max_occupancy,
    uint32 *fifo_level)
{

    dnx_algo_port_rmc_info_t rmc_info;
    uint32 max_rmcs_per_port;
    int rmc_index, rmc_found = 0;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc_info));
        if ((rmc_info.rmc_id != IMB_CDU_RMC_INVALID) && (rmc_info.sch_priority == sch_priority))
        {
            SHR_IF_ERR_EXIT(imb_cdu_internal_port_rx_fifo_status_get
                            (unit, port, rmc_info.rmc_id, max_occupancy, fifo_level));
            rmc_found = 1;
            break;
        }
    }
    if (!rmc_found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Sch Priority %d is not mapped to any of the port's priority group\n", sch_priority);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get NIF TX fifo status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
 * \param [out] pm_credits - indicate the credits from PM tx buffer
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
imb_cdu_port_tx_fifo_status_get(
    int unit,
    bcm_port_t port,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *pm_credits)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_port_tx_fifo_status_get(unit, port, max_occupancy, fifo_level, pm_credits));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set port advertise abilities
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in]  num_ability     - number of ability the port will advertise
 * \param [in]  abilities       - local advertisement for each ability
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
imb_cdu_port_autoneg_ability_advert_set(
    int unit,
    bcm_port_t port,
    int num_ability,
    const bcm_port_speed_ability_t * abilities)
{
    portmod_port_speed_ability_t *an_abilities = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(an_abilities, num_ability * sizeof(portmod_port_speed_ability_t), "port an abilities", "%s%s%s\r\n",
              EMPTY, EMPTY, EMPTY);
    sal_memset(an_abilities, 0, num_ability * sizeof(portmod_port_speed_ability_t));

    imb_portmod_an_ability_from_bcm_an_ability_get(num_ability, abilities, an_abilities);

    SHR_IF_ERR_EXIT(portmod_port_autoneg_ability_advert_set(unit, port, num_ability, an_abilities));
exit:
    SHR_FREE(an_abilities);
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get port advertise abilities
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in]  max_num_ability   - the max num of autoneg ability this port can advertise
 * \param [out]  abilities       - indicate the ability this port can advertise
 * \param [out]  actual_num_ability   - the actual num of ability that this port can advertise
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
imb_cdu_port_autoneg_ability_advert_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    portmod_port_speed_ability_t *an_abilities = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(an_abilities, max_num_ability * sizeof(portmod_port_speed_ability_t), "port an abilities", "%s%s%s\r\n",
              EMPTY, EMPTY, EMPTY);
    sal_memset(an_abilities, 0, max_num_ability * sizeof(portmod_port_speed_ability_t));

    SHR_IF_ERR_EXIT(portmod_port_autoneg_ability_advert_get
                    (unit, port, max_num_ability, an_abilities, actual_num_ability));

    imb_bcm_an_ability_from_portmod_an_ability_get(*actual_num_ability, an_abilities, abilities);

exit:
    SHR_FREE(an_abilities);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the local speed abilities 
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in]  max_num_ability   - the max num of autoneg ability this port can advertise
 * \param [out]  abilities       - indicate the ability this port can advertise
 * \param [out]  actual_num_ability   - the actual num of ability that this port can advertise
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
imb_cdu_port_speed_ability_local_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    portmod_port_speed_ability_t *an_abilities = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(an_abilities, max_num_ability * sizeof(portmod_port_speed_ability_t), "port an abilities",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(an_abilities, 0, max_num_ability * sizeof(portmod_port_speed_ability_t));

    SHR_IF_ERR_EXIT(portmod_port_speed_ability_local_get
                    (unit, port, max_num_ability, an_abilities, actual_num_ability));

    imb_bcm_an_ability_from_portmod_an_ability_get(*actual_num_ability, an_abilities, abilities);

exit:
    SHR_FREE(an_abilities);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the remote abilities
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in]  max_num_ability   - the max num of autoneg ability this port can advertise
 * \param [out]  abilities       - indicate the ability this port can advertise
 * \param [out]  actual_num_ability   - the actual num of ability that this port can advertise
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
imb_cdu_port_autoneg_ability_remote_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    portmod_port_speed_ability_t *an_abilities = NULL;
    phymod_autoneg_status_t an_status;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_autoneg_status_get(unit, port, &an_status));

    if (!an_status.enabled)
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED, "AN is not enabled for port %d! \n", port);
    }
    else if (!an_status.locked)
    {
        SHR_ERR_EXIT(_SHR_E_BUSY, "AN status is not locked for port %d! \n", port);
    }
    else
    {
        SHR_ALLOC(an_abilities, max_num_ability * sizeof(portmod_port_speed_ability_t), "port an abilities",
                  "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        sal_memset(an_abilities, 0, max_num_ability * sizeof(portmod_port_speed_ability_t));

        SHR_IF_ERR_EXIT(portmod_port_autoneg_ability_remote_get
                        (unit, port, max_num_ability, an_abilities, actual_num_ability));
        imb_bcm_an_ability_from_portmod_an_ability_get(*actual_num_ability, an_abilities, abilities);
    }

exit:
    SHR_FREE(an_abilities);
    SHR_FUNC_EXIT;

}

/**
 * \brief - Configure CDU scheduler
 *
 * \param [in] unit   - chip unit id
 * \param [in] port   - logical port
 * \param [in] enable - 0:disable port, 1:enable port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *  * None
 */
int
imb_cdu_port_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;
    int weight = 0;

    SHR_FUNC_INIT_VARS(unit);
    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_CDU_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }

        /** Set the port's CDU as active in the specified scheduler */
        SHR_IF_ERR_EXIT(imb_cdu_port_scheduler_active_set(unit, port, rmc.sch_priority, enable));

        /** Set RMC priority (Low, Priority, TDM) */
        SHR_IF_ERR_EXIT(imb_cdu_port_rmc_scheduler_config_set(unit, port, rmc.rmc_id, rmc.sch_priority));

        if (rmc.sch_priority == bcmPortNifSchedulerLow)
        {
            /** Configure RMC weight in Low priority scheduler */
            SHR_IF_ERR_EXIT(dnx_port_algo_imb_rmc_scheduler_weight_get(unit, port, &weight));
            SHR_IF_ERR_EXIT(imb_cdu_rmc_low_prio_scheduler_config_set(unit, port, rmc.rmc_id, weight));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get recommended default values of resource members for given port with speed
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - not used
 * \param [out] resource - each memeber other than speed with BCM_PORT_RESOURCE_DEFAULT_REQUEST
 *                         will be filled with default value
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
imb_cdu_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    bcm_pbmp_t phys;
    int num_lane;
    int lane_rate;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    if ((resource->flags != 0) || (resource->physical_port != 0) || (resource->encap != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: flags, physical_port and encap fields of bcm_port_resource_t are not in use. They should all be initialized to 0",
                     port);
    }

    if ((resource->port != 0) && (resource->port != port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port %d: resource->port should be 0 or equal to port passed to the API", port);
    }

    if (resource->speed == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: there is no given default value for speed. speed is mandatory as input to this API",
                     port);
    }

    /*
     *  calculate lane rate
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    BCM_PBMP_COUNT(phys, num_lane);
    lane_rate = resource->speed / num_lane;

    /*
     * in case user didn't configure the FEC already - give default value
     */
    if (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {

        switch (lane_rate)
        {
            case 10000:
            case 20000:
            case 25000:
                resource->fec_type = bcmPortPhyFecNone;
                break;
            case 50000:

                if ((num_lane == 8) || (num_lane == 4))
                {
                    /*
                     * For 400G,200G return 544 2xN
                     */
                    resource->fec_type = bcmPortPhyFecRs544_2xN;
                }
                else
                {
                    resource->fec_type = bcmPortPhyFecRs544;
                }

                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "port %d: Invalid speed (%d) and number of lanes (%d) configuration",
                             port, resource->speed, num_lane);
        }
    }

    /*
     * in case user didn't configure CL72 - by default it is enabled
     */
    if (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->link_training = 1;
    }

    /*
     * get default lane_config
     */
    if (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->phy_lane_config = 0;
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                     SOC_DNXC_PORT_LANE_CONFIG_MEDIA_TYPE_BACKPLANE_COPPER_TRACE);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(resource->phy_lane_config);

        /*
         * FORCE_ES, FORCE_NS must be clear for {NRZ} and for {PAM4 with link training}
         */
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(resource->phy_lane_config);

        if (DNXC_PORT_PHY_SPEED_IS_PAM4(lane_rate) && (resource->link_training == 0))
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(resource->phy_lane_config);
        }

        /*
         * internal fields that will be set in resource_set before calling portmod. Not exposed to user.
         */
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(resource->phy_lane_config);
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(resource->phy_lane_config);
        /*
         * this field should always be 0 in normal operation, can be changed only for debug.
         */
        PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_SET(resource->phy_lane_config, 0);

    }

    resource->port = port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map SIF instance (port) into NIF port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port. if port==BCM_PORT_INVALID, disconnect the sif instance from the NIF
 * \param [in] instace_id - statistics interface instance (port) id
 * \param [in] instace_core - the core that the sif instance belong to.
 * \param [in] connect - TRUE to connect stif instance to port, FALSE to disconnect
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_to_stif_instance_map_set(
    int unit,
    bcm_port_t port,
    int instace_id,
    int instace_core,
    int connect)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    int inner_cdu_id = 0;
    int first_lane_in_cdu = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** if connect, get the cdu info, if disconnect, cdu params initiate to zero */
    if (connect)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
        if (cdu_info.core != instace_core)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "core mismatch between {logical port=%d,core=%d} and {sif port=%d, core=%d}",
                         port, cdu_info.core, instace_id, instace_core);
        }
        inner_cdu_id = cdu_info.inner_cdu_id;
        first_lane_in_cdu = cdu_info.first_lane_in_cdu;
    }
    SHR_IF_ERR_EXIT(imb_cdu_internal_port_to_stif_instance_map
                    (unit, instace_core, instace_id, inner_cdu_id, first_lane_in_cdu));
exit:
    SHR_FUNC_EXIT;
}

/*
 * imb_cdu_phys_get
 *
 * @brief Get phys associated with imb instance
 *
 * @param [in]  unit             - unit id
 * @param [in]  imb_id           - imb instance id
 * @param [out]  phys            - phys associated with imb instance
 */
int
imb_cdu_phys_get(
    int unit,
    int imb_id,
    bcm_pbmp_t * phys)
{

    int pm_index;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(*phys);
    pm_index = dnx_data_nif.eth.cdu_pm_map_get(unit, imb_id)->pms[0];
    *phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Set Reset PM State
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_id - imb instance id
 * \param [in] in_reset - Reset state
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_pms_reset_set(
    int unit,
    int imb_id,
    uint32 in_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_pm_reset_set(unit, imb_id, in_reset));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Reset PM State
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_id - imb instance id
 * \param [out] in_reset - Reset state
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_pms_reset_get(
    int unit,
    int imb_id,
    uint32 *in_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_internal_pm_reset_get(unit, imb_id, in_reset));

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_port_gen_pfc_from_rmc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 enable)
{
    int rmc;
    bcm_pbmp_t rmcs;
    dnx_algo_port_cdu_access_info_t cdu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine the CDU access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_pbmp_get(unit, port, 0, &rmcs));
    _SHR_PBMP_ITER(rmcs, rmc)
    {
        /** Set HW to enable all RMCs of the port to produce PFC for the specified priority */
        SHR_IF_ERR_EXIT(imb_cdu_internal_port_gen_pfc_from_rmc_enable_set
                        (unit, cdu_access_info.core, cdu_access_info.inner_cdu_id, rmc, priority, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_port_gen_pfc_from_rmc_enable_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *enable)
{
    int rmc;
    bcm_pbmp_t rmcs;
    dnx_algo_port_cdu_access_info_t cdu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    /** Determine the CDU access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_pbmp_get(unit, port, 0, &rmcs));

    /** Check HW if the RMCs of the port are enabled to produce PFC for the specified priority */
    /** Get the Enable field of the first RMC only, the rest have the same value */
    _SHR_PBMP_FIRST(rmcs, rmc);
    SHR_IF_ERR_EXIT(imb_cdu_internal_port_gen_pfc_from_rmc_enable_get
                    (unit, cdu_access_info.core, cdu_access_info.inner_cdu_id, rmc, priority, enable));

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_port_gen_llfc_from_rmc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    int rmc;
    bcm_pbmp_t rmcs;
    int lane;
    bcm_pbmp_t lanes;
    int nof_phys_per_cdu;
    int lane_in_cdu;
    dnx_algo_port_cdu_access_info_t cdu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine the CDU access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_pbmp_get(unit, port, 0, &rmcs));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &lanes));
    nof_phys_per_cdu = dnx_data_nif.eth.cdu_lanes_nof_get(unit);

    /** Mapping of port's RMCs(all lanes) to generate LLFC */
    _SHR_PBMP_ITER(lanes, lane)
    {
        lane_in_cdu = lane % nof_phys_per_cdu;
        _SHR_PBMP_ITER(rmcs, rmc)
        {
            SHR_IF_ERR_EXIT(imb_cdu_internal_port_gen_llfc_from_rmc_enable_set
                            (unit, cdu_access_info.core, cdu_access_info.inner_cdu_id, lane_in_cdu, rmc, enable));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_port_stop_pm_from_cfc_llfc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    int lane;
    bcm_pbmp_t lanes;
    int nof_phys_per_cdu;
    uint32 lane_in_cdu;
    dnx_algo_port_cdu_access_info_t cdu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine the CUD access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &lanes));
    nof_phys_per_cdu = dnx_data_nif.eth.cdu_lanes_nof_get(unit);

    /** Set HW to stop transmission on all lanes of the port according LLFC signal received from CFC */
    _SHR_PBMP_ITER(lanes, lane)
    {
        lane_in_cdu = lane % nof_phys_per_cdu;
        SHR_IF_ERR_EXIT(imb_cdu_internal_port_stop_pm_from_cfc_llfc_enable_set
                        (unit, cdu_access_info.core, cdu_access_info.inner_cdu_id, lane_in_cdu, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

static int
imb_cdu_port_fc_rx_qmlf_threshold_set_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 *threshold,
    uint8 is_set)
{
    dnx_algo_port_rmc_info_t rmc;
    int rmc_fifo_size = 0;
    int nof_prio_groups_supported;
    int prio_group;
    dnx_algo_port_type_e port_type = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    /*
     * Validate threshold flags
     */
    if (!(flags & (BCM_COSQ_THRESHOLD_SET | BCM_COSQ_THRESHOLD_CLEAR)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Unit %d, Need to set at least one of flags BCM_COSQ_THRESHOLD_SET BCM_COSQ_THRESHOLD_CLEAR\n",
                     unit);
    }

    if (!(flags & (BCM_COSQ_THRESHOLD_ETH_PORT_LLFC | BCM_COSQ_THRESHOLD_ETH_PORT_PFC)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Unit %d, Need to set one of flags BCM_COSQ_THRESHOLD_ETH_PORT_LLFC BCM_COSQ_THRESHOLD_ETH_PORT_PFC\n",
                     unit);
    }

    if ((flags & BCM_COSQ_THRESHOLD_ETH_PORT_LLFC) && (flags & BCM_COSQ_THRESHOLD_ETH_PORT_PFC))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Unit %d, Can not set both of flags BCM_COSQ_THRESHOLD_ETH_PORT_LLFC BCM_COSQ_THRESHOLD_ETH_PORT_PFC\n",
                     unit);
    }

    /** Check the port type, thresholds are only for Ethernet ports */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    if (!(DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, FALSE)))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unit %d, Thresholds can only be set for Ethernet ports. \n", unit);
    }

    if (is_set)
    {
        /** Determine the RMC size. The threshold can be of any size up to the RMC size */
        nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);
        for (prio_group = 0; prio_group < nof_prio_groups_supported; ++prio_group)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, prio_group, &rmc));
            if (rmc.rmc_id == -1)
            {
                /** RMC is invalid */
                continue;
            }
            rmc_fifo_size = rmc.last_entry - rmc.first_entry + 1;
        }

        if (*threshold > rmc_fifo_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Unit %d, Unsupported threshold value! Must be of any size up to the RMC size: %d\n", unit,
                         rmc_fifo_size);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_port_fc_rx_qmlf_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 threshold)
{
    int rmc;
    bcm_pbmp_t rmcs;
    dnx_algo_port_cdu_access_info_t cdu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(imb_cdu_port_fc_rx_qmlf_threshold_set_get_verify(unit, port, flags, &threshold, TRUE));

    /** Determine the CUD access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_pbmp_get(unit, port, 0, &rmcs));

    /** The threshold should be set for all RMCs of the port */
    _SHR_PBMP_ITER(rmcs, rmc)
    {
        /** Set the threshold value to HW */
        SHR_IF_ERR_EXIT(imb_cdu_internal_port_fc_rx_qmlf_threshold_set
                        (unit, cdu_access_info.core, cdu_access_info.inner_cdu_id, rmc, flags, threshold));
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_port_fc_rx_qmlf_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 *threshold)
{
    int rmc;
    bcm_pbmp_t rmcs;
    dnx_algo_port_cdu_access_info_t cdu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(imb_cdu_port_fc_rx_qmlf_threshold_set_get_verify(unit, port, flags, threshold, FALSE));

    /** Determine the CUD access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_pbmp_get(unit, port, 0, &rmcs));

    /** Get the threshold value of the first RMC only, the rest have the same value */
    _SHR_PBMP_FIRST(rmcs, rmc);
    SHR_IF_ERR_EXIT(imb_cdu_internal_port_fc_rx_qmlf_threshold_get
                    (unit, cdu_access_info.core, cdu_access_info.inner_cdu_id, rmc, flags, threshold));

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_fc_reset_set(
    int unit,
    const imb_create_info_t * imb_info,
    uint32 in_reset)
{
    int nof_cdus_per_core, cdu_id;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
    core = imb_info->inst_id >= nof_cdus_per_core ? 1 : 0;
    cdu_id = imb_info->inst_id % nof_cdus_per_core;

    /** Set HW to put in/out of reset all CDUs */
    SHR_IF_ERR_EXIT(imb_cdu_internal_fc_reset_set(unit, core, cdu_id, in_reset));

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_llfc_from_glb_rsc_enable_set(
    int unit,
    const imb_create_info_t * imb_info,
    uint32 enable)
{
    int nof_cdus_per_core, cdu_id, lane;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
    core = imb_info->inst_id >= nof_cdus_per_core ? 1 : 0;
    cdu_id = imb_info->inst_id % nof_cdus_per_core;

    /** Set HW to enable/disable the generation of LLFC based on a signal for Global resources for all lanes of the CDU */
    for (lane = 0; lane < dnx_data_nif.eth.cdu_lanes_nof_get(unit); lane++)
    {
        SHR_IF_ERR_EXIT(imb_cdu_internal_llfc_from_glb_rsc_enable_set(unit, core, cdu_id, lane, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_pfc_rec_priority_map(
    int unit,
    int imb_id,
    uint32 nif_priority,
    uint32 egq_priority)
{
    imb_create_info_t imb_info;
    uint32 nif_priority_internal = 0;
    uint32 egq_priority_internal = 0;
    int nof_cdus_per_core, cdu_id;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    /** Get IMB info  and verify parameters */
    SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb_info));

    nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
    core = imb_info.inst_id >= nof_cdus_per_core ? 1 : 0;
    cdu_id = imb_info.inst_id % nof_cdus_per_core;

    /** Standard case */
    nif_priority_internal = nif_priority;
    egq_priority_internal = egq_priority;

    /** In Jericho 2 the last two CDUs of each core have one bit shift left so the bitmap configuration is changed.
      * The implications here are that the mapping at CFC also needs to be shifted, causing the last priority of the last
      * NIF port to be lost and the first priority of the cdu=(nof_cdus-2) to be unused */
    if (dnx_data_fc.inband.pfc_rec_cdu_shift_get(unit) && (cdu_id >= (dnx_data_nif.eth.cdu_nof_per_core_get(unit) - 2)))
    {
        nif_priority_internal = nif_priority;
        egq_priority_internal = ((egq_priority == 0) ? (BCM_COS_COUNT - 1) : (egq_priority - 1));
    }

    /** Set mapping to HW*/
    SHR_IF_ERR_EXIT(imb_cdu_internal_pfc_rec_priority_map
                    (unit, core, cdu_id, nif_priority_internal, egq_priority_internal));

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_pfc_rec_priority_unmap(
    int unit,
    int imb_id)
{
    imb_create_info_t imb_info;
    int nof_cdus_per_core, cdu_id;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    /** Get IMB info  and verify parameters */
    SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb_info));

    nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
    core = imb_info.inst_id >= nof_cdus_per_core ? 1 : 0;
    cdu_id = imb_info.inst_id % nof_cdus_per_core;

    /** Unmap the specified CDU in HW for all priorities */
    SHR_IF_ERR_EXIT(imb_cdu_internal_pfc_rec_priority_unmap(unit, core, cdu_id));

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
