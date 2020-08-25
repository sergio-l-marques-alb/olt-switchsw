/*
 * ! \file mdb_sbusdma_desc.h $Id$ Contains all of the MDB descriptor DMA implementation APIs.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef MDB_H_SBUSDMA_DESC_INCLUDED

#define MDB_H_SBUSDMA_DESC_INCLUDED

/* 
 * INCLUDES 
 * {
 */

#include <soc/mem.h>
#include <soc/drv.h>
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
 *  Allocate and initialize all of the descriptor DMA resources.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e mdb_sbusdma_desc_init(
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
shr_error_e mdb_sbusdma_desc_deinit(
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
shr_error_e mdb_sbusdma_desc_wait_done(
    int unit);

/**
 * \brief
 *  Add a memory write to the descriptor DMA chain.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *   \param [in] mem - The target memory.
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
shr_error_e mdb_sbusdma_desc_add_mem(
    int unit,
    soc_mem_t mem,
    uint32 array_index,
    int blk,
    uint32 offset,
    void *entry_data);

/**
 * \brief
 *  Add a register write to the descriptor DMA chain.
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 *   \param [in] reg - The target register.
 *   \param [in] instance - The register instance.
 *   \param [in] array_index - The register array index within
 *          the instance.
 *   \param [in] entry_data - The data to write to the register.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT:
 *  Non-zero in case of an error
 */
shr_error_e mdb_sbusdma_desc_add_reg(
    int unit,
    soc_reg_t reg,
    int instance,
    uint32 array_index,
    void *entry_data);

/**
 * \brief
 *  Get a TRUE/FALSE indication if the descriptor DMA is enabled
 * \par DIRECT INPUT
 *  \param [in] unit - The unit number.
 * \par INDIRECT INPUT
 *  See entry above.
 * \par DIRECT OUTPUT: 
 *  Returns TRUE/FALSE
 */
uint32 mdb_sbusdma_desc_is_enabled(
    int unit);

/*
 * }
 */

#endif /* !MDB_H_SBUSDMA_DESC_INCLUDED */
