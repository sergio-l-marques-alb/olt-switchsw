/** \file dnxcmn.h
 *  
 *  Common Utils for dnx only
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DNXCMN_H_INCLUDED
/*
 * {
 */
#define DNXCMN_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Includes 
 * { 
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
/*
 * }
 */

/*
 * MACROs
 * {  
 */
/** 
 * \brief - iterate over the relevant cores 
 * if core_id is BCM_CORE_ALL will iterate over all supported cores. 
 * Otherwise the iteration will be over the given core. 
 */
#define DNXCMN_CORES_ITER(unit, core_id, core_index) \
    for(core_index = ((core_id == BCM_CORE_ALL) ? 0 : core_id);\
        core_index < ((core_id == BCM_CORE_ALL) ?  dnx_data_device.general.nof_cores_get(unit) : (core_id + 1));\
        core_index++)

/** 
 * \brief - validate core is legal 
 *  if allow_all == true - validate that core is either valid core_id or BCM_CORE_ALL
 *  Otherwise - validate that core is valid core_id
 */
#define DNXCMN_CORE_VALIDATE(unit, core, allow_all) \
        if ((core >= dnx_data_device.general.nof_cores_get(unit) || (core < 0)) && (!(core == _SHR_CORE_ALL && allow_all)))\
        {\
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Core %d", core);\
        }

/**
 * \brief - indicates whether chip has more than one core or not.
 */
#define DNXCMN_CHIP_IS_MULTI_CORE(unit) \
        (dnx_data_device.general.nof_cores_get(unit) > 1)

/**
 * Get core clock frequency (supported units are Hz, KHz and MHz)
 */
#define DNXCMN_CORE_CLOCK_HZ_GET(unit) (dnx_data_device.general.core_clock_khz_get(unit) * 1000)
#define DNXCMN_CORE_CLOCK_KHZ_GET(unit) (dnx_data_device.general.core_clock_khz_get(unit))
#define DNXCMN_CORE_CLOCK_MHZ_GET(unit) (dnx_data_device.general.core_clock_khz_get(unit) / 1000)
/*
 * }
 */

/*
 * TYPEDEFs
 * {  
 */

#define DNXCMN_TIMEOUT                            (20*1000)
#define DNXCMN_MIN_POLLS                          (100)

/*
 * }
 */

/*
 * TYPEDEFs
 * {  
 */
/**
 * \brief - time units
 */
typedef enum
{
  /**
   * Invalid time unit
   */
    DNXCMN_TIME_UNIT_INVALID = -1,
  /**
   * Second
   */
    DNXCMN_TIME_UNIT_SEC = 0,
  /**
   * Mili Second
   */
    DNXCMN_TIME_UNIT_MILISEC = 1,
  /**
   * Micro Second
   */
    DNXCMN_TIME_UNIT_USEC = 2,

  /**
   * Nano Second
   */
    DNXCMN_TIME_UNIT_NSEC = 3
} dnxcmn_time_unit_e;

/**
 * Represet time which composed from amount of time and time units
 */
typedef struct
{
    dnxcmn_time_unit_e time_units;
    uint64 time;
} dnxcmn_time_t;
/*
 * }
 */

/*
 * TIME Functions
 * {  
 */
/**
 * \brief - Convert time to required time_units
 *          The result will be round off 
 *          
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] time_src - time to convert
 *   \param [in] required_time_units - required time units
 *   \param [in] time_dest - destination of the converted tiem
 * \par INDIRECT INPUT:
 *   * None
 */
shr_error_e dnxcmn_time_units_convert(
    int unit,
    dnxcmn_time_t * time_src,
    dnxcmn_time_unit_e required_time_units,
    dnxcmn_time_t * time_dest);

/**
 * \brief - Convert device clock cycles to time (according to required time units) 
 *          The result will be round down 
 *          
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] nof_clock_cycles - number of clock cycles to convert to time
 *   \param [in] required_time_unit - required time units
 *   \param [in] time - pointer to converted time (output)
 * \par INDIRECT INPUT:
 *   * dnx_data_device (to get the core clock)
 */
shr_error_e dnxcmn_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxcmn_time_unit_e required_time_unit,
    dnxcmn_time_t * time);

/**
 * \brief - Convert time to device clock cycles (according to specified time units) 
 *          
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] time - time to convert to clock cycles
 *   \param [in] nof_clock_cycles - pointer to converted number of clock cycles (output)
 * \par INDIRECT INPUT:
 *   * dnx_data_device (to get the core clock)
 */
shr_error_e dnxcmn_time_to_clock_cycles_get(
    int unit,
    dnxcmn_time_t * time,
    uint32 *nof_clock_cycles);

/**
 * \brief - Doing polling till the result matches the expected, 
 *        or we reach the time out
 *          
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] time_out - time out value, maximal time for
 *          polling
 *   \param [in] min_polls - minimum polls
 *   \param [in] entry_handle_id - DBAL handle to the table, need to 
 *          set Key field before passsing the handle
 *   \param [in] field - DBAL field name
 *   \param [in] expected_value - the expected value
 */
shr_error_e dnxcmn_polling(
    int unit,
    sal_usecs_t time_out,
    int32 min_polls,
    uint32 entry_handle_id,
    dbal_fields_e field,
    uint32 expected_value);

/*
 * }
 */
#endif /* !DNXCMN_H_INCLUDED */
