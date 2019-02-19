/*
 * $Id: mem.h,v 1.2 Broadcom SDK $
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
 * Arm Processor Subsystem remote management library
 */

#ifndef SOC_EA_COMMON_MEM_H
#define SOC_EA_COMMON_MEM_H

#include <sal/types.h>


#define SOC_EA_MEM_E_SUCCESS            0x00000000
#define SOC_EA_MEM_E_INVALID_PARAMETER  0x80000000
#define SOC_EA_MEM_E_OUT_OF_SPACE       0x80000001
#define SOC_EA_MEM_E_NO_MEM             0x80000002
#define SOC_EA_MEM_E_POOL_NOT_EMPTY     0x80000003
#define SOC_EA_MEM_E_INTERNAL_ERROR     0x80000004
#define SOC_EA_MEM_E_DUPLICATE_OP       0x80000005


typedef struct soc_ea_mem_cfg_entry_s {
    uint32      blk_sz;				/* The size of per block*/
    uint32      blk_num;			/* How many blocks do you want to apply?*/
} soc_ea_mem_cfg_entry_t;


/* Upper layer provides this information to create an EA static memory pool */
typedef struct soc_ea_mem_cfg_s {
    uint32  	n_entry;				/* How many entries do we have? */
    soc_ea_mem_cfg_entry_t  *entries;	/* Entry tables */
} soc_ea_mem_cfg_t;



typedef struct soc_ea_mem_blk_chain_s {
    struct soc_ea_mem_blk_chain_s    * head;   	/* head of the chain */
    struct soc_ea_mem_blk_chain_s    * tail;   	/* tail of the chain */
    struct soc_ea_mem_blk_chain_s    * next;	
    uint32          buf_sz;						
    uint8         * buf;
    uint8         * wr_ptr;
    uint8         * rd_ptr;
    uint32          idx;
} soc_ea_mem_blk_chain_t;

extern void soc_ea_mem_init(void);
extern void  * soc_ea_mem_pool_create (soc_ea_mem_cfg_t * mem_cfg, char * pool_name, uint32 * err_code);
extern uint32  soc_ea_mem_pool_free (void * mem_pool);
extern uint8 * soc_ea_mem_blk_alloc (void * mem_pool, uint32 sz, uint32 *err_code);
extern uint32  soc_ea_mem_blk_free (uint8 * buf);

extern soc_ea_mem_blk_chain_t * soc_ea_mem_blk_chain_alloc (void * mem_pool, uint32 sz, uint32 *err_code);
extern soc_ea_mem_blk_chain_t * soc_ea_mem_blk_chain_append (
	soc_ea_mem_blk_chain_t * chain, soc_ea_mem_blk_chain_t * new_entry, uint32 * err_code);
extern uint32 soc_ea_mem_blk_chain_free (soc_ea_mem_blk_chain_t * chain);

#ifdef BROADCOM_DEBUG
extern void soc_ea_mem_dump(void);
#endif

#endif /* SOC_EA_COMMON_MEM_H */

