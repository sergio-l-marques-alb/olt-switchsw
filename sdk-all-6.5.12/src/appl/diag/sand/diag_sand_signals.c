/*
 * $Id: diag_sand_dsig.c,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    diag_sand_dsig.c
 * Purpose:    Routines for handling debug and internal signals
 */

#include <soc/drv.h>

#include <sal/appl/sal.h>

#include <shared/bitop.h>
#include <shared/utilex/utilex_str.h>

#include <appl/diag/sand/diag_sand_dsig.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_signals.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>

#include <soc/sand/sand_signals.h>

#include <bcm/types.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#endif

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

static sh_sand_man_t signal_get_man = {
    "List of signals with values",
    "List of signals filtered and shown using misc. filtering and showing options",
    "signal [name=<string>]\n",
    "signal name=TM_Cmd",
};

static sh_sand_enum_t order_enum_table[] = {
    {"big", PRINT_BIG_ENDIAN},
    {"little", PRINT_LITTLE_ENDIAN},
    {NULL}
};

static sh_sand_option_t signal_get_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Signal name or its substring", ""},
    {"block", SAL_FIELD_TYPE_STR, "Show only signals from this block", ""},
    {"stage", SAL_FIELD_TYPE_STR, "Show only signals going to or coming from this stage", ""},
    {"to", SAL_FIELD_TYPE_STR, "Show only signals going to this stage", ""},
    {"from", SAL_FIELD_TYPE_STR, "Show only signals coming from this stage", ""},
    {"show", SAL_FIELD_TYPE_STR, "Misc. options to control filtering/output", ""},
    {"order", SAL_FIELD_TYPE_ENUM, "Print values in certain endian order <little/bug>", "big",
     (void *) order_enum_table},
    {"all", SAL_FIELD_TYPE_BOOL, "Prints all signals ignoring other filtering input", "No"},
#ifdef ADAPTER_SERVER_MODE
    {"tx", SAL_FIELD_TYPE_BOOL,
     "Sends predefined packet before fetching signals\n Used to have signal list before any packet was sent", "No"},
#endif
    {NULL}
};

static sh_sand_invoke_t signal_get_invokes[] = {
    {"get_all", "all tx"},
    {NULL}
};

static shr_error_e
sand_signal_expand_print(
    rhlist_t * field_list,
    int depth,
    prt_control_t * prt_ctr,
    int attr_offset,
    int flags)
{
    signal_output_t *field_output;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    RHITERATOR(field_output, field_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(attr_offset);

        if (!ISEMPTY(field_output->expansion) && sal_strcasecmp(RHNAME(field_output), field_output->expansion))
        {
            PRT_CELL_SET_SHIFT(depth, "%s(%s)", RHNAME(field_output), field_output->expansion);
        }
        else
        {
            PRT_CELL_SET_SHIFT(depth, "%s", RHNAME(field_output));
        }
        PRT_CELL_SET("%d", field_output->size);
        if (flags & SIGNALS_PRINT_VALUE)
        {
            PRT_CELL_SET("%s", field_output->print_value);
        }

        SHR_CLI_EXIT_IF_ERR(sand_signal_expand_print(field_output->field_list, depth + 1, prt_ctr, attr_offset, flags),
                            "");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
signal_get_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    match_t match_m;
    int core, core_num = 0;
    int attr_offset = 0, addr_offset = 0;
    int i_addr;
    rhlist_t *dsig_list = NULL;
    debug_signal_t *debug_signal;
    signal_output_t *signal_output;
    int all_flag, print_flags = SIGNALS_PRINT_VALUE;
    char *show_str;
    int object_col_id = 0;

    device_t *device;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
    }

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_DPP(unit))
    {
        core_num = SOC_DPP_DEFS_GET(unit, nof_cores);
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        core_num = dnx_data_device.general.nof_cores_get(unit);
    }
#endif

    sal_memset(&match_m, 0, sizeof(match_t));

    match_m.flags = SIGNALS_MATCH_EXPAND;

    SH_SAND_GET_STR("name", match_m.name);
    SH_SAND_GET_STR("block", match_m.block);
    SH_SAND_GET_STR("from", match_m.from);
    SH_SAND_GET_STR("to", match_m.to);
    SH_SAND_GET_STR("stage", match_m.stage);
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_BOOL("all", all_flag);
    SH_SAND_GET_ENUM("order", match_m.output_order);

    if (all_flag == TRUE)
    {
        match_m.name = NULL;
        match_m.from = NULL;
        match_m.to = NULL;
        match_m.stage = NULL;
    }
    else if (ISEMPTY(match_m.name) && ISEMPTY(match_m.from) && ISEMPTY(match_m.to) && ISEMPTY(match_m.stage))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Please specify some filtering criteria ar use \"all\" to show all signals\n");
    }

#ifdef ADAPTER_SERVER_MODE
    {
        /*
         * Send example packet to ensure appearance of signals
         */
        int tx_enable;
        SH_SAND_GET_BOOL("tx", tx_enable);
        if (tx_enable == TRUE)
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_tx(unit, example_data_string));
        }
    }
    /*
     * Currently CMODEL do not have signals per core,so we use 0 by default instead of all
     */
    if (core == BCM_CORE_ALL)
    {
        core = 0;
        print_flags |= SIGNALS_PRINT_CORE;
    }
#endif

    SH_SAND_GET_STR("show", show_str);

    if (!ISEMPTY(show_str))
    {   /* Show options parsing */
        char **tokens;
        uint32 realtokens = 0;
        int i_token;

        if ((tokens = utilex_str_split(show_str, ",", 6, &realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Problem parsing show string\n");
        }

        for (i_token = 0; i_token < realtokens; i_token++)
        {
            if (!sal_strcasecmp(tokens[i_token], "detail"))
                print_flags |= SIGNALS_PRINT_DETAIL;
            else if (!sal_strcasecmp(tokens[i_token], "hw"))
                print_flags |= SIGNALS_PRINT_HW;
            else if (!sal_strcasecmp(tokens[i_token], "source"))
                match_m.flags |= SIGNALS_MATCH_HW;
            else if (!sal_strcasecmp(tokens[i_token], "perm"))
                match_m.flags |= SIGNALS_MATCH_PERM;
            else if (!sal_strcasecmp(tokens[i_token], "noexpand"))
                match_m.flags &= ~SIGNALS_MATCH_EXPAND;
            else if (!sal_strcasecmp(tokens[i_token], "exact"))
                match_m.flags |= SIGNALS_MATCH_EXACT;
            else if (!sal_strcasecmp(tokens[i_token], "nocond"))
                match_m.flags |= SIGNALS_MATCH_NOCOND;
            else if (!sal_strcasecmp(tokens[i_token], "noresolve"))
                match_m.flags |= SIGNALS_MATCH_NORESOLVE;
            else if (!sal_strcasecmp(tokens[i_token], "lucky"))
                match_m.flags |= SIGNALS_MATCH_ONCE;
            else
            {
                utilex_str_split_free(tokens, realtokens);
                SHR_CLI_EXIT(_SHR_E_PARAM, "Unknown show option:%s\n", tokens[i_token]);
            }
        }

        utilex_str_split_free(tokens, realtokens);
    }

    if ((dsig_list = utilex_rhlist_create("prt_print", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
    }

    if (core == BCM_CORE_ALL)
    {
        if (core_num > 1)
        {
            print_flags |= SIGNALS_PRINT_CORE;
        }
        for (core = 0; core < core_num; core++)
        {
            sand_signal_list_get(device, unit, core, &match_m, dsig_list);
        }
    }
    else
    {
        if ((core < 0) || (core >= core_num))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal core ID:%d for device\n", core);
        }
        sand_signal_list_get(device, unit, core, &match_m, dsig_list);
    }

    PRT_TITLE_SET("Signals");

    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    attr_offset = PRT_COLUMN_NUM;

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Attribute");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Size");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_BINARY, PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "From");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "To");

    if ((print_flags & SIGNALS_PRINT_HW) || (print_flags & SIGNALS_PRINT_DETAIL))
    {
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "HW");
    }
    if (print_flags & SIGNALS_PRINT_DETAIL)
    {
        addr_offset = PRT_COLUMN_NUM;
        PRT_COLUMN_ADD("High");
        PRT_COLUMN_ADD("Low");
        PRT_COLUMN_ADD("MSB");
        PRT_COLUMN_ADD("LSB");
        PRT_COLUMN_ADD("Perm");
    }

    RHITERATOR(signal_output, dsig_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        debug_signal = signal_output->debug_signal;
        if (!ISEMPTY(debug_signal->expansion) && sal_strcasecmp(debug_signal->attribute, debug_signal->expansion))
        {
            PRT_CELL_SET("%s(%s)", debug_signal->attribute, debug_signal->expansion);
        }
        else
        {
            PRT_CELL_SET("%s", debug_signal->attribute);
        }
        PRT_CELL_SET("%d", debug_signal->size);
        PRT_CELL_SET("%s", signal_output->print_value);

        if (print_flags & SIGNALS_PRINT_CORE)
        {
            PRT_CELL_SET("%s_%d", debug_signal->block_n, signal_output->core);
        }
        else
        {
            PRT_CELL_SET("%s", debug_signal->block_n);
        }
        PRT_CELL_SET("%s", debug_signal->from);
        PRT_CELL_SET("%s", debug_signal->to);

        if ((print_flags & SIGNALS_PRINT_HW) || (print_flags & SIGNALS_PRINT_DETAIL))
        {
            PRT_CELL_SET("%s", debug_signal->hw);
        }
        if (print_flags & SIGNALS_PRINT_DETAIL)
        {
            for (i_addr = 0; i_addr < debug_signal->range_num; i_addr++)
            {
                if (i_addr != 0)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(addr_offset);
                }
                PRT_CELL_SET("%d", debug_signal->address[i_addr].high);
                PRT_CELL_SET("%d", debug_signal->address[i_addr].low);
                PRT_CELL_SET("%d", debug_signal->address[i_addr].msb);
                PRT_CELL_SET("%d", debug_signal->address[i_addr].lsb);
            }
            PRT_CELL_SET("%d", debug_signal->perm);
        }

        SHR_CLI_EXIT_IF_ERR(sand_signal_expand_print(signal_output->field_list, 1, prt_ctr, attr_offset, print_flags),
                            "");
    }

    PRT_COMMITX;
exit:
    if (dsig_list != NULL)
    {
        sand_signal_list_free(dsig_list);
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_compose_man = {
    "Compose data per signal structure",
    "Compose data per signal structure, using either default values or the ones provides as options\n"
        "Pay attention than when there are multiple occurrences of the same name, block/from options should be used to get the right one\n"
        "Value may be provided in any format, although output will be always in hex."
        " In the value string last character considered LSB and padded by O for all lacking in direction of MSB\n"
        "Use \"signal struct\" to view all possible structures",
    "sig compose name=<struct> [[field=value]...] [block=<name>] [from=<name>]\n",
    "sig cmps name=ETH DA=01.02.03.04.05.06\n"
        "sig prs System_Headers_Record=0x008000000000050000040036 block=ETPP from=PRP",
};

static sh_sand_option_t signal_compose_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Structure which data is be composed", NULL},
    {"block", SAL_FIELD_TYPE_STR, "expansion specific block name", ""},
    {"from", SAL_FIELD_TYPE_STR, "expansion specific from stage name", ""},
    {"order", SAL_FIELD_TYPE_ENUM, "print values in certain endian order <little/big>", "big",
     (void *) order_enum_table},
    {NULL}
};

static sh_sand_invoke_t signal_compose_invokes[] = {
    {"eth_proto", "name=eth"},
    {NULL}
};

static shr_error_e
signal_compose_expansion_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    SHR_FUNC_INIT_VARS(unit);
/*
    SHR_CLI_EXIT_IF_ERR(sand_signal_parse_exists(unit, keyword), "Expansion:%s does not exist\n", keyword);
*/
    *type_p = SAL_FIELD_TYPE_STR;

    SHR_FUNC_EXIT;
}

static shr_error_e
signal_compose_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    signal_output_t signal_output;
    int output_order;
    char *expansion_n = NULL, *block_n, *from_n;
    sh_sand_arg_t *sand_arg;
    int i_uint;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&signal_output, 0, sizeof(signal_output_t));

    SH_SAND_GET_STR("name", expansion_n);
    SH_SAND_GET_STR("block", block_n);
    SH_SAND_GET_STR("from", from_n);
    SH_SAND_GET_ENUM("order", output_order);
    /*
     * Fill Structure Name
     */
    sal_strncpy(RHNAME(&signal_output), expansion_n, RHNAME_MAX_SIZE - 1);
    /*
     * Create list for potential non-default field values \
     */
    if ((signal_output.field_list = utilex_rhlist_create("field_list", sizeof(signal_output_t), 0)) == NULL)
        goto exit;
    /*
     * Read all input fields with values
     */
    SH_SAND_GET_ITERATOR(sand_arg)
    {
        rhhandle_t temp = NULL;
        signal_output_t *field_output;
        char *field_n;

        field_n = SH_SAND_GET_NAME(sand_arg);
        /*
         * Add field to the list
         */
        if (utilex_rhlist_entry_add_tail(signal_output.field_list, field_n, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
            goto exit;
        field_output = (signal_output_t *) temp;
        /*
         * Fill field value inside the list element
         */
        sal_strncpy(field_output->print_value, SH_SAND_ARG_STR(sand_arg), DSIG_MAX_SIZE_STR - 1);
    }
    /*
     * We have structure ready - fetch final buffer
     */
    /** coverity[copy_paste_error : FALSE] */
    SHR_CLI_EXIT_IF_ERR(sand_signal_compose(unit, &signal_output, from_n, block_n, output_order), "");
    /*
     * It arrives in 2 forms: string for output
     */
    LOG_CLI((BSL_META("%s=%s\n"), RHNAME(&signal_output), signal_output.print_value));
    /*
     * And as uint32 array
     */
    LOG_CLI((BSL_META("%s\n"), RHNAME(&signal_output)));
    for (i_uint = 0; i_uint < BITS2WORDS(signal_output.size); i_uint++)
    {
        LOG_CLI((BSL_META("%08X\n"), signal_output.value[i_uint]));
    }

exit:
    if (signal_output.field_list != NULL)
    {
        sand_signal_list_free(signal_output.field_list);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_parser_man = {
    "signal parser parse value according to the structure name",
    "signal parser(sig prs) command accepts signal structure name and optionally block/from names to parse provided value\n"
        "Pay attention than when there are multiple occurrences of the same name, block/from options should be used to get the right one\n"
        "Value may be provided in any format, although output will be always in hex."
        " In the value string last character considered LSB and padded by O for all lacking in direction of MSB\n"
        "Use \"signal struct\" to view all possible structures",
    "sig prs [<struct>=<value>] [block=<name>] [from=<name>]\n",
    "sig prs SRv6_Qualifier=0x1264\n" "sig prs System_Headers_Record=0x008000000000050000040036 block=ETPP from=PRP",
};

static sh_sand_option_t signal_parser_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "expansion structure name", ""},
    {"size", SAL_FIELD_TYPE_INT32, "value size in bits", "0"},
    {"block", SAL_FIELD_TYPE_STR, "expansion specific block name", ""},
    {"from", SAL_FIELD_TYPE_STR, "expansion specific from stage name", ""},
    {"order", SAL_FIELD_TYPE_ENUM, "print values in certain endian order <little/big>", "big",
     (void *) order_enum_table},
    {NULL}
};

static shr_error_e
signal_parser_expansion_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sand_signal_parse_exists(unit, keyword), "Expansion:%s does not exist\n", keyword);

    *type_p = SAL_FIELD_TYPE_ARRAY32;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
signal_parser_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int attr_offset;
    rhlist_t *dsig_list = NULL;
    signal_output_t *signal_output;
    int output_order;
    int object_col_id = 0;
    char *expansion_n = NULL, *block_n, *from_n;
    sh_sand_arg_t *sand_arg;
    int size_in;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("block", block_n);
    SH_SAND_GET_STR("from", from_n);
    SH_SAND_GET_ENUM("order", output_order);
    SH_SAND_GET_INT32("size", size_in);

    PRT_TITLE_SET("Parsed Signal");

    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    attr_offset = PRT_COLUMN_NUM;

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Attribute");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Size");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_BINARY, PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "From");

    if ((dsig_list = utilex_rhlist_create("prt_print", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
    }

    SH_SAND_GET_ITERATOR(sand_arg)
    {
        expansion_n = SH_SAND_GET_NAME(sand_arg);
        sand_signal_parse_get(unit, expansion_n, block_n, from_n, output_order, SH_SAND_ARG_ARRAY_DATA(sand_arg),
                              size_in, dsig_list);
    }

    RHITERATOR(signal_output, dsig_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        if (sal_strcasecmp(signal_output->expansion, expansion_n))
        {
            PRT_CELL_SET("%s(%s)", expansion_n, signal_output->expansion);
        }
        else
        {
            PRT_CELL_SET("%s", expansion_n);
        }
        PRT_CELL_SET("%d", signal_output->size);
        PRT_CELL_SET("%s", signal_output->print_value);

        PRT_CELL_SET("%s", block_n);
        PRT_CELL_SET("%s", from_n);

        SHR_CLI_EXIT_IF_ERR(sand_signal_expand_print
                            (signal_output->field_list, 1, prt_ctr, attr_offset, SIGNALS_PRINT_VALUE), "");
    }

    PRT_COMMITX;
exit:
    if (dsig_list != NULL)
    {
        sand_signal_list_free(dsig_list);
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_struct_man = {
    "Signal structures",
    "List of parsing capabilites for certian signals",
    "signal struct [name=<string>]\n",
    "signal struct name=TM_Cmd",
};

static sh_sand_option_t signal_struct_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Struct name or its substring", ""},
    {"block", SAL_FIELD_TYPE_STR, "expansion specific block name", ""},
    {"from", SAL_FIELD_TYPE_STR, "expansion specific from stage name", ""},
    {"packet", SAL_FIELD_TYPE_BOOL, "show only network protocols", "No"},
    {"desc", SAL_FIELD_TYPE_BOOL, "show additional info", "No"},
    {"show", SAL_FIELD_TYPE_STR, "Misc. options to control filtering/output", ""},
    {NULL}
};

static shr_error_e
signal_struct_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n, *block_n, *from_n;
    device_t *device;
    sigstruct_t *cur_sigstruct;
    sigstruct_field_t *cur_sigstruct_field;
    int packet_flag, desc_flag;
    int field_column;
    char *show_str;
    int match_flags = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
    }

    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_STR("block", block_n);
    SH_SAND_GET_STR("from", from_n);
    SH_SAND_GET_BOOL("packet", packet_flag);
    SH_SAND_GET_BOOL("desc", desc_flag);

    SH_SAND_GET_STR("show", show_str);

    if (!ISEMPTY(show_str))
    {   /* Show options parsing */
        char **tokens;
        uint32 realtokens = 0;
        int i_token;

        if ((tokens = utilex_str_split(show_str, ",", 6, &realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Problem parsing show string\n");
        }

        for (i_token = 0; i_token < realtokens; i_token++)
        {
            if (!sal_strcasecmp(tokens[i_token], "perm"))
                match_flags |= SIGNALS_MATCH_PERM;
            else if (!sal_strcasecmp(tokens[i_token], "exact"))
                match_flags |= SIGNALS_MATCH_EXACT;
            else if (!sal_strcasecmp(tokens[i_token], "lucky"))
                match_flags |= SIGNALS_MATCH_ONCE;
            else
            {
                utilex_str_split_free(tokens, realtokens);
                SHR_CLI_EXIT(_SHR_E_PARAM, "Unknown show option:%s\n", tokens[i_token]);
            }
        }

        utilex_str_split_free(tokens, realtokens);
    }

    PRT_TITLE_SET("Parsed Structures");

    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD("Signal");
    PRT_COLUMN_ADD("From");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Size");
    if (desc_flag == TRUE)
    {
        PRT_COLUMN_ADD("Expansion");
        PRT_COLUMN_ADD("Plugin");
    }
    field_column = PRT_COLUMN_NUM;

    PRT_COLUMN_ADD("Field");
    PRT_COLUMN_ADD("Start");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Smart");
    PRT_COLUMN_ADD("Condition");

    RHITERATOR(cur_sigstruct, device->struct_list)
    {
        /*
         * Print only structure used to represent network ones
         */
        if ((packet_flag == TRUE) && (cur_sigstruct->order != PRINT_BIG_ENDIAN))
            continue;
        /*
         * If there is specific "from" stage name, only signal from this stage may be matched to it
         */
        if (!ISEMPTY(from_n) && (ISEMPTY(cur_sigstruct->from_n) ||
                                 (sal_strcasestr(cur_sigstruct->from_n, from_n) == NULL)))
            continue;
        /*
         * If there is specific block name, only signal belonging to this block may be matched to it
         */
        if (!ISEMPTY(block_n) && (ISEMPTY(cur_sigstruct->block_n) ||
                                  (sal_strcasestr(cur_sigstruct->block_n, block_n) == NULL)))
            continue;

        if (!ISEMPTY(match_n))
        {
            if (match_flags && SIGNALS_MATCH_EXACT)
            {
                if (sal_strcasecmp(RHNAME(cur_sigstruct), match_n))
                    continue;
            }
            else
            {
                if (sal_strcasestr(RHNAME(cur_sigstruct), match_n) == NULL)
                    continue;
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", RHNAME(cur_sigstruct));
        PRT_CELL_SET("%s", cur_sigstruct->from_n);
        PRT_CELL_SET("%s", cur_sigstruct->block_n);
        PRT_CELL_SET("%d", cur_sigstruct->size);

        if (desc_flag == TRUE)
        {
            PRT_CELL_SET("%s", cur_sigstruct->expansion_m.name);
            PRT_CELL_SET("%s", cur_sigstruct->plugin_n);
        }

        RHITERATOR(cur_sigstruct_field, cur_sigstruct->field_list)
        {
            if (cur_sigstruct_field != utilex_rhlist_entry_get_first(cur_sigstruct->field_list))
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(field_column);
            }

            PRT_CELL_SET("%s", RHNAME(cur_sigstruct_field));
            if (cur_sigstruct->order == PRINT_LITTLE_ENDIAN)
            {
                PRT_CELL_SET("%d", cur_sigstruct_field->start_bit);
            }
            else
            {
                PRT_CELL_SET("%d", cur_sigstruct->size - cur_sigstruct_field->end_bit - 1);
            }
            PRT_CELL_SET("%d", cur_sigstruct_field->size);
            PRT_CELL_SET("%s", sal_field_type_str(cur_sigstruct_field->type));
            if (!ISEMPTY(cur_sigstruct_field->expansion_m.name))
            {
                PRT_CELL_SET("%s(E)", cur_sigstruct_field->expansion_m.name);
            }
            else if (!ISEMPTY(cur_sigstruct_field->resolution))
            {
                PRT_CELL_SET("%s(R)", cur_sigstruct_field->resolution);
            }
            else
            {
                PRT_CELL_SKIP(1);
            }
            PRT_CELL_SET("%s", cur_sigstruct_field->cond_attribute);
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_stage_man = {
    "Lists stages of the pipeline",
    "List of stages serves as a facilitator to signal filtering tool",
    "signal stages [name=<string>]\n",
    "signal stage block=IRPP\n" "signal stage name=parser",
};

static sh_sand_option_t signal_stage_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Full stage name or its substring", ""},
    {"block", SAL_FIELD_TYPE_STR, "Full block name or its substring", ""},
    {NULL}
};

static shr_error_e
signal_stage_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n, *block_n;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_STR("block", block_n);

    PRT_TITLE_SET("SIGNAL STAGES");

    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Stage");

#ifdef ADAPTER_SERVER_MODE
    PRT_COLUMN_ADD("ID");
#endif

    {
        int i_bl, i_st;
        pp_block_t *cur_pp_block;
        pp_stage_t *cur_pp_stage;
        device_t *device;
        if ((device = sand_signal_device_get(unit)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
        }

        for (i_bl = 0; i_bl < device->block_num; i_bl++)
        {
            cur_pp_block = &device->pp_blocks[i_bl];

            if (!ISEMPTY(block_n) && (sal_strcasestr(cur_pp_block->name, block_n) == NULL))
                continue;

            for (i_st = 0; i_st < cur_pp_block->stage_num; i_st++)
            {
                cur_pp_stage = &cur_pp_block->stages[i_st];

                if (!ISEMPTY(match_n) && (sal_strcasestr(cur_pp_stage->name, match_n) == NULL))
                    continue;

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", cur_pp_block->name);
                PRT_CELL_SET("%s", cur_pp_stage->name);
#ifdef ADAPTER_SERVER_MODE
                PRT_CELL_SET("%d", cur_pp_stage->id);
#endif
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_param_man = {
    "List of parameters",
    "List of parameters - names for values of certain signals",
    "signal param [name=<string>]\n",
    "signal param name=FHEI_Code",
};

static sh_sand_option_t signal_param_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Parameter name or its substring", ""},
    {NULL}
};

static shr_error_e
signal_param_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int value_offset;
    char *match_n;
    device_t *device;
    sigparam_t *cur_sigparam;
    sigparam_value_t *cur_sigparam_value;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
    }

    SH_SAND_GET_STR("name", match_n);

    PRT_TITLE_SET("Parameters Decoding");
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD("Signal");
    PRT_COLUMN_ADD("Size");
    value_offset = 2;
#ifdef BCM_DNX_SUPPORT
    PRT_COLUMN_ADD("DBAL");
    value_offset++;
#endif
    PRT_COLUMN_ADD("Value Name");
    PRT_COLUMN_ADD("Value");

    RHITERATOR(cur_sigparam, device->param_list)
    {
        if (!ISEMPTY(match_n) && (sal_strcasestr(RHNAME(cur_sigparam), match_n) == NULL))
            continue;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", RHNAME(cur_sigparam));
        PRT_CELL_SET("%d", cur_sigparam->size);
#ifdef BCM_DNX_SUPPORT
        PRT_CELL_SET("%s", cur_sigparam->dbal_n);
#endif

        RHITERATOR(cur_sigparam_value, cur_sigparam->value_list)
        {
            if (cur_sigparam_value != utilex_rhlist_entry_get_first(cur_sigparam->value_list))
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(value_offset);
            }

            PRT_CELL_SET("%s", RHNAME(cur_sigparam_value));
            PRT_CELL_SET("%d", cur_sigparam_value->value);

        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_reset_man = {
    "Deinit/Init signal DB",
    "Deinit/Init signal DB - use after changing any field in XML DB",
    "signal reset\n",
    "sig rst",
};

static shr_error_e
signal_reset_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    device_t *device;

    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
    }

    SHR_CLI_EXIT_IF_ERR(sand_signal_reread(unit, device), "");

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_sand_signal_cmds[] = {
    {"get", signal_get_cb, NULL, signal_get_options, &signal_get_man, NULL, signal_get_invokes}
    ,
    {"compose", signal_compose_cb, NULL, signal_compose_options, &signal_compose_man, signal_compose_expansion_get,
     signal_compose_invokes}
    ,
    {"parser", signal_parser_cb, NULL, signal_parser_options, &signal_parser_man, signal_parser_expansion_get}
    ,
    {"structure", signal_struct_cb, NULL, signal_struct_options, &signal_struct_man}
    ,
    {"stage", signal_stage_cb, NULL, signal_stage_options, &signal_stage_man}
    ,
    {"parameter", signal_param_cb, NULL, signal_param_options, &signal_param_man}
    ,
    {"reset", signal_reset_cb, NULL, NULL, &signal_reset_man}
    ,
    {NULL}
};

sh_sand_man_t sh_sand_signal_man = {
    cmd_sand_signal_desc,
    NULL,
    NULL,
    NULL,
};

cmd_result_t
cmd_sand_signal(
    int unit,
    args_t * args)
{
    cmd_result_t result;
    /*
     * Verify command 
     */
    SH_SAND_VERIFY(sh_sand_signal_cmds, result);

    sh_sand_act(unit, args, sh_sand_signal_cmds, sh_sand_sys_cmds);
    ARG_DISCARD(args);
    /*
     * Always return OK - we provide all help & usage from inside framework
     */
exit:
    return result;

}

const char cmd_sand_signal_usage[] = "Please use \"signal(sig) usage\" for help\n";
/*
 * General shell style description
 */
const char cmd_sand_signal_desc[] = "Present device signals per filtering criteria";
