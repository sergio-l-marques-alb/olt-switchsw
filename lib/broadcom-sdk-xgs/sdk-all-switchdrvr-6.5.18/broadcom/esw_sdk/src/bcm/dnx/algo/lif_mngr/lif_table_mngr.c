/*
 * $Copyright: (c) 2019 Broadcom.
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
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <soc/dnx/swstate/auto_generated/types/algo_lif_types.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include "lif_mngr_internal.h"

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */
#define LIF_TABLE_MANAGER_INVALID_RESULT_TYPE (-1)
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
extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

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
 * Similar to dnx_algo_lif_table_mngr_decide_result_type but with is_test option
 * is_test option is used to hint which fields can be omitted to get valid result
 */
static shr_error_e dnx_algo_lif_table_mngr_decide_result_type_internal(
    int unit,
    uint32 table_specific_flags,
    lif_mngr_outlif_phase_e outlif_phase,
    uint32 entry_handle_id,
    uint32 *result_type_decision,
    uint8 is_test);

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
                    
                    if ((dbal_table_id == DBAL_TABLE_EEDB_ARP)
                        && (dbal_result_type == DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC_1TAG_STAT))
                    {
                        result_types_arr[dbal_result_type].result_type_size += 1;
                    }
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
dnx_lif_table_mngr_table_specific_flags_fill_ingress_ac_non_native(
    int unit,
    int result_type,
    uint32 *table_specific_flags)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (result_type)
    {
            /*
             * AC VLAN-PORT non-native P2P:
             *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_1_VLAN
             *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_VSI_W_1_VLAN
             *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION
             *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE
             *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P
             */
            
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_1_VLAN:
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_VSI_W_1_VLAN:
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION:
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE:
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P:
        {
            *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P;
            break;
        }
        default:
            break;
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
 * \brief - Set specific rules for result type filtering per
 *        table - egress IPV4 tunnel
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
dnx_lif_table_mngr_table_specific_flags_fill_egress_ipv4_tunnel(
    int unit,
    int result_type,
    uint32 *table_specific_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Here adding constraints per table for special tables */

    /** Allowed result types */
    if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_GRE_GENEVE)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_GRE_ANY_IN_4;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_IP_ANY_IN_4;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_BFD_IPV4_SIP_WA)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_BFD_SIP_WA;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_UDP;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_GPE;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN)
    {
        *table_specific_flags |= (DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_GPE | DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_ECN);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result type (%d) is not supported", result_type);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per
 *        table - egress IPV4 tunnel
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] result_type - Result type of the current lif
 *   \param [in] outlif_phase - lif mngr outlif phase of current lif, invalid for inlif
 *   \param [out] table_specific_flags - filled in this function
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_table_specific_flags_fill_egress_ipv6_tunnel(
    int unit,
    int result_type,
    uint32 outlif_phase,
    uint32 *table_specific_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Here adding constraints per table for special tables */

    /** Allowed result types */
    if (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_RAW)
    {
        if (outlif_phase == LIF_MNGR_OUTLIF_PHASE_IPV6_RAW_SRV6_TUNNEL)
        {
            *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_SRV6_IP;
        }
        else
        {
            *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_IP_ANY_IN_6;
        }
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_GRE)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_GRE_ANY_IN_6;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_UDP)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_UDP_6;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN)
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_GPE_6;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN_ECN)
    {
        *table_specific_flags |= (DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_GPE_6 | DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_ECN);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result type (%d) is not supported", result_type);

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
    SHR_FUNC_INIT_VARS(unit);

    /** Here adding constraints per table for special tables */

    /** Allowed result types */
    if ((result_type == DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC)
        || (result_type == DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC_1TAG_STAT))
    {
        *table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION;
    }

    SHR_EXIT();

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

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per table: ingress AC non-native
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
dnx_lif_table_mngr_table_specific_result_type_filtering_ingress_ac_non_native(
    int unit,
    uint32 table_specific_flags,
    lif_table_mngr_table_specific_rules_t * table_specific_rules)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P))
    {
        /*
         * AC VLAN-PORT non-native P2P:
         *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC2EEI         
         *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_1_VLAN
         *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_VSI_W_1_VLAN
         *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION
         *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE
         *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P
         */
        
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC2EEI);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE_GENERIC_DATA);

    }
    else
    {
        /*
         * AC VLAN-PORT non-native not P2P:
         *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP
         *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE
         *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA
         *  - DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_RCH_CONTROL_LIF
         */
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp), DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_RCH_CONTROL_LIF);

    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per
 *        table: PON_DTC_CLASSIFICATION
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
dnx_lif_table_mngr_table_specific_result_type_filtering_ingress_pon_ac_non_native(
    int unit,
    uint32 table_specific_flags,
    lif_table_mngr_table_specific_rules_t * table_specific_rules)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P))
    {
        /*
         * AC VLAN-PORT non-native P2P:
         *  - DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC2EEI         
         *  - DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_P2P_W_1_VLAN
         *  - DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_P2P_W_VSI_W_1_VLAN
         *  - DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION
         *  - DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_P2P_LARGE
         *  - DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_P2P
         */
        
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC2EEI);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_P2P);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_P2P_LARGE);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_P2P_LARGE_GENERIC_DATA);

    }
    else
    {
        /*
         * AC VLAN-PORT non-native not P2P:
         *  - DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_MP
         *  - DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_MP_LARGE
         *  - DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_MP_LARGE_GENERIC_DATA
         */
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_MP);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_MP_LARGE);
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_PON_DTC_CLASSIFICATION_IN_LIF_AC_MP_LARGE_GENERIC_DATA);

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
 * \brief - Set specific rules for result type filtering per 
 *        table: egress eedb_ipv4_tunnel
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
dnx_lif_table_mngr_table_specific_result_type_filtering_egress_ipv4_tunnel(
    int unit,
    uint32 table_specific_flags,
    lif_table_mngr_table_specific_rules_t * table_specific_rules)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Here adding constraints per table for special tables */

    /** Allowed result types */
    if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_GRE_ANY_IN_4))
    {
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_GRE_GENEVE);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_IP_ANY_IN_4))
    {
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_UDP))
    {
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_BFD_SIP_WA))
    {
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_BFD_IPV4_SIP_WA);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_GPE))
    {
        if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_ECN))
        {
            SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                       DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN);
        }
        else
        {
            SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                       DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table specific flags (%d) is not supported", table_specific_flags);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set specific rules for result type filtering per 
 *        table: egress eedb_ipv6_tunnel
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
dnx_lif_table_mngr_table_specific_result_type_filtering_egress_ipv6_tunnel(
    int unit,
    uint32 table_specific_flags,
    lif_table_mngr_table_specific_rules_t * table_specific_rules)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Here adding constraints per table for special tables */

    /** Allowed result types */
    if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_IP_ANY_IN_6) ||
        _SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_SRV6_IP))
    {
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_RAW);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_GRE_ANY_IN_6))
    {
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_GRE);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_UDP_6))
    {
        SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                   DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_UDP);
    }
    else if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_GPE_6))
    {
        if (_SHR_IS_FLAG_SET(table_specific_flags, DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_ECN))
        {
            SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                       DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN_ECN);
        }
        else
        {
            SHR_BITSET(&(table_specific_rules->allowed_result_types_bmp),
                       DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN);
        }
    }
    else if (table_specific_flags == 0)
    {
        /** For srv6 tunnel, no specific flags. All result types are
         *  allowed  */
        int nof_res_types;
        SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get(unit, DBAL_TABLE_EEDB_IPV6_TUNNEL, &nof_res_types));
        SHR_BITSET_RANGE(&(table_specific_rules->allowed_result_types_bmp), 0, nof_res_types);

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table specific flags (%d) is not supported", table_specific_flags);

    }

    SHR_EXIT();

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
        /** Disallow the result types above (nof labels and PHP are done through forbidden fields) */
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
 * See .h file
 */
shr_error_e
dnx_lif_table_mngr_table_specific_result_type_filtering(
    int unit,
    uint32 entry_handle_id,
    uint32 table_specific_flags,
    lif_table_mngr_table_specific_rules_t * table_specific_rules)
{
    int nof_res_types;
    dbal_tables_e dbal_table_id;
    uint32 forbidden_fields_per_table_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(table_specific_rules, 0, sizeof(lif_table_mngr_table_specific_rules_t));
    SHR_BITCLR_RANGE(&(table_specific_rules->allowed_result_types_bmp), 0, DBAL_MAX_NUMBER_OF_RESULT_TYPES);

    /*
     * Take DBAL table ID from handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

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
    else if (dbal_table_id == DBAL_TABLE_EEDB_IPV4_TUNNEL)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering_egress_ipv4_tunnel
                        (unit, table_specific_flags, table_specific_rules));
    }
    else if (dbal_table_id == DBAL_TABLE_EEDB_IPV6_TUNNEL)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering_egress_ipv6_tunnel
                        (unit, table_specific_flags, table_specific_rules));
    }
    else if (dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering_ingress_ac_non_native
                        (unit, table_specific_flags, table_specific_rules));
    }
    else if (dbal_table_id == DBAL_TABLE_PON_DTC_CLASSIFICATION)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering_ingress_pon_ac_non_native
                        (unit, table_specific_flags, table_specific_rules));
    }
    else
    {
        /** No constraints, set all result types of this table */
        /** Get number of result types of this table */
        SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get(unit, dbal_table_id, &nof_res_types));
        SHR_BITSET_RANGE(&(table_specific_rules->allowed_result_types_bmp), 0, nof_res_types);
    }
    /*
     * Forbidden fields filtering: Read FORBIDDEN action fields per dbal table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_FORDIDDEN_FIELDS_PER_TABLE,
                                      &forbidden_fields_per_table_handle_id));

    dbal_entry_key_field32_set(unit, forbidden_fields_per_table_handle_id, DBAL_FIELD_DBAL_TABLE, dbal_table_id);
    dbal_value_field_arr32_request(unit, forbidden_fields_per_table_handle_id, DBAL_FIELD_FORBIDDEN_FIELDS_BMP,
                                   INST_SINGLE, table_specific_rules->forbidden_fields_idx_bitmap);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, forbidden_fields_per_table_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
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
    uint32 outlif_phase,
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
    else if (dbal_table_id == DBAL_TABLE_EEDB_IPV4_TUNNEL)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_flags_fill_egress_ipv4_tunnel
                        (unit, result_type, table_specific_flags));
    }
    else if (dbal_table_id == DBAL_TABLE_EEDB_IPV6_TUNNEL)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_flags_fill_egress_ipv6_tunnel
                        (unit, result_type, outlif_phase, table_specific_flags));
    }
    else if (dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_flags_fill_ingress_ac_non_native
                        (unit, result_type, table_specific_flags));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print debug info in case result type wasn't found.
 * Print options for fields that can be omitted to get valid result
 *
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - table
 *   \param [in] table_specific_flags - specific flags requested by the user
 *   \param [in] outlif_phase - logical outlif phase
 *   \param [in] entry_handle_id - original handle id containing the fields to set
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *
 */
static shr_error_e
dnx_algo_lif_table_mngr_no_result_type_found_debug(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 table_specific_flags,
    lif_mngr_outlif_phase_e outlif_phase,
    uint32 entry_handle_id)
{
    int field_idx, field_idx_to_ignore;
    shr_error_e rv;
    CONST dbal_logical_table_t *table;
    uint8 hint_found = FALSE;
    char str_tmp[256];
    char msg[10000] = { 0 };
    size_t strn_size = 256;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(str_tmp, 0, sizeof(str_tmp));
    sal_strncat_s(msg, "\n****************** LIF TABLE MANAGER DEBUG INFO ******************\n", sizeof(msg));
    sal_snprintf(str_tmp, strn_size, "COULD NOT FIND RESULT TYPE IN TABLE %s CONTAINING THE FOLLOWING FIELDS: \r\n",
                 dbal_logical_table_to_string(unit, dbal_table_id));
    sal_strncat_s(msg, str_tmp, sizeof(msg));

    /** Get table information */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** Loop on all required fields */
    for (field_idx = 0; field_idx < table->multi_res_info[table->nof_result_types].nof_result_fields; field_idx++)
    {
        dbal_fields_e field_id;
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

        /** Get potential required field */
        field_id = table->multi_res_info[table->nof_result_types].results_info[field_idx].field_id;

        if (field_id == DBAL_FIELD_RESULT_TYPE)
        {
            /** No need to print result type field */
            continue;
        }

        /** Check if the field is required */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            uint32 field_val_nof_bits;

            /** This field is required */
            SHR_IF_ERR_EXIT(utilex_bitstream_size_of_bitstream_in_bits
                            (field_val, DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS, (uint32 *) &field_val_nof_bits));

            sal_memset(str_tmp, 0, sizeof(str_tmp));
            sal_snprintf(str_tmp, strn_size, "%s: size %d \r\n", dbal_field_to_string(unit, field_id),
                         field_val_nof_bits);
            sal_strncat_s(msg, str_tmp, sizeof(msg));
        }

    }

    sal_memset(str_tmp, 0, sizeof(str_tmp));
    sal_snprintf(str_tmp, strn_size, "\nTable specific flags value: %d \r\n\n", table_specific_flags);
    sal_strncat_s(msg, str_tmp, sizeof(msg));

    /*
     * Trying to omit one field to see if a result can be found
     */
    for (field_idx_to_ignore = 0;
         field_idx_to_ignore < table->multi_res_info[table->nof_result_types].nof_result_fields; field_idx_to_ignore++)
    {
        dbal_fields_e field_id_to_ignore;
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

        /** Get potential required field */
        field_id_to_ignore = table->multi_res_info[table->nof_result_types].results_info[field_idx_to_ignore].field_id;

        if (field_id_to_ignore == DBAL_FIELD_RESULT_TYPE)
        {
            /** No need to print result type field */
            continue;
        }

        /** Check if the field is required */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id_to_ignore, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            /*
             * Try omitting this field and find a result type
             */
            uint32 result_type_decision;
            uint32 test_entry_handle_id;

            /*
             * Copy all fields beside the chosen one to a new handle
             */
            DBAL_HANDLE_COPY(unit, entry_handle_id, &test_entry_handle_id);
            dbal_entry_value_field_unset(unit, test_entry_handle_id, field_id_to_ignore, INST_SINGLE);

            /*
             * Try choosing result type
             */
            SHR_IF_ERR_EXIT(dnx_algo_lif_table_mngr_decide_result_type_internal
                            (unit, table_specific_flags, outlif_phase, test_entry_handle_id, &result_type_decision,
                             TRUE));
            if (result_type_decision != LIF_TABLE_MANAGER_INVALID_RESULT_TYPE)
            {
                if (hint_found == FALSE)
                {
                    sal_strncat_s(msg, "HINT: One of these fields can be omitted to receive a successful result: \r\n",
                                  sizeof(msg));
                    hint_found = TRUE;
                }
                sal_strncat_s(msg, dbal_field_to_string(unit, field_id_to_ignore), sizeof(msg));
                sal_strncat_s(msg, "\r\n", sizeof(msg));
            }
            DBAL_HANDLE_FREE(unit, test_entry_handle_id);
        }
    }
    if (hint_found == FALSE)
    {
        sal_strncat_s(msg,
                      "HINT: Couldn't find a single field that can be omitted to receive a successful result. \r\n",
                      sizeof(msg));
    }
    sal_strncat_s(msg, "HINT: Make sure all the mandatory fields are configured. \r\n", sizeof(msg));
    sal_strncat_s(msg, "\n****************** END OF LIF TABLE MANAGER DEBUG INFO ******************\n", sizeof(msg));

    LOG_WARN_EX(BSL_LOG_MODULE, "%s %s%s%s\r\n", msg, EMPTY, EMPTY, EMPTY);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Evaluate a single field and value for acceptance in
 * a specific result type. The function is called for every user
 * supplied field that is on the DBAL handle prior to calling
 * the LIF table manager API to decide the appropriate result
 * type.
 * The function is called for each result type that is
 * considered by the decision mechanism.
 * Only fields up to 32 bits are supported.
 *
 *   \param [in] unit - unit id
 *   \param [in] field_val - pointer to a field value
 *   \param [in] table_field_info - A pointer to the static
 *          field information in the context of the specific
 *          result type.
 *   \param [out] is_valid_result_type - The evaluation result -
 *          The field is either valid for the result type or
 *          not.
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_algo_lif_table_mngr_decide_result_type_internal
 */
static shr_error_e
dnx_algo_lif_table_mngr_evaluate_field32(
    int unit,
    uint32 *field_val,
    dbal_table_field_info_t * table_field_info,
    uint8 *is_valid_result_type)
{
    uint32 encoded_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 arr_stripped_field_value_ptr[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *is_valid_result_type = TRUE;

    /*
     * Avoid range checks for fields over 32 bits or L2 addresses of any length.
     * In those cases, the field is accepted for the result type selection and                                                          .
     * it's the user's responsibility to provide an in-range value.
     */
    if ((table_field_info->field_nof_bits > SAL_UINT32_NOF_BITS) ||
        (SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ADDRESS)))
    {
        return _SHR_E_NONE;
    }

    /*
     * Verify that the field's logical value matches the field's attributes in the specific result type. 
     */
    if ((table_field_info->arr_prefix_size == 0) ||
        (SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM)))
    {
        /*
         * Verify that the logical value is in the defined range for the field
         */
        if ((field_val[0] > table_field_info->max_value) || (field_val[0] < table_field_info->min_value))
        {
            *is_valid_result_type = FALSE;
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                    "result_type_selection out-of-range for field %s(%d), value %d, min_val %d, max_val %d !\n"),
                                         dbal_field_to_string(unit, table_field_info->field_id),
                                         table_field_info->field_id, field_val[0], table_field_info->min_value,
                                         table_field_info->max_value));
            return _SHR_E_NONE;
        }
    }
    /*
     * In case of an arr-prefix field that isn't an ENUM an ARR-Prefix decoding is required in order to 
     * check that the value meets the ARR-Prefix requirements. 
     * The decoding function also performs the same logical value verification. 
     */
    else
    {
        arr_stripped_field_value_ptr[0] = field_val[0];
        rv = dbal_field_arr_prefix_decode(unit, table_field_info, arr_stripped_field_value_ptr[0],
                                          arr_stripped_field_value_ptr, FALSE);
        if (SHR_FAILURE(rv))
        {
            /*
             * Failed decoding implies that the field value didn't mathch the ARR Prefix or
             * that their aren't enough actual bits in the specific field 
             */
            *is_valid_result_type = FALSE;
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                    "result_type_selection ARR-Prefix out-of-range for field_id %s(%d), value %d !\n"),
                                         dbal_field_to_string(unit, table_field_info->field_id),
                                         table_field_info->field_id, arr_stripped_field_value_ptr[0]));
            return _SHR_E_NONE;
        }
    }

    /*
     * For ENUM values, verify that the encoded value is also in range
     */
    if (SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM))
    {
        /*
         * Perform the ENUM encoding
         */
        SHR_IF_ERR_EXIT(dbal_fields_field_encode(unit, table_field_info->field_id, 0, table_field_info->field_nof_bits,
                                                 field_val, encoded_field_value));

        /*
         * For ENUMs with ARR_Prefix perform an ARR-Prefix decode that also performs an ARR-Prefix
         * validation and a range check 
         */
        if ((table_field_info->arr_prefix_size != 0))
        {
            arr_stripped_field_value_ptr[0] = encoded_field_value[0];
            rv = dbal_field_arr_prefix_decode(unit, table_field_info, arr_stripped_field_value_ptr[0],
                                              arr_stripped_field_value_ptr, FALSE);

            if (SHR_FAILURE(rv))
            {
                /*
                 * Failed decoding implies that the field value didn't mathch the ARR Prefix or
                 * that their aren't enough actual bits in the specific field 
                 */
                *is_valid_result_type = FALSE;
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                        "result_type_selection ENUM ARR-Prefix out-of-range for field %s(%d), value %d !\n"),
                                             dbal_field_to_string(unit, table_field_info->field_id),
                                             table_field_info->field_id, arr_stripped_field_value_ptr[0]));
                return _SHR_E_NONE;
            }
        }
        /*
         * For ENUMs with no ARR_Prefix perform a range check with the actual size of the field
         */
        else
        {
            uint32 enum_max_encoded_val = (table_field_info->field_nof_bits == SAL_UINT32_NOF_BITS) ?
                SAL_UINT32_MAX : SAL_UPTO_BIT(table_field_info->field_nof_bits);
            if (encoded_field_value[0] > enum_max_encoded_val)
            {
                *is_valid_result_type = FALSE;
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                        "result_type_selection ENUM out-of-range for field %s(%d), value %d,  max_val %d !\n"),
                                             dbal_field_to_string(unit, table_field_info->field_id),
                                             table_field_info->field_id, encoded_field_value[0], enum_max_encoded_val));
                return _SHR_E_NONE;
            }
        }
    }

    return _SHR_E_NONE;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Similar to dnx_algo_lif_table_mngr_decide_result_type but with is_test option
 * is_test option is used to hint which fields can be omitted to get valid result
 */
static shr_error_e
dnx_algo_lif_table_mngr_decide_result_type_internal(
    int unit,
    uint32 table_specific_flags,
    lif_mngr_outlif_phase_e outlif_phase,
    uint32 entry_handle_id,
    uint32 *result_type_decision,
    uint8 is_test)
{
    int field_idx, result_type_iterator;
    lif_table_mngr_table_specific_rules_t table_specific_rules;
    shr_error_e rv;
    uint8 valid_result_type = FALSE;
    CONST dbal_logical_table_t *table;
    uint32 sorted_result_types_entry_handle_id;
    uint32 result_type_idx;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Use table specific filtering of result types for tables where additional rules should be applied */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering
                    (unit, entry_handle_id, table_specific_flags, &table_specific_rules));
    /** Take DBAL table ID from handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    /** Get table information */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
    /** Iterate over all possible result types */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_LIF_TABLES_RESULT_TYPE_SORTED, &sorted_result_types_entry_handle_id));
    dbal_entry_key_field32_set(unit, sorted_result_types_entry_handle_id, DBAL_FIELD_DBAL_TABLE, dbal_table_id);
    for (result_type_iterator = 0; result_type_iterator < table->nof_result_types; result_type_iterator++)
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

            /** Loop on all required fields by checking all possible fields in the superset result type */
            while ((valid_result_type == TRUE)
                   && (field_idx < table->multi_res_info[table->nof_result_types].nof_result_fields))
            {
                dbal_fields_e field_id;
                uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
                uint8 is_forbidden = 0, is_field_required;
                dbal_table_field_info_t table_field_info;

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
                if (rv == _SHR_E_NONE)
                {
                    is_field_required = TRUE;
                }
                else
                {
                    dbal_to_phase_info_t dbal_to_phase_info;
                    dnx_local_outlif_ll_indication_e ll_indication = DNX_LOCAL_OUTLIF_LL_OPTIONAL;
                    uint8 ll_indication_found = FALSE;
                    dnx_algo_local_outlif_logical_phase_e outlif_logical_phase;

                    sal_memset(&dbal_to_phase_info, 0, sizeof(dbal_to_phase_info_t));

                    /**If field is outlif next pointer and user didn't set it - check if it's not mandatory on this result type*/
                    if (field_id == DBAL_FIELD_NEXT_OUTLIF_POINTER)
                    {
                        dbal_to_phase_info.dbal_result_type = result_type_idx;
                        dbal_to_phase_info.dbal_table = dbal_table_id;
                        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num
                                        (unit, outlif_phase, &outlif_logical_phase));
                        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                                        dbal_valid_combinations.find(unit, outlif_logical_phase, &dbal_to_phase_info,
                                                                     &ll_indication, &ll_indication_found));

                        if ((ll_indication_found) && (ll_indication_found == DNX_LOCAL_OUTLIF_LL_ALWAYS))
                        {
                            /**link list is mandatory on this result type, and user didn't set it --> can't use this RT*/
                            valid_result_type = FALSE;
                            break;
                        }
                    }

                    /** This field is not required, check if it's forbidden */
                    is_field_required = FALSE;
                    is_forbidden =
                        SHR_BITGET(table_specific_rules.forbidden_fields_idx_bitmap, field_idx) ? TRUE : FALSE;
                    if (!is_forbidden)
                    {
                        /** Not required and not forbidden - this field is not a constraint on result type decision */
                        field_idx++;
                        continue;
                    }
                }

                /** Check if field exists in the result type we are considering */
                rv = dbal_tables_field_info_get_no_err(unit, dbal_table_id, field_id, FALSE,
                                                       result_type_idx, 0, &table_field_info);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

                if (is_field_required && (rv == _SHR_E_NONE))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_lif_table_mngr_evaluate_field32
                                    (unit, field_val, &table_field_info, &valid_result_type));
                    field_idx++;
                }
                else if (((is_field_required && (rv == _SHR_E_NOT_FOUND)) ||
                          (!is_field_required && is_forbidden && (rv == _SHR_E_NONE))))
                {
                    /*
                     * Either field is required and does not exist on result type, or forbidden and does exist.
                     * This result type cannot be used, continue to next result type
                     */
                    valid_result_type = FALSE;
                }
                else
                {
                    /*
                     * Else: Field is forbidden and not found on entry - this is good! Check next field
                     */
                    field_idx++;
                }
            }
            /*
             * If we got here with valid result type it means all the required fields exist in current result type,
             * and the forbidden do not exist. No need to continue
             */
            if (valid_result_type == TRUE)
            {
                *result_type_decision = result_type_idx;
                break;
            }
        }
    }

    if (valid_result_type == FALSE)
    {
        /*
         * Result type not found - if not test return debug info,
         * else set result_type_decision to invalid
         */
        if (is_test == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_lif_table_mngr_no_result_type_found_debug
                            (unit, dbal_table_id, table_specific_flags, outlif_phase, entry_handle_id));
            SHR_ERR_EXIT(_SHR_E_PARAM, "No result type found \r\n");
        }
        else
        {
            *result_type_decision = LIF_TABLE_MANAGER_INVALID_RESULT_TYPE;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_algo_lif_table_mngr_decide_result_type(
    int unit,
    uint32 table_specific_flags,
    lif_mngr_outlif_phase_e outlif_phase,
    uint32 entry_handle_id,
    uint32 *result_type_decision)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_lif_table_mngr_decide_result_type_internal
                    (unit, table_specific_flags, outlif_phase, entry_handle_id, result_type_decision, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
