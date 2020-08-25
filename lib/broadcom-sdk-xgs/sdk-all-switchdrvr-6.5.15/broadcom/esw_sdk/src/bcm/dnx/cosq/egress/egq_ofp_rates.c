/**
 * \file egq_ofp_rates.c
 *
 * Egress queuing port management functionality for DNX.
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX
/*
 * Ported from:
 *   src/soc/dnx/legacy/ARAD/arad_ofp_rates.c
 *   src/soc/dnx/legacy/JER/jer_ofp_rates.c
 */
/*************
 * INCLUDES  *

 *************/
/* { */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <soc/mem.h> 
#include <soc/dnx/dbal/dbal.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_ofp_rate_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>

#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>

#include <shared/utilex/utilex_u64.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ofp_rate_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
int
dnx_ofp_rates_interface_internal_rate_set(
    int unit,
    int core,
    uint32 egr_if_id,
    uint32 internal_rate)
{
    uint32 cal_set, org_if_val, org_otm_val;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_OTM_CALENDAR_CRDT_TABLE;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction. Fixed part
     */
    /*
     * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
     * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    for (cal_set = 0; cal_set < DNX_OFP_RATES_NOF_CALS_IN_DUAL_MODE; cal_set++)
    {
        if (internal_rate > dnx_data_egr_queuing.params.max_credit_number_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Specified calendar rate (%d) for interface is too big (larger than %d) \n",
                         internal_rate, dnx_data_egr_queuing.params.max_credit_number_get(unit));
        }
        /*
         * key construction. Variable part.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (egr_if_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_SET, (uint32) (cal_set));
        /*
         * Set the values
         */
        dbal_field_id = DBAL_FIELD_OTM_CAL_CRDT_TO_ADD;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (internal_rate));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_interface_internal_rate_get(
    int unit,
    int core,
    uint32 egr_if_id,
    uint32 *internal_rate)
{
    uint32 org_if_val, org_otm_val;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;
    uint32 cal_set;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    /*
     * get internal interface rate
     */
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_OTM_CALENDAR_CRDT_TABLE;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * Read only from calendar set '0' since vlue for calendar set '1' must be the same as
     * value for calendar set '0'.
     */
    cal_set = 0;
    /*
     * key construction.
     */
    /*
     * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
     * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (egr_if_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_SET, (uint32) (cal_set));
    /*
     * Get the value
     */
    dbal_field_id = DBAL_FIELD_OTM_CAL_CRDT_TO_ADD;
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) internal_rate);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Implementation
 */
int
dnx_ofp_rates_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_init(
    int unit)
{
    int core, cal_id, idx, ps_id, tcg_id, res;
    uint32 otm_port, tcg, qpare;
    uint32 init_max_burst;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;
    uint32 field32_value;
    uint32 egr_nof_base_q_pairs;
    uint32 egr_nof_ps;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    init_max_burst = DNX_EGQ_MAX_BURST_IN_BYTES;
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.init(unit));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * update sw db with default burst size
         */
        egr_nof_base_q_pairs = dnx_data_egr_queuing.params.nof_q_pairs_get(unit);
        for (idx = 0; idx < egr_nof_base_q_pairs; idx++)
        {
            /*
             * port burst
             */
            res = dnx_ofp_rate_db.otm.shaping.burst.set(unit, core, idx, init_max_burst);
            SHR_IF_ERR_EXIT(res);
            /*
             * port priority burst
             */
            res = dnx_ofp_rate_db.qpair.burst.set(unit, core, idx, init_max_burst);
            SHR_IF_ERR_EXIT(res);
        }
        egr_nof_ps = egr_nof_base_q_pairs / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        for (ps_id = 0; ps_id < egr_nof_ps; ps_id++)
        {
            for (tcg_id = 0; tcg_id < DNX_NOF_TCGS; tcg_id++)
            {
                /*
                 * tcg burst
                 */
                res = dnx_ofp_rate_db.tcg.burst.set(unit, core, ps_id, tcg_id, init_max_burst);
                SHR_IF_ERR_EXIT(res);
            }
        }
        {
            /*
             * By default Enable Interface, OTM, QPair, TCG shapers
             */
            dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
            /*
             * key construction.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /*
             * Set the values
             */
            field32_value = (uint32) (1);
            dbal_field_id = DBAL_FIELD_INTERFACE_SPR_ENA;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
            dbal_field_id = DBAL_FIELD_OTM_SPR_ENA;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
            dbal_field_id = DBAL_FIELD_QPAIR_SPR_ENA;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
            dbal_field_id = DBAL_FIELD_TCG_SPR_ENA;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.set(unit, TRUE));
        {
            /*
             * Iterate over all calendars
             */
#if (1)
/* { */
            
            /*
             * Reuse allocated buffer.
             */
            dbal_table_id = DBAL_TABLE_EGQ_SHAPER_OTM_CALENDAR_CRDT_TABLE;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
            /*
             * key construction. Fixed part
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

            for (cal_id = 0; cal_id < dnx_data_egr_queuing.params.nof_calendars_get(unit); cal_id++)
            {
                uint32 cal_set;
                /*
                 * key construction. Variable part.
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (cal_id));
                for (cal_set = 0; cal_set < DNX_OFP_RATES_NOF_CALS_IN_DUAL_MODE; cal_set++)
                {
                    uint32 field32_value;
                    /*
                     * key construction. Variable part.
                     */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_SET, (uint32) (cal_set));
                    /*
                     * Set the values
                     */
                    field32_value = (uint32) (dnx_data_egr_queuing.params.max_credit_number_get(unit));
                    dbal_field_id = DBAL_FIELD_OTM_CAL_CRDT_TO_ADD;
                    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
                    dbal_field_id = DBAL_FIELD_OTM_CAL_MAX_BURST;
                    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                }
            }
/* } */
#endif
            /*
             * Reuse allocated buffer.
             */
            dbal_table_id = DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
            /*
             * key construction. Fixed part
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            for (cal_id = 0; cal_id < dnx_data_egr_queuing.params.nof_calendars_get(unit); cal_id++)
            {
                /*
                 * key construction. Variable part.
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (cal_id));
                /*
                 * set calendar length to 0
                 */
                /*
                 * Set the values
                 */
                field32_value = (uint32) (0);
                dbal_field_id = DBAL_FIELD_PORT_OFFSET_HIGH_A;
                dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
                dbal_field_id = DBAL_FIELD_PORT_OFFSET_HIGH_B;
                dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
        /** init valid rate indication to invalid */
        for (otm_port = 0; otm_port < dnx_data_port.general.nof_tm_ports_get(unit); otm_port++)
        {
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.valid.set(unit, core, otm_port, FALSE));
        }
        for (qpare = 0; qpare < DNX_OFP_RATES_CAL_LEN_EGQ_PORT_PRIO_MAX; qpare++)
        {
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.valid.set(unit, core, qpare, FALSE));
        }
        for (tcg = 0; tcg < DNX_OFP_RATES_CAL_LEN_EGQ_TCG_MAX; tcg++)
        {
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.
                            valid.set(unit, core, tcg / DNX_EGR_NOF_Q_PAIRS_IN_PS, tcg % DNX_EGR_NOF_Q_PAIRS_IN_PS,
                                      FALSE));
        }
        {
            /*
             * enable credit table read
             */
            /*
             * By default Enable Interface, OTM, QPair, TCG shapers
             */
            dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
            /*
             * key construction.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /*
             * Set the values
             */
            field32_value = (uint32) (1);
            dbal_field_id = DBAL_FIELD_QP_CRDT_TABLE_READ_EN;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
            dbal_field_id = DBAL_FIELD_OTM_CRDT_TABLE_READ_EN;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
            dbal_field_id = DBAL_FIELD_TCG_CRDT_TABLE_READ_EN;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egq_otm_shapers_set(
    int unit,
    int core,
    uint8 if_recalc)
{
    uint32 egr_if_id, internal_rate, calcal_length, calcal_instances, cal_rate;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * If if_recalc is TRUE need to recalculate calcal and interface shapers
     */
    if (if_recalc == TRUE)
    {
        /*
         * recalculate calcal
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_calcal_config(unit, core));
        /*
         * mark that calcal is modified
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_is_modified.set(unit, core, TRUE));

        /*
         * Recalculate all interface shapers
         */
        for (egr_if_id = 0; egr_if_id < dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit); egr_if_id++)
        {
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.rate.get(unit, core, egr_if_id, &cal_rate));
            if (cal_rate)
            {
                /*
                 * the calculation need to consider the amount of calcal instances and the calcal size
                 */
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.get(unit, core, &calcal_length));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.
                                nof_calcal_instances.get(unit, core, egr_if_id, &calcal_instances));
                /*
                 * translate Kbps rate to internal rate (clocks)
                 */
                SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_shaper_rate_to_internal
                                (unit, cal_rate, calcal_instances, calcal_length, &internal_rate));

                /*
                 * set interface shaper rate
                 */
                SHR_IF_ERR_EXIT(dnx_ofp_rates_interface_internal_rate_set(unit, core, egr_if_id, internal_rate));
            }
        }
    }
    else
    {
        /*
         * Copy existing calcal
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_calcal_copy(unit, core));
    }

    /*
     * Recalculate all port shapers
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_single_port_rate_hw_set(unit, core));

    /*
     * mark that all shapers are recalculated with the new calcal 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_is_modified.set(unit, core, FALSE));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egq_interface_shaper_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *if_shaper_rate)
{
    uint32 egr_if_id, internal_rate, calcal_length, calcal_instances;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get egress interface shaper id 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_port2chan_cal_get(unit, core, tm_port, &egr_if_id));

    /*
     * get interface shaper internal rate 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_interface_internal_rate_get(unit, core, egr_if_id, &internal_rate));

    /*
     * the calculation need to consider the amount of calcal instances and the calcal size
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.get(unit, core, &calcal_length));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.nof_calcal_instances.get(unit, core, egr_if_id, &calcal_instances));

    /*
     * translate from internal rate (clocks) to Kbps 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_shaper_rate_from_internal
                    (unit, internal_rate, calcal_instances, calcal_length, if_shaper_rate));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egq_single_port_rate_sw_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 rate)
{
    dnx_algo_port_type_e port_type;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * in case of non-egq ports - do nothing
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
    {   /* do nothing */
        SHR_EXIT();
    }

    /*
     * Setting rate to sw 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.rate.set(unit, core, tm_port, rate));

    /*
     * Mark port as valid 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.valid.set(unit, core, tm_port, TRUE));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_port2chan_cal_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *calendar)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_egr_port2egress_offset(unit, core, tm_port, calendar));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_retrieve_egress_shaper_reg_field_names(
    int unit,
    dnx_ofp_rates_cal_info_t * cal_info,
    dnx_ofp_rates_cal_set_e cal2set,
    dnx_ofp_rates_egq_chan_arb_field_type_e field_type,
    dbal_tables_e * dbal_table_id_p,
    dbal_fields_e * dbal_field_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_table_id_p, _SHR_E_PARAM, "dbal_table_id_p");
    SHR_NULL_CHECK(dbal_field_id_p, _SHR_E_PARAM, "dbal_field_id_p");
    SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");

    switch (cal_info->cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            *dbal_table_id_p = DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH;
            *dbal_field_id_p =
                (cal2set == DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_PORT_OFFSET_HIGH_A : DBAL_FIELD_PORT_OFFSET_HIGH_B;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            *dbal_table_id_p = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
            *dbal_field_id_p =
                (cal2set == DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_NUM_OF_QP_SPR_A : DBAL_FIELD_NUM_OF_QP_SPR_B;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            *dbal_table_id_p = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
            *dbal_field_id_p =
                (cal2set == DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_NUM_OF_TCG_SPR_A : DBAL_FIELD_NUM_OF_TCG_SPR_B;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal enum for calendar type (%d). Should be between %d and %d",
                         cal_info->cal_type, FIRST_DNX_OFP_RATES_EGQ_CAL_TYPE, NUM_DNX_OFP_RATES_EGQ_CAL_TYPE - 1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_egress_shaper_mem_field_read(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    dbal_tables_e dbal_table_id,
    dbal_fields_e dbal_field_id,
    uint32 *data_p)
{
    uint32 entry_handle_id;
    uint32 field32_value;
    uint32 offset;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (cal_info->cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
            /*
             * key construction. Fixed part
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            offset = cal_info->chan_arb_id;
            /*
             * Key construction. Variable part.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (offset));
            /*
             * Get the value
             */
            dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, data_p);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            *data_p = (1 + *data_p);
            if (offset)
            {
                /*
                 * New key construction. Variable part.
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (offset - 1));
                /*
                 * Get the value
                 */
                dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &field32_value);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                *data_p -= (field32_value + 1);
            }
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
            /*
             * key construction.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /*
             * Get the value
             */
            dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, data_p);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            (*data_p)++;
            break;
        }
        default:
        {
            break;
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_egress_shaper_mem_field_write(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    dbal_tables_e dbal_table_id,
    dbal_fields_e dbal_field_id,
    uint32 data)
{
    uint32 entry_handle_id;
    uint32 field32_value;
    uint32 offset;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (cal_info->cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
            /*
             * key construction. Fixed part
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            offset = cal_info->chan_arb_id;
            if (offset)
            {
                /*
                 * Key construction. Variable part.
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (offset - 1));
                /*
                 * Set the value
                 */
                field32_value = cal_info->arb_slot_id - 1;
                dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            /*
             * Key construction. Variable part.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (offset));
            /*
             * Set the value
             */
            field32_value = data - 1 + cal_info->arb_slot_id;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
            /*
             * key construction.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /*
             * Set the value
             */
            field32_value = data - 1;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        }
        default:
        {
            break;
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egress_shaper_cal_write(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    dnx_ofp_rates_cal_set_e cal2set,
    dnx_ofp_rates_egq_chan_arb_field_type_e field_type,
    uint32 data)
{
    dbal_tables_e dbal_table_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * retrieve memory and field names 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_retrieve_egress_shaper_reg_field_names(unit,
                                                                         cal_info,
                                                                         cal2set,
                                                                         field_type, &dbal_table_id, &dbal_field_id));

    /*
     * write 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egress_shaper_mem_field_write(unit,
                                                                core, cal_info, dbal_table_id, dbal_field_id, data));
exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egress_shaper_cal_read(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    dnx_ofp_rates_cal_set_e cal2set,
    dnx_ofp_rates_egq_chan_arb_field_type_e field_type,
    uint32 *data)
{
    dbal_tables_e dbal_table_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * retrieve memory and field names 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_retrieve_egress_shaper_reg_field_names(unit,
                                                                         cal_info,
                                                                         cal2set,
                                                                         field_type, &dbal_table_id, &dbal_field_id));

    /*
     * Read 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egress_shaper_mem_field_read(unit,
                                                               core, cal_info, dbal_table_id, dbal_field_id, data));
exit:
    SHR_FUNC_EXIT;
}

static uint32
dnx_ofp_rates_retrieve_egress_shaper_setting_field(
    int unit,
    dnx_ofp_rates_cal_info_t * cal_info,
    dbal_fields_e * dbal_field_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");
    SHR_NULL_CHECK(dbal_field_id_p, _SHR_E_PARAM, "dbal_field_id_p");
    /*
     * Check which calendars (EGQ & SCH - Calendars get 'A' or 'B')
     * are currently active.
     */
    switch (cal_info->cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            *dbal_field_id_p = DBAL_FIELD_OTM_SPR_SET_SEL;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            *dbal_field_id_p = DBAL_FIELD_QPAIR_SPR_SET_SEL;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            *dbal_field_id_p = DBAL_FIELD_TCG_SPR_SET_SEL;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unkonown type (%d) in cal_info", cal_info->cal_type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * this function is enable/disable design read from otm and interface credit table.
 * This function will enable/disable shapers accordingly in order to not cause any traffic lost.
 * however, disabling shaper will effect all the ports in the system
 */
int
dnx_ofp_rates_if_port_read_enable_set(
    int unit,
    int core,
    uint32 read_en,
    uint32 *org_if_read_en,
    uint32 *org_port_read_en)
{
#if (0)
/* { */
    soc_reg_above_64_val_t reg_val;
/* } */
#endif
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;
    uint32 field32_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * By default Enable Interface, OTM, QPair, TCG shapers
     */
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    if (read_en)
    {
        field32_value = *org_if_read_en;
        dbal_field_id = DBAL_FIELD_INTERFACE_SPR_ENA;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
        field32_value = *org_port_read_en;
        dbal_field_id = DBAL_FIELD_OTM_SPR_ENA;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
    }
    else
    {
        /*
         * Get the value
         */
        dbal_field_id = DBAL_FIELD_INTERFACE_SPR_ENA;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) org_if_read_en);
        dbal_field_id = DBAL_FIELD_OTM_SPR_ENA;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) org_port_read_en);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        field32_value = 0;
        dbal_field_id = DBAL_FIELD_INTERFACE_SPR_ENA;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
        field32_value = *org_port_read_en;
        dbal_field_id = DBAL_FIELD_OTM_SPR_ENA;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
    }
    field32_value = read_en;
    dbal_field_id = DBAL_FIELD_OTM_CRDT_TABLE_READ_EN;
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, field32_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

#if (0)
/* { */
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, core, 0, reg_val));
    if (read_en)
    {
        soc_reg_above_64_field32_set(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, INTERFACE_SPR_ENAf,
                                     *org_if_read_en);
        soc_reg_above_64_field32_set(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_ENAf,
                                     *org_port_read_en);
    }
    else
    {
        *org_if_read_en =
            soc_reg_above_64_field32_get(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, INTERFACE_SPR_ENAf);
        *org_port_read_en =
            soc_reg_above_64_field32_get(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_ENAf);
        soc_reg_above_64_field32_set(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, INTERFACE_SPR_ENAf, 0);
        soc_reg_above_64_field32_set(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_ENAf, 0);
    }
    soc_reg_above_64_field32_set(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_CRDT_TABLE_READ_ENf, read_en);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPS_EGRESS_SHAPER_ENABLE_SETTINGSr, core, 0, reg_val));
/* } */
#endif

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * this function is enable/disable design read from port priority credit table.
 * This function will enable/disable shapers accordingly in order to not cause any traffic lost.
 * however, disabling shaper will effect all the ports in the system
 */
static int
dnx_ofp_rates_tc_read_enable_set(
    int unit,
    int core,
    uint32 read_en,
    uint32 *org_tc_read_en_p)
{
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_field_id = DBAL_FIELD_QPAIR_SPR_ENA;
    if (read_en)
    {
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (*org_tc_read_en_p));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /*
         * Get the value
         */
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) (org_tc_read_en_p));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (0));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    dbal_field_id = DBAL_FIELD_QP_CRDT_TABLE_READ_EN;
    /*
     * Set the value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (read_en));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * this function is enable/disable design read from tcg credit table.
 * This function will enable/disable shapers accordingly in order to not cause any traffic lost.
 * however, disabling shaper will effect all the ports in the system
 */
static int
dnx_ofp_rates_tcg_read_enable_set(
    int unit,
    int core,
    uint32 read_en,
    uint32 *org_tcg_read_en_p)
{
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_field_id = DBAL_FIELD_TCG_SPR_ENA;
    if (read_en)
    {
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (*org_tcg_read_en_p));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /*
         * Get the value
         */
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) (org_tcg_read_en_p));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (0));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    dbal_field_id = DBAL_FIELD_TCG_CRDT_TABLE_READ_EN;
    /*
     * Set the value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (read_en));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*transform if_id+nif_type to nif_idx for api commands
  if_id : 0-31 
  nif_type : ILKN-0,ILKN-1, RXAUI-0 ...
  nif_idx:0-1000 (api "knows" only nif_idx values. 
*/

/* JER2_ARAD calender algorithm { */

int
dnx_ofp_rates_fixed_len_cal_build(
    int unit,
    uint32 *port_nof_slots,
    uint32 nof_ports,
    uint32 calendar_len,
    uint32 max_calendar_len,
    uint32 is_fqp_pqp,
    uint32 *calendar)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build(unit, (uint32 *) port_nof_slots, nof_ports,
                                                            calendar_len, max_calendar_len, calendar));
exit:
    SHR_FUNC_EXIT;
}
/* JER2_ARAD calender algorithm } */

int
dnx_ofp_rates_from_rates_to_calendar(
    int unit,
    uint32 *ports_rates,
    uint32 nof_ports,
    uint32 total_credit_bandwidth,
    uint32 max_calendar_len,
    dnx_ofp_rates_cal_sch_t * calendar,
    uint32 *calendar_len)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_from_rates_to_calendar
                    (unit, (uint32 *) ports_rates, nof_ports, total_credit_bandwidth, max_calendar_len, calendar->slots,
                     calendar_len));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Read indirect table scm_tbl from block EGQ,
 */
static int
dnx_egq_scm_tbl_get(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 entry_offset,
    dnx_egq_scm_tbl_data_t * egq_nif_scm_tbl_data)
{
    dbal_tables_e egq_scm_name;
    dbal_fields_e egq_cr_field_name, egq_index_field_name;
    uint32 entry_handle_id;
    void *ptr;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");
    SHR_NULL_CHECK(egq_nif_scm_tbl_data, _SHR_E_PARAM, "egq_nif_scm_tbl_data");

    ptr = sal_memset(egq_nif_scm_tbl_data, 0x0, sizeof(*egq_nif_scm_tbl_data));
    if (ptr == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "sal_memset() has failed trying to set at %s", "egq_nif_scm_tbl_data");
    }
    switch (cal_info->cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            if (cal_info->chan_arb_id > (dnx_data_egr_queuing.params.nof_calendars_get(unit) - 1))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "Table offset for EPS_OTM_HP_CRDT_TABLE (%d) is higher than allowed (%d).",
                             cal_info->chan_arb_id, (dnx_data_egr_queuing.params.nof_calendars_get(unit) - 1));
            }
            egq_scm_name = DBAL_TABLE_EGQ_SHAPING_OTM_HP_CRDT_TABLE;
            egq_cr_field_name = DBAL_FIELD_OTM_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_OTM_INDEX;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            egq_scm_name = DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE;
            egq_cr_field_name = DBAL_FIELD_QPAIR_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_QPAIR_INDEX;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            egq_scm_name = DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE;
            egq_cr_field_name = DBAL_FIELD_TCG_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_TCG_INDX;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unknown/illegal cal_info->cal_type (%d)", cal_info->cal_type);

        }
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, egq_scm_name, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DCALENDAR, entry_offset);
    /*
     * Get the values
     */
    dbal_value_field32_request(unit, entry_handle_id, egq_cr_field_name, INST_SINGLE,
                               &(egq_nif_scm_tbl_data->port_cr_to_add));
    dbal_value_field32_request(unit, entry_handle_id, egq_index_field_name, INST_SINGLE,
                               &(egq_nif_scm_tbl_data->ofp_index));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * Write indirect table scm_tbl from block EGQ,
 */
static int
dnx_egq_scm_tbl_set(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 entry_offset,
    dnx_egq_scm_tbl_data_t * egq_nif_scm_tbl_data)
{
    dbal_tables_e egq_scm_name;
    dbal_fields_e egq_cr_field_name, egq_index_field_name;
    uint32 entry_handle_id;
    dbal_tables_e egq_lp_scm_name;
    dbal_fields_e egq_lp_cr_field_name, egq_lp_index_field_name;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    switch (cal_info->cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            if (cal_info->chan_arb_id > (dnx_data_egr_queuing.params.nof_calendars_get(unit) - 1))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "Table offset for EPS_OTM_HP_CRDT_TABLE (%d) is higher than allowed (%d).",
                             cal_info->chan_arb_id, (dnx_data_egr_queuing.params.nof_calendars_get(unit) - 1));
            }
            /*
             *  Go to the correct table, per chan arb id
             */
            egq_scm_name = DBAL_TABLE_EGQ_SHAPING_OTM_HP_CRDT_TABLE;
            egq_lp_scm_name = DBAL_TABLE_EGQ_SHAPING_OTM_LP_CRDT_TABLE;
            egq_cr_field_name = DBAL_FIELD_OTM_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_OTM_INDEX;
            egq_lp_cr_field_name = DBAL_FIELD_OTM_LP_CRDT_TO_ADD;
            egq_lp_index_field_name = DBAL_FIELD_OTM_LP_INDEX;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            egq_scm_name = DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE;
            egq_cr_field_name = DBAL_FIELD_QPAIR_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_QPAIR_INDEX;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            egq_scm_name = DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE;
            egq_cr_field_name = DBAL_FIELD_TCG_CRDT_TO_ADD;
            egq_index_field_name = DBAL_FIELD_TCG_INDX;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unknown/illegal cal_info->cal_type (%d)", cal_info->cal_type);
        }
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, egq_scm_name, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DCALENDAR, entry_offset);
    /*
     * Set the values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, egq_cr_field_name, INST_SINGLE,
                                 (uint32) (egq_nif_scm_tbl_data->port_cr_to_add));
    dbal_entry_value_field32_set(unit, entry_handle_id, egq_index_field_name, INST_SINGLE,
                                 (uint32) (egq_nif_scm_tbl_data->ofp_index));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (cal_info->cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB)
    {
        /*
         * low priority need to have double size of credit
         */
        /*
         * Reuse allocated buffer.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, egq_lp_scm_name, entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DCALENDAR, entry_offset);
        /*
         * Set the values
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, egq_lp_cr_field_name, INST_SINGLE,
                                     (uint32) (egq_nif_scm_tbl_data->port_cr_to_add * 2));
        dbal_entry_value_field32_set(unit, entry_handle_id, egq_lp_index_field_name, INST_SINGLE,
                                     (uint32) (egq_nif_scm_tbl_data->ofp_index));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 *  This functions reads the calculated calendar values from the device
 *  It also reads per-port maximal burst configuration (EGQ).
 *  Note: rates_table is only used to get the per-port shaper (max burst)
 */
static int
dnx_ofp_rates_active_generic_calendars_retrieve_egq(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    dnx_ofp_rates_cal_egq_t * egq_cal,
    uint32 *egq_cal_len)
{
    int res;
    uint32 offset, slot;
    dnx_egq_scm_tbl_data_t egq_data;
    uint32 cal2get;
    dnx_ofp_egq_rates_cal_entry_t *cal_slot;
    dbal_fields_e field_cal_set, dbal_field_id;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    uint32 dbal_field_val;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");
    SHR_NULL_CHECK(egq_cal, _SHR_E_PARAM, "egq_cal");
    SHR_NULL_CHECK(egq_cal_len, _SHR_E_PARAM, "egq_cal_len");
    cal_slot = NULL;
    *egq_cal_len = 0;
    offset = 0;
    slot = 0;
    res = dnx_ofp_rates_retrieve_egress_shaper_setting_field(unit, cal_info, &field_cal_set);
    SHR_IF_ERR_EXIT(res);
    {
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value
         */
        dbal_value_field32_request(unit, entry_handle_id, field_cal_set, INST_SINGLE, &cal2get);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Read the Active EGQ calendar indirectly
     */
    /*
     * read calendar length
     */
    res = dnx_ofp_rates_egress_shaper_cal_read(unit, core, cal_info, cal2get,
                                               DNX_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN, egq_cal_len);
    SHR_IF_ERR_EXIT(res);
    /*
     * EGQ tables are double in size -
     * the second half is for get 'B' calendars/shapers.
     */
    /*
     * Calculate offset (depends on calender length and cal2get)
     */
    offset = cal2get * dnx_data_egr_queuing.params.calendar_size_get(unit);
    if (cal_info->chan_arb_id)
    {
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (cal_info->chan_arb_id - 1));
        /*
         * Get the value
         */
        dbal_field_id = (cal2get ? DBAL_FIELD_PORT_OFFSET_HIGH_B : DBAL_FIELD_PORT_OFFSET_HIGH_A);
        dbal_field_val = 0;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &dbal_field_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        offset += (1 + dbal_field_val);
    }
    if (*egq_cal_len > dnx_data_egr_queuing.params.calendar_size_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal cal length, 0x%x\n", *egq_cal_len);
    }
    for (slot = 0; slot < *egq_cal_len; ++slot)
    {
        cal_slot = egq_cal->slots + slot;
        res = dnx_egq_scm_tbl_get(unit, core, cal_info, offset + slot, &egq_data);
        SHR_IF_ERR_EXIT(res);
        cal_slot->credit = egq_data.port_cr_to_add;
        cal_slot->base_q_pair = egq_data.ofp_index;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
static int
dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 *ports_rates,
    uint32 nof_ports,
    uint32 total_shaper_bandwidth,
    uint32 max_calendar_len,
    uint32 recalc,
    uint8 add_dummy_tail,
    dnx_ofp_rates_cal_egq_t * calendar,
    uint32 *calendar_len)
{
    int res = _SHR_E_NONE;
    uint32 loc_calendar_len = nof_ports;

    SHR_FUNC_INIT_VARS(unit);

    if (!recalc)
    {
        res = dnx_ofp_rates_active_generic_calendars_retrieve_egq(unit, core, cal_info, calendar, &loc_calendar_len);
        SHR_IF_ERR_EXIT(res);
    }
    else
    {
        /*
         * In the porting proces,
         * we assume, here, that 'dnx_ofp_rates_cal_info_t' is exactly the same as
         * dnx_ofp_rates_cal_info_t.
         * We assume, here, that 'dnx_ofp_rates_cal_egq_t' is exactly the same as
         * dnx_ofp_rates_cal_egq_t.
         */
        res = dnx_algo_ofp_rates_fill_shaper_generic_calendar_credits(unit,
                                                                      core,
                                                                      (dnx_ofp_rates_cal_info_t *) cal_info,
                                                                      (uint32 *) ports_rates,
                                                                      nof_ports,
                                                                      loc_calendar_len,
                                                                      FALSE, (dnx_ofp_rates_cal_egq_t *) calendar);
        SHR_IF_ERR_EXIT(res);
    }
    *calendar_len = nof_ports;

exit:
    SHR_FUNC_EXIT;
}
static int
dnx_ofp_rates_from_generic_calendar_to_ports_egq_rate(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    dnx_ofp_rates_cal_egq_t * calendar,
    uint32 calendar_len,
    uint32 nof_ports,
    uint32 *ports_rates)
{
    uint32 port_idx, calc, slot_id, egq_resolution;
    const dnx_ofp_egq_rates_cal_entry_t *slot;
    UTILEX_U64 u64_1, u64_2;
    uint32 rem;
    uint32 calcal_length, calcal_instances;
    uint32 overflowed;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");
    SHR_NULL_CHECK(calendar, _SHR_E_PARAM, "calendar");

    if (0 == calendar_len)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal calendar len");
    }

    for (port_idx = 0; port_idx < nof_ports; ++port_idx)
    {
        ports_rates[port_idx] = 0;
    }
    for (slot_id = 0; slot_id < calendar_len; ++slot_id)
    {
        slot = &calendar->slots[slot_id];

        if (slot->base_q_pair >= DNX_EGR_NOF_Q_PAIRS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal base_q_pair");
        }
        /*
         * Increase the total sum of credit of this port_idx by the val of this slot
         */
        ports_rates[slot->base_q_pair] += slot->credit;
    }
    /*
     *  Calculate shaper rate for each port_idx according to:
     *
     *                                 total_port_credit [bits] * core_frequency [kilo-clocks]
     *  port_egq_rate [kbits/sec]= ---------------------------------------------------------
     *                                           calendar_length * slot_traverse_time [clocks]
     *
     */
    if (cal_info->cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.get(unit, core, &calcal_length));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.
                        nof_calcal_instances.get(unit, core, cal_info->chan_arb_id, &calcal_instances));
    }
    else
    {
        /*
         * there is no calcal. Assume only one entry 
         */
        calcal_length = 1;
        calcal_instances = 1;
    }
    if (cal_info->cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB)
    {
        /*
         * If callender type is chanelized arbiter EGQ resolution is 1/256 
         */
        egq_resolution = DNX_EGQ_UNITS_VAL_IN_BITS_CHAN_ARB;
    }
    else
    {
        /*
         * If callender type is QPAIR or TCG, EGQ resolution is 1/128 (not supported for A0) 
         */
        egq_resolution = DNX_EGQ_UNITS_VAL_IN_BITS_QPAIR_TCG;
    }
    for (port_idx = 0; port_idx < nof_ports; ++port_idx)
    {
        utilex_u64_multiply_longs(calcal_instances * ports_rates[port_idx], DNXCMN_CORE_CLOCK_KHZ_GET(unit), &u64_1);
        rem =
            utilex_u64_devide_u64_long(&u64_1,
                                       calcal_length * calendar_len * egq_resolution *
                                       DNX_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS, &u64_2);
        overflowed = utilex_u64_to_long(&u64_2, &(calc));
        if (overflowed)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected overflow");
        }
        calc = (rem > 0 ? calc + 1 : calc);

        ports_rates[port_idx] = calc;
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_ofp_rates.h
 */
int
dnx_ofp_rates_egq_shaper_rate_to_internal(
    int unit,
    uint32 rate_kbps,
    uint32 calendar_slots,
    uint32 calendar_size,
    uint32 *rate_internal)
{
    uint32 device_ticks_per_sec, rate_int, egq_resolution;
    UTILEX_U64 calc, calc2, calc3;

    SHR_FUNC_INIT_VARS(unit);
    device_ticks_per_sec = DNXCMN_CORE_CLOCK_HZ_GET(unit);
    if (device_ticks_per_sec == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "NOF ticks (device_ticks_per_sec) equals 0");
    }
    if (rate_kbps == 0)
    {
        rate_int = 0;
    }
    else
    {
        egq_resolution = dnx_data_egr_queuing.params.cal_res_get(unit);
        utilex_u64_multiply_longs(rate_kbps, (1000 * egq_resolution * calendar_size), &calc);
        utilex_u64_devide_u64_long(&calc, device_ticks_per_sec, &calc2);
        utilex_u64_devide_u64_long(&calc2, calendar_slots, &calc3);
        if (utilex_u64_to_long(&calc3, &rate_int))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Overflow in converting u64 (MS long 0x%08X, LS long 0x%08X) to u32",
                         calc3.arr[1], calc3.arr[0]);
        }
        /*
         * If the rate desired is smaller than the shaper resolution, 1 credit can be allocated.
         */
        if (rate_int == 0)
        {
            rate_int = 1;
        }
    }
    *rate_internal = rate_int;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egq_ofp_rates.h
 */
int
dnx_ofp_rates_egq_shaper_rate_from_internal(
    int unit,
    uint32 rate_internal,
    uint32 calendar_slots,
    uint32 calendar_size,
    uint32 *rate_kbps)
{
    uint32 device_ticks_per_sec, rate_kb, egq_resolution;
    UTILEX_U64 calc, calc2;

    SHR_FUNC_INIT_VARS(unit);

    device_ticks_per_sec = DNXCMN_CORE_CLOCK_HZ_GET(unit);
    if (rate_internal == 0)
    {
        rate_kb = 0;
    }
    else
    {
        utilex_u64_multiply_longs(rate_internal, (device_ticks_per_sec * calendar_slots), &calc);
        egq_resolution = dnx_data_egr_queuing.params.cal_res_get(unit);
        utilex_u64_devide_u64_long(&calc, (1000 * egq_resolution * calendar_size), &calc2);
        if (utilex_u64_to_long(&calc2, &rate_kb))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Overflow");
        }
    }
    *rate_kbps = rate_kb;
exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_max_credit_empty_port_set(
    int unit,
    int arg)
{
    int core;

    dbal_fields_e dbal_field_id;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * Enabling/Disabling shapers
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the value
         */
        dbal_field_id = DBAL_FIELD_EMPTY_PORT_STOP_COLLECTING_EN;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (arg ? 1 : 0));
        /*
         * Setting shapers
         */
        /*
         * Set the value
         */
        dbal_field_id = DBAL_FIELD_EMPTY_PORT_MAX_CREDIT;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (arg));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_max_credit_empty_port_get(
    int unit,
    int *arg)
{
    dbal_fields_e dbal_field_id;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Getting shapers
     */
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    core = 0;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * get the value
     */
    dbal_field_id = DBAL_FIELD_EMPTY_PORT_MAX_CREDIT;
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) (arg));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
static int
dnx_ofp_rates_tcg_id_egq_verify(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint8 *is_egq_valid)
{
    int nof_priorities;
    uint32 priority_i;
    dnx_egr_queuing_tcg_info_t tcg_port_info;
    soc_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_egq_valid, _SHR_E_PARAM, "is_egq_valid");
    *is_egq_valid = FALSE;
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    dnx_egr_queuing_tcg_info_clear(&tcg_port_info);
    SHR_IF_ERR_EXIT(dnx_egr_queuing_ofp_tcg_get(unit, core, tm_port, &tcg_port_info));
    for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
    {
        if (tcg_port_info.tcg_ndx[priority_i] == tcg_ndx)
        {
            *is_egq_valid = TRUE;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*********************************************************************
*     Update the device after the computation of the
*     calendars.
*     Details: in the H file. (search for prototype)
*********************************************************************/

static void
dnx_ofp_rates_cal_info_clear(
    dnx_ofp_rates_cal_info_t * info)
{
    if (info)
    {
        sal_memset(info, 0x0, sizeof(dnx_ofp_rates_cal_info_t));
    }
}

static int
dnx_egq_pmc_names_get(
    int unit,
    dnx_ofp_rates_cal_info_t * cal_info,
    dbal_tables_e * egq_pmc_name,
    dbal_fields_e * egq_cr_field_name)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");
    SHR_NULL_CHECK(egq_pmc_name, _SHR_E_PARAM, "egq_pmc_name");
    SHR_NULL_CHECK(egq_cr_field_name, _SHR_E_PARAM, "egq_cr_field_name");
    switch (cal_info->cal_type)
    {
        case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
        {
            *egq_pmc_name = DBAL_TABLE_EGQ_SHAPING_OTM_HP_CRDT_TABLE;
            *egq_cr_field_name = DBAL_FIELD_OTM_MAX_BURST;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
        {
            *egq_pmc_name = DBAL_TABLE_EGQ_SHAPING_QP_CREDIT_TABLE;
            *egq_cr_field_name = DBAL_FIELD_QPAIR_MAX_BURST;
            break;
        }
        case DNX_OFP_RATES_EGQ_CAL_TCG:
        {
            *egq_pmc_name = DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE;
            *egq_cr_field_name = DBAL_FIELD_TCG_MAX_BURST;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid cal_type (%d)", cal_info->cal_type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Read indirect table pmc_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
static int
dnx_egq_pmc_tbl_get(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 entry_offset,
    dnx_egq_pmc_tbl_data_t * egq_pmc_tbl_data)
{
    void *ptr;
    dbal_tables_e egq_pmc_name;
    dbal_fields_e egq_max_credit_field_name;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    ptr = sal_memset(egq_pmc_tbl_data, 0x0, sizeof(dnx_egq_pmc_tbl_data_t));
    if (ptr == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "sal_memset() failed on setting egq_pmc_tbl_data");
    }
    SHR_IF_ERR_EXIT(dnx_egq_pmc_names_get(unit, cal_info, &egq_pmc_name, &egq_max_credit_field_name));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, egq_pmc_name, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DCALENDAR, (uint32) (entry_offset));
    /*
     * Get the value
     */
    dbal_value_field32_request(unit, entry_handle_id, egq_max_credit_field_name, INST_SINGLE,
                               (uint32 *) (&(egq_pmc_tbl_data->port_max_credit)));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * Write indirect table pmc_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
static int
dnx_egq_pmc_tbl_set(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 entry_offset,
    dnx_egq_pmc_tbl_data_t * egq_pmc_tbl_data)
{
    dbal_tables_e egq_pmc_name;
    dbal_fields_e egq_max_credit_field_name;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_egq_pmc_names_get(unit, cal_info, &egq_pmc_name, &egq_max_credit_field_name));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, egq_pmc_name, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DCALENDAR, (uint32) (entry_offset));
    /*
     * Set the value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, egq_max_credit_field_name, INST_SINGLE,
                                 (uint32) (egq_pmc_tbl_data->port_max_credit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    if (egq_pmc_name == DBAL_TABLE_EGQ_SHAPING_OTM_HP_CRDT_TABLE)
    {
        egq_pmc_name = DBAL_TABLE_EGQ_SHAPING_OTM_LP_CRDT_TABLE;
        /*
         * Reuse allocated buffer.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, egq_pmc_name, entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DCALENDAR, (uint32) (entry_offset));
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, egq_max_credit_field_name, INST_SINGLE,
                                     (uint32) (egq_pmc_tbl_data->port_max_credit * 2));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*********************************************/
/***************STOP PORT AUX*****************/
/*********************************************/

static int
dnx_ofp_rates_active_egq_generic_calendars_config(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    dnx_ofp_rates_cal_egq_t * egq_cal,
    uint32 egq_cal_len)
{
    uint32 fld_val, egq_to_set, offset, slot;
    dnx_egq_scm_tbl_data_t egq_data;
    dnx_egq_pmc_tbl_data_t egq_pmc_data;
    dbal_fields_e field_cal_set_name;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    offset = 0;
    slot = 0;
    SHR_NULL_CHECK(egq_cal, _SHR_E_PARAM, "egq_cal");
    SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");
    /*
     *  Verify calendar length validity
     */
    if ((egq_cal_len > dnx_data_egr_queuing.params.calendar_size_get(unit)) || (egq_cal_len < 1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "egq_cal_len (%d) is out of range (%d,%d)", egq_cal_len, 1,
                     dnx_data_egr_queuing.params.calendar_size_get(unit));
    }
    /*
     * Check which calendars (Calendars set 'A' or 'B')
     * are currently active. Then build the non-active calendars,
     * and finally swap between the active calendars and the non-active ones.
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_retrieve_egress_shaper_setting_field(unit, cal_info, &field_cal_set_name));
    {
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;

        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value
         */
        dbal_value_field32_request(unit, entry_handle_id, field_cal_set_name, INST_SINGLE, &fld_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    egq_to_set = (fld_val == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
    /*
     * update both set calendar lengths
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egress_shaper_cal_write(unit, core, cal_info, egq_to_set,
                                                          DNX_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN, egq_cal_len));
    /*
     * Write to the non-Active EGQ calendar indirectly
     */
    /*
     * Tables are double in size -
     * The second half is for set 'B' calendars/shapers.
     */
    offset = egq_to_set * dnx_data_egr_queuing.params.calendar_size_get(unit);
    if (cal_info->chan_arb_id)
    {
        offset += cal_info->arb_slot_id;
    }
    for (slot = 0; slot < egq_cal_len; ++slot)
    {
        egq_data.port_cr_to_add = egq_cal->slots[slot].credit;
        egq_data.ofp_index = egq_cal->slots[slot].base_q_pair;
        SHR_IF_ERR_EXIT(dnx_egq_scm_tbl_set(unit, core, cal_info, offset + slot, &egq_data));
        switch (cal_info->cal_type)
        {
            case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.
                                burst.get(unit, core, egq_data.ofp_index, &egq_pmc_data.port_max_credit));
                break;
            }
            case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.
                                burst.get(unit, core, egq_data.ofp_index, &egq_pmc_data.port_max_credit));
                break;
            }
            case DNX_OFP_RATES_EGQ_CAL_TCG:
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.
                                burst.get(unit, core, egq_data.ofp_index / DNX_EGR_NOF_Q_PAIRS_IN_PS,
                                          egq_data.ofp_index % DNX_EGR_NOF_Q_PAIRS_IN_PS,
                                          &egq_pmc_data.port_max_credit));
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid calendar type %u", cal_info->cal_type);
            }
        }
        egq_pmc_data.port_max_credit *= dnx_data_egr_queuing.params.cal_burst_res_get(unit);
        SHR_IF_ERR_EXIT(dnx_egq_pmc_tbl_set(unit, core, cal_info, offset + slot, &egq_pmc_data));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_sw_db_port_priority_nof_valid_queues_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 priority_ndx,
    uint32 *nof_valid_queues)
{
    int base_q_pair, port_base_q_pair, nof_priorities;
    uint32 fap_port_ndx, ptc_ndx, is_valid, is_ptc_found;
    bcm_pbmp_t pbmp;
    bcm_port_t port_i;
    int core_i;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_valid_queues, _SHR_E_PARAM, "nof_valid_queues");
    is_ptc_found = FALSE;
    *nof_valid_queues = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &port_base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
    BCM_PBMP_ITER(pbmp, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port_i, &core_i, &fap_port_ndx));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &nof_priorities));
        for (ptc_ndx = 0; ptc_ndx < nof_priorities; ++ptc_ndx)
        {
            if ((port_base_q_pair == base_q_pair) && (ptc_ndx == priority_ndx))
            {
                is_ptc_found = TRUE;
            }
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.valid.get(unit, core, base_q_pair + ptc_ndx, &is_valid));
            if (is_valid)
            {
                *nof_valid_queues += 1;
            }
        }
    }
    if (!is_ptc_found)
    {
        *nof_valid_queues += 1;
    }
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_sw_db_tcg_nof_valid_entries_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 tcg,
    uint32 *nof_valid_entries)
{
    int nof_priorities, base_q_pair;
    uint32 fap_port_ndx, ps, tcg_ps, is_tcg_found, tcg_ndx, is_valid, port_i;
    bcm_pbmp_t pbmp;
    int core_i;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_valid_entries, _SHR_E_PARAM, "nof_valid_entries");
    is_tcg_found = FALSE, *nof_valid_entries = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    tcg_ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
    BCM_PBMP_ITER(pbmp, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port_i, &core_i, &fap_port_ndx));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &nof_priorities));
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        for (tcg_ndx = 0; tcg_ndx < nof_priorities; ++tcg_ndx)
        {
            if ((ps == tcg_ps) && (tcg_ndx == tcg))
            {
                is_tcg_found = TRUE;
            }
            SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.valid.get(unit, core, ps, tcg_ndx, &is_valid));
            if (is_valid)
            {
                *nof_valid_entries += 1;
            }
        }
    }
    if (!is_tcg_found)
    {
        *nof_valid_entries += 1;
    }
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_port_priority_max_burst_for_fc_queues_verify(
    int unit,
    uint32 rate)
{

    SHR_FUNC_INIT_VARS(unit);

    if (rate > DNX_OFP_RATES_BURST_FC_Q_LIMIT_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "rate is above max");
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_port_priority_max_burst_for_empty_queues_verify(
    int unit,
    uint32 rate)
{

    SHR_FUNC_INIT_VARS(unit);

    if (rate > DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "rate is above max");
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_egq_tcg_shaper_verify(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint32 rate,
    dnx_ofp_rates_generic_func_state_e set_state)
{
    int base_q_pair, nof_priorities;
    uint32 nof_valid_entries, max_kbps;
    uint8 is_egq_valid;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    nof_valid_entries = 0;

    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (nof_priorities < dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "nof prioritiees (%d) doesn't support tcg It is below %d)",
                     nof_priorities, dnx_data_egr_queuing.params.tcg_min_priorities_get(unit));
    }
    /*
     * DNX_TCG_MIN may be changed and be greater then zero.
     */
    /*
     * coverity[unsigned_compare] 
     */
    if (((int) tcg_ndx > DNX_TCG_MAX) || ((int) tcg_ndx < DNX_TCG_MIN))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "tcg ndx (%d) is out of range (%d,%d)", tcg_ndx, DNX_TCG_MIN, DNX_TCG_MAX);
    }
    /*
     * In case TCG is not being mapped by any other port priority. Rate must be 0 (disabled).
     */
    switch (set_state)
    {
        case DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_BURST:
        {
            if (rate > DNX_OFP_RATES_BURST_LIMIT_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "burst rate (%d) is above max burst rate (%d)", rate,
                             DNX_OFP_RATES_BURST_LIMIT_MAX);
            }
            break;
        }
        case DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_RATE:
        {
            SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_id_egq_verify(unit, core, tm_port, tcg_ndx, &is_egq_valid));
            max_kbps = (is_egq_valid) ? DNX_IF_MAX_RATE_KBPS(unit) : 0;
            if (rate > max_kbps)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "rate (%d) is above max %d)", rate, max_kbps);
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal 'set_state' enum (%d)", set_state);
        }
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_sw_db_tcg_nof_valid_entries_get(unit, core, tm_port, tcg_ndx, &nof_valid_entries));
    if (nof_valid_entries > DNX_EGR_NOF_TCG_IDS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "egq_cal_len is out of range");
    }
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_egq_port_priority_shaper_verify(
    int unit,
    int core,
    uint32 tm_port,
    uint32 priority_ndx,
    uint32 rate,
    dnx_ofp_rates_generic_func_state_e set_state)
{
    int base_q_pair, nof_priorities;
    uint32 nof_valid_queues;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (priority_ndx > nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Priority index (%d) is out of range (larger than %d)", priority_ndx,
                     nof_priorities);
    }
    switch (set_state)
    {
        case DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_RATE:
        {
            if (rate > DNX_IF_MAX_RATE_KBPS(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "rate above max");
            }
            break;
        }
        case DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_BURST:
        {
            if (rate > DNX_OFP_RATES_BURST_LIMIT_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "burst above max");
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal enum");
        }
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_sw_db_port_priority_nof_valid_queues_get(unit,
                                                                           core,
                                                                           tm_port, priority_ndx, &nof_valid_queues));
    if (nof_valid_queues > DNX_EGR_NOF_Q_PAIRS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_valid_queues  %d) is out of range (larger than %d)", nof_valid_queues,
                     DNX_EGR_NOF_Q_PAIRS);
    }
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_single_port_verify(
    int unit,
    uint32 *rate)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get interface id
     */
    if (rate)
    {
        if (*rate > DNX_IF_MAX_RATE_KBPS(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "rate (%d) is above max (%d)", *rate, DNX_IF_MAX_RATE_KBPS(unit));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egq_single_port_rate_hw_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *rate)
{
    uint32 egq_cal_len, org_if_val, org_otm_val, *egq_rates = NULL;
    int base_q_pair;
    dnx_ofp_rates_cal_egq_t egq_cal;
    dnx_ofp_rates_cal_info_t cal_info;
    bcm_port_t logical_port;
    uint32 nof_instances;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(rate, _SHR_E_PARAM, "rate");
    SHR_ALLOC(egq_rates, sizeof(uint32) * DNX_EGR_NOF_BASE_Q_PAIRS, "egq_rates", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    dnx_ofp_rates_cal_info_clear(&cal_info);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_port2chan_cal_get(unit, core, tm_port, &cal_info.chan_arb_id));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.nof_calcal_instances.get(unit, core, cal_info.chan_arb_id, &nof_instances));

    /*
     * all ports on the interface have rate=0, no calendar was allocated 
     */
    if (nof_instances == 0)
    {
        *rate = 0;
    }
    else
    {
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_CHAN_ARB;
        egq_cal_len = 0;
        /*
         * Retrieve calendars
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_active_generic_calendars_retrieve_egq(unit,
                                                                            core, &cal_info, &egq_cal, &egq_cal_len));
        /*
         * Calculate egq rates
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_from_generic_calendar_to_ports_egq_rate(unit,
                                                                              core,
                                                                              &cal_info,
                                                                              &egq_cal,
                                                                              egq_cal_len,
                                                                              DNX_EGR_NOF_BASE_Q_PAIRS, egq_rates));
        /*
         * Get the rate of the requested port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        *rate = egq_rates[base_q_pair];
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
exit:
    SHR_FREE(egq_rates);
    SHR_FUNC_EXIT;
}

/*
 * This procedure is invoked nowahere in this code. It is left here in case it
 * can be used by soem future API.
 */
int
dnx_ofp_rates_egq_single_port_rate_sw_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *rate)
{
    dnx_algo_port_type_e port_type;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * in case of non-egq ports - return 0
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
    {
        *rate = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.rate.get(unit, core, tm_port, rate));
    }

exit:
    SHR_FUNC_EXIT;

}

int
dnx_ofp_rates_calcal_config(
    int unit,
    int core)
{
    int res = _SHR_E_NONE;
    uint32
        idx,
        act_cal,
        chan_arb_i,
        egq_calcal_chan_arb_rate_requested,
        egq_calcal_calendar_len,
        egq_calcal_rates[DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS],
        egq_calcal_instances[DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS];
    dnx_ofp_rates_cal_sch_t *egq_calcal_calendar = NULL;
    dnx_ofp_rates_cal_set_e cal2set;
    uint32 interface_select;
    uint32 org_if_val, org_otm_val;
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    egq_calcal_chan_arb_rate_requested = 0;
    egq_calcal_calendar_len = 0;
    SHR_ALLOC(egq_calcal_calendar, sizeof(*egq_calcal_calendar), "egq_calcal_calendar", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);
    /*
     * Clear
     */
    egq_calcal_chan_arb_rate_requested = 0;
    sal_memset(egq_calcal_instances, 0, sizeof(egq_calcal_instances));
    /*
     * Calculate CalCal according to the calendar ofp rates
     */
    for (chan_arb_i = 0; chan_arb_i < dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit); chan_arb_i++)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.rate.get(unit, core, chan_arb_i, &egq_calcal_rates[chan_arb_i]));
        egq_calcal_chan_arb_rate_requested += egq_calcal_rates[chan_arb_i];
    }
    /*
     * Convert rates to calendar
     */
    res = dnx_ofp_rates_from_rates_to_calendar(unit,
                                               egq_calcal_rates,
                                               dnx_data_egr_queuing.params.nof_calendars_get(unit),
                                               egq_calcal_chan_arb_rate_requested,
                                               DNX_OFP_RATES_CALCAL_LEN_EGQ_MAX,
                                               egq_calcal_calendar, &egq_calcal_calendar_len);
    SHR_IF_ERR_EXIT(res);
    /*
     * Get the active calendar
     */
    /*
     * Allocate buffer.
     */
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Get the value of OTM_SPR_SET_SELf
     */
    dbal_field_id = DBAL_FIELD_OTM_SPR_SET_SEL;
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &act_cal);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    cal2set = (act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
    /*
     * Write CalCal length to the inactive calendar
     */
    if (cal2set == DNX_OFP_RATES_CAL_SET_A)
    {
        dbal_field_id = DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_A;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_B;
    }
    /*
     * Set the value of CAL_CAL_LENGTH_FOR_OTM_SPR_ A/B
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE,
                                 (uint32) (egq_calcal_calendar_len - 1));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    res = dnx_ofp_rate_db.calcal_len.set(unit, core, egq_calcal_calendar_len);
    SHR_IF_ERR_EXIT(res);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    /*
     * Write CalCal entries, calculate nof_calal instances per chan_cal
     */
    /*
     * Reuse allocated buffer.
     */
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_CAL_CAL_CALENDAR;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    /*
     * key construction. Fixed part
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, (uint32) core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_SELECT, (uint32) cal2set);

    for (idx = 0; idx < egq_calcal_calendar_len; ++idx)
    {
        egq_calcal_instances[egq_calcal_calendar->slots[idx]] += 1;
        interface_select = egq_calcal_calendar->slots[idx];
        /*
         * key construction. Variable part
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_SLOT_ID, (uint32) (idx));
        dbal_field_id = DBAL_FIELD_CAL_INDX;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (interface_select));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
    /*
     * Set calal nof instances per chan_arb to sw_db
     */
    for (chan_arb_i = 0; chan_arb_i < dnx_data_egr_queuing.params.nof_calendars_get(unit); ++chan_arb_i)
    {
        res =
            dnx_ofp_rate_db.otm_cal.nof_calcal_instances.set(unit, core, chan_arb_i, egq_calcal_instances[chan_arb_i]);
        SHR_IF_ERR_EXIT(res);
    }
exit:
    SHR_FREE(egq_calcal_calendar);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_calcal_copy(
    int unit,
    int core)
{
    uint32 idx, entry_handle_id, calendar_len = 0, interface_select = 0;
    dnx_ofp_rates_cal_set_e cal2set, act_cal;
    dbal_fields_e dbal_field_id;
    uint32 org_if_val, org_otm_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the active calendar
     */
    /*
     * Allocate buffers.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, &entry_handle_id));
    /*
     * Key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Get the value of OTM_SPR_SET_SELf
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OTM_SPR_SET_SEL, INST_SINGLE, &act_cal);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    cal2set = ((act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A);
    /*
     * Get the active calendar's length
     * Choose the appropriate DBAL field to read based on the current active calendar
     */
    dbal_field_id =
        ((act_cal ==
          DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_A : DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_B);
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &calendar_len);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Set the calendar's length to the inactive calendar
     * Choose the appropriate DBAL field to write based on the current inactive calendar
     */
    dbal_field_id =
        ((cal2set ==
          DNX_OFP_RATES_CAL_SET_A) ? DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_A : DBAL_FIELD_CAL_CAL_LENGTH_FOR_OTM_SPR_B);
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, calendar_len);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * Added only because shaper configuration under traffic is causing losses
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }

    /*
     * Get the CalCal entries of the active calendar and copy them to the calendar2set without recalculating
     */
    /*
     * Reuse allocated buffer.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGQ_SHAPER_CAL_CAL_CALENDAR, entry_handle_id));
    /*
     * Key construction. Fixed part
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, (uint32) core);

    for (idx = 0; idx < calendar_len + 1; ++idx)
    {
        /*
         * Key construction variable part. Get interface_select from active calendar
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_SELECT, (uint32) act_cal);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_SLOT_ID, idx);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CAL_INDX, INST_SINGLE, &interface_select);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Key construction variable part. Set interface_select to the  calendar2set
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_SELECT, (uint32) cal2set);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_SLOT_ID, idx);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_INDX, INST_SINGLE, interface_select);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * Added only because shaper configuration under traffic is causing losses
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#if (1)
/* { */

int
dnx_ofp_rates_egq_single_port_rate_hw_set(
    int unit,
    bcm_core_t core)
{
    int base_q_pair;
    uint32
        tm_port_i,
        egq_calendar_len,
        cur_chan_arb_i, egq_if_rate_requested, recalc, act_cal, org_if_val, org_otm_val, nof_instances, cal_slots;
    uint32 is_valid;
    uint32 is_cal_modified, is_calcal_modified;
    dnx_ofp_rates_cal_info_t cal_info;
    dnx_ofp_rates_cal_set_e cal2set;
    bcm_pbmp_t pbmp;
    int chan_arb_i;
    bcm_port_t port_i;
    /*
     * Initialize all pointers which are released at 'exit'
     */
    dnx_ofp_rates_cal_egq_t *egq_calendar = NULL;
    uint32 *egq_rates = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    egq_if_rate_requested = 0;
    is_cal_modified = FALSE;
    is_calcal_modified = FALSE;
    SHR_ALLOC(egq_rates, sizeof(uint32) * dnx_data_egr_queuing.params.nof_q_pairs_get(unit), "egq_rates", "%s%s%s\r\n",
              EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(egq_calendar, sizeof(*egq_calendar), "egq_calendar", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    sal_memset(egq_calendar, 0, sizeof(dnx_ofp_rates_cal_egq_t));
    cal_info.arb_slot_id = 0;
    for (chan_arb_i = 0; chan_arb_i < dnx_data_egr_queuing.params.nof_calendars_get(unit); ++chan_arb_i)
    {
        cal_slots = 0;
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.nof_calcal_instances.get(unit, core, chan_arb_i, &nof_instances));
        if (nof_instances > 0)
        {
            /*
             * Retrieve rates from the software database
             */
            egq_if_rate_requested = 0;
            sal_memset(egq_rates, 0, dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * sizeof(uint32));
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
            BCM_PBMP_ITER(pbmp, port_i)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port_i, &core, &tm_port_i));
                SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
                SHR_IF_ERR_EXIT(dnx_ofp_rates_port2chan_cal_get(unit, core, tm_port_i, &cur_chan_arb_i));
                if (cur_chan_arb_i != chan_arb_i)
                {
                    /*
                     * handle current calendar
                     */
                    continue;
                }
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.valid.get(unit, core, tm_port_i, &is_valid));
                if (is_valid)
                {
                    /*
                     * Get egq rate
                     */
                    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.
                                    rate.get(unit, core, tm_port_i, &egq_rates[base_q_pair]));
                    egq_if_rate_requested += egq_rates[base_q_pair];
                    egq_calendar->slots[cal_slots].base_q_pair = base_q_pair;
                    cal_slots++;
                }
            }
            if (cal_slots)
            {
                /*
                 * HW limitation
                 * only calendar zero (CPU) can point to slot zero. if calendar zero does not exist,
                 * calendars need to start from slot 1
                 */
                if (chan_arb_i && cal_info.arb_slot_id == 0)
                {
                    cal_info.arb_slot_id++;
                }
                /*
                 * Configure calendar
                 */
                cal_info.chan_arb_id = chan_arb_i;
                cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_CHAN_ARB;
                /*
                 * calendar will need to be recalculated only if cal cal has been changed or
                 * the interface rate has been changed
                 */
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.modified.get(unit, core, chan_arb_i, &is_cal_modified));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_is_modified.get(unit, core, &is_calcal_modified));
                recalc = is_calcal_modified | is_cal_modified;
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.modified.set(unit, core, chan_arb_i, FALSE));
                SHR_IF_ERR_EXIT(dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar(unit,
                                                                                       core,
                                                                                       &cal_info,
                                                                                       egq_rates,
                                                                                       cal_slots,
                                                                                       egq_if_rate_requested,
                                                                                       dnx_data_port.
                                                                                       general.nof_tm_ports_get(unit),
                                                                                       recalc, FALSE, egq_calendar,
                                                                                       &egq_calendar_len));
                /*
                 * write to inactive calendar slots(ports rates) and length
                 */
                SHR_IF_ERR_EXIT(dnx_ofp_rates_active_egq_generic_calendars_config(unit,
                                                                                  core,
                                                                                  &cal_info,
                                                                                  egq_calendar, egq_calendar_len));
            }
        }
        cal_info.arb_slot_id += cal_slots;
    }
    {
        /*
         * change active active calendar
         */
        uint32 entry_handle_id;
        dbal_tables_e dbal_table_id;
        dbal_fields_e dbal_field_id;
        /*
         * Allocate buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value of OTM_SPR_SET_SELf
         */
        dbal_field_id = DBAL_FIELD_OTM_SPR_SET_SEL;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &act_cal);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        cal2set = (act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
        /*
         * Set the value of the new 'set'
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (cal2set));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }
exit:
    SHR_FREE(egq_rates);
    SHR_FREE(egq_calendar);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_max_burst_generic_set(
    int unit,
    int core,
    uint32 base_q_pair,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 max_burst)
{
    uint32 fld_val, egq_to_set, cal_len, slot, offset;
    dbal_fields_e field_cal_set_name;
    dnx_egq_pmc_tbl_data_t pmc_tbl_data;
    dnx_egq_scm_tbl_data_t scm_tbl_data;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");
    SHR_IF_ERR_EXIT(dnx_ofp_rates_retrieve_egress_shaper_setting_field(unit, cal_info, &field_cal_set_name));
    {
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value
         */
        dbal_value_field32_request(unit, entry_handle_id, field_cal_set_name, INST_SINGLE, &fld_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    egq_to_set = (fld_val == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_A : DNX_OFP_RATES_CAL_SET_B;
    pmc_tbl_data.port_max_credit = max_burst;
    /*
     * read calendar length
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egress_shaper_cal_read(unit, core, cal_info, egq_to_set,
                                                         DNX_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN, &cal_len));
    if (cal_len == 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No Shaping set! Set Shaping before setting Burst Max.");
    }
    offset = egq_to_set * dnx_data_egr_queuing.params.calendar_size_get(unit);
    if (cal_info->chan_arb_id)
    {
        uint32 dbal_field_val;
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (cal_info->chan_arb_id - 1));
        /*
         * Get the value
         */
        dbal_field_id = (egq_to_set ? DBAL_FIELD_PORT_OFFSET_HIGH_B : DBAL_FIELD_PORT_OFFSET_HIGH_A);
        dbal_field_val = 0;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &dbal_field_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        offset += (1 + dbal_field_val);
    }

    for (slot = 0; slot < cal_len; ++slot)
    {
        SHR_IF_ERR_EXIT(dnx_egq_scm_tbl_get(unit, core, cal_info, slot + offset, &scm_tbl_data));
        if (scm_tbl_data.ofp_index == base_q_pair)
        {
            SHR_IF_ERR_EXIT(dnx_egq_pmc_tbl_set(unit, core, cal_info, slot + offset, &pmc_tbl_data));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egq_port_priority_max_burst_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 priority_ndx,
    uint32 max_burst)
{
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 org_qp_val, egq_tcg_qpair_shaper_enable;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&cal_info, 0, sizeof(cal_info));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, FALSE, &org_qp_val));
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_shaper_verify(unit,
                                                                  core,
                                                                  tm_port,
                                                                  priority_ndx,
                                                                  max_burst,
                                                                  DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_BURST));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable));
    if (egq_tcg_qpair_shaper_enable)
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Setting max burst
         */
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY;
        SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_set(unit,
                                                            core,
                                                            (base_q_pair + priority_ndx),
                                                            &cal_info,
                                                            max_burst *
                                                            dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, TRUE, &org_qp_val));
    }
exit:
    SHR_FUNC_EXIT;

}

int
dnx_ofp_rates_egq_tcg_max_burst_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 tcg_ndx,
    uint32 max_burst)
{
    uint32 org_tcg_val, tcg_id, ps;
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 egq_tcg_qpair_shaper_enable;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, FALSE, &org_tcg_val));
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_shaper_verify(unit,
                                                        core,
                                                        tm_port,
                                                        tcg_ndx,
                                                        max_burst, DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_BURST));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable));
    if (egq_tcg_qpair_shaper_enable)
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Getting tcg_id
         */
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        tcg_id = DNX_OFP_RATES_TCG_ID_GET(ps, tcg_ndx);
        /*
         * Setting max burst
         */
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_TCG;
        cal_info.chan_arb_id = 0;
        SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_set(unit,
                                                            core,
                                                            tcg_id,
                                                            &cal_info,
                                                            max_burst *
                                                            dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, TRUE, &org_tcg_val));
    }
exit:
    SHR_FUNC_EXIT;

}

int
dnx_ofp_rates_single_port_max_burst_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 max_burst)
{
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    bcm_port_t logical_port;
    uint32 org_if_val, org_otm_val;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_single_port_verify(unit, &max_burst));
    /*
     * Getting the fap port's base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * Setting max burst
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm.shaping.burst.set(unit, core, base_q_pair, max_burst));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_port2chan_cal_get(unit, core, tm_port, &cal_info.chan_arb_id));
    cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_CHAN_ARB;
    SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_set(unit,
                                                        core,
                                                        base_q_pair,
                                                        &cal_info,
                                                        max_burst *
                                                        dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_ofp_rates_max_burst_generic_get(
    int unit,
    int core,
    uint32 base_q_pair,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 *max_burst)
{
    uint32 egq_to_get, fld_val, cal_len, slot, offset;
    dbal_fields_e field_cal_set_name;
    dnx_egq_pmc_tbl_data_t pmc_tbl_data;
    dnx_egq_scm_tbl_data_t scm_tbl_data;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(max_burst, _SHR_E_PARAM, "max_burst");
    SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");

    SHR_IF_ERR_EXIT(dnx_ofp_rates_retrieve_egress_shaper_setting_field(unit, cal_info, &field_cal_set_name));
    {
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value
         */
        dbal_value_field32_request(unit, entry_handle_id, field_cal_set_name, INST_SINGLE, &fld_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    egq_to_get = (fld_val == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_A : DNX_OFP_RATES_CAL_SET_B;
    /*
     * read calendar length
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egress_shaper_cal_read(unit, core, cal_info, egq_to_get,
                                                         DNX_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN, &cal_len));
    offset = egq_to_get * dnx_data_egr_queuing.params.calendar_size_get(unit);
    if (cal_info->chan_arb_id)
    {
        uint32 dbal_field_val;
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_OTM_SHAPER_LENGTH;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ID, (uint32) (cal_info->chan_arb_id - 1));
        /*
         * Get the value
         */
        dbal_field_id = (egq_to_get ? DBAL_FIELD_PORT_OFFSET_HIGH_B : DBAL_FIELD_PORT_OFFSET_HIGH_A);
        dbal_field_val = 0;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &dbal_field_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        offset += (1 + dbal_field_val);
    }
    for (slot = 0; slot < cal_len; ++slot)
    {
        SHR_IF_ERR_EXIT(dnx_egq_scm_tbl_get(unit, core, cal_info, slot + offset, &scm_tbl_data));
        if (scm_tbl_data.ofp_index == base_q_pair)
        {
            SHR_IF_ERR_EXIT(dnx_egq_pmc_tbl_get(unit, core, cal_info, slot + offset, &pmc_tbl_data));
            *max_burst = pmc_tbl_data.port_max_credit;
            break;
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_single_port_max_burst_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *max_burst)
{
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 org_if_val, org_otm_val;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, FALSE, &org_if_val, &org_otm_val));
    }
    SHR_NULL_CHECK(max_burst, _SHR_E_PARAM, "max_burst");
    /*
     * Getting the fap port's base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * Getting max burst
     */
    cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_CHAN_ARB;
    SHR_IF_ERR_EXIT(dnx_ofp_rates_port2chan_cal_get(unit, core, tm_port, &cal_info.chan_arb_id));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_get(unit, core, base_q_pair, &cal_info, max_burst));
    *max_burst = *max_burst / dnx_data_egr_queuing.params.cal_burst_res_get(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_port_read_enable_set(unit, core, TRUE, &org_if_val, &org_otm_val));
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egq_tcg_max_burst_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 tcg_ndx,
    uint32 *max_burst)
{
    uint32 org_tcg_val, tcg_id, ps;
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 egq_tcg_qpair_shaper_enable;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, FALSE, &org_tcg_val));
    }
    SHR_NULL_CHECK(max_burst, _SHR_E_PARAM, "max_burst");
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable));
    if (egq_tcg_qpair_shaper_enable)
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Getting tcg_id
         */
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        tcg_id = DNX_OFP_RATES_TCG_ID_GET(ps, tcg_ndx);
        /*
         * Setting max burst
         */
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_TCG;
        cal_info.chan_arb_id = 0;
        SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_get(unit, core, tcg_id, &cal_info, max_burst));
    }
    *max_burst = *max_burst / dnx_data_egr_queuing.params.cal_burst_res_get(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, TRUE, &org_tcg_val));
    }
exit:
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egq_port_priority_max_burst_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 priority_ndx,
    uint32 *max_burst)
{
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 egq_tcg_qpair_shaper_enable, org_tc_val;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&cal_info, 0, sizeof(cal_info));
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, FALSE, &org_tc_val));
    }
    SHR_NULL_CHECK(max_burst, _SHR_E_PARAM, "max_burst");
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable));
    if (egq_tcg_qpair_shaper_enable)
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Getting max burst
         */
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY;
        SHR_IF_ERR_EXIT(dnx_ofp_rates_max_burst_generic_get(unit,
                                                            core, (base_q_pair + priority_ndx), &cal_info, max_burst));
    }
    *max_burst = *max_burst / dnx_data_egr_queuing.params.cal_burst_res_get(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, TRUE, &org_tc_val));
    }

exit:
    SHR_FUNC_EXIT;

}

int
dnx_ofp_rates_port_priority_max_burst_for_empty_queues_set(
    int unit,
    uint32 max_burst_empty_queues)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_ofp_rates_port_priority_max_burst_for_empty_queues_verify(unit, max_burst_empty_queues));
    /*
     * Max burst for empty and fc queues {
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * Enabling/Disabling shapers
         */
        uint32 entry_handle_id;
        uint32 dbal_field_val;
        dbal_tables_e dbal_table_id;
        dbal_fields_e dbal_field_id;
        /*
         * Allocate buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_field_id = DBAL_FIELD_EMPTY_QP_STOP_COLLECTING_EN;
        /*
         * Set the value of 'EMPTY_QP_STOP_COLLECTING_EN'
         */
        dbal_field_val = (max_burst_empty_queues ? 1 : 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Setting shapers
         */
        dbal_field_id = DBAL_FIELD_EMPTY_QP_MAX_CREDIT;
        /*
         * Set the value of 'EMPTY_QP_MAX_CREDIT'
         */
        dbal_field_val = max_burst_empty_queues;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Max burst for empty and fc queues }
     */
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_port_priority_max_burst_for_fc_queues_set(
    int unit,
    uint32 max_burst_fc_queues)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_ofp_rates_port_priority_max_burst_for_fc_queues_verify(unit, max_burst_fc_queues));
    /*
     * Max burst for empty and fc queues {
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * Enabling/Disabling shapers
         */
        uint32 entry_handle_id;
        uint32 dbal_field_val;
        dbal_tables_e dbal_table_id;
        dbal_fields_e dbal_field_id;
        /*
         * Allocate buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_field_id = DBAL_FIELD_FC_QP_STOP_COLLECTING_EN;
        /*
         * Set the value of 'FC_QP_STOP_COLLECTING_EN'
         */
        dbal_field_val = (max_burst_fc_queues ? 1 : 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Setting shapers
         */
        dbal_field_id = DBAL_FIELD_FC_QP_MAX_CREDIT;
        /*
         * Set the value of 'FC_QP_MAX_CREDIT'
         */
        dbal_field_val = max_burst_fc_queues;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Max burst for empty and fc queues }
     */
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_port_priority_max_burst_for_fc_queues_get(
    int unit,
    uint32 *max_burst_fc_queues)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(max_burst_fc_queues, _SHR_E_PARAM, "max_burst_fc_queues");
    /*
     * Max burst for empty and fc queues {
     */
    {
        uint32 entry_handle_id;
        uint32 dbal_field_val;
        dbal_tables_e dbal_table_id;
        dbal_fields_e dbal_field_id;
        /*
         * Allocate buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        core = 0;
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_field_id = DBAL_FIELD_FC_QP_MAX_CREDIT;
        /*
         * Get the value
         */
        dbal_field_val = 0;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &dbal_field_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        *max_burst_fc_queues = dbal_field_val;
    }
    /*
     * Max burst for empty and fc queues }
     */
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_port_priority_max_burst_for_empty_queues_get(
    int unit,
    uint32 *max_burst_empty_queues)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(max_burst_empty_queues, _SHR_E_PARAM, "max_burst_empty_queues");
    /*
     * Max burst for empty and fc queues {
     */
    {
        uint32 entry_handle_id;
        uint32 dbal_field_val;
        dbal_tables_e dbal_table_id;
        dbal_fields_e dbal_field_id;
        /*
         * Allocate buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        core = 0;
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_field_id = DBAL_FIELD_EMPTY_QP_MAX_CREDIT;
        /*
         * Get the value
         */
        dbal_field_val = 0;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &dbal_field_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        *max_burst_empty_queues = dbal_field_val;
    }
    /*
     * Max burst for empty and fc queues }
     */
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* } */
#endif

#if (1)
/* { */
int
dnx_ofp_rates_egq_tcg_rate_sw_set(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint32 tcg_rate)
{
    int base_q_pair;
    uint32 ps;
    uint32 egq_tcg_qpair_shaper_enable;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_shaper_verify(unit,
                                                        core,
                                                        tm_port,
                                                        tcg_ndx,
                                                        tcg_rate, DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_RATE));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable));
    if (egq_tcg_qpair_shaper_enable)
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Getting ps
         */
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        /*
         * Setting rate
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.valid.set(unit, core, ps, tcg_ndx, TRUE));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.rate.set(unit, core, ps, tcg_ndx, tcg_rate));
    }
exit:
    SHR_FUNC_EXIT;
}
int
dnx_ofp_rates_egq_port_priority_rate_sw_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 prio_ndx,
    uint32 ptc_rate)
{
    int base_q_pair;
    uint32 egq_tcg_qpair_shaper_enable;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_shaper_verify(unit,
                                                                  core,
                                                                  tm_port,
                                                                  prio_ndx,
                                                                  ptc_rate,
                                                                  DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_RATE));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable));
    if (egq_tcg_qpair_shaper_enable)
    {
        /*
         * Getting the fap port's base_q_pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        /*
         * Setting rate
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.rate.set(unit, core, (base_q_pair + prio_ndx) /* q_pair */ ,
                                                       ptc_rate));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.valid.set(unit, core, (base_q_pair + prio_ndx) /* q_pair */ ,
                                                        TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}
int
dnx_ofp_rates_egq_tcg_rate_hw_get(
    int unit,
    int core,
    dnx_fap_port_id_t tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint32 *tcg_rate)
{
    uint32 egq_cal_len, ps, org_tcg_val, tcg_id;
    int base_q_pair;
    dnx_ofp_rates_cal_info_t cal_info;
    dnx_ofp_rates_cal_egq_t egq_cal;
    uint32 egq_tcg_qpair_shaper_enable;
    bcm_port_t logical_port;
    uint32 *egq_rates = NULL;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, FALSE, &org_tcg_val));
    }
    SHR_NULL_CHECK(tcg_rate, _SHR_E_PARAM, "tcg_rate");
    SHR_ALLOC(egq_rates, sizeof(uint32) * DNX_EGR_NOF_Q_PAIRS, "egq_rates", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    dnx_ofp_rates_cal_info_clear(&cal_info);
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable));
    if (egq_tcg_qpair_shaper_enable)
    {
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_TCG;
        egq_cal_len = 0;
        /*
         * Retrieve calendars
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_active_generic_calendars_retrieve_egq(unit,
                                                                            core, &cal_info, &egq_cal, &egq_cal_len));
        /*
         * Calculate egq rates
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_from_generic_calendar_to_ports_egq_rate(unit,
                                                                              core,
                                                                              &cal_info,
                                                                              &egq_cal,
                                                                              egq_cal_len,
                                                                              DNX_EGR_NOF_Q_PAIRS, egq_rates));
        /*
         * Get the rate of the requested port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        tcg_id = DNX_OFP_RATES_TCG_ID_GET(ps, tcg_ndx);
        *tcg_rate = egq_rates[tcg_id];
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, TRUE, &org_tcg_val));
    }
exit:
    SHR_FREE(egq_rates);
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egq_port_priority_rate_hw_get(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t ptc_ndx,
    uint32 *ptc_rate)
{
    uint32 egq_cal_len, *egq_rates = NULL;
    int base_q_pair;
    bcm_port_t logical_port;

    dnx_ofp_rates_cal_egq_t egq_cal;
    dnx_ofp_rates_cal_info_t cal_info;
    uint32 org_tc_val, egq_tcg_qpair_shaper_enable;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ptc_rate, _SHR_E_PARAM, "ptc_rate");
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, FALSE, &org_tc_val));
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable));
    if (egq_tcg_qpair_shaper_enable)
    {
        SHR_ALLOC(egq_rates, sizeof(uint32) * DNX_EGR_NOF_Q_PAIRS, "egq_rates", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        dnx_ofp_rates_cal_info_clear(&cal_info);
        cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY;
        egq_cal_len = 0;
        /*
         * Retrieve calendars
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_active_generic_calendars_retrieve_egq(unit,
                                                                            core, &cal_info, &egq_cal, &egq_cal_len));
        /*
         * Calculate egq rates
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_from_generic_calendar_to_ports_egq_rate(unit,
                                                                              core,
                                                                              &cal_info,
                                                                              &egq_cal,
                                                                              egq_cal_len,
                                                                              DNX_EGR_NOF_Q_PAIRS, egq_rates));
        /*
         * Get the rate of the requested port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        *ptc_rate = egq_rates[base_q_pair + ptc_ndx];
    }

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, TRUE, &org_tc_val));
    }

exit:
    SHR_FREE(egq_rates);
    SHR_FUNC_EXIT;
}

static int
dnx_rates_egq_tcg_rate_hw_set(
    int unit,
    int core)
{
    int base_q_pair, nof_priorities;
    uint32
        egq_calendar_len,
        fap_port_ndx,
        ps,
        recalc,
        tcg_ndx, tcg_id, tcg_offset, act_cal, org_tcg_val, sum_of_tcg_requested, port_i, rate, is_valid, cal_slots;
    dnx_ofp_rates_cal_info_t cal_info;
    dnx_ofp_rates_cal_set_e cal2set;
    bcm_pbmp_t pbmp;
    int core_i;
    uint32 egq_tcg_qpair_shaper_enable;
    uint32 entry_handle_id;
    uint32 dbal_field_val;
    dbal_tables_e dbal_table_id;
    dbal_fields_e dbal_field_id;
    uint32 *egq_rates = NULL;
    dnx_ofp_rates_cal_egq_t *egq_calendar = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    egq_calendar_len = 0;
    recalc = TRUE;
    sum_of_tcg_requested = 0;
    cal_slots = 0;
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, FALSE, &org_tcg_val));
    }
    sal_memset(&cal_info, 0, sizeof(cal_info));
    SHR_ALLOC(egq_rates, sizeof(uint32) * dnx_data_egr_queuing.params.nof_q_pairs_get(unit), "egq_rates", "%s%s%s\r\n",
              EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(egq_calendar, sizeof(dnx_ofp_rates_cal_egq_t), "egq_calendar", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable));
    if (egq_tcg_qpair_shaper_enable)
    {
        sal_memset(egq_rates, 0, dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * sizeof(uint32));
        /*
         * Get active calendar
         */
        {
            /*
             * Allocate buffer.
             */
            dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
            /*
             * key construction.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_field_id = DBAL_FIELD_TCG_SPR_SET_SEL;
            /*
             * Get the value of 'TCG_SPR_SET_SEL'
             */
            dbal_field_val = 0;
            dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &dbal_field_val);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            act_cal = dbal_field_val;
        }
        cal2set = (act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
        BCM_PBMP_ITER(pbmp, port_i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port_i, &core_i, &fap_port_ndx));
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &nof_priorities));
            ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
            tcg_offset = base_q_pair - (ps * DNX_EGR_NOF_Q_PAIRS_IN_PS);
            for (tcg_ndx = 0; tcg_ndx < nof_priorities; ++tcg_ndx)
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.rate.get(unit, core, ps, tcg_ndx + tcg_offset, &rate));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.tcg.valid.get(unit, core, ps, tcg_ndx + tcg_offset, &is_valid));
                if (is_valid)
                {
                    tcg_id = base_q_pair + tcg_ndx;
                    egq_rates[tcg_id] = rate;
                    sum_of_tcg_requested += rate;
                    egq_calendar->slots[cal_slots].base_q_pair = tcg_id;
                    cal_slots++;
                }
            }
        }
        if (cal_slots)
        {
            cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_TCG;
            SHR_IF_ERR_EXIT(dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar(unit,
                                                                                   core,
                                                                                   &cal_info,
                                                                                   egq_rates,
                                                                                   cal_slots,
                                                                                   sum_of_tcg_requested,
                                                                                   DNX_OFP_RATES_CAL_LEN_EGQ_TCG_MAX,
                                                                                   recalc,
                                                                                   FALSE,
                                                                                   egq_calendar, &egq_calendar_len));
            /*
             * Write to device
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_active_egq_generic_calendars_config(unit,
                                                                              core,
                                                                              &cal_info,
                                                                              egq_calendar, egq_calendar_len));
            /*
             * Set active calendar
             */
            /*
             * Set the value of 'TCG_SPR_SET_SEL' to 'cal2set'
             */
            dbal_field_val = cal2set;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tcg_read_enable_set(unit, core, TRUE, &org_tcg_val));
    }
exit:
    SHR_FREE(egq_rates);
    SHR_FREE(egq_calendar);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egq_tcg_rate_hw_set(
    int unit)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_rates_egq_tcg_rate_hw_set(unit, core));
    }
exit:
    SHR_FUNC_EXIT;
}

int
dnx_rates_egq_port_priority_rate_hw_set(
    int unit,
    int core)
{
    uint32
        egq_calendar_len,
        fap_port_ndx,
        ptc_ndx, recalc = TRUE, act_cal, org_qp_val, sum_of_ptc_requested, port_i, rate, is_valid, cal_slots;
    int base_q_pair, nof_priorities;
    dnx_ofp_rates_cal_info_t cal_info;
    dnx_ofp_rates_cal_set_e cal2set;
    bcm_pbmp_t pbmp;
    int core_i;
    uint32 egq_tcg_qpair_shaper_enable;
    uint32 entry_handle_id;
    uint32 dbal_field_val;
    dbal_tables_e dbal_table_id;
    dbal_fields_e dbal_field_id;
    dnx_ofp_rates_cal_egq_t *egq_calendar = NULL;
    uint32 *egq_rates = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    egq_calendar_len = 0;
    sum_of_ptc_requested = 0;
    cal_slots = 0;
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, FALSE, &org_qp_val));
    }
    SOC_CLEAR(&cal_info, dnx_ofp_rates_cal_info_t, 1);
    SHR_ALLOC(egq_rates, sizeof(uint32) * dnx_data_egr_queuing.params.nof_q_pairs_get(unit), "egq_rates", "%s%s%s\r\n",
              EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(egq_calendar, sizeof(dnx_ofp_rates_cal_egq_t), "egq_calendar", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    /*
     * Get active calendar
     */
    {
        /*
         * Allocate buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_field_id = DBAL_FIELD_QPAIR_SPR_SET_SEL;
        /*
         * Get the value of 'QPAIR_SPR_SET_SEL'
         */
        dbal_field_val = 0;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &dbal_field_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        act_cal = dbal_field_val;
    }
    cal2set = (act_cal == DNX_OFP_RATES_CAL_SET_A) ? DNX_OFP_RATES_CAL_SET_B : DNX_OFP_RATES_CAL_SET_A;
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable));
    if (egq_tcg_qpair_shaper_enable)
    {
        sal_memset(egq_rates, 0, dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * sizeof(uint32));
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
        BCM_PBMP_ITER(pbmp, port_i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port_i, &core_i, &fap_port_ndx));
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &nof_priorities));
            for (ptc_ndx = 0; ptc_ndx < nof_priorities; ++ptc_ndx)
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.rate.get(unit, core, (base_q_pair + ptc_ndx), &rate));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.qpair.valid.get(unit, core, (base_q_pair + ptc_ndx), &is_valid));
                if (is_valid)
                {
                    egq_rates[base_q_pair + ptc_ndx] = rate;
                    sum_of_ptc_requested += rate;
                    egq_calendar->slots[cal_slots].base_q_pair = base_q_pair + ptc_ndx;
                    cal_slots++;
                }
            }
        }
        if (cal_slots)
        {
            cal_info.cal_type = DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY;
            SHR_IF_ERR_EXIT(dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar(unit,
                                                                                   core,
                                                                                   &cal_info,
                                                                                   egq_rates,
                                                                                   cal_slots,
                                                                                   sum_of_ptc_requested,
                                                                                   DNX_OFP_RATES_CAL_LEN_EGQ_PORT_PRIO_MAX,
                                                                                   recalc,
                                                                                   FALSE,
                                                                                   egq_calendar, &egq_calendar_len));
            /*
             * Write to device
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_active_egq_generic_calendars_config(unit,
                                                                              core,
                                                                              &cal_info,
                                                                              egq_calendar, egq_calendar_len));
            /*
             * Set active calendar
             */
            /*
             * Set the value of 'QPAIR_SPR_SET_SEL' to 'cal2set'
             */
            dbal_field_val = cal2set;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_conditional_crdt_table_access))
    {
        /*
         * added only because shaper configuration under traffic causing losts
         */
        SHR_IF_ERR_EXIT(dnx_ofp_rates_tc_read_enable_set(unit, core, TRUE, &org_qp_val));
    }
exit:
    SHR_FREE(egq_rates);
    SHR_FREE(egq_calendar);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofp_rates_egq_port_priority_rate_hw_set(
    int unit)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_rates_egq_port_priority_rate_hw_set(unit, core));
    }

exit:
    SHR_FUNC_EXIT;
}

/* } */
#endif
