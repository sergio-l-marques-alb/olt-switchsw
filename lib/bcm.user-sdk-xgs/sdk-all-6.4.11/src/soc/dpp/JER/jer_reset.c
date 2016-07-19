/*
 * $Id: jer_reset.c, v1 21/09/2014 09:55:39 azarrin $
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
 */

/*************
 * INCLUDES  *
 *************/

/* SOC dcmn includes */
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_cmic.h>

/* SOC DPP includes */
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/JER/jer_init.h>
#include <soc/dpp/JER/jer_fabric.h>
#include <soc/dpp/JER/jer_reset.h>
#include <soc/dpp/JER/jer_dram.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JER/jer_regs.h>
#include <shared/swstate/sw_state.h>

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/*************
 * TYPE DEFS *
 *************/


/*************
 * FUNCTIONS *
 *************/
int soc_jer_reset_blocks_poll_init_finish(int unit)
{
    int i;

    SOCDNX_INIT_FUNC_DEFS;
    
    for(i = 0; i < SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; i++) {
        /* ING */
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRE_RESET_STATUS_REGISTERr, i, 0, CTXT_STATUS_INIT_DONEf, 0x1));
        if (!SOC_IS_QAX(unit)) { 
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRE_RESET_STATUS_REGISTERr, i, 0, CTXT_MAP_INIT_DONEf, 0x1));

            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, i, 0, CONTEXT_STATUS_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, i, 0, CHUNK_STATUS_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, i, 0, WORD_INDEX_FIFO_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, i, 0, FREE_PCB_FIFO_INIT_DONEf, 0x1));

            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, FPF_0_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, FPF_1_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, IS_FPF_0_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, IS_FPF_1_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, DESTINATION_TABLE_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, LAG_MAPPING_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, LAG_TO_LAG_RANGE_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, MCDB_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, STACK_FEC_RESOLVE_INIT_DONEf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, STACK_TRUNK_RESOLVE_INIT_DONEf, 0x1));

            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, i, 0, IQC_INITf, 0x0));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, i, 0, STE_INITf, 0x0));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, i, 0, PDM_INITf, 0x0));

            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, MRPS_INIT_SEQ_ONr, i, 0, MCDA_INIT_SEQ_ONf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, MTRPS_EM_INIT_SEQ_ONr, i, 0, MCDA_INIT_SEQ_ONf, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IHB_ISEM_RESET_STATUS_REGISTERr, i, 0, ISEM_KEYT_RESET_DONEf, 0x1));
        }

        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, i, 0, IPS_INIT_TRIGGERf, 0x0));
        
        /* EGR */
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EGQ_EGQ_BLOCK_INIT_STATUSr, i, 0, EGQ_BLOCK_INITf, 0x0));
    }
    
    /* ING */
    SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, PPDB_A_OEMA_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, OEMA_KEYT_RESET_DONEf, 0x1));
    SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, PPDB_A_OEMB_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, OEMB_KEYT_RESET_DONEf, 0x1));

    SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, PPDB_B_LARGE_EM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, LARGE_EM_KEYT_RESET_DONEf, 0x1));

    SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EDB_GLEM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, GLEM_KEYT_RESET_DONEf, 0x1)); 
    SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EDB_ESEM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, ESEM_KEYT_RESET_DONEf, 0x1));

    SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, OAMP_REMOTE_MEP_EXACT_MATCH_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, REMOTE_MEP_EXACT_MATCH_KEYT_RESET_DONEf, 0x1));
        
exit:
    SOCDNX_FUNC_RETURN;  
}



/*********************************************************************
 *     Resets the end-to-end scheduler. The reset is performed
 *     by clearing the internal scheduler pipes, and then
 *     performing soft-reset.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
STATIC int jer_sch_reset(int unit)
{
    uint32
        mc_conf_0_fld_val,
        mc_conf_1_fld_val,
        ingr_shp_en_fld_val, 
        timeout_val,
        backup_msg_en,
        dlm_enable;
    uint32
        tbl_data[ARAD_SCH_SCHEDULER_INIT_TBL_ENTRY_SIZE] = {0};
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < SOC_JER_NOF_INSTANCES_SCH; i++) {

/*        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_DVS_CONFIG_1r, i, 0, FORCE_PAUSEf,  0x1));*/
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, i, 0, DISABLE_FABRIC_MSGSf,  0x1));

        SOCDNX_IF_ERR_EXIT(READ_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r(unit, i, &mc_conf_0_fld_val));
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r(unit, i, &mc_conf_1_fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r, i, 0, MULTICAST_GFMC_ENABLEf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r, i, 0, MULTICAST_BFMC_1_ENABLEf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r, i, 0, MULTICAST_BFMC_2_ENABLEf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r, i, 0, MULTICAST_BFMC_3_ENABLEf,  0x0));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, i, 0, INGRESS_SHAPING_ENABLEf, &ingr_shp_en_fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, i, 0, INGRESS_SHAPING_ENABLEf,  0x0));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr, i, 0, DLM_ENABLEf, &dlm_enable));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr, i, 0, DLM_ENABLEf,  0x0));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, SCH_SMP_BACK_UP_MESSAGESr, i, 0, BACKUP_MSG_ENABLEf, &backup_msg_en));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SMP_BACK_UP_MESSAGESr, i, 0, BACKUP_MSG_ENABLEf,  0x0));

        /* clear on set to all the sch memory error interrupts */  
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_ECC_INTERRUPT_REGISTERr(unit, i, 0xffffffff));

        soc_mem_field32_set(unit, SCH_SCHEDULER_INITm, tbl_data, SCH_INITf, 0x0);

        /* keep current timeout and set a new timeout needed for writing this special table */
        SOCDNX_IF_ERR_EXIT(READ_CMIC_SBUS_TIMEOUTr(unit, &timeout_val));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_TIMEOUTr(unit, 0xffffffff));

        /* write the table entry */
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_INITm(unit, i, 0x0, tbl_data));

        /* restore original timeout */
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_TIMEOUTr(unit, timeout_val));

        /*
        * Recover original configuration
        */
        
        /*SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_DVS_CONFIG_1r, i, 0, FORCE_PAUSEf,  0x0));*/
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, i, 0, DISABLE_FABRIC_MSGSf,  0x0));

        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r(unit, i,  mc_conf_0_fld_val));
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r(unit, i,  mc_conf_1_fld_val));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, i, 0, INGRESS_SHAPING_ENABLEf,  ingr_shp_en_fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr, i, 0, DLM_ENABLEf,  dlm_enable));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SMP_BACK_UP_MESSAGESr, i, 0, BACKUP_MSG_ENABLEf,  backup_msg_en));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_reset_nif_txi_oor(int unit) 
{
    soc_reg_above_64_val_t reg_above64_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CREATE_MASK(reg_above64_val, 128, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INIT_FQP_TXI_NIFr(unit, SOC_CORE_ALL, reg_above64_val));

    sal_usleep(1);

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INIT_FQP_TXI_NIFr(unit, SOC_CORE_ALL, reg_above64_val));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int jer_soft_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_RST_DOMAIN      rst_domain)
{
    uint32
        reg_val,
        res = SOC_SAND_OK;
/*        fmc_scheduler_configs_reg_val_orig,*/
    uint8
        is_traffic_enabled_orig,
        is_ingr,
        is_egr,
        is_ctrl_cells_enabled_orig,
        is_fabric = 0,
        is_schan_locked = 0;
    soc_reg_above_64_val_t
        soft_init_reg_val,
        soft_init_reg_val_orig,
        fld_above64_val,
        reg_above64_val;
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    is_ingr   = SOC_SAND_NUM2BOOL((rst_domain == ARAD_DBG_RST_DOMAIN_INGR)            || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC) || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_FULL)            || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC));
    is_egr    = SOC_SAND_NUM2BOOL((rst_domain == ARAD_DBG_RST_DOMAIN_EGR)            || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC) || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_FULL)           || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC));
    is_fabric = SOC_SAND_NUM2BOOL((rst_domain == ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC) || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC)  || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC));

    LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): Start. is_ingr=%d, is_egr=%d, is_fabric=%d\n"), FUNCTION_NAME(), is_ingr, is_egr, is_fabric));

    /************************************************************************/
    /* Disable Traffic                                                      */
    /************************************************************************/
    /*
     *  Store current traffic-enable-state (just in case: if we got here, it is enabled)
    */
    res = jer_mgmt_enable_traffic_get(unit, &is_traffic_enabled_orig);
    SOCDNX_IF_ERR_EXIT(handle_sand_result(res));
    LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): Disable Traffic. is_traffic_enabled_orig=%d\n"), FUNCTION_NAME(), is_traffic_enabled_orig));

    res = jer_mgmt_enable_traffic_set(unit, FALSE);
    SOCDNX_IF_ERR_EXIT(handle_sand_result(res));

    

    if (is_fabric == TRUE) {
        /*
         *  Store current traffic-enable-state (just in case: if we got here, it is enabled)
         */
        res = arad_mgmt_all_ctrl_cells_enable_get(unit, &is_ctrl_cells_enabled_orig);
        SOCDNX_IF_ERR_EXIT(handle_sand_result(res));

        /* calling unsafe methods is discouraged. in this case we do it because arad_dbg_dev_reset is always called safely */
        res = arad_mgmt_all_ctrl_cells_enable_set_unsafe(unit, FALSE, ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_SOFT_RESET);
        SOCDNX_IF_ERR_EXIT(handle_sand_result(res));
    }

    /* This lock is added since threads might access the device during soft reset, causing schan timeout */
    is_schan_locked = 1;
    SCHAN_LOCK(unit);

    /************************************************************************/
    /* Read original configuration                                          */
    /************************************************************************/
    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit,soft_init_reg_val));
    sal_memcpy(soft_init_reg_val_orig, soft_init_reg_val, sizeof(soc_reg_above_64_val_t));

    LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): Read original configuration. soft_init_reg_val_orig=0x%x,0x%x,0x%x,0x%x\n"), FUNCTION_NAME(), soft_init_reg_val_orig[0], soft_init_reg_val_orig[1], soft_init_reg_val_orig[2], soft_init_reg_val_orig[3]));
    
    /************************************************************************/
    /* IN-RESET                                                             */
    /************************************************************************/
    LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): IN-RESET\n"), FUNCTION_NAME()));

    SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
    SOC_REG_ABOVE_64_CREATE_MASK(fld_above64_val, 1, 0);

    if (is_ingr) {
#ifdef JER_SOFT_RESET_FIXES
        SOCDNX_IF_ERR_EXIT(READ_IPS_FMC_SCHEDULER_CONFIGSr(unit, &fmc_scheduler_configs_reg_val_orig));
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_FMC_SCHEDULER_CONFIGSr(unit,  0x04000000));
#endif

        /* IQMT should get reset before the others */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_21f, fld_above64_val);/* IQMT_BLKID */
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        /*
         *  Soft-init: put in-reset IPS, IQM, IPT, MMU, DPRC, IRE, IDR, IRR, FDT, PDM
         */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_1f, fld_above64_val);/* IRE_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_2f, fld_above64_val);/* IDR_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_3f, fld_above64_val);/* MRPS0_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_4f, fld_above64_val);/* MRPS1_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_5f, fld_above64_val);/* MRPS2_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_6f, fld_above64_val);/* MRPS3_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_8f, fld_above64_val);/* IRR_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_9f, fld_above64_val);/* MMU_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_10f, fld_above64_val);/* OCB_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_19f, fld_above64_val);/* IQM0_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_20f, fld_above64_val);/* IQM1_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_22f, fld_above64_val);/* CRPS_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_23f, fld_above64_val);/* IPS0_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_24f, fld_above64_val);/* IPS1_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_25f, fld_above64_val);/* IPST_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_26f, fld_above64_val);/* IPT_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_55f, fld_above64_val);/* CFC_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_56f, fld_above64_val);/* OAMP_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_57f, fld_above64_val);/* OLP_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_58f, fld_above64_val);/* IHP0_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_59f, fld_above64_val);/* IHP1_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_60f, fld_above64_val);/* IHB0_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_61f, fld_above64_val);/* IHB1_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_62f, fld_above64_val);/* LPM_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_63f, fld_above64_val);/* PPDB_A_BLKID*/
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_64f, fld_above64_val);/* PPDB_B_BLKID*/
        if (is_fabric == 0x1) {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_27f, fld_above64_val);/* FCT_BLKID   */
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_28f, fld_above64_val);/* FDT_BLKID   */
        }

        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        rv = soc_jer_dram_init_drc_soft_init(unit, &SOC_DPP_CONFIG(unit)->arad->init.drc_info, 0x1);
        SOCDNX_IF_ERR_EXIT(rv);

        /* Reseting CMICM TXI credits */ 
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TXBUF_IPINTF_INTERFACE_CREDITSr(unit, 0x40));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TXBUF_IPINTF_INTERFACE_CREDITSr(unit, 0x0));

    } /* is_ingr */

    if (is_egr) {

        /* Reset NBI special ports */
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ADDITIONAL_RESETSr(unit, &reg_val));
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 0);
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, EIF_RSTNf, 0);
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_0_RX_RSTNf, 0);
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_1_RX_RSTNf, 0);
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_0_TX_RSTNf, 0);
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_1_TX_RSTNf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ADDITIONAL_RESETSr(unit,  reg_val));

        SOCDNX_IF_ERR_EXIT(READ_NBIL_ADDITIONAL_RESETSr(unit, 0, &reg_val));
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 0);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_RX_RSTNf, 0);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_RX_RSTNf, 0);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_TX_RSTNf, 0);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_TX_RSTNf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ADDITIONAL_RESETSr(unit, 0, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_NBIL_ADDITIONAL_RESETSr(unit, 1, &reg_val));
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 0);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_RX_RSTNf, 0);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_RX_RSTNf, 0);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_TX_RSTNf, 0);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_TX_RSTNf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ADDITIONAL_RESETSr(unit, 1, reg_val));

        /* NOTE: EDB must be reset before EGQ/EPNI */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_52f, fld_above64_val);/* EDB */
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        /* NOTE: EGQ/EPNI must be reset before NBI */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_46f, fld_above64_val);/* EGQ0_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_47f, fld_above64_val);/* EGQ1_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_48f, fld_above64_val);/* EPNI0_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_49f, fld_above64_val);/* EPNI1_BLKID */
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        /*
         *  Soft-init: put in-reset IPS, IQM, IPT, MMU, DPRC, IRE, IDR, IRR, FDT, PDM
         */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_33f, fld_above64_val);/* FDA_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_104f, fld_above64_val);/* NBIH_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_105f, fld_above64_val);/* NBIL0_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_106f, fld_above64_val);/* NBIL1_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_50f, fld_above64_val);/* CGM0_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_51f, fld_above64_val);/* CGM1_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_55f, fld_above64_val);/* CFC_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_56f, fld_above64_val);/* OAMP_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_57f, fld_above64_val);/* OLP_BLKID */

        if (is_fabric == 0x1) {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_31f, fld_above64_val);/* FCR_BLKID   */
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_32f, fld_above64_val);/* FDR_BLKID   */
        }

        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        /* Close FCR */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_FL_STSf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_CRD_FCRf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_SRf,  0x1));

        /* Rest scheduler (in-out) */
        rv = jer_sch_reset(unit);
        SOCDNX_IF_ERR_EXIT(rv);

    } /* is_egr */

    /************************************************************************/
    /* Validate Data Path is clean - active queue = 0                       */
    /************************************************************************/
    if (is_ingr) {    
    }

    if (is_egr) {    

    }

    /************************************************************************/
    /* OUT-OF-RESET                                                         */
    /************************************************************************/
    LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): OUT-OF-RESET.\n"), FUNCTION_NAME()));

    SOC_REG_ABOVE_64_CLEAR(fld_above64_val);

    if (is_ingr) {

        /* IQMT should get out of reset before the others */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_21f, fld_above64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_1f, fld_above64_val);/* IRE_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_2f, fld_above64_val);/* IDR_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_3f, fld_above64_val);/* MRPS0_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_4f, fld_above64_val);/* MRPS1_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_5f, fld_above64_val);/* MRPS2_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_6f, fld_above64_val);/* MRPS3_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_8f, fld_above64_val);/* IRR_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_9f, fld_above64_val);/* MMU_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_10f, fld_above64_val);/* OCB_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_19f, fld_above64_val);/* IQM0_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_20f, fld_above64_val);/* IQM1_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_22f, fld_above64_val);/* CRPS_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_23f, fld_above64_val);/* IPS0_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_24f, fld_above64_val);/* IPS1_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_25f, fld_above64_val);/* IPST_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_26f, fld_above64_val);/* IPT_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_55f, fld_above64_val);/* CFC_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_56f, fld_above64_val);/* OAMP_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_57f, fld_above64_val);/* OLP_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_58f, fld_above64_val);/* IHP0_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_59f, fld_above64_val);/* IHP1_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_60f, fld_above64_val);/* IHB0_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_61f, fld_above64_val);/* IHB1_BLKID  */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_62f, fld_above64_val);/* LPM_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_63f, fld_above64_val);/* PPDB_A_BLKID*/
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_64f, fld_above64_val);/* PPDB_B_BLKID*/
        if (is_fabric == 0x1) {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_27f, fld_above64_val);/* FCT_BLKID   */
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_28f, fld_above64_val);/* FDT_BLKID   */
        }

        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        /* DPRC - out of soft init*/
        rv = soc_jer_dram_init_drc_soft_init(unit, &SOC_DPP_CONFIG(unit)->arad->init.drc_info, 0);
        SOCDNX_IF_ERR_EXIT(rv);

#ifdef JER_SOFT_RESET_FIXES
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_FMC_SCHEDULER_CONFIGSr(unit,  fmc_scheduler_configs_reg_val_orig));
#endif

        /* Check if IQM finished its 96K cycles initialization */
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 0, 0, IQC_INITf, 0));
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 0, 0, STE_INITf, 0));
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 0, 0, PDM_INITf, 0));
        if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1 )
        {
            /* in single core all PDM banks are assigned to core 0, this causes the PDM init of core 1 to stay in reset, also no point in checking other init fields on core 1 */
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 1, 0, PDM_INITf, 0));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 1, 0, IQC_INITf, 0));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 1, 0, STE_INITf, 0));
        }
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, 0, 0, IPS_INIT_TRIGGERf, 0));
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, 1, 0, IPS_INIT_TRIGGERf, 0));

        /* 
         *  Re-enable auto-gen in IDR
         */
        SOCDNX_IF_ERR_EXIT(soc_jer_ocb_dram_buffer_autogen_set(unit));

    } /* is_ingr */

    if(is_egr) {

        /* NOTE: EDB must be reset before EGQ/EPNI */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_52f, fld_above64_val);/* EDB_BLKID */
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        /* NOTE: EGQ has to be reset before NBI blocks,
         * due to TXI counter.
         * The correct flow:
         * 1. Reset EGQ
         * 2. Reset egq2nif TXI.
         * 3. Reset NBI.
        */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_46f, fld_above64_val);/* EGQ0_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_47f, fld_above64_val);/* EGQ1_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_48f, fld_above64_val);/* EPNI0_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_49f, fld_above64_val);/* EPNI1_BLKID */
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        /* Take NIF TXIs in EGQ out of reset */
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_nif_txi_oor(unit));

        /*
         *  Soft-init: put in-reset IPS, IQM, IPT, MMU, DPRC, IRE, IDR, IRR, FDT, PDM
         */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_33f, fld_above64_val);/* FDA_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_104f, fld_above64_val);/* NBIH_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_105f, fld_above64_val);/* NBIL0_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_106f, fld_above64_val);/* NBIL1_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_50f, fld_above64_val);/* CGM0_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_51f, fld_above64_val);/* CGM1_BLKID   */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_55f, fld_above64_val);/* CFC_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_56f, fld_above64_val);/* OAMP_BLKID */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_57f, fld_above64_val);/* OLP_BLKID */

        if (is_fabric == 0x1) {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_31f, fld_above64_val);/* FCR_BLKID   */
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_32f, fld_above64_val);/* FDR_BLKID   */
        }

        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        sal_usleep(1);

        /* Reset NBI special ports */
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ADDITIONAL_RESETSr(unit, &reg_val));
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 1);
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, EIF_RSTNf, 1);
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_0_RX_RSTNf, 1);
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_1_RX_RSTNf, 1);
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_0_TX_RSTNf, 1);
        soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_1_TX_RSTNf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ADDITIONAL_RESETSr(unit,  reg_val));

        SOCDNX_IF_ERR_EXIT(READ_NBIL_ADDITIONAL_RESETSr(unit, 0, &reg_val));
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 1);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_RX_RSTNf, 1);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_RX_RSTNf, 1);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_TX_RSTNf, 1);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_TX_RSTNf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ADDITIONAL_RESETSr(unit, 0, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_NBIL_ADDITIONAL_RESETSr(unit, 1, &reg_val));
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 1);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_RX_RSTNf, 1);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_RX_RSTNf, 1);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_TX_RSTNf, 1);
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_TX_RSTNf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ADDITIONAL_RESETSr(unit, 1, reg_val));

        /* reset_ip_to_cmic_credits */
        SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_above64_val));
        SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_EGQ_0_RXI_RESET_Nf, fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_EGQ_1_RXI_RESET_Nf, fld_above64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above64_val));
                
        SOC_REG_ABOVE_64_CREATE_MASK(fld_above64_val, 1, 0);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_EGQ_0_RXI_RESET_Nf, fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_EGQ_1_RXI_RESET_Nf, fld_above64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above64_val));

        /* Reset PackerInterleaver (PIR) credits */
        SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_above64_val));
        SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_TXI_CREDITS_INIT_VALUEf, fld_above64_val);
        SOC_REG_ABOVE_64_CREATE_MASK(fld_above64_val, 1, 0);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_TXI_CREDITS_INITf, fld_above64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above64_val));
        SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_TXI_CREDITS_INITf, fld_above64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above64_val));

        /* Sending CPU credits from CMIC to EGQ */
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x1));

        /* Open FCR */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_FL_STSf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_CRD_FCRf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_SRf,  0x0));

        /* Resetting CMICM RXI credits */ 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_CMICMr, SOC_CORE_ALL, 0, INIT_FQP_TXI_CMICMf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_OLPr, SOC_CORE_ALL, 0, INIT_FQP_TXI_OLPf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_OAMr, SOC_CORE_ALL, 0, INIT_FQP_TXI_OAMf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_RCYr, SOC_CORE_ALL, 0, INIT_FQP_TXI_RCYf,  0x1));

        /* Waiting for longest block to finish */
        rv = soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EGQ_EGQ_BLOCK_INIT_STATUSr, REG_PORT_ANY, 0, EGQ_BLOCK_INITf, 0);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_INIT,
                     (BSL_META_U(unit,
                                 "%s(): Error Validate out-of-reset done indications: EGQ_EGQ_BLOCK_INIT_STATUSr, EGQ_BLOCK_INITf.\n"), FUNCTION_NAME()));
        }

    } /* is_egr */

    /************************************************************************/
    /* OUT-OF-RESET, Revert to original (Soft-init per-block map)           */
    /************************************************************************/
    /* soft_init_reg_val_orig is probably 0x0 - take all other blocks out of soft init */
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val_orig));

    /* soft_init_reg_val_orig is probably 0x0 - take all other blocks out of soft init */
    SCHAN_UNLOCK(unit);
    is_schan_locked = 0;

    /************************************************************************/
    /* Restore Configuration if needed                                      */
    /************************************************************************/
    if (is_fabric == 0x1) {
        res = arad_mgmt_all_ctrl_cells_enable_set_unsafe(unit, is_ctrl_cells_enabled_orig, ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_SOFT_RESET);
        SOCDNX_IF_ERR_EXIT(handle_sand_result(res));
    }

    /************************************************************************/
    /*  Restore traffic                                                     */
    /************************************************************************/

    LOG_VERBOSE(BSL_LS_SOC_INIT,
                (BSL_META_U(unit,
                            "%s(): Restore traffic.\n"), FUNCTION_NAME()));
    res = jer_mgmt_enable_traffic_set(unit, is_traffic_enabled_orig);
    SOCDNX_IF_ERR_EXIT(handle_sand_result(res));

    /************************************************************************/
    /*  Clear interrupts                                                    */
    /************************************************************************/
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_INTERRUPT_REGISTERr(unit, SOC_CORE_ALL,  0xffffffff));

exit:
    if (is_schan_locked) {
        SCHAN_UNLOCK(unit);
    }   
    SOCDNX_FUNC_RETURN;
}

/* 
 * take blocks in or out of sbus reset: 
 * is_in_reset = 1 in reset
 * is_in_reset = 0 out of reset
 */
STATIC int soc_jer_reset_sbus_reset(int unit, int is_in_reset)
{
    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;
    /* IQMT */     
    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SBUS_RESETr, reg_above_64_val, BLOCKS_SBUS_RESET_21f, is_in_reset);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
    /* IQMs */     
    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SBUS_RESETr, reg_above_64_val, BLOCKS_SBUS_RESET_19f, is_in_reset);
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SBUS_RESETr, reg_above_64_val, BLOCKS_SBUS_RESET_20f, is_in_reset);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
    /* all the rest */
    if (is_in_reset) {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    } else {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));

exit:
    SOCDNX_FUNC_RETURN;
}

/* 
 * take blocks in or out of soft reset: 
 * is_in_reset = 1 in reset
 * is_in_reset = 0 out of reset
 */
STATIC int soc_jer_reset_blocks_reset(int unit, int is_in_reset)
{
    int disable_hard_reset;
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit)) {
    /* IQMT */     
    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_21f, is_in_reset);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));

    /* IQMs */     
    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_19f, is_in_reset);
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, BLOCKS_SOFT_RESET_20f, is_in_reset);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    }

    /* All the rest */
    if (is_in_reset) {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    } else {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));

    /* Take PDM in or out of reset */
    SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PDM_RSTNf, is_in_reset ? 0 : 1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above_64_val));

    /* Take MBU in or out of reset */
    SOCDNX_IF_ERR_EXIT(READ_ECI_ECIC_BLOCKS_RESETr(unit, &reg_val));
    soc_reg_field_set(unit, ECI_ECIC_BLOCKS_RESETr, &reg_val, FIELD_0_0f, is_in_reset);
    soc_reg_field_set(unit, ECI_ECIC_BLOCKS_RESETr, &reg_val, TIME_SYNC_RESETf, is_in_reset);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_ECIC_BLOCKS_RESETr(unit, reg_val));

    disable_hard_reset = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_without_device_hard_reset", 0);
    LOG_VERBOSE(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): disable_hard_reset = %d\n"), FUNCTION_NAME(), disable_hard_reset));
    if (disable_hard_reset == 1) {
        if (is_in_reset == 1) {
            /* Fix for : IRE reset doesn't clear CMIC credits - when IRE going out of reset it always adds credits to the CMIC, so CMIC Credits needs to be reset when ever IRE is reset */
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TXBUF_IPINTF_INTERFACE_CREDITSr(unit, 0x40));
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TXBUF_IPINTF_INTERFACE_CREDITSr(unit, 0x0));

            /* Release all credits of CMIC packet I/F so that it can accept packets */
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x0));
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x1));
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x0));
        }
    }

    if (is_in_reset == 0x0) {
        /* Verify blocks are OOR */
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_blocks_poll_init_finish(unit));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_reset_blocks_soft_init(int unit, int reset_action)
{
    soc_reg_above_64_val_t reg_above_64_val;
    
    SOCDNX_INIT_FUNC_DEFS;

    if ((reset_action == SOC_DPP_RESET_ACTION_IN_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) { 
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    }

    sal_usleep(100);

    if ((reset_action == SOC_DPP_RESET_ACTION_OUT_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {

        /* IQMT */
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_21f, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));

        /* IQMs */
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_19f, 0x0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_20f, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));

        /* EDB */
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_52f, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));

        /* EGQs */
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_46f, 0x0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_47f, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));

        /* EPNIs */
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_48f, 0x0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_49f, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));

        /* Take ALL blocks Out of soft reset */
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));

        /* Validate Soft init finished */
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_blocks_poll_init_finish(unit));

        /* Reset IRE - CMIC TXI */
        if (!SOC_IS_QAX(unit)) { 
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_SYS_CFG_RESERVED_1r, REG_PORT_ANY, FIELD_0_0f, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_SYS_CFG_RESERVED_1r, REG_PORT_ANY, FIELD_0_0f, 0x0));
        }

    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_jer_device_blocks_reset(int unit, int reset_action)
{
    SOCDNX_INIT_FUNC_DEFS;

    if ((reset_action == SOC_DPP_RESET_ACTION_IN_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) { 

        /* Sbus Reset*/
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_sbus_reset(unit, 1));
        
        /* Soft Reset*/
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_blocks_reset(unit, 1));
    }

    if ((reset_action == SOC_DPP_RESET_ACTION_OUT_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {

        /* sbus reset */
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_sbus_reset(unit, 0));
        
        /* Configure PLL */
        SOCDNX_IF_ERR_EXIT(jer_pll_init(unit));

        /* soft reset */
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_blocks_reset(unit, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_jer_init_reg_access(
    int unit,
    int reset_action)
{
    SOCDNX_INIT_FUNC_DEFS;

 

     /* init defines  & sw_state data structures*/
    if ((SOC_CONTROL(unit)->soc_flags & SOC_F_INITED) == 0) {
        soc_jer_specific_info_config_derived(unit);
        SOC_DPP_CONFIG(unit)->emulation_system = soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0);
        soc_dpp_implementation_defines_init(unit);
        SOCDNX_IF_ERR_RETURN(shr_sw_state_init(unit, 0x0, socSwStateDataBlockRegularInit, SHR_SW_STATE_MAX_DATA_SIZE_IN_BYTES));
        SOCDNX_IF_ERR_EXIT(soc_dpp_info_config_ports(unit));
    }

    /*
     * Reset device.
     * Also enable device access, set default Iproc/CmicD configuration
     * No access allowed before this stage.
     */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_reset(unit, reset_action));
    /* Enable Access to device blocks */
    SOCDNX_IF_ERR_EXIT(soc_jer_device_blocks_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET));

    /* Init blocks' broadcast IDs */
    if (!SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_init_brdc_blk_id_set(unit)); 
    }

    /* Init WB engine for ports reg access if not already initiated */
    if ((SOC_CONTROL(unit)->soc_flags & SOC_F_INITED) == 0) {
        SOCDNX_IF_ERR_RETURN(soc_dpp_wb_engine_init(unit)); 
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    soc_arad_init_empty_scache(unit);
#endif

    /* Enable ports reg access */
    if ((SOC_CONTROL(unit)->soc_flags & SOC_F_INITED) == 0) {
        SOCDNX_IF_ERR_EXIT(soc_jer_ports_config(unit));
    }

    SOC_CONTROL(unit)->soc_flags |= SOC_F_INITED;
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_device_reset(int unit, int mode, int action)
{
    uint32 enable;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Jericho function. invalid Device")));
    }

    switch (mode) {
    case SOC_DPP_RESET_MODE_HARD_RESET:
        SOCDNX_IF_ERR_RETURN(soc_dcmn_cmic_device_hard_reset(unit, action));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_RESET:
        SOCDNX_IF_ERR_RETURN(soc_jer_device_blocks_reset(unit, action));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_FULL));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_INGR));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_EGR));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC));
        break;
    case SOC_DPP_RESET_MODE_INIT_RESET:
        SOCDNX_IF_ERR_RETURN(soc_dpp_jericho_init(unit, action));
        break;
    case SOC_DPP_RESET_MODE_REG_ACCESS:
        SOCDNX_IF_ERR_RETURN(soc_jer_init_reg_access(unit, action));
        break;
    case SOC_DPP_RESET_MODE_ENABLE_TRAFFIC:
        enable = (action == SOC_DPP_RESET_ACTION_IN_RESET) ? FALSE : TRUE;
        SOCDNX_IF_ERR_RETURN(handle_sand_result(jer_mgmt_enable_traffic_set(unit, enable)));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT:
        SOCDNX_IF_ERR_RETURN(soc_jer_reset_blocks_soft_init(unit, action));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unknown/Unsupported Reset Mode")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

