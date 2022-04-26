/** \file ctest_dnx_mpls_evpn.c
 *
 * Tests for MPLS based EVPN
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLSTESTSDNX

#include <sal/core/libc.h>
#include <bcm/mpls.h>
#include <bcm/failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <bcm/vlan.h>
#include <bcm/vswitch.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/mpls/mpls_range.h>

#define CTEST_EVPN__IML_LABEL_RANGE_OFFSET      500

/*
 * \brief
 *   This is the callback procedure for bcm_mpls_tunnel_switch_traverse() which is
 *   invoke in 'dnx_mpls_evpn_term_test()' below.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] info -
 *   Structure of type bcm_mpls_tunnel_switch_t. Contains info identifying the
 *   entry for which this callback has been called. See bcm_dnx_mpls_tunnel_switch_traverse()
 *   Note that info->flags is as per BCM_MPLS_SWITCH_*
 * \param [in] user_data -
 *   User data that has been set by the caller of bcm_mpls_tunnel_switch_traverse()
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   bcm_dnx_mpls_tunnel_switch_traverse()
 *   BCM_MPLS_SWITCH_*
 */
static int
tunnel_switch_traverse_cb(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    void *user_data)
{
    /*
     * BCM_MPLS_SWITCH_xxx.
     */
    uint32 flags;
    uint32 user_flags;

    SHR_FUNC_INIT_VARS(unit);

    flags = info->flags;
    /*
     * If 'flags' has BCM_MPLS_SWITCH_NEXT_HEADER_L2 set, this is EVPN.
     */
    if (flags & BCM_MPLS_SWITCH_NEXT_HEADER_L2)
    {
        LOG_CLI_EX("tunnel_switch_traverse_cb: This is EVPN.  %s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    /*
     * If 'flags' has BCM_MPLS_SWITCH_EVPN_IML set, this is IML.
     */
    if (flags & BCM_MPLS_SWITCH_EVPN_IML)
    {
        LOG_CLI_EX("tunnel_switch_traverse_cb: This is IML.  %s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    /*
     * The rest are MPLS.
     */
    if ((flags & (BCM_MPLS_SWITCH_EVPN_IML | BCM_MPLS_SWITCH_NEXT_HEADER_L2)) == 0)
    {
        LOG_CLI_EX("tunnel_switch_traverse_cb: This is MPLS.  %s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    /*
     * Check that the test flags are set in the retrieved info->flags.
     */
    user_flags = *((uint32 *) user_data);
    SHR_ASSERT_EQ(user_flags, user_flags & flags);
exit:
    if (SHR_GET_CURRENT_ERR() == BCM_E_NONE)
    {
        LOG_CLI_EX("tunnel_switch_traverse_cb was completed SUCCESSFULLY.  %s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}
/*
 * Data to pass on, via bcm_mpls_tunnel_initiator_traverse(), to 'tarverse callback':
 */
static unsigned long User_data_initiator[4] = { 0x0FF, 0x0AA, 0x0A5, 0x0BB };
/*
 * Two (hardcoded) labels are used on the  'dnx_mpls_evpn_encap_test()' below
 */
static bcm_mpls_label_t User_labels[2] = { 111, 222 };
/*
 * \brief
 *   This is the callback procedure for bcm_mpls_tunnel_initiator_traverse() which is
 *   invoke in 'dnx_mpls_evpn_encap_test()' below.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] num_labels -
 *   variable of type int. Indicates the number of MPLS labels in 'label_array' below.
 * \param [in] label_array -
 *   Array of structures of type bcm_mpls_egress_label_t. Contains all MPLS labels
 *   corresponding to local_outlif on HW table. Ny=umber of elements is as per 'num_labels'
 *   above.
 * \param [in] user_data -
 *   User data that has been set by the caller of bcm_mpls_tunnel_switch_traverse()
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   bcm_mpls_tunnel_initiator_traverse()
 *   bcm_dnx_mpls_tunnel_initiator_traverse()
 *   bcm_mpls_tunnel_initiator_traverse_cb
 *   BCM_MPLS_SWITCH_*
 */
static int
tunnel_initiator_traverse_cb(
    int unit,
    int num_labels,
    bcm_mpls_egress_label_t * label_array,
    void *user_data)
{
    unsigned long *long_user_data;
    uint32 caller_flags;

    SHR_FUNC_INIT_VARS(unit);

    long_user_data = (unsigned long *) user_data;
    caller_flags = long_user_data[(sizeof(User_data_initiator) / sizeof(User_data_initiator[0])) - 1];
#if (0)
/* { */
    LOG_CLI_EX
        ("tunnel_initiator_traverse_cb: num_labels %d, label_array[0].label %d, label_array[0].flags 0x%08X. Caller flags 0x%08lX\n",
         num_labels, label_array[0].label, label_array[0].flags,
         long_user_data[(sizeof(User_data_initiator) / sizeof(User_data_initiator[0])) - 1]);
/* } */
#endif
    if (caller_flags & BCM_MPLS_EGRESS_LABEL_IML)
    {
        LOG_CLI_EX("tunnel_initiator_traverse_cb: Traversing %s %s result type %s %s\n",
                   "DBAL_TABLE_EEDB_EVPN", "including only", "DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML", EMPTY);
    }
    else if (caller_flags & BCM_MPLS_EGRESS_LABEL_EVPN)
    {
        LOG_CLI_EX("tunnel_initiator_traverse_cb: Traversing %s %s result type %s %s\n",
                   "DBAL_TABLE_EEDB_EVPN", "excluding", "DBAL_RESULT_TYPE_EEDB_EVPN_ETPS_MPLS_1_IML", EMPTY);
    }
    else
    {
        LOG_CLI_EX("tunnel_initiator_traverse_cb: Traversing %s %s result type %s %s\n",
                   "all entries on DBAL_TABLE_EEDB_MPLS_TUNNEL", "without", "", EMPTY);
    }
    if (num_labels != 1)
    {
        /*
         * We expect one label as per our tests below.
         */
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "\r\n" "Wrong 'num_labels':\r\n" "==> Received %d\r\n" "==> Expected %d\r\n", num_labels, 1);
    }
    if ((label_array[0].label != User_labels[0]) && (label_array[0].label != User_labels[1]))
    {
        /*
         * We expect labels as per User_labels[] above.
         */
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "\r\n"
                     "Wrong MPLS label:\r\n"
                     "==> Received %d\r\n"
                     "==> Expected either %d or %d\r\n",
                     (int) (label_array[0].label), (int) User_labels[0], (int) User_labels[1]);
    }
    if (sal_strncmp((char *) long_user_data, (char *) User_data_initiator, sizeof(User_data_initiator)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "\r\n"
                     "User data passed on to this callback is not as loaded.\r\n"
                     "==> Received (0x%08lX,0x%08lX,0x%08lX,0x%08lX)\r\n"
                     "==> Expected (0x%08lX,0x%08lX,0x%08lX,0x%08lX)\r\n",
                     long_user_data[0], long_user_data[1], long_user_data[2], long_user_data[3],
                     User_data_initiator[0], User_data_initiator[1], User_data_initiator[2], User_data_initiator[3]);
    }
exit:
    if (SHR_GET_CURRENT_ERR() == BCM_E_NONE)
    {
        LOG_CLI_EX("tunnel_initiator_traverse_cb was completed SUCCESSFULLY.  %s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

/*
 * 'range tests:
 *   bcm_dnx_mpls_range_action_add()
 *   bcm_dnx_mpls_range_action_remove()
 *   bcm_dnx_mpls_range_action_get()
 */
/* { */
/**
 * \brief
 *   Test EVPN/IML range operations.
 *   For detailed description of the test, see the manual, below ('dnx_mpls_evpn_range_man').
 */
static shr_error_e
dnx_mpls_evpn_range_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /*
     * This variables stores the severity of LIF dnx
     */
    bsl_severity_t original_severity_bcmdnx_lif = 0;
    /*
     * This variables stores the severity of mpls dnx
     */
    bsl_severity_t original_severity_bcmdnx_mpls = 0;
    /*
     * This variables stores the severity of resmngr dnx
     */
    bsl_severity_t original_severity_bcmdnx_resmngr = 0;
    /*
     * This variables stores the severity of swstatednx general
     */
    bsl_severity_t original_severity_swstatednx_general = 0;
    int original_severity_was_initialized = FALSE;
    int detailed;
    /*
     * 'first_label' is the base label for constructing the ranges.
     * The first range is 'first_label', 'first_label + num_labels_in_range - 1'
     * The second labels is 'first_label + num_labels_in_range', 'first_label + 2*num_labels_in_range - 1'
     * etc.
     */
    bcm_mpls_label_t first_label, current_label;
    unsigned int num_labels_in_range;
    unsigned int num_ranges, range_index, index;
    bcm_mpls_label_t label_low, label_high;
    bcm_mpls_range_action_t action;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_BOOL("detailed", detailed);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_bcmdnx_resmngr);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstatednx_general);
    original_severity_was_initialized = TRUE;
    num_ranges = dnx_data_mpls.general.nof_mpls_range_elements_get(unit);
    first_label = 500;
    num_labels_in_range = 1000;
    /*
     * Step '1'
     */
    {
        if (detailed != FALSE)
        {
            LOG_CLI_EX("1.  Create %d legal ranges %s%s%s\r\n", num_ranges, EMPTY, EMPTY, EMPTY);
        }
        current_label = first_label;
        sal_memset(&action, 0, sizeof(action));
        action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
        /*
         * Create the ranges
         */
        for (range_index = 1; range_index < num_ranges; range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_add(unit, label_low, label_high, &action));
        }
        /*
         * Get the ranges
         */
        current_label = first_label;
        for (range_index = 1; range_index < num_ranges; range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_get(unit, label_low, label_high, &action));
        }
        /*
         * Delete the ranges in a different order.
         */
        range_index = num_ranges / 2;
        sal_memset(&action, 0, sizeof(action));
        action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
        for (index = 1; index < num_ranges; range_index++, index++)
        {
            if (range_index >= num_ranges)
            {
                range_index = 1;
            }
            current_label = first_label + ((range_index - 1) * num_labels_in_range);
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_remove(unit, label_low, label_high));
        }
        if (detailed != FALSE)
        {
            LOG_CLI_EX("1.1 Step '1' completed successfully %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
    }
    /*
     * Step '2'
     */
    {
        if (detailed != FALSE)
        {
            LOG_CLI_EX("2.  Try to add illegal ranges - Expect BCM_E_EXISTS error (%d) %s%s%s\r\n", BCM_E_EXISTS, EMPTY,
                       EMPTY, EMPTY);
        }
        current_label = first_label;
        sal_memset(&action, 0, sizeof(action));
        action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
        /*
         * Create the ranges
         */
        for (range_index = 1; range_index < (num_ranges / 2); range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_add(unit, label_low, label_high, &action));
        }
        /*
         * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
        current_label = first_label;
        label_low = current_label;
        current_label += (num_labels_in_range - 1);
        label_high = current_label;
        current_label++;
        SHR_ASSERT_EQ(BCM_E_EXISTS, bcm_mpls_range_action_add(unit, label_low, label_high, &action));
        /*
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        current_label = first_label;
        /*
         * Clean up. Remove created the ranges
         */
        for (range_index = 1; range_index < (num_ranges / 2); range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_remove(unit, label_low, label_high));
        }
        if (detailed != FALSE)
        {
            LOG_CLI_EX
                ("2.1 Step '2' completed successfully. All is OK: BCM_E_EXISTS error (%d) was ejected as expected.  %s%s%s\n",
                 BCM_E_EXISTS, EMPTY, EMPTY, EMPTY);
        }
    }
    /*
     * Step '3'
     */
    {
        if (detailed != FALSE)
        {
            LOG_CLI_EX("3.  Try to add more than %d ranges - Expect BCM_E_FULL error (%d) %s%s\r\n", num_ranges,
                       BCM_E_FULL, EMPTY, EMPTY);
        }
        current_label = first_label;
        sal_memset(&action, 0, sizeof(action));
        action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
        /*
         * Create the ranges
         */
        for (range_index = 1; range_index < num_ranges; range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_add(unit, label_low, label_high, &action));
        }
        /*
         * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
        label_low = current_label;
        current_label += (num_labels_in_range - 1);
        label_high = current_label;
        current_label++;
        SHR_ASSERT_EQ(BCM_E_FULL, bcm_mpls_range_action_add(unit, label_low, label_high, &action));
        /*
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_bcmdnx_resmngr);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstatednx_general);
        /*
         * Clean up. Remove created ranges.
         */
        current_label = first_label;
        for (range_index = 1; range_index < num_ranges; range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_remove(unit, label_low, label_high));
        }
        if (detailed != FALSE)
        {
            LOG_CLI_EX
                ("3.1 Step '3' completed successfully. All is OK: BCM_E_FULL error (%d) was ejected as expected.  %s%s%s\n",
                 BCM_E_FULL, EMPTY, EMPTY, EMPTY);
        }
    }
    /*
     * Step '4'
     */
    {
        if (detailed != FALSE)
        {
            LOG_CLI_EX("4.  Try to delete a non-existing range - Expect BCM_E_NOT_FOUND error (%d) %s%s%s\r\n",
                       BCM_E_NOT_FOUND, EMPTY, EMPTY, EMPTY);
        }
        current_label = first_label;
        sal_memset(&action, 0, sizeof(action));
        action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
        /*
         * Create the ranges
         */
        for (range_index = 1; range_index < num_ranges; range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_add(unit, label_low, label_high, &action));
        }
        /*
         * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
        label_low = current_label;
        current_label += (2 * num_labels_in_range - 1);
        label_high = current_label;
        current_label++;
        SHR_ASSERT_EQ(BCM_E_NOT_FOUND, bcm_mpls_range_action_remove(unit, label_low, label_high));
        /*
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        /*
         * Clean up. Remove created ranges.
         */
        current_label = first_label;
        for (range_index = 1; range_index < num_ranges; range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_remove(unit, label_low, label_high));
        }
        if (detailed != FALSE)
        {
            LOG_CLI_EX
                ("4.1 Step '4' completed successfully. All is OK: BCM_E_NOT_FOUND error (%d) was ejected as expected.  %s%s%s\n",
                 BCM_E_NOT_FOUND, EMPTY, EMPTY, EMPTY);
        }
    }
    /*
     * Step '5'
     */
    {
        if (detailed != FALSE)
        {
            LOG_CLI_EX
                ("5.  Try to get a non-existing range - Expect BCM_E_NOT_FOUND error (%d) and no error log.\r\n"
                 "==> Note: It is up to the viewing user to verify no error log is injected %s%s%s\r\n",
                 BCM_E_NOT_FOUND, EMPTY, EMPTY, EMPTY);
        }
        current_label = first_label;
        sal_memset(&action, 0, sizeof(action));
        action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
        /*
         * Create the ranges
         */
        for (range_index = 1; range_index < num_ranges; range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_add(unit, label_low, label_high, &action));
        }
        current_label = first_label;
        label_low = current_label;
        current_label += (2 * num_labels_in_range - 1);
        label_high = current_label;
        current_label++;
        sal_memset(&action, 0, sizeof(action));
        SHR_ASSERT_EQ(BCM_E_NOT_FOUND, bcm_mpls_range_action_get(unit, label_low, label_high, &action));
        /*
         * Clean up. Remove created ranges.
         */
        current_label = first_label;
        for (range_index = 1; range_index < num_ranges; range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_remove(unit, label_low, label_high));
        }
        if (detailed != FALSE)
        {
            LOG_CLI_EX
                ("5.1 Step '5' completed successfully. All is OK: BCM_E_NOT_FOUND error (%d) was reported as expected.  %s%s%s\n",
                 BCM_E_NOT_FOUND, EMPTY, EMPTY, EMPTY);
        }
    }
    /*
     * Step '6'
     */
    {
        if (detailed != FALSE)
        {
            LOG_CLI_EX
                ("6.  Try to add a range with a 'reserved' label - Expect BCM_E_PARAM error (%d). %s%s%s\r\n",
                 BCM_E_PARAM, EMPTY, EMPTY, EMPTY);
        }
        current_label = MPLS_MAX_RESERVED_LABEL;
        sal_memset(&action, 0, sizeof(action));
        action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
        /*
         * Create the range. Expect to fail
         */
        /*
         * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
        label_low = current_label;
        current_label += (num_labels_in_range - 1);
        label_high = current_label;
        SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_range_action_add(unit, label_low, label_high, &action));
        /*
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        if (detailed != FALSE)
        {
            LOG_CLI_EX
                ("6.1 Step '6' completed successfully. All is OK: BCM_E_PARAM error (%d) was reported as expected.  %s%s%s\n",
                 BCM_E_PARAM, EMPTY, EMPTY, EMPTY);
        }
    }
    /*
     * Step '7'
     */
    {
        int num_labels;
        bcm_mpls_label_t in_labels[2];
        int is_iml;
        bcm_mpls_label_t found_labels_low, found_labels_high;
        int profile_index;

        if (detailed != FALSE)
        {
            LOG_CLI_EX("7.  DNX level. Locate a range on some profile %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
        current_label = first_label;
        sal_memset(&action, 0, sizeof(action));
        action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
        /*
         * Create the ranges
         */
        for (range_index = 1; range_index < num_ranges; range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_add(unit, label_low, label_high, &action));
        }
        profile_index = 3;
        current_label = first_label + (profile_index * num_labels_in_range);
        num_labels = 1;
        in_labels[0] = current_label;
        SHR_IF_ERR_EXIT(dnx_mpls_label_iml_range_check
                        (unit, num_labels, in_labels, &is_iml, &found_labels_low, &found_labels_high));
        /*
         * This label should now be found on the third profile (as per 'profile_index').
         */
        if (is_iml == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Procedure dnx_mpls_label_iml_range_check() has failed to detect mpls label 0x%08X\r\n"
                         "==> as being IML and as residing on range no. %d. Range is (%d,%d)\r\n",
                         current_label, profile_index,
                         first_label + (profile_index * num_labels_in_range),
                         first_label + (profile_index * num_labels_in_range) + (num_labels_in_range - 1));
        }
        if ((found_labels_low != (first_label + (profile_index * num_labels_in_range))) ||
            (found_labels_high != (first_label + (profile_index * num_labels_in_range) + (num_labels_in_range - 1))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Procedure dnx_mpls_label_iml_range_check() has failed to detect mpls label 0x%08X\r\n"
                         "==> as on range no. %d. Range is (%d,%d)\r\n",
                         current_label, profile_index,
                         first_label + (profile_index * num_labels_in_range),
                         first_label + (profile_index * num_labels_in_range) + (num_labels_in_range - 1));
        }
        /*
         * Clean up. Remove created ranges.
         */
        current_label = first_label;
        for (range_index = 1; range_index < num_ranges; range_index++)
        {
            label_low = current_label;
            current_label += (num_labels_in_range - 1);
            label_high = current_label;
            current_label++;
            SHR_IF_ERR_EXIT(bcm_mpls_range_action_remove(unit, label_low, label_high));
        }
        if (detailed != FALSE)
        {
            LOG_CLI_EX("7.1 Step '7' completed successfully. All is OK.  %s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
    }
    /*
     * Just make sure in case there is some error code hanging due to negative tests.
     */
    SHR_SET_CURRENT_ERR(BCM_E_NONE);
exit:
    if (original_severity_was_initialized == TRUE)
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_bcmdnx_resmngr);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstatednx_general);
    }
    if (SHR_GET_CURRENT_ERR() == BCM_E_NONE)
    {
        LOG_CLI_EX("The full test was completed SUCCESSFULLY.  %s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}
/*
 * Range test manual
 */
sh_sand_man_t dnx_mpls_evpn_range_man = {
    .brief = "Semantic test of MPLS EVPN/IML range set/get/delete",
    .full =
        "Semantic test of MPLS EVPN/IML range set/get/delete\n"
        "Tests the various MPLS range operations to their limits, including\n"
        "negative tests\n"
        "1. Create 8 exclusive ranges. Get each of them. Delete all 8 in a different order.\n"
        "2. Try to create ranges which are not exclusive. Get BCM_E_PARAM error\n"
        "3. Try to create more than 8 ranges. Get error\n"
        "4. Try to delete a non-existing range. Get error\n"
        "5. Try to get a non-existing range. Get 'not found' without error log\n"
        "6. Try to add range which contains a 'reserved' MPLS label\n"
        "7. DNX level. Locate a range on some range (not necessarily first)",
    .synopsis = "[DETAiled]",
    .examples = "detailed\n"
};

sh_sand_option_t dnx_mpls_evpn_range_options[] = {
    {
     .keyword = "detailed",
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Display extra printouts while carrying out this test",
     .def = "False",
     }
    ,
    {NULL}
};

/*
 * }
 */
/**
 * \brief Test EVPN/IML encapsulation.
 * For detailed description of the test, see the manual.
 */
static shr_error_e
dnx_mpls_evpn_encap_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int nof_tests = 0, test_i;
    int test_mc;
    int test_uc;
    int test_cw;
    char *test_names[3];
    uint32 eg_label_flag[3];
    bcm_encap_access_t enc_acc[3];
    bcm_mpls_egress_label_t *p_eg_labels[2] = { NULL, NULL };
    bcm_mpls_egress_label_t eg_label_wo_id;
    bcm_mpls_egress_label_t eg_label_w_id;
    bcm_mpls_egress_label_t eg_label_buffer;
    bcm_failover_t failover_id = 0;
    /*
     * This variables stores the severity of LIF dnx
     */
    bsl_severity_t original_severity_bcmdnx_lif = 0;
    /*
     * This variables stores the severity of mpls dnx
     */
    bsl_severity_t original_severity_bcmdnx_mpls = 0;
    /*
     * This variables stores the severity of port dnx
     */
    bsl_severity_t original_severity_bcmdnx_port = 0;

    SHR_FUNC_INIT_VARS(unit);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_PORT, original_severity_bcmdnx_port);
    SH_SAND_GET_BOOL("unicast", test_uc);
    SH_SAND_GET_BOOL("multicast", test_mc);
    SH_SAND_GET_BOOL("ControlWord", test_cw);

    /*
     * If no argument given, test all
     */
    if (!test_uc && !test_mc)
    {
        test_uc = test_mc = TRUE;
    }

    /*
     * Note regarding encap_access:
     * For L2VPN, Tunnel1 and Invalid act the same way, so when getting
     * a label that was set with bcmEncapAccessTunnel1, the encap_access
     * received is bcmEncapAccessInvalid. This is by design, for the sake
     * of default behavior.
     * For MPLS tunnel, the same situation happens with Tunnel2 and Invalid.
     *
     * For this test, then, only consistent encapsulation accesses were
     * selected. When an entry is re-added, its encap_access is replaced
     * with (2 - <original encap_access value>) and should remain consistent.
     */
    if (test_uc)
    {
        test_names[nof_tests] = "Unicast";
        eg_label_flag[nof_tests] = BCM_MPLS_EGRESS_LABEL_EVPN;
        enc_acc[nof_tests] = bcmEncapAccessTunnel2;
        if (test_cw)
        {
            eg_label_flag[nof_tests] |= BCM_MPLS_EGRESS_LABEL_CONTROL_WORD;
        }
        nof_tests++;
    }
    if (test_mc)
    {
        test_names[nof_tests] = "Multicast";
        eg_label_flag[nof_tests] = BCM_MPLS_EGRESS_LABEL_IML;
        enc_acc[nof_tests] = bcmEncapAccessInvalid;
        if (test_cw)
        {
            eg_label_flag[nof_tests] |= BCM_MPLS_EGRESS_LABEL_CONTROL_WORD;
        }
        nof_tests++;
    }

    for (test_i = 0; test_i < nof_tests; test_i++)
    {
        int label_count;
        uint32 test_flags = eg_label_flag[test_i] | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

        LOG_CLI((BSL_META("\n\n*\n* %s Test\n*\n"), test_names[test_i]));

        SHR_IF_ERR_EXIT(bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &failover_id));

        LOG_CLI((BSL_META("1. Create EVPN label without ID\n")));
        bcm_mpls_egress_label_t_init(&eg_label_wo_id);
        eg_label_wo_id.label = User_labels[0];
        eg_label_wo_id.flags = test_flags | BCM_MPLS_EGRESS_LABEL_PROTECTION;
        eg_label_wo_id.encap_access = enc_acc[test_i];
        eg_label_wo_id.egress_failover_id = failover_id;
        eg_label_wo_id.egress_failover_if_id = 1;
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_wo_id));
        p_eg_labels[0] = &eg_label_wo_id;

        LOG_CLI((BSL_META("2. Create EVPN label WITH_ID\n")));
        bcm_mpls_egress_label_t_init(&eg_label_w_id);
        eg_label_w_id.label = User_labels[1];
        eg_label_w_id.flags = test_flags | BCM_MPLS_EGRESS_LABEL_WITH_ID;
        eg_label_w_id.encap_access = enc_acc[test_i];
        BCM_L3_ITF_SET(eg_label_w_id.tunnel_id, BCM_L3_ITF_TYPE_LIF, 0x4100);
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_w_id));
        p_eg_labels[1] = &eg_label_w_id;
        {
            bcm_mpls_tunnel_initiator_traverse_info_t additional_info;

            LOG_CLI((BSL_META("3. Traverse over all local outlif's\n")));
            /*
             * Set traverse flags as per flags used for 'bcm_mpls_tunnel_initiator_create' above.
             */
            additional_info.flags = eg_label_flag[test_i];
            User_data_initiator[(sizeof(User_data_initiator) / sizeof(User_data_initiator[0])) - 1] =
                additional_info.flags;
            SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_traverse
                            (unit, &additional_info, tunnel_initiator_traverse_cb, (void *) User_data_initiator));
        }
        {
            /*
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            LOG_CLI((BSL_META("4. Try to create label WITH_ID with already created ID - expect parameter error\n")));
            /*
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            bcm_mpls_egress_label_t_init(&eg_label_buffer);
            eg_label_buffer.label = 444;
            eg_label_buffer.flags = test_flags | BCM_MPLS_EGRESS_LABEL_WITH_ID;
            eg_label_buffer.encap_access = enc_acc[test_i];
            eg_label_buffer.tunnel_id = eg_label_wo_id.tunnel_id;
            SHR_ASSERT_EQ(BCM_E_EXISTS, bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_buffer));
            /*
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        }
        {
            LOG_CLI((BSL_META("5. Try to create label with label out of range - expect parameter error\n")));
            /*
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            bcm_mpls_egress_label_t_init(&eg_label_buffer);
            eg_label_buffer.label = 0xfffff + 1;
            eg_label_buffer.flags = test_flags;
            eg_label_buffer.encap_access = enc_acc[test_i];
            SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_buffer));
            bcm_mpls_egress_label_t_init(&eg_label_buffer);
            eg_label_buffer.label = -1;
            eg_label_buffer.flags = test_flags;
            eg_label_buffer.encap_access = enc_acc[test_i];
            SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_buffer));
            /*
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        }
        LOG_CLI((BSL_META("6. Get egress label and compare fields\n")));
        bcm_mpls_egress_label_t_init(&eg_label_buffer);
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_get
                        (unit, p_eg_labels[0]->tunnel_id, 1, &eg_label_buffer, &label_count));
        SHR_ASSERT_EQ(1, label_count);
        SHR_ASSERT_EQ(p_eg_labels[0]->label, eg_label_buffer.label);
        SHR_ASSERT_EQ(p_eg_labels[0]->flags, eg_label_buffer.flags);
        SHR_ASSERT_EQ(p_eg_labels[0]->encap_access, eg_label_buffer.encap_access);
        SHR_ASSERT_EQ(p_eg_labels[0]->tunnel_id, eg_label_buffer.tunnel_id);
        SHR_ASSERT_EQ(p_eg_labels[0]->egress_failover_id, eg_label_buffer.egress_failover_id);
        SHR_ASSERT_EQ(p_eg_labels[0]->egress_failover_if_id, eg_label_buffer.egress_failover_if_id);

        LOG_CLI((BSL_META("7. Delete egress label\n")));
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_clear(unit, p_eg_labels[1]->tunnel_id));
        p_eg_labels[1] = NULL;
        {
            LOG_CLI((BSL_META("8. Try to get a deleted label - expect not found error\n")));
            /*
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_PORT, bslSeverityFatal);
            bcm_mpls_egress_label_t_init(&eg_label_buffer);
            SHR_ASSERT_EQ(BCM_E_NOT_FOUND, bcm_mpls_tunnel_initiator_get
                          (unit, eg_label_w_id.tunnel_id, 1, &eg_label_buffer, &label_count));
            /*
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_PORT, original_severity_bcmdnx_port);
        }
        LOG_CLI((BSL_META("9. Re-add deleted label\n")));
        eg_label_w_id.encap_access = 2 - enc_acc[test_i];
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_w_id));
        p_eg_labels[1] = &eg_label_w_id;

        LOG_CLI((BSL_META("10. Replace TTL mode\n")));
        eg_label_w_id.flags |= BCM_MPLS_EGRESS_LABEL_REPLACE;
        eg_label_w_id.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_w_id));
        bcm_mpls_egress_label_t_init(&eg_label_buffer);
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_get
                        (unit, p_eg_labels[1]->tunnel_id, 1, &eg_label_buffer, &label_count));
        SHR_ASSERT_EQ(1, label_count);
        SHR_ASSERT_EQ(p_eg_labels[1]->label, eg_label_buffer.label);
        SHR_ASSERT_EQ(p_eg_labels[1]->flags & (~(BCM_MPLS_EGRESS_LABEL_REPLACE | BCM_MPLS_EGRESS_LABEL_WITH_ID)),
                      eg_label_buffer.flags);
        SHR_ASSERT_EQ(p_eg_labels[1]->tunnel_id, eg_label_buffer.tunnel_id);
        {
            LOG_CLI((BSL_META("11. Try to create egress label with forbidden flag\n")));
            /*
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            bcm_mpls_egress_label_t_init(&eg_label_buffer);
            eg_label_buffer.label = 555;
            eg_label_buffer.flags = test_flags | BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP;
            eg_label_buffer.encap_access = enc_acc[test_i];
            SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_buffer));
            /*
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        }
        {
            LOG_CLI((BSL_META
                     ("12. Try to create egress label with missing TTL_DECREMENT flag - expect parameter error\n")));
            /*
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            bcm_mpls_egress_label_t_init(&eg_label_buffer);
            eg_label_buffer.label = 666;
            eg_label_buffer.flags = test_flags & ~BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            eg_label_buffer.encap_access = enc_acc[test_i];
            SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_buffer));
            /*
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        }
        {
            LOG_CLI((BSL_META("13. Try to create egress label with TTL_COPY flag - expect parameter error\n")));
            /*
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            bcm_mpls_egress_label_t_init(&eg_label_buffer);
            eg_label_buffer.label = 777;
            eg_label_buffer.flags = test_flags | BCM_MPLS_EGRESS_LABEL_TTL_COPY;
            eg_label_buffer.encap_access = enc_acc[test_i];
            SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_buffer));
            /*
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        }
        {
            LOG_CLI((BSL_META("14. Try to create egress label with EXP_COPY flag - expect parameter error\n")));
            /*
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            bcm_mpls_egress_label_t_init(&eg_label_buffer);
            eg_label_buffer.label = 555;
            eg_label_buffer.flags = test_flags | BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            eg_label_buffer.encap_access = enc_acc[test_i];
            SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_buffer));
            /*
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        }
        {
            LOG_CLI((BSL_META("15. Try to create egress label with TTL_SET flag - expect parameter error\n")));
            /*
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            bcm_mpls_egress_label_t_init(&eg_label_buffer);
            eg_label_buffer.label = 555;
            eg_label_buffer.flags = test_flags | BCM_MPLS_EGRESS_LABEL_TTL_SET;
            eg_label_buffer.encap_access = enc_acc[test_i];
            SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_buffer));
            /*
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        }
        {
            LOG_CLI((BSL_META("16. Try to create egress label with EXP_SET flag - expect parameter error\n")));
            /*
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            bcm_mpls_egress_label_t_init(&eg_label_buffer);
            eg_label_buffer.label = 555;
            eg_label_buffer.flags = test_flags | BCM_MPLS_EGRESS_LABEL_EXP_SET;
            eg_label_buffer.encap_access = enc_acc[test_i];
            SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label_buffer));
            /*
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
        }
        LOG_CLI((BSL_META("17. Clean up\n")));
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_clear(unit, eg_label_wo_id.tunnel_id));
        p_eg_labels[0] = NULL;
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_clear(unit, eg_label_w_id.tunnel_id));
        p_eg_labels[1] = NULL;
        if (failover_id)
        {
            SHR_IF_ERR_EXIT(bcm_failover_destroy(unit, failover_id));
            failover_id = 0;
        }
    }

    LOG_CLI((BSL_META("\n\n\n >>> Test PASSED <<<\n\n\n\n")));

exit:
    /*
     *  Restore the original severity after the end of Negative test.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_PORT, original_severity_bcmdnx_port);
    if (p_eg_labels[0] != NULL)
    {
        bcm_mpls_tunnel_initiator_clear(unit, p_eg_labels[0]->tunnel_id);
    }
    if (p_eg_labels[1] != NULL)
    {
        bcm_mpls_tunnel_initiator_clear(unit, p_eg_labels[1]->tunnel_id);
    }
    if (failover_id)
    {
        bcm_failover_destroy(unit, failover_id);
    }
    if (SHR_FUNC_ERR())
    {
        LOG_CLI((BSL_META("\n\n <<<  Test FAIL  >>>\n\n\n")));
    }
    SHR_FUNC_EXIT;
}

/** Test manual   */
sh_sand_man_t dnx_mpls_evpn_encap_test_man = {
    /** Brief */
    "Semantic test of MPLS EVPN/IML encapsulation for UC/MC traffic",
    /** Full */
    "Semantic test of MPLS EVPN/IML encapsulation for UC/MC traffic\n"
        "Tests MPLS tunnel creation/etc. with the ADD_ESI flag along the way\n"
        "1.  Create egress label\n"
        "2.  Create egress label WITH_ID\n"
        "3.  Traverse over all local outlif's\r\n"
        "4.  Try to create label WITH_ID with already created ID - expect parameter error\n"
        "5.  Try to create label with label out of range - expect parameter error\n"
        "6.  Get egress label and compare fields\n"
        "7.  Delete egress label\n"
        "8.  Try to get a deleted label - expect not found error\n"
        "9.  Re-add deleted label\n"
        "10. Replace TTL mode\n"
        "11. Try to create egress label with forbidden flag - expect parameter error\n"
        "12. Try to create egress label with missing TTL_DECREMENT flag - expect parameter error\n"
        "13. Try to create egress label with TTL_COPY flag - expect parameter error\n"
        "14. Try to create egress label with EXP_COPY flag - expect parameter error\n"
        "15. Try to create egress label with TTL_SET flag - expect parameter error\n"
        "16. Try to create egress label with EXP_SET flag - expect parameter error\n"
        "17. Clean up - Delete the created labels.",
    /** Synopsis */
    "[UniCast] [MultiCast] [CW]",
    /** Examples */
    "uc\n" "mc\n" "cw\n" "uc cw\n" "mc cw\n"
};

/**
 * \brief Test EVPN/IML termination.
 * For detailed description of the test, see the manual.
 */
static shr_error_e
dnx_mpls_evpn_term_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int nof_tests = 0, test_i = 0;
    int test_mc;
    int test_uc;
    int test_cw;
    char *test_names[3];
    uint32 tunnel_switch_flag[3];
    uint32 label_range_start[3];
    const int label_range_size = 500;
    int label_range_was_loaded = FALSE;
    bcm_vpn_t vpn = 0;
    bcm_failover_t failover_id = 0;

    bcm_mpls_tunnel_switch_t *tunnel_switch_to_delete[2] = { NULL, NULL };
    bcm_mpls_tunnel_switch_t tunnel_wo_id;
    bcm_mpls_tunnel_switch_t tunnel_w_id;
    bcm_mpls_tunnel_switch_t tunnel_buffer;
    /*
     * This variables stores the severity of LIF dnx
     */
    bsl_severity_t original_severity_bcmdnx_lif;
    /*
     * This variables stores the severity of mpls dnx
     */
    bsl_severity_t original_severity_bcmdnx_mpls;

    SHR_FUNC_INIT_VARS(unit);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
    SH_SAND_GET_BOOL("unicast", test_uc);
    SH_SAND_GET_BOOL("multicast", test_mc);
    SH_SAND_GET_BOOL("ControlWord", test_cw);
    /*
     * If no argument given, test all
     */
    if (!test_uc && !test_mc)
    {
        test_uc = test_mc = TRUE;
    }

    if (test_uc)
    {
        test_names[nof_tests] = "Unicast";
        tunnel_switch_flag[nof_tests] = BCM_MPLS_SWITCH_NEXT_HEADER_L2;
        label_range_start[nof_tests] = 0;
        nof_tests++;
    }
    if (test_mc)
    {
        if (dnx_data_mpls.general.feature_get(unit, dnx_data_mpls_general_mpls_term_1_or_2_labels))
        {
            /** If the feature is support, IML can be terminated togather with ESI.*/
            test_names[nof_tests] = "Multicast with or without ESI";
            tunnel_switch_flag[nof_tests] = BCM_MPLS_SWITCH_EVPN_IML;
            label_range_start[nof_tests] = 500;
            nof_tests++;
        }
        else
        {
            test_names[nof_tests] = "Multicast (IML without ESI)";
            tunnel_switch_flag[nof_tests] = BCM_MPLS_SWITCH_EVPN_IML | BCM_MPLS_SWITCH_EXPECT_BOS;
            label_range_start[nof_tests] = 500;
            nof_tests++;

            test_names[nof_tests] = "Multicast (IML with ESI)";
            tunnel_switch_flag[nof_tests] = BCM_MPLS_SWITCH_EVPN_IML;
            label_range_start[nof_tests] = 1500;
            nof_tests++;
        }
    }

    for (test_i = 0; test_i < nof_tests; test_i++)
    {
        uint32 test_flags = tunnel_switch_flag[test_i];
        for (vpn = 0; vpn <= 33; vpn += 33)
        {
            uint32 tunnel_switch_flag2 = (test_cw ? BCM_MPLS_SWITCH2_CONTROL_WORD : 0);
            if (vpn)
            {
                SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vpn));
            }
            else
            {
                tunnel_switch_flag2 |= BCM_MPLS_SWITCH2_CROSS_CONNECT;
            }

            LOG_CLI((BSL_META("\n\n*\n* %s %s Test\n*\n"), test_names[test_i], (vpn ? "MP" : "P2P")));

            SHR_IF_ERR_EXIT(bcm_failover_create(unit, BCM_FAILOVER_INGRESS, &failover_id));

            if (_SHR_IS_FLAG_SET(test_flags, BCM_MPLS_SWITCH_EVPN_IML))
            {
                uint32 low = label_range_start[test_i];
                uint32 high = label_range_start[test_i] + label_range_size;
                bcm_mpls_range_action_t action;

                LOG_CLI((BSL_META("For IML test, first define range (%d-%d)\n"), low, high));

                bcm_mpls_range_action_t_init(&action);
                action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
                SHR_IF_ERR_EXIT(bcm_mpls_range_action_add(unit, low, high, &action));
                label_range_was_loaded = TRUE;
            }

            LOG_CLI((BSL_META("1. Create EVPN label without ID\n")));
            bcm_mpls_tunnel_switch_t_init(&tunnel_wo_id);
            tunnel_wo_id.label = label_range_start[test_i] + 111;
            tunnel_wo_id.flags = test_flags;
            tunnel_wo_id.flags2 = tunnel_switch_flag2;
            tunnel_wo_id.action = BCM_MPLS_SWITCH_ACTION_POP;
            tunnel_wo_id.failover_id = failover_id;
            tunnel_wo_id.failover_tunnel_id = 1;
            tunnel_wo_id.vpn = vpn;
            SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &tunnel_wo_id));
            tunnel_switch_to_delete[0] = &tunnel_wo_id;

            LOG_CLI((BSL_META("2. Create EVPN label WITH_ID\n")));
            bcm_mpls_tunnel_switch_t_init(&tunnel_w_id);
            tunnel_w_id.label = label_range_start[test_i] + 222;
            tunnel_w_id.flags = test_flags | BCM_MPLS_SWITCH_WITH_ID;
            tunnel_w_id.flags2 = tunnel_switch_flag2;
            tunnel_w_id.action = BCM_MPLS_SWITCH_ACTION_POP;
            tunnel_w_id.vpn = vpn;
            BCM_GPORT_TUNNEL_ID_SET(tunnel_w_id.tunnel_id, tunnel_wo_id.tunnel_id + 2);
            SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &tunnel_w_id));
            tunnel_switch_to_delete[1] = &tunnel_w_id;
            {
                LOG_CLI((BSL_META
                         ("3. Try to create label WITH_ID with already created ID - expect parameter error\n")));
                /*
                 * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
                bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
                tunnel_buffer.label = label_range_start[test_i] + 333;
                tunnel_buffer.flags = test_flags | BCM_MPLS_SWITCH_WITH_ID;
                tunnel_buffer.flags2 = tunnel_switch_flag2;
                tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
                tunnel_buffer.tunnel_id = tunnel_wo_id.tunnel_id;
                tunnel_buffer.vpn = vpn;
                SHR_ASSERT_EQ(BCM_E_EXISTS, bcm_mpls_tunnel_switch_create(unit, &tunnel_buffer));
                /*
                 *  Restore the original severity after the end of Negative test.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
                LOG_CLI_EX("3.1 All is OK: BCM_E_EXISTS error (%d) was ejected as expected.  %s%s%s\n", BCM_E_EXISTS,
                           EMPTY, EMPTY, EMPTY);
            }
            {
                LOG_CLI((BSL_META("4. Try to create label with label out of range - expect parameter error\n")));

                /*
                 * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
                bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
                tunnel_buffer.label = 0xfffff + 1;
                tunnel_buffer.flags = test_flags;
                tunnel_buffer.flags2 = tunnel_switch_flag2;
                tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
                tunnel_buffer.vpn = vpn;
                SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_switch_create(unit, &tunnel_buffer));
                bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
                tunnel_buffer.label = -1;
                tunnel_buffer.flags = test_flags;
                tunnel_buffer.flags2 = tunnel_switch_flag2;
                tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
                tunnel_buffer.vpn = vpn;
                SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_switch_create(unit, &tunnel_buffer));
                if (_SHR_IS_FLAG_SET(test_flags, BCM_MPLS_SWITCH_EVPN_IML))
                {
                    LOG_CLI(("4.1 Try to create IML label below the defined IML range.\n"));
                    bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
                    tunnel_buffer.label = label_range_start[test_i] - 1;
                    tunnel_buffer.flags = test_flags;
                    tunnel_buffer.flags2 = tunnel_switch_flag2;
                    tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
                    SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_switch_create(unit, &tunnel_buffer));

                    LOG_CLI(("4.2 Try to create IML label above the defined IML range.\n"));
                    bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
                    tunnel_buffer.label = label_range_start[test_i] + label_range_size + 1;
                    tunnel_buffer.flags = test_flags;
                    tunnel_buffer.flags2 = tunnel_switch_flag2;
                    tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
                    SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_switch_create(unit, &tunnel_buffer));

                    LOG_CLI(("4.3 Try to create MPLS LER label inside a defined IML range.\n"));
                    bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
                    tunnel_buffer.label = label_range_start[test_i] + 1;
                    tunnel_buffer.flags = 0;
                    tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
                    SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_switch_create(unit, &tunnel_buffer));

                    LOG_CLI(("4.4 Try to create EVPN UC label inside a defined IML range.\n"));
                    bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
                    tunnel_buffer.label = label_range_start[test_i] + label_range_size;
                    tunnel_buffer.flags = BCM_MPLS_SWITCH_NEXT_HEADER_L2;
                    tunnel_buffer.flags2 = tunnel_switch_flag2;
                    tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
                    SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_switch_create(unit, &tunnel_buffer));
                }
                /*
                 *  Restore the original severity after the end of Negative test.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
                LOG_CLI_EX("4.5 All is OK: BCM_E_PARAM error (%d) was ejected as expected.  %s%s%s\n", BCM_E_PARAM,
                           EMPTY, EMPTY, EMPTY);
            }
            LOG_CLI((BSL_META("5. Get label and compare fields\n")));
            bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
            tunnel_buffer.label = tunnel_wo_id.label;
            tunnel_buffer.flags = test_flags;
            tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
            SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_get(unit, &tunnel_buffer));
            SHR_ASSERT_EQ(tunnel_wo_id.tunnel_id, tunnel_buffer.tunnel_id);
            SHR_ASSERT_EQ(tunnel_wo_id.flags2, tunnel_buffer.flags2);
            SHR_ASSERT_EQ(tunnel_wo_id.vpn, tunnel_buffer.vpn);
            SHR_ASSERT_EQ(tunnel_wo_id.failover_id, tunnel_buffer.failover_id);
            SHR_ASSERT_EQ(tunnel_wo_id.failover_tunnel_id, tunnel_buffer.failover_tunnel_id);
            {
                LOG_CLI((BSL_META("6. Traverse over all labels\n")));
                SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_traverse
                                (unit, tunnel_switch_traverse_cb, (void *) (&test_flags)));
            }
            LOG_CLI((BSL_META("7. Delete egress label\n")));
            SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_delete(unit, &tunnel_w_id));
            tunnel_switch_to_delete[1] = NULL;
            {
                LOG_CLI((BSL_META("8. Try to get a deleted label - expect not found error\n")));
                /*
                 * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
                bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
                tunnel_buffer.label = tunnel_w_id.label;
                tunnel_buffer.flags = test_flags;
                tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
                SHR_ASSERT_EQ(BCM_E_NOT_FOUND, bcm_mpls_tunnel_switch_get(unit, &tunnel_buffer));
                /*
                 *  Restore the original severity after the end of Negative test.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
                LOG_CLI_EX("8.1 All is OK: BCM_E_NOT_FOUND error (%d) was ejected as expected.  %s%s%s\n",
                           BCM_E_NOT_FOUND, EMPTY, EMPTY, EMPTY);
            }
            LOG_CLI((BSL_META("9. Re-add deleted label\n")));
            SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &tunnel_w_id));
            tunnel_switch_to_delete[1] = &tunnel_w_id;

            LOG_CLI((BSL_META("10. Enable STAT mode\n")));
            tunnel_w_id.flags |= BCM_MPLS_SWITCH_WITH_ID | BCM_MPLS_SWITCH_REPLACE;
            tunnel_w_id.flags2 |= BCM_MPLS_SWITCH2_STAT_ENABLE;
            SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &tunnel_w_id));
            bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
            tunnel_buffer.label = tunnel_w_id.label;
            tunnel_buffer.flags = test_flags;
            tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
            SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_get(unit, &tunnel_buffer));
            SHR_ASSERT_EQ(tunnel_w_id.flags & (~(BCM_MPLS_SWITCH_WITH_ID | BCM_MPLS_SWITCH_REPLACE)),
                          tunnel_buffer.flags);
            SHR_ASSERT_EQ(tunnel_w_id.flags2, tunnel_buffer.flags2);
            SHR_ASSERT_EQ(tunnel_w_id.vpn, tunnel_buffer.vpn);
            SHR_ASSERT_EQ(tunnel_w_id.tunnel_id, tunnel_buffer.tunnel_id);
            {
                LOG_CLI((BSL_META("11. Try to create label with forbidden flag - expect param error\n")));
                /*
                 * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, bslSeverityFatal);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
                bcm_mpls_tunnel_switch_t_init(&tunnel_buffer);
                tunnel_buffer.label = label_range_start[test_i] + 444;
                tunnel_buffer.flags = test_flags | BCM_MPLS_SWITCH_FRR;
                tunnel_buffer.flags2 = tunnel_switch_flag2;
                tunnel_buffer.action = BCM_MPLS_SWITCH_ACTION_POP;
                tunnel_buffer.vpn = vpn;
                SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_switch_create(unit, &tunnel_buffer));
                /*
                 *  Restore the original severity after the end of Negative test.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
                LOG_CLI_EX("10.1 All is OK: BCM_E_PARAM error (%d) was ejected as expected.  %s%s%s\n", BCM_E_PARAM,
                           EMPTY, EMPTY, EMPTY);
            }
            LOG_CLI((BSL_META("12. Clean up\n")));
            SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_delete(unit, tunnel_switch_to_delete[0]));
            tunnel_switch_to_delete[0] = NULL;
            SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_delete(unit, tunnel_switch_to_delete[1]));
            tunnel_switch_to_delete[1] = NULL;
            if (label_range_was_loaded == TRUE)
            {
                /*
                 * Delete IML range
                 */
                bcm_mpls_label_t label_low;
                bcm_mpls_label_t label_high;

                label_low = label_range_start[test_i];
                label_high = label_range_start[test_i] + label_range_size;
                SHR_IF_ERR_EXIT(bcm_mpls_range_action_remove(unit, label_low, label_high));
                label_range_was_loaded = FALSE;
            }
            if (vpn)
            {
                SHR_IF_ERR_EXIT(bcm_vlan_destroy(unit, vpn));
            }
            if (failover_id)
            {
                SHR_IF_ERR_EXIT(bcm_failover_destroy(unit, failover_id));
                failover_id = 0;
            }
        }
    }

    LOG_CLI((BSL_META("\n\n\n >>> Test PASSED <<<\n\n\n\n")));

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LIF, original_severity_bcmdnx_lif);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_bcmdnx_mpls);
    if (tunnel_switch_to_delete[0] != NULL)
    {
        bcm_mpls_tunnel_switch_delete(unit, tunnel_switch_to_delete[0]);
    }
    if (tunnel_switch_to_delete[1] != NULL)
    {
        bcm_mpls_tunnel_switch_delete(unit, tunnel_switch_to_delete[1]);
    }
    if (label_range_was_loaded)
    {
        /*
         * Delete IML range
         */
        bcm_mpls_label_t label_low;
        bcm_mpls_label_t label_high;
        int err;

        label_low = label_range_start[test_i];
        label_high = label_range_start[test_i] + label_range_size;
        err = bcm_mpls_range_action_remove(unit, label_low, label_high);
        if (err != 0)
        {
            LOG_CLI_EX("\n\n\n >>> bcm_mpls_range_action_remove() has failed wth error %d <<< %s%s%s\n\n\n\n", err,
                       EMPTY, EMPTY, EMPTY);
        }
    }
    if (SHR_FUNC_ERR())
    {
        if (vpn)
        {
            bcm_vlan_destroy(unit, vpn);
        }
        if (failover_id)
        {
            bcm_failover_destroy(unit, failover_id);
        }
        LOG_CLI((BSL_META("\n\n <<<  Test FAIL  >>>\n\n\n")));
    }
    SHR_FUNC_EXIT;
}

/** Test manual   */
sh_sand_man_t dnx_mpls_evpn_term_test_man = {
    /** Brief */
    "Semantic test of MPLS EVPN/IML termination for UC/MC traffic",
    /** Full */
    "Semantic test of MPLS EVPN/IML termination for UC/MC traffic\n"
        "Tests MPLS tunnel creation/etc. with the ADD_ESI flag along the way\n"
        "1.  Create label\n"
        "2.  Create label WITH_ID\n"
        "3.  Try to create label WITH_ID with already created ID - expect parameter error\n"
        "4.  Try to create label with label out of range - expect parameter error\n"
        "5.  Get label and compare fields\n"
        "6.  Traverse over all labels\n"
        "7.  Delete label\n"
        "8.  Try to get a deleted label - expect not found error\n"
        "9.  Re-add deleted label\n"
        "10. Replace TTL mode\n"
        "11. Try to create egress label with forbidden flag - expect parameter error\n"
        "12. Clean up. Delete the created labels.",
    /** Synopsis */
    "[UniCast] [MultiCast] [CW]",
    /** Examples */
    "uc\n" "mc\n" "uc cw\n" "mc cw\n" "cw\n"
};

sh_sand_option_t dnx_mpls_evpn_test_options[] = {
    {
     .keyword = "UniCast",
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Test only UC APIs",
     .def = "False",
     }
    ,
    {
     .keyword = "MultiCast",
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Test only MC APIs",
     .def = "False",
     }
    ,
    {
     .keyword = "ControlWord",
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Test with CONTROL_WORD flag",
     .def = "False",
     }
    ,
    {NULL}
};

sh_sand_invoke_t dnx_mpls_evpn_basic_test_invokes[] = {
    {"uc", "UniCast", CTEST_PRECOMMIT}
    ,
    {"mc", "MultiCast", CTEST_PRECOMMIT}
    ,
    {"cw", "ControlWord", CTEST_PRECOMMIT}
    ,
    {NULL}
    ,
};

/**
 * \brief
 *  Create directional cross connection from port1 to <encap2, port2>;
 *  Get and delete this cross connection.
 */
static shr_error_e
dnx_mpls_evpn_p2p_combination_test(
    int unit,
    bcm_gport_t port1,
    uint32 encap2,
    bcm_gport_t port2)
{
    bcm_vswitch_cross_connect_t gports, buffer;
    SHR_FUNC_INIT_VARS(unit);

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = port1;
    gports.encap2 = encap2;
    gports.port2 = port2;
    SHR_IF_ERR_EXIT(bcm_vswitch_cross_connect_add(unit, &gports));
    /** GET */
    bcm_vswitch_cross_connect_t_init(&buffer);
    buffer.port1 = port1;
    SHR_IF_ERR_EXIT(bcm_vswitch_cross_connect_get(unit, &buffer));
    SHR_ASSERT_EQ((gports.port2 & 0xfffff), (buffer.port2 & 0xfffff));
    SHR_ASSERT_EQ(gports.encap2, buffer.encap2);
    /** DELETE */
    SHR_IF_ERR_EXIT(bcm_vswitch_cross_connect_delete(unit, &gports));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Test EVPN usage in VSwitch cross connect (P2P) APIs.
 * For detailed description of the test, see the manual.
 */
static shr_error_e
dnx_mpls_evpn_p2p_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_vlan_port_t ac;
    bcm_mpls_tunnel_switch_t term;
    bcm_mpls_egress_label_t encap;

    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&ac);
    bcm_mpls_egress_label_t_init(&encap);
    bcm_mpls_tunnel_switch_t_init(&term);

    LOG_CLI((BSL_META("\n\n*\n* EVPN VSwitch Cross-Connect Test\n*\n")));

    LOG_CLI((BSL_META("1. Create P2P VLAN port\n")));
    ac.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    ac.port = 200;
    ac.match_vlan = 111;
    ac.flags = 0;
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &ac));

    LOG_CLI((BSL_META("2. Create EVPN encapsulation\n")));
    encap.label = 123;
    encap.flags = BCM_MPLS_EGRESS_LABEL_EVPN | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    encap.encap_access = bcmEncapAccessTunnel1;
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_create(unit, 0, 1, &encap));

    LOG_CLI((BSL_META("3. Create EVPN P2P Termination\n")));
    LOG_CLI((BSL_META("3.1. Create EVPN P2P Termination without the flag - expect error\n")));
    term.action = BCM_MPLS_SWITCH_ACTION_POP;
    term.flags = BCM_MPLS_SWITCH_NEXT_HEADER_L2;
    term.label = 321;
    SHR_ASSERT_EQ(BCM_E_PARAM, bcm_mpls_tunnel_switch_create(unit, &term));
    LOG_CLI((BSL_META("3.2. Create EVPN P2P Termination with BCM_MPLS_SWITCH2_CROSS_CONNECT flag\n")));
    term.flags2 = BCM_MPLS_SWITCH2_CROSS_CONNECT;
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &term));

    LOG_CLI((BSL_META("4. Cross-Connect VLAN->EVPN; Get and delete the connection.\n")));
    BCM_GPORT_LOCAL_SET(gport, 201);
    SHR_IF_ERR_EXIT(dnx_mpls_evpn_p2p_combination_test
                    (unit, ac.vlan_port_id, BCM_L3_ITF_VAL_GET(encap.tunnel_id), gport));

    LOG_CLI((BSL_META("5. Cross-Connect EVPN->VLAN; Get and delete the connection.\n")));
    SHR_IF_ERR_EXIT(dnx_mpls_evpn_p2p_combination_test
                    (unit, term.tunnel_id, BCM_FORWARD_ENCAP_ID_INVALID, ac.vlan_port_id));

    LOG_CLI((BSL_META("6. Cross-Connect EVPN->EVPN; Get and delete the connection.\n")));
    SHR_IF_ERR_EXIT(dnx_mpls_evpn_p2p_combination_test
                    (unit, term.tunnel_id, BCM_L3_ITF_VAL_GET(encap.tunnel_id), gport));

    LOG_CLI((BSL_META("\n\n\n >>> Test PASSED <<<\n\n\n\n")));

exit:
    bcm_vswitch_cross_connect_delete_all(unit);
    bcm_mpls_tunnel_switch_delete(unit, &term);
    bcm_mpls_tunnel_initiator_clear(unit, encap.tunnel_id);
    bcm_vlan_port_destroy(unit, ac.vlan_port_id);
    if (SHR_FUNC_ERR())
    {
        LOG_CLI((BSL_META("\n\n\n <<<  Test FAIL  >>>\n\n\n\n")));
    }
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_mpls_evpn_p2p_test_man = {
    .brief = "Semantic test for cross connect APIs where 1/2 of the sides is/are EVPN.",
    .full = "Semantic test for cross connect APIs where 1/2 of the sides is/are EVPN.\n"
        "1. Create P2P VLAN port\n"
        "2. Create EVPN encapsulation\n"
        "3. Create EVPN P2P Termination\n"
        "4. Cross-Connect VLAN->EVPN; Get and delete the connection.\n"
        "5. Cross-Connect EVPN->VLAN; Get and delete the connection.\n"
        "6. Cross-Connect EVPN->EVPN; Get and delete the connection.\n",
    .synopsis = "",
};

sh_sand_option_t dnx_mpls_evpn_p2p_test_options[] = {
    {NULL}
};

#define EVPN_LABEL_4_TEST       150
#define IML_RANGE_MIN_4_TEST    100
#define IML_RANGE_MAX_4_TEST    200
#define EVPN_VSI_4_TEST         10
#define NWK_GROUP_4_TEST        2

typedef enum
{
    EVPN_NWK_GROUP_TEST_TYPE_FIRST = 0,

    TEST_TYPE_INGRESS_UC = EVPN_NWK_GROUP_TEST_TYPE_FIRST,
    TEST_TYPE_INGRESS_IML,

    EVPN_NWK_GROUP_TEST_TYPE_EGRESS_FIRST,

    TEST_TYPE_EGRESS_UC = EVPN_NWK_GROUP_TEST_TYPE_EGRESS_FIRST,
    TEST_TYPE_EGRESS_IML,

    EVPN_NWK_GROUP_TEST_TYPE_NOF
} evpn_nwk_group_test_e;

static const char *evpn_nwk_group_test_names[EVPN_NWK_GROUP_TEST_TYPE_NOF] = {
    [TEST_TYPE_INGRESS_UC] = "Ingress - EVPN",
    [TEST_TYPE_INGRESS_IML] = "Ingress - IML",
    [TEST_TYPE_EGRESS_UC] = "Egress - EVPN",
    [TEST_TYPE_EGRESS_IML] = "Egress - IML",
};

static uint32
get_mpls_switch_flag_from_test_type(
    evpn_nwk_group_test_e test_type)
{
    if (test_type == TEST_TYPE_INGRESS_IML)
    {
        return BCM_MPLS_SWITCH_EVPN_IML;
    }
    else
    {
        return BCM_MPLS_SWITCH_NEXT_HEADER_L2;
    }
}

static shr_error_e
evpn_nwk_group_test_iml_range_set(
    int unit,
    int min,
    int max)
{
    bcm_mpls_range_action_t action;
    bcm_mpls_range_action_t_init(&action);
    action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
    return bcm_mpls_range_action_add(unit, min, max, &action);
}

static shr_error_e
evpn_nwk_group_test_lif_create_ingress(
    int unit,
    evpn_nwk_group_test_e test,
    bcm_gport_t * p_gport)
{
    bcm_mpls_tunnel_switch_t tunnel_switch;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, EVPN_VSI_4_TEST));

    if (test == TEST_TYPE_INGRESS_IML)
    {
        SHR_IF_ERR_EXIT(evpn_nwk_group_test_iml_range_set(unit, IML_RANGE_MIN_4_TEST, IML_RANGE_MAX_4_TEST));
    }

    bcm_mpls_tunnel_switch_t_init(&tunnel_switch);
    tunnel_switch.label = EVPN_LABEL_4_TEST;
    tunnel_switch.flags = get_mpls_switch_flag_from_test_type(test);
    tunnel_switch.action = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_switch.vpn = EVPN_VSI_4_TEST;
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &tunnel_switch));
    *p_gport = tunnel_switch.tunnel_id;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
evpn_nwk_group_test_lif_create_egress(
    int unit,
    evpn_nwk_group_test_e test,
    bcm_gport_t * p_gport)
{
    bcm_mpls_egress_label_t eg_label;
    uint32 flag = BCM_MPLS_EGRESS_LABEL_EVPN;

    SHR_FUNC_INIT_VARS(unit);

    if (test == TEST_TYPE_EGRESS_IML)
    {
        flag = BCM_MPLS_EGRESS_LABEL_IML;
    }

    bcm_mpls_egress_label_t_init(&eg_label);
    eg_label.label = EVPN_LABEL_4_TEST;
    eg_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | flag;
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label));

    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(*p_gport, eg_label.tunnel_id);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
evpn_nwk_group_test_lif_create(
    int unit,
    evpn_nwk_group_test_e test,
    bcm_gport_t * p_gport)
{
    SHR_FUNC_INIT_VARS(unit);

    if (test < EVPN_NWK_GROUP_TEST_TYPE_EGRESS_FIRST)
    {
        SHR_IF_ERR_EXIT(evpn_nwk_group_test_lif_create_ingress(unit, test, p_gport));
    }
    else
    {
        SHR_IF_ERR_EXIT(evpn_nwk_group_test_lif_create_egress(unit, test, p_gport));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
evpn_nwk_group_set_for_test(
    int unit,
    evpn_nwk_group_test_e test,
    bcm_gport_t gport)
{
    bcm_port_class_t port_class = bcmPortClassForwardEgress;

    SHR_FUNC_INIT_VARS(unit);

    if (test < EVPN_NWK_GROUP_TEST_TYPE_EGRESS_FIRST)
    {
        port_class = bcmPortClassForwardIngress;
    }
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport, port_class, NWK_GROUP_4_TEST));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
evpn_nwk_group_test_lif_update_ingress(
    int unit,
    evpn_nwk_group_test_e test,
    bcm_gport_t gport)
{
    bcm_mpls_tunnel_switch_t tunnel_switch;

    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_tunnel_switch_t_init(&tunnel_switch);
    tunnel_switch.label = EVPN_LABEL_4_TEST;
    tunnel_switch.flags = get_mpls_switch_flag_from_test_type(test);
    tunnel_switch.action = BCM_MPLS_SWITCH_ACTION_POP;

    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_get(unit, &tunnel_switch));

    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, EVPN_VSI_4_TEST + 1));

    tunnel_switch.vpn += 1;
    tunnel_switch.flags |= BCM_MPLS_SWITCH_WITH_ID | BCM_MPLS_SWITCH_REPLACE;

    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &tunnel_switch));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
evpn_nwk_group_test_lif_update_egress(
    int unit,
    evpn_nwk_group_test_e test,
    bcm_gport_t gport)
{
    bcm_mpls_egress_label_t eg_label;
    int label_count;
    bcm_l3_itf_t itf;

    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_egress_label_t_init(&eg_label);
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(itf, gport);
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_get(unit, itf, 1, &eg_label, &label_count));
    if (label_count < 1)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Could not find required egress label");
    }

    eg_label.label += 1;
    eg_label.flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
evpn_nwk_group_test_lif_update(
    int unit,
    evpn_nwk_group_test_e test,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);

    if (test < EVPN_NWK_GROUP_TEST_TYPE_EGRESS_FIRST)
    {
        SHR_IF_ERR_EXIT(evpn_nwk_group_test_lif_update_ingress(unit, test, gport));
    }
    else
    {
        SHR_IF_ERR_EXIT(evpn_nwk_group_test_lif_update_egress(unit, test, gport));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
evpn_nwk_group_validate(
    int unit,
    evpn_nwk_group_test_e test,
    bcm_gport_t gport)
{
    bcm_port_class_t port_class;
    uint32 nwk_group;

    SHR_FUNC_INIT_VARS(unit);

    port_class = bcmPortClassForwardEgress;
    if (test < EVPN_NWK_GROUP_TEST_TYPE_EGRESS_FIRST)
    {
        port_class = bcmPortClassForwardIngress;
    }
    SHR_IF_ERR_EXIT(bcm_port_class_get(unit, gport, port_class, &nwk_group));
    SHR_ASSERT_EQ(NWK_GROUP_4_TEST, nwk_group);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
evpn_nwk_group_test_clean_ingress(
    int unit,
    evpn_nwk_group_test_e test)
{
    bcm_mpls_tunnel_switch_t tunnel;

    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_tunnel_switch_t_init(&tunnel);
    tunnel.label = EVPN_LABEL_4_TEST;
    tunnel.flags = get_mpls_switch_flag_from_test_type(test);
    tunnel.action = BCM_MPLS_SWITCH_ACTION_POP;
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_delete(unit, &tunnel));

    if (test == TEST_TYPE_INGRESS_IML)
    {
        SHR_IF_ERR_EXIT(bcm_mpls_range_action_remove(unit, IML_RANGE_MIN_4_TEST, IML_RANGE_MAX_4_TEST));
    }

    SHR_IF_ERR_EXIT(bcm_vlan_destroy_all(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
evpn_nwk_group_test_clean(
    int unit,
    evpn_nwk_group_test_e test,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);

    if (test < EVPN_NWK_GROUP_TEST_TYPE_EGRESS_FIRST)
    {
        SHR_IF_ERR_EXIT(evpn_nwk_group_test_clean_ingress(unit, test));
    }
    else
    {
        bcm_l3_itf_t itf;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(itf, gport);
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_clear(unit, itf));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Test setting and getting nwk group for an EVPN LIF.
 * A delicate point is when a LIF is updated with REPLACE flag, since the nwk group is set via a different API.
 */
static shr_error_e
dnx_mpls_evpn_nwk_group_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    evpn_nwk_group_test_e test;
    bcm_gport_t gport = BCM_GPORT_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI(("Test assigning network group ID and retaining it after REPLACE\n"));
    for (test = EVPN_NWK_GROUP_TEST_TYPE_FIRST; test < EVPN_NWK_GROUP_TEST_TYPE_NOF; ++test)
    {
        LOG_CLI(("%s ... ", evpn_nwk_group_test_names[test]));
        SHR_IF_ERR_EXIT(evpn_nwk_group_test_lif_create(unit, test, &gport));
        SHR_IF_ERR_EXIT(evpn_nwk_group_set_for_test(unit, test, gport));
        SHR_IF_ERR_EXIT(evpn_nwk_group_test_lif_update(unit, test, gport));
        SHR_IF_ERR_EXIT(evpn_nwk_group_validate(unit, test, gport));
        SHR_IF_ERR_EXIT(evpn_nwk_group_test_clean(unit, test, gport));
        gport = BCM_GPORT_INVALID;
        LOG_CLI(("OK\n"));
    }
    LOG_CLI(("TEST PASSED\n"));

exit:
    if (gport != BCM_GPORT_INVALID)
    {
        evpn_nwk_group_test_clean(unit, test, gport);
    }
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_mpls_evpn_nwk_group_test_man = {
    .brief = "Semantic test for network group management in EVPN LIFs.",
    .full = "Semantic test for network group management in EVPN LIFs.\n",
    .synopsis = "",
};

sh_sand_option_t dnx_mpls_evpn_nwk_group_options[] = {
    {NULL}
};

typedef struct
{
    int count;
    uint32 switch_flags;

} labels_counter_info_t;

static shr_error_e
tunnel_initiator_counter_cb(
    int unit,
    int num_labels,
    bcm_mpls_egress_label_t * label_array,
    void *user_data)
{
    labels_counter_info_t *counter_info = (labels_counter_info_t *) user_data;
    counter_info->count++;
    return _SHR_E_NONE;
}

static shr_error_e
tunnel_switch_counter_cb(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    void *user_data)
{
    labels_counter_info_t *counter_info = (labels_counter_info_t *) user_data;
    if (counter_info->switch_flags & info->flags)
    {
        counter_info->count++;
    }
    return _SHR_E_NONE;
}

static shr_error_e
wipeout_test_assert_labels_count(
    int unit,
    sh_sand_control_t * sand_control,
    int expected_count)
{
    int uc, mc;
    int ingress, egress;

    bcm_mpls_tunnel_initiator_traverse_info_t traverse_info;
    labels_counter_info_t counter_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("MultiCast", mc);
    SH_SAND_GET_BOOL("UniCast", uc);
    SH_SAND_GET_BOOL("Term", ingress);
    SH_SAND_GET_BOOL("Encap", egress);

    LOG_CLI(("\t- Counting %s %s labels: ", mc ? "IML" : "EVPN", ingress ? "termination" : "encapsulation"));

    if (egress)
    {
        traverse_info.flags = (mc ? BCM_MPLS_EGRESS_LABEL_IML : 0) | (uc ? BCM_MPLS_EGRESS_LABEL_EVPN : 0);
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_traverse
                        (unit, &traverse_info, tunnel_initiator_counter_cb, (void *) &counter_info));
    }
    if (ingress)
    {
        counter_info.switch_flags = (mc ? BCM_MPLS_SWITCH_EVPN_IML : 0) | (uc ? BCM_MPLS_SWITCH_NEXT_HEADER_L2 : 0);
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_traverse(unit, tunnel_switch_counter_cb, (void *) &counter_info));
    }

    LOG_CLI(("Counted %d, Expected: %d\n", counter_info.count, expected_count));
    SHR_ASSERT_EQ(expected_count, counter_info.count);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
wipeout_test_labels_wipe(
    int unit,
    sh_sand_control_t * sand_control)
{
    int ingress, egress;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("Term", ingress);
    SH_SAND_GET_BOOL("Encap", egress);

    if (ingress)
    {
        LOG_CLI(("\t- Clearing ingress labels\n"));
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_delete_all(unit));
    }
    if (egress)
    {
        LOG_CLI(("\t- Clearing egress labels\n"));
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_clear_all(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
wipeout_test_label_create(
    int unit,
    sh_sand_control_t * sand_control,
    int label)
{
    int uc, mc;
    int ingress, egress;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("MultiCast", mc);
    SH_SAND_GET_BOOL("UniCast", uc);
    SH_SAND_GET_BOOL("Term", ingress);
    SH_SAND_GET_BOOL("Encap", egress);

    if (ingress)
    {
        bcm_mpls_tunnel_switch_t tunnel;
        bcm_mpls_tunnel_switch_t_init(&tunnel);
        tunnel.label = label + (mc ? CTEST_EVPN__IML_LABEL_RANGE_OFFSET : 0);
        tunnel.flags = (uc ? BCM_MPLS_SWITCH_NEXT_HEADER_L2 : 0) | (mc ? BCM_MPLS_SWITCH_EVPN_IML : 0);
        tunnel.action = BCM_MPLS_SWITCH_ACTION_POP;
        tunnel.vpn = EVPN_VSI_4_TEST;
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &tunnel));
    }
    if (egress)
    {
        bcm_mpls_egress_label_t eg_label;
        bcm_mpls_egress_label_t_init(&eg_label);
        eg_label.label = EVPN_LABEL_4_TEST;
        eg_label.flags =
            BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT |
            (uc ? BCM_MPLS_EGRESS_LABEL_EVPN : 0) | (mc ? BCM_MPLS_EGRESS_LABEL_IML : 0);
        SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_create(unit, 0, 1, &eg_label));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
wipeout_test_iml_range_set(
    int unit,
    int label_low,
    int label_high)
{
    bcm_mpls_range_action_t action = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    label_low += CTEST_EVPN__IML_LABEL_RANGE_OFFSET;
    label_high += CTEST_EVPN__IML_LABEL_RANGE_OFFSET;

    action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
    SHR_IF_ERR_EXIT(bcm_mpls_range_action_add(unit, label_low, label_high, &action));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
wipeout_test_iml_range_delete(
    int unit,
    int label_low,
    int label_high)
{
    SHR_FUNC_INIT_VARS(unit);

    label_low += CTEST_EVPN__IML_LABEL_RANGE_OFFSET;
    label_high += CTEST_EVPN__IML_LABEL_RANGE_OFFSET;

    SHR_IF_ERR_EXIT(bcm_mpls_range_action_remove(unit, label_low, label_high));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Test deleting all the EVPN LIFs using clear_all/delete_all APIs:
 * 1. Wipe all labels (and make sure there are none left).
 * 2. Try to wipe again (should pass).
 * 3. Create some labels.
 * 4. Make sure they were created.
 * 5. Delete them.
 * 6. Make sure they were deleted.
 */
static shr_error_e
dnx_mpls_evpn_wipeout_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int label;
    const int label_start = 1000;
    const int label_stop = 1200;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, EVPN_VSI_4_TEST));
    SHR_IF_ERR_EXIT(wipeout_test_iml_range_set(unit, label_start, label_stop));

    LOG_CLI((" * 1. Wipe all labels (and make sure there are none left).\n"));
    SHR_IF_ERR_EXIT(wipeout_test_labels_wipe(unit, sand_control));
    SHR_IF_ERR_EXIT(wipeout_test_assert_labels_count(unit, sand_control, 0));

    LOG_CLI((" * 2. Try to wipe again (should pass).\n"));
    SHR_IF_ERR_EXIT(wipeout_test_labels_wipe(unit, sand_control));

    LOG_CLI((" * 3. Create some labels.\n"));
    for (label = label_start; label < label_stop; ++label)
    {
        SHR_IF_ERR_EXIT(wipeout_test_label_create(unit, sand_control, label));
    }
    LOG_CLI((" * 4. Make sure they were created.\n"));
    SHR_IF_ERR_EXIT(wipeout_test_assert_labels_count(unit, sand_control, label_stop - label_start));

    LOG_CLI((" * 5. Delete them.\n"));
    SHR_IF_ERR_EXIT(wipeout_test_labels_wipe(unit, sand_control));

    LOG_CLI((" * 6. Make sure they were deleted.\n"));
    SHR_IF_ERR_EXIT(wipeout_test_assert_labels_count(unit, sand_control, 0));

    SHR_IF_ERR_EXIT(wipeout_test_iml_range_delete(unit, label_start, label_stop));
    SHR_IF_ERR_EXIT(bcm_vlan_destroy(unit, EVPN_VSI_4_TEST));

    LOG_CLI(("\n\t=====> PASS <=====\n\n"));

exit:
    if (SHR_FUNC_ERR())
    {
        bcm_mpls_tunnel_switch_delete_all(unit);
        bcm_mpls_tunnel_initiator_clear_all(unit);
        wipeout_test_iml_range_delete(unit, label_start, label_stop);
        bcm_vlan_destroy(unit, EVPN_VSI_4_TEST);
        LOG_CLI(("\n\t>>>  FAIL  <<<\n\n"));
    }
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_mpls_evpn_wipeout_test_man = {
    .brief = "Semantic test for clearing all EVPN LIFs.",
    .full = "Semantic test for clearing all EVPN LIFs.",
    .synopsis = "",
};

sh_sand_option_t dnx_mpls_evpn_wipeout_options[] = {
    {
     .keyword = "Encap",
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Test only tunnel initiators",
     .def = "False",
     }
    ,
    {
     .keyword = "Term",
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Test only tunnel terminators",
     .def = "False",
     }
    ,
    {
     .keyword = "UniCast",
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Test only UC APIs",
     .def = "False",
     }
    ,
    {
     .keyword = "MultiCast",
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Test only MC APIs",
     .def = "False",
     }
    ,
    {NULL}
};

sh_sand_invoke_t dnx_mpls_evpn_wipeout_invokes[] = {
    {"uc_term", "UniCast Term", CTEST_PRECOMMIT}
    ,
    {"mc_term", "MultiCast Term", CTEST_PRECOMMIT}
    ,
    {"uc_encap", "UniCast Encap", CTEST_PRECOMMIT}
    ,
    {"mc_encap", "MultiCast Encap", CTEST_PRECOMMIT}
    ,
    {NULL}
    ,
};

sh_sand_cmd_t dnx_mpls_evpn_tests[] = {
    {
     .keyword = "encap",
     .action = dnx_mpls_evpn_encap_test,
     .options = dnx_mpls_evpn_test_options,
     .man = &dnx_mpls_evpn_encap_test_man,
     .flags = CTEST_PRECOMMIT,
     .invokes = dnx_mpls_evpn_basic_test_invokes,
     }
    ,
    {
     .keyword = "termination",
     .action = dnx_mpls_evpn_term_test,
     .options = dnx_mpls_evpn_test_options,
     .man = &dnx_mpls_evpn_term_test_man,
     .flags = CTEST_PRECOMMIT,
     .invokes = dnx_mpls_evpn_basic_test_invokes,
     }
    ,
    {
     .keyword = "range",
     .action = dnx_mpls_evpn_range_test,
     .options = dnx_mpls_evpn_range_options,
     .man = &dnx_mpls_evpn_range_man,
     .flags = CTEST_PRECOMMIT,
     }
    ,
    {
     .keyword = "crossconnect",
     .action = dnx_mpls_evpn_p2p_test,
     .options = dnx_mpls_evpn_p2p_test_options,
     .man = &dnx_mpls_evpn_p2p_test_man,
     .flags = CTEST_PRECOMMIT,
     }
    ,
    {
     .keyword = "nwk_group",
     .action = dnx_mpls_evpn_nwk_group_test,
     .options = dnx_mpls_evpn_nwk_group_options,
     .man = &dnx_mpls_evpn_nwk_group_test_man,
     .flags = CTEST_PRECOMMIT,
     }
    ,
    {
     .keyword = "wipeout",
     .action = dnx_mpls_evpn_wipeout_test,
     .options = dnx_mpls_evpn_wipeout_options,
     .man = &dnx_mpls_evpn_wipeout_test_man,
     .invokes = dnx_mpls_evpn_wipeout_invokes,
     .flags = CTEST_PRECOMMIT,
     }
    ,
    {NULL}
};
