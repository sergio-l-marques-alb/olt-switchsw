/** \file algo_l3.c
 *
 */
/*
 * $Id: $
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
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/algo/failover/algo_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/swstate/auto_generated/access/ecmp_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_l3_access.h>

/**
 * }
 */
/**
* Defines:
* {
*/
#define L3_MYMAC_TABLE_SIZE             64

/**
 * }
 */
/**
* Macro:
* {
*/
/** Indicate whether the ECMP index is in the extended range of IDs. */
#define ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) (ecmp_index > dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit))
/** Receive the bank ID to which the ECMP index belongs */
#define ALGO_L3_ECMP_BANK_GET(unit, ecmp_index) (ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) ? \
        ((ecmp_index - dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit)) / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)) : \
        (ecmp_index / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)))
/*
 * Return the needed resource name based on the range to which the ECMP ID belongs - basic or extended
 */
#define ALGO_L3_ECMP_GROUP_RESOURCE(unit, ecmp_index) (ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) ? DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP : DNX_ALGO_L3_RES_ECMP_GROUP)

#define ALGO_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, exec)\
    (ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_index) ?\
        DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP_CB.exec :\
        DNX_ALGO_L3_RES_ECMP_GROUP_CB.exec)
/*
 * Returns the logical start address given the bank ID.
 * As the logical address is counted in FECs (not super FECS) the address is the NOF banks FEC capacity multiply the bank ID and each
 * big bank till this bank adds another two FECs.
 */
#define ALGO_L3_GET_START_LOGICAL_ADDRESS_FROM_BANK_ID(unit,bank_id) ((bank_id * dnx_data_l3.fec.fec_stage_map_size_get(unit)) + 2 * ALGO_L3_NOF_BIG_BANKS_BEFORE_CURRENT(bank_id))
/**
 * }
 */
/**
* Structures:
* {
*/
/**
 * }
 */

/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/*******************************************
 * FUNCTIONS related to EGR_POINTED        *
 *******************************************/
/*
 * {
 */
 /**
 * \brief - Initialize EGR_POINTED resources. \n
 *  The function will initialize and create all required EGR_POINTED res-manager.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 *   * Assume Resource Manager is initialized \see sw_state_algo_res_init
 *   * Indirect input: DNX-Data information such as nof EGR_POINTED objects.
 *   * Initialize Res manager pools of EGR_POINTED module see the \n
 *     dnx_egr_pointed_algo_res_init .c file which pools are initialized.
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_egr_pointed_res_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
    /*
     * EGR_POINTED resource management - Allocate
     */
    /*
     * For now, do not use element 0. Reserve it for exceptional cases
     */
    data.first_element = dnx_data_l3.egr_pointed.first_egr_pointed_id_get(unit);
    data.nof_elements = dnx_data_l3.egr_pointed.nof_egr_pointed_ids_get(unit) - 1;
    data.flags = 0;
    sal_strncpy_s(data.name, DNX_ALGO_EGR_POINTED_RESOURCE, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    /*
     * data.desc = "EGR_POINTED - Virtual egress object id which do not use GLEM";
     */
    SHR_IF_ERR_EXIT(algo_l3_db.egr_pointed_alloc.egr_pointed_res_manager.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - DeInitialize EGR_POINTED resources. \n
 *  The function will deinitialize all recources allocated by the
 *  Init function
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_egr_pointed_res_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Add deinit function
     */
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   allocate resource pool for EGR_POINTED
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_algo_l3_egr_pointed_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init alloc manager for virtual egr_pointed IDs
     */
    SHR_IF_ERR_EXIT(dnx_algo_egr_pointed_res_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   deallocate resource pool for EGR_POINTED
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_algo_l3_egr_pointed_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * De-init alloc manager for virtual egr_pointed-port IDs
     */
    SHR_IF_ERR_EXIT(dnx_algo_egr_pointed_res_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/**
 * \brief - Structure used for the allocation of FECs.
 */
typedef struct
{
    /** Type of the super FEC - indicating with/without Protection/Statistics */
    dbal_enum_value_field_fec_resource_type_e super_fec_type_tag;
    /** Id of the sub-resource in the FEC resource (0 to 29) */
    uint32 sub_resource_index;
} algo_dnx_l3_fec_data_t;

/**
 * \brief - number of EXEM VRRP result types.
 * Currently the following types are supported - VRID(legacy VRRP), port based, VSI based
 */
#define ALGO_L3_VRRP_NOF_EXEM_TYPES        (4)
/** The ID of the FEC sub-resouce index which has values in the ECMP and the FEC ranges. */
#define DNX_ALGO_L3_SHARED_RANGE_FEC_BANK  (1)

void
dnx_algo_l3_print_ingress_mymac_prefix_entry_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    bcm_mac_t *mac = (bcm_mac_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_MAC, "MAC addr", *mac, "ingress_mymac_entry_prefix",
                                        NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
}

void
dnx_algo_l3_print_vrrp_entry_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    algo_dnx_l3_vrrp_tcam_key_t *tcam_info = (algo_dnx_l3_vrrp_tcam_key_t *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_MAC, "MAC addr", tcam_info->mac_da,
                                        "Destination mac address", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_MAC, "MAC addr mask", tcam_info->mac_da_mask,
                                        "Destination mac address mask", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "Protocol group", tcam_info->protocol_group,
                                        "Protocol group", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "Protocol group mask",
                                        tcam_info->protocol_group, "Protocol group mask", NULL);

    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
}

void
dnx_algo_l3_print_ecmp_profiles_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_l3_ecmp_profile_t *ecmp_profile = (dnx_l3_ecmp_profile_t *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "Member table ID",
                                        ecmp_profile->members_table_id, "Member base", "0x%02X");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "Group size", ecmp_profile->group_size,
                                        "Size of ECMP group", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "Statistics object ID",
                                        ecmp_profile->statistic_object_id, "Stat object ID", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "Statistics object profile",
                                        ecmp_profile->statistic_object_profile, "Stat object profile", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "Stateful", ecmp_profile->stateful, "Stateful",
                                        NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "Protection", ecmp_profile->protection_flag,
                                        "Protection Flag", NULL);

    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
}

/**
 * \brief
 * Create Resource manager for RIF
 *
 * \param [in] unit - the unit number
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_algo_l3_rif_resource_create(
    int unit)
{

    sw_state_algo_res_create_data_t res_data;
    int init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&res_data, 0, sizeof(res_data));
    res_data.first_element = 1;
    res_data.flags = SW_STATE_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    res_data.nof_elements = dnx_data_l3.rif.nof_rifs_get(unit) - 1;
    res_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_LOCAL_OUTLIF;
    sal_strncpy(res_data.name, DNX_ALGO_L3_LIF_LOCAL_OUTLIF, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_rif_init_info_get(unit, &init_info));

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.create(unit, &res_data, &init_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_rif_allocate_generic(
    int unit,
    int rif_id,
    dbal_enum_value_result_type_eedb_rif_basic_e rif_result_type,
    dbal_tables_e table_id)
{
    lif_mngr_local_outlif_info_t outlif_info;
    dnx_algo_lif_local_outlif_resource_info_t resource_info;

    SHR_FUNC_INIT_VARS(unit);

    outlif_info.dbal_table_id = table_id;
    outlif_info.dbal_result_type = rif_result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_resource_info(unit, &outlif_info, &resource_info));

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.
                    rif_res_manager.allocate_single(unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, &resource_info, &rif_id));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set(unit, rif_id, table_id, rif_result_type));

    /*
     * Update the egress global lif counter, because we're adding this rif to the GLEM.
     */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_count(unit, DNX_ALGO_LIF_EGRESS, 1));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_rif_allocate(
    int unit,
    int rif_id,
    dbal_enum_value_result_type_eedb_rif_basic_e rif_result_type)
{
    lif_mngr_local_outlif_info_t outlif_info;
    dnx_algo_lif_local_outlif_resource_info_t resource_info;

    SHR_FUNC_INIT_VARS(unit);

    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_RIF_BASIC;
    outlif_info.dbal_result_type = rif_result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_resource_info(unit, &outlif_info, &resource_info));

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.
                    rif_res_manager.allocate_single(unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, &resource_info, &rif_id));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set
                    (unit, rif_id, DBAL_TABLE_EEDB_RIF_BASIC, rif_result_type));

    /*
     * Update the egress global lif counter, because we're adding this rif to the GLEM.
     */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_count(unit, DNX_ALGO_LIF_EGRESS, 1));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_rif_free(
    int unit,
    int rif_id)
{
    lif_mngr_local_outlif_info_t outlif_info;
    int rif_size;
    dnx_algo_gpm_rif_hw_resources_t rif_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_rif_intf_to_hw_resources(unit, rif_id, &rif_hw_resources));

    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_RIF_BASIC;
    outlif_info.dbal_result_type = rif_hw_resources.outlif_dbal_result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_nof_outlifs_per_entry(unit, &outlif_info, &rif_size));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(unit, rif_id));

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.free_several(unit, rif_size, rif_id));

    /*
     * Update the egress global lif counter, because we're removing this rif from the GLEM.
     */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_count(unit, DNX_ALGO_LIF_EGRESS, -1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create Template manager for the ingress MyMac prefixes table, with the following properties:
 * - entries : a 38 bits MyMac prefix which can be use by multiple VSIs.
 * - key : the key is an index to a MyMac prefixes table , the first entry is for a "none MyMac"
 *   VSIs so keys values are ranging between 1-MAX.
 * - max references per prefix is the number of VSIs as basically all the VSIs can point to the same MyMac prefix.
 * - The prefix allocation is performed through the ingress interface create/delete interface. Each time an ingress
 *   interface is created the matching MyMac prefix reference counter increases if a matching entry exists, if not a
 *   new entry is created (assuming the table isn't full). In case an ingress interface is deleted, the matching MyMac
 *   prefix reference counter decreases and if it reaches zero the entry is removed from the table.
 * \par DIRECT_INPUT:
 *   \param [in] unit - the unit number
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_l3_ingress_mymac_prefix_entry_template_create(
    int unit)
{
    sw_state_algo_template_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set a template for the MyMac prefix table
     */

    sal_memset(&data, 0, sizeof(data));
    /*
     * The MyMac prefix table contains 64 entries, the first entry is for none MyMac termination VSIs so the
     * template manager will start from the second entry and will use the remaining 63 entries for terminating
     * MyMac prefixes.
     */
    data.data_size = L3_MAC_ADDR_SIZE_IN_BYTES;
    data.first_profile = 1;
    data.nof_profiles = dnx_data_l3.fwd.nof_my_mac_prefixes_get(unit) - 1;
    /*
     * Each VSI can point once to the MyMac prefixes table, so the maximal number of references is the VSIs number.
     */
    data.max_references = dnx_data_l2.vsi.nof_vsis_get(unit);
    sal_strncpy(data.name, DNX_ALGO_L3_INGRESS_MYMAC_PREFIX_TABLE_RESOURCE,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ingress_mymac_prefix_table.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create Template manager for the VRRP (multiple MyMac) TCAM table, with the following properties:
 * - entries : a 48 bits MAC DA and 2 bits of protocol group. Can be used by multiple My Mac (VRRP) rules
 * - key : the key is an index to VRRP TCAM table.
 *   So keys values are ranging between 0-TCAM_TABLE_SIZE.
 * - max references per TCAM entry is the number of VSIs + EXEM table size (depending on the application).
 * - The prefix allocation is performed through the ingress interface create/delete interface. Each time an ingress
 *   interface is created the matching MyMac attributes reference counter increases if a matching entry exists, if not a
 *   new entry is created (assuming the table isn't full). In case an ingress interface is deleted, the matching MyMac
 *   prefix reference counter decreases and if it reaches zero the entry is removed from the table.
 *
 * \param [in] unit - the unit number
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_l3_vrrp_init(
    int unit)
{
    sw_state_algo_template_create_data_t template_data;
    sw_state_algo_res_create_data_t resource_data;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * The VRRP TCAM table is split into 2 tables. The first half is used for VSI based multiple my mac, the second half
     * is used for EXEM based multiple my mac.
     */

    /*
     * Create a template for VSI based VRRP MyMac table
     */
    {
        sal_memset(&template_data, 0, sizeof(sw_state_algo_template_create_data_t));

        template_data.data_size = sizeof(algo_dnx_l3_vrrp_tcam_key_t);
        /*
         * Since EXEM entries in the TCAM are masked out, we need to make sure they won't match for non-exem entries.
         * This will be done by invalidating the "da profiles" LSBs of that entries
         * (these LSBs are used to test whether the TCAM entry is a member of a certain VSI by comparing it to the VRID
         * bitmap inside the VSI).
         */
        {
            template_data.first_profile = ALGO_L3_VRRP_NOF_EXEM_TYPES;
            template_data.nof_profiles = dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit) - ALGO_L3_VRRP_NOF_EXEM_TYPES;
        }
        /*
         * Each VSI can point once to the MyMac prefixes table, so the maximal number of references is the VSIs number.
         */
        template_data.max_references = dnx_data_l2.vsi.nof_vsis_get(unit);

        sal_strncpy(template_data.name, DNX_ALGO_L3_VRRP_VSI_TCAM_TABLE_RESOURCE,
                    SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_vsi_tcam_table.create(unit, &template_data, NULL));

    }

    /*
     * Create a template and resource manager for  EXEM based VRRP MyMac table in case EXEM exists
     */
    {
        int capacity;
        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_SEXEM_1, &capacity));

        if (capacity > 0)
        {

            /** template manager */
            {
                sal_memset(&template_data, 0, sizeof(sw_state_algo_template_create_data_t));

                template_data.data_size = sizeof(algo_dnx_l3_vrrp_tcam_key_t);
                template_data.first_profile = dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit);
                template_data.nof_profiles = dnx_data_l3.vrrp.nof_tcam_entries_get(unit) -
                    dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit);
                template_data.max_references = capacity;

                sal_strncpy(template_data.name, DNX_ALGO_L3_VRRP_EXEM_TCAM_TABLE_RESOURCE,
                            SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

                SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.vrrp_exem_tcam_table.create(unit, &template_data, NULL));
            }

            /** Resource manager */
            {
                sal_memset(&resource_data, 0, sizeof(resource_data));

                resource_data.first_element = 0;
                resource_data.flags = 0;
                resource_data.nof_elements = capacity;
                sal_strncpy(resource_data.name, DNX_ALGO_L3_VRRP_EXEM_STATION_ID_RESOURCE,
                            SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

                SHR_IF_ERR_EXIT(algo_l3_db.vrrp_exem_l2_station_id_alloc.res_bitmap.create(unit, &resource_data, NULL));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Initialize all the resource managers that are needed for the ECMP  which are:
 *  Profiles management for each hierarchy (each hierarchy have 1k profiles available).
 *  consistent hashing manager - to manage the consisting hashing tables.
 *  SW state array of linked list - to track all the ECMP profiles that uses consistent members tables for quick
 *                 table address update if needed.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_algo_l3_ecmp_init(
    int unit)
{
    int hierarchy_iter;
    int ecmp_index = 0;
    int idx;
    sw_state_algo_template_create_data_t data;
    sw_state_algo_res_create_data_t create_data;
    consistent_hashing_cbs_t ecmp_const_hash_cb_funcs;
    sw_state_ll_init_info_t init_info;
    uint32 consistent_hashing_manager_handle;
    uint32 nof_ecmp_banks;

    char *ecmp_profiles_handle_name;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Template mangers allocation for ECMP profiles.
     *
     * Each ECMP hierarchy have a different bank of ECMP profile to manage (1k each) so each hierarchy profile
     * resources have its own template manager instance.
     */
    for (hierarchy_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
         hierarchy_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); hierarchy_iter++)
    {
        sal_memset(&data, 0, sizeof(data));

        data.data_size = sizeof(dnx_l3_ecmp_profile_t);
        data.first_profile = 0;
        data.nof_profiles = dnx_data_l3.ecmp.nof_group_profiles_per_hierarchy_get(unit);
        data.max_references = dnx_data_l3.ecmp.nof_ecmp_get(unit);

        SHR_IF_ERR_EXIT(dnx_l3_ecmp_hierarchy_profile_resource_get(unit, hierarchy_iter, &ecmp_profiles_handle_name));

        sal_strncpy(data.name, ecmp_profiles_handle_name, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.create(unit, hierarchy_iter, &data, NULL));

    }

    /*
     * Init the ECMP SW state structure
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.init(unit));

    /*
     * consistent hashing manager Initialization
     */
    ecmp_const_hash_cb_funcs.nof_resources_per_profile_type_get = &ecmp_chm_get_nof_resources_from_table_size;
    ecmp_const_hash_cb_funcs.profile_type_per_nof_resources_get = &ecmp_chm_get_table_size_from_nof_resources;
    ecmp_const_hash_cb_funcs.calendar_set = &ecmp_chm_members_table_set;
    ecmp_const_hash_cb_funcs.calendar_get = &ecmp_chm_members_table_get;
    ecmp_const_hash_cb_funcs.profile_move = &ecmp_chm_update_members_table_offset;
    ecmp_const_hash_cb_funcs.profile_assign = &ecmp_chm_update_ecmp_profile_with_members_table_offset;
    ecmp_const_hash_cb_funcs.profile_offset_get = &ecmp_chm_members_table_offset_get;
    ecmp_const_hash_cb_funcs.calendar_entries_in_profile_get = &ecmp_chm_entries_in_profile_get_cb;

    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_create
                    (unit,
                     L3_ECMP_TOTAL_NOF_RESOURCES(unit),
                     DNX_ALGO_L3_ECMP_CONSISTENT_MANAGER_HANDLE, &ecmp_const_hash_cb_funcs,
                     &consistent_hashing_manager_handle));

    SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.set(unit, consistent_hashing_manager_handle));

    /*
     * Init the multihead linked list (SW state)
     */
    sal_memset(&init_info, 0, sizeof(init_info));
    init_info.max_nof_elements =
        dnx_data_l3.ecmp.nof_group_profiles_per_hierarchy_get(unit) *
        dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

    init_info.expected_nof_elements =
        dnx_data_l3.ecmp.nof_group_profiles_per_hierarchy_get(unit) *
        dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

    init_info.nof_heads = L3_ECMP_TOTAL_NOF_RESOURCES(unit);

    SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.create_empty(unit, &init_info));

    /*
     * ALLOC MNGR - ECMP GROUP and ECMP EXTENDED GROUP
     */
    sal_memset(&create_data, 0, sizeof(create_data));
    create_data.first_element = 0;
    create_data.flags = SW_STATE_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    create_data.nof_elements = dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit);
    create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_ECMP;
    sal_strncpy(create_data.name, DNX_ALGO_L3_RES_ECMP_GROUP, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_res_manager.create(unit, &create_data, NULL));

    sal_memset(&create_data, 0, sizeof(create_data));
    create_data.first_element = dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit);
    create_data.flags = SW_STATE_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    create_data.nof_elements = dnx_data_l3.ecmp.nof_ecmp_get(unit) - dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit);
    create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_ECMP;
    sal_strncpy(create_data.name, DNX_ALGO_L3_RES_ECMP_EXTENDED_GROUP, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_extended_res_manager.create(unit, &create_data, NULL));

    /** Set the hierarchy value of all banks to default - 1st hierarchy. */
    nof_ecmp_banks =
        dnx_data_l3.ecmp.nof_ecmp_banks_basic_get(unit) + dnx_data_l3.ecmp.nof_ecmp_banks_extended_get(unit);
    for (idx = 0; idx < nof_ecmp_banks; idx++)
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_hierarchy.set(unit, idx, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1));
    }
    /** Allocate ECMP ID = 0 at initialization so that it cannot be allocated by user */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_res_manager.allocate_single(unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID,
                                                                  DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1,
                                                                  &ecmp_index));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_l3_nof_fec_banks_get(
    int unit,
    uint32 *fec_banks)
{
    uint32 max_fec_id_value;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id_value));

    *fec_banks = ((max_fec_id_value + 1) / dnx_data_l3.fec.fec_stage_map_size_get(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Initialize all the resource managers that are needed for the ECMP  which are:
 *  Profiles management for each hierarchy (each hierarchy have 1k profiles available).
 *  consistent hashing manager - to manage the consisting hashing tables.
 *  SW state array of linked list - to track all the ECMP profiles that uses consistent members tables for quick
 *                 table address update if needed.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_algo_l3_fec_init(
    int unit)
{
    sw_state_algo_res_create_data_t create_data;
    uint32 sub_resource_id;
    uint32 nof_banks;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_nof_fec_banks_get(unit, &nof_banks));

    if (nof_banks > 0)
    {

        sal_memset(&create_data, 0, sizeof(sw_state_algo_res_create_data_t));
        create_data.first_element = 0;
        create_data.nof_resource_pool_indexes = nof_banks;
        create_data.flags = SW_STATE_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM | SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
        create_data.nof_elements = dnx_data_l3.fec.max_fec_stage_map_size_get(unit);
        create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_FEC;
        sal_strncpy(create_data.name, DNX_ALGO_L3_RES_FEC, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.alloc
                        (unit, create_data.nof_resource_pool_indexes));
        for (sub_resource_id = 0; sub_resource_id < create_data.nof_resource_pool_indexes; sub_resource_id++)
        {
            SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.create
                            (unit, sub_resource_id, &create_data, NULL));
        }

        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_hierarchy.fec_hierarchy_map.alloc(unit));

        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.fec_resource_type.alloc(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Deinit all the algo component that were allocated in the init (dnx_algo_l3_ecmp_init)
 *  Profiles management for each hierarchy (each hierarchy have 1k profiles available).
 *  consistent hashing manager - to manage the consisting hashing tables.
 *  SW state array of linked list - to track all the ECMP profiles that uses consistent members tables for quick
 *                 table address update if needed.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_algo_l3_ecmp_deinit(
    int unit)
{
    uint32 consistent_hashing_manager_handle;
    uint8 is_init = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deinit of the consistent hashing manager.
     */
    SHR_IF_ERR_CONT(ecmp_db_info.is_init(unit, &is_init));

    if (is_init == TRUE)
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));

        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_destroy(unit, consistent_hashing_manager_handle));
    }

    /*
     * sw state module deinit is done automatically at device deinit
     */

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_init(
    int unit)
{
    uint8 is_init;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init sw state
     */
    SHR_IF_ERR_EXIT(algo_l3_db.is_init(unit, &is_init));
    if (!is_init)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.init(unit));
    }

    /*
     * Initialize the source address table template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_source_address_table_init(unit));

    /*
     * Initialize ingress mymac prefix template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ingress_mymac_prefix_entry_template_create(unit));

    /*
     * Initialize RIF resource manager.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_rif_resource_create(unit));
    /*
     * Initialize VRRP (multiple MyMac) template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_vrrp_init(unit));

    /*
     * Initialize ECMP template tables resources.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_init(unit));

    /** Initialise the FEC resource */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_init(unit));

    /** Initialise the EGR_POINTED resource */
    SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_init(unit));

    /** Initialise the EGR_POINTED tables */
    SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_tables_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit
     */

    /*
     * Resource and template manager don't require deinitialization per instance.
     */

    /*
     * Deinitialize the source address table allocation algorithm.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_source_address_table_deinit(unit));
    /*
     * Deinitialize ECMP resources.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_deinit(unit));
    /*
     * Deinitialize EGR_POINTED resources.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_deinit(unit));
    /*
     * Deinitialize EGR_POINTED tables.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_tables_deinit(unit));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_l3_ecmp_bitmap_create(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap,
    int core_id,
    int sub_resource_index,
    sw_state_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    int grain_size;
    uint8 max_tag_value;
    sw_state_resource_tag_bitmap_create_info_t res_tag_bitmap_create_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(create_data, _SHR_E_PARAM, "create_data");

    /*
     * set grain size to be equal to the minimum number of ECMPs that are in the same hierarchy
     */
    grain_size = dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);

    max_tag_value = dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(sw_state_resource_tag_bitmap_create_info_t));
    res_tag_bitmap_create_info.low_id = create_data->first_element;
    res_tag_bitmap_create_info.count = create_data->nof_elements;
    res_tag_bitmap_create_info.grain_size = grain_size;
    res_tag_bitmap_create_info.max_tag_value = max_tag_value;
    res_tag_bitmap_create_info.flags = SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_create
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_create_info, nof_elements, alloc_flags));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_l3_ecmp_bitmap_allocate(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    uint32 internal_flags;
    uint32 stage;
    uint32 nof_elements;
    uint32 current_hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    uint32 tag;
    uint32 ecmp_bank_size;
    uint32 first_elem_in_bank = 0;
    uint32 nof_banks;
    uint32 nof_banks_basic_mode;
    uint32 nof_banks_ext_mode;
    uint8 with_id;
    int ecmp_bank_valid = -1;
    const int bank_invalid = -1;
    int idx;
    sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Minimum number of consecutive ECMPs in the same hierarchy which belong to a single bank */
    ecmp_bank_size = dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);
    /** Number of ECMP banks in basic range (16). Found by dividing the nof groups in basic range by size of bank */
    nof_banks_basic_mode = dnx_data_l3.ecmp.nof_ecmp_banks_basic_get(unit);
    /** Number of ECMP banks in extended range. Found by dividing the nof groups in extended range by size of bank */
    nof_banks_ext_mode = dnx_data_l3.ecmp.nof_ecmp_banks_extended_get(unit);

    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");
    if (extra_arguments != NULL)
    {
        tag = *((uint32 *) extra_arguments);
    }
    else
    {
        tag = 0;
    }

    with_id = _SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID);

    /*
     * Different actions need to be carried out depending on the absence/presence of the WITH_ID flag.
     * If allocating without ID, an appropriate bank for the allocation of the ECMP ID needs to be found.
     */
    if (with_id != 0)
    {
        /*
         * Define the bank ID(stage) for the given range.
         * If in extended range, need to subtract the number of IDs in the basic range in order to receive the actual value.
         */
        stage = ALGO_L3_ECMP_BANK_GET(unit, *element);
        first_elem_in_bank = ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element) ?
            ((nof_banks_basic_mode + stage) * ecmp_bank_size) : (stage * ecmp_bank_size);

        /*
         * Receive the number of allocated elements in the bank.
         */
        SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_nof_used_elements_in_grain_get
                        (unit, module_id, res_tag_bitmap, stage, &nof_elements));
        /** Make sure that in case we are in the bank of ECMP ID 0, the number of elements will be correct */
        if (stage == 0 && !ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element))
        {
            nof_elements--;
        }
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, *element, &current_hierarchy));

        /*
         * If the hierarchy indicates NO_HIERARCHY that means that the bank is currently unavailable.
         */
        if (current_hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "The bank %d to which the ECMP ID %d belongs to is unavailable \n", stage,
                         *element);
        }

        /*
         * If the bank is already full, we cannot allocate any more elements in it.
         */
        if (nof_elements == dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "The bank %d to which the ECMP ID %d belongs to is full \n", stage, *element);
        }
    }
    else
    {
        /** Set the stage to an invalid bank ID (the total number of banks) - it will be redefined later. */
        stage = dnx_data_l3.ecmp.nof_ecmp_get(unit) / ecmp_bank_size;
        /** Define the nof_banks based on the range the ID belongs to - basic or extended */
        nof_banks = ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element) ? nof_banks_ext_mode : nof_banks_basic_mode;
        /*
         * Iterate over all banks for the given range (basic or extended) to find out an appropriate one for allocation.
         */
        for (idx = 0; idx < nof_banks; idx++)
        {
            /** Define the first element in the bank (0, 2048, 4096, 6144, etc.) based on the current iteration */
            first_elem_in_bank = ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element) ?
                ((nof_banks_basic_mode + idx) * ecmp_bank_size) : (idx * ecmp_bank_size);
            /*
             * Receive the number of allocated elements in the bank and the hierarchy defined for this bank.
             */
            SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_nof_used_elements_in_grain_get
                            (unit, module_id, res_tag_bitmap, idx, &nof_elements));
            /** Make sure that in case we are in the bank of ECMP ID 0, the number of elements will be correct */
            if (idx == 0 && !ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element))
            {
                nof_elements--;
            }
            SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, first_elem_in_bank, &current_hierarchy));
            /** Skip the bank if its hierarchy is marked as NO_HIERARCHY*/
            if (current_hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY)
            {
                continue;
            }
            /*
             * If the number of used elements in the bank is 0 mark it once as a valid ID for allocation.
             * This ID will be used in case no other bank is found that is partially taken and is with the same hier.
             */
            if (nof_elements == 0 && ecmp_bank_valid == -1)
            {
                ecmp_bank_valid = idx;
            }

            /*
             * If the hierarchy of this bank matches the current element hierarchy and is partially taken,
             * this bank is taken with preference to others.
             */
            if ((current_hierarchy == tag) && (nof_elements > 0)
                && (nof_elements < dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)))
            {
                stage = idx;
                break;
            }
        }

        /** If a valid partially taken bank was found, update the first_elem of the bank and the current element */
        if (stage != (dnx_data_l3.ecmp.nof_ecmp_get(unit) / ecmp_bank_size))
        {
            first_elem_in_bank = stage * ecmp_bank_size;
            *element = first_elem_in_bank;
        }
        /** Partially taken bank with the same hierarchy was not found but a free bank was found */
        else if (ecmp_bank_valid != bank_invalid)
        {
            nof_elements = 0;
            stage = ecmp_bank_valid;
            first_elem_in_bank = stage * ecmp_bank_size;
            *element = first_elem_in_bank;
        }
        /** If no free or valid bank was found, an error is returned. */
        else
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "No valid bank was found for allocating and ECMP ID without ID\n");
        }
    }

    /*
     * Translate the allocation flags.
     * The input flags are of type SW_STATE_ALGO_RES_ALLOCATE_*, but the sw_state_res_tag_bitmap use a
     * different set of flags.
     */
    internal_flags = SW_STATE_RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG;
    internal_flags |= (with_id) ? SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID : 0;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(sw_state_resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_alloc_info.flags = internal_flags;
    res_tag_bitmap_alloc_info.count = 1;
    res_tag_bitmap_alloc_info.tag = tag;

    /*
     * If adding the first element in a stage we need to carry out additional configuration in cases:which is part of the extended range
     *      * ECMP ID is in the extended range;
     *      * ECMP ID is in the first bank of the basic range resource.
     */
    if (nof_elements == 0)
    {
        /*
         * In case the ECMP ID is in the first bank of the basic range, the ECMP ID 0 that is allocated on init
         * may be in an incorrect hierarchy. We need to update the hierarchy of element 0.
         */
        if (stage == 0 && !ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element))
        {
            int ecmp_init_id = 0;
            sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;
            uint32 internal_flags_init = SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
            sal_memset(&res_tag_bitmap_free_info, 0, sizeof(sw_state_resource_tag_bitmap_alloc_info_t));
            res_tag_bitmap_free_info.count = 1;
            SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_free
                            (unit, module_id, res_tag_bitmap, res_tag_bitmap_free_info, ecmp_init_id));
            res_tag_bitmap_alloc_info.flags = internal_flags_init;
            SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_alloc
                            (unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, &ecmp_init_id));
        }
        /*
         * In case the ECMP ID is in the extended range. which uses the same memory as the extended range of ECMP
         * groups, we need to specify to the failover that we reserve these banks so that they are not overwritten.
         */
        if (ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, *element))
        {
            SHR_IF_ERR_EXIT(dnx_algo_failover_fec_bank_for_ecmp_extended_allocate(unit, stage));
        }

        /** Set the hierarchy for this bank in case we are allocating the first element in it. */
        SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_set
                        (unit, module_id, res_tag_bitmap, tag, FALSE, first_elem_in_bank, ecmp_bank_size));
    }

    res_tag_bitmap_alloc_info.flags = internal_flags;

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_alloc
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));

    /*
     * Update sw state ecmp_nof_used_elements_per_grain variable.
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_nof_used_elements_per_grain.inc(unit,
                                                                      (*element /
                                                                       dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)),
                                                                      res_tag_bitmap_alloc_info.count));
    SHR_IF_ERR_EXIT(ecmp_db_info.
                    ecmp_hierarchy.set(unit, (*element / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)), tag));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_bitmap_free(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int element)
{
    uint32 stage;
    uint32 nof_elements;
    sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;
    uint32 tag;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Define the bank ID for the given range.
     * If in extended range, need to subtract the number of IDs in the basic range in order to receive the actual value.
     */
    stage = ALGO_L3_ECMP_BANK_GET(unit, element);

    /**
     * Call the main function to clear the element from the bitmap
     */
    sal_memset(&res_tag_bitmap_free_info, 0, sizeof(sw_state_resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_free_info.count = 1;

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_free
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_free_info, element));

    /*
     * Update sw state ecmp_nof_used_elements_per_grain variable.
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_nof_used_elements_per_grain.dec(unit,
                                                                      (element /
                                                                       dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit)),
                                                                      res_tag_bitmap_free_info.count));
    /**
     * Receive the number of allocated elements for this stage.
     */
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_nof_used_elements_in_grain_get
                    (unit, module_id, res_tag_bitmap, stage, &nof_elements));

    /*
     * If the ecmp group belongs to the extended range of entries (32k to 40k) and is the last one in the bank,
     * then we need to free the shared bank of memory between the failover and the ECMP.
     */
    if (ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, element) && nof_elements == 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_failover_fec_bank_for_ecmp_extended_free(unit, stage));
    }

    /*
     * If we are deleting an element from the first bank in the basic range,
     * we need to verify if the bank doesn't have only ID 0 allocated in it.
     */
    if (!ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, element) && (stage == 0))
    {
        nof_elements--;
        /*
         * If only ID 0 is allocated in bank ID 0,
         * then we need to reset the hierarchy of the first bank to its initial value.
         */
        if (nof_elements == 0)
        {
            int ecmp_init_id = 0;
            uint32 internal_flags;
            sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
            tag = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
            SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_free(unit,
                                                              module_id, res_tag_bitmap, res_tag_bitmap_free_info,
                                                              ecmp_init_id));
            internal_flags = SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
            sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(sw_state_resource_tag_bitmap_alloc_info_t));
            res_tag_bitmap_alloc_info.flags = internal_flags;
            res_tag_bitmap_alloc_info.count = 1;
            res_tag_bitmap_alloc_info.tag = tag;
            SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_alloc
                            (unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, &ecmp_init_id));
        }
    }
    /** Set the hierarchy value to default in case deleting the last element in the bank. */
    if (nof_elements == 0)
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_hierarchy.set(unit, element / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit),
                                                        DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_hierarchy_get(
    int unit,
    int ecmp_index,
    uint32 *hierarchy)
{
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Receive the value of the bitmap handle (algo_instance_id).
     * It is saved in the SW state and is received based on whether the ECMP ID is in basic or extended range.
     * If the ECMP index is in extended mode, then we need to update the ecmp_index
     * to be in the 0 - 8k range and not 32k - 40k range.
     */

    bank_id = ecmp_index / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_hierarchy.get(unit, bank_id, hierarchy));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_group_is_alone_in_bank(
    int unit,
    int ecmp_id,
    int *bank_id,
    int *is_alone_in_bank)
{
    uint32 nof_elements = 0;

    SHR_FUNC_INIT_VARS(unit);

    *is_alone_in_bank = 0;

    /*
     * Define the bank ID for the given range.
     * If in extended range, need to subtract the number of IDs in the basic range in order to receive the actual value.
     *
     * Receive the value of the bitmap handle (algo_instance_id).
     * It is saved in the SW state and is received based on whether the ECMP ID is in basic or extended range.
     */

    *bank_id = ecmp_id / dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);
    /**
     * Receive the current number of elements for the given bank
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_nof_used_elements_per_grain.get(unit, *bank_id, &nof_elements));
    if (!ALGO_L3_ECMP_INDEX_IN_EXTENDED_RANGE(unit, ecmp_id) && (*bank_id == 0))
    {
        nof_elements--;
    }
    /**
     * If there is only a single element in the bank, return indication that the element is first in bank
     */
    if (nof_elements == 1)
    {
        *is_alone_in_bank = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_is_allocated(
    int unit,
    int ecmp_index,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Receive indication whether the group is allocated.
     * Based on which range the ECMP group ID belongs to, a different resource will be accessed.
     */
    SHR_IF_ERR_EXIT(ALGO_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, is_allocated(unit, ecmp_index, is_allocated)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_allocate(
    int unit,
    int *ecmp_index,
    uint32 ecmp_flags,
    uint32 ecmp_group_flags,
    uint32 hierarchy)
{
    uint32 alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    alloc_flags = (_SHR_IS_FLAG_SET(ecmp_flags, BCM_L3_WITH_ID)) ? SW_STATE_ALGO_RES_ALLOCATE_WITH_ID : 0;
    /*
     * Allocate the group in the needed resource.
     * The resource is selected based on which range the ECMP ID belongs to.
     */
    if (!_SHR_IS_FLAG_SET(ecmp_flags, BCM_L3_WITH_ID))
    {
        if (_SHR_IS_FLAG_SET(ecmp_group_flags, BCM_L3_ECMP_EXTENDED))
        {
            *ecmp_index = dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit);
            SHR_IF_ERR_EXIT(ecmp_db_info.
                            ecmp_extended_res_manager.allocate_single(unit, alloc_flags, &hierarchy, ecmp_index));
        }
        else
        {
            *ecmp_index = 0;
            SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_res_manager.allocate_single(unit, alloc_flags, &hierarchy, ecmp_index));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(ALGO_L3_ECMP_GROUP_RESOURCE_CB
                        (unit, *ecmp_index, allocate_single(unit, alloc_flags, &hierarchy, ecmp_index)));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_deallocate(
    int unit,
    int ecmp_index)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deallocate the ECMP group from the needed resource.
     * The resource is selected based on which range the ECMP ID belongs to.
     */
    SHR_IF_ERR_EXIT(ALGO_L3_ECMP_GROUP_RESOURCE_CB(unit, ecmp_index, free_single(unit, ecmp_index)));
exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_extended_bank_for_failover_allocate(
    int unit,
    int bank_id)
{
    uint32 ecmp_bank_size;
    uint32 nof_elements;

    SHR_FUNC_INIT_VARS(unit);

    ecmp_bank_size = dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);

    /*
     *  Modify the bank ID so that the valid banks for extended range will be from 16 to 19
     */
    bank_id = bank_id + (dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit) / ecmp_bank_size);

    /** Receive number of elements allocated in the Extended range resource */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_nof_used_elements_per_grain.get(unit, bank_id, &nof_elements));
    /** If the bank is not empty regarding ECMP, FEC allocation cannot reserve this bank. */
    if (nof_elements != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Cannot reserve shared bank %d for the use of FEC protection as it not empty\n",
                     bank_id);
    }
    /*
     *  marked with NO_HIERARCHY indicating unavailable bank for allocation.
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_hierarchy.set(unit, bank_id, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_ecmp_extended_bank_for_failover_destroy(
    int unit,
    int bank_id)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Modify the bank ID so that the valid banks for extended range will be from 16 to 19
     */
    bank_id = bank_id + (dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit) /
                         dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit));

    /** Mark the bank as available for allocation. */
    SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_hierarchy.set(unit, bank_id, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information
 */
shr_error_e
algo_l3_fec_hierarchy_stage_map_get(
    int unit,
    uint32 fec_index,
    uint32 *hierarchy_p)
{
    uint32 bank_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    bank_id = DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_hierarchy.fec_hierarchy_map.get(unit, bank_id, hierarchy_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Refer to the algo_l3_fec.h file for more information
 */
shr_error_e
dnx_algo_l3_fec_bitmap_create(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap,
    int core_id,
    int sub_resource_index,
    sw_state_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    int grain_size = 2;
    sw_state_resource_tag_bitmap_create_info_t res_tag_bitmap_create_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(create_data, _SHR_E_PARAM, "create_data");

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(sw_state_resource_tag_bitmap_create_info_t));
    res_tag_bitmap_create_info.low_id = create_data->first_element;
    res_tag_bitmap_create_info.count = create_data->nof_elements;
    res_tag_bitmap_create_info.grain_size = grain_size;
    res_tag_bitmap_create_info.max_tag_value = dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);
    res_tag_bitmap_create_info.flags = SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_create
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_create_info, nof_elements, alloc_flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the algo_l3_fec.h file for more information
 */
shr_error_e
dnx_algo_l3_fec_bitmap_allocate(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    uint32 internal_flags = 0;
    algo_dnx_l3_fec_data_t fec_alloc_data;
    uint32 grain_size = 2;
    uint8 with_id;
    uint8 alloc_simulation;
    sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");
    SHR_NULL_CHECK(extra_arguments, _SHR_E_PARAM, "extra_arguments");

    fec_alloc_data = *((algo_dnx_l3_fec_data_t *) extra_arguments);

    /*
     * Translate the allocation flags.
     * The input flags are of type DNX_ALGO_RES_ALLOCATE_*, but the sw_state_resource_tag_bitmap_alloc_info_t uses a
     * different set of flags.
     */
    with_id = _SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID);
    alloc_simulation = _SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_RES_ALLOCATE_SIMULATION);
    internal_flags |= (with_id) ? SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID : 0;
    internal_flags |= (alloc_simulation) ? SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY : 0;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(sw_state_resource_tag_bitmap_alloc_info_t));
    /*
     * If allocating in bank ID 1 and without ID,
     * then the ALLOC_IN_RANGE flag needs to be set in order to be able to allocate without ID
     * in the range between the end of the ECMP range and the end of the 2nd FEC bank (40960 - 52430).
     */
    if (!with_id && (fec_alloc_data.sub_resource_index == DNX_ALGO_L3_SHARED_RANGE_FEC_BANK))
    {
        /** range_start is the end of the ECMP range */
        res_tag_bitmap_alloc_info.range_start =
            dnx_data_l3.ecmp.total_nof_ecmp_get(unit) - dnx_data_l3.fec.max_fec_stage_map_size_get(unit);
        /** range_end is the start of the 3rd bank (bank ID 2)*/
        res_tag_bitmap_alloc_info.range_end = dnx_data_l3.fec.fec_stage_map_size_get(unit);
        internal_flags |= SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
    }

    res_tag_bitmap_alloc_info.count = 1;
    if (fec_alloc_data.super_fec_type_tag == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT
        || fec_alloc_data.super_fec_type_tag == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT)
    {
        res_tag_bitmap_alloc_info.count = grain_size;
        res_tag_bitmap_alloc_info.align = grain_size;
        /** Flag guarantees that in case of protection, the first entry in the pair will be even. */
        internal_flags |= SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_ALIGN;
    }

    res_tag_bitmap_alloc_info.flags = internal_flags;
    res_tag_bitmap_alloc_info.tag = fec_alloc_data.super_fec_type_tag;

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_alloc
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the algo_l3_fec.h file for more information
 */
shr_error_e
dnx_algo_l3_fec_bitmap_free(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int element)
{
    sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;
    uint32 super_fec_type;
    uint32 internal_flags = 0;
    uint32 grain_size = 2;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Call the main function to clear the element from the bitmap
     */
    sal_memset(&res_tag_bitmap_free_info, 0, sizeof(sw_state_resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_free_info.count = 1;
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap, element, &super_fec_type));

    /*
     * If we are freeing a FEC with protection, then we free the pair it belongs to
     * instead of the two elements separately.
     */
    if (super_fec_type == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT
        || super_fec_type == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT)
    {
        res_tag_bitmap_free_info.count = grain_size;
        res_tag_bitmap_free_info.align = grain_size;
        /** Flag guarantees that in case of protection, the first entry in the pair will be even. */
        internal_flags |= SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_ALIGN;
    }
    res_tag_bitmap_free_info.flags = internal_flags;
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_free
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_free_info, element));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Initialize the algo_dnx_l3_fec_banks_update_info_t with default values and only update the requested sub-resource id
 *  which is the only field which used as an input when using this structure
 * \param [in] subresource_id -
 *       the sub-resource ID which should be allocated using this structure
 * \param [out] cluster_to_update -
 *       Return a resource allocation information structure with default values.
 * \return
 *   None
 */
void
dnx_algo_l3_init_fec_cluster_struct(
    int subresource_id,
    algo_dnx_l3_fec_banks_update_info_t * cluster_to_update)
{
    algo_l3_fec_banks_group_type_e bank_group_type;

    cluster_to_update->resource_was_found = FALSE;

    for (bank_group_type = 0; bank_group_type < ALGOL_L3_NOF_FEC_BANKS_GROUP_TYPES; bank_group_type++)
    {
        cluster_to_update->clusters_info[bank_group_type].cluster_to_update[0].logical_start_address =
            ALGO_L3_INVALID_LOGICAL_ADDRESS;
        cluster_to_update->clusters_info[bank_group_type].cluster_to_update[1].logical_start_address =
            ALGO_L3_INVALID_LOGICAL_ADDRESS;
        cluster_to_update->clusters_info[bank_group_type].bank_id[0] = -1;
        cluster_to_update->clusters_info[bank_group_type].bank_id[1] = -1;
    }

    cluster_to_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[0] = subresource_id;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_fec_allocate(
    int unit,
    int *fec_index,
    uint32 flags,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    dbal_enum_value_field_fec_resource_type_e fec_resource_type,
    int *bank_id)
{
    uint32 alloc_flags = 0;
    /** Size in number of entries of the current FEC bank */
    uint32 fec_bank_size = dnx_data_l3.fec.fec_stage_map_size_get(unit);
    uint32 sub_resource_id;
    uint32 current_hierarchy;
    uint32 subr_id;
    uint32 sub_resource_pair_id;
    uint32 pair_hierarchy;
    /** total number of sub-resources in the FEC resource */
    uint32 nof_sub_resources;
    int subres_found = -1;
    /** number of free elements per given bank */
    int nof_free_elements = 0;
    int rv = BCM_E_NONE;
    /** number of big-sized banks that exist before the current bank_id */
    int nof_big_banks_passed = 0;
    /** data for a cluster which will be updated */
    algo_dnx_l3_fec_banks_update_info_t cluster_to_update;
    algo_dnx_l3_fec_data_t fec_alloc_data;

    SHR_FUNC_INIT_VARS(unit);

    /** Use field logical_start_address as indication whether the cluster is valid - if it equals -1, then it is not */
    SHR_IF_ERR_EXIT(dnx_algo_l3_nof_fec_banks_get(unit, &nof_sub_resources));

    alloc_flags = (_SHR_IS_FLAG_SET(flags, DNX_ALGO_L3_FEC_WITH_ID)) ? SW_STATE_ALGO_RES_ALLOCATE_WITH_ID : 0;

    dnx_algo_l3_init_fec_cluster_struct(-1, &cluster_to_update);

    /*
     * Based on the WITH_ID flag different set of actions are executed:
     * If allocating without ID then we need to find an empty sub-resource or one configured for the current hierarchy.
     * If allocating WITH_ID, check that there are empty slots in the sub-resource and that the hierarchies match.
     */
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_L3_FEC_WITH_ID))
    {

        /*
         * Receive the ID of the sub-resource to which the FEC belongs -
         * divide the physical FEC to the number of physical FECs per sub-resource.
         */
        sub_resource_id =
            DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, *fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);

        /*
         * check if current element hierarchy matches already defined hierarchy.
         * If it doesn't then an error will be returned.
         */
        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_hierarchy.
                        fec_hierarchy_map.get(unit, sub_resource_id, &current_hierarchy));

        if (current_hierarchy != DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY && hierarchy != current_hierarchy)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Expected hierarchy for bank %d is %d, instead received %d\n",
                         sub_resource_id, current_hierarchy + 1, hierarchy + 1);
        }

        /*
         * Receive the number of bigger banks that exist before the current one.
         * Bank IDs that are divided to 5 without residue are considered to be big-sized.
         */
        nof_big_banks_passed = ALGO_L3_NOF_BIG_BANKS_BEFORE_CURRENT(sub_resource_id);

        /*
         * FEC index is received in the range of 0 - 768K. It needs to be mapped to an ID in the sub-resource.
         * This is calculated by subtracting the number of entries from big banks and small banks.
         */
        *fec_index =
            *fec_index - (nof_big_banks_passed * dnx_data_l3.fec.max_fec_stage_map_size_get(unit) +
                          (sub_resource_id - nof_big_banks_passed) * dnx_data_l3.fec.fec_stage_map_size_get(unit));

        /*
         * If this sub-resource don't have a hierarchy associated with that it means that a cluster and a hierarchy should be allocated and set.
         */
        if (current_hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY)
        {
            sub_resource_pair_id = ALGO_L3_GET_FEC_SUB_RESOURCE_PAIR(sub_resource_id);

            SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_hierarchy.
                            fec_hierarchy_map.get(unit, sub_resource_pair_id, &pair_hierarchy));

            cluster_to_update.clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[0] = sub_resource_id;

            /** Verify FEC ID according to MDB table setup if allocating first element in sub-resource. */
            SHR_IF_ERR_EXIT(algo_l3_fec_mdb_table_resources_check(unit,
                                                                  hierarchy,
                                                                  (pair_hierarchy !=
                                                                   DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY) ?
                                                                  TRUE : FALSE, &cluster_to_update));
            if (cluster_to_update.resource_was_found == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "The FEC ID %d does not belong to an available MDB range\n", *fec_index);
            }

        }

    }
    else
    {
        /*
         * indicate if a pair of sub-resources were found
         */
        uint32 pair_subres_found = FALSE;

        algo_dnx_l3_fec_banks_update_info_t new_cluster_update;

        dnx_algo_l3_init_fec_cluster_struct(-1, &new_cluster_update);
        /*
         * Find a valid sub-resource ID to which to add the new FEC. Compare free slots and defined hierarchy.
         */

        for (subr_id = dnx_data_l3.fec.first_bank_without_id_alloc_get(unit); subr_id < nof_sub_resources; subr_id++)
        {
            fec_bank_size =
                ALGO_L3_FEC_BANK_IS_BIG(subr_id) ? dnx_data_l3.fec.max_fec_stage_map_size_get(unit) :
                dnx_data_l3.fec.fec_stage_map_size_get(unit);
            SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_hierarchy.
                            fec_hierarchy_map.get(unit, subr_id, &current_hierarchy));
            SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                            fec_res_manager.nof_free_elements_get(unit, subr_id, &nof_free_elements));
            /** Limit the size of resource FEC bank with 2 if bank is smaller */
            if (!ALGO_L3_FEC_BANK_IS_BIG(subr_id))
            {
                nof_free_elements -=
                    (dnx_data_l3.fec.max_fec_stage_map_size_get(unit) - dnx_data_l3.fec.fec_stage_map_size_get(unit));
            }
            /*
             * If the sub-resource have the same hierarchy or no hierarchy try to allocate in it, otherwise this
             * sub-resource is taken by a different FEC hierarchy.
             */
            if (current_hierarchy == hierarchy || current_hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY)
            {
                /*
                 * If the hierarchy of the sub-resource matches the one of the current FEC and there are FECs
                 * allocated in it, then this sub-resource is chosen for the new element.
                 * Otherwise, if the sub-resource is empty it will be considered for allocation.
                 */
                if (current_hierarchy == hierarchy && nof_free_elements < fec_bank_size)
                {
                    /** This sub-resource is already in use for this hierarchy. */
                    if (fec_resource_type != DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_NO_STAT
                        && nof_free_elements <= fec_bank_size / 2)
                    {
                        fec_alloc_data.super_fec_type_tag = fec_resource_type;
                        fec_alloc_data.sub_resource_index = subr_id;
                        /*
                         * Need to find an empty grain in the sub-resource. To do that we will use the allocation itself
                         * using the ALLOCATE_SIMULATION flag.
                         * If it passes then the sub-resource is valid for this allocation.
                         */
                        rv = algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.allocate_single
                            (unit, subr_id, (alloc_flags | SW_STATE_ALGO_RES_ALLOCATE_SIMULATION),
                             &fec_alloc_data, fec_index);
                        if (rv == BCM_E_NONE)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                /*
                 * If the bank is empty and no bank was found (if only a small bank was found try to find a big one)
                 */
                else if (nof_free_elements == fec_bank_size && (subres_found == -1 || pair_subres_found == FALSE))
                {
                    algo_dnx_l3_fec_banks_update_info_t tmp_cluster_update;
                    uint8 pair_is_allocated, pair_found = FALSE;
                    sub_resource_pair_id = ALGO_L3_GET_FEC_SUB_RESOURCE_PAIR(subr_id);
                    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_hierarchy.
                                    fec_hierarchy_map.get(unit, sub_resource_pair_id, &pair_hierarchy));

                    dnx_algo_l3_init_fec_cluster_struct(subr_id, &tmp_cluster_update);

                    pair_is_allocated = (pair_hierarchy != DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY) ? TRUE : FALSE;
                    /*
                     * If the sub-resource is empty then we will check if the current element can be allocated in it.
                     * If it can be, we will save its ID and other empty sub-resources will not be considered.
                     */
                    SHR_IF_ERR_EXIT(algo_l3_fec_mdb_table_resources_check
                                    (unit, hierarchy, pair_is_allocated, &tmp_cluster_update));

                    if (tmp_cluster_update.resource_was_found
                        && (tmp_cluster_update.clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[0] ==
                            MDB_MACRO_A || pair_is_allocated
                            || tmp_cluster_update.
                            clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].cluster_to_update[1].
                            logical_start_address != ALGO_L3_INVALID_LOGICAL_ADDRESS))
                    {
                        pair_found = TRUE;
                    }

                    /*
                     * In case a free cluster was found update the bank number and the available cluster.
                     * In case a single small bank (macro B cluster) was already found update only if a big bank can be used instead.
                     */
                    if (tmp_cluster_update.resource_was_found == TRUE
                        && (subres_found == -1 || (pair_subres_found == FALSE && pair_found)))
                    {
                        subres_found = subr_id;
                        if (pair_found)
                        {
                            pair_subres_found = TRUE;
                        }
                        sal_memcpy(&new_cluster_update, &tmp_cluster_update, sizeof(new_cluster_update));
                    }
                }
            }
        }

        /*
         * If we found a partially taken sub-resource for this hierarchy,
         * then it is the one which is chosen for the current allocation.
         * If we didn't find a partially taken sub-resource but we found a free sub-resource
         * (subres_found is not default value), then we will allocate this sub-resource for the use of the current hier.
         * If we didn't find a free resource or a partially taken one, an error will be returned.
         */
        if (subr_id < nof_sub_resources)
        {
            sub_resource_id = subr_id;
        }
        else if (subr_id == nof_sub_resources && subres_found != -1)
        {
            sub_resource_id = subres_found;

            sal_memcpy(&cluster_to_update, &new_cluster_update, sizeof(cluster_to_update));
        }
        else
        {
            /*
             * If the subr_id variable equals the number of sub-resources
             * then we have reached to the end of the loop and we didn't find a valid sub-resource.
             */
            SHR_ERR_EXIT(_SHR_E_FULL, "No valid bank for without ID allocation for hierarchy %d found\n",
                         hierarchy + 1);
        }

        nof_big_banks_passed = ALGO_L3_NOF_BIG_BANKS_BEFORE_CURRENT(sub_resource_id);
    }

    fec_alloc_data.super_fec_type_tag = fec_resource_type;
    fec_alloc_data.sub_resource_index = sub_resource_id;
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                    fec_res_manager.allocate_single(unit, sub_resource_id, alloc_flags, &fec_alloc_data, fec_index));
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.
                    fec_resource_type.set(unit, sub_resource_id, (*fec_index / 2), fec_resource_type));
    nof_big_banks_passed = ALGO_L3_NOF_BIG_BANKS_BEFORE_CURRENT(sub_resource_id);
    *fec_index = *fec_index + (nof_big_banks_passed * dnx_data_l3.fec.max_fec_stage_map_size_get(unit)) +
        ((sub_resource_id - nof_big_banks_passed) * dnx_data_l3.fec.fec_stage_map_size_get(unit));

    if (cluster_to_update.resource_was_found)
    {

        SHR_IF_ERR_EXIT(dnx_l3_fec_mdb_table_resources_set(unit, &cluster_to_update));

        sal_memcpy(bank_id, cluster_to_update.clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id,
                   ALGOL_L3_NOF_FEC_BANK_TYPES * sizeof(bank_id[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_fec_deallocate(
    int unit,
    int fec_index,
    int *bank_id)
{
    uint32 sub_resource_id;
    uint32 pair_sub_resource_id = -1;
    uint32 fec_bank_size;
    int nof_free_elements;
    int pair_empty = TRUE;
    int nof_big_banks_passed = 0;
    uint8 part_of_a_pair = 0;
    SHR_FUNC_INIT_VARS(unit);

    sub_resource_id =
        DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);
    nof_big_banks_passed = ALGO_L3_NOF_BIG_BANKS_BEFORE_CURRENT(sub_resource_id);
    fec_bank_size = (sub_resource_id % 5 == 0) ?
        dnx_data_l3.fec.max_fec_stage_map_size_get(unit) : dnx_data_l3.fec.fec_stage_map_size_get(unit);
    fec_index = fec_index - (nof_big_banks_passed * dnx_data_l3.fec.max_fec_stage_map_size_get(unit) +
                             (sub_resource_id - nof_big_banks_passed) * dnx_data_l3.fec.fec_stage_map_size_get(unit));

    /** Deallocate the FEC entry from the resource. */
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.free_single(unit, sub_resource_id, fec_index));

    /** Receive the number of free elements in the bank. */
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.nof_free_elements_get
                    (unit, sub_resource_id, &nof_free_elements));

    /** Limit the size of resource FEC bank with 2 if bank is smaller */
    if (!ALGO_L3_FEC_BANK_IS_BIG(sub_resource_id))
    {
        nof_free_elements -=
            (dnx_data_l3.fec.max_fec_stage_map_size_get(unit) - dnx_data_l3.fec.fec_stage_map_size_get(unit));
    }

    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                    resource_is_part_of_pair.bit_get(unit, sub_resource_id >> 1, &part_of_a_pair));

    if (part_of_a_pair)
    {
        int pair_nof_free_elements;
        uint32 pair_bank_size;

        pair_sub_resource_id = ALGO_L3_GET_FEC_SUB_RESOURCE_PAIR(sub_resource_id);

        pair_bank_size = ALGO_L3_FEC_BANK_IS_BIG(pair_sub_resource_id) ?
            dnx_data_l3.fec.max_fec_stage_map_size_get(unit) : dnx_data_l3.fec.fec_stage_map_size_get(unit);

        /** Receive the number of free elements in the bank. */
        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.nof_free_elements_get
                        (unit, pair_sub_resource_id, &pair_nof_free_elements));

        /** Limit the size of resource FEC bank with 2 if bank is smaller */
        if (!ALGO_L3_FEC_BANK_IS_BIG(pair_sub_resource_id))
        {
            pair_nof_free_elements -=
                (dnx_data_l3.fec.max_fec_stage_map_size_get(unit) - dnx_data_l3.fec.fec_stage_map_size_get(unit));
        }

        pair_empty = (pair_nof_free_elements == pair_bank_size) ? TRUE : FALSE;

    }

    /*
     * If we just removed the last element in the bank, then we need to reset the stage map value.
     * To do so, the bank_id value is set to the sub_resource_id and then it will be cleared.
     */
    if (nof_free_elements == fec_bank_size && pair_empty)
    {
        bank_id[0] = sub_resource_id;
        bank_id[1] = pair_sub_resource_id;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_fec_is_allocated(
    int unit,
    int fec_index,
    uint8 *is_allocated)
{
    uint32 sub_resource_id;
    int nof_big_banks_passed = 0;
    SHR_FUNC_INIT_VARS(unit);

    sub_resource_id =
        DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);
    nof_big_banks_passed = ALGO_L3_NOF_BIG_BANKS_BEFORE_CURRENT(sub_resource_id);
    fec_index = fec_index - (nof_big_banks_passed * dnx_data_l3.fec.max_fec_stage_map_size_get(unit) +
                             (sub_resource_id - nof_big_banks_passed) * dnx_data_l3.fec.fec_stage_map_size_get(unit));

    /** Receive indication whether the entry is allocated. */
    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.fec_res_manager.is_allocated
                    (unit, sub_resource_id, fec_index, is_allocated));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_fec_super_fec_type_get(
    int unit,
    int fec_index,
    uint32 *super_fec_type)
{
    uint32 sub_resource_id;
    uint32 grain_size = 2;
    int nof_big_banks_passed = 0;
    SHR_FUNC_INIT_VARS(unit);

    sub_resource_id =
        DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, fec_index) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);
    nof_big_banks_passed = ALGO_L3_NOF_BIG_BANKS_BEFORE_CURRENT(sub_resource_id);
    fec_index = fec_index - (nof_big_banks_passed * dnx_data_l3.fec.max_fec_stage_map_size_get(unit) +
                             (sub_resource_id - nof_big_banks_passed) * dnx_data_l3.fec.fec_stage_map_size_get(unit));

    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.fec_resource_type.get
                    (unit, sub_resource_id, fec_index / grain_size, super_fec_type));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
dnx_algo_l3_fec_protection_state_get(
    int unit,
    int fec_index,
    uint8 *protected)
{
    uint32 super_fec_type;
    SHR_FUNC_INIT_VARS(unit);

    fec_index = BCM_L3_ITF_VAL_GET(fec_index);

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_super_fec_type_get(unit, fec_index, &super_fec_type));

    /*
     * If the supr FEC type is one of the protected types then this FEC is part of a protection pair.
     */
    if (super_fec_type == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT
        || super_fec_type == DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT)
    {
        *protected = TRUE;
    }
    else
    {
        *protected = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
algo_l3_fec_mdb_table_resources_get(
    int unit,
    uint32 hierarchy,
    int *nof_clusters,
    mdb_cluster_alloc_info_t * clusters)
{
    mdb_cluster_alloc_info_t clusters_found[MDB_MAX_NOF_CLUSTERS];
    dbal_physical_tables_e dbal_physical_fec_table_id;
    int idx;
    int cl_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Choose the physical FEC table according to the input flags. */
    if (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2)
    {
        dbal_physical_fec_table_id = DBAL_PHYSICAL_TABLE_FEC_2;
    }
    else if (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3)
    {
        dbal_physical_fec_table_id = DBAL_PHYSICAL_TABLE_FEC_3;
    }
    else
    {
        dbal_physical_fec_table_id = DBAL_PHYSICAL_TABLE_FEC_1;
    }

    SHR_IF_ERR_EXIT(mdb_init_get_table_resources(unit, dbal_physical_fec_table_id, clusters_found, nof_clusters));

    /** Iterate over all clusters for the current FEC table */
    for (idx = 0; idx < *nof_clusters; idx++)
    {
        /** If the table Id doesn't match, then we skip this iteration */
        if (clusters_found[idx].dbal_physical_table_id != dbal_physical_fec_table_id)
        {
            continue;
        }
        sal_memcpy(&clusters[cl_id], &clusters_found[idx], sizeof(mdb_cluster_alloc_info_t));
        cl_id++;
    }

    *nof_clusters = cl_id;
exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to .h file for more information
 */
shr_error_e
algo_l3_fec_mdb_table_resources_check(
    int unit,
    dbal_enum_value_field_hierarchy_level_e hierarchy,
    uint8 pair_is_allocated,
    algo_dnx_l3_fec_banks_update_info_t * cluster_update)
{
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS];
    uint8 found_empty;
    uint32 idx, cl_id;
    uint32 cluster_is_free;
    uint32 sub_resource_id;
    uint32 nof_fec_per_cluster;
    uint32 resource_logical_start_address;
    int nof_clusters = 0;
    int first_pfec_in_cluster;
    int macro_b_indx = 0;
    int cluster_id_to_update_type_b[2] = { -1, -1 };
    int cluster_id_to_update_type_a = -1;
    int base_cluster_size = 0;
    int bank_type_idx;
    int skip_idx = -1;
    int bank_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l3_fec_mdb_table_resources_get(unit, hierarchy, &nof_clusters, clusters));
    /** try to allocate for the requested bank and then for the pair bank if needed */
    for (bank_type_idx = 0; bank_type_idx < ALGOL_L3_NOF_FEC_BANK_TYPES; bank_type_idx++)
    {

        if (bank_type_idx == ALGOL_L3_FEC_BANK_REQUESTED_TYPE)
        {
            bank_id = cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[0];
        }
        else
        {
            /** When working on the pair allocation it means the the requested bank allocation was found*/
            pair_is_allocated = TRUE;
            bank_id =
                ALGO_L3_GET_FEC_SUB_RESOURCE_PAIR(cluster_update->
                                                  clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[0]);
        }

        resource_logical_start_address = ALGO_L3_GET_START_LOGICAL_ADDRESS_FROM_BANK_ID(unit, bank_id);
        found_empty = FALSE;
        /** Iterate over the array of clusters and look for an existing range to which the FEC belongs. */
        for (idx = 0; idx < nof_clusters; idx += 2)
        {
            /*
             * The skip index is the index that was found for the requested bank and can't be used for the pair bank
             */
            if (idx == skip_idx)
            {
                continue;
            }
            cluster_is_free = 0;
            /** Receive the base size of the current pair of clusters. */
            base_cluster_size =
                (clusters[idx].macro_nof_rows * dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit)) *
                clusters[idx].macro_row_width / clusters[idx].entry_size;
            first_pfec_in_cluster =
                DNX_ALGO_L3_PHYSICAL_FEC_GET(unit,
                                             clusters[idx].logical_start_address *
                                             dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit));
            /** If there's any residue when finding the first FEC in the cluster, an additional FEC is added to it.*/
            first_pfec_in_cluster =
                ((clusters[idx].logical_start_address * dnx_data_l3.fec.super_fec_size_get(unit)) %
                 dnx_data_l3.fec.fec_row_size_get(unit) != 0) ? first_pfec_in_cluster + 1 : first_pfec_in_cluster;
            nof_fec_per_cluster =
                ((first_pfec_in_cluster / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit)) % 5 ==
                 0) ? (base_cluster_size + 2) : base_cluster_size;
            /*
             * Make sure that if the bigger cluster from the pair is the second one,
             * the number of FECs in the cluster pair will be correct.
             */
            if (nof_fec_per_cluster == base_cluster_size)
            {
                nof_fec_per_cluster =
                    ALGO_L3_FEC_BANK_IS_BIG(first_pfec_in_cluster /
                                            dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit) +
                                            1) ? (base_cluster_size + 2) : base_cluster_size;
            }

            /*
             * If the FEC index is in range - between the start address and the end address (start + nof_fecs),
             * then this is the valid cluster for this FEC ID.
             * Otherwise check if the cluster pair is free and if it is, then its ID will be saved.
             * In case the pair bank is allocated and this bank has a cluster with the requested address but is of MARO A type, it can't
             * be used as it overlap the pair bank
             */
            if (resource_logical_start_address >=
                (clusters[idx].logical_start_address * dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit))
                && (resource_logical_start_address <
                    (clusters[idx].logical_start_address * dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit)) +
                    nof_fec_per_cluster) && (!pair_is_allocated || clusters[idx].macro_type == MDB_MACRO_B))
            {

                sal_memcpy(&
                           (cluster_update->
                            clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].cluster_to_update[bank_type_idx]),
                           &clusters[idx], sizeof(mdb_cluster_alloc_info_t));
                cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[bank_type_idx] = bank_id;
                skip_idx = idx;
                break;
            }
            else if (cluster_id_to_update_type_b[1] == -1 && cluster_id_to_update_type_a == -1)
            {
                /*
                 * MACRO-A cluster can hold 52428 FECs; MACRO-B cluster can hold 26214;
                 * Go over each pair of two clusters that are used for FECs and check if the sub-resource
                 * to which the FECs are allocated is free.
                 */
                for (cl_id = 0; cl_id < nof_fec_per_cluster / dnx_data_l3.fec.fec_stage_map_size_get(unit); cl_id++)
                {
                    uint32 fec_hierarchy;
                    sub_resource_id =
                        DNX_ALGO_L3_PHYSICAL_FEC_GET(unit,
                                                     clusters[idx +
                                                              cl_id].logical_start_address *
                                                     dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit)) /
                        dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit) + cl_id;

                    SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_hierarchy.
                                    fec_hierarchy_map.get(unit, sub_resource_id, &fec_hierarchy));

                    /** If the sub resource is not taken by this hierarchy it means that the FEC cluster is free */
                    if (fec_hierarchy != hierarchy)
                    {
                        cluster_is_free++;
                    }
                }
                /*
                 * If the variable cluster_is_free has a value of the number of iterations done over the clusters
                 * (indicating that all sub-resources that were gone over in the previous for loop are free)
                 * then we have found a valid cluster for the wanted range.
                 */
                if (cluster_is_free == nof_fec_per_cluster / dnx_data_l3.fec.fec_stage_map_size_get(unit))
                {

                    /*
                     * In case the other sub resource pair is allocated we can only use a MACRO B cluster
                     */
                    if (clusters[idx].macro_type == MDB_MACRO_A && pair_is_allocated == FALSE)
                    {
                        found_empty = TRUE;
                        cluster_id_to_update_type_a = idx;
                    }
                    else if (clusters[idx].macro_type == MDB_MACRO_B && (macro_b_indx < 2))
                    {
                        found_empty = TRUE;
                        cluster_id_to_update_type_b[macro_b_indx++] = idx;
                    }
                }
            }

        }

        /*
         * In case no existing allocation wasn't found but an empty allocation was, update the "update" structure
         */
        if (cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].
            cluster_to_update[bank_type_idx].logical_start_address == ALGO_L3_INVALID_LOGICAL_ADDRESS && found_empty)
        {
            /*
             * If the FEC ID matched none of the available cluster ranges but an empty cluster was found the function
             * will return the available cluster for the requested range.
             */

            if (cluster_id_to_update_type_a != -1)
            {
                skip_idx = cluster_id_to_update_type_a;
                sal_memcpy(&
                           (cluster_update->
                            clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].cluster_to_update[bank_type_idx]),
                           &clusters[cluster_id_to_update_type_a], sizeof(mdb_cluster_alloc_info_t));
            }
            else
            {
                skip_idx = cluster_id_to_update_type_b[0];
                clusters[cluster_id_to_update_type_b[0]].logical_start_address = resource_logical_start_address;
                cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[bank_type_idx] = bank_id;
                sal_memcpy(&
                           (cluster_update->
                            clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].cluster_to_update[bank_type_idx]),
                           &clusters[cluster_id_to_update_type_b[0]], sizeof(mdb_cluster_alloc_info_t));
                /*
                 * Only relevant for the requested bank and not for the pair bank.
                 * This case should be used in case the pair is already allocated.
                 */
                if (pair_is_allocated && (bank_type_idx == ALGOL_L3_FEC_BANK_REQUESTED_TYPE))
                {
                    /*
                     * The pair is allocated and we found a MACRO B which isn't is place and should be moved into the requested bank ID
                     * range which might be occupied by a MACRO A cluster which wasn't been selected as the pair is allocated.
                     * To avoid two cluster of the same hierarchy on the same address the MACRO A cluster should be moved if there is
                     * one on the requested address.
                     */
                    if (clusters[cluster_id_to_update_type_b[0]].logical_start_address !=
                        resource_logical_start_address)
                    {
                        uint32 index_to_move = UTILEX_U32_MAX;
                        for (idx = 0; idx < nof_clusters; idx++)
                        {
                            if (clusters[idx].macro_type == MDB_MACRO_A &&
                                ((clusters[idx].logical_start_address <= resource_logical_start_address) &&
                                 (resource_logical_start_address <
                                  (clusters[idx].logical_start_address +
                                   2 * dnx_data_l3.fec.fec_stage_map_size_get(unit)))))
                            {
                                index_to_move = idx;
                                break;
                            }
                        }

                        if (index_to_move != UTILEX_U32_MAX)
                        {

                            uint32 unavailable_fec_banks[DNX_DATA_MAX_L3_FEC_FEC_MAX_NOF_BANKS >> 1];
                            uint32 nof_mcaro_a_allocation_units;

                            /*
                             * Each MACRO A takes 2 banks so the MACRO A allocation units will be the NOF of FEC banks
                             * divided by 2.
                             */
                            SHR_IF_ERR_EXIT(dnx_algo_l3_nof_fec_banks_get(unit, &nof_mcaro_a_allocation_units));

                            nof_mcaro_a_allocation_units >>= 1;

                            sal_memset(unavailable_fec_banks, 0,
                                       nof_mcaro_a_allocation_units * sizeof(unavailable_fec_banks[0]));
                            for (idx = 0; idx < nof_clusters; idx++)
                            {

                                /*
                                 * We can skip the index which we are about to move and don't mark it current place as taken as it is about to be moved.
                                 * The cluster which should be moved to an empty slot is considered as we don't want it to be place in its "old" place.
                                 */
                                if (cluster_id_to_update_type_b[0] != idx)
                                {
                                    uint32 bank_to_check =
                                        clusters[idx].logical_start_address /
                                        (dnx_data_l3.fec.fec_stage_map_size_get(unit) * 2);
                                    unavailable_fec_banks[bank_to_check] = TRUE;
                                }
                            }

                            idx = 0;

                            while (idx < nof_mcaro_a_allocation_units && unavailable_fec_banks[idx] == TRUE)
                            {
                                idx++;
                            }

                            if (idx >= nof_mcaro_a_allocation_units)
                            {
                                SHR_ERR_EXIT(_SHR_E_FAIL,
                                             "Macro A cluster that is found on the designated logical address couldn't be moved into a different location\n");
                            }
                            else
                            {
                                cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_MOVE].bank_id[0] = idx << 1;
                                sal_memcpy(&
                                           (cluster_update->
                                            clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_MOVE].cluster_to_update[0]),
                                           &clusters[index_to_move], sizeof(mdb_cluster_alloc_info_t));
                            }

                        }
                    }

                }
                else if (cluster_id_to_update_type_b[1] != -1)
                {
                    cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[1] =
                        ALGO_L3_GET_FEC_SUB_RESOURCE_PAIR(bank_id);
                    /*
                     * If the other sub resource pair isn't already allocated, another single MACRP B allocation is needed
                     */
                    sal_memcpy(&
                               (cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].cluster_to_update[1]),
                               &clusters[cluster_id_to_update_type_b[1]], sizeof(mdb_cluster_alloc_info_t));
                }

            }
        }

        /*
         * In case one of the following, don't search a cluster for the pair bank
         * 1. the pair bank is already allocated.
         * 2. A cluster for the requested bank (not the pair) wasn't found so not point looking for the pair
         * 3. A MACRO A cluster was found for the requested bank which includes the pair in it
         * 4. A MACRO B cluster was already found for the pair
         */
        if (pair_is_allocated
            || cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].
            cluster_to_update[0].logical_start_address == ALGO_L3_INVALID_LOGICAL_ADDRESS
            || cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].cluster_to_update[0].macro_type ==
            MDB_MACRO_A
            || cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].
            cluster_to_update[1].logical_start_address != ALGO_L3_INVALID_LOGICAL_ADDRESS)
        {
            break;
        }

    }

    cluster_update->resource_was_found =
        (cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].cluster_to_update[0].logical_start_address !=
         ALGO_L3_INVALID_LOGICAL_ADDRESS);

    /*
     * In case the a MACRO A was selected the order of the bank ID is important for allocation considerations.
     * The lower bank ID should be first
     */
    if (cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].cluster_to_update[0].macro_type ==
        MDB_MACRO_A)
    {
        cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[0] =
            ALGO_L3_GET_FEC_EVEN_SUB_RESOURCE(cluster_update->
                                              clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[0]);
        cluster_update->clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[1] =
            ALGO_L3_GET_FEC_SUB_RESOURCE_PAIR(cluster_update->
                                              clusters_info[ALGOL_L3_FEC_BANKS_GROUP_TO_UPDATE].bank_id[0]);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_res_dbal_ecmp_group_profile_is_allocated(
    int unit,
    int index,
    uint8 *is_allocated)
{
    int hierarchy_iter;
    int ref_count = 0;
    dnx_l3_ecmp_profile_t ecmp_profile;

    SHR_FUNC_INIT_VARS(unit);

    *is_allocated = FALSE;
    /** Iterate over the three hierarchies to find where we have references to this profile. */
    for (hierarchy_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
         hierarchy_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); hierarchy_iter++)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.profile_data_get
                        (unit, hierarchy_iter, _SHR_CORE_ALL, index, &ref_count, &ecmp_profile));
        /** If there are references to this profile data, then it is considered as allocated. */
        if (ref_count > 0)
        {
            *is_allocated = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_res_dbal_ecmp_group_profile_allocate(
    int unit,
    int *profile_index)
{
    uint8 first_reference;

    dnx_l3_ecmp_profile_t ecmp_profile;
    uint32 hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;

    SHR_FUNC_INIT_VARS(unit);
    /** Set some unique values for the ECMP profile so that the template manager doesn't consider it as existent. */
    sal_memset(&ecmp_profile, 0, sizeof(dnx_l3_ecmp_profile_t));
    ecmp_profile.group_size = *profile_index % dnx_data_l3.ecmp.max_group_size_get(unit);
    ecmp_profile.members_table_id = *profile_index;

    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.allocate_single
                    (unit, hierarchy, _SHR_CORE_ALL, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &ecmp_profile, NULL,
                     profile_index, &first_reference));
exit:
    SHR_FUNC_EXIT;
}

/*
 *  Refer to the .h file for more information
 */
shr_error_e
dnx_algo_l3_res_dbal_ecmp_group_profile_free(
    int unit,
    int profile_index)
{
    uint8 last_reference;
    uint32 hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.free_single
                    (unit, hierarchy, _SHR_CORE_ALL, profile_index, &last_reference));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l3_res_dbal_ecmp_group_profile_get_next(
    int unit,
    uint32 hierarchy,
    int *profile_index)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_l3_db.algo_l3_templates.ecmp_profile_hier_table.get_next
                    (unit, hierarchy, _SHR_CORE_ALL, profile_index));
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
