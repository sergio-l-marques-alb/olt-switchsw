/*
 * ! \file dnx_sbusdma_desc.h $Id$ Contains all of the DNX descriptor DMA implementation APIs.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DNX_H_SBUSDMA_DESC_INCLUDED

#define DNX_H_SBUSDMA_DESC_INCLUDED

/* 
 * INCLUDES 
 * {
 */

#include <shared/shrextend/shrextend_debug.h>

/*
 * }
 */

/* 
 * DEFINES 
 * {
 */

/*
 * }
 */

/* 
 * ENUMS 
 * {
 */

/*
 *\brief The modules supporting descriptor DMA
 */
typedef enum
{
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM = 0,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_EEDB = 1,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_INLIF = 2,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_VSI = 3,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_FEC = 4,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_MAP = 5,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_ISEM = 6,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM = 7,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_OEM = 8,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_PPMC = 9,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_GLEM = 10,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_ESEM = 11,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_EXEM = 12,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_RMEP_EM = 13,
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES
} sbusdma_desc_module_enable_e;

/*
 * }
 */

/* 
 * MACROS 
 * {
 */

/*
 * }
 */

/* 
 * TYPEDEFS 
 * {
 */

typedef struct dnx_sbusdma_desc_stats_s
{
    /*
     * The current descriptor DMA configuration.
     * Configured either by soc properties or diag commands
     */
    uint32 buff_size_kb;
    uint32 chain_length_max;
    uint32 timeout_usec;
    uint32 enabled;

    /*
     *  commit_counter - Total number of commits done.
     *  chain_length_counter_max - Max chain_length used in a single descriptor DMA chain.
     *  chain_length_counter_total - Total number of descriptors used across all commits.
     *  buff_size_kb_max - Max buff_size_kb used in a single descriptor DMA chain, note a rounding up error between kb and uint32.
     *  buff_size_kb_total - Total size of buff_size used across all commits, note a rounding up error between kb and uint32.
     */
    uint32 commit_counter;
    uint32 chain_length_counter_max;
    uint32 chain_length_counter_total;
    uint32 buff_size_kb_max;
    uint32 buff_size_kb_total;
} dnx_sbusdma_desc_stats_t;

/*
 * }
 */

/* 
 * GLOBALS 
 * {
 */

/*
 * }
 */

/* 
 * FUNCTIONS 
 * {
 */

/**
 * \brief
 *  Allocate and initialize all of the descriptor DMA resources based on the provided parameters.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *  \param [in] buff_size_kb - Total size in KB of dmmable memory allocated in favor of the descriptor DMA double-buffer.
 *  \param [in] chain_length_max - Maximal number of descriptors in a single chain.
 *  \param [in] timeout_usec - Timeout between the creation of a descriptor chain and its commit to HW, set 0 to disable timeout thread.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e dnx_sbusdma_desc_init_with_params(
    int unit,
    uint32 buff_size_kb,
    uint32 chain_length_max,
    uint32 timeout_usec);

/**
 * \brief
 *  Retrieve various statistics related to the status of the descriptor DMA at commit times.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *  \param [in] clear - Indicate to clear the counters.
 *  \param [out] desc_stats - The various descriptor DMA statistics.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e dnx_sbusdma_desc_get_stats(
    int unit,
    int clear,
    dnx_sbusdma_desc_stats_t * desc_stats);

/**
 * \brief
 *  Allocate and initialize all of the descriptor DMA resources based on the soc properties.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e dnx_sbusdma_desc_init(
    int unit);

/**
 * \brief
 *  Commit any remaining entries in the descriptor chain to HW
 *  and free all of the resources allocated by the descriptor
 *  DMA.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e dnx_sbusdma_desc_deinit(
    int unit);

/**
 * \brief
 *  Commit the descriptor chain to HW and wait until it
 *  finishes.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e dnx_sbusdma_desc_wait_done(
    int unit);

/**
 * \brief
 *  Add a memory write of a signle entry to the descriptor DMA chain.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *   \param [in] mem - The target memory, soc_mem_t.
 *   \param [in] array_index - The memory array index.
 *   \param [in] blk - The memory block.
 *   \param [in] offset - The memory offset (row).
 *   \param [in] entry_data - The data to write to the
 *       memory.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e dnx_sbusdma_desc_add_mem(
    int unit,
    uint32 mem,
    int array_index,
    int blk,
    uint32 offset,
    void *entry_data);

/**
 * \brief
 *  Add a memory write of multiple entries to the descriptor DMA chain.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *   \param [in] mem - The target memory, soc_mem_t.
 *   \param [in] array_index - The memory array index.
 *   \param [in] blk - The memory block.
 *   \param [in] offset - The memory offset (row).
 *   \param [in] count - Amount of entries to write.
 *   \param [in] entry_data - The data to write to the memory.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e dnx_sbusdma_desc_add_mem_table(
    int unit,
    uint32 mem,
    int array_index,
    int blk,
    uint32 offset,
    uint32 count,
    void *entry_data);

/**
 * \brief
 *  Add a memory clear of a whole table to the descriptor DMA chain.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *   \param [in] mem - The target memory, soc_mem_t.
 *   \param [in] array_index - The memory array index (-1 clears all mem indices).
 *   \param [in] blk - The memory block.
 *   \param [in] entry_data - The data to write to the memory entries
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e dnx_sbusdma_desc_add_mem_clear(
    int unit,
    uint32 mem,
    int array_index,
    int blk,
    void *entry_data);

/**
 * \brief
 *  Add a register write to the descriptor DMA chain.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *   \param [in] reg - The target register, soc_reg_t.
 *   \param [in] instance - The register instance.
 *   \param [in] array_index - The register array index within
 *          the instance.
 *   \param [in] entry_data - The data to write to the register.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e dnx_sbusdma_desc_add_reg(
    int unit,
    uint32 reg,
    int instance,
    uint32 array_index,
    void *entry_data);

/**
 * \brief
 *  Get a TRUE/FALSE indication if the descriptor DMA is enabled
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *  \param [in] module_enum - The relevant module.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT: 
 *  Returns TRUE/FALSE
 */
uint32 dnx_sbusdma_desc_is_enabled(
    int unit,
    sbusdma_desc_module_enable_e module_enum);

/*
 * }
 */

#endif /* !DNX_H_SBUSDMA_DESC_INCLUDED */
