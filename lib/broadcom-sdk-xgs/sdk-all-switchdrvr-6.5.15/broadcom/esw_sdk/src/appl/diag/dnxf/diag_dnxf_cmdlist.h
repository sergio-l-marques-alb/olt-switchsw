/** \file diag_dnxf_cmdlist.h
 *
 * Purpose: External declarations for command functions and
 *          their associated usage strings for fabric device
 */

#ifndef DIAG_DNXF_CMDLIST_H_INCLUDED
#define DIAG_DNXF_CMDLIST_H_INCLUDED

#include <appl/diag/shell.h>

#define DCL_CMD(_f,_u)  \
    extern cmd_result_t _f(int, args_t *); \
    extern char     _u[];

extern int bcm_dnxf_cmd_cnt;
extern cmd_t bcm_dnxf_cmd_list[];

DCL_CMD(cmd_dnxf_init_dnx, appl_dnxf_init_usage)
#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
    DCL_CMD(cmd_xxreload, cmd_xxreload_usage)
#endif /* defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT) */
#undef  DCL_CMD
#endif /* DIAG_DNXF_CMDLIST_H_INCLUDED */
