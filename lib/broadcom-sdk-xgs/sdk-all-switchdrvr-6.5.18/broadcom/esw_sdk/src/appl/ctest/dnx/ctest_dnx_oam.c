/** \file diag_dnx_oam_tests.c
 * 
 * Tests for OAM
 * 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/oam.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>

/* Function for period update of LMEP */
/**
 * \brief - This function updates the ccm_period,
 *        It is used to check the functinality of
 *        bcm_oam_endpoint_traverse API.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - Structure that was used for
 *        modification.
 * \param [in] user_data - new value
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
test_oam_endpoint_period_update_cb(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, endpoint_info->id, endpoint_info),
                        "Test failed.  Could not get local acc endpoint\n");

    endpoint_info->ccm_period = *((int *) user_data);
    endpoint_info->flags |= BCM_OAM_ENDPOINT_REPLACE;
    endpoint_info->flags |= BCM_OAM_ENDPOINT_WITH_ID;

    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info),
                        "Test failed.  Could not create local acc endpoint\n");

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit, "Update period of Endpoint [%d] to %d\n"), endpoint_info->id,
                 endpoint_info->ccm_period));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function compares two information structure, 
 *        presumably one that contains data to create a or
 *        modify a OAM remote endpoint, and the other that was read
 *        after the creation/modification occurs, and verifies
 *        that the relevant values are identical.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - Structure that was used for 
 *        creationg or modification
 * \param [in] endpoint_info2 - Structure to which the data was 
 *        later read.
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
diag_dnx_oam_rmep_test_compare_oam_info_structs(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    bcm_oam_endpoint_info_t * endpoint_info2)
{
    SHR_FUNC_INIT_VARS(unit);

    if (endpoint_info2->id != endpoint_info->id)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Endpoint ID read doesn't match endpoint ID written to endpoint.  "
                     "Written: 0x%08X. Read: 0x%08X.\n", endpoint_info->id, endpoint_info2->id);
    }
    if (endpoint_info2->flags != endpoint_info->flags)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Flags read don't match flags written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                     endpoint_info->flags, endpoint_info2->flags);
    }

    if (endpoint_info2->local_id != endpoint_info->local_id)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed. local_id  read don't match local_id written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                     endpoint_info->local_id, endpoint_info2->local_id);
    }

    if ((endpoint_info2->ccm_period != endpoint_info->ccm_period)
        && ((endpoint_info2->ccm_period + 1) != endpoint_info->ccm_period))
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed. ccm_period  read don't match ccm_period written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                     endpoint_info->ccm_period, endpoint_info2->ccm_period);
    }

    if (endpoint_info2->sampling_ratio != endpoint_info->sampling_ratio)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed. sampling_ratio  read don't match sampling_ratio written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                     endpoint_info->sampling_ratio, endpoint_info2->sampling_ratio);
    }

    if (endpoint_info2->loc_clear_threshold != endpoint_info->loc_clear_threshold)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed. loc_clear_threshold  read don't match loc_clear_threshold written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                     endpoint_info->loc_clear_threshold, endpoint_info2->loc_clear_threshold);
    }

    if (endpoint_info2->name != endpoint_info->name)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed. name  read don't match name written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                     endpoint_info->name, endpoint_info2->name);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function tests the OAM RMEP feature in a very basic 
 *        way: a number of RMEP endpoints are created and
 *        verified, modified and verified again, and finally
 *        deleted.  It is part of the ctest feature.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line 
 *        arguments were parsed (partially)
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
diag_dnx_oam_rmep_semantic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 num_eps, ep_index, half_num_eps;
    int rv;
    bcm_oam_endpoint_t *endpoint_id = NULL;
    bcm_oam_endpoint_info_t *endpoint_info = NULL, *endpoint_info1 = NULL, *endpoint_info2 = NULL;
    bcm_oam_endpoint_info_t *endpoint_info_non_acc = NULL;
    bcm_oam_group_info_t group_info;
    bcm_vlan_port_t vlan_port;
    bcm_oam_profile_t ingress_acc_profile, ingress_lif_profile, egress_lif_profile;
    uint8 group_name[] = { 0x1, 0x3, 0x2, 0xd, 0xe };
    bcm_oam_group_t group_id = -1;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("Number", num_eps);

    LOG_CLI((BSL_META("Test with %d endpoints\n"), num_eps));

    /** Set values for tunnel-type loop   */
    half_num_eps = (num_eps >> 1);

    SHR_ALLOC(endpoint_info, sizeof(bcm_oam_endpoint_info_t), "Struct for creating OAM local endpoint",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(endpoint_info_non_acc, sizeof(bcm_oam_endpoint_info_t), "Struct for creating OAM local non acc endpoint",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(endpoint_info1, sizeof(bcm_oam_endpoint_info_t), "Struct for creating OAM remote endpoints",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(endpoint_info2, sizeof(bcm_oam_endpoint_info_t), "Struct for reading OAM remote endpoints",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(endpoint_id, num_eps * sizeof(bcm_oam_endpoint_t), "Array of Remote endpoint IDs",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Group creation */
    bcm_oam_group_info_t_init(&group_info);
    sal_memcpy(group_info.name, &group_name, sizeof(group_name));
    SHR_CLI_EXIT_IF_ERR(bcm_oam_group_create(unit, &group_info), "Test failed.  Could not create group\n");
    group_id = group_info.id;

    /** create In-LIF */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.vsi = 0;
    vlan_port.port = 13;

    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed. Could not create In-LIF\n");

    /** Create a acc profile for ingress with specific id */
    ingress_acc_profile = 0xb;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_create
                        (unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileIngressAcceleratedEndpoint, &ingress_acc_profile),
                        "Test failed. Could not create ingress acc profile\n");

    /** Create a profile for ingress with specific id */
    ingress_lif_profile = 0xa;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_create
                        (unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileIngressLIF, &ingress_lif_profile),
                        "Test failed. Could not create ingress profile\n");

    /** Create a profile for egress with specific id */
    egress_lif_profile = 0xc;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_create
                        (unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileEgressLIF, &egress_lif_profile),
                        "Test failed. Could not create egress profile\n");

    /** Bind lif with profile */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_lif_profile_set
                        (unit, 0, vlan_port.vlan_port_id, ingress_lif_profile, egress_lif_profile),
                        "Test failed. Could not bind profiles to lif\n");

    /** Create local non acc endpoint */
    bcm_oam_endpoint_info_t_init(endpoint_info_non_acc);
    /** Fill the structure for local non acc endpoint create  */
    endpoint_info_non_acc->group = group_id;
    endpoint_info_non_acc->gport = vlan_port.vlan_port_id;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info_non_acc),
                        "Test failed.  Could not create local non acc endpoint\n");

    bcm_oam_endpoint_info_t_init(endpoint_info1);
    bcm_oam_endpoint_info_t_init(endpoint_info2);
    /** Fill the structure with the user provided parameters   */
    endpoint_info1->flags = BCM_OAM_ENDPOINT_REMOTE;
    endpoint_info1->id = 0;
    endpoint_info1->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    endpoint_info1->sampling_ratio = 1;
    endpoint_info1->loc_clear_threshold = 2;
    endpoint_info1->name = 2000;

    /*
     * First try with non acc local endpoint. It should throw error 
     */
    endpoint_info1->local_id = endpoint_info_non_acc->id;
    rv = bcm_oam_endpoint_create(unit, endpoint_info1);
    if (rv != _SHR_E_PARAM)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed. Should throw E_PARAM for non acc local endpoint  .\n");
    }

    /*
     * Delete the non acc endpoint 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_destroy(unit, endpoint_info_non_acc->id),
                        "Test failed.  Could not destroy local non acc endpoint\n");

    /** Create local endpoint first */
    bcm_oam_endpoint_info_t_init(endpoint_info);
    /** Fill the structure for local acc endpoint create  */
    endpoint_info->group = group_id;
    endpoint_info->opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    endpoint_info->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    endpoint_info->name = 1000;
    endpoint_info->gport = vlan_port.vlan_port_id;
    endpoint_info->acc_profile_id = ingress_acc_profile;
    BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, 13);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info),
                        "Test failed.  Could not create local endpoint\n");

    /*
     * Change it to acc local endpoint for further testing. 
     */
    endpoint_info1->local_id = endpoint_info->id;

    for (ep_index = 0; ep_index < num_eps; ep_index++)
    {
        endpoint_info1->name = 2000 + ep_index;

        /** Create OAM Remote endpoint   */
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info1), "Test failed.  Could not create endpoint\n");
        LOG_CLI((BSL_META("Added endpoint #%d\n"), endpoint_info1->id));

        /** Save endpoint ID for later   */
        endpoint_id[ep_index] = endpoint_info1->id;

        /** Read endpoint parameters and compare   */
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, endpoint_id[ep_index], endpoint_info2),
                            "Test failed.  Endpoint could not be read.\n");
        LOG_CLI((BSL_META("Checking values for endpoint #%d\n"), endpoint_id[ep_index]));
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_rmep_test_compare_oam_info_structs(unit, endpoint_info1, endpoint_info2),
                            "Test failed.  Parameters read did not match parameters written.\n");

        /** Modify OAM remote endpoint   */
        endpoint_info1->flags |= BCM_OAM_ENDPOINT_REPLACE | BCM_OAM_ENDPOINT_WITH_ID;
        endpoint_info1->loc_clear_threshold = 3;

        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info1), "Test failed.  Could not modify endpoint\n");
        LOG_CLI((BSL_META("Modify endpoint #%d\n"), endpoint_id[ep_index]));

        /** Read and compare again   */
        endpoint_info1->flags &= ~BCM_OAM_ENDPOINT_REPLACE;
        endpoint_info1->flags &= ~BCM_OAM_ENDPOINT_WITH_ID;
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, endpoint_id[ep_index], endpoint_info2),
                            "Test failed.  Endpoint could not be read.\n");
        LOG_CLI((BSL_META("Checking values for endpoint #%d\n"), endpoint_id[ep_index]));
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_rmep_test_compare_oam_info_structs(unit, endpoint_info1, endpoint_info2),
                            "Test failed.  Parameters read did not match modified parameters written.\n");
        LOG_CLI((BSL_META("Read modified endpoint #%d\n"), endpoint_id[ep_index]));

    }

    /** Delete half of the endpoints */
    for (ep_index = 0; ep_index < half_num_eps; ep_index++)
    {
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_destroy(unit, endpoint_id[ep_index]),
                            "Test failed.  Could not delete endpoint.\n");
        LOG_CLI((BSL_META("Deleted endpoint #%d\n"), endpoint_id[ep_index]));

        rv = bcm_oam_endpoint_get(unit, endpoint_id[ep_index], endpoint_info2);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  Endpoint #%d not really deleted.\n", endpoint_id[ep_index]);
        }
    }

    /** Delete the rest   */
    for (ep_index = half_num_eps; ep_index < num_eps; ep_index++)
    {
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_destroy(unit, endpoint_id[ep_index]),
                            "Test failed.  Could not delete endpoint.\n");
        LOG_CLI((BSL_META("Deleted endpoint #%d\n"), endpoint_id[ep_index]));

        rv = bcm_oam_endpoint_get(unit, endpoint_id[ep_index], endpoint_info2);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  Endpoint #%d not really deleted.\n", endpoint_id[ep_index]);
        }
    }
    LOG_CLI((BSL_META("Deleted all remaining endpoints\n")));

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_FREE(endpoint_info);
    SHR_FREE(endpoint_info_non_acc);
    SHR_FREE(endpoint_info1);
    SHR_FREE(endpoint_info2);
    SHR_FREE(endpoint_id);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function tests the OAM LMEP feature in a very basic 
 *        way: an  LMEP endpoint is created and
 *        multiple remote endpoints are created for this LMEP. When the
 *        LMEP is deleted, it should result in all the RMEPs deletion. 
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line 
 *        arguments were parsed (partially)
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
diag_dnx_oam_lmep_semantic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 num_eps = 2, ep_index;
    int rv;
    bcm_oam_endpoint_t *endpoint_id = NULL;
    bcm_oam_endpoint_info_t *endpoint_info = NULL, *endpoint_info1 = NULL;
    bcm_oam_group_info_t group_info;
    bcm_vlan_port_t vlan_port;
    bcm_oam_profile_t ingress_acc_profile, ingress_lif_profile, egress_lif_profile;
    uint8 group_name[] = { 0x1, 0x3, 0x2, 0xd, 0xe };
    bcm_oam_group_t group_id = -1;
    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Test with %d remote endpoints\n"), num_eps));

    SHR_ALLOC(endpoint_info, sizeof(bcm_oam_endpoint_info_t), "Struct for creating OAM local endpoint",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(endpoint_info1, sizeof(bcm_oam_endpoint_info_t), "Struct for creating OAM remote endpoints",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(endpoint_id, num_eps * sizeof(bcm_oam_endpoint_t), "Array of Remote endpoint IDs",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Group creation */
    bcm_oam_group_info_t_init(&group_info);
    sal_memcpy(group_info.name, &group_name, sizeof(group_name));
    SHR_CLI_EXIT_IF_ERR(bcm_oam_group_create(unit, &group_info), "Test failed.  Could not create group\n");
    group_id = group_info.id;

    /** create In-LIF */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.vsi = 0;
    vlan_port.port = 13;

    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed. Could not create In-LIF\n");

    /** Create a acc profile for ingress with specific id */
    ingress_acc_profile = 0xb;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_create
                        (unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileIngressAcceleratedEndpoint, &ingress_acc_profile),
                        "Test failed. Could not create ingress acc profile\n");

    /** Create a profile for ingress with specific id */
    ingress_lif_profile = 0xa;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_create
                        (unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileIngressLIF, &ingress_lif_profile),
                        "Test failed. Could not create ingress profile\n");

    /** Create a profile for egress with specific id */
    egress_lif_profile = 0xc;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_create
                        (unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileEgressLIF, &egress_lif_profile),
                        "Test failed. Could not create egress profile\n");

    /** Bind lif with profile */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_lif_profile_set
                        (unit, 0, vlan_port.vlan_port_id, ingress_lif_profile, egress_lif_profile),
                        "Test failed. Could not bind profiles to lif\n");

    /** Create local endpoint first */
    bcm_oam_endpoint_info_t_init(endpoint_info);
    /** Fill the structure for local acc endpoint create  */
    endpoint_info->group = group_id;
    endpoint_info->opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    endpoint_info->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    endpoint_info->name = 1000;
    endpoint_info->gport = vlan_port.vlan_port_id;
    endpoint_info->acc_profile_id = ingress_acc_profile;
    BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, 13);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info),
                        "Test failed.  Could not create local endpoint\n");

    bcm_oam_endpoint_info_t_init(endpoint_info1);
    /** Fill the structure with the user provided parameters   */
    endpoint_info1->flags = BCM_OAM_ENDPOINT_REMOTE;
    endpoint_info1->id = 0;
    endpoint_info1->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    endpoint_info1->sampling_ratio = 1;
    endpoint_info1->loc_clear_threshold = 2;
    endpoint_info1->name = 2000;
    endpoint_info1->local_id = endpoint_info->id;

    for (ep_index = 0; ep_index < num_eps; ep_index++)
    {
        endpoint_info1->name = 2000 + ep_index;

        /** Create OAM Remote endpoint   */
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info1), "Test failed.  Could not create endpoint\n");
        LOG_CLI((BSL_META("Added endpoint #%d\n"), endpoint_info1->id));

        /** Save endpoint ID for later   */
        endpoint_id[ep_index] = endpoint_info1->id;
    }

    /** Try creating the same remote endpoints. It should return E_EXISTS. */
    for (ep_index = 0; ep_index < num_eps; ep_index++)
    {
        endpoint_info1->name = 2000 + ep_index;
        endpoint_info1->id = endpoint_id[ep_index] & 0xFFFFFF;
        endpoint_info1->flags |= BCM_OAM_ENDPOINT_WITH_ID;
        rv = bcm_oam_endpoint_create(unit, endpoint_info1);
        if (rv != _SHR_E_EXISTS)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  Expected E_EXISTS. Got %d\n", rv);
        }
    }

    /** Delete the local endpoint. */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_destroy(unit, endpoint_info->id),
                        "Test failed. Local endpoint cannot be deleted\n");

    /*
     * Re-create the local endpoint and remote endpoints. It should go through. 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info),
                        "Test failed.  Could not re-create local endpoint\n");
    for (ep_index = 0; ep_index < num_eps; ep_index++)
    {
        endpoint_info1->name = 2000 + ep_index;
        endpoint_info1->id = endpoint_id[ep_index] & 0xFFFFFF;
        endpoint_info1->flags |= BCM_OAM_ENDPOINT_WITH_ID;
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info1),
                            "Test failed.  Could not re-create remote endpoint\n");
    }

    /** Finally Delete the local endpoint to cleanup. */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_destroy(unit, endpoint_info->id),
                        "Test failed. Local endpoint cannot be deleted\n");
    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_FREE(endpoint_info);
    SHR_FREE(endpoint_info1);
    SHR_FREE(endpoint_id);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function tests the bcm_oam_endpoint_traverse functionality.
 *        3 endpoint are created and the bcm_oam_endpoint_traverse API
 *        is called to update the ccm_period of all endpoins associated with
 *        specific group.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line
 *        arguments were parsed (partially)
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
diag_dnx_oam_endpoint_traverse_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int ep_index;
    int num_eps = 3;
    int new_period;
    bcm_oam_endpoint_info_t *endpoint_info = NULL;
    bcm_oam_group_info_t group_info;
    bcm_vlan_port_t vlan_port;
    bcm_oam_profile_t ingress_acc_profile, ingress_lif_profile, egress_lif_profile;
    uint8 group_name[] = { 0x1, 0x3, 0x2, 0xd, 0xe };
    bcm_oam_group_t group_id = 3;
    SHR_FUNC_INIT_VARS(unit);

    /** Set values for tunnel-type loop   */
    SHR_ALLOC(endpoint_info, sizeof(bcm_oam_endpoint_info_t), "Struct for creating OAM local endpoint",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Group creation */
    bcm_oam_group_info_t_init(&group_info);
    sal_memcpy(group_info.name, &group_name, sizeof(group_name));
    SHR_CLI_EXIT_IF_ERR(bcm_oam_group_create(unit, &group_info), "Test failed.  Could not create group\n");
    group_id = group_info.id;

    /** create In-LIF */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.vsi = 0;
    vlan_port.port = 13;

    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed. Could not create In-LIF\n");

    /** Create a acc profile for ingress with specific id */
    ingress_acc_profile = 0xb;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_create
                        (unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileIngressAcceleratedEndpoint, &ingress_acc_profile),
                        "Test failed. Could not create ingress acc profile\n");

    /** Create a profile for ingress with specific id */
    ingress_lif_profile = 0xa;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_create
                        (unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileIngressLIF, &ingress_lif_profile),
                        "Test failed. Could not create ingress profile\n");

    /** Create a profile for egress with specific id */
    egress_lif_profile = 0xc;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_create
                        (unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileEgressLIF, &egress_lif_profile),
                        "Test failed. Could not create egress profile\n");

    /** Bind lif with profile */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_lif_profile_set
                        (unit, 0, vlan_port.vlan_port_id, ingress_lif_profile, egress_lif_profile),
                        "Test failed. Could not bind profiles to lif\n");

    /** Create 3 local endpoint first */
    bcm_oam_endpoint_info_t_init(endpoint_info);
    /** Fill the structure for local acc endpoint create  */
    endpoint_info->group = group_id;
    endpoint_info->opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    endpoint_info->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    endpoint_info->gport = vlan_port.vlan_port_id;
    endpoint_info->acc_profile_id = ingress_acc_profile;
    BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, 13);

    for (ep_index = 0; ep_index < num_eps; ep_index++)
    {
        endpoint_info->name = ep_index;
        endpoint_info->level = ep_index;
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info),
                            "Test failed.  Could not create local endpoint\n");
    }

    new_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_traverse(unit, group_id, &test_oam_endpoint_period_update_cb, &new_period),
                        "Test failed.  Could not update the ccm_period for all endpoints \n");

    for (ep_index = 0; ep_index < num_eps; ep_index++)
    {
        /** Read endpoint parameters and compare   */
        SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, endpoint_info->id, endpoint_info),
                            "Test failed.  Endpoint could not be read.\n");

        /** Check if the ccm_period is updated   */
        if (endpoint_info->ccm_period != new_period)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed. ccm_period  read don't match ccm_period written to endpoint.  Written: 0x%08X. Read: 0x%08X.\n",
                         endpoint_info->ccm_period, new_period);
        }
    }

exit:
    SHR_FREE(endpoint_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set callback.
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
diag_dnx_oam_cb(
    int unit,
    uint32 flags,
    bcm_oam_event_type_t event_type,
    bcm_oam_group_t group,
    bcm_oam_endpoint_t endpoint,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Flags %d, Event type %d, Group %d, Endpoint 0x%x .\n"), flags, event_type, group, endpoint));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register events.
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
diag_dnx_oam_register_events(
    int unit)
{
    bcm_oam_event_types_t timeout_event, timein_event;
    SHR_FUNC_INIT_VARS(unit);

    BCM_OAM_EVENT_TYPE_CLEAR_ALL(timeout_event);
    BCM_OAM_EVENT_TYPE_CLEAR_ALL(timein_event);

    BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timeout_event, diag_dnx_oam_cb, (void *) 1),
                        "Test failed! Register of timeout event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 2),
                        "Test failed! Register of timein event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemote);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 3),
                        "Test failed! Register of rdi event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemoteUp);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 4),
                        "Test failed! Register of rdi event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointPortDown);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 5),
                        "Test failed! Register of rdi event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointPortUp);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 6),
                        "Test failed! Register of rdi event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointInterfaceDown);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 7),
                        "Test failed! Register of rdi event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointInterfaceUp);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 8),
                        "Test failed! Register of rdi event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointInterfaceTesting);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 9),
                        "Test failed! Register of rdi event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointInterfaceDormant);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 10),
                        "Test failed! Register of rdi event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointInterfaceUnkonwn);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 11),
                        "Test failed! Register of rdi event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointInterfaceNotPresent);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 12),
                        "Test failed! Register of rdi event failed.");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointInterfaceLLDown);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_event_register(unit, timein_event, diag_dnx_oam_cb, (void *) 13),
                        "Test failed! Register of rdi event failed.");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates a new snoop command and a new user defined trap
 *          associated with this command.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  sample_rate_dividend - Set probability of snooping a packet:
 *              sample_rate_dividend >= sample_rate_divisor ? 1 :
 *                                      sample_rate_dividend/sample_rate_divisor
 * \param [in]  sample_rate_divisor
 * \param [out] snoop_gport - Encoded trap code, trap and snoop strength as a gport.
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
diag_dnx_oam_snoop_config(
    int unit,
    uint32 sample_rate_dividend,
    uint32 sample_rate_divisor,
    bcm_gport_t * snoop_gport)
{
    int is_snoop;
    int trap_code;
    int count;
    int flags = 0;
    bcm_gport_t oamp_gports[2];
    bcm_mirror_destination_t mirror_dest;
    bcm_rx_trap_config_t trap_config_snoop;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create a new snoop command to be associated with the new trap 
     */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP;
    mirror_dest.gport = 0;
    mirror_dest.sample_rate_dividend = sample_rate_dividend;
    mirror_dest.sample_rate_divisor = sample_rate_divisor;
    SHR_CLI_EXIT_IF_ERR(bcm_mirror_destination_create(unit, &mirror_dest),
                        "Test failed. Could not create snoop command.\n");

    /*
     * Check if snoop was created 
     */
    is_snoop = BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest.mirror_dest_id);
    if (!is_snoop)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed. Could not create snoop.\n");
    }

    /*
     * Create a new trap 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_code),
                        "Test failed. Could not create trap.\n");

    /*
     * Get OAMP gport 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 2, oamp_gports, &count),
                        "Test failed. OAMP gport not found.\n");

    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
    trap_config_snoop.snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);
    trap_config_snoop.dest_port = oamp_gports[0];
    SHR_CLI_EXIT_IF_ERR(bcm_rx_trap_set(unit, trap_code, &trap_config_snoop),
                        "Test failed. Could not configure trap.\n");

    /*
     * Encode trap code, trap strength and snoop strength as a gport 
     */
    BCM_GPORT_TRAP_SET(*snoop_gport, trap_code, 0, 3);

    LOG_CLI((BSL_META("Trap with ID=%d was created. Snoop was configured successfully.\n"), trap_code));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function sets the action for the given profile
 *          to snoop CCM packets to CPU.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  snoop_gport
 * \param [in]  acc_profile_id
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
diag_dnx_oam_action_set_snoop(
    int unit,
    bcm_gport_t snoop_gport,
    bcm_oam_profile_t acc_profile_id)
{
    uint32 flags = 0;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    SHR_FUNC_INIT_VARS(unit);

    bcm_oam_action_key_t_init(&action_key);
    bcm_oam_action_result_t_init(&action_result);
    /*
     * Set action's key 
     */
    action_key.opcode = bcmOamOpcodeCCM;
    action_key.endpoint_type = bcmOAMMatchTypeMEP;
    action_key.dest_mac_type = bcmOAMDestMacTypeMcast;
    /*
     * Set action's result 
     */
    action_result.destination = snoop_gport;
    /*
     * Update the action for the given profile 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_action_set(unit, flags, acc_profile_id, &action_key, &action_result),
                        "Test failed. Could not set snoop action.\n");

    LOG_CLI((BSL_META("Action for snooping CCM packets was set on acc profile.\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function sets the action for the given profile
 *          to trap LBR, LTR and LTM packets to CPU.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  trap_gport - CPU trap gport
 * \param [in]  acc_profile_id
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
diag_dnx_oam_action_set_cpu_trap(
    int unit,
    bcm_gport_t trap_gport,
    bcm_oam_profile_t acc_profile_id)
{
    int opcode_index;
    int num_opcodes = 3;
    uint32 flags = 0;
    bcm_oam_opcode_t opcodes[] = { bcmOamOpcodeLBR, bcmOamOpcodeLTR, bcmOamOpcodeLTM };
    bcm_oam_dest_mac_type_t mac_type;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    SHR_FUNC_INIT_VARS(unit);

    for (opcode_index = 0; opcode_index < num_opcodes; ++opcode_index)
    {
        for (mac_type = bcmOAMDestMacTypeMcast; mac_type <= bcmOAMDestMacTypeMyCfmMac; ++mac_type)
        {
            bcm_oam_action_key_t_init(&action_key);
            bcm_oam_action_result_t_init(&action_result);
            /*
             * Set action's key 
             */
            action_key.opcode = opcodes[opcode_index];
            action_key.endpoint_type = bcmOAMMatchTypeMEP;
            action_key.dest_mac_type = mac_type;
            /*
             * Set action's result 
             */
            action_result.destination = trap_gport;
            /*
             * Update the action for the given profile 
             */
            SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_action_set(unit, flags, acc_profile_id, &action_key, &action_result),
                                "Test failed. Could not set CPU trap.\n");
        }
    }

    LOG_CLI((BSL_META("Action for trapping LBR, LTR and LTM packets to CPU was set on acc profile.\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates OAM group.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  group_id - ID of OAM group created.
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
diag_dnx_oam_group_create(
    int unit,
    bcm_oam_group_t * group_id)
{
    uint8 group_name[] = { 0x1, 0x3, 0x2, 0xd, 0xe };
    bcm_oam_group_info_t group_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create OAM group 
     */
    bcm_oam_group_info_t_init(&group_info);
    sal_memcpy(group_info.name, &group_name, sizeof(group_name));
    SHR_CLI_EXIT_IF_ERR(bcm_oam_group_create(unit, &group_info), "Test failed. Could not create group.\n");

    LOG_CLI((BSL_META("Group with ID=%d was created.\n"), group_info.id));

    *group_id = group_info.id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates local accelerated endpoint.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  port - Port on which the MEP resides.
 * \param [in]  group_id - The MEG on which the MEP resides.
 * \param [in]  gport - Gport associated with the MEP.
 * \param [in]  profile_id - Ingress accelerated profile ID.
 * \param [out] mep_id - ID of MEP created.
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
diag_dnx_oam_local_endpoint_create_with_rmep(
    int unit,
    bcm_oam_group_t group_id,
    bcm_gport_t gport,
    bcm_oam_profile_t profile_id,
    bcm_oam_endpoint_t * mep_id)
{
    bcm_port_t port = 200;
    bcm_mac_t src_mac_mep = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x0d };
    bcm_oam_endpoint_info_t *endpoint_info = NULL;
    bcm_oam_endpoint_info_t *rmep_info = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(endpoint_info, sizeof(bcm_oam_endpoint_info_t), "Struct for creating OAM local endpoint",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(rmep_info, sizeof(bcm_oam_endpoint_info_t), "Struct for creating OAM remote endpoint",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Create local accelerated endpoint 
     */
    bcm_oam_endpoint_info_t_init(endpoint_info);
    endpoint_info->type = bcmOAMEndpointTypeEthernet;
    endpoint_info->group = group_id;
    endpoint_info->level = 2;
    endpoint_info->gport = gport;
    endpoint_info->acc_profile_id = profile_id;
    BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, port);
    endpoint_info->name = 2;
    endpoint_info->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    endpoint_info->opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    endpoint_info->vlan = 10;
    endpoint_info->pkt_pri = 2;
    endpoint_info->outer_tpid = 0x8100;
    sal_memcpy(endpoint_info->src_mac_address, &src_mac_mep, sizeof(src_mac_mep));
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info),
                        "Test failed. Could not create local accelerated endpoint.\n");

    LOG_CLI((BSL_META("Local endpoint with ID=%d was created.\n"), endpoint_info->id));

    *mep_id = endpoint_info->id;

    /*
     * Create remote endpoint 
     */
    bcm_oam_endpoint_info_t_init(rmep_info);
    rmep_info->type = bcmOAMEndpointTypeEthernet;
    rmep_info->name = 1;
    rmep_info->local_id = endpoint_info->id;
    rmep_info->gport = gport;
    rmep_info->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    rmep_info->flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info->id = endpoint_info->id;
    rmep_info->loc_clear_threshold = 1;
    rmep_info->level = 2;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, rmep_info), "Test failed. Could not create remote endpoint.\n");

    LOG_CLI((BSL_META("Remote endpoint with ID=%d was created.\n"), rmep_info->id));

exit:
    SHR_FREE(endpoint_info);
    SHR_FREE(rmep_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function updates CCM period of
 *          local accelerated endpoint.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [out] mep_id
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
diag_dnx_oam_endpoint_update(
    int unit,
    bcm_oam_endpoint_t mep_id)
{
    bcm_oam_endpoint_info_t *endpoint_info = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(endpoint_info, sizeof(bcm_oam_endpoint_info_t), "Struct for reading OAM local endpoint",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Update CCM period 
     */
    bcm_oam_endpoint_info_t_init(endpoint_info);
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_get(unit, mep_id, endpoint_info),
                        "Test failed. Could not get local accelerated endpoint.\n");

    endpoint_info->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    endpoint_info->flags |= BCM_OAM_ENDPOINT_REPLACE;
    endpoint_info->flags |= BCM_OAM_ENDPOINT_WITH_ID;
    SHR_CLI_EXIT_IF_ERR(bcm_oam_endpoint_create(unit, endpoint_info),
                        "Test failed. Could not update CCM period of local accelerated endpoint.\n");

    LOG_CLI((BSL_META("Updated endpoint with ID=%d.\n"), mep_id));

exit:
    SHR_FREE(endpoint_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function repeatedly creates MEPs and RMEPs.
 *          Configures snoop and cpu trap on acc profile and
 *          updates local endpoint CCM period.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments.
 * \param [in] sand_control - Structure into which command line
 *        arguments were parsed (partially).
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
diag_dnx_oam_reconfiguration_semantic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int index;
    int recreate_limit = 50;
    int replace_limit = 50;
    int trap_code_cpu;
    uint32 flags = 0;
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t snoop_gport_1, snoop_gport_2, cpu_trap_gport;
    bcm_port_t port = 200;
    bcm_vlan_port_t vlan_port;
    bcm_gport_t gport;
    bcm_oam_group_t group_id;
    uint8 raw_profile_id = 1;
    bcm_oam_profile_t ing_profile_id;
    bcm_oam_profile_t egr_profile_id;
    bcm_oam_profile_t ing_acc_profile_id;
    bcm_oam_endpoint_t mep_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure snoop 
     */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_snoop_config(unit, 1, 2, &snoop_gport_1),
                        "Test failed. Could not create snoop.\n");

    SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_snoop_config(unit, 1, 10, &snoop_gport_2),
                        "Test failed. Could not create snoop.\n");

    /*
     * Configure CPU trap 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code_cpu),
                        "Test failed. Could not create trap.\n");

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
    trap_config.dest_port = BCM_GPORT_LOCAL_CPU;
    SHR_CLI_EXIT_IF_ERR(bcm_rx_trap_set(unit, trap_code_cpu, &trap_config), "Test failed. Could not configure trap.\n");

    /*
     * Encode trap code, trap strength and snoop strength as a gport 
     */
    BCM_GPORT_TRAP_SET(cpu_trap_gport, trap_code_cpu, 7, 0);

    /*
     * Register events 
     */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_register_events(unit), "Test failed. Could not register events.\n");

    /*
     * Set port classification ID 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_port_class_set(unit, port, bcmPortClassId, port),
                        "Test failed. Could not set port classification ID.\n");

    /*
     * Create AC LIF 
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port;
    vlan_port.match_vlan = 10;
    vlan_port.egress_vlan = 10;
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed. Could not create AC LIF.\n");

    gport = vlan_port.vlan_port_id;

    /*
     * Get ingress profile 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_id_get_by_type(unit, raw_profile_id, bcmOAMProfileIngressLIF,
                                                       &flags, &ing_profile_id),
                        "Test failed. Could not get ingress profile.\n");

    /*
     * Get egress profile 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_id_get_by_type(unit, raw_profile_id, bcmOAMProfileEgressLIF,
                                                       &flags, &egr_profile_id),
                        "Test failed. Could not get egress profile.\n");

    /*
     * Get ingress accelerated profile 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_profile_id_get_by_type(unit, raw_profile_id, bcmOAMProfileIngressAcceleratedEndpoint,
                                                       &flags, &ing_acc_profile_id),
                        "Test failed. Could not get ingress acc profile.\n");

    /** Bind LIF with profiles */
    SHR_CLI_EXIT_IF_ERR(bcm_oam_lif_profile_set(unit, flags, gport, ing_profile_id, egr_profile_id),
                        "Test failed. Could not bind profiles to LIF.\n");

    /*
     * Create OAM group
     */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_group_create(unit, &group_id), "Test failed. Could not create group.\n");

    /*
     * Create local accelerated endpoint 
     */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_local_endpoint_create_with_rmep
                        (unit, group_id, gport, ing_acc_profile_id, &mep_id),
                        "Test failed. Could not create accelerated endpoint.\n");

    /*
     * Snoop action set 
     */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_action_set_snoop(unit, snoop_gport_1, ing_acc_profile_id),
                        "Test failed. Could not set snoop action.\n");

    /*
     * CPU trap set 
     */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_action_set_cpu_trap(unit, cpu_trap_gport, ing_acc_profile_id),
                        "Test failed. Could not set CPU trap.\n");

    /*
     * Repeatedly delete everything and re-create 
     */
    for (index = 0; index < recreate_limit; ++index)
    {
        LOG_CLI((BSL_META("Destroy & re-create attempt -- iteration=%d\n"), index));
        SHR_CLI_EXIT_IF_ERR(bcm_oam_group_destroy(unit, group_id), "Test failed. Could not destroy group.\n");

        /*
         * Create OAM group
         */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_group_create(unit, &group_id), "Test failed. Could not create group.\n");

        /*
         * Create local accelerated endpoint 
         */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_local_endpoint_create_with_rmep
                            (unit, group_id, gport, ing_acc_profile_id, &mep_id),
                            "Test failed. Could not create local accelerated endpoint.\n");

        /*
         * Snoop action set 
         */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_action_set_snoop(unit, snoop_gport_1, ing_acc_profile_id),
                            "Test failed. Could not set snoop action.\n");

        /*
         * CPU trap set 
         */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_action_set_cpu_trap(unit, cpu_trap_gport, ing_acc_profile_id),
                            "Test failed. Could not set CPU trap.\n");
    }

    /*
     * Do replace of endpoint toggling between two snoops 
     */
    for (index = 0; index < replace_limit; ++index)
    {
        LOG_CLI((BSL_META("Replace attempt -- iteration=%d\n"), index));
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_endpoint_update(unit, mep_id), "Test failed. Could not update endpoint.\n");

        /*
         * Snoop action set 
         */
        if (index % 2 == 1)
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_action_set_snoop(unit, snoop_gport_1, ing_acc_profile_id),
                                "Test failed. Could not set snoop action.\n");
        }
        else
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_action_set_snoop(unit, snoop_gport_2, ing_acc_profile_id),
                                "Test failed. Could not set snoop action.\n");
        }

        /*
         * CPU trap set 
         */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oam_action_set_cpu_trap(unit, cpu_trap_gport, ing_acc_profile_id),
                            "Test failed. Could not set CPU trap.\n");
    }

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_FUNC_EXIT;
}

/** Test arguments   */
static sh_sand_option_t diag_dnx_oam_rmep_sem_test_options[] = {
    {"Number", SAL_FIELD_TYPE_INT32, "Number of endpoints", "4"},
    {NULL}
};

/** Test arguments   */
static sh_sand_option_t diag_dnx_oam_lmep_sem_test_options[] = {
    {NULL}
};

/** Test arguments   */
static sh_sand_option_t diag_dnx_oam_endpoint_traverse_sem_test_options[] = {
    {"Number", SAL_FIELD_TYPE_INT32, "Number of endpoints", "4"},
    {NULL}
};

/** Test arguments   */
static sh_sand_option_t diag_dnx_oam_reconfiguration_sem_test_options[] = {
    {NULL}
};

/** Test manual   */
static sh_sand_man_t diag_dnx_oam_rmep_sem_test_man = {
    /** Brief */
    "Semantic test of basic OAM RMEP related APIs",
    /** Full */
    "Create OAM Remote endpoints.  Get endpoints and compare."
        " Modify some endpoints.  Get endpoints and compare."
        " Delete half the endpoints individually." " Delete the rest of the endpoints.",
    /** Synopsis   */
    "ctest oam rmep [Number=<value>]",
    /** Example   */
    "Number=3",
};

static sh_sand_man_t diag_dnx_oam_lmep_sem_test_man = {
    /** Brief */
    "Semantic test of basic OAM LMEP related APIs",
    /** Full */
    "Create one OAM local endpoint."
        " Add remote endpoints to that local endpoint"
        " Delete the local endpoint" "Check that the remote endpoints are deleted as well.",
    /** Synopsis   */
    "ctest oam lmep",
    /** Example   */
    NULL,
};

static sh_sand_man_t diag_dnx_oam_endpoint_traverse_sem_test_man = {
    /** Brief */
    "Semantic test of basic OAM LMEP related APIs",
    /** Full */
    "Create one OAM local endpoint."
        " Delete the local endpoint" "Check that the remote endpoints are deleted as well.",
    /** Synopsis   */
    "ctest oam endpoint_traverse",
    /** Example   */
    NULL,
};

static sh_sand_man_t diag_dnx_oam_reconfiguration_sem_test_man = {
    /** Brief */
    "Semantic OAM reconfiguration test",
    /** Full */
    "Repeatedly create MEP and RMEP." "Configure snoop and cpu trap." "Replace MEP.",
    /** Synopsis   */
    "ctest oam reconfiguration",
    /** Example   */
    NULL,
};

/** Automatic test list (ctest pp oam semantic_rmep run)   */
static sh_sand_invoke_t diag_dnx_oam_rmep_sem_tests[] = {
    {"rmeps_4", "Number=4"},
    {NULL}
};

/** Automatic test list (ctest pp oam semantic_lmep run)   */
static sh_sand_invoke_t diag_dnx_oam_lmep_sem_tests[] = {
    {"lmeps_cr_dest", NULL},
    {NULL}
};

/** Automatic test list (ctest pp oam semantic_endpoint_traverse run)   */
static sh_sand_invoke_t diag_dnx_oam_endpoint_traverse_sem_tests[] = {
    {"endpoint_traverse", "Number=4"},
    {NULL}
};

/** Automatic test list (ctest pp oam semantic_reconfiguration run)   */
static sh_sand_invoke_t diag_dnx_oam_reconfiguration_sem_tests[] = {
    {"reconfiguration", NULL},
    {NULL}
};

/** List of OAM tests   */
sh_sand_cmd_t dnx_oam_test_cmds[] = {
    {"rmep", diag_dnx_oam_rmep_semantic_test_cmd, NULL, diag_dnx_oam_rmep_sem_test_options,
     &diag_dnx_oam_rmep_sem_test_man, NULL,
     diag_dnx_oam_rmep_sem_tests},
    {"lmep", diag_dnx_oam_lmep_semantic_test_cmd, NULL, diag_dnx_oam_lmep_sem_test_options,
     &diag_dnx_oam_lmep_sem_test_man, NULL,
     diag_dnx_oam_lmep_sem_tests},
    {"traverse", diag_dnx_oam_endpoint_traverse_test_cmd, NULL, diag_dnx_oam_endpoint_traverse_sem_test_options,
     &diag_dnx_oam_endpoint_traverse_sem_test_man, NULL,
     diag_dnx_oam_endpoint_traverse_sem_tests},
    {"reconfiguration", diag_dnx_oam_reconfiguration_semantic_test_cmd, NULL,
     diag_dnx_oam_reconfiguration_sem_test_options,
     &diag_dnx_oam_reconfiguration_sem_test_man, NULL,
     diag_dnx_oam_reconfiguration_sem_tests},
    {NULL}
};
