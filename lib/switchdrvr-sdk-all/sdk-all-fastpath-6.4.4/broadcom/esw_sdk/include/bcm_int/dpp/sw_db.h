/* $Id: sw_db.h,v 1.15 Broadcom SDK $
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
 * $
 */
#ifndef _BCM_DPP_SW_DB_H_
#define _BCM_DPP_SW_DB_H_

#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/state.h>
#include <bcm/switch.h>
#include <bcm/module.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/soc_sw_db.h>


#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#endif

#define _BCM_DPP_L3_NOF_FECS(__unit) (SOC_DPP_CONFIG(__unit)->l3.fec_db_size) 

/* module space:
 * 0-79 - modules.h
 * 80-119 - bcmm_int/dpp/wb_db.h
 * 120-179 - common/wb_engine.h
 * 180-255 -soc_sw_db.h*
 */
#define BCM_MODULE_WB_BASE                           (80) /*start from 80 to allow module.h to grow*/
#define BCM_MODULE_WB_ALOOC_MNGR                     (BCM_MODULE_WB_BASE + 1)
#define BCM_MODULE_WB_SWITCH                         (BCM_MODULE_WB_BASE + 2)
#define BCM_MODULE_GPORT                             (BCM_MODULE_WB_BASE + 3)
#define BCM_MODULE_COUNTERS                          (BCM_MODULE_WB_BASE + 5)
#define BCM_MODULE_WB_LAST                           (BCM_MODULE_WB_BASE + 6) /* Always last */

/* BCM_MODULE_XXX is used as the scache module ID.
   If BCM_MODULE__COUNT has reached BCM_MODULE_WB_BASE, BCM_MODULE_WB_BASE needs to be enlarged.
   If BCM_MODULE_WB_LAST has reached SOC_MODULE_WB_BASE, SOC_MODULE_WB_BASE needs to be enlarged.
   Notice that changing scache module ID breaks ISSU. */
#if (BCM_MODULE_WB_BASE == BCM_MODULE__COUNT)
  #error "The assumption that BCM_MODULE_MAX > BCM_MODULE__COUNT is not longer valid.\n"
#endif
#if (BCM_MODULE_WB_LAST == SOC_MODULE_WB_BASE)
  #error "The assumption that SOC_MODULE_WB_BASE > BCM_MODULE_WB_LAST is not longer valid.\n"
#endif



typedef struct dos_attack_info_s {
  uint8 tocpu;
  /* flags for all dos attack traps */
  uint8 sipequaldip;
  uint8 mintcphdrsize;
  uint8 v4firstfrag;
  uint8 tcpflags;
  uint8 l4port;
  uint8 tcpfrag;
  uint8 icmp;
  uint8 icmppktoversize;
  uint8 macsaequalmacda;
  uint8 icmpv6pingsize;
  uint8 icmpfragments;
  uint8 tcpoffset;
  uint8 udpportsequal;
  uint8 tcpportsequal;
  uint8 tcpflagssf;
  uint8 tcpflagsfup;
  uint8 tcphdrpartial;
  uint8 pingflood;
  uint8 synflood;
  uint8 tcpsmurf;
  uint8 tcpxmas;
  uint8 l3header;
} dos_attack_info_t;

typedef struct dpp_switch_sw_db_s {
  dos_attack_info_t dos_attack; 
} bcm_dpp_switch_sw_db_t;

typedef struct fec_to_ecmps_s {
  int *ecmps;  
  int *corresponding_fecs;
  int nof_ecmps;  
} fec_to_ecmps_t;

typedef struct fec_copy_info_s {
  int ecmp;  
  int real_fec;
} fec_copy_info_t;


typedef struct ecmp_size_s {
  int cur_size;
  int max_size;
} ecmp_size_t;

typedef struct multi_device_sync_flags_s {
  int eep;
  int inlif;  
} multi_device_sync_flags_t;


typedef struct bcm_dpp_l3_sw_db_s {
  fec_to_ecmps_t *fec_to_ecmp; /* for real-FEC store copy-FEC and their ECMPs */
  ecmp_size_t *ecmp_size; /* size of ECMP and max size */
  uint8 *ecmp_successive; /* [y] == 1 --> ECMP y is successsive*/
  uint8 *ecmp_protected; /* [y] == 1 --> ECMP y  is protected */
  fec_copy_info_t  *fec_copy_info; /* for copy-fec, store real FEC and my ECMP*/
} bcm_dpp_l3_sw_db_t;

typedef struct bcm_dpp_mirror_sw_db_s {
  int mirror_mode;
  uint8 egress_port_profile[SOC_PB_PP_MAX_NOF_LOCAL_PORTS];
}bcm_dpp_mirror_sw_db_t;

extern int _bcm_dpp_init_finished_ok[BCM_MAX_NUM_UNITS];
extern int _cell_id_curr[BCM_MAX_NUM_UNITS];
extern bcm_dpp_l3_sw_db_t _l3_sw_db[BCM_MAX_NUM_UNITS];
extern bcm_dpp_switch_sw_db_t _switch_sw_db[BCM_MAX_NUM_UNITS];
extern bcm_dpp_mirror_sw_db_t _mirror_sw_db[BCM_MAX_NUM_UNITS];
extern bcm_dpp_state_t _dpp_state[BCM_MAX_NUM_UNITS];

int 
_bcm_sw_db_switch_urpf_mode_set(int unit, int urpf_mode);

int 
_bcm_sw_db_switch_urpf_mode_get(int unit, int *urpf_mode);

int 
_bcm_sw_db_switch_dos_attack_info_set(int unit, bcm_switch_control_t bcm_type, int enable);

int 
_bcm_sw_db_switch_dos_attack_info_get(int unit, bcm_switch_control_t bcm_type, int *enable);















int
bcm_sw_db_l3_fec_to_ecmps_get(int unit, int fec, fec_to_ecmps_t *fec_to_ecmps); 

/* given copy FEC return pointer to real FEC and used ECMP */
int
bcm_sw_db_l3_copy_fec_to_ecmps_get(int unit, int l3a_intf, fec_copy_info_t *fec_copy_info);

int
bcm_sw_db_l3_add_fec_mapping_to_ecmp(int unit,uint32 flags, int intf, int ecmp, int cor_fec);

int
bcm_sw_db_l3_fec_remove_fec(int unit, int intf);

int
bcm_sw_db_l3_set_ecmp_sizes(int unit, int mpintf, int max_size, int curr_size);

int
bcm_sw_db_l3_get_ecmp_sizes(int unit, int mpintf, int *max_size, int *curr_size);

int
bcm_sw_db_l3_set_ecmp_is_successive(int unit, int mpintf, uint8 successive);

int
bcm_sw_db_l3_get_ecmp_is_successive(int unit, int mpintf, uint8 *successive);

int
bcm_sw_db_l3_set_ecmp_is_protected(int unit, int mpintf, uint8 protected);

int
bcm_sw_db_l3_get_ecmp_is_protected(int unit, int mpintf, uint8 *protected);


int
bcm_sw_db_l3_dump_fec_to_ecmps(int unit, int start, int end);

int 
_bcm_sw_db_init(int unit);

int 
_bcm_sw_db_deinit(int unit);

int 
_bcm_sw_db_sync(int unit);

int
_bcm_sw_db_cell_id_curr_get(int unit, int *cell_id);

int
_bcm_sw_db_cell_id_curr_set(int unit, int *cell_id);

int
_bcm_sw_db_petra_mirror_mode_set(int unit, int mode);

int
_bcm_sw_db_petra_mirror_mode_get(int unit, int *mode);

int
_bcm_sw_db_outbound_mirror_port_profile_set(int unit, int port, uint8 profile);

int
_bcm_sw_db_outbound_mirror_port_profile_get(int unit, int port, uint8 *profile);

#endif
