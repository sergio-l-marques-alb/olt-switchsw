/*
 * $Id: $
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
 * File: qax_ports.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/drv.h>

#define SOC_QAX_ILKN_NIF_PORT_ILKN0  120
#define SOC_QAX_ILKN_NIF_PORT_ILKN1  121
#define SOC_QAX_ILKN_NIF_PORT_ILKN2  122
#define SOC_QAX_ILKN_NIF_PORT_ILKN3  123


int
soc_qax_port_nrdy_th_optimal_value_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *value)
{
    uint32 num_of_lanes, if_rate_mbps;
    soc_port_if_t interface;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if(interface == SOC_PORT_IF_CPU || interface == SOC_PORT_IF_OLP) {
        *value = 36;
    } else if (interface == SOC_PORT_IF_OAMP) {
        *value = 66;
    } else if (interface == SOC_PORT_IF_RCY) {
        *value = 518;
    } else if (interface == SOC_PORT_IF_ILKN) {
        if (if_rate_mbps <= 300000) {
            *value = 260;
        } else { /* if_rate_mpbs <= 600000 */
            *value = 518;
        }
    } else if (interface == SOC_PORT_IF_QSGMII) {
        *value = 22;
    } else if (interface == SOC_PORT_IF_IPSEC || interface == SOC_PORT_IF_SAT) {
        *value = 130;
    } else if (interface == SOC_PORT_IF_LBG) {
        *value = 30;
    } else { /* value depends on number of lanes and rate of the interface */
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_of_lanes));

        if (num_of_lanes == 1) {
            if (if_rate_mbps <= 1000) { /* 1G */
                *value = 118;
            } else if (if_rate_mbps <= 10000) { /* 10G */
                *value = 96;
            } else  { /* 25G */
                *value = 64;
            }

        } else if (num_of_lanes == 2) {
            if (if_rate_mbps <= 10000) { /*10G */
                *value = 224;
            } else if (if_rate_mbps <= 25000) { /* 20G, 25G */
                *value = 192;
            } else { /* 40G, 50G */
                *value = 128;
            }

        } else if (num_of_lanes == 4) {
            if (if_rate_mbps <= 10000) { /* 10G */
                *value = 480;
            } else if (if_rate_mbps <= 25000) { /* 20G, 25G */
                *value = 448;
            } else if (if_rate_mbps <= 50000) { /* 40G, 50G */
                *value = 384;
            } else { /* 100G */
                *value = 260;
            }
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
* NAME:
*     soc_qax_port_swap_global_info_[g|s]et
* TYPE:
*   PROC
* FUNCTION:
*     Set or get the global port swap configuration. 
* INPUT:
*  int                              unit -
*       Identifier of the device to access.
*  soc_ppc_port_swap_global_info    ports_swap_info -
*       Port swap global info to set / get.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

soc_error_t 
soc_qax_port_swap_global_info_set(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info){
    int rv;
    uint64 reg_val_64;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val_64);

    /* Fill the buffer */        
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, GLOBAL_TAG_SWAP_SIZEf, ports_swap_info->global_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_OFFSET_0f, ports_swap_info->tag_swap_n_offset_0);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_OFFSET_1f, ports_swap_info->tag_swap_n_offset_1);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TPID_0_TAG_SWAP_SIZEf, ports_swap_info->tpid_0_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TPID_1_TAG_SWAP_SIZEf, ports_swap_info->tpid_1_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_TPID_0f, ports_swap_info->tag_swap_n_tpid_0);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_TPID_1f, ports_swap_info->tag_swap_n_tpid_1);

    /* Write to the register. It's an array register, duplicated by number of cores. */
    for (i = 0 ; i < SOC_DPP_DEFS_GET(unit, nof_cores) ; i++) {
        rv = WRITE_IRE_TAG_SWAP_CONFIGURATIONr(unit, i, reg_val_64);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_qax_port_swap_global_info_get(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info){
    int rv;
    uint64 reg_val_64;

    SOCDNX_INIT_FUNC_DEFS;

    /* Read from the register. */
    rv = READ_IRE_TAG_SWAP_CONFIGURATIONr(unit, IRE_BLOCK(unit), &reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);


    /* Fill the buffer */        
    ports_swap_info->global_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, GLOBAL_TAG_SWAP_SIZEf);   
    ports_swap_info->tag_swap_n_offset_0    = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_OFFSET_0f);  
    ports_swap_info->tag_swap_n_offset_1    = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_OFFSET_1f);
    ports_swap_info->tpid_0_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TPID_0_TAG_SWAP_SIZEf);
    ports_swap_info->tpid_1_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TPID_1_TAG_SWAP_SIZEf);
    ports_swap_info->tag_swap_n_tpid_0      = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_TPID_0f);
    ports_swap_info->tag_swap_n_tpid_1      = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_TPID_1f);

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_port_ilkn_nif_port_get(int unit, uint32 ilkn_intf_offset, uint32* nif_port)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (ilkn_intf_offset) {
    case 0:
        *nif_port = SOC_QAX_ILKN_NIF_PORT_ILKN0;
        break;
    case 1:
        *nif_port = SOC_QAX_ILKN_NIF_PORT_ILKN1;
        break;
    case 2:
        *nif_port = SOC_QAX_ILKN_NIF_PORT_ILKN2;
        break;
    case 3:
        *nif_port = SOC_QAX_ILKN_NIF_PORT_ILKN3;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid ILKN interface offset %d"),ilkn_intf_offset));
        break;
    }
exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
