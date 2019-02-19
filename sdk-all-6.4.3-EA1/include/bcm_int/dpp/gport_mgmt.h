/*
 * $Id: gport_mgmt.h,v 1.71 Broadcom SDK $
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
 * File:        gport_mgmt.h
 * Purpose:     GPORT Management internal definitions to the BCM library.
 */

#ifndef  INCLUDE_GPORT_MGMT_H
#define  INCLUDE_GPORT_MGMT_H

#include <bcm/types.h>
#include <bcm/vlan.h>
#include <bcm/mpls.h>
#include <bcm/mim.h>
#include <bcm/trill.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/trill.h>
#include <bcm/tunnel.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/PPD/ppd_api_mymac.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>

#include <shared/hash_tbl.h>


/* Number of LIFs as gports */
#define _BCM_DPP_NOF_GPORTS(unit) ((SOC_DPP_CONFIG(unit))->l2.nof_lifs)
/* Number of egress PWEs as gports */

#define _BCM_DPP_NUM_OF_TRILL_VSI (32*1024)


typedef enum _bcm_lif_type_s {
    _bcmDppLifTypeAny=0,       /* Any VP */
    _bcmDppLifTypeMplsPort,    /* MPLS VP */
    _bcmDppLifTypeMim,       /* MIM VP */
    _bcmDppLifTypeTrill,     /* TRILL VP */
    _bcmDppLifTypeVlan,      /* VLAN VP */
    _bcmDppLifTypeMplsTunnel, /* MPLS Tunnel */
    _bcmDppLifTypeIpTunnel, /* IP Tunnel */
    _bcmDppLifTypeL2Gre, /* l2Gre */
    _bcmDppLifTypeVxlan, /* VxLan */
    _bcmDppLifTypeLinkLayer, /* Link layer */
    _bcmDppLifTypeOverlayLinkLayer /* Link layer */
} _bcm_lif_type_e;

typedef struct {
    int type;
    int phy_gport;/*port, trunk, encap-id */
    int encap_id;/*encap-id, according to type*/
} _BCM_GPORT_PHY_PORT_INFO;

/* per LIF what is match key */


/* for LIF-index, what is the match creteria */

typedef struct _bcm_dpp_inlif_match_port_info_s {
    _bcm_lif_type_e lif_type;
    uint32          flags;
    int             criteria;
    bcm_port_t      port;          /* Port */
    int             match1;
    int             match2;
    int             match_tunnel; /* For VLAN-Port used for PON application, for MPLS-Port used for label2 applications */
    int             match_ethertype;
                                   /* for MP lif --> gport, can be resoted from HW, from learning info*/
    bcm_gport_t     gport_id;      /* relevant only for p2p, given lif return gport*/
    int             key1;          /* only for p2p, part of the lif entry key */
    bcm_gport_t     peer_gport;    /* only for p2p, gport_id of the peer gport  */

    bcm_gport_t     learn_gport_id; /*  only for MP, the learning gport, needed when configure LIF and the learning info is FEC
                                    (maybe shared with gport_id) */ 
    _bcm_petra_tpid_profile_t tpid_profile_type;
                                   /* tpid profile is used for the case of PWE/I-SID. 
                                      Tpid profile state let us know the status of tpid profile assignment.
                                      By default tpid profile type should be none as PWE/I-SID doesn't use any assigned tpid profile.
                                      In order to change tpid profile type use APIs bcm_port_tpid_* */
    int             vsi;           /* only for MP service tell what the VSI LIF is attached to,
                                      can be restored/read from HW, set in SW to improve performance */
} _bcm_dpp_inlif_match_info_t;

typedef struct _bcm_dpp_inlif_bookkeeping_s {
    _bcm_dpp_inlif_match_info_t *match_key; /* Match Key */
} _bcm_dpp_inlif_bookkeeping_t;

extern _bcm_dpp_inlif_bookkeeping_t  _bcm_dpp_inlif_bk_info[];


/* OutLIF match information. Managed by WB SW DB */
typedef struct _bcm_dpp_outlif_match_port_info_s {
    _bcm_lif_type_e lif_type;
} _bcm_dpp_outlif_match_info_t;


/* 
 * Defines
 */

#define _BCM_GPORT_NOF_LIFS         DPP_WB_ENGINE_NOF_LIFS
#define _BCM_GPORT_NOF_VSIS         ((SOC_DPP_CONFIG(unit))->l2.nof_vsis)
#define _BCM_PPD_VSI_P2P_SERVICE    ((SOC_DPP_CONFIG(unit))->l2.nof_vsis - 2)

#define _BCM_GPORT_ENCAP_ID_LIF_INVALID  (-1)

#define _BCM_DPP_VT_TRANSLATION_PROFILE_INITIAL_VID(vt_profile) \
        (vt_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID || \
         (!(SOC_DPP_CONFIG(unit)->pp.pon_application_enable) && vt_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_USE_INITIAL_VID) || \
         vt_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID || \
         (!(SOC_DPP_CONFIG(unit)->pp.pon_application_enable) && vt_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL_USE_INITIAL_VID) || \
         ((!(SOC_DPP_CONFIG(unit)->pp.evb_enable) && !(SOC_DPP_CONFIG(unit)->pp.flexible_qinq_enable)) && vt_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_DOUBLE_TAG_PRIORITY_USE_INITIAL_VID))

#define _BCM_DPP_VT_TRANSLATION_PROFILE_FLEXIBLE_Q_IN_Q(vt_profile) \
        ((SOC_DPP_CONFIG(unit)->pp.flexible_qinq_enable ) && (vt_profile == SOC_PPC_PORT_DEFINED_VT_PROFILE_FLEXIBLE_Q_IN_Q ))
/* 
 * MACROS
 */

/*
 * MACROs to access LIF info
 */
#define INLIF_INFO(_unit_)   (&_bcm_dpp_inlif_bk_info[_unit_])



#define _BCM_PPD_IS_VALID_FAILOVER_ID(failover_id)  ((failover_id)!=0)

/*  set failover id in case of encoding*/
#define _BCM_PPD_FAILOVER_ID_PLUS_1_TO_ID(_failover_id, _ingress_failover_id)  (DPP_FAILOVER_ID_GET((_failover_id), (_ingress_failover_id)))

/* is the given gport working gport in protection, assuming gport has protection*/
#define _BCM_PPD_GPORT_IS_WORKING_PORT(gport) ((gport)->failover_port_id != BCM_GPORT_TYPE_NONE)

/* given FEC for protecting gport returns FEC for working gport*/
#define _BCM_PPD_GPORT_PROTECT_TO_WORK_FEC(protec_fec) (protec_fec-1)

/* given FEC for working gport returns FEC for protection gport*/
#define _BCM_PPD_GPORT_WORK_TO_PROTECT_FEC(work_fec) (work_fec+1)

/* given FEC returns working FEC */
#define _BCM_PPD_GPORT_FEC_TO_WORK_FEC(fec) ((((fec)%2) == 1)?(fec-1):(fec))


/*
 * MACROs for PON
 */
/* given gport ID is it working port*/
#define _BCM_PPD_GPORT_IS_WORKING_PORT_ID(gport) (((gport)%2) == 0)

/* number of PON ports */
#define _BCM_PPD_NOF_PON_PHY_PORT (8)

/* number of PON PP ports */
#define _BCM_PPD_NOF_PON_PP_PORT  (128)

#define _BCM_PPD_IS_PON_PP_PORT(pon_pp_port) (((pon_pp_port) >= 0) && ((pon_pp_port) < _BCM_PPD_NOF_PON_PP_PORT))

/* PON channel default profile */
#define _BCM_PPD_PON_CHANNEL_DEFAULT_PROFILE (0)

/* default number of PON channel profiles */
#define _BCM_PPD_DEFAULT_NOF_PON_CHANNEL_PROFILE (16)

/* number of PON channel profiles when PON port is channelized */
#define _BCM_PPD_NOF_PON_CHANNEL_PROFILE(nof_channels) (16/(1<<((nof_channels)-1)))

/* Default offset of PON channel profiles */
#define _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_DEFAULT_OFFSET (3)

/*  offset of PON channel profiles */
#define _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_OFFSET(nof_channels) (3+((nof_channels)-1))

/* given PON PP port returns physical port */
#define _BCM_PPD_GPORT_PON_TO_PHY_PORT(pon_pp_port) ((pon_pp_port)&0x7)

/* given PON PP port returns local port */
#define _BCM_PPD_GPORT_PON_TO_LOCAL_PORT(pon_pp_port, offset) ((pon_pp_port)&((1<<(offset))-1))

/* given PON PP port returns PON channel profile */
#define _BCM_PPD_GPORT_PON_PP_PORT_TO_CHANNEL_PROFILE(pon_pp_port, offset) (((pon_pp_port)>>(offset))&0xf)

/* given PON port and PON channel profile returns PON PP port */
#define _BCM_PPD_GPORT_PON_CHANNEL_PROFILE_TO_PON_PP_PORT(pon_port, pon_channel_profile, offset) (((pon_channel_profile)<<(offset))|(pon_port)) 


/* indicate if the gport is an overlay gport: vxlan, l2gre, trill, vpls (identify by mpls gport) */
#define _BCM_GPORT_IS_OVERLAY(_gport) \
        _SHR_GPORT_IS_VXLAN_PORT(_gport) \
        || _SHR_GPORT_IS_L2GRE_PORT(_gport) \
        || _SHR_GPORT_IS_TRILL_PORT(_gport) \
        || _SHR_GPORT_IS_MPLS_PORT(_gport) \
        || _SHR_GPORT_IS_FORWARD_PORT(_gport)


/*
 * Encap ID that is used for bcm88xxx_system_resource_management=1. 
 * encap_id paramaeter is used in bcm_l2_gport_forward_info_get 
 * and being passed in cross connect and l2 addr APIs. See example of use in cint_system_vswitch.c 
 */

/* Invalid indication for encap_id 0 or -1 */
#define _BCM_DPP_ENCAP_INVALID_VAL (0XFFFFFFFF)
#define _BCM_DPP_PORT_ENCAP_INVALID(_encap_id) ((_encap_id) == _BCM_DPP_ENCAP_INVALID_VAL || _encap_id == 0)

/* Encap encoding */
/* 
   ENCAP ID:
 
   31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01
   ____________________________________________________________________________________________
  | Type|Usage|  <---------------------------------------       Value -------------------->    | 
  |_____|_____|________________________________________________________________________________|                  
  | <---------------------------------------------------------     Encap   --------------->    |
 
*/ 

/* encap usage. needed for remote encap for p2p
   as in this case need to set in HW what is the
   the peer gport type: ac2pwe, ac2ac, ac2isid
   and by pointer itself it cannot be identified
   so need to encode this information into encap-id
 */
#define _BCM_DPP_GPORT_ENCAP_USAGE_GENERAL  0
#define _BCM_DPP_GPORT_ENCAP_USAGE_PWE      1
#define _BCM_DPP_GPORT_ENCAP_USAGE_ISID     2
#define _BCM_DPP_GPORT_ENCAP_USAGE_AC       3

#define _BCM_DPP_GPORT_ENCAP_TYPE_OUTLIF    0
#define _BCM_DPP_GPORT_ENCAP_TYPE_EEI       1


#define _BCM_DPP_GPORT_ENCAP_TYPE_SHIFT      30

#define _BCM_DPP_GPORT_ENCAP_USAGE_SHIFT      28
#define _BCM_DPP_GPORT_ENCAP_USAGE_MASK       0x3

#define _BCM_DPP_GPORT_ENCAP_VAL_SHIFT        0
#define _BCM_DPP_GPORT_ENCAP_VAL_MASK         0xFFFFFFF

/* given encap_id return if this outlif encap according to type */
#define _BCM_DPP_GPORT_ENCAP_IS_OUTLIF(_encap_id)             \
    (((_encap_id) >> _BCM_DPP_GPORT_ENCAP_TYPE_SHIFT) == _BCM_DPP_GPORT_ENCAP_TYPE_OUTLIF)

/* given encap_id return if this EEI encap according to type */

/* given encap_id return encap value */
#define _BCM_DPP_GPORT_ENCAP_VAL_GET(_encap_id)     \
    (((_encap_id) >> _BCM_DPP_GPORT_ENCAP_VAL_SHIFT) & _BCM_DPP_GPORT_ENCAP_VAL_MASK)

/* given encap_id return encap usage */
#define _BCM_DPP_GPORT_ENCAP_USAGE_GET(_encap_id)     \
    (((_encap_id) >> _BCM_DPP_GPORT_ENCAP_USAGE_SHIFT) & _BCM_DPP_GPORT_ENCAP_USAGE_MASK)

/* set encap_id accoring to type, usage and value */
#define _BCM_DPP_GPORT_ENCAP_VAL_SET(_encap_id, _type,_usage, _val)     \
    ((_encap_id) = (_type << _BCM_DPP_GPORT_ENCAP_TYPE_SHIFT)   | \
      (((_usage) &_BCM_DPP_GPORT_ENCAP_USAGE_MASK) << _BCM_DPP_GPORT_ENCAP_USAGE_SHIFT)   | \
      (((_val) & _BCM_DPP_GPORT_ENCAP_VAL_MASK) << _BCM_DPP_GPORT_ENCAP_VAL_SHIFT))

/*
 * Function:
 *      _bcm_dpp_gport_mgmt_init
 * Purpose:
 *      init Gport module
 * Parameters:
 * Note:
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_dpp_gport_mgmt_init(int                     unit);

extern int
_bcm_dpp_gport_mgmt_sw_state_cleanup(int                     unit);



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
_bcm_dpp_gport_to_port_encap(int unit, bcm_gport_t gport, int *dest_port, int *encap_id);
/*
 * Function:
 *       _bcm_dpp_gport_encap_to_fwd_decision
 * Description:
 *       Parse gport+encap to soc-ppd-forwarding decision
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  encap_info -       [IN] encap_id EEI or outlif.
 *                          if invalid then 
 *  fwd_decsion -    [OUT] PPD forwarding decision
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_port_encap_to_fwd_decision(int unit, bcm_gport_t gport, int encap_id, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion);


/*
 * Function:
 *       _bcm_dpp_port_encap_from_fwd_decision
 * Description:
 *       Parse soc-ppd-forwarding decision to gport+encap
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [OUT] general port
 *  encap_id -       [OUT] encap_id EEI or outlif.
 *                          if invalid then 
 *  fwd_decsion -    [IN] PPD forwarding decision
 *  encap_usage -    [IN] AC,PWE,MIM, General
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_port_encap_from_fwd_decision(int unit, bcm_gport_t *gport, int *encap_id, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion, uint8 encap_usage);

/*
 * Function:
 *       _bcm_dpp_gport_to_fwd_decision
 * Description:
 *       Parse gport
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  encap_id -       [IN] encap_id considered only if gport is logical port
 *  fwd_decsion -    [OUT] PPD forwarding decision
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_gport_to_fwd_decision(int unit, bcm_gport_t gport, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion);
/*
 * Function:
 *       _bcm_dpp_gport_from_fwd_decision
 * Description:
 *       convert forwardubg decision (destination + editing information) to gport
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [OUT] general port
 *  encap_id -       [OUT] encap_id considered only if gport is logical port
 *  fwd_decsion -    [IN] PPD forwarding decision
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_gport_from_fwd_decision(int unit, bcm_gport_t *gport, SOC_PPD_FRWRD_DECISION_INFO  *fwd_decsion);
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
_bcm_dpp_gport_to_sand_pp_dest(int unit, bcm_gport_t gport, SOC_SAND_PP_DESTINATION_ID  *dest_id);

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
_bcm_dpp_gport_compare(int unit, bcm_gport_t gport1, bcm_gport_t gport2, uint8 * is_equal);

/*
 * Function:
 *       _bcm_dpp_mod_port_to_sys_port
 * Description:
 *       map module-ID + mod-port to system port
 * Parameters:
 *       unit -    [IN] DPP device unit number (driver internal).
 *  modid -   [IN] module id
 *  mode_port -     [IN] module user port
 *  sys_port -     [OUT] unique System port, or SOC_TMC_SYS_PHYS_PORT_INVALID if
 *                                      given module or port on module not exist
 * Returns:
 *       BCM_E_XXX
 */

int 
 _bcm_dpp_mod_port_to_sys_port(int unit, bcm_module_t modid, bcm_port_t mode_port, unsigned int *sys_port);

/*
 * Function:
 *	_bcm_dpp_mod_port_from_sys_port
 * Description:
 *    map module-ID + mod-port to system port
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *  modid -   [IN] module id
 *  mode_port -     [IN] module user port
 *  sys_port -     [OUT] unique System port, or SOC_TMC_SYS_PHYS_PORT_INVALID if
 *                       given module or port on module not exist
 * Returns:
 *    BCM_E_XXX
 */
int 
 _bcm_dpp_mod_port_from_sys_port(int unit, bcm_module_t *modid, bcm_port_t *mode_port, uint32 sys_port);

typedef struct _bcm_dpp_gport_info_s {
    bcm_pbmp_t pbmp_local_ports;
    bcm_port_t local_port; /* one of the ports in pbmp_local_ports*/
    uint32     sys_port;
    uint32     flags;
    int        lane;
} _bcm_dpp_gport_info_t;

#define _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT 0x1
#define _BCM_DPP_GPORT_INFO_F_IS_LAG        0x2
#define _BCM_DPP_GPORT_INFO_F_IS_PER_LANE   0x4

#define _BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info) (gport_info.flags & _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT ? 1: 0)
#define _BCM_DPP_GPORT_INFO_IS_LAG(gport_info) (gport_info.flags & _BCM_DPP_GPORT_INFO_F_IS_LAG ? 1: 0)
#define _BCM_DPP_GPORT_INFO_IS_PER_LANE(gport_info) (gport_info.flags & _BCM_DPP_GPORT_INFO_F_IS_PER_LANE ? 1: 0)

/*
 * Function:
 *    _bcm_dpp_gport_to_phy_port
 * Description:
 *    map gport to PPD local and system port
 * Parameters:
 *    unit  -       [IN] DPP device unit number (driver internal).
 *    gport -       [IN] general port
 *    operations -  [IN] see _BCM_DPP_GPORT_TO_PHY_OP_...
 *    gport_info -  [OUT] Retrive information
 * Returns:
 *    BCM_E_XXX
 */

#define _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT    0x1
#define _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY  0x2
int 
_bcm_dpp_gport_to_phy_port(int unit, bcm_gport_t gport, uint32 operations, _bcm_dpp_gport_info_t* gport_info);

/* given gport, return if it's local in this unit */
int 
_bcm_dpp_gport_is_local(int unit, bcm_gport_t port,  int *is_local);

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
_bcm_dpp_gport_from_sand_pp_dest(int unit, bcm_gport_t *gport, SOC_SAND_PP_DESTINATION_ID  *dest_id);

int _bcm_dpp_vlan_port_create(int unit, bcm_vlan_port_t *vlan_port);

/* Internal functions for vlan_port_create */
int l2_interface_calculate_act_fec(void *gport, int *act_fec, int gport_type);

int l2_interface_calculate_port_id(int unit, int act_fec, SOC_PPD_LIF_ID lif_id, void *gport, int gport_type);

int l2_interface_get_lif_from_port_id(void *gport, uint8 is_protected, uint8 is_mc_ingress, SOC_PPD_LIF_ID *lif_id, int gport_type);

int l2_interface_ingress_egress_only_validation(void *gport, uint8 validate, uint8 *is_ingress, uint8 *is_egress, int gport_type);

int l2_interface_protection_validation(int unit, void *gport, uint8 validate, int *is_protected, uint8 *is_ingress_protected, 
                                           uint8 *is_mc_ingress, uint8 *is_1_plus_1, int gport_type);

int vlan_port_create_forward_group_validation(bcm_vlan_port_t *vlan_port, uint8 validate, uint8 *is_forward_group);


/*
* returns if this port belongs to the given VPN 
* if port is not local and MP vpn, 
* then is_member cannot be resolved. 
*/
int _bcm_dpp_vpn_is_gport_member(
    int	                 unit,
    bcm_vpn_t	         vpn,
    bcm_gport_t          gport_id,
    int                  *is_member,
    int                  *is_local
 );

/* LIF Usage */
int _bcm_dpp_lif_usage_get(int unit, int in_lif, int out_lif, _bcm_lif_type_e *in_lif_usage, _bcm_lif_type_e *out_lif_usage);
int _bcm_dpp_in_lif_usage_set(int unit, int in_lif, _bcm_lif_type_e usage);
int _bcm_dpp_out_lif_usage_set(int unit, int out_lif, _bcm_lif_type_e usage);

/* General match utils */
int _bcm_dpp_lif_match_valid(int unit, int *in_lif, int *out_lif);
int _bcm_dpp_in_lif_match_gport_get(int unit, int in_lif);
int _bcm_dpp_in_lif_match_peer_gport_set(int unit, int in_lif, int peer_gport);
int _bcm_dpp_in_lif_match_peer_gport_get(int unit, int in_lif);
int _bcm_dpp_in_lif_match_delete(int unit, int in_lif);
int _bcm_dpp_out_lif_match_delete(int unit, int out_lif);

/* MPLS tunnel */
int _bcm_dpp_in_lif_mpls_term_match_add(int unit, bcm_mpls_tunnel_switch_t *mpls_term, int lif);
int _bcm_dpp_in_lif_mpls_term_match_get(int unit, bcm_mpls_tunnel_switch_t *mpls_term, int lif);
int _bcm_dpp_out_lif_mpls_tunnel_match_add(int unit, int lif);

/* PWE */
int _bcm_dpp_in_lif_mpls_match_get(int unit, bcm_mpls_port_t *mpls_port, int lif);
int _bcm_dpp_in_lif_mpls_match_add(int unit, bcm_mpls_port_t *mpls_port, int lif);
int _bcm_dpp_out_lif_mpls_match_add(int unit, bcm_mpls_port_t *mpls_port, int lif);

/* VLAN  (InLIF only)*/
int _bcm_dpp_in_lif_ac_match_get(int unit, bcm_vlan_port_t *vlan_port, int in_lif);
int _bcm_dpp_in_lif_ac_match_add(int unit, bcm_vlan_port_t *vlan_port, int in_lif);

/* Learn (InLIF only) */
int _bcm_dpp_in_lif_match_learn_gport_set(int unit, int in_lif, int learn_gport);
int _bcm_dpp_in_lif_match_learn_gport_get(int unit, int in_lif);

/* IP tunnel */
int _bcm_dpp_in_lif_tunnel_term_add(int unit, bcm_tunnel_terminator_t *tunnel_term, int lif);
int _bcm_dpp_in_lif_tunnel_term_get(int unit, bcm_tunnel_terminator_t *tunnel_term, int lif);
int _bcm_dpp_out_lif_tunnel_initiator_match_add(int unit, bcm_tunnel_initiator_t *tunnel_init, int lif);

/* MIM */
int _bcm_dpp_in_lif_mim_match_add(int unit, bcm_mim_port_t *mim_port, int lif);
int _bcm_dpp_in_lif_mim_match_get(int unit, bcm_mim_port_t *mim_port, int lif);
int _bcm_dpp_out_lif_mim_match_add(int unit, bcm_mim_port_t *mim_port, int lif);

/* Link Layer */
int _bcm_dpp_out_lif_ll_match_add(int unit, int lif);

/* Overlay Link Layer */
int _bcm_dpp_out_lif_overlay_ll_match_add(int unit, int lif);

/*
 * delete gport including
 * - allocated resources 
 * - SW state 
 * - HW state 
 */
int _bcm_dpp_gport_delete(
    int                  unit,
    int                  vpn,
    bcm_gport_t          port_id
 );

/*
 * given vpn and iter return next gport-value in vpn 
 * caller still need to call  
 */
int _bcm_dpp_vpn_get_next_gport(
    int                  unit,
    bcm_vpn_t            vpn, /* if -1 then get next gport belongs to any vsi */
    int                  *iter,
    bcm_gport_t          *port_val,
    int                  *cur_vsi /* the vsi of the return LIF*/
 );

int _bcm_dpp_gport_parse(int unit, bcm_gport_t port, int *type, int *val, _BCM_GPORT_PHY_PORT_INFO **phy_port_p);


/*
 * return if the given gport, 
 * is FEC with remote LIF 
 */
int _bcm_dpp_gport_to_lif(
    int                  unit,
    bcm_gport_t          gport_id,
    int                  *lif_id,
    int                  *out_lif_id,
    int                  *fec_id,
    int                  *is_local
 );


int 
_bcm_dpp_l2_interface_determine_ingress_egress(
   bcm_gport_t gport_id, 
   uint8 *is_ingress, 
   uint8 *is_egress);

int _bcm_dpp_gport_has_remote_lif(
    int                  unit,
    bcm_gport_t          gport_id,
    uint8                *remote_lif
 );

int _bcm_dpp_lif_to_gport(
    int                  unit,
    int                  lif_id, /* InLIF ID */
    int                  sw_state, 
    bcm_gport_t          *gport_id
 );

/* given lif return VSI this lif is assoicated to, return -1 if there is no VSI */
int _bcm_dpp_lif_to_vsi(
    int          unit,
    int          lif,
    int          *vsi,
    uint8        sw_state,
    bcm_gport_t  *gport
   );

/* Given a gport, return its fec index
 * fec_id is valid only if found
 * Validated for overlay gport
 */
int _bcm_dpp_gport_to_fec(
   int           unit, 
   bcm_gport_t   gport,
   uint32*       fec_id,
   uint8*        found
  );

int
_bcm_dpp_mpls_port_add(
       int       unit,
       bcm_vpn_t       vpn,
       bcm_mpls_port_t *       mpls_port);

/*
 * Function:
 *    _bcm_dpp_gport_resolve
 * Description:
 *    map gport to destination (FEC/phy-port) + encap-id
 * Parameters:
 *  unit -    [IN] DPP device unit number (driver internal).
 *  gport -   [IN] general port
 *  dest_id -     [OUT] destionation (FEC/physical port)
 *  encap_id -     [OUT] encap_id considered only if gport is logical port
 *  gport_type -     [OUT] gport type, FEC (protected), LIF , simple
 * Returns:
 *    BCM_E_XXX
 */
#define _BCM_DPP_GPORT_TYPE_FEC 1
#define _BCM_DPP_GPORT_TYPE_ENCAP 2
#define _BCM_DPP_GPORT_TYPE_MC 3
#define _BCM_DPP_GPORT_TYPE_SIMPLE 4




/* invalid gport type or not initialized */
#define _BCM_DPP_GPORT_IN_TYPE_INVALID    (0)
/* gport-id includes FEC only, FEC includes: port + EEP */
#define _BCM_DPP_GPORT_IN_TYPE_FEC    (_BCM_DPP_GPORT_TYPE_FEC)
/* gport is EEP: phy = physical port + encap-id = AC */
#define _BCM_DPP_GPORT_IN_TYPE_AC  (_BCM_DPP_GPORT_TYPE_ENCAP)
/* gport is only physical port, encap_id is not relevant */
#define _BCM_DPP_GPORT_IN_TYPE_PHY    (_BCM_DPP_GPORT_TYPE_SIMPLE)

/* gport is VC: phy = FEC + encap-id = VC */
#define _BCM_DPP_GPORT_IN_TYPE_FEC_VC  (_BCM_DPP_GPORT_TYPE_SIMPLE+1)
/* gport is VC: phy = physical port + encap-id = VC */

/* gport is EEP for mpls: phy = physical port + encap-id = EEP */
#define _BCM_DPP_GPORT_IN_TYPE_EEP  (_BCM_DPP_GPORT_TYPE_SIMPLE+2)

/* gport is PWE: phy = FEC + encap-id = EEP */
#define _BCM_DPP_GPORT_IN_TYPE_FEC_EEP  (_BCM_DPP_GPORT_TYPE_SIMPLE+3)
/* gport is VC: phy = physical port + encap-id = VC */

/* gport is FEC: phy = B-SA LSB + encap-id = {B-SA MSB,B-VID} */
#define _BCM_DPP_GPORT_IN_TYPE_MIM_MP  (_BCM_DPP_GPORT_TYPE_SIMPLE+4)

/* gport is MC trill */
#define _BCM_DPP_GPORT_IN_TYPE_TRILL_MC  (_BCM_DPP_GPORT_TYPE_SIMPLE+5)

/* gport is FEC: phy = LIF + encap-id = B-VID */
#define _BCM_DPP_GPORT_IN_TYPE_MIM_P2P  (_BCM_DPP_GPORT_TYPE_SIMPLE+6)

/* gport is out-lif/EEP for ip-tunnel: phy = physical port + encap-id = EEP */
#define _BCM_DPP_GPORT_IN_TYPE_IP_TUNNEL  (_BCM_DPP_GPORT_TYPE_SIMPLE+7)

/* l2gre gport: gport is in-lif for ip-tunnel: phy = physical port + encap-id = out-LIF */
#define _BCM_DPP_GPORT_IN_TYPE_L2_GRE  (_BCM_DPP_GPORT_TYPE_SIMPLE+8)

/* l2gre gport: gport is in-lif for ip-tunnel: phy = FEC + encap-id = out-LIF */
#define _BCM_DPP_GPORT_IN_TYPE_L2_GRE_EG_FEC  (_BCM_DPP_GPORT_TYPE_SIMPLE+9)

/* vxlan gport: gport is in-lif for ip-tunnel: phy = physical port + encap-id = out-LIF */
#define _BCM_DPP_GPORT_IN_TYPE_VXLAN  (_BCM_DPP_GPORT_TYPE_SIMPLE+10)

/* vxlan gport: gport is in-lif for ip-tunnel: phy = FEC + encap-id = out-LIF */
#define _BCM_DPP_GPORT_IN_TYPE_VXLAN_EG_FEC  (_BCM_DPP_GPORT_TYPE_SIMPLE+11)

/* ring gport: phy = FEC + encap-id */
#define _BCM_DPP_GPORT_IN_TYPE_RING  (_BCM_DPP_GPORT_TYPE_SIMPLE+12)

/* invalid options for PB */
/* AC + FEC */
/* VC + port */

/* MPLS port ENCAP ID parse */
#define _BCM_DPP_ENCAP_ID_VC_SHIFT                      0
#define _BCM_DPP_ENCAP_ID_VC_MASK                       0xfffff
#define _BCM_DPP_ENCAP_ID_PUSH_PROFILE_SHIFT            20
#define _BCM_DPP_ENCAP_ID_PUSH_PROFILE_MASK             0x7


#define _BCM_DPP_ENCAP_ID_VC_SET(encap_id, label, push_profile)                            \
  ((encap_id) = (((label) & _BCM_DPP_ENCAP_ID_VC_MASK)  << _BCM_DPP_ENCAP_ID_VC_SHIFT)  | \
  (((push_profile) & _BCM_DPP_ENCAP_ID_PUSH_PROFILE_MASK)     << _BCM_DPP_ENCAP_ID_PUSH_PROFILE_SHIFT))

#define _BCM_DPP_ENCAP_ID_VC_GET(encap_id)  \
        (((encap_id) >> _BCM_DPP_ENCAP_ID_VC_SHIFT) & _BCM_DPP_ENCAP_ID_VC_MASK)

#define _BCM_DPP_ENCAP_ID_PUSH_PROFILE_GET(encap_id)  \
        (((encap_id) >> _BCM_DPP_ENCAP_ID_PUSH_PROFILE_SHIFT) & _BCM_DPP_ENCAP_ID_PUSH_PROFILE_MASK)


extern shr_htb_hash_table_t gport_vlan_port_db[BCM_MAX_NUM_UNITS];
extern shr_htb_hash_table_t trill_info_db[BCM_MAX_NUM_UNITS];
extern shr_htb_hash_table_t mc_trill_db[BCM_MAX_NUM_UNITS];
extern shr_htb_hash_table_t mc_trill_route_info_db[BCM_MAX_NUM_UNITS];
extern shr_htb_hash_table_t mc_trill_root_src_db[BCM_MAX_NUM_UNITS];
extern shr_htb_hash_table_t trill_vpn_db[BCM_MAX_NUM_UNITS];

/* if set then don't check if gport already exist, shoulb be in construction */
#define _BCM_DPP_GPORT_RESOLVE_FLAG_NO_CHECK (0x1)
int 
_bcm_dpp_gport_resolve(int unit, bcm_gport_t gport, int flags, SOC_SAND_PP_DESTINATION_ID *dest_id, int *encap_id,int *gport_type);

/*
 * Function:
 *    _bcm_dpp_gport_to_fwd_decision
 * Description:
 *    convert gport to forwardubg decision (destination + editting information)
 * Parameters:
 *    unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  encap_id -       [IN] encap_id considered only if gport is logical port
 *  fwd_decsion -    [OUT] PPD forwarding decision
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_to_tm_dest_info(int unit, bcm_gport_t gport, SOC_TMC_DEST_INFO  *soc_petra_dest_info);

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
_bcm_dpp_gport_from_tm_dest_info(int unit, bcm_gport_t *gport, SOC_TMC_DEST_INFO  *soc_petra_dest_info);

int
_bcm_dpp_sw_db_hash_vlan_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove);

int 
_bcm_dpp_sw_db_hash_vlan_insert(int unit, shr_htb_key_t key, shr_htb_data_t data);

int
_bcm_dpp_sw_db_hash_trill_root_src_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove);

int 
_bcm_dpp_sw_db_hash_trill_root_src_insert(int unit, shr_htb_key_t key, shr_htb_data_t data);
int
_bcm_dpp_sw_db_hash_trill_route_info_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove);

int 
_bcm_dpp_sw_db_hash_trill_route_info_insert(int unit, shr_htb_key_t key, shr_htb_data_t data);


int
_bcm_dpp_sw_db_hash_mc_trill_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove);

int 
_bcm_dpp_sw_db_hash_mc_trill_insert(int unit, shr_htb_key_t key, shr_htb_data_t data);
int 
_bcm_dpp_sw_db_hash_trill_route_info_destroy(int unit);

int 
_bcm_dpp_sw_db_hash_mc_trill_destroy(int unit);

int
_bcm_dpp_sw_db_hash_trill_info_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove);

int 
_bcm_dpp_sw_db_hash_trill_info_insert(int unit, shr_htb_key_t key, shr_htb_data_t data);

int 
_bcm_dpp_sw_db_hash_trill_vpn_insert(int unit, shr_htb_key_t key, shr_htb_data_t data);
int
_bcm_dpp_sw_db_hash_trill_vpn_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove);


#define BCM_DPP_GPORT_TYPE_EGRESS_IF (0x50)
int 
_bcm_dpp_l2_gport_fill_fec(int unit, void *gport, int gport_type, SOC_PPD_FEC_ID fec_id, SOC_PPD_FRWRD_FEC_ENTRY_INFO *work_fec);

int _bcm_dpp_gport_fill_ac_key(
    int                  unit,
    void                 *gport, 
    int                  gport_type, 
    SOC_PPD_L2_LIF_AC_KEY  *in_ac_key
 );

/*
 * given gport return out_ac_key where this gport is defined
 */
int _bcm_dpp_gport_fill_out_ac_key(
    int                  unit,
    bcm_vlan_port_t      *vlan_port,     
    uint8                *is_cep,
    SOC_PPD_EG_AC_VBP_KEY *vbp_key,
    SOC_PPD_EG_AC_CEP_PORT_KEY *cep_key
 );

int 
_bcm_dpp_l2_fec_fill_gport(int unit, SOC_PPD_FEC_ID fec_id, void *gport, int gport_type);

void
_bcm_dpp_htb_cast_key_f(shr_htb_key_t key, uint8 **key_bytes, uint32 *key_size);

int _bcm_dpp_gport_is_protected(int unit, bcm_gport_t gport, int  *is_protected);
int _bcm_dpp_gport_get_used_type(int unit,
                                 bcm_gport_t gport,
                                 _bcm_lif_type_e *used_type);

/* 1+1 protection*/
int 
_bcm_dpp_gport_plus_1_mc_to_id(int mc_id, int is_primary, int *gport_val);
int 
_bcm_dpp_gport_plus_1_id_to_mc(int gport_val, int *mc_id , int *is_primary);



/* store and get vsi */
int _bcm_dpp_in_lif_match_vsi_set(int unit, int in_lif, int vsi);
int _bcm_dpp_in_lif_match_vsi_get(int unit, int in_lif, int *vsi);

/*
 * Function:
 *      _bcm_dpp_in_lif_match_tpid_type_set
 * Purpose:
 *      Obtain tpid type information of an MPLS port/I-SID InLif
 * Parameters:
 *      unit    - (IN) Device Number
 *      tpid type - (OUT) tpid type information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_in_lif_match_tpid_type_set(int unit, _bcm_petra_tpid_profile_t tpid_type, int in_lif);

/*
 * Function:
 *      _bcm_dpp_lif_match_tpid_type_get
 * Purpose:
 *      Obtain tpid type information of an MPLS port/I-SID Lif
 * Parameters:
 *      unit    - (IN) Device Number
 *      tpid type - (OUT) tpid type information
 *      in_lif  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_in_lif_match_tpid_type_get(int unit, _bcm_petra_tpid_profile_t *tpid_type, int in_lif);

/* update FEC entry type, to given type */
int _bcm_dpp_gport_fix_fec_type(
    int unit,
    int fec_id,
    int  new_fec_type);

int _bcm_dpp_sw_db_hash_vlan_iterate(int unit, shr_htb_cb_t restore_cb);
int _bcm_dpp_sw_db_hash_trill_info_iterate(int unit, shr_htb_cb_t restore_cb);
int _bcm_dpp_sw_db_hash_mc_trill_iterate(int unit, shr_htb_cb_t restore_cb);
int _bcm_dpp_sw_db_hash_trill_route_info_iterate(int unit, shr_htb_cb_t restore_cb);
int _bcm_dpp_sw_db_hash_trill_src_iterate(int unit, shr_htb_cb_t restore_cb);
int _bcm_dpp_sw_db_hash_trill_vpn_iterate(int unit, shr_htb_cb_t restore_cb);

/*
 * Function:
 *      _bcm_dpp_port_is_pon_port
 * Purpose:
 *      Check if the port is PON port
 * Parameters:
 *      unit        - (IN)  Device Number
 *      port        - (IN)  Device PP port Number
 *      is_pon_port - (OUT) TRUE/FALSE
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_port_is_pon_port(int unit, bcm_port_t port, int *is_pon_port);

/*
 * Function:
 *      _bcm_dpp_pon_lif_is_3_tags_data
 * Purpose:
 *      Check if the given lif index is 3 TAGs manipulation.
 * Parameters:
 *      unit           - (IN)  Device Number
 *      out_lif_id     - (IN)  out lif index
 *      is_3_tags_data - (OUT) TRUE/FALSE
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_lif_is_3_tags_data(int unit, int out_lif_id, int *is_3_tags_data);

/*
 * Function:
 *      _bcm_dpp_pon_encap_data_enty_add
 * Purpose:
 *       Add an EEDB entry with PON Tunnel info
 * Parameters:
 *      unit        - (IN)  Device Number
 *      entry_index - (IN)  Data entry index
 *      tpid        - (IN)  Tpid for out Tunnel Tag
 *      pcp         - (IN)  Pcp for out Tunnel Tag
 *      tunnel_id   - (IN)  Tunnel_id for out Tunnel Tag
 *      out_ac_id   - (IN)  Out AC index
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_encap_data_enty_add(int unit, uint32 entry_index, uint16 tpid,
    int pcp, bcm_tunnel_id_t tunnel_id, uint32 out_ac_id);

/*
 * Function:
 *      _bcm_dpp_pon_encap_data_enty_get
 * Purpose:
 *       Get PON Tunnel info of an EEDB entry 
 * Parameters:
 *      unit        - (IN)  Device Number
 *      entry_index - (IN)  Data entry index
 *      tpid        - (OUT) Tpid for out Tunnel Tag
 *      pcp         - (OUT) Pcp for out Tunnel Tag
 *      tunnel_id   - (OUT) Tunnel_id for out Tunnel Tag
 *      out_ac_id   - (OUT) Out AC index
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_encap_data_enty_get(int unit, uint32 entry_index, uint16 *tpid,
    int *pcp, bcm_tunnel_id_t *tunnel_id, int *out_ac_id);


#endif /* INCLUDE_GPORT_MGMT_H */

