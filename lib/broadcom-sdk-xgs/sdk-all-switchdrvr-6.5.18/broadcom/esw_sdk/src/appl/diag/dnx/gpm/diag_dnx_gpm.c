/** \file diag_dnx_gpm.c
 *
 * Diagnostics for the allocation managers.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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

/**
 * \brief
 *  QoS cell format
 */
typedef enum
{
    /** IN-LIF */
    LIF_DIRECTION_IN,
    /** OUT-LIF */
    LIF_DIRECTION_OUT,
    /** BOTH IN-LIF and OUT-LIF */
    LIF_DIRECTION_IN_OUT,
    /** Must be last */
    NOF_LIF_DIRECTIONS
} dnx_lif_direction_e;

/**External function declaration */
extern const char *strcaseindex(
    const char *s,
    const char *sub);

/*************
 *  MACROES  *
 *************/

/*************
 * FUNCTIONS *
 *************/

static shr_error_e
sh_dnx_gport_to_hw_resources_cmd_internal(
    int unit,
    int gport,
    dnx_lif_direction_e direction)
{
    dnx_algo_gpm_gport_hw_resources_t fec_hw_resources, lif_hw_resources;
    uint32 flags;
    char shell_commands_buffer[CHAR_BUFF_SIZE];
    char *dbal_entry_get_command = "dbal eNTry get table=";
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&fec_hw_resources, 0, sizeof(fec_hw_resources));
    sal_memset(&lif_hw_resources, 0, sizeof(lif_hw_resources));

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
        SHR_CLI_EXIT(_SHR_E_NONE, "Given gport 0x%08x has no hardware resources or doesn't exist.\n", gport);
    }

    /*
     * Print fec information, if it's valid.
     */
    if (fec_hw_resources.fec_id != DNX_ALGO_GPM_FEC_INVALID)
    {
        uint32 fec_hierarchy;
        dbal_tables_e fec_dbal_table;
        LOG_CLI((BSL_META("FEC info:\n")));

        SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get(unit, fec_hw_resources.fec_id, &fec_hierarchy));

        LOG_CLI((BSL_META("gport:0x%08X -> FEC ID:%d -> hierarchy: %d\n"), gport, fec_hw_resources.fec_id,
                 fec_hierarchy + 1));

        /*
         * Print fec dbal information, using the dbal entry get shell command.
         */
        fec_dbal_table = L3_FEC_TABLE_FROM_HIERARCHY_GET(fec_hierarchy);

        sal_memset(shell_commands_buffer, 0, CHAR_BUFF_SIZE);

        sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s super_fec_id=%d",
                     dbal_entry_get_command,
                     dbal_logical_table_to_string(unit, fec_dbal_table),
                     DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(fec_hw_resources.fec_id));
        SHR_IF_ERR_EXIT(sh_process_command(unit, shell_commands_buffer));

        LOG_CLI((BSL_META("\n\n")));
    }

    /*
     * Print inlif information, if it's valid.
     */
    if ((direction == LIF_DIRECTION_IN) || (direction == LIF_DIRECTION_IN_OUT))
    {
        if (lif_hw_resources.local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            const char *logical_table_str = dbal_logical_table_to_string(unit, lif_hw_resources.inlif_dbal_table_id);
            const char *result_type_str = dbal_result_type_to_string(unit, lif_hw_resources.inlif_dbal_table_id,
                                                                     lif_hw_resources.inlif_dbal_result_type);
            LOG_CLI((BSL_META("INLIF info:\n")));

            if (lif_hw_resources.global_in_lif != DNX_ALGO_GPM_LIF_INVALID)
            {
                LOG_CLI((BSL_META("gport:0x%08X -> Global inlif:0x%08X -> "), gport, lif_hw_resources.global_in_lif));
            }
            else
            {
                LOG_CLI((BSL_META("No global inlif for this gport (0x%08X).\n"), gport));
            }

            LOG_CLI((BSL_META("Local inlif:0x%08X\n"), lif_hw_resources.local_in_lif));

            LOG_CLI((BSL_META("Table: %s"), logical_table_str));

            /*
             * Print inlif dbal information, using the dbal entry get shell command.
             */
            sal_memset(shell_commands_buffer, 0, CHAR_BUFF_SIZE);

            sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s core_id=%d in_lif=%d result_type=%s",
                         dbal_entry_get_command,
                         logical_table_str,
                         (lif_hw_resources.local_in_lif_core == _SHR_CORE_ALL) ? 0 : lif_hw_resources.local_in_lif_core,
                         lif_hw_resources.local_in_lif, result_type_str);
            SHR_IF_ERR_EXIT(sh_process_command(unit, shell_commands_buffer));
        }
        else
        {
            LOG_CLI((BSL_META("No INLIF information for gport 0x%08X.\n"), gport));
        }

        LOG_CLI((BSL_META("\n\n")));
    }

    if ((direction == LIF_DIRECTION_OUT) || (direction == LIF_DIRECTION_IN_OUT))
    {
        /*
         * Print outlif information, if it's valid.
         */
        if (lif_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            const char *logical_table_str = dbal_logical_table_to_string(unit, lif_hw_resources.outlif_dbal_table_id);
            const char *result_type_str = dbal_result_type_to_string(unit, lif_hw_resources.outlif_dbal_table_id,
                                                                     lif_hw_resources.outlif_dbal_result_type);
            LOG_CLI((BSL_META("OUTLIF info:\n")));

            if (lif_hw_resources.global_out_lif != DNX_ALGO_GPM_LIF_INVALID)
            {
                LOG_CLI((BSL_META("gport:0x%08X -> Global outlif:0x%08X -> "), gport, lif_hw_resources.global_out_lif));
            }
            else
            {
                LOG_CLI((BSL_META("No global outlif for this gport (0x%08X).\n"), gport));
            }

            LOG_CLI((BSL_META("Local outlif:0x%08X -> phase:%d\n"), lif_hw_resources.local_out_lif,
                     lif_hw_resources.outlif_phase + 1));
            LOG_CLI((BSL_META("Table: %s"), logical_table_str));
            /*
             * Print outlif dbal information, using the dbal entry get shell command.
             */
            sal_memset(shell_commands_buffer, 0, CHAR_BUFF_SIZE);

            sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s out_lif=%d result_type=%s",
                         dbal_entry_get_command, logical_table_str, lif_hw_resources.local_out_lif, result_type_str);
            SHR_IF_ERR_EXIT(sh_process_command(unit, shell_commands_buffer));
        }
        else
        {
            LOG_CLI((BSL_META("No OUTLIF information for gport 0x%08X.\n"), gport));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_gport_to_hw_resources_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int gport;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("gport", gport);

    SHR_IF_ERR_EXIT(sh_dnx_gport_to_hw_resources_cmd_internal(unit, gport, LIF_DIRECTION_IN_OUT));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_lif_to_hw_resources_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int gport, lif, is_global_lif, core;
    int is_ingress, is_egress, is_dpc, lif_direction;
    char *direction;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("id", lif);
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_BOOL("global", is_global_lif);
    SH_SAND_GET_STR("type", direction);
    SH_SAND_GET_BOOL("dpc", is_dpc);

    is_ingress = strcaseindex(direction, "i") || strcaseindex(direction, "b");
    is_egress = strcaseindex(direction, "o") || strcaseindex(direction, "b");

    lif_direction = (is_ingress && is_egress) ? LIF_DIRECTION_IN_OUT :
        (is_ingress && !is_egress) ? LIF_DIRECTION_IN :
        (!is_ingress && is_egress) ? LIF_DIRECTION_OUT : NOF_LIF_DIRECTIONS;

    gport = BCM_GPORT_INVALID;
    flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
    if (is_ingress)
    {
        if (is_global_lif)
        {
            flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
        }
        else if (is_dpc)
        {
            flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS;
        }
        else
        {
            flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS;
        }
    }
    else if (is_egress)
    {
        if (is_global_lif)
        {
            flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        }
        else
        {
            flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        }

        /*
         * Set to meaningless value, this will not used.
         */
        is_dpc = 2;
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_NONE, "Legal options for lif direction TYpe: in,out,both.\n");
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, flags, core, lif, &gport));

    if (gport == BCM_GPORT_INVALID)
    {
        SHR_CLI_EXIT(_SHR_E_NONE, "Failed to translate LIF to gport, given %s %sLIF may not exist.\n",
                     is_global_lif ? "global" : "local",
                     (lif_direction == LIF_DIRECTION_IN) ? "in" : ((lif_direction == LIF_DIRECTION_OUT) ? "out" : ""));
    }

    SHR_IF_ERR_EXIT(sh_dnx_gport_to_hw_resources_cmd_internal(unit, gport, lif_direction));

exit:
    SHR_FUNC_EXIT;
}

#define DIAG_DNX_VLAN_PORT_ID 0x44804572

/*
 * Create a simple vlan port for the diagnostic.
 */
shr_error_e
sh_dnx_gport_diagnostics_init(
    int unit)
{
    bcm_vlan_port_t vp;
    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vp.flags = BCM_VLAN_PORT_WITH_ID;
    vp.vlan_port_id = DIAG_DNX_VLAN_PORT_ID;

    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Destroy the simple vlan port for the diagnostic.
 */
shr_error_e
sh_dnx_gport_diagnostics_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, DIAG_DNX_VLAN_PORT_ID));

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
    .init_cb = sh_dnx_gport_diagnostics_init,
    .deinit_cb = sh_dnx_gport_diagnostics_deinit,
    .examples = "gport=0x44804572"
};

static sh_sand_option_t sh_dnx_gport_to_hw_resources_options[] = {
    {"gport", SAL_FIELD_TYPE_INT32, "gport.", NULL, NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

static sh_sand_man_t sh_dnx_lif_to_hw_resources_man = {
    .brief = "Given a lif, display its gport and the hw resources related to it: Global and local lif.",
    .full =
        "All hw resources connected to this lif will be displayed. Non existing resources will not be displayed.\n"
        "A full dbal dump of each resource will also be displayed.",
    .init_cb = sh_dnx_gport_diagnostics_init,
    .deinit_cb = sh_dnx_gport_diagnostics_deinit,
    .examples = "id=0x1000 type=out global=true"
};

static sh_sand_option_t sh_dnx_lif_to_hw_resources_options[] = {
    {"ID", SAL_FIELD_TYPE_INT32, "lif id.", NULL, NULL, NULL, 0},
    {"GLOBal", SAL_FIELD_TYPE_BOOL, "global lif indication.", "1", NULL, NULL, 0},
    {"TYpe", SAL_FIELD_TYPE_STR, "lif direction: in, out or both.", "out", NULL, NULL, 0},
    {"DPC", SAL_FIELD_TYPE_BOOL, "if lif is local ingress, then DPC/SBC indication. Otherwise, ignored. "
     "If DPC(boolean:1), core id is required, or leave empty for all cores.", "0", NULL,
     NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

sh_sand_cmd_t sh_dnx_gpm_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"HW", sh_dnx_gport_to_hw_resources_cmd, NULL, sh_dnx_gport_to_hw_resources_options,
     &sh_dnx_gport_to_hw_resources_man},
    {"LIF", sh_dnx_lif_to_hw_resources_cmd, NULL, sh_dnx_lif_to_hw_resources_options,
     &sh_dnx_lif_to_hw_resources_man},
    {NULL}
};

sh_sand_man_t sh_dnx_gpm_man = {
    "Display gport information.",
    NULL,
};
