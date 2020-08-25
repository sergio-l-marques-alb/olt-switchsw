/** \file vswitch_port.c
 *
 *  Vswitch association with gport procedures for DNX.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/vswitch.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include <bcm/error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * Code will be added later
 */
#define WAITING_FOR_IMPLEMENTATION FALSE

/*
 * }
 */

/**
 * \brief - Verify function for bcm_dnx_vswitch_port_add
 */
static shr_error_e
dnx_vswitch_port_add_verify(
    int unit,
    bcm_vlan_t vsi,
    bcm_gport_t port)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the local In-LIF from the supplied gport
     * Get local In-LIF using DNX Algo Gport Managment:
     * This will also verify that the lif is legal and has an ingress side.
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    /*
     * verify gport is vlan/extender port
     */
    if (!BCM_GPORT_IS_VLAN_PORT(port) && !BCM_GPORT_IS_EXTENDER_PORT(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. gport = 0x%08X, PORT TYPE is not a VLAN port or Extender port LIF. \n", port);
    }

    /*
     * verify vsi is valid
     */
    if (!DNX_VSI_VALID(unit, vsi))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vsi = %d is not a valid vsi.\n", vsi);
    }

    /*
     * Verify that the VSI is allocated
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "vsi %d doesn't exist\n", vsi);
    }

    /*
     * Verify that the LIF type is the right one and has a VSI field:
     *   - AC symmetric VLAN-PORT
     *   - Native AC
     */
    if (BCM_GPORT_IS_VLAN_PORT(port))
    {
        if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
        {
            /*
             * AC LIF.
             * Check that it is a symmetric LIF:
             */
            if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(port) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY) ||
                (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(port) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting! port = 0x%08X is not a symmertic AC LIF!!\n", port);
            }

            /*
             * Check that it has a VSI field
             */
            if ((gport_hw_resources.inlif_dbal_result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_1_VLAN) ||
                (gport_hw_resources.inlif_dbal_result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE) ||
                (gport_hw_resources.inlif_dbal_result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting! port = 0x%08X is a symmertic AC LIF but it's type = %d is without VSI field!!\n",
                             port, gport_hw_resources.inlif_dbal_result_type);
            }
        }
        else if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION)
        {
            /*
             * Ingress Native AC.
             * Check that it has a VSI field
             */
            if (gport_hw_resources.inlif_dbal_result_type ==
                DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_VLAN_EDIT_ONLY)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting! port = 0x%08X is Ingress Native AC LIF but it's type = %d is without VSI field!!\n",
                             port, gport_hw_resources.inlif_dbal_result_type);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vswitch_port_get
 */
static shr_error_e
dnx_vswitch_port_get_verify(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t * vsi)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Checking that the vsi pointer is not NULL:
     */
    SHR_NULL_CHECK(vsi, _SHR_E_PARAM, "vsi");

    /*
     * Retrieve the local In-LIF from the supplied gport
     * Get local In-LIF using DNX Algo Gport Managment:
     * This will also verify that the lif is legal and has an ingress side.
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
    /*
     * verify gport is vlan port
     */
    if (!BCM_GPORT_IS_VLAN_PORT(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. gport = 0x%08X, PORT TYPE is not a VLAN port LIF. \n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vswitch_port_delete
 */
static shr_error_e
dnx_vswitch_port_delete_verify(
    int unit,
    bcm_vlan_t vsi,
    bcm_gport_t port)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the local In-LIF from the supplied gport
     * Get local In-LIF using DNX Algo Gport Managment:
     * This will also verify that the lif is legal and has an ingress side.
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /*
     * verify gport is vlan port
     */
    if (!BCM_GPORT_IS_VLAN_PORT(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. port = 0x%08X, PORT TYPE is not a VLAN port LIF. \n", port);
    }

    /*
     * verify vsi is valid
     */
    if (!DNX_VSI_VALID(unit, vsi))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vsi = %d is not a valid vsi.\n", vsi);
    }

    /*
     * Verify that the VSI is allocated
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "vsi %d doesn't exist\n", vsi);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vswitch_port_delete_all
 */
static shr_error_e
dnx_vswitch_port_delete_all_verify(
    int unit,
    bcm_vlan_t vsi)
{
#if (WAITING_FOR_IMPLEMENTATION==TRUE)
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify vsi is valid
     */
    if (!DNX_VSI_VALID(unit, vsi))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vsi = %d is not a valid vsi.\n", vsi);
    }

    /*
     * Verify that the VSI is allocated
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "vsi %d doesn't exist\n", vsi);
    }

exit:
    SHR_FUNC_EXIT;
#endif /* WAITING_FOR_IMPLEMENTATION */

    return _SHR_E_INTERNAL;
}


static shr_error_e
dnx_vswitch_port_in_lif_table_update_set(
    int unit,
    bcm_vlan_t vsi,
    int learn_enable,
    bcm_gport_t port)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get local In-LIF using DNX Algo Gport Managment:
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /*
     * Update the In-LIF information with the supplied VSI value
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, vsi);

    /*
     * Update the learn enable for applicable result types
     */
    if (((gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
         && (BCM_DNX_IS_RESULT_TYPE_AC_CROSS_CONNECT(gport_hw_resources.inlif_dbal_result_type) == FALSE))
        || ((gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION)
            && (BCM_DNX_IS_RESULT_TYPE_NATIVE_AC_CROSS_CONNECT(gport_hw_resources.inlif_dbal_result_type) == FALSE)))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, learn_enable);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_vswitch_port_in_lif_table_update_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t * vsi)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 tmp32bitVal;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get local In-LIF using DNX Algo Gport Managment:
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /*
     * Get the VSI value from the In-LIF table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, &tmp32bitVal);
    *vsi = (bcm_vlan_t) tmp32bitVal;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API associated a gport with a VSI by updating the vsi
 * attribute of an In-LIF.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vsi -
 *     The VSI that the In-LIF is associated with
 *   \param [in] port -
 *     A gport that represent an In-LIF that as is assciated
 *     with a VSI
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for
 *           example: invalid vsi.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 * HW table DBAL_TABLE_IN_AC_INFO_DB is modified with the following fields:
 *  - DBAL_FIELD_RESULT_TYPE - sets to DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_IN_LIF_FORMAT_AC_MP
 *  - DBAL_FIELD_VSI - sets to VSI value
 * \remark
 * This function also enable learning for MP LIFs. 
 */
shr_error_e
bcm_dnx_vswitch_port_add(
    int unit,
    bcm_vlan_t vsi,
    bcm_gport_t port)
{
    int learn_enable = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_port_add_verify(unit, vsi, port));

    SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_set(unit, vsi, learn_enable, port));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API gets the VSI which is associated to gport (the vsi attribute of an In-LIF).
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] port -
 *     A gport that represent an In-LIF that as is assciated with a VSI
 *   \param [in] vsi -
 *   Pointer to the VSI (that the In-LIF is associated with)
 * \par INDIRECT INPUT:
 * DBAL_TABLE_IN_AC_INFO_DB
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for
 *           example: invalid port.
 * \par INDIRECT OUTPUT
  *   the VSI value (that the In-LIF is associated with, at table DBAL_TABLE_IN_AC_INFO_DB, see vsi above)
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_vswitch_port_add
 */
shr_error_e
bcm_dnx_vswitch_port_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t * vsi)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_port_get_verify(unit, port, vsi));

    SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_get(unit, port, vsi));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API disassociates a gport from a VSI
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vsi -
 *     The VSI that the In-LIF is associated with
 *   \param [in] port -
 *     A gport that represent an In-LIF that as is assciated
 *     with a VSI - as should be removed
 * \par INDIRECT INPUT:
 *   * DBAL_TABLE_IN_AC_INFO_DB - see INDIRECT OUTPUT
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for
 *           example: invalid vsi.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 * HW table DBAL_TABLE_IN_AC_INFO_DB is modified with the following fields:
 * DBAL_FIELD_VSI - sets to DNX_VSI_DEFAULT value
 * \see
 *   * bcm_dnx_vswitch_port_add, bcm_dnx_vswitch_port_get
 */

shr_error_e
bcm_dnx_vswitch_port_delete(
    int unit,
    bcm_vlan_t vsi,
    bcm_gport_t port)
{
    bcm_vlan_t vsi_dbal;
    int learn_enable = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_port_delete_verify(unit, vsi, port));

    /*
     * check that the port is indeed associated with the given vsi
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_get(unit, port, &vsi_dbal));

    if (vsi != vsi_dbal)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. gport = 0x%08X is associated with vsi = %d.  Input vsi = %d.\n",
                     port, (bcm_vlan_t) vsi_dbal, vsi);
    }

    /*
     * set back the vsi to default (see DNX_VSI_DEFAULT definition)
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_set(unit, DNX_VSI_DEFAULT, learn_enable, port));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API disassociates all gports that are associated with the VSI.
 * It is done by traversing all In-LIF range and setting the vsi
 * attribute of an associated In-LIF to DNX_VSI_DEFAULT value.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vsi -
 *     The VSI that the In-LIFs are associated with
 * \par INDIRECT INPUT:
 *    DBAL_TABLE_IN_AC_INFO_DB
 *    dnx_data_lif.in_lif.nof_sbc_local_in_lifs_get - number of local In-LIF
 *
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for
 *           example: invalid vsi.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 * HW table DBAL_TABLE_IN_AC_INFO_DB is modified with the following fields:
 * DBAL_FIELD_VSI - sets to DNX_VSI_DEFAULT value
 * \see
 * bcm_dnx_vswitch_port_add, bcm_dnx_vswitch_port_get
 */

shr_error_e
bcm_dnx_vswitch_port_delete_all(
    int unit,
    bcm_vlan_t vsi)
{
    bcm_gport_t local_in_lif;
    int local_in_lif_nof;
    bcm_vlan_t vsi_dbal;
    int learn_enable = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_port_delete_all_verify(unit, vsi));

    local_in_lif = 0;
    local_in_lif_nof = dnx_data_lif.in_lif.nof_sbc_local_in_lifs_get(unit);

    

    local_in_lif_nof = local_in_lif_nof >> 1;
    do
    {

        /*
         * check that the port is associated with the vsi
         */
        SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_get(unit, local_in_lif, &vsi_dbal));

        if (vsi == vsi_dbal)
        {
            /*
             * set back the vsi to default (see DNX_VSI_DEFAULT definition)
             */
            SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_set
                            (unit, DNX_VSI_DEFAULT, learn_enable, local_in_lif));
        }

        local_in_lif++;
    }
    while (local_in_lif < local_in_lif_nof);

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 * This API traverse existing ports on vswitch.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *      Relevant unit.
 *   \param [in] vsi -
 *      The VSI that the In-LIF is associated with
 *   \param [in] cb -
 *      Vswitch callback function
 *   \param [in] user_data -
 *      Pointer to user_data (input to the call back function)
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * Not implemented yet.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vswitch_port_traverse(
    int unit,
    bcm_vlan_t vsi,
    bcm_vswitch_port_traverse_cb cb,
    void *user_data)
{
    return _SHR_E_INTERNAL;
}
