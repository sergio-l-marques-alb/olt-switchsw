/** \file kbp_recover.c
 *
 * Functions for handling Jericho2 KBP recover.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_ELKDNX

/*
 * {
 */

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/port.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dbal/dbal.h>

#if defined(INCLUDE_KBP)

#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_rop.h>
#include <bcm_int/dnx/kbp/kbp_recover.h>
#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];

/*
 * }
 */

/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */
/**
 * \brief
 *   Reset the KBP TX PCS, then restore the
 *   original configuration.
 * \param [in] unit - The unit number.
 * \param [in] mdio_id - The kbp mdio ID.
 * \param [in] flags - mdio access flags.
 * \param [in] kbp_port_id - kbp port ID.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_tx_pcs_reset(
    int unit,
    uint32 mdio_id,
    int kbp_port_id)
{
    uint8 data_8[2];
    uint32 kbp_reg_addr;

    SHR_FUNC_INIT_VARS(unit);

    kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PCS_EN_REG);
    SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_read(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));
    data_8[1] &= ~(1 << kbp_port_id);
    SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));

    sal_usleep(200000);

    data_8[1] |= (1 << kbp_port_id);
    SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Reset the KBP TX phase matching fifo.
 * \param [in] unit - The unit number.
 * \param [in] mdio_id - The kbp mdio ID.
 * \param [in] flags - mdio access flags.
 * \param [in] kbp_port_id - kbp port ID.
 * \param [in] reset - 0: out of reset, 1: in reset.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_phase_matching_fifo_reset(
    int unit,
    uint32 mdio_id,
    int kbp_port_id,
    int reset)
{
    uint16 reg_val;
    uint8 data_8[2];
    uint32 kbp_reg_addr;
    SHR_FUNC_INIT_VARS(unit);

    reg_val = reset ? DNX_KBP_TX_PHASE_MATCHING_FIFO_IN_RESET_VAL : DNX_KBP_TX_PHASE_MATCHING_FIFO_OUT_OF_RESET_VAL;
    data_8[0] = (reg_val >> 8) & 0xFF;
    data_8[1] = reg_val & 0xFF;
    if (kbp_port_id)
    {
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD4_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD5_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD6_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD7_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD8_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));
    }
    else
    {
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD0_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD1_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD2_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD3_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p, 16, kbp_reg_addr, data_8));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get KBP record counter.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] counter_val - counter value.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_recover_txpcs_soft_reset(
    int unit,
    int core,
    bcm_port_t port,
    uint32 mdio_id)
{
    bcm_port_link_state_t link_state;
    int kbp_port_id = 0, i;

    SHR_FUNC_INIT_VARS(unit);

    kbp_port_id =
        (dnx_data_elk.connectivity.topology_get(unit)->port_core_mapping[kbp_port_id] ==
         core) ? kbp_port_id : kbp_port_id + 1;

    SHR_IF_ERR_EXIT(dnx_kbp_tx_pcs_reset(unit, mdio_id, kbp_port_id));

    SHR_IF_ERR_EXIT(dnx_kbp_phase_matching_fifo_reset(unit, mdio_id, kbp_port_id, 1));
    sal_usleep(200000);
    SHR_IF_ERR_EXIT(dnx_kbp_phase_matching_fifo_reset(unit, mdio_id, kbp_port_id, 0));

    /*
     * Loop 1s until the link up 
     */
    for (i = 0; i < DNX_KBP_RECOVER_LINK_UP_LOOP_TIMES; i++)
    {
        sal_usleep(10000);
        SHR_IF_ERR_EXIT(bcm_port_link_state_get(unit, port, 0, &link_state));
        if (link_state.status == BCM_PORT_LINK_STATUS_UP)
        {
            break;
        }
    }
    if (i == DNX_KBP_RECOVER_LINK_UP_LOOP_TIMES)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "LINK NOT UP AFTER 1 sec for port %d\n"), port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Assemble the KBP CPU record.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] opcode - KBP opcode number.
 * \param [in] msb_data - MSB data.
 * \param [in] lsb_data - LSB data.
 * \param [in] lsb_enable - enable the LSB data.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_recover_run_recovery_sequence(
    int unit,
    uint32 core,
    uint32 mdio_id)
{
    int i;
    bcm_port_t ilkn_port;
    dnx_kbp_record_reply_t reccord_reply;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_ilkn_interface_param_get(unit, core, &ilkn_port, NULL, NULL, NULL));
    SHR_IF_ERR_EXIT(dnx_kbp_recover_txpcs_soft_reset(unit, core, ilkn_port, mdio_id));
    /*
     * Loop until the valid indicator is cleared 
     */
    for (i = 0; i < DNX_KBP_RECORD_REPLY_VALID_CHECK_TIMES; i++)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_cpu_lookup_reply(unit, core, &reccord_reply));
        if (reccord_reply.reply_valid == 0)
        {
            break;
        }
    }
    if (i == DNX_KBP_RECORD_REPLY_VALID_CHECK_TIMES)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Record reply valid indicator cannot be cleared\n")));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * }
 */
#endif /* defined(INCLUDE_KBP) */

#undef BSL_LOG_MODULE
