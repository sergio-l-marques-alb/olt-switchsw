/*
 * $Id: ukernel_debug.h,v 0.1 2014/10/11 samaresm Exp $
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
 * File:    ukernel_debug.h
 * Purpose:  
 */

#ifndef _UC_DBG_H
#define _UC_DBG_H

#include <bcm/types.h>
#include <bcm/error.h>
#include <soc/shared/mos_msg_common.h>

#if defined(SOC_UKERNEL_DEBUG)
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)

#define CMIC_UC_DBG_PTP_CORE_COMM_SUCCESS       (1<<0)
#define CMIC_UC_DBG_PTP_STACK_INIT_SUCCESS      (1<<1)

#define CMIC_UCDEBUG_OUTPUT_DISP_TIMEOUT        (1000)

typedef struct _soc_cmic_ucdbg_stats_s {
    uint32                  flags;
    uint32                  sw_intr_count_uc0;
    uint32                  sw_intr_count_uc1;
    uint32                  hostlog_indx_wr;
    uint32                  hostlog_indx_rd;
    uint32                  uclog_indx_rd;
    mos_msg_cmic_ucdbg_t    ucdbg_cache; /* memory cache of SRAM data struct */
    mos_msg_ucdbg_entry_t   hostlog[MOS_UCDBG_MAX_LOG_ENTRIES_HOST]; /* host log buff pointer */
    mos_msg_ucdbg_entry_t   *puclog[MOS_UCDBG_MAX_LOG_ENTRIES_UC]; /* uc log buffer pointer */
}_soc_cmic_ucdbg_stats_t;

extern int 
soc_cmic_ucdebug_init(int unit, int uC);

extern int 
soc_cmic_ucdebug_sw_intr_count_inc(int unit, uint32 rupt_num);


extern int 
soc_cmic_ucdebug_core_communication_status(int unit, uint32 flag);

extern int 
soc_cmic_ucdebug_dump_start(int unit);

extern int 
soc_cmic_ucdebug_dump_stop(int unit);

extern int 
soc_cmic_ucdebug_status(int unit);

extern int
soc_cmic_ucdebug_log_add(int unit, mos_msg_ucdbg_logtype_t logtype,
                     const char *format, ...);

#define SOC_CMIC_UCDBG_LOG_ADD(stuff_) \
    soc_cmic_ucdebug_log_add stuff_;

#else

#define soc_cmic_ucdebug_init(unit, uC)
#define soc_cmic_ucdebug_sw_intr_count_inc(unit, rupt_num)
#define soc_cmic_ucdebug_core_communication_status(unit, flag)
#define soc_cmic_ucdebug_dump_start(unit)
#define soc_cmic_ucdebug_dump_stop(unit)
#define soc_cmic_ucdebug_status(unit)

#define SOC_CMIC_UCDBG_LOG_ADD(stuff_)

#endif

#else

#define soc_cmic_ucdebug_init(unit, uC)
#define soc_cmic_ucdebug_sw_intr_count_inc(unit, rupt_num)
#define soc_cmic_ucdebug_core_communication_status(unit, flag)
#define soc_cmic_ucdebug_dump_start(unit)
#define soc_cmic_ucdebug_dump_stop(unit)
#define soc_cmic_ucdebug_status(unit)

#define SOC_CMIC_UCDBG_LOG_ADD(stuff_)

#endif

#endif /*_UKERNEL_DEBUG_H_*/
