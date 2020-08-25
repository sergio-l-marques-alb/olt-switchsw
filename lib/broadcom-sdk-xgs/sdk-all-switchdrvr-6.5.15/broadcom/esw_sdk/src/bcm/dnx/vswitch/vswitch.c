/** \file vswitch.c
 *
 *  Vswitch procedures for DNX.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/vsi/vswitch_vpn.h>
#include <bcm_int/dnx/l3/l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/swstate/auto_generated/access/vlan_access.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <bcm_int/common/multicast.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
 * \brief -
 * Verify function for BCM-API: bcm_dnx_vswitch_create().
 *
 * \param [in] unit - relevant unit.
 * \param [in] vsi - pointer to vsi
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_create
 */
static shr_error_e
dnx_vswitch_create_verify(
    int unit,
    bcm_vlan_t * vsi)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vsi, _SHR_E_PARAM, "vsi");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for BCM-API: bcm_dnx_vswitch_create_with_id()
 *
 * \param [in] unit - relevant unit.
 * \param [in] vsi - vsi ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_create_with_id
 */
static shr_error_e
dnx_vswitch_create_with_id_verify(
    int unit,
    bcm_vlan_t vsi)
{
    int nof_vsis;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify vsi is in range:
     */
    nof_vsis = dnx_data_l2.vsi.nof_vsis_get(unit);

    if (vsi >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vsi = %d is not valid!!! (MAX value for vsi is %d)\n", vsi, (nof_vsis - 1));
    }

    /*
     * Check that the vsi was not allocated. 
     * Note: in case the vsi is already allocated, sw_state_algo_res_is_allocated returns _SHR_E_EXISTS
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));

    if (is_allocated == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "vsi %d already exist!!!\n", vsi);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for BCM-API: bcm_dnx_vswitch_destroy()
 *
 * \param [in] unit - relevant unit.
 * \param [in] vsi - vsi ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_destroy
 */
static shr_error_e
dnx_vswitch_destroy_verify(
    int unit,
    bcm_vlan_t vsi)
{
    uint8 is_vswitch = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify vsi type is VSWITCH VSI
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, (int)vsi, _bcmDnxVsiTypeVswitch, &is_vswitch));
    if (!is_vswitch)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "VSI is not allocated for this usage!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function to get destination and outlif
 * from a DBAL entry.
 * \param [in] unit - relevant unit
 * \param [in] gport - key to DBAL table
 * \param [out] destination - pointer to the return destination value
 * \param [out] outlif - pointer to the return outlif value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vswitch_cross_connect_get
 *  bcm_dnx_vswitch_cross_connect_delete
 */
shr_error_e
dnx_vswitch_cross_connect_get_apply(
    int unit,
    bcm_gport_t gport,
    uint32 * destination,
    uint32 * outlif)
{
    uint32 temp_destination;
    uint32 temp_outlif;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * GPORT: Get local In-LIF using DNX Algo GPORT Managment:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    /** Taking a new handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Get fwd_info from DBAL table
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &temp_destination));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &temp_outlif));

    /*
     * return _SHR_E_NOT_FOUND if destination and outLif were not set or deleted (=0).
     */
    SHR_VAL_VERIFY( !(temp_destination | temp_outlif), 0, _SHR_E_NOT_FOUND, "Port is not cross-connected\n");

    /*
     * Assign destination and OutLif to return values.
     */
    *destination = temp_destination;
    *outlif = temp_outlif;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for BCM-API:
 * bcm_dnx_vswitch_cross_connect_add
 * \param [in] unit - relevant unit
 * \param [in] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the two ports to attach to P2P
 *        service.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vswitch_cross_connect_add
 */
shr_error_e
dnx_vswitch_cross_connect_add_verify(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources1;
    uint8 is_physical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gports, _SHR_E_PARAM, "gports");

    /*
     * Checks that the port1 type is either VLAN-PORT / MPLS-PORT
     */
    if (!(_SHR_GPORT_IS_VLAN_PORT(gports->port1)) && !(_SHR_GPORT_IS_MPLS_PORT(gports->port1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. gport1 = 0x%08X is not a VLAN-PORT or MPLS-PORT.\n", gports->port1);
    }

    /*
     * Checks that the port2 type is either Physical port or FEC or VLAN-PORT / MPLS-PORT
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gports->port2, &is_physical_port));

    if (!(_SHR_GPORT_IS_VLAN_PORT(gports->port2)) && !(_SHR_GPORT_IS_MPLS_PORT(gports->port2))
        && !_SHR_GPORT_IS_FORWARD_PORT(gports->port2) && !is_physical_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. gport2 = 0x%08X is not a physical port or VLAN-PORT or MPLS-PORT.\n",
                     gports->port2);
    }

    /*
     * Checks that the In-LIF of gport1 is P2P
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gports->port1, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                 &gport_hw_resources1), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    if (_SHR_GPORT_IS_VLAN_PORT(gports->port1))
    {

        /*
         * VLAN PORT
         * Expecting result type AC_P2P
         */
        if (BCM_DNX_IS_RESULT_TYPE_AC_CROSS_CONNECT(gport_hw_resources1.inlif_dbal_result_type) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting. IN-LIF of gport1 = 0x%08X is not a P2P IN-LIF (result type should be %d (or %d) but it is %d)\n",
                         gports->port1, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_1_VLAN,
                         DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE,
                         gport_hw_resources1.inlif_dbal_result_type);
        }
    }
    else
    {
        /*
         * MPLS PORT
         * Expecting result type PWE_P2P_NO_LEARN
         */
        if (gport_hw_resources1.inlif_dbal_result_type != DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting. IN-LIF of gport1 = 0x%08X is not a P2P IN-LIF (result type should be %d but it is %d)\n",
                         gports->port1, DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING,
                         gport_hw_resources1.inlif_dbal_result_type);
        }

    }

    /*
     * Symmetrical cross-connect?
     */
    if ((_SHR_IS_FLAG_SET(gports->flags, BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL) == FALSE))
    {
        /*
         * Checks that the In-LIF of gport2 is P2P
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources2;
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, gports->port2, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources2), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        if (_SHR_GPORT_IS_VLAN_PORT(gports->port2))
        {
            /*
             * VLAN PORT
             * Expecting result type AC_P2P
             */
            if (BCM_DNX_IS_RESULT_TYPE_AC_CROSS_CONNECT(gport_hw_resources2.inlif_dbal_result_type) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting. IN-LIF of gport2 = 0x%08X is not a P2P IN-LIF (result type should be %d (or %d) but it is %d)\n",
                             gports->port2, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_1_VLAN,
                             DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE,
                             gport_hw_resources1.inlif_dbal_result_type);
            }
        }
        else
        {
            /*
             * MPLS PORT
             * Expecting result type PWE_P2P_NO_LEARN
             */
            if (gport_hw_resources2.inlif_dbal_result_type !=
                DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting. IN-LIF of gport2 = 0x%08X is not a P2P IN-LIF (result type should be %d but it is %d)\n",
                             gports->port1, DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING,
                             gport_hw_resources1.inlif_dbal_result_type);
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * 1. Checks that the gport types are VLAN-PORT / MPLS-PORT
 * 2. Checks that the In-LIF of gport exists
 * 3. Verify result type:
 *     VLAN PORT ==> AC_P2P
 *     MPLS PORT ==> PWE_P2P_NO_LEARN
 *
 * \param [in] unit - relevant unit.
 * \param [in] is_destination - boolean, should the port be verified as P2P source port or P2Pdestination port.
 * \param [in] gport - gport to verify.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vswitch_cross_connect_get
 *  bcm_dnx_vswitch_cross_connect_delete
 */
shr_error_e
dnx_vswitch_cross_connect_port_verify(
    int unit,
    uint8 is_destination,
    bcm_gport_t gport)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources1;
    uint8 is_physical_port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set the relevent port to verify
     */
    if (!is_destination)
    {
        /*
         * Checks that the port type is either VLAN-PORT / MPLS-PORT
         */
        if (!(_SHR_GPORT_IS_VLAN_PORT(gport)) && !(_SHR_GPORT_IS_MPLS_PORT(gport)) && (!(_SHR_GPORT_IS_TUNNEL(gport))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. gport = 0x%08X is not a VLAN-PORT, MPLS-PORT or TUNNEL.\n",
                         gport);
        }
    }

    else
    {
        /*
         * Checks that the port type is either Physical port or FEC or VLAN-PORT / MPLS-PORT
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));

        if (!(_SHR_GPORT_IS_VLAN_PORT(gport)) && !(_SHR_GPORT_IS_MPLS_PORT(gport))
            && !_SHR_GPORT_IS_FORWARD_PORT(gport) && !is_physical_port)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting. gport = 0x%08X is not a VLAN-PORT nor MPLS-PORT nor FEC nor physical port.\n",
                         gport);
        }
    }

    /*
     * Checks that the In-LIF of gport exists
     */
    if (_SHR_GPORT_IS_VLAN_PORT(gport) || (_SHR_GPORT_IS_MPLS_PORT(gport)) || (_SHR_GPORT_IS_TUNNEL(gport)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources1));

        if (_SHR_GPORT_IS_VLAN_PORT(gport))
        {
            /*
             * VLAN PORT
             * Expecting result type AC_P2P
             */
            if (gport_hw_resources1.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
            {
                if (BCM_DNX_IS_RESULT_TYPE_AC_CROSS_CONNECT(gport_hw_resources1.inlif_dbal_result_type) == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Wrong setting. IN-LIF of gport%d = 0x%08X is not a P2P IN-LIF (dbal table is %s, result type should be %d (or %d) but it is %d)\n",
                                 gport, gport, dbal_logical_table_to_string(unit,
                                                                            gport_hw_resources1.inlif_dbal_table_id),
                                 DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_1_VLAN,
                                 DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE,
                                 gport_hw_resources1.inlif_dbal_result_type);
                }
            }
            if (gport_hw_resources1.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION)
            {
                if (BCM_DNX_IS_RESULT_TYPE_NATIVE_AC_CROSS_CONNECT(gport_hw_resources1.inlif_dbal_result_type) == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Wrong setting. IN-LIF of gport%d = 0x%08X is not a P2P IN-LIF (dbal table is %s, result type should be %d (or %d) but it is %d)\n",
                                 gport, gport, dbal_logical_table_to_string(unit,
                                                                            gport_hw_resources1.inlif_dbal_table_id),
                                 DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_VLAN_EDIT_VSI_P2P,
                                 DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_AC_VSI_P2P,
                                 gport_hw_resources1.inlif_dbal_result_type);
                }
            }
        }
        else if (_SHR_GPORT_IS_MPLS_PORT(gport))
        {
            /*
             * MPLS PORT
             * Expecting result type PWE_P2P_NO_LEARN
             */
            if (gport_hw_resources1.inlif_dbal_result_type !=
                DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting. IN-LIF of gport%d = 0x%08X is not a P2P IN-LIF (result type should be %d but it is %d)\n",
                             gport, gport, DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING,
                             gport_hw_resources1.inlif_dbal_result_type);
            }
        }
        else
        {
            /*
             * TUNNEL - used for EVPN
             * Expecting result type EVPN_EVI_P2P_NO_LEARNING
             */
            if (gport_hw_resources1.inlif_dbal_result_type !=
                DBAL_RESULT_TYPE_IN_LIF_FORMAT_EVPN_IN_LIF_EVPN_EVI_P2P_NO_LEARNING)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting. IN-LIF of gport%d = 0x%08X is not a P2P IN-LIF (result type should be %d but it is %d)\n",
                             gport, gport, DBAL_RESULT_TYPE_IN_LIF_FORMAT_EVPN_IN_LIF_EVPN_EVI_P2P_NO_LEARNING,
                             gport_hw_resources1.inlif_dbal_result_type);
            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for attaching one direction of P2P service.
 * It updates the given IN-LIF entry in the LIF table with the the given
 * forwarding information (out-lif, destination).
 *
 * \param [in] unit - relevant unit.
 * \param [in] gport_hw_resources - pointer to
 *        dnx_algo_gpm_gport_hw_resources_t structure holding
 *        the GPORT hardware resources information.
 * \param [in] forward_info - pointer to structure type
 *        dnx_algo_gpm_forward_info_t holding the pre-fec
 *        forward information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_cross_connect_add
 */
static shr_error_e
dnx_vswitch_cross_connect_add_direction_apply(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &entry_handle_id));
    /*
     * Set key:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources->local_in_lif);
    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources->inlif_dbal_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P);

    if ((forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF) ||
        (forward_info->fwd_info_result_type == BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, forward_info->outlif);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, forward_info->destination);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for deleting one port from relevant
 * table, port is taken from gport object and chosen by
 * value og port_num parameter.
 *
 * \param [in] unit - relevant unit
 * \param [in] gport - gport to delete it's P2P reference to peer.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vswitch_cross_connect_delete
 */
static shr_error_e
dnx_vswitch_cross_connect_delete_apply(
    int unit,
    bcm_gport_t gport)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * gport: Get local In-LIF using DNX Algo Gport Managment:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    /** Taking a new handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);

    /** Reseting added values in entry */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for traversing P2P sevices
 * \param [in] unit - relevant unit.
 * \param [in] table_id - table to traverse, can be:
 *                             DBAL_TABLE_IN_AC_INFO_DB
 *                             DBAL_TABLE_IN_LIF_FORMAT_PWE
 *                             DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION
 * \param [in] cb - Callback function to perform
 * \param [in] user_data - pointer to additional user data.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vswitch_cross_connect_traverse
 */
static shr_error_e
dnx_vswitch_cross_connect_traverse_apply(
    int unit,
    dbal_tables_e table_id,
    bcm_vswitch_cross_connect_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 ignore_lif = 0;
    int is_end;
    uint32 local_in_lif;
    uint32 hw_resource;
    uint32 result_type;
    bcm_vswitch_cross_connect_t gports;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Look for In-LIF data in relevant hw_resource
     */
    hw_resource = (table_id == DBAL_TABLE_IN_AC_INFO_DB) ?
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS : DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS;

    /*
     * Taking a handle
     */
    DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id);
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Add rule - ignore In-LIF 0
     */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &ignore_lif, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        /*
         * Initialize gport
         */
        sal_memset(&gports, 0x0, sizeof(bcm_vswitch_cross_connect_t));

        /*
         * Get entry result type -- table value
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

        /*
         * Verify lif is P2P
         */
        if ((table_id == DBAL_TABLE_IN_AC_INFO_DB && BCM_DNX_IS_RESULT_TYPE_AC_CROSS_CONNECT(result_type)) ||
            (table_id == DBAL_TABLE_IN_LIF_FORMAT_PWE
             && result_type == DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING)
            || (table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION
                && BCM_DNX_IS_RESULT_TYPE_NATIVE_AC_CROSS_CONNECT(result_type)))
        {
            /*
             * Get local In-LIF -- table key
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF, &local_in_lif));

            /*
             * Get gport from In-LIF
             */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                            (unit, hw_resource, DBAL_CORE_ALL, local_in_lif, &(gports.port1)));

            /*
             * Get connected gport
             */
            SHR_IF_ERR_REPLACE_AND_EXIT(bcm_dnx_vswitch_cross_connect_get(unit, &gports),
                                         _SHR_E_NOT_FOUND, _SHR_E_NONE);

            /*
             * Make sure Out-LIF is connected
             */
            if (gports.port2 != 0)
            {
                /*
                 * Run callback function
                 */
                SHR_IF_ERR_EXIT(cb(unit, &gports, user_data));
            }
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Create a Virtual Switching Instance.
 *
 * \param [in] unit - Unit id
 * \param [out] vsi - pointer to Virtual Switching Instance
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
bcm_dnx_vswitch_create(
    int unit,
    bcm_vlan_t * vsi)
{
    int allocated_vsi;
    uint32 flags;
    uint8 replaced = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_create_verify(unit, vsi));

    /*
     * DNX SW Algorithm, allocate VSI.
     */
    flags = 0;
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_alloc(unit, flags, _bcmDnxVsiTypeVswitch, &allocated_vsi, &replaced));

    /*
     * Write to HW VSI table
     */
    SHR_IF_ERR_EXIT(dnx_vsi_table_default_set(unit, allocated_vsi));

    *vsi = allocated_vsi;
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Create a Virtual Switching Instance.
 * The ID of the VSI is an input parameter: vsi.
 *
 * \param [in] unit - relevant unit.
 * \param [in] vsi - the vsi ID.
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
bcm_dnx_vswitch_create_with_id(
    int unit,
    bcm_vlan_t vsi)
{
    int allocated_vsi;
    uint8 replaced = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_create_with_id_verify(unit, vsi));
    allocated_vsi = vsi;
    /*
     * DNX SW Algorithm, allocate VSI.
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_alloc
                    (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, _bcmDnxVsiTypeVswitch, &allocated_vsi, &replaced));

    if (!replaced)
    {
        /*
         * Write to HW VSI table (only if required)
         */
        SHR_IF_ERR_EXIT(dnx_vsi_table_default_set(unit, allocated_vsi));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: attachs given 2 ports in P2P service
 * \param [in] unit - relevant unit.
 * \param [in] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the two ports to attached to P2P
 *        service:
 *          - port1 - First gport in cross connect.
 *          - port2 - Second gport in cross connect.
 *          - encap1 - First gport encap id.
 *          - encap2 - Second gport encap id.
 *          - flags - BCM_VSWITCH_CROSS_CONNECT_XXX flags.
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
bcm_dnx_vswitch_cross_connect_add(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources1;
    dnx_algo_gpm_forward_info_t forward_info2;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_port_verify(unit, FALSE, gports->port1));

    /*
     * Symmetrical cross-connect?
     */
    if ((_SHR_IS_FLAG_SET(gports->flags, BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL) == FALSE))
    {
        SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_port_verify(unit, TRUE, gports->port2));
    }

    /*
     * 1. Configure gport1 ==> gport2:
     * Update In-LIF (P2P) of gport1 with FORWARD INFO of gport2
     */

    /*
     * gport1: Get local In-LIF using DNX Algo Gport Managment:
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gports->port1, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                 &gport_hw_resources1), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    /*
     * gport2: Resolve forward information:
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, gports->port2, gports->encap2, &forward_info2));
    /*
     * Update HW: direction gport1 ==> gport2
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_add_direction_apply(unit, &gport_hw_resources1, &forward_info2));

    /*
     * Symmetrical cross-connect?
     */
    if ((_SHR_IS_FLAG_SET(gports->flags, BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL) == FALSE))
    {
        /*
         * 2. Configure gport2 ==> gport1:
         * Update In-LIF (P2P) of gport2 with FORWARD INFO of gport1
         *
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources2;
        dnx_algo_gpm_forward_info_t forward_info1;

        /*
         * gport2: Get local In-LIF using DNX Algo Gport Managment:
         */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, gports->port2, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources2), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        /*
         * gport1: Resolve forward information:
         */
        SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                        (unit, gports->port1, gports->encap1, &forward_info1));
        /*
         * Update HW: direction gport2 ==> gport1
         */
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_add_direction_apply(unit, &gport_hw_resources2, &forward_info1));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: For a given P2P gport, returns its P2P peer.
 * \param [in] unit - relevant unit.
 * \param [in, out] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the port given and will contain information
 *        on peer port on return.
 *        service:
 *          - [in] port1 - given port.
 *          - [out] port2 - peer of port1, retun value.
 *          - [in] encap1 - First gport encap id.
 *          - [out] encap2 - Second gport encap id.
 *          - [in] flags - BCM_VSWITCH_CROSS_CONNECT_XXX flags.
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
bcm_dnx_vswitch_cross_connect_get(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify port1 in gports struct is valid.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_port_verify(unit, FALSE, gports->port1));

    /*
     * gport1: Get destination and outlif from DBAL table with gports->port1 as key
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_get_apply
                    (unit, gports->port1, &(forward_info.outlif), &(forward_info.destination)));

    /*
     * Forwarding is done according to destination and Outlif
     */
    forward_info.fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;

    /*
     * gport2: Resolve forward information:
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_from_forward_information
                    (unit, &(gports->port2), (uint32*) &(gports->encap2), &forward_info, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: For a given P2P gport, delete it's cross connect entry.
 * \param [in] unit - relevant unit.
 * \param [in] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the port given.
 *        service:
 *          - [in] port1 - given port.
 *          - [in] port2 - if flags=0, 2nd given port to delete.
 *          - [in] flags - BCM_VSWITCH_CROSS_CONNECT_XXX flags.
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
bcm_dnx_vswitch_cross_connect_delete(
    int unit,
    bcm_vswitch_cross_connect_t * gports)
{
    uint32 destination;
    uint32 outlif;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Symmetrical cross-connect?
     */
    if ((_SHR_IS_FLAG_SET(gports->flags, BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL) == FALSE))
    {
        /*
         * Verifying gport is valid
         */
        SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_port_verify(unit, TRUE, gports->port2));

        /*
         * Verifying gport exists as P2P
         */
        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_get_apply
                        (unit, gports->port2, &destination, &outlif));

        SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_delete_apply(unit, gports->port2));
    }

    /*
     * Verifying gport is valid
     */
    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_cross_connect_port_verify(unit, FALSE, gports->port1));

    /*
     * Verifying gport exists as P2P
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_get_apply
                    (unit, gports->port1, &destination, &outlif));

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_delete_apply(unit, gports->port1));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Traverse on all existing P2P services and preform given Callback function.
 * \param [in] unit - relevant unit.
 * \param [in] cb - Callback function to perform
 * \param [in] user_data - pointer to additional user data.
 *
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
bcm_dnx_vswitch_cross_connect_traverse(
    int unit,
    bcm_vswitch_cross_connect_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "callback");

    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_apply(unit, DBAL_TABLE_IN_AC_INFO_DB, cb, user_data));
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_apply(unit, DBAL_TABLE_IN_LIF_FORMAT_PWE, cb, user_data));
    SHR_IF_ERR_EXIT(dnx_vswitch_cross_connect_traverse_apply
                    (unit, DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION, cb, user_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * internal cb function for BCM-API:
 * bcm_dnx_vswitch_cross_connect_delete_all
 * \param [in] unit - relevant unit
 * \param [in] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the P2P entry to delete
 * \param [in] user_data - unused pointer added becuase
 *           of function typedef
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vswitch_cross_connect_delete_all
 *  bcm_dnx_vswitch_cross_connect_delete
 */
static shr_error_e
dnx_vswitch_cross_connect_delete_all_cb(
    int unit,
    bcm_vswitch_cross_connect_t * gports,
    void* user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set Cross Connect one direction flag.
     */
    gports->flags |= BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

    SHR_IF_ERR_REPLACE_AND_EXIT(bcm_dnx_vswitch_cross_connect_delete
            (unit, gports), _SHR_E_NOT_FOUND, _SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Delete all P2P LIFs.
 * \param [in] unit - relevant unit.
 *
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
bcm_dnx_vswitch_cross_connect_delete_all(
    int unit)
{
    uint32 additional_data = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_vswitch_cross_connect_traverse
            (unit, dnx_vswitch_cross_connect_delete_all_cb, &additional_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API: Destroy a Virtual Switching Instance.
 * \param [in] unit - relevant unit.
 * \param [in] vsi - vsi to destroy.
 *
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
bcm_dnx_vswitch_destroy(
    int unit,
    bcm_vlan_t vsi)
{
    uint8 used = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_destroy_verify(unit, vsi));

    /*
     * DNX SW Algorithm, free VSI.
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_dealloc(unit, _bcmDnxVsiTypeVswitch, (int) vsi));

    /*
     *  check if this VSI is also used by other type of services
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_check_except_type(unit, vsi, _bcmDnxVsiTypeVswitch, &used));
    if (!used)
    {
        /*
         * Clear HW VSI table entry
         */
        SHR_IF_ERR_EXIT(dnx_vsi_table_clear_set(unit, vsi));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}
