/*
 * $Id: drc_combo28.c,v 1.1.2.13 Broadcom SDK $
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
#define _ERR_MSG_MODULE_NAME BSL_SOC_DRAM

/* 
 * Includes
 */ 
#include <shared/bsl.h>
/* SOC DCMN/DPP/SAND includes */
#include <soc/mem.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/error.h>

/* SOC DPP Arad includes */ 
#include <soc/dpp/ARAD/arad_chip_regs.h>

/* SOC DPP DRC includes */ 
#include <soc/dpp/DRC/drc_combo28.h>
#include <soc/dpp/DRC/drc_combo28_cb.h>
#include <soc/dpp/DRC/drc_combo28_init.h>

/* Defines */
#define SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY   SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE*SHMOO_COMBO28_BYTE
#define DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE 256
#define DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE 256
#define DRC_COMBO28_VAL_STR_BUFFER_SIZE 1024
#define DRC_COMBO28_DDR_PHY_REF_CLK 100

/*
 * Utility functions 
 */
/* CalmShell Bits swap function */
int soc_dpp_drc_combo28_util_calm_shell_bits_swap(int unit, uint32 in_buf, uint32 *out_buf)
{
    uint32 shadow_mr_addr_lcl;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(out_buf);
    
    shadow_mr_addr_lcl  = 0x0;
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 1, 1), 0,  0);    /* Take Bit1 from in_buf and set in Bit0 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 0, 0), 1,  1);    /* Take Bit0 from in_buf and set in Bit1 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 2, 2), 2,  2);    /* Take Bit2 from in_buf and set in Bit2 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 4, 4), 3,  3);    /* Take Bit4 from in_buf and set in Bit3 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 3, 3), 4,  4);    /* Take Bit3 from in_buf and set in Bit4 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 6, 6), 5,  5);    /* Take Bit6 from in_buf and set in Bit5 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 5, 5), 6,  6);    /* Take Bit5 from in_buf and set in Bit6 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 8, 8), 7,  7);    /* Take Bit8 from in_buf and set in Bit7 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 7, 7), 8,  8);    /* Take Bit7 from in_buf and set in Bit8 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 9, 9), 9,  9);    /* Take Bit9 from in_buf and set in Bit9 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 10, 10), 10, 10); /* Take Bit10 from in_buf and set in Bit10 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 13, 13), 11, 11); /* Take Bit13 from in_buf and set in Bit11 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 12, 12), 12, 12); /* Take Bit12 from in_buf and set in Bit12 of shadow */
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 11, 11), 13, 13); /* Take Bit11 from in_buf and set in Bit13 of shadow */

    *out_buf = shadow_mr_addr_lcl;

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): mr=%d, *shadow_mr_addr=%d.\n"),FUNCTION_NAME(), in_buf, *out_buf));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_dpp_drc_combo28_dram_cpu_command
 * Purpose:
 *      
 * Parameters:
 *      unit                - unit number
 *      drc_ndx             - DRC index
 *      ras_n          
 *      cas_n                
 *      we_n                
 *      act_n             
 *      bank
 *      address
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after a device is attached
 *      or whenever a chip reset is required.
 */
int soc_dpp_drc_combo28_dram_cpu_command(int unit, int drc_ndx, uint32 ras_n, uint32 cas_n, uint32 we_n, uint32 act_n, uint32 bank, uint32 address)
{
    uint32 
        abi_n=0,
        abi_n_odd=0,
        parity,
        addr,
        addr_odd,
        bank_odd,
        bus_for_abi,
        parity_bus,
        dram_type,
        field_data,
        clam_shell_mode,
        i;
    uint64 
        reg64_val,
        field64_data,
        field64_val;
    int cnt;
    static soc_reg_t
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr},
        DRC_CPU_COMMANDSl[] = {DRCA_CPU_COMMANDSr, DRCB_CPU_COMMANDSr, DRCC_CPU_COMMANDSr, 
                               DRCD_CPU_COMMANDSr, DRCE_CPU_COMMANDSr, DRCF_CPU_COMMANDSr, 
                               DRCG_CPU_COMMANDSr, DRCH_CPU_COMMANDSr},
        DRC_CLAM_SHELLl[] = {DRCA_CLAM_SHELLr, DRCB_CLAM_SHELLr, DRCC_CLAM_SHELLr,
                             DRCD_CLAM_SHELLr, DRCE_CLAM_SHELLr, DRCF_CLAM_SHELLr,
                             DRCG_CLAM_SHELLr, DRCH_CLAM_SHELLr},
        DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr,
                                        DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr,
                                        DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr};

    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, ras_n=%d, cas_n=%d, we_n=%d, act_n=%d, bank=0x%x, address=0x%x\n"), 
                            FUNCTION_NAME(), drc_ndx, ras_n, cas_n, we_n, act_n, bank, address)); 

    /* Set address & bank according to dram type */
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    dram_type = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, DRAM_TYPEf);
    if (SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4 == dram_type) {
        addr = SOC_SAND_SET_BITS_RANGE(address, 13, 0);

        /* for DDR4, odd command is used only for clamshell */
        soc_dpp_drc_combo28_util_calm_shell_bits_swap(unit, addr, &addr_odd);

        bank_odd = bank;
        bank_odd &= SOC_SAND_ZERO_BITS_MASK(1,0);
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(bank, 0, 0), 1,  1);
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(bank, 1, 1), 0,  0);

    } else {
        addr = SOC_SAND_GET_BITS_RANGE(address, 14, 8);
  
        addr_odd = 0;
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 7, 7), 0,  0);
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 1, 1), 1,  1);
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 0, 0), 2,  2);
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 6, 6), 3,  3);
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 13, 13), 4,  4);
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 15, 15), 5,  5);

        bank_odd = 0;
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 2, 2), 0,  0);
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 5, 5), 1,  1);
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 4, 4), 2,  2);
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 3, 3), 3,  3);

        /* calculate abi for even command */
        cnt = 0;
        abi_n = 1;
        bus_for_abi = 0;
        bus_for_abi |= SOC_SAND_SET_BITS_RANGE(addr, 13, 0);
        bus_for_abi |= SOC_SAND_SET_BITS_RANGE(bank, 17, 14);

        SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
        field64_val = soc_reg64_field_get(unit, DRCA_DRAM_SPECIAL_FEATURESr, reg64_val, ABIf);
        field_data = COMPILER_64_LO(field64_val);

        for (i = 0; i < 10; i++) {
            cnt = cnt + SOC_SAND_GET_BITS_RANGE(bus_for_abi,i ,i);
        }
        if ((cnt < 5) && (1 == field_data)) {
            addr = SOC_SAND_GET_BITS_RANGE(~addr, 13, 0);
            bank = SOC_SAND_GET_BITS_RANGE(~bank, 3, 0);
            abi_n = 0;
        }

        /* calculate abi for odd command */
        cnt = 0;
        abi_n_odd = 1;
        bus_for_abi = 0;
        bus_for_abi |= SOC_SAND_SET_BITS_RANGE(addr_odd, 13, 0);
        bus_for_abi |= SOC_SAND_SET_BITS_RANGE(bank_odd, 17, 14);
        for (i = 0; i < 10; i++) {
            cnt = cnt + SOC_SAND_GET_BITS_RANGE(bus_for_abi,i ,i);
        }
        if ((cnt < 5) && (1 == field_data)) {
            addr_odd = SOC_SAND_GET_BITS_RANGE(~addr_odd, 13, 0);
            bank_odd = SOC_SAND_GET_BITS_RANGE(~bank_odd, 3, 0);
            abi_n_odd = 0;
        } 
    }

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): addr=0x%x, addr_odd=0x%x, bank_odd=0x%x, abi_n_odd=%d, addr=0x%x, bank=0x%x, abi_n=%d\n"), 
                            FUNCTION_NAME(), addr, addr_odd, bank_odd, abi_n_odd, addr, bank, abi_n)); 

    /* calculate parity (relevant only for DDR4) */
    parity_bus = 0;
    parity_bus |= SOC_SAND_SET_BITS_RANGE(ras_n, 0, 0);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(cas_n, 1, 1);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(we_n, 2, 2);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(act_n, 3, 3);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(addr, 17, 4);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(bank, 21, 18);
    parity = 0;
    for (i=0 ; i < 23 ; i++) {
        parity ^= SOC_SAND_GET_BITS_RANGE(parity_bus, i ,i);
    }

    /* write everything to the register */
    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    COMPILER_64_SET(field64_val, 0x0, bank);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, BANK_ADDRESSf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, addr);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, ADDRESSf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, we_n);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, WE_Nf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, cas_n);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CAS_Nf,  field64_val);
    COMPILER_64_SET(field64_val, 0x0, ras_n);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, RAS_N_OR_REF_Nf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, parity);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, PARITYf,  field64_val);
    COMPILER_64_SET(field64_val, 0x0, ((dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) ? act_n : abi_n));
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, ACT_N_ABI_Nf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, bank_odd);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, BANK_ADDRESS_ODDf,  field64_val);
    COMPILER_64_SET(field64_val, 0x0, addr_odd);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, ADDRESS_ODDf,  field64_val);
    COMPILER_64_SET(field64_val, 0x0, we_n);    
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, WE_N_ODDf,  field64_val);
    COMPILER_64_SET(field64_val, 0x0, cas_n);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CAS_N_ODDf,  field64_val);
    COMPILER_64_SET(field64_val, 0x0, ras_n);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, RAS_N_OR_REF_N_ODDf,  field64_val);
    COMPILER_64_SET(field64_val, 0x0, parity);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, PARITY_ODDf,  field64_val);
    COMPILER_64_SET(field64_val, 0x0, ((dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) ? act_n : abi_n_odd));
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, ACT_N_ABI_N_ODDf,  field64_val);

    /* CS_N values */
    /* for clamshell we select one dram in the even command and the other in the odd command */
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_CLAM_SHELLl[drc_ndx], REG_PORT_ANY, 0, &field_data));
    clam_shell_mode = soc_reg_field_get(unit, DRCA_CLAM_SHELLr, field_data, CLAM_SHELL_MODEf);
    if (1 == clam_shell_mode) {
        field_data = soc_reg_field_get(unit, DRCA_CLAM_SHELLr, field_data, USE_CLAM_SHELL_DDR_0f);
        if (0x1 == field_data) {
            COMPILER_64_SET(field64_val, 0x0, 0x1);   
        } else {
            COMPILER_64_SET(field64_val, 0x0, 0x2);
        }
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CS_Nf, field64_val);
        if (0x1 == field_data) {
            COMPILER_64_SET(field64_val, 0x0, 0x2);   
        } else {
            COMPILER_64_SET(field64_val, 0x0, 0x1);
        }
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CS_N_ODDf, field64_val);

    } else { 
        /* Otherwise, select both dram on the even clk, and both or none in the odd one, according to drc frequency mode */
        COMPILER_64_ZERO(field64_val);
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CS_Nf, field64_val);
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &field64_data));
        field_data = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, field64_data, DRC_IN_HALF_DRAM_CMD_FREQ_MODEf);
        if (0x1 == field_data) {
            COMPILER_64_SET(field64_val, 0x0, 0x3);
        } else {
            COMPILER_64_ZERO(field64_val); 
        }
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CS_N_ODDf, field64_val);
    }

    /* Some constants */
    COMPILER_64_ZERO(field64_val);
    COMPILER_64_SET(field64_val, 0x0, ((dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) ? 0x1 : 0x0));
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CKEf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, 0x1); 
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, RESf, field64_val);
    COMPILER_64_ZERO(field64_val); 
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, ODTf, field64_val);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, ODT_ODDf, field64_val);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CONST_VALf, field64_val);
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CMD_CONST_VALf, field64_val);

    /* update the registers with all the values we've just set */ 
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    /* send command towards DRAM */
    COMPILER_64_ZERO(field64_val);    
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CPU_CMD_VLDf, field64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg64_val));
  
    COMPILER_64_SET(field64_val, 0x0, 0x1); 
    soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CPU_CMD_VLDf, field64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg64_val)); 

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Sent CPU command to DRAM.\n"), FUNCTION_NAME(), drc_ndx));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_load_mrs(int unit, int drc_ndx, uint32 mr_ndx, uint32 mrs_opcode)
{
    int 
        address,
        rv;
    uint32 reg_val, field_val;
    static soc_reg_t
        DRC_DRAM_MRl[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = 
            {{DRCA_DRAM_MR_0r, DRCB_DRAM_MR_0r, DRCC_DRAM_MR_0r, DRCD_DRAM_MR_0r, DRCE_DRAM_MR_0r, DRCF_DRAM_MR_0r, DRCG_DRAM_MR_0r, DRCH_DRAM_MR_0r},
             {DRCA_DRAM_MR_1r, DRCB_DRAM_MR_1r, DRCC_DRAM_MR_1r, DRCD_DRAM_MR_1r, DRCE_DRAM_MR_1r, DRCF_DRAM_MR_1r, DRCG_DRAM_MR_1r, DRCH_DRAM_MR_1r},
             {DRCA_DRAM_MR_2r, DRCB_DRAM_MR_2r, DRCC_DRAM_MR_2r, DRCD_DRAM_MR_2r, DRCE_DRAM_MR_2r, DRCF_DRAM_MR_2r, DRCG_DRAM_MR_2r, DRCH_DRAM_MR_2r},
             {DRCA_DRAM_MR_3r, DRCB_DRAM_MR_3r, DRCC_DRAM_MR_3r, DRCD_DRAM_MR_3r, DRCE_DRAM_MR_3r, DRCF_DRAM_MR_3r, DRCG_DRAM_MR_3r, DRCH_DRAM_MR_3r},
             {DRCA_DRAM_MR_4r, DRCB_DRAM_MR_4r, DRCC_DRAM_MR_4r, DRCD_DRAM_MR_4r, DRCE_DRAM_MR_4r, DRCF_DRAM_MR_4r, DRCG_DRAM_MR_4r, DRCH_DRAM_MR_4r},
             {DRCA_DRAM_MR_5r, DRCB_DRAM_MR_5r, DRCC_DRAM_MR_5r, DRCD_DRAM_MR_5r, DRCE_DRAM_MR_5r, DRCF_DRAM_MR_5r, DRCG_DRAM_MR_5r, DRCH_DRAM_MR_5r},
             {DRCA_DRAM_MR_6r, DRCB_DRAM_MR_6r, DRCC_DRAM_MR_6r, DRCD_DRAM_MR_6r, DRCE_DRAM_MR_6r, DRCF_DRAM_MR_6r, DRCG_DRAM_MR_6r, DRCH_DRAM_MR_6r},
             {DRCA_DRAM_MR_7r, DRCB_DRAM_MR_7r, DRCC_DRAM_MR_7r, DRCD_DRAM_MR_7r, DRCE_DRAM_MR_7r, DRCF_DRAM_MR_7r, DRCG_DRAM_MR_7r, DRCH_DRAM_MR_7r},
             {DRCA_DRAM_MR_8r, DRCB_DRAM_MR_8r, DRCC_DRAM_MR_8r, DRCD_DRAM_MR_8r, DRCE_DRAM_MR_8r, DRCF_DRAM_MR_8r, DRCG_DRAM_MR_8r, DRCH_DRAM_MR_8r},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_DRAM_MR_12r, DRCB_DRAM_MR_12r, DRCC_DRAM_MR_12r, DRCD_DRAM_MR_12r, DRCE_DRAM_MR_12r, DRCF_DRAM_MR_12r, DRCG_DRAM_MR_12r, DRCH_DRAM_MR_12r},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_DRAM_MR_14r, DRCB_DRAM_MR_14r, DRCC_DRAM_MR_14r, DRCD_DRAM_MR_14r, DRCE_DRAM_MR_14r, DRCF_DRAM_MR_14r, DRCG_DRAM_MR_14r, DRCH_DRAM_MR_14r},
             {DRCA_DRAM_MR_15r, DRCB_DRAM_MR_15r, DRCC_DRAM_MR_15r, DRCD_DRAM_MR_15r, DRCE_DRAM_MR_15r, DRCF_DRAM_MR_15r, DRCG_DRAM_MR_15r, DRCH_DRAM_MR_15r}},
        DRC_SHADOW_DRAM_MRl[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = 
             {{DRCA_SHADOW_DRAM_MR_0r, DRCB_SHADOW_DRAM_MR_0r, DRCC_SHADOW_DRAM_MR_0r, DRCD_SHADOW_DRAM_MR_0r, DRCE_SHADOW_DRAM_MR_0r, DRCF_SHADOW_DRAM_MR_0r, DRCG_SHADOW_DRAM_MR_0r, DRCH_SHADOW_DRAM_MR_0r},
             {DRCA_SHADOW_DRAM_MR_1r, DRCB_SHADOW_DRAM_MR_1r, DRCC_SHADOW_DRAM_MR_1r, DRCD_SHADOW_DRAM_MR_1r, DRCE_SHADOW_DRAM_MR_1r, DRCF_SHADOW_DRAM_MR_1r, DRCG_SHADOW_DRAM_MR_1r, DRCH_SHADOW_DRAM_MR_1r},
             {DRCA_SHADOW_DRAM_MR_2r, DRCB_SHADOW_DRAM_MR_2r, DRCC_SHADOW_DRAM_MR_2r, DRCD_SHADOW_DRAM_MR_2r, DRCE_SHADOW_DRAM_MR_2r, DRCF_SHADOW_DRAM_MR_2r, DRCG_SHADOW_DRAM_MR_2r, DRCH_SHADOW_DRAM_MR_2r},
             {DRCA_SHADOW_DRAM_MR_3r, DRCB_SHADOW_DRAM_MR_3r, DRCC_SHADOW_DRAM_MR_3r, DRCD_SHADOW_DRAM_MR_3r, DRCE_SHADOW_DRAM_MR_3r, DRCF_SHADOW_DRAM_MR_3r, DRCG_SHADOW_DRAM_MR_3r, DRCH_SHADOW_DRAM_MR_3r},
             {DRCA_SHADOW_DRAM_MR_4r, DRCB_SHADOW_DRAM_MR_4r, DRCC_SHADOW_DRAM_MR_4r, DRCD_SHADOW_DRAM_MR_4r, DRCE_SHADOW_DRAM_MR_4r, DRCF_SHADOW_DRAM_MR_4r, DRCG_SHADOW_DRAM_MR_4r, DRCH_SHADOW_DRAM_MR_4r},
             {DRCA_SHADOW_DRAM_MR_5r, DRCB_SHADOW_DRAM_MR_5r, DRCC_SHADOW_DRAM_MR_5r, DRCD_SHADOW_DRAM_MR_5r, DRCE_SHADOW_DRAM_MR_5r, DRCF_SHADOW_DRAM_MR_5r, DRCG_SHADOW_DRAM_MR_5r, DRCH_SHADOW_DRAM_MR_5r},
             {DRCA_SHADOW_DRAM_MR_6r, DRCB_SHADOW_DRAM_MR_6r, DRCC_SHADOW_DRAM_MR_6r, DRCD_SHADOW_DRAM_MR_6r, DRCE_SHADOW_DRAM_MR_6r, DRCF_SHADOW_DRAM_MR_6r, DRCG_SHADOW_DRAM_MR_6r, DRCH_SHADOW_DRAM_MR_6r},
             {DRCA_SHADOW_DRAM_MR_7r, DRCB_SHADOW_DRAM_MR_7r, DRCC_SHADOW_DRAM_MR_7r, DRCD_SHADOW_DRAM_MR_7r, DRCE_SHADOW_DRAM_MR_7r, DRCF_SHADOW_DRAM_MR_7r, DRCG_SHADOW_DRAM_MR_7r, DRCH_SHADOW_DRAM_MR_7r},
             {DRCA_SHADOW_DRAM_MR_8r, DRCB_SHADOW_DRAM_MR_8r, DRCC_SHADOW_DRAM_MR_8r, DRCD_SHADOW_DRAM_MR_8r, DRCE_SHADOW_DRAM_MR_8r, DRCF_SHADOW_DRAM_MR_8r, DRCG_SHADOW_DRAM_MR_8r, DRCH_SHADOW_DRAM_MR_8r},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_SHADOW_DRAM_MR_12r, DRCB_SHADOW_DRAM_MR_12r, DRCC_SHADOW_DRAM_MR_12r, DRCD_SHADOW_DRAM_MR_12r, DRCE_SHADOW_DRAM_MR_12r, DRCF_SHADOW_DRAM_MR_12r, DRCG_SHADOW_DRAM_MR_12r, DRCH_SHADOW_DRAM_MR_12r},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_SHADOW_DRAM_MR_14r, DRCB_SHADOW_DRAM_MR_14r, DRCC_SHADOW_DRAM_MR_14r, DRCD_SHADOW_DRAM_MR_14r, DRCE_SHADOW_DRAM_MR_14r, DRCF_SHADOW_DRAM_MR_14r, DRCG_SHADOW_DRAM_MR_14r, DRCH_SHADOW_DRAM_MR_14r},
             {DRCA_SHADOW_DRAM_MR_15r, DRCB_SHADOW_DRAM_MR_15r, DRCC_SHADOW_DRAM_MR_15r, DRCD_SHADOW_DRAM_MR_15r, DRCE_SHADOW_DRAM_MR_15r, DRCF_SHADOW_DRAM_MR_15r, DRCG_SHADOW_DRAM_MR_15r, DRCH_SHADOW_DRAM_MR_15r}};
    static soc_field_t DRAM_MRl[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX] = 
         {DRAM_MR_0f, DRAM_MR_1f, DRAM_MR_2f, DRAM_MR_3f, DRAM_MR_4f, DRAM_MR_5f, DRAM_MR_6f, DRAM_MR_7f,
          DRAM_MR_8f, INVALIDf, INVALIDf, INVALIDf, DRAM_MR_12f, INVALIDf, DRAM_MR_14f, DRAM_MR_15f};

    static soc_field_t 
         SHADOW_DRAM_MR_ADDRl[] = {SHADOW_DRAM_MR_0_ADDRf,  SHADOW_DRAM_MR_1_ADDRf, SHADOW_DRAM_MR_2_ADDRf, SHADOW_DRAM_MR_3_ADDRf, 
                                   SHADOW_DRAM_MR_4_ADDRf,  SHADOW_DRAM_MR_5_ADDRf, SHADOW_DRAM_MR_6_ADDRf, SHADOW_DRAM_MR_7_ADDRf, 
                                   SHADOW_DRAM_MR_8_ADDRf,  INVALIDf,               INVALIDf,               INVALIDf, 
                                   SHADOW_DRAM_MR_12_ADDRf, INVALIDf,               SHADOW_DRAM_MR_14_ADDRf, SHADOW_DRAM_MR_15_ADDRf};

    SOCDNX_INIT_FUNC_DEFS;

    if ((mr_ndx == 9) || (mr_ndx == 10) || (mr_ndx == 11) || (mr_ndx == 13)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("mr_ndx=%d is not used.\n"), mr_ndx));
    }
    
    if ((mrs_opcode > 0x3ffff)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("mrs_opcode=0x%x are out 18bit size.\n"), mrs_opcode));
    }
    
    address = SOC_SAND_GET_BITS_RANGE(mrs_opcode, 15, 0);
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Load MRS %d with opcode 0x%x, address=0x%x.\n"), FUNCTION_NAME(), drc_ndx, mr_ndx, mrs_opcode, address));

    /* Seting DRC MR reg */
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRC_DRAM_MRl[mr_ndx][drc_ndx], &reg_val, DRAM_MRl[mr_ndx], mrs_opcode);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, reg_val));
    
    /* Seting DRC Shadow MR reg */
    rv = soc_dpp_drc_combo28_util_calm_shell_bits_swap(unit, mrs_opcode, &field_val);
    SOCDNX_IF_ERR_EXIT(rv);
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_SHADOW_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRC_SHADOW_DRAM_MRl[mr_ndx][drc_ndx], &reg_val, SHADOW_DRAM_MR_ADDRl[mr_ndx], field_val); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_SHADOW_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, reg_val));     

    /* Seting DRAM MR reg */
    rv = soc_dpp_drc_combo28_dram_cpu_command(unit, drc_ndx, 0 /* ras_n */, 0 /* cas_n */, 0 /* we_n */, 1 /* act_n */, mr_ndx /* bank */, address /* address */);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Configuration Functions
 */

int soc_dpp_drc_combo28_info_config_default(int unit, soc_dpp_drc_combo28_info_t *drc_info)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* Verify Data not NULL */
    SOCDNX_NULL_CHECK(drc_info);

    /* Clear Info */
    sal_memset(drc_info, 0x0, sizeof (soc_dpp_drc_combo28_info_t));

    SOC_PBMP_CLEAR(drc_info->dram_bitmap);

    SOC_PBMP_CLEAR(drc_info->ref_clk_bitmap);

exit:
    SOCDNX_FUNC_RETURN;
}

int _soc_dpp_drc_combo28_str_prop_ext_ram_type(int unit, uint32 *dram_type)
{
    int rv = SOC_E_NONE;
    char 
        *propkey, 
        *propval;

    propkey = spn_EXT_RAM_TYPE;
    propval = soc_property_get_str(unit, propkey);    

    if (propval == NULL) {
        LOG_ERROR(BSL_LS_SOC_DRAM,
                  (BSL_META_U(unit,
                              "Missing property Key %s\n"), propkey));
        return SOC_E_FAIL;
    }

    if (sal_strcmp(propval, "DDR4") == 0) {
        *dram_type = SHMOO_COMBO28_DRAM_TYPE_DDR4;
    } else if (sal_strcmp(propval, "GDDR5") == 0) {
        *dram_type = SHMOO_COMBO28_DRAM_TYPE_GDDR5;        
    } else {
        LOG_ERROR(BSL_LS_SOC_DRAM,
                  (BSL_META_U(unit,
                              "Unexpected property value (\"%s\") for %s\n\r"), propval, propkey));
        return SOC_E_FAIL;
    }

    return rv;
}

int _soc_dpp_drc_combo28_dram_param_get(int unit, char *prop, uint32 *p_val)
{
    char 
      *propval, 
      *s;
    int val;

    SOCDNX_INIT_FUNC_DEFS;

    propval = soc_property_get_str(unit, prop);

    if (propval) {
        val = sal_ctoi(propval, &s);
        if ((*s == 'c') && (*(s+1) == '\0')) {
            /* Property is XXXXc, so convert it to clocks */
             *p_val = val | SOC_SAND_BIT(SOC_DPP_DRC_COMBO28_VAL_IS_IN_CLOCKS_BIT);
        } else if (*s == '\0') {
            /* Property is a simple number */
            *p_val = val;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Property %s is in a unknown format. Should be a number, or a number followed by c (e.g. 1000c)\n"), prop));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *       _soc_dpp_drc_combo28_check_swap
 * Purpose:
 *      Make sure that the swap data is logical, meaning that every bit was swapped maximum once and that the swapped value was used only once
 * Parameters:
 *      unit          - Device Number
 *      nof_items     - Number of items to check for swappings
 *      no_swap_value - Value to indicate that no swap has occoured for this item
 *      data_array    - an array that stores the swap data
 * Returns:
 *      SOC_E_XXX
 *  
 * IMPORTANT NOTE: 
 *      suitable for up to 32 items 
 */
int _soc_dpp_drc_combo28_check_swap(int unit, int nof_items, int no_swap_value, uint32 data_array[])
{
    uint32 values_bitmap, cells_bitmap;
    int item, nof_allowed_items = sizeof(uint32) * SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE;
    SOCDNX_INIT_FUNC_DEFS;

    /* Check that number of checked items is legal */
    if(nof_items > nof_allowed_items) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("%s - number of items to check (%d) is greater than allowed number (%d)\n"), FUNCTION_NAME(), nof_items, nof_allowed_items));  
    }

    /* Reset bitmaps */
    values_bitmap = 0;
    cells_bitmap = 0;

    /* Mark swapped values and swapped cells */
    for( item = 0; item < nof_items; ++item) {
        if(data_array[item] != no_swap_value) {
            values_bitmap |= 1 << data_array[item];
            cells_bitmap  |= 1 << item;
        }
    }

    /* Compare both bitmaps match to assert that swap is logical */
    if( (values_bitmap ^ cells_bitmap) != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("\nSwap Doesn't make sence. swapped cells(BitMap): 0x%x\t swapped values(BitMap): 0x%x should match\n"), cells_bitmap, values_bitmap));
    }

exit:
    SOCDNX_FUNC_RETURN;        
}

/*
 * Function:
 *       _soc_dpp_drc_combo28_check_ram_dq_swap
 * Purpose:
 *      Make sure that the swap data is logical, meaning that every bit that was swapped in the dq_swap was swapped only once and that every swapped value was used only once
 * Parameters:
 *      unit    - Device Number
 *      drc_ndx - Index of DRC for which to check the swap
 *      dq_swap[][][] - an array that stores the swap data
 * Returns:
 *      SOC_E_XXX
 */
int _soc_dpp_drc_combo28_check_ram_dq_swap(int unit, int drc_ndx, uint32 dq_swap[8][4][8])
{
    int byte;

    SOCDNX_INIT_FUNC_DEFS;
    for(byte = 0; byte < SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES; ++byte) {
        SOCDNX_IF_ERR_EXIT(_soc_dpp_drc_combo28_check_swap(unit, SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE, SOC_DPP_DRC_COMBO28_DQ_SWAP_VAL_DEFAULT_NO_SWAP, dq_swap[drc_ndx][byte]));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *       _soc_dpp_drc_combo28_check_ram_addr_bank_swap
 * Purpose:
 *      Make sure that the swap data is logical, meaning that every bit that was swapped in the addr_bank_swap was swapped only once and that every swapped value was used only once
 * Parameters:
 *      unit    - Device Number
 *      drc_ndx - Index of DRC for which to check the swap
 *      addr_bank_swap[][] - an array that stores the swap data
 * Returns:
 *      SOC_E_XXX
 */
int _soc_dpp_drc_combo28_check_ram_addr_bank_swap(int unit, int drc_ndx, uint32 addr_bank_swap[8][18])
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(_soc_dpp_drc_combo28_check_swap(unit, SOC_DPP_DRC_COMBO28_NOF_ADDR_BANK_BITS_PER_INTERFACE, SOC_DPP_DRC_COMBO28_ADDR_BANK_SWAP_VAL_DEFAULT_NO_SWAP, addr_bank_swap[drc_ndx]));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _soc_dpp_drc_combo28_str_prop_ext_ram_addr_bank_swap
 * Purpose:
 *      Translate the Swap data recived via SOC Properties to usuable data
 * Parameters:
 *      unit    - Device Number
 *      drc_ndx - Index of DRC to get relevant SoC Properties for
 *      addr_bank_swap[][] - an array to store result and return to caller
 * Returns:
 *      SOC_E_XXX
 */
int _soc_dpp_drc_combo28_str_prop_ext_ram_addr_bank_swap(int unit, int drc_ndx, uint32 addr_bank_swap[8][18])
{
    int bit;
    char suffix[DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* 
     * ext_ram_addr_bank_swap_dramX_bitY=M 
     * This loop also init to addr_bank_swap array default values SOC_DPP_DRC_COMBO28_ADDR_BANK_SWAP_VAL_DEFAULT_NO_SWAP
     */ 
    for (bit = 0; bit < SOC_DPP_DRC_COMBO28_NOF_ADDR_BANK_BITS_PER_INTERFACE; bit++) {
        sal_snprintf(suffix, DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE, "dram%d_bit%d", drc_ndx, bit );
        addr_bank_swap[drc_ndx][bit] = soc_property_suffix_num_get(unit, 9,  spn_EXT_RAM_ADDR_BANK_SWAP, suffix, SOC_DPP_DRC_COMBO28_ADDR_BANK_SWAP_VAL_DEFAULT_NO_SWAP);
        LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): suffix=%s=%d.\n"),FUNCTION_NAME(), suffix, addr_bank_swap[drc_ndx][bit]));
    }

    SOCDNX_FUNC_RETURN;
}

int _soc_dpp_drc_combo28_str_prop_ext_ram_dq_swap(int unit, int drc_ndx, uint32 dq_swap[8][4][8])
{
    int byte, bit;
    char suffix[DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* 
     * ext_ram_dq_swap_dramX_byteY_bitZ=M 
     * This loop also init dq_swap array to SOC_DPP_DRC_COMBO28_DQ_SWAP_VAL_DEFAULT_NO_SWAP
     */ 
    for (byte = 0; byte < SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES; byte++) {
        for (bit = 0; bit < SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE; bit++) {
            sal_snprintf(suffix, DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE, "dram%d_byte%d_bit%d", drc_ndx, byte, bit );
            dq_swap[drc_ndx][byte][bit] = soc_property_suffix_num_get(unit, 0,  spn_EXT_RAM_DQ_SWAP, suffix, SOC_DPP_DRC_COMBO28_DQ_SWAP_VAL_DEFAULT_NO_SWAP);
            LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): suffix=%s=%d.\n"),FUNCTION_NAME(), suffix, dq_swap[drc_ndx][byte][bit]));
        }
    }

    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_info_config(int unit, soc_dpp_drc_combo28_info_t *drc_info) 
{
    int i = 0x0;
    char print_buf[512];

    SOCDNX_INIT_FUNC_DEFS;

    /* dram_bitmap should be defined before this point */

    SOC_PBMP_COUNT(drc_info->dram_bitmap, drc_info->dram_num);

    /* Verify number of drams */
    if (drc_info->dram_num > SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Nubmber of dram is above max")));  
    }

    drc_info->enable = (drc_info->dram_num == 0) ? FALSE : TRUE;

    /* get EXT_RAM_AUTO_TUNE. if not set, try DDR3_AUTO_TUNE or 0x2 as defualt */
    drc_info->auto_tune = soc_property_get(unit, spn_EXT_RAM_AUTO_TUNE, -1); 
    if (-1 == drc_info->auto_tune) {
        drc_info->auto_tune = soc_property_get(unit, spn_DDR3_AUTO_TUNE, 0x2);
    }

    /* DDR phy ref clock should always be 100MHz for combo28 */
    drc_info->ref_clk_mhz = DRC_COMBO28_DDR_PHY_REF_CLK;

    drc_info->dram_freq = soc_property_get(unit, spn_EXT_RAM_FREQ, 0);
    drc_info->data_rate_mbps = (2 * drc_info->dram_freq);

    _shr_pbmp_format(drc_info->dram_bitmap, print_buf);
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s: dram_bitmap=%s, dram_num=%d, enable=%d, auto_tune=%d, dram_freq=%d, data_rate=%d\n"), 
                 FUNCTION_NAME(), print_buf, drc_info->dram_num, drc_info->enable, drc_info->auto_tune, drc_info->dram_freq, drc_info->data_rate_mbps));


    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_str_prop_ext_ram_type(unit, &drc_info->dram_type));

    drc_info->nof_columns = soc_property_get(unit, spn_EXT_RAM_COLUMNS, 1024);
    drc_info->nof_rows    = soc_property_get(unit, spn_EXT_RAM_ROWS, 0x0);
    drc_info->nof_banks   = soc_property_get(unit, spn_EXT_RAM_BANKS, 0x0);

    /* Dram Parameters */
    drc_info->dram_param.c_cas_latency    = soc_property_get(unit, spn_EXT_RAM_C_CAS_LATENCY, 0x0);
    drc_info->dram_param.c_wr_latency     = soc_property_get(unit, spn_EXT_RAM_C_WR_LATENCY, 0x0);
    drc_info->dram_param.init_wait_period = soc_property_get(unit, spn_EXT_RAM_INIT_WAIT_PERIOD, 0x3ff); 

    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RC,             &(drc_info->dram_param.t_rc)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RFC,            &(drc_info->dram_param.t_rfc)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RCD_RD,         &(drc_info->dram_param.t_rcd_rd)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RCD_WR,         &(drc_info->dram_param.t_rcd_wr)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RRD_L,          &(drc_info->dram_param.t_rrd_l)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RRD_S,          &(drc_info->dram_param.t_rrd_s)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RAS,            &(drc_info->dram_param.t_ras)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RP,             &(drc_info->dram_param.t_rp)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_WR,             &(drc_info->dram_param.t_wr)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_FAW,            &(drc_info->dram_param.t_faw)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_32AW,           &(drc_info->dram_param.t_32aw)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RTP_L,          &(drc_info->dram_param.t_rtp_l)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_WTR_L,          &(drc_info->dram_param.t_wtr_l)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_WTR_S,          &(drc_info->dram_param.t_wtr_s)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_CCD_L,          &(drc_info->dram_param.t_ccd_l)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_CCD_S,          &(drc_info->dram_param.t_ccd_s)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_ZQCS,           &(drc_info->dram_param.t_zqcs)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_REF,            &(drc_info->dram_param.t_ref)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_CRC_ALERT,      &(drc_info->dram_param.t_crc_alert)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RST,            &(drc_info->dram_param.t_rst)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_AL,             &(drc_info->dram_param.t_al)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_CRC_RD_LATENCY, &(drc_info->dram_param.t_crc_rd_latency)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_CRC_WR_LATENCY, &(drc_info->dram_param.t_crc_wr_latency)));


    /* 
     * Features
     */
    drc_info->gear_down_mode        = soc_property_get(unit, spn_EXT_RAM_GEAR_DOWN_MODE, 0x0);
    drc_info->abi                   = soc_property_get(unit, spn_EXT_RAM_ABI, 0x0);
    drc_info->write_dbi             = soc_property_get(unit, spn_EXT_RAM_WRITE_DBI, 0x0);
    drc_info->read_dbi              = soc_property_get(unit, spn_EXT_RAM_READ_DBI, 0x0);
    drc_info->cmd_par_latency       = soc_property_get(unit, spn_EXT_RAM_CMD_PAR_LATENCY, 0x0);    
    drc_info->write_crc             = soc_property_get(unit, spn_EXT_RAM_WRITE_CRC, 0x0);
    drc_info->read_crc              = soc_property_get(unit, spn_EXT_RAM_READ_CRC, 0x0);

    if ((drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) && (drc_info->read_crc==1 )) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram type DDR4 does not support read CRC ")));  
    }
    
    /* Dram Clam shell mode */
    SOC_PBMP_ITER(drc_info->dram_bitmap, i) { 
        if(soc_property_port_get(unit, i, spn_DRAM0_CLAMSHELL_ENABLE, FALSE)) {
            if(soc_property_port_get(unit, i, spn_DRAM1_CLAMSHELL_ENABLE, FALSE)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOC_MSG("Wrong Dram ClamShell configuration. only one of the dram should be enabled")));
            }
            drc_info->dram_clam_shell_mode[i] = DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DRAM_0;
        } else if (soc_property_port_get(unit, i, spn_DRAM1_CLAMSHELL_ENABLE, FALSE)) {
            drc_info->dram_clam_shell_mode[i] = DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DRAM_1;
        } else {
            drc_info->dram_clam_shell_mode[i] = DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DISABLED;
        }
    }

    /* DQ Swap and Address Bank Swap */
    SOC_PBMP_ITER(drc_info->dram_bitmap, i) {
        SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_str_prop_ext_ram_dq_swap(unit, i, drc_info->dram_dq_swap));
        SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_check_ram_dq_swap(unit, i, drc_info->dram_dq_swap));
        SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_str_prop_ext_ram_addr_bank_swap(unit, i, drc_info->dram_addr_bank_swap));
        SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_check_ram_addr_bank_swap(unit, i, drc_info->dram_addr_bank_swap));
    }
    
    /* Enable/disable Dram BIST on initialization */
    drc_info->bist_enable = soc_property_suffix_num_get(unit, -1, spn_BIST_ENABLE, "dram", 0x1);

    if ((drc_info->bist_enable == 1) && (drc_info->auto_tune == 0x3)) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram Bist was enabled without auto tuning - Invalid soc properties configuration")));
    }
    

     

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 *    PHY access functions
 */

int soc_dpp_drc_combo28_phy_reg_write(int unit, int drc_ndx, uint32 addr, uint32 data)
{
    uint32
        soc_sand_rv,
        reg_val;
    const static soc_reg_t 
        DRC_REGISTER_CONTROLl[] = {DRCA_REGISTER_CONTROLr, DRCB_REGISTER_CONTROLr, DRCC_REGISTER_CONTROLr,
                                   DRCD_REGISTER_CONTROLr, DRCE_REGISTER_CONTROLr, DRCF_REGISTER_CONTROLr,
                                   DRCG_REGISTER_CONTROLr, DRCH_REGISTER_CONTROLr},
        DRC_REGISTER_CONTROL_WDATAl[] = {DRCA_REGISTER_CONTROL_WDATAr, DRCB_REGISTER_CONTROL_WDATAr, DRCC_REGISTER_CONTROL_WDATAr,
                                         DRCD_REGISTER_CONTROL_WDATAr, DRCE_REGISTER_CONTROL_WDATAr, DRCF_REGISTER_CONTROL_WDATAr,
                                         DRCG_REGISTER_CONTROL_WDATAr, DRCH_REGISTER_CONTROL_WDATAr};
   
    SOCDNX_INIT_FUNC_DEFS; 

    /* Verify Dram index */
    if (drc_ndx >= SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram index is above max in device(%d)\n"), SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)));
    }
    /* Verify address is 12 bit */

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): drc_ndx=%d, addr=0x%08x, data=0x%08x\n"), FUNCTION_NAME(), drc_ndx, addr, data));

    reg_val = 0;
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TYPEf, 0x0);
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ADDRESSf, addr);
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TRIGf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_REGISTER_CONTROLl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_REGISTER_CONTROL_WDATAl[drc_ndx], REG_PORT_ANY, 0, data));

    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TRIGf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_REGISTER_CONTROLl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    soc_sand_rv = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          DRC_REGISTER_CONTROLl[drc_ndx],
          REG_PORT_ANY,
          0,
          REGISTER_ACCESS_TRIGf,
          0
        );
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
          
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_phy_reg_read(int unit, int drc_ndx, uint32 addr, uint32 *data)
{
    uint32
        soc_sand_rv,
        reg_val;
    const static soc_reg_t 
        DRC_REGISTER_CONTROLl[] = {DRCA_REGISTER_CONTROLr, DRCB_REGISTER_CONTROLr, DRCC_REGISTER_CONTROLr,
                                   DRCD_REGISTER_CONTROLr, DRCE_REGISTER_CONTROLr, DRCF_REGISTER_CONTROLr,
                                   DRCG_REGISTER_CONTROLr, DRCH_REGISTER_CONTROLr},
        DRC_REGISTER_ACCESS_RD_DATA_VALIDl[] = {DRCA_REGISTER_ACCESS_RD_DATA_VALIDr, DRCB_REGISTER_ACCESS_RD_DATA_VALIDr, DRCC_REGISTER_ACCESS_RD_DATA_VALIDr,
                                                DRCD_REGISTER_ACCESS_RD_DATA_VALIDr, DRCE_REGISTER_ACCESS_RD_DATA_VALIDr, DRCF_REGISTER_ACCESS_RD_DATA_VALIDr,
                                                DRCG_REGISTER_ACCESS_RD_DATA_VALIDr, DRCH_REGISTER_ACCESS_RD_DATA_VALIDr},
        DRC_REGISTER_CONTROL_RDATAl[] = {DRCA_REGISTER_CONTROL_RDATAr, DRCB_REGISTER_CONTROL_RDATAr, DRCC_REGISTER_CONTROL_RDATAr,
                                         DRCD_REGISTER_CONTROL_RDATAr, DRCE_REGISTER_CONTROL_RDATAr, DRCF_REGISTER_CONTROL_RDATAr,
                                         DRCG_REGISTER_CONTROL_RDATAr, DRCH_REGISTER_CONTROL_RDATAr};
   
    SOCDNX_INIT_FUNC_DEFS; 

    /* Verify Data not NULL */
    SOCDNX_NULL_CHECK(data);

    /* Verify Dram index */
    if (drc_ndx >= SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram index is above max in device(%d)\n"), SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)));
    }

    /* Verify address is 12 bit */

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): drc_ndx=%d, addr=0x%08x.\n"), FUNCTION_NAME(), drc_ndx, addr));

    reg_val = 0;
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TYPEf, 0x1);
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ADDRESSf, addr);
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TRIGf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_REGISTER_CONTROLl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TRIGf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_REGISTER_CONTROLl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    soc_sand_rv = arad_polling(
        unit,
        ARAD_TIMEOUT,
        ARAD_MIN_POLLS,
        DRC_REGISTER_CONTROLl[drc_ndx],
        REG_PORT_ANY,
        0,
        REGISTER_ACCESS_TRIGf,
        0
    );
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    soc_sand_rv = arad_polling(
        unit,
        ARAD_TIMEOUT,
        ARAD_MIN_POLLS,
        DRC_REGISTER_ACCESS_RD_DATA_VALIDl[drc_ndx],
        REG_PORT_ANY,
        0,
        REGISTER_ACCESS_RD_DATA_VALIDf,
        1
    );
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_REGISTER_CONTROL_RDATAl[drc_ndx], REG_PORT_ANY, 0, data));

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): *data=0x%08x\n"), FUNCTION_NAME(), *data));
      
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_phy_reg_modify(int unit, int drc_ndx, uint32 addr, uint32 data, uint32 mask)
{
    int 
        rv = SOC_E_NONE;
    uint32
        tmp, otmp,
        masked_data;
   
    SOCDNX_INIT_FUNC_DEFS; 

    /* Verify Dram index */
    if (drc_ndx >= SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram index is above max in device(%d)\n"), SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)));
    }

    /* Verify address is 12 bit */

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): drc_ndx=%d, addr=0x%08x, data=0x%08x, mask0x%08x\n"), FUNCTION_NAME(), drc_ndx, addr, data, mask));

    masked_data = data & mask;

    rv = soc_dpp_drc_combo28_phy_reg_read(unit, drc_ndx, addr, &tmp);
    SOCDNX_IF_ERR_EXIT(rv);

    otmp = tmp;
    tmp &= ~(mask);
    tmp |= masked_data;

    if (otmp != tmp) {
        rv = soc_dpp_drc_combo28_phy_reg_write(unit, drc_ndx, addr, tmp);
        SOCDNX_IF_ERR_EXIT(rv);
    }
      
exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *       soc_dpp_drc_combo28_shmoo_cfg_get
 * Purpose:
 *      Get the Shmoo configuration from soc properties
 * Parameters:
 *      unit               - Device Number
 *      drc_ndx            - drc index for which to get configuration from soc properties
 *      shmoo_config_param - where to store found parameters
 * Returns:
 *      SOC_E_NONE      - on success
 *      SOC_E_PARAM     - if a problem with recived arguments was detacted
 *      SOC_E_NOT_FOUND - incase couldn't get all of the relevant information to restore from the SOC properties
 */
int soc_dpp_drc_combo28_shmoo_cfg_get(int unit, int drc_ndx, combo28_shmoo_config_param_t* shmoo_config_param)
{
    int 
        byte,
        bit,
        var_array[SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY];
        
    char name_str[DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE];

    SOCDNX_INIT_FUNC_DEFS; 
    
    /* Verify shmoo_config_param not NULL */
    SOCDNX_NULL_CHECK(shmoo_config_param);
                
    for (byte = 0; byte < SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte) {
        /* Get dq_byte_wr_min_VDL_BITy */
        sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY * sizeof(int));    
        sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_byte%d", spn_COMBO28_TUNE_DQ_WR_MIN_VDL, byte);
        if (soc_property_ci_get_csv(unit, drc_ndx, name_str, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
        }
        
        for (bit  =0; bit < SHMOO_COMBO28_BYTE; ++bit) {
            shmoo_config_param->dq_byte_wr_min_vdl_bit[byte][bit] = var_array[bit];     
        }
    }
    
    /* Get dq_byte_wr_min_VDL_DBI */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_WR_MIN_VDL_DBI, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_wr_min_vdl_dbi[byte] = var_array[byte];
    }

    /* Get dq_byte_wr_min_VDL_EDC */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_WR_MIN_VDL_EDC, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_wr_min_vdl_edc[byte] = var_array[byte];
    }
    
    /* Get dq_byte_wr_max_VDL_DATA */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));    
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_WR_MAX_VDL_DATA, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_wr_max_vdl_data[byte] = var_array[byte];
    }
    
    /* Get dq_byte_wr_max_VDL_DQS */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_WR_MAX_VDL_DQS, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_wr_max_vdl_dqs[byte] = var_array[byte];
    }    
   
    for (byte = 0; byte < SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte) {
        /* Get dq_byte_rd_min_vdl_bit */
        sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));     
        sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_byte%d", spn_COMBO28_TUNE_DQ_RD_MIN_VDL, byte);
        if(soc_property_ci_get_csv(unit, drc_ndx, name_str, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
        }

        for (bit = 0; bit < SHMOO_COMBO28_BYTE; ++bit) {
            shmoo_config_param->dq_byte_rd_min_vdl_bit[byte][bit] = var_array[bit];     
        }
    }
    
    /* Get dq_byte_rd_min_vdl_dbi */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_RD_MIN_VDL_DBI, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_rd_min_vdl_dbi[byte] = var_array[byte];
    }

    /* Get dq_byte_rd_min_vdl_edc */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_RD_MIN_VDL_EDC, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_rd_min_vdl_edc[byte] = var_array[byte];
    }
    
    /* Get dq_byte_rd_max_vdl_dqsp */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_RD_MAX_VDL_DQSP, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_rd_max_vdl_dqsp[byte] = var_array[byte];
    }
    
    /* Get dq_byte_rd_max_vdl_dqsn */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_RD_MAX_VDL_DQSN, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_rd_max_vdl_dqsn[byte] = var_array[byte];
    }    

    /* Get dq_byte_ren_fifo_config */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_REN_FIFO_CONFIG, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_ren_fifo_config[byte] = var_array[byte];
    } 

    /* Get dq_byte_edcen_fifo_config */   
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));    
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_EDCEN_FIFO_CONFIG, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_edcen_fifo_config[byte] = var_array[byte];
    }
     
    /* Get dq_byte_rd_max_vdl_fsm */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_READ_MAX_VDL_FSM, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_rd_max_vdl_fsm[byte] = var_array[byte];
    }
    
    /* Get dq_byte_vref_dac_config */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_VREF_DAC_CONFIG, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_vref_dac_config[byte] = var_array[byte];
    }
    
    /* Get dq_byte_macro_reserved_reg */
    sal_memset(var_array, 0, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_MACRO_RESERVED_REG, SOC_DPP_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_macro_reserved_reg[byte] = var_array[byte];
    }        
   
    /* Get aq_l_max_vdl_addr */
    shmoo_config_param->aq_l_max_vdl_addr = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_L_MAX_VDL_ADDR, 0x0);
    
    /* Get aq_l_max_vdl_ctrl */
    shmoo_config_param->aq_l_max_vdl_ctrl = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_L_MAX_VDL_CTRL, 0x0);    

    /* Get aq_l_macro_reserved_reg */
    shmoo_config_param->aq_l_macro_reserved_reg = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_L_MACRO_RESERVED_REG, 0x0);    

    /* Get aq_u_max_vdl_addr */
    shmoo_config_param->aq_u_max_vdl_addr = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_U_MAX_VDL_ADDR, 0x0);

    /* Get aq_u_max_vdl_ctrl */
    shmoo_config_param->aq_u_max_vdl_ctrl = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_U_MAX_VDL_CTRL, 0x0);

    /* Get aq_u_macro_reserved_reg */
    shmoo_config_param->aq_u_macro_reserved_reg = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_U_MACRO_RESERVED_REG, 0x0);

    /* Get common_macro_reserved_reg */
    shmoo_config_param->common_macro_reserved_reg = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_COMMON_MACRO_RESERVED_REG, 0x0);
    
    /* Get control_regs_read_clock_config */
    shmoo_config_param->control_regs_read_clock_config = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_READ_CLOCK_CONFIG, 0x0);
    
    /* Get control_regs_input_shift_ctrl */
    shmoo_config_param->control_regs_input_shift_ctrl = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_INPUT_SHIFT_CTRL, 0x0);   

    /* Get control_regs_ren_fifo_central_init */
    shmoo_config_param->control_regs_ren_fifo_central_init = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_REN_FIFO_CENTRAL_INITIALIZER, 0x0);
    
    /* Get control_regs_edcen_fifo_central_init */
    shmoo_config_param->control_regs_edcen_fifo_central_init = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_EDCEN_FIFO_CENTRAL_INIT, 0x0);
        
    /* Get control_regs_shared_vref_dac_config */
    shmoo_config_param->control_regs_shared_vref_dac_config = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_SHARED_VREF_DAC_CONFIG, 0x0);

    /* Get control_regs_reserved_reg */ 
    shmoo_config_param->control_regs_reserved_reg = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_RESERVED_REG, 0x0);

exit:

    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_cfg_set(int unit, int drc_ndx, combo28_shmoo_config_param_t* shmoo_config_param)
{
    int 
        written = 0,
        byte,
        bit;
    char name_str[DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE];
    char val_str[DRC_COMBO28_VAL_STR_BUFFER_SIZE];    

    SOCDNX_INIT_FUNC_DEFS; 
    
    /* Verify shmoo_config_param not NULL */
    SOCDNX_NULL_CHECK(shmoo_config_param);
            
    for (byte=0; byte < SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte) {
        /* Set  combo28_tune_DQ_WR_MIN_VDL*/
        sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_byte%d_ci%d.%d", spn_COMBO28_TUNE_DQ_WR_MIN_VDL, byte, drc_ndx, unit);

        written = 0;
        val_str[0]=0;
        for(bit=0; bit<SHMOO_COMBO28_BYTE; ++bit){
            written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_wr_min_vdl_bit[byte][bit] );    
        }
        
        if (soc_mem_config_set(name_str, val_str) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Faile to save combo28_tune_DQ_WR_MIN_VDL configuration")));
        }
    }
                 
    /* Set combo28_tune_DQ_WR_MIN_VDL_DBI */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_WR_MIN_VDL_DBI, drc_ndx, unit);     
        
    written = 0;
    val_str[0]='\0';
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_wr_min_vdl_dbi[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_WR_MIN_VDL_DBI configuration")));
    }

    /* Set combo28_tune_DQ_WR_MIN_VDL_EDC */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_WR_MIN_VDL_EDC, drc_ndx, unit);     

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_wr_min_vdl_edc[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_WR_MIN_VDL_EDC configuration")));
    }
    
    /* Set combo28_tune_DQ_WR_MAX_VDL_DATA */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_WR_MAX_VDL_DATA, drc_ndx, unit);

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_wr_max_vdl_data[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_WR_MAX_VDL_DATA configuration")));
    }
        
    /* Set combo28_tune_DQ_WR_MAX_VDL_DQS */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_WR_MAX_VDL_DQS, drc_ndx, unit);

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_wr_max_vdl_dqs[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_WR_MAX_VDL_DQS configuration")));
    }
       
    for (byte = 0; byte < SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte) {
        /* Set  combo28_tune_DQ_RD_MIN_VDL*/
        sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_byte%d_ci%d.%d", spn_COMBO28_TUNE_DQ_RD_MIN_VDL, byte, drc_ndx, unit);

        written = 0;
        val_str[0]=0; 
        for(bit=0; bit<SHMOO_COMBO28_BYTE; ++bit){
            written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_min_vdl_bit[byte][bit] );    
        }
        
        if (soc_mem_config_set(name_str, val_str) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Faile to save combo28_tune_DQ_READ_MIN_VDL configuration")));
        }
    }
                 
    /* Set combo28_tune_DQ_RD_MIN_VDL_DBI */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_RD_MIN_VDL_DBI, drc_ndx, unit);     

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_min_vdl_dbi[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_READ_MIN_VDL_DBI configuration")));
    }

    /* Set combo28_tune_DQ_RD_MIN_VDL_EDC */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_RD_MIN_VDL_EDC, drc_ndx, unit);     
    
    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_min_vdl_edc[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_RD_MIN_VDL_EDC configuration")));
    }
    
    /* Set combo28_tune_DQ_RD_MAX_VDL_DQSP */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_RD_MAX_VDL_DQSP, drc_ndx, unit);     
    
    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_max_vdl_dqsp[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_RD_MAX_VDL_DQSP configuration")));
    }
        
    /* Set combo28_tune_DQ_RD_MAX_VDL_DQSN */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_RD_MAX_VDL_DQSN, drc_ndx, unit);     

    written = 0;
    val_str[0]=0;    
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_max_vdl_dqsn[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_RD_MAX_VDL_DQSN configuration")));
    }

    /* Set combo28_tune_DQ_REN_FIFO_CONFIG */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_REN_FIFO_CONFIG, drc_ndx, unit);     
    
    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_ren_fifo_config[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_REN_FIFO_CONFIG configuration")));
    }
    
    /* Set combo28_tune_DQ_EDCEN_FIFO_CONFIG */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_EDCEN_FIFO_CONFIG, drc_ndx, unit);     

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_edcen_fifo_config[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_EDCEN_FIFO_CONFIG configuration")));
    }
    
    /* Set combo28_tune_DQ_READ_MAX_VDL_FSM */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_READ_MAX_VDL_FSM, drc_ndx, unit);     
    
    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_max_vdl_fsm[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_READ_MAX_VDL_FSM configuration")));
    }
    
    /* Set combo28_tune_DQ_VREF_DAC_CONFIG */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_VREF_DAC_CONFIG, drc_ndx, unit);     
    
    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_vref_dac_config[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_VREF_DAC_CONFIG configuration")));
    }  

    /* Set combo28_tune_DQ_BYTE_MACRO_RESERVED_REG */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_MACRO_RESERVED_REG, drc_ndx, unit);     

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_macro_reserved_reg[byte] );    
    }

    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_BYTE_MACRO_RESERVED_REG configuration")));
    }  
              
    /* Set combo28_tune_aq_l_max_vdl_addr */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_L_MAX_VDL_ADDR, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_l_max_vdl_addr );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_TUNE_aq_l_max_vdl_addr configuration")));
    }       
    
    /* Set combo28_tune_aq_l_max_vdl_ctrl */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_L_MAX_VDL_CTRL, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_l_max_vdl_ctrl );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_aq_l_max_vdl_ctrl configuration")));
    } 

    /* Set combo28_tune_aq_l_macro_reserved_reg */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_L_MACRO_RESERVED_REG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_l_macro_reserved_reg );    

    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("FaileD to save combo28_tune_aq_l_macro_reserved_reg configuration")));
    } 

    /* Set combo28_tune_aq_u_max_vdl_addr */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_U_MAX_VDL_ADDR, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_u_max_vdl_addr );    
        
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_aq_u_max_vdl_addr configuration")));
    }       
    
    /* Set combo28_tune_aq_u_max_vdl_ctrl */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_U_MAX_VDL_CTRL, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_u_max_vdl_ctrl );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_aq_u_max_vdl_ctrl configuration")));
    }    

    /* Set combo28_tune_aq_u_macro_reserved_reg */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_U_MACRO_RESERVED_REG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_u_macro_reserved_reg );    

    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_aq_u_macro_reserved_reg configuration")));
    }    

    /* Set combo28_tune_common_macro_reserved_reg */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_COMMON_MACRO_RESERVED_REG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->common_macro_reserved_reg );    

    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_common_macro_reserved_reg configuration")));
    } 

    /* Set control_regs_read_clock_config */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_READ_CLOCK_CONFIG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_read_clock_config );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_control_regs_read_clock_config configuration")));
    }     
    
    /* Set control_regs_input_shift_ctrl */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_INPUT_SHIFT_CTRL, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_input_shift_ctrl );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_control_regs_input_shift_ctrl configuration")));
    } 

    /* Set control_regs_ren_fifo_central_init */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_REN_FIFO_CENTRAL_INITIALIZER, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_ren_fifo_central_init );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_control_regs_ren_fifo_central_init configuration")));
    }
  
    /* Set control_regs_edcen_fifo_central_init */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_EDCEN_FIFO_CENTRAL_INIT, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_edcen_fifo_central_init );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_control_regs_edcen_fifo_central_init configuration")));
    }  
    
    /* Set control_regs_shared_vref_dac_config */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_SHARED_VREF_DAC_CONFIG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_shared_vref_dac_config );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_control_regs_shared_vref_dac_config configuration")));
    }     

    /* Set control_regs_reserved_reg */
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_RESERVED_REG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_reserved_reg );    

    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Faile to save combo28_tune_control_regs_reserved_reg configuration")));
    }     

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_enable_wr_crc(int unit, int drc_ndx, int enable)
{   
    int 
    rv = SOC_E_NONE,
    mrs_opcode=0,
    mrs_mask=0,
    mrs_num;
    uint32 
    dram_type;
    uint64 reg64_val;
   
    static soc_reg_t 
        DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr, 
                                        DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr,
                                        DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr},
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
                                                         DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};
     
    SOCDNX_INIT_FUNC_DEFS;
 
    /* Configure DRC side , Assuming EdcWrLattency is already configured */
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    soc_reg64_field32_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, WRITE_CRCf, (enable == 0x0) ? FALSE : TRUE);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    /* Configure Dram side */
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    dram_type = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, DRAM_TYPEf);

    if (dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) {
    
        SOC_SAND_SET_BIT(mrs_opcode, ((enable == 0x0) ? FALSE : TRUE), 12);
        SOC_SAND_SET_BIT(mrs_mask, 0x1 ,12);        
        mrs_num = 2;
         
    } else {
    
        SOC_SAND_SET_BIT(mrs_opcode, ((enable == 0x0) ? TRUE : FALSE), 10);
        SOC_SAND_SET_BIT(mrs_mask, 0x1 ,10);       
        mrs_num = 4;
        
    }

    rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, mrs_num, mrs_opcode, mrs_mask);
    SOCDNX_IF_ERR_EXIT(rv);
        
exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *       soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc
 * Purpose:
 *      After shmoo activation the dbi and crc are reset. this function restore them to previous condition
 * Parameters:
 *      unit               - Device Number
 *      drc_info           - drc info
 *      drc_ndx            - drc index for which to restore condition
 * Returns:
 *      SOC_E_NONE      - on success
 *      SOC_E_XXXX      - on failure
 */
int soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc(int unit, soc_dpp_drc_combo28_info_t* drc_info, int drc_ndx)
{

    SOCDNX_INIT_FUNC_DEFS;

    if (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) 
    {
        if (drc_info->write_dbi == 1) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_enable_wr_dbi(unit, drc_ndx, 1));
        }

        if(drc_info->read_dbi == 1) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_enable_rd_dbi(unit, drc_ndx, 1));
        }

        if(drc_info->write_crc == 1) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_enable_wr_crc(unit, drc_ndx, 1));
        }

        if(drc_info->read_dbi == 1) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_enable_rd_crc(unit, drc_ndx, 1));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

