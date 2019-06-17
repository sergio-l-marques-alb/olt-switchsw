/** \file instru_sflow.c
 * $Id$
 *
 * Visibility procedures for DNX in instrumentation module.
 *
 * This file contains functions for sflow configuration.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/auto_generated/access/instru_access.h>

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/instru/instru.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm/types.h>
#include <include/bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <include/bcm_int/dnx/lif/in_lif_profile.h>

/*
 * }
 */

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/instru.h>

/*
 * }
 */

/*
 * }
 */
/*
 * Function Declaration.
 * {
 */

/*
 * }
 */

/*
 * Defines.
 * {
 */

/**
 * \brief
 * Default value, needed for the sFlow ETPS.
 */
#define INSTRU_SFLOW_OAM_LIF_SET      1

/**
 * \brief
 * Default oam lif db key value
 */
#define INSTRU_OAM_KEY_PREFIX   0

/**
 * \brief
 * Default oam lif db MDL MP TYPE value
 */
#define INSTRU_MDL_MP_TYPE  0

/**
 * \brief
 * Number of RAW entries in the sFlow stack.
 */
#define INSTRU_SFLOW_NOF_RAW_ENCAP_ENTRIES dnx_data_instru.sflow.nof_sflow_raw_entries_per_stack_get(unit)

/**
 * \brief
 * Number of
 */
#define DNX_INSTRU_SFLOW_VERSION (5)
 /*
  * }
  */

 /*
  * Internal functions.
  * {
  */

 /*
  * }
  */

/**
 * \brief - Given an instru global lif, returns the local lif.
 */
static shr_error_e
dnx_instru_sflow_encap_id_to_local_lif(
    int unit,
    int sflow_encap_id,
    int *local_lif)
{
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    bcm_gport_t encap_in_tunnel;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Turn the global lif into a tunnel, then call gport to hw resources.
     */
    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    BCM_GPORT_TUNNEL_ID_SET(encap_in_tunnel, sflow_encap_id);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, encap_in_tunnel,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

    *local_lif = hw_res.local_out_lif;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify sflow create API params. Verify supported flags. In case of REPLACE flag on, validate that such an encap_id exists
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_info - Pointer to a struct from which the relevant data
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_encap_create_verify(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int nof_sflow_encaps;
    SHR_FUNC_INIT_VARS(unit);

    if (sflow_encap_info->flags & ~(BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Unsupported flag mask %x. Supported flags are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE",
                     sflow_encap_info->flags);
    }

    /** Verify replace case */
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        dnx_algo_gpm_gport_hw_resources_t hw_res;
        bcm_gport_t encap_in_tunnel;

        /*
         * Turn the global lif into a tunnel, then call gport to hw resources.
         */
        sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
        BCM_GPORT_TUNNEL_ID_SET(encap_in_tunnel, sflow_encap_info->sflow_encap_id);

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, encap_in_tunnel,
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

        if (hw_res.outlif_dbal_table_id != DBAL_TABLE_EEDB_SFLOW)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_INSTRU_SFLOW_ENCAP_REPLACE flag was given, but sFlow encap 0x%x is not of type sFlow.",
                         sflow_encap_info->sflow_encap_id);
        }
    }
    else
    {
        /*
         * If it's not replace, verify that the maximum number of sFlow encaps wasn't exceeded.
         */
        SHR_IF_ERR_EXIT(instru.sflow_info.nof_sflow_encaps.get(unit, &nof_sflow_encaps));

        if (nof_sflow_encaps == dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Can't create another sFlow instance. Maximum number has been exceeded: %d",
                         nof_sflow_encaps);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate an sflow encap lif and increase the encaps counter.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] sflow_encap_info - The flags will be used as input, as well as the sFlow encap if
 *                                    the WITH_ID flag is given. Otherwise, the sFlow encap will be filled.
 * \param [out] sflow_local_outlif - Allocated local sFlow encap lif
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_encap_allocate(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int *sflow_local_outlif)
{
    int lif_alloc_flags = 0, global_lif_id;
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * If we were given the replace flag, just translate the sFlow encap to local lif and we're done.
     * The lif's existence has already been verified.
     */
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        /** Get local lif from algo gpm. */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                        (unit, sflow_encap_info->sflow_encap_id, sflow_local_outlif));
        SHR_EXIT();
    }

    /*
     * If it's not replace, then allocate the sFlow encap entry.
     */
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID))
    {
        /*
         * If WITH_ID flag is used - get the global out-lif from the user input and
         * add alloc_with_id flag
         */
        global_lif_id = sflow_encap_info->sflow_encap_id;
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }

    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_SFLOW;
    outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &global_lif_id, NULL, &outlif_info));

    /*
     * Increase the sFlow encaps counter.
     */
    SHR_IF_ERR_EXIT(instru.sflow_info.nof_sflow_encaps.inc(unit, 1));

    /*
     * Return the allocated global and local lifs.
     */
    sflow_encap_info->sflow_encap_id = global_lif_id;
    *sflow_local_outlif = outlif_info.local_outlif;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - free the sflow encap lif and reduce the encaps counter.
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_id - The sFlow encap to free.
 * \param [in] sflow_local_outlif - The local outlif to free.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_encap_free(
    int unit,
    int sflow_encap_id,
    int sflow_local_outlif)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Delete global and local lifs.
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, sflow_encap_id, NULL, sflow_local_outlif));

    /*
     * Decrease the sFlow encaps counter.
     */
    SHR_IF_ERR_EXIT(instru.sflow_info.nof_sflow_encaps.dec(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate a sFlow destination profile according to the required UDP tunnel.
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_info - UDP tunnel will be taken from here, as well as the old sFlow destination profile if the
 *                                  REPLACE flag is set.
 * \param [out] sflow_destination_profile - The destination profile for this UDP tunnel.
 * \param [out] write_destination_profile - If set, write the new profile to HW. If not, the profile is already written.
 * \param [out] old_sflow_destination_profile - If REPLACE flag was set, this holds the profile that the sflow encap was pointing to.
 * \param [out] delete_old_destination_profile - If REPLACE flag was set, and this is set, then remove this profile from HW.
 *                                          If it's not set, then then the profile is still in use, and don't remove it.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_destination_profile_allocate(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int *sflow_destination_profile,
    int *write_destination_profile,
    int *old_sflow_destination_profile,
    int *delete_old_destination_profile)
{
    uint8 first_reference, last_reference;
    uint8 success;
    sflow_destination_key_t destination;

    SHR_FUNC_INIT_VARS(unit);

    *write_destination_profile = FALSE;
    *delete_old_destination_profile = FALSE;

    sal_memset(&destination, 0, sizeof(sflow_destination_key_t));
    destination.sub_agent_id = sflow_encap_info->sub_agent_id;
    destination.udp_tunnel = sflow_encap_info->tunnel_id;

    /*
     * Check for replace flag.
     */
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        /*
         * If the replace flag was given, then read the old sFlow destination profile, and
         * use exchange operation.
         * No need to check if the entry was found because it was already checked by _verify.
         */
        uint8 found;

        SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.find(unit,
                                                                                        &sflow_encap_info->sflow_encap_id,
                                                                                        old_sflow_destination_profile,
                                                                                        &found));

        SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.exchange(unit, _SHR_CORE_ALL, 0, &destination,
                                                                             *old_sflow_destination_profile, NULL,
                                                                             sflow_destination_profile,
                                                                             &first_reference, &last_reference));

        /*
         * Remove the mapping from the encap id to the old UDP profile.
         */
        SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.delete(unit,
                                                                                          &sflow_encap_info->sflow_encap_id));

        if (last_reference)
        {
            *delete_old_destination_profile = TRUE;
        }
    }
    else
    {
        /*
         * If it's not replace, just call allocate.
         */
        SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.allocate_single(unit, _SHR_CORE_ALL, 0,
                                                                                    &destination, NULL,
                                                                                    sflow_destination_profile,
                                                                                    &first_reference));
        *old_sflow_destination_profile = -1;
        last_reference = FALSE;
    }

    /*
     * If it's the first time we meet this UDP tunnel profile, then we need to update the raw lifs.
     */
    if (first_reference)
    {
        *write_destination_profile = TRUE;
    }

    /*
     * Add the new UDP profile to the mapping.
     */
    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.insert(unit,
                                                                                      &sflow_encap_info->sflow_encap_id,
                                                                                      sflow_destination_profile,
                                                                                      &success));

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't create mapping between sFlow encap and UDP profile.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free a sFlow destination profile.
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_info - UDP tunnel will be taken from here.
 * \param [out] sflow_destination_profile - The destination profile for this UDP tunnel.
 * \param [out] delete_destination_profile - If set, then remove this profile from HW.
 *                                          If it's not set, then the profile is still in use, and don't remove it.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_destination_profile_free(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int *sflow_destination_profile,
    int *delete_destination_profile)
{
    uint8 last_reference, found;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the udp profile, then delete it and clear the mapping.
     * No need to verify found because we already checked it in _verify function.
     */
    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.find(unit,
                                                                                    &sflow_encap_info->sflow_encap_id,
                                                                                    sflow_destination_profile, &found));

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.free_single(unit, _SHR_CORE_ALL,
                                                                            *sflow_destination_profile,
                                                                            &last_reference));

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.delete(unit,
                                                                                      &sflow_encap_info->sflow_encap_id));

    if (last_reference)
    {
        *delete_destination_profile = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate the raw lifs for a destination profile.
 *
 * \param [in] unit - Relevant unit
 * \param [out] sflow_destination_profile - The destination profile for these raw lifs.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_raw_lifs_allocate(
    int unit,
    int sflow_destination_profile)
{
    int lif_alloc_flags = 0;
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA;
    lif_alloc_flags |= LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;

    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, NULL, NULL, &outlif_info));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile,
                                                                 0, outlif_info.local_outlif));

    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, NULL, NULL, &outlif_info));
    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile,
                                                                 1, outlif_info.local_outlif));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free the raw lifs for a destination profile.
 *
 * \param [in] unit - Relevant unit
 * \param [out] sflow_destination_profile - The destination profile for these raw lifs.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_raw_lifs_free(
    int unit,
    int sflow_destination_profile)
{
    int local_outlif;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 0, &local_outlif));

    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, NULL, local_outlif));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile, 0, 0));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 1, &local_outlif));

    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, NULL, local_outlif));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile, 1, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set raw data lif
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_info - Encap info for these raw data lifs.
 * \param [in] sflow_destination_profile - the sFlow destination profile for these lifs.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_raw_data_lifs_write(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int sflow_destination_profile)
{
    uint32 entry_handle_id;
    int current_outlif, next_outlif;
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    uint32 entry_data[4];
    bcm_ip_t agent_ip_address;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    /*
     * Write the raw outlif for encap 3.
     * Its data is described below.
     * It's next pointer is the UDP tunnel local lif, so get it from algo_gpm.
     */
    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 1, &current_outlif));

    /*
     * entry_data[0] - sub agent id - take from input.
     * entry_data[1] - agent ip addr - take from sw state
     * entry_data[2] - agent ip - currently 1 for ipv4.
     * entry_data[3] - sFlow version - const 5 for now.
     */
    SHR_IF_ERR_EXIT(instru.sflow_info.agent_ip_address.get(unit, &agent_ip_address));
    entry_data[0] = sflow_encap_info->sub_agent_id;
    entry_data[1] = agent_ip_address;
    entry_data[2] = 1;
    entry_data[3] = DNX_INSTRU_SFLOW_VERSION;

    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, sflow_encap_info->tunnel_id,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

    next_outlif = hw_res.local_out_lif;

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_outlif);
    /** Data fields*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, next_outlif);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 0x3);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EEDB_DATA_ENTRY, entry_handle_id));
    /*
     * Write the raw outlif for encap 2.
     * Its data is 0.
     * Its next pointer is the raw outlif for encap 3.
     */
    next_outlif = current_outlif;
    entry_data[0] = entry_data[1] = entry_data[2] = entry_data[3] = 0;

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 0, &current_outlif));

    /** Take DBAL handle. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_outlif);
    /** Data fields*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, next_outlif);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 0x3);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set sflow encap lif
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_local_outlif - Local sFlow encap lif
 * \param [in] sflow_destination_profile - sFlow destination profile for this sFlow encap. It will be used to
 *                                  get the next outlif pointer.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_encap_write(
    int unit,
    int sflow_local_outlif,
    uint32 sflow_destination_profile)
{
    uint32 entry_handle_id;
    int next_outlif;
    uint32 entry_data[3];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 0, &next_outlif));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, sflow_local_outlif);
    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP);
    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE, INSTRU_SFLOW_OAM_LIF_SET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, next_outlif);

    /*
     * Set entry raw data.
     * entry_data[0] : const 0.
     * entry_data[1] : Number of flow records (currently const 1)
     * entry_data[2] : Data format2 (currently const 1)
     *
     */
    entry_data[0] = 0;
    entry_data[1] = 1;
    entry_data[2] = 1;

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, entry_data);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set oam lif db data
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_info - Pointer to a struct from which the relevant data (stat_cmd, counter_command_id) is taken
 * \param [in] sflow_outlif - Serves as a key to the oam lif db dbal table
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_add_oam_lif_db(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int sflow_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, INSTRU_OAM_KEY_PREFIX);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, sflow_outlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL_MP_TYPE, INST_SINGLE, INSTRU_MDL_MP_TYPE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_BASE, INST_SINGLE,
                                 sflow_encap_info->stat_cmd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                 sflow_encap_info->counter_command_id);
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get oam lif db data
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_info - Pointer to a struct to which the relevant data (stat_cmd, counter_command_id) will be written into
 * \param [in] sflow_outlif - Serves as a key to the oam lif db dbal table
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_oam_lif_db_get(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int sflow_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, INSTRU_OAM_KEY_PREFIX);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, sflow_outlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    /** Get DATA fields */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_OAM_COUNTER_BASE, INST_SINGLE,
                                                        (uint32 *) &sflow_encap_info->stat_cmd));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                     (uint32 *) &sflow_encap_info->counter_command_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the sFlow destination connected to the sFlow encap.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] sflow_encap_info - sFlow encap info. The encap id is used as input, and the tunnel_id
 *                                      and sub_agent_id fields will be filled.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_destination_get(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    uint8 found;
    int sflow_destination_profile;
    sflow_destination_key_t destination_key;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.find(unit,
                                                                                    &sflow_encap_info->sflow_encap_id,
                                                                                    &sflow_destination_profile,
                                                                                    &found));

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.profile_data_get(unit, _SHR_CORE_ALL,
                                                                                 sflow_destination_profile, NULL,
                                                                                 &destination_key));

    sflow_encap_info->tunnel_id = destination_key.udp_tunnel;
    sflow_encap_info->sub_agent_id = destination_key.sub_agent_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete oam lif db entry
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_outlif - the sflow lif id, key to the dbal table
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_oam_lif_db_delete(
    int unit,
    int sflow_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, INSTRU_OAM_KEY_PREFIX);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, sflow_outlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    /** Clear dbal entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete sflow etps entry, remove global lif entry from GLEM
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_local_outlif - the sflow lif id, key to the dbal table
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_encap_clear(
    int unit,
    int sflow_local_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, sflow_local_outlif);
    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP);

    /** Clear dbal entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete raw data etps entries.
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_destination_profile - the sFlow destination profile that holds the raw lifs.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_sflow_raw_data_lifs_clear(
    int unit,
    int sflow_destination_profile)
{
    uint32 entry_handle_id;
    int current_outlif, current_outlif_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    for (current_outlif_index = 0; current_outlif_index < INSTRU_SFLOW_NOF_RAW_ENCAP_ENTRIES; current_outlif_index++)
    {
        SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile,
                                                                     current_outlif_index, &current_outlif));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_outlif);

        /** Clear dbal entry */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create an sflow entity. Sflow ETPS points to a raw data ETPS, which points to a udp ETPS entry in the EEDB
 *
 * \param [in] unit - relevant unit
 * \param [in] sflow_encap_info - A pointer to sflow struct:
 *                                   flags -
 *                                          BCM_INSTRU_SFLOW_ENCAP_WITH_ID - sflow encap id given
 *                                          BCM_INSTRU_SFLOW_ENCAP_REPLACE - replace an existing sflow lif. No need to update glem
 *                                   sflow_encap_id - sflow global lif
 *                                   tunnel_id - udp eedb entry tunnel id
 *                                   stat_cmd - statistic command to the crps - increment and read
 *                                   counter_command_id - crps interface id
 *
 * \return
 *   Negative in case of an error
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_instru_sflow_encap_create(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int sflow_global_lif = 0;
    int sflow_destination_profile = 0;
    int sflow_local_outlif = 0;
    int write_destination_profile, old_sflow_destination_profile, delete_old_destination_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_create_verify(unit, sflow_encap_info));

    /*
     * Allocate all resources.
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_allocate(unit, sflow_encap_info, &sflow_local_outlif));
    sflow_global_lif = sflow_encap_info->sflow_encap_id;

    SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_profile_allocate(unit, sflow_encap_info, &sflow_destination_profile,
                                                                  &write_destination_profile,
                                                                  &old_sflow_destination_profile,
                                                                  &delete_old_destination_profile));

    if (write_destination_profile && (sflow_destination_profile != old_sflow_destination_profile))
    {
        /*
         * We need to allocate new raw lifs if the udp profile is new, but not if we replace the content of
         * an existing profile.
         */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lifs_allocate(unit, sflow_destination_profile));
    }

    /*
     * Write to HW.
     */
    if (write_destination_profile)
    {
        /*
         * Only write the raw lifs if it's a new UDP profile, or same profile with new content.
         */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_data_lifs_write(unit, sflow_encap_info, sflow_destination_profile));
    }

    if (sflow_destination_profile != old_sflow_destination_profile)
    {
        /*
         * The only information stored by the sFlow encap is the pointer to the udp tunnel,
         * so if it didn't change then no need to update it.
         */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_write(unit, sflow_local_outlif, sflow_destination_profile));
    }

    SHR_IF_ERR_EXIT(dnx_instru_sflow_add_oam_lif_db(unit, sflow_encap_info, sflow_local_outlif));

    if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        /**
         * No need to write to GLEM if we perform replace since the global and local lif don't change.
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, sflow_global_lif, sflow_local_outlif));
    }

    if (delete_old_destination_profile)
    {
        /*
         * If it's the last instance of the old UDP profile, and it wasn't reused,
         * then free and clear the raw lifs associated with it.
         */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_data_lifs_clear(unit, old_sflow_destination_profile));

        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lifs_free(unit, sflow_destination_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an sflow entity, based on the encap id (sflow global lif) specified
 *
 * \param [in] unit - relevant unit
 * \param [in] sflow_encap_info - A pointer to sflow struct with the global lif which serves as "key" for deletion
 *
 * \return
 *   Negative in case of an error
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_instru_sflow_encap_delete(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int local_lif, sflow_destination_profile, delete_destination_profile = -1;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Clear HW.
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif(unit, sflow_encap_info->sflow_encap_id, &local_lif));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_delete(unit, local_lif));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_clear(unit, local_lif));

    /** Remove global lif from GLEM */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, sflow_encap_info->sflow_encap_id));

    /*
     * Free sFlow destination profile, and if necessary, free and clear raw outlifs.
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_profile_free
                    (unit, sflow_encap_info, &sflow_destination_profile, &delete_destination_profile));

    if (delete_destination_profile)
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_data_lifs_clear(unit, sflow_destination_profile));

        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lifs_free(unit, sflow_destination_profile));
    }

    /*
     * Free sFlow encap outlif.
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_free(unit, sflow_encap_info->sflow_encap_id, local_lif));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get an sflow entity, based on the encap id (sflow global lif) specified
 *
 * \param [in] unit - relevant unit
 * \param [in] sflow_encap_info - A pointer to sflow struct with the global lif, write data into it
 *
 * \return
 *   Negative in case of an error
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_instru_sflow_encap_get(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int local_lif;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if entry exists in GLEM*/
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif(unit, sflow_encap_info->sflow_encap_id, &local_lif));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_get(unit, sflow_encap_info, local_lif));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_get(unit, sflow_encap_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse over all configured sflow entries. Use given callback function on each entity
 *
 * \param [in] unit - relevant unit
 * \param [in] cb - A pointer to a callback function for data retreive
 * \param [in] user_data - data sent from the user that will be passed to the callback function
 *
 * \return
 *   Negative in case of an error
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_instru_sflow_encap_traverse(
    int unit,
    bcm_instru_sflow_encap_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end = 0;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    int local_lif, gport;
    bcm_instru_sflow_encap_info_t sflow_encap_info;
    uint32 result_rule_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    result_rule_value = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /**Iterate only over SFLOW_HDR_SAMP entries*/
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                     &result_rule_value, NULL));

    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                                                INST_SINGLE, field_value));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, field_value));
        local_lif = field_value[0];
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                    _SHR_CORE_ALL, local_lif, &gport));
        sflow_encap_info.sflow_encap_id = BCM_GPORT_TUNNEL_ID_GET(gport);
        SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_get(unit, &sflow_encap_info, local_lif));
        SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_get(unit, &sflow_encap_info));
        /*
         * If user provided a name for the callback function,
         * call it with passing the data from the found entry.
         */
        if (cb != NULL)
        {
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, &sflow_encap_info, user_data));
        }

        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_instru_sflow_virtual_register_set(
    int unit,
    dbal_fields_e field,
    int value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SFLOW, &entry_handle_id));

    /** Set the sampling rate field. */
    dbal_entry_value_field32_set(unit, entry_handle_id, field, INST_SINGLE, value);

    /** Commit dbal entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_instru_sflow_virtual_register_get(
    int unit,
    dbal_fields_e field,
    int *value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SFLOW, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        field, INST_SINGLE,
                                                        (uint32 *) value));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_sampling_rate_set(
    int unit,
    int sampling_rate)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_set(unit, DBAL_FIELD_SFLOW_SAMPLING_RATE, sampling_rate));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_sampling_rate_get(
    int unit,
    int *sampling_rate)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_get(unit, DBAL_FIELD_SFLOW_SAMPLING_RATE, sampling_rate));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
dnx_instru_sflow_uptime_set(
    int unit,
    int uptime)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_set(unit, DBAL_FIELD_SFLOW_UPTIME, uptime));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_uptime_get(
    int unit,
    int *uptime)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_get(unit, DBAL_FIELD_SFLOW_UPTIME, uptime));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_instru_sflow_agent_id_address_set(
    int unit,
    bcm_ip_t agent_ip_address)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.sflow_info.agent_ip_address.set(unit, agent_ip_address));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_agent_id_address_get(
    int unit,
    bcm_ip_t * agent_ip_address)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.sflow_info.agent_ip_address.get(unit, agent_ip_address));

exit:
    SHR_FUNC_EXIT;
}
