/** \file out_lif_profile.h
 *
 * out_lif profile management for DNX.
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef OUT_LIF_PROFILE_H_INCLUDED
/*
 * {
 */
#define OUT_LIF_PROFILE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm/l3.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/***************************************************************************************/
/*
 * Defines
 */
/***************************************************************************************/
/*
 * Default profiles/values defines
 */
/**
 * Specify default out_lif_profile
 */
#define DNX_OUT_LIF_PROFILE_DEFAULT (0)
/**
 * Specify default out_lif_orientation
 */
#define DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION (0)
#define DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE (56)
/**
 * Specify default split_horizon enable/disable
 */
#define DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE (1)
/**
 * Specify default OAM LIF Profile
 */
#define DNX_OUT_LIF_PROFILE_OAM_PROFILE (0)
/***************************************************************************************/
/*
 * Enums
 */
/***************************************************************************************/
/*
 * Structures
 */

/**
 * /brief - out_lif template data structure.
 * The template manager's data of out_lif_profile used for orientation only.
 * this structure is for inner usage only.
 */
typedef struct
{
    uint8 out_lif_orientation;
} out_lif_template_data_orientation_t;

/**
 * /brief - out_lif template data structure.
 * The template manager's data of out_lif_profile used for oam_profile only.
 * this structure is for inner usage only.
 */
typedef struct
{
    uint8 oam_lif_profile;
} out_lif_template_data_oam_profile_t;

/**
 * /brief - out_lif template data structure.
 * The template manager's data of out_lif_profileused for all data-fields.
 * this structure is for inner usage only.
 */
typedef struct
{
    uint8 out_lif_orientation;
    uint8 etpp_split_horizon_enable;
    uint8 oam_lif_profile;
} out_lif_template_data_t;

/**
 * \brief - the input structure of dnx_out_lif_profile_exchange,
 * includes the out_lif profile properties to set.
 * \remark
 *  need to call out_lif_profile_info_t_init in order to initiate the fields to the default valid values
 *  and then to update the structure and send it to the API.
 */
typedef struct
{
    uint8 out_lif_orientation;
    uint8 etpp_split_horizon_enable;
    uint8 oam_lif_profile;
} out_lif_profile_info_t;
/***************************************************************************************/
/*
 * API
 */
/**
 * \brief
 *   initializes the module out_lif_profile:
 *   writes default profiles/values to relevant tables, for example:
 *   OUT_LIF_PROFILE_TABLE
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * updates dbal tables OUT_LIF_PROFILE_TABLE
 * \see
 *   * None
 */
shr_error_e dnx_out_lif_profile_module_init(
    int unit);

/**
 * \brief
 *   Deinitialize the module out_lif_profile:
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_out_lif_profile_module_deinit(
    int unit);

/**
 * \brief
 *   updates template manager for L2/L3 profiles, and exchanges the old out_lif_profile with
 *   new out_lif_profile according to LIF properties which are stored in out_lif_profile_info.
 * \param [in] unit -
 *   The unit number.
 * \param [out] dbal_table_id -
 *   DBAL table ID, used to differentiate between the supported outlif profile supported ranges \n
 * \param [out] flags -
 *   Flags, which might be used in future, to differentiate between the supported outlif profile supported ranges \n
 * \param [in] out_lif_profile_info -
 *   Pointer to out_lif_profile_info \n
 *     Pointed memory includes LIF-PROFILE properties
 * \param [in] old_out_lif_profile -
 *   old out_lif_profile id to exchange
 * \param [out] new_out_lif_profile -
 *   Pointer to new_out_lif_profile \n
 *     This procedure loads pointed memory by the newly assigned out_lif_profile
 *     id in template manager
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   We don't clean HW tables in case of last profile case, because of the
 *   issue that when we delete the profile till we move the InLIF
 *   to the new place traffic will get some "undefine" state.
 * \see
 *   * dnx_out_lif_profile_algo_handle_routing_enablers_profile
 *   * dnx_out_lif_profile_algo_convert_to_rpf_mode
 *   * dnx_algo_template_exchange
 *   * DBAL_TABLE_OUT_LIF_PROFILE_TABLE
 *   * NOF_ROUTING_PROFILES_DUPLICATES
 */
shr_error_e dnx_out_lif_profile_exchange(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 flags,
    out_lif_profile_info_t * out_lif_profile_info,
    int old_out_lif_profile,
    int *new_out_lif_profile);

/**
 * \brief
 *   gets out_lif_profile and returns references count
 * \param [in] unit -
 *   The unit number.
 * \param [in] out_lif_profile -
 *   out_lif_profile id to get ref_count for
 * \param [out] ref_count -
 *   Pointer to ref_count \n
 *     Pointed memory includes references count of the out_lif_profile
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_out_lif_profile_get_ref_count(
    int unit,
    int out_lif_profile,
    int *ref_count);

/**
 * \brief
 *   gets out_lif_profile and returns out_lif_profile data
 * \param [in] unit -
 *   The unit number.
 * \param [in] out_lif_profile -
 *   out_lif_profile id to get data for
 * \param [out] out_lif_profile_info -
 *   Pointer to out_lif_profile_info \n
 *     Pointed memory includes out_lif_profile properties
 * \param [in] dbal_table_id -
 *  dbal table id, with which the profile is associated.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_out_lif_profile_get_data(
    int unit,
    int out_lif_profile,
    out_lif_profile_info_t * out_lif_profile_info,
    dbal_tables_e dbal_table_id);

/*
 * \brief
 *      initialize fields of the structure out_lif_profile_info_t to the default values
 * \param [in] unit -
 *   The unit number.
 * \param [out] out_lif_profile_info -
 *   Pointer to out_lif_profile_info \n
 *     Pointed memory includes LIF-PROFILE properties initiated to the default values
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void out_lif_profile_info_t_init(
    int unit,
    out_lif_profile_info_t * out_lif_profile_info);

/**
* \brief
*  This function purpose is to set Out Lif profile in HW
* \par DIRECT INPUT:
*   \param [in] unit  -  unit Id
*   \param [in] port  - InLIF gport
*   \param [in] new_out_lif_profile  -  OutLIF-profile to configure
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_out_lif_profile_set(
    int unit,
    bcm_gport_t port,
    int new_out_lif_profile);

/**
* \brief
*  This function purpose is to get the out Lif profile for given inLIF
* \par DIRECT INPUT:
*   \param [in] unit  -  unit Id
*   \param [in] port  - OutLIF gport
*   \param [out] out_lif_profile_p  -  OutLIF-profile configured for InLIF given
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_out_lif_profile_get(
    int unit,
    bcm_gport_t port,
    int *out_lif_profile_p);
/*
 * }
 */
#endif /* OUT_LIF_PROFILE_H_INCLUDED */
