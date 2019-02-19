/*
 * $Id: l2gre.c,v 1.32 Broadcom SDK $
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
 * DNX L2Gre implementation
 */
#include <soc/defs.h>
 
/* defined(BCM_PETRAB_SUPPORT) ||  defined(BCM_ARAD_SUPPORT) && */

#if defined(INCLUDE_L3) 

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_L2GRE

#include <shared/bsl.h>

#include <bcm/l2gre.h>


#include <bcm_int/control.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>

#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/gport_mgmt.h> 
#include <bcm_int/dpp/vswitch.h> 

#include <shared/shr_resmgr.h>
#include <bcm/debug.h>
#include <bcm/l2gre.h>
#include <bcm/vlan.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/petra_dispatch.h>

#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>


/***************************************************************/

/*
 * Local defines
 */


#define DPP_L2GRE_MSG(string)   "%s[%d]: " string, __FILE__, __LINE__

#define DPP_L2GRE_INIT_CHECK(unit) \
do { \
    BCM_DPP_UNIT_CHECK(unit); \
    if (!_dpp_l2gre_state[unit]->lock) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,  (_BSL_BCM_MSG("%s: L2GREs unitialized on unit:%d \n"), \
                           FUNCTION_NAME(), unit)); \
    } \
} while (0) 

#define DPP_L2GRE_UNIT_LOCK(unit) \
do { \
    if (sal_mutex_take(_dpp_l2gre_state[unit]->lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d. \n"), \
                    FUNCTION_NAME(), unit)); \
    } \
    _lock_taken = 1;  \
} while (0)


#define DPP_L2GRE_UNIT_UNLOCK(unit) \
do { \
    if(1 == _lock_taken) { \
        _lock_taken = 0;  \
        if (sal_mutex_give(_dpp_l2gre_state[unit]->lock)) { \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_give failed for unit %d. \n"), \
                        FUNCTION_NAME(), unit)); \
        } \
    } \
} while (0)


/* map l2vpn to VSI, some tables consider only 12 lsb bits of the VSI */
#define _BCM_L2GRE_VPN_TO_VSI(__unit, __l2vpn)  (__l2vpn & 0xFFF)

#define _BCM_L2GRE_VPN_REPLACE_FLAGS (BCM_L2GRE_VPN_REPLACE               | \
                                      BCM_L2GRE_VPN_UNKNOWN_UCAST_REPLACE | \
                                      BCM_L2GRE_VPN_UNKNOWN_MCAST_REPLACE | \
                                      BCM_L2GRE_VPN_BCAST_REPLACE)

/* 
* sw state for l2Gre
*/  

/*
 * if all devices in system synced. 
 */
static    int _Bcm_dpp_sync_all = 1;


/* dpp l2gres status */

typedef struct _dpp_l2gre_state_s {
    sal_mutex_t lock;
} _dpp_l2gre_state_t;


static _dpp_l2gre_state_t *_dpp_l2gre_state[BCM_MAX_NUM_UNITS];




/* 
 * map l2-gre-port-id to in-LIF
 */
STATIC
int
  _bcm_petra_l2gre_port_id_to_lif(
      int unit, 
      bcm_gport_t l2gre_port_id)          /* GPORT identifier. */
{
    return BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port_id);
}

/* 
 * map in-LIF to l2-gre-port-id 
 */
STATIC
int
  _bcm_petra_l2gre_port_id_from_lif(
      int unit, 
      int in_lif,
      bcm_gport_t *l2gre_port_id)          
{
    BCM_GPORT_L2GRE_PORT_ID_SET(*l2gre_port_id,in_lif);
    return BCM_E_NONE;
}

int
  _bcm_petra_l2gre_port_id_to_resources(
      int unit, 
      bcm_gport_t l2gre_port_id,
      int         *in_lif, 
      int         *out_lif,
      int         *eg_fec,
      int         *match_port, 
      int         *is_local
      )          
{
    _BCM_GPORT_PHY_PORT_INFO *phy_port=NULL;
    int             rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    *eg_fec = -1;
    *out_lif = -1;
    /* in_lif */
    *in_lif = _bcm_petra_l2gre_port_id_to_lif(unit,l2gre_port_id);


    /* from has get <FEC,port> or <outlif,Port> */
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                        &(l2gre_port_id), 
                        (shr_htb_data_t*)(void *)&phy_port,FALSE);        
    if (!phy_port || BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("gport-id not found")));
    }

    /* according to type get <FEC,port> or <outlif,Port> */
    if(phy_port->type == _BCM_DPP_GPORT_IN_TYPE_L2_GRE_EG_FEC) {
        *eg_fec = phy_port->phy_gport;  /* FEC */
        *match_port = phy_port->encap_id;  /* as encap-id has no specific use, store match-port instead of read from FEC */
    }
    else if (phy_port->type == _BCM_DPP_GPORT_IN_TYPE_L2_GRE){/* else store port + outlif*/
        *match_port = phy_port->phy_gport;  /* physical port*/
        *out_lif = phy_port->encap_id; /* encap_id = out-AC */
    }
    else{
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("gport id of l2gre mistmatch internal info")));
    }

    /* check if this local */
    rv = _bcm_dpp_gport_is_local(unit,*match_port,is_local);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 * given l2gre port return used if the call to configure ingress/egress side 
 * according to match port 
 */ 
int
_bcm_petra_l2_gre_side_config_get(
    int unit, 
    bcm_l2gre_port_t *l2gre_port,
    int *ingress, 
    int *egress
 ){
    
    BCMDNX_INIT_FUNC_DEFS;

    /* l2-gre port always config ingress + egress as it exist in same device */
    *ingress = 1;
    *egress = 1;
    
    BCMDNX_FUNC_RETURN;
}

/* 
 * given l2gre port return used resources for this  gport. 
 * consider ingres/egress. 
 */ 
int
_bcm_petra_l2_gre_resources_get(
    int unit, 
    bcm_l2gre_port_t *l2gre_port,
    int ingress, 
    int egress, 
    int *in_lif, 
    int *out_lif, 
    SOC_PPD_FEC_ID *fec_index
 ){
    int rv = BCM_E_NONE;
    int is_local;


    BCMDNX_INIT_FUNC_DEFS;

    /* init to -1, for not exist */
    *in_lif = -1;
    *out_lif = -1;
    *fec_index = -1;

    if(ingress) {
        /* get match tunnel (terminator) */
        rv = _bcm_dpp_gport_to_lif(unit,l2gre_port->match_tunnel_id,in_lif,NULL,NULL,&is_local);
        BCMDNX_IF_ERR_EXIT(rv);

        /* use egress_tunnel if valid, points egress objects for protection/ECMP*/
        if ((l2gre_port->flags & BCM_L2GRE_PORT_EGRESS_TUNNEL)) {
            /* get FEC id */
            rv = _bcm_l3_intf_to_fec(unit, l2gre_port->egress_if, fec_index );
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /* get outlif if valid */
        if (l2gre_port->egress_tunnel_id != 0) {
            /* get match tunnel (terminator) */
            rv = _bcm_dpp_gport_to_lif(unit,l2gre_port->egress_tunnel_id,NULL,out_lif,NULL,&is_local);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    if(egress) {
        /* use egress_tunnel if valid, points egress objects for protection/ECMP*/
        if ((l2gre_port->flags & BCM_L2GRE_PORT_EGRESS_TUNNEL)) {
            /* get FEC id */
            rv = _bcm_l3_intf_to_fec(unit, l2gre_port->egress_if, fec_index);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /* get outlif if valid */
        if (l2gre_port->egress_tunnel_id != 0) {
            /* get match tunnel (terminator) */
            rv = _bcm_dpp_gport_to_lif(unit,l2gre_port->egress_tunnel_id,NULL,out_lif,NULL,&is_local);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_l2_gre_check_lif(int unit, int lif, int ingress){
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t info;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32 next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES], nof_entries;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;

    if(ingress) {
        /* check if it's allocated from alloc manager */
        rv = bcm_dpp_am_l3_lif_is_allocated(unit, _BCM_DPP_AM_L3_LIF_IP_TERM, 0, lif);
        if(rv != BCM_E_EXISTS) {
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* check it's used for IPv4 [with GRE] from lif SW DB */
        rv = _bcm_dpp_in_lif_tunnel_term_get(unit, &info, lif);
        BCMDNX_IF_ERR_EXIT(rv);

        /* type has to be with l2-GRE */
        if(info.type != bcmTunnelTypeL2Gre && info.type != bcmTunnelTypeIpAnyIn4) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: l2 GRE cannot be build over this type tunnel has to be bcmTunnelTypeL2Gre\n"), FUNCTION_NAME()));
        }
    }
    else{
        /* egress info checked only if all devices in systems are synced
         * otherwsie check is not performed and input assumed to be valid
         */
        if(!_Bcm_dpp_sync_all) {
            BCM_EXIT;
        }

        /* check this entry is allocated */
        rv = bcm_dpp_am_ip_tunnel_eep_is_alloced(unit, 0, lif);
        if(rv != BCM_E_EXISTS) {
            BCM_IF_ERROR_RETURN(rv);
        }

        soc_sand_dev_id = (unit);

        /* get egress info to check type is ipv4 gre */
        soc_sand_rv =
            soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                   SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, lif, 1,
                                   encap_entry_info, next_eep, &nof_entries);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

        if (encap_entry_info[0].entry_type != SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: egress tunnel has to be of IP type \n"), FUNCTION_NAME()));
        }

        if( encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode != SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE &&
            encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode != SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE
          ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: egress tunnel has to be of GRE type \n"), FUNCTION_NAME()));
        }

        /* fix type to be enhanced if's not */
        if( encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode != SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE) {
            encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode = SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE;

            soc_sand_rv =
            soc_ppd_eg_encap_ipv4_entry_add(soc_sand_dev_id, lif, 
                            &encap_entry_info[0].entry_val.ipv4_encap_info, next_eep[0]);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
        }

    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_l2_gre_learn_info_map_set(
    int unit, 
    int out_lif,
    bcm_gport_t match_port,
    SOC_PPD_FEC_ID fec_id,
    SOC_PPD_FRWRD_DECISION_INFO *learn_info
 ){
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* if FEC used, then learn FEC */
    if(fec_id != -1) {
        SOC_PPD_FRWRD_DECISION_FEC_SET(soc_sand_dev_id, learn_info, fec_id, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else{
        /* otherwise learn out-lif + port */
        soc_sand_rv = _bcm_dpp_gport_to_fwd_decision(soc_sand_dev_id,match_port,learn_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* set outlif to learn */
        learn_info->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_RAW;
        learn_info->additional_info.outlif.val = out_lif;
    }


exit:
    BCMDNX_FUNC_RETURN;
}


/*
*  SW state store for L2gre:
*   mapping from l2-gre-gport (in-lif) to <out-FEC> or <out-LIF, out-port>
*   stored in hash table.
*  
*   used for:
*    - forward to gport.
*    - get function
*  
*   this information always stored: for local and remote gports.
*  
*   optimization:
*     - for local l2-gre port, if egress points to FEC and not out-LIF
*       then no need for storage, FEC can be restored from in-LIF 
*  
*/

int 
bcm_petra_l2gre_port_sw_state_add(
    int unit, 
    bcm_vpn_t l2vpn, 
    bcm_l2gre_port_t *l2gre_port,
    int is_local,
    int eg_fec,
    int in_lif,
    int out_lif,
    int update)
 {
     _BCM_GPORT_PHY_PORT_INFO *phy_port=NULL;
     int             rv = BCM_E_NONE;

     BCMDNX_INIT_FUNC_DEFS

    if(!update) {
        /* if new gport then allocate node to be pointed by the has table */
        BCMDNX_ALLOC(phy_port, sizeof(_BCM_GPORT_PHY_PORT_INFO), "l2gre_port");
        if (phy_port == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("alloc failed for l2gre")));
        }
    }
    else{ /* if update remove from hash, reusue same memory*/
        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                            &(l2gre_port->l2gre_port_id), 
                            (shr_htb_data_t*)(void *)&phy_port,TRUE);        
        if (!phy_port || BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("update un-exist entry")));
        }
    }

    /* if fec store only FEC */
    if(eg_fec != -1) {
        phy_port->type = _BCM_DPP_GPORT_IN_TYPE_L2_GRE_EG_FEC;
        phy_port->phy_gport = eg_fec;  /* physical port*/
        phy_port->encap_id = l2gre_port->match_port; /* as encap-id has no specific use, store match-port instead of read from FEC */
    }
    else{/* else store port + outlif*/
        phy_port->type = _BCM_DPP_GPORT_IN_TYPE_L2_GRE;
        phy_port->phy_gport = l2gre_port->match_port;  /* physical port*/
        phy_port->encap_id = out_lif; /* encap_id = out-AC */

        if (out_lif != -1) {
            /* Update outLIF usage */
            rv = _bcm_dpp_out_lif_usage_set(unit,out_lif,_bcmDppLifTypeL2Gre);
            if (BCM_FAILURE(rv)) {
                BCM_FREE(phy_port);
                BCM_RETURN_VAL_EXIT(rv);
            }
        }
    }

    rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                         &(l2gre_port->l2gre_port_id),
                                         phy_port);        
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_L2,
                  (BSL_META_U(unit,
                              "error(%s) Updating l2gre Gport Dbase (0x%x)\n"),
                   bcm_errmsg(rv), l2gre_port->l2gre_port_id));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (in_lif != -1) {
        /* update lif usage */
        rv = _bcm_dpp_in_lif_usage_set(unit, in_lif, _bcmDppLifTypeL2Gre);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_in_lif_match_vsi_set(unit, in_lif, l2vpn);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_l2gre_port_sw_state_remove(
    int unit, 
    bcm_vpn_t l2vpn, 
    bcm_gport_t l2gre_port_id,
    int is_local,
    int eg_fec,
    int in_lif,
    int out_lif)
 {
     _BCM_GPORT_PHY_PORT_INFO *phy_port=NULL;
     int             rv = BCM_E_NONE;

     BCMDNX_INIT_FUNC_DEFS

    /* remove from hash, reusue same memory*/
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                        &(l2gre_port_id), 
                        (shr_htb_data_t*)(void *)&phy_port,TRUE);        
    if (!phy_port || BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("update un-exist entry")));
    }
    BCM_FREE(phy_port);

    if (in_lif != -1) {
        /* update lif usage back to tunnel of type gre */
        rv = _bcm_dpp_in_lif_usage_set(unit, in_lif, _bcmDppLifTypeIpTunnel);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_in_lif_match_vsi_set(unit, in_lif, 0);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (out_lif != -1) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_out_lif_usage_set(unit, out_lif, _bcmDppLifTypeIpTunnel));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2gre_port_add_verify(
    int unit, 
    bcm_vpn_t l2vpn, 
    int update,
    bcm_l2gre_port_t *l2gre_port)
{
    int rv = BCM_E_NONE;
    bcm_l2gre_port_t l2gre_port_tmp;

    BCMDNX_INIT_FUNC_DEFS;

    /* check paramters */
    if ( l2vpn != BCM_L2GRE_VPN_INVALID) {
        /* check vpn is already exist */
        rv = _bcm_dpp_vlan_check(unit,l2vpn);
        BCMDNX_IF_ERR_EXIT(rv);
     }

    if(l2gre_port->criteria != BCM_L2GRE_PORT_MATCH_VPNID){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: only l2gre ports can be created by this API \n"), FUNCTION_NAME()));
    }

     if (l2gre_port->flags & BCM_L2GRE_PORT_WITH_ID ) {
        if (!BCM_GPORT_IS_L2GRE_PORT(l2gre_port->l2gre_port_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: given id is not l2gre-port  \n"), FUNCTION_NAME()));
        }
    }

    /* use egress_tunnel if valid, points egress objects for protection/ECMP*/
    if ((l2gre_port->flags & BCM_L2GRE_PORT_EGRESS_TUNNEL)) {
        if(!_BCM_PETRA_L3_ITF_IS_VALID_FEC(unit,l2gre_port->egress_if)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: egress_if is not valid  \n"), FUNCTION_NAME()));
        }
    } 

    /* check given match tunnel */
    if (!BCM_GPORT_IS_TUNNEL(l2gre_port->match_tunnel_id)) {
        return BCM_E_PARAM;
    }
    /* if update then has to be with ID */
    if (update && !(l2gre_port->flags & BCM_L2GRE_PORT_WITH_ID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: if update then has to be WITH_ID \n"), FUNCTION_NAME()));
    }

    /* Now only support to replace match_port and BCM_L2GRE_PORT_NETWORK */
    if (update) {
        bcm_l2gre_port_t_init(&l2gre_port_tmp);
        l2gre_port_tmp.l2gre_port_id = l2gre_port->l2gre_port_id;
        rv = bcm_petra_l2gre_port_get(unit, l2vpn, &l2gre_port_tmp);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!(l2gre_port->match_port != l2gre_port_tmp.match_port) &&
            !((l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) ^ (l2gre_port_tmp.flags & BCM_L2GRE_PORT_NETWORK))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Only support to replace match_port and flag with BCM_L2GRE_PORT_NETWORK \n"), FUNCTION_NAME()));
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function
 *      bcm_petra_l2gre_port_add
 * Purpose
 *      add l2 gre port to VPN
 * Parameters
 *      (in) unit       = unit number
 *      (in) mode       = group mode
 *      (out) nl2gres = number of l2gres
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
int 
bcm_petra_l2gre_port_add(
    int unit, 
    bcm_vpn_t l2vpn, 
    bcm_l2gre_port_t *l2gre_port)
{
    int rv = BCM_E_NONE;
    SOC_PPD_FEC_ID fec_index = -1; /* if egress is FEC and not tunnel */
    int in_lif = -1; /* where match tunnel is defined */
    int out_lif = -1; /* where egress tunnel is defined */
    int ingress = 1;
    int egress = 1;
    int is_local_config=0;
    int update = 0;

    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_LIF_ENTRY_INFO *lif_info = NULL;
    SOC_SAND_PP_HUB_SPOKE_ORIENTATION 
        orientation_val ;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);

    DPP_L2GRE_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(l2gre_port);

    soc_sand_dev_id = (unit);

    update = (l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)?1:0;
    rv = bcm_petra_l2gre_port_add_verify(unit, l2vpn, update, l2gre_port);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get port orientation from flag */
    if (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) {
        orientation_val  = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
    } else {
        orientation_val  = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
    } 

    /* take lock */ 
    DPP_L2GRE_UNIT_LOCK(unit);

    rv = _bcm_dpp_gport_is_local(unit,l2gre_port->match_port,&is_local_config);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if not local has to be with ID */
    if (!is_local_config && !(l2gre_port->flags & BCM_L2GRE_PORT_WITH_ID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: if not local l2gre port, then has to be WITH_ID \n"), FUNCTION_NAME()));
    }

    /* check if this configuration related to ingress or egress  */
    rv = _bcm_petra_l2_gre_side_config_get(unit,l2gre_port,&ingress,&egress);
    BCMDNX_IF_ERR_EXIT(rv);


    /* map l2gport to used resource */
    rv = _bcm_petra_l2_gre_resources_get(unit,l2gre_port,ingress,egress,&in_lif,&out_lif,&fec_index);
    BCMDNX_IF_ERR_EXIT(rv);

    /* If out_lif used and no protection (fec_index == -1) then it has to be equal to in_lif */
    if(out_lif != - 1  && fec_index == -1 && out_lif != in_lif) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: match_tunnel and egress_tunnel has to be equal \n"), FUNCTION_NAME()));
    }

    /* three things to update */
    /* 1. LIF, learn-info + VSI-assignment mode.
       2. map GRE-vpn to VSI and vice-versa, note this done in vpn-create
       3. store SW state
     */

    /* update ingress */
    if(in_lif != -1 && is_local_config) {
        /* check it's used for IPv4 [with GRE] from lif SW DB */
        rv = _bcm_petra_l2_gre_check_lif(unit, in_lif, 1);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_ALLOC(lif_info, sizeof(SOC_PPD_LIF_ENTRY_INFO), "bcm_petra_l2gre_port_add.lif_info");
        if (lif_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        
        /* update LIF info */
        soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id,in_lif,lif_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if(lif_info->type != SOC_PPD_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: inlif Type is not expected, expect for IP tunnel term \n"), FUNCTION_NAME()));
        }

        /* assume: MP so set learn info */
        lif_info->value.ip_term_info.learn_enable = 1;
        rv = _bcm_petra_l2_gre_learn_info_map_set(unit,out_lif,l2gre_port->match_port,fec_index, &(lif_info->value.ip_term_info.learn_record));
        BCMDNX_IF_ERR_EXIT(rv);

        /* set VSI to zero as taken from VSI-assignment mode according to LIF-index*/
        /* note the assigment of VSI is taken from LIF only, the vsi here is used to save SW state */
        lif_info->value.ip_term_info.vsi = l2vpn;
        lif_info->value.ip_term_info.vsi_assignment_mode = SOC_PPD_VSI_EQ_IN_LIF;

        /* Update orientation in inLif structure */
        lif_info->value.ip_term_info.orientation = orientation_val ;

        soc_sand_rv = soc_ppd_lif_table_entry_update(soc_sand_dev_id,in_lif,lif_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    }

    /* update egress */
    if(out_lif != -1 && is_local_config) {
        /* fix gre type from 4 to 8 */
        rv = _bcm_petra_l2_gre_check_lif(unit, out_lif,0);
        BCMDNX_IF_ERR_EXIT(rv);
#ifdef BCM_ARAD_SUPPORT
        /* update orientation for outlif in auxiliary table if egress hairpin is enabled set egress state as well */
        if (SOC_DPP_CONFIG(unit)->arad->pp_op_mode.split_horizon_filter_enable) {
            soc_sand_rv = soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(
               soc_sand_dev_id,out_lif,orientation_val);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
#endif /* BCM_ARAD_SUPPORT */
    }

    /* set l2gre port id if needed*/
    if(!(l2gre_port->flags & BCM_L2GRE_PORT_WITH_ID)) {
        _bcm_petra_l2gre_port_id_from_lif(unit,in_lif,&(l2gre_port->l2gre_port_id));
    }

    /* store SW state */
    bcm_petra_l2gre_port_sw_state_add(unit,l2vpn,l2gre_port,is_local_config,fec_index,in_lif,out_lif,update);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCM_FREE(lif_info);
    DPP_L2GRE_UNIT_UNLOCK(unit);
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2gre_cleanup(
    int unit)
{
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    if (!_dpp_l2gre_state[unit]) {
        /* not init, so detach done already */
        BCM_RETURN_VAL_EXIT(BCM_E_NONE);
    }
    /* get rid of allocated structure */
    BCM_FREE(_dpp_l2gre_state[unit]);

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_l2gre_init(
    int unit)
{
    _dpp_l2gre_state_t *temp_state = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* create new unit state information */
    BCMDNX_ALLOC(temp_state, sizeof(_dpp_l2gre_state_t), "bcm_petra_l2gre_init");

    if (temp_state) {
        sal_memset(temp_state, 0, sizeof(_dpp_l2gre_state_t));
        temp_state->lock = sal_mutex_create("_dpp_l2gre_unit_lock");
        if (!temp_state->lock) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("%s: fail to allocate lock \n"), FUNCTION_NAME()));
        }
    } else {
      BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("%s: fail to allocate memory \n"), FUNCTION_NAME()));
    }


    _dpp_l2gre_state[unit] = temp_state;
    BCMDNX_FUNC_RETURN; /* Don't go to exit because there is no need to free temp_state*/

    /* configure soc global parameters */
exit:
    BCM_FREE(temp_state);
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;  
}



int 
bcm_petra_l2gre_port_delete(
    int unit, 
    bcm_vpn_t l2vpn, 
    bcm_gport_t l2gre_port_id)
{
    int         in_lif;
    int         out_lif;
    int         eg_fec;
    int         phy_port;
    int       is_local;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_LIF_ENTRY_INFO *lif_info = NULL;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    soc_sand_dev_id = (unit);

    /* take lock */ 
    DPP_L2GRE_UNIT_LOCK(unit);

    in_lif = _bcm_petra_l2gre_port_id_to_lif(unit, l2gre_port_id);
    if (in_lif == -1) {
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    /* get <port,in-lif, out-lif, FEC associated with this GPORT */
    rv = _bcm_petra_l2gre_port_id_to_resources(unit,l2gre_port_id,&in_lif,&out_lif,&eg_fec,&phy_port,&is_local);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 1. remove: LIF learn-info + VSI-assignment mode.*/
    if(in_lif != -1 && is_local) {
        /* check it's used for IPv4 [with GRE] from lif SW DB */
        rv = _bcm_petra_l2_gre_check_lif(unit, in_lif, 1);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_ALLOC(lif_info, sizeof(SOC_PPD_LIF_ENTRY_INFO), "bcm_petra_l2gre_port_delete.lif_info");
        if (lif_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }
       /* update LIF info */
        soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id,in_lif,lif_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if(lif_info->type != SOC_PPD_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: inlif Type is not expected, expect for IP tunnel term \n"), FUNCTION_NAME()));
        }

        /* disable learning  */
        lif_info->value.ip_term_info.learn_enable = 0;
        /* set VSI to zero as */
        lif_info->value.ip_term_info.vsi = 0;
        /* set back vsi assignment to normal, will be zero for this removed tunnel*/
        lif_info->value.ip_term_info.vsi_assignment_mode = SOC_PPD_VSI_EQ_VSI_BASE;

        soc_sand_rv = soc_ppd_lif_table_entry_update(soc_sand_dev_id,in_lif,lif_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* remove store SW state */
    rv = bcm_petra_l2gre_port_sw_state_remove(unit,l2vpn,l2gre_port_id,is_local,eg_fec,in_lif,out_lif);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCM_FREE(lif_info);
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_L2GRE_UNIT_UNLOCK(unit);     
    }
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_l2gre_port_delete_all(
    int unit, 
    bcm_vpn_t l2vpn)
{
    return bcm_petra_vswitch_port_delete_all(unit,l2vpn);
}



int 
bcm_petra_l2gre_port_get(
    int unit, 
    bcm_vpn_t l2vpn, 
    bcm_l2gre_port_t *l2gre_port)
{
    int         in_lif;
    int         out_lif;
    int         eg_fec;
    int         phy_port;
    int  gport_id;
    int       is_local;
    int rv = BCM_E_NONE;
    SOC_PPD_LIF_ENTRY_INFO lif_info;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    _bcm_lif_type_e lif_usage;

    BCMDNX_INIT_FUNC_DEFS;

    /* take lock */ 
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L2GRE_UNIT_LOCK(unit);

    soc_sand_dev_id = (unit);

    in_lif = _bcm_petra_l2gre_port_id_to_lif(unit, l2gre_port->l2gre_port_id);
    if (in_lif == -1) {
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    gport_id = l2gre_port->l2gre_port_id;

    /* get <port,in-lif, out-lif, FEC associated with this GPORT */
    rv = _bcm_petra_l2gre_port_id_to_resources(unit,l2gre_port->l2gre_port_id,&in_lif,&out_lif,&eg_fec,&phy_port,&is_local);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_usage_get(unit, -1, out_lif, NULL, &lif_usage));
    if (lif_usage != _bcmDppLifTypeL2Gre) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("OutLIF is not L2GRE")));
    }

    /* fill l2gre_port*/
    bcm_l2gre_port_t_init(l2gre_port);

    /* ID */
    l2gre_port->l2gre_port_id = gport_id;

    /* const values */
    l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_VPNID;
    /* match port */
    l2gre_port->match_port = phy_port;
    /* FEC */
    if(eg_fec != -1) {
        _bcm_l3_fec_to_intf(unit, eg_fec, &l2gre_port->egress_if);
        l2gre_port->flags |= BCM_L2GRE_PORT_EGRESS_TUNNEL;
    }
    else{ /* point directly to tunnel */
        BCM_GPORT_TUNNEL_ID_SET(l2gre_port->egress_tunnel_id, out_lif);
    }

    /* match tunnel */
    BCM_GPORT_TUNNEL_ID_SET(l2gre_port->match_tunnel_id, in_lif);

    /* Get port orientation information from register */
    SOC_PPD_LIF_ENTRY_INFO_clear(&lif_info);
    soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id,in_lif,&lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if(lif_info.type != SOC_PPD_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: inlif Type is not expected, expect for IP tunnel term \n"), FUNCTION_NAME()));
    }

    if (lif_info.value.ip_term_info.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB ) {
        l2gre_port->flags |= BCM_L2GRE_PORT_NETWORK;
    }

exit:

    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_L2GRE_UNIT_UNLOCK(unit);     
    }

    BCMDNX_FUNC_RETURN;
}

/* not supported */
int 
bcm_petra_l2gre_port_get_all(
    int unit, 
    bcm_vpn_t l2vpn, 
    int port_max, 
    bcm_l2gre_port_t *port_array, 
    int *port_count)
{
    int rv = BCM_E_NONE;
    int iter;
    int port_val;
    int ports_num = 0;
    int vsi;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(port_array);
    BCMDNX_NULL_CHECK(port_count);

    /* check vsi exist */
    rv = _bcm_dpp_vlan_check(unit, l2vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    for(iter = 0; iter != -1;) {
        /* get next port */
        rv = _bcm_dpp_vpn_get_next_gport(unit,l2vpn,&iter,&port_val,&vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        /* no more ports in vpn */
        if(port_val == BCM_GPORT_INVALID) {
            break;
        }

        port_array[ports_num].l2gre_port_id = port_val;
        /* only for L2GRE ports full content is returned for othe port types, only id is retuend use vswitch_get for these ports */
        if(BCM_GPORT_IS_L2GRE_PORT(port_val)) {
            rv = bcm_petra_l2gre_port_get(unit,l2vpn,&port_array[ports_num]);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /* delete next port */
        ++ports_num;
        if (ports_num == port_max) {
            *port_count = ports_num;
            BCM_EXIT;
        }
    }
    *port_count = ports_num;

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_l2gre_vpn_create_verify(
    int unit,
    int update,
    bcm_l2gre_vpn_config_t *info)
{
    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters */
    /* p2p is not supported */
    if(info->flags & BCM_L2GRE_VPN_ELINE){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: BCM_L2GRE_VPN_ELINE is not supported \n"), FUNCTION_NAME()));
    }

    if(!(info->flags & BCM_L2GRE_VPN_WITH_VPNID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: BCM_L2GRE_VPN_WITH_VPNID, must present in vpn create \n"), FUNCTION_NAME()));
    }

    /* if update then has to be with ID */
    if (update && !(info->flags & BCM_L2GRE_VPN_WITH_ID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: if update then has to be WITH_ID \n"), FUNCTION_NAME()));
    }

    /* if update then the VPN should be existed */
    if (update && (BCM_E_EXISTS != bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, info->vpn))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VPN %d is not allocated"),info->vpn));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2gre_vpn_create_vlan_control_update(
    int unit,
    bcm_vlan_control_vlan_t *vlan_control,
    bcm_l2gre_vpn_config_t *info)
{
    uint32 replace_flags = 0;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(vlan_control);

    /* Update unknown unicast mc-group, unknown multicast  mc-group and broadcast mc-group
        * base replace flag.
        */
    rv = bcm_petra_vlan_control_vlan_get(unit,info->vpn,vlan_control);
    BCMDNX_IF_ERR_EXIT(rv);
    
    if (info->flags & BCM_L2GRE_VPN_REPLACE) {
        replace_flags = (BCM_L2GRE_VPN_UNKNOWN_UCAST_REPLACE | 
                         BCM_L2GRE_VPN_UNKNOWN_MCAST_REPLACE |
                         BCM_L2GRE_VPN_BCAST_REPLACE);
    } else {
        replace_flags = (info->flags & (BCM_L2GRE_VPN_UNKNOWN_UCAST_REPLACE | 
                         BCM_L2GRE_VPN_UNKNOWN_MCAST_REPLACE |
                         BCM_L2GRE_VPN_BCAST_REPLACE));
    }
    
    if (replace_flags & BCM_L2GRE_VPN_UNKNOWN_UCAST_REPLACE) {
        vlan_control->unknown_unicast_group = info->unknown_unicast_group;
    }
    
    if (replace_flags & BCM_L2GRE_VPN_UNKNOWN_MCAST_REPLACE) {
        vlan_control->unknown_multicast_group = info->unknown_multicast_group;
    }
    
    if (replace_flags & BCM_L2GRE_VPN_BCAST_REPLACE) {
        vlan_control->broadcast_group = info->broadcast_group;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_l2gre_vpn_create(
    int unit, 
    bcm_l2gre_vpn_config_t *info)
{
    int flags=0;
    bcm_vlan_control_vlan_t vlan_control;
    SOC_PPD_L2_LIF_GRE_KEY gre_key;
    SOC_PPD_L2_LIF_GRE_INFO gre_info;
    int gre_lif_index;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int rv = BCM_E_NONE;
    int update = 0;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    DPP_L2GRE_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(info);

    update = (info->flags & _BCM_L2GRE_VPN_REPLACE_FLAGS)?1:0;
    rv = bcm_petra_l2gre_vpn_create_verify(unit, update, info);
	BCMDNX_IF_ERR_EXIT(rv);

    /* take lock */ 
    DPP_L2GRE_UNIT_LOCK(unit);

    soc_sand_dev_id = (unit);

    /* For create a new VPN, allocate a new VPN and add entry mapping VPN to VSI */
    if (!update) {
        /* with ID allocate */
        if(info->flags & BCM_L2GRE_VPN_WITH_ID) {
            flags=SHR_RES_ALLOC_WITH_ID;
        }
        
        /* map l2vpn to Vpn */
        SOC_PPD_L2_LIF_GRE_KEY_clear(&gre_key);
        SOC_PPD_L2_LIF_GRE_INFO_clear(&gre_info);

        /* allocate VSI for l2gre usage */
        rv = _bcm_dpp_vswitch_vsi_usage_alloc(unit,flags,_bcmDppVsiTypeL2gre,&info->vpn,NULL);
        BCMDNX_IF_ERR_EXIT(rv);

        /* packet's gre key */
        gre_key.vpn_key = info->vpnid;
        /* VSI */
        gre_info.vsid = info->vpn;
        /* HW constraint LIF = VSI
           the check is done in the vpn-create
         */
        gre_lif_index = gre_info.vsid;

        /* add entry mapping VPN to VSI, using SEM
           beside add mapping at egress from VSI to VPN
         */
        soc_sand_rv = soc_ppd_l2_lif_gre_add(
                soc_sand_dev_id,
                &gre_key,
                gre_lif_index,
                &gre_info,
                &failure_indication);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(failure_indication);
    }

    bcm_vlan_control_vlan_t_init(&vlan_control);

    if (update) {
        rv = bcm_petra_l2gre_vpn_create_vlan_control_update(unit, &vlan_control, info);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        vlan_control.broadcast_group = info->broadcast_group;
        vlan_control.unknown_multicast_group = info->unknown_multicast_group;
        vlan_control.unknown_unicast_group = info->unknown_unicast_group;
        /* forwarding vlan has to be equal to vsi */
        vlan_control.forwarding_vlan = info->vpn;
    }

    rv = bcm_petra_vlan_control_vlan_set(unit,info->vpn,vlan_control);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_L2GRE_UNIT_UNLOCK(unit);     
    }
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;  
}


int 
bcm_petra_l2gre_vpn_destroy(
    int unit, 
    bcm_vpn_t l2vpn)
{
    int rv = BCM_E_NONE;
    SOC_PPD_LIF_ID gre_lif_index;
    SOC_PPD_L2_LIF_GRE_KEY gre_key;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;


    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);

    DPP_L2GRE_INIT_CHECK(unit);

    SOC_PPD_L2_LIF_GRE_KEY_clear(&gre_key);

    /* remove mapping from GRE-vpn to l2vpn (vsi)*/

        /* take lock */ 
    DPP_L2GRE_UNIT_LOCK(unit);

    /* check vpn is already exist */
    rv = _bcm_dpp_vlan_check(unit,l2vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    /* as only vsi given, get Gre-VPN from HW mapping */
   /* map l2vpn to Vpn */
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_l2_lif_vsi_to_isid(soc_sand_dev_id,_BCM_L2GRE_VPN_TO_VSI(unit, l2vpn),&gre_key.vpn_key);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    /* 
     * remove entry mapping VPN to VSI
     */
    soc_sand_rv = soc_ppd_l2_lif_gre_remove(
            soc_sand_dev_id,
            &gre_key,
            &gre_lif_index);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* no need to free usage of gre_lif_index as it was not allocated */

    /* destroy vswitch and delete all ports */
    rv = bcm_petra_vswitch_destroy_internal(unit, l2vpn, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    /* set usage */
    rv = _bcm_dpp_vswitch_vsi_usage_type_set(unit, l2vpn, _bcmDppVsiTypeL2gre,0);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_L2GRE_UNIT_UNLOCK(unit);     
    }
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;  
}


int 
bcm_petra_l2gre_vpn_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
    /* call vswitch_destroy_all */
    /* where vswitch_destroy_all: call l2gre_vpn_destroy if needed */
}


int 
bcm_petra_l2gre_vpn_get(
    int unit, 
    bcm_vpn_t l2vpn, 
    bcm_l2gre_vpn_config_t *info)
{

    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t vlan_control;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_L2GRE_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(info);

    /* check vpn is already exist */
    rv = _bcm_dpp_vlan_check(unit,l2vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    /* take lock */ 
    DPP_L2GRE_UNIT_LOCK(unit);
    soc_sand_dev_id = (unit);

    rv = bcm_petra_vlan_control_vlan_get(unit,l2vpn,&vlan_control);
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_l2gre_vpn_config_t_init(info);

    /* set back vpn id after clear */
    info->vpn = l2vpn;
    /* set flooding groups */
    info->broadcast_group = vlan_control.broadcast_group;
    info->unknown_multicast_group = vlan_control.unknown_multicast_group;
    info->unknown_unicast_group = vlan_control.unknown_unicast_group;

    /* only MP is supported */
    info->flags |= BCM_L2GRE_VPN_ELINE;

    /* GRE-vpn-id, HW map according to 12 lsb of vpn */
    soc_sand_rv = soc_ppd_l2_lif_vsi_to_isid(soc_sand_dev_id,_BCM_L2GRE_VPN_TO_VSI(unit, l2vpn),&info->vpnid);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_L2GRE_UNIT_UNLOCK(unit);     
    }
    BCMDNX_FUNC_RETURN;  
}

int 
bcm_petra_l2gre_vpn_traverse(
    int unit, 
    bcm_l2gre_vpn_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;
    return rv;
}

#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */
