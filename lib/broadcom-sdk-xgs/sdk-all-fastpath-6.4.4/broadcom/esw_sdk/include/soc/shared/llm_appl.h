/* 
 * $Id: llm_appl.h,v 1.10 Broadcom SDK $
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
 * File:    llm_appl.h
 * Purpose: llm application definitions common to SDK and uKernel.
 *
 *          Messages between SDK and uKernel.
 * 
 */

#ifndef   _SOC_SHARED_LLM_APPL_H_
#define   _SOC_SHARED_LLM_APPL_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <soc/defs.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>

#define LLM_MSG_LOCK(unit)        \
    sal_mutex_take(llm_appl_info[unit].llm_mutex, sal_mutex_FOREVER)
#define LLM_MSG_UNLOCK(unit)      \
    sal_mutex_give(llm_appl_info[unit].llm_mutex)
#define LLM_TUNNEL_LOCK(unit)        \
      sal_mutex_take(llm_appl_info[unit].l2_mutex, sal_mutex_FOREVER)
#define LLM_TUNNEL_UNLOCK(unit)      \
     sal_mutex_give(llm_appl_info[unit].l2_mutex)


typedef struct llm_appl_info_s
{
    int rx_channel;             /* Local RX DMA channel for BHH packets */
    int uc_num;                 /* uController number running BHH appl */
    int dma_buffer_len;         /* DMA max buffer size */
    uint8* dma_buffer;          /* DMA buffer */
    uint8* dmabuf_reply;        /* DMA reply buffer */
    sal_thread_t event_thread_id;  /* Event handler thread id */
    int event_thread_kill; /* Whether to stop event thread */
    uint32 event_enable;      /* Event enable/disable */
    sal_mutex_t  llm_mutex; /* mutex for LLM message */
    sal_mutex_t  l2_mutex;      /* arm restore */

} llm_appl_info_t;

extern llm_appl_info_t llm_appl_info[SOC_MAX_NUM_DEVICES];
extern llm_appl_info_t * shr_llm_appl_info(int unit);
extern int shr_llm_appl_init(int unit);
extern int shr_llm_appl_deinit(int unit);

#endif

