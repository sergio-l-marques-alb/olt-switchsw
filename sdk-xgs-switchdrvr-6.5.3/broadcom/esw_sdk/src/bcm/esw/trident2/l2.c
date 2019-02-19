/*
 * $Id: l2.c,v 1.297 Broadcom SDK $
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
 * File:        l2.c
 * Purpose:     Trident2 L2 function implementations
 */

#include <shared/bsl.h>
#include <soc/defs.h>

#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/drv.h>
#include <soc/l2x.h>

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trident2.h>

/*
 * Function:
 *      bcm_td2_l2_init
 * Description:
 *      Initialize chip-dependent parts of L2 module
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

int
bcm_td2_l2_init(int unit)
{
    BCM_IF_ERROR_RETURN(bcm_tr_l2_init(unit));

    if (SOC_CONTROL(unit)->l2x_age_interval != 0) {
        SOC_IF_ERROR_RETURN(soc_td2_l2_bulk_age_stop(unit));
    }

    if (soc_property_get(unit, spn_RUN_L2_SW_AGING,
                         soc_feature(unit, soc_feature_l2_hw_aging_bug))) {
        SOC_IF_ERROR_RETURN(soc_td2_l2_bulk_age_start(unit, 0));
        SOC_CONTROL(unit)->l2x_sw_aging = 1;
    }

    return BCM_E_NONE;
}

#endif /* BCM_TRIDENT2_SUPORT */
