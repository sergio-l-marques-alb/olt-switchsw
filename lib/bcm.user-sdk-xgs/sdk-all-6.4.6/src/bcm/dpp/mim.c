/*
 * $Id: mim.c,v 1.72 Broadcom SDK $
 *
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
 * Soc_petra-B Layer 2 Management
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_MIM
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm/module.h>
#include <bcm/field.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/mim.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/switch.h>

#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/wb_db_gport.h>

#include <shared/shr_resmgr.h>

#include <soc/dpp/PPD/ppd_api_mymac.h>
#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>

#if LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_bmact.h>
#endif

#ifdef  BCM_ARAD_SUPPORT
    #include <soc/dpp/ARAD/arad_sw_db.h>
#endif

#include <soc/dpp/mbcm.h>


/* 
 * Globals
 */

#ifdef BCM_PETRAB_SUPPORT
/* Flag to check initialized status */
static int dpp_mim_initialized[BCM_LOCAL_UNITS_MAX] = {0};
#endif

/*
 * MiM Module Helper functions
 */

uint8
  __dpp_mim_initialized_get(int unit)
{
    uint8 initialized = 0;
    if ((unit >= 0) && (unit < BCM_LOCAL_UNITS_MAX)) {
#ifdef BCM_PETRAB_SUPPORT
        /* write mim_out_ac to default_sem_index reg */
        if (SOC_IS_PETRAB(unit)) {
            initialized = dpp_mim_initialized[unit];
        } else {
#endif
            if (SOC_DPP_PP_ENABLE(unit)) {
                soc_ppd_frwrd_mact_mim_init_get(unit, &initialized);
            }
#ifdef BCM_PETRAB_SUPPORT
        }
#endif
    }
    return initialized;
}

SOC_PPD_LIF_ID
  __dpp_mim_lif_ndx_get(int unit)
{
    return SOC_DPP_CONFIG(unit)->pp.mim_global_lif_ndx;
}

SOC_PPD_AC_ID
  __dpp_mim_global_out_ac_get(int unit)
{
    /* No default out ac in AVT mode*/
    return ((SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) ? 0 : SOC_DPP_CONFIG(unit)->pp.mim_global_out_ac);
}


/*
 * Function:
 *      dpp_mim_set_global_mim_tpid
 * Purpose:
 *      set the I-tag TPID to MiM TPID
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int dpp_mim_set_global_mim_tpid(int unit){

    int soc_sand_rv;
    SOC_PPD_LLP_PARSE_TPID_VALUES tpid_vals;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    tpid_vals.tpid_vals[4] = BCM_PETRA_MIM_ITAG_TPID;
    
    soc_sand_rv = soc_ppd_llp_parse_tpid_values_set(unit, &tpid_vals);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      bcm_petra_mim_init
 * Purpose:
 *      Initialize the MIM software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_mim_init(int unit)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO profile_info;
    SOC_PPD_EG_AC_INFO ac_info;
    bcm_dpp_am_local_inlif_info_t local_inlif_info;
    bcm_dpp_am_local_out_lif_info_t local_out_lif_info;


    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);                      

    if (__dpp_mim_initialized_get(unit)) {
         BCM_EXIT;
    }
    /* Enable MiM */

    if (!SOC_WARM_BOOT(unit)) {

        if (SOC_IS_ARAD(unit)) {
            if (!SOC_IS_ARADPLUS(unit)) {
                if ((SOC_DPP_CONFIG(unit)->pp.next_hop_mac_extension_enable)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("next hop mac extension soc property and mim do not coexist")));                
                }
            }
        }

        soc_sand_dev_id = (unit);

        if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) { /* Standard vlan translate mode has a default ac for MIM, while AVT mode doesn't */
        
            if (SOC_IS_JERICHO(unit)) {

                sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));
                sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

                /* 
                 *  Allocate mim ingress global lif.
                 */

                /* Set the local lif info */
                local_inlif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_COMMON;
                local_inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_ac;

                /* Allocate the global and loca lif */
                rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, (int*)&SOC_DPP_CONFIG(unit)->pp.mim_global_lif_ndx, &local_inlif_info, NULL, FALSE);
                BCM_IF_ERROR_RETURN(rv);

                /* Retrieve the allocated local lif */
                MIM_ACCESS.mim_local_lif_ndx.set(unit, local_inlif_info.base_lif_id);


                /* 
                 * Allocate the egress mim global lif.
                 */                                   

                /* Set the local lif info */
                local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eg_out_ac;
                local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_out_ac;

                /* Allocate the global and local lif */
                rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, (int*)&SOC_DPP_CONFIG(unit)->pp.mim_global_out_ac, NULL, &local_out_lif_info, TRUE);
                BCM_IF_ERROR_RETURN(rv);

                /* Retrieve the allocated local lif */
                MIM_ACCESS.mim_local_out_ac.set(unit, local_out_lif_info.base_lif_id);

            }
            if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_PETRAB(unit)) {
                /* alloc a special lif, used only fof MiM B-VID -> B-VSI */
                rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_VLAN_EDITING, SHR_RES_ALLOC_WITH_ID, &SOC_DPP_CONFIG(unit)->pp.mim_global_lif_ndx);
                BCMDNX_IF_ERR_EXIT(rv);

                MIM_ACCESS.mim_local_lif_ndx.set(unit, SOC_DPP_CONFIG(unit)->pp.mim_global_lif_ndx);

                /* alloc a special out-ac, used only fof MiM */
                if (SOC_IS_PETRAB(unit)) {
                    rv = bcm_dpp_am_l2_ac_alloc(unit, 0, SHR_RES_ALLOC_WITH_ID, &SOC_DPP_CONFIG(unit)->pp.mim_global_out_ac);
                } else {
                    rv = bcm_dpp_am_out_ac_alloc(unit, _BCM_DPP_AM_OUT_AC_TYPE_DEFAULT, SHR_RES_ALLOC_WITH_ID, &SOC_DPP_CONFIG(unit)->pp.mim_global_out_ac);
                }
                BCMDNX_IF_ERR_EXIT(rv);

                MIM_ACCESS.mim_local_out_ac.set(unit, SOC_DPP_CONFIG(unit)->pp.mim_global_out_ac);

            }

            

            if (SOC_IS_ARAD(unit)) {
                SOC_PPD_AC_ID  mim_local_out_ac; 
                MIM_ACCESS.mim_local_out_ac.get(unit, &mim_local_out_ac); 
                /* set out-ac to point at default profile */
                SOC_PPD_EG_AC_INFO_clear(&ac_info);
                ac_info.edit_info.nof_tags = 2; /* nof tags must be 2 */
                soc_sand_rv = soc_ppd_eg_ac_info_set(soc_sand_dev_id, mim_local_out_ac, &ac_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

        }
        
        soc_sand_rv = soc_ppd_frwrd_bmact_init(soc_sand_dev_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

#ifdef BCM_PETRAB_SUPPORT
        /* write mim_out_ac to default_sem_index reg */
        if (SOC_IS_PETRAB(unit)) {
            SOC_PPD_AC_ID  mim_local_out_ac; 
            MIM_ACCESS.mim_local_out_ac.get(unit, &mim_local_out_ac); 
            soc_sand_rv = soc_pb_pp_frwrd_bmact_default_sem_index_set(soc_sand_dev_id, mim_local_out_ac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
#endif
        /* in ARAD the deafult out_ac is per_port and will be done in bcm_petra_port_control_set() with bcmPortControlMacInMac */
        
        /* set the I-tag TPID */
        rv = dpp_mim_set_global_mim_tpid(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        
        if (SOC_IS_PETRAB(unit)) {
            
            /* set the UNKNOWN_DA_7 trap profile */
            SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);
            
            profile_info.bitmap_mask |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
            profile_info.dest_info.frwrd_dest.type = SOC_PPD_FRWRD_DECISION_TYPE_MC;    
            profile_info.dest_info.add_vsi = TRUE;
            profile_info.dest_info.frwrd_dest.dest_id = BCM_PETRA_MIM_BVID_MC_GROUP_BASE;
            profile_info.processing_info.da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC;
            
            soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id, _BCM_DPP_BVID_UNKNOWN_DA_FLD_TRAP, &profile_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }


#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {
            dpp_mim_initialized[unit] = TRUE;
        } else {
#endif
            soc_sand_rv = soc_ppd_frwrd_mact_mim_init_set(soc_sand_dev_id, TRUE);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);           
#ifdef BCM_PETRAB_SUPPORT
        }
#endif

    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_mim_detach(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    if (!__dpp_mim_initialized_get(unit)) {
         BCM_EXIT;
    }

exit:
    BCMDNX_FUNC_RETURN;

}

/*
 * Function:
 *      bcm_petra_mim_vpn_create
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_mim_vpn_create(
    int unit, 
    bcm_mim_vpn_config_t *info)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    int bc, bc_type, uc, uc_type, mc, mc_type;
    uint32 flood_vpn, vpn = info->vpn; /* save initial vpn */
    SOC_PPD_VSI_DEFAULT_FRWRD_KEY dflt_frwrd_key;
    SOC_PPD_ACTION_PROFILE action_profile;
    SOC_PPD_BMACT_BVID_INFO bvid_info;
    bcm_vlan_control_vlan_t control;
    SOC_PPD_SYS_VSI_ID vsi_id, tmp_vsi;
    SOC_PPD_L2_LIF_ISID_KEY isid_key;
    SOC_SAND_PP_ISID old_lookup_id;
    SOC_PPD_LIF_ID tmp_lif, isid_local_lif_id = 0;
    int isid_global_lif_id = 0;
    SOC_PPD_L2_LIF_ISID_INFO isid_info;
    SOC_SAND_SUCCESS_FAILURE success;
    uint8 found, replaced;
    uint8 update;
    uint32 l2_ac_alloc_flags = 0;
    bcm_mim_port_t mim_port;
    bcm_dpp_am_local_inlif_info_t local_inlif_info;    

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);

    /* make sure MiM is enabled */
    MIM_INIT(unit);

    /* function must be called with VPN id */
    if (!(info->flags & BCM_MIM_VPN_WITH_ID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIM_VPN_WITH_ID must be set")));
    }
    
    update = ((info->flags & BCM_MIM_VPN_REPLACE) ? TRUE : FALSE);
    
    /* replace is not supported */
    if (update && (SOC_IS_PETRAB(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIM_VPN_REPLACE is not supported")));
    }
    

    soc_sand_dev_id = (unit);

    if (info->flags & BCM_MIM_VPN_BVLAN) { /* B-VID */

        /* add 0x1111 to B-VID (to distinguish from vlan) */
        info->vpn = _BCM_DPP_VLAN_TO_BVID(info->vpn);

        /* set flooding for B-VID */
        bc_type = _BCM_MULTICAST_TYPE_GET(info->broadcast_group);
        bc = _BCM_MULTICAST_ID_GET(info->broadcast_group);
        mc_type = _BCM_MULTICAST_TYPE_GET(info->unknown_multicast_group);
        mc = _BCM_MULTICAST_ID_GET(info->unknown_multicast_group);
        uc_type = _BCM_MULTICAST_TYPE_GET(info->unknown_unicast_group);
        uc = _BCM_MULTICAST_ID_GET(info->unknown_unicast_group);

        /* all groups should be equal to B-VID+12K and of type MIM */
        flood_vpn = vpn + BCM_PETRA_MIM_BVID_MC_GROUP_BASE;
        if ((bc_type != _BCM_MULTICAST_TYPE_MIM) ||
            (mc_type != _BCM_MULTICAST_TYPE_MIM) ||
            (uc_type != _BCM_MULTICAST_TYPE_MIM) ||
            (bc != flood_vpn) || (mc != flood_vpn) || (uc != flood_vpn)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("broadcast_group, unknown_multicast_group and unknown_unicast_group must be "
                                                    "of type BCM_MULTICAST_TYPE_MIM and equal to vpn+12K")));
        }
     
        if (SOC_IS_PETRAB(unit)) {

            /* Set action profile for UNKNOWN_DA_7 for UC/MC/BC */
            SOC_PPD_VSI_DEFAULT_FRWRD_KEY_clear(&dflt_frwrd_key);
            SOC_PPD_ACTION_PROFILE_clear(&action_profile);

            dflt_frwrd_key.default_forward_profile = DPP_BVID_DEFAULT_FRWRD_PROFILE;

            action_profile.trap_code = _BCM_DPP_BVID_UNKNOWN_DA_FLD_TRAP;
            action_profile.frwrd_action_strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
            action_profile.snoop_action_strength = 0;

            for (dflt_frwrd_key.orientation = 0; dflt_frwrd_key.orientation < SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS; ++dflt_frwrd_key.orientation) {
                for (dflt_frwrd_key.da_type = 0; dflt_frwrd_key.da_type < SOC_SAND_PP_NOF_ETHERNET_DA_TYPES; ++dflt_frwrd_key.da_type) {
                    soc_sand_rv = soc_ppd_vsi_default_frwrd_info_set(soc_sand_dev_id, &dflt_frwrd_key, &action_profile);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }

            /* set B-VID profile to its default profile */
            soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_get(soc_sand_dev_id, vpn, &bvid_info); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            bvid_info.default_forward_profile = DPP_BVID_DEFAULT_FRWRD_PROFILE;

            soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_set(soc_sand_dev_id, vpn, &bvid_info); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        }
        else { /* Arad */
            /* set B-VID FID class */
            soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_get(soc_sand_dev_id, vpn, &bvid_info); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            bvid_info.b_fid_profile = SOC_PPC_BFID_EQUAL_TO_BVID; /* B-FID = B-VID */
            SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(soc_sand_dev_id, &(bvid_info.uknown_da_dest), flood_vpn, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_set(soc_sand_dev_id, vpn, &bvid_info); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        /* set control to independent VLAN Learning */
        bcm_vlan_control_vlan_t_init(&control);

        control.forwarding_vlan = info->vpn;

        rv = bcm_petra_vlan_control_vlan_set(unit, info->vpn, control);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else { /* VSI */
        /* allocate VSI */

        if (!update) {
            /* allocate VSI for MIM usage */
            rv = _bcm_dpp_vswitch_vsi_usage_alloc(unit,SHR_RES_ALLOC_WITH_ID,_bcmDppVsiTypeMim,&info->vpn,&replaced);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        vsi_id = info->vpn;

        
        if (!update) {
            /* 
             * VSI is uniquely identified by I-SID
             * check if VSI<->I-SID mapping already exists
             */
            SOC_PPD_L2_LIF_ISID_KEY_clear(&isid_key);
            isid_key.isid_domain = 0; /* isid_domain is always 0 */
            isid_key.isid_id = info->lookup_id;

            soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &tmp_lif, &tmp_vsi, &isid_info, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("lookup_id is not unique to this VPN")));
            }
        } else {
            soc_sand_rv = soc_ppd_l2_lif_vsi_to_isid(soc_sand_dev_id, vsi_id, &old_lookup_id);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            SOC_PPD_L2_LIF_ISID_KEY_clear(&isid_key);
            isid_key.isid_domain = 0; /* isid_domain is always 0 */
            isid_key.isid_id = old_lookup_id;

            soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &isid_local_lif_id, &vsi_id, &isid_info, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("lookup_id is not previous configured to this VPN")));
            }            
            isid_global_lif_id = isid_info.global_lif;
        }

        /* set flooding for VSI */
        bcm_vlan_control_vlan_t_init(&control);

        control.forwarding_vlan = info->vpn;

        control.broadcast_group = info->broadcast_group;
        control.unknown_multicast_group = info->unknown_multicast_group;
        control.unknown_unicast_group = info->unknown_unicast_group;

        rv = bcm_petra_vlan_control_vlan_set(unit, info->vpn, control);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         * map VSI to I-SID and I-SID to VSI 
         * set default forwarding as drop 
         * update B-tag TPID index 
         */

        /* allocate LIF */
/*
 * COVERITY
 *
 * The variable isid_lif_id is assigned in bcm_dpp_am_l2_ac_alloc
 */
/* coverity[uninit_use_in_call] */
        if(info->flags & BCM_MIM_VPN_SERVICE_ENCAP_WITH_ID) {
            l2_ac_alloc_flags |= SHR_RES_ALLOC_WITH_ID;
            isid_global_lif_id = BCM_GPORT_TUNNEL_ID_GET(info->service_encap_id);
        }

        if (!update) {
            if (SOC_IS_JERICHO(unit)) {

                /* Set the inlif parameters */
                sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));
                local_inlif_info.local_lif_flags = DPP_IS_FLAG_SET(info->flags, BCM_MIM_VPN_SERVICE_ENCAP_INGRESS_WIDE) ? 
                                                                BCM_DPP_AM_IN_LIF_FLAG_WIDE : BCM_DPP_AM_IN_LIF_FLAG_COMMON;
                local_inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_isid;


                /* Allocate the global and local lif */
                rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, l2_ac_alloc_flags, &isid_global_lif_id, &local_inlif_info, NULL, FALSE);
                BCM_IF_ERROR_RETURN(rv);

                /* Retrieve the allocated local lif */
                isid_local_lif_id = local_inlif_info.base_lif_id;

            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_PETRAB(unit)) {
                rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_ISID, l2_ac_alloc_flags, (SOC_PPD_LIF_ID*)&isid_global_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
                isid_local_lif_id = isid_global_lif_id;
            }
        } else { /* update */
            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, isid_global_lif_id, (int*)&isid_local_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        BCM_GPORT_TUNNEL_ID_SET(info->service_encap_id, isid_global_lif_id);

        SOC_PPD_L2_LIF_ISID_INFO_clear(&isid_info);
        isid_info.service_type = SOC_PPD_L2_LIF_ISID_SERVICE_TYPE_MP;
        isid_info.learn_enable = TRUE;
        isid_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
        isid_info.default_frwrd.default_frwd_type = SOC_PPD_L2_LIF_DFLT_FRWRD_SRC_VSI;
        isid_info.default_frwrd.default_forwarding.type = SOC_PPD_FRWRD_DECISION_TYPE_DROP;
        isid_info.global_lif = isid_global_lif_id;
        isid_info.is_extended = (info->flags & BCM_MIM_VPN_SERVICE_ENCAP_INGRESS_WIDE) ? TRUE:FALSE;

        if (!update) {
            soc_sand_rv = soc_ppd_l2_lif_isid_add(soc_sand_dev_id, vsi_id, &isid_key, isid_local_lif_id, &isid_info, &success);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            if (old_lookup_id != info->lookup_id) {                
                /*update the lookup_id */
                SOC_PPD_L2_LIF_ISID_KEY_clear(&isid_key);
                isid_key.isid_domain = 0; /* isid_domain is always 0 */
                isid_key.isid_id = info->lookup_id;
                
                soc_sand_rv = soc_ppd_l2_lif_isid_add(soc_sand_dev_id, vsi_id, &isid_key, isid_local_lif_id, &isid_info, &success);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                
                /*update the lookup_id */
                SOC_PPD_L2_LIF_ISID_KEY_clear(&isid_key);
                isid_key.isid_domain = 0; /* isid_domain is always 0 */
                isid_key.isid_id = old_lookup_id;

                /* remove the isid */            
                soc_sand_rv = soc_ppd_l2_lif_isid_remove(soc_sand_dev_id, &isid_key, &isid_local_lif_id);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }                

        /* for 32k ISIDs, store the VSI->ISID mapping in the SW DB (will be set in HW when the vlan ports are attached to the VSI) */
        #ifdef BCM_ARAD_SUPPORT
        if (SOC_IS_ARAD(unit)) {
            if (soc_property_get(unit, spn_MIM_NUM_VSIS, 0) == 32768) {
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.vsi.vsi_to_isid.set(soc_sand_dev_id, vsi_id, info->lookup_id));
            }
        }
        #endif

        /*
         * update TPID index for B-tag TPID
         * this uses the I-SID Lif and therfore must be done after lif_isid_add
         */
        rv = _bcm_petra_mim_tpid_profile_update(unit, isid_local_lif_id, info->match_service_tpid);
        BCMDNX_IF_ERR_EXIT(rv);    

        /* save type and LIF-ID data - for restoration */
        bcm_mim_port_t_init(&mim_port);
        mim_port.mim_port_id = info->service_encap_id;
        rv = _bcm_dpp_in_lif_mim_match_add(unit, &mim_port, isid_local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);

    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_mim_vpn_get(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_mim_vpn_config_t *info)
{
    int rv = BCM_E_NONE;
    bcm_mim_vpn_t vid, flood_vid;
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    bcm_vlan_control_vlan_t control;
    SOC_PPD_SYS_VSI_ID vsi_id;
    SOC_SAND_PP_ISID isid;
    SOC_PPD_L2_LIF_ISID_KEY isid_key;
    SOC_PPD_LIF_ID local_lif_index;
    int global_lif_index;
    SOC_PPD_L2_LIF_ISID_INFO isid_info;
    uint8 found;
    uint8 is_wide_entry;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    SOC_PPD_L2_LIF_ISID_KEY_clear(&isid_key);        

    /* make sure MiM is enabled */
    MIM_INIT(unit);

    soc_sand_dev_id = (unit);

    if (_BCM_DPP_VLAN_IS_BVID(vpn)) { /* B-VID */
        /* remove 0x1111 from B-VID */
        vid = _BCM_DPP_BVID_TO_VLAN(vpn);

        /* set the groups */
        flood_vid = vid + BCM_PETRA_MIM_BVID_MC_GROUP_BASE;
        _BCM_MULTICAST_GROUP_SET(info->broadcast_group, _BCM_MULTICAST_TYPE_MIM, flood_vid);
        _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group, _BCM_MULTICAST_TYPE_MIM, flood_vid);
        _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group, _BCM_MULTICAST_TYPE_MIM, flood_vid);
    }
    else { /* VSI */

        /* get flooding for VSI */
        rv = bcm_petra_vlan_control_vlan_get(unit, vpn, &control);
        BCMDNX_IF_ERR_EXIT(rv);

        info->broadcast_group = control.broadcast_group;
        info->unknown_multicast_group = control.unknown_multicast_group;
        info->unknown_unicast_group = control.unknown_unicast_group;

        /* get the I-SID from the VSI */
        vsi_id = vpn;
        soc_sand_rv = soc_ppd_l2_lif_vsi_to_isid(soc_sand_dev_id, vsi_id, &isid);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        info->lookup_id = isid;
        isid_key.isid_domain = 0; /* isid_domain is always 0 */
        isid_key.isid_id = isid;

        soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &local_lif_index, &vsi_id, &isid_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_lif_index, &global_lif_index);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Get wide entry info */
        if (SOC_IS_JERICHO(unit)) {

            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_is_wide_entry,(unit, local_lif_index, TRUE, &is_wide_entry));
            BCMDNX_IF_ERR_EXIT(rv);

            info->flags |= (is_wide_entry) ? BCM_MIM_VPN_SERVICE_ENCAP_INGRESS_WIDE : 0;
        }

        BCM_GPORT_TUNNEL_ID_SET(info->service_encap_id, global_lif_index);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_mim_vpn_destroy(
    int unit, 
    bcm_mim_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    SOC_PPD_VSI_DEFAULT_FRWRD_KEY dflt_frwrd_key;
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO profile_info;
    SOC_PPD_ACTION_PROFILE action_profile;
    SOC_PPD_L2_LIF_ISID_KEY isid_key;
    bcm_pbmp_t pbmp_sum, pbmp,ubmp;
    SOC_SAND_PP_ISID isid_id;
    SOC_PPD_LIF_ID isid_local_lif_id;
    int isid_global_lif_id;
    bcm_if_t service_encap_id; 

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);

    /* make sure MiM is enabled */
    MIM_INIT(unit);

    soc_sand_dev_id = (unit);

    /* reset the flooding */
    if (_BCM_DPP_VLAN_IS_BVID(vpn)) { /* B-VID */

        /* set the trap profile to default settings */
        SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);

        profile_info.bitmap_mask |= SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
        profile_info.dest_info.frwrd_dest.type = SOC_PPD_FRWRD_DECISION_TYPE_UC_FLOW;    
        profile_info.dest_info.add_vsi = FALSE;
        profile_info.dest_info.frwrd_dest.dest_id = 0;

        if (SOC_IS_PETRAB(unit)) {

            soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id, _BCM_DPP_BVID_UNKNOWN_DA_FLD_TRAP, &profile_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
     
            /* Set action profile for UNKNOWN_DA_7 for UC/MC/BC */
            SOC_PPD_VSI_DEFAULT_FRWRD_KEY_clear(&dflt_frwrd_key);
            SOC_PPD_ACTION_PROFILE_clear(&action_profile);

            dflt_frwrd_key.default_forward_profile = DPP_BVID_DEFAULT_FRWRD_PROFILE;

            action_profile.trap_code = _BCM_DPP_BVID_UNKNOWN_DA_FLD_TRAP;
            action_profile.frwrd_action_strength = 0;
            action_profile.snoop_action_strength = 0;

            for (dflt_frwrd_key.orientation = 0; dflt_frwrd_key.orientation < SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS; ++dflt_frwrd_key.orientation) {
                for (dflt_frwrd_key.da_type = 0; dflt_frwrd_key.da_type < SOC_SAND_PP_NOF_ETHERNET_DA_TYPES; ++dflt_frwrd_key.da_type) {
                    soc_sand_rv = soc_ppd_vsi_default_frwrd_info_set(soc_sand_dev_id, &dflt_frwrd_key, &action_profile);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }
    }
    else { /* VSI */

        /* BCM_DPP_VLAN_CHK_ID(unit, vpn);*/

        /* Validate VSI does not exist */
        rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vpn);
        if (rv == BCM_E_NOT_FOUND) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI does not exists")));
        }

        /* Clear vlan membership */
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_CLEAR(ubmp);
        BCM_PBMP_CLEAR(pbmp_sum);

        rv = bcm_petra_vlan_port_get(unit, vpn, &pbmp, &ubmp);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_OR(pbmp_sum, ubmp);
        BCM_PBMP_OR(pbmp_sum, pbmp);
    
        rv = bcm_petra_vlan_port_remove(unit, vpn, pbmp);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         * VSI is uniquely identified by I-SID 
         * get the I-SID mapped to this VSI 
         */
        soc_sand_rv = soc_ppd_l2_lif_vsi_to_isid(soc_sand_dev_id, vpn, &isid_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        SOC_PPD_L2_LIF_ISID_KEY_clear(&isid_key);
        isid_key.isid_domain = 0; /* isid_domain is always 0 */
        isid_key.isid_id = isid_id;

        /* 
         * remove VSI<->I-SID mapping
         * this will also return the isid_lif_id 
         */ 
        soc_sand_rv = soc_ppd_l2_lif_isid_remove(soc_sand_dev_id, &isid_key, &isid_local_lif_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, isid_local_lif_id, &isid_global_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);


        if(SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
            BCM_GPORT_TUNNEL_ID_SET(service_encap_id, isid_global_lif_id);
            rv = bcm_petra_port_tpid_delete_all(unit, service_encap_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            /* isid_lif_id should no longer point at TPID index for B-tag TPID */
            rv = _bcm_petra_mim_tpid_profile_update(unit, isid_local_lif_id, 0);
            BCMDNX_IF_ERR_EXIT(rv);  
        }

        /* Deallocate isid_lif_id */
        if (SOC_IS_JERICHO(unit)) {
            rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, isid_global_lif_id, isid_local_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID, FALSE);
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_PETRAB(unit)) {
            rv = bcm_dpp_am_l2_ac_dealloc(unit, 0, isid_local_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* remove mim lif match SW */
        rv = _bcm_dpp_in_lif_match_delete(unit, isid_local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv =  _bcm_dpp_vswitch_vsi_usage_dealloc(unit, _bcmDppVsiTypeMim, vpn);
        BCMDNX_IF_ERR_EXIT(rv);

    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_mim_port_add
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_mim_port_add(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_mim_port_t * mim_port)
{
    int rv, soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_SAND_PP_MAC_ADDRESS src_bmac;
    SOC_PPD_FEC_ID fec_id /* work */, protect_fec_id;
    uint32 fec_flags = 0, eep_flags = 0, lif_flags = 0;
    int value, sec_fec = 0, is_protected, update = 0;
    uint32 nof_entries, next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    SOC_PPD_EG_ENCAP_LL_INFO ll_encap_info, *ll_encap_info_ptr;
    SOC_PPD_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    SOC_PPD_BMACT_ENTRY_KEY bmac_key;
    SOC_PPD_BMACT_ENTRY_INFO bmact_entry_info;
    uint8 found;
    SOC_SAND_SUCCESS_FAILURE success;
    _BCM_GPORT_PHY_PORT_INFO *phy_port = NULL;
    _BCM_GPORT_PHY_PORT_INFO *remove_phy_port = NULL;
    uint32 mac_addr_arr[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
    SOC_PPD_LIF_ID local_in_lif_id;
    int global_in_lif_id, global_out_lif_id = 0, local_out_lif_id;
    SOC_PPD_L2_LIF_ISID_KEY isid_key;
    SOC_PPD_L2_LIF_ISID_INFO isid_info;
    SOC_PPD_SYS_VSI_ID vsi_index;
    SOC_SAND_PP_VLAN_ID out_vlan = mim_port->egress_tunnel_vlan;    
    uint32  soc_ppd_port;
    int local_port, core;
    _bcm_dpp_gport_info_t gport_info;
    _bcm_lif_type_e
        usage;
    bcm_dpp_am_local_inlif_info_t local_inlif_info;
    bcm_dpp_am_local_out_lif_info_t local_out_lif_info;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);

    MIM_INIT(unit);

    /* check invalid flags and match criteria */
    

    soc_sand_dev_id = (unit);

    /* check that port is PBP enabled */
    rv = bcm_petra_port_control_get(
        unit,
        mim_port->port,
        bcmPortControlMacInMac,
        &value);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!value) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("port is not set as bcmPortControlMacInMac")));
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, mim_port->port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    is_protected = _BCM_PPD_IS_VALID_FAILOVER_ID(mim_port->failover_id);

    fec_id = BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id);

    if (is_protected && mim_port->failover_gport_id != BCM_GPORT_TYPE_NONE) {

        /* if this is working fec, failover-gport has to be protected otherwise return error */
        rv = _bcm_dpp_gport_is_protected(unit, mim_port->failover_gport_id, &is_protected);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!is_protected) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("failover_id is valid but there is no failover_gport_id")));
        }

        /* this gport is second gport in protection */
        protect_fec_id =  BCM_GPORT_MIM_PORT_ID_GET(mim_port->failover_gport_id);
        fec_id = _BCM_PPD_GPORT_PROTECT_TO_WORK_FEC(protect_fec_id);
        sec_fec = 1;
    }

    if (mim_port->flags & BCM_MIM_PORT_REPLACE){
        if (mim_port->flags & BCM_MIM_PORT_WITH_ID) {

            /* fec is already allocated - get info */
            if (!BCM_GPORT_IS_MIM_PORT((mim_port->mim_port_id))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("gport mim_port_id is not MIM")));
            }
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot replace port when port ID is not supplied")));
        }
        update = 1;
    }
    else {
        /* allocate FEC */
        if (mim_port->flags & BCM_MIM_PORT_WITH_ID) {
            fec_flags |= SHR_RES_ALLOC_WITH_ID;                    
        }
        if (is_protected) {
            if (!sec_fec) {
                /* first fec in protection - allocate 2 fec entries */
                rv = bcm_dpp_am_fec_alloc(unit, fec_flags, 0, 2, &fec_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            /* if sec_fec then work & protect fec are already allocated */
        }
        else {
            /* allocate 1 fec entry */
            rv = bcm_dpp_am_fec_alloc(unit, fec_flags, 0, 1, &fec_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* return FEC to user */
        BCM_GPORT_MIM_PORT_ID_SET(mim_port->mim_port_id, fec_id);
    }

    if (!update) {
        /* allocate EEP and return to user */
        if (mim_port->flags & BCM_MIM_PORT_ENCAP_WITH_ID) {
            eep_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
            global_out_lif_id = mim_port->encap_id;
        }
        if (SOC_IS_JERICHO(unit)) {
            sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

            /* Set the local lif info */
            local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eep_global;
            local_out_lif_info.app_alloc_info.application_type = 0;

            /* Allocate the global and local lif */
            rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, eep_flags, &global_out_lif_id, NULL, &local_out_lif_info, TRUE);
            BCM_IF_ERROR_RETURN(rv);

            /* Retrieve the allocated local lif */
            local_out_lif_id = local_out_lif_info.base_lif_id;


        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = bcm_dpp_am_l3_eep_alloc(unit, _BCM_DPP_AM_L3_EEP_TYPE_DEFAULT, eep_flags, &global_out_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
            local_out_lif_id = global_out_lif_id;
        }

        mim_port->encap_id = global_out_lif_id;
    }

    if (_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info) ) { /* Only Local settings MAC Tunnel identification */
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {

            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

            /* Translate parameters to SOC_SAND parameters */
            rv = _bcm_petra_mac_to_sand_mac(mim_port->match_tunnel_srcmac, &src_bmac);
            BCMDNX_IF_ERR_EXIT(rv);

            /* add B-MACT entry, mapping <B-VID, B-SA> --> FEC. Only for LEARN. Forwarding is done in bcm_petra_l2_addr_add */
            SOC_PPD_BMACT_ENTRY_KEY_clear(&bmac_key);

            bmac_key.b_mac_addr = src_bmac;
            bmac_key.b_vid = _BCM_DPP_BVID_TO_VLAN(mim_port->match_tunnel_vlan);

            if (SOC_IS_ARAD(unit)) {
                bmac_key.flags = SOC_PPD_BMACT_ENTRY_TYPE_LEARN;
                bmac_key.local_port_ndx = soc_ppd_port;
            }

            soc_sand_rv = soc_ppd_frwrd_bmact_entry_get(soc_sand_dev_id, &bmac_key, &bmact_entry_info, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            bmact_entry_info.i_sid_domain = 0;
            bmact_entry_info.sa_learn_fec_id = fec_id;   

            soc_sand_rv = soc_ppd_frwrd_bmact_entry_add(soc_sand_dev_id, &bmac_key, &bmact_entry_info, &success);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            SOC_SAND_IF_FAIL_RETURN(success);
        }
    }


    /* add FEC entry that points at EEP */
    rv = _bcm_dpp_l2_gport_fill_fec(unit, mim_port, BCM_GPORT_MIM_PORT, fec_id, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    if (update) {

        /* Check that mim_port->encap_id is an the global lif of a MIM OutLIF (in SW DB) */
        global_out_lif_id = mim_port->encap_id;
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_out_lif_id, &local_out_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
        BCMDNX_IF_ERR_EXIT( _bcm_dpp_lif_usage_get(unit, -1, local_out_lif_id, NULL, &usage) );
        if (usage != _bcmDppLifTypeMim) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Can't replace OutLIF to MIM, because it's a different type (%d)"),usage));
        }

        BCMDNX_ALLOC(encap_entry_info, sizeof(SOC_PPD_EG_ENCAP_ENTRY_INFO) * SOC_PPD_NOF_EG_ENCAP_EEP_TYPES, "bcm_petra_mim_port_add.encap_entry_info");
        if (encap_entry_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }
        /* get existing EEP entry */
        soc_sand_rv = soc_ppd_eg_encap_entry_get(soc_sand_dev_id, SOC_PPD_EG_ENCAP_EEP_TYPE_LL, local_out_lif_id, 1, encap_entry_info, 
                                         next_eep, &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (nof_entries <= 0 || encap_entry_info[0].entry_type != SOC_PPD_EG_ENCAP_ENTRY_TYPE_LL_ENCAP) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id does not exist or is of the wrong type")));
        }
        ll_encap_info_ptr = &(encap_entry_info[0].entry_val.ll_info);
    }
    else {
        /* add EEP entry */
        SOC_PPD_EG_ENCAP_LL_INFO_clear(&ll_encap_info);
        ll_encap_info_ptr = &ll_encap_info;
    }

    rv = _bcm_petra_mac_to_sand_mac(mim_port->egress_tunnel_dstmac, &(ll_encap_info_ptr->dest_mac));
    BCMDNX_IF_ERR_EXIT(rv);

    if (_BCM_DPP_VLAN_IS_BVID(out_vlan)) { /* egress B-VID */
        /* remove 0x1111 from egress B-VID */
        out_vlan = _BCM_DPP_BVID_TO_VLAN(out_vlan);
    }
    ll_encap_info_ptr->out_vid = out_vlan;
    ll_encap_info_ptr->out_vid_valid = (mim_port->egress_tunnel_vlan == BCM_VLAN_INVALID ? FALSE : TRUE);

    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                 (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION,
                                  mim_port->flags & BCM_MIM_PORT_TYPE_PEER ? 0 : 1,
                                  &(ll_encap_info_ptr->outlif_profile)));
        BCMDNX_IF_ERR_EXIT(soc_sand_rv);
    }

    soc_sand_rv = soc_ppd_eg_encap_ll_entry_add(soc_sand_dev_id, local_out_lif_id, ll_encap_info_ptr);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    /* Add outlif to SW DB */
    rv = _bcm_dpp_out_lif_mim_match_add(unit,mim_port,local_out_lif_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (vpn == BCM_PETRA_MIM_VPN_INVALID) { /* P2P */

        /* I-SID is the identifier of the MIM port
           make sure there's no other port with the same I-SID */
        SOC_PPD_L2_LIF_ISID_KEY_clear(&isid_key);

        isid_key.isid_domain = 0;
        isid_key.isid_id = mim_port->egress_tunnel_service;

        soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &local_in_lif_id, &vsi_index, &isid_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (found && !update) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("egress_tunnel_service must be unique to this gport")));
        }

        if (!update) {
            if(mim_port->flags & BCM_MIM_PORT_SERVICE_ENCAP_WITH_ID) {
                lif_flags |= SHR_RES_ALLOC_WITH_ID;
                global_in_lif_id = BCM_GPORT_TUNNEL_ID_GET(mim_port->service_encap_id);
            }
            /* allocate LIF */
            if (SOC_IS_JERICHO(unit)) {

                /* Allocate the local and global lifs */
                sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));

                local_inlif_info.local_lif_flags = DPP_IS_FLAG_SET(mim_port->flags, BCM_MIM_PORT_SERVICE_ENCAP_INGERSS_WIDE) ? 
                                                                BCM_DPP_AM_IN_LIF_FLAG_WIDE : BCM_DPP_AM_IN_LIF_FLAG_COMMON;
                local_inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_isid;

                rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, lif_flags, &global_in_lif_id, &local_inlif_info, NULL, FALSE);
                BCM_IF_ERROR_RETURN(rv);

                /* Retrieve the allocated local in lif id */
                local_in_lif_id = local_inlif_info.base_lif_id;

            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_PETRAB(unit)) {
                rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_ISID, lif_flags, (SOC_PPD_LIF_ID*)&global_in_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
                local_in_lif_id = global_in_lif_id;
            }

            BCM_GPORT_TUNNEL_ID_SET(mim_port->service_encap_id, global_in_lif_id);

            /* add I-SID entry */
            SOC_PPD_L2_LIF_ISID_INFO_clear(&isid_info);
        } else {
            /* We already get the local inlif index from the HW, now get the global lif from the isid info. */
            global_in_lif_id = isid_info.global_lif;
        }

        isid_info.service_type = SOC_PPD_L2_LIF_ISID_SERVICE_TYPE_P2P;
        /* in PB, LIF destination cannot be drop so use trap-code */
        /* this forwarding is temporary and will be replaced when the port is cross-connected to another port */
        isid_info.default_frwrd.default_forwarding.type = SOC_PPD_FRWRD_DECISION_TYPE_TRAP;
        isid_info.default_frwrd.default_frwd_type = SOC_PPD_L2_LIF_DFLT_FRWRD_SRC_LIF;
        if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
            isid_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        }
        else {
            isid_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
        }
        isid_info.global_lif = global_in_lif_id;
        isid_info.is_extended = DPP_IS_FLAG_SET(mim_port->flags, BCM_MIM_PORT_SERVICE_ENCAP_INGERSS_WIDE);

#ifdef BCM_ARAD_SUPPORT
        /* set VSI as MAC_IN_MAC_VSI */
        if (SOC_IS_ARAD(unit)) {
            vsi_index = SOC_DPP_CONFIG(unit)->arad->pp_op_mode.p2p_info.mim_vsi;
        }
#endif /* BCM_ARAD_SUPPORT */
#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {  
            vsi_index = SOC_DPP_CONFIG(unit)->pb->pp_op_mode.p2p_info.mim_vsi;
        }
#endif /* BCM_PETRAB_SUPPORT */

        soc_sand_rv = soc_ppd_l2_lif_isid_add(soc_sand_dev_id, vsi_index, &isid_key, local_in_lif_id, &isid_info, &success);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(success);

        if (update) {
            /* get a ptr to previous db entry */
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit, &mim_port->mim_port_id, (shr_htb_data_t*)(void *)&phy_port, FALSE);        
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
            }
        }
        else {
            /* add db entry, mapping gport -> LIF + B-VID */
            BCMDNX_ALLOC(phy_port, sizeof(_BCM_GPORT_PHY_PORT_INFO), "vlan_gport");
            if (phy_port == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("allocation failed")));
            }
            sal_memset(phy_port, 0, sizeof(_BCM_GPORT_PHY_PORT_INFO));
        }

        /* update previous entry / fill new entry */
        phy_port->type = _bcmDppGportResolveTypeMimP2P;
        phy_port->phy_gport = global_in_lif_id; /* LIF */
        phy_port->encap_id = mim_port->match_tunnel_vlan; /* B-VID */

        if (!update) {
            /* add new db entry */
            rv = _bcm_dpp_sw_db_hash_vlan_insert(unit, &mim_port->mim_port_id, phy_port);        
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
            }
        }
#ifdef BCM_WARM_BOOT_SUPPORT
        else {
            rv = _bcm_dpp_wb_gport_update_port_state(unit, bcm_dpp_wb_gport_oper_update, &mim_port->mim_port_id, phy_port);
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Persistent Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
        }
#endif /* BCM_WARM_BOOT_SUPPORT */

        /* save B-SA, I-SID and other mim_port data as lif_info - for restoration */
        rv = _bcm_dpp_in_lif_mim_match_add(unit, mim_port, local_in_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else { /* MP */

        if (update) {
            /* get a ptr to previous db entry */
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit, &mim_port->mim_port_id, (shr_htb_data_t*)(void *)&phy_port, FALSE);        
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
            }
        }
        else {
            /* add db entry, mapping gport -> B-SA + B-VID */
            BCMDNX_ALLOC(phy_port, sizeof(_BCM_GPORT_PHY_PORT_INFO), "vlan_gport");
            if (phy_port == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("allocation failed")));
            }
            sal_memset(phy_port, 0, sizeof(_BCM_GPORT_PHY_PORT_INFO));
        }

        /* update previous entry / fill new entry */
        phy_port->type = _bcmDppGportResolveTypeMimMP;

        soc_sand_rv = soc_sand_pp_mac_address_struct_to_long(&bmac_key.b_mac_addr, mac_addr_arr);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        phy_port->phy_gport = mac_addr_arr[0];  /* B-SA LSB */
        phy_port->encap_id = mac_addr_arr[1] | (mim_port->match_tunnel_vlan << 16); /* B-SA MSB (16 bits) + B-VID (16 bits) */

        if (!update) {
            /* add new db entry */
            rv = _bcm_dpp_sw_db_hash_vlan_insert(unit, &mim_port->mim_port_id, phy_port);        
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
            }
        }
        else { /* update, remove & add */ 

            rv = _bcm_dpp_sw_db_hash_vlan_find(unit, &mim_port->mim_port_id, (shr_htb_data_t*)(void *)&remove_phy_port,TRUE);        
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) remove Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
            }

            rv = _bcm_dpp_sw_db_hash_vlan_insert(unit, &mim_port->mim_port_id, phy_port);        
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
            }
        }



#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_gport_update_port_state(unit, bcm_dpp_wb_gport_oper_update, &mim_port->mim_port_id, phy_port);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Persistent Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
        }
#endif /* BCM_WARM_BOOT_SUPPORT */
    }

exit:
    BCM_FREE(encap_entry_info);
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_mim_port_get(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_mim_port_t * mim_port)
{
    int rv, soc_sand_dev_id;
    uint32 soc_sand_rv, nof_entries;
    SOC_SAND_PP_MAC_ADDRESS src_bmac;
    SOC_PPD_FEC_ID fec_id;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32 next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    _BCM_GPORT_PHY_PORT_INFO *phy_port = NULL;
    uint32 mac_addr_arr[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
    SOC_PPD_LIF_ID local_inlif;
    int global_inlif, local_outlif;
    SOC_PPD_L2_LIF_ISID_KEY isid_key;
    SOC_PPD_SYS_VSI_ID vsi_index;
    SOC_PPD_L2_LIF_ISID_INFO isid_info;
    uint8 found;
    _bcm_lif_type_e lif_usage;
    uint8 is_wide_entry;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    MIM_INIT(unit);

    /* Translate parameters to SOC_SAND parameters */
    soc_sand_dev_id = (unit);

    /* get B-SA/LIF and B-VID from db (by port) */
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit, &mim_port->mim_port_id, (shr_htb_data_t*)(void *)&phy_port, FALSE);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Searching Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
    }
    if (phy_port->type != _bcmDppGportResolveTypeMimMP && phy_port->type != _bcmDppGportResolveTypeMimP2P) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport is not MIM")));
    }

    if (phy_port->type == _bcmDppGportResolveTypeMimP2P) { /* P2P */

        global_inlif = phy_port->phy_gport;  /* LIF */
        mim_port->match_tunnel_vlan = phy_port->encap_id >> 16; /* B-VID */

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, global_inlif, (int*)&local_inlif);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Get wide entry info */
        if (SOC_IS_JERICHO(unit)) {

            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_is_wide_entry,(unit, local_inlif, TRUE, &is_wide_entry));
            BCMDNX_IF_ERR_EXIT(rv);

            mim_port->flags |= (is_wide_entry) ? BCM_MIM_PORT_SERVICE_ENCAP_INGERSS_WIDE : 0;
        }


        /* get B-SA, I-SID and other mim_port data from lif_info */
        rv = _bcm_dpp_in_lif_mim_match_get(unit, mim_port, local_inlif);
        BCMDNX_IF_ERR_EXIT(rv);

        SOC_PPD_L2_LIF_ISID_KEY_clear(&isid_key);
        isid_key.isid_domain = 0;
        isid_key.isid_id = mim_port->egress_tunnel_service;

        soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &local_inlif, &vsi_index, &isid_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCM_GPORT_TUNNEL_ID_SET(mim_port->service_encap_id, global_inlif);
    }
    else {

        mac_addr_arr[0] = phy_port->phy_gport;  /* B-SA LSB */
        mac_addr_arr[1] = phy_port->encap_id & 0xffff; /* B-SA MSB (16 bits) */
        mim_port->match_tunnel_vlan = phy_port->encap_id >> 16; /* B-VID (16 bits) */

        soc_sand_rv = soc_sand_pp_mac_address_long_to_struct(mac_addr_arr, &src_bmac);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = _bcm_petra_mac_from_sand_mac(mim_port->match_tunnel_srcmac, &src_bmac);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* get FEC */
    fec_id = BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id);

    /* restore failover data and encap_id from FEC */
    rv = _bcm_dpp_l2_fec_fill_gport(unit, fec_id, mim_port, BCM_GPORT_MIM_PORT);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* Verify out LIF is MIM */
    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, mim_port->encap_id, &local_outlif);
    BCMDNX_IF_ERR_EXIT(rv);   

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_usage_get(soc_sand_dev_id, -1, local_outlif, NULL, &lif_usage));
    if (lif_usage != _bcmDppLifTypeMim) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OutLIF is not MIM")));
    }

    /* get EEP entry */
    soc_sand_rv = soc_ppd_eg_encap_entry_get(soc_sand_dev_id, SOC_PPD_EG_ENCAP_EEP_TYPE_LL, local_outlif, 1, encap_entry_info, 
                                     next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (encap_entry_info[0].entry_type != SOC_PPD_EG_ENCAP_ENTRY_TYPE_LL_ENCAP) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("invalid outlif setting. expect LL entry")));
    }
    rv = _bcm_petra_mac_from_sand_mac(mim_port->egress_tunnel_dstmac, &encap_entry_info[0].entry_val.ll_info.dest_mac);
    BCMDNX_IF_ERR_EXIT(rv);

    if (encap_entry_info[0].entry_val.ll_info.out_vid_valid) {
        mim_port->egress_tunnel_vlan = encap_entry_info[0].entry_val.ll_info.out_vid;
    }
    else {
        mim_port->egress_tunnel_vlan = BCM_VLAN_INVALID;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_mim_port_delete(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_gport_t mim_port_id)
{
    int rv;
    uint32 soc_sand_rv;
    SOC_SAND_PP_MAC_ADDRESS src_bmac;
    SOC_PPD_FEC_ID fec_id /* work */, protect_fec_id;
    uint32 fec_flags = 0;
    SOC_PPD_BMACT_ENTRY_KEY bmac_key;
    _BCM_GPORT_PHY_PORT_INFO *phy_port;
    SOC_PPD_LIF_ID tmp_local_lif;
    bcm_mim_port_t mim_port;        
    uint32  soc_ppd_port;
    int local_port, core;
    SOC_PPD_L2_LIF_ISID_KEY isid_key;
    SOC_PPD_LIF_ID isid_local_lif_id;
    int isid_global_lif_id, global_outlif_id, local_outlif_id;
    _bcm_dpp_gport_info_t gport_info;
    _bcm_lif_type_e lif_usage;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);

    MIM_INIT(unit);

    bcm_mim_port_t_init(&mim_port);

    mim_port.mim_port_id = mim_port_id;

    rv = bcm_petra_mim_port_get(unit, vpn, &mim_port);
    BCMDNX_IF_ERR_EXIT(rv);

    global_outlif_id = mim_port.encap_id;

    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_outlif_id, &local_outlif_id);
    BCMDNX_IF_ERR_EXIT(rv);
    

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_usage_get(unit, -1, local_outlif_id, NULL, &lif_usage));
    if (lif_usage != _bcmDppLifTypeMim) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OutLIF is not a MIM")));
    }

    rv = _bcm_petra_mac_to_sand_mac(mim_port.match_tunnel_srcmac, &src_bmac);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get FEC */
    fec_id = BCM_GPORT_MIM_PORT_ID_GET(mim_port.mim_port_id);
    if (fec_id == -1) {
        rv = BCM_E_PARAM;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) GPORT (0x%x) not of type MIM"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port.mim_port_id)));
    }

    /* remove FEC entry */
    soc_sand_rv = soc_ppd_frwrd_fec_remove(unit, fec_id);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* remove EEP entry */
    soc_sand_rv = soc_ppd_eg_encap_entry_remove(unit, SOC_PPD_EG_ENCAP_EEP_TYPE_LL, local_outlif_id);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* get db entry to check if P2P port */
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit, &(mim_port.mim_port_id), (shr_htb_data_t*)(void *)&phy_port, FALSE);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Searching Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port.mim_port_id)));
    }

    if (phy_port->type == _bcmDppGportResolveTypeMimP2P) { /* P2P */

        isid_global_lif_id = phy_port->phy_gport;  /* LIF */

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, isid_global_lif_id, (int*)&tmp_local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_port_tpid_delete_all(unit, mim_port.service_encap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        SOC_PPD_L2_LIF_ISID_KEY_clear(&isid_key);
        isid_key.isid_domain = 0; /* isid_domain is always 0 */
        isid_key.isid_id = mim_port.egress_tunnel_service;

        /* delete lif from HW */
        soc_sand_rv = soc_ppd_l2_lif_isid_remove(unit, &isid_key, &isid_local_lif_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (isid_local_lif_id != tmp_local_lif) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("isid lif id doesn't match lif index")));
        }

        /* de-allocate LIF */
        if (SOC_IS_JERICHO(unit)) {
            rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, isid_global_lif_id, isid_local_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID, FALSE);
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_PETRAB(unit)) {
            rv = bcm_dpp_am_l2_ac_dealloc(unit, 0, isid_local_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* remove mim lif match SW */
        rv = _bcm_dpp_in_lif_match_delete(unit, isid_local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);

    }
    else { /* MP */

        rv = _bcm_dpp_gport_to_phy_port(unit, mim_port.port, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        if (_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)) { /* Only Local settings MAC Tunnel identification */
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

                /* remove B-MACT entry */
                SOC_PPD_BMACT_ENTRY_KEY_clear(&bmac_key);

                bmac_key.b_mac_addr = src_bmac;
                bmac_key.b_vid = _BCM_DPP_BVID_TO_VLAN(mim_port.match_tunnel_vlan);

                if (SOC_IS_ARAD(unit)) {
                    bmac_key.flags |= SOC_PPD_BMACT_ENTRY_TYPE_LEARN;
                    bmac_key.local_port_ndx = soc_ppd_port;
                }

                soc_sand_rv = soc_ppd_frwrd_bmact_entry_remove(unit, &bmac_key);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }

    /* remove db entry */
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit, &(mim_port.mim_port_id), (shr_htb_data_t*)(void *)&phy_port, TRUE);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Searching Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port.mim_port_id)));
    }

    /* de-allocate FEC */    
    fec_id = BCM_GPORT_MIM_PORT_ID_GET(mim_port.mim_port_id);

    if (_BCM_PPD_IS_VALID_FAILOVER_ID(mim_port.failover_id)) {
        /* if failover is valid then second member in protection already allocated, get info */
        if (mim_port.failover_gport_id != BCM_GPORT_TYPE_NONE) {
            protect_fec_id =  BCM_GPORT_MIM_PORT_ID_GET(mim_port.failover_gport_id);
            fec_id = _BCM_PPD_GPORT_PROTECT_TO_WORK_FEC(protect_fec_id);
        }
        else
        {
            /* de-allocate 2 fec entries */
            rv = bcm_dpp_am_fec_dealloc(unit, fec_flags, 0, 2, fec_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else {
        /* de-allocate 1 fec entry */
        rv = bcm_dpp_am_fec_dealloc(unit, fec_flags, 0, 1, fec_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* de-allocate EEP */
    if (SOC_IS_JERICHO(unit)) {
        rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_outlif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_outlif_id, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_PETRAB(unit)) {
        rv = bcm_dpp_am_l3_eep_dealloc(unit, global_outlif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Remove entry from SW DB */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_out_lif_match_delete(unit,local_outlif_id));

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


