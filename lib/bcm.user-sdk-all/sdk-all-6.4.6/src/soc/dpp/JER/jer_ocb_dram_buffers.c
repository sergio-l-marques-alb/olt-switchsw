/*
 * $Id: jer_drv.c Exp $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * Defines
 */
#define JER_OCB_DEFAULT_OCB_ONLY_MAX_TO_HIGH_TH_DIFFERENCE 800
#define JER_OCB_DEFAULT_OCB_ONLY_MAX_TO_LOW_TH_DIFFERENCE 900
#define JER_OCB_DEFAULT_OCB_MIXED_PERCENTAGE_OF_HIGH_TH_FROM_MAX 81/100
#define JER_OCB_DEFAULT_OCB_MIXED_PERCENTAGE_OF_LOW_TH_FROM_MAX 80/100

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
        ocb_0_size = 0x0, fmc_0_size = 0x0, mnmc_0_size = 0x0, fbc_fmc_0_size = 0x0, fbc_mnmc_0_size = 0x0,
        ocb_1_size = 0x0, fmc_1_size = 0x0, mnmc_1_size = 0x0, fbc_fmc_1_size = 0x0, fbc_mnmc_1_size = 0x0,
        mnmc_total_size = 0x0, fmc_ocb_total_size = 0x0, fmc_ocb_remainder_to_512k_alignment = 0x0, mnmc_without_remainder_size = 0x0, mnmc_aligned_to_1024k_size = 0x0,
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
        fmc_ocb_total_size = ocb_0_size + fmc_0_size;
        mnmc_total_size = max_nof_dram_buffers - fmc_ocb_total_size;
    }
    if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
        ocb_0_size /= 2;
        fmc_0_size /= 2;
        ocb_1_size = ocb_0_size;
        fmc_1_size = fmc_0_size;

        /* Transition between MNMC_0 and MNMC_1 should be on 512K alignments to allow legal PDM banks distribution */
        /* | ...ocb_size... | ...fmc_size... | ...fmc_ocb_remainder_to_512k_alignment... | ................................mnmc_aligned_to_1024k_size.............................. | remainder to total size | */
        /* | .......fmc_ocb_total_size...... | .......................................................................mnmc_total_size........................................................................ | */
        /* | .......fmc_ocb_total_size...... | .....................................mnmc_0_size.................................... | ...............................mnmc_1_size............................. | */
        fmc_ocb_remainder_to_512k_alignment = (fmc_ocb_total_size % (512 * 1024)) ? (512 * 1024) - (fmc_ocb_total_size % (512 * 1024)) : 0 ;
        mnmc_without_remainder_size = mnmc_total_size - fmc_ocb_remainder_to_512k_alignment;
        mnmc_aligned_to_1024k_size = mnmc_without_remainder_size - (mnmc_without_remainder_size % (1024 * 1024));
        mnmc_0_size = mnmc_aligned_to_1024k_size / 2 + fmc_ocb_remainder_to_512k_alignment;
        mnmc_1_size = mnmc_total_size - mnmc_0_size;
    } else {
        ocb_1_size = 0;
        mnmc_0_size = mnmc_total_size;
        mnmc_1_size = 0;
        fmc_1_size = 0;
    }

    /* OCB_0 + FMC_0 and OCB_1 + FMC_1 needs to be on 16k alignment */
    if ((ocb_0_size + fmc_0_size) % (16 * 1024) != 0 || (ocb_1_size + fmc_1_size) % (16 * 1024) != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Size of OCB-0 + FMC-0 (=%d) and OCB-1 + FMC-1 (=%d) should be on 16K alignments.\n"),
                                                          (ocb_0_size + fmc_0_size),(ocb_1_size + fmc_1_size) ));
    }

    LOG_VERBOSE(BSL_LS_SOC_INIT, (BSL_META_U(unit, "unit %d: OCB-0 size: %d, OCB-1 size: %d, MNMC-0 size: %d, MNMC-1 size: %d, FMC-0 size: %d, FMC-1 size: %d\n"),
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
    /* Validate transition between MNMC_0 to MNMC_1 is aligned to 512k to fit into PDM banks */
    if (dbuffs->mnmc_1.start % (512 * 1024) != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("MNMC-0 (=%d) and MNMC-1 (=%d) should be on 512K alignments.\n"), mnmc_0_size, mnmc_1_size));
    }
    dbuffs->mnmc_1.end = SOC_SAND_RNG_LAST(dbuffs->mnmc_1.start, mnmc_1_size);
    dbuffs->mnmc_1.size = mnmc_1_size;

    /*
     *  FBC
     */ 
    if (arad_init->dram.nof_drams == 0) {
        fbc_fmc_0_size = 0;
        fbc_fmc_1_size = 0;
        fbc_mnmc_0_size = 0;
        fbc_mnmc_1_size = 0;
    } else {
        rv = arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->fmc_0.end, dbuffs->fmc_0.start), arad_init->dram.dbuff_size, &fbc_fmc_0_size); 
        SOCDNX_IF_ERR_EXIT(handle_sand_result(rv));
        /* if fmc_0_size is 0, the fbc should be 0 as well and not 1 */
        fbc_fmc_0_size = (fmc_0_size == 0) ? 0 : fbc_fmc_0_size ;

        rv = arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->fmc_1.end, dbuffs->fmc_1.start), arad_init->dram.dbuff_size, &fbc_fmc_1_size); 
        SOCDNX_IF_ERR_EXIT(handle_sand_result(rv));
        /* if fmc_1_size is 0, the fbc should be 0 as well and not 1 */
        fbc_fmc_1_size = (fmc_0_size == 0) ? 0 : fbc_fmc_1_size ;

        rv = arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->mnmc_0.end, dbuffs->mnmc_0.start), arad_init->dram.dbuff_size, &fbc_mnmc_0_size);
        SOCDNX_IF_ERR_EXIT(handle_sand_result(rv));
        rv = arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->mnmc_1.end, dbuffs->mnmc_1.start), arad_init->dram.dbuff_size, &fbc_mnmc_1_size);
        SOCDNX_IF_ERR_EXIT(handle_sand_result(rv));
    }

    /* FBC Full Multicast - core 0 */
    dbuffs->fbc_fmc_0.start = SOC_SAND_RNG_NEXT(dbuffs->mnmc_1.end);
    dbuffs->fbc_fmc_0.end = SOC_SAND_RNG_LAST(dbuffs->fbc_fmc_0.start, fbc_fmc_0_size);
    dbuffs->fbc_fmc_0.size = fbc_fmc_0_size;

    /* FBC Full Multicast - core 1 */
    dbuffs->fbc_fmc_1.start = SOC_SAND_RNG_NEXT(dbuffs->fbc_fmc_0.end);
    dbuffs->fbc_fmc_1.end = SOC_SAND_RNG_LAST(dbuffs->fbc_fmc_1.start, fbc_fmc_1_size);
    dbuffs->fbc_fmc_1.size = fbc_fmc_1_size;

    /* FBC Mini Multicast - core 1 */
    dbuffs->fbc_mnmc_0.start = SOC_SAND_RNG_NEXT(dbuffs->fbc_fmc_1.end);
    dbuffs->fbc_mnmc_0.end = SOC_SAND_RNG_LAST(dbuffs->fbc_mnmc_0.start, fbc_mnmc_0_size);
    dbuffs->fbc_mnmc_0.size = fbc_mnmc_0_size;

    /* FBC Mini Multicast - core 0 */
    dbuffs->fbc_mnmc_1.start = SOC_SAND_RNG_NEXT(dbuffs->fbc_mnmc_0.end);
    dbuffs->fbc_mnmc_1.end = SOC_SAND_RNG_LAST(dbuffs->fbc_mnmc_1.start, fbc_mnmc_1_size);
    dbuffs->fbc_mnmc_1.size = fbc_mnmc_1_size;

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_ocb_dram_buffer_conf_set(int unit) 
{
    int nof_banks_in_use = 0;
    int current_bank_index = 0;
    uint32  field_val;
    uint64 reg64_val;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs_bdries;
    ARAD_MGMT_INIT  *arad_init;
    ARAD_INIT_OCB *ocb;

    SOCDNX_INIT_FUNC_DEFS;

    arad_init = &SOC_DPP_CONFIG(unit)->arad->init;
    ocb = &arad_init->ocb;
    dbuffs_bdries = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;
    
    
    /* General Configuration */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_INTERNAL_REUSEf, 1));

    /* Set DBuff Boundries */ 
    if (dbuffs_bdries->ocb_0.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, OCB_0_BUFF_PTR_STARTf, dbuffs_bdries->ocb_0.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, OCB_0_BUFF_PTR_ENDf, dbuffs_bdries->ocb_0.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, OCB_0_BUFF_PTR_STARTf, 0x3fffff)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, OCB_0_BUFF_PTR_ENDf, 0x3fffff)); 
    }

    if (dbuffs_bdries->ocb_1.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, OCB_1_BUFF_PTR_STARTf, dbuffs_bdries->ocb_1.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, OCB_1_BUFF_PTR_ENDf, dbuffs_bdries->ocb_1.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, OCB_1_BUFF_PTR_STARTf, 0x3fffff)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, OCB_1_BUFF_PTR_ENDf, 0x3fffff));
    }

    if (dbuffs_bdries->fmc_0.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, FULL_MULTICAST_0_DB_PTR_STARTf, dbuffs_bdries->fmc_0.start));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, FULL_MULTICAST_0_DB_PTR_ENDf, dbuffs_bdries->fmc_0.end));
    } else {
        /* set to unreachable value */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, FULL_MULTICAST_0_DB_PTR_STARTf, 0x3fffff));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, FULL_MULTICAST_0_DB_PTR_ENDf, 0x3fffff));    
    }

    if (dbuffs_bdries->fmc_1.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, FULL_MULTICAST_1_DB_PTR_STARTf, dbuffs_bdries->fmc_1.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, FULL_MULTICAST_1_DB_PTR_ENDf, dbuffs_bdries->fmc_1.end));
    } else {
        /* set to unreachable value */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, FULL_MULTICAST_1_DB_PTR_STARTf, 0x3fffff)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, FULL_MULTICAST_1_DB_PTR_ENDf, 0x3fffff));
    }

    if (dbuffs_bdries->mnmc_0.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, MINI_MULTICAST_0_DB_PTR_STARTf, dbuffs_bdries->mnmc_0.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, MINI_MULTICAST_0_DB_PTR_ENDf, dbuffs_bdries->mnmc_0.end));
    } else {
        /* set to unreachable value */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, MINI_MULTICAST_0_DB_PTR_STARTf, 0x3fffff)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, MINI_MULTICAST_0_DB_PTR_ENDf, 0x3fffff));
    }

    if (dbuffs_bdries->mnmc_1.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, MINI_MULTICAST_1_DB_PTR_STARTf, dbuffs_bdries->mnmc_1.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, MINI_MULTICAST_1_DB_PTR_ENDf, dbuffs_bdries->mnmc_1.end));
    } else {
        /* set to unreachable value */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, MINI_MULTICAST_1_DB_PTR_STARTf, 0x3fffff)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, MINI_MULTICAST_1_DB_PTR_ENDf, 0x3fffff));
    }

    if (dbuffs_bdries->fbc_fmc_0.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf, dbuffs_bdries->fbc_fmc_0.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf, dbuffs_bdries->fbc_fmc_0.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf, 0x3fffff)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf, 0x3fffff));
    }

    if (dbuffs_bdries->fbc_fmc_1.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf, dbuffs_bdries->fbc_fmc_1.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf, dbuffs_bdries->fbc_fmc_1.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf, 0x3fffff)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf, 0x3fffff));
    }

    if (dbuffs_bdries->fbc_mnmc_0.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf, dbuffs_bdries->fbc_mnmc_0.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf, dbuffs_bdries->fbc_mnmc_0.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf, 0x3fffff)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf, 0x3fffff));
    }

    if (dbuffs_bdries->fbc_mnmc_1.size != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf, dbuffs_bdries->fbc_mnmc_1.start)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf, dbuffs_bdries->fbc_mnmc_1.end));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf, 0x3fffff)); 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf, 0x3fffff));
    }

    /* configure Dram reject multicast range to invalid range */
    COMPILER_64_ZERO(reg64_val);
    soc_reg64_field32_set(unit, IDR_DRAM_REJECT_MULTICAST_RANGEr, &reg64_val, DRAM_REJECT_MULTICAST_RANGE_N_LOWf, 1);
    soc_reg64_field32_set(unit, IDR_DRAM_REJECT_MULTICAST_RANGEr, &reg64_val, DRAM_REJECT_MULTICAST_RANGE_N_HIGHf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_IDR_DRAM_REJECT_MULTICAST_RANGEr(unit, 0, reg64_val));
    SOCDNX_IF_ERR_EXIT(WRITE_IDR_DRAM_REJECT_MULTICAST_RANGEr(unit, 1, reg64_val));

    /* The following OCB bank configuration assumes that Device uses all OCB resources and at start of buffer list */
    if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
        if( ocb->ocb_enable == OCB_ENABLED){
            if (ocb->databuffer_size == 256) {
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x00ff)); 
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0xff00));

                for (current_bank_index = 0; current_bank_index < JER_OCB_BANK_NUM; current_bank_index++) {
                    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                    field_val = dbuffs_bdries->ocb_0.start + (current_bank_index * JER_OCB_BANK_SIZE);
                    soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, field_val);
                    field_val = dbuffs_bdries->ocb_0.start + ((current_bank_index + 1) * JER_OCB_BANK_SIZE) - 1;
                    soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, field_val);
                    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
                }
            } else {  
                /* ocb->databuffer_size == 512 */
                /* when data buffer size is 512 only even banks are used */
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0055));
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0x5500));

                for (nof_banks_in_use = 0, current_bank_index = 0; current_bank_index < JER_OCB_BANK_NUM; current_bank_index += 2, ++nof_banks_in_use) {
                    /* update only even banks */
                    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                    field_val = dbuffs_bdries->ocb_0.start + (nof_banks_in_use * JER_OCB_BANK_SIZE);
                    soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, field_val);
                    field_val = dbuffs_bdries->ocb_0.start + ((nof_banks_in_use + 1) * JER_OCB_BANK_SIZE) - 1;
                    soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, field_val);
                    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
                }
            }
        } else {
            /* ocb->ocb_enable == OCB_DISABLED */
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0000)); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0000));

            for (current_bank_index = 0; current_bank_index < JER_OCB_BANK_NUM; current_bank_index++) {
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, 0x3fffff);
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, 0x3fffff);
                SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
            }
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, FLUSCNT_BANK_SELECTf, 0xff00));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, MNUSCNT_BANK_SELECTf, 0xf0));
    } else {
        if( ocb->ocb_enable == OCB_ENABLED){
            /* due to a hard design limitation only even banks are used in single core mode (0,2,4,?) */
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x5555));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0000));

            for (nof_banks_in_use = 0, current_bank_index = 0; current_bank_index < JER_OCB_BANK_NUM; current_bank_index += 2, ++nof_banks_in_use) {
                /* update only even banks */
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                field_val = dbuffs_bdries->ocb_0.start + (nof_banks_in_use * JER_OCB_BANK_SIZE);
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, field_val);
                field_val = dbuffs_bdries->ocb_0.start + ((nof_banks_in_use + 1) * JER_OCB_BANK_SIZE) - 1;
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, field_val);
                SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
            }

            for (current_bank_index = 1; current_bank_index < JER_OCB_BANK_NUM; current_bank_index += 2) {
                /* update only odd banks to invalid ranges*/
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, 0x3fffff);
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, 0x3fffff);
                SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
            }
        } else {
            /* ocb->ocb_enable == OCB_DISABLED */
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 0, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0000));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, 1, FBC_BANK_IS_USED_BY_OCB_Nf, 0x0000));

            for (current_bank_index = 0; current_bank_index < JER_OCB_BANK_NUM; ++current_bank_index) {
                /* update only odd banks to invalid ranges*/
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, &reg64_val));
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_START_BANK_Nf, 0x3fffff);
                soc_reg64_field32_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, &reg64_val, OCB_BUFF_END_BANK_Nf, 0x3fffff);
                SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, current_bank_index, reg64_val));
            }
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, FLUSCNT_BANK_SELECTf, 0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, MNUSCNT_BANK_SELECTf, 0));
    }

    field_val = 0x0;
    if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
        if (dbuffs_bdries->mnmc_1.size == 0) {
            field_val = 0x38; /* Allocate PDM banks for OCB even if there is no MNMC buffers */
        } else {
            field_val = SOC_SAND_SET_BITS_RANGE(0xff, dbuffs_bdries->mnmc_1.end / (512 * 1024), dbuffs_bdries->mnmc_1.start / (512 * 1024));
        }
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
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_GENERAL_CFG_3r, REG_PORT_ANY, 0, DRAM_BUFF_SIZEf, field_val));
    
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
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_GENERAL_CFG_3r, REG_PORT_ANY, 0, OCB_BUFF_SIZEf, field_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_ocb_dram_buffer_autogen_set(int unit) 
{
    
    uint32  field_val;
    ARAD_MGMT_INIT  *arad_init;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs_bdries;

    SOCDNX_INIT_FUNC_DEFS;

    arad_init = &SOC_DPP_CONFIG(unit)->arad->init;
    dbuffs_bdries = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;

    /*
     * AutoGen
     */
    if (arad_init->ocb.ocb_enable != OCB_DISABLED) {
        field_val = 0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_0_AUTOGEN_ENABLEf, field_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_1_AUTOGEN_ENABLEf, field_val));

        if (dbuffs_bdries->ocb_0.size != 0) {
            field_val = 0x1;
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_0_AUTOGEN_ENABLEf, field_val));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, OCB_0_AUTOGEN_DONEf, 0x1));
        }

        if (dbuffs_bdries->ocb_1.size != 0) {
            field_val = 0x1;
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_1_AUTOGEN_ENABLEf, field_val));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, OCB_1_AUTOGEN_DONEf, 0x1));
        }        
    }
    if (arad_init->drc_info.dram_num != 0) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_0_AUTOGEN_ENABLEf, 0x0));
        field_val = (dbuffs_bdries->mnmc_0.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_0_AUTOGEN_ENABLEf, field_val));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_1_AUTOGEN_ENABLEf, 0x0));
        field_val = (dbuffs_bdries->mnmc_1.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_1_AUTOGEN_ENABLEf, field_val));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_0_AUTOGEN_ENABLEf, 0x0));
        field_val = (dbuffs_bdries->fmc_0.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_0_AUTOGEN_ENABLEf, field_val));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_1_AUTOGEN_ENABLEf, 0x0));
        field_val = (dbuffs_bdries->fmc_1.size != 0)? 0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_1_AUTOGEN_ENABLEf, field_val));
    } else {
        /* When there is no active DRAM, configure all the reassembly contexts as OCB only. This allows recycled packets
         * whose trap destination is anon existent queue to be OCB eligable and enables outbound mirroring in this case.
         */
        soc_reg_above_64_val_t r = {0};
        r[0] = r[1] = r[2] = r[3] = r[4] = r[5] = r[6] = r[7] = r[8] = r[9] = 0xffffffff;
        r[10]=0x3ff; /* Set the 330 bits/reassembly contexts to OCB only */
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_REASSEMBLY_CONTEXT_IS_OCB_ONLYr(unit, r));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_ocb_conf_set(int unit) 
{

    int i;
    uint64 reg64_val;
    uint32 table_entry[2] = {0};
    soc_reg_above_64_val_t reg_above_64;
    uint32 core = 0, nof_cores = 0, max_nof_ocb_dbuffs = 0;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs_bdries;
    ARAD_INIT_OCB *ocb;
    soc_dpp_drc_combo28_info_t *drc_info;

    SOCDNX_INIT_FUNC_DEFS;
    
    nof_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    dbuffs_bdries = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;
    ocb = &SOC_DPP_CONFIG(unit)->arad->init.ocb;
    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;
        
    /* Configure OCB block */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, OCB_GENERAL_CONFIGr, REG_PORT_ANY, 0, RD_ADDR_FIFO_ALMOST_FULL_THf, 0xa));

    /* configuring OCB Dram-Mix defaults */ 
    soc_mem_field32_set(unit, IQM_OCBPRMm, table_entry, OCB_QUE_BUFF_SIZE_EN_TH_0f, 0x17f);
    soc_mem_field32_set(unit, IQM_OCBPRMm, table_entry, OCB_QUE_BUFF_SIZE_EN_TH_1f, 0x17f);
    soc_mem_field32_set(unit, IQM_OCBPRMm, table_entry, OCB_QUE_SIZE_EN_TH_0f, 0x17f);
    soc_mem_field32_set(unit, IQM_OCBPRMm, table_entry, OCB_QUE_SIZE_EN_TH_1f, 0x17f);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IQM_OCBPRMm, MEM_BLOCK_ANY, table_entry));
    
    /* IDR_QUEUE_IS_DRAM_ELIGIBLE and IDR_QUEUE_IS_OCB_ELIGIBLE are dynamic so we need to enable dynamic table writes to write them. */
    /* Enable dynamic access. */
    SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1));

    /* Indicate all Queue are eligable for OCB for the first packet. setting dynamic tables : IDR_QUEUE_IS_DRAM_ELIGIBLE, IDR_QUEUE_IS_OCB_ELIGIBLE */
    table_entry[1] = table_entry[0] = 0;
    soc_mem_field32_set(unit, IDR_QUEUE_IS_DRAM_ELIGIBLEm, table_entry, ELIGIBILITYf, 0xFFFFFFFF);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IDR_QUEUE_IS_DRAM_ELIGIBLEm, 0, 1, MEM_BLOCK_ANY, 0, 3071, table_entry));

    table_entry[1] = table_entry[0] = 0;
    soc_mem_field32_set(unit, IDR_QUEUE_IS_OCB_ELIGIBLEm, table_entry, ELIGIBILITYf, 0xFFFFFFFF);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IDR_QUEUE_IS_OCB_ELIGIBLEm, 0, 1, MEM_BLOCK_ANY, 0, 3071, table_entry));

    /* Disable dynamic access. */
    SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 0));

    /* Enable DRAM-Mix OCB shaper to maximum value - No SW configuration for this Shaper */
    for (i = 0; i < 2; i++) {
        /* IDR_OCB_USE_SHAPERr is an array REG */
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IDR_OCB_USE_SHAPERr, REG_PORT_ANY, i, &reg64_val));
        if (ocb->ocb_enable == OCB_ENABLED) {
            soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_ENABLEf, 0x1);
            soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_TIMER_CYCLESf, 0x0);
            soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_GRANT_SIZEf, 0x100);
            soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_MAX_BURSTf, 0xffff);
        } else {
            soc_reg64_field32_set(unit, IDR_OCB_USE_SHAPERr, &reg64_val, OCB_N_SHAPER_ENABLEf, 0x0);
        }
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
        if (ocb->ocb_enable == OCB_ENABLED) {
            if (drc_info->dram_num != 0) {
                soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, 0x1); 
                soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, 0x0);
            } else {
                soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, 0x0); 
                soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, 0x17fff);
            }
        } else {
            /* If OCB is disabled, set range so that low is higher than high */
            soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, 0x1);
            soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, 0x0);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, i, reg64_val));
    }

    /* if OCB is available */
    if( ocb->ocb_enable == OCB_ENABLED)
    {
        /* Configure Dram and OCB Buffers threshold */
        for (core = 0; core < nof_cores; ++core) 
        {
            max_nof_ocb_dbuffs = (core == 0) ? dbuffs_bdries->ocb_0.size : dbuffs_bdries->ocb_1.size;
            if (max_nof_ocb_dbuffs < JER_OCB_DEFAULT_OCB_ONLY_MAX_TO_LOW_TH_DIFFERENCE) 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("available OCB size is to small to be configured\n")));
            }

            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64)); 
            soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_ONLY_DB_OCC_TH_Hf, max_nof_ocb_dbuffs - JER_OCB_DEFAULT_OCB_ONLY_MAX_TO_HIGH_TH_DIFFERENCE); 
            soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_ONLY_DB_OCC_TH_Lf, max_nof_ocb_dbuffs - JER_OCB_DEFAULT_OCB_ONLY_MAX_TO_LOW_TH_DIFFERENCE); 
            soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Hf, max_nof_ocb_dbuffs * JER_OCB_DEFAULT_OCB_MIXED_PERCENTAGE_OF_HIGH_TH_FROM_MAX); 
            soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Lf, max_nof_ocb_dbuffs * JER_OCB_DEFAULT_OCB_MIXED_PERCENTAGE_OF_LOW_TH_FROM_MAX); 
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64)); 
        }
    } else {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
        WRITE_IDR_REASSEMBLY_CONTEXT_IS_OCB_ONLYr(unit, reg_above_64);
        WRITE_IDR_TRAFFIC_CLASS_IS_OCB_ONLYr(unit, 0, 0x0);
        WRITE_IDR_TRAFFIC_CLASS_IS_OCB_ONLYr(unit, 1, 0x0);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* return 1 if the value fits the field size */

/*
 * Function:
 *      jer_ocb_validate_value_fit_field_size
 * Purpose:
 *      checking if a value fits inside a field on a reg.
 * Parameters:
 *      unit    - Device Number
 *      reg     - reg to look for field in
 *      field   - field to check for fit in size
 *      value   - value to fit in field
 * Returns:
 *      0 - on failure
 *      1 - on success
 */
STATIC int jer_ocb_validate_value_fit_field_size(int unit, soc_reg_t reg, soc_field_t field, int value)
{
    return !(value >> soc_reg_field_length(unit, reg, field)) ;
}



/*
 * Function:
 *      soc_jer_ocb_control_range_dram_mix_dbuff_threshold_set
 * Purpose:
 *      set the ocb dram mix thresholds
 * Parameters:
 *      unit      - Device Number
 *      core      - core number
 *      range_max - high threshold value
 *      range_min - low threshold value
 * Returns:
 *      SOC_E_XXX 
 */
int soc_jer_ocb_control_range_dram_mix_dbuff_threshold_set( SOC_SAND_IN int unit,   SOC_SAND_IN int core,   SOC_SAND_IN int range_max,   SOC_SAND_IN int range_min)
{
    soc_reg_above_64_val_t reg_above_64;
    SOCDNX_INIT_FUNC_DEFS;

    /* Check size of thresholds according to fields */
    if( (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, OCB_N_MIXED_DB_OCC_TH_Hf, range_max)) ||
        (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, OCB_N_MIXED_DB_OCC_TH_Lf, range_min))    )
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("range_max = %d, range_min = %d, out of allowed range\n"), range_max, range_min));
    }

    /* Set Dram mix dbuff threshold */
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64));
    soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Hf, range_max);
    soc_reg_above_64_field32_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Lf, range_min);
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_set(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_ocb_control_range_ocb_committed_multicast_set
 * Purpose:
 *      set the ocb committed multicast ID range.
 * Parameters:
 *      unit      - Device Number
 *      index     - set first or second range (index 0 or 1)
 *      range_max - highest multicast ID in group
 *      range_min - lowest multicast ID in group
 * Returns:
 *      SOC_E_XXX 
 */
int soc_jer_ocb_control_range_ocb_committed_multicast_set( SOC_SAND_IN int unit,   SOC_SAND_IN uint32 index,   SOC_SAND_IN int range_max,   SOC_SAND_IN int range_min)
{
    uint64 reg_64;
    SOCDNX_INIT_FUNC_DEFS;

    /* Check size of thresholds according to fields */
    if( (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, range_max)) ||
        (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, range_min))    )
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("range_max = %d, range_min = %d, out of allowed range\n"), range_max, range_min));
    }

    /* Get Multicast OCB committed range */
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, index, &reg_64));
    soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg_64, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, range_max);
    soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg_64, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf, range_min);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, index, reg_64));

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_ocb_control_range_ocb_eligible_multicast_set
 * Purpose:
 *      set the ocb eligible multicast ID range.
 * Parameters:
 *      unit      - Device Number
 *      index     - get first or second range (index 0 or 1)
 *      range_max - highest multicast ID in group
 *      range_min - lowest multicast ID in group
 * Returns:
 *      SOC_E_XXX 
 */
int soc_jer_ocb_control_range_ocb_eligible_multicast_set( SOC_SAND_IN int unit,   SOC_SAND_IN uint32 index,   SOC_SAND_IN int range_max,   SOC_SAND_IN int range_min)
{
    uint64 reg_64;
    SOCDNX_INIT_FUNC_DEFS;

    /* Check size of thresholds according to fields */
    if( (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, OCB_ELIGIBLE_MULTICAST_RANGE_N_HIGHf, range_max)) ||
        (0 == jer_ocb_validate_value_fit_field_size(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, OCB_ELIGIBLE_MULTICAST_RANGE_N_LOWf, range_min))    )
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("range_max = %d, range_min = %d, out of allowed range\n"), range_max, range_min));
    }

    /* Get Multicast OCB eligible range */
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, index, &reg_64));
    soc_reg64_field32_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, &reg_64, OCB_ELIGIBLE_MULTICAST_RANGE_N_HIGHf, range_max);
    soc_reg64_field32_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, &reg_64, OCB_ELIGIBLE_MULTICAST_RANGE_N_LOWf, range_min);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, index, reg_64));

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_ocb_control_range_dram_mix_dbuff_threshold_get
 * Purpose:
 *      get the ocb dram mix thresholds
 * Parameters:
 *      unit      - Device Number
 *      core      - core number
 *      range_max - high threshold value (returned value)
 *      range_min - low threshold value  (returned value)
 * Returns:
 *      SOC_E_XXX 
 */
int soc_jer_ocb_control_range_dram_mix_dbuff_threshold_get( SOC_SAND_IN int unit,  SOC_SAND_IN int core,  SOC_SAND_OUT int* range_max,  SOC_SAND_OUT int* range_min)
{
    soc_reg_above_64_val_t reg_above_64;
    SOCDNX_INIT_FUNC_DEFS;

    /* Get Dram mix dbuff threshold */
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, REG_PORT_ANY, core, reg_above_64));
    *range_max = soc_reg_above_64_field32_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Hf);
    *range_min = soc_reg_above_64_field32_get(unit, IDR_OCCUPIED_DBUFF_THRESHOLDr, reg_above_64, OCB_N_MIXED_DB_OCC_TH_Lf);

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_ocb_control_range_ocb_committed_multicast_get
 * Purpose:
 *      get the ocb committed multicast ID range.
 * Parameters:
 *      unit      - Device Number
 *      index     - get first or second range (index 0 or 1)
 *      range_max - highest multicast ID in group (returned value)
 *      range_min - lowest multicast ID in group (returned value)
 * Returns:
 *      SOC_E_XXX 
 */
int soc_jer_ocb_control_range_ocb_committed_multicast_get( SOC_SAND_IN int unit,  SOC_SAND_IN uint32 index,  SOC_SAND_OUT int* range_max,  SOC_SAND_OUT int* range_min)
{
    uint64 reg_64;
    SOCDNX_INIT_FUNC_DEFS;

    /* Get Multicast OCB committed range */
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, index, &reg_64));
    *range_max = soc_reg64_field32_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, reg_64, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf);
    *range_min = soc_reg64_field32_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, reg_64, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf);

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_ocb_control_range_ocb_eligible_multicast_get
 * Purpose:
 *      get the ocb eligible multicast ID range.
 * Parameters:
 *      unit      - Device Number
 *      index     - get first or second range (index 0 or 1)
 *      range_max - highest multicast ID in group (returned value)
 *      range_min - lowest multicast ID in group (returned value)
 * Returns:
 *      SOC_E_XXX 
 */
int soc_jer_ocb_control_range_ocb_eligible_multicast_get( SOC_SAND_IN int unit,  SOC_SAND_IN uint32 index,  SOC_SAND_OUT int* range_max,  SOC_SAND_OUT int* range_min)
{
    uint64 reg_64;
    SOCDNX_INIT_FUNC_DEFS;

    /* Get Multicast OCB eligible range */
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, index, &reg_64));
    *range_max = soc_reg64_field32_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, reg_64, OCB_ELIGIBLE_MULTICAST_RANGE_N_HIGHf);
    *range_min = soc_reg64_field32_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, reg_64, OCB_ELIGIBLE_MULTICAST_RANGE_N_LOWf);

exit:
    SOCDNX_FUNC_RETURN;
}


