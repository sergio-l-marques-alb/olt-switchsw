/** \file bcm/dnx/synce/synce.c
 *
 * Functions for handling SyncE init sequences.
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SYNCEDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm/error.h>
#include <bcm_int/dnx_dispatch.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_synce.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define DNX_INVALID_SYNCE_SRC_PORT  -1

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

/**
 * \brief
 *    Get the Synce configuration from DNX DATA,
 *    and configure them using BCM API.
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_synce_init(
    int unit)
{
    int synce_index;
    int nof_synce_plls;
    const dnx_data_synce_general_cfg_t *synce_cfg;
    bcm_switch_control_t src_clk_type, div_type;

    SHR_FUNC_INIT_VARS(unit);

    nof_synce_plls = dnx_data_synce.general.nof_plls_get(unit);

    for (synce_index = 0; synce_index < nof_synce_plls; synce_index++)
    {
        synce_cfg = dnx_data_synce.general.cfg_get(unit, synce_index);

        if (synce_cfg->source_clock_port != DNX_INVALID_SYNCE_SRC_PORT)
        {
            if (synce_index == DBAL_ENUM_FVAL_SYNCE_INDEX_MASTER_SYNCE)
            {
                src_clk_type = bcmSwitchSynchronousPortClockSource;
                div_type = bcmSwitchSynchronousPortClockSourceDivCtrl;
            }
            else
            {
                src_clk_type = bcmSwitchSynchronousPortClockSourceBkup;
                div_type = bcmSwitchSynchronousPortClockSourceBkupDivCtrl;
            }
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, src_clk_type, synce_cfg->source_clock_port));
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, div_type, synce_cfg->output_clock_sel));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Deinit SyncE.
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_synce_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Currently nothing to deinit
     */
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
