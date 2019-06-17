/** \file dnxcmn.c
 *  
 *  Common utils for dnx only.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

/*
 * Include files.
 * {
 */

#include <bcm/types.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/drv.h> 
#include <soc/dnx/dbal/dbal.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>

/*
 * }
 */

/*
 * Time
 * {
 */

/*
 * See .h file
 */
shr_error_e
dnxcmn_time_units_convert(
    int unit,
    dnxcmn_time_t * time_src,
    dnxcmn_time_unit_e required_time_units,
    dnxcmn_time_t * time_dest)
{
    /*
     * sec milisec usec nsec
     */
    int time_units_power[] = { 0, 3, 6, 9 };
    uint32 abs_power_delta;
    uint64 multiplier;
    int power_delta;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that time units included in time_units_power
     */
    if (time_src->time_units < 0 || (time_src->time_units >= sizeof(time_units_power) / sizeof(int)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported time units %d", time_src->time_units);
    }
    if (required_time_units < 0 || (required_time_units >= sizeof(time_units_power) / sizeof(int)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported time units %d", required_time_units);
    }

    /*
     * Get the required multiplier
     */
    power_delta = time_units_power[required_time_units] - time_units_power[time_src->time_units];
    abs_power_delta = utilex_abs(power_delta);

    COMPILER_64_UPOW(multiplier, 10, abs_power_delta);
    /*
     * Convert
     */
    if (power_delta >= 0)
    {
        COMPILER_64_COPY(time_dest->time, time_src->time);
        COMPILER_64_UMUL_32(time_dest->time, multiplier);
    }
    else
    {
        COMPILER_64_COPY(time_dest->time, time_src->time);
        COMPILER_64_UDIV_32(time_dest->time, multiplier);
    }
    time_dest->time_units = required_time_units;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxcmn_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxcmn_time_unit_e required_time_unit,
    dnxcmn_time_t * time)
{
    uint32 core_clock_khz;
    dnxcmn_time_t time_nano;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get core clock freq
     */
    core_clock_khz = dnx_data_device.general.core_clock_khz_get(unit);

    /*
     * First convert clock cycles to nano sec 
     * time [NSEC] = 1000000000 * ((nof_clock_cycles) / (core_clock_khz * 1000)) / 
     */
    time_nano.time_units = DNXCMN_TIME_UNIT_NSEC;
    COMPILER_64_COPY(time_nano.time, nof_clock_cycles);
    COMPILER_64_UMUL_32(time_nano.time, 1000000);
    COMPILER_64_UDIV_32(time_nano.time, core_clock_khz);

    /*
     * Convert to time according to requested units
     */
    SHR_IF_ERR_EXIT(dnxcmn_time_units_convert(unit, &time_nano, required_time_unit, time));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxcmn_time_to_clock_cycles_get(
    int unit,
    dnxcmn_time_t * time,
    uint32 *nof_clock_cycles)
{
    uint32 core_clock_khz;
    uint64 long_nof_clock_cycles;

    dnxcmn_time_t time_nano;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get core clock freq
     */
    core_clock_khz = dnx_data_device.general.core_clock_khz_get(unit);

    /*
     * Convert time to nano sec
     */
    SHR_IF_ERR_EXIT(dnxcmn_time_units_convert(unit, time, DNXCMN_TIME_UNIT_NSEC, &time_nano));

    /*
     * Get nof of clock cycles 
     * nof_clock_cycles = (core_clock [KHZ] * 1000) * (time [NSEC] / 1000000000)
     */
    COMPILER_64_COPY(long_nof_clock_cycles, time_nano.time);
    COMPILER_64_UMUL_32(long_nof_clock_cycles, core_clock_khz);
    COMPILER_64_UDIV_32(long_nof_clock_cycles, 1000000);

    /*
     * Extract nof long cycles
     */
    COMPILER_64_TO_32_LO(*nof_clock_cycles, long_nof_clock_cycles);
    if (COMPILER_64_HI(long_nof_clock_cycles) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Over flow when converting time to clock cycles");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxcmn_polling(
    int unit,
    sal_usecs_t time_out,
    int32 min_polls,
    uint32 entry_handle_id,
    dbal_fields_e field,
    uint32 expected_value)
{
    soc_timeout_t to;
    uint32 result = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef JR2_CRASH_RECOVERY_SUPPORT
    /*
     * Implement crash recovery support here
     */
#endif /* JR2_CRASH_RECOVERY_SUPPORT */

    /*
     * Get field value
     */
    dbal_value_field32_request(unit, entry_handle_id, field, INST_SINGLE, &result);

    soc_timeout_init(&to, time_out, min_polls);
    for (;;)
    {
        /*
         * Get entry
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

#ifdef PLISIM
        if (SAL_BOOT_PLISIM)
        {
            result = expected_value;
        }
#endif
        if (result == expected_value)
        {
            break;
        }
        if (soc_timeout_check(&to))
        {
            LOG_ERROR(BSL_LS_SOC_REG,
                      (BSL_META("TIMEOUT when polling field: %s \n"), dbal_field_to_string(unit, field)));
            SHR_ERR_EXIT(_SHR_E_TIMEOUT, "_SHR_E_TIMEOUT");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
