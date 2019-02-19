/*
 * $Id: qos.h,v 1.12 Broadcom SDK $
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
 * This file contains QOS module definitions internal to the BCM library.
 */

#ifndef _BCM_INT_DPP_QOS_H
#define _BCM_INT_DPP_QOS_H

#include <bcm/qos.h>
#include <shared/bitop.h>

/* QoS State structure */
typedef struct bcm_dpp_qos_state_s {
    int            init;            /* TRUE if port module has been initialized */
    PARSER_HINT_ARR SHR_BITDCL     *ing_lif_cos_mpls_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *ing_lif_cos_ipv4_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *ing_lif_cos_ipv6_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *ing_lif_cos_l2_inner_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *ing_lif_cos_l2_outer_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *ing_lif_cos_l2_untag_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *ing_pcp_vlan_ctag_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *ing_pcp_vlan_utag_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *ing_pcp_vlan_stag_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_remark_encap_mpls_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_remark_encap_ipv6_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_remark_encap_ipv4_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_remark_encap_l2_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_remark_mpls_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_remark_ipv6_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_remark_ipv4_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_mpls_php_ipv6_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_mpls_php_ipv4_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_pcp_vlan_ctag_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_pcp_vlan_utag_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_pcp_vlan_stag_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_pcp_vlan_mpls_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_pcp_vlan_ipv6_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_pcp_vlan_ipv4_bitmap;
    PARSER_HINT_ARR SHR_BITDCL     *egr_l2_i_tag_bitmap;
#ifdef BCM_88660
    PARSER_HINT_ARR SHR_BITDCL     *egr_dscp_exp_marking_bitmap;
#endif /* BCM_88660 */
    PARSER_HINT_ARR int            *ing_cos_opcode_flags;
    int            *egr_pcp_vlan_dscp_exp_profile_id;
    int            *egr_pcp_vlan_dscp_exp_enable;
    int            *egr_remark_encap_enable;
    int 	       *dp_max;
    PARSER_HINT_ARR SHR_BITDCL     *opcode_bmp;
} bcm_dpp_qos_state_t;


#define _BCM_QOS_MAP_ING_MPLS_EXP_MAX        8
#define _BCM_QOS_MAP_ING_L3_DSCP_MAX         256
#define _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX      16
#define _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX   16
#define _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX   8
#define _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX   32
#define _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX   256
#define _BCM_QOS_MAP_ING_COS_OPCODE_TC_DP_MAX 32

#define _BCM_QOS_MAP_EGR_VLAN_PRI_MAX           8
#define _BCM_QOS_MAP_EGR_VLAN_CFI_MAX			2
#define _BCM_QOS_MAP_EGR_DSCP_MAX               256
#define _BCM_QOS_MAP_EGR_EXP_MAX                8

#ifdef BCM_88660
# define _BCM_QOS_MAP_EGR_RESOLVED_DP_MAX        2
/* The max qos profile allowed for use_layer2_pcp */
# define _BCM_QOS_MAP_ING_USE_PCP_MAX_QOS_PROFILE 15
#endif /* BCM_88660 */

#define _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX    8
#define _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX     256
#define _BCM_QOS_MAP_EGR_REMARK_L2_MAX          (8*4)


#define _BCM_QOS_MAP_EXP_MAX            8        
#define _BCM_QOS_MAP_DSCP_MAX           256
#define _BCM_QOS_MAP_PRI_MAX            8
#define _BCM_QOS_MAP_CFI_MAX            4
#define _BCM_QOS_MAP_PRI_CFI_MAX        16
#define _BCM_QOS_MAP_TC_DP_MAX          (_BCM_QOS_MAP_TC_MAX * 4)
#define _BCM_QOS_MAP_TC_MAX             8
#define _BCM_QOS_MAP_UP_MAX             8
#define _BCM_QOS_MAP_PCP_MAX            8
#define _BCM_QOS_MAP_PORT_OFFSET_MAX    8


#define BCM_INT_QOS_MAP_PROFILE_MASK        0xffff
#define BCM_INT_QOS_MAP_PROFILE_SHIFT       0
#define BCM_INT_QOS_MAP_TYPE_SHIFT          16

#define BCM_INT_QOS_MAP_TYPE_INGRESS         0x1000
#define BCM_INT_QOS_MAP_TYPE_EGRESS          0x2000

#define BCM_INT_QOS_MAP_TYPE_PCP_VLAN           0x0001
#define BCM_INT_QOS_MAP_TYPE_LIF_COS            0x0002
#define BCM_INT_QOS_MAP_TYPE_REMARK             0x0004
#define BCM_INT_QOS_MAP_TYPE_ENCAP              0x0008
#define BCM_INT_QOS_MAP_TYPE_MPLS_ELSP          0x0010
#define BCM_INT_QOS_MAP_TYPE_L2_I_TAG           0x0020
#define BCM_INT_QOS_MAP_TYPE_OPCODE             0x0040
#define BCM_INT_QOS_MAP_TYPE_MPLS_PHP           0x0080
#define BCM_INT_QOS_MAP_TYPE_DSCP_EXP_MARKING   0x0100
#define BCM_INT_QOS_MAP_TYPE_OAM_EGRESS_PROFILE 0x200



extern bcm_dpp_qos_state_t *qos_state[SOC_MAX_NUM_DEVICES];


#define BCM_QOS_MAP_PROFILE_GET(map_id)       \
        (((map_id) >> BCM_INT_QOS_MAP_PROFILE_SHIFT) & BCM_INT_QOS_MAP_PROFILE_MASK)

#define BCM_INT_QOS_MAP_IS_INGRESS(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) & BCM_INT_QOS_MAP_TYPE_INGRESS)

#define BCM_INT_QOS_MAP_IS_EGRESS(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) & BCM_INT_QOS_MAP_TYPE_EGRESS)

#define BCM_INT_QOS_MAP_IS_PCP_VLAN(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) & (BCM_INT_QOS_MAP_TYPE_PCP_VLAN))

#define BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_PCP_VLAN | BCM_INT_QOS_MAP_TYPE_INGRESS))

#define BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_PCP_VLAN | BCM_INT_QOS_MAP_TYPE_EGRESS))

#define BCM_INT_QOS_MAP_IS_LIF_COS(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_LIF_COS | BCM_INT_QOS_MAP_TYPE_INGRESS))

#define BCM_INT_QOS_MAP_IS_REMARK(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_REMARK | BCM_INT_QOS_MAP_TYPE_EGRESS))

#define BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_MPLS_PHP | BCM_INT_QOS_MAP_TYPE_EGRESS))

#ifdef BCM_88660
# define BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_DSCP_EXP_MARKING | BCM_INT_QOS_MAP_TYPE_EGRESS))
#endif /* BCM_88660 */

#define BCM_INT_QOS_MAP_IS_ENCAP(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_ENCAP | BCM_INT_QOS_MAP_TYPE_EGRESS))

#define BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)  \
 (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_L2_I_TAG | BCM_INT_QOS_MAP_TYPE_EGRESS))

#define BCM_INT_QOS_MAP_IS_OPCODE(map_id)  \
 (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_OPCODE | BCM_INT_QOS_MAP_TYPE_INGRESS))

#define BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(map_id, profile_id)	\
  (map_id) = (((BCM_INT_QOS_MAP_TYPE_PCP_VLAN | BCM_INT_QOS_MAP_TYPE_INGRESS) << BCM_INT_QOS_MAP_TYPE_SHIFT) \
	      | (profile_id & BCM_INT_QOS_MAP_PROFILE_MASK))

#define BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(map_id, profile_id)	\
  (map_id) = (((BCM_INT_QOS_MAP_TYPE_PCP_VLAN | BCM_INT_QOS_MAP_TYPE_EGRESS) << BCM_INT_QOS_MAP_TYPE_SHIFT) \
	      | (profile_id & BCM_INT_QOS_MAP_PROFILE_MASK))

#define BCM_INT_QOS_MAP_LIF_COS_SET(map_id, profile_id)	\
  (map_id) = (((BCM_INT_QOS_MAP_TYPE_LIF_COS | BCM_INT_QOS_MAP_TYPE_INGRESS) << BCM_INT_QOS_MAP_TYPE_SHIFT) \
	      | (profile_id & BCM_INT_QOS_MAP_PROFILE_MASK))

#define BCM_INT_QOS_MAP_REMARK_SET(map_id, profile_id)	\
  (map_id) = (((BCM_INT_QOS_MAP_TYPE_REMARK | BCM_INT_QOS_MAP_TYPE_EGRESS) << BCM_INT_QOS_MAP_TYPE_SHIFT) \
              | (profile_id & BCM_INT_QOS_MAP_PROFILE_MASK))

#define BCM_INT_QOS_MAP_EGRESS_MPLS_PHP_SET(map_id, profile_id) \
  (map_id) = (((BCM_INT_QOS_MAP_TYPE_MPLS_PHP | BCM_INT_QOS_MAP_TYPE_EGRESS) << BCM_INT_QOS_MAP_TYPE_SHIFT) \
              | (profile_id & BCM_INT_QOS_MAP_PROFILE_MASK))

#ifdef BCM_88660
#define BCM_INT_QOS_MAP_EGRESS_DSCP_EXP_MARKING_SET(map_id, profile_id) \
  (map_id) = (((BCM_INT_QOS_MAP_TYPE_DSCP_EXP_MARKING | BCM_INT_QOS_MAP_TYPE_EGRESS) << BCM_INT_QOS_MAP_TYPE_SHIFT) \
              | (profile_id & BCM_INT_QOS_MAP_PROFILE_MASK))
#endif


#define BCM_INT_QOS_MAP_EGRESS_L2_I_TAG_SET(map_id, profile_id)	\
  (map_id) = (((BCM_INT_QOS_MAP_TYPE_L2_I_TAG | BCM_INT_QOS_MAP_TYPE_EGRESS) << BCM_INT_QOS_MAP_TYPE_SHIFT) \
	      | (profile_id & BCM_INT_QOS_MAP_PROFILE_MASK))

#define BCM_INT_QOS_MAP_TYPE_OPCODE_SET(map_id, profile_id)	\
  (map_id) = (((BCM_INT_QOS_MAP_TYPE_OPCODE | BCM_INT_QOS_MAP_TYPE_INGRESS) << BCM_INT_QOS_MAP_TYPE_SHIFT) \
	      | (profile_id & BCM_INT_QOS_MAP_PROFILE_MASK))

#define BCM_INT_QOS_MAP_MPLS_ELSP_SET(map_id, profile_id)	\
  (map_id) = (((BCM_INT_QOS_MAP_TYPE_MPLS_ELSP | BCM_INT_QOS_MAP_TYPE_INGRESS) << BCM_INT_QOS_MAP_TYPE_SHIFT) \
	      | (profile_id & BCM_INT_QOS_MAP_PROFILE_MASK))

#define BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)    \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_MPLS_ELSP | BCM_INT_QOS_MAP_TYPE_INGRESS))


#define BCM_INT_QOS_MAP_OAM_EGRESS_PROFILE_SET(map_id, profile_id) \
      (map_id) = (((BCM_INT_QOS_MAP_TYPE_OAM_EGRESS_PROFILE | BCM_INT_QOS_MAP_TYPE_EGRESS) << BCM_INT_QOS_MAP_TYPE_SHIFT) \
	      | (profile_id & BCM_INT_QOS_MAP_PROFILE_MASK))

#define BCM_INT_QOS_MAP_IS_OAM_EGRESS_PROFILE(map_id) \
  (((map_id) >> BCM_INT_QOS_MAP_TYPE_SHIFT) == (BCM_INT_QOS_MAP_TYPE_OAM_EGRESS_PROFILE | BCM_INT_QOS_MAP_TYPE_EGRESS))



#endif 	/* _BCM_INT_DPP_QOS_H */
