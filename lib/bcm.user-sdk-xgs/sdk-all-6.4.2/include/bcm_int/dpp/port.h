/*
 * $Id: port.h,v 1.39 Broadcom SDK $
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
 * File:        port.h
 * Purpose:     PORT internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_PORT_H_
#define   _BCM_INT_DPP_PORT_H_

#include <soc/dpp/Petra/PB_TM/pb_nif.h>
#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_llp_trap.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>

#include <soc/dpp/TMC/tmc_api_action_cmd.h>
#include <bcm/port.h>

/*
 * port definition
 */

/* color mapping (DP value)*/
#define _BCM_DPP_COLOR_GREEN   0
#define _BCM_DPP_COLOR_YELLOW  1
#define _BCM_DPP_COLOR_RED     2
#define _BCM_DPP_COLOR_BLACK   3

/* number of FAP ports */
#define _BCM_PETRA_NOF_PP_PORTS(unit) (SOC_DPP_DEFS_GET(unit, nof_pp_ports))
#define _BCM_PETRA_NOF_TM_PORTS(unit) (SOC_DPP_DEFS_GET(unit, nof_logical_ports))
#define _BCM_PETRA_NOF_MAX_PP_PORTS   (SOC_PPD_MAX_NOF_LOCAL_PORTS)
/* number of traps used for learning mode per port*/
#define _BCM_PETRA_PORT_LEARN_NOF_TRAPS 4

#define _BCM_DPP_PORT_TRAP_CODE_SA_DROP_NOF_PROFILES 4
#define _BCM_DPP_PORT_TRAP_CODE_DA_UNKNOWN_NOF_PROFILES 4

#define _BCM_DPP_PORT_DISCARD_MODE_MIM_BIT              (0x40)
#define _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO           (0x80)
#define _BCM_DPP_PORT_DISCARD_MODE_MAX                  ((BCM_PORT_DISCARD_COUNT - 1) | _BCM_DPP_PORT_DISCARD_MODE_MIM_BIT | _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO)
#define _BCM_DPP_PORT_DISCARD_MODE_IS_MIM(_mode_)       (_mode_ & _BCM_DPP_PORT_DISCARD_MODE_MIM_BIT)       
#define _BCM_DPP_PORT_DISCARD_MODE_MIM_REMOVE(_mode_)   (_mode_ &= ~_BCM_DPP_PORT_DISCARD_MODE_MIM_BIT)       
#define _BCM_DPP_PORT_DISCARD_MODE_MIM_SET(_mode_)      (_mode_ |= _BCM_DPP_PORT_DISCARD_MODE_MIM_BIT)  
#define _BCM_DPP_PORT_DISCARD_MODE_IS_OUTER_PRIO(_mode_) (_mode_ & _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO)       
#define _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO_REMOVE(_mode_) (_mode_ &= ~_BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO)       
#define _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO_SET(_mode_) (_mode_ |= _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO)


/* user define trap used for drop */
#define _BCM_PETRA_UD_DROP_TRAP (SOC_PPC_TRAP_CODE_USER_DEFINED_DROP_TRAP)
#define _BCM_PETRA_UD_DFLT_TRAP (SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP)

typedef struct bcm_dpp_mal_assoc_s {
    uint8 mal;
    uint8 lanes;
    uint16 nif_type;
} bcm_dpp_mal_assoc_t;

typedef struct bcm_dpp_user_defined_traps_s {
    SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO trap;
    uint32 snoop_cmd;
    uint32 gport_trap_id; /*Added for support: bcmRxTrapL2Cache (virtual traps) */
    uint32 ud_unique; /*Added for support: sync user define traps in rx.c with port.c */
} bcm_dpp_user_defined_traps_t;


typedef struct bcm_dpp_snoop_s {
	SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO snoop_action_info; /*snoop profile data */
	uint32 snoop_unique; /*Added for support: sync snoop between diffrent modules */
} bcm_dpp_snoop_t;


/* number of bits per tag-structure
    0:0  PcpDeiProfile  This is the port-based PCP-DEI mapping profile.
    6:1  Ivec  This is the port-based ingress VLAN edit command. 19:16  
    10:7 IncomingTagStructure  Used together with the VLAN-Edit-Profile 
    11:11 discard
    12:12: outer-cep
    13:13 inner-cep
    19:14 reserved
*/
#define _BCM_DPP_PORT_TPID_CLASS_BITS_PER_TAG_STRCT (20)

/* number of U32 need, for all tag structures 2^5 = 32 ==> 20 U32 */
#define _BCM_DPP_PORT_TPID_CLASS_INFO_BUF_NOF_U32 (20)

typedef struct _bcm_dpp_port_tpid_class_info_s {
    uint32 buff[_BCM_DPP_PORT_TPID_CLASS_INFO_BUF_NOF_U32];
} _bcm_dpp_port_tpid_class_info_t;



typedef enum _bcm_petra_tpid_profile_e{
    _bcm_petra_tpid_profile_none = 0,
    _bcm_petra_tpid_profile_outer,
    _bcm_petra_tpid_profile_outer_inner,
    _bcm_petra_tpid_profile_outer_inner2,
    _bcm_petra_tpid_profile_outer_outer,
    _bcm_petra_tpid_profile_inner_outer,
    _bcm_petra_tpid_profile_inner_outer2, /* TPID1: outer + inner TPID2:outer */
    _bcm_petra_tpid_profile_outer_inner_same, /* TPID1: outer TPID1: inner */
    _bcm_petra_tpid_profile_outer_inner_same2, /* TPID2: outer TPID2: inner */
    _bcm_petra_tpid_profile_outer_c_tag, /* TPID1: C-tag*/
    _bcm_petra_tpid_profile_outer_outer_c_tag, /* TPID1: C-tag TPID2: C-tag*/
    _bcm_petra_tpid_profile_outer_inner_c_tag, /* TPID1: S-tag, TPID2: C-tag */
    _bcm_petra_tpid_profile_outer_trill_fgl, /* TPID1: S-tag, TPID2: C-tag */
    _bcm_petra_tpid_profile_count
} _bcm_petra_tpid_profile_t;

typedef enum _bcm_petra_dtag_mode_e{
    _bcm_petra_dtag_mode_accept = 0, /* Accept double tag */
    _bcm_petra_dtag_mode_ignore, /* Ignore double tag. Parse as one tag */    
    _bcm_petra_dtag_mode_count
} _bcm_petra_dtag_mode_t;

typedef enum _bcm_petra_ac_key_map_type_e{
    _bcm_petra_ac_key_map_type_normal = 0,
    _bcm_petra_ac_key_map_type_compressional,
    _bcm_petra_ac_key_map_type_count
} _bcm_petra_ac_key_map_type_t;

typedef enum _bcm_petra_port_mact_mgmt_action_profile_e{
    _bcm_petra_port_mact_mgmt_action_profile_none = 0,
    _bcm_petra_port_mact_mgmt_action_profile_drop,
    _bcm_petra_port_mact_mgmt_action_profile_trap,
    _bcm_petra_port_mact_mgmt_action_profile_trap_and_drop    
} _bcm_petra_port_mact_mgmt_action_profile_t;



typedef enum _bcm_dpp_port_map_type_e {
   _bcm_dpp_port_map_type_up_to_dp=0,/* mapping UP to DP */         
   _bcm_dpp_port_map_type_in_up_to_tc_and_de,
   _bcm_dpp_port_map_type_count       
} _bcm_dpp_port_map_type_t;


#define _BCM_DPP_PORT_MAP_MAX_NOF_TBLS   (8)



/* number of different tpid values */
#define _BCM_PORT_NOF_TPID_VALS (4)
/* number of TPID profiles pair of TPIDs */
#define _BCM_PORT_NOF_TPID_PROFILES (4)

#define _BCM_PETRA_NOF_TPIDS_PER_PORT (2)




typedef struct {

    uint16 tpid[_BCM_PETRA_NOF_TPIDS_PER_PORT][_BCM_PORT_NOF_TPID_PROFILES]; /* tpid 1/2 index <value> */
    int tpid_count[_BCM_PETRA_NOF_TPIDS_PER_PORT][_BCM_PORT_NOF_TPID_PROFILES]; /* number of users of this tpid in the profile */

} _bcm_petra_port_tpid_info;





typedef struct bcm_dpp_port_config_s {

    int bcm_petra_port_init_arrays_done;

    int port_pp_initialized;

    int map_tbl_use[_bcm_dpp_port_map_type_count][_BCM_DPP_PORT_MAP_MAX_NOF_TBLS];

    /* for each learn trap what is the usage */
    int trap_to_flag[_BCM_PETRA_PORT_LEARN_NOF_TRAPS];

    _bcm_petra_port_tpid_info *_bcm_port_tpid_info;

} bcm_dpp_port_config_t;


typedef struct _bcm_dpp_port_tpid_class_acceptable_frame_type_info_s {
    uint32 buffer;
} _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t;


extern pbmp_t _bcm_dpp_port_prbs_mac_mode[BCM_MAX_NUM_UNITS];
extern bcm_dpp_port_config_t _dpp_port_config[BCM_MAX_NUM_UNITS];

/*
 * External functions
 */
extern int bcm_petra_port_advert_remote_get(int unit, bcm_port_t port, bcm_port_abil_t *ability_mask);

int 
_bcm_petra_port_class_nof_vlan_domains_count(
    int unit, 
    uint32 class_id,
	int *nof_pp_ports);

extern uint32
soc_petra_ihp_prog_n00_load_unsafe(int unit);

extern int 
_bcm_petra_port_to_nif_id(int unit, bcm_port_t port, int *nif_id);

extern int
_bcm_petra_port_gport_validate(int unit, bcm_port_t port_in, bcm_port_t *port_out);

extern int
bcm_petra_port_count_get(int unit, int *p_num_ports);

extern int
_bcm_petra_port_link_get(int unit, bcm_port_t port, int hw, int *up);

extern int
_bcm_petra_port_find_free_port_and_allocate(int unit, bcm_port_if_t interface, int with_id, bcm_port_t *port);

/* These functions below are required for cosq TM */

extern int
bcm_petra_fap_port_speed_get(int unit, int fap_port, int *p_speed_in_kbps);

extern int
bcm_petra_fap_port_parent_nif_get(int unit, int fap_port, int *p_nif_port);

extern int 
bcm_port_to_nif_id(int unit, bcm_port_t port, int *nif_id);

extern int 
bcm_port_from_nif_id(int unit, int nif_id, bcm_port_t *port);

extern int 
bcm_port_to_scaled_down_nif_id(int unit, int nif_id, uint32 *scaled_down_nif_id);

extern int
bcm_petra_port_vlan_priority_map_set(int unit, bcm_port_t port, int pkt_pri, int cfi, int internal_pri, bcm_color_t color);

extern int
bcm_petra_port_vlan_priority_map_get(int unit, bcm_port_t port, int pkt_pri, int cfi, int *internal_pri, bcm_color_t *color);

extern int
bcm_petra_port_vlan_priority_unmap_set(int unit, bcm_port_t port, int internal_pri, bcm_color_t color, int pkt_pri, int cfi);

extern int
bcm_petra_port_vlan_priority_unmap_get(int unit, bcm_port_t port, int internal_pri, bcm_color_t color, int *pkt_pri, int *cfi);

extern int
bcm_petra_port_vlan_pri_map_get(int unit, bcm_port_t port, bcm_vlan_t vid, int pkt_pri, int cfi, int *internal_pri, bcm_color_t *color);

extern int
bcm_petra_port_vlan_pri_map_set(int unit, bcm_port_t port, bcm_vlan_t vid, int pkt_pri, int cfi, int internal_pri, int color);

extern int
_bcm_petra_port_color_encode(int, bcm_color_t, int *);

extern int 
_bcm_petra_port_color_decode(int unit, int dp, bcm_color_t *color);


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern int
_bcm_dpp_port_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
 	 
int
_bcm_petra_port_discard_extend_dtag_mode_set(int unit,bcm_port_t port,_bcm_petra_dtag_mode_t dtag_mode);

int
_bcm_petra_port_discard_extend_get(int unit, bcm_port_t port, int *mode);

int
_bcm_petra_port_discard_extend_dtag_mode_get(int unit,bcm_port_t port,_bcm_petra_dtag_mode_t *dtag_mode);

int 
_bcm_petra_port_discard_extend_mode_set(int unit, bcm_port_t port, int mode);

int
bcm_petra_port_tpid_profile_exact_match_search(int unit, int index, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_exact_match);

int
bcm_petra_port_tpid_profile_first_match_search(int unit, int index, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_exact_match);

int
bcm_petra_port_tpid_profile_opposite_match_search(int unit, int indx, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_exact_match);

int
bcm_petra_port_tpid_profile_similar_match_search(int unit, int tpid_profile_idx, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_match);


/* given a lif, update its tpid profile index (to match given tpid) */
int
_bcm_petra_mim_tpid_profile_update(int unit, int lif, uint16 tpid);


extern int
bcm_petra_port_deinit(int unit);

/* Attributes taht can be controlled on BCM88650*/
#define _BCM_DPP_PORT_ATTRS     \
    (BCM_PORT_ATTR_ENABLE_MASK      | \
    BCM_PORT_ATTR_SPEED_MASK       | \
    BCM_PORT_ATTR_DUPLEX_MASK      | \
    BCM_PORT_ATTR_LINKSCAN_MASK    | \
    BCM_PORT_ATTR_VLANFILTER_MASK  | \
    BCM_PORT_ATTR_INTERFACE_MASK   | \
    BCM_PORT_ATTR_LOOPBACK_MASK    | \
    BCM_PORT_ATTR_STP_STATE_MASK   | \
    BCM_PORT_ATTR_PAUSE_TX_MASK    | \
    BCM_PORT_ATTR_PAUSE_RX_MASK)

int 
bcm_petra_tpid_profile_info_set(int unit, int port_profile, _bcm_petra_tpid_profile_t profile_type, int accept_mode, _bcm_petra_dtag_mode_t dtag_mode, _bcm_petra_ac_key_map_type_t ac_key_map_type, uint8 cep_port, uint8 evb_port);

int _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t_clear(int unit, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *tpid_class_acceptable_frame_info);

void _bcm_dpp_port_tpid_class_info_t_to_bcm_dpp_port_tpid_class_acceptable_frame_type_info_t(int unit, _bcm_dpp_port_tpid_class_info_t *tpid_class_info, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *tpid_class_acceptable_frame_info);

int bcm_petra_port_tpid_profile_alloc(int unit, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int *tpid_profile);

#endif /* _BCM_INT_PETRA_PORT_H_ */
