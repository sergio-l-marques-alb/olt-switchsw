/*
 * $Id: jer_drv.c Exp $
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
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/*
 * Includes
 */
#include <shared/bsl.h>

/* SAL includes */
#include <sal/appl/sal.h>

/* SOC includes */
#include <soc/error.h>

/* SOC DPP includes */
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>

#include <soc/dcmn/dcmn_mem.h>

/*
 * Functions
 */

/* SOC property parsing functions */
int soc_jer_str_prop_mc_nbr_full_dbuff_get(int unit, int *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_MULTICAST_NBR_FULL_DBUFF;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE") == 0) {
            *p_val = JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE;
        } else if (sal_strcmp(propval, "JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE") == 0) {
            *p_val = JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE;
        } else if (sal_strcmp(propval, "JERICHO_INIT_FMC_NO_REP_DBUFF_MODE") == 0) {
            *p_val = JERICHO_INIT_FMC_NO_REP_DBUFF_MODE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));  
        }
    } else {
        *p_val = JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_ocb_dram_buffer_conf_calc(int unit) 
{
    uint32
        ocb_0_size = 0x0, fmc_0_size, mnmc_0_size,
        ocb_1_size, fmc_1_size, mnmc_1_size,
        mnmc_total_size, fbc_fmc_size, fbc_mnmc_size,
        max_nof_dram_buffers,
        rv;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs;
    ARAD_MGMT_INIT  *arad_init;

    SOCDNX_INIT_FUNC_DEFS;

    dbuffs = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;
    arad_init = &SOC_DPP_CONFIG(unit)->arad->init;
    
    /*
     *  Full Multicast
     */ 
    if (arad_init->dram.fmc_dbuff_mode == JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE) {
        fmc_0_size = 512 * 1024;
    } else if (arad_init->dram.fmc_dbuff_mode == JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE) {
        fmc_0_size = 256 * 1024;
    } else {       
        /* JERICHO_INIT_FMC_NO_REP_DBUFF_MODE */ 
        fmc_0_size = 0;
    }
    
    /*
     *  OCB
     * Number of ocb dbuff calculated according to ocb dbuff size: 16Mb size -> 2MB. if buffs = 1k we have 2k  ocb.  
     */ 
    if (arad_init->ocb.ocb_enable != OCB_DISABLED) {
        ocb_0_size = (SOC_DPP_DEFS_GET(unit, ocb_memory_size) / 8) * (1024 * 1024 / arad_init->ocb.databuffer_size);
    } 
    if (ocb_0_size > (64 * 1024)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Size of OCB-0 + OCB-1 (=%d) should not exceed 64K.\n"), ocb_0_size));
    }

    /* Get total number of buffers */

    if (arad_init->dram.nof_drams == 0) {
        fmc_0_size = 0;
        mnmc_total_size = 0;
        max_nof_dram_buffers = 0;
    } else {
        /*
         *  Mini Multicast
         */ 
        max_nof_dram_buffers = SOC_DPP_CONFIG(unit)->jer->dbuffs.max_nof_dram_buffers;
        mnmc_total_size = max_nof_dram_buffers - ocb_0_size - fmc_0_size;
    }
    if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
        ocb_0_size /= 2;
        fmc_0_size /= 2;
        ocb_1_size = ocb_0_size;
        fmc_1_size = fmc_0_size;
        /* MNMC should be on 512K alignments */
        mnmc_0_size = (mnmc_total_size - (mnmc_total_size % (1024 * 1024))) / 2 + (mnmc_total_size % (1024 * 512));
        mnmc_1_size = (mnmc_total_size - (mnmc_total_size % (1024 * 1024))) / 2 + ((512 * 1024) * ((mnmc_total_size % (1024 * 1024)) / (1024 * 512)));
    } else {
        ocb_1_size = 0;
        mnmc_0_size = mnmc_total_size;
        mnmc_1_size = 0;
        fmc_1_size = 0;
    }
    if ((ocb_0_size + fmc_0_size) % (16 * 1024) != 0 || (ocb_1_size + fmc_1_size) % (16 * 1024) != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Size of OCB-0 + FMC-0 (=%d) and OCB-1 + FMC-1 (=%d) should be on 16K alignments.\n"),
                                                          (ocb_0_size + fmc_0_size),(ocb_1_size + fmc_1_size) ));
    }

    LOG_VERBOSE(BSL_LS_SOC_INIT,
                (BSL_META_U(unit,
                            "unit %d: OCB-0 size: %d, OCB-1 size: %d, MNMC-0 size: %d, MNMC-1 size: %d, FMC-0 size: %d, FMC-1 size: %d\n"),
                            unit, ocb_0_size, ocb_1_size, mnmc_0_size, mnmc_1_size, fmc_0_size, fmc_1_size));


    /* OCB - core 0 */
    dbuffs->ocb_0.start = 0;
    dbuffs->ocb_0.end = SOC_SAND_RNG_LAST(dbuffs->ocb_0.start, ocb_0_size);
    dbuffs->ocb_0.size = ocb_0_size;

    /* Full Multicast - core 0 */
    dbuffs->fmc_0.start = SOC_SAND_RNG_NEXT(dbuffs->ocb_0.end);
    dbuffs->fmc_0.end = SOC_SAND_RNG_LAST(dbuffs->fmc_0.start, fmc_0_size);
    dbuffs->fmc_0.size = fmc_0_size;

    if (ocb_0_size == 0) {
        dbuffs->ocb_0.end = 0;
    }

    /* OCB - core 1 */
    dbuffs->ocb_1.start = SOC_SAND_RNG_NEXT(dbuffs->fmc_0.end);
    dbuffs->ocb_1.end = SOC_SAND_RNG_LAST(dbuffs->ocb_1.start, ocb_1_size);
    dbuffs->ocb_1.size = ocb_1_size;

    /* Full Multicast - core 1 */
    if (arad_init->dram.nof_drams == 0) {
        dbuffs->fmc_1.start = 0;
    } else {
        dbuffs->fmc_1.start = SOC_SAND_RNG_NEXT(dbuffs->ocb_1.end);
    }
    dbuffs->fmc_1.end = SOC_SAND_RNG_LAST(dbuffs->fmc_1.start, fmc_1_size);
    dbuffs->fmc_1.size = fmc_1_size;

    /* Mini Multicast - core 0 */
    dbuffs->mnmc_0.start = SOC_SAND_RNG_NEXT(dbuffs->fmc_1.end);
    dbuffs->mnmc_0.end = SOC_SAND_RNG_LAST(dbuffs->mnmc_0.start, mnmc_0_size); 
    dbuffs->mnmc_0.size = mnmc_0_size;

    /* Mini Multicast - core 1 */
    dbuffs->mnmc_1.start = SOC_SAND_RNG_NEXT(dbuffs->mnmc_0.end);
    if (dbuffs->mnmc_1.start % (512 * 1024) != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("MNMC-0 (=%d) and MNMC-1 (=%d) should be on 512K alignments.\n"), mnmc_0_size, mnmc_1_size));
    }
    dbuffs->mnmc_1.end = SOC_SAND_RNG_LAST(dbuffs->mnmc_1.start, mnmc_1_size);
    dbuffs->mnmc_1.size = mnmc_1_size;

    /*
     *  FBC
     */ 
    if (arad_init->dram.nof_drams == 0) {
        fbc_fmc_size = 0;
        fbc_mnmc_size = 0;
    } else {
        rv = arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->fmc_0.end, dbuffs->fmc_0.start) + SOC_SAND_RNG_COUNT(dbuffs->fmc_1.end, dbuffs->fmc_1.start),
                                      arad_init->dram.dbuff_size, &fbc_fmc_size);
        SOCDNX_IF_ERR_EXIT(handle_sand_result(rv));
        rv = arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->mnmc_1.end, dbuffs->mnmc_0.start), arad_init->dram.dbuff_size, &fbc_mnmc_size);
        SOCDNX_IF_ERR_EXIT(handle_sand_result(rv));
    }

    dbuffs->fbc_fmc.start = SOC_SAND_RNG_NEXT(dbuffs->mnmc_1.end);
    dbuffs->fbc_fmc.end = SOC_SAND_RNG_LAST(dbuffs->fbc_fmc.start, fbc_fmc_size);
    dbuffs->fbc_fmc.size = fbc_fmc_size;

    dbuffs->fbc_mnmc.start = SOC_SAND_RNG_NEXT(dbuffs->fbc_fmc.end);
    dbuffs->fbc_mnmc.end = SOC_SAND_RNG_LAST(dbuffs->fbc_mnmc.start, fbc_mnmc_size);
    dbuffs->fbc_mnmc.size = fbc_mnmc_size;

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_ocb_dram_buffer_conf_set(int unit) 
{
    int i;
    uint32  field_val;
    uint64 reg64_val;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs_bdries;
    ARAD_MGMT_INIT  *arad_init;

    SOCDNX_INIT_FUNC_DEFS;

    arad_init = &SOC_DPP_CONFIG(unit)->arad->init;
    dbuffs_bdries = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;
    
    /* General Configuration */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_INTERNAL_REUSEf, 1));

    /* Set DBuff Boundries */ 
    if (dbuffs_bdries->ocb_0.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, OCB_0_BUFF_PTR_STARTf, dbuffs_bdries->ocb_0.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, OCB_0_BUFF_PTR_ENDf, dbuffs_bdries->ocb_0.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, OCB_0_BUFF_PTR_STARTf, 0)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, OCB_0_BUFF_PTR_ENDf, 0)); 
    }

    if (dbuffs_bdries->ocb_1.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, OCB_1_BUFF_PTR_STARTf, dbuffs_bdries->ocb_1.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, OCB_1_BUFF_PTR_ENDf, dbuffs_bdries->ocb_1.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, OCB_1_BUFF_PTR_STARTf, 0)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, OCB_1_BUFF_PTR_ENDf, 0));
    }

    if (dbuffs_bdries->fmc_0.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, FULL_MULTICAST_0_DB_PTR_STARTf, dbuffs_bdries->fmc_0.start));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, FULL_MULTICAST_0_DB_PTR_ENDf, dbuffs_bdries->fmc_0.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, FULL_MULTICAST_0_DB_PTR_STARTf, 0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, FULL_MULTICAST_0_DB_PTR_ENDf, 0));    
    }

    if (dbuffs_bdries->fmc_1.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, FULL_MULTICAST_1_DB_PTR_STARTf, dbuffs_bdries->fmc_1.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, FULL_MULTICAST_1_DB_PTR_ENDf, dbuffs_bdries->fmc_1.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, FULL_MULTICAST_1_DB_PTR_STARTf, 0)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, FULL_MULTICAST_1_DB_PTR_ENDf, 0));
    }

    if (dbuffs_bdries->mnmc_0.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, MINI_MULTICAST_0_DB_PTR_STARTf, dbuffs_bdries->mnmc_0.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, MINI_MULTICAST_0_DB_PTR_ENDf, dbuffs_bdries->mnmc_0.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, MINI_MULTICAST_0_DB_PTR_STARTf, 0)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, MINI_MULTICAST_0_DB_PTR_ENDf, 0));
    }

    if (dbuffs_bdries->mnmc_1.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, MINI_MULTICAST_1_DB_PTR_STARTf, dbuffs_bdries->mnmc_1.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, MINI_MULTICAST_1_DB_PTR_ENDf, dbuffs_bdries->mnmc_1.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, MINI_MULTICAST_1_DB_PTR_STARTf, 0)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, MINI_MULTICAST_1_DB_PTR_ENDf, 0));
    }

    if (dbuffs_bdries->fbc_fmc.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf, dbuffs_bdries->fbc_fmc.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf, dbuffs_bdries->fbc_fmc.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf, 0)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf, 0));
    }

    if (dbuffs_bdries->fbc_mnmc.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf, dbuffs_bdries->fbc_mnmc.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf, dbuffs_bdries->fbc_mnmc.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf, 0)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf, 0));
    }

    /* Configure OCB and PDM banks*/

    /* The following OCB bank configuration assumes that Device uses all OCB resources and at start of buffer list */
    if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x00ff));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0xff00));

        for (i = 0; i < JER_OCB_BANK_NUM; i++) {
            SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, i, &reg64_val));
            field_val = dbuffs_bdries->ocb_0.start + (i * JER_OCB_BANK_SIZE);
            soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, field_val);
            field_val = dbuffs_bdries->ocb_0.start + ((i + 1) * JER_OCB_BANK_SIZE) - 1;
            soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, field_val);
            SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, i, reg64_val));
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, FLUSCNT_BANK_SELECTf, 0xff00));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, MNUSCNT_BANK_SELECTf, 0xf0));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0x0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, FLUSCNT_BANK_SELECTf, 0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, MNUSCNT_BANK_SELECTf, 0));
    }

    if (dbuffs_bdries->mnmc_1.size != 0) {
        field_val = SOC_SAND_SET_BITS_RANGE(0xff, dbuffs_bdries->mnmc_1.end / (512 * 1024), dbuffs_bdries->mnmc_1.start / (512 * 1024)); 
    } else {
        field_val = 0;
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_BANK_SELECTr, REG_PORT_ANY, 0, BDB_BANK_SELECTf, field_val));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_BANK_SELECTr, REG_PORT_ANY, 0, PDM_BANK_SELECTf, field_val));

    /*
     * Configure Dram and OCB buffer sizes
     */
    switch(arad_init->dram.dbuff_size){
    case SOC_TMC_ITM_DBUFF_SIZE_BYTES_256:
        field_val = 0;
        break;
    case SOC_TMC_ITM_DBUFF_SIZE_BYTES_512:
        field_val = 1;
        break;
    case SOC_TMC_ITM_DBUFF_SIZE_BYTES_1024:
        field_val = 2;
        break;
    case SOC_TMC_ITM_DBUFF_SIZE_BYTES_2048:
        field_val = 3;
        break;
    default:
        field_val = 2;
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_GLOBAL_GENERAL_CFG_3r, REG_PORT_ANY, 0, DRAM_BUFF_SIZEf, field_val));
    
    switch(arad_init->ocb.databuffer_size){
    case 256:
        field_val = 0;
        break;
    case 512:
        field_val = 1;
        break;
    default:
        field_val = 0;
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_GLOBAL_GENERAL_CFG_3r, REG_PORT_ANY, 0, OCB_BUFF_SIZEf, field_val));


    /*
     * AutoGen
     */
    if (arad_init->ocb.ocb_enable != OCB_DISABLED) {
        field_val = 0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_0_AUTOGEN_ENABLEf, field_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_1_AUTOGEN_ENABLEf, field_val));

        field_val = (dbuffs_bdries->ocb_0.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_0_AUTOGEN_ENABLEf, field_val));

        field_val = (dbuffs_bdries->ocb_1.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_1_AUTOGEN_ENABLEf, field_val));

        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, OCB_0_AUTOGEN_DONEf, 0x1));
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, OCB_1_AUTOGEN_DONEf, 0x1));
    }
    if (arad_init->drc_info.dram_num != 0) {
        field_val = (dbuffs_bdries->mnmc_0.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_0_AUTOGEN_ENABLEf, field_val));

        field_val = (dbuffs_bdries->mnmc_1.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_1_AUTOGEN_ENABLEf, field_val));

        field_val = (dbuffs_bdries->fmc_0.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_0_AUTOGEN_ENABLEf, field_val));

        field_val = (dbuffs_bdries->fmc_1.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_1_AUTOGEN_ENABLEf, field_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_ocb_conf_set(int unit) 
{

    int i;
    uint64 table_entry,
            reg64_val;
    uint32 table_entry_array[2];

    SOCDNX_INIT_FUNC_DEFS;
    
    /* Configure OCB block */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, OCB_GENERAL_CONFIGr, REG_PORT_ANY, 0, RD_ADDR_FIFO_ALMOST_FULL_THf, 0xa));

    /* configuring OCB Dram-Mix defaults */ 
    COMPILER_64_ZERO(table_entry);
    soc_mem_field32_set(unit, IQM_OCBPRMm, &table_entry, OCB_QUE_BUFF_SIZE_EN_TH_0f, 0x17f);
    soc_mem_field32_set(unit, IQM_OCBPRMm, &table_entry, OCB_QUE_BUFF_SIZE_EN_TH_1f, 0x17f);
    soc_mem_field32_set(unit, IQM_OCBPRMm, &table_entry, OCB_QUE_SIZE_EN_TH_0f, 0x17f);
    soc_mem_field32_set(unit, IQM_OCBPRMm, &table_entry, OCB_QUE_SIZE_EN_TH_1f, 0x17f);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IQM_OCBPRMm, MEM_BLOCK_ANY, &table_entry));
    
    /* IDR_QUEUE_IS_DRAM_ELIGIBLE and IDR_QUEUE_IS_OCB_ELIGIBLE are dynamic so we need to enable dynamic table writes to write them. */
    /* Enable dynamic access. */
    SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1));

    /* Indicate all Queue are eligable for OCB for the first packet. setting dynamic tables : IDR_QUEUE_IS_DRAM_ELIGIBLE, IDR_QUEUE_IS_OCB_ELIGIBLE */
    table_entry_array[1] = table_entry_array[0] = 0;
    soc_mem_field32_set(unit, IDR_QUEUE_IS_DRAM_ELIGIBLEm, &table_entry_array, ELIGIBILITYf, 0xFFFFFFFF);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IDR_QUEUE_IS_DRAM_ELIGIBLEm, 0, 1, MEM_BLOCK_ANY, 0, 3071, &table_entry));

    table_entry_array[1] = table_entry_array[0] = 0;
    soc_mem_field32_set(unit, IDR_QUEUE_IS_OCB_ELIGIBLEm, &table_entry_array, ELIGIBILITYf, 0xFFFFFFFF);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IDR_QUEUE_IS_OCB_ELIGIBLEm, 0, 1, MEM_BLOCK_ANY, 0, 3071, &table_entry));

    /* Disable dynamic access. */
    SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 0));

    /* Enable DRAM-Mix OCB shaper to maximum value - No SW configuration for this Shaper */
    for (i = 0; i < 2; i++) {
        /* IDR_OCB_USE_SHAPERr is an array REG */
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IDR_OCB_USE_SHAPERr, REG_PORT_ANY, i, &reg64_val));
        soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_ENABLEf, 0x1);
        soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_TIMER_CYCLESf, 0x0);
        soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_GRANT_SIZEf, 0x100);
        soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_MAX_BURSTf, 0xffff);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, IDR_OCB_USE_SHAPERr, REG_PORT_ANY, i, reg64_val));
    }

    /* Enable a DRAM buffer to be used instead of an OCB buffer when there are no OCB buffers available for an OCB-Mixed packet */
    SOCDNX_IF_ERR_EXIT(READ_IDR_BUFFER_CHANGE_CONFIGURATIONr(unit, &reg64_val));
    soc_reg64_field32_set(unit, IDR_BUFFER_CHANGE_CONFIGURATIONr, &reg64_val, ENABLE_BUFFER_CHANGEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_IDR_BUFFER_CHANGE_CONFIGURATIONr(unit, reg64_val));

    /* Disable timeout mechanism - still performs recycle but without time out, time out was here for debug purposes */
    SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IDR_DRAM_FIFO_RECYCLE_TIMEOUT_CONFIGURATIONr, REG_PORT_ANY, RECYCLE_TIMEOUT_ENABLEf, 0));

    /* Setting OCB Multicast ranges */
    for (i = 0; i < 2; i++) {
        /* IDR_OCB_ELIGIBLE_MULTICAST_RANGEr and IDR_OCB_COMMITTED_MULTICAST_RANGEr are array REGs */
        /* DRAM-Mix OCB */
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, i, &reg64_val));
        soc_reg64_field32_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, &reg64_val, OCB_ELIGIBLE_MULTICAST_RANGE_N_LOWf, 0x0);
        soc_reg64_field32_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, &reg64_val, OCB_ELIGIBLE_MULTICAST_RANGE_N_HIGHf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, i, reg64_val));

        /* OCB-Only */
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, i, &reg64_val));
        soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, 0x0);
        soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, i, reg64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

