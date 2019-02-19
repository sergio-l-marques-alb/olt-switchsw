/*
 * $Id: arad_trunk.c,v 1.0 Broadcom SDK $
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
 * File:    arad_trunk.c
 * Purpose: SOC level functinos for Link Aggregation (a.k.a Trunking)
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TRUNK

#include <soc/drv.h>
#include <soc/register.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_trunk.h>

/*
 * Function:
 *      soc_arad_trunk_resolve_ingress_mc_destination_method
 * Purpose:
 *      allow/forbid adding trunk as ingress mc destination - according to soc property USE_TRUNK_AS_INGRESS_MC_DESTINATION.
 *      default value is 0 (use trunk as ingress mc destination). this behavior is possible only from AradPlus and onwards.
 * Parameters:
 *      unit    - Device Number 
 * Returns:
 *      SOC_E_XXX 
 */
int soc_arad_trunk_resolve_ingress_mc_destination_method(int unit, int8 use_trunk_as_ingress_mc_dest)
{
    int value;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS(unit) && !SOC_IS_ARDON(unit))
    {
        /* value of 0 will enable the fix that allows use if trunk as ingress mc destination */
        value = (use_trunk_as_ingress_mc_dest == 1) ? 0 : 1;
        SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, REG_PORT_ANY, CFG_BUG_FIX_86_DISABLEf, value));
    } else if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        /* in order to allow trunk as ingress mc destination in arad need to disable LAG_FILTER_MC, which means in effect disabling trunk as egress mc destination */
        value = (use_trunk_as_ingress_mc_dest == 1) ? 0 : 1;
        SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, EGQ_LAG_FILTER_ENABLEr, REG_PORT_ANY, ENABLE_LAG_FILTER_MCf, value));
    }
exit:
    SOCDNX_FUNC_RETURN;
}



#define _ERR_MSG_MODULE_NAME BSL_SOC_TRUNK
