/*******************************************************************************
 *
 * Copyright 2011-2019 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in an
 * Authorized License, Broadcom grants no license (express or implied), right to
 * use, or waiver of any kind with respect to the Software, and Broadcom expressly
 * reserves all rights in and to the Software and all intellectual property rights
 * therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
 * SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 * OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *******************************************************************************/

#ifndef __KAPS_IX_MGR_H
#define __KAPS_IX_MGR_H

#include <stdint.h>
#include "kaps_errors.h"
#include "kaps_hw_limits.h"
#include "kaps_pool.h"
#include "kaps_portable.h"
#include "kaps_device.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct kaps_ad_db;

/*Maximum number of indexes that can be affected during a compaction operation*/
#define KAPS_MAX_IX_INVOLVED_IN_COMPACTION (20 * 1024)

/*Maximum number of holes per LSN that will be retained during IX compaction*/
#define KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION (4)

/*Maximum number of holes per LSN that will be retained during IX compaction*/
#define KAPS_MAX_RESV_IX_FOR_SPECIAL_ENTRIES (2048)





/**
 *
 * @cond INTERNAL
 *
 * @file kaps_ix_mgr.h
 *
 * Internal details and structures used for managing index space
 */

/**
 * Properties of a single Index chunk
 */

    typedef enum kaps_ix_chunk_type
    {
        IX_FREE_CHUNK = 0,  /**< Chunk available for allocation */
        IX_ALLOCATED_CHUNK
                        /**< Chunk already allocated */
    } kaps_ix_chunk_type;

/**
 * Identifies who is using the IX chunk
 */
    typedef enum kaps_ix_user_type
    {
        IX_USER_LSN = 0,/**< The IX chunk is used by an LSN */
        IX_USER_OVERFLOW, /**< The IX chunk is used by overflow*/
        IX_LMPSOFAR       /**< The IX Chunk is used by Lmpsofar Pfx */
    } kaps_ix_user_type;

/**
 * @brief Chunk used to represent a region of contiguous space in the  index space
 */

    struct kaps_ix_chunk
    {
        uint32_t start_ix:27;
                          /**< starting index of the chunk*/
        uint32_t size:27;  /**< Size of the index space */
        uint32_t type:1;
                     /**< Chunk type (free or allocated) */
        uint32_t user_type:1;
                          /**< whether LSN or overflow is using the ix chunk*/
        void *lsn_ptr;      /**< pointer to the LSN using the ix chunk */
        struct kaps_ad_chunk *ad_info; /**< AD information if there is an AD associated*/
        struct kaps_ix_chunk *prev_free_chunk;
                                           /**< pointer to the previous chunk of same size in the doubly linked list */
        struct kaps_ix_chunk *next_free_chunk;
                                           /**< pointer to the next chunk of the same size in the doubly linked list */
        struct kaps_ix_chunk *prev_neighbor;
                                           /**< pointer to the previous neighboring chunk (neighbor may be allocated or free) */
        struct kaps_ix_chunk *next_neighbor;
                                           /**< pointer to the next neighboring chunk (neighbor may be allocated or free) */
    };

/**
 * @brief Chunk used to playback the index compaction on the LSNs
 */
    struct kaps_ix_compaction_chunk
    {
        struct kaps_ix_chunk *kaps_ix_chunk;
                                          /**< IX chunk being compacted*/
        int32_t from_start_ix;  /**< starting index of the source*/
        int32_t from_end_ix;    /**< ending index of the source */
        int32_t from_size;      /**< size of the source */
        int32_t to_start_ix;    /**< starting index of the destination*/
        int32_t to_end_ix;      /**< ending index of the destination */
        int32_t to_size;        /**< size of the destination */
        int32_t ad_blk_no;          /**< UDA SB No of the destination */
        struct kaps_ix_compaction_chunk *next;
                                             /**< pointer to the next playback chunk */
    };

/**
 * @brief Index Manager.
 */

    struct kaps_ix_mgr
    {
        struct kaps_ix_chunk **free_list;
                                      /**< Array having pointers to the head of the doubly linked lists.
                                                              Each list has free chunks of the same size. chunk_list[0] has free chunks
                                                              of size 1 and chunk_list[MAX_NUM_CHUNK_LEVELS-1] has free chunks of size >=
                                                              ::MAX_NUM_CHUNK_LEVELS */
        uint32_t *num_ix_in_free_list;
                                   /**< Array that is dynamically created. num_ix_free_list[0] has the total size
                                                              of all the free chunks in the linked list at free_list[0]*/

        uint32_t daisy_chain_id;  /** Daisy chain of the Index Manager*/

        int32_t start_ix;
                      /**< starting index assigned to the index manager*/
        int32_t end_ix;
                    /**< ending index assigned to the index manager */
        int32_t size;
                    /**< Size of the index space assigned to the index manager*/

        int32_t cb_start_ix;
                           /**< compaction buffer starting index (for coherency ix chunks will be first moved to the
                                                        compaction buffer IX range and then be moved back to normal IX range*/
        int32_t cb_end_ix; /**< compaction buffer ending index (for coherency)*/
        int32_t cb_size;   /**< size of the compaction buffer*/

        uint32_t max_alloc_chunk_size;
                                   /**< maximum size of the IX allocation chunk*/
        uint32_t max_num_chunk_levels;  /* Maximum number of levels in the array of free lists */

        int32_t ref_count;
                       /**< Num DB Pointing to this mgr */
        int32_t is_finalized;
                          /**< is finalized called on this mgr */
        int32_t already_calculated;
        struct kaps_ix_chunk *neighbor_list;
                                         /**< List consisting of allocated and free chunks
                                            arranged in sorted order*/
        struct kaps_ix_chunk *neighbor_list_end;
                                             /**< pointer to the tail (end chunk) of the neighbor list*/
        struct kaps_ix_chunk *last_rembered_chunk;
        uint32_t num_allocated_ix;
                                /**< Total Number of allocated indexes that have been allocated*/
        uint32_t ix_translate[KAPS_HW_MAX_UDA_SB];
                                                  /**< UDA SB address AD entry translation */

        uint32_t ix_segments_start[KAPS_HW_MAX_UDA_SB];
                                                /**< IX segments start locations */
        uint32_t ix_segments_end[KAPS_HW_MAX_UDA_SB];
                                                    /**< IX segments end locations */
        struct kaps_ix_mgr *next;
                                /**< Pointer to next IX mgr in case of multiple AD databases */
        struct kaps_ad_db *main_ad_db;
                                   /**< ad_db mgr is handling with */
        struct kaps_ad_db *alloc_ad_db;

        POOL_DEF(kaps_ix_chunk) ix_chunk_pool;          /**< Pool for ix_chunks */

        struct kaps_device *device;  /**< pointer to the memory allocator */
        uint32_t num_entries_per_ad_blk;
                                 /**< Number of entries per UDA super block*/

        uint32_t ix_boundary_per_ad_blk; /**< The boundary size between two blocks*/                               
    };

/**
 * @brief Statistics associated with the Index manager
 */

    struct kaps_ix_mgr_stats
    {
        uint32_t total_num_allocated_ix;
                                     /**< Number of allocated indexes in the index space */
        uint32_t total_num_free_ix;  /**< Number of free indexes in the index space */
        uint32_t num_entries_per_sb[256];
        uint32_t num_entries_present[256];

    };

/**
 * Initialize the Index Manager
 *
 * @param device valid device handle
 * @param max_alloc_chunk_size Maximum size of an allocated index chunk that is possible
 * @param ad_db Valid AD database pointer for which the IX Manager should be initialized
 * @param daisy_chain_id  ID of the daisy chain
 * @param is_warmboot flag to identify is device is in Warmboot mode.
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_ix_mgr_init(
    struct kaps_device *device,
    uint32_t max_alloc_chunk_size,
    struct kaps_ad_db *ad_db,
    uint32_t daisy_chain_id,
    uint8_t is_warmboot);

/**
 * Initialize the Index Manager for all the AD databases in the list
 *
 * @param device valid device handle
 * @param max_alloc_chunk_size Maximum size of an allocated index chunk that is possible
 * @param ad_db the first AD database pointer in the AD database linked list
 * @param is_warmboot flag to identify is device is in Warmboot mode.
 *
 * @return KAPS_OK on success or an error code
 */

kaps_status
kaps_ix_mgr_init_for_all_ad_dbs(
    struct kaps_device * device,
    uint32_t max_alloc_chunk_size,
    struct kaps_ad_db * ad_db,
    uint8_t is_warmboot);






/**
 * Allocates a free chunk of contiguous indexes of requested size
 *
 * @param mgr_p valid Index Manager initialized using kaps_ix_mgr_init()
 * @param rqt_size Number of contiguous indexes to be allocated
 * @param lsn_ptr the LSN which is requesting the IX space. Can be NULL
 * @param user_type whether LSN or overflow is using the IX space
 * @param chunk contains pointer to the allocated chunk. In case allocation is
 * unsuccessful, a NULL pointer is returned
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_ix_mgr_alloc(
    struct kaps_ix_mgr *mgr_p,
    struct kaps_ad_db *ad_db,
    uint32_t rqt_size,
    void *lsn_ptr,
    enum kaps_ix_user_type user_type,
    struct kaps_ix_chunk **chunk);

/**
 * Allocates a free chunk of contiguous indexes of requested size from the requested location
 *
 * @param mgr_p valid Index Manager initialized using kaps_ix_mgr_init()
 * @param rqt_size Number of contiguous indexes to be allocated
 * @param start_ix Start index of the indexes to be allocated
 * @param lsn_ptr the LSN which is requesting the IX space. Can be NULL
 * @param user_type whether LSN or overflow is using the IX space
 * @param chunk contains pointer to the allocated chunk. In case allocation is
 * unsuccessful, a NULL pointer is returned
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_ix_mgr_wb_alloc(
    struct kaps_ix_mgr *mgr,
    struct kaps_ad_db *ad_db,
    uint32_t rqt_size,
    uint32_t start_ix,
    void *lsn_ptr,
    enum kaps_ix_user_type user_type,
    struct kaps_ix_chunk **alloc_chunk_pp);

/**
 * Creates the freelist structure and sorts the neighbor list after warmboot is completed
 *
 * @param mgr_p valid Index Manager initialized using kaps_ix_mgr_init()
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_ix_mgr_wb_finalize(
    struct kaps_ix_mgr *mgr);




kaps_status
kaps_ix_mgr_wb_finalize_for_all_ad_dbs(
    struct kaps_device *device,
    struct kaps_ad_db *ad_db);


/**
 * Checks if a grow up operation is possible and returns the number of indexes by which we can grow up
 * Grow up involves acquiring the preceeding indexes
 *
 * @param mgr valid Index Manager initialized using kaps_ix_mgr_init()
 * @param cur_ix_chunk Already allocated index chunk which needs to grow
 * @param grow_up_size_p Number of contiguous indexes by which we can grow up is returned here
 *
 * @return KAPS_OK on success
 */
    kaps_status kaps_ix_mgr_check_grow_up(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *cur_ix_chunk,
    uint32_t * grow_up_size_p);

/**
 * Perform the Grow up operation by acquiring the preceeding indexes. The kaps_ix_mgr_check_grow_up should be called
 * before calling this function to make sure that grow up can occur
 *
 * @param mgr valid Index Manager initialized using kaps_ix_mgr_init()
 * @param cur_ix_chunk Already allocated index chunk which needs to grow
 * @param grow_up_size Number of indexes by which to grow up
 *
 * @return KAPS_OK on success
 */
    kaps_status kaps_ix_mgr_grow_up(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *cur_ix_chunk,
    uint32_t grow_up_size);

/**
 * Checks if a grow down operation is possible and returns the number of indexes by which we can grow down
 * Grow down involves acquiring the next set of contiguous indexes after the current chunk
 *
 * @param mgr valid Index Manager initialized using kaps_ix_mgr_init()
 * @param cur_ix_chunk Already allocated index chunk which needs to grow
 * @param grow_down_size_p Number of contiguous indexes by which we can grow down is returned here
 *
 * @return KAPS_OK on success
 */
    kaps_status kaps_ix_mgr_check_grow_down(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *cur_ix_chunk,
    uint32_t * grow_down_size_p);

/**
 * Perform the Grow down operation by acquiring the next set of contiguous indexes after the current chunk.
 * The kaps_ix_mgr_check_grow_up should be called before calling this function to make sure that grow up can occur
 *
 * @param mgr valid Index Manager initialized using kaps_ix_mgr_init()
 * @param cur_ix_chunk Already allocated index chunk which needs to grow
 * @param grow_down_size Number of indexes by which to grow down
 *
 * @return KAPS_OK on success
 */
    kaps_status kaps_ix_mgr_grow_down(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *cur_ix_chunk,
    uint32_t grow_down_size);

/**
 * Free an allocated chunk of indexes and returns it to the Index Manager
 *
 * @param mgr_p valid Index Manager initialized using kaps_ix_mgr_init()
 * @param chunk pointer to the chunk that should be freed
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_ix_mgr_free(
    struct kaps_ix_mgr *mgr_p,
    struct kaps_ix_chunk *chunk);

/**
 * Compacts a region in the IX space and returns the playback actions to be performed on the LSNs
 *
 * @param mgr valid Index Manager initialized using kaps_ix_mgr_init()
 * @param ad_db AD database for which compaction should be done
 * @param was_compaction_done whether compaction was successfully performed is returned
 * @param playback_head_pp the head of the list containing the playback actions is returned
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_ix_mgr_compact(
    struct kaps_ix_mgr *mgr,
    struct kaps_ad_db *ad_db,
    int32_t * was_compaction_done,
    struct kaps_ix_compaction_chunk **playback_head_pp);

/**
 * Destroy the Index Manager and free up the heap memory
 *
 * @param mgr_p valid Index Manager initialized using kaps_ix_mgr_init()
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_ix_mgr_destroy(
    struct kaps_ix_mgr *mgr_p);




/**
 * Destroy the Index Manager for all the AD databases
 *
 * @param ad_db the first AD database pointer in the AD database linked list
 *
 * @return KAPS_OK on success or an error code
 */

kaps_status
kaps_ix_mgr_destroy_for_all_ad_dbs(
    struct kaps_device *device,
    struct kaps_ad_db *ad_db);


/**
 * Calculates and returns the Index manager stats
 *
 * @param mgr_p valid Index Manager initialized using kaps_ix_mgr_init()
 * @param ix_stats valid memory for storing the statistics
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_ix_mgr_calc_stats(
    const struct kaps_ix_mgr *mgr_p,
    struct kaps_ix_mgr_stats *ix_stats);

/**
 * Pretty print the Index Manager stats
 *
 * @param ix_stats valid stats initialized using kaps_ix_mgr_calc_stats()
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_ix_mgr_print_stats(
    struct kaps_ix_mgr_stats *ix_stats);

/**
 * Verifiy the Index Manager data structures
 *
 * @param mgr valid Index Manager initialized using kaps_ix_mgr_init()
 * @param recalc_neighbor_list_end this is set to 1 if we want the mgr->neighbor_list_tail to be properly calculated in the verify function
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_ix_mgr_verify(
    struct kaps_ix_mgr *mgr,
    uint32_t recalc_neighbor_list_end);

/**
 * Saves the UDA Translate Table during ISSU
 *
 * @param mgr valid Index Manager initialized using kaps_ix_mgr_init()
 * @param write_fn User callback function to save data to non-volatile memory
 * @param handle User handle passed back through read/write callback functions
 * @param nv_offset Starting offset in non-volatile memory to save data
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_ix_mgr_wb_save_ix_translate(
    struct kaps_ix_mgr *mgr,
    kaps_device_issu_write_fn write_fn,
    void *handle,
    uint32_t * nv_offset);

/**
 * Restores the UDA Translate Table during ISSU
 *
 * @param mgr valid Index Manager initialized using kaps_ix_mgr_init()
 * @param read_fn User callback function to restore data from non-volatile memory
 * @param handle User handle passed back through read/write callback functions
 * @param nv_offset Starting offset in non-volatile memory to read data
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_ix_mgr_wb_restore_ix_translate(
    struct kaps_ix_mgr *mgr,
    kaps_device_issu_write_fn read_fn,
    void *handle,
    uint32_t * nv_offset);



/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif
#endif /*__IX_MGR_H*/
