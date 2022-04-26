/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _SOC_DNXC_TIME_H_
#define _SOC_DNXC_TIME_H_

#include <shared/shrextend/shrextend_debug.h>

/**
 * \brief - time units
 */
typedef enum
{
  /**
   * Invalid time unit
   */
    DNXC_TIME_UNIT_INVALID = -1,
  /**
   * Second
   */
    DNXC_TIME_UNIT_SEC = 0,
  /**
   * Mili Second
   */
    DNXC_TIME_UNIT_MILISEC = 1,
  /**
   * Micro Second
   */
    DNXC_TIME_UNIT_USEC = 2,

  /**
   * Nano Second
   */
    DNXC_TIME_UNIT_NSEC = 3
} dnxc_time_unit_e;

/**
 * Represent time which composed from amount of time and time units
 */
typedef struct dnxc_time_e
{
    dnxc_time_unit_e time_units;
    uint64 time;
} dnxc_time_t;

/**
 * \brief
 *   Convert from a representation of a time value
 *   from one unit type to another..
 */
shr_error_e dnxc_time_units_convert(
    int unit,
    dnxc_time_t * time_src,
    dnxc_time_unit_e required_time_units,
    dnxc_time_t * time_dest);

/**
 * \brief
 *   Convert clock cycles to time (according to specified time units
 *   and specific clock frequency).
 */
shr_error_e dnxc_time_clock_cycles_to_time_common_get(
    int unit,
    uint32 nof_clock_cycles,
    uint32 clock_freq,
    dnxc_time_unit_e required_time_unit,
    dnxc_time_t * time);

/**
 * \brief
 *   Convert time to clock cycles (according to specified time units
 *   and specific clock frequency).
 */
shr_error_e dnxc_time_time_to_clock_cycles_common_get(
    int unit,
    dnxc_time_t * time,
    uint32 clock_freq,
    uint32 *nof_clock_cycles);

/**
 * \brief
 *   Convert time to clock cycles (according to specified time units
 *   and specific clock frequency).
 */
shr_error_e dnxc_time_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxc_time_unit_e required_time_unit,
    dnxc_time_t * time);

/**
 * \brief - Convert system reference clock cycles to time (according to required time units)
 *          The result will be round down
 */
shr_error_e dnxc_time_system_ref_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxc_time_unit_e required_time_unit,
    dnxc_time_t * time);

/**
 * \brief - Convert time to device clock cycles (according to specified time units)
 */
shr_error_e dnxc_time_time_to_clock_cycles_get(
    int unit,
    dnxc_time_t * time,
    uint32 *nof_clock_cycles);

/**
 * \brief - Convert time to system reference clock cycles (according to specified time units)
 */
shr_error_e dnxc_time_time_to_system_ref_clock_cycles_get(
    int unit,
    dnxc_time_t * time,
    uint32 *nof_clock_cycles);

#include <soc/defs.h>
#endif /* _SOC_DNXC_TIME_H_ */
