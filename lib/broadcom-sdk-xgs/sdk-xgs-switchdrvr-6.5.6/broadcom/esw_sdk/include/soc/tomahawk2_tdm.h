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
 * File:        tomahawk2_tdm.h
 */

#ifndef _SOC_TOMAHAWK2_TDM_H_
#define _SOC_TOMAHAWK2_TDM_H_

#include <soc/esw/port.h>


/*** START SDK API COMMON CODE ***/

extern int _soc_tomahawk2_tdm_mmu_calendar_set(
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_idb_calendar_set(
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_oversub_group_set(
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_oversub_group_set_sel(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int set_idb, int set_mmu);


extern void _soc_tomahawk2_speed_to_ovs_class_mapping(int  unit,
                                                      int  speed,
                                                      int *ovs_class);

extern void soc_tomahawk2_get_hsp_info(int  unit,
                                        int  speed,
                                        int  is_idb,
                                        int  is_mmu,
                                        int *is_hsp,
                                        int *same_spacing);

extern void soc_tomahawk2_port_ratio_get(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int clport, int *mode, int prev_or_new);


extern int _soc_tomahawk2_tdm_pkt_shaper_calendar_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_idb_opportunistic_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state, int cpu_op_en,
    int lb_opp_en, int opp1_port_en, int opp2_port_en, int opp_ovr_sub_en);


extern int _soc_tomahawk2_tdm_mmu_opportunistic_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int cpu_op_en, int lb_opp_en, int opp1_port_en, int opp2_port_en,
    int opp_ovr_sub_en);


extern int _soc_tomahawk2_tdm_idb_hsp_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_mmu_hsp_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_idb_ppe_credit_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int full_credit_threshold_0,
    int opp_credit_threshold_0,
    int full_credit_threshold_1,
    int opp_credit_threshold_1 );


extern int _soc_tomahawk2_tdm_calculation(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);


extern void soc_tomahawk2_pipe_map_get(
    int     unit,
    soc_port_schedule_state_t *port_schedule_state, uint32 *pipe_map);


extern int soc_tomahawk2_tdm_init(
    int unit,
    soc_port_schedule_state_t *
    port_schedule_state);


extern int _soc_tomahawk2_tdm_idb_dpp_ctrl_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int credits);


extern void soc_tomahawk2_ovs_pipe_map_get(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    uint32 *ovs_pipe_map);


extern void soc_print_port_schedule_state(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern void soc_print_port_map(int unit, soc_port_map_type_t *port_map);


extern void soc_print_tdm_schedule_slice( int unit, 
    soc_tdm_schedule_t *tdm_schedule, int hpipe, int is_ing);


extern void soc_print_port_resource(int unit,
    soc_port_resource_t *port_resource, int entry_num);


extern void
soc_tomahawk2_port_schedule_speed_remap(
    int  unit,
    soc_port_schedule_state_t *port_schedule_state);

/*** END SDK API COMMON CODE ***/

#endif	/* !_SOC_TOMAHAWK2_TDM_H_ */
