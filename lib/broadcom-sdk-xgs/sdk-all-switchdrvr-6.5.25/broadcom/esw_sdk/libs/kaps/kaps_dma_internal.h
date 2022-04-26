/*******************************************************************************
 *
 * Copyright 2014-2020 Broadcom Corporation
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

#ifndef __KAPS_DMA_INTERNAL_H
#define __KAPS_DMA_INTERNAL_H

#include "kaps_errors.h"
#include "kaps_bitmap.h"
#include "kaps_dma.h"
#include "kaps_db_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Direct Memory Access Database
 *
 */
struct kaps_dma_db {
    struct kaps_db db_info;          /**< Common database information */
    struct kaps_fast_bitmap *alloc_fbmap;/**< bitmap to bookkeep DMA UDA allocation */
    uint32_t dma_tag;               /**< Tag used for this DMA database */
    uint16_t user_width_1;          /**< Width of the Direct Memory Access data in bits */
};

struct kaps_dma_db_wb_info {
    uintptr_t stale_ptr;              /**< Database ptr*/
    uint32_t tid;                     /**< Control plane database identifier (database ID) */
    uint32_t capacity;                /**< Minimum required capacity */
    uint16_t desc;                    /**< User provided description length */
    uint16_t user_width_1;            /**< Width of the Direct Memory Access data in bits */
    uint32_t dma_tag;                 /**< Tag used for this DMA database */
    uint32_t bb_alloc;                /**< User Specified Bucket Block */
    char description[4];              /**< Description of the database */
};

/**
 * Saves the DMA DB information such as DB id, Capacity , DMA Tag
 * to the NV memory using the read/write callbacks provided.
 *
 * @param db Valid database pointer
 * @param wb_fun WB state
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_dma_db_wb_save_info(struct kaps_db *db,
                                   struct kaps_wb_cb_functions *wb_fun);

/**
 * Restores the DMA DB saved to NV memory.
 * This API is responsible for initializing the DB and performing other operations
 * such as setting various properties , setting the DMA Tag on DB while in the ISSU restore phase.
 *
 * @param device Valid device pointer
 * @param wb_fun WB state
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_dma_db_wb_restore_info(struct kaps_device *device,
                                      struct kaps_wb_cb_functions *wb_fun);

#ifdef __cplusplus
}
#endif

#endif


