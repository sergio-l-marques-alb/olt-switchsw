/*
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
 * $Id: alpm.c$
 * File:    alpm_common.c
 * Purpose: ALPM common code.
 * Requires:
 */

/* Implementation notes:
 */
#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>
#include <soc/apache.h>
#include <shared/bsl.h>

#ifdef ALPM_ENABLE

#include <shared/util.h>
#include <shared/l3.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#include <soc/esw/trie.h>

/* returns 0 for Combined Search mode and 1 for Parallel Search mode,
 * 2 for TCAM/ALPM mode, equals to L3_DEFIP_RPF_CONTROLr.LOOKUP_MODEf value
 */
int
soc_alpm_cmn_mode_get(int unit)
{
    int mode = 0;

    /* 'mode' here equals to l3_alpm_enable config property value */
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        mode = soc_apache_alpm_mode_get(unit);
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWK(unit)) {
        mode = soc_tomahawk_alpm_mode_get(unit);
    }
#endif
    switch (mode) {
    case 1:     /* Parallel mode */
        return SOC_ALPM_MODE_PARALLEL;
    case 3:     /* TCAM/ALPM mode */
        return SOC_ALPM_MODE_TCAM_ALPM;
    case 2:
    default:    /* Combined mode */
        return SOC_ALPM_MODE_COMBINED;
    }

    return SOC_ALPM_MODE_COMBINED;
}

int soc_alpm_cmn_banks_get(int unit)
{
    int banks = 0;
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        banks = soc_apache_get_alpm_banks(unit);
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWK(unit)) {
        banks = soc_th_get_alpm_banks(unit);
    }
#endif

    return banks;
}

#endif /* ALPM_ENABLE */

void soc_alpm_cmn_bkt_view_set(int unit, int index, soc_mem_t view)
{
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        _soc_apache_alpm_bkt_view_set(unit, index, view);
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWK(unit)) {
        _soc_tomahawk_alpm_bkt_view_set(unit, index, view);
    }
#endif
}

soc_mem_t soc_alpm_cmn_bkt_view_get(int unit, int index)
{
    soc_mem_t mem = INVALIDm;
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        mem = _soc_apache_alpm_bkt_view_get(unit, index);
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWK(unit)) {
        mem = _soc_tomahawk_alpm_bkt_view_get(unit, index);
    }
#endif

    return mem;
}

int
soc_alpm_l3_defip_index_map(int unit, soc_mem_t mem, int index)
{
    return soc_trident2_l3_defip_index_map(unit, mem, index);
}

int
soc_alpm_l3_defip_index_remap(int unit, soc_mem_t mem, int index)
{
    return soc_trident2_l3_defip_index_remap(unit, mem, index);
}

