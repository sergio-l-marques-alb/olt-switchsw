/*
 * $Id: qax_sram.c Exp $
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
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/*
 * Includes
 */
#include <shared/bsl.h>

/* SAL includes */
#include <sal/appl/sal.h>

/* SOC includes */
#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>

/* SOC DPP includes */
#include <soc/dpp/QAX/qax_sram.h>
#include <soc/dpp/QAX/qax_link_bonding.h>

int soc_qax_sram_conf_set(int unit)
{

    uint32 mem_entry[SOC_MAX_MEM_WORDS] = {0};
    uint32 reg32_val;
    uint64 reg64_val;

    SOCDNX_INIT_FUNC_DEFS;

    /* SPB_STATIC_CONFIGURATIONr */
    COMPILER_64_ZERO(reg64_val);
    SOCDNX_IF_ERR_EXIT( READ_SPB_STATIC_CONFIGURATIONr(unit, &reg64_val));
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, DDC_2_NUM_OF_BUFF_THf, 4);
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, DEL_NUM_OF_BUFF_AF_THf, 0x7d0);
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, DEL_NUM_OF_PKT_AF_THf, 0x64);
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, PTC_PDQ_RDY_TH_WORDf, 8);
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, STOP_IRE_WHEN_FBC_EMPTYf, 1);
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, STOP_IRE_THf, 5);
    SOCDNX_IF_ERR_EXIT( WRITE_SPB_STATIC_CONFIGURATIONr(unit, reg64_val));

    /* SPB_CONTEXT_MRUm */
    sal_memset(mem_entry, 0, SOC_MAX_MEM_WORDS);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, mem_entry, MAX_SIZEf, 0x2fff);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, mem_entry, MAX_ORG_SIZEf, 0x2fff);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, mem_entry, MIN_ORG_SIZEf, 0x20);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, mem_entry, MIN_SIZEf, 0x20);
    SOCDNX_IF_ERR_EXIT( dcmn_fill_table_with_entry( unit, SPB_CONTEXT_MRUm, MEM_BLOCK_ALL, mem_entry));

    /* SPB_DYNAMIC_CONFIGURATIONr */
    SOCDNX_IF_ERR_EXIT( READ_SPB_DYNAMIC_CONFIGURATIONr(unit,&reg32_val));
    soc_reg_field_set(unit, SPB_DYNAMIC_CONFIGURATIONr, &reg32_val, SOC_IS_QUX(unit)?MAX_BUFFERS_THRESHOLDf:FIELD_1_6f, 0x2e);
    soc_reg_field_set(unit, SPB_DYNAMIC_CONFIGURATIONr, &reg32_val, FBC_INITf, 1);
    SOCDNX_IF_ERR_EXIT( WRITE_SPB_DYNAMIC_CONFIGURATIONr(unit,reg32_val));
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, SPB_DYNAMIC_CONFIGURATIONr, REG_PORT_ANY, FBC_INITf, 0));

    /* Init SRAM for LBG */
#ifdef BCM_QAX_SUPPORT
    if (SOC_IS_QAX(unit) && soc_property_get(unit, spn_LINK_BONDING_ENABLE, 0)) {
        SOCDNX_IF_ERR_EXIT(qax_lb_ing_init(unit));
    }
#endif

exit:
    SOCDNX_FUNC_RETURN;
}
