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
#ifndef _DPP_SW_STATE_H
#define _DPP_SW_STATE_H

#define DPP_SOC_CONFIG_VERSION_LENGTH_MAX 100

typedef struct soc_dpp_soc_arad_sw_state_s{
    PARSER_HINT_PTR soc_arad_sw_state_tm_t *tm;
    PARSER_HINT_PTR soc_arad_sw_state_pp_t *pp;
} soc_dpp_soc_arad_sw_state_t;
/*
 * Entry point structure for all SAND utilities.
 */
typedef struct soc_dpp_soc_sand_sw_state_s{
  PARSER_HINT_PTR soc_sand_sw_state_sorted_list_t *sorted_list ;
  PARSER_HINT_PTR soc_sand_sw_state_occ_bitmap_t  *occ_bitmap ;
  PARSER_HINT_PTR soc_sand_sw_state_hash_table_t  *hash_table ;
  PARSER_HINT_PTR soc_sand_sw_state_multi_set_t   *multi_set ;
} soc_dpp_soc_sand_sw_state_t ;


typedef struct soc_jericho_sw_state_tm_s {
    uint16 nof_remote_faps_with_remote_credit_value;
    PARSER_HINT_PTR SOC_TMC_FC_PFC_GEN_BMP_INFO *pfc_gen_bmp;
    PARSER_HINT_ARR SOC_TMC_ITM_INGRESS_RESERVED_RESOURCE *ingress_reserved_resource;
    soc_pbmp_t is_tdm_queuing_on; /*port bitmap indicating if port have tdm queuing on */	
} soc_jericho_sw_state_tm_t;


typedef struct soc_jericho_sw_state_pp_s {
  PARSER_HINT_PTR jer_pp_mpls_term_t mpls_term;
} soc_jericho_sw_state_pp_t; 

typedef struct soc_dpp_soc_jericho_sw_state_s {
    soc_jericho_sw_state_tm_t tm;
    soc_jericho_sw_state_pp_t pp;
} soc_dpp_soc_jericho_sw_state_t;

typedef struct soc_qax_sw_state_tm_s {
    soc_lb_info_t lb_info;
} soc_qax_sw_state_tm_t;

typedef struct soc_dpp_soc_qax_sw_state_s {
    soc_qax_sw_state_tm_t       tm;
    soc_qax_sw_state_ipsec_t    ipsec;
} soc_dpp_soc_qax_sw_state_t;

typedef struct soc_dpp_soc_config_s{
    uint32      autosync;
    char        version[DPP_SOC_CONFIG_VERSION_LENGTH_MAX];
} soc_dpp_soc_config_t;

typedef struct soc_dpp_soc_sw_state_s{
    soc_dpp_soc_arad_sw_state_t arad;
	soc_dpp_soc_jericho_sw_state_t jericho;
    soc_dpp_soc_qax_sw_state_t qax;
	soc_dpp_soc_sand_sw_state_t sand;
    soc_dpp_soc_config_t config;
} soc_dpp_soc_sw_state_t;

typedef struct soc_dpp_shr_sw_state_s{
    _sw_state_res_unit_desc_t       *resmgr_info;
    sw_state_hash_table_db_t        htbls;
    sw_state_res_tag_bitmap_info_t  bmp_info;
    _shr_template_unit_desc_t       *template_info;
} soc_dpp_shr_sw_state_t;


#endif /*#define _DPP_SW_STATE_H*/

