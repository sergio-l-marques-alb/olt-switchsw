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
* $File:  tomahawk2_flexport.h
*/


#ifndef SOC_TOMAHAWK2_FLEXPORT_H
#define SOC_TOMAHAWK2_FLEXPORT_H


/*! @file tomahawk2_flexport.h
 *   @brief FlexPort functions and structures for Tomahawk2.
 *   Details are shown below.
 */

#include <soc/flexport/flexport_common.h>
#include <soc/flexport/tomahawk2_tdm_flexport.h>
#include <soc/flexport/tomahawk2_mmu_flex_init.h>
#include <soc/flexport/tomahawk2_idb_flexport.h>
#include <soc/flexport/tomahawk2_ep_flexport.h>

#include <soc/flexport/tomahawk2_mac_flexport.h>

/*! @struct soc_tomahawk2_flex_scratch_t
 *   @brief Scratch pad locals. Caller must not allocate. Callee allocates
 *          at entry and deallocates before return. This area is pointed
 *          to by the cookie in soc_port_schedule_state_t struct type.
 */
typedef struct soc_tomahawk2_flex_scratch_s {
    /*!< Previous OVS tables needed by the OVS consolidation sequence */
    soc_tdm_schedule_pipe_t prev_tdm_ingress_schedule_pipe[SOC_MAX_NUM_PIPES];
    soc_tdm_schedule_pipe_t prev_tdm_egress_schedule_pipe[SOC_MAX_NUM_PIPES];
    /*!< Current state of OVS tables; is changing during flexport sequence
     *   initial state is prev OVS tables and after OVS flexport sequence
     *   converges to new_ OVS tables*/
    soc_tdm_schedule_pipe_t curr_tdm_ingress_schedule_pipe[SOC_MAX_NUM_PIPES];
    soc_tdm_schedule_pipe_t curr_tdm_egress_schedule_pipe[SOC_MAX_NUM_PIPES];
    /*!< Previous and new Oversub ratios; per PIPE and per HALF PIPE
     * These are coded as follows:
     *  10  - this is LR;
     *  15  - this is OVS with 3:2;         ovs_ratio <= 1.5;
     *  20  - this is OVS with 2:1;  1.5 <  ovs_ratio <= 2.0;
     */
    int prev_pipe_ovs_ratio[SOC_MAX_NUM_PIPES]; /* per PIPE ovs_ratio
                                                 * per Half PIPE ovs_ratio */
    int prev_hpipe_ovs_ratio[SOC_MAX_NUM_PIPES][_TH2_OVS_HPIPE_COUNT_PER_PIPE];
    int new_pipe_ovs_ratio[SOC_MAX_NUM_PIPES]; /* per PIPE ovs_ratio
                                                * per Half PIPE ovs_ratio */
    int new_hpipe_ovs_ratio[SOC_MAX_NUM_PIPES][_TH2_OVS_HPIPE_COUNT_PER_PIPE];

    /* This keeps the exact bitrate of the out_port_map.log_port_speed[] */
    enum port_speed_e exact_out_log_port_speed[SOC_MAX_NUM_PORTS];
} soc_tomahawk2_flex_scratch_t;


extern void *soc_tomahawk2_port_lane_info_alloc (void);


extern int soc_tomahawk2_set_port_lane_info (
    int         unit,
    const void *lane_info_ptr,
    int         port_idx,
    int         pgw,
    int         xlp
    );


extern int soc_tomahawk2_get_port_lane_info (
    int         unit,
    const void *lane_info_ptr,
    int *       port_idx,
    int *       pgw,
    int *       xlp
    );


extern int soc_tomahawk2_reconfigure_ports (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk2_flex_start (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk2_flex_end (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk2_flex_top_port_down (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk2_flex_top_port_up (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );

extern int soc_tomahawk2_get_ct_class(int speed);

extern int (*flexport_phases[MAX_FLEX_PHASES]) (int unit,
                                                soc_port_schedule_state_t
                                                *port_schedule_state);

#endif /* SOC_TOMAHAWK2_FLEXPORT_H */
