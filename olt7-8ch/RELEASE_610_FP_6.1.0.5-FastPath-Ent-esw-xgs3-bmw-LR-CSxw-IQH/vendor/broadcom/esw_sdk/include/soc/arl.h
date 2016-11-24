/*
 * $Id: arl.h,v 1.1 2011/04/18 17:10:57 mruas Exp $
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
 * File: 	arl.h
 * Purpose: 	Defines structures and routines for ARL operations
 *              defined in:
 *              drv/arl.c      HW table management
 *              drv/arlmsg.c   ARL message handling
 */

#ifndef _SOC_ARL_H
#define _SOC_ARL_H

#include <shared/avl.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/robo/memregs.h>
extern int soc_arl_attach(int unit);
extern int soc_arl_detach(int unit);
extern int soc_arl_init(int unit);

typedef void (*soc_robo_arl_cb_fn)(int unit,
                              l2_arl_sw_entry_t *entry_del,
                              l2_arl_sw_entry_t *entry_add,
                              void *fn_data);
extern int soc_robo_arl_register(int unit, soc_robo_arl_cb_fn fn, 
                                    void *fn_data);
extern int soc_robo_arl_unregister(int unit, soc_robo_arl_cb_fn fn, 
                                    void *fn_data);

#define soc_robo_arl_unregister_all(unit) \
	soc_robo_arl_unregister((unit), NULL, NULL)

extern void soc_robo_arl_callback(int unit,
                             l2_arl_sw_entry_t *entry_del,
                             l2_arl_sw_entry_t *entry_add);
/*
 * ARL miscellaneous functions
 */

#define ARL_MODE_NONE		0
#define ARL_MODE_POLL_MBUF	1	/* Monitor dropped messages only */
#define ARL_MODE_INTR_MBUF	2
#define ARL_MODE_INTR_DMA	3
#define ARL_MODE_ROBO_POLL	4
#define ARL_ROBO_POLL_INTERVAL	3000000

extern int soc_arl_mode_set(int unit, int mode);
extern int soc_arl_mode_get(int unit, int *mode);

extern int soc_robo_arl_mode_set(int unit, int mode);
extern int soc_robo_arl_mode_get(int unit, int *mode);

extern int soc_arl_freeze(int unit);
extern int soc_arl_thaw(int unit);
extern int soc_robo_arl_freeze(int unit);
extern int soc_robo_arl_thaw(int unit);
extern int soc_arl_frozen_cml_set(int unit, soc_port_t port, int cml,
				  int *repl_cml);
extern int soc_arl_frozen_cml_get(int unit, soc_port_t port, int *cml);
extern void _drv_arl_hash(uint8 *hash_value, uint8 length, uint16 *hash_result);
/*
 * For ARL software shadow database access
 */
extern int soc_arl_database_dump(int unit, uint32 index, 
                                     l2_arl_sw_entry_t *entry);
extern int soc_arl_database_delete(int unit, uint32 index);

#define ARL_TABLE_WRITE 0  /* For ARL Write operateion */
#define ARL_TABLE_READ 1   /* For ARL Read operateion */
#define ARL_ENTRY_NULL(e1)\
    ((e1)->entry_data[0] == 0 && \
     (e1)->entry_data[1] == 0 && \
     (e1)->entry_data[2] == 0)
 

#endif	/* !_SOC_ARL_H */
