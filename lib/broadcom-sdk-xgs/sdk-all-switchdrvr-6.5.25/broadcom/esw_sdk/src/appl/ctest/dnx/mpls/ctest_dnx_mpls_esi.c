/** \file ctest_dnx_mpls_esi.c
 * 
 * Tests for MPLS based EVPN - ESI DB APIs
 * 
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <bcm/mpls.h>
#include <bcm/vlan.h>
#include <bcm/trunk.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>

#define NOF_ESI_STRUCTS         2

typedef struct
{
    int call_count;
    int encountered[NOF_ESI_STRUCTS];
    bcm_mpls_esi_info_t *esi_info[NOF_ESI_STRUCTS];
} mpls_esi_test_data_t;

/**
 * \brief ESI DB traversal tester: prints the ESI DB information
 * and counts the entries.
 * \param [in] unit
 * \param [in] esi_info - an encountered entry's information
 * \param [out] user_data - Assumes to point to a mpls_esi_test_data_t
 */
static shr_error_e
ctest_dnx_mpls_esi_cb(
    int unit,
    bcm_mpls_esi_info_t * esi_info,
    void *user_data)
{
    mpls_esi_test_data_t *test_data = user_data;
    int idx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(user_data, _SHR_E_PARAM, "user_data");

    test_data->call_count++;

    LOG_CLI((BSL_META("(%d)ESI Entry - KEY: {src_port=0x%08x, out_class_id=0x%08x} => ESI Label: %d (0x%05x)\n"),
             test_data->call_count, esi_info->src_port, esi_info->out_class_id, esi_info->esi_label,
             esi_info->esi_label));

    for (idx = 0; idx < NOF_ESI_STRUCTS; idx++)
    {
        bcm_mpls_esi_info_t *cur_esi_info = test_data->esi_info[idx];
        if ((esi_info->esi_label == cur_esi_info->esi_label) &&
            (esi_info->flags == cur_esi_info->flags) && (esi_info->out_class_id == cur_esi_info->out_class_id))
        {
            if (!BCM_GPORT_IS_SET(cur_esi_info->src_port))
            {
                /*
                 * ESI set with local port. Traverse returns system port gport.
                 * Compare the value only
                 */
                if (cur_esi_info->src_port == BCM_GPORT_SYSTEM_PORT_ID_GET(esi_info->src_port))
                {
                    test_data->encountered[idx]++;
                }
            }
            else if (BCM_GPORT_IS_LOCAL(cur_esi_info->src_port))
            {
                /*
                 * ESI set with local port gport. Traverse returns system port gport.
                 */
                if (BCM_GPORT_LOCAL_GET(cur_esi_info->src_port) == BCM_GPORT_SYSTEM_PORT_ID_GET(esi_info->src_port))
                {
                    test_data->encountered[idx]++;
                }
            }
            else
            {
                /*
                 * ESI set with system port or trunk gport.
                 * Compare as is.
                 */
                if (cur_esi_info->src_port == esi_info->src_port)
                {
                    test_data->encountered[idx]++;
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Tests creation of ESI entry in the ESI DB.
 * See man page for details.
 */
static shr_error_e
dnx_mpls_esi_basic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_mpls_esi_info_t esi_info_1;
    bcm_mpls_esi_info_t esi_info_2;
    bcm_mpls_esi_info_t esi_info_buffer;
    mpls_esi_test_data_t test_data;
    int do_cleanup;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("clean", do_cleanup);

    LOG_CLI((BSL_META("Add ESI DB entry\n")));
    bcm_mpls_esi_info_t_init(&esi_info_1);
    esi_info_1.esi_label = 321;
    BCM_GPORT_SYSTEM_PORT_ID_SET(esi_info_1.src_port, 201);
    esi_info_1.out_class_id = 31;
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_add(unit, &esi_info_1));

    LOG_CLI((BSL_META("Get the ESI entry\n")));
    bcm_mpls_esi_info_t_init(&esi_info_buffer);
    esi_info_buffer.src_port = esi_info_1.src_port;
    esi_info_buffer.out_class_id = esi_info_1.out_class_id;
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_get(unit, &esi_info_buffer));
    SHR_ASSERT_EQ(esi_info_1.src_port, esi_info_buffer.src_port);
    SHR_ASSERT_EQ(esi_info_1.out_class_id, esi_info_buffer.out_class_id);
    SHR_ASSERT_EQ(esi_info_1.esi_label, esi_info_buffer.esi_label);
    SHR_ASSERT_EQ(esi_info_1.flags, esi_info_buffer.flags);

    LOG_CLI((BSL_META("Delete the ESI entry\n")));
    bcm_mpls_esi_info_t_init(&esi_info_buffer);
    esi_info_buffer.src_port = esi_info_1.src_port;
    esi_info_buffer.out_class_id = esi_info_1.out_class_id;
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_delete(unit, &esi_info_buffer));

    LOG_CLI((BSL_META("Check that the packet was deleted (error expected)\n")));
    SHR_ASSERT_EQ(_SHR_E_NOT_FOUND, bcm_mpls_esi_encap_get(unit, &esi_info_buffer));

    LOG_CLI((BSL_META("Re-Add the entry\n")));
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_add(unit, &esi_info_1));

    LOG_CLI((BSL_META("Re-Add again (error expected)\n")));
    SHR_ASSERT_EQ(_SHR_E_EXISTS, bcm_mpls_esi_encap_add(unit, &esi_info_1));

    LOG_CLI((BSL_META("Add another ESI DB entry\n")));
    bcm_mpls_esi_info_t_init(&esi_info_2);
    esi_info_2.esi_label = 322;
    esi_info_2.src_port = 200;
    esi_info_2.out_class_id = dnx_data_l2.vlan_domain.nof_vlan_domains_get(unit);
    LOG_CLI((BSL_META("- Out of range class_id (error expected)\n")));
    SHR_ASSERT_EQ(_SHR_E_PARAM, bcm_mpls_esi_encap_add(unit, &esi_info_2));
    LOG_CLI((BSL_META("- Then with out of range label (error expected)\n")));
    esi_info_2.out_class_id = 33;
    esi_info_2.esi_label = 0xfffff + 1;
    SHR_ASSERT_EQ(_SHR_E_PARAM, bcm_mpls_esi_encap_add(unit, &esi_info_2));
    esi_info_2.esi_label = 1;
    SHR_ASSERT_EQ(_SHR_E_PARAM, bcm_mpls_esi_encap_add(unit, &esi_info_2));
    LOG_CLI((BSL_META("- Finally all ok\n")));
    esi_info_2.esi_label = 322;
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_add(unit, &esi_info_2));

    LOG_CLI((BSL_META("Traverse ESI DB - expect 2 calls to the callback function\n")));
    sal_memset(&test_data, 0, sizeof(mpls_esi_test_data_t));
    test_data.esi_info[0] = &esi_info_1;
    test_data.esi_info[1] = &esi_info_2;
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_traverse(unit, ctest_dnx_mpls_esi_cb, &test_data));
    SHR_ASSERT_EQ(NOF_ESI_STRUCTS, test_data.call_count);
    while (test_data.call_count--)
    {
        /*
         * Verify that each esi structure was encountered exactly once.
         */
        SHR_ASSERT_EQ(1, test_data.encountered[test_data.call_count]);
    }

    LOG_CLI((BSL_META("Try to add ESI with invalid src_port (error expected)")));
    bcm_mpls_esi_info_t_init(&esi_info_buffer);
    esi_info_buffer.out_class_id = 121;
    esi_info_buffer.esi_label = 212;
    esi_info_buffer.src_port = BCM_GPORT_INVALID;
    SHR_ASSERT_EQ(_SHR_E_PARAM, bcm_mpls_esi_encap_add(unit, &esi_info_buffer));

    if (do_cleanup)
    {
        LOG_CLI((BSL_META("Clean up\n")));
        SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_delete(unit, &esi_info_2));
        SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_delete(unit, &esi_info_1));
    }
    LOG_CLI((BSL_META("\n\n\n>>>  Test PASS  <<<\n\n\n\n")));
exit:
    if (SHR_FUNC_ERR())
    {
        LOG_CLI((BSL_META("\n\n#@!  Test FAIL  !@#\n\n\n")));
    }
    SHR_FUNC_EXIT;
}

/** Test manual   */
sh_sand_man_t dnx_mpls_esi_basic_test_man = {
    .brief = "Semantic test of MPLS ESI related APIs",
    .full = "Add, delete, get, re-add, traverse and verify input validation with proper errors for "
        "ESI DB management APIs.",
    .synopsis = "ctest mpls esi encap [clean=false]"
};

sh_sand_option_t dnx_mpls_esi_basic_test_options[] = {
    {
     .keyword = "clean",
     .type = SAL_FIELD_TYPE_BOOL,
     .def = "True",
     .desc = "Perform cleanup at the end of the test (Set false for debugging)",
     }
    ,
    {NULL}
};

/**
 * \brief Test creation and management of ESI encapsulation DB entries when the src_port
 * field supplied is a LAG (GPORT_TRUNK)
 */
static shr_error_e
dnx_mpls_esi_trunk_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_mpls_esi_info_t esi_info_1;
    bcm_mpls_esi_info_t esi_info_2;
    bcm_mpls_esi_info_t esi_info_buffer;

    bcm_trunk_t t1_id;
    bcm_gport_t t1_gport;

    bcm_trunk_t t2_id;
    bcm_gport_t t2_gport;

    mpls_esi_test_data_t test_data;
    int do_cleanup;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("clean", do_cleanup);

    LOG_CLI((BSL_META("Create Trunk 1 id (T1)\n")));
    BCM_TRUNK_ID_SET(t1_id, 0, 10);
    SHR_IF_ERR_EXIT(bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &t1_id));

    BCM_GPORT_TRUNK_SET(t1_gport, t1_id);

    LOG_CLI((BSL_META("Create Trunk 2 id (T2)\n")));
    BCM_TRUNK_ID_SET(t2_id, 0, 20);
    SHR_IF_ERR_EXIT(bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &t2_id));

    BCM_GPORT_TRUNK_SET(t2_gport, t2_id);

    LOG_CLI((BSL_META("Add ESI DB entry\n")));
    bcm_mpls_esi_info_t_init(&esi_info_1);
    esi_info_1.esi_label = 321;
    esi_info_1.src_port = t1_gport;
    esi_info_1.out_class_id = 31;
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_add(unit, &esi_info_1));

    LOG_CLI((BSL_META("Get the ESI entry\n")));
    bcm_mpls_esi_info_t_init(&esi_info_buffer);
    esi_info_buffer.src_port = esi_info_1.src_port;
    esi_info_buffer.out_class_id = esi_info_1.out_class_id;
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_get(unit, &esi_info_buffer));
    SHR_ASSERT_EQ(esi_info_1.src_port, esi_info_buffer.src_port);
    SHR_ASSERT_EQ(esi_info_1.out_class_id, esi_info_buffer.out_class_id);
    SHR_ASSERT_EQ(esi_info_1.esi_label, esi_info_buffer.esi_label);
    SHR_ASSERT_EQ(esi_info_1.flags, esi_info_buffer.flags);

    LOG_CLI((BSL_META("Delete the ESI entry\n")));
    bcm_mpls_esi_info_t_init(&esi_info_buffer);
    esi_info_buffer.src_port = esi_info_1.src_port;
    esi_info_buffer.out_class_id = esi_info_1.out_class_id;
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_delete(unit, &esi_info_buffer));

    LOG_CLI((BSL_META("Check that the packet was deleted (error expected)\n")));
    SHR_ASSERT_EQ(_SHR_E_NOT_FOUND, bcm_mpls_esi_encap_get(unit, &esi_info_buffer));

    LOG_CLI((BSL_META("Re-Add the entry\n")));
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_add(unit, &esi_info_1));

    LOG_CLI((BSL_META("Re-Add again (error expected)\n")));
    SHR_ASSERT_EQ(_SHR_E_EXISTS, bcm_mpls_esi_encap_add(unit, &esi_info_1));

    LOG_CLI((BSL_META("Add another ESI DB entry\n")));
    bcm_mpls_esi_info_t_init(&esi_info_2);
    esi_info_2.esi_label = 322;
    esi_info_2.src_port = t2_gport;
    esi_info_2.out_class_id = dnx_data_l2.vlan_domain.nof_vlan_domains_get(unit);
    LOG_CLI((BSL_META("- Out of range class_id (error expected)\n")));
    SHR_ASSERT_EQ(_SHR_E_PARAM, bcm_mpls_esi_encap_add(unit, &esi_info_2));
    LOG_CLI((BSL_META("- Then with out of range label (error expected)\n")));
    esi_info_2.out_class_id = 33;
    esi_info_2.esi_label = 0xfffff + 1;
    SHR_ASSERT_EQ(_SHR_E_PARAM, bcm_mpls_esi_encap_add(unit, &esi_info_2));
    esi_info_2.esi_label = 1;
    SHR_ASSERT_EQ(_SHR_E_PARAM, bcm_mpls_esi_encap_add(unit, &esi_info_2));
    LOG_CLI((BSL_META("- Finally all ok\n")));
    esi_info_2.esi_label = 322;
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_add(unit, &esi_info_2));

    LOG_CLI((BSL_META("Traverse ESI DB - expect 2 calls to the callback function\n")));
    sal_memset(&test_data, 0, sizeof(mpls_esi_test_data_t));
    test_data.esi_info[0] = &esi_info_1;
    test_data.esi_info[1] = &esi_info_2;
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_traverse(unit, ctest_dnx_mpls_esi_cb, &test_data));
    SHR_ASSERT_EQ(NOF_ESI_STRUCTS, test_data.call_count);
    while (test_data.call_count--)
    {
        /*
         * Verify that each esi structure was encountered exactly once.
         */
        SHR_ASSERT_EQ(1, test_data.encountered[test_data.call_count]);
    }

    if (do_cleanup)
    {
        LOG_CLI((BSL_META("Clean up\n")));
        SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_delete(unit, &esi_info_2));
        SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_delete(unit, &esi_info_1));
        SHR_IF_ERR_EXIT(bcm_trunk_destroy(unit, t2_id));
        SHR_IF_ERR_EXIT(bcm_trunk_destroy(unit, t1_id));
    }

    LOG_CLI((BSL_META("\n\n\n>>>  Test PASS  <<<\n\n\n\n")));
exit:
    if (SHR_FUNC_ERR())
    {
        LOG_CLI((BSL_META("\n\n#@!  Test FAIL  !@#\n\n\n")));
    }
    SHR_FUNC_EXIT;
}

/** Test manual   */
sh_sand_man_t dnx_mpls_esi_trunk_test_man = {
    .brief = "Semantic test of MPLS ESI related APIs with GPORT_TRUNK as src_port",
    .full = "Add, delete, get, re-add, traverse and verify input validation with proper errors for "
        "ESI DB management APIs, where the src_port field is GPORT_TRUNK (LAG).",
    .synopsis = "ctest mpls esi trunk [clean=false]"
};

sh_sand_option_t dnx_mpls_esi_trunk_options[] = {
    {
     .keyword = "clean",
     .type = SAL_FIELD_TYPE_BOOL,
     .def = "True",
     .desc = "Perform cleanup at the end of the test (Set false for debugging)",
     }
    ,
    {NULL}
};

/**
 * \brief Deletes all ESI DB entries. user_data is expected to point to an integer counter
 * which will be increased with each call.
 */
static shr_error_e
ctest_dnx_mpls_esi_cleanup_cb(
    int unit,
    bcm_mpls_esi_info_t * esi_info,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    (*((int *) user_data))++;
    LOG_CLI((BSL_META("DELETE ESI Entry - KEY: {src_port=0x%08x, out_class_id=0x%08x} => ESI Label: %d (0x%05x)\n"),
             esi_info->src_port, esi_info->out_class_id, esi_info->esi_label, esi_info->esi_label));
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_delete(unit, esi_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Test deleting esi entries through traverse API.
 */
static shr_error_e
dnx_mpls_esi_clean_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_mpls_esi_info_t esi_info_1;
    bcm_mpls_esi_info_t esi_info_2;

    bcm_trunk_t t1_id;
    bcm_gport_t t1_gport;

    int nof_deleted = 0;
    int create = 0;
    int min_deleted = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("create", create);

    if (create)
    {
        LOG_CLI((BSL_META("Create Trunk 1 id (T1)\n")));
        BCM_TRUNK_ID_SET(t1_id, 0, 77);
        SHR_IF_ERR_EXIT(bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &t1_id));

        BCM_GPORT_TRUNK_SET(t1_gport, t1_id);

        LOG_CLI((BSL_META("Add ESI DB entry\n")));
        bcm_mpls_esi_info_t_init(&esi_info_1);
        esi_info_1.esi_label = 212;
        esi_info_1.src_port = t1_gport;
        esi_info_1.out_class_id = 41;
        SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_add(unit, &esi_info_1));

        LOG_CLI((BSL_META("Add another ESI DB entry\n")));
        bcm_mpls_esi_info_t_init(&esi_info_2);
        esi_info_2.esi_label = 121;
        esi_info_2.src_port = 200;
        esi_info_2.out_class_id = 45;
        SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_add(unit, &esi_info_2));

        min_deleted += 2;
    }

    LOG_CLI((BSL_META("Traverse ESI DB - expect at least %d calls to the callback function\n"), min_deleted));
    SHR_IF_ERR_EXIT(bcm_mpls_esi_encap_traverse(unit, ctest_dnx_mpls_esi_cleanup_cb, &nof_deleted));
    if (min_deleted > nof_deleted)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected at least %d entries to be deleted, but deleted only %d.",
                     min_deleted, nof_deleted);
    }

    if (create)
    {
        SHR_IF_ERR_EXIT(bcm_trunk_destroy(unit, t1_id));
    }

    LOG_CLI((BSL_META("\n\n\n>>>  Test PASS  <<<\n\n\n\n")));
exit:
    if (SHR_FUNC_ERR())
    {
        LOG_CLI((BSL_META("\n\n#@!  Test FAIL  !@#\n\n\n")));
    }
    SHR_FUNC_EXIT;
}

/** Test manual   */
sh_sand_man_t dnx_mpls_esi_clean_test_man = {
    .brief = "Semantic test of MPLS ESI traverse for deleting all the DB entries",
    .full = "Create 2 entries (unless 'create=False' is supplied) and traverse the DB deleting every"
        " encountered entry. The number of deleted entries is verified to be at least 2, if the"
        " entries create by the test was enabled ('create=True' or left to default).",
    .synopsis = "ctest mpls esi clean [create=<True/False>]",
    .examples = "create=False\n",
};

sh_sand_option_t dnx_mpls_esi_clean_options[] = {
    {
     .keyword = "create",
     .type = SAL_FIELD_TYPE_BOOL,
     .def = "True",
     .desc = "Create 2 entries before traversing the DB and deleting everything. "
     "Can be set to False, to use the command for cleanning only",
     }
    ,
    {NULL}
};

sh_sand_cmd_t dnx_mpls_esi_tests[] = {
    {
     .keyword = "encap",
     .action = dnx_mpls_esi_basic_test,
     .options = dnx_mpls_esi_basic_test_options,
     .man = &dnx_mpls_esi_basic_test_man,
     .flags = CTEST_PRECOMMIT}
    ,
    {
     .keyword = "trunk",
     .action = dnx_mpls_esi_trunk_test,
     .options = dnx_mpls_esi_trunk_options,
     .man = &dnx_mpls_esi_trunk_test_man,
     .flags = CTEST_PRECOMMIT}
    ,
    {
     .keyword = "clean",
     .action = dnx_mpls_esi_clean_test,
     .options = dnx_mpls_esi_clean_options,
     .man = &dnx_mpls_esi_clean_test_man,
     .flags = CTEST_PRECOMMIT}
    ,
    {NULL}
};
