/*
 * $Id: qos.c,v 1.68 Broadcom SDK $
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
 * QoS module
 *
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_QOS

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <shared/bitop.h>
#include <shared/gport.h>
#include <shared/shr_resmgr.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_lif_cos.h>
#include <soc/dpp/PPD/ppd_api_eg_qos.h> 
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPC/ppc_api_profile_mgmt.h>

#include <shared/gport.h>
#include <shared/shr_resmgr.h>

#include <bcm/l2.h>
#include <bcm/qos.h>
#include <bcm/debug.h>
#include <bcm/error.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/wb_db_qos.h>
#include <bcm_int/dpp/wb_db_cosq.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/state.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/switch.h>





#define _BCM_ERROR_IF_OUT_OF_RANGE(val, max_limit)  \
    if (val > max_limit) {                          \
        return BCM_E_PARAM;                         \
    }                                               

#define _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_MPLS(_profile_, _exp_) \
    ((_profile_)*_BCM_QOS_MAP_ING_MPLS_EXP_MAX + (_exp_))
#define _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(_profile_, _dscp_) \
    ((_profile_)*_BCM_QOS_MAP_ING_L3_DSCP_MAX + (_dscp_))
#define _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_INNER(_profile_, _up_, _dei_) \
    ((_profile_)*_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX + ((_up_)<<1) + (_dei_))
#define _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_OUTER(_profile_, _up_, _dei_) \
    ((_profile_)*_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX + ((_up_)<<1) + (_dei_))
#define _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_UNTAG(_profile_, _tc_, _dp_) \
    ((_profile_)*_BCM_QOS_MAP_TC_DP_MAX + ((_tc_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_CTAG(_profile_, _up_) \
    ((_profile_)*_BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX + (_up_))
#define _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_UTAG(_profile_, _tc_, _dp_) \
    ((_profile_)*_BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX + ((_tc_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_STAG(_profile_, _up_, _dei_) \
    ((_profile_)*_BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX + ((_up_)<<1) + (_dei_))
#define _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_MPLS(_profile_, _exp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX + (_exp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(_profile_, _dscp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX + (_dscp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L2(_profile_, _pcp_, _dp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_REMARK_L2_MAX + ((_pcp_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_REMARK_MPLS(_profile_, _exp_, _dp_) \
    (((_profile_)*_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4) + ((_exp_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(_profile_, _dscp_, _dp_) \
    (((_profile_)*_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) + ((_dscp_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_CTAG(_profile_, _up_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_VLAN_PRI_MAX + (_up_))
#define _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_UTAG(_profile_, _tc_, _dp_) \
    (((_profile_)*_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) + ((_tc_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_STAG(_profile_, _up_, _dei_) \
    (((_profile_)*_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*2) + ((_up_)<<1) + (_dei_))        
#define _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_MPLS(_profile_, _exp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_EXP_MAX + (_exp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(_profile_, _dscp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_DSCP_MAX + (_dscp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(_profile_, _exp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX + (_exp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_L2_I_TAG(_profile_, _tc_, _dp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4 + ((_tc_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_DSCP_EXP_MARKING(_profile_, _inlif_profile, _tc_, _dp_) \
    ((_profile_)*(_BCM_QOS_MAP_EGR_RESOLVED_DP_MAX * _BCM_QOS_MAP_TC_MAX * SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS) + ((_inlif_profile)<<5)+ ((_tc_)<<2) + (_dp_))


/* returns the number of set bits in the range of _max_in_profile_*_profile_ to _max_in_profile_*(_profile_+1) for _bitmap_ */
#define _DPP_QOS_ENTRY_USED_CNT(_bitmap_, _profile_, _max_in_profile_, _count_) \
    SHR_BITCOUNT_RANGE((_bitmap_), _count_, _max_in_profile_*_profile_, _max_in_profile_);

#define _DPP_QOS_ALLOC(_obj_, _size_, _str1_, _str2_, _str3_)                               \
if (_obj_ == NULL) {                                                                        \
    alloc_size = _size_;                                                                    \
    BCMDNX_ALLOC(_obj_, alloc_size, _str1_);                                                   \
    if (_obj_ == NULL) {                                                                    \
        LOG_ERROR(BSL_LS_BCM_QOS, \
                  (BSL_META_U(unit, \
                              "%s: %s, unit %d\n"), FUNCTION_NAME(), _str2_, unit));   \
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG(_str3_)));                          \
    }                                                                                       \
    sal_memset(_obj_,0,alloc_size);                                                         \
}

bcm_dpp_qos_state_t *qos_state[SOC_MAX_NUM_DEVICES];

#define _BCM_DPP_QOS_DEFAULT_EGR_PCP_DEI_PROFILE (0)

#define _BCM_DPP_QOS_PWE_P2P_MAX_PROFILE  (15)

/* Initialize the QoS module. */
int 
bcm_petra_qos_init(int unit)
{
    int rv = BCM_E_NONE, alloc_size, profile_ndx;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    if (qos_state[unit] == NULL) {
        BCMDNX_ALLOC(qos_state[unit], sizeof(bcm_dpp_qos_state_t), "QOS Info");
        if (qos_state[unit] == NULL) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Error allocating QOS_state, unit %d\n"), FUNCTION_NAME(), unit));
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate QOS state")));
        }
        sal_memset(qos_state[unit], 0, sizeof(bcm_dpp_qos_state_t));
        SOC_DPP_STATE(unit)->qos_state = qos_state[unit];

    /* ing_lif_cos */
    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos), 
        "LIF COS MPLS Bitmap", 
        "Error allocating LIF COS MPLS Bitmap", 
        "Unable to allocate LIF COS MPLS Bitmap");
    
    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos), 
        "LIF COS Ipv4 Bitmap", 
        "Error allocating LIF COS Ipv4 Bitmap", 
        "Unable to allocate LIF COS Ipv4 Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos), 
        "LIF COS Ipv6 Bitmap", 
        "Error allocating LIF COS Ipv6 Bitmap", 
        "Unable to allocate LIF COS Ipv6 Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos), 
        "LIF COS L2 Inner Bitmap", 
        "Error allocating LIF COS L2 Inner Bitmap", 
        "Unable to allocate LIF COS L2 Inner Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos), 
        "LIF COS L2 Outer Bitmap", 
        "Error allocating LIF COS L2 Outer Bitmap", 
        "Unable to allocate LIF COS L2 Outer Bitmap");
    
    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_TC_DP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos), 
        "LIF COS L2 Untag Bitmap", 
        "Error allocating LIF COS L2 Untag Bitmap", 
        "Unable to allocate LIF COS L2 Untag Bitmap");

    /* ing_pcp_vlan */
    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_ctag_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan), 
        "ING PCP Ctag Bitmap", 
        "Error allocating ING PCP Ctag Bitmap", 
        "Unable to allocate ING PCP Ctag Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_utag_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan), 
        "ING PCP Utag Bitmap", 
        "Error allocating ING PCP Utag Bitmap", 
        "Unable to allocate ING PCP Utag Bitmap");
    
    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_stag_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan), 
        "ING PCP Stag Bitmap", 
        "Error allocating ING PCP Stag Bitmap", 
        "Unable to allocate ING PCP Stag Bitmap");

    /* egr_remark */
    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_mpls_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id), 
        "EGR REMARK ENCAP MPLS Bitmap", 
        "Error allocating EGR REMARK ENCAP MPLS Bitmap", 
        "Unable to allocate EGR REMARK ENCAP MPLS Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv6_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id), 
        "EGR REMARK ENCAP IPV6 Bitmap", 
        "Error allocating EGR REMARK ENCAP IPV6 Bitmap", 
        "Unable to allocate EGR REMARK ENCAP IPV6 Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv4_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id), 
        "EGR REMARK ENCAP IPV6 Bitmap", 
        "Error allocating EGR REMARK ENCAP IPV6 Bitmap", 
        "Unable to allocate EGR REMARK ENCAP IPV6 Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_l2_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L2_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id), 
        "EGR REMARK ENCAP L2 Bitmap", 
        "Error allocating EGR REMARK ENCAP L2 Bitmap", 
        "Unable to allocate EGR REMARK ENCAP L2 Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_remark_mpls_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id), 
        "EGR REMARK MPLS Bitmap", 
        "Error allocating EGR REMARK MPLS Bitmap", 
        "Unable to allocate EGR REMARK MPLS Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv6_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id), 
        "EGR REMARK IPV6 Bitmap", 
        "Error allocating EGR REMARK IPV6 Bitmap", 
        "Unable to allocate EGR REMARK IPV6 Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv4_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id), 
        "EGR REMARK IPV4 Bitmap", 
        "Error allocating EGR REMARK IPV4 Bitmap", 
        "Unable to allocate EGR REMARK IPV4 Bitmap");  

    /* egr_mpls_php */
    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv6_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php), 
        "EGR MPLS PHP IPV6 Bitmap", 
        "Error allocating EGR MPLS PHP IPV6 Bitmap", 
        "Unable to allocate EGR MPLS PHP IPV6 Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php), 
        "EGR MPLS PHP IPV4 Bitmap", 
        "Error allocating EGR MPLS PHP IPV4 Bitmap", 
        "Unable to allocate EGR MPLS PHP IPV4 Bitmap");
    
    /* egr_pcp_vlan */
    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ctag_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan), 
        "EGR PCP VLAN Ctag Bitmap", 
        "Error allocating EGR PCP VLAN Ctag Bitmap", 
        "Unable to allocate EGR PCP VLAN Ctag Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_utag_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan), 
        "EGR PCP VLAN Utag Bitmap", 
        "Error allocating EGR PCP VLAN Utag Bitmap", 
        "Unable to allocate EGR PCP VLAN Utag Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_stag_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*2) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan), 
        "EGR PCP VLAN Stag Bitmap", 
        "Error allocating EGR PCP VLAN Stag Bitmap", 
        "Unable to allocate EGR PCP VLAN Stag Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_EXP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan), 
        "EGR PCP VLAN MPLS Bitmap", 
        "Error allocating EGR PCP VLAN MPLS Bitmap", 
        "Unable to allocate EGR PCP VLAN MPLS Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan), 
        "EGR PCP VLAN IPv6 Bitmap", 
        "Error allocating EGR PCP VLAN IPv6 Bitmap", 
        "Unable to allocate EGR PCP VLAN IPv6 Bitmap");

    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_DSCP_MAX) * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan), 
        "EGR PCP VLAN IPv4 Bitmap", 
        "Error allocating EGR PCP VLAN IPv4 Bitmap", 
        "Unable to allocate EGR PCP VLAN IPv4 Bitmap");
    
    /* egr_l2_i_tag */
    _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_l2_i_tag_bitmap, 
        SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) * SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag), 
        "EGR L2 I Tag Bitmap", 
        "Error allocating EGR L2 I Tag Bitmap", 
        "Unable to allocate EGR L2 I Tag Bitmap");

#ifdef BCM_88660

    if (SOC_IS_ARADPLUS(unit)) {
        int simple_mode = soc_property_get(unit, spn_BCM886XX_QOS_L3_L2_MARKING, 0);

        /* egr_dscp_exp_marking */
        _DPP_QOS_ALLOC(SOC_DPP_STATE(unit)->qos_state->egr_dscp_exp_marking_bitmap, 
            SHR_BITALLOCSIZE((_BCM_QOS_MAP_EGR_RESOLVED_DP_MAX * _BCM_QOS_MAP_TC_MAX * SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS) * 
            SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking), 
            "EGR DSCP EXP MARKING Bitmap", 
            "Error allocating EGR DSCP EXP MARKING Bitmap", 
            "Unable to allocate EGR DSCP EXP MARKING Bitmap");

        if (!SOC_WARM_BOOT(unit)) {
            /* If we are in simple mode, then there is a DSCP marking bit in the inlif profile. */
            /* For all inlif profiles with this bit set, we turn DSCP marking on. */
            if (simple_mode) {
                uint32 mask = 0;
                uint32 inlif_profile_idx;
                uint32 bitmap = 0;
                uint32 soc_sand_rv = 0;
                SOC_PPC_EG_QOS_GLOBAL_INFO info;

                /* Get the mask of the relevant inlif profile */
                rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_profile_mgmt_if_profile_val_set,(unit, SOC_PPC_PROFILE_MGMT_TYPE_INLIF, SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_SIMPLE_DSCP_MARKING, /*value*/1, &mask));
                BCMDNX_IF_ERR_EXIT(rv);

                /* Setup the bitmap */
                for (inlif_profile_idx = 0; inlif_profile_idx < SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_idx++) {
                
                    if ((inlif_profile_idx & mask) == 0) {
                        continue;
                    }
                    bitmap |= (1 << inlif_profile_idx);
                }

                SOC_PPC_EG_QOS_GLOBAL_INFO_clear(&info);
                soc_sand_rv = soc_ppd_eg_qos_global_info_get(unit, &info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                info.in_lif_profile_bitmap = bitmap;
                soc_sand_rv = soc_ppd_eg_qos_global_info_set(unit, &info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);        
            }
        }
    }


#endif /* BCM_88660 */
    if (SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags == NULL) {
        alloc_size = sizeof(int) * SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode;
        BCMDNX_ALLOC(SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags,
                  alloc_size, "COS Opcode Flags");
        if (SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags == NULL) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Error allocating COS Opcode Flags, unit %d\n"), FUNCTION_NAME(), unit));
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate COS Opcode Flags")));
        }
        sal_memset(SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags,0,alloc_size);
    }

    if (SOC_DPP_STATE(unit)->qos_state->opcode_bmp == NULL) {
        alloc_size = SHR_BITALLOCSIZE(SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode);
        BCMDNX_ALLOC(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, alloc_size, "COS Opcode Bmp");
        if (SOC_DPP_STATE(unit)->qos_state->opcode_bmp == NULL) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Error allocating COS OPCODE Bitmap, unit %d\n"), FUNCTION_NAME(), unit));
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate COS Opcode Bitmap")));
        }
        sal_memset(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, 0, alloc_size); 
    }
    }
    SOC_DPP_STATE(unit)->qos_state->init = TRUE;

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_qos_state_init(unit);
    if (rv != BCM_E_NONE) {
      LOG_ERROR(BSL_LS_BCM_QOS,
                (BSL_META_U(unit,
                            "%s: Unit %d, failed warmboot init, error 0x%x\n"),FUNCTION_NAME(), unit, rv));
      BCMDNX_IF_ERR_EXIT(rv);
    }
#endif

    if (!SOC_WARM_BOOT(unit)) {
        if (SOC_IS_ARAD(unit)) {
            /* 
             * Allocate from advance QOS Egress PCP-DEI MAP profile 0.
             * PCP-DEP Map profile 0 is used for the default egress VLAN editing (in case of untagged packets).
             * In order for user to use QOS Egress PCP-DEI Map profile 0 destroy and create it again.
             */
            profile_ndx = _BCM_DPP_QOS_DEFAULT_EGR_PCP_DEI_PROFILE;
            rv = bcm_dpp_am_qos_egr_pcp_vlan_alloc(unit, SHR_RES_ALLOC_WITH_ID, &profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);        
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/* Detach the QoS module. */
int 
bcm_petra_qos_detach(int unit)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* this unit not init, no nothing to detach; done. */
    if (!qos_state[unit]) {
        BCM_EXIT;
    }

    /* ing_lif_cos */
    if (SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap);
    } 

    if (SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap);
    }

    /* ing_pcp_vlan */
    if (SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_ctag_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_ctag_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_utag_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_utag_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_stag_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_stag_bitmap);
    }

    /* egr_remark */
    if (SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_mpls_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_mpls_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv6_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv6_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv4_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv4_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_l2_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_l2_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_remark_mpls_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_remark_mpls_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv6_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv6_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv4_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv4_bitmap);
    }

    /* egr_mpls_php */
    if (SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv6_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv6_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap);
    }

    /* egr_pcp_vlan */
    if (SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ctag_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ctag_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_utag_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_utag_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_stag_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_stag_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap);
    }

    if (SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap);
    }

    /* egr_l2_i_tag */
    if (SOC_DPP_STATE(unit)->qos_state->egr_l2_i_tag_bitmap) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_l2_i_tag_bitmap);
    }

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
        /* egr_dscp_exp_marking */
        if (SOC_DPP_STATE(unit)->qos_state->egr_dscp_exp_marking_bitmap) {
            BCM_FREE(SOC_DPP_STATE(unit)->qos_state->egr_dscp_exp_marking_bitmap);
        }
    }
#endif /* BCM_88660 */

    if (SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags);
    }

    if (SOC_DPP_STATE(unit)->qos_state->opcode_bmp) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state->opcode_bmp);
    }

    if (SOC_DPP_STATE(unit)->qos_state) {
        BCM_FREE(SOC_DPP_STATE(unit)->qos_state);
        qos_state[unit] = NULL;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* tear down warm boot state */
    rv = _bcm_dpp_wb_qos_state_deinit(unit);
#endif /* BCM_WARM_BOOT_SUPPORT */


    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/* Create a QoS map profile */
int 
bcm_petra_qos_map_create(int unit, uint32 flags, int *map_id)
{
    int rv = BCM_E_NONE;
    int am_flags = 0;
    int profile_ndx = BCM_QOS_MAP_PROFILE_GET(*map_id);
   
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    if (flags & BCM_QOS_MAP_WITH_ID) {
        am_flags |= SHR_RES_ALLOC_WITH_ID;
    }

    if (flags & BCM_QOS_MAP_INGRESS) { 

        if (flags & BCM_QOS_MAP_L2_VLAN_PCP) {
            rv = bcm_dpp_am_qos_ing_pcp_vlan_alloc(unit, am_flags, &profile_ndx);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: Out of Ingress PCP Profile resources\n"),FUNCTION_NAME()));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(*map_id, profile_ndx);
        } 
        else if (flags & BCM_QOS_MAP_MPLS_ELSP) {
            rv = bcm_dpp_am_qos_ing_elsp_alloc(unit, am_flags, &profile_ndx);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: Out of MPLS ELSP Profile resources\n"),FUNCTION_NAME()));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            BCM_INT_QOS_MAP_MPLS_ELSP_SET(*map_id, profile_ndx);
        } else if (flags & BCM_QOS_MAP_OPCODE) {
            rv = bcm_dpp_am_qos_ing_cos_opcode_alloc(unit, am_flags, &profile_ndx);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: Out of COS OPCODE Profile resources\n"),FUNCTION_NAME()));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            BCM_INT_QOS_MAP_TYPE_OPCODE_SET(*map_id, profile_ndx);
        }
        else { /* L3 */
            rv = bcm_dpp_am_qos_ing_lif_cos_alloc(unit, am_flags, &profile_ndx);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: Out of Ingress Lif/Cos resources\n"),FUNCTION_NAME()));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            BCM_INT_QOS_MAP_LIF_COS_SET(*map_id, profile_ndx);
        }
    } 
    else if (flags & BCM_QOS_MAP_EGRESS) {

         if (flags & BCM_QOS_MAP_L2_VLAN_PCP) {
            if (SOC_IS_ARAD(unit) && (am_flags & SHR_RES_ALLOC_WITH_ID) && 
                profile_ndx == _BCM_DPP_QOS_DEFAULT_EGR_PCP_DEI_PROFILE) {
                /* 
                 * No need to go to Alloc since we allocate it from advance.
                 * We dont verify existance of Profile 0 just returning this profile
                 */
            } else {
                rv = bcm_dpp_am_qos_egr_pcp_vlan_alloc(unit, am_flags, &profile_ndx);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: Out of Egress PCP Profile resources\n"),FUNCTION_NAME()));
                    BCMDNX_IF_ERR_EXIT(rv);
                }                
            }
            BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(*map_id, profile_ndx);  
         } 
         else if (flags & BCM_QOS_MAP_MIM_ITAG) {
             rv = bcm_dpp_am_qos_egr_l2_i_tag_alloc(unit, am_flags, &profile_ndx);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Out of Egress PCP Profile resources\n"),FUNCTION_NAME()));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
             BCM_INT_QOS_MAP_EGRESS_L2_I_TAG_SET(*map_id, profile_ndx);  
         } 
         else if (flags & BCM_QOS_MAP_MPLS_PHP) {
             rv = bcm_dpp_am_qos_egr_mpls_php_id_alloc(unit, am_flags, &profile_ndx);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Out of Egress MPLS PHP Profile resources\n"),FUNCTION_NAME()));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
             BCM_INT_QOS_MAP_EGRESS_MPLS_PHP_SET(*map_id, profile_ndx);  
         }
#ifdef BCM_88660
         else if ((flags & BCM_QOS_MAP_L3_L2) && SOC_IS_ARADPLUS(unit)) {
             /* Create a QOS marking profile (each PP port may have a marking profile, */
             /* and each marking profile (along with TC/DP and InLIF) */
             /* determines the TOS/EXP marking at egress).*/
             rv = bcm_dpp_am_qos_egr_dscp_exp_marking_alloc(unit, am_flags, &profile_ndx);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Out of Egress DSCP/EXP Marking Profile resources\n"),FUNCTION_NAME()));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
             BCM_INT_QOS_MAP_EGRESS_DSCP_EXP_MARKING_SET(*map_id, profile_ndx);  

         }
#endif
         else {
             rv = bcm_dpp_am_qos_egr_remark_id_alloc(unit,am_flags, &profile_ndx);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Out of Egress Remark ID resources\n"),FUNCTION_NAME()));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
             BCM_INT_QOS_MAP_REMARK_SET(*map_id, profile_ndx);           
         }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_qos_map_add_pcp_vlan_params_verify(int unit, uint32 flags, bcm_qos_map_t *map, int map_id){
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (flags & BCM_QOS_MAP_MIM_ITAG) { 
      _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_PCP_MAX);/*int_pri = PCP */
    } else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_UP_MAX);/*int_pri = UP */ 
    } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_TC_MAX);/*int_pri = TC */ 
    } else if (flags & BCM_QOS_MAP_L2_OUTER_TAG) {  /* OUTER default L2 */
        _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_PCP_MAX);/*int_pri = PCP */
    } else  if (flags & BCM_QOS_MAP_MPLS) {
        if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)){
            _BCM_ERROR_IF_OUT_OF_RANGE(map->exp, _BCM_QOS_MAP_EXP_MAX);
        } else { /* valid only for egress editing */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map_id should for egress PCP VLAN"))); 
        }
    } else if (flags & BCM_QOS_MAP_L3) {
        if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)){
            _BCM_ERROR_IF_OUT_OF_RANGE(map->dscp, _BCM_QOS_MAP_DSCP_MAX);
        } else { /* valid only for egress editing */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map_id should for egress PCP VLAN"))); 
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("one of the following flags must be set: BCM_QOS_MAP_MIM_ITAG | BCM_QOS_MAP_L2_INNER_TAG | "
                                                "BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_MPLS | BCM_QOS_MAP_L3")));
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_qos_map_add_params_verify(int unit, uint32 flags, bcm_qos_map_t *map, int map_id){
    int rv = BCM_E_NONE;
    int dp = 0;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_ERROR_IF_OUT_OF_RANGE(map->pkt_pri, BCM_PRIO_MAX);
    _BCM_ERROR_IF_OUT_OF_RANGE(map->pkt_cfi, _BCM_QOS_MAP_PRI_CFI_MAX); 
    _BCM_ERROR_IF_OUT_OF_RANGE(map->exp,  _BCM_QOS_MAP_EXP_MAX); 
    _BCM_ERROR_IF_OUT_OF_RANGE(map->dscp, _BCM_QOS_MAP_DSCP_MAX); 
 
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if (BCM_INT_QOS_MAP_IS_INGRESS(map_id)) {
        if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)){
            _bcm_petra_qos_map_add_pcp_vlan_params_verify(unit, flags, map, map_id);
        } 
        else if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
            _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_TC_MAX); /*int_pri = TC */

            if ((BCM_QOS_MAP_PROFILE_GET(map_id) > 0xf) && (map->remark_int_pri != 0)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot configure remark_int_pri for map_id > 15")));
            }
            if (flags & BCM_QOS_MAP_L3) {
                _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX); 
            }
            else if (flags & BCM_QOS_MAP_MPLS) {
                _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX); 
            }
        } 
        else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {
            _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_EXP_MAX); /*int_pri = EXP */
        } 
        else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {
            _BCM_ERROR_IF_OUT_OF_RANGE(map->port_offset, _BCM_QOS_MAP_PORT_OFFSET_MAX); 
        } 
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map_id must be either PCP_VLAN, LIF_COS, MPLS_ELSP or OPCODE")));
        } 
    }
    else if (BCM_INT_QOS_MAP_IS_EGRESS(map_id)) {
        if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)){
            _bcm_petra_qos_map_add_pcp_vlan_params_verify(unit, flags, map, map_id);
        } 
        else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {
            _bcm_petra_qos_map_add_pcp_vlan_params_verify(unit, flags, map, map_id);
        }
        else if (BCM_INT_QOS_MAP_IS_REMARK(map_id)){
            if (flags & BCM_QOS_MAP_ENCAP) { /*in_dscp_exp = map->remark_int_pri, out_dscp = map->dscp, out_exp = map->exp */
                if (flags & BCM_QOS_MAP_MPLS) {
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_EXP_MAX); /* remark_int_pri = in_EXP */    
                } else if (flags & BCM_QOS_MAP_L3) {
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_DSCP_MAX); /* remark_int_pri = in_DSCP */
                } else if (flags & BCM_QOS_MAP_L2) {
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_EXP_MAX); /* remark_int_pri = in_EXP */ 
                } else {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("when map_id is REMARK and flag BCM_QOS_MAP_ENCAP is set, flag BCM_QOS_MAP_MPLS "
                                                          "or BCM_QOS_MAP_L3 or BCM_QOS_MAP_L2 must be set as well")));
                }
            }
            else { /* Not encap header*/
                if (flags & BCM_QOS_MAP_MPLS) { /*in_dscp_exp = int_pri, out_dscp_exp = remark_int_pri*/
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_EXP_MAX);        /*int_pri = in_EXP */
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_EXP_MAX); /*remark_int_pri = out_EXP */ 
                } else if (flags & BCM_QOS_MAP_L3) {
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_DSCP_MAX);        /*int_pri = in_DSCP */
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_DSCP_MAX); /*remark_int_pri = out_DSCP */ 
                }
            }
        }
        else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)){
          _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_EXP_MAX);        /*int_pri = in_EXP */
          _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_DSCP_MAX); /*remark_int_pri = out_DSCP */               
          if (!((flags & BCM_QOS_MAP_IPV6) || (flags & BCM_QOS_MAP_IPV4))) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("when map_id is MPLS PHP one of the following flags are supported BCM_QOS_MAP_IPV6, BCM_QOS_MAP_L3.")));
          }
        }
#ifdef BCM_88660
        else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)){
          _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_TC_MAX);        /*int_pri = Egress-TC */
          _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_color, _BCM_QOS_MAP_EGR_RESOLVED_DP_MAX); /*remark_int_pri = resolved-dp */               
          _BCM_ERROR_IF_OUT_OF_RANGE(map->port_offset, SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS); /*remark_int_pri = inlif profile */               
          _BCM_ERROR_IF_OUT_OF_RANGE(map->dscp, _BCM_QOS_MAP_DSCP_MAX);
          _BCM_ERROR_IF_OUT_OF_RANGE(map->exp, _BCM_QOS_MAP_EXP_MAX);

          if (flags) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No flags are supported for DSCP/EXP marking.")));
          }
        }
#endif /* BCM_88660 */
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map_id must be either PCP_VLAN, L2_I_TAG or REMARK")));
        }
    }
    else { 
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map_id must be either INGRESS or EGRESS")));
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/* Destroy a QoS map profile */
int 
bcm_petra_qos_map_destroy(int unit, int map_id)
{
    int sand_rv, rv = BCM_E_NONE;
    int profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    bcm_qos_map_t qos_map;
    int idx = 0, flags = 0;
    int int_pri = 0, int_cfi = 0, color = 0;
    int dscp_cnt = 0, tc_cnt = 0, dp_cnt = 0, pri_cfi_cnt = 0;
    int remark_int_pri = 0;
    int in_dscp = 0, in_exp = 0;
    int dev_id, dp = 0;
#ifdef BCM_88660
    int resolved_dp_idx, tc_idx, inlif_idx;
#endif /* BCM_88660 */
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type;
    bcm_color_t tmp_color;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    dev_id = (unit);
    if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)) {
    
    /* Destroy all STAG */
    bcm_qos_map_t_init(&qos_map);
    for (idx=0; idx < _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX; idx++) {
        qos_map.int_pri = (idx >> 1);
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, (idx & 1), &(qos_map.color)));
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_OUTER_TAG;
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Vlan PCP Outer Tag Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), (idx>>1), (idx & 1), profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    /* Destroy all CTAG */
    bcm_qos_map_t_init(&qos_map);
    for (idx=0; idx < _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX; idx++) {
        qos_map.int_pri = idx;
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_INNER_TAG;
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Vlan PCP Inner Tag Pri %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    /* Destroy all UNTAGGED */
    bcm_qos_map_t_init(&qos_map);
    for (idx=0; idx < _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX; idx++) {
        qos_map.int_pri = (idx >> 2);
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, (idx % 4), &(qos_map.color)));
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_UNTAGGED;
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Vlan PCP Untagged Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), (idx>>2), (idx % 4), profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    rv = bcm_dpp_am_qos_ing_pcp_vlan_dealloc(unit, 0, profile_ndx);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: PCP Profile  Resource %d not allocated\n"),FUNCTION_NAME(), profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    } else if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
    
    /* Destroy all MPLS */
    bcm_qos_map_t_init(&qos_map);
    for (idx = 0; idx < _BCM_QOS_MAP_ING_MPLS_EXP_MAX; idx++) {
        qos_map.exp = idx;
        rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_MPLS, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif/Cos MPLS %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    /* Destroy all L3 */
    bcm_qos_map_t_init(&qos_map);
    for (idx = 0; idx < _BCM_QOS_MAP_ING_L3_DSCP_MAX; idx++) {
        qos_map.dscp = idx;
        rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif/Cos L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV6, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif/Cos L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* Destroy all L2 */
    bcm_qos_map_t_init(&qos_map);
    for (idx = 0; idx < _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; idx++) {
        qos_map.pkt_cfi = idx & 1;
        qos_map.pkt_pri = idx >> 1;
        
        rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_OUTER_TAG, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif/Cos L2 OUTER TAG %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_INNER_TAG, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif/Cos L2 INNER TAG %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* Destroy all UNTAGGED */
    bcm_qos_map_t_init(&qos_map);
    for (idx = 0; idx < _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; idx++) {
        qos_map.pkt_pri = idx >> 1;
        for (dp=0; dp < SOC_DPP_CONFIG(unit)->qos.dp_max; dp++) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp, &tmp_color));
        qos_map.pkt_cfi = tmp_color;
        flags = BCM_QOS_MAP_L2_UNTAGGED;
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif/Cos L2 UNTAGGED Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), idx>>1, dp, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
		}
    }
        
    rv = bcm_dpp_am_qos_ing_lif_cos_dealloc(unit, 0,  profile_ndx);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif/Cos Resources %d not allocated\n"),FUNCTION_NAME(), profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    } else if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)) {

    /* Destroy all STAG */
    bcm_qos_map_t_init(&qos_map);
    for (int_pri=0; int_pri < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri++) {
        qos_map.int_pri = int_pri;
        for (int_cfi=0; int_cfi < _BCM_QOS_MAP_EGR_VLAN_CFI_MAX; int_cfi++) {
        qos_map.color = int_cfi;
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_OUTER_TAG;
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Egress Vlan PCP Outer Tag PCP %d DEI %d reset %d failed\n"),FUNCTION_NAME(), int_pri, dp, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        }
    }
    /* Destroy all CTAG */
    bcm_qos_map_t_init(&qos_map);
    for (int_pri=0; int_pri < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri++) {
        qos_map.int_pri = int_pri;
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_INNER_TAG;
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Egress Vlan PCP Inner Tag Pri %d reset %d failed\n"),FUNCTION_NAME(), int_pri, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    /* Destroy all UNTAGGED */
    bcm_qos_map_t_init(&qos_map);
    for (int_pri=0; int_pri < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri++) {
        qos_map.int_pri = int_pri;
        for (dp=0; dp < SOC_DPP_CONFIG(unit)->qos.dp_max; dp++) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp, &(qos_map.color)));
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_UNTAGGED;
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Vlan PCP Untagged Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), int_pri, dp, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        }
    }
    
    if ( profile_ndx == SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id ) {
        /* Destroy all L3 IPV4 */ 
        bcm_qos_map_t_init(&qos_map);
        for (in_dscp=0; in_dscp < _BCM_QOS_MAP_EGR_DSCP_MAX; in_dscp++) {
        qos_map.dscp = in_dscp;
        flags = BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4; 
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Vlan PCP Untagged Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), int_pri, dp, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        }
        
        /* Destroy all L3 IPV6 */ 
        bcm_qos_map_t_init(&qos_map);
        for (in_dscp=0; in_dscp < _BCM_QOS_MAP_EGR_DSCP_MAX; in_dscp++) {
        qos_map.dscp = in_dscp;
        flags = BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6; 
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Vlan PCP Untagged Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), int_pri, dp, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        }
        
        /* Destroy all MPLS */ 
        bcm_qos_map_t_init(&qos_map);
        for (in_exp=0; in_exp < _BCM_QOS_MAP_EGR_EXP_MAX; in_exp++) {
        qos_map.exp = in_exp;
        flags = BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_MPLS;
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Vlan PCP Untagged Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), int_pri, dp, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        }
        
    }
    rv = bcm_dpp_am_qos_egr_pcp_vlan_dealloc(unit, 0, profile_ndx);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: PCP Profile Resource %d not allocated\n"),FUNCTION_NAME(),profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    } else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {

        bcm_qos_map_t_init(&qos_map);
        for (int_pri=0; int_pri < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri++) {
            qos_map.int_pri = int_pri;
            for (dp=0; dp < SOC_DPP_CONFIG(unit)->qos.dp_max; dp++) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp, &(qos_map.color)));
            flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_MIM_ITAG;
            rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: Egress L2 I TAG PCP %d DEI %d reset %d failed\n"),FUNCTION_NAME(), int_pri, dp, profile_ndx));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            }
        }
        rv = bcm_dpp_am_qos_egr_l2_i_tag_dealloc(unit, 0, profile_ndx);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Egress L2 I TAG %d not allocated\n"),FUNCTION_NAME(),profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
#ifdef BCM_88660
    } else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)) {

        if (bcm_dpp_am_qos_egr_dscp_exp_marking_is_alloced(unit, profile_ndx) != BCM_E_EXISTS)
        {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Egress DSCP/EXP marking profile %d not allocated\n"),FUNCTION_NAME(),profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        bcm_qos_map_t_init(&qos_map);

        /* Reset all lines with profile profile_ndx */
        for (resolved_dp_idx = 0; resolved_dp_idx < _BCM_QOS_MAP_EGR_RESOLVED_DP_MAX; resolved_dp_idx++) {
            for (tc_idx = 0; tc_idx < _BCM_QOS_MAP_TC_MAX; tc_idx++) {
                for (inlif_idx = 0; inlif_idx < SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_idx++) {
                    qos_map.int_pri = tc_idx;
                    qos_map.remark_color = resolved_dp_idx;
                    qos_map.port_offset = inlif_idx;
                    rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
                    if (rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_QOS,
                                  (BSL_META_U(unit,
                                              "%s: Egress DSCP/EXP marking profile %d with tc %d, resolved_dp %d, inlif %d reset failed\n"),FUNCTION_NAME(), profile_ndx, tc_idx, resolved_dp_idx, inlif_idx));
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
            }
        }

        rv = bcm_dpp_am_qos_egr_dscp_exp_marking_dealloc(unit, 0, profile_ndx);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Egress DSCP/EXP marking profile %d not allocated\n"),FUNCTION_NAME(),profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }

#endif /* BCM_88660 */
    } else if (BCM_INT_QOS_MAP_IS_REMARK(map_id)) {
        if (BCM_INT_QOS_MAP_IS_ENCAP(map_id)) {

           /* Destroy all MPLS */
           for (remark_int_pri = 0; remark_int_pri < _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX; remark_int_pri++) { 
             bcm_qos_map_t_init(&qos_map);
             qos_map.remark_int_pri = remark_int_pri;
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_MPLS, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark MPLS %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
           }
           
           /* Destroy all L3 IPV6 */ 
           for (remark_int_pri = 0; remark_int_pri < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; remark_int_pri++) { 
             bcm_qos_map_t_init(&qos_map);
             qos_map.remark_int_pri = remark_int_pri;
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark L3 IPV6 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
           }
           /* Destroy all L3 IPV4 */ 
           for (remark_int_pri = 0; remark_int_pri < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; remark_int_pri++) { 
             bcm_qos_map_t_init(&qos_map);
             qos_map.remark_int_pri = remark_int_pri;
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
           }
         
      
        } else { /* Not encap header */
            /* Destroy all MPLS */
           for (color = 0; color < SOC_DPP_CONFIG(unit)->qos.dp_max; color++) {
               for (int_pri = 0; int_pri < _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX; int_pri++) {
             bcm_qos_map_t_init(&qos_map);
             qos_map.color = color;
             qos_map.int_pri = int_pri;
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_MPLS, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark MPLS %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
               }
           }

           /* Destroy all L3 */ 
           for (color = 0; color < SOC_DPP_CONFIG(unit)->qos.dp_max; color++) {
               for (int_pri = 0; int_pri < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; int_pri++) {
             bcm_qos_map_t_init(&qos_map);
             qos_map.color = color;
             qos_map.int_pri = int_pri;
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV6, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark L3 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
               }
           }

           /* Destroy all L3 IPV4 */ 
           for (color = 0; color < SOC_DPP_CONFIG(unit)->qos.dp_max; color++) {
               for (int_pri = 0; int_pri < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; int_pri++) {
             bcm_qos_map_t_init(&qos_map);
             qos_map.color = color;
             qos_map.int_pri = int_pri;
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L3, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark L3 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
               }
           }
        }
        rv = bcm_dpp_am_qos_egr_remark_id_dealloc(unit,0, profile_ndx);
        if (rv != BCM_E_NONE) {
          LOG_ERROR(BSL_LS_BCM_QOS,
                    (BSL_META_U(unit,
                                "%s: Remark Profile Resource %d not allocated\n"),FUNCTION_NAME(),profile_ndx));
          BCMDNX_IF_ERR_EXIT(rv);
        }
    } else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {

    /* Destroy all MPLS */
    bcm_qos_map_t_init(&qos_map);
    for (idx = 0; idx < _BCM_QOS_MAP_ING_MPLS_EXP_MAX; idx++) {
        qos_map.exp = idx;
        rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_MPLS_ELSP, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: MPLS ELSP %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    rv = bcm_dpp_am_qos_ing_elsp_dealloc(unit, 0,  profile_ndx);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: MPLS ELSP Resources %d not allocated\n"),FUNCTION_NAME(), profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    } else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)) {
        /* Destroy all MPLS */
        bcm_qos_map_t_init(&qos_map);
        for (idx = 0; idx < _BCM_QOS_MAP_EGR_EXP_MAX; idx++) {
            qos_map.int_pri = idx;
            rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_IPV4, &qos_map, map_id);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: EGRESS MPLS PHP %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_IPV6, &qos_map, map_id);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: EGRESS MPLS PHP %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        rv = bcm_dpp_am_qos_egr_mpls_php_id_dealloc(unit, 0,  profile_ndx);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: MPLS MPLS PHP Resources %d not allocated\n"),FUNCTION_NAME(), profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {

        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, profile_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);

        if (opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L3) {
            /* Destroy all IPV4 */
            for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX; dscp_cnt++) {
                bcm_qos_map_t_init(&qos_map);
                qos_map.dscp = dscp_cnt;
                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_IPV4, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                /* Destroy all IPV6 */
                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_IPV6, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                /* Destroy all TC_DP associated with profile_ndx */
                for (tc_cnt = 0; tc_cnt < _BCM_QOS_MAP_TC_MAX; tc_cnt++) {
                    for (dp_cnt = 0; dp_cnt < 4; dp_cnt++) {
                        bcm_qos_map_t_init(&qos_map);
                        qos_map.int_pri = tc_cnt;
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(qos_map.color)));
                        rv = bcm_petra_qos_map_delete(unit, 0, &qos_map, map_id);
                        if (rv != BCM_E_NONE) {
                            LOG_ERROR(BSL_LS_BCM_QOS,
                                      (BSL_META_U(unit,
                                                  "%s: L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    }
                }
            }
        }
        else if (opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L2) {   /* L2 */
            for (pri_cfi_cnt = 0; pri_cfi_cnt <  _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; pri_cfi_cnt++) {
                bcm_qos_map_t_init(&qos_map);
                qos_map.int_pri = pri_cfi_cnt >> 1;
                qos_map.pkt_cfi = pri_cfi_cnt & 1;
                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: L2|L2_OUTER_TAG %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_INNER_TAG, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: L2|L2_INNER_TAG %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_UNTAGGED, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: L2|L2_UNTAGGED %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        } 
        else if (opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP) { /* TC/DP */
            /* Destroy all TC_DP associated with profile_ndx */
            for (tc_cnt = 0; tc_cnt < _BCM_QOS_MAP_TC_MAX; dscp_cnt++) {
                for (dp_cnt = 0; dp_cnt < 4; dp_cnt++) {
                    bcm_qos_map_t_init(&qos_map);
                    qos_map.int_pri = tc_cnt;
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(qos_map.color)));
                    rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2_UNTAGGED, &qos_map, map_id);
                    if (rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_QOS,
                                  (BSL_META_U(unit,
                                              "%s: L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
            }
        } 
          
        /* Clear bit associated with opcode index */
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, profile_ndx);

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_ing_opcode_bmp_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


/* Add an entry to a QoS map */

STATIC int 
_bcm_petra_qos_map_ingress_elsp_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0;
    uint32 profile_ndx = 0;
    SOC_PPD_FRWRD_ILM_GLBL_INFO glbl_info;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_elsp) {
    
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid MPLS ELSP profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS ELSP profile")));
    }
    
    SOC_PPD_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);
    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_get(dev_id, &glbl_info));

    glbl_info.elsp_info.exp_map_tbl[map->exp] = map->int_pri;
    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_set(dev_id, &glbl_info));
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_lif_cos_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0, tmp_dp = 0;
    uint32 profile_ndx = 0;
    SOC_PPD_LIF_COS_PROFILE_INFO profile_info;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY map_entry;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY mpls_key;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY ip_key;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY l2_key;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY tc_dp_key;
    int count1 = 0, count2 = 0, count3 = 0;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid lif_cos profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF COS profile")));
    }
    
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&map_entry);
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(&mpls_key);
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(&ip_key);
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(&l2_key);
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&tc_dp_key);
    
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id,
                              profile_ndx,
                              &profile_info));
    
    if (flags & BCM_QOS_MAP_MPLS) {
    
        /*
        * 'map_when_ip' and 'map_when_mpls' should have the 
        * same value for Soc_petra-B.
        */
        profile_info.map_when_ip = TRUE;
        profile_info.map_when_mpls = TRUE;

        mpls_key.in_exp = map->exp;
        map_entry.tc = map->int_pri;
        map_entry.dp = dp;
        if (SOC_IS_ARAD(unit)) {
            map_entry.dscp = map->remark_int_pri;
        }
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_mpls_label_info_set(dev_id,
                                     profile_ndx,
                                     &mpls_key,
                                     &map_entry));

        if ((map_entry.tc + map_entry.dp) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap ing_lif_cos_mpls_bitmap should be set.
             */
            SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_MPLS(profile_ndx, map->exp));
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap ing_lif_cos_mpls_bitmap should be clear.
             */
            SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_MPLS(profile_ndx, map->exp));

            _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_ING_MPLS_EXP_MAX, count1);
            if (count1 == 0) {
                _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count2);
                _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count3);
                if ((count2 == 0) && (count3 == 0)) {
                    /*
                     * 'map_when_ip' and 'map_when_mpls' should have the 
                     * same value for Soc_petra-B.
                     */
                    profile_info.map_when_ip = FALSE;
                    profile_info.map_when_mpls = FALSE;
                }       
            } 
        }
        
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_ing_lif_cos_mpls_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */        
    } 

    if (flags & BCM_QOS_MAP_L3) {

        /*
        * 'map_when_ip' and 'map_when_mpls' should have the 
        * same value for Soc_petra-B.
        */
        profile_info.map_when_ip = TRUE;
        profile_info.map_when_mpls = TRUE;

        ip_key.ip_type = (flags & BCM_QOS_MAP_IPV6) ? SOC_SAND_PP_IP_TYPE_IPV6 : SOC_SAND_PP_IP_TYPE_IPV4;
        ip_key.tos = map->dscp;

        map_entry.tc = map->int_pri;
        map_entry.dp = dp;
        if (SOC_IS_ARAD(unit)) {
            map_entry.dscp = map->remark_int_pri;
        }
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_ip_info_set(dev_id,
                                 profile_ndx,
                                 &ip_key,
                                 &map_entry));

        if ((map_entry.tc + map_entry.dp) != 0) {
            if (flags & BCM_QOS_MAP_IPV6) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap ing_lif_cos_ipv6_bitmap should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));
            }
            else {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap BCM_QOS_MAP_IPV4 should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));
            }
        } else {
            if (flags & BCM_QOS_MAP_IPV6) {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_lif_cos_ipv6_bitmap should be clear..
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_lif_cos_ipv4_bitmap should be clear.
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));
            }
            
            _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count1);
            _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count2);
            if ((count1 == 0) && (count2 == 0)) {
                _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_ING_MPLS_EXP_MAX, count3);
                if (count3 == 0) {
                    /*
                     * 'map_when_ip' and 'map_when_mpls' should have the 
                     * same value for Soc_petra-B.
                     */
                    profile_info.map_when_ip = FALSE;
                    profile_info.map_when_mpls = FALSE;
                }       
            }            
        }
        
        if (flags & BCM_QOS_MAP_IPV6) {
#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_ing_lif_cos_ipv6_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
        }
        else {
#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_ing_lif_cos_ipv4_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */            
        }
    }

    if (flags & BCM_QOS_MAP_L2 && (!(flags & BCM_QOS_MAP_L2_UNTAGGED))) {

        profile_info.map_when_l2 = TRUE;

        if (SOC_IS_PETRAB(unit)) { /* Petra-B outer_tpid means TPID_id */
            l2_key.outer_tpid = 1;
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                l2_key.outer_tpid = 2;
            }
        } else { /* ARAD */
            l2_key.outer_tpid = (flags & BCM_QOS_MAP_L2_INNER_TAG) ? 1:0;
        }
        

        l2_key.incoming_up = map->pkt_pri;
        l2_key.incoming_dei = map->pkt_cfi;

        map_entry.tc = map->int_pri;
        map_entry.dp = dp;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_l2_info_set(dev_id,
                                     profile_ndx,
                                     &l2_key,
                                     &map_entry));

        if ((map_entry.tc + map_entry.dp) != 0) {
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                /* if entry content is non-zero, the entry is in use, 
                 * corresponding bit in bitmap ing_lif_cos_l2_inner_bitmap should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_INNER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));
            }
            else {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap ing_lif_cos_l2_outer_bitmap should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_OUTER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));
            }
        } else {
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_lif_cos_l2_inner_bitmap should be clear..
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_INNER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_lif_cos_l2_outer_bitmap should be clear..
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_OUTER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));
            }

            _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX, count1);
            _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX, count2);
            if ((count1 == 0) && (count2 == 0)) {           
                profile_info.map_when_l2 = FALSE;
            }
        }

#ifdef BCM_WARM_BOOT_SUPPORT
        if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
            rv = _bcm_dpp_wb_qos_update_ing_lif_cos_l2_inner_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            rv = _bcm_dpp_wb_qos_update_ing_lif_cos_l2_outer_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);            
        }
#endif /* BCM_WARM_BOOT_SUPPORT */           
    }

    if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        profile_info.map_from_tc_dp = TRUE;

        tc_dp_key.tc = map->pkt_pri;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->pkt_cfi,&tmp_dp));         
        tc_dp_key.dp = tmp_dp;

        map_entry.tc = map->int_pri;
        map_entry.dp = dp;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_tc_dp_info_set(dev_id,
                                     profile_ndx,
                                     &tc_dp_key,
                                     &map_entry));

        if ((map_entry.tc + map_entry.dp) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap ing_lif_cos_l2_untag_bitmap should be set.
             */
            SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_UNTAG(profile_ndx, tc_dp_key.tc, tc_dp_key.dp));
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap ing_lif_cos_l2_untag_bitmap should be clear.
             */
            SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_UNTAG(profile_ndx, tc_dp_key.tc, tc_dp_key.dp));

            _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap, profile_ndx, _BCM_QOS_MAP_TC_DP_MAX, count1);
            if (count1 == 0) {
                profile_info.map_from_tc_dp = FALSE;
            }
        }
        
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_ing_lif_cos_l2_untag_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);            
#endif /* BCM_WARM_BOOT_SUPPORT */          
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && (flags & BCM_QOS_MAP_L3_L2)) {
        if (profile_ndx > _BCM_QOS_MAP_ING_USE_PCP_MAX_QOS_PROFILE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid lif_cos profile (%d) out of range (0-15).\n"), FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF COS profile for using layer 2 pcp")));
        }
        
        profile_info.use_layer2_pcp = TRUE;
    }
#endif /* BCM_88660_A0 */

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id,
                              profile_ndx,
                              &profile_info));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_qos_map_ingress_pcp_vlan_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    BCMDNX_INIT_FUNC_DEFS;

    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if ((flags & BCM_QOS_MAP_L2_VLAN_PCP) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_L2_VLAN_PCP must be set")));
    }

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid ingress vlan pcp profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
    }

    if (flags & BCM_QOS_MAP_L2) {
        /* ing_vlan_edit */
        if (flags & BCM_QOS_MAP_L2_INNER_TAG) {

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set(dev_id, 
                                    profile_ndx, 
                                    map->int_pri,
                                    map->pkt_pri,
                                    map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap ing_pcp_vlan_ctag_bitmap should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_ctag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_pcp_vlan_ctag_bitmap should be clear.
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_ctag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
            }
            
#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_ing_pcp_vlan_ctag_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);            
#endif /* BCM_WARM_BOOT_SUPPORT */             
        } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set(dev_id, 
                                        profile_ndx, 
                                        map->int_pri,
                                        dp,
                                        map->pkt_pri,
                                        map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap ing_pcp_vlan_utag_bitmap should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_utag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_pcp_vlan_utag_bitmap should be clear.
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_utag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
            }

#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_ing_pcp_vlan_utag_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);            
#endif /* BCM_WARM_BOOT_SUPPORT */              
        } else { /* OUTER TAG */

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set(dev_id, 
                                    profile_ndx, 
                                    map->int_pri,
                                    (dp) ? 1 : 0,
                                    map->pkt_pri,
                                    map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap ing_pcp_vlan_stag_bitmap should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_stag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_STAG(profile_ndx, map->int_pri, (dp) ? 1 : 0));
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_pcp_vlan_stag_bitmap should be clear.
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_stag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_STAG(profile_ndx, map->int_pri, (dp) ? 1 : 0));
            }

#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_ing_pcp_vlan_stag_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);            
#endif /* BCM_WARM_BOOT_SUPPORT */              
        }

    }
    
    BCMDNX_IF_ERR_EXIT(rv);    
exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int 
_bcm_petra_qos_map_ingress_cos_ipv4_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id;
    uint32 sand_rv, opcode_ndx;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    if ((opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L3) == 0) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
    }

    action_info.is_packet_valid = (flags & BCM_QOS_MAP_PACKET_INVALID) ? FALSE : TRUE;
    action_info.is_qos_only = (flags & BCM_QOS_MAP_IGNORE_OFFSET) ? TRUE : FALSE;
    action_info.ac_offset = map->port_offset;

    sand_rv = soc_ppd_lif_cos_opcode_ipv4_tos_map_set(dev_id, opcode_ndx, map->dscp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    
    BCMDNX_IF_ERR_EXIT(rv);
    SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags[opcode_ndx] |= flags;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_ipv6_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id;
    uint32 sand_rv, opcode_ndx;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    if ((opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L3) == 0) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
    }

    action_info.is_packet_valid = (flags & BCM_QOS_MAP_PACKET_INVALID) ? FALSE : TRUE;
    action_info.is_qos_only = (flags & BCM_QOS_MAP_IGNORE_OFFSET) ? TRUE : FALSE;
    action_info.ac_offset = map->port_offset;

    sand_rv = soc_ppd_lif_cos_opcode_ipv6_tos_map_set(dev_id, opcode_ndx, map->dscp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    BCMDNX_IF_ERR_EXIT(rv);
    SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags[opcode_ndx] |= flags;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_l2_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id;
    uint32 opcode_ndx, sand_rv;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPD_LLP_PARSE_TPID_INDEX outer_tpid_ndx;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);

    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    _bcm_petra_qos_map_add_pcp_vlan_params_verify(unit, flags, map, map_id);

    sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    if ((opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L2) == 0) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
    }

    if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        outer_tpid_ndx = SOC_PPD_LLP_PARSE_TPID_INDEX_NONE;
    } else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        outer_tpid_ndx = SOC_PPD_LLP_PARSE_TPID_INDEX_TPID2;
    } else {
        outer_tpid_ndx = SOC_PPD_LLP_PARSE_TPID_INDEX_TPID1;
    }

    action_info.is_packet_valid = (flags & BCM_QOS_MAP_PACKET_INVALID) ? FALSE : TRUE;
    action_info.is_qos_only = (flags & BCM_QOS_MAP_IGNORE_OFFSET) ? TRUE : FALSE;
    action_info.ac_offset = map->port_offset;

    sand_rv = soc_ppd_lif_cos_opcode_vlan_tag_map_set(dev_id, opcode_ndx, outer_tpid_ndx, map->int_pri, map->pkt_cfi, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    BCMDNX_IF_ERR_EXIT(rv);
    SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags[opcode_ndx] |= flags;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_tc_dp_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id, dp;
    uint32 opcode_ndx, sand_rv;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    if ((opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP) == 0) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    action_info.is_packet_valid = (flags & BCM_QOS_MAP_PACKET_INVALID) ? FALSE : TRUE;
    action_info.is_qos_only = (flags & BCM_QOS_MAP_IGNORE_OFFSET) ? TRUE : FALSE;
    action_info.ac_offset = map->port_offset;

    sand_rv = soc_ppd_lif_cos_opcode_tc_dp_map_set(dev_id, opcode_ndx, map->int_pri, dp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    BCMDNX_IF_ERR_EXIT(rv);
    SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags[opcode_ndx] |= flags;
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_cos_opcode_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id;
    uint32 opcode_ndx, sand_rv;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type, type;
    BCMDNX_INIT_FUNC_DEFS;

    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if ((flags & BCM_QOS_MAP_IPV4) || (flags & BCM_QOS_MAP_IPV6)) {
        type = SOC_PPD_LIF_COS_OPCODE_TYPE_L3 | SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP;
    } else if (flags & BCM_QOS_MAP_L2) {
        type = SOC_PPD_LIF_COS_OPCODE_TYPE_L2;
    } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        type = SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP;
    } else {
        type = SOC_PPD_LIF_COS_OPCODE_TYPE_L3 | SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP;
    }

    if (SHR_BITGET(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, opcode_ndx) == 0) {
        sand_rv = soc_ppd_lif_cos_opcode_types_set(dev_id, opcode_ndx, type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, opcode_ndx);

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_qos_update_ing_opcode_bmp_state(unit, opcode_ndx);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */


    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if (opcode_type != type) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Map ID does not match flags.\n")));
        }
    }

    if (type & SOC_PPD_LIF_COS_OPCODE_TYPE_L3) {
        if (flags & BCM_QOS_MAP_IPV4) {
            rv = _bcm_petra_qos_map_ingress_cos_ipv4_add(unit, flags, map, map_id);
        } 
        else if (flags & BCM_QOS_MAP_IPV6) {
            rv = _bcm_petra_qos_map_ingress_cos_ipv6_add(unit, flags, map, map_id);
        } 
        else {
            rv = _bcm_petra_qos_map_ingress_cos_tc_dp_add(unit, flags, map, map_id);
        }
    }
    else if (type & SOC_PPD_LIF_COS_OPCODE_TYPE_L2) {   /* L2 */
        rv = _bcm_petra_qos_map_ingress_cos_l2_add(unit, flags, map, map_id);
    } 
    else if (type & SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP) { /* TC/DP */
        rv = _bcm_petra_qos_map_ingress_cos_tc_dp_add(unit, flags, map, map_id);
    }

    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_qos_update_ing_cos_opcode_state(unit, opcode_ndx);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_qos_map_egress_remark_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    SOC_PPD_EG_QOS_MAP_KEY prm_in_qos_key;
    SOC_PPD_EG_QOS_PARAMS  prm_out_qos_params;  
    SOC_PPD_EG_ENCAP_QOS_MAP_KEY prm_in_encap_qos_key;
    SOC_PPD_EG_ENCAP_QOS_PARAMS  prm_out_encap_qos_params;
    int tmp;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPD_EG_QOS_MAP_KEY_clear(&prm_in_qos_key);
    SOC_PPD_EG_QOS_PARAMS_clear(&prm_out_qos_params);

    SOC_PPD_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
    SOC_PPD_EG_ENCAP_QOS_PARAMS_clear(&prm_out_encap_qos_params);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if (flags & BCM_QOS_MAP_ENCAP) {
      
      if (flags & BCM_QOS_MAP_MPLS) {
          if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
          }

          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }

          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri;
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_MPLS;


          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
          
          prm_out_encap_qos_params.ip_dscp = map->dscp; 
          prm_out_encap_qos_params.mpls_exp = map->exp; 
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));

          if ((map->dscp + map->exp) != 0) {
              /* if entry content is non-zero, the entry is in use.
               * corresponding bit in bitmap egr_remark_encap_mpls_bitmap should be set.
               */
              SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_MPLS(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
          }
          else {
              /* if entry content is zero, the entry is not in use.
               * corresponding bit in bitmap egr_remark_encap_mpls_bitmap should be clear.
               */
              SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_MPLS(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
          }
          
#ifdef BCM_WARM_BOOT_SUPPORT
          rv = _bcm_dpp_wb_qos_update_egr_remark_encap_mpls_bitmap_state(unit, profile_ndx);
          BCMDNX_IF_ERR_EXIT(rv);            
#endif /* BCM_WARM_BOOT_SUPPORT */           
     }
     else if (flags & BCM_QOS_MAP_IPV6) 
     {
          if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
          }

          /* In !ARAD-A0 Remark profile for IP Tunnels is only 15 remark ids */
          if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id-1) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range. There are only 15 Remark profile for IP tunnels")));
          }

          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }
          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri;
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_IPV6;


          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
          prm_out_encap_qos_params.ip_dscp = map->dscp; 
          prm_out_encap_qos_params.mpls_exp = map->exp; 
           
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));

          if ((map->dscp + map->exp) != 0) {
              /* if entry content is non-zero, the entry is in use.
               * corresponding bit in bitmap egr_remark_encap_ipv6_bitmap should be set.
               */
              SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
          }
          else {
              /* if entry content is zero, the entry is not in use.
               * corresponding bit in bitmap egr_remark_encap_ipv6_bitmap should be clear.
               */
              SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
          }

#ifdef BCM_WARM_BOOT_SUPPORT
          rv = _bcm_dpp_wb_qos_update_egr_remark_encap_ipv6_bitmap_state(unit, profile_ndx);
          BCMDNX_IF_ERR_EXIT(rv);            
#endif /* BCM_WARM_BOOT_SUPPORT */           
     }
     else if (flags & BCM_QOS_MAP_IPV4) 
     {
          if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
          }

          /* In !ARAD-A0 Remark profile for IP Tunnels is only 15 remark ids */
          if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id-1) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range. There are only 15 Remark profile for IP tunnels")));
          }

          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }
          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri;
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_IPV4;


          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
          prm_out_encap_qos_params.ip_dscp = map->dscp; 
          prm_out_encap_qos_params.mpls_exp = map->exp; 
           
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));

        if ((map->dscp + map->exp) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_remark_encap_ipv4_bitmap should be set.
             */
            SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
        }
        else {
             /* if entry content is zero, the entry is not in use.
              * corresponding bit in bitmap egr_remark_encap_ipv4_bitmap should be clear.
              */
              SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
        }

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_remark_encap_ipv4_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);            
#endif /* BCM_WARM_BOOT_SUPPORT */           
     }
     else if (flags & BCM_QOS_MAP_L2) 
     {
          /* Only used for ARAD_B0_and_above */
          if (SOC_IS_ARAD_A0(unit) || SOC_IS_PETRAB(unit)) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("REMARK flag L2 is not supported")));
          }

          if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
          }

          /* Ethernet is only 4 remark ids */
          if (profile_ndx >= 4) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range. There are only 4 Remark profile for Ethernet encap remark")));
          }

          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }
          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)(map->remark_int_pri & 0x7);
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_ETH;
          tmp = 0;
          BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&tmp)); /* dp */
          prm_in_encap_qos_key.dp = tmp;

          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
          prm_out_encap_qos_params.ip_dscp = map->dscp; 
          prm_out_encap_qos_params.mpls_exp = map->exp; 
           
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));

        if ((map->dscp + map->exp) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_remark_encap_l2_bitmap should be set.
             */
            SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_l2_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L2(profile_ndx, map->remark_int_pri, map->color));
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap egr_remark_encap_mpls_bitmap should be clear.
             */
            SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_l2_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L2(profile_ndx, map->remark_int_pri, map->color));
        }
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_remark_encap_l2_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);            
#endif /* BCM_WARM_BOOT_SUPPORT */          
     }
    }
    else { /* Not encap header */
        if (flags & BCM_QOS_MAP_MPLS) {
    
            if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
        
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid remark profile (%d) out of range\n"),
                                      FUNCTION_NAME(), profile_ndx));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
            }
            /* internal pri */
            prm_in_qos_key.remark_profile = profile_ndx;
            prm_in_qos_key.dp = dp;
            prm_in_qos_key.in_dscp_exp = (uint32) map->int_pri; 
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));
    
            prm_out_qos_params.mpls_exp = map->remark_int_pri; 
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_set(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));

            if (map->remark_int_pri != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap egr_remark_mpls_bitmap should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_remark_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_MPLS(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap egr_remark_mpls_bitmap should be clear.
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_MPLS(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
            }

#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_egr_remark_mpls_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);            
#endif /* BCM_WARM_BOOT_SUPPORT */             
        }
        else if (flags & BCM_QOS_MAP_L3) {
        
            if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
        
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid remark profile (%d) out of range\n"),
                                      FUNCTION_NAME(), profile_ndx));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
            }
            /* internal pri */
            prm_in_qos_key.remark_profile = profile_ndx;
            prm_in_qos_key.dp = dp;
            prm_in_qos_key.in_dscp_exp = (uint32) map->int_pri; 
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));
        
            /* IPV6 */
            if (flags & BCM_QOS_MAP_IPV6) {
                prm_out_qos_params.ipv6_tc = map->remark_int_pri; 
            }
            else { /* IPV4 */
                prm_out_qos_params.ipv4_tos = map->remark_int_pri; 
            }
    
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_set(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));


            if (map->remark_int_pri != 0) {
                if (flags & BCM_QOS_MAP_IPV6) {
                    /* if entry content is non-zero, the entry is in use.
                     * corresponding bit in bitmap egr_remark_ipv6_bitmap should be set.
                     */
                    SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                }
                else {
                    /* if entry content is non-zero, the entry is in use.
                     * corresponding bit in bitmap egr_remark_ipv4_bitmap should be set.
                     */                    
                    SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                }
            } else {
                if (flags & BCM_QOS_MAP_IPV6) {
                    /* if entry content is zero, the entry is not in use.
                     * corresponding bit in bitmap egr_remark_ipv6_bitmap should be clear.
                     */
                    SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                }
                else {
                    /* if entry content is zero, the entry is not in use.
                     * corresponding bit in bitmap egr_remark_ipv4_bitmap should be clear.
                     */
                    SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                }                
            }

#ifdef BCM_WARM_BOOT_SUPPORT
            if (flags & BCM_QOS_MAP_IPV6) {
                rv = _bcm_dpp_wb_qos_update_egr_remark_ipv6_bitmap_state(unit, profile_ndx);
                BCMDNX_IF_ERR_EXIT(rv);            
            }
            else {
                rv = _bcm_dpp_wb_qos_update_egr_remark_ipv4_bitmap_state(unit, profile_ndx);
                BCMDNX_IF_ERR_EXIT(rv);                
            }
#endif /* BCM_WARM_BOOT_SUPPORT */
        } 
        else {
            rv = BCM_E_UNAVAIL;
        }
    }
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_qos_map_egress_mpls_php_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;  
    int dev_id = 0;
    uint32 profile_ndx = 0;
    SOC_PPD_EG_QOS_PHP_REMARK_KEY prm_in_php_remark_qos_key;
    

    BCMDNX_INIT_FUNC_DEFS;

    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPD_EG_QOS_PHP_REMARK_KEY_clear(&prm_in_php_remark_qos_key);

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php) 
    {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid MPLS PHP profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS PHPs profile")));
    }
    
    if (flags & BCM_QOS_MAP_IPV6) {
          
          prm_in_php_remark_qos_key.exp = map->int_pri; /* In-DSCP-EXP before */
          prm_in_php_remark_qos_key.exp_map_profile = profile_ndx;
          prm_in_php_remark_qos_key.php_type = SOC_PPD_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6;                 
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_php_remark_set(dev_id,
                             &prm_in_php_remark_qos_key,
                             map->remark_int_pri)); /* Out-DSCP-EXP after */

        if (map->remark_int_pri != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_mpls_php_ipv6_bitmap should be set.
             */
            SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));
        }
        else {
             /* if entry content is zero, the entry is not in use.
              * corresponding bit in bitmap egr_mpls_php_ipv6_bitmap should be clear.
              */
            SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));
        }
        
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_mpls_php_ipv6_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
     }
     else if (flags & BCM_QOS_MAP_IPV4) 
     {
        
          prm_in_php_remark_qos_key.exp = map->int_pri; /* In-DSCP-EXP before */
          prm_in_php_remark_qos_key.exp_map_profile = profile_ndx;
          prm_in_php_remark_qos_key.php_type = SOC_PPD_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4;
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_php_remark_set(dev_id,
                             &prm_in_php_remark_qos_key,
                             map->remark_int_pri)); /* Out-DSCP-EXP after */
          
        if (map->remark_int_pri != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_mpls_php_ipv4_bitmap should be set.
             */
            SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap egr_mpls_php_ipv4_bitmap should be clear.
             */
            SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));
        }

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_mpls_php_ipv4_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */        
     }     
     else
     {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS PHPs flags")));
     }

     BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_88660
static int
_bcm_petra_qos_map_egress_dscp_exp_marking_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE, sand_rv;  
    uint32 profile_ndx = 0;
    SOC_PPC_EG_QOS_MARKING_KEY key;
    SOC_PPC_EG_QOS_MARKING_PARAMS val;
    int simple_mode;

    BCMDNX_INIT_FUNC_DEFS;

    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking) 
    {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid DSCP/EXP marking profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid DSCP/EXP marking profile")));
    }
    
    /* We don't allow any flags */
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid DSCP/EXP marking flags")));
    }

    simple_mode = soc_property_get(unit, spn_BCM886XX_QOS_L3_L2_MARKING, 0);
    if (simple_mode) {
        uint32 inlif_profile_same_dscp_mask = 0;
        uint32 inlif_profile_index;

        SOC_PPC_EG_QOS_MARKING_KEY_clear(&key);
        SOC_PPC_EG_QOS_MARKING_PARAMS_clear(&val);

        key.resolved_dp_ndx = map->remark_color;
        key.tc_ndx = map->int_pri;
        key.marking_profile = profile_ndx;

        val.ip_dscp = map->dscp;
        val.mpls_exp = map->exp;

        /* get mask of inlif profile with dscp marking set */
        sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_profile_mgmt_if_profile_val_set, (unit, SOC_PPC_PROFILE_MGMT_TYPE_INLIF, SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_SIMPLE_DSCP_MARKING, /*value*/1,&inlif_profile_same_dscp_mask));
        SOCDNX_IF_ERR_EXIT(sand_rv);

        /* Set this mapping for all the relevant bits */
        for (inlif_profile_index = 0; inlif_profile_index < SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_index++) {
            if (inlif_profile_index & inlif_profile_same_dscp_mask) {
                key.in_lif_profile = inlif_profile_index;

                SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_marking_set(unit, &key, &val));
            }
        }
    } else {
        SOC_PPC_EG_QOS_MARKING_KEY_clear(&key);
        SOC_PPC_EG_QOS_MARKING_PARAMS_clear(&val);

        key.resolved_dp_ndx = map->remark_color;
        key.tc_ndx = map->int_pri;
        key.in_lif_profile = map->port_offset;
        key.marking_profile = profile_ndx;

        val.ip_dscp = map->dscp;
        val.mpls_exp = map->exp;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_marking_set(unit, &key, &val));

    }
    if ((map->dscp + map->exp) != 0) {
        /* if entry content is non-zero, the entry is in use.
         * corresponding bit in bitmap egr_dscp_exp_marking_bitmap should be set.
         */
        SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_dscp_exp_marking_bitmap, 
            _DPP_QOS_ENTRY_INDEX_EGR_DSCP_EXP_MARKING(profile_ndx, key.in_lif_profile, key.tc_ndx, key.resolved_dp_ndx));
    }
    else {
        /* if entry content is zero, the entry is not in use.
         * corresponding bit in bitmap egr_dscp_exp_marking_bitmap should be clear.
         */
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_dscp_exp_marking_bitmap, 
            _DPP_QOS_ENTRY_INDEX_EGR_DSCP_EXP_MARKING(profile_ndx, key.in_lif_profile, key.tc_ndx, key.resolved_dp_ndx));
    }


    /* Unused warning */    
    rv = rv;

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_qos_update_egr_dscp_exp_marking_bitmap_state(unit, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN;
}

#endif /* BCM_88660 */

STATIC int
_bcm_petra_qos_map_egress_pcp_vlan_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY key_mapping;
    int count1 = 0, count2 = 0, count3 = 0;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if ((flags & BCM_QOS_MAP_L2_VLAN_PCP) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_L2_VLAN_PCP must be set")));
    }

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid egress vlan pcp profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
    }

    if (flags & BCM_QOS_MAP_L2) {

        if (flags & BCM_QOS_MAP_MIM_ITAG) {
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set(dev_id, profile_ndx, map->int_pri,
                                     dp, map->pkt_pri, map->pkt_cfi));

        }
        else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {

            key_mapping =  SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_TC_DP;
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                                       profile_ndx, 
                                       key_mapping));

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_untagged_set(dev_id, 
                                       profile_ndx, 
                                       map->int_pri, 
                                       dp, 
                                       map->pkt_pri, 
                                       map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap egr_pcp_vlan_utag_bitmap should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_utag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap egr_pcp_vlan_utag_bitmap should be clear.
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_utag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
            }

#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_utag_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */            
        } 
        else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {

            key_mapping =  SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_PCP;
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                                       profile_ndx, 
                                       key_mapping));

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_ctag_set(dev_id, 
                                   profile_ndx, 
                                   map->int_pri, 
                                   map->pkt_pri, 
                                   map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap egr_pcp_vlan_ctag_bitmap should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ctag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap egr_pcp_vlan_ctag_bitmap should be clear.
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ctag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
            }
            
#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_ctag_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
        } 
        else { /* OUTER TAG */

            key_mapping =  SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_PCP;
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                                       profile_ndx, 
                                       key_mapping));

        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_stag_set(dev_id, 
                                   profile_ndx, 
                                   map->int_pri, 
                                   map->color, 
                                   map->pkt_pri, 
                                   map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap egr_pcp_vlan_ctag_bitmap should be set.
                 */
                SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_stag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_STAG(profile_ndx, map->int_pri, map->color));
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap egr_pcp_vlan_stag_bitmap should be clear.
                 */
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_stag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_STAG(profile_ndx, map->int_pri, map->color));
            }
            
#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_stag_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
        }
    } 
    else if (flags & BCM_QOS_MAP_MPLS) { 
        /* Arad only */
        if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable != 1) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: dscp_exp mapping is disabled. \n"), FUNCTION_NAME()));  
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DSCP_EXP mapping is disabled")));
        }
        if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id == -1) {  /* no profile is set to dscp_exp mapping yet */
            SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = profile_ndx;
        }
        if (profile_ndx !=  SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid egress vlan pcp profile (%d). profile ndx for dscp_exp mapping is set to (%d). \n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable, profile_ndx)); 
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
        }
        key_mapping =  SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_DSCP_EXP;
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                                       profile_ndx, 
                                       key_mapping));

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                               SOC_PPD_PKT_FRWRD_TYPE_MPLS, 
                               map->exp,
                               map->pkt_pri, 
                               map->pkt_cfi));
        
        if ((map->pkt_pri + map->pkt_cfi) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_pcp_vlan_mpls_bitmap should be set.
             */
            SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_MPLS(profile_ndx, map->exp));
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap egr_pcp_vlan_mpls_bitmap should be clear.
             */
            SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_MPLS(profile_ndx, map->exp));

            _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count1);
            if (count1 == 0) {
                _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);
                _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count3);
                if ((count2 == 0) && (count3 == 0)) {
                    SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
                }
            }
        }
        
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_mpls_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */        
    } 
    else if ((flags & BCM_QOS_MAP_L3) && (flags & BCM_QOS_MAP_IPV6)) {
          /* Arad only */
          if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: dscp_exp mapping is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DSCP_EXP mapping is disabled")));
          }
          if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id == -1) {  /* no profile is set to dscp_exp mapping yet */
              SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = profile_ndx;
          }
          if (profile_ndx !=  SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid egress vlan pcp profile (%d). profile ndx for dscp_exp mapping is set to (%d). \n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable, profile_ndx)); 
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
          }
          key_mapping =  SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_DSCP_EXP;
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                           profile_ndx, 
                           key_mapping));

          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                     SOC_PPD_PKT_FRWRD_TYPE_IPV6_UC, 
                     map->dscp,
                                   map->pkt_pri, 
                                   map->pkt_cfi));
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                     SOC_PPD_PKT_FRWRD_TYPE_IPV6_MC, 
                     map->dscp,
                                   map->pkt_pri, 
                                   map->pkt_cfi));

          if ((map->pkt_pri + map->pkt_cfi) != 0) {
              /* if entry content is non-zero, the entry is in use.
               * corresponding bit in bitmap egr_pcp_vlan_ipv6_bitmap should be set.
               */
              SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));
          }
          else {
              /* if entry content is zero, the entry is not in use.
               * corresponding bit in bitmap egr_pcp_vlan_ipv6_bitmap should be clear.
               */
              SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));
              
              _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count1);
              if (count1 == 0) {
                  _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);
                  _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count3);
                  if ((count2 == 0) && (count3 == 0)) {
                      SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
                  }
              }              
          }
          
#ifdef BCM_WARM_BOOT_SUPPORT
          rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_ipv6_bitmap_state(unit, profile_ndx);
          BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
     }
     else if ((flags & BCM_QOS_MAP_L3) && (flags & BCM_QOS_MAP_IPV4)) {
          /* Arad only */

          if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: dscp_exp mapping is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DSCP_EXP mapping is disabled")));
          }
          if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id == -1) {  /*no profile is set to dscp_exp mapping yet*/
              SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = profile_ndx;
          }
          if (profile_ndx !=  SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid egress vlan pcp profile (%d). profile ndx for dscp_exp mapping is set to (%d). \n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable, profile_ndx)); 
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
          }
          key_mapping =  SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_DSCP_EXP;
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                           profile_ndx, 
                           key_mapping));

          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                     SOC_PPD_PKT_FRWRD_TYPE_IPV4_UC, 
                     map->dscp,
                                   map->pkt_pri, 
                                   map->pkt_cfi));
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                     SOC_PPD_PKT_FRWRD_TYPE_IPV4_MC, 
                     map->dscp,
                                   map->pkt_pri, 
                                   map->pkt_cfi));
          
          if ((map->pkt_pri + map->pkt_cfi) != 0) {
              /* if entry content is non-zero, the entry is in use.
               * corresponding bit in bitmap egr_pcp_vlan_ipv4_bitmap should be set.
               */
              SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));
          }
          else {
              /* if entry content is zero, the entry is not in use.
               * corresponding bit in bitmap egr_pcp_vlan_ipv4_bitmap should be clear.
               */
              SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));

              _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count1);
              if (count1 == 0) {
                  _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);
                  _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count3);
                  if ((count2 == 0) && (count3 == 0)) {
                      SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
                  }
              }               
          }
#ifdef BCM_WARM_BOOT_SUPPORT
          rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_ipv4_bitmap_state(unit, profile_ndx);
          BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
     } else {
         rv = BCM_E_PARAM;
     }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_qos_map_egress_l2_i_tag_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if ((flags & BCM_QOS_MAP_L2_VLAN_PCP) == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_L2_VLAN_PCP must be set")));
    }

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag) {
    
    LOG_ERROR(BSL_LS_BCM_QOS,
              (BSL_META_U(unit,
                          "%s: invalid egress egress l2 i tag profile (%d) out of range\n"),
                          FUNCTION_NAME(), profile_ndx));
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid L2 I-TAG profile")));
    }


    if (flags & BCM_QOS_MAP_MIM_ITAG) {
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set(dev_id, profile_ndx, map->int_pri,
                                   dp, map->pkt_pri, map->pkt_cfi));

        if ((map->pkt_pri + map->pkt_cfi) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_l2_i_tag_bitmap should be set.
             */
            SHR_BITSET(SOC_DPP_STATE(unit)->qos_state->egr_l2_i_tag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_L2_I_TAG(profile_ndx, map->int_pri, dp));
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap egr_l2_i_tag_bitmap should be clear.
             */
            SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_l2_i_tag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_L2_I_TAG(profile_ndx, map->int_pri, dp));
        }

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_l2_i_tag_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
    } else {
        rv = BCM_E_PARAM;
    }
     
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) { /* L3 */
        rv = _bcm_petra_qos_map_ingress_lif_cos_add(unit, flags, map, map_id);
    } 
    else if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)) { /* L2 */
        rv = _bcm_petra_qos_map_ingress_pcp_vlan_add(unit, flags, map, map_id);
    } 
    else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {
        rv = _bcm_petra_qos_map_ingress_elsp_add(unit, flags, map, map_id);
    } 
    else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {
        rv = _bcm_petra_qos_map_ingress_cos_opcode_add(unit, flags, map, map_id);
    } 
    else {
        rv = BCM_E_UNAVAIL;
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_egress_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (BCM_INT_QOS_MAP_IS_REMARK(map_id)) {
        rv = _bcm_petra_qos_map_egress_remark_add(unit, flags, map, map_id);
    BCMDNX_IF_ERR_EXIT(rv);

    } else if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)) {
        rv = _bcm_petra_qos_map_egress_pcp_vlan_add(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {
        rv = _bcm_petra_qos_map_egress_l2_i_tag_add(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)) {
        rv = _bcm_petra_qos_map_egress_mpls_php_add(unit, flags, map, map_id);
#ifdef BCM_88660
    } else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)) {
        rv = _bcm_petra_qos_map_egress_dscp_exp_marking_add(unit, flags, map, map_id);
#endif /* BCM_88660*/
    } else {
        rv = BCM_E_UNAVAIL;
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    if (map == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_qos_map_t * is NULL")));
    }

    rv = _bcm_petra_qos_map_add_params_verify(unit, flags, map, map_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (BCM_INT_QOS_MAP_IS_INGRESS(map_id))  {
        rv = _bcm_petra_qos_map_ingress_add(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS(map_id)) {
        rv = _bcm_petra_qos_map_egress_add(unit, flags, map, map_id);
    }
    BCMDNX_IF_ERR_EXIT(rv);
 exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/* bcm_petra_qos_map_multi_get */

int
bcm_petra_qos_map_multi_get(int unit, uint32 flags,
                int map_id, int array_size, 
                bcm_qos_map_t *array, int *array_count)
{
    int rv = BCM_E_NONE, sand_rv;
    int idx = 0, num_entries = 0, dev_id = 0;
    int exp_cnt = 0, tc_cnt = 0, dp_cnt = 0, dscp_cnt = 0, pri_cfi_cnt = 0, int_pri_cnt = 0, tc_dp_cnt = 0, int_cfi = 0;
    int exp = 0, dscp = 0, outer_tpid_index = 0, is_pkt_valid = 0, is_qos_only = 0;
    SOC_PPD_PKT_FRWRD_TYPE pkt_hdr_type;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type;
    bcm_color_t tmp_color;
    bcm_qos_map_t *map;
    uint32 profile_ndx = 0;
    uint32 tmp_dscp_exp = 0;
#ifdef BCM_88660
    int resolved_dp_idx, tc_idx, inlif_idx;
#endif /* BCM_88660 */
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY map_entry;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY mpls_key;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY ip_key;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY l2_key;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY tc_dp_key;
    SOC_PPD_FRWRD_ILM_GLBL_INFO glbl_info;
    SOC_PPD_EG_QOS_MAP_KEY prm_in_qos_key;
    SOC_PPD_EG_QOS_PARAMS  prm_out_qos_params;
    SOC_PPD_EG_ENCAP_QOS_MAP_KEY prm_in_encap_qos_key;
    SOC_PPD_EG_ENCAP_QOS_PARAMS  prm_out_encap_qos_params;
    SOC_PPD_EG_QOS_PHP_REMARK_KEY prm_in_qos_php_key;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO action_info;
#ifdef BCM_88660
    SOC_PPD_EG_QOS_MARKING_KEY marking_key;
    SOC_PPD_EG_QOS_MARKING_PARAMS marking_params;
#endif /* BCM_88660 */

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    is_pkt_valid = (flags & BCM_QOS_MAP_PACKET_INVALID) ? FALSE : TRUE;
    is_qos_only = (flags & BCM_QOS_MAP_IGNORE_OFFSET) ? TRUE : FALSE;

    if (array_count == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("array_count is NULL")));
    }
    
    if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)) {
        if (flags & (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP)) {
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                num_entries = _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX;
            } 
            else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                num_entries = _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX;
            } 
            else if (flags & BCM_QOS_MAP_L2_OUTER_TAG) {
                num_entries = _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX;
            } 
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
        if (flags & BCM_QOS_MAP_MPLS) {
            num_entries = _BCM_QOS_MAP_ING_MPLS_EXP_MAX;
        } 
        else if ((flags & BCM_QOS_MAP_L3) && (flags & (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6))) {
            num_entries = _BCM_QOS_MAP_ING_L3_DSCP_MAX;
        } 
        else if ((flags & BCM_QOS_MAP_L2) && 
                   (flags & (BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_L2_INNER_TAG))) {
            num_entries = _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX;
        }
        else if ((flags & BCM_QOS_MAP_L2_UNTAGGED)) {
            num_entries = _BCM_QOS_MAP_TC_DP_MAX;
        }
#ifdef BCM_88660_A0
        else if ((SOC_IS_ARADPLUS(unit)) && (flags & BCM_QOS_MAP_L3_L2)) {
            num_entries = 0;
        }
#endif /* BCM_88660_A0 */
    } 
    else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {
        num_entries = _BCM_QOS_MAP_ING_MPLS_EXP_MAX;
    } 
    else if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)) {
        if (flags & BCM_QOS_MAP_ENCAP) { 
            if (flags & BCM_QOS_MAP_MPLS) {
                num_entries = _BCM_QOS_MAP_EGR_EXP_MAX;
            } 
            else if (flags & BCM_QOS_MAP_L3) {
                num_entries = _BCM_QOS_MAP_EGR_DSCP_MAX;
            }
        }
        else if ((flags & BCM_QOS_MAP_L2) && (flags & BCM_QOS_MAP_L2_VLAN_PCP)) {
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                num_entries = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX;
            } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                num_entries = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX * SOC_DPP_CONFIG(unit)->qos.dp_max;
            } else { /* outer */ 
                num_entries = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX * SOC_DPP_CONFIG(unit)->qos.dp_max;
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {
        num_entries = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX * SOC_DPP_CONFIG(unit)->qos.dp_max;
    } 
#ifdef BCM_88660
    else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)) {
        num_entries = _BCM_QOS_MAP_EGR_RESOLVED_DP_MAX * _BCM_QOS_MAP_TC_MAX * SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS;
    }
#endif
    else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)) {
        num_entries = _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX;
    } 
    else if (BCM_INT_QOS_MAP_IS_REMARK(map_id)) {
        if (flags & BCM_QOS_MAP_ENCAP) {
            if (flags & BCM_QOS_MAP_MPLS) {
                num_entries =  _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX;
            } 
            else if ((flags & BCM_QOS_MAP_L3) && (flags & (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6))) {
                num_entries =  _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX;
            }
            else if ((flags & BCM_QOS_MAP_L2) && (SOC_IS_ARAD_B0_AND_ABOVE(unit))) { /* L2 */ 
                num_entries = SOC_SAND_PP_MPLS_EXP_MAX * SOC_DPP_CONFIG(unit)->qos.dp_max;
            }
        } 
        else {
            if (flags & BCM_QOS_MAP_MPLS) {
                num_entries =  _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX;
            } else if ((flags & (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4)) ||
                       (flags & (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6))) {
                num_entries =  _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX;
            } 
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, profile_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);

        if (opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L3) {
            if ((flags & BCM_QOS_MAP_IPV4) | (flags & BCM_QOS_MAP_IPV6)) {
                num_entries = _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX;
            } else {
                num_entries = _BCM_QOS_MAP_ING_COS_OPCODE_TC_DP_MAX;
            }
        } else if (opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L2) {
            num_entries = _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX;
        } else if (opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP) {
            num_entries = _BCM_QOS_MAP_ING_COS_OPCODE_TC_DP_MAX;
        } else {
            num_entries = 0;
        }
    }
    
    if (num_entries == 0) {
        *array_count = num_entries;
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("For this type of map_id and these flags, there are no entries to get")));
    }
    
    if (array_size == 0) { /* querying the size of map */
        *array_count = num_entries;
        BCM_EXIT;
    }
    
    if (array == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("array is NULL")));
    }
    
    if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
        if (flags & BCM_QOS_MAP_MPLS) {
            for (exp_cnt = 0; exp_cnt < _BCM_QOS_MAP_ING_MPLS_EXP_MAX; exp_cnt++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);
                SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(&mpls_key);
        
                mpls_key.in_exp = exp_cnt;
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_mpls_label_info_get(dev_id,
                                         profile_ndx,
                                         &mpls_key,
                                         &map_entry));
                map->exp = exp_cnt;
                map->int_pri = map_entry.tc;
                if (SOC_IS_ARAD(unit)) {
                    map->remark_int_pri = map_entry.dscp;
                }
                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, map_entry.dp, &(map->color)));
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    exp_cnt = _BCM_QOS_MAP_ING_MPLS_EXP_MAX;
                    break;
                }
            }
        } 
        else if (flags & BCM_QOS_MAP_L3) {
            for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_ING_L3_DSCP_MAX; dscp_cnt++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);
                SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(&ip_key);
        
                ip_key.ip_type = (flags & BCM_QOS_MAP_IPV6) ? SOC_SAND_PP_IP_TYPE_IPV6 : SOC_SAND_PP_IP_TYPE_IPV4;
                ip_key.tos = dscp_cnt;
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_ip_info_get(dev_id,
                                     profile_ndx,
                                     &ip_key,
                                     &map_entry));
                map->dscp = dscp_cnt;
                map->int_pri = map_entry.tc;
                if (SOC_IS_ARAD(unit)) {
                    map->remark_int_pri = map_entry.dscp;
                }
                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, map_entry.dp, &(map->color)));
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    dscp_cnt = _BCM_QOS_MAP_ING_L3_DSCP_MAX;
                    break;
                }
            }
        } 
        else if (flags & BCM_QOS_MAP_L2 && !(flags & BCM_QOS_MAP_L2_UNTAGGED)) { /* Tag exist */
            for (pri_cfi_cnt = 0; pri_cfi_cnt <  _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; pri_cfi_cnt++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);
                SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(&l2_key);
        
                if (SOC_IS_PETRAB(unit)) {
                    l2_key.outer_tpid = 1;
                    if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                        l2_key.outer_tpid = 2;
                    }
                } else {
                    l2_key.outer_tpid = (flags & BCM_QOS_MAP_L2_INNER_TAG) ? 1:0;
                }
        
                map->pkt_pri = pri_cfi_cnt >> 1;
                map->pkt_cfi = pri_cfi_cnt & 1;
                l2_key.incoming_up = map->pkt_pri;
                l2_key.incoming_dei = map->pkt_cfi;
        
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_l2_info_get(dev_id,
                                     profile_ndx,
                                     &l2_key,
                                     &map_entry));
                map->int_pri = map_entry.tc;
                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, map_entry.dp, &(map->color)));
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    pri_cfi_cnt = _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX;
                    break;
                }
            }
        } 
        else if ((flags & BCM_QOS_MAP_L2_UNTAGGED)) { /* Untagged */
            for (tc_dp_cnt = 0; tc_dp_cnt <  _BCM_QOS_MAP_TC_DP_MAX; tc_dp_cnt++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);
                SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&tc_dp_key);
        
                map->pkt_pri = tc_dp_cnt >> 2;
                map->pkt_cfi = tc_dp_cnt & 3;
                tc_dp_key.tc = map->pkt_pri;
                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, tc_dp_key.dp, &tmp_color));                 
                map->pkt_cfi = (uint8)tmp_color;

                SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_tc_dp_info_get(dev_id,
                                     profile_ndx,
                                     &tc_dp_key,
                                     &map_entry));
                map->int_pri = map_entry.tc;
                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, map_entry.dp, &(map->color)));
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    tc_dp_cnt = _BCM_QOS_MAP_TC_DP_MAX;
                    break;
                }
            }
        } else {
            rv = BCM_E_PARAM;
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {

        SOC_PPD_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);
        BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_get(dev_id, &glbl_info));
    
        for (exp_cnt = 0; exp_cnt < _BCM_QOS_MAP_ING_MPLS_EXP_MAX; exp_cnt++) {
            map = &array[idx++];
            bcm_qos_map_t_init(map);

            map->exp = exp_cnt;
            map->int_pri = glbl_info.elsp_info.exp_map_tbl[exp_cnt];
        
            if ((idx >= num_entries) || (idx >= array_size)) {
                exp_cnt = _BCM_QOS_MAP_ING_MPLS_EXP_MAX;
                break;
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)) {
        if (flags & (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP)) {
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                for (int_pri_cnt = 0; int_pri_cnt < _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX; int_pri_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get(dev_id,
                                        profile_ndx,
                                        int_pri_cnt,
                                        &(map->pkt_pri),
                                        &(map->pkt_cfi)));
            
                    map->int_pri = int_pri_cnt;
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        int_pri_cnt = _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX;
                        break;
                    }
                }
            } 
            else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                for (pri_cfi_cnt = 0; pri_cfi_cnt < _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX; pri_cfi_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get(dev_id,
                                            profile_ndx,
                                            (pri_cfi_cnt >> 2),
                                            (pri_cfi_cnt % 4),
                                            &(map->pkt_pri),
                                            &(map->pkt_cfi)));
            
                    map->int_pri = pri_cfi_cnt>>2;          
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, (pri_cfi_cnt % 4), &(map->color)));
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        pri_cfi_cnt = _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX;
                        break;
                    }
                }
            }
            else if (flags & BCM_QOS_MAP_L2_OUTER_TAG) {
                for (pri_cfi_cnt = 0; pri_cfi_cnt < _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX; pri_cfi_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get(dev_id,
                                        profile_ndx,
                                        (pri_cfi_cnt >> 1),
                                        (pri_cfi_cnt & 1),
                                        &(map->pkt_pri),
                                        &(map->pkt_cfi)));

                    map->int_pri = pri_cfi_cnt>>1;          
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, (pri_cfi_cnt & 1), &(map->color)));
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        pri_cfi_cnt = _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX;
                        break;
                    }
                }
            } 
            else {
                rv = BCM_E_PARAM;
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_REMARK(map_id)) {
        if (flags & BCM_QOS_MAP_ENCAP) { 
            if (flags & BCM_QOS_MAP_MPLS) {
                for (exp_cnt = 0; exp_cnt < _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX; exp_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    SOC_PPD_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
            
                    prm_in_encap_qos_key.remark_profile = profile_ndx;
                    prm_in_encap_qos_key.in_dscp_exp = exp_cnt;
                    prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_MPLS;
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                                        &prm_in_encap_qos_key,
                                        &prm_out_encap_qos_params));
            
                    map->remark_int_pri = prm_in_encap_qos_key.in_dscp_exp;
                    map->remark_color = prm_in_encap_qos_key.remark_profile; 
                    map->exp  = prm_out_encap_qos_params.mpls_exp;
                    map->dscp = prm_out_encap_qos_params.ip_dscp;
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        exp_cnt = _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX;
                        break;
                    }
                }
            } 
            else if (flags & BCM_QOS_MAP_L3) { /* IPV4/V6 */ 
                for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; dscp_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    SOC_PPD_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
            
                    prm_in_encap_qos_key.remark_profile = profile_ndx;
                    prm_in_encap_qos_key.in_dscp_exp = dscp_cnt;
            
                    if (flags & BCM_QOS_MAP_IPV6) {
                        prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_IPV6;
                    } 
                    else {
                        prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_IPV4;
                    }
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                                        &prm_in_encap_qos_key,  
                                        &prm_out_encap_qos_params));
            
                    map->remark_int_pri = prm_in_encap_qos_key.in_dscp_exp;
                    map->remark_color = prm_in_encap_qos_key.remark_profile; 
                    map->exp  = prm_out_encap_qos_params.mpls_exp;
                    map->dscp = prm_out_encap_qos_params.ip_dscp;
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        dscp_cnt = _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX;
                        break;
                    }
                }
            } 
            else if (flags & BCM_QOS_MAP_L2 && (SOC_IS_ARAD_B0_AND_ABOVE(unit))) { /* L2 */

                /* get at most Color x In-DSCP-EXP entries*/
                for (dscp_cnt = 0; dscp_cnt < SOC_SAND_PP_MPLS_EXP_MAX; dscp_cnt++) {
                    for (dp_cnt = 0; dp_cnt < SOC_DPP_CONFIG(unit)->qos.dp_max; dp_cnt++) {
                        map = &array[idx++];
                        bcm_qos_map_t_init(map);
                        SOC_PPD_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
                
                        prm_in_encap_qos_key.remark_profile = profile_ndx;
                        prm_in_encap_qos_key.in_dscp_exp = dscp_cnt;
                        prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_ETH;
                        prm_in_encap_qos_key.dp = dp_cnt;

                        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                                            &prm_in_encap_qos_key,  
                                            &prm_out_encap_qos_params));
                
                        map->remark_int_pri = prm_in_encap_qos_key.in_dscp_exp;
                        map->remark_color = prm_in_encap_qos_key.remark_profile;
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit,dp_cnt,&map->color));
                        map->exp  = prm_out_encap_qos_params.mpls_exp;
                        map->dscp = prm_out_encap_qos_params.ip_dscp;
                
                        if ((idx >= num_entries) || (idx >= array_size)) {
                            dscp_cnt = SOC_SAND_PP_MPLS_EXP_MAX;
                            break;
                        }
                    }
                }
            } 
            else {
                rv = BCM_E_PARAM; 
            }
        } 
        else{ /*not encapsulated*/
            if (flags & BCM_QOS_MAP_MPLS) {
                for (exp_cnt = 0; exp_cnt < _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX; exp_cnt++) {
                    for (dp_cnt = 0; dp_cnt < SOC_DPP_CONFIG(unit)->qos.dp_max; dp_cnt++) {
                        map = &array[idx++];
                        bcm_qos_map_t_init(map);
                        SOC_PPD_EG_QOS_MAP_KEY_clear(&prm_in_qos_key);
            
                        prm_in_qos_key.in_dscp_exp = exp_cnt;
                        prm_in_qos_key.dp = dp_cnt;
                        prm_in_qos_key.remark_profile = profile_ndx;
            
                        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                      &prm_in_qos_key,
                                      &prm_out_qos_params));
            
                        map->remark_int_pri = prm_out_qos_params.mpls_exp;
                        map->exp = prm_in_qos_key.in_dscp_exp;
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, prm_in_qos_key.dp, &(map->color)));
                        if ((idx >= num_entries) || (idx >= array_size)) {
                            exp_cnt = _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX;
                            dp_cnt = SOC_DPP_CONFIG(unit)->qos.dp_max;
                            break;
                        }
                    }
                }
            } 
            else if (flags & BCM_QOS_MAP_L3) {
                for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; dscp_cnt++) {
                    for (dp_cnt = 0; dp_cnt < SOC_DPP_CONFIG(unit)->qos.dp_max; dp_cnt++) {
                        map = &array[idx++];
                        bcm_qos_map_t_init(map);
                        SOC_PPD_EG_QOS_MAP_KEY_clear(&prm_in_qos_key);
            
                        prm_in_qos_key.in_dscp_exp = dscp_cnt;
                        prm_in_qos_key.dp = dp_cnt;
                        prm_in_qos_key.remark_profile = profile_ndx;
						
                        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                      &prm_in_qos_key,
                                      &prm_out_qos_params));
                        if (flags & BCM_QOS_MAP_IPV6) {
                            map->remark_int_pri = prm_out_qos_params.ipv6_tc;
                        } else {
                            map->remark_int_pri = prm_out_qos_params.ipv4_tos;
                        }
                        map->int_pri = prm_in_qos_key.in_dscp_exp;
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, prm_in_qos_key.dp, &(map->color)));
                        if ((idx >= num_entries) || (idx >= array_size)) {
                            dscp_cnt = _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX;
                            dp_cnt = SOC_DPP_CONFIG(unit)->qos.dp_max;
                            break;
                        }
                    }
                }
            } 
            else {
                rv = BCM_E_PARAM;
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)) {
        if (flags & BCM_QOS_MAP_ENCAP) {
            if (flags & BCM_QOS_MAP_MPLS) {
                for (exp = 0; exp < _BCM_QOS_MAP_EGR_EXP_MAX; exp++){
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
            
                    map->exp = exp;
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get(dev_id,
                                           SOC_PPD_PKT_FRWRD_TYPE_MPLS,
                                           exp,
                                           &(map->pkt_pri),
                                           &(map->pkt_cfi)));
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        pri_cfi_cnt = _BCM_QOS_MAP_EGR_EXP_MAX;
                        break;
                    }
                }
            }
            else if (flags & BCM_QOS_MAP_L3) {
                pkt_hdr_type = (flags & BCM_QOS_MAP_IPV6)? SOC_PPD_PKT_FRWRD_TYPE_IPV6_UC : SOC_PPD_PKT_FRWRD_TYPE_IPV4_UC;
                for (dscp = 0; dscp < _BCM_QOS_MAP_EGR_DSCP_MAX; dscp++){
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    map->dscp = dscp;
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get(dev_id,
                                           pkt_hdr_type,
                                           dscp,
                                           &(map->pkt_pri),
                                           &(map->pkt_cfi)));                  
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        pri_cfi_cnt = _BCM_QOS_MAP_EGR_DSCP_MAX;
                        break;
                    }
                }
            } 
            else {
                rv = BCM_E_PARAM;
            }
        } 
        else { /*not encapsulated*/
            if (flags & BCM_QOS_MAP_L2_VLAN_PCP) {
                if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                    for (int_pri_cnt = 0; int_pri_cnt < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri_cnt++) {
                        map = &array[idx++];
                        bcm_qos_map_t_init(map);
            
                        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_ctag_get(dev_id,
                                           profile_ndx,
                                           int_pri_cnt,
                                           &(map->pkt_pri),
                                           &(map->pkt_cfi)));
            
                        map->int_pri = int_pri_cnt;
            
                        if ((idx >= num_entries) || (idx >= array_size)) {
                            int_pri_cnt = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX;
                            break;
                        }
                    }
                } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                    for (int_pri_cnt = 0; int_pri_cnt < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri_cnt++) {
                        for (dp_cnt=0; dp_cnt < SOC_DPP_CONFIG(unit)->qos.dp_max; dp_cnt++) {
                            map = &array[idx++];
                            bcm_qos_map_t_init(map);
                
                            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_untagged_get(dev_id,
                                               profile_ndx,
                                               int_pri_cnt,
                                               dp_cnt,
                                               &(map->pkt_pri),
                                               &(map->pkt_cfi)));
                
                            map->int_pri = int_pri_cnt;         
                            BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(map->color)));
                
                            if ((idx >= num_entries) || (idx >= array_size)) {
                                int_pri_cnt = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX;
                                dp_cnt = SOC_DPP_CONFIG(unit)->qos.dp_max;
                                break;
                            }
                        }
                    }
                } else  {/* outer */ 
                    for (int_pri_cnt = 0; int_pri_cnt < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri_cnt++) {
                        for (int_cfi=0; int_cfi < _BCM_QOS_MAP_EGR_VLAN_CFI_MAX; int_cfi++) {

                            map = &array[idx++];
                            bcm_qos_map_t_init(map);
                
                            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_stag_get(dev_id,
                                           profile_ndx,
                                           int_pri_cnt,
                                           int_cfi,
                                           &(map->pkt_pri),
                                           &(map->pkt_cfi)));
                
                
                            map->color = int_cfi;
                            map->int_pri = int_pri_cnt;
                
                            if ((idx >= num_entries) || (idx >= array_size)) {
                                int_pri_cnt = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX;
                                int_cfi = _BCM_QOS_MAP_EGR_VLAN_CFI_MAX;
                                break;
                            }
                        }
                    }
                }
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {
        for (int_pri_cnt = 0; int_pri_cnt < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri_cnt++) {
            for (dp_cnt=0; dp_cnt < SOC_DPP_CONFIG(unit)->qos.dp_max; dp_cnt++) {
        
                map = &array[idx++];
                bcm_qos_map_t_init(map);
        
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get(dev_id, profile_ndx, int_pri_cnt,
                                     dp_cnt, &(map->pkt_pri), &(map->pkt_cfi)));

                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(map->color)));

                map->int_pri = int_pri_cnt;
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    int_pri_cnt = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX;
                    dp_cnt = SOC_DPP_CONFIG(unit)->qos.dp_max;
                    break;
                }
            }
        }
    } 
#ifdef BCM_88660
    else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)) {
        /* Reset all lines with profile profile_ndx */
        idx = 0;
        for (resolved_dp_idx = 0; (resolved_dp_idx < _BCM_QOS_MAP_EGR_RESOLVED_DP_MAX) && (idx < array_size); resolved_dp_idx++) {
            for (tc_idx = 0; (tc_idx < _BCM_QOS_MAP_TC_MAX) && (idx < array_size); tc_idx++) {
                for (inlif_idx = 0; (inlif_idx < SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS) && (idx < array_size); inlif_idx++) {
                    SOC_PPC_EG_QOS_MARKING_KEY_clear(&marking_key);
                    SOC_PPC_EG_QOS_MARKING_PARAMS_clear(&marking_params);

                    map = &array[idx];

                    bcm_qos_map_t_init(map);
                    
                    marking_key.resolved_dp_ndx = resolved_dp_idx;
                    marking_key.tc_ndx = tc_idx;
                    marking_key.in_lif_profile = inlif_idx;
                    marking_key.marking_profile = profile_ndx;
                    
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_marking_get(dev_id, &marking_key, &marking_params));
                    map->remark_color = marking_key.resolved_dp_ndx;
                    map->int_pri = marking_key.tc_ndx;
                    map->port_offset = marking_key.in_lif_profile;
                    map->dscp = marking_params.ip_dscp;
                    map->exp = marking_params.mpls_exp;

                    idx++;
                }
            }
        }

    } 
#endif
    else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)) {
        for (exp_cnt = 0; exp_cnt < _BCM_QOS_MAP_EGR_EXP_MAX; exp_cnt++) {
            map = &array[idx++];
            bcm_qos_map_t_init(map);

            SOC_PPD_EG_QOS_PHP_REMARK_KEY_clear(&prm_in_qos_php_key);
            
            prm_in_qos_php_key.exp = exp_cnt;
            prm_in_qos_php_key.exp_map_profile = profile_ndx;

            if (flags & BCM_QOS_MAP_IPV6) {
                prm_in_qos_php_key.php_type = SOC_PPD_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6;
            } else {
                prm_in_qos_php_key.php_type = SOC_PPD_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4;
            }

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_php_remark_get(dev_id,
                          &prm_in_qos_php_key,
                          &tmp_dscp_exp));
            map->remark_int_pri = tmp_dscp_exp;
            map->int_pri = exp_cnt;
            
            if ((idx >= num_entries) || (idx >= array_size)) {
                exp_cnt = _BCM_QOS_MAP_EGR_EXP_MAX;                
                break;
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {
        if (opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L3) {
            if (flags & BCM_QOS_MAP_IPV6) {
                for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX; dscp_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);
                    
                    map->dscp = dscp_cnt;
                    
                    sand_rv = soc_ppd_lif_cos_opcode_ipv6_tos_map_get(dev_id, profile_ndx, map->dscp, &action_info);
                    BCM_SAND_IF_ERR_EXIT(sand_rv);
                    if ((action_info.is_packet_valid != is_pkt_valid) ||
                        (action_info.is_qos_only != is_qos_only)) {
                        idx--;
                        continue;
                    }
                    
                    map->port_offset = action_info.ac_offset;
                    
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        dscp_cnt = _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX;
                        break;
                    }
                }
            } 
            else if (flags & BCM_QOS_MAP_IPV4) {
                for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX; dscp_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);
                    
                    map->dscp = dscp_cnt;
                    
                    sand_rv = soc_ppd_lif_cos_opcode_ipv4_tos_map_get(dev_id, profile_ndx, map->dscp, &action_info);
                    BCM_SAND_IF_ERR_EXIT(sand_rv);
                    if ((action_info.is_packet_valid != is_pkt_valid) ||
                        (action_info.is_qos_only != is_qos_only)) {
                        idx--;
                        continue;
                    }
                    
                    map->port_offset = action_info.ac_offset;
                    
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        dscp_cnt = _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX;
                        break;
                    }
                }
            } else {
                for (tc_cnt = 0; tc_cnt < _BCM_QOS_MAP_TC_MAX; dscp_cnt++) {
                    for (dp_cnt = 0; dp_cnt < 4; dp_cnt++) {
                        map = &array[idx++];
                        bcm_qos_map_t_init(map);
                        SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);
                        
                        map->int_pri = tc_cnt;
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(map->color)));
                        
                        sand_rv = soc_ppd_lif_cos_opcode_tc_dp_map_get(dev_id, profile_ndx, map->int_pri, dp_cnt, &action_info);
                        BCM_SAND_IF_ERR_EXIT(sand_rv);
                        if ((action_info.is_packet_valid != is_pkt_valid) ||
                            (action_info.is_qos_only != is_qos_only)) {
                            idx--;
                            continue;
                        }
                        
                        map->port_offset = action_info.ac_offset;
                        
                        if ((idx >= num_entries) || (idx >= array_size)) {
                            dscp_cnt = _BCM_QOS_MAP_TC_MAX;
                            dp_cnt = 4;
                            break;
                        }
                    }
                }
            }
        }
        else if (opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L2) {
            for (pri_cfi_cnt = 0; pri_cfi_cnt <  _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; pri_cfi_cnt++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);
        
                outer_tpid_index = SOC_PPD_LLP_PARSE_TPID_INDEX_TPID1;
                if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                    outer_tpid_index = SOC_PPD_LLP_PARSE_TPID_INDEX_NONE;
                } else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                    outer_tpid_index = SOC_PPD_LLP_PARSE_TPID_INDEX_TPID2;
                }
        
                map->int_pri = pri_cfi_cnt >> 1;
                map->pkt_cfi = pri_cfi_cnt & 1;
                
                sand_rv = soc_ppd_lif_cos_opcode_vlan_tag_map_get(dev_id, profile_ndx, outer_tpid_index, 
                                                              map->int_pri, map->pkt_cfi, &action_info);
                BCM_SAND_IF_ERR_EXIT(sand_rv);
                if ((action_info.is_packet_valid != is_pkt_valid) ||
                    (action_info.is_qos_only != is_qos_only)) {
                    idx--;
                    continue;
                }

                map->port_offset = action_info.ac_offset;

                if ((idx >= num_entries) || (idx >= array_size)) {
                    pri_cfi_cnt = _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX;
                    break;
                }
            }
        } 
        else if (opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP) {
            for (tc_cnt = 0; tc_cnt < _BCM_QOS_MAP_TC_MAX; dscp_cnt++) {
                for (dp_cnt = 0; dp_cnt < 4; dp_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

                    map->int_pri = tc_cnt;
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(map->color)));

                    sand_rv = soc_ppd_lif_cos_opcode_tc_dp_map_get(dev_id, profile_ndx, map->int_pri, dp_cnt, &action_info);
                    BCM_SAND_IF_ERR_EXIT(sand_rv);
                    if ((action_info.is_packet_valid != is_pkt_valid) ||
                        (action_info.is_qos_only != is_qos_only)) {
                        idx--;
                        continue;
                    }

                    map->port_offset = action_info.ac_offset;

                    if ((idx >= num_entries) || (idx >= array_size)) {
                        dscp_cnt = _BCM_QOS_MAP_TC_MAX;
                        dp_cnt = 4;
                        break;
                    }
                }
            }
        } 
    }
    
    *array_count = idx;
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/* Clear an entry from a QoS map */
STATIC int 
_bcm_petra_qos_map_ingress_elsp_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0;
    uint32 profile_ndx = 0;
    SOC_PPD_FRWRD_ILM_GLBL_INFO glbl_info;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_elsp) {
    
    LOG_ERROR(BSL_LS_BCM_QOS,
              (BSL_META_U(unit,
                          "%s: invalid MPLS ELSP profile (%d) out of range\n"),
                          FUNCTION_NAME(), profile_ndx));
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ELSP profile")));
    }
    
    SOC_PPD_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);
    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_get(dev_id, &glbl_info));

    glbl_info.elsp_info.exp_map_tbl[map->exp] = 0;
    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_set(dev_id, &glbl_info));
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_lif_cos_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0, tmp_dp = 0;
    uint32 profile_ndx = 0;
    SOC_PPD_LIF_COS_PROFILE_INFO profile_info;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY map_entry;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY mpls_key;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY ip_key;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY l2_key;
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY tc_dp_key;
    int count1 = 0, count2 = 0, count3 = 0;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
    
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos) {
        
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid lif_cos profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF COS profile")));
    }
    }
    
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&map_entry);
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(&mpls_key);
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(&ip_key);
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(&l2_key);
    SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&tc_dp_key);
    
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if (flags & BCM_QOS_MAP_MPLS) {
    
    mpls_key.in_exp = map->exp;
    map_entry.tc = 0;
    map_entry.dp = 0;
    if (SOC_IS_ARAD(unit)) {
        map_entry.dscp = 0;
    }

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_mpls_label_info_set(dev_id,
                                     profile_ndx,
                                     &mpls_key,
                                     &map_entry));
    /* the entry is not in use:
     * corresponding bit in bitmap ing_lif_cos_mpls_bitmap should be clear.
     */
    SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_MPLS(profile_ndx, map->exp));
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_qos_update_ing_lif_cos_mpls_bitmap_state(unit, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
    
	_DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_ING_MPLS_EXP_MAX, count1);
    if (count1 == 0) {
        _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count2);
        _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count3);

        if ((count2 == 0) && (count3 == 0)) {
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id,
                                  profile_ndx,
                                  &profile_info));
            
            /*
             * 'map_when_ip' and 'map_when_mpls' should have the 
             * same value for Soc_petra-B.
             */
            
            profile_info.map_when_ip = FALSE;
            profile_info.map_when_mpls = FALSE;
            
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id,
                                  profile_ndx,
                                  &profile_info));
        }       
    }    
    } 
    
    if (flags & BCM_QOS_MAP_L3) {
        
    ip_key.ip_type = (flags & BCM_QOS_MAP_IPV6) ? SOC_SAND_PP_IP_TYPE_IPV6 : SOC_SAND_PP_IP_TYPE_IPV4;
    ip_key.tos = map->dscp;
    map_entry.tc = 0;
    map_entry.dp = 0;
    if (SOC_IS_ARAD(unit)) {
        map_entry.dscp = 0;
    }

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_ip_info_set(dev_id,
                                 profile_ndx,
                                 &ip_key,
                                 &map_entry));
    
    if (flags & BCM_QOS_MAP_IPV6) {
        /* the entry is not in use:
         * corresponding bit in bitmap ing_lif_cos_ipv6_bitmap should be clear.
         */        
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));   

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_ing_lif_cos_ipv6_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */        
    }
    else {
        /* the entry is not in use:
         * corresponding bit in bitmap ing_lif_cos_ipv4_bitmap should be clear.
         */        
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_ing_lif_cos_ipv4_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */ 
    }
    
    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count1);
    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count2);
    
    if ((count1 == 0) && (count2 == 0)) {
        _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_ING_MPLS_EXP_MAX, count3);
        
        if (count3 == 0) {
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id,
                                  profile_ndx,
                                  &profile_info));
            
            /*
             * 'map_when_ip' and 'map_when_mpls' should have the 
             * same value for Soc_petra-B.
             */
            
            profile_info.map_when_ip = FALSE;
            profile_info.map_when_mpls = FALSE;
            
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id,
                                  profile_ndx,
                                  &profile_info));
        }
    }
    
    }

    /* Tag exist */
    if ((flags & BCM_QOS_MAP_L2) && !(flags & BCM_QOS_MAP_L2_UNTAGGED)) {

    if (SOC_IS_PETRAB(unit)) {
        l2_key.outer_tpid = 1;
        if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
            l2_key.outer_tpid = 2;
        }
    } else { /* ARAD */
        l2_key.outer_tpid = (flags & BCM_QOS_MAP_L2_INNER_TAG) ? 1:0;
    }

    l2_key.incoming_up = map->pkt_pri;
    l2_key.incoming_dei = map->pkt_cfi;

    map_entry.tc = 0;
    map_entry.dp = 0;

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_l2_info_set(dev_id,
                                 profile_ndx,
                                 &l2_key,
                                 &map_entry));

    if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        /* the entry is not in use:
         * corresponding bit in bitmap ing_lif_cos_l2_inner_bitmap should be clear.
         */        
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_INNER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_ing_lif_cos_l2_inner_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */         
    }
    else {
        /* the entry is not in use:
         * corresponding bit in bitmap ing_lif_cos_l2_outer_bitmap should be clear.
         */        
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_OUTER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_ing_lif_cos_l2_outer_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
    }
    
    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX, count1);
    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX, count2);
    if ((count1 == 0) && (count2 == 0)) {
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id,
                                  profile_ndx,
                                  &profile_info));
            
        profile_info.map_when_l2 = FALSE;
            
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id,
                                  profile_ndx,
                                  &profile_info));
    }
    }

    if (flags & BCM_QOS_MAP_L2_UNTAGGED) {

    tc_dp_key.tc = map->pkt_pri;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->pkt_cfi,&tmp_dp));
    tc_dp_key.dp = tmp_dp;
    map_entry.tc = 0;
    map_entry.dp = 0;

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_tc_dp_info_set(dev_id,
                                 profile_ndx,
                                 &tc_dp_key,
                                 &map_entry));
    /* the entry is not in use:
     * corresponding bit in bitmap ing_lif_cos_l2_untag_bitmap should be clear.
     */    
    SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_UNTAG(profile_ndx, tc_dp_key.tc, tc_dp_key.dp));

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_qos_update_ing_lif_cos_l2_untag_bitmap_state(unit, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */

    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap, profile_ndx, _BCM_QOS_MAP_TC_DP_MAX, count1);

    if (count1 == 0) {
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id,
                                  profile_ndx,
                                  &profile_info));
            
        profile_info.map_from_tc_dp = FALSE;
            
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id,
                                  profile_ndx,
                                  &profile_info));
    }
    }


    if (flags & BCM_QOS_MAP_L3_L2) {

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id, profile_ndx, &profile_info));
            
        profile_info.use_layer2_pcp = FALSE;
            
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id, profile_ndx, &profile_info));
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_pcp_vlan_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if ((flags & BCM_QOS_MAP_L2_VLAN_PCP) == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_L2_VLAN_PCP must be set")));
    }

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) {
    
    LOG_ERROR(BSL_LS_BCM_QOS,
              (BSL_META_U(unit,
                          "%s: invalid ingress vlan pcp profile (%d) out of range\n"),
                          FUNCTION_NAME(), profile_ndx));
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
    }

    if (flags & BCM_QOS_MAP_L2) {
    /* ing_vlan_edit */

    map->pkt_pri = 0;
    map->pkt_cfi = 0;

    if (flags & BCM_QOS_MAP_L2_INNER_TAG) {

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set(dev_id, 
                                    profile_ndx, 
                                    map->int_pri,
                                    map->pkt_pri,
                                    map->pkt_cfi));
        
        /* the entry is not in use:
         * corresponding bit in bitmap ing_pcp_vlan_ctag_bitmap should be clear.
         */
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_ctag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
        
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_ing_pcp_vlan_ctag_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
    } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set(dev_id, 
                                        profile_ndx, 
                                        map->int_pri,
                                        dp,
                                        map->pkt_pri,
                                        map->pkt_cfi));

        /* the entry is not in use:
         * corresponding bit in bitmap ing_pcp_vlan_utag_bitmap should be clear.
         */
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_utag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));

#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_ing_pcp_vlan_utag_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
    } else { /* OUTER TAG */

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set(dev_id, 
                                    profile_ndx, 
                                    map->int_pri,
                                    (dp) ? 1 : 0,
                                    map->pkt_pri,
                                    map->pkt_cfi));

        /* the entry is not in use:
         * corresponding bit in bitmap ing_pcp_vlan_stag_bitmap should be clear.
         */
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_stag_bitmap, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_STAG(profile_ndx, map->int_pri, (dp) ? 1 : 0));

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_ing_pcp_vlan_stag_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */
    }
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_cos_ipv4_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id;
    uint32 sand_rv, opcode_ndx;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if (SHR_BITGET(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, opcode_ndx) == 0) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: OPCODE TYPE not configured \n"), FUNCTION_NAME()));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Opcode Type not configured")));
    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if ((opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L3) == 0) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
        }
    }

    action_info.is_packet_valid = TRUE;
    action_info.is_qos_only = TRUE;
    action_info.ac_offset = 0;

    sand_rv = soc_ppd_lif_cos_opcode_ipv4_tos_map_set(dev_id, opcode_ndx, map->dscp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_ipv6_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id;
    uint32 sand_rv, opcode_ndx;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    if (SHR_BITGET(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, opcode_ndx) == 0) {
        sand_rv = soc_ppd_lif_cos_opcode_types_set(dev_id, opcode_ndx,(SOC_PPD_LIF_COS_OPCODE_TYPE_L3|SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP)); 
        BCM_SAND_IF_ERR_EXIT(sand_rv);
    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if ((opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L3) == 0) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
        }
    }

    action_info.is_packet_valid = TRUE;
    action_info.is_qos_only = TRUE;
    action_info.ac_offset = 0;

    sand_rv = soc_ppd_lif_cos_opcode_ipv6_tos_map_set(dev_id, opcode_ndx, map->dscp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_l2_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
   int rv = BCM_E_NONE;
    int dev_id;
    uint32 opcode_ndx, sand_rv;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPD_LLP_PARSE_TPID_INDEX outer_tpid_ndx;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);

    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if (SHR_BITGET(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, opcode_ndx) == 0) {
        sand_rv = soc_ppd_lif_cos_opcode_types_set(dev_id, opcode_ndx,(SOC_PPD_LIF_COS_OPCODE_TYPE_L2));
        BCM_SAND_IF_ERR_EXIT(sand_rv);
    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if ((opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_L2) == 0) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
        }
    }

    if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        outer_tpid_ndx = SOC_PPD_LLP_PARSE_TPID_INDEX_NONE;
    } else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        outer_tpid_ndx = SOC_PPD_LLP_PARSE_TPID_INDEX_TPID2;
    } else {
        outer_tpid_ndx = SOC_PPD_LLP_PARSE_TPID_INDEX_TPID1;
    }

    action_info.is_packet_valid = TRUE;
    action_info.is_qos_only = TRUE;
    action_info.ac_offset = 0;

    sand_rv = soc_ppd_lif_cos_opcode_vlan_tag_map_set(dev_id, opcode_ndx, outer_tpid_ndx, map->int_pri, map->pkt_cfi, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_tc_dp_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id, dp;
    uint32 opcode_ndx, sand_rv;
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    if (SHR_BITGET(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, opcode_ndx) == 0) {
        sand_rv = soc_ppd_lif_cos_opcode_types_set(dev_id, opcode_ndx,(SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP));
        BCM_SAND_IF_ERR_EXIT(sand_rv);
    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if ((opcode_type & SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP) == 0) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
        }
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    action_info.is_packet_valid = TRUE;
    action_info.is_qos_only = TRUE;
    action_info.ac_offset = 0;

    sand_rv = soc_ppd_lif_cos_opcode_tc_dp_map_set(dev_id, opcode_ndx, map->int_pri, dp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_cos_opcode_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id;
    uint32 opcode_ndx, sand_rv;
    SOC_PPD_LIF_COS_OPCODE_TYPE opcode_type, type;
    BCMDNX_INIT_FUNC_DEFS;
 
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if ((flags & BCM_QOS_MAP_IPV4) || (flags & BCM_QOS_MAP_IPV6)) {
        type = SOC_PPD_LIF_COS_OPCODE_TYPE_L3 | SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP;
    } else if (flags & BCM_QOS_MAP_L2) {
        type = SOC_PPD_LIF_COS_OPCODE_TYPE_L2;
    } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        type = SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP;
    } else {
        type = SOC_PPD_LIF_COS_OPCODE_TYPE_L3 | SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP;
    }

    if (SHR_BITGET(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, opcode_ndx) != 1) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Invalid Map ID (0x%x), opcode not configured \n"), FUNCTION_NAME(), map_id));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Map ID Opcode not set.\n")));
    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if (opcode_type != type) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Map ID does not match flags.\n")));
        }
    }

    if (type & SOC_PPD_LIF_COS_OPCODE_TYPE_L3) {
        if (flags & BCM_QOS_MAP_IPV4) {
            rv = _bcm_petra_qos_map_ingress_cos_ipv4_delete(unit, flags, map, map_id);
        } 
        else if (flags & BCM_QOS_MAP_IPV6) {
            rv = _bcm_petra_qos_map_ingress_cos_ipv6_delete(unit, flags, map, map_id);
        } 
        else {
            rv = _bcm_petra_qos_map_ingress_cos_tc_dp_delete(unit, flags, map, map_id);
        }
    }
    else if (type & SOC_PPD_LIF_COS_OPCODE_TYPE_L2) {   /* L2 */
        rv = _bcm_petra_qos_map_ingress_cos_l2_delete(unit, flags, map, map_id);
    } 
    else if (type & SOC_PPD_LIF_COS_OPCODE_TYPE_TC_DP) { /* TC/DP */
        rv = _bcm_petra_qos_map_ingress_cos_tc_dp_delete(unit, flags, map, map_id);
    }

    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_qos_update_ing_cos_opcode_state(unit, opcode_ndx);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_egress_remark_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    SOC_PPD_EG_QOS_MAP_KEY prm_in_qos_key;
    SOC_PPD_EG_QOS_PARAMS  prm_out_qos_params;
    SOC_PPD_EG_ENCAP_QOS_MAP_KEY prm_in_encap_qos_key;
    SOC_PPD_EG_ENCAP_QOS_PARAMS  prm_out_encap_qos_params;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPD_EG_QOS_MAP_KEY_clear(&prm_in_qos_key);
    SOC_PPD_EG_QOS_PARAMS_clear(&prm_out_qos_params);
    SOC_PPD_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
    SOC_PPD_EG_ENCAP_QOS_PARAMS_clear(&prm_out_encap_qos_params);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if (flags & BCM_QOS_MAP_ENCAP) {
      /* Currently unavailable, Arad  */ 
      if (flags & BCM_QOS_MAP_MPLS) {
          if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
          }
          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }
  
          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri; 
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_MPLS;


          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
          
          prm_out_encap_qos_params.mpls_exp = 0;
          prm_out_encap_qos_params.ip_dscp = 0;

          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));


         /* the entry is not in use:
          * corresponding bit in bitmap egr_remark_encap_mpls_bitmap should be clear.
          */
         SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_MPLS(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));

#ifdef BCM_WARM_BOOT_SUPPORT
         rv = _bcm_dpp_wb_qos_update_egr_remark_encap_mpls_bitmap_state(unit, profile_ndx);
         BCMDNX_IF_ERR_EXIT(rv);                
#endif /* BCM_WARM_BOOT_SUPPORT */        
     }
     else if (flags & BCM_QOS_MAP_IPV6) 
     {
         if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
          }
          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }

          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri; 
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_IPV6;

          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
          prm_out_encap_qos_params.ip_dscp = 0; 
          prm_out_encap_qos_params.mpls_exp = 0;
          
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));


          /* the entry is not in use:
           * corresponding bit in bitmap egr_remark_encap_ipv6_bitmap should be clear.
           */
          SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
          
#ifdef BCM_WARM_BOOT_SUPPORT
          rv = _bcm_dpp_wb_qos_update_egr_remark_encap_ipv6_bitmap_state(unit, profile_ndx);
          BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */
     }
     else if (flags & BCM_QOS_MAP_IPV4) {
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
        }
        if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
        }

        /* internal pri */
        prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri; 
        prm_in_encap_qos_key.remark_profile = profile_ndx;
        prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_IPV4;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
        prm_out_encap_qos_params.ip_dscp= 0; 
        prm_out_encap_qos_params.mpls_exp = 0;
          
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));

        /* the entry is not in use:
         * corresponding bit in bitmap egr_remark_encap_ipv4_bitmap should be clear.
         */        
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_remark_encap_ipv4_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */        
     }
     else if (flags & BCM_QOS_MAP_L2) { 
        /* Only used for ARAD_B0_and_above */
        if (SOC_IS_ARAD_A0(unit) || SOC_IS_PETRAB(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("REMARK flag L2 is not supported")));
        }

        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
        {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
        }

        /* Ethernet is only 4 remark ids */
        if (profile_ndx >= 4) 
        {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range. There are only 4 Remark profile for Ethernet encap remark")));
        }

        if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
        }

        /* internal pri */
        prm_in_encap_qos_key.in_dscp_exp = (uint32)(map->remark_int_pri & 0x7);
        prm_in_encap_qos_key.remark_profile = profile_ndx;
        prm_in_encap_qos_key.pkt_hdr_type = SOC_PPD_PKT_HDR_TYPE_ETH;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp)); /* dp */
        prm_in_encap_qos_key.dp = dp;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
        prm_out_encap_qos_params.ip_dscp= 0; 
        prm_out_encap_qos_params.mpls_exp = 0;
          
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));


        /* the entry is not in use:
         * corresponding bit in bitmap egr_remark_encap_l2_bitmap should be clear.
         */
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_l2_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L2(profile_ndx, map->remark_int_pri, map->color));

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_remark_encap_l2_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */        
     }
    }
    else { /* Not encap header */
        if (flags & BCM_QOS_MAP_MPLS) {
            if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
        
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid remark profile (%d) out of range\n"),
                                      FUNCTION_NAME(), profile_ndx));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
            }
        
            /* internal pri */
            prm_in_qos_key.remark_profile = profile_ndx;
            prm_in_qos_key.dp = dp;
            prm_in_qos_key.in_dscp_exp = (uint32)map->int_pri;
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));
    
            prm_out_qos_params.mpls_exp = 0; /* set to lowest priority on delete */ 
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_set(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));

            /* the entry is not in use:
             * corresponding bit in bitmap egr_remark_mpls_bitmap should be clear.
             */        
            SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_MPLS(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
            
#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_qos_update_egr_remark_mpls_bitmap_state(unit, profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */ 
        }
        else if (flags & BCM_QOS_MAP_L3) {
        
            if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
        
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid remark profile (%d) out of range\n"),
                                      FUNCTION_NAME(), profile_ndx));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
            }
        
            /* internal pri */
            prm_in_qos_key.remark_profile = profile_ndx;
            prm_in_qos_key.dp = dp;
            prm_in_qos_key.in_dscp_exp = (uint32) map->int_pri;
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));
        
            /* IPV6 */
            if (flags & BCM_QOS_MAP_IPV6) {
                prm_out_qos_params.ipv6_tc = 0; /* set to unmapped priority */
            }
            else { /* IPV4 */
                prm_out_qos_params.ipv4_tos = 0; 
            }
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_set(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));

            if (flags & BCM_QOS_MAP_IPV6) {
                /* the entry is not in use:
                 * corresponding bit in bitmap egr_remark_ipv6_bitmap should be clear.
                 */                
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));

#ifdef BCM_WARM_BOOT_SUPPORT
                rv = _bcm_dpp_wb_qos_update_egr_remark_ipv6_bitmap_state(unit, profile_ndx);
                BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */
            } else {
                /* the entry is not in use:
                 * corresponding bit in bitmap egr_remark_ipv4_bitmap should be clear.
                 */            
                SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));

#ifdef BCM_WARM_BOOT_SUPPORT
                rv = _bcm_dpp_wb_qos_update_egr_remark_ipv4_bitmap_state(unit, profile_ndx);
                BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */                
            }       
        } else {
            rv = BCM_E_UNAVAIL;
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_egress_mpls_php_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{  
    int rv = BCM_E_NONE;
    int dev_id = 0;
    uint32 profile_ndx = 0;
    SOC_PPD_EG_QOS_PHP_REMARK_KEY prm_in_php_remark_qos_key;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPD_EG_QOS_PHP_REMARK_KEY_clear(&prm_in_php_remark_qos_key);

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php) 
    {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid MPLS PHP profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS PHPs profile")));
    }


    if (flags & BCM_QOS_MAP_IPV6) {
          
        prm_in_php_remark_qos_key.exp = map->int_pri; /* In-DSCP-EXP before */
        prm_in_php_remark_qos_key.exp_map_profile = profile_ndx;
        prm_in_php_remark_qos_key.php_type = SOC_PPD_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6;
                   
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_php_remark_set(dev_id,
                             &prm_in_php_remark_qos_key,
                             0)); /* Out-DSCP-EXP after */

        /* the entry is not in use:
         * corresponding bit in bitmap egr_mpls_php_ipv6_bitmap should be clear.
         */
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_mpls_php_ipv6_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */
    }
    else if (flags & BCM_QOS_MAP_IPV4) 
    {
        
        prm_in_php_remark_qos_key.exp = map->int_pri; /* In-DSCP-EXP before */
        prm_in_php_remark_qos_key.exp_map_profile = profile_ndx;
        prm_in_php_remark_qos_key.php_type = SOC_PPD_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4;
          
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_php_remark_set(dev_id,
                             &prm_in_php_remark_qos_key,
                             0)); /* Out-DSCP-EXP after */
          
        /* the entry is not in use:
         * corresponding bit in bitmap egr_mpls_php_ipv4_bitmap should be clear.
         */          
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_mpls_php_ipv4_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */
    }     
    else
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS PHPs flags")));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_egress_pcp_vlan_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    int count1 = 0, count2 = 0, count3 = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));
    
    if ((flags & BCM_QOS_MAP_L2_VLAN_PCP) == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_L2_VLAN_PCP must be set")));
    }
    
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) {
    
    LOG_ERROR(BSL_LS_BCM_QOS,
              (BSL_META_U(unit,
                          "%s: invalid egress vlan pcp profile (%d) out of range\n"),
                          FUNCTION_NAME(), profile_ndx));
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
    }
    
    if (flags & BCM_QOS_MAP_L2) {
    /* eg_vlan_edit */
    
    map->pkt_pri = 0;
    map->pkt_cfi = 0;
    
    if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_ctag_set(dev_id, 
                                   profile_ndx, 
                                   map->int_pri, 
                                   map->pkt_pri, 
                                   map->pkt_cfi));

        /* the entry is not in use:
         * corresponding bit in bitmap egr_pcp_vlan_ctag_bitmap should be clear.
         */        
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ctag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_CTAG(profile_ndx, map->int_pri));

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_ctag_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */        
    } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_untagged_set(dev_id, 
                                       profile_ndx, 
                                       map->int_pri, 
                                       dp, 
                                       map->pkt_pri, 
                                       map->pkt_cfi));

        /* the entry is not in use:
         * corresponding bit in bitmap egr_pcp_vlan_utag_bitmap should be clear.
         */
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_utag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
        
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_utag_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */
    } else { /* OUTER TAG */
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_stag_set(dev_id, 
                                   profile_ndx, 
                                   map->int_pri, 
                                   map->color, 
                                   map->pkt_pri, 
                                   map->pkt_cfi));

        /* the entry is not in use:
         * corresponding bit in bitmap egr_pcp_vlan_stag_bitmap should be clear.
         */        
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_stag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_STAG(profile_ndx, map->int_pri, map->color));

#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_stag_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */       
    }
    
    } else if (flags & BCM_QOS_MAP_MPLS) { 
    /* Arad only */ 
       if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable != 1) {
               LOG_ERROR(BSL_LS_BCM_QOS,
                         (BSL_META_U(unit,
                                     "%s: dscp_exp mapping is disabled. \n"), FUNCTION_NAME()));  
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DSCP_EXP mapping is disabled")));
       }
       if (profile_ndx !=  SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id) {
           LOG_ERROR(BSL_LS_BCM_QOS,
                     (BSL_META_U(unit,
                                 "%s: invalid egress vlan pcp profile (%d). profile ndx for dscp_exp mapping is set to (%d). \n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable, profile_ndx)); 
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
       }
       SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
               SOC_PPD_PKT_FRWRD_TYPE_MPLS,  
               map->exp, 
               map->pkt_pri, 
               map->pkt_cfi));
       
        /* the entry is not in use:
         * corresponding bit in bitmap egr_pcp_vlan_mpls_bitmap should be clear.
         */   
        SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_MPLS(profile_ndx, map->exp));
    
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_mpls_bitmap_state(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */

        _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count1);
        if (count1 == 0) {
            _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);
            _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count3);
            if ((count2 == 0) && (count3 == 0)) {
                SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
            }
        }
    } else if (flags & BCM_QOS_MAP_L3) {
    /* Arad only */ 

       if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable != 1) {
               LOG_ERROR(BSL_LS_BCM_QOS,
                         (BSL_META_U(unit,
                                     "%s: dscp_exp mapping is disabled. \n"), FUNCTION_NAME()));  
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DSCP_EXP mapping is disabled")));
       }
       if (profile_ndx !=  SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id) {
           LOG_ERROR(BSL_LS_BCM_QOS,
                     (BSL_META_U(unit,
                                 "%s: invalid egress vlan pcp profile (%d). profile ndx for dscp_exp mapping is set to (%d). \n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable, profile_ndx)); 
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
       }
       if (flags & BCM_QOS_MAP_IPV6) {
           SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                  SOC_PPD_PKT_FRWRD_TYPE_IPV6_UC,  
                  map->dscp, 
                  map->pkt_pri, 
                  map->pkt_cfi));
           SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                  SOC_PPD_PKT_FRWRD_TYPE_IPV6_MC,  
                  map->dscp, 
                  map->pkt_pri, 
                  map->pkt_cfi));

           /* the entry is not in use:
            * corresponding bit in bitmap egr_pcp_vlan_ipv6_bitmap should be clear.
            */ 
           SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));
           
#ifdef BCM_WARM_BOOT_SUPPORT
           rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_ipv6_bitmap_state(unit, profile_ndx);
           BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */ 

           _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count1);
           if (count1 == 0) {
               _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);
               _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count3);
               if ((count2 == 0) && (count3 == 0)) {
                   SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
               }
           }
       } else if (flags & BCM_QOS_MAP_IPV4) {
           SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                  SOC_PPD_PKT_FRWRD_TYPE_IPV4_UC,  
                  map->dscp, 
                  map->pkt_pri, 
                  map->pkt_cfi));
           SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                  SOC_PPD_PKT_FRWRD_TYPE_IPV4_MC,  
                  map->dscp, 
                  map->pkt_pri, 
                  map->pkt_cfi));

           /* the entry is not in use:
            * corresponding bit in bitmap egr_pcp_vlan_ipv4_bitmap should be clear.
            */
           SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));

#ifdef BCM_WARM_BOOT_SUPPORT
           rv = _bcm_dpp_wb_qos_update_egr_pcp_vlan_ipv4_bitmap_state(unit, profile_ndx);
           BCMDNX_IF_ERR_EXIT(rv); 
#endif /* BCM_WARM_BOOT_SUPPORT */ 

           _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count1);
           if (count1 == 0) {
               _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);            
               _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count3);
               if ((count2 == 0) && (count3 == 0)) {
                   SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
               }
           }
       }      
     }
     else 
     {
        rv = BCM_E_UNAVAIL;
     }
    
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_egress_l2_i_tag_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));
        
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag) {
    
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid egress l2 i tag (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid L2 I-TAG profile")));
    }
        
    map->pkt_pri = 0;
    map->pkt_cfi = 0;
    
    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_stag_set(dev_id, 
                               profile_ndx, 
                               map->int_pri, 
                               map->color, 
                               map->pkt_pri, 
                               map->pkt_cfi));

    /* the entry is not in use:
     * corresponding bit in bitmap egr_l2_i_tag_bitmap should be clear.
     */
    SHR_BITCLR(SOC_DPP_STATE(unit)->qos_state->egr_l2_i_tag_bitmap, _DPP_QOS_ENTRY_INDEX_EGR_L2_I_TAG(profile_ndx, map->int_pri, dp));

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_qos_update_egr_l2_i_tag_bitmap_state(unit, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
   
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_88660
STATIC int 
_bcm_petra_qos_map_egress_dscp_exp_marking_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    uint32 profile_ndx = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking) {
    
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid egress DSCP/EXP marking profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid DSCP/EXP profile")));
    }

    map->dscp = 0;
    map->exp = 0;

    rv = _bcm_petra_qos_map_egress_dscp_exp_marking_add(unit, flags, map, map_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_88660 */


static int 
_bcm_petra_qos_map_ingress_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
    rv = _bcm_petra_qos_map_ingress_lif_cos_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)) {
    rv = _bcm_petra_qos_map_ingress_pcp_vlan_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {
    rv = _bcm_petra_qos_map_ingress_elsp_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {
    rv = _bcm_petra_qos_map_ingress_cos_opcode_delete(unit, flags, map, map_id);
    } else {
    rv = BCM_E_UNAVAIL;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_egress_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;



    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_INT_QOS_MAP_IS_REMARK(map_id)) {
    rv = _bcm_petra_qos_map_egress_remark_delete(unit, flags, map, map_id);
    BCMDNX_IF_ERR_EXIT(rv);
    BCM_EXIT;

    } else if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)) {
        rv = _bcm_petra_qos_map_egress_pcp_vlan_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {
        rv = _bcm_petra_qos_map_egress_l2_i_tag_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)) {
        rv = _bcm_petra_qos_map_egress_mpls_php_delete(unit, flags, map, map_id);
#ifdef BCM_88660
    } else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)) {
        rv = _bcm_petra_qos_map_egress_dscp_exp_marking_delete(unit, flags, map, map_id);
#endif /* BCM_88660 */
    } else {
        rv = BCM_E_UNAVAIL;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_UNAVAIL;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);

    _bcm_petra_qos_map_add_params_verify(unit, flags, map, map_id);

    if (BCM_INT_QOS_MAP_IS_INGRESS(map_id))  {
        rv = _bcm_petra_qos_map_ingress_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS(map_id)) {
        rv = _bcm_petra_qos_map_egress_delete(unit, flags, map, map_id);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_qos_ingress_pcp_vlan_gport_map_set(int unit, 
                          bcm_gport_t gport, 
                          int map_id)
{
    int lif_id = 0;
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPD_LIF_ENTRY_INFO *lif_entry_info = NULL;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);

    /* map gport to used LIF */
    rv = _bcm_dpp_gport_to_lif(unit, gport, &lif_id, NULL, NULL, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);

    if(is_local_conf) {
        BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPD_LIF_ENTRY_INFO), "_bcm_petra_qos_ingress_pcp_vlan_gport_map_set.lif_entry_info");
        if (lif_entry_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }

        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, lif_id, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_AC) {
            lif_entry_info->value.ac.ing_edit_info.edit_pcp_profile = map_id;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport LIF is not of type AC")));
        }

        soc_sand_rv = soc_ppd_lif_table_entry_update(dev_id, lif_id, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    BCM_EXIT;
exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_petra_qos_ingress_lif_cos_gport_map_set(int unit, 
                        bcm_gport_t gport, 
                        int map_id)
{
    int lif_id = 0;
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPD_LIF_ENTRY_INFO *lif_entry_info = NULL;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);

    /* map gport to used LIF */
    rv = _bcm_dpp_gport_to_lif(unit, gport, &lif_id, NULL, NULL, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);

    if(is_local_conf) {
        BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPD_LIF_ENTRY_INFO), "_bcm_petra_qos_ingress_lif_cos_gport_map_set.lif_entry_info");
        if (lif_entry_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }

        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, lif_id, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info->type == SOC_PPD_LIF_ENTRY_TYPE_PWE){
            lif_entry_info->value.pwe.cos_profile = map_id;
        }
        else if (lif_entry_info->type == SOC_PPD_LIF_ENTRY_TYPE_AC) {
            if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) || 
            (SOC_DPP_CONFIG(unit)->pp.vmac_enable) || (SOC_DPP_CONFIG(unit)->pp.local_switching_enable)) {
                lif_entry_info->value.ac.cos_profile &= 0x20;
                lif_entry_info->value.ac.cos_profile |= map_id;
            } else {
                lif_entry_info->value.ac.cos_profile = map_id; 
            }
        }
        else if (lif_entry_info->type == SOC_PPD_LIF_ENTRY_TYPE_ISID) {
            lif_entry_info->value.isid.cos_profile = map_id; 
        }
        else if (lif_entry_info->type == SOC_PPD_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
            lif_entry_info->value.ip_term_info.cos_profile = map_id;
        }
        else if (lif_entry_info->type == SOC_PPD_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
            lif_entry_info->value.mpls_term_info.cos_profile = map_id;
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport LIF should be of type PWE, AC or ISID")));
        }
        soc_sand_rv = soc_ppd_lif_table_entry_update(dev_id, lif_id, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {

    }
    BCM_EXIT;
exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_qos_ingress_opcode_gport_map_set(int unit, 
                                            bcm_gport_t gport, 
                                            int map_id)
{
    int lif_id = 0;
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPD_LIF_ENTRY_INFO *lif_entry_info = NULL;
    SOC_PPD_L2_LIF_AC_KEY  ac_key;
    SOC_PPD_L2_LIF_AC_GROUP_INFO *acs_group_info = NULL;
    int rv, opcode_ndx;
    SOC_PPD_LIF_ID base_lif_index = 0;
    bcm_vlan_port_t vlan_port;
    SOC_SAND_SUCCESS_FAILURE success;
    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);

    ac_key.inner_vid = SOC_PPD_LIF_IGNORE_INNER_VID;
    ac_key.outer_vid = SOC_PPD_LIF_IGNORE_OUTER_VID;

    bcm_vlan_port_t_init(&vlan_port);

    /* map gport to used LIF */
    rv = _bcm_dpp_gport_to_lif(unit, gport, &lif_id, NULL, NULL, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);

    if(is_local_conf) {

        BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPD_LIF_ENTRY_INFO), "_bcm_petra_qos_ingress_opcode_gport_map_set.lif_entry_info");
        if (lif_entry_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, lif_id, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info->type & (SOC_PPD_LIF_ENTRY_TYPE_AC|
                                   SOC_PPD_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP |
                                   SOC_PPD_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP)) {
            lif_entry_info->value.ac.cos_profile = opcode_ndx; 

            soc_sand_rv = soc_ppd_lif_table_entry_update(dev_id, lif_id, lif_entry_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = _bcm_dpp_in_lif_ac_match_get(unit, &vlan_port, lif_id);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_gport_fill_ac_key(unit, &vlan_port, BCM_GPORT_VLAN_PORT, &ac_key);
            
            BCMDNX_ALLOC(acs_group_info, sizeof(SOC_PPD_L2_LIF_AC_GROUP_INFO), "_bcm_petra_qos_ingress_opcode_gport_map_set.acs_group_info");
            if (acs_group_info == NULL) {        
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
            }
            soc_sand_rv = soc_ppd_l2_lif_ac_with_cos_get(dev_id, &ac_key, &base_lif_index, acs_group_info, &found);

            if (found == TRUE) {
                acs_group_info->opcode_id = opcode_ndx;

                soc_sand_rv = soc_ppd_l2_lif_ac_with_cos_add(dev_id, &ac_key, base_lif_index, acs_group_info, &success);
                BCMDNX_IF_ERR_EXIT(translate_sand_success_failure(success));
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Lif needs to have vlan group configured\n")));
            }
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Lif needs to be type AC\n")));
        }
    }

    BCM_EXIT;

exit:
    BCM_FREE(lif_entry_info);
    BCM_FREE(acs_group_info);
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_qos_ingress_gport_map_validate(int unit, bcm_gport_t gport, int ing_map)
{
    int rv = BCM_E_NONE;
    int is_local_conf;
    uint32 soc_sand_rv;
    int lif_id = 0;
    int profile_ndx;
    SOC_PPD_LIF_ENTRY_INFO lif_entry_info;
    int vsi;

    BCMDNX_INIT_FUNC_DEFS;

    profile_ndx = BCM_QOS_MAP_PROFILE_GET(ing_map);

    /* validity check: P2P has only 4 bits instead of 6, user can not configure cos profile that is > 16 for PWE P2P */
    rv = _bcm_dpp_gport_to_lif(unit, gport, &lif_id, NULL, NULL, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);

    if(is_local_conf) {
        soc_sand_rv = soc_ppd_lif_table_entry_get(unit, lif_id, &lif_entry_info);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

        if (lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_PWE) {
            rv = _bcm_dpp_in_lif_match_vsi_get(unit, lif_id, &vsi);
            BCMDNX_IF_ERR_EXIT(rv);

            if (((vsi==0) || (vsi==-1)) && (profile_ndx > _BCM_DPP_QOS_PWE_P2P_MAX_PROFILE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For PWE P2P, cos profile value range is 0-15")));
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
_bcm_petra_qos_ingress_gport_map_set(int unit, bcm_gport_t gport, int ing_map)
{
    int rv = BCM_E_NONE;
    int profile_ndx;
    
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_qos_ingress_gport_map_validate(unit, gport, ing_map);
    BCMDNX_IF_ERR_EXIT(rv);

    profile_ndx = BCM_QOS_MAP_PROFILE_GET(ing_map);
    
    if (BCM_INT_QOS_MAP_IS_LIF_COS(ing_map)) {
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid lif_cos profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF COS profile")));
    } else {

      rv = _bcm_petra_qos_ingress_lif_cos_gport_map_set(unit, 
                                gport, 
                                profile_ndx);
      if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: error updating qos ingress map id\n"),FUNCTION_NAME()));
        BCMDNX_IF_ERR_EXIT(rv);
      }       
    }   
    } else if (BCM_INT_QOS_MAP_IS_OPCODE(ing_map)) {
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid opcode profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Opcode index out of range\n")));
    } else {

      rv = _bcm_petra_qos_ingress_opcode_gport_map_set(unit, gport, ing_map);
      if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: error updating qos ingress opcode map id\n"),FUNCTION_NAME()));
        BCMDNX_IF_ERR_EXIT(rv);
      }       
    }   
    } else if (BCM_INT_QOS_MAP_IS_PCP_VLAN(ing_map)) {
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid ing pcp vlan profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
    } else {
        rv = _bcm_petra_qos_ingress_pcp_vlan_gport_map_set(unit, 
                                   gport, 
                                   profile_ndx);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: error updating qos ingress map id\n"),FUNCTION_NAME()));
        BCMDNX_IF_ERR_EXIT(rv);
        }         
    }   
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_qos_egress_gport_map_set(int unit, bcm_gport_t gport, int egr_map)
{
    int rv = BCM_E_NONE;
    int profile_ndx;
    SOC_PPD_EG_AC_INFO eg_ac_info;
    int lif_id = 0;
    int is_local_conf;
    int port_i;
    int tunnel_id, core;
    int is_uniform;
    int sip_val, ttl_val, tos_val;
    int sip_template, ttl_template, tos_template, is_allocated;
    SOC_PPD_PORT soc_ppd_port;
    SOC_PPD_EG_QOS_PORT_INFO eg_qos_port_info;
    SOC_PPD_EG_ENCAP_ENTRY_INFO   encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32         next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32         nof_entries;
    uint32          soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    
    if (BCM_INT_QOS_MAP_IS_REMARK(egr_map)) {

        profile_ndx = BCM_QOS_MAP_PROFILE_GET(egr_map);
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), BCM_QOS_MAP_PROFILE_GET(egr_map)));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
        }
        if (BCM_GPORT_IS_MPLS_PORT(gport)) { 
            rv = BCM_E_NONE;
        }

        if (!BCM_GPORT_IS_TUNNEL(gport)){
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: tunnel_id (%d) is not a legal ipv4 tunnel\n"), FUNCTION_NAME(), gport));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Gport is not an ipv4 tunnel")));
        }

        tunnel_id = BCM_GPORT_TUNNEL_ID_GET(gport);
        if(tunnel_id == _SHR_GPORT_INVALID) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: tunnel_id (%d) is not a legal ipv4 tunnel\n"), FUNCTION_NAME(), gport));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal tunnel_id")));
        } else {

            rv = _bcm_dpp_am_template_ip_tunnel_data_get(unit, tunnel_id, &sip_val, &ttl_val, &tos_val);
            BCMDNX_IF_ERR_EXIT(rv);
            
            is_uniform = SOC_SAND_PP_TOS_IS_UNIFORM_GET(tos_val);

            if (!is_uniform){
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: tunnel_id (%d) is not uniform\n"), FUNCTION_NAME(), BCM_QOS_MAP_PROFILE_GET(egr_map)));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
            }

            SOC_SAND_PP_TOS_SET(tos_val, profile_ndx, is_uniform);
            rv = _bcm_dpp_am_template_ip_tunnel_exchange(unit, gport, 1, sip_val, ttl_val, tos_val, &sip_template, &ttl_template, &tos_template, &is_allocated); /*encode tos val and update*/
            BCMDNX_IF_ERR_EXIT(rv);

            if(is_allocated) {
                /* TOS */
                soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(unit, tos_template, tos_val);
                SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
            }

            /* get the encap entry info so we can update the tos value */
            soc_sand_rv =
            soc_ppd_eg_encap_entry_get(unit,
                                   SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                   tunnel_id, 1,
                                   encap_entry_info, next_eep,
                                   &nof_entries);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if(encap_entry_info[0].entry_type != SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("given Tunnel ID has to point to IP tunnel encapsulation")));
            }
            
            encap_entry_info[0].entry_val.ipv4_encap_info.dest.tos_index = tos_template;
        
            /*return identical encap entry, but with different tos value */
            soc_sand_rv =
                soc_ppd_eg_encap_ipv4_entry_add(unit, tunnel_id, 
                                                                        &(encap_entry_info[0].entry_val.ipv4_encap_info), next_eep[0]);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
            
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(egr_map)) {
        if (BCM_QOS_MAP_PROFILE_GET(egr_map) >= SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid mpls php profile (%d) out of range\n"), FUNCTION_NAME(), BCM_QOS_MAP_PROFILE_GET(egr_map)));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid EGRESS MPLS PHP profile")));
        }

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, gport, 0, &gport_info));

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

            SOC_PPD_EG_QOS_PORT_INFO_clear(&eg_qos_port_info);
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_port_info_get(unit, soc_ppd_port, &eg_qos_port_info));

            eg_qos_port_info.exp_map_profile = BCM_QOS_MAP_PROFILE_GET(egr_map);

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_port_info_set(unit, soc_ppd_port, &eg_qos_port_info));
        }

    }
#ifdef BCM_88660
    else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(egr_map)) {
        if (BCM_QOS_MAP_PROFILE_GET(egr_map) >= SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid DSCP/EXP marking profile (%d) out of range\n"), FUNCTION_NAME(), BCM_QOS_MAP_PROFILE_GET(egr_map)));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid DSCP/EXP marking profile")));
        }

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport, &soc_ppd_port, &core)));
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_port_info_get(unit, soc_ppd_port, &eg_qos_port_info));

        eg_qos_port_info.marking_profile = BCM_QOS_MAP_PROFILE_GET(egr_map);
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_port_info_set(unit, soc_ppd_port, &eg_qos_port_info));

    }
#endif /* BCM_88660 */
    else {
        profile_ndx = BCM_QOS_MAP_PROFILE_GET(egr_map);
    
        if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(egr_map)) {
            if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) {
        
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid egress pcp vlan profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
            } 
            else { 

                /* map gport to used LIF */
                rv = _bcm_dpp_gport_to_lif(unit, gport, NULL, &lif_id, NULL, &is_local_conf);
                BCMDNX_IF_ERR_EXIT(rv);

                if(is_local_conf) {
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_get(unit, lif_id, &eg_ac_info));
            
                    eg_ac_info.edit_info.pcp_profile = profile_ndx;
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_set(unit, lif_id, &eg_ac_info));
                }
            }
        } 
        else {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid profile, must be egress pcp vlan type\n"),FUNCTION_NAME()));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid profile")));
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Attach a QoS map to an object (Gport) */
int 
bcm_petra_qos_port_map_set(int unit, bcm_gport_t port, int ing_map, int egr_map)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    if (ing_map != -1)  {
        rv = _bcm_petra_qos_ingress_gport_map_set(unit, port, ing_map);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (egr_map != -1) {
        rv = _bcm_petra_qos_egress_gport_map_set(unit, port, egr_map);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_qos_ingress_lif_cos_gport_map_get(int unit, 
                         bcm_gport_t gport, 
                         int *map_id)
{
    int lif_id = 0;
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPD_LIF_ENTRY_INFO lif_entry_info;
    SOC_PPD_L2_LIF_AC_KEY  ac_key;
    SOC_PPD_L2_LIF_AC_GROUP_INFO *acs_group_info = NULL;
    int rv;
    SOC_PPD_LIF_ID base_lif_index=0;
    bcm_vlan_port_t vlan_port;
    uint8 found;
    int profile_ndx = -1;

    BCMDNX_INIT_FUNC_DEFS;
    *map_id = -1;

    dev_id = (unit);

    /* map gport to used LIF */
    rv = _bcm_dpp_gport_to_lif(unit, gport, &lif_id, NULL, NULL, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);

    if(is_local_conf) {

        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, lif_id, &lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_PWE){
            profile_ndx = lif_entry_info.value.pwe.cos_profile;         
        }
        else if (lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_AC) {
            if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) || 
            (SOC_DPP_CONFIG(unit)->pp.vmac_enable) || (SOC_DPP_CONFIG(unit)->pp.local_switching_enable)) {
                profile_ndx = lif_entry_info.value.ac.cos_profile & 0x1f;
            } else {
                profile_ndx = lif_entry_info.value.ac.cos_profile;
            }
        }
        else if (lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_ISID) {
            profile_ndx = lif_entry_info.value.isid.cos_profile;
        }
        else if (lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
            profile_ndx = lif_entry_info.value.ip_term_info.cos_profile;
        }
        else if (lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
            profile_ndx = lif_entry_info.value.mpls_term_info.cos_profile;
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport LIF should be of type PWE, AC or ISID")));
        }
    }
    else {
        BCM_EXIT;
    }

    if (SOC_IS_PETRAB(unit) && lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_AC) {
        rv = _bcm_dpp_in_lif_ac_match_get(unit, &vlan_port, lif_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_fill_ac_key(unit, &vlan_port, BCM_GPORT_VLAN_PORT, &ac_key);

        BCMDNX_ALLOC(acs_group_info, sizeof(SOC_PPD_L2_LIF_AC_GROUP_INFO), "_bcm_petra_qos_ingress_opcode_gport_map_set.acs_group_info");
        if (acs_group_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        soc_sand_rv = soc_ppd_l2_lif_ac_with_cos_get(dev_id, &ac_key, &base_lif_index, acs_group_info, &found);
        if ((found == TRUE) && 
            SHR_BITGET(SOC_DPP_STATE(unit)->qos_state->opcode_bmp, acs_group_info->opcode_id)) {
            BCM_INT_QOS_MAP_LIF_COS_SET(*map_id, acs_group_info->opcode_id);
            BCM_EXIT;
        }
    }
    
    if ( (profile_ndx != -1) && (bcm_dpp_am_qos_ing_lif_cos_is_alloced(unit, profile_ndx) == BCM_E_EXISTS)) {
        BCM_INT_QOS_MAP_LIF_COS_SET(*map_id, profile_ndx);
    }
    BCM_EXIT;
exit:
    BCM_FREE(acs_group_info);
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_qos_ingress_pcp_vlan_gport_map_get(int unit, 
                          bcm_gport_t gport, 
                          int *map_id)
{
    int lif_id = 0;
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPD_LIF_ENTRY_INFO lif_entry_info;
    int rv;
    int profile_ndx = -1;

    BCMDNX_INIT_FUNC_DEFS;
    *map_id = -1;

    dev_id = (unit);

    /* map gport to used LIF */
    rv = _bcm_dpp_gport_to_lif(unit, gport, &lif_id, NULL, NULL, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);

    if(is_local_conf) {

        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, lif_id, &lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_AC) {
            profile_ndx = lif_entry_info.value.ac.ing_edit_info.edit_pcp_profile;
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport LIF is not of type AC")));
        }
    }
    if ( (profile_ndx != -1) && (bcm_dpp_am_qos_ing_pcp_vlan_is_alloced(unit, profile_ndx) == BCM_E_EXISTS)) {
        BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(*map_id, profile_ndx);
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * If (flags & BCM_QOS_MAP_INGRESS) then the flags are used to determine which profile should be returned. 
 *  
 */
int 
_bcm_petra_qos_ingress_gport_map_get(int unit, bcm_gport_t gport, int *ing_map, uint32 flags)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (!(flags & BCM_QOS_MAP_INGRESS)) {
      flags = 0;
      /* -1 ingress map indicates user wants LIF COS map */
      if (*ing_map == -1) {
        flags |= BCM_QOS_MAP_L2_VLAN_PCP;
      }
    }

    if (flags & BCM_QOS_MAP_L2_VLAN_PCP) {
        rv = _bcm_petra_qos_ingress_pcp_vlan_gport_map_get(unit, gport, ing_map);
    } else {
        rv = _bcm_petra_qos_ingress_lif_cos_gport_map_get(unit, gport, ing_map);
    }

    BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG_STR("%s: error getting qos ingress map id\n"), FUNCTION_NAME()));

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * If flags & EGRESS then the function uses the flags to determine the profile. 
 * Otherwise the gport is used. 
 *  
 * The profiles are generally only dependant on the gport, however if the port is system/local then 
 * there are two options: MPLS PHP and DSCP/EXP marking. 
 * If flags are present then this is easy. 
 * Otherwise, if a gport has both profiles then the MPLS PHP profile is returned. 
 *  
 * We assume the flags have been validated. 
 */
int 
_bcm_petra_qos_egress_gport_map_get(int unit, bcm_gport_t gport, int *egr_map, uint32 flags)
{
    int profile_ndx = 0; /* only 1 id is allocated for egress SOC_PETRA */
    int rv = BCM_E_NONE, core;
    SOC_PPD_EG_AC_INFO eg_ac_info;
    SOC_PPD_EG_QOS_PORT_INFO eg_qos_port_info;
    int lif_id = 0;
    int is_local_conf;
    int tunnel_id;
    int sip_val, ttl_val, tos_val;
    SOC_PPD_PORT soc_ppd_port;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    /* Set to invalid map */
    *egr_map = -1;
    
    /* if port is system port / local port */
    if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_SYSTEM_PORT(gport) || BCM_GPORT_IS_MODPORT(gport) || (SOC_PORT_VALID(unit,gport))) {
      BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    
      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port, &core)));

      SOC_PPD_EG_QOS_PORT_INFO_clear(&eg_qos_port_info);

      SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_port_info_get(unit, soc_ppd_port, &eg_qos_port_info));

      if (flags & BCM_QOS_MAP_EGRESS) {
        if (flags & BCM_QOS_MAP_MPLS_PHP) {
          BCM_INT_QOS_MAP_EGRESS_MPLS_PHP_SET(*egr_map, eg_qos_port_info.exp_map_profile);
#ifdef BCM_88660_A0
        } else if (SOC_IS_ARADPLUS(unit)) {
          if (flags & BCM_QOS_MAP_L3_L2) {
            BCM_INT_QOS_MAP_EGRESS_DSCP_EXP_MARKING_SET(*egr_map, eg_qos_port_info.marking_profile);
          }
#endif /* BCM_88660_A0 */
        }
      } else {
        BCM_INT_QOS_MAP_EGRESS_MPLS_PHP_SET(*egr_map, eg_qos_port_info.exp_map_profile);
      }
    } else {

        if (BCM_GPORT_IS_TUNNEL(gport)) {
            
            tunnel_id = BCM_GPORT_TUNNEL_ID_GET(gport);

            if(tunnel_id == _SHR_GPORT_INVALID) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: tunnel_id (%d) is not a legal ipv4 tunnel\n"), FUNCTION_NAME(), gport));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal tunnel_id")));
            } 

            rv = _bcm_dpp_am_template_ip_tunnel_data_get(unit, tunnel_id, &sip_val, &ttl_val, &tos_val);
            BCMDNX_IF_ERR_EXIT(rv);
            
            tos_val = SOC_SAND_PP_TOS_VAL_GET(tos_val);
            BCM_INT_QOS_MAP_REMARK_SET(*egr_map, tos_val);

        } else if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            
            /* map gport to used LIF */
            rv = _bcm_dpp_gport_to_lif(unit, gport, NULL, &lif_id, NULL, &is_local_conf);
            BCMDNX_IF_ERR_EXIT(rv);

            if(is_local_conf) {
                  
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_get(unit, lif_id, &eg_ac_info));
                  
                profile_ndx = eg_ac_info.edit_info.pcp_profile;
            }
            BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(*egr_map, profile_ndx);      
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid gport type")));
        }
    }


    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Validate that flags are valid and match the gport type. 
 *  
 */
int
_bcm_petra_qos_port_map_get_validate_flags(int unit, bcm_gport_t gport, uint32 flags)
{
  BCMDNX_INIT_FUNC_DEFS;

  if (flags & BCM_QOS_MAP_INGRESS) {
    /* Always valid */

  } else if (flags & BCM_QOS_MAP_EGRESS) {
    const char *valid_string = NULL;
    uint32 valid_flags = 0;
    if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_SYSTEM_PORT(gport) || BCM_GPORT_IS_MODPORT(gport) || (SOC_PORT_VALID(unit,gport))) {
      valid_string = "only BCM_QOS_MAP_MPLS_PHP is valid";
      valid_flags = BCM_QOS_MAP_MPLS_PHP;

#ifdef BCM_88660_A0
      if (SOC_IS_ARADPLUS(unit)) {
        valid_string = "one of BCM_QOS_MAP_MPLS_PHP and BCM_QOS_MAP_L3_l2 must be specified";
        valid_flags |= BCM_QOS_MAP_L3_L2 ;
      }
#endif /* BCM_88660_A0 */
    } else if (BCM_GPORT_IS_TUNNEL(gport) || BCM_GPORT_IS_VLAN_PORT(gport)) {
      /* Always valid */
      valid_flags = ~0;
    } else {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("%s: Illegal gport.\n"), FUNCTION_NAME()));
    }

    if (!(flags & valid_flags)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("%s: Illegal flags (%s).\n"), FUNCTION_NAME(), valid_string));
    }
  } else {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("%s: Illegal flags (one of BCM_QOS_MAP_INGERSS and BCM_QOS_MAP_EGRESS must be specified).\n"), FUNCTION_NAME()));
  }

exit:
  BCMDNX_FUNC_RETURN;
  
}

/* bcm_petra_qos_port_map_get */
int
bcm_petra_qos_port_map_get(int unit, bcm_gport_t gport, 
                         int *ing_map, int *egr_map)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (ing_map) {
        rv = _bcm_petra_qos_ingress_gport_map_get(unit, gport, ing_map, 0);
    }
    if (egr_map) {
        rv = _bcm_petra_qos_egress_gport_map_get(unit, gport, egr_map, 0);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_qos_port_map_type_get(int unit, bcm_gport_t gport, uint32 flags, int *map_id)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    
    if (!map_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("%s: map_id is NULL\n"),FUNCTION_NAME()));
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_qos_port_map_get_validate_flags(unit, gport, flags));

    if (flags & BCM_QOS_MAP_INGRESS) {
        rv = _bcm_petra_qos_ingress_gport_map_get(unit, gport, map_id, flags);
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        rv = _bcm_petra_qos_egress_gport_map_get(unit, gport, map_id, flags);
    }
    BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG_STR("%s: error getting qos map id\n"),FUNCTION_NAME()));

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_qos_ingress_port_vlan_map_set(int unit,  bcm_port_t port, bcm_vlan_t vid,
                     int ing_map)
{
    int rv = BCM_E_NONE;
    SOC_PPD_L2_LIF_AC_KEY ac_key;
    SOC_PPD_LIF_ID lif_id;
    SOC_PPD_L2_LIF_AC_INFO ac_info;
    uint8 found;
    int profile_ndx, port_i;
    int core;
    SOC_SAND_SUCCESS_FAILURE soc_sand_success;
    SOC_PPD_PORT soc_ppd_port;
    SOC_PPD_PORT_INFO port_info;
    uint32 soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    profile_ndx = BCM_QOS_MAP_PROFILE_GET(ing_map);
    
    if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(ing_map)) {
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) {

            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid ingress pcp vlan profile (%d) out of range\n"),
                                  FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
        }
    } else if (BCM_INT_QOS_MAP_IS_LIF_COS(ing_map)) {
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos) {

            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid ingress lif cos profile (%d) out of range\n"),
                                  FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF COS profile")));
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid map type for this association map_id(0x%08x)\n"),
                              FUNCTION_NAME(), ing_map));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map type should be PCP VLAN or LIF COS")));

    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));    
    
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
    
        SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);
        SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        ac_key.key_type     = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
        ac_key.vlan_domain  = port_info.vlan_domain;
        ac_key.outer_vid    = vid;
        ac_key.inner_vid    = SOC_PPD_LIF_IGNORE_INNER_VID;
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_l2_lif_ac_get(unit, &ac_key, &lif_id, &ac_info, &found));
        
        if (found == FALSE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif AC not found for vid(%d) port(%d)\n"),
                                  FUNCTION_NAME(), vid, port));
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF was not found for these vid and port")));
        }

        if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(ing_map)) {
            ac_info.ing_edit_info.edit_pcp_profile = profile_ndx;
        } else {
            if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) || 
            (SOC_DPP_CONFIG(unit)->pp.vmac_enable) || (SOC_DPP_CONFIG(unit)->pp.local_switching_enable)) {
                ac_info.cos_profile &= 0x20;
                ac_info.cos_profile |= profile_ndx;
            } else {
                ac_info.cos_profile = profile_ndx;
            }
        }

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_l2_lif_ac_add(unit, &ac_key, lif_id, &ac_info, &soc_sand_success));
        
        BCMDNX_IF_ERR_EXIT(translate_sand_success_failure(soc_sand_success));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_qos_egress_port_vlan_map_set(int unit,  bcm_port_t port, bcm_vlan_t vid,
                     int egr_map)
{

    int rv = BCM_E_NONE;
    unsigned int dev_id = 0;
    int profile_ndx, core;
    SOC_PPD_L2_LIF_AC_KEY ac_key;
    SOC_PPD_LIF_ID lif_id;
    SOC_PPD_L2_LIF_AC_INFO ac_info;
    uint8 found;
    int port_i;
    SOC_PPD_EG_AC_INFO eg_ac_info;
    SOC_PPD_PORT soc_ppd_port;
    SOC_PPD_PORT_INFO port_info;
    uint32 soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    profile_ndx = BCM_QOS_MAP_PROFILE_GET(egr_map);
    
    if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(egr_map)) {

        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) {

            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid egress pcp vlan profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
        } 
        else { 
            dev_id = (unit);

            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));

            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

                SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

                soc_sand_rv = soc_ppd_port_info_get(dev_id, soc_ppd_port, &port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                ac_key.key_type     = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
                ac_key.vlan_domain  = port_info.vlan_domain;
                ac_key.outer_vid    = vid;
                ac_key.inner_vid    = SOC_PPD_LIF_IGNORE_INNER_VID;
        
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_l2_lif_ac_get(dev_id, &ac_key, &lif_id, &ac_info, &found));

                if (found == FALSE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: Lif AC not found for vid(%d) port(%d)\n"), FUNCTION_NAME(), vid, port));
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF was not found for these vid and port")));
                }   
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_get(dev_id, lif_id, &eg_ac_info));

                eg_ac_info.edit_info.pcp_profile = profile_ndx;
        
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_set(dev_id, lif_id, &eg_ac_info));
            }
        }
    } 
    else {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid profile, must be egress pcp vlan type\n"),FUNCTION_NAME()));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("profile must be of type PCP VLAN")));
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_petra_qos_port_vlan_map_set */
int
bcm_petra_qos_port_vlan_map_set(int unit,  bcm_port_t port, bcm_vlan_t vid,
                int ing_map, int egr_map)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    if (ing_map != -1)  {
    rv = _bcm_petra_qos_ingress_port_vlan_map_set(unit, port, vid, ing_map);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (egr_map != -1) {
    rv = _bcm_petra_qos_egress_port_vlan_map_set(unit, port, vid, egr_map);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}
int
_bcm_petra_qos_ingress_lif_cos_port_vlan_map_get(int unit, 
                         bcm_port_t port,
                         bcm_vlan_t vid,
                         int *map_id)
{
    int rv = BCM_E_NONE;
    SOC_PPD_L2_LIF_AC_KEY ac_key;
    SOC_PPD_LIF_ID lif_id;
    SOC_PPD_L2_LIF_AC_INFO ac_info;
    uint8 found;
    int profile_ndx=0, port_i, core;
    SOC_PPD_PORT soc_ppd_port;
    SOC_PPD_PORT_INFO port_info;
    uint32 soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));
        
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));
        
        SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);
        SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);

        soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        ac_key.key_type     = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;    
        ac_key.vlan_domain  = port_info.vlan_domain;
        ac_key.outer_vid    = vid;    
        ac_key.inner_vid    = SOC_PPD_LIF_IGNORE_INNER_VID;
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_l2_lif_ac_get(unit, &ac_key, &lif_id, &ac_info, &found));
        
        if (found == FALSE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif AC not found for vid(%d) port(%d)\n"), FUNCTION_NAME(), vid, port));
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF not found for these vid and port")));
        }

        if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) || 
        (SOC_DPP_CONFIG(unit)->pp.vmac_enable) || (SOC_DPP_CONFIG(unit)->pp.local_switching_enable)) {
            profile_ndx = ac_info.cos_profile & 0x1f;
        } else {
            profile_ndx = ac_info.cos_profile;
        }

    }

    if (bcm_dpp_am_qos_ing_lif_cos_is_alloced(unit, profile_ndx) == BCM_E_EXISTS) {
        BCM_INT_QOS_MAP_LIF_COS_SET(*map_id, profile_ndx);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_qos_ingress_pcp_vlan_port_vlan_map_get(int unit, 
                          bcm_port_t port,
                          bcm_vlan_t vlan,
                          int *map_id)
{
    int rv = BCM_E_NONE;
    SOC_PPD_L2_LIF_AC_KEY ac_key;
    SOC_PPD_LIF_ID lif_id;
    SOC_PPD_L2_LIF_AC_INFO ac_info;
    uint8 found=0;
    int profile_ndx=0, port_i;
    int core;
    SOC_PPD_PORT soc_ppd_port;
    SOC_PPD_PORT_INFO port_info;
    uint32 soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));
        
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
    
        SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);
        SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        ac_key.key_type     = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
        ac_key.vlan_domain  = port_info.vlan_domain;
        ac_key.outer_vid    = vlan;
        ac_key.inner_vid    = SOC_PPD_LIF_IGNORE_INNER_VID;
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_l2_lif_ac_get(unit, &ac_key, &lif_id, &ac_info, &found));
        
        if (found == FALSE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif AC not found for vid(%d) port(%d)\n"), FUNCTION_NAME(), vlan, port));
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF not found for these vid and port")));
        }
        
        profile_ndx = ac_info.ing_edit_info.edit_pcp_profile;
    }

    if (bcm_dpp_am_qos_ing_pcp_vlan_is_alloced(unit, profile_ndx) == BCM_E_EXISTS) {
    BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(*map_id, profile_ndx);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_qos_ingress_port_vlan_map_get(int unit, bcm_port_t port, bcm_vlan_t vlan, int *ing_map)
{
    int rv = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    /* -1 ingress map indicates user wants LIF COS map */
    if (*ing_map == -1) {       
    rv = _bcm_petra_qos_ingress_lif_cos_port_vlan_map_get(unit, 
                                  port,
                                  vlan,
                                  ing_map);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: error getting qos ingress map id\n"),FUNCTION_NAME()));
        BCMDNX_IF_ERR_EXIT(rv);
    }         
    } else {
    rv = _bcm_petra_qos_ingress_pcp_vlan_port_vlan_map_get(unit, 
                                   port,
                                   vlan,
                                   ing_map);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: error getting qos ingress map id\n"),FUNCTION_NAME()));
        BCMDNX_IF_ERR_EXIT(rv);
    }         
    }   
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_qos_egress_port_vlan_map_get(int unit, bcm_port_t port, bcm_vlan_t vid, int *egr_map)
{
    int rv = BCM_E_NONE;
    int profile_ndx;
    SOC_PPD_L2_LIF_AC_KEY ac_key;
    SOC_PPD_LIF_ID lif_id;
    SOC_PPD_L2_LIF_AC_INFO ac_info;
    uint8 found;
    int port_i;
    int core;
    SOC_PPD_EG_AC_INFO eg_ac_info;
    SOC_PPD_PORT soc_ppd_port;
    SOC_PPD_PORT_INFO port_info;
    uint32 soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));
    
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
    
        SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        ac_key.key_type     = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
        ac_key.vlan_domain  = port_info.vlan_domain;
        ac_key.outer_vid    = vid;
        ac_key.inner_vid    = SOC_PPD_LIF_IGNORE_INNER_VID;
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_l2_lif_ac_get(unit, &ac_key, &lif_id, &ac_info, &found));
        
        if (found == FALSE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif AC not found for vid(%d) port(%d)\n"),
                                  FUNCTION_NAME(), vid, port));
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF not found for these vid and port")));
        }
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_get(unit, lif_id, &eg_ac_info));

        profile_ndx = eg_ac_info.edit_info.pcp_profile;
        
        if (bcm_dpp_am_qos_egr_pcp_vlan_is_alloced(unit, profile_ndx) == BCM_E_EXISTS) {
            BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(*egr_map, profile_ndx);
        } else {
            rv = BCM_E_NOT_FOUND;   
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/* bcm_petra_qos_port_vlan_map_get */
int
bcm_petra_qos_port_vlan_map_get(int unit, bcm_port_t port, bcm_vlan_t vid,
                              int *ing_map, int *egr_map)
{
    int rv = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    if (ing_map) {
    rv = _bcm_petra_qos_ingress_port_vlan_map_get(unit, port, vid, ing_map);
    }
    if (egr_map) {
    rv = _bcm_petra_qos_egress_port_vlan_map_get(unit, port, vid, egr_map);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_petra_qos_multi_get */
int
bcm_petra_qos_multi_get(int unit, int array_size, int *map_ids_array, 
            int *flags_array, int *array_count)
{
    int rv = BCM_E_NONE;
    int idx = 0, count = 0, map_id = 0, map_flags = 0;
    int entry_count = 0;
#ifdef BCM_88660
    SOC_PPD_LIF_COS_PROFILE_INFO profile_info;
#endif /*BCM_88660*/

    BCMDNX_INIT_FUNC_DEFS;
    if (array_size == 0) {
        /* querying the number of map-ids for storage allocation */
        if (array_count == NULL) {
            rv = BCM_E_PARAM;
        }
        if (BCM_SUCCESS(rv)) {
            count = 0;
            *array_count = 0;
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos; idx++) {
        if(bcm_dpp_am_qos_ing_lif_cos_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan; idx++) {
        if(bcm_dpp_am_qos_ing_pcp_vlan_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_elsp; idx++) {
        if(bcm_dpp_am_qos_ing_elsp_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode; idx++) {
        if(bcm_dpp_am_qos_ing_cos_opcode_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id; idx++) {
        if(bcm_dpp_am_qos_egr_remark_id_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php; idx++) {
        if(bcm_dpp_am_qos_egr_mpls_php_id_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan; idx++) {
        if(bcm_dpp_am_qos_egr_pcp_vlan_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag; idx++) {
        if(bcm_dpp_am_qos_egr_l2_i_tag_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
#ifdef BCM_88660
        if (SOC_IS_ARADPLUS(unit)) {
            for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking; idx++) {
                if(bcm_dpp_am_qos_egr_dscp_exp_marking_is_alloced(unit,idx) == BCM_E_EXISTS) {
                    count++;
                }
            }
        }
#endif /* BCM_88660 */
        *array_count = count;
        }
    } else {
        if ((map_ids_array == NULL) || (flags_array == NULL) || 
            (array_count == NULL)) {
            rv = BCM_E_PARAM;
        }
        if (BCM_SUCCESS(rv)) {
            count = 0;
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_ing_lif_cos_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_LIF_COS_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_mpls_bitmap, idx, _BCM_QOS_MAP_ING_MPLS_EXP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= BCM_QOS_MAP_MPLS;
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv6_bitmap, idx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6);
                    }            
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_ipv4_bitmap, idx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, entry_count);          
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_inner_bitmap, idx, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_INNER_TAG);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_outer_bitmap, idx, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_lif_cos_l2_untag_bitmap, idx, _BCM_QOS_MAP_TC_DP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= BCM_QOS_MAP_L2_UNTAGGED;
                    }

#ifdef BCM_88660
                    if (SOC_IS_ARADPLUS(unit)) {
                        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(unit,
                            idx,
                            &profile_info));
                        if (profile_info.use_layer2_pcp == TRUE) {
                            map_flags |= BCM_QOS_MAP_L3_L2;
                        }
                    }
#endif

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_ing_pcp_vlan_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_ctag_bitmap, idx, _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_INNER_TAG);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_utag_bitmap, idx, _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_UNTAGGED);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->ing_pcp_vlan_stag_bitmap, idx, _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_OUTER_TAG);
                    }

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_elsp) && 
                       (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_ing_elsp_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_MPLS_ELSP_SET(map_id, idx);
                    map_flags = BCM_QOS_MAP_MPLS_ELSP;
                
                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode) && 
                       (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_ing_cos_opcode_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_TYPE_OPCODE_SET(map_id, idx);
                    map_flags = SOC_DPP_STATE(unit)->qos_state->ing_cos_opcode_flags[idx];
                
                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_egr_remark_id_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_REMARK_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_mpls_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_MPLS);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv6_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6);
                    }            
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_ipv4_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX, entry_count);          
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_remark_encap_l2_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_L2_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L2);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_remark_mpls_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= BCM_QOS_MAP_MPLS;
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv6_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_remark_ipv4_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4);
                    }

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_EGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_egr_mpls_php_id_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_EGRESS_MPLS_PHP_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv6_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4);
                    }

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_EGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_egr_pcp_vlan_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ctag_bitmap, idx, _BCM_QOS_MAP_EGR_VLAN_PRI_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_INNER_TAG);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_utag_bitmap, idx, _BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_UNTAGGED);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_stag_bitmap, idx, _BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_mpls_bitmap, idx, _BCM_QOS_MAP_EGR_EXP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= BCM_QOS_MAP_MPLS;
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv6_bitmap, idx, _BCM_QOS_MAP_EGR_DSCP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_pcp_vlan_ipv4_bitmap, idx, _BCM_QOS_MAP_EGR_DSCP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4);
                    }

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_EGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_egr_l2_i_tag_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_EGRESS_L2_I_TAG_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(SOC_DPP_STATE(unit)->qos_state->egr_mpls_php_ipv4_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= BCM_QOS_MAP_MIM_ITAG;
                    }

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_EGRESS);
                    count++;
                }
            }
            
#ifdef BCM_88660
            if (SOC_IS_ARADPLUS(unit)) {
                for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking) && 
                             (count < array_size)); idx++) {
                    if (bcm_dpp_am_qos_egr_dscp_exp_marking_is_alloced(unit, idx) == BCM_E_EXISTS) {
                        BCM_INT_QOS_MAP_EGRESS_DSCP_EXP_MARKING_SET(map_id, idx);

                        map_flags = 0;
                        *(map_ids_array + count) = map_id;
                        *(flags_array + count) = (map_flags | BCM_QOS_MAP_EGRESS);
                        count++;
                    }
                }
            }
#endif /* BCM_88660 */
            *array_count = count;
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

