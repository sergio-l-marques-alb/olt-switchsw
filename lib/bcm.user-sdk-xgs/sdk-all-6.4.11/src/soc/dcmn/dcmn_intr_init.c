
/*
 * $Id: soc_dcmn_intr_handler.c, v1 16/06/2014 09:55:39 azarrin $
 *
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
 * Purpose:    Implement soc interrupt handler.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>

#include <soc/intr.h>
#include <soc/ipoll.h>
#include <soc/dpp/JER/jer_intr_cb_func.h>
#include <soc/dpp/JER/jer_intr_corr_act_func.h>
#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JER/jer_defs.h>

#include <soc/dcmn/dcmn_intr_handler.h>
#include <soc/dcmn/error.h>
#include <soc/register.h>

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

/*************
 * DECLARATIONS *
 *************/

/*************
 * FUNCTIONS *
 *************/
STATIC 
int
soc_dcmn_ser_init_cb(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
    soc_reg_t reg = ainfo->reg;
    char *reg_name = SOC_REG_NAME(unit,reg);
    int rc;
    int inst=0;
    int blk;
    soc_reg_above_64_val_t above_64;
    SOCDNX_INIT_FUNC_DEFS;
    
    if(sal_strstr(reg_name, "MEM_MASK") == NULL ||  sal_strstr(reg_name, "MESH_TOPOLOGY_ECC_ERR") != NULL)
        SOC_EXIT;

    /* unmask SER monitor registers*/
    SOC_REG_ABOVE_64_ALLONES(above_64);

    /*exception - PACKET_CRC ecc ser protection should not be enabled according HW bug */
    if (reg == IDR_ECC_ERR_2B_MONITOR_MEM_MASKr) {
        soc_reg_above_64_field32_set(unit, reg, above_64, PACKET_CRC_ECC_2B_ERR_MASKf, 0); 
    }
    if (reg == IDR_ECC_ERR_1B_MONITOR_MEM_MASKr) {
        soc_reg_above_64_field32_set(unit, reg, above_64, PACKET_CRC_ECC_1B_ERR_MASKf, 0); 
    }

    if (SOC_IS_JERICHO(unit)) {
        /* This memories couse ECI.Ecc2bErrInt */
        /* They are debug memories so its ok to mask them */
        if (reg == ECI_ECC_ERR_1B_MONITOR_MEM_MASKr) {
            soc_reg_above_64_field32_set(unit, reg, above_64, FIELD_0_0f , 0); 
        }
        if (reg == ECI_ECC_ERR_2B_MONITOR_MEM_MASKr) {
            soc_reg_above_64_field32_set(unit, reg, above_64, FIELD_0_0f , 0); 
        }

        /* exception - ASIC bug that cause the IPT.LargeLatencyPktDrop interrupt to jump before we have trafic. */
        /* if trafic started, its ok to turn on the feature */
        if (reg == IPT_PACKET_LATENCY_MEASURE_CFGr) {
            soc_reg_above_64_field32_set(unit, reg, above_64, EN_LATENCY_DROPf , 0); 
        }

        /*exception - MMU_LBM ecc ser protection should not be enabled according HW bug */
        if (reg == MMU_ECC_ERR_1B_MONITOR_MEM_MASK_1r) {
            soc_reg_above_64_field32_set(unit, reg, above_64, LBM_ECC_1B_ERR_MASKf, 0); 
        }
        if (reg == MMU_ECC_ERR_2B_MONITOR_MEM_MASK_1r) {
            soc_reg_above_64_field32_set(unit, reg, above_64, LBM_ECC_2B_ERR_MASKf, 0); 
        }
    }

    SOC_BLOCK_ITER(unit, blk, SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, reg).block)) {
        rc = soc_reg_above_64_set(unit, reg, inst, 0, above_64);
        SOCDNX_IF_ERR_EXIT(rc);
        inst++;
    }
exit:
    SOCDNX_FUNC_RETURN;

}

int
soc_dcmn_ser_init(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_reg_iterate(unit, soc_dcmn_ser_init_cb, NULL));


exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
