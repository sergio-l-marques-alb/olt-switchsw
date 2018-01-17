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


#ifdef CRASH_RECOVERY_SUPPORT

#include <soc/hwstate/hw_log.h>
#include <shared/swstate/sw_state_journal.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/dcmn/dcmn_wb.h>
#include <soc/dpp/ARAD/arad_sim_em.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

#define DCMN_CR_COULD_NOT_GUARANTEE 0
#define DCMN_CR_COMMITTED           1
#define DCMN_CR_ABORTED             2


#define DCMN_CR_UNIT_CHECK(unit) ((unit) >= 0 && (unit) < SOC_MAX_NUM_DEVICES)

#define DCMN_CR_DEFAULT_JOURNAL_SIZE (20000000) /*20MB*/

#if !defined(__KERNEL__) && defined (LINUX)
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/ha.h>
#define CR_VERSION_1_0 1
#define CR_STRUCT_SIG 0
typedef enum {
    HA_CR_SUB_ID_0 = 0
} HA_CR_sub_id_tl;
#endif
#endif

uint8 dcmn_cr_suspect_crash[SOC_MAX_NUM_DEVICES];

extern int diff_two_files(int unit, char *file_name1, char *file_name2);
extern int dcmn_cr_test_second_crash(int unit);

/* INTERNAL FUNCTION DECLARATION */
STATIC int soc_dcmn_cr_journaling_unfreeze(int unit);
STATIC int soc_dcmn_cr_prepare_for_commit(int unit);

extern sw_state_journal_t *sw_state_journal[SOC_MAX_NUM_DEVICES];
soc_dcmn_cr_t *dcmn_cr_info[SOC_MAX_NUM_DEVICES];

int soc_dcmn_cr_init(int unit){

    uint32 hw_journal_size;
    uint32 sw_journal_size;
    uint32 mode;
    uint32 size;

    SOC_INIT_FUNC_DEFS;

    /* supported only for jericho */
    if (!SOC_IS_JERICHO(unit)) SOC_EXIT;
    /* assign the mode and exit if the CR journaling mode is disabled */
    if(!(mode = soc_property_get(unit, spn_HA_HW_JOURNAL_MODE, DCMN_CR_JOURNALING_MODE_DISABLED))) {
        SOC_EXIT;
    }

#ifdef BCM_WARM_BOOT_API_TEST
    /* Verify that NV memory location is "shared memory" */
    /* Do it only in when testing CR with Crash simulations */
    _SOC_IF_ERR_EXIT(soc_stable_sanity(unit));
#endif

    /* allocate shared memory Crash Recovery struct */
    size = sizeof(soc_dcmn_cr_t);
    dcmn_cr_info[unit] = ha_mem_alloc(unit, HA_CR_Mem_Pool, HA_CR_SUB_ID_0, 
                           CR_VERSION_1_0, CR_STRUCT_SIG, &size);
    if (!dcmn_cr_info[unit])
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                       (BSL_META_U(unit, "CR shared mem Allocation failed for unit %d.\n"), unit));
        return SOC_E_MEMORY;
    }

    /* set the journaling mode */
    soc_dcmn_cr_journaling_mode_set(unit, mode);

    _SOC_IF_ERR_EXIT(soc_dcmn_cr_utils_journaling_thread_set(unit));

    /* mark crash recovery as enabled */
    SOC_CR_ENABLE(unit);

    /* init HW Log */
    hw_journal_size = soc_property_get(unit, spn_HA_HW_JOURNAL_SIZE, 0);
    _SOC_IF_ERR_EXIT(soc_hw_log_init(unit, hw_journal_size));

    /* init SW State Journal */
    sw_journal_size = soc_property_get(unit, spn_HA_SW_JOURNAL_SIZE, 0);
    _SOC_IF_ERR_EXIT(sw_state_journal_init(unit, sw_journal_size));

    /* reset the logging flag */
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_utils_logging_stop(unit));

    if (SOC_WARM_BOOT(unit)) {
        /* suspect a crash */
        dcmn_cr_suspect_crash[unit] = TRUE;
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_transaction_start(int unit) {
    
    uint8 is_top_level_api_call;

    SOC_INIT_FUNC_DEFS;

    /* supported only for jericho */
    if (!SOC_IS_JERICHO(unit)) SOC_EXIT;

    if (SOC_CR_ENABALED(unit)) {

#ifdef DRCM_CR_MULTIPLE_THREAD_JOURNALING
        /* set the transaction thread to the main thread */
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_utils_journaling_thread_set(unit));
#else
        /* exit if not the journaling thread */
        DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);
#endif /* DRCM_CR_MULTIPLE_THREAD_JOURNALING */

        /* enable API in API only for journaling_per_api mode */
        if(soc_dcmn_cr_is_journaling_per_api(unit)) {
            is_top_level_api_call = soc_dcmn_cr_api_is_top_level(unit);

            /* increase the number of APIs that are in the current transaction */
            _SOC_IF_ERR_EXIT(soc_dcmn_cr_api_counter_increase(unit));

            /* exit if this is a nested API call*/
            if(!is_top_level_api_call) {
                SOC_EXIT;
            }
        }

        /* ensure that hw log is not bypassed prior to a transaction start */
        _SOC_IF_ERR_EXIT(soc_hw_ensure_immediate_hw_access_disabled(unit));
        _SOC_IF_ERR_EXIT(soc_hw_log_ensure_not_suppressed(unit));

        if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable && SOC_DPP_IS_LEM_SIM_ENABLE(unit)) {
            /* clean LEM transaction shadow */
            _SOC_IF_ERR_EXIT(chip_sim_em_delete_all(unit, ARAD_CHIP_SIM_LEM_CR_BASE, ARAD_CHIP_SIM_LEM_KEY, SOC_DPP_DEFS_GET(unit, lem_width), 
                                     ARAD_CHIP_SIM_LEM_PAYLOAD, -1, -1));
        }

        /* Mark transaction as started */
        dcmn_cr_info[unit]->transaction_mode = DCMN_TRANSACTION_MODE_LOGGING;

        /* set the logging flag */
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_utils_logging_start(unit));
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int 
soc_dcmn_cr_prepare_for_commit(int unit){
    SOC_INIT_FUNC_DEFS;

    /* stop the logging of the hw log and the sw state journal */
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_utils_logging_stop(unit));

    /* raise ready for commit flag */
    dcmn_cr_info[unit]->transaction_mode = DCMN_TRANSACTION_MODE_COMMITTING;

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_commit(int unit){

#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
    uint8                   kbp_dirty,kaps_dirty;
    uint32                  kbp_tbl_id,kaps_tbl_id;
#endif
    DCMN_TRANSACTION_MODE  transaction_mode;
    
    SOC_INIT_FUNC_DEFS;
    
    /* supported only for jericho */
    if (!SOC_IS_JERICHO(unit)) SOC_EXIT;

    /* sanity */
    if (!SOC_CR_ENABALED(unit)) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                       (BSL_META_U(unit,
                          "unit:%d Crash Recovery ERROR: trying to commit while Crash Recovery feature is disabled\n"), unit));
    }

    /* exit if not the journaling thread */
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    /* immediate hw access is a short term event. It should not be enabled during a commit */ 
    _SOC_IF_ERR_EXIT(soc_hw_ensure_immediate_hw_access_disabled(unit));

    /* perform some steps to insure all data is synced to NV memory */
    /* skip it if you are "re-committing after recovering from a crash */
    transaction_mode = dcmn_cr_info[unit]->transaction_mode;
    if (transaction_mode != DCMN_TRANSACTION_MODE_COMMITTING) {
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_prepare_for_commit(unit));
    }

    if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable && SOC_DPP_IS_LEM_SIM_ENABLE(unit)) {
        /* clean LEM transaction shadow */
        _SOC_IF_ERR_EXIT(chip_sim_em_delete_all(unit, ARAD_CHIP_SIM_LEM_CR_BASE, ARAD_CHIP_SIM_LEM_KEY, SOC_DPP_DEFS_GET(unit, lem_width), 
                                     ARAD_CHIP_SIM_LEM_PAYLOAD, -1, -1));
    }

    /* call kbp install if needed */
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    if (ARAD_KBP_IS_CR_MODE(unit)) {
        kbp_dirty = dcmn_cr_info[unit]->kbp_dirty;
        if (kbp_dirty) {
            kbp_tbl_id = dcmn_cr_info[unit]->kbp_tbl_id;
            if (arad_kbp_cr_db_commit(unit,kbp_tbl_id)) {
                _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "unit:%d Crash Recovery ERROR: transaction commit failed to install KBP\n"), unit));
            }
            _SOC_IF_ERR_EXIT(arad_kbp_cr_transaction_cmd(unit,FALSE));
        }
        /* clean KBP dirty bit */
        dcmn_cr_info[unit]->kbp_dirty = 0;
    }
    if (JER_KAPS_IS_CR_MODE(unit)) {
        /* call kaps install if needed */
        kaps_dirty = dcmn_cr_info[unit]->kaps_dirty;
        if (kaps_dirty) {
            kaps_tbl_id = dcmn_cr_info[unit]->kaps_tbl_id;
            if (jer_kaps_cr_db_commit(unit,kaps_tbl_id)) {
                _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "unit:%d Crash Recovery ERROR: transaction commit failed to install KAPS\n"), unit));
            }
            _SOC_IF_ERR_EXIT(jer_kaps_cr_transaction_cmd(unit,FALSE));
        }
        /* clean KAPS dirty bit */
        dcmn_cr_info[unit]->kaps_dirty = 0;
    }
#endif

    /* The following is not part of the logic of the commit.
     * It compiles only for CRASH RECOVERY api test.
     */
#if defined(BCM_ARAD_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
    /* if the conditions hold perform the second crash (CR write ahead test) */
    _SOC_IF_ERR_EXIT(dcmn_cr_test_second_crash(unit));
#endif

    /* apply hw log */
    _SOC_IF_ERR_EXIT(soc_hw_log_commit(unit));

    /* lower ready for commit flag */
    dcmn_cr_info[unit]->transaction_mode = DCMN_TRANSACTION_MODE_IDLE;

    /* dismiss SW State roll back journal */
    _SOC_IF_ERR_EXIT(sw_state_journal_clear(unit));

    /* dismiss HW Log */
    soc_hw_log_purge(unit);

    /* unfreeze the journaling */
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_journaling_unfreeze(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int soc_dcmn_cr_journaling_unfreeze(int unit){
    SOC_INIT_FUNC_DEFS;

    /* no point in unsuppressing if not enabled */
    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    /* exit if not the journaling thread */
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    /* if logging was suppressed, unsupress it after commit */
    _SOC_IF_ERR_EXIT(soc_hw_log_unsuppress(unit));

    /* reset the immediate access counter */
    _SOC_IF_ERR_EXIT(soc_hw_reset_immediate_hw_access_counter(unit));

    /* Inform that state is consistent and protected by Crash Recovery again */
    dcmn_cr_info[unit]->is_recovarable = TRUE;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_dispatcher_commit(int unit){

    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    /* exit if not the journaling thread */
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    /* decrease the number of APIs that are in the current transaction */
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_api_counter_decrease(unit));

    /* don't commit in case this is a nested API call */
    if(!soc_dcmn_cr_api_is_top_level(unit)) {
        SOC_EXIT;
    }

    _SOC_IF_ERR_EXIT(soc_dcmn_cr_commit(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_abort(int unit){
    SOC_INIT_FUNC_DEFS;

    /* apply roll back log */
    _SOC_IF_ERR_EXIT(sw_state_journal_roll_back(unit));

    /* dismiss SW State roll back journal */
    _SOC_IF_ERR_EXIT(sw_state_journal_clear(unit));

    /* clear kaps/kbp last transaction status */
#if defined(INCLUDE_KBP) && !defined(BCM_88030) 
    if (ARAD_KBP_IS_CR_MODE(unit) && SOC_IS_DONE_INIT(unit)) {
        _SOC_IF_ERR_EXIT(arad_kbp_cr_clear_restore_status(unit));
    }
    if (JER_KAPS_IS_CR_MODE(unit) && SOC_IS_DONE_INIT(unit)) {
        _SOC_IF_ERR_EXIT(jer_kaps_cr_clear_restore_status(unit));
    }
#endif

    /* dismiss HW Log */
    _SOC_IF_ERR_EXIT(soc_hw_log_purge(unit));

    /* reset the nested API counter */
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_api_counter_reset(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_recover(int unit){
    DCMN_TRANSACTION_MODE commit_status;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
    uint8 kaps_commited = 0,kbp_commited = 0;
#endif
    int is_recovarable;
    SOC_INIT_FUNC_DEFS;

    /* if not suspecting a crash exit (e.g. we are in cold reboot) */
    if (!dcmn_cr_suspect_crash[unit]) SOC_EXIT;

    /* check if ready for commit flag is raised */
    commit_status = dcmn_cr_info[unit]->transaction_mode;
    if (commit_status == DCMN_TRANSACTION_MODE_COMMITTING) {
        /* check if KBP/KAPS committed */
#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
        if (ARAD_KBP_IS_CR_MODE(unit) && SOC_IS_DONE_INIT(unit)) {
            kbp_commited = arad_kbp_cr_query_restore_status(unit);
        }
        if (JER_KAPS_IS_CR_MODE(unit) && SOC_IS_DONE_INIT(unit)) {
            kaps_commited = jer_kaps_cr_query_restore_status(unit);
        }
		
        if (0 && (!kaps_commited || !kbp_commited)) {
		
            _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
            dcmn_cr_info[unit]->transaction_status = DCMN_CR_ABORTED;
        }
        else
#endif
        {
    
            dcmn_cr_info[unit]->perform_commit = TRUE;
    
        }
    } else if (commit_status == DCMN_TRANSACTION_MODE_LOGGING) {
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
        dcmn_cr_info[unit]->transaction_status = DCMN_CR_ABORTED;
    } else if (commit_status == DCMN_TRANSACTION_MODE_IDLE) {
        dcmn_cr_info[unit]->transaction_status = DCMN_CR_COULD_NOT_GUARANTEE;
        dcmn_cr_info[unit]->not_recoverable_reason = dcmn_cr_no_support_not_in_api;
    }

    is_recovarable = dcmn_cr_info[unit]->is_recovarable;
    if (!is_recovarable) {
        dcmn_cr_info[unit]->transaction_status = DCMN_CR_COULD_NOT_GUARANTEE;
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_suppress(int unit, dcmn_cr_no_support reason){

    SOC_INIT_FUNC_DEFS;

    /* no point in suppressing if not enabled */
    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    /* make sure that we are currently in a middle of a transaction */
    if (!soc_dcmn_cr_utils_is_logging(unit)) SOC_EXIT;

    /* exit if not the journaling thread */
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    /* immediate hw access should be disabled at the point of suppression */
    _SOC_IF_ERR_EXIT(soc_hw_ensure_immediate_hw_access_disabled(unit));

    /* Inform that state is not protected by Crash Recovery and may not be consistent after a crash */
    dcmn_cr_info[unit]->is_recovarable = FALSE;
    dcmn_cr_info[unit]->not_recoverable_reason = reason;


    _SOC_IF_ERR_EXIT(soc_dcmn_cr_commit(unit));
    _SOC_IF_ERR_EXIT(soc_hw_log_suppress(unit));
    
    dcmn_cr_info[unit]->is_recovarable = FALSE;
    
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_journaling_mode_get(int unit)
{
    return (SOC_CONTROL(unit)->journaling_mode);
}

int soc_dcmn_cr_is_journaling_per_api(int unit)
{
    return (DCMN_CR_JOURNALING_MODE_AFTER_EACH_API == SOC_CONTROL(unit)->journaling_mode);
}

int soc_dcmn_cr_journaling_mode_set(int unit, DCMN_CR_JOURNALING_MODE mode)
{
    SOC_INIT_FUNC_DEFS;

    /* is the journaling mode is disabled, then disable the CR mechanism */
    if(DCMN_CR_JOURNALING_MODE_DISABLED == mode){
        SOC_CR_DISABLE(unit);
    }

    SOC_CONTROL(unit)->journaling_mode = mode;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int dcmn_cr_perform_commit_after_recover(int unit) {
    SOC_INIT_FUNC_DEFS;
    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;
    if (dcmn_cr_info[unit]->perform_commit == TRUE) {
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_commit(unit));
        dcmn_cr_info[unit]->perform_commit = FALSE;
        dcmn_cr_info[unit]->transaction_status = DCMN_CR_COMMITTED;
    }
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

#endif
