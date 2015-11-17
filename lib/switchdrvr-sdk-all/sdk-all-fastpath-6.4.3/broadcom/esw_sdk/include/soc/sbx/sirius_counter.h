/* 
 * $Id: sirius_counter.h,v 1.12 Broadcom SDK $
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
 *
 * File:        sirius_counter.h
 * Purpose:     SBX Software Counter Collection module definitions for SIRIUS.
 */

#ifndef   _SOC_SBX_SIRIUS_COUNTER_H_
#define   _SOC_SBX_SIRIUS_COUNTER_H_

#include <soc/sbx/counter.h>



extern int soc_sbx_sirius_counter_init(int unit, 
				       uint32 flags, 
				       int interval,
				       pbmp_t pbmp);
extern int soc_sbx_sirius_process_slq_stats(int unit);
extern int soc_sbx_sirius_process_global_stats(int unit);
extern int soc_sbx_sirius_process_fd_drop_stats(int unit, int clear);
extern int soc_sbx_sirius_get_segment(uint32 unit, 
				      uint8 cu,
				      int32 *rSegment);
extern int soc_sbx_sirius_free_segment(uint32 unit, 
				       uint32 segment);
extern int soc_sbx_sirius_create_group(uint32 unit, 
					 uint32 segment, 
					 uint32 cu_num, 
					 uint32 cntrId, 
					 uint32 size);
extern int soc_sbx_sirius_remove_group(uint32 unit, 
				       uint32 segment, 
				       uint32 cu_num, 
				       uint32 cntrId, 
				       uint32 size);
extern int soc_sbx_sirius_provision_group(uint32 unit, 
					    uint32 segment, 
					    uint64 *ullCntAddr);
extern int soc_sbx_sirius_init_group(uint32 unit, 
				     uint32 segment, 
				     uint32 cntr_id, 
				     uint32 num_cntrs);
extern int soc_sbx_sirius_flush_segment(uint32 unit, 
					uint32 segment, 
					uint32 cntrId, 
					uint32 num_cntrs);

#define    SIRIUS_COUNTER_QM_COUNT 32

int soc_sbx_sirius_test(int unit);

extern uint32 soc_sirius_qm_counter_base_set(int unit,
					     int32 nBaseQueue,
					     int32 enable);

extern uint32 soc_sirius_qm_counter_base_get(int unit, int32 *nBaseQueue);

extern uint32 soc_sirius_qm_counter_read(int unit, int32 set, uint32 *puCounterBase);

extern int soc_sbx_sirius_counter_port_get(int unit, int port, int block, int set,
                                           int counter, uint64 *val);

extern int soc_sbx_sirius_counter_port_set(int unit, int port, int block , int set,
                                           int counter, uint64 val);

extern int soc_sbx_sirius_counter_bset_collect( int unit, int discard, int block, int set);

extern int soc_sirius_counter_enable_get(int unit, int *base, int *result);

extern int soc_sirius_counter_enable_set(int unit, int base );

extern int soc_sirius_counter_enable_clear(int unit);

#endif /* _SOC_SBX_SIRIUS_COUNTER_H_ */
