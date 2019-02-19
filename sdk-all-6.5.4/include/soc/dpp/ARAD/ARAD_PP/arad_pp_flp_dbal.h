/* $Id: dpp_dbal.c, v 1.95 Broadcom SDK $
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
 * $
*/

#ifndef __ARAD_PP_FLP_DBAL_INCLUDED__
#define __ARAD_PP_FLP_DBAL_INCLUDED__


#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>

uint32 arad_pp_dbal_flp_hw_based_key_enable(int unit, int program_id, SOC_DPP_HW_KEY_LOOKUP hw_key_based_lookup_enable);


/* oam statistics programs configuration */
uint32 arad_pp_flp_dbal_oam_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_oam_down_untagged_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_bfd_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_oam_single_tag_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_oam_double_tag_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_bfd_mpls_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_bfd_pwe_statistics_program_tables_init(int unit, int bfd_statistics_prog_id);
uint32 arad_pp_dbal_flp_ethernet_ing_ivl_learn_tables_create(int unit, int prog_id);
uint32 arad_pp_dbal_flp_ethernet_ing_ivl_inner_learn_tables_create(int unit, int prog_id);
uint32 arad_pp_dbal_flp_ethernet_ing_ivl_fwd_outer_learn_tables_create(int unit, int prog_id);
soc_error_t arad_pp_flp_dbal_bfd_echo_program_tables_init(int unit);

uint32 arad_pp_flp_dbal_ipv4mc_tcam_tables_init(int unit);

uint32 arad_pp_flp_dbal_trill_program_tcam_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4uc_rpf_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4uc_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4_dc_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4mc_bridge_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4compmc_with_rpf_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4uc_l3vpn_program_tables_init(int unit,int custom_prgrm);
uint32 arad_pp_flp_dbal_ipv4uc_l3vpn_rpf_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_ipv6uc_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv6uc_with_rpf_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_ipv6mc_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv6mc_kbp_table_init(int unit);
uint32 arad_pp_flp_dbal_ipv6uc_kbp_table_init(int unit);
uint32 arad_pp_flp_dbal_ipv6uc_rpf_kbp_table_init(int unit, int prog_id);

uint32 arad_pp_flp_dbal_vpws_tagged_program_tables_init(int unit);

uint32 arad_pp_flp_dbal_fcoe_program_tables_init(int unit, int is_vsan_from_vsi, int prog1, int prog2);
uint32 arad_pp_flp_dbal_fcoe_npv_program_tables_init(int unit, int is_vsan_from_vsi, int fcoe_no_vft_prog_id, int fcoe_vft_prog_id);

/* Update FLP program LEM 1st lookup type to source */
uint32 arad_pp_flp_dbal_source_lookup_with_aget_access_enable(int unit, int prog_id);
uint32 arad_pp_flp_dbal_mpls_lsr_stat_table_init(int unit);



uint32 arad_pp_flp_dbal_program_info_dump(int unit, uint32 cur_prog_id);
uint32 arad_pp_flp_dbal_ethernet_tk_epon_uni_v6_program_tables_init(int unit,uint8 sa_auth_enabled,uint8 slb_enabled,uint32 tcam_access_profile_id);

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif/*__ARAD_PP_DBAL_INCLUDED__*/

