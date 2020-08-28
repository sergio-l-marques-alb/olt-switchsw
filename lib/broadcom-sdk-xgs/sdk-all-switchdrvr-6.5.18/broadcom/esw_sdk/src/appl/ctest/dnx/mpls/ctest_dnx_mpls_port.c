/** \file ctest_dnx_mpls_port.c
 *
 * Tests for MPLS PORT
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

#include <bcm/mpls.h>
#include <bcm/vlan.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>

int
dnx_mpls_port_delete_callback(
    int unit,
    bcm_mpls_port_t * mpls_port,
    void *user_data_null)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_mpls_port_delete(unit, 0, mpls_port->mpls_port_id));
exit:
    SHR_FUNC_EXIT;
}

int
dnx_mpls_port_counter_callback(
    int unit,
    bcm_mpls_port_t * mpls_port,
    void *counter)
{
    int cur_count;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(counter, _SHR_E_PARAM, "counter");

    cur_count = *((int *) counter);
    *((int *) counter) = cur_count + 1;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Test for MPLS PORT traverse.
 * For detailed description of the test, see the manual.
 */
static shr_error_e
dnx_mpls_port_traverse_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int counter = 0, ii;
    bcm_mpls_port_traverse_info_t additional_info = { 0 };

    bcm_mpls_port_t mpls_port;
    bcm_mpls_port_t *mpls_port_p;
    bcm_mpls_egress_label_t egress_label;
    bcm_mpls_egress_label_t *egress_label_p;
    int const nof_egress_entries = 20;
    int const nof_ingress_entries = 16;
    uint32 mpls_port_ing_base_id = 5000;
    uint32 mpls_port_egr_base_id = 6000;
    uint32 match_ing_label_base = 7000;
    uint32 match_egr_label_base = 8000;
    uint32 match_ing_encap_id_base = 0x500;
    uint32 match_egr_encap_id_base = 0x600;
    uint32 match_ing_egress_tunnel_if_base = 0x20000000;
    uint32 egr_label_flags;

    SHR_FUNC_INIT_VARS(unit);

    mpls_port_p = &mpls_port;
    egress_label_p = &egress_label;

    egr_label_flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    LOG_CLI((BSL_META("1. Create %d Ingress entries with cyclic VPN, incremental ID and label\n"),
             nof_ingress_entries));
    for (ii = 0; ii < nof_ingress_entries; ii++)
    {
        int vpn = ii % 2;
        bcm_gport_t gport;
        bcm_mpls_port_t_init(mpls_port_p);
        mpls_port_p->criteria = BCM_MPLS_PORT_MATCH_LABEL;
        mpls_port_p->flags = BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
        mpls_port_p->flags2 = BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_STAT_ENABLE;
        if (vpn == 0)
        {
            mpls_port_p->flags2 |= BCM_MPLS_PORT2_CROSS_CONNECT;
        }
        mpls_port_p->ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        mpls_port_p->ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        mpls_port_p->ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
        mpls_port_p->encap_id = match_ing_encap_id_base + ii;
        mpls_port_p->egress_tunnel_if = match_ing_egress_tunnel_if_base + ii;

        BCM_GPORT_SUB_TYPE_LIF_SET(gport, _SHR_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, mpls_port_ing_base_id + ii);
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_p->mpls_port_id, gport);

        mpls_port_p->match_label = match_ing_label_base + ii;
        SHR_IF_ERR_EXIT(bcm_mpls_port_add(unit, vpn, mpls_port_p));
    }

    LOG_CLI((BSL_META("2. Create %d Egress entries with incremental ID and label\n"), nof_egress_entries));
    for (ii = 0; ii < nof_egress_entries; ii++)
    {
        bcm_gport_t gport;
        bcm_mpls_port_t_init(mpls_port_p);
        bcm_mpls_egress_label_t_init(egress_label_p);
        egress_label_p->egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        egress_label_p->egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        egress_label_p->exp = ii % 8;
        egress_label_p->ttl = 50 + ii % 8;
        egress_label_p->flags = egr_label_flags;
        egress_label_p->label = match_egr_label_base + ii;
        mpls_port_p->egress_label = egress_label;
        mpls_port_p->flags = BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
        mpls_port_p->flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
        mpls_port_p->encap_id = match_egr_encap_id_base + ii;
        BCM_GPORT_SUB_TYPE_LIF_SET(gport, _SHR_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, mpls_port_egr_base_id + ii);
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_p->mpls_port_id, gport);
        SHR_IF_ERR_EXIT(bcm_mpls_port_add(unit, 0, mpls_port_p));
    }

    LOG_CLI((BSL_META("3. Call traverse with Illegal flags - should fail with PARAMS\n")));
    additional_info.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_EGRESS_ONLY;
    SHR_ASSERT_EQ(_SHR_E_PARAM,
                  bcm_mpls_port_traverse(unit, additional_info, dnx_mpls_port_counter_callback, &counter));

    LOG_CLI((BSL_META("4. Call traverse with VPN and Egress Only flag - should fail with PARAMS\n")));
    additional_info.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    additional_info.vpn = 1;
    SHR_ASSERT_EQ(_SHR_E_PARAM,
                  bcm_mpls_port_traverse(unit, additional_info, dnx_mpls_port_counter_callback, &counter));

    LOG_CLI((BSL_META("5. Use traverse to count Ingress entries (no VPN rule)\n")));
    counter = 0;
    additional_info.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
    additional_info.vpn = 0;
    SHR_IF_ERR_EXIT(bcm_mpls_port_traverse(unit, additional_info, dnx_mpls_port_counter_callback, &counter));
    SHR_ASSERT_EQ(nof_ingress_entries, counter);

    LOG_CLI((BSL_META("6. Use traverse to count Ingress entries VPN = 1\n")));
    counter = 0;
    additional_info.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
    additional_info.vpn = 1;
    SHR_IF_ERR_EXIT(bcm_mpls_port_traverse(unit, additional_info, dnx_mpls_port_counter_callback, &counter));
    SHR_ASSERT_EQ(nof_ingress_entries / 2, counter);

    LOG_CLI((BSL_META("7. Use traverse to count Egress entries\n")));
    counter = 0;
    additional_info.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    additional_info.vpn = 0;
    SHR_IF_ERR_EXIT(bcm_mpls_port_traverse(unit, additional_info, dnx_mpls_port_counter_callback, &counter));
    SHR_ASSERT_EQ(nof_egress_entries, counter);

    LOG_CLI((BSL_META("8. Use traverse to delete all entries\n")));
    counter = 0;
    additional_info.flags2 = 0;
    additional_info.vpn = 0;
    SHR_IF_ERR_EXIT(bcm_mpls_port_traverse(unit, additional_info, dnx_mpls_port_delete_callback, NULL));

    LOG_CLI((BSL_META("9. Use traverse to verify no Ingress entries\n")));
    counter = 0;
    additional_info.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
    additional_info.vpn = 0;
    SHR_IF_ERR_EXIT(bcm_mpls_port_traverse(unit, additional_info, dnx_mpls_port_counter_callback, &counter));
    SHR_ASSERT_EQ(0, counter);

    LOG_CLI((BSL_META("10. Use traverse to verify no Egress entries\n")));
    counter = 0;
    additional_info.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    additional_info.vpn = 0;
    SHR_IF_ERR_EXIT(bcm_mpls_port_traverse(unit, additional_info, dnx_mpls_port_counter_callback, &counter));
    SHR_ASSERT_EQ(0, counter);

    LOG_CLI(("\n\n\n >>> Test PASSED <<<\n\n\n"));

exit:
    SHR_FUNC_EXIT;
}

/** Test manual   */
sh_sand_man_t dnx_mpls_port_traverse_test_man = {
    /** Brief */
    "Semantic test of MPLS PORT traverse API",
    /** Full */
    "Semantic test of MPLS PORT traverse API\n"
        "1. Create Ingress entries with incremental ID, VPN, and label\n"
        "2. Create Egress entries with incremental ID and label\n"
    /** Synopsis */
        "ctest mpls port traverse",
    /** Examples */
    "ctest mpls port traverse"
};

/**
 * \brief Test for MPLS PORT P2P APIs.
 * For detailed description of the test, see the manual.
 */
static shr_error_e
dnx_mpls_port_crossconnect_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    bcm_mpls_port_t mpls_port;
    bcm_mpls_port_t mpls_port_buffer;
    bcm_vpn_t vpn;
    bcm_gport_t mpls_port_id = BCM_GPORT_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI(("\n*\n* MPLS PORT Cross-Connect Test\n*\n"));

    LOG_CLI(("1. Try to create P2P MPLS Port with Illegal params - expect error\n"));
    LOG_CLI(("1.1 Missing CROSS_CONNECT flag\n"));
    vpn = 0;
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = 1000;
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
    mpls_port.port = 200;
    SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_port_add(unit, vpn, &mpls_port));

    LOG_CLI(("1.2 vpn > 0\n"));
    
    vpn = 10;
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = 1000;
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_CROSS_CONNECT;
    mpls_port.port = 200;
    SHR_ASSERT_EQ(BCM_E_UNAVAIL, bcm_mpls_port_add(unit, vpn, &mpls_port));

    LOG_CLI(("1.3 Egress mpls port with CROSS_CONNECT flag\n"));
    vpn = 0;
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.egress_label.label = 2000;
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY | BCM_MPLS_PORT2_CROSS_CONNECT;
    SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_port_add(unit, vpn, &mpls_port));

    LOG_CLI(("2. Create MPLS Port with vpn=0 and CROSS_CONNECT flag\n"));
    vpn = 0;
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = 1000;
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_CROSS_CONNECT;
    mpls_port.port = 200;
    SHR_IF_ERR_EXIT(bcm_mpls_port_add(unit, vpn, &mpls_port));
    mpls_port_id = mpls_port.mpls_port_id;

    LOG_CLI(("3. Get created MPLS Port and compare fields\n"));
    vpn = 0;
    bcm_mpls_port_t_init(&mpls_port_buffer);
    mpls_port_buffer.mpls_port_id = mpls_port_id;
    SHR_IF_ERR_EXIT(bcm_mpls_port_get(unit, vpn, &mpls_port_buffer));
    SHR_ASSERT_EQ(mpls_port.criteria, mpls_port_buffer.criteria);
    SHR_ASSERT_EQ(mpls_port.match_label, mpls_port_buffer.match_label);
    SHR_ASSERT_EQ(mpls_port.flags, mpls_port_buffer.flags);
    SHR_ASSERT_EQ(mpls_port.flags2, mpls_port_buffer.flags2);

    LOG_CLI(("4. Clean up\n"));
    vpn = 0;
    SHR_IF_ERR_EXIT(bcm_mpls_port_delete(unit, vpn, mpls_port_id));
    mpls_port_id = BCM_GPORT_INVALID;

    LOG_CLI(("\n\n\n >>> Test PASSED <<<\n\n\n"));

exit:
    if (mpls_port_id != BCM_GPORT_INVALID)
    {
        bcm_mpls_port_delete(unit, vpn, mpls_port_id);
    }
    if (SHR_FUNC_ERR())
    {
        LOG_CLI((BSL_META("\n\n <<<  Test FAIL  >>>\n\n\n")));
    }
    SHR_FUNC_EXIT;
}

/** Test manual   */
sh_sand_man_t dnx_mpls_port_crossconnect_test_man = {
    .brief = "Semantic test of MPLS PORT P2P APIs",
    .full = "Semantic test of MPLS PORT P2P APIs:\n"
        "Create/Get/Delete ingress mpls ports with and without the CROSS_CONNECT flag",
    .synopsis = "ctest mpls port crossconnect",
    .examples = "",
};

sh_sand_cmd_t dnx_mpls_port_tests[] = {
    {
     .keyword = "traverse",
     .action = dnx_mpls_port_traverse_test,
     .options = NULL,
     .man = &dnx_mpls_port_traverse_test_man,
     .flags = CTEST_PRECOMMIT,
     }
    ,
    {
     .keyword = "crossconnect",
     .action = dnx_mpls_port_crossconnect_test,
     .options = NULL,
     .man = &dnx_mpls_port_crossconnect_test_man,
     .flags = CTEST_PRECOMMIT,
     }
    ,
    {NULL}
};
