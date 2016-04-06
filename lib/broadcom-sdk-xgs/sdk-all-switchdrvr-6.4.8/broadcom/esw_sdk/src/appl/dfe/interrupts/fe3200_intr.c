/*
 * $Id: fe3200_appl_intr.c, v1 Broadcom SDK $
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
 * 
 * Purpose:    Implements application interrupt lists for FE3200.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>

#include <soc/dfe/fe3200/fe3200_intr.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>
#include <appl/dcmn/interrupts/dcmn_intr.h>

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_APPL_INTR

/*************
 * TYPE DEFS *
 *************/
fe3200_interrupt_type fe3200_int_disable_on_init[] = {
    INVALIDr
};

fe3200_interrupt_type fe3200_int_active_on_init[] = {
    FE3200_INT_MESH_TOPOLOGY_ERROR_ECC,
    FE3200_INT_RTP_ERROR_ECC,
    FE3200_INT_RTP_DRHP_0_INT_REG,
    FE3200_INT_RTP_DRHP_1_INT_REG,
    FE3200_INT_RTP_DRHP_2_INT_REG,
    FE3200_INT_RTP_DRHP_3_INT_REG,
    FE3200_INT_RTP_CRH_0_INT_REG,
    FE3200_INT_RTP_CRH_1_INT_REG,
    FE3200_INT_RTP_CRH_2_INT_REG,
    FE3200_INT_RTP_CRH_3_INT_REG,
    FE3200_INT_RTP_GENERAL_INT_REG,
    FE3200_INT_DCMC_ERROR_ECC,
    FE3200_INT_DCL_ERROR_ECC,
    FE3200_INT_OCCG_ERROR_ECC,
    FE3200_INT_CCS_ERROR_ECC,
    FE3200_INT_DCM_ERROR_ECC,
    FE3200_INT_DCH_ERROR_ECC,
    FE3200_INT_FSRD_ERROR_ECC,
    FE3200_INT_FSRD_INT_REG_0,
    FE3200_INT_FSRD_INT_REG_1,
    FE3200_INT_FSRD_INT_REG_2,
    FE3200_INT_FMAC_ERROR_ECC,
    FE3200_INT_FMAC_INT_REG_1,
    FE3200_INT_FMAC_INT_REG_2,
    FE3200_INT_FMAC_INT_REG_3,
    FE3200_INT_FMAC_INT_REG_4,
    FE3200_INT_FMAC_INT_REG_5,
    FE3200_INT_FMAC_INT_REG_6,
    FE3200_INT_FMAC_INT_REG_7,
    FE3200_INT_FMAC_INT_REG_8,
    FE3200_INT_ECI_ERROR_ECC,
    FE3200_INT_CCS_ECC_ECC_1B_ERR_INT,
    FE3200_INT_CCS_ECC_ECC_2B_ERR_INT,
    FE3200_INT_DCH_ECC_ECC_1B_ERR_INT,
    FE3200_INT_DCH_ECC_ECC_2B_ERR_INT,
    FE3200_INT_DCH_ECC_PARITY_ERR_INT,
    FE3200_INT_DCL_ECC_ECC_1B_ERR_INT,
    FE3200_INT_DCL_ECC_ECC_2B_ERR_INT,
    FE3200_INT_DCL_ECC_PARITY_ERR_INT,
    FE3200_INT_DCMC_ECC_ECC_1B_ERR_INT,
    FE3200_INT_DCMC_ECC_ECC_2B_ERR_INT,
    FE3200_INT_DCMC_ECC_PARITY_ERR_INT,
    FE3200_INT_DCM_ECC_ECC_1B_ERR_INT,
    FE3200_INT_DCM_ECC_ECC_2B_ERR_INT,
    FE3200_INT_DCM_ECC_PARITY_ERR_INT,
    FE3200_INT_ECI_ECC_ECC_1B_ERR_INT,
    FE3200_INT_ECI_ECC_ECC_2B_ERR_INT,
    FE3200_INT_ECI_ECC_PARITY_ERR_INT,
    FE3200_INT_FMAC_ECC_ECC_1B_ERR_INT,
    FE3200_INT_FMAC_ECC_ECC_2B_ERR_INT,
    FE3200_INT_FMAC_ECC_PARITY_ERR_INT,
    FE3200_INT_FSRD_ECC_ECC_1B_ERR_INT,
    FE3200_INT_FSRD_ECC_ECC_2B_ERR_INT,
    FE3200_INT_FSRD_ECC_PARITY_ERR_INT,
    FE3200_INT_MESH_TOPOLOGY_ECC_ECC_1B_ERR_INT,
    FE3200_INT_MESH_TOPOLOGY_ECC_ECC_2B_ERR_INT,
    FE3200_INT_MESH_TOPOLOGY_ECC_PARITY_ERR_INT,
    FE3200_INT_OCCG_ECC_ECC_1B_ERR_INT,
    FE3200_INT_OCCG_ECC_ECC_2B_ERR_INT,
    FE3200_INT_OCCG_ECC_PARITY_ERR_INT,
    FE3200_INT_RTP_ECC_ECC_1B_ERR_INT,
    FE3200_INT_RTP_ECC_ECC_2B_ERR_INT,
    FE3200_INT_RTP_ECC_PARITY_ERR_INT,
    INVALIDr
};

fe3200_interrupt_type fe3200_int_disable_print_on_init[] = {
    INVALIDr
};

/*************
 * FUNCTIONS *
 *************/
int
fe3200_interrupt_cmn_param_init(int unit, intr_common_params_t* common_params)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(common_params);

    common_params->int_disable_on_init = fe3200_int_disable_on_init;
    common_params->int_disable_print_on_init = fe3200_int_disable_print_on_init;
    common_params->int_active_on_init = fe3200_int_active_on_init;

exit:
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

