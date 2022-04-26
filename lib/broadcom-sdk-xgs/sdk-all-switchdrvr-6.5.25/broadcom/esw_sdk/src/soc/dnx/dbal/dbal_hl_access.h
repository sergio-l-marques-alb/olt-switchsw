
/**
 *\file dbal_hl_access.h
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DBAL_HL_ACCESS_H_INCLUDED
#define DBAL_HL_ACCESS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**************
 *  INCLUDES  *
 **************/
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <soc/drv.h>    /* SOC_BLOCK_ANY */
#include <shared/bsl.h>

#include "dbal_internal.h"
#include <soc/dnx/dbal/dbal.h>
#include <soc/mem.h>
#include <soc/sand/sand_mem.h>
#include <soc/mcm/memregs.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/init/init_mem.h>
#include <soc/cmic.h>

/*************
 *  DEFINES  *
 *************/
#define DBAL_HL_LOGGER_STATUS_UPDATE                                                                    \
 if(dbal_logger_is_enable(unit, entry_handle->table_id))                                                \
    {                                                                                                   \
        logger_enable = 1;                                                                              \
    }

#define DBAL_ACTION_PERFORMED_LOG                                                       \
        if(logger_enable)                                                               \
        {                                                                               \
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Action performed!\n")));        \
        }                                                                               \


#define DBAL_FIELD_START_LOG(field_id, is_mem, hw_element)                                                  \
    if(logger_enable)                                                     \
    {                                                                                                       \
        LOG_INFO_EX(BSL_LOG_MODULE, "\nHandling field: %s mapped to %s %-30s %s\n", dbal_field_to_string(unit, field_id), \
            is_mem?"memory":"regstr", is_mem?SOC_MEM_NAME(unit, hw_element): SOC_REG_NAME(unit, hw_element), EMPTY);   \
    }

#define DBAL_MEM_READ_LOG( memory, mem_offset, block, mem_array_offset)                                         \
    if(logger_enable)                                                     \
    {                                                                                                       \
        LOG_INFO_EX(BSL_LOG_MODULE, "Reading from memory %-30s, entry offset %d, block %d, array offset %d ... ",  \
            SOC_MEM_NAME(unit, memory), mem_offset, block, mem_array_offset);                               \
    }

#define DBAL_MEM_WRITE_LOG( memory, mem_offset, block, mem_array_offset)                                        \
    if(logger_enable)                                                     \
    {                                                                                                       \
        LOG_INFO_EX(BSL_LOG_MODULE, "Writing into memory %-30s, entry offset %d, block %d, array offset %d ... ",  \
            SOC_MEM_NAME(unit, memory), mem_offset, block, mem_array_offset);                               \
    }

#define DBAL_MEM_WRITE_ARRAY_LOG( memory, mem_offset, block, num_elem)                                              \
    if(logger_enable)                                                         \
    {                                                                                                           \
        LOG_INFO_EX(BSL_LOG_MODULE, "Writing into memory %-30s, entry offset %d, block %d, array elmnts %d ... ",\
            SOC_MEM_NAME(unit, memory), mem_offset, block, num_elem);                                           \
    }

#define DBAL_REG_READ_LOG( reg, reg_offset, block, reg_array_offset)                                                \
    if(logger_enable)                                                         \
    {                                                                                                           \
            LOG_INFO_EX(BSL_LOG_MODULE, "Reading from regstr %-30s, entry offset %d, block %d, array offset %d ... ",\
            SOC_REG_NAME(unit, reg), reg_offset, block, reg_array_offset);                                      \
    }

#define DBAL_REG_WRITE_LOG( reg, reg_offset, block, reg_array_offset)                                               \
    if(logger_enable)                                                         \
    {                                                                                                           \
        LOG_INFO_EX(BSL_LOG_MODULE, "Writing into regstr %-30s, entry offset %d, block %d, array offset %d ... ",    \
             SOC_REG_NAME(unit, reg), reg_offset, block, reg_array_offset);                                     \
    }

#define DBAL_REG_WRITE_ARRAY_LOG( reg, reg_offset, block, num_elem)                                                 \
    if(logger_enable)                                                         \
    {                                                                                                           \
        LOG_INFO_EX(BSL_LOG_MODULE, "Writing into register %-30s, entry offset %d, block %d, array elmnts %d ... ",\
                SOC_REG_NAME(unit, reg), reg_offset, block, num_elem);                                          \
    }

#define DBAL_DATA_LOG( data, is_mem)                                                \
{                                                                                   \
    if(logger_enable)                             \
    {                                                                               \
        int ii;                                                                         \
        int size = is_mem?DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS:SOC_REG_ABOVE_64_MAX_SIZE_U32;\
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Data hex: ")));                        \
        for (ii = 0; ii < size; ii++)                                                   \
        {                                                                               \
            if(sal_memcmp(&(data[ii]), zero_buffer_to_compare, size-ii) == 0) \
            {                                                                           \
                LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit," {0 <repeats %d times>}"),8*(DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS-ii)));\
                ii = DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS;                             \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08x"), data[ii]));      \
            }                                                                           \
        }                                                                               \
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));                          \
    }                                                                                   \
}

/**
 *  \brief
 *  Macro that returns a struct to the HL result info type MEMORY in handle according to the reuslt type
 */
#define DBAL_HL_MUL_RES_INFO(access_type) (entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type])

#define DBAL_HL_MEM_MUL_RES_INFO    DBAL_HL_MUL_RES_INFO(DBAL_HL_ACCESS_MEMORY)

#define DBAL_HL_REG_MUL_RES_INFO    DBAL_HL_MUL_RES_INFO(DBAL_HL_ACCESS_REGISTER)

/*****************************
 * INNER HL ACCESS FUNCTIONS *
 *****************************/

/**
 *\brief
 *  Calculates the block for the memory, using block index info
 *  from the XML.
 *  If there is  block offset, will return the
 *  block index and num_of_blocks == 1.
 *  If no block index specification in the XML, will use block
 *  calculation in a symmetric way: 0..n/2-1 blocks for core 0
 *  and n/2...n-1 blocks for core 1.
 */
shr_error_e dbal_hl_mem_block_calc(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 core_id,
    soc_mem_t memory,
    dbal_offset_encode_info_t * block_index_info,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    int *block,
    int *num_of_blocks);

/**
 *  \brief
 *  Clear a table for a case of HL_TCAM.*/
shr_error_e dbal_hl_tcam_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 *  \brief
 *  Get an entry from HW for a case of HL_TCAM.*/
shr_error_e dbal_hl_tcam_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 *  \brief
 *  Set an entry in HW for a case of HL_TCAM.*/
shr_error_e dbal_hl_tcam_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 *  \brief
 *  Get next for an HL TCAM iterator.*/
shr_error_e dbal_hl_tcam_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 *  \brief
 *  Determines if the entry is default for tcam table
 *  Takes into consideration if mask is bitwise not encoded
 */
shr_error_e dbal_hl_tcam_is_entry_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default);

/**
 *  \brief
 *  General hook function for entry get, dispatcher for all hooks according to tables. incase of no hook is
 *  implemented for this table the function returns error */
shr_error_e dbal_hl_hook_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *continue_regular_process);

/**
 *  \brief
 *  General hook function for entry set, dispatcher for all hooks according to tables.
 *  In case of no hook is implemented for this table the function returns error.
 */
shr_error_e dbal_hl_hook_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);

/*
 * \brief
 * Encode the input_val into output_val according to encoding parameters
 */
shr_error_e dbal_hl_access_encode(
    int unit,
    dbal_value_field_encode_types_e encode_type,
    uint32 encode_param,
    uint32 *input_val,
    uint32 *output_val,
    uint32 field_size);

/*
 * \brief
 * Decode the input_val into output_val according to encoding parameters
 */
shr_error_e dbal_hl_access_decode(
    int unit,
    dbal_value_field_encode_types_e encode_type,
    uint32 encode_param,
    uint32 *input_val,
    uint32 *output_val,
    uint32 field_size);

/**
 * \brief
 * General hook function for entry res_type_resolution, dispatcher for all hooks according to tables.
 * In case of no hook is implemented for this table the function returns error
 */
shr_error_e dbal_hl_hook_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *continue_regular_process);

/**
 * \brief
 * this function writes from the table according to the access type (register/memory/sw).
 */
shr_error_e dbal_hl_set_by_access_type(
    int unit,
    dbal_hard_logic_access_types_e access_type,
    dbal_entry_handle_t * entry_handle);

/**
 * \brief
 * this function reads from the table according to the access type (register/memory/sw).
 */
shr_error_e dbal_hl_get_by_access_type(
    int unit,
    dbal_hard_logic_access_types_e access_type,
    dbal_entry_handle_t * entry_handle,
    int result_type);

/**
 * \brief
 * Set entry in OAMP_RMEP_DB table
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_handle - Pointer to DBAL struct
 */
shr_error_e dbal_hl_hook_oamp_rmep_db_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 * \brief
 * Translate MEP DB address from logical method to physical location
 * \param [in] unit - Number of hardware unit used.
 * \param [in] logical_address - entry address that has column in LSBs, offset in MSBs.
 *
 * \return
 *   resulting entry address that has column in the MSBs, offset in the LSBs
 */
uint32 dbal_hl_hook_oam_calc_access_address(
    int unit,
    uint32 logical_address);

/**
 * \brief
 * Get entry in OAMP_MEP_DB table
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_handle - Pointer to DBAL struct
 */
shr_error_e dbal_hl_hook_oamp_mep_db_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 * \brief
 * Set entry in OAMP_MEP_DB table
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_handle - Pointer to DBAL struct
 */
shr_error_e dbal_hl_hook_oamp_mep_db_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);

#endif /* DBAL_HL_ACCESS_H_INCLUDED */
