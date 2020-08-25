/** \file iqs_tune.c
 *  Tuning for ingress queue scheduler.
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/tune/iqs_tune.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx_dispatch.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#include <soc/dnx/legacy/TMC/tmc_api_fabric.h>

/*
 * }
 * Include files.
 */

/*
 * See .h
 */
shr_error_e
dnx_tune_iqs_delay_tolerance_preset_get(
    int unit,
    bcm_cosq_delay_tolerance_preset_attr_t * preset_attr,
    bcm_cosq_delay_tolerance_t * delay_tolerance)
{
    int credit_resolution, bw_level;
    int slow_enable, max_inflight_credits, max_inflight_bytes, max_inflight_bytes_rtt;
    int fabric_delay;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(delay_tolerance, 0, sizeof(bcm_cosq_delay_tolerance_t));

    /** high priority */
    if (preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_HIGH_PRIORITY)
    {
        delay_tolerance->flags |= BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY;
    }

    /** OCB only */
    if (preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_OCB_ONLY)
    {
        delay_tolerance->flags |= BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY;
    }

    /** slow enable */
    slow_enable = (preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_SLOW_ENABLED) ? 1 : 0;

    /** Low latency */
    if (!slow_enable)
    {
        delay_tolerance->flags |= BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY;
    }

    /*
     * Divide the rate to bandwidth level. and calculate the required credit resolution for each rate
     */
    SHR_IF_ERR_EXIT(dnx_iqs_rate_to_bw_level_get(unit, preset_attr->rate, &bw_level));
    SHR_IF_ERR_EXIT(dnx_iqs_bw_level_to_credit_resolution_get(unit, bw_level, &credit_resolution));

    /** bandwidth level */
    delay_tolerance->bandwidth_level = bw_level;

    /** delayed size */
    if (preset_attr->flags & BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_MULTICAST)
    {
        fabric_delay = dnx_data_iqs.credit.fabric_multicast_delay_get(unit);
    }
    else
    {
        fabric_delay = dnx_data_iqs.credit.fabric_delay_get(unit);
    }

    /** maximal amount of in-flight credits (for a single direction) */
    max_inflight_credits = UTILEX_DIV_ROUND_UP(preset_attr->rate * fabric_delay, preset_attr->credit_size * 8);

    /** maximal amount of in-flight bytes (for a single direction) */
    max_inflight_bytes = max_inflight_credits * preset_attr->credit_size;

    /** maximal amount of in-flights bytes in round-trip for both direction */
    max_inflight_bytes_rtt = 2 * max_inflight_bytes;

    /** satisfied thresholds */
    delay_tolerance->credit_request_satisfied_backoff_enter_thresh = (128 - 8) * credit_resolution * 1024;    /** bytes */
    delay_tolerance->credit_request_satisfied_backoff_exit_thresh = (128 - 8) * credit_resolution * 1024;     /** bytes */
    delay_tolerance->credit_request_satisfied_backlog_enter_thresh = (128 - 8) * credit_resolution * 1024;    /** bytes */
    delay_tolerance->credit_request_satisfied_backlog_exit_thresh = (128 - 8) * credit_resolution * 1024;     /** bytes */

    delay_tolerance->credit_request_satisfied_backslow_enter_thresh = delay_tolerance->credit_request_satisfied_backoff_enter_thresh * 75 / 100;     /** bytes 75% */
    delay_tolerance->credit_request_satisfied_backslow_exit_thresh = delay_tolerance->credit_request_satisfied_backoff_enter_thresh * 75 / 100;      /** bytes 75% */

    delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh = slow_enable ? preset_attr->credit_size : 2 * (max_inflight_bytes - preset_attr->credit_size);   /** bytes */
    delay_tolerance->credit_request_satisfied_empty_queue_thresh = slow_enable ? 0 : delay_tolerance->credit_request_satisfied_empty_queue_max_balance_thresh / 2;  /** bytes */
    delay_tolerance->credit_request_satisfied_empty_queue_exceed_thresh = 1;

    /** hungry thresholds - all in bytes*/
    delay_tolerance->credit_request_hungry_off_to_slow_thresh = slow_enable ? 0 /** enable slow on the first packet */ : -1 * max_inflight_bytes_rtt;      /** bytes */

    delay_tolerance->credit_request_hungry_off_to_slow_thresh =
        UTILEX_MIN(delay_tolerance->credit_request_hungry_off_to_slow_thresh, dnx_data_iqs.credit.hungry_max_get(unit));

    delay_tolerance->credit_request_hungry_off_to_normal_thresh = slow_enable ? max_inflight_bytes_rtt : (-1 * max_inflight_bytes_rtt);       /** bytes */

    delay_tolerance->credit_request_hungry_off_to_normal_thresh =
        UTILEX_MIN(delay_tolerance->credit_request_hungry_off_to_normal_thresh,
                   dnx_data_iqs.credit.hungry_max_get(unit));

    delay_tolerance->credit_request_hungry_slow_to_normal_thresh = slow_enable ? max_inflight_bytes_rtt : (-1 * 3 * max_inflight_bytes);      /** bytes */

    delay_tolerance->credit_request_hungry_slow_to_normal_thresh =
        UTILEX_MIN(delay_tolerance->credit_request_hungry_slow_to_normal_thresh,
                   dnx_data_iqs.credit.hungry_max_get(unit));

    delay_tolerance->credit_request_hungry_normal_to_slow_thresh = slow_enable ? max_inflight_bytes_rtt : (-1 * 3 * max_inflight_bytes);      /** bytes */

    delay_tolerance->credit_request_hungry_normal_to_slow_thresh =
        UTILEX_MIN(delay_tolerance->credit_request_hungry_normal_to_slow_thresh,
                   dnx_data_iqs.credit.hungry_max_get(unit));

    delay_tolerance->credit_request_hungry_multiplier = 2048;   /** bytes */

    /** slow levels - all in bytes*/
    if (slow_enable)
    {
        int default_slow_rates[BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS];      /** bytes */
        int slow_level_index, min_slow_rate, factor, slow_level_th;
        int off_to_norm_th;

        min_slow_rate = preset_attr->rate / 10;
        factor = 155;

        for (slow_level_index = 0; slow_level_index < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; slow_level_index++)
        {
            if (slow_level_index == 0)
            {
                default_slow_rates[slow_level_index] =
                    UTILEX_DIV_ROUND_UP(preset_attr->credit_size * 8, 2 * fabric_delay);
            }
            else
            {
                if ((default_slow_rates[slow_level_index - 1] * factor) / 100 > preset_attr->rate)
                {
                    default_slow_rates[slow_level_index] =
                        UTILEX_MAX(default_slow_rates[slow_level_index - 1], preset_attr->rate);
                }
                else
                {
                    default_slow_rates[slow_level_index] = (default_slow_rates[slow_level_index - 1] * factor) / 100;
                }

            }
        }

        off_to_norm_th = delay_tolerance->credit_request_hungry_off_to_normal_thresh;

        for (slow_level_index = 0; slow_level_index < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; slow_level_index++)
        {
            slow_level_th = 0;

            if (min_slow_rate < default_slow_rates[slow_level_index])
            {
                if (slow_level_index > 1 && (off_to_norm_th < 0))
                {
                    slow_level_th = 0;
                }
                else
                {
                    slow_level_th = off_to_norm_th * (slow_level_index + 1) / 8;
                }
            }

            /** same threshold for up and down */
            delay_tolerance->slow_level_thresh_down[slow_level_index] = slow_level_th;
            delay_tolerance->slow_level_thresh_up[slow_level_index] = slow_level_th;
        }
    }

    /** credit watchdog delete threshold */
    delay_tolerance->credit_request_watchdog_delete_queue_thresh = 500; /* mili seconds */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Tune credit watchdog
 * Configure range of queues to be all queues and set credit retransmit threshold.
 * Delete threshold configured per credit request profile in dnx_tune_iqs_delay_tolerance_preset_get()
 */
static shr_error_e
dnx_tune_iqs_credit_watchdog_init(
    int unit)
{
    int min_queue, max_queue, retransmit_threshold, enable;
    bcm_cosq_delay_tolerance_t delay_tolerance;
    bcm_cosq_range_t range;
    SHR_FUNC_INIT_VARS(unit);

    min_queue = 0;
    max_queue = dnx_data_ipq.queues.nof_queues_get(unit) - 1;   /* max queue */
    enable = 1;
    retransmit_threshold = 500; /* micro seconds */

    /** Set retransmit threshold */
    sal_memset(&delay_tolerance, 0, sizeof(bcm_cosq_delay_tolerance_t));
    delay_tolerance.credit_request_watchdog_status_msg_gen = retransmit_threshold;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_delay_tolerance_level_set
                    (unit, BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG, &delay_tolerance));

    /** Set queue range and enable */
    sal_memset(&range, 0, sizeof(bcm_cosq_range_t));
    range.range_start = min_queue;
    range.range_end = max_queue;
    range.is_enabled = enable;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_range_set(unit, 0, 0, bcmCosqWatchdogQueue, &range));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_tune_iqs_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Enable and config thresholds for credit watchdog */
    SHR_IF_ERR_EXIT(dnx_tune_iqs_credit_watchdog_init(unit));

exit:
    SHR_FUNC_EXIT;
}
