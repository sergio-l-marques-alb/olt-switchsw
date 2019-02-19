/*
 * $Id: ukernel_debug.c,v 0.1 2014/10/11 samaresm Exp $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:    ukernel_debug.c
 * Purpose:  
 */

#include <stddef.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <sal/core/time.h>
#include <appl/diag/system.h>
#include <appl/diag/dport.h>
#include <bcm/error.h>
#include <shared/pack.h>

#if defined(VXWORKS)
#include <vxWorks.h>
#include <version.h>
#include "config.h"
#include <time.h>
#include <stdlib.h>
#endif

#if defined(UNIX) && !defined(__KERNEL__)
#include <time.h>
#include <stdlib.h>
#endif

#if defined(SOC_UKERNEL_DEBUG)
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
#include <soc/uc_dbg.h>
#include <appl/diag/ukernel_debug.h>

#define UCDBG_COMMAND_USAGE \
    "ucdbg <subcommand>\n" \
    "\t ucdbg start     starts showing ukernel debug log\n" \
    "\t ucdbg stop      stops showing ukernel debug log\n" \
    "\t ucdbg status    shows ukernel debug module status\n"

char cmd_cmic_ucdebug_usage[] = "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "   ucdbg <option> [args...]\n"
#else
    "   " UCDBG_COMMAND_USAGE "\n"
#endif /* COMPILER_STRING_CONST_LIMIT */
    ;


cmd_result_t 
cmd_cmic_ucdebug_dump_start(int unit, args_t *a)
{
    int rval = 0;
    
    rval = soc_cmic_ucdebug_dump_start(unit);
    switch (rval) {
        case BCM_E_UNAVAIL:
            return (CMD_NOTIMPL);
            break;
        case BCM_E_FAIL:
            return (CMD_FAIL);
    }
    return (CMD_OK);
}

cmd_result_t
cmd_cmic_ucdebug_dump_stop(int unit, args_t *a)
{
    int rval = 0;
    
    rval = soc_cmic_ucdebug_dump_stop(unit);
    switch (rval) {
        case BCM_E_UNAVAIL:
            return (CMD_NOTIMPL);
            break;
        case BCM_E_FAIL:
            return (CMD_FAIL);
    }
    return (CMD_OK);

}

cmd_result_t
cmd_cmic_ucdebug_status(int unit, args_t *a)
{
    int rval = 0;
    
    rval = soc_cmic_ucdebug_status(unit);
    switch (rval) {
        case BCM_E_UNAVAIL:
            return (CMD_NOTIMPL);
            break;
        case BCM_E_FAIL:
            return (CMD_FAIL);
    }
    return (CMD_OK);
}

/************* Main cmic_uc Debug command *******************/    
typedef struct {
    char *str;
    cmd_result_t (*func)(int unit, args_t *args);
} uc_debug_subcommand_t;

cmd_result_t
cmd_cmic_ucdebug(int unit, args_t *a)
{
    uc_debug_subcommand_t subcommands[] = {
#ifndef __KERNEL__
        {"start", cmd_cmic_ucdebug_dump_start},
        {"stop", cmd_cmic_ucdebug_dump_stop},
        {"status", cmd_cmic_ucdebug_status},
#endif
    };
    int i;
    const char *arg;

    if(!soc_feature(unit, soc_feature_ukernel_debug)) {
        return (CMD_NOTIMPL);
    }
    arg = ARG_GET(a);
    if (!arg) {
       return (CMD_USAGE);
    }

    for (i = 0; i < sizeof(subcommands) / sizeof(subcommands[0]); ++i) {
        if (parse_cmp(subcommands[i].str, arg, 0)) {
            return (*subcommands[i].func)(unit, a);
        }
    }

    return (CMD_USAGE);
}

#endif /* #if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT) */
#endif
