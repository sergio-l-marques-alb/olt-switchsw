/*
 **************************************************************************************
 Copyright 2012-2019 Broadcom Corporation

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

#ifndef __KAPS_AD_H
#define __KAPS_AD_H

/**
 * @file ad.h
 *
 * Associated data creation and management.
 */

#include <stdint.h>
#include <stdio.h>

#include "kaps_errors.h"
#include "kaps_device.h"
#include "kaps_db.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * * @addtogroup AD_API
 * @{
 */

/**
 * Opaque AD database handle.
 */

struct kaps_ad_db;

/**
 * Opaque AD handle.
 */

struct kaps_ad;

/**
 * Creates an AD database
 *
 * @param device Valid device handle.
 * @param id Database ID. This is a control-plane identifier only.
 * @param capacity Expected maximum capacity for this database. Can be zero, which means unlimited.
 * @param width_1 Width of AD entries in bits, which must be a multiple of eight.
 * @param db Database handle, initialized and returned on success.
 *
 * @returns KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_init(struct kaps_device *device, uint32_t id, uint32_t capacity,
                          uint32_t width_1, struct kaps_ad_db **db);

/**
 * Destroys the AD database
 *
 * @param db Valid AD database handle to destroy.
 *
 * @returns KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_destroy(struct kaps_ad_db *db);

/**
 * Adds a specific hardware resource to the AD database
 *
 * @param db Valid AD database handle.
 * @param resource The resource to be added defined in ::kaps_hw_resource.
 * @param ... Variable resource arguments.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_set_resource(struct kaps_ad_db *db, enum kaps_hw_resource resource, ...);

/**
 * Retrieves the hardware resources allocated for this database
 *
 * @param db Valid AD database handle.
 * @param resource The resource to be queried, defined in ::kaps_hw_resource.
 * @param ... Resource arguments. Must be pointers into which the values will be returned.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_get_resource(struct kaps_ad_db *db, enum kaps_hw_resource resource, ...);

/**
 * Sets properties for the database.
 *
 * @param db Valid AD database handle.
 * @param property The property to set, defined by ::kaps_db_properties.
 * @param ... Variable arguments to set properties.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_set_property(struct kaps_ad_db *db, enum kaps_db_properties property, ...);

/**
 * Gets properties for the database.
 *
 * @param db valid AD database handle
 * @param property The property to get defined by ::kaps_db_properties
 * @param ... variable arguments to get properties into
 *
 * @return KAPS_OK on success or an error code
 */

kaps_status kaps_ad_db_get_property(struct kaps_ad_db *db, enum kaps_db_properties property, ...);

/**
 * Adds the AD entry. The value array must point to valid memory of the width of the AD database.
 *
 * @param db Valid AD database handle.
 * @param value Associated data content.
 * @param ad Associated data handle returned on success. Can be used to associate with ACL, LPM and EM database entries.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_add_entry(struct kaps_ad_db *db, uint8_t * value, struct kaps_ad **ad);

/**
 * Deletes an AD entry. If any database entries continue to point to the
 * AD, the operation will fail.
 *
 * @param db Valid AD database handle.
 * @param ad Associated data handle to delete.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_delete_entry(struct kaps_ad_db *db, struct kaps_ad *ad);

/**
 * Updates an AD entry.
 *
 * @param db Valid AD database handle.
 * @param ad Associated data handle to update.
 * @param value The new AD contents.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_update_entry(struct kaps_ad_db *db, struct kaps_ad *ad, uint8_t * value);

/**
 * Looks up the AD contents for the specified handle.
 *
 * @param db Valid AD database handle.
 * @param ad Associated-data handle.
 * @param value Array into which the AD contents are copied. There must be enough valid memory to contain the AD data.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_get(struct kaps_ad_db *db, struct kaps_ad *ad, uint8_t * value);

/**
 * Pretty-prints the AD entry.
 *
 * @param db Valid AD database handle.
 * @param ad Associated-data handle.
 * @param fp File pointer.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_entry_print(struct kaps_ad_db *db, struct kaps_ad *ad, FILE * fp);

/**
 * Deletes all AD entries in the database. Assumes there are
 * no live references to the AD present
 *
 * @param db Valid AD database handle.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_delete_all_entries(struct kaps_ad_db *db);


/**
 * Deletes all AD entries in the database which are not being used
 * and there are no live references to those. AD entries created newly
 * and not associated with any entry will also be deleted.
 *
 * @param db Valid AD database handle.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_ad_db_delete_unused_entries(struct kaps_ad_db *db);


/**
 * @}
 */

/**
 * @addtogroup ISSU_API
 * @{
 */

/**
 * Returns the new AD DB Handle after ISSU operation
 *
 *
 * @param device the KBP device handle
 * @param stale_ptr DB handle before ISSU.
 * @param dbp New DB handle.
 *
 * @retval KAPS_OK on success and result structure is populated.
 * @retval KAPS_INVALID_ARGUMENT for invalid or null parameters.
 */

kaps_status kaps_ad_db_refresh_handle(struct kaps_device *device, struct kaps_ad_db *stale_ptr, struct kaps_ad_db **dbp);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /*__AD_H */
