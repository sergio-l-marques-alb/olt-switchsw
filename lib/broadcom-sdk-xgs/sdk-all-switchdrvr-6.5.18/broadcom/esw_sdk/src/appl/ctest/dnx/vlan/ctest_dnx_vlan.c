/** \file diag_dnx_vlan_tests.c
 * 
 * Tests for vlan
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
#include <bcm/l3.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/dbal/dbal.h>

/**
 * \brief - This function tests the vlan and L3 interface creation order.
 *          vlan creation must be done before L3 interface creation.
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
diag_dnx_vlan_semantic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int eth_rif_id, vid;
    bcm_l3_intf_t l3if;
    bsl_severity_t original_severity_sw_state;

    SHR_FUNC_INIT_VARS(unit);

    original_severity_sw_state = BSL_INFO;
    eth_rif_id = vid = 200;     /* random value */

    SHR_CLI_EXIT_IF_ERR(bcm_vlan_create(unit, vid), "Test failed. Could not create vlan\n");
    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    l3if.l3a_mac_addr[5] = 0xff;
    l3if.l3a_intf_id = l3if.l3a_vid = eth_rif_id;
    l3if.dscp_qos.qos_map_id = 0;

    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_create(unit, &l3if), "Test failed. Could not create ETH-RIF\n");

    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_delete(unit, &l3if), "Test failed. Could not delete ETH-RIF\n");

    SHR_CLI_EXIT_IF_ERR(bcm_vlan_destroy(unit, vid), "Test failed. Could not create vlan\n");

    /*
     * save the original severity level of the sw state and severity level to bslSeverityFatal..
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_VLAN, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_VLAN, bslSeverityFatal);

    /*
     * Create L3 interface and then create vlan. Should fail
     */
    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_create(unit, &l3if), "Test failed. Could not create ETH-RIF\n");

    SHR_CLI_EXIT_IF_ERR(bcm_vlan_create(unit, vid), "Test failed. could not create VSI after ETH-RIF\n");

    /*
     * Cleanup
     */
    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_delete(unit, &l3if), "Test failed. Could not delete ETH-RIF second attempt\n");
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_destroy(unit, vid), "Test failed. Could not delete Vid\n");

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_VLAN, original_severity_sw_state);
    SHR_FUNC_EXIT;
}

/** Test manual   */
static sh_sand_man_t diag_dnx_vlan_sem_test_man = {
    /** Brief */
    "Semantic test of vlan related APIs",
    /** Full */
    "Create vlan.  Create L3 interface." "Delete L3 interface. Destroy vlan." "Create L3 interface. Create vlan. ",
    /** Synopsis   */
    "ctest vlan semantic"
};

/** List of vlan tests   */
sh_sand_cmd_t dnx_vlan_test_cmds[] = {
    {"semantic", diag_dnx_vlan_semantic_test_cmd, NULL, NULL, &diag_dnx_vlan_sem_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {NULL}
};
