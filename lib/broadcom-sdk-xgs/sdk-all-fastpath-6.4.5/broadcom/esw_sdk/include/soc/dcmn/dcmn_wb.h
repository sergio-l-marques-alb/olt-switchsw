/*
 * $Id: dfe_port.c,v 1.13 Broadcom SDK $
 *
 * $Copyright: Copyright 2012 Broadcom Corporation.
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

#include <soc/dcmn/dcmn_defs.h>

/**********************************************************/
/*                  Defines                               */
/**********************************************************/
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_NONE                    (0) /*don't perform any test*/
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API         (1) /*perform wb test at end of every api call*/
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_END_OF_DVAPIS           (2) /*perform wb test at end of every dvapi test*/

#if defined(BCM_WARM_BOOT_API_TEST)

#include <appl/diag/shell.h>

#ifdef BCM_DPP_SUPPORT
#include <bcm_int/petra_dispatch.h>
#endif

#ifdef BCM_DFE_SUPPORT
#include <bcm_int/dfe_dispatch.h>
#endif


#define _DCMN_BCM_WARM_BOOT_API_TEST_RESET_TEST(unit, wb_rv)                                    \
    do {                                                                                        \
        int warmboot_test_mode_enable;                                                          \
        int no_wb_test;                                                                         \
        int disable_once, skip_count, test_counter;                                             \
                                                                                                \
        if (sal_thread_self() == sal_thread_main_get()) {                                       \
                                                                                                \
            soc_dcmn_wb_test_mode_get(unit, &warmboot_test_mode_enable);                        \
            soc_dcmn_wb_no_wb_test_get(unit, &no_wb_test);                                      \
            soc_dcmn_wb_disable_once_get(unit, &disable_once);                                  \
            soc_dcmn_wb_test_skip_multiple_wb_tests_get(unit, &skip_count);                     \
                                                                                                \
            if ((_DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API == warmboot_test_mode_enable)\
                    && (0 == no_wb_test) && (0 == disable_once)) {                              \
                                                                                                \
                /* count even if skipped */                                                     \
                soc_dcmn_wb_test_counter_plus_plus(unit);                                       \
                soc_dcmn_wb_test_counter_get(unit, &test_counter);                              \
                LOG_WARN(BSL_LS_BCM_API,                                                        \
                         (BSL_META_U(unit,                                                      \
                                     "**** WB BCM API %s **** (test counter: %d) ****\n"),      \
                                     FUNCTION_NAME(), test_counter));                           \
                if (skip_count>0)  {                                                            \
                    soc_dcmn_wb_test_skip_counter_minus_minus(unit);                            \
                    LOG_WARN(BSL_LS_BCM_API,                                                    \
                             (BSL_META_U(unit,                                                  \
                                         "  --> skipping warmboot test (%d more tests to skip)\n"),\
                                         skip_count-1));                                        \
                }                                                                               \
                  \
                /*        after 5 api calls, start performing reboot after 10 apis          */  \
                /*        instead of after every api call                                   */  \
                else if ((test_counter>5) && (test_counter%10 != 0))  {                         \
                    LOG_WARN(BSL_LS_BCM_API,                                                    \
                             (BSL_META_U(unit,                                                  \
                                         "  --> too many wb tests, skipping warmboot test for faster regression (performing wb test after 10 api calls)\n")));\
                }                                                                               \
                else {                                                                          \
                    LOG_DEBUG(BSL_LS_BCM_API,                               \
                              (BSL_META_U(unit,                             \
                                          "Unit:%d Starting warm boot test\n"), \
                               unit));                                      \
                                                                            \
                                                                            \
                    /*waiting for warmboot test to finish to avoide recursive calling to "tr 141 W=1" */ \
                    soc_dcmn_wb_no_wb_test_set(unit,1);                     \
                                                                            \
                    if (0 != sh_process_command(unit, "tr 141 W=1")) {      \
                        wb_rv = -1;                                         \
                    }                                                       \
                                                                            \
                    if (wb_rv != BCM_E_NONE) {                              \
                        LOG_DEBUG(BSL_LS_BCM_API,                           \
                                  (BSL_META_U(unit,                         \
                                              "Unit:%d Warm boot test failed\n"), \
                                   unit));                                  \
                    } else {                                                \
                        LOG_DEBUG(BSL_LS_BCM_API,                           \
                                  (BSL_META_U(unit,                         \
                                              "Unit:%d Warm boot test finish successfully\n"), \
                                   unit));                                  \
                    }                                                       \
                                                                            \
                    /*enable warmboot test*/                                \
                    soc_dcmn_wb_no_wb_test_set(unit,0);                     \
                }                                                        \
            }                                                           \
                                                                        \
            /* in case WB sequence was skipped once, */                 \
            /* settgin the flag to '0' in order that next time WB sequence will occure  */ \
            if (1 == disable_once) {                                    \
                soc_dcmn_wb_disable_once_set(unit, 0);                  \
            }                                                           \
        }                                                               \
    } while(0)

#ifdef BCM_DFE_SUPPORT
#define _DCMN_BCM_WARM_BOOT_API_TEST_SYNC_DFE(_u, _auto_sync, _rv)      \
     if (SOC_CONTROL(_u)->scache_dirty) {                               \
        _rv = bcm_dfe_switch_control_set(_u, bcmSwitchControlSync, 1);  \
     }

#else /*!BCM_DFE_SUPPORT*/
#define _DCMN_BCM_WARM_BOOT_API_TEST_SYNC_DFE(_u, _auto_sync, _rv)      do {} while(0)
#endif /*BCM_DFE_SUPPORT*/

#ifdef BCM_DPP_SUPPORT
#define _DCMN_BCM_WARM_BOOT_API_TEST_SYNC_DPP(_u, _auto_sync, _rv)                  \
    _rv = bcm_petra_switch_control_get(_u, bcmSwitchControlAutoSync, &_auto_sync);  \
    if (BCM_SUCCESS(_rv) && (_auto_sync != 0)) {                                    \
        if (SOC_CONTROL(_u)->scache_dirty) {                                        \
            _rv = bcm_petra_switch_control_set(_u, bcmSwitchControlSync, 1);        \
        }                                                                           \
    }
#else /*!BCM_DPP_SUPPORT*/
#define _DCMN_BCM_WARM_BOOT_API_TEST_SYNC_DPP(_u, _auto_sync, _rv)         do {} while(0)
#endif /*BCM_DPP_SUPPORT*/


#define _DCMN_BCM_WARM_BOOT_API_TEST_SYNC(_u)                           \
    do {                                                                \
        int _rv = BCM_E_NONE;                                                    \
        int _auto_sync;                                                 \
                                                                        \
        if (SOC_UNIT_VALID(_u)) {                                       \
            if (SOC_IS_DFE(_u))                                       \
            {                                                           \
                _DCMN_BCM_WARM_BOOT_API_TEST_SYNC_DFE(_u, _auto_sync, _rv)\
            }                                                           \
            if (SOC_IS_DPP(_u)) {                                     \
                _DCMN_BCM_WARM_BOOT_API_TEST_SYNC_DPP(_u, _auto_sync, _rv)\
            }                                                          \
            if (_rv == BCM_E_NONE) {\
                _DCMN_BCM_WARM_BOOT_API_TEST_RESET_TEST(_u, _rv);        \
            }\
            if (_rv != BCM_E_NONE) {                                     \
                r_rv = _rv;                                              \
            }\
        }                                                               \
    } while(0)
                                                       
/* WB_TEST_MODE_SKIP_WB_SEQUENCE flag disable the WB test mode for current BCM API call.
 * i.e - if test mode is on (i.e - perform warm rebooot at the end of APIs), when turning this
 * flag on the driver will NOT perform WB sequence when the BCM API finish to run. 
 * for example - to be used by APIs that create a mismatch between SW state and HW state
 * (like in the field instructions that are saved to SW but not yet commited to HW) */ 
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)        \
    do {                                                                \
        if (sal_thread_self() == sal_thread_main_get()) {               \
            /* perform only for main thread */                          \
            soc_dcmn_wb_disable_once_set(unit, 1);                      \
        }                                                               \
    } while(0)
/* OVERRIDE_WB_TEST_MODE          - temporary disable WB test mode */ 
/* RETRACT_OVERRIDEN_WB_TEST_MODE - re-enable WB test mode */ 
/* for example - can be used by BCM APIs that internally call other BCM APIs after which 
   WB sequence should not be called */ 
#define _DCMN_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit)        \
    soc_dcmn_wb_no_wb_test_set(unit, 1)
#define _DCMN_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit)\
    soc_dcmn_wb_no_wb_test_set(unit, 0)

#else /*!defined(BCM_WARM_BOOT_API_TEST)*/

#define _DCMN_BCM_WARM_BOOT_API_TEST_RESET_TEST(unit, wb_rv)                do {} while(0)
#define _DCMN_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit)            do {} while(0)
#define _DCMN_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit)   do {} while(0)
/* in case it is not DPP WB TEST defining the Macro to do nothing */
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)            do {} while(0)

#endif /*defined(BCM_WARM_BOOT_API_TEST)*/

/**********************************************************/
/*                  Functions                             */
/**********************************************************/

extern void soc_dcmn_wb_test_mode_set(int unit, int enable);
extern void soc_dcmn_wb_test_mode_get(int unit, int *enable);
extern void soc_dcmn_wb_no_wb_test_set(int unit, int wb_flag);
extern void soc_dcmn_wb_no_wb_test_get(int unit, int *wb_flag);
extern void soc_dcmn_wb_disable_once_set(int unit, int wb_flag);
extern void soc_dcmn_wb_disable_once_get(int unit, int *wb_flag);
extern void soc_dcmn_wb_test_counter_set(int unit, int counter);
extern void soc_dcmn_wb_test_counter_get(int unit, int *counter);
extern void soc_dcmn_wb_test_counter_plus_plus(int unit);
extern void soc_dcmn_wb_test_counter_reset(int unit);
extern void soc_dcmn_wb_test_skip_multiple_wb_tests_set(int unit, int nof_tests_to_skip);
extern void soc_dcmn_wb_test_skip_counter_minus_minus(int unit);
extern void soc_dcmn_wb_test_skip_multiple_wb_tests_get(int unit, int *nof_tests_to_skip);


/* general flag, indication whether we are in WB test mode or not */
extern int    _soc_dcmn_wb_warmboot_test_mode[SOC_MAX_NUM_DEVICES];
/* temporary disabling WB test mode */
extern int    _soc_dcmn_wb_override_wb_test[SOC_MAX_NUM_DEVICES];
/* disabling WB test mode for one BCM API call */
extern int    _soc_dcmn_wb_disable_once_wb_test[SOC_MAX_NUM_DEVICES];


#endif /*!_SOC_DCMN_WB_H_*/

