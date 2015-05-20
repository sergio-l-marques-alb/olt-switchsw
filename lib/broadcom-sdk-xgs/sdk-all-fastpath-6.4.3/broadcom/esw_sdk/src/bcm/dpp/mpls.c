/*
 * $Id: mpls.c,v 1.119 Broadcom SDK $
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
 * Soc_petra-B Layer 2 Management
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_MPLS
#include <shared/bsl.h>
#include "bcm_int/common/debug.h"
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/failover.h>

#include <bcm_int/control.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/mpls.h>
#include <bcm_int/common/field.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/vlan.h>

#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/PPD/ppd_api_eg_encap.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <shared/shr_resmgr.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_profile_mgmt.h>

#include <soc/dpp/JER/JER_PP/jer_pp_mpls_term.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/mbcm.h>

#include <soc/dcmn/dcmn_wb.h>

#ifdef BCM_88660_A0
/* Reserved labels 0-15. */
/* For these lables (in Arad+) the check BOS handling may be set per label. */
#define _BCM_PETRA_MPLS_MAX_RESERVED_MPLS_LABEL 15

#endif /* BCM_88660_A0 */

static    int _Bcm_dpp_sync_lif_eep = 1;
/*
 *global SW state 
 */

_bcm_dpp_mpls_bookkeeping_t _bcm_dpp_mpls_bk_info[BCM_MAX_NUM_UNITS] = { {0} };

/*
 * when traversing the ILM, to perfrom action on each entry how many 
 * entries to return in each iteration 
 */
#define _BCM_PETRA_MPLS_TRVRS_ITER_BLK_SIZE (20)


/* static varibales for traverse */
SOC_PPD_FRWRD_ILM_KEY *_bcm_mpls_traverse_ilm_keys[BCM_MAX_NUM_UNITS] = {NULL};
SOC_PPD_FRWRD_DECISION_INFO *_bcm_mpls_traverse_ilm_vals[BCM_MAX_NUM_UNITS] = {NULL};

#define _BCM_DPP_MPLS_LIF_TERM_DEFAULT_PROFILE (0)

typedef enum _bcm_dpp_mpls_egress_action_type_s {
    _bcm_dpp_mpls_egress_action_type_push=0,
    _bcm_dpp_mpls_egress_action_type_swap,
    _bcm_dpp_mpls_egress_action_type_pop,
    _bcm_dpp_mpls_egress_action_type_swap_coupling
} _bcm_dpp_mpls_egress_action_type_t;

/*
 * Function:
 *      bcm_dpp_mpls_check_init
 * Purpose:
 *      Check if MPLS is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int _bcm_dpp_mpls_check_init(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    if (!_bcm_dpp_mpls_bk_info[unit].initialized) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("not initialized")));
    } else {
        BCM_EXIT;
    }
exit:
        BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dpp_mpls_lock
 * Purpose:
 *      Take the MPLS software module Lock Sempahore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int bcm_dpp_mpls_lock(
    int unit)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_mpls_check_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    sal_mutex_take(MPLS_INFO((unit))->mpls_mutex, sal_mutex_FOREVER);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_dpp_mpls_unlock
 * Purpose:
 *      Release  the MPLS software module Lock Semaphore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

void bcm_dpp_mpls_unlock(
    int unit)
{
    int rv;

    rv = _bcm_dpp_mpls_check_init(unit);

    if (rv == BCM_E_NONE) {
        sal_mutex_give(MPLS_INFO((unit))->mpls_mutex);
    }
}


/*
 * Function:
 *      _bcm_petra_mpls_free_resource
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void _bcm_petra_mpls_free_resource(
    int unit,
    _bcm_dpp_mpls_bookkeeping_t * mpls_info)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (!mpls_info) {
        return;
    }

    if (_bcm_mpls_traverse_ilm_keys[unit] != NULL) {
       BCM_FREE(_bcm_mpls_traverse_ilm_keys[unit]);
    }

    if (_bcm_mpls_traverse_ilm_vals[unit] != NULL) {
       BCM_FREE(_bcm_mpls_traverse_ilm_vals[unit]);
    }

    BCMDNX_FUNC_RETURN_VOID;

}

/* 
 *   utilities function for port/vpn manipulation
 */



/* check if the given vpn is valid as l2 VPN */
STATIC int _bcm_dpp_l2_vpn_check(
    int unit,
    bcm_vpn_t vpn)
{
    int exist = 0;
    int vpn_id;

    BCMDNX_INIT_FUNC_DEFS;

    vpn_id = _BCM_DPP_VPN_ID_GET(vpn);

    /* Cross Connect VPN isn't allocated, but is valid for configuration */
    if (_BCM_DDP_IS_CROSS_CONNECT_P2P_VPN(vpn_id)) {
        return BCM_E_NONE;
    }

    if (vpn_id >= ((SOC_DPP_CONFIG(unit))->l2.nof_vsis)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VPN ID is out of range"))); 
    }

    /* check if vsi/vpn */
    exist = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vpn_id);
    if (exist != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VPN does not exist")));
    }
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_cleanup(
    int unit)
{
    _bcm_dpp_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    rv = bcm_dpp_mpls_lock(unit);
    BCMDNX_IF_ERR_CONT(rv);

    /* Free software resources */
    (void) _bcm_petra_mpls_free_resource(unit, mpls_info);

    bcm_dpp_mpls_unlock(unit);

    /* Destroy MPLS protection mutex. */
    if (MPLS_INFO((unit))->mpls_mutex) {
        sal_mutex_destroy(MPLS_INFO((unit))->mpls_mutex);
        MPLS_INFO((unit))->mpls_mutex = NULL;
    }
    
    /* Mark the state as uninitialized */
    mpls_info->initialized = FALSE;

    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/* bcm_mpls_egress_label_t_init not dispatchable */

int bcm_petra_mpls_exp_map_create(
    int unit,
    uint32 flags,
    int *exp_map_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable feature")));
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_exp_map_destroy(
    int unit,
    int exp_map_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable feature")));
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_exp_map_get(
    int unit,
    int exp_map_id,
    bcm_mpls_exp_map_t * exp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable feature")));
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_exp_map_set(
    int unit,
    int exp_map_id,
    bcm_mpls_exp_map_t * exp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable feature")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_mpls_exp_map_t_init not dispatchable */


int bcm_petra_mpls_init(
    int unit)
{
    _bcm_dpp_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int rv = BCM_E_NONE;
    SOC_PPC_MYMAC_TRILL_INFO mymac;    
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = (unit);

    /*
     * allocate resources
     */
    if (mpls_info->initialized) {
        rv = bcm_petra_mpls_cleanup(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*init warmbootable variables*/
    BCMDNX_ALLOC(_bcm_mpls_traverse_ilm_keys[unit], sizeof(SOC_PPD_FRWRD_ILM_KEY) *
                                        _BCM_PETRA_MPLS_TRVRS_ITER_BLK_SIZE,
                                        "mpls traverse info");
    if (_bcm_mpls_traverse_ilm_keys[unit] == NULL) {
        _bcm_petra_mpls_free_resource(unit, mpls_info);
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory error: couldnt allocate MPLS SW DB traverse ilm keys information")));
    }


    BCMDNX_ALLOC(_bcm_mpls_traverse_ilm_vals[unit], sizeof(SOC_PPD_FRWRD_DECISION_INFO) *
                  _BCM_PETRA_MPLS_TRVRS_ITER_BLK_SIZE, "mpls traverse info");
    if (_bcm_mpls_traverse_ilm_vals[unit] == NULL) {
        _bcm_petra_mpls_free_resource(unit, mpls_info);
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory error: couldnt allocate MPLS SW DB traverse ilm vals information")));
    }

    MPLS_INFO((unit))->mpls_mutex = sal_mutex_create("mpls_mutex");
    if (!MPLS_INFO((unit))->mpls_mutex) {
        _bcm_petra_mpls_free_resource(unit, mpls_info);
        BCM_EXIT;
    }

    if (SOC_IS_ARAD(unit)) {
        SOC_PPD_MPLS_TERM_PROFILE_INFO term_profile;

        SOC_PPD_MPLS_TERM_PROFILE_INFO_clear(&term_profile);
        
#ifdef BCM_88660_A0
        term_profile.check_bos = 1;
        if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) {
          term_profile.check_bos = 0;
        }
#endif /* BCM_88660_A0 */

        rv = _bcm_dpp_am_template_lif_term_init(unit, _BCM_DPP_MPLS_LIF_TERM_DEFAULT_PROFILE, &term_profile);
        BCMDNX_IF_ERR_EXIT(rv);

    }

    if (!SOC_WARM_BOOT(unit)) {
        /* MPLS TP reserved mac address */
        if (soc_property_get(unit, spn_MPLS_TP_MYMAC_RESERVED_ADDRESS, 0)) {
            if (SOC_DPP_CONFIG(unit)->trill.mode) {
                /* Trill do not co exist with MPLS TP reserved addresss */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Trill and soc property MPLS_TP_MYMAC_RESERVED_ADDRESS do not coexist")));
            }
            
            soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &mymac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           
            /* MPLS TP mymac address 01-00-5E-90-00-00*/
            mymac.all_rbridges_mac.address[5] = 1;
            mymac.all_rbridges_mac.address[4] = 0;
            mymac.all_rbridges_mac.address[3] = 0x5e;
            mymac.all_rbridges_mac.address[2] = 0x90;
            mymac.all_rbridges_mac.address[1] = 0;
            mymac.all_rbridges_mac.address[0] = 0;
            soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &mymac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

	/* Init MPLS special label termination */
    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mpls_termination_spacial_labels_init,(unit)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);    

    /* Create MPLS-Explicit-Dummy for reserved label explicit NULL */
    if (SOC_DPP_CONFIG(unit)->pp.explicit_null_support
#ifdef BCM_WARM_BOOT_SUPPORT
        && !SOC_WARM_BOOT(unit)
#endif
        ) {
        bcm_mpls_tunnel_switch_t explicit_null_tunnel_switch;
        bcm_mpls_tunnel_switch_t_init(&explicit_null_tunnel_switch);
        BCM_GPORT_TUNNEL_ID_SET(explicit_null_tunnel_switch.tunnel_id, SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id);
        explicit_null_tunnel_switch.flags |= (BCM_MPLS_SWITCH_WITH_ID);
        explicit_null_tunnel_switch.flags |= 0x08000000 /*(BCM_MPLS_SWITCH_LOOKUP_NONE)*/ ; /* only allocate LIF */
        explicit_null_tunnel_switch.action = BCM_MPLS_SWITCH_ACTION_POP;
        rv = bcm_petra_mpls_tunnel_switch_create(unit, &explicit_null_tunnel_switch);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create Explicit-NULL-LIF")));
        }
    }

    /* Mark the state as initialized */
    mpls_info->initialized = TRUE;

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


/*
 * Check if given Push Profile was already allocated. 
 * If it was, return it's attributes. 
 */
int _bcm_dpp_mpls_port_push_profile_get_internal(
    int unit,
    bcm_mpls_port_t *   mpls_port,
    uint8 * is_allocated)
{
    int rv = BCM_E_NONE;
    uint8 has_cw, is_first;
    uint32 push_profile;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_mpls_egress_label_t_init(&mpls_port->egress_label);

    push_profile = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
    push_profile = SOC_DPP_PUSH_PROFILE_FROM_ENCAP_ID(push_profile);

    /* Get information from HW */
    rv =  _bcm_mpls_tunnel_push_profile_info_get(unit,
                                                 push_profile,
                                                 &has_cw,
                                                 &mpls_port->egress_label);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Making sure this profile is allocated but trying to add it in test mode
       and see if it's allocated for the first time */
    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_push_profile_alloc(unit,
                                                           &push_profile,
                                                           has_cw,
                                                           &mpls_port->egress_label,
                                                           TRUE,
                                                           FALSE,
                                                           FALSE,
                                                           FALSE,
                                                           &is_first));

    if (is_first) {
        *is_allocated = FALSE;
    }
    else {
        *is_allocated = TRUE;
        mpls_port->flags |= has_cw ? BCM_MPLS_PORT_CONTROL_WORD : 0;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Push Profile allocation - verify
 */
int _bcm_dpp_mpls_port_push_profile_alloc_verify(
    int unit,
    bcm_mpls_port_t *   mpls_port)
{
    uint32 legal_flags;

    BCMDNX_INIT_FUNC_DEFS;

    if (!(mpls_port->flags2 & BCM_MPLS_PORT2_TUNNEL_PUSH_INFO)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Use BCM_MPLS_PORT2_TUNNEL_PUSH_INFO flag to allocate push profile")));
    }

    if (!(mpls_port->flags & BCM_MPLS_PORT_WITH_ID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("push profile can be allocated only WITH_ID")));
    }

    /*checking input parameters*/
    legal_flags = BCM_MPLS_PORT_WITH_ID|BCM_MPLS_PORT_CONTROL_WORD|BCM_MPLS_PORT_REPLACE;
    if (mpls_port->flags & ~legal_flags)  {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported flag set. Supported flags: BCM_MPLS_PORT_WITH_ID, BCM_MPLS_PORT_REPLACE, BCM_MPLS_PORT_CONTROL_WORD\n")));
	}


exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * allocate Push Profile
 */
int _bcm_dpp_mpls_port_push_profile_alloc(
    int unit,
    bcm_mpls_port_t *   mpls_port)
{
    int rv = BCM_E_NONE;
    uint8 has_cw, update=0;
    uint32 push_profile, encap_id;
    bcm_mpls_port_t mpls_port_dummy;
    uint8 is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_mpls_port_push_profile_alloc_verify(unit, mpls_port);
    BCMDNX_IF_ERR_EXIT(rv);

    if (mpls_port->flags & BCM_MPLS_PORT_REPLACE){
       update = 1;
    }

    bcm_mpls_port_t_init(&mpls_port_dummy);
    mpls_port_dummy.mpls_port_id = mpls_port->mpls_port_id;

    /* Check that this profile does not exist yet */
    rv = _bcm_dpp_mpls_port_push_profile_get_internal(unit, &mpls_port_dummy, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    encap_id = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
    push_profile = SOC_DPP_PUSH_PROFILE_FROM_ENCAP_ID(encap_id);

    if (update && !is_allocated) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given push profile was not allocated yet.")));
    }

    if (!update && is_allocated) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given push profile was already allocated.")));
    }

    has_cw = (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD) ? 1:0;

    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_push_profile_alloc(unit,
                                                           &push_profile,
                                                           has_cw,
                                                           &mpls_port->egress_label,
                                                           FALSE,
                                                           TRUE,
                                                           update,
                                                           TRUE,
                                                           NULL));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Get Push Profile
 */
int _bcm_dpp_mpls_port_push_profile_get(
    int unit,
    bcm_mpls_port_t *   mpls_port)
{
    int rv = BCM_E_NONE;
    uint8 is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

    rv =_bcm_dpp_mpls_port_push_profile_get_internal(unit, mpls_port, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_allocated) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given push profile was not allocated.")));
    }

    mpls_port->flags2 |= BCM_MPLS_PORT2_TUNNEL_PUSH_INFO;

    exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    /* mpls_port_add api can be used for push profile allocation.
       In this case WITH_ID flag should be set as well as push profile sub type bit in mpls_port_id */
    if (SOC_DPP_ENCAP_ID_IS_PUSH_PROFILE(mpls_port->mpls_port_id)) {
        rv = _bcm_dpp_mpls_port_push_profile_alloc(unit, mpls_port);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_mpls_port_add(unit, vpn, mpls_port));
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_port_delete(
    int unit,
    bcm_vpn_t vpn,
    bcm_gport_t mpls_port_id)
{
    int lif;
    int is_member, is_local;
    uint32 push_profile;
    int is_last_dummy;
    bcm_mpls_port_t mpls_port_dummy;
    uint8 is_allocated;


    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    /* mpls_port_delete api can be used to free push profile allocated earlier by mpls_port_add.
       In this case push profile sub type bit in mpls_port_id should be set */
    if (SOC_DPP_ENCAP_ID_IS_PUSH_PROFILE(mpls_port_id)) {

        mpls_port_dummy.mpls_port_id = mpls_port_id;

        rv =_bcm_dpp_mpls_port_push_profile_get_internal(unit, &mpls_port_dummy, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_allocated) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given push profile was not allocated.")));
        }

        push_profile = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id);
        push_profile = SOC_DPP_PUSH_PROFILE_FROM_ENCAP_ID(push_profile);

        rv = _bcm_dpp_am_template_mpls_push_profile_free(unit,
                                                         push_profile,
                                                         &is_last_dummy);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    if (SOC_DPP_ENCAP_ID_IS_EGRESS_ONLY(mpls_port_id) || SOC_DPP_ENCAP_ID_IS_FORWARD_GROUP(mpls_port_id)) {
        if (_BCM_DPP_VPN_IS_SET(vpn)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For egress and fec objects, vpn parameter must be unset")));
        }
    }

    /* check parameters */
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);


    lif = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id);
    if (lif == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS port Id")));
    }

    
    if (SOC_DPP_ENCAP_ID_IS_EGRESS_ONLY(mpls_port_id) || SOC_DPP_ENCAP_ID_IS_FORWARD_GROUP(mpls_port_id)) {
        is_member = 1;
        is_local = 1;
    }
    else {
        /* check if the port is member in the vpn */
        rv = _bcm_dpp_vpn_is_gport_member(unit, vpn, mpls_port_id, &is_member,
                                          &is_local);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* if member then remove it */
    /* if not local also remove it, will remove only SW state */
    if (is_member || !is_local) {
        rv = _bcm_dpp_gport_delete(unit, vpn, mpls_port_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_port_delete_all(
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    int iter, in_lif_val;
    int port_val;
    int vsi;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    /* check parameters */
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    for (iter = 0; iter != -1;) {
        /* get next port */
        rv = _bcm_dpp_vpn_get_next_gport(unit, vpn, &iter, &port_val,&vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        /* no more ports in vpn */
        if (port_val == BCM_GPORT_INVALID) {
            break;
        }

        /* Don't delete default In-LIF */
        in_lif_val = iter - 1;
        if ((in_lif_val == SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop) ||
		    (in_lif_val == SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple)) {
            continue;
        }

        /* fill next port */
        rv = _bcm_dpp_gport_delete(unit, vpn, port_val);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port)
{
    int port_val;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO fec_entry[2];
    SOC_PPD_FRWRD_FEC_PROTECT_INFO protect_info;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE protect_type;
    int work_fec_id;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32 next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES], nof_entries;
    SOC_SAND_PP_DESTINATION_ID dest_id;
    int gport_type;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    int is_local;
    int encap_id;
    SOC_PPD_L2_LIF_PWE_INFO  pwe_info;
    uint8 found;
    uint32 lif_idx;
    int in_lif_id, out_lif_id;
    uint8 is_ingress, is_egress;
    _BCM_GPORT_PHY_PORT_INFO *phy_port=NULL;
    SOC_PPD_MPLS_TERM_PROFILE_INFO term_profile_info;
    _bcm_lif_type_e lif_usage;
    int mpls_port_encap_id;
    uint32 profile_orientation;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    /* mpls_port_get api can be used to get push profile allocated earlier by mpls_port_add.
       In this case push profile sub type bit in mpls_port_id should be set */
    if (SOC_DPP_ENCAP_ID_IS_PUSH_PROFILE(mpls_port->mpls_port_id)) {

        rv = _bcm_dpp_mpls_port_push_profile_get(unit, mpls_port);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    /* check parameters */
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    /* is mpls port */
    if (!BCM_GPORT_IS_MPLS_PORT(mpls_port->mpls_port_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid parameter: mpls_port_id is not an MPLS port type")));
    }

    port_val = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
    if (port_val == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid parameter: mpls_port_id is not an MPLS port")));
    }

    /* Initialize the structure */
    bcm_mpls_port_t_init(mpls_port);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id, port_val);

    /* resolve content of the mpls-port: encap-id, dest:FEC/port Type: protected/VC/AC */
    rv = _bcm_dpp_gport_resolve(unit, mpls_port->mpls_port_id, 0, &dest_id,
                                &encap_id, &gport_type);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_gport_to_lif(unit, mpls_port->mpls_port_id, &in_lif_id, &out_lif_id,
                               NULL, &is_local);
    BCMDNX_IF_ERR_EXIT(rv);

    is_ingress = (in_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;
    is_egress = (out_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;

    if ((is_egress && !is_ingress) || SOC_DPP_ENCAP_ID_IS_FORWARD_GROUP(mpls_port->mpls_port_id)) {
        if (_BCM_DPP_VPN_IS_SET(vpn)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For egress and fec objects, vpn parameter must be unset")));
        }
    }

    BCM_ENCAP_ID_SET(mpls_port->encap_id, ((is_ingress) ? in_lif_id : out_lif_id));

    if (is_ingress) {
        /* Ensure InLIF is PWE */
        BCMDNX_IF_ERR_EXIT( _bcm_dpp_lif_usage_get(soc_sand_dev_id, in_lif_id, -1, &lif_usage, NULL) );
        if (lif_usage != _bcmDppLifTypeMplsPort) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("InLIF is not PWE")));
        }

        /* Get the match parameters */
        rv = _bcm_dpp_in_lif_mpls_match_get(unit, mpls_port, in_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_am_template_lif_term_data_get(unit, in_lif_id, &term_profile_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((term_profile_info.flags & SOC_PPD_MPLS_TERM_DISCARD_TTL_0) || (term_profile_info.flags & SOC_PPD_MPLS_TERM_DISCARD_TTL_1)) {
            mpls_port->vccv_type = bcmMplsPortControlChannelTtl;
        }

    }


    /* get protection info */
    if (gport_type == _BCM_DPP_GPORT_IN_TYPE_FEC) {
        work_fec_id = _BCM_PPD_GPORT_FEC_TO_WORK_FEC(dest_id.dest_val);

        soc_sand_rv =
            soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, work_fec_id,
                                    &protect_type, &fec_entry[0],
                                    &fec_entry[1], &protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (protect_type == SOC_PPD_FRWRD_FEC_PROTECT_TYPE_NONE) {
            BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->failover_port_id, dest_id.dest_val);
        }
        else {
            if (protect_type != SOC_PPD_FRWRD_FEC_PROTECT_TYPE_PATH) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid protection type: internal error")));      /* not expected */
            }

            mpls_port->failover_id = protect_info.oam_instance_id;
            if (mpls_port->failover_id != BCM_FAILOVER_ID_LOCAL) {
                DPP_FAILOVER_TYPE_SET(mpls_port->failover_id, protect_info.oam_instance_id, DPP_FAILOVER_TYPE_FEC);
            }

            if (_BCM_PPD_GPORT_IS_WORKING_PORT_ID(dest_id.dest_val)) {
                BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->failover_port_id,
                                           _BCM_PPD_GPORT_WORK_TO_PROTECT_FEC
                                           (work_fec_id));
            }
        }

        /* In JERICHO possibly set the CASCADED flag. */
        if (SOC_IS_JERICHO(unit)) {
            _bcm_dpp_am_fec_alloc_usage usage;
            rv = bcm_dpp_am_fec_get_usage(unit, work_fec_id, &usage);
            BCMDNX_IF_ERR_EXIT(rv);

            if (usage == _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED) {
                mpls_port->flags2 |= BCM_MPLS_PORT2_CASCADED;
            }
        }
    }
    /* 1+1 protection */
    else if (gport_type == _BCM_DPP_GPORT_TYPE_MC) {
        soc_sand_rv = soc_ppd_l2_lif_pwe_get(soc_sand_dev_id, mpls_port->match_label,&lif_idx,&pwe_info,&found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("LIF PWE not found")));
        }

        DPP_FAILOVER_TYPE_SET(mpls_port->failover_id, pwe_info.protection_pointer, DPP_FAILOVER_TYPE_INGRESS);
        _BCM_MULTICAST_GROUP_SET(mpls_port->failover_mc_group, _BCM_MULTICAST_TYPE_L2, dest_id.dest_val);
    }

    mpls_port->flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;

    if (is_egress) {

        rv = _bcm_dpp_lif_usage_get(soc_sand_dev_id, -1, mpls_port->encap_id, NULL, &lif_usage);
        BCMDNX_IF_ERR_EXIT(rv);
        if (lif_usage != _bcmDppLifTypeMplsPort) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("OutLIF is not PWE")));
        }

        soc_sand_rv =
           soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                      SOC_PPD_EG_ENCAP_EEP_TYPE_LIF_EEP,
                                      mpls_port->encap_id, 1,
                                      encap_entry_info, next_eep,
                                      &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* In case of egress protection, retrieve the egress protection info
           TBD - May change to check the value that defines whether a 1/4 entry exists */
        /*
        if (encap_entry_info[0].entry_val.pwe_info.protection_pointer != 0) {
            mpls_port->egress_failover_id = encap_entry_info[0].entry_val.pwe_info.protection_pointer;
            mpls_port->egress_failover_port_id = encap_entry_info[0].entry_val.pwe_info.protection_path;
        } */


        if (!is_ingress) {
            _BCM_PETRA_L3_ITF_SET(mpls_port->egress_tunnel_if, _BCM_PETRA_L3_ITF_ENCAP, next_eep[0]);
            mpls_port->egress_label.label = encap_entry_info[0].entry_val.pwe_info.label;
        }
    }
    if (((mpls_port->failover_port_id != BCM_GPORT_TYPE_NONE) && is_ingress && !is_egress) ||
        (_BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->failover_id) && _BCM_MULTICAST_IS_SET(mpls_port->failover_mc_group)) ||
        (is_ingress && !BCM_ENCAP_REMOTE_GET(mpls_port->encap_id))) {

        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                           &(mpls_port->mpls_port_id), 
                                           (shr_htb_data_t *)(void *)&phy_port, FALSE);
        BCMDNX_IF_ERR_EXIT(rv);

        if (phy_port->type == _BCM_DPP_GPORT_IN_TYPE_EEP) {
            mpls_port->flags2 |= BCM_MPLS_PORT2_LEARN_ENCAP;
        }

        /* Get egress tunnel */
        if (phy_port->type == _BCM_DPP_GPORT_IN_TYPE_FEC_VC) {
            _BCM_PETRA_L3_ITF_SET(mpls_port->egress_tunnel_if, _BCM_PETRA_L3_ITF_FEC, phy_port->phy_gport);
        }
        else { 
            if (is_egress) {
                _BCM_PETRA_L3_ITF_SET(mpls_port->egress_tunnel_if, _BCM_PETRA_L3_ITF_ENCAP, next_eep[0]);
            }
        }
    }

    if (is_egress || _BCM_DDP_IS_CROSS_CONNECT_P2P_VPN(vpn)) {   
        /* get it from encap_id (SW-state) if PWE is not protected otherwise get it from EEP */
        if (gport_type == _BCM_DPP_GPORT_IN_TYPE_FEC_VC) {
            mpls_port->egress_label.label = _BCM_DPP_ENCAP_ID_VC_GET(encap_id);
            
        } 
        else if (gport_type == _BCM_DPP_GPORT_TYPE_MC) {
            /* egress label */
        }
        /* from EEP */
        else {                              
            if (encap_entry_info[0].entry_type != SOC_PPD_EG_ENCAP_ENTRY_TYPE_PWE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Encapsulation entry type must be PWE")));  /* has to be pwe */
            }
                
            mpls_port->egress_label.label =
                encap_entry_info[0].entry_val.pwe_info.label;
        }
    }

    if (!is_ingress && is_egress) {
        mpls_port->flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    }

    mpls_port_encap_id = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
    if (SOC_DPP_ENCAP_ID_IS_FORWARD_GROUP(mpls_port_encap_id)) {
        mpls_port->flags |= BCM_MPLS_PORT_FORWARD_GROUP;
    }

    /* network group id */
    if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 0)) {
        if ((mpls_port->flags & BCM_MPLS_PORT_NETWORK) == 0){
            mpls_port->network_group_id = 0;
        }
        else if (is_ingress) {
            soc_sand_rv = soc_ppd_l2_lif_pwe_get(unit, mpls_port->match_label, &lif_idx, &pwe_info, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            soc_sand_rv = arad_pp_profile_mgmt_if_profile_get_val_from_map(
               unit,
               SOC_PPC_PROFILE_MGMT_TYPE_INLIF,
               SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_ORIENTATION,
               pwe_info.lif_profile,
               &profile_orientation
            );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            mpls_port->network_group_id = (profile_orientation << 1) | pwe_info.orientation;
        }
        else if (is_egress) {
            soc_sand_rv = arad_pp_profile_mgmt_if_profile_get_val_from_map(
               unit,
               SOC_PPC_PROFILE_MGMT_TYPE_OUTLIF,
               SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE_ORIENTATION,
               encap_entry_info[0].entry_val.pwe_info.lif_profile,
               &profile_orientation
            );
            BCM_SAND_IF_ERR_EXIT(rv);

            mpls_port->network_group_id = profile_orientation;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_mpls_port_t * port_array,
    int *port_count)
{
    int nof_ports = 0;
    int rv = BCM_E_NONE;
    int iter, in_lif_val;
    int port_val;
    bcm_gport_t cur_gport;
    int vsi;


    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters */
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    for (iter = 0; iter != -1;) {

        /* get next port */
        rv = _bcm_dpp_vpn_get_next_gport(unit, vpn, &iter, &port_val, &vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        /* no more ports in vpn */
        if (port_val == BCM_GPORT_INVALID) {
            break;
        }

        /* Skip default In-LIF*/
        in_lif_val = iter - 1;
        if ((in_lif_val == SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop) ||
		    (in_lif_val == SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple)) {
            continue;
        }

        /* fill next port */
        cur_gport = port_val;
        port_array[nof_ports].mpls_port_id = cur_gport;

        rv = bcm_petra_mpls_port_get(unit, vpn, &port_array[nof_ports]);
        BCMDNX_IF_ERR_EXIT(rv);
        ++nof_ports;

        if (nof_ports >= port_max) {
            break;
        }
    }

    *port_count = nof_ports;

exit:
    BCMDNX_FUNC_RETURN;
}


/* bcm_mpls_port_t_init not dispatchable */

int _bcm_mpls_tunnel_push_profile_info_get(
    int unit,
    int push_profile,
    uint8 * has_cw,
    bcm_mpls_egress_label_t * label_entry)
{
    SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO profile_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(label_entry);

    soc_sand_dev_id = (unit);

    soc_sand_rv =
        soc_ppd_eg_encap_push_profile_info_get(soc_sand_dev_id, push_profile,
                                           &profile_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    label_entry->exp = profile_info.exp;
    label_entry->ttl = profile_info.ttl;
    *has_cw = profile_info.has_cw;
    label_entry->qos_map_id = 0;
    if (profile_info.remark_profile != 0) {
        BCM_INT_QOS_MAP_REMARK_SET(label_entry->qos_map_id,profile_info.remark_profile);
    }
    if (profile_info.add_entropy_label) {
        label_entry->flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
    }
      
      
      /* pipe: set TTL and EXP, and always dec TTL */
      if (profile_info.model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE) {
        label_entry->flags =
            (BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET
             | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
        
#ifdef BCM_88660
        if (SOC_IS_ARADPLUS(unit)) {
            uint8 exp_copy_mode = 0;

            /* In arad+ pipe mode can be represented by */
            /* 1. TTL_SET | EXP_COPY in exp_copy_mode mode */
            /* 2. TTL_SET | EXP_SET in !exp_copy_mode mode */
            /* So remove add EXP_COPY/SET according to the mode */
            soc_sand_rv = soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get(soc_sand_dev_id, &exp_copy_mode);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (exp_copy_mode) {
                label_entry->flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY; 
                label_entry->flags &= ~BCM_MPLS_EGRESS_LABEL_EXP_SET;
            }
            else {
                label_entry->flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET; 
                label_entry->flags &= ~BCM_MPLS_EGRESS_LABEL_EXP_COPY; 
            }
        }
         
#endif /* BCM_88660 */
      } else {                    /* uniform: copy TTL and EXP, and always dec TTL */

        label_entry->flags =
            (BCM_MPLS_EGRESS_LABEL_TTL_COPY |
             BCM_MPLS_EGRESS_LABEL_EXP_COPY |
             BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_mpls_tunnel_push_profile_alloc
 * Purpose:
 *      Allocate push profile according to given attributes and configure HW
 * Parameters:
 *      (IN)     unit           - Device Number
 *      (IN/OUT) push_profile   - Allocated push profile, IN if with_id is set
 *      (IN) cw                 - Indication if CW is set in the push profile
 *      (IN) label_entry        - Push profile parameters are given inside egress label struct
 *      (IN) is_test            - If set, the process is done without allocating the profile
 *      (IN) with_id            - If set, push_profile is allocated with id indicated by push_profile parameter
 *      (IN) update_existing_profile  - If set, the existing profile data specified by push_profile field will be updated by given parameters
 *      (OUT) is_first          - If allocated profile does not exist in the HW  yet, this parameter is set
 *  
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_mpls_tunnel_push_profile_alloc(
    int unit,
    uint32 * push_profile,
    uint8 cw,
    bcm_mpls_egress_label_t * label_entry,
    uint8 is_test,
    uint8 with_id,
    uint8 update_existing_profile,
    uint8 set_hw,
    uint8 * is_first)
{
    SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO profile_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    int first_appear;
    int tmp_push_profile = 0;
    int is_last;
    int flags = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(label_entry);
    BCMDNX_NULL_CHECK(push_profile);

    soc_sand_dev_id = (unit);

    if (update_existing_profile && is_test) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG(" Error in push profile allocation. Can not test profile updating"))); 
    }

    if (update_existing_profile && !with_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG(" Error in push profile allocation. When updating existing profile with_id flag must be set")));
    }

    SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_clear(&profile_info);

    profile_info.exp_mark_mode = SOC_PPD_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE;
    profile_info.exp = label_entry->exp;
#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
        uint8 exp_copy_mode = 0;

        if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
            /* Validity check of the pipe model flags */
            soc_sand_rv = soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get(soc_sand_dev_id, &exp_copy_mode);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if ((exp_copy_mode) && (label_entry->flags & BCM_MPLS_EGRESS_LABEL_EXP_SET)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" Configuration enables only EXP_COPY in PIPE mode. Can be changed using switch control bcmSwitchMplsPipeTunnelLabelExpSet"))); 
            }
            if ((!exp_copy_mode) && (label_entry->flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" Configuration enables only EXP_SET in PIPE mode. Can be changed using switch control bcmSwitchMplsPipeTunnelLabelExpSet"))); 
            }

            profile_info.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE;
        }
        else {
            if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" In Uniform model only EXP_COPY is suppored"))); 
            }

            profile_info.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
        }

        if ((profile_info.model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM) || (exp_copy_mode)) {
            profile_info.exp = 0;
        }
    }
    else {
#endif /* BCM_88660 */

        /* pipe: set TTL and EXP, and always dec TTL */
        if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
            profile_info.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE;
        } else {                    /* uniform: copy TTL and EXP, and always dec TTL */
            profile_info.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
        }
#ifdef BCM_88660
    }
#endif /* BCM_88660 */

    profile_info.ttl = label_entry->ttl;
    profile_info.has_cw = cw;
    profile_info.remark_profile = BCM_QOS_MAP_PROFILE_GET(label_entry->qos_map_id);

    /* Entropy label */
    if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE) {
        profile_info.add_entropy_label = TRUE;
    }      
      
    if (with_id) {
        tmp_push_profile = (int)(*push_profile);
        flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    }

    if (update_existing_profile) { 
        rv = _bcm_dpp_am_template_mpls_push_profile_exchange(unit, tmp_push_profile, &profile_info,  &is_last, &tmp_push_profile, &first_appear, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        rv = _bcm_dpp_am_template_mpls_push_profile_alloc(unit, flags, 0,
                                                          &profile_info,
                                                          &tmp_push_profile,
                                                          &first_appear);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (!is_test && first_appear && set_hw) {
        soc_sand_rv =
            soc_ppd_eg_encap_push_profile_info_set(soc_sand_dev_id, tmp_push_profile,
                                               &profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    *push_profile = (uint32) tmp_push_profile;

    if (is_test) {
        rv = _bcm_dpp_am_template_mpls_push_profile_free(unit, tmp_push_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (is_first) {
        *is_first = first_appear;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_mpls_tunnel_lif_term_mpls_port_to_flags(
      int unit,
      bcm_mpls_port_t *   mpls_port,
      uint32 *inner_flags
    )
{
    BCMDNX_INIT_FUNC_DEFS;

    *inner_flags = 0;

    if (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_CONTROL_WORD;
    }

    /* In VPN, only add Entropy label */
    if (mpls_port->flags & BCM_MPLS_PORT_ENTROPY_ENABLE) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_ENTROPY;
    }

    if (mpls_port->flags & BCM_MPLS_PORT_WITH_GAL) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_GAL;
    }

    if (mpls_port->vccv_type == bcmMplsPortControlChannelTtl) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_0;
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_1;
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_mpls_tunnel_lif_term_tunnel_switch_to_flags(
      int unit,
      bcm_mpls_tunnel_switch_t * tunnel_switch,
      uint32 *inner_flags
    )
{

    BCMDNX_INIT_FUNC_DEFS;

    *inner_flags = 0;

    if (tunnel_switch->flags & BCM_MPLS_SWITCH_FRR) {
        *inner_flags = _BCM_DPP_MPLS_LIF_TERM_FRR;
    } else if (tunnel_switch->flags & BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL) {
        *inner_flags = _BCM_DPP_MPLS_LIF_TERM_COUPLING;
    }

    /* Entropy: in case of MPLS Tunnel, add entropy and ELI labels */ 
    if (tunnel_switch->flags & (BCM_MPLS_SWITCH_ENTROPY_ENABLE)) {
        *inner_flags |= (_BCM_DPP_MPLS_LIF_TERM_ENTROPY | _BCM_DPP_MPLS_LIF_TERM_ELI);
    }

    /* Reject TTL 0/1 */
    if (tunnel_switch->flags & (BCM_MPLS_SWITCH_TRAP_TTL_0)) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_0;
    }

    if (tunnel_switch->flags & (BCM_MPLS_SWITCH_TRAP_TTL_1)) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_1;
    }

    if (tunnel_switch->flags & (BCM_MPLS_SWITCH_SKIP_ETHERNET)) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_SKIP_ETHERNET;
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) {
        if (tunnel_switch->label <= _BCM_PETRA_MPLS_MAX_RESERVED_MPLS_LABEL) {
            /* Reserved labels always have check bos on. */
            *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_CHECK_BOS;
        }
    }
#endif /* BCM_88660_A0 */

    BCM_EXIT;
    
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_mpls_tunnel_lif_term_profile_exchange(
      int unit,
      SOC_PPD_LIF_ID lif_id,
      uint32 inner_flags,
      uint8* term_profile)
{
    int rv;
    int old_template, new_template, is_allocated, is_last;
    SOC_PPD_MPLS_TERM_PROFILE_INFO term_profile_info;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    SOC_PPD_MPLS_TERM_PROFILE_INFO_clear(&term_profile_info);

    /* Has CW */
    if (inner_flags & _BCM_DPP_MPLS_LIF_TERM_CONTROL_WORD) {
        term_profile_info.flags |= SOC_PPD_MPLS_TERM_HAS_CW;
    }

    if (inner_flags & _BCM_DPP_MPLS_LIF_TERM_ENTROPY) {
        term_profile_info.nof_headers += 1;
    }

    if (inner_flags & _BCM_DPP_MPLS_LIF_TERM_GAL) {
        term_profile_info.nof_headers += 1;
    }

    if (inner_flags & _BCM_DPP_MPLS_LIF_TERM_ELI) {
        term_profile_info.nof_headers += 1;
    }

    if (inner_flags & (_BCM_DPP_MPLS_LIF_TERM_FRR | _BCM_DPP_MPLS_LIF_TERM_COUPLING)) {
        term_profile_info.ttl_exp_index = 1; 
        term_profile_info.nof_headers += 1;
    }

    if (inner_flags & (_BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_0)) {
        term_profile_info.flags |= SOC_PPD_MPLS_TERM_DISCARD_TTL_0;
    }

    if (inner_flags & (_BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_1)) {
        term_profile_info.flags |= SOC_PPD_MPLS_TERM_DISCARD_TTL_1;
    }

    if (inner_flags & (_BCM_DPP_MPLS_LIF_TERM_SKIP_ETHERNET)) {
        term_profile_info.flags |= SOC_PPD_MPLS_TERM_SKIP_ETH;
    }

#ifdef BCM_88660_A0
    term_profile_info.check_bos = 1;
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) {
        term_profile_info.check_bos = (inner_flags & _BCM_DPP_MPLS_LIF_TERM_CHECK_BOS) ? 1 : 0;
    }
#endif /* BCM_88660_A0 */
    
    rv = _bcm_dpp_am_template_lif_term_profile_exchange(unit,lif_id,&term_profile_info, &old_template, &is_last, &new_template, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    if (term_profile != NULL) {
        *term_profile = new_template;
    }

    if (is_allocated) {
        /* Update HW */
        soc_sand_rv = soc_ppd_mpls_term_profile_info_set(soc_sand_dev_id, new_template, &term_profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}
      



int _bcm_mpls_tunnel_bcm_from_ppd(
    int unit,
    bcm_mpls_egress_label_t * label_entry,
    SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO * tunnel_info)
{
    uint8 has_cw;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(label_entry);
    BCMDNX_NULL_CHECK(tunnel_info);

    label_entry->label = tunnel_info->tunnel_label;
    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_push_profile_info_get(unit,
                                                  tunnel_info->push_profile, 
                                                  &has_cw, /* Not intersting field */
                                                  label_entry));
    label_entry->action = BCM_MPLS_EGRESS_ACTION_PUSH;
    label_entry->flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
exit:
    BCMDNX_FUNC_RETURN;
}




int _bcm_mpls_tunnel_bcm_to_ppd(
    int unit,
    bcm_mpls_egress_label_t * label_entry,
    uint8 has_cw,
    SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO * tunnel_info)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(label_entry);
    BCMDNX_NULL_CHECK(tunnel_info);
    /* fill label */
    tunnel_info->tunnel_label = label_entry->label;
    /* allocate push profile to hold TTL/EXP and model */
    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_push_profile_alloc(unit,
                                               &tunnel_info->push_profile,
                                               has_cw,
                                               label_entry,
                                               FALSE,
                                               FALSE,
                                               FALSE,
                                               TRUE,
                                               NULL));
    
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_mpls_tunnel_switch_egress_label_to_mpls_command(
    int unit,
    bcm_mpls_egress_label_t * label_entry,
    SOC_PPC_MPLS_COMMAND * command_info)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(label_entry);
    BCMDNX_NULL_CHECK(command_info);
    /* fill label */
    command_info->label = label_entry->label;
    command_info->has_cw = FALSE;    
    /* allocate push profile to hold TTL/EXP and model */
    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_push_profile_alloc(unit,
                                               &command_info->push_profile,
                                               FALSE,
                                               label_entry,
                                               FALSE,
                                               FALSE,
                                               FALSE,
                                               TRUE,
                                               NULL));
    
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_mpls_egress_tunnel_is_bind_entries(
    int unit,
    bcm_mpls_egress_label_t * label_array,
    int start_indx,
    int *is_bind_entries)
{
    BCMDNX_INIT_FUNC_DEFS;

    *is_bind_entries = 0;

    /* Case of PUSH */
    if ((start_indx != 1) && /* In case start indx is 1, no case of two consecutive labels (it is the last label in stack) */
        (label_array[start_indx-1].tunnel_id == label_array[start_indx-2].tunnel_id) && /* tunnel id is the same */
        ((label_array[start_indx-1].flags & 
          (BCM_MPLS_EGRESS_LABEL_EXP_SET | BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_TTL_SET))) && /* not push 1 */
        (((label_array[start_indx-2].flags & 
          (BCM_MPLS_EGRESS_LABEL_EXP_SET | BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_TTL_SET)))) ) /* not push 2 */ {
    
       /* special case is true */
       /* Note: For Arad+ the EXP_SET flag is illegal, but since this is a superset of the legal flags, there is no need for change. */
       *is_bind_entries = 1;      
    } 

    /* Case of SWAP_COUPLING */
    if ((start_indx != 1) && /* In case start indx is 1, no case of two consecutive labels (it is the last label in stack) */
        (label_array[start_indx-2].tunnel_id == 0) && /* Tunnel ID of NOP action must be 0 */
        (label_array[start_indx-2].flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID && label_array[start_indx-2].action == BCM_MPLS_EGRESS_ACTION_NOP) && /* NOP action on the first label */
        (label_array[start_indx-1].flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID && label_array[start_indx-1].action == BCM_MPLS_EGRESS_ACTION_SWAP)) { /* SWAP action on the second label */
      *is_bind_entries = 1;
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_mpls_egress_tunnel_to_egress_action_type(
    int unit,
    bcm_mpls_egress_label_t * label_array,
    int index,
    _bcm_dpp_mpls_egress_action_type_t *action_type)
{

    BCMDNX_INIT_FUNC_DEFS;

    
    if (label_array[index].flags & (BCM_MPLS_EGRESS_LABEL_ACTION_VALID)) {
        /* Explicit */
        if (label_array[index-1].action == BCM_MPLS_EGRESS_ACTION_NOP &&  /* Coupling */
            label_array[index].flags & (BCM_MPLS_EGRESS_LABEL_ACTION_VALID) &&
            label_array[index].action == BCM_MPLS_EGRESS_ACTION_SWAP) {
            *action_type = _bcm_dpp_mpls_egress_action_type_swap_coupling;
        } else if (label_array[index].action == BCM_MPLS_EGRESS_ACTION_SWAP) {
            *action_type = _bcm_dpp_mpls_egress_action_type_swap;
        } else if (label_array[index].action == BCM_MPLS_EGRESS_ACTION_PHP) {
            *action_type = _bcm_dpp_mpls_egress_action_type_pop;
        } else if (label_array[index].action == BCM_MPLS_EGRESS_ACTION_PUSH) {
            *action_type = _bcm_dpp_mpls_egress_action_type_push;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given label index %d without explicit operation flags 0x%x actions 0x%x"), index,label_array[index].flags,label_array[index].action)); 
        }
        
    } else {
        /* Implicit */
        /* Action POP MPLS is invalid for implicit */
        if ((label_array[index].flags & (BCM_MPLS_EGRESS_LABEL_PHP_IPV4|BCM_MPLS_EGRESS_LABEL_PHP_IPV6|BCM_MPLS_EGRESS_LABEL_PHP_L2))) {
            *action_type = _bcm_dpp_mpls_egress_action_type_pop;
        } else if ((label_array[index].flags & (BCM_MPLS_EGRESS_LABEL_EXP_SET | BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_TTL_SET))) {
            /* Note: For Arad+ the EXP_SET flag is illegal, but since the above is a superset of the legal flags, there is no need for change. */
            *action_type = _bcm_dpp_mpls_egress_action_type_push;
        } else {
            /* No pop and no push then Swap */
            *action_type = _bcm_dpp_mpls_egress_action_type_swap;
        }
    }  

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_tunnel_free_push_profile(
    int unit,
    int eep)
{
    int indx, cur_count, cur_indx;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32 next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES], nof_entries;
    int is_last;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, eep, 0,
                               encap_entry_info, next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    for (indx = 0; indx < nof_entries; ++indx) {
        if (encap_entry_info[indx].entry_type ==
            SOC_PPD_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) {
            cur_count =
                encap_entry_info[indx].entry_val.mpls_encap_info.nof_tunnels;
            for (cur_indx = 0; cur_indx < cur_count; ++cur_indx) {
                rv = _bcm_dpp_am_template_mpls_push_profile_free(unit,
                                                                 encap_entry_info
                                                                 [indx].
                                                                 entry_val.
                                                                 mpls_encap_info.
                                                                 tunnels
                                                                 [cur_indx].
                                                                 push_profile,
                                                                 &is_last);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            break;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_port_free_push_profile(
    int unit,
    int eep)
{
    int cur_count;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32 next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES], nof_entries;
    int is_last;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(unit,
                               SOC_PPD_EG_ENCAP_EEP_TYPE_LIF_EEP, eep, 1,
                               encap_entry_info, next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (encap_entry_info[0].entry_type != SOC_PPD_EG_ENCAP_ENTRY_TYPE_PWE) {
	    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error, EEDB entry is not PWE TYPE"))); 
    }

    cur_count = encap_entry_info[0].entry_val.mpls_encap_info.nof_tunnels;
    if (cur_count > 1) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error, deleting PWE entry with MPLS entry"))); 
    }
    
    rv = _bcm_dpp_am_template_mpls_push_profile_free(unit,
                                                     encap_entry_info[0].
                                                     entry_val.
                                                     pwe_info.
                                                     push_profile,
                                                     &is_last);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC 
int _bcm_petra_mpls_tunnel_switch_egress_label_from_pop_info(
    int unit,
    bcm_mpls_egress_label_t * egress_label,    
    SOC_PPD_EG_ENCAP_POP_INFO *mpls_pop_info)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_PETRAB(unit)) {
        if (mpls_pop_info->pop_type == SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET) {
            egress_label->flags |= BCM_MPLS_EGRESS_LABEL_PHP_L2;
        }
        else if(mpls_pop_info->pop_type == SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE
                || mpls_pop_info->pop_type == SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM) {
            egress_label->flags |= BCM_MPLS_EGRESS_LABEL_PHP_IPV4;
        }
        else if(mpls_pop_info->pop_type == SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE
                || mpls_pop_info->pop_type == SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM) {
            egress_label->flags |= BCM_MPLS_EGRESS_LABEL_PHP_IPV6;
        }

        if (mpls_pop_info->pop_type == SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM) {
            egress_label->flags |= (BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
        } else {
            egress_label->flags |= (BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
        }       
    } else {
        /* ARAD */
        if (mpls_pop_info->pop_next_header == SOC_TMC_PKT_FRWRD_TYPE_BRIDGE) {
            egress_label->flags |= BCM_MPLS_EGRESS_LABEL_PHP_L2;
        }
        else if(mpls_pop_info->pop_next_header == SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC) {
            egress_label->flags |= BCM_MPLS_EGRESS_LABEL_PHP_IPV4;             
        }
        else if(mpls_pop_info->pop_next_header == SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC) {
            egress_label->flags |= BCM_MPLS_EGRESS_LABEL_PHP_IPV6;
        }
          
        if (mpls_pop_info->model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE) {
            egress_label->flags |= (BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET
             | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
        } else {
            egress_label->flags |= (BCM_MPLS_EGRESS_LABEL_TTL_COPY |
             BCM_MPLS_EGRESS_LABEL_EXP_COPY |
             BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
        }        
    }

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
         if (mpls_pop_info->model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE) {
                egress_label->flags &= ~BCM_MPLS_EGRESS_LABEL_EXP_SET;
                egress_label->flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            } 
    }
 
#endif /* BCM_88660 */

    egress_label->flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    egress_label->action = BCM_MPLS_EGRESS_ACTION_PHP;
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC 
int _bcm_petra_mpls_tunnel_switch_egress_label_to_pop_info(
    int unit,
    bcm_mpls_egress_label_t * egress_label,
    int vsi,
    SOC_PPD_EG_ENCAP_POP_INFO *mpls_pop_info)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_PETRAB(unit)) {
        if(egress_label->flags & BCM_MPLS_EGRESS_LABEL_PHP_L2) {
            mpls_pop_info->pop_type = SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET;
            
            mpls_pop_info->ethernet_info.has_cw = FALSE; /* need flag to bcm*/
            mpls_pop_info->ethernet_info.tpid_profile = 0; /* set using bcm_port_tpid with gport = tunnel = encap*/
            mpls_pop_info->ethernet_info.out_vsi = vsi;
        }
        else if(egress_label->flags & BCM_MPLS_EGRESS_LABEL_PHP_IPV4) {
            mpls_pop_info->pop_type = SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE;
        }
        else if(egress_label->flags & BCM_MPLS_EGRESS_LABEL_PHP_IPV6) {
            mpls_pop_info->pop_type = SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE;
        }

        if (egress_label->flags & ((BCM_MPLS_EGRESS_LABEL_TTL_COPY |
             BCM_MPLS_EGRESS_LABEL_EXP_COPY |
             BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT))) {
            /* uniform: copy TTL and EXP, and always dec TTL */
            mpls_pop_info->pop_type += 1;
        }
    } else {
        /* ARAD */
        mpls_pop_info->pop_type = SOC_TMC_MPLS_COMMAND_TYPE_POP;
        if(egress_label->flags & BCM_MPLS_EGRESS_LABEL_PHP_L2) {
            mpls_pop_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
            
            mpls_pop_info->ethernet_info.has_cw = FALSE; /* need flag to bcm*/
            mpls_pop_info->ethernet_info.tpid_profile = 0; /* set using bcm_port_tpid with gport = tunnel = encap*/
            mpls_pop_info->ethernet_info.out_vsi = vsi;
        }
        else if(egress_label->flags & BCM_MPLS_EGRESS_LABEL_PHP_IPV4) {
            mpls_pop_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC; 
        }
        else if(egress_label->flags & BCM_MPLS_EGRESS_LABEL_PHP_IPV6) {
            mpls_pop_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC; 
        } else {
            mpls_pop_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_MPLS; 
        }

        /* Enough to check only one of them (TTL/EXP since on verify it checks both) */
        if (egress_label->flags & (BCM_MPLS_EGRESS_LABEL_TTL_SET)) {
            mpls_pop_info->model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE;
        }
        else if (egress_label->flags & (BCM_MPLS_EGRESS_LABEL_TTL_COPY)) {
            /* uniform: copy TTL and EXP, and always dec TTL */
            mpls_pop_info->model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
        }
    }
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_tunnel_initiator_data_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array,
    int vsi_param, /* needed only in case intf is of type EEP*/
    int ll_eep_param, /* needed only in case intf is of type EEP*/
    int is_swap, /* needed only in case intf is of type EEP*/
    int *tunnel_eep
    ) /* needed only in case intf is of type EEP*/
{
    int eep = 0, flags = 0, phase_stage, start_indx, skip_entries, is_bind_entries;
    SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO mpls_encap_info;
    SOC_PPD_EG_ENCAP_POP_INFO mpls_pop_info;
    SOC_PPD_EG_ENCAP_SWAP_INFO mpls_swap_info;
    SOC_PPD_EG_ENCAP_DATA_INFO mpls_data_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    int eep_alloced=0;
    _bcm_dpp_mpls_egress_action_type_t label_action_type;
    int indx;
    SOC_PPD_LIF_ID lif_id;
    int ll_eep = 0, tmp_indx = 0;
    int vsi = 0;
    SOC_PPD_EG_ENCAP_ENTRY_INFO   encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32         next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32         nof_entries;
    uint8 nof_replace_flags = 0;
    /* If 1 then mode flag validation should be done*/
    int mode_flag_validation = 1;
    bcm_mpls_egress_label_t existing_label_entry;
    uint8 has_cw = 0;
    int first_encap_push_profile = 0;
    int validate_pwe_replace = 0, pwe_label_read = 0;
    _bcm_lif_type_e usage;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(label_array);

    soc_sand_dev_id = (unit);
    SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO_clear(&mpls_encap_info);
    SOC_PPD_EG_ENCAP_POP_INFO_clear(&mpls_pop_info);
    SOC_PPD_EG_ENCAP_SWAP_INFO_clear(&mpls_swap_info);
    SOC_PPD_EG_ENCAP_DATA_INFO_clear(&mpls_data_info);
    SOC_PPD_EG_ENCAP_ENTRY_INFO_clear(encap_entry_info);  

    /* verify paramters */        
    if ((num_labels < 0) || (num_labels > 4)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("support up to four labels")));
    }

    for (indx = 0; indx < num_labels; ++indx) {
        if (label_array[indx].exp > 7) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("EXP value %d in label array index %d is out of range"), label_array[indx].exp, indx));
        }
        if (label_array[indx].label > 0xfffff) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Label value is out of range in label array index %d"),indx));
        }
                
        if (label_array[indx].pkt_pri != 0
            || label_array[indx].pkt_cfi != 0
            || (SOC_IS_PETRAB(unit) && label_array[indx].qos_map_id != 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("pkt_pri, pkt_cfi must be 0 label array index %d"),indx));
        }

#ifdef BCM_88660
        if (SOC_IS_ARADPLUS(unit)) {
            mode_flag_validation = 0;
        }
#endif /* BCM_88660 */

        if (mode_flag_validation != 0) {
            /* if both ttl and exp present then have to be same */
            /* ttl decrement ignored */
            if ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Misconfiguration cant be combination of TTL SET - Pipe and EXP Copy - Uniform label array index %d"),indx));
            }

            if ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_TTL_COPY) && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_EXP_SET)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Misconfiguration cant be combination of TTL Copy - Uniform and EXP Set - Pipe label array index %d"),indx));
            }        
        }

        if(label_array[indx].flags & (BCM_MPLS_EGRESS_LABEL_DROP|BCM_MPLS_EGRESS_LABEL_PRI_REMARK
                                      |BCM_MPLS_EGRESS_LABEL_PRI_SET)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flags 0x%x not supported label array index %d"),label_array[indx].flags,indx));
        }

        if (SOC_IS_PETRAB(unit) && label_array[indx].flags & (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Entropy flag is not supported by Soc_petra-B, label array flags 0x%x label array index %d"),label_array[indx].flags,indx));
        }

        if (label_array[indx].qos_map_id != 0 && !BCM_INT_QOS_MAP_IS_REMARK(label_array[indx].qos_map_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("QOS MAP ID provided %d is not supported label array index %d"),label_array[indx].qos_map_id,indx));
        }

        if (BCM_QOS_MAP_PROFILE_GET(label_array[indx].qos_map_id) >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("QOS MAP ID provided %d is higher than device support, label array index %d"),label_array[indx].qos_map_id,indx));
        }

        if ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) && !(label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If BCM_MPLS_EGRESS_LABEL_REPLACE flag is set, then BCM_MPLS_EGRESS_LABEL_WITH_ID flag must also be set. label_array index: %d"), indx));
        }

        if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE){
            /* Check that OutLIF is MPLS tunnel or PWE in innermost label (in SW DB) */
            eep = _BCM_PETRA_L3_ITF_VAL_GET(label_array[indx].tunnel_id);
            BCMDNX_IF_ERR_EXIT( _bcm_dpp_lif_usage_get(unit, -1, eep, NULL, &usage) );
            if (usage != _bcmDppLifTypeMplsTunnel) {
                if ((indx == 0) && (usage == _bcmDppLifTypeMplsPort)) {
                    /* Replace request for PWE outermost label */
                    validate_pwe_replace = 1; 
                } else if ((indx == 1) && (usage == _bcmDppLifTypeMplsPort)
                           && (eep == _BCM_PETRA_L3_ITF_VAL_GET(label_array[0].tunnel_id))) {
                    /* Replace request for PWE second outermost label when using the same EEP */
                    validate_pwe_replace = 1;
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Wrong outLIF type to REPLACE in label %d (%d)"),indx, usage)); 
                }
            }
            nof_replace_flags++;
        }

        if ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID) && (_BCM_PETRA_L3_ITF_VAL_GET(label_array[indx].tunnel_id) == 0)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If BCM_MPLS_EGRESS_LABEL_WITH_ID is set, tunnel_id must be different than 0. label_array index: %d"), indx));
        }
    }

    if (nof_replace_flags > 0 && nof_replace_flags != num_labels ){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If BCM_MPLS_EGRESS_LABEL_REPLACE flag is set in one label, it must be set in all labels.")));
    }

    if (validate_pwe_replace) {
        /* A PWE label is being replaced. Make sure only permitted fields are changed */

        /* Read label 0 (The only one that can be PWE */
        bcm_mpls_egress_label_t_init(&existing_label_entry);
        rv = bcm_petra_mpls_tunnel_initiator_get(unit, label_array[0].tunnel_id, 1, &existing_label_entry, &pwe_label_read);
        BCMDNX_IF_ERR_EXIT(rv);

        if (pwe_label_read < 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Didn't get the PWE label to REPLACE.")));
        }

        /* Validate all fields except l3_intf_id */
        if (
            (existing_label_entry.label != label_array[0].label)
            || (existing_label_entry.qos_map_id  != label_array[0].qos_map_id)
            || (existing_label_entry.exp         != label_array[0].exp)
            || (existing_label_entry.ttl         != label_array[0].ttl)
            || (existing_label_entry.pkt_pri     != label_array[0].pkt_pri)
            || (existing_label_entry.pkt_cfi     != label_array[0].pkt_cfi)
            || (existing_label_entry.action      != label_array[0].action)
            || (existing_label_entry.tunnel_id   != label_array[0].tunnel_id)
            ) 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Can only change l3_intf_id in PWE label REPLACE.")));
        }
    }

    phase_stage = -1; 
    /* Run over all label stack. from end to start */
    for (start_indx = num_labels; start_indx > 0; start_indx = start_indx - 1 - skip_entries) {
        phase_stage++;

        /* Special cases to handle two entries as one entry in EEDB:
           1. In case of two consecutive labels are push and have same tunnel ID than bind them to one entry in HW 
           2. In case of SWAP coupling: one is NOP the other is SWAP Coupling */
        rv = _bcm_mpls_egress_tunnel_is_bind_entries(unit, label_array, start_indx, &is_bind_entries);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_bind_entries) {
           skip_entries = 1;
           indx = start_indx - 2;

           /* In case of SWAP_COUPLING, use the tunnel_id of second entry. */
           if ((start_indx != 1) && /* In case start indx is 1, no case of two consecutive labels (it is the last label in stack) */
               (label_array[start_indx-2].tunnel_id == 0) && /* Tunnel ID of NOP action must be 0 */
               (label_array[start_indx-2].flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID && label_array[start_indx-2].action == BCM_MPLS_EGRESS_ACTION_NOP) && /* NOP action on the first label */
               (label_array[start_indx-1].flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID && label_array[start_indx-1].action == BCM_MPLS_EGRESS_ACTION_SWAP)) {
               indx = start_indx - 1;
           }
        } else {
           skip_entries = 0;
           indx = start_indx - 1;
        }  

        rv = _bcm_mpls_egress_tunnel_to_egress_action_type(unit, label_array, indx, &label_action_type);
        BCMDNX_IF_ERR_EXIT(rv);

        /* EEP allocation { */
        eep = _BCM_PETRA_L3_ITF_VAL_GET(label_array[indx].tunnel_id);
        
        if ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID) && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)) {
            /* free usage of push profiles */
            rv = bcm_petra_mpls_tunnel_free_push_profile(unit, eep);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (eep != 0) { /* Allocate or Already exist */
            flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
            rv = bcm_dpp_am_mpls_eep_is_alloced(unit,eep);
            if (rv == BCM_E_PARAM) {
                BCMDNX_IF_ERR_EXIT(rv);
            }

            eep_alloced = (rv == BCM_E_EXISTS)?1:0;     

            /* Can't replace non - existing tunnel */
            if (!eep_alloced && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Tunnel id to be replaced %d does not exist, label array index %d"),label_array[indx].tunnel_id,indx));
            }

            /* Can't replace an existing tunnel with no replace flag*/
            if (eep_alloced && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID) && !(label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Tunnel id %d already exists but replace flag is absent, label array index %d. "),label_array[indx].tunnel_id,indx));
            }
        } else {                    /* if not tunneled alloc new eep */
            flags = 0;
            eep_alloced = 0;
        }

        if(!eep_alloced) {
            /* 
             * allocate entry, from MPLS encap DB, to be used for swap/pop/push 
             * phase_stage indicates if it is the first or second entry. 
             * last entry points to LL encap.
             */
            if ((indx+is_bind_entries+1 == num_labels) && _BCM_PETRA_L3_ITF_IS_ENCAP(label_array[indx+is_bind_entries].l3_intf_id)) { /* tunnel points to tunnel */
                phase_stage++;
            }

            if (phase_stage > 1) { /* no more than two rounds i.e. only two entries are possible */
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Too many mpls labels are in stack. Supported up to two mpls entries")));
            }

            if (SOC_IS_PETRAB(unit)) {
                  /* start allocation from LIF to ensure synced */
                  /* mark LIF as allocated, so PWE cannot be allocated in this place.  */
                  /* In ARAD, no need for such synced */
                  if(_Bcm_dpp_sync_lif_eep) {
                      lif_id = eep;
                      rv = bcm_dpp_am_l2_vpn_pwe_alloc(unit, _BCM_DPP_AM_L2_PWE_TERM_DEFAULT, flags, 2, &lif_id, NULL);
                      BCMDNX_IF_ERR_EXIT(rv);
              
                      flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
                      eep = lif_id;
                  }
            }

            /* Allocate simple or second mpls tunnel */
            rv = bcm_dpp_am_mpls_eep_alloc(unit, ((phase_stage == 0)? _BCM_DPP_AM_MPLS_EEP_TYPE_SIMPLE_MPLS_TUNNEL:_BCM_DPP_AM_MPLS_EEP_TYPE_SECOND_MPLS_TUNNEL), flags, 1, &eep);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        _BCM_PETRA_L3_ITF_SET(label_array[indx].tunnel_id,_BCM_PETRA_L3_ITF_ENCAP,eep);
        if (is_bind_entries) {
            _BCM_PETRA_L3_ITF_SET(label_array[indx+1].tunnel_id,_BCM_PETRA_L3_ITF_ENCAP,eep);
        }
        /* EEP allocation } */

        if (!(label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)) {
            /* Update sw db */
            rv = _bcm_dpp_out_lif_mpls_tunnel_match_add(soc_sand_dev_id, eep);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* fill EEP { */

        /* fill EEP with next pointer information { */
        nof_entries = 0;
        if (eep_alloced) {
            soc_sand_rv =
              soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                     SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                     eep, 1,
                                     encap_entry_info, next_eep,
                                     &nof_entries);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

              first_encap_push_profile = encap_entry_info[0].entry_val.mpls_encap_info.tunnels[0].push_profile;
        } 
        

        if (nof_entries > 0 && 
            (!_BCM_DPP_MPLS_EEDB_TYPES(encap_entry_info[0].entry_type)) && /* Not MPLS types */
            encap_entry_info[0].entry_type != SOC_PPD_EG_ENCAP_ENTRY_TYPE_PWE && /* Not PWE types */
            encap_entry_info[0].entry_type != SOC_PPD_EG_ENCAP_ENTRY_TYPE_NULL && /* Not defined */
            !(SOC_PPD_EG_ENCAP_DATA_COUPLING_SWAP_TYPE(soc_sand_dev_id,encap_entry_info[0].entry_type,&(encap_entry_info[0].entry_val.data_info)))) { /* Not Coupling-Swap */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tunnel ID given is already allocated with type that is different than MPLS")));
        }

        /* check if there is tunnel associated with intf at all */
        if (((indx+1+is_bind_entries) == num_labels) & _BCM_PETRA_L3_ITF_IS_ENCAP(label_array[indx+is_bind_entries].l3_intf_id)) { /* Special case: interface in last label is Tunnel */
            next_eep[0] = (_BCM_PETRA_L3_ITF_VAL_GET(label_array[indx+is_bind_entries].l3_intf_id));
            vsi = 0;
        } else if (((indx+1+is_bind_entries) == num_labels)) { /* last entry it should point to ll */
            next_eep[0] = ll_eep;
            vsi = _BCM_PETRA_L3_ITF_VAL_GET(label_array[indx+is_bind_entries].l3_intf_id);
        } else { /* take from stack in case of not last */
            next_eep[0] = (_BCM_PETRA_L3_ITF_VAL_GET(label_array[indx+1+is_bind_entries].tunnel_id)); 
            vsi = 0;
        }
        /* fill EEP with next pointer information } */
        

        /* fill EEP with encap information, include oam_lif_set and drop for repleaced entries. { */
        if (label_action_type == _bcm_dpp_mpls_egress_action_type_swap) { /* if swap */
            mpls_swap_info.out_vsi = vsi;
            mpls_swap_info.swap_label = label_array[indx].label;


            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
                mpls_swap_info.oam_lif_set = encap_entry_info[0].entry_val.swap_info.oam_lif_set;
                mpls_swap_info.drop = encap_entry_info[0].entry_val.swap_info.drop;
            }

            soc_sand_rv =
                soc_ppd_eg_encap_swap_command_entry_add(soc_sand_dev_id, eep, &mpls_swap_info, next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        } else if(label_action_type == _bcm_dpp_mpls_egress_action_type_pop) { /* if pop */
            rv = _bcm_petra_mpls_tunnel_switch_egress_label_to_pop_info(unit, &(label_array[indx]), vsi, 
                                                           &(mpls_pop_info));
            BCM_IF_ERROR_RETURN(rv);

            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
                mpls_pop_info.oam_lif_set = encap_entry_info[0].entry_val.pop_info.oam_lif_set;
                mpls_pop_info.drop = encap_entry_info[0].entry_val.pop_info.drop;
            }

            soc_sand_rv =
                soc_ppd_eg_encap_pop_command_entry_add(soc_sand_dev_id, eep, &mpls_pop_info, next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        } else if (label_action_type == _bcm_dpp_mpls_egress_action_type_swap_coupling) { /* if swap coupling */
            SOC_PPD_EG_ENCAP_DATA_COUPLING_SWAP_FORMAT_SET(soc_sand_dev_id, 0x8848, label_array[indx].label, &mpls_data_info);

            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
                mpls_data_info.oam_lif_set = encap_entry_info[0].entry_val.data_info.oam_lif_set;
                mpls_data_info.drop = encap_entry_info[0].entry_val.data_info.drop;
            }

            /* add enry to allocated place */
            soc_sand_rv = 
                soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id, eep, &mpls_data_info, TRUE, next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        } else { /* if encap push */

            mpls_encap_info.nof_tunnels = 1+is_bind_entries;
            if (SOC_IS_PETRAB(unit)) {
                mpls_encap_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
            }

            /* Set information for the first tunnel. Check if has_cw is set in push profile.
             * If it does this label is VC label and the new push profile will have CW as well. */
            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
                rv = _bcm_mpls_tunnel_push_profile_info_get(unit,
                                                  first_encap_push_profile, 
                                                  &has_cw,
                                                  &existing_label_entry);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                has_cw = 0;
            }

            rv = _bcm_mpls_tunnel_bcm_to_ppd(unit, &label_array[indx], has_cw,
                                              &mpls_encap_info.tunnels[0]);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Set information of the other tunnels if exist */
            for (tmp_indx = 1; tmp_indx < mpls_encap_info.nof_tunnels; ++tmp_indx) {
                rv = _bcm_mpls_tunnel_bcm_to_ppd(unit, &label_array[indx+tmp_indx], 0 /*has_cw*/,
                                                  &mpls_encap_info.tunnels[tmp_indx]);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
                mpls_encap_info.oam_lif_set = encap_entry_info[0].entry_val.mpls_encap_info.oam_lif_set;
                mpls_encap_info.drop = encap_entry_info[0].entry_val.mpls_encap_info.drop;
            }

            /* set VSI for last EEP */
            mpls_encap_info.out_vsi =  vsi;
            /* add encap entry */
            soc_sand_rv =
                soc_ppd_eg_encap_mpls_entry_add(soc_sand_dev_id, eep, &mpls_encap_info, next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

#ifdef BCM_ARAD_SUPPORT
            /* In ARAD: orietntation is being set in different API, per outlif. */
            if (SOC_IS_ARAD(unit)) {
                if (SOC_DPP_CONFIG(unit)->arad->pp_op_mode.split_horizon_filter_enable) {
                    soc_sand_rv =
                      soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(soc_sand_dev_id, eep, SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
#endif /* BCM_ARAD_SUPPORT */

        }
        /* fill EEP with encap information { */
    }
        
    *tunnel_eep = eep;


exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
     BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API no longer supported. Please use bcm_petra_mpls_tunnel_initiator_create instead.")));

  exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    int tunnel_eep;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (intf != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("intf parameter must be set to 0")));
    }

    rv = bcm_petra_mpls_tunnel_initiator_data_set(
        unit,
        intf,
        num_labels,
        label_array,
        0,  /* vsi */
        0,  /* ll-eep */
        0, /* is-swap */
        &tunnel_eep);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_tunnel_initiator_create(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    rv = _bcm_petra_mpls_tunnel_initiator_set(unit, intf, num_labels, label_array);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
  exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


/* note after this user has to call egress-object create with id/update */
int bcm_petra_mpls_tunnel_initiator_clear(
    int unit,
    bcm_if_t intf)
{
    int eep, is_tunneled;
    unsigned int soc_sand_dev_id;
    int rv = BCM_E_NONE;
    int vsi, ll_eep;
    SOC_PPD_FEC_ID fec_id;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE
      protect_type;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO
      working_fec;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO
      protect_fec;
    SOC_PPD_FRWRD_FEC_PROTECT_INFO
      protect_info;
    int encap_itf;
    uint8 success;
    uint32 soc_sand_rv;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];    
    uint32 next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES], nof_entries;    
    _bcm_lif_type_e usage = 0;


    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    /* check if allocated eep */
    if (_BCM_PETRA_L3_ITF_IS_ENCAP(intf)) {
        eep = _BCM_PETRA_L3_ITF_VAL_GET(intf);
        rv = bcm_dpp_am_mpls_eep_is_alloced(unit,eep);
        if (rv != BCM_E_EXISTS) {
            BCM_EXIT;
        }
    }

    /* check if there is tunnel associated with intf at all */
    rv = _bcm_tunnel_intf_is_tunnel(unit, intf, &is_tunneled, &eep, &vsi, &ll_eep);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * Dont remove PWE entries
     */ 
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_usage_get(unit, -1, eep, NULL, &usage));
    if (usage != _bcmDppLifTypeMplsTunnel) { /* OutLIF is not MPLS tunnel - nothing to do */
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OutLIF is not MPLS tunnel")));
    } 

    /* if not then no labels */
    if (is_tunneled == 0) {
        BCM_EXIT;
    }

    /* free push profile used by these tunnel */
    rv = bcm_petra_mpls_tunnel_free_push_profile(unit, eep);
    BCMDNX_IF_ERR_EXIT(rv);

    /* remove tunnel */
    if(_BCM_PETRA_L3_ITF_IS_RIF(intf)) {
       /* mark this interface as not tunneled */
        
        rv = _bcm_tunnel_intf_eep_set(unit, intf, 0);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if(_BCM_PETRA_L3_ITF_IS_FEC(intf)) {
        _bcm_l3_intf_to_fec(unit, intf, &fec_id);

        soc_sand_rv =
            soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, fec_id,
                                    &protect_type, &working_fec,
                                    &protect_fec, &protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        _BCM_PETRA_L3_ITF_SET(encap_itf, _BCM_PETRA_L3_ITF_ENCAP,working_fec.eep);
        rv = _bcm_tunnel_intf_is_tunnel(unit,encap_itf,  &is_tunneled, &eep, &vsi, &ll_eep);
        BCMDNX_IF_ERR_EXIT(rv);

        working_fec.eep = ll_eep;

        working_fec.app_info.out_rif = vsi;
        soc_sand_rv =
            soc_ppd_frwrd_fec_entry_add(soc_sand_dev_id, fec_id,
                                    protect_type, &working_fec,
                                    &protect_fec, &protect_info, &success);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    } else {
        soc_sand_rv =
            soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                               eep, 1,
                               encap_entry_info, next_eep,
                               &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (nof_entries > 0 && 
            (_BCM_DPP_MPLS_EEDB_TYPES(encap_entry_info[0].entry_type) || 
             SOC_PPD_EG_ENCAP_DATA_COUPLING_SWAP_TYPE(soc_sand_dev_id,encap_entry_info[0].entry_type,&(encap_entry_info[0].entry_val.data_info)))) {

            soc_sand_rv = soc_ppd_eg_encap_entry_remove(soc_sand_dev_id,
                               SOC_PPD_EG_ENCAP_EEP_TYPE_LIF_EEP,
                               eep);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);            
        } else {
            /* Nothing to do */
            BCM_EXIT;
        }
    }

    /* free memory */
    if (SOC_IS_PETRAB(unit)) {
      rv = bcm_dpp_am_l2_vpn_pwe_lif_dealloc(unit, eep);
      BCMDNX_IF_ERR_EXIT(rv);
    }
    
    rv = bcm_dpp_am_mpls_eep_dealloc(unit, eep);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Remove entry from SW DB */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_out_lif_match_delete(unit,eep));

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_tunnel_initiator_clear_all(
    int unit)
{
    int indx;
    int rv = BCM_E_NONE;
    bcm_if_t eep;
    _bcm_lif_type_e lif_usage;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    /* Traverse over all outlifs */
    for (indx = 0; indx < SOC_DPP_CONFIG(unit)->l2.nof_lifs; indx+=2 /* MPLS always even */) {
        _BCM_PETRA_L3_ITF_SET(eep, _BCM_PETRA_L3_ITF_ENCAP, indx);

        /* Get LIF usage */
        BCMDNX_IF_ERR_EXIT( _bcm_dpp_lif_usage_get(unit, -1, indx, NULL, &lif_usage));
        if (lif_usage == _bcmDppLifTypeMplsTunnel) {
            rv = bcm_petra_mpls_tunnel_initiator_clear(unit, eep); 
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_tunnel_initiator_get(
    int unit,
    bcm_if_t intf,
    int label_max,
    bcm_mpls_egress_label_t * label_array,
    int *label_count)
{
    int eep, is_tunneled;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO mpls_tunnel_info;
    uint32 next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES], nof_entries;
    int indx, cur_indx;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int cur_count = 0;
    int rv = BCM_E_NONE;
    int ll_eep, vsi;
    int tunnel_id;
    _bcm_lif_type_e lif_usage;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(label_array);
    BCMDNX_NULL_CHECK(label_count);

    soc_sand_dev_id = (unit);
    SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO_clear(&mpls_tunnel_info);

    if (!_BCM_PETRA_L3_ITF_IS_ENCAP(intf)) { /* Must be OutLIF */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("intf parameter must be ENCAP type (OutLIF)")));        
    }

    /* check if there is tunnel associated with intf at all */
    rv = _bcm_tunnel_intf_is_tunnel(unit, intf, &is_tunneled, &eep, &vsi, &ll_eep);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if not then no labels */
    if (is_tunneled == 0) {
        *label_count = 0;
        BCM_EXIT;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_usage_get(soc_sand_dev_id, -1, eep, NULL, &lif_usage));
    if ((lif_usage != _bcmDppLifTypeMplsTunnel) 
        && (lif_usage != _bcmDppLifTypeMplsPort)) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("OutLIF is not MPLS")));
    }

    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, eep, 2,
                               encap_entry_info, next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* initial value */
    *label_count = 0;

    for (indx = 0; indx < nof_entries; ++indx) {
        cur_count = 0;

        if (indx == 0) {
            _BCM_PETRA_L3_ITF_SET(tunnel_id,_BCM_PETRA_L3_ITF_ENCAP,eep);
        } else {
            _BCM_PETRA_L3_ITF_SET(tunnel_id,_BCM_PETRA_L3_ITF_ENCAP,next_eep[indx-1]);
        }

        if (encap_entry_info[indx].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) {  /* mpls encap */
            cur_count =
                encap_entry_info[indx].entry_val.mpls_encap_info.nof_tunnels;
            for (cur_indx = 0; cur_indx < cur_count; ++cur_indx) {
                mpls_tunnel_info.tunnel_label =
                    encap_entry_info[indx].entry_val.mpls_encap_info.
                    tunnels[cur_indx].tunnel_label;
                mpls_tunnel_info.push_profile =
                    encap_entry_info[indx].entry_val.mpls_encap_info.
                    tunnels[cur_indx].push_profile;
                
                rv = _bcm_mpls_tunnel_bcm_from_ppd(unit,
                                                   &label_array
                                                   [*label_count],
                                                   &mpls_tunnel_info);
                BCMDNX_IF_ERR_EXIT(rv);
                
                /* Update label id */
                label_array[*label_count].tunnel_id = tunnel_id;
                ++*label_count;
                if (*label_count >= label_max) {        /* done */
                    break;
                }
            }

            if (*label_count >= label_max) {        /* done */
                break;
            }
        } else if (encap_entry_info[indx].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_PWE) {  /* also PWE entry can be used for TUNNEL encapsulation */
            cur_count = 1;
            mpls_tunnel_info.push_profile =
                encap_entry_info[indx].entry_val.pwe_info.push_profile;
            mpls_tunnel_info.tunnel_label =
                encap_entry_info[indx].entry_val.pwe_info.label;

            rv = _bcm_mpls_tunnel_bcm_from_ppd(unit,
                                               &label_array[*label_count],
                                               &mpls_tunnel_info);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Update label id */
            label_array[*label_count].tunnel_id = tunnel_id;

            ++*label_count;
            if (*label_count >= label_max) {    /* done */
                break;
            }
        } else if (encap_entry_info[indx].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_POP_CMND) {  /* POP entry */
            cur_count = 1;
            rv = _bcm_petra_mpls_tunnel_switch_egress_label_from_pop_info(unit,
                  &label_array[*label_count],&(encap_entry_info[indx].entry_val.pop_info));
                        
            /* Update label id */
            label_array[*label_count].tunnel_id = tunnel_id;

            ++*label_count;
            if (*label_count >= label_max) {    /* done */
                break;
            }
        } else if (encap_entry_info[indx].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_SWAP_CMND) {  /* SWAP entry */
            cur_count = 1;
            label_array[*label_count].label = encap_entry_info[indx].entry_val.swap_info.swap_label;
            label_array[*label_count].flags |= (BCM_MPLS_EGRESS_LABEL_ACTION_VALID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
            label_array[*label_count].action = BCM_MPLS_EGRESS_ACTION_SWAP;
                        
            /* Update label id */
            label_array[*label_count].tunnel_id = tunnel_id;

            ++*label_count;
            if (*label_count >= label_max) {    /* done */
                break;
            }
        } else if (SOC_PPD_EG_ENCAP_DATA_COUPLING_SWAP_TYPE(soc_sand_dev_id,encap_entry_info[indx].entry_type,&(encap_entry_info[indx].entry_val.data_info))) {  /* DATA entry can be used for coupling */
            cur_count = 2;
            if (*label_count + cur_count > label_max) {
                break;
            }
            /* Label 1 NOP */
            label_array[*label_count].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
            label_array[*label_count].action = BCM_MPLS_EGRESS_ACTION_NOP;
             ++*label_count;

            /* Label 2 SWAP */
            label_array[*label_count].label =
                SOC_PPD_EG_ENCAP_DATA_COUPLING_SWAP_LABEL_GET(soc_sand_dev_id,&(encap_entry_info[indx].entry_val.data_info));
            label_array[*label_count].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
            label_array[*label_count].action = BCM_MPLS_EGRESS_ACTION_SWAP;

            /* Update label id */
            label_array[*label_count].tunnel_id = tunnel_id;

             ++*label_count;
            if (*label_count >= label_max) {    /* done */
                break;
            }
           
        } else {
            break;
        }

    }

    /* Update VSI (last entry) */
    _BCM_PETRA_L3_ITF_SET(label_array[*label_count-1].l3_intf_id,_BCM_PETRA_L3_ITF_RIF,vsi);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * when mpls action is pop_direct or php, 
 * then the pop-type <to eth, ip, mpls) is set according to vpn type
 */
int _bcm_petra_mpls_tunnel_switch_vpn_to_pop_type(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    SOC_PPD_MPLS_COMMAND * mpls_info)
{
    
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_PETRAB(unit)) {
      /* get next header indicated in flags */
      if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_L2){
          mpls_info->command = SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET;
      }
      else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4){
          mpls_info->command = SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE;
      }
      else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6){
          mpls_info->command = SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE;
      }
      else/* if not present get from VPN type */
      {
          mpls_info->command = SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE;
      }

      /* fix pip/unifrom according to TTL/EXP flags */
      /* if not ethernet check if it unifrom or pipe */
      if (mpls_info->command != SOC_TMC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET) {
          /* if uniform */
          if (info->
              flags & (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL)) {
              /* fix pop type */
              mpls_info->command += 1;
          }
      }
    } else {
      /* ARAD only */
      /* Retreive tpid_profile, model, has_cw, pop_next_header */
      mpls_info->command = SOC_TMC_MPLS_COMMAND_TYPE_POP;

      /* get next header indicated in flags */
      if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_L2){
          mpls_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE; 
      }
      else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4){
          mpls_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC; 
      }
      else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6){
          mpls_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC; 
      }
      else/* if not present take from mpls */
      {
          mpls_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_MPLS;
      }

      /* if not ethernet check if it unifrom or pipe */
      if (mpls_info->pop_next_header != SOC_TMC_PKT_FRWRD_TYPE_BRIDGE) {
          /* if uniform */
          if (info->
              flags & (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL)) {
              /* fix pop type */
              mpls_info->model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
          } else {
              mpls_info->model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE;
          }
      }
      
      
      mpls_info->has_cw = FALSE;
      mpls_info->tpid_profile = 0;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * when mpls action is termination, 
 * then the next protocol, forwarding code <to eth, ip, mpls) is set according to vpn type or flags
 */
STATIC int _bcm_petra_mpls_tunnel_switch_vpn_to_next_prtcl_type(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    SOC_PPC_MPLS_TERM_INFO * term_info)
{

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_PETRAB(unit)) {
      /* get next header indicated in flags */
      if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_L2){
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given flag type of Next Header L2: No support for Ethernet next protocol")));
      }
      else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4){
          term_info->next_prtcl = SOC_PPD_L3_NEXT_PRTCL_TYPE_IPV4;
      }
      else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6){
          term_info->next_prtcl = SOC_PPD_L3_NEXT_PRTCL_TYPE_IPV6;
      }
      else
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given VPN type of VPLS: No support for Ethernet next protocol")));
    } else { /* ARAD */
#ifdef BCM_88660_A0
          if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) {
              /* For Arad+ the default is IPv4. */
              term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC;
          } else 
#endif /* BCM_88660_A0 */
          {
              /* get next header indicated in flags */
              if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_L2){
                  term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE; 
              }
              else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4){
                  term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC; 
              }
              else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6){
                  term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC;
              }
              else /* If not present assume MPLS */
              {
                  term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_MPLS;
              }
          }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * when mpls action is termination, 
 * Get VPN and flags according to the next protocol, forwarding code <to eth, ip, mpls) 
 */
STATIC int _bcm_petra_mpls_tunnel_switch_next_prtcl_type_to_vpn(
    int unit,
    SOC_PPC_MPLS_TERM_INFO * term_info,
    SOC_PPD_RIF_INFO * rif_info,
    bcm_mpls_tunnel_switch_t * info)
{
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_VPN_ID_SET(info->vpn, rif_info->vrf_id);

    if (SOC_IS_PETRAB(unit)) {
        if (term_info->next_prtcl == SOC_PPD_L3_NEXT_PRTCL_TYPE_IPV4) 
            info->flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV4;
        if (term_info->next_prtcl == SOC_PPD_L3_NEXT_PRTCL_TYPE_IPV6) 
            info->flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV6;
    } else {
        if (term_info->forwarding_code == SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC) 
            info->flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV4;
        if (term_info->forwarding_code == SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC)
            info->flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV6;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int _bcm_petra_mpls_tunnel_switch_bcm_entry_to_ilm_key(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    SOC_PPD_FRWRD_ILM_KEY * ilm_key
    )
{
    SOC_PPD_FRWRD_ILM_GLBL_INFO glbl_info;
    int rv, idx;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPD_FRWRD_ILM_KEY_clear(ilm_key);
    soc_sand_dev_id = (unit);

    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_get(soc_sand_dev_id, &glbl_info));

    /* In case key support l3-interface, flag must be set */    
    if (glbl_info.key_info.mask_inrif != (!(info->flags & BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flag must be set only in case global key info includes In-RIF ")));
    }

    /* set key */
    /* label */
    if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
        ilm_key->in_label = BCM_MPLS_INDEXED_LABEL_VALUE_GET(info->label);
    }
    else {
        ilm_key->in_label = info->label;
    }

    if (info->flags & BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL) {
        /* Coupling */
        ilm_key->in_label_second = info->second_label;
    }

    /* port can be part of the key */
    if (!glbl_info.key_info.mask_port) {
       int core;
       _bcm_dpp_gport_info_t gport_info;

        /* Retrive local PP ports */
        rv = _bcm_dpp_gport_to_phy_port(unit, info->port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);
   
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &ilm_key->in_local_port, &core)));
    }
    
    /* inrif can be part of the key */
    if (!glbl_info.key_info.mask_inrif) {
        ilm_key->inrif = info->ingress_if;
    }

    /* in exp can be part of the key */
    if (info->flags & BCM_MPLS_SWITCH_LOOKUP_INT_PRI) {
        if ((info->label >= glbl_info.elsp_info.labels_range.start) && 
            (info->label <= glbl_info.elsp_info.labels_range.end)) {
            for(idx=0;idx<8; idx++) {
                if(glbl_info.elsp_info.exp_map_tbl[idx] == info->exp_map) {
                    break;
                }
            }
            if (idx < 8) {
                ilm_key->mapped_exp = info->exp_map;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In case of ILM ELSP after EXP does not exist")));
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_petra_mpls_tunnel_switch_ilm_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPD_FRWRD_ILM_KEY ilm_key;
    SOC_PPD_FRWRD_DECISION_INFO ilm_val;
    SOC_PPD_FEC_ID fec_id;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    SOC_TMC_MULT_ID tmp_mc;
    int soc_sand_rv;

    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);

    /* ilm key */
    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_ilm_key(unit, info, &ilm_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* ilm value */
    /* has to be EEI of type MPLS */

    rv = _bcm_l3_intf_to_fec(unit, info->egress_if, &fec_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_gport_fix_fec_type(unit, fec_id, SOC_PPD_FEC_TYPE_MPLS_LSR);
    BCMDNX_IF_ERR_EXIT(rv);

    SOC_PPD_FRWRD_DECISION_INFO_clear(&ilm_val);
    if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {

      
      /* make swap */
        /* in PB, only label is swapped  (TTL/EXP not swapped) */
        SOC_PPD_FRWRD_DECISION_ILM_SWAP_SET(unit, &ilm_val,
                                        info->egress_label.label, fec_id, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
    } else if (info->action == BCM_MPLS_SWITCH_ACTION_PHP
               || info->action == BCM_MPLS_SWITCH_ACTION_POP_DIRECT) {
        
        ilm_val.additional_info.eei.type = SOC_PPD_EEI_TYPE_MPLS;        
        ilm_val.additional_info.eei.val.mpls_command.command =
            SOC_PPD_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE;

        /* assign pop type, according to flags and VPN type */
        rv = _bcm_petra_mpls_tunnel_switch_vpn_to_pop_type(unit, info,
                                                           &(ilm_val.
                                                             additional_info.
                                                             eei.val.
                                                             mpls_command));
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (info->action == BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH) { /* PUSH ?! */
        ilm_val.type = SOC_PPD_FRWRD_DECISION_TYPE_FEC;
        ilm_val.dest_id = fec_id;
        ilm_val.additional_info.eei.type = SOC_PPD_EEI_TYPE_MPLS;
        ilm_val.additional_info.eei.val.mpls_command.command =
            SOC_PPD_MPLS_COMMAND_TYPE_PUSH;
        rv = _bcm_petra_mpls_tunnel_switch_egress_label_to_mpls_command(unit, &(info->egress_label),
                                                                        &(ilm_val.
                                                                       additional_info.
                                                                       eei.val.
                                                                       mpls_command));
        BCMDNX_IF_ERR_EXIT(rv);
        
        
    } else if (info->action == BCM_MPLS_SWITCH_ACTION_NOP) {
        /* EEI 0 */
        ilm_val.type = SOC_PPD_FRWRD_DECISION_TYPE_FEC;
        ilm_val.dest_id = fec_id;
        ilm_val.additional_info.eei.type = SOC_PPD_EEI_TYPE_EMPTY;        
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    /* set destination */
    /* of p2mp then destination is MC, otherwise destination is egress-object */
    if(info->flags & BCM_MPLS_SWITCH_P2MP) {
        ilm_val.type = SOC_PPD_FRWRD_DECISION_TYPE_MC;
        rv = _bcm_petra_multicast_group_to_id(info->mc_group,&tmp_mc);
        BCMDNX_IF_ERR_EXIT(rv);
        ilm_val.dest_id = tmp_mc;
    }
    else{
        ilm_val.type = SOC_PPD_FRWRD_DECISION_TYPE_FEC;
        ilm_val.dest_id = fec_id;       
    }

    soc_sand_rv =
        soc_ppd_frwrd_ilm_add(unit, &ilm_key, &ilm_val, &failure_indication);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(failure_indication);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int _bcm_petra_mpls_tunnel_switch_ilm_entry_to_bcm(
    int unit,
    SOC_PPD_FRWRD_ILM_KEY * ilm_key,
    SOC_PPD_FRWRD_DECISION_INFO * ilm_val,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO profile_info;
    SOC_PPD_FRWRD_ILM_GLBL_INFO glbl_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv;
    uint8 has_cw;
    int core=0; 

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_get(soc_sand_dev_id, &glbl_info));

    /* set key */
    /* label */
    info->label = ilm_key->in_label;

    if (!glbl_info.key_info.mask_port) {
        bcm_port_t local_port;

        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_pp_to_local_port_get, (unit, core, ilm_key->in_local_port, &local_port));
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_GPORT_LOCAL_SET(info->port,local_port);
    }

    if (!glbl_info.key_info.mask_inrif) {
        info->ingress_if = ilm_key->inrif;        
    }

    /* has to be EEI of type MPLS */
    if (ilm_val->additional_info.eei.type == SOC_PPD_EEI_TYPE_MPLS) {
          /* swapped or pushed label */
          info->egress_label.label =
              ilm_val->additional_info.eei.val.mpls_command.label;
          if (ilm_val->additional_info.eei.val.mpls_command.command ==
              SOC_PPD_MPLS_COMMAND_TYPE_SWAP) {
              /* swap action */
              info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
          } else if (ilm_val->additional_info.eei.val.mpls_command.command ==
              SOC_PPD_MPLS_COMMAND_TYPE_PUSH) {
              info->action = BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH;
          } else {
              info->action = BCM_MPLS_SWITCH_ACTION_PHP;
              info->egress_label.label = BCM_MPLS_LABEL_INVALID;
          }

          if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP ||
              info->action == BCM_MPLS_SWITCH_ACTION_PHP) {
          
              soc_sand_rv =
                  soc_ppd_eg_encap_push_profile_info_get(soc_sand_dev_id,
                                                     ilm_val->
                                                     additional_info.eei.val.
                                                     mpls_command.
                                                     push_profile, &profile_info);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
              /* pipe: set TTL and EXP, and always dec TTL */
              if (profile_info.model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE) {
                  info->flags = (BCM_MPLS_SWITCH_TTL_DECREMENT);
              } else {            /* uniform: copy TTL and EXP, and always dec TTL */

                  info->flags =
                      (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL
                       | BCM_MPLS_SWITCH_TTL_DECREMENT);
              }
         } else { /* Action is Egress PUSH */
               rv = _bcm_mpls_tunnel_push_profile_info_get(unit, 
                                                           ilm_val->
                                                           additional_info.eei.val.
                                                           mpls_command.
                                                           push_profile,
                                                           &has_cw,
                                                           &(info->egress_label));
               BCMDNX_IF_ERR_EXIT(rv);               
         }

    } else if (ilm_val->additional_info.eei.type == SOC_PPD_EEI_TYPE_EMPTY) {
          info->action = BCM_MPLS_SWITCH_ACTION_NOP; 
          info->flags |= BCM_MPLS_SWITCH_TTL_DECREMENT; /* always done */
          info->egress_label.label = BCM_MPLS_LABEL_INVALID;
    } else {
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Internal error, in case of FEC EEI type must be MPLS or Empty")));
    }

    if (ilm_val->type == SOC_PPD_FRWRD_DECISION_TYPE_FEC) {       
        _bcm_l3_fec_to_intf(unit, ilm_val->dest_id, &info->egress_if);        
    } else if (ilm_val->type == SOC_PPD_FRWRD_DECISION_TYPE_MC) {
        info->flags |= BCM_MPLS_SWITCH_P2MP;
        rv = _bcm_petra_multicast_group_from_id(&info->mc_group,_BCM_MULTICAST_TYPE_EGRESS_OBJECT,ilm_val->dest_id);
        BCMDNX_IF_ERR_EXIT(rv);               
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Not supported type: Expect always for ILM to be of type FEC or Multicast")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_mpls_tunnel_switch_ilm_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPD_FRWRD_ILM_KEY ilm_key;
    SOC_PPD_FRWRD_DECISION_INFO ilm_val;
    uint8 found;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_ilm_key(unit, info, &ilm_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* ilm value */
    soc_sand_rv = soc_ppd_frwrd_ilm_get(soc_sand_dev_id, &ilm_key, &ilm_val, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!found) {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    rv = _bcm_petra_mpls_tunnel_switch_ilm_entry_to_bcm(unit, &ilm_key,
                                                        &ilm_val, info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_petra_mpls_tunnel_switch_ilm_remove(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPD_FRWRD_ILM_KEY ilm_key;
    SOC_PPD_FRWRD_DECISION_INFO ilm_val;
    int soc_sand_rv, rv;
    unsigned int soc_sand_dev_id;

    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_ilm_key(unit, info, &ilm_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* ilm value*/
    soc_sand_rv = soc_ppd_frwrd_ilm_get(soc_sand_dev_id, &ilm_key, &ilm_val, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!found) {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    soc_sand_rv = soc_ppd_frwrd_ilm_remove(soc_sand_dev_id, &ilm_key);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}






/* when action is pop, then another forwarding lookup will be performed
 *  so do the pop in the termination DB
 *  if the forwarding is Ethernet, VPLS, then set termination in ranges
 */
int _bcm_petra_mpls_tunnel_switch_term_range_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    SOC_PPD_MPLS_TERM_INFO * term_info,
    SOC_PPD_RIF_INFO * rif_info)
{
    SOC_PPD_MPLS_TERM_LABEL_RANGE_INFO range_info;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int range_indx = 0;
    int match_indx = 0, match_valid = 0;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* number of range */
    for (range_indx = 0; range_indx < 3; ++range_indx) {
        /* if range not valid then fill termination info */
        soc_sand_rv =
            soc_ppd_mpls_term_label_range_get(soc_sand_dev_id, range_indx, &range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* fit range, the label fell in this range */
        if (info->label >= range_info.range.first_label
            && info->label <= range_info.range.last_label) {
            match_indx = range_indx;
            match_valid = 1;
            break;
        }
    }

    /* IF found update range configuration */
    if (match_valid) {
        soc_sand_rv =
            soc_ppd_mpls_term_label_range_get(soc_sand_dev_id, match_indx, &range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        sal_memcpy(&range_info.term_info, term_info,
                   sizeof(SOC_PPD_MPLS_TERM_INFO));
        soc_sand_rv =
            soc_ppd_mpls_term_label_range_set(soc_sand_dev_id, match_indx, &range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if(term_info->rif != SOC_PPD_RIF_NULL) {
            soc_sand_rv = soc_ppd_rif_info_set(soc_sand_dev_id, term_info->rif, rif_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not found MPLS termination label range"))); /* the given label cannot set in any range */
    }

    /* set relevant label bit */
    soc_sand_rv =
        soc_ppd_mpls_term_range_terminated_label_set(soc_sand_dev_id, info->label, 1);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_mpls_tunnel_switch_term_range_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    SOC_PPD_MPLS_TERM_INFO * term_info,
    SOC_PPD_RIF_INFO * rif_info)
{
    SOC_PPD_MPLS_TERM_LABEL_RANGE_INFO range_info;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int range_indx = 0;
    int match_indx = 0, match_valid = 0;
    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* number of range */
    for (range_indx = 0; range_indx < 3; ++range_indx) {
        /* if range not valid then fill termination info */
        soc_sand_rv =
            soc_ppd_mpls_term_label_range_get(soc_sand_dev_id, range_indx, &range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* fit range, the label fell in this range */
        if (info->label >= range_info.range.first_label
            && info->label <= range_info.range.last_label) {
            match_indx = range_indx;
            match_valid = 1;
            break;
        }
    }

    /* IF found get Rif/termination info */
    if (match_valid) {
        soc_sand_rv =
            soc_ppd_mpls_term_range_terminated_label_get(soc_sand_dev_id,
                                                     info->label, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("label does not match Range termination range")));
        }

        soc_sand_rv =
            soc_ppd_mpls_term_label_range_get(soc_sand_dev_id, match_indx, &range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        sal_memcpy(term_info, &range_info.term_info,
                   sizeof(SOC_PPD_MPLS_TERM_INFO));

        if (term_info->rif != SOC_PPD_RIF_NULL) {
            soc_sand_rv = soc_ppd_rif_info_get(soc_sand_dev_id, term_info->rif, rif_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    } else {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND); /* the given label cannot set in any range */
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_mpls_term_to_pwe_lif_types(
    int unit,
    uint32 lif_types,
    uint32 *pwe_lif_types)
{

    BCMDNX_INIT_FUNC_DEFS;    

    *pwe_lif_types = 0;

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_DEFAULT;
    }

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1;
    }

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_2) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_INDEXED_2;
    }

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_3) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_INDEXED_3;
    }

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_2) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1_2;
    }

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_3) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1_3;
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;      
}

int _bcm_petra_mpls_term_resolve_lif_type_by_index(
    int unit,
    int port,
    bcm_mpls_label_t label,
    uint32 *lif_types)
{
    int is_frr_port;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;    

    rv = bcm_petra_port_control_get(unit, port, bcmPortControlMplsFRREnable, &is_frr_port);
    BCMDNX_IF_ERR_EXIT(rv);

    switch (BCM_MPLS_INDEXED_LABEL_INDEX_GET(label)) {
    case 1:     
      if (SOC_DPP_CONFIG(unit)->pp.mpls_namespaces[0] == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3) {
          *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_3;
      } else if (SOC_DPP_CONFIG(unit)->pp.mpls_namespaces[0] == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2) {
          *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_2;
      } else {
          *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1;
      }  
      break;
    case 2:
      *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_2;
      break;
    case 3:
      *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_3;
      break;
    default:
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported index value: %d"), BCM_MPLS_INDEXED_LABEL_INDEX_GET(label)));
    }
    
exit:
    BCMDNX_FUNC_RETURN;      
}

int _bcm_petra_mpls_term_key_to_lif_alloc_type(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    uint32 *lif_types)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;    

    if (info->flags & BCM_MPLS_SWITCH_FRR) { /* FRR */
        *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_FRR;
    } else if ((!(info->flags & BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL)) && SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) { /* Indexed not coupling, not frr entries */
        rv = _bcm_petra_mpls_term_resolve_lif_type_by_index(unit, info->port, info->label, lif_types);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM; /* Default */
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC
int _bcm_petra_mpls_tunnel_switch_bcm_entry_to_term_key(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    SOC_PPD_MPLS_LABEL_RIF_KEY *rif_key)
{
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_MPLS_TERM_LKUP_INFO glbl_info;
    uint8 is_key_label_only;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    SOC_PPD_MPLS_LABEL_RIF_KEY_clear(rif_key);

    soc_sand_rv = soc_ppd_mpls_term_lkup_info_get(soc_sand_dev_id, &glbl_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* In case key support l3-interface, flag must be set */    
    if ((glbl_info.key_type == SOC_PPD_MPLS_TERM_KEY_TYPE_LABEL_RIF) == (!(info->flags & BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flag must be set only in case global key info includes In-RIF ")));
    }

    is_key_label_only = SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1);
    if (!is_key_label_only) {
      if (((info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4) && !(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6))
          || (!(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4) && (info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6))) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flags 0x%x in case one of next procotol IP is set both of them must be set"), info->flags));
      }
    }
    /* set key */
    /* label */
    if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
        rif_key->label_id = BCM_MPLS_INDEXED_LABEL_VALUE_GET(info->label);
        rif_key->label_index = BCM_MPLS_INDEXED_LABEL_INDEX_GET(info->label);
    } else {
        if (!_BCM_DPP_MPLS_LABEL_IN_RANGE(info->label)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MPLS label is out of range. The range is 0 - (1<<20)-1")));
        }
        rif_key->label_id = info->label;
    }

    /* RIF can be part of the key */
    if (glbl_info.key_type == SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL_RIF) {
        rif_key->vsid = info->ingress_if;
    }

    if (info->flags & (BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL | BCM_MPLS_SWITCH_FRR)) { /* Coupling */
        if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
            rif_key->label_id_second = BCM_MPLS_INDEXED_LABEL_VALUE_GET(info->second_label);
        } else {
            if (!_BCM_DPP_MPLS_LABEL_IN_RANGE(info->second_label)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MPLS second label is out of range. The range is 0 - (1<<20)-1")));
            }
            rif_key->label_id_second = info->second_label;
        }
        rif_key->flags |= (info->flags & BCM_MPLS_SWITCH_FRR) ? SOC_PPD_RIF_MPLS_LABEL_FRR:SOC_PPD_RIF_MPLS_LABEL_COUPLING;
    }
    
    if (info->flags & BCM_MPLS_SWITCH_ENTROPY_ENABLE) { /* ELI */
        rif_key->flags |= SOC_PPD_RIF_MPLS_LABEL_ELI; /* Add ELI in case of Entropy for MPLS tunnels */
    }     

    if (rif_key->flags & SOC_PPD_RIF_MPLS_LABEL_COUPLING && SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Coupling is not supported when mpls label index is enabled ")));
    }

    if (!is_key_label_only) {
        if ((info->flags & BCM_MPLS_SWITCH_ENTROPY_ENABLE && 
            !((info->flags & (BCM_MPLS_SWITCH_NEXT_HEADER_L2|BCM_MPLS_SWITCH_NEXT_HEADER_IPV4|BCM_MPLS_SWITCH_NEXT_HEADER_IPV6))))) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ELI Entorpy is not supported when entropy is not BOS"))); 
        }
    }

    /* In Arad the BOS expectation is determined by the next type. */
    if (info->flags & (BCM_MPLS_SWITCH_NEXT_HEADER_L2|BCM_MPLS_SWITCH_NEXT_HEADER_IPV4|BCM_MPLS_SWITCH_NEXT_HEADER_IPV6)) {
        rif_key->flags |= SOC_PPD_RIF_MPLS_LABEL_EXPECT_BOS;
    }

    /* 
     *  Not allowed:
     *  1. SOC_PPC_MPLS_LABEL_INDEX_SECOND in case of L1_L2 namespace (use L1 only)
     *  2. SOC_PPC_MPLS_LABEL_INDEX_THIRD in case of L1_L3 namespace (use L1 only)
     */ 
    if (rif_key->label_index == SOC_PPC_MPLS_LABEL_INDEX_SECOND &&
        SOC_DPP_CONFIG(unit)->pp.mpls_namespaces[0] == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Provided label index second where there is no L2 namespace. Use L1 for both L1,L2"))); 
    }
    if (rif_key->label_index == SOC_PPC_MPLS_LABEL_INDEX_THIRD &&
        SOC_DPP_CONFIG(unit)->pp.mpls_namespaces[0] == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Provided label index second where there is no L3 namespace. Use L1 for both L1,L3"))); 
    }
       
#ifdef BCM_88660_A0
    if (is_key_label_only) {
        /* In Arad+ the BOS expectation is determined by the flags for the reserved labels. */
        /* For unreserved labels there is no expect BOS or no BOS (since they don't check the */
        /* BOS to begin with). */
        rif_key->flags &= ~SOC_PPD_RIF_MPLS_LABEL_EXPECT_BOS;
        if (info->label <= _BCM_PETRA_MPLS_MAX_RESERVED_MPLS_LABEL) {
            rif_key->flags |= (info->flags & BCM_MPLS_SWITCH_EXPECT_BOS) ? SOC_PPD_RIF_MPLS_LABEL_EXPECT_BOS : 0;
        }
    }
#endif /* BCM_88660_A0 */

    if (info->flags & 0x08000000 /* BCM_MPLS_SWITCH_LOOKUP_NONE */) {
        rif_key->flags |= SOC_PPD_RIF_MPLS_LABEL_LOOKUP_NONE;
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_mpls_tunnel_switch_term_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPD_MPLS_LABEL_RIF_KEY rif_key;
    SOC_PPD_MPLS_TERM_INFO term_info;
    SOC_PPD_RIF_INFO rif_info;
    SOC_PPD_MPLS_TERM_PROFILE_INFO term_profile_info;
    uint8 found;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_LIF_ID lif_indx;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    SOC_PPD_MPLS_LABEL_RIF_KEY_clear(&rif_key);
    SOC_PPD_MPLS_TERM_INFO_clear(&term_info);
    SOC_PPD_RIF_INFO_clear(&rif_info);

    /* get from range */
    if (SOC_DPP_CONFIG(unit)->pp.mpls_label_range_enable) {
        rv = _bcm_petra_mpls_tunnel_switch_term_range_get(unit, info,
                                                          &term_info, &rif_info);
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    if (rv != BCM_E_NOT_FOUND) {
        BCMDNX_IF_ERR_EXIT(rv);
        /* if found then update action to pop */
        info->action = BCM_MPLS_SWITCH_ACTION_POP;
    }

    /* if not found in range get from sem termination */
    if (rv == BCM_E_NOT_FOUND) {
        /* set key */
        rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_term_key(unit,info,&rif_key);
        BCMDNX_IF_ERR_EXIT(rv);
          

        soc_sand_rv =
            soc_ppd_rif_mpls_label_map_get(soc_sand_dev_id, &rif_key, &lif_indx,
                                       &term_info, &rif_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (!found) {
            BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
        }
        info->action = BCM_MPLS_SWITCH_ACTION_POP;
        /* update from LIF sw state */
        rv = _bcm_dpp_in_lif_mpls_term_match_get(unit, info, lif_indx);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id,lif_indx);

        /* Retrieve some flags from the the termination profile info. */
        soc_sand_rv = soc_ppd_mpls_term_profile_info_get(soc_sand_dev_id, term_info.term_profile, &term_profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (term_profile_info.flags & SOC_PPD_MPLS_TERM_SKIP_ETH) {
            info->flags |= BCM_MPLS_SWITCH_SKIP_ETHERNET;
        }
#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) {
            info->flags |= (rif_key.flags & SOC_PPD_RIF_MPLS_LABEL_EXPECT_BOS) ? BCM_MPLS_SWITCH_EXPECT_BOS : 0;
        }
#endif /* BCM_88660_A0 */

    }
    /* map term info and rif info to bcm-switch info */

    

    /* termination value */
    if (term_info.cos_profile != 0) {	 	 
        BCM_INT_QOS_MAP_LIF_COS_SET(info->qos_map_id, term_info.cos_profile);	 	 
    }
    info->tunnel_if = (term_info.rif == SOC_PPD_RIF_NULL) ? BCM_IF_INVALID:term_info.rif;

    if (term_info.processing_type == SOC_PPD_MPLS_TERM_MODEL_UNIFORM) {
        info->flags |= (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL);
    }

    if (term_info.rif != SOC_PPD_RIF_NULL) {
        rv = _bcm_petra_mpls_tunnel_switch_next_prtcl_type_to_vpn(unit,&term_info,&rif_info,info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_mpls_tunnel_switch_term_range_get_next(
    int unit,
    int *iter,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPD_MPLS_TERM_LABEL_RANGE_INFO range_info;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int range_indx = 0;
    int rv = BCM_E_NONE;
    uint8 terminated = FALSE;
    int update_label = 0;
    int lbl_indx;


    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    range_indx = *iter & (3 << 30);
    lbl_indx = *iter - range_indx;

    if (*iter == 0) {
        update_label = 1;
    }

    /* number of range */
    for (; range_indx < 3 && !terminated; ++range_indx) {
        /* ger range info */
        soc_sand_rv =
            soc_ppd_mpls_term_label_range_get(soc_sand_dev_id, range_indx, &range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (update_label == 1) {
            lbl_indx = range_info.range.first_label;
        }

        for (; lbl_indx <= range_info.range.last_label; ++lbl_indx) {
            soc_sand_rv =
                soc_ppd_mpls_term_range_terminated_label_get(soc_sand_dev_id,
                                                         lbl_indx, &terminated);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (terminated) {
                break;
            }
        }

        update_label = 1;
    }

    if (!terminated) {
        *iter = -1;
        BCM_EXIT;
    }

    /* get all information of the label from termination db */
    info->label = lbl_indx;
    *iter = (range_indx << 30) | lbl_indx;

    rv = _bcm_petra_mpls_tunnel_switch_term_get(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_mpls_tunnel_switch_term_lif_get_next(
    int unit,
    int *iter,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPD_MPLS_TERM_LABEL_RANGE_INFO range_info;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int range_indx = 0;
    int rv = BCM_E_NONE;
    uint8 terminated = FALSE;
    int update_label = 0;
    int lbl_indx;


    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    range_indx = *iter & (3 << 30);
    lbl_indx = *iter - range_indx;

    if (*iter == 0) {
        update_label = 1;
    }

    /* number of range */
    for (; range_indx < 3; ++range_indx) {
        /* ger range info */
        soc_sand_rv =
            soc_ppd_mpls_term_label_range_get(soc_sand_dev_id, range_indx, &range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (update_label == 1) {
            lbl_indx = range_info.range.first_label;
        }

        for (; lbl_indx <= range_info.range.last_label; ++lbl_indx) {
            soc_sand_rv =
                soc_ppd_mpls_term_range_terminated_label_get(soc_sand_dev_id,
                                                         info->label,
                                                         &terminated);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (terminated) {
                break;
            }
        }

        update_label = 1;
    }

    if (!terminated) {
        *iter = 1;
        BCM_EXIT;
    }

    /* get all information of the label from termination db */
    rv = _bcm_petra_mpls_tunnel_switch_term_get(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);

    info->label = lbl_indx;
    *iter = (range_indx << 30) | lbl_indx;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_mpls_tunnel_switch_term_range_remove(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPD_MPLS_TERM_LABEL_RANGE_INFO range_info;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int range_indx = 0;
    int match_valid = 0;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* number of range */
    for (range_indx = 0; range_indx < 3; ++range_indx) {
        /* if range not valid then fill termination info */
        soc_sand_rv =
            soc_ppd_mpls_term_label_range_get(soc_sand_dev_id, range_indx, &range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* fit range, the label fell in this range */
        if (info->label >= range_info.range.first_label
            && info->label <= range_info.range.last_label) {
            match_valid = 1;
            break;
        }
    }

    /* IF not found then return */
    if (!match_valid) {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    /* set relevant label bit */
    soc_sand_rv =
        soc_ppd_mpls_term_range_terminated_label_set(soc_sand_dev_id, info->label, 0);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* when action is pop, then another forwarding lookup will be performed
   so do the pop in the termination DB
   */
int _bcm_petra_mpls_tunnel_switch_term_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPD_MPLS_LABEL_RIF_KEY rif_key;
    SOC_PPD_MPLS_TERM_INFO term_info;
    SOC_PPD_RIF_INFO rif_info;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int lif_indx;
    int rv = BCM_E_NONE;
    uint32 inner_flags = 0;
    uint32 term_lif_type;
    uint32 alloc_flags = 0;
    int update = 0;
    int err_code = 0;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    SOC_PPD_MPLS_LABEL_RIF_KEY_clear(&rif_key);
    SOC_PPD_MPLS_TERM_INFO_clear(&term_info);
    SOC_PPD_RIF_INFO_clear(&rif_info);

    /* In case label ranges are defined, we assert the equality og the in label to the  lower limit of the range*/
    if (SOC_IS_JERICHO(unit)) {
        uint32 entry_index;
        SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO range_action_info;
        SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(&range_action_info);

        /* verify whether the in label belongs to a certain label range*/
        rv = _bcm_dpp_mpls_range_action_verify(unit,info->label, info->label, &entry_index, &err_code);
        BCMDNX_IF_ERR_EXIT(rv);
        /* In this case, there is a label range containing the in label*/
        if (err_code == _BCM_MPLS_TERM_LABEL_RANGE_INTERSECT || err_code == _BCM_MPLS_TERM_LABEL_RANGE_EXIST) {
            soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_action_get,(unit, entry_index, &range_action_info)));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
            /* the label belongs to a certain range, so we assert the equality to the lower limit of the range*/
            if (info->label != range_action_info.label_low) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In case a label range is defined, the in label should be equal to the lower limit of the range")));
            }
        }
    }

    /* If REPLACE flag is set, WITH_ID must also be set */
    if (info->flags & BCM_MPLS_SWITCH_REPLACE && !(info->flags & BCM_MPLS_SWITCH_WITH_ID)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If REPLACE flag is set, WITH_ID flag must also be set.")));
    }

    if (info->flags & BCM_MPLS_SWITCH_ENTROPY_ENABLE && (!SOC_DPP_CONFIG(unit)->pp.mpls_eli_enable)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In case ENTROPY flag is set, ELI soc property mpls_entropy_label_indicator_enable must be enabled ")));
    }

    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_term_key(unit,info,&rif_key);
    BCMDNX_IF_ERR_EXIT(rv);


    /* termination value */
    term_info.cos_profile = BCM_QOS_MAP_PROFILE_GET(info->qos_map_id);

    /* set next protocol according to info */
    rv = _bcm_petra_mpls_tunnel_switch_vpn_to_next_prtcl_type(unit,info,&term_info);
    BCMDNX_IF_ERR_EXIT(rv);
    
    if (info->flags & (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL)) {
        term_info.processing_type = SOC_PPD_MPLS_TERM_MODEL_UNIFORM;
    } else {
        term_info.processing_type = SOC_PPD_MPLS_TERM_MODEL_PIPE;
    }

    term_info.rif = (info->tunnel_if == BCM_IF_INVALID) ? SOC_PPD_RIF_NULL:info->tunnel_if;


    /* set RIF info */
    if (term_info.rif != SOC_PPD_RIF_NULL) {
        soc_sand_rv = soc_ppd_rif_info_get(soc_sand_dev_id, term_info.rif, &rif_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rif_info.vrf_id = _BCM_DPP_VPN_ID_GET(info->vpn);
        rif_info.cos_profile = term_info.cos_profile;
    }



    /* following attributes is not updated */
    /*   rif_info.uc_rpf_enable = 0;
       rif_info.ttl_scope_index = 0; */    
    /* try to add in range */
    if (SOC_DPP_CONFIG(unit)->pp.mpls_label_range_enable) {
        rv = _bcm_petra_mpls_tunnel_switch_term_range_add(unit, info,
                                                          &term_info, &rif_info);
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    if (rv != BCM_E_NOT_FOUND) {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* if not applicable in range add in sem termination */
    if (rv == BCM_E_NOT_FOUND) {
        rv = _bcm_petra_mpls_term_key_to_lif_alloc_type(unit, info, &term_lif_type);
        BCMDNX_IF_ERR_EXIT(rv);

        update = (info->flags & BCM_MPLS_SWITCH_REPLACE) ? 1:0; 

        /* check update/with-id allocations*/
        if (BCM_GPORT_IS_TUNNEL(info->tunnel_id) || (info->flags & BCM_MPLS_SWITCH_WITH_ID)) {
            lif_indx = _SHR_GPORT_TUNNEL_ID_GET(info->tunnel_id);            
            alloc_flags |= SHR_RES_ALLOC_WITH_ID;

			/* Special case for Explicit NULL: Explicit NULL 
				termination type is concluded according to LIF-ID. LIF-ID is determined by SOC property */
            if (SOC_DPP_CONFIG(unit)->pp.explicit_null_support && (info->flags & 0x08000000 /* BCM_MPLS_SWITCH_LOOKUP_NONE */) &&
                lif_indx == SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id) {
                term_lif_type = _BCM_DPP_AM_L3_LIF_MPLS_TERM_EXPLICIT_NULL;
            }
        }

        /* cannot update without given id */
        if (update && !(alloc_flags & SHR_RES_ALLOC_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Updated flag is set without providing proper ID")));
        }

        /* if update then don't overwrite this consts */
        if (!update) {
            term_info.learn_enable = 0;
        }

        /* allocate LIf for mpls tunnel termination */
        if (update) {/* update check LIF already allocated */
            rv = bcm_dpp_am_l3_lif_is_allocated(unit, term_lif_type, 0, lif_indx);
            if(rv != BCM_E_EXISTS){
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Updated mpls tunnel does not exist")));
            }
        }
        else{/* allocated LIF [with-id]]*/
            rv = bcm_dpp_am_l3_lif_alloc(unit, term_lif_type, alloc_flags, &lif_indx);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* LIF termination handling */
        if (SOC_IS_ARAD(unit)) {

            rv = _bcm_mpls_tunnel_lif_term_tunnel_switch_to_flags(unit, info, &inner_flags);
            BCMDNX_IF_ERR_EXIT(rv);

            BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_lif_term_profile_exchange(unit,
                                               lif_indx,
                                               inner_flags,
                                               &(term_info.term_profile)));     
        }

        soc_sand_rv =
            soc_ppd_rif_mpls_label_map_add(soc_sand_dev_id, &rif_key, lif_indx,
                                       &term_info, &rif_info,
                                       &failure_indication);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(failure_indication);

        /* tunnel id */
        /* tunnel ID always is LIF-id,
         * also for 1+1: protection, MC-destination is seperate handled object. 
         */
        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id,lif_indx);

        rv = _bcm_dpp_in_lif_mpls_term_match_add(unit, info, lif_indx);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_mpls_tunnel_switch_term_remove(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPD_MPLS_LABEL_RIF_KEY rif_key;
    SOC_PPD_RIF_INFO rif_info;
    SOC_PPD_MPLS_TERM_INFO term_info;
    int in_lif_id;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_LIF_ID lif_indx;
    int rv = BCM_E_NONE;
    uint8 found;
	_bcm_lif_type_e lif_usage;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    SOC_PPD_MPLS_LABEL_RIF_KEY_clear(&rif_key);

	/* Remove MPLS tunnel when lookup is none means skip SEM configuration. 
	   Once SEM configuration is removed, 
	   LIF-ID must be INPUT and not implicit found from SEM lookup */
    if (info->flags & 0x08000000 /* BCM_MPLS_SWITCH_LOOKUP_NONE */ &&
        info->tunnel_id == 0) {     
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When lookup key is BCM_MPLS_SWITCH_LOOKUP_NONE tunnel_id must be valid.")));
    }
    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_term_key(unit, info, &rif_key);
    BCMDNX_IF_ERR_EXIT(rv);      

    /* try to remove from range */
    if (SOC_DPP_CONFIG(unit)->pp.mpls_label_range_enable) {
        rv = _bcm_petra_mpls_tunnel_switch_term_range_remove(unit, info);
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    if (rv != BCM_E_NOT_FOUND) {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* if not applicable in range remove from sem termination */
    if (rv == BCM_E_NOT_FOUND) {

        if (rif_key.flags & SOC_PPD_RIF_MPLS_LABEL_LOOKUP_NONE) {
           /* No lookup to retreive LIF-ID, lif_indx is provided by the API in this case */
           rv = _bcm_dpp_gport_to_lif(unit, info->tunnel_id, &in_lif_id, NULL,
                               NULL, NULL);
           BCMDNX_IF_ERR_EXIT(rv);
           lif_indx = in_lif_id;   
		   BCMDNX_IF_ERR_EXIT( _bcm_dpp_lif_usage_get(unit, in_lif_id, -1, &lif_usage, NULL) );
           found = (lif_usage == _bcmDppLifTypeMplsTunnel) ? TRUE:FALSE;
        } else {

          soc_sand_rv =
              soc_ppd_rif_mpls_label_map_get(soc_sand_dev_id, &rif_key, &lif_indx,
                                         &term_info, &rif_info, &found);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        if (!found) {
            BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
        }

        BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_lif_term_profile_exchange(unit,
                                               lif_indx,
                                               0x0, /* Default */
                                               NULL));     

        soc_sand_rv =
            soc_ppd_rif_mpls_label_map_remove(soc_sand_dev_id, &rif_key, &lif_indx);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = bcm_dpp_am_l3_lif_dealloc(unit, lif_indx);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_in_lif_match_delete(unit, lif_indx);
        BCMDNX_IF_ERR_EXIT(rv);

    }
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_tunnel_switch_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API no longer supported. Please use bcm_mpls_tunnel_switch_create instead.")));

  exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_mpls_tunnel_switch_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    /* verify parameters */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(info);

    /* all these flags should be reset */
    if (info->
        flags & (BCM_MPLS_SWITCH_COUNTED | BCM_MPLS_SWITCH_INT_PRI_SET |
                 BCM_MPLS_SWITCH_INT_PRI_MAP | BCM_MPLS_SWITCH_COLOR_MAP |
                 BCM_MPLS_SWITCH_INNER_EXP | BCM_MPLS_SWITCH_INNER_TTL |
                 BCM_MPLS_SWITCH_DROP)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags")));
    }

    if (SOC_IS_PETRAB(unit) && info->flags & BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags")));
    }

    /* Next protocol has to be specified in Arad or Arad+ with disabled soc property BCM886XX_MPLS_TERMINATION_KEY_MODE
       In this case the next protocol is used to find whether lif is BOS or not */
    if ((info->action == BCM_MPLS_SWITCH_ACTION_POP) &&
        (!SOC_IS_ARAD_B1_AND_BELOW(unit) || !soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) &&
            ((info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_L2) || 
             (info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4) ||
             (info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags. Next protocol is taken from the first nibble of the next header after MPLS. See BCM88660 SW Compatibility between versions 6.3 6.4, Section MPLS L3 Router")));
    }

    /* not supported cobmination of flags */

    /* both should be set, or reset, (i.e. has same bit value) */
    if ((!(info->flags & BCM_MPLS_SWITCH_OUTER_EXP)
         && (info->flags & BCM_MPLS_SWITCH_OUTER_TTL))
        || ((info->flags & BCM_MPLS_SWITCH_OUTER_EXP)
            && !(info->flags & BCM_MPLS_SWITCH_OUTER_TTL))
        ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid configuration, expect BCM_MPLS_SWITCH_OUTER_TTL and BCM_MPLS_SWITCH_OUTER_EXP to have the same settings")));
    }
    /* unsupported action - out of range */
    if (info->action > BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Action type")));
    }

    /* if action is not pop, then his is ILM action */
    if (info->action != BCM_MPLS_SWITCH_ACTION_POP) {
        /* these flags should be set */
        if ((info->action != BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH) && 
            (info->action != BCM_MPLS_SWITCH_ACTION_POP_DIRECT) && 
            (info->action != BCM_MPLS_SWITCH_ACTION_PHP) && 
            (!(info->flags & BCM_MPLS_SWITCH_TTL_DECREMENT))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_SWITCH_TTL_DECREMENT must be set in case of ILM SWAP")));     /* TTL always decemented (even if inherented/copied */
        }

        rv = _bcm_petra_mpls_tunnel_switch_ilm_add(unit, info);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {                    /* else this is termination operation */
        rv = _bcm_petra_mpls_tunnel_switch_term_add(unit, info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_tunnel_switch_create(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    rv = _bcm_petra_mpls_tunnel_switch_add(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
 exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_tunnel_switch_delete(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int rv1,rv2 = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    /* Delete the ILM */
    rv1 = _bcm_petra_mpls_tunnel_switch_ilm_remove(unit, info);
    if (rv1 != BCM_E_NOT_FOUND) {
        BCMDNX_IF_ERR_EXIT(rv1);
    }
    /* Delete Tunnel termination */
    rv2 = _bcm_petra_mpls_tunnel_switch_term_remove(unit, info);
    
    if (rv1 == BCM_E_NOT_FOUND && rv2 == BCM_E_NOT_FOUND) {
       /* In case of both NOT FOUND return error */
       BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Given label is not found in both ILM and Tunnel termination"))); /* the given label cannot set in any range */
    } else if (rv2 != BCM_E_NOT_FOUND) {
       /* in case tunnel termination returned error that is not found */
       BCMDNX_IF_ERR_EXIT(rv2);
    } else {
       /* in case everything is ok - first found, second not found */
       BCM_EXIT;
    }
    
    

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


/*
 * low level function to invalidate all entries in MPLS tunnel termination table 
 * used table access to save time 
 * only for PB 
 */
#if LINK_PB_PP_LIBRARIES
STATIC int bcm_petra_mpls_tunnel_switch_range_delete_all(
    unsigned int soc_sand_dev_id)
{

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(soc_sand_dev_id)) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("Error while taking soc_sand chip descriptor mutex")));
    }


    if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(soc_sand_dev_id)) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("Error while giving soc_sand chip descriptor mutex")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}
#endif

int bcm_petra_mpls_tunnel_switch_delete_all(
    int unit)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;

    int rv = BCM_E_NONE;
    _bcm_lif_type_e usage;
    bcm_mpls_tunnel_switch_t mpls_term;
    int cur_lif;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    /* clear ILM */
    soc_sand_rv = soc_ppd_frwrd_ilm_table_clear(soc_sand_dev_id);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    /* clear termination */
    bcm_mpls_tunnel_switch_t_init(&mpls_term);

    /* clear SEM termination */
    for (cur_lif = 0; cur_lif < _BCM_GPORT_NOF_LIFS; ++cur_lif) {

        rv = _bcm_dpp_lif_usage_get(unit, cur_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, &usage, NULL);
        BCMDNX_IF_ERR_EXIT(rv);
        /* if lif used for MPLS termiantion remove it */
        if (usage == _bcmDppLifTypeMplsTunnel) {
            rv = _bcm_dpp_in_lif_mpls_term_match_get(unit, &mpls_term, cur_lif);
            BCMDNX_IF_ERR_EXIT(rv);

            /* When remove LIF that is without SEM lookup, LIF-ID must be input */
            if (mpls_term.flags & 0x08000000 /* BCM_MPLS_SWITCH_LOOKUP_NONE */) {
                BCM_GPORT_TUNNEL_ID_SET(mpls_term.tunnel_id, cur_lif);
            }
            rv = _bcm_petra_mpls_tunnel_switch_term_remove(unit, &mpls_term);
            BCMDNX_IF_ERR_EXIT(rv);

            /* remove status */
            rv = _bcm_dpp_in_lif_match_delete(unit, cur_lif);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

#if LINK_PB_PP_LIBRARIES
    if (SOC_DPP_CONFIG(unit)->pp.mpls_label_range_enable) {
    /* clear range termination */
    rv = bcm_petra_mpls_tunnel_switch_range_delete_all(soc_sand_dev_id);
    BCMDNX_IF_ERR_EXIT(rv);
    }    
#endif

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_mpls_tunnel_switch_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* get from termination db */
    rv = _bcm_petra_mpls_tunnel_switch_term_get(unit, info);
    if (rv == BCM_E_NOT_FOUND) {        /* if not found get from ILM db */
        rv = _bcm_petra_mpls_tunnel_switch_ilm_get(unit, info);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_mpls_tunnel_switch_t_init not dispatchable */

int bcm_petra_mpls_tunnel_switch_traverse(
    int unit,
    bcm_mpls_tunnel_switch_traverse_cb cb,
    void *user_data)
{
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    int indx;
    uint32 nof_entries;
    bcm_mpls_tunnel_switch_t cur_entry;
    int cur_lif;
    _bcm_lif_type_e usage;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int rv = BCM_E_NONE;
    int iter;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    bcm_mpls_tunnel_switch_t_init(&cur_entry);

    /* traverse range */

    for (iter = 0; iter != -1;) {
        rv = _bcm_petra_mpls_tunnel_switch_term_range_get_next(unit, &iter,
                                                               &cur_entry);
        BCMDNX_IF_ERR_EXIT(rv);
        if (iter != -1) {
            rv = cb(unit, &cur_entry, user_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    bcm_mpls_tunnel_switch_t_init(&cur_entry);

    /* traverse SEM termination */
    for (cur_lif = 0; cur_lif < _BCM_GPORT_NOF_LIFS; ++cur_lif) {

        rv = _bcm_dpp_lif_usage_get(unit, cur_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, &usage, NULL);
        BCMDNX_IF_ERR_EXIT(rv);
        /* if lif used for MPLS termiantion remove it */
        if (usage == _bcmDppLifTypeMplsTunnel) {
            rv = _bcm_dpp_in_lif_mpls_term_match_get(unit, &cur_entry, cur_lif);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_petra_mpls_tunnel_switch_term_get(unit, &cur_entry);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = cb(unit, &cur_entry, user_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* traverse ILM */
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

    /* excluding IPMC compatible entries */
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = _BCM_PETRA_MPLS_TRVRS_ITER_BLK_SIZE;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
    while (!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        soc_sand_rv =
            soc_ppd_frwrd_ilm_get_block(soc_sand_dev_id, &block_range,
                                    _bcm_mpls_traverse_ilm_keys[unit],
                                    _bcm_mpls_traverse_ilm_vals[unit], &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (nof_entries == 0) {
            break;
        }
        for (indx = 0; indx < nof_entries; ++indx) {
            /* get entry */
            rv = _bcm_petra_mpls_tunnel_switch_ilm_entry_to_bcm(unit,
                                                                &_bcm_mpls_traverse_ilm_keys[unit]
                                                                [indx],
                                                                &_bcm_mpls_traverse_ilm_vals[unit]
                                                                [indx],
                                                                &cur_entry);
            BCMDNX_IF_ERR_EXIT(rv);
            /* act on entry */
            rv = cb(unit, &cur_entry, user_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_vpn_id_create(
    int unit,
    bcm_mpls_vpn_config_t * info)
{
    int rv = BCM_E_NONE;
    bcm_vlan_t vpn_id;
    int flags = 0;
    bcm_vlan_control_vlan_t vlan_control;
    _bcm_vsi_type_e vsi_usage_type;
    int update, with_id;
    
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);

    vpn_id = _BCM_DPP_VPN_ID_GET(info->vpn);

    /* not supported flag */
    if ((info->flags & (BCM_MPLS_VPN_L3 | BCM_MPLS_VPN_VPWS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flags BCM_MPLS_VPN_L3 & BCM_MPLS_VPN_VPWS are not supported")));
    }

    /* for vpls user has to call with id */
    if (info->flags & BCM_MPLS_VPN_VPLS) {
        if (!(info->flags & BCM_MPLS_VPN_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VPN MPLS port has to be called with id")));
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VPN MPLS port has to be called with flag BCM_MPLS_VPN_VPLS")));
    }

    /* Read the state of the VPN ID control flags and the VPN ID */
    update = (info->flags & BCM_MPLS_VPN_REPLACE) ? TRUE : FALSE;
    with_id = (info->flags & BCM_MPLS_VPN_WITH_ID) ? TRUE : FALSE;
    vpn_id = _BCM_DPP_VPN_ID_GET(info->vpn);
    rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vpn_id, &vsi_usage_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Validate the VPN ID control flags with the current allocation state */
    if (update) {
        if (!with_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_VPN_REPLACE can't be used without BCM_MPLS_VPN_WITH_ID as well")));
        } else if (vsi_usage_type == _bcmDppVsiTypeAny) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VPN not allocated. In case of Replace assumed VPN is already allocated")));
        } else if (!(vsi_usage_type & _bcmDppVsiTypeMpls)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("The VPN is already allocated but not as an MPLS type")));
        }
    } else if (with_id && (vsi_usage_type & _bcmDppVsiTypeMpls)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case the VSI is already allocated, BCM_MPLS_VPN_REPLACE should be used")));
    }

    /* New VPN - Allocate the VPLS ID in the SW DBs */
    if (!update) {
        /* check if with ID */
        if (with_id) {
            flags |= SHR_RES_ALLOC_WITH_ID;
        }

        /* Add MPLS to the VSI usage and perform alloc manager allocation, if it's an unused VSI */
        rv = _bcm_dpp_vswitch_vsi_usage_alloc(unit, flags, _bcmDppVsiTypeMpls, &vpn_id ,NULL);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    _BCM_DPP_VPN_ID_SET(info->vpn, vpn_id);

    /* Set flooding fields */
    bcm_vlan_control_vlan_t_init(&vlan_control);
    vlan_control.broadcast_group = info->broadcast_group;
    vlan_control.unknown_multicast_group = info->unknown_multicast_group;
    vlan_control.unknown_unicast_group = info->unknown_unicast_group;
    /* forwarding vlan has to be equal to vsi */
    vlan_control.forwarding_vlan = info->vpn;

    rv = bcm_petra_vlan_control_vlan_set(unit,info->vpn,vlan_control);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_vpn_id_destroy(
    int unit,
    bcm_vpn_t vpn)
{
    int vpn_id;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    /* check parameters */
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Delete all the mpls ports on this VPN */
    rv = bcm_petra_mpls_port_delete_all(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    vpn_id = _BCM_DPP_VPN_ID_GET(vpn);

    /* free resource used for VPN (VSI) / VPN ID */
    rv = _bcm_dpp_vswitch_vsi_usage_dealloc(unit, _bcmDppVsiTypeMpls, vpn_id);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_vpn_id_destroy_all(
    int unit)
{
    int vsi;
    int cur_lif;
    _bcm_vsi_type_e mpls_usage;
    int rv = BCM_E_NONE;
    int vpn;
    bcm_vpn_t cur_vpn, vpn_id = 0;
    bcm_gport_t gport;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    /* Delete any MPLS ports on the cross connect VPN(0) */
    rv = bcm_petra_mpls_port_delete_all(unit, 0);
    BCMDNX_IF_ERR_EXIT(rv);

    /* destroy MP usage */
    /* go over, LIF and check get all valid LIF x VPN */
    for (cur_lif = 0; cur_lif < _BCM_GPORT_NOF_LIFS; ++cur_lif) {

        rv = _bcm_dpp_lif_to_vsi(unit, cur_lif, &vsi, TRUE, &gport);
        BCMDNX_IF_ERR_EXIT(rv);

        /* not relevant lif */
        if (vsi == -1) {
            continue;
        }

        rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vsi, &mpls_usage);
        BCMDNX_IF_ERR_EXIT(rv);
        /* not relevant vsi */
        if ((mpls_usage & _bcmDppVsiTypeMpls) == 0) {
            continue;
        }
        /* get vpn */
        _BCM_DPP_VPN_ID_SET(cur_vpn, vsi);
        rv = bcm_petra_mpls_port_delete(unit, cur_vpn, gport);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Destroy MP usage */
    for (vpn = 0; vpn < _BCM_GPORT_NOF_VSIS; ++vpn) {
        rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vpn, &mpls_usage);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Destroy VPNs */
        if (mpls_usage & _bcmDppVsiTypeMpls) {
            _BCM_DPP_VPN_ID_SET(vpn_id, vpn);
            rv = bcm_petra_mpls_vpn_id_destroy(unit, vpn_id);
            BCMDNX_IF_ERR_EXIT(rv);   
        }       
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_vpn_id_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_vpn_config_t * info)
{
    int rv = BCM_E_NONE;
    int vpn_id;
    bcm_vlan_control_vlan_t vlan_control;
    _bcm_vsi_type_e vsi_usage_type;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_NULL_CHECK(info);

    /* clear rest of information */
    bcm_mpls_vpn_config_t_init(info);
    info->flags = BCM_MPLS_VPN_VPLS;
    info->vpn = vpn;

    /* get vsi value */
    vpn_id = _BCM_DPP_VPN_ID_GET(info->vpn);

    /* Verify that the VSI is used for MPLS */
    rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vpn_id, &vsi_usage_type);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!(vsi_usage_type & _bcmDppVsiTypeMpls)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The VPN isn't allocated for MPLS")));
    }

    /* if mp get flooding information */
    bcm_vlan_control_vlan_t_init(&vlan_control);

    rv = bcm_petra_vlan_control_vlan_get(unit,info->vpn,&vlan_control);
    BCMDNX_IF_ERR_EXIT(rv);  

    _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group, _BCM_MULTICAST_TYPE_VPLS, _BCM_MULTICAST_ID_GET(vlan_control.unknown_unicast_group));
    _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group, _BCM_MULTICAST_TYPE_VPLS, _BCM_MULTICAST_ID_GET(vlan_control.unknown_multicast_group));
    _BCM_MULTICAST_GROUP_SET(info->broadcast_group, _BCM_MULTICAST_TYPE_VPLS, _BCM_MULTICAST_ID_GET(vlan_control.broadcast_group));                      

exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_mpls_vpn_t_init not dispatchable */

int bcm_petra_mpls_vpn_traverse(
    int unit,
    bcm_mpls_vpn_traverse_cb cb,
    void *user_data)
{
    int vpn;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_vpn_t cur_vpn;
    int rv;
    uint8 used;

    BCMDNX_INIT_FUNC_DEFS;

    /* traverse MP usage */
    for (vpn = BCM_VLAN_DEFAULT; vpn < _BCM_GPORT_NOF_VSIS; ++vpn) {
        rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vpn);
        if (rv == BCM_E_EXISTS ) {
            rv = _bcm_dpp_vswitch_vsi_usage_type_check(unit, vpn, _bcmDppVsiTypeMpls, &used);
            BCMDNX_IF_ERR_EXIT(rv);
            if (used) {
                _BCM_DPP_VPN_ID_SET(cur_vpn, vpn);

                rv = bcm_petra_mpls_vpn_id_get(unit, cur_vpn, &vpn_info);
                BCMDNX_IF_ERR_EXIT(rv);

                cb(unit, &vpn_info, user_data);
            }
        }
    }

    BCM_EXIT;

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_range_action_add(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high,
    bcm_mpls_range_action_t * action)
{
    int rv = BCM_E_NONE, soc_sand_rv, err_code = 0;
    uint32 vacant_entry_index;

    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO range_profile_info;
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO range_action_info;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_clear(&range_profile_info);
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(&range_action_info);
    
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(action);
    
    if (action->flags & BCM_MPLS_RANGE_ACTION_COMPRESSED) {
        if (action->compressed_label != label_low) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("compressed_label must equal label_low in case BCM_MPLS_RANGE_ACTION_COMPRESSED is on")));
        }
        range_profile_info.mpls_label_range_use_base = 1;
    }

    if (!(label_high >= label_low)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("label_high can't be smaller than label_low")));
    }
    
    rv = _bcm_dpp_mpls_range_action_verify(unit,  label_low, label_high,&vacant_entry_index,&err_code);
    BCMDNX_IF_ERR_EXIT(rv);
    if (err_code == _BCM_MPLS_TERM_LABEL_RANGE_EXIST) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Given label range already exists")));
    } else if (err_code == _BCM_MPLS_TERM_LABEL_RANGE_INTERSECT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given label range intersects with an already allocated one")));
    } else if (err_code == _BCM_MPLS_TERM_LABEL_RANGE_TABLE_FULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tabel is full")));
    }

    range_action_info.label_low = label_low;
    range_action_info.label_high = label_high;

    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_action_set,(unit, vacant_entry_index, &range_action_info)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_profile_set,(unit, vacant_entry_index, &range_profile_info)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_range_action_remove(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high)
{
    int rv = BCM_E_NONE, soc_sand_rv, err_code = 0;
    uint32 entry_index;
    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO range_profile_info;
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO range_action_info;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_clear(&range_profile_info);
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(&range_action_info);
    
    BCM_DPP_UNIT_CHECK(unit);

    if (!(label_high >= label_low)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("label_high can't be smaller than label_low")));
    }
    
    rv = _bcm_dpp_mpls_range_action_verify(unit, label_low, label_high,&entry_index,&err_code);
    BCMDNX_IF_ERR_EXIT(rv);
    if (err_code  != _BCM_MPLS_TERM_LABEL_RANGE_EXIST) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given label range doesn't exist in the table")));
    }

    /* Placing zeros in the relevant entry */
    range_action_info.bos_value_mask = 0;
    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_action_set,(unit, entry_index, &range_action_info)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_profile_set,(unit, entry_index, &range_profile_info)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_range_action_get(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high,
    bcm_mpls_range_action_t *action
    )
{
    int rv = BCM_E_NONE, soc_sand_rv, err_code = 0;
    uint32 entry_index;

    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO range_profile_info;
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO range_action_info;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_clear(&range_profile_info);
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(&range_action_info);
    
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(action);

    if (!(label_high >= label_low)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("label_high can't be smaller than label_low")));
    }
    
    rv = _bcm_dpp_mpls_range_action_verify(unit, label_low, label_high,&entry_index,&err_code);
    BCMDNX_IF_ERR_EXIT(rv);
    if (err_code  != _BCM_MPLS_TERM_LABEL_RANGE_EXIST) {
        action->compressed_label = 0;
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given label range doesn't exist in the table")));
    }

    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_profile_get,(unit, entry_index, &range_profile_info)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
    if (range_profile_info.mpls_label_range_use_base == 1) {
        action->compressed_label = label_low;
        action->flags |= BCM_MPLS_RANGE_ACTION_COMPRESSED; 
    }

exit:
    BCMDNX_FUNC_RETURN;
}
 
/* 
 * This is an auxilliary function that receives a range of mpls labels and returns a value err_code
 * that signifies the presence of this range in the table. 
 * Optional return values: 
 *  err_code == _BCM_MPLS_TERM_LABEL_RANGE_TABLE_FULL : Entries are exhausted
 *  err_code == _BCM_MPLS_TERM_LABEL_RANGE_EXIST: given label range exists in a certain entry.
 *  err_code == _BCM_MPLS_TERM_LABEL_RANGE_INTERSECT: given label range intersects with a certain (label in) entry
 *  err_code == _BCM_MPLS_TERM_LABEL_RANGE_VACANT_ENTRY: There is a vacant entry in the table, to which this label can be assigned.
 *  The vacant entry will be returned in vacant_entry_index.
 */
int _bcm_dpp_mpls_range_action_verify(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high,
    uint32 * vacant_entry_index,
    int *err_code)
{
    uint32 entry_index;
    int soc_sand_rv, found = 0;
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO range_action_info;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(&range_action_info);

    BCM_DPP_UNIT_CHECK(unit);

    *err_code = _BCM_MPLS_TERM_LABEL_RANGE_TABLE_FULL;

    for (entry_index = 0; entry_index < _BCM_MPLS_TERM_LABEL_RANGE_NOF_ENTRIES; entry_index++) {
        soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_action_get,(unit, entry_index, &range_action_info)));
        BCMDNX_IF_ERR_EXIT(soc_sand_rv);
        /* Checks whether the given range is identical to the range in the current entry*/
        if (range_action_info.label_low == label_low && range_action_info.label_high == label_high) {
            *vacant_entry_index = entry_index;
            *err_code = _BCM_MPLS_TERM_LABEL_RANGE_EXIST;
            break; 
            
        }
        /* Checks whether the given range intersects with the range in the current entry*/
        if ((range_action_info.label_low <= label_low &&  range_action_info.label_high >= label_low) || \
            (range_action_info.label_low <= label_high &&  range_action_info.label_high >= label_high) ) {
            *vacant_entry_index = entry_index;
            *err_code = _BCM_MPLS_TERM_LABEL_RANGE_INTERSECT;
            break; 
        }
        /* Saving an empty entry */
        if (!range_action_info.label_low && !range_action_info.label_high && !found) {
            found = 1;
            *vacant_entry_index = entry_index;
            *err_code = _BCM_MPLS_TERM_LABEL_RANGE_VACANT_ENTRY;
        }
    }
 
exit:
    BCMDNX_FUNC_RETURN;
} 


