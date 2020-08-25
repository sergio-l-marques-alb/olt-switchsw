/*
 * $Id: dbal_kbp_access.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include "dbal_internal.h"
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/cmic.h>
#include <sal/types.h>

#if defined(INCLUDE_KBP)
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/swstate/auto_generated/access/kbp_fwd_tcam_access_mapper_access.h>

/**
 * \brief - The function prints the key and result of a physical
 *        entry.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \param [in] print_only_key - Indication to skip the printing
 *        of the payload.
 * \param [in] action - String that adds additional information
 *        to the print.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_kbp_physical_entry_print(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 print_only_key,
    char *action)
{
    int index, bsl_severity;
    int key_size_in_words = BITS2WORDS(entry_handle->phy_entry.key_size);
    int payload_size_in_words = BITS2WORDS(entry_handle->phy_entry.payload_size);
    
    uint8 is_acl = (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT ? TRUE : FALSE);
    int logger_action = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    if (bsl_severity >= bslSeverityInfo)
    {
        if (dbal_logger_is_enable(unit, entry_handle->table_id))
        {
            if ((!sal_strcasecmp(action, "get from")) || (!sal_strcasecmp(action, "get next from"))
                || (!sal_strcasecmp(action, "Access ID get")))
            {
                logger_action = 1;
            }
            else if (!sal_strcasecmp(action, "delete from"))
            {
                logger_action = 2;
            }
            else
            {
                logger_action = 0;
            }

            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_KBP, logger_action);

            /** Physical table */
            LOG_CLI((BSL_META("Entry %s db_p %p ad_db_p %p\n"),
                     action,
                     ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->db_p,
                     ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->ad_db_p));

            /** Print key / mask / prefix / priority */
            LOG_CLI((BSL_META("Phy. entry buffer:\n")));
            LOG_CLI((BSL_META("Key(%3d bits): 0x"), entry_handle->phy_entry.key_size));
            for (index = key_size_in_words - 1; index >= 0; index--)
            {
                LOG_CLI((BSL_META("%08x"), entry_handle->phy_entry.key[index]));
            }

            if (is_acl)
            {
                LOG_CLI((BSL_META("\n")));
                LOG_CLI((BSL_META("Key Mask     : 0x")));
                for (index = key_size_in_words - 1; index >= 0; index--)
                {
                    LOG_CLI((BSL_META("%08x"), entry_handle->phy_entry.k_mask[index]));
                }
            }

            LOG_CLI((BSL_META(" %s %d\n"), (is_acl ? "Priority" : "/"), entry_handle->phy_entry.prefix_length));

            if (!print_only_key)
            {
                /** Print payload */
                LOG_CLI((BSL_META("Payload(%3d bits): 0x"), entry_handle->phy_entry.payload_size));
                for (index = payload_size_in_words - 1; index >= 0; index--)
                {
                    LOG_CLI((BSL_META("%08x"), entry_handle->phy_entry.payload[index]));
                }
                LOG_CLI((BSL_META("\n")));
            }
            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_KBP, logger_action);
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 * this function resolve the result type of an entry.
 * If no error occur, the cur_res_type on entry handle will be update.
 * If cannot find match result type, _SHR_E_NOT_FOUND is returned
 */
shr_error_e
dbal_kbp_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int jj;
    int curr_res_type = 0;
    uint32 field_value[1] = { 0 };
    dbal_logical_table_t *table = entry_handle->table;
    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);

    while (curr_res_type < table->nof_result_types)
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id,
                                                   DBAL_FIELD_RESULT_TYPE, 0, curr_res_type, 0, &table_field_info));

        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, &table_field_info, DBAL_FIELD_RESULT_TYPE,
                                                   entry_handle->phy_entry.payload, field_value));

        for (jj = 0; jj < entry_handle->table->multi_res_info[curr_res_type].result_type_nof_hw_values; jj++)
        {
            if (entry_handle->table->multi_res_info[curr_res_type].result_type_hw_value[jj] == field_value[0])
            {
                entry_handle->cur_res_type = curr_res_type;
                SHR_EXIT();
            }
        }
        curr_res_type++;
    }
    if (curr_res_type == table->nof_result_types)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Used to fill in the hash key using the info in the entry handle.
 * Returns the hash key and IPv4/6 index (IPv4=0; IPv6=1).
 */
static shr_error_e
dbal_kbp_access_id_hash_key_fill(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dnx_kbp_fwd_tcam_access_hash_key_t * hash_key,
    uint32 *table_index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(hash_key, _SHR_E_PARAM, "hash_key");
    SHR_NULL_CHECK(table_index, _SHR_E_PARAM, "table_index");
    sal_memset(hash_key, 0, sizeof(dnx_kbp_fwd_tcam_access_hash_key_t));

    if (entry_handle->table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD)
    {
        *table_index = DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV4_MC;
    }
    else if (entry_handle->table_id == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD)
    {
        *table_index = DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV6_MC;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "get_access_id supported only for KBP IPv4/6 MC TCAM tables\n");
    }

    SHR_BITCOPY_RANGE(hash_key->key, 0, entry_handle->phy_entry.key, 0, entry_handle->table->key_size);
    SHR_BITCOPY_RANGE(hash_key->key_mask, 0, entry_handle->phy_entry.k_mask, 0, entry_handle->table->key_size);
    SHR_BITAND_RANGE(hash_key->key, hash_key->key_mask, 0, entry_handle->table->key_size, hash_key->key);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Get the entry access ID that corresponds to the key in the entry handle.
 */
shr_error_e
dbal_kbp_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 access_id = 0;
    uint32 table_index;
    uint8 found = FALSE;
    dnx_kbp_fwd_tcam_access_hash_key_t hash_key = { {0}, {0} };

    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM &&
        entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "get_access_id supported only for TCAM\n");
    }

    SHR_IF_ERR_EXIT(dbal_kbp_access_id_hash_key_fill(unit, entry_handle, &hash_key, &table_index));
    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "access ID get"));
    SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.find(unit, table_index, &hash_key, &access_id, &found));

    if (!found)
    {
        entry_handle->phy_entry.entry_hw_id = 0;
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    else
    {
        entry_handle->phy_entry.entry_hw_id = (int) access_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Create new entry access ID to the key in the entry handle
 * Internal for dbal_kbp_access
 */
static shr_error_e
dbal_kbp_entry_access_id_create(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 access_id)
{
    uint32 table_index;
    uint8 success = FALSE;
    dnx_kbp_fwd_tcam_access_hash_key_t hash_key = { {0}, {0} };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_kbp_access_id_hash_key_fill(unit, entry_handle, &hash_key, &table_index));
    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "access ID create"));
    SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.insert(unit, table_index, &hash_key, &access_id, &success));

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Failed to create new access_id %d\n", access_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Destroy the entry access ID that corresponds to the key in the entry handle
 * Internal for dbal_kbp_access
 */
static shr_error_e
dbal_kbp_entry_access_id_destroy(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 table_index;
    dnx_kbp_fwd_tcam_access_hash_key_t hash_key = { {0}, {0} };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_kbp_access_id_hash_key_fill(unit, entry_handle, &hash_key, &table_index));
    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "access ID destroy"));
    SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.delete(unit, table_index, &hash_key));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function retrieves the association data of a
 *        given entry in an LPM database in the external TCAM.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_kbp_entry_lpm_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int res;
    kbp_db_t_p db_p = NULL;
    kbp_ad_db_t_p ad_db_p = NULL;
    uint8 hitbit = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_RETRIEVE_HANDLES_FROM_DBAL_ENTRY(entry_handle, db_p, ad_db_p);

    res = dnx_kbp_entry_lpm_get(unit,
                                db_p,
                                ad_db_p,
                                NULL,
                                entry_handle->phy_entry.key,
                                entry_handle->table->key_size,
                                entry_handle->phy_entry.payload,
                                entry_handle->table->max_payload_size, entry_handle->phy_entry.prefix_length, &hitbit);

    if (res == _SHR_E_NOT_FOUND)
    {
                /** Exit without error print. */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "get from"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function adds an LPM entry in the external TCAM.
 *        It also performs entry update according to the entry
 *        update indication in the entry handle. Rollbacks are
 *        performed when the entry fails to be added.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_kbp_entry_lpm_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_db_t_p db_p = NULL;
    kbp_ad_db_t_p ad_db_p = NULL;
    uint32 is_default;
    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_RETRIEVE_HANDLES_FROM_DBAL_ENTRY(entry_handle, db_p, ad_db_p);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, entry_handle->table_id, DBAL_FIELD_IS_DEFAULT, 0, entry_handle->cur_res_type, 0,
                     &table_field_info));
    SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                    (unit, &table_field_info, DBAL_FIELD_IS_DEFAULT, entry_handle->phy_entry.payload,
                     &is_default));

    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "add to"));

    SHR_IF_ERR_EXIT(dnx_kbp_entry_lpm_add(unit,
                                          db_p,
                                          ad_db_p,
                                          NULL,
                                          entry_handle->phy_entry.key,
                                          entry_handle->table->key_size,
                                          entry_handle->phy_entry.payload,
                                          entry_handle->table->max_payload_size,
                                          entry_handle->phy_entry.prefix_length,
                                          entry_handle->is_entry_update,
                                          FALSE,
                                          (uint8) is_default));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function deletes an LPM entry from the external
 *        TCAM.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_kbp_entry_lpm_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_db_t_p db_p = NULL;
    kbp_ad_db_t_p ad_db_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_RETRIEVE_HANDLES_FROM_DBAL_ENTRY(entry_handle, db_p, ad_db_p);
    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "delete from"));

    SHR_IF_ERR_EXIT(dnx_kbp_entry_lpm_delete(unit,
                                             db_p,
                                             ad_db_p,
                                             NULL,
                                             entry_handle->phy_entry.key,
                                             entry_handle->table->key_size, entry_handle->phy_entry.prefix_length));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function retrieves the data, mask and association
 *        data of a given entry in entry_handle->phy_entry.entry_hw_id
 *        from an ACL database in the external TCAM.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_kbp_entry_acl_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_db_t_p db_p = NULL;
    kbp_ad_db_t_p ad_db_p = NULL;
    struct kbp_entry *db_entry = (struct kbp_entry *) INT_TO_PTR(entry_handle->phy_entry.entry_hw_id);
    struct kbp_entry_info entry_info;
    uint8 data[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 asso_data[DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES] = { 0 };
    uint32 inverted_mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = {0};
    kbp_status kbp_rv;

    SHR_FUNC_INIT_VARS(unit);

    /** cannot perform get, it means that the entry was not found */
    if (entry_handle->access_id_set == 0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    DNX_KBP_RETRIEVE_HANDLES_FROM_DBAL_ENTRY(entry_handle, db_p, ad_db_p);

    kbp_rv = kbp_entry_get_info(db_p, db_entry, &entry_info);
    if (kbp_rv != KBP_OK)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    sal_memcpy(data, entry_info.data, sizeof(uint8) * (entry_info.width_8));
    sal_memcpy(mask, entry_info.mask, sizeof(uint8) * (entry_info.width_8));
    DNX_KBP_TRY(kbp_ad_db_get(ad_db_p, entry_info.ad_handle, asso_data));

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, entry_handle->table->key_size, data, entry_handle->phy_entry.key));
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, entry_handle->table->key_size, mask, inverted_mask));
    /*
     * The mask in the KBP takes the opposite convention to DBAL (in KBP 1 is "don't care", in DBAL 0 is "don't care".
     * We invert the mask.
     */
    SHR_BITNEGATE_RANGE(inverted_mask, 0, entry_handle->table->key_size, entry_handle->phy_entry.k_mask);

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal
                    (unit, entry_handle->table->max_payload_size, asso_data, entry_handle->phy_entry.payload));

    entry_handle->phy_entry.prefix_length = entry_info.prio_len;
    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "get from"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function deletes an ACL entry from the external
 *        TCAM. The entry is provided in entry_handle->phy_entry.entry_hw_id.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_kbp_entry_acl_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_db_t_p db_p = NULL;
    kbp_ad_db_t_p ad_db_p = NULL;

    struct kbp_entry *db_entry = (struct kbp_entry *) INT_TO_PTR(entry_handle->phy_entry.entry_hw_id);
    struct kbp_entry_info entry_info;

    SHR_FUNC_INIT_VARS(unit);

    /** cannot perform get, it means that the entry was not found */
    if (entry_handle->access_id_set == 0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    DNX_KBP_RETRIEVE_HANDLES_FROM_DBAL_ENTRY(entry_handle, db_p, ad_db_p);

    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "delete from"));

    DNX_KBP_TRY(kbp_entry_get_info(db_p, db_entry, &entry_info));

    DNX_KBP_TRY(kbp_db_delete_entry(db_p, db_entry));
    DNX_KBP_TRY(kbp_ad_db_delete_entry(ad_db_p, entry_info.ad_handle));
    DNX_KBP_TRY(kbp_db_install(db_p));

    /** Destroy entry access ID only for IPMC */
    if (entry_handle->table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD
        || entry_handle->table_id == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD)
    {
        SHR_IF_ERR_EXIT(dbal_kbp_entry_access_id_destroy(unit, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function adds an ACL entry in the external TCAM.
 *        It also performs entry update according to the entry
 *        update indication in the entry handle. Rollbacks are
 *        performed when the entry fails to be added.
 *        The function returns the created db_entry in
 *        entry_handle->phy_entry.entry_hw_id.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_kbp_entry_acl_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int kbp_ret_val;
    kbp_db_t_p db_p = NULL;
    kbp_ad_db_t_p ad_db_p = NULL;
    struct kbp_entry *db_entry = NULL;
    struct kbp_ad *ad_entry = NULL;
    uint8 data[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 asso_data[DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES] = { 0 };
    uint32 inverted_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_RETRIEVE_HANDLES_FROM_DBAL_ENTRY(entry_handle, db_p, ad_db_p);

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, entry_handle->table->key_size, entry_handle->phy_entry.key, data));
    /*
     * The mask in the KBP takes the opposite convention to DBAL (in KBP 1 is "don't care", in DBAL 0 is "don't care".
     * We invert the mask.
     */
    SHR_BITNEGATE_RANGE(entry_handle->phy_entry.k_mask, 0, entry_handle->table->key_size, inverted_mask);
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, entry_handle->table->key_size, inverted_mask, mask));

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp
                    (unit, entry_handle->table->max_payload_size, entry_handle->phy_entry.payload, asso_data));

    /** it means that the access id should be retrieve by the KBP_SDK */
    if (entry_handle->access_id_set == 0)
    {
        if (entry_handle->is_entry_update)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "cannot update entry - not found:\n");
        }
        DNX_KBP_TRY(kbp_db_add_ace(db_p, data, mask, entry_handle->phy_entry.prefix_length, &db_entry));
    }
    else
    {
        if (entry_handle->is_entry_update)
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_acl_delete(unit, entry_handle));
        }
        /**  here we use new API that add to entry to specific location kbp_db_add_ace_with_index, the index is the db_entry */
        DNX_KBP_TRY(kbp_db_add_ace_with_index
                    (db_p, data, mask, entry_handle->phy_entry.prefix_length, entry_handle->phy_entry.entry_hw_id));
        db_entry = INT_TO_PTR(entry_handle->phy_entry.entry_hw_id);
    }

    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "add to"));

    kbp_ret_val = kbp_ad_db_add_entry(ad_db_p, asso_data, &ad_entry);
    if (KBP_TO_DNX_RESULT(kbp_ret_val) != _SHR_E_NONE)
    {
        /** Rollback */
        kbp_db_delete_entry(db_p, db_entry);
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "kbp_ad_db_add_entry failed: %s\n", kbp_get_status_string(kbp_ret_val));
    }

    kbp_ret_val = kbp_entry_add_ad(db_p, db_entry, ad_entry);
    if (KBP_TO_DNX_RESULT(kbp_ret_val) != _SHR_E_NONE)
    {
        /** Rollback */
        kbp_db_delete_entry(db_p, db_entry);
        kbp_ad_db_delete_entry(ad_db_p, ad_entry);
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "kbp_entry_add_ad failed: %s\n", kbp_get_status_string(kbp_ret_val));
    }

    kbp_ret_val = kbp_db_install(db_p);
    if (KBP_TO_DNX_RESULT(kbp_ret_val) != _SHR_E_NONE)
    {
        /** Rollback */
        kbp_db_delete_entry(db_p, db_entry);
        kbp_ad_db_delete_entry(ad_db_p, ad_entry);

        SHR_ERR_EXIT(_SHR_E_INTERNAL, "kbp_db_install failed: %s\n", kbp_get_status_string(kbp_ret_val));
    }

    if (entry_handle->access_id_set == 0)
    {
        void *tmp;
        /*
         * error: cast from pointer to integer of different size 
         */
        tmp = (void *) (db_entry);
        entry_handle->phy_entry.entry_hw_id = PTR_TO_INT(tmp);
    }

    /*
     * Create entry access ID only for IPMC.
     * In case the entry was updated, the old key was already deleted.
     */
    if (entry_handle->table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD
        || entry_handle->table_id == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD)
    {
        SHR_IF_ERR_EXIT(dbal_kbp_entry_access_id_create(unit, entry_handle, entry_handle->phy_entry.entry_hw_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function retrieves the payload of an LPM or ACL
 *        entry from the external TCAM according to the database
 *        type.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_kbp_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    shr_error_e res;
    SHR_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_LPM:
        {
            res = dbal_kbp_entry_lpm_get(unit, entry_handle);
            break;
        }
        
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
        {
            res = dbal_kbp_entry_acl_get(unit, entry_handle);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL table type %s not supported for KBP\n",
                         dbal_table_type_to_string(unit, entry_handle->table->table_type));
        }
    }

    if (res != _SHR_E_NONE)
    {
        if (res == _SHR_E_NOT_FOUND)
        {
            /** Exit without error print */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        else
        {
            
            SHR_ERR_EXIT(res, "dbal_kbp_entry_%s_get failed\n",
                         (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT ? "acl" : "lpm"));
        }
    }

    if (entry_handle->table->has_result_type)
    {
        SHR_SET_CURRENT_ERR(dbal_kbp_res_type_resolution(unit, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function adds an LPM or ACL entry to the
 *        external TCAM according to the database type.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_kbp_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_LPM:
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_lpm_add(unit, entry_handle));
            break;
        }
        
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_acl_add(unit, entry_handle));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL table type %d not supported for KBP\n",
                         entry_handle->table->table_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function deletes an LPM or ACL entry from the
 *        external TCAM according to the database type.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_kbp_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_LPM:
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_lpm_delete(unit, entry_handle));
            break;
        }
        
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_acl_delete(unit, entry_handle));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL table type %d not supported for KBP\n",
                         entry_handle->table->table_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function deletes all entries from a database in
 *        the external TCAM.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_kbp_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_db_t_p db_p = NULL;
    kbp_ad_db_t_p ad_db_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Retrieve KBP handles */
    DNX_KBP_RETRIEVE_HANDLES_FROM_DBAL_ENTRY(entry_handle, db_p, ad_db_p);

    DNX_KBP_TRY(kbp_db_delete_all_entries(db_p));
    DNX_KBP_TRY(kbp_ad_db_delete_all_entries(ad_db_p));
    DNX_KBP_TRY(kbp_db_install(db_p));

    /*
     * Clear all entries access IDs only for IPMC. 
     */
    if (entry_handle->table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD)
    {
        SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.clear(unit, DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV4_MC));
    }
    else if (entry_handle->table_id == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD)
    {
        SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.clear(unit, DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV6_MC));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function initializes a table iterator.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_kbp_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_entry_iterator_t *mdb_iterator;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    mdb_iterator = &iterator_info->mdb_iterator;

    sal_memset(mdb_iterator, 0x0, sizeof(dbal_physical_entry_iterator_t));
    DNX_KBP_RETRIEVE_HANDLES_FROM_DBAL_ENTRY(entry_handle, mdb_iterator->mdb_lpm_db_p, mdb_iterator->mdb_lpm_ad_db_p);

    DNX_KBP_TRY(kbp_db_entry_iter_init(mdb_iterator->mdb_lpm_db_p, &(mdb_iterator->mdb_lpm_iter)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function deinitializes a table iterator.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_kbp_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_entry_iterator_t *mdb_iterator;
    dbal_iterator_info_t *iterator_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    mdb_iterator = &(iterator_info->mdb_iterator);

    DNX_KBP_RETRIEVE_HANDLES_FROM_DBAL_ENTRY(entry_handle, mdb_iterator->mdb_lpm_db_p, mdb_iterator->mdb_lpm_ad_db_p);
    DNX_KBP_TRY(kbp_db_entry_iter_destroy(mdb_iterator->mdb_lpm_db_p, mdb_iterator->mdb_lpm_iter));
    sal_memset(mdb_iterator, 0x0, sizeof(dbal_physical_entry_iterator_t));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function gets the next entry from a table using
 *        an initialized table iterator.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_kbp_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;
    struct kbp_entry *kpb_entry;
    struct kbp_entry_info entry_info;
    struct kbp_entry_iter *iterator_p;
    kbp_db_t_p db_p;
    kbp_ad_db_t_p ad_db_p;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_physical_entry_t *phy_entry = &(entry_handle->phy_entry);
    uint32 inverted_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    db_p = iterator_info->mdb_iterator.mdb_lpm_db_p;
    ad_db_p = iterator_info->mdb_iterator.mdb_lpm_ad_db_p;
    iterator_p = iterator_info->mdb_iterator.mdb_lpm_iter;

    DNX_KBP_TRY(kbp_db_entry_iter_next(db_p, iterator_p, &kpb_entry));

    if (kpb_entry == NULL)
    {
        /** No more entries in the table */
        iterator_info->is_end = TRUE;
    }
    else
    {
        DNX_KBP_TRY(kbp_entry_get_info(db_p, kpb_entry, &entry_info));

        if (kpb_entry != NULL)
        {
            if (entry_info.ad_handle != NULL)
            {
                uint8 asso_data[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };

                DNX_KBP_TRY(kbp_ad_db_get(ad_db_p, entry_info.ad_handle, asso_data));

                sal_memset(phy_entry, 0x0, sizeof(*phy_entry));

                /** Set key and mask fields. */
                phy_entry->key_size = table->key_size;
                SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, table->key_size, entry_info.data, phy_entry->key));
                SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, table->key_size, entry_info.mask, inverted_mask));
                /*
                 * The mask in the KBP takes the opposite convention to DBAL (in KBP 1 is "don't care", in DBAL 0 is "don't care".
                 * We invert the mask.
                 */
                SHR_BITNEGATE_RANGE(inverted_mask, 0, table->key_size, phy_entry->k_mask);

                /** Set payload fields */
                phy_entry->payload_size = table->max_payload_size;
                SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal
                                (unit, table->max_payload_size, asso_data, phy_entry->payload));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(phy_entry->p_mask, 0, table->max_payload_size - 1));
                phy_entry->prefix_length = entry_info.prio_len;

                phy_entry->entry_hw_id = PTR_TO_INT(kpb_entry);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBPSDK entry AD handle is null.\n");
            }
        }
    }

    if (!iterator_info->is_end)
    {
        if (entry_handle->table->has_result_type)
        {
            SHR_IF_ERR_EXIT(dbal_kbp_res_type_resolution(unit, entry_handle));
        }

        SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "get next from"));
        iterator_info->entries_counter++;
    }

exit:
    SHR_FUNC_EXIT;
}

#else
/* *INDENT-OFF* */

shr_error_e dbal_kbp_entry_get(int unit,dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_entry_add(int unit,dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_entry_delete(int unit,dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_table_clear(int unit,dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_table_iterator_init(int unit, dbal_entry_handle_t * entry_handle){ SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_table_entry_get_next(int unit, dbal_entry_handle_t * entry_handle) {SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n"); exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_table_iterator_deinit(int unit,dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_res_type_resolution(int unit, dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_access_id_by_key_get(int unit,dbal_entry_handle_t * entry_handle) {SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

/* *INDENT-ON* */
#endif /** defined (INCLUDE_KBP) */
