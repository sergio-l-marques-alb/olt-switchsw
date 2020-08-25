/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file algo_mpls.c
 *
 *  mpls algorithms initialization and deinitialization.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

/*************
 * INCLUDES  *
 *************/
/*
 * { 
 */
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/algo/mpls/algo_mpls.h>
#include <soc/dnx/swstate/auto_generated/access/algo_mpls_access.h>

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
*   Print an entry of the enablers vector template. See
*       \ref dnx_algo_template_print_data_cb for more details.
*/
void
dnx_algo_mpls_termination_profile_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_mpls_termination_profile_t *term_profile = (dnx_mpls_termination_profile_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "labels_to_terminate",
                                        term_profile->labels_to_terminate, "labels to terminate", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "ttl_exp_label_index",
                                        term_profile->ttl_exp_label_index, "ttl exp label index", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "reject_ttl_0", term_profile->reject_ttl_0,
                                        "reject_ttl_0", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "reject_ttl_1", term_profile->reject_ttl_1,
                                        "reject_ttl_1", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "has_cw", term_profile->has_cw, "has_cw", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "expect_bos", term_profile->expect_bos,
                                        "expect_bos", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "check_bos", term_profile->check_bos, "check_bos",
                                        NULL);

    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
}

/**
* \brief
*   Print the push profile termplate . See
*       \ref dnx_algo_template_print_data_cb for more details.
*/
void
dnx_algo_mpls_push_profile_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_jr_mode_mpls_push_profile_t *push_profile = (dnx_jr_mode_mpls_push_profile_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "ttl_model",
                                        push_profile->ttl_model, "ttl_model", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "exp_model",
                                        push_profile->exp_model, "exp_model", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "has_cw", push_profile->has_cw, "has_cw", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "ttl", push_profile->ttl, "ttl", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "exp", push_profile->exp, "exp", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "remark_profile", push_profile->remark_profile,
                                        "remark_profile", NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "add_entropy_label",
                                        push_profile->add_entropy_label, "add_entropy_label", NULL);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "add_entropy_label_indicator",
                                        push_profile->add_entropy_label_indicator, "add_entropy_label_indicator", NULL);

    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);

}
/**
 * \brief 
 * allocate resource pool for all MPLS profiles
 */
shr_error_e
dnx_algo_mpls_init(
    int unit)
{
    sw_state_algo_template_create_data_t template_data;
    sw_state_algo_res_create_data_t res_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_mpls_db.init(unit));
    /*
     * Init MPLS Termination Profile
     */
    sal_memset(&template_data, 0, sizeof(sw_state_algo_template_create_data_t));
    template_data.flags = 0;
    template_data.data_size = sizeof(dnx_mpls_termination_profile_t);
    template_data.first_profile = 0;
    template_data.max_references = dnx_data_lif.in_lif.nof_dpc_local_in_lifs_get(unit);
    template_data.nof_profiles = dnx_data_mpls.general.nof_mpls_termination_profiles_get(unit);
    sal_strncpy(template_data.name, DNX_MPLS_TERMINATION_PROFILE_TEMPLATE,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_mpls_db.termination_profile.create(unit, &template_data, NULL));
    /*
     * Init MPLS Push Profile
     */
    sal_memset(&template_data, 0, sizeof(sw_state_algo_template_create_data_t));
    template_data.flags = 0;
    template_data.data_size = sizeof(dnx_jr_mode_mpls_push_profile_t);
    template_data.first_profile = 0;
    template_data.max_references = SAL_INT32_MAX;
    template_data.nof_profiles = dnx_data_mpls.general.nof_mpls_push_profiles_get(unit);
    sal_strncpy(template_data.name, DNX_MPLS_TERMINATION_PROFILE_TEMPLATE,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_mpls_db.push_profile.create(unit, &template_data, NULL));
    /*
     * Init MPLS Label range resource manager
     */
    sal_memset(&res_data, 0, sizeof(res_data));
    res_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC;
    res_data.first_element = 0;
    res_data.nof_elements = dnx_data_mpls.general.nof_mpls_range_elements_get(unit);
    sal_strncpy(res_data.name, "MPLS Label Range Profile", sizeof(res_data.name));
    SHR_IF_ERR_EXIT(algo_mpls_db.label_range_profile.create(unit, &res_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_mpls_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * } 
 */
