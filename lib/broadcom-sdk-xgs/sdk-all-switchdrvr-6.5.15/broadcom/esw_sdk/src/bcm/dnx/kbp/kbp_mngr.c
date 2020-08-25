/** \file kbp_mngr.c
 *
 *  kbp procedures for DNX.
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

#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include <bcm/types.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/drv.h> 
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_field_types_defs.h>

#include "kbp_mngr_internal.h"
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */

/*
 * GLOBALs
 * {
 */

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

extern shr_error_e dbal_entry_handle_update_field_ids(
    int unit,
    uint32 entry_handle_id);

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (g_kbp_compiled && dnx_kbp_device_enabled(unit))
    {
        SHR_IF_ERR_EXIT(kbp_mngr_init_internal(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (g_kbp_compiled && dnx_kbp_device_enabled(unit))
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_internal.h
 */
shr_error_e
kbp_mngr_opcode_printable_entry_get(
    int unit,
    uint32 flags,
    uint8 fwd_context,
    uint8 acl_context,
    uint32 *key_sig_value,
    int key_sig_size,
    uint32 *res_sig_value,
    int res_sig_size,
    uint8 *nof_print_info,
    kbp_printable_entry_t * entry_print_info)
{
    uint32 entry_handle_id;
    uint8 is_valid = FALSE;
    uint8 opcode_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_translate(unit, fwd_context, acl_context, &opcode_id));

    /** Value 2 is for FWD and RPF lookups */
    sal_memset(entry_print_info, 0x0, 2 * sizeof(kbp_printable_entry_t));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (!is_valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Opcode %d is not valid\n", opcode_id);
    }

    /** Key parsing */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_printable_entry_key_parsing
                    (unit, opcode_id, key_sig_value, key_sig_size, entry_print_info));
    /** Result parsing */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_printable_entry_result_parsing
                    (unit, flags, opcode_id, res_sig_value, res_sig_size, nof_print_info, entry_print_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h 
*/
shr_error_e
kbp_mngr_opcode_to_contexts_get(
    int unit,
    uint8 opcode_id,
    uint8 *fwd_nof_contexts,
    kbp_mngr_fwd_acl_context_mapping_t fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE])
{
    uint32 entry_handle_id;
    int ctx_index;
    uint32 ctx_id;
    uint32 nof_ctxs;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fwd_nof_contexts, _SHR_E_PARAM, "fwd_nof_contexts");
    SHR_NULL_CHECK(fwd_acl_ctx_mapping, _SHR_E_PARAM, "fwd_acl_ctx_mapping");

    sal_memset(fwd_acl_ctx_mapping, 0,
               (sizeof(fwd_acl_ctx_mapping[0]) * DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_OPCODE_FWD_CONTEXTS, INST_SINGLE, &nof_ctxs));

    if (nof_ctxs == 0)
    {
        (*fwd_nof_contexts) = nof_ctxs;
        SHR_EXIT();
    }

    for (ctx_index = 0; ctx_index < nof_ctxs; ctx_index++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_FWD_CONTEXT, ctx_index, &ctx_id));

        fwd_acl_ctx_mapping[ctx_index].fwd_context = ctx_id;
        
        if (opcode_id >= DNX_KBP_ACL_CONTEXT_BASE)
        {
            fwd_acl_ctx_mapping[ctx_index].acl_contexts[0] = opcode_id;
        }
        else
        {
            fwd_acl_ctx_mapping[ctx_index].acl_contexts[0] = ctx_id;
        }
        fwd_acl_ctx_mapping[ctx_index].nof_acl_contexts = 1;
    }

    (*fwd_nof_contexts) = nof_ctxs;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_context_to_opcode_get(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 *opcode_id,
    char *opcode_name)
{
    uint32 entry_handle_id;
    uint8 is_valid = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_translate(unit, fwd_context, acl_context, opcode_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, (*opcode_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (opcode_name)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

        if (!is_valid)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode (%d) not valid\n", *opcode_id);
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_sync(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    if (dnx_kbp_device_enabled(unit))
    {
        SHR_IF_ERR_EXIT(kbp_mngr_sync_internal(unit));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device is not enabled\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_db_create(
    int unit,
    dbal_tables_e dbal_table_id)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_db_create_internal(unit, dbal_table_id, 0, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_status_update(
    int unit,
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    /** Updating KBP device status */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_STATUS, INST_SINGLE, kbp_mngr_status);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_status_get(
    int unit,
    dbal_enum_value_field_kbp_device_status_e * kbp_mngr_status)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    /** Get the KBP device status */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_KBP_STATUS, INST_SINGLE, kbp_mngr_status);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_ipv4_public_enabled_get(
    int unit,
    uint8 *enabled)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    /** Get the KBP device status */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IPV4_PUBLIC_ENABLED, INST_SINGLE, enabled);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_ipv6_public_enabled_get(
    int unit,
    uint8 *enabled)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    /** Get the KBP device status */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IPV6_PUBLIC_ENABLED, INST_SINGLE, enabled);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_db_id_by_name_get(
    int unit,
    char *name,
    uint8 *db_id)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_db_id_by_name_get_internal(unit, name, db_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_total_result_size_get(
    int unit,
    uint8 opcode_id,
    uint32 *result_size)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_total_result_size_get_internal(unit, opcode_id, result_size));

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_master_key_set(
    int unit,
    uint8 opcode_id,
    uint32 nof_segments,
    kbp_mngr_key_segment_t * key_segments)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_set_internal(unit, opcode_id, nof_segments, key_segments));

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_master_key_get(
    int unit,
    uint32 opcode_id,
    uint32 *nof_key_segments,
    kbp_mngr_key_segment_t * key_segments)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get_internal(unit, opcode_id, nof_key_segments, key_segments));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_master_key_segments_add(
    int unit,
    uint32 opcode_id,
    uint32 nof_new_segments,
    kbp_mngr_key_segment_t * key_segments)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add_internal(unit, opcode_id, nof_new_segments, key_segments));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_lookup_add(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_add_internal(unit, opcode_id, lookup_info));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_lookup_get(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_get_internal(unit, opcode_id, lookup_info));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_result_clear(
    int unit,
    uint8 opcode_id,
    uint8 result_id)
{
    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_result_clear_internal(unit, opcode_id, result_id));
exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_opcode_clear(
    int unit,
    uint8 opcode_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   allocate the next available opcode.
*   \param [in] unit - Relevant unit.
*   \param [in] new_opcode_id - the opcode Id to use.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
static shr_error_e
kbp_mngr_next_available_opcode_get(
    int unit,
    uint8 *new_opcode_id)
{
    uint32 entry_handle_id;
    int is_end;
    uint8 is_valid = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));

        /** DBAL_ITER_MODE_ALL used here to get all entries (also default entries) - we are looking for an invalid entry */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid, NULL));

        /** Taking the first opcode from number 32. (reserve the first opcodes to fwd opcodes) */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, DBAL_CONDITION_BIGGER_THAN,
                     DNX_KBP_DYNAMIC_OPCODE_BASE - 1, 0));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    if (is_end)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "No available opcodes to allocate\n");
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, new_opcode_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   allocate the next available context to be used for ACL dynamic context.
*   \param [in] unit - Relevant unit.
*   \param [in] acl_ctx - the returned acl_ctx to use.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
static shr_error_e
kbp_mngr_next_available_context_get(
    int unit,
    uint8 *acl_ctx)
{
    uint32 entry_handle_id;
    uint32 acl_ctx_status = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    (*acl_ctx) = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_STATUS, INST_SINGLE, &acl_ctx_status));

    for (index = 0; index < DNX_KBP_NOF_FWD_CONTEXTS; index++)
    {
        if (!utilex_bitstream_test_bit(&acl_ctx_status, index))
        {
            (*acl_ctx) = DNX_KBP_ACL_CONTEXT_BASE + index;
            break;
        }
    }

    if (index == DNX_KBP_NOF_FWD_CONTEXTS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No ACL contexts available\n");
    }

    utilex_bitstream_set_bit(&acl_ctx_status, index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_STATUS, INST_SINGLE, acl_ctx_status);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
kbp_mngr_opcode_create(
    int unit,
    uint8 source_opcode_id,
    char *opcode_name,
    uint8 *acl_ctx,
    uint8 *new_opcode_id)
{
    uint32 entry_handle_id, ii;
    uint8 is_source_opcode_valid = 0;
    uint8 nof_fwd_contexts = 0;
    kbp_mngr_fwd_acl_context_mapping_t fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    (*new_opcode_id) = 0;
    (*acl_ctx) = 0;

        /** getting info of the source opcode */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, source_opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_source_opcode_valid));

    if (!is_source_opcode_valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "source opcode (%d) is not valid, cannot allocate new opcode\n", source_opcode_id);
    }

    SHR_IF_ERR_EXIT(kbp_mngr_next_available_opcode_get(unit, new_opcode_id));

    SHR_IF_ERR_EXIT(kbp_mngr_next_available_context_get(unit, acl_ctx));

        /** setting generic opcode parameters. */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, (*new_opcode_id));
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name);

        /** Update the dbal handle to set all the fields in the handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_update_field_ids(unit, entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** duplicate lookup info table */
    for (ii = 0; ii < DNX_KBP_HIT_INDICATION_SIZE_IN_BITS; ii++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, source_opcode_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ID, ii);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, (*new_opcode_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_update_field_ids(unit, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

        /** duplicate result info table */
    for (ii = 0; ii < DNX_KBP_HIT_INDICATION_SIZE_IN_BITS; ii++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, source_opcode_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_ID, ii);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, (*new_opcode_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_update_field_ids(unit, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_contexts_get(unit, source_opcode_id, &nof_fwd_contexts, fwd_acl_ctx_mapping));

        
    if (nof_fwd_contexts == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "source opcode (%d) mapped to zero contexts \n", source_opcode_id);
    }

        /** setting the context to opcode mapping table */
    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set
                    (unit, fwd_acl_ctx_mapping[0].fwd_context, (*acl_ctx), (*new_opcode_id)));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "dynamic opcode created %s (%d) acl context %d cascaded from %s (%d)\n"), opcode_name,
                 (*new_opcode_id), (*acl_ctx), dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_KBP_FWD_OPCODE,
                                                                           source_opcode_id), source_opcode_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_key_segment_t_init(
    int unit,
    kbp_mngr_key_segment_t * segment_p)
{
    SHR_FUNC_INIT_VARS(unit);

    segment_p->nof_bytes = DNX_KBP_INVALID_SEGMENT_SIZE;
    sal_memset(segment_p->name, 0, sizeof(segment_p->name));

    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_opcode_lookup_info_t_init(
    int unit,
    kbp_opcode_lookup_info_t * lookup_info_p)
{
    uint32 idx;

    SHR_FUNC_INIT_VARS(unit);

    lookup_info_p->result_index = DNX_KBP_INVALID_RESULT_ID;
    lookup_info_p->dbal_table_id = DNX_KBP_INVALID_DB_ID;
    lookup_info_p->nof_segments = 0;
    lookup_info_p->result_offset = 0;
    lookup_info_p->result_size = 0;

    for (idx = 0; idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP; idx++)
    {
        lookup_info_p->key_segment_index[idx] = DNX_KBP_INVALID_SEGMENT_ID;
    }
    SHR_FUNC_EXIT;
}

#if defined(INCLUDE_KBP)

extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];
extern kbp_warmboot_t Kbp_warmboot_data[SOC_MAX_NUM_DEVICES];
extern FILE *Kbp_file_fp[SOC_MAX_NUM_DEVICES];

/**
* See kbp_mngr.h
*/
shr_error_e
kbp_mngr_wb_sync(
    int unit)
{
    kbp_warmboot_t *warmboot_data;

    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    if (dnx_kbp_device_enabled(unit))
    {
        dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

        /** If KBP sync was not performed, skip KBP warmboot sync */
        if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
        {
            SHR_EXIT();
        }

        warmboot_data = &Kbp_warmboot_data[unit];

        DNX_KBP_TRY(kbp_device_save_state
                    (Kbp_app_data[unit]->device_p, warmboot_data->kbp_file_read, warmboot_data->kbp_file_write,
                     warmboot_data->kbp_file_fp));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Open a file to facilitate KBPSDK warmboot.
 */
static shr_error_e
kbp_mngr_wb_file_open(
    int unit,
    char *filename,
    int device_type)
{
    int is_warmboot;
    char prefixed_file_name[SOC_PROPERTY_NAME_MAX + 256];
    char *stable_filename = NULL;

    FILE **file_fp = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == filename)
    {
        return 0;
    }

    if ((device_type == KBP_DEVICE_OP) || (device_type == KBP_DEVICE_OP2))
    {
        file_fp = &Kbp_file_fp[unit];
    }

    if (*file_fp == NULL)
    {
        is_warmboot = SOC_WARM_BOOT(unit);

        sal_memset(prefixed_file_name, 0, sizeof(prefixed_file_name));

        stable_filename = soc_property_get_str(unit, spn_STABLE_FILENAME);

        /*
         * assuming stable_filename is unique for each separate run
         */
        if (NULL != stable_filename)
        {
            sal_strncat(prefixed_file_name, stable_filename, sizeof(prefixed_file_name) - 1);

            sal_strncat(prefixed_file_name, "_", sizeof(prefixed_file_name) - sal_strlen(prefixed_file_name) - 1);
        }
        sal_strncat(prefixed_file_name, filename, sizeof(prefixed_file_name) - sal_strlen(prefixed_file_name) - 1);

        if ((*file_fp = sal_fopen(prefixed_file_name, is_warmboot != 0 ? "r+" : "w+")) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error:  sal_fopen() Failed\n");
        }
    }

    Kbp_warmboot_data[unit].kbp_file_fp = Kbp_file_fp[unit];
    Kbp_warmboot_data[unit].kbp_file_read = &dnx_kbp_file_read_func;
    Kbp_warmboot_data[unit].kbp_file_write = &dnx_kbp_file_write_func;

exit:
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr.h
*/
uint8
kbp_mngr_smt_enabled(
    int unit)
{
    return ((Kbp_app_data[unit]->device_type == KBP_DEVICE_OP2) ? TRUE : FALSE);
}

/**
 * See kbp_mngr.h
 */
shr_error_e
dnx_kbp_device_init(
    int unit)
{
    uint32 flags = Kbp_app_data[unit]->flags;
    uint8 kbp_device_connect_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    flags |= KBP_DEVICE_ISSU /** this falg is used for WB support */ ;

    if (SOC_WARM_BOOT(unit))
    {
        flags |= KBP_DEVICE_SKIP_INIT;
    }
    if (kbp_mngr_smt_enabled(unit))
    {
        flags |= KBP_DEVICE_SMT;
    }

    switch (dnx_data_elk.connectivity.connect_mode_get(unit))
    {
        case DNX_NIF_ELK_TCAM_CONNECT_MODE_SINGLE:
        {
            kbp_device_connect_mode = DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE;
            break;
        }
        case DNX_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED:
        {
            kbp_device_connect_mode = DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SHARED;
            break;
        }
        case DNX_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT:
        {
            kbp_device_connect_mode = DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SMT;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported KBP device connect mode provided\n");
        }
    }

    SHR_IF_ERR_EXIT(kbp_mngr_wb_file_open(unit, "kbp", Kbp_app_data[unit]->device_type));

    if (!Kbp_app_data[unit]->dalloc_p)
    {
        SHR_ERR_EXIT(_SHR_E_INIT, " default allocator was not performed.\n");
    }
    DNX_KBP_TRY(kbp_device_init
                (Kbp_app_data[unit]->dalloc_p, Kbp_app_data[unit]->device_type, flags,
                 (struct kbp_xpt *) Kbp_app_data[unit]->xpt_p, NULL, &Kbp_app_data[unit]->device_p));

    if (kbp_mngr_smt_enabled(unit))
    {
        DNX_KBP_TRY(kbp_device_thread_init(Kbp_app_data[unit]->device_p, 0, &Kbp_app_data[unit]->smt_p[0]));
        DNX_KBP_TRY(kbp_device_thread_init(Kbp_app_data[unit]->device_p, 1, &Kbp_app_data[unit]->smt_p[1]));
    }

    if (!SOC_WARM_BOOT(unit))
    {
        uint32 entry_handle_id;

        /** Update the device connect mode */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEVICE_CONNECT_MODE, INST_SINGLE,
                                     kbp_device_connect_mode);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE));
    }
    else
    {
        DNX_KBP_TRY(kbp_mngr_sync(unit));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr.h
 */
shr_error_e
dnx_kbp_device_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    if ((Kbp_app_data[unit] == NULL) || (Kbp_app_data[unit]->device_p == NULL))
    {
        /*
         * Nothing needs to deinit, exit directly.
         */
        SHR_EXIT();
    }

    DNX_KBP_TRY(kbp_device_destroy(Kbp_app_data[unit]->device_p));

exit:
    SHR_FUNC_EXIT;
}

#endif
