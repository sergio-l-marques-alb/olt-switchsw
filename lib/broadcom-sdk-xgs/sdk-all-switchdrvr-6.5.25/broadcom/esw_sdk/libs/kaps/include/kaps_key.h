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

#ifndef __KAPS_KEY_H
#define __KAPS_KEY_H

/**
 * @file kaps_key.h
 *
 * This module helps define the key layout for databases and instructions.
 * This forms the basis for extraction of subkeys from master keys specified
 * as part of instructions. This allows the control-plane API
 * to program the key-processing units (KPUS) in hardware.
 */

#include <stdint.h>

#include "kaps_errors.h"
#include "kaps_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup KEY_API
 * @{
 */

/**
 * Opaque key handle
 */

struct kaps_key;

/**
 * The type of the key field. This helps
 * the control plane identify the type of a
 * field. This information is mainly used for
 * programming the KPUs and effective range
 * handling in hardware.
 */

enum kaps_key_field_type {
    KAPS_KEY_FIELD_TERNARY,/**< Arbitrary data/mask field with don't cares. */
    KAPS_KEY_FIELD_PREFIX, /**< An IPv4 or IPv6 address prefix. */
    KAPS_KEY_FIELD_EM,     /**< Exact-match field that does not have don't cares (for example, Table ID). */
    KAPS_KEY_FIELD_RANGE,  /**< 16-bit range field that can be compared as >= or <= (for example, ports). */
    KAPS_KEY_FIELD_TABLE_ID, /**< Equivalent to exact match, identifies the table identifier
                               tuple when the database is a collection of multiple tables. */
    KAPS_KEY_FIELD_HOLE,   /**< Arbitrary number of bytes in the key, which are zero-filled.
                             It is assumed the entry in DBA will not compare this portion of the key.
                             This can be used to express BMR functionality */
    /* This should be the last entry */
    KAPS_KEY_FIELD_DUMMY_FILL, /**< Ignores the field in the key specified for instructions only */
    KAPS_KEY_FIELD_INVALID
};

/**
 * Creates a new key, which will be used to define the layout of entries.
 *
 * @param device Valid KBP device handle.
 * @param key The initialized key, returned on success.
 *
 * @retval KAPS_OUT_OF_MEMORY when the key cannot be allocated.
 * @retval KAPS_INVALID_ARGUMENT if the arguments are NULL or invalid.
 * @retval KAPS_OK on success.
 */

kaps_status kaps_key_init(struct kaps_device *device, struct kaps_key **key);

/**
 * Adds a field to the key. Repeated calls to add field
 * help to construct the key. The field names provided are
 * not interpreted in any way. However,  to program the KPU correctly, the field names
 * in the databases must match the field names in the instruction
 * key. The field width must be a multiple of eight bits.
 *
 * @param key Valid key handle.
 * @param name Field name.
 * @param width_1 Width of the field in bits. Must be multiple of eight bits.
 * @param type The type of the field defined by ::kaps_key_field_type.
 *
 * @retval KAPS_OUT_OF_MEMORY when the key field cannot be allocated.
 * @retval KAPS_INVALID_ARGUMENT if the arguments are NULL or invalid.
 * @retval KAPS_OK on success.
 */

kaps_status kaps_key_add_field(struct kaps_key *key, char *name, uint32_t width_1, enum kaps_key_field_type type);

/**
 * This API allows user to add overlay key field on the master key.
 * Repeated calls to this API allow to specify multiple overlay key fields.
 * The field names provided are not interpreted in any way. However, to program the KPU
 * correctly, the field names in the databases must match the field names in the instruction key.
 *
 * @param master_key Valid master key handle.
 * @param name Field name.
 * @param width_1 Width of the field in bits. Must be multiple of eight bits.
 * @param type The type of the field defined by ::kaps_key_field_type.
 * @param offset_1 Offset of the field in master key in bits. Must be multiple of eight bits.
 *
 * @retval KAPS_OUT_OF_MEMORY when the key field cannot be allocated.
 * @retval KAPS_INVALID_ARGUMENT if the arguments are NULL or invalid.
 * @retval KAPS_OK on success.
 */
kaps_status kaps_key_overlay_field(struct kaps_key *master_key, char *name, uint32_t width_1, enum kaps_key_field_type type, uint32_t offset_1);




/**
 * This API allows user to specify fields as critical.
 * This is an optional API.
 * Fields that are exected to have least amount of dontcares in the entries can be marked as critical fields.
 * This information helps the SDK to optimize the entry storage
 * This API can be called multiple times for the same key with different fields. The sdk uses the fields in the same order that they are set.
 * for example if this api is used to set "field5" as critical followed by "field3" as critical, then the SDK uses field5 followed by field3
 *
 * @param key Valid key handle.
 * @param name Field name.
 *
 * @retval KAPS_INVALID_ARGUMENT if the arguments are NULL or invalid.
 * @retval KAPS_OK on success.
 */
kaps_status kaps_key_set_critical_field(struct kaps_key *key, char *name);



/**
 * Verifies that the database_key can be derived from the specified master key.
 *
 * @param master_key Valid key handle to the master key.
 * @param db_key Valid key handle to the database key that specifies the layout records.
 * @param error_field Pointer to the name of the field (if any) that causes an error. The memory
 *                    returned must not be freed. Can be NULL if user is not interested in the error field.
 *
 * @retval KAPS_OK if the database key can be derived from the master key.
 * @retval KAPS_KEY_FIELD_MISSING if a field specified in the database key is missing from the master key.
 * @retval KAPS_KEY_GRAN_ERROR if the database key can be derived from the master key only from byte boundaries.
 * @retval KAPS_INVALID_ARGUMENT if the arguments are NULL or invalid.
 */

kaps_status kaps_key_verify(struct kaps_key *master_key, struct kaps_key *db_key, char **error_field);

/**
 * Pretty-print the key information.
 *
 * @param key Valid key handle.
 * @param fp Pointer to the file where the contents are to be dumped.
 *
 * @retval KAPS_INVALID_ARGUMENT if the arguments are NULL or invalid.
 * @retval KAPS_OK on success.
 */

kaps_status kaps_key_print(struct kaps_key *key, FILE * fp);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /*__KEY_H */
