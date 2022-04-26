 /*
  **************************************************************************************
  Copyright 2015-2019 Broadcom Corporation

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

#ifndef __KAPS_HB_H
#define __KAPS_HB_H

 /**
  * @file kaps_hb.h
  *
  * Hit Bit implementation
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
  * * @addtogroup HB_API
  * @{
  */

 /**
  * Opaque Hit Bit database handle.
  */

 struct kaps_hb_db;

 /**
  * Opaque HB handle.
  */

 struct kaps_hb;

 /**
  * Creates a hit bit database
  *
  * @param device Valid device handle.
  * @param id Database ID. This is a control-plane identifier only.
  * @param capacity The number of hit bits managed by this database. Zero means dynamic capacity.
           Dynamic capacity is not supported at this time.
  * @param hb_dbp Database handle, initialized and returned on success.
  *
  * @returns KAPS_OK on success or an error code otherwise.
  */

 kaps_status kaps_hb_db_init(struct kaps_device *device, uint32_t id, uint32_t capacity,
                           struct kaps_hb_db **hb_dbp);

 /**
  * Destroys the hit bit database.
  *
  * @param hb_db Valid database handle to destroy.
  *
  * @returns KAPS_OK on success or an error code otherwise.
  */

 kaps_status kaps_hb_db_destroy(struct kaps_hb_db *hb_db);

 /**
  * Sets properties for the hit bit database.
  *
  * @param hb_db Valid database handle.
  * @param property The property to set, defined by ::kaps_db_properties.
  * @param ... Variable arguments to set properties.
  *
  * @return KAPS_OK on success or an error code otherwise.
  */

 kaps_status kaps_hb_db_set_property(struct kaps_hb_db *hb_db, enum kaps_db_properties property, ...);

 /**
  * Gets properties for the hit bit database.
  *
  * @param hb_db valid database handle
  * @param property The property to get defined by ::kaps_db_properties
  * @param ... variable arguments to get properties into
  *
  * @return KAPS_OK on success or an error code otherwise.
  */

 kaps_status kaps_hb_db_get_property(struct kaps_hb_db *hb_db, enum kaps_db_properties property, ...);

 /**
  * Creates a hit bit entry.
  *
  * @param hb_db Valid database handle.
  * @param hb Hit bit handle returned on success. Can be used to associate with LPM database entries.
  *
  * @return KAPS_OK on success or an error code otherwise.
  */

 kaps_status kaps_hb_db_add_entry(struct kaps_hb_db *hb_db, struct kaps_hb **hb);

 /**
  * Deletes given hit bit entry. If any database entries continue to point to this
  * hit bit entry, the operation will fail.
  *
  * @param hb_db Valid database handle.
  * @param hb Hit bit handle to delete.
  *
  * @return KAPS_OK on success or an error code otherwise.
  */

 kaps_status kaps_hb_db_delete_entry(struct kaps_hb_db *hb_db, struct kaps_hb *hb);

 /**
  * Deletes all hit bit entries in the database. Assumes there are
  * no live references to the hit bits present.
  *
  * @param hb_db Valid database handle.
  *
  * @return KAPS_OK on success or an error code otherwise.
  */

 kaps_status kaps_hb_db_delete_all_entries(struct kaps_hb_db *hb_db);

 /**
   * Deletes unused hit bit entries in the database.
   *
   * @param hb_db Valid database handle.
   *
   * @return KAPS_OK on success or an error code otherwise.
   */
 kaps_status kaps_hb_db_delete_unused_entries(struct kaps_hb_db *hb_db);

 /**
  * Performs a timer tick on the hit bit database. The timer operations
  * allow software to collect the hit bit information for all the
  * entries active in the database. No entries are aged or impacted,
  * the timer is used as a signal to count unreached entries for
  * aging subsequently.
  *
  * @param hb_db Valid Hit bit database handle
  *
  * @return KAPS_OK on success or an error code
  */
 kaps_status kaps_hb_db_timer(struct kaps_hb_db *hb_db);


/**
  * Returns the list of entries that are aged based on the age count specified.
  * The API fills the provided pre-allocated entry buffer with aged entries up to a
  * max of buf_size. The actual number of entries returned is filled in num_entries.
  *
  * @param hb_db Valid hit bit database handle
  * @param buf_size Size of the entry handle buffer provided
  * @param num_entries Returns the number of entries that are filled
  * @param entries Buffer to fill the aged entry handles
  *
  * @return KAPS_OK on success or an error code
  */

kaps_status kaps_db_get_aged_entries(struct kaps_hb_db *hb_db, uint32_t buf_size, uint32_t *num_entries,
                                   struct kaps_entry **entries);


/**
 * Returns the new DB Handle after ISSU operation.
 *
 *
 * @param device Valid KBP device handle
 * @param stale_ptr DB handle before ISSU
 * @param dbp New DB handle
 *
 * @retval KAPS_OK On success and result structure is populated.
 * @retval KAPS_INVALID_ARGUMENT for invalid or null parameters.
 */
kaps_status kaps_hb_db_refresh_handle(struct kaps_device *device, struct kaps_hb_db *stale_ptr, struct kaps_hb_db **dbp);



/**
 * @brief Opaque entry iterator handle.
 */

struct kaps_aged_entry_iter;

/**
 * Creates an iterator to iterate over all aged entries in the database.
 * Currently entries cannot be deleted when iterating over them.
 *
 * @param hb_db Valid hit bit database handle.
 * @param iter Iterator initialized and returned on success.
 *
 * @retval KAPS_OK On success.
 * @retval KAPS_OUT_OF_MEMORY If out of memory.
 */

kaps_status kaps_hb_db_aged_entry_iter_init(struct kaps_hb_db *hb_db, struct kaps_aged_entry_iter **iter);

/**
 * Gets the next aged entry in the database, or NULL if the last entry.
 *
 * @param hb_db Valid hit bit database handle.
 * @param iter Valid initialized iterator.
 * @param entry A non-NULL valid entry handle returned on success, or NULL after the last entry.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_hb_db_aged_entry_iter_next(struct kaps_hb_db *hb_db, struct kaps_aged_entry_iter *iter, struct kaps_entry **entry);

/**
 * Reclaims the iterator resources.
 *
 * @param hb_db Valid hit bit database handle.
 * @param iter Valid initialized iterator.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_hb_db_aged_entry_iter_destroy(struct kaps_hb_db *hb_db, struct kaps_aged_entry_iter *iter);


/**
 * Gets the idle count for the hit bit
 *
 * @param hb_db Valid hit bit database handle.
 * @param hb Hit bit entry for which we need the idle count.
 * @param idle_count The idle count for the entry is returned here.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_hb_entry_get_idle_count(struct kaps_hb_db *hb_db, struct kaps_hb *hb, uint32_t *idle_count);

/**
 * Sets the idle count for the hit bit
 *
 * @param hb_db Valid hit bit database handle.
 * @param hb Hit bit entry for which we need to set the idle count.
 * @param idle_count The idle count which should be set.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_hb_entry_set_idle_count(struct kaps_hb_db *hb_db, struct kaps_hb *hb, uint32_t idle_count);


/**
 * Gets the bit value for the hit bit.
 *
 * @param hb_db Valid hit bit database handle.
 * @param hb_handle Hit bit entry for which we need the bit value.
 * @param bit_value Value of the hit bit for the entry is returned here.
 * @param clear_on_read If 1, the hit bit will be cleared in hardware.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_hb_entry_get_bit_value(struct kaps_hb_db *hb_db, struct kaps_hb *hb_handle, uint32_t *bit_value, uint8_t clear_on_read);



/**
 * @}
 */


#ifdef __cplusplus
 }
#endif
#endif /*__KAPS_HB_H */

