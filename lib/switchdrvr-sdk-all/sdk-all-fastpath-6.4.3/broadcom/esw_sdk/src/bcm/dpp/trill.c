/*
 * $Id: trill.c,v 1.89 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_BCM_TRILL

#include <shared/bsl.h>

#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/sw_db.h>

#include <bcm_int/common/multicast.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/control.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/trill_pb.h>
#include <bcm_int/dpp/switch.h>

#include <bcm_int/dpp/l3.h>
/*
#include <bcm_int/dpp/mpls.h> 
*/ 
#include <bcm_int/common/field.h>
#include <bcm_int/dpp/wb_db_gport.h>
#include <bcm_int/dpp/wb_db_trill.h>
#include <bcm_int/dpp/state.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>
#include <soc/dpp/PPD/ppd_api_llp_sa_auth.h>
#include <soc/dpp/PPD/ppd_api_llp_vid_assign.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <shared/shr_resmgr.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_bmact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_llp_sa_auth.h>
#ifdef  BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#endif /* BCM_ARAD_SUPPORT */
#include <soc/dpp/PPD/ppd_api_eg_ac.h>


/*#define BCM_TRILL_MULTIPATH 0*/


       
bcm_dpp_trill_state_t *trill_state[SOC_MAX_NUM_DEVICES];

static int _bcm_dpp_trill_multicast_adjacency_add( int unit,
                                            bcm_trill_multicast_adjacency_t *trill_multicast_adjacency);

static int _bcm_dpp_trill_multicast_adjacency_delete( int unit,
                                               bcm_trill_multicast_adjacency_t *trill_multicast_adjacency);

int
_bcm_dpp_trill_port_is_ecmp(
    int unit,
    bcm_trill_port_t    *trill_port,
    int                 *is_ecmp,
    SOC_PPD_FEC_TYPE    *ecmp_fec_type)
{
    SOC_PPD_FEC_ID                      fec_index = -1;
    SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO    fec_entry_use_info;
    int                                 fec_ecmp_reserverd;
    int                                 ecmp_max_size, ecmp_curr_size;
    int                                 rv = BCM_E_NONE;    
    unsigned int                        soc_sand_dev_id;
    uint32                              soc_sand_rv  = SOC_SAND_OK;
    bcm_if_t                            intf_array[10];
    int                                 intf_count;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE      protect_type;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO        working_fec;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO        protect_fec;
    SOC_PPD_FRWRD_FEC_PROTECT_INFO      protect_info;

    BCMDNX_INIT_FUNC_DEFS; 
    *is_ecmp = 0;


    if (trill_port->egress_if == 0) 
        BCM_EXIT;
    soc_sand_dev_id = (unit);

    rv = _bcm_l3_intf_to_fec(unit, trill_port->egress_if, &fec_index );
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_PETRAB(unit)) {
         /* LCOV_EXCL_START */
        soc_sand_rv = soc_ppd_frwrd_fec_entry_use_info_get(soc_sand_dev_id, fec_index, &fec_entry_use_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *is_ecmp = (fec_entry_use_info.type == SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ECMP);
         /* LCOV_EXCL_STOP */
    } else {
        fec_ecmp_reserverd = SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved - 1;

        if (fec_index < fec_ecmp_reserverd ) {
            rv = bcm_sw_db_l3_get_ecmp_sizes(unit, fec_index, &ecmp_max_size, &ecmp_curr_size);;
            BCMDNX_IF_ERR_EXIT(rv);

            *is_ecmp = (ecmp_curr_size != 0);

        }
    }

    if (*is_ecmp) {
        
        if (SOC_IS_PETRAB(unit) || (soc_property_get(unit, "custom_feature_ecmp_api_multipath", 0) == 1)) {

          rv = bcm_l3_egress_multipath_get(unit, fec_index, 10,
                                 intf_array, &intf_count);
          BCMDNX_IF_ERR_EXIT(rv);

        } else {
          bcm_l3_egress_ecmp_t ecmp;

          bcm_l3_egress_ecmp_t_init(&ecmp);
          ecmp.ecmp_intf = fec_index;

          rv = bcm_l3_egress_ecmp_get(unit, &ecmp, 10,
                                 intf_array, &intf_count);
          BCMDNX_IF_ERR_EXIT(rv);

          intf_array[0] = _BCM_PETRA_L3_ITF_VAL_GET(intf_array[0]);
        }

        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, intf_array[0],
                                    &protect_type, &working_fec,
                                    &protect_fec, &protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *ecmp_fec_type = working_fec.type;

    }
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
   BCMDNX_FUNC_RETURN;
}

/* extract nickname from gport database
   trill port id must be multicast */
int
_bcm_dpp_dt_nickname_trill_get(
    int unit,
    bcm_gport_t trill_port_id, 
    int         *dt_nickname)
{
    int  rv = BCM_E_NONE;
    _BCM_GPORT_PHY_PORT_INFO    *phy_port_ptr;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       &trill_port_id,
                                       (shr_htb_data_t*)(void *)&phy_port_ptr,
                                       FALSE);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(rv,  (_BSL_BCM_MSG("Find trill port ")));
    }

    if (phy_port_ptr->type != _BCM_DPP_GPORT_IN_TYPE_TRILL_MC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,  (_BSL_BCM_MSG("Trill port not multicast")));
    }

    *dt_nickname = phy_port_ptr->encap_id;

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;

}

int
_bcm_dpp_mc_to_trill_get(
    int unit,
    bcm_multicast_t group, 
    bcm_gport_t     **port)
{
    int  rv = BCM_E_NONE;
    int  mc_id;

    BCMDNX_INIT_FUNC_DEFS;
    mc_id = _BCM_MULTICAST_ID_GET(group);
    rv = _bcm_dpp_sw_db_hash_mc_trill_find(unit,
                                        &(mc_id),
                                        (shr_htb_data_t*)(void *)port,
                                        FALSE);        
    if (rv != BCM_E_NOT_FOUND) {
         BCMDNX_IF_ERR_EXIT(rv); 
    }
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_add_to_trill_port_list(
     int unit,
     bcm_gport_t gport) 
{
    bcm_gport_t *tmp_ports;
    int new_cnt ;
    _bcm_petra_trill_port_list_t *trill_ports;
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv = BCM_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */

    BCMDNX_INIT_FUNC_DEFS;
    trill_ports = SOC_DPP_STATE(unit)->trill_state->trill_ports;

    if (trill_ports->port_cnt == trill_ports->allocated_cnt ) {
        new_cnt = trill_ports->allocated_cnt + _BCM_TRILL_PORTS_ALLOC_SIZE;
        tmp_ports = NULL;
        BCMDNX_ALLOC(tmp_ports, sizeof(bcm_gport_t) * new_cnt, "trill_port_list");
        if (tmp_ports == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        sal_memcpy(tmp_ports, trill_ports->ports, trill_ports->port_cnt * sizeof(bcm_gport_t) );
        BCM_FREE(trill_ports->ports);
        trill_ports->ports = tmp_ports;
        trill_ports->allocated_cnt = new_cnt;
    }
    trill_ports->ports[trill_ports->port_cnt] = gport;
    trill_ports->port_cnt++;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        rv = _bcm_dpp_wb_trill_update_ports_state(unit, SOC_DPP_STATE(unit)->trill_state->trill_ports);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
exit:  
    BCMDNX_FUNC_RETURN;

}
int
_bcm_petra_remove_from_trill_port_list(
     int unit,
     bcm_gport_t gport) 
{
    
    int i,j ;
    _bcm_petra_trill_port_list_t *trill_ports;
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv = BCM_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */

    BCMDNX_INIT_FUNC_DEFS;

    trill_ports = SOC_DPP_STATE(unit)->trill_state->trill_ports;

    for (i = 0; i < trill_ports->port_cnt; i++ ) {
        if (trill_ports->ports[i] == gport) {
            for (j=i+1 ; j<trill_ports->port_cnt  ; ++j) { /*used since memmove is unavailable and sal_memcpy is not to be used when the memory overlaps*/
                trill_ports->ports[j-1] = trill_ports->ports[j];
            }
            /*sal_memcpy(&(trill_ports->ports[i]), &(trill_ports->ports[i+1]), This was the original part. commented out and replace by above loop by sinai.
                       (trill_ports->port_cnt - i -1) * sizeof(bcm_gport_t) );*/
            trill_ports->port_cnt--;
            break;
        }
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_trill_update_ports_state(unit, SOC_DPP_STATE(unit)->trill_state->trill_ports);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}
int bcm_petra_trill_init_data(
                              int unit)
{
    int                                     rv = BCM_E_NONE;    
    int                                     alloc_size;
    _bcm_petra_trill_port_list_t            *trill_ports;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_DPP_CONFIG(unit)->trill.mode) {
        rv = BCM_E_DISABLED;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill is not enabled (soc property)\n"), bcm_errmsg(rv)));
    }

    if (trill_state[unit] == NULL) {
        BCMDNX_ALLOC(trill_state[unit], sizeof(bcm_dpp_trill_state_t), "TRILL Info");
        if (trill_state[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate TRILL state")));
        }
        sal_memset(trill_state[unit], 0, sizeof(bcm_dpp_trill_state_t));
        SOC_DPP_STATE(unit)->trill_state = trill_state[unit];

        SOC_DPP_STATE(unit)->trill_state->mask_set = FALSE;
        SOC_DPP_STATE(unit)->trill_state->trill_out_ac = 0;
        SOC_DPP_STATE(unit)->trill_state->last_used_id = -1;


        if (SOC_DPP_STATE(unit)->trill_state->trill_ports == NULL) {
            alloc_size = sizeof(_bcm_petra_trill_port_list_t);
            BCMDNX_ALLOC(SOC_DPP_STATE(unit)->trill_state->trill_ports,
                      alloc_size, "trill ports data");
            if (SOC_DPP_STATE(unit)->trill_state->trill_ports == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate trill ports")));
            }
            sal_memset(SOC_DPP_STATE(unit)->trill_state->trill_ports, 0, alloc_size);
        }
    }


    trill_ports = SOC_DPP_STATE(unit)->trill_state->trill_ports;
    trill_ports->port_cnt = 0;
    trill_ports->ports = NULL;
    trill_ports->allocated_cnt = _BCM_TRILL_PORTS_ALLOC_SIZE;
    BCMDNX_ALLOC(trill_ports->ports, sizeof(bcm_gport_t) * _BCM_TRILL_PORTS_ALLOC_SIZE, "trill_port_list");
    if (trill_ports->ports == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    /* init all to none */
    sal_memset(trill_ports->ports,_SHR_GPORT_NONE, sizeof(bcm_gport_t) * _BCM_TRILL_PORTS_ALLOC_SIZE);

    /* Initialization has not been completed by default */
    SOC_DPP_STATE(unit)->trill_state->init = FALSE;

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_trill_state_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
exit:
    BCMDNX_FUNC_RETURN;
}
int bcm_petra_trill_deinit_data(
                                int unit)
{
    int                                     rv = BCM_E_NONE;    
    _bcm_petra_trill_port_list_t            *trill_ports;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_DPP_CONFIG(unit)->trill.mode) {
        rv = BCM_E_DISABLED;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill is not enabled (soc property)\n"), bcm_errmsg(rv)));
    }

    if (trill_state[unit] != NULL) {

        trill_ports = SOC_DPP_STATE(unit)->trill_state->trill_ports;
        if (trill_ports) {
            if (trill_ports->ports) {
                BCM_FREE(trill_ports->ports);
            }
            BCM_FREE(trill_ports);
        }
        BCM_FREE(trill_state[unit]);
    }

    /* init all to none */

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_trill_state_deinit(unit);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_init(
    int unit)
{
    unsigned int                            soc_sand_dev_id;
    uint32                                  soc_sand_rv  = SOC_SAND_OK;
    SOC_PPD_EG_ENCAP_RANGE_INFO             encap_range_info;
    int                                     rv = BCM_E_NONE;
    int                                     tpid_idx = 0;
    bcm_dpp_vlan_egress_edit_profile_info_t mapping_profile;
    SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY        command_key;
    SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO       command_info;
    int                                     old_profile, new_profile=0, is_last, is_allocated;
    SOC_PPD_EG_AC_INFO                      ac_info;
    SOC_PPD_AC_ID                           trill_out_ac = 0;
    SOC_SAND_SUCCESS_FAILURE                success;
#ifdef  BCM_ARAD_SUPPORT
    uint8                                   trill_is_init;
    int                                     lif_id;
    SOC_PPD_LIF_ID                          lif_index = 0;
    bcm_port_t                              port;
    uint32                                  trill_disable_designated_vlan_check;
#endif

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

#ifdef  BCM_ARAD_SUPPORT
    rv = arad_pp_sw_db_trill_enable_get(soc_sand_dev_id, &trill_is_init);
    BCMDNX_IF_ERR_EXIT(rv);

    if (trill_is_init) {
        if (!SOC_WARM_BOOT(unit))
        {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: Trill already initialized.\n")));
        }
     }
     else {
        if (SOC_WARM_BOOT(unit)) {
                BCM_EXIT;
        }
     }
#endif /*BCM_ARAD_SUPPORT*/

    if (!SOC_DPP_CONFIG(unit)->trill.mode) {
        rv = BCM_E_DISABLED;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill is not enabled (soc property)\n"), bcm_errmsg(rv)));
    }

    if (!SOC_DPP_STATE(unit)->trill_state) {
        rv = BCM_E_MEMORY;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill state should be allocated during bcm init but is not\n"), bcm_errmsg(rv)));
    }

    if (!SOC_IS_ARAD(unit)) {
        /* LCOV_EXCL_START */
        /* alloc a special out-ac, used only for Trill */
        if (SOC_IS_PETRAB(unit)) {
            rv = bcm_dpp_am_l2_ac_alloc(unit, 0, 0, &trill_out_ac);
        } else {
            rv = bcm_dpp_am_out_ac_alloc(unit, _BCM_DPP_AM_OUT_AC_TYPE_DEFAULT, 0, &trill_out_ac);
        }    
        BCMDNX_IF_ERR_EXIT(rv);
        
        /* get a profile to config vlan_edit_command */
        mapping_profile.evec[SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE].tags_to_remove = 0;
        mapping_profile.evec[SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE].outer_tag.tpid_index = tpid_idx;
        mapping_profile.evec[SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE].outer_tag.vid_source = SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_ENCAP_INFO;
        
        rv = _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_exchange(unit, trill_out_ac, 
                                                                        &mapping_profile, &old_profile, &is_last, 
                                                                        &new_profile, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);
        
        /* set vlan_edit_command for trill */
        SOC_PPD_EG_VLAN_EDIT_COMMAND_KEY_clear(&command_key);
        
        command_key.edit_profile = new_profile;
        command_key.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        
        soc_sand_rv = soc_ppd_eg_vlan_edit_command_info_get(soc_sand_dev_id, &command_key, &command_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        command_info.tags_to_remove = 0;
        command_info.outer_tag.vid_source = SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_ENCAP_INFO;
        command_info.outer_tag.tpid_index = tpid_idx; 
        command_info.outer_tag.pcp_dei_source = SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP;
        command_info.inner_tag.vid_source = SOC_PPD_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY;
        command_info.inner_tag.pcp_dei_source = SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;
        
        soc_sand_rv = soc_ppd_eg_vlan_edit_command_info_set(soc_sand_dev_id, &command_key, &command_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        /* write trill_out_ac to default_sem_index reg */
        if (SOC_IS_PETRAB(unit)) {
            soc_sand_rv = soc_pb_pp_frwrd_bmact_default_sem_index_set(soc_sand_dev_id, 
                                                                      trill_out_ac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        
        if (new_profile != 0) {
            soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, 
                                                 trill_out_ac, &ac_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            ac_info.edit_info.edit_profile = new_profile;
            
            soc_sand_rv = soc_ppd_eg_ac_info_set(soc_sand_dev_id, 
                                                 trill_out_ac, &ac_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        /* encap for egress trill is 0x3fff - the limit is set accordingly */
        SOC_PPD_EG_ENCAP_RANGE_INFO_clear(&encap_range_info);
        encap_range_info.ll_limit = 0x3fff;
        encap_range_info.ip_tnl_limit = 0x3fff;
        
        soc_sand_rv = soc_ppd_eg_encap_range_info_set(soc_sand_dev_id,&encap_range_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
        /* LCOV_EXCL_STOP */
    } else {
        if (!SOC_WARM_BOOT(unit)) {
            soc_sand_rv = soc_ppd_frwrd_trill_native_inner_tpid_add(soc_sand_dev_id, _BCM_PETRA_TRILL_NATIVE_TPID, &success);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            SOC_SAND_IF_FAIL_RETURN(success);
        }
    }
#ifdef BCM_ARAD_SUPPORT
    SOC_DPP_STATE(unit)->trill_state->trill_out_ac = trill_out_ac;

    trill_disable_designated_vlan_check = soc_property_get(unit, spn_TRILL_DESIGNATED_VLAN_CHECK_DISABLE, 0);
    if (!trill_disable_designated_vlan_check) {
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            lif_id = soc_property_port_suffix_num_get(unit, port, -1, spn_CUSTOM_FEATURE, "trill_designated_vlan_inlif", -1);
            if (lif_id != -1) {
                lif_index = lif_id;
                rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_VLAN_VSI, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &lif_index);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    rv = arad_pp_sw_db_trill_enable_set(unit, 1);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_ARAD_SUPPORT*/

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_port_add(
    int unit,
    bcm_trill_port_t * trill_port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_trill_port_add(unit, trill_port, FALSE));

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}
int bcm_petra_trill_port_delete(int unit, bcm_gport_t trill_port_id) {
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_trill_port_delete(unit, trill_port_id, TRUE));
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}
int bcm_petra_trill_port_delete_all(int unit){
    BCMDNX_INIT_FUNC_DEFS;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_port_get(int unit,bcm_trill_port_t *trill_port){
    int                             rv = BCM_E_NONE;
    _BCM_GPORT_PHY_PORT_INFO        *phy_port_ptr;
    _bcm_petra_trill_info_t         *trill_info_ptr;
    unsigned int                    soc_sand_dev_id;
    uint32                          soc_sand_rv  = SOC_SAND_OK;
    SOC_PPD_FRWRD_TRILL_GLOBAL_INFO tril_global_info;
    SOC_PPD_MYMAC_TRILL_INFO        mymac_trill_info;
    int                             virtual_nick_name_index;
    bcm_gport_t                     port_id=trill_port->trill_port_id ;  /* GPORT identifier. */
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    bcm_trill_port_t_init(trill_port);

    /* return virtual nickname and flag if gport is a trill virtual nickname gport */
    if (BCM_GPORT_TRILL_PORT_ID_IS_VIRTUAL(port_id)) {
        /* check if the index is allocated in sw */
        virtual_nick_name_index = _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_GET(port_id);
        rv =  bcm_dpp_am_trill_virtual_nick_name_is_alloc(unit,0,virtual_nick_name_index);
        if (rv == BCM_E_NOT_FOUND) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("virtual nickname isn't allocated. gport: %d\n"), port_id));
        }
        trill_port->flags |= BCM_TRILL_PORT_VIRTUAL;
        trill_port->name = _BCM_DPP_GPORT_TRILL_NICKNAME_GET(port_id);
        BCM_EXIT;
    }

    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       &port_id,
                                       (shr_htb_data_t*)(void *)&phy_port_ptr,
                                       FALSE);        
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
    }

    rv = _bcm_dpp_sw_db_hash_trill_info_find(unit,
                                       &port_id,
                                       (shr_htb_data_t*)(void *)&trill_info_ptr,
                                       FALSE);        
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
    }

    if (phy_port_ptr->type == _BCM_DPP_GPORT_IN_TYPE_TRILL_MC) {
        trill_port->flags = BCM_TRILL_PORT_MULTICAST;
    }
    /* 
    else if (trill_info_ptr->type == _BCM_PETRA_TRILL_PORT_MULTIPATH) {
        trill_port->flags = BCM_TRILL_MULTIPATH;
    }*/
    
    if (trill_info_ptr->fec_id != -1 ) {
        _bcm_l3_fec_to_intf(unit, trill_info_ptr->fec_id, &trill_port->egress_if); 
    }
    trill_port->trill_port_id = port_id; 
    trill_port->name = phy_port_ptr->encap_id; 

    soc_sand_rv = soc_ppd_frwrd_trill_global_info_get(soc_sand_dev_id, &tril_global_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    trill_port->hopcount = tril_global_info.cfg_ttl; 

    soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &mymac_trill_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    if (phy_port_ptr->encap_id ==  mymac_trill_info.my_nick_name) {
        trill_port->flags |= BCM_TRILL_PORT_LOCAL;
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_port_get_all(
    int unit, 
    int port_max, 
    bcm_trill_port_t *port_array, 
    int *port_count)
{
    int i;
    int     rv;
    _bcm_petra_trill_port_list_t *trill_ports;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    trill_ports = SOC_DPP_STATE(unit)->trill_state->trill_ports;

   *port_count =( trill_ports->port_cnt <=  port_max) ? trill_ports->port_cnt:port_max;

    for (i = 0; i < *port_count; i++) {
        bcm_trill_port_t_init(&(port_array[i]));
        port_array[i].trill_port_id = trill_ports->ports[i];

        rv = bcm_petra_trill_port_get(unit, &(port_array[i]));
        BCMDNX_IF_ERR_EXIT(rv);

    }    

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_cleanup(int unit) {
    int status;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    if (SOC_IS_PETRAB(unit)) {
        status = _bcm_dpp_sw_db_hash_trill_route_info_destroy(unit); 
        if( BCM_FAILURE(status)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("error(%s) freeing MC trill route DB failed\n"),
                                   bcm_errmsg(status)));
        }
    }
    /* old implementation that use sw db for mapping mc-id to nickname*/
    if (SOC_DPP_CONFIG(unit)->trill.mc_id)
    {
        status = _bcm_dpp_sw_db_hash_mc_trill_destroy(unit);
        if( BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_TRILL,
                      (BSL_META_U(unit,
                                  "error(%s) freeing MC trill DB failed\n"),
                       bcm_errmsg(status)));
        }
    }
    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_trill_rbridge_entry_add
 * Purpose:
 *      Create unicast Rbridge with transit functionality
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN)  Trill port configuration
 * Returns:
 *      BCM_E_XXX
 */
int bcm_petra_trill_rbridge_entry_add(
    int unit,
    bcm_trill_rbridge_t * trill_port)
{
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv = SOC_SAND_OK;
    SOC_SAND_SUCCESS_FAILURE success;
    SOC_PPD_LIF_ID lif_id = 0;
    SOC_PPD_FEC_ID fec_index = -1;
    SOC_PPC_FRWRD_DECISION_INFO fwd_decision; 

    
    BCMDNX_INIT_FUNC_DEFS;   
    soc_sand_dev_id = (unit);

    SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision); 

    if (trill_port->egress_if != 0) {
        rv = _bcm_l3_intf_to_fec(unit, trill_port->egress_if, &fec_index );
        BCMDNX_IF_ERR_EXIT(rv);
    } 

    /* learn data: built from forward decision. (dest_id=fec_id) */
    SOC_PPD_FRWRD_DECISION_FEC_SET(
       unit, &fwd_decision, fec_index, soc_sand_rv); 

    soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_add(soc_sand_dev_id, lif_id, trill_port->name, &fwd_decision, &success);  
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success);
    BCM_EXIT;
    
exit:    
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_trill_rbridge_entry_delete
 * Purpose:
 *      Delete unicast Rbridge with transit functionality
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN)  Trill port configuration
 * Returns:
 *      BCM_E_XXX
 */
int bcm_petra_trill_rbridge_entry_delete(
    int unit,
    bcm_trill_rbridge_t * trill_port)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv = SOC_SAND_OK;
    SOC_PPD_LIF_ID lif_id;
    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_remove(soc_sand_dev_id, trill_port->name, &lif_id);  
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    BCM_EXIT;
    
exit:    
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_trill_rbridge_entry_get
 * Purpose:
 *      Get unicast Rbridge with transit functionality
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT)  Trill port configuration
 * Returns:
 *      BCM_E_XXX
 */
int bcm_petra_trill_rbridge_entry_get(
    int unit,
    bcm_trill_rbridge_t * trill_port)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv = SOC_SAND_OK;
    SOC_PPD_LIF_ID lif_id;
    SOC_PPD_FEC_ID fec_index = -1;
    uint8 is_found;
    SOC_PPC_FRWRD_DECISION_INFO fwd_decision; 

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision); 

    soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_get(soc_sand_dev_id, trill_port->name, &lif_id, &fwd_decision, &is_found);  
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (is_found) {
        fec_index = fwd_decision.dest_id;
        _bcm_l3_fec_to_intf(unit, fec_index, &(trill_port->egress_if));
    }        
    BCM_EXIT;
    
exit:    
    BCMDNX_FUNC_RETURN;
}
int 
_ppd_trill_multicast_route_add(
    int unit, 
    uint32              flags,
    bcm_trill_name_t    root_name, 
    bcm_vlan_t          vlan, 
    bcm_vlan_t          inner_vlan, 
    bcm_multicast_t     group,
    bcm_trill_name_t    src_name, 
    bcm_gport_t         src_port) {

     unsigned int                           soc_sand_dev_id;
     SOC_PPD_TRILL_MC_ROUTE_KEY                 trill_mc_key;

     SOC_SAND_SUCCESS_FAILURE                success;
     
     int                                    rv = BCM_E_NONE;
     uint32                                 mc_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = (unit);

    if ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) &&
        (SOC_DPP_CONFIG(unit)->trill.transparent_service == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,  (_BSL_BCM_MSG("Trill transparent service is invalid when soc property is not enabled")));
    }
   
    SOC_PPD_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);
    trill_mc_key.tree_nick = root_name;
    trill_mc_key.fid = vlan;
    trill_mc_key.ing_nick = src_name;
    trill_mc_key.adjacent_eep = 0;
    trill_mc_key.esadi = ((flags & BCM_TRILL_MULTICAST_ESADI) ? 0x1 : 0x0);
    trill_mc_key.tts = ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) ? 0x1 : 0x0);
    if (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL)  {
        trill_mc_key.outer_vid = vlan; 
        if (inner_vlan == BCM_VLAN_INVALID) {
            trill_mc_key.inner_vid = 0;
        } else {
            trill_mc_key.inner_vid = inner_vlan;
        }
    }
    mc_id = _BCM_MULTICAST_ID_GET(group);

    rv = soc_ppd_frwrd_trill_multicast_route_add(soc_sand_dev_id, &trill_mc_key, mc_id, &success);
    BCM_SAND_IF_ERR_EXIT(rv);
    SOC_SAND_IF_FAIL_RETURN(success);
   
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
int 
_ppd_trill_multicast_route_remove(
    int unit, 
    uint32              flags,
    bcm_trill_name_t    root_name,
    bcm_vlan_t          vlan, 
    bcm_vlan_t          inner_vlan,
    bcm_multicast_t     group,
    bcm_trill_name_t    src_name) {

     unsigned int                           soc_sand_dev_id;
     SOC_PPD_TRILL_MC_ROUTE_KEY                 trill_mc_key;
     
     int                                    rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = (unit);

    if ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) &&
        (SOC_DPP_CONFIG(unit)->trill.transparent_service == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,  (_BSL_BCM_MSG("Trill transparent service is invalid when soc property is not enabled")));
    }  
	 
    SOC_PPD_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);
    trill_mc_key.tree_nick = root_name;
    trill_mc_key.fid = vlan;
    trill_mc_key.ing_nick = src_name;
    trill_mc_key.adjacent_eep = 0;
    trill_mc_key.esadi = ((flags & BCM_TRILL_MULTICAST_ESADI) ? 0x1 : 0x0);
    trill_mc_key.tts = ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) ? 0x1 : 0x0);
    if (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL)  {
        trill_mc_key.outer_vid = vlan; 
        if (inner_vlan == BCM_VLAN_INVALID) {
            trill_mc_key.inner_vid = 0;
        } else {
            trill_mc_key.inner_vid = inner_vlan;
        }
    }

    rv = soc_ppd_frwrd_trill_multicast_route_remove(soc_sand_dev_id, &trill_mc_key);
    BCM_SAND_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_ppd_trill_multicast_route_get(
    int unit, 
    uint32              flags,
    bcm_trill_name_t    root_name, 
    bcm_vlan_t          vlan, 
    bcm_vlan_t          inner_vlan,     
    bcm_trill_name_t    src_name, 
    bcm_gport_t         src_port,
    bcm_multicast_t     *group,
    uint8               *found) {

    unsigned int                           soc_sand_dev_id;
    SOC_PPD_TRILL_MC_ROUTE_KEY             trill_mc_key;     
    int                                    rv = BCM_E_NONE;
    uint32                                 mc_id;
     

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = unit;

    if ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) &&
        (SOC_DPP_CONFIG(unit)->trill.transparent_service == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,  (_BSL_BCM_MSG("Trill transparent service is invalid when soc property is not enabled")));
    } 
   
    SOC_PPD_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);
    trill_mc_key.tree_nick = root_name;
    trill_mc_key.fid = vlan;
    trill_mc_key.ing_nick = src_name;
    trill_mc_key.adjacent_eep = 0;
    trill_mc_key.esadi = ((flags & BCM_TRILL_MULTICAST_ESADI) ? 0x1 : 0x0);
    trill_mc_key.tts = ((flags & BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE) ? 0x1 : 0x0);
    if (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL)  {
        trill_mc_key.outer_vid = vlan; 
        if (inner_vlan == BCM_VLAN_INVALID) {
            trill_mc_key.inner_vid = 0;
        } else {
            trill_mc_key.inner_vid = inner_vlan;
        }
    }
    
    rv = soc_ppd_frwrd_trill_multicast_route_get(soc_sand_dev_id, &trill_mc_key, &mc_id, found);
    BCM_SAND_IF_ERR_EXIT(rv);
    *group = mc_id;   
   
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* Currently supports only change of BCM_TRILL_PORT_LOCAL:                  */
/* accepts:     local->MC+local                                             */
/*              MC+local->MC                                                */
/*              MC+local->local                                             */
/*              MC->local+MC                                                 */

int
_bcm_dpp_trill_port_replace(
    int unit,
    bcm_trill_port_t *  trill_port)
{
    bcm_trill_port_t            old_port;
    int                         rv = BCM_E_NONE;
    uint32                      flag_change, orig_flags, is_local;
   _bcm_petra_trill_mc_route_info_t            *route_list;
    int                                        route_cnt;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_GPORT_TRILL_PORT_ID_SET(old_port.trill_port_id, trill_port->name );
    old_port.name = 0;

    orig_flags = trill_port->flags;
    rv = bcm_petra_trill_port_get(unit, &old_port);
    if (rv != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port to replace(0x%x)\n"),
                  bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->name)));
    }

    flag_change = old_port.flags ^ trill_port->flags;
    
    if ( (flag_change & ~BCM_TRILL_PORT_LOCAL & ~BCM_TRILL_PORT_MULTICAST & ~BCM_TRILL_PORT_REPLACE) != 0) {
        rv = BCM_E_UNAVAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("error(%s) This flag change is not allowed.\n"),
              bcm_errmsg(rv)));
    }


    /* reset replace bit*/
    trill_port->flags &= 0xffffffdf;

    rv = _bcm_dpp_trill_port_is_local(unit, old_port.name, &is_local);
    BCMDNX_IF_ERR_EXIT(rv);


   if (is_local && !(trill_port->flags & BCM_TRILL_PORT_LOCAL)) {
       rv = _bcm_dpp_trill_port_reset_local_info(unit, old_port.name );
       BCMDNX_IF_ERR_EXIT(rv);

   }
 
   /* Change of FEC is not supported, but allowed to add a FEC. 
      A FEC may be added when a fecless MC port is also used as UC.
   */
   if ((old_port.egress_if !=  trill_port->egress_if) && (old_port.egress_if !=0 )) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("error(%s) Trill port 0x%x: Change of egress_if is not supported.) .\n"),
              bcm_errmsg(rv),trill_port->name));
   }

   if ((old_port.flags & BCM_TRILL_PORT_MULTICAST ) && !(trill_port->flags & BCM_TRILL_PORT_MULTICAST)) {
       rv = _bcm_dpp_trill_mc_route_get(unit, old_port.name, &route_list, &route_cnt);
       BCMDNX_IF_ERR_EXIT(rv);

       BCM_FREE(route_list);       

       if (route_cnt > 0) {
           BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Trill port 0x%x has multicast routes. The Multicast flag cannot be reset.\n"),
                old_port.name));
       }
  
   }
   rv = _bcm_dpp_trill_port_add(unit, trill_port, TRUE);
   BCMDNX_IF_ERR_EXIT(rv);

  

  BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    /* restore orig flags*/
   trill_port->flags = orig_flags;

   BCMDNX_FUNC_RETURN;
}
/* 
   This function may be called from bcm_dpp_trill_port_add or from _bcm_dpp_trill_port_replace.
 
   When called from bcm_dpp_trill_port_add: the flag is_replace is false,In this case the replace
   opertaion is indicated by the trill port flag BCM_TRILL_PORT_REPLACE.
 
   When called from _bcm_dpp_trill_port_replace he flag is_replace is true.
 
   When a port is added a new ID is allocated for it.
   Multiple ports may have the same nickname but for each nickname a port with id=nickname is also created.
   this port will be fecless (others may have a fec).
   This port is used for searches done by nickname such as trill_multicast_entry_add.
 
   For ports with MC fec a mapping MC->port is added. Calling  trill_multicast_entry_add for
   such port will check that the MC is the same as the FEC.
   For fecless ports trill_multicast_entry_add will create the MC->trill mapping.
 
 
   
*/    
                                                             
int
_bcm_dpp_trill_port_add(
    int unit,
    bcm_trill_port_t *  trill_port,
    int                 is_replace)
{
    _BCM_GPORT_PHY_PORT_INFO        *phy_port = NULL;
    _bcm_petra_trill_info_t         *hash_trill_info = NULL;
    _BCM_GPORT_PHY_PORT_INFO        *phy_port_fecless = NULL;
    _bcm_petra_trill_info_t         *hash_trill_info_fecless = NULL;

    int                             rv = BCM_E_NONE;
    SOC_PPD_LIF_ID                      lif_id;
    
    unsigned int                    soc_sand_dev_id;
    uint32                          soc_sand_rv  = SOC_SAND_OK;
    SOC_SAND_SUCCESS_FAILURE        success;
    SOC_PPD_FEC_ID                  fec_index = -1;

    SOC_PPD_MYMAC_TRILL_INFO        trill_info;
    SOC_PPD_FRWRD_TRILL_GLOBAL_INFO trill_global_info;
    
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE      protect_type;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO        working_fec;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO        protect_fec;
    SOC_PPD_FRWRD_FEC_PROTECT_INFO      protect_info;
    uint8                               fec_success;
    int                                 is_ecmp = 0;
    SOC_PPD_FEC_TYPE                    ecmp_fec_type;
    int                                 prev_fec;
    bcm_gport_t                         *flood_trill_port_p;
    int                                 dt_nickname;
    int                                 port_id;
    bcm_trill_port_t                    fecless_port;
    int                              virtual_nick_name_index;
    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    if(trill_port->flags & BCM_TRILL_PORT_REPLACE) {
        return _bcm_dpp_trill_port_replace(unit, trill_port);
    }

    /* add virtual nickname: using nickname from trill_port
                             return trill port id (gport) of type virtual nickname */
    if (trill_port->flags & BCM_TRILL_PORT_VIRTUAL) {

        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: virtual nicknames are supported starting with arad+ \n"), FUNCTION_NAME()));
        } else {
            /* get index for a new virtual nickname, using allocation mgr
             * return an index in range [1;3] */
            rv = bcm_dpp_am_trill_virtual_nick_name_alloc(unit, trill_port->flags, &virtual_nick_name_index);
            BCMDNX_IF_ERR_EXIT(rv);

            /* add virtual nickname in hw
               trill_info.virtual_nick_names */
            soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            trill_info.virtual_nick_names[virtual_nick_name_index-1] = trill_port->name;
            soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* virtual_nick_name_index from allocmgr has range: [0;2]
             * virtual_nick_name_index in gport: 0 means my_nickname.
             *                                   [1;3] are virtual nicknames */
            /* return trill port: trill port id:  encoding: 0:15: nickname, 16:17: virtual index */
            _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_PORT_ID_SET(trill_port->trill_port_id, 
                                                          virtual_nick_name_index, 
                                                          trill_port->name);

            BCM_EXIT;

        }
    }

    if((trill_port->flags & BCM_TRILL_PORT_DROP)
       || 
       (trill_port->flags & BCM_TRILL_PORT_COPYTOCPU)
       ||
       (trill_port->flags & BCM_TRILL_PORT_NETWORK)

       ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags")));
    }

    SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_clear(&trill_global_info);
    SOC_PPD_MYMAC_TRILL_INFO_clear(&trill_info);

    if (trill_port->egress_if != 0) {
        rv = _bcm_l3_intf_to_fec(unit, trill_port->egress_if, &fec_index );
        BCMDNX_IF_ERR_EXIT(rv);
    } 

    

    /*Set mymac info for local rbridge*/

    if (trill_port->flags & BCM_TRILL_PORT_LOCAL ) {
        rv = _bcm_dpp_trill_port_set_local_info(unit, trill_port->name);
        BCMDNX_IF_ERR_EXIT(rv);
 
    }
    
    soc_sand_rv = soc_ppd_frwrd_trill_global_info_get(soc_sand_dev_id, &trill_global_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if (trill_global_info.cfg_ttl != trill_port->hopcount) {
        /*
        LOG_WARN(BSL_LS_BCM_TRILL,
                 (BSL_META_U(unit,
                             "Port hopcount must be equal to global trill hopcount.port value ignored.")));
        */
    }

    if (is_replace) {
        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                           &trill_port->trill_port_id,
                                           (shr_htb_data_t*)(void *)&phy_port,
                                           FALSE);        
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port (0x%x)\n"),
                          bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
        }

        rv = _bcm_dpp_sw_db_hash_trill_info_find(unit,
                                           &trill_port->trill_port_id,
                                           (shr_htb_data_t*)(void *)&hash_trill_info,
                                           FALSE);        
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port (0x%x)\n"),
                          bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
        }

    } else {
        rv = _bcm_dpp_trill_alloc_port_id(unit,&port_id);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_GPORT_TRILL_PORT_ID_SET(trill_port->trill_port_id, port_id);

        BCMDNX_ALLOC(hash_trill_info, sizeof(_bcm_petra_trill_info_t), "hash_trill_info");
        if (hash_trill_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }
        BCMDNX_ALLOC(phy_port, sizeof(_BCM_GPORT_PHY_PORT_INFO), "trill_gport");
        if (phy_port == NULL) {
            BCM_FREE(hash_trill_info);
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }

        sal_memset(phy_port, 0, sizeof(_BCM_GPORT_PHY_PORT_INFO));
        sal_memset(hash_trill_info, 0, sizeof(_bcm_petra_trill_info_t));
    }

    rv = _bcm_dpp_trill_port_is_ecmp(unit, trill_port, &is_ecmp, &ecmp_fec_type );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    if (is_ecmp) {
        phy_port->type = _BCM_DPP_GPORT_IN_TYPE_FEC;
        hash_trill_info->type = _BCM_PETRA_TRILL_PORT_MULTIPATH;
    } else if (trill_port->flags & BCM_TRILL_PORT_MULTICAST) {
        phy_port->type = _BCM_DPP_GPORT_IN_TYPE_TRILL_MC;
        hash_trill_info->type = _BCM_PETRA_TRILL_PORT_MC;
    } else {
        phy_port->type = _BCM_DPP_GPORT_IN_TYPE_FEC;
        hash_trill_info->type = _BCM_PETRA_TRILL_PORT_UNI;

    }

    prev_fec = hash_trill_info->fec_id;

    if (!is_replace) {

        phy_port->encap_id = trill_port->name; /* encap_id = nickname */
        phy_port->phy_gport = fec_index; 
        hash_trill_info->fec_id = fec_index; 

        rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                         &(trill_port->trill_port_id), 
                                         phy_port);        
        if (BCM_FAILURE(rv)) {
            BCM_FREE(hash_trill_info);       
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
        }

        rv = _bcm_dpp_sw_db_hash_trill_info_insert(unit,
                                         &(trill_port->trill_port_id), 
                                         hash_trill_info);        
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
        }

        rv = _bcm_petra_add_to_trill_port_list(unit, trill_port->trill_port_id);
        BCMDNX_IF_ERR_EXIT(rv);

        bcm_trill_port_t_init(&fecless_port);
        BCM_GPORT_TRILL_PORT_ID_SET(fecless_port.trill_port_id, trill_port->name);

        /* Find the name=id copy*/
        rv = _bcm_dpp_sw_db_hash_trill_info_find(unit,
                                           &(fecless_port.trill_port_id),
                                           (shr_htb_data_t*)(void *)&hash_trill_info_fecless,
                                           FALSE);        
        if (BCM_FAILURE(rv)) {

            BCM_GPORT_TRILL_PORT_ID_SET(fecless_port.trill_port_id, trill_port->name);

            BCMDNX_ALLOC(hash_trill_info_fecless, sizeof(_bcm_petra_trill_info_t), "hash_trill_info");
            if (hash_trill_info_fecless == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
            }
            BCMDNX_ALLOC(phy_port_fecless, sizeof(_BCM_GPORT_PHY_PORT_INFO), "trill_gport");
            if (phy_port_fecless == NULL) {
                    BCM_FREE(hash_trill_info_fecless);
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
            }

            sal_memcpy(phy_port_fecless, phy_port, sizeof(_BCM_GPORT_PHY_PORT_INFO));
            sal_memcpy(hash_trill_info_fecless, hash_trill_info, sizeof(_bcm_petra_trill_info_t));

            phy_port_fecless->phy_gport = -1; 
            hash_trill_info_fecless->fec_id = -1; 

            rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                             &(fecless_port.trill_port_id), 
                                             phy_port_fecless);        
            if (BCM_FAILURE(rv)) {
                BCM_FREE(hash_trill_info);       
                BCM_FREE(hash_trill_info_fecless);
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)\n"),
                          bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(fecless_port.trill_port_id)));
            }

            rv = _bcm_dpp_sw_db_hash_trill_info_insert(unit,
                                             &(fecless_port.trill_port_id), 
                                             hash_trill_info_fecless);        
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)\n"),
                          bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(fecless_port.trill_port_id)));
            }
                
        }
        hash_trill_info_fecless->use_cnt++;
    } else if (fec_index != -1) {
        /* Add FEC on MC port - Needed for flooding*/
        if (prev_fec != -1) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x) to FEC %d already has Fec %d\n"),
                bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id), fec_index, hash_trill_info->fec_id));   
        } else {
            phy_port->phy_gport = fec_index; 
            hash_trill_info->fec_id = fec_index; 

        }
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_gport_update_port_state(unit, bcm_dpp_wb_gport_oper_update, &(trill_port->trill_port_id), phy_port);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_wb_gport_update_trill_info_state(unit, bcm_dpp_wb_gport_oper_add, &(trill_port->trill_port_id), hash_trill_info);
    BCMDNX_IF_ERR_EXIT(rv);

#endif /* BCM_WARM_BOOT_SUPPORT */

    }

    /* Trill port with FEC may be UC,MC or MC+UC. In the case of MC+UC the MC must be fecless and the FEC */
    /* is for the UC behavior.                                                                            */ 
    /* Replace operation for fec change is not supported.                                                                            */ 

    if ( ( (fec_index != -1) && !is_ecmp   ) 
         || (is_ecmp && (ecmp_fec_type == SOC_PPC_FEC_TYPE_TRILL_UC) )) {
        
        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, fec_index,
                                    &protect_type, &working_fec,
                                    &protect_fec, &protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        
        /* For MC with FEC : update tree in FEC */
        if (working_fec.type == SOC_PPC_FEC_TYPE_TRILL_MC) {
            if (!(trill_port->flags & BCM_TRILL_PORT_MULTICAST)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("error(%s) Non MC trill (0x%x) has a MC fec %d\n"),
                      bcm_errmsg(rv), trill_port->trill_port_id, fec_index));
        
            }
            if (is_replace) {
                /* For replace with MC FEC we assume it is for flooding.                         */
                /* In this case we check that the MC group is associated with a trill dist tree  */
                /* by bcm_trill_multicast_entry_add.                                             */
                /* It is true also for create (not only replace) but since we allow the flood MC */
                /* to be the same as port dist_tree we assume the association will be done later. */

                /* old implementation that use sw db for mapping mc-id to nickname*/
                if (SOC_DPP_CONFIG(unit)->trill.mc_id)
                {
                    rv = _bcm_dpp_mc_to_trill_get(unit,
                                                  working_fec.dest.dest_val, 
                                                  &flood_trill_port_p);
                    if ( rv != BCM_E_NONE) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,  (_BSL_BCM_MSG("error(%s) The MC flooding group %d is not associated to a trill rootname.\n"),
                          bcm_errmsg(rv), working_fec.dest.dest_val));
                    }
                }
                else{
                    rv = _bcm_dpp_dt_nickname_trill_get(unit,
                                                        trill_port->trill_port_id,
                                                        &dt_nickname);
                    if ( rv != BCM_E_NONE) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,  (_BSL_BCM_MSG("error(%s) The trill port %d is not associated to a distribution tree.\n"),
                          bcm_errmsg(rv), trill_port->trill_port_id));
                    }
                }
            }

            if (trill_port->name == 0) {
                rv = _bcm_dpp_gport_fix_fec_type(unit, fec_index, SOC_PPD_FEC_TYPE_SIMPLE_DEST);
                if (BCM_FAILURE(rv)) {
                    BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Changing fec %d to simple failed (0x%x)\n"),
                          bcm_errmsg(rv), fec_index, BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id)));
                }
            } else {
                working_fec.app_info.dist_tree_nick = trill_port->name;
        
                soc_sand_rv = soc_ppd_frwrd_fec_entry_add(soc_sand_dev_id, fec_index,
                                    protect_type, &working_fec,
                                    &protect_fec, &protect_info, &fec_success);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            /* old implementation that use sw db for mapping mc-id to nickname*/
            if (SOC_DPP_CONFIG(unit)->trill.mc_id){
                rv = _bcm_dpp_mc_to_trill_add(unit,  working_fec.dest.dest_val, trill_port->trill_port_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }

        
        } else {
            /* Add route for unicast entry only.For multicast the route is added explicitly by bcm_trill_multicast_add/source add */
            if ( !(trill_port->flags & BCM_TRILL_PORT_LOCAL ) ) {
                SOC_PPC_FRWRD_DECISION_INFO fwd_decision; 

                /* learn data: built from forward decision. (dest_id=fec_id) */
                SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision); 

                SOC_PPD_FRWRD_DECISION_FEC_SET(
                   unit, &fwd_decision, fec_index, soc_sand_rv); 

                /* Create unicast Rbridge with transit functionality if BCM_TRILL_PORT_TUNNEL is not set */
                if (!(trill_port->flags & BCM_TRILL_PORT_TUNNEL)) {
                    soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_add(soc_sand_dev_id, lif_id, trill_port->name, &fwd_decision, &success);  
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    SOC_SAND_IF_FAIL_RETURN(success);
                }
                /* Create SVP if BCM_TRILL_PORT_EGRESS is not set */
                if (!(trill_port->flags & BCM_TRILL_PORT_EGRESS)) {        

                    SOC_PPD_L2_LIF_TRILL_INFO lif_trill_info;
                    SOC_PPC_L2_LIF_TRILL_INFO_clear(&lif_trill_info); 

                    lif_trill_info.fec_id = fec_index;
                    lif_trill_info.learn_enable  = TRUE;

                    /* learn info */
                    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                       SOC_PPD_FRWRD_DECISION_FEC_SET(
                          unit, &(lif_trill_info.learn_info), fec_index, soc_sand_rv); 
                    }
                    if (SOC_IS_JERICHO(unit)) {
                       SOC_PPD_FRWRD_DECISION_TRILL_SET(
                          unit, &(lif_trill_info.learn_info), trill_port->name, 0, fec_index, soc_sand_rv);
                    }

                    rv = bcm_dpp_trill_lif_alloc(unit,0,&lif_id);
                    BCMDNX_IF_ERR_EXIT(rv);
                    
                    soc_sand_rv = soc_ppd_frwrd_trill_ingress_lif_add(soc_sand_dev_id, lif_id, trill_port->name, &lif_trill_info, &success);  
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    SOC_SAND_IF_FAIL_RETURN(success);
                }

                /* old implementation(multicast adjacency added as a part of trill_port_add function*/
                if (SOC_DPP_CONFIG(unit)->trill.mult_adjacency)
                {
                    if (!is_ecmp) {
                    
                        SOC_PPD_TRILL_ADJ_INFO          mac_auth_info;
                        bcm_l3_egress_t                 egress_if;
                        SOC_SAND_PP_MAC_ADDRESS         soc_sand_mac;
                        int                             i;
                        _bcm_dpp_gport_info_t           gport_info;

                        /* Current assumption: FEC and LL reside on the same device) */
                        rv = bcm_petra_l3_egress_get(unit, trill_port->egress_if, &egress_if); /* Lookup FEC information */
                        BCMDNX_IF_ERR_EXIT(rv);                
                    
                        SOC_PPD_TRILL_ADJ_INFO_clear(&mac_auth_info);
                        
                        rv = _bcm_dpp_gport_to_phy_port(unit, egress_if.port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT , &gport_info); 
                        BCMDNX_IF_ERR_EXIT(rv);
                        mac_auth_info.expect_system_port.sys_id = gport_info.sys_port;
                    
                        rv = bcm_petra_l3_egress_get(unit, egress_if.encap_id, &egress_if); /* Lookup LL information */
                        BCMDNX_IF_ERR_EXIT(rv);                
                    
                        /* get encap ID from FEC */
                        mac_auth_info.expect_adjacent_eep = working_fec.eep;
                        for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
                            soc_sand_mac.address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - i - 1] = egress_if.mac_addr[i];
                        }
                    
                        soc_sand_rv = soc_ppd_frwrd_trill_adj_info_set(soc_sand_dev_id, &soc_sand_mac, &mac_auth_info, TRUE, &success);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        SOC_SAND_IF_FAIL_RETURN(success);
                    }
                }
            }
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
   BCMDNX_FUNC_RETURN;
}
/* The reset local is optional - It is used in replace operations.*/
int
_bcm_dpp_trill_port_delete(
    int unit,
    bcm_gport_t trill_port_id,
    int         reset_local_info)
{
    _bcm_petra_trill_info_t     *hash_trill_info_ptr;
    _BCM_GPORT_PHY_PORT_INFO    *phy_port_ptr;
    int                         rv = BCM_E_NONE;
    
    unsigned int                soc_sand_dev_id;
    uint32                      soc_sand_rv  = SOC_SAND_OK;
    SOC_PPD_LIF_ID                  lif_id=0;

    SOC_PPD_MYMAC_TRILL_INFO        trill_info;
    SOC_PPD_FRWRD_TRILL_GLOBAL_INFO tril_global_info;
    SOC_PPD_FEC_ID                  fec_index = -1;
    
    bcm_trill_port_t            trill_port;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE      protect_type;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO        working_fec;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO        protect_fec;
    SOC_PPD_FRWRD_FEC_PROTECT_INFO      protect_info;
    
    _bcm_petra_trill_mc_route_info_t            *route_list;
     int                                        route_cnt;
     int                                        is_ecmp = 0;
#ifdef BCM_ARAD_SUPPORT
     soc_dpp_config_arad_t                      *dpp_arad;
#endif /* BCM_ARAD_SUPPORT */
     SOC_PPD_FEC_TYPE                           ecmp_fec_type;
     bcm_trill_name_t                           trill_name;
     int                                        virtual_nick_name_index;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

#ifdef BCM_ARAD_SUPPORT
    /* Check that we're not in TM mode, to prevent segmentation faults */
    dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    if (SOC_IS_ARAD(unit) && !dpp_arad->init.pp_enable) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Trill in not available in TM mode\n")));
    }
#endif /* BCM_ARAD_SUPPORT */

    /* remove virtual nickname if gport is a trill virtual nickname gport */
    if (BCM_GPORT_TRILL_PORT_ID_IS_VIRTUAL(trill_port_id)) {
        /* return an index in range [1;3] */
        virtual_nick_name_index = _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_GET(trill_port_id);

        /* remove virtual nickname in sw */
        rv = bcm_dpp_am_trill_virtual_nick_name_dealloc(unit, 0, virtual_nick_name_index);
        BCMDNX_IF_ERR_EXIT(rv);
        /* remove virtual nickname in hw */
        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* virtual_nick_names in range [0;2] */
        trill_info.virtual_nick_names[virtual_nick_name_index-1] = 0;
        soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCM_EXIT;
    }

    trill_port.trill_port_id = trill_port_id;
    trill_port.name = 0;
    rv = bcm_petra_trill_port_get(unit, &trill_port);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_trill_mc_route_get(unit, trill_port.name, &route_list, &route_cnt);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_FREE(route_list);       

    if (route_cnt > 0) {
           BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Trill port 0x%x has multicast routes. The Multicast flag cannot be reset.\n"),
                trill_port.name));
    }


    /* Remove entry in hash_vlan*/
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       &(trill_port_id),
                                       (shr_htb_data_t*)(void *)&phy_port_ptr,
                                       TRUE);        
    if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Removing trill port (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id)));
    }

    /* Remove entry in hash_trill_info*/
    rv = _bcm_dpp_sw_db_hash_trill_info_find(unit,
                                       &(trill_port_id),
                                       (shr_htb_data_t*)(void *)&hash_trill_info_ptr,
                                       FALSE);        
    if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Removing trill info (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id)));
    }
    else {
        fec_index = hash_trill_info_ptr->fec_id;

        rv = _bcm_dpp_sw_db_hash_trill_info_find(unit,
                                           &(trill_port_id),
                                           (shr_htb_data_t*)(void *)&hash_trill_info_ptr,
                                           TRUE);        
        if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Removing trill info (0x%x)\n"),
                          bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id)));
        }
    }

    if (fec_index != -1) {

         soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, fec_index,
                                    &protect_type, &working_fec,
                                    &protect_fec, &protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      
    }

    SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_clear(&tril_global_info);
    SOC_PPD_MYMAC_TRILL_INFO_clear(&trill_info);

    
   
    if (reset_local_info) {
        rv = _bcm_dpp_trill_port_reset_local_info(unit, phy_port_ptr->encap_id );
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_trill_port_is_ecmp(unit, &trill_port, &is_ecmp, &ecmp_fec_type );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if ( (phy_port_ptr->type != _BCM_DPP_GPORT_IN_TYPE_TRILL_MC) && !(trill_port.flags & BCM_TRILL_PORT_LOCAL ) && !is_ecmp) {        
        /* Remove unicast Rbridge with transit functionality if BCM_TRILL_PORT_TUNNEL is not set */
        if (!(trill_port.flags & BCM_TRILL_PORT_TUNNEL)) {   
            soc_sand_rv = soc_ppd_frwrd_trill_unicast_route_remove(soc_sand_dev_id, phy_port_ptr->encap_id , &lif_id);  
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }          
        /* Remove SVP if BCM_TRILL_PORT_EGRESS is not set */
        if (!(trill_port.flags & BCM_TRILL_PORT_EGRESS)) {   
            soc_sand_rv = soc_ppd_frwrd_trill_ingress_lif_remove(soc_sand_dev_id, phy_port_ptr->encap_id , &lif_id);  
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }        

        /* old implementation(multicast adjacency deleted as a part of trill_port_delete function*/
        if (SOC_DPP_CONFIG(unit)->trill.mult_adjacency){
            SOC_PPD_TRILL_ADJ_INFO      mac_auth_info;
            bcm_l3_egress_t             egr;
            SOC_SAND_SUCCESS_FAILURE    success;
            SOC_SAND_PP_MAC_ADDRESS     soc_sand_mac;         
            int i;
            _bcm_dpp_gport_info_t       gport_info;

            SOC_PPD_TRILL_ADJ_INFO_clear(&mac_auth_info);
            soc_sand_SAND_PP_SYS_PORT_ID_clear(&mac_auth_info.expect_system_port);
            
            rv = bcm_petra_l3_egress_get(unit, trill_port.egress_if, &egr);
            BCMDNX_IF_ERR_EXIT(rv);
           
            if (SOC_IS_PETRAB(unit)) {
                rv = _bcm_dpp_gport_to_phy_port(unit, egr.port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT , &gport_info); 
                BCMDNX_IF_ERR_EXIT(rv);
                mac_auth_info.expect_system_port.sys_id = gport_info.sys_port;
            }
                 
             mac_auth_info.expect_adjacent_eep = working_fec.eep;
             for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
                  soc_sand_mac.address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - i - 1] = egr.mac_addr[i];
             }
           
             soc_sand_rv = soc_ppd_frwrd_trill_adj_info_set(soc_sand_dev_id, &soc_sand_mac, &mac_auth_info, FALSE, &success);
             BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
             SOC_SAND_IF_FAIL_RETURN(success); 
        }
           
    }
    if (fec_index != -1) {
        /* old implementation that use sw db for mapping mc-id to nickname*/
        if (SOC_DPP_CONFIG(unit)->trill.mc_id){
            if (working_fec.type==SOC_PPC_FEC_TYPE_TRILL_MC) {
                bcm_gport_t  *existing_port = NULL;

                /* checking sw db at this stage as it could be removed previously in case of transit mc rb*/
                rv = _bcm_dpp_mc_to_trill_get(unit,
                                              working_fec.dest.dest_val, 
                                              &existing_port);
                if (rv == BCM_E_NONE)
                {                    
                    _bcm_dpp_mc_to_trill_remove( unit, working_fec.dest.dest_val, trill_port.name);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }

        soc_sand_rv = soc_ppd_frwrd_fec_remove(soc_sand_dev_id, fec_index); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
   _bcm_petra_remove_from_trill_port_list(unit, trill_port_id);
   BCM_FREE(hash_trill_info_ptr);
   BCM_FREE(phy_port_ptr);

   /* Find the name=id copy and remove it if the usage is 0*/
   trill_name = trill_port.name;

   bcm_trill_port_t_init(&trill_port);
   BCM_GPORT_TRILL_PORT_ID_SET(trill_port.trill_port_id, trill_name);

   rv = _bcm_dpp_sw_db_hash_trill_info_find(unit,
                                      &(trill_port.trill_port_id),
                                      (shr_htb_data_t*)(void *)&hash_trill_info_ptr,
                                      FALSE);        
   if (BCM_FAILURE(rv)) {
           BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) hash_trill_info not found for (0x%x)\n"),
                     bcm_errmsg(rv), trill_name));
   }
   hash_trill_info_ptr->use_cnt--;

   if (hash_trill_info_ptr->use_cnt == 0) {

       rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                          &(trill_port.trill_port_id),
                                          (shr_htb_data_t*)(void *)&phy_port_ptr,
                                          TRUE);        
       if (BCM_FAILURE(rv)) {
               BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) hash_vlan_find not fouund for trill port (0x%x)\n"),
                         bcm_errmsg(rv), trill_name));
       }
       rv = _bcm_dpp_sw_db_hash_trill_info_find(unit,
                                          &(trill_port.trill_port_id),
                                          (shr_htb_data_t*)(void *)&hash_trill_info_ptr,
                                          TRUE);        
       if (BCM_FAILURE(rv)) {
               BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) hash_trill_info not found for (0x%x)\n"),
                         bcm_errmsg(rv), trill_name));

       }
       BCM_FREE(hash_trill_info_ptr);
       BCM_FREE(phy_port_ptr);

   }



   BCMDNX_IF_ERR_EXIT(rv);
    
exit:
   BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_trill_multicast_entry_add(
    int unit, 
    bcm_trill_multicast_entry_t *trill_mc) 
{
     SOC_PPD_TRILL_MC_ROUTE_KEY   trill_mc_key;
     int                          rv = BCM_E_NONE;
     bcm_trill_port_t             trill_port;
     bcm_gport_t                  *existing_port = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);
   
    SOC_PPD_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);

    /* For ARAD:                                */
    /*   1.just add the route without source,   */
    /*   2.Add the mapping trill_port-mc group  (for Soc_petra it is done when a MC trill port is added to MC group*/
    /*                                           For arad the trill port is not added to group so the mapping is done here.)*/
    if (SOC_IS_ARAD(unit)) {
       bcm_trill_port_t_init(&trill_port);
       /* old implementation that use sw db for mapping mc-id to nickname*/
       if (SOC_DPP_CONFIG(unit)->trill.mc_id){

           rv = _bcm_dpp_mc_to_trill_get(unit, 
                                         trill_mc->group, 
                                         &existing_port);
           if (rv != BCM_E_NOT_FOUND) {

               trill_port.trill_port_id = *existing_port;
               rv = bcm_petra_trill_port_get(unit, &trill_port);
               BCMDNX_IF_ERR_EXIT(rv);

               if (trill_port.name != trill_mc->root_name) {
                   LOG_ERROR(BSL_LS_BCM_TRILL,
                             (BSL_META_U(unit,
                                         "Multicast group %x already defined for a different trill port (0x%x)\n"),
                              trill_mc->group, BCM_GPORT_TRILL_PORT_ID_GET(*existing_port)));
                   BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, ( _BSL_BCM_MSG("The MC group already defined for a different trill port")));
                }
           } else {

               trill_port.name = trill_mc->root_name;

               BCM_GPORT_TRILL_PORT_ID_SET(trill_port.trill_port_id, trill_port.name );
               rv = bcm_petra_trill_port_get(unit, &trill_port);
               BCMDNX_IF_ERR_EXIT(rv);

               rv = _bcm_dpp_mc_to_trill_add(unit, trill_mc->group, trill_port.trill_port_id);
               BCMDNX_IF_ERR_EXIT(rv);
             }
          } 

       /* set transit forwarding HW lookups */
       if ((trill_mc->flags & BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK) == 0) {
           /* Add  without source*/
           rv =  _ppd_trill_multicast_route_add(unit, trill_mc->flags, 
                                                trill_mc->root_name, 
                                                trill_mc->c_vlan, 
                                                trill_mc->c_vlan_inner ,
                                                trill_mc->group, 0,0);
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }else{ /*Petra-B only*/
        rv = _bcm_pb_trill_multicast_entry_add(unit,trill_mc);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}
  
int 
bcm_petra_trill_multicast_entry_delete(
    int unit, 
    bcm_trill_multicast_entry_t *trill_mc)
{
     bcm_trill_port_t                       trill_port;
     int                                    rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);
   
    /* For ARAD just remove  the route. */
    if (SOC_IS_ARAD(unit)) {

        rv = bcm_petra_trill_multicast_entry_get(unit, trill_mc);
        BCMDNX_IF_ERR_EXIT(rv);

        bcm_trill_port_t_init(&trill_port);
        trill_port.name = trill_mc->root_name;
        BCM_GPORT_TRILL_PORT_ID_SET(trill_port.trill_port_id, trill_port.name);

        rv = bcm_petra_trill_port_get(unit, &trill_port);
        BCMDNX_IF_ERR_EXIT(rv);

        /* old implementation that use sw db for mapping mc-id to nickname*/
        if (SOC_DPP_CONFIG(unit)->trill.mc_id){
            rv = _bcm_dpp_mc_to_trill_remove(unit, trill_mc->group, trill_mc->root_name);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* delete transit forwarding */
        if ((trill_mc->flags & BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK) == 0) {
            /* Remove  without source*/
            rv =  _ppd_trill_multicast_route_remove(unit, trill_mc->flags,
                                                    trill_mc->root_name, 
                                                    trill_mc->c_vlan, 
                                                    trill_mc->c_vlan_inner, 
                                                    trill_mc->group, 0);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }else{ /*Petra-B */
        /* LCOV_EXCL_START */
        rv = _bcm_pb_trill_multicast_entry_delete(unit,trill_mc);
        BCMDNX_IF_ERR_EXIT(rv);
         /* LCOV_EXCL_STOP */
    }

    BCM_EXIT;

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/* Get TRILL multicast entry */
int bcm_petra_trill_multicast_entry_get(
    int unit,
    bcm_trill_multicast_entry_t *trill_mc)
{
    int                                    rv = BCM_E_NONE;
    uint8                                  found = FALSE;
    bcm_trill_port_t                       trill_port;
    bcm_gport_t                            *existing_port = NULL;
    bcm_multicast_t                        group;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);   

    /* For ARAD:                                */
    /*   1.just get the route without source,   */
    /*   2.Get the mapping trill_port-mc group  (for Soc_petra it is done when a MC trill port is added to MC group*/
    /*                                           For arad the trill port is not added to group so the mapping is done here.)*/
    if (SOC_IS_ARAD(unit)) {
        /* old implementation that use sw db for mapping mc-id to nickname*/
        if (SOC_DPP_CONFIG(unit)->trill.mc_id) {

            rv = _bcm_dpp_mc_to_trill_get(unit, 
                                          trill_mc->group, 
                                          &existing_port);
             BCMDNX_IF_ERR_EXIT(rv);

             bcm_trill_port_t_init(&trill_port);

             trill_port.trill_port_id = *existing_port;
             rv = bcm_petra_trill_port_get(unit, &trill_port);

            BCMDNX_IF_ERR_EXIT(rv);
            if (trill_port.name != trill_mc->root_name) {
                LOG_ERROR(BSL_LS_BCM_TRILL,
                          (BSL_META_U(unit,
                                      "Trill multicast entry name=%x group %x not found.\n"),
                           trill_mc->group, BCM_GPORT_TRILL_PORT_ID_GET(*existing_port)));
                 BCMDNX_ERR_EXIT_MSG(_SHR_E_NOT_FOUND, (_BSL_BCM_MSG("Trill multicast entry not found")));

           }
        }
        /* Get  without source*/
        rv =  _ppd_trill_multicast_route_get(unit, trill_mc->flags, 
                                             trill_mc->root_name, 
                                             trill_mc->c_vlan, 
                                             trill_mc->c_vlan_inner ,
                                             0,0,
                                             &group, 
                                             &found);
         BCMDNX_IF_ERR_EXIT(rv);

         if (found)
            trill_mc->group = group;
         else{
             if(!SOC_DPP_CONFIG(unit)->trill.mc_id){
                 BCMDNX_ERR_EXIT_MSG(_SHR_E_NOT_FOUND, (_BSL_BCM_MSG("Trill multicast entry not found")));
             }
         }

    }else /*Petra-B*/{
        /* LCOV_EXCL_START */
        rv = _bcm_pb_trill_multicast_entry_get(unit,trill_mc);            
        BCMDNX_IF_ERR_EXIT(rv);
        /* LCOV_EXCL_STOP */
    }

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_multicast_adjacency_add(
                     int unit,
                     bcm_trill_multicast_adjacency_t *trill_multicast_adjacency)
{
     BCMDNX_INIT_FUNC_DEFS;
     BCM_DPP_UNIT_CHECK(unit);
     _BCM_DPP_SWITCH_API_START(unit);
     BCMDNX_IF_ERR_EXIT(_bcm_dpp_trill_multicast_adjacency_add(unit, trill_multicast_adjacency));
exit:
     _BCM_DPP_SWITCH_API_END(unit);
     BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_multicast_adjacency_delete(
                     int unit,
                     bcm_trill_multicast_adjacency_t *trill_multicast_adjacency)
{
     BCMDNX_INIT_FUNC_DEFS;
     BCM_DPP_UNIT_CHECK(unit);
     _BCM_DPP_SWITCH_API_START(unit);
     BCMDNX_IF_ERR_EXIT(_bcm_dpp_trill_multicast_adjacency_delete(unit, trill_multicast_adjacency));
exit:
     _BCM_DPP_SWITCH_API_END(unit);
     BCMDNX_FUNC_RETURN;
}


int bcm_petra_trill_multicast_adjacency_delete_all(int unit)
{
     BCMDNX_INIT_FUNC_DEFS;
     BCM_EXIT;
exit:
     BCMDNX_FUNC_RETURN;
}


int bcm_petra_trill_multicast_adjacency_traverse(
                     int unit,
                     bcm_trill_multicast_adjacency_traverse_cb a,
                     void *b)
{
     BCMDNX_INIT_FUNC_DEFS;
     BCM_EXIT;
exit:
     BCMDNX_FUNC_RETURN;
}


int bcm_petra_trill_multicast_delete_all(
    int unit, 
    bcm_trill_name_t root_name){

     BCMDNX_INIT_FUNC_DEFS;
     BCM_EXIT;
exit:
     BCMDNX_FUNC_RETURN;
}

/* LCOV_EXCL_START */
int 
bcm_petra_trill_multicast_source_add(
    int unit, 
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_rbridge_name, 
    bcm_gport_t port,
    bcm_if_t intf) 
{
    int                                     rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* For ARAD just return unavail */
    if (SOC_IS_ARAD(unit))
        rv = BCM_E_UNAVAIL;
    else /*Petra-B*/ {
        rv = _bcm_pb_trill_multicast_source_add(unit, root_name, source_rbridge_name, port,intf);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}
/* LCOV_EXCL_STOP */
/* LCOV_EXCL_START */
int 
bcm_petra_trill_multicast_source_delete(
    int unit,
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_name, 
    bcm_gport_t gport,
    bcm_if_t    intf)
{
    int      rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* For ARAD just return unavail */
    if (SOC_IS_ARAD(unit))
        rv = BCM_E_UNAVAIL;
    else{
        rv = bcm_petra_trill_multicast_source_delete(unit,root_name,source_name,gport,intf);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_trill_multicast_adjacency_add
 * Purpose:
 *      Set multicast adjacency entry.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      trill_multicast_adjacency   - (IN)
 *                   (IN) port        Trill Network port
 *                   (IN) encap_intf  Neghbor Interface
 *                   (IN) flags       not in use
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_multicast_adjacency_add( int unit,
                                            bcm_trill_multicast_adjacency_t *trill_multicast_adjacency)
{

    SOC_PPD_TRILL_ADJ_INFO    mac_auth_info;
    SOC_SAND_PP_MAC_ADDRESS   soc_sand_mac;
    SOC_SAND_SUCCESS_FAILURE  success;
    bcm_l3_egress_t           egress_if;
    int                       i;
    int                       rv = BCM_E_NONE;
    uint32                    soc_sand_rv  = SOC_SAND_OK;
    _bcm_dpp_gport_info_t     gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPD_TRILL_ADJ_INFO_clear(&mac_auth_info);
    soc_sand_SAND_PP_SYS_PORT_ID_clear(&mac_auth_info.expect_system_port);

    /* get system port id - used as a key in multiacst adjacency table*/
    rv = _bcm_dpp_gport_to_phy_port(unit, trill_multicast_adjacency->port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT , &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv); 
    mac_auth_info.expect_system_port.sys_id = gport_info.sys_port;

    /* get eep adjacency - used as a result in multiacst adjacency table*/
    rv = _bcm_l3_intf_to_fec(unit, trill_multicast_adjacency->encap_intf, &mac_auth_info.expect_adjacent_eep );
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_l3_egress_t_init(&egress_if);
    egress_if.flags    = BCM_L3_EGRESS_ONLY;

    /* get mac*/
    rv = bcm_petra_l3_egress_get(unit, trill_multicast_adjacency->encap_intf, &egress_if); /* Lookup LL information */
    BCMDNX_IF_ERR_EXIT(rv);

    for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
        soc_sand_mac.address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - i - 1] = egress_if.mac_addr[i];
    }

    /* Set multicast adjacency entry Keys: port,mac Result:eep adjacency*/
    soc_sand_rv = soc_ppd_frwrd_trill_adj_info_set(unit, &soc_sand_mac, &mac_auth_info, TRUE, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success);

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_trill_multicast_adjacency_delete
 * Purpose:
 *      Set multicast adjacency entry.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      trill_multicast_adjacency   - (IN)
 *                   (IN) port        Trill Network port
 *                   (IN) encap_intf  Neghbor Interface
 *                   (IN) flags       not in use
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_multicast_adjacency_delete( int unit,
                                               bcm_trill_multicast_adjacency_t *trill_multicast_adjacency)
{
 
    SOC_PPD_TRILL_ADJ_INFO    mac_auth_info;
    bcm_l3_egress_t           egress_if;
    SOC_SAND_SUCCESS_FAILURE  success;
    uint32                    soc_sand_rv  = SOC_SAND_OK;
    SOC_SAND_PP_MAC_ADDRESS   soc_sand_mac;
    int                       i;
    _bcm_dpp_gport_info_t     gport_info;
    int                       rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPD_TRILL_ADJ_INFO_clear(&mac_auth_info);
    soc_sand_SAND_PP_SYS_PORT_ID_clear(&mac_auth_info.expect_system_port);

    /* get system port id - used as a key in multiacst adjacency table*/
    rv = _bcm_dpp_gport_to_phy_port(unit, trill_multicast_adjacency->port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);
    mac_auth_info.expect_system_port.sys_id = gport_info.sys_port;
        
    bcm_l3_egress_t_init(&egress_if);
    egress_if.flags    = BCM_L3_EGRESS_ONLY;

    /* get mac*/
    rv = bcm_petra_l3_egress_get(unit, trill_multicast_adjacency->encap_intf, &egress_if);/* Lookup LL information */
    BCMDNX_IF_ERR_EXIT(rv);

    for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
        soc_sand_mac.address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - i - 1] = egress_if.mac_addr[i];
    }
    /* Remove multicast adjacency entry Keys: port,mac*/
    soc_sand_rv = soc_ppd_frwrd_trill_adj_info_set(unit, &soc_sand_mac, &mac_auth_info, FALSE, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success); 

    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;

}

 /* LCOV_EXCL_STOP */
int
_bcm_dpp_mc_to_trill_add(
    int unit,
    bcm_multicast_t group, 
    bcm_gport_t port)
{
    int  rv = BCM_E_NONE;
    bcm_gport_t  *port_p = NULL;
    bcm_gport_t  *existing_port = NULL;
    _BCM_GPORT_PHY_PORT_INFO *phy_port=NULL;
    int                     mc_id;

    BCMDNX_INIT_FUNC_DEFS;
    mc_id = _BCM_MULTICAST_ID_GET(group);

    rv = _bcm_dpp_mc_to_trill_get(unit,
                                  group, 
                                  &existing_port);
    if (rv != BCM_E_NOT_FOUND) {
        if (*existing_port != port) {
            LOG_ERROR(BSL_LS_BCM_TRILL,
                      (BSL_META_U(unit,
                                  "Multicast group %x already defined for a different trill port (0x%x)\n"),
                       group, BCM_GPORT_TRILL_PORT_ID_GET(*existing_port)));
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("The MC group already defined for a different trill port")));

        }
    }
    else { 
        BCMDNX_ALLOC(port_p, sizeof(bcm_gport_t), "mc_trill_port");
        if (port_p == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        *port_p = port;
        rv = _bcm_dpp_sw_db_hash_mc_trill_insert(unit,
                                          &(mc_id),
                                          port_p);        
        if (BCM_FAILURE(rv)) {
            BCM_FREE(port_p);
            LOG_ERROR(BSL_LS_BCM_TRILL,
                      (BSL_META_U(unit,
                                  "error(%s) Updating MC trill mapping (0x%x)\n"),
                       bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(port)));
            BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);

        }
    }
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       (&port), 
                                       (shr_htb_data_t*)(void *)&phy_port,
                                       FALSE);        
    if (!phy_port || BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_dpp_sw_db_hash_vlan_find failed")));
    }
   
     phy_port->phy_gport = group;

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_gport_update_port_state(unit, bcm_dpp_wb_gport_oper_update, &port, phy_port);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
           
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
int
_bcm_dpp_mc_to_trill_remove(
    int unit,
    bcm_multicast_t group, 
    bcm_trill_name_t name)
{
    int  rv = BCM_E_NONE;
    bcm_gport_t  *port_p = NULL;
    bcm_gport_t  *existing_port = NULL;
    _BCM_GPORT_PHY_PORT_INFO *phy_port=NULL;
    int                     mc_id;

    BCMDNX_INIT_FUNC_DEFS;
    mc_id = _BCM_MULTICAST_ID_GET(group);

    rv = _bcm_dpp_mc_to_trill_get(unit,
                                  group, 
                                  &existing_port);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_TRILL,
                  (BSL_META_U(unit,
                              "MC to trill mapping was not found for MC %x and trill port (0x%x)\n"),
                   group, BCM_GPORT_TRILL_PORT_ID_GET(*existing_port)));
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The MC group was not found for trill port")));
    }


    rv = _bcm_dpp_sw_db_hash_mc_trill_find(unit,
                                          &(mc_id),
                                          (shr_htb_data_t*)(void *)&port_p,
                                           TRUE);        
    if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TRILL,
                      (BSL_META_U(unit,
                                  "error(%s) Remove MC to trill mapping %d (0x%x)\n"),
                       bcm_errmsg(rv), group, name));
            BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);

    }

    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       (existing_port), 
                                       (shr_htb_data_t*)(void *)&phy_port,
                                       FALSE);        
    if (!phy_port || BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_dpp_sw_db_hash_vlan_find failed")));
    }   
    phy_port->phy_gport = 0;
           
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_gport_update_port_state(unit, bcm_dpp_wb_gport_oper_update, existing_port, phy_port);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
    
	BCM_FREE(port_p); /* free memory */
exit:
    BCMDNX_FUNC_RETURN;
}

/* LCOV_EXCL_START */
int 
bcm_petra_trill_multicast_source_get(
    int unit, 
    bcm_trill_name_t root_name, 
    bcm_trill_name_t source_rbridge_name, 
    bcm_gport_t *port) 
{
    int   rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    
    /* For ARAD just return unavail */
    if (SOC_IS_ARAD(unit))
        rv = BCM_E_UNAVAIL;
    else /*Petra-B*/{
        rv = _bcm_pb_trill_multicast_source_get(unit,root_name,source_rbridge_name,port);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
/* LCOV_EXCL_STOP */
int 
bcm_petra_trill_multicast_source_traverse(
    int unit,
    bcm_trill_multicast_source_traverse_cb a,
    void *b)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_multicast_entry_traverse(
    int unit,
    bcm_trill_multicast_entry_traverse_cb a,
    void *b)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
int                                                                                                      
_bcm_petra_trill_port_fec_id_get (
    int unit, 
    bcm_gport_t trill_port_id, 
    int *fec_id) 
{
    int                         rv = BCM_E_NONE;

    _bcm_petra_trill_info_t    *trill_info_ptr;
    BCMDNX_INIT_FUNC_DEFS;

    *fec_id = -1;
    rv = _bcm_dpp_sw_db_hash_trill_info_find(unit,
                                       &(trill_port_id),
                                       (shr_htb_data_t*)(void *)&trill_info_ptr,
                                       FALSE);        
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Find trill port (0x%x)\n"),
                      bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id)));
    }

    *fec_id = trill_info_ptr->fec_id;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_trill_get_trill_out_ac(int unit, SOC_PPD_AC_ID *out_ac) {
   BCMDNX_INIT_FUNC_DEFS;
    *out_ac = SOC_DPP_STATE(unit)->trill_state->trill_out_ac;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
int   _bcm_petra_trill_set_trill_out_ac(int unit, SOC_PPD_AC_ID out_ac) {
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv = BCM_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */

    BCMDNX_INIT_FUNC_DEFS;
    SOC_DPP_STATE(unit)->trill_state->trill_out_ac = out_ac;
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_trill_update_out_ac_state(unit, SOC_DPP_STATE(unit)->trill_state->trill_out_ac);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_trill_port_set_local_info(
    int unit,
    bcm_trill_name_t port_name)
{
    unsigned int                        soc_sand_dev_id;
    SOC_PPD_MYMAC_TRILL_INFO            trill_info;
    uint32                              soc_sand_rv  = SOC_SAND_OK;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /*Set mymac info for local rbridge*/

    SOC_PPD_MYMAC_TRILL_INFO_clear(&trill_info);
    soc_sand_rv=soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    trill_info.my_nick_name = port_name;
       
    soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_trill_port_is_local(
    int unit,
    bcm_trill_name_t port_name,
    uint32                 *is_local)
{
    unsigned int                        soc_sand_dev_id;
    SOC_PPD_MYMAC_TRILL_INFO            trill_info;
    uint32                              soc_sand_rv  = SOC_SAND_OK;

    BCMDNX_INIT_FUNC_DEFS;
    *is_local = FALSE;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *is_local = (port_name ==  trill_info.my_nick_name);
  
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}        
int
_bcm_dpp_trill_port_reset_local_info(
    int unit,
    bcm_trill_name_t port_name)
{
    
    unsigned int                        soc_sand_dev_id;
    SOC_PPD_MYMAC_TRILL_INFO            trill_info;
    uint32                              soc_sand_rv  = SOC_SAND_OK;
    uint32                              is_local;
    int                                 rv;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_trill_port_is_local(unit, port_name, &is_local );
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_local) {
        soc_sand_dev_id = (unit);

        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        trill_info.my_nick_name = 0;
        soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}  
/* Returnes a list of MC routes for the given root_name.
   The allocated result array should be freed by the caller*/
      
int      
_bcm_dpp_trill_mc_route_get(
    int                                        unit,
    bcm_trill_name_t                           root_name,
    _bcm_petra_trill_mc_route_info_t           **route_list,
    int                                        *cnt)
{

    unsigned int                           soc_sand_dev_id;
    SOC_PPD_TRILL_MC_ROUTE_KEY             trill_mc_key;
    int                                    rv = BCM_E_NONE;
    uint32                                 mc_id;
    uint8                                  found;
    bcm_vlan_data_t                        *vlan_list; 
    int                                    vlan_cnt;
    int                                    i,j;
    _bcm_petra_trill_mc_route_info_t       *route = NULL;
    int                                    route_cnt = 0;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);
    *cnt = 0;
    *route_list = NULL;

    rv = bcm_petra_vlan_list(unit, &vlan_list, &vlan_cnt);
    BCMDNX_IF_ERR_EXIT(rv);

    if (vlan_cnt == 0) {
        BCM_EXIT;
    }
    BCMDNX_ALLOC(route, sizeof(_bcm_petra_trill_mc_route_info_t)*vlan_cnt, "_bcm_dpp_trill_mc_route_get");
    if (route == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }

    for (i = 0; i < vlan_cnt; i++) {
        SOC_PPD_TRILL_MC_ROUTE_KEY_clear(&trill_mc_key);
        trill_mc_key.tree_nick = root_name;
        trill_mc_key.fid = vlan_list[i].vlan_tag;

        for (j =0 ; j < 2; j++) {
            trill_mc_key.tts = j;
            rv = soc_ppd_frwrd_trill_multicast_route_get(soc_sand_dev_id, &trill_mc_key, &mc_id, &found);
            BCM_SAND_IF_ERR_EXIT(rv);

            if (found) {
                route[route_cnt].root_name = root_name;
                route[route_cnt].vlan = vlan_list[i].vlan_tag;
                route[route_cnt].group = mc_id;

                route_cnt++;
            }
        }
    }
    

    BCM_EXIT;
exit:
    if (route_cnt == 0) {
        BCM_FREE(route);
    } else {
        *route_list = route;
        *cnt = route_cnt;
    }

    BCMDNX_FUNC_RETURN;
}    
/*
 * Function:
 *      bcm_petra_trill_vpn_create
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_petra_trill_vpn_create(
    int unit, 
    bcm_trill_vpn_config_t *info) {
#ifdef BCM_ARAD_SUPPORT

    int                     rv = BCM_E_NONE;
    int                     update;
    uint32                  vsi_id;
    bcm_vlan_control_vlan_t control;
    _bcm_petra_trill_vpn_info_t  *vpn_hash_info = NULL/*, *vpn_hash_tmp=NULL*/;

    BCMDNX_INIT_FUNC_DEFS;

    if ( (SOC_DPP_CONFIG(unit)->trill.mode != SOC_PPD_TRILL_MODE_FGL) && ( info->low_vid != BCM_VLAN_INVALID )) {
        rv = BCM_E_DISABLED;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill FGL is not enabled (soc property): Double tagged VPN is not allowed.\n"), bcm_errmsg(rv)));
    }

    if ((SOC_DPP_CONFIG(unit)->trill.transparent_service != 0) && 
        ((info->flags & BCM_TRILL_VPN_TRANSPARENT_SERVICE) != 0) &&
        (info->low_vid != BCM_VLAN_INVALID)) {
        rv = BCM_E_DISABLED;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Trill transparent servcie is enabled: Double tagged VPN is not allowed.\n"), bcm_errmsg(rv)));
    }

    update = ((info->flags & BCM_BCM_TRILL_VPN_REPLACE) ? TRUE : FALSE);

    /* Validate VSI does not exist */
    rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, info->vpn);
    if (rv != BCM_E_EXISTS) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("error(%s) VSI not fownd (%d)\n"),
                bcm_errmsg(rv), info->vpn));
    }

    
    vsi_id = info->vpn;
    
    /* set flooding for VSI */
    bcm_vlan_control_vlan_t_init(&control);

    control.forwarding_vlan = info->vpn;

    control.broadcast_group = info->broadcast_group;
    control.unknown_multicast_group = info->unknown_multicast_group;
    control.unknown_unicast_group = info->unknown_unicast_group;

    rv = bcm_petra_vlan_control_vlan_set(unit, info->vpn, control);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!update) {
        BCMDNX_ALLOC(vpn_hash_info, sizeof(_bcm_petra_trill_vpn_info_t), "trill_vpn_hash_info");
    } else {
        rv = _bcm_dpp_sw_db_hash_trill_vpn_find(unit, 
                                            &vsi_id, 
                                            (shr_htb_data_t*)(void *)&vpn_hash_info, 
                                            TRUE);        
        if (BCM_FAILURE(rv)) {
             BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Remove TRILL vpn DB (0x%x)\n"),
                       bcm_errmsg(rv), info->vpn));
        }
    }
    
    vpn_hash_info->high_vid=info->high_vid;
    vpn_hash_info->low_vid=info->low_vid;
    vpn_hash_info->flags=info->flags;

    rv = _bcm_dpp_sw_db_hash_trill_vpn_insert(unit,
                                      &vsi_id, 
                                      vpn_hash_info);        
    if (BCM_FAILURE(rv)) {
         BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating TRILL vpn DB (0x%x)\n"),
                   bcm_errmsg(rv), info->vpn));
    }

     
exit:
     BCMDNX_FUNC_RETURN;
#else /* BCM_ARAD_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_ARAD_SUPPORT */
}

int _bcm_dpp_trill_get_vsi_info(
        int unit, 
        bcm_vlan_t vsi, 
        _bcm_petra_trill_vpn_info_t **vsi_info) {

    int                     rv = BCM_E_NONE;
    uint32                  vsi_id;
   
    BCMDNX_INIT_FUNC_DEFS;

    *vsi_info = NULL;
    vsi_id = vsi;
    rv = _bcm_dpp_sw_db_hash_trill_vpn_find(unit, &vsi_id, (shr_htb_data_t*)(void *)vsi_info, FALSE);
    BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(rv);

  
exit:


     BCMDNX_FUNC_RETURN;


}
#ifdef BCM_ARAD_SUPPORT
/*
 * Function:
 *      _bcm_dpp_trill_config_vpn
 * Purpose:
 *      Set the vpn configuration.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_config_vpn(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

     int                                 rv = BCM_E_NONE;
     _bcm_petra_trill_vpn_info_t        *hash_vpn_info = NULL;

     BCMDNX_INIT_FUNC_DEFS;

     rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &hash_vpn_info);
     BCMDNX_IF_ERR_EXIT(rv);

     if ((hash_vpn_info->flags & BCM_BCM_TRILL_VPN_ACCESS) != 0)  {
         rv = _bcm_dpp_trill_config_vpn_access(unit, vsi, port);
         BCMDNX_IF_ERR_EXIT(rv);
     }

     if ((hash_vpn_info->flags & BCM_BCM_TRILL_VPN_NETWORK) != 0)  {
         rv = _bcm_dpp_trill_config_vpn_network(unit, vsi, port);
         BCMDNX_IF_ERR_EXIT(rv);
     }
exit:
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_trill_unconfig_vpn(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

     int                                rv = BCM_E_NONE;
     _bcm_petra_trill_vpn_info_t        *hash_vpn_info = NULL;

     BCMDNX_INIT_FUNC_DEFS;

     rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &hash_vpn_info);
     BCMDNX_IF_ERR_EXIT(rv);

     if ((hash_vpn_info->flags & BCM_BCM_TRILL_VPN_ACCESS) != 0)  {
         rv = _bcm_dpp_trill_delete_vpn_access(unit, vsi, port);
        BCMDNX_IF_ERR_EXIT(rv);
     }

     if ((hash_vpn_info->flags & BCM_BCM_TRILL_VPN_NETWORK) != 0)  {
         rv = _bcm_dpp_trill_delete_vpn_network(unit, vsi, port);
         BCMDNX_IF_ERR_EXIT(rv);
     }

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      _bcm_dpp_trill_config_vpn_access
 * Purpose:
 *      Set the vpn information - Access side.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_config_vpn_access(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

    int                                 rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) { /* In advanced vlan translation mode, this change must be called in a different api. */
        rv = _bcm_dpp_trill_vlan_editing_create(unit, vsi, port);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_trill_delete_vpn_access(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

    int                                 rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) { /* In advanced vlan translation mode, this change must be called in a different api. */
        rv = _bcm_dpp_trill_vlan_editing_delete(unit, vsi, port);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_trill_config_vpn_access
 * Purpose:
 *      Set the vpn information - Access side.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_config_vpn_network(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) 
{
    int                                 rv = BCM_E_NONE;
    _bcm_petra_trill_vpn_info_t         *trill_vpn = NULL;
    SOC_SAND_SUCCESS_FAILURE            success;
    uint32                              soc_sand_rv  = SOC_SAND_OK;
    unsigned int                        soc_sand_dev_id;
    uint32                              flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);
    rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &trill_vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    if (trill_vpn->flags & BCM_TRILL_VPN_TRANSPARENT_SERVICE) {
        flags = SOC_PPD_TRILL_VSI_TRANSPARENT_SERVICE;
    }
    soc_sand_rv = soc_ppd_frwrd_trill_vsi_entry_add(soc_sand_dev_id, vsi, flags, trill_vpn->high_vid, trill_vpn->low_vid, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success);

exit:
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_trill_delete_vpn_network(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) 
{
    int                                 rv = BCM_E_NONE;
    _bcm_petra_trill_vpn_info_t         *trill_vpn = NULL;
    uint32                              soc_sand_rv  = SOC_SAND_OK;
    unsigned int                        soc_sand_dev_id;
    uint32                              flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);
    rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &trill_vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    if (trill_vpn->flags & BCM_TRILL_VPN_TRANSPARENT_SERVICE) {
        flags = SOC_PPD_TRILL_VSI_TRANSPARENT_SERVICE;
    }
    soc_sand_rv = soc_ppd_frwrd_trill_vsi_entry_remove(soc_sand_dev_id, vsi, flags, trill_vpn->high_vid, trill_vpn->low_vid);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      _bcm_dpp_trill_vlan_editing_create
 * Purpose:
 *      map VSI to outer-vid and inner-vid
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_vlan_editing_create(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

     int                                 rv = BCM_E_NONE;
     bcm_vlan_action_set_t               action;
     _bcm_petra_trill_vpn_info_t         *trill_vpn = NULL;

     BCMDNX_INIT_FUNC_DEFS;

     rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &trill_vpn);
     BCMDNX_IF_ERR_EXIT(rv);


    rv = bcm_petra_vlan_translate_action_get(unit, port, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);
    BCMDNX_IF_ERR_EXIT(rv);

     action.new_outer_vlan = trill_vpn->high_vid;
     action.outer_tpid = _BCM_PETRA_TRILL_NATIVE_TPID;
     if ((trill_vpn->flags & BCM_TRILL_VPN_TRANSPARENT_SERVICE) != 0) {
         action.inner_tpid = _BCM_PETRA_TRILL_NATIVE_TPID;
         action.ut_inner = bcmVlanActionNone;
         action.ut_inner_pkt_prio = bcmVlanActionNone;
         action.ot_outer = bcmVlanActionReplace;
         action.ot_outer_prio = bcmVlanActionReplace;
         action.ot_inner = bcmVlanActionCopy;
         action.ot_inner_pkt_prio = bcmVlanActionCopy;
         action.dt_outer = bcmVlanActionNone;
         action.dt_inner = bcmVlanActionNone;
         action.dt_outer_prio = bcmVlanActionNone;
         action.dt_inner_prio = bcmVlanActionNone;

         action.inner_tpid_action = bcmVlanTpidActionModify;
         action.outer_tpid_action = bcmVlanTpidActionModify;
     } else {
         action.ut_inner = bcmVlanActionAdd;
         action.ut_inner_pkt_prio = bcmVlanActionAdd;
         if (trill_vpn->low_vid == BCM_VLAN_INVALID) {             
             action.new_inner_vlan = 0;
             action.inner_tpid = 0;
             action.ot_outer = bcmVlanActionReplace;
             action.ot_outer_prio = bcmVlanActionReplace;
             action.ot_inner = bcmVlanActionNone;
             action.ot_inner_pkt_prio = bcmVlanActionNone;
             action.dt_outer = bcmVlanActionNone;
             action.dt_inner = bcmVlanActionNone;
             action.dt_outer_prio = bcmVlanActionNone;
             action.dt_inner_prio = bcmVlanActionNone;
             
             action.outer_tpid_action = bcmVlanTpidActionModify;             
         } else {
             action.new_inner_vlan = trill_vpn->low_vid;
             action.inner_tpid = _BCM_PETRA_TRILL_NATIVE_TPID;
             action.ot_outer = bcmVlanActionReplace;
             action.ot_outer_prio = bcmVlanActionReplace;
             action.ot_inner = bcmVlanActionAdd;
             action.ot_inner_pkt_prio = bcmVlanActionAdd;
             action.dt_outer = bcmVlanActionReplace;
             action.dt_inner = bcmVlanActionReplace;
             action.dt_outer_prio = bcmVlanActionReplace;
             action.dt_inner_prio = bcmVlanActionReplace;

             action.inner_tpid_action = bcmVlanTpidActionModify;
             action.outer_tpid_action = bcmVlanTpidActionModify;
        }
    }   
     action.it_outer = bcmVlanActionAdd;
     action.it_inner = bcmVlanActionReplace;
     action.ut_outer = bcmVlanActionAdd;     

     
     action.it_outer_pkt_prio = bcmVlanActionAdd;
     action.ut_outer_pkt_prio = bcmVlanActionAdd;

     rv = bcm_petra_vlan_translate_action_create(unit, port, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action); 
     if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
         BCMDNX_IF_ERR_EXIT(rv);
     }
     rv = BCM_E_NONE;
exit:
    BCMDNX_FUNC_RETURN;

 }
int _bcm_dpp_trill_vlan_editing_delete(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port) {

     int                                 rv = BCM_E_NONE;
     bcm_vlan_action_set_t               action;
     _bcm_petra_trill_vpn_info_t         *trill_vpn = NULL;

     BCMDNX_INIT_FUNC_DEFS;

     rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &trill_vpn);
     BCMDNX_IF_ERR_EXIT(rv);


    rv = bcm_petra_vlan_translate_action_get(unit, port, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_petra_vlan_translate_action_delete(unit, port, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;

 }

#endif /* BCM_ARAD_SUPPORT */
/*
 * Function:
 *    _bcm_dpp_trill_gport_to_fwd_decision
 * Description:
 *    convert trill gport to forwarding decision (destination + editing information)
 * Parameters:
 *  unit        -  [IN]    DPP device unit number (driver internal).
 *  gport       -  [IN]    general trill port
 *  mc_id       -  [IN]    MC_ID group
 *  fwd_decsion -  [OUT] PPD forwarding decision
 * Returns:
 *    BCM_E_XXX
 */
int _bcm_dpp_trill_gport_to_fwd_decision(int unit, bcm_gport_t gport, bcm_multicast_t mc_id, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion)
{
    uint32 soc_sand_rv;
    bcm_error_t rv;
    _BCM_GPORT_PHY_PORT_INFO        *phy_port_ptr;
    SOC_TMC_MULT_ID     mc_group_id = 0;
 
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(fwd_decsion);

    SOC_PPD_FRWRD_DECISION_INFO_clear(fwd_decsion);

    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                       &gport,
                                       (shr_htb_data_t*)(void *)&phy_port_ptr,
                                       FALSE);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trill port not  found\n")));
    }

    if (phy_port_ptr->type != _BCM_DPP_GPORT_IN_TYPE_TRILL_MC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" Trill port must be multicast\n")));
    }

    _bcm_petra_mc_id_to_ppd_val(unit, mc_id ,&mc_group_id);
    if (mc_group_id > BCM_DPP_MAX_MC_ID(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("l2mc_index is out of range")));
    }

    SOC_PPD_FRWRD_DECISION_TRILL_SET(unit, fwd_decsion, phy_port_ptr->encap_id, TRUE, mc_group_id, soc_sand_rv)
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trill_vpn_destroy(
    int unit, 
    bcm_vpn_t vpn) {


    int     rv = BCM_E_NONE;
    uint32  vsi_id;
    bcm_trill_vpn_config_t          info;
    _bcm_petra_trill_vpn_info_t     *hash_info;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_petra_trill_vpn_get(unit, vpn, &info);
    BCMDNX_IF_ERR_EXIT(rv);

    vsi_id = vpn;
    rv = _bcm_dpp_sw_db_hash_trill_vpn_find(unit, &vsi_id, (shr_htb_data_t*)(void *)&hash_info, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}


int bcm_petra_trill_vpn_destroy_all(
    int unit) {
    BCMDNX_INIT_FUNC_DEFS;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}
int bcm_petra_trill_vpn_get(
    int unit, 
    bcm_vpn_t vpn, 
    bcm_trill_vpn_config_t *info) {
#ifdef BCM_ARAD_SUPPORT 
    int                     rv = BCM_E_NONE;
    uint32                  vsi_id;
    _bcm_petra_trill_vpn_info_t  *vpn_hash_info = NULL;
    bcm_vlan_control_vlan_t      vlan_control;
    BCMDNX_INIT_FUNC_DEFS;

    vsi_id = vpn;
    bcm_trill_vpn_config_t_init(info);

    rv = _bcm_dpp_trill_get_vsi_info(unit, vpn, &vpn_hash_info);
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_vlan_control_vlan_t_init(&vlan_control);
    rv = bcm_petra_vlan_control_vlan_get(unit, vpn, &vlan_control);
    BCMDNX_IF_ERR_EXIT(rv);


    info->flags = vpn_hash_info->flags;
    info->vpn = vsi_id;
    info->high_vid = vpn_hash_info->high_vid;
    info->low_vid = vpn_hash_info->low_vid;
    info->broadcast_group = _BCM_MULTICAST_ID_GET(vlan_control.broadcast_group);
    info->unknown_unicast_group =   _BCM_MULTICAST_ID_GET(vlan_control.unknown_multicast_group);
    info->unknown_multicast_group = _BCM_MULTICAST_ID_GET(vlan_control.unknown_multicast_group);


exit:
     BCMDNX_FUNC_RETURN;
#else /* BCM_ARAD_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_ARAD_SUPPORT */


}

int bcm_petra_trill_vpn_traverse(
    int unit, 
    bcm_trill_vpn_traverse_cb cb, 
    void *user_data) {
    BCMDNX_INIT_FUNC_DEFS;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}

/* Alocate a new trill port id .                                        */
/* Trill ports may have id=name which is not allocated by this routine, */
/* So the new ID is verified to be unsued                               */

int _bcm_dpp_trill_alloc_port_id(int unit, int *id) {
    int                         last_id;
    bcm_gport_t                 trill_port_id;
    _BCM_GPORT_PHY_PORT_INFO    *phy_port_ptr;
    int                         rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;


    for (last_id=SOC_DPP_STATE(unit)->trill_state->last_used_id+1;;last_id++) {
        BCM_GPORT_TRILL_PORT_ID_SET(trill_port_id, last_id);

        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                           &trill_port_id,
                                           (shr_htb_data_t*)(void *)&phy_port_ptr,
                                           FALSE);        
        if (BCM_FAILURE(rv)) {
            break;
        }

    }


    *id = last_id;
    SOC_DPP_STATE(unit)->trill_state->last_used_id = last_id;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}

