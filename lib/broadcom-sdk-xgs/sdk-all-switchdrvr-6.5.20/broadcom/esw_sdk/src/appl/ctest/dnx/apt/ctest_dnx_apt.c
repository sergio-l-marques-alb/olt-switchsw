/*
 * ! \file ctest_dnx_apt.c
 * Contains all of the API performance infrastructure ctest functions.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL
#include <shared/bsl.h>

/*
 * Include files.
 * {
 */

#include <appl/ctest/dnxc/ctest_dnxc_system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "ctest_dnx_apt.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define DNX_APT_MAX_NUMBER_OF_CALLS 100000
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

/*
 * GLOBALs
 * {
 */

/* *INDENT-OFF* */
extern const dnx_apt_object_def_t *dnx_apt_objects[];

static sh_sand_option_t dnx_apt_test_options[] = {
    {"Name", SAL_FIELD_TYPE_STR,
            "API performance test name\t"
            "optional:\t"
            "        NaMe=\"\"    - run all tests valid for ctest regression (default)\t"
            "        NaMe=\"all\" - run all tests",
        /** default value */ ""},
    {"runs", SAL_FIELD_TYPE_INT32, "API performance test number of runs", "0"},
    {"calls", SAL_FIELD_TYPE_INT32, "API performance test number of API calls", "0"},
    {"validation_method", SAL_FIELD_TYPE_INT32,
            "API performance test validation method:\t"
            "       -1 - from test (default)\t"
            "        0 - worst run\t"
            "        1 - best run\t"
            "        2 - average all runs\t"
            "        3 - stable average\t"
            "        4 - at least half runs pass\t"
            "        5 - skip validation and print average",
        /** default value */ "-1"},
    {"DeBuG", SAL_FIELD_TYPE_BOOL, "Enable debug output during test execution", "FALSE"},
    {NULL}
};

static sh_sand_man_t dnx_apt_test_man = {
    "API performance test.",
    "Executes, measures and validates the performance of the tested API",
    "NaMe=<test_name> RUNs=<number_of_runs> calls=<number_of_calls> validation_method=<-1:5> DeBuG=<TRUE/FALSE>",
    "NaMe=L3_route_IPv4_LPM_insert"
};

static sh_sand_option_t dnx_apt_dump_options[] = {
    {"All", SAL_FIELD_TYPE_BOOL, "List all API performance tests", "FALSE"},
    {NULL}
};

static sh_sand_man_t dnx_apt_dump_man = {
    "API performance tests dump.",
    "Lists all API performance tests and their description.",
    "",
    ""
};

static sh_sand_option_t dnx_apt_validate_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_apt_validate_man = {
    "API performance tests validation.",
    "Validates all API performance tests are encountered only once, do not have the same description and the same combination of flags and functions.",
    "",
    ""
};

/* *INDENT-ON* */

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/*
 * API performance tests validation command.
 * Validates each test have unique name and description.
 * Validates that the combination of test flags and functions is unique.
 * Executed in pre-commit.
 */
static shr_error_e
dnx_apt_validate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int obj_index = 0;
    int number_of_objects = dnx_apt_number_of_objects();

    SHR_FUNC_INIT_VARS(unit);

    for (obj_index = 0; obj_index < number_of_objects - 1; obj_index++)
    {
        int search_index;

        for (search_index = obj_index + 1; search_index < number_of_objects; search_index++)
        {
            if (!sal_strncasecmp(dnx_apt_objects[obj_index]->test_name,
                                 dnx_apt_objects[search_index]->test_name, DNX_APT_MAX_NAME_LENGTH))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Test \"%s\" encountered more than once.\n",
                             dnx_apt_objects[obj_index]->test_name);
            }
            if (!sal_strncasecmp(dnx_apt_objects[obj_index]->test_description,
                                 dnx_apt_objects[search_index]->test_description, DNX_APT_MAX_DESCRIPTION_LENGTH))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Tests \"%s\" and \"%s\" have the same description \"%s\"\n",
                             dnx_apt_objects[obj_index]->test_name, dnx_apt_objects[search_index]->test_name,
                             dnx_apt_objects[obj_index]->test_description);
            }
            if ((dnx_apt_objects[obj_index]->support_flags == dnx_apt_objects[search_index]->support_flags) &&
                (dnx_apt_objects[obj_index]->ctest_flags == dnx_apt_objects[search_index]->ctest_flags) &&
                (dnx_apt_objects[obj_index]->time_threshold == dnx_apt_objects[search_index]->time_threshold) &&
                (dnx_apt_objects[obj_index]->custom_support_check ==
                 dnx_apt_objects[search_index]->custom_support_check)
                && (dnx_apt_objects[obj_index]->init == dnx_apt_objects[search_index]->init)
                && (dnx_apt_objects[obj_index]->pre_execute == dnx_apt_objects[search_index]->pre_execute)
                && (dnx_apt_objects[obj_index]->execute == dnx_apt_objects[search_index]->execute)
                && (dnx_apt_objects[obj_index]->post_execute == dnx_apt_objects[search_index]->post_execute)
                && (dnx_apt_objects[obj_index]->deinit == dnx_apt_objects[search_index]->deinit))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Tests \"%s\" and \"%s\" have identical flags and functions.\n",
                             dnx_apt_objects[obj_index]->test_name, dnx_apt_objects[search_index]->test_name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Searches for a test using test name string.
 * Returns the test object index when found.
 * Returns error _SHR_E_NOT_FOUND when not found.
 */
static shr_error_e
dnx_apt_object_find(
    int unit,
    char *test_name,
    int *index)
{
    int obj_index = 0;
    int number_of_objects = dnx_apt_number_of_objects();

    SHR_FUNC_INIT_VARS(unit);

    for (obj_index = 0; obj_index < number_of_objects; obj_index++)
    {
        if (!sal_strncasecmp(test_name, dnx_apt_objects[obj_index]->test_name, DNX_APT_MAX_NAME_LENGTH))
        {
            *index = obj_index;
            break;
        }
    }

    if (obj_index == number_of_objects)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Test \"%s\" does not exist.\n", test_name);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Execute API performance tests.
 * If test name is provided and the test exists, it will execute a single test (even if not supported).
 * If "all" is provided as a test name, all tests will be executed.
 * If no test name is provided, all tests that are not skipped in ctest regression will be executed.
 */
static shr_error_e
dnx_apt_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_name;
    int number_of_runs = 0;
    int number_of_calls = 0;
    int validation_method = -1;
    uint8 enable_debug_output = FALSE;
    int number_of_objects = dnx_apt_number_of_objects();
    int obj_index = 0;
    int iter_start = 0;
    int iter_end = number_of_objects;
    uint8 execute_single = FALSE;
    uint8 execute_all = FALSE;
    bsl_severity_t severity;

    dnx_apt_object_def_t object = { {0} };

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);

    /** Get input parameters */
    SH_SAND_GET_STR("Name", test_name);
    SH_SAND_GET_INT32("runs", number_of_runs);
    SH_SAND_GET_INT32("calls", number_of_calls);
    SH_SAND_GET_INT32("validation_method", validation_method);
    SH_SAND_GET_BOOL("debug", enable_debug_output);

    /** Validate input number of calls */
    if (number_of_calls < 0 || number_of_calls > DNX_APT_MAX_NUMBER_OF_CALLS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Input number of calls should be between 0 (default) and %d.\n",
                     DNX_APT_MAX_NUMBER_OF_CALLS);
    }

    /** Validate input validation method */
    if ((validation_method != DNX_APT_VALIDATION_METHOD_DEFAULT) &&
        ((validation_method < DNX_APT_VALIDATION_METHOD_FIRST)
         || (validation_method >= DNX_APT_VALIDATION_METHOD_LAST)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported validation method %d.\n", validation_method);
    }

    /** Enable debug output */
    if (enable_debug_output)
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, bslSeverityDebug);
    }

    if (!ISEMPTY(test_name))
    {
        if (!sal_strncasecmp(test_name, "all", DNX_APT_MAX_NAME_LENGTH))
        {
            /** Execute all tests */
            execute_all = TRUE;
        }
        else
        {
            /** Execute a single test */

            /** Find the test object */
            SHR_IF_ERR_EXIT(dnx_apt_object_find(unit, test_name, &obj_index));

            /** Update the start and end iteration indexes in order to loop only once */
            iter_start = obj_index;
            iter_end = iter_start + 1;
            execute_single = TRUE;
        }
    }

    for (obj_index = iter_start; obj_index < iter_end; obj_index++)
    {
        if (!execute_single)
        {
            shr_error_e rv;
            bsl_severity_t severity;
            /** Disable logging in order to hide messages for the reason a test is not supported */
            SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, bslSeverityError);
            rv = dnx_apt_test_support_check(unit, dnx_apt_objects[obj_index]);
            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);
            if (rv != _SHR_E_NONE)
            {
                /** Skip unsupported tests */
                continue;
            }
        }
        if (!execute_all && (dnx_apt_objects[obj_index]->ctest_flags & SH_CMD_SKIP_EXEC))
        {
            /** Skip tests that are not run in ctest regression */
            continue;
        }

        /** Copy the test object to a local one in order to modify it */
        sal_memcpy(&object, dnx_apt_objects[obj_index], sizeof(dnx_apt_object_def_t));

        /** Set validation method */
        object.validation_method =
            (validation_method != DNX_APT_VALIDATION_METHOD_DEFAULT) ? validation_method : object.validation_method;

        /** Set number of runs and/or calls if they are not 0 */
        object.number_of_runs = number_of_runs ? number_of_runs : object.number_of_runs;
        object.number_of_calls = number_of_calls ? number_of_calls : object.number_of_calls;

        /** Run the test */
        SHR_SET_CURRENT_ERR(dnx_apt_engine_test_run(unit, &object));
    }

exit:
    /** Disable debug output if enabled */
    if (enable_debug_output)
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);
    }
    SHR_FUNC_EXIT;
}

/*
 * Create tests dynamically.
 * Tests are created only if the compilation is supported (Intel-GTS-OPT)
 * or if the test is supported on the current device configuration.
 */
static shr_error_e
dnx_apt_test_dyncamic_tests_creation_cmd(
    int unit,
    rhlist_t * test_list)
{
    int obj_index = 0;
    int number_of_objects = dnx_apt_number_of_objects();
    bsl_severity_t severity = bslSeverityError;

    SHR_FUNC_INIT_VARS(unit);

    /** Create dynamic test list only if compilation is supported (Intel-GTS-OPT) */
    if (!dnx_apt_is_compilation_supported())
    {
        SHR_EXIT();
    }

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);

    for (obj_index = 0; obj_index < number_of_objects; obj_index++)
    {
        uint32 ctest_flags = dnx_apt_objects[obj_index]->ctest_flags;
        char *test_name = (char *) dnx_apt_objects[obj_index]->test_name;
        char test_args[DNX_APT_MAX_NAME_LENGTH + 5] = "Name=";
        shr_error_e rv;

        /** Disable logging in order to hide messages for the reason a test is not supported */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, bslSeverityError);
        rv = dnx_apt_test_support_check(unit, dnx_apt_objects[obj_index]);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);

        if (rv != _SHR_E_NONE)
        {
            /** The test is supported on the current device configuration. */
            continue;
        }

        sal_strncat(test_args, dnx_apt_objects[obj_index]->test_name, (DNX_APT_MAX_NAME_LENGTH + 4));

        SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, test_name, test_args, ctest_flags), "Add test failed");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Lists all available supported tests.
 * When "all" is provided, lists also the unsupported tests.
 */
static shr_error_e
dnx_apt_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int iter = 0;
    int number_of_objects = dnx_apt_number_of_objects();
    uint8 dump_all = FALSE;
    bsl_severity_t severity;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("All", dump_all);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);

    PRT_TITLE_SET("API performance tests");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Validation method");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Time threshold");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Number of runs / calls");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTEST");
    if (dump_all)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Current configuration status");
    }

    for (iter = 0; iter < number_of_objects; iter++)
    {
        shr_error_e rv;

        /** Disable logging in order to hide messages for the reason a test is not supported */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, bslSeverityError);
        rv = dnx_apt_test_support_check(unit, dnx_apt_objects[iter]);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);

        if (!dump_all && (rv != _SHR_E_NONE))
        {
            continue;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", dnx_apt_objects[iter]->test_name);
        PRT_CELL_SET("%s", dnx_apt_objects[iter]->test_description);
        PRT_CELL_SET("%s", dnx_apt_validation_method_string(dnx_apt_objects[iter]->validation_method));
        if (dnx_apt_objects[iter]->optional_threshold_info)
        {
            /** Print optional threshold info */
            char optional_info[DNX_APT_MAX_DESCRIPTION_LENGTH] = { 0 };
            SHR_IF_ERR_EXIT(dnx_apt_objects[iter]->optional_threshold_info(unit, optional_info));
            PRT_CELL_SET("%s", optional_info);
        }
        else
        {
            /** Print threshold */
            PRT_CELL_SET("%.3fus", dnx_apt_objects[iter]->time_threshold(unit));
        }
        PRT_CELL_SET("%d / %d", dnx_apt_objects[iter]->number_of_runs, dnx_apt_objects[iter]->number_of_calls);
        PRT_CELL_SET("%s%s%s", (dnx_apt_objects[iter]->ctest_flags & CTEST_PRECOMMIT ? "PRECOMMIT " : ""),
                     (dnx_apt_objects[iter]->ctest_flags & CTEST_POSTCOMMIT ? "POSTCOMMIT " : ""),
                     (dnx_apt_objects[iter]->ctest_flags & SH_CMD_SKIP_EXEC ? "SKIPPED " : ""));
        if (dump_all)
        {
            PRT_CELL_SET("%s", (rv == _SHR_E_NONE ? "Supported" :
                                (rv == _SHR_E_UNAVAIL ? "Not Supported" : "Error validating")));
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_cmd_t dnx_apt_cmds[] = {
    {"TeST", dnx_apt_test_cmd, NULL, dnx_apt_test_options, &dnx_apt_test_man, NULL, NULL, CTEST_PASS, dnx_apt_test_dyncamic_tests_creation_cmd},
    {"DuMP", dnx_apt_dump_cmd, NULL, dnx_apt_dump_options, &dnx_apt_dump_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {"Validate", dnx_apt_validate_cmd, NULL, dnx_apt_validate_options, &dnx_apt_validate_man, NULL, NULL, CTEST_PRECOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/*
 * }
 */
