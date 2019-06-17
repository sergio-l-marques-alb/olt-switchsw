/**
 * \file diag_dnx_visibility.c
 *
 *  Created on: Jan 13, 2018
 *      Author: mf954075
 *
 * Visibility management shell commands.
 */

#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * INCLUDES
 * {
 */
/*
 * shared
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
#include <shared/utilex/utilex_bitstream.h>
/*
 * soc
 */
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
/*
 * bcm
 */
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm/trunk.h>
/*
 * appl
 */
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

#include "tm/diag_dnx_lag.h"
/*
 * }
 */

/**
 * Keeping all the tokens form general shell to be compliant :)
 */
static sh_sand_enum_t dnx_visibility_modes[] = {
    {"show", -1, "Used to represent get command instead of set"},
    {"None", bcmInstruVisModeNone, "No visibility mode - visibility is disabled"},
    {"Always", bcmInstruVisModeAlways,
     "Always mode - last packet is recorded, ignoring visibility configuration, AKA J1"},
    {"Select", bcmInstruVisModeSelective, "First packet macthing visibiity setting will be recorded"},
    {NULL}
};

/**
 * Keeping all the tokens form general shell to be compliant :)
 */
static sh_sand_enum_t dnx_vis_directions[] = {
    {"Ingress", BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS, "Controls ingress PP port based visibility"},
    {"Egress", BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, "Controls egress TM port based visibility"},
    {"Both", BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
     "Relevant for ingress & egress part of PP"},
    {NULL}
};

typedef enum
{
    /*
     * Switch something on
     */
    DNX_COMMAND_OFF = FALSE,
    /*
     * Switch something off
     */
    DNX_COMMAND_ON = TRUE,
    /*
     * DO nothing - probably show
     */
    DNX_COMMAND_NONE,
} dnx_command_e;

/**
 * Enable style command enum
 */
static sh_sand_enum_t dnx_visibility_command[] = {
    {"On", DNX_COMMAND_ON, "Property is set"},
    {"Off", DNX_COMMAND_OFF, "Property is reset"},
    {"None", DNX_COMMAND_NONE, "Property is not modified"},
    {NULL}
};

static shr_error_e
visibility_device_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int mode;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("mode", mode);

    if (mode == -1)
    {
        /*
         * Show current state
         */
        bcm_instru_vis_mode_control_t get_mode;
        SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &get_mode, TRUE));
        LOG_CLI((BSL_META("Visibility Mode %s\n"), sh_sand_enum_str(dnx_visibility_modes, get_mode)));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_visibility_mode_set(unit, mode));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
visibility_port_show(
    int unit,
    uint32 direction,
    sh_sand_control_t * sand_control,
    bcm_pbmp_t * logical_ports_p)
{
    bcm_port_t logical_port;
    bcm_pbmp_t logical_ports;
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    uint32 flags;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(logical_ports_p, _SHR_E_PARAM, "logical_ports_p");

    PRT_TITLE_SET("Visibility Status Per Port");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "PP Port");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "TM Port");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Core");
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Ingress Enable");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Ingress Force");
    }
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Egress Enable");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Egress Force");
    }

    logical_ports = *logical_ports_p;
    BCM_PBMP_ITER(logical_ports, logical_port)
    {
        dnx_algo_port_type_e port_type;
        uint32 pp_port, tm_port;
        char port_type_str[DNX_ALGO_PORT_TYPE_STR_LENGTH];

        flags = 0;
        sal_memset(&port_interface_info, 0x0, sizeof(bcm_port_interface_info_t));
        sal_memset(&port_mapping_info, 0x0, sizeof(bcm_port_mapping_info_t));

        SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &port_interface_info, &port_mapping_info));
        if ((flags & (BCM_PORT_ADD_KBP_PORT | BCM_PORT_ADD_STIF_PORT)) ||
                                                                    (port_interface_info.interface == BCM_PORT_IF_SFI))
        {
            continue;
        }

        pp_port = port_mapping_info.pp_port;
        tm_port = port_mapping_info.tm_port;

        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get(unit, port_type, port_type_str));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", BCM_PORT_NAME(unit, logical_port));

        if(pp_port == -1)
        {
            PRT_CELL_SET("N/A");
        }
        else
        {
            PRT_CELL_SET("%d", pp_port);
        }
        PRT_CELL_SET("%d", tm_port);
        PRT_CELL_SET("%s", port_type_str);
        PRT_CELL_SET("%d", port_mapping_info.core);

        if(pp_port == -1)
        {
            PRT_CELL_SET("N/A");
            PRT_CELL_SET("N/A");
        }
        else
        {
            if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
            {
                int status;
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_get(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                                   pp_port, &status), "Port Enable get failed\n");
                PRT_CELL_SET("%d", status);
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_get(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                                  pp_port, &status), "Port Force get failed\n");
                PRT_CELL_SET("%d", status);
            }
        }
        if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
        {
            int status;
            SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_get(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                               tm_port, &status), "Port Enable get failed\n");
            PRT_CELL_SET("%d", status);
            SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_get(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                              tm_port, &status), "Port Force get failed\n");
            PRT_CELL_SET("%d", status);
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
visibility_port_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    dnx_command_e enable, force;
    uint32 direction;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("name", logical_ports);
    SH_SAND_GET_ENUM("enable", enable);
    SH_SAND_GET_ENUM("force", force);
    SH_SAND_GET_ENUM("direction", direction);

    if ((force == DNX_COMMAND_NONE) && (enable == DNX_COMMAND_NONE))
    {
        /*
         * Show port status
         */
        visibility_port_show(unit, direction, sand_control, &logical_ports);
    }
    else
    {
        if (force != DNX_COMMAND_NONE)
        {
            BCM_PBMP_ITER(logical_ports, port)
            {
                dnx_algo_port_type_e port_type;
                SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
                if (!DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
                    continue;

                SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, direction, port, force), "Port Force failed\n");
            }
        }
        if (enable != DNX_COMMAND_NONE)
        {
            BCM_PBMP_ITER(logical_ports, port)
            {
                dnx_algo_port_type_e port_type;
                SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
                if (!DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
                    continue;

                SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, direction, port, enable),
                                    "Port Enable failed\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file / man section */
static shr_error_e
visibility_lag_show(
    int unit,
    uint32 in_pool,
    uint32 in_group,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Print table header */
    PRT_TITLE_SET("LAGs");
    PRT_COLUMN_ADD("Pool");
    PRT_COLUMN_ADD("Group");
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("PP Port");
    PRT_COLUMN_ADD("Enable");
    PRT_COLUMN_ADD("Force");
    for (int pool = 0; pool < dnx_data_trunk.parameters.nof_pools_get(unit); ++pool)
    {
        bcm_trunk_t trunk_id;
        bcm_trunk_pp_port_allocation_info_t allocation_info;
        int core;
        int max_nof_groups_in_pool = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_groups_in_pool;
        for (int group = 0; group < max_nof_groups_in_pool; ++group)
        {
            int trunk_group_is_used;
                /** Get info */
            SHR_CLI_EXIT_IF_ERR(dnx_trunk_is_used_get(unit, pool, group, &trunk_group_is_used), "");
            if (!trunk_group_is_used)
                continue;

            if((in_pool != -1) && (in_pool != pool))
                continue;

            if((in_group != -1) && (in_group != group))
                continue;

            BCM_TRUNK_ID_SET(trunk_id, pool, group);
            sal_memset(&allocation_info, 0, sizeof(bcm_trunk_pp_port_allocation_info_t));
            SHR_CLI_EXIT_IF_ERR(bcm_trunk_pp_port_allocation_get(unit, trunk_id, 0, &allocation_info), "");

            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {
                uint32 status;
                int pp_port = allocation_info.pp_port_per_core_array[core];
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                if(core == 0)
                {
                    PRT_CELL_SET("%d", pool);
                    PRT_CELL_SET("%d", group);
                }
                else
                {
                    PRT_CELL_SKIP(2);
                }
                PRT_CELL_SET("%d", core);
                PRT_CELL_SET("%d", pp_port);
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_pp_port_enable_get(unit, core, pp_port, &status),
                                                                                        "Port Enable get failed\n");
                PRT_CELL_SET("%d", status);
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_pp_port_force_get(unit, core, pp_port, &status),
                                                                                        "Port Force get failed\n");
                PRT_CELL_SET("%d", status);
            }
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
visibility_lag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_command_e enable, force;
    uint32 pool, group;
    int in_core;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("pool", pool);
    SH_SAND_GET_INT32("group", group);
    SH_SAND_GET_INT32("core", in_core);
    SH_SAND_GET_ENUM("enable", enable);
    SH_SAND_GET_ENUM("force", force);

    if ((force == DNX_COMMAND_NONE) && (enable == DNX_COMMAND_NONE))
    {
        /*
         * Show port status
         */
        SHR_CLI_EXIT_IF_ERR(visibility_lag_show(unit, pool, group, sand_control), "");
    }
    else
    {
        bcm_trunk_t trunk_id;
        bcm_trunk_pp_port_allocation_info_t allocation_info;
        int core;

        if((pool == -1) || (group == -1))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Lag pool and group need to be provided for this operation\n");
        }
        BCM_TRUNK_ID_SET(trunk_id, pool, group);
        sal_memset(&allocation_info, 0, sizeof(bcm_trunk_pp_port_allocation_info_t));
        SHR_CLI_EXIT_IF_ERR(bcm_trunk_pp_port_allocation_get(unit, trunk_id, 0, &allocation_info), "");

        if (force != DNX_COMMAND_NONE)
        {
            DNXCMN_CORES_ITER(unit, in_core, core)
            {
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_pp_port_force_set(unit, core,
                                                                     allocation_info.pp_port_per_core_array[core],
                                                                     force), "Port Force Set failed\n");
            }
        }
        if (enable != DNX_COMMAND_NONE)
        {
            DNXCMN_CORES_ITER(unit, in_core, core)
            {
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_pp_port_enable_set(unit, core,
                                                                      allocation_info.pp_port_per_core_array[core],
                                                                      enable), "Port Enable Set failed\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
visibility_qual_show(
    int unit,
    sh_sand_control_t * sand_control,
    int qual_id_first,
    int qual_id_last)
{
    int i_qual;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Visibility Status Per PRT Qualifier");
    PRT_COLUMN_ADD("QUAL ID");
    PRT_COLUMN_ADD("Enable");

    for (i_qual = qual_id_first; i_qual <= qual_id_last; i_qual++)
    {
        int status;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", i_qual);

        SHR_CLI_EXIT_IF_ERR(dnx_visibility_prt_qualifier_get(unit, i_qual, &status), "PRT Qual get failed\n");
    PRT_CELL_SET("%d", status)}

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
visibility_qual_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int qual_id_first, qual_id_last;
    dnx_command_e enable;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE("id", qual_id_first, qual_id_last);
    SH_SAND_GET_ENUM("enable", enable);

    if (enable == DNX_COMMAND_NONE)
    {
        /*
         * Show port status
         */
        SHR_CLI_EXIT_IF_ERR(visibility_qual_show(unit, sand_control, qual_id_first, qual_id_last),
                            "Problem showing visibility status");
    }
    else
    {
        if (enable != DNX_COMMAND_NONE)
        {
            int i_qual;
            for (i_qual = qual_id_first; i_qual <= qual_id_last; i_qual++)
            {
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_prt_qualifier_set(unit, i_qual, enable),
                                    "PRT Qualifier:%d Visibility Set failed", i_qual);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
visibility_resume_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core;
    uint32 direction;
    int tmp_value, flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_ENUM("direction", direction);
    SH_SAND_GET_BOOL("zero", tmp_value);
    if (tmp_value == TRUE)
        flags |= DNX_VISIBILITY_RESUME_ZERO;

    SH_SAND_GET_BOOL("memory", tmp_value);
    if (tmp_value == TRUE)
        flags |= DNX_VISIBILITY_RESUME_MEM;

    SHR_IF_ERR_EXIT(dnx_visibility_resume(unit, core, direction, flags));

    /** Reset TCAM hit indication register in case of visibility resume command was invoked. */
    SHR_IF_ERR_EXIT(bcm_field_entry_hit_flush(unit, BCM_FIELD_ENTRY_HIT_FLUSH_ALL, 0));

exit:
    SHR_FUNC_EXIT;
}

static char *
visibility_ready_str(
    shr_error_e status)
{
    char *status_str;
    switch (status)
    {
        case _SHR_E_NONE:
            status_str = "Ready";
            break;
        case _SHR_E_EMPTY:
            status_str = "Not Ready";
            break;
        default:
            status_str = "Error";
            break;
    }
    return status_str;
}

static shr_error_e
visibility_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_id, core_idx;
    shr_error_e status;
    int block_flag;
    bcm_instru_vis_mode_control_t get_mode;
    dbal_enum_value_field_pp_asic_block_e asic_block;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_BOOL("block", block_flag);

    PRT_TITLE_SET("Debug Memory Status");

    if (block_flag == TRUE)
    {
        PRT_COLUMN_ADD("Block");
    }
    else
    {
        PRT_COLUMN_ADD("Direction");
    }
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Status");

    SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &get_mode, TRUE));
    PRT_INFO_ADD("  Visibility Mode - \"%s\"", sh_sand_enum_str(dnx_visibility_modes, get_mode));

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        if (block_flag == TRUE)
        {
            for (asic_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA; asic_block <= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC;
                 asic_block++)
            {
                status = dnx_debug_mem_is_ready(unit, core_idx, asic_block);

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", dnx_debug_mem_name(unit, asic_block));
                PRT_CELL_SET("%d", core_idx);
                PRT_CELL_SET("%s", visibility_ready_str(status));
            }
        }
        else
        {
            status = dnx_debug_direction_is_ready(unit, core_idx, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                                  DNX_VISIBILITY_BLOCK_UNDEFINED);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "Ingress");
            PRT_CELL_SET("%d", core_idx);
            PRT_CELL_SET("%s", visibility_ready_str(status));

            status = dnx_debug_direction_is_ready(unit, core_idx, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                                  DNX_VISIBILITY_BLOCK_UNDEFINED);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "Egress");
            PRT_CELL_SET("%d", core_idx);
            PRT_CELL_SET("%s", visibility_ready_str(status));
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief - Man page for 'visibility mode' shell command
 */
static sh_sand_man_t visibility_device_man = {
    .brief = "Manage Device visibility abilities",
    .synopsis = NULL,   /* NULL means autogenerated */
    .examples = "mode=select\n" "mode=always" "\n"
};
/**
 * \brief - Options list for 'visibility mode' shell command
 */
static sh_sand_option_t visibility_device_args[] = {
   /* Name      | Type                | Description          | Default     */
    {"mode",      SAL_FIELD_TYPE_ENUM, "Visibility Mode Type", "show", dnx_visibility_modes},
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief - Man page for 'visibility qualifier' shell command
 */
static sh_sand_man_t visibility_qual_man = {
    .brief = "Manage PRT Qualifier visibility properties",
    .examples = "enable=on id=1\n"
};
/**
 * \brief - Options list for 'visibility qualifier' shell command
 */
static sh_sand_option_t visibility_qual_args[] = {
   /* Name      | Type                | Description          | Default     */
    {"enable",    SAL_FIELD_TYPE_ENUM, "Enable/Disable qualifier visibility ", "none", dnx_visibility_command},
    {"id",        SAL_FIELD_TYPE_UINT32, "PRT Qualifier ID", "0-2"},
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief - Man page for 'visibility port' shell command
 */
static sh_sand_man_t visibility_port_man = {
    .brief = "Manage port visibility properties",
    .full = "Manage port visibility properties, use without arguments to present current state",
    .synopsis = "[name=]<port_name> [enable=<on|off]>] [force=<on|off>]",
    .examples = "\n" "xe13 enable=on\n" "force=on name=xe14\n" "enable=off direction=ingress name=all\n" "enable=on"
};
/**
 * \brief - Options list for 'visibility mode' shell command
 */
static sh_sand_option_t visibility_port_args[] = {
   /* Name      | Type                | Description          | Default     */
    {"direction", SAL_FIELD_TYPE_ENUM, "Visibility Direction", "both", dnx_vis_directions},
    {"enable",    SAL_FIELD_TYPE_ENUM, "Enable/Disable port visibility ", "none", dnx_visibility_command},
    {"force",     SAL_FIELD_TYPE_ENUM, "Force/Unforce port visibility", "none", dnx_visibility_command},
    {"name",      SAL_FIELD_TYPE_PORT, "Port Name or type", "all", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}      /* End of options list - must be last. */
};


/**
 * \brief - Man page for 'visibility port' shell command
 */
static sh_sand_man_t visibility_lag_man = {
    .brief = "Manage lag visibility properties",
    .init_cb = sh_dnx_lag_init_cb,
    .synopsis = "[enable=<on|off]>] [force=<on|off>] [pool=<pool id>] [group=<group id>]",
    .examples = "\n enable=off pool=0 group=21\n force=on pool=0 group=22"
};

static sh_sand_option_t visibility_lag_args[] = {
    /*name        type                  desc                 default ext                             valid_tange */
    {"pool",      SAL_FIELD_TYPE_INT32, "lag pool",          "-1",   (void*)sh_enum_table_lag_pool,  "0-max"},
    {"group",     SAL_FIELD_TYPE_INT32, "lag group in pool", "-1"},
    {"enable",    SAL_FIELD_TYPE_ENUM,  "Enable/Disable port visibility ", "none", dnx_visibility_command},
    {"force",     SAL_FIELD_TYPE_ENUM,  "Force/Unforce port visibility",   "none", dnx_visibility_command},
    {NULL}
};

/**
 * \brief - Man page for 'visibility port' shell command
 */
static sh_sand_man_t visibility_resume_man = {
    .brief = "Clear debug memory and allow for next packet to be recorded",
    .examples = "direction=ingress\n" "core=1\n" "zero\n" "memory=no\n"
};
/**
 * \brief - Options list for 'visibility mode' shell command
 */
static sh_sand_option_t visibility_resume_args[] = {
   /* Name      | Type                | Description          | Default     */
    {"direction", SAL_FIELD_TYPE_ENUM, "Visibility Direction", "both", dnx_vis_directions},
    {"zero",      SAL_FIELD_TYPE_BOOL, "NUllify the cache in addition to invalidation", "no"},
    {"memory",    SAL_FIELD_TYPE_BOOL, "Clear valids on debug memory", "yes"},
    {NULL}      /* End of options list - must be last. */
};

static sh_sand_man_t visibility_status_man = {
    .brief = "Show readiness status for debug memories",
    .examples = "core=1\n" "block"
};

/**
 * \brief - Options list for 'visibility mode' shell command
 */
static sh_sand_option_t visibility_status_args[] = {
   /* Name      | Type                | Description          | Default     */
    {"block",      SAL_FIELD_TYPE_BOOL, "Show status per block", "no"},
    {NULL}      /* End of options list - must be last. */
};
sh_sand_man_t sh_dnx_visibility_man = {
    .brief = "Visibility Control commands",
};

sh_sand_cmd_t sh_dnx_visibility_cmds[] = {
  /* Name       | Leaf Action         | Node | Options for Leaf     | Usage                */
    {"global",    visibility_device_cmd, NULL,  visibility_device_args, &visibility_device_man},
    {"port",      visibility_port_cmd,   NULL,  visibility_port_args,   &visibility_port_man},
    {"lag",       visibility_lag_cmd,    NULL,  visibility_lag_args,    &visibility_lag_man},
    {"qualifier", visibility_qual_cmd,   NULL,  visibility_qual_args,   &visibility_qual_man},
    {"resume",    visibility_resume_cmd, NULL,  visibility_resume_args, &visibility_resume_man},
    {"status",    visibility_status_cmd, NULL,  visibility_status_args, &visibility_status_man},
    {NULL}
};
/* *INDENT-ON* */
