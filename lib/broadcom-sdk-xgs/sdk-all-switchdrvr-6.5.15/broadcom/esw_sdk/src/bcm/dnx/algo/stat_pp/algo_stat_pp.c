/** \file algo_stat_pp.c
 * $Id$
 *
 * Resource and templates needed for the stat_pp feature.
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
#include <bcm/stat.h>
#include <bcm_int/dnx/algo/stat_pp/algo_stat_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
#include <soc/dnx/swstate/auto_generated/access/algo_stat_pp_access.h>

/** Callbacks needed for the template used in stat_pp   */

/**
 * \brief - This function creates the resource configuration 
 *          needed for the stat_pp IRPP profiles allocation
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
dnx_algo_stat_pp_irpp_profile_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
    /** First element id for group id res mngr */
    data.first_element = 0;
    /** Number of groups that can be configured for device   */
    data.nof_elements = dnx_data_stat.stat_pp.max_irpp_profile_value_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_STAT_PP_IRPP_PROFILE_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(stat_pp_db.stat_pp_irpp_profile_id.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the resource configuration 
 *          needed for the stat_pp ETPP profiles allocation
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
dnx_algo_stat_pp_etpp_profile_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
    /** First element id for group id res mngr */
    data.first_element = 0;
    /** Number of groups that can be configured for device   */
    data.nof_elements = dnx_data_stat.stat_pp.max_etpp_profile_value_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_STAT_PP_ETPP_PROFILE_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(stat_pp_db.stat_pp_etpp_profile_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the resource configuration 
 *          needed for the stat_pp ERPP profiles allocation
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
dnx_algo_stat_pp_erpp_profile_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
    /** First element id for group id res mngr */
    data.first_element = 0;
    /** Number of groups that can be configured for device   */
    data.nof_elements = dnx_data_stat.stat_pp.max_erpp_profile_value_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_STAT_PP_ERPP_PROFILE_ID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(stat_pp_db.stat_pp_erpp_profile_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init function, init all resources
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
dnx_algo_stat_pp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init the algo_stat_pp sw state module.
     */
    SHR_IF_ERR_EXIT(stat_pp_db.init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_stat_pp_irpp_profile_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_stat_pp_etpp_profile_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_stat_pp_erpp_profile_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - deInit function, empty
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
dnx_algo_stat_pp_deinit(
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
