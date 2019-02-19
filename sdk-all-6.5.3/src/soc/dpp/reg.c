/* 
 * $Id: reg.c,v 1.3 Broadcom SDK $
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
 * SOC register access implementation for DPP
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_REG
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/drv.h>
#include <shared/util.h>

/* Read a SOC register */
int
soc_dpp_reg32_read(int unit, uint32 addr, uint32 *data) {
    LOG_INFO(BSL_LS_SOC_REG,
             (BSL_META_U(unit,
                         "soc_dpp_reg32_read called. Addr:0x%x\n"), addr));
    
    addr = WORDS2BYTES(addr & (~0x00080000));

    *data = CMREAD(unit, addr);

    LOG_INFO(BSL_LS_SOC_REG,
             (BSL_META_U(unit,
                         "soc_dpp_reg32_read Addr:0x%x returned val:"
                         "0x%08x\n"), addr,*data));
    return SOC_E_NONE;
}

/* Read a 64-bit SOC register */
int
soc_dpp_reg64_read(int unit, uint32 addr, uint64 *data) {
    uint32  data_hi;
    uint32  data_lo;
    uint32  offset;

    LOG_INFO(BSL_LS_SOC_REG,
             (BSL_META_U(unit,
                         "soc_dpp_reg64_read called. Addr:0x%x\n"), addr));
    
    addr = WORDS2BYTES(addr & (~0x00080000));

    offset = 4;
    data_hi = CMREAD(unit, addr);
    data_lo = CMREAD(unit, (addr + offset));

    COMPILER_64_SET(*data, data_hi, data_lo);
    return SOC_E_NONE;
}


/* Write to a SOC register */
int
soc_dpp_reg32_write(int unit, uint32 addr, uint32 data) {

    LOG_INFO(BSL_LS_SOC_REG,
             (BSL_META_U(unit,
                         "soc_dpp_reg32_write called. Addr:0x%x, "
                         "data:0x%08x\n"), addr, data));
    
    addr = WORDS2BYTES(addr & (~0x00080000));

    CMWRITE(unit, addr, data);

    return SOC_E_NONE;
}

/* Write to a 64-bit SOC register */
int
soc_dpp_reg64_write(int unit, uint32 addr, uint64 data) {
    uint32  offset;

    LOG_INFO(BSL_LS_SOC_REG,
             (BSL_META_U(unit,
                         "soc_dpp_reg64_write called. Addr:0x%x\n"), addr));
    
    addr = WORDS2BYTES(addr & (~0x00080000));

    offset = 4;
    CMWRITE(unit, addr, (COMPILER_64_HI(data)));
    CMWRITE(unit, (addr + offset), COMPILER_64_LO(data));

    return SOC_E_NONE;
}


/* Read SOC register. Checks if the register is 32 or 64 bits */
int
soc_dpp_reg_read(int unit, soc_reg_t reg, uint32 addr, uint64 *data) {
    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    if (SOC_REG_IS_64(unit, reg)) {
        return soc_dpp_reg64_read(unit, addr, data);
    } else {
        uint32 data32;

        SOCDNX_IF_ERR_RETURN(soc_dpp_reg32_read(unit, addr, &data32));
        COMPILER_64_SET(*data, 0, data32);
    }

    return SOC_E_NONE;
}

/* Write SOC register. Checks if the register is 32 or 64 bits */
int
soc_dpp_reg_write(int unit, soc_reg_t reg, uint32 addr, uint64 data) {
    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    if (SOC_REG_IS_64(unit, reg)) {
        return soc_dpp_reg64_write(unit, addr, data);
    } else {
        SOCDNX_IF_ERR_RETURN(soc_dpp_reg32_write(unit, addr,
                                                COMPILER_64_LO(data)));
    }

    return SOC_E_NONE;
}
