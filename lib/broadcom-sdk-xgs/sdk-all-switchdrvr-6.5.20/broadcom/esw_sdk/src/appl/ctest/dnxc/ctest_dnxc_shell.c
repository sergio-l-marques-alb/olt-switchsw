/**
 * \file  ctest_dnxc_shell.c
 *
 * Framework related ctests
 */

#include <shared/bsl.h>

/** sal */
#include <sal/appl/sal.h>

#include <shared/dbx/dbx_file.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>

#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*************
 * EXTERNS   *
 *************/

extern void format_long_integer(
    char *buf,
    uint32 *val,
    int nval);

static sh_sand_enum_t shell_input_enum_table[] = {
    {"New", 1},
    {"Old", 2},
    {"All", 3},
    {"Global", 4},
    {NULL}
};

/* *INDENT-OFF* */
static sh_sand_man_t shell_input_man = {
    .brief = "Test input options",
    .full = "Print values for all options, each option has different type. Try to assign value to see how it will be accepted",
    .examples = "ip4=10.0.0.1 type=global\n" "id=0x200 mac=00:11:22:33:44:55 id=45\n" "status_input 3"
};

static sh_sand_option_t shell_input_options[] = {
    {"name",      SAL_FIELD_TYPE_STR,     "Print the string used as input for this option",     ""},
    {"id",        SAL_FIELD_TYPE_UINT32,  "Print uint32 value used as input for this option",   "1", (void *) shell_input_enum_table, "1-100"},
    {"range",     SAL_FIELD_TYPE_UINT32,  "Print value range used as input for this option",    "1-10"},
    {"all",       SAL_FIELD_TYPE_BOOL,    "Print boolean value used as input for this option",  "no"},
    {"ip4",       SAL_FIELD_TYPE_IP4,     "Print input or default ipv4 address",                "1.2.3.4"},
    {"ip6",       SAL_FIELD_TYPE_IP6,     "Print input or default ipv6 address",                "abcd:02:03:04:ef01:06:07:08"},
    {"mac",       SAL_FIELD_TYPE_MAC,     "Print input or default mac address",                 "07:00:00:00:00:08"},
    {"dump",      SAL_FIELD_TYPE_ARRAY32, "Print uint32 array",                                 "0x123456789"},
    {"type",      SAL_FIELD_TYPE_ENUM,    "Print input or default enum",                        "global", (void *) shell_input_enum_table},
    /*
     * Status and state are for testing free variables, that replaced variable and variable2 see
     */
    {"status",    SAL_FIELD_TYPE_STR,     "String without option name assumed",                 "", NULL, NULL, SH_SAND_ARG_FREE},
    {"state",     SAL_FIELD_TYPE_UINT32,  "uint32 value without option name assumed",           "1", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};
/* *INDENT-ON */

static shr_error_e
shell_input_option_cb(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    if (!sal_strcasecmp(keyword, "test"))
    {
        if (type_p != NULL)
            *type_p = SAL_FIELD_TYPE_ENUM;
        if (id_p != NULL)
            *id_p = 1000;
        if (ext_ptr_p != NULL)
        {
            *ext_ptr_p = (void *) shell_input_enum_table;
        }
        return _SHR_E_NONE;
    }
    else
        return _SHR_E_NOT_FOUND;
}

static shr_error_e
shell_input_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n, *filename;
    int boolean;
    int value_int32;
    uint32 value_uint32, value_test = 0xFFFF;
    uint32 range_start, range_end;
    int enum_index;
    sal_ip_addr_t ip_addr_start, ip_addr_end;
    sal_mac_addr_t mac_addr_data, mac_addr_mask;
    sal_ip6_addr_t ip6_addr;
    sh_sand_arg_t *sand_arg;
    int test_is_present;
    uint32 *array_uint32;
    char *variable_n;
    uint32 variable2_uint32;
    char buffer[SH_SAND_MAX_ARRAY32_SIZE * 8 + 4];
    int is_present;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("framework test");
    PRT_INFO_ADD("We need to verify that info string of 127 characters nay be printed, so we need to make sure this line is indeed 128");
    PRT_INFO_ADD("Now this line is even longer and without separator. We need to verify that info string of 127 characters nay be printed, so we need to make sure this line is indeed 128");
    PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Value");

    SH_SAND_GET_STR("file", filename);

    SH_SAND_GET_BOOL("all", boolean);
    SH_SAND_GET_INT32("core", value_int32);
    SH_SAND_GET_STR("name", match_n);
    SH_SAND_IS_PRESENT("name", is_present);
    SH_SAND_GET_UINT32("id", value_uint32);
    SH_SAND_GET_UINT32_RANGE("range", range_start, range_end);
    SH_SAND_GET_ARRAY32("dump", array_uint32);
    SH_SAND_GET_IP4_RANGE("ip4", ip_addr_start, ip_addr_end);
    SH_SAND_GET_IP6("ip6", ip6_addr);
    SH_SAND_GET_MAC_MASKED("mac", mac_addr_data, mac_addr_mask);
    SH_SAND_GET_ENUM("type", enum_index);
    SH_SAND_GET_ENUM_DYN("test", value_test, test_is_present);
    SH_SAND_GET_STR("status", variable_n);
    SH_SAND_GET_UINT32("state", variable2_uint32);

    SH_SAND_GET_ITERATOR(sand_arg)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", SH_SAND_GET_NAME(sand_arg));
        PRT_CELL_SET("%s", sal_field_type_str(SH_SAND_GET_TYPE(sand_arg)));
        PRT_CELL_SET("%d", SH_SAND_ARG_UINT32_DATA(sand_arg));
    }

    if (test_is_present)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("test");
        PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_ENUM));
        PRT_CELL_SET("%d", value_test);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("file");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_STR));
    PRT_CELL_SET("%s", filename);
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("bool");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_BOOL));
    PRT_CELL_SET("%s", sh_sand_bool_str(boolean));
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("core");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_INT32));
    PRT_CELL_SET("%d(0x%08x)", value_int32, value_int32);
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("id");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_UINT32));
    PRT_CELL_SET("%d(0x%08x)", value_uint32, value_uint32);
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("range");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_UINT32));
    PRT_CELL_SET("Range:%d-%d", range_start, range_end);
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("array32");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_ARRAY32));
    format_long_integer(buffer, array_uint32, SH_SAND_MAX_ARRAY32_SIZE);
    PRT_CELL_SET("%s", buffer);
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("name");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_STR));
    PRT_CELL_SET("%s(%d)", match_n, is_present);
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("ip4");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_IP4));
    PRT_CELL_SET("%d.%d.%d.%d-%d.%d.%d.%d", (ip_addr_start >> 24) & 0xff, (ip_addr_start >> 16) & 0xff,
                 (ip_addr_start >> 8) & 0xff, ip_addr_start & 0xff,
                 (ip_addr_end >> 24) & 0xff, (ip_addr_end >> 16) & 0xff, (ip_addr_end >> 8) & 0xff, ip_addr_end & 0xff);
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("ip6");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_IP6));
    PRT_CELL_SET("%x:%x:%x:%x:%x:%x:%x:%x",
                 (((uint16) ip6_addr[0] << 8) | ip6_addr[1]), (((uint16) ip6_addr[2] << 8) | ip6_addr[3]),
                 (((uint16) ip6_addr[4] << 8) | ip6_addr[5]), (((uint16) ip6_addr[6] << 8) | ip6_addr[7]),
                 (((uint16) ip6_addr[8] << 8) | ip6_addr[9]), (((uint16) ip6_addr[10] << 8) | ip6_addr[11]),
                 (((uint16) ip6_addr[12] << 8) | ip6_addr[13]), (((uint16) ip6_addr[14] << 8) | ip6_addr[15]));
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("mac");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_MAC));
    PRT_CELL_SET("%02X:%02X:%02X:%02X:%02X:%02X,%02X:%02X:%02X:%02X:%02X:%02X",
                 mac_addr_data[0], mac_addr_data[1], mac_addr_data[2],
                 mac_addr_data[3], mac_addr_data[4], mac_addr_data[5],
                 mac_addr_mask[0], mac_addr_mask[1], mac_addr_mask[2],
                 mac_addr_mask[3], mac_addr_mask[4], mac_addr_mask[5]);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("enum");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_ENUM));
    PRT_CELL_SET("%s(%d)", SH_SAND_GET_ENUM_STR("type", enum_index), enum_index);
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("variable");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_STR));
    PRT_CELL_SET("%s", variable_n);
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("variable2");
    PRT_CELL_SET("%s", sal_field_type_str(SAL_FIELD_TYPE_UINT32));
    PRT_CELL_SET("%d(0x%08x)", variable2_uint32, variable2_uint32);
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t shell_resource_man = {
    .brief = "Test keywords list",
    .full = "Check different aspects of keywords, alphabet order, uniqueness off shortcut, ..."
};

static sh_sand_option_t shell_resource_options[] = {
    /*
     * Name 
     *//*
     * Type 
     *//*
     * Description 
     *//*
     * Default 
     */
    {"full", SAL_FIELD_TYPE_BOOL, "Add additional verifications, that should not fail precommit", "No"},
    {NULL}
    /** End of options list - must be last. */
};

static shr_error_e
shell_resource_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int i_key, k_key;

    SHR_FUNC_INIT_VARS(unit);

    for (i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {
        /*
         * Check that the 1st character is letter 
         */
        if (!isupper(sh_sand_keywords[i_key].keyword[0]))
        {
            LOG_CLI((BSL_META("First character of Keyword \"%s\" is not capital letter\n"),
                     sh_sand_keywords[i_key].keyword));
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        }
        /*
         * Check AlephBet order 
         */
        if (i_key != 0)
        {
            if (sal_strcasecmp(sh_sand_keywords[i_key].keyword, sh_sand_keywords[i_key - 1].keyword) < 0)
            {
                LOG_CLI((BSL_META("Keyword \"%s\" should be before \"%s\"\n"),
                         sh_sand_keywords[i_key].keyword, sh_sand_keywords[i_key - 1].keyword));
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
        }

        if ((!ISEMPTY(sh_sand_keywords[i_key].short_key)) &&
                                                        !(sh_sand_keywords[i_key].flags & SH_SAND_KEYWORD_ALLOW_DOUBLE))
        {
            for (k_key = 0; k_key < i_key; k_key++)
            {
                if (sal_strcasecmp(sh_sand_keywords[i_key].short_key, sh_sand_keywords[k_key].short_key) == 0)
                {
                    LOG_CLI((BSL_META("Shortcut for \"%s\" is the same as for \"%s\"\n"),
                             sh_sand_keywords[i_key].keyword, sh_sand_keywords[k_key].keyword));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
            }
        }
        else
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "No shortcut for \"%s\"\n"), sh_sand_keywords[i_key].keyword));
        }
        if (sh_sand_keywords[i_key].count == 0)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Keyword \"%s\" not used\n"), sh_sand_keywords[i_key].keyword));
        }
        if (!(sh_sand_keywords[i_key].flags & SH_SAND_KEYWORD_SINGLE_CHARACTER_SHORTCUT) &&
                                                                (sal_strlen(sh_sand_keywords[i_key].short_key) == 1))
        {
            LOG_CLI((BSL_META("Shortcut:\"%s\" for keyword:\"%s\" is 1 character only\n"),
                     sh_sand_keywords[i_key].short_key, sh_sand_keywords[i_key].keyword));
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t plural_prt_man = {
    .brief = "Test PRT facilities",
    .full = "Test various combinations of PRT MACROS"
};

static shr_error_e
plural_prt_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS_PL(2);
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET_PL(0, "Plural Table 1");
    PRT_TITLE_SET_PL(1, "Plural Table 1");

    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD_PL(0, "Column 1");
    PRT_COLUMN_ADD_PL(0, "Column 2");
    PRT_COLUMN_ADD_PL(1, "Column 1");
    PRT_COLUMN_ADD_PL(1, "Column 2");

    PRT_ROW_ADD_PL(0, PRT_ROW_SEP_NONE);

    PRT_ROW_ADD_PL(1, PRT_ROW_SEP_NONE);

    PRT_CELL_SET_PL(0, "%s", "Table 0 Column 1");
    PRT_CELL_SET_PL(1, "%s", "Table 1 Column 1");
    PRT_CELL_SET_PL(0, "%s", "Table 0 Column 2");
    PRT_CELL_SET_PL(1, "%s", "Table 1 Column 2");

    PRT_COMMITX_PL(0);
    PRT_COMMITX_PL(1);
exit:
    PRT_FREE_PL(0);
    PRT_FREE_PL(1);
    SHR_FUNC_EXIT;
}

static shr_error_e
combine_prt_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS_PL(3);
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET_PL(0, "Plural Table 0");
    PRT_TITLE_SET_PL(1, "Plural Table 1");
    PRT_TITLE_SET_PL(2, "Plural Table 2");

    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD_PL(0, "Column 1");
    PRT_COLUMN_ADD_PL(0, "Column 2");
    PRT_COLUMN_ADD_PL(0, "Column 3");
    PRT_COLUMN_ADD_PL(1, "Column 1");
    PRT_COLUMN_ADD_PL(1, "Column 2");
    PRT_COLUMN_ADD_PL(2, "Column 1");
    PRT_COLUMN_ADD_PL(2, "Column 2");
    PRT_COLUMN_ADD_PL(2, "Column 3");
    PRT_COLUMN_ADD_PL(2, "Column 4");

    PRT_ROW_ADD_PL(0, PRT_ROW_SEP_NONE);

    PRT_ROW_ADD_PL(1, PRT_ROW_SEP_NONE);

    PRT_ROW_ADD_PL(2, PRT_ROW_SEP_NONE);

    PRT_CELL_SET_PL(0, "%s", "Table 0 Column 1");
    PRT_CELL_SET_PL(1, "%s", "Table 1 Column 1");
    PRT_CELL_SET_PL(0, "%s", "Table 0 Column 222222222222");
    PRT_CELL_SET_PL(1, "%s", "Table 1 Column 2");
    PRT_CELL_SET_PL(0, "%s", "Table 0 Column 3");

    PRT_CELL_SET_PL(2, "%s", "Table 2 Column 1111");
    PRT_CELL_SET_PL(2, "%s", "Table 2 Column 2");
    PRT_CELL_SET_PL(2, "%s", "Table 2 Column 333");
    PRT_CELL_SET_PL(2, "%s", "Table 2 Column 4");

    PRT_ROW_ADD_PL(0, PRT_ROW_SEP_NONE);

    PRT_ROW_ADD_PL(1, PRT_ROW_SEP_NONE);

    PRT_CELL_SET_PL(0, "%s", "Table 0 Column 1");
    PRT_CELL_SET_PL(1, "%s", "Table 1 Column 1");
    PRT_CELL_SET_PL(0, "%s", "Table 0 Column 2");
    PRT_CELL_SET_PL(1, "%s", "Table 1 Column 2");
    PRT_CELL_SET_PL(0, "%s", "Table 0 Column 3");

    PRT_COMMITX_COMBINE_PL(3);
exit:
    PRT_FREE_PL(0);
    PRT_FREE_PL(1);
    PRT_FREE_PL(2);
    SHR_FUNC_EXIT;
}

static sh_sand_man_t info_prt_man = {
    .brief = "Pure Info Table",
};

static shr_error_e
info_prt_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("INFO");
    PRT_INFO_ADD("Long Line created only to test table without columns");
    PRT_INFO_ADD("Second even longer line created only to test table without columns and without rows");

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
static sh_sand_cmd_t sh_sand_prt_test_cmds[] = {
    {"plural", plural_prt_cmd, NULL, NULL, &plural_prt_man, NULL, NULL, CTEST_PRECOMMIT},
    {"combine", combine_prt_cmd, NULL, NULL, &plural_prt_man, NULL, NULL, CTEST_PRECOMMIT},
    {"info", info_prt_cmd, NULL, NULL, &info_prt_man, NULL, NULL, CTEST_PRECOMMIT},
    {NULL}
};
sh_sand_invoke_t dnxc_input_tests[] = {
    {"first", "name=example",    CTEST_PRECOMMIT},
    {"ipv4",  "ip4=10.11.12.13", CTEST_PRECOMMIT},
    {"free_variables",  "status_string 3", CTEST_PRECOMMIT},
    {"range", "range=100-300",   CTEST_PRECOMMIT | SH_CMD_SKIP_TEST_WB},
    {NULL}
};

static sh_sand_man_t shell_screen_man = {
    .brief = "Test shell command usage per root level command",
    .full = "Tests brief and full description, arguments, enum values for arguments and so on."
             "Prints report on all errors found",
    .examples = "access\n sig"
};
/* *INDENT-ON* */

static shr_error_e
shell_test_list_create(
    int unit,
    rhlist_t * test_list)
{
    sh_sand_cmd_t *sh_sand_cmd;
    sh_sand_cmd_t *sh_sand_root_a, *sh_sand_device_a;
    int flags = 0;
    char test_arguments[SH_SAND_MAX_TOKEN_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_root_get(unit, &sh_sand_root_a), "");
    sh_sand_device_a = sh_sand_root_a->child_cmd_a;

    SH_SAND_CMD_ITERATOR_COND(sh_sand_cmd, sh_sand_device_a, flags)
    {
        sal_snprintf(test_arguments, SH_SAND_MAX_TOKEN_SIZE - 1, "module=%s", sh_sand_cmd->keyword);
        SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, sh_sand_cmd->keyword, test_arguments,
                                             sh_sand_cmd->flags), "Add shell test for:%s failed\n",
                            sh_sand_cmd->keyword);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
shell_screen_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *subcmd_name;

    char command[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("module", subcmd_name);
    if (ISEMPTY(subcmd_name))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Command requires argument - name of shell command to be tested. See examples\n");
    }

    sal_snprintf(command, RHSTRING_MAX_SIZE - 1, "%s utest", subcmd_name);

    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, command), "");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t shell_exec_man = {
    "Test all shell commands",
    "Traverse shell command tree and run all commands and their invocation options",
};

static sh_sand_option_t shell_test_arguments[] = {
    {"module", SAL_FIELD_TYPE_STR, "Root level command name from shell command tree", "", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static shr_error_e
shell_exec_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *subcmd_name;

    char command[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("module", subcmd_name);
    if (ISEMPTY(subcmd_name))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Command requires argument - name of shell command to be executed. See examples\n");
    }

    sal_snprintf(command, RHSTRING_MAX_SIZE - 1, "%s utest", subcmd_name);

    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, command), "");

    sal_snprintf(command, RHSTRING_MAX_SIZE - 1, "%s exec all lg qt", subcmd_name);

    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, command), "");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t shell_write_man = {
    "Test all shell commands",
    "Traverse shell command tree, run all command's examples and save tabular output into XML files",
};

static shr_error_e
shell_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *subcmd_name;

    char command[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("module", subcmd_name);
    if (ISEMPTY(subcmd_name))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Command requires argument - name of shell command to be executed. See examples\n");
    }

    sal_snprintf(command, RHSTRING_MAX_SIZE - 1, "%s exec write all lg qt", subcmd_name);

    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, command), "");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
shell_verify_test_add(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *flex_ptr,
    int flags)
{
    rhlist_t *test_list = (rhlist_t *) flex_ptr;
    char test_arguments[SH_SAND_MAX_TOKEN_SIZE];
    char test_name[SH_SAND_MAX_TOKEN_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Skip commands marked by SH_CMD_NO_XML_VERIFY flag
     */
    if (sh_sand_cmd->flags & SH_CMD_NO_XML_VERIFY)
    {
        SHR_EXIT();
    }
    /*
     * Each command contain space separated command line, append entire command chain (spaces will be turned to _ below
     */
    sal_strncpy(test_name, sh_sand_cmd->cmd_only, SH_SAND_MAX_TOKEN_SIZE - 1);
    /*
     * Replace all non-compliant characters in the test name by underscore
     */
    utilex_str_escape(test_name, '_');
    /*
     * Make it all lower case to ease the view
     */
    utilex_str_to_lower(test_name);

    sal_snprintf(test_arguments, SH_SAND_MAX_TOKEN_SIZE - 1, "lg command=\"%s\"", sh_sand_cmd->cmd_only);
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, test_name, test_arguments,
                                         sh_sand_cmd->flags), "Add shell test for:%s failed\n", test_name);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
shell_verify_list_create(
    int unit,
    rhlist_t * test_list)
{
    sh_sand_cmd_t *sh_sand_root_a, *sh_sand_device_a;

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_root_get(unit, &sh_sand_root_a), "");
    /*
     * Take first child which should be device specific shell command root
     */
    sh_sand_device_a = sh_sand_root_a->child_cmd_a;

    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse(unit, NULL, sh_sand_device_a, shell_verify_test_add, (void *) test_list, 0, NULL   /* label 
                                                                                                                                 */ ), "");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t shell_verify_man = {
    "Verify XML output for shell commands",
    "Traverse shell command tree, run all command's examples, save tabular output into XML files and compare to gold",
};

static sh_sand_option_t shell_verify_arguments[] = {
    {"command", SAL_FIELD_TYPE_STR, "Shell command from any level", "", NULL, NULL, SH_SAND_ARG_FREE},
    {"logger", SAL_FIELD_TYPE_BOOL, "Log all commands execution as it happens", "No"},
    {NULL}
};

static shr_error_e
shell_verify_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int log_flag;
    char *command;
    char full_command[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("command", command);
    if (ISEMPTY(command))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Command requires argument - name of shell command to be executed. See examples\n");
    }

    sal_snprintf(full_command, RHSTRING_MAX_SIZE - 1, "%s exec write verify all qt", command);
    SH_SAND_GET_BOOL("logger", log_flag);
    if (log_flag == TRUE)
    {
        sal_strncat(full_command, " lg", RHSTRING_MAX_SIZE - 1 - sal_strlen(full_command));
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, full_command), "");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t shell_cint_man = {
    .brief = "Test cint invocation utilities for ctest"
};

static shr_error_e
shell_cint_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/dbal/cint_dbal.c"), "CINT Load Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "dbal_access_example", NULL, 0), "CINT Run Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_reset(unit), "CINT Reset Failed\n");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
shell_cint_ext_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    ctest_cint_argument_t cint_arguments[2];

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "sand/utility/cint_sand_utils_global.c"),
                        "CINT cint_sand_utils_global Load Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "sand/utility/cint_sand_utils_l3.c"),
                        "CINT cint_sand_utils_l3 Load Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/l3/cint_snake_test.c"), "CINT cint_snake_test Load Failed\n");

    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_cmd("bcm_port_t ports[1];"), "cint cmd failed");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_cmd("ports[0] = 0;"), "cint cmd failed");

    /** array of ports */
    cint_arguments[0].type = SAL_FIELD_TYPE_STR;
    cint_arguments[0].value.value_str_ptr = "ports";
    /** nof ports */
    cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[1].value.value_int32 = 1;

       /** array of ports */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "cint_l3_snake", cint_arguments, 2), "CINT Run Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_reset(unit), "CINT Reset Failed\n");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t test_property_man = {
    .brief = "Test soc property setting API"
};

static shr_error_e
test_property_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    rhhandle_t ctest_soc_set_h = NULL;

    ctest_soc_property_t ctest_soc_property[] = {
        {"trunk_group_max_members", "32"},
        {"bist_enable", "0"},
        {"tm_port_header*", NULL},
        {NULL}
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

exit:
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

static sh_sand_man_t shell_regress_man = {
    "Perform all tests for a single shell command.",
};

static shr_error_e
shell_regress_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *subcmd_name;
    char command[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("module", subcmd_name);
    if (ISEMPTY(subcmd_name))
    {
        SHR_EXIT();
    }

    sal_snprintf(command, RHSTRING_MAX_SIZE - 1, "%s utest", subcmd_name);
    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, command), "");

    sal_snprintf(command, RHSTRING_MAX_SIZE - 1, "%s exec all qt", subcmd_name);
    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, command), "");

    sal_snprintf(command, RHSTRING_MAX_SIZE - 1, "%s exec write all qt", subcmd_name);
    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, command), "");

    sal_snprintf(command, RHSTRING_MAX_SIZE - 1, "%s exec write verify all qt", subcmd_name);
    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, command), "");

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_cmd_t dnxc_framework_test_cmds[] = {
    {"input",       shell_input_cmd,           NULL,   shell_input_options,    &shell_input_man,           shell_input_option_cb, dnxc_input_tests, CTEST_PRECOMMIT},
    {"resource",    shell_resource_cmd,        NULL,   shell_resource_options, &shell_resource_man,        NULL,                  NULL,             CTEST_PRECOMMIT | SH_CMD_SKIP_TEST_WB},
    {"print",       NULL,                      sh_sand_prt_test_cmds},
    {"usagetest",   shell_screen_cmd,          NULL,   shell_test_arguments,   &shell_screen_man,          NULL,                  NULL,             CTEST_POSTCOMMIT, shell_test_list_create},
    {"shell",       shell_exec_cmd,            NULL,   shell_test_arguments,   &shell_exec_man,            NULL,                  NULL,             CTEST_PASS, shell_test_list_create},
    {"write",       shell_write_cmd,           NULL,   shell_test_arguments,   &shell_write_man,           NULL,                  NULL,             CTEST_POSTCOMMIT, shell_test_list_create},
    {"verify",      shell_verify_cmd,          NULL,   shell_verify_arguments, &shell_verify_man,          NULL,                  NULL,             CTEST_POSTCOMMIT, shell_verify_list_create},
    {"regress",     shell_regress_cmd,         NULL,   shell_test_arguments,   &shell_regress_man},
    /*
     * Callback at the end is for command eligibility and not for test creation, which is marks using SH_CMD_CONDITIONAL flag
     */
    {"cint",     shell_cint_cmd,     NULL, NULL,                   &shell_cint_man,     NULL,                  NULL,             CTEST_PRECOMMIT | SH_CMD_CONDITIONAL | SH_CMD_ROLLBACK, sh_cmd_is_dnx},
    {"cint_ext", shell_cint_ext_cmd, NULL, NULL,                   &shell_cint_man,     NULL,                  NULL,             CTEST_PRECOMMIT | SH_CMD_CONDITIONAL | SH_CMD_ROLLBACK, sh_cmd_is_dnx},
    {"property", test_property_cmd,  NULL, NULL,                   &test_property_man,  NULL,                  NULL,             CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
