/*
 * $Id: ddrc28.c,v 1.1.2.2 Broadcom SDK $
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
 * File:       ddrc28.c
 * Purpose:    Phy driver for 28nm combo DDR phy
 */

#include <shared/bsl.h>

#include <sal/types.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/ddrc28.h>
#include <soc/shmoo_combo28.h>

#define DDRC28_REG_INFO(reg) (soc_phy_ddrc28_reg_list[reg])
#define DDRC28_REG_IS_VALID(reg) ((reg) >= 0 && (reg) < SOC_PHY_DDRC28_NUM_REGS)

int
soc_phy_ddrc28_reg32_read(int unit, soc_reg_t reg, int instance, uint32 *data)
{
    uint32 reg_addr = DDRC28_REG_INFO(reg).offset;

    return soc_combo28_phy_reg_read(unit, instance, reg_addr, 
                                                  data);
}

int
soc_phy_ddrc28_reg32_write(int unit, soc_reg_t reg, int instance, uint32 data)
{
    uint32 reg_addr = DDRC28_REG_INFO(reg).offset;

    return soc_combo28_phy_reg_write(unit, instance, reg_addr, 
                                                   data);
}

void
soc_phy_ddrc28_reg_field_set(int unit, soc_reg_t reg, uint32 *regval,
                             soc_field_t field, uint32 value)
{
    soc_field_info_t *finfop;
    uint32           mask;

    if (!DDRC28_REG_IS_VALID(reg)) {
        assert(DDRC28_REG_IS_VALID(reg));
    }

    SOC_FIND_FIELD(field,
                   DDRC28_REG_INFO(reg).fields,
                   DDRC28_REG_INFO(reg).nFields,
                   finfop);
    if (finfop == NULL) {
        assert(finfop);
    }

    if (finfop->len < 32) {
        mask = (1 << finfop->len) - 1;

        if ((value & ~mask) != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "soc_phy_ddrc28_reg_field_set(): value too big for field\n")));
            assert((value & ~mask) == 0);
        }
    } else {
        mask = -1;
    }

    *regval = (*regval & ~(mask << finfop->bp)) | value << finfop->bp;
}

uint32
soc_phy_ddrc28_reg_field_get(int unit, soc_reg_t reg, uint32 regval, 
                             soc_field_t field)
{
    soc_field_info_t *finfop;
    uint32           val;

    if (!DDRC28_REG_IS_VALID(reg)) {
        assert(DDRC28_REG_IS_VALID(reg));
    }

    SOC_FIND_FIELD(field,
                   DDRC28_REG_INFO(reg).fields,
                   DDRC28_REG_INFO(reg).nFields,
                   finfop);

    if (finfop == NULL) {
        assert(finfop);
    }

    /* coverity[var_deref_op : FALSE] */
    val = regval >> finfop->bp;
    if (finfop->len < 32) {
        return val & ((1 << finfop->len) - 1);
    } else {
        return val;
    }

}
