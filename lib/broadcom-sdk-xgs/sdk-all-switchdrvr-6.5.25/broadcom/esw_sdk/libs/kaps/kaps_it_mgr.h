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

#ifndef __KAPS_IT_MGR_H
#define __KAPS_IT_MGR_H

#include "kaps_errors.h"
#include "kaps_allocator.h"
#include "kaps_hw_limits.h"
#include "kaps_portable.h"
#include "kaps_device_alg.h"
#include "kaps_ab.h"
#include "kaps_instruction_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AD_IN_PLACE_IN_UIT (0x1000000)

/**
 * @brief Internal structure to bookkeep IT information
 */

    struct it_bank_info
    {
        uint8_t type;       /**< ::kaps_entry_status */
        uint8_t instance_id;/**<::it_instance_id */
        uint8_t bank_no;    /**< The UIT bank number */
        uint8_t nab;        /**< Number of ABs currently sharing the bank */
        uint8_t is_used_bmp;/**< Each bit represents that subportion of UIT bank in use*/
        uint8_t ab_conf;     /**< ab_conf used for this bank */
        uint32_t ab_width_8;/**< The width of the Array Blocks using this UIT bank*/
        struct kaps_db *db;  /**< db assigned to this bank*/
    };

/**
 * @brief the structure for returning the IT stats
 */
    struct it_mgr_stats
    {
        uint32_t num_used_uit_banks;/**< Number of used UIT banks (valid only for OP)*/
    };

    enum it_instance_id
    {
        IT_INSTANCE_0,
        IT_INSTANCE_1
    };

/**
 * @brief Internal structure to bookkeep IT information for KAPS
 */
    struct kaps_it_mgr
    {
        struct it_bank_info bank[KAPS_MAX_NUM_ADS_BLOCKS];
                                                        /**< KAPS banks */
    };

/**
 * @brief Internal structure for managing the IT space in different devices
 */
    struct it_mgr
    {
        struct kaps_allocator *alloc;/**< handle to the memory allocator*/
        struct kaps_device *device;  /**< device handle */

        union
        {
            struct kaps_it_mgr kaps;/**< KAPS IT manager*/
        } u;
    };

/**
 * Initialize the Information Table Manager
 *
 * @param device valid device handle
 * @param alloc allocator to use for allocations
 * @param mgr_pp contains the pointer to the initialized IT Manager
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_it_mgr_init(
    struct kaps_device *device,
    struct kaps_allocator *alloc,
    struct it_mgr **mgr_pp);

/**
 * Allocates a contiguous space in the Information Table. The IT
 * size is calculated based on the requesting array block width
 *
 * @param self valid information table manager handle
 * @param db valid database handle
 * @param type type of IT space requested
 * @param instance_id if the db is cloned, then two copied of IT will have different instance id
 * @param ab_width_in_bytes the width of the array block that is requesting the IT space
 * @param ab The AB for which the IT space is being requested
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_it_mgr_alloc(
    struct it_mgr *self,
    struct kaps_db *db,
    enum kaps_entry_status type,
    enum it_instance_id instance_id,
    uint32_t ab_width_in_bytes,
    struct kaps_ab_info *ab);

/**
 * Frees a contiguous space in the Information Table. The IT
 * size is calculated based on the requesting array block width
 *
 * @param self valid information table manager handle
 * @param db valid database pointer
 * @param ab The AB from which the IT space is being freed
 * @param ab_width_in_bytes the width of the array block that is freeing the IT space
 * @param type type of the entries (TRIE/PC_OVERFLOW/OVERFLOW)
 *
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_it_mgr_free(
    struct it_mgr *self,
    struct kaps_db *db,
    struct kaps_ab_info *ab,
    uint32_t ab_width_in_bytes,
    enum kaps_entry_status type);

/**
 * Destroys the Information Table Manager and frees up the dynamic memory
 *
 * @param self valid information table manager handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_it_mgr_destroy(
    struct it_mgr *self);

/**
 * Allocates a space in the Information Table at a specified offset. The IT
 * size is calculated based on the requesting array block width
 *
 * @param self valid information table manager handle
 * @param db valid database handle
 * @param type type of IT space requested
 * @param ab_width_in_bytes the width of the array block that is requesting the IT space
 * @param The AB for which the IT space is being requested
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_it_mgr_wb_alloc(
    struct it_mgr *self,
    struct kaps_db *db,
    enum kaps_entry_status type,
    uint32_t ab_width_in_bytes,
    struct kaps_ab_info *ab);

#ifdef __cplusplus
}
#endif

#endif /*__IT_MGR_H*/
