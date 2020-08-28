/** \file init_mem.h
 * DNX init memories. 
 * - Set memories to default values 
 * - Get the default value defined per each memory 
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCMINT_DNX_INIT_INIT_MEM_H_INCLUDED
/*
 * { 
 */
#define _BCMINT_DNX_INIT_INIT_MEM_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/control.h>

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif
#include <soc/mcm/allenum.h>

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/* 
 * Typedefs 
 * { 
 */
 /**
  * \brief - each valid writable memory default value will be set according to mode
  */
typedef enum
{
    /**
     * Set default value to zero
     */
    dnx_init_mem_default_mode_zero = 0,
    /**
     * Do not set default value.  
     * Might be used if the table will be set in module init. 
     */
    dnx_init_mem_default_mode_none = 1,
    /**
     * Set default value to all ones (0xFFFF...)  
     */
    dnx_init_mem_default_mode_all_ones = 2,
    /**
     * Each entry value will be the index.
     */
    dnx_init_mem_default_mode_index = 3,
    /**
     * Custom, expected a callback to provide the default value.
     * This mode do not support a default value per entry (all the input parameters in the callback shuld be ignored).
     */
    dnx_init_mem_default_mode_custom = 4,

    /**
     * Custom per entry, expected a callback to provide the default value.
     * This mode does not use slam dma and therefore less optimized
     */
    dnx_init_mem_default_mode_custom_per_entry = 5
} dnx_init_mem_default_mode_e;

/**
 * \brief - memory reset mode.
 *          Specify the method used to reset all writable memories that are not listed in dnx data (module='dev_init', sub_module='mem', table='default')
 */
typedef enum
{
    /**
     * Do not reset memories
     */
    dnx_init_mem_reset_mode_none = 0,
    /**
     * Reset using slam DMA
     */
    dnx_init_mem_reset_mode_dma = 1,
    /**
     * Reset using indirect commands mechanism
     */
    dnx_init_mem_reset_mode_indirect = 2,
} dnx_init_mem_reset_mode_e;

/** 
 * \brief 
 * Callback to get custom default value. 
 * For simple custom mode all the input parameters should be ignored (one table for all the memory)
 */
typedef int (
    *dnx_init_mem_default_get_f) (
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);
/*
 * } 
 */

/**
 * \brief - 
 * Set default values for all valid writable memories.
 * This function use slam dma or dma to efficiently write to memories.
 * If the memory included in table dnx_data_dev_init.mem.default, the default value will be set accordingly.
 * Otherwise the memory will be set to 0. 
 *  
 * In a case the memory initialized manually - memory mode should be set to 'skip'. 
 * In a case the memory should be set to 0 - done automatically (no need to add to dnx data table) 
 * Otherwise the memory should be add to dnx data table (instructions below) 
 *  
 *  
 * Defining default value for memory:
 * ----------------------------------
 * 1. Go to device xml in dnx data (module='dev_init', sub_module='mem', table='default')
 * 2. Increase table size by 1
 * 3. Add a new entry:
 *        'index' - is just a running index.
 *        'mem' - is memory define
 *        'mode' - is one of the modes defined in enum 'dnx_init_mem_default_mode_e'
 *        'default_get_cb' - is optional (set to NULL if not required).
 *                           A callback to get the default data.
 *                           in a case the mode is 'custom' all the input parameters in callback should ignored (same default for all entries).
 *                           in a case the mode is 'custom_per_entry' different defaults for different entries might be defined.
 *         'field' - set just specific field (all the reset will be set to 0) - not relevant for custom modes
 * 4. Run dnxdata autocoder            
 *             
 * 
 * \param [in] unit - unit # 
 *  
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_mem_default_init(
    int unit);
/**
 * \brief - Release SW resources allocated in mem default init process ( dnx_init_mem_default_init() )
 *            
 * \param [in] unit - unit # 
 *  
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_mem_default_deinit(
    int unit);

/**
 * \brief - check if the memory has non standard default value and return it is_non_standard (non standard means that 
 *        not all zero).
 *  
 * \param [in] unit - unit # 
 * \param [in] mem - required memory
 * \param [in] is_non_standard - pointer to the result 0 means standard (all zero)
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_mem_default_mode_get(
    int unit,
    soc_mem_t mem,
    int *is_non_standard);

/**
 * \brief - Get default value of memory.
 *          Return the value which defined explicitly in dnx_data_dev_init.mem.default.
 *          If not found will return zero.
 * 
 * \param [in] unit - unit # 
 * \param [in] mem - required memory
 * \param [in] array_index - used for memory arrays (if not relevant set to 0) 
 * \param [in] copyno - block index 
 * \param [in] index - entry index 
 * \param [out] entry - pointer to uint32 buffer (size must be > SOC_MEM_WORDS(unit, mem))
 * \param [out] mode - the mode used to set the defaults
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_mem_default_get(
    int unit,
    soc_mem_t mem,
    int array_index,
    int copyno,
    int index,
    uint32 *entry,
    dnx_init_mem_default_mode_e * mode);

#endif /* _BCMINT_DNX_INIT_INIT_MEM_H_INCLUDED */
