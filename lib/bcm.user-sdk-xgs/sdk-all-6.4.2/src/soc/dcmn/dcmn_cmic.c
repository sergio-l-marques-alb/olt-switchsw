/*
 * $Id: dcmn_cmic.c,v 1.0 Broadcom SDK $
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
 * SOC DCMN IPROC
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif /* _ERR_MSG_MODULE_NAME */

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dcmn/dcmn_cmic.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dcmn/error.h>

int soc_dcmn_cmic_device_hard_reset(int unit, int reset_action)
{
    uint32 
        reg32_val = 0;
    soc_timeout_t 
        to;

    SOCDNX_INIT_FUNC_DEFS;

    if ((reset_action == SOC_DCMN_RESET_ACTION_IN_RESET) || (reset_action == SOC_DCMN_RESET_ACTION_INOUT_RESET)) {
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CPS_RESETr(unit, 0x1)); 

        soc_timeout_init(&to, 100000, 100);
        for(;;) {
            SOCDNX_IF_ERR_EXIT(READ_CMIC_CPS_RESETr(unit, &reg32_val));
#ifdef PLISIM
            if (SAL_BOOT_PLISIM) {
                reg32_val = 0x0;
            }
#endif
            if (reg32_val == 0x0) {
                break;
            }
            if (soc_timeout_check(&to)) {
                SOCDNX_EXIT_WITH_ERR(_SHR_E_INIT, (_BSL_SOC_MSG("Error: CPS reset field not asserted correctly.")));
                break;
            }
        }   
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dcmn_cmic_sbus_timeout_set(int unit, uint32 core_freq_khz, int schan_timeout)
{
    
    uint32 freq_mhz = core_freq_khz / 1000;
    uint32 ticks,
           max_uint = 0xFFFFFFFF,
           max_ticks= 0x3FFFFF;
    
    SOCDNX_INIT_FUNC_DEFS;

    /* configure ticks to be a HW timeout that is 75% of SW timeout.
     * units:
     *  schanTimeout is in microsecond
     *  frequency is recieved in KHz, and modified to be in MHz.
     *  after the modification: ticks = frequency * Timeout 
     */

    if ((max_uint / freq_mhz) > schan_timeout) { /* make sure ticks can be represented in 32 bits*/
        ticks = freq_mhz * schan_timeout;
        ticks = ((ticks / 100) * 75); /* make sure hardware timeout is smaller than software*/
    } else {
        ticks = max_ticks;
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_TIMEOUTr(unit, ticks));
 
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dcmn_cmic_pcie_userif_purge_ctrl_init(int unit)
{
    uint32 reg;

    SOCDNX_INIT_FUNC_DEFS;
    
    if (soc_feature(unit, soc_feature_cmicm) && 
        soc_feature(unit, soc_feature_sbusdma)) {
        /* Enable PCIe purge on error */
        SOCDNX_IF_ERR_EXIT(READ_CMIC_PCIE_USERIF_PURGE_CONTROLr(unit, &reg));
        soc_reg_field_set(unit, CMIC_PCIE_USERIF_PURGE_CONTROLr, &reg,
                          ENABLE_PURGE_IF_USERIF_TIMESOUTf, 1);
        soc_reg_field_set(unit, CMIC_PCIE_USERIF_PURGE_CONTROLr, &reg,
                          ENABLE_PIO_PURGE_IF_USERIF_RESETf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_PCIE_USERIF_PURGE_CONTROLr(unit, reg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



