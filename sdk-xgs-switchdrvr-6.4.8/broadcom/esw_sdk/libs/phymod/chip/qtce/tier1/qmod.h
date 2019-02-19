/*----------------------------------------------------------------------
 * $Id: qmod.h,
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
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators  
 *----------------------------------------------------------------------*/
/*
 * $Id: //depot/ip/qtc/main/dv/common/qmod/include/qmod.h#3 $ 
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
 */

#ifndef _QMOD_H_
#define _QMOD_H_

#ifndef _DV_TB_
#define _SDK_QMOD_ 1
#endif

#ifdef _SDK_QMOD_
#include <phymod/phymod.h>
#endif

#ifndef PHYMOD_ST  
#ifdef _SDK_QMOD_
  #define PHYMOD_ST   const phymod_access_t
#else
  #define PHYMOD_ST  qmod_st
#endif /* _SDK_QMOD_ */
#endif /* PHYMOD_ST */

#ifdef _DV_TB_
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"
#endif

typedef enum {
    QMOD_AN_MODE_CL73 = 0,
    QMOD_AN_MODE_CL37,
    QMOD_AN_MODE_CL73BAM,
    QMOD_AN_MODE_CL37BAM,
    QMOD_AN_MODE_HPAM,       
    QMOD_AN_MODE_SGMII,
    QMOD_AN_MODE_NONE,
    QMOD_AN_MODE_TYPE_COUNT
}qmod_an_mode_type_t;

typedef enum {
    QMOD_TX_LANE_TRAFFIC = 0,  
    QMOD_TX_LANE_RESET = 2,
    QMOD_TX_LANE_RESET_TRAFFIC = 4,
    QMOD_TX_LANE_TYPE_COUNT
}tx_lane_disable_type_t;


typedef struct qmod_an_adv_ability_s{
  uint16_t an_base_speed; 
  uint16_t an_bam_speed; 
  uint16_t an_bam_speed1; 
  uint16_t an_nxt_page; 
  uint16_t an_pause; 
  uint16_t an_fec; 
  uint16_t an_cl72;
  uint16_t an_hg2; 
  uint16_t cl37_sgmii_speed;
  qmod_an_mode_type_t an_type;
}qmod_an_adv_ability_t;

extern int qmod_pll_lock_wait(PHYMOD_ST* pc, int timeOutValue);
extern int qmod_credit_control(PHYMOD_ST* pc, int enable);
extern int qmod_tx_lane_control(PHYMOD_ST* pc, int enable, tx_lane_disable_type_t tx_dis_type);
extern int qmod_power_control(PHYMOD_ST* pc);
extern int qmod_override_set(PHYMOD_ST* pc, override_type_t or_type, uint16_t or_val);
extern int qmod_credit_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit);
extern int qmod_rx_lane_control_set(PHYMOD_ST* pc, int enable);        
extern int qmod_mld_am_timers_set(PHYMOD_ST* pc, int rx_am_timer_init,int tx_am_timer_init_val);
extern int qmod_init_pmd(PHYMOD_ST* pc, int pmd_touched, int uc_active);
extern int qmod_pmd_osmode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf, int os_mode);
extern int qmod_pll_lock_get(PHYMOD_ST* pc, int* lockStatus);
extern int qmod_pmd_lock_get(PHYMOD_ST* pc, int* lockStatus);
extern int qmod_wait_pmd_lock(PHYMOD_ST* pc, int timeOutValue, int* lockStatus);
extern int qmod_pcs_bypass_ctl (PHYMOD_ST* pc, int cntl);
extern int qmod_pmd_reset_bypass (PHYMOD_ST* pc, int cntl);
extern int qmod_set_pll_mode(PHYMOD_ST* pc, int pmd_tched, qmod_spd_intfc_type sp, int pll_mode);
extern int qmod_tick_override_set(PHYMOD_ST* pc, int tick_override, int numerator, int denominator);
extern int qmod_tx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int qmod_tx_pmd_loopback_control(PHYMOD_ST* pc, int cntl);
extern int qmod_rx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int qmod_encode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf);
extern int qmod_encode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf);
extern int qmod_decode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf);
extern int qmod_decode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf);
extern int qmod_spd_intf_get(PHYMOD_ST* pc, int *spd_intf);
extern int qmod_revid_read(PHYMOD_ST* pc, int *revid);
extern int qmod_init_pcs(PHYMOD_ST* pc);
extern int qmod_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched);
extern int qmod_wait_sc_done(PHYMOD_ST* pc, uint16_t *data);
extern int qmod_get_pcs_link_status(PHYMOD_ST* pc, int *link);
extern int qmod_disable_set(PHYMOD_ST* pc);
extern int qmod_pmd_x4_reset(PHYMOD_ST* pc);
extern int qmod_init_pmd_sw(PHYMOD_ST* pc, int pmd_touched, int uc_active,
                             qmod_spd_intfc_type spd_intf,  int t_pma_os_mode);
extern int qmod_fecmode_set(PHYMOD_ST* pc, int fec_enable);
extern int qmod_credit_override_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit);
extern int qmod_pmd_lane_reset(PHYMOD_ST* pc);
extern int qmod_cjpat_crpat_mode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd, int port, int pat, int pnum, int size, int ipgsz);
extern int qmod_cjpat_crpat_check(PHYMOD_ST* pc);
extern int qmod_cjpat_crpat_control(PHYMOD_ST* pc, int enable);
extern int qmod_pcs_lane_swap(PHYMOD_ST *pc, int lane_map);
extern int qmod_trigger_speed_change(PHYMOD_ST* pc);
extern int qmod_rx_lane_control_get(PHYMOD_ST* pc, int *value);
extern int qmod_pmd_override_control(PHYMOD_ST* pc, int cntl, int value);


typedef struct qmod_an_ability_s {
    qmod_an_adv_ability_t cl37_adv;  /*this will include all the cl37 and cl37-bam related  (everything inlclduing fec, cl72 ) */
    qmod_an_adv_ability_t cl73_adv ;/*this will include all the cl73 and cl73-bam related */
} qmod_an_ability_t;

typedef struct qmod_an_control_s {
  qmod_an_mode_type_t an_type; 
  uint16_t num_lane_adv; 
  uint16_t enable;
  uint16_t pd_kx_en;
  uint16_t pd_kx4_en; 
  an_property_enable  an_property_type;
} qmod_an_control_t;


typedef struct qmod_an_init_s{
  uint16_t  an_fail_cnt;
  uint16_t  an_oui_ctrl; 
  uint16_t  linkfailtimer_dis; 
  uint16_t  linkfailtimerqua_en; 
  uint16_t  an_good_check_trap; 
  uint16_t  an_good_trap; 
  uint16_t  disable_rf_report; 
  uint16_t  cl37_bam_ovr1g_pgcnt; 
  uint16_t  cl73_remote_fault; 
  uint16_t  cl73_nonce_match_over;
  uint16_t  cl73_nonce_match_val;
  uint16_t  cl73_transmit_nonce; 
  uint16_t  base_selector;
} qmod_an_init_t;


typedef struct qmod_an_timers_s{
  uint16_t  value;
} qmod_an_timers_t;

#ifdef _SDK_QMOD_
extern int qmod_autoneg_set_init(PHYMOD_ST* pc, qmod_an_init_t *an_init_st); 
extern int qmod_autoneg_control(PHYMOD_ST* pc, qmod_an_control_t *an_control); 
extern int qmod_autoneg_timer_init(PHYMOD_ST* pc);
extern int qmod_autoneg_set(PHYMOD_ST* pc, qmod_an_ability_t *an_ability_st);
extern int qmod_set_an_port_mode(PHYMOD_ST* pc, int num_of_lanes, int starting_lane, int single_port);
extern int qmod_pmd_lane_swap_tx(PHYMOD_ST *pc, uint32_t tx_lane_map_0, uint32_t tx_lane_map_1, uint32_t tx_lane_map_2, uint32_t tx_lane_map_3);
int qmod_get_mapped_speed(qmod_spd_intfc_type spd_intf, int *speed);
#endif
int get_mapped_speed(qmod_spd_intfc_type spd_intf, int *speed);
int get_actual_speed(int speed_id, int *speed);


typedef enum {
  QMOD_SPD_ILLEGAL_             = 0   /*!< Illegal value (enum boundary)   */
} qmod_spd_intfc_type_t;


typedef struct sc_table_entry_s {
  int illegal_;
} sc_table_entry_t;


#endif  /*  _qmod_H_ */

