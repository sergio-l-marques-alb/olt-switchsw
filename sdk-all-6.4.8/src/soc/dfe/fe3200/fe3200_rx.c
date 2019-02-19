/*
 * $Id: fe3200_rx.c,v 1.8.64.2 Broadcom SDK $
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
 * SOC FE3200 RX
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_RX

#include <bcm/types.h>

#include <shared/bsl.h>

#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/defs.h>
#include <soc/types.h>
#include <soc/error.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>

#include <soc/dfe/fe3200/fe3200_rx.h>
#include <soc/dfe/fe3200/fe3200_stack.h>

#define LINKS_ARRAY_SIZE (SOC_DFE_MAX_NOF_LINKS/32 + (SOC_DFE_MAX_NOF_LINKS%32? 1:0) + 1 /*for ECC*/)

soc_error_t
soc_fe3200_rx_cpu_address_modid_set(int unit, int num_of_cpu_addresses, int *cpu_addresses)
{
    int i;
    bcm_module_t alrc_max_base_index, update_base_index;
    uint32 mem_val[LINKS_ARRAY_SIZE];
    uint32 reg_val32;

    SOCDNX_INIT_FUNC_DEFS;

    /* Check for valid num of cpu addresses */
    if ((num_of_cpu_addresses < 0) || (SOC_FE3200_RX_MAX_NOF_CPU_BUFFERS > SOC_FE3200_RX_MAX_NOF_CPU_BUFFERS))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("num_of_cpu_addresses invalid \n")));
    }

    /* issue a warning if a cpu address is not in the interval [AlrcMaxBaseIndex,UpdateBaseIndex]*/

    SOCDNX_IF_ERR_EXIT(soc_fe3200_stk_module_max_all_reachable_get(unit, &alrc_max_base_index));
    SOCDNX_IF_ERR_EXIT(soc_fe3200_stk_module_max_fap_get(unit, &update_base_index));

    for (i = 0 ; i < num_of_cpu_addresses ; i++)
    {
        if (!((cpu_addresses[i] >= alrc_max_base_index) && (cpu_addresses[i] <= update_base_index)))
        {
            LOG_CLI((BSL_META_U(unit,
                "warning: cpu address is not in the interval [AlrcMaxBaseIndex,UpdateBaseIndex] (the ignored interval for calculating all reachable vector)\n")));
        }
    }


    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_FE_DEST_IDS_1r(unit, &reg_val32));
    /* updating first cpu address */
    if (num_of_cpu_addresses >= 1)
    {
        soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_0f, cpu_addresses[0]);
    }
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_0_ENf, num_of_cpu_addresses >= 1 ? 1:0);

    if (num_of_cpu_addresses >= 2)
    {
        soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_1f, cpu_addresses[1]);
    }
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_1_ENf, num_of_cpu_addresses >= 2 ? 1:0);

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_FE_DEST_IDS_1r(unit, reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_FE_DEST_IDS_2r(unit, &reg_val32));

    if (num_of_cpu_addresses >= 3)
    {
        soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val32, FE_DEST_ID_2f, cpu_addresses[2]);
    }

    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val32, FE_DEST_ID_2_ENf, num_of_cpu_addresses >= 3 ? 1:0);

    if (num_of_cpu_addresses >= 4)
    {
        soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val32, FE_DEST_ID_3f, cpu_addresses[3]);
    }

    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val32, FE_DEST_ID_3_ENf, num_of_cpu_addresses >= 4 ? 1:0);

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_FE_DEST_IDS_2r(unit, reg_val32));

    if (SOC_DFE_IS_FE13(unit) && !SOC_DFE_CONFIG(unit).fabric_local_routing_enable)
    {
        for (i=0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch)/2; i++) { /*FE1*/
            SOCDNX_IF_ERR_EXIT(READ_DCH_GLOBAL_FE_DEST_IDS_1r(unit, i, &reg_val32));
            soc_reg_field_set(unit, DCH_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_0_ENf, 0X0);
            soc_reg_field_set(unit, DCH_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_1_ENf, 0x0);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_GLOBAL_FE_DEST_IDS_1r(unit, i, reg_val32));
        }

        /* 
         * FE13 ignores cpu destinations - 
         * shouold set reachablity tables to -1 on order to indicate that the cpu is reachable -                                                       
         */
        SHR_BITSET_RANGE(mem_val, 0, SOC_DFE_DEFS_GET(unit, nof_links));
        for (i = 0 ; i < num_of_cpu_addresses ; i++)
        {
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_RMHMTm(unit,MEM_BLOCK_ALL,cpu_addresses[i], mem_val));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_DUCTPm(unit, 0, MEM_BLOCK_ALL,cpu_addresses[i], mem_val));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_DUCTPm(unit, 1, MEM_BLOCK_ALL,cpu_addresses[i], mem_val));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_DUCTPm(unit, 2, MEM_BLOCK_ALL,cpu_addresses[i], mem_val));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_DUCTPm(unit, 3, MEM_BLOCK_ALL,cpu_addresses[i], mem_val));
        }                  
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_rx_cpu_address_modid_init(int unit)
{
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_FE_DEST_IDS_1r(unit, &reg_val));
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val, FE_DEST_ID_0_ENf, 0);
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val, FE_DEST_ID_1_ENf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_FE_DEST_IDS_1r(unit, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_FE_DEST_IDS_2r(unit, &reg_val));
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val, FE_DEST_ID_2_ENf, 0);
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val, FE_DEST_ID_3_ENf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_FE_DEST_IDS_2r(unit, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME
