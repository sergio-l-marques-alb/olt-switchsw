/** \file diag_dnx_gpm.c
 *
 * Diagnostics for the allocation managers.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*************
 * INCLUDES  *
 *************/
/*shared*/
#include <shared/bsl.h>
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/shell.h>
/*bcm*/
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3_fec.h>
/*soc*/
#include <soc/dnx/dbal/dbal_string_mgmt.h>
/*sal*/
#include <sal/appl/sal.h>

/*************
 *  DEFINES  *
 *************/

#define CHAR_BUFF_SIZE 500

/*************
 *  MACROES  *
 *************/

/*************
 * FUNCTIONS *
 *************/

static shr_error_e
sh_dnx_gport_to_hw_resoures_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_algo_gpm_gport_hw_resources_t fec_hw_resources, lif_hw_resources;
    int gport;
    uint32 flags;
    char shell_commands_buffer[CHAR_BUFF_SIZE];
    char *dbal_entry_get_command = "dbal eNTry get table=";
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&fec_hw_resources, 0, sizeof(fec_hw_resources));
    sal_memset(&lif_hw_resources, 0, sizeof(lif_hw_resources));

    SH_SAND_GET_INT32("gport", gport);

    if (gport == -1)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "No gport given.\n");
    }

    /*
     * First, find the fec / lif resources for this gport.
     * Use NON_STRICT because we don't know which resources will be valid.
     */
    flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_FEC | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, flags, &fec_hw_resources));

    flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, flags, &lif_hw_resources));

    /*
     * If none are found, return error.
     * There are currently no gports that have a global lif and no local lif, but there are gports that only have local lif,
     * so we don't check the global lif's existence.
     */
    if (fec_hw_resources.fec_id == DNX_ALGO_GPM_FEC_INVALID
        && lif_hw_resources.local_in_lif == DNX_ALGO_GPM_LIF_INVALID
        && lif_hw_resources.local_out_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Given gport 0x%08x has no hardware resources or doesn't exist.\n", gport);
    }

    cli_out("HW resources for gport 0x%08x:\n\n", gport);

    /*
     * Print fec information, if it's valid.
     */
    if (fec_hw_resources.fec_id != DNX_ALGO_GPM_FEC_INVALID)
    {
        uint32 fec_hierarchy;
        dbal_tables_e fec_dbal_table;
        cli_out("FEC:\n");

        SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get(unit, fec_hw_resources.fec_id, &fec_hierarchy));

        cli_out("FEC ID is: %d, and it's in hierarchy #%d\n", fec_hw_resources.fec_id, fec_hierarchy + 1);

        /*
         * Print fec dbal information, using the dbal entry get shell command.
         */
        cli_out("DBAL info:\n");

        fec_dbal_table = L3_FEC_TABLE_FROM_HIERARCHY_GET(fec_hierarchy);

        sal_memset(shell_commands_buffer, 0, CHAR_BUFF_SIZE);

        sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s super_fec_id=%d",
                     dbal_entry_get_command,
                     dbal_logical_table_to_string(unit, fec_dbal_table),
                     DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(fec_hw_resources.fec_id));
        SHR_IF_ERR_EXIT(sh_process_command(unit, shell_commands_buffer));

        cli_out("\n\n");
    }

    /*
     * Print inlif information, if it's valid.
     */
    if (lif_hw_resources.local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        const char *logical_table_str = dbal_logical_table_to_string(unit, lif_hw_resources.inlif_dbal_table_id);
        const char *result_type_str = dbal_result_type_to_string(unit, lif_hw_resources.inlif_dbal_table_id,
                                                                 lif_hw_resources.inlif_dbal_result_type);
        cli_out("INLIF:\n");

        if (lif_hw_resources.global_in_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            cli_out("Global inlif is %d\n", lif_hw_resources.global_in_lif);
        }
        else
        {
            cli_out("No global inlif for this gport.\n");
        }

        cli_out("Local inlif id is %d, and it's of type %s, subtype %s\n", lif_hw_resources.local_in_lif,
                logical_table_str, result_type_str);

        /*
         * Print inlif dbal information, using the dbal entry get shell command.
         */
        cli_out("DBAL info:\n");
        sal_memset(shell_commands_buffer, 0, CHAR_BUFF_SIZE);

        sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s core_id=%d in_lif=%d result_type=%s",
                     dbal_entry_get_command,
                     logical_table_str,
                     (lif_hw_resources.local_in_lif_core == _SHR_CORE_ALL) ? 0 : lif_hw_resources.local_in_lif_core,
                     lif_hw_resources.local_in_lif, result_type_str);
        SHR_IF_ERR_EXIT(sh_process_command(unit, shell_commands_buffer));

        cli_out("\n\n");
    }

    /*
     * Print outlif information, if it's valid.
     */
    if (lif_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        const char *logical_table_str = dbal_logical_table_to_string(unit, lif_hw_resources.outlif_dbal_table_id);
        const char *result_type_str = dbal_result_type_to_string(unit, lif_hw_resources.outlif_dbal_table_id,
                                                                 lif_hw_resources.outlif_dbal_result_type);

        cli_out("OUTLIF:\n");

        if (lif_hw_resources.global_out_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            cli_out("Global outlif is %d\n", lif_hw_resources.global_out_lif);
        }
        else
        {
            cli_out("No global outlif for this gport.\n");
        }

        cli_out("Local outlif id is %d, on phase %d, and it's of type %s, subtype %s\n", lif_hw_resources.local_out_lif,
                lif_hw_resources.outlif_phase + 1, logical_table_str, result_type_str);

        /*
         * Print outlif dbal information, using the dbal entry get shell command.
         */
        cli_out("DBAL info:\n");
        sal_memset(shell_commands_buffer, 0, CHAR_BUFF_SIZE);

        sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s out_lif=%d result_type=%s",
                     dbal_entry_get_command, logical_table_str, lif_hw_resources.local_out_lif, result_type_str);
        SHR_IF_ERR_EXIT(sh_process_command(unit, shell_commands_buffer));

        cli_out("\n\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * List of the supported commands, pointer to command function and command usage function.
 */

static sh_sand_man_t sh_dnx_gport_to_hw_resources_man = {
    .brief = "Given a gport, display the hw resources related to it: Global and local lif, and fec.",
    .full =
        "All hw resources connected to this gport will be displayed. Non existing resources will not be displayed.\n"
        "A full dbal dump of each resource will also be displayed.",
};

static sh_sand_option_t sh_dnx_gport_to_hw_resources_options[] = {
    {"gport", SAL_FIELD_TYPE_INT32, "gport.", "-1", NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

sh_sand_cmd_t sh_dnx_gpm_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"HW_RESources", sh_dnx_gport_to_hw_resoures_cmd, NULL, sh_dnx_gport_to_hw_resources_options,
     &sh_dnx_gport_to_hw_resources_man},
    {NULL}
};

sh_sand_man_t sh_dnx_gpm_man = {
    "Display gport information.",
    NULL,
};
