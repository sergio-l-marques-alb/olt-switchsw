/** \file sat_gtf.c
 * $Id$
 *
 * SAT procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SAT

 /*
  * Include files.
  * {
  */
#include <bcm/sat.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/types.h>
#include <shared/bsl.h>
#include <bcm_int/dnx/sat/sat.h>

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_sat_access.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** Index of LSB in MAC */
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

/**
* \brief
* To verify SAT gtf create for BCM-API: bcm_sat_gtf_create*() .
* \par
* \param [in] unit  - Relevant unit.
* \param [in] gtf_id  - GTF id
* \param [in] flags  - Relevant flags
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_create_verify(
    int unit,
    uint32 flags,
    bcm_sat_gtf_t * gtf_id)
{
    uint8 is_allocated = 0;
    int legal_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(gtf_id, _SHR_E_PARAM, "gtf_id ");

    legal_flags |= BCM_SAT_GTF_WITH_ID;
    if (flags & ~legal_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: unsupported flag combination\n");
    }

    /** check allocation if BCM_SAT_GTF_WITH_ID is present */
    if (flags & BCM_SAT_GTF_WITH_ID)
    {
        SHR_RANGE_VERIFY(*gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
        SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, *gtf_id, &is_allocated));
    }
    /** if gtf ID already exists, return error*/
    if (is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "Error: GTF with id %d exists\n", *gtf_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Create GTF id.
* \param [in] unit  -
*   Relevant unit.
* \param [in] flags  -
*   Relevant flags
* \param [in] gtf_id  -
*    GTF id
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_create(
    int unit,
    uint32 flags,
    bcm_sat_gtf_t * gtf_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_create_verify(unit, flags, gtf_id));
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) flags(%d) gtf_id(%d)\n", BSL_FUNC, BSL_LINE, unit, flags, (*gtf_id)));

    SHR_IF_ERR_EXIT(dnx_sat_gtf_id_allocate(unit, flags, gtf_id));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Set GTF packet send or not.
* \par
* \param [in] unit - Relevant unit.
* \param [in] gtf_id -   gtf_id .
* \param [in] enable -  enable or disable.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_packet_gen_set(
    int unit,
    bcm_sat_gtf_t gtf_id,
    int enable)
{
    uint32 entry_handle_id;
    uint32 rate_pattern_mode = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_RATE_CONTROL, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BURST_MODE, INST_SINGLE, &rate_pattern_mode);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_ENABLE, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PKT_GEN_ENABLE, INST_SINGLE, enable);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * combined mode need to reset the ITER CNT
     */
    if (rate_pattern_mode == 1)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_RATE_CONTROL, &entry_handle_id));
        /** setting key fields */
        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);
        /** setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_ITER_CNT_RESET, INST_SINGLE, 1);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_RATE_CONTROL, &entry_handle_id));
        /** setting key fields */
        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);
        /** setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_ITER_CNT_RESET, INST_SINGLE, 0);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
* \brief
* To reset SAT timer config to initial value.
* \par
 * \param [in] unit -Relevant unit.
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
shr_error_e
dnx_sat_reset_timer_config(
    int unit)
{
    uint32 freq_hz = 0;
    uint32 clocks_per_cycle = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Reset Time tickets
     */
    freq_hz = dnx_data_device.general.core_clock_khz_get(unit) * 1000;  /* Khz*1000 */
    clocks_per_cycle = (freq_hz / (DNX_DATA_MAX_SAT_GENERAL_GRANULARITY / DNX_SAT_BITS_PER_BYTE));      /* 1kbits per
                                                                                                         * sec(125
                                                                                                         * cycles per
                                                                                                         * sec) */
    if (clocks_per_cycle == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Frequency is error \n");
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TIMER_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_CLKS_IN_CYCLE, INST_SINGLE, clocks_per_cycle);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify SAT gtf destroy for BCM-API: bcm_sat_gtf_destroy*() .
* \par
 * \param [in] unit -Relevant unit.
* \param [in] gtf_id  - GTF id
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_destroy_verify(
    int unit,
    bcm_sat_gtf_t gtf_id)
{
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_RANGE_VERIFY(gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");

    /** check gtf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_allocated));

    /** return error if the gtf_iddoes not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: gtf_id  %d does not exist\n", gtf_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Clear gtf packet header in HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] header_id -  header id.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_packet_header_config_destroy(
    int unit,
    int header_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * For writing on dynamic tables, have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_PKT_HEADER_CONFIG, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_SAT_PACKET_HEADER_INDEX, header_id);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Destroy GTF id.
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF id
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_destroy(
    int unit,
    bcm_sat_gtf_t gtf_id)
{
    int pkt_gen_en = 0;
    dnx_sat_data_t psat_data;
    dnx_sat_gtf_pkt_hdr_tbl_info_t header_tbl_data;
    int idx = 0;
    int found = 0;
    int old_profile = 0;
    int new_profile = 0;
    int is_allocated = 0;
    int is_last = 0;
    int nof_allocate_gtf = 0;
    uint32 header_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_destroy_verify(unit, gtf_id));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d)\n", BSL_FUNC, BSL_LINE, unit, gtf_id));

    /*
     * Disable Generate Packet
     */
    pkt_gen_en = 0;
    SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_gen_set(unit, gtf_id, pkt_gen_en));

    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));

    header_id = psat_data.gtf_header_id[gtf_id];
    /*
     * clear current gtf header id
     */
    psat_data.gtf_header_id[gtf_id] = 0;

    /*
     * check if any other GTF are using
     */
    for (idx = 0; idx <= DNX_SAT_GTF_ID_MAX; idx++)
    {
        if (psat_data.gtf_header_id[idx] == header_id)
        {
            /*
             * find it, can't delete profile
             */
            found = 1;
            break;
        }
    }
    /*
     * no other gtf are using the profile, delete it
     */
    if (!found)
    {
        sal_memset(&header_tbl_data, 0, sizeof(header_tbl_data));
        old_profile = header_id;
        SHR_IF_ERR_EXIT(dnx_am_template_sat_pkt_header_exchange
                        (0, 0, &header_tbl_data, old_profile, &is_last, &new_profile, &is_allocated));
        psat_data.gtf_header_id[gtf_id] = new_profile;
        if (is_last)
        {
            SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_header_config_destroy(unit, old_profile - 1));
        }
    }

    SHR_IF_ERR_EXIT(dnx_sat_gtf_id_free(unit, gtf_id));
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.nof_allocated_elements_in_range_get(unit, 0, DNX_SAT_GTF_ID_MAX,
                                                                                         &nof_allocate_gtf));

    if (nof_allocate_gtf == 0)
    {
        /*
         * if no gtf, init rate mode
         */
        psat_data.rate_mode = 0;
        /*
         * if no gtf, init granularity flag
         */
        psat_data.granularity_flag = 0;
        dnx_sat_reset_timer_config(0);
    }

    SHR_IF_ERR_EXIT(dnx_sat_data_set(unit, &psat_data));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Destroy All of GTF id.
* \param [in] unit  -
*   Relevant unit.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_destroy_all(
    int unit)
{
    int pkt_gen_en = 0;
    dnx_sat_data_t psat_data;
    bcm_sat_gtf_t gtf_id;
    int is_last = 0;
    dnx_sat_gtf_pkt_hdr_tbl_info_t header_tbl_data;
    int old_profile = 0;
    int new_profile = 0;
    int is_header_id_allocated = 0;
    uint8 is_gtf_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d)\n", BSL_FUNC, BSL_LINE, unit));
    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));

    /*
     * Disable Generate Packet
     */
    pkt_gen_en = 0;
    for (gtf_id = DNX_SAT_GTF_ID_MIN; gtf_id <= DNX_SAT_GTF_ID_MAX; gtf_id++)
    {
        /** check gtf_id is allocated/exsits */
        SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_gtf_allocated));
        if (is_gtf_allocated)
        {
            /*
             * Disable Generate Packet
             */
            SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_gen_set(unit, gtf_id, pkt_gen_en));

            /*
             * Delet all tx packet header profile
             */
            old_profile = psat_data.gtf_header_id[gtf_id];
            sal_memset(&header_tbl_data, 0, sizeof(header_tbl_data));
            SHR_IF_ERR_EXIT(dnx_am_template_sat_pkt_header_exchange
                            (0, 0, &header_tbl_data, old_profile, &is_last, &new_profile, &is_header_id_allocated));
            psat_data.gtf_header_id[gtf_id] = new_profile;

            if (is_last)
            {
                SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_header_config_destroy(unit, old_profile - 1));
            }
            SHR_IF_ERR_EXIT(dnx_sat_gtf_id_free(unit, gtf_id));
        }
    }

    /*
     * if no gtf, init rate mode
     */
    psat_data.rate_mode = 0;
    /*
     * if no gtf, init granularity flag
     */
    psat_data.granularity_flag = 0;
    SHR_IF_ERR_EXIT(dnx_sat_data_set(unit, &psat_data));
    dnx_sat_reset_timer_config(0);

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*     Gtf rate convert from internal.
* \par
* \param [in] unit - Relevant unit.
* \param [in] rate_mnt_val  -  rate in MANTISSA.
* \param [in] rate_exp_val - rate in EXPONENT.
* \param [in] flags -  flags.
* \param [out] rate -  internal rate .
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_rate_from_internal_rate_convert(
    int unit,
    uint32 rate_mnt_val,
    uint32 rate_exp_val,
    uint32 flags,
    uint32 *rate)
{
    uint32 entry_handle_id;
    uint32 bytes_per_cycle = 0;
    uint32 cycles_per_sec = 0;
    uint32 clocks_per_sec = 0;
    uint32 clocks_per_cycle = 0;
    uint64 rate64bits;
    uint32 rate_div;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (rate == NULL)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    /*
     * 1. caculate cycle per sec
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TIMER_CONFIG, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NUM_CLOCKS_SEC, INST_SINGLE, &clocks_per_sec);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NUM_CLKS_IN_CYCLE, INST_SINGLE, &clocks_per_cycle);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (clocks_per_cycle != 0)
    {
        cycles_per_sec = clocks_per_sec / clocks_per_cycle;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Frequency is error \n");
    }
    /*
     * 2. caculate bytes per cycle, given rate(Units: kbps)
     "x = eq_const_multi * (eq_const_mnt_inc + mnt) * 2^exp".
     */
    SHR_IF_ERR_EXIT(utilex_compute_complex_to_mnt_exp(rate_mnt_val, rate_exp_val, 1, 0, &bytes_per_cycle));

    if (flags)
    {
        *rate = bytes_per_cycle;
    }
    else
    {
        /*
         * 3. get rate(Units: kbps) by bytes_per_cycle and cycles_per_sec:
         rate is Kbps, so rate change to bytes should *1000 / 8bits = 125,  and vice versa.
         */
        /*
         * this calculation using 64bit macros as the multiplication exceeds 32bits
         */
        rate_div = 125;
        COMPILER_64_ZERO(rate64bits);
        COMPILER_64_ADD_32(rate64bits, bytes_per_cycle);
        COMPILER_64_UMUL_32(rate64bits, cycles_per_sec);
        COMPILER_64_UDIV_32(rate64bits, rate_div);
        *rate = COMPILER_64_LO(rate64bits);
    }
    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Gtf rate convert to  internal.
* \par
* \param [in] unit - Relevant unit.
* \param [in] rate -   rate .
* \param [in] flags -  flags.
* \param [in] mnt_nof_bits  - length of BYTES_PER_CYCLE_MNT.
* \param [in] exp_nof_bits - length of BYTES_PER_CYCLE_EXP.
* \param [in] granularity -  granularity.
* \param [out] rate_mnt_val  - value of BYTES_PER_CYCLE_MNT.
* \param [out] rate_exp_val - value of BYTES_PER_CYCLE_EXP.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_rate_to_internal_rate_convert(
    int unit,
    uint32 rate,
    uint32 flags,
    uint32 mnt_nof_bits,
    uint32 exp_nof_bits,
    uint32 granularity,
    uint32 *rate_mnt_val,
    uint32 *rate_exp_val)
{
    uint32 entry_handle_id;
    uint32 bytes_per_cycle = 0;
    uint32 clocks_per_sec = 0;
    uint32 clocks_per_cycle = 0;
    uint32 old_clocks_per_cycle = 0;
    int nof_allocate_gtf = 0;
    uint64 rate64bits;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(rate_mnt_val, _SHR_E_PARAM, "rate_mnt_val ");
    SHR_NULL_CHECK(rate_exp_val, _SHR_E_PARAM, "rate_exp_val ");

    COMPILER_64_ZERO(rate64bits);

    if ((flags & BCM_SAT_GTF_RATE_WITH_GRANULARITY) && (granularity == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Parameter granularity can't be zero \n");
    }

    /*
     * 1. caculate cycle per sec
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TIMER_CONFIG, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NUM_CLOCKS_SEC, INST_SINGLE, &clocks_per_sec);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NUM_CLKS_IN_CYCLE, INST_SINGLE, &clocks_per_cycle);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    old_clocks_per_cycle = clocks_per_cycle;

    if (!(flags & BCM_SAT_GTF_RATE_WITH_GRANULARITY) && clocks_per_sec && clocks_per_cycle)
    {
        granularity = clocks_per_sec / clocks_per_cycle;
    }

    if (granularity == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " parameter error !!! \n");
    }

    if (flags & BCM_SAT_GTF_RATE_IN_PACKETS)
    {
        if (flags & BCM_SAT_GTF_RATE_WITH_GRANULARITY)
        {
            clocks_per_cycle = clocks_per_sec / granularity;
            bytes_per_cycle = rate / granularity;
        }
        else
        {
            clocks_per_cycle = clocks_per_sec;
            bytes_per_cycle = rate;
        }
    }
    else
    {
        /*
         * 2. caculate bytes per cycle, given rate(Units: kbps)
         *  rate is Kbps, so rate should *1000 / 8bits = 125
         */
        /*
         * this calculation using 64bit macros as the multiplication exceeds 32bits
         */
        COMPILER_64_SET(rate64bits, 0, rate);   /* rate is 32bit, copy it to a 64bits variable */
        COMPILER_64_UMUL_32(rate64bits, 125);
        COMPILER_64_UDIV_32(rate64bits, granularity);
        bytes_per_cycle = COMPILER_64_LO(rate64bits);
        clocks_per_cycle = clocks_per_sec / granularity;
    }

    if (old_clocks_per_cycle != clocks_per_cycle)
    {
        SHR_IF_ERR_EXIT(algo_sat_db.
                        sat_res_alloc_gtf_id.nof_allocated_elements_in_range_get(unit, 0, DNX_SAT_GTF_ID_MAX,
                                                                                 &nof_allocate_gtf));
        if (nof_allocate_gtf > 1)
        {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit, "Fail(%s), more than one gtf inuse, granularity can't changed.\n"),
                       soc_errmsg(BCM_E_PARAM)));
            return BCM_E_PARAM;
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TIMER_CONFIG, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_CLKS_IN_CYCLE, INST_SINGLE,
                                     clocks_per_cycle);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * 3. caculate interval value for bytes per cycle
     */
    SHR_IF_ERR_EXIT(utilex_break_to_mnt_exp_round_up
                    (bytes_per_cycle, mnt_nof_bits, exp_nof_bits, 0, rate_mnt_val, rate_exp_val));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Get GTF bandwidth from HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] gtf_id -   gtf_id .
* \param [out] bw -  sat gtf bandwidth.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_bandwidth_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_bandwidth_t * bw)
{
    uint32 entry_handle_id;
    uint32 rate_mnt_val = 0, rate_exp_val = 0;
    uint32 burst_mnt_val = 0, burst_exp_val = 0;
    uint32 crdt_per_pkt = 0;
    uint32 complex = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_BANDWIDTH_CONTROL, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);
    /** setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BYTES_PER_CYCLE_MANTISSA, INST_SINGLE, &rate_mnt_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BYTES_PER_CYCLE_EXPONENT, INST_SINGLE, &rate_exp_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_CREDITS_MANTISSA, INST_SINGLE, &burst_mnt_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_CREDITS_EXPONENT, INST_SINGLE, &burst_exp_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDITS_PER_PACKET_EN, INST_SINGLE, &crdt_per_pkt);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    bw->flags = crdt_per_pkt;

    SHR_IF_ERR_EXIT(dnx_sat_gtf_rate_from_internal_rate_convert
                    (unit, rate_mnt_val, rate_exp_val, crdt_per_pkt, &(bw->rate)));

    SHR_IF_ERR_EXIT(utilex_compute_complex_to_mnt_exp(burst_mnt_val, burst_exp_val, 1, 0, &complex));

    /*
     * if rate is Kbps, then change to bytes should *1000 / 8bits = 125, and vice versa.
     */
    bw->max_burst = (crdt_per_pkt) ? complex : (complex / 125);

    SHR_EXIT();

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify SAT gtf bandwidth config for BCM-API: bcm_sat_gtf_bandwidth_get*() .
* \par
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF id
* \param [in] bw  -
*     Configure sat gtf bandwidth
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_bandwidth_get_verify(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_bandwidth_t * bw)
{
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bw, _SHR_E_PARAM, "bw ");
    SHR_RANGE_VERIFY(gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
   /** check gtf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_allocated));

   /** return error if the gtf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: gtf_id  %d does not exist\n", gtf_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  SAT gtf bandwidth get.
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF id
* \param [in] priority  -
*     not use it
* \param [in] bw  -
*     Configure sat gtf bandwidth
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_bandwidth_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    int priority,
    bcm_sat_gtf_bandwidth_t * bw)
{
    bcm_sat_gtf_bandwidth_t dnx_bandwidth;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_bandwidth_get_verify(unit, gtf_id, bw));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d) priority(%d) \n", BSL_FUNC, BSL_LINE,
                                 unit, gtf_id, priority));
    sal_memset(&dnx_bandwidth, 0, sizeof(dnx_bandwidth));
    SHR_IF_ERR_EXIT(dnx_sat_gtf_bandwidth_get(unit, gtf_id, &dnx_bandwidth));

    bw->rate = dnx_bandwidth.rate;
    bw->max_burst = dnx_bandwidth.max_burst;
    bw->flags = dnx_bandwidth.flags;

    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Set GTF bandwidth inHW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] gtf_id -   gtf_id .
* \param [in] bw -  sat gtf bandwidth.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_bandwidth_set(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_bandwidth_t * bw)
{
    uint32 entry_handle_id;
    uint32 mnt_nof_bits = 0, exp_nof_bits = 0;
    uint32 rate_mnt_val = 0, rate_exp_val = 0;
    uint32 burst_mnt_val = 0, burst_exp_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * 1. caculate bytes per cycle for rate
     */
    /*
     * field length of BYTES_PER_CYCLE_MNT and BYTES_PER_CYCLE_EXP
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SAT_TX_BANDWIDTH_CONTROL, DBAL_FIELD_BYTES_PER_CYCLE_MANTISSA, FALSE, 0, 0,
                     (int *) (&mnt_nof_bits)));
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SAT_TX_BANDWIDTH_CONTROL, DBAL_FIELD_BYTES_PER_CYCLE_EXPONENT, FALSE, 0, 0,
                     (int *) (&exp_nof_bits)));

    SHR_IF_ERR_EXIT(dnx_sat_gtf_rate_to_internal_rate_convert
                    (unit, bw->rate, bw->flags, mnt_nof_bits, exp_nof_bits, bw->granularity, &rate_mnt_val,
                     &rate_exp_val));
    /*
     * 2. caculate max credits for max_burst
     */
    /*
     * field length of MAX_CREDITS_MNT and MAX_CREDITS_EXP__Nf
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SAT_TX_BANDWIDTH_CONTROL, DBAL_FIELD_MAX_CREDITS_MANTISSA, FALSE, 0, 0,
                     (int *) (&mnt_nof_bits)));
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SAT_TX_BANDWIDTH_CONTROL, DBAL_FIELD_MAX_CREDITS_EXPONENT, FALSE, 0, 0,
                     (int *) (&exp_nof_bits)));
    if (bw->flags & BCM_SAT_GTF_RATE_IN_PACKETS)
    {
        SHR_IF_ERR_EXIT(utilex_break_to_mnt_exp_round_up
                        (bw->max_burst, mnt_nof_bits, exp_nof_bits, 0, &burst_mnt_val, &burst_exp_val));
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_break_to_mnt_exp_round_up
                        (bw->max_burst * 125, mnt_nof_bits, exp_nof_bits, 0, &burst_mnt_val, &burst_exp_val));
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_BANDWIDTH_CONTROL, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYTES_PER_CYCLE_MANTISSA, INST_SINGLE, rate_mnt_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYTES_PER_CYCLE_EXPONENT, INST_SINGLE, rate_exp_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_CREDITS_MANTISSA, INST_SINGLE, burst_mnt_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_CREDITS_EXPONENT, INST_SINGLE, burst_exp_val);

    if (bw->flags & BCM_SAT_GTF_RATE_IN_PACKETS)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDITS_PER_PACKET_EN, INST_SINGLE, 1);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDITS_PER_PACKET_EN, INST_SINGLE, 0);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify SAT gtf bandwidth config for BCM-API: bcm_sat_gtf_bandwidth_set*() .
* \par
* \param [in] unit  - Relevant unit.
* \param [in] gtf_id  - GTF id
* \param [in] bw  - Configure sat gtf bandwidth
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_bandwidth_set_verify(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_bandwidth_t * bw)
{
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bw, _SHR_E_PARAM, "bw ");
    SHR_RANGE_VERIFY(gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
    if (bw->flags & BCM_SAT_GTF_RATE_IN_PACKETS)
    {
        SHR_MAX_VERIFY(bw->rate, DNX_SAT_GTF_BW_PPS_RATE_MAX, _SHR_E_PARAM, "rate");
    }
    else
    {
        SHR_MAX_VERIFY(bw->rate, DNX_SAT_GTF_BW_RATE_MAX, _SHR_E_PARAM, "rate");
    }
    SHR_MAX_VERIFY(bw->max_burst, DNX_SAT_GTF_BW_BURST_MAX, _SHR_E_PARAM, "max_burst");

    /** check gtf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_allocated));

    /** return error if the gtf_iddoes not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: gtf_id  %d does not exist\n", gtf_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  SAT gtf bandwidth set.
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF id
* \param [in] priority  -
*     not use it
* \param [in] bw  -
*     Configure sat gtf bandwidth
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_bandwidth_set(
    int unit,
    bcm_sat_gtf_t gtf_id,
    int priority,
    bcm_sat_gtf_bandwidth_t * bw)
{
    bcm_sat_gtf_bandwidth_t dnx_bandwidth;
    dnx_sat_data_t psat_data;
    uint8 rate_mode = 0;        /* 1: bps 2:pps */
    uint8 is_granularity_set = 1;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_bandwidth_set_verify(unit, gtf_id, bw));
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d) priority(%d) rate(%d) max_burst(%d)\n", BSL_FUNC, BSL_LINE,
                                 unit, gtf_id, priority, bw->rate, bw->max_burst));

    sal_memset(&dnx_bandwidth, 0, sizeof(dnx_bandwidth));
    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));

    if (bw->flags & BCM_SAT_GTF_RATE_WITH_GRANULARITY)
    {
        /*
         * checking granularity value
         */
        if (bw->granularity == 0)
        {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit, "Fail(%s) granularity shouldn't be zero \n"), soc_errmsg(BCM_E_PARAM)));
            return BCM_E_PARAM;
        }
        /*
         * checking if granularity is set. Granularity is shared by all GTF
         */
        if (is_granularity_set == psat_data.granularity_flag)
        {
            LOG_WARN(BSL_LS_BCM_SAT, (BSL_META_U(unit,
                                                 "Warning : Granularity is shared by all GTFs, which will influce all the GTF  granularity\n")));
        }
        /*
         * setting granularity flag
         */
        psat_data.granularity_flag = 1;
    }

    if (bw->flags & BCM_SAT_GTF_RATE_IN_PACKETS)
    {
        rate_mode = DNX_SAT_PPS_MODE;
    }
    else
    {
        rate_mode = DNX_SAT_BPS_MODE;
    }

    /*
     * fisrt GTF will decide the "global mode" for BPS/PPS 
     */
    if (0 == psat_data.rate_mode)
    {
        psat_data.rate_mode = rate_mode;
        if (rate_mode == DNX_SAT_PPS_MODE)
        {
            /*
             * Default set granularity flag in PPS mode, default granularity is 1 packet/s
             */
            psat_data.granularity_flag = 1;
        }
    }
    else if (rate_mode != psat_data.rate_mode)
    {
        LOG_ERROR(BSL_LS_BCM_SAT, (BSL_META_U(unit,
                                              "Fail(%s)  BPS/PPS mode conflicts with global rate mode (which is decided by first GTF ), you should delete all GTFs before change it\n"),
                                   soc_errmsg(BCM_E_PARAM)));
        return BCM_E_PARAM;
    }

    dnx_bandwidth.rate = bw->rate;
    dnx_bandwidth.max_burst = bw->max_burst;
    dnx_bandwidth.flags = bw->flags;
    dnx_bandwidth.granularity = bw->granularity;

    SHR_IF_ERR_EXIT(dnx_sat_gtf_bandwidth_set(unit, gtf_id, &dnx_bandwidth));

    SHR_IF_ERR_EXIT(dnx_sat_data_set(unit, &psat_data));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify SAT Traverse all GTF id for BCM-API: bcm_sat_gtf_traverse*() .
* \par
* \param [in] unit  - Relevant unit.
* \param [in] user_data  -user data
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_traverse_verify(
    int unit,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(user_data, _SHR_E_PARAM, "user_data ");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Traverse all GTF id.
* \param [in] unit  -
*   Relevant unit.
* \param [in] cb  -
*    Gtf callback function prototype.
* \param [in] user_data  -
*    user data
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_traverse(
    int unit,
    bcm_sat_gtf_traverse_cb cb,
    void *user_data)
{
    bcm_sat_ctf_t gtf_id;
    uint8 is_gtf_allocated;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_traverse_verify(unit, user_data));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) \n", BSL_FUNC, BSL_LINE, unit));

    for (gtf_id = DNX_SAT_GTF_ID_MIN; gtf_id <= DNX_SAT_GTF_ID_MAX; gtf_id++)
    {
        /** check gtf_id is allocated/exsits */
        SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_gtf_allocated));
        if (is_gtf_allocated)
        {
            /*
             * Invoke user callback.
             */
            (*cb) (unit, gtf_id, user_data);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  Dump data in buffer - For debug usage.
*/
void
dnx_sat_packet_config_dump(
    bcm_sat_gtf_packet_config_t * config)
{
    int i = 0, j = 0, ele = 0;
    /*
     * uint8 *buffer; int len;
     */
    if (!LOG_CHECK(BSL_LS_BCM_SAT | BSL_VERBOSE))
    {
        return;
    }
    if (config == NULL)
    {
        return;
    }

    cli_out("\n");
    cli_out("  payload_type:%d\n", config->payload.payload_type);
    cli_out("  payload_pattern:");
    ele = (config->payload.payload_type == bcmSatPayloadConstant8Bytes) ? 8 : 4;
    if (config->payload.payload_type != bcmSatPayloadPRBS)
    {
        for (i = 0; i < ele; i++)
        {
            cli_out("%02x ", config->payload.payload_pattern[i]);
        }
    }
    cli_out("\n");
    for (i = 0; i < BCM_SAT_GTF_NUM_OF_PRIORITIES; i++)
    {
        cli_out("  %s packet_edit:\n", ((i == 0) ? "cir" : "eir"));
        cli_out("    packet_length:");
        for (j = 0; j < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; j++)
        {
            cli_out("%d ", config->packet_edit[i].packet_length[j]);
        }
        cli_out("\n");
        cli_out("    packet_length_pattern:");
        for (j = 0; j < BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH; j++)
        {
            cli_out("%d ", config->packet_edit[i].packet_length_pattern[j]);
        }
        cli_out("\n");
        cli_out("    pattern_length:%d\n", config->packet_edit[i].pattern_length);
        cli_out("    number_of_stamps:%d\n", config->packet_edit[i].number_of_stamps);
        for (j = 0; j < config->packet_edit[i].number_of_stamps; j++)
        {
            cli_out
                ("    stamp(%d):  stamp_type:%d  field_type:%d  inc_step:%d  inc_period_packets:%d  value:%d  offset:%d\n",
                 j, config->packet_edit[i].stamps[j].stamp_type, config->packet_edit[i].stamps[j].field_type,
                 config->packet_edit[i].stamps[j].inc_step, config->packet_edit[i].stamps[j].inc_period_packets,
                 config->packet_edit[i].stamps[j].value, config->packet_edit[i].stamps[j].offset);
        }
        cli_out("    number_of_ctfs:%d\n", config->packet_edit[i].number_of_ctfs);
        cli_out("    flags:%d\n", config->packet_edit[i].flags);
    }
    cli_out("  packet_context_id:%d\n", config->packet_context_id);
    cli_out("  offsets:\n");
    cli_out("    seq_number_offset:%d\n", config->offsets.seq_number_offset);
    cli_out("    timestamp_offset:%d\n", config->offsets.timestamp_offset);
}

/**
* \brief
* To verify gtf stamp information
* \par
* \param [in] unit - Relevant unit.
* \param [out] pkt_edit -  sat packet of gtf information.
* \param [in] stamp_field_info -  stamp information.
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_stamp_field_verify(
    int unit,
    bcm_sat_gtf_packet_edit_t * pkt_edit,
    dnx_sat_gtf_stamp_field_tbl_info_t * stamp_field_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(pkt_edit, _SHR_E_PARAM, "pkt_edit ");
    SHR_NULL_CHECK(stamp_field_info, _SHR_E_PARAM, "stamp_field_info ");
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Get gtf stamp information.
* \par
* \param [in] unit - Relevant unit.
* \param [out] pkt_edit -  sat packet of gtf information.
* \param [in] stamp_field_info -  stamp information.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_get_stamp_field(
    int unit,
    bcm_sat_gtf_packet_edit_t * pkt_edit,
    dnx_sat_gtf_stamp_field_tbl_info_t * stamp_field_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_stamp_field_verify(unit, pkt_edit, stamp_field_info));

    if (stamp_field_info->stamp_2_ena == 1)
    {
        /*
         * JR2 support 16bits stamp, rather than 2 bits
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "JR2 support 16bits stamp, rather than 2 bits\n");
    }
    else if (stamp_field_info->counter_8_ena == 1)
    {
        pkt_edit->stamps[0].stamp_type = bcmSatStampCounter8Bit;
        pkt_edit->stamps[0].inc_step = stamp_field_info->counter_8_inc_step;
        pkt_edit->stamps[0].inc_period_packets = stamp_field_info->counter_8_period;

        if (stamp_field_info->counter_8_shift == 0)
        {
            pkt_edit->stamps[0].offset = (stamp_field_info->counter_8_offset) * DNX_SAT_BITS_PER_BYTE;
        }
        else
        {
            pkt_edit->stamps[0].offset =
                (stamp_field_info->counter_8_offset - 1) * DNX_SAT_BITS_PER_BYTE + (DNX_SAT_BITS_PER_BYTE -
                                                                                    stamp_field_info->counter_8_shift);
        }
    }
    else if (stamp_field_info->counter_16_ena == 1)
    {
        pkt_edit->stamps[0].stamp_type = bcmSatStampCounter16Bit;
        pkt_edit->stamps[0].inc_step = stamp_field_info->counter_16_inc_step;
        pkt_edit->stamps[0].inc_period_packets = stamp_field_info->counter_16_period;

        if (stamp_field_info->counter_16_shift == 0)
        {
            pkt_edit->stamps[0].offset = (stamp_field_info->counter_16_offset) * DNX_SAT_BITS_PER_BYTE;
        }
        else
        {
            pkt_edit->stamps[0].offset =
                (stamp_field_info->counter_16_offset - 1) * DNX_SAT_BITS_PER_BYTE + (DNX_SAT_BITS_PER_BYTE -
                                                                                     stamp_field_info->counter_16_shift);
        }
    }
    else if (stamp_field_info->stamp_16_ena == 1)
    {
        pkt_edit->stamps[0].stamp_type = bcmSatStampConstant16Bit;
        pkt_edit->stamps[0].value = stamp_field_info->stamp_16_value;

        if (stamp_field_info->stamp_16_shift == 0)
        {
            pkt_edit->stamps[0].offset = (stamp_field_info->stamp_16_offset - 1) * DNX_SAT_BITS_PER_BYTE;
        }
        else
        {
            pkt_edit->stamps[0].offset =
                (stamp_field_info->stamp_16_offset - 2) * DNX_SAT_BITS_PER_BYTE + (DNX_SAT_BITS_PER_BYTE -
                                                                                   stamp_field_info->stamp_16_shift);
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Build gtf stamp information.
* \par
* \param [in] unit - Relevant unit.
* \param [in] pkt_edit -  sat packet of gtf information.
* \param [out] stamp_field_info -  stamp information.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_construct_stamp_field(
    int unit,
    bcm_sat_gtf_packet_edit_t * pkt_edit,
    dnx_sat_gtf_stamp_field_tbl_info_t * stamp_field_info)
{
    const uint32 field_2_cnt_8_mask[7] = { 0xff, 0xe0, 0xc0, 0xff, 0xff, 0xff, 0xff };
    const uint32 field_2_cnt_16_mask[7] = { 0xffff, 0xe000, 0xc000, 0xffff, 0xffff, 0xfff0, 0xfff0 };
    const bcm_sat_stamp_t *tmp_stamp;
    int i = 0;
    int tmp_stamp_offset_bits = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_stamp_field_verify(unit, pkt_edit, stamp_field_info));

    for (i = 0; i < pkt_edit->number_of_stamps; i++)
    {
        tmp_stamp = &(pkt_edit->stamps[i]);

        if (tmp_stamp->stamp_type == bcmSatStampConstant2Bit)
        {
            tmp_stamp_offset_bits = tmp_stamp->offset;
            stamp_field_info->stamp_2_ena = 1;
            if (tmp_stamp_offset_bits % DNX_SAT_BITS_PER_BYTE == 7)
            {
                stamp_field_info->stamp_2_offset = tmp_stamp_offset_bits / DNX_SAT_BITS_PER_BYTE + 1;
                stamp_field_info->stamp_2_shift = 7;
            }
            else
            {
                stamp_field_info->stamp_2_offset = tmp_stamp_offset_bits / DNX_SAT_BITS_PER_BYTE;
                stamp_field_info->stamp_2_shift =
                    DNX_SAT_BITS_PER_BYTE - 2 - tmp_stamp_offset_bits % DNX_SAT_BITS_PER_BYTE;
            }
            stamp_field_info->stamp_2_value = tmp_stamp->value;
        }
        else if (tmp_stamp->stamp_type == bcmSatStampCounter8Bit)
        {
            tmp_stamp_offset_bits = tmp_stamp->offset;
            stamp_field_info->counter_8_ena = 1;
            stamp_field_info->counter_8_offset =
                (tmp_stamp_offset_bits / DNX_SAT_BITS_PER_BYTE) +
                ((tmp_stamp_offset_bits % DNX_SAT_BITS_PER_BYTE) ? 1 : 0);
            stamp_field_info->counter_8_shift =
                (DNX_SAT_BITS_PER_BYTE - (tmp_stamp_offset_bits % DNX_SAT_BITS_PER_BYTE)) % DNX_SAT_BITS_PER_BYTE;
            stamp_field_info->counter_8_mask = field_2_cnt_8_mask[tmp_stamp->field_type];
            stamp_field_info->counter_8_inc_step = tmp_stamp->inc_step;
            stamp_field_info->counter_8_period = tmp_stamp->inc_period_packets;
        }
        else if (tmp_stamp->stamp_type == bcmSatStampCounter16Bit)
        {
            tmp_stamp_offset_bits = tmp_stamp->offset;
            stamp_field_info->counter_16_ena = 1;
            stamp_field_info->counter_16_offset =
                (tmp_stamp_offset_bits / DNX_SAT_BITS_PER_BYTE) +
                ((tmp_stamp_offset_bits % DNX_SAT_BITS_PER_BYTE) ? 1 : 0);
            stamp_field_info->counter_16_shift =
                (DNX_SAT_BITS_PER_BYTE - (tmp_stamp_offset_bits % DNX_SAT_BITS_PER_BYTE)) % DNX_SAT_BITS_PER_BYTE;
            stamp_field_info->counter_16_mask = field_2_cnt_16_mask[tmp_stamp->field_type];
            stamp_field_info->counter_16_inc_step = tmp_stamp->inc_step;
            stamp_field_info->counter_16_period = tmp_stamp->inc_period_packets;
        }
        else if (tmp_stamp->stamp_type == bcmSatStampConstant16Bit)
        {
            tmp_stamp_offset_bits = tmp_stamp->offset;
            stamp_field_info->stamp_16_ena = 1;
            if (tmp_stamp_offset_bits % DNX_SAT_BITS_PER_BYTE == 0)
            {
                stamp_field_info->stamp_16_offset = tmp_stamp_offset_bits / DNX_SAT_BITS_PER_BYTE;
                stamp_field_info->stamp_16_shift = 0;
            }
            else
            {
                stamp_field_info->stamp_16_offset = tmp_stamp_offset_bits / DNX_SAT_BITS_PER_BYTE + 1;
                stamp_field_info->stamp_16_shift =
                    DNX_SAT_BITS_PER_BYTE - tmp_stamp_offset_bits % DNX_SAT_BITS_PER_BYTE;
            }
            stamp_field_info->stamp_16_value = tmp_stamp->value;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify ggtf packet header.
* \par
* \param [in] unit - Relevant unit.
* \param [in] pkt_hdr -  sat packet header.
* \param [out] pkt_hdr_info -  sat packet header information.
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/

static shr_error_e
dnx_sat_gtf_packet_header_field_verify(
    int unit,
    bcm_pkt_t * pkt_hdr,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * pkt_hdr_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(pkt_hdr, _SHR_E_PARAM, "pkt_hdr ");
    SHR_NULL_CHECK(pkt_hdr_info, _SHR_E_PARAM, "pkt_hdr_info ");
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Build gtf packet header.
* \par
* \param [in] unit - Relevant unit.
* \param [in] pkt_hdr -  sat packet header.
* \param [out] pkt_hdr_info -  sat packet header information.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
shr_error_e
dnx_sat_gtf_construct_packet_header(
    int unit,
    bcm_pkt_t * pkt_hdr,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * pkt_hdr_info)
{
    uint32 pkt_data = 0;
    uint32 pkt_hdr_bits = DNX_SAT_PKT_HERDER_BITS, pkt_bits_remainder = 0;
    uint32 bytes_copy, bytes_copied;
    uint32 offset = 0;
    int i = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_packet_header_field_verify(unit, pkt_hdr, pkt_hdr_info));

    offset = pkt_hdr_bits - DNX_SAT_BITS_PER_BYTE;
    bytes_copy = pkt_hdr_bits / DNX_SAT_BITS_PER_BYTE;

    for (i = 0; (i < bytes_copy) && (i < pkt_hdr->pkt_data[0].len); i++)
    {
        pkt_data = pkt_hdr->pkt_data[0].data[i];
        SHR_BITCOPY_RANGE(pkt_hdr_info->pkt_header, offset, &pkt_data, 0, DNX_SAT_BITS_PER_BYTE);
        offset -= DNX_SAT_BITS_PER_BYTE;
    }

    bytes_copied = i;

    if (bytes_copied < pkt_hdr->pkt_data[0].len)
    {
        pkt_bits_remainder = pkt_hdr_bits % DNX_SAT_BITS_PER_BYTE;
        if (pkt_bits_remainder != 0)
        {
            /*
             * Coverity:
             * For JR2/J2C/Q2A DNX_SAT_PKT_HERDER_BITS is 8*N,
             * In case other device, the DNX data isn't 8*N, there have some bits not copied, so keep algorithm.
             */
             /* coverity[dead_error_begin : FALSE]  */
            offset = offset - pkt_bits_remainder;
            pkt_data = pkt_hdr->pkt_data[0].data[bytes_copied];
            SHR_BITCOPY_RANGE(pkt_hdr_info->pkt_header, offset, &pkt_data, (DNX_SAT_BITS_PER_BYTE - pkt_bits_remainder),
                              pkt_bits_remainder);
        }
    }

    pkt_hdr_info->pkt_header_len = pkt_hdr->pkt_data[0].len;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     GET gtf packet header.
* \par
* \param [in] unit - Relevant unit.
* \param [out] pkt_hdr -  sat packet header.
* \param [in] pkt_hdr_info -  sat packet header information.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
shr_error_e
dnx_sat_gtf_get_packet_header(
    int unit,
    bcm_pkt_t * pkt_hdr,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * pkt_hdr_info)
{
    uint32 pkt_data = 0;
    uint32 pkt_hdr_bits = 1024;
    uint32 bytes_copy;
    uint32 offset = 0;
    int i = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_packet_header_field_verify(unit, pkt_hdr, pkt_hdr_info));

    offset = pkt_hdr_bits - DNX_SAT_BITS_PER_BYTE;
    bytes_copy = pkt_hdr_bits / DNX_SAT_BITS_PER_BYTE;

    for (i = 0; i < bytes_copy; i++)
    {
        SHR_BITCOPY_RANGE(&pkt_data, 0, pkt_hdr_info->pkt_header, offset, DNX_SAT_BITS_PER_BYTE);
        pkt_hdr->pkt_data[0].data[i] = pkt_data;
        offset -= DNX_SAT_BITS_PER_BYTE;
    }

    pkt_hdr->pkt_data[0].len = pkt_hdr_info->pkt_header_len;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Set gtf packet header in HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] header_id -  header id.
* \param [in] config -  sat packet header information.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_packet_header_config_set(
    int unit,
    int header_id,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * config)
{
    uint32 entry_handle_id;
    uint32 pkt_hdr_lsb[DNX_SAT_PKT_HERDER_LSB_FIELD_SIZE] = { 0 };
    uint32 pkt_hdr_msb[DNX_SAT_PKT_HERDER_MSB_FIELD_SIZE] = { 0 };
    int i = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * For writing on dynamic tables, have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    /*
     * 2.1 set payload info
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_PKT_HEADER_CONFIG, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_SAT_PACKET_HEADER_INDEX, header_id);

    for (i = 0; i < DNX_SAT_PKT_HERDER_LSB_FIELD_SIZE; i++)
    {
        pkt_hdr_lsb[i] = config->pkt_header[i];
        pkt_hdr_msb[i] = config->pkt_header[i + DNX_SAT_PKT_HERDER_LSB_FIELD_SIZE];
    }

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_TX_PACKET_HEADER_LOW, INST_SINGLE, pkt_hdr_lsb);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_TX_PACKET_HEADER_HIGH, INST_SINGLE, pkt_hdr_msb);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Get gtf packet header from HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] header_id -  header id.
* \param [in] config -  sat packet header information.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_packet_header_config_get(
    int unit,
    int header_id,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * config)
{
    uint32 entry_handle_id;
    uint32 pkt_hdr_lsb[DNX_SAT_PKT_HERDER_LSB_FIELD_SIZE];
    uint32 pkt_hdr_msb[DNX_SAT_PKT_HERDER_MSB_FIELD_SIZE];
    int i = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * For writing on dynamic tables, have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    /*
     * 2.1 set payload info
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_PKT_HEADER_CONFIG, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_SAT_PACKET_HEADER_INDEX, header_id);

    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_TX_PACKET_HEADER_LOW, INST_SINGLE, pkt_hdr_lsb);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_TX_PACKET_HEADER_HIGH, INST_SINGLE, pkt_hdr_msb);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    for (i = 0; i < DNX_SAT_PKT_HERDER_LSB_FIELD_SIZE; i++)
    {
        config->pkt_header[i] = pkt_hdr_lsb[i];
        config->pkt_header[i + DNX_SAT_PKT_HERDER_LSB_FIELD_SIZE] = pkt_hdr_msb[i];
    }

exit:
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Set gtf packet  in HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] gtf_id -  gtf id.
* \param [in] header_id -  header id.
* \param [in] header_len -  header len.
* \param [in] config -  Sat gtf packet config.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_packet_config_set(
    int unit,
    bcm_sat_gtf_t gtf_id,
    uint32 header_id,
    uint32 header_len,
    bcm_sat_gtf_packet_config_t * config)
{
    int rv = _SHR_E_NONE;
    uint32 payload_type = 0;
    uint32 entry_handle_id;
    uint32 payload_rpt_pattern[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U32] = { 0 };
    dnx_sat_gtf_stamp_field_tbl_info_t stamp_field_info;
    uint32 sequence_ena = 0;
    uint32 sequence_period = 0;
    uint32 sequence_offset = 0;
    uint32 pkt_add_crc = 0;
    uint32 pkt_add_tlv = 0;
    uint32 crc_offset = 0;
    uint32 offset = 0;
    uint32 length_pattern[DNX_SAT_PKT_LENGTH_PATTERN_FIELD_SIZE] = { 0 };       /* total 96bits = 32 * 3 */
    int i = 0;
    int priority = 0;           /* GTF is a flow, only use priority 0 */
    uint32 pkt_length_fields[DNX_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS] = {
        DBAL_FIELD_PKT_LENGTH_0,
        DBAL_FIELD_PKT_LENGTH_1,
        DBAL_FIELD_PKT_LENGTH_2,
        DBAL_FIELD_PKT_LENGTH_3,
        DBAL_FIELD_PKT_LENGTH_4,
        DBAL_FIELD_PKT_LENGTH_5,
        DBAL_FIELD_PKT_LENGTH_6,
        DBAL_FIELD_PKT_LENGTH_7
    };
    /*
     * PktHeaderBase[i] represent bits [11:02] of the PktHeader memory's address The header is 4B aligned, think of the
     * memory as logically 1024x32b, with each header start being read from that pointed by PktHeaderBase for packet's
     * flow.
     */
    uint32 header_add[32] = {
        0, 32, 64, 96, 128, 160, 192, 224,
        256, 288, 320, 352, 384, 416, 448, 480,
        512, 544, 576, 608, 640, 672, 704, 736,
        768, 800, 832, 864, 896, 928, 960, 992
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * 1.1 prepare payload
     */
    if (config->payload.payload_type == bcmSatPayloadConstant8Bytes)
    {
        rv = dnx_sat_packet_config_exp_payload_uint8_to_long(config->payload.payload_pattern,
                                                             DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8, 0,
                                                             payload_rpt_pattern);
        SHR_IF_ERR_EXIT(rv);
        payload_type = 0;
    }
    else if (config->payload.payload_type == bcmSatPayloadConstant4Bytes)
    {
        rv = dnx_sat_packet_config_exp_payload_uint8_to_long(config->payload.payload_pattern,
                                                             DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8 / 2, 0,
                                                             payload_rpt_pattern);
        SHR_IF_ERR_EXIT(rv);
        payload_type = 1;
    }
    else if (config->payload.payload_type == bcmSatPayloadPRBS)
    {
        payload_type = 2;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "payload_type ERROR\n");
    }

    /*
     * 1.2 prepare stamp
     */
    sal_memset(&(stamp_field_info), 0, sizeof(stamp_field_info));
    rv = dnx_sat_gtf_construct_stamp_field(unit, &(config->packet_edit[priority]), &(stamp_field_info));
    SHR_IF_ERR_EXIT(rv);

    /*
     * 1.3 prepare sequence
     */
    if (config->offsets.seq_number_offset != DNX_SAT_GTF_SEQ_NUM_DISABLE)
    {
        sequence_ena = 1;
        sequence_offset = config->offsets.seq_number_offset;
        sequence_period = config->packet_edit[priority].number_of_ctfs - 1;
    }
    else
    {
        sequence_ena = 0;
    }

    /*
     * 1.4 prepare tlv and crc
     */
    if (config->packet_edit[priority].flags & BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV)
    {
        pkt_add_tlv = 1;
    }
    else
    {
        pkt_add_tlv = 0;
    }

    if (config->packet_edit[priority].flags & BCM_SAT_GTF_PACKET_EDIT_ADD_CRC)
    {
        pkt_add_crc = 1;
        crc_offset = config->offsets.crc_byte_offset;
    }
    else
    {
        pkt_add_crc = 0;
    }

    /*
     * For writing on dynamic tables, have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    /*
     * 2.1 set payload info
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_PKT_PARAM_CONFIG, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PAYLOAD_TYPE, INST_SINGLE, payload_type);
    /*
     * payload_rpt_pattern_64 = payload_rpt_pattern[1]; payload_rpt_pattern_64 = ((payload_rpt_pattern_64 <<32) |
     * payload_rpt_pattern[0]);
     */
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RPT_PATTERN, INST_SINGLE, payload_rpt_pattern);

    /*
     * 2.2 set length, pattern, stamp, sequence, tlv and crc
     */
    /*
     * PKT_LENGTH_XXXf
     */
    for (i = 0; i < DNX_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, pkt_length_fields[i], INST_SINGLE,
                                     config->packet_edit[priority].packet_length[i]);
    }
    /*
     * LENGTH_PATTERNf
     */
    offset = 0;
    for (i = 0; i < config->packet_edit[priority].pattern_length; i++)
    {
        SHR_BITCOPY_RANGE(length_pattern, offset, &(config->packet_edit[priority].packet_length_pattern[i]), 0, 3);
        offset += 3;
    }
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_LENGTH_PATTERN, INST_SINGLE, length_pattern);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LENGTH_PATTERN_SIZE, INST_SINGLE,
                                 config->packet_edit[priority].pattern_length - 1);

    /*
     * STMP_16_BIT_XXXf
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STMP_16_BIT_ENABLE, INST_SINGLE,
                                 stamp_field_info.stamp_16_ena);
    if (stamp_field_info.stamp_16_ena != 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STMP_16_BIT_OFFSET, INST_SINGLE,
                                     stamp_field_info.stamp_16_offset);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STMP_16_BIT_SHIFT, INST_SINGLE,
                                     stamp_field_info.stamp_16_shift);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STMP_16_BIT_VALUE, INST_SINGLE,
                                     stamp_field_info.stamp_16_value);
    }
    /*
     * CNTR_8_BIT_XXXf
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_ENABLE, INST_SINGLE,
                                 stamp_field_info.counter_8_ena);
    if (stamp_field_info.counter_8_ena != 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_OFFSET, INST_SINGLE,
                                     stamp_field_info.counter_8_offset);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_SHIFT, INST_SINGLE,
                                     stamp_field_info.counter_8_shift);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_MASK, INST_SINGLE,
                                     stamp_field_info.counter_8_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_INC_STEP, INST_SINGLE,
                                     stamp_field_info.counter_8_inc_step);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_PERIOD, INST_SINGLE,
                                     stamp_field_info.counter_8_period);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_NEXT_VALUE, INST_SINGLE, 0);
    }
    /*
     * CNTR_16_BIT_XXXf
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_ENABLE, INST_SINGLE,
                                 stamp_field_info.counter_16_ena);
    if (stamp_field_info.counter_16_ena != 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_OFFSET, INST_SINGLE,
                                     stamp_field_info.counter_16_offset);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_SHIFT, INST_SINGLE,
                                     stamp_field_info.counter_16_shift);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_MASK, INST_SINGLE,
                                     stamp_field_info.counter_16_mask);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_INC_STEP, INST_SINGLE,
                                     stamp_field_info.counter_16_inc_step);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_PERIOD, INST_SINGLE,
                                     stamp_field_info.counter_16_period);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_NEXT_VALUE, INST_SINGLE, 0);
    }
    /*
     * SEQ_NUM_XXXf
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEQ_NUM_ENABLE, INST_SINGLE, sequence_ena);
    if (sequence_ena == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEQ_NUM_PERIOD, INST_SINGLE, sequence_period);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEQ_NUM_OFFSET, INST_SINGLE, sequence_offset);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEQ_NUM_NEXT_VALUE, INST_SINGLE, 0);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADD_END_TLV, INST_SINGLE, pkt_add_tlv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_32_ENABLE, INST_SINGLE, pkt_add_crc);
    if (pkt_add_crc == 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_32_OFFSET, INST_SINGLE, crc_offset);
    }

    /*
     * set sat packet as non-TDM packet && set src channel to 0
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL_NUM, INST_SINGLE, 0);

    /*
     * Core id
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_CORE_ID, INST_SINGLE,
                                 config->packet_context_id);

    /*
     * PKET HEADER POINT
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PKT_HEADER_POINT, INST_SINGLE,
                                 header_add[header_id]);
    /*
     * PKET HEADER Lenth
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PKT_HEADER_LENGTH, INST_SINGLE, header_len);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Get gtf packet from HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] gtf_id -  gtf id.
* \param [in] header_id -  header id.
* \param [out] config -  Sat gtf packet config.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_packet_config_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    uint32 header_id,
    bcm_sat_gtf_packet_config_t * config)
{
    int rv = _SHR_E_NONE;
    uint32 payload_type = 0;
    uint32 entry_handle_id;
    uint32 payload_rpt_pattern[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U32] = { 0 };
    dnx_sat_gtf_stamp_field_tbl_info_t stamp_field_info;
    uint32 sequence_ena = 0;
    uint32 sequence_period = 0;
    uint32 sequence_offset = 0;
    uint32 pkt_add_crc = 0;
    uint32 pkt_add_tlv = 0;
    uint32 crc_offset = 0;
    uint32 offset = 0;
    uint32 length_pattern[DNX_SAT_PKT_LENGTH_PATTERN_FIELD_SIZE] = { 0 };
    uint32 length_pattern_size = 0;
    int i = 0;
    uint32 packet_length[8] = { 0 };
    uint32 pkt_length_fields[DNX_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS] = {
        DBAL_FIELD_PKT_LENGTH_0,
        DBAL_FIELD_PKT_LENGTH_1,
        DBAL_FIELD_PKT_LENGTH_2,
        DBAL_FIELD_PKT_LENGTH_3,
        DBAL_FIELD_PKT_LENGTH_4,
        DBAL_FIELD_PKT_LENGTH_5,
        DBAL_FIELD_PKT_LENGTH_6,
        DBAL_FIELD_PKT_LENGTH_7
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * For writing on dynamic tables, have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    /*
     * 2.1 set payload info
     */
    sal_memset(&(stamp_field_info), 0, sizeof(stamp_field_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_PKT_PARAM_CONFIG, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PAYLOAD_TYPE, INST_SINGLE, &payload_type);

    /*
     * payload_rpt_pattern_64 = payload_rpt_pattern[1]; payload_rpt_pattern_64 = ((payload_rpt_pattern_64 <<32) |
     * payload_rpt_pattern[0]);
     */
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_RPT_PATTERN, INST_SINGLE, payload_rpt_pattern);

    /*
     * 1.0 get length, pattern,
     */
    /*
     * PKT_LENGTH_XXXf
     */
    for (i = 0; i < DNX_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++)
    {
        dbal_value_field32_request(unit, entry_handle_id, pkt_length_fields[i], INST_SINGLE, &(packet_length[i]));
    }
    /*
     * LENGTH_PATTERNf
     */
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_LENGTH_PATTERN, INST_SINGLE, length_pattern);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LENGTH_PATTERN_SIZE, INST_SINGLE,
                               &length_pattern_size);
    /*
     * STMP_16_BIT_XXXf
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STMP_16_BIT_ENABLE, INST_SINGLE,
                               &(stamp_field_info.stamp_16_ena));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STMP_16_BIT_OFFSET, INST_SINGLE,
                               &(stamp_field_info.stamp_16_offset));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STMP_16_BIT_SHIFT, INST_SINGLE,
                               &(stamp_field_info.stamp_16_shift));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STMP_16_BIT_VALUE, INST_SINGLE,
                               &(stamp_field_info.stamp_16_value));
    /*
     * CNTR_8_BIT_XXXf
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_ENABLE, INST_SINGLE,
                               &(stamp_field_info.counter_8_ena));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_OFFSET, INST_SINGLE,
                               &(stamp_field_info.counter_8_offset));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_SHIFT, INST_SINGLE,
                               &(stamp_field_info.counter_8_shift));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_MASK, INST_SINGLE,
                               &(stamp_field_info.counter_8_mask));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_INC_STEP, INST_SINGLE,
                               &(stamp_field_info.counter_8_inc_step));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_8_BIT_PERIOD, INST_SINGLE,
                               &(stamp_field_info.counter_8_period));

    /*
     * CNTR_16_BIT_XXXf
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_ENABLE, INST_SINGLE,
                               &(stamp_field_info.counter_16_ena));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_OFFSET, INST_SINGLE,
                               &(stamp_field_info.counter_16_offset));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_SHIFT, INST_SINGLE,
                               &(stamp_field_info.counter_16_shift));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_MASK, INST_SINGLE,
                               &(stamp_field_info.counter_16_mask));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_INC_STEP, INST_SINGLE,
                               &(stamp_field_info.counter_16_inc_step));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_PERIOD, INST_SINGLE,
                               &(stamp_field_info.counter_16_period));
    /*
     * dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CNTR_16_BIT_NEXT_VALUE,INST_SINGLE,0);
     */

    /*
     * SEQ_NUM_XXXf
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SEQ_NUM_ENABLE, INST_SINGLE, &sequence_ena);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SEQ_NUM_OFFSET, INST_SINGLE, &sequence_period);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SEQ_NUM_PERIOD, INST_SINGLE, &sequence_offset);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ADD_END_TLV, INST_SINGLE, &pkt_add_tlv);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRC_32_ENABLE, INST_SINGLE, &pkt_add_crc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRC_32_OFFSET, INST_SINGLE, &crc_offset);

    /*
     * Core id
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FLOW_CORE_ID, INST_SINGLE,
                               &(config->packet_context_id));

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    for (i = 0; i < DNX_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++)
    {
        config->packet_edit[0].packet_length[i] = packet_length[i];
    }

    config->packet_edit[0].pattern_length = length_pattern_size + 1;
    offset = 0;
    for (i = 0; i < config->packet_edit[0].pattern_length; i++)
    {
        SHR_BITCOPY_RANGE(&(config->packet_edit[0].packet_length_pattern[i]), 0, length_pattern, offset, 3);
        offset += 3;
    }

    /*
     * 1.1 payload
     */
    if (payload_type == 0)
    {
        rv = dnx_sat_packet_config_exp_payload_long_to_uint8(config->payload.payload_pattern,
                                                             DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8, 0,
                                                             payload_rpt_pattern);
        SHR_IF_ERR_EXIT(rv);
        config->payload.payload_type = bcmSatPayloadConstant8Bytes;
    }
    else if (payload_type == 1)
    {
        rv = dnx_sat_packet_config_exp_payload_long_to_uint8(config->payload.payload_pattern,
                                                             DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8 / 2, 0,
                                                             payload_rpt_pattern);
        SHR_IF_ERR_EXIT(rv);
        config->payload.payload_type = bcmSatPayloadConstant4Bytes;
    }
    else if (payload_type == 2)
    {
        config->payload.payload_type = bcmSatPayloadPRBS;
    }

    /*
     * 1.2 stamp
     */
    rv = dnx_sat_gtf_get_stamp_field(unit, &(config->packet_edit[0]), &(stamp_field_info));
    SHR_IF_ERR_EXIT(rv);

    /*
     * 1.3 sequence
     */
    if (sequence_ena)
    {
        config->offsets.seq_number_offset = sequence_offset;
        config->packet_edit[0].number_of_ctfs = sequence_period + 1;
    }
    else
    {
        config->offsets.seq_number_offset = DNX_SAT_GTF_SEQ_NUM_DISABLE;
    }

    /*
     * 1.4 tlv and crc
     */
    if (pkt_add_tlv)
    {
        config->packet_edit[0].flags = config->packet_edit[0].flags | BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV;
    }

    if (pkt_add_crc)
    {
        config->packet_edit[0].flags = config->packet_edit[0].flags | BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV;
        config->offsets.crc_byte_offset = crc_offset;
    }

exit:
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify packet information configurefor BCM-API: bcm_sat_gtf_packet_config_set*() .
* \par
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] config  -
*    Sat gtf packet config
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_packet_config_set_verify(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_packet_config_t * config)
{
    int priority;
    int i = 0;
    uint8 is_allocated = 0;
    int max_seq_number_offset = 0;
    int max_crc_byte_offset = 0;

    int stamp_count[bcmSatStampCount] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
    SHR_RANGE_VERIFY(config->sat_header_type, bcmSatHeaderUserDefined, bcmSatHeaderY1731, _SHR_E_PARAM,
                     "sat_header_type only support UserDefined");
    SHR_NULL_CHECK(config->header_info.pkt_data, _SHR_E_PARAM, "config->header_info.pkt_data");
    SHR_RANGE_VERIFY(config->header_info.pkt_data[0].len, DNX_SAT_GTF_PKT_HDR_LEN_MIN,
                     (DNX_SAT_GTF_PKT_HDR_LEN_MAX), _SHR_E_PARAM, "header_info.pkt_data[0].len");
    SHR_RANGE_VERIFY(config->payload.payload_type, bcmSatPayloadConstant8Bytes, (bcmSatPayloadsCount - 1), _SHR_E_PARAM,
                     "payload_type");
    SHR_MAX_VERIFY(config->packet_context_id, (dnx_drv_max_num_of_cores() - 1), _SHR_E_PARAM, "packet_context_id");
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATGtfSeqNumOffset, &max_seq_number_offset));
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATGtfCrcOffset, &max_crc_byte_offset));
    SHR_MAX_VERIFY(config->offsets.seq_number_offset, max_seq_number_offset, _SHR_E_PARAM, "max_seq_number_offset");
    SHR_MAX_VERIFY(config->offsets.crc_byte_offset, max_crc_byte_offset, _SHR_E_PARAM, "max_crc_byte_offset");

    /** check gtf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_allocated));

    /** return error if the gtf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: gtf_id  %d does not exist\n", gtf_id);
    }

    for (priority = 0; priority < (BCM_SAT_GTF_NUM_OF_PRIORITIES - 1); priority++)
    {
        SHR_RANGE_VERIFY(config->packet_edit[priority].pattern_length, 1, BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH,
                         _SHR_E_PARAM, "pattern_length");

        for (i = 0; i < config->packet_edit[priority].pattern_length; i++)
        {
            SHR_MAX_VERIFY(config->packet_edit[priority].packet_length_pattern[i],
                           DNX_SAT_GTF_PKT_LEN_PATN_INX_MAX, _SHR_E_PARAM, "packet_length_pattern");
        }
        for (i = 0; i < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++)
        {
            if ((config->packet_edit[priority].packet_length[i] != 0)
                && (config->packet_edit[priority].packet_length[i] < DNX_SAT_GTF_PACKET_LENGTH_MIN))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "packet length must large than %d\n", DNX_SAT_GTF_PACKET_LENGTH_MIN - 1);
            }
        }
        SHR_MAX_VERIFY(config->packet_edit[priority].number_of_stamps, BCM_SAT_GTF_MAX_STAMPS,
                       _SHR_E_PARAM, "number_of_stamps");
        SHR_RANGE_VERIFY(config->packet_edit[priority].number_of_ctfs, DNX_SAT_GTF_SEQ_PERIOD_MIN,
                         DNX_SAT_GTF_SEQ_PERIOD_MAX, _SHR_E_PARAM, "number_of_ctfs");

        stamp_count[bcmSatStampConstant2Bit] = stamp_count[bcmSatStampCounter8Bit] =
            stamp_count[bcmSatStampCounter16Bit] = 0;
        for (i = 0; i < BCM_SAT_GTF_MAX_STAMPS; i++)
        {
            SHR_RANGE_VERIFY(config->packet_edit[priority].stamps[i].stamp_type, bcmSatStampInvalid,
                             (bcmSatStampCount - 1), _SHR_E_PARAM, "stamp_type");
            SHR_RANGE_VERIFY(config->packet_edit[priority].stamps[i].field_type, bcmSatStampFieldUserDefined,
                             (bcmSatStampFieldsCount - 1), _SHR_E_PARAM, "stamp_field_type");

            if ((config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampCounter8Bit) ||
                (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampCounter16Bit))
            {
                SHR_RANGE_VERIFY(config->packet_edit[priority].stamps[i].inc_step, DNX_SAT_GTF_STAMP_INC_STEP_MIN,
                                 _SHR_E_PARAM, DNX_SAT_GTF_STAMP_INC_STEP_MAX, "stamp_inc_step");
                SHR_MAX_VERIFY(config->packet_edit[priority].stamps[i].inc_period_packets,
                               DNX_SAT_GTF_STAMP_INC_PERIOD_MAX, _SHR_E_PARAM, "stamp_inc_period_packets");
            }
            else if (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampConstant2Bit)
            {
                SHR_ERR_EXIT(BCM_E_PARAM, "Jericho 2 support 16bits constant rather than 2 bits\n");
            }
            else if (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampConstant16Bit)
            {
                SHR_MAX_VERIFY(config->packet_edit[priority].stamps[i].value,
                               DNX_SAT_GTF_STAMP_16_bit_VALUE_MAX, _SHR_E_PARAM, "stamp_value");
            }
            stamp_count[config->packet_edit[priority].stamps[i].stamp_type]++;
        }

        if ((stamp_count[bcmSatStampConstant2Bit] > 1) ||
            (stamp_count[bcmSatStampCounter8Bit] > 1) ||
            (stamp_count[bcmSatStampCounter16Bit] > 1) || (stamp_count[bcmSatStampConstant16Bit] > 1))
        {
            SHR_ERR_EXIT(BCM_E_PARAM,
                         " a CIR/EIR can have only one 2bit stamp, one 8bit counter and one 16bit counter\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   GTF packet information configure.
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] config  -
*    Sat gtf packet config
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_packet_config_set(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_packet_config_t * config)
{
    bcm_sat_gtf_packet_config_t *dnx_pkt_cfg = NULL;
    int i = 0;
    dnx_sat_data_t psat_data;
    uint8 pkt_data[128];
    dnx_sat_gtf_pkt_hdr_tbl_info_t header_tbl_data;
    int priority;
    int old_profile = 0;
    int new_profile = 0;
    int is_allocated = 0;
    int is_last = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    sal_memset(&header_tbl_data, 0, sizeof(header_tbl_data));
    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d)\n", BSL_FUNC, BSL_LINE, unit, gtf_id));
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_packet_config_set_verify(unit, gtf_id, config));

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config ");

    dnx_sat_packet_config_dump(config);

    SHR_ALLOC(dnx_pkt_cfg, sizeof(bcm_sat_gtf_packet_config_t), "dnx_sat_gtf_packet_config", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);
    if (dnx_pkt_cfg == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "unit %d, failed to allocate gtf packet memory \n", unit);
    }

    sal_memset(dnx_pkt_cfg, 0, sizeof(bcm_sat_gtf_packet_config_t));
    /*
     * Don't allow to configure gtf packet during packet generate enable
     */
    dnx_pkt_cfg->header_info.pkt_data = &(dnx_pkt_cfg->header_info._pkt_data);
    dnx_pkt_cfg->header_info.blk_count = 1;
    dnx_pkt_cfg->header_info.pkt_data[0].data = pkt_data;
    dnx_pkt_cfg->header_info.pkt_data[0].len = config->header_info.pkt_data[0].len;
    sal_memcpy(dnx_pkt_cfg->header_info.pkt_data[0].data, config->header_info.pkt_data[0].data,
               sizeof(uint8) * config->header_info.pkt_data[0].len);
    dnx_pkt_cfg->payload.payload_type = config->payload.payload_type;
    sal_memcpy(dnx_pkt_cfg->payload.payload_pattern, config->payload.payload_pattern,
               sizeof(uint8) * BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE);
    for (priority = 0; priority < (BCM_SAT_GTF_NUM_OF_PRIORITIES - 1); priority++)
    {
        sal_memcpy(dnx_pkt_cfg->packet_edit[priority].packet_length, config->packet_edit[priority].packet_length,
                   sizeof(uint32) * BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS);
        sal_memcpy(dnx_pkt_cfg->packet_edit[priority].packet_length_pattern,
                   config->packet_edit[priority].packet_length_pattern,
                   sizeof(uint32) * BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH);
        dnx_pkt_cfg->packet_edit[priority].pattern_length = config->packet_edit[priority].pattern_length;

        dnx_pkt_cfg->packet_edit[priority].number_of_stamps = config->packet_edit[priority].number_of_stamps;
        for (i = 0; i < config->packet_edit[priority].number_of_stamps; i++)
        {
            dnx_pkt_cfg->packet_edit[priority].stamps[i].stamp_type =
                config->packet_edit[priority].stamps[i].stamp_type;
            dnx_pkt_cfg->packet_edit[priority].stamps[i].field_type =
                config->packet_edit[priority].stamps[i].field_type;
            if ((config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampCounter8Bit)
                || (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampCounter16Bit))
            {
                dnx_pkt_cfg->packet_edit[priority].stamps[i].inc_step =
                    config->packet_edit[priority].stamps[i].inc_step;
                dnx_pkt_cfg->packet_edit[priority].stamps[i].inc_period_packets =
                    config->packet_edit[priority].stamps[i].inc_period_packets;
            }
            else if ((config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampConstant2Bit) ||
                     (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampConstant16Bit))
            {
                dnx_pkt_cfg->packet_edit[priority].stamps[i].value = config->packet_edit[priority].stamps[i].value;
            }
            dnx_pkt_cfg->packet_edit[priority].stamps[i].offset = config->packet_edit[priority].stamps[i].offset;
        }
        dnx_pkt_cfg->packet_edit[priority].number_of_ctfs = config->packet_edit[priority].number_of_ctfs;
        if (config->packet_edit[priority].flags & BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV)
        {
            dnx_pkt_cfg->packet_edit[priority].flags |= BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV;
        }
        if (config->packet_edit[priority].flags & BCM_SAT_GTF_PACKET_EDIT_ADD_CRC)
        {
            dnx_pkt_cfg->packet_edit[priority].flags |= BCM_SAT_GTF_PACKET_EDIT_ADD_CRC;
        }
    }
    dnx_pkt_cfg->packet_context_id = config->packet_context_id;
    dnx_pkt_cfg->offsets.payload_offset = config->offsets.payload_offset;
    dnx_pkt_cfg->offsets.timestamp_offset = config->offsets.timestamp_offset;
    dnx_pkt_cfg->offsets.seq_number_offset = config->offsets.seq_number_offset;
    dnx_pkt_cfg->offsets.crc_byte_offset = config->offsets.crc_byte_offset;

    SHR_IF_ERR_EXIT(dnx_sat_gtf_construct_packet_header(unit, &(dnx_pkt_cfg->header_info), &header_tbl_data));
    old_profile = psat_data.gtf_header_id[gtf_id];
    SHR_IF_ERR_EXIT(dnx_am_template_sat_pkt_header_exchange
                    (unit, 0, &header_tbl_data, old_profile, &is_last, &new_profile, &is_allocated));

    psat_data.gtf_header_id[gtf_id] = new_profile;

    SHR_IF_ERR_EXIT(dnx_sat_data_set(unit, &psat_data));

    if (is_allocated)
    {
        SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_header_config_set(unit, new_profile - 1, &header_tbl_data));
    }
    if (is_last)
    {
        SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_header_config_destroy(unit, old_profile - 1));
    }

    SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_config_set
                    (unit, gtf_id, new_profile - 1, header_tbl_data.pkt_header_len, dnx_pkt_cfg));

    sal_free(dnx_pkt_cfg);

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*   GTF packet information configure.
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] config  -
*    Sat gtf packet config
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * ( Keep it for backward compatability)
*/
shr_error_e
bcm_dnx_sat_gtf_packet_config(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_packet_config_t * config)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_sat_gtf_packet_config_set(unit, gtf_id, config));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify packet information configurefor BCM-API: bcm_sat_gtf_packet_config_get*() .
* \par
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] config  -
*    Sat gtf packet config
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_packet_config_get_verify(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_packet_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config ");
    SHR_RANGE_VERIFY(gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Get GTF packet information configure.
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] config  -
*    Sat gtf packet config
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_packet_config_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_packet_config_t * config)
{
    dnx_sat_gtf_pkt_hdr_tbl_info_t header_tbl_data;
    uint32 header_id = 0;
    dnx_sat_data_t psat_data;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_packet_config_get_verify(unit, gtf_id, config));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d)\n", BSL_FUNC, BSL_LINE, unit, gtf_id));

    sal_memset(&header_tbl_data, 0, sizeof(header_tbl_data));
    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));
    header_id = psat_data.gtf_header_id[gtf_id];

    SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_header_config_get(unit, header_id, &header_tbl_data););
    SHR_IF_ERR_EXIT(dnx_sat_gtf_get_packet_header(unit, &(config->header_info), &header_tbl_data));
    SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_config_get(unit, gtf_id, header_id, config));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify Start to send GTF packet  for BCM-API: bcm_sat_gtf_packet_start*() .
* \par
* \param [in] unit  -Relevant unit.
* \param [in] gtf_id  -GTF ID.
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_packet_start_verify(
    int unit,
    bcm_sat_gtf_t gtf_id)
{
    uint8 is_allocated = 0;
    int packet_length = 0;
    bcm_sat_gtf_packet_config_t config;
    SHR_FUNC_INIT_VARS(unit);
    SHR_RANGE_VERIFY(gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
   /** check gtf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_allocated));

   /** return error if the gtf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: gtf_id  %d does not exist\n", gtf_id);
    }
    /** Get and Verify parameters */
    SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_config_get(unit, gtf_id, 0, &config));
    for (int i = 0; i < DNX_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++)
    {
        packet_length += config.packet_edit[0].packet_length[i];
    }
    if (!packet_length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "packet config for gtf(%d) not exist\n", gtf_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Start to send GTF packet .
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] priority  -
*    SAT GTF Priority
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_packet_start(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_pri_t priority)
{
    int pkt_gen_en = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_packet_start_verify(unit, gtf_id));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d) priority(%d)\n", BSL_FUNC, BSL_LINE, unit, gtf_id, priority));

    /*
     * Enable Genrate Packet
     */
    pkt_gen_en = 1;
    SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_gen_set(unit, gtf_id, pkt_gen_en));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify stop to send GTF packet  for BCM-API: bcm_sat_gtf_packet_stop*() .
* \par
* \param [in] unit  -Relevant unit.
* \param [in] gtf_id  -GTF ID.
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_packet_stop_verify(
    int unit,
    bcm_sat_gtf_t gtf_id)
{
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_RANGE_VERIFY(gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");

   /** check gtf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_allocated));

   /** return error if the gtf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: gtf_id  %d does not exist\n", gtf_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Stop to send GTF packet .
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] priority  -
*    SAT GTF Priority
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_packet_stop(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_pri_t priority)
{
    int pkt_gen_en = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_packet_stop_verify(unit, gtf_id));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d) priority(%d)\n", BSL_FUNC, BSL_LINE, unit, gtf_id, priority));

    /*
     * disable Genrate Packet
     */
    pkt_gen_en = 0;
    SHR_IF_ERR_EXIT(dnx_sat_gtf_packet_gen_set(unit, gtf_id, pkt_gen_en));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Get gtf rate pattern from HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] gtf_id -  gtf id.
* \param [out] config -  sat gtf rate pattern .
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_rate_pattern_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_rate_pattern_t * config)
{
    uint32 entry_handle_id;
    uint32 high_mnt_val = 0, high_exp_val = 0, low_mnt_val = 0, low_exp_val = 0;
    uint32 interval_cnt_to_cfg;
    uint32 rate_pattern_mode = 0;
    uint32 complex = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_RATE_CONTROL, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_UPPER_MANTISSA, INST_SINGLE, &high_mnt_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_UPPER_EXPONENT, INST_SINGLE, &high_exp_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_LOWER_MANTISSA, INST_SINGLE, &low_mnt_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_LOWER_EXPONENT, INST_SINGLE, &low_exp_val);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STOP_ITER_CNT, INST_SINGLE,
                               &(config->stop_iter_count));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STOP_BURST_CNT, INST_SINGLE,
                               &(config->stop_burst_count));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STOP_INTERVAL_CNT, INST_SINGLE,
                               &(config->stop_interval_count));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BURST_PKT_WEIGHT, INST_SINGLE,
                               &(config->burst_packet_weight));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INTERVAL_PKT_WEIGHT, INST_SINGLE,
                               &(config->interval_packet_weight));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BURST_MODE, INST_SINGLE, &rate_pattern_mode);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SET_INTRVAL_CNT_TO_CFG, INST_SINGLE,
                               &interval_cnt_to_cfg);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (interval_cnt_to_cfg == 0x1)
    {
        config->flags |= BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST;
    }

    /*
     * if unit is Kbps, then change to bytes should *1000 / 8bits = 125, and vice versa.
     */
    SHR_IF_ERR_EXIT(utilex_compute_complex_to_mnt_exp(high_mnt_val, high_exp_val, 1, 0, &complex));
    config->high_threshold = complex / 125;

    SHR_IF_ERR_EXIT(utilex_compute_complex_to_mnt_exp(low_mnt_val, low_exp_val, 1, 0, &complex));
    config->low_threshold = complex / 125;

    if (rate_pattern_mode == 0)
    {
        config->rate_pattern_mode = (config->high_threshold == config->low_threshold) ?
            bcmSatGtfRatePatternContinues : bcmSatGtfRatePatternSimpleBurst;
    }
    else if (rate_pattern_mode == 1)
    {
        config->rate_pattern_mode = bcmSatGtfRatePatternCombined;
    }
    else if (rate_pattern_mode == 3)
    {
        config->rate_pattern_mode = bcmSatGtfRatePatternInterval;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Fail   in parsing rate_pattern_mode(%d) \n", rate_pattern_mode);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify stop to send GTF packet  for BCM-API: bcm_sat_gtf_packet_stop*() .
* \par
* \param [in] unit  -Relevant unit.
* \param [in] gtf_id  -GTF ID.
* \param [in] config  -Configure sat gtf rate pattern
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_rate_pattern_get_verify(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_rate_pattern_t * config)
{
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config ");
    SHR_RANGE_VERIFY(gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
    /** check gtf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_allocated));

    /** return error if the gtf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: gtf_id  %d does not exist\n", gtf_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Get GTF rate pattern .
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] priority  -
*    SAT GTF Priority
* \param [out ] config  -
*    Configure sat gtf rate pattern
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_rate_pattern_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    int priority,
    bcm_sat_gtf_rate_pattern_t * config)
{
    bcm_sat_gtf_rate_pattern_t dnx_rate_pattern;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_rate_pattern_get_verify(unit, gtf_id, config));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) \n", BSL_FUNC, BSL_LINE, unit));

    sal_memset(&dnx_rate_pattern, 0, sizeof(dnx_rate_pattern));
    SHR_IF_ERR_EXIT(dnx_sat_gtf_rate_pattern_get(unit, gtf_id, &dnx_rate_pattern));

    config->rate_pattern_mode = dnx_rate_pattern.rate_pattern_mode;
    config->high_threshold = dnx_rate_pattern.high_threshold;
    config->low_threshold = dnx_rate_pattern.low_threshold;
    config->stop_iter_count = dnx_rate_pattern.stop_iter_count;
    config->stop_burst_count = dnx_rate_pattern.stop_burst_count;
    config->stop_interval_count = dnx_rate_pattern.stop_interval_count;
    config->burst_packet_weight = dnx_rate_pattern.burst_packet_weight;
    config->interval_packet_weight = dnx_rate_pattern.interval_packet_weight;
    if (dnx_rate_pattern.flags & BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST)
    {
        config->flags |= BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Set gtf rate pattern in HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] gtf_id -  gtf id.
* \param [in] config -  sat gtf rate pattern .
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_rate_pattern_set(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_rate_pattern_t * config)
{
    uint32 entry_handle_id;
    uint32 up_mnt_nof_bits = 0, up_exp_nof_bits = 0;
    uint32 low_mnt_nof_bits = 0, low_exp_nof_bits = 0;
    uint32 high_mnt_val = 0, high_exp_val = 0, low_mnt_val = 0, low_exp_val = 0;
    uint32 field_data = 0;
    uint32 is_pkt_mode = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_BANDWIDTH_CONTROL, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);
    /** getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDITS_PER_PACKET_EN, INST_SINGLE, &is_pkt_mode);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * field length of THRESHOLD_UPPER_MNT and THRESHOLD_UPPER_EXP_
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SAT_TX_RATE_CONTROL, DBAL_FIELD_THRESHOLD_UPPER_MANTISSA, FALSE, 0, 0,
                     (int *) (&up_mnt_nof_bits)));
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SAT_TX_RATE_CONTROL, DBAL_FIELD_THRESHOLD_UPPER_EXPONENT, FALSE, 0, 0,
                     (int *) (&up_exp_nof_bits)));

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SAT_TX_RATE_CONTROL, DBAL_FIELD_THRESHOLD_LOWER_MANTISSA, FALSE, 0, 0,
                     (int *) (&low_mnt_nof_bits)));
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SAT_TX_RATE_CONTROL, DBAL_FIELD_THRESHOLD_LOWER_EXPONENT, FALSE, 0, 0,
                     (int *) (&low_exp_nof_bits)));

    SHR_IF_ERR_EXIT(utilex_break_to_mnt_exp_round_up
                    (is_pkt_mode ? config->high_threshold : (config->high_threshold * 125), up_mnt_nof_bits,
                     up_exp_nof_bits, 0, &high_mnt_val, &high_exp_val));
    SHR_IF_ERR_EXIT(utilex_break_to_mnt_exp_round_up
                    (is_pkt_mode ? config->low_threshold : (config->low_threshold * 125), low_mnt_nof_bits,
                     low_exp_nof_bits, 0, &low_mnt_val, &low_exp_val));

    if ((config->rate_pattern_mode == bcmSatGtfRatePatternContinues)
        || (config->rate_pattern_mode == bcmSatGtfRatePatternSimpleBurst))
    {
        field_data = 0;
    }
    else if (config->rate_pattern_mode == bcmSatGtfRatePatternCombined)
    {
        field_data = 1;
    }
    else if (config->rate_pattern_mode == bcmSatGtfRatePatternInterval)
    {
        field_data = 3;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Fail  rate_pattern_mode(%d) is not supported\n", config->rate_pattern_mode);
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_RATE_CONTROL, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_UPPER_MANTISSA, INST_SINGLE, high_mnt_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_UPPER_EXPONENT, INST_SINGLE, high_exp_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_LOWER_MANTISSA, INST_SINGLE, low_mnt_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_LOWER_EXPONENT, INST_SINGLE, low_exp_val);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BURST_MODE, INST_SINGLE, field_data);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_ITER_CNT, INST_SINGLE, config->stop_iter_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_BURST_CNT, INST_SINGLE,
                                 config->stop_burst_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_INTERVAL_CNT, INST_SINGLE,
                                 config->stop_interval_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BURST_PKT_WEIGHT, INST_SINGLE,
                                 config->burst_packet_weight);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERVAL_PKT_WEIGHT, INST_SINGLE,
                                 config->interval_packet_weight);

    if (config->flags & BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_INTRVAL_CNT_TO_CFG, INST_SINGLE, 1);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_INTRVAL_CNT_TO_CFG, INST_SINGLE, 0);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify  GTF rate pattern  for BCM-API: bcm_sat_gtf_rate_pattern_set*() .
* \par
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in ] config  -
*    Configure sat gtf rate pattern
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_rate_pattern_set_verify(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_rate_pattern_t * config)
{
    uint8 is_allocated = 0;
    int stop_iter_max = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config ");
    SHR_RANGE_VERIFY(gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
    SHR_RANGE_VERIFY(config->high_threshold, DNX_SAT_GTF_RATE_PATN_HIGH_TH_MIN,
                     DNX_SAT_GTF_RATE_PATN_HIGH_TH_MAX, _SHR_E_PARAM, "high_threshold");
    SHR_RANGE_VERIFY(config->low_threshold, DNX_SAT_GTF_RATE_PATN_LOW_TH_MIN, DNX_SAT_GTF_RATE_PATN_LOW_TH_MAX,
                     _SHR_E_PARAM, "low_threshold");

    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATGtfRateStopIter, &stop_iter_max));

    SHR_MAX_VERIFY(config->stop_iter_count, stop_iter_max, _SHR_E_PARAM, "stop_iter_count");

    /** check gtf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_allocated));

    /** return error if the gtf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: gtf_id  %d does not exist\n", gtf_id);
    }

    if (config->rate_pattern_mode == bcmSatGtfRatePatternContinues)
    {
        if (config->high_threshold != config->low_threshold)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Fail in continues mode, high_threshold(%d) should equals to low_threshold(%d)\n",
                         config->high_threshold, config->low_threshold);
        }
    }
    else if (config->rate_pattern_mode == bcmSatGtfRatePatternSimpleBurst)
    {
        if (config->high_threshold <= config->low_threshold)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Fail in simplebusrt mode, high_threshold(%d) should greater to low_threshold(%d)\n",
                         config->high_threshold, config->low_threshold);
        }
    }
    else if (config->rate_pattern_mode == bcmSatGtfRatePatternCombined)
    {
        if (config->high_threshold < config->low_threshold)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Fail in combined mode, high_threshold(%d) should greater or equal to low_threshold(%d)\n",
                         config->high_threshold, config->low_threshold);
        }
    }
    else if (config->rate_pattern_mode == bcmSatGtfRatePatternInterval)
    {
        if (config->high_threshold < config->low_threshold)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Fail in interval mode, high_threshold(%d) should greater or equal to low_threshold(%d)\n",
                         config->high_threshold, config->low_threshold);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Fail  rate_pattern_mode(%d) is not supported\n", config->rate_pattern_mode);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Set GTF rate pattern .
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] priority  -
*    SAT GTF Priority
* \param [in ] config  -
*    Configure sat gtf rate pattern
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_rate_pattern_set(
    int unit,
    bcm_sat_gtf_t gtf_id,
    int priority,
    bcm_sat_gtf_rate_pattern_t * config)
{
    bcm_sat_gtf_rate_pattern_t dnx_rate_pattern;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_rate_pattern_set_verify(unit, gtf_id, config));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d) priority(%d)\n"
                                 "  rate_pattern_mode(%d)\n"
                                 "  high_threshold(%d) low_threshold(%d)\n"
                                 "  stop_iter_count(%d) stop_burst_count(%d) stop_interval_count(%d)\n"
                                 "  burst_packet_weight(%d) interval_packet_weight(%d) flags(%d)\n", BSL_FUNC, BSL_LINE,
                                 unit, gtf_id, priority,
                                 config->rate_pattern_mode, config->high_threshold, config->low_threshold,
                                 config->stop_iter_count, config->stop_burst_count, config->stop_interval_count,
                                 config->burst_packet_weight, config->interval_packet_weight, config->flags));

    sal_memset(&dnx_rate_pattern, 0, sizeof(dnx_rate_pattern));

    if (config->flags & BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST)
    {
        dnx_rate_pattern.flags |= BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST;
    }
    dnx_rate_pattern.rate_pattern_mode = config->rate_pattern_mode;
    dnx_rate_pattern.high_threshold = config->high_threshold;
    dnx_rate_pattern.low_threshold = config->low_threshold;
    dnx_rate_pattern.stop_iter_count = config->stop_iter_count;
    dnx_rate_pattern.stop_burst_count = config->stop_burst_count;
    dnx_rate_pattern.stop_interval_count = config->stop_interval_count;
    dnx_rate_pattern.burst_packet_weight = config->burst_packet_weight;
    dnx_rate_pattern.interval_packet_weight = config->interval_packet_weight;

    SHR_IF_ERR_EXIT(dnx_sat_gtf_rate_pattern_set(unit, gtf_id, &dnx_rate_pattern));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   get GTF stat .
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] priority  -
*    SAT GTF Priority
* \param [in ] flags  -
*    Relevant flags
* \param [in ] type  -
*    SAT gtf counter types
* \param [in ] value  -
*    stat values
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_stat_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    int priority,
    uint32 flags,
    bcm_sat_gtf_stat_counter_t type,
    uint64 *value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "please use the multi get API (bcm_sat_gtf_stat_multi_get) ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Get gtf stat from HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] gtf_id -  gtf id.
* \param [out] packet_cnt -  packet cnt .
* \param [out] byte_cnt -  byte cnt .
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
shr_error_e
dnx_sat_gtf_stat_multi_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    uint64 *packet_cnt,
    uint64 *byte_cnt)
{
    uint32 entry_handle_id;
    uint32 packet_cnt_hw = 0;   /* transmitted packet in HW */
    uint32 byte_cnt_hw[2] = { 0 };      /* transmitted byte in HW */
    uint64 packet_cnt_hw_64 = COMPILER_64_INIT(0, 0);
    uint64 byte_cnt_hw_64 = COMPILER_64_INIT(0, 0);
    uint32 packet_cnt_sw[2] = { 0 };    /* packet Counter in SW */
    uint32 byte_cnt_sw[2] = { 0 };      /* byte packet Counter in SW */
    uint64 packet_cnt_sw_64 = COMPILER_64_INIT(0, 0);   /* packet Counter in SW */
    uint64 byte_cnt_sw_64 = COMPILER_64_INIT(0, 0);     /* byte packet Counter in SW */
    uint64 work_reg_64;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * STEP -1 Get the HW counters
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_STAT_GET, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);
    /** setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TX_PKT_COUNTERS, INST_SINGLE, &packet_cnt_hw);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_TX_BYTE_COUNTERS, INST_SINGLE, byte_cnt_hw);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    COMPILER_64_SET(byte_cnt_hw_64, byte_cnt_hw[1], byte_cnt_hw[0]);

    COMPILER_64_SET(packet_cnt_hw_64, 0, packet_cnt_hw);

    /*
     * STEP - 2 Get the SW counters
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_STAT_SW, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);
    /** setting value fields */
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_PKT_CNT, INST_SINGLE, packet_cnt_sw);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_BYTE_CNT, INST_SINGLE, byte_cnt_sw);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    COMPILER_64_SET(byte_cnt_sw_64, byte_cnt_sw[1], byte_cnt_sw[0]);
    COMPILER_64_SET(packet_cnt_sw_64, packet_cnt_sw[1], packet_cnt_sw[0]);

    /*
     * Implement:
     *   *packet_cnt = packet_cnt_sw_64 + packet_cnt_hw_64;
     */
    COMPILER_64_COPY(work_reg_64, packet_cnt_sw_64);
    COMPILER_64_ADD_64(work_reg_64, packet_cnt_hw_64);
    COMPILER_64_COPY(*packet_cnt, work_reg_64);
    /*
     * Implement:
     *   *byte_cnt = byte_cnt_sw_64 + byte_cnt_hw_64;
     */
    COMPILER_64_COPY(work_reg_64, byte_cnt_sw_64);
    COMPILER_64_ADD_64(work_reg_64, byte_cnt_hw_64);
    COMPILER_64_COPY(*byte_cnt, work_reg_64);

    /*
     * STEP - 3 Update the counters
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_STAT_SW, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);

    /** setting value fields */
    COMPILER_64_TO_32_HI(byte_cnt_sw[1], byte_cnt_sw_64);
    COMPILER_64_TO_32_LO(byte_cnt_sw[0], byte_cnt_sw_64);

    COMPILER_64_TO_32_HI(packet_cnt_sw[1], packet_cnt_sw_64);
    COMPILER_64_TO_32_LO(packet_cnt_sw[0], packet_cnt_sw_64);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PKT_CNT, INST_SINGLE, packet_cnt_sw);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_BYTE_CNT, INST_SINGLE, byte_cnt_sw);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify  GTF stat getting  for BCM-API: bcm_sat_gtf_stat_multi_get*() .
* \par
* \param [in] unit  -Relevant unit.
* \param [in] gtf_id  -GTF ID.
* \param [in ] stat  - stat values
 \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
* shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_gtf_stat_multi_get_verify(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_stat_multi_t * stat)
{
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(stat, _SHR_E_PARAM, "stat ");
    SHR_RANGE_VERIFY(gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_allocated));
    /** if gtf ID not  exists, return error*/
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: GTF with id %d not exists\n", gtf_id);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   get GTF stat .
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in ] stat  -
*    stat values
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_stat_multi_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    bcm_sat_gtf_stat_multi_t * stat)
{
    uint64 packet_cnt = COMPILER_64_INIT(0, 0); /* Counter transmitted packet */
    uint64 byte_cnt = COMPILER_64_INIT(0, 0);   /* Counter transmitted byte */

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_gtf_stat_multi_get_verify(unit, gtf_id, stat));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d) \n", BSL_FUNC, BSL_LINE, unit, gtf_id));

    SHR_IF_ERR_EXIT(dnx_sat_gtf_stat_multi_get(unit, gtf_id, &packet_cnt, &byte_cnt));

    COMPILER_64_COPY(stat->transmitted_packet_cnt, packet_cnt);
    COMPILER_64_COPY(stat->transmitted_byte_cnt, byte_cnt);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Callback function use to update gtf stat.
* \par
* \param [in] unit - Relevant unit.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
void
dnx_gtf_stat_update_callback(
    int unit)
{
    int rv = _SHR_E_NONE;
    uint64 packet_cnt = COMPILER_64_INIT(0, 0); /* Counter transmitted packet */
    uint64 byte_cnt = COMPILER_64_INIT(0, 0);   /* Counter transmitted byte */
    bcm_sat_ctf_t gtf_id;
    dnx_sat_data_t psat_data;
    uint8 is_gtf_allocated;

    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    rv = dnx_sat_data_get(unit, &psat_data);
    if (rv != _SHR_E_NONE)
    {
        return;
    }

    for (gtf_id = DNX_SAT_GTF_ID_MIN; gtf_id <= DNX_SAT_GTF_ID_MAX; gtf_id++)
    {
        /** check gtf_id is allocated/exsits */
        rv = algo_sat_db.sat_res_alloc_gtf_id.is_allocated(unit, gtf_id, &is_gtf_allocated);
        if (rv != _SHR_E_NONE)
        {
            return;
        }
        if (is_gtf_allocated)
        {
            /*
             * Invoke user callback.
             */
            rv = dnx_sat_gtf_stat_multi_get(unit, gtf_id, &packet_cnt, &byte_cnt);
            if (rv != _SHR_E_NONE)
            {
                return;
            }
        }
    }

    return;
}
