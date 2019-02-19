/*
 * $Id: enduro.h,v 1.1 2011/04/18 17:10:57 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        enduro.h
 */

#ifndef _SOC_ENDURO_H_
#define _SOC_ENDURO_H_

#include <soc/drv.h>
#include <shared/sram.h>

typedef int (*soc_enduro_oam_handler_t)(int unit, soc_field_t fault_field);

typedef _shr_ext_sram_entry_t en_ext_sram_entry_t;
 
extern int soc_enduro_misc_init(int);
extern int soc_enduro_mmu_init(int);
extern int soc_enduro_age_timer_get(int, int *, int *);
extern int soc_enduro_age_timer_max_get(int, int *);
extern int soc_enduro_age_timer_set(int, int, int);
extern void soc_enduro_oam_handler_register(soc_enduro_oam_handler_t handler);
#if 0 
extern void soc_enduro_mem_config(int unit);
#endif
extern void soc_enduro_parity_error(void *unit_vp, void *d1, void *d2,
                                     void *d3, void *d4);
extern int _soc_enduro_mem_parity_control(int unit, soc_mem_t mem,
                                            int copyno, int enable);                                     
#if 0 
extern int soc_enduro_esm_intr_status(int unit);
extern int soc_enduro_esm_init_read_config(int unit);
extern int soc_enduro_esm_init_set_tcam_freq(int unit, int freq);
extern int soc_enduro_esm_init_set_sram_freq(int unit, int freq);
#endif
#if 0 
extern int soc_en_tcam_access(int unit, int op_type, int num_inst,
                              int num_pre_nop, int num_post_nop,
                              uint32 *dbus, int *ibus);
#endif
#if 0 
extern int soc_enduro_ext_sram_enable_set(int unit, soc_reg_t mode,
                                           int enable, int clr);
extern int soc_enduro_ext_sram_op(int unit, soc_reg_t mode,
                                   en_ext_sram_entry_t *entry);
extern int soc_enduro_l2x_to_ext_l2(int unit, l2x_entry_t *l2x_entry,
                                     ext_l2_entry_entry_t *ext_l2_entry);
extern int soc_enduro_ext_l2_to_l2x(int unit,
                                     ext_l2_entry_entry_t *ext_l2_entry,
                                     l2x_entry_t *l2x_entry);
#endif
extern int soc_en_xqport_mode_change(int unit, soc_port_t port, 
                                      soc_mac_mode_t mode);
extern soc_functions_t soc_enduro_drv_funs;
#endif	/* !_SOC_ENDURO_H_ */
