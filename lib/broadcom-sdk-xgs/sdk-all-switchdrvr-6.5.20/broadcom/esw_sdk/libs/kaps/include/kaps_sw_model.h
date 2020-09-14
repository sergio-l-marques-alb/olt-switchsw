/*
 **************************************************************************************
 Copyright 2014-2019 Broadcom Corporation

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

#ifndef INCLUDED_KAPS_MODEL_H
#define INCLUDED_KAPS_MODEL_H

#include <stdint.h>

#include "kaps_device.h"
#include "kaps_errors.h"
#include "kaps_init.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Statistics obtained from the C-Model
 */
struct kaps_sw_model_stats
{
    /**< Maximum number of DBA blocks lit by any key during a compare, per LTR */
    uint32_t max_num_dba_blocks_lit_per_key[KAPS_HW_MAX_LTRS][KAPS_HW_MAX_KPUS];
};

/**
 * Model properties that can be set by user
 */
enum kaps_sw_model_property
{
    KAPS_SW_MODEL_DUMP,    /**< Dump PIO writes to file, a valid file pointer must be provided */
    KAPS_SW_MODEL_UNROLL_COMPLEX_INST, /**< Logs the unrolled complex instruction by issuing
                                                                broken up instructions*/
    KAPS_SW_MODEL_TRACE,   /**< Trace print the search results */
    KAPS_SW_MODEL_TRIG_EVENTS,  /** Trigger callbacks for HW write events */
    KAPS_SW_MODEL_INVALID  /**< This should be the last entry */
};


/**
 * @brief Configuration details for the KAPS model
 */
struct kaps_sw_model_kaps_config
{
    uint8_t id; /**< Device id */
    uint8_t sub_type;   /**< Device sub type */
    uint16_t total_num_rpb; /**< Total number of root blocks in the KAPS device*/
    uint16_t total_num_small_bb; /**< Total number of small bucket blocks in KAPS device */
    uint16_t total_num_bb;  /**< Total number of bucket blocks in the KAPS device */
    uint16_t num_rows_in_rpb; /**< Number of rows in one root block */
    uint16_t num_rows_in_small_bb; /**< Number of rows in one small bucket block */
    uint16_t num_rows_in_bb; /**< Number of rows in one bucket block */
    uint16_t profile;        /**< profile in which the device will be used */
};

/**
 * @brief configuration for 12K model
 */
struct kaps_sw_model_12k_config
{
    uint32_t num_devices;     /**< Number of cascaded devices */
    uint32_t dba_mb;          /**< 40 or 80Mb device */
    uint32_t algorithmic_lpm; /**< Algorithmic LPM support on device */
    uint32_t algorithmic_acl; /**< Algorithmic ACL support on device */
};

/**
 * @brief Configuration details for the OP model
 */
struct kaps_sw_model_op2_config
{
    uint32_t dev_type;         /**< 3:0   : 0 - Silicon , Non-zero values between <0x1-0x14> for internal Model testing
                                    7-4   : are internal use [device types]
                                   11-8   : are internal use [small density device types] */
    int32_t num_cores;
};


/**
 * Initialize the software model and return a transport layer
 *
 * @param alloc valid allocator handle
 * @param type the device type to initialize the model for ::kaps_device_type
 * @param flags ::kaps_device_flags OR'd together.
 * @param config has the configuration input details for creating the Model. If NULL, the default config is chosen
 * @param xpt pointer to the transport layer initialized by the function on success
 *
 * @return KAPS_OK on success or an error code
 */

kaps_status kaps_sw_model_init(struct kaps_allocator *alloc, enum kaps_device_type type,
                             uint32_t flags, uint32_t kaps_sub_type, void **xpt);


/**
 * Initialize the software model and return a transport layer
 *
 * @param alloc valid allocator handle
 * @param type the device type to initialize the model for ::kaps_device_type
 * @param flags ::kaps_device_flags OR'd together.
 * @param config has the configuration input details for creating the Model. If NULL, the default config is chosen
 * @param xpt pointer to the transport layer initialized by the function on success
 *
 * @return KAPS_OK on success or an error code
 */

kaps_status kaps_sw_model_init_with_profile(struct kaps_allocator *alloc, enum kaps_device_type type,
                             uint32_t flags, struct kaps_sw_model_kaps_config *config, void **xpt);


/**
 * Destroy the resources allocated by the software model
 *
 * @param xpt valid initialized transport layer handle returned by kaps_sw_model_init()
 *
 * @return KAPS_OK on success or an error code
 */

kaps_status kaps_sw_model_destroy(void *xpt);

/**
 * Obtain the Model Statistics
 *
 * @param xpt valid initialized transport layer handle returned by kaps_sw_model_init()
 * @param stats the model statistics will be  returned here
 *
 * @return KAPS_OK on success or an error code
 */

kaps_status kaps_sw_model_get_stats(void *xpt, struct kaps_sw_model_stats *stats);

/**
 * Set property attributes on the model
 *
 * @param xpt the model handle
 * @param property the model property to set in ::kaps_sw_model_property
 * @param ... the property value
 *
 * @return KAPS_OK on success or an error code
 */

kaps_status kaps_sw_model_set_property(void *xpt, enum kaps_sw_model_property property, ...);

#ifdef __cplusplus
}
#endif
#endif /*__MODEL_H */
