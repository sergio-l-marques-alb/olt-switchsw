/*
 * $Id: rx.h,v 1.19 Broadcom SDK $ 
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
 * IPMC Internal header
 */

#ifndef _BCM_INT_DPP_RX_H_
#define _BCM_INT_DPP_RX_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/rx.h>
#include <soc/dpp/dpp_wb_engine.h>

/* Defines BCM no action profile */
#define _BCM_DPP_TRAP_EG_NO_ACTION (0)
#define _BCM_DPP_TRAP_PREVENT_SWAPPING (-1)
#define _BCM_DPP_TRAP_INVALID_VIRTUAL_ID (-2)
/*
 *  Egress traps don't really work that way.  The 'trap' action takes a profile
 *  (not a trap) as an argument, and follows the profile action.  Still, need a
 *  way to manage these profiles, but until then, just take the top one.
 */
#define _BCM_PPD_TRAP_MGMT_EG_ACTION_PROFILE_MAX       (7) 
#define _DPP_SWITCH_TRAP_TO_CPU_PROFILE_INDEX          (0x2)
#define _DPP_SWITCH_EG_TRAP_DROP_PROFILE_INDEX         (0x1)
#define _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT                (10)
#define _BCM_DPP_RX_TRAP_TYPE_MASK                	   (0x3)
#define _BCM_DPP_RX_TRAP_ID_MASK					   (0x3ff)
#define _BCM_DPP_RX_TRAP_ID_BIT_MAP_SHIFT			   (16)


typedef enum
{
  /*
   *  ingress trap.
   */
  _BCM_DPP_RX_TRAP_INGRESS = 0x0,
  /*
   *  TBD trap.
   */
  _BCM_DPP_RX_TRAP_OAMP_ERROR = 0x1,
  /*
   *  egresss trap.
   */
  _BCM_DPP_RX_TRAP_EGRESS = 0x2,
  /*
   *  virtual trap.
   */
  _BCM_DPP_RX_TRAP_VIRTUAL = 0x3,
  /*
   *  Number of types in _BCM_DPP_RX_TRAP_TYPE_SIZES
   */
  _BCM_DPP_RX_TRAP_TYPE_SIZES = 0x4
}_BCM_DPP_RX_TRAP_TYPE;



/* gport trap full encoding:
   bits 0:9 trap id
   bits 10:11 virtual/eggress symbole
   bits 12:15 trap strength
   bits 16:19 snoop strength
   bits 26:31 trap type
*/
#define _BCM_TRAP_ID_IS_VIRTUAL(_virtual_trap)        (((_virtual_trap >> _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) & _BCM_DPP_RX_TRAP_TYPE_MASK) == _BCM_DPP_RX_TRAP_VIRTUAL) /* Bits 11:10 == 0x3 mark the trap is virtual */
#define _BCM_TRAP_ID_IS_EGRESS(_eg_trap)        (((_eg_trap >> _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) & _BCM_DPP_RX_TRAP_TYPE_MASK) == _BCM_DPP_RX_TRAP_EGRESS) /* Bits 11:10 == 0x2 mark the trap is virtual */
#define _BCM_TRAP_ID_IS_OAM(_oam_trap)        (((_oam_trap >> _BCM_DPP_RX_TRAP_SUB_TYPE_SHIFT) & _BCM_DPP_RX_TRAP_TYPE_MASK) == _BCM_DPP_RX_TRAP_OAMP_ERROR) /* Bits 11:10 == 0x1 mark the trap is oam */

/*clears virtual bit*/
#define _BCM_TRAP_ID_TO_VIRTUAL(_virtual_trap)  ( _BCM_DPP_RX_TRAP_ID_MASK & _virtual_trap) /*set the bit 11:10 to zero, , total number of bits for traps is 10*/
#define _BCM_TRAP_ID_TO_OAM(_oam_trap)  ( _BCM_DPP_RX_TRAP_ID_MASK & _oam_trap) /*set the bit 11:10 to zero, , total number of bits for traps is 10*/

#define _BCM_NUMBER_OF_VIRTUAL_TRAPS _SOC_NUMBER_OF_VIRTUAL_TRAPS

typedef soc_dpp_rx_virtual_traps_t bcm_dpp_rx_virtual_traps_t;

extern int _packet_counter[BCM_MAX_NUM_UNITS];

int _bcm_dpp_rx_virtual_trap_get(int unit, int virtual_trap_id, bcm_dpp_rx_virtual_traps_t *virtual_trap_str);
int _bcm_dpp_rx_virtual_trap_set(int unit, int virtual_trap_id, bcm_dpp_rx_virtual_traps_t *virtual_trap_str);
int _bcm_dpp_rx_egress_set(int unit,int trap_id, SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO *prm_egr_profile_info);
int _bcm_dpp_rx_egress_set_to_drop_info(int unit, SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO *eg_drop_info);
int _bcm_dpp_rx_egress_set_to_cpu_drop_info(int unit, SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO *eg_cpu_drop_info);
int _bcm_dpp_rx_trap_set(int unit, int trap_id, bcm_rx_trap_config_t *config, uint8 trap_fwd_offset_index);
int _bcm_petra_rx_partial_init(int);
int _bcm_dpp_rx_trap_id_to_egress(int _eg_trap);
int _bcm_dpp_rx_ppd_to_forwarding_type(SOC_TMC_PKT_FRWRD_TYPE ppdForwardingType, bcm_forwarding_type_t *bcmForwardingType);
int _bcm_dpp_rx_forwarding_type_to_ppd(bcm_forwarding_type_t bcmForwardingType, SOC_TMC_PKT_FRWRD_TYPE *ppdForwardingType);

void _bcm_dpp_rx_ppd_trap_get(int unit, bcm_rx_trap_t trap_type, int *soc_ppd_trap_id);
void _bcm_dpp_rx_trap_type_get(int unit, int soc_ppd_trap_id, uint8 *found, bcm_rx_trap_t *trap_type);

int _bcm_dpp_rx_packet_parse(int unit, bcm_pkt_t *pkt, uint8 device_access_allowed);
void _bcm_dpp_rx_packet_cos_parse(int unit, bcm_pkt_t *pkt);

int _bcm_rx_ppd_trap_code_from_trap_id(int unit, int trap_id, SOC_PPD_TRAP_CODE *ppd_trap_code);

#endif /* _BCM_INT_DPP_RX_H_ */

