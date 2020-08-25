/** \file diag_dnx_ingress_prd.c
 * 
 * diagnostics for Port Priority Drop
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COSQ

/*
 * Include files.
 * {
 */
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <shared/util.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/appl/sal.h>

#include <bcm/switch.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

#include "diag_dnx_ingress_prd.h"
/*
 * }
 */

/*
 * Macros
 * {
 */

#define DNX_DIAG_PORT_PRD_MAX_STR_LEN            32
/*
 * Assemble the PRD priority string, for exampe if the
 * available priority is 0 and 1, the final string should be "0, 1"
 */
#define DNX_DIAG_PORT_PRD_PRIORITY_STR_ASSEMBLE(prd_prio_str, prio_bmp, priority)  \
    do {                                                                           \
        char tmp_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN] = "";                          \
        sal_sprintf(tmp_str, (*prio_bmp == 0) ? "%d" : ",%d", priority);           \
        sal_strncat(prd_prio_str, tmp_str, sizeof(prd_prio_str) -  sal_strlen(prd_prio_str) - 1); \
        SHR_BITSET(prio_bmp, priority);                                            \
    } while (0)

/*
 * }
 */

/**
* Functions
* {
*/

/**
 * \brief - Dump the port PRD info
 *
 * \param [in] unit - chip unit id
 * \param [in] args - diag command arguments
 * \param [in] sand_control - diag comman control
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

shr_error_e
sh_dnx_ingress_port_drop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t nif_pbmp;
    bcm_port_t port;
    bcm_port_prio_config_t priority_config;
    int priority_group, priority, prio_count = 0;
    uint32 src_prio, threshold, prio_bmp[1];
    uint64 drop_count = 0;
    bcm_port_nif_scheduler_t sch_prio;
    char *header_str = NULL;
    char *sch_prio_str[] = { "Low", "High", "TDM" };
    char prd_prio_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char threshold_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char drop_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    dnx_algo_port_type_e port_type;
    bcm_switch_control_key_t switch_control_key;
    bcm_switch_control_info_t switch_control_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("Ingress Port Drop");

    PRT_COLUMN_ADD("port");
    PRT_COLUMN_ADD("PRD Port Type");
    PRT_COLUMN_ADD("PRD Priority");
    PRT_COLUMN_ADD("Sch Priority");
    PRT_COLUMN_ADD("FIFO Size(in entries)");
    PRT_COLUMN_ADD("Threshold");
    PRT_COLUMN_ADD("PRD Drop count");

    SH_SAND_GET_PORT("port", nif_pbmp);

    BCM_PBMP_ITER(nif_pbmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
        /*
         * Skip the irrelevant port
         */
        if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, FALSE /* exclude elk */ , FALSE /* exclude stif */ ))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(bcm_port_priority_config_get(unit, port, &priority_config));

        for (priority_group = 0; priority_group < priority_config.nof_priority_groups; ++priority_group)
        {
            /*
             * Get the PRD Priority
             */
            src_prio = priority_config.priority_groups[priority_group].source_priority;

            /** Clear the variable */
            *prio_bmp = 0;
            sal_memset(prd_prio_str, '\0', sizeof(prd_prio_str));

            /** Assemble the priority string. */
            if (src_prio & BCM_PORT_F_PRIORITY_0)
            {
                DNX_DIAG_PORT_PRD_PRIORITY_STR_ASSEMBLE(prd_prio_str, prio_bmp, 0);
            }
            if (src_prio & BCM_PORT_F_PRIORITY_1)
            {
                DNX_DIAG_PORT_PRD_PRIORITY_STR_ASSEMBLE(prd_prio_str, prio_bmp, 1);
            }
            if (src_prio & BCM_PORT_F_PRIORITY_2)
            {
                DNX_DIAG_PORT_PRD_PRIORITY_STR_ASSEMBLE(prd_prio_str, prio_bmp, 2);
            }
            if (src_prio & BCM_PORT_F_PRIORITY_3)
            {
                DNX_DIAG_PORT_PRD_PRIORITY_STR_ASSEMBLE(prd_prio_str, prio_bmp, 3);
            }

            /*
             * Get the PRD drop count
             */
            sch_prio = priority_config.priority_groups[priority_group].sch_priority;
            SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, sch_prio, &drop_count));
            format_uint64_decimal(drop_str, drop_count, ',');

            /*
             * Get the PRD header type
             */
            switch_control_key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN;
            switch_control_key.type = bcmSwitchPortHeaderType;
            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(unit, port, switch_control_key, &switch_control_info));
            switch (switch_control_info.value)
            {
                case BCM_SWITCH_PORT_HEADER_TYPE_ETH:
                    header_str = "ETH";
                    break;
                case BCM_SWITCH_PORT_HEADER_TYPE_TM:
                    header_str = "ITMH";
                    break;
                case BCM_SWITCH_PORT_HEADER_TYPE_STACKING:
                    header_str = "FTMH";
                    break;
                case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP:
                case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2:
                    header_str = "PTCH_2";
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported header type %d\n", switch_control_info.value);
            }

            /*
             * Get the Threshold and Print the table for each row
             */
            prio_count = 0;
            SHR_BIT_ITER(prio_bmp, dnx_data_nif.prd.nof_priorities_get(unit), priority)
            {
                SHR_IF_ERR_EXIT(bcm_cosq_ingress_port_drop_threshold_get(unit, port, 0, priority, &threshold));
                sal_sprintf(threshold_str, "%d - %u", priority, threshold);

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                if ((prio_count == 0) && (priority_group == 0))
                {
                    /** Print the first row for each port */
                    PRT_CELL_SET("%d", port);

                    PRT_CELL_SET("%s", header_str);
                    PRT_CELL_SET("%s", prd_prio_str);
                    PRT_CELL_SET("%s", sch_prio_str[sch_prio]);
                    PRT_CELL_SET("%d", priority_config.priority_groups[priority_group].num_of_entries);
                    PRT_CELL_SET("%s", threshold_str);
                    PRT_CELL_SET("%s", drop_str);
                }
                else if ((prio_count == 0) && (priority_group != 0))
                {
                    /** Print the first row for each priority group, except the first row for each port*/
                    PRT_CELL_SET("");
                    PRT_CELL_SET("");
                    PRT_CELL_SET("%s", prd_prio_str);
                    PRT_CELL_SET("%s", sch_prio_str[sch_prio]);
                    PRT_CELL_SET("%d", priority_config.priority_groups[priority_group].num_of_entries);
                    PRT_CELL_SET("%s", threshold_str);
                    PRT_CELL_SET("%s", drop_str);
                }
                else
                {
                    /** Print the reset rows */
                    PRT_CELL_SET("");
                    PRT_CELL_SET("");
                    PRT_CELL_SET("");
                    PRT_CELL_SET("");
                    PRT_CELL_SET("");
                    PRT_CELL_SET("%s", threshold_str);
                    PRT_CELL_SET("");
                }
                prio_count++;
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;

}

/**
 * \brief DNX Port PRD dignostics
 * List of the supported commands, pointer to command function and command usage function.
 */

/* *INDENT-OFF* */


sh_sand_option_t sh_dnx_ingress_port_drop_options[] = {
    /*keyword,   action,                    command, options,                            man*/
    {"port",      SAL_FIELD_TYPE_PORT, "port # / logical port type / port name",    "nif",      NULL},
    {NULL}
};

sh_sand_man_t sh_dnx_ingress_port_drop_man = {
    .brief    = "Diagnostic pack for NIF PRD",
    .full     = "Diagnostic pack for NIF PRD",
    .synopsis = "[port=<integer|port_name|nif>]",
    .examples = "\n"
                "port=1\n"
                "port=xe\n"
                "port=nif"
};

/* *INDENT-ON* */
/*
 * }
 */
#undef _ERR_MSG_MODULE_NAME
