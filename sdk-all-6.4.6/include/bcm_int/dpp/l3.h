/*
 * $Id: l3.h,v 1.19 Broadcom SDK $
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
 * L3 Internal header
 */

#ifndef _BCM_INT_DPP_L3_H_
#define _BCM_INT_DPP_L3_H_

#include <sal/types.h>
#include <bcm/types.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <shared/swstate/sw_state.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vsi.h>

#define _BCM_PETRA_L3_NOF_ITFS SOC_DPP_CONFIG(unit)->l3.nof_rifs
#define _BCM_PETRA_L3_ITF_ID_BASE (0)

#define _BCM_PETRA_L3_ITF_RIF     (0)
#define _BCM_PETRA_L3_ITF_FEC     (1)
#define _BCM_PETRA_L3_ITF_ENCAP   (2)


#define _BCM_PETRA_L3_ITF_SHIFT        29

#define _BCM_PETRA_L3_ITF_ID_MASK         0x1FFFFFFF

/* Number of entries in the VridMyMacCam table */
#define _BCM_PETRA_L3_VRRP_CAM_MAX_NOF_ENTIRES(_unit)    (SOC_PPC_VRRP_CAM_MAX_NOF_ENTRIES(_unit))

/* Maximum number of VSIs that can be assigned to a vrrp entry. */
#define _BCM_PETRA_L3_VRRP_MAX_VSI(_unit)      SOC_PPC_VRRP_MAX_VSI(_unit)


/* Multiple mymac enabled modes. 
 *  Multiple mymac and VRRP can coexist on the same device, but sometimes the user might want to disable VRRP.
 *  For that reason, the multiple mymac enabled soc_dpp_config can be configured to 2 at device bringup.
 */

#define _BCM_PETRA_L3_MULTIPLE_MYMAC_ENABLED_EXCLUSIVE                  2

#define _BCM_L3_MULTIPLE_MYMAC_ENABLED(_unit)  \
        (SOC_DPP_CONFIG(_unit)->l3.multiple_mymac_enabled)

#define _BCM_L3_MULTIPLE_MYMAC_EXCLUSIVE(_unit)  \
        (SOC_DPP_CONFIG(_unit)->l3.multiple_mymac_enabled == _BCM_PETRA_L3_MULTIPLE_MYMAC_ENABLED_EXCLUSIVE)


/* Multiple mymac modes. 
 *  Mode None                   : No protocol distinction and no use of mac masks.
 *  Mode IPv4 distinct          : No use of mac masks, distinction only between IPv4 and other protocols.
 *  Mode Protocol Group Active  : Full use of protocol groups for protocol distinction, full use of mac masks.
 */
#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_NONE                          0
#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IPV4_DISTINCT                 1
#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_PROTOCOL_GROUP_ACTIVE         2

#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(unit) (SOC_DPP_CONFIG(unit)->l3.multiple_mymac_mode)

#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_PROTOCOL_GROUP_IS_ACTIVE(_unit)  \
        (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(_unit) >= _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_PROTOCOL_GROUP_ACTIVE)

#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IS_IPV4_DISTINCT(_unit)   \
        (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(_unit) == _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IPV4_DISTINCT)

#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IS_NONE(_unit)   \
        (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(_unit) == _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_NONE)


/* Given a vsi, returns true if it's the vrrp's "configure all VSIs" vsi. */
#define _BCM_PETRA_L3_VRRP_VLAN_IS_ALL_VSI(_vsi) (_vsi == 0)

/* Given the api's l3 protocol group flags, returns the soc ppc flags. */
#define BCM_PETRA_L3_VRRP_TRANSLATE_API_FLAGS_TO_INTERNAL_PROTOCOL_GROUP_FLAGS(_src, _target)         \
    (_target) = 0;                                                                              \
    DPP_TRANSLATE_FLAG(_src, BCM_L3_VRRP_IPV4, _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4);   \
    DPP_TRANSLATE_FLAG(_src, BCM_L3_VRRP_IPV6, _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV6);

/* Given the soc ppc l3 protocol group flags, returns the api flags. */
#define BCM_PETRA_L3_VRRP_TRANSLATE_PROTOCOL_GROUP_INTERNAL_FLAGS_TO_API(_src, _target)             \
    (_target) = 0;                                                                              \
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4, _target, BCM_L3_VRRP_IPV4);   \
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV6, _target, BCM_L3_VRRP_IPV6);   


void Lpm_Cache_Test(int32 masklen, int nof_routes, int flags, int num);


#define _BCM_PETRA_L3_ITF_SET(_l3_itf_, _type_, _id_) \
    ((_l3_itf_) = ((bcm_if_t)(((_type_) << _BCM_PETRA_L3_ITF_SHIFT)  | \
     ((_id_) & _BCM_PETRA_L3_ITF_ID_MASK))))


#define _BCM_PETRA_L3_ITF_VAL_GET(_l3_itf_) \
    ((_l3_itf_) & _BCM_PETRA_L3_ITF_ID_MASK)


#define _BCM_PETRA_L3_ITF_IS_FEC(_l3_itf_)    \
        (((_l3_itf_) >> _BCM_PETRA_L3_ITF_SHIFT) == _BCM_PETRA_L3_ITF_FEC)


#define _BCM_PETRA_L3_ITF_IS_RIF(_l3_itf_)    \
        (((_l3_itf_) >> _BCM_PETRA_L3_ITF_SHIFT) == _BCM_PETRA_L3_ITF_RIF)

#define _BCM_PETRA_L3_ITF_IS_ENCAP(_l3_itf_)    \
        (((_l3_itf_) >> _BCM_PETRA_L3_ITF_SHIFT) == _BCM_PETRA_L3_ITF_ENCAP)


#define _BCM_PETRA_L3_ITF_IS_VALID_FEC(_unit, _l3_itf_)    \
        ((_l3_itf_) != 0)

#define L3_ACCESS sw_state_access[unit].dpp.bcm.l3

/* ECMP Object */
typedef int bcm_ecmp_t;

/*
 *  This structure contains information about a given unit's l3 state.
 */
typedef struct bcm_dpp_l3_state_s {
    int         init;
    int         urpf_mode;
    PARSER_HINT_ARR int *bcm_tunnel_intf_to_eep;         /* The associated EEP per interface */
} bcm_dpp_l3_state_t;

typedef struct bcm_dpp_l3_info_s {
  int used_intf;
  int used_vrf;
  int used_host;
  int used_route;
  PARSER_HINT_PTR bcm_dpp_l3_state_t *dpp_l3_state;
  uint32 vrrp_protocol_set_types[SOC_DPP_DEFS_MAX(VRRP_NOF_PROTOCOL_GROUPS)];
  uint32 vrrp_protocol_set_counts[SOC_DPP_DEFS_MAX(VRRP_NOF_PROTOCOL_GROUPS)];
} bcm_dpp_l3_info_t;

/* Internal structure to handle VRFs routing */
typedef struct _dpp_l3_vrf_route_s {
    int is_ipv6;                        /* TRUE indicates ipv6 address */
    bcm_vrf_t vrf_id ;                  /* Virtual router instance. */
    bcm_ip_t vrf_subnet;                /* IP subnet address (IPv4). */
    bcm_ip6_t vrf_ip6_net;              /* IP subnet address (IPv6). */
    bcm_ip_t vrf_ip_mask;               /* IP subnet mask (IPv4). */
    bcm_ip6_t vrf_ip6_mask;             /* IP subnet mask (IPv6). */
    bcm_if_t vrf_intf;                  /* L3 interface associated with route, (FEC-id) */
    uint32 l3a_flags;                   /* L3 flags */
    bcm_if_t encap_id;                  /* Encapsulation index. */
} _dpp_l3_vrf_route_t;


/* Init vrf structure */


extern bcm_dpp_l3_info_t *_dpp_l3_info[BCM_MAX_NUM_UNITS];


void
  __dpp_l3_vrf_route_t_init(_dpp_l3_vrf_route_t *data);

/* Common functions to handle ipv4 vrf route (UC and MC) */
int
  _bcm_ppd_frwrd_ipv4_vrf_route_add(
    int unit,
    _dpp_l3_vrf_route_t *info
  );

int
  _bcm_ppd_frwrd_ipv4_vrf_route_get(
    int unit,
    _dpp_l3_vrf_route_t *info
  );

int
  _bcm_ppd_frwrd_ipv4_vrf_route_remove(
    int unit,
    _dpp_l3_vrf_route_t *info
  );

/* Common functions to handle ipv6 vrf route (UC and MC) */
int
  _bcm_ppd_frwrd_ipv6_vrf_route_add(
    int unit,
    _dpp_l3_vrf_route_t *info,
    SOC_PPD_FRWRD_DECISION_INFO *route_info
  );

int
  _bcm_ppd_frwrd_ipv6_vrf_route_get(
    int unit,
    _dpp_l3_vrf_route_t *info
  );

int
  _bcm_ppd_frwrd_ipv6_vrf_route_remove(
    int unit,
    _dpp_l3_vrf_route_t *info
  );

/* Conversions */
int
  _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(
    int unit,
    SOC_SAND_PP_IPV6_ADDRESS *soc_sand_ipv6_addr,
    bcm_ip6_t *ipv6_addr
  );

int
  _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(
    int unit,
    bcm_ip6_t ipv6_addr,
    SOC_SAND_PP_IPV6_ADDRESS *soc_sand_ipv6_addr
  );


/* create MPLS interface vlan x mac*/
int 
_bcm_petra_mpls_intf_create(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_t vlan
 );


int
  _bcm_l3_intf_to_fec(
    int unit,
    bcm_if_t intf,
    SOC_PPD_FEC_ID *fec_index
  );

void
  _bcm_l3_fec_to_intf(
    int unit,
    SOC_PPD_FEC_ID fec_index,
    bcm_if_t *intf
  );

int
  _bcm_l3_encap_id_to_eep(
    int unit,
    int encap_id,
    int *eep
  );

int bcm_dpp_l3_lock(int unit);
int bcm_dpp_l3_unlock(int unit);

int _bcm_tunnel_intf_ll_eep_set(int unit, bcm_if_t intf, int tunnel_eep, int ll_eep);

int _bcm_tunnel_intf_eep_set(int unit, bcm_if_t intf, int eep);

int _bcm_tunnel_intf_eep_get(int unit, bcm_if_t intf, int *eep);

int _bcm_tunnel_intf_is_tunnel(int unit, bcm_if_t intf, int *tunneled, int *eep, int *vsi, int *ll_eep);

/* get eedb index from router/tunnel intf */
int _bcm_l3_intf_eep_get(int unit, bcm_if_t intf, uint8 *tunneled, int *eep);

#ifdef BCM_88660_A0

int _bcm_l3_vrrp_mac_to_global_mac(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info, uint32 flags, int vrid);

int _bcm_l3_vrrp_setting_get(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info);

int _bcm_l3_vrrp_setting_exists(int unit, int profile_index, int vlan);

int _bcm_l3_vrrp_setting_delete(int unit, int cam_index, SOC_PPC_VRRP_CAM_INFO *cam_info, int vlan);

int _bcm_l3_vrrp_setting_add(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info, int vlan);

int
_bcm_l3_sand_mac_is_vrrp_mac(SOC_SAND_PP_MAC_ADDRESS *address, uint8 *is_vrrp);


#endif /*BCM_88660_A0*/

#endif /* _BCM_INT_DPP_L3_H_ */

