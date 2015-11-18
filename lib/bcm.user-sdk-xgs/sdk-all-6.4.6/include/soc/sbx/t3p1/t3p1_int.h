/*
 * $Id: t3p1_int.h,v 1.1 Broadcom SDK $
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
 * This file contains aggregated definitions for Guadalupe 3 microcode
 */

#ifndef _SOC_SBX_T3P1_INT_H
#define _SOC_SBX_T3P1_INT_H

#include <soc/types.h>
#include <soc/drv.h>

#ifdef BCM_CALADAN3_SUPPORT

#include <soc/sbx/sbx_drv.h>
#include <soc/sbx/caladan3/ocm.h>
#include <soc/sbx/caladan3/lrp.h>
#include <soc/sbx/caladan3/util.h>
#include <soc/sbx/caladan3/port.h>
#include <soc/sbx/caladan3/ucodemgr.h>
#include <soc/sbx/caladan3/util.h>
#include <sal/appl/sal.h>
#include <soc/debug.h>
#include <soc/sbx/caladan3/port.h>
#include <soc/sbx/caladan3/ped.h>
#include <soc/sbx/caladan3/ppe.h>
#include <soc/sbx/t3p1/t3p1_ppe_rule_encode.h>
#include <soc/sbx/t3p1/t3p1_ped.h>
#include <soc/sbx/t3p1/t3p1_tmu.h>
#include <soc/sbx/t3p1/t3p1_cop.h>
#include <soc/sbx/t3p1/t3p1_cmu.h>
#include <soc/sbx/t3p1/t3p1_ppe_tables.h>

#include <soc/sbx/caladan3/simintf.h>

/* Bit mask fields used in fieldMask argument to psc hash template get/set functions */

#define SOC_SBX_T3P1_BUBBLE_UPDATE_MODE_DISABLED            0
#define SOC_SBX_T3P1_BUBBLE_UPDATE_MODE_RESERVED            1
#define SOC_SBX_T3P1_BUBBLE_UPDATE_MODE_INSERT_COUNT_TIMES  2
#define SOC_SBX_T3P1_BUBBLE_UPDATE_MODE_INSERT_CONTINUOUSLY 3
#define SOC_SBX_T3P1_BUBBLE_UPDATE_MODE_LAST                SOC_SBX_T3P1_BUBBLE_UPDATE_MODE_INSERT_CONTINUOUSLY


typedef struct soc_sbx_t3p1_state_s {
    int unit;
    soc_sbx_caladan3_ucode_pkg_t *ucode;
    void *regSet;
    void *tableinfo;
    void *tmu_mgr;
    void *ppe_mgr;
} soc_sbx_t3p1_state_t;

typedef struct soc_sbx_t3p1_util_timer_event_s {
    soc_sbx_t3p1_cop_timer_segment_t timer_segment;
    uint32 id;
    uint8  forced_timeout;
    uint8  timer_active_when_forced;
} soc_sbx_t3p1_util_timer_event_t;

/**
 *
 * PPE Property table configuration tables
 */
#define SOC_SBX_T3P1_PPE_PROPERTY_TABLE_SEGMENT_MAX 8
typedef struct soc_sbx_t3p1_ppe_ptable_segment_s {
  char *name;
  int segment;
  int seg_id;
  int start;
} soc_sbx_t3p1_ppe_ptable_segment_t;

typedef struct soc_sbx_t3p1_ppe_ptable_cfg_s {
  int mode;
  int portA;
  int portB;
  int portC;
  int portD;
  soc_sbx_t3p1_ppe_ptable_segment_t segment[SOC_SBX_T3P1_PPE_PROPERTY_TABLE_SEGMENT_MAX];
} soc_sbx_t3p1_ppe_ptable_cfg_t;

typedef void (*soc_sbx_t3p1_util_timer_event_callback_f)(int unit, soc_sbx_t3p1_util_timer_event_t *event, void *user_cookie);


#define UTG_SYM_GET(h, p, n, v) \
    soc_sbx_caladan3_ucodemgr_sym_get(h, p, n, v)
#define UTG_SYM_SET(h, p, n, v) \
    soc_sbx_caladan3_ucodemgr_sym_set(h, p, n, v)

/* Hand coded PPE functions */
extern int soc_sbx_t3p1_ppe_property_table_segment_get(int unit, int seg_id,
                                                       uint32 offset, uint8 *data);
extern int soc_sbx_t3p1_ppe_property_table_segment_set(int unit, int seg_id,
                                                       uint32 offset, uint8  data);

extern int soc_sbx_t3p1_ppe_entry_p2e_get(int unit, int iport, soc_sbx_t3p1_p2e_t *e);
extern int soc_sbx_t3p1_ppe_entry_p2e_set(int unit, int iport, soc_sbx_t3p1_p2e_t *e);
extern int soc_sbx_t3p1_ppe_entry_ep2e_set(int unit, int iport, soc_sbx_t3p1_ep2e_t *e);
extern int soc_sbx_t3p1_ppe_entry_ep2e_get(int unit, int iport, soc_sbx_t3p1_ep2e_t *e);

extern uint32 soc_sbx_t3p1_util_crc32_word(uint32 x);
extern int soc_sbx_t3p1_util_register_timer_callback(int unit, soc_sbx_t3p1_util_timer_event_callback_f cb, void *user_cookie);

#endif /* BCM_CALADAN3_SUPPORT */
#endif
