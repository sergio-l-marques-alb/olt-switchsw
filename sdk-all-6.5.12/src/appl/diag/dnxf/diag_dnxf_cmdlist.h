/*
 * $Id: cmdlist.h,v 1.26 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    diag_dnx_cmdlist.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */

#ifndef DIAG_DNXF_CMDLIST_H_INCLUDED
#define DIAG_DNXF_CMDLIST_H_INCLUDED

#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_access.h>

#define DCL_CMD(_f,_u)  \
    extern cmd_result_t _f(int, args_t *); \
    extern char     _u[];

extern int bcm_dnxf_cmd_cnt;
extern cmd_t bcm_dnxf_cmd_list[];

DCL_CMD(cmd_dnxf_init_dnx, appl_dnxf_init_usage) DCL_CMD(cmd_dpp_clear, cmd_dpp_clear_usage)
#if defined(__DUNE_WRX_BCM_CPU__)
    DCL_CMD(cmd_dpp_cpu_i2c, cmd_dpp_cpu_i2c_usage)
#endif
    DCL_CMD(cmd_dpp_counter, cmd_dpp_counter_usage)
    DCL_CMD(cmd_dpp_gport, cmd_dpp_gport_usage) DCL_CMD(cmd_dpp_pbmp, cmd_dpp_pbmp_usage)
DCL_CMD(if_dnx_phy, if_dnx_phy_usage)
DCL_CMD(if_dnx_port_stat, if_dnx_port_stat_usage)
DCL_CMD(if_dnx_port, if_dnx_port_usage)
DCL_CMD(cmd_dpp_soc, cmd_dpp_soc_usage) DCL_CMD(cmd_sand_switch_control, cmd_sand_switch_control_usage)
#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
    DCL_CMD(cmd_xxreload, cmd_xxreload_usage)
#endif /* defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT) */
#undef  DCL_CMD
#endif /* DIAG_DNXF_CMDLIST_H_INCLUDED */
