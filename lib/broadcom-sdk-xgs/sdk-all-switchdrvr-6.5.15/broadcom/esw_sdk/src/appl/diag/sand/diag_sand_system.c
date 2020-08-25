/**
 * \file diag_sand_system.c
 *
 * System commands for dnx/dnxf shells
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <sal/appl/sal.h>

#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/drv.h>

#include <appl/diag/bslenable.h>
#include <appl/diag/parse.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_system.h>

#include <ibde.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager.h>
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * Maximum size of argument string in usage before preempting description
 */
#define SH_SAND_ARGUMENT_PREFIX_MAX     48
/*
 * Number of non-functional characters in argument output format
 */
#define SH_SAND_ARGUMENT_PREFIX_BASE    4
/*
 * Number of characters added to argument prefix if it is free one
 */
#define SH_SAND_ARGUMENT_FREE_TAG_SIZE  5

/**
 * \brief
 *      Looks for specified argument and excludes it from the example
 * \param [in] unit - unit id
 * \param [in] arguments_str - string of single example
 * \param [in] argument      - argument to be excluded from example
 * \param [in] value_substring - exclude only if argument value has this string
 *
 * \retval
 *     _SHR_E_EXISTS - if argument exists but does not contain value substring
 *     _SHR_E_NONE in any other case
 */
static shr_error_e
sh_sand_exclude_argument(
    int unit,
    char *arguments_str,
    char *argument,
    char *value_substring)
{
    int k_cur, arg_offset;
    char *arg_ptr, *space_ptr;
    char *value_substring_found;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Find if there is an argument in the string
     */
    if ((arg_ptr = sal_strcasestr(arguments_str, argument)) == NULL)
    {
        SHR_EXIT();
    }
    /*
     * Offset of the place where argument starts
     */
    arg_offset = arg_ptr - arguments_str;
    /*
     * Verify that argument is either at the beginning of the line or there is ' ' before it
     * If not - there was other name with this argument name being part of it
     */
    if ((arg_offset != 0) && (arguments_str[arg_offset - 1] != ' '))
    {
        SHR_EXIT();
    }

    /*
     * Found the space after the argument
     */
    space_ptr = sal_strcasestr(arg_ptr, " ");
    /*
     * We found the argument according to the name
     * If the argument value substring is provided, check that it is contained in value
     * If the value_substring is not found - leave, argument was used with something else
     */
    if ((value_substring != NULL) && (value_substring_found = sal_strcasestr(arg_ptr, value_substring)) == NULL)
    {
        /*
         * If there no space or space_ptr is less than found value_ptr - we can leave
         */
        if ((space_ptr == NULL) || (value_substring_found < space_ptr))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EXISTS);
            SHR_EXIT();
        }
    }

    if (space_ptr != NULL)
    {
        /*
         * Copy all after ' ' to the place of file and NULL terminate it
         */
        for (k_cur = 0; space_ptr[1 + k_cur] != 0; k_cur++)
        {
            arg_ptr[k_cur] = space_ptr[1 + k_cur];
        }
        arg_ptr[k_cur] = 0;
    }
    else if (arg_offset == 0)
    {
        /*
         * No space after the argument - just NULL terminate the source to annihilate the argument,
         */
        arguments_str[arg_offset] = 0;
    }
    else
    {
        /*
         * if argument was not first terminate on space before argument
         */
        arguments_str[arg_offset - 1] = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * List all defined tests for node or leaf
 * \param [in] sh_sand_cmd - pointer to command which will generate output
 * \param [in] folder      - relative path to folder where xml file will be saved
 * \param [in] filename    - pointer to allocated space where resulting test name will be formed
 *
 * \remark
 *     Routine does not return the failure - if something went wrong test will fail
 */
void
sh_sand_fill_output_name(
    sh_sand_cmd_t * sh_sand_cmd,
    char *folder,
    char *filename)
{
    /*
     * Currently all dumps will start from shell_ - we may change it if we need
     */
    sal_snprintf(filename, SH_SAND_MAX_TOKEN_SIZE - 1, "%s/shell_", folder);
    /*
     * Each command contain space separated command line, append entire command chain (spaces will be turned to _ below
     */
    sal_strncat(filename, sh_sand_cmd->cmd_only, SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(filename));
    /*
     * Finally append xml extension
     */
    sal_strncat(filename, ".xml", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(filename));
    /*
     * Replace all non-compliant characters in the test name by underscore
     */
    utilex_str_escape(filename, '_');
    /*
     * Make it all lower case to ease the view
     */
    utilex_str_to_lower(filename);
    return;
}

/**
 * \brief Routine serves to generate array of examples per leaf command
 * \param [in]  unit           - unit id
 * \param [in]  sh_sand_cmd    - pointer to leaf command
 * \param [out] examples_p     - pointer to the examples string array
 * \param [out] examples_num_p - pointer to the number of element in examples array
 * \retval _SHR_E_NONE for success
 * \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 * \retval other errors for other failure type
 * \remark
 *    If no examples string was provided in man structure, there will be only 1 element with option-less command
 *    Responsibility of caller to free the array using utilex_str_split_free
 */

static shr_error_e
sh_sand_examples_get(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    char ***examples_p,
    uint32 *examples_num_p)
{
    int i_tok;
    char **tokens = NULL;
    uint32 maxtokens = 100;
    char **examples = NULL;
    uint32 examples_num;
    int str_size;
    SHR_FUNC_INIT_VARS(unit);

    if ((sh_sand_cmd == NULL) || (sh_sand_cmd->man == NULL))
    {   /* On this stage there is no need to print message, all errors should be rectified by verify */
        SHR_EXIT();
    }

    if (!ISEMPTY(sh_sand_cmd->man->examples))
    {
        /*
         * If examples string is not empty, split it into separate options strings
         */
        if ((tokens = utilex_str_split(sh_sand_cmd->man->examples, "\n", maxtokens, &examples_num)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "failed to parse example string %s\n", sh_sand_cmd->man->examples);
        }
        /*
         * Allocate pointer array for full commands
         */
        if ((examples = sal_alloc(sizeof(char *) * examples_num, "examples")) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memory for examples:\"%s\"\n", sh_sand_cmd->man->examples);
        }
        for (i_tok = 0; i_tok < examples_num; i_tok++)
        {
            examples[i_tok] = NULL;
        }
        for (i_tok = 0; i_tok < examples_num; i_tok++)
        {
            /*
             * Go through all options and create full commands
             */
            str_size = sal_strlen(tokens[i_tok]) + sal_strlen(sh_sand_cmd->cmd_only) + 3;
            if ((examples[i_tok] = sal_alloc(str_size, "example")) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memory for examples:\"%s\"\n",
                             sh_sand_cmd->man->examples);
            }

            sal_snprintf(examples[i_tok], str_size - 1, "%s %s", sh_sand_cmd->cmd_only, tokens[i_tok]);
        }
    }
    else
    {
        /*
         * If examples string is not defined, only item will be command string itself without options
         */
        examples_num = 1;
        /*
         * Allocate pointer array for full commands
         */
        if ((examples = sal_alloc(sizeof(char *) * examples_num, "examples")) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memory for examples of:\"%s\"\n", sh_sand_cmd->keyword);
        }
        str_size = sal_strlen(sh_sand_cmd->cmd_only) + 1;
        if ((examples[0] = sal_alloc(str_size, "example")) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memory for examples:\"%s\"\n", sh_sand_cmd->keyword);
        }
        sal_strncpy(examples[0], sh_sand_cmd->cmd_only, str_size - 1);
        examples[0][str_size - 1] = 0;
    }

    *examples_p = examples;
    *examples_num_p = examples_num;

exit:
    if (tokens != NULL)
    {
        utilex_str_split_free(tokens, examples_num);
    }
    if (!SHR_FUNC_VAL_IS(_SHR_E_NONE) && (examples != NULL))
    {
        utilex_str_split_free(examples, examples_num);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief Routine serves to generate array of examples per leaf command
 * \param [in]  unit           - unit id
 * \param [in]  sh_sand_cmd    - pointer to leaf command
 * \param [out] synopsis_p     - pointer to the synopsis string array
 * \param [out] synopsis_num_p - pointer to the number of element in synopsis array
 * \retval _SHR_E_NONE for success
 * \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 * \retval other errors for other failure type
 * \remark
 *    If no synopsis string was provided in man structure, there will be only 1 element with option-less command
 *    Responsibility of caller to free the array using utilex_str_split_free
 */
static shr_error_e
sh_sand_synopsis_get(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    char ***synopsis_p,
    uint32 *synopsis_num_p)
{
    int i_tok;
    char **tokens = NULL;
    uint32 maxtokens = 100;
    char **synopsis = NULL;
    uint32 synopsis_num;
    int str_size;
    SHR_FUNC_INIT_VARS(unit);

    if ((sh_sand_cmd == NULL) || (sh_sand_cmd->man == NULL))
    {   /* On this stage there is no need to print message, all errors should be rectified by verify */
        SHR_EXIT();
    }

    if (!ISEMPTY(sh_sand_cmd->man->synopsis))
    {
        /*
         * If synopsis string is not empty, split it into separate options strings
         */
        if ((tokens = utilex_str_split(sh_sand_cmd->man->synopsis, "\n", maxtokens, &synopsis_num)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "failed to parse synopsis string:\"%s\"\n", sh_sand_cmd->man->synopsis);
        }
        /*
         * Allocate pointer array for full commands
         */
        if ((synopsis = sal_alloc(sizeof(char *) * synopsis_num, "synopsis")) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memory for synopsis:\"%s\"\n", sh_sand_cmd->man->synopsis);
        }
        for (i_tok = 0; i_tok < synopsis_num; i_tok++)
        {
            synopsis[i_tok] = NULL;
        }
        for (i_tok = 0; i_tok < synopsis_num; i_tok++)
        {
            /*
             * Go through all options and create full commands
             */
            str_size = sal_strlen(tokens[i_tok]) + sal_strlen(sh_sand_cmd->cmd_only) + 3;
            if ((synopsis[i_tok] = sal_alloc(str_size, "synopsis_item")) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memory for synopsis:\"%s\"\n",
                             sh_sand_cmd->man->synopsis);
            }

            sal_snprintf(synopsis[i_tok], str_size - 1, "%s %s", sh_sand_cmd->cmd_only, tokens[i_tok]);
        }
    }
    else
    {
        sh_sand_option_t *option_p;
        char option_only[RHSTRING_MAX_SIZE];
        char *format_str;
        char *cmd_str;

        /*
         * If synopsis string is not defined, it will be auto-generated
         */
        synopsis_num = 1;
        /*
         * Allocate pointer array for full commands
         */
        if ((synopsis = sal_alloc(sizeof(char *) * synopsis_num, "synopsis")) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memory for synopsis:\"%s\"\n", sh_sand_cmd->man->synopsis);
        }
        if ((cmd_str = sal_alloc(SH_SAND_MAX_TOKEN_SIZE, "synopsis_item")) == NULL)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
            SHR_EXIT();
        }
        /*
         * Copy the command and then add options
         */
        sal_strncpy(cmd_str, sh_sand_cmd->cmd_only, SH_SAND_MAX_TOKEN_SIZE - 1);
        /*
         * If synopsis was not provided auto-generate one from options
         */
        for (option_p = sh_sand_cmd->options; (option_p != NULL) && (option_p->keyword != NULL); option_p++)
        {
            switch (option_p->type)
            {
                case SAL_FIELD_TYPE_BOOL:
                    format_str = "[=no]";
                    break;
                default:
                    format_str = "=<...>";
                    break;
            }
            if (option_p->def == NULL)
            {   /** The one without default is must */
                sal_snprintf(option_only, RHSTRING_MAX_SIZE - 1, " %s%s", option_p->keyword, format_str);
            }
            else
            {   /** When default is present cli parameter is optional */
                sal_snprintf(option_only, RHSTRING_MAX_SIZE - 1, " [%s%s]", option_p->keyword, format_str);
            }
            sal_strncpy(cmd_str + strlen(cmd_str), " ", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(cmd_str));
            sal_strncpy(cmd_str + strlen(cmd_str), option_only, SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(cmd_str));
        }

        synopsis[0] = cmd_str;
    }

    *synopsis_p = synopsis;
    *synopsis_num_p = synopsis_num;

exit:
    if (tokens != NULL)
    {
        utilex_str_split_free(tokens, synopsis_num);
    }
    if (!SHR_FUNC_VAL_IS(_SHR_E_NONE) && (synopsis != NULL))
    {
        utilex_str_split_free(synopsis, synopsis_num);
    }
    SHR_FUNC_EXIT;
}

static void
sh_sand_print(
    char *string,
    int left_margin,
    int right_margin,
    int term_width)
{
    int token_size;
    int str_shift, print_size;
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
         * If the last character in string to be printed is \n - do not double it, so print 1 character less
         */
        print_size = (*(string + str_offset + str_shift - 1) == '\n') ? (str_shift - 1) : (str_shift);

        LOG_CLI((BSL_META("%.*s\n"), print_size, string + str_offset));
        /*
         * Update current string offset taking into account delimiter
         */
        str_offset += str_shift;
    }
    while (str_offset < str_size);      /* once current offset exceeds string length stop */
}

static int
sh_sand_argument_prefix_size(
    sh_sand_option_t * options)
{
    char *def_str;
    sh_sand_option_t *option;
    int lead_size = SH_SAND_ARGUMENT_PREFIX_BASE, cur_size;
    /*
     * Calculate the longest string consisting from option name type and default
     */
    for (option = options; !ISEMPTY(option->keyword); option++)
    {
        if (option->def == NULL)
        {
            def_str = "NONE";
        }
        else if (ISEMPTY(option->def))
        {
            def_str = "EMPTY";
        }
        else
        {
            def_str = option->def;
        }

        cur_size = SH_SAND_ARGUMENT_PREFIX_BASE + sal_strlen(option->keyword) +
                                                  sal_strlen(sal_field_type_str(option->type)) + sal_strlen(def_str);

        if(option->flags & SH_SAND_ARG_FREE)
        {
            cur_size += SH_SAND_ARGUMENT_FREE_TAG_SIZE;
        }
        /*
         * Don't touch lead_size if cur_size is more than certain max, otherwise increase is up to highest but not
         * bigger than this max
         */
        lead_size = (cur_size > 48) ? lead_size : ((cur_size > lead_size) ? cur_size : lead_size);
    }
    return lead_size;
}

static void
sh_sand_print_arguments(
    int unit,
    sh_sand_option_t * options,
    int left_margin,
    int right_margin,
    int term_width)
{
    int token_size;
    int str_shift;
    int str_offset;
    int str_size;
    char *def_str, *keyword;
    char lead_str[RHSTRING_MAX_SIZE];
    char format_str[RHKEYWORD_MAX_SIZE];
    sh_sand_option_t *option;
    sh_sand_enum_t *enum_entry;
    int lead_size;

    diag_sand_prt_char(left_margin, ' ');
    LOG_CLI((BSL_META("\033[1m%s\033[0m\n\n"), "Argument (type:default) - description"));

    lead_size = sh_sand_argument_prefix_size(options);

    sal_snprintf(format_str, RHKEYWORD_MAX_SIZE, "%s-%ds", "%", lead_size);

    for (option = options; option->keyword != NULL; option++)
    {
        if (option->def == NULL)
        {
            def_str = "NONE";
        }
        else if (ISEMPTY(option->def))
        {
            def_str = "EMPTY";
        }
        else
        {
            def_str = option->def;
        }

        if (option->full_key != NULL)
        {
            keyword = option->full_key;
        }
        else
        {
            keyword = option->keyword;
        }

        if(option->flags & SH_SAND_ARG_FREE)
        {
            sal_snprintf(lead_str, RHSTRING_MAX_SIZE - 1, "%s (free,%s:%s)", keyword, sal_field_type_str(option->type), def_str);
        }
        else
        {
            sal_snprintf(lead_str, RHSTRING_MAX_SIZE - 1, "%s (%s:%s)", keyword, sal_field_type_str(option->type), def_str);
        }

        diag_sand_prt_char(left_margin, ' ');
        cli_out(format_str, lead_str);

        token_size = term_width - (left_margin + 3 + lead_size) - right_margin;
        str_size = strlen(option->desc);
        str_offset = 0;
        /*
         * If lead size if too long - move to the next line and take shift
         */
        if (sal_strlen(lead_str) > lead_size)
        {
            cli_out("\n");
            diag_sand_prt_char(left_margin + lead_size, ' ');
        }
        cli_out(" - ");
        do
        {
            if (str_offset != 0)
            {
                /*
                 * Print left margin
                 */
                diag_sand_prt_char(left_margin + 3 + lead_size, ' ');
            }
            /*
             * Get shift where last white space in the token or first new line are situated
             */
            str_shift = utilex_str_get_shift(option->desc + str_offset, token_size);
            /*
             * Print string up to this delimiter, print only specified number of characters(str_shift) from the string
             */
            LOG_CLI((BSL_META("%.*s\n"), str_shift, option->desc + str_offset));
            /*
             * Update current string offset taking into account delimiter
             */
            str_offset += str_shift;
        }
        while (str_offset < str_size);  /* once current offset exceeds string length stop */

        enum_entry = (sh_sand_enum_t *) (option->ext_ptr);
        if (!ISEMPTY(option->valid_range))
        {
            char start_str[RHNAME_MAX_SIZE], end_str[RHNAME_MAX_SIZE];
            if (sh_sand_option_valid_range_get(unit, option, start_str, end_str) == _SHR_E_NONE)
            {
                diag_sand_prt_char(left_margin + 3, ' ');
                LOG_CLI((BSL_META("\033[1m%s\033[0m\n"), "Valid Range"));

                diag_sand_prt_char(left_margin + 3, ' ');
                LOG_CLI((BSL_META("%-10s - from:%s to:%s\n"), "", start_str, end_str));
            }
        }
        if (enum_entry != NULL)
        {
            diag_sand_prt_char(left_margin + 3, ' ');
            if (option->type == SAL_FIELD_TYPE_ENUM)
            {
                LOG_CLI((BSL_META("\033[1m%s\033[0m\n"), "Available Values"));
            }
            else
            {
                LOG_CLI((BSL_META("\033[1m%s\033[0m\n"), "Predefined Values"));
            }

            for (; !ISEMPTY(enum_entry->string); enum_entry++)
            {
                diag_sand_prt_char(left_margin + 3, ' ');
                if (ISEMPTY(enum_entry->desc))
                {
                    LOG_CLI((BSL_META("%-10s - %s\n"), enum_entry->string, "TBD"));
                }
                else
                {
                    LOG_CLI((BSL_META("%-10s - %s\n"), enum_entry->string, enum_entry->desc));
                }
            }
        }
    }
}

static shr_error_e
sh_sand_usage_error(
    int unit,
    shell_flex_t * shell_flex_p,
    char *command_n,
    char *option_n,
    char *error_str,
    int *first)
{
    prt_control_t *prt_ctr = NULL;
    SHR_FUNC_INIT_VARS(unit);

    if (shell_flex_p != NULL)
    {
        shell_flex_p->failed++;
        prt_ctr = shell_flex_p->prt_ctr;
    }

    if (prt_ctr != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        if ((*first) == 0)
        {
            PRT_CELL_SKIP(1);
        }
        else
        {
            *first = 0;
            PRT_CELL_SET("%s", command_n);
        }
        PRT_CELL_SET("%s", option_n);
        PRT_CELL_SET("%s", error_str);
    }
    else
    {
        LOG_CLI((BSL_META("cmd:'%s' argument:'%s' - '%s'"), command_n, option_n, error_str));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_usage_verify(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *flex_ptr,
    int flags)
{
    sh_sand_man_t *man;
    sh_sand_option_t *option;
    sh_sand_enum_t *enum_entry;
    shell_flex_t *shell_flex_p = (shell_flex_t *) flex_ptr;
    int first = 1;
    char error_str[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if ((man = sh_sand_cmd->man) == NULL)
    {
        sh_sand_usage_error(unit, shell_flex_p, sh_sand_cmd->cmd_only, "", "No manual", &first);
        SHR_EXIT();
    }
    else
    {
        int length;
        if (ISEMPTY(man->brief))
        {
            sh_sand_usage_error(unit, shell_flex_p, sh_sand_cmd->cmd_only, "", "Empty Brief Description", &first);
        }
        else if (flags & SH_CMD_ALL)
        {
            if ((length = sal_strlen(man->brief)) >= RHSTRING_MAX_SIZE)
            {
                sal_snprintf(error_str, RHSTRING_MAX_SIZE - 1,
                             "Brief size:%d is longer than:%d", length, RHSTRING_MAX_SIZE);
                sh_sand_usage_error(unit, shell_flex_p, sh_sand_cmd->cmd_only, "", error_str, &first);
            }
            else if (length < RHKEYWORD_MAX_SIZE)
            {
                sal_snprintf(error_str, RHSTRING_MAX_SIZE - 1,
                             "Brief size:%d is less than:%d", length, RHKEYWORD_MAX_SIZE);
                sh_sand_usage_error(unit, shell_flex_p, sh_sand_cmd->cmd_only, "", error_str, &first);
            }
            /** Do not check full description for branches */
            if ((sh_sand_cmd->child_cmd_a != NULL) && ISEMPTY(man->full))
            {
                sh_sand_usage_error(unit, shell_flex_p, sh_sand_cmd->cmd_only, "", "No full description", &first);
            }
        }
    }

    if ((option = sh_sand_cmd->options) != NULL)
    {
        for (; option->keyword != NULL; option++)
        {
            char *option_ptr;
            /*
             * Verify description of command, option and enum
             */
            if (ISEMPTY(option->desc))
            {
                sh_sand_usage_error(unit, shell_flex_p, sh_sand_cmd->cmd_only, option->keyword, "No description",
                                    &first);
            }

            for (enum_entry = (sh_sand_enum_t *) (option->ext_ptr); enum_entry && enum_entry->string; enum_entry++)
            {
                if (ISEMPTY(enum_entry->desc))
                {
                    sal_snprintf(error_str, RHSTRING_MAX_SIZE - 1, "No description for enum:%s", enum_entry->string);
                    sh_sand_usage_error(unit, shell_flex_p, sh_sand_cmd->cmd_only, option->keyword, error_str, &first);
                }
            }
            /*
             * Do not check presence for free variables or
             * the ones that were specifically marked as not participating in examples for any reason
             */
            if (option->flags & (SH_SAND_ARG_FREE | SH_SAND_ARG_QUIET))
                continue;
            /*
             * No check if argument is present either in full or short form and is not occasional part of other word
             */
            if (ISEMPTY(man->examples) ||
                ((sal_strcasestr(man->examples, option->keyword) == NULL) &&
                 ((ISEMPTY(option->short_key)) ||
                  ((option_ptr = sal_strcasestr(man->examples, option->short_key)) == NULL) ||
                  ((option_ptr[sal_strlen(option->short_key)] != '=') &&
                   (option_ptr[sal_strlen(option->short_key)] != ' ') &&
                   (option_ptr[sal_strlen(option->short_key)] != '\n') &&
                   (option_ptr[sal_strlen(option->short_key)] != '"')))))
            {
                char *tmp_str;
                if (option->def == NULL)
                {
                    tmp_str = "Argument with no default must be used in examples";
                }
                else
                {
                    tmp_str = "Not used in examples";
                }
                sh_sand_usage_error(unit, shell_flex_p, sh_sand_cmd->cmd_only, option->keyword, tmp_str, &first);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_usage_leaf_tabular(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_control_t * sand_control)
{
    sh_sand_option_t *option;
    int flag;
    char **tokens = NULL;
    uint32 realtokens = 0;
    char *cmd_str;
    sh_sand_enum_t *enum_entry;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if ((cmd_str = sal_alloc(SH_SAND_MAX_TOKEN_SIZE, "cmd_str")) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
        SHR_EXIT();
    }

    if ((sh_sand_cmd == NULL) || (sh_sand_cmd->man == NULL))
    {   /* On this stage there is no need to print message, all errors should be rectified by verify */
        SHR_EXIT();
    }

    PRT_TITLE_SET("%s", "Usage");

    PRT_COLUMN_ADD("Item");
    PRT_COLUMN_ADD("Argument");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Default");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("SYNOPSIS");
    PRT_CELL_SKIP(1);
    SHR_CLI_EXIT_IF_ERR(sh_sand_synopsis_get(unit, sh_sand_cmd, &tokens, &realtokens), "");
    {
        int i_tok;
        for (i_tok = 0; i_tok < realtokens; i_tok++)
        {
            if (i_tok != 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            PRT_CELL_SET("%s", tokens[i_tok]);
        }
    }
    utilex_str_split_free(tokens, realtokens);
    tokens = NULL;

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("DESCRIPTION");
    PRT_CELL_SKIP(1);
    if (!ISEMPTY(sh_sand_cmd->man->full))
    {
        PRT_CELL_SET("%s", sh_sand_cmd->man->full);
    }
    else
    {
        PRT_CELL_SET("%s", sh_sand_cmd->man->brief);
    }

    if ((sh_sand_cmd->options != NULL) && (sh_sand_cmd->options->keyword != NULL))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("OPTIONS");
        for (option = sh_sand_cmd->options; option->keyword != NULL; option++)
        {
            char *def_str;
            if (option->def == NULL)
            {
                def_str = "NONE";
            }
            else if (ISEMPTY(option->def))
            {
                def_str = "EMPTY";
            }
            else
            {
                def_str = option->def;
            }
            /*
             * For the first option skip 1 cell, for all others allocate new row 
             */
            if (option != sh_sand_cmd->options)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            if (option->full_key != NULL)
            {
                PRT_CELL_SET("%s", option->full_key);
            }
            else
            {
                PRT_CELL_SET("%s", option->keyword);
            }
            PRT_CELL_SET("%s", option->desc);
            PRT_CELL_SET("%s", sal_field_type_str(option->type));
            PRT_CELL_SET("%s", def_str);
            for (enum_entry = (sh_sand_enum_t *) (option->ext_ptr); enum_entry && enum_entry->string; enum_entry++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
                PRT_CELL_SET_SHIFT(1, "%s", enum_entry->string);
                if (!ISEMPTY(enum_entry->desc))
                {
                    PRT_CELL_SET_SHIFT(1, "%s", enum_entry->desc);
                }
                else
                {
                    PRT_CELL_SET_SHIFT(1, "%s", "TBD");
                }
            }
        }
        PRT_ROW_SET_MODE(PRT_ROW_SEP_UNDERSCORE);
    }

    /*
     * Show system options only when all option used
     */
    SH_SAND_GET_BOOL("all", flag);
    if (flag == TRUE)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("SYSTEM OPTIONS");
        for (option = sh_sand_sys_arguments; option->keyword != NULL; option++)
        {
            char *def_str;
            if (option->def == NULL)
            {
                def_str = "NONE";
            }
            else if (ISEMPTY(option->def))
            {
                def_str = "EMPTY";
            }
            else
            {
                def_str = option->def;
            }

            /*
             * For the first option skip 1 cell, for all others allocate new row 
             */
            if (option != sh_sand_sys_arguments)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }
            PRT_CELL_SET("%s", option->keyword);
            PRT_CELL_SET("%s", option->desc);
            PRT_CELL_SET("%s", sal_field_type_str(option->type));
            PRT_CELL_SET("%s", def_str);
            for (enum_entry = (sh_sand_enum_t *) (option->ext_ptr); enum_entry && enum_entry->string; enum_entry++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
                PRT_CELL_SET_SHIFT(1, "%s", enum_entry->string);
                if (!ISEMPTY(enum_entry->desc))
                {
                    PRT_CELL_SET_SHIFT(1, "%s", enum_entry->desc);
                }
                else
                {
                    PRT_CELL_SET_SHIFT(1, "%s", "TBD");
                }
            }
        }
        PRT_ROW_SET_MODE(PRT_ROW_SEP_UNDERSCORE);
    }
    /*
     * Now print examples
     */
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("EXAMPLES");
    SHR_CLI_EXIT_IF_ERR(sh_sand_examples_get(unit, sh_sand_cmd, &tokens, &realtokens), "");
    {
        int i_tok;
        for (i_tok = 0; i_tok < realtokens; i_tok++)
        {
            if (i_tok != 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            PRT_CELL_SKIP(1);
            PRT_CELL_SET("%s", tokens[i_tok]);
        }
    }
    utilex_str_split_free(tokens, realtokens);
    tokens = NULL;

    PRT_COMMITX;
exit:
    if (tokens != NULL)
    {
        utilex_str_split_free(tokens, realtokens);
    }
    if (cmd_str != NULL)
    {
        sal_free(cmd_str);
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_usage_leaf(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_control_t * sand_control)
{
    int columns, margin;
    int flag;
    char **tokens = NULL;
    uint32 realtokens = 0;

    SHR_FUNC_INIT_VARS(unit);

    if ((sh_sand_cmd == NULL) || (sh_sand_cmd->man == NULL))
    {   /* On this stage there is no need to print message, all errors should be rectified by verify */
        SHR_EXIT();
    }

    SH_SAND_GET_BOOL("tabular", flag);
    if (flag == TRUE)
    {
        sh_sand_usage_leaf_tabular(unit, sh_sand_cmd, sand_control);
        goto exit;
    }

    SH_SAND_GET_INT32("column", columns);
    SH_SAND_GET_INT32("margin", margin);

    LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "SYNOPSIS"));
    SHR_CLI_EXIT_IF_ERR(sh_sand_synopsis_get(unit, sh_sand_cmd, &tokens, &realtokens), "");
    {
        int i_tok;
        for (i_tok = 0; i_tok < realtokens; i_tok++)
        {
            sh_sand_print(tokens[i_tok], margin, margin, columns);
        }
    }
    utilex_str_split_free(tokens, realtokens);
    tokens = NULL;

    LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "DESCRIPTION"));
    if (!ISEMPTY(sh_sand_cmd->man->full))
    {
        sh_sand_print((char *) sh_sand_cmd->man->full, margin, margin, columns);
    }
    else
    {
        sh_sand_print((char *) sh_sand_cmd->man->brief, margin, margin, columns);
    }

    if ((sh_sand_cmd->options != NULL) && (sh_sand_cmd->options->keyword != NULL))
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "ARGUMENTS"));
        sh_sand_print_arguments(unit, sh_sand_cmd->options, margin, margin, columns);
    }

    SH_SAND_GET_BOOL("all", flag);
    if (flag == TRUE)
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "SYSTEM ARGUMENTS"));
        sh_sand_print_arguments(unit, sh_sand_sys_arguments, margin, margin, columns);
    }

    LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "EXAMPLES"));
    SHR_CLI_EXIT_IF_ERR(sh_sand_examples_get(unit, sh_sand_cmd, &tokens, &realtokens), "");
    {
        int i_tok;
        for (i_tok = 0; i_tok < realtokens; i_tok++)
        {
            sh_sand_print(tokens[i_tok], margin, margin, columns);
        }
    }
    utilex_str_split_free(tokens, realtokens);
    tokens = NULL;

    LOG_CLI((BSL_META("\n")));

exit:
    if (tokens != NULL)
    {
        utilex_str_split_free(tokens, realtokens);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_usage_exec(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *flex_ptr,
    int flags)
{
    int columns;
    sh_sand_control_t *sand_control;
    SHR_FUNC_INIT_VARS(unit);

    sand_control = (sh_sand_control_t *) flex_ptr;
    SH_SAND_GET_INT32("column", columns);

    diag_sand_prt_char(columns, '*');
    cli_out("\n* %s\n", sh_sand_cmd->cmd_only);
    diag_sand_prt_char(columns, '*');
    cli_out("\n\n");

    SHR_CLI_EXIT_IF_ERR(sh_sand_usage_leaf(unit, sh_sand_cmd, sand_control),
                        "Error on usage for:%s", sh_sand_cmd->cmd_only);

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
    sh_sand_cmd_t *sh_sys_cmds_a = sand_control->sh_sys_cmd_a;
    int flag;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("list", flag);
    if (flag == TRUE)
    {
        SHR_SET_CURRENT_ERR(sh_sand_cmd_traverse
                            (unit, NULL, sand_control->sh_sand_cmd_a, cmd_usage_exec, (void *) sand_control, 0));
        SHR_EXIT();
    }
    PRT_TITLE_SET("%s", "Supported commands");

    PRT_COLUMN_ADD("Command");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    SH_SAND_CMD_ITERATOR(sh_sand_cmd, sh_sand_cmd_a)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        if (!ISEMPTY(sh_sand_cmd->full_key))
        {
            PRT_CELL_SET("%s", sh_sand_cmd->full_key);
        }
        else
        {
            PRT_CELL_SET("%s", sh_sand_cmd->keyword);
        }
        if (sh_sand_cmd->man && !ISEMPTY(sh_sand_cmd->man->brief))
        {
            PRT_CELL_SET("%s", sh_sand_cmd->man->brief);
        }
    }
    SH_SAND_GET_BOOL("all", flag);
    if ((flag == TRUE) && (sh_sys_cmds_a != NULL))
    {
        PRT_ROW_SET_MODE(PRT_ROW_SEP_UNDERSCORE);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "System Commands");
        SH_SAND_CMD_ITERATOR(sh_sand_cmd, sh_sys_cmds_a)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            if (!ISEMPTY(sh_sand_cmd->full_key))
            {
                PRT_CELL_SET("%s", sh_sand_cmd->full_key);
            }
            else
            {
                PRT_CELL_SET("%s", sh_sand_cmd->keyword);
            }
            if (sh_sand_cmd->man && !ISEMPTY(sh_sand_cmd->man->brief))
            {
                PRT_CELL_SET("%s", sh_sand_cmd->man->brief);
            }
        }
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t sys_usage_man = {
    .brief = "Print list of commands available",
    .full = "Print list of commands available for the command",
    .synopsis = NULL,
    .examples = NULL
};

/* *INDENT-OFF* */
sh_sand_option_t sys_usage_arguments[] = {
    {"column",  SAL_FIELD_TYPE_INT32, "Maximum columns number for output",          "120"},
    {"margin",  SAL_FIELD_TYPE_INT32, "Left&Right margin of defined display width", "6"},
    {"tabular", SAL_FIELD_TYPE_BOOL,  "Print usage in tabular view",                "No"},
    {"all",     SAL_FIELD_TYPE_BOOL,  "Show all including system ones",             "No"},
    {"list",    SAL_FIELD_TYPE_BOOL,  "Show usage of all leafs inside the branch",  "No"},
    {NULL}
};
/* *INDENT-ON* */

shr_error_e
sys_usage_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    if (sand_control->sh_sand_cmd_a)
    {
        sh_sand_usage_branch(unit, sand_control->sh_sand_cmd_a, sand_control);
    }
    else if (sand_control->sh_sand_cmd)
    {
        sh_sand_usage_leaf(unit, sand_control->sh_sand_cmd, sand_control);
    }

    SHR_FUNC_EXIT;
}

sh_sand_man_t sys_verify_man = {
    .brief = "Verifies command definition",
    .full = "Verifies brief and full description, arguments, enum values for arguments ans so on."
        "Prints report on all errors found",
};

/* *INDENT-OFF* */
sh_sand_option_t sys_verify_arguments[] = {
    {"all",     SAL_FIELD_TYPE_BOOL,  "Performs additional checks on usage",             "No"},
    {NULL}
};

shr_error_e
sys_verify_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shell_flex_t shell_flex;
    int all_flag, flags = 0;
    char *test_command;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if(sand_control->sh_sand_cmd_a == NULL)
    {
        test_command = sand_control->sh_sand_cmd->cmd_only;
    }
    else if(sand_control->sh_sand_cmd_a->parent_cmd)
    {
        test_command = sand_control->sh_sand_cmd_a->parent_cmd->cmd_only;
    }
    else
    {
        test_command = "Root Level";
    }
    PRT_TITLE_SET("UsageTEST results for:%s", test_command);

    SH_SAND_GET_BOOL("all", all_flag);
    if (all_flag == TRUE)
        flags |= SH_CMD_ALL;

    PRT_COLUMN_ADD("Command");
    PRT_COLUMN_ADD("Argument");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Error");
    shell_flex.failed = 0;
    shell_flex.succeeded = 0;
    shell_flex.skipped = 0;
    shell_flex.prt_ctr = prt_ctr;

    if (sand_control->sh_sand_cmd_a)
    {
        /** Invoke callback for branches as well as for leafs */
        flags |= SH_CMD_TRAVERSE_ALL;
        SHR_SET_CURRENT_ERR(sh_sand_cmd_traverse
                         (unit, NULL, sand_control->sh_sand_cmd_a, sh_sand_usage_verify, (void *) &shell_flex, flags));
    }
    else if (sand_control->sh_sand_cmd)
    {
        sh_sand_usage_verify(unit, NULL, sand_control->sh_sand_cmd, (void *) &shell_flex, flags);
    }

    if(shell_flex.failed != 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("Failures:");
        PRT_CELL_SET("%d", shell_flex.failed);
    }
    else
    {
        PRT_INFO_ADD("No Failures");
        PRT_INFO_SET_MODE(PRT_ROW_SEP_UNDERSCORE_BEFORE)
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_man_t sys_exec_man = {
    .brief = "Perform all commands under the node",
    .full = "Perform all commands under the node, excluding the ones that have arguments without defaults",
    .synopsis = "[dnx] command [command stack] exec [all] [lg] [fail] [force]",
};

sh_sand_option_t sys_exec_arguments[] = {
    {"all", SAL_FIELD_TYPE_BOOL, "Execute all shell commands ignoring errors", "No"},
    {"logger", SAL_FIELD_TYPE_BOOL, "Log all commands execution as it happens", "No"},
    {"failed", SAL_FIELD_TYPE_BOOL, "Results table will show only failed tests", "No"},
    {"force", SAL_FIELD_TYPE_BOOL, "Execute all shell commands ignoring SKIP flag", "No"},
    {"quiet", SAL_FIELD_TYPE_BOOL, "DO not print regular command log - only the resulting table", "No"},
    {"write", SAL_FIELD_TYPE_BOOL, "Save command output to XML file under the name of the command", "No"},
    {"memory", SAL_FIELD_TYPE_BOOL, "Verify the example frees all allocated memory", "No"},
    {NULL}
};
/* *INDENT-ON* */

static shr_error_e
cmd_shell_exec_single(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    char *options_str,
    void *flex_ptr,
    int flags)
{
    args_t *leaf_args = NULL;
    char command_str[SH_SAND_MAX_TOKEN_SIZE] = { 0 };
    sal_usecs_t usec;
    char *str_next;
    sh_sand_control_t sand_ctr_m;
    shell_flex_t *shell_flex_p = (shell_flex_t *) flex_ptr;
    prt_control_t *prt_ctr;
    unsigned long alloc_start, free_start, alloc_end, free_end;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&sand_ctr_m, 0, sizeof(sh_sand_control_t));

    if ((shell_flex_p == NULL) || ((prt_ctr = shell_flex_p->prt_ctr) == NULL))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Memory Error\n");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    /*
     * Create actual command line
     */
    sal_snprintf(command_str, SH_SAND_MAX_TOKEN_SIZE - 1, "%s %s", sh_sand_cmd->cmd_only, options_str);

    PRT_CELL_SET("%s", command_str);
    usec = sal_time_usecs();

    if (flags & SH_CMD_LOG)
    {
        LOG_CLI((BSL_META("Command:'%s'\n"), command_str));
    }

    if ((leaf_args = sal_alloc(sizeof(args_t), "leafs")) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
        PRT_CELL_SET("Memory Error");
        SHR_EXIT();
    }
    sal_memset(leaf_args, 0, sizeof(args_t));

    if (!ISEMPTY(options_str))
    {
        if (diag_parse_args(options_str, &str_next, leaf_args))
        {       /* Parses up to ; or EOL */
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            PRT_CELL_SET("Shell Error");
        }
    }

    /*
     * Check if the command is not to be skipped when running in batch, then add options if any
     */
    if ((sh_sand_cmd->flags & SH_CMD_SKIP_EXEC) && !(flags & SH_CMD_FORCE))
    {
        PRT_CELL_SET("%s", "SKIP");
        shell_flex_p->skipped++;
        if (flags & SH_CMD_FAILURE_ONLY)
        {
            /*
             * Do not record succeeded tests - only failed ones
             */
            PRT_ROW_DELETE();
            SHR_EXIT();
        }
    }
    else if (SHR_FAILURE(sh_sand_option_list_process(unit, leaf_args, sh_sand_cmd, &sand_ctr_m, sh_sand_cmd->flags)))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        PRT_CELL_SET("%s", "Argument Error");
        shell_flex_p->failed++;
    }
    else
    {
        if (flags & SH_CMD_SILENT)
        {
            sh_process_command(unit, "debug appl shell warn");
        }
        sand_ctr_m.sh_sand_cmd = sh_sand_cmd;
        /** get memory allocation before running current step */
        sal_get_alloc_counters(&(alloc_start), &(free_start));

        SHR_SET_CURRENT_ERR(sh_sand_cmd->action(unit, leaf_args, &sand_ctr_m));

        /** get memory allocation after ctest run */
        sal_get_alloc_counters(&(alloc_end), &(free_end));

        if (flags & SH_CMD_SILENT)
        {
            sh_process_command(unit, "debug appl shell info");
        }

        if(flags & SH_CMD_MEMORY_CHECK)
        {
            if((SHR_GET_CURRENT_ERR() == _SHR_E_NONE) && (alloc_end - alloc_start) != (free_end - free_start))
            {
                SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
            }
        }

        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            shell_flex_p->succeeded++;
            if (flags & SH_CMD_FAILURE_ONLY)
            {
                /*
                 * Do not record succeeded tests - only failed ones
                 */
                PRT_ROW_DELETE();
                SHR_EXIT();
            }
        }
        else
        {
            shell_flex_p->failed++;
        }
        PRT_CELL_SET("%s", shrextend_errmsg_get(SHR_GET_CURRENT_ERR()));
    }

    usec = sal_time_usecs() - usec;
    PRT_CELL_SET("%u", usec);
    if(flags & SH_CMD_MEMORY_CHECK)
    {
        PRT_CELL_SET("%lu", alloc_end - alloc_start);
        PRT_CELL_SET("%lu", free_end - free_start);
    }
exit:
    if (flags & SH_CMD_LOG)
    {
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("\tSuccess\n")));
        }
        else
        {
            LOG_CLI((BSL_META("\tFailure\n")));
        }
    }
    /*
     * Clean allocations from sand_ctr
     */
    sh_sand_option_list_clean(unit, &sand_ctr_m);
    /*
     * Free memory used for argument parsing
     */
    if (leaf_args != NULL)
    {
        sal_free(leaf_args);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_shell_exec(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *flex_ptr,
    int flags)
{
    shr_error_e cur_error;
    int i_tok;
    char **tokens = NULL;
    uint32 realtokens = 0, maxtokens = 100;
    char options_str[SH_SAND_MAX_TOKEN_SIZE] = { 0 };
    char add_arguments_str[SH_SAND_MAX_TOKEN_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd->man == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Man pointer NULL for:%s", sh_sand_cmd->cmd_only);
    }
#ifndef NO_FILEIO
    if (flags & SH_CMD_OUTPUT_XML)
    {
        char filename[SH_SAND_MAX_TOKEN_SIZE];

        SHR_CLI_EXIT_IF_ERR(dbx_file_dir_create("xml"), "Failed to create 'xml'\n");

        /** Add file= with the filename created from the command path */
        sh_sand_fill_output_name(sh_sand_cmd, "xml", filename);
        /*
         * Delete the file cleaning the place for new dumps
         */
        SHR_CLI_EXIT_IF_ERR(dbx_file_remove(filename), "");

        sal_snprintf(add_arguments_str, SH_SAND_MAX_TOKEN_SIZE - 1, "file=%s", filename);
    }
#endif
    /*
     * Invoke init callback is set
     */
    if (sh_sand_cmd->man->init_cb)
    {
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            /*
             * If no deinit is defined - invoke transaction rollback
             */
            if (sh_sand_cmd->man->deinit_cb == NULL)
            {
                SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));
            }
        }
#endif
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd->man->init_cb(unit), "Init Callback on:%s failed\n", sh_sand_cmd->cmd_only);
    }
    /*
     * Check if there is examples list, if not we assume there only one command invocation scenario
     */
    if (ISEMPTY(sh_sand_cmd->man->examples))
    {
        SHR_SET_CURRENT_ERR(cmd_shell_exec_single(unit, args, sh_sand_cmd, add_arguments_str, flex_ptr, flags));
    }
    else
    {
        tokens = utilex_str_split(sh_sand_cmd->man->examples, "\n", maxtokens, &realtokens);
        if (tokens != NULL)
        {
            for (i_tok = 0; i_tok < realtokens; i_tok++)
            {
                /*
                 * If output XML flag is imposed we need to exclude file= argument from the list
                 */
                if (flags & SH_CMD_OUTPUT_XML)
                {
                    if (sh_sand_exclude_argument(unit, tokens[i_tok], "file=", ".xml") == _SHR_E_EXISTS)
                    {
                        /*
                         * Skip test of command that has used file argument for different purpose
                         * Do not test this example in framework write test and stop testing this command
                         */
                        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                        break;
                    }
                }
                sal_snprintf(options_str, SH_SAND_MAX_TOKEN_SIZE - 1, "%s %s", tokens[i_tok], add_arguments_str);

                cur_error = cmd_shell_exec_single(unit, args, sh_sand_cmd, options_str, flex_ptr, flags);
                /*
                 * Preserve error for exit if it was one
                 */
                if (cur_error != _SHR_E_NONE)
                {
                    SHR_SET_CURRENT_ERR(cur_error);
                }
            }
        }
    }
    /*
     * Each command will be separated by underscore line in resulting table
     */
    {
        prt_control_t *prt_ctr = ((shell_flex_t *) flex_ptr)->prt_ctr;
        PRT_ROW_SET_MODE(PRT_ROW_SEP_UNDERSCORE);
    }

    if (sh_sand_cmd->man->deinit_cb)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd->man->deinit_cb(unit), "DeInit Callback on:%s failed\n", sh_sand_cmd->cmd_only);
    }
#ifdef BCM_DNX_SUPPORT
    else if (SOC_IS_DNX(unit) && (sh_sand_cmd->man->init_cb))
    {
        /*
         * If there was init callback but no deinit - transaction should be in place
         */
        SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));
    }
#endif
exit:
    /*
     * Invoke deinit callback is set
     */
    if (tokens != NULL)
    {
        utilex_str_split_free(tokens, realtokens);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sys_exec_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int flags = 0, in_flag;
    shell_flex_t shell_flex;
    sal_usecs_t usec;
    unsigned long alloc_start, free_start, alloc_end, free_end;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    SH_SAND_GET_BOOL("all", in_flag);
    if (in_flag == TRUE)
        flags |= SH_CMD_ALL;
    SH_SAND_GET_BOOL("logger", in_flag);
    if (in_flag == TRUE)
        flags |= SH_CMD_LOG;
    SH_SAND_GET_BOOL("force", in_flag);
    if (in_flag == TRUE)
        flags |= SH_CMD_FORCE;
    SH_SAND_GET_BOOL("failed", in_flag);
    if (in_flag == TRUE)
        flags |= SH_CMD_FAILURE_ONLY;
    SH_SAND_GET_BOOL("quiet", in_flag);
    if (in_flag == TRUE)
        flags |= SH_CMD_SILENT;
    SH_SAND_GET_BOOL("write", in_flag);
    if (in_flag == TRUE)
        flags |= SH_CMD_OUTPUT_XML;
    SH_SAND_GET_BOOL("memory", in_flag);
    if (in_flag == TRUE)
        flags |= SH_CMD_MEMORY_CHECK;

    PRT_TITLE_SET("%s", "Commands Examples Execution");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Command");
    PRT_COLUMN_ADD("Status");
    PRT_COLUMN_ADD("Time(usec)");
    PRT_COLUMN_ALIGN;
    if(flags & SH_CMD_MEMORY_CHECK)
    {
        PRT_COLUMN_ADD("Memory Alloc");
        PRT_COLUMN_ADD("Memory Free");
    }
    shell_flex.failed = 0;
    shell_flex.succeeded = 0;
    shell_flex.skipped = 0;
    shell_flex.prt_ctr = prt_ctr;

    usec = sal_time_usecs();
    /** get memory allocation before running current step */
    sal_get_alloc_counters(&(alloc_start), &(free_start));

    if (sand_control->sh_sand_cmd_a)
    {
        SHR_SET_CURRENT_ERR(sh_sand_cmd_traverse
                            (unit, NULL, sand_control->sh_sand_cmd_a, cmd_shell_exec, (void *) &shell_flex, flags));
    }
    else if (sand_control->sh_sand_cmd)
    {
        SHR_SET_CURRENT_ERR(cmd_shell_exec(unit, NULL, sand_control->sh_sand_cmd, (void *) &shell_flex, flags));
    }
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Failed:%d", shell_flex.failed);
    PRT_CELL_SET("Success:%d", shell_flex.succeeded);
    usec = sal_time_usecs() - usec;
    PRT_CELL_SET("%u", usec);

    /** get memory allocation after ctest run */
    sal_get_alloc_counters(&(alloc_end), &(free_end));
    if(flags & SH_CMD_MEMORY_CHECK)
    {
        PRT_CELL_SET("%lu", alloc_end - alloc_start);
        PRT_CELL_SET("%lu", free_end - free_start);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#ifndef NO_FILEIO
/*
 * user Manual generation command is not relevant for NO-FILEIO systems
 * {
 */
static shr_error_e
sh_sand_manual_leaf(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    void *parent_node,
    int depth)
{
    void *cur_node;
    char **examples = NULL;
    uint32 examples_num;
    char **synopsis = NULL;
    uint32 synopsis_num;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sh_sand_cmd, _SHR_E_PARAM, "sh_sand_cmd");

    if ((cur_node = dbx_xml_child_add(parent_node, sh_sand_cmd->keyword, depth)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add %s node\n", sh_sand_cmd->keyword);
    }
    if ((sh_sand_cmd->man != NULL) && (sh_sand_cmd->man->brief != NULL))
    {
        RHDATA_SET_STR(cur_node, "Brief", (char *) sh_sand_cmd->man->brief);
    }
    if ((sh_sand_cmd->man != NULL) && (sh_sand_cmd->man->full != NULL))
    {
        RHDATA_SET_STR(cur_node, "Description", (char *) sh_sand_cmd->man->full);
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_synopsis_get(unit, sh_sand_cmd, &synopsis, &synopsis_num), "");
    {
        void *synopsis_node, *item_node;
        int i_ex;
        if ((synopsis_node = dbx_xml_child_add(cur_node, "Synopsis", depth + 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add synopsis node to %s node\n", sh_sand_cmd->keyword);
        }
        for (i_ex = 0; i_ex < synopsis_num; i_ex++)
        {
            if ((item_node = dbx_xml_child_add(synopsis_node, "item", depth + 2)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add item node to parent for %s\n", sh_sand_cmd->keyword);
            }
            RHDATA_SET_STR(item_node, "command", synopsis[i_ex]);
            dbx_xml_node_end(item_node, depth + 2);
        }
        dbx_xml_node_end(synopsis_node, depth + 1);
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_examples_get(unit, sh_sand_cmd, &examples, &examples_num), "");
    {
        void *examples_node, *item_node;
        int i_ex;
        if ((examples_node = dbx_xml_child_add(cur_node, "Examples", depth + 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add examples node to %s node\n", sh_sand_cmd->keyword);
        }
        for (i_ex = 0; i_ex < examples_num; i_ex++)
        {
            if ((item_node = dbx_xml_child_add(examples_node, "item", depth + 2)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add item node to parent for %s\n", sh_sand_cmd->keyword);
            }
            RHDATA_SET_STR(item_node, "command", examples[i_ex]);
            dbx_xml_node_end(item_node, depth + 2);
        }
        dbx_xml_node_end(examples_node, depth + 1);
    }

    if (sh_sand_cmd->options != NULL)
    {
        void *options_node, *single_node;
        sh_sand_option_t *option;
        if ((options_node = dbx_xml_child_add(cur_node, "Arguments", depth + 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add argument node to %s node\n", sh_sand_cmd->keyword);
        }
        for (option = sh_sand_cmd->options; option->keyword != NULL; option++)
        {
            char *def_str;
            if ((single_node = dbx_xml_child_add(options_node, "argument", depth + 2)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add argument node to %s node\n", sh_sand_cmd->keyword);
            }
            RHDATA_SET_STR(single_node, "name", option->keyword);
            RHDATA_SET_STR(single_node, "type", sal_field_type_str(option->type));
            if (option->def == NULL)
            {
                def_str = "NONE";
            }
            else if (ISEMPTY(option->def))
            {
                def_str = "EMPTY";
            }
            else
            {
                def_str = option->def;
            }
            RHDATA_SET_STR(single_node, "default", def_str);
            if (option->desc)
            {
                RHDATA_SET_STR(single_node, "description", option->desc);
            }
            if (option->ext_ptr != NULL)
            {
                sh_sand_enum_t *enum_entry = (sh_sand_enum_t *) (option->ext_ptr);
                void *values_node, *value_node;
                if ((values_node = dbx_xml_child_add(single_node, "EnumValues", depth + 3)) == NULL)
                {
                    SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add EnumValues node to %s node\n", option->keyword);
                }
                for (; !ISEMPTY(enum_entry->string); enum_entry++)
                {
                    if ((value_node = dbx_xml_child_add(values_node, "value", depth + 4)) == NULL)
                    {
                        SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add enum value node to EnumValues of %s node\n",
                                     option->keyword);
                    }
                    RHDATA_SET_STR(value_node, "name", enum_entry->string);
                    if (ISEMPTY(enum_entry->desc))
                    {
                        def_str = "TBD";
                    }
                    else
                    {
                        def_str = enum_entry->desc;
                    }
                    RHDATA_SET_STR(value_node, "description", def_str);
                    dbx_xml_node_end(value_node, depth + 4);
                }
                dbx_xml_node_end(values_node, depth + 3);
            }
            dbx_xml_node_end(single_node, depth + 2);
        }
        dbx_xml_node_end(options_node, depth + 1);
    }
    dbx_xml_node_end(cur_node, depth);

exit:
    if (examples != NULL)
    {
        utilex_str_split_free(examples, examples_num);
    }
    if (synopsis != NULL)
    {
        utilex_str_split_free(synopsis, synopsis_num);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_manual_branch(
    int unit,
    char *parent_name,
    char *parent_description,
    sh_sand_cmd_t * sh_sand_cmd_a,
    void *parent_node,
    int depth)
{
    sh_sand_cmd_t *sh_sand_cmd;
    void *cur_node;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(sh_sand_cmd_a, _SHR_E_INTERNAL, "sh_sand_cmd_a");

    if (!ISEMPTY(parent_name))
    {
        if ((cur_node = dbx_xml_child_add(parent_node, parent_name, depth)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", parent_name);
        }
        if (!ISEMPTY(parent_description))
        {
            RHDATA_SET_STR(cur_node, "Brief", parent_description);
        }
    }
    else
    {
        cur_node = parent_node;
    }

    SH_SAND_CMD_ITERATOR(sh_sand_cmd, sh_sand_cmd_a)
    {
        if (sh_sand_cmd->child_cmd_a != NULL)
        {
            if (sh_sand_cmd->man != NULL)
            {
                parent_description = (char *) sh_sand_cmd->man->brief;
            }
            else
            {
                parent_description = NULL;
            }
            SHR_CLI_EXIT_IF_ERR(sh_sand_manual_branch(unit, sh_sand_cmd->full_key, parent_description,
                                                      sh_sand_cmd->child_cmd_a, cur_node, depth + 1), "");
        }
        else if (sh_sand_cmd->action != NULL)
        {
            SHR_CLI_EXIT_IF_ERR(sh_sand_manual_leaf(unit, sh_sand_cmd, cur_node, depth + 1), "");
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error(Command is neither node, nor leaf) on:%s\n", sh_sand_cmd->keyword);
        }
    }
    dbx_xml_node_end(cur_node, depth);

exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t sys_manual_man = {
    .brief = "Export usage to xml file. By default it will be manual.xml placed in $PWD."
        "Argument 'file' may be used to assign any other name or/and path",
    .examples = "file=Full_UM.xml\n"
};

const char *introduction =
    "This document presents description of all shell commands written in the context of framework,"
    "that provides unified approach in command invocation and usage."
    "All commands are organized in command tree, where leaves are actual executable commands."
    "Manual presents description, synopsis, arguments and examples per each leaf."
    "Each argument has certain type which defines input format, as may be see from argument description"
    "Argument may have default and thus not required on command line, if there is no default(tagged by 'None' in usage)"
    "user must provide it."
    "All arguments should be provided in arg=<value> paradigm with 2 exceptions:"
    "1. Boolean argument provided without value imply 'TRUE', "
    "2. Argument with 'free' tag allows omitting argument name, providing values only. Values without argument names "
    "will be associated with free argument according to the order. 1st nameless argument to the 1st free one,"
    "2nd to the second and so on"
    "Besides the arguments presented per command, there are system arguments that may be used with any leaf,"
    "There are also system commands which may be invoked after each branch or leaf, see details below";

shr_error_e
sys_manual_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *filename;
    void *top_node, *intro_node;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    SH_SAND_GET_STR("file", filename);
    /*
     * Validate input
     */
    if (ISEMPTY(filename))
    {
        filename = "manual.xml";
    }
    else
    {
        if (dbx_file_get_type(filename) != DBX_FILE_XML)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "File must have .xml suffix. File name was:\"%s\"\n", filename);
        }
    }
    /*
     * Create XML Document
     */
    if ((top_node = dbx_file_get_xml_top(unit, filename, "top",
                                         CONF_OPEN_CREATE | CONF_OPEN_OVERWRITE | CONF_OPEN_CURRENT_LOC)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Failed to create:\"%s\"\n", filename);
    }
    /*
     *  Add Introduction
     */
    if ((intro_node = dbx_xml_child_add(top_node, "General", 1)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add introduction node\n");
    }
    RHDATA_SET_STR(intro_node, "Introduction", (char *) introduction);
    /*
     * Add system arguments description
     */
    {
        sh_sand_cmd_t system_arguments_cmd;

        sal_memset(&system_arguments_cmd, 0, sizeof(sh_sand_cmd_t));

        system_arguments_cmd.keyword = system_arguments_cmd.full_key = "System_Arguments";
        system_arguments_cmd.man = &sh_sand_sys_arguments_man;
        system_arguments_cmd.options = sh_sand_sys_arguments;
        sh_sand_manual_leaf(unit, &system_arguments_cmd, intro_node, 1);
    }
    /*
     *  Add System Command Section
     */
    if (sand_control->sh_sys_cmd_a != NULL)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_manual_branch(unit, "System_Commands",
                                                  "Commands applicable after each regular command",
                                                  sand_control->sh_sys_cmd_a, intro_node, 1), "");
    }
    /*
     * For leaf just fill one node and left, for node go into recursion on the entire branch
     */
    if (sand_control->sh_sand_cmd_a)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_manual_branch(unit, NULL, NULL, sand_control->sh_sand_cmd_a, top_node, 0), "");
    }
    else if (sand_control->sh_sand_cmd)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_manual_leaf(unit, sand_control->sh_sand_cmd, top_node, 0), "");
    }

    dbx_xml_top_save(top_node, filename);
    dbx_xml_top_close(top_node);

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
#endif

#ifndef __KERNEL__
/*
 * Set severity corresponding to bsl meta identifier as specified
 * and return originally stored severity.
 */
static bsl_severity_t
set_log_level_to(
    unsigned long meta_identifier,
    bsl_severity_t required_severity)
{
    int layer, source;
    bsl_severity_t stored_severity;

    layer = BSL_LAYER_GET(meta_identifier);
    source = BSL_SOURCE_GET(meta_identifier);
    stored_severity = bslenable_get(layer, source);
    bslenable_set((bsl_layer_t) layer, (bsl_source_t) source, required_severity);
    return (stored_severity);
}
#endif /* __KERNEL__ */

char cmd_set_device_usage[] =
    "\n Change the device identification (PCIE device+revision ID) to the SDK.\n"
    "Usages:\n" "set_device <device ID> <revision ID>\n" "\n";

cmd_result_t
cmd_set_device(
    int unit,
    args_t * a)
{
#ifndef __KERNEL__
    bsl_severity_t original_severity;
    cmd_result_t ret;
    char *param;
    uint32 dev_id;
    uint32 rev_id;
    ibde_dev_t *dev = (ibde_dev_t *) bde->get_dev(unit);

    /*
     * Set log level of I2C to 'warning'. Restore at exit.
     */
    original_severity = set_log_level_to(BSL_LS_SOC_I2C, bslSeverityWarn);

    /*
     * get dev id 
     */
    param = ARG_GET(a);
    if (!param)
    {
        return CMD_USAGE;
    }
    else
    {
        dev_id = sal_ctoi(param, 0);
    }

    /*
     * get rev id 
     */
    param = ARG_GET(a);
    if (!param)
    {
        rev_id = DNXC_A0_REV_ID;
    }
    else
    {
        rev_id = sal_ctoi(param, 0);
    }

    CMDEV(unit).dev.dev_id = dev_id;
    CMDEV(unit).dev.rev_id = rev_id;

    dev->device = dev_id;
    dev->rev = rev_id;

    ret = CMD_OK;

    /*
     * Restore log level of I2C.
     */
    set_log_level_to(BSL_LS_SOC_I2C, original_severity);
    return (ret);
#else /* __KERNEL__ */
    cli_out("This function is unavailable in Kernel mode\n");
    return CMD_USAGE;
#endif /* __KERNEL__ */
}

/* *INDENT-OFF* */
sh_sand_cmd_t sh_sand_sys_cmds[] = {
    {"usage",           sys_usage_cmd,  NULL, sys_usage_arguments,  &sys_usage_man},
    {"help",            sys_usage_cmd,  NULL, sys_usage_arguments,  &sys_usage_man},
#ifndef NO_FILEIO
    {"manual",          sys_manual_cmd, NULL, NULL,               &sys_manual_man},
#endif
#ifdef INCLUDE_CTEST
    {"exec",            sys_exec_cmd,   NULL, sys_exec_arguments,   &sys_exec_man},
    {"UsageTEST",       sys_verify_cmd, NULL, sys_verify_arguments, &sys_verify_man},
#endif
    {NULL}
};
/* *INDENT-ON* */
