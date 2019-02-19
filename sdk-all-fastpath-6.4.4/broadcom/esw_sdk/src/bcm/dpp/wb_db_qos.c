/*
 * $Id: wb_db_qos.c,v 1.8 Broadcom SDK $
 *
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
 * Warmboot - Level 2 support (QOS Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm/qos.h>
#include <bcm/fabric.h>
#include <soc/error.h>
#include <soc/types.h>
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/wb_db_qos.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <soc/dpp/soc_sw_db.h>
#include <shared/shr_resmgr.h>
#include <shared/shr_template.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/state.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <soc/dpp/mbcm.h>


#if defined(BCM_WARM_BOOT_SUPPORT)

bcm_dpp_wb_qos_info_t   *_dpp_wb_qos_info_p[BCM_MAX_NUM_UNITS] = {0};

/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_qos_layout_init(int unit, int version)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t    *wb_info; 
    int                      entry_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    wb_info->version = version;

    switch (version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            /* Qos State */
            /* ing_lif_cos */
            wb_info->ing_lif_cos_mpls_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->ing_lif_cos_ipv4_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->ing_lif_cos_ipv6_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->ing_lif_cos_l2_inner_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->ing_lif_cos_l2_outer_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->ing_lif_cos_l2_untag_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_TC_DP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            /* ing_pcp_vlan */
            wb_info->ing_pcp_vlan_ctag_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->ing_pcp_vlan_utag_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->ing_pcp_vlan_stag_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            /* egr_remark */
            wb_info->egr_remark_encap_mpls_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_remark_encap_ipv6_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_remark_encap_ipv4_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_remark_encap_l2_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L2_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_remark_mpls_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_remark_ipv6_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_remark_ipv4_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            /* egr_mpls_php */
            wb_info->egr_mpls_php_ipv6_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_mpls_php_ipv4_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            /* egr_pcp_vlan */
            wb_info->egr_pcp_vlan_ctag_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_pcp_vlan_utag_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_pcp_vlan_stag_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*2) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_pcp_vlan_mpls_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_pcp_vlan_ipv6_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->egr_pcp_vlan_ipv4_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            /* egr_l2_i_tag */
            wb_info->egr_l2_i_tag_bitmap_off = entry_size;
            entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
#ifdef BCM_88660
            if (SOC_IS_ARADPLUS(unit)) {
                /* egr_l2_i_tag */
                wb_info->egr_dscp_exp_marking_bitmap_off = entry_size;
                entry_size += SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_RESOLVED_DP_MAX*_BCM_QOS_MAP_TC_MAX*SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS) * 
                    SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking);
                entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            }
#endif
            
            wb_info->ing_cos_opcode_off = entry_size;
            entry_size += sizeof(int) * SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->opcode_bmp_off = entry_size;
            entry_size += SHR_BITALLOCSIZE(SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->size = entry_size;
                        
            break;
        

        default:
            /* no other version supported */
            rc = BCM_E_INTERNAL;
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_qos_restore_qos_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t                    *wb_info;
    int size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, (wb_info->scache_ptr + wb_info->ing_lif_cos_mpls_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, (wb_info->scache_ptr + wb_info->ing_lif_cos_ipv4_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, (wb_info->scache_ptr + wb_info->ing_lif_cos_ipv6_bitmap_off), size);
    
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap, (wb_info->scache_ptr + wb_info->ing_lif_cos_l2_inner_bitmap_off), size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap, (wb_info->scache_ptr + wb_info->ing_lif_cos_l2_outer_bitmap_off), size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_TC_DP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap, (wb_info->scache_ptr + wb_info->ing_lif_cos_l2_untag_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_ctag_bitmap, (wb_info->scache_ptr + wb_info->ing_pcp_vlan_ctag_bitmap_off), size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_utag_bitmap, (wb_info->scache_ptr + wb_info->ing_pcp_vlan_utag_bitmap_off), size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_stag_bitmap, (wb_info->scache_ptr + wb_info->ing_pcp_vlan_stag_bitmap_off), size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_mpls_bitmap, (wb_info->scache_ptr + wb_info->egr_remark_encap_mpls_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv6_bitmap, (wb_info->scache_ptr + wb_info->egr_remark_encap_ipv6_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv4_bitmap, (wb_info->scache_ptr + wb_info->egr_remark_encap_ipv4_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L2_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_l2_bitmap, (wb_info->scache_ptr + wb_info->egr_remark_encap_l2_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_remark_mpls_bitmap, (wb_info->scache_ptr + wb_info->egr_remark_mpls_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv6_bitmap, (wb_info->scache_ptr + wb_info->egr_remark_ipv6_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv4_bitmap, (wb_info->scache_ptr + wb_info->egr_remark_ipv4_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv6_bitmap, (wb_info->scache_ptr + wb_info->egr_mpls_php_ipv6_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap, (wb_info->scache_ptr + wb_info->egr_mpls_php_ipv4_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ctag_bitmap, (wb_info->scache_ptr + wb_info->egr_pcp_vlan_ctag_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_utag_bitmap, (wb_info->scache_ptr + wb_info->egr_pcp_vlan_utag_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*2) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_stag_bitmap, (wb_info->scache_ptr + wb_info->egr_pcp_vlan_stag_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, (wb_info->scache_ptr + wb_info->egr_pcp_vlan_mpls_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, (wb_info->scache_ptr + wb_info->egr_pcp_vlan_ipv6_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, (wb_info->scache_ptr + wb_info->egr_pcp_vlan_ipv4_bitmap_off), size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_l2_i_tag_bitmap, (wb_info->scache_ptr + wb_info->egr_l2_i_tag_bitmap_off), size);

#ifdef BCM_88660
            if (SOC_IS_ARADPLUS(unit)) {
                size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_RESOLVED_DP_MAX*_BCM_QOS_MAP_TC_MAX*SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS) * 
                    SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking);
                sal_memcpy(SOC_DPP_STATE(unit)->qos_state->egr_dscp_exp_marking_bitmap, 
                           (wb_info->scache_ptr + wb_info->egr_dscp_exp_marking_bitmap_off), size);
            }
#endif 

            size = sizeof(int) * SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode;
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags, (wb_info->scache_ptr + wb_info->ing_cos_opcode_off), size);
            
            size = SHR_BITALLOCSIZE(SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode);
            sal_memcpy(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, (wb_info->scache_ptr + wb_info->opcode_bmp_off), size);            
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_qos_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_qos_restore_qos_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_qos_save_qos_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t                    *wb_info;
    int size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy((wb_info->scache_ptr + wb_info->ing_lif_cos_mpls_bitmap_off), SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy((wb_info->scache_ptr + wb_info->ing_lif_cos_ipv4_bitmap_off), SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy((wb_info->scache_ptr + wb_info->ing_lif_cos_ipv6_bitmap_off), SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy((wb_info->scache_ptr + wb_info->ing_lif_cos_l2_inner_bitmap_off), SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap, size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy((wb_info->scache_ptr + wb_info->ing_lif_cos_l2_outer_bitmap_off), SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap, size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_TC_DP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
            sal_memcpy((wb_info->scache_ptr + wb_info->ing_lif_cos_l2_untag_bitmap_off), SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
            sal_memcpy((wb_info->scache_ptr + wb_info->ing_pcp_vlan_ctag_bitmap_off), SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_ctag_bitmap, size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
            sal_memcpy((wb_info->scache_ptr + wb_info->ing_pcp_vlan_utag_bitmap_off), SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_utag_bitmap, size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
            sal_memcpy((wb_info->scache_ptr + wb_info->ing_pcp_vlan_stag_bitmap_off), SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_stag_bitmap, size);
                
            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_remark_encap_mpls_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_mpls_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_remark_encap_ipv6_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv6_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_remark_encap_ipv4_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv4_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L2_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_remark_encap_l2_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_l2_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_remark_mpls_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_remark_mpls_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_remark_ipv6_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv6_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_remark_ipv4_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv4_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_mpls_php_ipv6_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv6_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_mpls_php_ipv4_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_pcp_vlan_ctag_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ctag_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_pcp_vlan_utag_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_utag_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*2) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_pcp_vlan_stag_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_stag_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_pcp_vlan_mpls_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_pcp_vlan_ipv6_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_pcp_vlan_ipv4_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, size);

            size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag);
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_l2_i_tag_bitmap_off), SOC_DPP_STATE(unit)->qos_state->egr_l2_i_tag_bitmap, size);

#ifdef BCM_88660
            if (SOC_IS_ARADPLUS(unit)) {
                size = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_RESOLVED_DP_MAX*_BCM_QOS_MAP_TC_MAX*SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS) * 
                    SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking);
                sal_memcpy((wb_info->scache_ptr + wb_info->egr_dscp_exp_marking_bitmap_off), 
                           SOC_DPP_STATE(unit)->qos_state->egr_dscp_exp_marking_bitmap, size);
            }
#endif /* BCM_88660 */
            
            size = sizeof(int) * SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode;
            sal_memcpy((wb_info->scache_ptr + wb_info->ing_cos_opcode_off), SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags, size);
            
            size = SHR_BITALLOCSIZE(SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode);
            sal_memcpy((wb_info->scache_ptr + wb_info->opcode_bmp_off), SOC_DPP_STATE(unit)->qos_state->opcode_bmp, size);            
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_qos_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_qos_info_p[unit] == NULL) {
        BCMDNX_ALLOC(_dpp_wb_qos_info_p[unit], sizeof(bcm_dpp_wb_qos_info_t), "wb qos");
        if (_dpp_wb_qos_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Not enough memory available to allocate wb Qos")));
        }
    }

    sal_memset(_dpp_wb_qos_info_p[unit], 0x00, sizeof(bcm_dpp_wb_qos_info_t));

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_qos_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_qos_info_p[unit] != NULL) {
       BCM_FREE(_dpp_wb_qos_info_p[unit]);
        _dpp_wb_qos_info_p[unit] = NULL;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Global functions
 *
 * The save functionality has been implemented
 *   - some consistency checks rather then blindly coping data structures
 *     Thus easier to debug, catch errors.
 *   - The above implementation aslo make its semetric with the the per
 *     API update.
 */

int
_bcm_dpp_wb_qos_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t   *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if ( !(BCM_DPP_WB_QOS_IS_DIRTY(unit)) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_qos_save_qos_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_DPP_WB_QOS_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_state_init(int unit)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_QOS_CURRENT_VERSION, recovered_ver;
    uint8                   *scache_ptr;
    bcm_dpp_wb_qos_info_t   *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    rc = _bcm_dpp_wb_qos_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetrieve, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_qos_layout_init(unit, recovered_ver);
        if (rc != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rc);
        }

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_qos_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_qos_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_qos_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_qos_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        if (already_exists == TRUE) {
            /* state should have been removed by Host. Stale State */
            rc = _bcm_dpp_wb_qos_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
        else {
            /* Initialize the state to know value */
            rc = _bcm_dpp_wb_qos_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }

    wb_info->init_done = TRUE;

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_wb_qos_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_ing_lif_cos_mpls_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *ing_lif_cos_mpls_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_MPLS_EXP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_MPLS_EXP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->ing_lif_cos_mpls_bitmap_off);
            ing_lif_cos_mpls_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                ing_lif_cos_mpls_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap[start_index + idx];
            }

            data = (uint8 *)&(ing_lif_cos_mpls_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_ing_lif_cos_ipv4_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *ing_lif_cos_ipv4_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->ing_lif_cos_ipv4_bitmap_off);
            ing_lif_cos_ipv4_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                ing_lif_cos_ipv4_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap[start_index + idx];
            }

            data = (uint8 *)&(ing_lif_cos_ipv4_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_qos_update_ing_lif_cos_ipv6_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *ing_lif_cos_ipv6_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->ing_lif_cos_ipv6_bitmap_off);
            ing_lif_cos_ipv6_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                ing_lif_cos_ipv6_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap[start_index + idx];
            }

            data = (uint8 *)&(ing_lif_cos_ipv6_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_qos_update_ing_lif_cos_l2_inner_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *ing_lif_cos_l2_inner_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->ing_lif_cos_l2_inner_bitmap_off);
            ing_lif_cos_l2_inner_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                ing_lif_cos_l2_inner_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap[start_index + idx];
            }

            data = (uint8 *)&(ing_lif_cos_l2_inner_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_qos_update_ing_lif_cos_l2_outer_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *ing_lif_cos_l2_outer_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->ing_lif_cos_l2_outer_bitmap_off);
            ing_lif_cos_l2_outer_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                ing_lif_cos_l2_outer_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap[start_index + idx];
            }

            data = (uint8 *)&(ing_lif_cos_l2_outer_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_qos_update_ing_lif_cos_l2_untag_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *ing_lif_cos_l2_outer_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_TC_DP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_TC_DP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->ing_lif_cos_l2_outer_bitmap_off);
            ing_lif_cos_l2_outer_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                ing_lif_cos_l2_outer_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap[start_index + idx];
            }

            data = (uint8 *)&(ing_lif_cos_l2_outer_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_ing_pcp_vlan_ctag_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *ing_pcp_vlan_ctag_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->ing_pcp_vlan_ctag_bitmap_off);
            ing_pcp_vlan_ctag_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                ing_pcp_vlan_ctag_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_ctag_bitmap[start_index + idx];
            }

            data = (uint8 *)&(ing_pcp_vlan_ctag_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_ing_pcp_vlan_utag_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *ing_pcp_vlan_utag_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->ing_pcp_vlan_utag_bitmap_off);
            ing_pcp_vlan_utag_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                ing_pcp_vlan_utag_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_utag_bitmap[start_index + idx];
            }

            data = (uint8 *)&(ing_pcp_vlan_utag_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_ing_pcp_vlan_stag_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *ing_pcp_vlan_stag_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->ing_pcp_vlan_stag_bitmap_off);
            ing_pcp_vlan_stag_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                ing_pcp_vlan_stag_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_stag_bitmap[start_index + idx];
            }

            data = (uint8 *)&(ing_pcp_vlan_stag_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_remark_encap_mpls_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_remark_encap_mpls_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_remark_encap_mpls_bitmap_off);
            egr_remark_encap_mpls_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_remark_encap_mpls_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_mpls_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_remark_encap_mpls_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_remark_encap_ipv6_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_remark_encap_ipv6_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_remark_encap_ipv6_bitmap_off);
            egr_remark_encap_ipv6_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_remark_encap_ipv6_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv6_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_remark_encap_ipv6_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_remark_encap_ipv4_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_remark_encap_ipv4_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_remark_encap_ipv4_bitmap_off);
            egr_remark_encap_ipv4_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_remark_encap_ipv4_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv4_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_remark_encap_ipv4_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_remark_encap_l2_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_remark_encap_l2_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L2_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L2_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_remark_encap_l2_bitmap_off);
            egr_remark_encap_l2_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_remark_encap_l2_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_l2_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_remark_encap_l2_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_qos_update_egr_remark_mpls_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_remark_mpls_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_remark_mpls_bitmap_off);
            egr_remark_mpls_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_remark_mpls_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_remark_mpls_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_remark_mpls_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_remark_ipv6_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_remark_ipv6_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_remark_ipv6_bitmap_off);
            egr_remark_ipv6_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_remark_ipv6_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv6_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_remark_ipv6_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_remark_ipv4_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_remark_ipv4_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_remark_ipv4_bitmap_off);
            egr_remark_ipv4_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_remark_ipv4_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv4_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_remark_ipv4_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_pcp_vlan_ctag_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_pcp_vlan_ctag_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_pcp_vlan_ctag_bitmap_off);
            egr_pcp_vlan_ctag_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_pcp_vlan_ctag_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ctag_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_pcp_vlan_ctag_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_pcp_vlan_utag_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_pcp_vlan_utag_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_pcp_vlan_utag_bitmap_off);
            egr_pcp_vlan_utag_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_pcp_vlan_utag_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_utag_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_pcp_vlan_utag_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_pcp_vlan_stag_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_pcp_vlan_stag_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*2) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*2) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_pcp_vlan_stag_bitmap_off);
            egr_pcp_vlan_stag_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_pcp_vlan_stag_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_stag_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_pcp_vlan_stag_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_pcp_vlan_mpls_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_pcp_vlan_mpls_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_EXP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_EXP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_pcp_vlan_mpls_bitmap_off);
            egr_pcp_vlan_mpls_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_pcp_vlan_mpls_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_pcp_vlan_mpls_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_pcp_vlan_ipv6_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_pcp_vlan_ipv6_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_pcp_vlan_ipv6_bitmap_off);
            egr_pcp_vlan_ipv6_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_pcp_vlan_ipv6_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_pcp_vlan_ipv6_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_pcp_vlan_ipv4_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_pcp_vlan_ipv4_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_pcp_vlan_ipv4_bitmap_off);
            egr_pcp_vlan_ipv4_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_pcp_vlan_ipv4_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_pcp_vlan_ipv4_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_l2_i_tag_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_l2_i_tag_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_l2_i_tag_bitmap_off);
            egr_l2_i_tag_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_l2_i_tag_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_l2_i_tag_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_l2_i_tag_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_88660

int
_bcm_dpp_wb_qos_update_egr_dscp_exp_marking_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_dscp_exp_marking_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_RESOLVED_DP_MAX*_BCM_QOS_MAP_TC_MAX*SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS)*(profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_RESOLVED_DP_MAX*_BCM_QOS_MAP_TC_MAX*SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS)*profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_dscp_exp_marking_bitmap_off);
            egr_dscp_exp_marking_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_dscp_exp_marking_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_dscp_exp_marking_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_dscp_exp_marking_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
#endif

int
_bcm_dpp_wb_qos_update_egr_mpls_php_ipv6_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_mpls_php_ipv6_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_mpls_php_ipv6_bitmap_off);
            egr_mpls_php_ipv6_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_mpls_php_ipv6_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv6_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_mpls_php_ipv6_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_egr_mpls_php_ipv4_bitmap_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *egr_mpls_php_ipv4_bitmap_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              idx, start_index, end_index;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:
            end_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * (profile_ndx + 1)) - 1;
            if (profile_ndx == 0) {
                start_index = 0;
            }
            else {
                start_index = SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * profile_ndx) - 1;
            }

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_mpls_php_ipv4_bitmap_off);
            egr_mpls_php_ipv4_bitmap_data = (int*)data;

            /* update framework cache and peristent storage                       */
            for (idx = 0; idx < (end_index - start_index + 1); idx++) {
                egr_mpls_php_ipv4_bitmap_data[start_index + idx] = SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap[start_index + idx];
            }

            data = (uint8 *)&(egr_mpls_php_ipv4_bitmap_data[start_index]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(SHR_BITDCL)*(end_index - start_index + 1);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_qos_update_ing_cos_opcode_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *ing_cos_opcode_flags_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->ing_cos_opcode_off);
            ing_cos_opcode_flags_data = (int*)data;

            /* update framework cache and peristent storage                       */
            ing_cos_opcode_flags_data[profile_ndx] = SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags[profile_ndx];

            data = (uint8 *)&(ing_cos_opcode_flags_data[profile_ndx]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(int);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_qos_update_ing_opcode_bmp_state(int unit, int profile_ndx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_qos_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    SHR_BITDCL                      *opcode_bmp_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_QOS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_QOS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_QOS, 0);

    wb_info = BCM_DPP_WB_QOS_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_QOS_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->opcode_bmp_off);
            opcode_bmp_data = (SHR_BITDCL*)data;

            /* update framework cache and peristent storage                       */
            opcode_bmp_data[profile_ndx/32] = SOC_DPP_STATE(unit)->qos_state->opcode_bmp[profile_ndx/32];

            data = (uint8 *)&(opcode_bmp_data[profile_ndx/32]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(int);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

