/*
* $Id: instr.h,v 1.0 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * File:        instrumentation.h
 */

#ifndef _BCM_INT_INSTRUMENTATION_H
#define _BCM_INT_INSTRUMENTATION_H

#include <soc/defs.h>

#ifdef BCM_INSTRUMENTATION_SUPPORT

#include <bcm/types.h>
#include <bcm/switch.h>


/* packet trace feature support */
extern int bcm_esw_pkt_trace_init(int unit);
extern int _bcm_esw_pkt_trace_info_get(int unit, uint32 options, uint8 port, 
                                       int len, uint8 *data, 
                                       bcm_switch_pkt_trace_info_t 
                                                            *pkt_trace_info);

extern int _bcm_esw_pkt_trace_src_port_set(int unit, uint32 logical_src_port);
extern int _bcm_esw_pkt_trace_src_port_get(int unit);
extern int _bcm_esw_pkt_trace_src_pipe_get(int unit);
extern int _bcm_esw_pkt_trace_hw_reset(int unit);
extern int _bcm_esw_pkt_trace_cpu_profile_get(int unit, uint32* profile_id);
typedef struct _bcm_switch_pkt_trace_port_info_s {
    uint32 pkt_trace_src_logical_port;
    uint8  pkt_trace_src_pipe;

} _bcm_switch_pkt_trace_port_info_t;


/* for tomahawk */
/* size is in number of double words */
#define TH_PTR_RESULTS_IVP_MAX_INDEX  2 
#define TH_PTR_RESULTS_ISW1_MAX_INDEX 4
#define TH_PTR_RESULTS_ISW2_MAX_INDEX 8
extern int _bcm_th_pkt_trace_info_get(int unit,
                                      bcm_switch_pkt_trace_info_t *pkt_trace_info);
extern int _bcm_th_pkt_trace_src_port_set(int unit, uint32 logical_src_port);
extern int _bcm_th_pkt_trace_src_port_get(int unit);
extern int _bcm_th_pkt_trace_src_pipe_get(int unit);
extern int _bcm_th_pkt_trace_hw_reset(int unit);
extern int _bcm_th_pkt_trace_cpu_profile_init(int unit);
extern int _bcm_th_pkt_trace_cpu_profile_set(int unit,uint32 options);
extern int _bcm_th_pkt_trace_cpu_profile_get(int unit,uint32* profile_id);
extern int _bcm_th_pkt_trace_int_lbport_set(int unit, uint8 port, int enable);

/* end of packet trace feature support */
#endif /* BCM_INSTRUMENTATION_SUPPORT */

#endif
