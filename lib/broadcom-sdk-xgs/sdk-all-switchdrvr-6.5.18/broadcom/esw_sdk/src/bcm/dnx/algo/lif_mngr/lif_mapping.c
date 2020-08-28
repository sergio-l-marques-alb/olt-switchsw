
/** \file lif_mapping.c
 *
 * This file contains the APIs required to set and get global to local lif mapping and vice versa.
 *  
 * In the DNX line of devices, lifs are used as logical interfaces on which packets are
 *   recieved and sent. The index for the physical tables that hold these lifs (inlif table
 *   and EEDB) are refered to as local lifs. However, the identifier of these lifs in the
 *   rest of the system is detached from the physical table index, and is refered to as the
 *   global lif.
 * Logically, Each allocated local lif is mapped to an allocated global lif, and vice versa.
 *   However, the HW only keeps two of these mappings: Ingress local to global, and egress
 *   global to local. SW users need the missing two mappings: Ingress global to local and
 *   egress local to global. This module maintains these mappings. In addition, it shadows the
 *   mappings that exist in the HW, to save HW calls during the driver's run.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
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
 * Hash tables must take a power of two for table size. This macro is used to convert 
 *   an arbitrary size into containing legitimate hash table size.
 *   It finds the smallest power of two which is larger than or equal to the input size.
 */
#define DNX_ALGO_LIF_HASH_TABLE_SIZE_CONVERT(_size) \
        (1 << utilex_log2_round_up(_size))

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
 * \brief
 *  Given global lif mapping flags, verify that exactly one of {DNX_ALGO_LIF_INGRESS, DNX_ALGO_LIF_EGRESS}
 *   is set.
 *
 * \see
 * DNX_ALGO_LIF_INGRESS
 * DNX_ALGO_LIF_EGRESS
 */
static shr_error_e
dnx_algo_lif_mapping_flags_verify(
    int unit,
    uint32 flags)
{
    uint8 is_ingress, is_egress;
    SHR_FUNC_INIT_VARS(unit);

    is_ingress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_INGRESS);
    is_egress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_EGRESS);

    /*
     *  Check that exactly one of is_ingress or is_egress flags is set.
     */
    if ((!is_ingress && !is_egress) || (is_ingress && is_egress))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Global lif mapping flags must contain either DNX_ALGO_LIF_INGRESS or DNX_ALGO_LIF_EGRESS. "
                     "Given %x08x", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 *  \brief
 *  Verification function for dnx_algo_lif_mapping_create.
 *  Check flags, and that lifs are in legal ID range.
 */
static shr_error_e
dnx_algo_lif_mapping_create_verify(
    int unit,
    uint32 flags,
    int global_lif,
    lif_mapping_local_lif_info_t * local_lif_info)
{
    uint8 is_ingress;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(local_lif_info, _SHR_E_PARAM, "local_lif_info");
    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_flags_verify(unit, flags));

    /*
     * Verify lif IDs are legal.
     */
    is_ingress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_INGRESS);

    if (is_ingress)
    {
        LIF_MNGR_GLOBAL_IN_LIF_VERIFY(unit, global_lif);
        LIF_MNGR_LOCAL_DPC_IN_LIF_VERIFY(unit, local_lif_info->local_lif);
    }
    else
    {
        /*
         * The function dnx_algo_lif_mapping_flags_verify checks that at least one of DNX_ALGO_LIF_INGRESS 
         *    or DNX_ALGO_LIF_EGRESS is set, so if is_ingress is FALSE, then is_egress must be true.
         */
        LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);
        LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, local_lif_info->local_lif);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mapping_create(
    int unit,
    uint32 flags,
    int global_lif,
    lif_mapping_local_lif_info_t * local_lif_info)
{
    int is_egress;
    uint8 success;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_algo_lif_mapping_create_verify(unit, flags, global_lif, local_lif_info));

    is_egress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_EGRESS);

    if (is_egress)
    {
        /*
         *  Global to local
         */
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                        egress_global_lif_to_lif_htb.insert(unit, &global_lif, &local_lif_info->local_lif, &success));
        if (success == 0)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            goto exit;
        }
        /*
         *  Local to global
         */
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                        egress_lif_to_global_lif_map.set(unit, local_lif_info->local_lif, global_lif));
    }
    else
    {
        /*
         * The function dnx_algo_lif_mapping_flags_verify inside dnx_algo_lif_mapping_create_verify checks that at least 
         * one of DNX_ALGO_LIF_INGRESS or DNX_ALGO_LIF_EGRESS is set, so if is_ingress is FALSE, then is_egress must be true.
         */
        /*
         *  Global to local
         */
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                        ingress_global_lif_to_lif_htb.insert(unit, &global_lif, local_lif_info, &success));
        if (success == 0)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            goto exit;
        }

        /*
         *  Local to global
         */
        if (local_lif_info->phy_table == DBAL_PHYSICAL_TABLE_INLIF_1)
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                            ingress_sbc_lif_to_global_lif_map.set(unit, local_lif_info->local_lif, global_lif));
        }
        else
        {
            int current_core, core_id = local_lif_info->core_id;
            DNXCMN_CORES_ITER(unit, core_id, current_core)
            {
                SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.ingress_dpc_lif_to_global_lif_map.set(unit, current_core,
                                                                                              local_lif_info->local_lif,
                                                                                              global_lif));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 *  \brief
 *  Verification function for dnx_algo_lif_mapping_remove.
 *  Check flags, and that lifs are in legal ID range.
 */
static shr_error_e
dnx_algo_lif_mapping_remove_verify(
    int unit,
    uint32 flags,
    int global_lif)
{
    int is_ingress;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_flags_verify(unit, flags));

    /*
     * Verify lif IDs are legal.
     */
    is_ingress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_INGRESS);

    if (is_ingress)
    {
        LIF_MNGR_GLOBAL_IN_LIF_VERIFY(unit, global_lif);
    }
    else
    {
        /*
         * The function dnx_algo_lif_mapping_flags_verify checks that at least one of DNX_ALGO_LIF_INGRESS 
         *    or DNX_ALGO_LIF_EGRESS is set, so if is_ingress is FALSE, then is_egress must be true.
         */
        LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mapping_remove(
    int unit,
    uint32 flags,
    int global_lif)
{
    uint8 is_ingress;
    lif_mapping_local_lif_info_t local_lif_info;
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get type and global/local direction
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_lif_mapping_remove_verify(unit, flags, global_lif));

    is_ingress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_INGRESS);

    sal_memset(&local_lif_info, 0, sizeof(local_lif_info));

    /*
     *  Remove mapping.
     */
    /*
     *  Remove the mapping from global to local lif.
     */
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                        ingress_global_lif_to_lif_htb.find(unit, &global_lif, &local_lif_info, &found));
        if (found == 0)
        {
            SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
        }
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.ingress_global_lif_to_lif_htb.delete(unit, &global_lif));
    }
    else
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                        egress_global_lif_to_lif_htb.find(unit, &global_lif, &local_lif_info.local_lif, &found));
        if (found == 0)
        {
            SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
        }
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.egress_global_lif_to_lif_htb.delete(unit, &global_lif));
    }

    /*
     *  Use the local lif we found in the map and remove the mapping from local to global lif.
     *  If the mapping wasn't found, then htb_find should return a _SHR_E_NOT_FOUND error
     *   and we won't reach this line, so no need to check the local lif.
     */
    if (is_ingress)
    {
        if (local_lif_info.phy_table == DBAL_PHYSICAL_TABLE_INLIF_1)
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                            ingress_sbc_lif_to_global_lif_map.set(unit, local_lif_info.local_lif, LIF_MNGR_INVALID));
        }
        else
        {
            int current_core, core_id;
            core_id = local_lif_info.core_id;
            DNXCMN_CORES_ITER(unit, core_id, current_core)
            {
                SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.ingress_dpc_lif_to_global_lif_map.set(unit, current_core,
                                                                                              local_lif_info.local_lif,
                                                                                              LIF_MNGR_INVALID));
            }
        }

    }
    else
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                        egress_lif_to_global_lif_map.set(unit, local_lif_info.local_lif, LIF_MNGR_INVALID));
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 *  \brief
 *  Verification function for dnx_algo_lif_mapping_local_to_global_get.
 *  Check flags, NULL pointers and that lifs are in legal ID range.
 */
static shr_error_e
dnx_algo_lif_mapping_local_to_global_get_verify(
    int unit,
    uint32 flags,
    lif_mapping_local_lif_info_t * local_lif_info,
    int *global_lif)
{
    int is_ingress;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(global_lif, _SHR_E_PARAM, "global_lif");
    SHR_NULL_CHECK(local_lif_info, _SHR_E_PARAM, "local_lif_info")
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_flags_verify(unit, flags));

    /*
     * Verify lif IDs are legal.
     */
    is_ingress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_INGRESS);

    if (is_ingress)
    {
        if (local_lif_info->phy_table == DBAL_PHYSICAL_TABLE_INLIF_1)
        {
            LIF_MNGR_LOCAL_SBC_IN_LIF_VERIFY(unit, local_lif_info->local_lif);
        }
        else if (local_lif_info->phy_table == DBAL_PHYSICAL_TABLE_INLIF_2)
        {
            int core_id = local_lif_info->core_id;
            DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);
            LIF_MNGR_LOCAL_DPC_IN_LIF_VERIFY(unit, local_lif_info->local_lif);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal physical table. Must be DBAL_PHYSICAL_TABLE_INLIF_[1|2]");
        }
    }
    else
    {
        /*
         * The function dnx_algo_lif_mapping_flags_verify checks that at least one of DNX_ALGO_LIF_INGRESS 
         *    or DNX_ALGO_LIF_EGRESS is set, so if is_ingress is FALSE, then is_egress must be true.
         */
        LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, local_lif_info->local_lif);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mapping_local_to_global_get(
    int unit,
    uint32 flags,
    lif_mapping_local_lif_info_t * local_lif_info,
    int *global_lif)
{
    int is_egress;

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Check input
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_lif_mapping_local_to_global_get_verify(unit, flags, local_lif_info, global_lif));

    /*
     * Get lif according to its direction.
     */
    is_egress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_EGRESS);

    if (local_lif_info->local_lif < dnx_data_l3.rif.nof_rifs_get(unit) && is_egress)
    {
        /*
         * If the given lif is actually an egress rif, then its global lif is the same as its local lif.
         */
        *global_lif = local_lif_info->local_lif;
        SHR_EXIT();
    }

    if (is_egress)
    {
        /*
         * Get global lif from the egress map.
         */
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                        egress_lif_to_global_lif_map.get(unit, local_lif_info->local_lif, global_lif));
    }
    else
    {
        /*
         * The function dnx_algo_lif_mapping_flags_verify checks that at least one of DNX_ALGO_LIF_INGRESS 
         *    or DNX_ALGO_LIF_EGRESS is set, so if is_egress is FALSE, then is_ingress must be true.
         */

        /*
         * Get ingress.
         */
        if (local_lif_info->phy_table == DBAL_PHYSICAL_TABLE_INLIF_1)
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                            ingress_sbc_lif_to_global_lif_map.get(unit, local_lif_info->local_lif, global_lif));
        }
        else
        {
            int core_id = local_lif_info->core_id;
            int current_core = (core_id == _SHR_CORE_ALL) ? 0 : local_lif_info->core_id;
            SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.ingress_dpc_lif_to_global_lif_map.get(unit, current_core,
                                                                                          local_lif_info->local_lif,
                                                                                          global_lif));
        }

    }

    if (*global_lif == LIF_MNGR_INVALID)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 *  \brief
 *  Verification function for dnx_algo_lif_mapping_global_to_local_get.
 *  Check flags, NULL pointers and that lifs are in legal ID range.
 */
static shr_error_e
dnx_algo_lif_mapping_global_to_local_get_verify(
    int unit,
    uint32 flags,
    int global_lif,
    lif_mapping_local_lif_info_t * local_lif_info)
{
    uint8 is_ingress;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(local_lif_info, _SHR_E_PARAM, "local_lif_info");

    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_flags_verify(unit, flags));

    /*
     * Verify lif IDs are legal.
     */
    is_ingress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_INGRESS);

    if (is_ingress)
    {
        LIF_MNGR_GLOBAL_IN_LIF_VERIFY(unit, global_lif);

        if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Global inlif 0x%x can't be smaller than the number of rifs: 0x%x", global_lif,
                         dnx_data_l3.rif.nof_rifs_get(unit));
        }
    }
    else
    {
        /*
         * The function dnx_algo_lif_mapping_flags_verify checks that at least one of DNX_ALGO_LIF_INGRESS 
         *    or DNX_ALGO_LIF_EGRESS is set, so if is_ingress is FALSE, then is_egress must be true.
         */
        LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mapping_global_to_local_get(
    int unit,
    int flags,
    int global_lif,
    lif_mapping_local_lif_info_t * local_lif_info)
{
    uint8 is_egress;
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check and analyze input. 
     */

    SHR_INVOKE_VERIFY_DNX(dnx_algo_lif_mapping_global_to_local_get_verify(unit, flags, global_lif, local_lif_info));
    sal_memset(local_lif_info, 0, sizeof(lif_mapping_local_lif_info_t));
    is_egress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_EGRESS);

    if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
    {
        /*
         * If the given lif is actually rif, then its global lif is the same as its local lif.
         */
        local_lif_info->local_lif = global_lif;
        SHR_EXIT();
    }

    if (is_egress)
    {
        /*
         * Find the mapped lif in the hash table.
         */
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                        egress_global_lif_to_lif_htb.find(unit, &global_lif, &(local_lif_info->local_lif), &found));
    }
    else
    {
        /*
         * Find the mapped lif in the hash table.
         */
        SHR_SET_CURRENT_ERR(lif_mngr_db.lif_mapping.
                            ingress_global_lif_to_lif_htb.find(unit, &global_lif, local_lif_info, &found));
    }

    if (found == 0)
    {
        local_lif_info->local_lif = LIF_MNGR_INVALID;
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mapping_init(
    int unit)
{
    int lif_map_size, max_nof_lifs;
    int lif;
    sw_state_htbl_init_info_t ingress_init_info;
    sw_state_htbl_init_info_t egress_init_info;
    int current_core;
    int nof_glem_lines, nof_dpc_local_inlifs, nof_sbc_local_inlifs;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&ingress_init_info, 0, sizeof(ingress_init_info));
    sal_memset(&egress_init_info, 0, sizeof(egress_init_info));

    /*
     * Create ingress local to global lif map, and fill it with invalid entries.
     * Once for sbc inlif, and once for dpc inlif.
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, &nof_sbc_local_inlifs, &nof_dpc_local_inlifs));

    lif_map_size = nof_sbc_local_inlifs;
    SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.ingress_sbc_lif_to_global_lif_map.alloc(unit, lif_map_size));

    for (lif = 0; lif < lif_map_size; lif++)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.ingress_sbc_lif_to_global_lif_map.set(unit, lif, LIF_MNGR_INVALID));
    }

    lif_map_size = nof_dpc_local_inlifs;
    SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.ingress_dpc_lif_to_global_lif_map.alloc(unit, lif_map_size));
    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, current_core)
    {
        for (lif = 0; lif < lif_map_size; lif++)
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.
                            ingress_dpc_lif_to_global_lif_map.set(unit, current_core, lif, LIF_MNGR_INVALID));
        }
    }

    /*
     * Create ingress global to local lif map. 
     * There's one to one mapping between global to local ingress lifs, so that's the limit on the number of 
     *  global lifs. 
     */
    max_nof_lifs = UTILEX_MAX(nof_sbc_local_inlifs, nof_dpc_local_inlifs);
    ingress_init_info.max_nof_elements = DNX_ALGO_LIF_HASH_TABLE_SIZE_CONVERT(max_nof_lifs);
    ingress_init_info.expected_nof_elements = DNX_ALGO_LIF_HASH_TABLE_SIZE_CONVERT(max_nof_lifs);
    ingress_init_info.hash_function = NULL;
    ingress_init_info.print_cb_name = "dnx_algo_lif_mapping_htb_entry_print";

    SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.ingress_global_lif_to_lif_htb.create(unit, &ingress_init_info));

    /*
     * Create egress local to global lif map.
     */
    lif_map_size = dnx_data_lif.out_lif.nof_local_out_lifs_get(unit);
    SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.egress_lif_to_global_lif_map.alloc(unit));

    for (lif = 0; lif < lif_map_size; lif++)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.egress_lif_to_global_lif_map.set(unit, lif, LIF_MNGR_INVALID));
    }

    /*
     * Create egress global to local lif map. 
     * The physical constraint on egress global lifs is the size of the GLEM. 
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_glem_lines_get(unit, &nof_glem_lines));

    egress_init_info.max_nof_elements = DNX_ALGO_LIF_HASH_TABLE_SIZE_CONVERT(nof_glem_lines);
    egress_init_info.expected_nof_elements = DNX_ALGO_LIF_HASH_TABLE_SIZE_CONVERT(nof_glem_lines);
    egress_init_info.hash_function = NULL;
    egress_init_info.print_cb_name = "dnx_algo_lif_mapping_htb_entry_print";
    SHR_IF_ERR_EXIT(lif_mngr_db.lif_mapping.egress_global_lif_to_lif_htb.create(unit, &egress_init_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mapping_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state memory free is done automatically at device deinit 
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
