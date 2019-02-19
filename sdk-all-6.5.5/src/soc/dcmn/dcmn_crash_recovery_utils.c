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
 * INFO: this is a utility module that is shared between the other Crash Recovery modules.
 * 
 */
#include <sal/core/thread.h>
#include <soc/error.h>
#include <shared/bsl.h>

#include <soc/dcmn/dcmn_crash_recovery_utils.h>

#ifdef CRASH_RECOVERY_SUPPORT

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

extern soc_dcmn_cr_t *dcmn_cr_info[SOC_MAX_NUM_DEVICES];

uint8 soc_dcmn_cr_utils_is_logging(int unit)
{
    return (dcmn_cr_utils[unit].is_logging);
}

int soc_dcmn_cr_utils_logging_start(int unit)
{
    SOC_INIT_FUNC_DEFS;

    /* don't allow two transactions to run simultaneously */
    if(dcmn_cr_utils[unit].is_logging) {
        return SOC_E_EXISTS;
    }

    dcmn_cr_utils[unit].is_logging = TRUE;
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_utils_logging_stop(int unit)
{
    SOC_INIT_FUNC_DEFS;

    dcmn_cr_utils[unit].is_logging = FALSE;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

uint8 soc_dcmn_cr_utils_is_journaling_thread(int unit)
{
    return (sal_thread_self() == dcmn_cr_utils[unit].tid);
}

int soc_dcmn_cr_utils_journaling_thread_set(int unit)
{
    SOC_INIT_FUNC_DEFS;

    dcmn_cr_utils[unit].tid = sal_thread_self();
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_api_counter_increase(int unit)
{
    SOC_INIT_FUNC_DEFS;
    dcmn_cr_info[unit]->nested_api_cnt++;
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}

int soc_dcmn_cr_api_counter_decrease(int unit)
{
    SOC_INIT_FUNC_DEFS;

    /* return an error if outside a transaction */
    if(dcmn_cr_info[unit]->nested_api_cnt < 1) {
        return SOC_E_FAIL;
    }

    dcmn_cr_info[unit]->nested_api_cnt--;

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_api_counter_count_get(int unit)
{
    return dcmn_cr_info[unit]->nested_api_cnt;
}

int soc_dcmn_cr_api_counter_reset(int unit)
{
    SOC_INIT_FUNC_DEFS;

    dcmn_cr_info[unit]->nested_api_cnt = 0;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}

uint8 soc_dcmn_cr_api_is_top_level(int unit)
{
    return (soc_dcmn_cr_api_counter_count_get(unit) == 0);
}


#endif /* CRASH_RECOVERY_SUPPORT */
