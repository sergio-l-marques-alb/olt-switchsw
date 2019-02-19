/*
 * $Id: arad_stat.c,v 1.14 Broadcom SDK $
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
 * SOC ARAD FABRIC STAT
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_STAT

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dpp/drv.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <shared/bitop.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/ARAD/arad_defs.h>
#include <soc/dpp/ARAD/arad_stat.h>
#include <bcm_int/dpp/utils.h>
#include <soc/dpp/mbcm.h>
#include <bcm_int/dpp/error.h>
#include <soc/dpp/port_sw_db.h>


STATIC int 
soc_arad_mac_controlled_counter_get(int unit, int counter_id, int port, uint64* val){

    int blk_idx, lane_idx ,link,rv;
    uint32 control_reg;
    SOCDNX_INIT_FUNC_DEFS;

    if(IS_SFI_PORT(unit,port)) {

        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        blk_idx = link/SOC_ARAD_NOF_LINKS_IN_MAC;
        lane_idx = link % SOC_ARAD_NOF_LINKS_IN_MAC;

        counter_id = counter_id-SOC_ARAD_MAC_COUNTER_FIRST;

        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, &control_reg));
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, LANE_SELECTf, lane_idx);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, COUNTER_SELECTf, counter_id);
        SOC_DPP_ALLOW_WARMBOOT_WRITE(WRITE_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, control_reg), rv);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUTr(unit, blk_idx, val));

    } else {
        COMPILER_64_SET(*val, 0, 0);
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_arad_nif_controlled_counter_get(int unit, int counter_id, int port, uint64* val)
{
    int soc_sand_rv, soc_sand_dev_id;
    SOC_SAND_64CNT  soc_sand_value;
    SOC_SAND_64CNT  soc_sand_value_1, soc_sand_value_2;
    SOCDNX_INIT_FUNC_DEFS;

    if(!IS_SFI_PORT(unit,port)) {
        

        soc_sand_dev_id = (unit);
        sal_memset(&soc_sand_value, 0, sizeof(soc_sand_value));

            switch (counter_id) {
            case ARAD_NIF_TX_NON_UNICAST_PACKETS:
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_TX_BCAST_PACKETS, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_TX_MCAST_BURSTS, &soc_sand_value_2);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_2);
                
                break;
            case ARAD_NIF_RX_NON_UNICAST_PACKETS:
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_BCAST_PACKETS, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_MCAST_BURSTS, &soc_sand_value_2);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_2);
                break;
            case ARAD_NIF_RX_LEN_1515CFG_MAX:
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_LEN_1519_2043, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_LEN_2044_4091, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_LEN_4092_9212, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_LEN_9213CFG_MAX, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                break;
            default:
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, counter_id, &soc_sand_value);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                    counter_id, port, port));
                }
            }
        COMPILER_64_SET(*val, soc_sand_value.u64.arr[1], soc_sand_value.u64.arr[0]);

    } else {
        COMPILER_64_SET(*val, 0, 0);
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}

soc_controlled_counter_t soc_arad_controlled_counter[] = {
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_BCAST_PACKETS,
        "RX BCAST PACKETS",
        "RX BCAST PACKETS",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_MCAST_BURSTS,   
        "RX MCAST BURSTS",
        "RX MCAST BURSTS",    
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_ERR_PACKETS,      
        "RX ERR PACKETS", 
        "RX ERR PACKETS", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
        },
        {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_BELOW_MIN,     
        "RX LEN BELOW MIN",
        "RX LEN BELOW MIN",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_MIN_59,
        "RX LEN MIN 59", 
        "RX LEN MIN 59",        
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_60,  
        "RX LEN 60", 
        "RX LEN 60",            
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_61_123,     
        "RX LEN 61 123",   
        "RX LEN 61 123", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_124_251,      
        "RX LEN 124 251", 
        "RX LEN 124 251", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_252_507,       
        "RX LEN 252 507",
        "RX LEN 252 507",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_508_1019,      
        "RX LEN 508 1019",
        "RX LEN 508 1019",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_1020_1514,    
        "RX LEN 1020 1514", 
        "RX LEN 1020 1514", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
        },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_1515_1518,
        "RX LEN 1515 1518",
        "RX LEN 1515 1518",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
        },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_1519_2043,  
        "RX LEN 1519 2043",   
        "RX LEN 1519 2043",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_2044_4091,
        "RX LEN 2044 4091",
        "RX LEN 2044 4091",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_4092_9212, 
        "RX LEN 4092 9212",  
        "RX LEN 4092 9212",   
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_9213CFG_MAX,  
        "RX LEN 9213CFG MAX", 
        "RX LEN 9213CFG MAX", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_1515CFG_MAX,  
        "RX LEN 1515CFG MAX", 
        "RX LEN 1515CFG MAX", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW /*special kind should not be collected*/ | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_ABOVE_MAX,
        "RX LEN ABOVE MAX",
        "RX LEN ABOVE MAX",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_BELOW_MIN, 
        "TX LEN BELOW MIN",
        "TX LEN BELOW MIN",    
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_MIN_59,  
        "TX LEN MIN 59",
        "TX LEN MIN 59",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_60,
        "TX LEN 60", 
        "TX LEN 60", 
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_61_123,  
        "TX LEN 61 123",
        "TX LEN 61 123",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_124_251,
        "TX LEN 124 251",
        "TX LEN 124 251",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_252_507, 
        "TX LEN 252 507",
        "TX LEN 252 507",      
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_508_1019,
        "TX LEN 508 1019",
        "TX LEN 508 1019",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_1020_1514,
        "TX LEN 1020 1514",
        "TX LEN 1020 1514",     
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_1515_1518,    
        "TX LEN 1515 1518",
        "TX LEN 1515 1518", 
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_1519_2043, 
        "TX LEN 1519 2043",
        "TX LEN 1519 2043",    
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_2044_4091,
        "TX LEN 2044 4091",   
        "TX LEN 2044 4091",   
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_4092_9212,
        "TX LEN 4092 9212",
        "TX LEN 4092 9212",     
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_9213CFG_MAX, 
        "TX LEN 9213CFG MAX",  
        "TX LEN 9213CFG MAX",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_BCAST_PACKETS,  
        "TX BCAST PACKETS",
        "TX BCAST PACKETS",      
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_MCAST_BURSTS,
        "TX MCAST BURSTS",
        "TX MCAST BURSTS",     
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_ERR_PACKETS,
        "TX ERR PACKETS",
        "TX ERR PACKETS",       
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_OK_OCTETS,
        "RX OK OCTETS",  
        "RX OK OCTETS",       
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_MEDIUM | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_OK_OCTETS,
        "TX OK OCTETS",
        "TX OK OCTETS",         
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_MEDIUM | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_OK_PACKETS,
        "RX OK PACKETS",
        "RX OK PACKETS",        
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_OK_PACKETS, 
        "TX OK PACKETS",
        "TX OK PACKETS",       
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_NON_UNICAST_PACKETS,
        "RX N UC PACKETS",
        "RX N UC PACKETS",        
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW /*special kind should not be collected*/  | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_NON_UNICAST_PACKETS, 
        "TX N UC PACKETS",
        "TX N UC PACKETS",       
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW /*special kind should not be collected*/ | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_ILKN_PER_CHANNEL, 
        "RX ILKN PER CHANNEL",
        "RX ILKN PER CHANNEL",       
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW /*special kind should not be collected*/ | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_ILKN_PER_CHANNEL, 
        "TX ILKN PER CHANNEL",
        "TX ILKN PER CHANNEL",       
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW /*special kind should not be collected*/ | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_TX_CONTROL_CELLS_COUNTER,
        "TX Control cells",
        "TX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_TX_DATA_CELL_COUNTER,
        "TX Data cell",
        "TX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_TX_DATA_BYTE_COUNTER,
        "TX Data byte",
        "TX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_CRC_ERRORS_COUNTER,
        "RX CRC errors",
        "RX CRC errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_CORRECTABLE_ERROR, /*SOC_ARAD_MAC_COUNTERS_RX_BEC_CRC_ERROR , SOC_ARAD_MAC_COUNTERS_RX_8B_10B_DISPARITY_ERRORS*/
        "RX (L)FEC correctable \\ BEC crc \\ 8b/10b disparity",
        "RX correctable",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_CONTROL_CELLS_COUNTER,
        "RX Control cells",
        "RX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_DATA_CELL_COUNTER,
        "RX Data cell",
        "RX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_DATA_BYTE_COUNTER,
        "RX Data byte",
        "RX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_DROPPED_RETRANSMITTED_CONTROL,
        "RX dropped retransmitted control",
        "RX drop retransmit",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_TX_BEC_RETRANSMIT,
        "TX BEC retransmit",
        "TX BEC retransmit",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_BEC_RETRANSMIT,
        "RX BEC retransmit",
        "RX BEC retransmit",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_TX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS,
        "TX Asyn fifo rate at units of 40 bits",
        "TX Asyn fifo rate",
        _SOC_CONTROLLED_COUNTER_FLAG_NOT_PRINTABLE | _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS,
        "RX Asyn fifo rate at units of 40 bits",
        "RX Asyn fifo rate",
        _SOC_CONTROLLED_COUNTER_FLAG_NOT_PRINTABLE | _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_UNCORRECTABLE_ERRORS, /*SOC_ARAD_MAC_COUNTERS_RX_BEC_RX_FAULT, SOC_ARAD_MAC_COUNTERS_RX_8B_10B_CODE_ERRORS*/
        "RX (L)FEC uncorrectable \\ BEC fault \\ 8b/10b code errors",
        "RX uncorrectable",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_LLFC_PRIMARY, 
        "RX LLFC Primary",
        "RX LLFC Primary",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_LLFC_SECONDARY, 
        "RX LLFC Secondary",
        "RX LLFC Secondary",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        NULL,
        -1,
        "",
        "",
        COUNTER_IDX_NOT_COLLECTED
    }
};

/*
 * Function:
 *      soc_arad_fabric_stat_init
 * Purpose:
 *      Init ARAD fabric stat
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      SOC_E_XXX         Operation failed
 */
soc_error_t 
soc_arad_fabric_stat_init(int unit)
{
    int i;
    uint32 reg_val, nof_inst_fmac;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARDON(unit)) {
        nof_inst_fmac = SOC_DPP_DEFS_GET(unit, nof_instances_fmac);
    } else {
        nof_inst_fmac = SOC_DPP_DEFS_GET(unit, nof_instances_fmac);
    }

    if (!SOC_WARM_BOOT(unit)) {
        for(i=0 ; i<nof_inst_fmac ; i++) {
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, i, &reg_val));
            soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_COUNTER_MODEf, 0);
            soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_BYTE_COUNTER_HEADERf, 1);
            soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, COUNTER_CLEAR_ON_READf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, i, reg_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_arad_stat_nif_init(
 * Purpose:
 *      Init ARAD nif stat
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      SOC_E_XXX         Operation failed
 */
soc_error_t 
soc_arad_stat_nif_init(int unit)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_WARM_BOOT(unit)) {
        SOC_EXIT;
    }

    /*Configuration for all modes*/
    SOCDNX_IF_ERR_EXIT(soc_arad_stat_clear_on_read_set(unit, 0));

    SOCDNX_IF_ERR_EXIT(READ_NBI_STATISTICSr(unit, &reg_val));
    soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_CNT_ALL_BY_PKTf, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_STATISTICSr(unit, reg_val));

    /*ILKN counter mode*/
    if (SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode == soc_arad_stat_ilkn_counters_mode_physical) {
        /*Do nothing  - Default configuration*/
    } else if (SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode == soc_arad_stat_ilkn_counters_mode_packets_per_channel) {
        SOCDNX_IF_ERR_EXIT(READ_NBI_STATISTICSr(unit, &reg_val));
        soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_ILKN_0_CNT_PER_CHf, 0x1);
        soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_ILKN_1_CNT_PER_CHf, 0x1);
        soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_RX_EOP_COUNT_ENABLEf, 0x1);
        soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_TX_EOP_COUNT_ENABLEf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_STATISTICSr(unit, reg_val));
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("soc_arad_stat_nif_init: unavail ilkn_counters_mode(%u)"), SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
/*
 * Function:
 *      soc_arad_stat_clear_on_read_set
 * Purpose:
 *      enable / disable nif clear on read statistic
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      enable - (IN)
 * Returns:
 *      SOC_E_XXX         Operation failed
 */
soc_error_t 
soc_arad_stat_clear_on_read_set(int unit, int enable)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_NBI_STATISTICSr(unit, &reg_val));
    soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_CLEAR_ON_READf, enable);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_STATISTICSr(unit, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}
soc_error_t 
soc_arad_stat_controlled_counter_enable_get(int unit, soc_port_t port, int index, int *enable) {
    soc_control_t	*soc;
    uint32 channel;
    SOCDNX_INIT_FUNC_DEFS;

    *enable = 1;
    soc = SOC_CONTROL(unit);

    if (BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), port)) {
        if (!(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_MAC)) {
            *enable = 0;
            SOC_EXIT;
        }
    } else {
        /*NIF*/
        if (!(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_NIF)) {
            *enable = 0;
        }
    }
    if(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_NOT_PRINTABLE) {
        *enable = 0;
    }

    /*filter channlized counter for non chanellized port*/
    if (index == ARAD_NIF_RX_ILKN_PER_CHANNEL || index == ARAD_NIF_TX_ILKN_PER_CHANNEL) {
        if (! (SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, port) && SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), port))) {
            *enable = 0;
        }

        /*not all channel ids are supported*/
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
        if (channel > SOC_ARAD_NIF_ILKN_COUNTER_PER_CHANNEL_CANNEL_SUPPORTED_MAX) {
            *enable = 0;
        }
    } else { /*filter non channlized counters for channlized port*/
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), port)) {
            soc_port_t phy_port, port_base;

            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            port_base = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (port != port_base) {
                *enable = 0;
            }
            if (SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, port) ) {
                /*counters per packet length are not supported*/
                switch (index) {
                    case ARAD_NIF_RX_LEN_BELOW_MIN:
                    case ARAD_NIF_RX_LEN_MIN_59:
                    case ARAD_NIF_RX_LEN_60:
                    case ARAD_NIF_RX_LEN_61_123:
                    case ARAD_NIF_RX_LEN_124_251:
                    case ARAD_NIF_RX_LEN_252_507:
                    case ARAD_NIF_RX_LEN_508_1019:
                    case ARAD_NIF_RX_LEN_1020_1514:
                    case ARAD_NIF_RX_LEN_1515_1518:
                    case ARAD_NIF_RX_LEN_1519_2043:
                    case ARAD_NIF_RX_LEN_2044_4091:
                    case ARAD_NIF_RX_LEN_4092_9212:
                    case ARAD_NIF_RX_LEN_9213CFG_MAX:
                    case ARAD_NIF_RX_LEN_1515CFG_MAX:
                    case ARAD_NIF_RX_LEN_ABOVE_MAX:
                    case ARAD_NIF_TX_LEN_BELOW_MIN:
                    case ARAD_NIF_TX_LEN_MIN_59:
                    case ARAD_NIF_TX_LEN_60:
                    case ARAD_NIF_TX_LEN_61_123:
                    case ARAD_NIF_TX_LEN_124_251:
                    case ARAD_NIF_TX_LEN_252_507:
                    case ARAD_NIF_TX_LEN_508_1019:
                    case ARAD_NIF_TX_LEN_1020_1514:
                    case ARAD_NIF_TX_LEN_1515_1518:
                    case ARAD_NIF_TX_LEN_1519_2043:
                    case ARAD_NIF_TX_LEN_2044_4091:
                    case ARAD_NIF_TX_LEN_4092_9212:
                        *enable = 0;
                        break;
                }
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

