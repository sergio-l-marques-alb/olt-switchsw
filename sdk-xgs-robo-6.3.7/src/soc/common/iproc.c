/*
 * $Id: iproc.c 1.17 Broadcom SDK $
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
 * iProc support
 */

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/types.h>
#include <shared/alloc.h>
#include <soc/memtune.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <ibde.h>

#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>

/*
 * Function:
 *      soc_iproc_init
 * Purpose:
 *      Initialize iProc subsystem
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_XXX
 */
int soc_iproc_init(int unit)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_deinit
 * Purpose:
 *      Free up resources aquired by init.
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *       This function is currently not used. 
 */
int soc_iproc_deinit(int unit)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_getreg
 * Purpose:
 *      Read iProc register outside CMIC
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_XXX
 */
int soc_iproc_getreg(int unit, uint32 addr, uint32 *data)
{
    *data = soc_cm_iproc_read(unit, addr);
    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_iproc_setreg
 * Purpose:
 *      Write iProc register outside CMIC
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_XXX
 */
int soc_iproc_setreg(int unit, uint32 addr, uint32 data)
{
    soc_cm_iproc_write(unit, addr, data);
    return SOC_E_NONE;
}

/*
 * Function:    soc_iproc_shutdown
 * Purpose: Put an IPROC into sleep state
 * Parameters:  unit - unit number
 *              cpu_mask - mask of cores to shutdown
 *              level - shutdown level: 0=full, 1=partial
 * Returns: SOC_E_xxxx
 */
int
soc_iproc_shutdown(int unit, uint32 cpu_mask, int level)
{
    uint32      i, rval;
    uint32      sram_base, sram_size;
    uint32      iproc_addr;

    if (!soc_feature(unit, soc_feature_iproc)) {
        return (SOC_E_FAIL);
    }

    soc_cm_debug(DK_VERBOSE, "Unit:%d iproc_shutdown\n", unit);

    if (SOC_IS_HELIX4(unit)) {
        /* Assumes booting in QSPI mode with MDIO tied high */
        sram_base = 0x1b000000;
        sram_size = 512 * 1024;
        iproc_addr = sram_base + sram_size - 8;

        /* load WFI loop into SRAM (ARM mode) */
        soc_cm_iproc_write(unit, iproc_addr, 0xe320f003);
        soc_cm_iproc_write(unit, iproc_addr + 4, 0xeafffffd);

        /* Update LUT to point at WFI loop */
        for (i = 0; i < 8; ++i) {
            soc_cm_iproc_write(unit, 0xffff0400 + i*4, iproc_addr);
        }
        /* core 0 should be in WFI now */
        if (level == 0) {
            if (cpu_mask & 0x2) {
                READ_CRU_IHOST_PWRDWN_ENr(unit, &rval);
                soc_reg_field_set(unit, CRU_IHOST_PWRDWN_ENr, &rval, 
                                  LOGIC_CLAMP_ON_NEON1f, 1);
                WRITE_CRU_IHOST_PWRDWN_ENr(unit, rval);
                soc_reg_field_set(unit, CRU_IHOST_PWRDWN_ENr, &rval, 
                                  LOGIC_PWRDOWN_NEON1f, 1);
                WRITE_CRU_IHOST_PWRDWN_ENr(unit, rval);
                soc_reg_field_set(unit, CRU_IHOST_PWRDWN_ENr, &rval, 
                                  LOGIC_CLAMP_ON_CPU1f, 1);
                WRITE_CRU_IHOST_PWRDWN_ENr(unit, rval);
                soc_reg_field_set(unit, CRU_IHOST_PWRDWN_ENr, &rval, 
                                  RAM_CLAMP_ON_CPU1f, 1);
                WRITE_CRU_IHOST_PWRDWN_ENr(unit, rval);
                soc_reg_field_set(unit, CRU_IHOST_PWRDWN_ENr, &rval, 
                                  LOGIC_PWRDOWN_CPU1f, 1);
                WRITE_CRU_IHOST_PWRDWN_ENr(unit, rval);
                soc_reg_field_set(unit, CRU_IHOST_PWRDWN_ENr, &rval, 
                                  RAM_PWRDOWN_CPU1f, 1);
                WRITE_CRU_IHOST_PWRDWN_ENr(unit, rval);
            }
            /* Put cpu0 into shutdown after 100 clocks */
            READ_CRU_CPU0_POWERDOWNr(unit, &rval);
            soc_reg_field_set(unit, CRU_CPU0_POWERDOWNr, &rval, 
                              START_CPU0_POWERDOWN_SEQf, 1);
            soc_reg_field_set(unit, CRU_CPU0_POWERDOWNr, &rval, 
                              CPU0_POWERDOWN_TIMERf, 100);
            WRITE_CRU_CPU0_POWERDOWNr(unit, rval);
        }
    }
    return (SOC_E_NONE);
}

#endif
