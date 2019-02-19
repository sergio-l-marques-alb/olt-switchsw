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
* $File:  tomahawk2_mmu_port_up_sequence.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk2_tdm.h>
/*! @file tomahawk2_mmu_port_up_sequence.c
 *  @brief
 */

#include <soc/flexport/tomahawk2_flexport.h>


/*! @fn int soc_tomahawk2_mmu_port_up_sequence(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule Struct
 *  @brief API to update all MMU related functionalities during flexport
 *         port up operation.
 */
int
soc_tomahawk2_flex_mmu_reconfigure_phase2(
    int unit, soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    int logical_port;
    int oversub_ratio;
    int exact_port_speed;
    int lossy;
    int bst_mode;
    int pktstat_mode;
    uint64 temp64;
    soc_tomahawk2_flex_scratch_t *cookie;
    cookie = port_schedule_state_t->cookie;

    lossy= !(port_schedule_state_t->lossless);

    soc_tomahawk2_mmu_get_bst_mode(unit, &bst_mode);
    soc_tomahawk2_mmu_get_pktstat_mode(unit, &pktstat_mode);

    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {
            oversub_ratio =
                cookie->prev_pipe_ovs_ratio[port_schedule_state_t->resource[
                                                port].
                                            pipe];
            logical_port = port_schedule_state_t->resource[port].logical_port;
            exact_port_speed= cookie->exact_out_log_port_speed[logical_port];
            /* Re-adjust phy port mapping for valid ports */
            soc_tomahawk2_mmu_set_mmu_to_phy_port_mapping(
                unit, &port_schedule_state_t->resource[port]);
            COMPILER_64_SET(temp64, 0, TH2_MMU_FLUSH_OFF);
            soc_tomahawk2_mmu_vbs_port_flush(
                unit, &port_schedule_state_t->resource[port], temp64);
            soc_tomahawk2_mmu_rqe_port_flush(
                unit, &port_schedule_state_t->resource[port], temp64);
            soc_tomahawk2_mmu_mtro_port_flush(
                unit, &port_schedule_state_t->resource[port], temp64);

            /*  Reinitialize CT setting */   
            soc_tomahawk2_mmu_reinit_ct_setting(
                unit, &port_schedule_state_t->resource[port],
                port_schedule_state_t->cutthru_prop.asf_modes[logical_port],
                port_schedule_state_t->cutthru_prop.asf_mem_prof,
                port_schedule_state_t->frequency,
                exact_port_speed, oversub_ratio);

            /* Clear MTRO bucket memories */
            soc_tomahawk2_mmu_clear_mtro_bucket_mems(
                unit, &port_schedule_state_t->resource[port]);
            /* Clear VBS credit memories*/
            soc_tomahawk2_mmu_clear_vbs_credit_memories(
                unit, &port_schedule_state_t->resource[port]);
            /* Clear WRED Avg_Qsize instead of waiting for background process*/
            soc_tomahawk2_mmu_wred_clr(unit,
                                       &port_schedule_state_t->resource[port]);
            soc_tomahawk2_mmu_thdi_setup(unit,
                                         &port_schedule_state_t->resource[port],
                                         lossy);
            soc_tomahawk2_mmu_thdu_qgrp_min_limit_config(
                unit, &port_schedule_state_t->resource[port], lossy);
            /* Clear Drop Counters in CTR block*/
            soc_tomahawk2_mmu_ctr_clr(unit,
                                      &port_schedule_state_t->resource[port]);
            if (bst_mode == TH2_MMU_BST_WMARK_MODE) {
                /* Clear THDI BST in watermark mode..*/
                soc_tomahawk2_mmu_thdi_bst_clr(
                    unit, &port_schedule_state_t->resource[port]);
                /* Clear THDU BST counters in watermark mode*/
                soc_tomahawk2_mmu_thdu_bst_clr(
                    unit, &port_schedule_state_t->resource[port]);
                /* Clear THDM BST counters in watermark mode*/
                soc_tomahawk2_mmu_thdm_bst_clr(
                    unit, &port_schedule_state_t->resource[port]);
            }
            if (pktstat_mode == TH2_PKTSTATS_ON) {
                /* Clear PktStat counters in THDU for Queues*/
                soc_tomahawk2_mmu_thdu_pktstat_clr(
                    unit, &port_schedule_state_t->resource[port]); 
            }
            

            if (port_schedule_state_t->resource[port].oversub == 1) {
                /* DIP FSAF Reconfigure setting */
                soc_tomahawk2_mmu_reinit_dip_fsaf_setting(
                    unit, &port_schedule_state_t->resource[port]);
            }
        }
        else {
            soc_tomahawk2_mmu_set_mmu_to_phy_port_mapping(
                unit, &port_schedule_state_t->resource[port]);
        }
    }

    return SOC_E_NONE;
}


int
soc_tomahawk2_flex_mmu_port_up(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {
            /* Clear Previous EP Credits on the port. */
            soc_tomahawk2_mmu_clear_prev_ep_credits(
                unit, &port_schedule_state_t->resource[port]);
        }
    }
    return SOC_E_NONE;
}


#endif /* BCM_TOMAHAWK2_SUPPORT */
