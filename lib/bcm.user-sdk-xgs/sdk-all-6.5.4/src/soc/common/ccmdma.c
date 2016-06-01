/*
 * $Id: ccmdma.c,v 1.0 Broadcom SDK $
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
 * Purpose: SOC CCM DMA driver.
 *
 */

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/ccmdma_internal.h>

#ifdef BCM_CALADAN3_SUPPORT
#include <soc/sbx/caladan3.h>
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_CALADAN3_SUPPORT) || \
    defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#ifdef BCM_CCMDMA_SUPPORT

#if (!defined(_LIMITS_H)) && !defined(_LIBC_LIMITS_H_)

#if (!defined(INT_MIN)) && !defined(INT_MAX)
#define INT_MIN (((int)1)<<(sizeof(int)*8-1))
#define INT_MAX (~INT_MIN)
#endif

#ifndef UINT_MAX
#define UINT_MAX ((unsigned)-1)
#endif

#endif

STATIC soc_ccmdma_drv_t _ccmdma_drv[SOC_MAX_NUM_DEVICES];

/*******************************************
* @function soc_ccmdma_ch_get
* purpose get idle DMA channel
*
* @param unit [in] unit #
* @param vchan [out] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
********************************************/
extern int
soc_ccmdma_ch_get(int unit, int *vchan)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_ch_get != NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_ch_get(unit, vchan);
    }

    return rv;
}

/*******************************************
* @function soc_ccmdma_ch_put
* purpose put back the channel on free list
*
* @param ch [in] channel number
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
********************************************/
extern int
soc_ccmdma_ch_put(int unit, int vchan)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_ch_put != NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_ch_put(unit, vchan);
    }

    return rv;
}

/*******************************************
* @function soc_ccmdma_copy
* purpose to initiate DMA from source to
*            destination memory on vchan
* @param srcbuf [in] pointer to source memory
* @param dstbuf [in] pointer to dest memory
* @param count [in] number of bytes to xfer
* @param flags [in]
* @param vchan [in] channel number
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
********************************************/
int
soc_ccmdma_copy(int unit,  unsigned int *srcbuf, unsigned int *dstbuf,
                unsigned int src_is_internal, unsigned int dst_is_internal,
                int count, int flags, int vchan)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_copy != NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_copy(unit, srcbuf, dstbuf,
                                               src_is_internal, dst_is_internal,
                                               count, flags, vchan);
    }

    return rv;
}

/*******************************************
* @function soc_ccmdma_abort
* purpose to abort active DMA operation on
*            given channel
* @param vchan [in] channel number
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
********************************************/
extern int
soc_ccmdma_abort(int unit, int vchan)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_abort != NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_abort(unit, vchan);
    }

    return rv;
}

/*******************************************
* @function soc_ccmdma_clean
* purpose to clean ccmdma
* @param max_cmx [in] max cmc
*
* @returns SOC_E_XXXX
* @returns SOC_E_NONE
*
* @end
********************************************/
extern int
soc_ccmdma_clean(int unit, int max_cmc)
{
    int rv = SOC_E_UNAVAIL;

    if (_ccmdma_drv[unit].soc_ccmdma_clean != NULL) {
        rv = _ccmdma_drv[unit].soc_ccmdma_clean(unit, max_cmc);
    }

    return rv;
}

/*******************************************
* @function soc_ccmdma_init
* purpose API to Initialize CCM DMA
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
int soc_ccmdma_init(int unit)
{
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {

        SOC_IF_ERROR_RETURN(cmicm_ccmdma_init(unit,
                            &_ccmdma_drv[unit]));
    }
#endif
    return SOC_E_NONE;
}

/*******************************************
* @function soc_ccmdma_deinit
* purpose API to Deinitialize CCM DMA
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
*
* @end
********************************************/
int soc_ccmdma_deinit(int unit)
{
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {

        SOC_IF_ERROR_RETURN(cmicm_ccmdma_deinit(unit,
                            &_ccmdma_drv[unit]));
    }
#endif
    return SOC_E_NONE;
}

#endif /* BCM_CCMDMA_SUPPORT */
#endif /* BCM_ESW_SUPPORT */
