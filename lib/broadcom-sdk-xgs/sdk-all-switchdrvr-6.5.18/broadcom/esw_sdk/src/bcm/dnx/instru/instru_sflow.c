/** \file instru_sflow.c
 * $Id$
 *
 * Visibility procedures for DNX in instrumentation module.
 *
 * This file contains functions for sflow configuration.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/instru_access.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>

#include <bcm_int/dnx/instru/instru.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm/types.h>
#include <include/bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <include/bcm_int/dnx/lif/in_lif_profile.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>

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
 * \brief - Given an instru global lif, returns the local lif and it's result type.
 */
static shr_error_e
dnx_instru_sflow_encap_id_to_local_lif(
    int unit,
    int sflow_encap_id,
    int *local_lif,
    uint32 *result_type)
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
    *result_type = hw_res.outlif_dbal_result_type;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Verify sflow encap is indeed exists and it is with the correct table/result_type/phase
 */
static shr_error_e
dnx_instru_sflow_encap_id_verify(
    int unit,
    int sflow_encap_id)
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

    if ((hw_res.outlif_dbal_table_id != DBAL_TABLE_EEDB_SFLOW)
        || (hw_res.outlif_dbal_result_type >= DBAL_NOF_RESULT_TYPE_EEDB_SFLOW)
        || (hw_res.outlif_phase != LIF_MNGR_OUTLIF_PHASE_SFLOW))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! sFlow encap 0x%x is not of type sFlow, table_id = %s, result_type = %d, outlif_phase = %d\n",
                     sflow_encap_id, dbal_logical_table_to_string(unit, hw_res.outlif_dbal_table_id),
                     hw_res.outlif_dbal_result_type, hw_res.outlif_phase);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Verify sflow raw is indeed exists and it is with the correct table/result_type
 */
static shr_error_e
dnx_instru_sflow_raw_id_verify(
    int unit,
    int sflow_raw_id)
{
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    bcm_gport_t encap_in_tunnel;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Turn the global lif into a tunnel, then call gport to hw resources.
     */
    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    BCM_GPORT_TUNNEL_ID_SET(encap_in_tunnel, sflow_raw_id);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, encap_in_tunnel,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

    if ((hw_res.outlif_dbal_table_id != DBAL_TABLE_EEDB_DATA_ENTRY)
        || (hw_res.outlif_dbal_result_type != DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! sFlow encap 0x%x is not of type sFlow Data Entry, table_id = %s, result_type = %d\n",
                     sflow_raw_id, dbal_logical_table_to_string(unit, hw_res.outlif_dbal_table_id),
                     hw_res.outlif_dbal_result_type);
    }

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

    SHR_NULL_CHECK(sflow_encap_info, _SHR_E_PARAM, "sflow_encap_info");

    if (sflow_encap_info->flags &
        (~(BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE | BCM_INSTRU_SFLOW_ENCAP_AGGREGATED |
           BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR | BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flag: flags = 0x%08x. Supported flags are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_REPLACE = 0x%08x, "
                     "BCM_INSTRU_SFLOW_ENCAP_AGGREGATED = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW = 0x%08x\n",
                     sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                     BCM_INSTRU_SFLOW_ENCAP_AGGREGATED, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR,
                     BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW);
    }

    /** Verify DP (Data-Path sampling): */
    if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_AGGREGATED))
    {
        /** eventor_id is only relevant for header-sampling (eventor) */
        if (sflow_encap_info->eventor_id != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! eventor_id = %d is only relevant with flag BCM_INSTRU_SFLOW_ENCAP_AGGREGATED (0x%08x)! flags = 0x%08x",
                         sflow_encap_info->eventor_id, BCM_INSTRU_SFLOW_ENCAP_AGGREGATED, sflow_encap_info->flags);
        }
    }
    else
    {
        /** Verify aggregated header-sampling (eventor): */

        /** tunnel_id is only relevant for DP (Data-Path sampling) */
        if (sflow_encap_info->tunnel_id != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! tunnel_id = 0x%08x is only relevant when flag BCM_INSTRU_SFLOW_ENCAP_AGGREGATED (0x%08x) is OFF! flags = 0x%08x",
                         sflow_encap_info->tunnel_id, BCM_INSTRU_SFLOW_ENCAP_AGGREGATED, sflow_encap_info->flags);
        }
    }

    /** Verify replace case */
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        /** Verify that the flag WITH_ID is set */
        if (!(_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08X. flag BCM_INSTRU_SFLOW_ENCAP_REPLACE (0x%08X) is set - flag BCM_INSTRU_SFLOW_ENCAP_WITH_ID (0x%08X) must be set as well!!\n",
                         sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE, BCM_INSTRU_SFLOW_ENCAP_WITH_ID);
        }

        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_verify(unit, sflow_encap_info->sflow_encap_id));
    }
    else
    {
        /*
         * If it's not replace, verify that the maximum number of sFlow encaps wasn't exceeded.
         */
        if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR))
        {
            SHR_IF_ERR_EXIT(instru.sflow_info.nof_sflow_encaps.get(unit, &nof_sflow_encaps));

            if (nof_sflow_encaps == dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Can't create another sFlow instance. Maximum number has been exceeded: %d",
                             nof_sflow_encaps);
            }
        }
    }

    /** Verify EXTENDED INITIATOR case */
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR))
    {

        /*
         * The only relevant flags together with "EXTENDED INITIATOR" are "REPLACE" and "WITH_ID"
         */
        if (sflow_encap_info->flags &
            (~
             (BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE |
              BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! Unsupported flag: flags = 0x%08x. Supported flags for EXTENDED_INITIATOR are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_REPLACE = 0x%08x, "
                         "BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR = 0x%08x \n",
                         sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                         BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR);
        }

        /*
         * SFLOW DATAGRAM Header fields are not relevant for EXTENDED INITIATOR!
         * (are relevant for EXTENDED FLOW)
         */
        if ((sflow_encap_info->tunnel_id != 0) || (sflow_encap_info->eventor_id != 0)
            || (sflow_encap_info->sub_agent_id != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08x, tunnel_id = %d , eventor_id = %d, sub_agent_id = %d. Are not relevant with flag BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR (0x%08x)!",
                         sflow_encap_info->flags, sflow_encap_info->tunnel_id, sflow_encap_info->eventor_id,
                         sflow_encap_info->sub_agent_id, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR);
        }

        /*
         * CRPS counter fields are nor relevant for EXTENDED INITIATOR! 
         * (are relevant for EXTENDED FLOW)        
         */
        if ((sflow_encap_info->stat_cmd != 0) || (sflow_encap_info->counter_command_id != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08x, stat_cmd = %d , counter_command_id = %d. Are not relevant with flag BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR (0x%08x)!",
                         sflow_encap_info->flags, sflow_encap_info->stat_cmd, sflow_encap_info->counter_command_id,
                         BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR);
        }

    }

    /** Verify EXTENDED FLOW case */
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW))
    {

        /*
         * The only relevant flags together with "EXTENDED FLOW", "REPLACE" and "WITH_ID" and "AGGREGATED"
         */
        if (sflow_encap_info->flags &
            (~
             (BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE | BCM_INSTRU_SFLOW_ENCAP_AGGREGATED |
              BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! Unsupported flag: flags = 0x%08x. Supported flags for EXTENDED_FLOW are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_REPLACE = 0x%08x "
                         "BCM_INSTRU_SFLOW_ENCAP_AGGREGATED = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW = 0x%08x \n",
                         sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                         BCM_INSTRU_SFLOW_ENCAP_AGGREGATED, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW);
        }

        /*
         * Only AGGREGATED mode is supported
         */
        if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_AGGREGATED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! Unsupported option: flags = 0x%08x. EXTENDED_FLOW is only supported in AGGREGATED mode!\n",
                         sflow_encap_info->flags);
        }

        /** tunnel_id and sub_agent_id are irrelevant  */
        if ((sflow_encap_info->tunnel_id != 0) || (sflow_encap_info->sub_agent_id != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08x. tunnel_id = %d and sub_agent_id = %d are not relevant with flag BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW (0x%08x)!",
                         sflow_encap_info->flags, sflow_encap_info->tunnel_id, sflow_encap_info->sub_agent_id,
                         BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify sflow delete API params.
 */
static shr_error_e
dnx_instru_sflow_encap_delete_verify(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_info, _SHR_E_PARAM, "sflow_encap_info");

    /*
     * Verify that the sflow_encap_id is indeed allocated and it is EEDB SFLOW.
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_verify(unit, sflow_encap_info->sflow_encap_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate an sflow encap lif and increase the encaps counter.
 *
 */
static shr_error_e
dnx_instru_sflow_encap_flags_to_result_type(
    int unit,
    uint32 flags,
    uint32 *dbal_result_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Determine result type
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR))
    {
        *dbal_result_type = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST;
    }
    else if (_SHR_IS_FLAG_SET(flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW))
    {
        *dbal_result_type = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND;
    }
    else
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_INSTRU_SFLOW_ENCAP_AGGREGATED))
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP;
        }
        else
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP;
        }
    }

    SHR_EXIT();

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
 * \param [in] dbal_result_type - EEDB_SFLOW result_type.
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
    int *sflow_local_outlif,
    uint32 dbal_result_type)
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
        uint32 result_type;

        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                        (unit, sflow_encap_info->sflow_encap_id, sflow_local_outlif, &result_type));
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
    outlif_info.dbal_result_type = dbal_result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &global_lif_id, NULL, &outlif_info));

    /*
     * Increase the sFlow encaps counter.
     * NOF sflow is blocked by the NOF of mirror profiles in the system.
     * There is only one EXTENDED INITIATOR but there can be many EXTENDED FLOW, each one needs one CRPS time per one snooping. 
     */
    if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR))
    {
        SHR_IF_ERR_EXIT(instru.sflow_info.nof_sflow_encaps.inc(unit, 1));
    }

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
 * \param [in] dbal_result_type - EEDB_SFLOW result_type.
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
    int sflow_local_outlif,
    uint32 dbal_result_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Delete global and local lifs.
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, sflow_encap_id, NULL, sflow_local_outlif));

    /*
     * Decrease the sFlow encaps counter.
     * Note: 
     * NOF sflow is blocked by the NOF of mirror profiles in the system.
     * There is only one EXTENDED INITIATOR but there can be many EXTENDED FLOW, each one needs one CRPS time per one snooping. 
     * See allocate function.
     */
    if (dbal_result_type != DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST)
    {
        SHR_IF_ERR_EXIT(instru.sflow_info.nof_sflow_encaps.dec(unit, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify sflow Autonomous System Destination create API params.
 * 
 */
static shr_error_e
dnx_instru_sflow_encap_extended_dst_create_verify(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    uint32 ipv6_address[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_ext_dst_info, _SHR_E_PARAM, "sflow_encap_ext_dst_info");

    if (sflow_encap_ext_dst_info->flags &
        (~(BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE | BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flag: flags = 0x%08x. Supported flags are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_REPLACE = 0x%08x, "
                     "BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6 = 0x%08x\n",
                     sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                     BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6);
    }

    /** Verify replace case */
    if (_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        /** Verify that the flag WITH_ID is set */
        if (!(_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08X. flag BCM_INSTRU_SFLOW_ENCAP_REPLACE (0x%08X) is set - flag BCM_INSTRU_SFLOW_ENCAP_WITH_ID (0x%08X) must be set as well!!\n",
                         sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                         BCM_INSTRU_SFLOW_ENCAP_WITH_ID);
        }

        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_id_verify(unit, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id));
    }

    /*
     * Verify  Autonomous System path data and length:
     *   - length range is constant = 4.
     */
    if (sflow_encap_ext_dst_info->dst_as_path_length != BCM_INSTRU_SFLOW_ENCAP_EXTENDED_NOF_DSTS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! dst_as_path_length = %d. Only %d is supported!",
                     sflow_encap_ext_dst_info->dst_as_path_length, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_NOF_DSTS);

    }

    /*
     * Verify IP address
     */
    utilex_pp_ipv6_address_struct_to_long(sflow_encap_ext_dst_info->next_hop_ipv6_address, ipv6_address);

    if (!_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6))
    {
        /** IPv4 */
        if (sflow_encap_ext_dst_info->next_hop_ipv4_address == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! flags = 0x%08x. IPv4 format but next_hop_ipv4_address = 0!",
                         sflow_encap_ext_dst_info->flags);
        }

        if ((ipv6_address[0] != 0) || (ipv6_address[1] != 0) || (ipv6_address[2] != 0) || (ipv6_address[3] != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08x. IPv4 format but next_hop_ipv6_address = [0x%08x, 0x%08x, 0x%08x, 0x%08x] is not 0!",
                         sflow_encap_ext_dst_info->flags, ipv6_address[0], ipv6_address[1], ipv6_address[2],
                         ipv6_address[3]);
        }
    }
    else
    {
        /** IPv6 */
        if ((ipv6_address[0] == 0) && (ipv6_address[1] == 0) && (ipv6_address[2] == 0) && (ipv6_address[3] == 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! flags = 0x%08x. IPv6 format but next_hop_ipv6_address = 0!",
                         sflow_encap_ext_dst_info->flags);
        }

        if (sflow_encap_ext_dst_info->next_hop_ipv4_address != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! flags = 0x%08x. Thus, IPv6 format but next_hop_ipv4_address = 0x%08x!",
                         sflow_encap_ext_dst_info->flags, sflow_encap_ext_dst_info->next_hop_ipv4_address);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify sflow Autonomous System Destination get/delete API params.
 * 
 */
static shr_error_e
dnx_instru_sflow_encap_extended_dst_get_delete_verify(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_ext_dst_info, _SHR_E_PARAM, "sflow_encap_ext_dst_info");

    /*
     * Verify that the sflow_encap_extended_dst_id is indeed allocated and it is EEDB DATA ENTRY.
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_id_verify(unit, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify sflow Autonomous System source create API params.
 * 
 */
static shr_error_e
dnx_instru_sflow_encap_extended_src_create_verify(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_ext_src_info, _SHR_E_PARAM, "sflow_encap_ext_src_info");

    if (sflow_encap_ext_src_info->flags & (~(BCM_INSTRU_SFLOW_ENCAP_WITH_ID | BCM_INSTRU_SFLOW_ENCAP_REPLACE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flag: flags = 0x%08x. Supported flags are: BCM_INSTRU_SFLOW_ENCAP_WITH_ID = 0x%08x, BCM_INSTRU_SFLOW_ENCAP_REPLACE = 0x%08x \n",
                     sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID, BCM_INSTRU_SFLOW_ENCAP_REPLACE);
    }

    /** Verify replace case */
    if (_SHR_IS_FLAG_SET(sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        /** Verify that the flag WITH_ID is set */
        if (!(_SHR_IS_FLAG_SET(sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08X. flag BCM_INSTRU_SFLOW_ENCAP_REPLACE (0x%08X) is set - flag BCM_INSTRU_SFLOW_ENCAP_WITH_ID (0x%08X) must be set as well!!\n",
                         sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE,
                         BCM_INSTRU_SFLOW_ENCAP_WITH_ID);
        }

        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_id_verify(unit, sflow_encap_ext_src_info->sflow_encap_extended_src_id));
    }

    /** Verify Autonomous system number of source */
    if (sflow_encap_ext_src_info->src_as == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! src_as = 0.\n");
    }

    /** Verify Autonomous system number of source peer */
    if (sflow_encap_ext_src_info->src_as_peer == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! src_as_peer = 0.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify sflow Autonomous System source get/delete API params.
 * 
 */
static shr_error_e
dnx_instru_sflow_encap_extended_src_get_delete_verify(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sflow_encap_ext_src_info, _SHR_E_PARAM, "sflow_encap_ext_src_info");

    /*
     * Verify that the sflow_encap_extended_dst_id is indeed allocated and it is EEDB DATA ENTRY.
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_id_verify(unit, sflow_encap_ext_src_info->sflow_encap_extended_src_id));

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
    destination.eventor_id = sflow_encap_info->eventor_id;

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

        SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.exchange(unit, 0, &destination,
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
        SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.allocate_single(unit, 0,
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

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.free_single(unit,
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
 * \brief - Allocate one SFLOW DATA Entry ("raw") lif.
 *
 */
static shr_error_e
dnx_instru_sflow_raw_lif_alloc(
    int unit,
    lif_mngr_outlif_phase_e outlif_phase,
    int lif_alloc_flags,
    int *local_outlif,
    int *global_lif)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA;

    outlif_info.outlif_phase = outlif_phase;

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, global_lif, NULL, &outlif_info));

    *local_outlif = outlif_info.local_outlif;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free one SFLWO DATA Entry ("raw") lif.
 *
 */
static shr_error_e
dnx_instru_sflow_raw_lif_free(
    int unit,
    int local_outlif,
    int global_lif)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, global_lif, NULL, local_outlif));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears the data of one SFLOW DATA Entry ("raw") lif. 
 *
 */
static shr_error_e
dnx_instru_sflow_raw_lif_data_clear(
    int unit,
    int local_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    /** Clear dbal entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate the raw lifs for a header sampling destination profile.
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
dnx_instru_sflow_header_sampling_raw_lifs_allocate(
    int unit,
    int sflow_destination_profile)
{
    int local_outlif;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_alloc
                    (unit, LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, &local_outlif, NULL));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile, 0, local_outlif));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_alloc
                    (unit, LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, &local_outlif, NULL));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile, 1, local_outlif));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free the raw lifs for a header sampling destination profile.
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
dnx_instru_sflow_header_sampling_raw_lifs_free(
    int unit,
    int sflow_destination_profile)
{
    int local_outlif;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 0, &local_outlif));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_free(unit, local_outlif, LIF_MNGR_INVALID));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile, 0, 0));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile, 1, &local_outlif));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_free(unit, local_outlif, LIF_MNGR_INVALID));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.set(unit, sflow_destination_profile, 1, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete header sampling raw data etps entries.
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
dnx_instru_sflow_header_sampling_raw_lifs_data_clear(
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
 * \brief - set raw data lif
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_info - Encap info for these raw data lifs.
 * \param [in] sflow_destination_profile - the sFlow destination profile for these lifs.
 * \param [in] dbal_result_type - the lif entry result type.
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
    int sflow_destination_profile,
    uint32 dbal_result_type)
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
     * Entry data is different in case of DP or eventor
     */
    if (dbal_result_type == DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP)
    {
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
    }
    else
    {
        entry_data[0] = entry_data[1] = entry_data[2] = 0;
        entry_data[3] = sflow_encap_info->eventor_id << 24;
    }

    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

    /*
     * Tunnel encap pointer is needed only in case of DP
     */
    if (dbal_result_type == DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, sflow_encap_info->tunnel_id,
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));
    }

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
 * \param [in] eventor_id - the Eventor ID.
 * \param [in] dbal_result_type - the EEDB SFLOW result type.
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
    uint32 sflow_destination_profile,
    uint16 eventor_id,
    uint32 dbal_result_type)
{
    uint32 entry_handle_id;
    uint32 entry_data[3];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, sflow_local_outlif);

    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);

    switch (dbal_result_type)
    {
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP:
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP:
        {
            int next_outlif;

            SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.get(unit, sflow_destination_profile,
                                                                         0, &next_outlif));

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE,
                                         INSTRU_SFLOW_OAM_LIF_SET);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                         next_outlif);

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

            break;
        }

        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST:
        {
            /*
             * Set entry raw data.
             * entry_data[0] : const 0.
             * entry_data[1] : const 1 = data format.
             * entry_data[2] : const 1 = protocol ETH.
             *
             */
            entry_data[0] = 0;
            entry_data[1] = 1;
            entry_data[2] = 1;

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE,
                                         INSTRU_SFLOW_OAM_LIF_SET);

            /*
             * Set entry raw data.
             * entry_data[0] : const 0.
             * entry_data[1] : const 0.
             * entry_data[2] : eventor_id (MSBs).
             * Note: this entry is TOS of 2nd pass - only used for context setup and CRPS counter. 
             */
            entry_data[0] = 0;
            entry_data[1] = 0;
            entry_data[2] = eventor_id << 24;

            break;
        }
        default:
        {

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Unsupported result type: dbal_result_type = %d. Supported result_type are: DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP = %d, DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP = %d, "
                         "DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST = %d, DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND = %d\n",
                         dbal_result_type, DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP,
                         DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP,
                         DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST,
                         DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND);
        }
    }

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

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.profile_data_get(unit,
                                                                                 sflow_destination_profile, NULL,
                                                                                 &destination_key));

    sflow_encap_info->tunnel_id = destination_key.udp_tunnel;
    sflow_encap_info->sub_agent_id = destination_key.sub_agent_id;
    sflow_encap_info->eventor_id = destination_key.eventor_id;

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
 * \brief - Clear sflow EEDB entry
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_local_outlif - the sflow lif id, key to the dbal table
 * \param [in] result_type - the sflow EEDB entry result type
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
    int sflow_local_outlif,
    uint32 result_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, sflow_local_outlif);
    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    /** Clear dbal entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create an sflow entity for Header sampling - aggregated/non-aggregated..
 * Sflow ETPS points to a raw data ETPS, which points to a udp ETPS entry in the EEDB
 *
 */
int
dnx_instru_sflow_encap_header_sampling_create(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int sflow_global_lif = 0;
    int sflow_destination_profile = 0;
    int sflow_local_outlif = 0;
    int write_destination_profile, old_sflow_destination_profile, delete_old_destination_profile;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_flags_to_result_type(unit, sflow_encap_info->flags, &result_type));

    /*
     * Allocate all resource
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_allocate(unit, sflow_encap_info, &sflow_local_outlif, result_type));
    sflow_global_lif = sflow_encap_info->sflow_encap_id;

    SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_profile_allocate
                    (unit, sflow_encap_info, &sflow_destination_profile, &write_destination_profile,
                     &old_sflow_destination_profile, &delete_old_destination_profile));

    if (write_destination_profile && (sflow_destination_profile != old_sflow_destination_profile))
    {
        /*
         * We need to allocate new raw lifs if the udp profile is new, but not if we replace the content of
         * an existing profile.
         */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_header_sampling_raw_lifs_allocate(unit, sflow_destination_profile));
    }

    /*
     * Write to HW.
     */
    if (write_destination_profile)
    {
        /*
         * Only write the raw lifs if it's a new UDP profile, or same profile with new content.
         */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_data_lifs_write
                        (unit, sflow_encap_info, sflow_destination_profile, result_type));
    }

    if (sflow_destination_profile != old_sflow_destination_profile)
    {
        /*
         * The only information stored by the sFlow encap is the pointer to the udp tunnel,
         * so if it didn't change then no need to update it.
         */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_write
                        (unit, sflow_local_outlif, sflow_destination_profile, sflow_encap_info->eventor_id,
                         result_type));
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
        SHR_IF_ERR_EXIT(dnx_instru_sflow_header_sampling_raw_lifs_data_clear(unit, old_sflow_destination_profile));

        SHR_IF_ERR_EXIT(dnx_instru_sflow_header_sampling_raw_lifs_free(unit, sflow_destination_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create an sflow extended entity. 
 *
 */
static shr_error_e
dnx_instru_sflow_encap_extended_create(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    int sflow_destination_profile = 0;
    int sflow_local_outlif = 0;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_flags_to_result_type(unit, sflow_encap_info->flags, &result_type));

    /*
     * Allocate SFLOW EEDB resource
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_allocate(unit, sflow_encap_info, &sflow_local_outlif, result_type));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_write
                    (unit, sflow_local_outlif, sflow_destination_profile, sflow_encap_info->eventor_id, result_type));

    /*
     * For Extended SFLOW 2nd pass:
     *  - Update CRPS counter with the SFLOW EEDB
     *  - Save relevant info in DB
     */
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW))
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_add_oam_lif_db(unit, sflow_encap_info, sflow_local_outlif));
    }

    if (!_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        /**
         * No need to write to GLEM if we perform replace since the global and local lif don't change.
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                        (unit, _SHR_CORE_ALL, sflow_encap_info->sflow_encap_id, sflow_local_outlif));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create an sflow entity.
 *
 * \param [in] unit - relevant unit
 * \param [in] sflow_encap_info - A pointer to sflow struct:
 *                                   flags -
 *                                          BCM_INSTRU_SFLOW_ENCAP_WITH_ID - sflow encap id given
 *                                          BCM_INSTRU_SFLOW_ENCAP_REPLACE - replace an existing sflow lif. No need to update glem
 *                                          BCM_INSTRU_SFLOW_ENCAP_AGGREGATED - aggregated samples (using eventor)
 *                                          BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR - Sflow IPoETH sampling initator
 *                                          BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW - Sflow IPoETH sampling per flow
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
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_create_verify(unit, sflow_encap_info));

    /*
     * For SFLOW Extended - only EEDB should be allocated.
     */
    if (_SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR) ||
        _SHR_IS_FLAG_SET(sflow_encap_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW))
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_create(unit, sflow_encap_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_header_sampling_create(unit, sflow_encap_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
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
    int local_lif;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_delete_verify(unit, sflow_encap_info));

    /**
     * Clear HW.
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_info->sflow_encap_id, &local_lif, &result_type));

    switch (result_type)
    {
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP:
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP:
        {
            int sflow_destination_profile, delete_destination_profile = -1;

            /** Remove global lif from GLEM */
            SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, sflow_encap_info->sflow_encap_id));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_delete(unit, local_lif));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_clear(unit, local_lif, result_type));

            /*
             * Free sFlow destination profile, and if necessary, free and clear raw outlifs.
             */
            SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_profile_free
                            (unit, sflow_encap_info, &sflow_destination_profile, &delete_destination_profile));

            if (delete_destination_profile)
            {
                SHR_IF_ERR_EXIT(dnx_instru_sflow_header_sampling_raw_lifs_data_clear(unit, sflow_destination_profile));

                SHR_IF_ERR_EXIT(dnx_instru_sflow_header_sampling_raw_lifs_free(unit, sflow_destination_profile));
            }

            /*
             * Free sFlow encap outlif.
             */
            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_free
                            (unit, sflow_encap_info->sflow_encap_id, local_lif, result_type));

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST:
        {
            /** Remove global lif from GLEM */
            SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, sflow_encap_info->sflow_encap_id));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_clear(unit, local_lif, result_type));

            /*
             * Free sFlow encap outlif.
             */
            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_free
                            (unit, sflow_encap_info->sflow_encap_id, local_lif, result_type));

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND:
        {
            /** Remove global lif from GLEM */
            SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, sflow_encap_info->sflow_encap_id));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_delete(unit, local_lif));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_clear(unit, local_lif, result_type));

            /*
             * Free sFlow encap outlif.
             */
            SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_free
                            (unit, sflow_encap_info->sflow_encap_id, local_lif, result_type));

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! sflow_encap_id = 0x%08x has unsupported result type: result_type = %d! \n",
                         sflow_encap_info->sflow_encap_id, result_type);
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get an sflow entity data, based on the local lif and result type
 *
 */
static shr_error_e
dnx_instru_sflow_encap_get(
    int unit,
    int local_lif,
    uint32 result_type,
    bcm_instru_sflow_encap_info_t * sflow_encap_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (result_type)
    {
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_DP:
        {
            sflow_encap_info->flags = 0;

            SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_get(unit, sflow_encap_info, local_lif));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_get(unit, sflow_encap_info));

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_HDR_SAMP:
        {
            sflow_encap_info->flags = BCM_INSTRU_SFLOW_ENCAP_AGGREGATED;

            SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_get(unit, sflow_encap_info, local_lif));

            SHR_IF_ERR_EXIT(dnx_instru_sflow_destination_get(unit, sflow_encap_info));

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_FIRST:
        {
            sflow_encap_info->flags = BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR;
            sflow_encap_info->tunnel_id = 0;
            sflow_encap_info->sub_agent_id = 0;
            sflow_encap_info->eventor_id = 0;
            sflow_encap_info->stat_cmd = 0;
            sflow_encap_info->counter_command_id = 0;

            break;
        }
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND:
        {
            uint32 entry_handle_id;
            uint32 entry_data[4];

            sflow_encap_info->flags = BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW | BCM_INSTRU_SFLOW_ENCAP_AGGREGATED;

            SHR_IF_ERR_EXIT(dnx_instru_sflow_oam_lif_db_get(unit, sflow_encap_info, local_lif));

            /** Get Eventor ID from the Entry */

            /** Take DBAL handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));

            /** Set KEY field */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);

            /** Set RESULT_TYPE field */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

            dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, entry_data);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            /*
             * Set entry raw data.
             * entry_data[0] : const 0.
             * entry_data[1] : const 0.
             * entry_data[2] : eventor_id (MSBs).
             */
            sflow_encap_info->eventor_id = (entry_data[2] >> 24);

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! sflow_encap_id = 0x%08x has unsupported result type: result_type = %d! \n",
                         sflow_encap_info->sflow_encap_id, result_type);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
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
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Check if entry exists in GLEM*/
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_info->sflow_encap_id, &local_lif, &result_type));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_get(unit, local_lif, result_type, sflow_encap_info));

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

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_instru_sflow_encap_info_t sflow_encap_info;
        uint32 result_type, local_out_lif;
        bcm_gport_t gport;

        sal_memset(&sflow_encap_info, 0, sizeof(bcm_instru_sflow_encap_info_t));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                                                INST_SINGLE, &result_type));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &local_out_lif));

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                    _SHR_CORE_ALL, local_out_lif, &gport));

        sflow_encap_info.sflow_encap_id = BCM_GPORT_TUNNEL_ID_GET(gport);

        /** Get Sflow info */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_get(unit, local_out_lif, result_type, &sflow_encap_info));

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

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field, INST_SINGLE, (uint32 *) value));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_sampling_rate_set(
    int unit,
    int sampling_rate)
{
    uint32 prob_max_val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sampling rate range:
     *   1. If the sflow mode of operation is non-aggregated (without Eventor): 
     *           There is no limitation.
     *   2. If the sflow mode of operation is aggregated (using Eventor):
     *           The limitation is ~30Gbps --> 1/200 of line-rate.
     *           Typical sampling rate should be 1:1000 - 1:5000.
     *
     * The sampling rate is used for setting the mirroring, thus check mirroring range.
     * See bcm_dnx_mirror_destination_create.
     */
    prob_max_val = dnx_data_snif.ingress.prob_max_val_get(unit);

    if ((sampling_rate < 1) || (sampling_rate > prob_max_val))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "sampling_rate = %d is out of range [1:%d]!\n", sampling_rate, prob_max_val);
    }

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
dnx_instru_sflow_myrouter_as_number_set(
    int unit,
    int as_number)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_set(unit, DBAL_FIELD_SFLOW_MYROUTER_AS_NUMBER, as_number));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_sflow_myrouter_as_number_get(
    int unit,
    int *as_number)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_sflow_virtual_register_get(unit, DBAL_FIELD_SFLOW_MYROUTER_AS_NUMBER, as_number));

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

/**
 * \brief - set extended dst raw data lif
 */
static shr_error_e
dnx_instru_sflow_encap_extended_dst_raw_data_lif_write(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info,
    lif_mngr_outlif_phase_e outlif_phase,
    int current_local_outlif,
    int next_local_lif)
{
    uint32 entry_handle_id;
    uint32 entry_data[4];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    switch (outlif_phase)
    {
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1:
        {
            /*
             * Set entry raw data.
             * entry_data[0] : dst_as_path[0].
             * entry_data[1] : dst_as_path[1].
             * entry_data[2] : dst_as_path[2].
             * entry_data[3] : dst_as_path[3].
             * Note: invalid entries are set to 0, see verification.
             */
            entry_data[0] = sflow_encap_ext_dst_info->dst_as_path[0];
            entry_data[1] = sflow_encap_ext_dst_info->dst_as_path[1];
            entry_data[2] = sflow_encap_ext_dst_info->dst_as_path[2];
            entry_data[3] = sflow_encap_ext_dst_info->dst_as_path[3];

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2:
        {

            /*
             * Set entry raw data.
             * entry_data[0] : const 2 = AS-Path-Segment-Type.
             * entry_data[1] : const 4 = AS-List-Length.
             * entry_data[2] : const 1 = Num-of-Dest-Paths.
             * entry_data[3] : 0.
             * Note:
             * AS-List-Length is set constantly to 4 even if the user set it to less since the record contains 4 entries.
             * If the user set it to less, the invalid entries should be with zero value. See verification.
             *
             */
            entry_data[0] = 2;
            entry_data[1] = 4;
            entry_data[2] = 1;
            entry_data[3] = 0;

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_3:
        {

            /*
             * Set entry raw data.
             * entry_data[0] : Next-Hop-IPv6/4[31:0].
             * entry_data[1] : Next-Hop-IPv6[63:32].
             * entry_data[2] : Next-Hop-IPv6[95:64]. 
             * entry_data[3] : Next-Hop-IPv6[127:96].             
             *
             */
            if (!_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6))
            {
                entry_data[0] = sflow_encap_ext_dst_info->next_hop_ipv4_address;
                entry_data[1] = 0;
                entry_data[2] = 0;
                entry_data[3] = 0;
            }
            else
            {
                utilex_pp_ipv6_address_struct_to_long(sflow_encap_ext_dst_info->next_hop_ipv6_address, entry_data);
            }

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_4:
        {

            /*
             * Set entry raw data.
             * entry_data[0] : Record-Length (Extended Gateway record length = 56).
             * entry_data[1] : Next-Hop-IP-Type (unknown = 0, IPv4 =1, IPv6 = 2).
             * entry_data[2] : Data-Format2 (= 1003).
             * entry_data[3] : Num-of-Records (=2, Header sample + Extended Gateway data).
             */
            entry_data[0] = 56;

            if (!_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6))
            {
                /** IPv4 */
                entry_data[1] = 1;
            }
            else
            {
                /** IPv6 */
                entry_data[1] = 2;
            }
            entry_data[2] = 1003;
            entry_data[3] = 2;

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5:
        {

            /*
             * Set entry raw data.
             * entry_data[0] : Data-Format1 (= 1).
             * entry_data[1] : Length (total sample langth = 248
             * entry_data[2] : 0.
             * entry_data[3] : 0.
             */
            entry_data[0] = 1;
            entry_data[1] = 248;
            entry_data[2] = 0;
            entry_data[3] = 0;

            break;
        }
        default:
        {

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Unsupported SFLOW Extended Destination RAW pahse: outlif_phase = %d. \n",
                         outlif_phase);
        }
    }

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_local_outlif);
    /** Data fields*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);

    /** For REPLACE, need to update only the data */
    if (!_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     next_local_lif);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 0x3);

    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the extended dst next outlif from the raw lif
 */
static shr_error_e
dnx_instru_sflow_encap_extended_dst_raw_data_lif_next_outlif_get(
    int unit,
    int current_local_outlif,
    int *next_local_outlif)
{
    uint32 entry_handle_id;
    uint32 next_outlif_ptr;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_local_outlif);
    /** Data fields*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    /** Get next_outlif_ptr */
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                   &next_outlif_ptr);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *next_local_outlif = (int) next_outlif_ptr;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the extended dst data from the raw lif
 */
static shr_error_e
dnx_instru_sflow_encap_extended_dst_raw_data_lif_get(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info,
    lif_mngr_outlif_phase_e outlif_phase,
    int current_local_outlif)
{
    uint32 entry_handle_id;
    uint32 entry_data[4];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, current_local_outlif);
    /** Data fields*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    /** Get Sflow data */
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    switch (outlif_phase)
    {
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1:
        {
            /*
             * Get entry raw data.
             * entry_data[0] : dst_as_path[0].
             * entry_data[1] : dst_as_path[1].
             * entry_data[2] : dst_as_path[2].
             * entry_data[3] : dst_as_path[3].
             */
            sflow_encap_ext_dst_info->dst_as_path[0] = entry_data[0];
            sflow_encap_ext_dst_info->dst_as_path[1] = entry_data[1];
            sflow_encap_ext_dst_info->dst_as_path[2] = entry_data[2];
            sflow_encap_ext_dst_info->dst_as_path[3] = entry_data[3];

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2:
        {

            /*
             * Get entry raw data.
             * entry_data[0] : const 2 = AS-Path-Segment-Type.
             * entry_data[1] : const 4 = AS-List-Length.
             * entry_data[2] : const 1 = Num-of-Dest-Paths.
             * entry_data[3] : 0.
             *
             */
            sflow_encap_ext_dst_info->dst_as_path_length = 4;

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_3:
        {

            /*
             * Set entry raw data.
             * entry_data[0] : Next-Hop-IPv6/4[31:0].
             * entry_data[1] : Next-Hop-IPv6[63:32].
             * entry_data[2] : Next-Hop-IPv6[95:64]. 
             * entry_data[3] : Next-Hop-IPv6[127:96].             
             *
             * Note:
             * Updating here both IPv4 and IPv6 values. On next step, when reading the format type, will zero the one that was not set.
             */
            sflow_encap_ext_dst_info->next_hop_ipv4_address = entry_data[0];
            utilex_pp_ipv6_address_long_to_struct(entry_data, sflow_encap_ext_dst_info->next_hop_ipv6_address);

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_4:
        {

            /*
             * Set entry raw data.
             * entry_data[0] : Record-Length (Extended Gateway record length = 56).
             * entry_data[1] : Next-Hop-IP-Type (unknown = 0, IPv4 =1, IPv6 = 2).
             * entry_data[2] : Data-Format2 (= 1003).
             * entry_data[3] : Num-of-Records (=2, Header sample + Extended Gateway data).
             * Note:
             * based on Next-Hop-IP-Type (IPv4 or IPv6), zero the one that was not set.
             */
            if (entry_data[1] == 1)
            {
                /** It is IPv4, zero the IPv6  */
                entry_data[0] = entry_data[1] = entry_data[2] = entry_data[3] = 0;
                utilex_pp_ipv6_address_long_to_struct(entry_data, sflow_encap_ext_dst_info->next_hop_ipv6_address);
            }
            else if (entry_data[1] == 2)
            {
                /** It is IPv6, zero the IPv4 */
                sflow_encap_ext_dst_info->next_hop_ipv4_address = 0;

                sflow_encap_ext_dst_info->flags = BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Error! reading sFlow encap 0x%x, Next-Hop-IP-Type = %d! Valid values are IPv4(=1) and IPv6(=2).\n",
                             sflow_encap_ext_dst_info->sflow_encap_extended_dst_id, entry_data[1]);
            }

            break;
        }
        case LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5:
        {

            /*
             * Set entry raw data.
             * entry_data[0] : Data-Format1 (= 1).
             * entry_data[1] : Length (total sample langth = 248
             * entry_data[2] : 0.
             * entry_data[3] : 0.
             */

            break;
        }
        default:
        {

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Unsupported SFLOW Extended Destination RAW pahse: outlif_phase = %d. \n",
                         outlif_phase);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dnx_instru_sflow_encap_extended_dst_create(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    int current_local_outlif, next_local_outlif;
    int outlif_phase;
    int lif_alloc_flags;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Start to allocate RAW entries from last-to-first:
     * 1. Loop from LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5 to LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2:
     *      - allocate without global lif allocation.
     *      - set entry data.
     * 2. LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1:
     *      - allocate with global lif allocation.
     *      - set entry data.
     *      - update GLEM.
     */
    lif_alloc_flags = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
    next_local_outlif = 0;
    for (outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5; outlif_phase > LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1;
         outlif_phase--)
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_alloc
                        (unit, outlif_phase, lif_alloc_flags, &current_local_outlif, NULL));

        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_write
                        (unit, sflow_encap_ext_dst_info, outlif_phase, current_local_outlif, next_local_outlif));

        next_local_outlif = current_local_outlif;
    }

    /** LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1: */
    if (_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID))
    {
        lif_alloc_flags = LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }
    else
    {
        lif_alloc_flags = 0;
    }
    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_alloc
                    (unit, LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1, lif_alloc_flags, &current_local_outlif,
                     &(sflow_encap_ext_dst_info->sflow_encap_extended_dst_id)));

    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_write
                    (unit, sflow_encap_ext_dst_info, LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1, current_local_outlif,
                     next_local_outlif));

    SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                    (unit, _SHR_CORE_ALL, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id, current_local_outlif));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_instru_sflow_encap_extended_dst_replace(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    int current_local_outlif, next_local_outlif;
    int outlif_phase;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    /** Get local lif from algo gpm. */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id, &current_local_outlif, &result_type));

    /*
     * Loop from first-to-last:
     *      - get next outlif pointer.
     *      - set entry data.
     */
    for (outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1; outlif_phase <= LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5;
         outlif_phase++)
    {
        /** Get next outlif */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_next_outlif_get
                        (unit, current_local_outlif, &next_local_outlif));

        /** Update entry with new data */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_write
                        (unit, sflow_encap_ext_dst_info, outlif_phase, current_local_outlif, next_local_outlif));

        current_local_outlif = next_local_outlif;
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_dst_create(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_dst_create_verify(unit, sflow_encap_ext_dst_info));

    if (_SHR_IS_FLAG_SET(sflow_encap_ext_dst_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_replace(unit, sflow_encap_ext_dst_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_create(unit, sflow_encap_ext_dst_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_dst_delete(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    int current_local_outlif, next_local_outlif;
    int outlif_phase;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_dst_get_delete_verify(unit, sflow_encap_ext_dst_info));

    /** Get local lif from algo gpm. */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id, &current_local_outlif, &result_type));

    /** Remove global lif from GLEM */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem
                    (unit, _SHR_CORE_ALL, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id));

    /*
     * Loop on all:
     *  - get next outlif pointer.
     *  - clear the EEDB entry
     *  - deallocate the LIF 
     */
    for (outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1; outlif_phase <= LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5;
         outlif_phase++)
    {
        /** Get next outlif */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_next_outlif_get
                        (unit, current_local_outlif, &next_local_outlif));

        /** Clears entry data */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_data_clear(unit, current_local_outlif));

        /*
         * Delete the local lif.
         */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_free(unit, current_local_outlif, LIF_MNGR_INVALID));

        current_local_outlif = next_local_outlif;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_dst_get(
    int unit,
    bcm_instru_sflow_encap_extended_dst_info_t * sflow_encap_ext_dst_info)
{
    int current_local_outlif, next_local_outlif;
    int outlif_phase;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_dst_get_delete_verify(unit, sflow_encap_ext_dst_info));

    /** Get local lif from algo gpm. */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_ext_dst_info->sflow_encap_extended_dst_id, &current_local_outlif, &result_type));

    /*
     * Loop on all:
     *  - Get next outlif pointer.
     *  - Get the EEDB entry data
     */
    for (outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1; outlif_phase <= LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5;
         outlif_phase++)
    {
        /** Get next outlif */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_next_outlif_get
                        (unit, current_local_outlif, &next_local_outlif));

        /** Get the entry data */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_dst_raw_data_lif_get
                        (unit, sflow_encap_ext_dst_info, outlif_phase, current_local_outlif));

        current_local_outlif = next_local_outlif;
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_dst_traverse(
    int unit,
    bcm_instru_sflow_encap_extended_dst_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

/**
 * \brief - set extended source raw data lif
 */
static shr_error_e
dnx_instru_sflow_encap_extended_src_raw_data_lif_write(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info,
    int local_outlif)
{
    uint32 entry_handle_id;
    uint32 entry_data[4];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    /*
     * Set entry raw data.
     * entry_data[0] : AS-Source-Number.
     * entry_data[1] : AS-Number-of-Source-Peer.
     * entry_data[2] : 0.
     * entry_data[3] : 0.
     */
    entry_data[0] = sflow_encap_ext_src_info->src_as;
    entry_data[1] = sflow_encap_ext_src_info->src_as_peer;
    entry_data[2] = 0;
    entry_data[3] = 0;

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);
    /** Data fields*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);

    /** For REPLACE, need to update only the data */
    if (!_SHR_IS_FLAG_SET(sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, 0);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE, 0x3);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get extended source raw data lif
 */
static shr_error_e
dnx_instru_sflow_encap_extended_src_raw_data_lif_get(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info,
    int local_outlif)
{
    uint32 entry_handle_id;
    uint32 entry_data[4];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);
    /** Data fields*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SFLOW_RAW_DATA);

    /** Get Sflow data */
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_SFLOW_DATA_EMPTY, INST_SINGLE, entry_data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Get entry raw data.
     * entry_data[0] : AS-Source-Number.
     * entry_data[1] : AS-Number-of-Source-Peer.
     * entry_data[2] : 0.
     * entry_data[3] : 0.
     */
    sflow_encap_ext_src_info->src_as = entry_data[0];
    sflow_encap_ext_src_info->src_as_peer = entry_data[1];

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_src_create(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info)
{
    int local_outlif;
    int lif_alloc_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_src_create_verify(unit, sflow_encap_ext_src_info));

    if (_SHR_IS_FLAG_SET(sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_REPLACE))
    {
        uint32 result_type;

        /** Get local lif from algo gpm. */
        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                        (unit, sflow_encap_ext_src_info->sflow_encap_extended_src_id, &local_outlif, &result_type));

        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_src_raw_data_lif_write
                        (unit, sflow_encap_ext_src_info, local_outlif));
    }
    else
    {
        /** Allocate data entry */
        if (_SHR_IS_FLAG_SET(sflow_encap_ext_src_info->flags, BCM_INSTRU_SFLOW_ENCAP_WITH_ID))
        {
            lif_alloc_flags = LIF_MNGR_GLOBAL_LIF_WITH_ID;
        }
        else
        {
            lif_alloc_flags = 0;
        }

        SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_alloc
                        (unit, LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_6, lif_alloc_flags, &local_outlif,
                         &(sflow_encap_ext_src_info->sflow_encap_extended_src_id)));

        SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_src_raw_data_lif_write
                        (unit, sflow_encap_ext_src_info, local_outlif));

        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                        (unit, _SHR_CORE_ALL, sflow_encap_ext_src_info->sflow_encap_extended_src_id, local_outlif));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_src_delete(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info)
{
    int current_local_outlif;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_src_get_delete_verify(unit, sflow_encap_ext_src_info));

    /** Get local lif from algo gpm. */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_ext_src_info->sflow_encap_extended_src_id, &current_local_outlif, &result_type));

    /** Remove global lif from GLEM */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem
                    (unit, _SHR_CORE_ALL, sflow_encap_ext_src_info->sflow_encap_extended_src_id));

    /** Clears entry data */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_data_clear(unit, current_local_outlif));

    /*
     * Delete global and local lif.
     */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_raw_lif_free
                    (unit, current_local_outlif, sflow_encap_ext_src_info->sflow_encap_extended_src_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_src_get(
    int unit,
    bcm_instru_sflow_encap_extended_src_info_t * sflow_encap_ext_src_info)
{
    int current_local_outlif;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_sflow_encap_extended_src_get_delete_verify(unit, sflow_encap_ext_src_info));

    /** Get local lif from algo gpm. */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_id_to_local_lif
                    (unit, sflow_encap_ext_src_info->sflow_encap_extended_src_id, &current_local_outlif, &result_type));

    /** Get entry data */
    SHR_IF_ERR_EXIT(dnx_instru_sflow_encap_extended_src_raw_data_lif_get
                    (unit, sflow_encap_ext_src_info, current_local_outlif));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_src_traverse(
    int unit,
    bcm_instru_sflow_encap_extended_src_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
