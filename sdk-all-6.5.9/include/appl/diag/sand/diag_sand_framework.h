/**
 * \file diag_sand_framework.h
 *
 * Framework utilities, structures and definitions
 *
 */
/*
 * $Copyright: (c) 2016 Broadcom.
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

/**
 * \brief Maximum string size for single token input
 */
#define SH_SAND_MAX_TOKEN_SIZE      512
#define SH_SAND_MAX_ARRAY32_SIZE    16
#define SH_SAND_MAX_KEYWORD_SIZE    64
#define SH_SAND_MAX_COMMAND_DEPTH   10
#define SH_SAND_MAX_TIME_SIZE       64

#define SH_SAND_MAX_UINT32          0xFFFFFFFF
/**
 * \brief Typedef to construct structure that will hold all possible options of true and false
 */
typedef struct {
    /**
     * String representing true or false statement
     */
    char *string;
    /**
     * Value that will be FALSE for negative, TRUE for positive answers
     */
    int  value;
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
    sal_field_type_e *type,
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
    char            val_str[SH_SAND_MAX_TOKEN_SIZE];
    /**
     * It is plain int value, but we use separate to mark the boolean
     */
    int             val_bool;
    /**
     * It is plain int value, but we use separate to mark the enum
     */
    int             val_enum;
    /**
     * 32 bit signed value
     */
    int             val_int32;
    /**
     * 32 bit unsigned value
     */
    uint32          val_uint32;
    /**
     * MAC address - array of 6 bytes
     */
    sal_mac_addr_t  mac_addr;
    /**
     * IPv4 address - unsigned 32 bit value
     */
    sal_ip_addr_t   ip4_addr;
    /**
     * IPv6 address - array of 16 bytes
     */
    sal_ip6_addr_t  ip6_addr;
    /**
     * Array 32 bit unsigned value
     */
    uint32          array_uint32[SH_SAND_MAX_ARRAY32_SIZE];
    /**
     * Ports bitmap
     */
    bcm_pbmp_t          ports_bitmap;
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
     * Shortcut for the keyword obtained by capital letters
     */
    char short_key[SH_SAND_MAX_KEYWORD_SIZE];
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
    /*
     * Shortcut for option
     */
    char *short_key;
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
    rhentry_t           entry;
    /**
     * Option type, used to scan from string into value and print the option
     */
    sal_field_type_e    type;
    /**
     * Identify whether specific parameter was present or not on command line
     */
    int                 present;
    /**
     * Identify whether specific parameter is requested to be present - aka must option.
     * It happens, when no default is defined. Pay attention that string option having empty one as default is valid default definition
     * Only NULL pointer for default is considered absence of default
     */
    int                 requested;
    /**
     * If option present, value is contained here
     */
    sh_sand_param_u     param;
    /**
     * Contains default value used when option was not present
     */
    sh_sand_param_u     def;
    /*
     * Void pointer for different kind of extensions
     */
    void *ext_ptr;
    /*
     * Shortcut for option
     */
    char *short_key;
    /*
     * Deafult value string
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
typedef struct {
    /**
     * Pointer to static arguments list processed by framework.
     */
    rhlist_t *stat_args_list;       /* List of actual arguments, initialized on verify */
    /**
     * Pointer to dynamic arguments list obtained through callback from user.
     * List is emptied on action completion, but not erased
     */
    rhlist_t *dyn_args_list;        /* List of actual arguments, approved dynamically by user */
    /**
     * Pointer to command token list obtained from args
     * List is emptied on action completion, but not erased
     */
    rhlist_t *command_list;        /* List of command tokens */
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
    args_t *args,
    sh_sand_control_t *ctr);

#define CTEST_PRECOMMIT   0x0001
#define CTEST_POSTCOMMIT  0x0002

#define CTEST_DEFAULT    "default"
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
    rhentry_t           entry;
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
    sh_sand_man_t *man; /* Manual structure */
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
     * Legacy mode serves to support legacy commands, do not enable with new/rewritten ones
     */
    int legacy_mode;
    /*
     * On init/verify short key based on capital letters in sh_sand_keyword is assigned
     */
    char *short_key;
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
    args_t *args,
    sh_sand_cmd_t *cmd,
    void *flex_ptr,
    int depth);

/**
 * \brief Routine serves to invoke command from any level, it then acts recursively parsing command line
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 *     \param [in] sh_sand_cmd pointer to command list to start from
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e
sh_sand_act(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd);

/**
 * \brief Routine serves to invoke command from any level, it then acts recursively parsing command line
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 *     \param [in] sh_sand_cmd_a pointer to command list(array) to start from
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e
sh_sand_test(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a);

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
shr_error_e
sh_sand_test_add(
    int             unit,
    sh_sand_cmd_t*  sh_sand_cmd_a,
    char*           cmd_name,
    char*           test_name,
    char*           test_params,
    int             flags);

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
 *     Should be called on module deinit to free allocate dresources
 */
shr_error_e
sh_sand_test_remove(
    int             unit,
    sh_sand_cmd_t*  sh_sand_cmd_a,
    char*           cmd_name,
    char*           test_name);

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
    char *command);

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
 * \brief Obtain exact keyword for command - to have verified shortcut and pther different formats
 * \par DIRECT INPUT:
 *     \param [in] keyword - original keyword
 *     \param [in] resource_keyword_p - pointer to the string with resource keyword
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e
sh_sand_keyword_get(
    char *keyword,
    char **resource_keyword_p);

/**
 * \brief Return string for boolean value
 */
char *sh_sand_bool_str(
        int bool);
/**
 * \brief This macro is for local usage only to make external macros SH_SAND_GET* more transparent
 */
#define _SH_SAND_GET(mc_arg_keyword)                                                                                   \
    sh_sand_param_u *param = NULL;                                                                                     \
    if(sand_control != NULL)                                                                                           \
    {                                                                                                                  \
        sh_sand_arg_t *sand_arg;                                                                                       \
        if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->stat_args_list, mc_arg_keyword)) == NULL)         \
        {                                                                                                              \
            if((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, mc_arg_keyword)) == NULL)      \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_PARAM, "command line option:%s is not supported\n", mc_arg_keyword);               \
            }                                                                                                          \
        }                                                                                                              \
        if(sand_arg->present == TRUE)                                                                                  \
        {                                                                                                              \
            param = &sand_arg->param;                                                                                  \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            param = &sand_arg->def;                                                                                    \
        }                                                                                                              \
    }

#define SH_SAND_GET_STR(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             if(param != NULL)                                                                         \
                                 arg_value = param->val_str;                                                           \
                             else                                                                                      \
                                 arg_value = "";                                                                       \
                         }

#define SH_SAND_GET_BOOL(arg_keyword, arg_value)                                                                       \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             arg_value = param->val_bool;                                                              \
                         }

#define SH_SAND_GET_ENUM(arg_keyword, arg_value)                                                                       \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             arg_value = param->val_enum;                                                              \
                         }

#define SH_SAND_GET_PORT(arg_keyword, arg_value)                                                                       \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             arg_value = param->ports_bitmap;                                                              \
                         }
#define SH_SAND_GET_INT32(arg_keyword, arg_value)                                                                      \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             arg_value = param->val_int32;                                                             \
                         }

#define SH_SAND_GET_UINT32(arg_keyword, arg_value)                                                                     \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             arg_value = param->val_uint32;                                                            \
                         }

#define SH_SAND_GET_ARRAY32(arg_keyword, arg_value)                                                                    \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             arg_value = param->array_uint32;                                                          \
                         }

#define SH_SAND_GET_IP4(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             arg_value = param->ip4_addr;                                                              \
                         }

#define SH_SAND_GET_IP6(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             memcpy(arg_value, param->ip6_addr, sizeof(sal_ip6_addr_t));                               \
                         }

#define SH_SAND_GET_MAC(arg_keyword, arg_value)                                                                        \
                         {                                                                                             \
                             _SH_SAND_GET(arg_keyword)                                                                 \
                             memcpy(arg_value, param->mac_addr, sizeof(sal_mac_addr_t));                               \
                         }

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
                                 arg_value = sand_arg->param.val_str;                                                  \
                         }

#define SH_SAND_GET_BOOL_DYN(arg_keyword, arg_value, is_present)                                                       \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param.val_bool;                                                 \
                         }

#define SH_SAND_GET_ENUM_DYN(arg_keyword, arg_value, is_present)                                                       \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param.val_enum;                                                 \
                         }

#define SH_SAND_GET_INT32_DYN(arg_keyword, arg_value, is_present)                                                      \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param.val_int32;                                                \
                         }

#define SH_SAND_GET_ARRAY32_DYN(arg_keyword, arg_value, is_present)                                                    \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param.array_uint32;                                             \
                         }

#define SH_SAND_GET_UINT32_DYN(arg_keyword, arg_value, is_present)                                                     \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param.val_uint32;                                               \
                         }

#define SH_SAND_GET_IP4_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 arg_value = sand_arg->param.ip4_addr;                                                 \
                         }

#define SH_SAND_GET_IP6_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 memcpy(arg_value, sand_arg->param.ip6_addr, sizeof(sal_ip6_addr_t));                  \
                         }

#define SH_SAND_GET_MAC_DYN(arg_keyword, arg_value, is_present)                                                        \
                         {                                                                                             \
                             _SH_SAND_GET_DYN(arg_keyword, is_present);                                                \
                             if(is_present == TRUE)                                                                    \
                                 memcpy(arg_value, sand_arg->param.mac_addr, sizeof(sal_mac_addr_t));                  \
                         }

#define SH_SAND_GET_ITERATOR(mc_arg) \
    for(mc_arg = utilex_rhlist_entry_get_first(sand_control->dyn_args_list); mc_arg; mc_arg = utilex_rhlist_entry_get_next(mc_arg))

#define SH_SAND_GET_NAME(mc_arg)      RHNAME(mc_arg)
#define SH_SAND_GET_ID(mc_arg)        RHID(mc_arg)
#define SH_SAND_GET_TYPE(mc_arg)      mc_arg->type
#define SH_SAND_GET_VALUE(mc_arg)     sand_arg->param.val_uint32

extern sh_sand_option_t sh_sand_sys_options[];
extern sh_sand_keyword_t sh_sand_keywords[];

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
                cmd_result =  diag_sand_error_get(sh_sand_init(unit, sh_sand_cmds, command));       \
                if(cmd_result == CMD_OK)                                                            \
                {                                                                                   \
                    sh_sand_legacy_command_verified = TRUE;                                         \
                }                                                                                   \
                sal_free(command);                                                                  \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                cmd_result = CMD_OK;                                                                \
            }                                                                                       \
        }

#endif /* DIAG_SAND_FRAMEWORK_H_INCLUDED */
