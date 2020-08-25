/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file lif_table_mngr.c
 *
 *  Lif table allocations and HW writes.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_os_interface.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_table_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */
/*
* \brief Sorted array of result types per lif table
*/
typedef struct
{
   /**
    * The index of the result type
    */
    uint32 result_type_id;
   /**
    * Payload size
    */
    uint32 result_type_size;
} lif_table_mngr_lif_table_result_type_info_t;

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/**
 * \brief -
 *  Compare routine for sorting result types according to size.
 */
static int
lif_table_mngr_result_types_compre(
    void *a,
    void *b)
{
    lif_table_mngr_lif_table_result_type_info_t result_type1, result_type2;

    result_type1 = *(lif_table_mngr_lif_table_result_type_info_t *) a;
    result_type2 = *(lif_table_mngr_lif_table_result_type_info_t *) b;

    return (result_type1.result_type_size - result_type2.result_type_size);
}

/**
 * \brief - Sort all lif tables result types according to size.
 * Should be done once on device init.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *
 */
shr_error_e
dnx_algo_lif_table_mngr_sorted_result_types_table_init(
    int unit)
{
    dbal_physical_tables_e physical_table_id;
    uint32 dbal_table_id;
    CONST dbal_logical_table_t *table;
    uint32 dbal_result_type;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLES_RESULT_TYPE_SORTED, &entry_handle_id));

    /*
     * Loop on all DBAL tables
     */
    for (dbal_table_id = 1; dbal_table_id < DBAL_NOF_TABLES; dbal_table_id++)
    {
        /** Get table information */
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
        if (table->nof_physical_tables > 0)
        {
            /*
             * Get the physical table associated with this lif format. Verify that the physical table is EEDB.
             */
            SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                           &physical_table_id));
            /*
             * If the table is LIF table (in LIF or EEDB), add it into the DBs
             */
            if (physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_1 || physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_2
                || physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_3 || physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_4
                || physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1 || physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)

            {
                lif_table_mngr_lif_table_result_type_info_t result_types_arr[DBAL_MAX_NUMBER_OF_RESULT_TYPES];

                /*
                 * This is a LIF table - Add all result types to the list
                 */
                for (dbal_result_type = 0; dbal_result_type < table->nof_result_types; dbal_result_type++)
                {
                    result_types_arr[dbal_result_type].result_type_id = dbal_result_type;
                    result_types_arr[dbal_result_type].result_type_size =
                        table->multi_res_info[dbal_result_type].entry_payload_size;
                }
                /*
                 * Sort the list
                 */
                utilex_os_qsort(result_types_arr, table->nof_result_types,
                                sizeof(lif_table_mngr_lif_table_result_type_info_t),
                                lif_table_mngr_result_types_compre);
                /*
                 * Fill SW state table
                 * key: table id
                 * value: array of sorted result type ids
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_TABLE, dbal_table_id);
                for (dbal_result_type = 0; dbal_result_type < table->nof_result_types; dbal_result_type++)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SORTED_RESULT_TYPES,
                                                 dbal_result_type, result_types_arr[dbal_result_type].result_type_id);

                }
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per table - egress PWE
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] result_type - Result type of the current lif
 *   \param [out] table_specific_flags - filled in this function
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_table_specific_flags_fill_egress_pwe(
    int unit,
    int result_type,
    uint32 *table_specific_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Here filling constraints per table for special tables */
    if (result_type == DBAL_RESULT_TYPE_EEDB_PWE_ETPS_MPLS_1_AH)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_ONE_SPECIAL_LABEL;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per table - egress PWE
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] table_specific_flags - Input about the lif table specific info
 *   \param [out] table_specific_rules - Specific rules per table
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_table_specific_result_type_filtering_egress_pwe(
    int unit,
    uint32 table_specific_flags,
    lif_table_mngr_table_specific_rules_t * table_specific_rules)
{
    int nof_res_types;

    SHR_FUNC_INIT_VARS(unit);

    /** Get number of result types of this table */
    SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get(unit, DBAL_TABLE_EEDB_PWE, &nof_res_types));

    /** Here adding constraints per table for special tables */

    if (_SHR_IS_FLAG_SET
        (table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_ONE_SPECIAL_LABEL) ||
        _SHR_IS_FLAG_SET
        (table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TWO_SPECIAL_LABELS) ||
        _SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_THREE_SPECIAL_LABELS))
    {
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_PWE_ETPS_MPLS_1_AH);
    }
    else
    {
        SHR_BITSET_RANGE(&(table_specific_rules->allowed_result_types_bmp), 0, nof_res_types);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_PWE_ETPS_MPLS_1_AH);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per table - egress ARP
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] result_type - Result type of the current lif
 *   \param [out] table_specific_flags - filled in this function
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_table_specific_flags_fill_egress_arp(
    int unit,
    int result_type,
    uint32 *table_specific_flags)
{
    dbal_table_field_info_t field_info;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    /** Here adding constraints per table for special tables */

    /** Forbidden Fields */

    /** Check if VSI field exists */
    rv = dbal_tables_field_info_get_no_err(unit, DBAL_TABLE_EEDB_ARP, DBAL_FIELD_VSI, FALSE,
                                           result_type, INST_SINGLE, &field_info);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NOT_FOUND)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_NO_VSI;
    }
    /** Check if SOURCE_IDX field exists */
    rv = dbal_tables_field_info_get_no_err(unit, DBAL_TABLE_EEDB_ARP, DBAL_FIELD_SOURCE_IDX, FALSE,
                                           result_type, INST_SINGLE, &field_info);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NOT_FOUND)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_NO_SOURCE_IDX;
    }

    /** Allowed result types */
    if ((result_type == DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC)
        || (result_type == DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC_1TAG_STAT))
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per table: egress ARP
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] table_specific_flags - Input about the lif table specific info
 *   \param [out] table_specific_rules - Specific rules per table
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_table_specific_result_type_filtering_egress_arp(
    int unit,
    uint32 table_specific_flags,
    lif_table_mngr_table_specific_rules_t * table_specific_rules)
{
    int nof_res_types;

    SHR_FUNC_INIT_VARS(unit);

    /** Get number of result types of this table */
    SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get(unit, DBAL_TABLE_EEDB_ARP, &nof_res_types));

    /** Here adding constraints per table for special tables */

    /** Forbidden Fields */
    if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_NO_VSI))
    {
        table_specific_rules->forbidden_fields_dbal_ids[table_specific_rules->forbidden_fields_nof] = DBAL_FIELD_VSI;
        table_specific_rules->forbidden_fields_nof = table_specific_rules->forbidden_fields_nof + 1;
    }
    if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_NO_SOURCE_IDX))
    {
        table_specific_rules->forbidden_fields_dbal_ids[table_specific_rules->forbidden_fields_nof] =
            DBAL_FIELD_SOURCE_IDX;
        table_specific_rules->forbidden_fields_nof = table_specific_rules->forbidden_fields_nof + 1;
    }
    /** Allowed result types */
    if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION))
    {
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC_1TAG_STAT);
    }
    else
    {
        SHR_BITSET_RANGE(&(table_specific_rules->allowed_result_types_bmp), 0, nof_res_types);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC_1TAG_STAT);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per table: egress EVPN
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] table_specific_flags - Input about the lif table specific info
 *   \param [out] table_specific_rules - Specific rules per table
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_table_specific_result_type_filtering_egress_evpn(
    int unit,
    uint32 table_specific_flags,
    lif_table_mngr_table_specific_rules_t * table_specific_rules)
{
    int nof_res_types;

    SHR_FUNC_INIT_VARS(unit);

    /** Get number of result types of this table */
    SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get(unit, DBAL_TABLE_EEDB_EVPN, &nof_res_types));

    /** Here adding constraints per table for special tables */

    /** Allowed result types */
    if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_IML))
    {
        /** Only result types with IML are supported */
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML);
    }
    else
    {
        /** All result tyes without IML are supported */
        SHR_BITSET_RANGE(&(table_specific_rules->allowed_result_types_bmp), 0, nof_res_types);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per table: egress EVPN
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] result_type - Result type of the current lif
 *   \param [out] table_specific_flags - filled in this function
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_table_specific_flags_fill_egress_evpn(
    int unit,
    int result_type,
    uint32 *table_specific_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Here adding constraints per table for special tables */
    if (result_type == DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_IML;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per table: egress MPLS tunnel
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] table_specific_flags - Input about the lif table specific info
 *   \param [out] table_specific_rules - Specific rules per table
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_table_specific_result_type_filtering_egress_mpls_tunnel(
    int unit,
    uint32 table_specific_flags,
    lif_table_mngr_table_specific_rules_t * table_specific_rules)
{
    int nof_res_types;

    SHR_FUNC_INIT_VARS(unit);

    /** Get number of result types of this table */
    SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get(unit, DBAL_TABLE_EEDB_MPLS_TUNNEL, &nof_res_types));

    /** Forbidden fields */
    /** MPLS 2 case - label2 field is relevant only for 2 encapsulation */
    if (!_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_SECOND_LABEL_EXISTS))
    {
        table_specific_rules->forbidden_fields_dbal_ids[table_specific_rules->forbidden_fields_nof] =
            DBAL_FIELD_MPLS_LABEL_2;
        table_specific_rules->forbidden_fields_nof = table_specific_rules->forbidden_fields_nof + 1;
    }

    /** Allowed result types */
    /** PHP case - only for PHP result types are relevant */
    if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_PHP))
    {
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_PHP_STAT);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_ONE_SPECIAL_LABEL))
    {
        /** Only result types with additional headers are supported */
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1_AH);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_1_AH);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TWO_SPECIAL_LABELS))
    {
        /** Only result types with additional headers are supported */
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1_AH);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_2_AH);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_THREE_SPECIAL_LABELS))
    {
        /** Only result types with additional headers are supported */
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1_AH);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_3_AH);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TANDEM))
    {
        /** Only tandem result types are supported */
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1_TANDEM);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_TANDEM);
    }
    else
    {
        /** Disallow the result types above (nof labels and PHP are done through forbidden fields */
        SHR_BITSET_RANGE(&(table_specific_rules->allowed_result_types_bmp), 0, nof_res_types);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1_AH);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_1_AH);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_2_AH);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_3_AH);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1_TANDEM);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_TANDEM);
        SHR_BITCLR(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_PHP_STAT);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per table: egress MPLS tunnel
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] result_type - Result type of the current lif
 *   \param [out] table_specific_flags - filled in this function
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_table_specific_flags_fill_egress_mpls_tunnel(
    int unit,
    int result_type,
    uint32 *table_specific_flags)
{
    dbal_table_field_info_t field_info;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    switch (result_type)
    {
        case DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1_AH:
        case DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_1_AH:
        {
            *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_ONE_SPECIAL_LABEL;
            break;
        }
        case DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_2_AH:
        {
            *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TWO_SPECIAL_LABELS;
            break;
        }
        case DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_3_AH:
        {
            *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_THREE_SPECIAL_LABELS;
            break;
        }
        case DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1_TANDEM:
        case DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2_TANDEM:
        {
            *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TANDEM;
            break;
        }
        case DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_PHP_STAT:
        {
            *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_PHP;
            break;
        }
        default:
            break;
    }

    /*
     * Forbidden fields
     */
    /** Check if MPLS_LABEL_2 field exists */
    rv = dbal_tables_field_info_get_no_err(unit, DBAL_TABLE_EEDB_MPLS_TUNNEL, DBAL_FIELD_MPLS_LABEL_2, FALSE,
                                           result_type, INST_SINGLE, &field_info);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NONE)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_SECOND_LABEL_EXISTS;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_lif_table_mngr_table_specific_result_type_filtering(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 table_specific_flags,
    lif_table_mngr_table_specific_rules_t * table_specific_rules)
{
    int nof_res_types;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(table_specific_rules, 0, sizeof(lif_table_mngr_table_specific_rules_t));
    SHR_BITCLR_RANGE(&(table_specific_rules->allowed_result_types_bmp), 0, DBAL_MAX_NUMBER_OF_RESULT_TYPES);
    table_specific_rules->forbidden_fields_nof = 0;

    /** Here adding constraints per table for special tables */
    if (dbal_table_id == DBAL_TABLE_EEDB_MPLS_TUNNEL)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering_egress_mpls_tunnel
                        (unit, table_specific_flags, table_specific_rules));
    }
    else if (dbal_table_id == DBAL_TABLE_EEDB_EVPN)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering_egress_evpn
                        (unit, table_specific_flags, table_specific_rules));
    }
    else if (dbal_table_id == DBAL_TABLE_EEDB_PWE)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering_egress_pwe
                        (unit, table_specific_flags, table_specific_rules));

    }
    else if (dbal_table_id == DBAL_TABLE_EEDB_ARP)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering_egress_arp
                        (unit, table_specific_flags, table_specific_rules));
    }
    else
    {
        /** No constraints, set all result types of this table */
        /** Get number of result types of this table */
        SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get(unit, dbal_table_id, &nof_res_types));
        SHR_BITSET_RANGE(&(table_specific_rules->allowed_result_types_bmp), 0, nof_res_types);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_lif_table_mngr_table_specific_flags_fill(
    int unit,
    dbal_tables_e dbal_table_id,
    int result_type,
    uint32 *table_specific_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Here adding constraints per table for special tables */
    if (dbal_table_id == DBAL_TABLE_EEDB_MPLS_TUNNEL)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_flags_fill_egress_mpls_tunnel
                        (unit, result_type, table_specific_flags));
    }
    else if (dbal_table_id == DBAL_TABLE_EEDB_EVPN)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_flags_fill_egress_evpn
                        (unit, result_type, table_specific_flags));
    }
    else if (dbal_table_id == DBAL_TABLE_EEDB_PWE)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_flags_fill_egress_pwe
                        (unit, result_type, table_specific_flags));
    }
    else if (dbal_table_id == DBAL_TABLE_EEDB_ARP)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_flags_fill_egress_arp
                        (unit, result_type, table_specific_flags));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_algo_lif_table_mngr_decide_result_type(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 table_specific_flags,
    uint32 entry_handle_id,
    uint32 *result_type_decision)
{
    int field_idx, nof_res_types, result_type_iterator;
    lif_table_mngr_table_specific_rules_t table_specific_rules;
    shr_error_e rv;
    uint8 valid_result_type = FALSE;
    CONST dbal_logical_table_t *table;
    uint32 sorted_result_types_entry_handle_id;
    uint32 result_type_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Use table specific filtering of result types for tables where additional rules should be applied */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering
                    (unit, dbal_table_id, table_specific_flags, &table_specific_rules));
    /** Get number of result types of this table */
    SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get(unit, dbal_table_id, &nof_res_types));
    /** Get table information */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
    /** Iterate over all possible result types */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_LIF_TABLES_RESULT_TYPE_SORTED, &sorted_result_types_entry_handle_id));
    dbal_entry_key_field32_set(unit, sorted_result_types_entry_handle_id, DBAL_FIELD_DBAL_TABLE, dbal_table_id);
    for (result_type_iterator = 0; result_type_iterator < nof_res_types; result_type_iterator++)
    {
        /*
         * The entry number result_type_iterator in the array will give us the next in line result type,
         * beginning with the smallest and increasing.
         */
        dbal_value_field_arr32_request(unit, sorted_result_types_entry_handle_id, DBAL_FIELD_SORTED_RESULT_TYPES,
                                       result_type_iterator, &result_type_idx);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, sorted_result_types_entry_handle_id, DBAL_COMMIT));

        /** If the result type is allowed for this lif, go on */
        if (SHR_BITGET(&(table_specific_rules.allowed_result_types_bmp), result_type_idx))
        {
            valid_result_type = TRUE;
            field_idx = 0;

            /** Loop on all required fields */
            while ((valid_result_type == TRUE)
                   && (field_idx < table->multi_res_info[table->nof_result_types].nof_result_fields))
            {
                int field_max_nof_bits, field_val_nof_bits, field_max_value;
                dbal_fields_e field_id;
                uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

                /** Get potential required field */
                field_id = table->multi_res_info[table->nof_result_types].results_info[field_idx].field_id;

                if (field_id == DBAL_FIELD_RESULT_TYPE)
                {
                    /** No need to check result type field */
                    field_idx++;
                    continue;
                }

                /** Check if the field is required */
                rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
                if (rv != _SHR_E_NONE)
                {
                    /** This field is not required, continue to the next */
                    field_idx++;
                    continue;
                }

                /** The field is required, check if exists in the result type we are checking */
                rv = dbal_tables_field_size_get(unit, dbal_table_id, field_id, FALSE, (int) result_type_idx,
                                                INST_SINGLE, &field_max_nof_bits);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

                if (rv == _SHR_E_NONE)
                {

                    /** check the size of the field */
                    if (field_max_nof_bits < 32)
                    {
                        SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, dbal_table_id,
                                                                        field_id, FALSE, (int) result_type_idx,
                                                                        INST_SINGLE, &field_max_value));
                        if (field_val[0] > field_max_value)
                        {
                            /** continue to next result type */
                            valid_result_type = FALSE;
                        }
                        else
                        {
                            /** If we reached this line it means that the field exists in the result type with sufficient size */
                            field_idx++;
                            valid_result_type = TRUE;
                        }
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(utilex_size_of_bitstream_in_bits
                                        (field_val, DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS,
                                         (uint32 *) &field_val_nof_bits));
                        if (field_val_nof_bits > field_max_nof_bits)
                        {
                            /** continue to next result type */
                            valid_result_type = FALSE;
                        }
                        else
                        {
                            /** If we reached this line it means that the field exists in the result type with sufficient size */
                            field_idx++;
                            valid_result_type = TRUE;
                        }
                    }
                }
                else
                {
                    /*
                     * continue to next result type 
                     */
                    valid_result_type = FALSE;
                }
            }

            field_idx = 0;
            /** Loop on all forbidden fields */
            while ((valid_result_type == TRUE) && (field_idx < table_specific_rules.forbidden_fields_nof))
            {
                dbal_table_field_info_t field_info;
                rv = dbal_tables_field_info_get_no_err(unit, dbal_table_id,
                                                       table_specific_rules.forbidden_fields_dbal_ids[field_idx], FALSE,
                                                       (int) result_type_idx, INST_SINGLE, &field_info);

                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
                /** We expect not to find the field, else this result type is not good */
                if (rv == _SHR_E_NONE)
                {
                    /*
                     * field exists, continue to next result type 
                     */
                    valid_result_type = FALSE;
                }
                else
                {
                    /** If we reached this line it means that the field does not exist in the result type */
                    valid_result_type = TRUE;
                    field_idx++;
                }
            }
            /** If we got here with valid result type it means all the required fields exist in current result type, and the forbidden do not exist. */
            /** No need to continue */
            if (valid_result_type == TRUE)
            {
                *result_type_decision = result_type_idx;
                break;
            }
        }
    }

    if (valid_result_type == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No result type found ");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * }
 */
