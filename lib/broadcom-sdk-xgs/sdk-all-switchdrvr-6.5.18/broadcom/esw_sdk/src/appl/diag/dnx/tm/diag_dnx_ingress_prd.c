/** \file diag_dnx_ingress_prd.c
 * 
 * diagnostics for Port Priority Drop
 * 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
/** allow drv.h include excplictly for system*/
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <appl/diag/system.h> /** FORMAT_PBMP_MAX */
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

#define PRD_ROW_PRINT(port_string, header_type_string, prd_prio_string, sch_string, fifo_size_string, threshold_string, drop_string)    \
    do {                                                                    \
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);                                \
        PRT_CELL_SET("%s", port_string);                                    \
        PRT_CELL_SET("%s", header_type_string);                             \
        PRT_CELL_SET("%s", prd_prio_string);                                \
        PRT_CELL_SET("%s", sch_string);                                     \
        PRT_CELL_SET("%s", fifo_size_string);                               \
        PRT_CELL_SET("%s", threshold_string);                               \
        PRT_CELL_SET("%s", drop_string);                                    \
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
    uint64 port_64;
    bcm_port_prio_config_t priority_config;
    int priority_group, priority, prio_count = 0;
    uint32 src_prio, fifo_size, threshold, prio_bmp[1];
    uint64 fifo_size_64;
    uint64 drop_count = COMPILER_64_INIT(0, 0);
    bcm_port_nif_scheduler_t sch_prio;
    char *header_str = NULL;
    char *sch_prio_str[] = { "Low", "High", "TDM" };
    char prd_prio_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char threshold_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char fifo_size_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char port_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char drop_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    dnx_algo_port_info_s port_info;
    bcm_switch_control_key_t switch_control_key;
    bcm_switch_control_info_t switch_control_info;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;

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
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        /*
         * Skip the irrelevant port
         */
        
        if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE /* elk */ , FALSE /* stif */ , FALSE /* L1 */ , FALSE     /* FLEXE 
                                                                                                                         */ )
            || (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, FALSE, FALSE)))
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
             * Get Fifo size
             */
            fifo_size = priority_config.priority_groups[priority_group].num_of_entries;
            COMPILER_64_SET(fifo_size_64, 0, fifo_size);
            format_uint64_decimal(fifo_size_str, fifo_size_64, ',');
            /*
             * Convert port to string
             */
            COMPILER_64_SET(port_64, 0, port);
            format_uint64_decimal(port_str, port_64, ',');
            /** Get TDM mode. It will be used to check if TDM is in use.*/
            SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));
            /*
             * Get the PRD header type
             */
            switch_control_key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN;
            switch_control_key.type = bcmSwitchPortHeaderType;
            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(unit, port, switch_control_key, &switch_control_info));
            switch (switch_control_info.value)
            {
                case BCM_SWITCH_PORT_HEADER_TYPE_ETH:
                    switch (if_tdm_mode)
                    {
                        case DNX_ALGO_PORT_IF_TDM_ONLY:
                            header_str = "TDM";
                            break;
                        case DNX_ALGO_PORT_IF_TDM_HYBRID:
                            header_str = "TDM Hybrid";
                            break;
                        default:
                            header_str = "ETH";
                    }
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
             * Get the Threshold (except for TDM) and Print the table for each row
             */
            prio_count = 0;
            if ((src_prio & BCM_PORT_F_PRIORITY_TDM) == src_prio)
            {
                /** Print a row for TDM when only TDM bit is present in source priority.*/
                PRD_ROW_PRINT("", "", "TDM", sch_prio_str[sch_prio], fifo_size_str, "", drop_str);
            }
            else
            {
                SHR_BIT_ITER(prio_bmp, dnx_data_nif.prd.nof_priorities_get(unit), priority)
                {
                    SHR_IF_ERR_EXIT(bcm_cosq_ingress_port_drop_threshold_get(unit, port, 0, priority, &threshold));
                    sal_sprintf(threshold_str, "%d - %u", priority, threshold);
                    if ((priority_group == 0) && (prio_count == 0))
                    {
                        /** Print the first row for priority 0 on each port */
                        PRD_ROW_PRINT(port_str, header_str, prd_prio_str, sch_prio_str[sch_prio], fifo_size_str,
                                      threshold_str, drop_str);
                    }
                    else if ((priority_group != 0) && (prio_count == 0))
                    {
                        /** Print the first row for every subsequent priority group on the same port*/
                        PRD_ROW_PRINT("", "", prd_prio_str, sch_prio_str[sch_prio], fifo_size_str,
                                      threshold_str, drop_str);
                    }
                    else
                    {
                        /** Print other rows/priorities with same priority group */
                        PRD_ROW_PRINT("", "", "", "", "", threshold_str, "");
                    }
                    prio_count++;
                }
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
