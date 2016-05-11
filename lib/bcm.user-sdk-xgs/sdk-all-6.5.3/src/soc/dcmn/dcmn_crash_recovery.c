/*
 * $Id: $
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
 * INFO: this module is the entry poit for Crash Recovery feature
 * 
 * some design details:
 * - transaction starts at the beginning of an API and ends at the end of an API
 *
 */

#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <soc/dcmn/dcmn_crash_recovery.h>

#ifdef CRASH_RECOVERY_SUPPORT

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

int soc_dcmn_cr_init(int unit, dcmn_ha_mem_get mem_get, dcmn_ha_mem_release mem_release){
    SOC_INIT_FUNC_DEFS;

    /* enable autosync */

    /* register Application Alloc and Free Callbacks */

    /* call any NetLogic Init functions */

    /* init HW Log */

    /* init SW State Journal */

    /* Mark transaction as started */

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}
int soc_dcmn_cr_prepare_for_commit(int unit){
    SOC_INIT_FUNC_DEFS;

    /* sync HW Log to external storage */

    /* sync SW State Journal to external storage */

    /* raise ready for commit flag */

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}
int soc_dcmn_cr_commit(int unit){
    SOC_INIT_FUNC_DEFS;

    /* call kaps/kbp install if needed */

    /* apply hw log */

    /* lower ready for commit flag */

    /* dismiss SW State roll back journal */

    /* dismiss HW Log */

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_abort(int unit){
    SOC_INIT_FUNC_DEFS;

    /* apply roll back log */

    /* dismiss SW State roll back journal */

    /* dismiss KAPS/KBP ops */

    /* dismiss HW Log */

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_recover(int unit){
    SOC_INIT_FUNC_DEFS;

    /* check if ready for commit flag is raised */

        /* if not raised then call cr_abort and return*/
    
    /* check if KAPS rolled back */
    
        /* if yes then call cr_abort and return */

    /* apply HW Log */

    /* clear SW state journal */

    /* clear HW log */


    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

#endif
