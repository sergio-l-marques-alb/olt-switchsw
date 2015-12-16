/*
 * $Id: switch.h,v 1.12 Broadcom SDK $
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
 * Switch Internal header
 */

#ifndef _BCM_INT_DPP_SWITCH_H_
#define _BCM_INT_DPP_SWITCH_H_

#include <bcm_int/petra_dispatch.h>
#include <sal/types.h>
#include <bcm/types.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/l3.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>


#if (defined(BCM_PETRA_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
extern int    warmboot_test_mode[BCM_MAX_NUM_UNITS];
extern int    override_wb_test[BCM_MAX_NUM_UNITS];
extern int    disable_once_wb_test[BCM_MAX_NUM_UNITS];
#endif 

typedef struct {
    int interrupts_event_storm_nominal;
} bcm_dpp_switch_info_t;

int
_bcm_dpp_switch_init(int unit);
int
_bcm_dpp_switch_detach(int unit);

int _bcm_dpp_trill_ethertype_set(int unit, int arg);

/** 
 *  Defines for bcm_petra_switch_l3_protocol_group_*
 */

int 
bcm_dpp_switch_l3_protocol_group_init(int unit);

int
bcm_dpp_switch_l3_protocol_group_get_members_by_group(int unit, uint32 group_id, uint32 group_mask, uint32 *group_members);

int
bcm_dpp_switch_l3_protocol_group_get_group_by_members(int unit, uint32 requested_flags, uint32 *group_id, uint32 *group_mask, uint8 exact_match);

int
bcm_dpp_switch_l3_protocol_group_change_count(int unit, uint32 group_id, uint32 group_mask, int diff);

int 
_bcm_dpp_switch_control_get(int unit, bcm_switch_control_t bcm_type, int *arg);

#define DPP_SWITCH_L3_PROTOCOL_GROUP_MASK_EXACT 0x3

#define DPP_SWITCH_L3_PROTOCOL_GROUP_IPV4_DEFAULT   0X3
#define DPP_SWITCH_L3_PROTOCOL_GROUP_IPV6_DEFAULT   0X1

#define DPP_SWITCH_L3_PROTOCOL_GROUP_LOWEST_VALID  1


/* Verifies required conditions for bcm_switch_l3_protocol_group_set/get (Jericho in multiple mymac mode 2 or above). */
#define DPP_SWITCH_L3_PROTOCOL_GROUP_VERIFY(_unit)                                                               \
    BCM_DPP_UNIT_CHECK(unit);                                                                                    \
    if (!SOC_IS_JERICHO(_unit)) {                                                                                \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API is for jericho and above.")));                            \
    }                                                                                                            \
    if (!_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_PROTOCOL_GROUP_IS_ACTIVE(_unit)) {       \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Multiple mymac mode must be 2 or above to use this api.")));  \
    }

/* Given the api's l3 protocol group flags, returns the soc ppc flags. */
#define DPP_SWITCH_L3_PROTOCOL_GROUP_TRANSLATE_API_FLAGS_TO_INTERNAL(_src, _target)                             \
    (_target) = 0;                                                                                              \
    DPP_TRANSLATE_FLAG(_src, BCM_SWITCH_L3_PROTOCOL_GROUP_IPV4 , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4 );\
    DPP_TRANSLATE_FLAG(_src, BCM_SWITCH_L3_PROTOCOL_GROUP_IPV6 , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV6 );\
    DPP_TRANSLATE_FLAG(_src, BCM_SWITCH_L3_PROTOCOL_GROUP_ARP  , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_ARP  );\
    DPP_TRANSLATE_FLAG(_src, BCM_SWITCH_L3_PROTOCOL_GROUP_MPLS , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MPLS );\
    DPP_TRANSLATE_FLAG(_src, BCM_SWITCH_L3_PROTOCOL_GROUP_MIM  , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MIM  );\
    DPP_TRANSLATE_FLAG(_src, BCM_SWITCH_L3_PROTOCOL_GROUP_TRILL, _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_TRILL);\
    DPP_TRANSLATE_FLAG(_src, BCM_SWITCH_L3_PROTOCOL_GROUP_FCOE , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_FCOE );


/* Given the soc ppc l3 protocol group flags, returns the api flags. */
#define DPP_SWITCH_L3_PROTOCOL_GROUP_TRANSLATE_INTERNAL_FLAGS_TO_API(_src, _target)                             \
    (_target) = 0;                                                                                              \
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4 , _target, BCM_SWITCH_L3_PROTOCOL_GROUP_IPV4 );\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV6 , _target, BCM_SWITCH_L3_PROTOCOL_GROUP_IPV6 );\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_ARP  , _target, BCM_SWITCH_L3_PROTOCOL_GROUP_ARP  );\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MPLS , _target, BCM_SWITCH_L3_PROTOCOL_GROUP_MPLS );\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MIM  , _target, BCM_SWITCH_L3_PROTOCOL_GROUP_MIM  );\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_TRILL, _target, BCM_SWITCH_L3_PROTOCOL_GROUP_TRILL);\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_FCOE , _target, BCM_SWITCH_L3_PROTOCOL_GROUP_FCOE );

#endif /* _BCM_INT_DPP_SWITCH_H_ */


