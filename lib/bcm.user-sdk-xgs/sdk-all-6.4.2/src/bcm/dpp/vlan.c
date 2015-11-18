/*
 * $Id: vlan.c,v 1.236 Broadcom SDK $
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
 * Soc_petra-B VLAN Implementation
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_VLAN

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <sal/core/alloc.h>

#include <soc/drv.h>
#include <soc/dpp/mbcm.h>

#include <bcm_int/control.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm/error.h>
#include "bcm_int/common/debug.h"
#include <bcm/debug.h>
#include <bcm/vlan.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/dpp/stg.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/dpp/mim.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/failover.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/dpp/wb_db_vlan.h>
#endif /* def BCM_WARM_BOOT_SUPPORT */
#include <bcm_int/dpp/wb_db_port.h>

#include <soc/dpp/PPD/ppd_api_llp_filter.h>
#include <soc/dpp/PPD/ppd_api_llp_vid_assign.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_llp_cos.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/dpp_wb_engine.h>

#include <soc/dcmn/dcmn_wb.h>

#define DPP_VID_ASSIGN_IPV4_SUBNET_INDEX_MAX (15)

/* 
 * VLAN Termination
 */
#define DPP_VLAN_TRANSLATION_OUTER_TAG (0)
#define DPP_VLAN_TRANSLATION_INNER_TAG (1)
#define DPP_VLAN_TRANSLATION_MAX_NOF_TAGS (2)
#define DPP_VLAN_TRANSLATION_MAX_NOF_PORT_PROFILES (8)
#define DPP_VLAN_TRANSLATION_MAX_NOF_TAGS_TO_REMOVE (2)
#define DPP_VLAN_TRANSLATION_MAX_NOF_TAGS_TO_BUILD  (2)

#define DPP_VLAN_TRANSLATION_IVEC_ID_NOP_EDIT_PROFILE       (0)
#define DPP_VLAN_TRANSLATION_IVEC_ID_DEFAULT_EDIT_PROFILE   (8)
#define DPP_VLAN_TRANSLATION_IVEC_ID_DEFAULT_BASE           (8*7)
#define DPP_VLAN_TRANSLATION_IVEC_ID_PWE_TPID_PROFILE_START (0)
#define DPP_VLAN_TRANSLATION_IVEC_ID_PWE_TPID_PROFILE_END   (3)
/* VSI Profile for PMF: use only the 2 MSBs */
#define DPP_VSI_PROFILE_PMF_LSB     (2)   /* bits 3:2 */
#define DPP_VSI_PROFILE_PMF_MASK    (0x3) /* 2 bits */

typedef enum {
  DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_NONE = 0,
  DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_C_TAG = 1,
  DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_TAG_PRIORITY = 2, /* Priority Tag */
  DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_TAG = 3,  
  DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG = 4,
  DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG_PRIORITY = 5,
  DPP_NOF_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMATS = 6
} DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT;


#define IS_INNER_ACTION_SET(action, is_inner) \
    if ((action->dt_inner == bcmVlanActionAdd)      || (action->dt_inner == bcmVlanActionReplace)      ||    \
        (action->dt_inner_prio == bcmVlanActionAdd) || (action->dt_inner_prio == bcmVlanActionReplace) ||    \
        (action->ot_inner == bcmVlanActionAdd)      || (action->ot_inner == bcmVlanActionReplace)      ||    \
        (action->it_inner == bcmVlanActionAdd)      || (action->it_inner == bcmVlanActionReplace)      ||    \
        (action->it_inner_prio == bcmVlanActionAdd) || (action->it_inner_prio == bcmVlanActionReplace) ||    \
        (action->ut_inner == bcmVlanActionAdd)      || (action->ut_inner == bcmVlanActionReplace)) {         \
        is_inner = 1;                                                                                        \
    }

#define IS_OUTER_ACTION_SET(action, is_outer) \
    if ((action->dt_outer == bcmVlanActionAdd)      || (action->dt_outer == bcmVlanActionReplace)      ||    \
        (action->dt_outer_prio == bcmVlanActionAdd) || (action->dt_outer_prio == bcmVlanActionReplace) ||    \
        (action->ot_outer == bcmVlanActionAdd)      || (action->ot_outer == bcmVlanActionReplace)      ||    \
        (action->ot_outer_prio == bcmVlanActionAdd) || (action->ot_outer_prio == bcmVlanActionReplace) ||    \
        (action->it_outer == bcmVlanActionAdd)      || (action->it_outer == bcmVlanActionReplace)      ||    \
        (action->ut_outer == bcmVlanActionAdd)      || (action->ut_outer == bcmVlanActionReplace)) {         \
        is_outer = 1;                                                                                        \
    }


#define   _DPP_EG_CREATE_COMMAND_ACTION_KEY(_edit_profile, _tag_format) \
    ((((_tag_format) & DPP_VLAN_TAG_FORMAT_MASK) << DPP_NOF_EGRESS_VLAN_EDIT_PROFILE_BITS) |   \
     ((_edit_profile) & DPP_EGRESS_VLAN_EDIT_PROFILE_MASK))
#define   _DPP_EG_GET_EDIT_PROFILE_FROM_MAPPING_KEY(_key) \
    ((_key) & DPP_EGRESS_VLAN_EDIT_PROFILE_MASK)
#define   _DPP_EG_GET_TAG_FORMAT_FROM_MAPPING_KEY(_key) \
    (((_key) >> DPP_NOF_EGRESS_VLAN_EDIT_PROFILE_BITS) & DPP_VLAN_TAG_FORMAT_MASK)

static    int _Bcm_dpp_sync_all = 1;
#define DPP_VLAN_DEFAULT_FRWRD_NOF_PORT_PROFILES (2)

STATIC int
_bcm_petra_vlan_translate_action_verify(int unit, int is_ingress, bcm_vlan_action_set_t *action);

STATIC int
_bcm_petra_vlan_control_vlan_verify(int unit, CONST bcm_vlan_control_vlan_t *control);

STATIC int
_bcm_petra_vlan_flooding_per_lif_hw_profile_set(int unit, int port_profile, int lif_profile, SOC_PPD_L2_LIF_DEFAULT_FRWRD_ACTION frwrd_action[SOC_SAND_PP_NOF_ETHERNET_DA_TYPES]);

STATIC int 
_bcm_petra_vlan_mac_action_fill_from_info(int unit, bcm_vlan_action_set_t *action, SOC_PPD_LLP_VID_ASSIGN_MAC_INFO *info);

int 
_bcm_petra_vlan_translate_action_id_set(int unit, uint32 flags, int action_id, bcm_vlan_action_set_t *action, int *forced_tpid_profile);

/*
 *  VLAN working state per unit
 */
_bcm_dpp_vlan_unit_state_t *_bcm_dpp_vlan_unit_state[BCM_UNITS_MAX] = { NULL };


/*
 *   Function
 *      fid_ref_count_fid_add
 *   Purpose
 *     Initialize fids reference count static structure.
 *
 *   Parameters
 *      (IN)  unit         : unit number of the device
 *      (IN)  fid          : fid to add
 *      (OUT) fid_profile  : filled with fid profile pointed to fid
 *      (OUT) first_appear : 1 of entry was not already in array, 0 otherwise
 *   Returns
 *       BCM_E_NONE - success
 *       BCM_E_*    - failure
 */
STATIC int
fid_ref_count_fid_add(int unit, int fid, int *fid_profile, int *first_appear) {
    int index, avail;
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_PPD_VSI_FID_IS_VSID == fid) {
        *fid_profile = SOC_PPD_VSI_FID_IS_VSID;
        *first_appear = FALSE;
        BCM_EXIT;
    }

    for (index = 0, avail = DPP_NOF_SHARED_FIDS;
         index < DPP_NOF_SHARED_FIDS;
         index++) {
        if ((fid == _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].fid) &&
            (0 < _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].ref_count)) {
            /* this one is in use and matches; use it */
            break;
        } else if ((DPP_NOF_SHARED_FIDS == avail) &&
                   (0 == _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].ref_count)) {
            /* this one is empty; remember it for later */
            avail = index;
        }
    } /* for (all fid_ref_count slots) */
    if (index < DPP_NOF_SHARED_FIDS) {
        /* found it already in use */
        *first_appear = FALSE;
    } else if (avail < DPP_NOF_SHARED_FIDS) {
        /* found an available one */
        index = avail;
        *first_appear = TRUE;
        _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].fid = fid;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL,
                         (_BSL_BCM_MSG("unable to allocate fid_ref_count for fid"
                                   " %d: no available slots"),
                          fid));
    }
    /* account for new reference */
    _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].ref_count++;
#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dpp_vlan_fid_ref_count_save(unit, index, NULL, NULL);
#endif /* def BCM_WARM_BOOT_SUPPORT */
    /* return profile (adjusted index) */
    *fid_profile = index + 1;
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      fid_ref_count_fid_remove
 *   Purpose
 *     Initialize fids reference count static structure.
 *
 *   Parameters
 *      (IN)  unit         : unit number of the device
 *      (IN)  fid          : fid to add
 *      (OUT) fid_profile  : filled with fid profile pointed to fid
 *      (OUT) last_appear  : 1 of entry was last reference to fid in array, 0 otherwise
 *   Returns
 *       BCM_E_NONE - success
 *       BCM_E_*    - failure
 */
STATIC int
fid_ref_count_fid_remove(int unit, int fid, int* fid_profile, int *last_appear) {
    int index;
    BCMDNX_INIT_FUNC_DEFS;

    for (index = 0; index < DPP_NOF_SHARED_FIDS; index++) {
        if ((fid == _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].fid) &&
            (0 < _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].ref_count)) {
            /* this one is in use and matches; use it */
            break;
        }
    } /* for (index = 0; index < DPP_NOF_SHARED_FIDS; index++) */
    if (index >= DPP_NOF_SHARED_FIDS) {
        *fid_profile = 0;
        *last_appear = FALSE;
        BCM_EXIT;
    }
    _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].ref_count--;
    if (_bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].ref_count) {
        /* still nonzero references */
        *last_appear = FALSE;
    } else {
        /* zero references now, so must have been last reference */
        *last_appear = TRUE;
        _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].fid = ~0;
    }
    *fid_profile = index + 1;
#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dpp_vlan_fid_ref_count_save(unit, index, NULL, NULL);
#endif /* def BCM_WARM_BOOT_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_vlan_info_vlan_exist_get(int unit,
                                  bcm_vlan_t vlan,
                                  int *exists)
{
    BCMDNX_INIT_FUNC_DEFS;

    *exists = (0 != SHR_BITGET(_bcm_dpp_vlan_unit_state[unit]->vlan_info.vlan_bmp, vlan));

    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_vlan_info_vlan_exist_set(int unit,
                                  bcm_vlan_t vlan,
                                  int exists)
{
    SHR_BITDCL already;
    BCMDNX_INIT_FUNC_DEFS;

    already = SHR_BITGET(_bcm_dpp_vlan_unit_state[unit]->vlan_info.vlan_bmp, vlan);
    if (exists) {
        if (!already) {
            /* did not exist before */
            _bcm_dpp_vlan_unit_state[unit]->vlan_info.count++;
        }
        SHR_BITSET(_bcm_dpp_vlan_unit_state[unit]->vlan_info.vlan_bmp, vlan);
    } else {
        if (already) {
            /* did exist before */
            _bcm_dpp_vlan_unit_state[unit]->vlan_info.count--;
        }
        SHR_BITCLR(_bcm_dpp_vlan_unit_state[unit]->vlan_info.vlan_bmp, vlan);
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    /* if we aren't recovering data, update the warmboot state */
    if (!SOC_WARM_BOOT(unit)) {
        _bcm_dpp_vlan_info_wb_save_all(unit, NULL, NULL);
    }
#endif /* def BCM_WARM_BOOT_SUPPORT */

    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_vlan_default_set_internal
 *   Purpose
 *      Internal impolementation of bcm_petra_vlan_default_set. Behaves differently
 *      during init (creates vlan, adds all ports to it) and not during init (destroys
 *      previous default vlan, removes all ports currently members, create new default
 *      vlan, and add only ports which were previously members of default vlan ). 
 *
 *   Parameters
 *      (IN)  unit         : unit number of the device
 *      (IN)  vid          : default vlan
 *      (IN) is_init       : is called during vlan_init or not
 *   Returns
 *       BCM_E_NONE - success
 *       BCM_E_*    - failure
 */
int 
bcm_petra_vlan_default_set_internal(
    int unit,
    bcm_vlan_t vid,
    int is_init)
{   
    SOC_PPD_LLP_VID_ASSIGN_PORT_INFO port_info;
    bcm_port_t port_i;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_PORT soc_ppd_port_i;
    bcm_vlan_port_t vp;
    int core;
   
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_VLAN_CHK_ID(unit,vid);

    soc_sand_dev_id = (unit);

    bcm_vlan_port_t_init(&vp);

    /* BCM_DPP_VLAN_CHK_ID(unit, vid); */
          
    if (is_init) {
        /*
         *  If this is done during init, add all ports to default vlan.
         *  Otherwise, these pbmps are maintained
         */
        BCM_PBMP_ASSIGN(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp,
                        PBMP_E_ALL(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp,
                        PBMP_LB(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp,
                        PBMP_SFI_ALL(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp,
                        PBMP_RCY_ALL(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp,
                        PBMP_ST_ALL(unit));

        BCM_PBMP_ASSIGN(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp,
                        PBMP_E_ALL(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp,
                        PBMP_CMIC(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp,
                        PBMP_LB(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp,
                        PBMP_SFI_ALL(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp,
                        PBMP_RCY_ALL(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp,
                        PBMP_ST_ALL(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp,
                        PBMP_PON_ALL(unit));
    
#ifdef  BCM_VLAN_NO_DEFAULT_ETHER
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp,
                        PBMP_E_ALL(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp,
                        PBMP_E_ALL(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_ETHER */
#ifdef  BCM_VLAN_NO_DEFAULT_CPU
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp,
                        PBMP_CMIC(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_CPU */
#if defined(BCM_VLAN_NO_AUTO_STACK)
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp,
                        BCM_PBMP_STACK_CURRENT(unit));
        BCM_PBMP_REMOVE(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp,
                        BCM_PBMP_STACK_CURRENT(unit));
#endif  /* BCM_VLAN_NO_AUTO_STACK */
    } else {
        /* Destroy previous default vlan */
        BCMDNX_IF_ERR_EXIT(bcm_petra_vlan_port_remove(unit,
                                                   _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl,
                                                   _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp));
        BCMDNX_IF_ERR_EXIT(bcm_petra_vlan_destroy(unit,
                                               _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl));
    }
    _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl = vid;
#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dpp_vlan_info_wb_save_all(unit, NULL, NULL);
#endif /* dev BCM_WARM_BOOT_SUPPORT */
   
    if (_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl)
    {
        BCMDNX_IF_ERR_EXIT(bcm_petra_vlan_create(unit,
                                              _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl));
    
        BCMDNX_IF_ERR_EXIT(bcm_petra_vlan_port_add(unit,
                                                _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl,
                                                _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp,
                                                _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp));

        port_i = 0;
        BCM_PBMP_ITER(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
            soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(soc_sand_dev_id, soc_ppd_port_i, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
            port_info.pvid = _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl;
    
            soc_sand_rv = soc_ppd_llp_vid_assign_port_info_set(soc_sand_dev_id, soc_ppd_port_i, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


uint32
  _sand_multiset_buffer_get_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_IN  uint8                              *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_OUT uint8                              *data
  )
{
  uint32
    res = SOC_SAND_OK, rv =0;

  soc_sand_initialize_error_word(0,0,&rv);

  res = soc_sand_os_memcpy(
    data,
    buffer + (offset * len),
    len
    );

  if (soc_sand_update_error_code(res, &rv ) != 0) {
      LOG_ERROR(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "error in _sand_multiset_buffer_get_entry()" )));
      return rv;
  }

  return rv;
}


uint32
  _sand_multiset_buffer_set_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_INOUT  uint8                           *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_IN  uint8                              *data
  )
{
  uint32
    res = SOC_SAND_OK, rv =0 ;

  soc_sand_initialize_error_word(0,0,&rv);

  res = soc_sand_os_memcpy(
    buffer + (offset * len),
    data,
    len
    );
  if (soc_sand_update_error_code(res, &rv ) != 0) {
      LOG_ERROR(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,
                            "error in _sand_multiset_buffer_get_entry()" )));
      return rv;
  }

  return rv;
}

/* 
 * Purpose: 
 * Init the VLAN Edit Profile info
 */
void 
_bcm_dpp_vlan_edit_profile_mapping_info_t_init (bcm_dpp_vlan_edit_profile_mapping_info_t *info)
{
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT
    vlan_format;

  sal_memset(info,0x0,sizeof(bcm_dpp_vlan_egress_priority_mapping_table_info_t));
  for (vlan_format = 0; vlan_format < SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS; vlan_format++) {
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(&(info->ivec[vlan_format]));
    info->ivec[vlan_format].outer_tag.vid_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    info->ivec[vlan_format].outer_tag.pcp_dei_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    info->ivec[vlan_format].inner_tag.vid_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    info->ivec[vlan_format].inner_tag.pcp_dei_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
  }
}

/* 
 * Purpose: 
   * Init the VLAN Edit Profile info at egress
 */
void 
_bcm_dpp_vlan_egress_edit_profile_info_t_init (bcm_dpp_vlan_egress_edit_profile_info_t *info)
{
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT
    vlan_format;

  sal_memset(info,0x0,sizeof(bcm_dpp_vlan_egress_edit_profile_info_t));
  for (vlan_format = 0; vlan_format < SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS; vlan_format++) {
    SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO_clear(&(info->evec[vlan_format]));
    info->evec[vlan_format].outer_tag.vid_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    info->evec[vlan_format].outer_tag.pcp_dei_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    info->evec[vlan_format].inner_tag.vid_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    info->evec[vlan_format].inner_tag.pcp_dei_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
  }
}


/* 
 * Purpose: 
 * Init the Egress Priority Table Mapping info
 */
static void 
_bcm_dpp_vlan_egress_priority_mapping_table_info_t_init (bcm_dpp_vlan_egress_priority_mapping_table_info_t *info, int pcp_value)
{
  SOC_SAND_PP_TC tc_ndx;
  SOC_SAND_PP_DP dp_ndx;


  sal_memset(info,0x0,sizeof(bcm_dpp_vlan_egress_priority_mapping_table_info_t));
  for (tc_ndx = 0; tc_ndx < SOC_SAND_PP_TC_MAX; tc_ndx++) {
    for (dp_ndx = 0; dp_ndx < SOC_SAND_PP_DP_MAX; dp_ndx++) {
      info->pcp_dei[tc_ndx][dp_ndx].dei = 0;
      info->pcp_dei[tc_ndx][dp_ndx].pcp = pcp_value;
    }
  }
}

/* Convert the SOC_SAND VLAN Structure to the supported ones */
STATIC void
_bcm_petra_vlan_structure_convert(int unit,
                  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format,
                  DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT *supported_vlan_format,
                  uint8 *is_ivec_to_set,
                  uint8 *is_vlan_format_valid,
                  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT *vlan_format_equivalent)
{
  *supported_vlan_format = DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_NONE;
  *is_ivec_to_set = FALSE;
  *is_vlan_format_valid = TRUE;
  *vlan_format_equivalent = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
     
  switch (vlan_format) {
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE:
    *supported_vlan_format = DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_NONE;
    *is_ivec_to_set = TRUE;
    *vlan_format_equivalent = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
    break; 
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG:
    *supported_vlan_format = DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_C_TAG;
    *is_ivec_to_set = TRUE;
    *vlan_format_equivalent = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG;
    break; 
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG:
    *supported_vlan_format = DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_TAG;
    *is_ivec_to_set = TRUE;
    *vlan_format_equivalent = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    break; 
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG:
    *supported_vlan_format = DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_TAG_PRIORITY;
    *is_ivec_to_set = TRUE;
    *vlan_format_equivalent = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG;
    break; 
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG:
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_S_TAG:
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG:
    *supported_vlan_format = DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG;
    *is_ivec_to_set = FALSE;
    *vlan_format_equivalent = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    break; 
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG:
    *supported_vlan_format = DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG_PRIORITY;
    *is_ivec_to_set = TRUE;
    *vlan_format_equivalent = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG;
    break; 
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG:
    *supported_vlan_format = DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG;
    *is_ivec_to_set = TRUE;
    *vlan_format_equivalent = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    break; 
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_S_TAG:
    *supported_vlan_format = DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG_PRIORITY;
    *is_ivec_to_set = TRUE;
    *vlan_format_equivalent = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_S_TAG;
    break; 
  default:
    *is_vlan_format_valid = FALSE;
    break;
  }
}

int
_bcm_petra_vlan_translate_match_tpid_value_to_index(int unit, int tpid_value, SOC_PPD_LLP_PARSE_TPID_VALUES* tpid_vals, int* tpid_index)
{
    bcm_error_t rv = BCM_E_NONE;
    int index;

    BCMDNX_INIT_FUNC_DEFS;

    for (index = 0; index < SOC_PPD_LLP_PARSE_NOF_TPID_VALS; ++index) {
        if (tpid_value == tpid_vals->tpid_vals[index]) {
            /* Match */
            *tpid_index = index;
            break;
        }
    }

    if (index == SOC_PPD_LLP_PARSE_NOF_TPID_VALS) {
        /* Not found */
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("TPID value not found")));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/* Conversation */
typedef struct
{
    bcm_vlan_action_t outer_tag;
    bcm_vlan_action_t inner_tag;
} _bcm_dpp_vlan_action_bcm;

typedef struct
{
    int nof_tags_to_rmv;
    SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC vid_source_outer;
    SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC vid_source_inner;
    SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC vid_source_eg_outer;
    SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC vid_source_eg_inner;
} _bcm_dpp_vlan_action_ppd;

typedef struct
{
   _bcm_dpp_vlan_action_bcm vlan_action_bcm;
   _bcm_dpp_vlan_action_ppd vlan_action_ppd;
   int is_valid_ing_conf;
   int is_valid_eg_conf;
} _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING;

#define _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING_NOF_ENTRIES (49)
#define _BCM_DPP_VLAN_TRANSLATION_STATIC_AVANCED_MODE_MAPPING_NOF_ENTRIES (81)

/* Advanced mode */
/* outer,inner,to remove,ingress-outer-src,ingress-inner-src,egress-outer-src,egress-inner-src */
static _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING _bcm_petra_vlan_translation_static_mapping_advanced_mode[] = {
{{bcmVlanActionNone,bcmVlanActionNone},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionNone},{1, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionNone},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionNone},{1, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionNone},{1, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionNone},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionNone},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionOuterAdd,bcmVlanActionNone},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionInnerAdd,bcmVlanActionNone},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionNone,bcmVlanActionReplace},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionReplace},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionReplace},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionReplace},{2 ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionReplace},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionOuterAdd,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionInnerAdd,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionAdd},{1, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionAdd},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionAdd},{1, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionAdd},{0, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 0, 1},
{{bcmVlanActionOuterAdd,bcmVlanActionAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionInnerAdd,bcmVlanActionAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionNone,bcmVlanActionDelete},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionDelete},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionDelete},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionDelete},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionDelete},{1, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionDelete},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionDelete},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 0, 1},
{{bcmVlanActionOuterAdd,bcmVlanActionDelete},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionInnerAdd,bcmVlanActionDelete},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionNone,bcmVlanActionCopy},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionCopy},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionCopy},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionCopy},{2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionCopy},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionOuterAdd,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionInnerAdd,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionMappedReplace},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionReplace,bcmVlanActionMappedReplace},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionAdd,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionMappedReplace},{2, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionCopy,bcmVlanActionMappedReplace},{2, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionMappedReplace},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionOuterAdd,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionInnerAdd,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionMappedAdd},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionReplace,bcmVlanActionMappedAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 0, 1},
{{bcmVlanActionAdd,bcmVlanActionMappedAdd},{0, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionDelete,bcmVlanActionMappedAdd},{1, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionCopy,bcmVlanActionMappedAdd},{1, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG}, 0, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionMappedAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionMappedAdd},{0, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionOuterAdd,bcmVlanActionMappedAdd},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionInnerAdd,bcmVlanActionMappedAdd},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionOuterAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionOuterAdd},{1, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionOuterAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionOuterAdd},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionOuterAdd},{1, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionOuterAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionOuterAdd},{0, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 0, 1},
{{bcmVlanActionOuterAdd,bcmVlanActionOuterAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionInnerAdd,bcmVlanActionOuterAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionNone,bcmVlanActionInnerAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionInnerAdd},{1, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionInnerAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionInnerAdd},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionInnerAdd},{1, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionInnerAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionInnerAdd},{0, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG}, 0, 1},
{{bcmVlanActionOuterAdd,bcmVlanActionInnerAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG}, 1, 1},
{{bcmVlanActionInnerAdd,bcmVlanActionInnerAdd},{0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG ,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG}, 1, 1},
};

/* double-tag */
/* outer,inner,to remove,ingress-outer-src,ingress-inner-src,egress-outer-src,egress-inner-src */
CONST STATIC _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING _bcm_petra_vlan_translation_static_mapping_dtag[] = {
{{bcmVlanActionNone,bcmVlanActionNone},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionNone},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionNone},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionNone},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionNone},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionNone},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionNone},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionReplace},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionReplace},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionReplace},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionReplace},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionReplace},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionAdd},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionAdd},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionAdd},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionAdd},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionDelete},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionDelete},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionDelete},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionDelete},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionDelete},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionDelete},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionDelete},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionCopy},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionCopy},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionCopy},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionCopy},{2,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionCopy},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionMappedReplace},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionReplace,bcmVlanActionMappedReplace},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionAdd,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionMappedReplace},{2, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionCopy,bcmVlanActionMappedReplace},{2, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionMappedReplace},{2, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionMappedAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionReplace,bcmVlanActionMappedAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionAdd,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionMappedAdd},{1, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionCopy,bcmVlanActionMappedAdd},{1, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionMappedAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0}

};

/* outer */
/* outer,inner,to remove,ingress-outer-src,ingress-inner-src,egress-outer-src,egress-inner-src */
CONST STATIC _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING _bcm_petra_vlan_translation_static_mapping_outer[] = {
{{bcmVlanActionNone,bcmVlanActionNone},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionNone},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionNone},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionNone},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionNone},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionNone},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1,},
{{bcmVlanActionMappedAdd,bcmVlanActionNone},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionReplace,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionAdd},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionAdd},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionAdd},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionReplace,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionCopy},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionCopy},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionAdd,bcmVlanActionCopy},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionCopy},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionCopy,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionCopy},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionCopy},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionReplace,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionMappedAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionReplace,bcmVlanActionMappedAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionAdd,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionMappedAdd},{1, 0, 0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionCopy,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionMappedAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedAdd,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0}

};

/* inner */
/* outer,inner,to remove,ingress-outer-src,ingress-inner-src,egress-outer-src,egress-inner-src*/
CONST STATIC _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING _bcm_petra_vlan_translation_static_mapping_inner[] = {
{{bcmVlanActionNone,bcmVlanActionNone},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionNone},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionNone},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionNone},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionNone},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionNone},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionNone},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionReplace},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionReplace},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionReplace},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionReplace},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionAdd},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionAdd},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionAdd},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionAdd},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionDelete},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionDelete},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionDelete},{1,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionDelete},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionReplace,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionCopy},{-1,0,0, 0}, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionMappedReplace},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionReplace,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionMappedReplace},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionDelete,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionMappedReplace},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionMappedReplace},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionMappedAdd},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionReplace,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionMappedAdd},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionDelete,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionMappedAdd},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionMappedReplace,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionMappedAdd},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1}

};

/*untagged*/
/*outer,inner,to remove,ingress-outer-src,ingress-inner-src,egress-outer-src,egress-inner-src*/
CONST STATIC _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING _bcm_petra_vlan_translation_static_mapping_untagged[] = {
{{bcmVlanActionNone,bcmVlanActionNone},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionNone},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionNone},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionNone},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionNone},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionNone},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionNone},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionReplace,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionAdd},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionReplace,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionAdd},{0,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO,SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0,SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 1, 1},
{{bcmVlanActionDelete,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1}, 0, 1},
{{bcmVlanActionNone,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionReplace,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionDelete},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionReplace,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionCopy},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionReplace,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionDelete,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionMappedReplace},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionNone,bcmVlanActionMappedAdd},{0, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY}, 0, 1},
{{bcmVlanActionReplace,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionAdd,bcmVlanActionMappedAdd},{0, 0 ,0, SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1},
{{bcmVlanActionDelete,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionCopy,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedReplace,bcmVlanActionMappedAdd},{-1, 0, 0, 0, 0}, 0, 0},
{{bcmVlanActionMappedAdd,bcmVlanActionMappedAdd},{1, 0 ,0, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI, SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_VSI}, 0, 1}

};

/* 
 *
 */
STATIC int
_bcm_petra_vlan_action_to_ppd_edit_profile_info(int unit,
                                                DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT bcm_vlan_fomrat,
                                                bcm_vlan_action_t outer_tag_action,
                                                bcm_vlan_action_t inner_tag_action,
                                                int* is_valid_ing_conf,
                                                int* is_valid_eg_conf,
                                                int* nof_tags_to_rmv,
                                                SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC *vid_source_outer,
                                                SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC *vid_source_inner,
                                                SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC *vid_source_eg_outer,
                                                SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC *vid_source_eg_inner)
{
    CONST _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING *arr_pointer = NULL;
    int index;

    BCMDNX_INIT_FUNC_DEFS;

    *is_valid_ing_conf = FALSE;
    *is_valid_eg_conf = FALSE;
    switch (bcm_vlan_fomrat) {
    case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_NONE:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_untagged;
        break;
    case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_TAG:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_outer;
        break;
    case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_TAG_PRIORITY:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_outer;
        break;         
      case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_C_TAG:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_inner;
        break;         
      case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_dtag;
        break;         
      case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG_PRIORITY:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_dtag;
        break;         
      default:
        /* Nothing To do */
        BCM_EXIT;
    }

    for (index = 0; index < _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING_NOF_ENTRIES; index++) {
        if (arr_pointer[index].vlan_action_bcm.outer_tag == outer_tag_action &&
            arr_pointer[index].vlan_action_bcm.inner_tag == inner_tag_action) {
            /* Match */
            *is_valid_ing_conf = arr_pointer[index].is_valid_ing_conf;
            *is_valid_eg_conf = arr_pointer[index].is_valid_eg_conf;
            *nof_tags_to_rmv = arr_pointer[index].vlan_action_ppd.nof_tags_to_rmv;
            if (SOC_IS_PETRAB(unit)) {
                *vid_source_inner = (arr_pointer[index].vlan_action_ppd.vid_source_inner == SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2) ? 
                    SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO:arr_pointer[index].vlan_action_ppd.vid_source_inner;
            }
            if (SOC_IS_ARAD(unit)) {               
                *vid_source_inner = arr_pointer[index].vlan_action_ppd.vid_source_inner;
            }                       
            *vid_source_outer = arr_pointer[index].vlan_action_ppd.vid_source_outer;
            *vid_source_eg_outer = arr_pointer[index].vlan_action_ppd.vid_source_eg_outer;
            *vid_source_eg_inner = arr_pointer[index].vlan_action_ppd.vid_source_eg_inner;            

            /* 
             * Special Case Petra-B EVE:
             * In order for device to recognize outer-tag, EVE must be invoked even in case tag is unchanged. 
             * In case tag is unchanged, just remove and then add again.
             */
            if (SOC_IS_PETRAB(unit)) {
                /* Outer tag unchanged */
                if (bcm_vlan_fomrat != DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_NONE &&
                    outer_tag_action == bcmVlanActionNone &&
                    inner_tag_action == bcmVlanActionNone) {
                    *nof_tags_to_rmv = 1;
                    *vid_source_eg_outer = SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG;                    
                }

                /* Inner tag unchanged */
                if ((bcm_vlan_fomrat == DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG ||
                    bcm_vlan_fomrat == DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG_PRIORITY) &&
                    outer_tag_action == bcmVlanActionDelete && inner_tag_action == bcmVlanActionNone) {
                    *nof_tags_to_rmv = 2;
                    *vid_source_eg_inner = SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG;
                }
            }
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 *
 */
STATIC int
_bcm_petra_vlan_action_from_ppd_edit_profile_info(int unit,
                                                DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT bcm_vlan_fomrat,
                                                int is_ingress,
                                                int ingress_nof_tags_to_rmv,
                                                SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC vid_source_outer,
                                                SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC vid_source_inner,
                                                int egress_nof_tags_to_rmv,
                                                SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC vid_source_eg_outer,
                                                SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC vid_source_eg_inner,
                                                int* is_valid_conf,
                                                bcm_vlan_action_t *outer_tag_action,
                                                bcm_vlan_action_t *inner_tag_action)
{
    CONST _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING *arr_pointer = NULL;
    int index;

    BCMDNX_INIT_FUNC_DEFS;

    *is_valid_conf = FALSE;
    switch (bcm_vlan_fomrat) {
    case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_NONE:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_untagged;
        break;
    case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_TAG:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_outer;
        break;
    case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_TAG_PRIORITY:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_outer;
        break;         
      case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_C_TAG:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_inner;
        break;         
      case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_dtag;
        break;         
      case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG_PRIORITY:
        arr_pointer = _bcm_petra_vlan_translation_static_mapping_dtag;
        break;         
      default:
        /* Nothing to do */
        BCM_EXIT;
    }

    if (SOC_IS_PETRAB(unit) && is_ingress) {
        vid_source_inner = (vid_source_inner == SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO) ? 
            SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2:vid_source_inner;        
    }
    for (index = 0; index < _BCM_DPP_VLAN_TRANSLATION_STATIC_MAPPING_NOF_ENTRIES; index++) {
        if (is_ingress) {
          if (arr_pointer[index].vlan_action_ppd.nof_tags_to_rmv == ingress_nof_tags_to_rmv &&
              arr_pointer[index].vlan_action_ppd.vid_source_outer == vid_source_outer &&
              arr_pointer[index].vlan_action_ppd.vid_source_inner == vid_source_inner) {
              /* Match */
              if (arr_pointer[index].is_valid_ing_conf) {
                  *is_valid_conf = arr_pointer[index].is_valid_ing_conf;
                  *outer_tag_action = arr_pointer[index].vlan_action_bcm.outer_tag;
                  *inner_tag_action = arr_pointer[index].vlan_action_bcm.inner_tag;
                  BCM_EXIT;
              }
          }
        } else {
          if (arr_pointer[index].vlan_action_ppd.nof_tags_to_rmv == egress_nof_tags_to_rmv &&
              arr_pointer[index].vlan_action_ppd.vid_source_eg_outer == vid_source_eg_outer &&
              arr_pointer[index].vlan_action_ppd.vid_source_eg_inner == vid_source_eg_inner) {
              /* Match */
              if (arr_pointer[index].is_valid_eg_conf) {
                  *is_valid_conf = arr_pointer[index].is_valid_eg_conf;
                  *outer_tag_action = arr_pointer[index].vlan_action_bcm.outer_tag;
                  *inner_tag_action = arr_pointer[index].vlan_action_bcm.inner_tag;
                  BCM_EXIT;
              }
          }
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



/* 
 * Purpose: 
 * Convert the VLAN Action to the VLAN Edit Profile Info
 */
STATIC int
_bcm_petra_vlan_action_to_edit_profile_info_build(int unit,
                          bcm_vlan_action_set_t *action_set,
                          uint32 tpid_profile,
                          int is_ingress,
                          bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_info,
                          bcm_dpp_vlan_egress_edit_profile_info_t *eg_mapping_info)
{   
    bcm_error_t rc = BCM_E_NONE;
    int tag_type;
    int soc_sand_dev_id, nof_tags_to_rmv = 0, soc_sand_rv;
    SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format;
    uint8 is_ivec_to_set, is_vlan_format_valid;
    DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT supported_vlan_format;
    bcm_vlan_action_t action_tag[DPP_VLAN_TRANSLATION_MAX_NOF_TAGS] = {0,0}, pcp_action_tag[DPP_VLAN_TRANSLATION_MAX_NOF_TAGS] = {0,0};
    bcm_vlan_action_t action_pcp;    
    int is_valid_ing_conf, is_valid_eg_conf;
    SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC outer_vid_source = 0, inner_vid_source = 0;
    SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC pcp_dei_source;
    SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC outer_vid_source_eg = 0, inner_vid_source_eg = 0;
    SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC pcp_dei_source_eg;
    SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_equivalent;
    SOC_PPD_LLP_PARSE_TPID_VALUES tpid_vals;
    int tpid_value, tpid_index;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_NULL_CHECK(mapping_info);

    /* Init the mapping profile */
    _bcm_dpp_vlan_edit_profile_mapping_info_t_init (mapping_info);
    _bcm_dpp_vlan_egress_edit_profile_info_t_init (eg_mapping_info);

    SOC_PPD_LLP_PARSE_TPID_VALUES_clear(&tpid_vals);

    /* Retreive TPID index according to global TPID values */
    soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(soc_sand_dev_id, &tpid_vals);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    /* Set the mapping to the IVEC-Id: constant mapping according to the VLAN format */
    for (vlan_format = 0; vlan_format < SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS; vlan_format++) {
        /* Set only the necessary IVEC (the ones to set in the HW) */
        is_ivec_to_set = FALSE;
        is_vlan_format_valid = TRUE;

        _bcm_petra_vlan_structure_convert(unit, vlan_format, &supported_vlan_format, &is_ivec_to_set, &is_vlan_format_valid, &vlan_format_equivalent);

        if (is_ivec_to_set) {
          /* Choose the 2 relevant VLAN Actions for Outer-Tag / Inner-Tag according to the incoming VLAN format */
          switch (supported_vlan_format) {
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_NONE:
            action_tag[0] = action_set->ut_outer;
            action_tag[1] = action_set->ut_inner;
            pcp_action_tag[0] = action_set->ut_outer_pkt_prio;
            pcp_action_tag[1] = action_set->ut_inner_pkt_prio;
            break;
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_TAG:
            action_tag[0] = action_set->ot_outer;
            action_tag[1] = action_set->ot_inner;
            pcp_action_tag[0] = action_set->ot_outer_pkt_prio;
            pcp_action_tag[1] = action_set->ot_inner_pkt_prio;
            break;
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_TAG_PRIORITY:
            action_tag[0] = action_set->ot_outer_prio;
            action_tag[1] = action_set->ot_inner;
            pcp_action_tag[0] = action_set->ot_outer_pkt_prio;
            pcp_action_tag[1] = action_set->ot_inner_pkt_prio;
            break;
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_C_TAG:
            action_tag[0] = action_set->it_outer;
            action_tag[1] = action_set->it_inner;
            pcp_action_tag[0] = action_set->it_outer_pkt_prio;
            pcp_action_tag[1] = action_set->it_inner_pkt_prio;
            break;        
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG:
            action_tag[0] = action_set->dt_outer;
            action_tag[1] = action_set->dt_inner;
            pcp_action_tag[0] = action_set->dt_outer_pkt_prio;
            pcp_action_tag[1] = action_set->dt_inner_pkt_prio;
            break;
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG_PRIORITY:
            action_tag[0] = action_set->dt_outer_prio;
            action_tag[1] = action_set->dt_inner_prio;
            pcp_action_tag[0] = action_set->dt_outer_pkt_prio;
            pcp_action_tag[1] = action_set->dt_inner_pkt_prio;
            break;
          default:
            break;
          }

        rc = _bcm_petra_vlan_action_to_ppd_edit_profile_info(unit,supported_vlan_format,
                                                action_tag[0], action_tag[1],
                                                &is_valid_ing_conf, &is_valid_eg_conf,
                                                &nof_tags_to_rmv, &outer_vid_source, &inner_vid_source,
                                                &outer_vid_source_eg, &inner_vid_source_eg);
        BCMDNX_IF_ERR_EXIT(rc);       

        if ((is_ingress && !is_valid_ing_conf) || (!is_ingress && !is_valid_eg_conf)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("User set incorrect actions")));
        }
        mapping_info->ivec[vlan_format].tags_to_remove = nof_tags_to_rmv;
        eg_mapping_info->evec[vlan_format].tags_to_remove = nof_tags_to_rmv;                   
        mapping_info->ivec[vlan_format].outer_tag.vid_source = outer_vid_source;
        eg_mapping_info->evec[vlan_format].outer_tag.vid_source = outer_vid_source_eg;
        mapping_info->ivec[vlan_format].inner_tag.vid_source = inner_vid_source;
        eg_mapping_info->evec[vlan_format].inner_tag.vid_source = inner_vid_source_eg;

        mapping_info->ivec[vlan_format].tpid_profile = tpid_profile;        
        
        /* Set the IVEC action type for both tags */
        for (tag_type = 0; tag_type < DPP_VLAN_TRANSLATION_MAX_NOF_TAGS; tag_type++) {
          action_pcp = pcp_action_tag[tag_type]; 

          /* Find the PCP-DEI action */
          pcp_dei_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;
          pcp_dei_source_eg = SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;
          switch (action_pcp) {
          case bcmVlanActionNone:
            pcp_dei_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG + tag_type;
            pcp_dei_source_eg = SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG + tag_type;
            break;
          case bcmVlanActionAdd:
          case bcmVlanActionReplace:
            pcp_dei_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NEW;
            pcp_dei_source_eg = SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP;
            break;
          case bcmVlanActionCopy:
            pcp_dei_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG - tag_type;
            pcp_dei_source_eg = SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG - tag_type;
            break;          
          default:
            break; 
          }

          /* Set the structure */
          if (tag_type == DPP_VLAN_TRANSLATION_OUTER_TAG) {            
            mapping_info->ivec[vlan_format].outer_tag.pcp_dei_source = pcp_dei_source;            
            eg_mapping_info->evec[vlan_format].outer_tag.pcp_dei_source = pcp_dei_source_eg;
            /* Set TPID only in case action is not delete */
            if ((outer_vid_source != SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY) && action_set->outer_tpid_action == bcmVlanTpidActionModify) {
              /* Match value to index */
              tpid_value = action_set->outer_tpid;
              rc = _bcm_petra_vlan_translate_match_tpid_value_to_index(unit, tpid_value, &tpid_vals, &tpid_index);
              BCMDNX_IF_ERR_EXIT(rc);                 
            } else {
              tpid_index = 0;
            }
            mapping_info->ivec[vlan_format].outer_tag.tpid_index = tpid_index;
            eg_mapping_info->evec[vlan_format].outer_tag.tpid_index = tpid_index;                        
          }
          else { /* Inner Tag */            
            mapping_info->ivec[vlan_format].inner_tag.pcp_dei_source = pcp_dei_source;
            /* Set TPID only in case action is not delete/empty */
            if ((inner_vid_source != SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY) && action_set->inner_tpid_action == bcmVlanTpidActionModify) {
              /* Match value to index */
              tpid_value = action_set->inner_tpid;
              rc = _bcm_petra_vlan_translate_match_tpid_value_to_index(unit, tpid_value, &tpid_vals, &tpid_index);
              BCMDNX_IF_ERR_EXIT(rc);              
            } else {
              tpid_index = 1;              
            }
            mapping_info->ivec[vlan_format].inner_tag.tpid_index = tpid_index;
            eg_mapping_info->evec[vlan_format].inner_tag.tpid_index = tpid_index;            
            eg_mapping_info->evec[vlan_format].inner_tag.pcp_dei_source = pcp_dei_source_eg;
          }          
        }
      }
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_vlan_translate_match_tpid_index_to_value(int unit, int tpid_index, SOC_PPD_LLP_PARSE_TPID_VALUES* tpid_vals, uint16* tpid_value)
{
    BCMDNX_INIT_FUNC_DEFS;

    *tpid_value = tpid_vals->tpid_vals[tpid_index];

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Purpose: 
 * Convert the VLAN Action from the VLAN Edit Profile Info
 */
STATIC int
_bcm_petra_vlan_action_from_edit_profile_info_parse(int unit,
                          bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_info,
                          bcm_dpp_vlan_egress_edit_profile_info_t *eg_mapping_info,
                          bcm_vlan_action_set_t *action_set,
                          bcm_vlan_action_set_t *action_set_eg)
{   
    bcm_error_t rc = BCM_E_NONE;
    int tag_type;
    int soc_sand_dev_id, soc_sand_rv;
    SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format;
    bcm_vlan_action_t action_tag[DPP_VLAN_TRANSLATION_MAX_NOF_TAGS], action_tag_eg[DPP_VLAN_TRANSLATION_MAX_NOF_TAGS];    
    bcm_vlan_action_t action_pcp[DPP_VLAN_TRANSLATION_MAX_NOF_TAGS],
      action_pcp_eg[DPP_VLAN_TRANSLATION_MAX_NOF_TAGS];
    SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC pcp_dei_source;
    SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC pcp_dei_source_eg;
    SOC_PPD_LLP_PARSE_TPID_VALUES tpid_vals;
    int is_valid_ing_conf, is_valid_eg_conf;
    uint8 is_ivec_to_set, is_vlan_format_valid;
    DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT supported_vlan_format;
    SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_equivalent;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_NULL_CHECK(mapping_info);
    BCMDNX_NULL_CHECK(eg_mapping_info);

    bcm_vlan_action_set_t_init(action_set);
    bcm_vlan_action_set_t_init(action_set_eg);

    SOC_PPD_LLP_PARSE_TPID_VALUES_clear(&tpid_vals);

    soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(soc_sand_dev_id, &tpid_vals);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Get the necessary VLAN Formats to fill the action set */
    for (vlan_format = 0; vlan_format < SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS; vlan_format++) {
        /* Set only the necessary IVEC (the ones to set in the HW) */
        is_ivec_to_set = FALSE;
        is_vlan_format_valid = TRUE;

        _bcm_petra_vlan_structure_convert(unit, vlan_format, &supported_vlan_format, &is_ivec_to_set, &is_vlan_format_valid, &vlan_format_equivalent);

        if (is_ivec_to_set) {
          /* Choose the 2 relevant VLAN Actions for Outer-Tag / Inner-Tag according to the incoming VLAN format */
          rc = _bcm_petra_vlan_action_from_ppd_edit_profile_info(unit,supported_vlan_format, TRUE,
                  mapping_info->ivec[vlan_format].tags_to_remove,
                  mapping_info->ivec[vlan_format].outer_tag.vid_source,
                  mapping_info->ivec[vlan_format].inner_tag.vid_source,
                  eg_mapping_info->evec[vlan_format].tags_to_remove,
                  eg_mapping_info->evec[vlan_format].outer_tag.vid_source,
                  eg_mapping_info->evec[vlan_format].inner_tag.vid_source,
                  &is_valid_ing_conf,
                  &action_tag[0], &action_tag[1]);
          BCMDNX_IF_ERR_EXIT(rc);    
          
          rc = _bcm_petra_vlan_action_from_ppd_edit_profile_info(unit,supported_vlan_format, FALSE,
                  mapping_info->ivec[vlan_format].tags_to_remove,
                  mapping_info->ivec[vlan_format].outer_tag.vid_source,
                  mapping_info->ivec[vlan_format].inner_tag.vid_source,
                  eg_mapping_info->evec[vlan_format].tags_to_remove,
                  eg_mapping_info->evec[vlan_format].outer_tag.vid_source,
                  eg_mapping_info->evec[vlan_format].inner_tag.vid_source,
                  &is_valid_eg_conf,
                  &action_tag_eg[0], &action_tag_eg[1]);
          BCMDNX_IF_ERR_EXIT(rc);   

          if (!(is_valid_ing_conf && is_valid_eg_conf)) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Inconsistency between the PPD action and the BCM action")));
          }

          for (tag_type = 0; tag_type < DPP_VLAN_TRANSLATION_MAX_NOF_TAGS; tag_type++) {
            /* Get the structure params */
            if (tag_type == DPP_VLAN_TRANSLATION_OUTER_TAG) {
              pcp_dei_source = mapping_info->ivec[vlan_format].outer_tag.pcp_dei_source;
              pcp_dei_source_eg = eg_mapping_info->evec[vlan_format].outer_tag.pcp_dei_source;
              /* Take TPID action only in case VID source is not empty (i.e. VID is created from VLAN translation) */
              if (mapping_info->ivec[vlan_format].outer_tag.vid_source != SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY) {
                    action_set->outer_tpid_action = bcmVlanTpidActionModify;
                    rc = _bcm_petra_vlan_translate_match_tpid_index_to_value(unit, mapping_info->ivec[vlan_format].outer_tag.tpid_index, &tpid_vals, &(action_set->outer_tpid));
                    BCMDNX_IF_ERR_EXIT(rc);
              }
              if (eg_mapping_info->evec[vlan_format].outer_tag.vid_source != SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY) {
                    action_set_eg->outer_tpid_action = bcmVlanTpidActionModify;
                    rc = _bcm_petra_vlan_translate_match_tpid_index_to_value(unit, eg_mapping_info->evec[vlan_format].outer_tag.tpid_index, &tpid_vals, &(action_set_eg->outer_tpid));
                    BCMDNX_IF_ERR_EXIT(rc);          
              }
            }
            else { /* Inner Tag */
              pcp_dei_source = mapping_info->ivec[vlan_format].inner_tag.pcp_dei_source;
              pcp_dei_source_eg = eg_mapping_info->evec[vlan_format].inner_tag.pcp_dei_source;
              /* Take TPID action only in case VID source is not empty (i.e. VID is created from VLAN translation) */
              if (mapping_info->ivec[vlan_format].inner_tag.vid_source != SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY) {
                  action_set->inner_tpid_action = bcmVlanTpidActionModify;
                  rc = _bcm_petra_vlan_translate_match_tpid_index_to_value(unit, mapping_info->ivec[vlan_format].inner_tag.tpid_index, &tpid_vals, &(action_set->inner_tpid));
                  BCMDNX_IF_ERR_EXIT(rc);
              }
              if (eg_mapping_info->evec[vlan_format].inner_tag.vid_source != SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY) {
                  action_set_eg->inner_tpid_action = bcmVlanTpidActionModify;
                  rc = _bcm_petra_vlan_translate_match_tpid_index_to_value(unit, eg_mapping_info->evec[vlan_format].inner_tag.tpid_index, &tpid_vals, &(action_set_eg->inner_tpid));
                  BCMDNX_IF_ERR_EXIT(rc);
              }
            }

            /* Convert back the PCP Action */
            switch (pcp_dei_source) {
            case SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE:
              action_pcp[tag_type] = (action_tag[tag_type] == bcmVlanActionDelete) ? bcmVlanActionDelete:bcmVlanActionNone;
              break;
            case SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG:
              action_pcp[tag_type] = (tag_type == DPP_VLAN_TRANSLATION_OUTER_TAG)? bcmVlanActionCopy:bcmVlanActionNone;
              break;
            case SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG:
              action_pcp[tag_type] = (tag_type == DPP_VLAN_TRANSLATION_INNER_TAG)? bcmVlanActionCopy:bcmVlanActionNone;
              break;
            case SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NEW:
              action_pcp[tag_type] = (action_tag[tag_type] == bcmVlanActionAdd) ? bcmVlanActionAdd:bcmVlanActionReplace;
              break;
            default:
              break;
            }

            switch (pcp_dei_source_eg) {
            case SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_NONE:
              action_pcp_eg[tag_type] = (action_tag_eg[tag_type] == bcmVlanActionDelete) ? bcmVlanActionDelete:bcmVlanActionNone;
              break;
            case SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG:
              action_pcp_eg[tag_type] = (tag_type == DPP_VLAN_TRANSLATION_OUTER_TAG)? bcmVlanActionCopy:bcmVlanActionNone;
              break;
            case SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG:
              action_pcp_eg[tag_type] = (tag_type == DPP_VLAN_TRANSLATION_INNER_TAG)? bcmVlanActionCopy:bcmVlanActionNone;
              break;
            case SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP:
              action_pcp_eg[tag_type] = (action_tag_eg[tag_type] == bcmVlanActionAdd) ? bcmVlanActionAdd:bcmVlanActionReplace;
              break;
            default:
              break;
            }       
          }

          switch (supported_vlan_format) {
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_NONE:
            action_set->ut_outer = action_tag[0];
            action_set->ut_inner = action_tag[1];
            action_set_eg->ut_outer = action_tag_eg[0];
            action_set_eg->ut_inner = action_tag_eg[1];
            action_set->ut_outer_pkt_prio = action_pcp[0];
            action_set->ut_inner_pkt_prio = action_pcp[1];
            action_set_eg->ut_outer_pkt_prio = action_pcp_eg[0];
            action_set_eg->ut_inner_pkt_prio = action_pcp_eg[1];
            break;
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_TAG:
            action_set->ot_outer = action_tag[0];
            action_set->ot_inner = action_tag[1];
            action_set_eg->ot_outer = action_tag_eg[0];
            action_set_eg->ot_inner = action_tag_eg[1];
            action_set->ot_outer_pkt_prio = action_pcp[0];
            action_set->ot_inner_pkt_prio = action_pcp[1];
            action_set_eg->ot_outer_pkt_prio = action_pcp_eg[0];
            action_set_eg->ot_inner_pkt_prio = action_pcp_eg[1];
            break;
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_TAG_PRIORITY:
            action_set->ot_outer_prio = action_tag[0];
            action_set->ot_inner = action_tag[1];
            action_set_eg->ot_outer_prio = action_tag_eg[0];
            action_set_eg->ot_inner = action_tag_eg[1];
            action_set->ot_outer_pkt_prio = action_pcp[0];
            action_set->ot_inner_pkt_prio = action_pcp[1];
            action_set_eg->ot_outer_pkt_prio = action_pcp_eg[0];
            action_set_eg->ot_inner_pkt_prio = action_pcp_eg[1];
            break;
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_C_TAG:
            action_set->it_outer = action_tag[0];
            action_set->it_inner = action_tag[1];
            action_set_eg->it_outer = action_tag_eg[0];
            action_set_eg->it_inner = action_tag_eg[1];
            action_set->it_outer_pkt_prio = action_pcp[0];
            action_set->it_inner_pkt_prio = action_pcp[1];
            action_set_eg->it_outer_pkt_prio = action_pcp_eg[0];
            action_set_eg->it_inner_pkt_prio = action_pcp_eg[1];
            break;        
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG:
            action_set->dt_outer = action_tag[0];
            action_set->dt_inner = action_tag[1];
            action_set_eg->dt_outer = action_tag_eg[0];
            action_set_eg->dt_inner = action_tag_eg[1];
            action_set->dt_outer_pkt_prio = action_pcp[0];
            action_set->dt_inner_pkt_prio = action_pcp[1];
            action_set_eg->dt_outer_pkt_prio = action_pcp_eg[0];
            action_set_eg->dt_inner_pkt_prio = action_pcp_eg[1];
            break;
          case DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT_S_C_TAG_PRIORITY:
            action_set->dt_outer_prio = action_tag[0];
            action_set->dt_inner_prio = action_tag[1];
            action_set_eg->dt_outer_prio = action_tag_eg[0];
            action_set_eg->dt_inner_prio = action_tag_eg[1];
            action_set->dt_outer_pkt_prio = action_pcp[0];
            action_set->dt_inner_pkt_prio = action_pcp[1];
            action_set_eg->dt_outer_pkt_prio = action_pcp_eg[0];
            action_set_eg->dt_inner_pkt_prio = action_pcp_eg[1];
            break;
          default:
            break;
          }      
        }
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 * Purpose: 
 * Set in the HW the PCP profile attributes
 */
STATIC int
_bcm_petra_vlan_egress_priority_tables_hw_set(int unit,
                        int pcp_profile,
                        bcm_dpp_vlan_egress_priority_mapping_table_info_t *mapping_info)
{   
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;
  SOC_SAND_PP_PCP_UP tag_up_ndx;
  SOC_SAND_PP_DEI_CFI dei_cfi_ndx;
  SOC_SAND_PP_TC tc_ndx;
  SOC_SAND_PP_DP dp_ndx;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_NULL_CHECK(mapping_info);

  /* Set the untagged table according to the mapping table */
  for (tc_ndx = 0; tc_ndx <= SOC_SAND_PP_TC_MAX; tc_ndx++) {
    for (dp_ndx = 0; dp_ndx <= SOC_SAND_PP_DP_MAX; dp_ndx++) {
      soc_sand_rv = soc_ppd_eg_vlan_edit_pcp_map_untagged_set(soc_sand_dev_id, pcp_profile, tc_ndx, dp_ndx, mapping_info->pcp_dei[tc_ndx][dp_ndx].pcp, mapping_info->pcp_dei[tc_ndx][dp_ndx].dei);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
  }

  /* For the other Egress Priority tables, set according to TC = 0 and DP = 0 */
  for (tag_up_ndx = 0; tag_up_ndx <= SOC_SAND_PP_PCP_UP_MAX; tag_up_ndx++) {
    soc_sand_rv = soc_ppd_eg_vlan_edit_pcp_map_ctag_set(soc_sand_dev_id, pcp_profile, tag_up_ndx, mapping_info->pcp_dei[0][0].pcp, mapping_info->pcp_dei[0][0].dei);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    for (dei_cfi_ndx = 0; dei_cfi_ndx <= SOC_SAND_PP_DEI_CFI_MAX; dei_cfi_ndx++) {
      soc_sand_rv = soc_ppd_eg_vlan_edit_pcp_map_stag_set(soc_sand_dev_id, pcp_profile, tag_up_ndx, dei_cfi_ndx, mapping_info->pcp_dei[0][0].pcp, mapping_info->pcp_dei[0][0].dei);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
  }

  /* Set the PCP profile to be according to the TC-DP table */
  soc_sand_rv = soc_ppd_eg_vlan_edit_pcp_profile_info_set(soc_sand_dev_id, pcp_profile, SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_PCP);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Purpose: 
 * Get the HW VLAN Edit Command
 */
void
_bcm_petra_vlan_edit_command_hw_get(int unit,
                                    int vlan_edit_profile,
                                    SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format,
                                    int *ivec_id,
                                    uint8 *is_ivec_to_set,
                                    uint8 *is_vlan_format_valid)
{   
    DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT supported_vlan_format;
    SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_equivalent;

    /* 
    * The IVEC-Id 0 - 7 is free for other purposes. 
    * Take here the IVEC-Id 0 for VLAN Edit Profile 0,
    * Take here the IVEC-Ids 7 - 13 for VLAN Edit Profile 1,
    * Take here the IVEC-Ids 14 - 20 for VLAN Edit Profile 2, etc 
	* Until VLAN Edit Profile 7 
	* Note: A change here requires also a change in the default value of 
	*       the FHEI IVE size 
    */
    *ivec_id = 7 * vlan_edit_profile;
    *is_ivec_to_set = FALSE;
    *is_vlan_format_valid = TRUE;

    _bcm_petra_vlan_structure_convert(unit, vlan_format, &supported_vlan_format, is_ivec_to_set, is_vlan_format_valid, &vlan_format_equivalent);
    *ivec_id += supported_vlan_format;

    if (vlan_edit_profile == 0) {
        *ivec_id = 0;
    }
}


/* 
 * Purpose: 
 * Return the HW Vlan-Edit-Profle according to the SW profile
 */
uint32
_bcm_petra_vlan_edit_profile_hw_get(int unit,
                                    int vlan_edit_profile)
{   
    return  (vlan_edit_profile == DPP_VLAN_TRANSLATION_IVEC_ID_DEFAULT_EDIT_PROFILE)? 0 : vlan_edit_profile;
}


/* 
 * Purpose: 
 * Set in the HW the VLAN Edit profile attributes
 */
int
_bcm_petra_vlan_edit_profile_info_hw_set(int unit,
                     int vlan_edit_profile,
                     bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_info)
{   
    bcm_error_t rc = BCM_E_NONE;
    int ivec_id;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;
    SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format;
    uint8 is_ivec_to_set, is_vlan_format_valid;
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY command_key;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_NULL_CHECK(mapping_info);

    /* Set the mapping to the IVEC-Id: constant mapping according to the VLAN format */
    for (vlan_format = 0; vlan_format < SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS; vlan_format++) {
      /* Get the IVEC-Id */
      _bcm_petra_vlan_edit_command_hw_get(unit,
                                          vlan_edit_profile,
                                          vlan_format,
                                          &ivec_id,
                                          &is_ivec_to_set,
                                          &is_vlan_format_valid);

      /* Set the mapping to the IVEC-Id, except for the default profile that has no allocated entries */
      if ((is_vlan_format_valid) && (vlan_edit_profile != DPP_VLAN_TRANSLATION_IVEC_ID_DEFAULT_EDIT_PROFILE)) {
        SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY_clear(&command_key);
        command_key.tag_format = vlan_format;
        command_key.edit_profile = vlan_edit_profile;
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_command_id_set(soc_sand_dev_id, &command_key, ivec_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }

      /* Set the IVEC to the HW if necessary */
      if (is_ivec_to_set) {
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_command_info_set(soc_sand_dev_id, ivec_id, &(mapping_info->ivec[vlan_format]));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_vlan_edit_profile_info_hw_get(int unit,
                     int vlan_edit_profile,
                     bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_info)
{   
    bcm_error_t rc = BCM_E_NONE;
    int ivec_id;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format;
  uint8 is_ivec_to_set, is_vlan_format_valid;
  DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT supported_vlan_format;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_equivalent;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_NULL_CHECK(mapping_info);
    _bcm_dpp_vlan_edit_profile_mapping_info_t_init(mapping_info);

  /* Set the mapping to the IVEC-Id: constant mapping according to the VLAN format */
  for (vlan_format = 0; vlan_format < SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS; vlan_format++) {
    /* 
     * The IVEC-Id 0 - 7 is free for other purposes. 
     * Take here the IVEC-Id 0 for VLAN Edit Profile 0,  
     * Take here the IVEC-Ids 7 - 13 for VLAN Edit Profile 1,
     * Take here the IVEC-Ids 14 - 20 for VLAN Edit Profile 2, etc 
     * Until VLAN Edit Profile 7 
     * Note: IVEC-Id 0-3 are also used for PWE TPID profile. (set in bcm_petra_vlan_init) 
     */
    ivec_id = 7 * vlan_edit_profile;
    is_ivec_to_set = FALSE;
    is_vlan_format_valid = TRUE;

    _bcm_petra_vlan_structure_convert(unit, vlan_format, &supported_vlan_format, &is_ivec_to_set, &is_vlan_format_valid, &vlan_format_equivalent);
    ivec_id += supported_vlan_format;

    if (vlan_edit_profile == 0) {
      ivec_id = 0;
    }

    /* Get the IVEC from the HW when necessary */
    if (is_ivec_to_set) {
      soc_sand_rv = soc_ppd_lif_ing_vlan_edit_command_info_get(soc_sand_dev_id, ivec_id, &(mapping_info->ivec[vlan_format]));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
  }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Purpose: 
 * Set in the HW the Egress VLAN Edit profile attributes
 */
STATIC int
_bcm_petra_vlan_edit_profile_info_eg_hw_set(int unit,
                      int vlan_edit_profile_eg,
                      bcm_dpp_vlan_egress_edit_profile_info_t *eg_edit_profile_set)
{   
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format;
  uint8 is_ivec_to_set, is_vlan_format_valid;
  SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY command_key;
  DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT supported_vlan_format;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_equivalent;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_NULL_CHECK(eg_edit_profile_set);

  /* Set the the EVECs according to the VLAN format */
  for (vlan_format = 0; vlan_format < SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS; vlan_format++) {
    is_ivec_to_set = FALSE;
    is_vlan_format_valid = TRUE;

    _bcm_petra_vlan_structure_convert(unit, vlan_format, &supported_vlan_format, &is_ivec_to_set, &is_vlan_format_valid, &vlan_format_equivalent);

    /* Set the EVEC to the HW if necessary */
    if (is_vlan_format_valid) {
      SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY_clear(&command_key);
      command_key.tag_format = vlan_format;
      command_key.edit_profile = vlan_edit_profile_eg;
      soc_sand_rv = soc_ppd_eg_vlan_edit_command_info_set(soc_sand_dev_id, &command_key, &(eg_edit_profile_set->evec[vlan_format_equivalent]));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
  }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Purpose: 
 * Init the Default Port-Profile VLAN translation
 */
STATIC int
bcm_petra_vlan_translation_port_default_init(int unit,
                       int pcp_profile)
{   
  int rv;
  unsigned int soc_sand_dev_id, port_profile_ndx, soc_sand_rv;
  SOC_PPD_LLP_PARSE_INFO parse_key;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_equivalent;
  DPP_VLAN_TRANSLATION_SUPPORTED_VLAN_FORMAT supported_vlan_format;
  uint8 is_ivec_to_set, is_vlan_format_valid, is_priority; 
  SOC_PPD_LLP_PARSE_TPID_INDEX outer_tpid, inner_tpid;
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO format_info;

  BCMDNX_INIT_FUNC_DEFS;
  rv = BCM_E_NONE;
  soc_sand_dev_id = (unit);

  /* 
   * Set all the Port-Profiles to go statically to IVEC-Ids 56-63
   */
  for (port_profile_ndx = 0; port_profile_ndx < DPP_VLAN_TRANSLATION_MAX_NOF_PORT_PROFILES; port_profile_ndx++) {
    SOC_PPD_LLP_PARSE_INFO_clear(&parse_key);
    for (is_priority = FALSE; is_priority <= TRUE; is_priority++) {
      for (outer_tpid = 0; outer_tpid < SOC_PPD_NOF_LLP_PARSE_TPID_INDEXS; outer_tpid++) {
        for (inner_tpid = 0; inner_tpid < SOC_PPD_NOF_LLP_PARSE_TPID_INDEXS; inner_tpid++) {          
          parse_key.is_outer_prio = is_priority;
          parse_key.outer_tpid = outer_tpid;
          parse_key.inner_tpid = inner_tpid;


          /* Get the VLAN Structure from this */
          SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_clear(&format_info);
          soc_sand_rv = soc_ppd_llp_parse_packet_format_info_get(soc_sand_dev_id, port_profile_ndx, &parse_key, &format_info);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

          _bcm_petra_vlan_structure_convert(unit, format_info.tag_format, &supported_vlan_format, &is_ivec_to_set, &is_vlan_format_valid, &vlan_format_equivalent);
          format_info.dlft_edit_command_id = DPP_VLAN_TRANSLATION_IVEC_ID_DEFAULT_BASE + supported_vlan_format;
          format_info.dflt_edit_pcp_profile = pcp_profile;

          soc_sand_rv = soc_ppd_llp_parse_packet_format_info_set(soc_sand_dev_id, port_profile_ndx, &parse_key, &format_info);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
      }
    }

  }

  BCMDNX_IF_ERR_EXIT(rv);
exit:
  BCMDNX_FUNC_RETURN;
}

/* 
 * Initialization for the global lif setings 
 * Set all unmatch Keys in ISEM to be LIF-Ports-Default 
 * Such that VSI = VLAN 
 */
int
bcm_petra_vlan_global_lif_init(
    int unit)
{
    int port_i, core;
    SOC_PPD_L2_LIF_AC_KEY ac_key;    
    SOC_SAND_SUCCESS_FAILURE soc_sand_success;
    SOC_PPD_PORT soc_ppd_port_i;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    bcm_pbmp_t pbmp;
    bcm_error_t rv;
    SOC_PPD_LIF_ENTRY_INFO *lif_entry_info = NULL;
    SOC_PPD_LIF_ID default_lif_id = 0;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_ASSIGN(pbmp, PBMP_E_ALL(unit));    
    BCM_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    BCM_PBMP_REMOVE(pbmp, PBMP_SFI_ALL(unit));    
    BCM_PBMP_REMOVE(pbmp, PBMP_ST_ALL(unit));

#ifdef  BCM_VLAN_NO_DEFAULT_ETHER
    BCM_PBMP_REMOVE(pbmp, PBMP_E_ALL(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_ETHER */
#ifdef  BCM_VLAN_NO_DEFAULT_CPU
    BCM_PBMP_REMOVE(pbmp, PBMP_CMIC(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_CPU */
#if defined(BCM_VLAN_NO_AUTO_STACK)
    BCM_PBMP_REMOVE(pbmp, BCM_PBMP_STACK_CURRENT(unit));
#endif  /* BCM_VLAN_NO_AUTO_STACK */

    /* 
     * Allocate Defualt LIFs
     */
    if (SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop == SOC_PPC_AC_ID_INVALID &&
        SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple == SOC_PPC_AC_ID_INVALID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("User set incorrect soc properties for logical_port_drop, logical_port_l2_bridge both cant be set to invalid")));
    }

    BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPD_LIF_ENTRY_INFO), "bcm_petra_vlan_global_lif_init.lif_entry_info");
    if (lif_entry_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    /* Allocate global lif drop */
    /* there is need for ingress allocation only */ 
    if (SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop != SOC_PPC_AC_ID_INVALID) {
        rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_VLAN_EDITING, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &(SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop));
        BCMDNX_IF_ERR_EXIT(rv);

        /* Update LIF to drop */
        SOC_PPD_LIF_ENTRY_INFO_clear(lif_entry_info);

        lif_entry_info->type = SOC_PPD_LIF_ENTRY_TYPE_AC;
        lif_entry_info->value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        /* by default Gport, inited as P2P with drop destiantion */
        lif_entry_info->value.ac.default_frwrd.default_frwd_type = SOC_PPD_L2_LIF_DFLT_FRWRD_SRC_LIF; 
        /* in PB, LIF destination cannot be drop so use trap-code*/
        if(SOC_IS_PETRAB(unit)){
            SOC_PPD_FRWRD_DECISION_TRAP_SET(soc_sand_dev_id, &(lif_entry_info->value.ac.default_frwrd.default_forwarding), _BCM_PETRA_UD_DROP_TRAP, 5, 0, soc_sand_rv);
        }
        else{
            SOC_PPD_FRWRD_DECISION_DROP_SET(soc_sand_dev_id, &(lif_entry_info->value.ac.default_frwrd.default_forwarding), soc_sand_rv);
        }
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        lif_entry_info->value.ac.service_type = SOC_PPD_L2_LIF_AC_SERVICE_TYPE_AC2AC;
        lif_entry_info->value.ac.vsid = _BCM_PPD_VSI_P2P_SERVICE;
        lif_entry_info->value.ac.learn_record.learn_type = SOC_PPD_L2_LIF_AC_LEARN_DISABLE;

        soc_sand_rv = soc_ppd_lif_table_entry_update(soc_sand_dev_id, SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }


    if (SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple != SOC_PPC_AC_ID_INVALID) {
    
        /* Allocate global lif simple */
        /* there is a need for ingress allocation only */
        rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_VLAN_EDITING, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &(SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple));
        BCMDNX_IF_ERR_EXIT(rv);    
        
        /* Update LIF to simple */
        SOC_PPD_LIF_ENTRY_INFO_clear(lif_entry_info);

        lif_entry_info->type = SOC_PPD_LIF_ENTRY_TYPE_AC;
        lif_entry_info->value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;        
        lif_entry_info->value.ac.default_frwrd.default_frwd_type = SOC_PPD_L2_LIF_DFLT_FRWRD_SRC_VSI; 
        lif_entry_info->value.ac.service_type = SOC_PPD_L2_LIF_AC_SERVICE_TYPE_MP;
        if (SOC_IS_PETRAB(unit)) {
            lif_entry_info->value.ac.vsid                        = 0x3fff; /* VSI = -1: VSI is equal to initial-VID */
        }
        else { /* ARAD VSI assignment mode */
            lif_entry_info->value.ac.vsid                        = 0;
            lif_entry_info->value.ac.vsi_assignment_mode         = SOC_PPC_VSI_EQ_IN_VID;
        }
        lif_entry_info->value.ac.learn_record.learn_type = SOC_PPD_L2_LIF_AC_LEARN_SYS_PORT;

        soc_sand_rv = soc_ppd_lif_table_entry_update(soc_sand_dev_id, SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);        
    }

    /* By default, set all ports to default LIF simple , otherwise to drop */
    if (SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple != SOC_PPC_AC_ID_INVALID) {
      default_lif_id = SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple; /* In case simple is set by default all ports mapped to simple */          
    } else {
      default_lif_id = SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop;      
    }

    /* Update port lookups */
    BCM_PBMP_ITER(pbmp,port_i) {
          
          BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

          SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);
          ac_key.key_type = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT;
          ac_key.vlan_domain = soc_ppd_port_i; /* In case of MATCH_PORT , vlan_domain equals ppd port index */
                    
          /* lif_entry_info->value.ac is either drop or simple. In case simple is valid it is the last one updated information thus it catch the information */
          soc_sand_rv = soc_ppd_l2_lif_ac_add(soc_sand_dev_id, &ac_key, default_lif_id, &(lif_entry_info->value.ac), &soc_sand_success);
          BCMDNX_IF_ERR_EXIT(translate_sand_success_failure(soc_sand_success));
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }     


exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}


/*
 *   Function
 *      _bcm_petra_vlan_edit_set_default_command_ids
 *   Purpose
 *     Configure the first SOC_PPD_NOF_XXX_VLAN_EDIT_RESERVED_ACTION_IDS entries
 *     in the Ingress/Egress command tablea to be reserved by default. This includes 
 *     allocating the entries in the SW and writing default entries to the HW, with
 *     the TPID profile set to the command ID.
 *   Parameters
 *      (IN)  unit         : unit number of the device
 *   Returns
 *       BCM_E_NONE - success
 *       BCM_E_*    - failure
 */
int _bcm_petra_vlan_edit_set_default_command_ids(
   int unit)
{
    int rv, command_ndx, tag_format_ndx, action_id, tpid_profile;

    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    BCMDNX_INIT_FUNC_DEFS;

    unit = (unit);

    /* Set the same action for each created action ID, except for the TPID profile */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionNone;
    action.dt_inner = bcmVlanActionNone;
    action.dt_outer_pkt_prio = bcmVlanActionNone;
    action.dt_inner_pkt_prio = bcmVlanActionNone;

    /* Traverse the reserved Ingress command IDs */
    for (command_ndx = 0; command_ndx < SOC_DPP_NOF_INGRESS_VLAN_EDIT_RESERVED_ACTION_IDS(unit); command_ndx++) {

        action_id = command_ndx;

        /* Create an action ID */
        rv = bcm_petra_vlan_translate_action_id_create(unit,
                BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set an action for each created action ID with a TPID profile that equals to the action ID */
        rv = _bcm_petra_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS,
                    action_id, &action, &action_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Traverse the reserved Egress command IDs */
    for (command_ndx = 0; command_ndx < SOC_DPP_NOF_EGRESS_VLAN_EDIT_RESERVED_ACTION_IDS(unit); command_ndx++) {

        action_id = command_ndx;

        /* Create an action ID */
        rv = bcm_petra_vlan_translate_action_id_create(unit,
                BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Set action 0 as the default for the untagged packets.
       In this case a VLAN tag will be added with a VSI VID and mapped PCP.
       Set the TPID profile to 0 although the it isn't used in the Egress as this
       also means that the TPIDs or invalid */
    bcm_vlan_action_set_t_init(&action);
    tpid_profile = 0;
    action_id = 0;
    action.dt_outer = bcmVlanActionMappedAdd;
    action.dt_inner = bcmVlanActionNone;
    action.dt_outer_pkt_prio = bcmVlanActionAdd;
    action.dt_inner_pkt_prio = bcmVlanActionNone;
    rv = _bcm_petra_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS,
                action_id, &action, &tpid_profile);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set action 1 as the default for all tagged packets. Set the TPID valid flag to FALSE */
    bcm_vlan_action_set_t_init(&action);
    action_id = 1;
    action.dt_outer = bcmVlanActionNone;
    action.dt_inner = bcmVlanActionNone;
    action.dt_outer_pkt_prio = bcmVlanActionNone;
    action.dt_inner_pkt_prio = bcmVlanActionNone;
    rv = _bcm_petra_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS,
                action_id, &action, &tpid_profile);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Map all the VLAN edit class entries for VLAN edit profile 0 to the default actions.
       The untagged tag format (0) mapped to action id 0, while all others to action id 1 */
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    action_class.vlan_edit_class_id = 0;
    for (tag_format_ndx = 0; tag_format_ndx < SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS; tag_format_ndx++) {

        /* Go over all the tag formats for VLAN edit profile 0 */
        action_class.tag_format_class_id = tag_format_ndx;

        /* Set the mapped action ID to be 0 for the first untagged format. Otherwise, map to action 1 */
        action_class.vlan_translation_action_id = (tag_format_ndx) ? 1 : 0;

        /* Perform the mapping set */
        rv = bcm_petra_vlan_translate_action_class_set(unit, &action_class);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
  BCMDNX_FUNC_RETURN;
}


/* 
 * Initialization for the VLAN Translation: 
 * Set all the Ingress PCP mapping tables such that 
 * PCP-Profile gives always PCP = PCP-Profile 
 * The function should be called only for a non Warm Boot 
 */
int 
bcm_petra_vlan_translation_init(
    int unit)
{
  int rv;
  uint32 soc_sand_rv;
  unsigned int soc_sand_dev_id;
  SOC_SAND_PP_TC tc_ndx;
  SOC_SAND_PP_DP dp_ndx;
  SOC_SAND_PP_PCP_UP out_pcp, pcp_profile_ndx, tag_up_ndx;
  SOC_SAND_PP_DEI_CFI out_dei, tag_dei_ndx;
  bcm_dpp_vlan_edit_profile_mapping_info_t edit_profile_info;
  bcm_dpp_vlan_egress_edit_profile_info_t edit_profile_info_eg;
  bcm_dpp_vlan_egress_priority_mapping_table_info_t egress_priority_mapping_info;

  BCMDNX_INIT_FUNC_DEFS;
  rv = BCM_E_NONE;

  soc_sand_dev_id = (unit);
          
  /*
   * Set the PCP-Profile to give PCP = PCP-Profile for all packet types at ingress 
   * At egress, the mapping tables are set dynamically to coexist with the 
   * bcm_port_vlan_priority_unmap_set API 
   */
  for (pcp_profile_ndx = 0; pcp_profile_ndx <= SOC_SAND_PP_PCP_UP_MAX; pcp_profile_ndx ++) {
    /* Set the untagged mapping table */
    for (tc_ndx = 0; tc_ndx <= SOC_SAND_PP_TC_MAX; tc_ndx ++) {
      for (dp_ndx = 0; dp_ndx <= SOC_SAND_PP_DP_MAX; dp_ndx ++) {
        /* Get the DEI */
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get(soc_sand_dev_id, pcp_profile_ndx, tc_ndx, dp_ndx, &out_pcp, &out_dei);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Set the previous DEI and PCP = PCP-Profile */
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set(soc_sand_dev_id, pcp_profile_ndx, tc_ndx, dp_ndx, pcp_profile_ndx, out_dei);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
    }

    /* Set the c-tag mapping table */
    for (tag_up_ndx = 0; tag_up_ndx <= SOC_SAND_PP_PCP_UP_MAX; tag_up_ndx ++) {
      /* Get the DEI */
      soc_sand_rv = soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get(soc_sand_dev_id, pcp_profile_ndx, tag_up_ndx, &out_pcp, &out_dei);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      /* Set the previous DEI and PCP = PCP-Profile */
      soc_sand_rv = soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set(soc_sand_dev_id, pcp_profile_ndx, tag_up_ndx, pcp_profile_ndx, out_dei);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      /* Set the s-tag mapping table */
      for (tag_dei_ndx = 0; tag_dei_ndx <= SOC_SAND_PP_DEI_CFI_MAX; tag_dei_ndx ++) {
        /* Get the DEI */
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get(soc_sand_dev_id, pcp_profile_ndx, tag_up_ndx, tag_dei_ndx, &out_pcp, &out_dei);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Set the previous DEI and PCP = PCP-Profile */
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set(soc_sand_dev_id, pcp_profile_ndx, tag_up_ndx, tag_dei_ndx, pcp_profile_ndx, out_dei);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
    }
  }

  /* 
   * Set to the HW the PCP Profile attributes
   */
  _bcm_dpp_vlan_egress_priority_mapping_table_info_t_init(&egress_priority_mapping_info, 0);
  rv = _bcm_petra_vlan_egress_priority_tables_hw_set(unit, 0, &egress_priority_mapping_info);
  BCMDNX_IF_ERR_EXIT(rv);

  if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(unit))
  {
    /* 
     * Init the Profiles: the Egress Priority Tables and 
     * the Edit Profile indexes 
     */
    _bcm_dpp_vlan_edit_profile_mapping_info_t_init(&edit_profile_info);
    rv = _bcm_dpp_am_template_vlan_edit_profile_mapping_init(unit, 0, &edit_profile_info);
    BCMDNX_IF_ERR_EXIT(rv);

    _bcm_dpp_vlan_egress_edit_profile_info_t_init(&edit_profile_info_eg);
    rv = _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_init(unit, 0, &edit_profile_info_eg);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * Set all the Port-Profiles to go statically to IVEC-Ids 56-63
     */
    rv = bcm_petra_vlan_translation_port_default_init(unit, 0);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * Set to the HW the VLAN Edit profile attribute for Profile #0 each time 
     */
    rv = _bcm_petra_vlan_edit_profile_info_hw_set(unit, DPP_VLAN_TRANSLATION_IVEC_ID_NOP_EDIT_PROFILE, &edit_profile_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * Set VLAN Edit profile attribute for default port profile 
     */
    rv = _bcm_petra_vlan_edit_profile_info_hw_set(unit, DPP_VLAN_TRANSLATION_IVEC_ID_DEFAULT_EDIT_PROFILE, &edit_profile_info);
    BCMDNX_IF_ERR_EXIT(rv);

  } else if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(soc_sand_dev_id)) {
  /* 
   * Reset Egress VLAN editing SW tables, if in advanced VLAN edit mode. 
   */
  #ifdef BCM_ARAD_SUPPORT
    /* reset WB state */
    rv = soc_dpp_wb_engine_state_init_vlan_edit(unit);
    BCMDNX_IF_ERR_EXIT(rv);
  #endif

    /* The first SOC_PPD_NOF_XXX_VLAN_EDIT_RESERVED_ACTION_IDS entries in the Ingress/Egress command ID tables are reserved
       by default. They are allocated and the HW is set to default values. */
    rv = _bcm_petra_vlan_edit_set_default_command_ids(unit);
    BCMDNX_IF_ERR_EXIT(rv);
  }

exit:
  BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_vlan_deinit(int unit)
{
    _bcm_dpp_vlan_unit_state_t *tempUnit;
    BCMDNX_INIT_FUNC_DEFS;

    tempUnit = _bcm_dpp_vlan_unit_state[unit];
    _bcm_dpp_vlan_unit_state[unit] = NULL;
    if (tempUnit) {
        BCM_FREE(tempUnit);
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_init(
    int unit)
{
    int rv, profile_i;
    uint32 soc_sand_rv, ivec_id;

    SOC_PPD_VSI_DEFAULT_FRWRD_KEY dflt_frwrd_key;
    SOC_PPD_ACTION_PROFILE action_profile;
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO profile_info;
    bcm_dpp_vlan_flooding_profile_info_t def_flooding_info;
    bcm_dpp_vlan_port_protocol_entries_t port_protocol_entries;
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO vlan_edit_command_info;
    _bcm_dpp_vlan_unit_state_t *tempUnit = NULL;
    unsigned int allocSize;
    unsigned int index;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    
    if (_bcm_dpp_vlan_unit_state[unit]) {
        /* unit data already exists; deallocate everything */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_vlan_deinit(unit));
    }
    allocSize = sizeof(*tempUnit);
    BCMDNX_ALLOC(tempUnit,
              allocSize,
              "dpp vlan unit information");
    if (!tempUnit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG("unable to allocate %d bytes vlan state"),
                          allocSize));
    }
    /* set up initial values for VLAN state */
    sal_memset(tempUnit, 0x00, allocSize);
    for (index = 0; index < DPP_NOF_SHARED_FIDS; index++) {
        tempUnit->fid_ref_count[index].fid = ~0;
        tempUnit->fid_ref_count[index].ref_count =0;
    }
    /* after this point the code assumes it's in the unit data array */
    _bcm_dpp_vlan_unit_state[unit] = tempUnit;

    /* VLAN edit buffers should init at each boot */
    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        soc_sand_rv = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_VLAN_EDIT_EG_ACTION_MAPPING);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        soc_sand_rv = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_VLAN_EDIT_ING_ACTION);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        soc_sand_rv = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_VLAN_EDIT_EG_ACTION);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }


#ifdef BCM_WARM_BOOT_SUPPORT
    /* set up (possibly restore from) warm boot state */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_vlan_state_init(unit));
    if (SOC_WARM_BOOT(unit)) {
        /*
         *  In warm boot mode, the state was loaded just now, and we do not
         *  want to go through the hardware setup and state adjustments from
         *  the initial point.
         */
        BCM_EXIT;
    }
#endif /* def BCM_WARM_BOOT_SUPPORT */
    if (_bcm_dpp_vlan_unit_state[unit]->vlan_info.init) {
        /* Do not do full init if already initialised */
        
        BCM_EXIT;
    }

    unit = (unit);
    
    /* 
     * Set VLAN Translation init
     */
    rv = bcm_petra_vlan_translation_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * Set Global LIF to match all failure lookups to do VLAN=VSI
     */
    rv = bcm_petra_vlan_global_lif_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);
    
    rv = bcm_petra_vlan_default_set_internal(unit, BCM_VLAN_DEFAULT, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);
    
    if (SOC_IS_PETRAB(unit)) {
        /* set drop acction, pointed when VSI is closed */
        SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);
        profile_info.bitmap_mask = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
        SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(profile_info.dest_info.frwrd_dest), soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        profile_info.dest_info.add_vsi = FALSE;
        soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit, _BCM_DPP_VLAN_UNKNOWN_DA_DROP_TRAP, &profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* 
         * Set drop profile for UNKNOWN_DA_0 for UC/MC/BC
         */
        SOC_PPD_VSI_DEFAULT_FRWRD_KEY_clear(&dflt_frwrd_key);
        SOC_PPD_ACTION_PROFILE_clear(&action_profile);

        dflt_frwrd_key.default_forward_profile = DPP_VSI_DEFAULT_DROP_PROFILE;

        /* ACTION to drop */
        action_profile.trap_code = _BCM_DPP_VLAN_UNKNOWN_DA_DROP_TRAP;
        action_profile.frwrd_action_strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
        action_profile.snoop_action_strength = 0;

        for(dflt_frwrd_key.orientation = 0; dflt_frwrd_key.orientation < SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS; ++dflt_frwrd_key.orientation) {
            for(dflt_frwrd_key.da_type = 0; dflt_frwrd_key.da_type < SOC_SAND_PP_NOF_ETHERNET_DA_TYPES; ++dflt_frwrd_key.da_type) {

                soc_sand_rv = soc_ppd_vsi_default_frwrd_info_set(unit, &dflt_frwrd_key, &action_profile);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }


        /* 
         * Set default forawrd action to point to UNKNOWN_DA_0 for UC/MC/BC
         */
        SOC_PPD_VSI_DEFAULT_FRWRD_KEY_clear(&dflt_frwrd_key);
        SOC_PPD_ACTION_PROFILE_clear(&action_profile);

        for(dflt_frwrd_key.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
             dflt_frwrd_key.orientation < SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
             ++dflt_frwrd_key.orientation) {

            dflt_frwrd_key.default_forward_profile = DPP_VSI_DEFAULT_FRWRD_PROFILE;
        
            action_profile.trap_code = _BCM_DPP_VLAN_UNKNOWN_DA_FLD_TRAP;
            action_profile.frwrd_action_strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
            action_profile.snoop_action_strength = 0;
        
            /* UC */
            dflt_frwrd_key.da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC;
            soc_sand_rv = soc_ppd_vsi_default_frwrd_info_set(unit, &dflt_frwrd_key, &action_profile);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
            /* MC */
            dflt_frwrd_key.da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC;
            soc_sand_rv = soc_ppd_vsi_default_frwrd_info_set(unit, &dflt_frwrd_key, &action_profile);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
            /* BC */
            dflt_frwrd_key.da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC;
            soc_sand_rv = soc_ppd_vsi_default_frwrd_info_set(unit, &dflt_frwrd_key, &action_profile);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        }
        /* 
         * Set second option for default forawrd action: 
         * UC points to UNKNOWN_DA_0
         * BC points to UNKNOWN_DA_1
         * MC points to UNKNOWN_DA_2
         */

        dflt_frwrd_key.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;;
        dflt_frwrd_key.default_forward_profile = DPP_VSI_DEFAULT_FRWRD_PROFILE_UC_0_MC_4K_UC_8K;

        action_profile.frwrd_action_strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
        action_profile.snoop_action_strength = 0;

        /* UC */
        action_profile.trap_code = _BCM_DPP_VLAN_UNKNOWN_DA_FLD_TRAP;
        dflt_frwrd_key.da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC;
        soc_sand_rv = soc_ppd_vsi_default_frwrd_info_set(unit, &dflt_frwrd_key, &action_profile);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* MC */
        action_profile.trap_code = _BCM_DPP_VLAN_UNKNOWN_DA_FLD_TRAP + 1;
        dflt_frwrd_key.da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC;
        soc_sand_rv = soc_ppd_vsi_default_frwrd_info_set(unit, &dflt_frwrd_key, &action_profile);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* BC */
        action_profile.trap_code = _BCM_DPP_VLAN_UNKNOWN_DA_FLD_TRAP + 2;
        dflt_frwrd_key.da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC;
        soc_sand_rv = soc_ppd_vsi_default_frwrd_info_set(unit, &dflt_frwrd_key, &action_profile);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Map: 
         * UNKNOWN_DA_0 to multicast group 0 + vsid
         * UNKNOWN_DA_1 to multicast group 4K + vsid
         * UNKNOWN_DA_2 to multicast group 8K + vsid
         */
        SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);

        profile_info.bitmap_mask |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
        profile_info.dest_info.frwrd_dest.type = SOC_PPD_FRWRD_DECISION_TYPE_MC;    
        profile_info.dest_info.add_vsi = TRUE;

        profile_info.dest_info.frwrd_dest.dest_id = 0;
        soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit, _BCM_DPP_VLAN_UNKNOWN_DA_FLD_TRAP, &profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        profile_info.dest_info.frwrd_dest.dest_id = 4 * 1024;
        soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit, _BCM_DPP_VLAN_UNKNOWN_DA_FLD_TRAP + 1, &profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        profile_info.dest_info.frwrd_dest.dest_id = 8 * 1024;
        soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit, _BCM_DPP_VLAN_UNKNOWN_DA_FLD_TRAP + 2, &profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        SOC_PPD_L2_LIF_DEFAULT_FRWRD_ACTION default_frwrd_action[SOC_SAND_PP_NOF_ETHERNET_DA_TYPES];
        SOC_SAND_PP_ETHERNET_DA_TYPE da_type;

        /* Only for ARAD */
        def_flooding_info.bc_add = 0;
        def_flooding_info.unknown_mc_add = 0;
        def_flooding_info.unknown_uc_add = 0;

        rv = _bcm_dpp_am_template_l2_flooding_init(unit,0,&def_flooding_info);
        BCMDNX_IF_ERR_EXIT(rv);

        for (da_type = 0; da_type < SOC_SAND_PP_NOF_ETHERNET_DA_TYPES; da_type++) {
            SOC_PPD_L2_LIF_DEFAULT_FRWRD_ACTION_clear(&(default_frwrd_action[da_type]));
            /* Fill action info */
            default_frwrd_action[da_type].add_vsi = TRUE;
            /* By Default UC = MC = BC = VSI */
            default_frwrd_action[da_type].offset = 0;
            /* type and the rest are set just to pass the PPD verify */
            default_frwrd_action[da_type].frwrd_dest.type = SOC_PPD_FRWRD_DECISION_TYPE_TRAP;
            default_frwrd_action[da_type].frwrd_dest.dest_id = 0;
            default_frwrd_action[da_type].frwrd_dest.additional_info.trap_info.action_profile.frwrd_action_strength = 0;
            default_frwrd_action[da_type].frwrd_dest.additional_info.trap_info.action_profile.snoop_action_strength = 0;
            default_frwrd_action[da_type].frwrd_dest.additional_info.trap_info.action_profile.trap_code = 0;
        }

        rv = _bcm_petra_vlan_flooding_per_lif_hw_profile_set(unit, 0, 0, default_frwrd_action);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* <ethertype,vlan>->port-profile multiset */
    /* By default all zeros */
    for (profile_i = 0; profile_i < DPP_PRTCL_NOF_ETHER_TYPE_IDS; ++profile_i) {
        sal_memset(&(port_protocol_entries.port_protocol_entry[profile_i]),0x0,sizeof(bcm_dpp_vlan_port_protocol_profile_info_t));
    }
    
    rv = _bcm_dpp_am_template_vlan_port_protocol_init(unit, NOS_PRTCL_PORT_PROFILES, &port_protocol_entries);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* Set vlan command ids 1-3 for PWE TPID-profile */
    for (ivec_id = DPP_VLAN_TRANSLATION_IVEC_ID_PWE_TPID_PROFILE_START; ivec_id <= DPP_VLAN_TRANSLATION_IVEC_ID_PWE_TPID_PROFILE_END ; ivec_id++) {       
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_command_info_get(unit, ivec_id, &vlan_edit_command_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
         
        vlan_edit_command_info.tpid_profile = ivec_id;
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_command_info_set(unit, ivec_id, &vlan_edit_command_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    _bcm_dpp_vlan_unit_state[unit]->vlan_info.init = TRUE;
#ifdef BCM_WARM_BOOT_SUPPORT
    /* full warmboot state sync at this point */
    _bcm_dpp_vlan_all_wb_state_sync_int(unit, NULL, NULL);
#endif /* def BCM_WARM_BOOT_SUPPORT */

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_detach(int unit)
{
    _bcm_dpp_vlan_unit_state_t *tempUnit;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    tempUnit = _bcm_dpp_vlan_unit_state[unit];
    _bcm_dpp_vlan_unit_state[unit] = NULL;
    if (tempUnit) {
        tempUnit->vlan_info.init = FALSE;
        sal_free(tempUnit);
    }

    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_vlan_create(
    int unit, 
    bcm_vlan_t vid)
{
    int rv;
    SOC_PPD_VSI_ID vsi;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_VSI_INFO vsi_info;
    bcm_multicast_t group;
    SOC_TMC_MULT_ID multicast_id_ndx; 
    bcm_stg_t stg_default;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    rv = BCM_E_NONE;
    
    BCM_DPP_VLAN_CHK_ID(unit,vid);

    soc_sand_dev_id = (unit);
    
    vsi = vid;
    /* Get default stg, STG module must be initialized before */
    rv = bcm_petra_stg_default_get(unit,&stg_default);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Validate VSI does not exist */
    rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vsi);
    if (rv == BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("VSI already exists")));
    }

    /* 
     * API creates VSI. The following is being done: 
     * 1. Allocate VSI-lif-index
     * 2. Set VSI default information. 
     * 3. Create VSI and update SW state  
     * 4. Set VSI to STG default 
     * 5. Clean Egress Multicast group related to VSI (same ID) 
     */

    /* 1. Allocate VSI-index */
    rv = _bcm_dpp_vswitch_vsi_usage_alloc(unit, SHR_RES_ALLOC_WITH_ID,_bcmDppVsiTypeVswitch, &vid, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 2. Set VSI default information */
    SOC_PPD_VSI_INFO_clear(&vsi_info);
    vsi_info.enable_my_mac                               = FALSE;
    vsi_info.fid_profile_id                              = SOC_PPD_VSI_FID_IS_VSID;    
    vsi_info.default_forwarding.additional_info.eei.type = SOC_PPD_EEI_TYPE_EMPTY;
    vsi_info.stp_topology_id                             = BCM_DPP_STG_TO_TOPOLOGY_ID(stg_default);
    /* Default forwarding MC offset = VSI id */
    if (SOC_IS_PETRAB(unit)) {
        vsi_info.default_forward_profile                 = DPP_VSI_DEFAULT_FRWRD_PROFILE;
    }
    if (SOC_IS_ARAD(unit)) {
        vsi_info.default_forwarding.type                 = SOC_PPD_FRWRD_DECISION_TYPE_MC;
        vsi_info.default_forwarding.dest_id              = vsi;
    }
    
    soc_sand_rv = soc_ppd_vsi_info_set(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           
    /* 3. Add VSI to STG topology */
    if (stg_default >= 0) {
        /* Must be after v bitmap insert */
        rv = bcm_petra_stg_vlan_add(unit, stg_default, vsi);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    /* 4. Clean multicast group. Assuming multicast group was already created on Init */
    _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L2, vsi);

    rv = _bcm_petra_multicast_group_to_id(group,&multicast_id_ndx);
    BCMDNX_IF_ERR_EXIT(rv); 

    /* skip multicast deletion if no egress MC created */
    rv = _bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_EGRESS, multicast_id_ndx , TRUE);
    if (rv != BCM_E_NOT_FOUND && rv != BCM_E_NONE) {
       BCMDNX_IF_ERR_EXIT(rv); 
    }
    if (rv != BCM_E_NOT_FOUND) {
       rv = bcm_petra_multicast_egress_delete_all(unit,group);
       BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_destroy(
    int unit, 
    bcm_vlan_t vid)
{
    int rv;    
    SOC_PPD_VSI_ID vsi;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_VSI_INFO vsi_info;
    bcm_multicast_t group;
    SOC_TMC_MULT_ID multicast_id_ndx; 
    bcm_stg_t stg;
    bcm_pbmp_t pbmp_sum, pbmp,ubmp;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    rv = BCM_E_NONE;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp_sum);

    BCM_DPP_VLAN_CHK_ID(unit,vid);

    soc_sand_dev_id = (unit);

    vsi = vid;       
    /* 
     * Destroy VSI. Following is done: 
     * 1. Verify VSI already exists 
     * 2. Clear vlan membership (and so AC keys) 
     * 3. Reset VSI information. 
     * 4. Clean multicast group 
     * 5. remove VSI from STG 
     * 6. Deallocate VSI 
     * 7. Remove VSI SW state.  
     */

    /* 1. Verify VSI already exists */
    rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vsi);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VSI doesn't exist")));
    }
    /* 2. Clear vlan membership */
    rv = bcm_petra_vlan_port_get(unit, vid, &pbmp, &ubmp);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_OR(pbmp_sum, ubmp);
    BCM_PBMP_OR(pbmp_sum, pbmp);
    
    rv = bcm_petra_vlan_port_remove(unit, vid, pbmp);
    BCMDNX_IF_ERR_EXIT(rv);        
     
    /* 3. Reset VSI */
    SOC_PPD_VSI_INFO_clear(&vsi_info);
    vsi_info.enable_my_mac                               = FALSE;
    vsi_info.fid_profile_id                              = SOC_PPD_VSI_FID_IS_VSID;
    vsi_info.default_forwarding.dest_id                  = 0;
    vsi_info.default_forwarding.type                     = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
    vsi_info.default_forwarding.additional_info.eei.type = SOC_PPD_EEI_TYPE_EMPTY;
    soc_sand_rv = soc_ppd_vsi_info_set(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    /* 4. Clean multicast group */
     _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L2, vsi);

     rv = _bcm_petra_multicast_group_to_id(group,&multicast_id_ndx);
     BCMDNX_IF_ERR_EXIT(rv); 

     /* skip multicast deletion if no egress MC created */
     rv = _bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_EGRESS, multicast_id_ndx , TRUE);
     if (rv != BCM_E_NOT_FOUND && rv != BCM_E_NONE) {
         BCMDNX_IF_ERR_EXIT(rv); 
     }
     if (rv != BCM_E_NOT_FOUND) {
         rv = bcm_petra_multicast_egress_delete_all(unit,group);
         BCMDNX_IF_ERR_EXIT(rv);
     }

    /* 5. Remove VLAN from its spanning tree group (if existed) */
    rv = bcm_petra_vlan_stg_get(unit, vsi, &stg);
    BCMDNX_IF_ERR_EXIT(rv);

    _bcm_petra_stg_vlan_destroy(unit, stg, vid);
    
    /* 6. Deallocate VSI-index */
    rv = _bcm_dpp_vswitch_vsi_usage_dealloc(unit, _bcmDppVsiTypeAll, vsi);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_destroy_all(
    int unit)
{
    int idx;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    for (idx = BCM_VLAN_MIN; idx < BCM_VLAN_COUNT; idx++) {
        if ((!SHR_BITGET(_bcm_dpp_vlan_unit_state[unit]->vlan_info.vlan_bmp,
                         idx)) ||
            (idx == _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl)) {
            continue;
        }
        
        BCMDNX_IF_ERR_EXIT(bcm_petra_vlan_destroy(unit, idx));
    }
    
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_vlan_port_add(
    int unit, 
    bcm_vlan_t vid, 
    bcm_pbmp_t pbmp, 
    bcm_pbmp_t ubmp,
    uint32 flags) /* flags : same as BCM_VLAN_GPORT_ADD_XXX */
{
    int rv = BCM_E_NONE, core;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_PORT ports[SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE];
    bcm_port_t port_i;
    bcm_pbmp_t pbmp_sum;
    SOC_PPD_PORT soc_ppd_port_i;
    uint8 tagged;
    int value = 0;
    uint32 vlan_domain_ndx = 0;
    uint8 is_bvid = FALSE, found;
    SOC_PPD_VSI_ID vsi = 0;
    SOC_PPD_L2_LIF_AC_INFO ac_info;
    SOC_PPD_L2_LIF_AC_KEY ac_key;
    SOC_PPD_LIF_ID lif_index;
    SOC_PPD_PORT_INFO port_info;
    uint8 update_membership = TRUE;
    uint8 update_tag = TRUE;
    uint8 update_ingress = TRUE;
    uint8 update_egress = TRUE;
    uint8 update_unknown_uc = TRUE;
    uint8 replace_membership = FALSE;
    uint8 set_val;
    uint8 input_is_vsi = FALSE;
    bcm_gport_t *port_array = NULL;
    bcm_if_t *encap_id_array = NULL;
    int port_count, port_max, index;    

    
    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;    

    if (flags & BCM_VLAN_GPORT_ADD_INGRESS_ONLY) {
        update_egress = FALSE;
    }

    if (flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY) {
        update_ingress = FALSE;
    }

    if (flags & BCM_VLAN_GPORT_ADD_MEMBER_REPLACE) {
        replace_membership = TRUE;
    }

    if (flags & BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD) {
        update_unknown_uc = FALSE;
    }

    if (flags & BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE) {
        update_tag = FALSE;
    }

    if (flags & BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE) {
        update_membership = FALSE;
    }

    if (flags & BCM_VLAN_GPORT_ADD_SERVICE) {
        input_is_vsi = TRUE;
    }

    /* Ingress and Egress only seperated */
    if ((flags & BCM_VLAN_GPORT_ADD_INGRESS_ONLY) && (flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given invalid flags can be set either Ingress ONLY or egress ONLY not both")));
    }

    

    if (_BCM_DPP_VLAN_IS_BVID(vid)) { /* vlan is B-VID */
      is_bvid = TRUE;
      /* by default B-VSI = B-VID */
      /* b-vsi is with 4 upper bits set, to differentiate it from normal vsi */
      vsi = vid;
      /* remove 0xf000 so b-vid can be used as vid */
      vid = _BCM_DPP_BVID_TO_VLAN(vid);
    }

    BCM_DPP_VLAN_CHK_ID(unit,vid);

    if (is_bvid) { /* vlan is B-VID */
        /* 
        * only ports that are PBP can be mapped to B-VID
        * go over all ports and check they are PBP 
        */         
        _SHR_PBMP_ITER(pbmp, port_i) {
            rv = bcm_petra_port_control_get(unit, port_i, bcmPortControlMacInMac, &value);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!value) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN is B-VID but ports are not PBP ports")));
            }
        }
    }
    
    soc_sand_dev_id = (unit);
    
    sal_memset(ports,0x0, sizeof(ports));

    if (!is_bvid) { 
        /* Verify vsi */
        vsi = vid;
    }

    /* 
     * API set vlan membership by giving vlan and set of ports 
     * Following is done: 
     * 1. Set ingress vlan membership (vlan set) 
     * 2. Add tagged / untagged indication (vlan set) 
     * 3. Add new ACs and match them to VSI LIF (when vlan=vsi) . Not relevant any more.
     * 4. Set egress vlan membership (per vsi) by first retreive VSI for each <port,vlan>. 
     * 5. Add ports to multicast groups, According to flooding type (by LIF/Port/VSI)
     */

    /* Get membership array from device, OR it with the input, and set to device */
    soc_sand_rv = soc_ppd_llp_filter_ingress_vlan_membership_get(soc_sand_dev_id, vid, ports);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
   
    rv = pbmp_from_ppd_port_bitmap(unit, &pbmp_sum, ports, SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE);
    BCMDNX_IF_ERR_EXIT(rv);
            
    if (replace_membership) { 
        BCM_PBMP_XOR(pbmp_sum, pbmp); /* Replace by all already set make as unset */
    } else {
        BCM_PBMP_OR(pbmp_sum, pbmp);
    }
    
    rv = pbmp_to_ppd_port_bitmap(unit, &pbmp_sum, ports, SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE);
    BCMDNX_IF_ERR_EXIT(rv);
    
    if (update_ingress && update_membership) {        
        /* 1. Set ingress vlan membership (per vlan) */
        soc_sand_rv = soc_ppd_llp_filter_ingress_vlan_membership_set(soc_sand_dev_id, vid, ports);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    if (update_egress && update_tag) {
      /* 2. For each port, configure to be transmitted as tagged/unttaged */
      BCM_PBMP_ITER(pbmp, port_i) {
          tagged = !(BCM_PBMP_MEMBER(ubmp, port_i));
                  
          BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

          if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable && IS_PON_PORT(unit,port_i)) {
              if (vid <= SOC_SAND_PON_TUNNEL_ID_MAX && !tagged) { /* In case of PON port , VLANs in range of Tunnel-ID must be tagged */
                  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In PON-ports, VLAN in range of Tunnel-ID must be tagged")));
              }
          }

          if (replace_membership) {
              soc_sand_rv = soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get(soc_sand_dev_id, soc_ppd_port_i, vid, &tagged);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

              tagged = !tagged; /* replace settings */
          }

          soc_sand_rv = soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set(soc_sand_dev_id, soc_ppd_port_i, vid, tagged);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
    }


    BCM_PBMP_ITER(pbmp, port_i) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

        /* 3. For each port, add new ACs and match them to VSI LIF */
        if (input_is_vsi) {
            vsi = vid;
        } else { /* !input_is_vsi */
          if (update_ingress) {
              if (!is_bvid) {
                  /* AC key: Vlan domain-Vlan. First retreive Vlan-domain */
                  soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, soc_ppd_port_i, &port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                  vlan_domain_ndx = port_info.vlan_domain;                 
              }

              /* Check existed AC. If not, Allocate AC (vlan=vsi) */
              SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);
              if (is_bvid) { /* vlan is B-VID */
                  ac_key.key_type                     = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN;
                  ac_key.vlan_domain                  = 0; /* always, VD != 0 is not supported */
              }
              else {
                  ac_key.key_type                     = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN; /* key value is the port_id,vlan_domain */
                  if (_BCM_DPP_VT_TRANSLATION_PROFILE_INITIAL_VID(port_info.vlan_translation_profile)) {
                      ac_key.key_type                 = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN; /* key value is the port_id,vlan_domain */
                  }
                  ac_key.vlan_domain                  = vlan_domain_ndx;
              }
              ac_key.outer_vid                    = vid;
              ac_key.inner_vid                    = SOC_PPD_LIF_IGNORE_INNER_VID;

              SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);
              soc_sand_rv = soc_ppd_l2_lif_ac_get(soc_sand_dev_id, &ac_key, &lif_index, &ac_info, &found);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          } else { /* !update_ingress */
              found = FALSE;
          }
              
          if (found) {
              /* <port,vlan> -> VSI case */
              vsi = ac_info.vsid;
          } else {
              /* VSI is the given vlan */
              vsi = vid;
          }
        } /* } input_is_vsi */

        if (update_egress) {
            
            if (update_membership) {
                /* 4. Set egress vlan membership (per vsi) */
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
                set_val = TRUE;
                
                if (replace_membership) { /* Replace value in membership */
                    if (is_bvid) {
                        /* vsi for port membership should be without 4 upper bits set */
                        soc_sand_rv = soc_ppd_eg_filter_vsi_port_membership_get(soc_sand_dev_id, vid, soc_ppd_port_i, &set_val);
                    }
                    else if (vsi <= BCM_VLAN_MAX){
                        soc_sand_rv = soc_ppd_eg_filter_vsi_port_membership_get(soc_sand_dev_id, vsi, soc_ppd_port_i, &set_val);
                    }
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    set_val = !set_val;
                }

                if (is_bvid) {
                    /* vsi for port membership should be without 4 upper bits set */
                    soc_sand_rv = soc_ppd_eg_filter_vsi_port_membership_set(soc_sand_dev_id, vid, soc_ppd_port_i, set_val);
                }
                else if (vsi <= BCM_VLAN_MAX){
                    soc_sand_rv = soc_ppd_eg_filter_vsi_port_membership_set(soc_sand_dev_id, vsi, soc_ppd_port_i, set_val);
                }
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

             /* 5. For each port, Add port to the multicast groups (flooding) , only in case it is in membership */
             if (update_unknown_uc && vsi <= BCM_VLAN_MAX) {
                 bcm_multicast_t multicast_group;
                 SOC_TMC_MULT_ID multicast_id_ndx; 
                 bcm_gport_t mc_gport;
                  
                 if (is_bvid) {
                     _BCM_MULTICAST_GROUP_SET(multicast_group, _BCM_MULTICAST_TYPE_MIM, vid + BCM_PETRA_MIM_BVID_MC_GROUP_BASE);
                 }
                 else {
                     _BCM_MULTICAST_GROUP_SET(multicast_group, _BCM_MULTICAST_TYPE_L2, vsi);
                 }

                 /* check egress multicast group exist */
                 rv = _bcm_petra_multicast_group_to_id(multicast_group,&multicast_id_ndx);
                 BCMDNX_IF_ERR_EXIT(rv);
                 /* check egress multicast group exist */
                 rv = _bcm_petra_multicast_group_to_id(multicast_group,&multicast_id_ndx);
                 BCMDNX_IF_ERR_EXIT(rv);
                 if (SOC_IS_JERICHO(unit)) { 
                   rv = 0; /* egres group for now always open */
                 } else {
                   rv = _bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_EGRESS, multicast_id_ndx , TRUE);                   
                 }
                 BCMDNX_IF_ERR_EXIT(rv);

                 BCM_GPORT_LOCAL_SET(mc_gport,port_i);

                 set_val = TRUE;
                 if (replace_membership) { /* Replace value in membership */

                    BCM_PBMP_COUNT(PBMP_E_ALL(unit),port_max);
                    BCMDNX_ALLOC(port_array, sizeof(bcm_gport_t) * port_max, "local ports egress multicast");
                    if (port_array == NULL) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed in allocation of temporary memory")));
                    }

                    BCMDNX_ALLOC(encap_id_array, sizeof(bcm_if_t) * port_max, "encap ids egress multicast");
                    if (encap_id_array == NULL) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed in allocation of temporary memory")));
                    }
                    
                    rv = bcm_petra_multicast_egress_get(unit, multicast_group, port_max, port_array, encap_id_array, &port_count);
                    BCMDNX_IF_ERR_EXIT(rv);

                    set_val = FALSE; /* Assumed not in multicast group */
                    for (index = 0; index < port_count; ++index) {
                        if (!BCM_GPORT_IS_LOCAL(port_array[index])) { /* Must be local port */
                           continue;
                        }

                        if (BCM_GPORT_LOCAL_GET(port_array[index]) == port_i) { 
                           set_val = TRUE; /* there is a match , port is a member */
                           break;
                        }
                    }
                    
                    set_val = !set_val; /* replace */                   
                 }

                 /* 
                  * If multicast groups are not the same for unknown mc group & bc 
                  * Add ports to those multicast groups too. 
                  * It is user responsible to create multicast groups for unknown & bc.
                  */ 
                 /* UC */
                 if (!SOC_IS_JERICHO(unit)) { 
                   
                   if (set_val) {
                     rv = bcm_petra_multicast_egress_add(unit, multicast_group, mc_gport, 0);
                     if (rv != BCM_E_EXISTS) {
                         BCMDNX_IF_ERR_EXIT(rv);
                     }
                   } else {
                     rv = bcm_petra_multicast_egress_delete(unit, multicast_group, mc_gport, 0);
                     if (rv != BCM_E_NOT_FOUND) {
                         BCMDNX_IF_ERR_EXIT(rv);
                     }
                   }
                 }
             }
        }
    }

exit:
    if(encap_id_array) {
       BCM_FREE(encap_id_array);
    }
    if(port_array) {
       BCM_FREE(port_array);
    }
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_vlan_port_add(
    int unit, 
    bcm_vlan_t vid, 
    bcm_pbmp_t pbmp, 
    bcm_pbmp_t ubmp)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    rv = _bcm_petra_vlan_port_add(unit, vid, pbmp, ubmp, 0);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_port_remove(
    int unit, 
    bcm_vlan_t vid, 
    bcm_pbmp_t pbmp)
{
    int          rv;
    unsigned int soc_sand_dev_id;
    uint32       soc_sand_rv;
    uint8    is_bvid = FALSE;
    int      value, core;

    
    
    uint32     ports[SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE];
    bcm_port_t   port_i;
    bcm_pbmp_t   pbmp_sum, pbmp_neg, vlan_domain_remove, vlan_domain_left;
    uint8    tagged;
    SOC_PPD_PORT soc_ppd_port_i;
    uint32 vlan_domain_ndx;
    uint8 found;
    SOC_PPD_VSI_ID vsi;
    SOC_PPD_L2_LIF_AC_INFO ac_info;
    SOC_PPD_L2_LIF_AC_KEY ac_key;
    SOC_PPD_PORT_INFO port_info;
    SOC_PPD_LIF_ID lif_index;    
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    rv = BCM_E_NONE;

    if (_BCM_DPP_VLAN_IS_BVID(vid)) { /* vlan is B-VID */
        is_bvid = TRUE;
        /* remove 0xf000 so b-vid can be used as vid */
        vid = _BCM_DPP_BVID_TO_VLAN(vid);
    }

    BCM_PBMP_CLEAR(pbmp_sum);
    BCM_PBMP_CLEAR(pbmp_neg);
    BCM_PBMP_CLEAR(vlan_domain_left);
    BCM_PBMP_CLEAR(vlan_domain_remove);

    BCM_DPP_VLAN_CHK_ID(unit,vid);

    if (is_bvid) {
        /* 
        * only ports that are PBP can be mapped to B-VID 
        * (and therefore, can be removed) 
        * go over all ports and check they are PBP 
        */ 
        _SHR_PBMP_ITER(pbmp, port_i) {
            bcm_petra_port_control_get(unit, port_i, bcmPortControlMacInMac, &value);
            if (!value) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN is B-VID but ports are not PBP ports")));
            }
        }
    }    

    soc_sand_dev_id = (unit);

    sal_memset(ports, 0x0, sizeof(ports));
    
    /* Get membership array from device, AND the negative of the input array with
     * it, and set to device */
    soc_sand_rv = soc_ppd_llp_filter_ingress_vlan_membership_get(soc_sand_dev_id, vid, ports);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
    rv = pbmp_from_ppd_port_bitmap(unit, &pbmp_sum, ports, SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE);
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCM_PBMP_NEGATE(pbmp_neg, pbmp); 
    BCM_PBMP_AND(pbmp_sum, pbmp_neg);
            
    /* get vlan domain information only for left vlan_domains */
    BCM_PBMP_ITER(pbmp_sum, port_i) {
        rv = bcm_petra_port_class_get(unit, port_i, bcmPortClassId, &vlan_domain_ndx);
        BCMDNX_IF_ERR_EXIT(rv);
        
        BCM_PBMP_PORT_ADD(vlan_domain_left, vlan_domain_ndx);        
    }

    /* 
     *  pbmp_sum = updated approve ports.
     *  pbmp = remove ports.
     *  vlan_domain_left = indicates all vlan domains that one approve port still mapped to.
     *  API remove ports from given vlan. The followig is done:
     *  1. Update ingress vlan membership.
     *  2. Set egress transmit to be untagged for all removed ports.
     *  3. Update egress vlan membership (per vsi) without removed ports.
     *  4. Remove ports from multicast flooding groups.
     *  5. Remove AC in case of vlan=vsi.
     *  
     */
     
    /* 1. Update ingress vlan membership */
    rv = pbmp_to_ppd_port_bitmap(unit, &pbmp_sum, ports, SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE);
    BCMDNX_IF_ERR_EXIT(rv);
    
    soc_sand_rv = soc_ppd_llp_filter_ingress_vlan_membership_set(soc_sand_dev_id, vid, ports);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    /* 2. For each removed port, configure to be transmitted as tagged. By default have all VLAN-tags to be tagged unless user specify otherwise */
    BCM_PBMP_ITER(pbmp, port_i) {
        tagged = TRUE;
        
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
    
        soc_sand_rv =
            soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set(soc_sand_dev_id, soc_ppd_port_i, vid, tagged);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    BCM_PBMP_ITER(pbmp, port_i) {

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));     

        if (!is_bvid) {

            /* AC key: Vlan domainxVlan. First retreive Vlan-domain */
            soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, soc_ppd_port_i, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            vlan_domain_ndx = port_info.vlan_domain;     

            /* Add information also to vlan domain remove ports */
            BCM_PBMP_PORT_ADD(vlan_domain_remove, vlan_domain_ndx);
        }

        /* Check existed AC. */
        SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);
        if (is_bvid) { /* vlan is B-VID */
            ac_key.key_type                     = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN;
            ac_key.vlan_domain                  = 0; /* always, VD != 0 is not supported */
        }
        else {
            ac_key.key_type                     = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN; /* key value is the port_id,vlan_domain */
            if (_BCM_DPP_VT_TRANSLATION_PROFILE_INITIAL_VID(port_info.vlan_translation_profile)) {
                ac_key.key_type                 = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN; /* key value is the port_id,vlan_domain */
            }
            ac_key.vlan_domain                  = vlan_domain_ndx;
        }
        ac_key.outer_vid                        = vid;
        ac_key.inner_vid                        = SOC_PPD_LIF_IGNORE_INNER_VID;

        SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);
        soc_sand_rv = soc_ppd_l2_lif_ac_get(soc_sand_dev_id, &ac_key, &lif_index, &ac_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (found) {
            vsi = ac_info.vsid;
        }

        /* VSI is the given vlan */
        vsi = vid;

        /* 3. For each removed port, remove from egress vlan membership */
        if (is_bvid) {
            /* vsi for port membership should be without 4 upper bits set */
            soc_sand_rv = soc_ppd_eg_filter_vsi_port_membership_set(soc_sand_dev_id, vid, soc_ppd_port_i, FALSE);
        }
        else {
            soc_sand_rv = soc_ppd_eg_filter_vsi_port_membership_set(soc_sand_dev_id, vsi, soc_ppd_port_i, FALSE);
        }
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* 5. Remove ports from multicast groups */
        {
            bcm_multicast_t multicast_group;
            SOC_TMC_MULT_ID multicast_id_ndx;
            bcm_gport_t mc_gport;                        

            if (is_bvid) {
                _BCM_MULTICAST_GROUP_SET(multicast_group, _BCM_MULTICAST_TYPE_MIM, vid + BCM_PETRA_MIM_BVID_MC_GROUP_BASE);
            }
            else {
                _BCM_MULTICAST_GROUP_SET(multicast_group, _BCM_MULTICAST_TYPE_L2, vsi);
            }

            BCM_GPORT_LOCAL_SET(mc_gport,port_i);

            /* 
             * If multicast groups are not the same for unknown mc group & bc 
             * Add ports to those multicast groups too. 
             * It is user responsible to create multicast groups for unknown & bc.
             */ 
            /* UC */
            if (is_bvid) {
                if (SOC_IS_PETRAB(unit)) {
                    rv = bcm_petra_multicast_ingress_delete(unit, multicast_group, mc_gport, 0);
                }
            } else {
                   if (SOC_IS_JERICHO(unit)) { 
                       rv = 0; 
                   } else{
                    /* skip multicast deletion if no egress MC created */
                       rv = _bcm_petra_multicast_group_to_id(multicast_group,&multicast_id_ndx);                   
                       BCMDNX_IF_ERR_EXIT(rv);
                       rv = _bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_EGRESS, multicast_id_ndx , TRUE);
                       if (rv != BCM_E_NOT_FOUND && rv != BCM_E_NONE) {
                           BCMDNX_IF_ERR_EXIT(rv); 
                       }
                       if (rv != BCM_E_NOT_FOUND) {
                           rv = bcm_petra_multicast_egress_delete(unit,multicast_group,mc_gport,0);
                    
                           if (rv != BCM_E_NOT_FOUND) {
                             BCMDNX_IF_ERR_EXIT(rv);
                           } else {
                              rv = BCM_E_NONE; /* Expectation member in multicast group will be either exist or not. */
                           }
                       }
                    }
            }
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_vlan_port_get(
    int unit, 
    bcm_vlan_t vid, 
    bcm_pbmp_t *pbmp, 
    bcm_pbmp_t *ubmp)
{
    int rv, core;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 ports[SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE] = {0};
    bcm_port_t port_i;
    uint8 tagged;
    SOC_PPD_PORT soc_ppd_port_i;
    int value;
    uint8 is_bvid = FALSE;
    
    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;
    
    if (_BCM_DPP_VLAN_IS_BVID(vid)) { /* vlan is B-VID */
      is_bvid = TRUE;
      /* remove 0xf000 so b-vid can be used as vid */
      vid = _BCM_DPP_BVID_TO_VLAN(vid);
    }

    BCM_DPP_VLAN_CHK_ID(unit,vid);

    soc_sand_dev_id = (unit);

    BCM_PBMP_CLEAR(*pbmp);
    BCM_PBMP_CLEAR(*ubmp);
    sal_memset(ports, 0x0, sizeof(ports));
    
    /* Get membership array from device */
    soc_sand_rv = soc_ppd_llp_filter_ingress_vlan_membership_get(soc_sand_dev_id, vid, ports);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
    rv = pbmp_from_ppd_port_bitmap(unit, pbmp, ports, SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_bvid) { /* vlan is B-VID */
        /* 
        * only ports that are PBP can be mapped to B-VID
        * go over all ports and return only PBP ports
        */ 
        _SHR_PBMP_ITER((*pbmp), port_i) {
            bcm_petra_port_control_get(unit, port_i, bcmPortControlMacInMac, &value);
            if (!value) {
                _SHR_PBMP_PORT_REMOVE((*pbmp), port_i);
            }
        }
        _SHR_PBMP_ITER((*pbmp), port_i) {
            bcm_petra_port_control_get(unit, port_i, bcmPortControlMacInMac, &value);
            if (!value) {
                _SHR_PBMP_PORT_REMOVE((*pbmp), port_i);
            }
        }
    }
    /* For each port, configure to be transmitted as tagged/unttaged */
    BCM_PBMP_ITER((*pbmp), port_i) {        
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

        soc_sand_rv = soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get(soc_sand_dev_id, soc_ppd_port_i, vid, &tagged);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        if (tagged == 0) {
            BCM_PBMP_PORT_ADD((*ubmp), port_i);
        }
    }
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_port_get(
    int unit, 
    bcm_vlan_t vid, 
    bcm_pbmp_t *pbmp, 
    bcm_pbmp_t *ubmp)
{    
    bcm_error_t rv = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_vlan_port_get(unit, vid, pbmp, ubmp);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_gport_add(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_gport_t port, 
    int flags)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_VLAN_CHK_ID(unit,vlan);
    
    if (port == BCM_GPORT_INVALID)
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port is invalid")));
    
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    if (!_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)){    /* If port is not local, there's nothing to config*/
        BCM_EXIT;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    
    BCM_PBMP_ASSIGN(pbmp, gport_info.pbmp_local_ports);

    if (flags & BCM_VLAN_PORT_UNTAGGED) {
        BCM_PBMP_ASSIGN(ubmp, gport_info.pbmp_local_ports);
    }
    
    rv = _bcm_petra_vlan_port_add(unit, vlan, pbmp, ubmp, flags);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_gport_delete(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_gport_t port)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_VLAN_CHK_ID(unit,vlan);
    
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_CLEAR(pbmp);
    
    BCM_PBMP_ASSIGN(pbmp, gport_info.pbmp_local_ports);
       
    rv = bcm_petra_vlan_port_remove(unit, vlan, pbmp);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_gport_delete_all(
    int unit, 
    bcm_vlan_t vlan)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_VLAN_CHK_ID(unit,vlan);
    
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    
    rv = bcm_petra_vlan_port_get(unit, vlan, &pbmp, &ubmp);
    BCMDNX_IF_ERR_EXIT(rv);
    
    rv = bcm_petra_vlan_port_remove(unit, vlan, pbmp);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


#define _BCM_DPP_VLAN_GPORT_GET_VLAN_INFO_ONLY (0x1)
#define _BCM_DPP_VLAN_GPORT_GET_VSI_INFO_ONLY  (0x2)
int
_bcm_petra_vlan_gport_get(
    int unit, 
    bcm_vlan_t vlan,
    bcm_gport_t port, 
    int internal_flags,
    int *flags)
{
    int rv;    
    uint32 soc_sand_rv;
    uint32 vlan_domain_ndx;
    bcm_pbmp_t pbmp;
    bcm_port_t local_port;
    uint8 ingress_member;
    uint8 egress_member;
    uint32 ports[SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE] = {0};
    SOC_PPD_VSI_ID vsi = 0;
    SOC_PPD_L2_LIF_AC_INFO ac_info;
    SOC_PPD_L2_LIF_AC_KEY ac_key;
    SOC_PPD_LIF_ID lif_index = 0;
    SOC_PPD_PORT soc_ppd_port_i;
    uint8 found,is_member = FALSE,tagged = FALSE, is_bvid = FALSE;
    int value, core;
    uint8 vsi_only = internal_flags & _BCM_DPP_VLAN_GPORT_GET_VSI_INFO_ONLY;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    if (!(vsi_only)) {
        BCM_DPP_VLAN_CHK_ID(unit,vlan);   
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    DPP_PBMP_SINGLE_PORT_GET(gport_info.pbmp_local_ports, local_port);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port_i, &core)));  

    BCM_PBMP_CLEAR(pbmp);    
    sal_memset(ports, 0x0, sizeof(ports));
    ingress_member = FALSE;
    egress_member = FALSE;

    if (_BCM_DPP_VLAN_IS_BVID(vlan)) { /* vlan is B-VID */
      is_bvid = TRUE;
      /* by default B-VSI = B-VID */
      /* b-vsi is with 4 upper bits set, to differentiate it from normal vsi */
      vsi = vlan;
      /* remove 0xf000 so b-vid can be used as vid */
      vlan = _BCM_DPP_BVID_TO_VLAN(vlan);
    }

    if (!(vsi_only)) {
        BCM_DPP_VLAN_CHK_ID(unit,vlan);
    }

    if (is_bvid) { /* vlan is B-VID */
        /* 
        * only ports that are PBP can be mapped to B-VID
        * go over all ports and check they are PBP 
        */ 
        
        rv = bcm_petra_port_control_get(unit, local_port, bcmPortControlMacInMac, &value);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!value) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN is B-VID but ports are not PBP ports")));
        }
    }
    
    /* Get membership array from device */
    if (!(internal_flags & (_BCM_DPP_VLAN_GPORT_GET_VSI_INFO_ONLY))) {
        soc_sand_rv = soc_ppd_llp_filter_ingress_vlan_membership_get(unit, vlan, ports);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                
        rv = pbmp_from_ppd_port_bitmap(unit, &pbmp, ports, SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE);
        BCMDNX_IF_ERR_EXIT(rv);
       
        if (BCM_PBMP_MEMBER(pbmp, local_port)) {
            ingress_member = TRUE;
        }
    }
    
    /* vsi or vlan */
    if (!is_bvid) {
        vsi = vlan;
    }
    
    if (!(internal_flags & (_BCM_DPP_VLAN_GPORT_GET_VLAN_INFO_ONLY | _BCM_DPP_VLAN_GPORT_GET_VSI_INFO_ONLY))) {
        /* lookup */
        if (!is_bvid) {
            /* AC key: Vlan domain-Vlan. First retreive Vlan-domain */
            rv = bcm_petra_port_class_get(unit, local_port, bcmPortClassId, &vlan_domain_ndx);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Retreive existed AC */
        SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);
        if (is_bvid) { /* vlan is B-VID */
            ac_key.key_type                     = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN;
            ac_key.vlan_domain                  = 0; /* always, VD != 0 is not supported */
        }
        else {
            ac_key.key_type                     = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN; /* key value is the port_id,vlan_domain */
            ac_key.vlan_domain                  = vlan_domain_ndx;
        }
        ac_key.outer_vid                        = vlan;
        ac_key.inner_vid                        = SOC_PPD_LIF_IGNORE_INNER_VID;

        SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);
        soc_sand_rv = soc_ppd_l2_lif_ac_get(unit, &ac_key, &lif_index, &ac_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (found) {
            /* <port,vlan> -> VSI case */
            vsi = ac_info.vsid;
        }
    }

    /* Egress membership */
    if (!(internal_flags & _BCM_DPP_VLAN_GPORT_GET_VLAN_INFO_ONLY)) {
        if (is_bvid) {
            /* vsi for port membership should be without 4 upper bits set */
            soc_sand_rv = soc_ppd_eg_filter_vsi_port_membership_get(unit, vlan, soc_ppd_port_i, &is_member);
        }
        else {
            if (vsi < 4096) {
                soc_sand_rv = soc_ppd_eg_filter_vsi_port_membership_get(unit, vsi, soc_ppd_port_i, &is_member);
            } else {
                soc_sand_rv = SOC_SAND_OK;
            }
        }
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        if (is_member) {
            egress_member = TRUE;
        }
    }

    /* Egress transmit tag , untag */
    if (!(internal_flags & _BCM_DPP_VLAN_GPORT_GET_VSI_INFO_ONLY)) {
        soc_sand_rv = soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get(unit, soc_ppd_port_i, vlan, &tagged);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        if (tagged == 0) {
            *flags |= BCM_VLAN_GPORT_ADD_UNTAGGED;
        }
    }
    
    if (ingress_member && !egress_member) {
        *flags |= BCM_VLAN_GPORT_ADD_INGRESS_ONLY;
    }

    if (egress_member && !ingress_member) {
        *flags |= BCM_VLAN_GPORT_ADD_EGRESS_ONLY;
    }

    /* In case port is not member of vlan return error */
    if (!(internal_flags & (_BCM_DPP_VLAN_GPORT_GET_VLAN_INFO_ONLY | _BCM_DPP_VLAN_GPORT_GET_VSI_INFO_ONLY))) {
      if (!ingress_member && !egress_member) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("port do not match to vlan")));
      }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_gport_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_gport_t port, 
    int *flags)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    *flags = 0;

    rv = _bcm_petra_vlan_gport_get(unit, vlan, port, 0, flags);
    BCMDNX_IF_ERR_EXIT(rv);
        
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_gport_get_all(
    int unit, 
    bcm_vlan_t vlan, 
    int array_max, 
    bcm_gport_t *gport_array, 
    int *is_untagged, 
    int *array_size)
{
    int rv;
    bcm_pbmp_t          pbmp, ubmp;
    bcm_port_t          port;
    int                 port_cnt;
    bcm_module_t        _mymodid = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_VLAN_CHK_ID(unit,vlan);
    BCMDNX_NULL_CHECK(is_untagged);
    BCMDNX_NULL_CHECK(array_size);
    BCM_DPP_MIN_CHECK(array_max, 0);

    rv = BCM_E_NONE;
    port_cnt = 0;

    rv = bcm_petra_vlan_port_get(unit, vlan, &pbmp, &ubmp);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(pbmp, port) {

        if (port_cnt == array_max) {
            break;
        }

        rv = bcm_petra_stk_my_modid_get(unit, &_mymodid);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_GPORT_MODPORT_SET(gport_array[port_cnt],_mymodid,port);
        
        if (is_untagged) {
            if (BCM_PBMP_MEMBER(ubmp, port)) {
                is_untagged[port_cnt] = TRUE;
            } else {
                is_untagged[port_cnt] = FALSE;
            }
        }

        port_cnt++;
    }

    /* If all attempts found nothing return not found */
    if (port_cnt == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Did not find any matching port")));
    }

    *array_size = port_cnt;
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_vlan_list_internal
 * Purpose:
 *      Main body of bcm_petra_vlan_list() and bcm_petra_vlan_list_by_pbmp().
 *      Assumes locking already done.
 * Parameters:
 *      list_all - if TRUE, lists all ports and ignores list_pbmp.
 *      list_pbmp - if list_all is FALSE, lists only VLANs containing
 *              any of the ports in list_pbmp.
 */

STATIC int
bcm_petra_vlan_list_internal(int unit, bcm_vlan_data_t **listp, int *countp,
               int list_all, bcm_pbmp_t list_pbmp)
{
    bcm_vlan_data_t     *list = NULL;
    int                 i, rv;
    int                 idx;
    int                 valid_count;
    bcm_pbmp_t          pbmp, ubmp, tbmp;

    BCMDNX_INIT_FUNC_DEFS;
    *countp = 0;
    *listp = NULL;

    if (!list_all && BCM_PBMP_IS_NULL(list_pbmp)) { /* Empty list */
        BCM_EXIT;
    }

    if (_bcm_dpp_vlan_unit_state[unit]->vlan_info.count == 0) {
        BCM_EXIT; /* Empty list */
    }

    BCMDNX_ALLOC(list,
              _bcm_dpp_vlan_unit_state[unit]->vlan_info.count * sizeof (list[0]),
              "vlan_list");
    if (list == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG("unable to allocate memory for vlan list"
                                   " of %d entries"),
                          _bcm_dpp_vlan_unit_state[unit]->vlan_info.count));
    }

    i = 0;
    valid_count = 0;

    /* Iterate over all vlans */
    for (idx = BCM_VLAN_MIN; idx < BCM_VLAN_COUNT; idx++) {
        if (!SHR_BITGET(_bcm_dpp_vlan_unit_state[unit]->vlan_info.vlan_bmp,
                        idx)) {
            /* vlan doesn't exist. continue */
            continue;
        }

        rv = bcm_petra_vlan_port_get(unit, idx, &pbmp, &ubmp);
        if (BCM_FAILURE(rv)) {
           BCM_FREE(list);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        valid_count++;

        BCM_PBMP_ASSIGN(tbmp, list_pbmp);
        BCM_PBMP_AND(tbmp, pbmp);
        if (list_all || BCM_PBMP_NOT_NULL(tbmp)) {
            list[i].vlan_tag = idx;
            BCM_PBMP_ASSIGN(list[i].port_bitmap, pbmp);
            BCM_PBMP_ASSIGN(list[i].ut_port_bitmap, ubmp);
            i++;

        }
        if (valid_count == _bcm_dpp_vlan_unit_state[unit]->vlan_info.count)   {
            break;
        }
    } 

    *countp = i;
    *listp = list;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_list(
    int unit, 
    bcm_vlan_data_t **listp, 
    int *countp)
{
    int         rv;
    bcm_pbmp_t  empty_pbm;

    BCMDNX_INIT_FUNC_DEFS;
    /* CHECK_INIT(unit); */

    BCM_PBMP_CLEAR(empty_pbm);
    /* BCM_LOCK(unit); */
    rv = bcm_petra_vlan_list_internal(unit, listp, countp, TRUE, empty_pbm);
    /* BCM_UNLOCK(unit); */

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_vlan_list_by_pbmp(
    int unit,
    bcm_pbmp_t ports,
    bcm_vlan_data_t **listp,
    int *countp)
{
    int         rv;

    BCMDNX_INIT_FUNC_DEFS;
    /* BCM_LOCK(unit); */
    rv = bcm_petra_vlan_list_internal(unit, listp, countp, FALSE, ports);
    /* BCM_UNLOCK(unit); */

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_vlan_list_destroy(
    int unit,
    bcm_vlan_data_t *list,
    int count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    if (list != NULL) {
       BCM_FREE(list);
    }

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_default_get(
    int unit, 
    bcm_vlan_t *vid_ptr)
{   
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(vid_ptr);
    *vid_ptr = _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl;
        
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_default_set(
    int unit, 
    bcm_vlan_t vid)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_VLAN_CHK_ID(unit,vid);

    BCMDNX_IF_ERR_EXIT(bcm_petra_vlan_default_set_internal(unit, vid, FALSE));
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_stg_get(
    int unit, 
    bcm_vlan_t vid, 
    bcm_stg_t *stg_ptr)
{
    bcm_error_t rv;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_VSI_INFO vsi_info;
    SOC_PPD_VSI_ID vsi;
    SOC_PPD_BMACT_BVID_INFO bvid_info;
    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;
   
    BCMDNX_NULL_CHECK(stg_ptr);

    soc_sand_dev_id = (unit);

  if (_BCM_DPP_VLAN_IS_BVID(vid)) { /* B-VID */

    /* remove 0x1111 from B-VID */
    vsi = _BCM_DPP_BVID_TO_VLAN(vid);

    BCM_DPP_VLAN_CHK_ID(unit,vsi);

    soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_get(soc_sand_dev_id, vsi, &bvid_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *stg_ptr = BCM_DPP_STG_FROM_TOPOLOGY_ID(bvid_info.stp_topology_id);
  }
  else { /* VSI */

    vsi = vid;

    BCM_DPP_VLAN_CHK_ID(unit,vsi);

    rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vsi); 
    if (rv != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VSI doesn't exist")));
    }

    soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *stg_ptr = BCM_DPP_STG_FROM_TOPOLOGY_ID(vsi_info.stp_topology_id);
  }
   
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_stg_set(
    int unit, 
    bcm_vlan_t vid, 
    bcm_stg_t stg)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    if (_BCM_DPP_VLAN_IS_BVID(vid)) { /* B-VID */
        /* Just call store */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_vlan_stg_set(unit,vid,stg));
    } else {
        /* Call STG module so it can be save also in SW */
        BCMDNX_IF_ERR_EXIT(bcm_petra_stg_vlan_add(unit,stg,vid));
    }

    BCM_EXIT; 
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


int 
_bcm_petra_vlan_stg_set(
    int unit, 
    bcm_vlan_t vid, 
    bcm_stg_t stg)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_VSI_INFO vsi_info;
    SOC_PPD_VSI_ID vsi, b_vid;
    SOC_PPD_BMACT_BVID_INFO bvid_info;
    
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    if (_BCM_DPP_VLAN_IS_BVID(vid)) { /* B-VID */

        /* remove 0x1111 from B-VID */
        b_vid = _BCM_DPP_BVID_TO_VLAN(vid);

        BCM_DPP_VLAN_CHK_ID(unit,b_vid);

        soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_get(soc_sand_dev_id, b_vid, &bvid_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        bvid_info.stp_topology_id = BCM_DPP_STG_TO_TOPOLOGY_ID(stg);

        soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_set(soc_sand_dev_id, b_vid, &bvid_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  } else { /* VSI */

        vsi = vid;

        BCM_DPP_VLAN_CHK_ID(unit,vsi);

        soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        vsi_info.stp_topology_id = BCM_DPP_STG_TO_TOPOLOGY_ID(stg);

        soc_sand_rv = soc_ppd_vsi_info_set(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  }

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_vlan_stp_get
 * Purpose:
 *      Get the spanning tree state for a port in the whole spanning
 *      tree group that contains the specified VLAN.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number.
 *      vid - VLAN ID.
 *      port - Port
 *      stp_state - (OUT) State to return.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_vlan_stp_get(
    int unit, 
    bcm_vlan_t vid, 
    bcm_port_t port, 
    int *stp_state)
{
    bcm_error_t rv;
    bcm_stg_t   stgid;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
        
    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(bcm_petra_vlan_stg_get(unit, vid, &stgid));
    BCMDNX_IF_ERR_EXIT(bcm_petra_stg_stp_get(unit, stgid, gport_info.local_port, stp_state));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_vlan_stp_set
 * Purpose:
 *      Set the spanning tree state for a port in the whole spanning
 *      tree group that contains the specified VLAN.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      vid - VLAN ID
 *      port - Port
 *      stp_state - State to set
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_vlan_stp_set(
    int unit, 
    bcm_vlan_t vid, 
    bcm_port_t port, 
    int stp_state)
{
    bcm_error_t rv;
    bcm_port_t  local_port;
    bcm_stg_t   stgid;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
        if (!IS_E_PORT(unit,port)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("API expects Ethernet port as a parameter")));
        }
    }
     
    BCMDNX_IF_ERR_EXIT(bcm_petra_vlan_stg_get(unit, vid, &stgid));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {           
        BCMDNX_IF_ERR_EXIT(bcm_petra_stg_stp_set(unit, stgid, local_port, stp_state));
    }
 
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_vlan_vector_stp_set
 *   Purpose
 *      Set the spanning tree state for the port on the vlan vector
 *   Parameters
 *      (in) int unit = unit number whose VID is to be checked
 *      (in) bcm_vlan_vector_t vlan_vector = vlan_vector whose STP state is to be changed
 *      (in) bcm_port_t port = port whose STP state is to be changed
 *      (in) int stp_state = new stp state for the port on the vlan vector
 *   Returns
 *      bcm_error_t = BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 */
int 
bcm_petra_vlan_vector_stp_set(
    int unit, 
    bcm_vlan_vector_t vlan_vector, 
    bcm_port_t port, 
    int stp_state)
{
    bcm_vlan_t  vid;   
        
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    for (vid = BCM_VLAN_MIN + 1; vid <= BCM_VLAN_MAX; vid++) {
       if (BCM_VLAN_VEC_GET(vlan_vector, vid)) {
          BCMDNX_IF_ERR_EXIT(bcm_petra_vlan_stp_set(unit, vid, port, stp_state));
       }
    }
   
exit:
    _BCM_DPP_SWITCH_API_END(unit);
   BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_port_default_action_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_action_set_t *action)
{
  int rv;
  unsigned int soc_sand_dev_id;
  bcm_dpp_vlan_edit_profile_mapping_info_t edit_profile_set;
  bcm_dpp_vlan_egress_edit_profile_info_t eg_mapping_info_unused;
  bcm_vlan_action_set_t action_eg;
  SOC_PPD_LLP_PARSE_INFO parse_key;
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO format_info;
  uint32 soc_sand_rv;

  BCMDNX_INIT_FUNC_DEFS;
  rv = BCM_E_NONE;
  soc_sand_dev_id = (unit);

  if (SOC_IS_ARDON(unit)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("VLan in not available in Ardon\n")));
  }

  /* Handle the API only if the SOC property is set to Normal VLAN translation mode */
  if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(soc_sand_dev_id)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Normal mode")));
  }

  /* Retrieve the data from the SW DB - the default In-AC is the key */ 
    rv = _bcm_petra_vlan_edit_profile_info_hw_get(unit, DPP_VLAN_TRANSLATION_IVEC_ID_DEFAULT_EDIT_PROFILE, &edit_profile_set);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Convert it to the Action */
  bcm_vlan_action_set_t_init(action);
  bcm_vlan_action_set_t_init(&action_eg);
  /* Convert the Edit Profile Data to the Action */
  _bcm_dpp_vlan_egress_edit_profile_info_t_init (&eg_mapping_info_unused);
  rv = _bcm_petra_vlan_action_from_edit_profile_info_parse(unit, &edit_profile_set, &eg_mapping_info_unused, action, &action_eg);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Fill TPID actions to none */
  action->outer_tpid_action = bcmVlanTpidActionNone;
  action->inner_tpid_action = bcmVlanTpidActionNone;

  /* Get the Priority with the first configuration as example */
  SOC_PPD_LLP_PARSE_INFO_clear(&parse_key);
  parse_key.is_outer_prio = 0;
  parse_key.outer_tpid = 0;
  parse_key.inner_tpid = 0;

  /* Get the VLAN Structure from this */
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_clear(&format_info);
  soc_sand_rv = soc_ppd_llp_parse_packet_format_info_get(soc_sand_dev_id, 0, &parse_key, &format_info);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  if (format_info.dflt_edit_pcp_profile == 0) {
    action->priority =  format_info.dflt_edit_pcp_profile;
  } else {
    BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(action->priority,format_info.dflt_edit_pcp_profile); 
  }

  BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_port_default_action_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_action_set_t *action)
{
  int rv, core;
  bcm_dpp_vlan_edit_profile_mapping_info_t edit_profile_set;
  uint32 tpid_profile, soc_sand_rv;
  SOC_PPD_PORT_INFO port_info;
  bcm_dpp_vlan_egress_edit_profile_info_t eg_mapping_info_unused;
  SOC_PPD_PORT soc_ppd_port;
  _bcm_dpp_gport_info_t gport_info;
  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  _BCM_DPP_SWITCH_API_START(unit);
  rv = BCM_E_NONE;

  /* Handle the API only if the SOC property is set to Normal VLAN translation mode */
  if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(unit)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Normal mode")));
  }

  /* Verify action */
  rv = _bcm_petra_vlan_translate_action_verify(unit,TRUE,action);
  BCMDNX_IF_ERR_EXIT(rv); 

  /* 
   * Set for any Port profile the same default action: 
   * the HW could handle several actions per Port-Profile, but it would require: 
   * - manage the Port-Profile also according to the actions 
   * - manage the IVEC-Ids instead of a static configuration  
   */ 

  /* Set all the Port-Profiles with this Priority */
  rv = bcm_petra_vlan_translation_port_default_init(unit, action->priority);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Set all the IVECs */
  /* Init the Edit Profile structure */
  _bcm_dpp_vlan_edit_profile_mapping_info_t_init (&edit_profile_set);
  _bcm_dpp_vlan_egress_edit_profile_info_t_init(&eg_mapping_info_unused);


  /* Get the Port TPID Profile */
  rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
  BCMDNX_IF_ERR_EXIT(rv);

  SOC_PPD_PORT_INFO_clear(&port_info);
  
  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port, &core))); 
  soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  tpid_profile = port_info.tpid_profile;

  /* Convert the Action set to the Edit Profile Structure */
  rv = _bcm_petra_vlan_action_to_edit_profile_info_build(unit, action, tpid_profile, TRUE, &edit_profile_set, &eg_mapping_info_unused);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Set the IVECs in the IVEC table */
  rv = _bcm_petra_vlan_edit_profile_info_hw_set(unit, DPP_VLAN_TRANSLATION_IVEC_ID_DEFAULT_EDIT_PROFILE, &edit_profile_set);
  BCMDNX_IF_ERR_EXIT(rv);

  action->action_id = DPP_VLAN_TRANSLATION_IVEC_ID_DEFAULT_EDIT_PROFILE;
exit:
  _BCM_DPP_SWITCH_API_END(unit);
  BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_port_default_action_delete(
    int unit, 
    bcm_port_t port)
{
  int rv;
  bcm_dpp_vlan_edit_profile_mapping_info_t edit_profile_set;
  bcm_dpp_vlan_egress_edit_profile_info_t eg_mapping_info_unused;
  bcm_vlan_action_set_t action;

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  _BCM_DPP_SWITCH_API_START(unit);
  rv = BCM_E_NONE;

  /* Handle the API only if the SOC property is set to Normal VLAN translation mode */
  if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(unit)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Normal mode")));
  }

  bcm_vlan_action_set_t_init(&action);

  /* Set all the Port-Profiles with this Priority */
  rv = bcm_petra_vlan_translation_port_default_init(unit, action.priority);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Set all the IVECs */
  /* Init the Edit Profile structure */
  _bcm_dpp_vlan_edit_profile_mapping_info_t_init (&edit_profile_set);
  _bcm_dpp_vlan_egress_edit_profile_info_t_init(&eg_mapping_info_unused);

  /* Convert the Action set to the Edit Profile Structure */
  rv = _bcm_petra_vlan_action_to_edit_profile_info_build(unit, &action, 0, TRUE, &edit_profile_set, &eg_mapping_info_unused);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Set the IVECs in the IVEC table */
  rv = _bcm_petra_vlan_edit_profile_info_hw_set(unit, DPP_VLAN_TRANSLATION_IVEC_ID_DEFAULT_EDIT_PROFILE, &edit_profile_set);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  _BCM_DPP_SWITCH_API_END(unit);
  BCMDNX_FUNC_RETURN;
}

STATIC
int 
bcm_petra_vlan_translate_egress_action_add_per_out_ac(
    int unit, 
    SOC_PPD_AC_ID out_ac, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_dpp_vlan_egress_edit_profile_info_t eg_edit_profile_set;
    int old_profile, is_last, new_profile, is_allocated;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_EG_AC_INFO ac_info;
    bcm_dpp_vlan_edit_profile_mapping_info_t edit_profile_set_unused;
    int profile_ndx;
    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);    

    /* Convert the Action set to the Edit Profile Structure */
    rv = _bcm_petra_vlan_action_to_edit_profile_info_build(unit, action, 0, FALSE, &edit_profile_set_unused, &eg_edit_profile_set);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Add it to the SW DB to get the Edit Profile */
    rv = _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_exchange(unit, out_ac, &eg_edit_profile_set, &old_profile, &is_last, &new_profile, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* 2. Set in the HW */
    if (is_allocated) {
        /* Set the EVECs in the EVEC table */
        rv = _bcm_petra_vlan_edit_profile_info_eg_hw_set(unit, new_profile, &eg_edit_profile_set);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    /* Modify the Out-AC Entry via get */
    SOC_PPD_EG_AC_INFO_clear(&ac_info);
    soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, out_ac, &ac_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if ((action->dt_inner_pkt_prio == bcmVlanActionAdd) || 
        (action->dt_outer_pkt_prio == bcmVlanActionAdd) ||
        (action->ot_outer_pkt_prio == bcmVlanActionAdd) ||
        (action->ot_inner_pkt_prio == bcmVlanActionAdd) ||
        (action->it_outer_pkt_prio == bcmVlanActionAdd) ||
        (action->it_inner_pkt_prio == bcmVlanActionAdd) ||
        (action->ut_outer_pkt_prio == bcmVlanActionAdd) ||
        (action->ut_inner_pkt_prio == bcmVlanActionAdd) ||
        (action->dt_inner_pkt_prio == bcmVlanActionReplace) || 
        (action->dt_outer_pkt_prio == bcmVlanActionReplace) ||
        (action->ot_outer_pkt_prio == bcmVlanActionReplace) ||
        (action->ot_inner_pkt_prio == bcmVlanActionReplace) ||
        (action->it_outer_pkt_prio == bcmVlanActionReplace) ||
        (action->it_inner_pkt_prio == bcmVlanActionReplace) ||
        (action->ut_outer_pkt_prio == bcmVlanActionReplace) ||
        (action->ut_inner_pkt_prio == bcmVlanActionReplace))
    {
        /* Associate to QOS ID */
        profile_ndx = BCM_QOS_MAP_PROFILE_GET(action->priority);
    
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid egress pcp vlan profile out of range")));
        }
        if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(action->priority) || (action->priority == 0)) {
            ac_info.edit_info.pcp_profile = profile_ndx;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PCP action is mapped but priority index is invalid")));
        }
    }
    
    ac_info.edit_info.vlan_tags[DPP_VLAN_TRANSLATION_OUTER_TAG].vid = action->new_outer_vlan;
    ac_info.edit_info.vlan_tags[DPP_VLAN_TRANSLATION_INNER_TAG].vid = action->new_inner_vlan;
    ac_info.edit_info.edit_profile = new_profile;
    ac_info.edit_info.nof_tags = DPP_VLAN_TRANSLATION_MAX_NOF_TAGS;

    soc_sand_rv = soc_ppd_eg_ac_info_set(soc_sand_dev_id, out_ac, &ac_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC
int 
bcm_petra_vlan_translate_egress_action_get_per_out_ac(
    int unit, 
    SOC_PPD_AC_ID out_ac, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_dpp_vlan_egress_edit_profile_info_t eg_edit_profile_set;
    uint32  soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_EG_AC_INFO ac_info;
    bcm_dpp_vlan_edit_profile_mapping_info_t edit_profile_set_unused;
    bcm_vlan_action_set_t action_set_unused;
    
    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);    
    bcm_vlan_action_set_t_init(action);
    _bcm_dpp_vlan_edit_profile_mapping_info_t_init (&edit_profile_set_unused);


    /* Tpid actions always Modify */
    action->outer_tpid_action = bcmVlanTpidActionModify;
    action->inner_tpid_action = bcmVlanTpidActionModify;

    if (SOC_IS_ARAD(unit) && out_ac == 0) {
         /*out_ac == 0 -> return default action*/
        action->new_outer_vlan = 0;
        action->new_inner_vlan = 0;
        action->priority = 0;
        BCM_EXIT;
    }

    /* Get the Edit Profile info */
    rv = _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_data_get(unit, out_ac, &eg_edit_profile_set);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Convert it to the action */
    rv = _bcm_petra_vlan_action_from_edit_profile_info_parse(unit, &edit_profile_set_unused, &eg_edit_profile_set, &action_set_unused, action);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get the Out-AC data */
    soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, out_ac, &ac_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    action->new_outer_vlan = ac_info.edit_info.vlan_tags[DPP_VLAN_TRANSLATION_OUTER_TAG].vid;
    action->new_inner_vlan = ac_info.edit_info.vlan_tags[DPP_VLAN_TRANSLATION_INNER_TAG].vid;  
    if (ac_info.edit_info.pcp_profile == 0) {
      action->priority = ac_info.edit_info.pcp_profile;
    } else {
      BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(action->priority,ac_info.edit_info.pcp_profile);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC
int 
bcm_petra_vlan_translate_egress_action_delete_per_out_ac(
    int unit, 
    SOC_PPD_AC_ID out_ac,
    uint8 found)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_dpp_vlan_egress_edit_profile_info_t eg_edit_profile_set;
    int old_profile, is_last, new_profile, is_allocated;
    bcm_vlan_action_set_t action_set;
    SOC_PPC_EG_AC_INFO ac_info;
    unsigned int soc_sand_dev_id, soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    bcm_vlan_action_set_t_init(&action_set);

    if (found == TRUE) {
      /* Remove the Edit Profile from the SW - replace by 0 */
      _bcm_dpp_vlan_egress_edit_profile_info_t_init(&eg_edit_profile_set);
      rv = _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_exchange(unit, out_ac, &eg_edit_profile_set, &old_profile, &is_last, &new_profile, &is_allocated);
      BCMDNX_IF_ERR_EXIT(rv);

      /* 
       * Update out-ac profile
       */ 
      if (old_profile != new_profile) {
          soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, out_ac, &ac_info);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

          ac_info.edit_info.edit_profile = new_profile;
          soc_sand_rv = soc_ppd_eg_ac_info_set(soc_sand_dev_id, out_ac, &ac_info);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}



int 
bcm_petra_vlan_port_egress_default_action_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    SOC_PPD_AC_ID out_ac = 0;
    BCMDNX_INIT_FUNC_DEFS;

    /* 
     *  Retreive Out-AC. Soc_petra-B: always zero.
     *  ARAD: Configuration per port.
     */
    if (SOC_IS_ARAD(unit)) {
        SOC_PPD_PORT soc_ppd_port;
        SOC_PPD_PORT_INFO port_info;
        unsigned int soc_sand_rv;
        int core;
        _bcm_dpp_gport_info_t gport_info;

        rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);
            
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port, &core))); 

        soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        out_ac = port_info.dflt_egress_ac;
    }

    /* Get the Action via Out-AC = 0 */
    rv = bcm_petra_vlan_translate_egress_action_get_per_out_ac(unit, out_ac, action);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_port_egress_default_action_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    SOC_PPD_AC_ID out_ac = 0;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    
    /* Verify action */
    rv = _bcm_petra_vlan_translate_action_verify(unit,FALSE,action);
    BCMDNX_IF_ERR_EXIT(rv); 

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     *  Retreive Out-AC. Soc_petra-B: always zero.
     *  ARAD: Configuration per port.
     */
    if (SOC_IS_ARAD(unit)) {
        SOC_PPD_PORT soc_ppd_port;
        SOC_PPD_PORT_INFO port_info;
        unsigned int soc_sand_rv;
        int is_alloc_ac = FALSE, core;
        bcm_port_t port_i;

        /* Allocate for all ports the same AC */
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));  

            soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (port_info.dflt_egress_ac == 0) {
                if (!is_alloc_ac) {
                    /* Port has no default Egress AC. Allocate a new one */
                    rv = bcm_dpp_am_out_ac_alloc(unit, _BCM_DPP_AM_OUT_AC_TYPE_DEFAULT, 0, &out_ac);
                    BCMDNX_IF_ERR_EXIT(rv);

                    is_alloc_ac = TRUE;
                }
            }

            if (is_alloc_ac) {
                /* Write to HW only when allocating a new out ac */
                port_info.dflt_egress_ac = out_ac;
                soc_sand_rv = soc_ppd_port_info_set(unit, soc_ppd_port, &port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            else {
                out_ac = port_info.dflt_egress_ac;
            }
        }
    }

    /* Set the Global Default Action via the Out-AC = 0 */
    rv = bcm_petra_vlan_translate_egress_action_add_per_out_ac(unit, out_ac, action);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_port_egress_default_action_delete(
    int unit, 
    bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;
    SOC_PPD_AC_ID out_ac = 0;
    int core;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     *  Retreive Out-AC. Soc_petra-B: always zero.
     *  ARAD: Configuration per port.
     */
    if (SOC_IS_ARAD(unit)) {
        SOC_PPD_PORT soc_ppd_port;
        SOC_PPD_PORT_INFO port_info;
        unsigned int soc_sand_rv;
        bcm_port_t port_i;  

        /* Allocate for all ports the same AC */
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));  

            soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            out_ac = port_info.dflt_egress_ac;

            if (out_ac != 0) {
                if (BCM_E_EXISTS == bcm_dpp_am_out_ac_is_alloced(unit, out_ac)) {
                    /* Set the Default Action via the Out-AC = out_ac */
                    rv = bcm_petra_vlan_translate_egress_action_delete_per_out_ac(unit, out_ac, TRUE);
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* Port has default Egress AC. DeAllocate it */
                    rv = bcm_dpp_am_out_ac_dealloc(unit, _BCM_DPP_AM_OUT_AC_TYPE_DEFAULT, 0, out_ac);
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                port_info.dflt_egress_ac = 0;
                soc_sand_rv = soc_ppd_port_info_set(unit, soc_ppd_port, &port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);                
            }
        }
    } else { /* Soc_petra-B */
        SOC_PPD_EG_AC_INFO ac_info;
        uint32 soc_sand_rv;

        /* Set the Global Default Action via the Out-AC = 0 */
        rv = bcm_petra_vlan_translate_egress_action_delete_per_out_ac(unit, out_ac, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Restore Out-AC information to default profile */
        SOC_PPD_EG_AC_INFO_clear(&ac_info);
        ac_info.edit_info.edit_profile = 0;
        ac_info.edit_info.pcp_profile = 0;
        ac_info.edit_info.nof_tags = 2;    
        soc_sand_rv = soc_ppd_eg_ac_info_set(unit, out_ac, &ac_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Protocol-based VLAN actions {
 */

/* Related defines */
#define DPP_VLAN_PORT_PROTOCOL_SET_VLAN_TC_DATA(vlan_is_valid, vlan, tc_is_valid, tc) \
  ((1 << 17)  + (vlan << 5) + (vlan_is_valid << 4) + (tc << 1) + tc_is_valid)

#define DPP_VLAN_PORT_PROTOCOL_CLEAN_ENTRY_SET(port_protocol_info,entry_ndx) \
  (port_protocol_info->port_protocol_entry[entry_ndx].vlan_tc_data = (1 << 17))

#define DPP_VLAN_PORT_PROTOCOL_ETHER_TYPE_ENTRY_GET(port_protocol_info,entry_ndx) \
  (port_protocol_info->port_protocol_entry[entry_ndx].ethertype)

#define DPP_VLAN_PORT_PROTOCOL_IS_VALID_ENTRY(port_protocol_info,entry_ndx) \
  (port_protocol_info->port_protocol_entry[entry_ndx].vlan_tc_data != 0)

#define DPP_VLAN_PORT_PROTOCOL_TC_ENTRY_GET(port_protocol_info,entry_ndx) \
  ((port_protocol_info->port_protocol_entry[entry_ndx].vlan_tc_data >> 1) & (0x7)) 

#define DPP_VLAN_PORT_PROTOCOL_TC_IS_VALID_ENTRY_GET(port_protocol_info,entry_ndx) \
  (port_protocol_info->port_protocol_entry[entry_ndx].vlan_tc_data & (0x1))

#define DPP_VLAN_PORT_PROTOCOL_VLAN_IS_VALID_ENTRY_GET(port_protocol_info,entry_ndx) \
  ((port_protocol_info->port_protocol_entry[entry_ndx].vlan_tc_data >> 4) & (0x1))

#define DPP_VLAN_PORT_PROTOCOL_VLAN_ENTRY_GET(port_protocol_info,entry_ndx) \
  ((port_protocol_info->port_protocol_entry[entry_ndx].vlan_tc_data >> 5) & (0xfff))

/* HW Set per profile */
STATIC int
_bcm_petra_vlan_port_protocol_hw_set(int unit, 
                                     int new_template, 
                                     bcm_dpp_vlan_port_protocol_entries_t *port_protocol_info)
{
    uint32 soc_sand_dev_id, soc_sand_rv;
    int entry_ndx;
    SOC_PPD_LLP_VID_ASSIGN_PRTCL_INFO prtcl_assign_info;
    SOC_PPD_LLP_COS_PRTCL_INFO llp_cos_prtcl_info;
    bcm_port_ethertype_t ether_type;
    bcm_vlan_t vlan;
    int vlan_is_valid, tc_is_valid;
    bcm_cos_t tc;
    SOC_SAND_SUCCESS_FAILURE soc_sand_success;
    
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    for (entry_ndx = 0; entry_ndx < DPP_PRTCL_NOF_ETHER_TYPE_IDS; ++entry_ndx) {

        if (!DPP_VLAN_PORT_PROTOCOL_IS_VALID_ENTRY(port_protocol_info,entry_ndx)) {
            /* invalid entry no need to search anymore. Entry not found */
            break;
        }

        ether_type = DPP_VLAN_PORT_PROTOCOL_ETHER_TYPE_ENTRY_GET(port_protocol_info,entry_ndx);
        vlan_is_valid = DPP_VLAN_PORT_PROTOCOL_VLAN_IS_VALID_ENTRY_GET(port_protocol_info,entry_ndx);
        tc_is_valid = DPP_VLAN_PORT_PROTOCOL_TC_IS_VALID_ENTRY_GET(port_protocol_info,entry_ndx);
        vlan = DPP_VLAN_PORT_PROTOCOL_VLAN_ENTRY_GET(port_protocol_info,entry_ndx);
        tc = DPP_VLAN_PORT_PROTOCOL_TC_ENTRY_GET(port_protocol_info,entry_ndx);

        SOC_PPD_LLP_VID_ASSIGN_PRTCL_INFO_clear(&prtcl_assign_info);
        prtcl_assign_info.vid_is_valid = vlan_is_valid;
        prtcl_assign_info.vid = vlan;

        soc_sand_rv = soc_ppd_llp_vid_assign_protocol_based_set(soc_sand_dev_id, new_template,
            ether_type, &prtcl_assign_info, &soc_sand_success);
        SOC_SAND_IF_ERR_OR_FAIL_RETURN(soc_sand_rv, soc_sand_success);

        SOC_PPD_LLP_COS_PRTCL_INFO_clear(&llp_cos_prtcl_info);
        llp_cos_prtcl_info.tc_is_valid = tc_is_valid;
        llp_cos_prtcl_info.tc = tc;

        soc_sand_rv = soc_ppd_llp_cos_protocol_based_set(soc_sand_dev_id, new_template,
            ether_type, &llp_cos_prtcl_info, &soc_sand_success);
        SOC_SAND_IF_ERR_OR_FAIL_RETURN(soc_sand_rv, soc_sand_success);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function convert from old to new entries according to data */
STATIC int
_bcm_petra_vlan_port_protocol_old_to_new(int unit, 
                                         bcm_dpp_vlan_port_protocol_entries_t *old_port_protocol_entries, 
                                         bcm_port_ethertype_t ether, 
                                         int vlan_is_valid,
                                         bcm_vlan_t vlan,
                                         int tc_is_valid,
                                         bcm_cos_t tc,
                                         bcm_dpp_vlan_port_protocol_entries_t *new_port_protocol_entries)
{
    bcm_error_t rv = BCM_E_NONE;
    int entry_ndx, old_entry_ndx = 0;
    int found, old_entry_count = 0;
    uint32 vlan_tc_data;
    bcm_port_ethertype_t ether_i;

    BCMDNX_INIT_FUNC_DEFS;

    for (entry_ndx = 0; entry_ndx < DPP_PRTCL_NOF_ETHER_TYPE_IDS; ++entry_ndx) {
        new_port_protocol_entries->port_protocol_entry[entry_ndx].ethertype = 0;
        new_port_protocol_entries->port_protocol_entry[entry_ndx].vlan_tc_data = 0;
		
        if (DPP_VLAN_PORT_PROTOCOL_IS_VALID_ENTRY(old_port_protocol_entries,entry_ndx)) {
            old_entry_count++;
        }
    }
    vlan_tc_data = DPP_VLAN_PORT_PROTOCOL_SET_VLAN_TC_DATA(vlan_is_valid, vlan, tc_is_valid, tc);
    /*
     * 1. Look for match ether type in table and get index. 
     * On Search assuming ether type is ordered from low to high 
     * 2. Set new data according to: 
     * 2.a. If new entry and not invalid one E_FOUND.  
     * 2.b. If new entry (and not invalid one) insert according to order. 
     * 2.c. If old entry and not an invalid entry => insert to same index. 
     * 2.d. If old entry that is now invalid =>  remove and set according to order.
     */    
    
    /* 1. Look for match ether type in table and get index. */
    found = FALSE;
    for (entry_ndx = 0; entry_ndx < DPP_PRTCL_NOF_ETHER_TYPE_IDS; ++entry_ndx) {

        if (!DPP_VLAN_PORT_PROTOCOL_IS_VALID_ENTRY(old_port_protocol_entries,entry_ndx)) {
            /* invalid entry no need to search anymore. Entry not found */
            break;
        }

        ether_i = DPP_VLAN_PORT_PROTOCOL_ETHER_TYPE_ENTRY_GET(old_port_protocol_entries,entry_ndx);
        if (ether == ether_i) {
            /* index found, already exist */
            found = TRUE;
            break;
        }

        if (ether_i < ether) {
            /* index not found, since table is ordered from low to high */
            break;
        }

        new_port_protocol_entries->port_protocol_entry[entry_ndx].ethertype = 
          old_port_protocol_entries->port_protocol_entry[entry_ndx].ethertype;
        new_port_protocol_entries->port_protocol_entry[entry_ndx].vlan_tc_data = 
          old_port_protocol_entries->port_protocol_entry[entry_ndx].vlan_tc_data;
    }

    /* 2.a. If new entry and invalid one return BCM_E_NOT_FOUND */
    if (!found && (!tc_is_valid && !vlan_is_valid)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Ether type not found")));;
    }

    /* 2.b. If new entry (and not invalid one) insert according to order. */
    if (!found && (tc_is_valid || vlan_is_valid)) {
        /* entry_ndx is the current index that we should insert */
        /* special case 1.: table is full */
        if (entry_ndx == DPP_PRTCL_NOF_ETHER_TYPE_IDS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Ether table is full")));
        }

        /* special case 2.: last entry is valid. i.e. table is full */
        if (DPP_VLAN_PORT_PROTOCOL_IS_VALID_ENTRY(new_port_protocol_entries,(DPP_PRTCL_NOF_ETHER_TYPE_IDS-1))){
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Ether table is full")));
        }

        /* special case 3.: old table is full and insert a new entry */
        if (old_entry_count == DPP_PRTCL_NOF_ETHER_TYPE_IDS){
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Ether table is full")));
        }

        /* Fill table entry first with new entry */
        old_entry_ndx = entry_ndx;
        new_port_protocol_entries->port_protocol_entry[entry_ndx].ethertype = ether;
        new_port_protocol_entries->port_protocol_entry[entry_ndx++].vlan_tc_data = vlan_tc_data;
                
    }

    /* 2.c. If old entry and not an invalid entry => insert to same index. */
    if (found && (tc_is_valid || vlan_is_valid)) {
      new_port_protocol_entries->port_protocol_entry[entry_ndx].ethertype = ether;
      new_port_protocol_entries->port_protocol_entry[entry_ndx++].vlan_tc_data = vlan_tc_data;
      old_entry_ndx = entry_ndx;
    }

    /* 2.d. If old entry that is now invalid =>  remove and set according to order. */
    if (found && !(tc_is_valid || vlan_is_valid)) {
      /* skip this entry */
      old_entry_ndx = entry_ndx + 1;  
    }

    /* Fill table to end according to 2.a,2.b,2.c */
    for (; (old_entry_ndx < DPP_PRTCL_NOF_ETHER_TYPE_IDS) && (entry_ndx < DPP_PRTCL_NOF_ETHER_TYPE_IDS); ++old_entry_ndx) {
        new_port_protocol_entries->port_protocol_entry[entry_ndx].ethertype = 
          old_port_protocol_entries->port_protocol_entry[old_entry_ndx].ethertype;
        new_port_protocol_entries->port_protocol_entry[entry_ndx++].vlan_tc_data = 
          old_port_protocol_entries->port_protocol_entry[old_entry_ndx].vlan_tc_data;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
   BCMDNX_FUNC_RETURN;
}

/* Function set Protocol based entry per request. Commit to HW when needed */
STATIC int
_bcm_petra_vlan_port_protocol_entry_set(
    int unit, 
    bcm_port_t local_port, 
    bcm_port_ethertype_t ether, 
    int vlan_is_valid,
    bcm_vlan_t vlan,
    int tc_is_valid,
    bcm_cos_t tc)
{
    SOC_PPD_PORT soc_ppd_port;
    bcm_dpp_vlan_port_protocol_entries_t old_port_protocol_entries, new_port_protocol_entries;
    bcm_dpp_vlan_port_protocol_entries_t *old_point = NULL;
    bcm_error_t rv = BCM_E_NONE;
    int old_template, new_template, core;
    int is_last, is_allocated, entry_ndx;
    uint32 soc_sand_rv, soc_sand_dev_id;
    SOC_PPD_LLP_VID_ASSIGN_PORT_INFO port_vid_assign_info;
    SOC_PPD_LLP_COS_PORT_INFO port_cos_info;
    SOC_SAND_SUCCESS_FAILURE soc_sand_success;
    bcm_port_ethertype_t ether_i;
    uint8 remove_old;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&new_port_protocol_entries, 0, sizeof(new_port_protocol_entries));

    soc_sand_dev_id = (unit);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));  

    old_point = &old_port_protocol_entries;

    /* Retreive information */
    rv = _bcm_dpp_am_template_vlan_port_protocol_data_get(unit, soc_ppd_port, &old_port_protocol_entries);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update information */
    rv = _bcm_petra_vlan_port_protocol_old_to_new(unit, &old_port_protocol_entries, ether, 
                                                  vlan_is_valid, vlan, tc_is_valid, tc, &new_port_protocol_entries);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check whether l2_next_prtcl_type can be successfully allocated. */
    if (tc_is_valid || vlan_is_valid) {
        soc_sand_rv = soc_ppd_l2_next_prtcl_type_allocate_test(unit, ether, &soc_sand_success);
        SOC_SAND_IF_ERR_OR_FAIL_RETURN(soc_sand_rv, soc_sand_success);
    }

    /* Exchange */
    rv = _bcm_dpp_am_template_vlan_port_protocol_exchange(unit, soc_ppd_port, &new_port_protocol_entries, &old_template, &is_last, &new_template, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    /* New profile set information */
    if (is_allocated) {
        rv = _bcm_petra_vlan_port_protocol_hw_set(unit, new_template, &new_port_protocol_entries);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* In case old is different than new and new is not an invalid profile (default profile that indicates port disable) */
    if ((old_template != new_template) && (new_template != NOS_PRTCL_PORT_PROFILES)) {
        SOC_PPD_PORT_INFO port_info;

        SOC_PPD_PORT_INFO_clear(&port_info);

        soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);          
        /* set new profile */
        port_info.ether_type_based_profile = new_template;

        soc_sand_rv = soc_ppd_port_info_set(soc_sand_dev_id, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);          
    }

    /* Enable / Disable port */
    SOC_PPD_LLP_VID_ASSIGN_PORT_INFO_clear(&port_vid_assign_info);
    SOC_PPD_LLP_COS_PORT_INFO_clear(&port_cos_info);

    soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_vid_assign_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    port_vid_assign_info.enable_protocol = (new_template != NOS_PRTCL_PORT_PROFILES) ? TRUE:FALSE;
    
    soc_sand_rv = soc_ppd_llp_vid_assign_port_info_set(soc_sand_dev_id, soc_ppd_port, &port_vid_assign_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);   
    
    soc_sand_rv = soc_ppd_llp_cos_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_cos_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    port_cos_info.l2_info.use_l2_protocol = (new_template != NOS_PRTCL_PORT_PROFILES) ? TRUE:FALSE;
    
    soc_sand_rv = soc_ppd_llp_cos_port_info_set(soc_sand_dev_id, soc_ppd_port, &port_cos_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);    

    /*
     * Remove old entries according to: 
     * 1. If old template is not equal to new template => remove all old entries.  
     * 2. If old template is equal to new template. 
     * 2.a. If remove an old entry => remove the entry. 
     * 2.b. If add a new entry =>  no entries need to be removed.
     */ 	
    if (is_last) {
        remove_old = TRUE;
		
        if (old_template != new_template) {
            /* 1. If old template is not equal to new template => remove all old entries */
            for (entry_ndx = 0; entry_ndx < DPP_PRTCL_NOF_ETHER_TYPE_IDS; ++entry_ndx) {
                if (DPP_VLAN_PORT_PROTOCOL_IS_VALID_ENTRY(old_point,entry_ndx)) {
                    DPP_VLAN_PORT_PROTOCOL_CLEAN_ENTRY_SET(old_point,entry_ndx);
                }
            }
        }
        else {
            /* 2. If old template is equal to new template */
            if (!tc_is_valid && !vlan_is_valid) {
                /* 2.a. If remove an old entry => remove the entry */
                for (entry_ndx = 0; entry_ndx < DPP_PRTCL_NOF_ETHER_TYPE_IDS; ++entry_ndx) {
                    if (DPP_VLAN_PORT_PROTOCOL_IS_VALID_ENTRY(old_point,entry_ndx)) {
						
                        ether_i = DPP_VLAN_PORT_PROTOCOL_ETHER_TYPE_ENTRY_GET(old_point,entry_ndx);
                        if (ether == ether_i) {
                            DPP_VLAN_PORT_PROTOCOL_CLEAN_ENTRY_SET(old_point,entry_ndx);
                            break;
                        }
                    }
                }				
            }
            else {
                /* 2.b. If add a new entry =>  no entries need to be removed */
                remove_old = FALSE;
            }
        }

        if (remove_old) {
            rv = _bcm_petra_vlan_port_protocol_hw_set(unit, old_template, &old_port_protocol_entries);
        }
        BCMDNX_IF_ERR_EXIT(rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
   BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_vlan_port_protocol_action_to_info(int unit,
                                             bcm_vlan_action_set_t *action,
                                             int *vlan_is_valid,
                                             bcm_vlan_t *vlan,
                                             int *tc_is_valid,
                                             bcm_cos_t *tc)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (action->ut_outer == bcmVlanActionAdd) {
        if (!BCM_VLAN_VALID(action->new_outer_vlan)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given invalid VLAN")));
        }
        *vlan = action->new_outer_vlan;
        *vlan_is_valid = TRUE;
    } else {
        *vlan_is_valid = FALSE;
    }
    
    if (action->priority == BCM_COS_INVALID) {
        *tc_is_valid = FALSE;
    } else {
        if (!(action->priority >= BCM_COS_MIN && action->priority <= BCM_COS_MAX)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given invalid COS")));
        }
        *tc_is_valid = TRUE;
        *tc = action->priority;
    }

    if (!(*tc_is_valid || *vlan_is_valid)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given both invalid tc and vlan")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function   :
 *      bcm_petra_vlan_port_protocol_action_add
 * Description   :
 *      Add protocol based VLAN with specified action.
 *      If the entry already exists, update the action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type. Not used
 *      ether     (IN) 16 bit ether type
 *      action    (IN) Action for outer tag and priority
 * Note:
 */
int 
bcm_petra_vlan_port_protocol_action_add(
    int unit, 
    bcm_port_t port, 
    bcm_port_frametype_t frame, 
    bcm_port_ethertype_t ether, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    int     local_port;
    int     tc_is_valid = 0, vlan_is_valid = 0;
    bcm_cos_t tc = 0;
    bcm_vlan_t vlan = 0;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);    
    rv = BCM_E_NONE;

    BCMDNX_NULL_CHECK(action);

    if (port == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port is invalid")));
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);


    if ((action->dt_outer     != bcmVlanActionNone) ||
        (action->dt_outer_prio != bcmVlanActionNone) ||
        (action->dt_inner      != bcmVlanActionNone) ||
        (action->dt_inner_prio != bcmVlanActionNone) ||
        (action->ot_outer      != bcmVlanActionNone) ||
        (action->ot_outer_prio != bcmVlanActionNone) ||
        (action->ot_inner      != bcmVlanActionNone) ||
        (action->it_outer      != bcmVlanActionNone) ||
        (action->it_inner      != bcmVlanActionNone) ||
        (action->it_inner_prio != bcmVlanActionNone) ||
        ((action->ut_outer      != bcmVlanActionAdd && 
         action->ut_outer      != bcmVlanActionNone)) ||
        (action->ut_inner      != bcmVlanActionNone) ||
        (action->outer_pcp     != bcmVlanPcpActionNone) ||
        (action->inner_pcp     != bcmVlanPcpActionNone) ||
        (action->new_inner_vlan != 0) ||
        (!BCM_VLAN_VALID(action->new_outer_vlan))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid action parameters.")));
    }

    rv = _bcm_petra_vlan_port_protocol_action_to_info(unit, action, &vlan_is_valid, &vlan, &tc_is_valid, &tc);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!tc_is_valid && !vlan_is_valid) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Addition: both VLAN and TC are invalid")));
    }



    /* Configuration per port */
    PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
        rv = _bcm_petra_vlan_port_protocol_entry_set(unit, local_port, ether, vlan_is_valid, vlan, tc_is_valid, tc);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_vlan_port_protocol_to_action_build(int unit,
                                              bcm_port_ethertype_t ether, 
                                              bcm_dpp_vlan_port_protocol_entries_t* port_protocol_info,
                                              bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    int entry_ndx;
    int found;
    int tc_is_valid, vlan_is_valid;
    bcm_cos_t tc;
    bcm_vlan_t vlan;
    bcm_port_ethertype_t ether_i;

    BCMDNX_INIT_FUNC_DEFS;

    found = FALSE;
    /* Find related ether type */
    for (entry_ndx = 0; entry_ndx < DPP_PRTCL_NOF_ETHER_TYPE_IDS; ++entry_ndx) {
        if (!DPP_VLAN_PORT_PROTOCOL_IS_VALID_ENTRY(port_protocol_info,entry_ndx)) {
            /* invalid entry no need to search anymore. Entry not found */
            break;
        }

        ether_i = DPP_VLAN_PORT_PROTOCOL_ETHER_TYPE_ENTRY_GET(port_protocol_info,entry_ndx);
        if (ether == ether_i) {
            /* index found, already exist */
            found = TRUE;
            break;
        }

        if (ether_i < ether) {
            /* index not found, since table is ordered from low to high */
            break;
        }
    }

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Ether type not found")));
    }

    /* fill action */
    vlan_is_valid = DPP_VLAN_PORT_PROTOCOL_VLAN_IS_VALID_ENTRY_GET(port_protocol_info,entry_ndx);
    tc_is_valid = DPP_VLAN_PORT_PROTOCOL_TC_IS_VALID_ENTRY_GET(port_protocol_info,entry_ndx);
    vlan = DPP_VLAN_PORT_PROTOCOL_VLAN_ENTRY_GET(port_protocol_info,entry_ndx);
    tc = DPP_VLAN_PORT_PROTOCOL_TC_ENTRY_GET(port_protocol_info,entry_ndx);
    if (vlan_is_valid) {
      action->ut_outer = bcmVlanActionAdd;
      action->new_outer_vlan = vlan;
    }

    if (tc_is_valid) {
      action->priority = tc;
    } else {
      action->priority = BCM_COS_INVALID;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      bcm_petra_vlan_port_protocol_action_get
 * Description   :
 *      Get protocol based VLAN with specified action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type. Not used
 *      ether     (IN) 16 bit ether type
 *      action    (OUT) Action for outer and priority
 * Note:
 *    
 */
int 
bcm_petra_vlan_port_protocol_action_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_frametype_t frame, 
    bcm_port_ethertype_t ether, 
    bcm_vlan_action_set_t *action)
{
    int     core;
    bcm_error_t rv = BCM_E_NONE;
    bcm_dpp_vlan_port_protocol_entries_t port_protocol_entries;
    SOC_PPD_PORT soc_ppd_port;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    if (port == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Invalid port.")));
    }

    BCMDNX_NULL_CHECK(action);

    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port, &core)));  

    rv = _bcm_dpp_am_template_vlan_port_protocol_data_get(unit,soc_ppd_port,&port_protocol_entries);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_vlan_port_protocol_to_action_build(unit, ether, &port_protocol_entries, action);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function   :
 *      bcm_petra_vlan_port_protocol_action_delete
 * Description   :
 *      Delete protocol based VLAN action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type. Not used
 *      ether     (IN) 16 bit ether type
 * Note:
 */
int 
bcm_petra_vlan_port_protocol_action_delete(
    int unit, 
    bcm_port_t port, 
    bcm_port_frametype_t frame, 
    bcm_port_ethertype_t ether)
{
    bcm_error_t rv;
    int tc_is_valid, vlan_is_valid;
    int     local_port;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* Same as Add with invalid entry*/
    tc_is_valid = 0;
    vlan_is_valid = 0;

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    /* Configuration per port */
    PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
        rv = _bcm_petra_vlan_port_protocol_entry_set(unit, local_port, ether, vlan_is_valid, 0, tc_is_valid, 0);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function   :
 *      bcm_petra_vlan_port_protocol_action_delete_all
 * Description   :
 *      Delete all protocol based VLAN actiona.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 * Note:
 */
int 
bcm_petra_vlan_port_protocol_action_delete_all(
    int unit, 
    bcm_port_t port)
{
    bcm_error_t rv;
    int tc_is_valid, vlan_is_valid;
    bcm_vlan_t  vlan = 0;
    bcm_cos_t   tc = 0;
    int     local_port, entry_ndx, core;
    bcm_dpp_vlan_port_protocol_entries_t port_protocol_entries;
    bcm_dpp_vlan_port_protocol_entries_t *point = NULL;
    bcm_port_ethertype_t ether_type;
    SOC_PPD_PORT soc_ppd_port;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    point = &port_protocol_entries;

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    /* Delete all ether types of given pbmp */
    PBMP_ITER(gport_info.pbmp_local_ports, local_port) {

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));  

        rv = _bcm_dpp_am_template_vlan_port_protocol_data_get(unit,soc_ppd_port,&port_protocol_entries);
        BCMDNX_IF_ERR_EXIT(rv);


        /* remove all ether types */
        for (entry_ndx = 0; entry_ndx < DPP_PRTCL_NOF_ETHER_TYPE_IDS; ++entry_ndx)
        {
            if (!DPP_VLAN_PORT_PROTOCOL_IS_VALID_ENTRY(point,entry_ndx))
            {
                /* invalid entry */
                continue;
            }
            ether_type = DPP_VLAN_PORT_PROTOCOL_ETHER_TYPE_ENTRY_GET(point,entry_ndx);
            tc_is_valid = 0;
            vlan_is_valid = 0;

            /* Configuration per port */
            rv = _bcm_petra_vlan_port_protocol_entry_set(unit, local_port, ether_type, vlan_is_valid, vlan, tc_is_valid, tc);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function   :
 *      bcm_petra_vlan_port_protocol_action_traverse
 * Description   :
 *      Traverse over vlan port protocol actions. 
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      cb        (IN) User provided call back function
 *      user_data (IN) User provided data
 * Note:
 */
int 
bcm_petra_vlan_port_protocol_action_traverse(
    int unit, 
    bcm_vlan_port_protocol_action_traverse_cb cb, 
    void *user_data)
{
    bcm_error_t                 rv = BCM_E_NONE;
    bcm_port_frametype_t        ft;
    bcm_port_ethertype_t        et;
    bcm_vlan_action_set_t       action;
    bcm_port_t                  port;
    bcm_dpp_vlan_port_protocol_entries_t port_protocol_info;
    bcm_dpp_vlan_port_protocol_entries_t *protocol_ptr = NULL;
    SOC_PPD_PORT                    soc_ppd_port;
    int                         entry_ndx, core;

    BCMDNX_INIT_FUNC_DEFS;


    /* Frame is not used */
    ft = 0;
    protocol_ptr = &port_protocol_info;

    /* Iterate over all ethernet ports */
    PBMP_ITER(PBMP_E_ALL(unit), port) {

      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));  

      rv = _bcm_dpp_am_template_vlan_port_protocol_data_get(unit, soc_ppd_port, &port_protocol_info);
      BCMDNX_IF_ERR_EXIT(rv);

      /* Iterate over all valid ether types */
      for (entry_ndx = 0; entry_ndx < DPP_PRTCL_NOF_ETHER_TYPE_IDS; ++entry_ndx) {
          if (DPP_VLAN_PORT_PROTOCOL_IS_VALID_ENTRY(protocol_ptr,entry_ndx)) {
              bcm_vlan_action_set_t_init(&action);
              
              et = DPP_VLAN_PORT_PROTOCOL_ETHER_TYPE_ENTRY_GET(protocol_ptr,entry_ndx);
              rv = _bcm_petra_vlan_port_protocol_to_action_build(unit,et,&port_protocol_info,&action);
              BCMDNX_IF_ERR_EXIT(rv);

              /* Valid entry call cb */
              rv = (cb(unit, port, ft, et, &action, user_data));
              BCMDNX_IF_ERR_EXIT(rv);
          }
      }
    }

    BCM_EXIT;
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Protocol-based VLAN actions }
 */

int 
bcm_petra_vlan_mac_action_traverse(
    int unit, 
    bcm_vlan_mac_action_traverse_cb cb, 
    void *user_data)
{
    bcm_error_t rv;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    const int block_size = 130;
    bcm_mac_t mac;
    bcm_vlan_action_set_t action;

    SOC_PPD_LLP_VID_ASSIGN_MATCH_RULE rule;
    SOC_SAND_TABLE_BLOCK_RANGE        block_range;
    SOC_SAND_PP_MAC_ADDRESS           *mac_address_key_arr = NULL;
    SOC_PPD_LLP_VID_ASSIGN_MAC_INFO   *vid_assign_info_arr = NULL;
    uint32                        i;
    uint32                      nof_entries;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);
   

    BCMDNX_ALLOC(mac_address_key_arr,
              block_size * sizeof(SOC_SAND_PP_MAC_ADDRESS),
              "mac_address_key_arr");
    if (!mac_address_key_arr) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG("unable to allocate %d bytes edit profile info"),
                          block_size * (int)sizeof(SOC_SAND_PP_MAC_ADDRESS)));
    }
    BCMDNX_ALLOC(vid_assign_info_arr,
              block_size * sizeof(SOC_PPD_LLP_VID_ASSIGN_MAC_INFO),
              "vid_assign_info_arr");
    if (!vid_assign_info_arr) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG("unable to allocate %d bytes edit profile info"),
                          block_size * (int)sizeof(SOC_PPD_LLP_VID_ASSIGN_MAC_INFO)));
    }

    SOC_PPD_LLP_VID_ASSIGN_MATCH_RULE_clear(&rule);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

    rule.rule_type = SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_ALL;
    rule.vid = SOC_PPD_IGNORE_VAL;
    block_range.iter = 0;
    block_range.entries_to_act = block_size;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    while (1) {
        soc_sand_rv = soc_ppd_llp_vid_assign_mac_based_get_block(soc_sand_dev_id, &rule,
                      &block_range, mac_address_key_arr, vid_assign_info_arr,
                      &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (nof_entries == 0) {
            break;
        }

        bcm_vlan_action_set_t_init(&action);
        for (i = 0; i < nof_entries; ++i) {
            rv = _bcm_petra_mac_from_sand_mac(mac, &mac_address_key_arr[i]);
            BCMDNX_IF_ERR_EXIT(rv);

            _bcm_petra_vlan_mac_action_fill_from_info(unit, &action, &(vid_assign_info_arr[i]));

            rv = cb(unit, mac, &action, user_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

  BCM_EXIT;
exit:
  if (mac_address_key_arr != NULL) {
      BCM_FREE(mac_address_key_arr);
  }
  if (vid_assign_info_arr != NULL) {
      BCM_FREE(vid_assign_info_arr);
  }

  _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);

  BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_mac_action_add(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    
    SOC_SAND_PP_MAC_ADDRESS soc_sand_mac_address_key;
    SOC_PPD_LLP_VID_ASSIGN_MAC_INFO mac_based_info;
    SOC_SAND_SUCCESS_FAILURE  success;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    if ((action->dt_outer_prio != bcmVlanActionNone) ||
        (action->dt_inner_prio != bcmVlanActionNone) ||
        (action->dt_outer      != bcmVlanActionNone) ||
        (action->dt_inner      != bcmVlanActionNone) ||
        (action->dt_inner_prio != bcmVlanActionNone) ||
        (action->ot_outer_prio != bcmVlanActionNone) ||
        (action->ot_inner      != bcmVlanActionNone) ||
        (action->it_outer      != bcmVlanActionNone) ||
        (action->it_inner      != bcmVlanActionNone) ||
        (action->it_inner_prio != bcmVlanActionNone) ||
        (action->ut_inner      != bcmVlanActionNone) ||
        (action->outer_pcp     != bcmVlanPcpActionNone) ||
        (action->inner_pcp     != bcmVlanPcpActionNone)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("One or more of the action parameters is invalid.")));
    }
    if (action->ut_outer!=bcmVlanActionAdd && action->ot_outer!=bcmVlanActionReplace) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action must set either action->ot_outer to Replace or action->ut_outer to Add.")));
    }   

    soc_sand_dev_id = (unit);
    
    soc_sand_SAND_PP_MAC_ADDRESS_clear(&soc_sand_mac_address_key);
    SOC_PPD_LLP_VID_ASSIGN_MAC_INFO_clear(&mac_based_info);
    
    mac_based_info.vid = action->new_outer_vlan;

    if (action->ut_outer==bcmVlanActionAdd){
        mac_based_info.use_for_untagged = TRUE;
    } else if (action->ut_outer != bcmVlanActionNone) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ut_outer must be bcmVlanActionNone or bcmVlanActionAdd.")));
    }

    if (action->ot_outer == bcmVlanActionReplace) {
        mac_based_info.override_tagged = TRUE;
    } else if (action->ot_outer != bcmVlanActionNone) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_outer must be bcmVlanActionNone or bcmVlanActionReplace.")));
    }

    rv = _bcm_petra_mac_to_sand_mac(mac, &soc_sand_mac_address_key);
    BCMDNX_IF_ERR_EXIT(rv);
    
    soc_sand_rv = soc_ppd_llp_vid_assign_mac_based_add(soc_sand_dev_id, &soc_sand_mac_address_key, &mac_based_info, &success);
    SOC_SAND_IF_ERR_OR_FAIL_RETURN(soc_sand_rv, success);
    
    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_mac_action_get(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint8 found;
    
    SOC_SAND_PP_MAC_ADDRESS soc_sand_mac_address_key;
    SOC_PPD_LLP_VID_ASSIGN_MAC_INFO mac_based_info;
       
    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    
    soc_sand_SAND_PP_MAC_ADDRESS_clear(&soc_sand_mac_address_key);
    SOC_PPD_LLP_VID_ASSIGN_MAC_INFO_clear(&mac_based_info);
    
   rv = _bcm_petra_mac_to_sand_mac(mac, &soc_sand_mac_address_key);
    BCMDNX_IF_ERR_EXIT(rv);
    
    soc_sand_rv = soc_ppd_llp_vid_assign_mac_based_get(soc_sand_dev_id, &soc_sand_mac_address_key, &mac_based_info, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Mac address not found.")));
    }

    _bcm_petra_vlan_mac_action_fill_from_info(unit, action, &mac_based_info);
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_vlan_mac_action_fill_from_info(int unit, bcm_vlan_action_set_t *action, SOC_PPD_LLP_VID_ASSIGN_MAC_INFO *info){
    BCMDNX_INIT_FUNC_DEFS;
    bcm_vlan_action_set_t_init(action);     
    action->new_outer_vlan = info->vid;   
    if (info->use_for_untagged) {      
        action->ut_outer = bcmVlanActionAdd;    
    }   
    if (info->override_tagged) {   
        action->ot_outer = bcmVlanActionReplace;    
    }   
    BCMDNX_FUNC_RETURN;
} 


int 
bcm_petra_vlan_mac_action_delete(
    int unit, 
    bcm_mac_t mac)
{
    bcm_error_t rv;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    
    SOC_SAND_PP_MAC_ADDRESS soc_sand_mac_address_key;
       
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    soc_sand_dev_id = (unit);
    
    soc_sand_SAND_PP_MAC_ADDRESS_clear(&soc_sand_mac_address_key);
    
    rv = _bcm_petra_mac_to_sand_mac(mac, &soc_sand_mac_address_key);
    BCMDNX_IF_ERR_EXIT(rv);
    
    soc_sand_rv = soc_ppd_llp_vid_assign_mac_based_remove(soc_sand_dev_id, &soc_sand_mac_address_key);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
   
    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
bcm_petra_vlan_mac_action_delete_all_cb(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_action_set_t *action, 
    void *user_data)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(bcm_petra_vlan_mac_action_delete(unit, mac));
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_vlan_mac_action_delete_all(
    int unit)
{
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    rv = bcm_petra_vlan_mac_action_traverse(unit, bcm_petra_vlan_mac_action_delete_all_cb, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/* 
 * Build the In-AC Key
 */ 
STATIC
int 
_bcm_petra_vlan_translate_action_in_ac_key_build(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    SOC_PPD_L2_LIF_AC_KEY *in_ac_key)
{
  bcm_error_t rv = BCM_E_NONE;
  int core;
  uint32 soc_sand_rv;
  int is_local;
  SOC_PPD_PORT_INFO port_info;
  SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE ac_key_type;
  SOC_PPD_PORT soc_ppd_port;
  int gport_type, gport_val;
  int lif_id, fec_id;
  _bcm_dpp_gport_info_t gport_info;
  BCMDNX_INIT_FUNC_DEFS;
  
  /* Build the In-AC Key */
  SOC_PPD_L2_LIF_AC_KEY_clear(in_ac_key);  

  rv = _bcm_dpp_gport_parse(unit,port,&gport_type, &gport_val, NULL);
  BCMDNX_IF_ERR_EXIT(rv);

  if (gport_type == _BCM_DPP_GPORT_TYPE_SIMPLE) {
      /* Simple conversation where port is physical */
      /* Get the Port VLAN Domain */
      rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
      BCMDNX_IF_ERR_EXIT(rv);

      SOC_PPD_PORT_INFO_clear(&port_info);

      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port, &core)));  
      soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      switch (key_type) {
        case bcmVlanTranslateKeyPortDouble:
          ac_key_type = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN;

          /* Validate the VLANs before using them for the key */
          BCM_DPP_VLAN_CHK_ID(unit, outer_vlan);
          BCM_DPP_VLAN_CHK_ID(unit, inner_vlan);
          in_ac_key->outer_vid = outer_vlan;
          in_ac_key->inner_vid = inner_vlan;
          break;
      case bcmVlanTranslateKeyPortOuter:
        if (_BCM_DPP_VT_TRANSLATION_PROFILE_INITIAL_VID(port_info.vlan_translation_profile)) {
            ac_key_type = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN;
        } else {
            ac_key_type = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
        }
        /* Verify the Outer-VLAN is not invalid, before using it for the key */
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan);
        in_ac_key->outer_vid = outer_vlan;
        break;
      default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Translation key is invalid")));
      }
      in_ac_key->key_type = ac_key_type;
      /* Get the VLAN domain from the Port attributes */
      in_ac_key->vlan_domain = port_info.vlan_domain;

  } else {
    if (key_type != bcmVlanTranslateKeyPortOuter || outer_vlan != BCM_VLAN_INVALID) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When gport is logical - vlan_port or mpls_port. Key type must be bcmVlanTranslateKeyPortOuter and outer vlan is invalid")));
    }
    /* VLAN Port GPORT - retreive LIF index directly */
    if (BCM_GPORT_IS_VLAN_PORT(port)) {
        bcm_vlan_port_t vlan_port;

        rv = _bcm_dpp_gport_to_lif(unit, port, &lif_id, NULL, &fec_id, &is_local);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_local) {
            /* API is relevant only for local configuration */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unexpected lif index. should be local only")));
        }

        rv = _bcm_dpp_in_lif_ac_match_get(unit, &vlan_port, lif_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_fill_ac_key(unit,&vlan_port,BCM_GPORT_VLAN_PORT,in_ac_key);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unexpected gport type. Must be physical or vlan_port")));
    }
  }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
STATIC
int 
_bcm_petra_vlan_translate_out_ac_to_remove(
    int unit, 
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    SOC_PPD_AC_ID out_ac,
    uint8 *is_out_ac_to_remove)
{
    bcm_error_t rv = BCM_E_NONE;
    int gport_type, gport_val;

    BCMDNX_INIT_FUNC_DEFS;

    *is_out_ac_to_remove = TRUE;

    /* 1. port is logical port. In that case no need to remove */
    rv = _bcm_dpp_gport_parse(unit,port_class,&gport_type, &gport_val, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    if (gport_type != _BCM_DPP_GPORT_TYPE_SIMPLE) {
        *is_out_ac_to_remove = FALSE;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Delete to Out-AC key
 */
STATIC
int
_bcm_petra_vlan_translate_action_out_ac_delete(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan,
    SOC_PPD_AC_ID out_ac,
    int is_found)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_EG_AC_INFO ac_info;
    int gport_type, gport_val;
    int is_cvid = FALSE;
    int vsi=0, cvid = 0, vlan_domain;
    SOC_PPD_EG_AC_VBP_KEY vbp_key;
    SOC_PPD_EG_AC_CEP_PORT_KEY cep_key;
    uint8 is_out_ac_to_remove;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);    

    SOC_PPD_EG_AC_INFO_clear(&ac_info);

    if (!is_found) {
        /* Nothing to remove */
        BCM_EXIT;
    }
    /* 
     * Retreive Out-AC index 
     * Two main options: 
     * 1. Port is a logical port (VLAN-port) in that case get OUT AC directly (==lif index) 
     * 2. Port is physical: get lookup by portxvlanx(vlan)
     */
    rv = _bcm_dpp_gport_parse(unit,port_class,&gport_type, &gport_val, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    if (gport_type == _BCM_DPP_GPORT_TYPE_SIMPLE) {
        /* Simple conversation where port is physical i.e. Vlan Domain */
        vlan_domain = port_class;
        /* Lookups can be either: vlan-domainxVSI or vlan-domainx(VSI)xCVID (only in case of CEP) */
        if (inner_vlan == BCM_VLAN_NONE || inner_vlan == BCM_VLAN_INVALID) {
            /* In this case vsi = outer_vlan. Case of vsi != outer_vlan user should set via Gport */
            vsi = outer_vlan;
            /* Verify the Outer-VLAN is not invalid */
            if (outer_vlan == BCM_VLAN_INVALID) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Outer vlan is not valid when key declared PortOuter")));
            }
        } else if (outer_vlan == BCM_VLAN_NONE || outer_vlan == BCM_VLAN_INVALID) {
            if (SOC_IS_ARAD(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("device support either PortOuter or PortDouble")));
            }
            cvid = inner_vlan;
            is_cvid = TRUE;
        } else {
            /* Double tag */
            if (SOC_IS_PETRAB(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("device support either PortOuter or PortInner")));
            }
            /* In this case vsi = outer_vlan. Case of vsi != outer_vlan user should set via Gport */
            vsi = outer_vlan;
            cvid = inner_vlan;
            is_cvid = TRUE;
        }

        /* Get port type: Can be a customer port or VBP */
        if (!is_cvid) {
            /* Build the Port * VSI Key */
            SOC_PPD_EG_AC_VBP_KEY_clear(&vbp_key);
            vbp_key.vsi = vsi;
            vbp_key.vlan_domain = vlan_domain;
        } else {
            /* CVID lookup: 1. Validate CEP port. 2. Lookup */
            
            /* Build the VD * (VSI) * CVID Key */
            SOC_PPD_EG_AC_CEP_PORT_KEY_clear(&cep_key);
            cep_key.vlan_domain = vlan_domain;
            cep_key.cvid = cvid;

            /* Attrbute of ARAD only */
            if (SOC_IS_ARAD(unit)) {
                cep_key.vsi = vsi;
            }
        } 
    } else {
        /* port is logical nothing to remove */
        BCM_EXIT;
    }

    if (is_found) {
        rv = _bcm_petra_vlan_translate_out_ac_to_remove(unit,port_class,outer_vlan,inner_vlan,
                                                        out_ac,&is_out_ac_to_remove);
        BCMDNX_IF_ERR_EXIT(rv);

        SOC_PPD_EG_AC_INFO_clear(&ac_info);
        ac_info.edit_info.edit_profile = 0;
        ac_info.edit_info.pcp_profile = 0;
        ac_info.edit_info.nof_tags = 2;    
        soc_sand_rv = soc_ppd_eg_ac_info_set(soc_sand_dev_id, out_ac, &ac_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (is_out_ac_to_remove) {        
            /* Remove Out-AC */
            rv = bcm_dpp_am_out_ac_dealloc(unit, _BCM_DPP_AM_OUT_AC_TYPE_DEFAULT, 0, out_ac);
            BCMDNX_IF_ERR_EXIT(rv);

            /* remove connection from lookup */
            if (!is_cvid) {
                soc_sand_rv = soc_ppd_eg_ac_port_vsi_info_remove(soc_sand_dev_id, &vbp_key, &out_ac);
                SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
            } else {
                soc_sand_rv = soc_ppd_eg_ac_port_cvid_info_remove(soc_sand_dev_id, &cep_key, &out_ac);
                SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
            }
        }
    }
  
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;   
}

/* 
 * Get the Out-AC Key
 */ 
STATIC
int 
_bcm_petra_vlan_translate_action_out_ac_get(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    int is_allocate_in_case_of_not_found,
    int *is_found,
    SOC_PPD_AC_ID *out_ac)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_AC_ID out_ac_lcl;
    uint8 found;
    SOC_PPD_EG_AC_INFO ac_info;
    bcm_port_t port;
    int gport_type, gport_val;
    int is_cvid = FALSE;
    int vsi = 0, cvid = 0, vlan_domain;
    SOC_PPD_EG_AC_VBP_KEY vbp_key;
    SOC_PPD_EG_AC_CEP_PORT_KEY cep_key;
    int fec_id, lif_id, is_local, core;
    SOC_SAND_SUCCESS_FAILURE soc_sand_success;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);    

    SOC_PPD_EG_AC_INFO_clear(&ac_info);
    
    /* 
     * Retreive Out-AC index 
     * Two main options: 
     * 1. Port is a logical port (VLAN-port) in that case get OUT AC directly (==lif index) 
     * 2. Port is physical: get lookup by portxvlanx(vlan)
     */
    rv = _bcm_dpp_gport_parse(unit,port_class,&gport_type, &gport_val, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_DPP_VLAN_CHK_ID(unit, outer_vlan);
    BCM_DPP_VLAN_CHK_ID(unit, inner_vlan);

    if (gport_type == _BCM_DPP_GPORT_TYPE_SIMPLE) {
      /* Simple convertion where port is physical i.e. Vlan Domain */
      BCM_DPP_PORT_CLASS_VALID(port_class);
      vlan_domain = port_class;
      /* Lookups can be either: vlan-domainxVSI or vlan-domainx(VSI)xCVID (only in case of CEP) */
      if (inner_vlan == BCM_VLAN_NONE) {
          /* In this case vsi = outer_vlan. Case of vsi != outer_vlan user should set via Gport */
          vsi = outer_vlan;
          /* Verify the Outer-VLAN is not invalid */
          if (outer_vlan == BCM_VLAN_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Outer vlan is not valid when key declared PortOuter")));
          }
      } else if (outer_vlan == BCM_VLAN_NONE) {
          if (SOC_IS_ARAD(unit)) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("device support either PortOuter or PortDouble")));
          }
          cvid = inner_vlan;
          is_cvid = TRUE;
      } else {
          /* Double tag */
          if (SOC_IS_PETRAB(unit)) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("device support either PortOuter or PortInner")));
          }
          /* In this case vsi = outer_vlan. Case of vsi != outer_vlan user should set via Gport */
          vsi = outer_vlan;
          cvid = inner_vlan;
          is_cvid = TRUE;
      }
   
      /* Get port type: Can be a customer port or VBP */
      if (!is_cvid) {
          
          /* Build the Port * VSI Key */
          SOC_PPD_EG_AC_VBP_KEY_clear(&vbp_key);
          vbp_key.vsi = vsi;
          vbp_key.vlan_domain = vlan_domain;

          /* See if this Key is already in */
          soc_sand_rv = soc_ppd_eg_ac_port_vsi_info_get(soc_sand_dev_id, &vbp_key, &out_ac_lcl, &ac_info, &found);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      } else {
          /* CVID lookup: 1. Validate CEP port. 2. Lookup */
          SOC_PPD_PORT_INFO port_info;
          uint32 class_id;
          SOC_PPD_PORT soc_ppd_port;
          bcm_port_t local_port;
          
          /* All ports that share this vlan domain must be of type CEP */  
          /* Go through all the ports with this VLAN-Domain */
          BCM_PBMP_ITER(PBMP_E_ALL(unit),local_port) {     
              rv = bcm_petra_port_class_get(unit, local_port, bcmPortClassId, &class_id);
              BCM_IF_ERROR_RETURN(rv);

              if (class_id == port_class) {
                  SOC_PPD_PORT_INFO_clear(&port_info);

                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

                  soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  if (port_info.port_type != SOC_SAND_PP_PORT_L2_TYPE_CEP) {
                      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port that is mapped to this vlan domain is not CEP type.")));
                  }
              }
          }

          /* Build the VD * (VSI) * CVID Key */
          SOC_PPD_EG_AC_CEP_PORT_KEY_clear(&cep_key);
          cep_key.vlan_domain = vlan_domain;
          cep_key.cvid = cvid;

          /* Attrbute of ARAD only */
          if (SOC_IS_ARAD(unit)) {
              cep_key.vsi = vsi;
          }
          soc_sand_rv = soc_ppd_eg_ac_port_cvid_info_get(soc_sand_dev_id, &cep_key, &out_ac_lcl, &ac_info, &found);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      } 
    } else {

        /* Port is Logical gport */
        if (outer_vlan != BCM_VLAN_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When gport is vlan_port outer vlan is invalid")));
        }

        port = port_class;

        /* VLAN Port GPORT - retreive LIF index directly */
        if (BCM_GPORT_IS_VLAN_PORT(port)) {
          
          rv = _bcm_dpp_gport_to_lif(unit, port, NULL, &lif_id, &fec_id, &is_local);
          BCMDNX_IF_ERR_EXIT(rv);

          if (!is_local) {
              /* API is relevant only for local configuration */
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unexpected lif index. should be local only")));
          }
        } else {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unexpected gport type. Must be physical or vlan_port")));
        }
        out_ac_lcl = lif_id;
        found = TRUE;
    }

    *is_found = found;

    if (found == FALSE && is_allocate_in_case_of_not_found) {
      /* Get a fresh Out-AC */
      rv = bcm_dpp_am_out_ac_alloc(unit, _BCM_DPP_AM_OUT_AC_TYPE_DEFAULT, 0, &out_ac_lcl);
      BCMDNX_IF_ERR_EXIT(rv);

      /* Set connection from lookup */
      ac_info.edit_info.nof_tags = DPP_VLAN_TRANSLATION_MAX_NOF_TAGS;
      if (!is_cvid) {
          soc_sand_rv = soc_ppd_eg_ac_port_vsi_info_add(soc_sand_dev_id, out_ac_lcl, &vbp_key, &ac_info, &soc_sand_success);
          BCM_IF_ERROR_RETURN(translate_sand_success_failure(soc_sand_success));
          SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
      } else {
          soc_sand_rv = soc_ppd_eg_ac_port_cvid_info_add(soc_sand_dev_id, out_ac_lcl, &cep_key, &ac_info, &soc_sand_success);
          BCM_IF_ERROR_RETURN(translate_sand_success_failure(soc_sand_success));
          SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
      }
    }
    *out_ac = out_ac_lcl;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_vlan_translation_tpids_to_tpid_profile(
    int unit,
    uint16 *tpids,
    int nof_tpids,
    int inner_tpid_only,
    uint32 *tpid_profile)
{
    bcm_error_t rv;
    int indx, is_exact_match, is_exact_match_in_opposite, is_similar_match;
    BCMDNX_INIT_FUNC_DEFS;

    /* Search for TPID profile, first search for exact match */
    for( indx = 0; indx < _BCM_PORT_NOF_TPID_PROFILES; ++indx) {
        rv = bcm_petra_port_tpid_profile_exact_match_search(unit, indx, tpids, nof_tpids, inner_tpid_only, &is_exact_match);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_exact_match) {
            *tpid_profile = indx;
            break;
        }
    }

    /* Search for TPID profile, exact match in opposite direction */
    if (!is_exact_match) {
        for(indx = 0; indx < _BCM_PORT_NOF_TPID_PROFILES; ++indx) {
            rv = bcm_petra_port_tpid_profile_opposite_match_search(unit, indx, tpids, nof_tpids, inner_tpid_only, &is_exact_match_in_opposite);
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_exact_match_in_opposite) {
                *tpid_profile = indx;
                break;
            }
        }
    }

    /* Given a pair of similar TPIDs, look for a TPID profile that includes this TPID */
    if (!is_exact_match && (!is_exact_match_in_opposite)) {
        for(indx = 0; indx < _BCM_PORT_NOF_TPID_PROFILES; ++indx) {
            rv = bcm_petra_port_tpid_profile_similar_match_search(unit, indx, tpids, nof_tpids, inner_tpid_only, &is_similar_match);
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_similar_match) {
                *tpid_profile = indx;
                break;
            }
        }
    }

    if (indx == _BCM_PORT_NOF_TPID_PROFILES) {
        /* Not FOUND */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("TPID profile not found")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_vlan_translation_tpid_actions_to_tpid_profile(int unit, bcm_vlan_action_set_t *action, uint32 *tpid_profile)
{
  bcm_error_t rv;
  uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
  int nof_tpids = 0;
  int inner_tpid_only;
  BCMDNX_INIT_FUNC_DEFS;


  if (action->outer_tpid_action == bcmVlanTpidActionModify) {
      tpids[nof_tpids] = action->outer_tpid;
      nof_tpids++;
  }

  if (action->inner_tpid_action == bcmVlanTpidActionModify) {
      tpids[nof_tpids] = action->inner_tpid;
      nof_tpids++;
  }

  inner_tpid_only = (nof_tpids == 1 && (action->inner_tpid_action == bcmVlanTpidActionModify)) ? 1:0;

  rv = _bcm_petra_vlan_translation_tpids_to_tpid_profile(unit, tpids, nof_tpids, inner_tpid_only, tpid_profile);
  BCMDNX_IF_ERR_EXIT(rv);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_vlan_translation_tpid_profile_and_values_to_tpid_actions(int unit, uint32 port_tpid_profile, bcm_vlan_action_set_t *action)
{
  bcm_error_t rv;
  uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
  int nof_tpids = 0;
  int inner_tpid_only;
  int indx, is_exact_match, is_first_exact_match;
  BCMDNX_INIT_FUNC_DEFS;

  if (action->outer_tpid) {
      tpids[nof_tpids] = action->outer_tpid;
      nof_tpids++;
  }

  if (action->inner_tpid) {
      tpids[nof_tpids] = action->inner_tpid;
      nof_tpids++;
  }

  inner_tpid_only = (nof_tpids == 1 && (action->inner_tpid != 0)) ? 1:0;

  /* Search for TPID profile */
  for(indx = 0; indx < _BCM_PORT_NOF_TPID_PROFILES; ++indx) {
      rv = bcm_petra_port_tpid_profile_exact_match_search(unit, indx, tpids, nof_tpids, inner_tpid_only, &is_exact_match);
      BCMDNX_IF_ERR_EXIT(rv);
      if (is_exact_match) {
          break;
      }
  }

  if (!is_exact_match) {
    for(indx = 0; indx < _BCM_PORT_NOF_TPID_PROFILES; ++indx) {
        rv = bcm_petra_port_tpid_profile_first_match_search(unit, indx, tpids, nof_tpids, inner_tpid_only, &is_first_exact_match);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_first_exact_match) {
            break;
        }
    }
  }

  if (indx == _BCM_PORT_NOF_TPID_PROFILES) {
      /* Not found */
      action->outer_tpid_action = (action->outer_tpid == 0) ? bcmVlanTpidActionNone:bcmVlanTpidActionModify;
      action->inner_tpid_action = (action->inner_tpid == 0) ? bcmVlanTpidActionNone:bcmVlanTpidActionModify;
  } else {
      /* Found. assume it is port configuration */
      action->outer_tpid_action = bcmVlanTpidActionNone; 
      action->inner_tpid_action = bcmVlanTpidActionNone;
  }
  

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_translate_action_add(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API no longer supported. Please use bcm_petra_vlan_translate_action_create instead.")));

  exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_vlan_translate_action_add(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_dpp_vlan_edit_profile_mapping_info_t *edit_profile_set = NULL;
  int old_profile, is_last, new_profile, is_allocated;
  uint32 tpid_profile = 0, soc_sand_rv;
  int profile_ndx;
  SOC_PPD_L2_LIF_AC_KEY in_ac_key;
  SOC_PPD_LIF_ID lif_index;
  SOC_PPD_L2_LIF_AC_INFO ac_info;
  uint8 found, is_inner_new_tag_to_use;
  SOC_SAND_SUCCESS_FAILURE success;
  bcm_dpp_vlan_egress_edit_profile_info_t *eg_mapping_info_unused = NULL;
  SOC_PPD_PORT soc_ppd_port;
  unsigned int allocSize = 0;
  SOC_PPD_LIF_ENTRY_INFO *lif_entry_info = NULL;
  int          is_local, lif_tmp, core;
  _bcm_dpp_gport_info_t gport_info;
  BCMDNX_INIT_FUNC_DEFS; 

  /* Verify action */
  rv = _bcm_petra_vlan_translate_action_verify(unit,TRUE,action);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Retreive AC key */
  SOC_PPD_L2_LIF_AC_KEY_clear(&in_ac_key);
  rv = _bcm_petra_vlan_translate_action_in_ac_key_build(unit, port, key_type, outer_vlan, inner_vlan, &in_ac_key);
  BCMDNX_IF_ERR_EXIT(rv);

  rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
  BCMDNX_IF_ERR_EXIT(rv);

  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port, &core)));

  /* Special Case: if both VLANs are invalid, go to the default port configuration */
  if ((in_ac_key.key_type == SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT)) {
    rv = bcm_petra_vlan_port_default_action_set(unit, soc_ppd_port, action);
    BCMDNX_IF_ERR_EXIT(rv);
  }

  /* Modify the In-AC Entry via get - add if found or not */
  SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);
  /* lif_index is used as an input parameter for soc_ppd_l2_lif_ac_get
   * when AC key type is SOC_PPD_L2_LIF_KEY_TYPENONE.
   */
  if (in_ac_key.key_type == SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_NONE) {
    rv = _bcm_dpp_gport_to_lif(unit,port, &lif_tmp, NULL, NULL, &is_local);
    BCMDNX_IF_ERR_EXIT(rv);
    lif_index = lif_tmp;
  }
  soc_sand_rv = soc_ppd_l2_lif_ac_get(unit, &in_ac_key, &lif_index, &ac_info, &found);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  if (found == TRUE) {
        /* Validity check - in AC P2P to PWE only outer vlan can be edited */
        if ((ac_info.service_type == SOC_PPD_L2_LIF_AC_SERVICE_TYPE_AC2PWE) ||
          (ac_info.service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB)) {
          if (action->new_inner_vlan != 0) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In AC P2P to PWE or Mac-in-Mac only outer vlan can be edited")));
          }
        }
  }
  
  /* 1. SW phase */
  /* Get the TPID Profile from action */
  /* Case 1: Both TPID actions are none: in that case take port tpid profile */
  if (action->outer_tpid_action == bcmVlanTpidActionNone && action->inner_tpid_action == bcmVlanTpidActionNone) {
      SOC_PPD_PORT_INFO port_info;

      soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      tpid_profile = port_info.tpid_profile;
  } else { /* Case 2: TPID actions can be retreived directly from action structure */
      rv = _bcm_petra_vlan_translation_tpid_actions_to_tpid_profile(unit, action, &tpid_profile);
      BCMDNX_IF_ERR_EXIT(rv);
  }

    allocSize = sizeof(bcm_dpp_vlan_edit_profile_mapping_info_t);

    BCMDNX_ALLOC(edit_profile_set,
              allocSize,
              "edit profile info");
    if (!edit_profile_set) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG("unable to allocate %d bytes edit profile info"),
                          allocSize));
    }


    allocSize = sizeof(bcm_dpp_vlan_egress_edit_profile_info_t);

    BCMDNX_ALLOC(eg_mapping_info_unused,
              allocSize,
              "eg mapping info unused");
    if (!eg_mapping_info_unused) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG("unable to allocate %d bytes egress mapping info"),
                          allocSize));
    }

  /* Convert the Action set to the Edit Profile Structure */
  rv = _bcm_petra_vlan_action_to_edit_profile_info_build(unit, action, tpid_profile, FALSE, edit_profile_set, eg_mapping_info_unused);
  BCMDNX_IF_ERR_EXIT(rv);

  /* 
   * 2 cases for allocation AC:
   * 1. AC key does not exist
   * 2. lif_vsi_index == lif_index: In that case we want to duplicate AC for specific vlan editing
   */
  if (found == FALSE) {
        
        /* Get the LIF-Index */
        rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_VLAN_EDITING, 0, &lif_index);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Fill information for AC */
        SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);
        ac_info.service_type                    = SOC_PPD_L2_LIF_AC_SERVICE_TYPE_MP;
        ac_info.learn_record.learn_type         = SOC_PPD_L2_LIF_AC_LEARN_SYS_PORT;
        ac_info.orientation                     = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        ac_info.vsid                            = in_ac_key.outer_vid;
        ac_info.default_frwrd.default_frwd_type = SOC_PPD_L2_LIF_DFLT_FRWRD_SRC_VSI;    

  }

  /* Add the Edit Profile Set in the SW profile index management */
  rv = _bcm_dpp_am_template_vlan_edit_profile_mapping_exchange(unit, lif_index, edit_profile_set, &old_profile, &is_last, &new_profile, &is_allocated);
  BCMDNX_IF_ERR_EXIT(rv);
  
  /* 2. Set in the HW */
  /* Set the IVECs in the IVEC table */
  if (is_allocated) {
      action->action_id = new_profile;
      rv = _bcm_petra_vlan_edit_profile_info_hw_set(unit, new_profile, edit_profile_set);
      BCMDNX_IF_ERR_EXIT(rv);
  }
  
  /* 
   * Select the modified VID with the rule: 
   * If an inner VID is Add / Replace, use the New-Inner-VID, otherwise the Outer one 
   * Assumption that both are equal if both must be used (HW restriction) 
   */
  is_inner_new_tag_to_use = FALSE;
  if ((action->dt_inner == bcmVlanActionAdd) || (action->dt_inner == bcmVlanActionReplace) 
    || (action->dt_inner_prio == bcmVlanActionAdd) || (action->dt_inner_prio == bcmVlanActionReplace) 
    || (action->ot_inner == bcmVlanActionAdd) || (action->ot_inner == bcmVlanActionReplace) 
    || (action->it_inner == bcmVlanActionAdd) || (action->it_inner == bcmVlanActionReplace) 
    || (action->it_inner_prio == bcmVlanActionAdd) || (action->it_inner_prio == bcmVlanActionReplace) 
    || (action->ut_inner == bcmVlanActionAdd) || (action->ut_inner == bcmVlanActionReplace)) {
    is_inner_new_tag_to_use = TRUE;
  }

  /* Add the In-AC Key */
  success = SOC_SAND_SUCCESS;

  ac_info.ing_edit_info.ing_vlan_edit_profile = new_profile;

  if ((action->dt_inner_pkt_prio == bcmVlanActionAdd) || 
        (action->dt_outer_pkt_prio == bcmVlanActionAdd) ||
        (action->ot_outer_pkt_prio == bcmVlanActionAdd) ||
        (action->ot_inner_pkt_prio == bcmVlanActionAdd) ||
        (action->it_outer_pkt_prio == bcmVlanActionAdd) ||
        (action->it_inner_pkt_prio == bcmVlanActionAdd) ||
        (action->ut_outer_pkt_prio == bcmVlanActionAdd) ||
        (action->ut_inner_pkt_prio == bcmVlanActionAdd) ||
        (action->dt_inner_pkt_prio == bcmVlanActionReplace) || 
        (action->dt_outer_pkt_prio == bcmVlanActionReplace) ||
        (action->ot_outer_pkt_prio == bcmVlanActionReplace) ||
        (action->ot_inner_pkt_prio == bcmVlanActionReplace) ||
        (action->it_outer_pkt_prio == bcmVlanActionReplace) ||
        (action->it_inner_pkt_prio == bcmVlanActionReplace) ||
        (action->ut_outer_pkt_prio == bcmVlanActionReplace) ||
        (action->ut_inner_pkt_prio == bcmVlanActionReplace))
  {
      /* Association to QOS */
      profile_ndx = BCM_QOS_MAP_PROFILE_GET(action->priority);
    
      if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid egress pcp vlan profile out of range")));
      }
      if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(action->priority) || (action->priority == 0)) {
          ac_info.ing_edit_info.edit_pcp_profile = profile_ndx;
      } else {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PCP action is mapped but priority index is invalid")));
      }      
  }

  if (SOC_IS_PETRAB(unit)) {
      ac_info.ing_edit_info.vid = (is_inner_new_tag_to_use == FALSE)? action->new_outer_vlan : action->new_inner_vlan;
  }
  if (SOC_IS_ARAD(unit)) {
      ac_info.ing_edit_info.vid  = action->new_outer_vlan;
      ac_info.ing_edit_info.vid2 = action->new_inner_vlan;
  }  

  soc_sand_rv = soc_ppd_l2_lif_ac_add(unit, &in_ac_key, lif_index, &ac_info, &success);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  if (success != SOC_SAND_SUCCESS) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Failed to bind ac_key to and set attributes")));
  }

  BCMDNX_IF_ERR_EXIT(rv);
exit:
  BCM_FREE(lif_entry_info);
  if (edit_profile_set != NULL) {
      BCM_FREE(edit_profile_set);
  }
  if (eg_mapping_info_unused != NULL) {
      BCM_FREE(eg_mapping_info_unused);
  }
  BCMDNX_FUNC_RETURN;
}



int 
bcm_petra_vlan_translate_action_create(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* Handle the API only if the SOC property is set to Normal VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Normal mode")));
    }

    rv = _bcm_petra_vlan_translate_action_add(unit, port, key_type, outer_vlan, inner_vlan, action);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
  exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_vlan_translate_action_new_vlan_fill(int unit,bcm_vlan_action_set_t *action, SOC_PPD_L2_LIF_AC_INFO *ac_info) {

    int is_inner = 0;
    int is_outer = 0;
    BCMDNX_INIT_FUNC_DEFS;

    IS_INNER_ACTION_SET(action,is_inner);
    IS_OUTER_ACTION_SET(action,is_outer);

    if (is_outer) {          
        action->new_outer_vlan = ac_info->ing_edit_info.vid;
    }                        
    if (is_inner) {          
        if (SOC_IS_PETRAB(unit)) {
            action->new_inner_vlan = ac_info->ing_edit_info.vid;
        } 
        if (SOC_IS_ARAD(unit)) {
            action->new_inner_vlan = ac_info->ing_edit_info.vid2;
        }
    }                                          
    
    if (ac_info->ing_edit_info.edit_pcp_profile == 0) {
        action->priority = ac_info->ing_edit_info.edit_pcp_profile;
    } else {
        BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(action->priority,ac_info->ing_edit_info.edit_pcp_profile);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_translate_action_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_dpp_vlan_edit_profile_mapping_info_t *edit_profile_set = NULL;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_L2_LIF_AC_KEY in_ac_key;
    SOC_PPD_LIF_ID lif_index;
    SOC_PPD_L2_LIF_AC_INFO ac_info;
    bcm_vlan_action_set_t action_eg;
    bcm_dpp_vlan_egress_edit_profile_info_t *eg_mapping_info_unused = NULL;
    uint8 found;
    unsigned int allocSize = 0;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);    

    /* Handle the API only if the SOC property is set to Normal VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(soc_sand_dev_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Normal mode")));
    }

    /* Init the Action */
    bcm_vlan_action_set_t_init(action);
    bcm_vlan_action_set_t_init(&action_eg);

    /* Special Case: if both VLANs are invalid, set the default port configuration with the Default action */
    if ((outer_vlan == BCM_VLAN_INVALID) && (inner_vlan == BCM_VLAN_INVALID)) {
      rv = bcm_petra_vlan_port_default_action_set(unit, port, action);
      BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Build the In-AC Key */
    SOC_PPD_L2_LIF_AC_KEY_clear(&in_ac_key);
    rv = _bcm_petra_vlan_translate_action_in_ac_key_build(unit, port, key_type, outer_vlan, inner_vlan, &in_ac_key);
    BCMDNX_IF_ERR_EXIT(rv);


    allocSize = sizeof(bcm_dpp_vlan_edit_profile_mapping_info_t);

    BCMDNX_ALLOC(edit_profile_set,
              allocSize,
              "edit profile info");
    if (!edit_profile_set) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG("unable to allocate %d bytes edit profile info"),
                          allocSize));
    }

    SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);
    soc_sand_rv = soc_ppd_l2_lif_ac_get(soc_sand_dev_id, &in_ac_key, &lif_index, &ac_info, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!found) {          
      /* Not found */
      BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Key not found")));
    }


    /* Get the Edit Profile index */
    _bcm_dpp_vlan_edit_profile_mapping_info_t_init (edit_profile_set);
    rv = _bcm_dpp_am_template_vlan_edit_profile_mapping_data_get(unit, lif_index, edit_profile_set);
    BCMDNX_IF_ERR_EXIT(rv);

    allocSize = sizeof(bcm_dpp_vlan_egress_edit_profile_info_t);

    BCMDNX_ALLOC(eg_mapping_info_unused,
              allocSize,
              "eg mapping info unused");
    if (!eg_mapping_info_unused) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG("unable to allocate %d bytes egress mapping info"),
                          allocSize));
    }


    /* Convert the Edit Profile Data to the Action */
    _bcm_dpp_vlan_egress_edit_profile_info_t_init (eg_mapping_info_unused);
    rv = _bcm_petra_vlan_action_from_edit_profile_info_parse(unit, edit_profile_set, eg_mapping_info_unused, action, &action_eg);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get the tpid actions */
    {
      int core;
      SOC_PPD_PORT_INFO port_info;
      SOC_PPD_PORT soc_ppd_port;
      uint32 port_tpid_profile;
      _bcm_dpp_gport_info_t gport_info_act;
      
      rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info_act); 
      BCMDNX_IF_ERR_EXIT(rv);

      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info_act.local_port, &soc_ppd_port, &core)));

      soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      port_tpid_profile = port_info.tpid_profile;

      rv = _bcm_petra_vlan_translation_tpid_profile_and_values_to_tpid_actions(unit, port_tpid_profile, action);
      BCMDNX_IF_ERR_EXIT(rv);
    }
    
    /* Get the Priority = PCP-Profile */
    rv = _bcm_petra_vlan_translate_action_new_vlan_fill(unit,action,&ac_info);
    BCMDNX_IF_ERR_EXIT(rv);            

exit:
    if (edit_profile_set != NULL) {
       BCM_FREE(edit_profile_set);
    }
    if (eg_mapping_info_unused != NULL) {
       BCM_FREE(eg_mapping_info_unused);
    }

    BCMDNX_FUNC_RETURN;
}

/* 
 * Indicate if the In-AC must be removed (because the VLAN Action add has created it) 
 * Or its Edit Profile must be zero instead                                      )
 * (the In--AC already exists previously to the VLAN Edit Profile definition) 
 */
STATIC
int 
_bcm_petra_vlan_translate_in_ac_to_remove(
    int unit, 
    bcm_port_t port,
    SOC_PPD_L2_LIF_AC_KEY *in_ac_key, 
    SOC_PPD_LIF_ID lif_index,
    uint8 *is_in_ac_to_remove)
{
    bcm_error_t rv = BCM_E_NONE;
    int gport_type, gport_val;

    BCMDNX_INIT_FUNC_DEFS;

    *is_in_ac_to_remove = TRUE;

    /* If port is logical port, no need to remove */
    rv = _bcm_dpp_gport_parse(unit,port,&gport_type, &gport_val, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    if (gport_type != _BCM_DPP_GPORT_TYPE_SIMPLE) {
        *is_in_ac_to_remove = FALSE;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_translate_action_delete(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan)
{
  bcm_error_t rv = BCM_E_NONE;
  bcm_dpp_vlan_edit_profile_mapping_info_t *edit_profile_set = NULL;
  int old_profile, is_last, new_profile, is_allocated;
  uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
  SOC_PPD_L2_LIF_AC_KEY in_ac_key;
  SOC_PPD_LIF_ID lif_index;
  SOC_PPD_L2_LIF_AC_INFO ac_info;
  uint8 found, is_in_ac_to_remove;
  SOC_SAND_SUCCESS_FAILURE success;
  bcm_vlan_action_set_t *action = NULL;
  unsigned int allocSize = 0;
  int          is_local, lif_tmp;

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  _BCM_DPP_SWITCH_API_START(unit);
  soc_sand_dev_id = (unit);    
  
  /* Handle the API only if the SOC property is set to Normal VLAN translation mode */
  if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(soc_sand_dev_id)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Normal mode")));
  }

  allocSize = sizeof(bcm_vlan_action_set_t);
  
  BCMDNX_ALLOC(action,
            allocSize,
            "vlan action set");
  if (!action) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                       (_BSL_BCM_MSG("unable to allocate %d bytes vlan action set"),
                        allocSize));
  }

  /* Init the Action */
  bcm_vlan_action_set_t_init(action);

  /* Special Case: if both VLANs are invalid, get the default port configuration */
  if ((outer_vlan == BCM_VLAN_INVALID) && (inner_vlan == BCM_VLAN_INVALID)) {
    rv = bcm_petra_vlan_port_default_action_get(unit, port, action);
    BCMDNX_IF_ERR_EXIT(rv);
  }

  /* Build the In-AC Key */
  SOC_PPD_L2_LIF_AC_KEY_clear(&in_ac_key);
  rv = _bcm_petra_vlan_translate_action_in_ac_key_build(unit, port, key_type, outer_vlan, inner_vlan, &in_ac_key);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Get the Edit Profile set */
  SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);

  /* lif_index is used as an input parameter for soc_ppd_l2_lif_ac_get
   * when AC key type is SOC_PPD_L2_LIF_KEY_TYPENONE.
   */
  if (in_ac_key.key_type == SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_NONE) {
    rv = _bcm_dpp_gport_to_lif(unit,port, &lif_tmp, NULL, NULL, &is_local);
    BCMDNX_IF_ERR_EXIT(rv);
    lif_index = lif_tmp;
  }
  soc_sand_rv = soc_ppd_l2_lif_ac_get(soc_sand_dev_id, &in_ac_key, &lif_index, &ac_info, &found);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  /* Modify to Profile 0 or Remove the In-AC Key only if found */
  if (found == TRUE) {
    rv = _bcm_petra_vlan_translate_in_ac_to_remove(unit, port, &in_ac_key, lif_index, &is_in_ac_to_remove);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_am_template_vlan_edit_profile_mapping_free(unit, lif_index, &old_profile, &is_last);    
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    allocSize = sizeof(bcm_dpp_vlan_edit_profile_mapping_info_t);

    BCMDNX_ALLOC(edit_profile_set,
              allocSize,
              "edit profile info");
    if (!edit_profile_set) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG("unable to allocate %d bytes edit profile info"),
                          allocSize));
    }

    /* Remove this In-AC from the Edit-Profile in SW */
    _bcm_dpp_vlan_edit_profile_mapping_info_t_init(edit_profile_set);

    if (is_in_ac_to_remove == FALSE) {      
      ac_info.ing_edit_info.ing_vlan_edit_profile = 0;
      ac_info.ing_edit_info.edit_pcp_profile = 0;
      soc_sand_rv = soc_ppd_l2_lif_ac_add(soc_sand_dev_id, &in_ac_key, lif_index, &ac_info, &success);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      if (success != SOC_SAND_SUCCESS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("soc_ppd_l2_lif_ac_add did not succeed")));
      }

      
    }
    else {
      soc_sand_rv = soc_ppd_l2_lif_ac_remove(soc_sand_dev_id, &in_ac_key, &lif_index);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      /* Remove this In-AC from the SW DB */
      rv = bcm_dpp_am_l2_ac_dealloc(unit, 0, lif_index);
      BCMDNX_IF_ERR_EXIT(rv);

    }
        
    rv = _bcm_dpp_am_template_vlan_edit_profile_mapping_alloc(unit, 0, lif_index, edit_profile_set, &new_profile, &is_allocated);    
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Should be reserved profile, in case of new profile return internal error */
    if (new_profile) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Entry should be already allocated %d but profile %d is new"), is_allocated, new_profile));
    }
  } else { 
      BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Entry not found")));
  }

  BCMDNX_IF_ERR_EXIT(rv);
  
exit:
    if (edit_profile_set != NULL) {
       BCM_FREE(edit_profile_set);
    }
    if (action != NULL) {
       BCM_FREE(action);
    }
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/* 
 * Verify Sturcture action for all vlan translation APIs
 */
STATIC int
_bcm_petra_vlan_translate_action_verify(int unit, int is_ingress, bcm_vlan_action_set_t *action)
{
        int is_inner, is_outer;
        
    BCMDNX_INIT_FUNC_DEFS;
    is_inner = 0;
    is_outer = 0;

    if (NULL == action) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action pointer is NULL")));
    }

    if (action->flags & BCM_VLAN_ACTION_SET_WITH_ID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No support of explicit action id (flag with_id is set) ")));
    }

    BCM_DPP_VLAN_CHK_ID(unit, action->new_outer_vlan);

    if (action->new_inner_vlan != BCM_VLAN_INVALID) {
        BCM_DPP_VLAN_CHK_ID(unit, action->new_inner_vlan); 
    }

    if (SOC_IS_PETRAB(unit)) {
      if (is_ingress &&
          action->new_inner_vlan != BCM_VLAN_NONE && action->new_outer_vlan != BCM_VLAN_NONE &&
          action->new_inner_vlan != action->new_outer_vlan) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action new_outer_vlan and new_inner_vlan have to be equal or one of them has to be BCM_VLAN_NONE")));
      }
    }


    switch (action->dt_outer) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
    case bcmVlanActionCopy:
        break;
    case bcmVlanActionMappedReplace:
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_outer for Ingress must be bcmVlanAction... None / Add / Replace / Copy / Delete")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_outer must be bcmVlanAction... None / Add / Replace / Copy / Delete / MappedReplace / MappedAdd")));
    }

    switch (action->dt_outer_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
    case bcmVlanActionCopy:
        break;
    case bcmVlanActionMappedReplace:
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_outer_prio for Ingress must be bcmVlanAction... None / Add / Replace / Copy / Delete")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_outer_prio must be bcmVlanAction... None / Add / Replace / Copy / Delete / MappedReplace / MappedAdd")));
    }

    switch (action->dt_outer_pkt_prio) {
    case bcmVlanActionNone:        
    case bcmVlanActionAdd:      
    case bcmVlanActionReplace:      
    case bcmVlanActionCopy:  
    case bcmVlanActionDelete:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_outer_pkt_prio must be bcmVlanAction... None / Add / Replace / Copy / Delete")));
    }

    switch (action->dt_outer_cfi) {
    case bcmVlanActionNone:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_outer_cfi must be bcmVlanActionNone")));
    }

    switch (action->dt_inner) {
    case bcmVlanActionNone:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
    case bcmVlanActionCopy:
    case bcmVlanActionAdd:
        break;
    case bcmVlanActionMappedReplace:
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_inner for Ingress must be bcmVlanAction... None / Add / Replace / Copy / Delete")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_inner must be bcmVlanAction... None / Add / Replace / Copy / Delete / MappedReplace / MappedAdd")));
    }

    switch (action->dt_inner_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
    case bcmVlanActionCopy:
    case bcmVlanActionAdd:
        break;
    case bcmVlanActionMappedReplace:
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_inner_prio for Ingress must be bcmVlanAction... None / Add / Replace / Copy / Delete")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_inner_prio must be bcmVlanAction... None / Add / Replace / Copy / Delete / MappedReplace / MappedAdd")));
    }

    switch (action->dt_inner_pkt_prio) {
    case bcmVlanActionNone:        
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionCopy: 
    case bcmVlanActionDelete:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_inner_pkt_prio must be bcmVlanAction... None / Add / Replace / Copy / Delete")));
    }

    switch (action->dt_inner_cfi) {
    case bcmVlanActionNone:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_inner_cfi must be bcmVlanActionNone")));
    }

    switch (action->ot_outer) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
    case bcmVlanActionMappedReplace:
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_outer for Ingress must be bcmVlanAction... None / Add / Replace / Delete")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_outer must be bcmVlanAction... None / Add / Replace / Delete / MappedReplace / MappedAdd")));
    }

    switch (action->ot_outer_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
    case bcmVlanActionMappedReplace:
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_outer_prio for Ingress must be bcmVlanAction... None / Add / Replace / Delete")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_outer_prio must be bcmVlanAction... None / Add / Replace / Delete / MappedReplace / MappedAdd")));
    }

    switch (action->ot_outer_pkt_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_outer_pkt_prio must be bcmVlanAction... None / Add / Replace / Delete")));
    }

    switch (action->ot_outer_cfi) {
    case bcmVlanActionNone:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_outer_cfi must be bcmVlanActionNone")));
    }

    switch (action->ot_inner) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionCopy:    
        break;
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_inner for Ingress must be bcmVlanAction... None / Add / Copy")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_inner must be bcmVlanAction... None / Add / Copy / MappedAdd")));
    }

    switch (action->ot_inner_pkt_prio) {
    case bcmVlanActionNone:           
    case bcmVlanActionAdd:
    case bcmVlanActionCopy:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_inner_pkt_prio must be bcmVlanAction... None / Add / Copy")));
    }

    switch (action->ot_inner_cfi) {
    case bcmVlanActionNone:
        break;   
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ot_inner_cfi must be bcmVlanActionNone")));
    }

    switch (action->it_outer) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionCopy:
        break;
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->it_outer for Ingress must be bcmVlanAction... None / Add / Copy")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->it_outer must be bcmVlanAction... None / Add / Copy / MappedAdd")));
    }

    switch (action->it_outer_pkt_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionCopy:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->it_outer_pkt_prio must be bcmVlanAction... None / Add / Copy")));
    }

    switch (action->it_outer_cfi) {
    case bcmVlanActionNone:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->it_outer_cfi must be bcmVlanActionNone")));
    }

    switch (action->it_inner) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
    case bcmVlanActionMappedReplace:
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->it_inner for Ingress must be bcmVlanAction... None / Add / Replace / Delete")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->it_inner must be bcmVlanAction... None / Add / Replace / Delete / MappedReplace / MappedAdd")));
    }

    switch (action->it_inner_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
    case bcmVlanActionMappedReplace:
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->it_inner_prio for Ingress must be bcmVlanAction... None / Add / Replace / Delete")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->it_inner_prio must be bcmVlanAction... None / Add / Replace / Delete / MappedReplace / MappedAdd")));
    }

    switch (action->it_inner_pkt_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;  
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->it_inner_pkt_prio must be bcmVlanAction... None / Add / Replace / Delete")));
    }

    switch (action->it_inner_cfi) {
    case bcmVlanActionNone:
        break;    
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->it_inner_cfi must be bcmVlanActionNone")));
    }

    switch (action->ut_outer) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
        break;
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ut_outer for Ingress must be bcmVlanAction... None / Add")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ut_outer must be bcmVlanAction... None / Add / MappedAdd")));
    }

    switch (action->ut_outer_pkt_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ut_outer_pkt_prio must be bcmVlanAction... None / Add")));
    }

    switch (action->ut_outer_cfi) {
    case bcmVlanActionNone:
        break;    
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ut_outer_cfi must be bcmVlanActionNone")));
    }

    switch (action->ut_inner) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
        break;
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ut_inner for Ingress must be bcmVlanAction... None / Add")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ut_inner must be bcmVlanAction... None / Add / MappedAdd")));
    }

    switch (action->ut_inner_pkt_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ut_inner_pkt_prio must be bcmVlanAction... None / Add")));
    }

    switch (action->ut_inner_cfi) {
    case bcmVlanActionNone:
        break;    
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ut_inner_cfi must be must be bcmVlanActionNone")));
    }    

    /* TPID actions */
    switch (action->outer_tpid_action) {
    case bcmVlanTpidActionNone:
    case bcmVlanTpidActionModify:
        break;    
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->outer_tpid_action must be bcmVlanTpidActionNone or bcmVlanTpidActionModify")));
    }    

    switch (action->inner_tpid_action) {
    case bcmVlanTpidActionNone:
    case bcmVlanTpidActionModify:
        break;    
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->inner_tpid_action must be bcmVlanTpidActionNone or bcmVlanTpidActionModify")));
    }

    IS_INNER_ACTION_SET(action,is_inner);
    IS_OUTER_ACTION_SET(action,is_outer);
    if (!is_inner && action->new_inner_vlan != 0) {          
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->new_inner_vlan must be 0 if not inner action.")));
    }                                          
    if (!is_outer && action->new_outer_vlan != 0) {          
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->new_outer_vlan must be 0 if not outer action.")));  
    }                        

    if (SOC_IS_PETRAB(unit) && is_ingress) {   
         if ((is_outer && is_inner) && (action->new_outer_vlan != action->new_inner_vlan)) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("new_outer_vlan must be equal to new_inner_vlan")));
         }
    }

    /* Not supported fields */
    switch (action->outer_pcp) {
    case bcmVlanActionNone:
        break;    
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("outer_pcp actions are not supported")));
    }
    
    switch (action->inner_pcp) {
    case bcmVlanActionNone:
        break;    
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("inner_pcp actions are not supported")));
    }    
    
    /* PCP value verify */
    if (action->priority != 0) {
        /* If not default priotity. Must be QOS ID that is of type VLAN PCP */
        if (is_ingress) {
            if (!BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(action->priority)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->priority is not of type QOS PCP_VLAN")));
            }
        } else { /* egress */
            if (!BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(action->priority)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->priority is not of type QOS PCP_VLAN")));
            }
        }
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_pon_vlan_translation_egress_action_resolve
 * Purpose:
 *      restore tunnel tag action for PON 2 tags manipulation
 * Parameters:
 *      unit        - (IN)  Device Number
 *      port        - (IN)  Device PP port Number
 *      out_ac      - (IN)  Out AC, VLAN actions to be installed
 *      action      - (IN/OUT) VLAN translation actions
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_petra_pon_vlan_translation_egress_action_resolve(
    int unit,
    bcm_port_t port,
    SOC_PPD_AC_ID out_ac, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_EG_AC_INFO ac_info;
    bcm_tunnel_id_t egress_tunnel_value;
    uint16 pon_tunnel_tpid, outer_tpid;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(action);

    /* in case of 3 TAGs manipulation, do nothing */
    if (((action->dt_outer == bcmVlanActionReplace) && (action->dt_inner == bcmVlanActionReplace)) ||
        (action->ot_inner != bcmVlanActionNone) ||
        (action->it_inner != bcmVlanActionNone) ||
        (action->ut_inner != bcmVlanActionNone))
    {
        BCM_EXIT;
    }

    soc_sand_dev_id = (unit);    

    /* Get the Out-AC Entry */
    SOC_PPD_EG_AC_INFO_clear(&ac_info);
    soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, out_ac, &ac_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* egress tunnel value is stored as outer vid */
    egress_tunnel_value = ac_info.edit_info.vlan_tags[0].vid;

    pon_tunnel_tpid = soc_property_get(unit, spn_PON_TPID_TUNNEL_ID, 0x8100);

    rv = bcm_petra_port_tpid_get(unit, port, &outer_tpid);
    BCMDNX_IF_ERR_EXIT(rv);

    if ((action->dt_outer == bcmVlanActionDelete) && 
        (action->dt_inner == bcmVlanActionDelete)) {
        /* NNI S+C -> PON untag */
        action->dt_outer = bcmVlanActionReplace;
        action->new_outer_vlan = egress_tunnel_value;
        action->dt_inner = bcmVlanActionDelete;
    } else if ((action->dt_outer == bcmVlanActionDelete) && 
               (action->dt_inner == bcmVlanActionNone)) {
        /* NNI S+C -> PON C */
        action->dt_outer = bcmVlanActionReplace;
        action->new_outer_vlan = egress_tunnel_value;
    } else if ((action->dt_outer == bcmVlanActionDelete) && 
               (action->dt_inner == bcmVlanActionReplace)) {
        /* NNI S+C -> PON C' */
        action->dt_outer = bcmVlanActionReplace;
        action->new_outer_vlan = egress_tunnel_value;
    } else if ((action->dt_outer == bcmVlanActionReplace) && 
               (action->dt_inner == bcmVlanActionNone)) {
        /* NNI S+C -> PON S'+C */
        action->dt_inner = bcmVlanActionAdd;
        action->new_inner_vlan = action->new_outer_vlan;
        action->dt_outer = bcmVlanActionReplace;
        action->new_outer_vlan = egress_tunnel_value;
        action->inner_tpid_action = bcmVlanTpidActionModify;
        action->inner_tpid = outer_tpid;
    } else if ((action->it_outer == bcmVlanActionDelete) && 
               (action->it_inner == bcmVlanActionNone)) {
        /* NNI C -> PON untag */
        action->it_outer = bcmVlanActionReplace;
        action->new_outer_vlan = egress_tunnel_value;
    } else if ((action->it_outer == bcmVlanActionReplace) && 
               (action->it_inner == bcmVlanActionNone)) {
        /* NNI C -> PON C' */
        action->it_inner = bcmVlanActionAdd;
        action->new_inner_vlan = action->new_outer_vlan;
        action->it_inner_pkt_prio = bcmVlanActionCopy; 
        action->it_outer = bcmVlanActionReplace;
        action->new_outer_vlan = egress_tunnel_value;
    } else if ((action->ot_outer == bcmVlanActionDelete) && 
               (action->ot_inner == bcmVlanActionNone)) {
        /* NNI S -> PON untag */
        action->ot_outer = bcmVlanActionReplace;
        action->new_outer_vlan = egress_tunnel_value;
    } else if ((action->ot_outer == bcmVlanActionReplace) && 
               (action->ot_inner == bcmVlanActionNone)) {
        /* NNI S -> PON S' */
        action->ot_inner = bcmVlanActionAdd;
        action->new_inner_vlan = action->new_outer_vlan;
        action->ot_outer = bcmVlanActionReplace;
        action->new_outer_vlan = egress_tunnel_value;
        action->inner_tpid_action = bcmVlanTpidActionModify;
        action->inner_tpid = outer_tpid;
        action->ot_inner_pkt_prio = bcmVlanActionCopy; 
    } else {
        BCM_EXIT;
    }

    /* Want to change the tpid of outer vid  */
    if (action->outer_tpid_action == bcmVlanTpidActionModify) {
        /* Overwrite outer vid tpid base input parameter */
        action->inner_tpid_action = bcmVlanTpidActionModify;
        action->inner_tpid = action->outer_tpid;
        /* Needn't changes tunnel ID tpid */
        action->outer_tpid_action = bcmVlanTpidActionNone;
        action->outer_tpid = 0;
    }

    if (pon_tunnel_tpid != outer_tpid) {            
        action->outer_tpid_action = bcmVlanTpidActionModify;
        action->outer_tpid = pon_tunnel_tpid;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_translate_egress_action_add(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    SOC_PPD_AC_ID out_ac = 0;
    int is_found;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    
    /* Handle the API only if the SOC property is set to Normal VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Normal mode")));
    }

    /* Verify action */
    rv = _bcm_petra_vlan_translate_action_verify(unit,FALSE,action);
    BCMDNX_IF_ERR_EXIT(rv);   

    /* No need to go to special case. For port configuration, call egress default action set */
    
    /* Compute the Out-AC */
    rv = _bcm_petra_vlan_translate_action_out_ac_get(unit, port_class, outer_vlan, inner_vlan, TRUE, &is_found, &out_ac);
    BCMDNX_IF_ERR_EXIT(rv);

    /* in case of PON application, re-orgnize actions for PON 2 tags manipulation */
    if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {        
        int is_pon_port;
        _bcm_dpp_gport_info_t gport_info_pon;

        /* map gport to physical port, and check if it's local */
        rv = _bcm_dpp_gport_to_phy_port(unit, port_class, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info_pon); 
        BCMDNX_IF_ERR_EXIT(rv);
        
        /* check if the port is PON port */
        rv = _bcm_dpp_port_is_pon_port(unit, gport_info_pon.local_port, &is_pon_port);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_pon_port) {
            rv = _bcm_petra_pon_vlan_translation_egress_action_resolve(unit, gport_info_pon.local_port, out_ac, action);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* Set action */
    rv = bcm_petra_vlan_translate_egress_action_add_per_out_ac(unit, out_ac, action);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_translate_egress_action_get(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    SOC_PPD_AC_ID out_ac = 0;
    int is_found;

    BCMDNX_INIT_FUNC_DEFS;
    
    /* Handle the API only if the SOC property is set to Normal VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Normal mode")));
    }

    bcm_vlan_action_set_t_init(action);

    /* Compute the Out-AC */
    rv = _bcm_petra_vlan_translate_action_out_ac_get(unit, port_class, outer_vlan, inner_vlan, FALSE, &is_found, &out_ac);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get the Out-AC info via the VBP Key */
    if (is_found != TRUE) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Out-AC doesn't exist.")));
    }

    rv = bcm_petra_vlan_translate_egress_action_get_per_out_ac(unit, out_ac, action);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_translate_egress_action_delete(
    int unit, 
    int port_class, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan)
{
    bcm_error_t rv = BCM_E_NONE;
    SOC_PPD_AC_ID out_ac = 0;
    bcm_vlan_action_set_t action_set;
    int is_found;

    BCMDNX_INIT_FUNC_DEFS;
    
    /* Handle the API only if the SOC property is set to Normal VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Normal mode")));
    }

    bcm_vlan_action_set_t_init(&action_set);

    rv = _bcm_petra_vlan_translate_action_out_ac_get(unit, port_class, outer_vlan, inner_vlan, FALSE, &is_found, &out_ac);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set it */
    rv = bcm_petra_vlan_translate_egress_action_delete_per_out_ac(unit, out_ac, is_found);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Deallocate AC */
    rv = _bcm_petra_vlan_translate_action_out_ac_delete(unit, port_class, outer_vlan, inner_vlan, out_ac, is_found);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function   :
 *      bcm_petra_vlan_translate_action_range_add
 * Description   :
 *      Add a range of VLANs and an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      outer_vlan_low  (IN) Packet outer VLAN ID low 
 *      outer_vlan_high (IN) Packet outer VLAN ID high
 *      inner_vlan_low  (IN) Packet inner VLAN ID low 
 *      inner_vlan_high (IN) Packet inner VLAN ID high
 *      action          (IN) Action for outer and inner tag
 */
int 
bcm_petra_vlan_translate_action_range_add(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t outer_vlan_low, 
    bcm_vlan_t outer_vlan_high, 
    bcm_vlan_t inner_vlan_low, 
    bcm_vlan_t inner_vlan_high, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_PPD_PORT ppd_port;
    int core;
    SOC_PPD_L2_VLAN_RANGE_KEY outer_vlan_range;
    SOC_PPD_L2_VLAN_RANGE_KEY inner_vlan_range;
    SOC_SAND_SUCCESS_FAILURE soc_sand_success;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_SWITCH_API_START(unit);
    /* check parameters */
    BCM_DPP_UNIT_CHECK(unit);

    if ((outer_vlan_low != BCM_VLAN_INVALID) &&
        (inner_vlan_low != BCM_VLAN_INVALID)) {
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_high);
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_high);
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_high);
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_high);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN range not supported.")));
    }

    /* Verify action */
    if ((action->ot_outer != bcmVlanActionCompressed) && 
        (action->it_inner != bcmVlanActionCompressed) && 
        (action->dt_outer != bcmVlanActionCompressed) &&
        (action->dt_inner != bcmVlanActionCompressed)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN range action not correct.")));
    } else if ((action->ot_outer == bcmVlanActionCompressed) &&
               (action->new_outer_vlan != outer_vlan_low)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("new outer VLAN not correct.")));
    } else if ((action->it_inner == bcmVlanActionCompressed) &&
               (action->new_inner_vlan != inner_vlan_low)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("new inner VLAN not correct.")));
    } else if ((action->dt_outer == bcmVlanActionCompressed) &&
               (action->dt_inner == bcmVlanActionCompressed)) {
        if ((action->new_outer_vlan != outer_vlan_low) || (action->new_inner_vlan != inner_vlan_low)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("new outer VLAN and new inner vlan not correct.")));
        }
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &ppd_port, &core)));
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        uint32 flags;
        soc_port_t phy_port;
        if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable && _BCM_PPD_IS_PON_PP_PORT(gport_info.local_port)) {
            phy_port = _BCM_PPD_GPORT_PON_TO_PHY_PORT(gport_info.local_port);
        } else {
            phy_port = gport_info.local_port;
        }
        BCM_SAND_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, phy_port, &flags));
        if(SOC_PORT_IS_INIT_VID_ONLY_INTERFACE(flags)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Can't use compressed-vid when tagged packets will be classify according to Initial-VID.")));
        }
    }
#endif
    /* Add outer VLAN range info to ppd_port */
    if (outer_vlan_low != BCM_VLAN_INVALID) {
        SOC_PPD_L2_VLAN_RANGE_KEY_clear(&outer_vlan_range);
        outer_vlan_range.first_vid = outer_vlan_low;
        outer_vlan_range.last_vid  = outer_vlan_high;
        outer_vlan_range.is_outer  = 1;
        outer_vlan_range.local_port_ndx = ppd_port;
        soc_sand_rv = soc_ppd_l2_lif_vlan_compression_add(unit, &outer_vlan_range, &soc_sand_success);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (soc_sand_success != SOC_SAND_SUCCESS) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("soc_ppd_l2_lif_vlan_compression_add failed")));
        }
    }

    /* Add inner VLAN range info to ppd_port */
    if (inner_vlan_low != BCM_VLAN_INVALID) {
        SOC_PPD_L2_VLAN_RANGE_KEY_clear(&inner_vlan_range);
        inner_vlan_range.first_vid = inner_vlan_low;
        inner_vlan_range.last_vid  = inner_vlan_high;
        inner_vlan_range.is_outer  = 0;
        inner_vlan_range.local_port_ndx = ppd_port;
        soc_sand_rv = soc_ppd_l2_lif_vlan_compression_add(unit, &inner_vlan_range, &soc_sand_success);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (soc_sand_success != SOC_SAND_SUCCESS) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("soc_ppd_l2_lif_vlan_compression_add failed")));
        }
    }

    /* Update AC MAP KEY for petra */
    if (SOC_IS_PETRAB(unit)) {        
        SOC_PPD_PORT_INFO port_info;
        int accept_mode;
        _bcm_petra_tpid_profile_t profile_type;
        _bcm_petra_dtag_mode_t dtag_mode;
        _bcm_petra_ac_key_map_type_t ac_key_map_type, new_ac_key_map_type;
        int last_appear,first_appear;
        int old_tpid_profile, new_tpid_profile;
        
        rv = _bcm_dpp_am_template_tpid_profile_data_get(unit, ppd_port, &profile_type, &accept_mode, &dtag_mode, &ac_key_map_type);
        BCMDNX_IF_ERR_EXIT(rv);

        if (ac_key_map_type == _bcm_petra_ac_key_map_type_normal) {
            soc_sand_rv = soc_ppd_port_info_get(unit, ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);            

            new_ac_key_map_type = _bcm_petra_ac_key_map_type_compressional;
            
            /* Remove old, and add new */
            rv = _bcm_dpp_am_template_tpid_profile_exchange(unit, ppd_port, profile_type, accept_mode, dtag_mode, new_ac_key_map_type, 
                            &old_tpid_profile, &last_appear, &new_tpid_profile, &first_appear);
            BCMDNX_IF_ERR_EXIT(rv);
            
            port_info.port_profile = new_tpid_profile;
            soc_sand_rv = soc_ppd_port_info_set(unit, ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = bcm_petra_tpid_profile_info_set(unit, port_info.port_profile, profile_type, accept_mode, dtag_mode, new_ac_key_map_type, port_info.port_type == SOC_SAND_PP_PORT_L2_TYPE_CEP, FALSE /* Petra-B no EVB */);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (rv != BCM_E_NONE) {
        if (outer_vlan_low != BCM_VLAN_INVALID) {
            soc_ppd_l2_lif_vlan_compression_remove(unit, &outer_vlan_range);
        }
        
        if (inner_vlan_low != BCM_VLAN_INVALID) {
            soc_ppd_l2_lif_vlan_compression_remove(unit, &inner_vlan_range);
        }    
    }
    _BCM_DPP_SWITCH_API_END(unit);    
    BCMDNX_FUNC_RETURN;
}
/*
 * Function   :
 *      bcm_petra_vlan_translate_action_range_get
 * Description   :
 *      Get a range of VLANs and an entry to ingress VLAN translation table.
 *      for Triumph
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      outer_vlan_low  (IN) Packet outer VLAN ID low 
 *      outer_vlan_high (IN) Packet outer VLAN ID high
 *      inner_vlan_low  (IN) Packet inner VLAN ID low 
 *      inner_vlan_high (IN) Packet inner VLAN ID high
 *      action          (OUT) Action for outer and inner tag
 */
int 
bcm_petra_vlan_translate_action_range_get(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t outer_vlan_low, 
    bcm_vlan_t outer_vlan_high, 
    bcm_vlan_t inner_vlan_low, 
    bcm_vlan_t inner_vlan_high, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv = BCM_E_NONE;    
    uint32 soc_sand_rv;
    int key_type = 0;
    SOC_PPD_PORT ppd_port;
    SOC_PPD_L2_PORT_VLAN_RANGE_INFO port_vlan_range_info;    
    uint32 range_ndx, range_iter;
    int is_found=FALSE, core;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    /* check parameters */
    BCM_DPP_UNIT_CHECK(unit);

    if ((outer_vlan_low != BCM_VLAN_INVALID) &&
        (inner_vlan_low != BCM_VLAN_INVALID)) {
        key_type = bcmVlanTranslateKeyPortDouble;
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_high);
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_high);
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortOuter;
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_high);
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortInner;
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_high);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN range not supported.")));
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &ppd_port, &core)));

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        uint32 flags;
        soc_port_t phy_port;
        if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable && _BCM_PPD_IS_PON_PP_PORT(gport_info.local_port)) {
            phy_port = _BCM_PPD_GPORT_PON_TO_PHY_PORT(gport_info.local_port);
        } else {
            phy_port = gport_info.local_port;
        }
        BCM_SAND_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, phy_port, &flags));
        if(SOC_PORT_IS_INIT_VID_ONLY_INTERFACE(flags)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Can't use compressed-vid when tagged packets will be classify according to Initial-VID.")));
        }
    }
#endif

    SOC_PPD_L2_PORT_VLAN_RANGE_INFO_clear(&port_vlan_range_info);
    /* Get all vlan ranges from ppd_port */
    soc_sand_rv = soc_ppd_l2_lif_vlan_compression_get(unit, ppd_port, &port_vlan_range_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    for (range_ndx = 0; range_ndx < SOC_PPD_LIF_VLAN_RANGE_MAX_SIZE; range_ndx++) {
        if (key_type == bcmVlanTranslateKeyPortDouble) {
            /* First find outer vlan range */
            if ((port_vlan_range_info.outer_vlan_range[range_ndx].first_vid == outer_vlan_low) && 
                (port_vlan_range_info.outer_vlan_range[range_ndx].last_vid == outer_vlan_high))
            {
                 /* Second find inner vlan range */
                for (range_iter = 0; range_iter < SOC_PPD_LIF_VLAN_RANGE_MAX_SIZE; range_iter++)
                {
                    if ((port_vlan_range_info.inner_vlan_range[range_iter].first_vid == inner_vlan_low) && 
                        (port_vlan_range_info.inner_vlan_range[range_iter].last_vid == inner_vlan_high))
                    {
                        action->dt_outer = bcmVlanActionCompressed;
                        action->new_outer_vlan = outer_vlan_low;
                        action->dt_inner = bcmVlanActionCompressed;
                        action->new_inner_vlan = inner_vlan_low;

                        is_found = TRUE;
                        goto exit;
                    }
                }
            }
        } else if ((key_type == bcmVlanTranslateKeyPortOuter) && 
                   (port_vlan_range_info.outer_vlan_range[range_ndx].first_vid == outer_vlan_low) && 
                   (port_vlan_range_info.outer_vlan_range[range_ndx].last_vid == outer_vlan_high)) {
            action->ot_outer = bcmVlanActionCompressed;
            action->new_outer_vlan = outer_vlan_low;
            is_found = TRUE;
            goto exit;
        } else if ((key_type == bcmVlanTranslateKeyPortInner) && 
                   (port_vlan_range_info.inner_vlan_range[range_ndx].first_vid == inner_vlan_low) && 
                   (port_vlan_range_info.inner_vlan_range[range_ndx].last_vid == inner_vlan_high)) {
            action->it_outer = bcmVlanActionCompressed;
            action->new_outer_vlan = inner_vlan_low;
            is_found = TRUE;
            goto exit;
        }
    }

    if (is_found != TRUE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VLAN range not found.")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function   :
 *      bcm_petra_vlan_translate_action_range_delete
 * Description   :
 *      Delete a range of VLANs and an entry from ingress VLAN translation table
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      outer_vlan_low  (IN) Packet outer VLAN ID low 
 *      outer_vlan_high (IN) Packet outer VLAN ID high
 *      inner_vlan_low  (IN) Packet inner VLAN ID low 
 *      inner_vlan_high (IN) Packet inner VLAN ID high
 */
int 
bcm_petra_vlan_translate_action_range_delete(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_t outer_vlan_low, 
    bcm_vlan_t outer_vlan_high, 
    bcm_vlan_t inner_vlan_low, 
    bcm_vlan_t inner_vlan_high)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_PPD_PORT ppd_port;
    int core;
    SOC_PPD_L2_VLAN_RANGE_KEY outer_vlan_range;
    SOC_PPD_L2_VLAN_RANGE_KEY inner_vlan_range;
    int is_port_comp_vlan_existed = FALSE;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_SWITCH_API_START(unit);
    
    /* check parameters */
    BCM_DPP_UNIT_CHECK(unit);

    if ((outer_vlan_low != BCM_VLAN_INVALID) &&
        (inner_vlan_low != BCM_VLAN_INVALID)) {
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_high);
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_high);
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, outer_vlan_high);
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_low);
        BCM_DPP_VLAN_CHK_ID(unit, inner_vlan_high);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN range not supported.")));
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);
     
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &ppd_port, &core)));

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        uint32 flags;
        soc_port_t phy_port;
        if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable && _BCM_PPD_IS_PON_PP_PORT(gport_info.local_port)) {
            phy_port = _BCM_PPD_GPORT_PON_TO_PHY_PORT(gport_info.local_port);
        } else {
            phy_port = gport_info.local_port;
        }
        BCM_SAND_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, phy_port, &flags));
        if(SOC_PORT_IS_INIT_VID_ONLY_INTERFACE(flags)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Can't use compressed-vid when tagged packets will be classify according to Initial-VID.")));
        }
    }
#endif

    /* Remove outer VLAN range info from ppd_port */
    if (outer_vlan_low != BCM_VLAN_INVALID) {
        SOC_PPD_L2_VLAN_RANGE_KEY_clear(&outer_vlan_range);
        outer_vlan_range.first_vid = outer_vlan_low;
        outer_vlan_range.last_vid  = outer_vlan_high;
        outer_vlan_range.is_outer  = 1;
        outer_vlan_range.local_port_ndx = ppd_port;
        soc_sand_rv = soc_ppd_l2_lif_vlan_compression_remove(unit, &outer_vlan_range);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* Remove inner VLAN range info from ppd_port */
    if (inner_vlan_low != BCM_VLAN_INVALID) {
        SOC_PPD_L2_VLAN_RANGE_KEY_clear(&inner_vlan_range);
        inner_vlan_range.first_vid = inner_vlan_low;
        inner_vlan_range.last_vid  = inner_vlan_high;
        inner_vlan_range.is_outer  = 0;
        inner_vlan_range.local_port_ndx = ppd_port;
        soc_sand_rv = soc_ppd_l2_lif_vlan_compression_remove(unit, &inner_vlan_range);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* Get vlan range info of port */
    if (SOC_IS_PETRAB(unit)) {        
        uint32 range_ndx;
        SOC_PPD_L2_PORT_VLAN_RANGE_INFO port_vlan_range_info;
        
        SOC_PPD_L2_PORT_VLAN_RANGE_INFO_clear(&port_vlan_range_info);        
        /* Get all vlan ranges from ppd_port */
        soc_sand_rv = soc_ppd_l2_lif_vlan_compression_get(unit, ppd_port, &port_vlan_range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Delete all vlan range from ppd_port */
        for (range_ndx = 0; range_ndx < SOC_PPD_LIF_VLAN_RANGE_MAX_SIZE; range_ndx++) {
            if ((port_vlan_range_info.outer_vlan_range[range_ndx].first_vid != BCM_VLAN_INVALID) || 
                (port_vlan_range_info.inner_vlan_range[range_ndx].first_vid != BCM_VLAN_INVALID)) {
                is_port_comp_vlan_existed = TRUE;
                break;
            }
        }
    }

    /* Update AC MAP KEY for petra */
    if (SOC_IS_PETRAB(unit) && (is_port_comp_vlan_existed == FALSE)) {
        SOC_PPD_PORT_INFO port_info;
        int accept_mode;
        _bcm_petra_tpid_profile_t profile_type;
        _bcm_petra_dtag_mode_t dtag_mode;
        _bcm_petra_ac_key_map_type_t ac_key_map_type, new_ac_key_map_type;
        int last_appear,first_appear;
        int old_tpid_profile, new_tpid_profile;
        
        rv = _bcm_dpp_am_template_tpid_profile_data_get(unit, ppd_port, &profile_type, &accept_mode, &dtag_mode, &ac_key_map_type);
        BCMDNX_IF_ERR_EXIT(rv);

        if (ac_key_map_type == _bcm_petra_ac_key_map_type_compressional) {
            soc_sand_rv = soc_ppd_port_info_get(unit, ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);            

            new_ac_key_map_type = _bcm_petra_ac_key_map_type_normal;
            
            /* Remove old, and add new */
            rv = _bcm_dpp_am_template_tpid_profile_exchange(unit, ppd_port, profile_type, accept_mode, dtag_mode, new_ac_key_map_type, 
                            &old_tpid_profile, &last_appear, &new_tpid_profile, &first_appear);
            BCMDNX_IF_ERR_EXIT(rv);
            
            port_info.port_profile = new_tpid_profile;
            soc_sand_rv = soc_ppd_port_info_set(unit, ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            rv = bcm_petra_tpid_profile_info_set(unit, port_info.port_profile, profile_type, accept_mode, dtag_mode, new_ac_key_map_type, port_info.port_type == SOC_SAND_PP_PORT_L2_TYPE_CEP, FALSE /* PetraB no EVB */);
            BCMDNX_IF_ERR_EXIT(rv);
        }        
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function   :
 *      bcm_petra_vlan_translate_action_range_delete_all
 * Description   :
 *      Delete all ranges of VLANs and entries from ingress 
 *      VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 */
int 
bcm_petra_vlan_translate_action_range_delete_all(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;    
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    uint32 range_ndx;
    bcm_port_t port;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t inner_vlan_low;
    bcm_vlan_t inner_vlan_high;
    SOC_PPD_PORT ppd_port;
    SOC_PPD_L2_PORT_VLAN_RANGE_INFO port_vlan_range_info;    
    int core;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);

    /* check parameters */
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    BCM_PBMP_ITER(PBMP_E_ALL(unit), port) {    
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &ppd_port, &core)));
#ifdef BCM_ARAD_SUPPORT
        if (SOC_IS_ARAD(unit)) {
            uint32 flags;
            soc_port_t phy_port;
            if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable && _BCM_PPD_IS_PON_PP_PORT(port)) {
                phy_port = _BCM_PPD_GPORT_PON_TO_PHY_PORT(port);
            } else {
                phy_port = port;
            }
            BCM_SAND_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, phy_port, &flags));
            if(SOC_PORT_IS_INIT_VID_ONLY_INTERFACE(flags)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Can't use compressed-vid when tagged packets will be classify according to Initial-VID.")));
            }
        }
#endif
        SOC_PPD_L2_PORT_VLAN_RANGE_INFO_clear(&port_vlan_range_info);        
        /* Get all vlan ranges from ppd_port */
        soc_sand_rv = soc_ppd_l2_lif_vlan_compression_get(soc_sand_dev_id, ppd_port, &port_vlan_range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Delete all vlan range from ppd_port */
        for (range_ndx = 0; range_ndx < SOC_PPD_LIF_VLAN_RANGE_MAX_SIZE; range_ndx++) {
            outer_vlan_low = port_vlan_range_info.outer_vlan_range[range_ndx].first_vid;
            outer_vlan_high = port_vlan_range_info.outer_vlan_range[range_ndx].last_vid;
            inner_vlan_low = port_vlan_range_info.inner_vlan_range[range_ndx].first_vid;
            inner_vlan_high = port_vlan_range_info.inner_vlan_range[range_ndx].last_vid;
            
            if ((inner_vlan_low != BCM_VLAN_INVALID) || (outer_vlan_low != BCM_VLAN_INVALID)) {
                rv = bcm_petra_vlan_translate_action_range_delete(unit, port, 
                    outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high);          
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_translate_egress_action_traverse(
    int unit, 
    bcm_vlan_translate_egress_action_traverse_cb cb, 
    void *user_data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Feature not supported."))); 
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_translate_action_traverse(
    int unit, 
    bcm_vlan_translate_action_traverse_cb cb, 
    void *user_data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Feature not supported."))); 
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_translate_action_range_traverse(
    int unit, 
    bcm_vlan_translate_action_range_traverse_cb cb, 
    void *user_data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Feature not supported."))); 
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      bcm_petra_vlan_translate_action_id_create
 * Description   :
 *      BCM API function to allocate an action ID for VLAN translation, either
 *      for Ingress or Egress.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      flags           (IN) BCM_VLAN_ACTION_SET_XXX. Either Ingress or Egress
 *                           should be selected. WITH_ID flag for allocating a
 *                           user selected ID value.
 *      action_id       (IN/OUT) The received allocation action ID. In case of
 *                           WITH_ID flag, stores the desired action ID.
 */
int 
bcm_petra_vlan_translate_action_id_create(
    int unit, 
    uint32 flags, 
    int *action_id)
{
    int rv;
    uint32 is_ingress;
    BCMDNX_INIT_FUNC_DEFS;

    /* Handle the API only if the SOC property is set to Advanced VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Advanced mode")));
    }

    /* Basic parameter validation */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(action_id);

    /* Verify that one of the Ingress/Egress flags is set, and only one is set. */
    is_ingress = (flags & BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    if ((is_ingress && (flags & BCM_VLAN_ACTION_SET_EGRESS)) ||
        (!is_ingress && !(flags & BCM_VLAN_ACTION_SET_EGRESS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN translate ID should be for either Ingress or Egress")));
    }

    /* Perform SW allocations, action_id validation performed by the alloc manager */
    rv = bcm_dpp_am_vlan_edit_action_id_alloc(unit, flags, action_id);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      bcm_petra_vlan_translate_action_id_destroy
 * Description   :
 *      Free a VLAN translation action ID, either in Ingress or Egress.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      flags           (IN) BCM_VLAN_ACTION_SET_XXX. Either Ingress or Egress
 *                           should be selected.
 *      action_id       (IN) The action ID to free.
 */
int 
bcm_petra_vlan_translate_action_id_destroy(
    int unit,
    uint32 flags,
    int action_id)
{
    int rv;
    uint32 is_ingress;
    BCMDNX_INIT_FUNC_DEFS;

    /* Handle the API only if the SOC property is set to Advanced VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Advanced mode")));
    }

    /* Basic parameter validation */
    BCM_DPP_UNIT_CHECK(unit);

    /* Verify that one of the Ingress/Egress flags is set, and only one is set. */
    is_ingress = (flags & BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    if ((is_ingress && (flags & BCM_VLAN_ACTION_SET_EGRESS)) ||
        (!is_ingress && !(flags & BCM_VLAN_ACTION_SET_EGRESS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN translate ID should be for either Ingress or Egress")));
    }

    /* Perform SW deallocation, action_id validation performed by the alloc manager */
    rv = bcm_dpp_am_vlan_edit_action_id_dealloc(unit, flags, action_id);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      bcm_petra_vlan_translate_action_id_destroy_all
 * Description   :
 *      Free all VLAN translation action IDs, either in Ingress or Egress.
 *      Reserved default allocations aren't freed and require usage of
 *      bcm_vlan_translate_action_id_destroy.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      flags           (IN) BCM_VLAN_ACTION_SET_XXX. Either Ingress or Egress
 *                           should be selected.
 */
int 
bcm_petra_vlan_translate_action_id_destroy_all(
    int unit,
    uint32 flags)
{
    int rv;
    uint32 is_ingress;
    BCMDNX_INIT_FUNC_DEFS;

    /* Handle the API only if the SOC property is set to Advanced VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Advanced mode")));
    }

    /* Basic parameter validation */
    BCM_DPP_UNIT_CHECK(unit);

    /* Verify that one of the Ingress/Egress flags is set, and only one is set. */
    is_ingress = (flags & BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    if ((is_ingress && (flags & BCM_VLAN_ACTION_SET_EGRESS)) ||
        (!is_ingress && !(flags & BCM_VLAN_ACTION_SET_EGRESS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN translate ID should be for either Ingress or Egress")));
    }

    /* Perform SW deallocation for all the pool */
    rv = bcm_dpp_am_vlan_edit_action_id_dealloc_all(unit, flags);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_petra_vlan_translate_action_to_bcm_dpp
 * Description   : 
 *      Internal function to convert a BCM API translate action to a partial
 *      BCM DPP structure that holds only the fields that are used in the
 *      Advanced VLAN edit mode. The BCM DPP structure is used to store the
 *      action in the SW.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      action          (IN) BCM API action structure. The Advanced mode usage
 *                           is tag format independent, thus only dt (double
 *                           tagged) fields are used.
 *      bcm_dpp_action  (OUT) BCM DPP action structure, that is used to store
 *                           the action in the SW.
 */
int
_bcm_petra_vlan_translate_action_to_bcm_dpp(
    int unit,
    bcm_vlan_action_set_t *action,
    _bcm_dpp_vlan_translate_action_t *bcm_dpp_action)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(action);
    BCMDNX_NULL_CHECK(bcm_dpp_action);

    /* Initialize the BCM DPP action structure */
    _bcm_dpp_vlan_translate_action_t_init(bcm_dpp_action);

    bcm_dpp_action->outer.vid_action = action->dt_outer;
    bcm_dpp_action->outer.pcp_action = action->dt_outer_pkt_prio;
    bcm_dpp_action->outer.tpid_action = action->outer_tpid_action;
    bcm_dpp_action->outer.tpid_val = action->outer_tpid;

    bcm_dpp_action->inner.vid_action = action->dt_inner;
    bcm_dpp_action->inner.pcp_action = action->dt_inner_pkt_prio;
    bcm_dpp_action->inner.tpid_action = action->inner_tpid_action;
    bcm_dpp_action->inner.tpid_val = action->inner_tpid;

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_petra_vlan_translate_action_from_bcm_dpp
 * Description   : 
 *      Internal function to convert a BCM DPP action structure to the BCM API
 *      action structure.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      bcm_dpp_action  (IN) BCM DPP action structure, that is used to store
 *                           the action in the SW.
 *      action          (OUT) BCM API action structure. The Advanced mode usage
 *                           is tag format independent, thus only dt (double
 *                           tagged) fields are used.
 */
int
_bcm_petra_vlan_translate_action_from_bcm_dpp(
    int unit,
    _bcm_dpp_vlan_translate_action_t *bcm_dpp_action,
    bcm_vlan_action_set_t *action)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(bcm_dpp_action);
    BCMDNX_NULL_CHECK(action);

    /* Initialize the command info structure */
    bcm_vlan_action_set_t_init(action);

    action->dt_outer = bcm_dpp_action->outer.vid_action;
    action->dt_outer_pkt_prio = bcm_dpp_action->outer.pcp_action;
    action->outer_tpid_action = bcm_dpp_action->outer.tpid_action;
    action->outer_tpid = bcm_dpp_action->outer.tpid_val;


    action->dt_inner = bcm_dpp_action->inner.vid_action;
    action->dt_inner_pkt_prio = bcm_dpp_action->inner.pcp_action;
    action->inner_tpid_action = bcm_dpp_action->inner.tpid_action;
    action->inner_tpid = bcm_dpp_action->inner.tpid_val;

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_dpp_vlan_translate_vid_action_to_default_ppd
 * Description   : 
 *      Internal function to convert a BCM API VID outer/inner action combination
 *      to PPD hardware Ingress/Egress structures. The conversion is achieved
 *      through a dedicated Advanced VLAN edit conversion table. The eventual
 *      PPD action may differ after taking into account the PCP & TPID as well.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      outer_vid_action (IN) BCM API outer VID action
 *      inner_vid_action (IN) BCM API inner VID action
 *      ing_command_info (OUT) A pointer to a PPD Ingress VLAN edit entry or
 *                           NULL if no conversion to Ingress required.
 *      eg_command_info (OUT) A pointer to a PPD Egress VLAN edit entry or
 *                           NULL if no conversion to Egress required.
 */
int 
_bcm_dpp_vlan_translate_vid_action_to_default_ppd(
    int unit,
    bcm_vlan_action_t outer_vid_action,
    bcm_vlan_action_t inner_vid_action,
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO *ing_command_info,
    SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO *eg_command_info)
{
    int map_idx;
    BCMDNX_INIT_FUNC_DEFS;

    /* Lookup for the BCM DPP action combination in the actions mapping to PPD table */
    for (map_idx = 0; map_idx < _BCM_DPP_VLAN_TRANSLATION_STATIC_AVANCED_MODE_MAPPING_NOF_ENTRIES; map_idx++) {
        if ((_bcm_petra_vlan_translation_static_mapping_advanced_mode[map_idx].vlan_action_bcm.outer_tag == outer_vid_action) &&
            (_bcm_petra_vlan_translation_static_mapping_advanced_mode[map_idx].vlan_action_bcm.inner_tag == inner_vid_action)) {

            /* If an Ingress PPD VLAN edit structure was supplied */
            if (ing_command_info) {
                /* Check that the mapping entry is valid for Ingress */
                if (_bcm_petra_vlan_translation_static_mapping_advanced_mode[map_idx].is_valid_ing_conf) {
                    /* Get the PPD action values from the mapping entry */
                    ing_command_info->tags_to_remove = _bcm_petra_vlan_translation_static_mapping_advanced_mode[map_idx].vlan_action_ppd.nof_tags_to_rmv;
                    ing_command_info->outer_tag.vid_source = _bcm_petra_vlan_translation_static_mapping_advanced_mode[map_idx].vlan_action_ppd.vid_source_outer;
                    ing_command_info->inner_tag.vid_source = _bcm_petra_vlan_translation_static_mapping_advanced_mode[map_idx].vlan_action_ppd.vid_source_inner;
                } else {
                    BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
                }
            }

            /* If an Egress PPD VLAN edit structure was supplied */
            if (eg_command_info) {
                /* Check that the mapping entry is valid for Egress */
                if (_bcm_petra_vlan_translation_static_mapping_advanced_mode[map_idx].is_valid_eg_conf) {
                    /* Get the PPD action values from the mapping entry */
                    eg_command_info->tags_to_remove = _bcm_petra_vlan_translation_static_mapping_advanced_mode[map_idx].vlan_action_ppd.nof_tags_to_rmv;
                    eg_command_info->outer_tag.vid_source = _bcm_petra_vlan_translation_static_mapping_advanced_mode[map_idx].vlan_action_ppd.vid_source_eg_outer;
                    eg_command_info->inner_tag.vid_source = _bcm_petra_vlan_translation_static_mapping_advanced_mode[map_idx].vlan_action_ppd.vid_source_eg_inner;
                } else {
                    BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
                }
            }
            BCM_EXIT;
        }
    }


    BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_dpp_vlan_translate_modify_optional_tags
 * Description   : 
 *      Internal function that applies logic to determine whether the default
 *      VID action configuration should be set to the Ingress/Egress HW, or
 *      modifies the HW configuration, if PCP/TPID modifications require it.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   : 
 *      unit             (IN) BCM unit number
 *      bcm_dpp_action   (IN) BCM DPP action entry that includes VID, PCP &
 *                            TPID values for Outer and Inner VLAN tags.
 *      vid_src_val_empty     (IN) Value for for VID source - empty Ingress/Egress.
 *      vid_src_val_inner_tag (IN) Value for for VID source - Inner Tag Ingress/Egress.
 *      vid_src_val_outer_tag (IN) Value for for VID source - Outer Tag Ingress/Egress.
 *      tags_to_remove  (IN/OUT) Pointer to nof tags to remove during the VLAN edit action.
 *      inner_tag_vid_source (IN/OUT) Pointer to Inner VID Source value for the action.
 *                            The supplied value is the default for the action.
 *      outer_tag_vid_source (IN/OUT) Pointer to Outer VID Source value for the action.
 *                            The supplied value is the default for the action.
 */
int 
_bcm_dpp_vlan_translate_modify_optional_tags(
    int unit,
    _bcm_dpp_vlan_translate_action_t *bcm_dpp_action,
    int vid_src_val_empty,
    int vid_src_val_inner_tag,
    int vid_src_val_outer_tag,
    uint8 *tags_to_remove,
    int *inner_tag_vid_source,
    int *outer_tag_vid_source)
{
    int nof_tags_after_removal, nof_tags_not_constructed = 0, nof_optional_tags_to_modify;
    int modify_outer_tag = FALSE;
    BCMDNX_INIT_FUNC_DEFS;

    /* If there are remaining tags after the default tag removal than
       it may be possible to edit them, if PCP or TPID actions are set for them */
    nof_tags_after_removal = DPP_VLAN_TRANSLATION_MAX_NOF_TAGS_TO_REMOVE - (*tags_to_remove);

    /* If not all the possible tag construction are used in default, it may be
       possible to edit more tags, if PCP or TPID actions are set */
    if ((*inner_tag_vid_source) == vid_src_val_empty) {
        nof_tags_not_constructed++;
        if ((*outer_tag_vid_source) == vid_src_val_empty) {
            nof_tags_not_constructed++;
        }
    }

    /* The nof optional tags that can be modified is the minimum value of the
       tags that are remaining after the tag removal and the number of tags that
       can still be constructed */
    nof_optional_tags_to_modify = (nof_tags_after_removal > nof_tags_not_constructed) ?
        nof_tags_not_constructed : nof_tags_after_removal;

    /* If optional tag modification is available for this action */
    if (nof_optional_tags_to_modify)
    {
        /* Check user modification on the inner tag.
           In cases where the nof of free tags for optional construction is bigger
           than the nof optional tags to modify, only the outer tag action will be used.*/
        if (nof_tags_not_constructed == nof_optional_tags_to_modify) {
            if (bcm_dpp_action->inner.tpid_action == bcmVlanTpidActionModify) {
                /* Configure the PPD inner tag VID to either the original Outer Tag
                   or the original Inner Tag value. If the nof removed tags is the same as
                   the optional tags to modify - The original tags and the PCP/TPID
                   action tags refer to the same Outer/Inner tags */
                *inner_tag_vid_source = (nof_tags_after_removal == nof_optional_tags_to_modify) ?
                    vid_src_val_inner_tag : vid_src_val_outer_tag;
                (*tags_to_remove)++;

                /* If the inner tag was modified for an action with two optional tags,
                   the outer tag must go in any case */
                modify_outer_tag = TRUE;
            }
        }

        /* Check user modification on the outer tag only if two additional
           tags can be modified due to PCP/TPID modification. */
        if (nof_tags_not_constructed == DPP_VLAN_TRANSLATION_MAX_NOF_TAGS_TO_BUILD) {
            if ((bcm_dpp_action->outer.tpid_action == bcmVlanTpidActionModify) ||
                (modify_outer_tag == TRUE)) {
                /* The PPD outer tag VID in case of two optional tag edits is
                   always Outer Tag value. Otherwise, the original outer was removed */
                *outer_tag_vid_source = (nof_optional_tags_to_modify == DPP_VLAN_TRANSLATION_MAX_NOF_TAGS_TO_BUILD) ?
                    vid_src_val_outer_tag : vid_src_val_inner_tag;
                (*tags_to_remove)++;
            }
        }
    }

    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_dpp_vlan_translate_vid_action_to_ppd
 * Description   : 
 *      Internal function to convert a BCM API VID outer/inner action combination
 *      to PPD hardware Ingress/Egress structures. The conversion is achieved
 *      through a dedicated Advanced VLAN edit conversion table. Than a function
 *      is called to set additional optional tag if required by PCP/TPID actions.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      bcm_dpp_action  (IN) BCM DPP action entry that includes VID, PCP &
 *                            TPID values for Outer and Inner VLAN tags.
 *      ing_command_info(OUT) A pointer to a PPD Ingress VLAN edit entry or
 *                           NULL if no conversion to Ingress required.
 *      eg_command_info (OUT) A pointer to a PPD Egress VLAN edit entry or
 *                           NULL if no conversion to Egress required.
 */
int 
_bcm_dpp_vlan_translate_vid_action_to_ppd(
    int unit,
    _bcm_dpp_vlan_translate_action_t *bcm_dpp_action,
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO *ing_command_info,
    SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO *eg_command_info)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Convert the BCM DPP VID action data to a default PPD action that is based
       only on VID actions, without taking in account the PCP & TPID */
    rv = _bcm_dpp_vlan_translate_vid_action_to_default_ppd(unit,
                bcm_dpp_action->outer.vid_action, bcm_dpp_action->inner.vid_action,
                ing_command_info, eg_command_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Modify additional optional tags if the PCP/TPID is set for them */
    if (ing_command_info) {
        rv = _bcm_dpp_vlan_translate_modify_optional_tags(unit, bcm_dpp_action,
                    SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY,
                    SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,
                    SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,
                    &(ing_command_info->tags_to_remove),
                    (int *)&(ing_command_info->inner_tag.vid_source), (int *)&(ing_command_info->outer_tag.vid_source));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (eg_command_info) {
        rv = _bcm_dpp_vlan_translate_modify_optional_tags(unit, bcm_dpp_action,
                    SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY,
                    SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG,
                    SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG,
                    &(eg_command_info->tags_to_remove),
                    (int *)&(eg_command_info->inner_tag.vid_source), (int *)&(eg_command_info->outer_tag.vid_source));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_dpp_vlan_translate_pcp_action_to_ppd
 * Description   : 
 *      Internal function to convert a BCM API PCP outer/inner actions
 *      to PPD hardware Ingress/Egress structures.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit             (IN) BCM unit number
 *      outer_pcp_action (IN) BCM API outer PCP action
 *      inner_pcp_action (IN) BCM API inner PCP action
 *      ing_command_info (OUT) A pointer to a PPD Ingress VLAN edit entry or
 *                           NULL if no conversion to Ingress required.
 *      eg_command_info  (OUT) A pointer to a PPD Egress VLAN edit entry or
 *                           NULL if no conversion to Egress required.
 */
int 
_bcm_dpp_vlan_translate_pcp_action_to_ppd(
    int unit,
    bcm_vlan_action_t outer_pcp_action,
    bcm_vlan_action_t inner_pcp_action,
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO *ing_command_info,
    SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO *eg_command_info)
{
    SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC ing_outer_pcp_dei_source, ing_inner_pcp_dei_source;
    SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC eg_outer_pcp_dei_source, eg_inner_pcp_dei_source;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get the PCP-DEI source for the outer tag */
    switch (outer_pcp_action) {
    case bcmVlanActionNone:
    case bcmVlanActionOuterAdd:
        ing_outer_pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG;
        eg_outer_pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG;
        break;
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:    
        ing_outer_pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NEW;
        eg_outer_pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP;
        break;
    case bcmVlanActionCopy:
    case bcmVlanActionInnerAdd:
        ing_outer_pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG;
        eg_outer_pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG;
        break;
    default:
        ing_outer_pcp_dei_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;
        eg_outer_pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;
        break;
    }

    /* Get the PCP-DEI source for the inner tag */
    switch (inner_pcp_action) {
    case bcmVlanActionNone:
    case bcmVlanActionInnerAdd:
        ing_inner_pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG;
        eg_inner_pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG;
        break;
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:    
        ing_inner_pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NEW;
        eg_inner_pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP;
        break;
    case bcmVlanActionCopy:
    case bcmVlanActionOuterAdd:
        ing_inner_pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG;
        eg_inner_pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG;
        break;
    default:
        ing_inner_pcp_dei_source = SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;
        eg_inner_pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;
        break;
    }

    /* Set the PCP-DEI values in the appropriate PPD structure, in case a VID action is configured */
    if (ing_command_info) {
        ing_command_info->outer_tag.pcp_dei_source = (ing_command_info->outer_tag.vid_source != SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY) ? 
            ing_outer_pcp_dei_source : SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;
        ing_command_info->inner_tag.pcp_dei_source = (ing_command_info->inner_tag.vid_source != SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY) ? 
            ing_inner_pcp_dei_source : SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;
    }

    if (eg_command_info) {
        eg_command_info->outer_tag.pcp_dei_source = (eg_command_info->outer_tag.vid_source != SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY) ?
            eg_outer_pcp_dei_source : SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;
        eg_command_info->inner_tag.pcp_dei_source = (eg_command_info->inner_tag.vid_source != SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY) ?
            eg_inner_pcp_dei_source : SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;
    }

    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_dpp_vlan_translate_tpid_to_ppd
 * Description   : 
 *      Internal function to convert a BCM API TPID outer/inner values
 *      to PPD hardware Ingress/Egress structures. For Ingress, the TPID
 *      values are validated for a matching TPID profile. If the forced
 *      TPID profile is set, the TPIDs aren't validated, in order to enable
 *      default reserved action entries.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit             (IN) BCM unit number
 *      outer_tpid       (IN) Outer TPID value
 *      inner_tpid       (IN) Inner TPID value
 *      ing_command_info (OUT) A pointer to a PPD Ingress VLAN edit entry or
 *                            NULL if no conversion to Ingress required.
 *      eg_command_info  (OUT) A pointer to a PPD Egress VLAN edit entry or
 *                            NULL if no conversion to Egress required.
 *      forced_tpid_profile (IN) A pointer to a TPID profile. If not NULL,
 *                           the supplied TPID profile is set to the Ingress
 *                           PPD struct with no validations, while the PPD
 *                           TPID index fields are set to 0.
 */
int 
_bcm_dpp_vlan_translate_tpid_to_ppd(
    int unit,
    uint32 flags,
    uint16 outer_tpid,
    uint16 inner_tpid,
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO *ing_command_info,
    SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO *eg_command_info,
    int *forced_tpid_profile
    )
{
    uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
    SOC_PPD_LLP_PARSE_TPID_VALUES tpid_vals;
    int nof_tpids = 0;
    int outer_tpid_index, inner_tpid_index, inner_tpid_only;
    int tpid_profile, tpid_count;
    bcm_error_t rv;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* If it's a forced TPID, it wasn't meant to be valid.
       Use the supplied TPID profile for Ingress and set the TPID values to 0 */
    if (forced_tpid_profile) {
        if (ing_command_info) {
            ing_command_info->tpid_profile = *forced_tpid_profile;
            ing_command_info->outer_tag.tpid_index = 0;
            ing_command_info->inner_tag.tpid_index = 0;
        }

        if (eg_command_info) {
            eg_command_info->outer_tag.tpid_index = 0;
            eg_command_info->inner_tag.tpid_index = 0;
        }
        BCM_EXIT;
    }

    /* For Ingress, resolve the TPID Profile */
    if (ing_command_info) {

        /* Create an array of the supplied TPIDs */
        if (outer_tpid) {
            tpids[nof_tpids++] = outer_tpid;
        }
        if (inner_tpid) {
            tpids[nof_tpids++] = inner_tpid;
        }
        inner_tpid_only = (inner_tpid && !outer_tpid);

        /* Resolve the TPID Profile from the supplied TPIDs */
        rv = _bcm_petra_vlan_translation_tpids_to_tpid_profile(unit,
                    tpids, nof_tpids, inner_tpid_only,
                    &(ing_command_info->tpid_profile));

        /* If tpid_profile is found and BCM_VLAN_ACTION_SET_TPID_ALLOW_DESTROY is set, destroy an existing tpid_profile. */
        if ((rv == BCM_E_NONE) && (flags & BCM_VLAN_ACTION_SET_TPID_ALLOW_DESTROY)) {
            if(nof_tpids >= 1) {
                _rv = _bcm_dpp_wb_port_tpid_count_get(unit, &tpid_count, 0, ing_command_info->tpid_profile);
                if (_rv != BCM_E_NONE) { \
                    BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: get tpid_count failed, unit %d"),FUNCTION_NAME(), unit));
                }
               --tpid_count;
                _rv = _bcm_dpp_wb_port_tpid_count_set(unit, tpid_count, 0, ing_command_info->tpid_profile);
                if (_rv != BCM_E_NONE) { \
                  BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: set tpid_count failed, unit %d"),FUNCTION_NAME(), unit));
                }
            }
            if(nof_tpids >= 2) {
                _rv = _bcm_dpp_wb_port_tpid_count_get(unit, &tpid_count, 1, ing_command_info->tpid_profile);
                if (_rv != BCM_E_NONE) { \
                    BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: get tpid_count failed, unit %d"),FUNCTION_NAME(), unit));
                }
                --tpid_count;
                _rv = _bcm_dpp_wb_port_tpid_count_set(unit, tpid_count, 1, ing_command_info->tpid_profile);
                if (_rv != BCM_E_NONE) { \
                  BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: set tpid_count failed, unit %d"),FUNCTION_NAME(), unit));
                }
            }
        } else if ((rv == BCM_E_PARAM) && (flags & BCM_VLAN_ACTION_SET_TPID_ALLOW_CREATE)) {
            /* If tpid_profile is not found and BCM_VLAN_ACTION_SET_TPID_ALLOW is set, create a new tpid_profile. */
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_tpid_profile_alloc(unit, tpids, nof_tpids, FALSE, &tpid_profile));
        
            if(nof_tpids >= 1) {
                _rv = _bcm_dpp_wb_port_tpid_count_get(unit, &tpid_count, 0, tpid_profile);
                if (_rv != BCM_E_NONE) { \
                    BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: get tpid_count failed, unit %d"),FUNCTION_NAME(), unit));
                }
                ++tpid_count;
                _rv = _bcm_dpp_wb_port_tpid_count_set(unit, tpid_count, 0, tpid_profile);
                if (_rv != BCM_E_NONE) { \
                  BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: set tpid_count failed, unit %d"),FUNCTION_NAME(), unit));
                }
            }
            if(nof_tpids  >= 2) {
                _rv = _bcm_dpp_wb_port_tpid_count_get(unit, &tpid_count, 1, tpid_profile);
                if (_rv != BCM_E_NONE) { \
                    BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: get tpid_count failed, unit %d"),FUNCTION_NAME(), unit));
                }
                ++tpid_count;
                _rv = _bcm_dpp_wb_port_tpid_count_set(unit, tpid_count, 1, tpid_profile);
                if (_rv != BCM_E_NONE) { \
                  BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: set tpid_count failed, unit %d"),FUNCTION_NAME(), unit));
                }
            }
            
            ing_command_info->tpid_profile = tpid_profile;
        } else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* Retrieve TPID index according to global TPID values */
    SOC_PPD_LLP_PARSE_TPID_VALUES_clear(&tpid_vals);
    soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(soc_sand_dev_id, &tpid_vals);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Lookup the TPIDs in the global TPID table and find the TPID index value */
    outer_tpid_index = 0;
    if (outer_tpid) {
        rv = _bcm_petra_vlan_translate_match_tpid_value_to_index(unit, outer_tpid, &tpid_vals, &outer_tpid_index);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    inner_tpid_index = 0;
    if (inner_tpid) {
        rv = _bcm_petra_vlan_translate_match_tpid_value_to_index(unit, inner_tpid, &tpid_vals, &inner_tpid_index);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Set the TPID indexes in the appropriate PPD structure, in case a VID action is configured */
    if (ing_command_info) {
        ing_command_info->outer_tag.tpid_index = (ing_command_info->outer_tag.vid_source != SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY) ?
            outer_tpid_index : 0;
        ing_command_info->inner_tag.tpid_index = (ing_command_info->inner_tag.vid_source != SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY) ? 
            inner_tpid_index : 0;
    }

    if (eg_command_info) {
        eg_command_info->outer_tag.tpid_index = (eg_command_info->outer_tag.vid_source != SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY) ?
            outer_tpid_index : 0;
        eg_command_info->inner_tag.tpid_index = (eg_command_info->inner_tag.vid_source != SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY) ?
            inner_tpid_index : 0;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_dpp_vlan_translate_action_to_ppd
 * Description   : 
 *      Internal function to convert a BCM DPP action entry to PPD hardware
 *      Ingress/Egress structures. The conversion handles VID, PCP & TPID
 *      fields by calling dedicated conversion functions.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit             (IN) BCM unit number
 *      bcm_dpp_action   (IN) BCM DPP action entry that includes VID, PCP &
 *                            TPID values for Outer and Inner VLAN tags.
 *      ing_command_info (OUT) A pointer to a PPD Ingress VLAN edit entry or
 *                            NULL if no conversion to Ingress required.
 *      eg_command_info  (OUT) A pointer to a PPD Egress VLAN edit entry or
 *                            NULL if no conversion to Egress required.
 *      forced_tpid_profile (IN) A pointer to a forced TPID profile or NULL.
 */
int 
_bcm_dpp_vlan_translate_action_to_ppd(
    int unit,
    uint32 flags,
    _bcm_dpp_vlan_translate_action_t *bcm_dpp_action,
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO *ing_command_info,
    SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO *eg_command_info,
    int *forced_tpid_profile)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(bcm_dpp_action);

    /* If an Ingress PPD command info struct was supplied, fill it with _bcm_dpp_vlan_translate_action_t values */
    if (ing_command_info) {
        /* Initialize the command info structure */
        SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(ing_command_info);

        /* Map the BCM DPP VID action data to a matching Ingress PPD VLAN Edit data */
        rv = _bcm_dpp_vlan_translate_vid_action_to_ppd(unit, bcm_dpp_action,
                    ing_command_info, NULL);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Map the BCM DPP PCP-DEI action data to matching Ingress PPD VLAN Edit data */
        rv = _bcm_dpp_vlan_translate_pcp_action_to_ppd(unit,
                    bcm_dpp_action->outer.pcp_action, bcm_dpp_action->inner.pcp_action,
                    ing_command_info, NULL);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Calculate the Ingress PPD TPID Profile and TPID indexes */
        rv = _bcm_dpp_vlan_translate_tpid_to_ppd(unit, flags,
                    bcm_dpp_action->outer.tpid_val, bcm_dpp_action->inner.tpid_val,
                    ing_command_info, NULL, forced_tpid_profile);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* If an Egress PPD command info struct was supplied, fill it with _bcm_dpp_vlan_translate_action_t values */
    if (eg_command_info) {
        /* Initialize the command info structure */
        SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO_clear(eg_command_info);

        /* Map the BCM DPP VID action data to a matching Egress PPD VLAN Edit data */
        rv = _bcm_dpp_vlan_translate_vid_action_to_ppd(unit, bcm_dpp_action,
                    NULL, eg_command_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Map the BCM DPP PCP-DEI action data to matching Egress PPD VLAN Edit data */
        rv = _bcm_dpp_vlan_translate_pcp_action_to_ppd(unit,
                    bcm_dpp_action->outer.pcp_action, bcm_dpp_action->inner.pcp_action,
                    NULL, eg_command_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Calculate the Egress PPD TPID */
        rv = _bcm_dpp_vlan_translate_tpid_to_ppd(unit, flags,
                    bcm_dpp_action->outer.tpid_val, bcm_dpp_action->inner.tpid_val,
                    NULL, eg_command_info, forced_tpid_profile);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_petra_vlan_edit_eg_command_id_set
 * Description   : 
 *      Internal function to set a SW mapping between an Egress VLAN edit
 *      command key and a given action ID. The SW mapping table emulates the
 *      HW IVEC mapping table that exists in the Ingress to map between
 *      a combination of tag format and VLAN edit profile to an action ID.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      command_key     (IN) A pointer to an Egress PPD command entry key.
 *      action_id       (IN) Action ID to map to the Egress PPD command
 *                           entry key.
 */
int 
_bcm_petra_vlan_edit_eg_command_id_set(
    int unit, 
    SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY *command_key,
    uint32 action_id)
{
    int command_map_key;
    uint8 sw_db_action_id;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(command_key);

    /* Create a key to the commands mapping SW table */
    command_map_key = _DPP_EG_CREATE_COMMAND_ACTION_KEY(command_key->edit_profile, command_key->tag_format);

    /* Store the action id that is mapped to the key */
    if (command_map_key < SOC_DPP_NOF_EGRESS_VLAN_EDIT_ACTION_MAPPINGS(unit)) {
        sw_db_action_id = action_id;
        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_VLAN_EDIT_EG_ACTION_MAPPING,
                                       &sw_db_action_id, command_map_key);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Got invalid key for EVE commands mappping table. edit_profile - %d, tag_format - %d"), 
                         command_key->edit_profile, command_key->tag_format));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_petra_vlan_edit_eg_command_id_get
 * Description   : 
 *      Internal function to get an action ID from a SW mapping table, given a
 *      an Egress VLAN edit command key. The SW mapping table emulates the
 *      HW IVEC mapping table that exists in the Ingress to map between
 *      a combination of tag format and VLAN edit profile to an action ID.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      command_key     (IN) A pointer to an Egress PPD command entry key.
 *      action_id       (OUT) A pointer to the result action ID.
 */
int 
_bcm_petra_vlan_edit_eg_command_id_get(
    int unit, 
    SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY *command_key,
    uint32 *action_id)
{
    int command_map_key;
    uint8 sw_db_action_id;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(command_key);
    BCMDNX_NULL_CHECK(action_id);

    /* Create a key to the commands mapping SW table */
    command_map_key = _DPP_EG_CREATE_COMMAND_ACTION_KEY(command_key->edit_profile, command_key->tag_format);

    /* Get the action id that is mapped to the key */
    if (command_map_key < SOC_DPP_NOF_EGRESS_VLAN_EDIT_ACTION_MAPPINGS(unit)) {

        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_VLAN_EDIT_EG_ACTION_MAPPING,
                                       &sw_db_action_id, command_map_key);
        BCMDNX_IF_ERR_EXIT(rv);
        *action_id = sw_db_action_id;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Got invalid key for EVE commands mappping table. edit_profile - %d, tag_format - %d"), 
                         command_key->edit_profile, command_key->tag_format));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_petra_vlan_edit_eg_command_id_find
 * Description   : 
 *      Internal function to perform an action ID lookup in the SW Egress action
 *      mapping table. The SW mapping table emulates the HW IVEC mapping table
 *      that exists in the Ingress to map between a combination of tag format
 *      and VLAN edit profile to an action ID. The lookup starts from the
 *      supplied mapping key onwards. If the find_from_current flag is set, the
 *      supplied entry is also matched for action ID.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      action_id       (IN) Action ID to match in the lookup
 *      find_from_current (IN) Flag that determines whether the supplied entry
 *                           itself is matched for action ID or the key is
 *                           incremented before the lookup starts.
 *      command_key     (IN/OUT) The SW action ID mapping key that is the
 *                           starting point for the lookup as well as the
 *                           result key when the action ID is matched.
 */
int
_bcm_petra_vlan_edit_eg_command_id_find(
    int unit,
    uint32 action_id,
    int find_from_current,
    SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY *command_key)
{
    int command_map_key;
    uint8 sw_db_action_id;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(command_key);

    /* Create a key to the commands mapping SW table */
    command_map_key = _DPP_EG_CREATE_COMMAND_ACTION_KEY(command_key->edit_profile, command_key->tag_format);

    /* If the lookup should be from the next entry, increment the search starting point */
    if (!find_from_current) {
        command_map_key++;
    }

    /* Go over the commands mapping SW table entries, starting from the supplied key */
    for (; command_map_key < SOC_DPP_NOF_EGRESS_VLAN_EDIT_ACTION_MAPPINGS(unit); command_map_key++) {
        /* Look for an entry with the requested action ID */
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_VLAN_EDIT_EG_ACTION_MAPPING,
                                       &sw_db_action_id, command_map_key);
        BCMDNX_IF_ERR_EXIT(rv);

        if (sw_db_action_id == action_id) {
            /* Found the requested entry */
            command_key->edit_profile = _DPP_EG_GET_EDIT_PROFILE_FROM_MAPPING_KEY(command_map_key);
            command_key->tag_format = _DPP_EG_GET_TAG_FORMAT_FROM_MAPPING_KEY(command_map_key);
            BCM_RETURN_VAL_EXIT(BCM_E_EXISTS);
        }
    }

    BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_petra_vlan_translate_action_id_set_verify
 * Description   : 
 *      Internal function to verify the configuration for the
 *      bcm_vlan_translate_action_id_set API. The function verifies that only
 *      supported VID/PCP actions are supplied based on Ingress/Egress
 *      selection.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      is_ingress      (IN) A flag for Ingress/Egress operation.
 *      action          (IN) BCM API action structure.
 */
STATIC int
_bcm_petra_vlan_translate_action_id_set_verify(
    int unit,
    int is_ingress,
    bcm_vlan_action_set_t *action)
{    
    BCMDNX_INIT_FUNC_DEFS;

    /* Basic parameter validation */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(action);

    /* Validate the VID values */
    switch (action->dt_outer) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
    case bcmVlanActionCopy:
    case bcmVlanActionOuterAdd:
    case bcmVlanActionInnerAdd:
        break;
    case bcmVlanActionMappedReplace:
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_outer for Ingress must be bcmVlanAction... None / Add / Replace / Copy / Delete / Outer Add / Inner Add")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_outer must be bcmVlanAction... None / Add / Replace / Copy / Delete / MappedReplace / MappedAdd / Outer Add / Inner Add")));
    }

    switch (action->dt_inner) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
    case bcmVlanActionCopy:
    case bcmVlanActionOuterAdd:
    case bcmVlanActionInnerAdd:
        break;
    case bcmVlanActionMappedReplace:
    case bcmVlanActionMappedAdd:
        if (is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_inner for Ingress must be bcmVlanAction... None / Add / Replace / Copy / Delete / Outer Add / Inner Add")));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_inner must be bcmVlanAction... None / Add / Replace / Copy / Delete / MappedReplace / MappedAdd / Outer Add / Inner Add")));
    }

    /* Validate the QoS values */
    switch (action->dt_outer_pkt_prio) {
    case bcmVlanActionNone:        
    case bcmVlanActionAdd:      
    case bcmVlanActionReplace:      
    case bcmVlanActionCopy:
    case bcmVlanActionOuterAdd:
    case bcmVlanActionInnerAdd:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_outer_pkt_prio must be bcmVlanAction... None / Add / Replace / Copy / Outer Add / Inner Add")));
    }

    switch (action->dt_inner_pkt_prio) {
    case bcmVlanActionNone:        
    case bcmVlanActionAdd:      
    case bcmVlanActionReplace:      
    case bcmVlanActionCopy:
    case bcmVlanActionOuterAdd:
    case bcmVlanActionInnerAdd:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->dt_inner_pkt_prio must be bcmVlanAction... None / Add / Replace / Copy / Outer Add / Inner Add")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_petra_vlan_translate_action_id_set
 * Description   : 
 *      Internal function that performs the operations that are required by the
 *      bcm_vlan_translate_action_id_set API. The function is required in order
 *      to enable usage of it for configuration of reserved default action
 *      entries by setting a forced TPID profile.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      flags           (IN) BCM_VLAN_ACTION_SET_XXX. Either Ingress or Egress
 *                           should be selected.
 *      action_id       (IN) Action ID. Must be allocated before set.
 *      action          (IN) A pointer to a BCM API action configuration structure
 *      forced_tpid_profile (IN) A pointer to a forced TPID profile or NULL.
 */ 
int 
_bcm_petra_vlan_translate_action_id_set(
    int unit, 
    uint32 flags, 
    int action_id, 
    bcm_vlan_action_set_t *action,
    int *forced_tpid_profile)
{
    int rv, is_first_lookup;
    uint32 is_ingress, soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO ing_command_info;
    SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY eg_command_key;
    SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO eg_command_info;
    _bcm_dpp_vlan_translate_action_t bcm_dpp_action;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* Basic parameter validation */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(action);

    /* Verify that one of the Ingress/Egress flags is set, and only one is set. */
    is_ingress = (flags & BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    if ((is_ingress && (flags & BCM_VLAN_ACTION_SET_EGRESS)) ||
        (!is_ingress && !(flags & BCM_VLAN_ACTION_SET_EGRESS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN translate ID should be for either Ingress or Egress")));
    }

    /* Perform BCM related validations on the action. More HW dependent validations are performed upon
       translation to PPD by _bcm_dpp_vlan_translate_action_to_ppd */
    rv = _bcm_petra_vlan_translate_action_id_set_verify(unit, is_ingress, action);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Verify that the entry is allocated */
    rv = bcm_dpp_am_vlan_edit_action_id_is_alloc(unit, flags, action_id);
    if (rv == BCM_E_NOT_FOUND) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The supplied VLAN translate action ID isn't allocated")));
    } else if (rv != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The supplied VLAN translate action ID is invalid")));
    }

    /* Convert the BCM API supplied VLAN translation action set to a partial bcm_dpp structure
       for storage in the actions SW table */
    rv = _bcm_petra_vlan_translate_action_to_bcm_dpp(unit, action, &bcm_dpp_action);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Handle the Ingress variation. The Set is done to the HW */
    if (is_ingress) {

        /* Convert the command from the SW stored BCM PPD format to the HW PPD format */
        rv = _bcm_dpp_vlan_translate_action_to_ppd(unit, flags, &bcm_dpp_action, &ing_command_info, NULL, forced_tpid_profile);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Write the command to the Ingress HW commands table */
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_command_info_set(soc_sand_dev_id, action_id, &ing_command_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* Handle the Egress variation. The Set is done to a SW DB and to the HW */
    else {

        /* The function validates and translates the action to the PPD structure */
        rv = _bcm_dpp_vlan_translate_action_to_ppd(unit, flags, &bcm_dpp_action, NULL, &eg_command_info, forced_tpid_profile);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Find all the mappings to the command ID in the SW command mappings table.
           In the first lookup, use the find from the current entry as not to miss the
           the first indexed entry */
        SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY_clear(&eg_command_key);
        eg_command_key.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        is_first_lookup = TRUE;

        while (_bcm_petra_vlan_edit_eg_command_id_find(unit, action_id, is_first_lookup, &eg_command_key) == BCM_E_EXISTS) {

            /* Set the command to the HW using the mapping key as the table index */
            soc_sand_rv = soc_ppd_eg_vlan_edit_command_info_set(soc_sand_dev_id, &eg_command_key, &eg_command_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            is_first_lookup = FALSE;
        }
    }

    /* Store the command in the appropriate actions SW table. This should be
       done only after the validations in _bcm_dpp_vlan_translate_action_to_ppd */
    rv = _bcm_dpp_vlan_edit_action_set(unit, action_id, is_ingress, &bcm_dpp_action);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      bcm_petra_vlan_translate_action_id_set
 * Description   : 
 *      BCM API to set the VLAN edit action configuration to a pre allocated
 *      Ingress/Egress action ID. bcm_vlan_translate_action_id_create should be
 *      called in order to allocate an action ID. Reserved default entries
 *      don't require user creation, if not destroyed. The main functionality
 *      is perform by the internal function _bcm_petra_vlan_translate_action_id_set.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      flags           (IN) BCM_VLAN_ACTION_SET_XXX. Either Ingress or Egress
 *                           should be selected.
 *      action_id       (IN) Action ID. Must be allocated before set.
 *      action          (IN) A pointer to a BCM API action configuration structure
 */ 
int 
bcm_petra_vlan_translate_action_id_set(
    int unit, 
    uint32 flags, 
    int action_id, 
    bcm_vlan_action_set_t *action)
{
    int rv;
    unsigned int soc_sand_dev_id;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* Handle the API only if the SOC property is set to Advanced VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(soc_sand_dev_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Advanced mode")));
    }

    /* Call an internal function to perform the set */
    rv = _bcm_petra_vlan_translate_action_id_set(unit, flags, action_id, action, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      bcm_petra_vlan_translate_action_id_get
 * Description   : 
 *      BCM API to get the VLAN edit action configuration for an allocated
 *      Ingress/Egress action ID. The data is retrieved from the SW action
 *      tables.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      flags           (IN) BCM_VLAN_ACTION_SET_XXX. Either Ingress or Egress
 *                           should be selected.
 *      action_id       (IN) Action ID. Must be allocated before get.
 *      action          (OUT) A pointer to a BCM API action configuration structure
 */ 
int 
bcm_petra_vlan_translate_action_id_get(
    int unit, 
    uint32 flags, 
    int action_id, 
    bcm_vlan_action_set_t *action)
{
    int rv;
    uint32 is_ingress;
    unsigned int soc_sand_dev_id;
    _bcm_dpp_vlan_translate_action_t bcm_dpp_action;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* Handle the API only if the SOC property is set to Advanced VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(soc_sand_dev_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Advanced mode")));
    }

    /* Basic parameter validation */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(action);

    /* Verify that one of the Ingress/Egress flags is set, and only one is set. */
    is_ingress = (flags & BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    if ((is_ingress && (flags & BCM_VLAN_ACTION_SET_EGRESS)) ||
        (!is_ingress && !(flags & BCM_VLAN_ACTION_SET_EGRESS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN translate ID should be for either Ingress or Egress")));
    }

    /* Verify that the entry is allocated */
    rv = bcm_dpp_am_vlan_edit_action_id_is_alloc(unit, flags, action_id);
    if (rv == BCM_E_NOT_FOUND) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The supplied VLAN translate action ID isn't allocated")));
    } else if (rv != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The supplied VLAN translate action ID is invalid")));
    }

    /* Get the SW stored action in the BCM DPP format */
    rv = _bcm_dpp_vlan_edit_action_get(unit, action_id, is_ingress, &bcm_dpp_action);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Convert the action info from BCM DPP format to BCM API VLAN translation action set */
    rv = _bcm_petra_vlan_translate_action_from_bcm_dpp(unit, &bcm_dpp_action, action);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_petra_vlan_translate_action_class_key_to_ppd
 * Description   : 
 *      Internal function to convert a BCM API structure for VLAN translate
 *      action class to PPD key structure to a mapping between a combination
 *      of tag format and VLAN edit profile to an action ID.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit             (IN) BCM unit number
 *      action_class     (IN) A pointer to a BCM API
 *                            action class configuration structure that
 *                            contains a tag format and VLAN edit profile.
 *      ing_command_key  (OUT) A pointer to the result key structure to the
 *                            PPD IVEC map table or NULL.
 *      eg_command_key   (OUT) A pointer to the result key structure to the
 *                            PPD SW mapping table or NULL.
 */
int 
_bcm_petra_vlan_translate_action_class_key_to_ppd(
    int unit, 
    bcm_vlan_translate_action_class_t *action_class,
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY *ing_command_key,
    SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY *eg_command_key)
{

  BCMDNX_INIT_FUNC_DEFS;

  /* If an Ingress PPD command key struct was supplied, fill it with bcm_vlan_translate_action_class_t values */
  if (ing_command_key != NULL) {

      SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY_clear(ing_command_key);
      ing_command_key->tag_format = action_class->tag_format_class_id;
      ing_command_key->edit_profile = action_class->vlan_edit_class_id;
  }

  /* If an Egress PPD command key struct was supplied, fill it with bcm_vlan_translate_action_class_t values */
  if (eg_command_key != NULL) {

      SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY_clear(eg_command_key);
      eg_command_key->tag_format = action_class->tag_format_class_id;
      eg_command_key->edit_profile = action_class->vlan_edit_class_id;
  }

   BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_petra_vlan_translate_action_class_set_verify
 * Description   : 
 *      Internal function to verify the configuration for the
 *      bcm_petra_vlan_translate_action_class_set API. The function validates
 *      the tag format and the VLAN edit profile values.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit             (IN) BCM unit number
 *      action_class     (IN) A pointer to a BCM API action class structure
 *                            that contains the mapping key for the retrieval
 *                            as well as the mapping result action ID.
 */
int 
_bcm_petra_vlan_translate_action_class_set_verify(
    int unit,
    uint32 is_ingress,
    bcm_vlan_translate_action_class_t *action_class)
{
    int nof_action_ids;
    BCMDNX_INIT_FUNC_DEFS;

    /* Validate the tag format */
    if (action_class->tag_format_class_id >= DPP_NOF_VLAN_TAG_FORMATS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid VLAN tag format")));
    }

    /* Validate the VLAN edit profile */
    BCM_DPP_VLAN_EDIT_PROFILE_VALID(unit, action_class->vlan_edit_class_id,
        (action_class->flags & BCM_VLAN_ACTION_SET_INGRESS));

    /* Validate the action ID according to Ingress/Egress action ID values */
    nof_action_ids = (is_ingress) ? SOC_DPP_NOF_INGRESS_VLAN_EDIT_ACTION_IDS(unit) : 
        SOC_DPP_NOF_EGRESS_VLAN_EDIT_ACTION_IDS(unit);
    if ((action_class->vlan_translation_action_id < 0) ||
        (action_class->vlan_translation_action_id >= nof_action_ids)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid VLAN edit action ID")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      bcm_petra_vlan_translate_action_class_set
 * Description   : 
 *      BCM API function to set a VLAN translate action class mapping. The API
 *      maps a combination of tag format and VLAN edit profile to an action ID.
 *      The mapping is stored in the IVEC HW table for Ingress and in a SW table
 *      for Egress.
 *      In the Egress case, if the action itself is already allocated,
 *      the action configuration is fetched from the SW table and set to the
 *      entry that matches the mapping key in the Egress commands HW table.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit             (IN) BCM unit number
 *      action_class     (IN) A pointer to a BCM API action class configuration
 *                            structure that contains a tag format, VLAN edit
 *                            profile and the mapped action ID.
 */
int 
bcm_petra_vlan_translate_action_class_set(
    int unit, 
    bcm_vlan_translate_action_class_t *action_class)
{
    int rv;
    uint32 is_ingress, soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY ing_command_key;
    SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY eg_command_key;
    SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO eg_command_info;
    _bcm_dpp_vlan_translate_action_t bcm_dpp_action;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* Handle the API only if the SOC property is set to Advanced VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(soc_sand_dev_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Advanced mode")));
    }

    /* Basic parameter validation */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(action_class);

    /* Verify that one of the Ingress/Egress flags is set, and only one is set. */
    is_ingress = (action_class->flags & BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    if ((is_ingress && (action_class->flags & BCM_VLAN_ACTION_SET_EGRESS)) ||
        (!is_ingress && !(action_class->flags & BCM_VLAN_ACTION_SET_EGRESS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN translate action class should be for either Ingress or Egress")));
    }

    /* Handle the Ingress variation. The Set is done to the HW.*/
    if (is_ingress) {

        /* Set the VLAN translate action class key to the user supplied values */
        rv = _bcm_petra_vlan_translate_action_class_key_to_ppd(unit, action_class, &ing_command_key, NULL);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set the user command ID using the PPD VLAN translate action class key */
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_command_id_set(soc_sand_dev_id, &ing_command_key,
                        action_class->vlan_translation_action_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* Handle the Egress variation. The Set is done to a SW DB. */
    else {

        /* Set the VLAN translate action class key to the user supplied values */
        rv = _bcm_petra_vlan_translate_action_class_key_to_ppd(unit, action_class, NULL, &eg_command_key);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set the mapping to a SW table */
        rv = _bcm_petra_vlan_edit_eg_command_id_set(unit, &eg_command_key, action_class->vlan_translation_action_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /* In case the command ID is valid, there's a need to update the Egress commands HW table,
           in the command ID entry with the action that was stored (if stored) in the actions SW table */
        if (bcm_dpp_am_vlan_edit_action_id_is_alloc(unit, BCM_VLAN_ACTION_SET_EGRESS, action_class->vlan_translation_action_id) == BCM_E_EXISTS) {

            /* Get the SW stored action in the BCM DPP format */
            rv = _bcm_dpp_vlan_edit_action_get(unit, action_class->vlan_translation_action_id, is_ingress, &bcm_dpp_action);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Convert the command from the SW stored BCM PPD format to the HW PPD format */
            rv = _bcm_dpp_vlan_translate_action_to_ppd(unit, 0, &bcm_dpp_action, NULL, &eg_command_info, NULL);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Set the commands HW table in the mapped location with the SW stored command  */
            soc_sand_rv = soc_ppd_eg_vlan_edit_command_info_set(soc_sand_dev_id, &eg_command_key, &eg_command_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_petra_vlan_translate_action_class_get_verify
 * Description   : 
 *      Internal function to verify the configuration for the
 *      bcm_petra_vlan_translate_action_class_get API. The function validates
 *      the tag format and the VLAN edit profile values.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit             (IN) BCM unit number
 *      action_class     (IN) A pointer to a BCM API action class structure
 *                            that contains the mapping key for the retrieval
 *                            as well as the mapping result action ID.
 */
int 
_bcm_petra_vlan_translate_action_class_get_verify(
    int unit, 
    bcm_vlan_translate_action_class_t *action_class)
{
    BCMDNX_INIT_FUNC_DEFS;

    /* Validate the tag format */
    if (action_class->tag_format_class_id >= DPP_NOF_VLAN_TAG_FORMATS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid VLAN tag format")));
    }

    /* Validate the VLAN edit profile */
    BCM_DPP_VLAN_EDIT_PROFILE_VALID(unit, action_class->vlan_edit_class_id,
        (action_class->flags & BCM_VLAN_ACTION_SET_INGRESS));

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      bcm_petra_vlan_translate_action_class_get
 * Description   : 
 *      BCM API function to get an action out of a VLAN translate action class
 *      mapping for a given mapping key that consists of a tag format and
 *      a VLAN edit profile.
 *      The action ID is retrieved from the IVEC HW table for Ingress and from
 *      a SW table for Egress.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit             (IN) BCM unit number
 *      action_class     (IN/OUT) A pointer to a BCM API action class structure
 *                            that contains the mapping key for the retrieval
 *                            as well as the mapping result action ID.
 */
int 
bcm_petra_vlan_translate_action_class_get(
    int unit, 
    bcm_vlan_translate_action_class_t *action_class)
{
    int rv;
    uint32 is_ingress, command_id, soc_sand_rv, max_command_id;
    unsigned int soc_sand_dev_id;
    SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY ing_command_key;
    SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY eg_command_key;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* Handle the API only if the SOC property is set to Advanced VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(soc_sand_dev_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Advanced mode")));
    }

    /* Basic parameter validation */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(action_class);

    /* Verify that one of the Ingress/Egress flags is set, and only one is set. */
    is_ingress = (action_class->flags & BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    if ((is_ingress && (action_class->flags & BCM_VLAN_ACTION_SET_EGRESS)) ||
        (!is_ingress && !(action_class->flags & BCM_VLAN_ACTION_SET_EGRESS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN translate action class should be for either Ingress or Egress")));
    }

    /* Perform BCM related validations */
    rv = _bcm_petra_vlan_translate_action_class_get_verify(unit, action_class);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Handle the Ingress variation. The Get is done from the HW. */
    if (is_ingress) {

        /* Set the VLAN translate action class key to the user supplied values */
        rv = _bcm_petra_vlan_translate_action_class_key_to_ppd(unit, action_class, &ing_command_key, NULL);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Get the Ingress LIF info from the HW */
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_command_id_get(soc_sand_dev_id, &ing_command_key, &command_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        max_command_id = SOC_DPP_NOF_INGRESS_VLAN_EDIT_ACTION_IDS(soc_sand_dev_id) - 1;
    }
    /* Handle the Egress variation. The Get is done from a SW DB. */
    else {
        /* Set the VLAN translate action class key to the user supplied values */
        rv = _bcm_petra_vlan_translate_action_class_key_to_ppd(unit, action_class, NULL, &eg_command_key);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Get the mapping from a SW table */
        rv = _bcm_petra_vlan_edit_eg_command_id_get(unit, &eg_command_key, &command_id);
        BCMDNX_IF_ERR_EXIT(rv);
        max_command_id = SOC_DPP_NOF_EGRESS_VLAN_EDIT_ACTION_IDS(soc_sand_dev_id) - 1;
    }

    /* Validate the result command ID */
    if (command_id >  max_command_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Got invalid command id")));
    }
    action_class->vlan_translation_action_id = command_id;

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_dpp_vlan_translate_action_t_init
 * Description   : 
 *      Internal function to initialize a SW stored BCM DPP action structure.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      action           (OUT) A pointer to a BCM DPP action structure that is
 *                             initialized.
 */
void 
_bcm_dpp_vlan_translate_action_t_init(
    _bcm_dpp_vlan_translate_action_t *action)
{
    if (action) {
        sal_memset(action, 0 , sizeof(*action));

        action->outer.vid_action = bcmVlanActionNone;
        action->outer.pcp_action = bcmVlanActionNone;
        action->outer.tpid_action = bcmVlanTpidActionNone;
        action->outer.tpid_val = 0;

        action->inner.vid_action = bcmVlanActionNone;
        action->inner.pcp_action = bcmVlanActionNone;
        action->inner.tpid_action = bcmVlanTpidActionNone;
        action->inner.tpid_val = 0;
    }
}


/*
 * Function   :
 *      _bcm_dpp_vlan_edit_action_set
 * Description   : 
 *      Internal function to set a BCM DPP action entry structure to the
 *      appropriate Ingress/Egress SW table in the supplied action ID entry
 *      location.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit             (IN) BCM unit number
 *      action_id        (IN) Action ID that serves as a key to the BCM DPP
 *                            action SW table.
 *      is_ingress       (IN) A flag for Ingress/Egress operation.
 *      action           (IN) A pointer to the BCM DPP action entry structure.
 */
int
_bcm_dpp_vlan_edit_action_set(
    int unit,
    int action_id,
    uint32 is_ingress,
    _bcm_dpp_vlan_translate_action_t *action)
{
    int action_table;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(action);

    /* Select the action table according to the is_ingress flag */
    action_table = (is_ingress) ? SOC_DPP_WB_ENGINE_VAR_ARAD_VLAN_EDIT_ING_ACTION : SOC_DPP_WB_ENGINE_VAR_ARAD_VLAN_EDIT_EG_ACTION;

    /* Set the supplied action info */
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, action_table, action, action_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function   :
 *      _bcm_dpp_vlan_edit_action_get
 * Description   : 
 *      Internal function to get a BCM DPP action entry structure from the
 *      appropriate Ingress/Egress SW table using the supplied action ID
 *      table key.
 *      Applicable only for Advanced VLAN edit mode.
 * Parameters   :
 *      unit             (IN) BCM unit number
 *      action_id        (IN) Action ID that serves as a key to the BCM DPP
 *                            action SW table.
 *      is_ingress       (IN) A flag for Ingress/Egress operation.
 *      action           (OUT) A pointer to the result BCM DPP action entry
 *                            structure.
 */
int
_bcm_dpp_vlan_edit_action_get(
    int unit,
    int action_id,
    uint32 is_ingress,
    _bcm_dpp_vlan_translate_action_t *action)
{
    int action_table;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(action);

    /* Select the action table according to the is_ingress flag */
    action_table = (is_ingress) ? SOC_DPP_WB_ENGINE_VAR_ARAD_VLAN_EDIT_ING_ACTION : SOC_DPP_WB_ENGINE_VAR_ARAD_VLAN_EDIT_EG_ACTION;

    /* Get the action info */
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, action_table, action, action_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_vlan_ip_action_add(
    int unit, 
    bcm_vlan_ip_t *vlan_ip, 
    bcm_vlan_action_set_t *action)
{
    bcm_error_t rv;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 entry_ndx, entry_ndx_tmp;
    SOC_SAND_PP_IPV4_SUBNET soc_sand_subnet, soc_sand_subnet_vid_info_iter, soc_sand_subnet_lif_cos_iter;
    SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_INFO soc_ppd_subnet_based_info, soc_ppd_subnet_based_info_iter;
    SOC_PPD_LLP_COS_IPV4_SUBNET_INFO llp_cos_info, soc_ppd_subnet_cos_info;
          
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    if ((vlan_ip->flags & BCM_VLAN_SUBNET_IP6)       ||
        (action->dt_outer      != bcmVlanActionNone) ||
        (action->dt_outer_prio != bcmVlanActionNone) ||
        (action->dt_inner      != bcmVlanActionNone) ||
        (action->dt_inner_prio != bcmVlanActionNone) ||
        (action->ot_outer      != bcmVlanActionNone) ||
        (action->ot_outer_prio != bcmVlanActionNone) ||
        (action->ot_inner      != bcmVlanActionNone) ||
        (action->it_outer      != bcmVlanActionNone) ||
        (action->it_inner      != bcmVlanActionNone) ||
        (action->it_inner_prio != bcmVlanActionNone) ||
        (action->ut_outer      != bcmVlanActionAdd) ||
        (action->ut_inner      != bcmVlanActionNone) ||
        (action->outer_pcp     != bcmVlanPcpActionNone) ||
        (action->inner_pcp     != bcmVlanPcpActionNone)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid action fields.")));
    }

    soc_sand_dev_id = (unit);    

    soc_sand_subnet.prefix_len = bcm_ip_mask_length(vlan_ip->mask);
    soc_sand_subnet.ip_address = vlan_ip->ip4;

    /* Verify TC are valid */
    if (action->priority >= BCM_COS_COUNT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->priority must be valid.")));
    }

    /* Verify VLAN or TC aleast are valid */
    if (action->priority == BCM_COS_INVALID && action->ut_outer != bcmVlanActionAdd) {
        /* Both invalid */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("action->ut_outer or action->priority must be valid.")));
    }

    for (entry_ndx = 0;
        (entry_ndx <= DPP_VID_ASSIGN_IPV4_SUBNET_INDEX_MAX);
         ++entry_ndx) {

        soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx, 
           &soc_sand_subnet_vid_info_iter, &soc_ppd_subnet_based_info_iter);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx,
            &soc_sand_subnet_lif_cos_iter, &llp_cos_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if ((!soc_ppd_subnet_based_info_iter.vid_is_valid && !llp_cos_info.tc_is_valid) ||
            ((soc_sand_subnet.ip_address == soc_sand_subnet_vid_info_iter.ip_address) &&
             (soc_sand_subnet.prefix_len == soc_sand_subnet_vid_info_iter.prefix_len))) {

            /* Entry already exists (override), or reached empty entry */
            break;

        } else if (soc_sand_subnet.prefix_len > soc_sand_subnet_vid_info_iter.prefix_len) {
            /* 
             * Entries of Subnet Classify table are sorted in descending order of prefix length.
             * The first entry has the longest prefix length.
             * The last entry has the shortest prefix length.
             */
            entry_ndx_tmp = DPP_VID_ASSIGN_IPV4_SUBNET_INDEX_MAX;

            soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx_tmp, 
                &soc_sand_subnet_vid_info_iter, &soc_ppd_subnet_based_info_iter);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx_tmp,
                &soc_sand_subnet_lif_cos_iter, &llp_cos_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* Table is always kept unfragmented, so if last entry is valid.
               the table is full */
            if (soc_ppd_subnet_based_info_iter.vid_is_valid || llp_cos_info.tc_is_valid) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("IP action table full")));
            }

            /* Push all entries till from the index we want to insert to down */
            while (--entry_ndx_tmp >= entry_ndx) {
                soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx_tmp, 
                    &soc_sand_subnet_vid_info_iter, &soc_ppd_subnet_based_info_iter);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
                soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx_tmp,
                    &soc_sand_subnet_lif_cos_iter, &llp_cos_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                if (soc_ppd_subnet_based_info_iter.vid_is_valid) {
                    soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx_tmp+1, 
                        &soc_sand_subnet_vid_info_iter, &soc_ppd_subnet_based_info_iter);

                    soc_ppd_subnet_based_info_iter.vid_is_valid = FALSE;

                    soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx_tmp, 
                        &soc_sand_subnet_vid_info_iter, &soc_ppd_subnet_based_info_iter);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }

                if (llp_cos_info.tc_is_valid) {
                    soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx_tmp+1,
                        &soc_sand_subnet_lif_cos_iter, &llp_cos_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    llp_cos_info.tc_is_valid = FALSE;

                    soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx_tmp,
                        &soc_sand_subnet_lif_cos_iter, &llp_cos_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }

                if (entry_ndx_tmp == 0) {
                    break;
                }
            }

            /* We've freed the required entry. Break the outer loop. */
            break;

        } else {
            /* Entry is equal in length (but different), or longer. Try the
               next entry */
        }
    }
    
    /* IPV4 to VID */
    SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(&soc_ppd_subnet_based_info);
    soc_ppd_subnet_based_info.vid_is_valid = (action->ut_outer == bcmVlanActionAdd) ? TRUE:FALSE;
    if (soc_ppd_subnet_based_info.vid_is_valid) {
        soc_ppd_subnet_based_info.vid = action->new_outer_vlan;
    }    
    soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx, 
           &soc_sand_subnet, &soc_ppd_subnet_based_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* IPV4 to TC */
    SOC_PPD_LLP_COS_IPV4_SUBNET_INFO_clear(&soc_ppd_subnet_cos_info);
    soc_ppd_subnet_cos_info.tc_is_valid = (action->priority == BCM_COS_INVALID) ? FALSE:TRUE;
    if (soc_ppd_subnet_cos_info.tc_is_valid) {
        soc_ppd_subnet_cos_info.tc = action->priority;
    }

    soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx, 
           &soc_sand_subnet, &soc_ppd_subnet_cos_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);   
    COMPILER_REFERENCE(rv);

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_ip_action_get(
    int unit, 
    bcm_vlan_ip_t *vlan_ip, 
    bcm_vlan_action_set_t *action)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 entry_ndx;
    SOC_SAND_PP_IPV4_SUBNET soc_sand_subnet, soc_sand_subnet_vid_info_iter, soc_sand_subnet_cos_info_iter;
    SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_INFO soc_ppd_subnet_based_info_iter;
    SOC_PPD_LLP_COS_IPV4_SUBNET_INFO soc_ppd_subnet_based_cos_info_iter;
          
    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);    

    soc_sand_subnet.prefix_len = bcm_ip_mask_length(vlan_ip->mask);
    soc_sand_subnet.ip_address = vlan_ip->ip4;

    for (entry_ndx = 0;
        (entry_ndx <= DPP_VID_ASSIGN_IPV4_SUBNET_INDEX_MAX);
         ++entry_ndx) {

        soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx, 
           &soc_sand_subnet_vid_info_iter, &soc_ppd_subnet_based_info_iter);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if ((soc_sand_subnet.prefix_len == soc_sand_subnet_vid_info_iter.prefix_len) &&
            (soc_sand_subnet.ip_address == soc_sand_subnet_vid_info_iter.ip_address)) {
            
            if (soc_ppd_subnet_based_info_iter.vid_is_valid) {
                /* Valid new VLAN */
                action->ut_outer = bcmVlanActionAdd;
                action->new_outer_vlan = soc_ppd_subnet_based_info_iter.vid;
            }

            soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx, 
                                                               &soc_sand_subnet_cos_info_iter, &soc_ppd_subnet_based_cos_info_iter);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if ((soc_sand_subnet.prefix_len != soc_sand_subnet_cos_info_iter.prefix_len) ||
               (soc_sand_subnet.ip_address != soc_sand_subnet_cos_info_iter.ip_address))
            {
                /* No match between ipv4 subnet cos and vlan */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("No match between ipv4 subnet cos and vlan")));
            }

            if (soc_ppd_subnet_based_cos_info_iter.tc_is_valid) {
                /* Valid new TC */
                action->priority = soc_ppd_subnet_based_cos_info_iter.tc;
            } else {
                action->priority = BCM_COS_INVALID;
            }
            break;
        }
    }

    if (entry_ndx > DPP_VID_ASSIGN_IPV4_SUBNET_INDEX_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("IP subnet not found.")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_ip_action_delete(
    int unit, 
    bcm_vlan_ip_t *vlan_ip)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 entry_ndx;
    SOC_SAND_PP_IPV4_SUBNET soc_sand_subnet, soc_sand_subnet_vid_info, soc_sand_subnet_lif_cos;
    SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_INFO soc_ppd_subnet_based_info_iter;
    SOC_PPD_LLP_COS_IPV4_SUBNET_INFO llp_cos_info;
          
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    soc_sand_dev_id = (unit);    

    soc_sand_subnet.prefix_len = bcm_ip_mask_length(vlan_ip->mask);
    soc_sand_subnet.ip_address = vlan_ip->ip4;

    for (entry_ndx = 0;
        (entry_ndx <= DPP_VID_ASSIGN_IPV4_SUBNET_INDEX_MAX);
         ++entry_ndx) {

        soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx, 
           &soc_sand_subnet_vid_info, &soc_ppd_subnet_based_info_iter);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if ((soc_sand_subnet.prefix_len == soc_sand_subnet_vid_info.prefix_len) &&
            (soc_sand_subnet.ip_address == soc_sand_subnet_vid_info.ip_address)) {

            if (soc_ppd_subnet_based_info_iter.vid_is_valid) {            
              soc_ppd_subnet_based_info_iter.vid_is_valid = FALSE;
              soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx, 
                  &soc_sand_subnet_vid_info, &soc_ppd_subnet_based_info_iter);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);              
            }

            soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx,
                &soc_sand_subnet_lif_cos, &llp_cos_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            llp_cos_info.tc_is_valid = FALSE;
            soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx, 
                &soc_sand_subnet_lif_cos, &llp_cos_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


            /* Both entries are invalid. Push all entries below up, so that the table
                won't be fragmented */
            {
                while (++entry_ndx < DPP_VID_ASSIGN_IPV4_SUBNET_INDEX_MAX) {
                    soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx, 
                        &soc_sand_subnet_vid_info, &soc_ppd_subnet_based_info_iter);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx,
                        &soc_sand_subnet_lif_cos, &llp_cos_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx-1, 
                        &soc_sand_subnet_vid_info, &soc_ppd_subnet_based_info_iter);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx-1,
                        &soc_sand_subnet_lif_cos, &llp_cos_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    if (!llp_cos_info.tc_is_valid && !soc_ppd_subnet_based_info_iter.vid_is_valid) {
                        /* All the rest are invalid. */
                        break;
                    }
                }

                /* Last entry we reached is now free */
                soc_ppd_subnet_based_info_iter.vid_is_valid = FALSE;
                soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx-1, 
                    &soc_sand_subnet_vid_info, &soc_ppd_subnet_based_info_iter);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                llp_cos_info.tc_is_valid = FALSE;
                soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx-1,
                    &soc_sand_subnet_lif_cos, &llp_cos_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            break;
        }
    }

    if (entry_ndx > DPP_VID_ASSIGN_IPV4_SUBNET_INDEX_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("IP subnet not found.")));
    }

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_ip_action_delete_all(
    int unit)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 entry_ndx;
    SOC_SAND_PP_IPV4_SUBNET soc_sand_subnet;
    SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_INFO soc_ppd_subnet_based_info;
    SOC_PPD_LLP_COS_IPV4_SUBNET_INFO llp_cos_info;
          
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    soc_sand_dev_id = (unit);    

    soc_sand_SAND_PP_IPV4_SUBNET_clear(&soc_sand_subnet);
    SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(&soc_ppd_subnet_based_info);
    SOC_PPD_LLP_COS_IPV4_SUBNET_INFO_clear(&llp_cos_info);

    /* Invalidate all vid and tc assign values */
    for (entry_ndx = 0;
        (entry_ndx <= DPP_VID_ASSIGN_IPV4_SUBNET_INDEX_MAX);
         ++entry_ndx) {

        /* invalidate the vid assign info and the subnet, since the whole entry is invalid */
        soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx, 
            &soc_sand_subnet, &soc_ppd_subnet_based_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_set(soc_sand_dev_id, entry_ndx,
            &soc_sand_subnet, &llp_cos_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_ip_action_traverse(
    int unit, 
    bcm_vlan_ip_action_traverse_cb cb, 
    void *user_data)
{
    int rv;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 entry_ndx;
    SOC_SAND_PP_IPV4_SUBNET soc_sand_subnet;
    SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_INFO soc_ppd_subnet_based_info;
    SOC_PPD_LLP_COS_IPV4_SUBNET_INFO llp_cos_info;
    bcm_vlan_ip_t vlan_ip;
    bcm_vlan_action_set_t action;

    BCMDNX_INIT_FUNC_DEFS;

    rv = BCM_E_NONE;
          
    soc_sand_dev_id = (unit);    

    for (entry_ndx = 0;
        (entry_ndx <= DPP_VID_ASSIGN_IPV4_SUBNET_INDEX_MAX);
         ++entry_ndx) {

        soc_sand_rv = soc_ppd_llp_vid_assign_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx, 
           &soc_sand_subnet, &soc_ppd_subnet_based_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        soc_sand_rv = soc_ppd_llp_cos_ipv4_subnet_based_get(soc_sand_dev_id, entry_ndx,
            &soc_sand_subnet, &llp_cos_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (soc_ppd_subnet_based_info.vid_is_valid || llp_cos_info.tc_is_valid) {

            vlan_ip.mask =
                soc_sand_subnet.prefix_len == 32 ?
                0xffffffff :
                (((1<<soc_sand_subnet.prefix_len)-1)<<(32-soc_sand_subnet.prefix_len));
            vlan_ip.ip4 = soc_sand_subnet.ip_address;

            if (soc_ppd_subnet_based_info.vid_is_valid) {
                action.ut_outer = bcmVlanActionAdd;
                action.new_outer_vlan = soc_ppd_subnet_based_info.vid;
            }

            if (llp_cos_info.tc_is_valid) {
                action.priority = llp_cos_info.tc;
            } else {
                action.priority = BCM_COS_INVALID;
            }
            
            rv = cb(unit, &vlan_ip, &action, user_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_control_get(
    int unit, 
    bcm_vlan_control_t type, 
    int *arg)
{
    bcm_error_t rv;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    bcm_port_config_t port_config;
    bcm_port_t port_i;

    
    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;
    
    soc_sand_dev_id = (unit);
    
    switch (type)
    {
    case bcmVlanMemberMismatchToCpu:
    case bcmVlanMemberMismatchLearn:
    {
        SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO profile_info;
        
        SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);
        
        soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(soc_sand_dev_id, SOC_PPD_TRAP_CODE_PORT_NOT_VLAN_MEMBER, &profile_info); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        switch (type)
        {
        case bcmVlanMemberMismatchToCpu:
            /* To CPU check */
            rv = bcm_petra_port_config_get(unit, &port_config);
            BCMDNX_IF_ERR_EXIT(rv);
            
            *arg = 0;
            if (profile_info.dest_info.frwrd_dest.type == SOC_PPD_FRWRD_DECISION_TYPE_UC_PORT) {
                SOC_TMC_DEST_INFO soc_petra_dest_info;
                PBMP_ITER(port_config.cpu, port_i) {

                  rv = _bcm_dpp_gport_to_tm_dest_info(unit, port_i, &soc_petra_dest_info);
                  BCMDNX_IF_ERR_EXIT(rv);

                  /* Check destination is CPU */
                  if (profile_info.dest_info.frwrd_dest.dest_id == soc_petra_dest_info.id) {
                      /* Match CPU port */
                      *arg = 1;
                  }
                }
            }
            break;
            
        case bcmVlanMemberMismatchLearn:            
            *arg = SOC_SAND_BOOL2NUM(profile_info.processing_info.enable_learning);          
            break;
        /* We mast the default - without the default - compilation error */
        /* coverity[dead_error_begin : FALSE] */
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("invalid switch case")));
            
        }
    }
    break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Type must be bcmVlanMemberMismatchToCpu or bcmVlanMemberMismatchLearn")));
    }
        
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_control_set(
    int unit, 
    bcm_vlan_control_t type, 
    int arg)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv;
    bcm_port_config_t port_config;
    bcm_port_t port;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    soc_sand_dev_id = (unit);
    
    switch (type)
    {        
    
    case bcmVlanMemberMismatchToCpu:
    case bcmVlanMemberMismatchLearn:
    {
        SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO profile_info;
        
        SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);
             
              
        soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(soc_sand_dev_id, SOC_PPD_TRAP_CODE_PORT_NOT_VLAN_MEMBER, &profile_info); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        profile_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
        
        switch (type)
        {
        case bcmVlanMemberMismatchToCpu:
            /* Set default vsi forwarding action to be CPU or MC */
            profile_info.bitmap_mask = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
            
            if (arg) {
                /* Set destination as CPU */
                SOC_TMC_DEST_INFO soc_petra_dest_info;

                rv = bcm_petra_port_config_get(unit, &port_config);
                BCMDNX_IF_ERR_EXIT(rv);

                DPP_PBMP_SINGLE_PORT_GET(port_config.cpu, port);

                rv = _bcm_dpp_gport_to_tm_dest_info(unit, port, &soc_petra_dest_info);
                BCMDNX_IF_ERR_EXIT(rv);

               /* No need to validate the dest_type since it's the CPU port */

                profile_info.dest_info.frwrd_dest.type = SOC_PPD_FRWRD_DECISION_TYPE_UC_PORT;
                profile_info.dest_info.frwrd_dest.dest_id = soc_petra_dest_info.id;
                profile_info.dest_info.add_vsi = FALSE;
            } else {
                /* Set destination as MC group 0 + VSI */
                profile_info.dest_info.frwrd_dest.type = SOC_PPD_FRWRD_DECISION_TYPE_MC;
                profile_info.dest_info.frwrd_dest.dest_id = 0;
                profile_info.dest_info.add_vsi = TRUE;
            }
            break;            
        case bcmVlanMemberMismatchLearn:
        {
            /* Enable / Disbable learning */
            profile_info.bitmap_mask = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_NONE;
            profile_info.processing_info.enable_learning = SOC_SAND_NUM2BOOL(arg);
            break;
        }
        /* We mast the default - without the default - compilation error */
        /* coverity[dead_error_begin : FALSE] */
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("invalid switch case")));            
        }

        soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id, SOC_PPD_TRAP_CODE_PORT_NOT_VLAN_MEMBER, &profile_info); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    }  
    break;    
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Type must be bcmVlanMemberMismatchToCpu bcmVlanMemberMismatchLearn.")));
    }
        
    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_control_port_get(
    int unit, 
    int port, 
    bcm_vlan_control_port_t type, 
    int *arg)
{
    int          rv;
    unsigned int soc_sand_dev_id;
    uint32       soc_sand_rv;
    
    SOC_PPD_LLP_VID_ASSIGN_PORT_INFO          port_vid_assign_info;
    SOC_PPD_LLP_PARSE_INFO                    parse_key;
    SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO      format_info;
    SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO  mapping_table_entry_info; 
    SOC_PPD_PORT_INFO                         port_info;
    SOC_PPD_PORT                              soc_ppd_port;
    SOC_PPD_L2_LIF_AC_KEY                     ac_key;
    SOC_PPD_LIF_ID                            lif_id, default_lif;
    SOC_PPD_L2_LIF_AC_INFO                    ac_info;
    uint8                                     found;
    int                                       value;
    int                                       is_pon_port, core;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPD_LLP_PARSE_INFO_clear(&parse_key);
    SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_clear(&format_info);
    SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&mapping_table_entry_info);
    SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);
    SOC_PPD_L2_LIF_AC_INFO_clear(&ac_info);
          
    rv = BCM_E_NONE;
    soc_sand_dev_id = (unit);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

    switch (type) {
    case bcmVlanLookupMACEnable:
        soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_vid_assign_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        *arg = SOC_SAND_BOOL2NUM(port_vid_assign_info.enable_sa_based);              
        break;

    case bcmVlanLookupIPEnable:
        soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_vid_assign_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        *arg = SOC_SAND_BOOL2NUM(port_vid_assign_info.enable_subnet_ip);        
        break;

    case bcmVlanPortIgnorePktTag:
        soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_vid_assign_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        *arg = SOC_SAND_BOOL2NUM(port_vid_assign_info.ignore_incoming_tag);
        break;

    case bcmVlanPortPriTaggedDrop:
      {
          int mode;
          /* get the port's previous mode */
          rv = _bcm_petra_port_discard_extend_get(unit, port, &mode);
          BCMDNX_IF_ERR_EXIT(rv);

          *arg = (_BCM_DPP_PORT_DISCARD_MODE_IS_OUTER_PRIO(mode)) ? 1:0;          
      }
      break;

    case bcmVlanPortTranslateKeyFirst:
      {          
          _bcm_petra_dtag_mode_t dtag_mode;

          rv = _bcm_petra_port_discard_extend_dtag_mode_get(unit,port,&dtag_mode);
          BCMDNX_IF_ERR_EXIT(rv);

          *arg = (dtag_mode == _bcm_petra_dtag_mode_accept) ? 1:0;            
      }
      break;

    case bcmVlanPortDoubleLookupEnable:
    {
        soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (IS_PON_PORT(unit,port)) {
            *arg = SOC_SAND_BOOL2NUM(port_info.enable_pon_double_lookup);
        }
        else {   
            if (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID ||
                port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID) {
                *arg = (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID) ? 1:0;
            }
            else if (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_DEFAULT ||
                     port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY) {
                *arg = (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY) ? 1:0;
            }
            else if (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_USE_INITIAL_VID ||
                     port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_DOUBLE_TAG_PRIORITY_USE_INITIAL_VID) {
                if (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_DOUBLE_TAG_PRIORITY_USE_INITIAL_VID &&
                    SOC_DPP_CONFIG(unit)->pp.mpls_databases[0] != 0) { /* MPLS (and FRR) on Double lookup avaiable in case MPLS is in ISEM-A only */
                   BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("FRR and Double lookup is not enabled in case MPLS Database is in ISEM-B"))); 
                }
                *arg = (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_DOUBLE_TAG_PRIORITY_USE_INITIAL_VID) ? 1:0;
            }
            else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("current VT profile isn't default, double tag priority, initial vid or double tag priority initial vid")));
            }
        }
    }
    break;

    case bcmVlanPortLookupTunnelEnable:
    {
        soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *arg = (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_PON_TUNNEL_ID) ? 1:0;
    }
    break;

    case bcmVlanPortIgnoreInnerPktTag:
    {
      if (!SOC_DPP_CONFIG(unit)->pp.custom_feature_vt_tst1) {          
          rv = _bcm_dpp_port_is_pon_port(unit, port, &is_pon_port); 
          BCMDNX_IF_ERR_EXIT(rv);
              
          if (!is_pon_port) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("API is only support in PON port, and PON port should be 0~7")));
          }
      }
      soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      *arg = (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG) ? 1:0;      
    }
    break;
    
    case bcmVlanTranslateIngressMissDrop:
    {                
        ac_key.key_type = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT;
        ac_key.vlan_domain = soc_ppd_port; /* In case of MATCH_PORT , vlan_domain equals ppd port index */

        soc_sand_rv = soc_ppd_l2_lif_ac_get(soc_sand_dev_id, &ac_key, &lif_id, &ac_info,&found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port do not have default LIF.")));
        }

        default_lif = SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple;

        if (MIM_IS_INIT(unit))
        {
            /* check that port is PBP enabled */
            rv = bcm_petra_port_control_get(
                unit,
                port,
                bcmPortControlMacInMac,
                &value);
            BCMDNX_IF_ERR_EXIT(rv);
            if (value) {
                default_lif = SOC_DPP_CONFIG(unit)->pp.mim_lif_ndx;
            }
        }

        /* Check if lif id is one of the default LIFs */
        if ( (lif_id != SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop) &&
             (lif_id != default_lif)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port default LIF is not one of the defaults, API is not supported in this case")));
        }

        *arg = (lif_id == SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop) ? 1:0;        
    }
    break;

    case bcmVlanPortPreferIP4:
        {
        soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *arg = (port_info.vlan_translation_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_FLEXIBLE_Q_IN_Q) ? 1:0;
        }
    break;

    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Type not supported.")));
    }
  
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_control_port_set(
    int unit, 
    int port, 
    bcm_vlan_control_port_t type, 
    int arg)
{
    int          rv;
    uint32       soc_sand_rv;
    
    SOC_PPD_LLP_VID_ASSIGN_PORT_INFO          port_vid_assign_info;
    SOC_PPD_LLP_PARSE_INFO                    parse_key;
    SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO      format_info;
    SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO  mapping_table_entry_info; 
    SOC_PPD_PORT_INFO                         port_info;
    SOC_PPD_LLP_COS_PORT_INFO                 cos_port_info;
    SOC_PPD_L2_LIF_AC_KEY                     ac_key;
    SOC_PPD_LIF_ENTRY_INFO                    lif_info;
    SOC_SAND_SUCCESS_FAILURE                  soc_sand_success;
    SOC_PPD_LIF_ID                            lif_id, default_lif;
    uint8                                     found;
    SOC_PPD_PORT soc_ppd_port;
    int mode;
    int value;
    _bcm_petra_dtag_mode_t dtag_mode;
    int is_pon_port, core;
    _bcm_dpp_gport_info_t gport_info;
    bcm_pbmp_t pbmp_local_ports;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    SOC_PPD_LLP_PARSE_INFO_clear(&parse_key);
    SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_clear(&format_info);
    SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&mapping_table_entry_info);
    SOC_PPD_PORT_INFO_clear(&port_info);
    SOC_PPD_LLP_COS_PORT_INFO_clear(&cos_port_info);
    SOC_PPD_L2_LIF_AC_KEY_clear(&ac_key);
    SOC_PPD_LIF_ENTRY_INFO_clear(&lif_info);
       
    rv = BCM_E_NONE;

    if (port == -1) {
        /* Device call all ethernet ports */
        pbmp_local_ports = PBMP_E_ALL(unit);
    } else {
        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ASSIGN(pbmp_local_ports, gport_info.pbmp_local_ports);
    }

    PBMP_ITER(pbmp_local_ports,port) {

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));
        
        switch (type) {
        case bcmVlanLookupMACEnable:
            soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(unit, soc_ppd_port, &port_vid_assign_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            port_vid_assign_info.enable_sa_based = SOC_SAND_NUM2BOOL(arg);
            
            soc_sand_rv = soc_ppd_llp_vid_assign_port_info_set(unit, soc_ppd_port, &port_vid_assign_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
 
            /* To enable initial-VID on port use soc property vlan_translation_initial_vlan_enable. 
             * In case it is not set, MAC enable apply to untagged packets only.
             */
            break;

        case bcmVlanLookupIPEnable: /* Enable Both VLAN and TC resultion for subnet */
            soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(unit, soc_ppd_port, &port_vid_assign_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            port_vid_assign_info.enable_subnet_ip = SOC_SAND_NUM2BOOL(arg);
            
            soc_sand_rv = soc_ppd_llp_vid_assign_port_info_set(unit, soc_ppd_port, &port_vid_assign_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit, soc_ppd_port, &cos_port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            cos_port_info.l3_info.use_ip_subnet = SOC_SAND_NUM2BOOL(arg);

            soc_sand_rv = soc_ppd_llp_cos_port_info_set(unit, soc_ppd_port, &cos_port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            break;

        case bcmVlanPortIgnorePktTag:
            soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(unit, soc_ppd_port, &port_vid_assign_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            port_vid_assign_info.ignore_incoming_tag = SOC_SAND_NUM2BOOL(arg);
            
            soc_sand_rv = soc_ppd_llp_vid_assign_port_info_set(unit, soc_ppd_port, &port_vid_assign_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            break;

        case bcmVlanPortPriTaggedDrop:
          {
              if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("In AVT mode there is no support for bcmVlanPortPriTaggedDrop ")));
              }

              /* get the port's previous mode */
              rv = _bcm_petra_port_discard_extend_get(unit, port, &mode);
              BCMDNX_IF_ERR_EXIT(rv);

              /* set the Outer prio bit, to change the mode into OuterPrio mode */
              if (arg) {
                _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO_SET(mode);
              } else {
                _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO_REMOVE(mode);
              }

              /* call this function instead of the API, so OuterPrio mode (>4) can be used */
              rv = _bcm_petra_port_discard_extend_mode_set(unit, port, mode);
              BCMDNX_IF_ERR_EXIT(rv);
          }
          break;
        case bcmVlanPortTranslateKeyFirst:
          {
              if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("In AVT mode there is no support for bcmVlanPortTranslateKeyFirst ")));
              }

              /* Enable / Disable Double tag. I.e. in case user set PortOuter lookup will ignore double tagged (sequence as one tag) */
              dtag_mode = (arg == bcmVlanTranslateKeyPortOuter) ? _bcm_petra_dtag_mode_ignore:_bcm_petra_dtag_mode_accept;

              rv = _bcm_petra_port_discard_extend_dtag_mode_set(unit, port, dtag_mode);
              BCMDNX_IF_ERR_EXIT(rv);
              
                            
          }       
          break;

          case bcmVlanPortDoubleLookupEnable:
          {
              soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

              if (IS_PON_PORT(unit,port)) {
                  port_info.enable_pon_double_lookup = SOC_SAND_NUM2BOOL(arg);
                  port_info.vlan_translation_profile = SOC_PPD_PORT_DEFINED_VT_PROFILE_PON_DEFAULT;
              }
              else {
                  if (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID ||
                      port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID) {
                      port_info.vlan_translation_profile = (arg == 0) ? SOC_PPD_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID:SOC_PPD_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID;
                  }
                  else if (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_DEFAULT ||
                           port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY) {
                      port_info.vlan_translation_profile = (arg == 0) ? SOC_PPD_PORT_DEFINED_VT_PROFILE_DEFAULT:SOC_PPD_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY;
                  }
                  else if (port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_USE_INITIAL_VID ||
                           port_info.vlan_translation_profile == SOC_PPD_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_DOUBLE_TAG_PRIORITY_USE_INITIAL_VID) {
                      port_info.vlan_translation_profile = (arg == 0) ? SOC_PPD_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_USE_INITIAL_VID:SOC_PPD_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_DOUBLE_TAG_PRIORITY_USE_INITIAL_VID;
                  }
                  else {
                      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("current VT profile isn't default, double tag priority, initial vid or double tag priority initial vid")));
                  }
              }

              soc_sand_rv = soc_ppd_port_info_set(unit, soc_ppd_port, &port_info);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          }
          break;

        case bcmVlanPortLookupTunnelEnable:
        {
            soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            port_info.vlan_translation_profile = (arg == 0) ? SOC_PPD_PORT_DEFINED_VT_PROFILE_PON_DEFAULT:SOC_PPD_PORT_DEFINED_VT_PROFILE_PON_TUNNEL_ID;
            
            soc_sand_rv = soc_ppd_port_info_set(unit, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
        
        case bcmVlanPortIgnoreInnerPktTag:
        {
            if (!SOC_DPP_CONFIG(unit)->pp.custom_feature_vt_tst1) {   
              rv = _bcm_dpp_port_is_pon_port(unit, port, &is_pon_port);
              BCMDNX_IF_ERR_EXIT(rv);

              if (!is_pon_port) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("API is only support in PON port, and PON port should be 0~7")));
              }
            }

            soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            if (arg == 0 && (SOC_DPP_CONFIG(unit)->pp.custom_feature_vt_tst1)) {
                port_info.vlan_translation_profile = SOC_PPD_PORT_DEFINED_VT_PROFILE_DEFAULT; 
            } else if (arg == 0) {              
                port_info.vlan_translation_profile = SOC_PPD_PORT_DEFINED_VT_PROFILE_PON_DEFAULT; 
            } else {
                port_info.vlan_translation_profile = SOC_PPD_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG;
            }
            
            soc_sand_rv = soc_ppd_port_info_set(unit, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;

        case bcmVlanTranslateIngressMissDrop:
        {                
            ac_key.key_type = SOC_PPD_L2_LIF_AC_MAP_KEY_TYPE_PORT;
            ac_key.vlan_domain = soc_ppd_port; /* In case of MATCH_PORT , vlan_domain equals ppd port index */

            soc_sand_rv = soc_ppd_l2_lif_ac_get(unit, &ac_key, &lif_id, &(lif_info.value.ac), &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Each port must have a valid LIF")));
            }

            default_lif = SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple;

            if (MIM_IS_INIT(unit))
            {
                /* check that port is PBP enabled */
                rv = bcm_petra_port_control_get(
                    unit,
                    port,
                    bcmPortControlMacInMac,
                    &value);
                BCMDNX_IF_ERR_EXIT(rv);
                if (value) {
                    default_lif = SOC_DPP_CONFIG(unit)->pp.mim_lif_ndx;
                }
            }

            /* Check if lif id is one of the default LIFs */
            if ( (lif_id != SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop) &&
                 (lif_id != default_lif)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port default LIF is not one of the defaults, API is not supported in this case")));
            }

            if (arg && SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop == SOC_PPC_AC_ID_INVALID) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Set Drop cant be done since soc property lif index drop is invalid")));
            }

            lif_id = (arg) ? (SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop):(default_lif);

            soc_sand_rv = soc_ppd_lif_table_entry_get(unit, lif_id, &lif_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);           

            if (lif_info.type != SOC_PPC_LIF_ENTRY_TYPE_AC &&  lif_info.type != SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP &&
                lif_info.type != SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("drop lif must be of type AC")));
            }
            soc_sand_rv = soc_ppd_l2_lif_ac_add(unit, &ac_key, lif_id, &lif_info.value.ac,&soc_sand_success);
            BCMDNX_IF_ERR_EXIT(translate_sand_success_failure(soc_sand_success));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;

        case bcmVlanPortPreferIP4:
        {
            soc_sand_rv = soc_ppd_port_info_get(unit, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            port_info.vlan_translation_profile = (arg == 0) ? SOC_PPD_PORT_DEFINED_VT_PROFILE_DEFAULT:SOC_PPD_PORT_DEFINED_VT_PROFILE_FLEXIBLE_Q_IN_Q;

            soc_sand_rv = soc_ppd_port_info_set(unit, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;

        default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Type not supported.")));
            break;
        }

    }
    
    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_mcast_flood_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_mcast_flood_t *mode)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API not supported."))); 
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_mcast_flood_set(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_mcast_flood_t mode)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API not supported."))); 
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_control_vlan_get(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_control_vlan_t *control)
{
    SOC_PPD_VSI_ID vsi;
    SOC_PPD_VSI_INFO vsi_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv, fid_profile;
    int fid_aging_cycles;
    int rv;
    SOC_PPD_FID fid;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    vsi = vlan;

    /* check if this is a normal vlan or a MP VSI */
    rv = _bcm_dpp_vlan_check(unit, vlan);
    BCMDNX_IF_ERR_EXIT(rv);
        
    soc_sand_dev_id = (unit);

    bcm_vlan_control_vlan_t_init(control);   

    soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    fid_profile = vsi_info.fid_profile_id;
    if (fid_profile == SOC_PPD_VSI_FID_IS_VSID) {   
        control->forwarding_vlan = vlan;        
    } else {        
        SOC_SAND_IF_ERR_RETURN(soc_ppd_frwrd_mact_fid_profile_to_fid_map_get(
                        soc_sand_dev_id, fid_profile, &fid));
        control->forwarding_vlan = fid;
    }

    /*
     * Control default forward {
     */
    if (SOC_IS_PETRAB(unit)) {    
        if (vsi_info.default_forward_profile == DPP_VSI_DEFAULT_FRWRD_PROFILE) {
            control->unknown_unicast_group   = vsi;
            control->unknown_multicast_group = vsi;
            control->broadcast_group         = vsi;
        } else if (vsi_info.default_forward_profile == DPP_VSI_DEFAULT_FRWRD_PROFILE_UC_0_MC_4K_UC_8K) {
            control->unknown_unicast_group   = vsi;
            control->unknown_multicast_group = 4096 + vsi;
            control->broadcast_group         = 8192 + vsi;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unexpected vsi_info.default_forward_profile: only 0/1 are supported")));
        }
    } else if (SOC_IS_ARAD(unit)) {
        /* API only get Default Forward of VSI i.e. Unknown UC */
        /* In case of Trill flooding port dest_id is FEC/ECMP and no MC-ID*/
        if (!(SOC_DPP_CONFIG(unit)->trill.mc_id) && (vsi_info.default_forwarding.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC)) {
           _BCM_MULTICAST_GROUP_SET(control->unknown_unicast_group, _BCM_MULTICAST_TYPE_SUBPORT, vsi_info.default_forwarding.dest_id); 
           _BCM_MULTICAST_GROUP_SET(control->unknown_multicast_group, _BCM_MULTICAST_TYPE_SUBPORT, vsi_info.default_forwarding.dest_id);
           _BCM_MULTICAST_GROUP_SET(control->broadcast_group, _BCM_MULTICAST_TYPE_SUBPORT, vsi_info.default_forwarding.dest_id);
        }else{ /* old implementation trill flooding port dest_id is MC_ID*/
           _BCM_MULTICAST_GROUP_SET(control->unknown_unicast_group, _BCM_MULTICAST_TYPE_L2, vsi_info.default_forwarding.dest_id); 
           _BCM_MULTICAST_GROUP_SET(control->unknown_multicast_group, _BCM_MULTICAST_TYPE_L2, vsi_info.default_forwarding.dest_id);
           _BCM_MULTICAST_GROUP_SET(control->broadcast_group, _BCM_MULTICAST_TYPE_L2, vsi_info.default_forwarding.dest_id);
        }
        
        /* Get the VSI profile */
        control->if_class = (vsi_info.profile_ingress >> DPP_VSI_PROFILE_PMF_LSB);
    }
    /* Get aging cycles */
    rv = _bcm_dpp_am_template_fid_aging_profile_data_get(unit, vsi, &fid_aging_cycles);
    BCMDNX_IF_ERR_EXIT(rv);

    control->aging_cycles = fid_aging_cycles;
    /*
     * Control default forward }
     */

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_vlan_flooding_per_lif_hw_profile_set(int unit,
                                                int port_profile, 
                                                int lif_profile,                                               
                                                SOC_PPD_L2_LIF_DEFAULT_FRWRD_ACTION frwrd_action[SOC_SAND_PP_NOF_ETHERNET_DA_TYPES])
{    
    uint32 soc_sand_rv, soc_sand_dev_id;
    SOC_PPD_L2_LIF_DEFAULT_FRWRD_KEY default_frwrd_key;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    SOC_PPD_L2_LIF_DEFAULT_FRWRD_KEY_clear(&default_frwrd_key);

    default_frwrd_key.lif_default_forward_profile = lif_profile;    
    
    /* 
     *  In case of lif flooding. HW set is for both port profile 0 & 1.
     *  In case of port flooding. HW set only for port profile 1.
     */
    for (default_frwrd_key.port_da_not_found_profile = port_profile; default_frwrd_key.port_da_not_found_profile <  DPP_VLAN_DEFAULT_FRWRD_NOF_PORT_PROFILES; ++default_frwrd_key.port_da_not_found_profile) {
        /* Set all different DA types (UC/MC/BC) */
        for (default_frwrd_key.da_type = 0; default_frwrd_key.da_type < SOC_SAND_PP_NOF_ETHERNET_DA_TYPES; ++default_frwrd_key.da_type) {            
            /* Call HW */
            soc_sand_rv = soc_ppd_l2_lif_default_frwrd_info_set(soc_sand_dev_id,&default_frwrd_key,&frwrd_action[default_frwrd_key.da_type]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        }                      
    }

    BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}





int
_bcm_petra_vlan_flooding_per_lif_set(int unit, 
                                     bcm_port_t port,
                                     SOC_PPD_LIF_ID lif_index,
                                     int is_port_flooding, 
                                     int is_lif_flooding,
                                     bcm_gport_t unknown_unicast_group, 
                                     bcm_gport_t unknown_multicast_group,
                                     bcm_gport_t broadcast_group)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv, soc_sand_dev_id;
    bcm_dpp_vlan_flooding_profile_info_t flooding_info;
    int old_template = 0, new_template = 0;
    int is_last = 0, is_allocated = 0;
    uint32 port_profile = 0, lif_profile = 0;    
    SOC_PPD_L2_LIF_DEFAULT_FRWRD_ACTION *default_frwrd_action = NULL;
    bcm_gport_t trrafic_type_dflt_dest[SOC_SAND_PP_NOF_ETHERNET_DA_TYPES];
    SOC_SAND_PP_ETHERNET_DA_TYPE da_type;
    SOC_PPD_LIF_ENTRY_INFO *lif_entry_info = NULL;
    
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);
    trrafic_type_dflt_dest[SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC] = unknown_multicast_group;
    trrafic_type_dflt_dest[SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC] = broadcast_group;
    trrafic_type_dflt_dest[SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC] = unknown_unicast_group;

    /*if (unknown_unicast_group > 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unexpected params: unknown UC addition must be equal to 0")));
    }*/

    /*if (unknown_unicast_group > unknown_multicast_group || unknown_unicast_group > broadcast_group) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unexpected params: unknown UC addition smaller than multicast addition and broadcast addition")));
    }*/

    if (is_lif_flooding && is_port_flooding) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unexpected params: Flooding can be set either by port or lif - not both")));
    }

    /*flooding_info.unknown_uc_add = 0;*/
    flooding_info.unknown_uc_add = unknown_unicast_group;
    flooding_info.unknown_mc_add = unknown_multicast_group;
    flooding_info.bc_add = broadcast_group;

    /* Decide port_profile, lif_profile according to input prameters */
    if (is_port_flooding) {
        if (flooding_info.unknown_uc_add == flooding_info.unknown_mc_add &&
            flooding_info.unknown_mc_add == flooding_info.bc_add) {
          /* Default profile - port profile is 0. LIF profile is 0. */
          port_profile = 0;
        } else {
          port_profile = 1;
          /* Need to allocate this profile information */
          is_allocated = 1;
        }

        /* In that case. LIF profile is always 0. */
        lif_profile = 0;         
    }

    if (is_lif_flooding) {
        rv = _bcm_dpp_am_template_l2_flooding_exchange(unit,lif_index,&flooding_info,&old_template,&is_last,&new_template,&is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        lif_profile = new_template;
    }

    if (is_allocated) {   
        BCMDNX_ALLOC(default_frwrd_action, sizeof(SOC_PPD_L2_LIF_DEFAULT_FRWRD_ACTION)* SOC_SAND_PP_NOF_ETHERNET_DA_TYPES, "_bcm_petra_vlan_flooding_per_lif_set.default_frwrd_action");
        if (!default_frwrd_action) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }

        for (da_type = 0; da_type < SOC_SAND_PP_NOF_ETHERNET_DA_TYPES; ++da_type) {
            SOC_PPD_L2_LIF_DEFAULT_FRWRD_ACTION_clear(&(default_frwrd_action[da_type]));
            /* Fill action info */

            /* assuming
               - only MC and drop is supported. 
               - if type not set then it MC (backward comp) 
             */
            if(BCM_GPORT_IS_BLACK_HOLE(trrafic_type_dflt_dest[da_type])) {
                default_frwrd_action[da_type].add_vsi = FALSE;/* set to drop, no offset*/
                default_frwrd_action[da_type].offset = 0; /* no offset*/
                SOC_PPD_FRWRD_DECISION_DROP_SET(soc_sand_dev_id, &default_frwrd_action[da_type].frwrd_dest, soc_sand_rv);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                
            }
            else{
                default_frwrd_action[da_type].add_vsi = TRUE; /* add offset to MC*/
                default_frwrd_action[da_type].offset = _BCM_MULTICAST_ID_GET(trrafic_type_dflt_dest[da_type]);
                /* if add_vsi == TRUE then offset is used instead of frwrd_dest
                   the destination will be the given offset + VSI */

                /* these are set just to pass the PPD verify, they will not be used */
                default_frwrd_action[da_type].frwrd_dest.type = SOC_PPD_FRWRD_DECISION_TYPE_TRAP;
                default_frwrd_action[da_type].frwrd_dest.dest_id = 0;
                default_frwrd_action[da_type].frwrd_dest.additional_info.trap_info.action_profile.frwrd_action_strength = 0;
                default_frwrd_action[da_type].frwrd_dest.additional_info.trap_info.action_profile.snoop_action_strength = 0;
            }

        }

        rv = _bcm_petra_vlan_flooding_per_lif_hw_profile_set(unit, port_profile, lif_profile, default_frwrd_action);
        BCMDNX_IF_ERR_EXIT(rv);                
    }

    if (is_port_flooding || (old_template != new_template)) {
        /* Set new profile mapping */

        /* Port case */
        if (is_port_flooding) {
            SOC_PPD_PORT_INFO port_info;
            int port_i, core;
            SOC_PPD_PORT soc_ppd_port_i;
            _bcm_dpp_gport_info_t gport_info;

            SOC_PPD_PORT_INFO_clear(&port_info);

            /* Retrive local PP ports */
            rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
            BCMDNX_IF_ERR_EXIT(rv);

            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        
              BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
              soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id,soc_ppd_port_i,&port_info);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
              
              port_info.da_not_found_profile = port_profile;

              soc_sand_rv = soc_ppd_port_info_set(soc_sand_dev_id,soc_ppd_port_i,&port_info);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
        
        /* LIF case */
        if (is_lif_flooding) {        
          BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPD_LIF_ENTRY_INFO), "_bcm_petra_vlan_flooding_per_lif_set.lif_entry_info");
          if (lif_entry_info == NULL) {        
              BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
          }

          SOC_PPD_LIF_ENTRY_INFO_clear(lif_entry_info);
          /* Retreive LIF default profile */
          soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, lif_index, lif_entry_info);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          if(lif_entry_info->type != SOC_PPD_LIF_ENTRY_TYPE_AC && lif_entry_info->type != SOC_PPD_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP && lif_entry_info->type != SOC_PPD_LIF_ENTRY_TYPE_PWE) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("expected to be AC,PWE type")));
          }

          if (lif_entry_info->type == SOC_PPD_LIF_ENTRY_TYPE_PWE) {
              lif_entry_info->value.pwe.default_forward_profile = new_template;
          } else {
              lif_entry_info->value.ac.default_forward_profile = new_template;
          }
          
          soc_sand_rv = soc_ppd_lif_table_entry_update(soc_sand_dev_id, lif_index, lif_entry_info);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

   BCMDNX_IF_ERR_EXIT(rv);
exit:
   BCM_FREE(lif_entry_info);
   BCM_FREE(default_frwrd_action);
   BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_vlan_flooding_per_lif_get(int unit, 
                                     bcm_port_t port,
                                     SOC_PPD_LIF_ID lif_index, 
                                     int is_port_flooding,
                                     int is_lif_flooding,
                                     bcm_gport_t *unknown_unicast_group, 
                                     bcm_gport_t *unknown_multicast_group,
                                     bcm_gport_t *broadcast_group)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    bcm_dpp_vlan_flooding_profile_info_t flooding_info;
    uint32 port_profile, lif_profile;
    SOC_PPD_L2_LIF_DEFAULT_FRWRD_KEY default_frwrd_key;
    SOC_PPD_L2_LIF_DEFAULT_FRWRD_ACTION default_frwrd_action;
    SOC_PPD_PORT_INFO port_info;
    SOC_PPD_PORT soc_ppd_port_i;
    int temp, core;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPD_L2_LIF_DEFAULT_FRWRD_KEY_clear(&default_frwrd_key);
    SOC_PPD_L2_LIF_DEFAULT_FRWRD_ACTION_clear(&default_frwrd_action);

    if (is_lif_flooding && is_port_flooding) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unexpected params: Flooding can be set either by port or lif - not both")));
    }

    /* Per LIF flooding is easy, just get from SW DB */
    if (is_lif_flooding) {
        rv = _bcm_dpp_am_template_l2_flooding_data_get(unit, lif_index, &flooding_info);
        BCMDNX_IF_ERR_EXIT(rv);

        *unknown_unicast_group = flooding_info.unknown_uc_add;
        *unknown_multicast_group = flooding_info.unknown_mc_add;
        *broadcast_group = flooding_info.bc_add;

        BCM_EXIT;
    }

    /* Per Port flooding { */
    /* In that case. LIF profile is always 0. */
    lif_profile = 0;

    /* Retreive port profile */   
    SOC_PPD_PORT_INFO_clear(&port_info);

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port_i, &core)));
    soc_sand_rv = soc_ppd_port_info_get(unit,soc_ppd_port_i,&port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      
    port_profile = port_info.da_not_found_profile;
    
    if (port_profile == 0) {
        /* Default profile */
        *unknown_unicast_group = 0;
        *unknown_multicast_group = 0;
        *broadcast_group = 0;
    } else {
        /* Retreive from HW */
        default_frwrd_key.lif_default_forward_profile = lif_profile;
        default_frwrd_key.port_da_not_found_profile = port_profile;

        /* Get all different DA types */
        for (default_frwrd_key.da_type = 0; default_frwrd_key.da_type < SOC_SAND_PP_NOF_ETHERNET_DA_TYPES; ++default_frwrd_key.da_type) {
            /* Skip in case of given NULL pointer */
            switch (default_frwrd_key.da_type) {
            case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC:
              if (unknown_unicast_group == NULL) {
                continue;
              }
              break;
            case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC:
              if (unknown_multicast_group == NULL) {
                continue;
              }
              break;
            case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC:
              if (broadcast_group == NULL) {
                continue;
              }
              break;
            default:
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("da type unknown")));
              break;                
            }

            /* Call HW */
            soc_sand_rv = soc_ppd_l2_lif_default_frwrd_info_get(unit,&default_frwrd_key,&default_frwrd_action);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);        

            if (default_frwrd_action.frwrd_dest.type != SOC_PPC_FRWRD_DECISION_TYPE_DROP &&
                (!default_frwrd_action.add_vsi)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("forward action profile is not initialized")));
            }

            temp = (default_frwrd_action.add_vsi) ? default_frwrd_action.offset:BCM_GPORT_BLACK_HOLE;

            /* Fill action info */
            switch (default_frwrd_key.da_type) {
            case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC:
              *unknown_unicast_group = temp;
              break;
            case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC:
              *unknown_multicast_group = temp;
              break;
            case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC:
              *broadcast_group = temp;
              break;
            default:
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("da type unknown")));
              break;                
            }
        }                                     
    }
   /* Per Port flooding } */

   BCMDNX_IF_ERR_EXIT(rv);
exit:
   BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_control_vlan_set(
    int unit, 
    bcm_vlan_t vlan, 
    bcm_vlan_control_vlan_t control)
{
    SOC_PPD_VSI_ID vsi;
    SOC_PPD_VSI_INFO vsi_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv;
    int fid, fid_first_appear=0, fid_last_appear=0, fid_profile=0;
    int uc, mc, bc;

    int fid_aging_cycles;
    int old_learn_profile,old_limit, old_handle_profile, old_aging_profile;
    int is_last, fid_aging_profile_first_appear;
    int learn_profile_first_appear;
    int new_learn_profile,new_aging_profile;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    soc_sand_dev_id = (unit);

    rv = _bcm_petra_vlan_control_vlan_verify(unit, &control);
    BCMDNX_IF_ERR_EXIT(rv);
    
    if (_BCM_DPP_VLAN_IS_BVID(vlan)) { /* B-VID */

        SOC_SAND_PP_VLAN_ID bvid;
        SOC_PPD_BMACT_BVID_INFO bvid_info;

        /* remove 0x1111 from B-VID */
        bvid = _BCM_DPP_BVID_TO_VLAN(vlan);

        BCM_DPP_VLAN_CHK_ID(unit,bvid);

        /* set the fid_profile of the B-VID */
        soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_get(soc_sand_dev_id, bvid, &bvid_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (control.forwarding_vlan == vlan) {
            /* independent VLAN Learning */
            bvid_info.b_fid_profile = SOC_PPD_BFID_EQUAL_TO_BVID;
        }
        else if (control.forwarding_vlan == 0) {
            /* shared VLAN Learning */
            bvid_info.b_fid_profile = SOC_PPD_BFID_IS_0;
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If B-VID, control.forwarding_vlan must be equal to vlan or 0.")));
        }

        soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_set(soc_sand_dev_id, bvid, &bvid_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else {
        
        /* check if this configure VSI* and not normal vlan */
        if(!BCM_VLAN_VALID(vlan)) {
            /* check this is valid MP VSI */
            rv = _bcm_dpp_vlan_check(unit, vlan);
            BCMDNX_IF_ERR_EXIT(rv);

            if(vlan != control.forwarding_vlan) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For VSI, control.forwarding_vlan must be equal to vlan."))); /* for VSI FID = VSI */
            }
        }
        else {
            /* 0 - 4K */
            BCM_DPP_VLAN_CHK_ID(unit,vlan);

            /* Verify vsi=vlan exist */
            rv = _bcm_dpp_vlan_check(unit, vlan);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        
        vsi = vlan;

        
        fid = 0;

        /*
         * control.forwarding_vlan {
         */
        
        soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        if (vsi_info.fid_profile_id != SOC_PPD_VSI_FID_IS_VSID)
        {
            /* Remove current fid from ref count */
            BCMDNX_IF_ERR_EXIT(fid_ref_count_fid_remove(unit, vsi_info.fid_profile_id,
                            &fid_profile, &fid_last_appear));
            
            if (fid_last_appear) {
                /* Last appearance. Remove fro HW */
                BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_mact_fid_profile_to_fid_map_set(
                        soc_sand_dev_id, fid_profile, fid));
            }
        }
              
        if (vlan == control.forwarding_vlan) {
            fid = SOC_PPD_VSI_FID_IS_VSID;
        } else {
            fid = control.forwarding_vlan;
            
            /* fid = fid_as_vsi; */
        }
        
        BCMDNX_IF_ERR_EXIT(fid_ref_count_fid_add(unit, fid, &fid_profile, &fid_first_appear));
        
        if (fid_first_appear) {
            /* First appearance of fid. Add to HW */
            BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_mact_fid_profile_to_fid_map_set(
                    soc_sand_dev_id, fid_profile, fid));
        }
        
        vsi_info.fid_profile_id = fid_profile;    
            
        /*
         * control.forwarding_vlan }
         */
        
        uc = _BCM_MULTICAST_ID_GET(control.unknown_unicast_group);
        mc = _BCM_MULTICAST_ID_GET(control.unknown_multicast_group);
        bc = _BCM_MULTICAST_ID_GET(control.broadcast_group);
        /* 
         * control.unknown {
         */
        if (SOC_IS_PETRAB(unit)) {

            if ((vsi == uc) && (vsi == mc) && (vsi == bc)) {
                /* All multicast groups are the same (and equal vsid). Use default forward profile */
                vsi_info.default_forward_profile = DPP_VSI_DEFAULT_FRWRD_PROFILE;
            } else if ((vsi == uc) && ((vsi + 4096) == mc) && ((vsi + 8192) == bc)) {
                vsi_info.default_forward_profile = DPP_VSI_DEFAULT_FRWRD_PROFILE_UC_0_MC_4K_UC_8K;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unexpected params: unknown UC/MC/BC can be either equal to vsi, or\n\r"
                                                        "(vsi == control.unknown_unicast_group) && (vsi + 4096) == control.unknown_multicast_group) && (vsi + 8192) == control.broadcast_group)")));
            }
        } else if (SOC_IS_ARAD(unit)) {
            /* VSI default forwarding is the same as LIF in ARAD */
            /* Retreieve VSI-lif-index and call flooding internal API */
            if(BCM_VLAN_VALID(vlan)) {
                bcm_gport_t         *trill_gport_p;
                int                 fec_id;

                /* old implementation that use sw db for mapping mc-id to nickname*/
                if (!SOC_DPP_CONFIG(unit)->trill.mc_id)
                {
                    /* mc-id has to be virtual mc-id and is actually FEC/ECMP value*/
                    if(_BCM_MULTICAST_TYPE_GET(control.unknown_multicast_group) == _BCM_MULTICAST_TYPE_SUBPORT)
                    {
                        fec_id = _BCM_MULTICAST_ID_GET(control.unknown_multicast_group);
                        SOC_PPD_FRWRD_DECISION_FEC_SET(soc_sand_dev_id, &(vsi_info.default_forwarding), fec_id, soc_sand_rv);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                    else{
                        if ((mc != uc) || (bc != uc)) {
                               /* When LIF-PER-VSI disable, call portcontrol per seperate LIF to set different flooding for BC/UC */
                               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For BC, MC settings call port control per seperate LIF")));
                        }
                        SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(soc_sand_dev_id, &(vsi_info.default_forwarding), uc, soc_sand_rv);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
                else
                {
                    rv = _bcm_dpp_mc_to_trill_get( unit,
                                                   uc, 
                                                   &trill_gport_p);

                    if ( rv == BCM_E_NONE) {
                         bcm_trill_port_t trill_port;

                         trill_port.trill_port_id = *trill_gport_p;

                         trill_port.name = 0;
                         rv = bcm_petra_trill_port_get(unit, &trill_port) ;   
                         BCMDNX_IF_ERR_EXIT(rv);

                         rv = _bcm_petra_trill_port_fec_id_get(unit, *trill_gport_p, &fec_id);
                         BCMDNX_IF_ERR_EXIT(rv);

                         if (fec_id==-1) {
                             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Fec not found for trill flooding\n\r")));
                         }
                         SOC_PPD_FRWRD_DECISION_FEC_SET(soc_sand_dev_id, &(vsi_info.default_forwarding), fec_id, soc_sand_rv);
                         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                     } else {    
                           if ((mc != uc) || (bc != uc)) {
                               /* When LIF-PER-VSI disable, call portcontrol per seperate LIF to set different flooding for BC/UC */
                               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For BC, MC settings call port control per seperate LIF")));
                        }
                        SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(soc_sand_dev_id, &(vsi_info.default_forwarding), uc, soc_sand_rv);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
                    }
                }
            } else {
                if ((mc != uc) || (bc != uc)) {
                    /* For VSI > 4K call portcontrol per seperate LIF to set different flooding for BC/UC */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For VSI higher than 4K call port control per seperate LIF")));
                }
                SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(soc_sand_dev_id, &(vsi_info.default_forwarding), uc, soc_sand_rv);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            /* Set the VSI profile */
            vsi_info.profile_ingress = ((control.if_class & DPP_VSI_PROFILE_PMF_MASK) << DPP_VSI_PROFILE_PMF_LSB);
        }

        /* 
         * control.unknown }
         */
        rv = _bcm_dpp_am_template_fid_aging_profile_data_get(unit, vsi, &fid_aging_cycles);
        BCMDNX_IF_ERR_EXIT(rv);

        if (control.aging_cycles != fid_aging_cycles) {

            /* get old information attached with given vsi*/
            rv = _bcm_dpp_am_template_l2_learn_profile_data_get(unit, vsi, &old_limit, &old_handle_profile, &old_aging_profile);
            BCMDNX_IF_ERR_EXIT(rv);


            /* Check if fid-aging profile is available*/
            rv = _bcm_dpp_am_template_fid_aging_profile_exchange(unit, TRUE, vsi, control.aging_cycles, &old_aging_profile, &is_last, &new_aging_profile,&fid_aging_profile_first_appear);
            BCMDNX_IF_ERR_EXIT(rv);

            /* allocate fid-profile with new fid-aging-profile, old-limit and old handle event */
            rv = _bcm_dpp_am_template_l2_learn_profile_exchange(unit, FALSE, vsi, old_limit, old_handle_profile, new_aging_profile, &old_learn_profile, &is_last, &new_learn_profile,&learn_profile_first_appear);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Now all resources are available - allocate fid-aging-profile*/
            rv = _bcm_dpp_am_template_fid_aging_profile_exchange(unit, FALSE, vsi, control.aging_cycles, &old_aging_profile, &is_last, &new_aging_profile,&fid_aging_profile_first_appear);
            BCMDNX_IF_ERR_EXIT(rv);

            /* if fid aging profile first appear configure hw */
            if (fid_aging_profile_first_appear) {
                soc_sand_rv = soc_ppd_frwrd_mact_aging_profile_config(soc_sand_dev_id,new_aging_profile,control.aging_cycles);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);            
            }

            /* if first appear of fid-learn-profile then configure profile*/
            if (learn_profile_first_appear) {

                SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO      limit_info;

                /* update fid-learn-profile to fid-aging profile */
                soc_sand_rv = soc_ppd_frwrd_mact_fid_aging_profile_set(unit,new_learn_profile, new_aging_profile);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* update fid-learn-profile to event-handle profile */
                soc_sand_rv = soc_ppd_frwrd_mact_event_handle_profile_set(soc_sand_dev_id,new_learn_profile,old_handle_profile);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
                /* update fid-learn-profile to limit, copy from old index */
                soc_sand_rv = soc_ppd_frwrd_mact_learn_profile_limit_info_get(soc_sand_dev_id,old_learn_profile,&limit_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                soc_sand_rv = soc_ppd_frwrd_mact_learn_profile_limit_info_set(soc_sand_dev_id,new_learn_profile,&limit_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                vsi_info.mac_learn_profile_id = new_learn_profile; 
            }
        }
        soc_sand_rv = soc_ppd_vsi_info_set(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    
    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_vlan_control_vlan_verify(int unit, CONST bcm_vlan_control_vlan_t *control) {
    BCMDNX_INIT_FUNC_DEFS;
    if (control->flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No support for control->flags")));
    }
    if (control->vrf != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("No support for control->flags, use bcm_petra_l3_intf_create to update vrf ")));
    }
    if ((control->outer_tpid != 0x8100) && (control->outer_tpid != 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No support for control->outer_tpid")));
    }

    if (control->l2_mcast_flood_mode != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No support for control->l2_mcast_flood_mode")));
    }

    if (control->ip4_mcast_flood_mode != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No support for control->ip4_mcast_flood_mode")));
    }

    if (control->ip6_mcast_flood_mode != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No support for control->ip6_mcast_flood_mode")));
    }
     
    if (control->forwarding_mode != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No support for control->forwarding_mode")));
    }
     
    if (control->forwarding_mode != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No support for control->forwarding_mode")));
    }

    if (control->urpf_mode != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No support for control->urpf_mode")));
    }

    if (control->cosq != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No support for control->cosq")));
    }

      
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_vector_flags_set(
    int unit, 
    bcm_vlan_vector_t vlan_vector, 
    uint32 flags_mask, 
    uint32 flags_value)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API not supported."))); 
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_port_create(
    int unit, 
    bcm_vlan_port_t *vlan_port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_vlan_port_create(unit, vlan_port));
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_port_destroy(
    int unit, 
    bcm_gport_t gport)
{

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  _BCM_DPP_SWITCH_API_START(unit);
  /*
   * check params
   */
    if (!BCM_GPORT_IS_VLAN_PORT(gport)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given gport is not of vlan-port type "))); /* given gport is not of vlan-port type */
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_delete(unit,0,gport));
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_vlan_pon_egress_info_get
 * Purpose:
 *      Get 
 * Parameters:
 *      unit        - (IN)  Device Number
 *      lif_id      - (IN)  input lif index
 *      out_lif_id  - (IN)  out AC index or out lif index
 *      vlan_port   - (OUT) info of layer 2 logical port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_vlan_port_pon_egress_info_get(
    int unit,
    int lif_id,
    bcm_vlan_port_t *vlan_port)
{
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint16 tpid;
    int pcp;
    bcm_tunnel_id_t tunnel_id;
    int out_ac_id;
    int is_3_tags_data;
    SOC_PPD_EG_AC_INFO eg_ac_info;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(vlan_port);

    soc_sand_dev_id = (unit);

    rv = _bcm_dpp_pon_lif_is_3_tags_data(unit, lif_id, &is_3_tags_data);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get egress info */
    /* in case of PON 3 TAGs modification, need to get egress info from OUT AC */
    if (is_3_tags_data) {
        rv = _bcm_dpp_pon_encap_data_enty_get(unit, lif_id, &tpid, &pcp, &tunnel_id, &out_ac_id);
        BCMDNX_IF_ERR_EXIT(rv);
        
        vlan_port->egress_tunnel_value = tunnel_id;

        soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, out_ac_id, &eg_ac_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        vlan_port->egress_vlan = eg_ac_info.edit_info.vlan_tags[0].vid;
        vlan_port->egress_inner_vlan = eg_ac_info.edit_info.vlan_tags[1].vid;
    }else { /* PON 2 TAGs manipulation */
        soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, lif_id, &eg_ac_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        vlan_port->egress_tunnel_value = eg_ac_info.edit_info.vlan_tags[0].vid;
        vlan_port->egress_vlan = eg_ac_info.edit_info.vlan_tags[1].vid;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_vlan_port_find(
    int unit, 
    bcm_vlan_port_t *vlan_port)
{
    SOC_PPD_EG_AC_INFO     eg_ac_info;
    int lif_id = 0, in_lif_id = 0, out_lif_id = 0;
    int fec_id = -1, act_fec = 0;
    int is_local;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO
        fec_entry[2];
    SOC_PPD_FRWRD_FEC_PROTECT_INFO
        protect_info;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE              
        protect_type;
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_LIF_ENTRY_INFO
        *lif_entry_info = NULL;
    SOC_PPD_L2_LIF_AC_KEY  in_ac_key;
    SOC_PPD_L2_LIF_AC_INFO  ac_info;
    SOC_PPD_L2_LIF_AC_INFO*  protection_ac_info = NULL;
    uint8 found;
    SOC_PPD_LIF_ID
        lif_id_tmp;
    int gport_val;
    uint8 remote_lif;
    uint8 is_ingress = TRUE, is_egress = TRUE;
    _BCM_GPORT_PHY_PORT_INFO *phy_port;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    
    BCMDNX_NULL_CHECK(vlan_port);

    /* get according to vlan-port-id. Do not look at match criteria. */
    if (BCM_GPORT_IS_VLAN_PORT(vlan_port->vlan_port_id) ) {

        /* Determine if it's an Egress only LIF according to the gport encapsulation */
        rv = _bcm_dpp_gport_to_lif(unit, vlan_port->vlan_port_id, &in_lif_id, &out_lif_id,
                                   &fec_id, &is_local);
        BCMDNX_IF_ERR_EXIT(rv);
        if(!is_local && !_Bcm_dpp_sync_all) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Vlan port-id not found.")));
        }

        is_ingress = (in_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;
        is_egress = (out_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;

        /* check if local or remote LIF */
        rv = _bcm_dpp_gport_has_remote_lif(unit,vlan_port->vlan_port_id,&remote_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_ENCAP_ID_SET(vlan_port->encap_id, ((is_ingress) ? in_lif_id : out_lif_id));
        if (remote_lif) {
            BCM_ENCAP_REMOTE_SET(vlan_port->encap_id);
        }

        if(!remote_lif && (is_local || _Bcm_dpp_sync_all)) {
            if (is_ingress) {
                /* Get the match parameters */
                rv = _bcm_dpp_in_lif_ac_match_get(unit, vlan_port, in_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
                /* get ingress info */
                BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPD_LIF_ENTRY_INFO), "bcm_petra_vlan_port_find.lif_entry_info");
                if (lif_entry_info == NULL) {        
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
                }
                soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, in_lif_id, lif_entry_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if(lif_entry_info->type != SOC_PPD_LIF_ENTRY_TYPE_AC && lif_entry_info->type != SOC_PPD_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("LIF entry is not AC.")));      /* expected to be AC type*/
                }
                vlan_port->vsi = lif_entry_info->value.ac.vsid;
                /* If vsi assignment mode is SOC_PPC_VSI_EQ_IN_VID and criteria is BCM_VLAN_PORT_MATCH_PORT, update vsi to BCM_VLAN_ALL */
                if ((lif_entry_info->value.ac.vsi_assignment_mode == SOC_PPC_VSI_EQ_IN_VID) &&
                    SOC_IS_ARAD(unit) &&
                    (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT)) {
                    vlan_port->vsi = BCM_VLAN_ALL;
                }

                /* Get the protection data from here */
                protection_ac_info = &(lif_entry_info->value.ac);
            }
        }
    }
    else if (vlan_port->vlan_port_id == 0) {

        /* Caller provided no gport. Vlan port may be found if criteria is valid. */
        switch (vlan_port->criteria) {
        case BCM_VLAN_PORT_MATCH_PORT               :
        case BCM_VLAN_PORT_MATCH_PORT_VLAN          :
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED  :
            break;
        case BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN  :
            if (!SOC_DPP_CONFIG(unit)->pp.initial_vid_enable) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("initial vid enable must be on to find vlan port with _PORT_INITIAL_VLAN criteria")));
            } 
            break;
        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED  :
            if (!SOC_DPP_CONFIG(unit)->pp.custom_feature_vt_tst1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("UNTAGGED database is not valid criteria")));
            } 
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN      :
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED :
            if (SOC_DPP_CONFIG(unit)->pp.vlan_match_db_mode != SOC_DPP_VLAN_DB_MODE_PCP) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN DB must be PCP mode to find vlan port with PCP criteria")));
            } 
            break;
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL                         :
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN                    :
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED            :
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP                     :
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN                :
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED        :
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE      :
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE :
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE               :
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_ETHERTYPE          :
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED_ETHERTYPE  :
            if (!SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PON application must be enabled to use _PORT_TUNNEL criteria.")));
            } 
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If vlan_port_id is not set, criteria must be valid.")));
            break;
        }

        /* If criteria is valid, try lookup to see if the gport is live somewhere */

                
        /* InLIF can be used as Egress only LIFs aren't supported for find according to ISEM key */
        if ((vlan_port->flags) & BCM_VLAN_PORT_CREATE_EGRESS_ONLY) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Find isn't supported for Egress only LIFs according to ISEM key")));
        }
        
        /* get lif/FEC attached to this cretira */
        rv = _bcm_dpp_gport_fill_ac_key(unit,vlan_port,BCM_GPORT_VLAN_PORT,&in_ac_key);
        BCMDNX_IF_ERR_EXIT(rv);
        
        soc_sand_rv = soc_ppd_l2_lif_ac_get(soc_sand_dev_id,&in_ac_key,&lif_id_tmp,&ac_info,&found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        lif_id = lif_id_tmp;
        
        /* such gport is not exist, exit*/
        if(!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("LIF not found.")));
        }
        /* get ingress info */
        vlan_port->vsi = ac_info.vsid;
        /* If vsi assignment mode is SOC_PPC_VSI_EQ_IN_VID and criteria is BCM_VLAN_PORT_MATCH_PORT, update vsi to BCM_VLAN_ALL */
        if ((ac_info.vsi_assignment_mode == SOC_PPC_VSI_EQ_IN_VID) &&
            SOC_IS_ARAD(unit) &&
            (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT)) {
            vlan_port->vsi = BCM_VLAN_ALL;
        }
        
        /* Get the protection data */
        protection_ac_info = &ac_info;
        
        if(ac_info.learn_record.learn_info.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC) {
            fec_id = ac_info.learn_record.learn_info.dest_id;
            /* gport value is FEC */
            BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id,fec_id);
        }
        else{
            fec_id = -1;
            /* if not protected then ID derived from LIF */
            BCMDNX_IF_ERR_EXIT(l2_interface_calculate_port_id(unit, act_fec, lif_id, vlan_port, BCM_GPORT_VLAN_PORT));
        }

        /* Finally, determine whether gport is ingress only or egress only according to the vlan_port_id */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_l2_interface_determine_ingress_egress(vlan_port->vlan_port_id, &is_ingress, &is_egress));
        out_lif_id = lif_id;
        
    } else { /* Caller provided gport that is not a vlan port */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given vlan_port_id is not a vlan port.")));
    }

    /* Handle Egress information */
    if (is_egress) {
        /* get PON egress info */
        if (SOC_IS_ARAD(unit) && SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
            rv = _bcm_dpp_vlan_port_pon_egress_info_get(unit, out_lif_id, vlan_port);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            /* get egress info */
            soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, out_lif_id, &eg_ac_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    
            vlan_port->egress_vlan = eg_ac_info.edit_info.vlan_tags[0].vid;
            vlan_port->egress_inner_vlan = eg_ac_info.edit_info.vlan_tags[1].vid;

#ifdef BCM_88660_A0
            if (SOC_IS_ARADPLUS(unit) && (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) && eg_ac_info.edit_info.use_as_data_entry) {
                vlan_port->egress_tunnel_value = SOC_PPD_EG_ENCAP_DATA_2ND_PON_TUNNEL_FORMAT_TUNNEL_ID_GET(soc_sand_dev_id, &eg_ac_info.edit_info);
            }

#endif
        }

        /* In case of egress protection, retrieve the egress protection info */
        /* TBD - Update after modification of eg_ac_info.edit_info with additional 1/4 entry */
        /*
        if (eg_ac_info.edit_info.protection_pointer != 0) {
            vlan_port->egress_failover_id = eg_ac_info.edit_info.protection_pointer;
            vlan_port->egress_failover_port_id = eg_ac_info.edit_info.protection_path;
        }   */
    }

    /* If port has 1+1 protection, get protection info */
    if (protection_ac_info && fec_id == -1 /* fec means 1:1 protection, port can't have both */) {
        /* Fill ingress_failover_id */
        if (protection_ac_info->protection_pointer) {
            DPP_FAILOVER_TYPE_SET(vlan_port->ingress_failover_id, protection_ac_info->protection_pointer, DPP_FAILOVER_TYPE_INGRESS);
        } else {
            vlan_port->ingress_failover_id = protection_ac_info->protection_pointer;
        }

        rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (shr_htb_key_t) &(vlan_port->vlan_port_id), (shr_htb_data_t) &phy_port, 0);
        
        /* This if/else block fills failover_port_id and failover_mc_group, if relevant */

        if (rv == BCM_E_NOT_FOUND && !is_egress) { /* Unprotected ingress only vlan ports don't keep hash information, so it's not an error */
            /* do nothing and exit 1+1 block */
            rv = BCM_E_NONE;
        } else {
            BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Database error")));
            if (phy_port->type == _BCM_DPP_GPORT_TYPE_MC) { /* GPORT_TYPE_MC marks that gport has mc ingress protection */

                /* failover_mc_group is embedded in the vlan port */
                gport_val = BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id);
                _BCM_MULTICAST_GROUP_SET(vlan_port->failover_mc_group, _BCM_MULTICAST_TYPE_L2, SOC_DPP_MC_GROUP_FROM_ENCAP_ID(gport_val));

                /* If working port, fill failover_port_id */
                if (protection_ac_info->protection_pointer      /* means port is protected */
                && !protection_ac_info->protection_pass_value   /* protection_pass_value 0 means port is working port, 1 means port is protecting port */
                    ) {

                    /* Create MC gport id */
                    _bcm_dpp_gport_plus_1_mc_to_id(vlan_port->failover_mc_group, 
                                                   0, 
                                                   &gport_val); 

                    /* Set vlan port id flags. */
                    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->failover_port_id, gport_val);   
                }

            } else if ((phy_port->type == _BCM_DPP_GPORT_IN_TYPE_AC /* Potentially ingress protection */
                        || phy_port->type == _BCM_DPP_GPORT_IN_TYPE_RING /* Ring protection */)
                       && protection_ac_info->protection_pointer /* If !=0, gport has ingress protection */
                       && !protection_ac_info->protection_pass_value /* protection_pass_value 0 means port is working port, 1 means protecting port */
                       ){

                vlan_port->failover_port_id = 1; /* 1 means working port, 0 means protecting port. */

            } else { /* Either no protection, or protecting port */
                vlan_port->failover_port_id = 0; /* leave empty. */
            }
        }
    }

    /* If port has 1:1 protection, get protection info. The information exists on remote devices as well. */
    if (fec_id != -1) {

        /* get information according to working FEC */
        fec_id = _BCM_PPD_GPORT_FEC_TO_WORK_FEC(fec_id);

        soc_sand_rv =
            soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, fec_id,
                                    &protect_type, &fec_entry[0],
                                    &fec_entry[1], &protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (SOC_DPP_ENCAP_TYPE_GET(BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id)) != _DPP_ENCAP_ID_FORWARD_VAL) {
            if (protect_type != SOC_PPD_FRWRD_FEC_PROTECT_TYPE_PATH) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("FEC is not of protection type path")));      /* not expected */
            }

            vlan_port->failover_id = protect_info.oam_instance_id;
            if (vlan_port->failover_id != BCM_FAILOVER_ID_LOCAL) {
                DPP_FAILOVER_TYPE_SET(vlan_port->failover_id, protect_info.oam_instance_id, DPP_FAILOVER_TYPE_FEC);
            }

            if (_BCM_PPD_GPORT_IS_WORKING_PORT_ID(vlan_port->vlan_port_id)) {
                BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->failover_port_id,
                                           _BCM_PPD_GPORT_WORK_TO_PROTECT_FEC
                                           (fec_id));
            }
        }

        /* In JERICHO possibly set the CASCADED flag. */
        if (SOC_IS_JERICHO(unit)) {
            _bcm_dpp_am_fec_alloc_usage usage;
            rv = bcm_dpp_am_fec_get_usage(unit, fec_id, &usage);
            BCMDNX_IF_ERR_EXIT(rv);

            if (usage == _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED) {
                vlan_port->flags |= BCM_VLAN_PORT_CASCADED;
            }
        }
    }

    /* Port may also be attached to a fec (1:1 protection) that is not a part of the gport */
    if (!vlan_port->failover_id && !vlan_port->ingress_failover_id  /* These three fields indicate that protection info was */
        && !vlan_port->failover_port_id                         /* already found and we can skip this block */
        && is_ingress         /* If port is egress only, it won't have this protection type */
        && protection_ac_info && !protection_ac_info->vsid /* If vsi is set, this protection type won't exist either. */
        ) {

        gport_val = _bcm_dpp_in_lif_match_learn_gport_get(unit, 
                                                          (in_lif_id > 0) ? in_lif_id : lif_id /* If vlan_port_id was provided, in_lif_id
                                                                                                will be set. Otherwise, lif_id */ );

        if (gport_val && gport_val != vlan_port->vlan_port_id) { /* By default, the learn gport is the gport itself. Otherwise, it's the 
                                                                    failover gport */
            vlan_port->failover_port_id = gport_val;
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}


/*
 * Function    : _bcm_petra_vlan_port_translation_info_to_ppd
 * Description : Converts VLAN translation per LIF info from a BCM API structure
 *               to a PPD Ingress/Egress structure.
 *               Applicable only for Advanced VLAN edit mode.
 *  
 * Parameters  : (IN)  unit          - BCM device number
 *  *            (IN)  vlan_port_translation - BCM API VLAN edit info per LIF.
 *               (OUT) ing_edit_info - PPD Ingress VLAN translation per LIF
 *                     configuration structure or NULL.
 *               (OUT) eg_edit_info - PPD Egress VLAN translation per LIF
 *                     configuration structure or NULL.
 * Returns     : BCM_E_XXX
 */
int 
_bcm_petra_vlan_port_translation_info_to_ppd(
    int unit, 
    bcm_vlan_port_translation_t *vlan_port_translation,
    SOC_PPC_L2_LIF_AC_ING_EDIT_INFO *ing_edit_info,
    SOC_PPC_EG_AC_VLAN_EDIT_INFO *eg_edit_info)
{

  BCMDNX_INIT_FUNC_DEFS;

  /* If a PPD ingress info struct was supplied, map to the VLAN Port translation to ingress */
  if (ing_edit_info != NULL) {
      ing_edit_info->ing_vlan_edit_profile = vlan_port_translation->vlan_edit_class_id;
      ing_edit_info->vid = vlan_port_translation->new_outer_vlan;
      ing_edit_info->vid2 = vlan_port_translation->new_inner_vlan;
  }

  /* If a PPD egress info struct was supplied, map to the VLAN Port translation to egress */
  if (eg_edit_info != NULL) {
      eg_edit_info->edit_profile = vlan_port_translation->vlan_edit_class_id;
      eg_edit_info->vlan_tags[0].vid = vlan_port_translation->new_outer_vlan;
      eg_edit_info->vlan_tags[1].vid = vlan_port_translation->new_inner_vlan;
  }

   BCMDNX_FUNC_RETURN;
}


/*
 * Function    : _bcm_petra_vlan_port_translation_info_from_ingress_ppd
 * Description : Converts Ingress VLAN translation per LIF info from a
 *               PPD structure to a BCM API structure.
 *               Applicable only for Advanced VLAN edit mode.
 *  
 * Parameters  : (IN)  unit          - BCM device number
 *               (IN)  ing_edit_info - PPD Ingress VLAN translation
 *                     per LIF configuration structure.
 *               (OUT) vlan_port_translation - BCM API VLAN edit info per LIF.
 * Returns     : BCM_E_XXX
 */
int 
_bcm_petra_vlan_port_translation_info_from_ingress_ppd(
    int unit,
    SOC_PPD_L2_LIF_AC_ING_EDIT_INFO *ing_edit_info,
    bcm_vlan_port_translation_t *vlan_port_translation)
{
  BCMDNX_INIT_FUNC_DEFS;

  /* Convert PPD ingress vlan editing info to the BCM API struct */
  vlan_port_translation->vlan_edit_class_id = ing_edit_info->ing_vlan_edit_profile;
  vlan_port_translation->new_outer_vlan = ing_edit_info->vid;
  vlan_port_translation->new_inner_vlan = ing_edit_info->vid2;

  BCMDNX_FUNC_RETURN;
}


/*
 * Function    : _bcm_petra_vlan_port_translation_info_from_egress_ppd
 * Description : Converts Egress VLAN translation per LIF info from a
 *               PPD structure to a BCM API structure.
 *               Applicable only for Advanced VLAN edit mode.
 *  
 * Parameters  : (IN)  unit         - BCM device number
 *               (IN)  eg_edit_info - PPD Egress VLAN translation
 *                     per LIF configuration structure.
 *               (OUT) vlan_port_translation - BCM API VLAN edit info per LIF.
 * Returns     : BCM_E_XXX
 */
int 
_bcm_petra_vlan_port_translation_info_from_egress_ppd(
    int unit,
    SOC_PPC_EG_AC_VLAN_EDIT_INFO *eg_edit_info,
    bcm_vlan_port_translation_t *vlan_port_translation)
{
  BCMDNX_INIT_FUNC_DEFS;

  /* Convert PPD egress VLAN editing info to the BCM API struct */
  vlan_port_translation->vlan_edit_class_id = eg_edit_info->edit_profile;
  vlan_port_translation->new_outer_vlan = eg_edit_info->vlan_tags[0].vid;
  vlan_port_translation->new_inner_vlan = eg_edit_info->vlan_tags[1].vid;

  BCMDNX_FUNC_RETURN;
}


/*
 * Function    : _bcm_petra_vlan_port_translation_set_verify
 * Description : Internal function to verify the configuration for the 
 *               bcm_petra_vlan_port_translation_set API. The function
 *               validates the VLAN values and the VLAN edit profile value.
 *               Applicable only for Advanced VLAN edit mode.
 *  
 * Parameters  : (IN)  unit         - BCM device number
 *               (IN)  vlan_port_translation - VLAN translation
 *                     configuration per LIF
 * Returns     : BCM_E_XXX
 */
int 
_bcm_petra_vlan_port_translation_set_verify(
    int unit, 
    bcm_vlan_port_translation_t *vlan_port_translation)
{
    BCMDNX_INIT_FUNC_DEFS;

    /* Validate the VLAN values */
    BCM_DPP_VLAN_CHK_ID(unit, vlan_port_translation->new_outer_vlan);
    BCM_DPP_VLAN_CHK_ID(unit, vlan_port_translation->new_inner_vlan);

    /* Validate the VLAN edit profile */
    BCM_DPP_VLAN_EDIT_PROFILE_VALID(unit, vlan_port_translation->vlan_edit_class_id,
        (vlan_port_translation->flags & BCM_VLAN_ACTION_SET_INGRESS));

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function    : bcm_petra_vlan_port_translation_set
 * Description : Set VLAN translation configuration per LIF.
 *               Applicable only for Advanced VLAN edit mode.
 *  
 * Parameters  : (IN)  unit         - BCM device number
 *               (IN)  vlan_port_translation - VLAN translation configuration
 *                     per LIF
 * Returns     : BCM_E_XXX
 */
int 
bcm_petra_vlan_port_translation_set(
    int unit, 
    bcm_vlan_port_translation_t *vlan_port_translation)
{
    int rv, is_local;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    uint32 is_ingress;
    SOC_PPD_LIF_ENTRY_INFO *lif_ing_info = NULL;
    SOC_PPD_EG_AC_INFO *lif_eg_info = NULL;
    int in_lif_id, out_lif_id;
    _bcm_lif_type_e lif_usage_type, in_lif_usage_type, out_lif_usage_type;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* Handle the API only if the SOC property is set to Advanced VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Advanced mode")));
    }

    /* Basic parameter validation */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(vlan_port_translation);


    /* Verify that one of the Ingress/Egress flags is set, and only one is set. */
    is_ingress = (vlan_port_translation->flags & BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    if ((is_ingress && (vlan_port_translation->flags & BCM_VLAN_ACTION_SET_EGRESS)) ||
        (!is_ingress && !(vlan_port_translation->flags & BCM_VLAN_ACTION_SET_EGRESS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN translate ID should be for either Ingress or Egress")));
    }

    /* Perform BCM related validations */
    rv = _bcm_petra_vlan_port_translation_set_verify(unit, vlan_port_translation);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Convert gport to LIF */
    rv = _bcm_dpp_gport_to_lif(unit, vlan_port_translation->gport, &in_lif_id, &out_lif_id, NULL, &is_local);
    BCMDNX_IF_ERR_EXIT(rv);



    /* Fail if it isn't a local gport */
    if (!is_local) {
        /* API is relevant only for local configuration */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The API supports only local ports")));
    }

    /* The flags are either INGRESS_ONLY or EGRESS_ONLY */
    if (is_ingress) {
        out_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    } else {
        in_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    }
    if ((in_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) && (out_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The supplied gport represents an invalid LIF")));
    }

    /* Verify the type is VLAN for the relevant LIF */
    rv = _bcm_dpp_lif_usage_get(unit, in_lif_id, out_lif_id, &in_lif_usage_type, &out_lif_usage_type);
    BCMDNX_IF_ERR_EXIT(rv);
    lif_usage_type = (in_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? out_lif_usage_type : in_lif_usage_type;

    if (lif_usage_type != _bcmDppLifTypeVlan) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The supplied gport isn't an allocated LIF")));
    }

    /* Handle the Ingress scenario */
    if (is_ingress) {
        /* BCMDNX_ALLOC for the HW LIF ingress info structure */
        BCMDNX_ALLOC(lif_ing_info, sizeof(*lif_ing_info), "bcm_petra_vlan_port_translation_set.lif_ing_info");
        if (lif_ing_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }

        /* Get the Ingress LIF info from the HW */
        soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, in_lif_id, lif_ing_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Verify the type is ac */
        if (lif_ing_info->type != SOC_PPC_LIF_ENTRY_TYPE_AC) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The supplied gport isn't of type AC")));
        }

        /* AC to PWE and AC to ISID services can get only one VID. Verify that the second is set to 0 */
        if (((lif_ing_info->value.ac.service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE) ||
             (lif_ing_info->value.ac.service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB)) &&
                (vlan_port_translation->new_inner_vlan != 0)) { 
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In AC P2P to PWE or Mac-In-Mac only outer vlan can be edited")));
        }

        /* Set the VLAN editing info to the user supplied values */
        rv = _bcm_petra_vlan_port_translation_info_to_ppd(unit, vlan_port_translation, &(lif_ing_info->value.ac.ing_edit_info), NULL);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Update the HW with the LIF info that includes the VLAN editing info for the LIF */
        soc_sand_rv = soc_ppd_lif_table_entry_update(soc_sand_dev_id, in_lif_id, lif_ing_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } 
    /* Handle the Egress scenario */
    else {
        /* BCMDNX_ALLOC for the HW LIF egress info structure */
        BCMDNX_ALLOC(lif_eg_info, sizeof(*lif_eg_info), "bcm_petra_vlan_port_translation_set.lif_eg_info");
        if (lif_eg_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }

        /* Get the Egress LIF info from the HW */
        soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, out_lif_id, lif_eg_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Set the VLAN editing info to the user supplied values */
        rv = _bcm_petra_vlan_port_translation_info_to_ppd(unit, vlan_port_translation, NULL, &(lif_eg_info->edit_info));
        BCMDNX_IF_ERR_EXIT(rv);

        /* Update the HW with the LIF info that includes the VLAN editing info for the LIF */
        soc_sand_rv = soc_ppd_eg_ac_info_set(soc_sand_dev_id, out_lif_id, lif_eg_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    BCM_FREE(lif_ing_info);
    BCM_FREE(lif_eg_info);

    BCMDNX_FUNC_RETURN;
}


/*
 * Function    : bcm_petra_vlan_port_translation_get
 * Description : Get the VLAN translation configuration per LIF.
 *               Applicable only for Advanced VLAN edit mode.
 *  
 * Parameters  : (IN)  unit         - BCM device number
 *               (OUT) vlan_port_translation - VLAN translation info per LIF
 * Returns     : BCM_E_XXX
 */
int 
bcm_petra_vlan_port_translation_get(
    int unit, 
    bcm_vlan_port_translation_t *vlan_port_translation)
{
    int rv, is_local;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    uint32 is_ingress;
    SOC_PPD_LIF_ENTRY_INFO *lif_ing_info = NULL;
    SOC_PPD_EG_AC_INFO *lif_eg_info = NULL;
    int in_lif_id, out_lif_id;
    _bcm_lif_type_e lif_usage_type, in_lif_usage_type, out_lif_usage_type;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* Handle the API only if the SOC property is set to Advanced VLAN translation mode */
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("The API is only available when VLAN translation is set to Advanced mode")));
    }

    /* Basic parameter validation */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(vlan_port_translation);

    /* Verify that one of the Ingress/Egress flags is set, and only one is set. */
    is_ingress = (vlan_port_translation->flags & BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    if ((is_ingress && (vlan_port_translation->flags & BCM_VLAN_ACTION_SET_EGRESS)) ||
        (!is_ingress && !(vlan_port_translation->flags & BCM_VLAN_ACTION_SET_EGRESS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN translate ID should be for either Ingress or Egress")));
    }

    /* Convert gport to LIF */
    rv = _bcm_dpp_gport_to_lif(unit, vlan_port_translation->gport, &in_lif_id, &out_lif_id, NULL, &is_local);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Fail if it isn't a local gport */
    if (!is_local) {
        /* API is relevant only for local configuration */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The API supports only local ports")));
    }

    /* The flags are either INGRESS_ONLY or EGRESS_ONLY */
    if (is_ingress) {
        out_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    } else {
        in_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    }
    if ((in_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) && (out_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The supplied gport represents an invalid LIF")));
    }

    /* Verify the type is VLAN for the relevant LIF */
    rv = _bcm_dpp_lif_usage_get(unit, in_lif_id, out_lif_id, &in_lif_usage_type, &out_lif_usage_type);
    BCMDNX_IF_ERR_EXIT(rv);
    lif_usage_type = (in_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? out_lif_usage_type : in_lif_usage_type;

    if (lif_usage_type != _bcmDppLifTypeVlan) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The supplied gport isn't an allocated LIF")));
    }

    /* Handle the Ingress scenario */
    if (is_ingress) {
        /* BCMDNX_ALLOC for the HW LIF ingress info structure */
        BCMDNX_ALLOC(lif_ing_info, sizeof(*lif_ing_info), "bcm_petra_vlan_port_translation_get.lif_ing_info");
        if (lif_ing_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }

        /* Get the Ingress LIF info from the HW */
        soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, in_lif_id, lif_ing_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Convert the VLAN editing info to the user supplied structure */
        rv = _bcm_petra_vlan_port_translation_info_from_ingress_ppd(unit, &(lif_ing_info->value.ac.ing_edit_info), vlan_port_translation);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    /* Handle the Egress scenario */
    else {
        /* BCMDNX_ALLOC for the HW LIF egress info structure */
        BCMDNX_ALLOC(lif_eg_info, sizeof(*lif_eg_info), "bcm_petra_vlan_port_translation_get.lif_eg_info");
        if (lif_eg_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }

        /* Get the Egress LIF info from the HW */
        soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, out_lif_id, lif_eg_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Set the VLAN editing info to the user supplied values */
        rv = _bcm_petra_vlan_port_translation_info_from_egress_ppd(unit, &(lif_eg_info->edit_info), vlan_port_translation);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCM_FREE(lif_ing_info);
    BCM_FREE(lif_eg_info);
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_vlan_gport_info_get(
    int unit,
    bcm_vlan_gport_info_t *vlan_gport_info)
{
    int internal_flags;
    int rv;    
    int flags = 0;
    int vlan_or_vsi;
    BCMDNX_INIT_FUNC_DEFS;

    internal_flags = 0;
    if (vlan_gport_info->flags & BCM_VLAN_GPORT_ADD_SERVICE) {
        internal_flags = _BCM_DPP_VLAN_GPORT_GET_VSI_INFO_ONLY;
        vlan_or_vsi = vlan_gport_info->vsi;
    } else {
        internal_flags = _BCM_DPP_VLAN_GPORT_GET_VLAN_INFO_ONLY;
        vlan_or_vsi = vlan_gport_info->vlan;
    }

    rv = _bcm_petra_vlan_gport_get(unit, vlan_or_vsi, vlan_gport_info->gport, internal_flags, &flags);
    BCMDNX_IF_ERR_EXIT(rv);

    vlan_gport_info->flags |= (uint32)flags;
                          
exit:
    BCMDNX_FUNC_RETURN;
}


