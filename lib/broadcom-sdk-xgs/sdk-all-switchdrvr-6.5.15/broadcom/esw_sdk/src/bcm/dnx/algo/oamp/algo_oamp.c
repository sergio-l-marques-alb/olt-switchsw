/** \file algo_oamp.c
 * $Id$
 *
 * Resource and templates needed for the OAMP OAM and BFD features.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <bcm/types.h>
#include <bcm/oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <soc/dnx/swstate/auto_generated/access/algo_oam_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_oamp_access.h>

/*
 * }
 */
/*
 * DEFINEs
 * {
 */

/** Pool name where mep_id is located according group name type and entry type */
#define DNX_OAMP_MEP_ID_RESOURCE_NAME(is_2byte_maid, is_full, is_extra_pool) ( \
                              (is_extra_pool)              ? DNX_ALGO_OAMP_EXTRA_MEP_ID         : \
                             ((is_2byte_maid && is_full)   ? DNX_ALGO_OAMP_2B_MAID_FULL_MEP_ID  : \
                             (((is_2byte_maid && !is_full) ? DNX_ALGO_OAMP_2B_MAID_SHORT_MEP_ID : \
                             (((!is_2byte_maid && is_full) ? DNX_ALGO_OAMP_ICC_MAID_FULL_MEP_ID : \
                                                             DNX_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID))))))

/** Resource callback where mep_id is located according group name type and entry type */
#define DNX_OAMP_MEP_ID_RESOURCE(is_2byte_maid, is_full, is_extra_pool, exec) ( \
                              (is_extra_pool)              ? SW_STATE_ALGO_OAMP_EXTRA_MEP_ID.exec         : \
                             ((is_2byte_maid && is_full)   ? SW_STATE_ALGO_OAMP_2B_MAID_FULL_MEP_ID.exec  : \
                             (((is_2byte_maid && !is_full) ? SW_STATE_ALGO_OAMP_2B_MAID_SHORT_MEP_ID.exec : \
                             (((!is_2byte_maid && is_full) ? SW_STATE_ALGO_OAMP_ICC_MAID_FULL_MEP_ID.exec : \
                                                             SW_STATE_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID.exec))))))

/** Nunber of entries in relevant hardware tables */
#define NOF_ITMH_PRIORITY_PROFILES     8
#define NOF_MPLS_PWE_TTL_EXP_PROFILES 16
#define NOF_PUNT_PROFILES             16
#define NOF_MEP_PROFILES              128

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/**
 * \brief - Function map mep_id to pool and index inside pool.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  mep_id   - mep id
 * \param [out] memory_type - map id memory type
 * \param [out] index    - index of mep id inside pool
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_oam_mep_id_alloc
 *   dnx_oam_mep_id_dealloc
 *   dnx_oamp_mep_db_memory_type_t
 */
static void
dnx_mep_id_to_pool_type_and_index(
    int unit,
    uint32 mep_id,
    dnx_oamp_mep_db_memory_type_t * memory_type,
    int *index)
{
    int mep_db_treshold;
    int nof_umc_entries;
    int nof_mep_db_short_entries;
    int nof_max_endpoints;

/**
 * MEP ID could reside in one of 5 Pools
 * S_ICC- Quarter entry with ICC MAID Long
 * S_2B - Quarter entry with 2 Byte MAID
 * F_2B-  Full Long   with ICC MAID Long
 * F_ICC- Full Short  with 2 Byte MAID
 * EXTRA - LM DM Pool (entry used for LM DM pool
 *
 * x(y) - x is endpoint id and y is index inside pool
 * each nep_db address could be related to one pool only(after threshold ep%4 != 0 couldn't be allocated
 *
 * UMC threshold = 8K
 * EXTRA resides in bank 9-12(each bank 12K)
 *     EXTRA start = 16K
 *
 * There two options regarding the threshold and number of umc(ICC MAID groups) that could be allocated
 * See below the example:
 *
 * Option 1:
 *                                     MEP DB
 *                      ------------- ------------ ------------ ------------
 *                     | S_ICC 0(0)  | S_2B 1(0)  | S_2B 2(1)  | S_2B 3(2)  |
 *                      ------------- ------------ ------------ ------------
 *                     | S_ICC 4(1)  | S_2B 5(3)  | S_2B 6(4)  | S_2B 7(5)  |
 *                      ------------- ------------ ------------ ------------
 *                     | S_ICC 8(2)  | S_2B 9(6)  | S_2B 10(7) | S_2B 11(8) |
 *                      ---------- ---------- ---------- -------------------
 *                     | S_ICC 12(3) | S_2B 13(9) | S_2B 14(10)| S_2B 15(11)|
 * THRESHOLD   ---->    ------------- ------------ ------------ ------------
 *                     | F_2B 16(0)                                         |
 *                      ----------------------------------------------------
 *                     | F_2B 20(1)                                         |
 * UMC THR ---->        ----------------------------------------------------
 *                     | F_ICC 24(0)                                        |
 *                      ----------------------------------------------------
 *                     | F_ICC 28(1)                                        |
 * EXTRA start --->     ----------------------------------------------------
 *                     | EXTRA 32(0)                                        |
 *                      ----------------------------------------------------
 *                     | EXTRA 36(1)                                        |
 *                      ----------------------------------------------------
 *
 * Option 2:
 *                     ------------- ------------ ------------ ------------
 *                    | S_ICC 0(0)  | S_2B 1(0)  | S_2B 2(1)  | S_2B 3(2)  |
 *                     ------------- ------------ ------------ ------------
 *                    | S_ICC 4(1)  | S_2B 5(3)  | S_2B 6(4)  | S_2B 7(5)  |
 *                     ------------- ------------ ------------ ------------
 *                    | S_ICC 8(2)  | S_2B 9(6)  | S_2B 10(7) | S_2B 11(8) |
 *                     ------------- ------------ ------------ ------------
 *                    | S_ICC 12(3) | S_2B 13(9) | S_2B 14(10)| S_2B 15(11)|
 *UMC THR      ---->   ------------- ------------ ------------ ------------
 *                    | S_2B 16(12) | S_2B 17(13)| S_2B 18(14)| S_2B 22(15)|
 *                     ------------- ------------ ------------ ------------
 *                    | S_2B 20(16) | S_2B 21(17)| S_2B 22(18)| S_2B 26(19)|
 *THRESHOLD ---->      ----------------------------------------------------
 *                    | F_ICC 24(0)                                        |
 *                     ----------------------------------------------------
 *                    | F_ICC 28(1)                                        |
 * EXTRA start --->    ----------------------------------------------------
 *                    | EXTRA 32(0)                                        |
 *                     ----------------------------------------------------
 *                    | EXTRA 36(1)                                        |
 *                     ----------------------------------------------------
*/

    mep_db_treshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_umc_entries = dnx_data_oam.oamp.nof_umc_get(unit);
    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    nof_max_endpoints = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

    if (mep_id >= nof_max_endpoints)
    {
        memory_type->is_extra_pool = 1;
        *index = (mep_id - nof_max_endpoints) / nof_mep_db_short_entries;
    }
    else
    {
        /** Do not assume this field is 0! */
        memory_type->is_extra_pool = 0;

        /** Quarter Pools area */
        if (mep_id < mep_db_treshold)
        {
            memory_type->is_full = 0;
            /** Quarter Long */
            if ((mep_id % nof_mep_db_short_entries == 0) && (mep_id / nof_mep_db_short_entries <= nof_umc_entries))
            {
                memory_type->is_2byte_maid = 0;
                *index = mep_id / nof_mep_db_short_entries;
            }
            /** Quarter Short */
            else
            {
                memory_type->is_2byte_maid = 1;
                /** We are located in area where one ID relate to ICC MAID pool and 3 ID's to Short pool */
                if (mep_id / nof_mep_db_short_entries < nof_umc_entries)
                {
                    *index = mep_id - (mep_id / nof_mep_db_short_entries) - 1;
                }
                /** We are located in area where all four ID's are from 2 Byte MAID Pool(it could be only on Option 2) */
                else
                {
                    *index = mep_id - nof_umc_entries;
                }
            }
        }
        /** Full Pools area */
        else
        {
            memory_type->is_full = 1;
            if (mep_id >= nof_umc_entries * nof_mep_db_short_entries)
            {
                memory_type->is_2byte_maid = 1;
                /**Full 2 Byte MAID area  Option 2 */
                if (mep_db_treshold / nof_mep_db_short_entries > nof_umc_entries)
                {
                    *index = (mep_id - mep_db_treshold) / nof_mep_db_short_entries;
                }
                /**Full 2 Byte MAID area  Option 1 */
                else
                {
                    *index = mep_id / nof_mep_db_short_entries - nof_umc_entries;
                }
            }
            /** Option 2 (between UMC and and Threshold) */
            else
            {
                memory_type->is_2byte_maid = 0;
                *index = (mep_id - mep_db_treshold) / nof_mep_db_short_entries;
            }
        }
    }
}

/**
 * \brief - Mapping pool type and index inside the pool to mep id.
 *          Used for endpoint id allocation.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] memory_type - type of mep_id memory
 * \param [in] index - mep_id index inside the pool.
 * \param [out] mep_id   - mep id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_oam_mep_id_alloc
 *   dnx_oamp_mep_db_memory_type_t
 */
static void
dnx_pool_type_and_index_to_mep_id(
    int unit,
    dnx_oamp_mep_db_memory_type_t * memory_type,
    int index,
    uint32 *mep_id)
{
    int nof_mep_db_short_entries;
    int nof_umc_entries;
    int mep_db_treshold;
    int nof_max_endpoints;

    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    nof_umc_entries = dnx_data_oam.oamp.nof_umc_get(unit);
    mep_db_treshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_max_endpoints = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

/**
 * MEP ID could reside in one of 5 Pools
 * S_ICC- Quarter entry with ICC MAID Long
 * S_2B - Quarter entry with 2 Byte MAID
 * F_2B-  Full Long   with ICC MAID Long
 * F_ICC- Full Short  with 2 Byte MAID
 * EXTRA - Extra Pool (entry used for Additional Data/Statistics pool
 *
 * x(y) - x is endpoint id and y is index inside pool
 * each mep_db address could be related to one pool only(after threshold ep%4 != 0 couldn't be allocated
 *
 * UMC threshold = 8K
 * EXTRA resides in bank 9-12(each bank 12K)
 *     EXTRA start = 16K
 *
 * There two options regarding the threshold and number of umc(ICC MAID groups) that could be allocated
 * See below the example:
 *
 * Option 1:
 *                                     MEP DB
 *                      ------------- ------------ ------------ ------------
 *                     | S_ICC 0(0)  | S_2B 1(0)  | S_2B 2(1)  | S_2B 3(2)  |
 *                      ------------- ------------ ------------ ------------
 *                     | S_ICC 4(1)  | S_2B 5(3)  | S_2B 6(4)  | S_2B 7(5)  |
 *                      ------------- ------------ ------------ ------------
 *                     | S_ICC 8(2)  | S_2B 9(6)  | S_2B 10(7) | S_2B 11(8) |
 *                      ---------- ---------- ---------- -------------------
 *                     | S_ICC 12(3) | S_2B 13(9) | S_2B 14(10)| S_2B 15(11)|
 * THRESHOLD   ---->    ------------- ------------ ------------ ------------
 *                     | F_2B 16(0)                                         |
 *                      ----------------------------------------------------
 *                     | F_2B 20(1)                                         |
 * UMC THR ---->        ----------------------------------------------------
 *                     | F_ICC 24(0)                                        |
 *                      ----------------------------------------------------
 *                     | F_ICC 28(1)                                        |
 * EXTRA start --->     ----------------------------------------------------
 *                     | EXTRA 32(0)                                        |
 *                      ----------------------------------------------------
 *                     | EXTRA 36(1)                                        |
 *                      ----------------------------------------------------
 *
 * Option 2:
 *                     ------------- ------------ ------------ ------------
 *                    | S_ICC 0(0)  | S_2B 1(0)  | S_2B 2(1)  | S_2B 3(2)  |
 *                     ------------- ------------ ------------ ------------
 *                    | S_ICC 4(1)  | S_2B 5(3)  | S_2B 6(4)  | S_2B 7(5)  |
 *                     ------------- ------------ ------------ ------------
 *                    | S_ICC 8(2)  | S_2B 9(6)  | S_2B 10(7) | S_2B 11(8) |
 *                     ------------- ------------ ------------ ------------
 *                    | S_ICC 12(3) | S_2B 13(9) | S_2B 14(10)| S_2B 15(11)|
 *UMC THR      ---->   ------------- ------------ ------------ ------------
 *                    | S_2B 16(12) | S_2B 17(13)| S_2B 18(14)| S_2B 22(15)|
 *                     ------------- ------------ ------------ ------------
 *                    | S_2B 20(16) | S_2B 21(17)| S_2B 22(18)| S_2B 26(19)|
 *THRESHOLD ---->      ----------------------------------------------------
 *                    | F_ICC 24(0)                                        |
 *                     ----------------------------------------------------
 *                    | F_ICC 28(1)                                        |
 * EXTRA start --->     ----------------------------------------------------
 *                    | EXTRA 32(0)                                        |
 *                     ----------------------------------------------------
 *                    | EXTRA 36(1)                                        |
 *                     ----------------------------------------------------
*/

    if (memory_type->is_extra_pool)
    {
        *mep_id = nof_max_endpoints + index * nof_mep_db_short_entries;
    }
    else
    {
        /** F_ICC area */
        if (memory_type->is_2byte_maid && memory_type->is_full)
        {
            /** Option 1 */
            if (mep_db_treshold / nof_mep_db_short_entries < nof_umc_entries)
            {
                *mep_id = (nof_umc_entries + index) * nof_mep_db_short_entries;
            }
            /** Option 2 */
            else
            {
                *mep_id = mep_db_treshold + index * nof_mep_db_short_entries;
            }
        }
        /** S_2B area */
        if (memory_type->is_2byte_maid && !memory_type->is_full)
        {
            /** We are located in area where one ID relate to ICC MAID pool and 3 ID's to 2 Byte MAID pool */
            if (index < nof_umc_entries * DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE)
            {
                *mep_id = (index / DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE) * nof_mep_db_short_entries +
                    ((index % DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE) + 1);
            }
            /** We are located in area where all 4 ID's related to 2 Byte MAID pool */
            else
            {
                *mep_id = nof_umc_entries * nof_mep_db_short_entries +
                    (index - nof_umc_entries * DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE);
            }
        }
        /** F_2B area */
        if (!memory_type->is_2byte_maid && memory_type->is_full)
        {
            *mep_id = mep_db_treshold + index * nof_mep_db_short_entries;
        }
        /** S_ICC area */
        if (!memory_type->is_2byte_maid && !memory_type->is_full)
        {
            *mep_id = index * nof_mep_db_short_entries;
        }
    }
}

/**
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_algo_oamp_mep_id_alloc(
    int unit,
    uint32 flags,
    dnx_oamp_mep_db_memory_type_t * mep_id_memory_type,
    uint32 *mep_id)
{
    int rv;
    int alloc_flags = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * In case BCM_OAM_PROFILE_WITH_ID flag is specified, allocate profile with profile_id
     */
    if (flags & SW_STATE_ALGO_RES_ALLOCATE_WITH_ID)
    {
        alloc_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
        dnx_mep_id_to_pool_type_and_index(unit, *mep_id, mep_id_memory_type, &index);

        SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE(mep_id_memory_type->is_2byte_maid,
                                                 mep_id_memory_type->is_full, mep_id_memory_type->is_extra_pool,
                                                 allocate_single(unit, alloc_flags, NULL, &index)));
    }
    else
    {
        if (mep_id_memory_type->is_2byte_maid)
        {
            /** Check if the Short pool is full. */
            alloc_flags = SW_STATE_ALGO_RES_ALLOCATE_SIMULATION;
            rv = DNX_OAMP_MEP_ID_RESOURCE(mep_id_memory_type->is_2byte_maid, mep_id_memory_type->is_full, 0,
                                          allocate_single(unit, alloc_flags, NULL, &index));
            if (rv == _SHR_E_NONE)
            {
                alloc_flags = 0;
                /** The Short pool is not full, so get Short pool free index*/
                SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE
                                (mep_id_memory_type->is_2byte_maid, mep_id_memory_type->is_full, 0,
                                 allocate_single(unit, alloc_flags, NULL, &index)));
            }
            else if (rv == _SHR_E_RESOURCE)
            {
                /** Get Long pool, as Short pool doesn't have free index */
                mep_id_memory_type->is_2byte_maid = 0;
                alloc_flags = 0;
                SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE
                                (0, mep_id_memory_type->is_full, 0, allocate_single(unit, alloc_flags, NULL, &index)));
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE
                            (mep_id_memory_type->is_2byte_maid, mep_id_memory_type->is_full, 0,
                             allocate_single(unit, alloc_flags, NULL, &index)));
        }
        /** Get mep_id according to index */
        dnx_pool_type_and_index_to_mep_id(unit, mep_id_memory_type, index, mep_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_algo_oamp_mep_id_is_allocated(
    int unit,
    uint32 mep_id,
    uint8 *is_alloc)
{
    int index;
    dnx_oamp_mep_db_memory_type_t mep_id_memory_type;

    SHR_FUNC_INIT_VARS(unit);

    dnx_mep_id_to_pool_type_and_index(unit, mep_id, &mep_id_memory_type, &index);

    SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE(mep_id_memory_type.is_2byte_maid,
                                             mep_id_memory_type.is_full, mep_id_memory_type.is_extra_pool,
                                             is_allocated(unit, index, is_alloc)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_algo_oamp_mep_id_dealloc(
    int unit,
    uint32 mep_id)
{
    int index;
    dnx_oamp_mep_db_memory_type_t mep_id_memory_type;

    SHR_FUNC_INIT_VARS(unit);

    /** Get pool type and MEP ID index inside pool from MEP ID ID */
    dnx_mep_id_to_pool_type_and_index(unit, mep_id, &mep_id_memory_type, &index);

    SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE(mep_id_memory_type.is_2byte_maid,
                                             mep_id_memory_type.is_full, mep_id_memory_type.is_extra_pool,
                                             free_single(unit, index)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the resource configuration
 *        required for the OAM/BFD endpoint id allocation. 5 different
 *        pools should be created according pool types.
 *        The size is calculated according threshold of MEP_DB.
 *        --------------------------------------------------------------------------------------
 *       | Pool                   |  Threshold < umc table size   | Threshold >= umc table size |
 *        --------------------------------------------------------------------------------------
 *       | Short ICC MAID(S_ICC)  | threshold                     |    umc table size           |
 *       | Short 2Byte MAID(S_2B) | 3*threshold                   |  3*umc table size           |
 *       | Long ICC MAID(L_ICC)   | umc table size - threshold    |           0                 |
 *       | Long 2Byte MAID(L_2B)  | max_nof_ep - umc table size   |   mep db size - threshold   |
 *       | EXTRA                  |                    mep db size - max_nof_ep                 |
 *        --------------------------------------------------------------------------------------
 *
 *
 * x(y) - x is mep id and y is index inside pool
 * each nep_db address could be related to one pool only(after threshold ep%4 != 0 couldn't be allocated
 *
 * There two options regarding the threshold and number of umc(ICC MAID groups) that could be allocated
 * See below:
 *
 * Option 1:
 *                                     MEP DB
 *                      ------------- ------------ ------------ ------------
 *                     | S_ICC 0(0)  | S_2B 1(0)  | S_2B 2(1)  | S_2B 3(2)  |
 *                      ------------- ------------ ------------ ------------
 *                     | S_ICC 4(1)  | S_2B 5(3)  | S_2B 6(4)  | S_2B 7(5)  |
 *                      ------------- ------------ ------------ ------------
 *                     | S_ICC 8(2)  | S_2B 9(6)  | S_2B 10(7) | S_2B 11(8) |
 *                      ------------- ------------ ------------ ------------
 *                     | S_ICC 12(3) | S_2B 13(9) | S_2B 14(10)| S_2B 15(11)|
 * THRESHOLD   ---->    ------------- ------------ ------------ ------------
 *                     | L_ICC 16(0)                                        |
 *                      ------------- ------------ ------------ ------------
 *                     | L_ICC 20(1)                                        |
 * UMC THR ---->        ------------- ------------ ------------ ------------
 *                     | L_2B 24(0)                                         |
 *                      ------------- ------------ ------------ ------------
 *                     | L_2B 28(1)                                         |
 * EXTRA  ----->       ------------- ------------ ------------ ------------
 *                     | EXTRA 32(0)                                        |
 *                      ------------- ------------ ------------ ------------
 *                     | EXTRA 36(1)                                        |
 *                      ------------- ------------ ------------ ------------
 *
 * Option 2:
 *                     ------------ ------------ ------------ ------------
 *                    | QL 0(0)    | S_2B 1(0)  | S_2B 2(1)  | S_2B 3(2)  |
 *                     ------------ ------------ ------------ ------------
 *                    | QL 4(1)    | S_2B 5(3)  | S_2B 6(4)  | S_2B 7(5)  |
 *                     ------------ ------------ ------------ ------------
 *                    | QL 8(2)    | S_2B 9(6)  | S_2B 10(7) | S_2B 11(8) |
 *                     ------------ ------------ ------------ ------------
 *                    | QL 12(3)   | S_2B 13(9) | S_2B 14(10)| S_2B 15(11)|
 * UMC THR     ---->   ------------ ------------ ------------ ------------
 *                    | S_2B 16(12)| S_2B 17(13)| S_2B 18(14)| S_2B 22(15)|
 *                     ------------ ------------ ------------ ------------
 *                    | S_2B 20(16)| S_2B 21(17)| S_2B 22(18)| S_2B 26(19)|
 * THRESHOLD ---->     ------------ ------------ ------------ ------------
 *                    | L_2B 24(0)                                        |
 *                     ------------ ------------ ------------ ------------
 *                    | L_2B 28(1)                                        |
 * EXTRA  ----->       ------------- ------------ ------------ -----------
 *                    | EXTRA 32(0)                                       |
 *                     ------------- ------------ ------------ -----------
 *                    | EXTRA 36(1)                                       |
 *                     ------------- ------------ ------------ -----------

 *
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oamp_mep_id_init(
    int unit)
{

    sw_state_algo_res_create_data_t data;
    int mep_db_treshold;
    int nof_umc;
    int oamp_max_nof_mep_id;
    int oamp_max_nof_ep_id;
    int nof_mep_db_short_entries;
    int nof_short_lines;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));

    mep_db_treshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (mep_db_treshold % 4 != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Full entry threshold need to be multiples of 4\r\n");
    }
    nof_umc = dnx_data_oam.oamp.nof_umc_get(unit);
    oamp_max_nof_mep_id = dnx_data_oam.oamp.max_nof_mep_id_get(unit);
    oamp_max_nof_ep_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    nof_short_lines = mep_db_treshold / nof_mep_db_short_entries;

    data.flags = 0;
    /*
     * First element id
     */
    data.first_element = 0;

    /*
     * init the algo_oapm sw state module.
     */
    SHR_IF_ERR_EXIT(algo_oamp_db.init(unit));
    /*
     * Configure the size of all EP pools.
     */
    if (nof_short_lines < nof_umc)
    {
        data.nof_elements = nof_short_lines;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_icc_maid_short_mep_id.create(unit, &data, NULL));
        }

        data.nof_elements = nof_short_lines * DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_2B_MAID_SHORT_MEP_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_2b_maid_short_mep_id.create(unit, &data, NULL));
        }

        data.nof_elements = nof_umc - nof_short_lines;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_ICC_MAID_FULL_MEP_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_icc_maid_full_mep_id.create(unit, &data, NULL));
        }

        data.nof_elements = oamp_max_nof_ep_id / nof_mep_db_short_entries - nof_umc;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_2B_MAID_FULL_MEP_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_2b_maid_full_mep_id.create(unit, &data, NULL));
        }
    }
    else
    {
        data.nof_elements = nof_umc;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_icc_maid_short_mep_id.create(unit, &data, NULL));
        }

        data.nof_elements =
            DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE * nof_umc + (mep_db_treshold -
                                                                     nof_umc * nof_mep_db_short_entries);
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_2B_MAID_SHORT_MEP_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_2b_maid_short_mep_id.create(unit, &data, NULL));
        }

        data.nof_elements = (oamp_max_nof_ep_id / nof_mep_db_short_entries) - nof_short_lines;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_2B_MAID_FULL_MEP_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_2b_maid_full_mep_id.create(unit, &data, NULL));
        }

    }

    data.nof_elements = (oamp_max_nof_mep_id - oamp_max_nof_ep_id) / nof_mep_db_short_entries;
    sal_strncpy(data.name, DNX_ALGO_OAMP_EXTRA_MEP_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oamp_db.oamp_extra_mep_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oamp_pp_port_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    uint16 system_port = *(uint16 *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT16, "System port", system_port, NULL, "0x%08X");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);

    return;
}

/**
 * \brief - This function creates the template needed for mapping local port to system port
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oamp_pp_port_2_sys_port_template_init(
    int unit)
{
    sw_state_algo_template_create_data_t oamp_port_map_profile_data;
    uint32 oamp_max_nof_endpoint_id;
    uint32 oamp_port_map_table_size;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    oamp_max_nof_endpoint_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    oamp_port_map_table_size = dnx_data_oam.oamp.local_port_2_system_port_size_get(unit);

    /** Create template for OAMP ICC map table */
    sal_memset(&oamp_port_map_profile_data, 0, sizeof(sw_state_algo_template_create_data_t));
    oamp_port_map_profile_data.flags = 0;
    oamp_port_map_profile_data.first_profile = 0;
    oamp_port_map_profile_data.nof_profiles = oamp_port_map_table_size;
    oamp_port_map_profile_data.max_references = oamp_max_nof_endpoint_id;
    oamp_port_map_profile_data.default_profile = 0;
    oamp_port_map_profile_data.data_size = sizeof(uint16);
    oamp_port_map_profile_data.default_data = NULL;
    sal_strncpy(oamp_port_map_profile_data.name, DNX_ALGO_OAMP_PP_PORT_2_SYS_PORT_TEMPLATE,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oamp_db.oamp_pp_port_2_sys_port.create(unit, &oamp_port_map_profile_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oam_punt_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_oam_oamp_punt_event_profile_t *punt_profile = (dnx_oam_oamp_punt_event_profile_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "punt_enable", punt_profile->punt_enable,
                                        "Indicates whether punt is enabled", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "punt_rate", punt_profile->punt_rate,
                                        "Rate of sampling packets (with events) to CPU", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "rx_state_update_en",
                                        punt_profile->rx_state_update_en,
                                        "Refer dbal_enum_value_field_profile_rx_state_update_en_e", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "profile_scan_state_update_en",
                                        punt_profile->profile_scan_state_update_en,
                                        "Refer dbal_enum_value_field_profile_scan_state_update_en_e", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "mep_rdi_update_loc_en",
                                        punt_profile->mep_rdi_update_loc_en,
                                        "Enable/disable MEP DB RDI indication in case LOC detected", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "mep_rdi_update_loc_clear_en",
                                        punt_profile->mep_rdi_update_loc_clear_en,
                                        "Enable/disable MEP DB RDI indication in case LOC Clear detected", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "mep_rdi_update_rx_en",
                                        punt_profile->mep_rdi_update_rx_en,
                                        "Enable/disable MEP DB RDI indication by copying RDI from rx pkt", NULL);

    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
}

/**
 * \brief - This function creates the template needed for the 
 *        Punt profile management
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *      Punt profiles: mirror the entries in
 *      the OAMP_PUNT_EVENT_HENDLING available for Punt information\n
 *  
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oam_punt_profile_template_init(
    int unit)
{
    sw_state_algo_template_create_data_t oam_punt_handling_profile_data;
    uint32 nof_references = 0;
    uint32 nof_scan_rmep_entries = 0;
    uint32 rmep_db_treshold = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_scan_rmep_entries = dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) + 1;
    rmep_db_treshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    if (rmep_db_treshold < nof_scan_rmep_entries)
    {
        nof_references = rmep_db_treshold + (nof_scan_rmep_entries - rmep_db_treshold) / 2;
    }
    else
    {
        nof_references = nof_scan_rmep_entries;
    }

    /** Create template for BFD OAMA Profiles */
    sal_memset(&oam_punt_handling_profile_data, 0, sizeof(sw_state_algo_template_create_data_t));
    oam_punt_handling_profile_data.flags = 0;
    oam_punt_handling_profile_data.first_profile = DNX_ALGO_PUNT_TABLE_START_ID;
    oam_punt_handling_profile_data.nof_profiles = DNX_ALGO_PUNT_TABLE_COUNT;
    oam_punt_handling_profile_data.max_references = nof_references;
    oam_punt_handling_profile_data.default_profile = 0;
    oam_punt_handling_profile_data.data_size = sizeof(dnx_oam_oamp_punt_event_profile_t);
    oam_punt_handling_profile_data.default_data = NULL;
    sal_strncpy(oam_punt_handling_profile_data.name, DNX_ALGO_OAMP_PUNT_EVENT_PROFILE_TEMPLATE,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.create(unit, &oam_punt_handling_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oam_itmh_priority_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_oam_itmh_priority_t *itmh_priority = (dnx_oam_itmh_priority_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "tc", itmh_priority->tc, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "dp", itmh_priority->dp, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
    return;
}

/**
 * \brief - This function creates the template for the ITMH 
 *        TC/DP profiles.  This resource is used by OAM and BFD
 *        accelerated endpoints in the ITMH header of
 *        transmitted packets.
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oam_oamp_itmh_priority_profile_id_init(
    int unit)
{
    sw_state_algo_template_create_data_t itmh_priority_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&itmh_priority_profile_data, 0, sizeof(sw_state_algo_template_create_data_t));
    itmh_priority_profile_data.flags = 0;
    itmh_priority_profile_data.first_profile = 0;
    itmh_priority_profile_data.nof_profiles = NOF_ITMH_PRIORITY_PROFILES;
    itmh_priority_profile_data.max_references = dnx_data_oam.general.oam_nof_oamp_meps_get(unit);
    itmh_priority_profile_data.default_profile = 0;
    itmh_priority_profile_data.data_size = sizeof(dnx_oam_itmh_priority_t);
    itmh_priority_profile_data.default_data = NULL;
    sal_strncpy(itmh_priority_profile_data.name, DNX_OAM_TEMPLATE_ITMH_PRIORITY,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.create(unit, &itmh_priority_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oam_mpls_pwe_exp_ttl_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_oam_ttl_exp_profile_t *profile = (dnx_oam_ttl_exp_profile_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "ttl", profile->ttl, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "exp", profile->exp, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
    return;
}

/**
 * \brief - This function creates the template for the OAM/BFD 
 *        MPLS/PWE TTL/EXP profiles.  This resource is used by OAM
 *        and BFD MPLS/PWE accelerated endpoints to construct
 *        transmitted packets.
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oam_oamp_mpls_pwe_ttl_exp_profile_id_init(
    int unit)
{
    sw_state_algo_template_create_data_t mpls_pwe_exp_ttl_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&mpls_pwe_exp_ttl_profile_data, 0, sizeof(sw_state_algo_template_create_data_t));
    mpls_pwe_exp_ttl_profile_data.flags = 0;
    mpls_pwe_exp_ttl_profile_data.first_profile = 0;
    mpls_pwe_exp_ttl_profile_data.nof_profiles = NOF_MPLS_PWE_TTL_EXP_PROFILES;
    mpls_pwe_exp_ttl_profile_data.max_references = dnx_data_oam.general.oam_nof_oamp_meps_get(unit);
    mpls_pwe_exp_ttl_profile_data.default_profile = 0;
    mpls_pwe_exp_ttl_profile_data.data_size = sizeof(dnx_oam_ttl_exp_profile_t);
    mpls_pwe_exp_ttl_profile_data.default_data = NULL;
    sal_strncpy(mpls_pwe_exp_ttl_profile_data.name, DNX_OAM_TEMPLATE_MPLS_PWE_EXP_TTL_PROFILE,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.create(unit, &mpls_pwe_exp_ttl_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_algo_oam_oamp_mep_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_oam_mep_profile_t *profile = (dnx_oam_mep_profile_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "piggy_back_lm", profile->piggy_back_lm, NULL,
                                        "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "slm_lm", profile->slm_lm, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "maid_check_dis", profile->maid_check_dis, NULL,
                                        "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "report_mode_lm", profile->report_mode_lm, NULL,
                                        "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "report_mode_dm", profile->report_mode_dm, NULL,
                                        "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "rdi_gen_method", profile->rdi_gen_method, NULL,
                                        "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "dmm_rate", profile->dmm_rate, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "lmm_rate", profile->lmm_rate, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "opcode_0_rate", profile->opcode_0_rate, NULL,
                                        "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "opcode_1_rate", profile->opcode_1_rate, NULL,
                                        "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "dmm_offset", profile->dmm_offset, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "dmr_offset", profile->dmr_offset, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "lmm_offset", profile->lmm_offset, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "lmr_offset", profile->lmr_offset, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "ccm_count", profile->ccm_count, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "dmm_count", profile->dmm_count, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "lmm_count", profile->lmm_count, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "opcode_0_count", profile->opcode_0_count, NULL,
                                        "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "opcode_1_count", profile->opcode_1_count, NULL,
                                        "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "dm_measurement_type",
                                        profile->dm_measurement_type, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
    return;
}

/**
 * \brief - This function creates the resource needed to allocate
 *          entries in the OAMP_MEP_ID table
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *

 * \see
 *   * None
 */
static shr_error_e
dnx_algo_acc_oam_res_init(
    int unit)
{
    sw_state_algo_res_create_data_t acc_mep_res_data;
    SHR_FUNC_INIT_VARS(unit);

    acc_mep_res_data.first_element = 0;
    acc_mep_res_data.flags = 0;
    acc_mep_res_data.nof_elements = dnx_data_oam.oamp.max_nof_mep_id_get(unit);
    acc_mep_res_data.advanced_algorithm = 0;
    sal_strncpy(acc_mep_res_data.name, DNX_ALGO_OAMP_MEP_DB_ENTRY, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.create(unit, &acc_mep_res_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the template needed for the 
 *          OAMP MEP profile management
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oam_oamp_mep_profile_id_init(
    int unit)
{
    sw_state_algo_template_create_data_t mep_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&mep_profile_data, 0, sizeof(sw_state_algo_template_create_data_t));
    mep_profile_data.flags = 0;
    mep_profile_data.first_profile = 0;
    mep_profile_data.nof_profiles = NOF_MEP_PROFILES;
    mep_profile_data.max_references = dnx_data_oam.general.oam_nof_oamp_meps_get(unit);
    mep_profile_data.default_profile = 0;
    mep_profile_data.data_size = sizeof(dnx_oam_mep_profile_t);
    mep_profile_data.default_data = NULL;
    sal_strncpy(mep_profile_data.name, DNX_OAM_TEMPLATE_MEP_PROFILE, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.create(unit, &mep_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize templates for all oamp profile types.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_algo_oamp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oamp_pp_port_2_sys_port_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_punt_profile_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_oamp_itmh_priority_profile_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_oamp_mpls_pwe_ttl_exp_profile_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_acc_oam_res_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_oamp_mep_profile_id_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Nothing to do here.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_algo_oamp_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP ID alloc/free
 *          This function helps calculate the allocated/freed id
 *          to be used in the alloc mngr for above threshold ids.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - RMEP id
 * \param [out] alloc_id - alloc_mngr id
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_id_above_threshold_to_alloc_mngr_id(
    int unit,
    int rmep_idx,
    int *alloc_id)
{
    uint32 self_contained_threshold;
    int temp_alloc_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    temp_alloc_id = rmep_idx - self_contained_threshold;
    /** Above the threshold, RMEP ID must be an even number */
    if (temp_alloc_id % 2 != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ids above self contained threshold need to be multiples of 2\r\n");
    }
    temp_alloc_id = temp_alloc_id / 2;
    *alloc_id = temp_alloc_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP creation
 *          This function deletes the new RMEP ID
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_id - ID of the remote endpoint.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_remote_endpoint_id_free(
    int unit,
    bcm_oam_endpoint_t rmep_id)
{
    uint32 self_contained_threshold;
    int alloc_id;

    SHR_FUNC_INIT_VARS(unit);

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    if (rmep_id < self_contained_threshold)
    {
        /*
         * Remove from below threshold pool
         */

        SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_below_threshold.free_single(unit, rmep_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_above_threshold_to_alloc_mngr_id(unit, rmep_id, &alloc_id));
        /*
         * Remove from above threshold pool
         */

        SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_above_threshold.free_single(unit, alloc_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP creation
 *          This function allocates the new RMEP ID
 *          and handles WITH_ID case as well.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] alloc_flags - flags variable to be used
 *             for resource allocation
 * \param [in,out] rmep_id - Resulting ID.  If ID is
 *        specified, this is an output argument.
 *        Otherwise, it's an input argument.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_remote_endpoint_id_alloc(
    int unit,
    int alloc_flags,
    bcm_oam_endpoint_t * rmep_id)
{
    uint32 self_contained_threshold;
    int alloc_id;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    /** allocate the remote endpoint ID */
    if (_SHR_IS_FLAG_SET(alloc_flags, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID))
    {
        /*
         * in case BCM_OAM_ENDPOINT_WITH_ID flag exists, allocate RMEP id, according to endpoint_info->id
         */
        if (*rmep_id < self_contained_threshold)
        {
            /*
             * Allocate from below threshold pool
             */
            SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_below_threshold.allocate_single(unit, alloc_flags, NULL, rmep_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_above_threshold_to_alloc_mngr_id(unit, *rmep_id, &alloc_id));
            /*
             * Allocate from above threshold pool
             */
            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_rmep_id_above_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id));
        }
    }
    else
    {
        /*
         * First try to allocate from above threshold. If resource is not there, then allocate
         * from below threshold.
         */
        rv = algo_oam_db.oam_rmep_id_above_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id);
        if (rv == _SHR_E_RESOURCE)
        {
            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_rmep_id_below_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id));
            *rmep_id = alloc_id;
        }
        else if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocate above threshold failed with %d\r\n", rv);
        }
        else
        {
            /*
             * Allocated from above threshold. The endpoint id is 2 times the allocated number + threshold
             */
            *rmep_id = (alloc_id * 2) + self_contained_threshold;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in algo_oamp.h
 */
shr_error_e
dnx_algo_oamp_mep_entry_alloc(
    int unit,
    int *oamp_entry_index)
{
    int full_entry_threshold, short_entries_in_full, short_index, temp_index;
    SHR_FUNC_INIT_VARS(unit);

    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (*oamp_entry_index >= full_entry_threshold)
    {
        /** Allocate full entry; reject index that isn't aligned */
        short_entries_in_full = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
        if ((*oamp_entry_index % short_entries_in_full) == 0)
        {
            for (short_index = 0; short_index < short_entries_in_full; short_index++)
            {
                        /** Allocate enough space for a full entry */
                temp_index = *oamp_entry_index + short_index;
                SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.allocate_single
                                (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, NULL, &temp_index));
            }
        }
        else
        {
                /** Index is not aligned */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: Index 0x%08X is above the threshold 0x%08X but not aligned.  Must be a multiple of %d\n",
                         *oamp_entry_index, full_entry_threshold, short_entries_in_full);
        }
    }
    else
    {
        /** Allocate 1 (short) entry */
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.allocate_single
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, NULL, oamp_entry_index));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in algo_oamp.h
 */
shr_error_e
dnx_algo_oamp_mep_entry_dealloc(
    int unit,
    int oamp_entry_index)
{
    int full_entry_threshold, short_entries_in_full, short_index;
    SHR_FUNC_INIT_VARS(unit);

    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (oamp_entry_index >= full_entry_threshold)
    {
        /** Free full entry; reject index that isn't aligned */
        short_entries_in_full = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
        if ((oamp_entry_index % short_entries_in_full) == 0)
        {
            for (short_index = 0; short_index < short_entries_in_full; short_index++)
            {
                        /** Free the space occupied by a full entry */
                SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.free_single(unit, oamp_entry_index + short_index));
            }
        }
        else
        {
                /** Index is not aligned */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: Index 0x%08X is above the threshold 0x%08X but not aligned.  Must be a multiple of %d\n",
                         oamp_entry_index, full_entry_threshold, short_entries_in_full);
        }
    }
    else
    {
        /** Free 1 (short) entry */
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.free_single(unit, oamp_entry_index));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in algo_oamp.h
 */
shr_error_e
dnx_oam_oamp_get_next_icc_entry(
    int unit,
    uint32 *icc_id_in_out,
    uint8 *entry_found)
{
    int mep_db_threshold, nof_mep_db_short_entries, index;
    SHR_FUNC_INIT_VARS(unit);
    *entry_found = FALSE;

    /**
     * Search is possible only if threshold is non-zero.
     * If threshold is 0, short mep banks are not created
     * and the get_next function will cause the system to
     * crash.
     */
    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (mep_db_threshold > 0)
    {
        /** Find this entry index in the pool */
        nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
        index = *icc_id_in_out / nof_mep_db_short_entries;

        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_icc_maid_short_mep_id.get_next(unit, &index));
        if (index != SW_STATE_ALGO_RES_ILLEGAL_ELEMENT)
        {
            *entry_found = TRUE;
            *icc_id_in_out = nof_mep_db_short_entries * index;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in algo_oamp.h
 */
shr_error_e
dnx_algo_oamp_mep_id_get_free_icc_id(
    int unit,
    uint32 *icc_id_out,
    uint8 *entry_not_found)
{
    int index, mep_db_threshold, nof_mep_db_short_entries, nof_short_lines;
    uint8 is_alloc;
    SHR_FUNC_INIT_VARS(unit);
    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    nof_short_lines = mep_db_threshold / nof_mep_db_short_entries;

    *entry_not_found = TRUE;
    for (index = 0; index < nof_short_lines; index++)
    {
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_icc_maid_short_mep_id.is_allocated(unit, index, &is_alloc));
        if (is_alloc == FALSE)
        {
            *entry_not_found = FALSE;
            *icc_id_out = nof_mep_db_short_entries * index;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
