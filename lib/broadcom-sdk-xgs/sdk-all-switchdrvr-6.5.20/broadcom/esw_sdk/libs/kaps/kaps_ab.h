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

#ifndef __KAPS_AB_H
#define __KAPS_AB_H

#include "kaps_device_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @cond INTERNAL
 */

/**
 * @file kaps_ab.h
 *
 * Internal details and structures used for writing to the TCAM Array Block
 *
 */

/**
 * @addtogroup AB
 * @{
 */

    enum kaps_ab_confs
    {
        KAPS_NORMAL_80 = 0,
                          /**< AB of normal physical shape configured as 4K 80-wide entries */
        KAPS_NORMAL_160 = 1
                         /**< AB of normal physical shape configured as 2K 160-wide entries */
    };

/**
 * Information on a single AB that is part of the
 * decision tree/overflow
 */

    struct kaps_ab_info
    {

        struct kaps_list_node ab_node;
                                    /**< To chain ABs to decision trees */
        uint32_t ab_num:11;     /**< The physical AB number */
        uint32_t num_slots:14; /**< num slots in this AB */
        uint32_t conf:4;       /**< AB config Type ::op_ab_confs */
        uint32_t blk_cleared:1;/**< To optimize block clear */
        uint32_t translate:1;  /**< Should index be based of base address */
        uint32_t base_addr;    /**< The AB config base address to use */
        uint32_t base_idx;     /**< The AB config base index to use */
        uint32_t ab_init_done:1; /**< Field indicating if AB is initialized */
        uint32_t is_dup_ab:1;  /**< Indicates if its a duplicate AB */
        uint32_t is_mapped_ab:1;
                               /**< this AB is mapped to another region*/

        struct kaps_db *db;     /**< The database the block has been assigned to */
        struct kaps_ab_info *dup_ab;
                                 /**< Points to the AB paired to this AB for clone/duplicated data base */
        struct kaps_ab_info *parent;/**< for 640b OP , parent AB . for AB4 , AB0 is parent AB */

        struct kaps_device *device;
                                /**< The device to which this AB belongs */

    };

#define KAPS_ABLIST_TO_ABINFO(ptr) CONTAINER_OF((ptr), struct kaps_ab_info, ab_node)

/**
 * @}
 */

/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif
#endif
