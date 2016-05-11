/*
 * $Id: diag_mmu.c,v 1.0 Broadcom SDK $
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
 * File:    diag_mmu.c
 * Purpose: Manages mmu diagnostics functions 
 */


#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <appl/diag/diag_mmu.h>
#include <appl/diag/diag.h>

#ifdef BCM_DPP_SUPPORT
#include <bcm_int/dpp/utils.h>
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>
#endif /* BCM_DPP_SUPPORT */
#include <soc/mcm/memregs.h>

#define DIAG_MMU_NOF_DRC_TYPE_COUNTERS 8

/*
 * Functions
 */

/*
 * Print diag mmu counters
 */
cmd_result_t cmd_dpp_diag_mmu_counters(int unit, args_t *a) {

    soc_reg_t mmu_drc_rd_byte_counters_regs[] = { MMU_DRCA_RD_BYTE_COUNTERr, MMU_DRCB_RD_BYTE_COUNTERr, MMU_DRCC_RD_BYTE_COUNTERr, MMU_DRCD_RD_BYTE_COUNTERr, 
                                                  MMU_DRCE_RD_BYTE_COUNTERr, MMU_DRCF_RD_BYTE_COUNTERr, MMU_DRCG_RD_BYTE_COUNTERr, MMU_DRCH_RD_BYTE_COUNTERr  };

    soc_field_t mmu_drc_rd_byte_counters_fields[] = { DRCA_RD_BYTE_COUNTERf, DRCB_RD_BYTE_COUNTERf, DRCC_RD_BYTE_COUNTERf, DRCD_RD_BYTE_COUNTERf, 
                                                      DRCE_RD_BYTE_COUNTERf, DRCF_RD_BYTE_COUNTERf, DRCG_RD_BYTE_COUNTERf, DRCH_RD_BYTE_COUNTERf  };

    soc_reg_t mmu_drc_wr_byte_counters_regs[] = { MMU_DRCA_WR_BYTE_COUNTERr, MMU_DRCB_WR_BYTE_COUNTERr, MMU_DRCC_WR_BYTE_COUNTERr, MMU_DRCD_WR_BYTE_COUNTERr, 
                                                  MMU_DRCE_WR_BYTE_COUNTERr, MMU_DRCF_WR_BYTE_COUNTERr, MMU_DRCG_WR_BYTE_COUNTERr, MMU_DRCH_WR_BYTE_COUNTERr  };

    soc_field_t mmu_drc_wr_byte_counters_fields[] = { DRCA_WR_BYTE_COUNTERf, DRCB_WR_BYTE_COUNTERf, DRCC_WR_BYTE_COUNTERf, DRCD_WR_BYTE_COUNTERf, 
                                                      DRCE_WR_BYTE_COUNTERf, DRCF_WR_BYTE_COUNTERf, DRCG_WR_BYTE_COUNTERf, DRCH_WR_BYTE_COUNTERf  };
    uint32 field32_val, reg32_val, i;

    LOG_INFO(BSL_LS_SOC_DIAG,(BSL_META_U(unit,"DRC Read and Write Byte Counters:\n")));
    for (i=0; i < DIAG_MMU_NOF_DRC_TYPE_COUNTERS; ++i) {
        /* MMU_DRC_RD_BYTE_COUNTER */
        if(SOC_REG_IS_ENABLED(unit, mmu_drc_rd_byte_counters_regs[i])) {
            if(soc_reg32_get(unit, mmu_drc_rd_byte_counters_regs[i], REG_PORT_ANY, 0, &reg32_val) < 0){
                return CMD_FAIL;
            }
            field32_val = soc_reg_field_get(unit, mmu_drc_rd_byte_counters_regs[i], reg32_val, mmu_drc_rd_byte_counters_fields[i]);
            LOG_INFO(BSL_LS_SOC_DIAG,(BSL_META_U(unit,"\t%s = 0x%x\n"), SOC_REG_NAME(unit, mmu_drc_rd_byte_counters_regs[i]), field32_val));
        }

        /* MMU_DRC_WR_BYTE_COUNTER */
        if(SOC_REG_IS_ENABLED(unit, mmu_drc_wr_byte_counters_regs[i])) {
            if(soc_reg32_get(unit, mmu_drc_wr_byte_counters_regs[i], REG_PORT_ANY, 0, &reg32_val) < 0){
                return CMD_FAIL;
            }
            field32_val = soc_reg_field_get(unit, mmu_drc_wr_byte_counters_regs[i], reg32_val, mmu_drc_wr_byte_counters_fields[i]);
            LOG_INFO(BSL_LS_SOC_DIAG,(BSL_META_U(unit,"\t%s = 0x%x\n"), SOC_REG_NAME(unit, mmu_drc_rd_byte_counters_regs[i]), field32_val));
        }
    }

    LOG_INFO(BSL_LS_SOC_DIAG,(BSL_META_U(unit,"\n\n\nPacket Counters:\n")));
    /* MMU_IDR_PACKET_COUNTER */
    if(SOC_REG_IS_ENABLED(unit, MMU_IDR_PACKET_COUNTERr)){
        if(soc_reg32_get(unit, MMU_IDR_PACKET_COUNTERr, REG_PORT_ANY, 0, &reg32_val) < 0){
            return CMD_FAIL;
        }
        field32_val = soc_reg_field_get(unit, MMU_IDR_PACKET_COUNTERr, reg32_val, IDR_PACKET_COUNTERf);
        LOG_INFO(BSL_LS_SOC_DIAG,(BSL_META_U(unit,"\t%s = 0x%x\n"), SOC_REG_NAME(unit, MMU_IDR_PACKET_COUNTERr), field32_val));
    }

    /* MMU_IPT_PACKET_COUNTER */
    if(SOC_REG_IS_ENABLED(unit, MMU_IPT_PACKET_COUNTERr)){
        if(soc_reg32_get(unit, MMU_IPT_PACKET_COUNTERr, REG_PORT_ANY, 0, &reg32_val) < 0){
            return CMD_FAIL;
        }
        field32_val = soc_reg_field_get(unit, MMU_IPT_PACKET_COUNTERr, reg32_val, IPT_PACKET_COUNTERf);
        LOG_INFO(BSL_LS_SOC_DIAG,(BSL_META_U(unit,"\t%s = 0x%x\n"), SOC_REG_NAME(unit, MMU_IPT_PACKET_COUNTERr), field32_val));
    }

    /* MMU_LBM_COUNTER */
    if(SOC_REG_IS_ENABLED(unit, MMU_IPT_PACKET_COUNTERr)){
        if(soc_reg32_get(unit, MMU_LBM_COUNTERr, REG_PORT_ANY, 0, &reg32_val) < 0){
            return CMD_FAIL;
        }
        field32_val = soc_reg_field_get(unit, MMU_LBM_COUNTERr, reg32_val, LBM_COUNTERf);
        LOG_INFO(BSL_LS_SOC_DIAG,(BSL_META_U(unit,"\t%s = 0x%x\n"), SOC_REG_NAME(unit, MMU_LBM_COUNTERr), field32_val));
    }

    return 0;
}


/*
 * Print diag mmu usage
 */
void cmd_dpp_diag_mmu_usage(int unit) {
    char cmd_dpp_diag_mmu_usage[] =
       "Usage (DIAG mmu):\n"
       "\tDIAGnotsics mmu commands\n"
       "\tUsages:\n"
       "\tDIAG mmu <OPTION>\n"
#ifdef __PEDANTIC__
       "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else 
       "OPTION can be:\n"
       "\tcounters - \tDisplay counters\n";
#endif
          
       LOG_INFO(BSL_LS_SOC_DIAG,(BSL_META_U(unit,"%s"), cmd_dpp_diag_mmu_usage));
}


/*
 * "Driver" for the diag ocb functionalities.
 */
cmd_result_t cmd_dpp_diag_mmu(int unit, args_t *a) {

    char      *function;

    if (!SOC_IS_ARAD(unit)) {
        LOG_INFO(BSL_LS_SOC_DIAG,(BSL_META_U(unit,"Error: MMU Diag is not supported for this unit=%d\n"), unit));
        return CMD_USAGE;
    } 

    function = ARG_GET(a); 
    if ((!function) || (!sal_strncasecmp(function, "counters", strlen(function)))) {
        return cmd_dpp_diag_mmu_counters(unit, a);
    } else {
        return CMD_USAGE;
    }
}

