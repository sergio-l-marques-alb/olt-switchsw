/*
 * $Id: dnx_pll_init.c, v1 09/06/2017 09:55:39 veselag $
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

/*************
 * INCLUDES  *
 *************/

/* SOC includes */
#include <soc/debug.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_debug.h>
#ifndef CMODEL_SERVER_MODE
#include <soc/sand/sand_aux_access.h>
#endif

/* SOC DNX includes */
#include <soc/dnx/pll/pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/gddr6/gddr6.h>
/* BCM DNX includes */
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/drv.h>
/*************
 * DEFINES   *
 *************/
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_PLL

/*************
 * TYPE DEFS *
 *************/

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief
 *  PLL1 init function.
 *  Generic function used to calculate and set the parameters to HW for all PLL1
 * \param [in] unit -
 *   The unit number.
 * \param [in] pll1_type -
 *   The type of PLL to be initialized.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
soc_dnx_pll_1_init(
    int unit,
    DNX_PLL1_TYPE pll1_type)
{
    uint32 entry_handle_id;
    int refclk_source_sel = -1, frefeff = -1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * For TS PLL the reference clock is set with SOC property
     */
    if (pll1_type == DNX_PLL1_TYPE_TS)
    {
        refclk_source_sel = (dnx_data_pll.pll1.feature_get(unit, dnx_data_pll_pll1_ts_refclk_source_internal) == 0);
    }
    else
    {
        refclk_source_sel = dnx_data_pll.pll1.config_get(unit, pll1_type)->refclk_source_sel;
    }
    /**
     * Get handle to the configuration DBAL table for PLL1
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL1_CFG, &entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL1_ID,
                               dnx_data_pll.pll1.config_get(unit, pll1_type)->pll1_id);
    /*
     * Set the configuration values for the fields according to the PLL type
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NDIV_INT, INST_SINGLE,
                                 dnx_data_pll.pll1.config_get(unit, pll1_type)->ndiv_int);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDIV, INST_SINGLE,
                                 dnx_data_pll.pll1.config_get(unit, pll1_type)->pdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTPUT_CML_EN, INST_SINGLE,
                                 dnx_data_pll.pll1.config_get(unit, pll1_type)->output_cml_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REFCLK_SOURCE_SEL, INST_SINGLE, refclk_source_sel);
    /**
     * The effective frequency can be calculated from the refference clock and P divider
     */
    frefeff =
        dnx_data_pll.pll1.config_get(unit, pll1_type)->refclk / dnx_data_pll.pll1.config_get(unit, pll1_type)->pdiv;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FREFEFF_INFO, INST_SINGLE, frefeff);
    /**
     * The M dividers for each channel will only be set if the channel is enabled for the specified PLL type
     */
    if (dnx_data_pll.pll1.config_get(unit, pll1_type)->ch0_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_0_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll1.config_get(unit, pll1_type)->ch0_mdiv);
    }
    if (dnx_data_pll.pll1.config_get(unit, pll1_type)->ch1_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_1_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll1.config_get(unit, pll1_type)->ch1_mdiv);
    }
    if (dnx_data_pll.pll1.config_get(unit, pll1_type)->ch2_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_2_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll1.config_get(unit, pll1_type)->ch2_mdiv);
    }
    if (dnx_data_pll.pll1.config_get(unit, pll1_type)->ch3_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_3_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll1.config_get(unit, pll1_type)->ch3_mdiv);
    }
    if (dnx_data_pll.pll1.config_get(unit, pll1_type)->ch4_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_4_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll1.config_get(unit, pll1_type)->ch4_mdiv);
    }
    if (dnx_data_pll.pll1.config_get(unit, pll1_type)->ch5_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_5_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll1.config_get(unit, pll1_type)->ch5_mdiv);
    }
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Get handle to the configuration DBAL table for PLL1 again for reset
     */
    sal_usleep(30);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PLL1_CFG, entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL1_ID,
                               dnx_data_pll.pll1.config_get(unit, pll1_type)->pll1_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_B, INST_SINGLE, 1);
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

#ifndef CMODEL_SERVER_MODE
    /*
     * Polling on pll_locked
     */
    if (soc_sand_is_emulation_system(unit) == 0)
    {
        /*
         * Get handle to the DBAL table
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PLL1_STATUS, entry_handle_id));
        /*
         * Set KEY field
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL1_ID,
                                   dnx_data_pll.pll1.config_get(unit, pll1_type)->pll1_id);
        SHR_IF_ERR_EXIT(dnxcmn_polling
                        (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_LOCK_STATUS, 1));
    }
#endif

    /*
     * Get handle to the configuration DBAL table for PLL1 again for post-reset
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PLL1_CFG, entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL1_ID,
                               dnx_data_pll.pll1.config_get(unit, pll1_type)->pll1_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POST_RESET_B, INST_SINGLE, 1);
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  PLL3 set function.
 *  Generic function used to set the parameters to HW for all PLL3
 * \param [in] unit -
 *   The unit number.
 * \param [in] pll3_type -
 *   The type of PLL to be initialized.
 * \param [in] ndiv -
 *   The N divider.
 * \param [in] mdiv -
 *   The M divider(for channel 0).
 * \param [in] pdiv -
 *   The Pre divider.
 * \param [in] fref -
 *   The effective reference frequency.
 * \param [in] is_bypass -
 *   Indicates if is Bypass mode.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
soc_dnx_pll_3_set(
    int unit,
    DNX_PLL3_TYPE pll3_type,
    uint32 ndiv,
    uint32 mdiv,
    uint32 pdiv,
    uint32 fref,
    uint32 is_bypass)
{
    uint32 entry_handle_id, route_ctrl = 0, en_ctrl;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * For bypass mode the values for en_ctrl and route_ctrl are different
     */
    if (is_bypass)
    {
        route_ctrl = dnx_data_pll.pll3.route_ctr_byp_get(unit);
        en_ctrl = dnx_data_pll.pll3.en_ctrl_byp_get(unit);
    }
    else
    {
        en_ctrl = dnx_data_pll.pll3.en_ctrl_get(unit);
    }

    /*
     * Get handle to the configuration DBAL table for PLL3
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL3_CFG, &entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL3_ID,
                               dnx_data_pll.pll3.config_get(unit, pll3_type)->pll3_id);
    /*
     * Set the configuration values for the fields according to the PLL type and input parameters
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KP, INST_SINGLE, dnx_data_pll.pll3.kp_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KI, INST_SINGLE, dnx_data_pll.pll3.ki_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDIV, INST_SINGLE, pdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NDIV_INT, INST_SINGLE, ndiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_0_MDIV, INST_SINGLE, mdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EN_CTRL, INST_SINGLE, en_ctrl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROUTE_CTRL, INST_SINGLE, route_ctrl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FREFEFF_INFO, INST_SINGLE, fref);
    /**
     * The M dividers for each channel will only be set if the channel is enabled for the specified PLL type.
     * Exception is channel 0, for which the M divider needs to be calculated according to the in and out frequencies
     * and is input for the function.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_0_MDIV, INST_SINGLE, mdiv);
    if (dnx_data_pll.pll3.config_get(unit, pll3_type)->ch1_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_1_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll3.config_get(unit, pll3_type)->ch1_mdiv);
    }
    if (dnx_data_pll.pll3.config_get(unit, pll3_type)->ch2_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_2_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll3.config_get(unit, pll3_type)->ch2_mdiv);
    }
    if (dnx_data_pll.pll3.config_get(unit, pll3_type)->ch3_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_3_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll3.config_get(unit, pll3_type)->ch3_mdiv);
    }
    if (dnx_data_pll.pll3.config_get(unit, pll3_type)->ch4_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_4_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll3.config_get(unit, pll3_type)->ch4_mdiv);
    }
    if (dnx_data_pll.pll3.config_get(unit, pll3_type)->ch5_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_5_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll3.config_get(unit, pll3_type)->ch5_mdiv);
    }
    /*
     * Set hardcoded values for the fields that depend on the PLL type, reference and output frequencies chosen
     */

    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Get handle to the configuration DBAL table for PLL3 again for reset
     */
    sal_usleep(40);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PLL3_CFG, entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL3_ID,
                               dnx_data_pll.pll3.config_get(unit, pll3_type)->pll3_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_B, INST_SINGLE, 1);
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

#ifndef CMODEL_SERVER_MODE
    /*
     * Polling on pll_locked
     */
    if ((soc_sand_is_emulation_system(unit) == 0) && (dnx_data_dev_init_general_access_only_get(unit) == 0))
    {
        /*
         * Get handle to the DBAL table
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PLL3_STATUS, entry_handle_id));
        /*
         * Set KEY field
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL3_ID,
                                   dnx_data_pll.pll3.config_get(unit, pll3_type)->pll3_id);
        SHR_IF_ERR_EXIT(dnxcmn_polling
                        (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_LOCK_STATUS, 1));
    }
#endif

    /*
     * Get handle to the configuration DBAL table for PLL3 again for post-reset
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PLL3_CFG, entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL3_ID,
                               dnx_data_pll.pll3.config_get(unit, pll3_type)->pll3_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POST_RESET_B, INST_SINGLE, 1);
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  PLL4 set function.
 *  Generic function used to set the parameters to HW for all PLL4
 * \param [in] unit -
 *   The unit number.
 * \param [in] pll4_type -
 *   The type of PLL to be initialized.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
soc_dnx_pll_4_init(
    int unit,
    DNX_PLL4_TYPE pll4_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get handle to the configuration DBAL table for PLL4
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL4_CFG, &entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL4_ID,
                               dnx_data_pll.pll4.config_get(unit, pll4_type)->pll4_id);
    /*
     * Set the configuration values for the fields according to the PLL type and input parameters
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REFCLK_SOURCE_SEL, INST_SINGLE,
                                 dnx_data_pll.pll4.config_get(unit, pll4_type)->ref_clk_src_sel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REFCLK, INST_SINGLE,
                                 dnx_data_pll.pll4.config_get(unit, pll4_type)->ref_clk);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DLL_REF_PDIV, INST_SINGLE,
                                 dnx_data_pll.pll4.config_get(unit, pll4_type)->dll_ref_pdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DLL_POST_EN, INST_SINGLE,
                                 dnx_data_pll.pll4.config_get(unit, pll4_type)->dll_post_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AUX_POST_EN, INST_SINGLE,
                                 dnx_data_pll.pll4.config_get(unit, pll4_type)->aux_post_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDIV, INST_SINGLE,
                                 dnx_data_pll.pll4.config_get(unit, pll4_type)->pdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NDIV_INT, INST_SINGLE,
                                 dnx_data_pll.pll4.config_get(unit, pll4_type)->ndiv_int);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_D2C_EN, INST_SINGLE,
                                 dnx_data_pll.pll4.config_get(unit, pll4_type)->d2c_en);
    /**
     * The M dividers for each channel will only be set if the channel is enabled for the specified PLL type.
     * Exception is channel 0, for which the M divider needs to be calculated according to the in and out frequencies
     * and is input for the function.
     */
    if (dnx_data_pll.pll4.config_get(unit, pll4_type)->ch0_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_0_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll4.config_get(unit, pll4_type)->ch0_mdiv);
    }
    if (dnx_data_pll.pll4.config_get(unit, pll4_type)->ch1_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_1_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll4.config_get(unit, pll4_type)->ch1_mdiv);
    }
    if (dnx_data_pll.pll4.config_get(unit, pll4_type)->ch2_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_2_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll4.config_get(unit, pll4_type)->ch2_mdiv);
    }
    if (dnx_data_pll.pll4.config_get(unit, pll4_type)->ch3_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_3_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll4.config_get(unit, pll4_type)->ch3_mdiv);
    }
    if (dnx_data_pll.pll4.config_get(unit, pll4_type)->ch6_mdiv != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_6_MDIV, INST_SINGLE,
                                     dnx_data_pll.pll4.config_get(unit, pll4_type)->ch6_mdiv);
    }

    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Get handle to the configuration DBAL table for PLL4 again for reset
     */
    sal_usleep(40);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PLL4_CFG, entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL4_ID,
                               dnx_data_pll.pll4.config_get(unit, pll4_type)->pll4_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_B, INST_SINGLE, 1);
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

#ifndef CMODEL_SERVER_MODE
    /*
     * Polling on pll_locked
     */
    if (soc_sand_is_emulation_system(unit) == 0)
    {
        /*
         * Get handle to the DBAL table
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PLL4_STATUS, entry_handle_id));
        /*
         * Set KEY field
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL4_ID,
                                   dnx_data_pll.pll4.config_get(unit, pll4_type)->pll4_id);
        SHR_IF_ERR_EXIT(dnxcmn_polling
                        (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_LOCK_STATUS, 1));
    }
#endif

    /*
     * Get handle to the configuration DBAL table for PLL4 again for post-reset
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PLL4_CFG, entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL4_ID,
                               dnx_data_pll.pll4.config_get(unit, pll4_type)->pll4_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POST_RESET_B, INST_SINGLE, 1);
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   SyncE PLL initialization function
 * \param [in] unit -
 *   The unit number.
 * \param [in] synce_index -
 *   Master or Slave SyncE.
 * \param [in] ndiv -
 *   ndiv for synce pll.
 * \param [in] mdiv -
 *   mdiv for synce pll.
 * \param [in] pdiv -
 *   pdiv for synce pll.
 * \param [in] fref -
 *   The effective reference frequency.
 * \param [in] is_bypass -
 *   If bypass the pll lock check
 * \param [in] is_nif -
 *   If is_nif the pll select nif ports,otherwise,select fabric
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
soc_dnx_synce_pll_set(
    int unit,
    int synce_index,
    uint32 ndiv,
    uint32 mdiv,
    uint32 pdiv,
    uint32 fref,
    uint32 is_bypass,
    uint32 is_nif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_PLL_CFG, &entry_handle_id));
    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_NDIV_INT, INST_SINGLE, ndiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_CH0_MDIV, INST_SINGLE, mdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_KP, INST_SINGLE, 6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_KI, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_PDIV, INST_SINGLE, pdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_STAT_RESET, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_PWRDN, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_FREFEFF_INFO, INST_SINGLE, fref);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_OUTPUT_CML_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_BYPASS_EN, INST_SINGLE, is_bypass);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_NIF_FAB_SEL, INST_SINGLE, is_nif ? 1 : 0);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    sal_usleep(30);

    entry_handle_id = 0;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_PLL_CFG, &entry_handle_id));
    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_RESETB, INST_SINGLE, 1);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Polling the PLL status
     */
    if (!SAL_BOOT_PLISIM)
    {
        if (!is_bypass)
        {
            if (soc_sand_is_emulation_system(unit) == 0)
            {
                /*
                 * Get handle to the DBAL table
                 */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_PLL_STATUS, &entry_handle_id));
                /*
                 * Set KEY field
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);
                SHR_IF_ERR_EXIT(dnxcmn_polling
                                (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_SYNCE_PLL_LOCK,
                                 1));
            }
        }
    }
    /*
     * alloc DBAL table handle again
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYNCE_PLL_CFG, &entry_handle_id));
    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_INDEX, synce_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_POST_RESETB, INST_SINGLE, 1);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function to calculate the parameters for the PLL3.
 *  Used to calculate the dividers, multipliers and effective reference
 *  frequency for the initialization of PLL3 according to given reference clocks
 * \param [in] unit -
 *   The unit number.
 * \param [in] ref_clock_in -
 *   The input reference clock according to SOC property.
 * \param [in] ref_clock_out -
 *   The output reference clock according to SOC property.
 * \param [out] ndiv -
 *   The N divider.
 * \param [out] mdiv -
 *   The M divider(for channel 0).
 * \param [out] pdiv -
 *   The Pre divider.
 * \param [out] fref -
 *   The effective reference frequency.
 * \param [in] is_bypass -
 *   Indicates if is Bypass mode.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
soc_dnx_pll_clk_div_get(
    int unit,
    DNX_SERDES_REF_CLOCK_TYPE ref_clock_in,
    DNX_SERDES_REF_CLOCK_TYPE ref_clock_out,
    uint32 *ndiv,
    uint32 *mdiv,
    uint32 *pdiv,
    uint32 *fref,
    uint32 is_bypass)
{
    SHR_FUNC_INIT_VARS(unit);
    {
        if (ref_clock_in == DNX_SERDES_REF_CLOCK_125)
        {
            *pdiv = 1;
            *fref = dnx_data_pll.pll3.ref_clock_125_get(unit) / (*pdiv);
            /*
             * The number 2 in the calculation of the N divider is a constant from HW
             */
            *ndiv = dnx_data_pll.pll3.vco_clock_get(unit) / ((*fref) * 2);
        }
        else if (ref_clock_in == DNX_SERDES_REF_CLOCK_156_25)
        {
            *pdiv = 1;
            *fref = dnx_data_pll.pll3.ref_clock_156_25_get(unit) / (*pdiv);
            /*
             * The number 2 in the calculation of the N divider is a constant from HW
             */
            *ndiv = dnx_data_pll.pll3.vco_clock_get(unit) / ((*fref) * 2);
            /*
             * Special case for 156t156byp - the effective reference frequency needs to be 125MHz
             */
            if (is_bypass)
            {
                *fref = dnx_data_pll.pll3.ref_clock_125_get(unit) / (*pdiv);
            }

        }
        else if (ref_clock_in == DNX_SERDES_REF_CLOCK_312_5)
        {
            *pdiv = 2;
            *fref = dnx_data_pll.pll3.ref_clock_312_5_get(unit) / (*pdiv);
            /*
             * The number 2 in the calculation of the N divider is a constant from HW
             */
            *ndiv = dnx_data_pll.pll3.vco_clock_get(unit) / ((*fref) * 2);

        }
        else
        {
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
    }
    /*
     * If the device is in bypass mode the In frequency is directly mapped to the Out frequency
     */
    if (is_bypass)
    {
        ref_clock_out = ref_clock_in;
    }
    {
        if (ref_clock_out == DNX_SERDES_REF_CLOCK_125)
        {
            *mdiv = dnx_data_pll.pll3.vco_clock_get(unit) / (dnx_data_pll.pll3.ref_clock_125_get(unit));
        }
        else if (ref_clock_out == DNX_SERDES_REF_CLOCK_156_25)
        {
            *mdiv = dnx_data_pll.pll3.vco_clock_get(unit) / (dnx_data_pll.pll3.ref_clock_156_25_get(unit));
        }
        else if (ref_clock_out == DNX_SERDES_REF_CLOCK_312_5)
        {
            *mdiv = dnx_data_pll.pll3.vco_clock_get(unit) / (dnx_data_pll.pll3.ref_clock_312_5_get(unit));
        }
        else
        {
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ndiv, pdiv and mdiv value for PLL3.
 * 
 * \param [in] unit - Unit ID
 * \param [in] pll_type - PLL type \n
 * \param [out] ndiv - Ndivider.
 * \param [out] mdiv - Mdivider.
 * \param [out] pdiv - Pdivider.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
soc_dnx_pll_3_div_get(
    int unit,
    DNX_PLL3_TYPE pll_type,
    uint32 *ndiv,
    uint32 *mdiv,
    uint32 *pdiv)
{
    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL3_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL3_ID,
                               dnx_data_pll.pll3.config_get(unit, pll_type)->pll3_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PDIV, INST_SINGLE, pdiv);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NDIV_INT, INST_SINGLE, ndiv);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CH_0_MDIV, INST_SINGLE, mdiv);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  PLL3 init function.
 *  Used to init NIF and Fabric PLL
 * \param [in] unit -
 *   The unit number.
 * \param [in] pll_type -
 *   The type of PLL to be initialized - NIF0/1 or Fabric0/1.
 * \param [in] ref_clock_in -
 *   The input reference clock accrding to SOC property.
 * \param [in] ref_clock_out -
 *   The output reference clock accrding to SOC property.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
soc_dnx_pll_3_init(
    int unit,
    DNX_PLL3_TYPE pll_type,
    DNX_SERDES_REF_CLOCK_TYPE ref_clock_in,
    DNX_SERDES_REF_CLOCK_TYPE ref_clock_out)
{
    uint32 ndiv = 0, mdiv = 0, fref = 0, pdiv = 0, is_bypass = 0;

    SHR_FUNC_INIT_VARS(unit);

    is_bypass = (ref_clock_out == DNX_SERDES_REF_CLOCK_BYPASS) ? 1 : 0;
    SHR_IF_ERR_EXIT(soc_dnx_pll_clk_div_get(unit, ref_clock_in, ref_clock_out, &ndiv, &mdiv, &pdiv, &fref, is_bypass));
    SHR_IF_ERR_EXIT(soc_dnx_pll_3_set(unit, pll_type, ndiv, mdiv, pdiv, fref, is_bypass));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure TS freq ctrl high
 *
 */
static shr_error_e
soc_dnx_pll_ts_freq_ctrl_high_set(
    int unit,
    int time_stamp_index,
    uint32 val)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (val == -1)
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL_CMIC_TIMESTAMP_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_INDEX, time_stamp_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_FREQ_CTRL_FRAC_HIGH, INST_SINGLE, val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - Configure TS freq ctrl low
 *
 */
static shr_error_e
soc_dnx_pll_ts_freq_ctrl_low_set(
    int unit,
    int time_stamp_index,
    uint32 val)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL_CMIC_TIMESTAMP_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_INDEX, time_stamp_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_FREQ_CTRL_FRAC_LOW, INST_SINGLE, val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable TS counter
 *
 */
static shr_error_e
soc_dnx_pll_ts_counter_enable_set(
    int unit,
    int time_stamp_index,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL_CMIC_TIMESTAMP_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_INDEX, time_stamp_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_COUNTER_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize HW cmic timestamp. 
 *
 * \param [in] unit - The unit number.
 * \param [in] time_stamp_index - The cmic time stamp index (zero or one). 
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
soc_dnx_pll_timestamp_init(
    int unit,
    int time_stamp_index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnx_pll_ts_freq_ctrl_high_set
                    (unit, time_stamp_index, dnx_data_pll.pll1.ts_phase_initial_hi_get(unit)));
    SHR_IF_ERR_EXIT(soc_dnx_pll_ts_freq_ctrl_low_set
                    (unit, time_stamp_index, dnx_data_pll.pll1.ts_phase_initial_lo_get(unit)));
    SHR_IF_ERR_EXIT(soc_dnx_pll_ts_counter_enable_set(unit, time_stamp_index, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
soc_dnx_pll_timestamp_get(
    int unit,
    int time_stamp_index,
    uint64 *timestamp_ns)
{
    uint32 entry_handle_id;
    uint32 low32 = 0;
    uint32 high32 = 0;
    uint64 timestamp_temp = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL_CMIC_TIMESTAMP_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_INDEX, time_stamp_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TS_LOW, INST_SINGLE, &low32);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TS_HIGH, INST_SINGLE, &high32);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** In HW the value is in nanoseconds and is separated into two parts - high and low bits. */
    COMPILER_64_SET(timestamp_temp, high32, low32);
    COMPILER_64_COPY(*timestamp_ns, timestamp_temp);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  HBMC access init function.
 *  Used to init HBMC
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
soc_dnx_hbmc_access_init(
    int unit)
{
    int nof_hbms;
    uint32 dram_bitmap[1];
    int hbm_index;
    SHR_FUNC_INIT_VARS(unit);

    /** this is done to prevent coverity issue of ARRAY_VS_SINGLETON from occuring */
    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    nof_hbms = dnx_data_dram.general_info.max_nof_drams_get(unit);
    SHR_BIT_ITER(dram_bitmap, nof_hbms, hbm_index)
    {
        /** put HBM channels into soft reset */
        SHR_IF_ERR_EXIT(dnx_hbmc_channels_soft_reset_set(unit, hbm_index, 1));
        /** put HBM channels into soft init */
        SHR_IF_ERR_EXIT(dnx_hbmc_channels_soft_init_set(unit, hbm_index, 1));

        /** steps before out of soft reset - basically pll configurations */
        SHR_IF_ERR_EXIT(dnx_hbmc_steps_before_out_of_soft_reset(unit, hbm_index));

        /** take HBM channels out of soft reset */
        SHR_IF_ERR_EXIT(dnx_hbmc_channels_soft_reset_set(unit, hbm_index, 0));
        /** take HBM channels out of soft init */
        SHR_IF_ERR_EXIT(dnx_hbmc_channels_soft_init_set(unit, hbm_index, 0));

        /*
         * The step below are taken from the HBMC init step in order to init them also for the register only access
         * option 
         */
        /** hbmc phy out of reset */
        SHR_IF_ERR_EXIT(dnx_hbmc_phy_out_of_reset(unit, hbm_index));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  PLL init function.
 *  Used to init NIF, Fabric, TimeSync and BroadSync PLL
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
soc_dnx_pll_init(
    int unit)
{
    DNX_PLL4_TYPE pll4_type;
    DNX_PLL3_TYPE pll3_type;
    DNX_PLL1_TYPE pll1_type;
    uint32 in_freq = 0, out_freq = 0;
    SHR_FUNC_INIT_VARS(unit);
#ifdef DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "Not initializing PLL emulation%s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
#endif

    /**
     * Init All PLL3s
     */
    if (dnx_data_pll.pll3.nof_pll3_get(unit) != 0)
    {
        for (pll3_type = DNX_PLL3_TYPE_FIRST; pll3_type < DNX_PLL3_TYPE_COUNT; pll3_type++)
        {
            /**
             * Skip initialization of invalid PLL types
             */
            if (dnx_data_pll.pll3.config_get(unit, pll3_type)->valid == DNX_PLL3_TYPE_INVALID)
            {
                continue;
            }

            /**
             * Get relevant Input and Output frequencies
             */
            switch (pll3_type)
            {
                case DNX_PLL3_TYPE_NIF0:
                    in_freq = dnx_data_pll.pll3.nif_pll_get(unit, 0)->in_freq;
                    out_freq = dnx_data_pll.pll3.nif_pll_get(unit, 0)->out_freq;
                    break;
                case DNX_PLL3_TYPE_NIF1:
                    in_freq = dnx_data_pll.pll3.nif_pll_get(unit, 1)->in_freq;
                    out_freq = dnx_data_pll.pll3.nif_pll_get(unit, 1)->out_freq;
                    break;
                case DNX_PLL3_TYPE_MAS0:
                    in_freq = dnx_data_pll.pll3.fabric_pll_get(unit, 0)->in_freq;
                    out_freq = dnx_data_pll.pll3.fabric_pll_get(unit, 0)->out_freq;
                    break;
                case DNX_PLL3_TYPE_MAS1:
                    in_freq = dnx_data_pll.pll3.fabric_pll_get(unit, 1)->in_freq;
                    out_freq = dnx_data_pll.pll3.fabric_pll_get(unit, 1)->out_freq;
                    break;
                default:
                     /* coverity[Dead default in switch::FALSE]  */
                    break;
            }

            /**
             * configure the PLL
             */
            if ((in_freq != DNX_SERDES_REF_CLOCK_DISABLE) && (out_freq != DNX_SERDES_REF_CLOCK_DISABLE))
            {
                SHR_IF_ERR_EXIT(soc_dnx_pll_3_init(unit, pll3_type, in_freq, out_freq));
            }
        }
    }

    /**
     * Init All PLL1s
     */
    if (dnx_data_pll.pll1.nof_pll1_get(unit) != 0)
    {
        for (pll1_type = DNX_PLL1_TYPE_FIRST; pll1_type < DNX_PLL1_TYPE_COUNT; pll1_type++)
        {
            /**
             * Skip initialization of invalid PLL types
             */
            if (dnx_data_pll.pll1.config_get(unit, pll1_type)->valid == DNX_PLL1_TYPE_INVALID)
            {
                continue;
            }

            /**
             * Get relevant Input and Output frequencies
             */
            switch (pll1_type)
            {
                case DNX_PLL1_TYPE_TS:
                    /**
                     * Check TS SOC property, if disabled skip initialization
                     */
                    if (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_ts_freq_lock) == 0)
                    {
                        continue;
                    }
                    break;
                case DNX_PLL1_TYPE_BS:
                    /**
                     * Check BS SOC property, if disabled skip initialization
                     */
                    if (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_bs_enable) == 0)
                    {
                        continue;
                    }
                    break;
                default:
                    break;
            }

            /**
             * Configure the PLL
             */
            SHR_IF_ERR_EXIT(soc_dnx_pll_1_init(unit, pll1_type));

        }
    }

    /**
     * Init All PLL4s
     */
    if (dnx_data_pll.pll4.nof_pll4_get(unit) != 0)
    {
        for (pll4_type = DNX_PLL4_TYPE_FIRST; pll4_type < DNX_PLL4_TYPE_COUNT; pll4_type++)
        {
            /**
             * Skip initialization of invalid PLL types
             */
            if (dnx_data_pll.pll4.config_get(unit, pll4_type)->valid == DNX_PLL4_TYPE_INVALID)
            {
                continue;
            }

            switch (pll4_type)
            {
                case DNX_PLL4_TYPE_TS:
                    /**
                     * Check TS SOC property, if disabled skip initialization
                     */
                    if (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_ts_freq_lock) == 0)
                    {
                        continue;
                    }
                    break;
                case DNX_PLL4_TYPE_BS:
                    /**
                     * Check BS SOC property, if disabled skip initialization
                     */
                    if (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_bs_enable) == 0)
                    {
                        continue;
                    }
                    break;
                default:
                    break;
            }

            /**
             * Configure the PLL4
             */
            SHR_IF_ERR_EXIT(soc_dnx_pll_4_init(unit, pll4_type));

        }
    }

    /**
     * TimeStamp enable
     */
    if (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_ts_freq_lock) != 0)
    {
        SHR_IF_ERR_EXIT(soc_dnx_pll_timestamp_init(unit, 1));
    }

    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(soc_dnx_hbmc_access_init(unit));
    }

    if (dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_is_supported))
    {
        SHR_IF_ERR_EXIT(soc_dnx_gddr6_access_init(unit));
    }

    /*
     * Needed FMAC Soft Reset after the PLL configuration for J2C because of the FAB PLL 
     */
    if (SOC_IS_J2C(unit))
    {
        soc_reg_above_64_val_t field_val;
        soc_reg_above_64_val_t reg_above_64_val;
        SOC_REG_ABOVE_64_CLEAR(field_val);
        field_val[0] = 1;
        /*
         * FMAC instances IN reset 
         */
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_0f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_1f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_2f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_3f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_4f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_5f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_6f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_7f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_8f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_9f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_10f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_11f,
                                   field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));

        /*
         * FMAC instance OUT of reset 
         */
        SOC_REG_ABOVE_64_CLEAR(field_val);
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_0f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_1f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_2f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_3f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_4f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_5f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_6f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_7f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_8f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_9f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_10f,
                                   field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_FMAC_11f,
                                   field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  PLL init function.
 *  Used to init NIF, Fabric, TimeSync and BroadSync PLL1
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
soc_dnx_pll1_init(
    int unit)
{
    DNX_PLL1_TYPE pll1_type;
    SHR_FUNC_INIT_VARS(unit);
#ifdef DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "Not initializing PLL emulation%s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
#endif

    /**
     * Init All PLL1s
     */
    if (dnx_data_pll.pll1.nof_pll1_get(unit) != 0)
    {
        for (pll1_type = DNX_PLL1_TYPE_FIRST; pll1_type < DNX_PLL1_TYPE_COUNT; pll1_type++)
        {
            /**
             * Skip initialization of invalid PLL types
             */
            if (dnx_data_pll.pll1.config_get(unit, pll1_type)->valid == DNX_PLL1_TYPE_INVALID)
            {
                continue;
            }

            /**
             * Get relevant Input and Output frequencies
             */
            switch (pll1_type)
            {
                case DNX_PLL1_TYPE_TS:
                    /**
                     * Check TS SOC property, if disabled skip initialization
                     */
                    if (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_ts_freq_lock) == 0)
                    {
                        continue;
                    }
                    break;
                case DNX_PLL1_TYPE_BS:
                    /**
                     * Check BS SOC property, if disabled skip initialization
                     */
                    if (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_bs_enable) == 0)
                    {
                        continue;
                    }
                    break;
                default:
                    break;
            }

            /**
             * Configure the PLL
             */
            SHR_IF_ERR_EXIT(soc_dnx_pll_1_init(unit, pll1_type));

        }
    }

exit:
    SHR_FUNC_EXIT;
}
