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

#ifndef __KAPS_UDA_MGR_H
#define __KAPS_UDA_MGR_H

#include "kaps_errors.h"
#include "kaps_hw_limits.h"
#include "kaps_pool.h"
#include "kaps_portable.h"
#include "kaps_algo_hw.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *
 * @cond INTERNAL
 *
 * @file kaps_uda_mgr.h
 *
 * Internal details and structures used for managing Memory in a UDA Partition
 * A brick is a 1Kb basic unit in an LPU
 */

    struct kaps_device;

/**
 * Number of levels in the array of lists
 */

#define MAX_NUM_CHUNK_LEVELS        (28)

/** Max possible number of UDA Regions */
#define MAX_UDA_REGIONS 48

/**
 * type of a single UDA chunk
 */

#define FREE_CHUNK      0 /** Chunk available for allocation */
#define ALLOCATED_CHUNK 1 /** Chunk already allocated */

#define UDM_NO_HALF     0
#define UDM_LOWER_HALF  1
#define UDM_HIGHER_HALF 2
#define UDM_BOTH_HALF   3

/**
 * @brief Chunk used to represent a region of contiguous space in the UDA memory area
 */

    struct uda_mem_chunk
    {
        struct kaps_db *db;     /* Associated Database Pointer */
        void *handle;           /* Information related to this chunk */
        struct uda_mem_chunk *prev_neighbor;    /* pointer to the previous neighboring chunk (neighbor may be allocated 
                                                 * or free) */
        struct uda_mem_chunk *next_neighbor;    /* pointer to the next neighboring chunk (neighbor may be allocated or
                                                 * free) */
        struct uda_mem_chunk *prev_free_chunk;  /* pointer to the previous chunk of same size in the doubly linked list 
                                                 */
        struct uda_mem_chunk *next_free_chunk;  /* pointer to the next chunk of the same size in the doubly linked list 
                                                 */

        uint32_t offset:20;     /* starting offset of the chunk, varies from 0 to 512K-1 */
        uint32_t region_id:6;   /* initial chunk id, varies from 0 to 64 */
        uint32_t rem_space:5;   /* remaining overallocated space in this chunk, varies from 0 to 15 */
        uint32_t type:1;
                     /** Chunk type (free or allocated) */
        uint32_t size:20;       /* Size of the chunk measured in LPUs, varies from 0 to 512K */
        struct uda_mem_chunk **prev_partial_free_chunk; /* pointer to the previous overallocated chunk (same amount of
                                                         * rem_space), allocating zero memory here, and re-using the
                                                         * space for "prev_free_chunk" pointer for this as they will
                                                         * never be used at same time */
        struct uda_mem_chunk **next_partial_free_chunk; /* pointer to the next overallocated chunk (same amount of
                                                         * rem_space), allocating zero memory for the similar reason
                                                         * above */
    };

/**
 * @brief UDA Memory Chunks Info.
 */
    struct chunk_info
    {
        struct uda_mem_chunk *chunk_p;
        uint32_t lsn_id;
    };

/**
 * @brief UDA Memory Manager.
 */
    struct region
    {
        uint8_t udms[MAX_NUM_CHUNK_LEVELS];
        uint8_t device_udm_sel_table_index;
        uint8_t region_powered_up;      /* whether the region has been powered up */
        uint16_t num_lpu;       /* number of UDMS belongs to the region */
        uint32_t num_row;       /* number of rows of a UDM belongs to this region */
    };

    typedef kaps_status(
    *UdaMgr__UdaSBC) (
    struct uda_mem_chunk * uda_chunk,
    int32_t to_region_id,
    int32_t to_offset);
    typedef kaps_status(
    *UdaMgr__UpdateIIT) (
    struct uda_mem_chunk * uda_chunk);
    typedef void (
    *UdaMgr__update_lsn_size) (
    struct kaps_db * db);

    struct uda_config
    {
        uint8_t joined_udcs[MAX_NUM_CHUNK_LEVELS];      /* If UDC 4 and UDC 5 are joined, then joined_udcs[5] will be
                                                         * set to 1 */
    };

    struct kaps_uda_mgr
    {
        UdaMgr__UdaSBC uda_mgr_sbc;
        UdaMgr__UpdateIIT uda_mgr_update_iit;
        UdaMgr__update_lsn_size uda_mgr_update_lsn_size;

        struct kaps_db *db;
        struct kaps_device *device;
        struct kaps_db *other_shared_dbs[KAPS_NUM_DB_PER_DEVICE];
        int8_t num_other_shared_dbs;
        struct uda_mem_chunk *free_list[MAX_NUM_CHUNK_LEVELS];  /* Array having pointers to the head of the doubly
                                                                 * linked lists. Each list has free chunks of the same
                                                                 * level. chunk_list[0] has free chunks of level 0, and 
                                                                 * * chunk_list[MAX_NUM_CHUNK_LEVELS-1] has free chunks
                                                                 * of level MAX_NUM_CHUNK_LEVELS-1. level is defined by
                                                                 * minimum(chunk_size, num_lpus in this region) */
        struct uda_mem_chunk *partial_free_list[MAX_NUM_CHUNK_LEVELS];
                                                                   /**< similar to free_list, used for overallocation of UDA chunks.
                                                                        Here each list has free chunks of the same overallocated_space. */
        struct kaps_allocator *alloc;   /* KBP allocator pointer */
        uint8_t alloc_udm[KAPS_MAX_NUM_CORES][KAPS_UDM_PER_UDC][KAPS_ALLOC_UDM_MAX_COLS];       /* bitmap of the UDMs
                                                                                                 * allocated to this
                                                                                                 * UDA mgr */

        struct region region_info[MAX_UDA_REGIONS];     /* regions of UDA memory allocated to this UDA mgr */
        struct region compacted_region; /* Compacted region */
        uint8_t enable_compaction;      /* is compaction enabled */

        struct uda_mem_chunk *neighbor_list;    /* List consisting of allocated and free chunks arranged in sorted
                                                 * order */

        struct uda_config config;       /* used to control the configuration of the UDA Manager */

        int32_t max_lpus_in_a_chunk;    /* used in case of overallocation. indicates the max size of over-allocated
                                         * chunk */
        uint32_t num_allocated_lpu_bricks;      /* Total Number of allocated LPU bricks in this UDA Mgr */
        uint32_t num_allocated_lpu_bricks_per_region[MAX_UDA_REGIONS];  /* Total Number of allocated LPU bricks in this 
                                                                         * Region */
        uint32_t total_lpu_bricks;      /* Total Number of LPU bricks in this UDA Mgr */
        uint32_t available_big_chunk_bricks;    /* keeps track of only big_chunks */
        int32_t max_rows_in_udm;        /* max no of rows present in a UDM in the device */
        int16_t dt_index;
        uint16_t no_of_regions;

            POOL_DEF(
    uda_mem_chunk) uda_chunk_pool;      /* Pool for UDA chunks */
        struct chunk_info **chunk_map[MAX_UDA_REGIONS]; /* Contains the pointer to uda_mem_chunk and the LSN ID for
                                                         * each LPU row */
        /*
         * flags 
         */
        uint32_t unit_test_mode:1;      /* when this flag is enabled, UDA manager does not enable the allocated UDMs */
        uint32_t enable_dynamic_allocation:1;   /* whether to enable dynamic allocation or not */
        uint32_t enable_over_allocation:1;      /* whether to enable overallocation or not */
        uint32_t is_finalized:1;        /* Whether we finalized already */
        uint32_t which_udm_half:2;      /* 0 means none, 1 means lower half of the UDM is in use, 2 means right half, 3 
                                         * means both halves */
        uint32_t chunk_alloc_started:1; /* indicates if the allocation of chunks started by SDK. Used for populating
                                         * the regions into FW */
    };

/**
 * @brief Statistics associated with the UDA memory manager
 */

    struct uda_mgr_stats
    {
        uint32_t total_num_allocated_lpu_bricks;        /* Number of allocated bricks in the UDA */
        uint32_t total_num_free_lpu_bricks;     /* Number of free bricks in the UDA */
    };

/**
 * Initialize the UDA Memory Manager
 *
 * @param db db handle
 * @param dt_index index of the decision tree to whom this UDA mgr belongs to
 * @param alloc allocator to use for allocations
 * @param alloc_udm bitmap of UDMs to use for this UDA manager
 * @param mgr_pp contains the pointer to the initialized UDA Memory Manager
 * @param max_lpus_in_a_chunk the LSN size, used for overallocation, indicates maximum no of LPUs which will be requested in one chunk
 * @param enable_over_allocation flag to enable overallocation
 * @param is_warmboot Specifies if SDK in warmboot mode
 * @param which_udm_half is only lower UDM half is allocated, or higher half, or both halves
 * @param unit_test_mode to run only in unit testing mode
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_uda_mgr_init(
    struct kaps_device *device,
    struct kaps_db *db,
    int8_t dt_index,
    struct kaps_allocator *alloc,
    uint8_t alloc_udm[][KAPS_UDM_PER_UDC][KAPS_ALLOC_UDM_MAX_COLS],
    struct kaps_uda_mgr **mgr_pp,
    uint8_t max_lpus_in_a_chunk,
    uint8_t enable_over_allocation,
    uint8_t is_warmboot,
    uint8_t which_udm_half,
    UdaMgr__UdaSBC,
    UdaMgr__UpdateIIT,
    UdaMgr__update_lsn_size,
    int8_t unit_test_mode);

/**
 * Initialize the UDA Memory Manager
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param config parameter for configuring the UDA Manager
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_uda_mgr_configure(
    struct kaps_uda_mgr *mgr,
    struct uda_config *config);

/**
 * Removes the reference of a DB from its common UDA Manager
 *
 * @param db Valid db handle
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status uda_mgr_remove_db_from_common_uda_mgr(
    struct kaps_db *db);

/**
 * Expand the size of the UDA manager.
 *
 * Assumptions: This works only with horizontal partitioning
 * where we are adding a bunch or rows that are above or
 * below the UDA memory managers existing memory.
 * The number of LPUs in a row is assumed to be the same as specified
 * in kaps_uda_mgr_init() call.
 *
 * @param mgr valid pointer to UDA memory manager
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_uda_mgr_expand_all_allocated_udms(
    struct kaps_uda_mgr *mgr);

/**
 * Allocates a free chunk of LPUs of requested size in the UDA partition
 *
 * @param mgr_p valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param rqt_num_lpus Size (number of LPUs) of the free chunk to be allocated
 * @param start_row Start row of the location of the LPU
 * @param start_lpu Start LPU number of the location of the LPU
 * @param lsn_id LSN id of the requesting LSN
 * @param chunk contains pointer to the allocated chunk. In case
 *        allocation is unsuccessful, a NULL pointer is returned
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_uda_mgr_wb_alloc(
    struct kaps_uda_mgr *mgr,
    void *handle,
    uint32_t rqt_num_lpus,
    uint32_t region_id,
    uint32_t offset,
    uint32_t lsn_id,
    struct uda_mem_chunk **alloc_chunk_pp);

/**
 * Allocates the chunk of LPUs of requested size and location in the
 * UDA partition during warmboot
 *
 * @param mgr_p valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param rqt_num_lpus Size (number of LPUs) of the free chunk to be allocated
 * @param device valid device handle
 * @param chunk contains pointer to the allocated chunk. In case allocation is
 *              unsuccessful, a NULL pointer is returned
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_uda_mgr_alloc(
    struct kaps_uda_mgr *mgr_p,
    int32_t rqt_num_lpus,
    struct kaps_device *device,
    void *handle,
    struct uda_mem_chunk **chunk,
    struct kaps_db *db);

/**
 * Returns the UDA chunk from a brick no
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param udc_no UDC no
 * @param udm_no UDM no
 * @param brick_no brick no
 *
 * @return UDA chunk on success
 */
    struct uda_mem_chunk *kaps_uda_mgr_get_chunk_from_uda_addr(
    struct kaps_uda_mgr *mgr,
    int32_t udc_no,
    int32_t udm_no,
    int32_t brick_no);

/**
 * Returns the DB pointer which has been allocated a given UDA brick
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param udc_no UDC no
 * @param udm_no UDM no
 * @param brick_no brick no
 *
 * @return db pointer on success
 */
    struct kaps_db *kaps_uda_mgr_get_db_from_uda_addr(
    struct kaps_uda_mgr *mgr,
    int32_t udc_no,
    int32_t udm_no,
    int32_t brick_no);

/**
 * Checks if a grow up operation is possible and returns the
 * number of LPU bricks by which we can grow up
 * Grow up involves acquiring the preceding LPU bricks
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param cur_uda_chunk Allocated UDA chunk which needs to grow
 * @param grow_up_size_p Number of LPU bricks by which we can grow up is returned here
 *
 * @return KAPS_OK on success
 */
    kaps_status kaps_uda_mgr_check_grow_up(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t * grow_up_size_p);

/**
 * Perform the Grow up operation by acquiring the preceding LPU bricks.
 * The kaps_uda_mgr_check_grow_up should be called
 * before calling this function to make sure that grow up can occur
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param cur_uda_chunk Allocated UDA chunk which needs to grow
 * @param grow_up_size Number of LPU bricks by which to grow up
 *
 * @return KAPS_OK on success
 */
    kaps_status kaps_kaps_uda_mgr_grow_up(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t grow_up_size);

/**
 * Perform the Shrink up operation by releasing the given no of
 * LPU bricks from the start of the current chunk.
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param cur_uda_chunk Allocated UDA chunk which needs to shrink
 * @param shrink_up_size Number of LPU bricks by which to shrink up
 *
 * @return KAPS_OK on success
 */
    kaps_status kaps_uda_mgr_shrink_up(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t shrink_up_size);

/**
 * Checks if a grow down operation is possible and returns the
 * number of LPU bricks by which we can grow down
 * Grow down involves acquiring the next set of LPU bricks after the current chunk
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param cur_uda_chunk Allocated UDA chunk which needs to grow
 * @param grow_down_size_p Number of LPU bricks by which we can grow down
 *        is returned here
 *
 * @return KAPS_OK on success
 */
    kaps_status kaps_uda_mgr_check_grow_down(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t * grow_down_size_p);

/**
 * Perform the Grow down operation by acquiring the next set of
 * LPU bricks after the current chunk.
 * The kaps_uda_mgr_check_grow_up should be called before calling this
 * function to make sure that grow up can occur
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param cur_uda_chunk Allocated UDA chunk which needs to grow
 * @param grow_down_size Number of LPU bricks by which to grow down
 *
 * @return KAPS_OK on success
 */
    kaps_status kaps_kaps_uda_mgr_grow_down(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t grow_down_size);

/**
 * Perform the Shrink down operation by releasing the given no of
 * LPU bricks from the end of the current chunk.
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param cur_uda_chunk Allocated UDA chunk which needs to shrink
 * @param shrink_down_size Number of LPU bricks by which to shrink down
 *
 * @return KAPS_OK on success
 */
    kaps_status kaps_uda_mgr_shrink_down(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t shrink_down_size);

/**
 * Combines the above four functions and tries to grow requested
 * number of bricks
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param cur_uda_chunk Allocated UDA chunk which needs to grow
 * @param num_bricks_to_grow Number of LPU bricks by which to grow
 * @param num_bricks_grown_p returns the number of LPU bricks by which the current chunk grew
 * @param flag_grown_up_p returns the direction of the grow, 1 for up, and 0 for down
 *
 * @return KAPS_OK on success
 */

    kaps_status kaps_uda_mgr_grow(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *cur_uda_chunk,
    uint32_t num_bricks_to_grow,
    uint32_t * num_bricks_grown_p,
    int8_t * flag_grown_up_p);

/**
 * Free an allocated chunk of LPUs and returns it to the UDA Memory Manager
 *
 * @param mgr_p valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param chunk pointer to the chunk that should be freed
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_uda_mgr_free(
    struct kaps_uda_mgr *mgr_p,
    struct uda_mem_chunk *chunk);

/**
 * Destroy the UDA Memory Manager and free up the heap memory
 *
 * @param mgr_p valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param device valid device handle
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_uda_mgr_destroy(
    struct kaps_uda_mgr *mgr_p);

/**
 * Create the freelist and neighbour list structures of the UDA mgr during warmboot
 *
 * @param mgr_p valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_uda_mgr_wb_finalize(
    struct kaps_allocator *alloc,
    struct kaps_uda_mgr *mgr);

/**
 * Create the UDA regions back during warmboot
 *
 * @param mgr valid UDA Memory Manager
 * @param alloc valid allocator
 * @param no_of_regions number of regions
 * @param region_info region info
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_uda_mgr_wb_restore_regions(
    struct kaps_uda_mgr *mgr,
    struct kaps_allocator *alloc,
    int32_t no_of_regions,
    struct region region_info[]);

/**
 * Calculates and returns the UDA memory manager stats
 *
 * @param mgr_p valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param uda_stats valid memory for storing the statistics
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_uda_mgr_calc_stats(
    const struct kaps_uda_mgr *mgr_p,
    struct uda_mgr_stats *uda_stats);

/**
 * Pretty print the UDA Memory Manager stats
 *
 * @param uda_stats valid stats initialized using kaps_uda_mgr_calc_stats()
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_uda_mgr_print_stats(
    struct uda_mgr_stats *uda_stats);

/**
 * Calculates and prints the UDA memory manager stats to the chunk level of details
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_uda_mgr_print_detailed_stats(
    const struct kaps_uda_mgr *mgr);

/**
 * Calculates the usage level of the UDA managed by this Memory Manager
 *
 * @param mgr_p pointer to the initialized UDA Manager
 * @param usage is 1.0 if the UDA Memory Manager is full, 0.0 if
 *        it is empty, in between if it is partially full
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_uda_mgr_usage(
    const struct kaps_uda_mgr *mgr_p,
    double *usage);

/**
 * Verify the UDA Memory manager data structures
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_uda_mgr_verify(
    struct kaps_uda_mgr *mgr);

/**
 * Compacts the UDA region
 * @param mgr UDA manager pointer
 * @param region_id region id to compact
*/

    kaps_status kaps_uda_mgr_compact(
    struct kaps_uda_mgr *mgr,
    int32_t region_id,
    int32_t new_region_id);

/**
 * Compacts all the UDA regions using the above function
 * @param mgr UDA manager pointer
 * @param region_id region id to compact
*/
    kaps_status kaps_kaps_uda_mgr_compact_all_regions(
    struct kaps_uda_mgr *mgr);

/**
 * Releases all the regions of UDA manager back to resource manager
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_uda_mgr_release_all_regions(
    struct kaps_uda_mgr *mgr);

/**
 * Releases only the region which has been allocated last,
 * this region is not useful for algo
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_uda_mgr_release_last_allocated_regions(
    struct kaps_uda_mgr *mgr);

/**
 * Enables the dynamic allocation in UDA Mgr
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_uda_mgr_enable_dynamic_allocation(
    struct kaps_uda_mgr *mgr);

/**
 * Disables the dynamic allocation in UDA Mgr
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_uda_mgr_disble_dynamic_allocation(
    struct kaps_uda_mgr *mgr);

/**
 * dynamically allocates new UDA region from resource manager
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 * @param num_udms_to_fit number of UDMs needed in a region
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_uda_mgr_dynamic_uda_alloc_n_lpu(
    struct kaps_uda_mgr *mgr,
    int32_t num_udms_to_fit);

/**
 * Return the number of free bricks in the UDA manager
 *
 * @param mgr valid UDA Memory Manager initialized using kaps_uda_mgr_init()
 *
 * @return Number of free bricks
 */
    uint32_t kaps_uda_mgr_get_num_free_bricks(
    struct kaps_uda_mgr *mgr);

    void kaps_uda_mgr_add_to_free_list(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *cur_chunk);
    void kaps_uda_mgr_add_to_neighbor_list(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *prev_chunk,
    struct uda_mem_chunk *cur_chunk);

/**
 * @endcond
 */

    kaps_status kaps_uda_mgr_release_udm_sel_table_index(
    struct kaps_device *device,
    uint32_t device_udm_sel_table_index);
    uint32_t kaps_uda_mgr_get_device_udm_sel_table_index(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk);
    uint32_t kaps_uda_mgr_compute_brick_udc(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk,
    int32_t brick_no);
    uint32_t kaps_uda_mgr_compute_brick_row(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk,
    int32_t brick_no);
    kaps_status kaps_uda_mgr_compute_abs_brick_udc_and_row(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk,
    int32_t brick_no,
    uint32_t * abs_udc,
    uint32_t * abs_row);
    uint32_t kaps_kaps_uda_mgr_compute_brick_row_relative(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk,
    int32_t brick_no);
    uint32_t kaps_uda_mgr_compute_brick_udm(
    struct kaps_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk,
    int32_t brick_no);
    kaps_status kaps_uda_mgr_get_no_of_contigous_bricks(
    struct kaps_uda_mgr *mgr,
    int32_t region_id,
    int32_t offset);
    uint32_t kaps_uda_mgr_max_region_width(
    struct kaps_uda_mgr *mgr);
    uint32_t kaps_kaps_uda_mgr_compute_brick_udc_from_region_id(
    struct kaps_uda_mgr *mgr,
    int32_t region_id,
    int32_t offset,
    int32_t brick_no);
    kaps_status kaps_kaps_uda_mgr_compute_brick_udc_and_row_from_region_id(
    struct kaps_uda_mgr *mgr,
    int32_t region_id,
    int32_t chunk_offset,
    int32_t brick_no,
    uint32_t * p_abs_lpu,
    uint32_t * p_abs_row);
    kaps_status kaps_uda_mgr_get_relative_params_from_abs(
    struct kaps_uda_mgr *mgr,
    uint32_t lpu_no,
    uint32_t row_no,
    uint32_t * p_region_id,
    uint32_t * p_offset);

#ifdef __cplusplus
}
#endif
#endif /*__UDA_MGR_H*/
