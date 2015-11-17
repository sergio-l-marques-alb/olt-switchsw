/**************************************************************************
 **************************************************************************
 *  File Name     :  falcon_furia_dependencies.c                          *  
 * Created On    :  05/29/2014                                            * 
 * Created By    :  Sharad Sharma                                         * 
 * Description   :  Version of Dependency APIs used in Eval               * 
 * Revision      :  $Id: $                                                * 
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$        * 
 * No portions of this material may be reproduced in any form without     * 
 * the written permission of:                                             * 
 *     Broadcom Corporation                                               * 
 *     5300 California Avenue                                             *
 *     Irvine, CA  92617                                                  * 
 *                                                                        * 
 * All information contained in this document is Broadcom Corporation     * 
 * company private proprietary, and trade secret.                         * 
 *                                                                        * 
 **************************************************************************
 **************************************************************************/

#include <phymod/phymod.h>
#include "falcon_furia_interface.h"
#include "falcon_furia_dependencies.h"
#include "../furia_reg_access.h"
#include "../furia_address_defines.h"
#include "falcon_furia_ram_regs.h"
#include "../common/srds_api_err_code.h"
phymod_access_t g_pa;
static const uint16_t M0_DATA_RAM_LENGTH    = 0x1400;     /* fixed by hardware design */

static uint8_t isMasterM0(void)
{
    uint32_t acc_flags = 0;

    /* Get the lane map from phymod access */
    acc_flags = PHYMOD_ACC_FLAGS(&g_pa);
    return ((acc_flags >> 31) & 0x1) ? 1 : 0; 
}

err_code_t falcon_furia_pm_acc_set(const phymod_access_t *pa)
{

    PHYMOD_MEMCPY(&g_pa, pa, sizeof(phymod_access_t));
    return (ERR_CODE_NONE);
}

err_code_t falcon_furia_pm_acc_get(phymod_access_t *pa)
{
    PHYMOD_MEMCPY(pa, &g_pa, sizeof(phymod_access_t));
    return (ERR_CODE_NONE);
}
err_code_t falcon_furia_pmd_rdt_reg(uint16_t address, uint16_t *val) {
    phymod_access_t pa;
    uint32_t data;
    falcon_furia_pm_acc_get(&pa);
    furia_reg_read(&pa, (0x10000 | (uint32_t) address), &data);
    *val = data & 0xffff;
    return ( ERR_CODE_NONE );
}

err_code_t falcon_furia_pmd_wr_reg(uint16_t address, uint16_t val) {
    phymod_access_t pa;
    uint32_t data = 0xffff & val;
    falcon_furia_pm_acc_get(&pa);
    furia_reg_write(&pa, (0x10000 | (uint32_t) address), data);
   return(ERR_CODE_NONE);
}

err_code_t falcon_furia_pmd_mwr_reg(uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val) {
    phymod_access_t pa;
    val = val << lsb;
    falcon_furia_pm_acc_get(&pa);
    furia_reg_modify(&pa, (0x10000 | (uint32_t) addr), val, mask);
    return(ERR_CODE_NONE);
}

err_code_t falcon_furia_delay_ns(uint16_t delay_ns) {

  if (delay_ns > 1000) {
    return (ERR_CODE_SERDES_DELAY);
  }
  return (ERR_CODE_NONE);
} 


err_code_t falcon_furia_delay_us(uint32_t delay_us) {
  PHYMOD_USLEEP(delay_us);
  return (ERR_CODE_NONE);
}

uint8_t falcon_furia_get_lane(void) {
  uint16_t reg_addr = 0x8000;
  uint16_t reg_val = 0; 
  uint16_t wr_lane_val = 0;
  uint16_t lane_number = 0;
  int rv = 0;
  rv = falcon_furia_pmd_rdt_reg(reg_addr, &reg_val); 
  if(!rv) {
     wr_lane_val = (reg_val & 0x00f0) >> 4;
     switch(wr_lane_val) {
         case 0x1:
             lane_number = 0;
         break;
         case 0x2:
             lane_number = 1;
         break;
         case 0x4:
             lane_number = 2;
         break;
         case 0x8:
             lane_number = 3;
         break;
         default:
             lane_number = 0;
         break;
     }
     return lane_number; 
  }
  return rv;
}
/*---------------------------------------------*/
/*  Serdes IP RAM access - Lane RAM Variables  */
/*---------------------------------------------*/
/*          rd - read; wr - write              */ 
/*          b  - byte; w  - word               */
/*---------------------------------------------*/
/** Unsigned Word Read of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs. Output as little-endian format the LSB is the lowest address.
* @param rd_val 16bit unsigned value to be read from RAM variable
* @param addr Address of RAM variable to be read
* @param size Size of RAM in word to be read
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t falcon_furia_rdw_ram(uint16_t *rd_val, uint16_t addr, uint16_t size)
{
    uint16_t uC_DRAM_data_reg = 0;
    phymod_access_t pa;
    uint32_t tmp_data;

    falcon_furia_pm_acc_get(&pa);

    if (((addr+size) > M0_DATA_RAM_LENGTH)  || ((addr % 2) != 0)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }

    if (isMasterM0()) {
        furia_reg_write(&pa, (uint32_t) FUR_M0ACCESS_ADDR_master_dram_mem_add_Adr, addr);
        uC_DRAM_data_reg = FUR_M0ACCESS_DATA_master_dram_rdata_Adr;
    } else {
        furia_reg_write(&pa, (uint32_t) FUR_M0ACCESS_ADDR_slave_dram_mem_add_Adr, addr);
        uC_DRAM_data_reg = (uint16_t) FUR_M0ACCESS_DATA_slave_dram_rdata_Adr;
    }

    while (size--) {
        furia_reg_read(&pa, (uint32_t)uC_DRAM_data_reg, &tmp_data);
        *rd_val = (uint16_t)(tmp_data & 0xffff);
        rd_val++;
    }

    return ERR_CODE_NONE;
}
/** Unsigned Byte Read of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs. 
* @param rd_val 8bit unsigned value to be read from RAM variable
* @param addr Address of RAM variable to be read
* @param size Size of RAM in byte to be read
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t falcon_furia_rdb_ram(uint8_t *rd_val, uint16_t addr, uint16_t size)
{
    uint32_t tmp_data = 0;
    uint16_t uC_DRAM_data_reg = 0;
    phymod_access_t pa;
    falcon_furia_pm_acc_get(&pa);

    if (((addr+size) > M0_DATA_RAM_LENGTH)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }

    if (isMasterM0()) {
        furia_reg_write(&pa, (uint32_t) FUR_M0ACCESS_ADDR_master_dram_mem_add_Adr, (addr - (addr%2)));
        /* the provided address must be an even one */
        uC_DRAM_data_reg = FUR_M0ACCESS_DATA_master_dram_rdata_Adr;
    } else {
        furia_reg_write(&pa, (uint32_t) FUR_M0ACCESS_ADDR_slave_dram_mem_add_Adr, (addr - (addr%2)));
        /* the provided address must be an even one */
        uC_DRAM_data_reg = (uint16_t)FUR_M0ACCESS_DATA_slave_dram_rdata_Adr;
    }

    if (addr % 2) {
        /* starting address is odd */
        furia_reg_read(&pa, (uint32_t)uC_DRAM_data_reg, &tmp_data);
        *rd_val = (uint8_t) (tmp_data>>8);
        rd_val++;
        size--;
    }

    while (size >= 2) {
        furia_reg_read(&pa, (uint32_t)uC_DRAM_data_reg, &tmp_data);
        *rd_val = (uint8_t) (tmp_data & 0xFF); rd_val++;
        *rd_val = (uint8_t) (tmp_data >> 8); rd_val++;
        size -= 2;
    }

    if (size) {
        furia_reg_read(&pa, (uint32_t)uC_DRAM_data_reg, &tmp_data);
        *rd_val = (uint8_t) (tmp_data & 0xFF);
        rd_val++;
    }

    return ERR_CODE_NONE;
}
/** Unsigned Byte Write of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs
* @param addr Address of RAM variable to be written
* @param size Size of RAM in byte to be written
* @param wr_val 8bit unsigned value to be written to RAM variable
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t falcon_furia_wrb_ram(uint16_t addr, uint16_t size, uint8_t *wr_val)
{
    uint16_t uC_DRAM_data_reg = 0;
    uint16_t end_addr;
    uint8_t front_buf[2], end_buf[2];
    uint16_t tmp_data = 0;
    phymod_access_t pa;
    falcon_furia_pm_acc_get(&pa);

    if (((addr+size) > M0_DATA_RAM_LENGTH)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }

    /* The native DRAM access is at granularity of 16-bit entity. If the first/last byte is at
     * an odd address, we need to a read modify write.
     *
     * In addition, the read and write cannot be mixed without a write to the DRAM address
     * register.
     *
     * So we will read the words that have the first/last requested byte and modify the result
     * before doing the batch write.
     */
    end_addr = addr + size;
    if ((end_addr % 2) != 0) {
        falcon_furia_rdb_ram(end_buf, end_addr-1, 2);
        end_buf[0] = wr_val[size-1];
    }

    if ((addr % 2) != 0) {
        falcon_furia_rdb_ram(front_buf, addr-1, 2);
        front_buf[1] = wr_val[0];
    }


    if (isMasterM0()) {
        furia_reg_write(&pa, (uint32_t) FUR_M0ACCESS_ADDR_master_dram_mem_add_Adr, (addr - (addr%2)));
        /* the provided address must be an even one */
        uC_DRAM_data_reg = (uint16_t)FUR_M0ACCESS_DATA_master_dram_wdata_Adr;
    } else {
        furia_reg_write(&pa, (uint32_t) FUR_M0ACCESS_ADDR_slave_dram_mem_add_Adr, (addr - (addr%2)));
        /* the provided address must be an even one */
        uC_DRAM_data_reg = (uint16_t)FUR_M0ACCESS_DATA_slave_dram_wdata_Adr;
    }

    if (addr % 2) {
        tmp_data  = front_buf[1];
        tmp_data <<= 8;
        tmp_data += front_buf[0];
        furia_reg_write(&pa, (uint32_t) uC_DRAM_data_reg, tmp_data);
        wr_val++;
        size--;
    }

    while (size >= 2) {
       tmp_data  = wr_val[1];
       tmp_data <<= 8;
       tmp_data += wr_val[0];
       furia_reg_write(&pa, (uint32_t) uC_DRAM_data_reg, tmp_data);

        wr_val += 2;
        size -= 2;
    }

    if (end_addr % 2) {
        tmp_data = end_buf[1];
        tmp_data <<= 8;
        tmp_data += end_buf[0];
        furia_reg_write(&pa, (uint32_t) uC_DRAM_data_reg, tmp_data);
    }

    return ERR_CODE_NONE;
}

/** Unsigned Word Write of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs
* @param addr Address of RAM variable to be written
* @param size Size of RAM in word to be written
* @param wr_val 16bit unsigned value to be written to RAM variable
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t falcon_furia_wrw_ram(uint16_t addr, uint16_t size, uint16_t *wr_val)
{
    uint16_t uC_DRAM_data_reg = 0;
    phymod_access_t pa;
    falcon_furia_pm_acc_get(&pa);

    if (((addr+size) > M0_DATA_RAM_LENGTH)  || ((addr % 2) != 0)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }

    if (isMasterM0()) {
        furia_reg_write(&pa, (uint32_t) FUR_M0ACCESS_ADDR_master_dram_mem_add_Adr, addr);
        uC_DRAM_data_reg = (uint16_t)FUR_M0ACCESS_DATA_master_dram_wdata_Adr;
    } else {
        furia_reg_write(&pa, (uint32_t) FUR_M0ACCESS_ADDR_slave_dram_mem_add_Adr, addr);
        uC_DRAM_data_reg = (uint16_t)FUR_M0ACCESS_DATA_slave_dram_wdata_Adr;
    }

    while (size--) {
        furia_reg_write(&pa, (uint32_t) uC_DRAM_data_reg, *wr_val);
        wr_val++;
    }

    return ERR_CODE_NONE;
}
/** Retrieve Lane Based Information.
* Retrieve current lane memory address information
* @param ext_uc_lane_info an output pointer pointed to uC lane info struct
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t falcon_furia_get_uc_ln_info(falcon_furia_uc_lane_info_st *lane_info)
{
    if (lane_info == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }

    /* fixed values based on a specific implementation */
    lane_info->is_direct_ram_access_avail = 1;
    lane_info->core_var_base_address = 0x400;   /* check furia_main for actual value */
    lane_info->lane_ram_size = 304;
    lane_info->trace_mem_size = 768;

    lane_info->uc_lane_idx = falcon_furia_get_lane();
    lane_info->trace_mem_base_address = 0x100;  /* check furia_main for actual value */
    lane_info->lane_var_base_address = 0x420    /* check furia_main for actual value */
        + lane_info->lane_ram_size*lane_info->uc_lane_idx;
    lane_info->diag_base_address = lane_info->trace_mem_base_address
        + 192*lane_info->uc_lane_idx;

    return ERR_CODE_NONE;
}
