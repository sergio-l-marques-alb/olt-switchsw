/*
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
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: pr.c,v 1.9.16.14 Broadcom SDK $
 *
 * File:    icc_user.c
 * Purpose:  Routines that setup ICC entries and customer application helpers
 * Requires:
 */



#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/drv.h>

#ifdef BCM_CALADAN3_SUPPORT
#include <soc/sbx/caladan3/port.h>
#include <soc/sbx/caladan3/sws.h>
#include <soc/sbx/caladan3/util.h>
#include <soc/sbx/sbx_drv.h>
#include <soc/mem.h>
#include <soc/mcm/allenum.h>



/*
 * Function:
 *     soc_sbx_calandan3_sws_icc_state_key_unpack
 * Purpose:
 *     unpack state value to state info 
 *      input: state;   output: state_ino
 */
int soc_sbx_calandan3_sws_icc_state_key_unpack(soc_sbx_calandan3_icc_state_info_t* state_info, uint8* state)
{
    int     rc = SOC_E_PARAM;

    if ((state_info != NULL) && (state != NULL))
    {
        rc = SOC_E_NONE;
        state_info->port      = state[0];
        state_info->profile   = 0;        /* now is not used */
        state_info->stage     = state[2] & 0xF;
        state_info->flag      = (state[2] & 0xF0) >> 4;
    }

    return rc;
}

/*
 * Function:
 *     soc_sbx_calandan3_sws_icc_state_key_pack
 * Purpose:
 *     pack state info to state value 
 *      input: state_ino;   output: state
 */
int soc_sbx_calandan3_sws_icc_state_key_pack(soc_sbx_calandan3_icc_state_info_t* state_info, uint8* state)
{
    int     rc = SOC_E_PARAM;

    if ((state_info != NULL) && (state != NULL))
    {
        rc = SOC_E_NONE;
        state[0]    = state_info->port;
        state[1]    = 0;
        state[2]    = ((state_info->flag & 0xF) << 4) | (state_info->stage & 0xF);
    }

    return rc;
}


/*
 * Function:
 *     soc_sbx_calandan3_sws_icc_state_result_get
 * Purpose:
 *     generage swc icc tcam result field state value
 */
uint32 soc_sbx_calandan3_sws_icc_state_result_get(uint32 flag, uint32 stage, uint32 port)
{
    uint32  rc = 0;

    rc = ((flag & 0xF) << 20) | ((stage & 0xF) << 16) | port;
    return rc;
}

int
sws_pr_icc_port_match_rule_set(int unit, int pr, int base_port, int squeue, int *idx)
{
    int i;
    int max;
    uint32 regval = 0;
    uint8 icckey[26] = {0}, iccmsk[26] = {0};
    uint8 state[3] = {0}, smask[3] = {0};
    soc_sbx_caladan3_pr_icc_lookup_data_t data;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, PR_ICC_CONFIG0r, 
                            pr, 0, &regval));
    max = soc_reg_field_get(unit, PR_ICC_CONFIG0r, regval, LOOKUPS_REQUIREDf);

    for (i=0; i < max; i++) {
        sal_memset(&data, 0, sizeof(data));
        sal_memset(&icckey, 0, sizeof(icckey));
        sal_memset(&iccmsk, 0, sizeof(iccmsk));
        state[0] = base_port;
        state[2] = i;
        smask[0] = 0x3F;
        smask[2] = 0xF;

        if (i == 2) {
            data.last = 1;
            data.queue_action = SOC_SBX_CALADAN3_PR_QUEUE_ACTION_LOOKUP;
            data.queue = squeue;
        } else {
            data.last = 0;
            data.state = ((i + 1) << 16) | (base_port);
        }
        soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, pr, *idx, 
                      SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                      icckey, iccmsk, 
                      state, smask, 
                      &data);
        ++(*idx);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_sbx_caladan3_sws_pr_tcam_match_all
 * Purpose:
 *     Initialize PR TCAM match all default entry
 */
void
soc_sbx_caladan3_sws_pr_tcam_match_all(int unit, int pr, int idx, int valid,
                                  soc_sbx_caladan3_pr_icc_lookup_data_t *data)
{
    uint8 iccKey[26], iccMsk[26];
    uint8 state[3], smask[3];
    sal_memset(iccKey, 0, sizeof(iccKey));
    sal_memset(iccMsk, 0, sizeof(iccMsk));
    sal_memset(state, 0, sizeof(state));
    sal_memset(smask, 0, sizeof(smask));
    soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, pr, idx, valid,
                                             iccKey, iccMsk,
                                             state, smask, data);
}


void
soc_sbx_caladan3_sws_pr_icc_program_arad_header(int unit, int force)
{
    int need_entries = 0, need_matchall = 0, idx = 0, port = 0, cos = 0;
    soc_sbx_caladan3_pr_icc_lookup_data_t data;
    uint8 icckey[26] = {0}, iccmsk[26] = {0};
    soc_sbx_caladan3_queues_t qtemp, *queues = NULL;
    uint8 state[3] = {0}, smask[3] = {0};
    int qid, rv = SOC_E_NONE;


    PBMP_ITER(PBMP_HG_ALL(unit), port) {
        if (!soc_sbx_caladan3_is_line_port(unit, port)) {
            /* Found a HG port on Fabric side, TCAM entries already present,
             * do not waste entries 
             */
            if ((force) && (sws_pr_icc_reserved_rules_get(unit, PR1_INSTANCE) > 0)) {
                /* Debug scenario */
                return;
            }
            need_entries = 1;
            break;
        }
    }

    idx=0;
    if (need_entries || force) {
        SOC_PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            cos = 0;
            if (!soc_sbx_caladan3_is_line_port(unit, port)) {
                /* Skip fabric ports */
                continue;
            }
            rv = soc_sbx_caladan3_port_queues_get(unit, port, 1, &queues);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Unit %d *** Error: Failed getting queue info for port %d\n"),
                           unit, port));
                return;
            }

            idx = sws_pr_icc_reserved_rules_get(unit, PR1_INSTANCE);
            SOC_SBX_C3_BMP_CLEAR(qtemp.squeue_bmp, SOC_SBX_CALADAN3_NUM_WORDS_FOR_SQUEUES);
            SOC_SBX_C3_BMP_ITER_RANGE(queues->squeue_bmp, 
                                      SOC_SBX_CALADAN3_SWS_FABRIC_SQUEUE_BASE,
                                      qid, SOC_SBX_CALADAN3_NUM_WORDS_FOR_SQUEUES) {
                if (SOC_SBX_C3_BMP_MEMBER_IN_RANGE(qtemp.squeue_bmp, 
                                                   SOC_SBX_CALADAN3_SWS_FABRIC_SQUEUE_BASE, 
                                                   qid)) {
                   continue;
                }
                SOC_SBX_C3_BMP_ADD(qtemp.squeue_bmp, qid % SOC_SBX_CALADAN3_SWS_QUEUE_PER_REGION);
                sal_memset(&icckey, 0, sizeof(icckey));
                sal_memset(&iccmsk, 0, sizeof(iccmsk));
                sal_memset(&data, 0, sizeof(data));

                
                icckey[2] = port;
                iccmsk[2] = 0xff;
                icckey[14] = cos << 3; 
                iccmsk[14] = cos > 0 ? 0x38 : 0;

                data.queue_action = SOC_SBX_CALADAN3_PR_QUEUE_ACTION_LOOKUP;
                data.last = 1;
                data.queue = qid;
                need_matchall = 1;
                soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, PR1_INSTANCE,
                                                         idx,
                                                         SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                                                         icckey, iccmsk, 
                                                         state, smask, 
                                                         &data);
                idx++;
                cos++;
            }
            sws_pr_icc_reserved_rules_set(unit, PR1_INSTANCE, idx);
        }
    }
    if (need_matchall) {
        if (idx < 255) {
            /* MATCH ALL entry:
             * Redirect packets to CMIC in future, allow override using soc parameter
             * Currently set to Drop.
             */
            port = CMIC_PORT(unit);
            rv = soc_sbx_caladan3_port_queues_get(unit, port, 1, &queues);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Unit %d *** Error: Failed getting queue info for port %d\n"),
                           unit, port));
                return;
            }
            data.queue_action = SOC_SBX_CALADAN3_PR_QUEUE_ACTION_LOOKUP;
            data.drop  = 1;
            data.last = 1;
            data.queue = queues->squeue_base;
            soc_sbx_caladan3_sws_pr_tcam_match_all(unit, PR1_INSTANCE,
                                                   255, 0x3, &data);
        } else {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "WARNING: Unit %d: Cannot add MATCH ALL entry"
                                 " in PR1 TCAM, potentially fatal\n"), unit));
        }

        /* Set ICC state */
        soc_sbx_caladan3_pr_set_icc_state(unit, 
            PR1_INSTANCE, SOC_SBX_CALADAN3_SWS_PR_ICC_ENABLE);
    }
}

void
soc_sbx_caladan3_sws_pr_icc_program_loopback_header(int unit, int force)
{
    int need_entries = 0, need_matchall = 0, idx = 0, port = 0, cos = 0;
    soc_sbx_caladan3_pr_icc_lookup_data_t data;
    uint8 icckey[26] = {0}, iccmsk[26] = {0};
    soc_sbx_caladan3_queues_t qtemp, *queues = NULL;
    uint8 state[3] = {0}, smask[3] = {0};
    int qid, rv = 0;


    PBMP_ITER(PBMP_HG_ALL(unit), port) {
        if (!soc_sbx_caladan3_is_line_port(unit, port)) {
            /* Found a HG port on Fabric side, TCAM entries already present,
             * do not waste entries 
             */
            if ((force) && (sws_pr_icc_reserved_rules_get(unit, PR1_INSTANCE) > 0)) {
                /* Debug scenario */
                return;
            }
            need_entries = 1;
            break;
        }
    }

    idx=0;
    if (need_entries || force) {
        SOC_PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            cos = 0;
            if (!soc_sbx_caladan3_is_line_port(unit, port)) {
                /* Skip fabric ports */
                continue;
            }
            rv = soc_sbx_caladan3_port_queues_get(unit, port, 1, &queues);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Unit %d *** Error: Failed getting queue info for port %d\n"),
                           unit, port));
                return;
            }
            idx = sws_pr_icc_reserved_rules_get(unit, PR1_INSTANCE);
            SOC_SBX_C3_BMP_CLEAR(qtemp.squeue_bmp, SOC_SBX_CALADAN3_NUM_WORDS_FOR_SQUEUES);
            SOC_SBX_C3_BMP_ITER_RANGE(queues->squeue_bmp, 
                                      SOC_SBX_CALADAN3_SWS_FABRIC_SQUEUE_BASE,
                                      qid, SOC_SBX_CALADAN3_NUM_WORDS_FOR_SQUEUES) {
                if (SOC_SBX_C3_BMP_MEMBER_IN_RANGE(qtemp.squeue_bmp, 
                                                   SOC_SBX_CALADAN3_SWS_FABRIC_SQUEUE_BASE, 
                                                   qid)) {
                   continue;
                }
                SOC_SBX_C3_BMP_ADD(qtemp.squeue_bmp, qid % SOC_SBX_CALADAN3_SWS_QUEUE_PER_REGION);
                sal_memset(&data, 0, sizeof(data));
                sal_memset(&icckey, 0, sizeof(icckey));
                sal_memset(&iccmsk, 0, sizeof(iccmsk));

                icckey[2] = (port << 3) & 0xff;
                iccmsk[2] = 0xf8;
                icckey[3] = (port >> 5);
                iccmsk[3] = 0x1;
                icckey[10] = cos << 3; 
                iccmsk[10] = cos > 0 ? 0x38 : 0;
                data.queue_action = SOC_SBX_CALADAN3_PR_QUEUE_ACTION_LOOKUP;
                data.last = 1;
                data.queue = qid;
                need_matchall = 1;
                soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, PR1_INSTANCE,
                                                         idx,
                                                         SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                                                         icckey, iccmsk, 
                                                         state, smask, 
                                                         &data);
                idx++;
                cos++;
            }
            sws_pr_icc_reserved_rules_set(unit, PR1_INSTANCE, idx);
        }
    }
    if (need_matchall) {
        if (idx < 255) {
            /* MATCH ALL entry:
             * Redirect packets to CMIC in future, allow override using soc parameter
             * Currently set to Drop.
             */
            port = CMIC_PORT(unit);
            rv = soc_sbx_caladan3_port_queues_get(unit, port, 1, &queues);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Unit %d *** Error: Failed getting queue info for port %d\n"),
                           unit, port));
                return;
            }
            data.queue_action = SOC_SBX_CALADAN3_PR_QUEUE_ACTION_LOOKUP;
            data.drop  = 1;
            data.last = 1;
            data.queue = queues->squeue_base;
            soc_sbx_caladan3_sws_pr_tcam_match_all(unit, PR1_INSTANCE, 
                                                   255, 0x3, &data);
        } else {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "WARNING: Unit %d: Cannot add"
                                 " MATCH ALL entry in PR 1 TCAM, potentially fatal\n"), unit));
        }
        /* Set ICC state */
        soc_sbx_caladan3_pr_set_icc_state(unit, 
            PR1_INSTANCE, SOC_SBX_CALADAN3_SWS_PR_ICC_ENABLE);
    }
}

void
soc_sbx_caladan3_sws_pr_icc_program_sirius_header(int unit, int force)
{
    int need_entries = 0, need_matchall = 0, idx = 0, port = 0, cos = 0;
    soc_sbx_caladan3_pr_icc_lookup_data_t data;
    uint8 icckey[26] = {0}, iccmsk[26] = {0};
    soc_sbx_caladan3_queues_t qtemp, *queues = NULL;
    uint8 state[3] = {0}, smask[3] = {0};
    int qid, rv = 0;


    PBMP_ITER(PBMP_HG_ALL(unit), port) {
        if (!soc_sbx_caladan3_is_line_port(unit, port)) {
            /* Found a HG port on Fabric side, TCAM entries already present,
             * do not waste entries 
             */
            if ((force) && (sws_pr_icc_reserved_rules_get(unit, PR1_INSTANCE) > 0)) {
                /* Debug scenario */
                return;
            }
            need_entries = 1;
            break;
        }
    }

    idx=0;
    if (need_entries || force) {
        SOC_PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            cos = 0;
            if (!soc_sbx_caladan3_is_line_port(unit, port)) {
                /* Skip fabric ports */
                continue;
            }
            rv = soc_sbx_caladan3_port_queues_get(unit, port, 1, &queues);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Unit %d *** Error: Failed getting queue info for port %d\n"),
                           unit, port));
                return;
            }
            idx = sws_pr_icc_reserved_rules_get(unit, PR1_INSTANCE);
            SOC_SBX_C3_BMP_CLEAR(qtemp.squeue_bmp, SOC_SBX_CALADAN3_NUM_WORDS_FOR_SQUEUES);
            SOC_SBX_C3_BMP_ITER_RANGE(queues->squeue_bmp, 
                                      SOC_SBX_CALADAN3_SWS_FABRIC_SQUEUE_BASE,
                                      qid, SOC_SBX_CALADAN3_NUM_WORDS_FOR_SQUEUES) {
                if (SOC_SBX_C3_BMP_MEMBER_IN_RANGE(qtemp.squeue_bmp, 
                                                   SOC_SBX_CALADAN3_SWS_FABRIC_SQUEUE_BASE, 
                                                   qid)) {
                   continue;
                }
                SOC_SBX_C3_BMP_ADD(qtemp.squeue_bmp, qid % SOC_SBX_CALADAN3_SWS_QUEUE_PER_REGION);
                sal_memset(&data, 0, sizeof(data));
                sal_memset(&icckey, 0, sizeof(icckey));
                sal_memset(&iccmsk, 0, sizeof(iccmsk));

                icckey[4] = port;
                iccmsk[4] = 0xff;
                icckey[8] = cos << 3; 
                iccmsk[8] = cos > 0 ? (0x7 << 3) : 0;

                data.queue_action = SOC_SBX_CALADAN3_PR_QUEUE_ACTION_LOOKUP;
                data.last = 1;
                need_matchall = 1;
                data.queue = qid;
                soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, PR1_INSTANCE, 
                                                         idx,
                                                         SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                                                         icckey, iccmsk, 
                                                         state, smask, 
                                                         &data);
                idx++;
                cos++;
            }
            sws_pr_icc_reserved_rules_set(unit, PR1_INSTANCE, idx);
        }
    }
    if (need_matchall) {
        if (idx < 255) {
            /* MATCH ALL entry:
             * Redirect packets to CMIC in future, allow override using soc parameter
             * Currently set to Drop.
             */
            port = CMIC_PORT(unit);
            rv = soc_sbx_caladan3_port_queues_get(unit, port, 1, &queues);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Unit %d *** Error: Failed getting queue info for port %d\n"),
                           unit, port));
                return;
            }
            data.queue_action = SOC_SBX_CALADAN3_PR_QUEUE_ACTION_LOOKUP;
            data.drop  = 1;
            data.last = 1;
            data.queue = queues->squeue_base;
            soc_sbx_caladan3_sws_pr_tcam_match_all(unit, PR1_INSTANCE, 
                                                   255, 0x3, &data);
        } else {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "WARNING: Unit %d: Cannot add"
                                 " MATCH ALL entry in PR 1 TCAM, potentially fatal\n"), unit));
        }

        /* Set ICC state */
        soc_sbx_caladan3_pr_set_icc_state(unit, 
            PR1_INSTANCE, SOC_SBX_CALADAN3_SWS_PR_ICC_ENABLE);
    }
}


#if 1
#define SWS_ICC_STATE_FLAG_TAG      0x1
#define SWS_ICC_STATE_FLAG_UNTAG    0x2
static int _soc_sws_pr_icc_tpid_tcam_entry_set(int unit, uint32* tpid, int tpid_num)
{
    int     idx, ii, pr;
    uint8   icckey[26] = {0}, iccmsk[26] = {0};
    uint8   state[4] = {0}, smask[4] = {0};
    uint32  regval = 0, max, stage, flag;
    soc_sbx_caladan3_pr_icc_lookup_data_t   data;
    soc_sbx_calandan3_icc_state_info_t      state_info, state_mask;


    pr = PR0_INSTANCE;
    idx = sws_pr_icc_reserved_rules_get(unit, pr);
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, PR_ICC_CONFIG0r, 
                            pr, 0, &regval));
    max = soc_reg_field_get(unit, PR_ICC_CONFIG0r, regval, LOOKUPS_REQUIREDf);
    /* add tag tcam rule entry */
    for (ii=0; ii<tpid_num+1; ii++, idx++)
    {
        sal_memset(&data, 0, sizeof(data));
        sal_memset(&icckey, 0, sizeof(icckey));
        sal_memset(&iccmsk, 0, sizeof(iccmsk));
        sal_memset(&state_info, 0, sizeof(state_info));
        sal_memset(&state_mask, 0, sizeof(state_mask));

        flag = SWS_ICC_STATE_FLAG_UNTAG;
        if (ii < tpid_num)
        {
            flag = SWS_ICC_STATE_FLAG_TAG;
            icckey[12] = (uint8)((tpid[ii] & 0xFF00) >> 8);
            icckey[13] = (uint8)(tpid[ii] & 0xFF);
            iccmsk[12] = 0xFF;
            iccmsk[13] = 0xFF;
        }

        state_info.stage = 0;
        state_mask.stage = 0xF;
        soc_sbx_calandan3_sws_icc_state_key_pack(&state_info, state);
        soc_sbx_calandan3_sws_icc_state_key_pack(&state_mask, smask);


        data.last = 0;
        data.state = soc_sbx_calandan3_sws_icc_state_result_get(flag, 1, 0);
        soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, pr, idx, 
                      SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                      icckey, iccmsk, 
                      state, smask, 
                      &data);
    }

    /* add middle stage rules */
    for (stage=1; stage<max-1; stage++)
    {
        sal_memset(&data, 0, sizeof(data));
        sal_memset(&icckey, 0, sizeof(icckey));
        sal_memset(&iccmsk, 0, sizeof(iccmsk));
        sal_memset(&state_info, 0, sizeof(state_info));
        sal_memset(&state_mask, 0, sizeof(state_mask));

        state_info.stage = stage;
        state_mask.stage = 0xF;
        state_info.flag = SWS_ICC_STATE_FLAG_TAG;
        state_mask.flag = 0xF;
        soc_sbx_calandan3_sws_icc_state_key_pack(&state_info, state);
        soc_sbx_calandan3_sws_icc_state_key_pack(&state_mask, smask);
        data.state = soc_sbx_calandan3_sws_icc_state_result_get(SWS_ICC_STATE_FLAG_TAG, 2, 0);
        soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, pr, idx, 
                      SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                      icckey, iccmsk, 
                      state, smask, 
                      &data);
        idx++;

        state_info.flag = SWS_ICC_STATE_FLAG_UNTAG;
        state_mask.flag = 0xF;
        soc_sbx_calandan3_sws_icc_state_key_pack(&state_info, state);
        soc_sbx_calandan3_sws_icc_state_key_pack(&state_mask, smask);
        data.state = soc_sbx_calandan3_sws_icc_state_result_get(SWS_ICC_STATE_FLAG_UNTAG, 2, 0);
        soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, pr, idx, 
                      SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                      icckey, iccmsk, 
                      state, smask, 
                      &data);
        idx++;
        
    }

    sws_pr_icc_reserved_rules_set(unit, pr, idx);

    return SOC_E_NONE;
}


static int _soc_sws_pr_icc_dot1p_queue_map_tcam_entry_set(int unit, uint8* dot1p_queue_map)
{
    int     idx, ii, pr;
    uint8   icckey[26] = {0}, iccmsk[26] = {0};
    uint8   state[4] = {0}, smask[4] = {0};
    uint32  regval = 0, max;
    soc_sbx_caladan3_pr_icc_lookup_data_t   data;
    soc_sbx_calandan3_icc_state_info_t      state_info, state_mask;

    pr = PR0_INSTANCE;
    idx = sws_pr_icc_reserved_rules_get(unit, pr);
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, PR_ICC_CONFIG0r, 
                            pr, 0, &regval));
    max = soc_reg_field_get(unit, PR_ICC_CONFIG0r, regval, LOOKUPS_REQUIREDf);
    /* add tag tcam rule entry */
    for (ii=0; ii<8; ii++, idx++)
    {
        sal_memset(&data, 0, sizeof(data));
        sal_memset(&icckey, 0, sizeof(icckey));
        sal_memset(&iccmsk, 0, sizeof(iccmsk));
        sal_memset(&state_info, 0, sizeof(state_info));
        sal_memset(&state_mask, 0, sizeof(state_mask));

        state_info.stage    = max - 1;
        state_info.flag     = SWS_ICC_STATE_FLAG_TAG;
        state_mask.stage    = 0xF;
        state_mask.flag     = 0xF;
        soc_sbx_calandan3_sws_icc_state_key_pack(&state_info, state);
        soc_sbx_calandan3_sws_icc_state_key_pack(&state_mask, smask);

        icckey[14] = (uint8)(ii << 5);
        iccmsk[14] = 0xE0;

        data.last = 1;
        data.queue_action = SOC_SBX_CALADAN3_PR_QUEUE_ACTION_INDEXED;
        data.queue = dot1p_queue_map[ii];
        data.state = soc_sbx_calandan3_sws_icc_state_result_get(SWS_ICC_STATE_FLAG_TAG, 1, 0);
        soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, pr, idx, 
                      SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                      icckey, iccmsk, 
                      state, smask, 
                      &data);
    }

    /* add untag tcam rule entry */
    sal_memset(&data, 0, sizeof(data));
    sal_memset(&icckey, 0, sizeof(icckey));
    sal_memset(&iccmsk, 0, sizeof(iccmsk));
    sal_memset(&state_info, 0, sizeof(state_info));
    sal_memset(&state_mask, 0, sizeof(state_mask));
    state_info.stage    = max - 1;
    state_info.flag     = SWS_ICC_STATE_FLAG_UNTAG;
    state_mask.stage    = 0xF;
    state_mask.flag     = 0xF;
    soc_sbx_calandan3_sws_icc_state_key_pack(&state_info, state);
    soc_sbx_calandan3_sws_icc_state_key_pack(&state_mask, smask);

    data.last = 1;
    data.queue_action = SOC_SBX_CALADAN3_PR_QUEUE_ACTION_INDEXED;
    data.queue = 0;     /* untg default map to queue0 */
    soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, pr, idx, 
                      SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                      icckey, iccmsk, 
                      state, smask, 
                      &data);
    idx++;
    sws_pr_icc_reserved_rules_set(unit, pr, idx);

    return SOC_E_NONE;
}




static int _soc_sws_pr_icc_dot1p_de_map_tcam_entry_set(int unit, uint8* dot1p_de_map)
{
    int     idx, ii, pr;
    uint8   icckey[26] = {0}, iccmsk[26] = {0};
    uint8   state[4] = {0}, smask[4] = {0};
    uint32  regval = 0, max;
    soc_sbx_caladan3_pr_icc_lookup_data_t   data;
    soc_sbx_calandan3_icc_state_info_t      state_info, state_mask;

    pr = PR0_INSTANCE;
    idx = sws_pr_icc_reserved_rules_get(unit, pr);
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, PR_ICC_CONFIG0r, 
                            pr, 0, &regval));
    max = soc_reg_field_get(unit, PR_ICC_CONFIG0r, regval, LOOKUPS_REQUIREDf);
    /* add tag tcam rule entry */
    for (ii=0; ii<8; ii++, idx++)
    {
        sal_memset(&data, 0, sizeof(data));
        sal_memset(&icckey, 0, sizeof(icckey));
        sal_memset(&iccmsk, 0, sizeof(iccmsk));
        sal_memset(&state_info, 0, sizeof(state_info));
        sal_memset(&state_mask, 0, sizeof(state_mask));

        state_info.stage    = max - 1;
        state_info.flag     = SWS_ICC_STATE_FLAG_TAG;
        state_mask.stage    = 0xF;
        state_mask.flag     = 0xF;
        soc_sbx_calandan3_sws_icc_state_key_pack(&state_info, state);
        soc_sbx_calandan3_sws_icc_state_key_pack(&state_mask, smask);

        icckey[14] = (uint8)(ii << 5);
        iccmsk[14] = 0xE0;

        data.last = 1;
        data.select_de = 0;     /* ??????????????? */
        data.default_de = dot1p_de_map[ii];
        data.state = soc_sbx_calandan3_sws_icc_state_result_get(SWS_ICC_STATE_FLAG_TAG, 1, 0);
        soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, pr, idx, 
                      SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                      icckey, iccmsk, 
                      state, smask, 
                      &data);
    }

    /* add untag tcam rule entry */
    sal_memset(&data, 0, sizeof(data));
    sal_memset(&icckey, 0, sizeof(icckey));
    sal_memset(&iccmsk, 0, sizeof(iccmsk));
    sal_memset(&state_info, 0, sizeof(state_info));
    sal_memset(&state_mask, 0, sizeof(state_mask));
    state_info.stage    = max - 1;
    state_info.flag     = SWS_ICC_STATE_FLAG_UNTAG;
    state_mask.stage    = 0xF;
    state_mask.flag     = 0xF;
    soc_sbx_calandan3_sws_icc_state_key_pack(&state_info, state);
    soc_sbx_calandan3_sws_icc_state_key_pack(&state_mask, smask);

    data.last = 1;
    data.select_de = 0;     /* ?????????????? */
    data.default_de = 1;     /* untg default map to queue0 */
    soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, pr, idx, 
                      SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_VALID,
                      icckey, iccmsk, 
                      state, smask, 
                      &data);
    idx++;
    sws_pr_icc_reserved_rules_set(unit, pr, idx);

    return SOC_E_NONE;
}


int sws_pr_icc_multi_queue_mode_set(int unit)
{
    uint32  tpid_conf[] = {0x8100, 0x9100, 0x9200, 0x88a8};
    uint8   dot1p_queue_map[] = {0, 0, 1, 1, 2, 2, 3, 3};
    int     idx;

    idx = 0;
    sws_pr_icc_reserved_rules_set(unit, PR0_INSTANCE, idx);
    _soc_sws_pr_icc_tpid_tcam_entry_set(unit, tpid_conf, COUNTOF(tpid_conf));
    _soc_sws_pr_icc_dot1p_queue_map_tcam_entry_set(unit, dot1p_queue_map);
    idx = sws_pr_icc_reserved_rules_get(unit, PR0_INSTANCE);
    LOG_CLI((BSL_META_U(unit,
                        "    sws_pr_icc_multi_queue_mode_set: total tcam idx=%d\n\r"), idx));
    return SOC_E_NONE;
}

int sws_pr_icc_multi_de_mode_set(int unit)
{
    uint32  tpid_conf[] = {0x8100, 0x9100, 0x9200, 0x88a8};
    uint8   dot1p_de_map[] = {3, 3, 2, 2, 1, 1, 0, 0};
    int     idx;

    idx = 0;
    sws_pr_icc_reserved_rules_set(unit, PR0_INSTANCE, idx);
    _soc_sws_pr_icc_tpid_tcam_entry_set(unit, tpid_conf, COUNTOF(tpid_conf));
    _soc_sws_pr_icc_dot1p_de_map_tcam_entry_set(unit, dot1p_de_map);
    idx = sws_pr_icc_reserved_rules_get(unit, PR0_INSTANCE);
    LOG_CLI((BSL_META_U(unit,
                        "    sws_pr_icc_multi_de_mode_set: total tcam idx=%d\n\r"), idx));
    return SOC_E_NONE;
}



int sws_pr_icc_disable_all_entry(int unit)
{
    int     idx, pr;
    uint8   icckey[26] = {0}, iccmsk[26] = {0};
    uint8   state[4] = {0}, smask[4] = {0};
    soc_sbx_caladan3_pr_icc_lookup_data_t   data;
    soc_sbx_calandan3_icc_state_info_t      state_info, state_mask;

    pr = PR0_INSTANCE;
    sal_memset(&data, 0, sizeof(data));
    sal_memset(&icckey, 0, sizeof(icckey));
    sal_memset(&iccmsk, 0, sizeof(iccmsk));
    sal_memset(&state_info, 0, sizeof(state_info));
    sal_memset(&state_mask, 0, sizeof(state_mask));
    for (idx=0; idx<255; idx++)
    {
        soc_sbx_caladan3_sws_pr_icc_tcam_program(unit, pr, idx, 
                      SOC_SBX_CALADAN3_SWS_PR_TCAM_ENTRY_INVALID,
                      icckey, iccmsk, 
                      state, smask, 
                      &data);
    }
    sws_pr_icc_reserved_rules_set(unit, PR0_INSTANCE, 0);
    LOG_CLI((BSL_META_U(unit,
                        "    sws_pr_icc_disable_all_entry:\n\r")));

    return SOC_E_NONE;
}


#endif


int
soc_sbx_caladan3_sws_tdm_has_channelized_ports(int unit)
{
    int channelized = FALSE;
    int dummy = 0;
    int rv = 0, port;

    PBMP_ALL_ITER(unit, port) {
        rv =  soc_sbx_caladan3_port_is_channelized_subport(unit, port, &channelized, &dummy);
        if (SOC_SUCCESS(rv) && (channelized == TRUE)) {
            return TRUE;
        }
    }
    return FALSE;
}

void
soc_sbx_caladan3_sws_pr_icc_enable(int unit)
{
    soc_sbx_caladan3_pr_icc_lookup_data_t data;
    sal_memset(&data, 0, sizeof(data));
    data.queue_action = SOC_SBX_CALADAN3_PR_QUEUE_ACTION_DEFAULT;
    data.last = 1;
    soc_sbx_caladan3_sws_pr_tcam_match_all(unit, PR0_INSTANCE, 255, 3, &data);
    soc_sbx_caladan3_sws_pr_tcam_match_all(unit, PR1_INSTANCE, 255, 3, &data);

    /* Set ICC state */
    soc_sbx_caladan3_pr_set_icc_state(unit, 
            PR0_INSTANCE, SOC_SBX_CALADAN3_SWS_PR_ICC_ENABLE);
    soc_sbx_caladan3_pr_set_icc_state(unit, 
            PR1_INSTANCE, SOC_SBX_CALADAN3_SWS_PR_ICC_ENABLE);
}

void
soc_sbx_caladan3_sws_pr_icc_program_port_match_entries(int unit)
{
    int first = 1, idx = 0, port = 0, portidx = 0;
    soc_sbx_caladan3_port_map_info_t *port_info = NULL;
    soc_sbx_caladan3_port_map_t *port_map = NULL;
    int channel = 0, add_entry;
    int rv;

    port_map = SOC_SBX_CFG_CALADAN3(unit)->port_map;

    /* Do we need to add port match entries to the ICC ?? */

    if (soc_sbx_caladan3_is_expanded_port_space(unit) ||
        soc_sbx_caladan3_sws_tdm_has_channelized_ports(unit) ||
        soc_sbx_caladan3_port_is_wc_remapped(unit) ) {

        PBMP_ALL_ITER(unit, port) {

            /* Do we need an entry for this port?? */
            add_entry = FALSE; 

            if (!soc_sbx_caladan3_is_line_port(unit, port)) {
                /* Skip fabric ports */
                continue;
            }

            rv = soc_sbx_caladan3_sws_pbmp_to_line_port(unit, port, &portidx);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Unit %d: "
                                      "*** Error: Failed to get port queue info for port %d\n"), unit, port));
                return;
            }
            /* We need squeue */
            port_info = &port_map->line_port_info[portidx];
         
            if (IS_IL_PORT(unit, port)) {
                channel = port_info->port_queues.squeue_base;
                add_entry = TRUE;
            }
            if (IS_E_PORT(unit, port) ||
                port_info->intftype == SOC_SBX_CALADAN3_PORT_INTF_CMIC) {
                add_entry = TRUE;
            }

            if (add_entry) {
                /* Fabric ports are skipped per above so could also just use PR0_INSTANCE
                 * This really only needs to be called for first port mapping on the PR.
                 * It's a lightweight function however..
                 */
                soc_sbx_caladan3_sws_set_icc_state(unit, 
                                                   PR_INSTANCE(port_info->physical_intf),
                                                   SOC_SBX_CALADAN3_SWS_PR_ICC_ENABLE);

                idx = sws_pr_icc_reserved_rules_get(unit, PR_INSTANCE(port_info->physical_intf));
                /*
                 * ICC 3 Stage method
                 *  -- Using multi-stage rule to match all packets on given port
                 */
                if (IS_XL_PORT(unit, port) && (SOC_PORT_BINDEX(unit, port) & 1)) {
                    /* Skip */
                    continue;
                }
                sws_pr_icc_port_match_rule_set(unit, PR_INSTANCE(port_info->physical_intf), 
                                               port_info->base_port + channel,
                                               port_info->port_queues.squeue_base,
                                               &idx);
                
                sws_pr_icc_reserved_rules_set(unit, 
                       PR_INSTANCE(port_info->physical_intf), idx);
            } else {
                if (!first && SOC_SBX_IS_HG_INTF_TYPE(port_info->intftype)) {
                    first = 0;
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "INFO: Unit %d: "
                                            "HG Interface requires PR ICC Tcam for corrrect operation\n"), unit));
                }
            }
        }
    }

}


#endif


