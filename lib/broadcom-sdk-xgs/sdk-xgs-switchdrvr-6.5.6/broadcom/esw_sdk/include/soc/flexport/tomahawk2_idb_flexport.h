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
* $File:  tomahawk2_idb_flexport.h
*/


#ifndef TOMAHAWK2_IDB_FLEXPORT_H
#define TOMAHAWK2_IDB_FLEXPORT_H

extern int soc_tomahawk2_get_pipe_from_phy_pnum(int pnum);


extern int soc_tomahawk2_get_pm_from_phy_pnum(int pnum);


extern int soc_tomahawk2_get_subp_from_phy_pnum(int pnum);


extern int soc_tomahawk2_idb_init_pnum_mapping_tbl(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_idb_invalidate_pnum_mapping_tbl(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_idb_clear_stats_new_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_idb_obm_reset_buffer(int unit, int pipe_num,
                                              int pm_num, int subp,
                                              int reset_buffer);


extern int soc_tomahawk2_idb_ca_reset_buffer(int unit, int pipe_num, int pm_num,
                                             int subp,
                                             int reset_buffer);

extern int soc_tomahawk2_idb_lpbk_ca_reset_buffer(int unit, int pipe_num,
                                                  int reset_buffer);

extern int soc_tomahawk2_idb_cpu_ca_reset_buffer(int unit, int pipe_num,
                                                 int reset_buffer);


extern int soc_tomahawk2_idb_port_mode_set(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_idb_wr_obm_shared_config(int unit, int pipe_num,
                                                  int pm_num, int subp,
                                                  int num_lanes,
                                                  int lossless);


extern int soc_tomahawk2_idb_wr_obm_flow_ctrl_cfg(int unit, int pipe_num,
                                                  int pm_num,
                                                  int subp,
                                                  int lossless);


extern int soc_tomahawk2_idb_wr_obm_ovs_en(int unit, int pipe_num, int pm_num,
                                            int subp);

extern int soc_tomahawk2_idb_wr_obm_thresh(int unit, int pipe_num, int pm_num,
                                           int subp,
                                           int num_lanes,
                                           int lossless);

extern int soc_tomahawk2_idb_wr_obm_fc_threshold(int unit, int pipe_num,
                                                 int pm_num, int subp,
                                                 int num_lanes,
                                                 int lossless);


extern int soc_tomahawk2_idb_obm_poll_buffer_empty(int unit, int pipe_num,
                                                   int pm_num,
                                                   int subp);


extern int soc_tomahawk2_idb_ca_poll_buffer_empty(int unit, int pipe_num,
                                                  int pm_num,
                                                  int subp);


extern int soc_tomahawk2_speed_to_bmop_class_map(int speed);


extern int soc_tomahawk2_flex_dis_forwarding_traffic(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_flex_en_forwarding_traffic(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_idb_obm_port_config_set(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_idb_obm_dscp_map_set(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_idb_obm_bubble_mop_set(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_idb_obm_force_saf_set(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_flex_idb_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_flex_idb_reconfigure(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_flex_idb_port_up(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_tomahawk2_idb_flexport(
    int unit, soc_port_schedule_state_t *port_schedule_state);


#endif
