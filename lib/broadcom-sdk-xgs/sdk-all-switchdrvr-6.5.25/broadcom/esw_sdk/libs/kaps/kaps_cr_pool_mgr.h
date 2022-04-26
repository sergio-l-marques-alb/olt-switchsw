/*
 **************************************************************************************
 Copyright 2009-2019 Broadcom Corporation

 This program is the proprietary software of Broadcom Corporation and/or its licensors,
 and may only be used, duplicated, modified or distributed pursuant to the terms and
 conditions of a separate, written license agreement executed between you and
 Broadcom (an "Authorized License").Except as set forth in an Authorized License,
 Broadcom grants no license (express or implied),right to use, or waiver of any kind
 with respect to the Software, and Broadcom expressly reserves all rights in and to
 the Software and all intellectual property rights therein.
 IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 WAY,AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization, constitutes the
    valuable trade secrets of Broadcom, and you shall use all reasonable efforts to
    protect the confidentiality thereof,and to use this information only in connection
    with your use of Broadcom integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH
    ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER
    EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM
    SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
    NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS
    BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES
    WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE
    THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
    OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
    ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 **************************************************************************************
 */

#ifndef __KAPS_CR_POOL_MGR_H
#define __KAPS_CR_POOL_MGR_H

#include "kaps_errors.h"
#include "kaps_device.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @cond INTERNAL
 */

#define KAPS_MAX_NUM_CR_POOLS_PER_DB  (128)

#define KAPS_CR_INVALID_POOL_ID (255)

#define KAPS_CR_LPM_MAX_NUM_ENTRIES_PER_POOL (128 * 1024)

#define KAPS_CR_GET_POOL_MGR(dev, cr_mgr)                                     \
    do {                                                                 \
        uint8_t *nv_device_ptr = dev->nv_ptr;                            \
        nv_device_ptr += dev->nv_size;                                   \
        nv_device_ptr -= sizeof(struct kaps_cr_pool_mgr);                     \
        cr_mgr = (struct kaps_cr_pool_mgr *)nv_device_ptr;                    \
    } while (0);

/**
 * Structure for maintaining the information for a pool
 */

    struct kaps_cr_pool_node
    {
        uint32_t pool_id:8; /**< pool identifier for the pool allocated in the NV memory by the CR pool manager */
        uint32_t in_use:1;  /**< indicates if the pool is currently being used or not */
        uint32_t num_valid_entries:23;
                                   /**< Number of valid user handles that have been associated with the NV memory in this pool*/
        uint32_t start_user_handle;
                                /**< Starting user handle associated with this pool*/
        uint32_t end_user_handle;
                                /**< Ending user handle associated with this pool */
        uint32_t start_nv_offset;
                                /**< Offset in the NV memory of the first entry in the pool */
        uint32_t end_nv_offset; /**< Offset in the NV memory of the last entry in the pool */
    };

/**
 * Structure for maintaining the information for all the pools in the database
 */

    struct kaps_cr_pools_for_db
    {
        struct kaps_db *db;
                         /**< valid database handle */
        struct kaps_cr_pool_node node_info[KAPS_MAX_NUM_CR_POOLS_PER_DB];
                                                                      /**< Information about the pools for the database*/
        uint32_t entry_size_8;
                            /**< Size of the information of each entry stored in the NV memory*/
        uint32_t num_entries_per_pool;
                                   /**< Number of entries in each pool*/
    };

/**
 * Crash pool manager structure
 */

    struct kaps_cr_pool_mgr
    {
        uint16_t num_dbs;
                        /**< Number of databases in the device corresponding to the crash pool manager*/
        uint32_t free_end_nv_offset;
                                 /**< The offset of the end of the free space in the NV memory*/
        struct kaps_cr_pools_for_db *pools_for_db;
                                               /**< Array that stores the information about the pools for each database*/
    };

/**
 * Iterator for traversing the CR pools
 */

    struct kaps_cr_pool_entry_iter
    {
        uint32_t entry_nr;
                        /**< next entry number in the pool while traversing the pool*/
    };

/**
* Create the Crash Pool Manager. The crash pool manager grows backwards from the end of the NV region
*
*
* @param device Valid device handle.
* @param end_nv_ptr  the pointer to the end of the NV region
* @param mgr_pp the pointer to the crash pool manager that was created is returned here
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_init(
    struct kaps_device *device,
    uint8_t * end_nv_ptr,
    struct kaps_cr_pool_mgr **mgr_pp);

/**
* Find the pool corresponding to the user handle. If the pool for the user handle doesn't yet exist then create it
*
*
* @param mgr Valid Crash Pool Manager
* @param db  valid database handle of the entry for which we are trying to find the pool
* @param user_handle user handle of the entry for which we are trying to find the pool
* @param pool_id id of the pool in the crash pool manager that was assigned to the user handle
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_associate_user_handle(
    struct kaps_cr_pool_mgr *mgr,
    struct kaps_db *db,
    uint32_t user_handle,
    uint32_t * pool_id);

/**
* Disassociate the user handle from the pool since the user handle is no longer in use
*
*
* @param mgr Valid Crash Pool Manager
* @param db  valid database handle of the entry which we are disassociating from the pool
* @param user_handle user handle of the entry which we are disassociating from the pool
* @param pool_id id of the pool in the crash pool manager that was assigned to the user handle
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_disassociate_user_handle(
    struct kaps_cr_pool_mgr *mgr,
    struct kaps_db *db,
    uint32_t user_handle);

/**
* Get the number of active slots and the total number of slots in all the pools for a particular database
*
*
* @param mgr Valid Crash Pool Manager
* @param db  valid handle of the database we are interested in
* @param num_active_slots number of active slots in all the pools for the database is returned here
* @param num_total_slots total number of slots in all the pools for the database is returned here
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_get_num_slots(
    struct kaps_cr_pool_mgr *mgr,
    struct kaps_db *db,
    uint32_t * num_active_slots,
    uint32_t * num_total_slots);

/**
* Returns the NV pointer corresponding to a user handle
*
*
* @param mgr Valid Crash Pool Manager
* @param db  valid handle of the database to which the user handle belongs
* @param user_handle user handle for which we have to find the NV pointer
* @param found indicates if we were able to find the NV pointer for the user handle
* @param entry_nv_ptr the pointer to the NV memory corresponding to the user handle is returned here
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_get_entry_nv_ptr(
    struct kaps_cr_pool_mgr *mgr,
    struct kaps_db *db,
    uint32_t user_handle,
    uint8_t * found,
    uint8_t ** entry_nv_ptr);

/**
* Returns the user handle corresponding to the NV pointer
*
*
* @param mgr Valid Crash Pool Manager
* @param db  valid handle of the database to which the user handle belongs
* @param entry_nv_ptr NV pointer for which we have to find the user handle
* @param found indicates if we were able to find the user handle for the NV pointer
* @param user_handle  the user handle corresponding to the NV pointer is returned here
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_get_user_handle(
    struct kaps_cr_pool_mgr *mgr,
    struct kaps_db *db,
    uint8_t * entry_nv_ptr,
    uint8_t * found_p,
    uint32_t * user_handle);

/**
* Returns the user handle based on the given nv_location
*
*
* @param mgr Valid Crash Pool Manager
* @param db  valid handle of the database to which the user handle belongs
* @param nv_location NV location for which we have to find the corresponding user handle
* @param found indicates if we were able to find the user handle for the NV pointer
* @param user_handle  the user handle corresponding to the NV pointer is returned here
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_get_user_handle_from_nv_location(
    struct kaps_cr_pool_mgr *mgr,
    struct kaps_db *db,
    uint32_t nv_location,
    uint8_t * found_p,
    uint32_t * user_handle);

/**
* Initializes the iterator for traversing all the NV pointers for the database
*
*
* @param mgr Valid Crash Pool Manager
* @param db  valid handle of the database which we are interested in
* @param iter pointer to the iterator is returned here
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_iter_init(
    struct kaps_cr_pool_mgr *mgr,
    struct kaps_db *db,
    struct kaps_cr_pool_entry_iter **iter);

/**
* Fetches the next NV pointer while iterating through the NV pointers for the database
*
*
* @param mgr Valid Crash Pool Manager
* @param db  valid handle of the database which we are interested in
* @param iter handle to the valid iterator
* @param entry_nv_ptr the next NV pointer is returned here
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_iter_next(
    struct kaps_cr_pool_mgr *mgr,
    struct kaps_db *db,
    struct kaps_cr_pool_entry_iter *iter,
    uint8_t ** entry_nv_ptr);

/**
* Destroys the iterator for traversing all the NV pointers for the database
*
*
* @param mgr Valid Crash Pool Manager
* @param db  valid handle of the database which we are interested in
* @param iter pointer to the iterator which should be destroyed
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_iter_destroy(
    struct kaps_cr_pool_mgr *mgr,
    struct kaps_db *db,
    struct kaps_cr_pool_entry_iter *iter);

/**
* Refreshes the database handles that are maintained in the crash pool manager for the device since they may have changed due
* to crash recovery/warmboot
*
* @param device Valid device handle
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_refresh_db_handles(
    struct kaps_device *device);

/**
* Checks if a particular number of bytes can be allocated in the NV memory for the crash pool manager of the device
*
*
* @param device Valid device handle
*
* @return 1 if the NV memory manager of the device doesn't have the space to satisfy the request, 0 otherwise
*/
    uint32_t kaps_cr_pool_mgr_is_nv_exhausted(
    struct kaps_device *device,
    uint32_t num_bytes_needed);

/**
* Prints the stats for the crash pool manager in the device
*
*
* @param device Valid device handle
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_cr_pool_mgr_get_stats(
    struct kaps_device *device);

/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif
#endif
