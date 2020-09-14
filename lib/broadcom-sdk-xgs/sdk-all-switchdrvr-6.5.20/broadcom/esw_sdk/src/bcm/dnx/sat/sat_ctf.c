/** \file sat_ctf.c
 * $Id$
 *
 * SAT procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
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
* To verify sat ctf create for BCM-API: bcm_sat_ctf_create*() .
* \par
* \param [in] unit - Relevant unit.
* \param [in] flags  -Relevant flags
* \param [in,out] ctf_id  - ctf_id
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_create_verify(
    int unit,
    uint32 flags,
    bcm_sat_ctf_t * ctf_id)
{
    uint8 is_allocated = 0;
    int legal_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ctf_id, _SHR_E_PARAM, "ctf_id ");

    legal_flags |= BCM_SAT_GTF_WITH_ID;
    if (flags & ~legal_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: unsupported flag combination\n");
    }

    /** check allocation if BCM_SAT_CTF_WITH_ID is present */
    if (flags & BCM_SAT_CTF_WITH_ID)
    {
        SHR_RANGE_VERIFY(*ctf_id, DNX_SAT_CTF_ID_MIN, DNX_SAT_CTF_ID_MAX, _SHR_E_PARAM, "ctf_id");
        SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.is_allocated(unit, *ctf_id, &is_allocated));
    }
    /** if gtf ID already exists, return error*/
    if (is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "Error: GTF with id %d exists\n", *ctf_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Create CTF id.
* \param [in] unit  -
*   Relevant unit.
* \param [in] flags  -
*   Relevant flags
* \param [out] ctf_id  -
*    ctf id
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_create(
    int unit,
    uint32 flags,
    bcm_sat_ctf_t * ctf_id)
{

    bcm_sat_ctf_report_config_t dnx_reports;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_create_verify(unit, flags, ctf_id));
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) ctf_id (%d) flags (%d)\n", BSL_FUNC, BSL_LINE, unit, (*ctf_id), flags));

    SHR_IF_ERR_EXIT(dnx_sat_ctf_id_allocate(unit, flags, ctf_id));
    /*
     *Do not generate cpu report by default.
     */
    sal_memset(&dnx_reports, 0, sizeof(bcm_sat_ctf_report_config_t));
    dnx_reports.report_sampling_percent = 0;
    SHR_IF_ERR_EXIT(dnx_sat_ctf_reports_config_set(unit, *ctf_id, &dnx_reports));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify sat ctf destroy for BCM-API: bcm_sat_ctf_destroy*() .
* \par
* \param [in] unit - Relevant unit.
* \param [in,out] ctf_id  - ctf_id
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_destroy_verify(
    int unit,
    bcm_sat_ctf_t ctf_id)
{
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_RANGE_VERIFY(ctf_id, DNX_SAT_CTF_ID_MIN, DNX_SAT_CTF_ID_MAX, _SHR_E_PARAM, "ctf_id");

    /** check ctf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.is_allocated(unit, ctf_id, &is_allocated));

    /** return error if the ctf_iddoes not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: ctf_id  %d does not exist\n", ctf_id);
    }
exit:
    SHR_FUNC_EXIT;
}

  /**
  * \brief
  *  Clear CTF dbal config in HW table .
  * \par
  *  \param [in] unit - Relevant unit.
  *  \param [in] ctf_id -  ctf_id.
  *  \param [in] table_id -  dbal_table_id.
  * \retval
  *  * Zero if no error was detected
  *   * Negative if error was detected. See \ref
  *     shr_error_e
  * \remark
  *  * None
  */
static shr_error_e
dnx_sat_ctf_dbal_destroy(
    int unit,
    bcm_sat_ctf_t ctf_id,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    dbal_fields_e key = DBAL_FIELD_EMPTY;
    uint8 key_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * For writing on dynamic tables (in the OAMP), have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_natural_id))
    {
        key = DBAL_FIELD_RX_FLOW_ID;
        key_val = ctf_id;
    }

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_odd_even_id))
    {
        if ((ctf_id % 2) == 0)
        {
            if (table_id == DBAL_TABLE_SAT_RX_FLOW_SET)
            {
                table_id = DBAL_TABLE_SAT_RX_FLOW_EVEN_SET;
            }
            else if (table_id == DBAL_TABLE_SAT_REPORT_CONFIG)
            {
                table_id = DBAL_TABLE_SAT_REPORT_EVEN_CONFIG;
            }
            else if (table_id == DBAL_TABLE_SAT_RX_PKT_PARAM_CONFIG)
            {
                table_id = DBAL_TABLE_SAT_RX_PKT_PARAM_EVEN_CONFIG;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: dbal table_id  %d does not exist\n", table_id);
            }
            key = DBAL_FIELD_EVEN_FLOW_ID;
        }
        else
        {
            if (table_id == DBAL_TABLE_SAT_RX_FLOW_SET)
            {
                table_id = DBAL_TABLE_SAT_RX_FLOW_ODD_SET;
            }
            else if (table_id == DBAL_TABLE_SAT_REPORT_CONFIG)
            {
                table_id = DBAL_TABLE_SAT_REPORT_ODD_CONFIG;
            }
            else if (table_id == DBAL_TABLE_SAT_RX_PKT_PARAM_CONFIG)
            {
                table_id = DBAL_TABLE_SAT_RX_PKT_PARAM_ODD_CONFIG;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: dbal table_id  %d does not exist\n", table_id);
            }
            key = DBAL_FIELD_ODD_FLOW_ID;
        }
        key_val = ctf_id / 2;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, key, key_val);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    /*
     * After done, disable dynamic memory access
     */
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Destory CTF id.
* \param [in] unit  -
*   Relevant unit.
* \param [in] ctf_id  -
*    ctf id
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_destroy(
    int unit,
    bcm_sat_ctf_t ctf_id)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_destroy_verify(unit, ctf_id));
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) ctf_id (%d)\n", BSL_FUNC, BSL_LINE, unit, ctf_id));

    SHR_IF_ERR_EXIT(dnx_sat_ctf_dbal_destroy(unit, ctf_id, DBAL_TABLE_SAT_RX_FLOW_SET));
    SHR_IF_ERR_EXIT(dnx_sat_ctf_dbal_destroy(unit, ctf_id, DBAL_TABLE_SAT_REPORT_CONFIG));
    SHR_IF_ERR_EXIT(dnx_sat_ctf_dbal_destroy(unit, ctf_id, DBAL_TABLE_SAT_RX_PKT_PARAM_CONFIG));
    SHR_IF_ERR_EXIT(dnx_sat_ctf_id_free(unit, ctf_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Destory all of CTF id.
* \param [in] unit  -
*   Relevant unit.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_destroy_all(
    int unit)
{
    bcm_sat_ctf_t ctf_id = 0;
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) \n", BSL_FUNC, BSL_LINE, unit));

    for (ctf_id = DNX_SAT_CTF_ID_MIN; ctf_id <= DNX_SAT_CTF_ID_MAX; ctf_id++)
    {
        /** check gtf_id is allocated/exsits */
        SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.is_allocated(unit, ctf_id, &is_allocated));
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_sat_ctf_dbal_destroy(unit, ctf_id, DBAL_TABLE_SAT_RX_FLOW_SET));
            SHR_IF_ERR_EXIT(dnx_sat_ctf_dbal_destroy(unit, ctf_id, DBAL_TABLE_SAT_REPORT_CONFIG));
            SHR_IF_ERR_EXIT(dnx_sat_ctf_dbal_destroy(unit, ctf_id, DBAL_TABLE_SAT_RX_PKT_PARAM_CONFIG));
            SHR_IF_ERR_EXIT(dnx_sat_ctf_id_free(unit, ctf_id));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   To get  max field value from HW table .
* \par
* \param [in] unit  -  Relevant unit.
* \param [in] type  - max value type.
* \param [out] max_value - max value.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
shr_error_e
dnx_sat_max_value_get(
    int unit,
    dnx_sat_max_value_type_t type,
    int *max_value)
{
    int nof_bits = 0;
    dbal_tables_e rx_param_table_id = DBAL_TABLE_EMPTY;
    dbal_tables_e report_table_id = DBAL_TABLE_EMPTY;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_natural_id))
    {
        rx_param_table_id = DBAL_TABLE_SAT_RX_PKT_PARAM_CONFIG;
        report_table_id = DBAL_TABLE_SAT_REPORT_CONFIG;
    }

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_odd_even_id))
    {
        rx_param_table_id = DBAL_TABLE_SAT_RX_PKT_PARAM_EVEN_CONFIG;
        report_table_id = DBAL_TABLE_SAT_REPORT_EVEN_CONFIG;
    }

    switch (type)
    {
        case dnxSATPayloadOffset:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, rx_param_table_id, DBAL_FIELD_PAYLOAD_OFFSET, FALSE, 0, 0, &nof_bits));
            break;
        case dnxSATSeqNumOffset:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, rx_param_table_id, DBAL_FIELD_SEQ_NUM_OFFSET, FALSE, 0, 0, &nof_bits));
            break;
        case dnxSATTimeStampOffset:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, rx_param_table_id, DBAL_FIELD_TIME_STAMP_OFFSET, FALSE, 0, 0, &nof_bits));
            break;
        case dnxSATCrcOffset:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, rx_param_table_id, DBAL_FIELD_CRC_OFFSET, FALSE, 0, 0, &nof_bits));
            break;
        case dnxSATGtfRateStopIter:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_SAT_TX_RATE_CONTROL, DBAL_FIELD_STOP_ITER_CNT, FALSE, 0, 0, &nof_bits));
            break;
        case dnxSATNumSlotForStateToSwitch:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_SAT_RX_AVARIBLE_SET, DBAL_FIELD_NUM_SLTS_FOR_STATS_TO_SWTICH, FALSE,
                             0, 0, &nof_bits));
            break;
        case dnxSATThersholdPerSlot:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_SAT_RX_AVARIBLE_SET, DBAL_FIELD_THRSH_IN_SLOT_TO_CHANGE_GOOD_BAD,
                             FALSE, 0, 0, &nof_bits));
            break;
        case dnxSATCtfTrapData:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_SAT_TRAP_SESSION_MAP, DBAL_FIELD_SESSION_MAP_KEY, TRUE, 0, 0, &nof_bits));
            break;
        case dnxSATCtfTrapDataMask:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_SAT_TRAP_SESSION_MAP, DBAL_FIELD_SESSION_MAP_KEY, TRUE, 0, 0, &nof_bits));
            break;
        case dnxSATCtfTrapSessionId:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_SAT_TRAP_SESSION_MAP, DBAL_FIELD_SESSION_MAP_ID, FALSE, 0, 0, &nof_bits));
            break;
        case dnxSATRportShreshold:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, report_table_id, DBAL_FIELD_REPORT_THREHOLD, FALSE, 0, 0, &nof_bits));
            break;
        case dnxSATCtfTrapId:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_SAT_RX_TRAP_CONFIG, DBAL_FIELD_RX_TRAP_CODE_0, FALSE, 0, 0, &nof_bits));
            break;
        case dnxSATGtfSeqNumOffset:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_SAT_TX_PKT_PARAM_CONFIG, DBAL_FIELD_SEQ_NUM_OFFSET, FALSE, 0, 0,
                             &nof_bits));
            break;
        case dnxSATGtfCrcOffset:
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_SAT_TX_PKT_PARAM_CONFIG, DBAL_FIELD_CRC_32_OFFSET, FALSE, 0, 0,
                             &nof_bits));
            break;
        default:
            break;
    }

    *max_value = (1 << nof_bits) - 1;
    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*    Ctf availability configuration Set in HW table.
* \par
* \param [in] unit - Relevant unit.
* \param [in] ctf_id -  ctf_id.
* \param [in] config - Sat ctf state for configuration.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_availability_config_set(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_availability_config_t * config)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_AVARIBLE_SET, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_SLTS_FOR_STATS_TO_SWTICH, INST_SINGLE,
                                 config->switch_state_num_of_slots);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRSH_IN_SLOT_TO_CHANGE_GOOD_BAD, INST_SINGLE,
                                 config->switch_state_threshold_per_slot);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_STATS_FLOW_ID, INST_SINGLE, ctf_id);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * (2) Reset state machine
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_AVARIBLE_SET, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RST_STATE_MCHN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_AVARIBLE_SET, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RST_STATE_MCHN, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify sat ctf availability config for BCM-API: bcm_sat_ctf_availability_config_set*() .
* \par
* \param [in] unit - Relevant unit.
* \param [in] ctf_id -  ctf_id.
* \param [in] config - Sat ctf state for configuration.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_availability_config_set_verify(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_availability_config_t * config)
{
    uint8 is_allocated = 0;
    int max_num_of_slot = 0;
    int max_state_thres_per_slot = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config ");
    SHR_RANGE_VERIFY(ctf_id, DNX_SAT_CTF_ID_MIN, DNX_SAT_CTF_ID_MAX, _SHR_E_PARAM, "ctf_id");

    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATNumSlotForStateToSwitch, &max_num_of_slot));
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATThersholdPerSlot, &max_state_thres_per_slot));

    SHR_MAX_VERIFY(config->switch_state_num_of_slots, max_num_of_slot, _SHR_E_PARAM, "switch_state_num_of_slots");
    SHR_MAX_VERIFY(config->switch_state_threshold_per_slot,
                   max_state_thres_per_slot, _SHR_E_PARAM, "switch_state_threshold_per_slot");
   /** check ctf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.is_allocated(unit, ctf_id, &is_allocated));

   /** return error if the ctf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: ctf_id  %d does not exist\n", ctf_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Ctf availability configuration Set.
*
* \par DIRECT INPUT:
*   \param [in] unit  -Relevant unit.
*   \param [in] ctf_id -SAT ctf id.
*   \param [in] config -SAT availability information.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_availability_config_set(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_availability_config_t * config)
{
    bcm_sat_ctf_availability_config_t avail_cfg;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_availability_config_set_verify(unit, ctf_id, config));

    LOG_VERBOSE(BSL_LS_BCM_SAT,
                ("%s %d: u(%d) ctf_id (%d) Number of slot (%d) Threshold per slot (%d)\n", BSL_FUNC, BSL_LINE, unit,
                 ctf_id, config->switch_state_num_of_slots, config->switch_state_threshold_per_slot));

    sal_memset(&avail_cfg, 0, sizeof(avail_cfg));
    avail_cfg.switch_state_num_of_slots = config->switch_state_num_of_slots;
    avail_cfg.switch_state_threshold_per_slot = config->switch_state_threshold_per_slot;

    SHR_IF_ERR_EXIT(dnx_sat_ctf_availability_config_set(unit, ctf_id, &avail_cfg));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *  Set ctf bin limitation in HW table.
 * \par
 *  \param [in] unit - Relevant unit.
 *  \param [in] max_bins_count -   max bins count.
 *  \param [out] bins_count - bins count number.
 *  \param [out] bins - Sat ctf bin limit .
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 *    shr_error_e
 * \remark
 *  * None
 */
static shr_error_e
dnx_sat_ctf_bin_limit_get(
    int unit,
    int max_bins_count,
    int *bins_count,
    bcm_sat_ctf_bin_limit_t * bins)
{
    uint32 entry_handle_id;
    int idx = 0;
    uint32 field = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** getting value fields */
    for (idx = 0; idx < max_bins_count; idx++)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_BIN_LIMIT, &entry_handle_id));
        switch (idx)
        {
            case dnxSATRxFLVBinLimit0:
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_0,
                                           INST_SINGLE, &field);
                break;
            case dnxSATRxFLVBinLimit1:
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_1,
                                           INST_SINGLE, &field);
                break;
            case dnxSATRxFLVBinLimit2:
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_2,
                                           INST_SINGLE, &field);
                break;
            case dnxSATRxFLVBinLimit3:
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_3,
                                           INST_SINGLE, &field);
                break;
            case dnxSATRxFLVBinLimit4:
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_4,
                                           INST_SINGLE, &field);
                break;
            case dnxSATRxFLVBinLimit5:
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_5,
                                           INST_SINGLE, &field);
                break;
            case dnxSATRxFLVBinLimit6:
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_6,
                                           INST_SINGLE, &field);
                break;
            case dnxSATRxFLVBinLimit7:
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_7,
                                           INST_SINGLE, &field);
                break;
            case dnxSATRxFLVBinLimit8:
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_8,
                                           INST_SINGLE, &field);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, " parameter error !!! \n");
        }
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        bins[idx].bin_select = idx;
        bins[idx].bin_limit = field;

        (*bins_count)++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 * To verify sat ctf bin limit config for BCM-API: bcm_dnx_sat_ctf_bin_limit_get*() .
 * \par
 * \param [in] unit  - Relevant unit.
 * \param [in] max_bins_count  -  Max bin count
 * \param [in] bins  -   Sat ctf bin limit
 * \param [out] bins_count  -  bins count
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 * shr_error_e
 * \remark
 * * None
 */
static shr_error_e
dnx_sat_ctf_bin_limit_get_verify(
    int unit,
    int max_bins_count,
    bcm_sat_ctf_bin_limit_t * bins,
    int *bins_count)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bins_count, _SHR_E_PARAM, "bins_count ");
    SHR_NULL_CHECK(bins, _SHR_E_PARAM, "bins ");
    SHR_RANGE_VERIFY(max_bins_count, DNX_SAT_CTF_BINS_LIMIT_CNT_MIN, DNX_SAT_CTF_BINS_LIMIT_CNT_MAX, _SHR_E_PARAM,
                     "max_bins_count");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get ctf bin limitation.
* \param [in] unit  -
*   Relevant unit.
* \param [in] max_bins_count  -
*   max bins count
* \param [in] bins  -
*    Sat ctf bin limit
* \param [out] bins_count  -
*    bins count
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_bin_limit_get(
    int unit,
    int max_bins_count,
    bcm_sat_ctf_bin_limit_t * bins,
    int *bins_count)
{
    bcm_sat_ctf_bin_limit_t dnx_bins_limit[DNX_SAT_CTF_BINS_LIMIT_CNT_MAX];
    int idx = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_bin_limit_get_verify(unit, max_bins_count, bins, bins_count));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) max_bins_count (%d)\n", BSL_FUNC, BSL_LINE, unit, max_bins_count));

    sal_memset(&dnx_bins_limit, 0, sizeof(dnx_bins_limit));
    SHR_IF_ERR_EXIT(dnx_sat_ctf_bin_limit_get(unit, max_bins_count, bins_count, dnx_bins_limit));

    for (idx = 0; idx < (*bins_count); idx++)
    {
        bins[idx].bin_select = dnx_bins_limit[idx].bin_select;
        bins[idx].bin_limit = dnx_bins_limit[idx].bin_limit;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *  Set ctf bin limitation in HW table.
 * \par
 *  \param [in] unit - Relevant unit.
 *  \param [in] bins_count - bins count number.
 *  \param [in] bins - Sat ctf bin limit .
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 *    shr_error_e
 * \remark
 *  * None
 */
static shr_error_e
dnx_sat_ctf_bin_limit_set(
    int unit,
    int bins_count,
    bcm_sat_ctf_bin_limit_t * bins)
{
    uint32 entry_handle_id;
    int idx = 0;
    uint32 field = 0;
    uint32 bin_select = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** getting value fields */
    for (idx = 0; idx < bins_count; idx++)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_BIN_LIMIT, &entry_handle_id));

        field = bins[idx].bin_limit;
        bin_select = bins[idx].bin_select;
        switch (bin_select)
        {
            case dnxSATRxFLVBinLimit0:
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_0,
                                             INST_SINGLE, field);
                break;
            case dnxSATRxFLVBinLimit1:
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_1,
                                             INST_SINGLE, field);
                break;
            case dnxSATRxFLVBinLimit2:
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_2,
                                             INST_SINGLE, field);
                break;
            case dnxSATRxFLVBinLimit3:
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_3,
                                             INST_SINGLE, field);
                break;
            case dnxSATRxFLVBinLimit4:
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_4,
                                             INST_SINGLE, field);
                break;
            case dnxSATRxFLVBinLimit5:
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_5,
                                             INST_SINGLE, field);
                break;
            case dnxSATRxFLVBinLimit6:
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_6,
                                             INST_SINGLE, field);
                break;
            case dnxSATRxFLVBinLimit7:
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_7,
                                             INST_SINGLE, field);
                break;
            case dnxSATRxFLVBinLimit8:
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIMIT_FLV_BIN_DELAY_COUNTERS_8,
                                             INST_SINGLE, field);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, " parameter error !!! \n");
        }
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 * To verify sat ctf bin limit config for BCM-API: bcm_dnx_sat_ctf_bin_limit_set*() .
 * \par
 * \param [in] unit  - Relevant unit.
 * \param [in] bins  -  Sat ctf bin limit
 * \param [out] bins_count  -  bins count
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 * shr_error_e
 * \remark
 * * None
 */
static shr_error_e
dnx_sat_ctf_bin_limit_set_verify(
    int unit,
    int bins_count,
    bcm_sat_ctf_bin_limit_t * bins)
{
    int idx = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bins, _SHR_E_PARAM, "bins ");
    SHR_RANGE_VERIFY(bins_count, DNX_SAT_CTF_BINS_LIMIT_CNT_MIN, DNX_SAT_CTF_BINS_LIMIT_CNT_MAX, _SHR_E_PARAM,
                     "bins_count");
    for (idx = 0; idx < bins_count; idx++)
    {
        SHR_MAX_VERIFY(bins[idx].bin_select, dnxSATRxFLVBinLimit8, _SHR_E_PARAM, "bin_select");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set ctf bin limitation.
* \param [in] unit  -
*   Relevant unit.
* \param [in] bins_count  -
*   bins count
* \param [in] bins  -
*    Sat ctf bin limit
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_bin_limit_set(
    int unit,
    int bins_count,
    bcm_sat_ctf_bin_limit_t * bins)
{
    bcm_sat_ctf_bin_limit_t dnx_bins_limit[DNX_SAT_CTF_BINS_LIMIT_CNT_MAX];
    int idx = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    sal_memset(&dnx_bins_limit, 0, sizeof(dnx_bins_limit));
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_bin_limit_set_verify(unit, bins_count, bins));
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) bins_count (%d) bin_select (%d) bin_limit (%d)\n", BSL_FUNC, BSL_LINE,
                                 unit, bins_count, bins->bin_select, bins->bin_limit));

    for (idx = 0; idx < bins_count; idx++)
    {
        SHR_MAX_VERIFY(bins[idx].bin_select, DNX_SAT_CTF_BINS_LIMIT_CNT_MAX, _SHR_E_PARAM, "bin_select");
        dnx_bins_limit[idx].bin_select = bins[idx].bin_select;
        dnx_bins_limit[idx].bin_limit = bins[idx].bin_limit;
    }
    SHR_IF_ERR_EXIT(dnx_sat_ctf_bin_limit_set(unit, bins_count, dnx_bins_limit));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*    find a  trap id.
* \par
* \param [in] unit  -Relevant unit.
* \param [in] psat_data - SAT sw data.
* \param [in] trap_id - trap_id.
* \param [out] trap_idx - trap_idx.
* \retval
*   * Zero if no error was detected
*   * retval _SHR_E_NOT_FOUND if not found
* \remark
* * None
*/
STATIC shr_error_e
dnx_sat_trap_id_find(
    int unit,
    dnx_sat_data_t * psat_data,
    uint32 trap_id,
    int32 *trap_idx)
{
    uint8 is_allocated = 0;
    int idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (idx = 0; idx < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_ID_ENTRIES; idx++)
    {
        SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_trap_id.is_allocated(unit, idx, &is_allocated));
        /** if trap ID already exists*/
        if (trap_id == psat_data->ctf_trap_id[idx] && is_allocated)
        {
            break;
        }
    }

    if (idx == DNX_DATA_MAX_SAT_COLLECTOR_TRAP_ID_ENTRIES)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }
    else
    {
        *trap_idx = idx;
    }
    /*
     * Do not print any error message here - error or not should be decided by caller
     */
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*    find a  trap entry.
* \par
* \param [in] unit  -Relevant unit.
* \param [in] psat_data - SAT sw data.
* \param [in] trap_data - trap_data.
* \param [in] trap_mask - trap_mask.
* \param [out] entry_idx - entry_idx.
* \retval
*   * Zero if no error was detected
*   * _SHR_E_NOT_FOUND if not found
* \remark
* * None
*/
STATIC shr_error_e
dnx_sat_ctf_trap_entry_find(
    int unit,
    dnx_sat_data_t * psat_data,
    uint32 trap_data,
    uint32 trap_mask,
    uint32 *entry_idx)
{
    int idx = 0;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    /*
     * Check the entry whether is existed
     */
    for (idx = 0; idx < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_DATA_ENTRIES; idx++)
    {
        if (psat_data->trap_data[idx].trap_data == trap_data && psat_data->trap_data[idx].trap_mask == trap_mask)
        {
            SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_trap_data.is_allocated(unit, idx, &is_allocated));
            if (is_allocated)
            {
                *entry_idx = idx;
                SHR_SET_CURRENT_ERR(_SHR_E_EXISTS);
                break;
            }
        }
    }
    /*
     * We do not know what result will be considered an error by caller - so no error messages here - only return
     */
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*    convert packet config payload type from unit8 to long.
* \par
* \param [in] exp_payload_u8 - packet payload (uint8 ).
* \param [in] num_of_bytes -  bytes length.
* \param [in] is_reverse - Big endian and Little endian.
* \param [out] exp_payload_long - packet payload (long ).
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
shr_error_e
dnx_sat_packet_config_exp_payload_uint8_to_long(
    uint8 exp_payload_u8[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8],
    uint8 num_of_bytes,
    uint32 is_reverse,
    uint32 exp_payload_long[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U32])
{
    uint32 tmp;
    uint32 char_indx, long_indx, write_to;
    uint32 first_word = 0, second_word = 1, last_word = 0;

    if (num_of_bytes > DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8)
    {
        return _SHR_E_PARAM;
    }

    last_word = (num_of_bytes > (DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8 / 2)) ? second_word : first_word;
    write_to = (is_reverse ? (sizeof(uint32) - 1) : 0);
    exp_payload_long[first_word] = 0;
    exp_payload_long[second_word] = 0;
    long_indx = (is_reverse ? last_word : first_word);

    for (char_indx = 0; char_indx < num_of_bytes; ++char_indx)
    {
        if (char_indx == sizeof(uint32))
        {
            long_indx = (is_reverse ? 0 : 1);
            write_to = (is_reverse ? (sizeof(uint32) - 1) : 0);
        }

        tmp = exp_payload_u8[char_indx];
        exp_payload_long[long_indx] |= (tmp << (SAL_CHAR_BIT * write_to));
        write_to = is_reverse ? (write_to - 1) : (write_to + 1);
    }

    return _SHR_E_NONE;
}

/**
* \brief
*    convert packet config payload type from long to uint8.
* \par
* \param [in] exp_payload_long - packet payload (long ).
* \param [in] num_of_bytes -  bytes length.
* \param [in] is_reverse - Big endian and Little endian.
* \param [out] exp_payload_u8 - packet payload (uint8 ).
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/

shr_error_e
dnx_sat_packet_config_exp_payload_long_to_uint8(
    uint8 exp_payload_u8[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8],
    uint8 num_of_bytes,
    uint32 is_reverse,
    uint32 exp_payload_long[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U32])
{
    uint32 char_indx, long_indx, write_to;
    uint32 first_word = 0, second_word = 1, last_word = 0;

    if (num_of_bytes > DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8)
    {
        return _SHR_E_PARAM;
    }

    last_word = (num_of_bytes > (DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8 / 2)) ? second_word : first_word;
    write_to = (is_reverse ? (sizeof(uint32) - 1) : 0);
    exp_payload_long[first_word] = 0;
    exp_payload_long[second_word] = 0;
    long_indx = (is_reverse ? last_word : first_word);

    for (char_indx = 0; char_indx < num_of_bytes; ++char_indx)
    {
        if (char_indx == sizeof(uint32))
        {
            long_indx = (is_reverse ? 0 : 1);
            write_to = (is_reverse ? (sizeof(uint32) - 1) : 0);
        }
        exp_payload_u8[char_indx] = (exp_payload_long[long_indx] >> (SAL_CHAR_BIT * write_to)) & 0xFF;
        write_to = is_reverse ? (write_to - 1) : (write_to + 1);
    }

    return _SHR_E_NONE;
}

/**
* \brief
*    OAMP dynamic memory access set in HW .
* \par
* \param [in] unit - Relevant unit.
* \param [in] enable -  enable or disable.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
shr_error_e
dnx_sat_dynamic_memory_access_set(
    int unit,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(enable, 2, _SHR_E_PARAM, "enable");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_DYNAMIC_MEMORY_ACCESS_SET, &entry_handle_id));

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DYNAMIC_MEMORY_ACCESS_ENABLE, INST_SINGLE, enable);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *  CTF packet config set in HW table .
 * \par
 *  \param [in] unit - Relevant unit.
 *  \param [in] ctf_id -  ctf_id.
 *  \param [in] packet_info - Sat ctf packet config .
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 *    shr_error_e
 * \remark
 *  * None
 */
static shr_error_e
dnx_sat_ctf_packet_config(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_packet_info_t * packet_info)
{
    uint32 entry_handle_id;
    uint32 exp_rpt_pattern_32[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U32] = { 0 };
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    dbal_fields_e key = DBAL_FIELD_EMPTY;
    uint8 key_val = 0;
    uint32 field = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * For writing on dynamic tables (in the OAMP), have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_natural_id))
    {
        table_id = DBAL_TABLE_SAT_RX_PKT_PARAM_CONFIG;
        key = DBAL_FIELD_RX_FLOW_ID;
        key_val = ctf_id;
    }

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_odd_even_id))
    {
        if ((ctf_id % 2) == 0)
        {
            table_id = DBAL_TABLE_SAT_RX_PKT_PARAM_EVEN_CONFIG;
            key = DBAL_FIELD_EVEN_FLOW_ID;
        }
        else
        {
            table_id = DBAL_TABLE_SAT_RX_PKT_PARAM_ODD_CONFIG;
            key = DBAL_FIELD_ODD_FLOW_ID;
        }
        key_val = ctf_id / 2;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, key, key_val);

    /** setting value fields */
    /*
     * PAYLOAD_OFFSET 
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PAYLOAD_OFFSET, INST_SINGLE,
                                 packet_info->offsets.payload_offset);
    /*
     * CRC_OFFSET
     */
    field = (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC) ? packet_info->offsets.crc_byte_offset : 0;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_OFFSET, INST_SINGLE, field);

    /*
     * SEQ_NUM_OFFSET
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEQ_NUM_OFFSET, INST_SINGLE,
                                 packet_info->offsets.seq_number_offset);

    /*
     * EXP_RPT_PATTERN
     */
    if (packet_info->payload.payload_type == bcmSatPayloadConstant4Bytes)
    {
        SHR_IF_ERR_EXIT(dnx_sat_packet_config_exp_payload_uint8_to_long
                        (packet_info->payload.payload_pattern, DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8 / 2, 1,
                         exp_rpt_pattern_32));
    }
    else if (packet_info->payload.payload_type == bcmSatPayloadConstant8Bytes)
    {
        SHR_IF_ERR_EXIT(dnx_sat_packet_config_exp_payload_uint8_to_long
                        (packet_info->payload.payload_pattern, DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8, 1,
                         exp_rpt_pattern_32));
    }
    /*
     * exp_rpt_pattern_64 = ((exp_rpt_pattern_32[1] <<32) | exp_rpt_pattern_32[0]);
     */
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_EXP_RPT_PATTERN, INST_SINGLE,
                                     exp_rpt_pattern_32);

    /*
     * ADD_END_TLV
     */
    field = (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV) ? 1 : 0;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADD_END_TLV, INST_SINGLE, field);

    /*
     * ADD_CRC
     */
    field = (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC) ? 1 : 0;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADD_CRC, INST_SINGLE, field);

    /*
     * TIME_STAMP_OFFSET
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TIME_STAMP_OFFSET, INST_SINGLE,
                                 packet_info->offsets.timestamp_offset);
    if (packet_info->payload.payload_type == bcmSatPayloadConstant4Bytes)
    {
        /*
         * USE_RPT_PTRN_PAYLOAD
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_RPT_PTRN_PAYLOAD, INST_SINGLE, 1);
        /*
         * USE_4_BYTE_PATTERN
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_4_BYTE_PATTERN, INST_SINGLE, 1);
        /*
         * USE_PRBS_PAYLOAD
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_PRBS_PAYLOAD, INST_SINGLE, 0);
    }
    else if (packet_info->payload.payload_type == bcmSatPayloadPRBS)
    {
        /*
         * USE_RPT_PTRN_PAYLOAD
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_RPT_PTRN_PAYLOAD, INST_SINGLE, 0);
        /*
         * USE_4_BYTE_PATTERN
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_4_BYTE_PATTERN, INST_SINGLE, 0);
        /*
         * USE_PRBS_PAYLOAD
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_PRBS_PAYLOAD, INST_SINGLE, 1);
    }
    else if (packet_info->payload.payload_type == bcmSatPayloadConstant8Bytes)
    {
        /*
         * USE_RPT_PTRN_PAYLOAD
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_RPT_PTRN_PAYLOAD, INST_SINGLE, 1);
        /*
         * USE_4_BYTE_PATTERN
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_4_BYTE_PATTERN, INST_SINGLE, 0);
        /*
         * USE_PRBS_PAYLOAD
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_PRBS_PAYLOAD, INST_SINGLE, 0);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Reset ExpSeqNumber as 0
     */
    /** Taking a handle */
    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_natural_id))
    {
        table_id = DBAL_TABLE_SAT_RX_EXP_SEQ_NUMBER_GET;
        key = DBAL_FIELD_RX_FLOW_ID;
        key_val = ctf_id;
    }

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_odd_even_id))
    {
        if ((ctf_id % 2) == 0)
        {
            table_id = DBAL_TABLE_SAT_RX_EXP_SEQ_NUMBER_EVEN_GET;
            key = DBAL_FIELD_EVEN_FLOW_ID;
        }
        else
        {
            table_id = DBAL_TABLE_SAT_RX_EXP_SEQ_NUMBER_ODD_GET;
            key = DBAL_FIELD_ODD_FLOW_ID;
        }
        key_val = ctf_id / 2;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, key, key_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP_SEQ_NUMBER, INST_SINGLE, 0);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:

    /*
     * After done, disable dynamic memory access
     */
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 * To verify sat ctf packet configuration for BCM-API: bcm_sat_ctf_packet_config*() .
 * \par
 * \param [in] unit  - Relevant unit.
  * \param [in] ctf_id  - ctf id
 * \param [in] packet_info - ctf packet information
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 * shr_error_e
 * \remark
 * * None
 */
static shr_error_e
dnx_sat_ctf_packet_config_verify(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_packet_info_t * packet_info)
{
    uint8 is_allocated = 0;
    int payload_offset_max = 0;
    int seq_num_offset_max = 0;
    int time_stamp_offset_max = 0;
    int crc_byte_offset_max = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info ");

    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATPayloadOffset, &payload_offset_max));
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATSeqNumOffset, &seq_num_offset_max));
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATTimeStampOffset, &time_stamp_offset_max));
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATCrcOffset, &crc_byte_offset_max));

    SHR_RANGE_VERIFY(ctf_id, DNX_SAT_CTF_ID_MIN, DNX_SAT_CTF_ID_MAX, _SHR_E_PARAM, "ctf_id");
    SHR_RANGE_VERIFY(packet_info->sat_header_type, bcmSatHeaderUserDefined, bcmSatHeadersCount, _SHR_E_PARAM,
                     "sat_header_type");
    SHR_RANGE_VERIFY(packet_info->payload.payload_type, bcmSatPayloadConstant8Bytes, bcmSatPayloadsCount - 1,
                     _SHR_E_PARAM, "payload_type");
    SHR_RANGE_VERIFY(packet_info->offsets.payload_offset, 0, payload_offset_max, _SHR_E_PARAM, "payload_offset");
    SHR_RANGE_VERIFY(packet_info->offsets.seq_number_offset, 0, seq_num_offset_max, _SHR_E_PARAM, "seq_number_offset");
    SHR_RANGE_VERIFY(packet_info->offsets.timestamp_offset, 0, time_stamp_offset_max, _SHR_E_PARAM, "timestamp_offset");
    if (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC)
    {
        SHR_RANGE_VERIFY(packet_info->offsets.crc_byte_offset, 0, crc_byte_offset_max, _SHR_E_PARAM, "crc_byte_offset");
    }
    /** check ctf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.is_allocated(unit, ctf_id, &is_allocated));

    /** return error if the ctf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: ctf_id  %d does not exist\n", ctf_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  CTF packet information configure.
* \param [in] unit  -
*   Relevant unit.
* \param [in] ctf_id  -
*    ctf id
* \param [in] packet_info  -
*    ctf packet information
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_packet_config(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_packet_info_t * packet_info)
{
    bcm_sat_ctf_packet_info_t dnx_packet_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_packet_config_verify(unit, ctf_id, packet_info));

    LOG_VERBOSE(BSL_LS_BCM_SAT,
                ("%s %d: u(%d) ctf_id (%d) sat_header_type (%d) payload_type (%d)  payload_offset (%d)  ,seq_number_offset  (%d) ,timestamp_offset (%d) \n",
                 BSL_FUNC, BSL_LINE, unit, ctf_id, packet_info->sat_header_type, packet_info->payload.payload_type,
                 packet_info->offsets.payload_offset, packet_info->offsets.seq_number_offset,
                 packet_info->offsets.timestamp_offset));

    sal_memset(&dnx_packet_info, 0, sizeof(dnx_packet_info));

    if (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV)
    {
        dnx_packet_info.flags |= BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
    }
    if (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC)
    {
        dnx_packet_info.flags |= BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
    }
    dnx_packet_info.sat_header_type = packet_info->sat_header_type;
    dnx_packet_info.payload.payload_type = packet_info->payload.payload_type;
    sal_memcpy(dnx_packet_info.payload.payload_pattern, packet_info->payload.payload_pattern,
               sizeof(uint8) * BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE);
    if (packet_info->sat_header_type == bcmSatHeaderUserDefined)
    {
        dnx_packet_info.offsets.payload_offset = packet_info->offsets.payload_offset;
        dnx_packet_info.offsets.seq_number_offset = packet_info->offsets.seq_number_offset;
        dnx_packet_info.offsets.timestamp_offset = packet_info->offsets.timestamp_offset;
        if (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC)
        {
            dnx_packet_info.offsets.crc_byte_offset = packet_info->offsets.crc_byte_offset;
        }
    }
    else if (packet_info->sat_header_type == bcmSatHeaderY1731)
    {
        dnx_packet_info.offsets.payload_offset =
            (packet_info->offsets.payload_offset ? packet_info->
             offsets.payload_offset : DNX_DATA_MAX_SAT_COLLECTOR_Y1731_PAYLOAD_OFFSET);
        dnx_packet_info.offsets.seq_number_offset =
            (packet_info->offsets.seq_number_offset ? packet_info->
             offsets.seq_number_offset : DNX_DATA_MAX_SAT_COLLECTOR_Y1731_SEQ_NUM_OFFSET);
        dnx_packet_info.offsets.timestamp_offset =
            (packet_info->offsets.timestamp_offset ? packet_info->
             offsets.timestamp_offset : DNX_DATA_MAX_SAT_COLLECTOR_Y1731_TIME_STAMP_OFFSET);
        if (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC)
        {
            dnx_packet_info.offsets.crc_byte_offset = (packet_info->offsets.crc_byte_offset ?
                                                       packet_info->offsets.crc_byte_offset :
                                                       DNX_DATA_MAX_SAT_COLLECTOR_Y1731_CRC_BYTE_OFFSET);
        }
    }
    else if (packet_info->sat_header_type == bcmSatHeaderMEF)
    {
        dnx_packet_info.offsets.payload_offset =
            (packet_info->offsets.payload_offset ? packet_info->
             offsets.payload_offset : DNX_DATA_MAX_SAT_COLLECTOR_MEF_PAYLOAD_OFFSET);
        dnx_packet_info.offsets.seq_number_offset =
            (packet_info->offsets.seq_number_offset ? packet_info->
             offsets.seq_number_offset : DNX_DATA_MAX_SAT_COLLECTOR_MEF_SEQ_NUM_OFFSET);
        dnx_packet_info.offsets.timestamp_offset =
            (packet_info->offsets.timestamp_offset ? packet_info->
             offsets.timestamp_offset : DNX_DATA_MAX_SAT_COLLECTOR_MEF_TIME_STAMP_OFFSET);
        if (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC)
        {
            dnx_packet_info.offsets.crc_byte_offset = (packet_info->offsets.crc_byte_offset ?
                                                       packet_info->offsets.crc_byte_offset :
                                                       DNX_DATA_MAX_SAT_COLLECTOR_MEF_CRC_BYTE_OFFSET);
        }
    }
    SHR_IF_ERR_EXIT(dnx_sat_ctf_packet_config(unit, ctf_id, &dnx_packet_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *  CTF identifier map in HW table .
 * \par
 *  \param [in] unit - Relevant unit.
 *  \param [in] ctf_id -  ctf_id.
 *  \param [in] identifier - Sat ctf is recognized by this parameters .
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 *    shr_error_e
 * \remark
 *  * None
 */
static shr_error_e
dnx_sat_ctf_identifier_map(
    int unit,
    bcm_sat_ctf_identifier_t * identifier,
    bcm_sat_ctf_t ctf_id)
{
    uint32 entry_handle_id;
    uint32 identifier_valid = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * For writing on dynamic tables (in the OAMP), have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    /*
     * 1) Checking the status of identifier is valid
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_FLOW_IDENTIFY_MAP, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, identifier->tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COLOR, identifier->color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_ID, identifier->trap_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SAT_SESSION_ID, identifier->session_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FLOW_IS_VALID, INST_SINGLE, &identifier_valid);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (identifier_valid)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "CTF ID exist !!! ");
    }

    /*
     * 2) setting the status of identifier
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_FLOW_IDENTIFY_MAP, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, identifier->tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COLOR, identifier->color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_ID, identifier->trap_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SAT_SESSION_ID, identifier->session_id);
    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_ID_TO_READ, INST_SINGLE, ctf_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_IS_VALID, INST_SINGLE, 1);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:

    /*
     * After done, disable dynamic memory access
     */
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 * To verify CTF identifier map for BCM-API: bcm_sat_ctf_identifier_map*() .
 * \par
 * \param [in] unit  - Relevant unit.
  * \param [in] ctf_id  - ctf id
 * \param [in] identifier  - ctf identifier information
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 *    shr_error_e
 * \remark
 * * None
 */
static shr_error_e
dnx_sat_ctf_identifier_map_verify(
    int unit,
    bcm_sat_ctf_identifier_t * identifier,
    bcm_sat_ctf_t ctf_id)
{
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(identifier, _SHR_E_PARAM, "identifier ");
    SHR_RANGE_VERIFY(ctf_id, DNX_SAT_CTF_ID_MIN, DNX_SAT_CTF_ID_MAX, _SHR_E_PARAM, "ctf_id");
    SHR_MAX_VERIFY(identifier->session_id, DNX_SAT_CTF_OAM_ID_MAX, _SHR_E_PARAM, "session_id");
    SHR_MAX_VERIFY(identifier->trap_id, DNX_SAT_CTF_TRAP_ID_MAX, _SHR_E_PARAM, "trap_id");
    SHR_MAX_VERIFY(identifier->color, DNX_SAT_CTF_COLOR_MAX, _SHR_E_PARAM, "color");
    SHR_MAX_VERIFY(identifier->tc, DNX_SAT_CTF_COS_MAX, _SHR_E_PARAM, "tc");

   /** check ctf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.is_allocated(unit, ctf_id, &is_allocated));

   /** return error if the ctf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: ctf_id  %d does not exist\n", ctf_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  CTF identifier map.
* \param [in] unit  -
*   Relevant unit.
* \param [in] identifier  -
*    ctf identifier information
* \param [in] ctf_id  -
*    ctf id
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_identifier_map(
    int unit,
    bcm_sat_ctf_identifier_t * identifier,
    bcm_sat_ctf_t ctf_id)
{

    bcm_sat_ctf_identifier_t dnx_identifier;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_identifier_map_verify(unit, identifier, ctf_id));
    LOG_VERBOSE(BSL_LS_BCM_SAT,
                ("%s %d: u(%d) ctf_id (%d) tc (%d) color(%d)  trap_id (%d) session_id(%d)\n", BSL_FUNC, BSL_LINE, unit,
                 ctf_id, identifier->tc, identifier->color, identifier->trap_id, identifier->session_id));

    sal_memset(&dnx_identifier, 0, sizeof(dnx_identifier));
    dnx_identifier.session_id = identifier->session_id;
    dnx_identifier.trap_id = identifier->trap_id;
    dnx_identifier.color = identifier->color;
    dnx_identifier.tc = identifier->tc;
    SHR_IF_ERR_EXIT(dnx_sat_ctf_identifier_map(unit, &dnx_identifier, ctf_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *  CTF identifier unmap in HW table .
 * \par
 *  \param [in] unit - Relevant unit.
 *  \param [in] identifier - Sat ctf is recognized by this parameters .
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 *    shr_error_e
 * \remark
 *  * None
 */
static shr_error_e
dnx_sat_ctf_identifier_unmap(
    int unit,
    bcm_sat_ctf_identifier_t * identifier)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_FLOW_IDENTIFY_MAP, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, identifier->tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COLOR, identifier->color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_ID, identifier->trap_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SAT_SESSION_ID, identifier->session_id);
    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_ID_TO_READ, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_IS_VALID, INST_SINGLE, 0);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 * To verify CTF identifier unmap for BCM-API: bcm_sat_ctf_identifier_unmap*() .
 * \par
 * \param [in] unit  - Relevant unit.
 * \param [in] identifier  - ctf identifier information
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 *    shr_error_e
 * \remark
 * * None
 */
static shr_error_e
dnx_sat_ctf_identifier_unmap_verify(
    int unit,
    bcm_sat_ctf_identifier_t * identifier)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(identifier, _SHR_E_PARAM, "identifier ");
    SHR_MAX_VERIFY(identifier->session_id, DNX_SAT_CTF_OAM_ID_MAX, _SHR_E_PARAM, "session_id");
    SHR_MAX_VERIFY(identifier->trap_id, DNX_SAT_CTF_TRAP_ID_MAX, _SHR_E_PARAM, "trap_id");
    SHR_MAX_VERIFY(identifier->color, DNX_SAT_CTF_COLOR_MAX, _SHR_E_PARAM, "color");
    SHR_MAX_VERIFY(identifier->tc, DNX_SAT_CTF_COS_MAX, _SHR_E_PARAM, "tc");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  CTF identifier unmap.
* \param [in] unit  -
*   Relevant unit.
* \param [in] identifier  -
*    ctf identifier information
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_identifier_unmap(
    int unit,
    bcm_sat_ctf_identifier_t * identifier)
{
    bcm_sat_ctf_identifier_t dnx_identifier;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_identifier_unmap_verify(unit, identifier));
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) tc (%d) color(%d)  trap_id (%d) session_id(%d)\n", BSL_FUNC, BSL_LINE,
                                 unit, identifier->tc, identifier->color, identifier->trap_id, identifier->session_id));

    sal_memset(&dnx_identifier, 0, sizeof(dnx_identifier));
    dnx_identifier.session_id = identifier->session_id;
    dnx_identifier.trap_id = identifier->trap_id;
    dnx_identifier.color = identifier->color;
    dnx_identifier.tc = identifier->tc;

    SHR_IF_ERR_EXIT(dnx_sat_ctf_identifier_unmap(unit, &dnx_identifier));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *  CTF report config in HW table .
 * \par
 *  \param [in] unit - Relevant unit.
 *  \param [in] ctf_id -  ctf_id.
 *  \param [in] reports - Sat ctf report object .
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 *    shr_error_e
 * \remark
 *  * None
 */
shr_error_e
dnx_sat_ctf_reports_config_set(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_report_config_t * reports)
{
    uint32 entry_handle_id;
    uint32 field = 0;
    int max_report_shreshold = DNX_SAT_CTF_MAX_REPORT_SHRESHOLD;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    dbal_fields_e key = DBAL_FIELD_EMPTY;
    uint8 key_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * For writing on dynamic tables (in the OAMP), have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATRportShreshold, &max_report_shreshold));

    /*
     * TIME_STAMP_OFFSETf
     */
    if (reports->report_sampling_percent == 0)
    {
        /*
         * never report
         */
        field = (uint32) max_report_shreshold;
    }
    else if (reports->report_sampling_percent == DNX_SAT_CTF_REPORT_SAMPLING_PER_MAX)
    {
        /*
         * always report
         */
        field = 0;
    }
    else
    {
        field =
            max_report_shreshold -
            ((reports->report_sampling_percent * (uint32) max_report_shreshold) / DNX_SAT_CTF_REPORT_SAMPLING_PER_MAX);
    }

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_natural_id))
    {
        table_id = DBAL_TABLE_SAT_REPORT_CONFIG;
        key = DBAL_FIELD_RX_FLOW_ID;
        key_val = ctf_id;
    }

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_odd_even_id))
    {
        if ((ctf_id % 2) == 0)
        {
            table_id = DBAL_TABLE_SAT_REPORT_EVEN_CONFIG;
            key = DBAL_FIELD_EVEN_FLOW_ID;
        }
        else
        {
            table_id = DBAL_TABLE_SAT_REPORT_ODD_CONFIG;
            key = DBAL_FIELD_ODD_FLOW_ID;
        }
        key_val = ctf_id / 2;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, key, key_val);

    /** setting key fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REPORT_THREHOLD, INST_SINGLE, field);

    if (reports->flags & BCM_SAT_CTF_REPORT_ADD_SEQ_NUM)
    {
        field = 1;
    }
    else
    {
        field = 0;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REPORT_PKT_SEQ_NUM, INST_SINGLE, field);

    if (reports->flags & BCM_SAT_CTF_REPORT_ADD_DELAY)
    {
        field = 1;
    }
    else
    {
        field = 0;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REPORT_PKT_DELAY, INST_SINGLE, field);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    /*
     * After done, disable dynamic memory access
     */
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 * To verify CTF report configure. for BCM-API: bcm_sat_ctf_reports_config_set*() .
 * \par
 * \param [in] unit  - Relevant unit.
 * \param [in] ctf_id  - ctf id
 * \param [in] reports - Sat ctf report object
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 *    shr_error_e
 * \remark
 * * None
 */
static shr_error_e
dnx_sat_ctf_reports_config_set_verify(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_report_config_t * reports)
{
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(reports, _SHR_E_PARAM, "reports ");

    SHR_RANGE_VERIFY(ctf_id, DNX_SAT_CTF_ID_MIN, DNX_SAT_CTF_ID_MAX, _SHR_E_PARAM, "ctf_id");
    SHR_MAX_VERIFY(reports->report_sampling_percent, DNX_SAT_CTF_REPORT_SAMPLING_PER_MAX,
                   _SHR_E_PARAM, "report_sampling_percent");
 /** check ctf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.is_allocated(unit, ctf_id, &is_allocated));

 /** return error if the ctf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: ctf_id  %d does not exist\n", ctf_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  CTF report configure.
* \param [in] unit  -
*   Relevant unit.
* \param [in] ctf_id  -
*    ctf id
* \param [in] reports  -
*    Sat ctf report object
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_reports_config_set(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_report_config_t * reports)
{
    bcm_sat_ctf_report_config_t dnx_reports;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_reports_config_set_verify(unit, ctf_id, reports));
    LOG_VERBOSE(BSL_LS_BCM_SAT,
                ("%s %d: u(%d) ctf_id (%d) report sampling_percent (%d) flags(%d)\n", BSL_FUNC, BSL_LINE, unit, ctf_id,
                 reports->report_sampling_percent, reports->flags));

    sal_memset(&dnx_reports, 0, sizeof(bcm_sat_ctf_report_config_t));
    dnx_reports.report_sampling_percent = reports->report_sampling_percent;
    if (reports->flags & BCM_SAT_CTF_REPORT_ADD_SEQ_NUM)
    {
        dnx_reports.flags |= BCM_SAT_CTF_REPORT_ADD_SEQ_NUM;
    }
    if (reports->flags & BCM_SAT_CTF_REPORT_ADD_DELAY)
    {
        dnx_reports.flags |= BCM_SAT_CTF_REPORT_ADD_DELAY;
    }
    SHR_IF_ERR_EXIT(dnx_sat_ctf_reports_config_set(unit, ctf_id, &dnx_reports));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*    CTF stat config in HW table .
* \par
* \param [in] unit - Relevant unit.
* \param [in] ctf_id -  ctf_id.
* \param [in] stat - Sat ctf stat object for configuration .
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_stat_config_set(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_stat_config_t * stat)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    dbal_fields_e key = DBAL_FIELD_EMPTY;
    uint8 key_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * For writing on dynamic tables (in the OAMP), have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    /*
     * (1) Setting rx flow table
     */
    /** Taking a handle */
    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_natural_id))
    {
        table_id = DBAL_TABLE_SAT_RX_FLOW_SET;
        key = DBAL_FIELD_RX_FLOW_ID;
        key_val = ctf_id;
    }

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_odd_even_id))
    {
        if ((ctf_id % 2) == 0)
        {
            table_id = DBAL_TABLE_SAT_RX_FLOW_EVEN_SET;
            key = DBAL_FIELD_EVEN_FLOW_ID;
        }
        else
        {
            table_id = DBAL_TABLE_SAT_RX_FLOW_ODD_SET;
            key = DBAL_FIELD_ODD_FLOW_ID;
        }
        key_val = ctf_id / 2;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field8_set(unit, entry_handle_id, key, key_val);

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALC_EQUAL_FDV_BINS, INST_SINGLE,
                                 (stat->use_global_bin_config ? 0 : 1));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_MIN_DELAY, INST_SINGLE, stat->bin_min_delay);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_STEP, INST_SINGLE, stat->bin_step);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * (2) Setting rx state table
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_STATE_SET, &entry_handle_id));

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_TOBE_COUNT, INST_SINGLE, ctf_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UPDATE_CNTR_ALWS, INST_SINGLE,
                                 stat->update_counters_in_unvavail_state);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** (3) Reset state machine*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_STATE_SET, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RST_STATE_MCHN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_STATE_SET, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RST_STATE_MCHN, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    /*
     * After done, disable dynamic memory access
     */
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify CTF stat configure for BCM-API: bcm_sat_ctf_stat_config_set*() .
* \par
* \param [in] unit - Relevant unit.
* \param [in] ctf_id  - ctf id
* \param [in] stat  - Sat ctf stat object for configuration
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_stat_config_set_verify(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_stat_config_t * stat)
{
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(stat, _SHR_E_PARAM, "stat ");
    SHR_RANGE_VERIFY(ctf_id, DNX_SAT_CTF_ID_MIN, DNX_SAT_CTF_ID_MAX, _SHR_E_PARAM, "ctf_id");

   /** check ctf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.is_allocated(unit, ctf_id, &is_allocated));

   /** return error if the ctf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: ctf_id  %d does not exist\n", ctf_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  CTF stat configure.
* \param [in] unit  -
*   Relevant unit.
* \param [in] ctf_id  -
*    ctf id
* \param [in] stat  -
*    Sat ctf stat object for configuration
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_stat_config_set(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_stat_config_t * stat)
{
    bcm_sat_ctf_stat_config_t dnx_sat_ctf_stat_cfg;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_stat_config_set_verify(unit, ctf_id, stat));
    sal_memset(&dnx_sat_ctf_stat_cfg, 0, sizeof(dnx_sat_ctf_stat_cfg));
    dnx_sat_ctf_stat_cfg.bin_min_delay = stat->bin_min_delay;
    dnx_sat_ctf_stat_cfg.bin_step = stat->bin_step;
    dnx_sat_ctf_stat_cfg.use_global_bin_config = (stat->use_global_bin_config ? 1 : 0);
    dnx_sat_ctf_stat_cfg.update_counters_in_unvavail_state = (stat->update_counters_in_unvavail_state ? 1 : 0);
    SHR_IF_ERR_EXIT(dnx_sat_ctf_stat_config_set(unit, ctf_id, &dnx_sat_ctf_stat_cfg));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *  CTF window size and conter update setting in HW table .
 * \par
 *  \param [in] unit - Relevant unit.
 *  \param [in] ctf_id -  ctf_id.
 *  \param [in] window_size - Sat ctf window size setting.
 *  \param [in] update_cntrs_always - Sat conter update setting.
 * \retval
 *  * Zero if no error was detected
 *  * Negative if error was detected. See \ref
 *    shr_error_e
 * \remark
 *  * None
 */
static shr_error_e
dnx_sat_ctf_stat_per_flow_config_set(
    int unit,
    bcm_sat_ctf_t ctf_id,
    uint32 window_size,
    uint32 update_cntrs_always)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    dbal_fields_e key = DBAL_FIELD_EMPTY;
    uint8 key_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * For writing on dynamic tables (in the OAMP), have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_natural_id))
    {
        table_id = DBAL_TABLE_SAT_RX_STAT_PER_FLOW_SET;
        key = DBAL_FIELD_RX_FLOW_ID;
        key_val = ctf_id;
    }

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_odd_even_id))
    {
        if ((ctf_id % 2) == 0)
        {
            table_id = DBAL_TABLE_SAT_RX_STAT_PER_FLOW_EVEN_SET;
            key = DBAL_FIELD_EVEN_FLOW_ID;
        }
        else
        {
            table_id = DBAL_TABLE_SAT_RX_STAT_PER_FLOW_ODD_SET;
            key = DBAL_FIELD_ODD_FLOW_ID;
        }
        key_val = ctf_id / 2;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, key, key_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WINDOW_SIZE, INST_SINGLE, window_size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALWAYS_UPDATE_CNTRS, INST_SINGLE,
                                 update_cntrs_always);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    /*
     * After done, disable dynamic memory access
     */
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

  /**
  * \brief
  *  CTF window size and conter update setting in HW table .
  * \par
  *  \param [in] unit - Relevant unit.
  *  \param [in] ctf_id -  ctf_id.
  *  \param [out] window_size - Sat ctf window size setting.
  *  \param [out] update_cntrs_always - Sat conter update setting.
  * \retval
  *  * Zero if no error was detected
  *  * Negative if error was detected. See \ref
  *    shr_error_e
  * \remark
  *  * None
  */
static shr_error_e
dnx_sat_ctf_stat_per_flow_config_get(
    int unit,
    bcm_sat_ctf_t ctf_id,
    uint32 *window_size,
    uint32 *update_cntrs_always)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    dbal_fields_e key = DBAL_FIELD_EMPTY;
    uint8 key_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * For writing on dynamic tables (in the OAMP), have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_natural_id))
    {
        table_id = DBAL_TABLE_SAT_RX_STAT_PER_FLOW_SET;
        key = DBAL_FIELD_RX_FLOW_ID;
        key_val = ctf_id;
    }

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_odd_even_id))
    {
        if ((ctf_id % 2) == 0)
        {
            table_id = DBAL_TABLE_SAT_RX_STAT_PER_FLOW_EVEN_SET;
            key = DBAL_FIELD_EVEN_FLOW_ID;
        }
        else
        {
            table_id = DBAL_TABLE_SAT_RX_STAT_PER_FLOW_ODD_SET;
            key = DBAL_FIELD_ODD_FLOW_ID;
        }
        key_val = ctf_id / 2;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, key, key_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_WINDOW_SIZE, INST_SINGLE, window_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ALWAYS_UPDATE_CNTRS, INST_SINGLE, update_cntrs_always);

     /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    /*
     * After done, disable dynamic memory access
     */
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*    Get CTF stat from HW .
* \par
* \param [in] unit - Relevant unit.
* \param [in] ctf_id -  ctf_id.
* \param [in] flags -  flags.
* \param [out] stat -  Sat ctf stat information.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_stat_get(
    int unit,
    bcm_sat_ctf_t ctf_id,
    uint32 flags,
    bcm_sat_ctf_stat_t * stat)
{
    uint32 entry_handle_id;
    uint32 field = 0;
    uint32 field_avail_cnt = 0;
    uint32 field_max_delay = 0;
    uint32 field_min_delay = 0;
    uint32 field_last_delay = 0;
    uint32 field_array_bin[10][2] = { {0} };
    uint32 field_array_pkt[2] = { 0 };
    uint32 field_array_ooo[2] = { 0 };
    uint32 field_array_err[2] = { 0 };
    uint32 field_array_byte[2] = { 0 };
    uint32 field_array_ooo_byte[2] = { 0 };
    uint32 field_array_err_byte[2] = { 0 };
    uint32 field_array_delay_cnt[2] = { 0 };
    uint32 field_array_time_stamp[2] = { 0 };
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    dbal_fields_e key = DBAL_FIELD_EMPTY;
    uint8 key_val = 0;
    uint32 window_size = 0;
    uint32 update_cntrs_always = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Read counters */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_STAT_GET, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_RX_FLOW_ID, ctf_id);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_RCVD_PKT_CNTR_ACCUM, INST_SINGLE, field_array_pkt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_OOO_PKT_CNTR_ACCUM, INST_SINGLE, field_array_ooo);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_ERR_PKT_CNTR_ACCUM, INST_SINGLE, field_array_err);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_RCVD_OCTET_CNTR_ACCUM, INST_SINGLE,
                                   field_array_byte);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_OOO_OCTET_CNTR_ACCUM, INST_SINGLE,
                                   field_array_ooo_byte);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_ERR_OCTET_CNTR_ACCUM, INST_SINGLE,
                                   field_array_err_byte);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_PKT_DELAY_ACCUM, INST_SINGLE, &field_last_delay);

    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_0_PKT_CNTR_ACCUM, INST_SINGLE,
                                   &(field_array_bin[0][0]));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_1_PKT_CNTR_ACCUM, INST_SINGLE,
                                   &(field_array_bin[1][0]));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_2_PKT_CNTR_ACCUM, INST_SINGLE,
                                   &(field_array_bin[2][0]));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_3_PKT_CNTR_ACCUM, INST_SINGLE,
                                   &(field_array_bin[3][0]));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_4_PKT_CNTR_ACCUM, INST_SINGLE,
                                   &(field_array_bin[4][0]));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_5_PKT_CNTR_ACCUM, INST_SINGLE,
                                   &(field_array_bin[5][0]));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_6_PKT_CNTR_ACCUM, INST_SINGLE,
                                   &(field_array_bin[6][0]));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_7_PKT_CNTR_ACCUM, INST_SINGLE,
                                   &(field_array_bin[7][0]));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_8_PKT_CNTR_ACCUM, INST_SINGLE,
                                   &(field_array_bin[8][0]));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FDV_BIN_9_PKT_CNTR_ACCUM, INST_SINGLE,
                                   &(field_array_bin[9][0]));

    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_ACCUM_DELAY_CNTR_ACCUM, INST_SINGLE,
                                   field_array_delay_cnt);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_PKT_DELAY_ACCUM, INST_SINGLE, &field_max_delay);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MIN_PKT_DELAY_ACCUM, INST_SINGLE, &field_min_delay);

    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_TOD_STAMP_ACCUM, INST_SINGLE,
                                   field_array_time_stamp);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SEC_IN_AVAIL_STATE_CNTR, INST_SINGLE,
                               &field_avail_cnt);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * get sequence number
     */
    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_natural_id))
    {
        table_id = DBAL_TABLE_SAT_RX_EXP_SEQ_NUMBER_GET;
        key = DBAL_FIELD_RX_FLOW_ID;
        key_val = ctf_id;
    }

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_odd_even_id))
    {
        if ((ctf_id % 2) == 0)
        {
            table_id = DBAL_TABLE_SAT_RX_EXP_SEQ_NUMBER_EVEN_GET;
            key = DBAL_FIELD_EVEN_FLOW_ID;
        }
        else
        {
            table_id = DBAL_TABLE_SAT_RX_EXP_SEQ_NUMBER_ODD_GET;
            key = DBAL_FIELD_ODD_FLOW_ID;
        }
        key_val = ctf_id / 2;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, key, key_val);

    /*
     * Expected sequence number always increase 1 based on received packet. So last packet sequence number will be
     * expected sequence number -1.
     */
    field = 0;
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXP_SEQ_NUMBER, INST_SINGLE, &field);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    COMPILER_64_SET(stat->last_packet_seq_number, 0, field - 1);
    COMPILER_64_SET(stat->sec_in_avail_state_counter, 0, field_avail_cnt);
    COMPILER_64_SET(stat->max_packet_delay, 0, field_max_delay);
    COMPILER_64_SET(stat->min_packet_delay, 0, field_min_delay);
    COMPILER_64_SET(stat->last_packet_delay, 0, field_last_delay);

    COMPILER_64_SET(stat->received_packet_cnt, field_array_pkt[1], field_array_pkt[0]);
    COMPILER_64_SET(stat->out_of_order_packet_cnt, field_array_ooo[1], field_array_ooo[0]);
    COMPILER_64_SET(stat->err_packet_cnt, field_array_err[1], field_array_err[0]);
    COMPILER_64_SET(stat->received_octet_cnt, field_array_byte[1], field_array_byte[0]);
    COMPILER_64_SET(stat->out_of_order_octet_cnt, field_array_ooo_byte[1], field_array_ooo_byte[0]);
    COMPILER_64_SET(stat->err_octet_cnt, field_array_err_byte[1], field_array_err_byte[0]);

    COMPILER_64_SET(stat->bin_delay_counters[0], field_array_bin[0][1], field_array_bin[0][0]);
    COMPILER_64_SET(stat->bin_delay_counters[1], field_array_bin[1][1], field_array_bin[1][0]);
    COMPILER_64_SET(stat->bin_delay_counters[2], field_array_bin[2][1], field_array_bin[2][0]);
    COMPILER_64_SET(stat->bin_delay_counters[3], field_array_bin[3][1], field_array_bin[3][0]);
    COMPILER_64_SET(stat->bin_delay_counters[4], field_array_bin[4][1], field_array_bin[4][0]);
    COMPILER_64_SET(stat->bin_delay_counters[5], field_array_bin[5][1], field_array_bin[5][0]);
    COMPILER_64_SET(stat->bin_delay_counters[6], field_array_bin[6][1], field_array_bin[6][0]);
    COMPILER_64_SET(stat->bin_delay_counters[7], field_array_bin[7][1], field_array_bin[7][0]);
    COMPILER_64_SET(stat->bin_delay_counters[8], field_array_bin[8][1], field_array_bin[8][0]);
    COMPILER_64_SET(stat->bin_delay_counters[9], field_array_bin[9][1], field_array_bin[9][0]);
    COMPILER_64_SET(stat->accumulated_delay_count, field_array_delay_cnt[1], field_array_delay_cnt[0]);
    COMPILER_64_SET(stat->time_of_day_stamp, field_array_time_stamp[1], field_array_time_stamp[0]);
    /*
     *  Setting rx state table
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_STATE_SET, &entry_handle_id));

    dnx_sat_ctf_stat_per_flow_config_get(unit, ctf_id, &window_size, &update_cntrs_always);
    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_TOBE_COUNT, INST_SINGLE, ctf_id);
    field = (flags & BCM_SAT_CTF_STAT_DO_NOT_CLR_ON_READ ? 0 : 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLEAR_ON_READ, INST_SINGLE, field);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dnx_sat_ctf_stat_per_flow_config_set(unit, ctf_id, window_size, update_cntrs_always);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify CTF stat configure for BCM-API: bcm_sat_ctf_stat_config_get*() .
* \par
* \param [in] unit - Relevant unit.
* \param [in] ctf_id  - ctf id
* \param [in] stat  - Sat ctf stat object for configuration
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_stat_get_verify(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_stat_t * stat)
{
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(stat, _SHR_E_PARAM, "stat ");
    SHR_RANGE_VERIFY(ctf_id, DNX_SAT_CTF_ID_MIN, DNX_SAT_CTF_ID_MAX, _SHR_E_PARAM, "ctf_id");
    /** check ctf_id is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.is_allocated(unit, ctf_id, &is_allocated));

    /** return error if the ctf_id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: ctf_id  %d does not exist\n", ctf_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  CTF stat get.
* \param [in] unit  -
*   Relevant unit.
* \param [in] ctf_id  -
*    ctf id
* \param [in] flags  -
*   Relevant flags
* \param [out] stat  -
*    Sat ctf stat
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_stat_get(
    int unit,
    bcm_sat_ctf_t ctf_id,
    uint32 flags,
    bcm_sat_ctf_stat_t * stat)
{
    bcm_sat_ctf_stat_t dnx_sat;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_stat_get_verify(unit, ctf_id, stat));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) ctf_id (%d) flags (%d) \n", BSL_FUNC, BSL_LINE, unit, ctf_id, flags));

    sal_memset(&dnx_sat, 0, sizeof(bcm_sat_ctf_stat_t));
    SHR_IF_ERR_EXIT(dnx_sat_ctf_stat_get(unit, ctf_id, flags, &dnx_sat));
    sal_memcpy(stat, &dnx_sat, sizeof(bcm_sat_ctf_stat_t));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     CTF trap set  in HW table.
* \par
* \param [in] unit - Relevant unit.
* \param [in] trap_id -  trap_id.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_trap_set(
    int unit,
    uint32 *trap_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * (1) Setting rx state table
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_RX_TRAP_CONFIG, &entry_handle_id));

    /** setting key fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_TRAP_CODE_0, INST_SINGLE, trap_id[0]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_TRAP_CODE_1, INST_SINGLE, trap_id[1]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_TRAP_CODE_2, INST_SINGLE, trap_id[2]);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify CTF stat configure for BCM-API: bcm_sat_ctf_stat_config_get*() .
* \par
* \param [in] unit - Relevant unit.
* \param [in] trap_id  - trap id
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_trap_add_verify(
    int unit,
    uint32 trap_id)
{
    int max_trap_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATCtfTrapId, &max_trap_id));

    SHR_MAX_VERIFY(trap_id, max_trap_id, _SHR_E_PARAM, "trap_id");
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add trap for CTF.
* \param [in] unit  -
*   Relevant unit.
* \param [in] trap_id  -
*    trap id
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_trap_add(
    int unit,
    uint32 trap_id)
{
    int trap_idx = 0;
    dnx_sat_data_t psat_data;
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_trap_add_verify(unit, trap_id));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) trap_id (%d)\n", BSL_FUNC, BSL_LINE, unit, trap_id));

    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));

    rv = dnx_sat_trap_id_find(unit, &psat_data, trap_id, &trap_idx);
    if (rv == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(BCM_E_EXISTS);
    }
    rv = _SHR_E_NONE;

    SHR_IF_ERR_EXIT(dnx_sat_ctf_trap_id_allocate(unit, &trap_idx));

    psat_data.ctf_trap_id[trap_idx] = trap_id;

    SHR_IF_ERR_EXIT(dnx_sat_ctf_trap_set(unit, psat_data.ctf_trap_id));

    SHR_IF_ERR_EXIT(dnx_sat_data_set(unit, &psat_data));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Check if Session in HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] session_id -  session  id .
* \param [out] found -  return TRUE if session id in HW.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
shr_error_e
dnx_sat_session_inuse(
    int unit,
    int session_id,
    int *found)
{
    uint32 entry_handle_id;
    int is_end;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(found, _SHR_E_PARAM, "user_data ");
    *found = FALSE;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TRAP_SESSION_MAP, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    is_end = 0;
    /*
     * Receive first entry in table.
     * If is_end is set to 1 after calling this API, then the table is empty.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 session, valid;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SESSION_MAP_ID, INST_SINGLE, &session));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &valid));

        if (valid && (session_id == session))
        {
            *found = TRUE;
            break;
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Map ctf trap data to Session  in HW.
* \par
* \param [in] unit - Relevant unit.
* \param [in] trap_data -  trap data.
* \param [in] trap_data_mask -  trap_data  mask.
* \param [in] entry_idx -  entry index.
* \param [in] session_id -  session  id .
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_trap_data_to_session_map(
    int unit,
    uint32 trap_data,
    uint32 trap_data_mask,
    uint32 entry_idx,
    uint32 session_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * For writing on dynamic tables (in the OAMP), have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TRAP_SESSION_MAP, &entry_handle_id));

    /** set access ID (index) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_idx));

    /** setting key fields */
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_SESSION_MAP_KEY, trap_data, trap_data_mask);

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SESSION_MAP_ID, INST_SINGLE, session_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, 1);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    /*
     * After done, disable dynamic memory access
     */
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verifyMap ctf trap to session ID for BCM-API: bcm_sat_ctf_trap_data_to_session_map*() .
* \par
* \param [in] unit - Relevant unit.
* \param [in] trap_data  -trap data
* \param [in] trap_data_mask  -trap data mask
* \param [in] session_id  -session id
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_trap_data_to_session_map_verify(
    int unit,
    uint32 trap_data,
    uint32 trap_data_mask,
    uint32 session_id)
{
    int max_trap_data = 0;
    int max_trap_data_mask = 0;
    int max_session_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATCtfTrapData, &max_trap_data));
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATCtfTrapDataMask, &max_trap_data_mask));
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATCtfTrapSessionId, &max_session_id));
    SHR_MAX_VERIFY(trap_data, max_trap_data, _SHR_E_PARAM, "trap_data");
    SHR_MAX_VERIFY(trap_data_mask, max_trap_data_mask, _SHR_E_PARAM, "trap_data_mask");
    SHR_MAX_VERIFY(session_id, max_session_id, _SHR_E_PARAM, "session_id");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Map ctf trap to session ID.
* \param [in] unit  -
*   Relevant unit.
* \param [in] trap_data  -
*    trap data
* \param [in] trap_data_mask  -
*   trap data mask
* \param [in] session_id  -
*    session id
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_trap_data_to_session_map(
    int unit,
    uint32 trap_data,
    uint32 trap_data_mask,
    uint32 session_id)
{
    int entry_idx;
    int rv = _SHR_E_NONE;
    dnx_sat_data_t psat_data;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_trap_data_to_session_map_verify(unit, trap_data, trap_data_mask, session_id));
    LOG_VERBOSE(BSL_LS_BCM_SAT,
                ("%s %d: u(%d) trap_data (%d) trap_data_mask (%d) session_id (%d)\n", BSL_FUNC, BSL_LINE, unit,
                 trap_data, trap_data_mask, session_id));

    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));
    /*
     * Return E_EXIST for creating duplicate session_map
     */
    rv = dnx_sat_ctf_trap_entry_find(unit, &psat_data, trap_data, trap_data_mask, (uint32 *) &entry_idx);
    if (rv == _SHR_E_EXISTS)
    {
        SHR_IF_ERR_EXIT(_SHR_E_EXISTS);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_sat_ctf_trap_entry_allocate(unit, &entry_idx));
    }
    SHR_IF_ERR_EXIT(dnx_sat_ctf_trap_data_to_session_map
                    (unit, trap_data, trap_data_mask, (uint32) entry_idx, session_id));

    psat_data.trap_data[entry_idx].trap_data = trap_data;
    psat_data.trap_data[entry_idx].trap_mask = trap_data_mask;
    psat_data.trap_data[entry_idx].ssid = session_id;

    SHR_IF_ERR_EXIT(dnx_sat_data_set(unit, &psat_data));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*     UNmap between ctf trap data & Session  in HW.
* \par
* \param [in] unit -  Relevant unit.
* \param [in] entry_idx -  entry index.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/

static shr_error_e
dnx_sat_ctf_trap_data_to_session_unmap(
    int unit,
    uint32 entry_idx)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * For writing on dynamic tables (in the OAMP), have to enable dynamic memory access
     */
    SHR_IF_ERR_EXIT(dnx_sat_dynamic_memory_access_set(unit, 1));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TRAP_SESSION_MAP, &entry_handle_id));

    /** set access ID (index) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_idx));

    /** setting key fields */
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_SESSION_MAP_KEY, 0, 0);

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SESSION_MAP_ID, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, 0);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    /*
     * After done, disable dynamic memory access
     */
    dnx_sat_dynamic_memory_access_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify Unmap ctf trap to session ID for BCM-API: bcm_sat_ctf_trap_data_to_session_unmap*() .
* \par
* \param [in] unit - Relevant unit.
* \param [in] trap_data  -trap data
* \param [in] trap_data_mask  -trap data mask
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_trap_data_to_session_unmap_verify(
    int unit,
    uint32 trap_data,
    uint32 trap_data_mask)
{
    int max_trap_data = 0;
    int max_trap_data_mask = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATCtfTrapData, &max_trap_data));
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATCtfTrapDataMask, &max_trap_data_mask));

    SHR_MAX_VERIFY(trap_data, max_trap_data, _SHR_E_PARAM, "trap_data");
    SHR_MAX_VERIFY(trap_data_mask, max_trap_data_mask, _SHR_E_PARAM, "trap_data_mask");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  UnMap ctf trap to session ID.
* \param [in] unit  -
*   Relevant unit.
* \param [in] trap_data  -
*    trap data
* \param [in] trap_data_mask  -
*   trap data mask
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_trap_data_to_session_unmap(
    int unit,
    uint32 trap_data,
    uint32 trap_data_mask)
{
    uint32 entry_idx = 0;
    dnx_sat_data_t psat_data;
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_trap_data_to_session_unmap_verify(unit, trap_data, trap_data_mask));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) trap_data (%d) trap_data_mask (%d)\n", BSL_FUNC, BSL_LINE,
                                 unit, trap_data, trap_data_mask));

    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));
    rv = dnx_sat_ctf_trap_entry_find(unit, &psat_data, trap_data, trap_data_mask, &entry_idx);
    if (rv != _SHR_E_EXISTS)
    {
        SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT(dnx_sat_ctf_trap_data_to_session_unmap(unit, entry_idx));

    sal_memset(&(psat_data.trap_data[entry_idx]), 0, sizeof(dnx_sat_ctf_trap_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_ctf_trap_entry_free(unit, (int) entry_idx));

    SHR_IF_ERR_EXIT(dnx_sat_data_set(unit, &psat_data));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_sat_ctf_trap_remove_verify(
    int unit,
    uint32 trap_id)
{
    int max_trap_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATCtfTrapId, &max_trap_id));

    SHR_MAX_VERIFY(trap_id, max_trap_id, _SHR_E_PARAM, "trap_id");
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Remove CTF trap ID.
* \param [in] unit  -
*   Relevant unit.
* \param [in] trap_id  -
*    trap id
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_trap_remove(
    int unit,
    uint32 trap_id)
{
    int32 trap_idx = 0;
    dnx_sat_data_t psat_data;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_trap_remove_verify(unit, trap_id));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) trap_id (%d) \n", BSL_FUNC, BSL_LINE, unit, trap_id));
    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));
    SHR_IF_ERR_EXIT(dnx_sat_trap_id_find(unit, &psat_data, trap_id, &trap_idx));

    psat_data.ctf_trap_id[trap_idx] = DNX_SAT_CTF_TRAP_ID_UNSET_VAL;
    SHR_IF_ERR_EXIT(dnx_sat_ctf_trap_id_free(unit, (int) trap_idx));

    SHR_IF_ERR_EXIT(dnx_sat_ctf_trap_set(unit, psat_data.ctf_trap_id));
    SHR_IF_ERR_EXIT(dnx_sat_data_set(unit, &psat_data));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the CTF trap ID index.
* \param [in] unit  -
*   Relevant unit.
* \param [in] trap_id  -
*    trap id
* \param [out] trap_idx  -
*    trap idx
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
dnx_sat_trap_idx_get(
    int unit,
    uint32 trap_id,
    int *trap_idx)
{
    dnx_sat_data_t psat_data;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_trap_add_verify(unit, trap_id));

    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) trap_id (%d) \n", BSL_FUNC, BSL_LINE, unit, trap_id));
    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));
    SHR_IF_ERR_EXIT(dnx_sat_trap_id_find(unit, &psat_data, trap_id, trap_idx));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Remove All of CTF trap .
* \param [in] unit  -
*   Relevant unit.
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_trap_remove_all(
    int unit)
{
    int trap_idx = 0;
    uint8 is_allocated = 0;
    dnx_sat_data_t psat_data;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    sal_memset(&psat_data, 0, sizeof(dnx_sat_data_t));
    SHR_IF_ERR_EXIT(dnx_sat_data_get(unit, &psat_data));

    for (trap_idx = 0; trap_idx < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_ID_ENTRIES; trap_idx++)
    {
        /** check trap_idx is allocated/exsits */
        SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_trap_id.is_allocated(unit, trap_idx, &is_allocated));
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_sat_ctf_trap_id_free(unit, trap_idx));
            psat_data.ctf_trap_id[trap_idx] = DNX_SAT_CTF_TRAP_ID_UNSET_VAL;
        }
    }

    SHR_IF_ERR_EXIT(dnx_sat_ctf_trap_set(unit, psat_data.ctf_trap_id));
    SHR_IF_ERR_EXIT(dnx_sat_data_set(unit, &psat_data));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify Traverse all ctf for BCM-API: bcm_sat_ctf_traverse*() .
* \par
* \param [in] unit - Relevant unit.
* \param [out] user_data  -
*    user data
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*   shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_ctf_traverse_verify(
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
*  Traverse all ctf .
* \param [in] unit  -
*   Relevant unit.
* \param [in] cb  -
*    Ctf callback function prototype.
* \param [out] user_data  -
*    user data
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_ctf_traverse(
    int unit,
    bcm_sat_ctf_traverse_cb cb,
    void *user_data)
{

    bcm_sat_ctf_t ctf_id;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_ctf_traverse_verify(unit, user_data));

    for (ctf_id = DNX_SAT_CTF_ID_MIN; ctf_id <= DNX_SAT_CTF_ID_MAX; ctf_id++)
    {
        /** check ctf_id is allocated/exsits */
        SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.is_allocated(unit, ctf_id, &is_allocated));
        if (is_allocated)
        {
            /*
             * Invoke user callback.
             */
            (*cb) (unit, ctf_id, user_data);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
