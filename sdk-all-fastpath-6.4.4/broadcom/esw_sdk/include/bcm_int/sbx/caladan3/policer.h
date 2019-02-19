/* 
 * $Id: policer.h,v 1.2 Broadcom SDK $
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
 * File:        policer.h
 * Purpose:     Policer internal definitions to the BCM library.
 */

#ifndef _BCM_INT_SBX_CALADAN3_POLICER_H_
#define _BCM_INT_SBX_CALADAN3_POLICER_H_

#define BCM_CALADAN3_XT_POLICERS  0xff /* 1-255 are reserved for exceptions */
#define BCM_CALADAN3_PORT_POLICERS (SBX_MAX_PORTS * SBX_MAX_COS)
#define BCM_CALADAN3_SPECIAL_POLICERS     1 /* so far just one: drop all */
#define BCM_CALADAN3_SPEC_POL_DROP_ALL    (BCM_CALADAN3_XT_POLICERS + \
                                         BCM_CALADAN3_PORT_POLICERS + 1)
#define BCM_CALADAN3_RESERVED_POLICERS    (BCM_CALADAN3_XT_POLICERS + \
                                         BCM_CALADAN3_PORT_POLICERS + \
                                         BCM_CALADAN3_SPECIAL_POLICERS)

#define BCM_CALADAN3_POLICER_GROUPID       SOC_SBX_G2P3_INGEGRPOL_ID
#define BCM_CALADAN3_EXC_POLICER_GROUPID   SOC_SBX_G2P3_XTPOL_ID

#define BCM_CALADAN3_NUM_MONITORS         0x8     /* Num of monitor resources */

typedef struct _bcm_policer_node_s {
    bcm_policer_t               id;         /* policer id */
    bcm_policer_group_mode_t    group_mode; /* group mode policer belongs */
    uint32                      cookie;     /* store info in this node */
    struct _bcm_policer_node_s  *prev;      /* prev in the created list */
    struct _bcm_policer_node_s  *next;      /* next in the created list */
    bcm_policer_config_t        *pol_cfg;   /* policer config */
} _bcm_policer_node_t;

typedef struct _bcm_policer_glob_s {
    sal_mutex_t                 lock;      /* per unit lock */
    int                         pol_count; /* number of created policers */
    _bcm_policer_node_t         *pol_head; /* list of created policers */
    _bcm_policer_node_t         *policers; /* ptr to array of policer nodes */
    shr_idxres_list_handle_t    res_idlist;/* usage list of reserved policer ids */
    shr_aidxres_list_handle_t   idlist;    /* usage list of policers ids */
    bcm_policer_t               mon_use_map[BCM_CALADAN3_NUM_MONITORS]; /* Monitor 
                                                                         usage map */
    uint32                    numExceptionPolicers;
} _bcm_policer_glob_t;

typedef struct _bcm_policer_node_g3p1_cookie_s {
    uint32      base_cntr;
} _bcm_policer_node_g3p1_cookie_t;


extern int
_bcm_caladan3_policer_max_id_get(int unit);

extern int
_bcm_caladan3_policer_num_policers_get(int unit);

extern int
_bcm_caladan3_policer_group_mode_get(int unit, bcm_policer_t id,
                                   bcm_policer_group_mode_t *grp_mode);

extern int
_bcm_caladand3_policer_unit_lock(int unit);

extern int
_bcm_caladan3_policer_unit_unlock(int unit);

extern int
_bcm_caladan3_attach_monitor(int unit, bcm_policer_t pol_id);

extern int
_bcm_caladan3_detach_monitor(int unit, bcm_policer_t pol_id);

extern int
_bcm_caladan3_is_monitor_stat_enabled(int unit, bcm_policer_t policer_id, 
                                    int *monitor_id);
extern int
_bcm_caladan3_monitor_stat_get(int unit, int monitor_id, 
                            bcm_policer_stat_t stat, uint64 *val);
extern int
_bcm_caladan3_monitor_stat_set(int unit, int monitor_id, 
                             bcm_policer_stat_t stat, uint64 val);


extern const char *
_bcm_caladan3_policer_group_mode_to_str(bcm_policer_group_mode_t groupMode);

extern int
bcm_caladan3_policer_stat_get(int unit, bcm_policer_t policer_id, bcm_cos_t cos,
                            bcm_policer_stat_t stat, uint64 *val);
extern int
bcm_caladan3_policer_stat_set(int unit, bcm_policer_t policer_id, bcm_cos_t cos,
                            bcm_policer_stat_t stat, uint64 val);

#ifdef BCM_CALADAN3_G3P1_SUPPORT

extern int _bcm_caladan3_g3p1_policer_lp_program(int unit, bcm_policer_t pol_id,
                                               soc_sbx_g3p1_lp_t *lp);

extern int _bcm_caladan3_g3p1_policer_stat_mem_get(int unit,
                                            bcm_policer_group_mode_t grp_mode, 
                                            bcm_policer_stat_t stat, int cos,
                                            int *ctr_offset, int *pkt);

extern int
_bcm_caladan3_g3p1_alloc_counters(int unit, 
                                bcm_policer_t policer_id,
                                int use_cookie,
                                uint32 *counter);

extern int
_bcm_caladan3_g3p1_free_counters(int unit,
                               bcm_policer_t policer_id,
                               int use_cookie,
                               uint32 counter);

extern int
_bcm_caladan3_g3p1_policer_stat_get(int unit, bcm_policer_t policer_id, 
                                  bcm_policer_stat_t stat, int cos, 
                                  int use_cookie, uint32 counter,
                                  int clear, uint64 *val);

extern int
_bcm_caladan3_g3p1_policer_stat_set(int unit, bcm_policer_t policer_id, 
                                  bcm_policer_stat_t stat, int cos,
                                  int use_cookie, uint32 counter,
                                  uint64 val);

extern int
_bcm_caladan3_g3p1_num_counters_get(int unit,
                                  bcm_policer_group_mode_t mode,
                                  int *ncounters);


#endif /* BCM_CALADAN3_G3P1_SUPPORT */

#endif /* _BCM_INT_SBX_CALADAN3_POLICER_H_ */
