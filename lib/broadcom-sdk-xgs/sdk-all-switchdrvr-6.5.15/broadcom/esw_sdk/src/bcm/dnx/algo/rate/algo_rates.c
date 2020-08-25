/** \file algo/rate/algo_rates.c
 *
 * Implementation for shapers rate calculations.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/**
* Include files:
* {
*/

#include <sal/compiler.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/rate/algo_rates.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

/** 
 * } 
 */

/**
* Defines:
* {
*/

/* 
 * The first value of 'credit' when searching for a matching 'credit' 
 * and 'interval' for 'rate'.
 */
#define DNX_ALGO_RATES_CREDIT_VAL_FIRST   1024

/** 
 * } 
 */

/**
* Functions:
* {
*/

/**
 * \brief
 *   Given credit size in Bytes, convert a shaper's rate given in Kbits/sec units
 *   to an interval between 2 consecutive credits in device clocks.
 *   The conversion is done according to:
 *                                                  Credit [Bytes] * Num_of_clocks_in_sec [Kclocks/sec]
 *   Interval_between_credits_in_clocks [clocks] = ----------------------------------------------------
 *                                                  Rate [KBytes/Sec]
 * 
 *  \param [in] unit -
 *    The unit id.
 *  \param [in] credit -
 *    The credit size [Bytes].
 *  \param [in] rate -
 *      The shaper's rate [KBytes/Sec].
 *  \param [out] interval -
 *      Interval between credits [clocks].
 *  \see
 *    * dnx_algo_rates_credit_interval_param_get
 */
static shr_error_e
dnx_algo_rates_credit_rate_to_interval_get(
    int unit,
    uint32 credit,
    uint32 rate,
    uint32 *interval)
{
    uint64 result;
    SHR_FUNC_INIT_VARS(unit);

    if (0 == rate)
    {
        /** Divide by zero */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "rate can't be 0.");
    }

    /** The calculation */
    COMPILER_64_SET(result, 0, credit);
    COMPILER_64_UMUL_32(result, dnx_data_device.general.core_clock_khz_get(unit));
    COMPILER_64_UDIV_32(result, rate);

    if (COMPILER_64_HI(result) != 0)
    {
        /** Overflow */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Computation overflow.");
    }

    *interval = COMPILER_64_LO(result);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given credit size in Bytes, convert an interval between 2 consecutive
 *   credits in device clocks to a shaper's rate given in Kbits/sec units.
 *   The conversion is done according to:
 *                        Credit [Bytes] * Num_of_clocks_in_sec [Kclocks/sec]
 *   Rate [KBytes/Sec] = -----------------------------------------------------
 *                        Interval_between_credits_in_clocks [clocks]
 * 
 *  \param [in] unit -
 *    The unit id.
 *  \param [in] credit -
 *    The credit size [Bytes].
 *  \param [in] interval -
 *      Interval between credits [clocks].
 *  \param [out] rate -
 *      The shaper's rate [KBytes/Sec].
 *  \see
 *    * dnx_algo_rates_credit_interval_param_get
 */
static shr_error_e
dnx_algo_rates_credit_interval_to_rate_get(
    int unit,
    uint32 credit,
    uint32 interval,
    uint32 *rate)
{
    uint64 result;
    SHR_FUNC_INIT_VARS(unit);

    if (0 == interval)
    {
        /** Divide by zero */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "interval can't be 0.");
    }

    /** The calculation */
    COMPILER_64_SET(result, 0, credit);
    COMPILER_64_UMUL_32(result, dnx_data_device.general.core_clock_khz_get(unit));
    COMPILER_64_UDIV_32(result, interval);

    if (COMPILER_64_HI(result) != 0)
    {
        /** Overflow */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Computation overflow.");
    }

    *rate = COMPILER_64_LO(result);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert a rate given in Kbits/sec units to an interval
 * between 2 consecutive credits in device clocks.
 *
 * The conversion is done according to:
 *                       Credit [Kbits] * Num_of_clocks_in_sec [clocks/sec]
 *  Rate [Kbits/Sec] =   -------------------------------------------------------------
 *                          interval_between_credits_in_clocks [clocks]
 *  \param [in] unit -
 *    The unit id.
 *  \param [in] rate -
 *      The rate [KBits/Sec].
 *  \param [out] interval -
 *      Interval between credits [clocks].
 *  \param [in] credit -
 *    The credit size [Bytes].
 *  \param [out] interval -
 *      Interval between credits [clocks].
 *
 * \note
 * Due to rounding issues we don't actually divide the credit
 * size by 1000 to get size in KBits. Instead we divide
 * the ticks parameter (which is much bigger) by 1000.
 *
 * \see - None
 */
shr_error_e
dnx_algo_rates_kbits_per_sec_to_clocks(
    int unit,
    uint32 rate,
    uint32 credit,
    uint32 *interval)
{
    SHR_FUNC_INIT_VARS(unit);

    /** internal function gets rate in KBytes/sec */
    SHR_IF_ERR_EXIT(dnx_algo_rates_credit_rate_to_interval_get(unit, credit, rate / UTILEX_NOF_BITS_IN_CHAR, interval));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Convert an interval between 2 consecutive credits given in
 * device clocks to rate in Kbits/sec units.
 *
 * The conversion is done according to:
 *                       Credit [Kbits] * Num_of_clocks_in_sec [clocks/sec]
 *  Rate [Kbits/Sec] =   -------------------------------------------------------------
 *                          interval_between_credits_in_clocks [clocks]
 *  \param [in] unit -
 *    The unit id.
 *  \param [in] interval -
 *      Interval between credits [clocks].
 *  \param [in] credit -
 *    The credit size [Bytes].
 *  \param [out] rate -
 *      The rate [KBits/Sec].
 * \note
 * For rounding issues we don't actually divide the credit
 * size by 1000 to get size in KBits. Instead we divide
 * the ticks parameter (which is much bigger) by 1000.
 * \see - None
 */
shr_error_e
dnx_algo_rates_clocks_to_kbits_per_sec(
    int unit,
    uint32 interval,
    uint32 credit,
    uint32 *rate)
{
    uint32 kbytes_per_sec;

    SHR_FUNC_INIT_VARS(unit);

    /** internal function returns rate in KBytes/sec */
    SHR_IF_ERR_EXIT(dnx_algo_rates_credit_interval_to_rate_get(unit, credit, interval, &kbytes_per_sec));
    *rate = kbytes_per_sec * UTILEX_NOF_BITS_IN_CHAR;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Calculate a more exact credit value.
 *
 *   The first 'credit' and 'interval' that were founded may result to a
 *   different rate than desired due to rounding errors.
 *   In this function we recalculate the credit value (assuming that
 *   'interval' is fixed) in order to get closer to the desired rate.
 *
 *   The calculation is done as follows:
 *     final_credit_value = current_credit_value * desired_rate / current_rate;
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] current_credit_value -
 *   The credit value retrieved after converting 'rate' to 'credit' and
 *   'interval'.
 * \param [in] desired_rate -
 *   The desired final rate.
 * \param [in] current_rate -
 *   The rate value retrieved using 'current_credit_value' and 'interval'.
 * \param [out] final_credit_value -
 *   A more accurate credit value that will result in a more accurate rate.
 */
static void
dnx_algo_rates_final_credit_value_get(
    int unit,
    uint32 current_credit_value,
    uint32 desired_rate,
    uint32 current_rate,
    uint32 *final_credit_value)
{
    uint64 result;
    uint64 tmp_result;
    uint64 round_helper;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(result);
    COMPILER_64_ZERO(tmp_result);
    COMPILER_64_ZERO(round_helper);

    /** current_credit_value * desired_rate */
    COMPILER_64_SET(tmp_result, 0, current_credit_value);
    COMPILER_64_UMUL_32(tmp_result, desired_rate);

    /** final_credit_value = current_credit_value * desired_rate / current_rate */
    COMPILER_64_COPY(result, tmp_result);
    COMPILER_64_UDIV_32(result, current_rate);
    *final_credit_value = COMPILER_64_LO(result);

    /*
     * In case the remainder of 'final_credit_value' was truncated by the
     * devision with 'current_rate', we want to round it up.
     *
     * round_helper = final_credit_value * current_rate
     */
    COMPILER_64_SET(round_helper, 0, *final_credit_value);
    COMPILER_64_UMUL_32(round_helper, current_rate);
    if (COMPILER_64_GT(tmp_result, round_helper))
    {
        *final_credit_value += 1;
    }

    if (*final_credit_value == 0)
    {
        *final_credit_value = 1;
    }

    SHR_VOID_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_algo_rates_rate_to_credit_interval(
    int unit,
    uint32 shaper_rate,
    dnx_algo_rates_shaper_bounds_t * shaper_bounds,
    int interval_resolution,
    uint32 *shaper_interval,
    uint32 *shaper_credit)
{
    uint32 interval_value = 0;
    uint32 interval_remainder = 0;
    uint32 credit_value = 0;
    uint32 final_credit_value = 0;
    uint32 current_rate = 0;
    int is_cal_and_delay_in_range = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    if (interval_resolution < 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Interval resolution must be at least 1 clock");
    }

    credit_value = DNX_ALGO_RATES_CREDIT_VAL_FIRST;

    /*
     * Skip calculations when shaper_rate is 0.
     */
    if (0 == shaper_rate)
    {
        *shaper_credit = shaper_bounds->credit_min;
        *shaper_interval = shaper_bounds->interval_max;
        SHR_EXIT();
    }

    /*
     * Find matching calendar and delay that will result in the
     * required shaper's shaper_rate.
     *
     * Loop:
     *   Pick a 'credit_value' and find its matching 'interval_value'.
     *   If 'interval_value' is out of bounds than increase/decrease
     *   'credit_value' accordingly.
     */
    while ((credit_value > 1) && (credit_value < shaper_bounds->credit_max) && !(is_cal_and_delay_in_range))
    {
        SHR_IF_ERR_EXIT(dnx_algo_rates_credit_rate_to_interval_get(unit, credit_value, shaper_rate, &interval_value));

        if (interval_value > (shaper_bounds->interval_max * interval_resolution))
        {
            credit_value /= 2;
        }
        else if (interval_value < (shaper_bounds->interval_min * interval_resolution))
        {
            credit_value *= 2;
            if (credit_value > shaper_bounds->credit_max)
            {
                credit_value = shaper_bounds->credit_max;
            }
        }
        else
        {
            is_cal_and_delay_in_range = TRUE;
        }
    }

    /*
     * Get interval_value again for the cases where credit_value is out of range.
     */
    SHR_IF_ERR_EXIT(dnx_algo_rates_credit_rate_to_interval_get(unit, credit_value, shaper_rate, &interval_value));
    if (interval_value > (shaper_bounds->interval_max * interval_resolution))
    {
        interval_value = (shaper_bounds->interval_max * interval_resolution);
    }

    /*
     * Round interval_value to be a multiple of interval_resolution.
     */
    interval_remainder = interval_value % interval_resolution;
    if (interval_remainder != 0)
    {
        interval_value += interval_resolution - interval_remainder;
    }

    /*
     * Get current shaper_rate according to the calculated 'credit_value' and 'interval_value'.
     */
    SHR_IF_ERR_EXIT(dnx_algo_rates_credit_interval_to_rate_get(unit, credit_value, interval_value, &current_rate));

    /*
     * Due to rounding errors, 'current_rate' might be different from 'shaper_rate',
     * so 'credit_value' should be tuned again so it will be closer to 'shaper_rate'.
     */
    dnx_algo_rates_final_credit_value_get(unit, credit_value, shaper_rate, current_rate, &final_credit_value);

    if (final_credit_value > shaper_bounds->credit_max)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Exact calendar is larger than maximum value.");
    }

    *shaper_interval = interval_value / interval_resolution;
    *shaper_credit = final_credit_value;

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_algo_rates_credit_interval_to_rate(
    int unit,
    int interval_resolution,
    uint32 shaper_interval,
    uint32 shaper_credit,
    uint32 *shaper_rate)
{
    SHR_FUNC_INIT_VARS(unit);

    if (interval_resolution < 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Interval resolution must be at least 1 clock");
    }

    SHR_IF_ERR_EXIT(dnx_algo_rates_credit_interval_to_rate_get
                    (unit, shaper_credit, shaper_interval * interval_resolution, shaper_rate));

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
