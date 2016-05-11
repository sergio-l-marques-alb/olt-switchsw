/*
 * $Id:$
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * File:       pe_app.c
 */


#include <shared/bsl.h>
#include <appl/diag/system.h>
 
#ifdef PE_SDK
extern int
pe_app(int testnum, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7);
#endif /* PE_SDK */


/*
 * Function: cmd_peapp
 * Purpose: calls pe app cmds
 * Parameters:  u - SOC unit #
 *              a - pointer to args
 * Returns: CMD_OK/CMD_FAIL
 */
cmd_result_t
cmd_peapp(int u, args_t *a)
{
    int numargs;
    int tn=0;
    int a0=0, a1=0, a2=0, a3=0, a4=0, a5=0, a6=0, a7=0;
    char *cmd;
    
    numargs = ARG_CNT(a);
    /* check for test number */
    if (numargs>=1){
        cmd = ARG_GET(a);
        tn = sal_ctoi(cmd, NULL);
    }
    /* check for arg0 */
    if (numargs>=2){
        cmd = ARG_GET(a);
        a0 = sal_ctoi(cmd, NULL);
    }
    /* check for arg1 */
    if (numargs>=3){
        cmd = ARG_GET(a);
        a1 = sal_ctoi(cmd, NULL);
    }
    /* check for arg2 */
    if (numargs>=4){
        cmd = ARG_GET(a);
        a2 = sal_ctoi(cmd, NULL);
    }
    /* check for arg3 */
    if (numargs>=5){
        cmd = ARG_GET(a);
        a3 = sal_ctoi(cmd, NULL);
    }
    /* check for arg4 */
    if (numargs>=6){
        cmd = ARG_GET(a);
        a4 = sal_ctoi(cmd, NULL);
    }
    /* check for arg5 */
    if (numargs>=7){
        cmd = ARG_GET(a);
        a5 = sal_ctoi(cmd, NULL);
    }
    /* check for arg6 */
    if (numargs>=8){
        cmd = ARG_GET(a);
        a6 = sal_ctoi(cmd, NULL);
    }
    /* check for arg7 */
    if (numargs>=9){
        cmd = ARG_GET(a);
        a7 = sal_ctoi(cmd, NULL);
    }

#ifdef PE_SDK
    pe_app(tn, a0, a1, a2, a3, a4, a5, a6, a7);
#else
    cli_out("cmd_peapp() commented out call to pe_app(%d, %d, %d, %d, %d, %d, %d, %d, %d)\n", tn, a0, a1, a2, a3, a4, a5, a6, a7);
#endif /* PE_SDK */

    return CMD_OK;
}


