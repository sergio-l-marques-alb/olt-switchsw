/**
 * \file ctest_dnx_mpls_tunnel.c
 *
 * Tests for MPLS TUNNEL:
 *   * FRR
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLSTESTSDNX

#include <bcm/mpls.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>

/*
 * 'traverse' tests
 * {
 */
/*
 * DEFINEs related to 'traverse' tests.
 * {
 */
/*
 * Size of 'user_data' that is passed on to the callback procedure.
 * User data is loaded by increasing numbers starting from '0'.
 */
#define NUM_BYTES_USER_DATA         10
/*
 * Number of 'mpls labels' which are loaded into the DBAL_TABLE_MPLS_FRR_TCAM_DB table
 * before calling 'traverse' to delete all of them.
 */
#define NUM_ENTRIES_FOR_TRAVERSE    20
/*
 * First label loaded into the the DBAL_TABLE_MPLS_FRR_TCAM_DB table
 * before calling 'traverse' to delete all of them. Label identifiers are
 * increased cobsequently so the last label loaded is
 * 'INITIAL_LABEL_FOR_TRAVERSE + NUM_ENTRIES_FOR_TRAVERSE - 1'
 */
#define INITIAL_LABEL_FOR_TRAVERSE  16
/*
 * }
 */
/**
 * \brief Callback for MPLS TUNNEL_FRR traverse. Invoked per cycle.
 * \see bcm_mpls_special_label_identifier_traverse_cb
 */
int
dnx_mpls_tunnel_frr_delete_callback(
    int unit,
    bcm_mpls_special_label_type_t * label_type_p,
    bcm_mpls_special_label_t * label_info_p,
    void *user_data_p)
{
    char *user_data_chars_p;
    unsigned int ii;

    SHR_FUNC_INIT_VARS(unit);
    user_data_chars_p = (char *) user_data_p;
    /*
     * We expect consequent numbers from 0 on.
     */
    for (ii = 0; ii < NUM_BYTES_USER_DATA; ii++)
    {
        if (user_data_chars_p[ii] != (char) ii)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "No match on received user data on index %d: received %d expected %d. Quit.\r\n",
                         ii, (unsigned int) (user_data_chars_p[ii]), ii);
        }
    }
    LOG_INFO_EX(BSL_LOG_MODULE,
                "%s(), line %d, Callback: Received user_data is as expected. %s %s\r\n",
                __FUNCTION__, __LINE__, EMPTY, EMPTY);
    LOG_INFO_EX(BSL_LOG_MODULE,
                "%s(), line %d, Callback: Going to delete label %d (label_type %d)\r\n",
                __FUNCTION__, __LINE__, label_info_p->label_value, *label_type_p);
    SHR_IF_ERR_EXIT(bcm_dnx_mpls_special_label_identifier_delete(unit, *label_type_p, *label_info_p));
    LOG_INFO_EX(BSL_LOG_MODULE,
                "%s(), line %d, Callback: Exit without error %s %s\r\n", __FUNCTION__, __LINE__, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief Test for MPLS TUNNEL_FRR traverse.
 * For detailed description of the test, see the SDD.
 */
static shr_error_e
dnx_mpls_tunnel_frr_traverse_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 label, count;
    char user_data[NUM_BYTES_USER_DATA];
    unsigned int ii;
    bcm_mpls_special_label_type_t label_type;
    bcm_mpls_special_label_t label_info;

    SHR_FUNC_INIT_VARS(unit);
    for (ii = 0; ii < sizeof(user_data); ii++)
    {
        user_data[ii] = (char) ii;
    }
    count = NUM_ENTRIES_FOR_TRAVERSE;
    label = INITIAL_LABEL_FOR_TRAVERSE;
    SHR_IF_ERR_EXIT(dnx_bcm_mpls_special_label_t_init(unit, &label_info));
    label_type = bcmMplsSpecialLabelTypeFrr;
    LOG_INFO_EX(BSL_LOG_MODULE,
                "%s(), line %d, Going to add labels: from %d to %d (incl.)\r\n",
                __FUNCTION__, __LINE__, label, (label + count - 1));
    for (ii = 0; ii < count; ii++)
    {
        label_info.label_value = (bcm_mpls_label_t) (label + ii);
        SHR_IF_ERR_EXIT(bcm_dnx_mpls_special_label_identifier_add(unit, label_type, label_info));
    }
    LOG_INFO_EX(BSL_LOG_MODULE,
                "%s(), line %d, Now call 'traverse' to delete all added entries. %s %s\r\n",
                __FUNCTION__, __LINE__, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(bcm_dnx_mpls_special_label_identifier_traverse
                    (unit, dnx_mpls_tunnel_frr_delete_callback, &user_data));
    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, All was OK. Exit without error. %s %s\r\n", __FUNCTION__, __LINE__,
                EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   List of tests for 'action' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_mpls_tunnel_frr_traverse_tests[] = {
    {"1", "", CTEST_PRECOMMIT}
    ,
    {NULL}
};
/*
 * 'traverse' test manual
 */
static sh_sand_man_t Dnx_mpls_tunnel_frr_traverse_test_man = {
    /** Brief */
    "Semantic test of MPLS TUNNEL FRR traverse API",
    /** Full */
    "Semantic test of MPLS TUNNEL FRR traverse API\n"
        "  This test loads a fixed number of entries into the DBAL_TABLE_MPLS_FRR_TCAM_DB\r\n"
        "  table and then calls the 'traverse' BCM API with a callback that deletes all\r\n"
        "  entries from the table.\r\n",
    /** Synopsis */
    "",
    /** Examples */
    ""
};
/*
 * }
 */
/*
 * 'add' tests
 * {
 */

/**
 * \brief
 *   This function is the basic 'add' testing  application.
 *
 * \param [in] unit -
 *    HW identifier of unit.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
dnx_mpls_tunnel_frr_add_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 label, count;
    uint32 ii;
    int clean_resources, negative_tests;
    bcm_mpls_special_label_type_t label_type;
    bcm_mpls_special_label_t label_info;
    bcm_mpls_special_label_t label_info_get;
    shr_error_e shr_error;
    /**
     * This variables stores the severity of MPLS
     */
    bsl_severity_t original_severity_mpls;

    SHR_FUNC_INIT_VARS(unit);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_mpls);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_UINT32("label", label);
    SH_SAND_GET_UINT32("count", count);
    SH_SAND_GET_BOOL("clean", clean_resources);
    SH_SAND_GET_BOOL("neg", negative_tests);
    if (count == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Input 'count' may not be zero. Quit.\r\n");
    }
    if (!negative_tests)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Carry out positive tests (expected to succeed) %s %s\r\n",
                    __FUNCTION__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_bcm_mpls_special_label_t_init(unit, &label_info));
        label_type = bcmMplsSpecialLabelTypeFrr;
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Going to add labels: from %d to %d (incl.)\r\n",
                    __FUNCTION__, __LINE__, label, (label + count - 1));
        for (ii = 0; ii < count; ii++)
        {
            label_info.label_value = (bcm_mpls_label_t) (label + ii);
            SHR_IF_ERR_EXIT(bcm_dnx_mpls_special_label_identifier_add(unit, label_type, label_info));
            SHR_IF_ERR_EXIT(dnx_bcm_mpls_special_label_t_init(unit, &label_info_get));
            label_info_get.label_value = (bcm_mpls_label_t) (label + ii);
            shr_error = bcm_dnx_mpls_special_label_identifier_get(unit, label_type, &label_info_get);
            if (shr_error != _SHR_E_NONE)
            {
                if (shr_error == _SHR_E_NOT_FOUND)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Label %d was added but was not found immediately after. Quit.\r\n",
                                 label_info_get.label_value);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Procedure %s has failed with error (%s) when searching for label %d. Quit.\r\n",
                                 "bcm_dnx_mpls_special_label_identifier_get",
                                 _SHR_ERRMSG(shr_error), label_info_get.label_value);
                }
            }
        }
        if (clean_resources)
        {
            SHR_IF_ERR_EXIT(dnx_bcm_mpls_special_label_t_init(unit, &label_info));
            for (ii = 0; ii < count; ii++)
            {
                label_info.label_value = (bcm_mpls_label_t) (label + ii);
                SHR_IF_ERR_EXIT(bcm_dnx_mpls_special_label_identifier_delete(unit, label_type, label_info));
            }
        }
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Exit procedure without error. %s %s\r\n", __FUNCTION__, __LINE__, EMPTY, EMPTY);
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Carry out negative tests (expected to succeed) %s %s\r\n",
                    __FUNCTION__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_bcm_mpls_special_label_t_init(unit, &label_info));
        label_type = bcmMplsSpecialLabelTypeFrr;
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Going to try to delete labels from an empty table: from %d to %d (incl.)\r\n",
                    __FUNCTION__, __LINE__, label, (label + count - 1));
        for (ii = 0; ii < count; ii++)
        {
            label_info.label_value = (bcm_mpls_label_t) (label + ii);
            /**
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            shr_error = bcm_dnx_mpls_special_label_identifier_delete(unit, label_type, label_info);
            /**
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_mpls);
            if (shr_error != _SHR_E_NONE)
            {
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "%s(), line %d, Label %d cound NOT be deleted, as expected. All is OK! %s\r\n",
                            __FUNCTION__, __LINE__, label_info.label_value, EMPTY);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Label %d was NOT added but was successfully 'deleted'. Quit.\r\n",
                             label_info.label_value);
            }
        }
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Going to add labels: from %d to %d (incl.)\r\n",
                    __FUNCTION__, __LINE__, label, (label + count - 1));
        for (ii = 0; ii < count; ii++)
        {
            label_info.label_value = (bcm_mpls_label_t) (label + ii);
            SHR_IF_ERR_EXIT(bcm_dnx_mpls_special_label_identifier_add(unit, label_type, label_info));
            SHR_IF_ERR_EXIT(dnx_bcm_mpls_special_label_t_init(unit, &label_info_get));
            label_info_get.label_value = (bcm_mpls_label_t) (label + ii + count);
            /**
             * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
            shr_error = bcm_dnx_mpls_special_label_identifier_get(unit, label_type, &label_info_get);
            /**
             *  Restore the original severity after the end of Negative test.
             */
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_mpls);
            if (shr_error != _SHR_E_NONE)
            {
                if (shr_error == _SHR_E_NOT_FOUND)
                {
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "%s(), line %d, Label %d was NOT found, as expected. All is OK! %s\r\n",
                                __FUNCTION__, __LINE__, label_info_get.label_value, EMPTY);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Procedure %s has failed with unexpected error (%s) when searching for label %d. Quit.\r\n",
                                 "bcm_dnx_mpls_special_label_identifier_get",
                                 _SHR_ERRMSG(shr_error), label_info.label_value);
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Label %d was NOT added but was successfully found. Quit.\r\n",
                             label_info_get.label_value);
            }
        }
        {
            SHR_IF_ERR_EXIT(dnx_bcm_mpls_special_label_t_init(unit, &label_info));
            for (ii = count; ii < (2 * count); ii++)
            {
                label_info.label_value = (bcm_mpls_label_t) (label + ii);
                /**
                 * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, bslSeverityFatal);
                shr_error = bcm_dnx_mpls_special_label_identifier_delete(unit, label_type, label_info);
                /**
                 *  Restore the original severity after the end of Negative test.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_mpls);
                if (shr_error != _SHR_E_NONE)
                {
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "%s(), line %d, Label %d cound NOT be deleted, as expected. All is OK! %s\r\n",
                                __FUNCTION__, __LINE__, label_info.label_value, EMPTY);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Label %d was NOT added but was successfully 'deleted'. Quit.\r\n",
                                 label_info.label_value);
                }
            }
        }
        if (clean_resources)
        {
            SHR_IF_ERR_EXIT(dnx_bcm_mpls_special_label_t_init(unit, &label_info));
            for (ii = 0; ii < count; ii++)
            {
                label_info.label_value = (bcm_mpls_label_t) (label + ii);
                SHR_IF_ERR_EXIT(bcm_dnx_mpls_special_label_identifier_delete(unit, label_type, label_info));
            }
        }
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Exit procedure without error. %s %s\r\n", __FUNCTION__, __LINE__, EMPTY, EMPTY);
    }
exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_MPLS, original_severity_mpls);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */

/**
 * \brief
 *   Options list for 'frr add' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
static sh_sand_option_t Sh_dnx_mpls_tunnel_frr_add_options[] = {
    /* Name */     /* Type */           /* Description */                               /* Default */   /* 'ENUM' table */
    {"label", SAL_FIELD_TYPE_UINT32, "Value of pecial label to add to table",             "0x10",           NULL},
    {"count", SAL_FIELD_TYPE_UINT32, "Number of consequent labels to be added",           "1",              NULL},
    {"clean", SAL_FIELD_TYPE_BOOL,   "Will test perform HW and SW-state clean-up or not", "Yes",            NULL},
    {"neg",   SAL_FIELD_TYPE_BOOL,   "Will test execute negative tests or not",           "No",             NULL},
    /*
     * End of options list - must be last. 
     */
    {NULL}
};

/* *INDENT-ON* */

/**
 * \brief
 *   List of tests for 'action' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_mpls_tunnel_frr_add_tests[] = {
    {"1", "label=0x50 count=6", CTEST_PRECOMMIT}
    ,
    {"2", "label=0x50 count=6 neg", CTEST_PRECOMMIT}
    ,
    {NULL}
};
/*
 * 'add' test manual
 */
static sh_sand_man_t Dnx_mpls_tunnel_frr_add_test_man = {
    /** Brief */
    "Semantic test of MPLS TUNNEL FRR add/get/delete API",
    /** Full */
    "Semantic test of MPLS TUNNEL_FRR add/get/delete API\n"
        "  'label' indicates the special FRR MPLS label to be added to the MPLS_FRR_TCAM_DB table\n"
        "  'count' indicates the number of consequent labels to be added to the MPLS_FRR_TCAM_DB table\n"
        "  'clean' indicates whether to delete entries added to the MPLS_FRR_TCAM_DB table, during this test\n"
        "  'neg'   indicates request to carry out negative tests (which are supposed to fail)\n",
    /** Synopsis */
    "label=<special label> count=<number of labels to add> clean=<yes | no> neg=<yes | no>",
    /** Examples */
    "label=0x80 count=3"
};
/*
 * }
 */
static sh_sand_cmd_t dnx_mpls_tunnel_frr_tests[] = {
    {
     .keyword = "traverse",
     .action = dnx_mpls_tunnel_frr_traverse_test,
     .options = NULL,
     .man = &Dnx_mpls_tunnel_frr_traverse_test_man,
     .invokes = Sh_mpls_tunnel_frr_traverse_tests,
     .flags = CTEST_PRECOMMIT},
    {
     .keyword = "add",
     .action = dnx_mpls_tunnel_frr_add_test,
     .options = Sh_dnx_mpls_tunnel_frr_add_options,
     .man = &Dnx_mpls_tunnel_frr_add_test_man,
     .invokes = Sh_mpls_tunnel_frr_add_tests,
     .flags = CTEST_PRECOMMIT},

    {NULL}
};

/** Test manual   */
static sh_sand_man_t Dnx_mpls_tunnel_frr_test_man = {
    /** Brief */
    "Semantic test of MPLS TUNNEL FRR related APIs",
    /** Full */
    "Add, delete, get, traverse and verify input validation with proper errors " "for TUNNEL FRR management APIs.",
    /** Synopsis   */
    ""
};

/** List of mpls tunnel tests   */
sh_sand_cmd_t dnx_mpls_tunnel_tests[] = {
    {"frr", NULL, dnx_mpls_tunnel_frr_tests, NULL, &Dnx_mpls_tunnel_frr_test_man}
    ,
    {NULL}
};
