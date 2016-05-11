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
 * INFO: this module is the entry point the branch of the sw state that compose the dpp bcm 
 * layer's sw state 
 *  
 */
#ifndef _BCM_SW_STATE_H
#define _BCM_SW_STATE_H

#include <soc/types.h>
#include <shared/swstate/sw_state.h>
#include <soc/error.h>

typedef struct soc_dpp_bcm_sw_state_s{
    PARSER_HINT_PTR bcm_stg_info_t                       *stg;
    PARSER_HINT_ARR _dpp_policer_state_t                 *policer;
    PARSER_HINT_AUTOSYNC_EXCLUDE PARSER_HINT_PTR _bcm_dpp_counter_state_t             *counter;
    PARSER_HINT_PTR _bcm_petra_mirror_unit_data_t        *mirror;
    PARSER_HINT_PTR bcm_dpp_l3_info_t                    *l3;
    PARSER_HINT_PTR l2_data_t                            *l2;
    PARSER_HINT_PTR trunk_state_t                        *trunk;
    PARSER_HINT_PTR _bcm_dpp_vlan_unit_state_t           *vlan;
    PARSER_HINT_PTR _bcm_dpp_vswitch_bookkeeping_t       *vswitch;
    PARSER_HINT_PTR bcm_dpp_rx_info_t                    *rx;
    PARSER_HINT_PTR bcm_dpp_failover_info_t              *failover;
    PARSER_HINT_PTR bcm_dpp_gport_mgmt_info_t            *gport_mgmt;
    PARSER_HINT_PTR bcm_dpp_bfd_info_t                   *bfd;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_info_t            *alloc_mngr;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_utils_info_t      *alloc_mngr_utils;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_glif_info_t       *alloc_mngr_glif;
    PARSER_HINT_PTR bcm_dpp_alloc_mngr_local_lif_info_t  *alloc_mngr_local_lif;
    PARSER_HINT_PTR bcm_dpp_ipmc_info_t                  *ipmc;
    PARSER_HINT_PTR bcm_dpp_switch_info_t                *_switch;
    PARSER_HINT_PTR bcm_dpp_port_info_t                  *port;
    PARSER_HINT_PTR bcm_dpp_trill_state_t                *trill;
    PARSER_HINT_PTR bcm_dpp_stack_config_t               *stack;
    PARSER_HINT_PTR bcm_dpp_qos_state_t                  *qos;
    PARSER_HINT_PTR bcm_dpp_mim_info_t                   *mim;
    PARSER_HINT_PTR bcm_dpp_oam_info_t                   *oam;
    PARSER_HINT_PTR bcm_dpp_cosq_info_t                  *cosq;
    PARSER_HINT_PTR bcm_dpp_field_info_t                 *field;
    PARSER_HINT_PTR bcm_dpp_lif_linked_list_t            *lif_linked_list ;
    PARSER_HINT_PTR bcm_dpp_lb_info_t                    *linkbonding;
} soc_dpp_bcm_sw_state_t;

#endif /* _SHR_SW_STATE_H */
