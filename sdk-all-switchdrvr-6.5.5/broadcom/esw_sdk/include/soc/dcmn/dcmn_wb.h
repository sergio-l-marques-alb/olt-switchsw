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
 * SOC DCMN CMICD IPROC
 */

#ifndef _SOC_DCMN_WB_H_
#define _SOC_DCMN_WB_H_

/**********************************************************/
/*                  Includes                              */
/**********************************************************/
#include <soc/defs.h>
#include <soc/drv.h>
#include <shared/bsl.h>

#include <soc/dcmn/dcmn_defs.h>

#ifdef BCM_WARM_BOOT_API_TEST
#include <appl/diag/shell.h>
#include<setjmp.h>
#endif

/**********************************************************/
/*                  Defines                               */
/**********************************************************/

#define SOC_WB_API_RESET_ERR_CHECK(disp_rv)\
    if (disp_rv != SOC_E_NONE) {return SOC_E_INTERNAL;}

/* used in field tests */
#ifdef BCM_WARM_BOOT_API_TEST
#define WB_TEST(op) op
#else
#define WB_TEST(op)
#endif

#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_NONE                          (0) /*don't perform any test*/
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API               (1) /*perform wb test at end of every api call*/
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_END_OF_DVAPIS                 (2) /*perform wb test at end of every dvapi test*/
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY                (3) /*perform cr test at end of every api call*/
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY_ROLL_BACK      (4) /*perform cr roll back test at every api call*/

#ifdef CRASH_RECOVERY_SUPPORT
/* crash status
 * for more information read the comment above the function dcmn_cr_signal_before_prepare_for_commit_set
   in the file dcmn_crash_recovery.c
 */
typedef enum _dcmn_cr_stat_s {
    _dcmnCrResetBeforeCommittingMode,
    _dcmnCrResetAfterCommittingMode
} _dcmn_cr_stat_t;

#ifdef BCM_WARM_BOOT_API_TEST
/* the buffer used by the cr test - for more info search BCM_CR_TRANSACTION_START */
extern jmp_buf dcmn_cr_buf[SOC_MAX_NUM_DEVICES];
extern _dcmn_cr_stat_t dcmn_cr_crash_status[SOC_MAX_NUM_DEVICES];
#endif /* BCM_WARM_BOOT_API_TEST */
#endif /* CRASH_RECOVERY_SUPPORT */

/* in the first DCMN_WARM_BOOT_MIN_DONT_SKIP calls, don't skip the reset sequence */
#define DCMN_WARM_EXP_PARAM 10

/* 
 * field override wb test mode 
 */ 
typedef enum _dcmn_wb_field_falgs_s {
    _dcmnWbFieldFlagDirectExtraction=0,
    _dcmnWbFieldFlagDataQual=1, 
    _dcmnWbFieldFlagNOF=2
} _dcmn_wb_field_falgs_e;

typedef enum _dcmn_wb_skip_decision_s {
    _dcmnWbSkipCounter = 0,
    _dcmnWbSkipFasterRegression = 1,
    _dcmnDontSkip = 2
} _dcmn_wb_skip_decision_t;

/* general flag, indication whether we are in WB test mode or not */
extern int    _soc_dcmn_wb_warmboot_test_mode[SOC_MAX_NUM_DEVICES];
/* temporary disabling WB test mode */
extern int    _soc_dcmn_wb_override_wb_test[SOC_MAX_NUM_DEVICES];
/* disabling WB test mode for one BCM API call */
extern int    _soc_dcmn_wb_disable_once_wb_test[SOC_MAX_NUM_DEVICES];
/* enforcing WB test mode for one BCM API call */
extern int    _soc_dcmn_wb_enforce_once_wb_test[SOC_MAX_NUM_DEVICES];

/**********************************************************/
/*                  Functions                             */
/**********************************************************/


extern void soc_dcmn_wb_test_mode_set(int unit, int mode);
extern void soc_dcmn_wb_test_mode_get(int unit, int *mode);
extern void soc_dcmn_wb_no_wb_test_set(int unit, int wb_flag);
extern void soc_dcmn_wb_no_wb_test_get(int unit, int *wb_flag);
extern void soc_dcmn_wb_field_test_mode_set(int unit, _dcmn_wb_field_falgs_e type, int wb_flag);
extern void soc_dcmn_wb_field_test_mode_get(int unit, _dcmn_wb_field_falgs_e type, int *wb_flag);
extern void soc_dcmn_wb_disable_once_set(int unit, int wb_flag);
extern void soc_dcmn_wb_disable_once_get(int unit, int *wb_flag);
extern void soc_dcmn_wb_enforce_once_set(int unit, int wb_flag);
extern void soc_dcmn_wb_enforce_once_get(int unit, int *wb_flag);
extern void soc_dcmn_wb_test_counter_set(int unit, int counter);
extern void soc_dcmn_wb_test_counter_get(int unit, int *counter);
extern void soc_dcmn_wb_test_counter_plus_plus(int unit);
extern int  dcmn_bcm_warm_boot_api_test_reset_conds (int unit);
extern _dcmn_wb_skip_decision_t dcmn_bcm_warm_boot_api_test_reset_skip_decider(int unit, int *nof_skip);
extern void dcmn_bcm_warm_boot_api_test_enable_if_disabled_once(int unit);
extern void dcmn_bcm_warm_boot_api_test_disable_if_enforced_once(int unit);
extern void dcmn_wb_api_nesting_inc(int unit);
extern void dcmn_wb_api_nesting_dec(int unit);
extern int  dcmn_wb_api_nesting_get(int unit);
extern void soc_dcmn_wb_test_counter_reset(int unit);
extern void soc_dcmn_wb_test_skip_multiple_wb_tests_set(int unit, int nof_tests_to_skip);
extern void soc_dcmn_wb_test_skip_counter_minus_minus(int unit);
extern void soc_dcmn_wb_test_skip_multiple_wb_tests_get(int unit, int *nof_tests_to_skip);
extern int  dcmn_bcm_warm_boot_api_test_mode_skip_wb_sequence(int unit);
extern int  dcmn_wb_exp_backoff(int test_counter, int *nof_skip);
/*************************************************************************************************/



#ifdef BCM_DPP_SUPPORT
#include <bcm_int/petra_dispatch.h>
#else /* !BCM_DPP_SUPPORT */
#ifdef BCM_DFE_SUPPORT
#include <bcm_int/dfe_dispatch.h>
#endif /* BCM_DFE_SUPPORT */
#endif /* BCM_DPP_SUPPORT */

/* remove this define if you want standard wb test mode (no autosync) */
/*#define DCMN_AUTOSYNC_TEST_MODE 1*/

#ifdef DCMN_AUTOSYNC_TEST_MODE

#define TR_141_COMMAND(is_cr) ("tr 141 w=1 NoSync=1")
#else /* !DCMN_AUTOSYNC_TEST_MODE */
#define TR_141_COMMAND(is_cr) ((is_cr) ? "tr 141 w=1 NoSync=1 NoDump=1" : "tr 141 W=1 NoDump=2")
#endif /* DCMN_AUTOSYNC_TEST_MODE */


int dcmn_bcm_warm_boot_api_test_reset(int unit, int8* is_reset);


#ifdef BCM_WARM_BOOT_API_TEST

/* WB_TEST_MODE_SKIP_WB_SEQUENCE flag disable the WB test mode for current BCM API call.
 * i.e - if test mode is on (i.e - perform warm rebooot at the end of APIs), when turning this
 * flag on the driver will NOT perform WB sequence when the BCM API finish to run. 
 * for example - to be used by APIs that create a mismatch between SW state and HW state
 * (like in the field instructions that are saved to SW but not yet commited to HW) */ 
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)        \
    dcmn_bcm_warm_boot_api_test_mode_skip_wb_sequence(unit)


/* OVERRIDE_WB_TEST_MODE          - temporary disable WB test mode */ 
/* RETRACT_OVERRIDEN_WB_TEST_MODE - re-enable WB test mode */ 
/* for example - can be used by BCM APIs that internally call other BCM APIs after which 
   WB sequence should not be called 
   NOTE: !!! I'ts the caller responsability to call RETRACT for every OVERIDE call!!!
         !!! there is an internal counter here, nof retracts should equal nof ovrides !!! */ 
#define _DCMN_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit)        \
    soc_dcmn_wb_no_wb_test_set(unit, 1)
#define _DCMN_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit)\
    soc_dcmn_wb_no_wb_test_set(unit, 0)


#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_STOP(unit);\
    soc_dcmn_wb_field_test_mode_set(unit, _dcmnWbFieldFlagDirectExtraction, 1)
#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_CONTINUE(unit);\
    soc_dcmn_wb_field_test_mode_set(unit, _dcmnWbFieldFlagDirectExtraction, 0)
#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_STOP(unit);\
    soc_dcmn_wb_field_test_mode_set(unit, _dcmnWbFieldFlagDataQual, 1)
#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_CONTINUE(unit);\
    soc_dcmn_wb_field_test_mode_set(unit, _dcmnWbFieldFlagDataQual, 0)

#else /* !BCM_WARM_BOOT_API_TEST */


#define _DCMN_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit)            do {} while(0)
#define _DCMN_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit)   do {} while(0)
/* in case it is not DPP WB TEST defining the Macro to do nothing */
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)            do {} while(0)

#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_STOP(unit)       do {} while(0)
#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_CONTINUE(unit)   do {} while(0)
#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_STOP(unit)          do {} while(0)
#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_CONTINUE(unit)      do {} while(0)
	
#endif /* BCM_WARM_BOOT_API_TEST */


#endif /*!_SOC_DCMN_WB_H_*/

