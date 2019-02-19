/*
 * $Id: mbcm.c,v 1.4 Broadcom SDK $
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
 * File:        mbcm.c
 * Purpose:     Implementation of bcm multiplexing
 */


#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/mbcm.h>

CONST mbcm_dpp_functions_t    *mbcm_dpp_driver[BCM_MAX_NUM_UNITS]={0};
soc_dpp_chip_family_t    mbcm_dpp_family[BCM_MAX_NUM_UNITS];

/****************************************************************
 *
 * Function:        mbcm_dpp_init
 * Parameters:      unit   --   unit to setup
 * 
 * Initialize the mbcm driver for the indicated unit.
 *
 ****************************************************************/
int
mbcm_dpp_init(int unit)
{
#ifdef  BCM_ARAD_SUPPORT 
    if (SOC_IS_ARAD(unit)) {        
#ifdef BCM_QAX_SUPPORT
        if (SOC_IS_QAX(unit)) {
            mbcm_dpp_driver[unit] = &mbcm_qax_driver;
            mbcm_dpp_family[unit] = BCM_FAMILY_QAX;
        } else
#endif
#ifdef BCM_JERICHO_PLUS_SUPPORT
        if (SOC_IS_JERICHO_PLUS(unit)) {
            mbcm_dpp_driver[unit] = &mbcm_jericho_driver;
            mbcm_dpp_family[unit] = BCM_FAMILY_JERICHO;
        } else
#endif /* BCM_JERICHO_PLUS_SUPPORT */
#ifdef BCM_JERICHO_SUPPORT
        if (SOC_IS_JERICHO(unit)) {
            mbcm_dpp_driver[unit] = &mbcm_jericho_driver;
            mbcm_dpp_family[unit] = BCM_FAMILY_JERICHO;
        } else
#endif /* BCM_JERICHO_SUPPORT */
        {
            mbcm_dpp_driver[unit] = &mbcm_arad_driver;
            mbcm_dpp_family[unit] = BCM_FAMILY_ARAD;
        }
        return SOC_E_NONE;
    }
#endif  /* BCM_ARAD_SUPPORT */

    LOG_INFO(BSL_LS_BCM_INIT,
             (BSL_META_U(unit,
                         "ERROR: mbcm_dpp_init unit %d: unsupported chip type\n"), unit));
    return SOC_E_INTERNAL;
}
