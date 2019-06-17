/** \file l2_egress.c
 * $Id$
 *
 * L2 procedures for DNX.
 *
 * This file contains functions for managing l2 egress APIs.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/mdb.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm/types.h>
#include <bcm/l2.h>
#include <bcm_int/dnx/vlan/vlan.h>

/*
 * }
 */

/**
 * \brief
 * Verify l2 egress parameters for BCM-API: bcm_dnx_l2_egress_create().
 * check the supported flags
 */
static shr_error_e
dnx_l2_egress_create_verify(
    int unit,
    bcm_l2_egress_t * egr)
{
    uint32 supported_flags, header_flags;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(egr, _SHR_E_PARAM, "egr");
    supported_flags =
        BCM_L2_EGRESS_WITH_ID | BCM_L2_EGRESS_REPLACE | BCM_L2_EGRESS_RECYCLE_HEADER |
        BCM_L2_EGRESS_RECYCLE_INJECTED_2_PP_HEADER;

    /** Check if the set flags are supported*/
    if (egr->flags & ~supported_flags)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "The used flag = 0x%08X is not in the range of supported ones", egr->flags);
    }
    /** Verify WITH_ID and REPLACE flags */
    if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_REPLACE) && !_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_L2_EGRESS_REPLACE can't be used without BCM_L2_EGRESS_WITH_ID");
    }
    /** Verify the Header flags is set */
    header_flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_RECYCLE_INJECTED_2_PP_HEADER;
    SHR_NOF_SET_BITS_IN_RANGE_VERIFY(egr->flags, header_flags, 1, 1,
                                     _SHR_E_PARAM,
                                     "Either BCM_L2_EGRESS_RECYCLE_HEADER or BCM_L2_EGRESS_RECYCLE_INJECTED_2_PP_HEADER must be set");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief -
* Verify function for BCM-API: bcm_dnx_l2_egress_get()
*
* \param [in] unit - relevant unit.
* \param [in] encap_id - Encapsulation index
* \param [in] egr - Pointer to information about
*                   device-independent L2 egress structure.
*
* \return
*   shr_error_e
*
* \remark
*   * None
* \see
* bcm_dnx_l2_egress_get
*/
static shr_error_e
dnx_l2_egress_get_verify(
    int unit,
    bcm_if_t encap_id,
    bcm_l2_egress_t * egr)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(egr, _SHR_E_PARAM, "egr");

   /** check encap id is an interface of type lif */
    if (!BCM_L3_ITF_TYPE_IS_LIF(encap_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_l2_egress_get is supported only with encap_id of type lif\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief -
* Verify function for BCM-API: bcm_dnx_l2_egress_destroy()
*
* \param [in] unit - relevant unit.
* \param [in] encap_id - Encapsulation index to delete
*
* \return
*   shr_error_e
*
* \remark
*   * None
* \see
* bcm_dnx_l2_egress_destroy
*/
static shr_error_e
dnx_l2_egress_destroy_verify(
    int unit,
    bcm_if_t encap_id)
{
    SHR_FUNC_INIT_VARS(unit);

   /** check encap id is an interface of type lif */
    if (!BCM_L3_ITF_TYPE_IS_LIF(encap_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_l2_egress_destroy is supported only with encap_id of type lif\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write to L2 egress entry out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] egr - A pointer to the struct that holds
 *     information for the L2 egress encapsulation entry
 *   \param [in] table_result_type- relevant result type of the table
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 */
static shr_error_e
dnx_l2_egress_table_set(
    int unit,
    bcm_l2_egress_t * egr,
    int table_result_type,
    uint32 local_outlif)
{
    uint32 entry_handle_id;
    uint32 fwd_code;
    uint32 recycle_default_inlif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(egr, _SHR_E_PARAM, "egr");

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_RCH, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, table_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHERTYPE, INST_SINGLE, egr->ethertype);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_OUTER_VLAN, INST_SINGLE, egr->outer_tpid);
/**    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INNER_VLAN, INST_SINGLE, egr->inner_tpid); */
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE, egr->dest_mac);
    if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_RECYCLE_HEADER))
    {
        fwd_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_ENC;
    }
    /** BCM_L2_EGRESS_RECYCLE_INJECTED_2_PP_HEADER*/
    else
    {
        fwd_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_PTCH_ENC;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CODE, INST_SINGLE, fwd_code);

    /** set dummy lif */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_default_recyle_lif_get(unit, &recycle_default_inlif));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DUMMY_IN_LIF, INST_SINGLE, recycle_default_inlif);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Reset given entry from L2 egress out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be reset.
 *   \param [in] result_type - result type associated with local lif
 */
static shr_error_e
dnx_l2_egress_table_clear(
    int unit,
    uint32 local_outlif,
    uint32 result_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_RCH, &entry_handle_id));

    /** Setting key fields -- DBAL_RESULT_TYPE_EEDB_RCH_ETPS_RCH */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    /** Setting result type */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Read from L2 egress entry out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [out] egr - A pointer to the struct that holds
 *     information for the L2 egress encapsulation entry
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be returned.
 *   \param [in] result_type - result type associated with local lif
 */
static shr_error_e
dnx_l2_egress_table_get(
    int unit,
    bcm_l2_egress_t * egr,
    uint32 local_outlif,
    uint32 result_type)
{
    uint32 entry_handle_id;
    uint32 fwd_code;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(egr, _SHR_E_PARAM, "egr");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_RCH, &entry_handle_id));

    /** Setting key fields -- DBAL_RESULT_TYPE_EEDB_RCH_ETPS_RCH */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    /** Setting result type */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /**
     * Get egress encapsulation data from dbal table
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_ETHERTYPE, INST_SINGLE, &(egr->ethertype)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_TPID_OUTER_VLAN, INST_SINGLE, &(egr->outer_tpid)));
/**
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_TPID_INNER_VLAN, INST_SINGLE, &(egr->inner_tpid)));
*/
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_L2_MAC, INST_SINGLE, egr->dest_mac));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FWD_CODE, INST_SINGLE, &fwd_code));
    if (fwd_code == DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_ENC)
    {
        egr->flags |= BCM_L2_EGRESS_RECYCLE_HEADER;
    }
    else if (fwd_code == DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_PTCH_ENC)
    {
        egr->flags |= BCM_L2_EGRESS_RECYCLE_INJECTED_2_PP_HEADER;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected forward code %d.\n", fwd_code);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The API creates and configures recycle header.
 *
 * \param [in] unit - Relevant unit
 * \param [in] egr - A pointer to the estructure, holding information about the recycle header entry.
 *                 * egr -> dest_mac - next hop forwarding destination mac
 *                 * egr -> inner_tpid - inner tpid value
 *                 * egr -> outer_tpid - outer tpid value
 *                 * egr -> ethertype - ethertype, like in ethernet header, describe the protocol encapsulated by the RCH header.
 *                 * egr -> encap_id - l3_intf: recycle encapsulation ID
 *                 * egr -> flags - Supported flags are: BCM_L2_EGRESS_WITH_ID, BCM_L2_EGRESS_REPLACE, BCM_L2_EGRESS_RECYCLE_HEADER.
 *                                When creating recycle header with given encap_id, the BCM_L2_EGRESS_WITH_ID had to be set.
 *                                When replacing the information for certain enty both flags BCM_L2_EGRESS_WITH_ID, BCM_L2_EGRESS_REPLACE
 *                                need to be set and a valid encap_id to be given.
 *                                The flag BCM_L2_RECYCLE_HEADER must always be present, when recycle header is being created in  the EEDB
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_egress_create(
    int unit,
    bcm_l2_egress_t * egr)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    lif_mngr_local_outlif_info_t outlif_info;
    int global_lif_id = 0;
    int lif_alloc_flags = 0;
    int table_result_type = 0;
    int lif_get_flags = 0;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);
    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l2_egress_create_verify(unit, egr));
    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

    /** WITH_ID flag is used - get global out-lif ID */
    if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_WITH_ID))
    {
        /** Add alloc_with_id flag */
        global_lif_id = BCM_L3_ITF_VAL_GET(egr->encap_id);
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }
    table_result_type = DBAL_RESULT_TYPE_EEDB_RCH_ETPS_RCH;

    /** Allocate new egr out-LIF - REPLACE flag is not set */
    if (!_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_REPLACE))
    {
        /** fill the local outlif info. */
        sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
        outlif_info.dbal_table_id = DBAL_TABLE_EEDB_RCH;
        outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RCH;
        /** Allocate LIF - WITH_ID if the flag was set */
        SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &global_lif_id, NULL, &outlif_info));
        /** update returned Encap ID's */
        BCM_L3_ITF_SET(egr->encap_id, BCM_L3_ITF_TYPE_LIF, global_lif_id);
        /** Add entry to out-LIF table */
        SHR_IF_ERR_EXIT(dnx_l2_egress_table_set(unit, egr, table_result_type, outlif_info.local_outlif));
        /** Write global to local LIF mapping to GLEM. */
        
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, global_lif_id, outlif_info.local_outlif));
    }

    /** Replace existing out-LIF */
    else
    {
        /** get GPort HW resources (global and local tunnel outlif) */
        lif_get_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;

        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, egr->encap_id);

        /** we don't have gport here, so we're using gport tunnel */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_get_flags, &gport_hw_resources),
                                    _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        /** Check that eep is an index of a RCH OutLIF (in SW DB) */
        if (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_RCH)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal to replace non RCH OutLIF to RCH OutLIF");
        }
        /** Add entry to out-LIF table */
        SHR_IF_ERR_EXIT(dnx_l2_egress_table_set(unit, egr, table_result_type, gport_hw_resources.local_out_lif));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Get l2 egress object configuration.
 * \param [in] unit - relevant unit.
 * \param [in] encap_id - Encapsulation index
 * \param [out] egr - Pointer to information about
 *        device-independent L2 egress object.
 *        service:
 *          - [out] ethertype - Ethertype to replace with.
 *          - [out] outer_tpid - outer vlan tag TPID.
 *          - [out] inner_tpid - inner vlan tag TPID.
 *          - [out] dest_mac - Destination MAC address to replace with.
 *          - [out] encap_id - encap_id given.
 *          - [out] flags - with BCM_L2_EGRESS_RECYCLE_HEADER.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_egress_get(
    int unit,
    bcm_if_t encap_id,
    bcm_l2_egress_t * egr)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t gport;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l2_egress_get_verify(unit, encap_id, egr));

    /** Get local outlif from he resources */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, encap_id);

    flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, flags, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /** Set return values in egr */
    bcm_l2_egress_t_init(egr);
    egr->encap_id = encap_id;
    SHR_IF_ERR_EXIT(dnx_l2_egress_table_get
                    (unit, egr, gport_hw_resources.local_out_lif, gport_hw_resources.outlif_dbal_result_type));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Destroy l2 egress object configuration.
 * \param [in] unit - relevant unit.
 * \param [in] encap_id - Encapsulation index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_egress_destroy(
    int unit,
    bcm_if_t encap_id)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t gport;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_l2_egress_destroy_verify(unit, encap_id));

    /** Get local outlif from he resources */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, encap_id);

    flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, flags, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /** Clear --- table values */
    SHR_IF_ERR_EXIT(dnx_l2_egress_table_clear
                    (unit, gport_hw_resources.local_out_lif, gport_hw_resources.outlif_dbal_result_type));

    /** Remove global to local LIF mapping from GLEM. core-Id=0 until bcm_dnx_l2_egress_create is fixed */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, gport_hw_resources.global_out_lif));

    /** Free LIF */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, gport_hw_resources.global_out_lif, NULL, gport_hw_resources.local_out_lif));

exit:
    SHR_FUNC_EXIT;
}
