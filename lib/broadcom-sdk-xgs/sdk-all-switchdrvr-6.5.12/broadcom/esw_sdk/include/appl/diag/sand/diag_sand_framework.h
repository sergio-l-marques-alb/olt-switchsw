/**
 * \file diag_sand_framework.h
 *
 * Framework utilities, structures and definitions
 *
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_SAND_FRAMEWORK_H_INCLUDED
#define DIAG_SAND_FRAMEWORK_H_INCLUDED

#include <sal/types.h>
#include <sal/appl/field_types.h>
#include <shared/utilex/utilex_rhlist.h>
#include <appl/diag/parse.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_utils.h>
#ifdef INCLUDE_AUTOCOMPLETE
#include <sal/appl/editline/autocomplete.h>
#endif

/**
 * Shell Init Flags
 */
#define SH_SAND_VERIFY_MAN          0x01
#define SH_SAND_VERIFY_KEYWORDS     0x02
#define SH_SAND_VERIFY_ALL          SH_SAND_VERIFY_KEYWORDS | SH_SAND_VERIFY_MAN

/* Command flags */
#define CTEST_PRECOMMIT     0x0001
#define CTEST_POSTCOMMIT    0x0002
#define SH_CMD_LEGACY       0x0004
#define SH_CMD_SKIP_EXEC    0x0008
#define CTEST_RUN_LOG       0x0010

#define CTEST_DEFAULT    "default"

/**
 * \brief
 *   Presence of this flag in argument state means that user explicitly used this option in shell command invocation
 */
#define SH_SAND_OPTION_PRESENT      0x01
/**
 * \brief
 *   Presence of this flag in argument state means that option has mask: name=value,mask
 */
#define SH_SAND_OPTION_MASKED       0x02
/**
 * \brief
 *   Presence of this flag in argument state means that option has range: name=start-end
 *   Range may be provided only for non-negative value
 */
#define SH_SAND_OPTION_RANGE        0x04

/**
 * \brief Maximum string size for single token input
 */
#define SH_SAND_MAX_TOKEN_SIZE      1024
#define SH_SAND_MAX_ARRAY32_SIZE    16
#define SH_SAND_MAX_KEYWORD_SIZE    128
#define SH_SAND_MAX_RESOURCE_SIZE   SH_SAND_MAX_KEYWORD_SIZE + 3        /* Basic keyword plus 1 character for null term 
                                                                         * and 2 for plural form */
#define SH_SAND_MAX_COMMAND_DEPTH   10
#define SH_SAND_MAX_TIME_SIZE       64

#define SH_SAND_MAX_UINT32          0xFFFFFFFF
/**
 * \brief Typedef to construct structure that will hold all possible options of true and false
 */
typedef struct
{
    /**
     * String representing true or false statement
     */
    char *string;
    /**
     * Value that will be FALSE for negative, TRUE for positive answers
     */
    int value;
} sh_sand_enum_t;

/**
 * \brief Typedef for shell leaf command
 * \par DIRECT INPUT:
 *     \param [in] keyword option name to be verified through the callback
 *     \param [in] id_p pointer to option identifier to be used by shell command, may be NULL
 * \par INDIRECT OUTPUT:
 *     \param [out] id option identifier to be used by shell command, transferred through id_p variable
 * \par DIRECT OUTPUT:
 *     \retval SAL_FIELD_TYPE_NONE for failure - option does not exist
 *     \retval SAL_FIELD_TYPE_* any other valid field type
 */
typedef shr_error_e(
    *sh_sand_option_cb_t) (
    int unit,
    char *keyword,
    sal_field_type_e * type,
    uint32 *id_p,
    /*
     * Void pointer for different kind os extensions
     */
    void **ext_ptr_p);

/**
 * \brief Union allowing to handle all types of parameters through the same pointer
 */
typedef union
{
    /**
     * String, copied from input
     */
    char val_str[SH_SAND_MAX_TOKEN_SIZE];
    /**
     * It is plain int value, but we use separate to mark the boolean
     */
    int val_bool;
    /**
     * It is plain int value, but we use separate to mark the enum
     */
    int val_enum;
    /**
     * 32 bit signed value
     */
    int val_int32;
    /**
     * 32 bit unsigned value
     */
    uint32 val_uint32;
    /**
     * MAC address - array of 6 bytes
     */
    sal_mac_addr_t mac_addr;
    /**
     * IPv4 address - unsigned 32 bit value
     */
    sal_ip_addr_t ip4_addr;
    /**
     * IPv6 address - array of 16 bytes
     */
    sal_ip6_addr_t ip6_addr;
    /**
     * Array 32 bit unsigned value
     */
    uint32 array_uint32[SH_SAND_MAX_ARRAY32_SIZE];
    /**
     * Ports bitmap
     */
    bcm_pbmp_t ports_bitmap;
} sh_sand_param_u;

/**
 * \brief Control structure for command keyword definition, provided by framework
 */
typedef struct sh_sand_keyword_s
{
    /**
     * Keyword
     */
    char *keyword;
    /**
     * Plural form for the keyword, no plural being provided in the sh_sand_keywords means auto creation
     */
    char plural[SH_SAND_MAX_RESOURCE_SIZE];
    /**
     * Shortcut for the keyword obtained by capital letters
     */
    char short_key[SH_SAND_MAX_RESOURCE_SIZE];
    /**
     * How much times specific keyword was used
     */
    int count;
} sh_sand_keyword_t;

/**
 * \brief Control structure for cli option definition, provided by command developer
 */
typedef struct sh_sand_option_s
{
    /**
     * Option Name
     */
    char *keyword;
    /**
     * Option type, used to scan from string into value and print the option
     */
    sal_field_type_e type;
    /**
     * Brief description, used in usage
     */
    char *desc;
    /**
     * Default string in the same format, as cli use is supposed to enter
     */
    char *def;
    /*
     * Void pointer for different kind os extensions
     */
    void *ext_ptr;
} sh_sand_option_t;

/**
 * \brief Control structure for processed cli option, provided by framework to leaf command.
 * Structure is initialized once and then only param_buffer, param and present variables are updated per command invocation
 */
typedef struct sh_sand_args_s
{
    /**
     * Entry allows single element to be queued on argument list, provided as input parameter for leaf routine
     */
    rhentry_t entry;
    /**
     * Option type, used to scan from string into value and print the option
     */
    sal_field_type_e type;
    /**
     * Identify different info per option
     * whether specific parameter was present or not on command line
     * whether mask was present or not, range or not
     */
    int state;
    /**
     * Identify whether specific parameter is requested to be present - aka must option.
     * It happens, when no default is defined. Pay attention that string option having empty one as default is valid default definition
     * Only NULL pointer for default is considered absence of default
     */
    int requested;
    /**
     * If option present
     *   1. If the option is single it will be here
     *   2. If it is range, start will be here
     *   3. Of it is masked value, value will be here
     */
    sh_sand_param_u param1;
    /**
     * If option present
     *   1. If the option is single, nothing will be here - should not be accessed
     *   2. If it is range, end will be here
     *   3. Of it is masked value, value will be here, if no mask provided will filled by 0xFF
     */
    sh_sand_param_u param2;
    /**
     * Identify different default info per option
     * whether specific parameter was present or not on command line
     * whether mask was present or not, range or not
     */
    int def_state;
    /**
     * Contains default value for param1 (see above)
     */
    sh_sand_param_u def_param1;
    /**
     * Contains default value for param2 (see above)
     */
    sh_sand_param_u def_param2;
    /*
     * Void pointer for different kind of extensions
     */
    void *ext_ptr;
    /*
     * Shortcut for option
     */
    char *short_key;
    /*
     * Default value string
     */
    char *def_str;
} sh_sand_arg_t;

/**
 * \brief Set of pointers to different info strings, which assembles into usage or man page
 */
typedef struct
{
    /**
     * brief command description not more than 80 characters
     */
    const char *brief;
    /**
     * Full command description limited by 1024 characters. May be increased through PRT_LONG_STR_SIZE
     */
    char *full;
    /**
     * How command line should look like.
     * E.g. access list [name=str] [property={reg, mem, signal, array}]
     */
    char *synopsis;
    /**
     * Characteristic examples of command usage
     */
    char *examples;
} sh_sand_man_t;

/**
 * \brief Control structure for shell command arguments, generated by command init and updated on invoke
 */
typedef struct
{
    /**
     * Pointer to static arguments list processed by framework.
     */
    rhlist_t *stat_args_list;   /* List of actual arguments, initialized on verify */
    /**
     * Pointer to dynamic arguments list obtained through callback from user.
     * List is emptied on action completion, but not erased
     */
    rhlist_t *dyn_args_list;    /* List of actual arguments, approved dynamically by user */
    /**
     * Pointer to command token list obtained from args
     * List is emptied on action completion, but not erased
     */
    rhlist_t *command_list;     /* List of command tokens */
    /**
     * Pointer to test list per  command
     * List is initiated from static invokes list per command or add_test facility
     * Oredr of adding tests is not important, so list may be modified any time
     */
    rhlist_t *test_list;        /* List of command tokens */
    /**
     * String that will contain all command line options
     * It is set to EMPTY before new command options processing
     */
    char options_str[SH_SAND_MAX_TOKEN_SIZE];
    /*
     * Each command may generate number of tables, all the tables from 1 invocation should be placed under the same node
     *    1. time stamp will be nullified before command invocation
     *    2. 1st table will create command node
     *    3. rest of tables will be placed under the same command node
     */
    char time_str[SH_SAND_MAX_TIME_SIZE];
    /*
     * System commands require pointer to underlying command array
     */
    struct sh_sand_cmd_s *sh_sand_cmd_a;
    struct sh_sand_cmd_s *sh_sys_cmd_a;
    struct sh_sand_cmd_s *sh_sand_cmd;
} sh_sand_control_t;

/**
 * \brief Typedef for shell leaf command
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 *     \param [in] ctr pointer to list of options processed by sand framework to be used with SH_SAND MACROS
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed
 *     \retval other errors for other failure type
 * \remark automatically frees the list
 */
typedef shr_error_e(
    *sh_sand_func_t) (
    int unit,
    args_t * args,
    sh_sand_control_t * ctr);

/**
 * \brief Control structure for full definition, provided by command developer
 */
typedef struct sh_sand_invoke_s
{
    /**
     * Pointer to test name
     */
    char *name;
    /**
     * Pointer to string with all parameters requested for specific command
     * e.g for "ctest mdb" - "table=LPM_PRIVATE", so that entire test line will be "ctest mdb table=LPM_PRIVATE"
     */
    char *params;
    /**
     * Flow execution flags, pointing to the stage test will take part in or other flow control
     */
    int flags;
} sh_sand_invoke_t;

/**
 * \brief Control structure for test list
 */
typedef struct sh_sand_test_s
{
    /**
     * Entry allows single element to be queued on argument list, provided as input parameter for leaf routine
     */
    rhentry_t entry;
    /**
     * Pointer to string with all parameters requested for specific command
     * e.g for "ctest mdb" - "table=LPM_PRIVATE", so that entire test line will be "ctest mdb table=LPM_PRIVATE"
     */
    char *params;
    /**
     * Flow execution flags, pointing to the stage test will take part in or other flow control
     */
    int flags;
} sh_sand_test_t;
/**
 * \brief Control structure for shell command definition, provided by command developer
 */
typedef struct sh_sand_cmd_s
{
    /**
     * Command name
     */
    char *keyword;
    /**
     * Pointer to leaf callback, if there is one
     */
    sh_sand_func_t action;
    /**
     * Pointer to next level command array, if there  is one
     */
    struct sh_sand_cmd_s *cmd;
    /**
     * Pointer to options list
     */
    sh_sand_option_t *options;
    /**
     * Pointer to man info structure, must be provided if command has leaf
     */
    sh_sand_man_t *man;         /* Manual structure */
    /*
     * When there is a need to accept dynamic options, callback need to be provided
     * Callback returns variable type and unique id that will allow to identify it inside without search
     */
    sh_sand_option_cb_t option_cb;
    /*
     * When there is a need to accept dynamic options, callback need to be provided
     * Callback returns variable type and unique id that will allow to identify it inside without search
     */
    sh_sand_invoke_t *invokes;
    /*
     * Misc flags, including
     * 1. Legacy mode serves to support legacy commands, do not enable with new/rewritten ones
     * 2. PRE/POST Commit indication for CTEST
     */
    int flags;
    /*
     * On init/verify short key based on capital letters in sh_sand_keyword is assigned
     */
    char *short_key;
    /*
     * On init/verify put here pointer to resource for the keyword found
     */
    char *full_key;
    /**
     * Pointer to control structure passed to the user on leaf function invocation.
     * This is the first dynamic parameter. Any static parameters that may be set in global structures should be above
     */
    sh_sand_control_t ctr;
} sh_sand_cmd_t;

/**
 * \brief Typedef for traverse callback command
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 *     \param [in] cmd pointer to leaf command that traversing arrived to
 *     \param [in] prt_ctr pointer to PRT control structure for recursive output
 *     \param [in] depth recursion depth for different purposes, like print or recursion control
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed
 *     \retval other errors for other failure type
 * \remark automatically frees the list
 */
typedef shr_error_e(
    *sh_sand_traverse_cb_t) (
    int unit,
    args_t * args,
    sh_sand_cmd_t * cmd,
    void *flex_ptr,
    int depth);

/**
 * \brief Routine serves to invoke command from any level, it then acts recursively parsing command line
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 *     \param [in] sh_sand_cmd_a pointer to command list(array) to start from
 *     \param [in] sh_sys_cmd_a pointer to system command list(array) for specified root
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_act(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_cmd_t * sh_sys_cmd_a);

/**
 * \brief Routine serves to invoke system command on the entire node
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 *     \param [in] cmd_name - command to be executed on node
 *     \param [in] sh_sand_cmd_a pointer to command list(array) to start from
 *     \param [in] sh_sys_cmd_a pointer to system command list(array) for specified root
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_act_all(
    int unit,
    args_t * args,
    char *cmd_name,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_cmd_t * sh_sys_cmd_a);

shr_error_e sh_sand_cmd_fetch(
    rhlist_t * command_list,
    char *command_str);

shr_error_e sh_sand_cmd_traverse(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_traverse_cb_t sh_sand_action,
    void *flex_ptr,
    int flags);

shr_error_e sys_usage_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief Routine serves to add test dynamically
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] sh_sand_cmd_a pointer to command list(array) to start from
 *     \param [in] cmd_name command name for which test should be added
 *     \param [in] test_name test name
 *     \param [in] test_params list of command line options for the test
 *     \param [in] flags marks tests export variety for different conditions
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_test_add(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    char *cmd_name,
    char *test_name,
    char *test_params,
    int flags);

/**
 * \brief Routine serves to remove dynamically added test
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] sh_sand_cmd_a pointer to command list(array) to start from
 *     \param [in] cmd_name command name for which test should be added
 *     \param [in] test_name test name
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 * \remark
 *     Should be called on module deinit to free allocated resources
 */
shr_error_e sh_sand_test_remove(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    char *cmd_name,
    char *test_name);

/**
 * \brief Routine serves to verify correctness of shell command tree and init reqested resources
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] sh_sand_cmd pointer to command list to start from
 *     \param [in] command accumulated from shell tree traversing, usually starts from NULL
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_init(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    char *command,
    int flags);

/**
 * \brief Routine serves to free resources for entire tree
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] sh_sand_cmd pointer to command list to start from
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_deinit(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd);

/**
 * \brief Return string for boolean value
 */
char *sh_sand_bool_str(
    int bool);
/**
 * \brief This macro is for local usage only to make external macros SH_SAND_GET* more transparent
 */

#ifdef INCLUDE_AUTOCOMPLETE
 /**
 * \brief Routine serves to add autocomplete support
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] parent pointer to parent autocomplete node
 *     \param [in] cmd commands tree
 */

void sh_sand_cmd_autocomplete_init(
    int unit,
    autocomplete_node_t * parent,
    sh_sand_cmd_t * cmd);

void sh_sand_cmd_autocomplete_deinit(
    int unit,
    sh_sand_cmd_t * cmd);

#endif
/*
 * Set of MACROS for obtaining regular option values, not ranegs and not masked
 * {
 */
/*
 * \brief - Fetches sand_arg as per option name and assigns value union ptr to "param" variable
 * \param [in] mc_arg_keyword - keyword for the argument
 * \remark
 *    Strictly internal MACRO - to be used only by underlying ones, makes local assumptions
 */
#define _SH_SAND_GET(mc_arg_keyword)                                                                                   \
    sh_sand_param_u *param = NULL;                                                                                     \
    sal_field_type_e param_type = SAL_FIELD_TYPE_MAX;                                                                  \
    if(sand_control == NULL)                                                                                           \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_PARAM, "No command control for:%s\n", mc_arg_keyword);                                     \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        sh_sand_arg_t *sand_arg;                                                                                       \
        if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->stat_args_list, mc_arg_keyword)) == NULL)         \
        {                                                                                                              \
            if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, mc_arg_keyword)) == NULL)      \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_PARAM, "command line option:%s is not supported\n", mc_arg_keyword);               \
            }                                                                                                          \
        }                                                                                                              \
        param_type = sand_arg->type;                                                                                   \
        if(sand_arg->state & SH_SAND_OPTION_RANGE)                                                                     \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_PARAM, "GET MACRO does not suit range input for option:%s, use RANGE ones\n",          \
                                                                                              mc_arg_keyword);         \
        }                                                                                                              \
        if(sand_arg->state & SH_SAND_OPTION_MASKED)                                                                    \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_PARAM, "GET MACRO does not suit masked input for option:%s, use MASKED ones\n",        \
                                                                                              mc_arg_keyword);         \
        }                                                                                                              \
        if(sand_arg->state & SH_SAND_OPTION_PRESENT)                                                                   \
        {                                                                                                              \
            param = &sand_arg->param1;                                                                                 \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            param = &sand_arg->def_param1;                                                                             \
        }                                                                                                              \
    }

/*
 * \brief - Verifies that GET macro is consistent wipe of option it requests, based on previously obtained param_type
 * \remark
 *    Strictly internal MACRO - to be used only by underlying ones, makes local assumptions
 */
#define _SH_SAND_TYPE_VERIFY(arg_keyword, in_type)                                                                     \
        if(in_type != param_type)                                                                                      \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_PARAM, "option:\"%s\" - GET macro of type:%s not matching definition type:%s\n",       \
                                            arg_keyword, sal_field_type_str(in_type), sal_field_type_str(param_type)); \
        }

/*
 * \brief - Fetches argument value of cli option
 * \param [in] arg_keyword - option name, exactly as defined in option list(sh_sand_option_t), case insensitive
 * \param [in] arg_value   - variable for value assignment
 * \remark
 *    1. Valid only in the context of shell command framework, sand_control need to be defined. either via initial
 *       framework callback or passed down as parameter
 *    2. Description valid for all SH_SAND_GET_* macros with the only difference being arg_value type, which will be
 *       Specified explicitly before each macro
 *    3. Value assignment may be by value or pointer (see details per macro
 */
/**
 * 1. char *arg_value
 * 2. by pointer, pointer to the value string is assigned to
 */
#define SH_SAND_GET_STR(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_STR)                                     \
                             arg_value = param->val_str;                                                               \
                         }
/**
 * 1. int arg_value
 * 2. by value, boolean value is assigned to
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_BOOL(arg_keyword, arg_value)                                                                       \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_BOOL)                                    \
                             arg_value = param->val_bool;                                                              \
                         }
/**
 * 1. int arg_value
 * 2. by value, enum value is assigned to
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_ENUM(arg_keyword, arg_value)                                                                       \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_ENUM)                                    \
                             arg_value = param->val_enum;                                                              \
                         }
/**
 * 1. int arg_value -
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_PORT(arg_keyword, arg_value)                                                                       \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_PORT)                                    \
                             arg_value = param->ports_bitmap;                                                          \
                         }
/**
 * 1. int arg_value
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_INT32(arg_keyword, arg_value)                                                                      \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_INT32)                                   \
                             arg_value = param->val_int32;                                                             \
                         }
/**
 * 1. uint32 arg_value
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_UINT32(arg_keyword, arg_value)                                                                     \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_UINT32)                                  \
                             arg_value = param->val_uint32;                                                            \
                         }
/**
 * 1. uint32 *arg_value
 * 2. by pointer
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_ARRAY32(arg_keyword, arg_value)                                                                    \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_ARRAY32)                                 \
                             arg_value = param->array_uint32;                                                          \
                         }
/**
 * 1. sal_ip_addr_t arg_value
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_IP4(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP4)                                     \
                             arg_value = param->ip4_addr;                                                              \
                         }
/**
 * 1. sal_ip6_addr_t arg_value
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_IP6(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP6)                                     \
                             memcpy(arg_value, param->ip6_addr, sizeof(sal_ip6_addr_t));                               \
                         }
/**
 * 1. sal_mac_addr_t arg_value
 * 2. by value
 * 3. See all details above in SH_SAND_GET_STR
 */
#define SH_SAND_GET_MAC(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_MAC)                                     \
                             memcpy(arg_value, param->mac_addr, sizeof(sal_mac_addr_t));                               \
                         }
/*
 * End of regular options fetching MACROS
 * }
 */

/*
 * Set of MACROS for obtaining range option values
 * {
 */
/*
 * \brief - Fetches sand_arg as per option name and assigns value union ptr to "param" variable, difference from regular
 *          _SH_SAND_GET is that 2 params are fetched, and if there is no second pne on command line, first is assigned
 *          to both
 * \param [in] mc_arg_keyword - keyword for the argument
 * \remark
 *    Strictly internal MACRO - to be used only by underlying ones, makes local assumptions
 *    Only certain FIELD_TYPES are eligible for range options, see below
 */
#define _SH_SAND_GET_RANGE(mc_arg_keyword)                                                                             \
    sh_sand_param_u *param_start = NULL;                                                                               \
    sh_sand_param_u *param_end   = NULL;                                                                               \
    sal_field_type_e param_type = SAL_FIELD_TYPE_MAX;                                                                  \
    if(sand_control == NULL)                                                                                           \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_PARAM, "No command control for:%s\n", mc_arg_keyword);                                     \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        sh_sand_arg_t *sand_arg;                                                                                       \
        if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->stat_args_list, mc_arg_keyword)) == NULL)         \
        {                                                                                                              \
            if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, mc_arg_keyword)) == NULL)      \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_PARAM, "command line option:%s is not supported\n", mc_arg_keyword);               \
            }                                                                                                          \
        }                                                                                                              \
        param_type = sand_arg->type;                                                                                   \
        if(sand_arg->state & SH_SAND_OPTION_MASKED)                                                                    \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_PARAM, "RANGE MACRO does not suit masked input for option:%s, use MASKED ones\n",      \
                                                                                                       mc_arg_keyword);\
        }                                                                                                              \
        if(sand_arg->state & SH_SAND_OPTION_PRESENT)                                                                   \
        {                                                                                                              \
            param_start = &sand_arg->param1;                                                                           \
            if(sand_arg->state & SH_SAND_OPTION_RANGE)                                                                 \
                param_end = &sand_arg->param2;                                                                         \
            else                                                                                                       \
                param_end = &sand_arg->param1;                                                                         \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            param_start = &sand_arg->def_param1;                                                                       \
            if(sand_arg->def_state & SH_SAND_OPTION_RANGE)                                                             \
                param_end = &sand_arg->def_param2;                                                                     \
            else                                                                                                       \
                param_end = &sand_arg->def_param1;                                                                     \
        }                                                                                                              \
    }
/**
 * All *_RANGE macros have in addition to first value, which will represent start of range second one that represents
 * end of range, if there is only one value on command line it will be considered range of one element start=end
 * Eligible types: INT32, UINT32, IP4, IP6, MAC
 */
#define SH_SAND_GET_INT32_RANGE(arg_keyword, arg_value_start, arg_value_end)                                           \
                         {                                                                                             \
                             _SH_SAND_GET_RANGE(arg_keyword)                                                           \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_INT32)                                   \
                             arg_value_start = param_start->val_int32;                                                 \
                             arg_value_end   = param_end->val_int32;                                                   \
                         }

#define SH_SAND_GET_UINT32_RANGE(arg_keyword, arg_value_start, arg_value_end)                                          \
                         {                                                                                             \
                             _SH_SAND_GET_RANGE(arg_keyword)                                                           \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_UINT32)                                  \
                             arg_value_start = param_start->val_uint32;                                                \
                             arg_value_end   = param_end->val_uint32;                                                  \
                         }

#define SH_SAND_GET_IP4_RANGE(arg_keyword, arg_value_start, arg_value_end)                                             \
                         {                                                                                             \
                             _SH_SAND_GET_RANGE(arg_keyword)                                                           \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP4)                                     \
                             arg_value_start = param_start->ip4_addr;                                                  \
                             arg_value_end   = param_end->ip4_addr;                                                    \
                         }

#define SH_SAND_GET_IP6_RANGE(arg_keyword, arg_value_start, arg_value_end)                                             \
                         {                                                                                             \
                             _SH_SAND_GET_RANGE(arg_keyword)                                                           \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP6)                                     \
                             memcpy(arg_value_start, param_start->ip6_addr, sizeof(sal_ip6_addr_t));                   \
                             memcpy(arg_value_end, param_end->ip6_addr, sizeof(sal_ip6_addr_t));                       \
                         }

#define SH_SAND_GET_MAC_RANGE(arg_keyword, arg_value_start, arg_value_end)                                             \
                         {                                                                                             \
                             _SH_SAND_GET_RANGE(arg_keyword)                                                           \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_MAC)                                     \
                             memcpy(arg_value_start, param_start->mac_addr, sizeof(sal_mac_addr_t));                   \
                             memcpy(arg_value_end, param_end->mac_addr, sizeof(sal_mac_addr_t));                       \
                         }
/*
 * End of range options fetching MACROS
 * }
 */
/*
 * Set of MACROS for obtaining masked option values
 * {
 */
/*
 * \brief - Fetches sand_arg as per option name and assigns value union ptr to "param" variable. It obtains 2 value,
 *          where first is considered to be data, second mask. If there is no second, default value of all FF is
 *          assigned to mask
 * \param [in] mc_arg_keyword - keyword for the argument
 * \remark
 *    Strictly internal MACRO - to be used only by underlying ones, makes local assumptions
 */
#define _SH_SAND_GET_MASKED(mc_arg_keyword)                                                                            \
    sh_sand_param_u *param_data = NULL;                                                                                \
    sh_sand_param_u *param_mask = NULL;                                                                                \
    sal_field_type_e param_type = SAL_FIELD_TYPE_MAX;                                                                  \
    if(sand_control == NULL)                                                                                           \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_PARAM, "No command control for:%s\n", mc_arg_keyword);                                     \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        sh_sand_arg_t *sand_arg;                                                                                       \
        if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->stat_args_list, mc_arg_keyword)) == NULL)         \
        {                                                                                                              \
            if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, mc_arg_keyword)) == NULL)      \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_PARAM, "command line option:%s is not supported\n", mc_arg_keyword);               \
            }                                                                                                          \
        }                                                                                                              \
        param_type = sand_arg->type;                                                                                   \
        if(sand_arg->state & SH_SAND_OPTION_RANGE)                                                                     \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_PARAM, "MASKED MACRO does not suit range input for option:%s, use range ones\n",       \
                                                                                                       mc_arg_keyword);\
        }                                                                                                              \
        if(sand_arg->state & SH_SAND_OPTION_PRESENT)                                                                   \
        {                                                                                                              \
            param_data = &sand_arg->param1;                                                                            \
            if(sand_arg->state & SH_SAND_OPTION_MASKED)                                                                \
                param_mask = &sand_arg->param2;                                                                        \
            else                                                                                                       \
                param_mask = &sand_arg->param1;                                                                        \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            param_data = &sand_arg->def_param1;                                                                        \
            if(sand_arg->def_state & SH_SAND_OPTION_MASKED)                                                            \
                param_mask = &sand_arg->def_param2;                                                                    \
            else                                                                                                       \
                param_mask = &full_param_mask;                                                                         \
        }                                                                                                              \
    }

/**
 * All *_RANGE macros have in addition to first value, which will represent data, second one that represents
 *       mask. if there is only one value on command line it will be considered FULLY unmasked or the value of
 *       0xFF filled in mask
 * Eligible types: INT32, UINT32, IP4, IP6, MAC
 */
#define SH_SAND_GET_INT32_MASKED(arg_keyword, arg_value_data, arg_value_mask)                                          \
                         {                                                                                             \
                             _SH_SAND_GET_MASKED(arg_keyword)                                                          \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_INT32)                                   \
                             arg_value_data = param_data->val_int32;                                                   \
                             arg_value_mask = param_mask->val_int32;                                                   \
                         }

#define SH_SAND_GET_UINT32_MASKED(arg_keyword, arg_value_data, arg_value_mask)                                         \
                         {                                                                                             \
                             _SH_SAND_GET_MASKED(arg_keyword)                                                          \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_UINT32)                                  \
                             arg_value_data = param_data->val_uint32;                                                  \
                             arg_value_mask = param_mask->val_uint32;                                                  \
                         }

#define SH_SAND_GET_IP4_MASKED(arg_keyword, arg_value_data, arg_value_mask)                                            \
                         {                                                                                             \
                             _SH_SAND_GET_MASKED(arg_keyword)                                                          \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP4)                                     \
                             arg_value_data = param_data->ip4_addr;                                                    \
                             arg_value_mask = param_mask->ip4_addr;                                                    \
                         }

#define SH_SAND_GET_IP6_MASKED(arg_keyword, arg_value_data, arg_value_mask)                                            \
                         {                                                                                             \
                             _SH_SAND_GET_MASKED(arg_keyword)                                                          \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_IP6)                                     \
                             memcpy(arg_value_data, param_data->ip6_addr, sizeof(sal_ip6_addr_t));                     \
                             memcpy(arg_value_mask, param_mask->ip6_addr, sizeof(sal_ip6_addr_t));                     \
                         }

#define SH_SAND_GET_MAC_MASKED(arg_keyword, arg_value_data, arg_value_mask)                                            \
                         {                                                                                             \
                             _SH_SAND_GET_MASKED(arg_keyword)                                                          \
                             _SH_SAND_TYPE_VERIFY(arg_keyword, SAL_FIELD_TYPE_MAC)                                     \
                             memcpy(arg_value_data, param_data->mac_addr, sizeof(sal_mac_addr_t));                     \
                             memcpy(arg_value_mask, param_mask->mac_addr, sizeof(sal_mac_addr_t));                     \
                         }
/*
 * End of masked options fetching MACROS
 * }
 */

#define _SH_SAND_GET_DYN(mc_arg_keyword, mc_is_present)                                                                \
            sh_sand_arg_t *sand_arg;                                                                                   \
            if((sand_control == NULL) || (sand_control->dyn_args_list == NULL) ||                                      \
                ((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, mc_arg_keyword)) == NULL))   \
            {                                                                                                          \
                mc_is_present = FALSE;                                                                                 \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                mc_is_present = TRUE;                                                                                  \
            }                                                                                                          \

#define SH_SAND_GET_STR_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.val_str;                                                 \
                         }

#define SH_SAND_GET_BOOL_DYN(arg_keyword, arg_value, is_present)                                                       \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.val_bool;                                                \
                         }

#define SH_SAND_GET_ENUM_DYN(arg_keyword, arg_value, is_present)                                                       \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.val_enum;                                                \
                         }

#define SH_SAND_GET_INT32_DYN(arg_keyword, arg_value, is_present)                                                      \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.val_int32;                                               \
                         }

#define SH_SAND_GET_ARRAY32_DYN(arg_keyword, arg_value, is_present)                                                    \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.array_uint32;                                            \
                         }

#define SH_SAND_GET_UINT32_DYN(arg_keyword, arg_value, is_present)                                                     \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.val_uint32;                                              \
                         }

#define SH_SAND_GET_IP4_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param1.ip4_addr;                                                \
                         }

#define SH_SAND_GET_IP6_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 memcpy(arg_value, sand_arg->param1.ip6_addr, sizeof(sal_ip6_addr_t));                 \
                         }

#define SH_SAND_GET_MAC_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 memcpy(arg_value, sand_arg->param1.mac_addr, sizeof(sal_mac_addr_t));                 \
                         }

#define SH_SAND_GET_ITERATOR(mc_arg) \
    for(mc_arg = utilex_rhlist_entry_get_first(sand_control->dyn_args_list); mc_arg; mc_arg = utilex_rhlist_entry_get_next(mc_arg))

#define SH_SAND_HAS_MASK(mc_arg)     (mc_arg->state & SH_SAND_OPTION_MASKED)

#define SH_SAND_GET_NAME(mc_arg)         RHNAME(mc_arg)
#define SH_SAND_GET_ID(mc_arg)           RHID(mc_arg)
#define SH_SAND_GET_TYPE(mc_arg)         mc_arg->type
#define SH_SAND_ARG_STR(mc_arg)          mc_arg->param1.val_str
#define SH_SAND_ARG_UINT32_DATA(mc_arg)  mc_arg->param1.val_uint32
#define SH_SAND_ARG_UINT32_MASK(mc_arg)  mc_arg->param2.val_uint32
#define SH_SAND_ARG_ARRAY_DATA(mc_arg)   mc_arg->param1.array_uint32
#define SH_SAND_ARG_ARRAY_MASK(mc_arg)   mc_arg->param2.array_uint32
#define SH_SAND_ARG_MAC_DATA(mc_arg)     mc_arg->param1.mac_addr
#define SH_SAND_ARG_MAC_MASK(mc_arg)     mc_arg->param2.mac_addr
#define SH_SAND_ARG_IP6_DATA(mc_arg)     mc_arg->param1.ip6_addr
#define SH_SAND_ARG_IP6_MASK(mc_arg)     mc_arg->param2.ip6_addr

extern sh_sand_option_t sh_sand_sys_options[];

extern sh_sand_man_t sh_sand_shell_man;
extern sh_sand_cmd_t sh_sand_shell_cmds[];

extern sh_sand_cmd_t sh_sand_shell_test_cmds[];

extern sh_sand_cmd_t sh_sand_sys_cmds[];

extern sh_sand_man_t sys_usage_man;
extern sh_sand_option_t sys_usage_options[];

extern sh_sand_invoke_t sh_sand_all_invokes[];

extern sh_sand_keyword_t sh_sand_keywords[];
extern sh_sand_param_u full_param_mask;

/**
 * \brief Initialize keyword list, create shortcut and plural versions
 * \par INDIRECT INPUT:
 *     \param [in] sh_sand_keywords - list of strings for keyword processing
 * \par INDIRECT OUTPUT:
 *     \param [out] sh_sand_keywords - shortcuts and plural version of keywords
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - Bad keyword in the list
 *   \retval Other            - Other errors as per shr_error_e
 *   Flow goes to exit in any case of failure
 */
shr_error_e sh_sand_keyword_list_init(
    void);

/**
 * \brief Verify existence of keyword and return shortcut and form for printing
 * \param [in] keyword_in - pointer to string to be verified on keyword list
 * \param [in/out] short_key_p - pointer, where pointer to shortcut is to be placed
 * \param [in/out] output_p - pointer, where string to be used for print will be assigned
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_NOT_FOUND - Keyword was not found in the list
 *   \retval Other            - Other errors as per shr_error_e
 *   Flow goes to exit in any case of failure
 * \remark
 *   This routine is a primary one to be used for communication between keyword list and its users
 *   Default are assigned to shortcut and keyword when verification is not required
 */
shr_error_e sh_sand_keyword_fetch(
    char *keyword_in,
    char **shortcut_out_p,
    char **keyword_out_p,
    int flags);

/**
 * \brief Add device specific set of commands to general dynamic list
 * \param [in] unit - unit id
 * \param [in] sh_sand_cmd_a - pointer to command list to start from
 * \param [in] cmd_callback - command to be called by general shell, serving entry point to the the framework
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval Other            - Other errors as per shr_error_e
 * \remark
 *
 */
shr_error_e sh_sand_cmd_add_to_dyn_list(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    cmd_func_t cmd_callback);

/**
 * \brief Remove device specific set of commands from general dynamic list
 * \param [in] unit - unit id
 * \param [in] sh_sand_cmd_a - pointer to command list to start from
 * \param [in] cmd_callback - command to be called by general shell, serving entry point to the the framework
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval Other            - Other errors as per shr_error_e
 * \remark
 *
 */
shr_error_e sh_sand_cmd_remove_from_dyn_list(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    cmd_func_t cmd_callback);

/**
 * \brief Recursive verification procedure for sand command array
 * \param [in] sh_sand_cmds - sh_sand_cmd_t * - pointer to command array for specific command
 * \param [out] cmd_result - variable that return status value will be saved in
 * \return
 *   Flow goes to exit in any case of failure
 * \remark
 *   Routine should have exit point
 * \see
 *   sh_sand_init
 */
#define SH_SAND_VERIFY(sh_sand_cmds, cmd_result) \
        {                                                                                           \
            static int sh_sand_legacy_command_verified = FALSE;                                     \
            if(sh_sand_legacy_command_verified == FALSE)                                            \
            {                                                                                       \
                char *command = sal_alloc(SH_SAND_MAX_TOKEN_SIZE, "sh_sand_command_verify");        \
                if(command == NULL)                                                                 \
                {                                                                                   \
                    cli_out("Memory allocation failure\n");                                         \
                    cmd_result = CMD_FAIL;                                                          \
                    goto exit;                                                                      \
                }                                                                                   \
                command[0] = 0;                                                                     \
                cmd_result =  diag_sand_error_get(sh_sand_init(unit, sh_sand_cmds, command, SH_SAND_VERIFY_ALL));       \
                if(cmd_result == CMD_OK)                                                            \
                {                                                                                   \
                    command[0] = 0;                                                                 \
                    sh_sand_init(unit, sh_sand_sys_cmds, command, SH_SAND_VERIFY_ALL);              \
                    sh_sand_legacy_command_verified = TRUE;                                         \
                }                                                                                   \
                sal_free(command);                                                                  \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                cmd_result = CMD_OK;                                                                \
            }                                                                                       \
        }

/**
 * \brief Fills string with formated time, oriented for log purposes mainly
 * \param [in,out] time_str - pointer to string where formatted time presentation will be copied
 * \remark
 *   time_str should be at least SH_SAND_MAX_TIME_SIZE
 */
void sh_sand_time_get(
    char *time_str);

#endif /* DIAG_SAND_FRAMEWORK_H_INCLUDED */
