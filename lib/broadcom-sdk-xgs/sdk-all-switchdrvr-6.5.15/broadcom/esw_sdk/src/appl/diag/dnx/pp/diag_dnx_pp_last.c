/** \file diag_dnx_pp_last.c
 *
 * Last PP Diagnostics File
 * Shows IRPP information on last packet that passed in pipe
 */

/*************
 * INCLUDES  *
 *************/
#include "diag_dnx_pp.h"

/*************
 * DEFINES   *
 *************/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

/*************
 *  MACROS  *
 *************/
/** builds a diag shell command with SHORT flag and executes in BCM shell */
#define __EXECUTE_SHORT_DIAG_SHELL_CMD(__str_var, __vis_info_str, __diag_name, __core_id_str)                \
    sal_strncpy(__str_var, __vis_info_str, RHNAME_MAX_SIZE-1);                                               \
    sal_strncat(__str_var, " ", RHNAME_MAX_SIZE-1);                                                          \
    sal_strncat(__str_var, __diag_name, RHNAME_MAX_SIZE-1);                                                  \
    sal_strncat(__str_var, " short", RHNAME_MAX_SIZE-1);                                                     \
    sal_strncat(__str_var, " core=", RHNAME_MAX_SIZE-1);                                                     \
    sal_strncat(__str_var, __core_id_str, RHNAME_MAX_SIZE-1);                                                \
    SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, __str_var), "");                                            \


/** builds a diag shell command with SHORT flag and executes in BCM shell */
#define __EXECUTE_CMD_WITH_CORE_ID(__str_var, __cmd_str, __core_id_str)                                      \
    sal_strncpy(__str_var, __cmd_str, RHNAME_MAX_SIZE-1);                                                    \
    sal_strncat(__str_var, " core=", RHNAME_MAX_SIZE-1);                                                     \
    sal_strncat(__str_var, __core_id_str, RHNAME_MAX_SIZE-1);                                                \
    SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, __str_var),"");                                             \


/*************
 * TYPEDEFS  *
 *************/

/*************
 * GLOBALS   *
 *************/

/*************
 * FUNCTIONS *
 *************/
/**
 * \brief - main function of showing the IRPP information after last packet passing in pipe
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_last_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_id, core_low, core_high;
    int core = -1;

    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core);

    /*
     * Determine if the user passed a correct core value. If a specific correct core was asked, iterate on it only, otherwise on 0,1
     */
    set_core_low_high(unit, core, &core_low, &core_high);

    for (core_id = core_low; core_id <= core_high; core_id++)
    {
        char shell_command[RHNAME_MAX_SIZE] = "";
        char core_id_str[RHNAME_MAX_SIZE] = "";

        int rc;
        uint8 core_is_valid;

        /** check if core_id is valid on the Ingress */
        rc = diag_pp_check_core_validity(unit, core_id, TRUE, &core_is_valid);
        if ((!core_is_valid) || (rc != _SHR_E_NONE))
        {
            cli_out("Please run a packet through Core_Id=%d.\n", core_id);
            continue;
        }

        /** convert the integer core_id to string */
        sal_itoa(core_id_str, core_id, 10, 0, 0);

        __EXECUTE_SHORT_DIAG_SHELL_CMD(shell_command, "pp vis", "ppi", core_id_str);

        /** Don't run ikleap vis in c-model, only board */
#ifndef ADAPTER_SERVER_MODE
        __EXECUTE_CMD_WITH_CORE_ID(shell_command, "ikleap vis stage=fwd1 short", core_id_str);
        __EXECUTE_CMD_WITH_CORE_ID(shell_command, "ikleap vis stage=fwd2 short", core_id_str);
#endif

        __EXECUTE_CMD_WITH_CORE_ID(shell_command, "pp vis fec last short", core_id_str);

    } /** of for of core_id */

exit:

    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_last_man = {
    .brief = "PP VISibility LAST diagnostics.\n" "Shows IRPP information on last packet that passed in pipe.\n",
    .full = NULL,
    .synopsis = "[PP VISibility LAST]",
    .examples = "\n" "file=diag_pp_vis_last.xml"
};
