/*! \file src/bcm/dnx/kbp/kbp_mngr_internal.h
 *
 * Internal DNX KBP APIs
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _KBP_MNGR_INTERNAL_H_INCLUDED__
/*
 * {
 */
#define _KBP_MNGR_INTERNAL_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/dbal/dbal.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_KBP_DUMMY_HOLE_DB_ID 0
#define DNX_KBP_KEY_SEGMENT_NOT_CONFIGURED "NOT_CONFIGURED"
#define DNX_KBP_OPCODES_XML_FILE_PATH   "kbp/auto_generated/standard_1/standard_1_kbp_opcodes_definition.xml"

/** Every master key must be a multiplier of 80 bits (10 bytes).*/
#define DNX_KBP_MASTER_KEY_BYTE_GRANULARITY   (10)
/** The minimum size result can have in bytes..*/
#define DNX_KBP_RESULT_SIZE_MIN_BYTES         (1)

/** this assumes that there are 32 cotexts that are reserved to FWD and 32 reserved for ACL. the first ACL ID context is
 *  32. this will allow support ISSU (we keeping some context for FWD and some for ACLs */
#define DNX_KBP_ACL_CONTEXT_BASE	          (32)
#define DNX_KBP_NOF_FWD_CONTEXTS	          (64)  /** NOF FWD2 contexts*/

/** keeping 32 opcodes for ARCH this will allow support ISSU (we keeping some context for FWD and some for ACLs */
#define DNX_KBP_DYNAMIC_OPCODE_BASE	          (32)


#define DNX_KBP_ENTRY_PARSE_FWD_ONLY          (1)
/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */
typedef enum
{
    DNX_KBP_DB_TABLE_INDICATION,
    DNX_KBP_OPCODE_TABLE_INDICATION,
    DNX_KBP_NOF_SW_TABLE_INDICATION
} dnx_kbp_sw_table_indication_e;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

shr_error_e kbp_mngr_opcode_lookup_set(
    int unit,
    uint8 opcode_id,
    uint8 lookup_id,
    uint8 lookup_type,
    uint8 lookup_db,
    uint8 result_id,
    uint8 nof_key_segments,
    uint8 key_segment_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP]);

shr_error_e kbp_mngr_opcode_result_set(
    int unit,
    uint8 opcode_id,
    uint8 result_id,
    uint8 offset,
    uint8 size);

shr_error_e kbp_mngr_key_segment_set(
    int unit,
    uint32 entry_handle_id,
    dnx_kbp_sw_table_indication_e table_indication,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES]);
/**
 * See kbp_mngr_db_create() in kbp_mngr.h
 */
shr_error_e kbp_mngr_db_create_internal(
    int unit,
    dbal_tables_e table_id,
    uint8 clone_id,
    uint8 *db_id);

/**
 * See kbp_mngr_db_id_by_name_get() in kbp_mngr.h
 */
shr_error_e kbp_mngr_db_id_by_name_get_internal(
    int unit,
    char *name,
    uint8 *db_id);

shr_error_e kbp_mngr_opcode_printable_entry_result_parsing(
    int unit,
    uint32 flags,
    uint8 opcode_id,
    uint32 *res_sig_value,
    int res_sig_size,
    uint8 *kbp_entry_print_num,
    kbp_printable_entry_t * kbp_entry_print_info);

shr_error_e kbp_mngr_opcode_printable_entry_key_parsing(
    int unit,
    uint8 opcode_id,
    uint32 *key_sig_value,
    int key_sig_size,
    kbp_printable_entry_t * kbp_entry_print_info);

/**
 * See kbp_mngr_opcode_total_result_size_get() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_total_result_size_get_internal(
    int unit,
    uint8 opcode_id,
    uint32 *result_size);

/**
 * See kbp_mngr_opcode_lookup_add() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_lookup_add_internal(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info);

/**
 * See kbp_mngr_opcode_lookup_get() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_lookup_get_internal(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info);

/**
 * See kbp_mngr_opcode_result_clear() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_result_clear_internal(
    int unit,
    uint8 opcode_id,
    uint8 result_id);

shr_error_e kbp_mngr_context_to_opcode_set(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 opcode_id);

shr_error_e kbp_mngr_context_to_opcode_translate(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 *opcode_id);
/**
 * See kbp_mngr_opcode_master_key_set() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_master_key_set_internal(
    int unit,
    uint8 opcode_id,
    uint32 nof_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
 * See kbp_mngr_opcode_master_key_get() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_master_key_get_internal(
    int unit,
    uint32 opcode_id,
    uint32 *nof_key_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
 * See kbp_mngr_opcode_master_key_segments_add() in kbp_mngr.h
 */
shr_error_e kbp_mngr_opcode_master_key_segments_add_internal(
    int unit,
    uint32 opcode_id,
    uint32 nof_new_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
 * See kbp_mngr_init() in kbp_mngr.h
 */
shr_error_e kbp_mngr_init_internal(
    int unit);

/**
 * See kbp_mngr_deinit() in kbp_mngr.h
 */
shr_error_e kbp_mngr_deinit_internal(
    int unit);

/**
 * See kbp_mngr_sync() in kbp_mngr.h
 */
shr_error_e kbp_mngr_sync_internal(
    int unit);

/*
 * }
 */
#endif /* _KBP_MNGR_INTERNAL_H_INCLUDED__ */
