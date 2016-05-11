/*
 * $Id: dfe_port.c,v 1.13 Broadcom SDK $
 *
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
 * SOC DCMN WB
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>


#if (defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT)

#include <soc/dcmn/dcmn_wb.h>

/* general flag, indication whether we are in WB test mode or not */
int    _soc_dcmn_wb_warmboot_test_mode[SOC_MAX_NUM_DEVICES];
/* temporary disabling WB test mode */
int    _soc_dcmn_wb_override_wb_test[SOC_MAX_NUM_DEVICES];
/* temporary disabling WB test mode due to temporarily field unstability */
int    _soc_dcmn_wb_field_test_mode_stop[SOC_MAX_NUM_DEVICES][_dcmnWbFieldFlagNOF];
/* disabling WB test mode for one BCM API call */
int    _soc_dcmn_wb_disable_once_wb_test[SOC_MAX_NUM_DEVICES];
/* counter, counting the number of warmboot test performed */
int    _soc_dcmn_wb_warmboot_test_counter[SOC_MAX_NUM_DEVICES];
/* number of warmboot tests to be skipped (not including the "disable once" tests */
int    _soc_dcmn_wb_warmboot_test_nof_tests_to_skip[SOC_MAX_NUM_DEVICES];


/* 
 * allowed values for warmboot_test_mode_set/get: 
 * ---------------------------------------------- 
 * 0:WB_TEST_MODE_NONE
 * 1:WB_TEST_MODE_AFTER_EVERY_API
 * 2:WB_TEST_MODE_END_OF_DVAPIS 
 *  
 */

void
soc_dcmn_wb_test_mode_set(int unit, int enable)
{
    _soc_dcmn_wb_warmboot_test_mode[unit] = enable;
}

void
soc_dcmn_wb_test_mode_get(int unit, int *enable)
{
    *enable =  _soc_dcmn_wb_warmboot_test_mode[unit];
}

void
soc_dcmn_wb_test_counter_set(int unit, int counter)
{
    _soc_dcmn_wb_warmboot_test_counter[unit] = counter;
}

void
soc_dcmn_wb_test_counter_get(int unit, int *counter)
{
    *counter =  _soc_dcmn_wb_warmboot_test_counter[unit];
}

void
soc_dcmn_wb_test_counter_plus_plus(int unit)
{
    _soc_dcmn_wb_warmboot_test_counter[unit]++;
}

void
soc_dcmn_wb_test_counter_reset(int unit)
{
    _soc_dcmn_wb_warmboot_test_counter[unit] = 0;
}

void
soc_dcmn_wb_test_skip_multiple_wb_tests_set(int unit, int nof_tests_to_skip)
{
    _soc_dcmn_wb_warmboot_test_nof_tests_to_skip[unit] = nof_tests_to_skip;
}

void
soc_dcmn_wb_test_skip_counter_minus_minus(int unit)
{
    if (_soc_dcmn_wb_warmboot_test_nof_tests_to_skip[unit] == 0) {
        return;
    }
    else {
        _soc_dcmn_wb_warmboot_test_nof_tests_to_skip[unit]--;
    }
}

void
soc_dcmn_wb_test_skip_multiple_wb_tests_get(int unit, int *nof_tests_to_skip)
{
    *nof_tests_to_skip = _soc_dcmn_wb_warmboot_test_nof_tests_to_skip[unit];
}



/* the two functions below set\get a flag that override the wb test mode
 * i.e if test mode is on (perform warm rebooot at the end of APIs) turning this
 * flag on will instruct the driver to not perform the reboots  */ 
void
soc_dcmn_wb_no_wb_test_set(int unit, int wb_flag)
{
    if (wb_flag == 0) 
    {
        _soc_dcmn_wb_override_wb_test[unit]--;
    }
    else
    {
        _soc_dcmn_wb_override_wb_test[unit]++;
    }
}

void
soc_dcmn_wb_no_wb_test_get(int unit, int *wb_flag)
{
    *wb_flag =  _soc_dcmn_wb_override_wb_test[unit];
}


void soc_dcmn_wb_field_test_mode_set(int unit, _dcmn_wb_field_falgs_e type, int wb_flag)
{
    _soc_dcmn_wb_field_test_mode_stop[unit][type] = wb_flag;
}  
void soc_dcmn_wb_field_test_mode_get(int unit, _dcmn_wb_field_falgs_e type, int *wb_flag)
{
    *wb_flag =  _soc_dcmn_wb_field_test_mode_stop[unit][type];
} 

/* the two functions below set\get a flag that disable the wb test mode
 * i.e if test mode is on (perform warm rebooot at the end of APIs) turning this
 * flag on will instruct the driver to not perform the when the BCM API finish to run, 
 * should be used by APIs that
 * create a mismatch btween SW state and HW state, for example: field instructions saved to SW
 * but not yet commited to HW should turn on this flag. */ 
void
soc_dcmn_wb_disable_once_set(int unit, int wb_flag)
{
    _soc_dcmn_wb_disable_once_wb_test[unit] = wb_flag;
}

void
soc_dcmn_wb_disable_once_get(int unit, int *wb_flag)
{
    *wb_flag = _soc_dcmn_wb_disable_once_wb_test[unit];
}
 

#endif /*(defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT)*/
