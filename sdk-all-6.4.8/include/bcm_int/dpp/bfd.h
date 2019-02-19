/*
 * $Id: oam.h,v 1.15 Broadcom SDK $ 
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * IPMC Internal header
 */
#ifndef _BCM_INT_DPP_BFD_H_
#define _BCM_INT_DPP_BFD_H_

#include <bcm/bfd.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/oam_sw_db.h>
#include <bcm_int/dpp/oam_hw_db.h>
#include <bcm_int/dpp/oam_resource.h>
#include <bcm_int/dpp/oam_dissect.h>

/*ttl tos profile for bfd ipv4 single hop*/
#define _BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE 15
#define _BCM_BFD_IPV4_ECHO_MIN_INTERVAL_PROFILE 14

typedef struct {
    int ref_counter;
    int tos_value;
} bcm_dpp_bfd_pp_s_hop;

typedef struct {
    int                         YourDiscriminator_ref_count;
    bcm_dpp_bfd_pp_s_hop        s_hop;
    SOC_PPC_BFD_INIT_TRAP_INFO  trap_info;
    uint32                      mpls_udp_sport_ref_counter;
    ENDPOINT_LIST_PTR           _bcm_bfd_endpoint_list ;
    uint32                      ipv4_udp_sport_ref_counter;
    uint32                      ipv4_multi_hop_acc_ref_counter;
    uint32                      pdu_ref_counter;
    uint32                      mpls_tp_cc_ref_counter;
} bcm_dpp_bfd_info_t;


typedef struct {
    uint8 free_tx_gport;
    uint8 free_src_ip_profile;
    uint8 free_tos_ttl_profile;
    uint8 free_bfd_tx_rate;
    uint8 free_req_interval_pointer_local_min_rx;
    uint8 free_req_interval_pointer_local_min_tx;
    uint8 free_pwe_push_profile;
    uint8 free_mhop_itmh_attributes;
    uint8 free_tx_itmh_attributes;
    uint8 free_flags;
} _bcm_bfd_mep_db_entry_pointers_to_free;


void bcm_dpp_bfd_endpoint_diag_print(bcm_bfd_endpoint_info_t* endpoint_info);


/*Function shared by OAM diagnostics and bfd diagnostics. 
   Simply prints information on an egress label*/
void bcm_dpp_print_mpls_egress_label(bcm_mpls_egress_label_t* egress_label);

int _bcm_petra_bfd_detach(int unit);

int  _bcm_bfd_endpoint_info_to_lem_info(int unit, const bcm_bfd_endpoint_info_t *ep_info, ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO *lem_info, int trap_code, int fwd_strenght, int snp_strength);

#endif /*_BCM_INT_DPP_BFD_H_*/
