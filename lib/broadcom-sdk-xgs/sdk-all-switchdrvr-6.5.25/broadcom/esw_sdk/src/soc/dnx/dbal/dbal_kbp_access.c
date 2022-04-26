/*
 * $Id: dbal_kbp_access.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <shared/bsl.h>

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
#include <sal/appl/sal.h>
#include "dbal_internal.h"
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/cmic.h>
#include <sal/types.h>

#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/swstate/auto_generated/access/kbp_fwd_tcam_access_mapper_access.h>

/**
 * Convenience macro for retrieving KBP DB and AD DB handles.
 * AD DB handles are retrieved to a pointer as array.
 */
#define DNX_KBP_HANDLES_GET(_table_, _kbp_handles_)                                                                 \
    do                                                                                                              \
    {                                                                                                               \
        if (_table_->kbp_handles)                                                                                   \
        {                                                                                                           \
            if (((kbp_db_handles_t *) _table_->kbp_handles)->db_p == NULL)                                          \
            {                                                                                                       \
                SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP Device is not Synced or the table is not in use\n");               \
            }                                                                                                       \
            _kbp_handles_ = _table_->kbp_handles;                                                                   \
        }                                                                                                           \
        else                                                                                                        \
        {                                                                                                           \
            SHR_ERR_EXIT(_SHR_E_INTERNAL,                                                                           \
                    "kbp handles not initialized for table %s\n", _table_->table_name);                             \
        }                                                                                                           \
    }                                                                                                               \
    while(0)

/** Macro for deciding if caching needs to be performed for a KBP DB */
#define DNX_KBP_ACCESS_PERFORM_CACHING(_caching_bmp_) \
    (_caching_bmp_ & (DNX_KBP_CACHING_BMP_ALLOWED | DNX_KBP_CACHING_BMP_FWD))

/*
 * Utility macro for checking if an ACL entry exists.
 * Retrieves the db_entry if it does and returns the appropriate error if it doesn't.
 */
#define DNX_KBP_ACCESS_ACL_DB_ENTRY_GET(_unit_, _entry_handle_, _db_entry_) \
do { \
    shr_error_e rv = _SHR_E_NONE; \
    rv = dbal_kbp_entry_acl_exists(_unit_, _entry_handle_); \
    if (rv == _SHR_E_NOT_FOUND) { SHR_IF_ERR_EXIT_NO_MSG(rv); } \
    else { SHR_IF_ERR_EXIT(rv); } \
    _db_entry_ = (struct kbp_entry *) INT_TO_PTR(_entry_handle_->phy_entry.entry_hw_id); \
} while(0)

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
    uint8 is_acl = (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID ? TRUE : FALSE);
    int logger_action = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    if (bsl_severity >= bslSeverityInfo)
    {
        if (dbal_logger_is_enable(unit, entry_handle->table_id))
        {
            if ((!sal_strncasecmp(action, "get from", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
                || (!sal_strncasecmp(action, "get next from", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
                || (!sal_strncasecmp(action, "Access ID get", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)))
            {
                logger_action = 1;
            }
            else if (!sal_strncasecmp(action, "delete from", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
            {
                logger_action = 2;
            }
            else
            {
                logger_action = 0;
            }

            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_KBP, logger_action);

            /** Physical table */
            LOG_CLI((BSL_META("Entry %s db_p %p ad_db_p (regular) %p ad_db_p (optimized) %p\n"),
                     action,
                     ((void *) ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->db_p),
                     ((void *) ((kbp_db_handles_t *) entry_handle->table->
                                kbp_handles)->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]),
                     ((void *) ((kbp_db_handles_t *) entry_handle->table->
                                kbp_handles)->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED])));

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
    dbal_entry_handle_t * entry_handle,
    uint8 payload_size)
{
    int index;
    int cur_res_type = 0;
    uint8 is_payload_opt = FALSE;
    /** result_type defined as an array to fix coverity issue */
    uint32 result_type[1] = { 0 };
    dbal_table_field_info_t table_field_info;
    int res_type_found = 0;

    SHR_FUNC_INIT_VARS(unit);

    if ((payload_size == ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->opt_result_size) ||
        (payload_size == ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->large_opt_result_size))
    {
        is_payload_opt = TRUE;
    }

    while (cur_res_type < entry_handle->table->nof_result_types)
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id,
                                                   DBAL_FIELD_RESULT_TYPE, 0, cur_res_type, 0, &table_field_info));

        /*
         * The KBP returns the value aligned to the payload size,
         * so if we want to read the result type we need to take it from the new offset
         */
        table_field_info.bits_offset_in_buffer =
            entry_handle->table->max_payload_size - table_field_info.field_nof_bits;
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, &table_field_info, DBAL_FIELD_RESULT_TYPE,
                                                   entry_handle->phy_entry.payload, result_type));

        for (index = 0; index < entry_handle->table->multi_res_info[cur_res_type].result_type_nof_hw_values; index++)
        {
            if (DNX_KBP_USE_OPTIMIZED_RESULT && (is_payload_opt == TRUE))
            {
                /** Look for the optimized result */
                if (entry_handle->table->multi_res_info[cur_res_type].result_type_hw_value[index] ==
                    DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE)
                {
                    result_type[0] = DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE;
                    entry_handle->cur_res_type = cur_res_type;
                    entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size = payload_size;
                    res_type_found = 1;
                    break;
                }
            }
            else
            {
                /** Look for the regular result */
                if (entry_handle->table->multi_res_info[cur_res_type].result_type_hw_value[index] == result_type[0])
                {
                    entry_handle->cur_res_type = cur_res_type;
                    entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size = payload_size;
                    res_type_found = 1;
                    break;
                }
            }
        }
        if (res_type_found)
        {
            break;
        }
        cur_res_type++;
    }

    if (cur_res_type == entry_handle->table->nof_result_types)
    {
        if (!res_type_found)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

    /*
     * here need to adjust the payload according to the offset between the result size to the max_payload
     * in case the retrieved payload size is 0, no need to adjust the payload, because it's all zeros
     */
    if ((payload_size != 0) &&
        (entry_handle->table->multi_res_info[cur_res_type].entry_payload_size < entry_handle->table->max_payload_size))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                        (unit,
                         entry_handle->table->max_payload_size -
                         entry_handle->table->multi_res_info[cur_res_type].entry_payload_size, TRUE, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Used to align payloads, which are smaller that the max payload size.
 */
static shr_error_e
dbal_kbp_res_type_align(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 payload_size)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        SHR_IF_ERR_EXIT(dbal_kbp_res_type_resolution(unit, entry_handle, payload_size));
    }
    else
    {
        entry_handle->cur_res_type = 0;
        if (entry_handle->table->multi_res_info[0].entry_payload_size < entry_handle->table->max_payload_size)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                            (unit,
                             entry_handle->table->max_payload_size -
                             entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size,
                             TRUE, entry_handle));
        }
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

    if (entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM)
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
 * Create new entry access ID to the key in the entry handle Internal for dbal_kbp_access the key mapping is being saved in hash table
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

/*
 * Retrieves the lo and hi of the ranges of the entry and update the phy_entry buffer if needed
 * Internal for dbal_kbp_access
 */
static shr_error_e
dbal_kbp_entry_get_ranges(
    int unit,
    dbal_entry_handle_t * entry_handle,
    struct kbp_entry_info *entry_info)
{
    int key_index;
    int range_id;
    uint16 lo[DNX_KBP_MAX_NOF_RANGES];
    uint16 hi[DNX_KBP_MAX_NOF_RANGES];

    SHR_FUNC_INIT_VARS(unit);

    /** Get range fields */
    for (range_id = 0; range_id < entry_info->nranges; range_id++)
    {
        lo[range_id] = entry_info->rinfo[range_id].lo;
        hi[range_id] = entry_info->rinfo[range_id].hi;
    }

    /** Set the range field in the entry handle */
    for (key_index = 0, range_id = 0; key_index < entry_handle->table->nof_key_fields; key_index++)
    {
        if (SHR_BITGET(entry_handle->table->keys_info[key_index].field_indication_bm, DBAL_FIELD_IND_IS_RANGED))
        {
            if (range_id >= entry_info->nranges)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected ranges are more than the retrieved ranges (%d)",
                             entry_info->nranges);
            }

            if (hi[range_id] != 0)
            {
                /*
                 * When range value hi is 0, key field value is present in the buffer.
                 * When it is not, the buffer has data value 0 and full don't care mask for the field.
                 * They need to be updated to data value = lo and full care mask.
                 */
                uint32 key_value[1] = { lo[range_id] };
                uint32 key_mask[1] = { 0xFFFFFFFF };
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(key_value,
                                                               entry_handle->table->
                                                               keys_info[key_index].bits_offset_in_buffer,
                                                               DNX_KBP_MAX_RANGE_SIZE_IN_BITS,
                                                               entry_handle->phy_entry.key));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (key_mask, entry_handle->table->keys_info[key_index].bits_offset_in_buffer,
                                 DNX_KBP_MAX_RANGE_SIZE_IN_BITS, entry_handle->phy_entry.k_mask));
            }
            entry_handle->key_field_ranges[key_index] = hi[range_id] - lo[range_id] + 1;
            entry_handle->nof_ranged_fields++;
            range_id++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_kbp_entry_add_ranges(
    int unit,
    dbal_entry_handle_t * entry_handle,
    kbp_db_t_p db_p,
    struct kbp_entry *db_entry)
{
    int key_index;
    int range_id = 0;
    int nof_ranges;
    uint16 lo[DNX_KBP_MAX_NOF_RANGES];
    uint16 hi[DNX_KBP_MAX_NOF_RANGES];

    SHR_FUNC_INIT_VARS(unit);

    /** Read ranges */
    for (key_index = 0; key_index < entry_handle->table->nof_key_fields; key_index++)
    {
        if (SHR_BITGET(entry_handle->table->keys_info[key_index].field_indication_bm, DBAL_FIELD_IND_IS_RANGED))
        {
            uint32 key_value[1] = { 0 };

            if (entry_handle->key_field_ids[key_index] == DBAL_FIELD_EMPTY)
            {
                lo[range_id] = 0;
                hi[range_id] = -1;
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_key_field_get(unit, entry_handle, entry_handle->key_field_ids[key_index],
                                                         key_value, NULL, DBAL_POINTER_TYPE_ARR_UINT32));

                lo[range_id] = key_value[0];
                hi[range_id] = key_value[0];
                if (entry_handle->key_field_ranges[key_index])
                {
                    hi[range_id] += entry_handle->key_field_ranges[key_index] - 1;
                }
            }
            range_id++;
        }
    }
    nof_ranges = range_id;

    /** Add range fields */
    for (range_id = 0; range_id < nof_ranges; range_id++)
    {
        DNX_KBP_TRY(kbp_entry_add_range(db_p, db_entry, lo[range_id], hi[range_id], range_id));
    }

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
    dbal_entry_handle_t * entry_handle,
    uint32 *payload_size)
{
    int res;
    kbp_ad_db_t_p ad_db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    /*
     * Select the AD DB:
     *      NULL, in case of optimized or 0b AD DB available (the AD DB will be selected internally)
     *      Regular, in case there are no optimized or 0b AD DB available.
     */
    ad_db_p = (DNX_KBP_USE_OPTIMIZED_RESULT || kbp_handles->ad_db_zero_size_p) ?
        NULL : kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR];

    res = dnx_kbp_entry_lpm_get(unit,
                                kbp_handles->db_p,
                                ad_db_p,
                                entry_handle->phy_entry.key,
                                entry_handle->table->key_size,
                                entry_handle->phy_entry.payload,
                                entry_handle->table->max_payload_size,
                                entry_handle->phy_entry.prefix_length, kbp_handles, payload_size);

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
    kbp_ad_db_t_p ad_db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;
    int is_cache_enabled = FALSE;
    int cur_res_type = entry_handle->cur_res_type;
    uint32 access_bmp_indication = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);

    /** Check if the result is optimized or regular */
    if (DNX_KBP_USE_OPTIMIZED_RESULT &&
        (entry_handle->table->multi_res_info[cur_res_type].result_type_hw_value[0] ==
         DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE) &&
        SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        /*
         * The result is optimized. Check:
         * 1. If the result is 0b size.
         * 2. Else if the result is large optimized
         * 3. Else the result is optimized
         *
         * Validations are performed on the valid bits of the payload.
         * In order to optimize the performance, we make the following assumptions:
         * 1. The total payload is always 64b.
         * 2. The optimized result is less than 32b.
         * 3. The large optimized result is no bigger than 32b.
         */

        if ((kbp_handles->ad_db_zero_size_p) &&
            ((entry_handle->phy_entry.payload[0] & ((1 << kbp_handles->opt_result_size) - 1)) == 0))
        {
            /** All relevant bits are zero. Use the 0 size result */
            ad_db_p = kbp_handles->ad_db_zero_size_p;
            entry_handle->table->multi_res_info[cur_res_type].entry_payload_size = 0;
        }
        else if ((kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_LARGE_OPTIMIZED]) &&
                 (entry_handle->phy_entry.payload[0] & ~((1 << kbp_handles->opt_result_size) - 1)))
        {
            /** Bits between the optimized size and large optimized size are set. Use the large optimized result */
            ad_db_p = kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_LARGE_OPTIMIZED];
            entry_handle->table->multi_res_info[cur_res_type].entry_payload_size = kbp_handles->large_opt_result_size;
        }
        else
        {
            /** Use the optimized result */
            ad_db_p = kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED];
            entry_handle->table->multi_res_info[cur_res_type].entry_payload_size = kbp_handles->opt_result_size;
        }
    }
    else
    {
        /** Use the regular result */
        ad_db_p = kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR];
    }

#if 0
    /** Passing default indication is required when public tables are supported */
    if (dbal_image_name_is_std_1(unit))
    {
        uint32 is_default = FALSE;
        /** IS_DEFAULT is always present for LPM tables in standard_1, but not in other images. Get its value. */
        dbal_table_field_info_t table_field_info;
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                        (unit, entry_handle->table_id, DBAL_FIELD_IS_DEFAULT, 0, cur_res_type, 0, &table_field_info));
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                        (unit, &table_field_info, DBAL_FIELD_IS_DEFAULT, entry_handle->phy_entry.payload, &is_default));
        if (is_default)
        {
            access_bmp_indication |= DNX_KBP_ACCESS_BMP_INDICATION_DEFAULT_ENTRY;
        }
    }
#endif

    /** Get caching indication */
    SHR_IF_ERR_EXIT(dbal_kbp_fwd_caching_enabled_get(unit, &is_cache_enabled));
    if (is_cache_enabled)
    {
        access_bmp_indication |= DNX_KBP_ACCESS_BMP_INDICATION_CACHING_ENABLED;
    }

    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "add to"));

    SHR_IF_ERR_EXIT(dnx_kbp_entry_lpm_add(unit,
                                          kbp_handles->db_p,
                                          ad_db_p,
                                          entry_handle->phy_entry.key,
                                          entry_handle->table->key_size,
                                          entry_handle->phy_entry.payload,
                                          entry_handle->table->multi_res_info[cur_res_type].entry_payload_size,
                                          entry_handle->phy_entry.prefix_length,
                                          &(entry_handle->phy_entry.indirect_commit_mode),
                                          access_bmp_indication, kbp_handles));

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
    kbp_ad_db_t_p ad_db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;
    int is_cache_enabled = FALSE;
    uint32 access_bmp_indication = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "delete from"));

    /*
     * Select the AD DB:
     *      NULL, in case of optimized or 0b AD DB available (the AD DB will be selected internally)
     *      Regular, in case there are no optimized or 0b AD DB available.
     */
    ad_db_p = (DNX_KBP_USE_OPTIMIZED_RESULT ? NULL :
               kbp_handles->ad_db_zero_size_p ? NULL : kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]);

    /** Set caching indication */
    SHR_IF_ERR_EXIT(dbal_kbp_fwd_caching_enabled_get(unit, &is_cache_enabled));
    if (is_cache_enabled)
    {
        access_bmp_indication |= DNX_KBP_ACCESS_BMP_INDICATION_CACHING_ENABLED;
    }

    SHR_IF_ERR_EXIT(dnx_kbp_entry_lpm_delete(unit,
                                             kbp_handles->db_p,
                                             ad_db_p,
                                             entry_handle->phy_entry.key,
                                             entry_handle->table->key_size,
                                             entry_handle->phy_entry.prefix_length,
                                             access_bmp_indication, kbp_handles, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function returns indication whether given ACL entry exists.
 *        For FWD entries the related hash table is checked and entry_hw_id is updated.
 *        For ACL entries only access_id_set is checked.
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
dbal_kbp_entry_acl_exists(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DBAL_TABLE_IS_TCAM_BY_KEY(entry_handle->table))
    {
        SHR_IF_ERR_EXIT_NO_MSG(dbal_kbp_access_id_by_key_get(unit, entry_handle));
    }
    else if ((entry_handle->access_id_set == 0))
    {
        /** cannot perform get, it means that the entry was not found */
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

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
    dbal_entry_handle_t * entry_handle,
    uint32 *payload_size)
{
    kbp_db_t_p db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;
    struct kbp_entry *db_entry = NULL;
    struct kbp_entry_info entry_info;
    uint8 data[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 asso_data[DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES] = { 0 };
    uint32 inverted_mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    kbp_status kbp_rv;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if entry exists, retrieve the db_entry if it does and return the appropriate error if it doesn't */
    DNX_KBP_ACCESS_ACL_DB_ENTRY_GET(unit, entry_handle, db_entry);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    db_p = kbp_handles->db_p;

    kbp_rv = kbp_entry_get_info(db_p, db_entry, &entry_info);
    if (dnx_kbp_error_translation(kbp_rv) != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    /** Return cached indication if requested */
    if (entry_handle->attribute_info[DBAL_ENTRY_ATTR_RES_CACHED] != NULL)
    {
        /** Entry is cached, if it's not active */
        *(entry_handle->attribute_info[DBAL_ENTRY_ATTR_RES_CACHED]) = (entry_info.active == 0) ? TRUE : FALSE;
    }
    else if ((entry_info.active == 0) && DBAL_TABLE_IS_TCAM_BY_KEY(entry_handle->table))
    {
        /** In case cached indication is not requested, return NOT_FOUND for cached entries; used for IPMC entries */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    sal_memcpy(data, entry_info.data, sizeof(uint8) * (entry_info.width_8));
    sal_memcpy(mask, entry_info.mask, sizeof(uint8) * (entry_info.width_8));
    DNX_KBP_TRY(kbp_ad_db_get(entry_info.ad_db, entry_info.ad_handle, asso_data));

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, entry_handle->table->key_size, data, entry_handle->phy_entry.key));
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, entry_handle->table->key_size, mask, inverted_mask));

    /*
     * The mask in the KBP takes the opposite convention to DBAL (in KBP 1 is "don't care", in DBAL 0 is "don't care".
     * We invert the mask. we also taking care of the offset of the key - in KBP the key aligned to the MSB,
     * so the LSB part until the first key field offset in the buffer
     */
    {
        uint32 key_offset =
            entry_handle->table->keys_info[entry_handle->table->nof_key_fields - 1].bits_offset_in_buffer;
        int size = entry_handle->table->key_size - key_offset;

        SHR_BITNEGATE_RANGE(inverted_mask, key_offset, size, entry_handle->phy_entry.k_mask);
    }

    /*
     * For field group (identified as dynamic tables) we do not override max_payload_size with the DB size.
     * For other we do at the moment.
     */
    if (entry_handle->table_id >= DBAL_NOF_TABLES)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, entry_handle->table->max_payload_size, payload_size));
    }
    else
    {
        *payload_size = entry_handle->table->max_payload_size;
    }
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, *payload_size, asso_data, entry_handle->phy_entry.payload));

    /** If entry is retrieved from optimized AD DB, use the optimized size */
    if (entry_info.ad_db == kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED])
    {
        *payload_size = kbp_handles->opt_result_size;
    }

    entry_handle->phy_entry.prefix_length = entry_info.prio_len;

    /** Ranges will update the data and mask with the min values of the ranges */
    SHR_IF_ERR_EXIT(dbal_kbp_entry_get_ranges(unit, entry_handle, &entry_info));

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
    int is_cache_enabled = FALSE;
    kbp_db_handles_t *kbp_handles = NULL;

    struct kbp_entry *db_entry = (struct kbp_entry *) INT_TO_PTR(entry_handle->phy_entry.entry_hw_id);
    struct kbp_entry_info entry_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if entry exists, retrieve the db_entry if it does and return the appropriate error if it doesn't */
    DNX_KBP_ACCESS_ACL_DB_ENTRY_GET(unit, entry_handle, db_entry);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);

    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "delete from"));

    DNX_KBP_TRY(kbp_entry_get_info(kbp_handles->db_p, db_entry, &entry_info));

    /** When requested, return indication if the entry is cached or not. */
    if (entry_handle->attribute_info[DBAL_ENTRY_ATTR_RES_CACHED] != NULL)
    {
        *(entry_handle->attribute_info[DBAL_ENTRY_ATTR_RES_CACHED]) = (entry_info.active == 0) ? TRUE : FALSE;
    }

    DNX_KBP_TRY(kbp_db_delete_entry(kbp_handles->db_p, db_entry));

    /** Check IPMC table for FWD caching and all other ACL tables for ACL caching */
    if (DBAL_TABLE_IS_TCAM_BY_KEY(entry_handle->table))
    {
        /** FWD caching */
        SHR_IF_ERR_EXIT(dbal_kbp_fwd_caching_enabled_get(unit, &is_cache_enabled));
    }
    else
    {
        /** ACL caching */
        SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.caching_enabled.get(unit, entry_handle->table->app_id,
                                                                       &is_cache_enabled));
    }

    if (!is_cache_enabled)
    {
        DNX_KBP_TRY(kbp_db_install(kbp_handles->db_p));
        if (entry_info.ad_db != kbp_handles->ad_db_zero_size_p)
        {
            /** Do not delete the AD entry for the 0b AD DB */
            DNX_KBP_TRY(kbp_ad_db_delete_entry(entry_info.ad_db, entry_info.ad_handle));
        }
    }

    /** Destroy entry access ID only for IPMC */
    if (DBAL_TABLE_IS_TCAM_BY_KEY(entry_handle->table))
    {
        SHR_IF_ERR_EXIT(dbal_kbp_entry_access_id_destroy(unit, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function updates an ACL entry in the external TCAM.
 *          It also handles indirect_commit_mode UPDATE and FORCE.
 *          Two different flows depending on whether the entry is FWD or ACL:
 *          FWD: The entry payload is updated if the AD DBs are the same.
 *              When the AD DBs are different, the old entry is deleted and the new entry is added.
 *              (add_entry parameter indicates to the upper add API if add needs to be performed.
 *          ACL: Entries are always deleted and then added in the upper add API.
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 * \param [in] db_p - KBP DB pointer.
 * \param [in] ad_db_p - KBP associated data DB pointer.
 * \param [in] asso_data - The payload data that will be updated.
 * \param [in] add_entry - Indication to the add API that the entry has not been updated and needs to be added.
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_kbp_entry_acl_update(
    int unit,
    dbal_entry_handle_t * entry_handle,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    uint32 priority,
    uint32 key_size,
    uint8 asso_data[DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES],
    uint8 *add_entry)
{
    shr_error_e rv = _SHR_E_NONE;
    struct kbp_entry *db_entry = NULL;
    struct kbp_entry_info entry_info = { 0 };
    uint8 entry_found = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * When indirect_commit_mode == DBAL_INDIRECT_COMMIT_MODE_UPDATE,
     *                              return error if the entry is not found of failed to be deleted (does not exist).
     *
     * When indirect_commit_mode == DBAL_INDIRECT_COMMIT_MODE_FORCE, the entry should be updated if it exists
     *                              or added if it doesn't. Don't return error.
     */

    rv = dbal_kbp_entry_acl_exists(unit, entry_handle);

    if (rv == _SHR_E_NOT_FOUND)
    {
        entry_found = FALSE;
        if (entry_handle->phy_entry.indirect_commit_mode == DBAL_INDIRECT_COMMIT_MODE_UPDATE)
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
    db_entry = (struct kbp_entry *) INT_TO_PTR(entry_handle->phy_entry.entry_hw_id);

    /*
     * Update the commit mode indication to from FORCE to UPDATE
     * in order for DBAL to correctly increment the table entry counter.
     */
    entry_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_UPDATE;

    /** Entry is found. Perform update */
    if (entry_found)
    {
        /** Retrieve the entry info */
        DNX_KBP_TRY(kbp_entry_get_info(db_p, db_entry, &entry_info));

        /** FWD */
        if (DBAL_TABLE_IS_TCAM_BY_KEY(entry_handle->table))
        {
            /*
             * KBP can have 2 AD DBs: regular and optimized.
             * Need to check which is the relevant AD DB and perform the correct action.
             */
            if (ad_db_p == entry_info.ad_db)
            {
                /** Same AD DB. Update the payload */
                DNX_KBP_TRY(kbp_ad_db_update_entry(ad_db_p, entry_info.ad_handle, asso_data));
                *add_entry = FALSE;
            }
            else
            {
                /** Different AD DBs. Update is not allowed, because it will cause traffic loss. */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Attempting to update an entry payload located in different AD DB, which could cause traffic loss\n");
            }
        }
        /** ACL */
        else
        {
            dbal_entry_handle_t entry_handle_get;
            dbal_entry_handle_t entry_handle_range_filled;
            int key_index;
            /** Parse the entry key */
            {
                uint32 inverted_mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
                uint32 key_offset;
                int size;
                uint32 range_key_value[1] = { 0 };
                uint32 range_key_mask[1] = { 0xFFFFFFFF };
                sal_memcpy(&entry_handle_get, entry_handle, sizeof(entry_handle_get));
                SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal
                                (unit, entry_handle_get.table->key_size, entry_info.data,
                                 entry_handle_get.phy_entry.key));
                SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal
                                (unit, entry_handle_get.table->key_size, entry_info.mask, inverted_mask));
                key_offset =
                    entry_handle_get.table->keys_info[entry_handle_get.table->nof_key_fields - 1].bits_offset_in_buffer;
                size = entry_handle_get.table->key_size - key_offset;
                SHR_BITNEGATE_RANGE(inverted_mask, key_offset, size, entry_handle_get.phy_entry.k_mask);
                entry_handle_get.nof_ranged_fields = 0;
                SHR_IF_ERR_EXIT(dbal_kbp_entry_get_ranges(unit, &entry_handle_get, &entry_info));
                sal_memcpy(&(entry_handle_range_filled), entry_handle, sizeof(entry_handle_range_filled));
                for (key_index = 0; key_index < entry_handle->table->nof_key_fields; key_index++)
                {
                    if (SHR_BITGET(entry_handle->table->keys_info[key_index].field_indication_bm,
                                   DBAL_FIELD_IND_IS_RANGED))
                    {
                        if (entry_handle->key_field_ids[key_index] == DBAL_FIELD_EMPTY)
                        {
                            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                            (range_key_value,
                                             entry_handle->table->keys_info[key_index].bits_offset_in_buffer,
                                             DNX_KBP_MAX_RANGE_SIZE_IN_BITS, entry_handle_range_filled.phy_entry.key));
                            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                            (range_key_mask,
                                             entry_handle->table->keys_info[key_index].bits_offset_in_buffer,
                                             DNX_KBP_MAX_RANGE_SIZE_IN_BITS,
                                             entry_handle_range_filled.phy_entry.k_mask));
                            (entry_handle_range_filled.nof_ranged_fields)++;
                            entry_handle_range_filled.key_field_ranges[key_index] = 0xFFFF - 0 + 1;
                        }
                    }
                }

            }
            /** Check if the entry priority have been updated */
            if (priority != entry_info.prio_len)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Attempting to update an entry using different priority\n");
            }
            /** Check if the entry key have been updated */
            if (sal_memcmp(entry_handle_get.phy_entry.key, entry_handle_range_filled.phy_entry.key,
                           sizeof(entry_handle_get.phy_entry.key)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Attempting to update an entry using different key\n");
            }
            /** Check if the entry key mask have been updated */
            if (sal_memcmp(entry_handle_get.phy_entry.k_mask, entry_handle_range_filled.phy_entry.k_mask,
                           sizeof(entry_handle_get.phy_entry.k_mask)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Attempting to update an entry using different key mask\n");
            }
            /** Check if the range uppder bounds have been updated. */
            for (key_index = 0; key_index < entry_handle->table->nof_key_fields; key_index++)
            {
                if (SHR_BITGET(entry_handle->table->keys_info[key_index].field_indication_bm, DBAL_FIELD_IND_IS_RANGED))
                {
                    if (entry_handle_range_filled.key_field_ranges[key_index] !=
                        entry_handle_get.key_field_ranges[key_index])
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Attempting to update an entry using different key. "
                                     "Range is different.\n");
                    }
                }
            }
            /** ACLs have only a single AD DB. Update the entry. */
            DNX_KBP_TRY(kbp_ad_db_update_entry(ad_db_p, entry_info.ad_handle, asso_data));
            *add_entry = FALSE;
        }
    }
    /** Entry is not found. Perform add. */
    else
    {
        *add_entry = TRUE;
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
    int is_cache_enabled = FALSE;
    kbp_db_t_p db_p = NULL;
    kbp_ad_db_t_p ad_db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;
    struct kbp_entry *db_entry = NULL;
    struct kbp_ad *ad_entry = NULL;
    uint8 data[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 asso_data[DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES] = { 0 };
    uint32 inverted_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint32 payload_size;
    int cur_res_type = entry_handle->cur_res_type;
    uint8 add_entry = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    db_p = kbp_handles->db_p;
    /** Check if the result is optimized or regular and use the relevant AD DB */
    if (DNX_KBP_USE_OPTIMIZED_RESULT && DBAL_TABLE_IS_TCAM_BY_KEY(entry_handle->table)
        && (entry_handle->table->multi_res_info[cur_res_type].result_type_hw_value[0] ==
            DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE))
    {
        /** Use optimized result. Update the payload size. */
        entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size =
            kbp_handles->opt_result_size;
        ad_db_p = kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED];
    }
    else
    {
        ad_db_p = kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR];
    }

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, entry_handle->table->key_size, entry_handle->phy_entry.key, data));
    /*
     * The mask in the KBP takes the opposite convention to DBAL (in KBP 1 is "don't care", in DBAL 0 is "don't care".
     * We invert the mask.
     */
    SHR_BITNEGATE_RANGE(entry_handle->phy_entry.k_mask, 0, entry_handle->table->key_size, inverted_mask);
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, entry_handle->table->key_size, inverted_mask, mask));

    /*
     * For field group (identified as dynamic tables) we have only one result type, but the payload size in DB can be
     * bigger than the size written to the result buffer, so we provide the DB size to avoid MSB alignment.
     * For FWD tables (identified as static tables) we may have multiple result sizes and we don't expect the DB size to be
     * bigger than the size on result buffer, so we provide the size of the result type.
     */
    if (entry_handle->table_id >= DBAL_NOF_TABLES)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, entry_handle->table->max_payload_size, &payload_size));
    }
    else
    {
        payload_size = entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;
    }
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, payload_size, entry_handle->phy_entry.payload, asso_data));

    /** Entry is for update. */
    if (entry_handle->phy_entry.indirect_commit_mode != DBAL_INDIRECT_COMMIT_MODE_NORMAL)
    {
        SHR_IF_ERR_EXIT(dbal_kbp_entry_acl_update(unit, entry_handle, db_p, ad_db_p,
                                                  entry_handle->phy_entry.prefix_length,
                                                  entry_handle->table->key_size, asso_data, &add_entry));
    }

    if (add_entry)
    {
        /** it means that the access id should be retrieve by the KBP_SDK */
        if (entry_handle->access_id_set == 0)
        {
            DNX_KBP_TRY(kbp_db_add_ace(db_p, data, mask, entry_handle->phy_entry.prefix_length, &db_entry));
        }
        else
        {
            /**  here we use new API that add to entry to specific location kbp_db_add_ace_with_index, the index is the db_entry */
            DNX_KBP_TRY(kbp_db_add_ace_with_index
                        (db_p, data, mask, entry_handle->phy_entry.prefix_length, entry_handle->phy_entry.entry_hw_id));
            db_entry = INT_TO_PTR(entry_handle->phy_entry.entry_hw_id);
        }

        /** Add ranges */
        SHR_IF_ERR_EXIT(dbal_kbp_entry_add_ranges(unit, entry_handle, db_p, db_entry));

        SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "add to"));

        kbp_ret_val = kbp_ad_db_add_entry(ad_db_p, asso_data, &ad_entry);
        if (dnx_kbp_error_translation(kbp_ret_val) != _SHR_E_NONE)
        {
            /** Rollback */
            kbp_db_delete_entry(db_p, db_entry);
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "kbp_ad_db_add_entry failed: %s\n", kbp_get_status_string(kbp_ret_val));
        }

        kbp_ret_val = kbp_entry_add_ad(db_p, db_entry, ad_entry);
        if (dnx_kbp_error_translation(kbp_ret_val) != _SHR_E_NONE)
        {
            /** Rollback */
            kbp_db_delete_entry(db_p, db_entry);
            kbp_ad_db_delete_entry(ad_db_p, ad_entry);
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "kbp_entry_add_ad failed: %s\n", kbp_get_status_string(kbp_ret_val));
        }

        /** Check IPMC table for FWD caching and all other ACL tables for ACL caching */
        if (DBAL_TABLE_IS_TCAM_BY_KEY(entry_handle->table))
        {
            /** FWD caching */
            SHR_IF_ERR_EXIT(dbal_kbp_fwd_caching_enabled_get(unit, &is_cache_enabled));
        }
        else
        {
            /** ACL caching */
            SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.caching_enabled.get(unit, entry_handle->table->app_id,
                                                                           &is_cache_enabled));
        }

        /** Check if the cache commit feature is enabled. If it is, skip the install step. */
        if (!is_cache_enabled)
        {
            kbp_ret_val = kbp_db_install(db_p);
            if (dnx_kbp_error_translation(kbp_ret_val) != _SHR_E_NONE)
            {
                /** Rollback */
                kbp_db_delete_entry(db_p, db_entry);
                kbp_ad_db_delete_entry(ad_db_p, ad_entry);

                if (dnx_kbp_error_translation(kbp_ret_val) == _SHR_E_FULL)
                {
                    SHR_ERR_EXIT(_SHR_E_FULL, "Table is full, %s\n", kbp_get_status_string(kbp_ret_val));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "kbp_db_install failed: %s\n", kbp_get_status_string(kbp_ret_val));
                }
            }
        }

        if (entry_handle->access_id_set == 0)
        {
            void *tmp;
            /** error: cast from pointer to integer of different size */
            tmp = (void *) (db_entry);
            entry_handle->phy_entry.entry_hw_id = PTR_TO_INT(tmp);
        }

        /** Create entry access ID only for IPMC. In case the entry was updated, the old key was already deleted. */
        if (DBAL_TABLE_IS_TCAM_BY_KEY(entry_handle->table))
        {
            int rv;
            rv = dbal_kbp_entry_access_id_create(unit, entry_handle, entry_handle->phy_entry.entry_hw_id);
            if (rv != _SHR_E_NONE)
            {
                /** Rollback */
                kbp_db_delete_entry(db_p, db_entry);
                kbp_ad_db_delete_entry(ad_db_p, ad_entry);
                SHR_SET_CURRENT_ERR(_SHR_E_FULL);
                SHR_EXIT();
            }
        }
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
    uint32 payload_size = 0;
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        {
            /** DBAL EM tables are used in KBP as LPM. Need to set preffix_length in order to work correctly. */
            entry_handle->phy_entry.prefix_length = entry_handle->phy_entry.key_size;
            res = dbal_kbp_entry_lpm_get(unit, entry_handle, &payload_size);
            break;
        }
        case DBAL_TABLE_TYPE_LPM:
        {
            res = dbal_kbp_entry_lpm_get(unit, entry_handle, &payload_size);
            break;
        }
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_BY_ID:
        {
            res = dbal_kbp_entry_acl_get(unit, entry_handle, &payload_size);
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
            SHR_ERR_EXIT(res, "entry get failed internal error table %s\n", entry_handle->table->table_name);
        }
    }

    SHR_IF_ERR_EXIT(dbal_kbp_res_type_align(unit, entry_handle, payload_size));

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
        {
            /** DBAL EM tables are used in KBP as LPM. Need to set preffix_length in order to work correctly. */
            entry_handle->phy_entry.prefix_length = entry_handle->phy_entry.key_size;
            SHR_IF_ERR_EXIT(dbal_kbp_entry_lpm_add(unit, entry_handle));
            break;
        }
        case DBAL_TABLE_TYPE_LPM:
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_lpm_add(unit, entry_handle));
            break;
        }
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_BY_ID:
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
        {
            /** DBAL EM tables are used in KBP as LPM. Need to set preffix_length in order to work correctly. */
            entry_handle->phy_entry.prefix_length = entry_handle->phy_entry.key_size;
            SHR_IF_ERR_EXIT(dbal_kbp_entry_lpm_delete(unit, entry_handle));
            break;
        }
        case DBAL_TABLE_TYPE_LPM:
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_lpm_delete(unit, entry_handle));
            break;
        }
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_BY_ID:
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
    kbp_db_handles_t *kbp_handles = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Retrieve KBP handles */
    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    db_p = kbp_handles->db_p;

    {
        DNX_KBP_TRY(kbp_db_delete_all_entries(db_p));
        if (kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR] != NULL)
        {
            DNX_KBP_TRY(kbp_ad_db_delete_all_entries(kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]));
        }
        if (DNX_KBP_USE_OPTIMIZED_RESULT)
        {
            if (kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED] != NULL)
            {
                DNX_KBP_TRY(kbp_ad_db_delete_all_entries(kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED]));
            }
            if (kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_LARGE_OPTIMIZED] != NULL)
            {
                DNX_KBP_TRY(kbp_ad_db_delete_all_entries(kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_LARGE_OPTIMIZED]));
            }
        }
        DNX_KBP_TRY(kbp_db_install(db_p));
    }

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
    kbp_db_handles_t *kbp_handles = NULL;
    dbal_physical_entry_iterator_t *kbp_iterator;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    kbp_iterator = &iterator_info->mdb_iterator;

    sal_memset(kbp_iterator, 0x0, sizeof(dbal_physical_entry_iterator_t));
    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);

    kbp_iterator->kbp_lpm_db_p = kbp_handles->db_p;
    DNX_KBP_TRY(kbp_db_entry_iter_init(kbp_iterator->kbp_lpm_db_p, &(kbp_iterator->kbp_lpm_iter)));

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
    kbp_db_handles_t *kbp_handles = NULL;
    dbal_physical_entry_iterator_t *kbp_iterator;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    kbp_iterator = &(iterator_info->mdb_iterator);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    kbp_iterator->kbp_lpm_db_p = kbp_handles->db_p;
    DNX_KBP_TRY(kbp_db_entry_iter_destroy(kbp_iterator->kbp_lpm_db_p, kbp_iterator->kbp_lpm_iter));
    sal_memset(kbp_iterator, 0x0, sizeof(dbal_physical_entry_iterator_t));

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
    dbal_logical_table_t *table = entry_handle->table;
    dbal_physical_entry_t *phy_entry = &(entry_handle->phy_entry);
    uint32 inverted_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint8 payload_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    db_p = iterator_info->mdb_iterator.kbp_lpm_db_p;
    /** ad_db_p is taken from the entry_info */
    iterator_p = iterator_info->mdb_iterator.kbp_lpm_iter;

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

                DNX_KBP_TRY(kbp_ad_db_get(entry_info.ad_db, entry_info.ad_handle, asso_data));

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

                SHR_IF_ERR_EXIT(dbal_kbp_entry_get_ranges(unit, entry_handle, &entry_info));

                /** If entry is retrieved from optimized AD DB, use the optimized size */
                payload_size = table->max_payload_size;
                if (entry_info.ad_db ==
                    ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED])
                {
                    payload_size = ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->opt_result_size;
                }
                /** If entry is retrieved from large optimized AD DB, use the large optimized size */
                else if (entry_info.ad_db ==
                         ((kbp_db_handles_t *) entry_handle->table->
                          kbp_handles)->ad_db_p[DNX_KBP_AD_DB_INDEX_LARGE_OPTIMIZED])
                {
                    payload_size = ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->large_opt_result_size;
                }
                /** If entry is retrieved from zero size AD DB, set payload size to 0 and the indication to true */
                else if (entry_info.ad_db == ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->ad_db_zero_size_p)
                {
                    payload_size = 0;
                    entry_handle->access_id_set = TRUE;
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBPSDK entry AD handle is null.\n");
            }
        }
    }

    if (!iterator_info->is_end)
    {
        SHR_IF_ERR_EXIT(dbal_kbp_res_type_align(unit, entry_handle, payload_size));

        SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "get next from"));
        iterator_info->entries_counter++;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function for returning if fwd caching is enabled.
 *   \param [in] unit - Relevant unit.
 *   \param [out] enabled - The returned indication.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_kbp_fwd_caching_enabled_get(
    int unit,
    int *enabled)
{
    uint8 is_allocated = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(KBP_ACCESS.is_init(unit, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP manager is not initialized; Caching is not available.");
    }
    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP is not enabled; Caching is not available.");
    }
    SHR_IF_ERR_EXIT(KBP_ACCESS.fwd_caching_enabled.get(unit, enabled));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function for enabling/disabling fwd caching feature.
 *   \param [in] unit - Relevant unit.
 *   \param [in] enable - Enable(1) or disable(0) caching.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_kbp_fwd_caching_enabled_set(
    int unit,
    int enable)
{
    uint8 is_allocated = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(KBP_ACCESS.is_init(unit, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP manager is not initialized; Caching is not available.");
    }
    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP is not enabled; Caching is not available.");
    }
    SHR_IF_ERR_EXIT(KBP_ACCESS.fwd_caching_enabled.set(unit, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Iterates over all KBP DBs swstate and perform caching if the DB allow FWD caching and FWD caching is enabled.
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e
dbal_kbp_fwd_cache_commit(
    int unit)
{
    int db_id;
    kbp_db_handles_t op2_kbp_handles;
    uint8 is_allocated = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(KBP_ACCESS.is_init(unit, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP manager is not initialized; Caching cannot be performed.");
    }
    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP is not enabled; Caching cannot be performed.");
    }

    /** Go over all DBs and perform caching according to the caching bitmap */
    for (db_id = 0; db_id < DNX_KBP_MAX_NOF_DBS; db_id++)
    {
        /** Get the KBP handles of this DB from swstate */
        SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, db_id, &op2_kbp_handles));
        if (DNX_KBP_ACCESS_PERFORM_CACHING(op2_kbp_handles.caching_bmp))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_cache_db_commit_internal(unit, op2_kbp_handles));
        }
    }

    /** Need to see if all other DBs need to be cleared of pending entries when error is encountered. */

exit:
    SHR_FUNC_EXIT;
}

#endif /** defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */
