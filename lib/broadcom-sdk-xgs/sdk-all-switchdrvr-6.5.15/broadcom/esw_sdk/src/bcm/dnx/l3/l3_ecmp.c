/** \file l3_ecmp.c
 *
 * Handles the control over the ECMPs entries.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

/*
 * Include files currently used for DNX.
 * {
 */

#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/access/ecmp_access.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/swstate/auto_generated/access/algo_l3_access.h>

#include <soc/dnx/dnx_err_recovery_manager.h>
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/l3.h>
#include <bcm/types.h>
#include <bcm_int/dnx/stat/stat_pp.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * The maximal size of ECMP members that can be consider a small ECMP group.
 */
#define L3_ECMP_MAX_ECMP_SMALL_GROUP_SIZE               16
/*
 * In case an ECMP group doesn't contains all the members the ECMP ID will identify the unique consistent member table
 * that was allocated for it. In case the group uses all the members (group size = max_paths) the group will use this
 * identifier that will indicate that this ECMP group uses a general consistent table for ECMP groups with " group size"
 * members.
 */
#define L3_ECMP_FULL_MEMBERS_IN_GROUP                   0xFFFFFFFF
/*
 * The L3 supported flags of the ECMP.
 */
#define L3_ECMP_SUPPORTED_BCM_L3_FLAGS                 (BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_2ND_HIERARCHY | BCM_L3_3RD_HIERARCHY)

/*
 * The BCM_L3_ECMP_* supported flags
 */
#define L3_ECMP_SUPPORTED_BCM_L3_ECMP_FLAGS            (BCM_L3_ECMP_LARGE_TABLE | BCM_L3_ECMP_EXTENDED)
/*
 * Row size in the members table
 */
#define L3_ECMP_MEMBER_TABLE_ROW_WIDTH                 32
/*
 * Row size in the members table
 */
#define L3_ECMP_MEMBER_TABLE_ADDRESS_DEFAULT_VAL       0

/*
 * The profile ID is used as a key to the consistent hashing manager.
 * As the profile ID isn't unique (there is one per hierarchy) the hierarchy information is part of the key.
 * Below are defines that will provide the information how to pack the hierarchy and the profile ID in a uint32 variable.
 */
#define L3_ECMP_PROFILE_ID_FIELD_SIZE                  10
#define L3_ECMP_PROFILE_ID_FIELD_MASK                  UTILEX_BITS_MASK(L3_ECMP_PROFILE_ID_FIELD_SIZE - 1, 0)
#define L3_ECMP_PROFILE_ID_FIELD_OFFSET                0

#define L3_ECMP_HIERARCHY_FIELD_SIZE                   utilex_log2_round_up(DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES)
#define L3_ECMP_HIERARCHY_FIELD_MASK                   UTILEX_BITS_MASK((L3_ECMP_HIERARCHY_FIELD_SIZE - 1), 0)
#define L3_ECMP_HIERARCHY_FIELD_OFFSET                 L3_ECMP_PROFILE_ID_FIELD_SIZE

/*
 * In case the consistent ECMP group isn't full (NOF members < group max size) a unique ID should be used to allocate an ECMP
 * profile just for that group.
 * In case of an add/remove action on the ECMP group we would like to first allocate a new profile to the group before releasing the
 * old one (to keep traffic going) so the current NOF members would also added to the ECMP unique key.
 */
#define L3_ECMP_ID_FIELD_SIZE                          16
#define L3_ECMP_ID_FIELD_MASK                          UTILEX_BITS_MASK(L3_ECMP_ID_FIELD_SIZE,0)
#define L3_ECMP_ID_FIELD_OFFSET                        0

#define L3_ECMP_NOF_MEMBERS_FIELD_SIZE                 utilex_log2_round_up(L3_ECMP_MAX_NOF_ECMP_MEMBERS)
#define L3_ECMP_NOF_MEMBERS_FIELD_MASK                 UTILEX_BITS_MASK(L3_ECMP_NOF_MEMBERS_FIELD_SIZE,0)
#define L3_ECMP_NOF_MEMBERS_FIELD_OFFSET               L3_ECMP_ID_FIELD_SIZE

/** Stat id is 16 bits long */
#define L3_ECMP_MAX_STAT_ID_VALUE 0xFFFF

/*
 * MACROs
 * {
 */

/**
 * \remark
 *      This macro returns the ECMP hierarchy level based on the
 *      ECMP BCM_L3_* flags
 *      Valid BCM_L3_* flags are:
 *          * BCM_L3_2ND_HIERARCHY - if flag is set, then it's
 *              2nd hierarchy level
 *          * BCM_L3_3RD_HIERARCHY - if flag is set, then it's
 *              3rd hierarchy level
 *      If neither of the two flags is added, then it is 1st hierarchy level.
 */
#define L3_ECMP_GET_HIERARCHY(flags) (_SHR_IS_FLAG_SET(flags, BCM_L3_2ND_HIERARCHY) ? \
                                     DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2 : \
                                     (_SHR_IS_FLAG_SET(flags, BCM_L3_3RD_HIERARCHY) ? DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3 : \
                                     DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1))
/*
 * Return the consistent table type to use
 * use_large_table - indication if to use the large table or not
 * max_path - if the use_large_table if off then max_path > 16 is medium table and otherwise small.
 */
#define L3_ECMP_GET_CONSISTENT_TABLE_TYPE(use_large_table, max_path) (use_large_table ? L3_ECMP_CONSISTENT_TABLE_LARGE : (max_path  > L3_ECMP_MAX_ECMP_SMALL_GROUP_SIZE ? L3_ECMP_CONSISTENT_TABLE_MEDIUM : L3_ECMP_CONSISTENT_TABLE_SMALL))
/*
 *Verifies that the consistent table type is valid.
 * type - the consistent hashing type table
 */
#define L3_ECMP_CONSISTENT_TYPE_VERIFY(type) SHR_IF_ERR_EXIT(((type < L3_ECMP_CONSISTENT_TABLE_SMALL) || (type > L3_ECMP_CONSISTENT_TABLE_LARGE)) ? _SHR_E_INTERNAL : _SHR_E_NONE)
/*
 * translate the consistent hashing manager profile offset into an address in the  members table by multiplying is by a single resource element (smallest table) NOF rows.
 * chm_offset - Given (resource) offset by the chm.
 */
#define L3_ECMP_GET_MEMBERS_TBL_ADDR_FROM_CHM_OFFSET(chm_offset) (chm_offset * ecmp_consistent_types_attr[L3_ECMP_CONSISTENT_TABLE_SMALL].nof_rows_in_members_table)
/*
 * while the small and medium table uses all of their entire space to store members, the large table uses only the largest multiplication of the
 * group max size that is smaller or equal to the large table size(e.g if there are 80 entries and large table size is 512 then only 480 entries
 * will be used).
 * consistent_type - the consistent hashing type table
 * group_size - the consistent ECMP group max size
 */
#define L3_ECMP_GET_MEMBERS_TABLE_USED_ROWS(consistent_type, group_size) (consistent_type == L3_ECMP_CONSISTENT_TABLE_LARGE) ? UTILEX_DIV_ROUND_DOWN(ecmp_consistent_types_attr[consistent_type].nof_entries,group_size)*group_size : ecmp_consistent_types_attr[consistent_type].nof_entries;
/*
 * the consistent hashing manager unique key is composed from the profile id and the hierarchy.
 * This MACRO creates it
 */
#define L3_ECMP_CHM_KEY_GET(profile_id, hierarchy) (((profile_id & L3_ECMP_PROFILE_ID_FIELD_MASK) << L3_ECMP_PROFILE_ID_FIELD_OFFSET) | ((hierarchy & L3_ECMP_HIERARCHY_FIELD_MASK) << L3_ECMP_HIERARCHY_FIELD_OFFSET) )
/*
 * the consistent hashing manager unique key is composed from the profile id and the hierarchy.
 * This MACRO extract the hierarchy from the key
 */
#define L3_ECMP_HIERARCHY_FROM_CHM_KEY_GET(chm_key) ((chm_key >> L3_ECMP_HIERARCHY_FIELD_OFFSET) & L3_ECMP_HIERARCHY_FIELD_MASK)
/*
 * the consistent hashing manager unique key is composed from the profile id and the hierarchy.
 * This MACRO extract the profile from the key
 */
#define L3_ECMP_PROFILE_ID_FROM_CHM_KEY_GET(chm_key) ((chm_key >> L3_ECMP_PROFILE_ID_FIELD_OFFSET) & L3_ECMP_PROFILE_ID_FIELD_MASK)
/*
 * return a unique key to allocate a unique profile for a consistent ECMP group which isn't full (NOF members < group max size).
 *
 * In case the consistent ECMP group isn't full (NOF members < group max size) a unique ID should be used to allocate an ECMP
 * profile just for that group.
 * In case of an add/remove action on the ECMP group we would like to first allocate a new profile to the group before releasing the
 * old one (to keep traffic going) so the current NOF members would also added to the ECMP unique key.
 */
#define L3_ECMP_PROFILE_UNIQUE_KEY(ecmp_id, intf_count) (((ecmp_id & L3_ECMP_ID_FIELD_MASK) << L3_ECMP_ID_FIELD_OFFSET) | ((intf_count & L3_ECMP_NOF_MEMBERS_FIELD_MASK) << L3_ECMP_NOF_MEMBERS_FIELD_OFFSET) )
/*
 * Determine whether the ECMP index belongs to the extended range of ECMP IDs.
 */
#define L3_ECMP_INDEX_IS_IN_EXTENDED_RANGE(unit, ecmp_index) (ecmp_index > dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit))
/*
 * Return the hierarchy flags with which the ECMP group was created based on its hierarchy
 */
#define L3_EGRESS_ECMP_GET_HIER_FLAGS(hierarchy) (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1 ? 0 : (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2 ? BCM_L3_2ND_HIERARCHY : BCM_L3_3RD_HIERARCHY))

/*
 * }
 */
/*
 * Enumeration.
 * {
 */

/*
 * }
 */
/*
 * }
 */
/*
 * Structures
 * {
 */
/*
 * Holds the ECMP attributes
 */
typedef struct
{
    /*
     * The ECMP group base FEC.
     */
    uint32 fec_base;
    /*
     * The index of the ECMP profile in the ECMP profile table
     */
    uint32 profile_index;
    /*
     * A bit that is set to 1 in case the ECMP RPF more is EM.
     */
    uint8 is_rpf_mode_em;
} dnx_l3_ecmp_attributes_t;
/*
 * Globals.
 * {
 */
const char *ecmp_profiles_resources[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] =
    { DNX_ALGO_L3_ECMP_PROFILE_HIER_1_TABLE_RESOURCE, DNX_ALGO_L3_ECMP_PROFILE_HIER_2_TABLE_RESOURCE,
    DNX_ALGO_L3_ECMP_PROFILE_HIER_3_TABLE_RESOURCE
};

#if (((L3_ECMP_MEMBER_TABLE_ROW_WIDTH%L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_SMALL) != 0) || ((L3_ECMP_MEMBER_TABLE_ROW_WIDTH%L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_MEDIUM) != 0) || ((L3_ECMP_MEMBER_TABLE_ROW_WIDTH%L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_LARGE) != 0))
#error "The members table row must be a multiplication of the consistent types member sizes."
#endif
#if (((L3_ECMP_CONSISTENT_TABLE_SMALL_NOF_ENTRIES % (L3_ECMP_MEMBER_TABLE_ROW_WIDTH/L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_SMALL)) != 0) || ((L3_ECMP_CONSISTENT_TABLE_MEDIUM_NOF_ENTRIES % (L3_ECMP_MEMBER_TABLE_ROW_WIDTH/L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_MEDIUM))!= 0) || ((L3_ECMP_CONSISTENT_TABLE_LARGE_NOF_ENTRIES % (L3_ECMP_MEMBER_TABLE_ROW_WIDTH/L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_LARGE))!= 0))
#error "The NOF members in a member table row must divide the total number of members."
#endif
/*
 * Holds the consistent table types attributes (see dnx_l3_ecmp_consistent_type_t).
 */
const dnx_l3_ecmp_consistent_type_t ecmp_consistent_types_attr[L3_ECMP_CONSISTENT_TABLE_NOF_TYPES] = {
    {
     L3_ECMP_4_BITS_MEMBER, L3_ECMP_CONSISTENT_TABLE_SMALL_NOF_UNIQUE_MEMBERS,
     L3_ECMP_CONSISTENT_TABLE_SMALL_NOF_ENTRIES, L3_ECMP_CONSISTENT_SMALL_RESOURCE,
     (L3_ECMP_MEMBER_TABLE_ROW_WIDTH / L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_SMALL),
     L3_ECMP_CONSISTENT_TABLE_SMALL_NOF_ENTRIES / (L3_ECMP_MEMBER_TABLE_ROW_WIDTH /
                                                   L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_SMALL)},
    {
     L3_ECMP_8_BITS_MEMBER, L3_ECMP_CONSISTENT_TABLE_MEDIUM_NOF_UNIQUE_MEMBERS,
     L3_ECMP_CONSISTENT_TABLE_MEDIUM_NOF_ENTRIES, L3_ECMP_CONSISTENT_MEDIUM_RESOURCE,
     (L3_ECMP_MEMBER_TABLE_ROW_WIDTH / L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_MEDIUM),
     L3_ECMP_CONSISTENT_TABLE_MEDIUM_NOF_ENTRIES / (L3_ECMP_MEMBER_TABLE_ROW_WIDTH /
                                                    L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_MEDIUM)},
    {
     L3_ECMP_8_BITS_MEMBER, L3_ECMP_CONSISTENT_TABLE_LARGE_NOF_UNIQUE_MEMBERS,
     L3_ECMP_CONSISTENT_TABLE_LARGE_NOF_ENTRIES, L3_ECMP_CONSISTENT_LARGE_RESOURCE,
     (L3_ECMP_MEMBER_TABLE_ROW_WIDTH / L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_LARGE),
     L3_ECMP_CONSISTENT_TABLE_LARGE_NOF_ENTRIES / (L3_ECMP_MEMBER_TABLE_ROW_WIDTH /
                                                   L3_ECMP_CONSISTENT_MEMBER_SIZE_TABLE_LARGE)}
};

/*
 * }
 */
/*
 * Functions
 * {
 */

/*
 * For more  details see .h file
 */
shr_error_e
dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get(
    int unit,
    dbal_enum_value_field_ecmp_mode_e ecmp_mode,
    l3_ecmp_consistent_type_e * consistent_table_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (ecmp_mode)
    {
        case DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_SMALL_TABLE:
        {
            *consistent_table_type = L3_ECMP_CONSISTENT_TABLE_SMALL;
            break;
        }
        case DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_MEDIUM_TABLE:
        {
            *consistent_table_type = L3_ECMP_CONSISTENT_TABLE_MEDIUM;
            break;
        }
        case DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_LARGE_TABLE:
        {
            *consistent_table_type = L3_ECMP_CONSISTENT_TABLE_LARGE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ECMP mode %d is either not consistent or invalid.", ecmp_mode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function verifies that the ECMP group is allocated.
 *   It if isn't, an error is returned.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_group_id - the ID of the ECMP group
 * \return
 *   \retval Zero if no error was detected and the group is allocated
 *   \retval _SHR_E_NOT_FOUND if the group is not allocated
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_get
 *  * bcm_dnx_l3_egress_ecmp_destroy
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 */
static shr_error_e
dnx_l3_ecmp_group_verify(
    int unit,
    int ecmp_group_id)
{
    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the ecmp ID value is valid.
     */
    if ((ecmp_group_id >= dnx_data_l3.ecmp.nof_ecmp_get(unit)
         || ecmp_group_id < dnx_data_l3.ecmp.first_valid_ecmp_group_id_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ECMP %d is out of range, ECMP id must be below %d and above 0.", ecmp_group_id,
                     dnx_data_l3.ecmp.nof_ecmp_get(unit));
    }
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_is_allocated(unit, ecmp_group_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "ECMP group with ID = %d is not allocated\n", ecmp_group_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
dnx_l3_ecmp_consistent_members_table_info_get(
    int unit,
    l3_ecmp_consistent_type_e consistent_table_type,
    dnx_l3_ecmp_consistent_type_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);

    L3_ECMP_CONSISTENT_TYPE_VERIFY(consistent_table_type);

    sal_memcpy(table_info, &ecmp_consistent_types_attr[consistent_table_type], sizeof(dnx_l3_ecmp_consistent_type_t));

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
dnx_l3_ecmp_hierarchy_profile_resource_get(
    int unit,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    char **profile_resouce)
{
    SHR_FUNC_INIT_VARS(unit);

    if (hierarchy >= DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1
        || hierarchy < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit))
    {
        *profile_resouce = (char *) ecmp_profiles_resources[hierarchy];
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, ".Unknown hierarchy type %d.", hierarchy);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function find an ECMP group profile based on the ECMP ID.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_intf -
 *   The ECMP group ID.
 * \param [out] ecmp_atr -
 *   The ECMP group attributes
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_get
 *  * bcm_dnx_l3_egress_ecmp_destroy
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 */
static shr_error_e
dnx_l3_ecmp_group_entry_result_get(
    int unit,
    bcm_if_t ecmp_intf,
    dnx_l3_ecmp_attributes_t * ecmp_atr)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(ecmp_atr, 0, sizeof(dnx_l3_ecmp_attributes_t));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TABLE, &entry_handle_id));
    /** Keys */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_ID, ecmp_intf);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** receive the group profile ID value and the FEC base ptr*/
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE,
                                                        INST_SINGLE, &(ecmp_atr->profile_index)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_FEC_POINTER_BASE,
                                                        INST_SINGLE, &(ecmp_atr->fec_base)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_RPF_MODE_IS_EM,
                                                       INST_SINGLE, &(ecmp_atr->is_rpf_mode_em)));

    /*
     * Set the FEC base with an interface prefix
     */
    BCM_L3_ITF_SET(ecmp_atr->fec_base, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_atr->fec_base));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function reads all the consistent hashing members offsets from the HW and returns a sorted array of the members offset
 * which contains a single instance of each member offset although it may appear more than once in the HW table.
 * \param [in] unit -
 *   The unit number.
 * \param [in] members_address -
 *   the base row in the members table to start read from.
 * \param [in] consistent_type -
 *   the consistent table type
 * \param [in] group_size -
 *   the ECMP max group size.
 * \param [in] protection -
 *   indicate the ECMP group is protected.
 * \param [in] fec_base -
 *   the ECMP group base FEC ID.
 * \param [in,out] members_array -
 *   An array that returns all the unique members that this ECMP group have.
 * \param [in,out] member_count -
 *   pointer to the return value of the NOF members in the ECMP group.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_egress_ecmp_get_unique_consistent_members(
    int unit,
    uint32 members_address,
    l3_ecmp_consistent_type_e consistent_type,
    uint32 group_size,
    uint32 protection,
    uint32 fec_base,
    bcm_if_t * members_array,
    int *member_count)
{

    uint32 entry_handle_id;
    uint32 row_indx, member_indx, member = 0, nof_members, members_pointer = 0;
    uint32 exists_members[UTILEX_DIV_ROUND_UP(L3_ECMP_MAX_NOF_ECMP_MEMBERS, SAL_UINT32_NOF_BITS)];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * A bit stream is used to mark each member offset that was found in the HW members table.
     */
    SHR_IF_ERR_EXIT(utilex_bitstream_clear
                    (exists_members, UTILEX_DIV_ROUND_UP(L3_ECMP_MAX_NOF_ECMP_MEMBERS, SAL_UINT32_NOF_BITS)));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FEC_ECMP_MEMBERS, &entry_handle_id));

    /*
     * Get the supported number of members in this table type
     */
    nof_members = L3_ECMP_GET_MEMBERS_TABLE_USED_ROWS(consistent_type, group_size);

    /*
     * The NOF relevant members in a table doesn't necessarily fills a row in the HW, so as long as the current NOF members offset that
     * was read is smaller than the expected one the NOF rows will increment.
     */
    for (row_indx = 0; members_pointer < nof_members; row_indx++)
    {

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_ROW_ID, members_address + row_indx);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        for (member_indx = 0;
             (member_indx < ecmp_consistent_types_attr[consistent_type].nof_members_in_table_row)
             && (members_pointer < nof_members); member_indx++, members_pointer++)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_VAL, member_indx, &member));

            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(exists_members, member));
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FEC_ECMP_MEMBERS, entry_handle_id));
    }

    *member_count = 0;

    for (member_indx = 0; member_indx < L3_ECMP_MAX_NOF_ECMP_MEMBERS; member_indx++)
    {
        /*
         * For each member offset that was found add the FEC base value and multiply the offset by two if protected.
         */
        if (utilex_bitstream_test_bit(exists_members, member_indx))
        {
            members_array[(*member_count)++] = fec_base + (member_indx << protection);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information
 */
shr_error_e
dnx_l3_egress_ecmp_get_ecmp_group_info(
    int unit,
    bcm_if_t ecmp_index,
    int *member_count,
    bcm_if_t * intf_array,
    dnx_l3_ecmp_profile_t * ecmp_profile)
{
    uint8 group_size_minus_one = 0;
    uint32 address = 0, member_iter;
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy = 0;
    l3_ecmp_consistent_type_e consistent_table_type;
    dnx_l3_ecmp_attributes_t ecmp_atr;
    uint32 stat_pp_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(ecmp_profile, 0, sizeof(*ecmp_profile));

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_group_entry_result_get(unit, ecmp_index, &ecmp_atr));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));

    /*
     * get the hierarchy of the ECMP group using its ID.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, (uint32 *) &ecmp_hierarchy));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, ecmp_atr.profile_index);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, ecmp_hierarchy);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_MODE,
                                                        INST_SINGLE, &(ecmp_profile->ecmp_mode)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE,
                                                        INST_SINGLE, &address));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_MAX_GROUP_SIZE_MINUS_ONE,
                                                       INST_SINGLE, &group_size_minus_one));

    /*
     * Get the statistic data if the profile is valid
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));
    if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID,
                                                            INST_SINGLE, &(ecmp_profile->statistic_object_id)));

        STAT_PP_ENGINE_PROFILE_SET(ecmp_profile->statistic_object_profile, stat_pp_profile,
                                   bcmStatCounterInterfaceIngressReceivePp);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_PROTECTION_ENABLE,
                                                       INST_SINGLE, &(ecmp_profile->protection_flag)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_RPF_MODE,
                                                        INST_SINGLE, &(ecmp_profile->rpf_mode)));

    /** The HW provide the group size minus one so we need to increment*/
    ecmp_profile->group_size = group_size_minus_one + 1;

    /*
     * Update the interfaces array, in case of ecmp mode "multiply and divide" the interfaces are consecutive and can be retrieve using the
     * base FEC and the group size.
     * In the consistent case the HW member table should be used for the update.
     */
    if (ecmp_profile->ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE)
    {
        *member_count = ecmp_profile->group_size;
        for (member_iter = 0; member_iter < *member_count; member_iter++)
        {
            intf_array[member_iter] = ecmp_atr.fec_base + (member_iter << ecmp_profile->protection_flag);
        }

        ecmp_profile->members_table_id = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                        (unit, ecmp_profile->ecmp_mode, &consistent_table_type));
        /** the address in the profile is just relative to the table size so we need to multiply in the NOF rows*/
        address = address * ecmp_consistent_types_attr[consistent_table_type].nof_rows_in_members_table;

        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_get_unique_consistent_members
                        (unit, address, consistent_table_type, ecmp_profile->group_size,
                         ecmp_profile->protection_flag, ecmp_atr.fec_base, intf_array, member_count));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Receive information regarding a single ECMP group -
 *  mode, flags, number of members and list of members.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_index -
 *   The ECMP ID.
 * \param [out] ecmp - an ECMP structure that will return flags,
 *   max paths and mode.
 * \param [in] intf_size -
 *   Number of members that can be added to the intf_array
 * \param [out] intf_array -
 *   Array of the group members
 * \param [out] intf_count -
 *   number of actual members in the intf_array.
 */
static shr_error_e
dnx_l3_egress_ecmp_group_data_get(
    int unit,
    bcm_if_t ecmp_index,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    dnx_l3_ecmp_profile_t ecmp_profile;
    bcm_if_t members[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    int actual_members_count = 0;
    uint32 hierarchy;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the real hierarchy ID. */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, &hierarchy));
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_get_ecmp_group_info
                    (unit, ecmp_index, &actual_members_count, members, &ecmp_profile));
    /*
     * If the size allocated in the array is insufficient for all members in the group, then an error will be returned.
     */
    if (actual_members_count > intf_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The provided array of size %d can't hold all the %d members that were found for ECMP group %d.\n",
                     intf_size, actual_members_count, ecmp_index);
    }

    *intf_count = actual_members_count;

    sal_memcpy(intf_array, members, actual_members_count * sizeof(bcm_if_t));

    ecmp->max_paths = ecmp_profile.group_size;

    ecmp->ecmp_group_flags =
        (ecmp_profile.ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_LARGE_TABLE) ? BCM_L3_ECMP_LARGE_TABLE : 0;
    /*
     * Set the rpf mode if any is defined.
     */
    if (ecmp_profile.rpf_mode == DBAL_ENUM_FVAL_ECMP_RPF_MODE_LOOSE)
    {
        ecmp->rpf_mode = bcmL3EcmpUrpfLoose;
    }
    else if (ecmp_profile.rpf_mode == DBAL_ENUM_FVAL_ECMP_RPF_MODE_STRICT_EM)
    {
        ecmp->rpf_mode = bcmL3EcmpUrpfStrictEm;
    }
    else
    {
        ecmp->rpf_mode = bcmL3EcmpUrpfInterfaceDefault;
    }

    /*
     * Define the dynamic mode based on the mode of the ECMP profile.
     * If it is Multiply and Divide, then the dynamic mode is disabled.
     * Otherwise, it is Resilient as only these two modes are supported currently.
     */
    ecmp->dynamic_mode =
        (ecmp_profile.ecmp_mode ==
         DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE) ? BCM_L3_ECMP_DYNAMIC_MODE_DISABLED :
        BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;

    /** Update statistics info */
    ecmp->stat_id = ecmp_profile.statistic_object_id;
    ecmp->stat_pp_profile = ecmp_profile.statistic_object_profile;
    ecmp->flags |= L3_EGRESS_ECMP_GET_HIER_FLAGS(hierarchy);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Fill all the ECMP profile fields
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   The ECMP group information.
 * \param [in] intf -
 *   A single interface from the ECMP group that can provide protection information.
 * \param [in] intf_count -
 *  Actual NOF interfaces that were added to the group.
 * \param [in] force_unique_profile -
 *  Force a unique profile, in case we want to have two identical profiles that have the same attributes but different profiles ID (used
 *  for creating a profile that will be update later for add/remove member peruses).
 * \param [in,out] ecmp_profile -
 *   Pointer the ECMP profile number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_fill_profile(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    bcm_if_t intf,
    int intf_count,
    uint8 force_unique_profile,
    dnx_l3_ecmp_profile_t * ecmp_profile)
{
    uint32 ecmp_group_max_size;

    SHR_FUNC_INIT_VARS(unit);

    ecmp_group_max_size = UTILEX_MAX(intf_count, ecmp->max_paths);

    sal_memset(ecmp_profile, 0, sizeof(*ecmp_profile));

    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED)
    {
        ecmp_profile->ecmp_mode = DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE;
    }
    else if (_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_LARGE_TABLE))
    {
        ecmp_profile->ecmp_mode = DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_LARGE_TABLE;
    }
    else if (ecmp_group_max_size <= L3_ECMP_MAX_ECMP_SMALL_GROUP_SIZE)
    {
        ecmp_profile->ecmp_mode = DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_SMALL_TABLE;
    }
    else
    {
        ecmp_profile->ecmp_mode = DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_MEDIUM_TABLE;
    }

    ecmp_profile->group_size = ecmp_group_max_size;

    /*
     * As we don't have the members table address at this stage the assumption is that:
     * 1. non consistent ECMP groups don't care about the members table hence: members_table_id = L3_ECMP_FULL_MEMBERS_IN_GROUP
     * 2. consistent ECMP groups that have all the member in group share the same members table so they get the same
     * value of L3_ECMP_FULL_MEMBERS_IN_GROUP and the "group size" attribute will separate two ECMP with different full group sizes.
     * 3.consistent ECMP group that are not full have a unique members table and there ECMP ID will be used.
     * 4.The force_unique_profile will be set when there is a full consistent table but a unique ID is require, this is for a full group
     * that one of the members is about to be subtracted. the CHM should gives this ECMP group the same table at first and when a member is
     * removed the CHM will know how to remove the member in a consistent why using the current full members table.
     */
    ecmp_profile->members_table_id = (((ecmp_profile->ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE)
                                       || (intf_count ==
                                           ecmp_group_max_size))
                                      && (!force_unique_profile)) ? L3_ECMP_FULL_MEMBERS_IN_GROUP :
        L3_ECMP_PROFILE_UNIQUE_KEY(BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), intf_count);

    /*
     * Update protection state
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, intf, &ecmp_profile->protection_flag));

    /*
     * Update the RPF state
     */
    if (ecmp->rpf_mode == bcmL3EcmpUrpfLoose)
    {
        ecmp_profile->rpf_mode = DBAL_ENUM_FVAL_ECMP_RPF_MODE_LOOSE;
    }
    else if (ecmp->rpf_mode == bcmL3EcmpUrpfStrictEm)
    {
        ecmp_profile->rpf_mode = DBAL_ENUM_FVAL_ECMP_RPF_MODE_STRICT_EM;
    }
    else
    {
        ecmp_profile->rpf_mode = DBAL_ENUM_FVAL_ECMP_RPF_MODE_RESERVED;
    }

    if (ecmp->stat_pp_profile != STAT_PP_PROFILE_INVALID)
    {
        ecmp_profile->statistic_object_id = ecmp->stat_id;
        ecmp_profile->statistic_object_profile = ecmp->stat_pp_profile;
    }

    

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Allocates a table in the members table memory using the consistent hashing manger.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   ecmp group info.
 * \param [in] fec_base -
 *   FEC base address.
 * \param [in] ecmp_profile_index -
 *   ECMP profile index.
 * \param [in] hierarchy -
 *   the ECMP hierarchy level.
 * \param [in] intf_count -
 *   NOF members in ECMP group.
 * \param [in] intf_array -
 *   ECMP members.
 */
static shr_error_e
dnx_l3_consistent_hashing_allocation(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    uint32 fec_base,
    int ecmp_profile_index,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    int intf_count,
    bcm_if_t * intf_array)
{
    uint32 members_offset_from_base[L3_ECMP_CONSISTENT_TABLE_MAX_NOF_ENTRIES];
    uint32 member_iter;
    uint32 consistent_hashing_manager_handle;
    uint32 max_path;
    uint8 is_protected;
    l3_ecmp_consistent_type_e consistent_table_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    max_path = UTILEX_MAX(intf_count, ecmp->max_paths);

    BCM_L3_ITF_SET(fec_base, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(fec_base));

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, intf_array[0], &is_protected));

    consistent_table_type =
        L3_ECMP_GET_CONSISTENT_TABLE_TYPE(_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_LARGE_TABLE), max_path);
    /*
     * Create an array of offset from the base FEC to be used as an input for the CHM.
     */
    for (member_iter = 0; member_iter < intf_count; member_iter++)
    {
        members_offset_from_base[member_iter] = (intf_array[member_iter] - fec_base) >> is_protected;
    }
    SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));

    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc(unit, consistent_hashing_manager_handle,
                                                                      L3_ECMP_CHM_KEY_GET(ecmp_profile_index,
                                                                                          hierarchy),
                                                                      consistent_table_type, max_path, intf_count,
                                                                      members_offset_from_base, NULL, NULL));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Allocate an ECMP profile according to the ECMP parameters and members type and count
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   ecmp group user information.
 * \param [in] intf_array -
 *   The ECMP group members.
 * \param [in] intf_count -
 *  NOF members in the ECMP group
 * \param [in] fec_base -
 *  the base FEC of the ECMP group
 * \param [in] force_unique_profile -
 *  allocate a unique profile for this ECMP group.
 * \param [out] profile_index -
 *  the allocated profile ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 */
static shr_error_e
dnx_l3_egress_ecmp_profile_allocation(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    bcm_if_t * intf_array,
    int intf_count,
    uint32 fec_base,
    uint8 force_unique_profile,
    int *profile_index)
{
    uint8 first_reference;
    dnx_l3_ecmp_profile_t ecmp_profile;
    dbal_enum_value_field_hierarchy_level_e hierarchy;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_fill_profile
                    (unit, ecmp, intf_array[0], intf_count, force_unique_profile, &ecmp_profile));

    hierarchy = L3_ECMP_GET_HIERARCHY(ecmp->flags);

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.allocate_single
                    (unit, hierarchy, _SHR_CORE_ALL, 0, &ecmp_profile, NULL, profile_index, &first_reference));

    /*
     * If this is the first time this profile is allocated write it into the HW profile table.
     */
    if (first_reference)
    {
        uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));
        /*
         * Keys
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, (uint32) *profile_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, (uint32) hierarchy);
        /*
         * Values
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MODE, INST_SINGLE, ecmp_profile.ecmp_mode);
        /** Set the ECMP members base to 0, in case of consistent hashing this field will be update by the consistent hashing manager */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE, INST_SINGLE,
                                     L3_ECMP_MEMBER_TABLE_ADDRESS_DEFAULT_VAL);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_GROUP_SIZE_MINUS_ONE, INST_SINGLE,
                                     ecmp_profile.group_size - 1);
        /*
         * Write statistic data
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, ecmp->stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(ecmp->stat_pp_profile));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_ENABLE, INST_SINGLE,
                                     ecmp_profile.protection_flag);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_RPF_MODE, INST_SINGLE,
                                     ecmp_profile.rpf_mode);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * If this is consistent mode then allocate an address in the members table and update the profile
         */
        if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
        {
            SHR_IF_ERR_EXIT(dnx_l3_consistent_hashing_allocation
                            (unit, ecmp, fec_base, *profile_index, hierarchy, intf_count, intf_array));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function removes an ECMP group and frees the space it
 *  has occupied both HW and SW.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_profile_index -
 *   The ID of the ECMP group profile
 * \param [in] ecmp_hierarchy -
 *   The ECMP hierarchy.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_destroy
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 */
static shr_error_e
dnx_l3_egress_ecmp_profile_free(
    int unit,
    uint32 ecmp_profile_index,
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy)
{
    uint32 entry_handle_id;
    uint32 member_base_address;
    uint32 chm_offset;
    uint8 last_reference = 0;
    int ref_count = 0;
    dnx_l3_ecmp_profile_t ecmp_profile;
    sw_state_ll_node_t node;
    sw_state_ll_node_t *node_ptr;
    uint32 profile_key, profile_key_expected;
    dbal_enum_value_field_ecmp_mode_e ecmp_mode;
    l3_ecmp_consistent_type_e consistent_table_type;
    uint32 consistent_hashing_manager_handle;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.profile_data_get
                    (unit, ecmp_hierarchy, _SHR_CORE_ALL, ecmp_profile_index, &ref_count, &ecmp_profile));
    /*
     * If the reference counter indicates 1, then later we will receive last reference indication.
     * DBAL entry clear needs to happen before template free.
     */
    if (ref_count == 1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));

        /*
         * Receive the ECMP group profile index and use it to retrieve the ECMP group hashing type.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, ecmp_profile_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, ecmp_hierarchy);

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MODE, INST_SINGLE, &ecmp_mode);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE, INST_SINGLE,
                                   &member_base_address);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * In case this a consistent mode remove this profile from the members address linked list and
         * release the profile from consistent hashing manager reference
         */
        if ((ecmp_mode != DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE))
        {

            /*
             * If this was the last profile, remove it from the link list of the matching resource of the address
             */
            SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                            (unit, ecmp_mode, &consistent_table_type));

            chm_offset = member_base_address * ecmp_consistent_types_attr[consistent_table_type].nof_resources;

            node_ptr = &node;

            profile_key_expected = L3_ECMP_CHM_KEY_GET(ecmp_profile_index, ecmp_hierarchy);

            SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.get_first(unit, chm_offset, node_ptr));

            /*
             * Iterate over the linked list to find the profile to remove.
             */
            while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
            {
                SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.node_value(unit, *node_ptr,
                                                                                            &profile_key));

                if (profile_key == profile_key_expected)
                {
                    SHR_IF_ERR_EXIT(ecmp_db_info.
                                    members_tbl_addr_profile_link_lists.remove_node(unit, chm_offset, *node_ptr));
                    break;
                }

                SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.next_node(unit, chm_offset, *node_ptr,
                                                                                           node_ptr));

            }

            if (!DNX_SW_STATE_LL_IS_NODE_VALID(node))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Released profile %d (hierarchy %d) wasn't found in the members addresses %d linked list,",
                             ecmp_profile_index, ecmp_hierarchy, chm_offset);
            }

            /*
             * free the consistent hashing manager from this profile
             */
            SHR_IF_ERR_EXIT(ecmp_db_info.
                            consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));

            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free
                            (unit, consistent_hashing_manager_handle,
                             L3_ECMP_CHM_KEY_GET(ecmp_profile_index, ecmp_hierarchy)));
        }

        /*
         * If this is the last reference of this profile, remove it from the HW profile table.
         * This HW access is not mandatory but will prevent the DBAL from showing unused profiles.
         */

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));
        /** Keys */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, (uint32) ecmp_profile_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, (uint32) ecmp_hierarchy);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * release the profile reference from the template manager
     */
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.free_single
                    (unit, ecmp_hierarchy, _SHR_CORE_ALL, ecmp_profile_index, &last_reference));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create or update an ECMP group entry based on the ECMP ID.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_intf -
 *   The ECMP group id.
 * \param [in] ecmp_profile_id -
 *   The ECMP profile (composed of all the ECMP attributes as
 *    defined in the HW profile definition).
 * \param [in] fec_base -
 *   FEC base pointer - first member in ECMP group
 * \param [in] rpf_mode_is_em -
 *   In case the RPF mode is EM this bit should be set to one, else it should remain 0.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_entry_set(
    int unit,
    bcm_if_t ecmp_intf,
    uint32 ecmp_profile_id,
    uint32 fec_base,
    uint32 rpf_mode_is_em)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TABLE, &entry_handle_id));
    /*
     * Keys
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_ID, ecmp_intf);
    /*
     * Values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, INST_SINGLE, ecmp_profile_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_POINTER_BASE, INST_SINGLE,
                                 BCM_L3_ITF_VAL_GET(fec_base));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RPF_MODE_IS_EM, INST_SINGLE,
                                 rpf_mode_is_em != 0 ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Allocate an ECMP group entry using the resource manager.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP group id.
 * \param [in] ecmp_flags - Control flags for the ECMP group
 *        * BCM_L3_WITH_ID - the group will be allocated with the given ID
 *        * BCM_L3_2ND_HIERARCHY - the group belongs to the 2nd hierarchy
 *        * BCM_L3_3RD_HIERARCHY - the group belongs to the 3rd hierarchy
 * \param [in] ecmp_group_flags - Additional control flags for the ECMP group.
 *        * BCM_L3_ECMP_EXTENDED - indicate that the group will be in the extended range.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_entry_alloc(
    int unit,
    bcm_if_t * ecmp_index,
    uint32 ecmp_flags,
    uint32 ecmp_group_flags)
{
    uint32 hierarchy = 0;
    int is_first;
    int bank_id;
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    hierarchy = L3_ECMP_GET_HIERARCHY(ecmp_flags);

    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_allocate(unit, ecmp_index, ecmp_flags, ecmp_group_flags, hierarchy));
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_group_is_alone_in_bank(unit, *ecmp_index, &bank_id, &is_first));
    /*
     * If the function dnx_algo_l3_ecmp_group_is_alone_in_bank returns indication that the group we are currently
     * allocating is the first one in the bank of the stage map (is_first = 1), then we need to update the DBAL table
     * responsible for saving the ECMP bank to stage mapping.
     */
    if (is_first == 1)
    {
        /** Update the DBAL table with the hierarchy value for the given bank_id (instance) */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_BANK_TO_STAGE_MAP, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, bank_id, hierarchy);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Remove an ECMP group entry from the DBAL table
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP group id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_entry_remove(
    int unit,
    uint32 ecmp_index)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Delete the DBAL entry from the ECMP table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_ID, ecmp_index);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Deallocate an ECMP group entry using the resource manager.
 *   Update the bank to hierarchy mapping if needed.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP group id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_entry_dealloc(
    int unit,
    bcm_if_t ecmp_index)
{
    uint32 init_hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
    int is_last;
    int bank_id;
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_group_is_alone_in_bank(unit, ecmp_index, &bank_id, &is_last));
    /*
     * If the function dnx_algo_l3_ecmp_group_is_alone_in_bank returns indication that the group we are currently
     * deallocating is the last one in the bank of the hierarchy map (is_last = 1), then we need to update the DBAL table
     * responsible for saving the ECMP bank to hierarchy mapping.
     */
    if (is_last == 1)
    {
        /** Update the DBAL table with the hierarchy value for the given bank_id (instance). */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_BANK_TO_STAGE_MAP, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, bank_id, init_hierarchy);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_deallocate(unit, ecmp_index));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifies the user input for the ECMP create function.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   The ECMP group information.
 * \param [in] intf_count -
 *   The ECMP current number of members in the group.
 * \param [in] intf_array -
 *  Array of the ECMP members.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
static shr_error_e
dnx_l3_egress_ecmp_create_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array)
{
    uint32 fec_index;
    uint32 min_fec;
    uint32 max_fec;
    uint32 hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    uint32 fec_hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    uint32 ecmp_hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    int fec_iter;
    int group_size;
    uint8 fec_duplicates[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    uint8 is_allocated = 0;
    uint8 is_protected = 0, fec_protection = 0;
    uint32 max_fec_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf_array, _SHR_E_PARAM, "bcm_if_t");
    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");

    /*
     * Verify the flags supplied to the ECMP structure.
     */
    if (_SHR_IS_FLAG_SET(ecmp->flags, ~L3_ECMP_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x).",
                     ecmp->flags, L3_ECMP_SUPPORTED_BCM_L3_FLAGS);
    }

    if (!_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID) && BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "WITH_ID flag is not provided, but user has specified an ECMP interface %d.\n",
                     BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
    }
    else if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
    {
        /*
         * Verify the ecmp ID value is valid.
         */
        if ((BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) >= dnx_data_l3.ecmp.nof_ecmp_get(unit)
             || BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) < dnx_data_l3.ecmp.first_valid_ecmp_group_id_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "ECMP %d is out of range, ECMP id must be below %d and above 0.",
                         BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), dnx_data_l3.ecmp.nof_ecmp_get(unit));
        }
    }

    /*
     * In case no ECMP ID was specified an unallocated ECMP ID will be taken, so there is no need to check the allocation state.
     */
    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_is_allocated(unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), &is_allocated));
    }

    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
    {
        /*
         * Using the replace flag requieres a the "with ID" flag (to indicate the replaced ECMP member).
         */
        if (!_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Using the BCM_L3_REPLACE flag is only valid when used with BCM_L3_WITH_ID.\n");

        }

        /*
         * To update ECMP group, it must already exist.
         */
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_L3_REPLACE flag was used but the ECMP group with ID = %d is not allocated.\n",
                         BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
        }

    }
    else if (is_allocated && !_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "ECMP group %d is already allocated please use the BCM_L3_REPLACE flag.\n",
                     BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
    }

    /*
     * Verify the ecmp ID value is valid.
     */
    if ((BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) >= dnx_data_l3.ecmp.nof_ecmp_get(unit)
         || BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) < dnx_data_l3.ecmp.first_valid_ecmp_group_id_get(unit))
        && _SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ECMP %d is out of range, ECMP id must be below %d and above 0.",
                     BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), dnx_data_l3.ecmp.nof_ecmp_get(unit));
    }

    if (_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, ~L3_ECMP_SUPPORTED_BCM_L3_ECMP_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 ECMP flags are used.");
    }

    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_2ND_HIERARCHY) && _SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_3RD_HIERARCHY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Two hierarchy levels were selected when a single hierarchy per ECMP is supported.");
    }

    if (_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_EXTENDED)
        && (!L3_ECMP_INDEX_IS_IN_EXTENDED_RANGE(unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf)))
        && _SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The ECMP group ID %d is not in the extended range but flag BCM_L3_ECMP_EXTENDED is provided.",
                     BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
    }

    if (!_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_EXTENDED)
        && (L3_ECMP_INDEX_IS_IN_EXTENDED_RANGE(unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The ECMP group ID %d is in the extended range but flag BCM_L3_ECMP_EXTENDED is not provided.",
                     BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
    }
    /*
     * Verify that the max_paths value is valid - between 0 and the ecmp maximum size of a group.
     */
    if ((ecmp->max_paths < 0) || (ecmp->max_paths > dnx_data_l3.ecmp.max_group_size_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The value %d isn't a valid value for max_paths, the maximal number of paths must be a positive number smaller than %d or 0 if unused.",
                     ecmp->max_paths, dnx_data_l3.ecmp.max_group_size_get(unit));
    }

    /*
     * Verify that the max_paths value is valid and that its value is bigger than the number of interfaces
     * passed to the function.
     */
    if (ecmp->max_paths != 0 && intf_count > ecmp->max_paths)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ECMP number of paths %d exceeds the defined maximal number of path %d.", intf_count,
                     ecmp->max_paths);
    }

    if (intf_count <= 0 || intf_count > dnx_data_l3.ecmp.max_group_size_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid number of interfaces value %d, should be positive and less than or equal to %d.",
                     intf_count, dnx_data_l3.ecmp.max_group_size_get(unit));
    }

    /*
     * Currently supported modes:
     *      * Multiply and divide
     *      * Consistent
     */
    if (ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_DISABLED
        && ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ECMP dynamic mode was selected.");
    }

    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED
        && (ecmp->max_paths != 0 && ecmp->max_paths != intf_count))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Incompatible max_paths %u and intf_count %d values in multiply and divide mode. They should be equal to one another.\n",
                     ecmp->max_paths, intf_count);
    }

    /*
     * Verify that that all the ECMP group members are
     *  - allocated
     *  - in the same hierarchy
     *  - in the same protection state
     */
    for (fec_iter = 0; fec_iter < intf_count; fec_iter++)
    {
        fec_index = BCM_L3_ITF_VAL_GET(intf_array[fec_iter]);
        /** Confirm the object existance with FEC allocation mngr */
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, fec_index, &is_allocated));
        if (is_allocated == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ECMP member FEC with ID %d has not been allocated\n", intf_array[fec_iter]);
        }
        /** Get hierarchy from fec_id */
        SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get(unit, fec_index, &hierarchy));
        /** Get the FEC protection state */
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, fec_index, &fec_protection));
        /*
         * If checking hierarchy of first element in array, copy the hierarchy value to fec_hierarchy
         * and continue to next iteration.
         */
        if (fec_iter == 0)
        {
            fec_hierarchy = hierarchy;
            is_protected = fec_protection;

        }
        /** If hierarchy of FEC member doesn't match previously found hierarhcies, return an error. */
        else if (fec_hierarchy != hierarchy)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Hierarchy value %d of FEC member %d doesn't match previously found value - %d\n",
                         hierarchy, intf_array[fec_iter], fec_hierarchy);
        }
        else if (fec_protection != is_protected)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Protection state %d of FEC member %d doesn't match other ECMP members protection state %d.\n",
                         fec_protection, intf_array[fec_iter], is_protected);
        }
        /** Make sure that if a FEC is protected the primary one is the one that was provided */
        if (fec_protection && (intf_array[fec_iter] % 2 == 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "In case protected FECs are used as ECMP group members, only the primary FECs (even numbered FECs, in this case %d) in the protection pair should be provided to the ECMP API and the not secondary FECs in the protection (odd numbered FECs, as provided in this case %d).",
                         intf_array[fec_iter] - 1, intf_array[fec_iter]);
        }

    }

    /*
     * As we don't assume sorted array of interface, we need to check that the lowest and highest FECs are in the range
     * of the group size and that the interfaces are not duplicated in the array.
     * In case of multiply and divide hashing where the NOF interfaces equal to the group size,
     * if the high and low FECs are in range and there are no duplication then all the FECs are consecutive.
     */
    group_size = UTILEX_MAX(ecmp->max_paths, intf_count);
    sal_memset(fec_duplicates, 0, sizeof(uint8) * L3_ECMP_MAX_NOF_ECMP_MEMBERS);
    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));
    /*
     * We need to check if all members are in range and that there are no duplications.
     */
    for (fec_iter = 0, max_fec = 0, min_fec = max_fec_id; fec_iter < intf_count; fec_iter++)
    {
        fec_index = BCM_L3_ITF_VAL_GET(intf_array[fec_iter]);

        if (fec_index > max_fec_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FEC index %d is out of range, max FEC index is %d.", fec_index, max_fec_id);
        }

        /** get min and get max */
        max_fec = UTILEX_MAX(fec_index, max_fec);
        min_fec = UTILEX_MIN(fec_index, min_fec);
    }
    /*
     * Verify that the min FEC and the max FEC are in range regarding the group size.
     */
    if (min_fec + (group_size << is_protected) <= max_fec)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Group size %d is incompatible with FEC min member %u and max member %u.",
                     group_size, min_fec, max_fec);
    }
    /*
     * Check for duplicates.
     * Map each member of the interface array to the fec_duplicates array based on its value.
     */
    for (fec_iter = 0; fec_iter < intf_count; fec_iter++)
    {
        fec_index = BCM_L3_ITF_VAL_GET(intf_array[fec_iter]);
        if (fec_duplicates[(fec_index - min_fec) >> is_protected] == 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FEC ID %d is duplicated in the interface array.", fec_index);
        }
        fec_duplicates[(fec_index - min_fec) >> is_protected] = 1;
    }

    /** Verify that the ECMP members have equal or higher hierarchy than the ECMP. */
    ecmp_hierarchy = L3_ECMP_GET_HIERARCHY(ecmp->flags);
    if (fec_hierarchy < ecmp_hierarchy)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Hierarchy value of FEC members %d should not be less than the hierarchy value of the ECMP group - %d\n",
                     fec_hierarchy, ecmp_hierarchy);
    }

    /*
     * Statistics check
     */
    if (ecmp->stat_id > L3_ECMP_MAX_STAT_ID_VALUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stat id %d, max value 0x%x", ecmp->stat_id, L3_ECMP_MAX_STAT_ID_VALUE);
    }
    SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                    (unit, BCM_CORE_ALL, ecmp->stat_id, ecmp->stat_pp_profile,
                     bcmStatCounterInterfaceIngressReceivePp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifies the validity of the new added member into the existing ECMP group
 * \param [in] unit -
 *   The unit number.
 * \param [in] old_interface -
 *   An already exists member in the ECMP group to verify that the new one have the same protection and hierarchy state.
 * \param [in] ecmp_protection_state -
 *   The ECMP group protection state.
 * \param [in] ecmp_hierarchy -
 *   The ECMP group hierarchy.
 * \param [in] intf - the added member to the existsting ECMP group.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 */
static shr_error_e
dnx_l3_egress_ecmp_add_member_verify(
    int unit,
    bcm_if_t old_interface,
    uint8 ecmp_protection_state,
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy,
    bcm_if_t intf)
{
    uint32 hierarchy;
    uint32 existing_hier;
    uint32 max_fec_id;
    uint8 is_allocated = 0;
    uint8 new_member_protection_satet;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));
    /*
     * Verify the intf value - make sure it's not less than 0 or more than the maximum number of FECs.
     */
    if (BCM_L3_ITF_VAL_GET(intf) > max_fec_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The FEC value %d is out of range - should be non-negative and no higher than %d.",
                     intf, max_fec_id);
    }

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(intf), &is_allocated));
    if (is_allocated == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ECMP member FEC with ID %d has not been allocated\n", intf);
    }

    /** Get hierarchy of intf */
    SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get(unit, BCM_L3_ITF_VAL_GET(intf), &hierarchy));
    SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get(unit, BCM_L3_ITF_VAL_GET(old_interface), &existing_hier));
    if (hierarchy != existing_hier)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "FEC member %d is in hierarchy %d but other existing members are in hierarchy %d", intf,
                     hierarchy, existing_hier);
    }

    /** Get the new member protection state */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, intf, &new_member_protection_satet));

    if (new_member_protection_satet != ecmp_protection_state)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "New member %d protection state %d isn't matching the ECMP group protection state %d.", intf,
                     new_member_protection_satet, ecmp_protection_state);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Removes an ECMP entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_index -
 *   The ECMP ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_egress_ecmp_group_destroy(
    int unit,
    uint32 ecmp_index)
{
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy;
    dnx_l3_ecmp_attributes_t ecmp_atr;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the real hierarchy ID.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, (uint32 *) &ecmp_hierarchy));

    /*
     * Get the ECMP attributes
     */
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_group_entry_result_get(unit, ecmp_index, &ecmp_atr));

    /** Delete the entry from the DBAL table */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_remove(unit, ecmp_index));

    /*
     * Deallocate the ECMP group from the resource manager
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_dealloc(unit, ecmp_index));

    /*
     * Release the profile
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, ecmp_atr.profile_index, ecmp_hierarchy));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Creates or updates an ECMP entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in,out] ecmp -
 *   The ECMP group information.
 *   The following are relevant field of the bcm_l3_egress_ecmp_t structure:
 *  - flags            -> BCM_L3_* flags (supported flags are BCM_L3_REPLACE and BCM_L3_WITH_ID).
 *  - ecmp_intf        -> The ECMP object.
 *  - max_paths        -> The maximal size of the ECMP group size.
 *                        if max_paths = 0 then it is ignore, otherwise -  ECMP max group size => max_paths >= intf_count.
 *                        In general members can be added to the group till it reaches MAX(max_paths,intf_count) size but no more than that.
 *  - ecmp_group_flags -> BCM_L3_ECMP_* flags (supported flags are BCM_L3_ECMP_LARGE_TABLE, BCM_L3_ECMP_EXTENDED).
 *  - dynamic_mode     -> The ECMP hashing mode, Indicate if the hashing mode is consistent of non-consistent using multiply and divide.
 *  - rpf_mode         -> Indicates the URPF mode for the ECMP group - loose, strict or disabled.
 * \param [in] intf_count -
 *   The ECMP current number of members in the group.
 * \param [in] intf_array -
 *  Array of the ECMP members either FEC-IDs or ECMP-IDs.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_create(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array)
{
    int ecmp_profile_index;
    uint32 fec_base;
    int intf_iter;
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_l3_egress_ecmp_t), ecmp);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_egress_ecmp_create_verify(unit, ecmp, intf_count, intf_array));

    /** Remove ecmp_intf type encapsulation before starting */
    ecmp->ecmp_intf = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);

    /*
     * Destroy ECMP group before adding it again in case REPLACE flag is provided.
     */
    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_group_destroy(unit, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf)));
    }
    /*
     * As the interface array isn't necessarily sorted we need to search for the base FEC.
     */
    fec_base = intf_array[0];
    for (intf_iter = 1; intf_iter < intf_count; intf_iter++)
    {
        fec_base = UTILEX_MIN(fec_base, intf_array[intf_iter]);
    }
    /*
     * Allocate the ECMP group using the resource manager.
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_alloc(unit, &ecmp->ecmp_intf, ecmp->flags, ecmp->ecmp_group_flags));

    /*
     * Allocate an ECMP profile
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_allocation
                    (unit, ecmp, intf_array, intf_count, fec_base, FALSE, &ecmp_profile_index));

    /*
     * Insert the ECMP group entry into the DBAL table.
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_set
                    (unit, ecmp->ecmp_intf, ecmp_profile_index, fec_base,
                     (ecmp->rpf_mode == bcmL3EcmpUrpfStrictEm ? 1 : 0)));

    /*
     * Update the ECMP interface with a prefix
     */
    BCM_L3_ITF_SET(ecmp->ecmp_intf, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Removes an ECMP entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   The ECMP group information.
 *   The following are relevant field of the bcm_l3_egress_ecmp_t structure:
 *  - ecmp_intf        -> The ECMP object.
 *  - dynamic_mode     -> The ECMP hashing mode, Indicate if the
 *    hashing mode is consistent of non-consistent using multiply and divide.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_destroy(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp)
{
    uint32 ecmp_index;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    ecmp_index = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
    SHR_INVOKE_VERIFY_DNX(dnx_l3_ecmp_group_verify(unit, ecmp_index));

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_group_destroy(unit, ecmp_index));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * bcm_l3_egress_ecmp_find API is not supported
 */
int
bcm_dnx_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t * intf_array,
    bcm_l3_egress_ecmp_t * ecmp)
{
    return _SHR_E_UNAVAIL;
}

/**
 * \brief
 *  Retrieves the member list for an ECMP entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in,out] ecmp -
 *   The ECMP group information.
 *   As input:
 *      - flags            -> BCM_L3_* flags (supported flags are BCM_L3_REPLACE and BCM_L3_WITH_ID).
 *      - ecmp_intf        -> The ECMP object.
 *      - max_paths        -> The maximal size of the ECMP group size.
 *                        if max_paths = 0 then it is ignore, otherwise -  ECMP max group size => max_paths >= intf_count.
 *                        In general members can be added to the group till it reaches MAX(max_paths,intf_count) size but no more than that.
 *      - ecmp_group_flags -> BCM_L3_ECMP_* flags.
 *   As output:
 *      - dynamic_mode     -> The ECMP hashing mode, Indicate if the hashing mode is consistent of non-consistent using multiply and divide.
 * \param [in] intf_size - number of allocated entries in
 *        intf_array
 * \param [out] intf_array - Array of the ECMP members either
 *        FEC-IDs or ECMP-IDs.
 * \param [out] intf_count - The number of members returned in
 *          the intf_array.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_get(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    uint32 ecmp_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");

    ecmp_index = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
    SHR_INVOKE_VERIFY_DNX(dnx_l3_ecmp_group_verify(unit, ecmp_index));

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_group_data_get(unit, ecmp_index, ecmp, intf_size, intf_array, intf_count));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Adds another member to an ECMP group.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   The ECMP group information.
 * \param [in] intf - member to be added
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_add(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    bcm_if_t intf)
{
    int ecmp_profile_index;
    uint32 ecmp_index;
    int intf_count = 0;
    uint8 is_protected;
    int intf_iter;
    bcm_l3_egress_ecmp_t current_ecmp;
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy;
    bcm_if_t members[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    dnx_l3_ecmp_attributes_t ecmp_atr;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    sal_memset(&current_ecmp, 0, sizeof(bcm_l3_egress_ecmp_t));
    /*
     * Get the requested ECMP group
     */
    current_ecmp.ecmp_intf = ecmp->ecmp_intf;

    ecmp_index = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
    SHR_INVOKE_VERIFY_DNX(dnx_l3_ecmp_group_verify(unit, ecmp_index));
    /*
     * get the hierarchy of the ECMP group using its ID.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, (uint32 *) &ecmp_hierarchy));

    current_ecmp.flags = L3_EGRESS_ECMP_GET_HIER_FLAGS(ecmp_hierarchy);
    SHR_IF_ERR_EXIT(bcm_dnx_l3_egress_ecmp_get
                    (unit, &current_ecmp, L3_ECMP_MAX_NOF_ECMP_MEMBERS, members, &intf_count));

    /*
     * Get the ECMP group protection state by testing one of the ECMP group members (the first one will do)
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, members[0], &is_protected));

    /*
     * Receive the ECMP group profile index and the data for the ecmp group profile.
     */
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_group_entry_result_get(unit, ecmp_index, &ecmp_atr));

    SHR_INVOKE_VERIFY_DNX(dnx_l3_egress_ecmp_add_member_verify(unit, members[0], is_protected, ecmp_hierarchy, intf));

    /*
     * Determine the place for the new member in the group - front of queue or back.
     */
    if (current_ecmp.dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
    {

        /*
         * Verify that the intf value is in range.
         */
        if (((current_ecmp.max_paths << is_protected) + ecmp_atr.fec_base <= intf) || intf < ecmp_atr.fec_base)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid value %u for new member of ECMP group %u - expecting value between %u and %u\n",
                         intf, ecmp_index, ecmp_atr.fec_base,
                         (current_ecmp.max_paths << is_protected) + ecmp_atr.fec_base);
        }

        for (intf_iter = 0; intf_iter < intf_count; intf_iter++)
        {
            if (members[intf_iter] == intf)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Interface %u already exists in ECMP group %u\n", intf, ecmp_index);
            }
        }

        /*
         * Add the new interface into the array and update the NOF interfaces in the ECMP
         */
        members[intf_count++] = intf;
        /*
         * In case the table becomes a full member group it can use a general profile with a general member table that all the full tables
         * with the same NOF members are using, otherwise use a unique profile that was allocated for this ECMP group.
         */
        if (current_ecmp.max_paths == intf_count)
        {

            /*
             * Get the general profile group of all the consistent ECMP group with the size of current_ecmp.max_paths and without any removed
             * member
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_allocation
                            (unit, &current_ecmp, members, intf_count, ecmp_atr.fec_base, FALSE, &ecmp_profile_index));

            /*
             * Update the ECMP group with the general ECMP profile index used for all the ECMP group with the same parameters.
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_set
                            (unit, ecmp_index, ecmp_profile_index, ecmp_atr.fec_base, ecmp_atr.is_rpf_mode_em));

            /*
             * release the old profile which was dedicated to this ECMP group without all the members in it after a new one was allocated
             * to preserve the traffic.
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, ecmp_atr.profile_index, ecmp_hierarchy));
        }
        else
        {
            uint32 consistent_hashing_manager_handle;

            SHR_IF_ERR_EXIT(ecmp_db_info.
                            consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));
            /*
             * The ECMP group isn't full yet (even after adding this member), to add the requested member use the CHM add function
             * that uses the current members table to add the new member with minimal affect on the existing ECMP paths.
             */
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_member_add(unit,
                                                                                   consistent_hashing_manager_handle,
                                                                                   L3_ECMP_CHM_KEY_GET
                                                                                   (ecmp_atr.profile_index,
                                                                                    ecmp_hierarchy),
                                                                                   intf - ecmp_atr.fec_base,
                                                                                   intf_count - 1, NULL));
        }

    }
    else
    {

        if (current_ecmp.max_paths == L3_ECMP_MAX_NOF_ECMP_MEMBERS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The ECMP already contains the maximal number of members (%d) %u\n",
                         current_ecmp.max_paths, current_ecmp.max_paths);
        }
        /*
         * If the ID of the new member that we are adding is:
         *          * the first current member ID - 1 then it goes to the front.
         *          * equal to the first member + the size of the group, then it goes to the back.
         * If it doesn't fall into either of those categories, then an error will be returned that the Id is incorrect.
         */
        if (ecmp_atr.fec_base == intf + (1 + is_protected))
        {
            /** new member goes in the front */
            ecmp_atr.fec_base = intf;
        } /** New member doesn't goes to the front and not to the end*/
        else if (ecmp_atr.fec_base + (intf_count << is_protected) != intf)
        {

            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid value %u for new member of ECMP group %u\n", intf, ecmp_index);
        }

        intf_count++;
        current_ecmp.max_paths++;

        /*
         * Create the new profile with the new member
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_allocation
                        (unit, &current_ecmp, members, intf_count, ecmp_atr.fec_base, FALSE, &ecmp_profile_index));
        /*
         * Update the ECMP group with the new ecmp profile index and the fec base.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_set
                        (unit, ecmp_index, ecmp_profile_index, ecmp_atr.fec_base, ecmp_atr.is_rpf_mode_em));
        /*
         * release the old profile after a new one was allocated to preserve the traffic.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, ecmp_atr.profile_index, ecmp_hierarchy));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Remove a member from an ECMP group.
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp -
 *   The ECMP group information.
 * \param [in] intf - member to be removed
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_delete(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    bcm_if_t intf)
{
    int ecmp_profile_index;
    uint32 ecmp_index;
    int intf_count;
    uint8 is_protected;
    int intf_iter;
    bcm_l3_egress_ecmp_t current_ecmp;
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy;
    bcm_if_t members[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    dnx_l3_ecmp_attributes_t ecmp_atr;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    sal_memset(&current_ecmp, 0, sizeof(bcm_l3_egress_ecmp_t));
    /*
     * Get the requested ECMP group
     */
    current_ecmp.ecmp_intf = ecmp->ecmp_intf;
    ecmp_index = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
    SHR_INVOKE_VERIFY_DNX(dnx_l3_ecmp_group_verify(unit, ecmp_index));
    /*
     * get the hierarchy of the ECMP group using its ID.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_index, (uint32 *) &ecmp_hierarchy));

    current_ecmp.flags = L3_EGRESS_ECMP_GET_HIER_FLAGS(ecmp_hierarchy);
    SHR_IF_ERR_EXIT(bcm_dnx_l3_egress_ecmp_get
                    (unit, &current_ecmp, L3_ECMP_MAX_NOF_ECMP_MEMBERS, members, &intf_count));

    /*
     * Get the ECMP group protection state by testing one of the ECMP group members (the first one will do)
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, members[0], &is_protected));

    /*
     * Receive the ECMP group profile index and the data for the ecmp group profile.
     */
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_group_entry_result_get(unit, ecmp_index, &ecmp_atr));

    /*
     * Verify that we are not attempting to delete the last member of an ECMP group.
     * If so, return an error that this action is illegal.
     */
    if (intf_count == 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal action deleting last member of ECMP group. It cannot have zero members.\n");
    }

    /*
     * Determine the place from where the member is to be deleted - front of queue or back.
     */
    if (current_ecmp.dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
    {
        uint32 consistent_hashing_manager_handle;

        int remove_pos = -1;
        /*
         * Verify that the intf value is in range.
         */
        if ((((current_ecmp.max_paths << is_protected) + ecmp_atr.fec_base) <= intf) || (intf < ecmp_atr.fec_base))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid value %u for deleting a member of ECMP group %u\n", intf, ecmp_index);
        }

        for (intf_iter = 0; intf_iter < intf_count; intf_iter++)
        {
            if (members[intf_iter] == intf)
            {
                remove_pos = intf_iter;
                break;
            }
        }

        if (remove_pos == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Interface %d to be removed was not found in ECMP group %d existing members\n",
                         intf, ecmp_index);
        }

        SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));
        /*
         * In case this is the first member that is removed a dedicated member table should be allocated which mean a dedicated ECMP profile
         * should be allocated.
         */
        if (current_ecmp.max_paths == intf_count)
        {

            /*
             * Create a new ECMP profile with the same attributes that the ECMP group holds before any member was removed from but is dedicated
             * only for this ECMP group.
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_allocation
                            (unit, &current_ecmp, members, intf_count, ecmp_atr.fec_base, TRUE, &ecmp_profile_index));

            /*
             * Update the ECMP group with the new profile index the is dedicated for it.
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_set
                            (unit, ecmp_index, ecmp_profile_index, ecmp_atr.fec_base, ecmp_atr.is_rpf_mode_em));

            /*
             * Using the CHM use the new profile to remove a member from the group an assign a new members table to this profile which is
             * unique only for this ECMP group.
             */
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_member_remove(unit,
                                                                                      consistent_hashing_manager_handle,
                                                                                      L3_ECMP_CHM_KEY_GET
                                                                                      (ecmp_profile_index,
                                                                                       ecmp_hierarchy),
                                                                                      intf - ecmp_atr.fec_base,
                                                                                      intf_count, NULL));

            /*
             * release the old profile after a new one was allocated to preserve the traffic.
             */
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, ecmp_atr.profile_index, ecmp_hierarchy));
        }
        else
        {

            /*
             * The ECMP group wasn't full (even before removing this member), to remove the requested member use the CHM remove function
             * that uses the current members table to remove the requested member without any affect on the existing ECMP paths.
             */
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_member_remove(unit,
                                                                                      consistent_hashing_manager_handle,
                                                                                      L3_ECMP_CHM_KEY_GET
                                                                                      (ecmp_atr.profile_index,
                                                                                       ecmp_hierarchy),
                                                                                      intf - ecmp_atr.fec_base,
                                                                                      intf_count, NULL));
        }

    }
    else
    {
        /*
         * If the ID of the member that we are removing is: * equal to the first current member ID, then it will be
         * removed from the front.  * equal to the first member + the size of the group, then it will be removed from
         * the back. If it doesn't fall into either of those categories, then an error will be returned that the Id is
         * incorrect.
         */
        if (ecmp_atr.fec_base == intf)
        {
            /** removing member from the front */
            ecmp_atr.fec_base = intf + (1 + is_protected);
        }/** Removed member doesn't belongs to the front and not to the end*/
        else if (ecmp_atr.fec_base + ((intf_count - 1) << is_protected) != intf)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid value %u for removing a member from an ECMP group\n", intf);
        }

        intf_count--;
        current_ecmp.max_paths--;

        /*
         * Create the new ecmp profile without the intf member.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_allocation
                        (unit, &current_ecmp, members, intf_count, ecmp_atr.fec_base, FALSE, &ecmp_profile_index));
        /*
         * Update the ECMP group with the new profile index and fec base.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_entry_set
                        (unit, ecmp_index, ecmp_profile_index, ecmp_atr.fec_base, ecmp_atr.is_rpf_mode_em));
        /*
         * release the old profile after a new one was allocated to preserve the traffic.
         */
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_profile_free(unit, ecmp_atr.profile_index, ecmp_hierarchy));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function verifies that the consistent hashing manager (chm) is legal.
 */
shr_error_e
chm_input_verify(
    int unit,
    uint32 chm_profile_offset,
    l3_ecmp_consistent_type_e consistent_table_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((consistent_table_type < L3_ECMP_CONSISTENT_TABLE_SMALL)
        || (consistent_table_type > L3_ECMP_CONSISTENT_TABLE_LARGE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Type %d isn't a valid consistent hashing table type.", consistent_table_type);
    }

    if (chm_profile_offset >= L3_ECMP_TOTAL_NOF_RESOURCES(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "chm profile offset %d exceeds max NOF resources (%d).", chm_profile_offset,
                     L3_ECMP_TOTAL_NOF_RESOURCES(unit));
    }

    /*
     * Each table is placed in an offset which is a multiplication of it NOF resources.
     */
    if ((chm_profile_offset % ecmp_consistent_types_attr[consistent_table_type].nof_resources) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Profile offset %d is invalid for table of type %d that takes %d resources.",
                     chm_profile_offset, consistent_table_type,
                     ecmp_consistent_types_attr[consistent_table_type].nof_resources);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given an ECMP profile (profile_key) and an address in the ECMP members table (calculated from the chm_resource_offset)
 * updates the ECMP members base field in the profile.
 * Note that this function assumes that the ECMP mode field in the profile was already written.
 */
shr_error_e
update_members_address_in_ecmp_profile(
    int unit,
    uint32 profile_key,
    uint32 chm_resource_offset)
{
    dbal_enum_value_field_ecmp_mode_e ecmp_mode = 0;
    l3_ecmp_consistent_type_e consistent_table_type = 0;
    uint32 entry_handle_id;
    uint32 ecmp_profile_indx;
    dbal_enum_value_field_hierarchy_level_e ecmp_hierarchy;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    ecmp_profile_indx = L3_ECMP_PROFILE_ID_FROM_CHM_KEY_GET(profile_key);

    ecmp_hierarchy = L3_ECMP_HIERARCHY_FROM_CHM_KEY_GET(profile_key);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));

    /*
     * The members table offset in the table determine by the actual table offset in the table and the consistent table size.
     * The addresses is alginate to the table size so the address in the profile will be "actual offset / table size"
     * An access to to the profile which was already field with the table type is made to get the table size and then the
     * relative offset to the table is updated.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, ecmp_profile_indx);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, ecmp_hierarchy);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MODE, INST_SINGLE, &ecmp_mode);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, ecmp_profile_indx);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, ecmp_hierarchy);

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get(unit, ecmp_mode, &consistent_table_type));

    SHR_IF_ERR_EXIT(chm_input_verify(unit, chm_resource_offset, consistent_table_type));
    /*
     * The chm NOF of resources divided by the table type NOF resources will provide us the relative
     * place of this table in the HW.
     * relative place means that if the HW members banks had only this type of table, it will be it serial number.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE, INST_SINGLE,
                                 chm_resource_offset / ecmp_consistent_types_attr[consistent_table_type].nof_resources);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_get_nof_resources_from_table_size(
    int unit,
    int profile_type,
    uint32 *nof_resources)
{
    SHR_FUNC_INIT_VARS(unit);

    L3_ECMP_CONSISTENT_TYPE_VERIFY(profile_type);

    *nof_resources = ecmp_consistent_types_attr[profile_type].nof_resources;

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_get_table_size_from_nof_resources(
    int unit,
    uint32 nof_resources,
    int *profile_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (nof_resources)
    {
        case L3_ECMP_CONSISTENT_SMALL_RESOURCE:
            *profile_type = L3_ECMP_CONSISTENT_TABLE_SMALL;
            break;
        case L3_ECMP_CONSISTENT_MEDIUM_RESOURCE:
            *profile_type = L3_ECMP_CONSISTENT_TABLE_MEDIUM;
            break;
        case L3_ECMP_CONSISTENT_LARGE_RESOURCE:
            *profile_type = L3_ECMP_CONSISTENT_TABLE_LARGE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "There is no consistent profile type that holds %d resources.",
                         nof_resources);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_members_table_set(
    int unit,
    uint32 chm_offset,
    consistent_hashing_calendar_t * calendar)
{

    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 row_indx, member_indx;
    uint32 calendar_ptr = 0;
    uint32 member_tbl_address;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(chm_input_verify(unit, chm_offset, calendar->profile_type));

    member_tbl_address = L3_ECMP_GET_MEMBERS_TBL_ADDR_FROM_CHM_OFFSET(chm_offset);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FEC_ECMP_MEMBERS, &entry_handle_id));

    if (ecmp_consistent_types_attr[calendar->profile_type].member_size_in_bits == L3_ECMP_4_BITS_MEMBER)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_FEC_ECMP_MEMBERS_4B_MEMBER_SIZE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_FEC_ECMP_MEMBERS_8B_MEMBER_SIZE);
    }

    /*
     * Using a nested loop each member in each row of the table is updated
     */
    for (row_indx = 0; row_indx < ecmp_consistent_types_attr[calendar->profile_type].nof_rows_in_members_table;
         row_indx++)
    {

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_ROW_ID, member_tbl_address + row_indx);

        for (member_indx = 0; member_indx < ecmp_consistent_types_attr[calendar->profile_type].nof_members_in_table_row;
             member_indx++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_VAL, member_indx,
                                         calendar->lb_key_member_array[calendar_ptr++]);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_members_table_get(
    int unit,
    uint32 chm_offset,
    consistent_hashing_calendar_t * calendar)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 row_indx, member_indx;
    uint32 member_index = 0;
    uint32 member_tbl_address;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(chm_input_verify(unit, chm_offset, calendar->profile_type));

    member_tbl_address = L3_ECMP_GET_MEMBERS_TBL_ADDR_FROM_CHM_OFFSET(chm_offset);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FEC_ECMP_MEMBERS, &entry_handle_id));

    /*
     * Using a nested loop each member in each row of the table is updated into the consistent hashing manager calendar.
     */
    for (row_indx = 0; row_indx < ecmp_consistent_types_attr[calendar->profile_type].nof_rows_in_members_table;
         row_indx++)
    {

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_ROW_ID, member_tbl_address + row_indx);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        for (member_indx = 0; member_indx < ecmp_consistent_types_attr[calendar->profile_type].nof_members_in_table_row;
             member_indx++)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_VAL, member_indx,
                             &calendar->lb_key_member_array[member_index++]));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_update_members_table_offset(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    int profile_type)
{
    sw_state_ll_node_t node;
    sw_state_ll_node_t *node_ptr;
    uint32 profile_key;
    consistent_hashing_calendar_t calendar;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(chm_input_verify(unit, old_offset, profile_type));
    SHR_IF_ERR_EXIT(chm_input_verify(unit, new_offset, profile_type));

    /*
     * Get the members from the old members table and place it in the new one.
     */
    calendar.profile_type = profile_type;
    SHR_IF_ERR_EXIT(ecmp_chm_members_table_get(unit, old_offset, &calendar));
    SHR_IF_ERR_EXIT(ecmp_chm_members_table_set(unit, new_offset, &calendar));

    /*
     * Read the profile that need to be updated
     */

    node_ptr = &node;

    SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.get_first(unit, old_offset, node_ptr));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        /*
         * Read the profile that need to be updated
         */
        SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.node_value(unit, *node_ptr, &profile_key));
        /*
         * Update the profile in the HW with the new address
         */
        SHR_IF_ERR_EXIT(update_members_address_in_ecmp_profile(unit, profile_key, new_offset));
        /*
         * Add the new address resource the update profile
         */
        SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.add_last(unit, new_offset, &profile_key));
        /*
         * Remove the profile from the old resource
         */
        SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.remove_node(unit, old_offset, *node_ptr));
        /*
         * Take the next profile from the old resource list if exists for update with the new address
         */
        SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.get_first(unit, old_offset, node_ptr));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_update_ecmp_profile_with_members_table_offset(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 chm_offset,
    void *user_info)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(update_members_address_in_ecmp_profile(unit, unique_identifyer, chm_offset));

    /*
     * Add this profile to the link list
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.add_last(unit, chm_offset, &unique_identifyer));

exit:
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_members_table_offset_get(
    int unit,
    uint32 unique_identifyer,
    uint32 *chm_offset)
{
    l3_ecmp_consistent_type_e consistent_table_type;
    dbal_enum_value_field_ecmp_mode_e ecmp_mode = 0;
    uint32 entry_handle_id;
    uint32 member_base_address = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE,
                               L3_ECMP_PROFILE_ID_FROM_CHM_KEY_GET(unique_identifyer));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL,
                               L3_ECMP_HIERARCHY_FROM_CHM_KEY_GET(unique_identifyer));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MODE, INST_SINGLE, &ecmp_mode);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE, INST_SINGLE, &member_base_address);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get(unit, ecmp_mode, &consistent_table_type));
    /*
     * The profile in the HW is relative to the used table so the get the real address the HW address is multiply by the table size.
     */
    *chm_offset = member_base_address * ecmp_consistent_types_attr[consistent_table_type].nof_resources;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more  details see .h file
 */
shr_error_e
ecmp_chm_entries_in_profile_get_cb(
    int unit,
    int profile_type,
    uint32 max_nof_members_in_profile,
    uint32 *nof_calendar_entries)
{
    SHR_FUNC_INIT_VARS(unit);

    L3_ECMP_CONSISTENT_TYPE_VERIFY(profile_type);

    *nof_calendar_entries = L3_ECMP_GET_MEMBERS_TABLE_USED_ROWS(profile_type, max_nof_members_in_profile);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Goes over all ECMP groups that have been created and runs
 *  a callback function for each one.
 * \param [in] unit - The unit number.
 * \param [in] trav_fn - a pointer to the callback function
 * \param [in] user_data - data sent from the user that will be
 *             passed to the callback function as 5th input parameter
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_traverse(
    int unit,
    bcm_l3_egress_ecmp_traverse_cb trav_fn,
    void *user_data)
{
    bcm_l3_egress_ecmp_t ecmp;
    int is_end;
    int intf_count = 0;
    uint32 group_count = 0;
    uint32 entry_handle_id;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    bcm_if_t intf_array[L3_ECMP_MAX_NOF_ECMP_MEMBERS];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify that a callback function has been provided */
    if (trav_fn == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No callback function has been provided to the ECMP traverse API\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TABLE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /*
     * Receive first entry in table.
     * If is_end is set to 1 after calling this API, then the table is empty.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    /*
     * is_end variable will be set to 1 when all entries including the last one are traversed
     * and *_iterator_get_next API is called again.
     */
    while (!is_end)
    {
        bcm_l3_egress_ecmp_t_init(&ecmp);
        /** Receive ECMP group ID which is key for the ECMP_TABLE - its value can be up to 40959 */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_ECMP_ID, field_value));
        ecmp.ecmp_intf = field_value[0];

        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_group_data_get(unit, ecmp.ecmp_intf, &ecmp, L3_ECMP_MAX_NOF_ECMP_MEMBERS,
                                                          intf_array, &intf_count));
        BCM_L3_ITF_SET(ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, ecmp.ecmp_intf);

        /** Invoke callback function */
        SHR_IF_ERR_EXIT((*trav_fn) (unit, &ecmp, intf_count, intf_array, user_data));

        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        group_count++;
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "ECMP traverse found %u valid entries\n"), group_count));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * }
 */
