/*
 * $Id: tunnel.c,v 1.53 Broadcom SDK $
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
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_TUNNEL

#include <shared/bsl.h>

#include <bcm_int/common/debug.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>


#include <bcm_int/control.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/mpls.h>
#include <bcm_int/dpp/state.h>
#include <bcm_int/dpp/qos.h>
#include <bcm/multicast.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/mpls.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/tunnel.h>
#include <bcm_int/dpp/failover.h>
#include <bcm_int/common/field.h>

#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPC/ppc_api_port.h>

#include <shared/swstate/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>


#include <shared/shr_resmgr.h>

static    int _Bcm_dpp_sync_lif_eep = 1;

/* Local defines */


#define BCM_DPP_TUNNEL_INIT_CHECK \
    {                                                                                           \
        int _init;                                                                              \
	    BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.init.get(unit, &_init));                     \
        if ((!_init)) {                                                                         \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("unit %d not initialised"), unit));   \
        }                                                                                       \
    }

#define _BCM_PETRA_TUNNEL_TERM_FLAGS_UNSUPPORTED \
 (BCM_TUNNEL_INIT_USE_INNER_DF         |     BCM_TUNNEL_TERM_KEEP_INNER_DSCP | \
 BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE |     BCM_TUNNEL_TERM_WLAN_SET_ROAM   | \
 BCM_TUNNEL_INIT_IPV4_SET_DF           |     BCM_TUNNEL_INIT_IPV6_SET_DF | \
 BCM_TUNNEL_INIT_IP4_ID_SET_FIXED      |     BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM | \
 BCM_TUNNEL_INIT_WLAN_MTU             |     BCM_TUNNEL_INIT_WLAN_FRAG_ENABLE | \
 BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED     |     BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID)


 
/*
 * when traversing the ILM, to perfrom action on each entry how many 
 * entries to return in each iteration 
 */

    /* is the given gport working gport in protection, assuming gport has protection*/
#define _BCM_PPD_TUNNEL_IS_WORKING_PORT(gport) ((gport)->failover_tunnel_id != BCM_GPORT_TYPE_NONE)
/*
 * MACROs
 */

/* set DF flag in tos-index*/
#define _BCM_DPP_IPV4_TUNNEL_DF_IN_TOS_SET(__tos_index, _df_value)  (__tos_index |= (_df_value) << 3)

#define _BCM_DPP_IPV4_TUNNEL_DF_FROM_TOS_GET(__tos_index)  (((__tos_index) >>  3) & 1)

int _bcm_ip_tunnel_sw_init(int unit) 
{
    BCMDNX_INIT_FUNC_DEFS;

    /* set template management that are tunnels point to invalid profile */

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        /* set template management to port to -1 data with dummy profile, -1 means use dummy profile, ip = uint(-1) is not valid as tunnel source */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_ip_tunnel_init(unit,-1,0xFFFFFFFF,-1,0xFFFFFFFF,-1,0xFFFFFFFF));
    }
#endif


 exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_ip_tunnel_sw_cleanup(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    /* Nothing to do here for now */
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_tunnel_type_support_check
 * Purpose:
 *      Check if tunnel type is supported on a device. 
 * Parameters:
 *      unit        - (IN) Bcm device number.
 *      tnl_type    - (IN) Tunnel type to check.   
 *      tunnel_term - (OUT) Type supported as tunnel terminator.   
 *      tunnel_init - (OUT) Type supported as tunnel initiator.   
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_petra_tunnel_type_support_check(int unit, bcm_tunnel_type_t tnl_type, 
                                     int *tunnel_term, int *tunnel_init)
{
    int term;      /* Tunnel Terminator supported. */
    int init;      /* Tunnel Initiator supported.  */

    switch (tnl_type) {
    case bcmTunnelTypeIpAnyIn4:
    case bcmTunnelTypeGreAnyIn4:
          term = TRUE;
          init = TRUE; /* at initiator need explicit type */
     break;    
    /* ARAD, Egress only */
    case bcmTunnelTypeErspan: /* ERSPAN tunnel, has to be above IPv4 tunnel */
    case bcmTunnelTypeRspan: /* */
      term = FALSE; 
      init = SOC_IS_ARAD(unit); /* supported only in ARAD */
    break;

    case bcmTunnelTypeIp4In6: /* for IPv4 need above header type */
    case bcmTunnelTypeIp6In6:
    case bcmTunnelTypeEtherIp4In4:
    case bcmTunnelTypeL2Gre:
    case bcmTunnelTypeVxlan:
    case bcmTunnelTypeGreAnyIn6:    
    case bcmTunnelTypeL2EncapExternalCpu:
        init = term = SOC_IS_ARAD(unit); /* supported only in ARAD */
    break;
    case bcmTunnelTypeIp4In4:
    case bcmTunnelTypeIp6In4:      
        init = FALSE;
        term = (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL);
      break;
    default:
          term = init = FALSE;
    }

    if (NULL != tunnel_term) {
        *tunnel_term = term;     
    }
    if (NULL != tunnel_init) {
        *tunnel_init = init;     
    }
    return (BCM_E_NONE);
}


/*
 * resolve the ppd tunnel type to use,
 */
int
_bcm_petra_tunnel_ipv4_type_to_ppd(
    int unit, 
    bcm_tunnel_initiator_t *tunnel,
    SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info
 )
{
    int rv = BCM_E_NONE;

    switch(tunnel->type) {
    /* simple IPv4 */
    case bcmTunnelTypeIpAnyIn4:          /* RFC 2003/2893: IPv6/4-in-IPv4 tunnel. */
        ipv4_encap_info->dest.encapsulation_mode = SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_NONE;
    break;
    /* IPv4 with GRE */
    case bcmTunnelTypeGre4In4:
    case bcmTunnelTypeGreAnyIn4:
        ipv4_encap_info->dest.encapsulation_mode = SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE;
    break;
    /* In Arad, VxLan encapsulation mode is the same as L2GRE. */
    case bcmTunnelTypeVxlan: 
    #ifdef BCM_88660_A0         
        if (SOC_IS_ARADPLUS(unit)){
            ipv4_encap_info->dest.encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN;
            break;
        }  
    #endif
    case bcmTunnelTypeL2Gre:
        ipv4_encap_info->dest.encapsulation_mode = SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE;
    break;

    /* Ether IP */
    case bcmTunnelTypeEtherIp4In4:
        ipv4_encap_info->dest.encapsulation_mode = SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP;
    break;
    /* these are not supported as for tunnel into IPv4, user should use anyIn4 */
    case bcmTunnelTypeIp4In4:            
    case bcmTunnelTypeIp6In4:            
    /* these are not supported as for tunnel into IPv6, user should use Ip4In6 or Ip6In6*/
    case bcmTunnelTypeIpAnyIn6:            
    default:
        return BCM_E_PARAM;
    }

    return rv;
}


/*
 * resolve the ppd tunnel type to use,
 */
int
_bcm_petra_tunnel_ipv4_type_from_ppd(
    int unit, 
    SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info,
    bcm_tunnel_initiator_t *tunnel
 )
{
    int rv = BCM_E_NONE;

    switch(ipv4_encap_info->dest.encapsulation_mode) {
    /* simple IPv4 */
    case SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_NONE: 
        tunnel->type = bcmTunnelTypeIp4In4;
    break;
    /* IPv4 with GRE */
    case SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE: 
        tunnel->type = bcmTunnelTypeGreAnyIn4;
    break;
    
    case SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE: 
        tunnel->type = bcmTunnelTypeL2Gre;
    break;
    /* Ether IP */
    case SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP: 
        tunnel->type = bcmTunnelTypeEtherIp4In4;
    break;
    /* VxLAN */
    case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN:
        tunnel->type = bcmTunnelTypeVxlan;
    break;
    default:
        return BCM_E_PARAM;
    }
    return rv;
}


/*
 * given interface and tunnel returns eep. 
 *  first try from tunnel-id if available
 *  otherwise from intf.
 */
int
_bcm_petra_tunnel_id_eep_get(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel,
    int *eep,
    int *is_tunneled)
{
    int eep1=0,eep2=0;
    int is_tunneled1=0,is_tunneled2=0;

    BCMDNX_INIT_FUNC_DEFS;

    /* init to zero */
    *is_tunneled = 0;
    *eep = 0;

    /* from tunnel gport */
    if (tunnel != NULL) {
        eep1 = BCM_GPORT_TUNNEL_ID_GET(tunnel->tunnel_id);
        if(eep1 != _SHR_GPORT_INVALID) {
            is_tunneled1 = 1;
        }
    }
    /* from tunnel interface */
    if (intf != NULL) {
        if(_BCM_PETRA_L3_ITF_IS_ENCAP(intf->l3a_intf_id)) { /* try to get from itf */
            is_tunneled2 = 1;
            eep2 = _BCM_PETRA_L3_ITF_VAL_GET(intf->l3a_intf_id);
        }
        else if (intf->l3a_tunnel_idx != 0) {
            is_tunneled2 = 1;
            eep2 = intf->l3a_tunnel_idx;
        }
    }
    if(is_tunneled2 && is_tunneled1 && eep2 != eep1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tunnel-id given in interface and tunnel-gport mismatch")));
    }

    if(is_tunneled2) {
        *eep = eep2;
        *is_tunneled = 1;
    }
    else if(is_tunneled1) {
        *eep = eep1;
        *is_tunneled = 1;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * set intf id and tunnel.tunnel_id to include EEP
 */
int
_bcm_petra_tunnel_id_eep_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel,
    int eep)
{

    BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id,eep);
    _BCM_PETRA_L3_ITF_SET(intf->l3a_intf_id, _BCM_PETRA_L3_ITF_ENCAP, eep);

    return BCM_E_NONE;
}


/*
 * return wether tunnel type is ipv6 
 */
int
_bcm_petra_tunnel_type_is_ipv6(int unit, bcm_tunnel_type_t tnl_type){
    if(tnl_type == bcmTunnelTypeIp4In6 || tnl_type == bcmTunnelTypeIp6In6) {
        return 1;
    }
    return 0;
}

/* defines for IPv6 tunnel what is the next protocol value to set in header for each protocol */
#define _BCM_PETRA_TUNNEL_NEXT_PRTCL_IPV4  (0x4)
#define _BCM_PETRA_TUNNEL_NEXT_PRTCL_IPV6  (0x29)
#define _BCM_PETRA_TUNNEL_NEXT_PRTCL_GRE  (0x2F)


/*
* map bcm tunnel initiator to ppd 
*/
int
_bcm_petra_tunnel_init_ipv6_to_ppd(
    int unit, 
    bcm_tunnel_initiator_t *tunnel,
    SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO *ipv6_encap_info
 )
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    ipv6_encap_info->tunnel.flow_label = tunnel->flow_label;
    ipv6_encap_info->tunnel.hop_limit = tunnel->ttl;
    rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit,tunnel->dip6,&ipv6_encap_info->tunnel.dest);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit,tunnel->sip6,&ipv6_encap_info->tunnel.src);
    BCMDNX_IF_ERR_EXIT(rv);

    if(tunnel->type == bcmTunnelTypeIp4In6) {
        ipv6_encap_info->tunnel.next_header = _BCM_PETRA_TUNNEL_NEXT_PRTCL_IPV4;
    }
    else if(tunnel->type == bcmTunnelTypeIp6In6) {
        ipv6_encap_info->tunnel.next_header = _BCM_PETRA_TUNNEL_NEXT_PRTCL_IPV6;
    }
    else{
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unsupported tunnel type for IPv6 ")));
    }

    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
* map bcm tunnel initiator from ppd 
*/
int
_bcm_petra_tunnel_init_ipv6_from_ppd(
    int unit, 
    bcm_tunnel_initiator_t *tunnel,
    SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO *ipv6_encap_info
 )
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    tunnel->flow_label = ipv6_encap_info->tunnel.flow_label;
    tunnel->ttl = ipv6_encap_info->tunnel.hop_limit;
    rv = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit,&ipv6_encap_info->tunnel.dest,&tunnel->dip6);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit,&ipv6_encap_info->tunnel.src, &tunnel->sip6);
    BCMDNX_IF_ERR_EXIT(rv);

    if(ipv6_encap_info->tunnel.next_header == _BCM_PETRA_TUNNEL_NEXT_PRTCL_IPV4) {
        tunnel->type = bcmTunnelTypeIp4In6;
    }
    else if(ipv6_encap_info->tunnel.next_header == _BCM_PETRA_TUNNEL_NEXT_PRTCL_IPV6) {
        tunnel->type = bcmTunnelTypeIp6In6;
    }
    else{
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unexpected IPv6 next-protocol")));
    }

    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * resolve the ppd tunnel type to use,
 */
int
_bcm_petra_tunnel_init_ipv4_from_ppd(
    int unit, 
    bcm_tunnel_initiator_t *tunnel,
    SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info
 )
{
    int index;
    uint32 sip = 0;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_SAND_PP_IPV4_TOS tos_encoded;
    SOC_SAND_PP_IP_TTL ttl_encoded;
    int rv = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* get info from tunnel entry */
    tunnel->dip = ipv4_encap_info->dest.dest;

    /* outRIF */
    tunnel->vlan = ipv4_encap_info->out_vsi;

    /* get info from profiles */
    /* SIP */
    index = ipv4_encap_info->dest.src_index;
    soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get(soc_sand_dev_id, index, &sip);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    tunnel->sip = sip;
    

    /* TTL */
    index = ipv4_encap_info->dest.ttl_index;
    soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get(soc_sand_dev_id, index, &ttl_encoded);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    tunnel->ttl = SOC_SAND_PP_TTL_VAL_GET(ttl_encoded);
    if(SOC_SAND_PP_TTL_IS_UNIFORM_GET(ttl_encoded)){
        tunnel->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
    }

    /* TOS */
    index = ipv4_encap_info->dest.tos_index;
    soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get(soc_sand_dev_id, index, &tos_encoded);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    if(!SOC_SAND_PP_TOS_IS_UNIFORM_GET(tos_encoded)){
        tunnel->dscp_sel = bcmTunnelDscpAssign;
        tunnel->dscp = SOC_SAND_PP_TOS_VAL_GET(tos_encoded);
    }
    else{
        tunnel->dscp_sel = bcmTunnelDscpMap;
    }

    /* DF-flag: parse tos-index into DF bit*/
    if(SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set){
        if(_BCM_DPP_IPV4_TUNNEL_DF_FROM_TOS_GET(index) == 1){
            tunnel->flags |= BCM_TUNNEL_INIT_IPV4_SET_DF;
        }
    }

    rv = _bcm_petra_tunnel_ipv4_type_from_ppd(unit,ipv4_encap_info,tunnel);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * map bcm tunnel info to PPD
 */
int
_bcm_petra_tunnel_init_ipv4_to_ppd(
    int unit, 
    bcm_tunnel_initiator_t *tunnel,
    int eep,
    int eep_alloced,
    SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info
 )
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_SAND_PP_IPV4_TOS tos_encoded;
    SOC_SAND_PP_IP_TTL ttl_encoded;
    int rv = BCM_E_NONE;
    uint32 is_uniform;
    uint8 df_flag = 0;
    int sip_template, ttl_template, tos_template,is_allocated,df_updated=0;


    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* set VSI for last EEP */
    ipv4_encap_info->dest.dest = tunnel->dip;

    if (SOC_IS_PETRAB(unit)) {
        ipv4_encap_info->dest.enable_gre = (tunnel->type == bcmTunnelTypeGreAnyIn4);
    }
    /* outRIF */
    ipv4_encap_info->out_vsi = tunnel->vlan;
    /* DSCP SET */
    /* uniform if not taken explictly from entry */
    is_uniform = tunnel->dscp_sel != bcmTunnelDscpAssign;
    SOC_SAND_PP_TOS_SET(tos_encoded,tunnel->dscp,is_uniform);

    /* TTL SET */
    /* uniform if taken from packet*/
    is_uniform = tunnel->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL;
    SOC_SAND_PP_TOS_SET(ttl_encoded,tunnel->ttl,is_uniform);

    /* use template managment to allocate profiles for SIP, TTL and TOS */
    /* eep_alloced tells if this is new Tunnel or not, 1 for old */
    rv = _bcm_dpp_am_template_ip_tunnel_exchange(unit,eep,!eep_alloced,tunnel->sip,ttl_encoded,tos_encoded,
                                                  &sip_template, &ttl_template, &tos_template,&is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    /* calc DF bit if enabled*/
    if(SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set){
        df_flag = (tunnel->flags & BCM_TUNNEL_INIT_IPV4_SET_DF)?1:0;
        /* always write to HW as this template may move from defragement to fragement area */
        df_updated = 1;
    }

    /* update indexes */
    ipv4_encap_info->dest.src_index = sip_template;
    ipv4_encap_info->dest.ttl_index = ttl_template;
    ipv4_encap_info->dest.tos_index = tos_template;
    _BCM_DPP_IPV4_TUNNEL_DF_IN_TOS_SET(ipv4_encap_info->dest.tos_index,df_flag);

    /* update type */
    rv = _bcm_petra_tunnel_ipv4_type_to_ppd(unit,tunnel,ipv4_encap_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if new allocated update hardware, one indication for all */
    if(is_allocated) {
        /* SIP */
        soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set(soc_sand_dev_id, sip_template, tunnel->sip);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
        /* TTL */
        soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set(soc_sand_dev_id, ttl_template, ttl_encoded);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
    }

    /* either new template, or due to fragement change */
    if (is_allocated || df_updated) {
        /* TOS */
        soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(soc_sand_dev_id, tos_template, tos_encoded);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * internal function to build ERSPAN tunnel.
 */
int _bcm_petra_tunnel_initiator_erspan_rspan_set(
   int unit, 
   bcm_l3_intf_t *intf, 
   bcm_tunnel_initiator_t *tunnel,
   int global_outlif /* outlif to use for ERSPAN tunnel */
   )
{
    int rv = BCM_E_NONE;
    bcm_mirror_destination_t
        mirror_dest;
    int
        dummy_global_outlif;
    uint8
        dummy_outlif_valid;
    int 
        replace=0;
    _bcm_lif_type_e usage;
    int local_lif = 0;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_mirror_destination_t_init(&mirror_dest);


    /* is this to update exist tunnel? */
    replace = (tunnel->flags & BCM_TUNNEL_REPLACE)?1:0;

    if (replace) {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_outlif, &local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT( _bcm_dpp_lif_usage_get(unit, -1, local_lif, NULL, &usage) );
        if (usage != _bcmDppLifTypeIpTunnel) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Can't replace OutLIF to IP Tunnel, because it's a different type (%d)"),usage));
        }
    }

    /* set ERSPAN tunnel in this place (if not zero)*/
    /* outlif calculated from tunnel->tunnel_id*/
    mirror_dest.encap_id = global_outlif;

    if (tunnel->type == bcmTunnelTypeErspan) {

        /* this mirror has to include GRE tunnel*/
        mirror_dest.flags = BCM_MIRROR_DEST_TUNNEL_IP_GRE;
        /* point to next tunnel in EEDB has to be IPv4 tunnel */
        mirror_dest.tunnel_id = tunnel->l3_intf_id;
        /* mirror has ERSPAN tunnel */
        mirror_dest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID;
        mirror_dest.span_id = tunnel->span_id;
    } else {
        mirror_dest.tpid =tunnel->tpid;
        mirror_dest.pkt_prio=tunnel->pkt_pri;
        mirror_dest.vlan_id=tunnel->vlan;
        mirror_dest.flags = BCM_MIRROR_DEST_TUNNEL_RSPAN;
    }

    /* call mirror internal function to create tunnel */
    rv = bcm_petra_mirror_destination_tunnel_create(
            unit,
            &mirror_dest,
            0,/*not in use */
            &dummy_global_outlif,
            &dummy_outlif_valid,
            replace
          );
    BCMDNX_IF_ERR_EXIT(rv);

    /* set return id, intf to include used outlif */
    rv = _bcm_petra_tunnel_id_eep_set(unit,intf,tunnel,mirror_dest.encap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (dummy_outlif_valid) {
        /* Add to SW DB */
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, dummy_global_outlif, &local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_out_lif_tunnel_initiator_match_add(unit, tunnel, local_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * l2 encap internal function used for allocatoion of l2 encap tunnel
 */
int
_bcm_petra_tunnel_l2_encap_data_set(
    int unit, 
    bcm_l3_intf_t *intf,
    bcm_tunnel_initiator_t *tunnel)
{

    int rv = BCM_E_NONE;
    int eep;
	uint32 flags=0;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(tunnel);

		/* for l2 encapsulation set using this function and done */
	if (tunnel->flags & BCM_TUNNEL_WITH_ID) {
		eep=tunnel->tunnel_id;
		flags|=BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
	}

	if (tunnel->flags & BCM_TUNNEL_REPLACE) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("BCM_TUNNEL_REPLACE is not supported for this tunnel type")));
	}

    rv = bcm_dpp_am_l2_encap_eep_alloc(unit, flags , &eep);
    if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("allocate bcmTunnelTypeL2EncapExternalCpu tunnel fail")));
    }
   
    rv = _bcm_dpp_out_lif_l2_encap_match_add(unit,eep);
    BCMDNX_IF_ERR_EXIT(rv);

    intf->l3a_intf_id = eep;
    BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id ,eep);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * globals
 */
int
bcm_petra_tunnel_initiator_data_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel,
    int vsi_param,
    int ll_eep_param,
    int *tunnel_eep)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO ipv4_encap_info;
    SOC_PPD_LIF_ID lif_id;
    int global_lif_id = 0;
    int is_tunneled = 0, local_out_lif=0, flags = 0, vsi = 0, ll_eep = 0;
    int eep_alloced=0;
    int index = 0;
    uint32 sip = 0;
    SOC_SAND_PP_IP_TTL ttl = 0;
    SOC_SAND_PP_IPV4_TOS tos = 0;
    SOC_PPD_FEC_ID fec_id;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE
      protect_type;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO
      working_fec;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO
      protect_fec;
    SOC_PPD_FRWRD_FEC_PROTECT_INFO
      protect_info;
    SOC_PPD_EG_ENCAP_DATA_INFO
      data_info;
    uint8 success;
    SOC_PPD_EG_ENCAP_ENTRY_INFO   encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32         next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32         nof_entries;
    uint32         ip_tunnel_alloc_flag=0;
    uint8 is_ipv6_tunnel;
    int update = 0, with_id = 0;
    _bcm_lif_type_e
        usage;
    uint8 is_mapped_lif = TRUE;
    int local_ll_outlif; 

    BCMDNX_INIT_FUNC_DEFS;


    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(intf);
    BCMDNX_NULL_CHECK(tunnel);

    SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO_clear(&ipv4_encap_info);

    soc_sand_dev_id = (unit);

    /* according to type identify if this ipv6 tunnel */
    is_ipv6_tunnel = _bcm_petra_tunnel_type_is_ipv6(unit,tunnel->type);
    /* alloc IPv4 or IPv6 tunnel */
    ip_tunnel_alloc_flag = (is_ipv6_tunnel)?BCM_DPP_AM_FLAG_ALLOC_IPV6:0;

    /* If mode is remark mapping, dscp should be 0*/
    if (tunnel->dscp_sel != bcmTunnelDscpAssign && tunnel->dscp != 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If dscp_sel is not bcmTunnelDscpAssign, dscp should be 0")));
    }

    /* verify parameters for IPv6 */
    if(is_ipv6_tunnel) {

        if(tunnel->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IPv6 tunnel initiator can be only Pipe")));
        }
        if(tunnel->flow_label != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flow label has to be zero")));
        }
        
        if(tunnel->dscp_sel != bcmTunnelDscpAssign) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IPv6 tunnel initiator can be only pipe ")));
        }
    }


    /* Read the state of the flags */
    update = (tunnel->flags & BCM_TUNNEL_REPLACE) ? TRUE : FALSE;
    with_id = (tunnel->flags & BCM_TUNNEL_WITH_ID) ? TRUE : FALSE;

    if (SOC_IS_PETRAB(unit)) {
        /* check if there is tunnel associated with intf at all */
        rv = _bcm_tunnel_intf_is_tunnel(unit, intf->l3a_intf_id, &is_tunneled, &local_out_lif, &vsi, &ll_eep);
        BCMDNX_IF_ERR_EXIT(rv);

        /* for encap interface get information from parameters*/
        if(_BCM_PETRA_L3_ITF_IS_ENCAP(intf->l3a_intf_id)) {
            vsi = vsi_param;
            ll_eep = ll_eep_param;
        }

        if(!is_tunneled) {
            if(_BCM_PETRA_L3_ITF_IS_RIF(intf->l3a_intf_id)) {
                ll_eep = ll_eep_param;
            }
        }

    }
    else{/* Arad */

        /* for ARAD interface for tunnel can be only EEP
         * EEP can based by tunnel_id if not zero. 
         * else by interface if not zero. 
         * otherwise eep is allocate from tunnel banks. 
         * and tunnel placements is stored in both tunnel_id and intf_id 
         */
        if (with_id) {
            rv = _bcm_petra_tunnel_id_eep_get(unit,intf,tunnel,&global_lif_id,&is_tunneled);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        vsi = tunnel->l3_intf_id;
    }

    /* above is common for all tunnel types: calculated eep/outlif to use */

    /* for erpsan set using this funtion and done */
    if (tunnel->type == bcmTunnelTypeErspan || tunnel->type == bcmTunnelTypeRspan) {
        rv = _bcm_petra_tunnel_initiator_erspan_rspan_set(unit, intf, tunnel, global_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    /* if tunneled reuse same eep */
    if (is_tunneled) {
        flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        if (SOC_IS_JERICHO(unit)) {
            rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif_id);
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = bcm_dpp_am_ip_tunnel_eep_is_alloced(unit,ip_tunnel_alloc_flag,global_lif_id);
        }

        if (rv == BCM_E_PARAM) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("check IP tunnel allocation")));
        }
        eep_alloced = (rv == BCM_E_EXISTS)?1:0;
    } else {   /* if not tunneled alloc new eep */
        flags = 0;
        eep_alloced = 0;
    }

    if (SOC_IS_ARAD(unit)) {
        if (update) {
            if (!with_id) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_TUNNEL_REPLACE can't be used without BCM_TUNNEL_WITH_ID as well")));
            }
            else if (!eep_alloced) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("In case of BCM_TUNNEL_REPLACE, the tunnel_id must exist")));
            }

            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_out_lif);
            BCMDNX_IF_ERR_EXIT(rv);


            /* Check that eep is an index of a Tunnel OutLIF (in SW DB) */
            BCMDNX_IF_ERR_EXIT( _bcm_dpp_lif_usage_get(unit, -1, local_out_lif, NULL, &usage) );
            if (usage != _bcmDppLifTypeIpTunnel) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Can't replace OutLIF to IP Tunnel, because it's a different type (%d)"),usage));
            }

        } else if (with_id && eep_alloced){
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case tunnel_id already exists, BCM_TUNNEL_REPLACE should be used")));
        }
    }


    /* get local LL outlif from global LL outlif.
       the interface object l3_intf_id of type encap contains global LL outlif */
    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, _BCM_PETRA_L3_ITF_VAL_GET(tunnel->l3_intf_id), &local_ll_outlif);
    BCMDNX_IF_ERR_EXIT(rv);

    if(!eep_alloced) {/* need to allocate IP tunnel */
        uint32 ip_tunnel_eep_alloc_type = _BCM_DPP_AM_IP_TUNNEL_EEP_TYPE_DEFAULT; /* type of eedb ip tunnel allocation */

        /* start allocation from LIF to ensure synced */
        /* mark LIF as allocated only For Soc_petra-B, in ARAD, sync done by alloc-mngr */
        if(_Bcm_dpp_sync_lif_eep && SOC_IS_PETRAB(unit)) {
            lif_id = local_out_lif;
            rv = bcm_dpp_am_l3_lif_ip_tnl_alloc(unit, flags, &lif_id);
            BCM_IF_ERROR_RETURN(rv);
    
            flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
            local_out_lif = lif_id;
        }

        /* identify type of eedb ip tunnel allocation 
         * - for IP tunnel eedb entry in ROO application
           - for all other applications: default */
        if (SOC_IS_ARADPLUS(unit)) {
            SOC_PPD_EG_ENCAP_ENTRY_TYPE entry_type; 


            rv = soc_ppd_eg_encap_entry_type_get(unit, local_ll_outlif, &entry_type);
            BCM_IF_ERROR_RETURN(rv);


            if (((entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA && SOC_IS_ARADPLUS_A0(unit)) 
                 || (entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP))    
                && SOC_IS_ROO_ENABLE(unit)) {
                ip_tunnel_eep_alloc_type = _BCM_DPP_AM_IP_TUNNEL_EEP_TYPE_ROO;
            } 
        }

        if (SOC_IS_JERICHO(unit)) {
            bcm_dpp_am_local_out_lif_info_t outlif_info;
            uint32 local_lif_flags = 0;

            /* Check if lif is direct or mapped */
            if (with_id) {
                rv = _bcm_dpp_lif_id_is_mapped(unit, FALSE, global_lif_id, &is_mapped_lif);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* If lif is not mapped, don't allocate global lif.
               If lif is mapped, the local lif should be equal to the given lif id. */
            if (is_mapped_lif) {
                rv = bcm_dpp_am_global_lif_alloc(unit, flags | BCM_DPP_AM_FLAG_ALLOC_EGRESS, &global_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                local_lif_flags = flags;
                outlif_info.base_lif_id = global_lif_id;
            }
            

            /* Set the outlif info struct */
            outlif_info.app_alloc_info.pool_id =  (is_ipv6_tunnel) ? dpp_am_res_eep_ip_tnl : dpp_am_res_ipv6_tunnel;
            outlif_info.app_alloc_info.application_type = (ip_tunnel_eep_alloc_type == _BCM_DPP_AM_IP_TUNNEL_EEP_TYPE_ROO) ?
                                                           bcm_dpp_am_egress_encap_app_ip_tunnel_roo : 0;
            outlif_info.local_lif_flags = (is_mapped_lif) ? BCM_DPP_AM_OUT_LIF_FLAG_COUPLED : BCM_DPP_AM_OUT_LIF_FLAG_DIRECT;
            outlif_info.glif = global_lif_id;

            rv = _bcm_dpp_am_local_outlif_alloc(unit, local_lif_flags, &outlif_info);
            BCMDNX_IF_ERR_EXIT(rv);

            local_out_lif = outlif_info.base_lif_id;

        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {

            rv = bcm_dpp_am_ip_tunnel_eep_alloc(unit, ip_tunnel_eep_alloc_type, flags|ip_tunnel_alloc_flag, &global_lif_id);
            if (rv != BCM_E_NONE) {
                    BCMDNX_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("allocate IPv4/IPv6 tunnel fail")));
            }

            local_out_lif = global_lif_id;
        }
    }

    if(eep_alloced) {
        soc_sand_rv =
            soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                   SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                   local_out_lif, 1,
                                   encap_entry_info, next_eep,
                                   &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else{
        next_eep[0] = SOC_PPD_EG_ENCAP_NEXT_EEP_INVALID;
    }

    if (SOC_IS_PETRAB(unit)) {
        /* set VSI for last EEP */
        ipv4_encap_info.dest.dest = tunnel->dip;
        ipv4_encap_info.dest.enable_gre = FALSE;
        ipv4_encap_info.out_vsi = vsi;

        /* set SIP, TTL, TOS, this values are profiles into tunnel data */

        for (index = 0; index < 16; index++) {
    	if (bcm_dpp_am_ip_tunnel_glbl_src_ip_is_alloced(unit, index) == BCM_E_NOT_FOUND) 
    	    continue;
    	
    	soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get(soc_sand_dev_id, index, &sip);
    	SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    	if (sip == tunnel->sip) break;
        }
    	
        flags = 0;
        if (index >= 16) {
    	rv = bcm_dpp_am_ip_tunnel_glbl_src_ip_alloc(unit, flags, &index);
    	if (rv != BCM_E_NONE) {
    	    BCMDNX_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("Global Source IP resource unavailable")));
    	}
    	soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set(soc_sand_dev_id, 
    							   index, 
    							   tunnel->sip);
    	SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
        }
        ipv4_encap_info.dest.src_index = index;

        for (index = 0; index < 4; index++) {
        	if (bcm_dpp_am_ip_tunnel_glbl_ttl_is_alloced(unit, index) == BCM_E_NOT_FOUND) 
        	    continue;
    	
        	soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get(soc_sand_dev_id, index, &ttl);
        	SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

        	if (ttl == tunnel->ttl) break;
        }
	
        flags = 0;
        if (index >= 4) {
        	rv = bcm_dpp_am_ip_tunnel_glbl_ttl_alloc(unit, flags, &index);
        	if (rv != BCM_E_NONE) {
        	    BCMDNX_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("Global TTL resource unavailable")));
        	}
        	soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set(soc_sand_dev_id, 
        							index, 
        							tunnel->ttl);
        	SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
        }

        ipv4_encap_info.dest.ttl_index = index;

        for (index = 0; index < 16; index++) {
        	if (bcm_dpp_am_ip_tunnel_glbl_tos_is_alloced(unit, index) == BCM_E_NOT_FOUND) 
        	    continue;
        	
        	soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get(soc_sand_dev_id, index, &tos);
        	SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

        	if (tos == tunnel->dscp) break;
        }
    	
        flags = 0;
        if (index >= 16) {
        	rv = bcm_dpp_am_ip_tunnel_glbl_tos_alloc(unit, flags, &index);
        	if (rv != BCM_E_NONE) {
        	    BCMDNX_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("Global TOS resource unavailable")));
        	}
        	soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(soc_sand_dev_id, 
            							index, 
            							tunnel->dscp);
          	SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
        }
        ipv4_encap_info.dest.tos_index = index;

        /* write to HW entry */
        soc_sand_rv =
        soc_ppd_eg_encap_ipv4_entry_add(soc_sand_dev_id, local_out_lif, 
                        &ipv4_encap_info, 0);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

        /* update SW according to allocated tunnel ID */
        /* mark this interface as tunneled */
        rv = _bcm_tunnel_intf_eep_set(unit, intf->l3a_intf_id, local_out_lif);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("EEP set failed")));
        }

        /* update interface as tunneled */
        if(_BCM_PETRA_L3_ITF_IS_RIF(intf->l3a_intf_id)) {
            /* mark this interface as tunneled */
            rv = _bcm_tunnel_intf_eep_set(unit, intf->l3a_intf_id, local_out_lif);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /* FEC */
        /* if not already tunneled then update FEC with new EEP */
        if(_BCM_PETRA_L3_ITF_IS_FEC(intf->l3a_intf_id) && !is_tunneled) {

            _bcm_l3_intf_to_fec(unit, intf->l3a_intf_id, &fec_id);

            soc_sand_rv =
                soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, fec_id,
                                        &protect_type, &working_fec,
                                        &protect_fec, &protect_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            working_fec.eep = local_out_lif;

            soc_sand_rv =
                soc_ppd_frwrd_fec_entry_add(soc_sand_dev_id, fec_id,
                                            protect_type, &working_fec,
                                            &protect_fec, &protect_info, &success);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
         }
    }
#ifdef BCM_ARAD_SUPPORT
    else { /* ARAD */
        /* set tunnel info into PPD */

        /* IP tunnel points to link layer. l3_intf_id is an outRif or an encap. */
        next_eep[0] = local_ll_outlif; 

        /* according to TYPE fill tunnel data */
        if(!is_ipv6_tunnel) {
            
            /* Set the condition to create a data entry for BFD packets over IPv4 */
            if (SOC_DPP_CONFIG(unit)->pp.bfd_extended_ipv4_src_ip
                && (tunnel->type == bcmTunnelTypeIpAnyIn4)
                && (tunnel->sip != 0) && (tunnel->dip == 0))
            {
                SOC_PPD_EG_ENCAP_DATA_INFO_clear(&data_info);
                SOC_PPD_EG_ENCAP_DATA_BFD_IPV4_FORMAT_SET(unit, tunnel->sip, tunnel->aux_data, &data_info);
                soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id, local_out_lif, &data_info, TRUE, next_eep[0]);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            else
            {
                rv = _bcm_petra_tunnel_init_ipv4_to_ppd(unit,tunnel,local_out_lif,eep_alloced,&ipv4_encap_info);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Can't replace oam_lif_set and drop bits */
                if (update) {
                    ipv4_encap_info.oam_lif_set = encap_entry_info[0].entry_val.ipv4_encap_info.oam_lif_set;
                    ipv4_encap_info.drop = encap_entry_info[0].entry_val.ipv4_encap_info.drop;
                }

                /* write to HW entry */
                soc_sand_rv =
                soc_ppd_eg_encap_ipv4_entry_add(soc_sand_dev_id, local_out_lif,
                                &ipv4_encap_info, next_eep[0]);
                SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
            }
        }
        else{/* IPv6 */
            SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO 
                ipv6_encap_info;

            SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO_clear(&ipv6_encap_info);

            rv = _bcm_petra_tunnel_init_ipv6_to_ppd(unit,tunnel,&ipv6_encap_info);
            BCMDNX_IF_ERR_EXIT(rv);

            /* write to HW entry*/
            soc_sand_rv =
            soc_ppd_eg_encap_ipv6_entry_add(soc_sand_dev_id, local_out_lif, 
                            &ipv6_encap_info, next_eep[0]);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
        }

        /* Write global lif mapping to the glem. */
        if (SOC_IS_JERICHO(unit) && is_mapped_lif && !eep_alloced) {
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_glem_access_entry_add, (unit, global_lif_id, local_out_lif));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        
        /* update SW according to allocated tunnel ID */
        rv = _bcm_petra_tunnel_id_eep_set(unit,intf,tunnel,global_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Add to SW DB */
        rv = _bcm_dpp_out_lif_tunnel_initiator_match_add(unit,tunnel,local_out_lif);
        BCMDNX_IF_ERR_EXIT(rv);

    }
#endif

    *tunnel_eep = local_out_lif;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_tunnel_initiator_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API no longer supported. Please use bcm_petra_tunnel_initiator_create instead.")));

  exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_tunnel_initiator_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    int tunnel_eep;
    int support;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(tunnel);
    BCMDNX_NULL_CHECK(intf);

    /* L2 tunneling  */
    if (tunnel->type == bcmTunnelTypeL2EncapExternalCpu){
        /* This API is supported for ARAD+ and above */
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("L2 tunnel from this API is supported only for ARAD+ and above")));
        }

        rv = _bcm_petra_tunnel_l2_encap_data_set(unit,intf,tunnel);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    BCM_DPP_TUNNEL_INIT_CHECK;

    /* Validate ttl. */
    if (!BCM_TTL_VALID(tunnel->ttl)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Param: tunnel->ttl is not valid. Valid range is 0-255")));
    }

    /* Validate IP tunnel DSCP SEL. */
    if (tunnel->dscp_sel > 2 || tunnel->dscp_sel < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Param: tunnel->dscp_sel is not valid. Valid range is 0-1.")));
    }

    /* Validate IP tunnel DSCP value. */
    if (tunnel->dscp > 63 || tunnel->dscp < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Param: tunnel->dscp is not valid. Valid range is 0-63.")));
    }

    if (SOC_IS_PETRAB(unit)) {
        /* combination for PIPE unfirom*/
        /* cannot keep one of: TTL, DSCP. both or none*/
        if((tunnel->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL) || (tunnel->dscp_sel != bcmTunnelDscpAssign))
        {
            LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                        (BSL_META_U(unit,
                                    "unsupported TTL, DSCP setting only PIPE (assign is supported) %x\n"), tunnel->flags));
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unsupported TTL, DSCP setting only PIPE (assign is supported)")));
        }
    }

    rv = _bcm_petra_tunnel_type_support_check(unit, tunnel->type, NULL, &support);
    if (rv != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("IP tunnel not supported")));
    }

    if (FALSE == support) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("Tunnel type not supported.")));
    }

    /* Defragement is set, while not supported */
    if(tunnel->flags & BCM_TUNNEL_INIT_IPV4_SET_DF && !SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("Defragement set not supported.")));
    }


    /* ERSPAN from this API is supported for ARAD+ and above */
    if (tunnel->type == bcmTunnelTypeErspan && SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("Erspan tunnel from this API is supported only for ARAD+ and above")));
    }


    rv = bcm_petra_tunnel_initiator_data_set(unit, 
					     intf, 
					     tunnel,
					     0,
					     0,
					     &tunnel_eep);
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_PETRAB(unit)) {
        tunnel->tunnel_id = tunnel_eep;
        intf->l3a_tunnel_idx = tunnel_eep;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_tunnel_initiator_create(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_tunnel_initiator_set(unit, intf, tunnel);
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCM_EXIT;
  exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_tunnel_initiator_clear(
    int unit, 
    bcm_l3_intf_t *intf)
{
    int global_lif_id, local_out_lif, is_tunneled, vsi, ll_eep;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32 next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES], nof_entries, index;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    uint32         ip_tunnel_alloc_flag=0;
    uint8 is_ipv6_tunnel=0;
    _bcm_lif_type_e lif_usage;
#ifdef BCM_ARAD_SUPPORT
    int sip_template, ttl_template, tos_template,is_allocated;
#endif

    BCMDNX_INIT_FUNC_DEFS;
 
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    if(SOC_IS_PETRAB(unit)) {
        /* check if there is tunnel associated with intf at all */
        rv = _bcm_tunnel_intf_is_tunnel(unit, intf->l3a_intf_id, &is_tunneled, &global_lif_id, &vsi, &ll_eep);
        BCM_IF_ERROR_RETURN(rv);
    }
    else{
        /* get eep from interface */
        rv = _bcm_petra_tunnel_id_eep_get(unit,intf,0,&global_lif_id,&is_tunneled);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_usage_get(unit, -1, local_out_lif, NULL, &lif_usage));

    if(lif_usage == _bcmDppLifTypeCpuL2Encap) {

        /* check if this tunnel is allocated */
        rv = bcm_dpp_am_l2_encap_eep_is_alloced(unit,local_out_lif);
        if (rv == BCM_E_NOT_FOUND) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found on this interface")));
        }
        rv = bcm_dpp_am_l2_encap_eep_dealloc(unit,local_out_lif);
            BCM_IF_ERROR_RETURN(rv);

		BCMDNX_IF_ERR_EXIT(_bcm_dpp_out_lif_match_delete(unit,local_out_lif));

		BCM_EXIT;
    }

	if (lif_usage != _bcmDppLifTypeIpTunnel) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("OutLIF is not a tunnel")));
    }

    /* if not then no tunnel, nothing to do */
    if (is_tunneled == 0) {
        return BCM_E_NOT_FOUND;
    }

    /* read entry from HW */
    soc_sand_rv = soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                     SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_out_lif, 1,
                     encap_entry_info, next_eep, &nof_entries);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    if(encap_entry_info[0].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) {
        is_ipv6_tunnel = 1;
    }
    else if(encap_entry_info[0].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
        is_ipv6_tunnel = 0;
    }
    /* ERSPAN / RSPAN*/
    else if (encap_entry_info[0].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA && SOC_PPD_EG_ENCAP_DATA_FORMAT_GET(soc_sand_dev_id,&(encap_entry_info->entry_val.data_info))) {
        rv = bcm_petra_mirror_destination_tunnel_destroy(unit, global_lif_id, local_out_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Remove entry from SW DB */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_out_lif_match_delete(unit,local_out_lif));

        /* done */
        BCM_EXIT;
    }
    else{
        LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                    (BSL_META_U(unit,
                                "given interface doesn't include ip-tunnel intf %x\n"), intf->l3a_intf_id));
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("given interface doesn't include ip-tunnel")));
    }

    ip_tunnel_alloc_flag = (is_ipv6_tunnel)?BCM_DPP_AM_FLAG_ALLOC_IPV6:0;

   /* check if this tunnel is allocated */
    if (SOC_IS_JERICHO(unit)) {
        rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif_id);
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_PETRAB(unit)) {
        rv = bcm_dpp_am_ip_tunnel_eep_is_alloced(unit,ip_tunnel_alloc_flag,global_lif_id);
    }
    if (rv == BCM_E_NOT_FOUND) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found on this interface")));
    }

    
    if(SOC_IS_PETRAB(unit)) {
        /* remove tunnel */
        if(_BCM_PETRA_L3_ITF_IS_RIF(intf->l3a_intf_id)) {
           /* mark this interface as not tunneled */
            
            rv = _bcm_tunnel_intf_eep_set(unit, intf->l3a_intf_id, 0);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    if(SOC_IS_PETRAB(unit)) {
        /* free used EEP entry in hw */
        soc_sand_rv = soc_ppd_eg_encap_entry_remove(soc_sand_dev_id,SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,local_out_lif);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        index = encap_entry_info[0].entry_val.ipv4_encap_info.dest.src_index;
        if (bcm_dpp_am_ip_tunnel_glbl_src_ip_is_alloced(unit, index) == BCM_E_EXISTS) {
            soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set(soc_sand_dev_id, index, 0);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
            rv = bcm_dpp_am_ip_tunnel_glbl_src_ip_dealloc(unit, index);
            BCM_IF_ERROR_RETURN(rv);
        }

        index = encap_entry_info[0].entry_val.ipv4_encap_info.dest.ttl_index;
        if (bcm_dpp_am_ip_tunnel_glbl_ttl_is_alloced(unit, index) == BCM_E_EXISTS) {
            soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set(soc_sand_dev_id, index, 0);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
            rv = bcm_dpp_am_ip_tunnel_glbl_ttl_dealloc(unit, index);
            BCM_IF_ERROR_RETURN(rv);
        }

        index = encap_entry_info[0].entry_val.ipv4_encap_info.dest.tos_index;
        if (bcm_dpp_am_ip_tunnel_glbl_tos_is_alloced(unit, index) == BCM_E_EXISTS) {
            soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(soc_sand_dev_id, index, 0);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
            rv = bcm_dpp_am_ip_tunnel_glbl_tos_dealloc(unit, index);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* mark this interface as not tunneled */
        rv = _bcm_tunnel_intf_eep_set(unit,intf->l3a_intf_id, 0);
        BCMDNX_IF_ERR_EXIT(rv);

        /* free memory */
        rv = bcm_dpp_am_l3_lif_ip_tnl_dealloc(unit, local_out_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_dpp_am_ip_tunnel_eep_dealloc(unit, ip_tunnel_alloc_flag, local_out_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else{ /* is-soc_petra */
#ifdef BCM_ARAD_SUPPORT
        /* free used profiles.
           ip tunnel profiles are used only for ipv4 tunnels*/
        if (!is_ipv6_tunnel) {
            rv = _bcm_dpp_am_template_ip_tunnel_exchange(unit,local_out_lif,0 /* exist entry */,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
                                                          &sip_template, &ttl_template, &tos_template,&is_allocated);
            BCMDNX_IF_ERR_EXIT(rv);

            /* free used eep */
            if (SOC_IS_JERICHO(unit)) {
                rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_out_lif, TRUE);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = bcm_dpp_am_ip_tunnel_eep_dealloc(unit,ip_tunnel_alloc_flag,local_out_lif);
                BCM_IF_ERROR_RETURN(rv);
            }
        }

        /* free used EEP entry in hw */
        soc_sand_rv = soc_ppd_eg_encap_entry_remove(soc_sand_dev_id,SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,local_out_lif);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Remove entry from SW DB */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_out_lif_match_delete(unit,local_out_lif));
#endif
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_tunnel_initiator_get(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    int global_lif, local_out_lif, is_tunneled;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO ipv4_tunnel_info;
    uint32 next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES], nof_entries;
    int vsi, ll_eep, index;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 sip = 0;
    SOC_SAND_PP_IP_TTL ttl = 0;
    SOC_SAND_PP_IPV4_TOS tos = 0;
    uint32         ip_tunnel_alloc_flag=0;
    uint8 is_ipv6_tunnel;
    _bcm_lif_type_e lif_usage;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(intf);
    BCMDNX_NULL_CHECK(tunnel);

    soc_sand_dev_id = (unit);
    SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO_clear(&ipv4_tunnel_info);

    is_ipv6_tunnel = _bcm_petra_tunnel_type_is_ipv6(unit,tunnel->type);
    ip_tunnel_alloc_flag = (is_ipv6_tunnel)?BCM_DPP_AM_FLAG_ALLOC_IPV6:0;

    /* check if there is tunnel associated with intf at all */
    if(SOC_IS_PETRAB(unit)) {
        rv = _bcm_tunnel_intf_is_tunnel(unit, intf->l3a_intf_id, &is_tunneled, &global_lif, &vsi, &ll_eep);
        BCM_IF_ERROR_RETURN(rv);
    }
    else{
        /* check if given interface is tunnel interface */
        rv = _bcm_petra_tunnel_id_eep_get(unit,intf,tunnel,&global_lif,&is_tunneled);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);


    /* if not then no labels */
    if (is_tunneled == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found on this interface")));
    }
    
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_usage_get(unit, -1, local_out_lif, NULL, &lif_usage));
	if(lif_usage == _bcmDppLifTypeCpuL2Encap) {

        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tunnel _bcmDppLifTypeCpuL2Encap is not supportd by this API")));
    }
    if (lif_usage != _bcmDppLifTypeIpTunnel) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("OutLIF is not a tunnel")));
    }

    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_out_lif, 1,
                               encap_entry_info, next_eep, &nof_entries);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    if(SOC_IS_PETRAB(unit)) {

        /* check if this tunnel is allocated */
        rv = bcm_dpp_am_ip_tunnel_eep_is_alloced(unit,ip_tunnel_alloc_flag,local_out_lif);
        if (rv == BCM_E_NOT_FOUND) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found on this interface")));
        }

        if (encap_entry_info[0].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {  /* ip tunnel encap */
            ipv4_tunnel_info.dest =
                encap_entry_info[0].entry_val.ipv4_encap_info.dest.dest;
            ipv4_tunnel_info.src_index =
                encap_entry_info[0].entry_val.ipv4_encap_info.dest.src_index;
            ipv4_tunnel_info.ttl_index =
                encap_entry_info[0].entry_val.ipv4_encap_info.dest.ttl_index;
            ipv4_tunnel_info.tos_index =
                encap_entry_info[0].entry_val.ipv4_encap_info.dest.tos_index;
            ipv4_tunnel_info.enable_gre =
                encap_entry_info[0].entry_val.ipv4_encap_info.dest.enable_gre;
            ipv4_tunnel_info.encapsulation_mode =
                encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode;

            tunnel->dip = ipv4_tunnel_info.dest;
            tunnel->tunnel_id = global_lif;
	
            index = encap_entry_info[0].entry_val.ipv4_encap_info.dest.src_index;
            if (bcm_dpp_am_ip_tunnel_glbl_src_ip_is_alloced(unit, index) == BCM_E_EXISTS) {
                soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get(soc_sand_dev_id, index, &sip);
                SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
                tunnel->sip = sip;
            }
	
            index = encap_entry_info[0].entry_val.ipv4_encap_info.dest.ttl_index;
            if (bcm_dpp_am_ip_tunnel_glbl_ttl_is_alloced(unit, index) == BCM_E_EXISTS) {
                soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get(soc_sand_dev_id, index, &ttl);
                SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
                tunnel->ttl = ttl;
            }
	
            index = encap_entry_info[0].entry_val.ipv4_encap_info.dest.tos_index;
            if (bcm_dpp_am_ip_tunnel_glbl_tos_is_alloced(unit, index) == BCM_E_EXISTS) {
                soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get(soc_sand_dev_id, index, &tos);
                SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
                tunnel->dscp = tos;
            }
        }
    }
    else{
        _bcm_lif_type_e usage; 
        int global_next_eep;

        /* point to next intf/tunnel */

        /* Check in SW DB that the outLIF is LL for overlay (build by l2_egress_create),
           If so, l3_intf_id is an encap */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_usage_get(unit, -1,  next_eep[0], NULL, &usage));

        /* Convert the next eep to global lif before it can be exposed to the user. */
        rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, next_eep[0], &global_next_eep);
        BCMDNX_IF_ERR_EXIT(rv);

        if (usage == _bcmDppLifTypeOverlayLinkLayer) {
            _BCM_PETRA_L3_ITF_SET(tunnel->l3_intf_id, _BCM_PETRA_L3_ITF_ENCAP, global_next_eep); 
        } 
        /* otherwise it's an outRif. */
        else {
            _BCM_PETRA_L3_ITF_SET(tunnel->l3_intf_id, _BCM_PETRA_L3_ITF_RIF, global_next_eep);
        }

        /* check outlif allocated for the configured usage */
        if (encap_entry_info[0].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP || encap_entry_info[0].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) {

            /* check if this tunnel is allocated */
            if (SOC_IS_JERICHO(unit)) {
                rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif);
            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = bcm_dpp_am_ip_tunnel_eep_is_alloced(unit,ip_tunnel_alloc_flag,global_lif);
            }
            if (rv == BCM_E_NOT_FOUND) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found on this interface")));
            }
        }


        if (encap_entry_info[0].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {

            /* set ipv4 tunnel info */
            rv = _bcm_petra_tunnel_init_ipv4_from_ppd(unit,tunnel,&encap_entry_info[0].entry_val.ipv4_encap_info);
            BCMDNX_IF_ERR_EXIT(rv);

        }
        else if (encap_entry_info[0].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) {
            /* set IPv6 tunnel info */
            rv = _bcm_petra_tunnel_init_ipv6_from_ppd(unit,tunnel,&encap_entry_info[0].entry_val.ipv6_encap_info);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else if ((encap_entry_info[0].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA)
                 && (SOC_PPD_EG_ENCAP_DATA_FORMAT_GET(soc_sand_dev_id,&(encap_entry_info->entry_val.data_info))))
        {
            if (SOC_IS_JERICHO(unit)) {
                rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif);
            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = bcm_dpp_am_eg_data_erspan_is_alloced(unit,global_lif);
            }
            if (rv == BCM_E_NOT_FOUND) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not allocated ")));
            }

            if (SOC_PPD_EG_ENCAP_EEDB_IS_FORMAT_ERSPAN(&(encap_entry_info->entry_val.data_info))) {
                /* set ERSPAN tunnel info */
                tunnel->type = bcmTunnelTypeErspan;
                tunnel->span_id = SOC_PPD_EG_ENCAP_DATA_ERSPAN_FORMAT_SPAN_ID_GET(soc_sand_dev_id,&(encap_entry_info->entry_val.data_info));
            } else { /* RSPAN*/
                tunnel->type = bcmTunnelTypeRspan;
                SOC_PPD_EG_ENCAP_DATA_RSPAN_FORMAT_GET(tunnel->tpid, tunnel->pkt_pri , tunnel->vlan ,&(encap_entry_info->entry_val.data_info));
            }
            rv = _bcm_petra_tunnel_id_eep_set(unit, intf, tunnel, global_lif);
            BCMDNX_IF_ERR_EXIT(rv); 

        } 
        else if((encap_entry_info[0].entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA)
                && (SOC_DPP_CONFIG(unit)->pp.bfd_extended_ipv4_src_ip)
                && (tunnel->type == bcmTunnelTypeIpAnyIn4))
        {
            int bfd_sip = 0;
            int aux_data = 0;
            int prg_val = 0;

            SOC_PPD_EG_ENCAP_DATA_BFD_IPV4_FORMAT_GET(unit, prg_val, bfd_sip, aux_data, &(encap_entry_info->entry_val.data_info));
            if (prg_val == SOC_PPD_EG_ENCAP_BFD_IPV4_PROG_VAR) {
                tunnel->sip = bfd_sip;
                tunnel->aux_data = aux_data;
            }
        }
        else{
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("expected to IP or ERSPAN tunnel entry ")));
        }

        /* set ID */
        rv = _bcm_petra_tunnel_id_eep_set(unit,intf,tunnel,global_lif);
        BCMDNX_IF_ERR_EXIT(rv);

    }

exit:
    BCMDNX_FUNC_RETURN;
}





int
bcm_petra_tunnel_term_key_to_ppd(
    int unit, 
    bcm_tunnel_terminator_t *info,
    SOC_PPD_RIF_IP_TERM_KEY    *term_key
 ) 
{
    int rv = BCM_E_NONE;
    uint32 port_property;

    BCMDNX_INIT_FUNC_DEFS;

    /* IPv6, termination according to DIP */
    if(info->type == bcmTunnelTypeIpAnyIn6) {
        term_key->dip6.prefix_len = bcm_ip6_mask_length(info->dip6_mask);
        rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->dip6, &(term_key->dip6.ipv6_address));
        term_key->flags = SOC_PPD_RIF_IP_TERM_FLAG_IPV6;
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    /* SIP termination for vxlan and l2gre:
       the taget of this termination is mapping SIP to Learning info
       in this case only sem-result-index is used (by PMF),
       sem-result table is not configured.
     */
    if (info->type == bcmTunnelTypeVxlan || info->type == bcmTunnelTypeL2Gre) {
        if(info->sip_mask != 0 && info->dip_mask == 0) {
            term_key->sip = info->sip;
            term_key->sip_prefix_len = bcm_ip_mask_length(info->sip_mask);
            term_key->flags |= SOC_PPD_RIF_IP_TERM_FLAG_KEY_ONLY; /* only add to SEM */
        }
    }


    if(info->sip_mask != 0) {
        term_key->sip = info->sip;
        term_key->sip_prefix_len = bcm_ip_mask_length(info->sip_mask);
    }
    /* key includes DIP */
    if(info->dip_mask != 0) {
        term_key->dip = info->dip;
        term_key->dip_prefix_len = bcm_ip_mask_length(info->dip_mask);
    }

    /* key includes IPV4 next protocol */
    switch (info->type) {
    case bcmTunnelTypeIp4In4:
        term_key->ipv4_next_protocol_prefix_len = 8;
        term_key->ipv4_next_protocol = _BCM_PETRA_TUNNEL_NEXT_PRTCL_IPV4; /* IPV4 */
        break;
    case bcmTunnelTypeIp6In4:
        term_key->ipv4_next_protocol_prefix_len = 8;
        term_key->ipv4_next_protocol = _BCM_PETRA_TUNNEL_NEXT_PRTCL_IPV6; /* IPV6 */
        break;
    case bcmTunnelTypeGreAnyIn4:
        term_key->ipv4_next_protocol_prefix_len = 8;
        term_key->ipv4_next_protocol = _BCM_PETRA_TUNNEL_NEXT_PRTCL_GRE; /* GRE */
        break;
    default:
        term_key->ipv4_next_protocol_prefix_len = 0;
    }

    /* In case key includes ipv4 next protocol, check if port property is also part of the key. */
    if (term_key->ipv4_next_protocol_prefix_len) {
        BCM_PBMP_ITER(info->pbmp, port_property){
            /* If port property is in the legal range, keep it. Otherwise, return error. */
            if (port_property <= SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX) {
                term_key->port_property_en = 1;
                term_key->port_property = port_property;
                break;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given port property is too high.")));
            }
        }

        /* Port property not found */
        if (port_property > SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX) {
            term_key->port_property_en = 0;
        }
    }

    /* Indicate it's IP-LIF-Dummy */
    if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id &&
        (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) &&
        ((SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id) == BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id))) {
        term_key->flags |= SOC_PPD_RIF_IP_TERM_FLAG_USE_DIP_DUMMY;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_tunnel_term_key_from_ppd(
    int unit, 
    bcm_tunnel_terminator_t *info,
    SOC_PPD_RIF_IP_TERM_KEY    *term_key
 ) 
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;


    if(term_key->flags == SOC_PPD_RIF_IP_TERM_FLAG_IPV6) {
        info->type = bcmTunnelTypeIpAnyIn6;
        rv = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit, &(term_key->dip6.ipv6_address),&info->dip6);
        BCMDNX_IF_ERR_EXIT(rv);
       
        rv = bcm_ip6_mask_create(info->dip6_mask, term_key->dip6.prefix_len);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    info->dip = term_key->dip;
    info->dip_mask = bcm_ip_mask_create(term_key->dip_prefix_len);

    info->sip = term_key->sip;
    info->sip_mask = bcm_ip_mask_create(term_key->sip_prefix_len);

    /* In case next protocol is enabled */
    if (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL &&
        term_key->ipv4_next_protocol_prefix_len == 8) {
        switch(term_key->ipv4_next_protocol) {
        case _BCM_PETRA_TUNNEL_NEXT_PRTCL_IPV4:
          info->type = bcmTunnelTypeIp4In4;
          break;
        case _BCM_PETRA_TUNNEL_NEXT_PRTCL_IPV6:
          info->type = bcmTunnelTypeIp6In4;
          break;
        case _BCM_PETRA_TUNNEL_NEXT_PRTCL_GRE:
          info->type = bcmTunnelTypeGreAnyIn4;
          break;
        default:
          break;
        }        

        /* If entry is using port property, fill pbmp */
        if (term_key->port_property_en) {
            BCM_PBMP_PORT_SET(info->pbmp, term_key->port_property);
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_tunnel_terminator_add(
    int unit, 
    bcm_tunnel_terminator_t *info) 
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API no longer supported. Please use bcm_petra_tunnel_terminator_create instead.")));

  exit:
    BCMDNX_FUNC_RETURN;

}

/* Add a tunnel terminator for DIP-SIP key. */
int 
_bcm_petra_tunnel_terminator_add(
    int unit, 
    bcm_tunnel_terminator_t *info) 
{
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPD_RIF_IP_TERM_KEY term_key;
    SOC_PPD_RIF_IP_TERM_INFO term_info;
    SOC_SAND_SUCCESS_FAILURE success;
    int global_lif, local_inlif;
    int rv = BCM_E_NONE;
    int is_uniform=0,is_1_plus_1,update=0,with_id=0,lif_allocated=0;
    uint32 alloc_flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(info);

    /* check parameters */

    /* Check for valid flags */
    if (info->flags & _BCM_PETRA_TUNNEL_TERM_FLAGS_UNSUPPORTED){
        LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                    (BSL_META_U(unit,
                                "unsupported flags %x\n"), info->flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("flags value not supported")));
    }

    /* combination for PIPE unfirom*/
    /* cannot keep one of: TTL, DSCP. both or none*/
    if(((info->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL) && !(info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP))||
       (!(info->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL) && (info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP))
       )
    {
        LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                    (BSL_META_U(unit,
                                "unsupported flags combination for TTL, DSCP %x\n"), info->flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unsupported flags combination for TTL, DSCP ")));
    }

    /* skip ethernet is supported only for arad+ */
    if ((SOC_IS_ARAD_B1_AND_BELOW(unit) || SOC_IS_JERICHO(unit))
        && (info->flags & BCM_TUNNEL_TERM_ETHERNET)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("flags value not supported below 88660_A0")));
    }

    soc_sand_dev_id = (unit);
    SOC_PPD_RIF_IP_TERM_INFO_clear(&term_info);
    SOC_PPD_RIF_IP_TERM_KEY_clear(&term_key);

    /* map BCM key to PPD key */
    rv = bcm_petra_tunnel_term_key_to_ppd(unit,info,&term_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* skip ethernet */
    #ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
      term_info.skip_ethernet = ((info->flags & BCM_TUNNEL_TERM_ETHERNET) != 0);
    }
    #endif


    if(term_key.flags & SOC_PPD_RIF_IP_TERM_FLAG_KEY_ONLY) {
        if (info->tunnel_id == 0) { /* tunnel indicate sem-result-index */
            LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                        (BSL_META_U(unit,
                                    "tunnel_id has to be set\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tunnel_id has to be set")));
        }
        local_inlif = info->tunnel_id;

        soc_sand_rv =
        soc_ppd_rif_ip_tunnel_term_add(soc_sand_dev_id, &term_key, local_inlif,&term_info,  NULL,&success);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(success);

        BCM_EXIT; /* done */
    }

    /* 1+1 protection */
    /* check if 1+1 protection is set, according to ingress_failover_id is valid */
    is_1_plus_1 = _BCM_PPD_IS_VALID_FAILOVER_ID(info->ingress_failover_id);
    if(SOC_IS_PETRAB(unit) && is_1_plus_1){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("1_plus_1 protection is not supported for petra-B")));
    }

    /* set RIF ID, -1 means keep don't update RIF and keep previous value */
    if (info->tunnel_if == BCM_IF_INVALID) {
        term_info.rif = SOC_PPD_RIF_NULL;
    }
    else{
        term_info.rif = info->tunnel_if; 
    }


    /* used in FP */
    term_info.service_type = info->if_class;

    /* module uniform or pipe
       uniform if keeping outer TTL and DSCP 
     */
    is_uniform = (info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) && (info->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL);
    term_info.processing_type= (is_uniform)?SOC_PPD_MPLS_TERM_MODEL_UNIFORM :SOC_PPD_MPLS_TERM_MODEL_PIPE;

    /* Read the state of the flags */
    update = (info->flags & BCM_TUNNEL_REPLACE) ? TRUE : FALSE;
    with_id = (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) ? TRUE : FALSE;

    /* check update/with-id allocations*/
    if (with_id && BCM_GPORT_IS_TUNNEL(info->tunnel_id)) {
        global_lif = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        alloc_flags |= SHR_RES_ALLOC_WITH_ID;

        if (SOC_IS_JERICHO(unit)) {
            rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_INGRESS, global_lif);
        } else if (SOC_IS_PETRAB(unit) || SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = bcm_dpp_am_l3_lif_is_allocated(unit, _BCM_DPP_AM_L3_LIF_IP_TERM, 0, global_lif);
        }
        lif_allocated = (rv == BCM_E_EXISTS) ? TRUE:FALSE;
    }

    /* this call will not update RIF attribute: VRF, cos-profile
       to update RIF attribute call bcm_petra_l3_intf_create/bcm_petra_l3_ingress_create*/
    /* cos profile is set for tunnel according to bcm_petra_qos_port_map_get*/

    /* allocate LIf for IP tunnel termination */
    /* cannot update without given id */
    if (update) {
        if (!with_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_TUNNEL_REPLACE can't be used without BCM_TUNNEL_WITH_ID as well")));
        }
        else if (!(alloc_flags & SHR_RES_ALLOC_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Updated flag is set without provide ID")));
        }
        else if (!lif_allocated) { /* update check LIF already allocated */
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Updated IP tunnel not exist")));
        }

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, global_lif, &local_inlif);
        BCMDNX_IF_ERR_EXIT(rv);

    } else if (with_id && lif_allocated) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case tunnel_id already exists, BCM_TUNNEL_REPLACE should be used")));
    } else { /* allocated LIF */
        if (SOC_IS_JERICHO(unit)) {
            bcm_dpp_am_local_inlif_info_t local_inlif_info;

            /* Allocate the global lif */
            rv =  bcm_dpp_am_global_lif_alloc(unit, alloc_flags | BCM_DPP_AM_FLAG_ALLOC_INGRESS, &global_lif);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Allocate an ingress lif */

            /* Get the application type by the lif type */
            local_inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ip_term;
            local_inlif_info.glif = global_lif;
            local_inlif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_COMMON;

            rv = _bcm_dpp_am_local_inlif_alloc(unit, 0, &local_inlif_info);
            BCMDNX_IF_ERR_EXIT(rv);

            local_inlif = local_inlif_info.base_lif_id;
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = bcm_dpp_am_l3_lif_alloc(unit, _BCM_DPP_AM_L3_LIF_IP_TERM, alloc_flags, &global_lif);
            BCMDNX_IF_ERR_EXIT(rv);
            local_inlif = global_lif;
        }
    }

    /* if update then don't overwrite this consts */
    if (!update) {
        /* for IP tunnel no learning, l2-GRE can override */
        term_info.learn_enable = 0;
        /* by default spoke so no filtering */
        term_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
    }

    /* 1+1 protection, set protection info in LIF */
    if (is_1_plus_1) {
        _BCM_PPD_FAILOVER_ID_PLUS_1_TO_ID(term_info.protection_pointer, info->ingress_failover_id);
        term_info.protection_pass_value = _BCM_PPD_TUNNEL_IS_WORKING_PORT(info)?0:1;
    }

    /* call ppd for config */
    soc_sand_rv =
	soc_ppd_rif_ip_tunnel_term_add(soc_sand_dev_id, &term_key, local_inlif, &term_info,  NULL,&success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success);

	/* tunnel id */
	/* tunnel ID always is LIF-id,
	 * also for 1+1: protection, MC-destination is seperate handled object. 
	 */
	BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id,global_lif);

    /* store in SW state LIF usage */
    rv = _bcm_dpp_in_lif_tunnel_term_add(unit, info, local_inlif);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Creates a new tunnel terminator for DIP-SIP key. */
int 
bcm_petra_tunnel_terminator_create(
    int unit, 
    bcm_tunnel_terminator_t *info) 
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_tunnel_terminator_add(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
  exit:
    BCMDNX_FUNC_RETURN;
}

/* Delete a tunnel terminator for DIP-SIP key. */
int 
bcm_petra_tunnel_terminator_delete(
    int unit, 
    bcm_tunnel_terminator_t *info)
{
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int rv = BCM_E_NONE;
    int local_inlif, global_inlif;
    SOC_PPD_RIF_IP_TERM_KEY
        term_key;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(info);

    soc_sand_dev_id = (unit);

    if (info->flags & BCM_L3_IP6) {
        LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                    (BSL_META_U(unit,
                                "IPv6: flags %x\n"), info->flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("IPv6 not supported")));
    }

    /* build key */
    SOC_PPD_RIF_IP_TERM_KEY_clear(&term_key);
    rv = bcm_petra_tunnel_term_key_to_ppd(unit,info,&term_key);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv =
        soc_ppd_rif_ip_tunnel_term_remove(soc_sand_dev_id, &term_key, (SOC_PPD_LIF_ID*)&local_inlif);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if (!(term_key.flags & SOC_PPD_RIF_IP_TERM_FLAG_KEY_ONLY)) {

        if (SOC_IS_JERICHO(unit)) {
            rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_inlif, &global_inlif);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_inlif, local_inlif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, FALSE);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            rv = bcm_dpp_am_l3_lif_dealloc(unit, local_inlif);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        
        rv = _bcm_dpp_in_lif_match_delete(unit, local_inlif);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Update a tunnel terminator for DIP-SIP key. */
int 
bcm_petra_tunnel_terminator_update(
    int unit, 
    bcm_tunnel_terminator_t *info)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_tunnel_terminator_update is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Get a tunnel terminator for DIP-SIP key. */
int 
bcm_petra_tunnel_terminator_get(
    int unit, 
    bcm_tunnel_terminator_t *info)
{
    SOC_PPD_RIF_IP_TERM_INFO term_info;
    SOC_PPD_RIF_INFO rif_info;
    uint8 found;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int global_lif, local_inlif;
    SOC_PPD_RIF_IP_TERM_KEY
        term_key;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(info);

    soc_sand_dev_id = (unit);
    SOC_PPD_RIF_IP_TERM_INFO_clear(&term_info);
    SOC_PPD_RIF_INFO_clear(&rif_info);

    if (info->flags & BCM_L3_IP6) {
        LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                    (BSL_META_U(unit,
                                "IPv6: flags %x\n"), info->flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("IPv6 not supported")));
    }

    /* map BCM key to PPD key */
    SOC_PPD_RIF_IP_TERM_KEY_clear(&term_key);

    rv = bcm_petra_tunnel_term_key_to_ppd(unit,info,&term_key);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_rif_ip_tunnel_term_get(soc_sand_dev_id, &term_key, (SOC_PPD_LIF_ID*) &local_inlif,
                                                 &term_info, &rif_info, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if (!found) {
        BCMDNX_IF_ERR_EXIT(BCM_E_NOT_FOUND);
    }
    
    /* set tunnel id handle*/
    rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_inlif, &global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id,global_lif);

    if (term_key.flags & SOC_PPD_RIF_IP_TERM_FLAG_KEY_ONLY) {
        BCM_EXIT;
    }

    /* uniform or pipe*/
    if(term_info.processing_type == SOC_PPD_MPLS_TERM_MODEL_UNIFORM) {
        info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP | BCM_TUNNEL_TERM_USE_OUTER_TTL;
    }

    /* skip ethernet */
    #ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        if (term_info.skip_ethernet) {
            info->flags |= BCM_TUNNEL_TERM_ETHERNET;
        }
    }
    #endif

    info->if_class = term_info.service_type;

    /* update from LIF sw state */
    rv = _bcm_dpp_in_lif_tunnel_term_get(unit, info, local_inlif);
    BCMDNX_IF_ERR_EXIT(rv);

    if (term_info.rif != SOC_PPD_RIF_NULL) {
        info->vrf = rif_info.vrf_id;
        info->tunnel_if = term_info.rif;
    }
    else{
        info->tunnel_if = BCM_IF_INVALID;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Set the valid payload VLANs for this WLAN tunnel. */
int 
bcm_petra_tunnel_terminator_vlan_set(
    int unit, 
    bcm_gport_t tunnel, 
    bcm_vlan_vector_t vlan_vec)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Get the valid payload VLANs for this WLAN tunnel. */
int 
bcm_petra_tunnel_terminator_vlan_get(
    int unit, 
    bcm_gport_t tunnel, 
    bcm_vlan_vector_t *vlan_vec)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Create a tunnel DSCP map instance. */
int 
bcm_petra_tunnel_dscp_map_create(
    int unit, 
    uint32 flags, 
    int *dscp_map_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Destroy a tunnel DSCP map instance. */
int 
bcm_petra_tunnel_dscp_map_destroy(
    int unit, 
    int dscp_map_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Get the { internal priority, color }-to-DSCP mapping in the specified
 * DSCP map.
 */
int 
bcm_petra_tunnel_dscp_map_get(
    int unit, 
    int dscp_map_id, 
    bcm_tunnel_dscp_map_t *dscp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Set the { internal priority, color }-to-DSCP mapping in the specified
 * DSCP map.
 */
int 
bcm_petra_tunnel_dscp_map_set(
    int unit, 
    int dscp_map_id, 
    bcm_tunnel_dscp_map_t *dscp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Set port egress { internal priority, color }-to-DSCP mapping. */
int 
bcm_petra_tunnel_dscp_map_port_set(
    int unit, 
    bcm_port_t port, 
    bcm_tunnel_dscp_map_t *dscp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Get port egress { internal priority, color }-to-DSCP mapping. */
int 
bcm_petra_tunnel_dscp_map_port_get(
    int unit, 
    bcm_port_t port, 
    bcm_tunnel_dscp_map_t *dscp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Set or get the global chip tunneling configuration. */
int 
bcm_petra_tunnel_config_get(
    int unit, 
    bcm_tunnel_config_t *tconfig)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Set or get the global chip tunneling configuration. */
int 
bcm_petra_tunnel_config_set(
    int unit, 
    bcm_tunnel_config_t *tconfig)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

