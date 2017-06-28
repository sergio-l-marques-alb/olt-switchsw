/**
 * \file diag_sand_framework.c
 *
 * Framework for sand shell commands development
 *
 */
/*
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#if defined(UNIX) && !defined(__KERNEL__)
#include <time.h>
#endif

#include <sal/core/regex.h>
#include <sal/appl/sal.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_file.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_prt.h>

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#endif

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

sh_sand_control_t sys_ctr = { NULL, NULL};

void
sh_sand_print(char *string, int left_margin, int right_margin, int term_width)
{
    int token_size;
    int str_shift;
    int str_offset = 0;
    int str_size = strlen(string);

    token_size = term_width - left_margin - right_margin;

    do
    {
        /*
         * Print left margin
         */
        diag_sand_prt_char(left_margin, ' ');
        /*
         * Get shift where last white space in the token or first new line are situated
         */
        str_shift = utilex_str_get_shift(string + str_offset, token_size);
        /*
         * Print string up to this delimiter, print only specified number of characters(str_shift) from the string
         */
        LOG_CLI((BSL_META("%.*s\n"), str_shift, string + str_offset));
        /*
         * If delimiter is new line print one more new line
         */
        if(*(string + str_offset) == '\n')
        {
            LOG_CLI((BSL_META("\n")));
        }
        /*
         * Update current string offset taking into account delimiter
         */
        str_offset += str_shift;
    } while(str_offset < str_size); /* once current offset exceeds string length stop */
}

/**
 * Keeping all the tokens form general shell to be compliant :)
 */
static sh_sand_enum_t sand_bool_table[] = {
    {"False",  FALSE},
    {"True",   TRUE},
    {"Y",      TRUE},
    {"N",      FALSE},
    {"Yes",    TRUE},
    {"No",     FALSE},
    {"On",     TRUE},
    {"Off",    FALSE},
    {"Yep",    TRUE},
    {"Nope",   FALSE},
    {"1",      TRUE},
    {"0",      FALSE},
    {"OKay",   TRUE},
    {"YOUBET", TRUE},
    {"NOWay",  FALSE},
    {"YEAH",   TRUE},
    {"NOT",    FALSE},
    {NULL}
};

char *sh_sand_bool_str(
        int bool)
{
    if(bool == FALSE)
        return sand_bool_table[0].string;
    else
        return sand_bool_table[1].string;
}

void
sh_sand_time_get(char *time_str)
{
#if defined(UNIX) && !defined(__KERNEL__)
    time_t time;
    struct tm *timestruct;

    time = sal_time();
    timestruct = sal_localtime(&time);
    sal_strftime(time_str, SH_SAND_MAX_TIME_SIZE, "%Y-%m-%d %H:%M:%S", timestruct);
    return;
#endif
}

/**
 * \brief - parse 'port' var and convert to ports bitmap 
 * See SAL_FIELD_TYPE_PORT for more info
 * 
 */
static shr_error_e
sh_sand_port_bitmap_get(
    int unit,
    char *diag_port_input,
    bcm_pbmp_t  *logical_ports_bitmap)
{
    bcm_port_t logical_port;
    char *str_ptr;
    SHR_FUNC_INIT_VARS(unit);

    /** Clear bitmap */
    BCM_PBMP_CLEAR(*logical_ports_bitmap);

    /*
     * Avoid verification before device init done
     */
    if (!SOC_IS_INIT(unit))
    {
        SHR_EXIT();
    }


    /** Integer value */
    logical_port = sal_ctoi(diag_port_input, &str_ptr);
    /* 
     *  if str_ptr does not point to the end of the string failed to parse.
     *  if failed - continue to the other options
     */
    if (*str_ptr == 0)
    {
        BCM_PBMP_PORT_ADD(*logical_ports_bitmap, logical_port);
        SHR_EXIT();
    }

    /*
     * Logical port type - 
     * Currently supporting 'tm'/'fabric'/'nif'/'all' 
     * Supported only by DNX!
     */
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        dnx_algo_port_logicals_type_e logicals_type;

        logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_INVALID;
        if (!sal_strcasecmp(diag_port_input, "tm"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_TM;
        }
        else if (!sal_strcasecmp(diag_port_input, "fabric"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC;
        }
        else if (!sal_strcasecmp(diag_port_input, "nif"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_NIF;
        }
        else if (!sal_strcasecmp(diag_port_input, "all"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_VALID;
        }
        if (logicals_type != DNX_ALGO_PORT_LOGICALS_TYPE_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, logicals_type, 0, logical_ports_bitmap));
            SHR_EXIT();
        }
    }
#endif

    /*
     * Parse by port name
     */
    if (SHR_FAILURE(parse_bcm_pbmp(unit, diag_port_input, logical_ports_bitmap)))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Port string:%s is not supported\n", diag_port_input);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_value_get(
    int unit,
    sal_field_type_e type,
    char *source,
    sh_sand_param_u * target,
    void *ext_ptr)
{
    char *end_ptr;
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case SAL_FIELD_TYPE_BOOL:
            if(source == NULL)
            { /* When boolean option appears on command line without value - meaning is TRUE */
                target->val_bool = TRUE;
            }
            else
            {
                int i_ind;
                for(i_ind = 0; sand_bool_table[i_ind].string != NULL; i_ind++)
                {
                    if(!sal_strcasecmp(sand_bool_table[i_ind].string, source))
                    {
                        target->val_bool = sand_bool_table[i_ind].value;
                        break;
                    }
                }
                if(sand_bool_table[i_ind].string == NULL)
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "Boolean string:%s is not supported\n", source);
                }
            }
            break;
        case SAL_FIELD_TYPE_INT32:
            target->val_int32 = sal_strtol(source, &end_ptr, 0);
            if (end_ptr[0] != 0)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            break;
        case SAL_FIELD_TYPE_UINT32:
            target->val_uint32 = sal_strtoul(source, &end_ptr, 0);
            if (end_ptr[0] != 0)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            break;
        case SAL_FIELD_TYPE_IP4:
            if (parse_ipaddr(source, &target->ip4_addr) != _SHR_E_NONE)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            break;
        case SAL_FIELD_TYPE_IP6:
            if (parse_ip6addr(source, target->ip6_addr) != _SHR_E_NONE)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            break;
        case SAL_FIELD_TYPE_STR:
            sal_strncpy(target->val_str, source, SH_SAND_MAX_TOKEN_SIZE - 1);
            break;
        case SAL_FIELD_TYPE_MAC:
            if (parse_macaddr(source, target->mac_addr) != _SHR_E_NONE)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            break;
        case SAL_FIELD_TYPE_ARRAY32:
            parse_long_integer(target->array_uint32, SH_SAND_MAX_ARRAY32_SIZE, source);
            break;
        case SAL_FIELD_TYPE_BITMAP:
            break;
        case SAL_FIELD_TYPE_ENUM:
            {
                sh_sand_enum_t *enum_entry = (sh_sand_enum_t *)ext_ptr;
                if(enum_entry == NULL)
                {
                    SHR_CLI_EXIT(_SHR_E_INTERNAL, "Bad enum option for:%s\n", source);
                }
                for(; enum_entry->string != NULL; enum_entry++)
                {
                    if(!sal_strcasecmp(enum_entry->string, source))
                    {
                        target->val_enum = enum_entry->value;
                        break;
                    }
                }
                if(enum_entry->string == NULL)
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "Enum string:%s is not supported\n", source);
                }
            }
            break;
        case SAL_FIELD_TYPE_PORT:
            {
                SHR_IF_ERR_EXIT(sh_sand_port_bitmap_get(unit, source, &target->ports_bitmap));
                break;
            }
        case SAL_FIELD_TYPE_NONE:
        default:
            SHR_CLI_EXIT(_SHR_E_PARAM, "Unsupported parameter type:%d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static void
sh_sand_keyword_list_init(void)
{
    int i_key, i_ch, i_sh;

    for(i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {   /* Build shortcut */
        i_sh = 0;
        for(i_ch = 0; i_ch < sal_strlen(sh_sand_keywords[i_key].keyword); i_ch++)
        {
            if(isupper(sh_sand_keywords[i_key].keyword[i_ch]))
            {
                sh_sand_keywords[i_key].short_key[i_sh++] = sh_sand_keywords[i_key].keyword[i_ch];
            }
        }
        /*
         * Null terminate the string. If no capital - no shortcut was defined
         */
        sh_sand_keywords[i_key].short_key[i_sh] = 0;
    }
}

shr_error_e
sh_sand_keyword_get(
    char *keyword,
    char **resource_keyword_p)
{
    int i_key;
    int str_len;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    for(i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {
        if(!sal_strcasecmp(sh_sand_keywords[i_key].keyword, keyword))
        {
            break;
        }
        str_len = sal_strlen(keyword);
        if((keyword[str_len - 1] == 's') || (keyword[str_len - 1] == 'S'))
        {
            str_len--;
            if(!sal_strncasecmp(sh_sand_keywords[i_key].keyword, keyword, str_len))
            {
                break;
            }
        }
    }

    if(sh_sand_keywords[i_key].keyword == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INIT, "Keyword:\"%s\" was not registered\n", keyword);
    }

    if(resource_keyword_p != NULL)
    {
        *resource_keyword_p = sh_sand_keywords[i_key].keyword;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_keyword_verify(
    char *keyword,
    char **short_key_p)
{
    int i_key;
    int str_len;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    for(i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {
        if(!sal_strcasecmp(sh_sand_keywords[i_key].keyword, keyword))
        {
            break;
        }
        str_len = sal_strlen(keyword);
        if((keyword[str_len - 1] == 's') || (keyword[str_len - 1] == 'S'))
        {
            str_len--;
            if(!sal_strncasecmp(sh_sand_keywords[i_key].keyword, keyword, str_len))
            {
                break;
            }
        }
    }

    if(sh_sand_keywords[i_key].keyword == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INIT, "Keyword:\"%s\" was not registered\n", keyword);
    }

    if(short_key_p != NULL)
    {
        *short_key_p = sh_sand_keywords[i_key].short_key;
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_arg_t*
sh_sand_option_get(rhlist_t *args_list, char *option_name)
{
    sh_sand_arg_t *sh_sand_arg;

    RHITERATOR(sh_sand_arg, args_list)
    {
        if (!sal_strcasecmp(option_name, RHNAME(sh_sand_arg))
            || ((sh_sand_arg->short_key != NULL) && (!sal_strcasecmp(option_name, sh_sand_arg->short_key))))
        {
            break;
        }
    }

    return sh_sand_arg;
}

static shr_error_e
sh_sand_option_list_process(
    int unit,
    args_t * args,
    rhlist_t * args_list,
    int legacy_mode,
    sh_sand_cmd_t * sh_sand_cmd)
{
    sh_sand_arg_t *sh_sand_arg;
    char *cur_arg;
    int i;
    int variable_present = FALSE;
    char **tokens = NULL;
    uint32 realtokens = 0;
    char *option_value = NULL, *option_name = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (args_list == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "ERROR - Null args_list\n");
    }

    if(legacy_mode == TRUE)
    {   /* No option processing for legacy commands */
        SHR_EXIT() ;
    }

    RHITERATOR(sh_sand_arg, args_list)
    {
        sh_sand_arg->present = FALSE;
    }

    if(sh_sand_cmd != NULL)
    {
        /*
         * Create or clean dynamic List
         */
        if(sh_sand_cmd->ctr.dyn_args_list == NULL)
        {
            if ((sh_sand_cmd->ctr.dyn_args_list = utilex_rhlist_create("Options", sizeof(sh_sand_arg_t), 0)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for dyn args list\n");
            }
        }
        else /* dyn_args_list != NULL */
        {
            utilex_rhlist_clean(sh_sand_cmd->ctr.dyn_args_list);
        }

        /*
         * Initialize option string
         */
        SET_EMPTY(sh_sand_cmd->ctr.options_str);
        sh_sand_time_get(sh_sand_cmd->ctr.time_str);
    }

    /*
     * Check all tokens left - supposedly options
     */
    for (i = 0; i < ARG_CNT(args); i++)
    {
        if ((cur_arg = ARG_GET_WITH_INDEX(args, ARG_CUR_INDEX(args) + i)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "No option for:%d\n", ARG_CUR_INDEX(args) + i);
        }

        if(sh_sand_cmd != NULL)
        {
            if((sal_strlen(sh_sand_cmd->ctr.options_str) + sal_strlen(cur_arg)) >= (SH_SAND_MAX_TOKEN_SIZE - 1))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Option string size is too long to include%s\n", cur_arg);
            }
            utilex_str_append(sh_sand_cmd->ctr.options_str, cur_arg);
        }

        /*
         * Analyze option
         */
        if((tokens = utilex_str_split(cur_arg, "=", 3, &realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\n");
        }
        /* If there is no = in the token, for one occurrence we assume that it is "variable" option */
        option_name = tokens[0];
        switch(realtokens)
        {
            case 0:
                SHR_CLI_EXIT(_SHR_E_MEMORY, "No tokens in:%s\n", cur_arg);
                break;
            case 1:
                if ((sh_sand_arg = sh_sand_option_get(args_list, option_name)) == NULL)
                {   /* For one unknown option we can assume its name is variable */
                    if(variable_present == FALSE)
                    {
                        option_name = "variable";
                        if ((sh_sand_arg = utilex_rhlist_entry_get_by_sub(args_list, option_name)) == NULL)
                        {
                            SHR_CLI_EXIT(_SHR_E_PARAM, "Option:%s is not supported\n", tokens[0]);
                        }
                        else
                        {
                            variable_present = TRUE;
                            option_value = tokens[0];
                        }
                    }
                    else
                    {
                        SHR_CLI_EXIT(_SHR_E_PARAM, "Only one free variable is allowed\n");
                    }
                }
                else
                {   /* Find option, but only boolean are allowed to miss the value */
                    if (sh_sand_arg->type != SAL_FIELD_TYPE_BOOL)
                    {
                        SHR_CLI_EXIT(_SHR_E_PARAM, "Option:\"%s\" requires a value\n", option_name);
                    }
                    else
                    {   /* Boolean option with no explicit value */
                        option_value = NULL;
                    }
                }
                break;
            case 2: /* regular option with value*/
                /*
                 * Look for keyword in the args_list
                 */
                if ((sh_sand_arg = sh_sand_option_get(args_list, option_name)) == NULL)
                {
                    sal_field_type_e    type    = SAL_FIELD_TYPE_NONE;
                    uint32              id      = 0;
                    void*               ext_ptr = NULL;
                    rhhandle_t          void_arg;
                    /*
                     * Check if the option is dynamic one
                     */
                    if((sh_sand_cmd != NULL) && (sh_sand_cmd->option_cb != NULL))
                    {
                        if(sh_sand_cmd->option_cb(unit, option_name, &type, &id, &ext_ptr) == _SHR_E_NONE)
                        {
                            if (utilex_rhlist_entry_add_tail(sh_sand_cmd->ctr.dyn_args_list,
                                                                    option_name, id, &void_arg) != _SHR_E_NONE)
                            {
                                SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Cannot add option:%s to dynamic list\n", option_name);
                            }
                            sh_sand_arg = void_arg;
                            sh_sand_arg->type = type;
                            sh_sand_arg->ext_ptr = ext_ptr;
                        }
                        else
                        {
                            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Option:%s not recognized\n", option_name);
                        }
                    }
                    else
                    {
                        SHR_CLI_EXIT(_SHR_E_PARAM, "Option:%s is not supported\n", option_name);
                    }
                }
                option_value = tokens[1];
                break;
            case 3:
            default:
                SHR_CLI_EXIT(_SHR_E_PARAM, "More than 1 \"=\" in the input\n");
                break;
        }

        sh_sand_arg->present = TRUE;
        if (sh_sand_value_get(unit, sh_sand_arg->type, option_value, &sh_sand_arg->param, sh_sand_arg->ext_ptr) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal value:\"%s\" for option:\"%s\"\n", option_value, option_name);
        }

        utilex_str_split_free(tokens, realtokens);
        tokens = NULL;
    }

    RHITERATOR(sh_sand_arg, args_list)
    {
        if((sh_sand_arg->present == FALSE) && (sh_sand_arg->requested == TRUE))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Presence of option:\"%s\" is requested\n", RHNAME(sh_sand_arg));
        }
        /*
         * Re-take default value for port type options that were not provide
         */
        if((sh_sand_arg->present == FALSE) && (sh_sand_arg->type == SAL_FIELD_TYPE_PORT))
        {
            if (sh_sand_value_get(unit, sh_sand_arg->type, sh_sand_arg->def_str, &sh_sand_arg->def, sh_sand_arg->ext_ptr) != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal Default value:\"%s\" for option:\"%s\"\n",
                                                                                sh_sand_arg->def_str, RHNAME(sh_sand_arg));
            }
        }
    }

exit:
    if(tokens != NULL)
    {   /* Means we left the loop on error */
        utilex_str_split_free(tokens, realtokens);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_option_list_init(
    int unit,
    sh_sand_option_t *options,
    rhlist_t **sand_args_list_p)
{
    sh_sand_option_t *option;
    rhhandle_t void_arg;
    sh_sand_arg_t *sh_sand_arg;
    rhlist_t *sand_args_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if(sand_args_list_p == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR - No pointer for args list provided\n");
    }
    /* If option == NULL, no command specific options provided, just system one will be relevant */
    if ((sand_args_list = utilex_rhlist_create("Options", sizeof(sh_sand_arg_t), 1)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for args list\n");
    }

    for (option = options; (option != NULL) && (option->keyword != NULL); option++)
    {
        sh_sand_option_t *cmp_option;

        for (cmp_option = sh_sand_sys_options; cmp_option->keyword != NULL; cmp_option++)
        {
            if (!sal_strcasecmp(option->keyword, cmp_option->keyword))
            {
                SHR_CLI_EXIT(_SHR_E_INIT, "Error - option:\"%s\" is system one\n", option->keyword);
            }
        }

        SHR_CLI_EXIT_IF_ERR(sh_sand_keyword_verify(option->keyword, &option->short_key), "");
        if(ISEMPTY(option->desc))
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Error - option:\"%s\" has no description\n", option->keyword);
        }
        if((option->type == SAL_FIELD_TYPE_ENUM) && (option->ext_ptr == NULL))
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Error - enum option:\"%s\" requests enum list to be provided\n", option->keyword);
        }

        if (utilex_rhlist_entry_add_tail(sand_args_list,
                                         option->keyword, RHID_TO_BE_GENERATED, &void_arg) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add option:\"%s\" to the list\n", option->keyword);
        }
        sh_sand_arg = void_arg;
        sh_sand_arg->type = option->type;
        sh_sand_arg->ext_ptr = option->ext_ptr;
        sh_sand_arg->short_key = option->short_key;
        sh_sand_arg->def_str = option->def;
        if (sh_sand_arg->def_str != NULL)
        {
            if (sh_sand_value_get(unit, sh_sand_arg->type, sh_sand_arg->def_str, &sh_sand_arg->def, sh_sand_arg->ext_ptr) != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal Default value:\"%s\" for option:\"%s\"\n",
                                                                            sh_sand_arg->def_str, RHNAME(sh_sand_arg));
            }
        }
        else
        {   /* Absence of default means that option presence is requested */
            sh_sand_arg->requested = TRUE;
        }
    }
    /*
     * Add all system options to the list
     */
    for (option = sh_sand_sys_options; option->keyword != NULL; option++)
    {
        if (utilex_rhlist_entry_add_tail(sand_args_list,
                                         option->keyword, RHID_TO_BE_GENERATED, &void_arg) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add system option:\"%s\" to the list\n", option->keyword);
        }
        sh_sand_arg = void_arg;
        sh_sand_arg->type = option->type;
        sh_sand_arg->ext_ptr = option->ext_ptr;
        sh_sand_arg->def_str = option->def;
        if (sh_sand_arg->def_str != NULL)
        {
            if (sh_sand_value_get(unit, sh_sand_arg->type, sh_sand_arg->def_str, &sh_sand_arg->def, sh_sand_arg->ext_ptr) != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal Default value:\"%s\" for option:\"%s\"\n",
                                                                                sh_sand_arg->def_str, RHNAME(sh_sand_arg));
            }
        }
        else
        {   /* Absence of default means that option presence is requested */
            sh_sand_arg->requested = TRUE;
        }
    }

exit:
    if((sand_args_list != NULL) && !SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        utilex_rhlist_free_all(sand_args_list);
    }
    else
    {
        if(sand_args_list_p != NULL)
        {
            *sand_args_list_p = sand_args_list;
        }
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_usage_leaf_tabular(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_control_t *sand_control)
{
    sh_sand_option_t *option;
    int item_col_id, option_col_id;
    int flag;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if ((sh_sand_cmd == NULL) || (sh_sand_cmd->man == NULL))
    { /* On this stage there is no need to print message, all errors should be rectified by verify */
        SHR_EXIT();
    }

    PRT_TITLE_SET("%s", "Usage");

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &item_col_id, "Item");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, item_col_id,  &option_col_id, "Option");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, option_col_id, NULL, "Type");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, option_col_id, NULL, "Default");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_ASCII, PRT_XML_ATTRIBUTE, option_col_id, NULL, "Description");

    if (sh_sand_cmd->man->synopsis != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("SYNOPSYS");
        PRT_CELL_SKIP(3);
        PRT_CELL_SET("%s", sh_sand_cmd->man->synopsis);
    }

    if (sh_sand_cmd->man->full != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("DESCRIPTION");
        PRT_CELL_SKIP(3);
        PRT_CELL_SET("%s", sh_sand_cmd->man->full);
    }

    if ((sh_sand_cmd->options != NULL) && (sh_sand_cmd->options->keyword != NULL))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("OPTIONS");
        for (option = sh_sand_cmd->options; option->keyword != NULL; option++)
        {
            char *def_str;
            if(option->def == NULL)
            {
                def_str = "NONE";
            }
            else if(ISEMPTY(option->def))
            {
                def_str = "EMPTY";
            }
            else
            {
                def_str = option->def;
            }

            /* For the first option skip 1 cell, for all others allocate new row */
            if(option != sh_sand_cmd->options)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            PRT_CELL_SET("%s", option->keyword);
            PRT_CELL_SET("%s", sal_field_type_str(option->type));
            PRT_CELL_SET("%s", def_str);
            PRT_CELL_SET("%s", option->desc);
        }
        PRT_ROW_SET_MODE(PRT_ROW_SEP_UNDERSCORE);
    }

    /*
     * Show system options only when verbose option used
     */
    SH_SAND_GET_BOOL("verbose", flag);
    if(flag == TRUE)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("SYSTEM OPTIONS");
        for (option = sh_sand_sys_options; option->keyword != NULL; option++)
        {
            char *def_str;
            if(option->def == NULL)
            {
                def_str = "NONE";
            }
            else if(ISEMPTY(option->def))
            {
                def_str = "EMPTY";
            }
            else
            {
                def_str = option->def;
            }

            /* For the first option skip 1 cell, for all others allocate new row */
            if(option != sh_sand_sys_options)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            PRT_CELL_SET("%s", option->keyword);
            PRT_CELL_SET("%s", sal_field_type_str(option->type));
            PRT_CELL_SET("%s", def_str);
            PRT_CELL_SET("%s", option->desc);
        }
        PRT_ROW_SET_MODE(PRT_ROW_SEP_UNDERSCORE);
    }

    if (sh_sand_cmd->man->examples != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("EXAMPLES");
        PRT_CELL_SKIP(3);
        PRT_CELL_SET("%s", sh_sand_cmd->man->examples);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_usage_leaf(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_control_t *sand_control)
{
    int columns, margin;
    int flag;
    sh_sand_option_t *option;

    SHR_FUNC_INIT_VARS(unit);

    if ((sh_sand_cmd == NULL) || (sh_sand_cmd->man == NULL))
    { /* On this stage there is no need to print message, all errors should be rectified by verify */
        SHR_EXIT();
    }

    SH_SAND_GET_INT32("tabular", flag);
    if(flag == TRUE)
    {
        sh_sand_usage_leaf_tabular(unit, sh_sand_cmd, sand_control);
        goto exit;
    }

    SH_SAND_GET_INT32("columns", columns);
    SH_SAND_GET_INT32("margin", margin);

    if (sh_sand_cmd->man->synopsis != NULL)
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "SYNOPSYS"));
        sh_sand_print(sh_sand_cmd->man->synopsis, margin, margin, columns);
    }

    if (sh_sand_cmd->man->full != NULL)
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "DESCRIPTION"));
        sh_sand_print(sh_sand_cmd->man->full, margin, margin, columns);
    }

    if ((sh_sand_cmd->options != NULL) && (sh_sand_cmd->options->keyword != NULL))
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "OPTIONS"));
        diag_sand_prt_char(margin, ' ');
        LOG_CLI((BSL_META("\033[1m%s\033[0m\n\n"), "option (type:default)"));
        for (option = sh_sand_cmd->options; option->keyword != NULL; option++)
        {
            char *def_str;

            if(option->def == NULL)
            {
                def_str = "NONE";
            }
            else if(ISEMPTY(option->def))
            {
                def_str = "EMPTY";
            }
            else
            {
                def_str = option->def;
            }

            diag_sand_prt_char(margin, ' ');
            LOG_CLI((BSL_META("%s (%s:%s)\n"), option->keyword, sal_field_type_str(option->type), def_str));
            if(option->desc)
            {
                sh_sand_print(option->desc, 2 * margin, margin, columns);
            }
        }
    }

    SH_SAND_GET_BOOL("verbose", flag);
    if(flag == TRUE)
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "SYSTEM OPTIONS"));
        for (option = sh_sand_sys_options; option->keyword != NULL; option++)
        {
            diag_sand_prt_char(margin, ' ');
            LOG_CLI((BSL_META("%s (%s:%s)\n"), option->keyword, sal_field_type_str(option->type),
                                                ((option->def == NULL) ? "NA" : option->def)));
            if(option->desc)
            {
                sh_sand_print(option->desc, 2 * margin, margin, columns);
            }
        }
    }

    if (sh_sand_cmd->man->examples != NULL)
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "EXAMPLES"));
        sh_sand_print(sh_sand_cmd->man->examples, margin, margin, columns);
    }
    LOG_CLI((BSL_META("\n")));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_usage_branch(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_control_t * sand_control)
{
    sh_sand_cmd_t *sh_sand_cmd;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s", "Supported commands");

    PRT_COLUMN_ADD("Command");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd && (sh_sand_cmd->keyword != NULL); sh_sand_cmd++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", sh_sand_cmd->keyword);
        if(sh_sand_cmd->man && !ISEMPTY(sh_sand_cmd->man->brief))
        {
            PRT_CELL_SET("%s", sh_sand_cmd->man->brief);
        }
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "usage");
    PRT_CELL_SET("%s", "print this info for branches or usage for leaf commands");
    PRT_COMMIT;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_cmd_fetch(
        rhlist_t *command_list,
        char *command_str)
{
    rhhandle_t rhhandle;

    SET_EMPTY(command_str);

    RHITERATOR(rhhandle, command_list)
    {
        utilex_str_append(command_str, RHNAME(rhhandle));
    }

    return _SHR_E_NONE;
}

static shr_error_e
sh_sand_cmd_traverse(
    int unit,
    args_t * args,
    sh_sand_cmd_t *sh_sand_cmd_a,
    sh_sand_traverse_cb_t sh_sand_action,
    void *flex_ptr,
    int flags)
{
    sh_sand_cmd_t *sh_sand_cmd;
    SHR_FUNC_INIT_VARS(unit);

    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
    {
        if (sh_sand_cmd->cmd != NULL)
        {
            SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse(unit, args, sh_sand_cmd->cmd, sh_sand_action, flex_ptr, flags),
                                                        "Error on traversing:%s\n",sh_sand_cmd->keyword);
        }
        else if (sh_sand_cmd->action != NULL)
        {
            SHR_CLI_EXIT_IF_ERR(sh_sand_action(unit, args, sh_sand_cmd, flex_ptr, flags),
                                                        "Error on:%s\n", sh_sand_cmd->keyword);
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error(Command is neither node, nor leaf) on:%s\n", sh_sand_cmd->keyword);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_cmd_get(
    int unit,
    char *cmd_name,
    sh_sand_cmd_t *sh_sand_cmd_a,
    sh_sand_cmd_t **sh_sand_cmd_p)
{
    sh_sand_cmd_t *sh_sand_cmd;
    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: Command list is NULL for:%s\n", cmd_name);
    }
    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
    {
        /*
         * Call to the first command that is supported and match the cmd_name
         */
        if (!sal_strcasecmp(cmd_name, sh_sand_cmd->keyword) ||
            (!ISEMPTY(sh_sand_cmd->short_key) && (!sal_strcasecmp(cmd_name, sh_sand_cmd->short_key))))
        {
            break;
        }
    }
    /*
     * Check if we have arrived to NULL entry
     */
    if(sh_sand_cmd->keyword != NULL)
    {   /*
         * We found something, if pointer was provided - fill it
         */
        if(sh_sand_cmd_p)
        {
            *sh_sand_cmd_p = sh_sand_cmd;
        }
    }
    else
    {
        LOG_CLI((BSL_META("ERROR: command:\"%s\" is not supported\n"), cmd_name));
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_sys_args_init(
    int unit,
    sh_sand_option_t *options,
    rhlist_t **sand_args_list_p)
{
    sh_sand_option_t *option;
    rhhandle_t void_arg;
    sh_sand_arg_t *sh_sand_arg;
    rhlist_t *sand_args_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /* If option == NULL, no command specific options provided, just system one will be relevant */
    if ((sand_args_list = utilex_rhlist_create("Options", sizeof(sh_sand_arg_t), 1)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for system args list\n");
    }

    /*
     * Add all options to the list
     */
    for (option = options; option->keyword != NULL; option++)
    {
        if (utilex_rhlist_entry_add_tail(sand_args_list,
                                         option->keyword, RHID_TO_BE_GENERATED, &void_arg) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add system option:\"%s\" to the list\n", option->keyword);
        }
        sh_sand_arg = void_arg;
        sh_sand_arg->type = option->type;
        sh_sand_arg->ext_ptr = option->ext_ptr;
        sh_sand_arg->def_str = option->def;
        if (sh_sand_arg->def_str != NULL)
        {
            if (sh_sand_value_get(unit, sh_sand_arg->type, sh_sand_arg->def_str, &sh_sand_arg->def, sh_sand_arg->ext_ptr) != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Default Value:\"%s\" for option:\"%s\" does not match option definition\n",
                                                                                sh_sand_arg->def_str, RHNAME(sh_sand_arg));
            }
        }
        else
        {
            memset(&sh_sand_arg->def, 0, sizeof(sh_sand_param_u));
        }
    }

exit:
    if((sand_args_list != NULL) && !SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        utilex_rhlist_free_all(sand_args_list);
    }
    else
    {
        if(sand_args_list_p != NULL)
        {
            *sand_args_list_p = sand_args_list;
        }
        else
        {
            utilex_rhlist_free_all(sand_args_list);
        }
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_cmds_init(
    int unit,
    char *command,
    rhlist_t **sand_cmds_list_p)
{
    rhlist_t *sand_cmds_list = NULL;
    char **cmd_tokens = NULL;
    uint32 cmd_num = 0;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    /* Fill command list from command string */
    if ((sand_cmds_list = utilex_rhlist_create("Commands", sizeof(rhentry_t), 1)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for control command list\n");
    }

    if((cmd_tokens = utilex_str_split(command, " ", SH_SAND_MAX_COMMAND_DEPTH, &cmd_num)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\n");
    }
    /* If there is no = in the token, for one occurrence we assume that it is "variable" option */

    for(i = 0; i < cmd_num; i++)
    {
        rhhandle_t command_handle;
        if (utilex_rhlist_entry_add_tail(sand_cmds_list,
                                        cmd_tokens[i], RHID_TO_BE_GENERATED, &command_handle) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Cannot add command:%s to command list\n", cmd_tokens[i]);
        }
    }

exit:
    if((sand_cmds_list != NULL) && !SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        utilex_rhlist_free_all(sand_cmds_list);
    }
    else
    {
        if(sand_cmds_list_p != NULL)
        {
            *sand_cmds_list_p = sand_cmds_list;
        }
        else
        {
            utilex_rhlist_free_all(sand_cmds_list);
        }
    }
    if(cmd_tokens != NULL)
    {
        utilex_str_split_free(cmd_tokens, cmd_num);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_cmd_all(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a)
{
    sh_sand_cmd_t *sh_sand_cmd;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Iterate over command array invoking all commands using system options. In this case we don't care about errors
     */
    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
    {
        if (sh_sand_cmd->cmd != NULL)
        {
            SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_all(unit, args, sh_sand_cmd->cmd),"");
        }
        else if (sh_sand_cmd->action != NULL)
        {  /* Process options */
            SHR_CLI_EXIT_IF_ERR(sh_sand_option_list_process(unit, args, sh_sand_cmd->ctr.stat_args_list, sh_sand_cmd->legacy_mode, sh_sand_cmd),"");
            SHR_CLI_EXIT_IF_ERR(sh_sand_cmd->action(unit, args, &sh_sand_cmd->ctr),"");
        }
        else
        {
            continue;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_act(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a)
{
    char *cmd_name;
    sh_sand_cmd_t *sh_sand_cmd;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: No further command provided\n");
    }

    if(sys_ctr.stat_args_list == NULL)
    {   /* Somehow system options list was not initialized, init sequence problem */
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: System options list was not initialized, check init sequence\n");
    }
    /*
     * Get command name
     */
    if(((cmd_name = ARG_GET(args)) == NULL) || !sal_strncasecmp(cmd_name, "?", strlen(cmd_name))
                                            || !sal_strncasecmp(cmd_name, "help", strlen(cmd_name))
                                            || !sal_strncasecmp(cmd_name, "usage", strlen(cmd_name)))
    {   /* if no command name - print help */
        if(sh_sand_option_list_process(unit, args, sys_ctr.stat_args_list, FALSE, NULL) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Erroneous system options\n");
        }
        sh_sand_usage_branch(unit, sh_sand_cmd_a, &sys_ctr);
        goto exit;
    }
    else if (!sal_strncasecmp(cmd_name, "all", strlen(cmd_name)))
    {
        /*
         * Iterate over command array invoking all commands. In this case we don't care about errors 
         */
        sh_sand_cmd_all(unit, args, sh_sand_cmd_a);
    }
    else
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sand_cmd_a, &sh_sand_cmd), "");

        if (sh_sand_cmd->cmd != NULL)
        {
            SHR_SET_CURRENT_ERR(sh_sand_act(unit, args, sh_sand_cmd->cmd));
        }
        else if (sh_sand_cmd->action != NULL) /* Currently either leaf or node, it may be both */
        {
            if(((cmd_name = ARG_CUR(args)) != NULL) &&
                                                   (   !sal_strncasecmp(cmd_name, "usage", strlen(cmd_name))
                                                    || !sal_strncasecmp(cmd_name, "help", strlen(cmd_name))))
            {   /* Move after usage to see options */
                ARG_NEXT(args);
                if(sh_sand_option_list_process(unit, args, sys_ctr.stat_args_list, FALSE, NULL) != _SHR_E_NONE)
                {
                    SHR_CLI_EXIT(_SHR_E_INTERNAL, "Erroneous system options for %s usage\n", sh_sand_cmd->keyword);
                }
                sh_sand_usage_leaf(unit, sh_sand_cmd, &sys_ctr);
            }
            else
            {   /* Process command options */
                if ((SHR_SET_CURRENT_ERR(sh_sand_option_list_process(unit, args, sh_sand_cmd->ctr.stat_args_list, sh_sand_cmd->legacy_mode, sh_sand_cmd))) == _SHR_E_NONE)
                {
                    SHR_SET_CURRENT_ERR(sh_sand_cmd->action(unit, args, &sh_sand_cmd->ctr));
                }
            }
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: command:%s had neither leaf nor branch\n", cmd_name);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_test_list_init(
    int unit,
    sh_sand_invoke_t *invokes,
    rhlist_t **sand_args_list_p)
{
    sh_sand_invoke_t *invoke;
    rhlist_t *sand_args_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if(invokes == NULL)
    {
/*
 * Legitimate case of test list empty, which points the the fact that there is only one default test or
 * that tests will be added dynamically later on
 */
        SHR_EXIT();
    }

    if(sand_args_list_p == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR - No pointer for args list provided\n");
    }
    sand_args_list = *sand_args_list_p;

    /* List may be already non NULL, if dynamic tests were added */
    if(sand_args_list == NULL)
    {
        if ((sand_args_list = utilex_rhlist_create("Test", sizeof(sh_sand_test_t), 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Failed to allocate list for test args\n");
        }
        *sand_args_list_p = sand_args_list;
    }

    for (invoke = invokes; invoke->name != NULL; invoke++)
    {
        sh_sand_test_t *test_invoke;
        rhhandle_t void_test;

        if (utilex_rhlist_entry_add_tail(sand_args_list,
                                invoke->name, RHID_TO_BE_GENERATED, &void_test) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add option:\"%s\" to the list\n", invoke->name);
        }
        test_invoke = void_test;
        test_invoke->flags = invoke->flags;
        if((test_invoke->params = sal_alloc(sal_strlen(invoke->params) + 1, "ctest")) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Error - failed to allocate memory for test params on:\"%s\" to the list\n", invoke->name);
        }
        sal_strcpy(test_invoke->params, invoke->params);
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_ctest_list_man = {
    "Present the list of all tests defined",
    "Present the list of all tests defined",
    "ctest list",
    "ctest list"
};

sh_sand_man_t dnx_ctest_export_man = {
    "Export the list of all tests defined to tlist file",
    "Export the list of all tests defined to tlist file, tlist with default name will be created if no input for filename was provided",
    "ctest export [file=Filename.tlist]",
    "ctest export file=testDvapiJer2Reg.tlist"
};

/**
* \brief
* List all defined tests for node or leaf
*****************************************************/
static shr_error_e
cmd_ctest_list(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *flex_ptr,
    int flags)
{
    sh_sand_test_t *sh_sand_test;
    int first_line = TRUE;
    prt_control_t *prt_ctr = (prt_control_t *)flex_ptr;

    char command_str[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    sh_sand_cmd_fetch(sh_sand_cmd->ctr.command_list, command_str);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("%s", command_str);
    if(sh_sand_cmd->ctr.test_list == NULL)
    {
        PRT_CELL_SET(CTEST_DEFAULT);
        PRT_CELL_SET("N/A");
        PRT_CELL_SET("PRECOMMIT");
        SHR_EXIT();
    }
    RHITERATOR(sh_sand_test, sh_sand_cmd->ctr.test_list)
    {
        if(first_line == TRUE)
        {
            first_line = FALSE;
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SKIP(1);
        }
        if(!ISEMPTY(RHNAME(sh_sand_test)))
        {
            PRT_CELL_SET("%s", RHNAME(sh_sand_test));
        }
        else
        {
            PRT_CELL_SET("N/A");
        }
        if(!ISEMPTY(sh_sand_test->params))
        {
            PRT_CELL_SET("%s", sh_sand_test->params);
        }
        else
        {
            PRT_CELL_SET("N/A");
        }
        /*
         * Tests with no parameters(defaults) are automatically included in precommit
         */
        if(sh_sand_test->flags & CTEST_PRECOMMIT)
        {
            PRT_CELL_SET("PRECOMMIT");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#if !defined(NO_FILEIO)

static shr_error_e cmd_ctest_export_tlist_add_test(
    void*               file_in,
    char*               command_str,
    sh_sand_cmd_t*      sh_sand_cmd,
    sh_sand_test_t*     sh_sand_test)
{
    rhhandle_t rhhandle;
    sh_sand_control_t * sand_control = &sys_ctr;
    int reset_flag;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    SH_SAND_GET_BOOL("reset", reset_flag);
    /*
     * All tests start from the standard preamble
     */
    if(reset_flag == TRUE)
        sal_strcpy(command_str, "test_list_add ResetOnFail::AT_CTest_");
    else
        sal_strcpy(command_str, "test_list_add AT_CTest_");

    /*
     * Now append entire command chain
     */
    RHITERATOR(rhhandle, sh_sand_cmd->ctr.command_list)
    {
        sal_strcpy(command_str + strlen(command_str), RHNAME(rhhandle));
        sal_strcpy(command_str + strlen(command_str), "_");
    }
    /*
     * Finally append test name
     */
    if(sh_sand_test != NULL)
        sal_strcpy(command_str + strlen(command_str), RHNAME(sh_sand_test));
    else
        sal_strcpy(command_str + strlen(command_str), CTEST_DEFAULT);

    sal_strcpy(command_str + strlen(command_str), "\n");
    /*
     * Record full line in the file, whose handle is delivered by flex_ptr
     */
    dbx_file_write(file_in, command_str);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Write List of tests to .tlist file
**/
static shr_error_e
cmd_ctest_export_tlist(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *file_in,
    int flags)
{
    sh_sand_test_t *sh_sand_test;

    char command_str[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    /* Check if there is list, if not we assume there only one test without parameters */
    if(sh_sand_cmd->ctr.test_list == NULL)
    {
        cmd_ctest_export_tlist_add_test(file_in, command_str, sh_sand_cmd, NULL);
    }
    else
    {
        RHITERATOR(sh_sand_test, sh_sand_cmd->ctr.test_list)
        {
            /* If export is for pre-commit only, include only tests with CTEST_PRECOMMIT */
            if(((flags & CTEST_PRECOMMIT) == TRUE) && !(sh_sand_test->flags & CTEST_PRECOMMIT))
                continue;

            cmd_ctest_export_tlist_add_test(file_in, command_str, sh_sand_cmd, sh_sand_test);
        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e cmd_ctest_export_tcl_add_test(
    void*               file_in,
    char*               command_str,
    sh_sand_cmd_t*      sh_sand_cmd,
    sh_sand_test_t*   sh_sand_test)
{
    rhhandle_t rhhandle;
    char command_only[RHSTRING_MAX_SIZE];
    sh_sand_control_t * sand_control = &sys_ctr;
    int reset_flag;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    SH_SAND_GET_BOOL("reset", reset_flag);
    /*
     * Line 1 - Create separation empty line - Test Prolog
     */
    dbx_file_write(file_in, "\n");
    /*
     * Line 2 - Procedure Name
     */
    if(reset_flag == TRUE)
        sal_strcpy(command_str, "proc ResetOnFail::AT_CTest_");
    else
        sal_strcpy(command_str, "proc AT_CTest_");
    /*
     * Now append entire command chain
     */
    RHITERATOR(rhhandle, sh_sand_cmd->ctr.command_list)
    {
        sal_strcpy(command_str + strlen(command_str), RHNAME(rhhandle));
        sal_strcpy(command_str + strlen(command_str), "_");
    }
    /*
     * Finally append test name
     */
    if(sh_sand_test != NULL)
        sal_strcpy(command_str + strlen(command_str), RHNAME(sh_sand_test));
    else
        sal_strcpy(command_str + strlen(command_str), CTEST_DEFAULT);
    /*
     * Append line ending
     */
    sal_strcpy(command_str + strlen(command_str), " {chip unit} {\n");
    /*
     * Record full line in the file, whose handle is delivered by file_in
     */
    dbx_file_write(file_in, command_str);
    /*
     * Line 3 - Create actual command line
     */
    sal_strcpy(command_str, "\tbcm shell \"ctest ");
    sh_sand_cmd_fetch(sh_sand_cmd->ctr.command_list, command_only);
    sal_strcpy(command_str + strlen(command_str), command_only);
    if(sh_sand_test != NULL)
    {
        sal_strcpy(command_str + strlen(command_str), " ");
        sal_strcpy(command_str + strlen(command_str), sh_sand_test->params);
    }
    sal_strcpy(command_str + strlen(command_str), "\"\n");
    dbx_file_write(file_in, command_str);
    /*
     * Line 4,5 Procedure Ending
     */
    dbx_file_write(file_in, "\treturn PASS\n");
    dbx_file_write(file_in, "}\n");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Write TCL implementing DVAPI to .tcl file
**/
static shr_error_e
cmd_ctest_export_tcl(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *file_in,
    int flags)
{
    sh_sand_test_t *sh_sand_test;
    char command_str[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if(sh_sand_cmd->ctr.test_list == NULL)
    {
        cmd_ctest_export_tcl_add_test(file_in, command_str, sh_sand_cmd, NULL);
    }
    else
    {
        RHITERATOR(sh_sand_test, sh_sand_cmd->ctr.test_list)
        {
            /* If export is for pre-commit only, include only tests with CTEST_PRECOMMIT */
            if(((flags & CTEST_PRECOMMIT) == TRUE) && !(sh_sand_test->flags & CTEST_PRECOMMIT))
                continue;

            cmd_ctest_export_tcl_add_test(file_in, command_str, sh_sand_cmd, sh_sand_test);
        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_test_export(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a)
{
    void *file;
    char *filename;
    sh_sand_control_t *sand_control = &sys_ctr;
    char filepath[RHFILE_MAX_SIZE];
    int flags = 0, precommit_flag;

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sh_sand_option_list_process(unit, args, sys_ctr.stat_args_list, FALSE, NULL),
                                                                                    "Erroneous options in command line\n");
    SH_SAND_GET_STR("file", filename);
    if(ISEMPTY(filename))
    {
        filename = "testDvapiJer2Ctest.tlist";
    }
    SH_SAND_GET_BOOL("pre", precommit_flag);
    if(precommit_flag == TRUE)
        flags |= CTEST_PRECOMMIT;

    /*
     * Generate TLIST file with list of all DVAPIs
     */
    dbx_file_get_sdk_path(filename, "/regress/bcm/tests/dnx/", filepath);
    if((file = dbx_file_open(filepath)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Cannot open:%s\n", filepath);
    }
    dbx_file_write(file,"#############################################################################################\n");
    dbx_file_write(file,"# This file contains DVAPI tests for Jericho-II that should be run in the pre commit process#\n");
    dbx_file_write(file,"#############################################################################################\n");
    dbx_file_write(file,"# DO not edit it, generated by running \"ctest export\" from bcm shell                        #\n");
    dbx_file_write(file,"# $Id$                                                                                      #\n");
    dbx_file_write(file,"# $Copyright: (c) 2015 Broadcom Corp.                                                       #\n");
    dbx_file_write(file,"# All Rights Reserved.$                                                                     #\n");
    dbx_file_write(file,"#############################################################################################\n");

    sh_sand_cmd_traverse(unit, args, sh_sand_cmd_a, cmd_ctest_export_tlist, (void *)file, flags);
    dbx_file_close(file);
    /*
     * Generate TCL file with ALL DVAPI
     */
    filename = "CTestDvapis.tcl";

    dbx_file_get_sdk_path(filename, "/regress/bcm/tests/dnx/ctest/", filepath);
    if((file = dbx_file_open(filepath)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Cannot open:%s\n", filepath);
    }
    dbx_file_write(file,"#############################################################################################\n");
    dbx_file_write(file,"# This file contains DVAPI tests for Jericho-II that should be run in the pre commit process#\n");
    dbx_file_write(file,"#############################################################################################\n");
    dbx_file_write(file,"# DO not edit it, generated by running \"ctest export\" from bcm shell                        #\n");
    dbx_file_write(file,"# $Id$                                                                                      #\n");
    dbx_file_write(file,"# $Copyright: (c) 2015 Broadcom Corp.                                                       #\n");
    dbx_file_write(file,"# All Rights Reserved.$                                                                     #\n");
    dbx_file_write(file,"#############################################################################################\n");
    dbx_file_write(file,"\npackage require cints_Dvapi_utils\n");

    sh_sand_cmd_traverse(unit, args, sh_sand_cmd_a, cmd_ctest_export_tcl, (void *)file, flags);
    dbx_file_close(file);

exit:
    SHR_FUNC_EXIT;
}
#else
static shr_error_e
sh_sand_test_export(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a)
{
    return _SHR_E_NONE;
}
#endif /* !defined NO_FILEIO */

static shr_error_e cmd_ctest_run_test(
    int               unit,
    sh_sand_cmd_t*    sh_sand_cmd,
    sh_sand_test_t*   sh_sand_test,
    prt_control_t*    prt_ctr)
{
    char command_str[RHSTRING_MAX_SIZE];
    char command_only[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(NO_UNIT);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    /*
     * Create actual command line
     */
    /*
     * Start from ctest
     */
    sal_strcpy(command_str, "ctest ");
    /*
     * Add all command sequence
     */
    sh_sand_cmd_fetch(sh_sand_cmd->ctr.command_list, command_only);
    sal_strcpy(command_str + strlen(command_str), command_only);

    PRT_CELL_SET("%s", command_only);
    /*
     * Add options if any
     */
    if(sh_sand_test != NULL)
    {
        PRT_CELL_SET("%s", RHNAME(sh_sand_test));
        sal_strcpy(command_str + strlen(command_str), " ");
        sal_strcpy(command_str + strlen(command_str), sh_sand_test->params);
        PRT_CELL_SET("%s", sh_sand_test->params);
    }
    else
    {
        PRT_CELL_SET("%s", CTEST_DEFAULT);
        PRT_CELL_SET("%s", "N/A");
    }

    if (sh_process_command(unit, command_str) != CMD_OK)
    {
        PRT_CELL_SET("FAIL");
    }
    else
    {
        PRT_CELL_SET("OK");
    }

exit:
    SHR_FUNC_EXIT;
}


/**
* \brief
*   Run all tests
**/
static shr_error_e
cmd_ctest_run(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *flex_ptr,
    int flags)
{
    sh_sand_test_t *sh_sand_test;
    prt_control_t *prt_ctr = (prt_control_t *)flex_ptr;

    SHR_FUNC_INIT_VARS(unit);

    /* Check if there is list, if not we assume there only one test without parameters */
    if(sh_sand_cmd->ctr.test_list == NULL)
    {
        cmd_ctest_run_test(unit, sh_sand_cmd, NULL, prt_ctr);
    }
    else
    {
        RHITERATOR(sh_sand_test, sh_sand_cmd->ctr.test_list)
        {
            /* If export is for pre-commit only, include only tests with CTEST_PRECOMMIT */
            if(((flags & CTEST_PRECOMMIT) == TRUE) && !(sh_sand_test->flags & CTEST_PRECOMMIT))
                continue;

            cmd_ctest_run_test(unit, sh_sand_cmd, sh_sand_test, prt_ctr);
        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_test_system_commands(
        int unit,
        args_t * args,
        char *cmd_name,
        sh_sand_cmd_t * sh_sand_cmd_a)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if((cmd_name == NULL) || (!sal_strncasecmp(cmd_name, "?", strlen(cmd_name)))
                          || (!sal_strncasecmp(cmd_name, "usage", strlen(cmd_name)))
                          || (!sal_strncasecmp(cmd_name, "help", strlen(cmd_name))))
    {   /* Print usage for the tree, options may control the output, move args after usage keyword */
        if(sh_sand_option_list_process(unit, args, sys_ctr.stat_args_list, FALSE, NULL) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Erroneous system options\n");
        }

        sh_sand_usage_branch(unit, sh_sand_cmd_a, &sys_ctr);
    }
    else if (!sal_strncasecmp(cmd_name, "list", sal_strlen(cmd_name)))
    {
        PRT_TITLE_SET("%s", "CTests List");
        PRT_COLUMN_ADD("Test Command");
        PRT_COLUMN_ADD("Test Name");
        PRT_COLUMN_ADD("Test Parameters");
        PRT_COLUMN_ADD("Flags");
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse(unit, args, sh_sand_cmd_a, cmd_ctest_list, (void *)prt_ctr, 0), "CTest List Failure");
        PRT_COMMIT;
    }
    else if (!sal_strncasecmp(cmd_name, "export", sal_strlen(cmd_name)))
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_test_export(unit, args, sh_sand_cmd_a), "Test Export Failure\n");
    }
    else if (!sal_strncasecmp(cmd_name, "run", sal_strlen(cmd_name)))
    {
        int flags = 0, precommit_flag;
        sh_sand_control_t *sand_control = &sys_ctr;

        if(sh_sand_option_list_process(unit, args, sys_ctr.stat_args_list, FALSE, NULL) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Erroneous system options\n");
        }

        SH_SAND_GET_BOOL("pre", precommit_flag);
        if(precommit_flag == TRUE)
            flags |= CTEST_PRECOMMIT;

        PRT_TITLE_SET("%s", "CTests Results");
        PRT_COLUMN_ADD("Test Command");
        PRT_COLUMN_ADD("Test Name");
        PRT_COLUMN_ADD("Test Parameters");
        PRT_COLUMN_ADD("Test Result");
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse(unit, args, sh_sand_cmd_a, cmd_ctest_run, (void *)prt_ctr, flags), "CTest Run Failure\n");
        PRT_COMMIT;
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_test(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a)
{
    char *cmd_name;
    sh_sand_cmd_t *sh_sand_cmd;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: No further command provided\n");
    }

    /*
     * Get command name
     */
    cmd_name = ARG_GET(args);

    SHR_SET_CURRENT_ERR(sh_sand_test_system_commands(unit, args, cmd_name, sh_sand_cmd_a));

    if(SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
    {
        /*
         * Now we can move after command word
         */
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sand_cmd_a, &sh_sand_cmd), "");

        if (sh_sand_cmd->cmd != NULL)
        {
            SHR_SET_CURRENT_ERR(sh_sand_test(unit, args, sh_sand_cmd->cmd));
        }
        else if (sh_sand_cmd->action != NULL) /* Currently either leaf or node, it may be both */
        {
            if(((cmd_name = ARG_CUR(args)) != NULL) &&
                                                   (   !sal_strncasecmp(cmd_name, "usage", strlen(cmd_name))
                                                    || !sal_strncasecmp(cmd_name, "help", strlen(cmd_name))))
            {   /* Move after usage to see options */
                ARG_NEXT(args);
                if(sh_sand_option_list_process(unit, args, sys_ctr.stat_args_list, FALSE, NULL) != _SHR_E_NONE)
                {
                    SHR_CLI_EXIT(_SHR_E_INTERNAL, "Erroneous system options for %s usage\n", sh_sand_cmd->keyword);
                }
                sh_sand_usage_leaf(unit, sh_sand_cmd, &sys_ctr);
            }
            else
            {   /*
                 * Process options
                 */
                if ((SHR_SET_CURRENT_ERR(sh_sand_option_list_process(unit, args, sh_sand_cmd->ctr.stat_args_list, sh_sand_cmd->legacy_mode, sh_sand_cmd))) == _SHR_E_NONE)
                {
                    sal_usecs_t usec = sal_time_usecs();
                    SHR_SET_CURRENT_ERR(sh_sand_cmd->action(unit, args, &sh_sand_cmd->ctr));
                    usec = sal_time_usecs() - usec;
                    cli_out("Test duration:%d.%d.%d\n", usec/1000000, (usec % 1000000) / 1000, (usec % 1000000) % 1000);
                }
            }
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: command:%s had neither leaf nor branch\n", cmd_name);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_test_add(
    int             unit,
    sh_sand_cmd_t*  sh_sand_cmd_a,
    char*           cmd_name,
    char*           test_name,
    char*           test_params,
    int             flags)
{
    sh_sand_cmd_t*  sh_sand_cmd;
    sh_sand_test_t *test_invoke;
    rhhandle_t void_test;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: No further command provided\n");
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sand_cmd_a, &sh_sand_cmd), "");

    if(sh_sand_cmd->ctr.test_list == NULL)
    {
        if ((sh_sand_cmd->ctr.test_list = utilex_rhlist_create("Tests", sizeof(sh_sand_test_t), 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for tests list\n");
        }
    }

    if (utilex_rhlist_entry_add_tail(sh_sand_cmd->ctr.test_list,
                            test_name, RHID_TO_BE_GENERATED, &void_test) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add option:\"%s\" to the list\n", test_name);
    }
    test_invoke = void_test;
    if((test_invoke->params = sal_alloc(sal_strlen(test_params) + 1, "ctest")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Error - failed to allocate memory for test params on:\"%s\" to the list\n", test_name);
    }
    sal_strcpy(test_invoke->params, test_params);
    test_invoke->flags = flags;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_test_remove(
    int             unit,
    sh_sand_cmd_t*  sh_sand_cmd_a,
    char*           cmd_name,
    char*           test_name)
{
    sh_sand_cmd_t*  sh_sand_cmd;
    sh_sand_test_t *test_invoke;
    rhhandle_t void_test;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: No further command provided\n");
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sand_cmd_a, &sh_sand_cmd), "");

    if(sh_sand_cmd->ctr.test_list == NULL)
    {
        /*
         * No test list - nothing to remove leave peacefully
         */
        SHR_EXIT();
    }

    if((void_test = utilex_rhlist_entry_get_by_name(sh_sand_cmd->ctr.test_list, test_name)) != NULL)
    {
        test_invoke = void_test;
        if((test_invoke = void_test) != NULL)
        {
            sal_free(test_invoke->params);
        }
        utilex_rhlist_entry_del_free(sh_sand_cmd->ctr.test_list, void_test);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_verify_man(char *command, sh_sand_man_t *man)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    if((man == NULL) || (man->brief == NULL) || (man->full == NULL) || (man->examples == NULL) || (man->synopsis == NULL))
    {
        LOG_CLI((BSL_META("Bad \"usage\" for command:%s\n"), command));
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_init(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    char *command)
{
    sh_sand_cmd_t *sh_sand_cmd;

    SHR_FUNC_INIT_VARS(unit);

    if(sys_ctr.stat_args_list == NULL)
    {   /* Should be done only once */
        SHR_CLI_EXIT_IF_ERR(sh_sand_sys_args_init(unit, sh_sand_sys_options, &sys_ctr.stat_args_list),
                                                    "ERROR - problem with system options\n");
        sh_sand_keyword_list_init();
    }
    /*
     * Iterate over command array 
     */
    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
    {   /* Verify that every command is either leaf or branch */
        /*
         * Keep previous command length 
         */
        int cmd_size = strlen(command);
        /*
         * Include present keyword 
         */
        if (cmd_size == 0)
            sal_strcpy(command, sh_sand_cmd->keyword);
        else
            sal_sprintf(command + cmd_size, " %s", sh_sand_cmd->keyword);
        /*
         * Obtain shortcut string
         * Meanwhile do not fail the run - just warn
         */
        sh_sand_keyword_verify(sh_sand_cmd->keyword, &sh_sand_cmd->short_key);

        if ((sh_sand_cmd->cmd == NULL) && (sh_sand_cmd->action == NULL))
        {
            LOG_CLI((BSL_META("ERROR - command:%s is neither leaf nor branch\n"), command));
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        }
        else
        {
            if (sh_sand_cmd->action)
            {   /* Command is leaf, verify that we have correct usage and options */
                SHR_CLI_EXIT_IF_ERR(sh_sand_verify_man(command, sh_sand_cmd->man), "");
                SHR_CLI_EXIT_IF_ERR(sh_sand_option_list_init(unit, sh_sand_cmd->options, &sh_sand_cmd->ctr.stat_args_list),
                                                    "Erroneous option list for command:\"%s\"\n", command);
                /* Fill command list from command string */
                SHR_CLI_EXIT_IF_ERR(sh_sand_cmds_init(unit, command, &sh_sand_cmd->ctr.command_list),
                                                    "Failure to extract command list from command:\"%s\"\n", command);
                /* Initialize test_list from static invokes list */
                SHR_CLI_EXIT_IF_ERR(sh_sand_test_list_init(unit, sh_sand_cmd->invokes, &sh_sand_cmd->ctr.test_list),
                                                    "Failure to initiate test list for command:\"%s\"\n", command);
            }
            if (sh_sand_cmd->cmd)
            {   /* Command is branch got into recursive call */
                SHR_CLI_EXIT_IF_ERR(sh_sand_init(unit, sh_sand_cmd->cmd, command), "");
            }
        }
        /*
         * Return previous command - cut off current keyword 
         */
        command[cmd_size] = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_deinit(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a)
{
    sh_sand_cmd_t *sh_sand_cmd;

    SHR_FUNC_INIT_VARS(unit);

    if(sys_ctr.stat_args_list != NULL)
    {   /* Should be done only once */
        utilex_rhlist_free_all(sys_ctr.stat_args_list);
        sys_ctr.stat_args_list = NULL;
    }
    /*
     * Iterate over command array
     */
    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
    {
        if (sh_sand_cmd->action)
        {
            if(sh_sand_cmd->ctr.stat_args_list)
            {
                utilex_rhlist_free_all(sh_sand_cmd->ctr.stat_args_list);
                sh_sand_cmd->ctr.stat_args_list = NULL;
            }

            if(sh_sand_cmd->ctr.command_list)
            {
                utilex_rhlist_free_all(sh_sand_cmd->ctr.command_list);
                sh_sand_cmd->ctr.command_list = NULL;
            }

            if(sh_sand_cmd->ctr.test_list)
            {
                utilex_rhlist_free_all(sh_sand_cmd->ctr.test_list);
                sh_sand_cmd->ctr.test_list = NULL;
            }
        }
        if (sh_sand_cmd->cmd)
        {   /* Command is branch got into recursive call */
            SHR_CLI_EXIT_IF_ERR(sh_sand_deinit(unit, sh_sand_cmd->cmd), "Deinit Failure");
        }
    }

exit:
    SHR_FUNC_EXIT;
}
