/**
 * \file algo_port_pp.c
 * Internal DNX Port PP Managment APIs
PIs $Copyright: (c) 2018 Broadcom.
PIs Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/dnx/port/port_tpid.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/switch/switch_tpid.h>
#include <soc/dnx/swstate/auto_generated/access/algo_port_pp_access.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>

/**
 * }
 */

/**
 * Force Forward
 * {
 */

/**
 * \brief - create the template manager used to track the traps used in force forward
 */
static shr_error_e
dnx_algo_port_pp_force_forward_init(
    int unit)
{
    sw_state_algo_template_create_data_t create_info;
    uint32 force_forward_template_data;
    int nof_cores, nof_traps;
    SHR_FUNC_INIT_VARS(unit);

    /** int vars */
    sal_memset(&create_info, 0, sizeof(sw_state_algo_template_create_data_t));
    force_forward_template_data = -1;

    /** create the template manager used to track the traps used in force forward */
    create_info.flags = SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE;
    create_info.first_profile = 0;
    create_info.nof_profiles = dnx_data_port.general.nof_pp_ports_get(unit);
    create_info.max_references = dnx_data_port.general.nof_pp_ports_get(unit);
    create_info.default_profile = 0;
    create_info.data_size = sizeof(uint32);
    create_info.default_data = &force_forward_template_data;
    sal_strncpy(create_info.name, DNX_PORT_PP_FORCE_FORWARD_TEMPLATE, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.mngr.create(unit, &create_info, NULL));

    /** allocate additional array to store the trap ids */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_traps = dnx_data_port.general.nof_pp_ports_get(unit);
    SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.trap_id.alloc(unit, nof_cores, nof_traps));

exit:
    SHR_FUNC_EXIT;
}

/**see .h file */
void
dnx_algo_port_pp_force_forward_template_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{

    LOG_CLI((BSL_META_U(unit, "force forward destination: 0x%x"), *(uint32 *) data));

}

/**
 * \brief -
 *   Intialize Port PP SIT profile algorithms.
 *   The function creats SIT profile Template profiles.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * dnx_algo_port_pp_init
 */
static shr_error_e
algo_port_sit_profile_template_init(
    int unit)
{
    sw_state_algo_template_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set a template for Port SIT profile
     */

    sal_memset(&data, 0, sizeof(data));

    data.first_profile = 0;

    data.nof_profiles = dnx_data_port_pp.general.nof_egress_sit_profile_get(unit) - 1;
    data.max_references = dnx_data_port.general.nof_pp_ports_get(unit) * dnx_data_device.general.nof_cores_get(unit);
    data.data_size = sizeof(dnx_sit_profile_t);
    sal_strncpy(data.name, DNX_ALGO_EGRESS_PP_PORT_SIT_PROFILE_TEMPLATE,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_port_pp_db.egress_pp_port_sit_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header file algo_port_pp.h for description.
 */
void
dnx_algo_port_esem_access_cmd_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    int esem_access_id, app_db_id, esem_offset, default_profile, index;
    dnx_esem_cmd_data_t *esem_cmd_data = (dnx_esem_cmd_data_t *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);
    for (index = 0; index < ESEM_ACCESS_IF_COUNT; index++)
    {
        if (esem_cmd_data->esem[index].valid == TRUE)
        {
            esem_access_id = index + 1;
            app_db_id = esem_cmd_data->esem[index].app_db_id;
            esem_offset = esem_cmd_data->esem[index].designated_offset;
            default_profile = esem_cmd_data->esem[index].default_result_profile;

            DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "esem access id", esem_access_id, NULL,
                                                "%d");
            DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "app db id", app_db_id, NULL, "%d");
            DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "designated offset", esem_offset, NULL,
                                                "%d");
            DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "default result profile",
                                                default_profile, NULL, "%d");
        }
    }
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);

    return;
}

/**
 * \brief -
 *   Create and Intialize the algorithm for esem access commands per template.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 * \see
 *   None
 */
static shr_error_e
algo_port_esem_access_cmd_template_init(
    int unit)
{
    sw_state_algo_template_create_data_t template_data;
    dnx_esem_cmd_data_t esem_cmd_data;
    smart_template_create_info_t extra_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialize the templates data/properties
     */
    sal_memset(&template_data, 0, sizeof(sw_state_algo_template_create_data_t));
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    template_data.data_size = sizeof(dnx_esem_cmd_data_t);
    template_data.default_data = &esem_cmd_data;
    template_data.first_profile = 0;
    template_data.default_profile = dnx_data_esem.access_cmd.no_action_get(unit);
    template_data.flags = SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE |
        SW_STATE_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM;
    template_data.nof_profiles = dnx_data_esem.access_cmd.nof_cmds_get(unit);
    template_data.max_references = dnx_data_port.general.nof_pp_ports_get(unit) +
        dnx_data_lif.out_lif.nof_local_out_lifs_get(unit) + 1;
    template_data.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SMART_TEMPLATE;
    sal_strncpy(template_data.name, DNX_ALGO_TEMPLATE_ESEM_ACCESS_CMD, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    /** Use the smart tempate algorithm to allocate template and resource bitmap for range allocation*/
    extra_info.resource_flags = 0;
    extra_info.resource_create_info.max_tag_value = 0;
    extra_info.resource_create_info.grain_size = template_data.nof_profiles;
    SHR_IF_ERR_EXIT(algo_port_pp_db.esem.access_cmd.create(unit, &template_data, (void *) &extra_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   The function destroy esem-accesee-cmd template.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  algo_port_esem_access_cmd_template_init
 */
static shr_error_e
algo_port_esem_access_cmd_template_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Sw state will be deinit internal.
     */
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Create and Intialize the algorithm for esem default result profile.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  dnx_algo_port_pp_init
 *
 */
static shr_error_e
algo_port_esem_default_result_profile_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * ESEM-DEFAULT-RESULT_PROFILE resource management
     */
    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
    data.flags = 0;
    data.first_element = 0;
    data.nof_elements = dnx_data_esem.default_result_profile.nof_profiles_get(unit);
    sal_strncpy(data.name, DNX_ALGO_ESEM_DEFAULT_RESULT_PROFILE, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  The function destroy the esem-default-result-profile resource.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  * None
 *
 * \see
 *  algo_port_match_esem_default_result_profile_init
 *  dnx_algo_port_pp_deinit
 */
static shr_error_e
algo_port_esem_default_result_profile_deinit(
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

/**
* Global Functions:
* {
*/

/*
* See header file algo_port_pp.h for description.
*/
shr_error_e
dnx_algo_port_pp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_port_pp_db.init(unit));

    /*
     * Initialize force forward sw state
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_force_forward_init(unit));

    /*
     * Initialize Port TPID Template profile
     */
    SHR_IF_ERR_EXIT(algo_port_tpid_init(unit));

    /*
     * Initialize Port SIT profile template
     */
    SHR_IF_ERR_EXIT(algo_port_sit_profile_template_init(unit));

    /*
     * Initialize ESEM access command template
     */
    SHR_IF_ERR_EXIT(algo_port_esem_access_cmd_template_init(unit));

    /*
     * Initialize ESEM default result profile resource
     */
    SHR_IF_ERR_EXIT(algo_port_esem_default_result_profile_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
* See header file algo_port_pp.h for description.
*/
shr_error_e
dnx_algo_port_pp_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_port_tpid_deinit(unit));

    /*
     * Destroy ESEM access command template
     */
    SHR_IF_ERR_EXIT(algo_port_esem_access_cmd_template_deinit(unit));

    /*
     * Destroy ESEM default result profile resource
     */
    SHR_IF_ERR_EXIT(algo_port_esem_default_result_profile_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
* See header file algo_port_pp.h for description.
*/

void
dnx_algo_egress_sit_profile_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_sit_profile_t *sit_profile = (dnx_sit_profile_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "tag_type", sit_profile->tag_type,
                                        "Sit tag type, see dbal_enum_value_field_sit_tag_type_e", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT16, "tpid", sit_profile->tpid,
                                        "Tpid value of the sit tag", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "pcp_dei_src",
                                        sit_profile->pcp_dei_src,
                                        "pcp dei value source, see dbal_enum_value_field_sit_pcp_dei_src_e", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "vid_src",
                                        sit_profile->vid_src,
                                        "vid value source, see dbal_enum_value_field_sit_vid_src_e", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);

}

/*
* See header file algo_port_pp.h for description.
*/
void
dnx_algo_egress_acceptable_frame_type_profile_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    int outer_tpid_indx;
    int inner_tpid_indx;

    /*
     * Print every acceptable frame type data entry in the profile in the following format
     */
    DNX_SW_STATE_PRINT(unit, "keys ={outer_tpid, inner_tpid}, data = {the acceptable frame type entry data}\n");

    /*
     * Priority tag is not considered here because only outer_tpid/inner_tpid is used to access acceptable frame type table
     */
    for (outer_tpid_indx = 0; outer_tpid_indx < BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS; outer_tpid_indx++)
    {
        for (inner_tpid_indx = 0; inner_tpid_indx < BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS; inner_tpid_indx++)
        {
            /*
             * Takes the bit out of the template and print it:
             */
            acceptable_frame_type_template_t *template_data;
            /*
             * see dnx_sw_state_dump.h for more information 
             */
            DNX_SW_STATE_DUMP_UPDATE_CURRENT_IDX(unit, inner_tpid_indx);
            template_data = (acceptable_frame_type_template_t *) data;

            DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, outer_tpid_indx, " ");
            DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, inner_tpid_indx, " ");
            DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit,
                                                  template_data->acceptable_frame_type_template[outer_tpid_indx]
                                                  [inner_tpid_indx], " ");

            DNX_SW_STATE_PRINT(unit, "keys ={%d, %d}, data = 0x%X\n", outer_tpid_indx, inner_tpid_indx,
                               template_data->acceptable_frame_type_template[outer_tpid_indx][inner_tpid_indx]);
        }
        /*
         * see dnx_sw_state_dump.h for more information 
         */
        DNX_SW_STAET_DUMP_END_OF_STRIDE(unit);
    }
}

/**
 * }
 */
