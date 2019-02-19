
/*
 * $Id: soc_dcmn_intr_handler.c, v1 16/06/2014 09:55:39 azarrin $
 *
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
static 
soc_reg_t soc_jer_interrupt_monitor_mem_reg[] = {
    CFC_ECC_ERR_1B_MONITOR_MEM_MASKr,
    CFC_ECC_ERR_2B_MONITOR_MEM_MASKr,
    CRPS_ECC_ERR_1B_MONITOR_MEM_MASKr,
    CRPS_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DRCA_ECC_ERR_1B_MONITOR_MEM_MASKr,
    DRCA_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DRCA_PAR_ERR_MEM_MASKr,
    DRCB_ECC_ERR_1B_MONITOR_MEM_MASKr,
    DRCB_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DRCB_PAR_ERR_MEM_MASKr,
    DRCC_ECC_ERR_1B_MONITOR_MEM_MASKr,
    DRCC_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DRCC_PAR_ERR_MEM_MASKr,
    DRCD_ECC_ERR_1B_MONITOR_MEM_MASKr,
    DRCD_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DRCD_PAR_ERR_MEM_MASKr,
    DRCE_ECC_ERR_1B_MONITOR_MEM_MASKr,
    DRCE_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DRCE_PAR_ERR_MEM_MASKr,
    DRCF_ECC_ERR_1B_MONITOR_MEM_MASKr,
    DRCF_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DRCF_PAR_ERR_MEM_MASKr,
    DRCG_ECC_ERR_1B_MONITOR_MEM_MASKr,
    DRCG_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DRCG_PAR_ERR_MEM_MASKr,
    DRCH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    DRCH_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DRCH_PAR_ERR_MEM_MASKr,
    ECI_ECC_ERR_1B_MONITOR_MEM_MASKr,
    ECI_ECC_ERR_2B_MONITOR_MEM_MASKr,
    EDB_ECC_ERR_1B_MONITOR_MEM_MASKr,
    EDB_ECC_ERR_2B_MONITOR_MEM_MASKr,
    EDB_PAR_ERR_MEM_MASKr,
    EGQ_ECC_ERR_1B_MONITOR_MEM_MASKr,
    EGQ_ECC_ERR_2B_MONITOR_MEM_MASKr,
    EGQ_PAR_ERR_MEM_MASKr,
    EPNI_ECC_ERR_1B_MONITOR_MEM_MASKr,
    EPNI_ECC_ERR_2B_MONITOR_MEM_MASKr,
    EPNI_PAR_ERR_MEM_MASKr,
    FCR_ECC_ERR_1B_MONITOR_MEM_MASKr,
    FCR_ECC_ERR_2B_MONITOR_MEM_MASKr,
    FCR_PAR_ERR_MEM_MASKr,
    FCT_ECC_ERR_1B_MONITOR_MEM_MASKr,
    FCT_ECC_ERR_2B_MONITOR_MEM_MASKr,
    FCT_PAR_ERR_MEM_MASKr,
    FDA_ECC_ERR_1B_MONITOR_MEM_MASKr,
    FDA_ECC_ERR_2B_MONITOR_MEM_MASKr,
    FDA_PAR_ERR_MEM_MASKr,
    FDA_P_1_PSM_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FDA_P_2_PSM_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FDA_P_3_PSM_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FDA_P_1_PSM_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FDA_P_2_PSM_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FDA_P_3_PSM_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FDR_ECC_ERR_1B_MONITOR_MEM_MASKr,
    FDR_ECC_ERR_2B_MONITOR_MEM_MASKr,
    FDR_PAR_ERR_MEM_MASKr,
    FDR_P_1_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FDR_P_1_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FDR_P_2_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FDR_P_2_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FDR_P_3_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FDR_P_3_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FDT_ECC_ERR_1B_MONITOR_MEM_MASKr,
    FDT_ECC_ERR_2B_MONITOR_MEM_MASKr,
    FDT_PAR_ERR_MEM_MASKr,
    FMAC_ECC_ERR_1B_MONITOR_MEM_MASKr,
    FMAC_ECC_ERR_2B_MONITOR_MEM_MASKr,
    FMAC_PAR_ERR_MEM_MASKr,
    FSRD_ECC_ERR_1B_MONITOR_MEM_MASKr,
    FSRD_ECC_ERR_2B_MONITOR_MEM_MASKr,
    FSRD_WC_UC_MEM_MASK_BITMAPr,
    IDR_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IDR_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IHB_PAR_ERR_MEM_MASKr,
    IHP_PAR_ERR_MEM_MASKr,
    ILKN_PMH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    ILKN_PMH_ECC_ERR_2B_MONITOR_MEM_MASKr,
    ILKN_PMH_PAR_ERR_MEM_MASKr,
    ILKN_PML_ECC_ERR_1B_MONITOR_MEM_MASKr,
    ILKN_PML_ECC_ERR_2B_MONITOR_MEM_MASKr,
    ILKN_PML_PAR_ERR_MEM_MASKr,
    IPS_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IPS_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IPST_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IPST_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IPT_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IPT_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IPT_PAR_ERR_MEM_MASKr,
    IQM_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IQM_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IQM_PAR_ERR_MEM_MASKr,
    IQMT_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IQMT_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IQMT_PAR_ERR_MEM_MASKr,
    IRE_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IRE_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IRR_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IRR_ECC_ERR_2B_MONITOR_MEM_MASKr,
    MMU_ECC_ERR_1B_MONITOR_MEM_MASKr,
    MMU_ECC_ERR_2B_MONITOR_MEM_MASKr,
    MMU_ECC_ERR_1B_MONITOR_MEM_MASK_1r,
    MMU_ECC_ERR_2B_MONITOR_MEM_MASK_1r,
    MMU_PAR_ERR_MEM_MASKr,
    MRPS_ECC_ERR_1B_MONITOR_MEM_MASKr,
    MRPS_ECC_ERR_2B_MONITOR_MEM_MASKr,
    NBIH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    NBIH_ECC_ERR_2B_MONITOR_MEM_MASKr,
    NBIH_PAR_ERR_MEM_MASKr,
    NBIL_ECC_ERR_1B_MONITOR_MEM_MASKr,
    NBIL_ECC_ERR_2B_MONITOR_MEM_MASKr,
    NBIL_PAR_ERR_MEM_MASKr,
    OAMP_ECC_ERR_1B_MONITOR_MEM_MASKr,
    OAMP_ECC_ERR_2B_MONITOR_MEM_MASKr,
    OAMP_PAR_ERR_MEM_MASKr,
    OCB_ECC_ERR_1B_MONITOR_MEM_MASKr,
    OCB_ECC_ERR_2B_MONITOR_MEM_MASKr,
    OCB_PAR_ERR_MEM_MASKr,
    OLP_ECC_ERR_1B_MONITOR_MEM_MASKr,
    OLP_ECC_ERR_2B_MONITOR_MEM_MASKr,
    OLP_PAR_ERR_MEM_MASKr,
    RTP_ECC_ERR_1B_MONITOR_MEM_MASKr,
    RTP_ECC_ERR_2B_MONITOR_MEM_MASKr,
    RTP_PAR_ERR_MEM_MASKr,
    SCH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    SCH_ECC_ERR_2B_MONITOR_MEM_MASKr,
    SCH_PAR_ERR_MEM_MASKr,
    INVALIDr
};

/*************
 * FUNCTIONS *
 *************/

int
soc_jer_ser_init(int unit)
{
    int idx;
    int rc;
    soc_reg_above_64_val_t above_64;

    SOCDNX_INIT_FUNC_DEFS;

    /* unmask SER monitor registers*/
    SOC_REG_ABOVE_64_ALLONES(above_64);
    for(idx=0; soc_jer_interrupt_monitor_mem_reg[idx] != INVALIDr; idx++) {
        rc = soc_reg_above_64_set(unit, soc_jer_interrupt_monitor_mem_reg[idx], SOC_CORE_ALL, 0, above_64);
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_interrupts_disable(int unit)
{
    int rc;
    int i, copy_no;
    soc_interrupt_db_t* interrupts;
    soc_block_types_t  block;
    soc_reg_t reg;
    int blk;
    int nof_interrupts;
    soc_reg_above_64_val_t data;

    SOCDNX_INIT_FUNC_DEFS;

    rc = soc_jer_nof_interrupts(unit, &nof_interrupts);
    SOCDNX_IF_ERR_EXIT(rc);

    if (!SAL_BOOT_NO_INTERRUPTS) {

        /* disable all block interrupts */
        SOC_REG_ABOVE_64_CLEAR(data);
        for (i = 0; i < SOC_JER_NOF_BLK; i++) {
            if (SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info[i].int_mask_reg != INVALIDr) {
                rc = soc_reg_above_64_set(unit, SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info[i].int_mask_reg, 0,  0, data); 
                SOCDNX_IF_ERR_EXIT(rc);
            }
        }
        /* disable all interrupt vectors */
        interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
        for (i=0 ; i < nof_interrupts; i++) { 
            reg = interrupts[i].reg;
            /* Unsupported interrupts */
            if (!SOC_REG_IS_VALID(unit, reg))
            {
               continue;
            }
            block = SOC_REG_INFO(unit, reg).block;
            SOC_REG_ABOVE_64_CLEAR(data);
            SOC_BLOCKS_ITER(unit, blk, block) {
                copy_no = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                if (interrupts[i].vector_info) {
                    rc = soc_reg_above_64_set(unit, interrupts[i].vector_info->int_mask_reg, copy_no,  0, data);
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_interrupts_deinit(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* mask all interrupts in cmic (even in WB mode). This Masking update WB DB, althoght we dont use WB DB. */ 
    soc_cmicm_intr3_disable(unit, 0xffffffff);
    soc_cmicm_intr4_disable(unit, 0xffffffff);
    soc_cmicm_intr5_disable(unit, 0xffffffff);
    soc_cmicm_intr6_disable(unit, 0xffffffff);

    if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
        if (soc_ipoll_disconnect(unit) < 0) {
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "error disconnecting polled interrupt mode\n")));
        }
    } else {
        /* unit # is ISR arg */
        if (soc_cm_interrupt_disconnect(unit) < 0) {
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "could not disconnect interrupt line\n")));
        }
    }
    
    if (dcmn_intr_handler_deinit(unit) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error at interrupt db deinitialization")));
    }
           
exit:               
    SOCDNX_FUNC_RETURN;
}

int soc_jer_interrupts_init(int unit)
{
    int cmc;

    SOCDNX_INIT_FUNC_DEFS;

    cmc = SOC_PCI_CMC(unit);
     
    /* mask all interrupts in cmic (even in WB mode). This Masking update WB DB, althoght we dont use WB DB. */ 
    soc_cmicm_intr3_disable(unit, 0xffffffff);
    soc_cmicm_intr4_disable(unit, 0xffffffff);
    soc_cmicm_intr5_disable(unit, 0xffffffff);
    soc_cmicm_intr6_disable(unit, 0xffffffff);

    if (!SAL_BOOT_NO_INTERRUPTS) {

        /* Init interrupt DB */
        if (dcmn_intr_handler_init(unit) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
        }
     
        /* connect interrupts / start interrupt thread */
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_connect(unit, soc_cmicm_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
            }
            SOC_CONTROL(unit)->soc_flags |= SOC_F_POLLED;
        } else {
            if (soc_cm_interrupt_connect(unit, soc_cmicm_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("could not connect interrupt line")));
            }
        }    

        if (!SOC_WARM_BOOT(unit)) {
            uint32 rval;

            rval = soc_pci_read(unit, CMIC_CMCx_PCIE_MISCEL_OFFSET(cmc));
            soc_reg_field_set(unit, CMIC_CMC0_PCIE_MISCELr, &rval, ENABLE_MSIf, 0x1);
            soc_pci_write(unit, CMIC_CMCx_PCIE_MISCEL_OFFSET(cmc), rval); 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
