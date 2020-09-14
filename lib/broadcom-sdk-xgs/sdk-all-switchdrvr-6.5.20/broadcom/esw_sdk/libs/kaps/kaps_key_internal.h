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

#ifndef __KAPS_KEY_INTERNAL_H
#define __KAPS_KEY_INTERNAL_H

#include "kaps_key.h"
#include "kaps_db_wb.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @cond INTERNAL
 *
 * @file key_internal.h
 *
 * Structural details of keys/KPUs
 */

/**
 * Number for calculating the ternary field shift heuristic
 */

#define KEY_TERNARY_SIZE_RATIO (0.6)

    struct range_info;
    struct kaps_db;
    struct kaps_device;
    struct kaps_instruction;

/**
 * Table key field format specification
 */
    struct kaps_key_field
    {
        uint16_t offset_1;        /**< Key field bit offset */
        uint16_t width_1;         /**< Key field bit width */
        uint16_t orig_offset_1;   /**< Offset as specified by user, unmanipulated by algorithm */
        uint16_t field_prio;      /**< critical field priority. Fields with lower value has be used for bit selection */
        uint32_t do_not_bmr:1;    /**< If the field type is HOLE, we might still not want to BMR (ex. range encoding hole) */
        uint32_t visited:1;       /**< Bookkeeping */
        uint32_t is_usr_bmr:1;    /**< Valid Only for user specified BMR */
        uint32_t is_padding_field:1;
                                  /**< User added hole for padding */
        struct range_info *rinfo; /**< Valid for ranges only, MCOR & DIRPE related info */
        struct kaps_key_field *p_field;
                                    /**< For clones, the corresponding parent field */
        enum kaps_key_field_type type;
                                   /**< Type of the field */
        struct kaps_key_field *next;
                                   /**< Pointer to the next field in the doubly linked list */
        struct kaps_key_field *prev;
                                   /**< Pointer to the previous field in the doubly linked list */
        char *name;               /**< Key field name */
    };

/**
 * Table Key Specification
 */

    struct kaps_key
    {
        uint32_t width_1:16;                /**< Width of the key in bits */
        uint32_t nranges:3;                 /**< Number of ranges in the key */
        uint32_t has_user_bmr:1;            /**< is user specified BMR available */
        uint32_t ref_count:8;               /**< Number of databases/instructions pointing to same key */
        uint32_t has_dummy_fill_field:1;    /**< does key have dummy fill key field */
        uint16_t critical_field_count;      /**< Number of critical fields in the Key */
        struct kaps_device *device;          /**< Back pointer to device */
        struct kaps_key_field *first_field;  /**< Pointer to the first key field in the current key */
        struct kaps_key_field *last_field;   /**< Pointer to the last key field in the current key */
        struct kaps_key_field *first_overlay_field;
                                                /**< Pointer to the first overlay key field in the master key */
        struct kaps_key_field *last_overlay_field;
                                                /**< Pointer to the last overlay key field in the master key */
    };

/**
 * Table Key Print Structure
 */

    struct kaps_print_key
    {
        char *fname;                    /**< String name for the key. */
        uint32_t width;                 /**< Width of key in bits. */
        enum kaps_key_field_type type;   /**< Type of key field. */
        struct kaps_print_key *next;     /**< Linked list of key fields. */
    };

/**
 * Destroy the key
 *
 * @param key valid key handle
 */

    void kaps_key_destroy_internal(
    struct kaps_key *key);

/**
 * Return printable string representation for type
 *
 * @param type  valid field type ::kaps_key_field_type
 *
 * @retval string
 */

    char *kaps_key_get_type_internal(
    enum kaps_key_field_type type);

/**
 * Move the key field from its existing location to the new offset
 *
 * @param key valid key handle
 * @param field the field to move
 * @param offset_1 the bit offset to move to
 */

    void kaps_key_move_internal(
    struct kaps_key *key,
    struct kaps_key_field *field,
    uint32_t offset_1);

/**
 * Re-adjust the offset information for the key fields
 *
 * @param key valid key handle
 */

    void kaps_key_adjust_offsets(
    struct kaps_key *key);

/**
 * Configure the key construction for the databases in the instruction based on the master key and the database key
 *
 * @param instruction valid instruction handle
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_key_configure_key_construction(
    struct kaps_instruction *instruction);

/**
 * Create a copy of the key
 *
 * @param device valid device handle
 * @param key the key to copy
 * @param result copied key returned on success
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_key_clone(
    struct kaps_device *device,
    struct kaps_key *key,
    struct kaps_key **result);

/**
 * Check for the standard format of key fields
 *
 * @param db valid database handle
 * @param key  valid key handle
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_key_validate_internal(
    struct kaps_db *db,
    struct kaps_key *key);

/**
 * If the parent key fields have been shuffled, this
 * API helps match up the clone's fields
 *
 * @param clone the cloned database key
 * @param parent the parent database key
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_key_match_clone_parent(
    struct kaps_db *clone,
    struct kaps_db *parent);

/**
 * Saves the Key.
 * The key_fields (name, type and width) in the user specified order.
 *
 * @param key the Key to save
 * @param wb_fun warm boot state
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_key_wb_save(
    struct kaps_key *key,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * Restores the Key.
 * The key_fields (name, type and width) are  added back in the user specified order.
 *
 * @param key the Key to save
 * @param wb_fun warm boot state
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_key_wb_restore(
    struct kaps_key *key,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * Reads a key from a ISSU dump file and constructs a kaps_print_key element
 *
 * @param key Pointer to kaps_print_key element
 * @param bin_fp pointer to ISSU dump file
 * @param nv_offset File offset
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_key_wb_read(
    struct kaps_print_key **key,
    FILE * bin_fp,
    uint32_t * nv_offset);

/**
 * Prints a key to the file specified.
 *
 * @param key Pointer to kaps_print_key element
 * @param txt_fp pointer to Output file
 *
 * @return KAPS_OK on success or error code
 */
    void kaps_key_wb_print(
    struct kaps_print_key *key,
    FILE * txt_fp);

/**
 * Prints all user specified fields within a key to the file specified.
 *
 * @param key Pointer to kaps_print_key element
 * @param txt_fp pointer to Output file
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_key_print_internal(
    struct kaps_key *key,
    FILE * fp);

/**
 * Frees the memory of a kaps_print_key
 *
 * @param key Pointer to kaps_print_key element
 * @param txt_fp pointer to Output file
 *
 * @return KAPS_OK on success or error code
 */
    void kaps_key_wb_free(
    struct kaps_print_key *key);

/**
 * Zero the key generated by the KPU. This allows for
 * saving on switching power when the KPU is mostly idle
 * across searches
 *
 * @param device valid device handle
 * @param ltr_num the LTR number
 * @param kpu_no the KPU number
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_key_zero_kpu_key(
    struct kaps_device *device,
    uint32_t ltr_num,
    uint32_t kpu_no);

/**
 * This is an internal API to Adds a field to the key. Repeated calls to add field
 * helps to construct the key. The field names provided are
 * not interpreted in any way. However,  to program the KPU correctly, the field names
 * in the databases must match the field names in the instruction
 * key. The field width must be a multiple of eight bits.
 *
 * @param key Valid key handle.
 * @param name Field name.
 * @param width_1 Width of the field in bits. Must be multiple of eight bits.
 * @param type The type of the field defined by ::kaps_key_field_type.
 * @param is_user_bmr hole is user specified one.
 *
 * @retval KAPS_OUT_OF_MEMORY when the key field cannot be allocated.
 * @retval KAPS_INVALID_ARGUMENT if the arguments are NULL or invalid.
 * @retval KAPS_OK on success.
 */

    kaps_status kaps_key_add_field_internal(
    struct kaps_key *key,
    char *name,
    uint32_t width_1,
    enum kaps_key_field_type type,
    uint32_t is_user_bmr);

/**
 * This API calculates the number of segments required for db in the instruction
 *
 *
 * @param instruction valid instruction handle
 * @param db valid db handle
 * @param nsegments Number of segments
 *.@param last_segment_width Number of bytes in last segment
 * @param last_sement_start_byte Start byte location of the last segment
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_key_get_number_of_segments(
    struct kaps_instruction *instruction,
    struct kaps_db *db,
    uint8_t * nsegments,
    uint8_t * last_segment_width,
    uint8_t * last_segment_start_byte);

/**
 * Configure the key construction for range insertion information
 * for the database based on the master key and the database key
 *
 * @param db valid database handle
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_key_configure_range_insertion(
    struct kaps_db *db);

/**
 * @endcond
 */
#ifdef __cplusplus
}
#endif

#endif /*__KEY_INTERNAL_H */
