/** \file diag_dnx_in_lif_profile.c
 *
 * in_lif_profile unit test.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* INCLUDE FILES:
* {
*/
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>

#include <bcm/types.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_in_lif_profile.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_out_lif_profile.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_lif_profile_access.h>

#include <appl/diag/sand/diag_sand_framework.h>

/**
 * }
 */

/*
 * DEFINES
 */
/*
 * number of in_lif_profiles is dnx_data_lif.in_lif.nof_in_lif_profiles_get(unit) / 2 because we use only 7 bits out of 8
 */
#define NOF_OUT_LIF_PROFILES 55
#define OUT_LIF_ORIENTATION_MAX_VALUE 3
#define DEFAULT_OUT_LIF_ORIENTATION_VALUE 0
/*
 * meanwhile there are 14 possible cs_in_lif_profile values:
 * PWE: 3 possible nof tags x is_lif_scoped = 6
 * GRE8: use_tni x use_global_route = 4
 * OTHER: intf_namespace x use_global_route = 4
 * total 14
 */
#define NOF_CS_IN_LIF_PROFILES 14
#define NOF_POSSIBLE_PWE_NOF_ETH_TAGS 3
#define MAX_NOF_ROUTING_ENABLERS_PROFILE 31
#define NOF_POSSIBLE_PWE_MODES 2
#define NOF_LIF_PROFILES_TO_ALLOC_PHASE_2 2
#define TOTAL_NOF_PROFILES (NOF_ETH_RIF_PROFILES + NOF_IN_LIF_PROFILES)
/*
 * Specify used to iterate over ingress enablers flags
 */
#define FLAGS_SHIFT_LEFT_NOF_BITS 5
/*
 * structs to support bcm shell command
 */
/*
 * {
 */

sh_sand_man_t dnx_in_lif_profile_test_man = {
    "Unit Test for in_lif_profile",
    "Unit Test for in_lif_profile, tests in_lif_profile module"
};

sh_sand_man_t dnx_out_lif_profile_test_man = {
    "Unit Test for out_lif_profile",
    "Unit Test for out_lif_profile, tests out_lif_profile module"
};

static shr_error_e
dnx_cs_in_lif_profile_unit_test(
    int unit)
{
    dbal_tables_e inlif_dbal_table_id[NOF_CS_IN_LIF_PROFILES];
    in_lif_profile_info_t in_lif_profile_info_array[NOF_CS_IN_LIF_PROFILES];
/**    in_lif_template_data_t in_lif_profile_template_data;*/
    int /**ref_count, in_lif_profile_counter,*/ use_pwe_lif_scoped, pwe_nof_eth_tags, new_in_lif_profile, array_counter,
        profiles_counter, intf_namespace, use_global_route;
    
    SHR_FUNC_INIT_VARS(unit);

    /*
     * build an array of distinct in_lif_profile_info combinations which reflect distinct possible cs_in_lif_profiles
     * each one of these can be used to create a new different in_lif_profile
     */
    profiles_counter = 0;

    /*
     * create all possible PWE in_lif_profiles
     */
    printf("creating all possible PWE cs_inlif_profiles\n");
    for (pwe_nof_eth_tags = 0; pwe_nof_eth_tags < NOF_POSSIBLE_PWE_NOF_ETH_TAGS; pwe_nof_eth_tags++)
    {
        for (use_pwe_lif_scoped = 0; use_pwe_lif_scoped < NOF_POSSIBLE_PWE_MODES; use_pwe_lif_scoped++)
        {
            in_lif_profile_info_t_init(unit, &in_lif_profile_info_array[profiles_counter]);
            if (use_pwe_lif_scoped)
            {
                in_lif_profile_info_array[profiles_counter].ingress_fields.cs_in_lif_profile_flags |=
                    DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED;
            }

            in_lif_profile_info_array[profiles_counter].ingress_fields.pwe_nof_eth_tags = pwe_nof_eth_tags;

            inlif_dbal_table_id[profiles_counter] = DBAL_TABLE_IN_LIF_FORMAT_PWE;
            profiles_counter++;
        }
    }

    /*
     * create all possible OTHER in_lif_profiles
     */
    printf("creating all possible OTHER cs_inlif_profiles\n");
    for (intf_namespace = 0; intf_namespace <= BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;
         intf_namespace += BCM_L3_INGRESS_MPLS_INTF_NAMESPACE)
    {
        for (use_global_route = 0; use_global_route <= BCM_L3_INGRESS_GLOBAL_ROUTE;
             use_global_route += BCM_L3_INGRESS_GLOBAL_ROUTE)
        {
            in_lif_profile_info_t_init(unit, &in_lif_profile_info_array[profiles_counter]);
            if (use_global_route)
            {
                in_lif_profile_info_array[profiles_counter].ingress_fields.l3_ingress_flags |=
                    BCM_L3_INGRESS_GLOBAL_ROUTE;
            }

            if (intf_namespace)
            {
                in_lif_profile_info_array[profiles_counter].ingress_fields.l3_ingress_flags |=
                    BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;
            }

            inlif_dbal_table_id[profiles_counter] = 0;
            profiles_counter++;
        }
    }

    /*
     * exchange profiles
     */
    printf("exchanging all possible cs_inlif_profiles\n");
    profiles_counter = 0;
    for (array_counter = 0; array_counter < 14; array_counter++)
    {
        if (array_counter >= NOF_IN_LIF_PROFILES - 1)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info_array[array_counter], DNX_IN_LIF_PROFILE_DEFAULT,
                         &new_in_lif_profile, LIF, inlif_dbal_table_id[array_counter]));
        profiles_counter++;
        printf("new_in_lif_profile = %d\n", new_in_lif_profile);
        if (new_in_lif_profile != profiles_counter)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "new_in_lif_profile unexpected value : %d, should be %d\n", new_in_lif_profile,
                         profiles_counter);
        }
    }

    
    printf("finished cs_inlif_profiles unit test\n");
    /*
     * clear template manager references
     */
    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile.clear(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * runs unit test for in_lif_profile module
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] args - added to match with shell cmds structure
 *  \param [in] sand_control - added to match with shell cmds structure
 * \par DIRECT OUTPUT:
 *    \retval Zero if no error was detected
 *    \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT INPUT
 *  \param [in] *args - see args in direct input above
 *  \param [in] *sand_control - see sand_control in direct input above
 * \par INDIRECT OUTPUT
 *   the executed test
 */
shr_error_e
dnx_in_lif_profile_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rc, new_in_lif_profile, flags, profiles_counter, ref_count, array_counter, ii;
    in_lif_profile_info_t in_lif_profile_info;
    in_lif_profile_info_t *in_lif_profile_info_array_p = NULL;

    int change_rpf_mode_flag, use_pwe_lif_scoped, pwe_nof_eth_tags, last_lif_index;
    int lif_profile_to_dealloc;
    dbal_tables_e inlif_dbal_table_id;
    int max_ref_count;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(in_lif_profile_info_array_p, sizeof(*in_lif_profile_info_array_p) * TOTAL_NOF_PROFILES,
              "in_lif_profile_info_array_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /*
     * this LIF in_lif_profile will be used later to deallocate one LIF in_lif_profile
     * the actual value is not important it just should be in the right range
     */
    lif_profile_to_dealloc = NOF_IN_LIF_PROFILES - 1;
    /*
     * not relevant for the unit test but need to pass it to exchange function
     */
    inlif_dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_PWE;

    /*
     * clear the in_lif_profile module templates
     * this step is to avoid failures when running the test twice or after another test that uses the same templates
     */
    SHR_IF_ERR_EXIT(algo_lif_profile_db.eth_rif_profile.clear(unit));
    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile.clear(unit));
    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile_l3_enablers.clear(unit));
    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_da_not_found_destination.clear(unit));

    /*
     * run unit test for cs_in_lif_profile
     */
    SHR_IF_ERR_EXIT(dnx_cs_in_lif_profile_unit_test(unit));

    /*
     * build an array of distinct TOTAL_NOF_PROFILES in_lif_profile_info combinations
     * each one of these can be used to create a new different in_lif_profile/eth_rif_profile
     */
    profiles_counter = 0;
    for (flags = 0; flags < MAX_NOF_ROUTING_ENABLERS_PROFILE; flags++)
    {
        for (pwe_nof_eth_tags = 0; pwe_nof_eth_tags < NOF_POSSIBLE_PWE_NOF_ETH_TAGS; pwe_nof_eth_tags++)
        {
            for (use_pwe_lif_scoped = 0; use_pwe_lif_scoped < NOF_POSSIBLE_PWE_MODES; use_pwe_lif_scoped++)
            {
                for (change_rpf_mode_flag = 0; change_rpf_mode_flag < 2; change_rpf_mode_flag++)
                {
                    if (profiles_counter == TOTAL_NOF_PROFILES)
                    {
                        break;
                    }
                    in_lif_profile_info_t_init(unit, &in_lif_profile_info_array_p[profiles_counter]);
                    in_lif_profile_info_array_p[profiles_counter].ingress_fields.l3_ingress_flags =
                        flags << FLAGS_SHIFT_LEFT_NOF_BITS;
                    if (use_pwe_lif_scoped)
                    {
                        in_lif_profile_info_array_p[profiles_counter].ingress_fields.cs_in_lif_profile_flags |=
                            DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED;
                    }
                    if (change_rpf_mode_flag)
                    {
                        in_lif_profile_info_array_p[profiles_counter].ingress_fields.urpf_mode = bcmL3IngressUrpfLoose;
                    }
                    in_lif_profile_info_array_p[profiles_counter].ingress_fields.pwe_nof_eth_tags = pwe_nof_eth_tags;
                    profiles_counter++;
                }
            }
        }
    }

    /*
     * make sure the default in_lif_profile is created after init and has the right number of references
     */
    printf("Step 1 => making sure the default profile is created after init...\n");
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, DNX_IN_LIF_PROFILE_DEFAULT, &in_lif_profile_info, ETH_RIF));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, DNX_IN_LIF_PROFILE_DEFAULT, &ref_count, ETH_RIF));

    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, &max_ref_count, NULL));
    if (ref_count != max_ref_count)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "the default in_lif_profile has a wrong reference count: %d\n", ref_count);
    }

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, DNX_IN_LIF_PROFILE_DEFAULT, &in_lif_profile_info, LIF));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, DNX_IN_LIF_PROFILE_DEFAULT, &ref_count, LIF));
    if (ref_count != max_ref_count)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "the default in_lif_profile has a wrong reference count: %d\n", ref_count);
    }

    /*
     * allocate NOF_IN_LIF_PROFILES - 3 new LIF in_lif_profiles, should pass
     * note: no reason the number NOF_IN_LIF_PROFILES - 3 was chosen but the goal is not to fill the template manager DNX_ALGO_IN_LIF_PROFILE
     */
    array_counter = 0;
    profiles_counter = 0;
    printf("Step 2 => allocating %d new LIF in_lif_profiles, should pass\n", NOF_IN_LIF_PROFILES - 3);
    while (profiles_counter < NOF_IN_LIF_PROFILES - 3)
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info_array_p[array_counter], DNX_IN_LIF_PROFILE_DEFAULT,
                         &new_in_lif_profile, LIF, inlif_dbal_table_id));
        array_counter++;
        profiles_counter++;
        printf(" => new in_lif_profile: %d\n", new_in_lif_profile);
        if (new_in_lif_profile != profiles_counter)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "new_in_lif_profile unexpected value : %d, should be %d\n", new_in_lif_profile,
                         profiles_counter);
        }
    }
    last_lif_index = new_in_lif_profile;

    /*
     * allocate (NOF_ETH_RIF_PROFILES - 1) new ETH-RIF in_lif_profiles,should pass
     */
    printf("Step 3 => allocating %d new ETH-RIF in_lif_profiles,should pass\n", NOF_ETH_RIF_PROFILES - 1);
    profiles_counter = 0;
    while (profiles_counter < (NOF_ETH_RIF_PROFILES - 1))
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info_array_p[array_counter], DNX_IN_LIF_PROFILE_DEFAULT,
                         &new_in_lif_profile, ETH_RIF, inlif_dbal_table_id));
        printf(" => new in_lif_profile: %d\n", new_in_lif_profile);
        array_counter++;
        profiles_counter++;
        if (new_in_lif_profile != profiles_counter)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "new_in_lif_profile unexpected value : %d, should be %d\n", new_in_lif_profile,
                         profiles_counter);
        }
    }

    /*
     * allocate one more new ETH-RIF in_lif_profile,table full, should fail
     */
    printf("Step 4 => allocating one more new ETH-RIF profile,table full, should fail\n");
    rc = dnx_in_lif_profile_exchange(unit, &in_lif_profile_info_array_p[array_counter], DNX_IN_LIF_PROFILE_DEFAULT,
                                     &new_in_lif_profile, ETH_RIF, inlif_dbal_table_id);
    if (rc == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "table full but dnx_in_lif_profile_exchange passed\n");
    }

    /*
     * fill in_lif_profile table
     */
    printf("Step 5 => allocating %d new LIF in_lif_profiles,should pass\n", NOF_LIF_PROFILES_TO_ALLOC_PHASE_2);
    profiles_counter = 0;
    while (profiles_counter < NOF_LIF_PROFILES_TO_ALLOC_PHASE_2)
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info_array_p[array_counter], DNX_IN_LIF_PROFILE_DEFAULT,
                         &new_in_lif_profile, LIF, inlif_dbal_table_id));
        printf(" => new in_lif_profile: %d\n", new_in_lif_profile);
        if (new_in_lif_profile != (last_lif_index + profiles_counter + 1))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "new_in_lif_profile unexpected value : %d, should be %d\n", new_in_lif_profile,
                         last_lif_index + profiles_counter);
        }
        array_counter++;
        profiles_counter++;
    }

    /*
     * allocate one more LIF in_lif_profile,table full, should fail
     */
    printf("Step 6 => allocating one more new LIF in_lif_profile,table full,should fail\n");
    rc = dnx_in_lif_profile_exchange(unit, &in_lif_profile_info_array_p[array_counter], DNX_IN_LIF_PROFILE_DEFAULT,
                                     &new_in_lif_profile, LIF, inlif_dbal_table_id);
    if (rc == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "table full but dnx_in_lif_profile_exchange passed\n");
    }

    /*
     * deallocate one ETH-RIF in_lif_profile, and then try to allocate a new LIF in_lif_profile, should fail
     */
    printf("Step 7 => deallocate one ETH-RIF in_lif_profile,should pass\n");
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, NOF_ETH_RIF_PROFILES - 1, &new_in_lif_profile, ETH_RIF));
    printf(" => new in_lif_profile: %d\n", new_in_lif_profile);
    if (new_in_lif_profile != DNX_IN_LIF_PROFILE_DEFAULT)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "ETH-RIF deallocate failed\n");
    }
    /*
     * allocate one more LIF in_lif_profile,table not full, should fail
     */
    printf("Step 8 => allocate one more new LIF in_lif_profile,should pass\n");
    rc = dnx_in_lif_profile_exchange(unit, &in_lif_profile_info_array_p[array_counter], DNX_IN_LIF_PROFILE_DEFAULT,
                                     &new_in_lif_profile, LIF, inlif_dbal_table_id);
    if (rc == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "allocate succeeded but it should fail\n");
    }

    /*
     * reallocate the ETH_RIF profile that we dealloced
     */
    printf("Step 9 => reallocate the ETH_RIF profile that we dealloced,should pass\n");
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info_array_p[array_counter], DNX_IN_LIF_PROFILE_DEFAULT, &new_in_lif_profile,
                     ETH_RIF, inlif_dbal_table_id));
    printf(" => new in_lif_profile: %d\n", new_in_lif_profile);
    if (new_in_lif_profile != NOF_ETH_RIF_PROFILES - 1)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "ETH-RIF reallocate failed\n");
    }

    /*
     * deallocate one LIF in_lif_profile, and then try to allocate a new ETH-RIF in_lif_profile, should fail
     */
    printf("Step 10 => deallocate one LIF profile,should pass\n");
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, NOF_IN_LIF_PROFILES - 1, &new_in_lif_profile, LIF));
    printf(" => new in_lif_profile: %d\n", new_in_lif_profile);
    if (new_in_lif_profile != DNX_IN_LIF_PROFILE_DEFAULT)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "LIF deallocate failed\n");
    }
    /*
     * allocate one more ETH_RIF in_lif_profile,table full, should fail
     */
    array_counter++;
    printf("Step 11 => allocating one more new ETH-RIF in_lif_profile,should fail\n");
    rc = dnx_in_lif_profile_exchange(unit, &in_lif_profile_info_array_p[array_counter], DNX_IN_LIF_PROFILE_DEFAULT,
                                     &new_in_lif_profile, ETH_RIF, inlif_dbal_table_id);
    if (rc == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "table full but dnx_in_lif_profile_exchange passed\n");
    }

    /*
     * allocate one more LIF in_lif_profile,table not full, should pass
     */
    printf("Step 12 => allocating one more new LIF profile,table not full,should pass\n");
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info_array_p[array_counter], DNX_IN_LIF_PROFILE_DEFAULT, &new_in_lif_profile,
                     LIF, inlif_dbal_table_id));
    printf(" => new in_lif_profile: %d\n", new_in_lif_profile);
    if (new_in_lif_profile != lif_profile_to_dealloc)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "LIF allocate failed\n");
    }

    /*
     * allocate same LIF in_lif_profile, should pass
     */
    printf("Step 13 => allocating one more new LIF profile,table not full,should pass\n");
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info_array_p[array_counter], DNX_IN_LIF_PROFILE_DEFAULT, &new_in_lif_profile,
                     LIF, inlif_dbal_table_id));
    printf(" => new in_lif_profile: %d\n", new_in_lif_profile);
    if (new_in_lif_profile != lif_profile_to_dealloc)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "LIF allocate failed\n");
    }

    /*
     * check ref_count of already existing in_lif_profiles
     * only in_lif_profile DNX_IN_LIF_PROFILE_DEFAULT,NOF_IN_LIF_PROFILES - 1 should have more than 1 ref_count
     */
    printf("Step 14 => checking ref_count for all in_lif_profiles...\n");
    for (ii = DNX_IN_LIF_PROFILE_DEFAULT; ii < NOF_IN_LIF_PROFILES; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, ii, &in_lif_profile_info, LIF));
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, ii, &ref_count, LIF));
        if (ii != DNX_IN_LIF_PROFILE_DEFAULT)
        {
            if ((ii == NOF_IN_LIF_PROFILES - 1) && (ref_count != 2))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "wrong ref_count for in_lif_profile = %d,ref_count = %d\n", ii, ref_count);
            }
            else if ((ii != NOF_IN_LIF_PROFILES - 1) && (ref_count != 1))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "wrong ref_count for in_lif_profile = %d,ref_count = %d\n", ii, ref_count);
            }
        }
    }

    /*
     * check ref_count of already existing eth_rif_profiles
     * only eth_rif_profile DNX_IN_LIF_PROFILE_DEFAULT should have more than 1 ref_count
     */
    printf("Step 15 => checking ref_count for all eth_rif_profiles...\n");
    for (ii = DNX_IN_LIF_PROFILE_DEFAULT; ii < NOF_ETH_RIF_PROFILES; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, ii, &in_lif_profile_info, ETH_RIF));
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, ii, &ref_count, ETH_RIF));
        if (ii != DNX_IN_LIF_PROFILE_DEFAULT && ref_count != 1)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "wrong ref_count for in_lif_profile = %d,ref_count = %d\n", ii, ref_count);
        }
    }

    printf(" => inlif profile TEST PASSED!\n");

exit:
    SHR_FREE(in_lif_profile_info_array_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * runs unit test for out_lif_profile module
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] args - added to match with shell cmds structure
 *  \param [in] sand_control - added to match with shell cmds structure
 * \par DIRECT OUTPUT:
 *    \retval Zero if no error was detected
 *    \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT INPUT
 *  \param [in] *args - see args in direct input above
 *  \param [in] *sand_control - see sand_control in direct input above
 * \par INDIRECT OUTPUT
 *   the executed test
 */
shr_error_e
dnx_out_lif_profile_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int ref_count, profiles_counter, out_lif_orientation_val, temp_counter, new_out_lif_profile,
        out_lif_profile_to_dealloc;
    etpp_out_lif_profile_info_t out_lif_profile_info_array[NOF_OUT_LIF_PROFILES];
    uint8 is_last = 0;
    SHR_FUNC_INIT_VARS(unit);

    out_lif_profile_to_dealloc = 1;

    SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.clear(unit));

    /*
     * build an array of distinct NOF_OUT_LIF_PROFILES out_lif_profile_info combinations
     * each one of these can be used to create a new different out_lif_profile
     */
    profiles_counter = 0;
    for (out_lif_orientation_val = DEFAULT_OUT_LIF_ORIENTATION_VALUE;
         out_lif_orientation_val <= OUT_LIF_ORIENTATION_MAX_VALUE; out_lif_orientation_val++)
    {
        if (profiles_counter == NOF_OUT_LIF_PROFILES)
        {
            break;
        }
        etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info_array[profiles_counter]);
        out_lif_profile_info_array[profiles_counter].out_lif_orientation = out_lif_orientation_val;

        profiles_counter++;
    }

    /*
     * make sure the default out_lif_profile is created after init and has the right number of references
     */
    printf(" => making sure the default profile is created after init...\n");
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_ref_count(unit, DNX_OUT_LIF_PROFILE_DEFAULT, &ref_count));
    /*
     * The maximum number of references is equal to the number of out_lifs,
     * multiplied by the number of ranges of etpp out_lif smart template manager
     */
    if (ref_count !=
        DNX_ALGO_ETPP_OUT_LIF_PROFILE_SMART_TEMPLATE_NOF_RANGES * dnx_data_lif.out_lif.nof_local_out_lifs_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "The default out_lif_profile has a wrong reference count - expected:%d, actual:%d\n",
                     DNX_ALGO_ETPP_OUT_LIF_PROFILE_SMART_TEMPLATE_NOF_RANGES *
                     dnx_data_lif.out_lif.nof_local_out_lifs_get(unit), ref_count);
    }

    /*
     * allocate all possible distinct out_lif_profiles and check the return out_lif_profile value
     */
    temp_counter = 0;
    printf("allocating all possible out_lif_profiles\n");
    while (temp_counter < profiles_counter)
    {
        SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                        (unit, 0, 0, &out_lif_profile_info_array[temp_counter], DNX_OUT_LIF_PROFILE_DEFAULT,
                         &new_out_lif_profile, &is_last));
        printf(" => new out_lif_profile: %d\n", new_out_lif_profile);
        if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT + temp_counter)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "new_out_lif_profile unexpected value : %d, should be %d\n", new_out_lif_profile,
                         DNX_OUT_LIF_PROFILE_DEFAULT + temp_counter);
        }
        temp_counter++;
    }

    /*
     * deallocate one out_lif_profile
     */
    printf(" => deallocate one out_lif_profile\n");
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, 0, 0, &out_lif_profile_info_array[DNX_OUT_LIF_PROFILE_DEFAULT], out_lif_profile_to_dealloc,
                     &new_out_lif_profile, &is_last));
    printf(" => new out_lif_profile: %d\n", new_out_lif_profile);
    if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "deallocate failed\n");
    }

    /*
     * allocate the out_lif_profile that we deallocated
     */
    printf(" => allocating same out_lif_profile\n");
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, 0, 0, &out_lif_profile_info_array[out_lif_profile_to_dealloc], DNX_OUT_LIF_PROFILE_DEFAULT,
                     &new_out_lif_profile, &is_last));
    printf(" => new out_lif_profile: %d\n", new_out_lif_profile);
    if (new_out_lif_profile != out_lif_profile_to_dealloc)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "deallocate failed\n");
    }

    /*
     * allocate the same out_lif_profile
     */
    printf(" => allocating same out_lif_profile\n");
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, 0, 0, &out_lif_profile_info_array[out_lif_profile_to_dealloc], DNX_OUT_LIF_PROFILE_DEFAULT,
                     &new_out_lif_profile, &is_last));
    printf(" => new out_lif_profile: %d\n", new_out_lif_profile);
    if (new_out_lif_profile != out_lif_profile_to_dealloc)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "deallocate failed\n");
    }

    /*
     * check ref_count of all profiles
     */
    printf("checking ref_count for all profiles\n");
    for (profiles_counter = DNX_OUT_LIF_PROFILE_DEFAULT; profiles_counter < NOF_OUT_LIF_PROFILES; profiles_counter++)
    {
        SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_ref_count(unit, profiles_counter, &ref_count));
        /*
         * The maximum number of references is equal to the number of out_lifs,
         * multiplied by the number of ranges of etpp out_lif smart template manager
         */
        if (profiles_counter == DNX_OUT_LIF_PROFILE_DEFAULT
            && ref_count !=
            DNX_ALGO_ETPP_OUT_LIF_PROFILE_SMART_TEMPLATE_NOF_RANGES *
            dnx_data_lif.out_lif.nof_local_out_lifs_get(unit) - 4)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "wrong ref_count for out_lif_profile = %d,ref_count = %d,should be %d\n",
                         profiles_counter, ref_count,
                         DNX_ALGO_ETPP_OUT_LIF_PROFILE_SMART_TEMPLATE_NOF_RANGES *
                         dnx_data_lif.out_lif.nof_local_out_lifs_get(unit) - 4);
        }
        else if (profiles_counter == 1 && ref_count != 2)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "wrong ref_count for out_lif_profile = %d,ref_count = %d,should be %d\n",
                         profiles_counter, ref_count, 2);
        }
        else if ((profiles_counter == 2 || profiles_counter == 3) && ref_count != 1)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "wrong ref_count for out_lif_profile = %d,ref_count = %d,should be %d\n",
                         profiles_counter, ref_count, 1);
        }
        else if (profiles_counter > 3 && ref_count != 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "wrong ref_count for out_lif_profile = %d,ref_count = %d,should be %d\n",
                         profiles_counter, ref_count, 0);
        }
    }

    /*
     * deallocate all profiles
     */
    printf(" => deallocate all out_lif_profiles\n");
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, 0, 0, &out_lif_profile_info_array[DNX_OUT_LIF_PROFILE_DEFAULT], 1, &new_out_lif_profile,
                     &is_last));
    if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "deallocate failed\n");
    }
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, 0, 0, &out_lif_profile_info_array[DNX_OUT_LIF_PROFILE_DEFAULT], 1, &new_out_lif_profile,
                     &is_last));
    if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "deallocate failed\n");
    }
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, 0, 0, &out_lif_profile_info_array[DNX_OUT_LIF_PROFILE_DEFAULT], 2, &new_out_lif_profile,
                     &is_last));
    if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "deallocate failed\n");
    }
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, 0, 0, &out_lif_profile_info_array[DNX_OUT_LIF_PROFILE_DEFAULT], 3, &new_out_lif_profile,
                     &is_last));
    if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "deallocate failed\n");
    }

    /*
     * check ref_count of all profiles after dealloc
     */
    printf("checking ref_count for all profiles\n");
    for (profiles_counter = DNX_OUT_LIF_PROFILE_DEFAULT; profiles_counter < NOF_OUT_LIF_PROFILES; profiles_counter++)
    {
        SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_ref_count(unit, profiles_counter, &ref_count));
        /*
         * The maximum number of references is equal to the number of out_lifs,
         * multiplied by the number of ranges of etpp out_lif smart template manager
         */
        if (profiles_counter == DNX_OUT_LIF_PROFILE_DEFAULT
            && ref_count !=
            DNX_ALGO_ETPP_OUT_LIF_PROFILE_SMART_TEMPLATE_NOF_RANGES * dnx_data_lif.out_lif.nof_local_out_lifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "wrong ref_count for out_lif_profile = %d,ref_count = %d,should be %d\n",
                         profiles_counter, ref_count,
                         DNX_ALGO_ETPP_OUT_LIF_PROFILE_SMART_TEMPLATE_NOF_RANGES *
                         dnx_data_lif.out_lif.nof_local_out_lifs_get(unit));
        }
        else if (profiles_counter > DNX_OUT_LIF_PROFILE_DEFAULT && ref_count != 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "wrong ref_count for out_lif_profile = %d,ref_count = %d,should be %d\n",
                         profiles_counter, ref_count, 0);
        }
    }

    printf(" => outlif profile TEST PASSED!\n");

exit:
    SHR_FUNC_EXIT;
}
