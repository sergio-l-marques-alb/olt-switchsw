/*
 * $Id: interrupt_handler.c,v 1.22 Broadcom SDK $
 *
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
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_BCM_INTR

/* 
 *  include  
 */ 
#include <sal/core/time.h>
#include <sal/core/dpc.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h>
#include <soc/dfe/fe1600/fe1600_port.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm_int/dfe_dispatch.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>

#include <appl/dfe/interrupts/interrupt_handler.h>
#include <appl/dfe/interrupts/interrupt_handler_cb_func.h>
#include <appl/dfe/interrupts/interrupt_handler_corr_act_func.h>

#include <appl/dcmn/interrupts/interrupt_handler.h>

fe1600_interrupt_type fe1600_int_disable_on_init[] = {
    FE1600_INT_LAST /*always last*/
};

fe1600_interrupt_type fe1600_int_active_on_init[] = {
    FE1600_INT_DCH_DCHDESCCNTOP,
    FE1600_INT_DCH_DCHDESCCNTOS,
    FE1600_INT_DCH_OUTOFSYNCINTP,
    FE1600_INT_DCH_OUTOFSYNCINTS,
/*
 *  Interrupts that are Vectors pointing to other Interrupt Register.
 */
    FE1600_INT_DCH_INTREG1INT,
    FE1600_INT_DCH_INTREG2INT,
    FE1600_INT_RTP_DRHP0INTREG,
    FE1600_INT_RTP_DRHP1INTREG,
    FE1600_INT_RTP_DRHP2INTREG,
    FE1600_INT_RTP_DRHP3INTREG,
    FE1600_INT_RTP_DRHS0INTREG,
    FE1600_INT_RTP_DRHS1INTREG,
    FE1600_INT_RTP_DRHS2INTREG,
    FE1600_INT_RTP_DRHS3INTREG,
    FE1600_INT_RTP_CRH0INTREG,
    FE1600_INT_RTP_CRH1INTREG,
    FE1600_INT_RTP_GENERALINTREG,
    FE1600_INT_RTP_ECC1BERRINTREG,
    FE1600_INT_RTP_ECC2BERRINTREG,
    FE1600_INT_RTP_PARERRINTREG,
    FE1600_INT_MAC_INTREG1,
    FE1600_INT_MAC_INTREG2,
    FE1600_INT_MAC_INTREG3,
    FE1600_INT_MAC_INTREG4,
    FE1600_INT_MAC_INTREG5,
    FE1600_INT_MAC_INTREG6,
    FE1600_INT_MAC_INTREG7,
    FE1600_INT_MAC_INTREG8,
    FE1600_INT_MAC_INTREG9,
    FE1600_INT_SRD_INTREG0,
    FE1600_INT_SRD_INTREG1,
    FE1600_INT_SRD_INTREG2,
    FE1600_INT_SRD_INTREG3,
/*
 * Memory Error Interrupts
 */
    FE1600_INT_DCH_ECC_1BERRINT,
    FE1600_INT_DCH_ECC_2BERRINT,
    FE1600_INT_DCH_PARITYERRINT,
    FE1600_INT_DCL_ECC_1BERRINT,
    FE1600_INT_DCL_ECC_2BERRINT,
    FE1600_INT_DCL_PARITYERRINT,
    FE1600_INT_DCMA_ECC_1BERRINT,
    FE1600_INT_DCMA_ECC_2BERRINT,
    FE1600_INT_DCMA_PARITYERRINT,
    FE1600_INT_DCMB_ECC_1BERRINT,
    FE1600_INT_DCMB_ECC_2BERRINT,
    FE1600_INT_DCMB_PARITYERRINT,
    FE1600_INT_CCS_ECC_1BERRINT,
    FE1600_INT_CCS_ECC_2BERRINT,
    FE1600_INT_CCS_PARITYERRINT,
    FE1600_INT_RTP_ECC1BERRINTREG,
    FE1600_INT_RTP_ECC2BERRINTREG,
    FE1600_INT_RTP_PARERRINTREG,
    FE1600_INT_RTP_ECC_1BERRINT,
    FE1600_INT_RTP_ECC_2BERRINT,
    FE1600_INT_RTP_PARITYERRINT,
    FE1600_INT_MAC_ECC_1BERRINT,
    FE1600_INT_MAC_ECC_2BERRINT,
    FE1600_INT_LAST /*always last*/
};

fe1600_interrupt_type fe1600_int_disable_print_on_init[] = {
    FE1600_INT_LAST
};

soc_mem_t fe1600_cached_mem[] = {
    RTP_MULTI_TBm,
    RTP_SCTINCm,
    RTP_TOTSFm,
    NUM_SOC_MEM
};

int
fe1600_interrupt_handler_init_cmn_param(int unit, interrupt_common_params_t* common_params)
{
    int nof_interrupts;
    int rc;

    SOC_INIT_FUNC_DEFS;

    SOC_NULL_CHECK(common_params);
 
    /*get number of interrupts */
    rc = soc_fe1600_nof_interrupts(unit, &nof_interrupts);
    _SOC_IF_ERR_EXIT(rc);
  
    common_params->nof_interrupts = nof_interrupts;
    common_params->interrupt_add_interrupt_handler_init = fe1600_interrupt_add_interrupt_handler_init;
    common_params->interrupt_handle_block_instance = soc_dfe_nof_block_instances;
    common_params->int_disable_on_init = fe1600_int_disable_on_init;
    common_params->int_active_on_init = fe1600_int_active_on_init;
    common_params->int_disable_print_on_init = fe1600_int_disable_print_on_init;
    common_params->cached_mem = fe1600_cached_mem;

exit:
    SOC_FUNC_RETURN;

}

#undef _ERR_MSG_MODULE_NAME


