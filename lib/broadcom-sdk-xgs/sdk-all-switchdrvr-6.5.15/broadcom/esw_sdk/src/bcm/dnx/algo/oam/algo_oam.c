/** \file algo_oam.c
 * $Id$
 *
 * Resource and templates needed for the oam feature.
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
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <soc/dnx/swstate/auto_generated/access/algo_oam_access.h>

/** Callbacks needed for the template used in oam   */

void
dnx_oam_group_icc_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_oam_group_icc_profile_data_t *icc_profile_data = (dnx_oam_group_icc_profile_data_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "icc_profile_data[5]", *icc_profile_data[5],
                                        NULL, "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "icc_profile_data[4]", *icc_profile_data[4],
                                        NULL, "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "icc_profile_data[3]", *icc_profile_data[3],
                                        NULL, "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "icc_profile_data[2]", *icc_profile_data[2],
                                        NULL, "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "icc_profile_data[1]", *icc_profile_data[1],
                                        NULL, "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "icc_profile_data[0]", *icc_profile_data[0],
                                        NULL, "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
    return;
}

/*
 * See prototype definition for function description in algo_oam.h
 */
void
dnx_oam_oamp_tpid_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    uint16 tpid = *(uint16 *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT16, "TPID", tpid, NULL, "0x%08X");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);

    return;
}

/*
 * See prototype definition for function description in algo_oam.h
 */
void
dnx_oam_oamp_sa_mac_msb_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    bcm_mac_t *sa_mac_msb = (bcm_mac_t *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_MAC, "Source MAC MSB", *sa_mac_msb, NULL, NULL);

    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);

    return;
}

/*
 * See prototype definition for function description in algo_oam.h
 */
void
dnx_oam_oamp_da_mac_msb_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    bcm_mac_t *da_mac_msb = (bcm_mac_t *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_MAC, "Destination MAC MSB", *da_mac_msb, NULL, NULL);

    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);

    return;
}

/*
 * See prototype definition for function description in algo_oam.h
 */
void
dnx_oam_oamp_da_mac_lsb_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    bcm_mac_t *da_mac_lsb = (bcm_mac_t *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_MAC, "Destination MAC MSB", *da_mac_lsb, NULL, NULL);

    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);

    return;
}

/**
 * \brief - This function creates the resource configuration 
 *        needed for the OAM group allocation
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
dnx_algo_oam_group_id_init(
    int unit)
{
    int capacity;
    sw_state_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init algo_oam sw state module.
     */
    SHR_IF_ERR_EXIT(algo_oam_db.init(unit));

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
    /** First element id for group id res mngr */
    data.first_element = 0;
    /** Number of groups that can be configured for device   */

    /*
     * Get Number of groups ==================== Number of entries in lif db = (IOEM_capacity - 100) / 2 ( size of
     * elemnt is 30 bits, each entry has <60) Number of endpoints = 8 * Number of entries in lif db Number of groups
     * = Number of endpoints 
     */
    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_IOEM_1, &capacity));
    data.nof_elements = 8 * (capacity - 100) / 2;

    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_OAM_GROUP_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oam_db.oam_group_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the resource configuration
 *        needed for the OAM profile allocation. 4 different
 *        pools should be created (1 per type).
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
dnx_algo_oam_profile_id_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));

    data.flags = 0;
    /*
     * First element id for profile id res mngr (0 is used for
     * default profile)
     */
    data.first_element = 1;
    /*
     * Number of profiles that can be configured for device
     * non-acc profiles (excluding profile 0)
     */
    data.nof_elements = dnx_data_oam.general.oam_nof_non_acc_action_profiles_get(unit) - 1;

    /** Ingress non-acc */
    sal_strncpy(data.name, DNX_ALGO_OAM_PROFILE_ID_ING, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_profile_id_ing.create(unit, &data, NULL));
    /** Egress non-acc */
    sal_strncpy(data.name, DNX_ALGO_OAM_PROFILE_ID_EG, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_profile_id_eg.create(unit, &data, NULL));

    /*
     * Number of profiles that can be configured for device
     * acc profiles (excluding profile 0)
     */
    data.nof_elements = dnx_data_oam.general.oam_nof_acc_action_profiles_get(unit) - 1;

    /** Ingress acc */
    sal_strncpy(data.name, DNX_ALGO_OAM_PROFILE_ID_ING_ACC, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_profile_id_ing_acc.create(unit, &data, NULL));
    /** Egress acc */
    sal_strncpy(data.name, DNX_ALGO_OAM_PROFILE_ID_EG_ACC, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_profile_id_eg_acc.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the templates needed for the
 *        OAM features
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Templates created are:\n
 *      1.OAM Accelerated MEP ICC profile Table: LONG type group name
 *      ICC profiles which contain 6 bytes of the group name.\n
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oam_init_templates(
    int unit)
{
    int capacity;
    sw_state_algo_template_create_data_t oam_group_icc_profile_data;
    uint32 nof_icc_profiles, nof_references;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_icc_profiles = dnx_data_oam.oamp.nof_icc_profiles_get(unit);

    /*
     * Get Number of endpoints ======================= Number of entries in lif db = (IOEM_capacity - 100) / 2 ( size
     * of elemnt is 30 bits, each entry has <60) Number of endpoints = 8 * Number of entries in lif db 
     */
    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_IOEM_1, &capacity));
    nof_references = 8 * (capacity - 100) / 2;

    /** Create template for OAMP ICC map table */
    sal_memset(&oam_group_icc_profile_data, 0, sizeof(sw_state_algo_template_create_data_t));
    oam_group_icc_profile_data.flags = 0;
    oam_group_icc_profile_data.first_profile = 0;
    oam_group_icc_profile_data.nof_profiles = nof_icc_profiles;
    oam_group_icc_profile_data.max_references = nof_references;
    oam_group_icc_profile_data.default_profile = 0;
    oam_group_icc_profile_data.data_size = sizeof(dnx_oam_group_icc_profile_data_t);
    oam_group_icc_profile_data.default_data = NULL;
    sal_strncpy(oam_group_icc_profile_data.name, DNX_ALGO_OAM_GROUP_ICC_PROFILE,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_group_icc_profile.create(unit, &oam_group_icc_profile_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the resource configuration 
 *        needed for the OAM RMEP allocation which is below self contained entries
 *        threshold
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
dnx_algo_oam_rmep_id_below_threshold_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 self_contained_threshold;
    uint32 nof_scaned_rmep_db_entries = 0;
    SHR_FUNC_INIT_VARS(unit);

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
    nof_scaned_rmep_db_entries = dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) + 1;

    if (self_contained_threshold > 0)
    {
        sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
        /** First element id for RMEP id below threshold res mngr */
        data.first_element = 0;
        if (self_contained_threshold < nof_scaned_rmep_db_entries)
        {
            /** Number of half entry or offloaded RMEPs that can be configured for device   */
            data.nof_elements = self_contained_threshold;
        }
        else
        {
            data.nof_elements = nof_scaned_rmep_db_entries;
        }

        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_OAM_RMEP_ID_BELOW_THRESHOLD, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_below_threshold.create(unit, &data, NULL));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the resource configuration 
 *        needed for the OAM RMEP allocation which is above self contained entries
 *        threshold
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
dnx_algo_oam_rmep_id_above_threshold_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 self_contained_threshold;
    uint32 nof_scaned_rmep_db_entries = 0;
    SHR_FUNC_INIT_VARS(unit);

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
    nof_scaned_rmep_db_entries = dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) + 1;

    if (nof_scaned_rmep_db_entries > self_contained_threshold)
    {
        sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
        /** First element id for RMEP id above threshold res mngr */
        data.first_element = 0;
        /*
         * Number of self contained RMEPs that can be configured for device is equal to
         * number of self contained RMEP OAMP entries divided by 2 since it takes 2 entries
         * for one self contained RMEP.
         */
        data.nof_elements = ((nof_scaned_rmep_db_entries - self_contained_threshold) / 2);
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_OAM_RMEP_ID_ABOVE_THRESHOLD, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_above_threshold.create(unit, &data, NULL));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the template needed for sa mac msb profiles
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
dnx_algo_oam_oamp_sa_mac_msb_template_init(
    int unit)
{
    sw_state_algo_template_create_data_t oam_sa_mac_msb_profile_data;
    uint32 max_nof_endpoint_id;
    uint32 nof_sa_mac_profiles;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_sa_mac_profiles = dnx_data_oam.oamp.oam_nof_sa_mac_profiles_get(unit);
    max_nof_endpoint_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

    /** Create template for SA MAC msb */
    sal_memset(&oam_sa_mac_msb_profile_data, 0, sizeof(sw_state_algo_template_create_data_t));
    oam_sa_mac_msb_profile_data.flags = 0;
    oam_sa_mac_msb_profile_data.first_profile = 0;
    oam_sa_mac_msb_profile_data.nof_profiles = nof_sa_mac_profiles;
    oam_sa_mac_msb_profile_data.max_references = max_nof_endpoint_id;
    oam_sa_mac_msb_profile_data.default_profile = 0;
    oam_sa_mac_msb_profile_data.data_size = sizeof(bcm_mac_t);
    oam_sa_mac_msb_profile_data.default_data = NULL;
    sal_strncpy(oam_sa_mac_msb_profile_data.name, DNX_ALGO_OAM_OAMP_SA_MAC_MSB_PROFILE_TEMPLATE,
                SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_sa_mac_msb_profile_template.create(unit, &oam_sa_mac_msb_profile_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the template needed for da mac msb profiles
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
dnx_algo_oam_oamp_da_mac_msb_template_init(
    int unit)
{
    sw_state_algo_template_create_data_t oam_da_mac_msb_profile_data;
    uint32 max_nof_endpoint_id;
    uint32 nof_da_mac_msb_profiles;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_da_mac_msb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_msb_profiles_get(unit);
    max_nof_endpoint_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

    /** Create template for DA MAC msb */
    sal_memset(&oam_da_mac_msb_profile_data, 0, sizeof(sw_state_algo_template_create_data_t));
    oam_da_mac_msb_profile_data.flags = 0;
    oam_da_mac_msb_profile_data.first_profile = 0;
    oam_da_mac_msb_profile_data.nof_profiles = nof_da_mac_msb_profiles;
    oam_da_mac_msb_profile_data.max_references = max_nof_endpoint_id;
    oam_da_mac_msb_profile_data.default_profile = 0;
    oam_da_mac_msb_profile_data.data_size = sizeof(bcm_mac_t);
    oam_da_mac_msb_profile_data.default_data = NULL;
    sal_strncpy(oam_da_mac_msb_profile_data.name, DNX_ALGO_OAM_OAMP_DA_MAC_MSB_PROFILE_TEMPLATE,
                SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.create(unit, &oam_da_mac_msb_profile_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the template needed for da mac lsb profiles
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
dnx_algo_oam_oamp_da_mac_lsb_template_init(
    int unit)
{
    sw_state_algo_template_create_data_t oam_da_mac_lsb_profile_data;
    uint32 max_nof_endpoint_id;
    uint32 nof_da_mac_lsb_profiles;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_da_mac_lsb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_lsb_profiles_get(unit);
    max_nof_endpoint_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

    /** Create template for DA MAC lsb */
    sal_memset(&oam_da_mac_lsb_profile_data, 0, sizeof(sw_state_algo_template_create_data_t));
    oam_da_mac_lsb_profile_data.flags = 0;
    oam_da_mac_lsb_profile_data.first_profile = 0;
    oam_da_mac_lsb_profile_data.nof_profiles = nof_da_mac_lsb_profiles;
    oam_da_mac_lsb_profile_data.max_references = max_nof_endpoint_id;
    oam_da_mac_lsb_profile_data.default_profile = 0;
    oam_da_mac_lsb_profile_data.data_size = sizeof(bcm_mac_t);
    oam_da_mac_lsb_profile_data.default_data = NULL;
    sal_strncpy(oam_da_mac_lsb_profile_data.name, DNX_ALGO_OAM_OAMP_DA_MAC_LSB_PROFILE_TEMPLATE,
                SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.create(unit, &oam_da_mac_lsb_profile_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the template for the OAM tpid profiles.
 *          Used for ETH OAM tpid
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
dnx_algo_oam_oamp_tpid_profile_id_init(
    int unit)
{
    sw_state_algo_template_create_data_t tpid_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tpid_profile_data, 0, sizeof(sw_state_algo_template_create_data_t));
    tpid_profile_data.flags = 0;
    tpid_profile_data.first_profile = 0;
    tpid_profile_data.nof_profiles = dnx_data_oam.oamp.oam_nof_tpid_profiles_get(unit);
    /** Two tags per endpoint could be configured(outer/inner tag) */
    tpid_profile_data.max_references = 2 * dnx_data_oam.general.oam_nof_oamp_meps_get(unit);
    tpid_profile_data.default_profile = 0;
    tpid_profile_data.data_size = sizeof(uint16);
    tpid_profile_data.default_data = NULL;
    sal_strncpy(tpid_profile_data.name, DNX_ALGO_OAM_OAMP_TPID_PROFILE_TEMPLATE,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.create(unit, &tpid_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize templates for all oam profile types.
 *        Current oam  Templates:\n
 *  OAM group id - group ID for OAM groups
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
dnx_algo_oam_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_oam_group_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_profile_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_init_templates(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_rmep_id_below_threshold_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_rmep_id_above_threshold_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_oamp_sa_mac_msb_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_oamp_da_mac_msb_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_oamp_da_mac_lsb_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_oamp_tpid_profile_id_init(unit));

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
dnx_algo_oam_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
