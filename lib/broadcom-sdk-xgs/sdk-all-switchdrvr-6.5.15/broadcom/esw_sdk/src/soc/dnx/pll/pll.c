/*
 * $Id: dnx_pll_init.c, v1 09/06/2017 09:55:39 veselag $
 *
 * $Copyright: (c) 2018 Broadcom.
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
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
/* BCM DNX includes */
#include <bcm_int/dnx/cmn/dnxcmn.h>
/* Legacy includes */
#include <soc/dnx/legacy/drv.h>
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
 *  Used to calculate and set the parameters to HW for TS and BS PLL
 * \param [in] unit -
 *   The unit number.
 * \param [in] pll_type -
 *   The type of PLL to be initialized - TS or BS.
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
    DNX_PLL_TYPE pll_type)
{
    uint32 entry_handle_id, key = -1, mdiv = -1, refclk_source_sel = -1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Set key for the DBAL table and M divider for channel 0 anf reference clock according to PLL type
     */
    switch (pll_type)
    {
        case DNX_PLL_TYPE_TS:
            key = 0;
            mdiv = dnx_data_pll.general.pll1_ts_ch_0_mdiv_get(unit);
            refclk_source_sel = 0;      
            break;
        case DNX_PLL_TYPE_BS:
            key = 1;
            mdiv = dnx_data_pll.general.pll1_bs_ch_0_mdiv_get(unit);
            refclk_source_sel = dnx_data_pll.general.pll1_bs_refclk_source_sel_get(unit);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Unsupported PLL type %d\n", pll_type);
    }
    /*
     * Get handle to the configuration DBAL table for PLL1
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL1_CFG, &entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL1_ID, key);
    /*
     * Set hardcoded values for the fields that depend on the PLL type
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NDIV_INT, INST_SINGLE,
                                 dnx_data_pll.general.pll1_ndiv_int_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_0_MDIV, INST_SINGLE, mdiv);
    if (pll_type == DNX_PLL_TYPE_TS)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_1_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.pll1_ch_1_mdiv_get(unit));
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDIV, INST_SINGLE,
                                 dnx_data_pll.general.pll1_pdiv_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTPUT_CML_EN, INST_SINGLE,
                                 dnx_data_pll.general.pll1_output_cml_en_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REFCLK_SOURCE_SEL, INST_SINGLE, refclk_source_sel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FREFEFF_INFO, INST_SINGLE,
                                 dnx_data_pll.general.pll1_frefeff_get(unit));
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Get handle to the configuration DBAL table for PLL1 again for reset
     */
    sal_usleep(30);
    entry_handle_id = 0;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL1_CFG, &entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL1_ID, key);
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
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL1_STATUS, &entry_handle_id));
        /*
         * Set KEY field
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL1_ID, (uint32) key);
        SHR_IF_ERR_EXIT(dnxcmn_polling
                        (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_LOCK_STATUS, 1));
    }
#endif

    /*
     * Get handle to the configuration DBAL table for PLL1 again for post-reset
     */
    entry_handle_id = 0;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL1_CFG, &entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL1_ID, key);
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
 *  Used to set the parameters to HW for NIF and Fabric PLL
 * \param [in] unit -
 *   The unit number.
 * \param [in] pll_type -
 *   The type of PLL to be initialized - NIF0/1 or Fabric0/1.
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
    DNX_PLL_TYPE pll_type,
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
     * Set values for bypass
     */
    if (is_bypass)
    {
        route_ctrl = dnx_data_pll.general.pll3_route_ctr_byp_get(unit);
        en_ctrl = dnx_data_pll.general.pll3_en_ctrl_byp_get(unit);
    }
    else
    {
        en_ctrl = dnx_data_pll.general.pll3_en_ctrl_get(unit);
    }

    /*
     * Get handle to the configuration DBAL table for PLL3
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL3_CFG, &entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL3_ID, (uint32) pll_type);
    /*
     * Set hardcoded values for the common fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KP, INST_SINGLE,
                                 dnx_data_pll.general.pll3_kp_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KI, INST_SINGLE,
                                 dnx_data_pll.general.pll3_ki_get(unit));
    if ((pll_type == DNX_PLL_TYPE_NIF_0) || (pll_type == DNX_PLL_TYPE_NIF_1))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_1_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.nif_pll_ch_1_mdiv_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_2_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.nif_pll_ch_2_mdiv_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_3_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.nif_pll_ch_3_mdiv_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_4_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.nif_pll_ch_4_mdiv_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_5_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.nif_pll_ch_5_mdiv_get(unit));
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_1_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.fabric_pll_ch_1_mdiv_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_2_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.fabric_pll_ch_2_mdiv_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_3_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.fabric_pll_ch_3_mdiv_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_4_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.fabric_pll_ch_4_mdiv_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_5_MDIV, INST_SINGLE,
                                     dnx_data_pll.general.fabric_pll_ch_5_mdiv_get(unit));
    }
    /*
     * Set hardcoded values for the fields that depend on the PLL type, reference and output fequencies chosen
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDIV, INST_SINGLE, pdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NDIV_INT, INST_SINGLE, ndiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CH_0_MDIV, INST_SINGLE, mdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EN_CTRL, INST_SINGLE, en_ctrl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROUTE_CTRL, INST_SINGLE, route_ctrl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FREFEFF_INFO, INST_SINGLE, fref);
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Get handle to the configuration DBAL table for PLL3 again for reset
     */
    sal_usleep(40);
    entry_handle_id = 0;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL3_CFG, &entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL3_ID, (uint32) pll_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_B, INST_SINGLE, 1);
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

#ifndef CMODEL_SERVER_MODE
    /*
     * Polling on pll_locked
     */
    if (!is_bypass)
    {
        if (soc_sand_is_emulation_system(unit) == 0)
        {
            /*
             * Get handle to the DBAL table
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL3_STATUS, &entry_handle_id));
            /*
             * Set KEY field
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL3_ID, (uint32) pll_type);
            SHR_IF_ERR_EXIT(dnxcmn_polling
                            (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_LOCK_STATUS, 1));
        }
    }
#endif

    /*
     * Get handle to the configuration DBAL table for PLL3 again for post-reset
     */
    entry_handle_id = 0;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL3_CFG, &entry_handle_id));
    /*
     * Set KEY field according to the PLL type
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL3_ID, (uint32) pll_type);
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
 *   SyncE PLL initializaiton function
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
    uint32 is_bypass)
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
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYNCE_PLL_NIF_FAB_SEL, INST_SINGLE,
                                 is_bypass ? 0 : 1);
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
 *   The input reference clock accrding to SOC property.
 * \param [in] ref_clock_out -
 *   The output reference clock accrding to SOC property.
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

    if (ref_clock_in == DNX_SERDES_REF_CLOCK_125)
    {
        *pdiv = 1;
        *fref = dnx_data_pll.general.pll3_ref_clock_125_get(unit) / (*pdiv);
        /*
         * The number 2 in the calculation of the N divider is a constant from HW
         */
        *ndiv = dnx_data_pll.general.pll3_vco_clock_get(unit) / ((*fref) * 2);
    }
    else if (ref_clock_in == DNX_SERDES_REF_CLOCK_156_25)
    {
        *pdiv = 1;
        *fref = dnx_data_pll.general.pll3_ref_clock_156_25_get(unit) / (*pdiv);
        /*
         * The number 2 in the calculation of the N divider is a constant from HW
         */
        *ndiv = dnx_data_pll.general.pll3_vco_clock_get(unit) / ((*fref) * 2);
        /*
         * Special case for 156t156byp - the effective reference frequency needs to be 125MHz
         */
        if (is_bypass)
        {
            *fref = dnx_data_pll.general.pll3_ref_clock_125_get(unit) / (*pdiv);
        }

    }
    else if (ref_clock_in == DNX_SERDES_REF_CLOCK_312_5)
    {
        *pdiv = 2;
        *fref = dnx_data_pll.general.pll3_ref_clock_312_5_get(unit) / (*pdiv);
        /*
         * The number 2 in the calculation of the N divider is a constant from HW
         */
        *ndiv = dnx_data_pll.general.pll3_vco_clock_get(unit) / ((*fref) * 2);

    }
    else
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    /*
     * If the device is in bypass mode the In frequency is directly mapped to the Out frequency
     */
    if (is_bypass)
    {
        ref_clock_out = ref_clock_in;
    }

    if (ref_clock_out == DNX_SERDES_REF_CLOCK_125)
    {
        *mdiv = dnx_data_pll.general.pll3_vco_clock_get(unit) / (dnx_data_pll.general.pll3_ref_clock_125_get(unit));
    }
    else if (ref_clock_out == DNX_SERDES_REF_CLOCK_156_25)
    {
        *mdiv = dnx_data_pll.general.pll3_vco_clock_get(unit) / (dnx_data_pll.general.pll3_ref_clock_156_25_get(unit));
    }
    else if (ref_clock_out == DNX_SERDES_REF_CLOCK_312_5)
    {
        *mdiv = dnx_data_pll.general.pll3_vco_clock_get(unit) / (dnx_data_pll.general.pll3_ref_clock_312_5_get(unit));
    }
    else
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
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
    DNX_PLL_TYPE pll_type,
    uint32 *ndiv,
    uint32 *mdiv,
    uint32 *pdiv)
{
    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL3_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PLL3_ID, pll_type);

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
    DNX_PLL_TYPE pll_type,
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
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

/*
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL_CMIC_TIMESTAMP_CONFIG_SEL, &entry_handle_id));
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_USE_COMMON_CTRL, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PLL_CMIC_TIMESTAMP_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_INDEX, time_stamp_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_FREQ_CTRL_FRAC, INST_SINGLE,
                                 dnx_data_pll.general.ts_phase_initial_lo_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_COUNTER_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

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
    nof_hbms = dnx_data_dram.hbm.nof_hbms_get(unit);
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
    SHR_FUNC_INIT_VARS(unit);
#ifdef DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "Not initializing PLL emulation%s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
#endif

    /*
     * NIF
     */
    if (dnx_data_pll.general.nif_pll_cfg_get(unit, 0)->in_freq != DNX_SERDES_REF_CLOCK_DISABLE
        && dnx_data_pll.general.nif_pll_cfg_get(unit, 0)->out_freq != DNX_SERDES_REF_CLOCK_DISABLE)
    {
        SHR_IF_ERR_EXIT(soc_dnx_pll_3_init
                        (unit, DNX_PLL_TYPE_NIF_0, dnx_data_pll.general.nif_pll_cfg_get(unit, 0)->in_freq,
                         dnx_data_pll.general.nif_pll_cfg_get(unit, 0)->out_freq));
    }
    if (dnx_data_pll.general.nif_pll_cfg_get(unit, 1)->in_freq != DNX_SERDES_REF_CLOCK_DISABLE
        && dnx_data_pll.general.nif_pll_cfg_get(unit, 1)->out_freq != DNX_SERDES_REF_CLOCK_DISABLE)
    {
        SHR_IF_ERR_EXIT(soc_dnx_pll_3_init
                        (unit, DNX_PLL_TYPE_NIF_1, dnx_data_pll.general.nif_pll_cfg_get(unit, 1)->in_freq,
                         dnx_data_pll.general.nif_pll_cfg_get(unit, 1)->out_freq));
    }

    /*
     * Fabric
     */
    if (dnx_data_pll.general.fabric_pll_cfg_get(unit, 0)->in_freq != DNX_SERDES_REF_CLOCK_DISABLE
        && dnx_data_pll.general.fabric_pll_cfg_get(unit, 0)->out_freq != DNX_SERDES_REF_CLOCK_DISABLE)
    {
        SHR_IF_ERR_EXIT(soc_dnx_pll_3_init
                        (unit, DNX_PLL_TYPE_FABRIC_0, dnx_data_pll.general.fabric_pll_cfg_get(unit, 0)->in_freq,
                         dnx_data_pll.general.fabric_pll_cfg_get(unit, 0)->out_freq));
    }
    if (dnx_data_pll.general.fabric_pll_cfg_get(unit, 1)->in_freq != DNX_SERDES_REF_CLOCK_DISABLE
        && dnx_data_pll.general.fabric_pll_cfg_get(unit, 1)->out_freq != DNX_SERDES_REF_CLOCK_DISABLE)
    {
        SHR_IF_ERR_EXIT(soc_dnx_pll_3_init
                        (unit, DNX_PLL_TYPE_FABRIC_1, dnx_data_pll.general.fabric_pll_cfg_get(unit, 1)->in_freq,
                         dnx_data_pll.general.fabric_pll_cfg_get(unit, 1)->out_freq));
    }

    /*
     * TS
     */
    if (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_ts_freq_lock) != 0)
    {
        SHR_IF_ERR_EXIT(soc_dnx_pll_1_init(unit, DNX_PLL_TYPE_TS));
    }

    /*
     * BS
     */
    if (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_bs_enable) != 0)
    {
        SHR_IF_ERR_EXIT(soc_dnx_pll_1_init(unit, DNX_PLL_TYPE_BS));
    }

    /*
     * TimeStamp enable
     */
    if (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_ts_freq_lock) != 0)
    {
        SHR_IF_ERR_EXIT(soc_dnx_pll_timestamp_init(unit, 1));
    }

    SHR_IF_ERR_EXIT(soc_dnx_hbmc_access_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  PLL deinit function.
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
soc_dnx_pll_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Currently nothing to deinit
     */

    SHR_FUNC_EXIT;
}
