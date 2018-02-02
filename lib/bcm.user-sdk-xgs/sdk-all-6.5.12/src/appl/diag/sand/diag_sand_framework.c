/**
 * \file diag_sand_framework.c
 *
 * Framework for sand shell commands development
 *
 */
/*
 * $Copyright: (c) 2017 Broadcom.
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
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#ifdef INCLUDE_AUTOCOMPLETE
#include <sal/appl/editline/autocomplete.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#endif

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

shr_error_e
sh_sand_cmd_add_to_dyn_list(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    cmd_func_t cmd_callback)
{
    sh_sand_cmd_t *sh_sand_cmd;

    SHR_FUNC_INIT_VARS(unit);

    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
    {
        cmd_t cmd;

        /** coverity[copy_paste_error : FALSE] */
        SHR_CLI_EXIT_IF_ERR(sh_sand_keyword_fetch(sh_sand_cmd->keyword, &sh_sand_cmd->short_key,
                                                  &sh_sand_cmd->full_key, SH_SAND_VERIFY_ALL), "");

        cmd.c_cmd = sh_sand_cmd->full_key;
        cmd.c_f = cmd_callback;
        if (sh_sand_cmd->man == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "No man page for top level command %s\n", sh_sand_cmd->keyword);
        }
        cmd.c_help = sh_sand_cmd->man->brief;
        cmd.c_usage = sh_sand_cmd->man->full;

        if (cmdlist_add(unit, &cmd) != CMD_OK)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed to add %s command\n", sh_sand_cmd->keyword);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_cmd_remove_from_dyn_list(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    cmd_func_t cmd_callback)
{
    sh_sand_cmd_t *sh_sand_cmd;

    SHR_FUNC_INIT_VARS(unit);

    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
    {
        cmd_t cmd;

        cmd.c_cmd = sh_sand_cmd->full_key;
        cmd.c_f = cmd_callback;
        if (sh_sand_cmd->man == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "No man page for top level command %s\n", sh_sand_cmd->keyword);
        }
        cmd.c_help = sh_sand_cmd->man->brief;
        cmd.c_usage = sh_sand_cmd->man->full;

        if (cmdlist_remove(unit, &cmd) != CMD_OK)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed to add %s command\n", sh_sand_cmd->keyword);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_param_u full_param_mask = {
    .array_uint32[0] = 0xFFFFFFFF,
    .array_uint32[1] = 0xFFFFFFFF,
    .array_uint32[2] = 0xFFFFFFFF,
    .array_uint32[3] = 0xFFFFFFFF,
    .array_uint32[4] = 0xFFFFFFFF,
    .array_uint32[5] = 0xFFFFFFFF,
    .array_uint32[6] = 0xFFFFFFFF,
    .array_uint32[7] = 0xFFFFFFFF,
    .array_uint32[8] = 0xFFFFFFFF,
    .array_uint32[9] = 0xFFFFFFFF,
    .array_uint32[10] = 0xFFFFFFFF,
    .array_uint32[11] = 0xFFFFFFFF,
    .array_uint32[12] = 0xFFFFFFFF,
    .array_uint32[13] = 0xFFFFFFFF,
    .array_uint32[14] = 0xFFFFFFFF,
    .array_uint32[15] = 0xFFFFFFFF,
};

sh_sand_invoke_t sh_sand_all_invokes[] = {
    {"all", "all", 0}
    ,
    {NULL}
};

/**
 * Keeping all the tokens form general shell to be compliant :)
 */
static sh_sand_enum_t sand_bool_table[] = {
    {"False", FALSE},
    {"True", TRUE},
    {"Y", TRUE},
    {"N", FALSE},
    {"Yes", TRUE},
    {"No", FALSE},
    {"On", TRUE},
    {"Off", FALSE},
    {"Yep", TRUE},
    {"Nope", FALSE},
    {"1", TRUE},
    {"0", FALSE},
    {"OKay", TRUE},
    {"YOUBET", TRUE},
    {"NOWay", FALSE},
    {"YEAH", TRUE},
    {"NOT", FALSE},
    {NULL}
};

char *
sh_sand_bool_str(
    int bool)
{
    if (bool == FALSE)
        return sand_bool_table[0].string;
    else
        return sand_bool_table[1].string;
}

void
sh_sand_time_get(
    char *time_str)
{
#if defined(UNIX) && !defined(__KERNEL__)
    time_t time;
    struct tm *timestruct;

    time = sal_time();
    timestruct = sal_localtime(&time);
    sal_strftime(time_str, SH_SAND_MAX_TIME_SIZE, "%Y-%m-%d_%H.%M.%S", timestruct);
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
    bcm_pbmp_t * logical_ports_bitmap)
{
    bcm_port_t logical_port;
    char *diag_port_input_copy = NULL;
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

    /** empty string */
    if (diag_port_input == NULL || *diag_port_input == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "port: unexpected empty string.\n");
    }
    /** copy the input string - to be able to modify it */
    diag_port_input_copy = sal_strdup(diag_port_input);
    if (diag_port_input_copy == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "port: failed to copy string.\n");
    }

    /** Integer value */
    logical_port = sal_ctoi(diag_port_input_copy, &str_ptr);
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
        if (!sal_strcasecmp(diag_port_input_copy, "tm"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_TM;
        }
        else if (!sal_strcasecmp(diag_port_input_copy, "fabric"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC;
        }
        else if (!sal_strcasecmp(diag_port_input_copy, "nif"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_NIF;
        }
        else if (!sal_strcasecmp(diag_port_input_copy, "all"))
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
    if (SHR_FAILURE(parse_bcm_pbmp(unit, diag_port_input_copy, logical_ports_bitmap)))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Port string:%s is not supported\n", diag_port_input);
    }

exit:
    SHR_FREE(diag_port_input_copy);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Fetch value from the string to value array according to provided string
 * \param [in] unit - unit id
 * \param [in] enum_entry - pointer to array of string/value pairs
 * \param [in] source - string to seach for in array
 * \param [in,out] value_p - pointer value, that will be assigned by routine in case of success
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - If array is NULL
 *   \retval _SHR_E_NOT_FOUND - If source string was not found in array
 * \remark
 *    Calling function behavior may differ depending on the parameter type, when the parameter is bool or enum,
 *    failure here means general failure, if it is other (like int, uint) failure will just cause treatment source as
 *    value itself
 */
static shr_error_e
sh_sand_enum_value_get(
    int unit,
    sh_sand_enum_t * enum_entry,
    char *source,
    int *value_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value_p, _SHR_E_PARAM, "value_p");

    if (enum_entry == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        for (; enum_entry->string != NULL; enum_entry++)
        {
            if (!sal_strcasecmp(enum_entry->string, source))
            {
                *value_p = enum_entry->value;
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                break;
            }
        }
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
            SHR_CLI_EXIT_IF_ERR(sh_sand_enum_value_get(unit, sand_bool_table, source, &target->val_bool),
                                "Boolean string:%s is not supported\n", source);
            break;
        case SAL_FIELD_TYPE_INT32:
            SHR_SET_CURRENT_ERR(sh_sand_enum_value_get(unit, (sh_sand_enum_t *) ext_ptr, source, &target->val_int32));
            if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
            {
                target->val_int32 = sal_strtoul(source, &end_ptr, 0);
                if (end_ptr[0] != 0)
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                else
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                }
            }
            break;
        case SAL_FIELD_TYPE_UINT32:
            SHR_SET_CURRENT_ERR(sh_sand_enum_value_get(unit, (sh_sand_enum_t *) ext_ptr, source,
                                                       (int *) &target->val_uint32));
            if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
            {
                target->val_uint32 = sal_strtoul(source, &end_ptr, 0);
                if (end_ptr[0] != 0)
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                else
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                }
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
            SHR_SET_CURRENT_ERR(sh_sand_enum_value_get(unit, (sh_sand_enum_t *) ext_ptr, source, &target->val_enum));
            if (SHR_FUNC_VAL_IS(_SHR_E_PARAM))
            {
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Bad Enum option for:%s\n", source);
            }
            else if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Enum string:%s is not supported\n", source);
            }
            break;
        case SAL_FIELD_TYPE_PORT:
            /** coverity[copy_paste_error : FALSE] */
            SHR_CLI_EXIT_IF_ERR(sh_sand_port_bitmap_get(unit, source, &target->ports_bitmap), "");
            break;
        case SAL_FIELD_TYPE_NONE:
        default:
            SHR_CLI_EXIT(_SHR_E_PARAM, "Unsupported parameter type:%d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_option_value_get(
    int unit,
    sal_field_type_e type,
    int *state_p,
    char *source,
    sh_sand_param_u * target1,
    sh_sand_param_u * target2,
    void *ext_ptr)
{
    char **tokens = NULL;
    uint32 realtokens = 0;
    char *value_1_p = NULL, *value_2_p = NULL;
    int state;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(state_p, _SHR_E_PARAM, "state_p");

    state = *state_p;

    if (ISEMPTY(source))
    {
        if (type == SAL_FIELD_TYPE_BOOL)
        {       /* When boolean option appears on command line without value - meaning is TRUE */
            target1->val_bool = TRUE;
        }
        else if (type == SAL_FIELD_TYPE_STR)
        {
            SET_EMPTY(target1->val_str);
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Value of type:\"%s\" cannot be empty\n", sal_field_type_str(type));
        }
    }
    else
    {
        if ((type != SAL_FIELD_TYPE_STR) && (type != SAL_FIELD_TYPE_PORT) && (type != SAL_FIELD_TYPE_ENUM) &&
            (type != SAL_FIELD_TYPE_BOOL))
        {       /* Check mask presence */
            if ((tokens = utilex_str_split(source, ",", 2, &realtokens)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\"%s\"\n", source);
            }
            else if (realtokens == 1)
            {   /* free previous result tokens */
                utilex_str_split_free(tokens, realtokens);
                /*
                 * Now check range presence
                 */
                if ((tokens = utilex_str_split(source, "-", 2, &realtokens)) == NULL)
                {
                    SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\"%s\"\n", source);
                }
                else if (realtokens == 2)
                {
                    if (ISEMPTY(tokens[1]))
                    {
                        SHR_CLI_EXIT(_SHR_E_PARAM, "Range end not provided\n");
                    }

                    if (ISEMPTY(tokens[0]))
                    {   /* Assuming single negative value, not range */
                        realtokens = 1;
                    }
                    else
                    {
                        state |= SH_SAND_OPTION_RANGE;
                    }
                }
            }
            else
            {   /* Now we know it is mask */
                if (ISEMPTY(tokens[0]) || ISEMPTY(tokens[1]))
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "Comma ',' requires 2 values:data and mask\n");
                }
                state |= SH_SAND_OPTION_MASKED;
                sal_memset(target2, 0xFF, sizeof(sh_sand_param_u));
                /*
                 * NO need to do anything more with mask value
                 */
                value_2_p = NULL;
            }

            if (realtokens == 2)
            {
                /*
                 * We had value string containing mask, so we assign both value pointers to be extracted below
                 */
                value_1_p = tokens[0];
                value_2_p = tokens[1];
            }
            else
            {
                value_1_p = source;
            }
        }
        else
        {
            value_1_p = source;
            value_2_p = NULL;
        }
        /*
         * First value will be always present and not empty
         */
        SHR_CLI_EXIT_IF_ERR(sh_sand_value_get(unit, type, value_1_p, target1, ext_ptr), "");
        /*
         * Second value is present only in special cases, like mask and range
         */
        if (value_2_p != NULL)
        {
            SHR_CLI_EXIT_IF_ERR(sh_sand_value_get(unit, type, value_2_p, target2, ext_ptr), "");
        }
    }
    /*
     * Return updated state to the user
     */
    *state_p = state;

exit:
    if (tokens != NULL)
    {   /* Means we left the loop on error */
        utilex_str_split_free(tokens, realtokens);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_arg_t *
sh_sand_option_get(
    int unit,
    rhlist_t * args_list,
    char *option_name,
    sh_sand_option_cb_t option_cb,
    rhlist_t * dyn_args_list)
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
    /*
     * Not found in static list - see if option callback will recognize this option and add it to dynamic one
     */
    if ((sh_sand_arg == NULL) && (option_cb != NULL))
    {
        sal_field_type_e type = SAL_FIELD_TYPE_NONE;
        uint32 id = 0;
        void *ext_ptr = NULL;
        rhhandle_t void_arg;

        if (option_cb(unit, option_name, &type, &id, &ext_ptr) == _SHR_E_NONE)
        {
            if (utilex_rhlist_entry_add_tail(dyn_args_list, option_name, id, &void_arg) != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META("ERROR - Cannot add option:%s to dynamic list"), option_name));
                return NULL;
            }
            sh_sand_arg = void_arg;
            sh_sand_arg->type = type;
            sh_sand_arg->ext_ptr = ext_ptr;
        }
    }
    /*
     * Finally check if keyword starts from the string provided by option_name
     */
    if (sh_sand_arg == NULL)
    {
        RHITERATOR(sh_sand_arg, args_list)
        {
            if (RHNAME(sh_sand_arg) == sal_strcasestr(RHNAME(sh_sand_arg), option_name))
            {
                break;
            }
        }
    }

    return sh_sand_arg;
}

shr_error_e
sh_sand_option_list_process(
    int unit,
    args_t * args,
    rhlist_t * args_list,
    int flags,
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

    if (flags & SH_CMD_LEGACY)
    {   /* No option processing for legacy commands */
        SHR_EXIT();
    }

    RHITERATOR(sh_sand_arg, args_list)
    {
        sh_sand_arg->state = 0;
    }

    if (sh_sand_cmd != NULL)
    {
        /*
         * Create or clean dynamic List
         */
        if (sh_sand_cmd->ctr.dyn_args_list == NULL)
        {
            if ((sh_sand_cmd->ctr.dyn_args_list = utilex_rhlist_create("Options", sizeof(sh_sand_arg_t), 0)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for dyn args list\n");
            }
        }
        else    /* dyn_args_list != NULL */
        {
            utilex_rhlist_clean(sh_sand_cmd->ctr.dyn_args_list);
        }

        /*
         * Initialize option string
         */
        SET_EMPTY(sh_sand_cmd->ctr.options_str);
        sh_sand_time_get(sh_sand_cmd->ctr.time_str);
    }

    if (args == NULL)
    {
        /*
         * No Command line options provided
         */
        SHR_EXIT();
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

        if (sh_sand_cmd != NULL)
        {
            if ((sal_strlen(sh_sand_cmd->ctr.options_str) + sal_strlen(cur_arg)) >= (SH_SAND_MAX_TOKEN_SIZE - 1))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Option string size is too long to include:\"%s\"\n", cur_arg);
            }
            utilex_str_append(sh_sand_cmd->ctr.options_str, cur_arg);
        }

        /*
         * Analyze option
         */
        if ((tokens = utilex_str_split(cur_arg, "=", 3, &realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\n");
        }
        /*
         * If there is no = in the token, for one occurrence we assume that it is "variable" option 
         */
        option_name = tokens[0];
        switch (realtokens)
        {
            case 0:
                SHR_CLI_EXIT(_SHR_E_MEMORY, "No tokens in:%s\n", cur_arg);
                break;
            case 1:
                if ((sh_sand_arg = sh_sand_option_get(unit, args_list, option_name, sh_sand_cmd->option_cb,
                                                      sh_sand_cmd->ctr.dyn_args_list)) == NULL)
                {       /* For one unknown option we can assume its name is variable */
                    if (variable_present == FALSE)
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
                {       /* Find option, but only boolean are allowed to miss the value */
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
            case 2:    /* regular option with value */
                /*
                 * Look for keyword in the args_list
                 */
                if ((sh_sand_arg = sh_sand_option_get(unit, args_list, option_name, sh_sand_cmd->option_cb,
                                                      sh_sand_cmd->ctr.dyn_args_list)) == NULL)
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "Option:%s is not supported\n", option_name);
                }
                option_value = tokens[1];
                break;
            case 3:
            default:
                SHR_CLI_EXIT(_SHR_E_PARAM, "More than 1 \"=\" in the input\n");
                break;
        }

        sh_sand_arg->state |= SH_SAND_OPTION_PRESENT;

        if (sh_sand_option_value_get(unit, sh_sand_arg->type, &sh_sand_arg->state,
                                     option_value, &sh_sand_arg->param1, &sh_sand_arg->param2,
                                     sh_sand_arg->ext_ptr) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal value:\"%s\" for option:\"%s\" type:\"%s\"\n",
                         option_value, option_name, sal_field_type_str(sh_sand_arg->type));
        }

        utilex_str_split_free(tokens, realtokens);
        tokens = NULL;
    }

    RHITERATOR(sh_sand_arg, args_list)
    {
        /*
         * If option was present - great
         */
        if (sh_sand_arg->state & SH_SAND_OPTION_PRESENT)
        {
            continue;
        }
        /*
         * If not and its presence required - error
         */
        if (sh_sand_arg->requested == TRUE)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Presence of option:\"%s\" is requested\n", RHNAME(sh_sand_arg));
        }
        /*
         * If not and this is TYPE_PORT - Re-take default value for it
         */
        if (sh_sand_arg->type == SAL_FIELD_TYPE_PORT)
        {
            if (sh_sand_value_get(unit, sh_sand_arg->type, sh_sand_arg->def_str, &sh_sand_arg->def_param1,
                                  sh_sand_arg->ext_ptr) != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal Default value:\"%s\" for option:\"%s\"\n",
                             sh_sand_arg->def_str, RHNAME(sh_sand_arg));
            }
        }
    }

exit:
    if (tokens != NULL)
    {   /* Means we left the loop on error */
        utilex_str_split_free(tokens, realtokens);
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *      Analyze option and add the result to possible arguments list of specific command, represented by sand_args_list
 * \param [in] unit
 * \param [in] sand_args_list - allocated per leaf command and serving as possible arguments list
 * \param [in] option_p - pointer to option entry in either system or per command options array
 * \param [in] flags    - flags from command invocation
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval other errors as er shr_error_e * \param [in] option_p - pointer to option entry in either system or per command options array
 *
 * \remark
 *
 */
static shr_error_e
sh_sand_option_init(
    int unit,
    rhlist_t * sand_args_list,
    sh_sand_option_t * option_p,
    int flags)
{
    rhhandle_t void_arg;
    sh_sand_arg_t *sh_sand_arg;

    char *short_key, *keyword;

    SHR_FUNC_INIT_VARS(unit);

    /** coverity[copy_paste_error : FALSE] */
    SHR_CLI_EXIT_IF_ERR(sh_sand_keyword_fetch(option_p->keyword, &short_key, &keyword, flags), "");

    if (ISEMPTY(option_p->desc))
    {
        SHR_CLI_EXIT(_SHR_E_INIT, "Error - option:\"%s\" has no description\n", option_p->keyword);
    }
    if ((option_p->type == SAL_FIELD_TYPE_ENUM) && (option_p->ext_ptr == NULL))
    {
        SHR_CLI_EXIT(_SHR_E_INIT, "Error - enum option:\"%s\" requests enum list to be provided\n", option_p->keyword);
    }

    if (utilex_rhlist_entry_add_tail(sand_args_list, keyword, RHID_TO_BE_GENERATED, &void_arg) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add option:\"%s\" to the list\n", option_p->keyword);
    }
    sh_sand_arg = void_arg;
    sh_sand_arg->type = option_p->type;
    sh_sand_arg->ext_ptr = option_p->ext_ptr;
    sh_sand_arg->short_key = short_key;
    sh_sand_arg->def_str = option_p->def;
    if (sh_sand_arg->def_str != NULL)
    {
        if (sh_sand_arg->type != SAL_FIELD_TYPE_PORT)
        {       /* For PORT we obtain default in run time, no need to do it on init, it will not represent the actual
                 * port list */
            if (sh_sand_option_value_get(unit, sh_sand_arg->type, &sh_sand_arg->def_state, sh_sand_arg->def_str,
                                         &sh_sand_arg->def_param1, &sh_sand_arg->def_param2,
                                         sh_sand_arg->ext_ptr) != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal Default value:\"%s\" for option:\"%s\" type:\"%s\"\n",
                             sh_sand_arg->def_str, RHNAME(sh_sand_arg), sal_field_type_str(sh_sand_arg->type));
            }
        }
    }
    else
    {   /* 
         * Absence of default means that option presence is requested
         */
        sh_sand_arg->requested = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
sh_sand_option_list_init(
    int unit,
    sh_sand_option_t * options,
    rhlist_t ** sand_args_list_p,
    int flags)
{
    sh_sand_option_t *option_p;
    rhlist_t *sand_args_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sand_args_list_p, _SHR_E_PARAM, "sand_args_list_p");
    /*
     * If options == NULL, no command specific options provided, just system one will be relevant
     */
    if ((sand_args_list = utilex_rhlist_create("Options", sizeof(sh_sand_arg_t), 1)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for args list\n");
    }

    for (option_p = options; (option_p != NULL) && (option_p->keyword != NULL); option_p++)
    {
        sh_sand_option_t *cmp_option;
        /*
         * Verify that options are not the same as system ones
         */
        for (cmp_option = sh_sand_sys_options; cmp_option->keyword != NULL; cmp_option++)
        {
            if (!sal_strcasecmp(option_p->keyword, cmp_option->keyword))
            {
                SHR_CLI_EXIT(_SHR_E_INIT, "Error - option:\"%s\" is system one\n", option_p->keyword);
            }
        }

        SHR_CLI_EXIT_IF_ERR(sh_sand_option_init(unit, sand_args_list, option_p, flags), "");
    }
    /*
     * Add all system options to the list
     */
    for (option_p = sh_sand_sys_options; option_p->keyword != NULL; option_p++)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_option_init(unit, sand_args_list, option_p, flags), "");
    }

exit:
    if ((sand_args_list != NULL) && !SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        utilex_rhlist_free_all(sand_args_list);
    }
    else
    {
        if (sand_args_list_p != NULL)
        {
            *sand_args_list_p = sand_args_list;
        }
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_cmd_fetch(
    rhlist_t * command_list,
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

shr_error_e
sh_sand_cmd_traverse(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_traverse_cb_t sh_sand_action,
    void *flex_ptr,
    int flags)
{
    sh_sand_cmd_t *sh_sand_cmd;
    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
        /** coverity[copy_paste_error : FALSE] */
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: Command list is NULL in traverse\n");
    }
    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
    {
        if (sh_sand_cmd->cmd != NULL)
        {
            SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse(unit, args, sh_sand_cmd->cmd, sh_sand_action, flex_ptr, flags),
                                "Error on traversing:%s\n", sh_sand_cmd->keyword);
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
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_cmd_t ** sh_sand_cmd_p)
{
    sh_sand_cmd_t *sh_sand_cmd;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sh_sand_cmd_p, _SHR_E_PARAM, "sh_sand_cmd_p");

    if (ISEMPTY(cmd_name))
    {
        cmd_name = "usage";
    }
    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: Command list is NULL for:%s\n", cmd_name);
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
    {
        /*
         * Call to the first command that is supported and match the cmd_name
         */
        if (!sal_strcasecmp(cmd_name, sh_sand_cmd->keyword) ||
            (!ISEMPTY(sh_sand_cmd->short_key) && (!sal_strcasecmp(cmd_name, sh_sand_cmd->short_key))))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            break;
        }
    }
    /*
     * Now try command prefix substring
     */
    if ((sh_sand_cmd->keyword == NULL) && (sal_strlen(cmd_name) > 1))
    {
        for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
        {
            if (sh_sand_cmd->keyword == sal_strcasestr(sh_sand_cmd->keyword, cmd_name))
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                break;
            }
        }
    }

    *sh_sand_cmd_p = sh_sand_cmd;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_cmds_init(
    int unit,
    char *command,
    rhlist_t ** sand_cmds_list_p)
{
    rhlist_t *sand_cmds_list = NULL;
    char **cmd_tokens = NULL;
    uint32 cmd_num = 0;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Fill command list from command string 
     */
    if ((sand_cmds_list = utilex_rhlist_create("Commands", sizeof(rhentry_t), 1)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for control command list\n");
    }

    if ((cmd_tokens = utilex_str_split(command, " ", SH_SAND_MAX_COMMAND_DEPTH, &cmd_num)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\n");
    }
    /*
     * If there is no = in the token, for one occurrence we assume that it is "variable" option 
     */

    for (i = 0; i < cmd_num; i++)
    {
        rhhandle_t command_handle;
        if (utilex_rhlist_entry_add_tail(sand_cmds_list,
                                         cmd_tokens[i], RHID_TO_BE_GENERATED, &command_handle) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Cannot add command:%s to command list\n", cmd_tokens[i]);
        }
    }

exit:
    if ((sand_cmds_list != NULL) && !SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        utilex_rhlist_free_all(sand_cmds_list);
    }
    else
    {
        if (sand_cmds_list_p != NULL)
        {
            *sand_cmds_list_p = sand_cmds_list;
        }
        else
        {
            utilex_rhlist_free_all(sand_cmds_list);
        }
    }
    if (cmd_tokens != NULL)
    {
        utilex_str_split_free(cmd_tokens, cmd_num);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_act(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_cmd_t * sh_sys_cmd_a)
{
    char *cmd_name;
    sh_sand_cmd_t *sh_sand_cmd = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: No further command provided\n");
    }

    /*
     * Get command name
     */
    cmd_name = ARG_GET(args);

    /*
     * Now we can move after command word
     */
    SHR_SET_CURRENT_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sys_cmd_a, &sh_sand_cmd));
    if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
    {   /** If not found in system list look in local one - not found - error   */
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sand_cmd_a, &sh_sand_cmd),
                            "ERROR: command \"%s\" is not supported\n", cmd_name);
    }
    else
    {   /* 
         * System command found - fill pointer to upstream array
         */
        sh_sand_cmd->ctr.sh_sand_cmd_a = sh_sand_cmd_a;
        sh_sand_cmd->ctr.sh_sys_cmd_a = sh_sys_cmd_a;
        sh_sand_cmd->ctr.sh_sand_cmd = NULL;
    }
    if (sh_sand_cmd->cmd != NULL)
    {   /* 
         * Highly improbable but possible to have nested system commands, so basic assumption here is that command is regular one
         */
        SHR_SET_CURRENT_ERR(sh_sand_act(unit, args, sh_sand_cmd->cmd, sh_sys_cmd_a));
    }
    else if (sh_sand_cmd->action)
    {
        
        if ((cmd_name = ARG_CUR(args)) != NULL)
        {
            sh_sand_cmd_t *sh_sand_cmd_sys = NULL;

            SHR_SET_CURRENT_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sys_cmd_a, &sh_sand_cmd_sys));
            if (SHR_FUNC_VAL_IS(_SHR_E_NONE))
            {
                ARG_NEXT(args);
                sh_sand_cmd_sys->ctr.sh_sand_cmd_a = NULL;
                sh_sand_cmd_sys->ctr.sh_sys_cmd_a = sh_sys_cmd_a;
                /*
                 * Record base command
                 */
                sh_sand_cmd_sys->ctr.sh_sand_cmd = sh_sand_cmd;
                /*
                 * Move to system one to preserve the general flow
                 */
                sh_sand_cmd = sh_sand_cmd_sys;
            }
        }
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_CLI_EXIT_IF_ERR(sh_sand_option_list_process
                            (unit, args, sh_sand_cmd->ctr.stat_args_list, sh_sand_cmd->flags, sh_sand_cmd), "");
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd->action(unit, args, &sh_sand_cmd->ctr), "");
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: command:%s had neither leaf nor branch\n", cmd_name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_act_all(
    int unit,
    args_t * args,
    char *cmd_name,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_cmd_t * sh_sys_cmd_a)
{
    sh_sand_cmd_t *sh_sand_cmd = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: No further command provided\n");
    }

    /*
     * Now we can move after command word
     */
    SHR_SET_CURRENT_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sys_cmd_a, &sh_sand_cmd));
    if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
    {   /** If not found in system list look in local one - not found - error   */
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: %s command not found\n", cmd_name);
    }
    /*
     * System command found - fill pointer to upstream array
     */
    sh_sand_cmd->ctr.sh_sand_cmd_a = sh_sand_cmd_a;
    sh_sand_cmd->ctr.sh_sys_cmd_a = sh_sys_cmd_a;
    sh_sand_cmd->ctr.sh_sand_cmd = NULL;

    /*
     * Perform system command on the entire tree
     */
    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd->action(unit, args, &sh_sand_cmd->ctr), "");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_verify_man(
    char *command,
    sh_sand_man_t * man)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    if ((man == NULL) || (man->brief == NULL) || (man->full == NULL) || (man->examples == NULL)
        || (man->synopsis == NULL))
    {
        LOG_CLI((BSL_META("Bad \"usage\" for command:%s\n"), command));
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_test_list_init(
    int unit,
    sh_sand_invoke_t * invokes,
    rhlist_t ** sand_args_list_p)
{
    sh_sand_invoke_t *invoke;
    rhlist_t *sand_args_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (invokes == NULL)
    {
/*
 * Legitimate case of test list empty, which points the the fact that there is only one default test or
 * that tests will be added dynamically later on
 */
        SHR_EXIT();
    }

    if (sand_args_list_p == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR - No pointer for args list provided\n");
    }
    sand_args_list = *sand_args_list_p;

    /*
     * List may be already non NULL, if dynamic tests were added 
     */
    if (sand_args_list == NULL)
    {
        if ((sand_args_list = utilex_rhlist_create("Test", sizeof(sh_sand_test_t), 0)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Failed to allocate list for test args\n");
        }
        *sand_args_list_p = sand_args_list;
    }

    for (invoke = invokes; invoke->name != NULL; invoke++)
    {
        sh_sand_test_t *test_invoke;
        rhhandle_t void_test;
        int param_len = sal_strlen(invoke->params);

        if (utilex_rhlist_entry_add_tail(sand_args_list, invoke->name, RHID_TO_BE_GENERATED, &void_test) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add test:\"%s\"\n"
                         "Test Name should be unique per command\n", invoke->name);
        }
        test_invoke = void_test;
        test_invoke->flags = invoke->flags;
        if ((test_invoke->params = sal_alloc(param_len + 1, "dnx")) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Error - failed to allocate memory for test params on:\"%s\" to the list\n",
                         invoke->name);
        }
        sal_strncpy(test_invoke->params, invoke->params, param_len);
        test_invoke->params[param_len] = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_test_add(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    char *cmd_name,
    char *test_name,
    char *test_params,
    int flags)
{
    sh_sand_cmd_t *sh_sand_cmd;
    sh_sand_test_t *test_invoke;
    rhhandle_t void_test;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: No further command provided\n");
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sand_cmd_a, &sh_sand_cmd), "");

    if (sh_sand_cmd->ctr.test_list == NULL)
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
    if ((test_invoke->params = sal_alloc(sal_strlen(test_params) + 1, "dnx")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Error - failed to allocate memory for test params on:\"%s\" to the list\n",
                     test_name);
    }
    sal_strcpy(test_invoke->params, test_params);
    test_invoke->flags = flags;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_test_remove(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    char *cmd_name,
    char *test_name)
{
    sh_sand_cmd_t *sh_sand_cmd;
    sh_sand_test_t *test_invoke;
    rhhandle_t void_test;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: No further command provided\n");
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sand_cmd_a, &sh_sand_cmd), "");

    if (sh_sand_cmd->ctr.test_list == NULL)
    {
        /*
         * No test list - nothing to remove leave peacefully
         */
        SHR_EXIT();
    }

    if ((void_test = utilex_rhlist_entry_get_by_name(sh_sand_cmd->ctr.test_list, test_name)) != NULL)
    {
        test_invoke = void_test;
        if ((test_invoke = void_test) != NULL)
        {
            sal_free(test_invoke->params);
        }
        utilex_rhlist_entry_del_free(sh_sand_cmd->ctr.test_list, void_test);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_init(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    char *command,
    int flags)
{
    sh_sand_cmd_t *sh_sand_cmd;
    SHR_FUNC_INIT_VARS(unit);

    sh_sand_keyword_list_init();
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
         */
        SHR_CLI_EXIT_IF_ERR(sh_sand_keyword_fetch(sh_sand_cmd->keyword, &sh_sand_cmd->short_key,
                                                  &sh_sand_cmd->full_key, flags), "Failure to verify command:\"%s\"\n",
                            command);

        if ((sh_sand_cmd->cmd == NULL) && (sh_sand_cmd->action == NULL))
        {
            LOG_CLI((BSL_META("ERROR - command:%s is neither leaf nor branch\n"), command));
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        }
        else
        {
            if (sh_sand_cmd->action)
            {   /* Command is leaf, verify that we have correct usage and options */
                if (flags & SH_SAND_VERIFY_MAN)
                {
                    SHR_CLI_EXIT_IF_ERR(sh_sand_verify_man(command, sh_sand_cmd->man), "");
                }
                SHR_CLI_EXIT_IF_ERR(sh_sand_option_list_init
                                    (unit, sh_sand_cmd->options, &sh_sand_cmd->ctr.stat_args_list, flags),
                                    "Error in option list for command:\"%s\"\n", command);
                /*
                 * Fill command list from command string 
                 */
                SHR_CLI_EXIT_IF_ERR(sh_sand_cmds_init(unit, command, &sh_sand_cmd->ctr.command_list),
                                    "Failure to extract command list from command:\"%s\"\n", command);
                /*
                 * Initialize test_list from static invokes list 
                 */
                SHR_CLI_EXIT_IF_ERR(sh_sand_test_list_init(unit, sh_sand_cmd->invokes, &sh_sand_cmd->ctr.test_list),
                                    "Failure to initiate test list for command:\"%s\"\n", command);
            }
            if (sh_sand_cmd->cmd)
            {   /* Command is branch got into recursive call */
                SHR_CLI_EXIT_IF_ERR(sh_sand_init(unit, sh_sand_cmd->cmd, command, flags), "");
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

    /*
     * Iterate over command array
     */
    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd->keyword != NULL; sh_sand_cmd++)
    {
        if (sh_sand_cmd->action)
        {
            if (sh_sand_cmd->ctr.stat_args_list)
            {
                utilex_rhlist_free_all(sh_sand_cmd->ctr.stat_args_list);
                sh_sand_cmd->ctr.stat_args_list = NULL;
            }

            if (sh_sand_cmd->ctr.command_list)
            {
                utilex_rhlist_free_all(sh_sand_cmd->ctr.command_list);
                sh_sand_cmd->ctr.command_list = NULL;
            }

            if (sh_sand_cmd->ctr.test_list)
            {
                utilex_rhlist_free_all(sh_sand_cmd->ctr.test_list);
                sh_sand_cmd->ctr.test_list = NULL;
            }
        }
        if (sh_sand_cmd->cmd)
        {       /* Command is branch got into recursive call */
            /** coverity[copy_paste_error : FALSE] */
            SHR_CLI_EXIT_IF_ERR(sh_sand_deinit(unit, sh_sand_cmd->cmd), "Deinit Failure");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef INCLUDE_AUTOCOMPLETE
/* fill the shell autocomplete tree with the commands, sub commands and options */

void
sh_sand_cmd_autocomplete_init(
    int unit,
    autocomplete_node_t * parent,
    sh_sand_cmd_t * cmd)
{

    sh_sand_cmd_t *child_cmd = cmd;

    for (; child_cmd != NULL && child_cmd->keyword != NULL; child_cmd++)
    {
        /*
         * add a node to the autocomplete tree with the keyword as the string and the upper command as a parent
         */
        autocomplete_node_t *ac_node = autocomplete_node_add(unit, parent, child_cmd->keyword);
        if (child_cmd->cmd == NULL)
        {
            sh_sand_option_t *option = child_cmd->options;
            for (; option != NULL && option->keyword != NULL; option++)
            {
                char option_text[AUTOCOMPLETE_MAX_STRING_LEN] = { 0 };
                sal_strncat(option_text, option->keyword, AUTOCOMPLETE_MAX_STRING_LEN - sal_strlen(option_text));
                sal_strncat(option_text, "=", AUTOCOMPLETE_MAX_STRING_LEN - sal_strlen(option_text));
                autocomplete_node_add(unit, ac_node, option_text);
            }
        }
        else
        {
            sh_sand_cmd_autocomplete_init(unit, ac_node, child_cmd->cmd);
        }
    }

    if (parent != NULL && cmd != NULL && cmd->keyword != NULL)
    {
        autocomplete_node_add(unit, parent, "usage");
    }
}

void
sh_sand_cmd_autocomplete_deinit(
    int unit,
    sh_sand_cmd_t * cmd)
{
    sh_sand_cmd_t *child_cmd = cmd;

    for (; child_cmd != NULL && child_cmd->keyword != NULL; child_cmd++)
    {
        autocomplete_node_t *ac_node = autocomplete_find_root(unit, child_cmd->keyword);
        autocomplete_node_delete(unit, ac_node);
    }
}

#endif /* INCLUDE_AUTOCOMPLETE */
