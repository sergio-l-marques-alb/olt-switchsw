/**
 * \file  ctest_dnxc_shell.c
 *
 * Framework related ctests
 */

#include <shared/bsl.h>

/** sal */
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <sal/appl/field_types.h>

#include <shared/dbx/dbx_file.h>

#include <soc/sand/sand_aux_access.h>

#include <appl/diag/sand/diag_sand_framework.h>

#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*************
 * EXTERNS   *
 *************/
extern int test_dnxc_init_test(
    int unit,
    args_t * a,
    void *p);
extern int test_dnxc_init_test_init(
    int unit,
    args_t * a,
    void **p);
extern int test_dnxc_init_test_done(
    int unit,
    void *p);

shr_error_e
ctest_dnxc_init_deinit(
    int unit,
    char *command)
{
    args_t *init_args = NULL;
    SHR_FUNC_INIT_VARS(unit);

    if ((init_args = sal_alloc(sizeof(args_t), "leafs")) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
        SHR_EXIT();
    }

    if (diag_parse_args(command, NULL, init_args))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        SHR_EXIT();
    }

    test_dnxc_init_test_init(unit, init_args, NULL);
    test_dnxc_init_test(unit, init_args, NULL);
    test_dnxc_init_test_done(unit, NULL);
exit:
    if (init_args != NULL)
    {
        sal_free(init_args);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnxc_add_property(
    int unit,
    rhlist_t * ctest_soc_property_list,
    char *property_name,
    char *property_value)
{
    rhhandle_t void_arg;
    ctest_soc_entry_t *property_entry;
    char *cur_value;

    SHR_FUNC_INIT_VARS(unit);

    cur_value = sal_config_get(property_name);
    /** We are not interested if current and to be set values are equal */
    if ((ISEMPTY(cur_value) && ISEMPTY(property_value)) ||
        ((cur_value != NULL) && (property_value != NULL) && !sal_strcasecmp(cur_value, property_value)))
    {
        SHR_EXIT();
    }

    if (utilex_rhlist_entry_add_tail(ctest_soc_property_list, property_name,
                                     RHID_TO_BE_GENERATED, &void_arg) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Cannot add soc property:%s to list", property_name);
    }
    property_entry = (ctest_soc_entry_t *) void_arg;
    /** cur_value may be NULL in case soc property was not set before */
    if (cur_value != NULL)
    {
        property_entry->value = sal_strdup(cur_value);
    }
    if (sal_config_set(RHNAME(property_entry), property_value) != 0)
    {
        SHR_CLI_EXIT(_SHR_E_CONFIG, "Failed to set property:%s to %s\n", property_name, property_value);
    }
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Set property %s=%s\n"), property_name, property_value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnxc_set_soc_properties(
    int unit,
    ctest_soc_property_t * ctest_soc_property,
    rhhandle_t * ctest_soc_property_set_h)
{
    char **tokens = NULL;
    uint32 realtokens = 0;
    rhlist_t *ctest_soc_property_list = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ctest_soc_property_set_h, _SHR_E_PARAM, "ctest_soc_property_set_h");
    SHR_NULL_CHECK(ctest_soc_property, _SHR_E_PARAM, "ctest_soc_property");

    if ((ctest_soc_property_list = utilex_rhlist_create("SocPropertySet", sizeof(ctest_soc_entry_t), 1)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for soc property entries list\n");
    }

    for (; ctest_soc_property != NULL && !ISEMPTY(ctest_soc_property->property); ctest_soc_property++)
    {
        /*
         * If value is NULL, we need to delete the property,
         * if property has '*' we'll search for all properties matching substring before the '*'
         */
        if (ctest_soc_property->value == NULL)
        {
            if ((tokens = utilex_str_split(ctest_soc_property->property, "*", 2, &realtokens)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory to parse %s\n", ctest_soc_property->property);
            }
            if (realtokens == 2)
            {
                char *property_name = NULL;
                char *property_value;
                if (ISEMPTY(tokens[0]))
                {
                    SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - '*' is first character in:%s. It will delete all properties\n",
                                 ctest_soc_property->property);
                }
                for (;;)
                {
                    if (sal_config_get_next(&property_name, &property_value) < 0)
                    {
                        break;
                    }
                    if ((sal_strstr(property_name, tokens[0]) != NULL))
                    {
                        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_add_property(unit, ctest_soc_property_list,
                                                                    property_name, NULL), "");
                        property_name = NULL;
                    }
                }
            }
            else
            {   /** Delete single property */
                SHR_CLI_EXIT_IF_ERR(ctest_dnxc_add_property(unit, ctest_soc_property_list,
                                                            ctest_soc_property->property, ctest_soc_property->value),
                                    "");
            }
        }
        else
        {   /** Add/Modify single soc property */
            SHR_CLI_EXIT_IF_ERR(ctest_dnxc_add_property(unit, ctest_soc_property_list,
                                                        ctest_soc_property->property, ctest_soc_property->value), "");
        }
    }

    if (RHLNUM(ctest_soc_property_list) == 0)
    {
        utilex_rhlist_free_all(ctest_soc_property_list);
        ctest_soc_property_list = NULL;
    }
    else
    {

        /*
         * Enable the init verify and turn off the multithread init.
         */
        if (sal_config_set("custom_feature_init_verify", "1") != 0)
        {
            SHR_CLI_EXIT(_SHR_E_CONFIG, "Failed to set property:custom_feature_init_verify to 1\n");
        }
        if (sal_config_set("custom_feature_multithread_en", "0") != 0)
        {
            SHR_CLI_EXIT(_SHR_E_CONFIG, "Failed to set property:custom_feature_multithread_en to 0\n");
        }

        ctest_dnxc_init_deinit(unit, NULL);
    }

    *ctest_soc_property_set_h = ctest_soc_property_list;
exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnxc_restore_soc_properties(
    int unit,
    rhhandle_t ctest_soc_property_set_h)
{
    ctest_soc_entry_t *property_entry;

    SHR_FUNC_INIT_VARS(unit);

    if (ctest_soc_property_set_h == NULL)
    {   /** Quietly exit - nothing to do */
        SHR_EXIT();
    }
    RHITERATOR(property_entry, ctest_soc_property_set_h)
    {
        if (sal_config_set(RHNAME(property_entry), property_entry->value) != 0)
        {
            SHR_CLI_EXIT(_SHR_E_CONFIG, "Failed to restore property:%s to %s\n",
                         RHNAME(property_entry), property_entry->value);
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Set property %s=%s\n"), RHNAME(property_entry),
                                     property_entry->value));
        if (property_entry->value != NULL)
        {
            sal_free(property_entry->value);
        }
    }

    ctest_dnxc_init_deinit(unit, NULL);

    utilex_rhlist_free_all(ctest_soc_property_set_h);
exit:
    SHR_FUNC_EXIT;
}

#if defined(INCLUDE_CINT)
extern int cint_interpreter_ctest_cmd(
    char *command);
#endif

shr_error_e
ctest_dnxc_cint_cmd(
    char *command)
{
#if defined(INCLUDE_CINT)
    return cint_interpreter_ctest_cmd(command);
#else
    LOG_CLI((BSL_META_U(0, "ERROR - Not supported without INCLUDE_CINT\n")));
    return _SHR_E_DISABLED;
#endif
}

shr_error_e
ctest_dnxc_cint_load(
    int unit,
    char *cint_name)
{
    char sdk_location[RHFILE_MAX_SIZE];
    char load_command[RHFILE_MAX_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First check may be the file exists as is
     */
    if (!dbx_file_exists(cint_name))
    {
        /*
         * No - fetch SDK location and try again
         */
        dbx_file_find_sdk_path(sdk_location);
        sal_strncat(sdk_location, "/", RHFILE_MAX_SIZE - 1 - strlen(sdk_location));

        sal_strncat(sdk_location, "src/examples/", RHFILE_MAX_SIZE - 1 - strlen(sdk_location));
        sal_strncat(sdk_location, cint_name, RHFILE_MAX_SIZE - 1 - strlen(sdk_location));
        if (!dbx_file_exists(sdk_location))
        {
            LOG_CLI((BSL_META_U(unit, "CINT File not found in 1:'%s'\n"), cint_name));
            LOG_CLI((BSL_META_U(unit, "                       2:'%s'\n"), sdk_location));

            dbx_file_find_sdk_path(sdk_location);
            sal_strncat(sdk_location, "/../../src/examples/", RHFILE_MAX_SIZE - 1 - strlen(sdk_location));
            sal_strncat(sdk_location, cint_name, RHFILE_MAX_SIZE - 1 - strlen(sdk_location));
            if (!dbx_file_exists(sdk_location))
            {
                LOG_CLI((BSL_META_U(unit, "                       3:'%s'\n"), sdk_location));
                dbx_file_find_sdk_path(sdk_location);
                sal_strncat(sdk_location, "/examples/", RHFILE_MAX_SIZE - 1 - strlen(sdk_location));
                sal_strncat(sdk_location, cint_name, RHFILE_MAX_SIZE - 1 - strlen(sdk_location));
                if (!dbx_file_exists(sdk_location))
                {
                    SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "                       4:'%s'\n", sdk_location);
                }
            }
            else
            {

            }
        }
    }
    else
    {
        sal_strncpy(sdk_location, cint_name, RHFILE_MAX_SIZE - 1);
    }

    sal_snprintf(load_command, RHFILE_MAX_SIZE - 1, "cint %s", sdk_location);

    LOG_CLI((BSL_META_U(unit, "Loading:'%s'\n"), sdk_location));

    if (sh_process_command(unit, load_command) != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR - Failed to enable api mode\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see ctest_dnxc_utils.h
 */
shr_error_e
ctest_dnxc_cint_run(
    int unit,
    char *main_name,
    ctest_cint_argument_t * cint_arguments,
    int nof_arguments)
{
    char cint_str[RHSTRING_MAX_SIZE];
    int i_arg;
    int str_len;

    SHR_FUNC_INIT_VARS(unit);

    sal_snprintf(cint_str, RHSTRING_MAX_SIZE - 1, "%s(%d", main_name, unit);
    for (i_arg = 0; i_arg < nof_arguments; i_arg++)
    {
        str_len = sal_strlen(cint_str);
        switch (cint_arguments[i_arg].type)
        {
            case SAL_FIELD_TYPE_INT32:
                sal_snprintf(cint_str + str_len, RHSTRING_MAX_SIZE - 1 - str_len, ",%d",
                             cint_arguments[i_arg].value.value_int32);
                break;
            case SAL_FIELD_TYPE_UINT32:
                sal_snprintf(cint_str + str_len, RHSTRING_MAX_SIZE - 1 - str_len, ",%u",
                             cint_arguments[i_arg].value.value_uint32);
                break;
            case SAL_FIELD_TYPE_STR:
                sal_snprintf(cint_str + str_len, RHSTRING_MAX_SIZE - 1 - str_len, ",%s",
                             cint_arguments[i_arg].value.value_str_ptr);
                break;
            default:
                SHR_CLI_EXIT(_SHR_E_PARAM, "Unsupported argument type:%s\n",
                             sal_field_type_str(cint_arguments[i_arg].type));
                break;
        }
    }
    str_len = sal_strlen(cint_str);

    sal_snprintf(cint_str + str_len, RHSTRING_MAX_SIZE - 1 - str_len, ");");

    LOG_CLI((BSL_META_U(unit, "Running:'%s'\n"), cint_str));

    /*
     * When CINT is not compiled in - there is no sense to run these tests - so to minimize compilation problems
     * cint_interpreter call is just compiled out
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_cmd(cint_str), "ERROR - Cint command:'%s' Failed\n", cint_str);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnxc_cint_reset(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(INCLUDE_CINT)
    if (cint_interpreter_ctest_cmd("cint_reset();") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR - Failed to reset cint\n");
    }
#else
    SHR_CLI_EXIT(_SHR_E_PARAM, "ERROR - Not supported without cint\n");
#endif /* INCLUDE_CINT */

exit:
    SHR_FUNC_EXIT;
}
