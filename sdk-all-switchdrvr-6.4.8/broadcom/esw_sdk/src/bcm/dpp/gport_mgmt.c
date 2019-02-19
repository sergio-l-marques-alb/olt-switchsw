/*
 * $Id: gport_mgmt.c,v 1.247 Broadcom SDK $
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
 * The GPORT Management
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_PORT

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_hash_tbl.h>

#include "bcm_int/common/debug.h"

#include <sal/core/libc.h>
#include <sal/core/alloc.h>

#include <soc/drv.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/types.h>
#include <bcm/stack.h>
#include <bcm/vlan.h>
#include <bcm/mpls.h>

#include <shared/hash_tbl.h>
#include <bcm_int/common/multicast.h>

#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/mpls.h>
#include <bcm_int/dpp/mim.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/failover.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <bcm_int/dpp/pon.h>

#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_lif_ing_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPD/ppd_api_lag.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>
#include <soc/dpp/PPD/ppd_api_mymac.h>
#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>

#include <soc/dpp/trunk_sw_db.h>

/*
 * Defines
 */
 /* maximum gport used*/


/* is encap-id used in mapping to forwarding decision */

#define _BCM_DPP_GPORT_TO_FWD_DECISION_USE_ENCAP_ID(_unit,_encap_id) \
 ((_encap_id != BCM_FORWARD_ENCAP_ID_INVALID) && SOC_DPP_IS_SYS_RSRC_MGMT_ADVANCED(_unit))

#define _BCM_DPP_GPORT_FROM_FWD_DECISION_USE_ENCAP_ID(_unit)  (SOC_DPP_IS_SYS_RSRC_MGMT_ADVANCED(_unit))


#define GPORT_MGMT_ACCESS           sw_state_access[unit].dpp.bcm.gport_mgmt

#define _BCM_DPP_LIF_TO_GPORT_USE_SW_STATE  0x01
#define _BCM_DPP_LIF_TO_GPORT_INGRESS       0x02
#define _BCM_DPP_LIF_TO_GPORT_EGRESS        0x04
#define _BCM_DPP_LIF_TO_GPORT_GLOBAL_LIF    0x08
#define _BCM_DPP_LIF_TO_GPORT_LOCAL_LIF     0x10


/*
 * Functions
 */

STATIC
int _bcm_dpp_lif_to_gport_encoding(
    int                 unit,
    int                 lif_id, /* InLIF ID */
    _bcm_lif_type_e     usage, /* LIF usage */
    bcm_gport_t         *gport_id
  ) 
{

    BCMDNX_INIT_FUNC_DEFS;

    if (gport_id == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("API _bcm_dpp_lif_to_gport_encoding must recieve not null gport_id")));
    }

     /* 
     * Per usage operate accordingly 
     * add type to gport_id according to LIF TYPE 
     */
    switch (usage) {
    case _bcmDppLifTypeMplsPort:
        BCM_GPORT_MPLS_PORT_ID_SET(*gport_id,lif_id);
      break;
    case _bcmDppLifTypeL2Gre:
        BCM_GPORT_L2GRE_PORT_ID_SET(*gport_id,lif_id);
      break;
    case _bcmDppLifTypeVxlan:
        BCM_GPORT_VXLAN_PORT_ID_SET(*gport_id,lif_id);
      break;
    case _bcmDppLifTypeIpTunnel:
        BCM_GPORT_TUNNEL_ID_SET(*gport_id,lif_id);
      break;
    case _bcmDppLifTypeMplsTunnel:
        BCM_GPORT_TUNNEL_ID_SET(*gport_id,lif_id);
      break;
    case _bcmDppLifTypeVlan:
        BCM_GPORT_VLAN_PORT_ID_SET(*gport_id,lif_id);
      break;
    case _bcmDppLifTypeTrill:
        BCM_GPORT_TRILL_PORT_ID_SET(*gport_id,lif_id);
      break;
    case _bcmDppLifTypeMim:
        BCM_GPORT_MIM_PORT_ID_SET(*gport_id,lif_id);
      break;
    case _bcmDppLifTypeExtender:
        BCM_GPORT_MIM_PORT_ID_SET(*gport_id, lif_id);
        break;
    default:
        *gport_id = BCM_GPORT_INVALID;
    }   


exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Function:
 *     _bcm_dpp_out_lif_to_pon_lif
 * Purpose:
 *      Given a gport id and a global lif, returns the pon lif for this gport.
 *      If the lif is not a pon lif, it returns an error.
 *  
 *  
 * Parameters:
 *     unit                 - (IN) Device number
 *     gport_id             - (IN) Gport to be checked.
 *     global_lif_id        - (IN) Global lif for this gport.
 *     pon_out_lif_id       - (OUT) LOCAL out lif for this gport.
 *  
 * Returns: 
 *     BCM_E_NONE       - If the value was retrieved successfully..
 *     BCM_E_*          - Otherwise.
 */
STATIC int
_bcm_dpp_out_lif_to_pon_lif(int unit, int gport_id, int global_lif_id, int *pon_out_lif_id) {
    int rv;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {

        /* Get the phy port so we could tell if it's a local port. */
        rv = _bcm_dpp_gport_to_phy_port(unit, gport_id, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);


        if ((_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)) && (global_lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
            int is_pon_port, is_3_tags_data;
            uint16 tpid;
            int pcp;
            bcm_tunnel_id_t tunnel_id;

            /* check if the port is PON port */
            rv = _bcm_dpp_pon_port_is_pon_port(unit, gport_info.local_port, &is_pon_port);
            BCMDNX_IF_ERR_EXIT(rv);
            
            if (is_pon_port) {
                rv = _bcm_dpp_pon_lif_is_3_tags_data(unit, global_lif_id, &is_3_tags_data);
                BCMDNX_IF_ERR_EXIT(rv);

                if (is_3_tags_data) {
                    /* get out_ac or out_lif index from input lif index */
                    rv = _bcm_dpp_pon_encap_data_enty_get(unit, (uint32)global_lif_id, &tpid, &pcp, &tunnel_id, pon_out_lif_id);
                    BCMDNX_IF_ERR_EXIT(rv);
                    BCM_EXIT;
                }
            }
        }
    }

    /* If lif is not a pon lif, return error. */
    BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * given gport return global LIF where this gport is defined
 */
STATIC
int _bcm_dpp_gport_to_global_lif(
    int                  unit,
    bcm_gport_t          gport_id,
    int                  *in_lif_id,
    int                  *out_lif_id,
    int                  *fec_id,
    int                  *is_local
 )
{
    int rv = BCM_E_NONE;
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    SOC_SAND_PP_DESTINATION_ID dest_id;
    int encap_id;
    _bcm_dpp_gport_resolve_type_e forwarding_type;
    int tmp_lif;
    uint8 remote_lif=0;
    uint8 is_ingress = TRUE, is_egress = TRUE;
    _bcm_dpp_gport_info_t gport_info;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    int flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* port is not valid nothing to do */
    if(gport_id == BCM_GPORT_INVALID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport given is invalid when trying to map gport to lif")));
    }

    rv = _bcm_dpp_gport_parse(unit,gport_id,&gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if the purpose of the goal to get LIF ID,
       and LIF ID simply exist in the gport-id,
       THEN
       just parse the LIF-ID
       Note: currently (in order not to ruin exist code) this option supported only for IP-tunnel
       can be done also for other types.
     */
    if(BCM_GPORT_IS_TUNNEL(gport_id) && (in_lif_id != NULL || out_lif_id != NULL)) {
        if(gport_parse_info.type == _bcmDppGportParseTypeEncap) {
            if (in_lif_id != NULL) {
              if (gport_parse_info.flags & _BCM_DPP_GPORT_PARSE_EGRESS_ONLY) {
                *in_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID; /* Egrees only */
              } else {
                *in_lif_id = gport_parse_info.val;
              }
            }

            if (out_lif_id != NULL) {
               if (gport_parse_info.flags & _BCM_DPP_GPORT_PARSE_INGRESS_ONLY) {
                 *out_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID; /* Ingress only */
               } else {
                 *out_lif_id = gport_parse_info.val;
               }
			}
            /* assume local always*/
            if (is_local) {
                *is_local = 1;
            }
            BCM_EXIT;
        }
    }


    /* Determine Ingress & Egress LIF existance according to the gport encapsulation. */
    if ((BCM_GPORT_IS_VLAN_PORT(gport_id) || BCM_GPORT_IS_MPLS_PORT(gport_id)) && 
        ((BCM_GPORT_SUB_TYPE_IS_LIF(gport_id)) || (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(gport_id)))){
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_l2_intf_determine_ingress_egress(gport_id, &is_ingress, &is_egress));
    }

    /* 
     *  If the port is MPLS port EGRESS ONLY, no SW DB is used for the gport.
     */
    if(BCM_GPORT_IS_MPLS_PORT(gport_id) && (is_egress && !is_ingress)) {

        if(gport_parse_info.type == _bcmDppGportParseTypeEncap) {
            if (in_lif_id != NULL) {
              if (gport_parse_info.flags & _BCM_DPP_GPORT_PARSE_EGRESS_ONLY) {
                *in_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID; /* Egrees only */
              } else {
                *in_lif_id = gport_parse_info.val;
              }
            }

            if (out_lif_id != NULL) {
               if (gport_parse_info.flags & _BCM_DPP_GPORT_PARSE_INGRESS_ONLY) {
                 *out_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID; /* Ingress only */
               } else {
                 *out_lif_id = gport_parse_info.val;
               }
			}
            /* assume local always*/
            if (is_local) {
                *is_local = 1;
            }

            if (fec_id != NULL) {
                *fec_id = -1;
            }
            BCM_EXIT;
        }
   }

    /* 
     *  MPLS Forwarding group - don't check inlif.
     */
    if(BCM_GPORT_IS_MPLS_PORT(gport_id) && (!is_egress && !is_ingress)) {
        flags = _BCM_DPP_GPORT_RESOLVE_FLAG_NO_CHECK;
   }

    /* get ECANP-ID for this mpls-port, or I-SID for this mim_port */
    rv = _bcm_dpp_gport_resolve(unit,gport_id,flags,&dest_id,&encap_id,&forwarding_type);
    BCMDNX_IF_ERR_VERB_EXIT(rv);
    if(fec_id) {
        if ((!is_egress || !is_ingress) && !BCM_GPORT_IS_MPLS_PORT(gport_id)) {
            *fec_id = -1;
        }
        else {
            if((forwarding_type == _bcmDppGportResolveTypeFec) ||
               (forwarding_type == _bcmDppGportResolveTypeL2GreEgFec) ||
               (forwarding_type == _bcmDppGportResolveTypeVxlanEgFec)) {

                *fec_id = dest_id.dest_val;
            }
            else if(forwarding_type == _bcmDppGportResolveTypeMimMP || forwarding_type == _bcmDppGportResolveTypeMimP2P) {
                *fec_id = BCM_GPORT_MIM_PORT_ID_GET(gport_id);
            }
            else {
                *fec_id = -1;
            }
        }
    }
    
    tmp_lif = encap_id;

    /* PWE side without protection ==> encap-id = VC, get lif from gport id */
    if (forwarding_type == _bcmDppGportResolveTypeFecVC) {
        /*  get lif from gport id */
        rv = _bcm_dpp_gport_parse(unit,gport_id,&gport_parse_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if(gport_parse_info.type != _bcmDppGportParseTypeEncap) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("gport of MPLS port of type protection must be parsed as encap-type"))); /* has to be of encap-type */
        }
        tmp_lif = gport_parse_info.val;
    }

    rv = _bcm_dpp_gport_l2_intf_gport_has_remote_lif(unit,gport_id,&remote_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Trill-port special handling becuase gport_id is Virtual-SW-ID */
    if (BCM_GPORT_IS_TRILL_PORT(gport_id)) {

        unsigned int soc_sand_dev_id;
        uint32 soc_sand_rv;
        SOC_PPD_LIF_ID trill_lif_id;
        SOC_PPD_L2_LIF_TRILL_INFO  trill_info;
        uint8 is_found;
        bcm_trill_port_t trill_port;

        soc_sand_dev_id = (unit);

        /* get trill-information from sw_db key is the virtual-SW-ID */
        rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (sw_state_htb_key_t) &gport_id, (sw_state_htb_data_t) &phy_port, FALSE);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "error(%s) Searching Gport Dbase (0x%x)\n"),
                       bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(gport_id)));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Following code check type of trill port.
         * Local RBride doesn't have InLif
         */
        bcm_trill_port_t_init(&trill_port);

        trill_port.trill_port_id = gport_id;

        rv = bcm_petra_trill_port_get(soc_sand_dev_id, &trill_port);
        if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_TRILL,
                              (BSL_META_U(unit,
                                          "error(%s) Getting Trill port configuration (0x%x)\n"),
                               bcm_errmsg(rv), BCM_GPORT_TRILL_PORT_ID_GET(gport_id)));
                    BCMDNX_IF_ERR_EXIT(rv);
        }

        if(trill_port.flags & BCM_TRILL_PORT_LOCAL)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("gport trill port of type local RBridge does not have InLIF")));
        }

        /* Retrieve lookup information to retreive lif_id */
        soc_sand_rv = soc_ppd_frwrd_trill_ingress_lif_get(soc_sand_dev_id, phy_port.encap_id /* encap_id = nickname */, 
                                                            &trill_lif_id /* lif_id */,&trill_info /* dummy_val */, &is_found);  
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (!is_found) { /* No LIF for this gport */
            trill_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
        }

        if (in_lif_id != NULL) { /* In LIF is the returned value from unicast_route_get */
           *in_lif_id = trill_lif_id;
        }

        if (out_lif_id != NULL) { /* No OutLIF for Trill */
          *out_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
        }

        /* assume local always*/
        if (is_local) {
          *is_local = 1;
        }
        BCM_EXIT;
    }

    /* map gport to physical port, and check if it's local */
    if (is_local) {
        rv = _bcm_dpp_gport_to_phy_port(unit, gport_id, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);
        *is_local = _BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info) && !remote_lif;
        /* assume local always for PWE */
        if (BCM_GPORT_IS_MPLS_PORT(gport_id)) {
            *is_local = 1;
        }        
    }

    if (in_lif_id && BCM_GPORT_IS_MIM_PORT(gport_id) && !remote_lif){

        /* get lif from sw_db */
        rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (sw_state_htb_key_t) &gport_id, (sw_state_htb_data_t) &phy_port, FALSE);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "error(%s) Searching Gport Dbase (0x%x)\n"),
                       bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(gport_id)));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        if (phy_port.type != _bcmDppGportResolveTypeMimP2P) {
            /* MiM MP port does not have a lif */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MIM port can be LIF only when MIM is P2P and not MP")));
        }

        tmp_lif = phy_port.phy_gport;  
    }

    /* in_lif not required */
    if (in_lif_id) {
        *in_lif_id = (is_ingress) ? tmp_lif : _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    }

    /* out_lif_id is not required, except PON 3 TAGs manipulation */
    if (out_lif_id) {
        *out_lif_id = (is_egress) ? tmp_lif : _BCM_GPORT_ENCAP_ID_LIF_INVALID;

        /* If gport is MPLS port - Outlif can be not configured. Verify using match parameters
           We allocate outlif if one of the following is true:                                                                                                                                                                              .
           1. PWE is multipoint and not protected - than we need the Outlif for multicast traffic only.
           2. 1:1 PWE protection - FEC that points to 2 PWE-OutLIFs each OutLIF signal different VC label and MPLS tunnel.
           3. PWE is P2P and not protected - then we don't need the Outlif except BCM_MPLS_PORT_COUNTED flag is set -                                                                                                                       .
                in order to count we must have a lif.
           4. MPLS tunnel is not FEC - then PWE must be Outlif that points to the MPLS EEI. 
           Note: In case PWE is not Forward Group we don't allocate outlif
		 */

        if (BCM_GPORT_IS_MPLS_PORT(gport_id)) {
            if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(gport_id)) {
                *out_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
            }
            else {
                int local_in_lif;
                rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, tmp_lif, &local_in_lif);
                BCMDNX_IF_ERR_EXIT(rv);

                if (local_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
                    _bcm_dpp_gport_sw_resources gport_sw_resources;

                    rv = _bcm_dpp_lif_to_sw_resources(unit, local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (gport_sw_resources.in_lif_sw_resources.additional_flags & _BCM_DPP_INFLIF_MATCH_INFO_MPLS_PORT_NO_OUTLIF) {
                        /* Point-to-point */       
                        if (forwarding_type == _bcmDppGportResolveTypeFecVC) {
                            *out_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
                        }
                    }
                }
            }
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}



STATIC
int _bcm_dpp_gport_to_global_and_local_lif(int                   unit,
                                            bcm_gport_t          gport_id,
                                            int                  *global_lif,
                                            int                  *local_in_lif,
                                            int                  *local_out_lif,
                                            int                  *fec_id,
                                            int                  *is_local){
    int rv = BCM_E_NONE;
    uint8 is_ingress, is_egress;
    int global_in_lif, global_out_lif;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_IF_NOT_NULL_FILL(global_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID);
    DPP_IF_NOT_NULL_FILL(local_in_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID);
    DPP_IF_NOT_NULL_FILL(local_out_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID);

    /* First, get the global lif with gport_to_lif */
    rv = _bcm_dpp_gport_to_global_lif(unit, gport_id, (local_in_lif) ? &global_in_lif : NULL, (local_out_lif) ? &global_out_lif : NULL, fec_id, is_local);
    BCMDNX_IF_ERR_VERB_EXIT(rv);

    is_ingress = (local_in_lif) ? (global_in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) : FALSE;
    is_egress = (local_out_lif) ? (global_out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) : FALSE;

    /* Verify that the gport points to only one global lif on both ingress and egress */
    if (is_ingress && is_egress && global_in_lif != global_out_lif) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Sanity failed: given gport points to more than one global lif. ")));
    }


    if (is_ingress) {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, global_in_lif, local_in_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        DPP_IF_NOT_NULL_FILL(local_in_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID);
    }

    if (is_egress) {
        /* First, check if the lif is a pon lif */
        rv = _bcm_dpp_out_lif_to_pon_lif(unit, gport_id, global_out_lif, local_out_lif);

        /* Otherwise, get the real mapping */
        if (rv == BCM_E_NOT_FOUND) {
            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_out_lif, local_out_lif);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }else {
        DPP_IF_NOT_NULL_FILL(local_out_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID);
    }

    DPP_IF_NOT_NULL_FILL(global_lif, (is_ingress) ? global_in_lif : global_out_lif);


exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_gport_mgmt_fec_remote_lif_set(
   int unit,
   uint32 fec_id,
   uint8 is_remote)
{
	bcm_error_t rv;
	uint32 tmp = 0;
	uint32 idx = fec_id % 32;

    BCMDNX_INIT_FUNC_DEFS;

	rv = GPORT_MGMT_ACCESS.fec_remote_lif.get(unit, fec_id/32, &tmp);
	BCMDNX_IF_ERR_EXIT(rv);

	if (is_remote) {
		tmp |= (1 << idx);
	}
	else {
		tmp &= ~(1 << idx);
	}

	rv = GPORT_MGMT_ACCESS.fec_remote_lif.set(unit, fec_id/32, tmp);
	BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_gport_mgmt_fec_remote_lif_get(
   int unit,
   uint32 fec_id,
   uint8 *is_remote)
{
	int rv;
	uint32 tmp = 0;
	uint32 idx = fec_id % 32;

    BCMDNX_INIT_FUNC_DEFS;

	rv = GPORT_MGMT_ACCESS.fec_remote_lif.get(unit, fec_id/32, &tmp);
	BCMDNX_IF_ERR_EXIT(rv);

	*is_remote =  (tmp & (1 << idx)) ? 1 : 0;

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_gport_mgmt_sw_resources_fec_to_gport_set(
   int unit,
   uint32 fec_id,
   uint32 gport)
{
	bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;

	rv = GPORT_MGMT_ACCESS.bcm_dpp_sw_resources_fec_to_gport.set(unit, fec_id, gport);
	BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_gport_mgmt_sw_resources_fec_to_gport_get(
   int unit,
   uint32 fec_id,
   bcm_gport_t *gport)
{
	int rv;

    BCMDNX_INIT_FUNC_DEFS;

	rv = GPORT_MGMT_ACCESS.bcm_dpp_sw_resources_fec_to_gport.get(unit, fec_id, gport);
	BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *  htb_default_cast_key_f
 * Purpose:
 *  Default routine for casting the caller defined key to a string of bytes
 * Parameters:
 *  (in)  key        - key to cast
 *  (out) key_bytes  - string of bytes representing key
 *  (out) key_size   - length of key_bytes
 * Returns:
 *   none
 * Notes:
 *   The default cast is a no-op
 */
void
_bcm_dpp_htb_cast_key_f(shr_htb_key_t key, uint8 **key_bytes, uint32 *key_size)
{

    *key_size = 4;
    *key_bytes = key;
}

/*
 * Function:
 *      _bcm_dpp_gport_mgmt_sw_state_init
 * Purpose:
 *      allocate and Initialize SW state needed for gport management
 * Parameters:
 *      unit        - unit number
 * Note:
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_dpp_gport_mgmt_sw_state_init(int                     unit)
{
    uint8 is_allocated;
    int gport_vlan_port_db;
    int gport_trill_info_db;
    int gport_mc_trill_db;
    int trill_vpn_db;
    bcm_error_t rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if(!SOC_WARM_BOOT(unit)) {
        rv = GPORT_MGMT_ACCESS.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = GPORT_MGMT_ACCESS.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        rv = GPORT_MGMT_ACCESS.outlif_info.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = GPORT_MGMT_ACCESS.outlif_info.alloc(unit, SOC_DPP_DEFS_GET(unit, nof_out_lifs));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = GPORT_MGMT_ACCESS.inlif_info.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = GPORT_MGMT_ACCESS.inlif_info.alloc(unit, SOC_DPP_DEFS_GET(unit, nof_local_lifs));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = GPORT_MGMT_ACCESS.fec_remote_lif.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = GPORT_MGMT_ACCESS.fec_remote_lif.alloc(unit, ((SOC_DPP_CONFIG(unit)->l3.fec_db_size + 31) /32));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = GPORT_MGMT_ACCESS.bcm_dpp_sw_resources_fec_to_gport.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = GPORT_MGMT_ACCESS.bcm_dpp_sw_resources_fec_to_gport.alloc(unit, (SOC_DPP_CONFIG(unit)->l3.fec_db_size));
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* initialize gport database */
        rv = sw_state_htb_create(unit, &gport_vlan_port_db,
                       _BCM_DPP_NOF_GPORTS(unit),
                       sizeof(bcm_gport_t),
                       sizeof(_BCM_GPORT_PHY_PORT_INFO),
                       "Gport DB");
        if( BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "error(%s) allocating gport DB failed\n"),
                       bcm_errmsg(rv)));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.vlan_port_db_htb_handle.set(unit,gport_vlan_port_db));

        /* for Trill Info store  */
        rv = sw_state_htb_create(unit, &gport_trill_info_db,
                       _BCM_DPP_NOF_GPORTS(unit),
                       sizeof(bcm_gport_t),
                       sizeof(_bcm_petra_trill_info_t),
                       "Trill Info");
        if( BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "error(%s) allocating Trill Info DB failed\n"),
                                  bcm_errmsg(rv)));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.trill_info_db_htb_handle.set(unit, gport_trill_info_db));

        /* old implementation that use sw db for mapping mc-id to nickname*/
        if (SOC_DPP_CONFIG(unit)->trill.mc_id) {
            /* for MC Trill store  */
            rv = sw_state_htb_create(unit, &gport_mc_trill_db,
                           SOC_DPP_CONFIG(unit)->tm.nof_mc_ids,
                           sizeof(bcm_multicast_t),
                           sizeof(bcm_gport_t),
                           "MC Trill");
            if( BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "error(%s) allocating MC Trill DB failed\n"),
                                      bcm_errmsg(rv)));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.mc_trill_db_htb_handle.set(unit, gport_mc_trill_db));
        }

        /* For trill vpn info - Map vsi to vlan-high/vlan-low*/
        rv = sw_state_htb_create(unit, &trill_vpn_db,
                       _BCM_DPP_NUM_OF_TRILL_VSI,
                       sizeof(SOC_PPD_SYS_VSI_ID),
                       sizeof(_bcm_petra_trill_vpn_info_t),
                       "Trill vpn Info");
        if( BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "error(%s) allocating Trill VPN DB failed\n"),
                                  bcm_errmsg(rv)));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.trill_vpn_db_htb_handle.set(unit, trill_vpn_db));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_gport_mgmt_sw_state_cleanup(int                     unit)
{
    int status = 0;
    int gport_vlan_port_db;
    int gport_trill_info_db;
    int gport_mc_trill_db;
    int trill_vpn_db;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.vlan_port_db_htb_handle.get(unit, &gport_vlan_port_db));
    status = sw_state_htb_destroy(unit, gport_vlan_port_db, (sw_state_htb_data_free_f)0);
    if( BCM_FAILURE(status)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "error(%s) freeing gport DB failed\n"),
                              bcm_errmsg(status)));
    }

    BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.trill_info_db_htb_handle.get(unit, &gport_trill_info_db));
    status = sw_state_htb_destroy(unit, gport_trill_info_db, (sw_state_htb_data_free_f)0);
    if( BCM_FAILURE(status)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "error(%s) freeing trill Info DB failed\n"),
                              bcm_errmsg(status)));
    }
    /* old implementation that use sw db for mapping mc-id to nickname*/
    if (SOC_DPP_CONFIG(unit)->trill.mc_id) {
        BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.mc_trill_db_htb_handle.get(unit, &gport_mc_trill_db));
        status = sw_state_htb_destroy(unit, gport_mc_trill_db, (sw_state_htb_data_free_f)0);
        if( BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "error(%s) freeing trill MC DB failed\n"),
                                  bcm_errmsg(status)));
        }
    }

    BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.trill_vpn_db_htb_handle.get(unit, &trill_vpn_db));
    status = sw_state_htb_destroy(unit, trill_vpn_db, (sw_state_htb_data_free_f)0);
    if( BCM_FAILURE(status)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "error(%s) freeing trill Vpn DB failed\n"),
                              bcm_errmsg(status)));
    }

    /*if (lif_info->match_key) {
       BCM_FREE(lif_info->match_key);
        lif_info->match_key = NULL;
    }*/

    BCMDNX_IF_ERR_EXIT(status);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_gport_plus_1_mc_to_id(int mc_id, int is_primary, int *gport_val)
{
    BCM_GPORT_SUB_TYPE_MULTICAST_SET(*gport_val ,is_primary, _BCM_MULTICAST_ID_GET(mc_id));
    return BCM_E_NONE;
}

int 
_bcm_dpp_gport_plus_1_id_to_mc(int gport_val, int *mc_id , int *is_primary)
{
    
    *mc_id = BCM_GPORT_SUB_TYPE_MULTICAST_VAL_GET(gport_val);
    *is_primary = BCM_GPORT_SUB_TYPE_MULTICAST_PS_GET(gport_val);
    return BCM_E_NONE;
}

/*
 * SW stat access functions 
 * { 
 */

int
_bcm_dpp_sw_db_hash_vlan_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
    int rv;
    int gport_vlan_port_db;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.vlan_port_db_htb_handle.get(unit, &gport_vlan_port_db));
    rv = sw_state_htb_find(unit, gport_vlan_port_db, key, data, remove);
    if (rv == BCM_E_NOT_FOUND) {
        BCM_RETURN_VAL_EXIT(rv);
    } else {
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_vlan_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int gport_vlan_port_db;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.vlan_port_db_htb_handle.get(unit, &gport_vlan_port_db));
    BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit, gport_vlan_port_db, key, data));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_lif_sw_resources_vsi_get
 * Purpose:
 *      return VSI Inlif is associated to.
 *      0 if InLIF not associated with VSI
 * Parameters:
 *     unit    - (IN) Device Number
*      in_lif  - (IN) InLIF id 
*      vsi -     (OUT) vsi value
 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_lif_sw_resources_vsi_get(int unit, int in_lif, int *vsi)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_to_sw_resources(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));
    *vsi = gport_sw_resources.in_lif_sw_resources.vsi;
    

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;}

/*
 * Function:
 *      _bcm_dpp_lif_sw_resources_vsi_set
 * Purpose:
 *      return VSI Inlif is associated to.
 *      0 if not.
 * Parameters:
 *     unit    - (IN) Device Number
 *     in_lif  - (IN) InLIF id 
 *     vsi     - (IN) vsi value
 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_lif_sw_resources_vsi_set(int unit, int in_lif, int vsi)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_lif_to_sw_resources(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.in_lif_sw_resources.vsi = vsi;

    rv = _bcm_dpp_lif_sw_resources_set(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_lif_sw_resources_learn_gport_set(int unit, int in_lif, int learn_gport){
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_lif_to_sw_resources(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.in_lif_sw_resources.learn_gport_id = learn_gport;

    rv = _bcm_dpp_lif_sw_resources_set(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_lif_sw_resources_learn_gport_get(int unit, int in_lif, int * learn_gport) {
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_lif_to_sw_resources(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    *learn_gport = gport_sw_resources.in_lif_sw_resources.learn_gport_id;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_lif_sw_resources_lif_usage_get
 * Purpose:
 *      Retrieve the usage type of a local LIF. For InLIF, the value is retrieved
 *      from the Ingress SW DB. For OutLIF (intended for Egress only LIFs),
 *      the value is retrieved from the alloc_manager and the type is always
 *      set to vlan.
 * Parameters:
 *      unit    - (IN) Device Number
 *      in_lif  - (IN) InLIF ID or _BCM_GPORT_ENCAP_ID_LIF_INVALID
 *      out_lif - (IN) OutLIF ID or _BCM_GPORT_ENCAP_ID_LIF_INVALID
 *      in_lif_usage -  (OUT) Returned InLIF type from the Ingress SW DB
 *      out_lif_usage - (OUT) Returned OutLIF type according to the outlif
 *                      alloc manager. _bcmDppLifTypeVlan or _bcmDppLifTypeAny.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_lif_sw_resources_lif_usage_get(int unit, int in_lif, int out_lif, _bcm_lif_type_e *in_lif_usage, _bcm_lif_type_e *out_lif_usage)
{ 
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    uint32 flags=0;
    
    BCMDNX_INIT_FUNC_DEFS;

    if (in_lif_usage && (in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
        flags |= _BCM_DPP_GPORT_SW_RESOURCES_INGRESS;
    }
    if (out_lif_usage  && (out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
        flags |= _BCM_DPP_GPORT_SW_RESOURCES_EGRESS;
    } 
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_to_sw_resources(unit, in_lif, out_lif, flags, &gport_sw_resources));

    if (in_lif_usage && (in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
        *in_lif_usage = gport_sw_resources.in_lif_sw_resources.lif_type;
    }
    else if (in_lif_usage) {
        *in_lif_usage = _bcmDppLifTypeAny;
    }
    if (out_lif_usage && (out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
        *out_lif_usage = gport_sw_resources.out_lif_sw_resources.lif_type;
    } 
    else if (out_lif_usage) {
        *out_lif_usage = _bcmDppLifTypeAny;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_lif_sw_resources_lif_usage_set
 * Purpose:
 *      Set the usage type of an InLIF in the Ingress SW DB.
 * Parameters:
 *      unit    - (IN) Device Number
 *      in_lif  - (IN) InLIF ID or _BCM_GPORT_ENCAP_ID_LIF_INVALID
 *      out_lif - (IN) OutLIF ID or _BCM_GPORT_ENCAP_ID_LIF_INVALID
 *      in_lif_usage -  (IN) sage type for the InLIF
 *      out_lif_usage - (IN) sage type for the OutLIF
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_lif_sw_resources_lif_usage_set(int unit, int in_lif, int out_lif, _bcm_lif_type_e in_lif_usage, _bcm_lif_type_e out_lif_usage)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    uint32 flags=0;
    
    BCMDNX_INIT_FUNC_DEFS;

    if (in_lif_usage && (in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
        flags |= _BCM_DPP_GPORT_SW_RESOURCES_INGRESS;
    }
    if (out_lif_usage && (out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
        flags |= _BCM_DPP_GPORT_SW_RESOURCES_EGRESS;
    } 

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_to_sw_resources(unit, in_lif, out_lif, flags, &gport_sw_resources));

    if (in_lif_usage) {
        gport_sw_resources.in_lif_sw_resources.lif_type = in_lif_usage;
    }
    if (out_lif_usage) {
        gport_sw_resources.out_lif_sw_resources.lif_type = out_lif_usage;
    } 

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_sw_resources_set(unit, in_lif, out_lif, flags, &gport_sw_resources));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * SW stat access functions 
 * } 
 */



/*
 * Function:
 *      _bcm_dpp_gport_mgmt_init
 * Purpose:
 *      init Gport module including
 *        - SW state init
 *        - define [port] profiles for AC lookup: for each tag-format whether to lookup PV or PVV
 *        - define [in/out AC] profiles + commands for ing/eg vlan editing: for each tag-format what vlans to remove/push
 * Parameters:
 * Note:
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_dpp_gport_mgmt_init(int                     unit)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_gport_mgmt_sw_state_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* drop trap code */

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *  _bcm_dpp_gport_local_to_global_gport
 * Description:
 *    given gport convert it to global represintation, for example,
 *    if gport IS_LOCAL then map it to mod-port
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *  gport -   [IN] gport to map
 *  global_gport -     [OUT] global gport represetation
 * Returns:
 *    BCM_E_XXX
 */
int 
 _bcm_dpp_gport_local_to_global_gport(int unit, bcm_port_t gport, bcm_port_t *global_gport)
{

    bcm_module_t    _mymodid = 0;
    uint32 local_port;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(global_gport);

    if(BCM_GPORT_IS_LOCAL(gport)){ /* local-port*/

        rv = bcm_petra_stk_my_modid_get(unit, &_mymodid);
        BCMDNX_IF_ERR_EXIT(rv);
        local_port = BCM_GPORT_LOCAL_GET(gport);
        BCM_GPORT_MODPORT_SET(*global_gport,_mymodid,local_port);
    }
    else
    {/* it's already system/global configuration*/
        *global_gport = gport;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    _bcm_dpp_gport_to_phy_port
 * Description:
 *    map gport to PPD local and system port
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *  gport -   [IN] general port
 *  local_only -     [IN] get only local port
 *  pbmp_local_ports -     [OUT] bitmap of local ports
 *  sys_port -     [OUT] system port (returns if local_only is not set)
 *  is_local_ports -  [OUT] is gport contains local ports
 *  is_lag -     [OUT] is port is physical port or trunk or invalid.
 * Returns:
 *    BCM_E_XXX
 *    BCM_E_PORT if given gport is not physical port (but logical port)
 */
int 
_bcm_dpp_gport_to_phy_port(int unit, bcm_gport_t port, uint32  operations, _bcm_dpp_gport_info_t* gport_info)
{
    bcm_module_t        modid = 0, my_modid = 0;
    bcm_port_t          mod_port = 0;
    uint32              local_port = 0;
    SOC_PPD_LAG_INFO    *lag_info = NULL;
    int                 trunk_id,member_idx;
    bcm_gport_t         gport;
    int                 rv = BCM_E_NONE;
    uint32              soc_sand_rv, tm_port;
    bcm_gport_t         inter_gport = 0;
    uint8               pon_app_enable;
    int                 found_local_port, queue_id;
    _bcm_dpp_gport_sw_resources gport_sw_resources;

    _BCM_GPORT_PHY_PORT_INFO        phy_port;
    SOC_TMC_IPQ_QUARTET_MAP_INFO    queue_map_info;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO    fec_entry[2];
    SOC_PPD_FRWRD_FEC_PROTECT_INFO  protect_info;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE  protect_type;

    int core = SOC_CORE_ALL;
    BCMDNX_INIT_FUNC_DEFS;

    /* Verify parameters */
    BCMDNX_NULL_CHECK(gport_info);
    gport_info->sys_port = SOC_TMC_SYS_PHYS_PORT_INVALID;
    gport_info->flags = 0;
    gport_info->lane = -1;
    gport_info->phyn = -1;
    gport_info->local_port = SOC_MAX_NUM_PORTS;
    BCM_PBMP_CLEAR(gport_info->pbmp_local_ports);

    pon_app_enable = SOC_DPP_CONFIG(unit)->pp.pon_application_enable;    

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else if (pon_app_enable && (_BCM_PPD_IS_PON_PP_PORT(port))) { 
        BCM_GPORT_LOCAL_SET(gport,port);
    } else if (SOC_PORT_VALID(unit, port)) {
        BCM_GPORT_LOCAL_SET(gport,port);
    } else if (port == BCM_GPORT_INVALID) {        
        BCM_EXIT; /* Nothing to do just exit ... */
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Local port %d not valid"), port)); 
    }

    rv = bcm_petra_stk_my_modid_get(unit, &my_modid);
    BCMDNX_IF_ERR_EXIT(rv);
    
    if(BCM_GPORT_IS_LOCAL(gport)){ /* local-port*/

        gport_info->local_port = BCM_GPORT_LOCAL_GET(gport);
        BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports,gport_info->local_port);
        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;
        
        /* get physical system port, identify <mod,port>*/
        if(operations & _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT){

            rv = soc_port_sw_db_local_to_tm_port_get(unit, gport_info->local_port, &tm_port, &core);
            BCMDNX_IF_ERR_EXIT(rv);

            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_local_to_sys_phys_port_map_get,(unit, my_modid + core, tm_port, &(gport_info->sys_port))));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if(gport_info->sys_port == SOC_TMC_SYS_PHYS_PORT_INVALID) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("System port value is invalid port")));
            }
        }
    }
    else if(BCM_GPORT_IS_MODPORT(gport)){ /* mod-port*/
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        mod_port = BCM_GPORT_MODPORT_PORT_GET(gport); 

        if ((modid >= my_modid) && (modid < (my_modid + SOC_DPP_DEFS_GET(unit, nof_cores)))) {
            gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;

            core = modid - my_modid;
            SOC_DPP_CORE_VALIDATE(unit, core, FALSE);
            rv = soc_port_sw_db_tm_to_local_port_get(unit, core, mod_port, (soc_port_t *)&local_port);
            BCMDNX_IF_ERR_EXIT(rv);

            BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, local_port);
            gport_info->local_port = local_port;

        } 

        /* get physical system port, identify <mod,port>*/
        if(operations & _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT){
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_local_to_sys_phys_port_map_get,(unit, modid, mod_port, &(gport_info->sys_port))));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if(gport_info->sys_port == SOC_TMC_SYS_PHYS_PORT_INVALID) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("System port value is invalid port")));
            }
        }
    }
    else if(BCM_GPORT_IS_DEVPORT(gport)){  /* Device Port */
        gport_info->local_port = BCM_GPORT_DEVPORT_PORT_GET(gport);
        BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, gport_info->local_port);
        if (unit == BCM_GPORT_DEVPORT_DEVID_GET(gport)) {
            gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;
        }
    }
    else if(BCM_GPORT_IS_TRUNK(gport)){  /*trunk*/
        trunk_id = BCM_GPORT_TRUNK_GET(gport);
        /* map lag to system port*/
        if (trunk_id > BCM_DPP_MAX_TRUNK_ID(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Trunk id is higher than the maximal value")));
        }
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_logical_sys_id_build_with_device,(unit, TRUE, trunk_id, 0, 0,&(gport_info->sys_port))));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LAG;

        /* read lag, and check it has local members*/
        BCMDNX_ALLOC(lag_info, sizeof(SOC_PPD_LAG_INFO), "_bcm_dpp_gport_to_phy_port.lag_info");
        if (lag_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        soc_sand_rv = soc_dpp_trunk_sw_db_get(unit, trunk_id, lag_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        found_local_port = 0;
        for(member_idx = 0; member_idx < lag_info->nof_entries; ++member_idx) {
            /* map sys port to fap and tm-port*/
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit,(lag_info->members[member_idx].sys_port), (uint32*)&modid, (uint32*)&mod_port)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, modid, my_modid)){

                gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;

                core = modid - my_modid;
                SOC_DPP_CORE_VALIDATE(unit, core, FALSE);
                rv = soc_port_sw_db_tm_to_local_port_get(unit, core, mod_port, (soc_port_t *)&local_port);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Add local port to local ports bitmap */
                BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, local_port);
                if(!found_local_port) {
                    gport_info->local_port = local_port;
                    found_local_port = 1;
                }
            }         
        }
    }
    else if(BCM_GPORT_IS_SYSTEM_PORT(gport)){  /*system port*/ 
        gport_info->sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);

        /* map sys port to fap and tm-port*/
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit,gport_info->sys_port, (uint32*)&modid, (uint32 *)&mod_port)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCM_DPP_UNIT_CHECK(unit);
        if (SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, modid, my_modid)){
            gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;

            core = SOC_DPP_MODID_TO_CORE(unit, my_modid, modid);
            rv = soc_port_sw_db_tm_to_local_port_get(unit, core, mod_port, (soc_port_t *)&local_port);
            BCMDNX_IF_ERR_EXIT(rv);
            gport_info->local_port = local_port;
            BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, local_port);

        }
    }
    else if(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)){  /*unicast queue */ 
        
        queue_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);

        /* map flow to sys port */               
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_queue_to_flow_mapping_get,(unit, core, (queue_id/4), &queue_map_info)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        gport_info->sys_port = queue_map_info.system_physical_port;

        /* map sys port to fap and tm-port*/
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit, gport_info->sys_port, (uint32*)&modid, (uint32 *)&mod_port)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        if (SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, modid, my_modid)){
            gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;

            core = SOC_DPP_MODID_TO_CORE(unit, my_modid,  modid);
            rv = soc_port_sw_db_tm_to_local_port_get(unit, core, mod_port, (soc_port_t *)&local_port);
            BCMDNX_IF_ERR_EXIT(rv);
            gport_info->local_port = local_port;
            BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, local_port);

        }
    }
    else if(BCM_GPORT_IS_LOCAL_CPU(gport)){ /* CPU-port*/
 
        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;
        gport_info->local_port = CMIC_PORT(unit);
        BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, gport_info->local_port);

        /* get physical system port, identify <mod,port>*/
        if(operations & _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT){
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_local_to_sys_phys_port_map_get,(unit, my_modid, gport_info->local_port, &(gport_info->sys_port))));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if(gport_info->sys_port == SOC_TMC_SYS_PHYS_PORT_INVALID) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("System port value is invalid port")));
            }
        }

    } 
    else if(BCM_PHY_GPORT_IS_PHYN(port)) {
        gport_info->local_port = BCM_PHY_GPORT_PHYN_PORT_PORT_GET(port); 
        gport_info->phyn = BCM_PHY_GPORT_PHYN_PORT_PHYN_GET(port); 
        BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, gport_info->local_port);
        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;
    } 
    else if(BCM_PHY_GPORT_IS_PHYN_LANE(port)) {
        gport_info->local_port = BCM_PHY_GPORT_PHYN_LANE_PORT_PORT_GET(port); 
        gport_info->phyn = BCM_PHY_GPORT_PHYN_LANE_PORT_PHYN_GET(port); 
        gport_info->lane = BCM_PHY_GPORT_PHYN_LANE_PORT_LANE_GET(port); 
        BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, gport_info->local_port);
        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;
    } 
    else if(BCM_PHY_GPORT_IS_PHYN_SYS_SIDE(port)) {
        gport_info->local_port = BCM_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_GET(port); 
        gport_info->phyn = BCM_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_GET(port); 
        BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, gport_info->local_port);
        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;
        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_SYS_SIDE;
    } 
    else if(BCM_PHY_GPORT_IS_PHYN_SYS_SIDE_LANE(port)) {
        gport_info->local_port = BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_GET(port); 
        gport_info->phyn = BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_GET(port); 
        gport_info->lane = BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_GET(port); 
        BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, gport_info->local_port);
        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;
        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_SYS_SIDE;
    }
    else if(BCM_PHY_GPORT_IS_LANE(gport)) {
         gport_info->local_port = BCM_PHY_GPORT_LANE_PORT_PORT_GET(port);
         /* won't be shiffted by 32 bits */
         /* coverity[negative_shift:FALSE] */
         BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, gport_info->local_port);
         gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;
    } else if(BCM_GPORT_IS_LOCAL_FABRIC(gport)) {
        gport_info->local_port = BCM_GPORT_LOCAL_FABRIC_GET(gport) + FABRIC_LOGICAL_PORT_BASE(unit);
        BCM_PBMP_PORT_ADD(gport_info->pbmp_local_ports, gport_info->local_port);
        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT;
    }
    else if (BCM_GPORT_IS_BLACK_HOLE(gport)) {
       gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_BLACK_HOLE; 
    }
    else if (BCM_GPORT_IS_TRAP(gport)) {
        bcm_rx_trap_config_t trap_config;

        bcm_rx_trap_config_t_init(&trap_config);

        rv = bcm_petra_rx_trap_get(unit, BCM_GPORT_TRAP_GET_ID(port), &trap_config);
        BCMDNX_IF_ERR_EXIT(rv);

        if (trap_config.flags & BCM_RX_TRAP_UPDATE_DEST) {
            rv = _bcm_dpp_gport_to_phy_port(unit, trap_config.dest_port, 0, gport_info);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    /* try to resolve logical gport to phy-port */
    else {
        _bcm_dpp_gport_parse_info_t gport_parse_info; 
        int local_in_lif;               
        
        /* map gport to FEC/port */
        rv = _bcm_dpp_gport_parse(unit, gport, &gport_parse_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* if FEC, get physical port from the FEC */
        if (gport_parse_info.type == _bcmDppGportParseTypeProtectedFec || gport_parse_info.type == _bcmDppGportParseTypeForwardFec) {
            if (gport_parse_info.val < SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved) {
                
                gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT; 
                BCM_EXIT;
            }

            /* get dest from FEC*/
            soc_sand_rv = soc_ppd_frwrd_fec_entry_get(unit,gport_parse_info.val,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* map FEC dest to gport */
            soc_sand_rv = _bcm_dpp_gport_from_sand_pp_dest(unit,&inter_gport,&fec_entry[0].dest);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (fec_entry[0].dest.dest_type == SOC_SAND_PP_DEST_TYPE_DROP) {
                gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_BLACK_HOLE; 
            }
            else {
                /* map gport to required params */
                rv = _bcm_dpp_gport_to_phy_port(unit,inter_gport,operations,gport_info);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }        
        else if (gport_parse_info.type == _bcmDppGportParseTypeSimple) { /* simple gport*/
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unexpected error, gport must be resolved already by utility checks above"))); /* cannot be this case, should be resolved in physical ports above*/
        }
        else {  /* rest of cases: get stored information */

            /* Check if it's an ingress_only gport, in which case the retrieved data
               will be from the Ingress SW DB */
            if (BCM_GPORT_IS_VLAN_PORT(gport) && (gport_parse_info.type == _bcmDppGportParseTypeEncap) && (gport_parse_info.flags & _BCM_DPP_GPORT_PARSE_INGRESS_ONLY)) {
                rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, gport_parse_info.val, &local_in_lif);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Retrieve Ingress SW DB info according to the gport parsing*/
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_to_sw_resources(unit, local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

                /* Call for phy port resolution with Ingress stored gport */
                rv = _bcm_dpp_gport_to_phy_port(unit, gport_sw_resources.in_lif_sw_resources.port, operations, gport_info);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (BCM_GPORT_IS_MPLS_PORT(gport) && (gport_parse_info.type == _bcmDppGportParseTypeEncap)) {
              /* 
               * MPLS Port Encap. In case of egress only, no port
               *  and then we just return it is local.
               *  In case of ingress only or ingress+egress we take the information from the inlif sw db
               */
              if (!(gport_parse_info.flags & _BCM_DPP_GPORT_PARSE_EGRESS_ONLY)) {
                  /* Retrieve Ingress SW DB info according to the gport parsing*/
                  rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, gport_parse_info.val, &local_in_lif);
                  BCMDNX_IF_ERR_EXIT(rv);

                  BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_to_sw_resources(unit, local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

                  /* Call for phy port resolution with Ingress stored gport */
                  rv = _bcm_dpp_gport_to_phy_port(unit, gport_sw_resources.in_lif_sw_resources.port, operations, gport_info);
                  BCMDNX_IF_ERR_EXIT(rv);
              } 
            } else {
                /*LIF encap is stored in SW*/
                rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                                   (sw_state_htb_key_t) (&gport), 
                                                   (sw_state_htb_data_t) &phy_port,
                                                   FALSE);
                if (rv == BCM_E_NOT_FOUND) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Entry not found")));
                }

                if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Entry found but data is missed")));
                }

                /* For FEC VC and Ring Protection, the physical port is retieved via
                   the FEC destination */
                if ((phy_port.type == _bcmDppGportResolveTypeFecVC) || (phy_port.type == _bcmDppGportResolveTypeRing) || (phy_port.type == _bcmDppGportResolveTypeFecEEP) ||
                    (SOC_IS_JERICHO(unit) && (phy_port.type == _bcmDppGportResolveTypeFec))) {
                    if (phy_port.phy_gport < SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved) {
                        
                        gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT; 
                        BCM_EXIT;
                    }
                    else {
                        /* get dest from FEC*/
                        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(unit,phy_port.phy_gport,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                        /* map FEC dest to gport */
                        soc_sand_rv = _bcm_dpp_gport_from_sand_pp_dest(unit,&inter_gport,&fec_entry[0].dest);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                } else {
                    /* call for inner gport <it has to be physical gport > */
                    /* map gport to required params */
                    inter_gport = phy_port.phy_gport;
                }
                rv = _bcm_dpp_gport_to_phy_port(unit, inter_gport, operations, gport_info);
                BCMDNX_IF_ERR_EXIT(rv);
            }            
        }

        if (BCM_GPORT_IS_MPLS_PORT(gport)) { /* Always local */
            gport_info->flags |= _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT; 
        }
    }

    if(operations & _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY) {
        if (!(gport_info->flags & _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("given gport: 0x%x that is not resolved to local port"), gport)); 
        }

        BCM_PBMP_ITER(gport_info->pbmp_local_ports, local_port) {
            if (BCM_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, all), local_port)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_DISABLED, (_BSL_BCM_MSG("port %d is disabled"), local_port));
            }
        }
    }
  
exit:
    BCM_FREE(lag_info);
    BCMDNX_FUNC_RETURN;
}

/* given gport, return if it's local in this unit */
int 
_bcm_dpp_gport_is_local(int unit, bcm_gport_t port,  int *is_local)
{
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    if(BCM_GPORT_IS_TUNNEL(port)) {
      *is_local = 1;
    } else {
      BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));
      (*is_local) = _BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info);
    }

exit:
    BCMDNX_FUNC_RETURN;

}

/* 
 *  See .h for more information
 */
int _bcm_dpp_gport_parse(int unit, bcm_gport_t port, _bcm_dpp_gport_parse_info_t *gport_parse_info)
{
    bcm_gport_t gport;
    uint8 pon_app_enable;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    pon_app_enable = SOC_DPP_CONFIG(unit)->pp.pon_application_enable;

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else if (pon_app_enable && (_BCM_PPD_IS_PON_PP_PORT(port))) {
        BCM_GPORT_LOCAL_SET(gport,port);
    } else if (SOC_PORT_VALID(unit, port)) { 
        BCM_GPORT_LOCAL_SET(gport,port);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("gport given is invalid port"))); 
    }
    
    gport_parse_info->flags = 0;
    gport_parse_info->val = 0;
    gport_parse_info->type = 0;

    /* is valid gport at all*/
    if (BCM_GPORT_IS_SET(gport)) {
        /* if fail, then try to get information from gport-id */
        gport_parse_info->type = _bcmDppGportParseTypeProtectedFec;
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            gport_parse_info->val = BCM_GPORT_VLAN_PORT_ID_GET(gport);
        } 
        else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
            gport_parse_info->val = BCM_GPORT_MPLS_PORT_ID_GET(gport);
        }
        else if (BCM_GPORT_IS_MIM_PORT(gport)) {
            gport_parse_info->val = BCM_GPORT_MIM_PORT_ID_GET(gport);
        }
        else if (BCM_GPORT_IS_TRILL_PORT(gport)) {
            gport_parse_info->val = BCM_GPORT_TRILL_PORT_ID_GET(gport);
        }
        else if (BCM_GPORT_IS_TUNNEL(gport)) { /* for IP-tunnel ID, is in/out-LIF ID */
            gport_parse_info->val = BCM_GPORT_TUNNEL_ID_GET(gport);            
            if(BCM_GPORT_SUB_TYPE_IS_MULTICAST(gport)) {
                gport_parse_info->type = _bcmDppGportParseTypeMC;
            }
            else{
                gport_parse_info->type = _bcmDppGportParseTypeEncap;
            }
        }
        else if (BCM_GPORT_IS_L2GRE_PORT(gport)) { /* for IP-tunnel ID, is in/out-LIF ID */
            gport_parse_info->val = BCM_GPORT_L2GRE_PORT_ID_GET(gport);            
            gport_parse_info->type = _bcmDppGportParseTypeEncap;  
        }
        else if (BCM_GPORT_IS_VXLAN_PORT(gport)) { /* for IP-tunnel ID, is in/out-LIF ID */
            gport_parse_info->val = BCM_GPORT_VXLAN_PORT_ID_GET(gport);            
            gport_parse_info->type = _bcmDppGportParseTypeEncap;
        }
        else if (BCM_GPORT_IS_FORWARD_PORT(gport)) {
            gport_parse_info->val  = BCM_GPORT_FORWARD_PORT_GET(gport);
            gport_parse_info->type = _bcmDppGportParseTypeForwardFec;
        }
        else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
            gport_parse_info->val  = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
        }
        else {
            
            gport_parse_info->type = _bcmDppGportParseTypeSimple;
            gport_parse_info->val = gport;
        }

        /*set the fields of gport_parse_info*/

        /* it can be MC */
        if(BCM_GPORT_SUB_TYPE_IS_MULTICAST(gport)) {
            gport_parse_info->type  = _bcmDppGportParseTypeMC;
            gport_parse_info->flags |= BCM_GPORT_SUB_TYPE_MULTICAST_PS_GET(gport_parse_info->val)==BCM_GPORT_SUB_TYPE_MULTICAST_PS_PRIMARY ? _BCM_DPP_GPORT_PARSE_PRIMARY:_BCM_DPP_GPORT_PARSE_SECONDARY;
            gport_parse_info->val   = BCM_GPORT_SUB_TYPE_MULTICAST_VAL_GET(gport_parse_info->val);

        } else if(BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(gport)) {
            /* it can be forward */
            gport_parse_info->type  = _bcmDppGportParseTypeForwardFec;
            gport_parse_info->val   = BCM_GPORT_SUB_TYPE_FORWARD_GROUP_GET(gport_parse_info->val);

        } else if(BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(gport)) {
            gport_parse_info->type  = _bcmDppGportParseTypePushProfile;
            gport_parse_info->val   = BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_GET(gport_parse_info->val);

        } else if(BCM_GPORT_SUB_TYPE_IS_LIF(gport)){
            gport_parse_info->type = _bcmDppGportParseTypeEncap;
            if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport_parse_info->val) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY)) {
                gport_parse_info->flags |= _BCM_DPP_GPORT_PARSE_INGRESS_ONLY;
            }
            if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport_parse_info->val) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY)) {
                gport_parse_info->flags |= _BCM_DPP_GPORT_PARSE_EGRESS_ONLY;
            }
            gport_parse_info->val = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(gport_parse_info->val);
        }

    } 
    else
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Invalid port")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}



/* 
 *  given gport return if it's protected or not
 * 
 *  derived from gport-id format, if it's vlan_port/mpls_port
 *  that include FEC then it's protected otherwise gport is not protected
 */
int _bcm_dpp_gport_is_protected(int unit, bcm_gport_t gport, int  *is_protected)
{
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    *is_protected = FALSE;

    rv = _bcm_dpp_gport_parse(unit,gport,&gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

    if (gport_parse_info.type == _bcmDppGportParseTypeProtectedFec ||
        gport_parse_info.type == _bcmDppGportParseTypeForwardFec) {/*FEC encap is stored in SW*/        
        *is_protected = TRUE;
    }
   
exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 *  Returns for a given gport, the used lif type 
 *  Turns the gport to lif, before fetching the type from the DB.
 */
int _bcm_dpp_lif_sw_resources_gport_usage_get(int unit,
                                 bcm_gport_t gport,
                                 _bcm_lif_type_e *used_type)
{
    int rv = BCM_E_NONE, global_lif_id, local_in_lif_id=_BCM_GPORT_ENCAP_ID_LIF_INVALID, local_out_lif_id=_BCM_GPORT_ENCAP_ID_LIF_INVALID, fec_id, is_local;
    _bcm_lif_type_e in_lif_usage_type, out_lif_usage_type;

    BCMDNX_INIT_FUNC_DEFS;

    /* Initiate the response for error exists */
    *used_type = _bcmDppLifTypeAny;

    /* Convert the gport to lif in order to access the lif sw db in case of in_lif or
       the alloc manager state in case the it's an out_lif */
    rv = _bcm_dpp_gport_to_global_and_local_lif(unit, gport, &global_lif_id, &local_in_lif_id, &local_out_lif_id, &fec_id, &is_local);
    if (rv != BCM_E_NOT_FOUND) {
        BCMDNX_IF_ERR_EXIT(rv);
    }


    /* In case of FEC, don't evaluate the LIFs, except for a local Out-LIF that can be evaluated. */
    if ((rv == BCM_E_NOT_FOUND) || (fec_id != -1)) {
        local_in_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;

        if ((rv == BCM_E_NOT_FOUND) && (!is_local)) {
            local_out_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
        }
    }

    if (local_out_lif_id == 0) {
        local_out_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    }

    if ((local_out_lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID) || (local_in_lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
        /* Get the usage type for the LIFs */
        rv = _bcm_dpp_lif_sw_resources_lif_usage_get(unit, local_in_lif_id, local_out_lif_id, &in_lif_usage_type, &out_lif_usage_type);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if ((local_in_lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID) && (local_out_lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
        /* Both InLIF and OutLIF are defined - make sure they are the same */
        if (in_lif_usage_type == out_lif_usage_type) {
            /* OK - return value */
            *used_type = in_lif_usage_type;
        }
        else { /* Error - InLIF type defined differently than OutLIF type */
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("InLIF and OutLIF configured differently")));
        }
    }
    else
    {
        /* Only one outlif is valid - return it's usage */
        *used_type = (local_in_lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? in_lif_usage_type : 
            (local_out_lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? out_lif_usage_type : _bcmDppLifTypeAny;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 *  Resolve the forwarding information.
 *  Given gport return destination (FEC/phy-port) + encap-id + type
 *  Input: gport parameter.
 *  Output:
 *    Dune Destination (FEC/Phy-port)
 *    Encap-ID (Additional information such as EEI/OutLIF)
 *    Gport_type (Gport type may indicate Encap-ID information, _bcm_dpp_gport_resolve_type_e enum)
 *  
 *  Logic:
 *    if gport is FEC then read from FEC
 *    otherwise read from SW state (hash table)
 */
int _bcm_dpp_gport_resolve(int unit, bcm_gport_t gport, int flags, SOC_SAND_PP_DESTINATION_ID *dest_id, int *encap_id, _bcm_dpp_gport_resolve_type_e *gport_type)
{
    int global_lif_id = 0, local_lif_id = 0;
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    SOC_PPD_FRWRD_DECISION_INFO  dummy_fwd_decsion;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO
        fec_entry[2];
    SOC_PPD_FRWRD_FEC_PROTECT_INFO
        protect_info;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE              
        protect_type;
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    bcm_mim_port_t mim_port;    
    int lif;
    int is_ingress_only = FALSE, is_egress_only = FALSE;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    uint8 lif_used_for_forwarding = 1;
    _bcm_lif_type_e lif_usage;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    rv = _bcm_dpp_gport_parse(unit,gport,&gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

    *encap_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;

    if (gport == BCM_GPORT_TYPE_NONE) {
        *gport_type = _bcmDppGportResolveTypeInvalid;
        BCM_EXIT;
    }
    /* 1+1 protection: in case MC gport */
    if (gport_parse_info.type == _bcmDppGportParseTypeMC ) { /* gport is MC, for 1-plus-1 */
        *gport_type = _bcmDppGportResolveTypeMC;
        /* multicast destination can be resolved from ID */        
        dest_id->dest_type = SOC_SAND_PP_DEST_MULTICAST;
        dest_id->dest_val = gport_parse_info.val;
        /* encap id i.e. outlif get from hash table*/
        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                           (sw_state_htb_key_t) (&gport),
                                           (sw_state_htb_data_t) &phy_port,
                                           FALSE);        
        if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Entry found but data missed or Entry not found")));
        }
        *encap_id = phy_port.encap_id;
        BCM_EXIT;
    }


    /* Validate gport existance */
    if (gport_parse_info.type == _bcmDppGportParseTypeEncap) {
        /* The resolve main functionality is for OutLIFs as it determines
           destination info. For Ingress only LIFs, only the LIF_ID and
           gport_type will be resolved. */
        *gport_type = _bcmDppGportResolveTypeAC; /* This is not really correct to use , need to reexamine again */
        global_lif_id =  gport_parse_info.val;
        if (BCM_GPORT_IS_VLAN_PORT(gport) || BCM_GPORT_IS_MPLS_PORT(gport)) {
            if (gport_parse_info.flags & _BCM_DPP_GPORT_PARSE_INGRESS_ONLY) {
                is_ingress_only = TRUE;
            }
            if (gport_parse_info.flags & _BCM_DPP_GPORT_PARSE_EGRESS_ONLY) {
                is_egress_only = TRUE;
            }
        }
        /* lif_used_for_forwarding means that LIF is included in the hash table */
        lif_used_for_forwarding = 0;
        if (!is_ingress_only && !is_egress_only && BCM_GPORT_IS_MPLS_PORT(gport)) {
            /* 
             * Verify are we LIF+Port (include hash-table entry) or 
             * LIF that is not used for forwarding (LIF that is part of protection without FEC).
             * We don't have simple SW state for that, then just lookup hash-table and according to found/not-found decide.
             */
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                               (sw_state_htb_key_t) (sw_state_htb_key_t) (&gport),
                                               (sw_state_htb_data_t) &phy_port,
                                               FALSE);        
            if (!GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                lif_used_for_forwarding = 1; /* Found it is not protected lif */
            }             
        } else if (is_ingress_only && BCM_GPORT_IS_MPLS_PORT(gport)) {
            /* Ingress only , in hash */
            lif_used_for_forwarding = 1;
        }

        /* Validate the OutLIF, or the InLIF in case of ingress only gport. */
        if (!(flags & _BCM_DPP_GPORT_RESOLVE_FLAG_NO_CHECK)) {
            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, (is_egress_only) ? _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS : 
                                                                        _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, global_lif_id, &local_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_egress_only) {
                rv = _bcm_dpp_lif_sw_resources_lif_usage_get(unit, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_lif_id, NULL, &lif_usage);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                rv = _bcm_dpp_lif_sw_resources_lif_usage_get(unit, local_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID, &lif_usage, NULL);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (lif_usage == _bcmDppLifTypeAny) {
                BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("gport not found ")));
            }
        }
    }

    if (gport_parse_info.type == _bcmDppGportParseTypeForwardFec || gport_parse_info.type == _bcmDppGportParseTypeProtectedFec) {/*FEC encap is stored in SW*/
        *gport_type = _bcmDppGportResolveTypeFec;
        dest_id->dest_type = SOC_SAND_PP_DEST_FEC;
        dest_id->dest_val = gport_parse_info.val;

        if (BCM_GPORT_IS_TRILL_PORT(gport)) {
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                               (sw_state_htb_key_t) (&gport),
                                               (sw_state_htb_data_t) &phy_port,
                                               FALSE);        
            if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("gport not found")));
            }
            dest_id->dest_val = phy_port.phy_gport;
            *encap_id = phy_port.encap_id;
            *gport_type = phy_port.type;
        }

        else if (BCM_GPORT_IS_MIM_PORT(gport)) {

            dest_id->dest_val = gport_parse_info.val; /* FEC */

            /* get sw_db entry to check port type */
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                               (sw_state_htb_key_t) (&gport),
                                               (sw_state_htb_data_t) &phy_port,
                                               FALSE);        
            if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("gport not found")));
            }

            *gport_type = phy_port.type;

            if (phy_port.type == _bcmDppGportResolveTypeMimP2P) { /* P2P */

                rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, phy_port.phy_gport, (int*)&lif);
                BCMDNX_IF_ERR_EXIT(rv);

                /* get I-SID from lif_info */
                rv = _bcm_dpp_in_lif_mim_match_get(unit, &mim_port, lif);
                BCMDNX_IF_ERR_EXIT(rv);

                *encap_id = mim_port.egress_tunnel_service; /* I-SID */
            }
            else if (phy_port.type == _bcmDppGportResolveTypeMimMP) { /* MP */

                /* get enacp_id from fec entry */
                soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, dest_id->dest_val, &protect_type, &fec_entry[0], &fec_entry[1], &protect_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                *encap_id = fec_entry[0].eep; /* EEP */
            }
            else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("gport not found ")));
            }
        }

        else {
        
            /* get encap ID from FEC*/
            soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,dest_id->dest_val,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* if FEC used for bridging into AC  */
            if(fec_entry[0].type == SOC_PPD_FEC_TYPE_BRIDGING_WITH_AC) {
                *encap_id = fec_entry[0].app_info.out_ac_id;
            }
            else {
                *encap_id = fec_entry[0].eep ? fec_entry[0].eep : _BCM_GPORT_ENCAP_ID_LIF_INVALID;
            }
        }
        BCM_EXIT;
    }

    if (gport_parse_info.type == _bcmDppGportParseTypeSimple) { /* simple gport*/
        rv = _bcm_dpp_gport_to_fwd_decision(unit,gport,&dummy_fwd_decsion);
        BCMDNX_IF_ERR_EXIT(rv);
        
        soc_sand_rv = soc_ppd_fwd_decision_to_sand_dest(soc_sand_dev_id,&dummy_fwd_decsion,dest_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);    
    } else if ((is_ingress_only && BCM_GPORT_IS_VLAN_PORT(gport)) || /* Ingress only gport, just use the parse resolution, destination info is invalid */
               (is_egress_only && BCM_GPORT_IS_MPLS_PORT(gport))  || /* Egress only gport, just use the parse resolution, destination info is invalid */
               (lif_used_for_forwarding == 0 && BCM_GPORT_IS_MPLS_PORT(gport) && gport_parse_info.type == _bcmDppGportParseTypeEncap)) /* LIF is not used for forwarding */ {
        *encap_id = global_lif_id;
        dest_id->dest_type = -1;
    } else {  /* rest of cases: get stored information */

        /*LIF encap is stored in SW*/
        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                           (sw_state_htb_key_t) (&gport),
                                           (sw_state_htb_data_t) &phy_port,
                                           FALSE);        
        if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("gport not found")));
        }
        else {
            /* For some FEC Types and for Ring Protection, the destination is a FEC
               and the OutLIF that are stored in the DB entry */
            if ((phy_port.type == _bcmDppGportResolveTypeFecVC) || (phy_port.type == _bcmDppGportResolveTypeFecEEP) ||
                (phy_port.type == _bcmDppGportResolveTypeRing)) {      /*FEC encap is stored in SW*/
                dest_id->dest_type = SOC_SAND_PP_DEST_FEC;
                dest_id->dest_val = phy_port.phy_gport;
            }
            else if (phy_port.type == _bcmDppGportResolveTypeL2GreEgFec) { /* l2gre gport is always LIF (in-tunnel)*/
                dest_id->dest_type = SOC_SAND_PP_DEST_FEC;
                dest_id->dest_val = phy_port.phy_gport;
                
                /* get outlif from FEC */
                soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,dest_id->dest_val,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                *encap_id = fec_entry[0].eep; /* EEP */
                *gport_type = phy_port.type;
                BCM_EXIT;
            }
            else if (phy_port.type == _bcmDppGportResolveTypeVxlanEgFec) { /* VXLAN gport is always LIF (in-tunnel)*/
                dest_id->dest_type = SOC_SAND_PP_DEST_FEC;
                dest_id->dest_val = phy_port.phy_gport;
                
                /* get outlif from FEC */
                soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,dest_id->dest_val,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                *encap_id = fec_entry[0].eep; /* EEP */
                *gport_type = phy_port.type;
                BCM_EXIT;
            }
            else if (BCM_GPORT_IS_MPLS_PORT(gport) && (phy_port.type == _bcmDppGportResolveTypeFec)) { /* MPLS PORT FEC */
                dest_id->dest_type = SOC_SAND_PP_DEST_FEC;
                dest_id->dest_val = phy_port.phy_gport;

                *encap_id = global_lif_id;
                *gport_type = phy_port.type;
                BCM_EXIT;
            }
            else{
                /* call for inner gport <it has to be physical gport > */
                rv = _bcm_dpp_gport_to_fwd_decision(unit,phy_port.phy_gport,&dummy_fwd_decsion);
                BCMDNX_IF_ERR_EXIT(rv);

                soc_sand_rv = soc_ppd_fwd_decision_to_sand_dest(soc_sand_dev_id,&dummy_fwd_decsion,dest_id);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            *encap_id = phy_port.encap_id;
            *gport_type = phy_port.type;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Convert gport to encap usage
 */
int _bcm_dpp_gport_id_to_encap_usage(int unit, bcm_gport_t gport, uint8 *encap_usage){

    BCMDNX_INIT_FUNC_DEFS;

    *encap_usage = BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL;

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        *encap_usage = BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_VLAN_PORT;
    }
    else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        *encap_usage = BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_MPLS_PORT;
    }
    else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        *encap_usage = BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_ISID;
    }

    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *       _bcm_dpp_gport_to_port_encap
 * Description:
 *       map gport to forwarding info <phy-port (can be FEC) and encap that store LIF or EEI)
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  dest_port -      [OUT] dest_port is physical TM port or FEC. i.e. pp-destination
 *  encap_id -       [OUT] encap-info includes eei or outlif
 * Returns:
 *       BCM_E_XXX
 */int 
_bcm_dpp_gport_to_port_encap(int unit, bcm_gport_t gport, int *dest_port, int *encap_id)
{

    SOC_PPD_FRWRD_DECISION_INFO  fwd_decsion;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    uint8 encap_usage;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_DPP_IS_SYS_RSRC_MGMT_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("calling this property while feature not supported. soc-property: system_resource_management ")));
    }

    *dest_port = BCM_GPORT_INVALID;
    *encap_id = BCM_FORWARD_ENCAP_ID_INVALID;

    /* set destination */
    rv = _bcm_dpp_gport_parse(unit,gport,&gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* given gport is phy-port */
    if (gport_parse_info.type == _bcmDppGportParseTypeSimple) {
        *dest_port = gport;
        *encap_id = BCM_FORWARD_ENCAP_ID_INVALID;

        LOG_INFO(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                             "gport is physical port:0x%x encap_id=0x%x\n"),*dest_port, *encap_id));
        BCM_EXIT;
    }

    /* gport to frwrd decision */
    rv = _bcm_dpp_gport_to_fwd_decision(unit,gport,&fwd_decsion);
    BCMDNX_IF_ERR_EXIT(rv);


    rv = _bcm_dpp_gport_id_to_encap_usage(unit,gport,&encap_usage);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,dest_port,encap_id,&fwd_decsion,encap_usage);
    BCMDNX_IF_ERR_EXIT(rv);

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "_bcm_dpp_gport_to_port_encap gport:0x%x dest_port:0x%x encap_id=0x%x\n"),gport, *dest_port, *encap_id));


exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *       _bcm_dpp_port_encap_to_fwd_decision
 * Description:
 *       Parse gport+encap to soc-ppd-forwarding decision
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port can be FEC or physical port or vport (then encap not used)
 *  encap_id -       [IN] encap_id EEI or outlif.
 *                          if invalid then 
 *  fwd_decsion -    [OUT] PPD forwarding decision
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_port_encap_to_fwd_decision(int unit, bcm_gport_t gport, int encap_id, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* map port to frwrd-decision */
    SOC_PPD_FRWRD_DECISION_INFO_clear(fwd_decsion);

    /* 1. dest */
    /* is FEC */
    if (BCM_GPORT_IS_FORWARD_PORT(gport)) {
        fwd_decsion->type = SOC_PPD_FRWRD_DECISION_TYPE_FEC;
        fwd_decsion->dest_id = BCM_GPORT_FORWARD_PORT_GET(gport);
    }
    else {
        /* gport is just physical gport. map it using generic APi */
        rv = _bcm_dpp_gport_to_fwd_decision(unit,gport,fwd_decsion);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (!(BCM_GPORT_IS_VLAN_PORT(gport) && BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(gport)) && 0/*!_BCM_DPP_GPORT_TO_FWD_DECISION_USE_ENCAP_ID(unit,encap_id)*/) {
        /* if encap is not in use then done */
        /* if the encap type is forward, need to continue to use ACL profile from encap */
        BCM_EXIT;
    }

    /* 2. now need to fill the additional info according to encap */
    if (encap_id != BCM_FORWARD_ENCAP_ID_INVALID) {
        if((BCM_GPORT_IS_VLAN_PORT(gport) || BCM_GPORT_IS_MPLS_PORT(gport)) && BCM_GPORT_SUB_TYPE_GET(gport) == BCM_GPORT_SUB_TYPE_FORWARD_GROUP) {
            fwd_decsion->additional_info.outlif.type = SOC_PPD_OUTLIF_ENCODE_TYPE_RAW_INVALID;
            fwd_decsion->additional_info.outlif.val = BCM_FORWARD_ENCAP_ID_VAL_GET(encap_id);
        }
        else if (BCM_FORWARD_ENCAP_ID_IS_OUTLIF(encap_id)) {
            fwd_decsion->additional_info.outlif.type = SOC_PPD_OUTLIF_ENCODE_TYPE_RAW;
            fwd_decsion->additional_info.outlif.val = BCM_FORWARD_ENCAP_ID_VAL_GET(encap_id);
        }
        else if (encap_id != BCM_FORWARD_ENCAP_ID_INVALID) { /* EEI */
            fwd_decsion->additional_info.eei.type = SOC_PPD_EEI_TYPE_RAW;
            fwd_decsion->additional_info.eei.val.raw = BCM_FORWARD_ENCAP_ID_VAL_GET(encap_id);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *    _bcm_dpp_phy_port_from_fwd_decision
 * Description:
 *    convert forwardubg decision (destination + editting information) to gport
 * Parameters:
 *  unit  -          [IN] DPP device unit number (driver internal).
 *  gport -          [OUT] general port
 *  fwd_decsion -    [IN] PPD forwarding decision
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_phy_port_from_fwd_decision(int unit, bcm_gport_t *gport, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion)
{
    int rv = BCM_E_NONE;
    bcm_module_t modid;
    bcm_port_t mode_port;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(fwd_decsion);

    if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_DROP) {
        *gport = BCM_GPORT_BLACK_HOLE;
    }
    else if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_MC) {
        BCM_GPORT_MCAST_SET(*gport,fwd_decsion->dest_id);
    }
    else if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_UC_PORT){
        /* map system port to mod-port*/
        rv = _bcm_dpp_port_mod_port_from_sys_port(unit,&modid,&mode_port,fwd_decsion->dest_id);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_GPORT_MODPORT_SET(*gport,modid,mode_port);        
    }
    else if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_UC_LAG) {
        /* map system port*/
        BCM_GPORT_TRUNK_SET(*gport,fwd_decsion->dest_id);
    }
    else if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_UC_FLOW) {
        /* map system port*/
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(*gport,fwd_decsion->dest_id);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Can not retrieve port type")));
    }

    exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *    _bcm_dpp_gport_from_fwd_decision
 * Description:
 *    convert forwardubg decision (destination + editting information) to gport
 * Parameters:
 *    unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [OUT] general port
 *  fwd_decsion -    [IN] PPD forwarding decision
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_from_fwd_decision(int unit, bcm_gport_t *gport, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion)
{
    int rv = BCM_E_NONE;
    int add_info_null = FALSE;
    _bcm_lif_type_e out_lif_usage;
    bcm_gport_t phy_gport;
    _bcm_dpp_gport_info_t gport_info;
    int trap_id_converted;
    int outlif_encoded; 
    int local_out_lif;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(fwd_decsion);

    if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_UC_FLOW ||
        fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_UC_PORT ||
        fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_UC_LAG ||
        fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_DROP){
        if (fwd_decsion->additional_info.outlif.type == SOC_PPD_OUTLIF_ENCODE_TYPE_NONE) {
            add_info_null = TRUE;
        }
    }
    else if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_MC ||
             fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_FEC){
        if (fwd_decsion->additional_info.eei.type == SOC_PPD_EEI_TYPE_EMPTY && /* Out-LIF could go together with FEC when used for profile */
            fwd_decsion->additional_info.outlif.type != SOC_PPD_OUTLIF_ENCODE_TYPE_RAW_INVALID) {
            add_info_null = TRUE;
        }
    }

    /* In case there is no additional info */
    if (add_info_null) {
        if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_FEC) {
            /* point to FEC without additional info */
            /* Get FEC gport from SW DB */
           	rv = GPORT_MGMT_ACCESS.bcm_dpp_sw_resources_fec_to_gport.get(unit, fwd_decsion->dest_id, gport);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            /* destination is phy-port inside gport info, map fwd-physical dest to gport */
            rv = _bcm_dpp_phy_port_from_fwd_decision(unit, gport, fwd_decsion);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    /* Additional info present */
    else {
        if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_TRAP) {
            rv = _bcm_dpp_rx_get_converted_trap_sw_id(unit, fwd_decsion->additional_info.trap_info.action_profile.trap_code, &trap_id_converted);
            /* Get trap info from additional info */
            BCM_GPORT_TRAP_SET(*gport, trap_id_converted, 
                               fwd_decsion->additional_info.trap_info.action_profile.frwrd_action_strength,
                               fwd_decsion->additional_info.trap_info.action_profile.snoop_action_strength);
        }
        else if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_FEC){
        /* additional_info includes EEI */
            /* Get FEC gport from SW DB */
           	rv = GPORT_MGMT_ACCESS.bcm_dpp_sw_resources_fec_to_gport.get(unit, fwd_decsion->dest_id, gport);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else { /* Out-Lif */

            /* verify that that the gport is local */
            /* destination is phy-port inside gport info, map fwd-physical dest to gport */
            rv = _bcm_dpp_phy_port_from_fwd_decision(unit, &phy_gport, fwd_decsion);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_gport_to_phy_port(unit, phy_gport, 0, &gport_info);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!(gport_info.flags & _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT)) {
                LOG_VERBOSE(BSL_LS_BCM_PORT, (BSL_META_U(unit,"Outlif not local\n")));
                rv = BCM_E_NOT_FOUND;
                BCM_EXIT;
            }

            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, fwd_decsion->additional_info.outlif.val, &local_out_lif);
            BCMDNX_IF_ERR_EXIT(rv);

            /* destination is local - can try to find in SW DB */
            rv = _bcm_dpp_lif_sw_resources_lif_usage_get(unit, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_out_lif, NULL, &out_lif_usage);
            BCMDNX_IF_ERR_EXIT(rv);

            if (out_lif_usage == _bcmDppLifTypeAny) {
                LOG_VERBOSE(BSL_LS_BCM_PORT, (BSL_META_U(unit,"Outlif not found\n")));
                rv = BCM_E_NOT_FOUND;
                BCM_EXIT;
            }

            if ((out_lif_usage == _bcmDppLifTypeMplsPort)) {
                BCM_GPORT_SUB_TYPE_LIF_SET(outlif_encoded, 0, fwd_decsion->additional_info.outlif.val);
                BCM_GPORT_MPLS_PORT_ID_SET(*gport, outlif_encoded);
            }
            else if ((out_lif_usage == _bcmDppLifTypeVlan)) {
                BCM_GPORT_SUB_TYPE_LIF_SET(outlif_encoded, 0, fwd_decsion->additional_info.outlif.val);
                BCM_GPORT_VLAN_PORT_ID_SET(*gport, outlif_encoded);
            }
            else {
                rv = _bcm_dpp_lif_to_gport_encoding(unit, fwd_decsion->additional_info.outlif.val, out_lif_usage, gport);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *       _bcm_dpp_encap_from_fwd_decision
 * Description:
 *       Parse soc-ppd-forwarding decision to encap
 *       This can be called only in system mode
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [OUT] general port
 *  encap_id -       [OUT] encap_id EEI or outlif.
 *                          if invalid then
 *  encap_usage   :  [IN] AC,PWE,MIM, General
 *  fwd_decsion -    [IN] PPD forwarding decision
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_encap_from_fwd_decision(int unit, int *encap_id, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion, uint8 encap_usage)
{

    uint32 dest,additional_info;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* frwrd decision addtional info to HW/Arch encoding */
    unit = (unit);
    soc_sand_rv = arad_pp_fwd_decision_in_buffer_build(unit,ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,fwd_decsion,&dest,&additional_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* map additional info to encap:EEI/outLIF*/
    if (fwd_decsion->additional_info.outlif.type != SOC_PPD_OUTLIF_ENCODE_TYPE_NONE) {
        /* set encap to include outlif value */
        BCM_FORWARD_ENCAP_ID_VAL_SET(*encap_id, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF,encap_usage,additional_info);
    }
    else if (fwd_decsion->additional_info.eei.type != SOC_PPD_EEI_TYPE_EMPTY) {
        /* set encap to include eei value */
        if (fwd_decsion->additional_info.eei.type == SOC_PPD_EEI_TYPE_MPLS) {
            BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(*encap_id, fwd_decsion->additional_info.eei.val.mpls_command.label, fwd_decsion->additional_info.eei.val.mpls_command.push_profile);
            encap_usage = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT;
        }
        BCM_FORWARD_ENCAP_ID_VAL_SET(*encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI,encap_usage,additional_info);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *       _bcm_dpp_port_from_fwd_decision
 * Description:
 *       Parse soc-ppd-forwarding decision to port
 *       This can be called only in system mode
 *  
 *       This function is called only from _bcm_dpp_gport_and_encap_from_fwd_decision
 *  
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [OUT] general port
 *  port  -          [OUT] port (FEC or system port)
 *  fwd_decsion -    [IN] PPD forwarding decision
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_port_from_fwd_decision(int unit, bcm_gport_t *port, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion)
{

    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* destination is FEC */
    if(fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_FEC){
        BCM_GPORT_FORWARD_PORT_SET(*port, fwd_decsion->dest_id);
        LOG_INFO(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                             "dest is FEC =0x%x\n"),fwd_decsion->dest_id));
    }
    else {
        /* destination is phy-port inside gport info, map fwd-physical dest to gport */
        rv = _bcm_dpp_phy_port_from_fwd_decision(unit, port, fwd_decsion);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *       _bcm_dpp_gport_and_encap_from_fwd_decision
 * Description:
 *       Parse soc-ppd-forwarding decision to gport+encap
 *       This can be called only in system mode 
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [OUT] general port
 *  encap_id -       [OUT] encap_id EEI or outlif.
 *                          if invalid then
 *  encap_usage   :  [IN] AC,PWE,MIM, General, -1 in case encap is not required
 *  fwd_decsion -    [IN] PPD forwarding decision
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_gport_and_encap_from_fwd_decision(int unit, bcm_gport_t *gport, int *encap_id, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion, uint32 encap_usage)
{

    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* If EEI is not empty we must return port(FEC)+ encap */
    if (fwd_decsion->additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) {
        /* get encap_id from frwrd decision addtional info */    
        rv = _bcm_dpp_encap_from_fwd_decision(unit,encap_id, fwd_decsion, encap_usage);
        BCMDNX_IF_ERR_EXIT(rv);

        if (fwd_decsion->type == SOC_PPD_FRWRD_DECISION_TYPE_FEC) {
            /* get gport (FEC) from frwrd decision port */    
            rv = _bcm_dpp_gport_from_fwd_decision(unit, gport, fwd_decsion);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            /* destination is phy-port inside gport info, map fwd-physical dest to gport */
            rv = _bcm_dpp_phy_port_from_fwd_decision(unit, gport, fwd_decsion);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        

        BCM_EXIT;
    }
    else {
        if (!SOC_DPP_IS_SYS_RSRC_MGMT_ADVANCED(unit) || (encap_usage == -1)/* !(If api is MACT or CC) */) {
            rv = _bcm_dpp_gport_from_fwd_decision(unit,gport,fwd_decsion);
            if ((encap_usage != -1)/* (If api is MACT or CC) */) {
                if (rv != BCM_E_NOT_FOUND) {
                    BCMDNX_IF_ERR_EXIT(rv);
                    BCM_EXIT;
                }
            }
            else {
                BCMDNX_IF_ERR_EXIT(rv);
                BCM_EXIT;
            }
        }
    }

    /* get encap_id from frwrd decision addtional info */    
    rv = _bcm_dpp_encap_from_fwd_decision(unit,encap_id, fwd_decsion, encap_usage);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get port from frwrd decision */    
    rv = _bcm_dpp_port_from_fwd_decision(unit, gport, fwd_decsion);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    _bcm_dpp_gport_to_fwd_decision
 *    Relevant only to forwarding gports (gports that can be placed in forwarding database such as MACT
 * Description:
 *    convert gport to forwarding decision (destination + editing information)
 * Parameters:
 *    unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  encap_id -       [IN] encap_id considered only if gport is logical port
 *  fwd_decsion -    [OUT] PPD forwarding decision
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_to_fwd_decision(int unit, bcm_gport_t port, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion)
{
    bcm_module_t    _modid;
    bcm_port_t      _mode_port;
    uint32       tm_port;
    uint32       _sys_phy_port_id = 0;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    _bcm_dpp_gport_resolve_type_e gport_type;
    int             encap_id=0;
    bcm_gport_t gport;
    SOC_SAND_PP_DESTINATION_ID dest_id;
    int param0;
    uint32 soc_sand_rv, trap_id, snoop_str, trap_str;
    int rv = BCM_E_NONE;
    int push_profile, vc_label, core;
    uint8           is_trill_mc = FALSE;    
    uint8 pon_app_enable;
    
    BCMDNX_INIT_FUNC_DEFS;
    if (NULL == fwd_decsion){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fwd_decsion is NULL")));
    }

    unit = (unit);
    SOC_PPD_FRWRD_DECISION_INFO_clear(fwd_decsion);

    pon_app_enable = SOC_DPP_CONFIG(unit)->pp.pon_application_enable;

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else if (pon_app_enable && (_BCM_PPD_IS_PON_PP_PORT(port))) {
        uint32 offset;
        
        if (SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_enable) {
            offset = _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_OFFSET(SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_num);
        } else {
            offset = _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_DEFAULT_OFFSET;
        }

        BCM_GPORT_LOCAL_SET(gport,_BCM_PPD_GPORT_PON_TO_LOCAL_PORT(port, offset));
    } else if (SOC_PORT_VALID(unit, port)) { 
        BCM_GPORT_LOCAL_SET(gport,port);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("gport given is invalid port"))); 
    }

    if (BCM_GPORT_IS_BLACK_HOLE(gport)) { /* drop */
        SOC_PPD_FRWRD_DECISION_DROP_SET(unit, fwd_decsion, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_EXIT;
    }
    if(BCM_GPORT_IS_LOCAL(gport)){ /* local-port*/
        rv = bcm_petra_stk_my_modid_get(unit, &_modid);
        BCMDNX_IF_ERR_EXIT(rv);
        _mode_port = BCM_GPORT_LOCAL_GET(gport);
        
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, _mode_port, &tm_port, &core)));

        _modid = SOC_DPP_CORE_TO_MODID(_modid, core);

        /* get physical system port, identify <mod,port>*/
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_local_to_sys_phys_port_map_get,(unit,_modid,tm_port,&_sys_phy_port_id)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_PPD_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, fwd_decsion, _sys_phy_port_id, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_EXIT;
    }
    if(BCM_GPORT_IS_LOCAL_CPU(gport)){ /* CPU-port*/
        rv = bcm_petra_stk_my_modid_get(unit, &_modid);
        BCMDNX_IF_ERR_EXIT(rv);
        _mode_port = CMIC_PORT(unit);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, _mode_port, &tm_port, &core)));

        /* get physical system port, identify <mod,port>*/
        _modid = SOC_DPP_CORE_TO_MODID(_modid, core);
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_local_to_sys_phys_port_map_get,(unit,_modid,tm_port,&_sys_phy_port_id)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_PPD_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, fwd_decsion, _sys_phy_port_id, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_EXIT;
    }
    if(BCM_GPORT_IS_MODPORT(gport)){ /* mod-port*/
        _modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        _mode_port = BCM_GPORT_MODPORT_PORT_GET(gport);
        
        /* get physical system port, identify <mod,port>*/
        rv = _bcm_dpp_port_mod_port_to_sys_port(unit,_modid,_mode_port,&_sys_phy_port_id);
        BCMDNX_IF_ERR_EXIT(rv);

        SOC_PPD_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, fwd_decsion, _sys_phy_port_id, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_EXIT;
    }
    if(BCM_GPORT_IS_TRUNK(gport)){  /*trunk*/
        param0 = BCM_GPORT_TRUNK_GET(gport);
        SOC_PPD_FRWRD_DECISION_LAG_SET(unit, fwd_decsion, param0, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_EXIT;
    }
    if(BCM_GPORT_IS_MCAST(gport)) { /*Multicast*/
        param0 = BCM_GPORT_MCAST_GET(gport);
        /* Get multicast ID */
        param0 = _BCM_MULTICAST_ID_GET(param0);
        SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, fwd_decsion, param0, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_EXIT;
    }
    if(BCM_GPORT_IS_SYSTEM_PORT(gport)){  /*sytem port*/
        _sys_phy_port_id = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);
        /* checking that system port is valid */
        if (_sys_phy_port_id  > ARAD_NOF_SYS_PHYS_PORTS_GET(unit) - 1)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport SYSTEM port id is invalid.")));
        } 
        SOC_PPD_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, fwd_decsion, _sys_phy_port_id, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_EXIT;
    }


    if(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)){  /*CosQ/Flow*/
        param0 = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
        SOC_PPD_FRWRD_DECISION_EXPL_FLOW_SET(unit, fwd_decsion, param0, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_EXIT;
    }

    if (BCM_GPORT_IS_TRAP(gport)) {
        SOC_PPD_TRAP_CODE ppd_trap_code;

        trap_id = BCM_GPORT_TRAP_GET_ID(gport);
        /* trap_id may be a virtual trap. convert it to ppd trap to create forward decision */
        rv = _bcm_rx_ppd_trap_code_from_trap_id(unit, trap_id, &ppd_trap_code);
        BCMDNX_IF_ERR_EXIT(rv);

        snoop_str = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(gport);
        trap_str = BCM_GPORT_TRAP_GET_STRENGTH(gport);
        SOC_PPD_FRWRD_DECISION_TRAP_SET(unit, fwd_decsion, ppd_trap_code, trap_str, snoop_str, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_EXIT;
    }
    
    rv = _bcm_dpp_gport_parse(unit,gport,&gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

      /* A Forward Port is a FEC */
    if (BCM_GPORT_IS_FORWARD_PORT(gport)) {
        SOC_PPD_FRWRD_DECISION_FEC_SET(unit, fwd_decsion, gport_parse_info.val, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_EXIT;
    }

    if ((BCM_GPORT_IS_VLAN_PORT(gport)) || (BCM_GPORT_IS_EXTENDER_PORT(gport))) {  /* vlan_port or Extender port */
        if (gport_parse_info.type == _bcmDppGportParseTypeProtectedFec || 
            gport_parse_info.type == _bcmDppGportParseTypeForwardFec) { /* if FEC done */
            SOC_PPD_FRWRD_DECISION_PROTECTED_AC_SET(unit, fwd_decsion, 0, gport_parse_info.val, soc_sand_rv); /* encap_id is ignored */
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        /* 1+1 protection */
        /* if the gport has 1+1 protection then
           fwd-decision is MC-group */
        else if (gport_parse_info.type == _bcmDppGportParseTypeMC) {
            /* get MC from gport */        
            SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, fwd_decsion, gport_parse_info.val, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);            
        }
        else {
            rv = _bcm_dpp_gport_resolve(unit,gport,0, &dest_id,&encap_id,&gport_type);
            BCMDNX_IF_ERR_EXIT(rv);
            /* found fec then forwarding decision is FEC */
            if ((gport_type == _bcmDppGportResolveTypeAC) ||   /* phy + AC */
                (gport_type == _bcmDppGportResolveTypeRing)) { /* FEC + AC */
                /* set physical dest information */
                soc_sand_rv = soc_ppd_sand_dest_to_fwd_decision(unit,&dest_id,fwd_decsion);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                /* update oulif to include the AC */
                (fwd_decsion)->additional_info.outlif.type = SOC_PPD_OUTLIF_ENCODE_TYPE_RAW;
                (fwd_decsion)->additional_info.outlif.val = encap_id /* OutLIF */;
            }
            else{
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport VLAN port given does not include enough information"))); /* vlan-port doesn't include relevant/enough information */
            }
        }
        BCM_EXIT;
    }
    if(BCM_GPORT_IS_MPLS_PORT(gport)){  /*mpls_port*/

        rv = _bcm_dpp_gport_resolve(unit,gport,0, &dest_id,&encap_id,&gport_type);
        BCMDNX_IF_ERR_EXIT(rv);

        if(gport_type == _bcmDppGportResolveTypeFec) { /* FEC: protected PWE/AC */
            SOC_PPD_FRWRD_DECISION_PROTECTED_PWE_SET(unit, fwd_decsion, dest_id.dest_val, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else if (gport_parse_info.type == _bcmDppGportParseTypeMC) {
            /* get MC from gport */        
            SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, fwd_decsion, gport_parse_info.val, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);            
        }
        else if(gport_type == _bcmDppGportResolveTypePhyEEP) { /* phy + pwe */
            /* set physical dest information */
            soc_sand_rv = soc_ppd_sand_dest_to_fwd_decision(unit,&dest_id,fwd_decsion);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* update oulif to include the PWE/VC */
            (fwd_decsion)->additional_info.outlif.type = SOC_PPD_OUTLIF_ENCODE_TYPE_RAW;
            (fwd_decsion)->additional_info.outlif.val = encap_id;
        }
        else if(gport_type == _bcmDppGportResolveTypeFecVC) {  /*VC+FEC: pwe protected Tunnel */
            vc_label = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(encap_id);
            push_profile = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(encap_id);
            SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_SET(unit, fwd_decsion, vc_label, push_profile, dest_id.dest_val /* FEC */, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else if(gport_type == _bcmDppGportResolveTypeFecEEP) { /* FEC + eep for tunnel, protected PWE */
            /* set FEC*/
            soc_sand_rv = soc_ppd_sand_dest_to_fwd_decision(unit,&dest_id,fwd_decsion);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* update oulif to include the PWE/VC */
            (fwd_decsion)->additional_info.outlif.type = SOC_PPD_OUTLIF_ENCODE_TYPE_RAW;
            (fwd_decsion)->additional_info.outlif.val = encap_id;
        }
        else{
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport MPLS port does not include enough informaiton"))); /* mpls-port doesn't include relevant/enough information */
        }
        BCM_EXIT;
    }
    if (BCM_GPORT_IS_MIM_PORT(gport)) {  /* mim_port */

        /* get the I-SID (encap_id), FEC (dest) and type */
        rv = _bcm_dpp_gport_resolve(unit, gport, 0, &dest_id, &encap_id, &gport_type);
        BCMDNX_IF_ERR_EXIT(rv);

        if (gport_type == _bcmDppGportResolveTypeMimP2P) {
            /* turn I-SID and FEC into frwrd decision */
            SOC_PPD_FRWRD_DECISION_MAC_IN_MAC_SET(unit, fwd_decsion, encap_id, dest_id.dest_val, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else if (gport_type == _bcmDppGportResolveTypeMimMP || gport_type == _bcmDppGportResolveTypeFec) {
            SOC_PPD_FRWRD_DECISION_FEC_SET(unit, fwd_decsion, dest_id.dest_val, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else{
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("gport MIM port does not include enough information"))); 
        }
        BCM_EXIT;
    }
    if(BCM_GPORT_IS_TRILL_PORT(gport)){  

        rv = _bcm_dpp_gport_resolve(unit,gport,0,&dest_id,&encap_id,&gport_type);
        BCMDNX_IF_ERR_EXIT(rv);

        is_trill_mc = (gport_type == _bcmDppGportResolveTypeTrillMC);
        if (is_trill_mc) {
            dest_id.dest_val = _BCM_MULTICAST_ID_GET(dest_id.dest_val);
        }       
        SOC_PPD_FRWRD_DECISION_TRILL_SET(unit, fwd_decsion, encap_id, is_trill_mc, dest_id.dest_val, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCM_EXIT;
    }
   
    if(BCM_GPORT_IS_L2GRE_PORT(gport)){  

        rv = _bcm_dpp_gport_resolve(unit,gport,0,&dest_id,&encap_id,&gport_type);
        BCMDNX_IF_ERR_EXIT(rv);

        /* FEC */
        if(gport_type == _bcmDppGportResolveTypeL2GreEgFec) {
            SOC_PPD_FRWRD_DECISION_FEC_SET(unit, fwd_decsion, dest_id.dest_val, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else if (gport_type == _bcmDppGportResolveTypeL2Gre) { /* port + outlif */
            soc_sand_rv = soc_ppd_sand_dest_to_fwd_decision(unit,&dest_id,fwd_decsion);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            (fwd_decsion)->additional_info.outlif.type = SOC_PPD_OUTLIF_ENCODE_TYPE_RAW;
            (fwd_decsion)->additional_info.outlif.val = encap_id;
        }
        else{
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("L2GRE gport does not include enough information"))); /* vlan-port doesn't include relevant/enough information */
        }
        BCM_EXIT;
    }

    if(BCM_GPORT_IS_VXLAN_PORT(gport)){  

        rv = _bcm_dpp_gport_resolve(unit,gport,0,&dest_id,&encap_id,&gport_type);
        BCMDNX_IF_ERR_EXIT(rv);

        /* FEC */
        if(gport_type == _bcmDppGportResolveTypeVxlanEgFec) {
            SOC_PPD_FRWRD_DECISION_FEC_SET(unit, fwd_decsion, dest_id.dest_val, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else if (gport_type == _bcmDppGportResolveTypeVxlan) { /* port + outlif */
            soc_sand_rv = soc_ppd_sand_dest_to_fwd_decision(unit,&dest_id,fwd_decsion);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            (fwd_decsion)->additional_info.outlif.type = SOC_PPD_OUTLIF_ENCODE_TYPE_RAW;
            (fwd_decsion)->additional_info.outlif.val = encap_id;
        }
        else{
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VXLAN gport does not include enough information"))); /* vlan-port doesn't include relevant/enough information */
        }
        BCM_EXIT;
    }    

    /* not supported. if still relevant, should be handled before calling this API*/
    BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("not valid gport ")));
    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *    _bcm_dpp_gport_to_sand_pp_dest
 * Description:
 *    convert gport to soc_sand-pp-destination
 * Parameters:
 *    unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  dest_id -       [IN] SOC_SAND destination
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_to_sand_pp_dest(int unit, bcm_gport_t gport, SOC_SAND_PP_DESTINATION_ID  *dest_id)
{
    SOC_PPD_FRWRD_DECISION_INFO  fwd_decision;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(dest_id);
    soc_sand_dev_id = (unit);

    rv = _bcm_dpp_gport_to_fwd_decision(unit,gport,&fwd_decision);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_fwd_decision_to_sand_dest(soc_sand_dev_id,&fwd_decision,dest_id);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *    _bcm_dpp_gport_compare
 * Description:
 *    compare two gports
 * Parameters:
 *    unit -          [IN] DPP device unit number (driver internal).
 *  gport1 -          [IN] general port 1
 *  gport2 -          [IN] general port 2 
 *  is_equal -         [OUT] result of comparison
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_compare(int unit, bcm_gport_t gport1, bcm_gport_t gport2, _bcm_dpp_gport_compare_type_e type, uint8 * is_equal)
{
    int rv = BCM_E_NONE;
    SOC_SAND_PP_DESTINATION_ID  dest_id1, dest_id2;
    int lif1, lif2;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_equal);

    switch (type) {
    case _bcmDppGportCompareTypeSystemPort:

        rv = _bcm_dpp_gport_to_sand_pp_dest(unit, gport1, &dest_id1);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_to_sand_pp_dest(unit, gport2, &dest_id2);
        BCMDNX_IF_ERR_EXIT(rv);

        *is_equal = ((dest_id1.dest_type == dest_id2.dest_type) && (dest_id1.dest_val == dest_id2.dest_val));

        break;

    case _bcmDppGportCompareTypeInLif:
        rv = _bcm_dpp_gport_to_hw_resources(unit, gport1, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        lif1 = gport_hw_resources.global_in_lif;

        rv = _bcm_dpp_gport_to_hw_resources(unit, gport2, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        lif2 = gport_hw_resources.global_in_lif;
        *is_equal = (lif1 == lif2);

        break;

    case _bcmDppGportCompareTypeOutLif:
        rv = _bcm_dpp_gport_to_hw_resources(unit, gport1, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        lif1 = gport_hw_resources.global_out_lif;

        rv = _bcm_dpp_gport_to_hw_resources(unit, gport2, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        lif2 = gport_hw_resources.global_out_lif;
        *is_equal = (lif1 == lif2);

        break;

    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unknown compare type")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    _bcm_dpp_gport_to_sand_pp_dest
 * Description:
 *    convert gport from soc_sand-pp-destination
 * Parameters:
 *    unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  dest_id -       [IN] SOC_SAND destination
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_from_sand_pp_dest(int unit, bcm_gport_t *gport, SOC_SAND_PP_DESTINATION_ID  *dest_id)
{
    SOC_PPD_FRWRD_DECISION_INFO  fwd_decision;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(dest_id);
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_sand_dest_to_fwd_decision(soc_sand_dev_id,dest_id,&fwd_decision);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _bcm_dpp_gport_from_fwd_decision(unit,gport,&fwd_decision);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    _bcm_dpp_gport_to_tm_dest_info
 * Description:
 *    convert gport from TM dest information
 * Parameters:
 *  unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [OUT] general port
 *  soc_petra_dest_info - [OUT] Soc_petra destination info
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_from_tm_dest_info(int unit, bcm_gport_t *gport, SOC_TMC_DEST_INFO  *soc_petra_dest_info)
{
    int rv = BCM_E_NONE;
    bcm_module_t modid;
    bcm_port_t mode_port;

    BCMDNX_INIT_FUNC_DEFS;
    /* verify input parameters */
    BCMDNX_NULL_CHECK(soc_petra_dest_info);
    BCMDNX_NULL_CHECK(gport);
    
    switch(soc_petra_dest_info->type) {
    case SOC_TMC_DEST_TYPE_SYS_PHY_PORT:
        /* map system port to mod-port*/
        rv = _bcm_dpp_port_mod_port_from_sys_port(unit,&modid,&mode_port,soc_petra_dest_info->id);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_GPORT_MODPORT_SET(*gport,modid,mode_port);
        break;
    case SOC_TMC_DEST_TYPE_MULTICAST:
        BCM_GPORT_MCAST_SET(*gport,soc_petra_dest_info->id);
        break;
    case SOC_TMC_DEST_TYPE_LAG:
        BCM_GPORT_TRUNK_SET(*gport,soc_petra_dest_info->id);
        break;
    case SOC_TMC_DEST_TYPE_QUEUE:
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(*gport,soc_petra_dest_info->id);
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unknown destination type")));

    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    _bcm_dpp_gport_to_tm_dest_info
 * Description:
 *    convert gport to TM dest information
 * Parameters:
 *    unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  soc_petra_dest_info - [OUT] Soc_petra destination info
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_to_tm_dest_info(int unit, bcm_gport_t port, SOC_TMC_DEST_INFO  *soc_petra_dest_info)
{
    bcm_module_t    _modid;
    bcm_port_t      _mode_port;
    uint32       tm_port;
    uint32       _sys_phy_port_id = 0;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE, core;
    bcm_gport_t gport;

    BCMDNX_INIT_FUNC_DEFS;
    if (NULL == soc_petra_dest_info){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NULL output pointer")));
    }

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else if (SOC_PORT_VALID(unit, port)) { 
        BCM_GPORT_LOCAL_SET(gport,port);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("invalid port given as input"))); 
    }
    
    if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_MODPORT(gport)) { /* local-port or mod-port */
        BCMDNX_IF_ERR_EXIT(bcm_petra_stk_gport_sysport_get(unit, port, &gport));
        soc_petra_dest_info->type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;
        soc_petra_dest_info->id = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);
    }
    else if(BCM_GPORT_IS_TRUNK(gport)){  /*trunk*/
        soc_petra_dest_info->type = SOC_TMC_DEST_TYPE_LAG;
        soc_petra_dest_info->id = BCM_GPORT_TRUNK_GET(gport);
    }
    else if(BCM_GPORT_IS_SYSTEM_PORT(gport)){  /*sytem port*/
        soc_petra_dest_info->type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;
        soc_petra_dest_info->id = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);
    }
    else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) { /* COSQ / Queue */
        soc_petra_dest_info->type = SOC_TMC_DEST_TYPE_QUEUE;
        soc_petra_dest_info->id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        soc_petra_dest_info->type = SOC_TMC_DEST_TYPE_QUEUE;
        soc_petra_dest_info->id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
    }
    else if(BCM_GPORT_IS_MCAST(gport)) {
        soc_petra_dest_info->type = SOC_TMC_DEST_TYPE_MULTICAST;
        soc_petra_dest_info->id = _BCM_MULTICAST_ID_GET(BCM_GPORT_MCAST_GET(gport));
    }
    else if(BCM_GPORT_IS_BLACK_HOLE(gport)) {
        soc_petra_dest_info->type = SOC_TMC_DEST_TYPE_QUEUE;
        soc_petra_dest_info->id = 0; /* drop the packet */
    }
    else if(BCM_GPORT_IS_LOCAL_CPU(gport)){ /* CPU-port*/
        rv = bcm_petra_stk_my_modid_get(unit, &_modid);
        BCMDNX_IF_ERR_EXIT(rv);
        _mode_port = CMIC_PORT(unit);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, _mode_port, &tm_port, &core)));
        /* get physical system port, identify <mod,port>*/
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_local_to_sys_phys_port_map_get,(unit,_modid,tm_port,&_sys_phy_port_id)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        soc_petra_dest_info->type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;
        soc_petra_dest_info->id = _sys_phy_port_id;
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Gport given can not be parsed as TM information")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    _bcm_dpp_l2_gport_fill_fec
 * Description:
 *    given L2 gport (VLAN port) or Egress interface that is protected and FEC_id, fill FEC entry, also update protection info,
 * Parameters:
 *    unit -           [IN] DPP device unit number (driver internal).
 *  vlan_port -          [IN] vlan port
 *  fec_id -       [IN] where to set FEC 
 * Returns:
 *    BCM_E_XXX
 */
int _bcm_dpp_l2_gport_fill_fec(int unit, void *gport, int gport_type, SOC_PPD_FEC_ID fec_id, SOC_PPD_FRWRD_FEC_ENTRY_INFO *work_fec){
    int sec_fec;
    SOC_PPD_FEC_ID protect_fec_id,work_fec_id;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO
        fec_entry[2],
        protec_fec;
    SOC_PPD_FRWRD_FEC_PROTECT_INFO
        protect_info;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE               
        protect_type;
    uint32 soc_sand_rv;
    uint8 success;
    int rv = BCM_E_NONE;
    int fec_encap = 0;
    unsigned int soc_sand_dev_id;
    bcm_vlan_port_t *vlan_port = (bcm_vlan_port_t*)(gport);
    bcm_mpls_port_t *mpls_port = (bcm_mpls_port_t*)(gport);
    bcm_mim_port_t *mim_port = (bcm_mim_port_t*)(gport);
    bcm_l3_egress_t *egress_if = (bcm_l3_egress_t*)(gport);
    bcm_failover_t failover_id;
    int is_work=0;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    if(gport_type == BCM_GPORT_VLAN_PORT) {
        failover_id = (vlan_port)->failover_id;
       /* For 2nd forward group AC, is_work always is 0 */
        if ((vlan_port->flags & BCM_VLAN_PORT_FORWARD_GROUP) &&
            vlan_port->failover_port_id) {
            is_work = 0;
        }
        else {
            is_work = _BCM_PPD_GPORT_IS_WORKING_PORT(vlan_port);
        }

        if (work_fec != NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Internal error: in case gport_type is not egress_if work_fec must be null")));
        }
    }
    else if (gport_type == BCM_GPORT_MPLS_PORT) {

        /* Validity check - In Arad we don't support destination FEC. In Jericho we support it for H-Protectopn*/
        if (BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if) && !SOC_IS_JERICHO(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: mpls_port->egress_tunnel_if parameter can not be FEC.")));
        }

        failover_id = (mpls_port)->failover_id;
       /* For 2nd forward group MPLS, is_work always is 0 */
        if ((mpls_port->flags & BCM_MPLS_PORT_FORWARD_GROUP) &&
            mpls_port->failover_port_id) {
            is_work = 0;
        }
        else {
            is_work = _BCM_PPD_GPORT_IS_WORKING_PORT(mpls_port);
        }

        if (work_fec != NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Internal error: in case gport_type is not egress_if work_fec must be null")));
        }
    }
    else if (gport_type == BCM_GPORT_MIM_PORT) {
        failover_id = (mim_port)->failover_id;
        is_work = (mim_port->failover_gport_id != BCM_GPORT_TYPE_NONE);

        if (work_fec != NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Internal error: in case gport_type is not egress_if work_fec must be null")));
        }
    }
    else if (gport_type == BCM_DPP_GPORT_TYPE_EGRESS_IF) {
        failover_id = (egress_if)->failover_id;
        is_work = (egress_if->failover_if_id != BCM_GPORT_TYPE_NONE);
        if (work_fec == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Internal error: in case gport_type is egress_if work_fec must be given")));
        }
    }
    else{
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Gport given can not be parsed as FEC information")));
    }

    /* if second member in protection*/
    if (is_work) {
        if(gport_type == BCM_GPORT_VLAN_PORT) {
            protect_fec_id =  BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->failover_port_id);
        }
        else if (gport_type == BCM_GPORT_MIM_PORT) {
               protect_fec_id =  BCM_GPORT_MIM_PORT_ID_GET(mim_port->failover_gport_id);
        }
        else if (gport_type == BCM_GPORT_MPLS_PORT)/* MPLS port*/
        {
            protect_fec_id =  BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->failover_port_id);
        }
        else /*egress intf */
        {
            protect_fec_id = BCM_L3_ITF_VAL_GET(egress_if->failover_if_id);
        }

        work_fec_id = _BCM_PPD_GPORT_PROTECT_TO_WORK_FEC(protect_fec_id);
        sec_fec = 1;
    }
    else /* first gport in protection*/
    {
        protect_fec_id =  fec_id;
        work_fec_id = _BCM_PPD_GPORT_PROTECT_TO_WORK_FEC(protect_fec_id);
        sec_fec = 0;
    }

    /* Fill FEC info*/
    SOC_PPD_FRWRD_FEC_ENTRY_INFO_clear(&fec_entry[0]);
    SOC_PPD_FRWRD_FEC_ENTRY_INFO_clear(&fec_entry[1]);

    if (sec_fec) {
        /* read first*/
        /*
         * COVERITY
         */
        /* No overrun. if protect_fec_id is -1 - 
         * The function soc_pb_pp_frwrd_fec_entry_get_verify will fail and will avoid the access to the array with invalid index.
         */
        /* covberity[overrun-call:FALSE] */
        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,protect_fec_id,&protect_type,&fec_entry[1],&protec_fec,&protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (fec_entry[1].type == SOC_PPC_NOF_FEC_TYPES_ARAD) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Protected FEC must be configured before the primary FEC.")));
        }
    }

    /* In case of Egress interface copy all information given from work_fec to fec_entry[0] */
    if (gport_type == BCM_DPP_GPORT_TYPE_EGRESS_IF) {
        sal_memcpy(&fec_entry[0],work_fec,sizeof(SOC_PPD_FRWRD_FEC_ENTRY_INFO));
    }

    /* map gport to destination */
    if(gport_type == BCM_GPORT_VLAN_PORT) {
        rv = _bcm_dpp_gport_to_sand_pp_dest(unit, (vlan_port)->port, &(fec_entry[0].dest));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if (gport_type == BCM_GPORT_MPLS_PORT) {
        if (SOC_IS_JERICHO(unit) && BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if)) { /* H-Protection VPLS */
            SOC_PPD_FRWRD_DECISION_INFO  fwd_decision;

            /* Create forward decision of type FEC with MPLS FEC as destination */
            SOC_PPD_FRWRD_DECISION_INFO_clear(&fwd_decision);
            fwd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
            fwd_decision.dest_id = BCM_L3_ITF_VAL_GET(mpls_port->egress_tunnel_if);

            soc_sand_rv = soc_ppd_fwd_decision_to_sand_dest(soc_sand_dev_id, &fwd_decision, &(fec_entry[0].dest));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else {
            rv = _bcm_dpp_gport_to_sand_pp_dest(unit, (mpls_port)->port, &(fec_entry[0].dest));
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else if (gport_type == BCM_GPORT_MIM_PORT) {
        rv = _bcm_dpp_gport_to_sand_pp_dest(unit, (mim_port)->port, &(fec_entry[0].dest));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if (gport_type == BCM_DPP_GPORT_TYPE_EGRESS_IF) {
        /* Nothing to do since destination is already given by work_fec */
    }

    /* if vlan-port, or access side in MPLS port*/
    /* attach AC to the port*/
    if (gport_type == BCM_GPORT_VLAN_PORT) {
        
        /* encap-id = AC */
        fec_encap = BCM_ENCAP_ID_GET((vlan_port)->encap_id);

        /* set FEC entry type and encap id */
        /* ARAD FEC type is tunnel (or forward when using out-lif profile) */
        if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id))) {
            fec_entry[0].type = SOC_PPD_FEC_TYPE_FORWARD;
        } else {
            fec_entry[0].type = SOC_PPD_FEC_TYPE_TUNNELING;
        }
        fec_entry[0].eep = fec_encap;
    
    }
    else if ((gport_type == BCM_GPORT_MPLS_PORT) || (gport_type == BCM_GPORT_MIM_PORT))
    {
        if(gport_type == BCM_GPORT_MIM_PORT) {
            fec_entry[0].eep = BCM_ENCAP_ID_GET(mim_port->encap_id);/*encap-id = EEP */
        }
        else {
            fec_entry[0].eep = BCM_ENCAP_ID_GET(mpls_port->encap_id);/*encap-id = EEP */
        }
        if (SOC_IS_ARAD(unit)) {
            fec_entry[0].type = SOC_PPD_FEC_TYPE_TUNNELING;
        }
        else { /* Soc_petra-B */
            fec_entry[0].type = SOC_PPD_FEC_TYPE_BRIDGING_INTO_TUNNEL;
        }
    }
    else /* egress intf */
    {
        /* Nothing to do since fec_type,eep,intf is already given by work_fec */
    }

    /* calc protection info, for both: even and odd FEC*/
    SOC_PPD_FRWRD_FEC_PROTECT_INFO_clear(&protect_info);

    if (failover_id == BCM_FAILOVER_ID_LOCAL) {
        protect_type = SOC_PPD_FRWRD_FEC_PROTECT_TYPE_FACILITY;
    } else {
        DPP_FAILOVER_ID_GET(protect_info.oam_instance_id, failover_id);
        protect_type = (protect_info.oam_instance_id == 0) ? SOC_PPD_FRWRD_FEC_PROTECT_TYPE_NONE : SOC_PPD_FRWRD_FEC_PROTECT_TYPE_PATH;
    }

    if (sec_fec) {
        soc_sand_rv = soc_ppd_frwrd_fec_entry_add(soc_sand_dev_id,work_fec_id,protect_type,&fec_entry[0],&fec_entry[1],&protect_info,&success);
    }
    else
    {
        /* as this odd, entry only odd entry will be set */ 
        soc_sand_rv = soc_ppd_frwrd_fec_entry_add(soc_sand_dev_id,protect_fec_id,protect_type,&fec_entry[0],&fec_entry[0],&protect_info,&success);
    }
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if(!success) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("FEC table is full or unexpected failure in FEC table")));
    }
    /* store FEC in gport */
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_l2_fec_fill_gport(
    int unit, 
    SOC_PPD_FEC_ID fec_id, 
    void *gport, 
    int gport_type)
{
    int rv = BCM_E_NONE;
    SOC_PPD_FEC_ID protect_fec_id, work_fec_id;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO fec_entry;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE protect_type;
    uint32 soc_sand_rv;
    bcm_vlan_port_t *vlan_port = (bcm_vlan_port_t*)(gport);
    bcm_mpls_port_t *mpls_port = (bcm_mpls_port_t*)(gport);
    bcm_mim_port_t *mim_port = (bcm_mim_port_t*)(gport);
    int is_work = 0;
    bcm_failover_t failover_id;

    BCMDNX_INIT_FUNC_DEFS;

    if(gport_type == BCM_GPORT_VLAN_PORT) {
        is_work = _BCM_PPD_GPORT_IS_WORKING_PORT_ID(vlan_port->vlan_port_id);
    }
    else if (gport_type == BCM_GPORT_MPLS_PORT) {
        is_work = _BCM_PPD_GPORT_IS_WORKING_PORT_ID(mpls_port->mpls_port_id);
    }
    else if (gport_type == BCM_GPORT_MIM_PORT) {
        is_work = _BCM_PPD_GPORT_IS_WORKING_PORT_ID(mim_port->mim_port_id);
    }
    else{
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport can not be parsed correctly in FEC table")));
    }

    rv = _bcm_ppd_frwrd_fec_entry_from_hw(unit, fec_id, &protect_type, &fec_entry, &failover_id);
    BCM_SAND_IF_ERR_EXIT(rv);


    if (protect_type != SOC_PPD_FRWRD_FEC_PROTECT_TYPE_NONE) {
        /* check if second member in protection*/
        if (is_work) {
            if (fec_id % 2) {
                protect_fec_id = fec_id;
            }
            else {
                work_fec_id = fec_id;
                protect_fec_id = _BCM_PPD_GPORT_WORK_TO_PROTECT_FEC(work_fec_id);
            }

            if (gport_type == BCM_GPORT_VLAN_PORT) {
                BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->failover_port_id, protect_fec_id);
            }
            else if (gport_type == BCM_GPORT_MIM_PORT) {
                BCM_GPORT_MIM_PORT_ID_SET(mim_port->failover_gport_id, protect_fec_id);
            }
            else /* MPLS port*/
            {
                BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->failover_port_id, protect_fec_id);
            }
        }
    }

    /* map destination to gport */
    if (gport_type == BCM_GPORT_VLAN_PORT) {
        rv = _bcm_dpp_gport_from_sand_pp_dest(unit, &((vlan_port)->port), &(fec_entry.dest));
    }
    else if (gport_type == BCM_GPORT_MPLS_PORT) {
        if (SOC_IS_JERICHO(unit) && (fec_entry.dest.dest_type == SOC_SAND_PP_DEST_FEC)) { /* H-Protection VPLS */
            SOC_PPD_FRWRD_DECISION_INFO  fwd_decision;

            soc_sand_rv = soc_ppd_sand_dest_to_fwd_decision(unit, &(fec_entry.dest), &fwd_decision);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            BCM_L3_ITF_SET(mpls_port->egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, fwd_decision.dest_id);
        }
        else {
            rv = _bcm_dpp_gport_from_sand_pp_dest(unit, &((mpls_port)->port), &(fec_entry.dest));
        }
    }
    else { /* MIM port */
        rv = _bcm_dpp_gport_from_sand_pp_dest(unit, &((mim_port)->port), &(fec_entry.dest));
    }
    BCMDNX_IF_ERR_EXIT(rv);

    /* if vlan-port, get port from attached AC */
    if (fec_entry.type == SOC_PPD_FEC_TYPE_BRIDGING_WITH_AC) {
        (vlan_port)->encap_id = fec_entry.app_info.out_ac_id; /*encap-id = AC */
    }
    else {
        
        if (gport_type == BCM_GPORT_VLAN_PORT) {/* only for ARAD */
            (vlan_port)->encap_id = fec_entry.eep; /*encap-id = AC */
        }
        else if (gport_type == BCM_GPORT_MPLS_PORT) {/* only for ARAD */
            (mpls_port)->encap_id = fec_entry.eep; /*encap-id = AC */
        }
        else if (gport_type == BCM_GPORT_MIM_PORT) {
            mim_port->encap_id = fec_entry.eep; /*encap-id = EEP */
        }
    }

    if (gport_type == BCM_GPORT_VLAN_PORT) {
        (vlan_port)->failover_id = failover_id;
    } else if (gport_type == BCM_GPORT_MIM_PORT) {
        (mim_port)->failover_id = failover_id;
    } else { /* MPLS */
        (mpls_port)->failover_id = failover_id;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_gport_l2_intf_calculate_act_fec
 * Purpose:
 *       Calculate the fec that will be used to set the port_id. The fec will come from either the port_id
 *       or from failover_id. If neither fields hold a valid fec, the returned fec will be 0.
 * Parameters:
 *      gport       : (IN)  The vlan/mpls port, called with vlan/mpls_port_create, for which the fec will be calculated.
 *      act_fec     : (OUT) Will be filled with the fec_id, or 0 if the vlan port doesn't contain viable
 *                          fec id.
 *      gport_type  : (IN) port type (MPLS/VLAN)
 * Returns: 
 *      BCM_E_PARAM : If  BCM_VLAN_PORT_WITH_ID flag was set, but port_id wasn't set.
 *      BCM_E_NONE  : Otherwise.
 */
int _bcm_dpp_gport_l2_intf_calculate_act_fec(void *gport, int *act_fec, int gport_type){

    int protect_fec_id,work_fec_id, fec_id;
    uint8 with_id, is_work;
    bcm_gport_t port_id;
    bcm_vlan_port_t *vlan_port = (bcm_vlan_port_t*)(gport);
    bcm_mpls_port_t *mpls_port = (bcm_mpls_port_t*)(gport);

    with_id = (gport_type == BCM_GPORT_VLAN_PORT) ? (vlan_port->flags & BCM_VLAN_PORT_WITH_ID) : (mpls_port->flags & BCM_MPLS_PORT_WITH_ID);
    port_id = (gport_type == BCM_GPORT_VLAN_PORT) ? (vlan_port->vlan_port_id) : (mpls_port->mpls_port_id);
    is_work = (gport_type == BCM_GPORT_VLAN_PORT) ? _BCM_PPD_GPORT_IS_WORKING_PORT_ID(vlan_port->vlan_port_id) : (mpls_port->failover_port_id != BCM_GPORT_TYPE_NONE);

    *act_fec = 0; /* If neither conditions apply, it will be allocated later */

    /* If with id, then allocate in same place */
    if(with_id) {
        if (!port_id) { /* Illegal to allocate fec WITH_ID without providing an ID. */
            return BCM_E_PARAM;
        }
        fec_id = (gport_type == BCM_GPORT_VLAN_PORT) ? BCM_GPORT_VLAN_PORT_ID_GET(port_id) : BCM_GPORT_MPLS_PORT_ID_GET(port_id);        
        *act_fec = fec_id;
    }

    /* If second member in protection already allocated, it will hold the active fec. */
    if (is_work) {
        protect_fec_id =  (gport_type == BCM_GPORT_VLAN_PORT) ? BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->failover_port_id) : BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->failover_port_id);
        work_fec_id = _BCM_PPD_GPORT_PROTECT_TO_WORK_FEC(protect_fec_id);
        *act_fec = work_fec_id;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_dpp_gport_l2_intf_calculate_port_id
 * Purpose:
 *      Given different potential componenets of the port, sets port_id according to its type.
 *      Not all arguments must be given, only one legal combination.
 * Parameters: 
 *      unit                    : (IN) The unit on which the port will be created. 
 *      act_fec                 : (IN) fec for port_id in case of forward group or 1:1 protection
 *      lif_id                  : (IN) lif id for port_id if first three arguments are 0
 *      gport                   : (INOUT) mpls/vlan port
 *          ->flags             : (IN)  Relevant flags are BCM_VLAN/MPLS_PORT_CREATE_EGRESS_ONLY, BCM_VLAN/MPLS_PORT_CREATE_INGRESS_ONLY
 *                                          and BCM_VLAN/MPLS_PORT_FORWARD_GROUP.
 *          ->failover_mc_group : (IN)  Used to test if it's 1+1 protection, and then the MC should be used for id.
 *          ->failover_id       : (IN)  Used to test if it's 1:1 protection, and theb the fec should be used for id.
 *          ->vlan/mpls_port_id      : (OUT) Will be filled with the final vlan/mpls_port_id.
 *      gport_type - (IN) port type (MPLS/VLAN)
 * Returns: 
 *      BCM_E_PARAM : If the act_fec wasn't filled for protection or forward group ports, or if lif_id wasn't filled
 *                      for lif ports.
 *      BCM_E_NONE
 */
int
_bcm_dpp_gport_l2_intf_calculate_port_id(int unit, int act_fec, SOC_PPD_LIF_ID lif_id, void *gport, int gport_type){
    int gport_id;
    uint8 is_ingress, is_egress;
    int is_protected;
    uint8 is_forward_group, is_mc_ingress = 0;
    uint8 lif_gport;
    bcm_multicast_t failover_mc_group = 0;
    bcm_vlan_port_t *vlan_port = (bcm_vlan_port_t*)(gport);
    bcm_mpls_port_t *mpls_port = (bcm_mpls_port_t*)(gport);
    bcm_extender_port_t *extender_port = (bcm_extender_port_t*)(gport);
    int is_work = 0;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    if((gport_type != BCM_GPORT_VLAN_PORT) && (gport_type != BCM_GPORT_MPLS_PORT) && (gport_type != BCM_GPORT_EXTENDER_PORT)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Wrong gport type input to _bcm_dpp_gport_l2_intf_calculate_port_id function")));
    }

    if (gport_type != BCM_GPORT_EXTENDER_PORT) {
        rv = _bcm_dpp_gport_l2_intf_ingress_egress_only_validation(gport, FALSE, &is_ingress, &is_egress, gport_type);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_gport_l2_intf_protection_validation(unit, gport, gport_type, FALSE, &is_protected, NULL, &is_mc_ingress, NULL, NULL);
        BCMDNX_IF_ERR_EXIT(rv);

        failover_mc_group = (gport_type == BCM_GPORT_VLAN_PORT) ? vlan_port->failover_mc_group : mpls_port->failover_mc_group;

        is_forward_group = (gport_type == BCM_GPORT_VLAN_PORT) ? ((vlan_port->flags & BCM_VLAN_PORT_FORWARD_GROUP) == BCM_VLAN_PORT_FORWARD_GROUP) : ((mpls_port->flags & BCM_MPLS_PORT_FORWARD_GROUP) == BCM_MPLS_PORT_FORWARD_GROUP);

        is_work = (gport_type == BCM_GPORT_VLAN_PORT) ? _BCM_PPD_GPORT_IS_WORKING_PORT(vlan_port) : _BCM_PPD_GPORT_IS_WORKING_PORT(mpls_port);

        lif_gport = (!is_forward_group && !is_protected && !is_mc_ingress);

        /* Input validation*/
        if (   ((is_forward_group || is_protected) && !act_fec)             /* Fec gport but no fec */
            || (is_mc_ingress && ! failover_mc_group)                       /* MC gport but no MC group */
            || (lif_gport && (lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID))   /* Lif gport but no lif_id */
            || (!is_ingress && !is_egress)                                  /* Illegal combination */
            ){
            return BCM_E_NONE;
        }
    } else {
        if (lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            return BCM_E_NONE;
        }

        is_forward_group = is_protected = is_mc_ingress = FALSE;
        is_ingress = is_egress = TRUE;
    }

    /* Determine gport_id */
    if(is_forward_group) {
        /* For port of type forward, the port id is equal to the FEC ID, and the type is forward */
        BCM_GPORT_SUB_TYPE_FORWARD_GROUP_SET( gport_id , act_fec);
    }
    else if (is_protected) {
        gport_id = act_fec;
    }
    else if(is_mc_ingress) {
        _bcm_dpp_gport_plus_1_mc_to_id(failover_mc_group,is_work,&gport_id);
    }
    else
    {
        if (!is_ingress) {
            BCM_GPORT_SUB_TYPE_LIF_SET( gport_id,BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY,lif_id);
        }
        else if (!is_egress) {
            BCM_GPORT_SUB_TYPE_LIF_SET( gport_id,BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,lif_id);
        } else {
            BCM_GPORT_SUB_TYPE_LIF_SET( gport_id,0 ,lif_id);
        }
    }

    /* Set vlan_port_id */
    switch (gport_type) {
    case BCM_GPORT_VLAN_PORT:
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, gport_id);
        break;
    case BCM_GPORT_MPLS_PORT:
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id, gport_id);
        break;
    case BCM_GPORT_EXTENDER_PORT:
        BCM_GPORT_EXTENDER_PORT_ID_SET(extender_port->extender_port_id, gport_id);
        break;
    }

    return BCM_E_NONE;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_gport_l2_intf_get_lif_from_port_id
 * Purpose:
 *      When creating a vlan/mpls/extender port WITH_ID (not ENCAP_WITH_ID), use this function to get the used lif.
 *      If port is protected or mc_ingress, the vlan_port_id will stand for fec and mc_group respectivly, and not for lif.
 * Parameters: 
 *      gport               : (IN) the vlan/mpls_port/extender->vlan/mpls/extender_port_id 
 *      is_protected        : (IN) is the port 1:1 protected?
 *      is_mc_ingress       : (IN) is the port 1+1 prtected?
 *      lif_id              : (OUT) the final lif_id. If port id is not based on lif (protected) will be an invalid value.
 *      gport_type          : (IN) port type (MPLS/VLAN/EXTENDER) 
 * Returns: 
 *      BCM_E_NONE
 */
int _bcm_dpp_gport_l2_intf_get_lif_from_port_id(void *gport, uint8 is_protected, uint8 is_mc_ingress, SOC_PPD_LIF_ID *lif_id, int gport_type){

    bcm_vlan_port_t *vlan_port;
    bcm_mpls_port_t *mpls_port;
    bcm_extender_port_t *extender_port;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    /* Extract the object ID according to the object type */
    switch (gport_type) {
    case BCM_GPORT_VLAN_PORT:
        vlan_port = (bcm_vlan_port_t*)(gport);
        *lif_id = BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id);
        break;
    case BCM_GPORT_MPLS_PORT:
        mpls_port = (bcm_mpls_port_t*)(gport);
        *lif_id = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
        break;
    case BCM_GPORT_EXTENDER_PORT:
        extender_port = (bcm_extender_port_t*)(gport);
        *lif_id = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port->extender_port_id);
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("Got wrong type in _bcm_dpp_gport_l2_intf_get_lif_from_port_id - %d"), gport_type));
    }

    /* lif encoded or unencoded and there is no protection then id is for LIF id*/
    if (BCM_GPORT_SUB_TYPE_IS_LIF(*lif_id) ){
        *lif_id = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(*lif_id);
    }

    else if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(*lif_id)) {
        *lif_id = BCM_GPORT_SUB_TYPE_FORWARD_GROUP_GET(*lif_id);
    }

    else if (!(BCM_GPORT_SUB_TYPE_IS_PROTECTION(*lif_id) && !is_protected && !is_mc_ingress)){
        *lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    }

exit:
    BCMDNX_FUNC_RETURN_NO_UNIT;
}

/*
 * Function:
 *      _bcm_dpp_gport_l2_intf_ingress_egress_only_validation
 * Purpose:
 *      Given a vlan port struct, returns wether the vlan port is ingress only, egress only or neither.
 * Parameters: 
 *      gport          : (IN)  mpls/vlan port
 *          ->flags    : (IN)  Relevant flags are BCM_VLAN/MPLS_PORT_CREATE_EGRESS_ONLY and BCM_VLAN/MPLS_PORT_CREATE_INGRESS_ONLY.
 *      validate       : (IN)  Whether to validate the input, or just fill the fields. 
 *      is_ingress     : (OUT) Whether the port exists in ingress.
 *      is_egress      : (OUT) Whether the port exists in egress.
 *      gport_type     : (IN) port type (MPLS/VLAN)  
 * Returns:
 *      BCM_E_PARAM : If validate is on and either:
 *                      * Port is neither ingress nor egress
 *                      * The port is egress only with a port match other than none.
 *      BCM_E_NONE
 */
int _bcm_dpp_gport_l2_intf_ingress_egress_only_validation(void *gport, uint8 validate, uint8 *is_ingress, uint8 *is_egress, int gport_type){

    bcm_vlan_port_t *vlan_port = (bcm_vlan_port_t*)(gport);
    bcm_mpls_port_t *mpls_port = (bcm_mpls_port_t*)(gport);

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    if(gport_type == BCM_GPORT_VLAN_PORT) {
        DPP_IF_NOT_NULL_FILL(is_ingress, (vlan_port->flags & BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE);
        DPP_IF_NOT_NULL_FILL(is_egress, (vlan_port->flags & BCM_VLAN_PORT_CREATE_INGRESS_ONLY)? FALSE : TRUE);
    }
    else if (gport_type == BCM_GPORT_MPLS_PORT) {
        DPP_IF_NOT_NULL_FILL(is_ingress, (mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY) ? FALSE : TRUE);
        DPP_IF_NOT_NULL_FILL(is_egress, (mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY)? FALSE : TRUE);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Wrong gport type input to _bcm_dpp_gport_l2_intf_ingress_egress_only_validation function")));
    }

    if (validate && is_egress /* tests are not relevant if is_egress is null */) {
        if (gport_type == BCM_GPORT_VLAN_PORT) {
                /* In case of Egress only the match criteria has to be NONE */
            if ((!*is_ingress) && (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NONE)) {
                BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("An Egress only VLAN Port has to be with match criteria NONE")));
            }
        }

        if (is_ingress) {
            /* A port has to be either ingress or egress */
            if ((!*is_ingress) && (!*is_egress)) {
                BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("The Port can't be both Ingress only and Egress only")));
            }        
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_gport_l2_intf_protection_validation
 * Purpose:
 *      Given a vlan/mpls port struct, returns the protection type of the port, and validates that it's legal.
 * Parameters: 
 *      unit                    : (IN)  The unit on which the port will be created. 
 *      gport                   : (IN)  Vlan/mpls port struct to be tested
 *          ->failover_id       : (IN)  If set, marks 1:1 protection.
 *          ->failover_port_id  : (IN)  Protected port for 1:1 protection. 
 *          ->ingress_failover_id:(IN)  If set, marks 1+1 ingress protection (only for vlan port)
 *          ->failover_mc_group : (IN)  If set, makrs 1+1 MC protected.
 *      gport_type              : (IN)  port type (MPLS/VLAN)
 *      validate                : (IN)  Whether to validate the input, or just fill the fields. 
 *      is_protected            : (OUT) Whether the port is 1:1 protected
 *      is_ingress_protected    : (OUT) Whether the port is 1+1 ingress protected.
 *      is_mc_ingress           : (OUT) Whether the port is 1+1 MC protected.
 *      is_1_plus_1             : (OUT) Whether the port is 1+1 protected.
 *      is_egress_protected     : (OUT) Whether the port has egress protection.
 * Returns:                
 *      BCM_E_PARAM : If validate is on and either:               
 *                      * Both 1+1 and 1:1 protections are on.
 *                      * Port is working port of 1:1 protection, but failover port is not protected.
 *                      * MPLS MC Group supplied without a failover ID
 *      BCM_E_NONE
 */
int _bcm_dpp_gport_l2_intf_protection_validation(int unit, void *gport, int gport_type, uint8 validate, int *is_1_to_1, uint8 *is_ingress_protected, 
                                           uint8 *is_mc_ingress, uint8 *is_1_plus_1, uint8 *is_egress_protected){
    int rv = BCM_E_NONE;
    bcm_failover_t failover_id;
    bcm_multicast_t failover_mc_group;
    bcm_vlan_port_t *vlan_port = (bcm_vlan_port_t*)(gport);
    bcm_mpls_port_t *mpls_port = (bcm_mpls_port_t*)(gport);
    bcm_gport_t failover_port_id, protecting_out_lif_id;
    uint8 is_work;
    bcm_failover_t egress_failover_id, egress_failover_id_val;
    int protecting_local_out_lif_id;
    bcm_dpp_am_local_out_lif_info_t protecting_local_out_lif_info;

    BCMDNX_INIT_FUNC_DEFS;

    failover_id = (gport_type == BCM_GPORT_VLAN_PORT) ? vlan_port->failover_id : mpls_port->failover_id;
    failover_mc_group = (gport_type == BCM_GPORT_VLAN_PORT) ? vlan_port->failover_mc_group : mpls_port->failover_mc_group;
    failover_port_id = (gport_type == BCM_GPORT_VLAN_PORT) ? vlan_port->failover_port_id : mpls_port->failover_port_id;
    is_work = (gport_type == BCM_GPORT_VLAN_PORT) ? _BCM_PPD_GPORT_IS_WORKING_PORT(vlan_port) : _BCM_PPD_GPORT_IS_WORKING_PORT(mpls_port);
    egress_failover_id = (gport_type == BCM_GPORT_VLAN_PORT) ? vlan_port->egress_failover_id : mpls_port->egress_failover_id;

    /* Check if 1+1 protection is tried */
    
    if (gport_type == BCM_GPORT_VLAN_PORT) {
        /* Protection in ingress path */
        DPP_IF_NOT_NULL_FILL(is_ingress_protected, _BCM_PPD_IS_VALID_FAILOVER_ID(vlan_port->ingress_failover_id));
    }

    /* Protection by mulitcast on reverse path */
    DPP_IF_NOT_NULL_FILL(is_mc_ingress, _BCM_MULTICAST_IS_SET(failover_mc_group));

    /* Use device (1+1) protection in both cases */
    DPP_IF_NOT_NULL_FILL(is_1_plus_1, DPP_VALUE_IF_NOT_NULL(is_mc_ingress) || DPP_VALUE_IF_NOT_NULL(is_ingress_protected));

    if (gport_type == BCM_GPORT_VLAN_PORT) {
        /* If this is 1 to 1 protected then my protecting fec had to be created with protection (failover-id)*/
        DPP_IF_NOT_NULL_FILL(is_1_to_1, _BCM_PPD_IS_VALID_FAILOVER_ID(failover_id));

        /* Egress protection is determined according to a flag */
        DPP_IF_NOT_NULL_FILL(is_egress_protected, ((vlan_port->flags & BCM_VLAN_PORT_EGRESS_PROTECTION) ? 1 : 0));
    }
    if (gport_type == BCM_GPORT_MPLS_PORT) {
        /* failover_id when a mc group is not given indicates 1:1 protection. */
        DPP_IF_NOT_NULL_FILL(is_1_to_1 ,_BCM_PPD_IS_VALID_FAILOVER_ID(failover_id) && !(DPP_VALUE_IF_NOT_NULL(is_mc_ingress)));

        /* Egress protection is determined according to a flag */
        DPP_IF_NOT_NULL_FILL(is_egress_protected, ((mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_PROTECTION) ? 1 : 0));
    }

    /* Perform all type of Protection validations, if required */
    if (validate) {
        /* Perform device related validations */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit) && DPP_VALUE_IF_NOT_NULL(is_egress_protected)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Egress protection is supported only from Jericho")));
        }

        /* Verify that the port is not both 1+1 protection and 1:1 protection  */
        if(DPP_VALUE_IF_NOT_NULL(is_1_plus_1) && DPP_VALUE_IF_NOT_NULL(is_1_to_1)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port can't be both 1+1 protected and 1:1 protected")));
        }

        /* If this is working FEC, then failover-port has to be protected otherwise return error */
        if(!(vlan_port->flags & BCM_VLAN_PORT_FORWARD_GROUP) && DPP_VALUE_IF_NOT_NULL(is_1_to_1) && is_work) {
            rv = _bcm_dpp_gport_is_protected(unit,failover_port_id,is_1_to_1);
            BCMDNX_IF_ERR_EXIT(rv);
            if(!*is_1_to_1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failover port ID must be a valid failover-port in case protection is set")));
            }
        }

        /* Perform failover ID value range validations */
        /* 1:1 Protection validations */
        if (DPP_VALUE_IF_NOT_NULL(is_1_to_1)) {
            rv = _bcm_dpp_failover_is_valid_id(unit, failover_id, DPP_FAILOVER_TYPE_FEC);
            BCMDNX_IF_ERR_EXIT(rv);

        /* VLAN Port 1+1 validations */
        } else if (gport_type == BCM_GPORT_VLAN_PORT) {
            if (DPP_VALUE_IF_NOT_NULL(is_ingress_protected)) {
                rv = _bcm_dpp_failover_is_valid_id(unit, vlan_port->ingress_failover_id, DPP_FAILOVER_TYPE_INGRESS);
                BCMDNX_IF_ERR_EXIT(rv);
            }

        /* MPLS Port 1+1 validations */
        } else if ((gport_type == BCM_GPORT_MPLS_PORT) && (DPP_VALUE_IF_NOT_NULL(is_1_plus_1))) {

            /* Verify that a failover ID was supplied */
            if (!(_BCM_PPD_IS_VALID_FAILOVER_ID(failover_id))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MPLS 1+1 protection requires a failover ID")));
            }

            /* Validate the failover ID */
            rv = _bcm_dpp_failover_is_valid_id(unit, failover_id, DPP_FAILOVER_TYPE_INGRESS);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Egress Protection validations */
        if (DPP_VALUE_IF_NOT_NULL(is_egress_protected)) {

            /* Verify that the supplied egress failover-id is valid */
            rv = _bcm_dpp_failover_is_valid_id(unit, egress_failover_id, DPP_FAILOVER_TYPE_ENCAP);
            BCMDNX_IF_ERR_EXIT(rv);

            /* This validation is in remark as it should be applied only when both egress
               protection Out-LIFs are local to the device. Therefore, it's less suitable
               for Jericho, but could be useful for Qumran */
            if (SOC_IS_QMX(unit)) {

                /* In case of a working port in Coupled mode, verify that the paired port
                   points to the same failover ID */
                if (SOC_DPP_IS_PROTECTION_EGRESS_COUPLED(unit)) {

                    /* Get the protecting Out-LIF */
                    protecting_out_lif_id = (gport_type == BCM_GPORT_VLAN_PORT) ?
                        vlan_port->egress_failover_port_id : mpls_port->egress_failover_port_id;

                    /* This validation is only for the working Out-LIF - the one that has a protecting Out-LIF */
                    if (protecting_out_lif_id) {

                        /* Get the local Out-LIF ID for the protecting, in order to find the attached failover ID */
                        rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, protecting_out_lif_id);
                        if (rv != BCM_E_EXISTS) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Egress protecting gport not allocated")));
                        }
                        rv =_bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS,
                                                                            protecting_out_lif_id, &protecting_local_out_lif_id);
                        BCMDNX_IF_ERR_EXIT(rv);

                        /* Get the protecting local Out-LIF stored information that includes the egress failover-id */
                        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.outlif_info.data_base.get(unit,
                                                       protecting_local_out_lif_id, &protecting_local_out_lif_info);
                        BCMDNX_IF_ERR_EXIT(rv);

                        /* Compare the protecting egress failover-id with the supplied one for the working out-lif */
                        DPP_FAILOVER_ID_GET(egress_failover_id_val, egress_failover_id);
                        if (egress_failover_id_val != protecting_local_out_lif_info.failover_id) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Egress protection failover IDs don't match for Working & backup ports")));
                        }
                    }
                }
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * given vpn and bcm-mpls-port 
 * fill the PWE information 
 */


#define _BCM_DPP_PORT_REPLACE               0x00000001 /* Replace existing
                                                          entry. */
#define _BCM_DPP_PORT_WITH_ID               0x00000002 /* Add using the
                                                          specified ID. */
#define _BCM_DPP_PORT_ENCAP_WITH_ID         0x00000004 
/* the peer port can be mpls-port only, (cannot be vlan_port) */
#define _BCM_DPP_PORT_SAME_PEER_ONLY        0x00000008
/* in case 2 ports of the VPN is connected and this is second to call to update the connection from the 2-peer to 1-peer  */

/* port-id is failover port */
#define _BCM_DPP_PORT_PORTECT               0x00000020

/* port-id is working port, protect_port_id is failover port */
#define _BCM_DPP_PORT_WORK          0x00000040



/*
 * return if the given gport, 
 * is FEC with remote LIF 
 */
int _bcm_dpp_gport_l2_intf_gport_has_remote_lif(
    int                  unit,
    bcm_gport_t          gport_id,
    uint8                *remote_lif
 )
{
    int rv = BCM_E_NONE;       
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    
    BCMDNX_INIT_FUNC_DEFS;

    *remote_lif = 0;

    /* port is not valid nothing to do */
    if(gport_id == BCM_GPORT_INVALID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid gport")));
    }

     /* check what is the type of gport  */
    rv = _bcm_dpp_gport_parse(unit,gport_id,&gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if gport is FEC, then check if LIF configured seperately, if so,
       then LIF configuration has to be done using the LIF gport.*/
    /* Remote is set only for VLAN_PORT and MPLS_PORT */
    if((gport_parse_info.type == _bcmDppGportParseTypeProtectedFec || gport_parse_info.type == _bcmDppGportParseTypeForwardFec) && (BCM_GPORT_IS_VLAN_PORT(gport_id) || BCM_GPORT_IS_MPLS_PORT(gport_id))) {
            rv = bcm_dpp_gport_mgmt_fec_remote_lif_get(unit, gport_parse_info.val, remote_lif);
            BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_lif_to_gport(
    int                 unit,
    int                 lif_id, /* InLIF ID */
    uint32              flags, 
    bcm_gport_t         *gport_id
 )
{
    bcm_error_t rv = BCM_E_NONE;
    _bcm_lif_type_e usage;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_LIF_ENTRY_INFO *lif_entry_info = NULL;
    SOC_PPD_FRWRD_DECISION_INFO* learn_info_ptr = NULL;
    uint8 from_learning = 0;    
    int gport_lif_used = 0;
    uint8 is_ingress, is_egress, is_local, is_global, sw_state;
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int local_lif = 0;

    BCMDNX_INIT_FUNC_DEFS;


    sw_state = DPP_IS_FLAG_SET(flags, _BCM_DPP_LIF_TO_GPORT_USE_SW_STATE);
    is_ingress = DPP_IS_FLAG_SET(flags, _BCM_DPP_LIF_TO_GPORT_INGRESS);
    is_egress = DPP_IS_FLAG_SET(flags, _BCM_DPP_LIF_TO_GPORT_EGRESS);
    is_global = DPP_IS_FLAG_SET(flags, _BCM_DPP_LIF_TO_GPORT_GLOBAL_LIF);
    is_local = DPP_IS_FLAG_SET(flags, _BCM_DPP_LIF_TO_GPORT_LOCAL_LIF);

    if (!(is_ingress ^ is_egress)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("API _bcm_dpp_lif_to_gport must recieve ingress XOR egress lif flag")));
    }

    if (!(is_global ^ is_local)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("API _bcm_dpp_lif_to_gport must recieve local XOR global lif flag")));
    }


    soc_sand_dev_id = (unit);

    if(gport_id == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("API _bcm_dpp_lif_to_gport must recieve not null gport_id")));
    }

    /* In arad, the ingress local lif, egress local lif and global lif are identical */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        is_ingress = is_local = is_global = TRUE;
    }

    *gport_id = 0;

    if (is_local) {
        rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, (is_ingress) ? _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS : _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS,
                                                             lif_id, &gport_lif_used);
        BCMDNX_IF_ERR_EXIT(rv);
        local_lif = lif_id;
    } else {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, (is_ingress) ? _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS : _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS,
                                                             lif_id, &local_lif);
        BCMDNX_IF_ERR_EXIT(rv);
        gport_lif_used = lif_id;
    }

    /* 
     *  Check usage first
     */
    if (is_ingress) {
        rv = _bcm_dpp_lif_sw_resources_lif_usage_get(unit, local_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, &usage, NULL);
        BCMDNX_IF_ERR_EXIT(rv);

        if (usage == _bcmDppLifTypeAny) {
            /* no specific usage gport is invalid */
            *gport_id = BCM_GPORT_INVALID;
            BCM_EXIT;
        }
    }
    else {
        rv = _bcm_dpp_lif_sw_resources_lif_usage_get(unit, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_lif, NULL, &usage);
        BCMDNX_IF_ERR_EXIT(rv);

        if (usage == _bcmDppLifTypeAny) {
            /* no specific usage gport is invalid */
            *gport_id = BCM_GPORT_INVALID;
            BCM_EXIT;
        }
    }

    /* Option 1 LifType is a simple LIF where gport_id is always LIF ID */
    if (usage == _bcmDppLifTypeL2Gre || 
        usage == _bcmDppLifTypeVxlan ||
        usage == _bcmDppLifTypeIpTunnel ||
        usage == _bcmDppLifTypeMplsTunnel) {
        *gport_id = gport_lif_used;
        /* Just encode and thats it */
        rv = _bcm_dpp_lif_to_gport_encoding(unit, *gport_id, usage, gport_id);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT; /* Done */
    }

    
    if (is_egress) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("API _bcm_dpp_lif_to_gport does not support egress flag for given gport")));
    }

    /* If not - take from SW state */    
    /* If local lif id is valid, the gport will be based on the global lif. Get the global lif for this gport. */
    if (sw_state && is_ingress) {
        /* Try first from SW */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_to_sw_resources(unit, local_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));
        *gport_id = gport_sw_resources.in_lif_sw_resources.gport_id;

        if (*gport_id != 0) {
            /* In case of found, insert type encoding and thats it */
            rv = _bcm_dpp_lif_to_gport_encoding(unit, *gport_id, usage, gport_id);
            BCMDNX_IF_ERR_EXIT(rv);
            BCM_EXIT; /* Done */
        }
    }
    
    /* Still not found: call HW */
    if (is_ingress) {
        BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPD_LIF_ENTRY_INFO), "_bcm_dpp_lif_to_gport.lif_entry_info");
        if (lif_entry_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id,lif_id,lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* 
         *  HW save this information from the learning information. Depends on LIF type we can try to retreive learn information if exist
         *  Type          | Learn information
         *  AC, Extender  | Take from learn record in case learning is done per AC
         *  PWE           | Take once PWE learn is enabled
         *  IP Tunnel RIF | Take once IP tunnel learn is enabled
         */
        if (lif_entry_info->type & (SOC_PPD_LIF_ENTRY_TYPE_AC|SOC_PPD_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP|SOC_PPD_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP)) {
            if (lif_entry_info->value.ac.learn_record.learn_type == SOC_PPD_L2_LIF_AC_LEARN_INFO) {
                learn_info_ptr = &lif_entry_info->value.ac.learn_record.learn_info;
                from_learning = TRUE;
            }
        } else if (lif_entry_info->type == SOC_PPD_LIF_ENTRY_TYPE_PWE) {
            if (lif_entry_info->value.pwe.learn_record.enable_learning) {
                learn_info_ptr = &lif_entry_info->value.pwe.learn_record.learn_info;  
                from_learning = TRUE;
            }                  
        } else if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {                           
            if (lif_entry_info->value.ip_term_info.learn_enable) {
                learn_info_ptr = &lif_entry_info->value.ip_term_info.learn_record;
                from_learning = TRUE;
            }
        } else if (lif_entry_info->type == SOC_PPD_LIF_ENTRY_TYPE_EXTENDER) {
            if (lif_entry_info->value.extender.learn_record.learn_type == SOC_PPD_L2_LIF_AC_LEARN_INFO) {
                learn_info_ptr = &lif_entry_info->value.extender.learn_record.learn_info;
                from_learning = TRUE;
            }
        }

        /* In case we have this from learning take from HW */

        if (from_learning) {
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_from_fwd_decision(unit, gport_id, learn_info_ptr));
            if (*gport_id != 0) {
                /* In case of found, we are done */
                BCM_EXIT; /* Done */
            }
        }
        

        /* Up until now , did not find. Try SW state in case we didnt try so far */
        if (!sw_state && *gport_id == 0) { /* no other way must use SW as last chance */  
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_to_sw_resources(unit, lif_id, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));
            *gport_id = gport_sw_resources.in_lif_sw_resources.gport_id;
        }
    }

    /* Encoding and done */
    rv = _bcm_dpp_lif_to_gport_encoding(unit, *gport_id, usage, gport_id);
    BCMDNX_IF_ERR_EXIT(rv);
   

exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}


/* See .h for more information */
int
_bcm_dpp_gport_from_hw_resources(int unit, bcm_gport_t *gport, uint32 flags, _bcm_dpp_gport_hw_resources *gport_hw_resources)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(gport_hw_resources);
    BCMDNX_NULL_CHECK(gport);

    if ((flags & _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK) || 
        ((flags & _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF) == _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF) ||
        ((flags & _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF) == _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF) || 
        ((flags & _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF) == _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported flag in _bcm_dpp_gport_from_hw_resources\n")));
	}

    

    if (flags & _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only ingress/egress global lif, and ingress local lif is handled, local egress is not supported\n")));       
    }

    if (flags & _BCM_DPP_GPORT_HW_RESOURCES_FEC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only ingress/egress global lif, and ingress local lif is handled, FEC is not supported\n")));       
    }

    if (flags & _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS) {
        rv = _bcm_dpp_lif_to_gport(unit, gport_hw_resources->global_in_lif, _BCM_DPP_LIF_TO_GPORT_GLOBAL_LIF|_BCM_DPP_LIF_TO_GPORT_INGRESS|_BCM_DPP_LIF_TO_GPORT_USE_SW_STATE, gport);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if (flags & _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS) {
        rv = _bcm_dpp_lif_to_gport(unit, gport_hw_resources->global_out_lif, _BCM_DPP_LIF_TO_GPORT_GLOBAL_LIF|_BCM_DPP_LIF_TO_GPORT_EGRESS|_BCM_DPP_LIF_TO_GPORT_USE_SW_STATE, gport);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if (flags & _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS) {
        rv = _bcm_dpp_lif_to_gport(unit, gport_hw_resources->local_in_lif, _BCM_DPP_LIF_TO_GPORT_LOCAL_LIF|_BCM_DPP_LIF_TO_GPORT_INGRESS|_BCM_DPP_LIF_TO_GPORT_USE_SW_STATE, gport);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    exit:
    BCMDNX_FUNC_RETURN;
}

/* See .h for more information */
int
_bcm_dpp_lif_to_sw_resources(int unit, int in_lif, int out_lif, uint32 flags, _bcm_dpp_gport_sw_resources *gport_sw_resources)
{
    _bcm_dpp_sw_resources_inlif_t  inlif_sw_res;
    _bcm_dpp_sw_resources_outlif_t outlif_sw_res;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(gport_sw_resources);

    if ((flags & (_BCM_DPP_GPORT_SW_RESOURCES_INGRESS|_BCM_DPP_GPORT_SW_RESOURCES_EGRESS)) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No flags requested\n")));
    }

    /* InLlif SW info was requested */
    if ((flags & _BCM_DPP_GPORT_SW_RESOURCES_INGRESS)) {
        if (in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            /* Fill gport_sw_resources */
            BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.inlif_info.get(unit, in_lif, &inlif_sw_res));

            /* Fill gport_sw_resources */
            sal_memcpy(&(gport_sw_resources->in_lif_sw_resources),&inlif_sw_res,sizeof(_bcm_dpp_sw_resources_inlif_t));        
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In Lif invalid\n")));
        }
    }

    /* OutLif SW info was requested */
    if ((flags & _BCM_DPP_GPORT_SW_RESOURCES_EGRESS)) {
        if (out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.outlif_info.get(unit, out_lif, &outlif_sw_res));

            /* Fill gport_sw_resources */
            sal_memcpy(&(gport_sw_resources->out_lif_sw_resources),&outlif_sw_res,sizeof(_bcm_dpp_sw_resources_outlif_t));
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Out Lif invalid\n")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* See .h for more information */
int
_bcm_dpp_lif_sw_resources_set(int unit, int in_lif, int out_lif, uint32 flags, _bcm_dpp_gport_sw_resources *gport_sw_resources)
{

    _bcm_dpp_sw_resources_inlif_t  inlif_sw_res;
    _bcm_dpp_sw_resources_outlif_t outlif_sw_res;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(gport_sw_resources);

    if ((flags & (_BCM_DPP_GPORT_SW_RESOURCES_INGRESS|_BCM_DPP_GPORT_SW_RESOURCES_EGRESS)) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No flags specified\n")));
    }

    /* InLlif SW info was requested */
    if ((flags & _BCM_DPP_GPORT_SW_RESOURCES_INGRESS)) {
        if (in_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            /* Fill gport_sw_resources */
            sal_memcpy(&inlif_sw_res,&(gport_sw_resources->in_lif_sw_resources),sizeof(_bcm_dpp_sw_resources_inlif_t));

            BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.inlif_info.set(unit, in_lif, &inlif_sw_res));
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In Lif invalid\n")));
        }
    }

    /* OutLif SW info was requested */
    if ((flags & _BCM_DPP_GPORT_SW_RESOURCES_EGRESS)) {
        if (out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
            /* Fill gport_sw_resources */
            sal_memcpy(&outlif_sw_res,&(gport_sw_resources->out_lif_sw_resources),sizeof(_bcm_dpp_sw_resources_outlif_t));

            BCMDNX_IF_ERR_EXIT(GPORT_MGMT_ACCESS.outlif_info.set(unit, out_lif, &outlif_sw_res));
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Out Lif invalid\n")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* See .h for more information */
int
_bcm_dpp_lif_sw_resources_delete(int unit, int in_lif, int out_lif, uint32 flags)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_DETACHING(unit)) {
        sal_memset(&gport_sw_resources, 0, sizeof(gport_sw_resources));
        gport_sw_resources.in_lif_sw_resources.lif_type = _bcmDppLifTypeAny;
        gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeAny;

        rv = _bcm_dpp_lif_sw_resources_set(unit, in_lif, out_lif, flags, &gport_sw_resources);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* See .h for more information */
int
_bcm_dpp_gport_to_hw_resources(int unit, bcm_gport_t gport, uint32 flags, _bcm_dpp_gport_hw_resources *gport_hw_resources)
{
    int rv;
    int* global_in_lif = NULL;
    int* global_out_lif = NULL;
    int* fec = NULL;
    int* local_in_lif = NULL;
    int* local_out_lif = NULL;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(gport_hw_resources);

    if ((flags & (_BCM_DPP_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF|_BCM_DPP_GPORT_HW_RESOURCES_FEC)) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No flags requested, gport (0x%x)\n"),gport));
    }
    gport_hw_resources->fec_id = -1;
    gport_hw_resources->global_in_lif = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    gport_hw_resources->global_out_lif = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    gport_hw_resources->local_in_lif = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    gport_hw_resources->local_out_lif = _BCM_GPORT_ENCAP_ID_LIF_INVALID;

    if (flags & _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS) {
        global_in_lif = &(gport_hw_resources->global_in_lif);
    }

    if (flags & _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS) {
        global_out_lif = &(gport_hw_resources->global_out_lif);
    }

    if (flags & _BCM_DPP_GPORT_HW_RESOURCES_FEC) {
        fec = &(gport_hw_resources->fec_id);
    }

    if (flags & (_BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF | _BCM_DPP_GPORT_HW_RESOURCES_FEC)) {
        rv = _bcm_dpp_gport_to_global_lif(unit, gport, global_in_lif /* Either NULL or valid pointer */
                                                     , global_out_lif /* Either NULL or valid pointer */  
                                                     , fec /* Either NULL or valid pointer */
                                                     , NULL);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    /* Local-LIF ? */
    if (flags & _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF) {
        if (flags & _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS) {
            local_in_lif = &(gport_hw_resources->local_in_lif); /* Either NULL or valid pointer */
        }
        if (flags & _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS) {
            local_out_lif = &(gport_hw_resources->local_out_lif); /* Either NULL or valid pointer */
        }
        rv = _bcm_dpp_gport_to_global_and_local_lif(unit,gport,NULL,local_in_lif,local_out_lif,NULL,NULL);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Strict check ? */
    if (flags & _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK) {
        if ((flags & _BCM_DPP_GPORT_HW_RESOURCES_FEC) && gport_hw_resources->fec_id == -1) {            
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Requested FEC_ID but it is invalid, gport (0x%x)\n"),gport));
        }
        if ((flags & _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS) && gport_hw_resources->global_in_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {            
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Requested GLOBAL_LIF_INGRESS but it is invalid, gport (0x%x)\n"),gport));
        }
        if ((flags & _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS) && gport_hw_resources->global_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {            
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Requested GLOBAL_LIF_EGRESS but it is invalid, gport (0x%x)\n"),gport));
        }
        if ((flags & _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS) && gport_hw_resources->local_in_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {            
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Requested LOCAL_LIF_INGRESS but it is invalid, gport (0x%x)\n"),gport));
        }
        if ((flags & _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS) && gport_hw_resources->local_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {            
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Requested LOCAL_LIF_EGRESS but it is invalid, gport (0x%x)\n"),gport));
        }
    }
    
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Given a vlan_port_id/mpls_port_id, determines whether this gport represents ingress only or egress only.
 * Given a different kind of gport, does nothing.
 */
int
_bcm_dpp_gport_l2_intf_determine_ingress_egress(bcm_gport_t gport_id, uint8 *is_ingress, uint8 *is_egress){

    if (BCM_GPORT_IS_VLAN_PORT(gport_id) || BCM_GPORT_IS_MPLS_PORT(gport_id)) {

        /* By default, a vlan port is both ingress and egress */
        *is_ingress = TRUE;
        *is_egress = TRUE;

        if (BCM_GPORT_SUB_TYPE_IS_LIF(gport_id)) {
            if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY)) {
                *is_ingress = FALSE;
            }
            if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY)) {
                *is_egress = FALSE;
            }
        }

        if (BCM_GPORT_IS_MPLS_PORT(gport_id)) {
            if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(gport_id)) {
                *is_ingress = FALSE;
                *is_egress = FALSE;
            }
        }
    } else {
        /* If not vlan port, return error */
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


/*
 * given vpn and iter return next gport-value in vpn 
 * caller still need to call  
 */
int _bcm_dpp_vpn_get_next_gport(
    int                  unit,
    bcm_vpn_t            vpn,
    int                  *iter,
    bcm_gport_t          *port_val,
    int                  *cur_vsi)
{
    int cur_lif;
    int vsi;
    _bcm_lif_type_e usage;
    int rv = BCM_E_NONE;
    uint8 sw_state = TRUE;
    int lif_to_gport_flags = _BCM_DPP_LIF_TO_GPORT_INGRESS | _BCM_DPP_LIF_TO_GPORT_LOCAL_LIF;


    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(iter);
    BCMDNX_NULL_CHECK(port_val);

    cur_lif = *iter;

    *port_val = BCM_GPORT_INVALID;

    if(*iter == -1) {
        BCM_EXIT;
    }

    if(vpn == 0) {
        vsi = 0;
    }
    else{
        vsi = _BCM_DPP_VPN_ID_GET(vpn);
    }

    /* reach the end set to -1 and exit */
    if(cur_lif == _BCM_GPORT_NOF_LIFS) {
        *iter = -1;
        *port_val = BCM_GPORT_INVALID;
        BCM_EXIT;
    }

    /* for Multipoint traverse LIF table and compare VSI */
    for(; cur_lif < _BCM_GPORT_NOF_LIFS; ++cur_lif) {
        rv = _bcm_dpp_lif_sw_resources_lif_usage_get(unit, cur_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, &usage, NULL);
        BCMDNX_IF_ERR_EXIT(rv);
        if((usage == _bcmDppLifTypeAny) || (SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple == cur_lif) || (SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop == cur_lif)) {
            continue;
        }

        /* Get vsi from SW state, if possible */
        rv = _bcm_dpp_lif_to_vsi(unit, cur_lif, cur_vsi, sw_state, NULL);          
        BCMDNX_IF_ERR_EXIT(rv);

        /* if VSI is zero, don't compare VSI and return all LIFs in use */
        if(vsi != 0) {
            if(*cur_vsi != vsi) {
                continue;
            }
        }

        /* found match either LIF is assigned to this VSI or all LIFs in use (VSI = 0) */
        break;                        
    }

    /* reach end of table */
    if(cur_lif == _BCM_GPORT_NOF_LIFS) {
        *port_val = BCM_GPORT_INVALID;
        *iter = -1;
        BCM_EXIT;
    }

    /* 
     * Resolve port_val to be the relevant gport
     */

    lif_to_gport_flags |= _BCM_DPP_LIF_TO_GPORT_USE_SW_STATE;
    
    rv = _bcm_dpp_lif_to_gport(unit, cur_lif, lif_to_gport_flags, port_val);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* update iter */
    *iter = cur_lif+1;
    if(cur_lif == _BCM_GPORT_NOF_LIFS) {
        *iter = -1;
        BCM_EXIT;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int _bcm_dpp_vsi_assignment_to_vsi(
    int unit,
    uint32 vsi_assignment_mode,
    int lif,
    int vsi_base,
    int * vsi)
{
    BCMDNX_INIT_FUNC_DEFS;

    *vsi = -1;

    switch (vsi_assignment_mode) {
    case SOC_PPC_VSI_EQ_VSI_BASE:
        *vsi = vsi_base;
        break;
    case SOC_PPC_VSI_EQ_IN_LIF:
        *vsi = lif;
        break;
    case SOC_PPC_VSI_EQ_IN_VID_PLUS_VSI_BASE:
        *vsi = -1 ;/* can't be validated */
        break;
    default:
        *vsi = vsi_base;
        break;
    }
    
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * From LIF retireve VSI. 
 * sw_state parameter: Optimized or not optimized call. 
 * In case sw_state is 1 then resolving lif to vsi will be in optimized time which is 
 * take from SW DBs as much as possible.
 */
int _bcm_dpp_lif_to_vsi(
    int unit,
    int          lif,
    int          *vsi,
    uint8        sw_state,
    bcm_gport_t  *gport /* valid for mp, and p2p, for P2P read from SW db*/
   )
{
    SOC_PPD_LIF_ENTRY_INFO
        lif_entry_info;
    unsigned int soc_sand_dev_id;
    int rv;
    uint32 soc_sand_rv;    
    int tmp_vsi = 0;
    bcm_gport_t tmp_gport;
    int is_local = FALSE;
    _bcm_lif_type_e in_lif_usage_type;

    int lif_to_gport_flags = _BCM_DPP_LIF_TO_GPORT_INGRESS | _BCM_DPP_LIF_TO_GPORT_LOCAL_LIF;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    if(vsi == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI given is NULL")));
    }

    /* init as not valid */
    *vsi = -1;
    if (gport != NULL) {
      *gport = -1;
    }

    /* Get the usage type for lif validation */
    rv = _bcm_dpp_lif_sw_resources_lif_usage_get(unit, lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, &in_lif_usage_type, NULL);
    BCMDNX_IF_ERR_EXIT(rv);
    if (in_lif_usage_type == _bcmDppLifTypeAny) {
        BCM_EXIT;
    }

    /* Attemp to retrieve the vsi from the SW */
    if (sw_state) {
        lif_to_gport_flags |= _BCM_DPP_LIF_TO_GPORT_USE_SW_STATE;

        _bcm_dpp_lif_sw_resources_vsi_get(unit, lif, &tmp_vsi);
        if (tmp_vsi != 0) {
            *vsi = tmp_vsi;
        }
    }

    /* Get the the gport from the LIF */
    rv = _bcm_dpp_lif_to_gport(unit, lif, lif_to_gport_flags, &tmp_gport);
    BCMDNX_IF_ERR_EXIT(rv);

    if (gport != NULL) {
        *gport = tmp_gport;
    }

    if ((*vsi == -1) &&
        ((in_lif_usage_type == _bcmDppLifTypeVlan) || (in_lif_usage_type == _bcmDppLifTypeMplsPort))) {

        /* verify that that the gport is local */
        rv = _bcm_dpp_gport_is_local(unit, tmp_gport, &is_local);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_local) {
            soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id,lif,&lif_entry_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_EMPTY) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF type is Empty even if lif match is valid")));
            }

            /* is it pwe */
            if(lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_PWE){
                *vsi = lif_entry_info.value.pwe.vsid;                
                if(lif_entry_info.value.pwe.service_type == SOC_PPD_L2_LIF_PWE_SERVICE_TYPE_MP) {            
                    /* vsi might come from VSI assignment mode */
                    rv = _bcm_dpp_vsi_assignment_to_vsi(unit, lif_entry_info.value.pwe.vsi_assignment_mode, lif, *vsi, vsi);
                    BCMDNX_IF_ERR_EXIT(rv);
                    
                }
            }
            /* is it AC */
            else if(lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_AC){
                *vsi = lif_entry_info.value.ac.vsid;        
                if(lif_entry_info.value.ac.service_type == SOC_PPD_L2_LIF_AC_SERVICE_TYPE_MP) {            
                    /* vsi might come from VSI assignment mode */
                    rv = _bcm_dpp_vsi_assignment_to_vsi(unit, lif_entry_info.value.ac.vsi_assignment_mode, lif, *vsi, vsi);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
             /* is it mim */
            else if(lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_ISID){
                *vsi = -1;
            }
            /* Port Extender */
            else if (lif_entry_info.type == SOC_PPD_LIF_ENTRY_TYPE_AC) {
                *vsi = lif_entry_info.value.extender.vsid;
                if (lif_entry_info.value.extender.service_type == SOC_PPD_L2_LIF_AC_SERVICE_TYPE_MP) {            
                    /* vsi might come from VSI assignment mode */
                    rv = _bcm_dpp_vsi_assignment_to_vsi(unit, lif_entry_info.value.extender.vsi_assignment_mode, lif, *vsi, vsi);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_gport_alloc_global_and_local_lif
 * Purpose:
 *       Allocates global lif and local lifs according to given arguements:
 *          If local_in_lif_info == NULL && local_out_lif_info == NULL returns error.
 *          If local_in_lif_info != NULL, allocates a local in lif and a global in lif.
 *          If local_out_lif_info != NULL, allocates a local out lif, and if lif is mapped allocates a global lif and writes to GLEM.
 *  
 *  
 *      The fields that should be filled are:
 *          local_in_lif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_COMMON OR BCM_DPP_AM_IN_LIF_FLAG_WIDE
 *          local_in_lif_info.app_type = bcm_dpp_am_ingress_lif_app_*
 *  
 *          local_out_lif_info.local_lif_flags          = 0 OR BCM_DPP_AM_OUT_LIF_FLAG_WIDE
 *          outlif_info.app_alloc_info.pool_id          = dpp_am_res_* (has to be an egress lif pool).
 *          outlif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_*
 *          
 *  
 * Parameters:
 *      unit                - (IN) Device Number
 *      flags               - (IN) Only BCM_DPP_AM_FLAG_ALLOC_WITH_ID is supported.
 *      global_lif_id       - (INOUT) Fill with the global lif id if WITH_ID, otherwise will be filled with the allocated globall if id.
 *      local_inlif_info    - (INOUT) Local inlif data, will be filled with the allocated local in lif id.
 *      local_outlif_info   - (INOUT) Local outlif data, will be filled with the allocated local out lif id.
 *      write_glem_entry    - (IN) True will write the Glem entry to HW, false will not.
 *  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_gport_alloc_global_and_local_lif(int                               unit, 
                                          uint32                            flags,
                                          int                               *global_lif_id, 
                                          bcm_dpp_am_local_inlif_info_t     *local_in_lif_info, 
                                          bcm_dpp_am_local_out_lif_info_t   *local_out_lif_info, 
                                          uint8                             write_glem_entry) 
{
    int rv = BCM_E_NONE;  
    uint32 global_lif_flags = 0, local_lif_flags = 0;
    uint8 is_mapped_lif = TRUE, is_ingress, is_egress;

    BCMDNX_INIT_FUNC_DEFS;

    /* 
     *  Input validation and setup.
     */
    BCMDNX_NULL_CHECK(global_lif_id);
    global_lif_flags = flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
    is_ingress = (local_in_lif_info != NULL);
    is_egress = (local_out_lif_info != NULL);

    /* Make sure user gave either an ingress or egress lif */
    if (!is_ingress && !is_egress) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can't allocate global lif without local lifs.")));
    }

    /* 
     *  Allocate the global lif.
     */

    /* Check if lif is mapped or direct */
    if ((global_lif_flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) && is_egress) {
        rv = _bcm_dpp_lif_id_is_mapped(unit, FALSE, *global_lif_id, &is_mapped_lif);
        BCMDNX_IF_ERR_EXIT(rv);
        
        if (!is_mapped_lif && local_out_lif_info->app_alloc_info.application_type != bcm_dpp_am_egress_encap_app_out_ac) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only AC lifs can be direct mapped.")));
        }

    }


    /* If lif is direct, don't allocate egress global lif */
    global_lif_flags |= ((is_ingress) ? BCM_DPP_AM_FLAG_ALLOC_INGRESS : 0)
                     | ((is_egress && is_mapped_lif) ? BCM_DPP_AM_FLAG_ALLOC_EGRESS  : 0);

    /* It is possible that lif is direct and egress only. In that case, don't allocate global lif */
    if (is_ingress || is_mapped_lif) {
        rv = bcm_dpp_am_global_lif_alloc(unit, global_lif_flags, global_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* 
     *  Next, allocate the local lifs.
     */

    if (is_ingress) {
        /* Allocate the ingress lif */
        local_in_lif_info->glif = *global_lif_id;
        rv = _bcm_dpp_am_local_inlif_alloc(unit, local_lif_flags, local_in_lif_info);
        BCMDNX_IF_ERR_EXIT(rv);
    } 

    if (is_egress) {   
        /* Allocate the egress lif */

        /* If egress lif is direct (not mapped), use the lif id for the local lif. */
        if (!is_mapped_lif) {
            local_out_lif_info->base_lif_id = *global_lif_id;
            local_lif_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        }

        local_out_lif_info->glif = *global_lif_id;
        local_out_lif_info->local_lif_flags |= (is_mapped_lif) ? BCM_DPP_AM_OUT_LIF_FLAG_COUPLED : BCM_DPP_AM_OUT_LIF_FLAG_DIRECT;

        rv = _bcm_dpp_am_local_outlif_alloc(unit, local_lif_flags, local_out_lif_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* If lif is mapped, write the GLEM entry as well. */

        if (is_mapped_lif && write_glem_entry) {
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_glem_access_entry_add, (unit, *global_lif_id, local_out_lif_info->base_lif_id));
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_gport_delete_global_and_local_lif
 * Purpose:
 *       Given global and local lif, removes them. If remove_glem_entry is set, removes the glem entry according to global lif as well.
 * Parameters:
 *      unit                - (IN) Device Number
 *      global_lif          - (IN) Global lif to be deallocated.
 *      local_inlif_id      - (IN) Local inlif to be deallocated.
 *      local_outlif_id     - (IN) Local outlif to be deallocated.
 *      remove_glem_entry   - (IN) True will remove Glem entry from HW, false will not.
 *  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_gport_delete_global_and_local_lif(int unit, int global_lif, int local_in_lif_id, int local_out_lif_id, uint8 remove_glem_entry) {
    int rv = BCM_E_NONE;  
    uint32 global_lif_flags = 0;
    uint8 is_egress_mapped, is_ingress, is_egress, is_sync;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("_bcm_dpp_gport_delete_global_and_local_lif is unavailable for ARADPLUS and below")));
    }
    
    is_ingress = (local_in_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;
    is_egress = (local_out_lif_id == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;
    rv = _bcm_dpp_lif_id_is_mapped(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &is_egress_mapped);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_global_lif_id_is_sync(unit, global_lif, &is_sync);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync && (!is_ingress || !is_egress)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Both sides of a sync lif must be given to deallocate")));
    }


    if (is_ingress) {
        /* Deallocate ingress if it's an ingress object */
        if (_bcm_dpp_am_local_inlif_is_alloc(unit, local_in_lif_id) == BCM_E_EXISTS) {
            rv = _bcm_dpp_am_local_inlif_dealloc(unit, local_in_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        global_lif_flags |= BCM_DPP_AM_FLAG_ALLOC_INGRESS;
    }

    if (is_egress) {
        /* Deallocate egress if it's an ingress object */
        if (_bcm_dpp_am_local_outlif_is_alloc(unit, local_out_lif_id) == BCM_E_EXISTS) {
            rv = _bcm_dpp_am_local_outlif_dealloc(unit, local_out_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (is_egress_mapped) {
            global_lif_flags |= BCM_DPP_AM_FLAG_ALLOC_EGRESS;
        }
    }

    if (is_egress_mapped && (is_egress || is_ingress)) {
        rv = bcm_dpp_am_global_lif_dealloc(unit, global_lif_flags, global_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (remove_glem_entry && is_egress && is_egress_mapped) {
        /* Remove global lif from GLEM */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_glem_access_entry_remove, (unit, global_lif));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * allocate PWE for MPLS port
 */
int _bcm_dpp_gport_fix_fec_type(
    int unit,
    int fec_id,
    int  new_fec_type)
{
    SOC_PPD_FRWRD_FEC_ENTRY_INFO
        fec_entry[2];
    SOC_PPD_FRWRD_FEC_PROTECT_INFO
        protect_info;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE              
        protect_type;
    uint8
      success;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* In case this is ECMP fec, no need to fix it */
    if (fec_id < SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved) {
        BCM_EXIT;
    }

    soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,fec_id,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (SOC_IS_ARAD(unit)) {
      /* In case of LSR special converting, set eep pointer to be out_rif */
      /* In case of host special converting, set eep pointer to be out_rif */
      if(fec_entry[0].type == SOC_PPD_FEC_TYPE_ROUTING && (new_fec_type == SOC_PPD_FEC_TYPE_IP_UC)) {
        /* Replace eep pointer to be as arp pointer */
        fec_entry[0].eep = fec_entry[0].app_info.out_rif;
        fec_entry[0].app_info.out_rif = 0;
        new_fec_type = SOC_PPD_FEC_TYPE_TUNNELING;
      }
    }
    
    fec_entry[0].type = new_fec_type;
    if(protect_type != SOC_PPD_FRWRD_FEC_PROTECT_TYPE_NONE) {
        fec_entry[1].type = new_fec_type;
        
    }
    soc_sand_rv = soc_ppd_frwrd_fec_entry_add(soc_sand_dev_id,fec_id,protect_type,&fec_entry[0],&fec_entry[1],&protect_info, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * support functions for warmboot
 */
int
_bcm_dpp_sw_db_hash_vlan_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
    int rv;
    int gport_vlan_port_db;

    rv = GPORT_MGMT_ACCESS.vlan_port_db_htb_handle.get(unit, &gport_vlan_port_db);
    if (rv != BCM_E_NONE) {
        return(rv);
    }

    rv = sw_state_htb_iterate(unit, gport_vlan_port_db, restore_cb);

    return(rv);
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
STATIC int
_bcm_dpp_gport_port_state_dump_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int                              rc = BCM_E_NONE;
    _BCM_GPORT_PHY_PORT_INFO        *port_state = NULL;
    bcm_gport_t                     *gport;


    port_state = (_BCM_GPORT_PHY_PORT_INFO *)data;
    gport = (bcm_gport_t *)key;
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "Key: 0x%x, Type: %d, phyGport: 0x%x, encapId: 0x%x\n"), (*gport), port_state->type, port_state->phy_gport, port_state->encap_id));

    return(rc);
}

STATIC int
_bcm_dpp_gport_trill_info_state_dump_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int                              rc = BCM_E_NONE;
    _bcm_petra_trill_info_t         *state = NULL;
    bcm_gport_t                     *gport;


    state = (_bcm_petra_trill_info_t *)data;
    gport = (bcm_gport_t *)key;
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "Key: 0x%x, type: 0x%x fec_id: 0x%x\n"), (*gport), state->type, state->fec_id));

    return(rc);
}

STATIC int
_bcm_dpp_gport_mc_trill_state_dump_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int                              rc = BCM_E_NONE;
    int                             *port_id = NULL;
    bcm_gport_t                     *gport;


    port_id = (int *)data;
    gport = (bcm_gport_t *)key;
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "Key: 0x%x, PortId: 0x%x\n"), (*gport), (*port_id)));

    return(rc);
}


STATIC int
_bcm_dpp_gport_lif_match_info_state_dump(int unit, int lif, _bcm_dpp_sw_resources_inlif_t *state)
{
    int                              rc = BCM_E_NONE;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "Key: 0x%x, lif_type 0x%x, flags 0x%x, criteria 0x%x\n"),
                            lif, state->lif_type, state->flags, state->criteria));
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "port 0x%x, match1 0x%x, match2 0x%x, gport_id 0x%x\n"),
                            state->port,state->match1, state->match2, state->gport_id));
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "key1 0x%x, peer_gport 0x%x, tpid_profile_type 0x%x, VSI 0x%x\n"),
                            state->key1, state->peer_gport, state->tpid_profile_type, state->vsi));        
    return(rc);
}

int
_bcm_dpp_gport_sw_dump(int unit)
{
    int                              rc = BCM_E_NONE;
    int lif;
    _bcm_dpp_gport_sw_resources gport_sw_resources;


    /* dump port state info */
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "\nPort State\n")));
    rc = _bcm_dpp_sw_db_hash_vlan_iterate(unit, _bcm_dpp_gport_port_state_dump_cb);
    if (rc != BCM_E_NONE) {
        return(rc);
    }

    /* dump trill info state info */
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "\nTrill Info\n")));
    rc = _bcm_dpp_trill_sw_db_hash_trill_info_iterate(unit, _bcm_dpp_gport_trill_info_state_dump_cb);
    if (rc != BCM_E_NONE) {
        return(rc);
    }

    /* dump trill mc state info */
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "\nTrill MC\n")));
    rc = _bcm_dpp_trill_sw_db_hash_trill_info_iterate(unit, _bcm_dpp_gport_mc_trill_state_dump_cb);
    if (rc != BCM_E_NONE) {
        return(rc);
    }

    /* dump lif match state info */
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "\nLIF Match Info\n")));

    for (lif=0; lif < _BCM_GPORT_NOF_LIFS; lif++) {
        rc = _bcm_dpp_lif_to_sw_resources(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
        if (rc != BCM_E_NONE) {
            return(rc);
        }

        rc = _bcm_dpp_gport_lif_match_info_state_dump(unit, lif, &gport_sw_resources.in_lif_sw_resources);
        if (rc != BCM_E_NONE) {
            return(rc);
        }
    }

    return(rc);
}

#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
