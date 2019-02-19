/*
 * $Id: dfe_intr.c,v 1.87 Broadcom SDK $

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

/*************
 * INCLUDES  *
 *************/
#include <soc/error.h>
#include <soc/ipoll.h>
#include <soc/drv.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h> 

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

/*************
 * FUNCTIONS *
 *************/
int soc_fe1600_interrupt_deinit(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
                           
    if(!SAL_BOOT_NO_INTERRUPTS) {
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_disconnect(unit) < 0) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "error disconnecting polled interrupt mode\n")));
            }
        } else {
            /* unit # is ISR arg */
            if (soc_cm_interrupt_disconnect(unit) < 0) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "could not disconnect interrupt line\n")));
            }
        }
    }



    if(!SAL_BOOT_NO_INTERRUPTS) 
    {
        fe1600_interrupts_dfe_control_data_deinit(unit);
        
        fe1600_interrupts_array_deinit(unit);       
    }

    SOCDNX_FUNC_RETURN;
}

int soc_fe1600_interrupt_init(int unit) 
{
    int rv;
    soc_control_t* soc;

    SOCDNX_INIT_FUNC_DEFS;

    soc = SOC_CONTROL(unit);

    if(!SAL_BOOT_NO_INTERRUPTS) {
        rv = fe1600_interrupts_array_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = fe1600_interrupts_dfe_control_data_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }
 

    /* take from HW the interrupts mask */
    /* note: there is al so high level of cmic mask that dosn't saved in wormboot. */

    soc_intr_block_lo_disable(unit, 0xFFFFFFFF);
    soc_intr_block_hi_disable(unit, 0xFFFFFFFF);

    if(!SAL_BOOT_NO_INTERRUPTS) {
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_connect(unit, dfe_local_soc_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
            }
            soc->soc_flags |= SOC_F_POLLED;
        } else {
            /* unit # is ISR arg */
            if (soc_cm_interrupt_connect(unit, dfe_local_soc_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("could not connect interrupt line")));
            }
        }
    }
exit:
    if (SOCDNX_FUNC_ERROR)
    {
        soc_fe1600_interrupt_deinit(unit);
    }
    SOCDNX_FUNC_RETURN;
}

