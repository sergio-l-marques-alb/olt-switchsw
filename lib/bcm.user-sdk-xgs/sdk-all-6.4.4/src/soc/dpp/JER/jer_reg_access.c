/*
 * $Id: stat.c,v 1.30 Broadcom SDK $
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
 * SOC_JER_REG_ACCESS
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_REG

 /*************
 * INCLUDES  *
 *************/
#include <soc/dcmn/error.h>
#include <soc/types.h>
#include <soc/defs.h>
#include <soc/cm.h>
#include <soc/portmode.h>
#include <soc/drv.h>

#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/mbcm.h>

 /*************
 * DEFINES   *
 *************/

 /*************
 *  MACROS   *
 *************/

 /*************
 * TYPE DEFS *
 *************/

 /*************
 * GLOBALS   *
 *************/

 /*************
 * FUNCTIONS *
 *************/

STATIC int 
soc_jer_ilkn_core_instance_access_get(int unit, int ilkn_offset, soc_mem_t* mem, int* block_idx)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (ilkn_offset) {
    case 0:
        *mem = ILKN_PMH_PORT_0_CPU_ACCESSm;
        *block_idx = SOC_BLOCK_ANY;
        break;
    case 1:
        *mem = ILKN_PMH_PORT_1_CPU_ACCESSm;
        *block_idx = SOC_BLOCK_ANY;
        break;
    case 2:
        *mem = ILKN_PML_PORT_0_CPU_ACCESSm;
        *block_idx = ILKN_PML_BLOCK(unit, 0);
        break;
    case 3:
        *mem = ILKN_PML_PORT_1_CPU_ACCESSm;
        *block_idx = ILKN_PML_BLOCK(unit, 0);
        break;
    case 4:
        *mem = ILKN_PML_PORT_0_CPU_ACCESSm;
        *block_idx = ILKN_PML_BLOCK(unit, 1);
        break;
    case 5:
        *mem = ILKN_PML_PORT_1_CPU_ACCESSm;
        *block_idx = ILKN_PML_BLOCK(unit, 1);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("ilkn offset %d is invalid"), ilkn_offset));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_ilkn_reg32_get/set
 * Purpose:
 *      Access ILKN-core registers through ILKN wrap blocks:
 *      ILKN_PMH, ILKN_PML0, ILKN_PML1.
 *      designated memorys in wrap blocks contain all core registers.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      reg   - ILKN-core reg num. 
 *      port  - ilkn port num.
 *      index - index of reg, if array.
 *      data  - (OUT) 32-bit reg value.
 * Returns:
 *      SOC_E_XXX.
 */
int 
soc_jer_ilkn_reg32_get(int unit, soc_reg_t reg, int port, int index, uint32 *data)
{
    uint32 addr, offset;
    int entry_inx, block, blk_id;
    uint8 acc_type;
    soc_mem_t mem;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(data);

    if(SOC_REG_IS_ABOVE_32(unit, reg)){
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "soc_jer_ilkn_reg32_get: Register is too big\n")));
    }
    /*get reg address to retrieve offset in ILKN-wrap table*/
    addr = soc_reg_addr_get(unit, reg, port, index, FALSE, &block, &acc_type);
    entry_inx = addr / BITS2BYTES(32);

    /*get ilkn core instance (should be 0-5)*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, SOC_PROTOCOL_OFFSET_FLAGS_FORCE_ILKN, &offset));
    /*find which mem to access according to ilkn-core offset*/
    SOCDNX_IF_ERR_EXIT(soc_jer_ilkn_core_instance_access_get(unit, offset, &mem, &blk_id));
    /*read correct line from mem*/
    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, mem, blk_id, entry_inx, data));

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_ilkn_reg32_set(int unit, soc_reg_t reg, int port, int index, uint32 data)
{
    uint32 addr, offset;
    int entry_inx, block, blk_id;
    uint8 acc_type;
    soc_mem_t mem;
    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_REG_IS_ABOVE_32(unit, reg)){
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "soc_jer_ilkn_reg32_set: Register is too big\n")));
    }
    /*get reg address to retrieve offset in ILKN-wrap table*/
    addr = soc_reg_addr_get(unit, reg, port, index, FALSE, &block, &acc_type);
    entry_inx = addr / BITS2BYTES(32);

    /*get ilkn core instance (should be 0-5)*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, SOC_PROTOCOL_OFFSET_FLAGS_FORCE_ILKN, &offset));
    /*find which mem to access according to ilkn-core offset*/
    SOCDNX_IF_ERR_EXIT(soc_jer_ilkn_core_instance_access_get(unit, offset, &mem, &blk_id));
    /*read correct line from mem*/
    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, mem, blk_id, entry_inx, &data));

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_ilkn_reg64_get(int unit, soc_reg_t reg, int port, int index, uint64 *data)
{
    uint32 data_low;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(data);

    SOCDNX_IF_ERR_EXIT(soc_jer_ilkn_reg32_get(unit, reg, port, index, &data_low));

    COMPILER_64_SET(*data, 0, data_low);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_ilkn_reg64_set(int unit, soc_reg_t reg, int port, int index, uint64 data)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_ilkn_reg32_set(unit, reg, port, index, COMPILER_64_LO(data)));

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_ilkn_reg_above_64_get(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data)
{
    uint32 data_low;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);

    SOCDNX_IF_ERR_EXIT(soc_jer_ilkn_reg32_get(unit, reg, port, index, &data_low));

    sal_memcpy(data, &data_low, WORDS2BYTES(1));

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_ilkn_reg_above_64_set(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data)
{
    uint32 data_low;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memcpy(&data_low, data, WORDS2BYTES(1));

    SOCDNX_IF_ERR_EXIT(soc_jer_ilkn_reg32_set(unit, reg, port, index, data_low));

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>
