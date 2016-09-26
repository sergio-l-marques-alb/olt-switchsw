/*
 * $Id: ccmdma_internal.h,v 1.0 Broadcom SDK $
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
 * File:        ccmdma_internal.h
 * Purpose:     Maps out structures used for CCM DMA operations and
 *              internal routines.
 */

#ifndef _SOC_CCMDMA_INTERNAL_H
#define _SOC_CCMDMA_INTERNAL_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <soc/ccmdma.h>

#define CMIC_CMCx_CCMDMA_CHSELECT_TIMEOUT    (1000)


/* CCM DMA function pointers */
typedef struct soc_ccmdma_drv_s {
    int (*soc_ccmdma_ch_get)(int unit, int *vchan);
    int (*soc_ccmdma_ch_put)(int unit, int vchan);
    int (*soc_ccmdma_copy)(int unit, sal_paddr_t *srcbuf, sal_paddr_t *dstbuf,
                           unsigned int src_is_internal, unsigned int dst_is_internal,
                           int count, int flags, int vchan);
    int (*soc_ccmdma_abort)(int unit, int vchan);
    int (*soc_ccmdma_clean)(int unit, int max_cmc);
}soc_ccmdma_drv_t;

/*******************************************
* @function cmicm_ccmdma_init
* purpose API to Initialize cmicm CCM DMA
*
* @param unit [in] unit
* @param drv [out] soc_ccmdma_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
extern int cmicm_ccmdma_init(int unit, soc_ccmdma_drv_t *drv);

/*******************************************
* @function cmicm_ccmdma_deinit
* purpose API to Deinitialize cmicm CCM DMA
*
* @param unit [in] unit
* @param drv [in] soc_ccmdma_drv_t pointer
*
* @returns SOC_E_NONE

*
* @end
********************************************/
extern int cmicm_ccmdma_deinit(int unit, soc_ccmdma_drv_t *drv);

#endif /* !_SOC_CCMDMA_INTERNAL_H */
