/**
 * \file algo_l3.h Internal DNX L3 Managment APIs
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef ALGO_L3_H_INCLUDED
/*
 * {
 */
#define ALGO_L3_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/**
 * Defines
 * {
 */

/**
 * Resource name defines for algo l3.
 * \see
 * dnx_algo_l3_ingress_mymac_prefix_entry_template_create
 * dnx_algo_l3_source_address_table_init
 */
#define DNX_ALGO_L3_INGRESS_MYMAC_PREFIX_TABLE_RESOURCE     "Ingress MyMac prefix table"

#define DNX_ALGO_L3_EGRESS_SOURCE_ADDRESS_TABLE_RESOURCE    "Egress source address table"

#define DNX_ALGO_L3_ECMP_PROFILE_HIER_1_TABLE_RESOURCE      "ECMP first hierarchy profile table"

#define DNX_ALGO_L3_ECMP_PROFILE_HIER_2_TABLE_RESOURCE      "ECMP second hierarchy profile table"

#define DNX_ALGO_L3_ECMP_PROFILE_HIER_3_TABLE_RESOURCE      "ECMP third hierarchy profile table"

#define DNX_ALGO_L3_ECMP_CONSISTENT_MANAGER_HANDLE          "ECMP consistent hashing manager handle"

#define DNX_ALGO_L3_VRRP_VSI_TCAM_TABLE_RESOURCE            "VRRP MyMac TCAM VSI table"

#define DNX_ALGO_L3_VRRP_EXEM_TCAM_TABLE_RESOURCE           "VRRP MyMac TCAM EXEM table"

#define DNX_ALGO_L3_VRRP_EXEM_STATION_ID_RESOURCE           "VRRP MyMac EXEM station ID"

#define DNX_ALGO_L3_LIF_LOCAL_OUTLIF                        "LIF local outlif"

/**
 * Resource name defines for algo l3 ECMP
 */
#define DNX_ALGO_L3_RES_ECMP_GROUP "DNX_ALGO_L3_RES_ECMP_GROUP"
#define DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP "DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP"
/*
 * represent a FEC invalid logical address.
 */
#define ALGO_L3_INVALID_LOGICAL_ADDRESS -1
/**
 * Resource cb defines for algo l3 ECMP
 */
#define DNX_ALGO_L3_RES_ECMP_GROUP_CB ecmp_db_info.ecmp_res_manager
#define DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP_CB ecmp_db_info.ecmp_extended_res_manager

/**
 * Resource name defines for algo L3 FEC in the resource manager
 */
#define DNX_ALGO_L3_RES_FEC "DNX_ALGO_L3_FEC"

/** Define flags for the FEC allocation */
/** Flag which is used to indicate with-id allocation */
#define DNX_ALGO_L3_FEC_WITH_ID         (0x00000001)

/** Translate fec_id (20bits) to super fec_id (19 bits) by taking bits [1:19] */
#define DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(_fec_id_) (_fec_id_ >> 1)

/** Retrieve the physical FEC from the FEC ID ((super_FEC * fec_size) / row_size)*/
#define DNX_ALGO_L3_PHYSICAL_FEC_GET(unit,fec_id) (DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(fec_id) * dnx_data_l3.fec.super_fec_size_get(unit) /  dnx_data_l3.fec.fec_row_size_get(unit))
/** Find out whether the current bank_id has a big size of 26216 or not (26214) */
#define ALGO_L3_FEC_BANK_IS_BIG(bank_id) (bank_id % 5 == 0)
/** Receive the number of bigger banks which exist before the current bank_id */
#define ALGO_L3_NOF_BIG_BANKS_BEFORE_CURRENT(bank_id) ((bank_id + 4) / 5)
/*
 * FEC pair subresourcs have a consultive banks ID where the even bank ID has the lower value
 */
#define ALGO_L3_GET_FEC_EVEN_SUB_RESOURCE(sub_resource_id) (sub_resource_id & (UTILEX_U32_MAX-1))
/*
 * Get the matching sub-resource pair, sub-resource pairs are even and odd consecutive numbers (where the even is the first in the pair).
 */
#define ALGO_L3_GET_FEC_SUB_RESOURCE_PAIR(sub_resource_id) (ALGO_L3_GET_FEC_EVEN_SUB_RESOURCE(sub_resource_id) + (1 - (sub_resource_id & 1) ))
/**
 * }
 */

/**
 * emums
 * {
 */
/*
 * List of the the FEC banks groups types that are used during the FEC bank allocation.
 */
typedef enum
{
    /*
     * Information of the cluster and banks that are to be updated for the requested resource.
     */
    ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE = 0,
    /*
     * Information of the clusters that need to be moved in order to allocate the requested resource.
     */
    ALGOL_L3_FEC_BANKS_GROUP_TO_MOVE = 1,
    /*
     * Number of bank allocation information types.
     */
    ALGOL_L3_NOF_FEC_BANKS_GROUP_TYPES = 2
} algo_l3_fec_banks_group_type_e;

/*
 * List of the the FEC banks groups types that are used during the FEC bank allocation.
 */
typedef enum
{
    /*
     * The requested bank resource
     */
    ALGOL_L3_FEC_BANK_REQUESTED_TYPE = 0,
    /*
     * The pair of the requested bank resource (a pair of resources is two consecutive number with a leading even number)
     */
    ALGOL_L3_FEC_BANK_REQUESTED_PAIR_TYPE = 1,
    /*
     * NOF of bank types
     */
    ALGOL_L3_NOF_FEC_BANK_TYPES = 2
} algo_l3_fec_banks_types_in_group_e;

/**
 * }
 */

/**
 * Structures
 * {
 */
/**
 * \brief - VRRP tcam key fields
 */
typedef struct
{
    /**
     * Destination MAC address
     */
    bcm_mac_t mac_da;

    /**
     * Destination MAC address mask
     */
    bcm_mac_t mac_da_mask;

    /**
     * Protocol group ID
     */
    uint32 protocol_group;

    /**
     * Protocol group ID mask
     */
    uint32 protocol_group_mask;
} algo_dnx_l3_vrrp_tcam_key_t;

/**
 * \brief - The information of the max FEC banks group allocation.
 */
typedef struct
{
    /**
     * The Bank ID that the cluster should move to.
     */
    int bank_id[ALGOL_L3_NOF_FEC_BANK_TYPES];

    /**
     * The cluster that needs to be update into the requested position
     */
    mdb_cluster_alloc_info_t cluster_to_update[ALGOL_L3_NOF_FEC_BANK_TYPES];

} algo_dnx_l3_fec_cluster_update_info_t;

/**
 * \brief - Information of all the clusters that needs to be updated in a single group of banks allocation.
 */
typedef struct
{
    /**
     * Indicate if a resource was found for the requested banks allocation.
     */
    uint8 resource_was_found;
    /**
     * Holds cluster to bank mapping of clusters that should be replaced during the allocation process.
     */
    algo_dnx_l3_fec_cluster_update_info_t clusters_info[ALGOL_L3_NOF_FEC_BANKS_GROUP_TYPES];

} algo_dnx_l3_fec_banks_update_info_t;

/**
 * }
 */

/**
* \brief
*   gets the NOF of FEC allocation banks.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*     Identifier of the device to access.
*    \param [out] fec_banks -
*      NOF of FEC allocation banks.
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_l3_nof_fec_banks_get(
    int unit,
    uint32 *fec_banks);

/**
* \brief
*   Intialize l3 algorithms.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*  \par INDIRECT INPUT:
*    - DBAL table sizes, used to initialize source address table template.
*  \par DIRECT OUTPUT:
*    shr_error_e -
*      Error return value
*  \par INDIRECT OUTPUT:
*      None
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_l3_init(
    int unit);

/**
* \brief
*   Deintialize l3 algorithms.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*  \par INDIRECT INPUT:
*    - DBAL table sizes, used to initialize source address table template.
*  \par DIRECT OUTPUT:
*    shr_error_e -
*      Error return value
*  \par INDIRECT OUTPUT:
*      None
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_l3_deinit(
    int unit);

/**
* \brief
*   Print an entry of the ingress mymac prefix template. See
*       \ref dnx_algo_template_print_data_cb for more details.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*     Identifier of the device to access.
*    \param [in] data -
*      Pointer of the struct to be printed.
*      \b As \b input - \n
*       The pointer is expected to hold a struct of type bcm_mac_t. \n
*       It's the user's responsibility to verify it beforehand.
*    \param [in] print_cb_data -
*      Pointer of the print callback data.
*  \par INDIRECT INPUT:
*    \b *data \n
*     See DIRECT INPUT above
*  \par DIRECT OUTPUT:
*    None.
*  \par INDIRECT OUTPUT:
*      The default output stream.
*  \remark
*    None
*  \see
*    dnx_algo_template_print_data_cb
*    shr_error_e
*****************************************************/
void dnx_algo_l3_print_ingress_mymac_prefix_entry_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief
 *  prints a VRRP multiple MyMac TCAM profile.  See \ref dnx_algo_template_print_data_cb for more details.
 * \param [in] unit -
 *   The unit number.
 * \param [in] data -
 *  pointer to an VRRP TCAM profile structure.
 *    \param [in] print_cb_data -
 *      Pointer to the print callback data.
 * \return
 *   None
 */
void dnx_algo_l3_print_vrrp_entry_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief
 *  prints an ECMP profile.  See \ref dnx_algo_template_print_data_cb for more details.
 * \param [in] unit -
 *   The unit number.
 * \param [in] data -
 *  pointer to an ECMP profile structure.
 *    \param [in] print_cb_data -
 *      Pointer to the print callback data.
 * \return
 *   None
 */
void dnx_algo_l3_print_ecmp_profiles_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief
 *   Receive the hierarchy of the ECMP group based on its ID.
 *   The data is taken from the SW state parameter where the
 *   information for the hierarchy of each 2k stage is kept.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP ID of the wanted element.
 * \param [out] hierarchy -
 *   The hierarchy value of the ECMP group
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_add
 *  * bcm_dnx_l3_egress_ecmp_delete
 *  * bcm_dnx_l3_egress_ecmp_destroy
 */
shr_error_e dnx_algo_l3_ecmp_hierarchy_get(
    int unit,
    int ecmp_index,
    uint32 *hierarchy);

/**
 * \brief
 *   Finds out if the given ECMP group is the only one added in its range of 2k values.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_id - The ECMP group id.
 * \param [out] bank_id - The bank to which the ECMP belongs.
 * \param [out] is_alone_in_bank -
 *   Boolean value indicating whether the ECMP ID is first in the range or not
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_egress_ecmp_create
 */
shr_error_e dnx_algo_l3_ecmp_group_is_alone_in_bank(
    int unit,
    int ecmp_id,
    int *bank_id,
    int *is_alone_in_bank);

/**
 * \brief
 *   Finds out if the given ECMP group is allocated in the resource.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP group id.
 * \param [out] is_allocated - Indication whether the group exists.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_ecmp_group_verify
 */
shr_error_e dnx_algo_l3_ecmp_is_allocated(
    int unit,
    int ecmp_index,
    uint8 *is_allocated);

/**
 * \brief
 *   Allocates an ECMP group in the resource with a given hierarchy.
 * \param [in] unit - The unit number.
 * \param [in,out] ecmp_index - The ECMP group id.
 *          As input:
 *              * when flag BCM_L3_WITH_ID is provided to ecmp_flags.
 *          As output:
 *              * when flag BCM_L3_WITH_ID is missing.
 * \param [in] ecmp_flags - Control flags for the allocation in the resource
 * \param [in] ecmp_group_flags - Control flags for ECMP group
 * \param [in] hierarchy -
 *   Hierarchy value of the ECMP group.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_ecmp_entry_alloc
 */
shr_error_e dnx_algo_l3_ecmp_allocate(
    int unit,
    int *ecmp_index,
    uint32 ecmp_flags,
    uint32 ecmp_group_flags,
    uint32 hierarchy);

/**
 * \brief
 *   Deallocates an ECMP from the resource using its ID.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_index - The ECMP group id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_ecmp_entry_dealloc
 */
shr_error_e dnx_algo_l3_ecmp_deallocate(
    int unit,
    int ecmp_index);

/**
 * \brief
 *   Reserves a bank of memory for the use of the FEC protection
 * \param [in] unit - The unit number.
 * \param [in] bank_id - The bank ID that is going to be reserved.
 *             Valid bank IDs are 0 to 3.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_ecmp_extended_bank_for_failover_allocate(
    int unit,
    int bank_id);

/**
 * \brief
 *   Free a bank of memory from the use of FEC protection.
 *   It can be used by FEC protection and ECMP extended mode.
 * \param [in] unit - The unit number.
 * \param [in] bank_id - The bank ID that is going to be freed.
 *             Valid bank IDs are 0 to 3.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_ecmp_extended_bank_for_failover_destroy(
    int unit,
    int bank_id);

/**
 * \brief
 *   Allocates a FEC entry in the resource.
 * \param [in] unit - The unit number.
 * \param [in,out] fec_index - The FEC id.
 *          As input:
 *              * when flag BCM_L3_WITH_ID is provided to fec_flags.
 *          As output:
 *              * when flag BCM_L3_WITH_ID is missing.
 * \param [in] flags - DNX_ALGO_L3_ Control flags for the allocation in the resource
 * \param [in] hierarchy - Hierarchy value:
 *          * DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1
 *          * DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2
 *          * DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3
 * \param [in] fec_resource_type - Super FEC resource type:
 *          * DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_NO_STAT
 *          * DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT
 *          * DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_W_STAT
 *          * DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT
 * \param [out] bank_id - The bank ID to which the fec_index belongs to
 *          * -1 if there's no need to update the hierarchy mapping
 *          * != -1 if the hierarchy mapping needs to be updated.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_create_fec_allocate
 */
shr_error_e dnx_algo_l3_fec_allocate(
    int unit,
    int *fec_index,
    uint32 flags,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    dbal_enum_value_field_fec_resource_type_e fec_resource_type,
    int *bank_id);

/**
 * \brief
 *   Deallocates a FEC ID from the resource.
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC id.
 * \param [out] bank_id - The bank ID to which the fec_index belongs to
 *          * -1 if there's no need to update the hierarchy mapping
 *          * != -1 if the hierarchy mapping needs to be updated.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_destroy_fec_deallocate
 */
shr_error_e dnx_algo_l3_fec_deallocate(
    int unit,
    int fec_index,
    int *bank_id);

/**
 * \brief
 *   Receive the hierarchy of the FEC based on its ID.
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC ID of the wanted element.
 * \param [out] hierarchy_p -
 *   The hierarchy value of the FEC
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_l3_fec_bitmap_allocate
 *  * dnx_l3_egress_fec_entry_add
 *  * dnx_l3_egress_fec_entry_get
 */
shr_error_e algo_l3_fec_hierarchy_stage_map_get(
    int unit,
    uint32 fec_index,
    uint32 *hierarchy_p);

/**
 * \brief
 *   Set the hierarchy for a given FEC bank ID.
 *   The data is set in the DBAL table.
 * \param [in] unit - The unit number.
 * \param [in] bank_id - The bank ID
 * \param [out] hierarchy -
 *   The hierarchy value of the FEC
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_l3_fec_deallocate
 *  * dnx_algo_l3_fec_allocate
 */
shr_error_e algo_l3_fec_hierarchy_stage_map_set(
    int unit,
    uint32 bank_id,
    uint32 hierarchy);

/**
 * \brief
 *   Finds out if the given FEC ID is allocated in the resource.
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC id.
 * \param [out] is_allocated - Indication whether the ID exists.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_create_fec_verify
 *  * dnx_l3_egress_fec_info_get
 *  * dnx_l3_egress_fec_info_delete
 */
shr_error_e dnx_algo_l3_fec_is_allocated(
    int unit,
    int fec_index,
    uint8 *is_allocated);

/**
 * \brief
 *   Receive the type of the Super FEC - it indicates whether
 *   the FEC has protection or not and whether it has statistics or not.
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC id.
 * \param [out] super_fec_type - Type of the super FEC.
 *      The type can be:
 *          * No protection, no statistics
 *          * With protection, no statistics
 *          * No protection, with statistics
 *          * With protection and with statistics
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_fec_entry_add
 */
shr_error_e dnx_algo_l3_fec_super_fec_type_get(
    int unit,
    int fec_index,
    uint32 *super_fec_type);

/**
 * \brief
 *   return the FEC protection state
 * \param [in] unit - The unit number.
 * \param [in] fec_index - The FEC id.
 * \param [out] protected - the protection state of the FEC 0/1.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_algo_l3_fec_protection_state_get(
    int unit,
    int fec_index,
    uint8 *protected);

/**
 * \brief
 *   Receive the MDB clusters which are currently allocated
 *   for the use of a particular FEC table.
 * \param [in] unit - The unit number.
 * \param [in] hierarchy - The FEC index hierarchy
 * \param [out] nof_clusters - number of clusters allocated for the FEC table.
 * \param [out] clusters - An array of clusters in use by the given FEC table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * algo_l3_fec_mdb_table_resources_set
 *  * algo_l3_fec_mdb_table_resources_check
 */
shr_error_e algo_l3_fec_mdb_table_resources_get(
    int unit,
    uint32 hierarchy,
    int *nof_clusters,
    mdb_cluster_alloc_info_t * clusters);

/**
 * \brief
 *   Check if the FEC index belongs to one of the existing MDB ranges.
 *   If it doesn't, then try to find a new one for the FEC range.
 *   This function will also try to allocated the FEC bank unless it is allocated.
 * \param [in] unit - The unit number.
 * \param [in] hierarchy - FEC index hierarchy
 * \param [in] pair_is_allocated - indicate if the sub-resource allocation pair is already allocated.
 * \param [in,out] cluster_update - contains the resource id that should be allocated and returns all the clusters that
 * needs to be updated for this bank and might be also the pair bank to be in use.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_l3_fec_allocate
 */
shr_error_e algo_l3_fec_mdb_table_resources_check(
    int unit,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    uint8 pair_is_allocated,
    algo_dnx_l3_fec_banks_update_info_t * cluster_update);

/**
 * \brief
 *   Verify whether the ECMP profile ID is allocated by checking if there are any references to it.
 * \param [in] unit - The unit number.
 * \param [in] index - The ECMP group profile index.
 * \param [out] is_allocated - Indication whether the profile exists.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_res_dbal_is_allocated
 */
shr_error_e dnx_algo_l3_res_dbal_ecmp_group_profile_is_allocated(
    int unit,
    int index,
    uint8 *is_allocated);

/**
 * \brief
 *   Allocate an ECMP profile ID in the template manager
 *   with unique data result and a particular ID
 * \param [in] unit - The unit number.
 * \param [in] profile_index - The ECMP group profile index.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_res_dbal_alloc
 */
shr_error_e dnx_algo_l3_res_dbal_ecmp_group_profile_allocate(
    int unit,
    int *profile_index);

/**
 * \brief
 *   Free an ECMP profile ID from the template manager.
 * \param [in] unit - The unit number.
 * \param [in] profile_index - The ECMP group profile index.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_res_dbal_free
 */
shr_error_e dnx_algo_l3_res_dbal_ecmp_group_profile_free(
    int unit,
    int profile_index);

/**
 * \brief
 *   Receive the following ECMP profile ID after the current one
 * \param [in] unit - The unit number.
 * \param [in] hierarchy - The unit number.
 * \param [in,out] profile_index - The ECMP group profile index.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_algo_res_dbal_get_next
 */
shr_error_e dnx_algo_l3_res_dbal_ecmp_group_profile_get_next(
    int unit,
    uint32 hierarchy,
    int *profile_index);

/**
 * \brief
 *  Allocate a rif, and save its information in the sw state.
 *
 * \param [in] unit  -  The unit number.
 * \param [in] rif_id  -  The l3 interface structure.
 * \param [in] rif_result_type  -  The result type of RIF EEDB table.
 * \param [in] table_id  -  The dbal table id of the of RIF EEDB table.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_l3_rif_allocate_generic(
    int unit,
    int rif_id,
    dbal_enum_value_result_type_eedb_rif_basic_e rif_result_type,
    dbal_tables_e table_id);

/**
 * \brief
 *  Allocate a rif, and save its information in the sw state.
 *
 * \param [in] unit  -  The unit number.
 * \param [in] rif_id  -  The l3 interface structure.
 * \param [in] rif_result_type  -  The result type of RIF EEDB table.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_l3_rif_allocate(
    int unit,
    int rif_id,
    dbal_enum_value_result_type_eedb_rif_basic_e rif_result_type);

/**
 * \brief
 *  Free a rif, and remove its information from the sw state.
 *
 * \param [in] unit  -  The unit number.
 * \param [in] rif_id  -  The l3 interface structure.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_l3_rif_free(
    int unit,
    int rif_id);

/* ALGO CINT exported API - START */
shr_error_e dnx_cint_algo_l3_rif_allocate_generic(
    int unit,
    int rif_id,
    char *rif_result_type_name,
    char *table_name);

shr_error_e dnx_cint_algo_l3_fec_allocate(
    int unit,
    int *fec_index,
    uint32 flags,
    char *hierarchy_name,
    char *fec_resource_type_name,
    int *bank_id);

/* ALGO CINT exported API - END */

/*
 * Info related to EGR_POINTED
 * {
 */
/**
 * Resource name defines for algo egr_pointed
 */
#define DNX_ALGO_EGR_POINTED_RESOURCE "EGR_POINTED"
/*
 * }
 */
/*
 * }
 */
#endif/*_ALGO_L3_API_INCLUDED__*/
