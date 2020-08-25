/** \file kbp_mngr_init.c
 *
 *  kbp procedures for DNX that should be used during KBP mngr init.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_KBP_MNGR

/*
 * Include files.
 * {
 */
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <shared/util.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_rop.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/dnxc_ha.h>

#include <soc/drv.h> 
#include "kbp_mngr_internal.h"
/*
 * }
 */
#if defined(INCLUDE_KBP)
/*
 * DEFINEs
 * {
 */

/*
 * Hash tables must take a power of two for table size.
 * This macro is used to convert an arbitrary size into containing legitimate hash table size.
 * It finds the smallest power of two which is larger than or equal to the input size.
 */
#define KBP_FWD_TCAM_ACCESS_HASH_TABLE_SIZE_CONVERT(_size) \
        (1 << utilex_log2_round_up(_size))

/*
 * }
 */
/*
 * GLOBALs
 * {
 */
extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];
extern kbp_warmboot_t Kbp_warmboot_data[SOC_MAX_NUM_DEVICES];

/*
 * }
 */
/*
 * FUNCTIONs
 * {
 */

extern int sal_atoi(
    const char *nptr);

/**
 * Convert DNX (DBAL) DB type for KBP DB type
 */
static shr_error_e
kbp_mngr_dbal_db_type_to_kbp_db_type(
    int unit,
    dbal_table_type_e type,
    int *kbp_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DBAL_TABLE_TYPE_LPM:
            *kbp_type = KBP_DB_LPM;
            break;
        
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
            *kbp_type = KBP_DB_ACL;
            break;
        case DBAL_TABLE_TYPE_EM:
            *kbp_type = KBP_DB_EM;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported DBAL table type to KBP %d\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Convert ResultLocation from string to enum
 */
static shr_error_e
kbp_mngr_valid_result_location_parse(
    int unit,
    char *result_location,
    uint8 *valid_location)
{
    SHR_FUNC_INIT_VARS(unit);

    if (sal_strcmp(result_location, "FWD") == 0)
    {
        *valid_location = DBAL_DEFINE_KBP_RESULT_LOCATION_FWD;
    }
    else if (sal_strcmp(result_location, "RPF") == 0)
    {
        *valid_location = DBAL_DEFINE_KBP_RESULT_LOCATION_RPF;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid result location: %s. Valid are FWD and RPF\n", result_location);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Convert given integer to KBP priority define
 */
static shr_error_e
kbp_mngr_kbp_meta_priority_get(
    int unit,
    int meta_priority,
    int *kbp_meta_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(kbp_meta_priority, _SHR_E_INTERNAL, "kbp_meta_priority");

    switch (meta_priority)
    {
        case 0:
            *kbp_meta_priority = KBP_ENTRY_META_PRIORITY_0;
            break;
        case 1:
            *kbp_meta_priority = KBP_ENTRY_META_PRIORITY_1;
            break;
        case 2:
            *kbp_meta_priority = KBP_ENTRY_META_PRIORITY_2;
            break;
        case 3:
            *kbp_meta_priority = KBP_ENTRY_META_PRIORITY_3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported META PRIORITY value %d\n", meta_priority);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Convert DBAL key field type to KBP key field type
 */
static shr_error_e
kbp_mngr_dbal_key_field_type_to_kbp_key_field_type(
    int unit,
    uint32 dbal_key_field_type,
    int *kbp_key_field_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbp_key_field_type, _SHR_E_INTERNAL, "kbp_key_field_type");

    switch (dbal_key_field_type)
    {
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_INVALID:
            *kbp_key_field_type = KBP_KEY_FIELD_INVALID;
            break;
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_PREFIX:
            *kbp_key_field_type = KBP_KEY_FIELD_PREFIX;
            break;
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM:
            *kbp_key_field_type = KBP_KEY_FIELD_EM;
            break;
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_RANGE:
            *kbp_key_field_type = KBP_KEY_FIELD_RANGE;
            break;
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_TERNARY:
            *kbp_key_field_type = KBP_KEY_FIELD_TERNARY;
            break;
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE:
            *kbp_key_field_type = KBP_KEY_FIELD_HOLE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported DBAL KBP_KEY_FIELD %d\n", dbal_key_field_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Transforms KBP key field type to DBAL key field type
 */
static shr_error_e
kbp_mngr_kbp_key_field_type_to_dbal_key_field_type(
    int unit,
    uint32 kbp_key_field_type,
    uint32 *dbal_key_field_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dbal_key_field_type, _SHR_E_INTERNAL, "dbal_key_field_type");

    switch (kbp_key_field_type)
    {
        case KBP_KEY_FIELD_TERNARY:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_TERNARY;
            break;
        case KBP_KEY_FIELD_PREFIX:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_PREFIX;
            break;
        case KBP_KEY_FIELD_EM:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM;
            break;
        case KBP_KEY_FIELD_RANGE:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_RANGE;
            break;
        case KBP_KEY_FIELD_HOLE:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE;
            break;
        case KBP_KEY_FIELD_INVALID:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_INVALID;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid KBP_KEY_FIELD %d\n", kbp_key_field_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Gets the key segments info from KBP_DB_INFO or KBP_OPCODE_INFO table
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - Handle to the DB or OPCODE table.
 *   \param [in] index - Segment index to get.
 *   \param [out] segment_type - Returned segment type in KBP define; Returned if not NULL.
 *   \param [out] segment_size - Returned segment size in bytes; Returned if not NULL.
 *   \param [out] segment_name - Returned segment name; Returned if not NULL.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_key_segment_get(
    int unit,
    uint32 entry_handle_id,
    dnx_kbp_sw_table_indication_e table_indication,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES])
{
    uint32 key_segment[DNX_KBP_KEY_SEGMENT_SIZE] = { 0 };
    uint32 dbal_segment_type, field_nof_bytes;
    uint32 segment_name_u32[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_WORDS] = { 0 };
    int kbp_segment_type = KBP_KEY_FIELD_INVALID;
    dbal_fields_e key_field;

    SHR_FUNC_INIT_VARS(unit);

    key_field = (table_indication == DNX_KBP_DB_TABLE_INDICATION ? DBAL_FIELD_DB_KEY : DBAL_FIELD_MASTER_KEY);

    if (segment_type == NULL && segment_size == NULL && segment_name == NULL)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, key_field, index, key_segment));

    if (segment_name)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, key_field, DBAL_FIELD_SEGMENT_NAME, segment_name_u32, key_segment));
        SHR_IF_ERR_EXIT(utilex_U32_to_U8
                        (segment_name_u32, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, (uint8 *) segment_name));
    }

    if (segment_size)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, key_field, DBAL_FIELD_SEGMENT_SIZE, &field_nof_bytes, key_segment));
        *segment_size = field_nof_bytes;
    }

    if (segment_type)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, key_field, DBAL_FIELD_SEGMENT_TYPE, &dbal_segment_type, key_segment));
        /** Convert the DBAL key field type to KBP key field type */
        SHR_IF_ERR_EXIT(kbp_mngr_dbal_key_field_type_to_kbp_key_field_type(unit, dbal_segment_type, &kbp_segment_type));
        *segment_type = kbp_segment_type;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sets the key segments info to KBP_DB_INFO or KBP_OPCODE_INFO table
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - Handle to the DB or OPCODE table.
 *   \param [in] index - Segment index to get.
 *   \param [in] segment_type - Input segment type in KBP define; Set if not NULL.
 *   \param [in] segment_size - Input segment size in bytes; Set if not NULL.
 *   \param [in] segment_name - Input segment name; Set if not NULL.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e
kbp_mngr_key_segment_set(
    int unit,
    uint32 entry_handle_id,
    dnx_kbp_sw_table_indication_e table_indication,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES])
{
    uint32 key_segment[DNX_KBP_KEY_SEGMENT_SIZE] = { 0 };
    uint32 dbal_segment_type;
    uint32 segment_type_get, segment_size_get;
    uint32 segment_name_u32[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_WORDS] = { 0 };
    char segment_name_get[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
    dbal_fields_e key_field;

    SHR_FUNC_INIT_VARS(unit);

    key_field = (table_indication == DNX_KBP_DB_TABLE_INDICATION ? DBAL_FIELD_DB_KEY : DBAL_FIELD_MASTER_KEY);

    if (segment_type == NULL && segment_size == NULL && segment_name == NULL)
    {
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_get
                        (unit, entry_handle_id, table_indication, index, &segment_type_get, &segment_size_get,
                         segment_name_get));
    }

    utilex_U8_to_U32((uint8 *) (segment_name ? segment_name : segment_name_get), DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES,
                     segment_name_u32);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, key_field, DBAL_FIELD_SEGMENT_NAME, segment_name_u32, key_segment));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, key_field, DBAL_FIELD_SEGMENT_SIZE,
                     (segment_size ? segment_size : &segment_size_get), key_segment));

    /** Convert the KBP key field type to DBAL key field type */
    SHR_IF_ERR_EXIT(kbp_mngr_kbp_key_field_type_to_dbal_key_field_type
                    (unit, (segment_type ? *segment_type : segment_type_get), &dbal_segment_type));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, key_field, DBAL_FIELD_SEGMENT_TYPE, &dbal_segment_type, key_segment));

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, key_field, index, key_segment);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Parse segments indices from CSV string to index array. Used in OPCODE XML parsing.
 */
static shr_error_e
kbp_mngr_parse_segments_to_array(
    int unit,
    char *segment_string,
    uint8 *segment_array,
    uint8 *nof_segments)
{
    char segment;
    char delimiter1 = ',';
    char delimiter2 = ' ';
    int str_len = 0;
    int char_pos = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(segment_string, _SHR_E_INTERNAL, "segment_string");
    SHR_NULL_CHECK(segment_array, _SHR_E_INTERNAL, "segment_array");
    SHR_NULL_CHECK(nof_segments, _SHR_E_INTERNAL, "nof_segments");

    *nof_segments = 0;
    str_len = sal_strlen(segment_string);
    for (char_pos = 0; char_pos < str_len; char_pos++)
    {
        if (segment_string[char_pos] == delimiter1 || segment_string[char_pos] == delimiter2)
        {
            continue;
        }
        else
        {
            segment = segment_string[char_pos];
            segment_array[(*nof_segments)] = sal_atoi(&segment);
            (*nof_segments)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Maps FWD and ACL contexts to KBP opcode ID.
*   \param [in] unit - Relevant unit.
*   \param [in] fwd_context - Forwarding context (only from IFWD2 stage).
*   \param [in] acl_context - ACL context.
*   \param [in] opcode_id - The KBP opcode ID used by the contexts.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e
kbp_mngr_context_to_opcode_set(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 opcode_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_context);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT, acl_context);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_OPCODE_ID, INST_SINGLE, opcode_id);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Validate that for specific fwc_ctx_id the opcode_id is configured as expeced
* \param [in]  unit - The Unit number.
* \param [in]  fwd_ctx_id - fwd_context.
* \param [int] opcode_id - The expected opcode ID according to ARCH definitions (XML).
* \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
* \remark
*   None
* \see
*   shr_error_e
*/
shr_error_e
kbp_mngr_fwd_context_to_opcode_hw_verify(
    int unit,
    int fwd_ctx_id,
    int opcode_id)
{
    uint32 entry_handle_id;
    uint32 configured_opcode_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_ctx_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT, fwd_ctx_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_KBP_OPCODE_ID, INST_SINGLE, &configured_opcode_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (opcode_id != configured_opcode_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "context %s (%d) configured to opcode %d expected to be configured to opcode %s (%d).\n",
                     dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID, fwd_ctx_id), fwd_ctx_id,
                     configured_opcode_id, dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_KBP_FWD_OPCODE,
                                                                       opcode_id), opcode_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   reads the contexts that related to this copcode from XML and verfiy that HW is configured according to it
*/
static shr_error_e
kbp_mngr_opcode_to_context_read(
    int unit,
    void *opcode_node,
    int opcode_id)
{
    void *iter_top;
    void *iter;
    uint32 entry_handle_id;
    uint32 ctx_id[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE] = { 0 };
    int ctx_index = 0;
    int nof_ctxs = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    iter_top = dbx_xml_child_get_first(opcode_node, "FwdContexts");
    if (iter_top == NULL)
    {
        /** no context related to this opcode currently it is OK. need to think about it */
        SHR_EXIT();
    }

    RHDATA_ITERATOR(iter, iter_top, "Context")
    {
        char ctx_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };

        RHDATA_GET_STR_STOP(iter, "Name", ctx_name);
        /*
         * receive the ctx value 
         */
        SHR_IF_ERR_EXIT(dbal_enum_type_string_to_id
                        (unit, DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID, ctx_name, &ctx_id[ctx_index]));

        SHR_IF_ERR_EXIT(kbp_mngr_fwd_context_to_opcode_hw_verify(unit, ctx_id[ctx_index], opcode_id));

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "opcode %s (%d) related to contexts %s\n"),
                                     dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_KBP_FWD_OPCODE, opcode_id),
                                     opcode_id, ctx_name));

        ctx_index++;
    }

    nof_ctxs = ctx_index;
    if (nof_ctxs > DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Opcode %d is mapped to %d contexts, which is more than the available %d\n",
                     opcode_id, nof_ctxs, DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);

    for (ctx_index = 0; ctx_index < nof_ctxs; ctx_index++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FWD_CONTEXT, ctx_index,
                                     ctx_id[ctx_index]);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_OPCODE_FWD_CONTEXTS, INST_SINGLE, nof_ctxs);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Used to set the lookup info opcode.
 */
shr_error_e
kbp_mngr_opcode_lookup_set(
    int unit,
    uint8 opcode_id,
    uint8 lookup_id,
    uint8 lookup_type,
    uint8 lookup_db,
    uint8 result_id,
    uint8 nof_key_segments,
    uint8 key_segment_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP])
{
    uint32 entry_handle_id;
    uint32 used_segments_bmp = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Validate lookup segment indices */
    for (index = 0; index < nof_key_segments; index++)
    {
        if (key_segment_index[index] > DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid key segment index %d\n", key_segment_index[index]);
        }
        if (utilex_bitstream_test_bit(&used_segments_bmp, key_segment_index[index]))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "found repeating key segment index %d\n", key_segment_index[index]);
        }
        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&used_segments_bmp, key_segment_index[index]));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ID, lookup_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, lookup_db);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_TYPE, INST_SINGLE, lookup_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE, result_id);

    /** Set lookup segment indices */
    for (index = 0; index < nof_key_segments; index++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_SEGMENT_IDS, index,
                                     key_segment_index[index]);
    }
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, nof_key_segments);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Used to set the result info (offset and size) per opcode. The padding is set to 0 and will be updated in sync.
 */
shr_error_e
kbp_mngr_opcode_result_set(
    int unit,
    uint8 opcode_id,
    uint8 result_id,
    uint8 offset,
    uint8 size)
{
    uint32 entry_handle_id;
    uint8 is_valid;
    uint8 get_offset;
    uint8 get_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_RESULT_ID, result_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

    if (is_valid)
    {
        /** Another lookup is added to this result; perform parameters validation */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Validating existing result %d for opcode %d, offset %d, size %d\n"), result_id,
                     opcode_id, offset, size));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, &get_offset));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &get_size));
        if ((offset != get_offset) || (size != get_size))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The new result parameters for result %d do not match the currently configured ones\nNew offset, size = %d, %d; Current offset, size = %d, %d\n",
                         result_id, offset, size, get_offset, get_size);
        }
    }
    else
    {
        /** Configure the result */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Configuring result %d for opcode %d, offset %d, size %d\n"), result_id,
                     opcode_id, offset, size));
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RESULT_PADDING, INST_SINGLE, 0);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, offset);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, size);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Parse the information for a single opcode and update KBP_OPCODE_INFO according it.
* \param [in]  unit - The Unit number.
* \param [in]  opcode_node - The node to an opcode in the opcode catalogue.
* \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
* \remark
*   None
* \see
*   shr_error_e
*/
static shr_error_e
kbp_mngr_opcode_read(
    int unit,
    void *opcode_node)
{
    void *iter_top;
    void *iter;
    int index;

    int opcode_id;
    char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };
    uint32 lookup_bmp = 0;
    uint32 result_bmp = 0;
    uint8 nof_public_lookups = 0;
    uint8 nof_opcodes;
    uint8 is_allocated;
    sw_state_htbl_init_info_t fwd_tcam_access_hash_info;

    int key_id[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    int nof_keys = 0;

    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));

    RHDATA_GET_STR_STOP(opcode_node, "Name", opcode_name);

    RHDATA_GET_INT_STOP(opcode_node, "OpCode", opcode_id);

    
    if (!SAL_BOOT_PLISIM)
    {
        if ((opcode_id == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_UNICAST_PRIVATE_W_UC_RPF) ||
            (opcode_id == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_MULTICAST_PRIVATE_W_UC_RPF))
        {
            if (!dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Opcode %s (%d) is disabled since IPv4 external fwd is not enabled\n"),
                             opcode_name, opcode_id));
                SHR_EXIT();
            }
        }
        if ((opcode_id == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV6_UNICAST_PRIVATE_W_UC_RPF) ||
            (opcode_id == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV6_MULTICAST_PRIVATE_W_UC_RPF))
        {
            if (!dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Opcode %s (%d) is disabled since IPv6 external fwd is not enabled\n"),
                             opcode_name, opcode_id));
                SHR_EXIT();
            }
        }
        
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_ALL, (uint8 *) opcode_name);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Opcode %s (%d) is enabled\n"), opcode_name, opcode_id));

    iter_top = dbx_xml_child_get_first(opcode_node, "MasterKey");
    if (iter_top == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find KBP opcode master key configuration (opcode %d)\n", opcode_id);
    }

    /** Parse master key */
    index = 0;
    RHDATA_ITERATOR(iter, iter_top, "Field")
    {
        int key_size = 0;
        char key_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
        /** Set all segment types to EM */
        uint32 segment_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM;

        RHDATA_GET_STR_STOP(iter, "Name", key_name);
        RHDATA_GET_INT_STOP(iter, "Segment", key_id[index]);
        RHDATA_GET_INT_STOP(iter, "BitSize", key_size);

        /** Convert the size from bits to bytes */
        key_size = BITS2BYTES(key_size);

        /** Set the parsed master key segment */
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_set
                        (unit, entry_handle_id, DNX_KBP_OPCODE_TABLE_INDICATION, index, &segment_type,
                         (uint32 *) &key_size, key_name));
        index++;
    }
    nof_keys = index;

    if (nof_keys > DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "The number of key segments (%d) read from the opcodes definition XML exceeds the maximum available number of segments %d\n",
                     nof_keys, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, nof_keys);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));

    /** Parse used DBs */
    index = 0;
    RHDATA_ITERATOR(iter, opcode_node, "AppDB")
    {
        char res_index[16] = { 0 };
        char segments[16] = { 0 };
        int app_db_segment_index = 0;
        uint8 db_id = 0;
        uint32 res_size = 0;
        uint32 res_offset = 0;
        uint8 lookup_type = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FREE;
        char app_db_name[DBAL_MAX_STRING_LENGTH] = { 0 };
        uint8 app_db_result_index = 0;
        uint8 app_db_segment_id[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
        uint8 app_db_nof_segments = 0;

        RHDATA_GET_STR_STOP(iter, "Name", app_db_name);

        SHR_IF_ERR_EXIT(kbp_mngr_db_id_by_name_get(unit, app_db_name, &db_id));

        RHDATA_GET_STR_STOP(iter, "ResultLocation", res_index);
        SHR_IF_ERR_EXIT(kbp_mngr_valid_result_location_parse(unit, res_index, &app_db_result_index));

        /** Assign result size and offset according to the location (hard-coded) */
        if (app_db_result_index == DBAL_DEFINE_KBP_RESULT_LOCATION_FWD)
        {
            res_size = DNX_KBP_RESULT_SIZE_FWD;
            res_offset = 0;
            if (sal_strstr(app_db_name, "PUBLIC"))
            {
                lookup_type = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD_PUBLIC;
                nof_public_lookups++;
            }
            else
            {
                lookup_type = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD;
            }
        }
        else if (app_db_result_index == DBAL_DEFINE_KBP_RESULT_LOCATION_RPF)
        {
            res_size = DNX_KBP_RESULT_SIZE_RPF;
            res_offset = DNX_KBP_RESULT_SIZE_FWD;
            if (sal_strstr(app_db_name, "PUBLIC"))
            {
                lookup_type = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF_PUBLIC;
                nof_public_lookups++;
            }
            else
            {
                lookup_type = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF;
            }
        }

        RHDATA_GET_STR_STOP(iter, "Segments", segments);
        SHR_IF_ERR_EXIT(kbp_mngr_parse_segments_to_array(unit, segments, app_db_segment_id, &app_db_nof_segments));

        /** Get the AppDb segments info from the opcode (master key) and validate them */
        for (app_db_segment_index = 0; app_db_segment_index < app_db_nof_segments; app_db_segment_index++)
        {
            int key_index = 0;
            /** Find the segment in the opcode (master key) */
            for (key_index = 0; key_index < nof_keys; key_index++)
            {
                if (key_id[key_index] == app_db_segment_id[app_db_segment_index])
                {
                    /** Master key segment found */
                    break;
                }
            }
            if (key_index >= nof_keys)
            {
                /** Non-existing segment index */
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "AppDb %s on search %d has a segment index %d that does not exist in opcode (%d) %s\n",
                             app_db_name, index, app_db_segment_id[app_db_segment_index], opcode_id, opcode_name);
            }
        }

        dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
        dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, (uint8) index);

        if (utilex_bitstream_test_bit(&lookup_bmp, index))
        {
            /** If we have a second lookup on the same index, we have a public lookup */
            lookup_type = (lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD) ?
                DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD_PUBLIC : DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF_PUBLIC;
        }
        else
        {
            utilex_bitstream_set_bit(&lookup_bmp, index);
        }

        utilex_bitstream_set_bit(&result_bmp, app_db_result_index);

        /** Update lookup info */
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_set
                        (unit, opcode_id, (uint8) index, lookup_type, db_id, app_db_result_index, app_db_nof_segments,
                         app_db_segment_id));

        /** Update result info */
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_result_set(unit, opcode_id, app_db_result_index, res_offset, res_size));

        index++;
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, lookup_bmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, result_bmp);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_NOF_PUBLIC_LOOKUPS, INST_SINGLE, nof_public_lookups);

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Get the number of valid opcodes */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_GENERAL_INFO, entry_handle_id));
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_NOF_OPCODES, INST_SINGLE, &nof_opcodes);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** updating the number of OPCODES in the KBP */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_OPCODES, INST_SINGLE, nof_opcodes + 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Parse the related fwd contexts to this opcode  */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_context_read(unit, opcode_node, opcode_id));

    /** Initialize IPv4/6 MC for internal storing of access IDs */
    SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.is_init(unit, &is_allocated));
    if (!is_allocated)
    {
        SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.init(unit));
    }
    if (opcode_id == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_MULTICAST_PRIVATE_W_UC_RPF)
    {
        sal_memset(&fwd_tcam_access_hash_info, 0, sizeof(fwd_tcam_access_hash_info));
        fwd_tcam_access_hash_info.expected_nof_elements = KBP_FWD_TCAM_ACCESS_HASH_TABLE_SIZE_CONVERT(1 << 12);
        fwd_tcam_access_hash_info.max_nof_elements = KBP_FWD_TCAM_ACCESS_HASH_TABLE_SIZE_CONVERT(1 << 12);
        fwd_tcam_access_hash_info.hash_function = NULL;
        fwd_tcam_access_hash_info.print_cb_name = "kbp_ipv4_fwd_tcam_access_mapping_htb_entry_print";

        SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.create(unit, DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV4_MC,
                                                                       &fwd_tcam_access_hash_info));
    }
    if (opcode_id == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV6_MULTICAST_PRIVATE_W_UC_RPF)
    {
        sal_memset(&fwd_tcam_access_hash_info, 0, sizeof(fwd_tcam_access_hash_info));
        fwd_tcam_access_hash_info.expected_nof_elements = KBP_FWD_TCAM_ACCESS_HASH_TABLE_SIZE_CONVERT(1 << 12);
        fwd_tcam_access_hash_info.max_nof_elements = KBP_FWD_TCAM_ACCESS_HASH_TABLE_SIZE_CONVERT(1 << 12);
        fwd_tcam_access_hash_info.hash_function = NULL;
        fwd_tcam_access_hash_info.print_cb_name = "kbp_ipv6_fwd_tcam_access_mapping_htb_entry_print";

        SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.create(unit, DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV6_MC,
                                                                       &fwd_tcam_access_hash_info));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Update the padding of the results of a certain opcode.
*   whenever a result ID does not begin after the previous one has ended, we need to pad the result ID
*   to make them continuous.
*   If we have a hole between the result IDs (dummy result ID) we use it for padding, giving every hole
*   at least one byte of padding.
*   \param [in] unit - Relevant unit.
*   \param [in] opcode_id - The opcode_id for which we calculate the padding
*   \param [in] result_bmp - The bitmap of all result types used 
*                            (all ones mut start from the first bit and be consecutive).
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    Intended to be called from kbp_mngr_opcode_info_update.
*  \see
*    shr_error_e
*/
static shr_error_e
kbp_mngr_opcode_info_update_result_padding_calc(
    int unit,
    uint8 opcode_id,
    uint32 result_bmp)
{
    uint32 res_entry_handle_id;
    uint8 result_id;
    uint8 result_size[DNX_KBP_MAX_NOF_RESULTS] = { 0 };
    uint8 result_offset[DNX_KBP_MAX_NOF_RESULTS] = { 0 };
    uint32 buffer_size = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));

    /** Get resutl size and offset */
    for (result_id = 0; result_id < DNX_KBP_MAX_NOF_RESULTS; result_id++)
    {
        if (utilex_bitstream_test_bit(&result_bmp, result_id))
        {
            dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, result_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, res_entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE,
                             &result_offset[result_id]));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, res_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &result_size[result_id]));

        }
    }

    /** Calculate paddings */
    for (result_id = 0; result_id < DNX_KBP_MAX_NOF_RESULTS; result_id++)
    {
        if (utilex_bitstream_test_bit(&result_bmp, result_id))
        {
            int padding = 0;

            /*
             * If the result has zero size, we pad it with extra offset we may have from a future result.
             * We assume zero size means that the result is a dummy result.
             * If we have multiple consecutive dummy results we give all but the last 1 byte padding (minimum)
             */
            if (result_size[result_id] == 0)
            {
                if (utilex_bitstream_test_bit(&result_bmp, result_id + 1) == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error, dummy result for opcode %d at the end of the bitmap. "
                                 "last result %d, result bitmap 0x%x\n", opcode_id, result_id, result_bmp);
                }
                if (result_size[result_id + 1] == 0)
                {
                    padding = BYTES2BITS(DNX_KBP_RESULT_SIZE_MIN_BYTES);
                }
                else
                {
                    padding = result_offset[result_id + 1] - buffer_size;
                }
                if (padding <= 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error, not enough space for padding dummy results before "
                                 "result_id %d in OPCODE ID %d (offset %d)\n",
                                 result_id, opcode_id, result_offset[result_id + 1]);
                }
                buffer_size += padding;
            }
            else
            {
                /*
                 * Calculate the required result padding according to the difference
                 * in the offset and size between results
                 */
                padding = result_offset[result_id] - buffer_size;
                if (padding < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error, not enough offset for result_id %d in opcode_id %d. "
                                 "Offset %d, previous result_id ends in %d.\n",
                                 opcode_id, result_id, result_offset[result_id], buffer_size);
                }
                buffer_size += padding + result_size[result_id];
            }

            if (padding < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error, reached negative padding for opcode_id %d, reuslt_id %d, "
                             "offset %d, padding %d\n", opcode_id, result_id, result_offset[result_id], padding);
            }

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Update result %d padding for opcode %d to %d\n"), result_id, opcode_id,
                         padding));

            dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, result_id);

            dbal_entry_value_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_PADDING, INST_SINGLE, padding);
            dbal_entry_value_field8_set(unit, res_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, res_entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Updates all valid opcodes lookups and results and introduce dummy lookups
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_opcode_info_update(
    int unit)
{
    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;
    int is_end;
    uint8 is_dummy_needed = FALSE;
    uint8 is_valid = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint8 opcode_id;
        uint8 result_id;
        uint32 lookup_bmp = 0;
        uint32 result_bmp = 0;
        uint8 nof_public_lookups;

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_NOF_PUBLIC_LOOKUPS, INST_SINGLE, &nof_public_lookups));

        /** Iterate over all results and find holes in them */
        for (result_id = 0; result_id < DNX_KBP_MAX_NOF_RESULTS - nof_public_lookups; result_id++)
        {
            if (!utilex_bitstream_test_bit(&result_bmp, result_id))
            {
                if (utilex_bitstream_have_one_in_range(&result_bmp, result_id + 1, DNX_KBP_MAX_NOF_RESULTS - 1))
                {
                    /** Hole was found; create dummy lookup for it */
                    uint8 lookup_id = result_id + nof_public_lookups;

                    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
                    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, lookup_id);

                    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
                    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE,
                                                DNX_KBP_DUMMY_HOLE_DB_ID);
                    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE,
                                                result_id);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lkp_entry_handle_id, DBAL_COMMIT));

                    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&lookup_bmp, lookup_id));
                    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&result_bmp, result_id));

                    is_dummy_needed = TRUE;
                }
            }
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, lookup_bmp);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, result_bmp);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Update padding
         */
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_info_update_result_padding_calc(unit, opcode_id, result_bmp));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (is_dummy_needed)
    {
        /** Update Dummy DB info to valid */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_DB_INFO, entry_handle_id));
        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, DNX_KBP_DUMMY_HOLE_DB_ID);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sync all Valid LTRs (opcodes) to KBP device.
 *   Each entry in KBP_OPCODE_INFO contains all information for creating KBP LTR.
 *   Create a KBP instruction for all valid entries in KBP_OPCODE_INFO and add the lookup DBs to that instruction.
 *   \param [in] unit - Relevant unit.
 *   \param [in] smt_id - KBP thread ID 0 or 1. In case SMT is not enabled, this parameter is irrelevant.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_opcode_sync(
    int unit,
    uint32 smt_id)
{
    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;
    uint32 res_entry_handle_id;
    uint32 lookup_bmp = 0;
    uint32 result_bmp = 0;
    int is_end;
    uint8 is_valid = TRUE;
    struct kbp_device *device = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (kbp_mngr_smt_enabled(unit))
    {
        if (smt_id >= DNX_KBP_MAX_NOF_SMTS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid SMT ID %d\n", smt_id);
        }
        device = Kbp_app_data[unit]->smt_p[smt_id];
    }
    else
    {
        /** OP compatibility */
        smt_id = 0;
        device = Kbp_app_data[unit]->device_p;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 opcode_id = 0;
        uint32 instruction_id = 0;
        uint32 nof_segments = 0;
        int index;
        kbp_key_t_p key;
        kbp_instruction_handles_t inst_handle;
        uint32 ad_width[DNX_KBP_MAX_NOF_LOOKUPS] = { 0 };
        uint32 res_padding[DNX_KBP_MAX_NOF_LOOKUPS] = { 0 };
        uint8 rpf_result[DNX_KBP_MAX_NOF_RESULTS] = { 0 };
        uint32 core = 0;
        uint32 master_key_nof_bytes = 0;
        uint32 result_buffer_nof_bytes = 0;

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
        instruction_id = opcode_id + smt_id * DNX_KBP_SMT_INSTRUCTION_OFFSET;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));

        if (lookup_bmp == 0)
        {
                        /** incase the opcode has no lookups no need to configure it in the KBP */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            continue;
        }

        if (!SOC_WARM_BOOT(unit))
        {
            sal_memset(&inst_handle, 0x0, sizeof(inst_handle));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_segments));

            DNX_KBP_TRY_PRINT(kbp_instruction_init
                              (device, instruction_id, opcode_id, &inst_handle.inst_p), "INSTR", instruction_id);

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Created new Instruction ID %d, %p\n"), instruction_id, inst_handle.inst_p));

            DNX_KBP_TRY(kbp_key_init(device, &key));
            /** setting the OPCODE master key in KBP according to table definitions */
            for (index = 0; index < nof_segments; index++)
            {
                uint32 field_nof_bytes;
                char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
                int kbp_segment_type;

                SHR_IF_ERR_EXIT(kbp_mngr_key_segment_get
                                (unit, entry_handle_id, DNX_KBP_OPCODE_TABLE_INDICATION, index,
                                 (uint32 *) &kbp_segment_type, &field_nof_bytes, segment_name));

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Adding master key segment %d, name %s, size %d type %d\n"), index,
                             segment_name, field_nof_bytes, kbp_segment_type));

                DNX_KBP_TRY_PRINT(kbp_key_add_field(key, segment_name, BYTES2BITS(field_nof_bytes), kbp_segment_type),
                                  "INSTR", instruction_id);

                master_key_nof_bytes += field_nof_bytes;
            }
            /*
             * Pad the key to a multiplier of 80 bits.
             */
            if ((master_key_nof_bytes % DNX_KBP_MASTER_KEY_BYTE_GRANULARITY) != 0)
            {
                unsigned int padding_size_bytes =
                    DNX_KBP_MASTER_KEY_BYTE_GRANULARITY - (master_key_nof_bytes % DNX_KBP_MASTER_KEY_BYTE_GRANULARITY);
                DNX_KBP_TRY_PRINT(kbp_key_add_field
                                  (key, "PADD_80b", BYTES2BITS(padding_size_bytes), KBP_KEY_FIELD_HOLE),
                                  "Padding key for INSTR", instruction_id);
            }

            DNX_KBP_TRY_PRINT(kbp_instruction_set_key(inst_handle.inst_p, key), "instruction", instruction_id);

            for (index = 0; index < DNX_KBP_MAX_NOF_LOOKUPS; index++)
            {
                if (utilex_bitstream_test_bit(&lookup_bmp, index))
                {
                    uint32 db_id = 0;
                    uint32 result_index = 0;
                    kbp_db_handles_t db_handle;
                    uint32 lookup_type;

                    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
                    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, (uint8) index);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));

                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, &db_id));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, lkp_entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE, &result_index));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_TYPE, INST_SINGLE, &lookup_type));

                    rpf_result[result_index] =
                        ((lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF)
                         || (lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF_PUBLIC)) ? TRUE : FALSE;

                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Setting lookup DB %d search index %d result location %d\n"),
                                 db_id, index, result_index));
                    /** Read DB handles from SW state */
                    SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, db_id, &db_handle));
                    DNX_KBP_TRY_PRINT(kbp_instruction_add_db(inst_handle.inst_p, db_handle.db_p, result_index), "DB",
                                      db_id);
                }
            }

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));

            for (index = 0; index < DNX_KBP_MAX_NOF_RESULTS; index++)
            {
                if (utilex_bitstream_test_bit(&result_bmp, index))
                {
                    dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
                    dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, (uint8) index);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));

                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, res_entry_handle_id, DBAL_FIELD_RESULT_PADDING, INST_SINGLE,
                                     &res_padding[index]));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, res_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &ad_width[index]));

                    /** Convert padding and size from bits to bytes */
                    res_padding[index] = BITS2BYTES(res_padding[index]);
                    ad_width[index] = BITS2BYTES(ad_width[index]);

                    result_buffer_nof_bytes += res_padding[index] + ad_width[index];
                }
            }

            DNX_KBP_TRY_PRINT(kbp_instruction_install(inst_handle.inst_p), "INSTR", instruction_id);

            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {
                if (dnx_kbp_interface_enabled(unit))
                {
                    dnx_kbp_opcode_map_t opcode_map = { 0 };

                    /** opcode request reply configurations */
                    opcode_map.opcode = opcode_id;
                    opcode_map.tx_type = DNX_KBP_OPCODE_TYPE_REQUEST;
                    opcode_map.tx_size = master_key_nof_bytes;
                    opcode_map.rx_type = DNX_KBP_OPCODE_TYPE_REPLY;
                    opcode_map.rx_size = result_buffer_nof_bytes + 1;
                    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));

                    if (Kbp_app_data[unit]->device_type == KBP_DEVICE_OP)
                    {
                        dnx_kbp_lut_data_t lut_data = { 0 };

                        /** set LUT configurations */
                        lut_data.rec_is_valid = TRUE;
                        /** Currently opcode id is equal to LTR */
                        lut_data.instr = DNX_KBP_ROP_LUT_INSTR_GET(opcode_id);
                        lut_data.rec_size = master_key_nof_bytes;
                        SHR_IF_ERR_EXIT(dnx_kbp_lut_write(unit, core, opcode_id, &lut_data));
                    }
                    else
                    {
                        DNX_KBP_TRY(kbp_instruction_set_opcode(inst_handle.inst_p, opcode_id));
                    }
                }
            }

            /** set KBP_INST_PROP_RESULT_DATA_LEN */
            for (index = 0; index < DNX_KBP_MAX_NOF_LOOKUPS; index++)
            {
                if (utilex_bitstream_test_bit(&result_bmp, index))
                {
                    if (ad_width[index] + res_padding[index] <= 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Result %d has no size (%d) and no padding(%d).\n",
                                     index, ad_width[index], res_padding[index]);
                    }
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Result property set index %d length %dB (pad %dB res %dB)\n"),
                                 index, (ad_width[index] + res_padding[index]), res_padding[index], ad_width[index]));
                    DNX_KBP_TRY(kbp_instruction_set_property
                                (inst_handle.inst_p, KBP_INST_PROP_RESULT_DATA_LEN, index, rpf_result[index],
                                 (ad_width[index] + res_padding[index])));
                }
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(KBP_ACCESS.instruction_info.get(unit, instruction_id, &inst_handle));
            DNX_KBP_TRY(kbp_instruction_refresh_handle(device, inst_handle.inst_p, &inst_handle.inst_p));
        }

        /** save instruction handles in SW state */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SW_STATE));
        SHR_IF_ERR_EXIT(KBP_ACCESS.instruction_info.set(unit, instruction_id, &inst_handle));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SW_STATE));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Configure dummy DB for hole lookups in KBP_DB_INFO
 */
static shr_error_e
kbp_mngr_dummy_db_configure(
    int unit)
{
    uint32 entry_handle_id;
    uint32 segment_bytes = 1;
    uint32 segment_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, DNX_KBP_DUMMY_HOLE_DB_ID);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DB_NAME, INST_ALL, (uint8 *) "DUMMY_HOLE");
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_KEY_WIDTH, INST_SINGLE, 80);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_WIDTH, INST_SINGLE, 32);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_TYPE, INST_SINGLE, DBAL_TABLE_TYPE_TCAM_DIRECT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, 0);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_CLONE_DB, INST_SINGLE, 0);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, FALSE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(kbp_mngr_key_segment_set
                    (unit, entry_handle_id, DNX_KBP_DB_TABLE_INDICATION, 0, &segment_type, &segment_bytes, "HOLE"));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize a single KBP_DB_INFO table from DBAL table.
 * \param [in] unit - Relevant unit.
 * \param [in] db_id - KBP DB ID.
 * \param [in] clone_id - cloned KBP DB ID, 0 means not cloned.
 * \param [in] table_id - DBAL table ID.
 * \param [in] table - Pointer to a DBAL logical table.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
kbp_mngr_db_info_configure(
    int unit,
    uint8 db_id,
    uint8 clone_id,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    uint32 key_width = 0;
    uint32 result_width = 0;
    int size;
    int key_index;
    const dbal_logical_table_t *table;
    uint8 is_public = FALSE;
    uint32 public_field = DBAL_NOF_FIELDS;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(dnx_kbp_valid_key_width(unit, table->key_size, &key_width));
    SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, table->max_payload_size, &result_width));

    /** All tables sizes are set to 4096 */
    size = 4096;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, db_id);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DB_NAME, INST_ALL, (uint8 *) table->table_name);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, table_id);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_KEY_WIDTH, INST_SINGLE, key_width);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_WIDTH, INST_SINGLE, result_width);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_TYPE, INST_SINGLE, table->table_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, size);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_CLONE_DB, INST_SINGLE, clone_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, FALSE);

    
    if (sal_strstr(table->table_name, "PRIVATE"))
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_META_PRIORITY, INST_SINGLE, 0);
    }
    else if (sal_strstr(table->table_name, "PUBLIC"))
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_META_PRIORITY, INST_SINGLE, 1);
        if (sal_strstr(table->table_name, "IPV4"))
        {
            is_public = TRUE;
            public_field = DBAL_FIELD_IPV4_PUBLIC_ENABLED;
        }
        else if (sal_strstr(table->table_name, "IPV6"))
        {
            is_public = TRUE;
            public_field = DBAL_FIELD_IPV6_PUBLIC_ENABLED;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Public KBP tables need to be IPv4 or IPv6\n");
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    for (key_index = 0; key_index < table->nof_key_fields; key_index++)
    {
        uint32 field_nof_bytes = BITS2BYTES(table->keys_info[key_index].field_nof_bits);
        uint32 segment_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM;
        char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = DNX_KBP_KEY_SEGMENT_NOT_CONFIGURED;

        /** Initialize the segment names to "NOT_CONFIGURED". They will be updated after the table is validated. */
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_set
                        (unit, entry_handle_id, DNX_KBP_DB_TABLE_INDICATION, key_index, &segment_type, &field_nof_bytes,
                         segment_name));

    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE,
                                 table->nof_key_fields);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (is_public == TRUE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_GENERAL_INFO, entry_handle_id));
        dbal_entry_value_field8_set(unit, entry_handle_id, public_field, INST_SINGLE, TRUE);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Update the key segments of a given DB using the opcode and lookup info.
 *   All DBs are created without key segment names.
 *   They need to be updated using the master key segment names in order to correctly perform device lock.
 * \param [in]  unit - The Unit number.
 * \param [in]  entry_handle_id - Handle to the OPCODE table.
 * \param [in]  lkp_entry_handle_id - Handle to the OPCODE LOOKUP table that will be used to update the segments.
 * \param [in]  db_id - The DB ID in KBP_DB_INFO of the DB that has its segments updated.
 * \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
static shr_error_e
kbp_mngr_db_segment_names_update(
    int unit,
    uint32 entry_handle_id,
    uint32 lkp_entry_handle_id,
    uint8 db_id)
{
    int index;
    uint32 db_entry_handle_id;
    uint32 nof_master_key_segments;
    uint8 nof_db_segments;
    uint32 db_segment_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    char segment_name[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP][DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { {0} };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_master_key_segments));
    for (index = 0; index < nof_master_key_segments; index++)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_get
                        (unit, entry_handle_id, DNX_KBP_OPCODE_TABLE_INDICATION, index, NULL, NULL,
                         segment_name[index]));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));
    dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, db_entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_db_segments));
    for (index = 0; index < nof_db_segments; index++)
    {
        /** Get the DB key segment index that is mapped to the master key segments */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_SEGMENT_IDS, index, &db_segment_index[index]));
    }

    for (index = 0; index < nof_db_segments; index++)
    {
        /** Update all DB segment names */
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_set
                        (unit, db_entry_handle_id, DNX_KBP_DB_TABLE_INDICATION, index, NULL, NULL,
                         segment_name[db_segment_index[index]]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create a clone DB using the information from the original DB.
 *   The newly cloned DB key segments will not be set. They need to be updated after the clone is created.
 * \param [in]  unit - The Unit number.
 * \param [in]  original_db_id - The ID in KBP_DB_INFO of the DB that is being cloned.
 * \param [out] clone_db_id - The ID of the new cloned DB in KBP_DB_INFO.
 * \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
static shr_error_e
kbp_mngr_db_clone_create(
    int unit,
    uint8 original_db_id,
    uint8 *clone_db_id)
{
    uint8 db_id;
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, original_db_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &dbal_table_id));
    SHR_IF_ERR_EXIT(kbp_mngr_db_create_internal(unit, dbal_table_id, original_db_id, &db_id));

    *clone_db_id = db_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See kbp_mngr_create() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_db_create_internal(
    int unit,
    dbal_tables_e table_id,
    uint8 clone_id,
    uint8 *db_id)
{
    uint32 entry_handle_id, nof_dbs;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    /** updating the number of DBs in the KBP */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NOF_DBS, INST_SINGLE, &nof_dbs);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    nof_dbs++;
    SHR_IF_ERR_EXIT(kbp_mngr_db_info_configure(unit, nof_dbs, clone_id, table_id));
    SHR_IF_ERR_EXIT(dbal_tables_app_id_set(unit, table_id, nof_dbs));

    /** updating the number of DBs in the KBP */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_DBS, INST_SINGLE, nof_dbs);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (db_id != NULL)
    {
        *db_id = nof_dbs;
    }

    /*
     * If the device is locked, update status to indicate that the SW has been changed after device lock
     * and that sync need to be redone in order to apply the updated configurations.
     * Currently it's not allowed to perform sync and lock more than once
     * and trying to apply new configurations after device lock will return error.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate the segments of a DB to the master key segments of an opcode, which perform lookup to this DB.
 *   All DB key segments need to have the same type and size as the segments from the master key they are mapped to
 *   in order for the DB to be valid.
 *   This function also returns error indication if the DB needs to have its segments updated
 *   and if a clone DB need to be used.
 * \param [in]  unit - The Unit number.
 * \param [in]  entry_handle_id - Handle to the OPCODE table.
 * \param [in]  lkp_entry_handle_id - Handle to the OPCODE LOOKUP table.
 * \param [in]  db_id - The DB ID in KBP_DB_INFO of the DB that is being validated.
 * \param [in]  key_type - The type of each opcode master key segment.
 * \param [in]  key_size - The size of each opcode master key segment.
 * \param [in]  key_name - The name of each opcode master key segment.
 * \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *    _SHR_E_EXISTS    - Indicates that the DB exists and its segments need to be updated.
 *    _SHR_E_NOT_FOUND - Indicates that the DB does not match the segments and a clone need to be used.
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
static shr_error_e
kbp_mngr_db_info_validate(
    int unit,
    uint32 entry_handle_id,
    uint32 lkp_entry_handle_id,
    uint8 db_id,
    uint32 key_type[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP],
    uint32 key_size[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP],
    char key_name[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP][DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES])
{
    int index;
    uint32 db_entry_handle_id;
    uint8 db_update = TRUE;
    uint8 db_found = TRUE;
    uint8 nof_db_segments;
    uint32 db_key_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];

    uint32 db_key_type;
    uint32 db_key_size;
    char db_key_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));
    dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, db_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &db_update));
    /** If DB is already valid, no need to update; invert -> valid = !update; !valid = update */
    db_update = !db_update;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, db_entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_db_segments));
    for (index = 0; index < nof_db_segments; index++)
    {
        /** Get the DB key segment index that is mapped to the master key segments */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_SEGMENT_IDS, index, &db_key_index[index]));
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_get
                        (unit, db_entry_handle_id, DNX_KBP_DB_TABLE_INDICATION, index, &db_key_type, &db_key_size,
                         db_key_name));

        /** Validate master key and DB key segments have the same type and size */
        if ((db_key_type != key_type[db_key_index[index]]) || (db_key_size != key_size[db_key_index[index]]))
        {
            char db_name[DBAL_MAX_STRING_LENGTH] = { 0 };
            char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };
            uint8 opcode_id;
            uint8 lookup_index;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                            (unit, db_entry_handle_id, DBAL_FIELD_DB_NAME, INST_SINGLE, (uint8 *) db_name));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                            (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name));
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                            (unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                            (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, &lookup_index));
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "DB (%d) %s on search %d in opcode (%d) %s differ in segments from the master key; segment index %d DB size %d type %d - master key size %d type %d\n",
                         db_id, db_name, lookup_index, opcode_id, opcode_name, db_key_index[index], db_key_size,
                         db_key_type, key_size[db_key_index[index]], key_type[db_key_index[index]]);
        }

        if (db_update == TRUE)
        {
            if (sal_strcmp(db_key_name, DNX_KBP_KEY_SEGMENT_NOT_CONFIGURED) != 0)
            {
                /** DB segment name is configured; DB is initialized */
                db_update = FALSE;
            }
        }
        if (db_found == TRUE)
        {
            if (sal_strcmp(db_key_name, key_name[db_key_index[index]]) != 0)
            {
                /** DB segment name does not match the opcode (master key) segment name */
                db_found = FALSE;
            }
        }
        if ((db_update == FALSE) && (db_found == FALSE))
        {
            /** DB is valid and does not match */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

    if (db_update == TRUE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EXISTS);
        SHR_EXIT();
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Update all valid forwarding DB info using information from opcode info.
 *   Update key segment names, db width and decide if a clone needs to be created for a DB.
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_db_info_update(
    int unit)
{
    uint32 entry_handle_id;
    uint32 db_entry_handle_id;
    uint32 lkp_entry_handle_id;
    int is_end;
    uint8 is_valid = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Create DUMMY DB for HOLE lookups */
    SHR_IF_ERR_EXIT(kbp_mngr_dummy_db_configure(unit));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        int lookup_index;
        int key_index;
        uint8 opcode_id;
        uint32 nof_keys;
        uint32 lookup_bmp;
        uint8 db_id;
        uint32 key_type[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
        uint32 key_size[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
        char key_name[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP][DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { {0} };

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_keys));
        for (key_index = 0; key_index < nof_keys; key_index++)
        {
            SHR_IF_ERR_EXIT(kbp_mngr_key_segment_get
                            (unit, entry_handle_id, DNX_KBP_OPCODE_TABLE_INDICATION, key_index, &key_type[key_index],
                             &key_size[key_index], key_name[key_index]));
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));

        /** Iterate over all valid lookups and validate the lookup DB */
        for (lookup_index = 0; lookup_index < DNX_KBP_MAX_NOF_LOOKUPS; lookup_index++)
        {
            uint8 is_hole_lookup = FALSE;

            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, (uint8) lookup_index);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));

            if (!utilex_bitstream_test_bit(&lookup_bmp, lookup_index))
            {
                if (utilex_bitstream_have_one_in_range(&lookup_bmp, lookup_index + 1, DNX_KBP_MAX_NOF_LOOKUPS - 1))
                {
                    /*
                     * Current lookup is not valid, but is followed by at least one valid lookup.
                     * This indicates hole and will be configured at later stage.
                     */

                    is_hole_lookup = TRUE;
                }
                else
                {
                    /** No other valid lookups. Can exit the loop */
                    break;
                }
            }
            if (!is_hole_lookup)
            {
                shr_error_e res;

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                                (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, &db_id));
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Validating DB ID %d on opcode %d lookup index %d - "), db_id, opcode_id,
                             lookup_index));
                res =
                    kbp_mngr_db_info_validate(unit, entry_handle_id, lkp_entry_handle_id, db_id, key_type, key_size,
                                              key_name);

                if (res == _SHR_E_EXISTS)
                {
                    uint8 lookup_type = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FREE;

                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "update\n")));
                    /** Update the DB with segments from the opcode master key */
                    SHR_IF_ERR_EXIT(kbp_mngr_db_segment_names_update
                                    (unit, entry_handle_id, lkp_entry_handle_id, db_id));

                    /** Mark the DB as valid */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                    (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_TYPE, INST_SINGLE, &lookup_type));

                    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_DB_INFO, db_entry_handle_id));
                    dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
                    dbal_entry_value_field8_set(unit, db_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
                    /** Set the ad_width for DBs on FWD lookup to 64b in order to perform the lookup correctly */
                    if (lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD
                        || lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD_PUBLIC)
                    {
                        dbal_entry_value_field32_set(unit, db_entry_handle_id, DBAL_FIELD_RESULT_WIDTH, INST_SINGLE,
                                                     DNX_KBP_RESULT_SIZE_FWD);
                    }
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, db_entry_handle_id, DBAL_COMMIT));
                }
                else if (res == _SHR_E_NOT_FOUND)
                {
                    /*
                     * Search for other DBs, that are mapped to the same DBAL table (search for existing clones).
                     * Create new clone if no matching DB is found
                     */
                    int db_is_end;
                    uint8 clone_db_id;
                    dbal_tables_e dbal_table;
                    dbal_tables_e clone_dbal_table;

                    /** Get the DBAL table mapped to the lookup DB */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_DB_INFO, db_entry_handle_id));
                    dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                    (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &dbal_table));

                    SHR_IF_ERR_EXIT(dbal_iterator_init
                                    (unit, db_entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, db_entry_handle_id, &db_is_end));

                    while (!db_is_end)
                    {
                        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                                        (unit, db_entry_handle_id, DBAL_FIELD_DB_ID, &clone_db_id));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                        (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE,
                                         &clone_dbal_table));

                        if ((db_id != clone_db_id) && (dbal_table == clone_dbal_table))
                        {
                            /** A different DB to the same DBAL table (possible clone). Validate segments. */
                            res =
                                kbp_mngr_db_info_validate(unit, entry_handle_id, lkp_entry_handle_id, clone_db_id,
                                                          key_type, key_size, key_name);
                            if (res == _SHR_E_NONE)
                            {
                                /** Existing clone DB found */
                                break;
                            }
                        }
                        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, db_entry_handle_id, &db_is_end));
                    }
                    if (res == _SHR_E_NOT_FOUND)
                    {
                        /*
                         * All appropriate DBs were checked and no matching clone DB was found.
                         * Create new clone DB.
                         */
                        SHR_IF_ERR_EXIT(kbp_mngr_db_clone_create(unit, db_id, &clone_db_id));
                        SHR_IF_ERR_EXIT(kbp_mngr_db_segment_names_update
                                        (unit, entry_handle_id, lkp_entry_handle_id, clone_db_id));

                        /** Mark the clone as valid */
                        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_DB_INFO, db_entry_handle_id));
                        dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, clone_db_id);
                        dbal_entry_value_field8_set(unit, db_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, db_entry_handle_id, DBAL_COMMIT));

                        /** Update the opcode DB lookup */
                        dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
                        dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE,
                                                    clone_db_id);
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lkp_entry_handle_id, DBAL_COMMIT));
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "new clone DB ID %d created\n"), clone_db_id));
                    }
                    else if (res == _SHR_E_NONE)
                    {
                        /*
                         * Matching clone DB was found.
                         * Update the lookup info.
                         */
                        dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
                        dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE,
                                                    clone_db_id);
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lkp_entry_handle_id, DBAL_COMMIT));
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "use clone DB ID %d\n"), clone_db_id));
                    }
                    else
                    {
                        SHR_ERR_EXIT(res, "kbp_mngr_db_info_validate failed\n");
                    }
                }
                else if (res != _SHR_E_NONE)
                {
                    SHR_ERR_EXIT(res, "kbp_mngr_db_info_validate failed\n");
                }
                else
                {
                    /** In case of no error, the DB is valid and the segments match the segments in the master key */
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "found\n")));
                }
            }
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sync all Valid DBs to KBP device.
 *   Each entry in KBP_DB_INFO contains all information for creating a KBP DB.
 *   Create a KBP DB or clone DB for all valid entries in KBP_DB_INFO.
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_db_sync(
    int unit)
{
    uint32 entry_handle_id;
    int is_end;
    uint8 is_valid = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 db_id = 0, clone_db = 0;

        int index;
        dbal_table_type_e db_type = 0;
        int kbp_db_type = 0;
        kbp_key_t_p key;
        kbp_db_handles_t db_handles;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_DB_TYPE, INST_SINGLE, &db_type));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DB_ID, &db_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_CLONE_DB, INST_SINGLE, &clone_db));

        if (!SOC_WARM_BOOT(unit))
        {
            uint32 table_size = 0, result_width = 0, key_width = 0, meta_priority = 0, nof_segments = 0;

            sal_memset(&db_handles, 0, sizeof(db_handles));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, &table_size));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_RESULT_WIDTH, INST_SINGLE, &result_width));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_KEY_WIDTH, INST_SINGLE, &key_width));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_META_PRIORITY, INST_SINGLE, &meta_priority));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_segments));

            SHR_IF_ERR_EXIT(kbp_mngr_dbal_db_type_to_kbp_db_type(unit, db_type, &kbp_db_type));

            if (clone_db == 0)
            {
                /** not a clone - init the DB */
                DNX_KBP_TRY_PRINT(kbp_db_init
                                  (Kbp_app_data[unit]->device_p, kbp_db_type, db_id, table_size, &db_handles.db_p),
                                  "DB", db_id);
                DNX_KBP_TRY_PRINT(kbp_ad_db_init
                                  (Kbp_app_data[unit]->device_p, db_id, table_size, result_width, &db_handles.ad_db_p),
                                  "DB", db_id);
                DNX_KBP_TRY_PRINT(kbp_db_set_ad(db_handles.db_p, db_handles.ad_db_p), "DB", db_id);

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U
                             (unit, "Created new DB ID %d type %d, size %d, key_width %db result_width %db %p\n"),
                             db_id, db_type, table_size, key_width, result_width, (void *) db_handles.db_p));
            }
            else
            {
                kbp_db_handles_t cloned_db_handles;
                /** Read DB handles from SW state */
                SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, clone_db, &cloned_db_handles));

                DNX_KBP_TRY_PRINT(kbp_db_clone(cloned_db_handles.db_p, db_id, &(db_handles.db_p)), "DB", db_id);

                db_handles.ad_db_p = cloned_db_handles.ad_db_p;

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "DB ID %d Cloned; new DB ID %d, %p\n"), clone_db, db_id,
                             (void *) db_handles.db_p));
            }

            DNX_KBP_TRY_PRINT(kbp_key_init(Kbp_app_data[unit]->device_p, &key), "DB", db_id);

            /** setting the DB key in KBP according to table definitions */
            for (index = 0; index < nof_segments; index++)
            {
                uint32 field_nof_bytes;
                char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
                int kbp_segment_type;

                SHR_IF_ERR_EXIT(kbp_mngr_key_segment_get
                                (unit, entry_handle_id, DNX_KBP_DB_TABLE_INDICATION, index,
                                 (uint32 *) &kbp_segment_type, &field_nof_bytes, segment_name));

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Adding Segment %d, name %s, size %d type %d\n"), index,
                             segment_name, field_nof_bytes, kbp_segment_type));

                DNX_KBP_TRY_PRINT(kbp_key_add_field(key, segment_name, BYTES2BITS(field_nof_bytes), kbp_segment_type),
                                  segment_name, index);
            }

            DNX_KBP_TRY(kbp_db_set_key(db_handles.db_p, key));
            if (meta_priority != 0)
            {
                int kbp_meta_priority = 0;
                /** Convert meta priority to KBP value, using hard coded mapping */
                SHR_IF_ERR_EXIT(kbp_mngr_kbp_meta_priority_get(unit, meta_priority, &kbp_meta_priority));
                DNX_KBP_TRY(kbp_db_set_property(db_handles.db_p, KBP_PROP_ENTRY_META_PRIORITY, kbp_meta_priority));
            }
            /** Setting the algorithmic property to 0 for the Dummy DB, fixes a device lock failure */
            if (db_id == DNX_KBP_DUMMY_HOLE_DB_ID)
            {
                DNX_KBP_TRY(kbp_db_set_property(db_handles.db_p, KBP_PROP_ALGORITHMIC, 0));
            }
            if (kbp_db_type == KBP_DB_ACL)
            {
                kbp_db_set_property(db_handles.db_p, KBP_PROP_MIN_PRIORITY, DNX_KBP_MIN_ACL_PRIORITY);
            }
        }
        else
        {
            if (db_id != DNX_KBP_DUMMY_HOLE_DB_ID)
            {
                SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, db_id, &db_handles));
                DNX_KBP_TRY(kbp_db_refresh_handle(Kbp_app_data[unit]->device_p, db_handles.db_p, &db_handles.db_p));
                DNX_KBP_TRY(kbp_ad_db_refresh_handle
                            (Kbp_app_data[unit]->device_p, db_handles.ad_db_p, &db_handles.ad_db_p));
            }
        }

        /** save DB handles in SW state */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SW_STATE));
        SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.set(unit, db_id, &db_handles));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SW_STATE));

        if ((db_id != DNX_KBP_DUMMY_HOLE_DB_ID) && (clone_db == 0))
        {
            uint32 dbal_table_id;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &dbal_table_id));

            /** save DB handles in DBAL*/
            SHR_IF_ERR_EXIT(dbal_tables_table_access_info_set
                            (unit, dbal_table_id, DBAL_ACCESS_METHOD_KBP, (void *) &db_handles));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   read the KBP opcode definitions XML and updates the KBP SW info.
 *   the original XML defined by ARCH the opcode XML is created by it by the magic push.
 *   during this process all the opcodes are being loaded to the kbp mnr.
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_fwd_opcode_info_init(
    int unit)
{
    void *catalogue;
    void *opcode;

    SHR_FUNC_INIT_VARS(unit);

    catalogue =
        dbx_file_get_xml_top(unit, DNX_KBP_OPCODES_XML_FILE_PATH, "KbpOpcodesDbCatalogue", CONF_OPEN_PER_DEVICE);
    if (catalogue == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find KBP configuration xml file: %s\n", DNX_KBP_OPCODES_XML_FILE_PATH);
    }

        /** following part is a temporary solution that updates the context to opcode mapping since the following opcodes are
     *  not configured in the pemla. so the table is not updated. once MPLS/ IPMC and NOP will be set in pemla this W.A
     *  can be removed */

        
    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___NOP_CTX,
                                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___NOP_CTX,
                                                   DBAL_ENUM_FVAL_KBP_FWD_OPCODE_NOP));

        
    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B,
                                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B,
                                                   DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_MULTICAST_PRIVATE_W_UC_RPF));

    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B,
                                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B,
                                                   DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_MULTICAST_PRIVATE_W_UC_RPF));

    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B,
                                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B,
                                                   DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_MULTICAST_PRIVATE_W_UC_RPF));

        
    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_W_BF_W_F2B,
                                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_W_BF_W_F2B,
                                                   DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV6_MULTICAST_PRIVATE_W_UC_RPF));

    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B,
                                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B,
                                                   DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV6_MULTICAST_PRIVATE_W_UC_RPF));

    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B,
                                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B,
                                                   DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV6_MULTICAST_PRIVATE_W_UC_RPF));

    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD,
                                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD,
                                                   DBAL_ENUM_FVAL_KBP_FWD_OPCODE_MPLS_GENERAL));

    
    if (SOC_IS_J2C(unit))
    {
        SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC,
                                                       DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC,
                                                       DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_UNICAST_PRIVATE_W_UC_RPF));

        SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC,
                                                       DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC,
                                                       DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV6_UNICAST_PRIVATE_W_UC_RPF));
    }

    /** reading each opcode and updating the info */
    RHDATA_ITERATOR(opcode, catalogue, "Opcode")
    {
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_read(unit, opcode));
    }

exit:
    dbx_xml_top_close(catalogue);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Runs over all DBAL tables and configure according to the information the KBP SW tables.
 * \param [in] unit - Relevant unit.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
kbp_mngr_fwd_db_info_init(
    int unit)
{
    dbal_tables_e table_iter;
    uint8 db_id = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_KBP, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_NONE, &table_iter));
    while (table_iter != DBAL_TABLE_EMPTY)
    {
        db_id++;
        SHR_IF_ERR_EXIT(kbp_mngr_db_info_configure(unit, db_id, 0, table_iter));
        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_iter, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_KBP, DBAL_PHYSICAL_TABLE_NONE,
                         DBAL_TABLE_TYPE_NONE, &table_iter));
    }

    /** updating the number of DBs in the KBP */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_DBS, INST_SINGLE, db_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Updating FWD Applications DB in DBAL SW tables.
*   tables are read from DBAL and the KBP related properties are set in SW
*   LTRs (Opcodes) are read from XML and set in SW
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
static shr_error_e
kbp_mngr_fwd_applications_info_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(kbp_mngr_fwd_db_info_init(unit));
    SHR_IF_ERR_EXIT(kbp_mngr_fwd_opcode_info_init(unit));

exit:
    SHR_FUNC_EXIT;
}


/**
 * See kbp_mngr_sync() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_sync_internal(
    int unit)
{
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;
    uint32 smt_id;
    int smt_id_start;
    int smt_id_end;
    SHR_FUNC_INIT_VARS(unit);

    smt_id_start = 0;
    smt_id_end = (kbp_mngr_smt_enabled(unit) ? DNX_KBP_MAX_NOF_SMTS : 1);

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

        /** for now sync and lock is performed only once */
        if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT)
        {
            /** Suppose to indicate that KBP mngr was never initialized. */
            SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP manager has not performed any initializations\n");
        }
        else if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED)
        {
            /** Changes to SW were made after device lock. Applying them is currently not supported. */
            SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP manager has already performed sync and device lock\n");
        }
        else if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
        {
            /** No changes detected after device lock. Do nothing */
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_SYNC_IN_PROCESS));

        /** Update DB and opcode configuration tables */
        SHR_IF_ERR_EXIT(kbp_mngr_db_info_update(unit));
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_info_update(unit));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_DBAL));
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

        if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
        {
            /** Sync was not performed before warmboot; skip the sync in warmboot. */
            SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_DBAL));
            SHR_EXIT();
        }
        else
        {
            kbp_warmboot_t *warmboot_data = &Kbp_warmboot_data[unit];

            SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_WB_IN_PROCESS));

            DNX_KBP_TRY(kbp_device_restore_state(Kbp_app_data[unit]->device_p,
                                                 warmboot_data->kbp_file_read,
                                                 warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp));
        }
    }

    SHR_IF_ERR_EXIT(kbp_mngr_db_sync(unit));
    for (smt_id = smt_id_start; smt_id < smt_id_end; smt_id++)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_sync(unit, smt_id));
    }

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Performing device Lock %p\n"), Kbp_app_data[unit]->device_p));
    DNX_KBP_TRY(kbp_device_set_property(Kbp_app_data[unit]->device_p, KBP_DEVICE_PRE_CLEAR_ABS, 1));
    DNX_KBP_TRY(kbp_device_lock(Kbp_app_data[unit]->device_p));
    SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED));

    if (SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_DBAL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_init() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_init_internal(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "KBP manager init\n")));
    if (!SOC_WARM_BOOT(unit))
    {
        char *mdb_image_name = NULL;

        
        mdb_image_name = soc_property_suffix_num_str_get(unit, -1, spn_CUSTOM_FEATURE, "device_image");
        if (mdb_image_name == NULL)
        {
            mdb_image_name = "standard_1";
        }

        if (sal_strcmp(mdb_image_name, "standard_1") != 0)
        {
            SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT));
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(KBP_ACCESS.init(unit));

        /** Reading all KBP data from XML (updating DBAL SW tables) */
        SHR_IF_ERR_EXIT(kbp_mngr_fwd_applications_info_init(unit));
        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_FWD_CONFIG_DONE));
    }

    /** No configuration is required in warmboot. All configurations should already be done in normal boot */

exit:
    SHR_FUNC_EXIT;
}

#else
/* *INDENT-OFF* */
/** dummy prototype to return error if KBP not compiled */

#define DUMMY_PROTOTYPE_ERR_MSG(__func_name__)                                                      \
        SHR_FUNC_INIT_VARS(unit);                                                                   \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot perform %s, KBP lib is not compiled", __func_name__); \
        exit:                                                                                       \
        SHR_FUNC_EXIT;

shr_error_e kbp_mngr_db_create_internal(int unit, dbal_tables_e table_id, uint8 clone_id, uint8 *db_id){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_db_create_internal")}
shr_error_e kbp_mngr_init_internal(int unit){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_init_internal")}
shr_error_e kbp_mngr_sync_internal(int unit){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_sync_internal")}
shr_error_e kbp_mngr_context_to_opcode_set(int unit,uint8 fwd_context,uint8 acl_context,uint8 opcode_id){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_context_to_opcode_set")}
/* *INDENT-ON* */
#endif
