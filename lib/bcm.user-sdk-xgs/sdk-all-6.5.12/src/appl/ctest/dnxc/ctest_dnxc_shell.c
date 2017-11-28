/*
 * $Id: diag_sand_access.c,v 1.20 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        diag_sand_access.c
 * Purpose:     Diag shell direct access commands
 */

#include <shared/bsl.h>

#include <sal/appl/sal.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

static sh_sand_enum_t shell_input_enum_table[] = {
    {"New", 1},
    {"Old", 2},
    {"All", 3},
    {"Global", 4},
    {NULL}
};

static sh_sand_man_t shell_input_man = {
    "Test input options",
    "Print values for all options, each option has different type. Try to assign value to see how it will be accepted",
    "ACCess test [option=<value>] ... \n",
    "acc test ip4=10.0.0.1 bool=yes\n" "acc test id=0x200 mac=00:11:22:33:44:55",
};

static sh_sand_option_t shell_input_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Print the string used as input for this option", ""},
    {"id", SAL_FIELD_TYPE_UINT32, "Print uint32 value used as input for this option", "1",
     (void *) shell_input_enum_table},
    {"range", SAL_FIELD_TYPE_UINT32, "Print value range used as input for this option", "1-10"},
    {"all", SAL_FIELD_TYPE_BOOL, "Print boolean value used as input for this option", "no"},
    {"ip4", SAL_FIELD_TYPE_IP4, "Print input or default ipv4 address", "1.2.3.4"},
    {"ip6", SAL_FIELD_TYPE_IP6, "Print input or default ipv6 address", "abcd:02:03:04:ef01:06:07:08"},
    {"mac", SAL_FIELD_TYPE_MAC, "Print input or default mac address", "07:00:00:00:00:08"},
    {"dump", SAL_FIELD_TYPE_ARRAY32, "Print uint32 array", "0x123456789"},
    {"type", SAL_FIELD_TYPE_ENUM, "Print input or default enum", "global", (void *) shell_input_enum_table},
    {NULL}
};

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
    char buffer[SH_SAND_MAX_ARRAY32_SIZE * 8 + 4];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("framework test");
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
    SH_SAND_GET_UINT32("id", value_uint32);
    SH_SAND_GET_UINT32_RANGE("range", range_start, range_end);
    SH_SAND_GET_ARRAY32("dump", array_uint32);
    SH_SAND_GET_IP4_RANGE("ip4", ip_addr_start, ip_addr_end);
    SH_SAND_GET_IP6("ip6", ip6_addr);
    SH_SAND_GET_MAC_MASKED("mac", mac_addr_data, mac_addr_mask);
    SH_SAND_GET_ENUM("type", enum_index);
    SH_SAND_GET_ENUM_DYN("test", value_test, test_is_present);

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
    PRT_CELL_SET("%s", match_n);
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
    PRT_CELL_SET("%d", enum_index);
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t shell_resource_man = {
    "Test keywords list",
    "Check different aspects of keywords, alphabet order, uniqueness off shortcut, ...",
    "",
    "",
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

        if (!ISEMPTY(sh_sand_keywords[i_key].short_key))
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
    }

    SHR_FUNC_EXIT;
}

static sh_sand_man_t plural_prt_man = {
    "Test PRT facilites",
    "Test various combinations of PRT MACROS",
    "",
    "",
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

static sh_sand_cmd_t sh_sand_prt_test_cmds[] = {
    {"plural", plural_prt_cmd, NULL, NULL, &plural_prt_man, NULL, NULL, CTEST_PRECOMMIT},
    {NULL}
};

sh_sand_invoke_t dnxc_input_tests[] = {
    {"first", "name=example", CTEST_PRECOMMIT},
    {"ipv4", "ip4=10.11.12.13", CTEST_PRECOMMIT},
    {NULL}
};

/* *INDENT-OFF* */
sh_sand_cmd_t dnxc_framework_test_cmds[] = {
    {"input",    shell_input_cmd,    NULL, shell_input_options, &shell_input_man,    shell_input_option_cb, dnxc_input_tests, CTEST_PRECOMMIT},
    {"resource", shell_resource_cmd, NULL, NULL,                &shell_resource_man, NULL,                  NULL, CTEST_PRECOMMIT},
    {"print",      NULL,               sh_sand_prt_test_cmds},
    {NULL}
};
