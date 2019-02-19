/*
 * $Id: $
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
 * File: jer_egr_queuing.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/dpp/JER/jer_fabric.h>

#define JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP 16
int
  soc_jer_mgmt_credit_worth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *credit_worth
  )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_IPST_CREDIT_CONFIG_1r(unit, &reg_val));
    *credit_worth = soc_reg_field_get(unit, IPST_CREDIT_CONFIG_1r, reg_val, CREDIT_VALUE_0f);

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
* Set the fabric system ID of the device. Must be unique in the system.
*********************************************************************/
uint32 jer_mgmt_system_fap_id_set(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  sys_fap_id
  )
{
    uint32 reg32, group_ctx, fap_id_plus_1 = sys_fap_id + 1;
    uint64 reg64;
    int is_mesh, offset, i;
    SHR_BITDCLNAME (dqcq_map, 32);
    SOCDNX_INIT_FUNC_DEFS;
    if (sys_fap_id >= ARAD_NOF_FAPS_IN_SYSTEM || (sys_fap_id % SOC_DPP_DEFS_GET(unit, nof_cores))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("FAP ID %u is illegal, must be a multiple if %u and under %u."),
          sys_fap_id, SOC_DPP_DEFS_GET(unit, nof_cores), ARAD_NOF_FAPS_IN_SYSTEM));
    }
    /* configure the IDs of all cores, and configure traffic to local cores not to go through the fabric */
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32, DEV_ID_0f, sys_fap_id);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32, DEV_ID_1f, fap_id_plus_1);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32, FORCE_FABRICf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32));

    COMPILER_64_ZERO(reg64);
    soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MAP_LSBf, sys_fap_id | (sys_fap_id << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
    soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MASKf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64));
    COMPILER_64_ZERO(reg64);
    soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MAP_MSBf, fap_id_plus_1 | (fap_id_plus_1 << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
    soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MASK_1f, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64));

    if (SOC_DPP_CONFIG(unit)->tdm.is_bypass &&
        SOC_DPP_CONFIG(unit)->arad->init.fabric.is_128_in_system &&
        SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_TDM_CONFIGURATIONr,
          REG_PORT_ANY, 0, TDM_SOURCE_FAP_IDf,
          sys_fap_id + SOC_DPP_CONFIG(unit)->arad->tdm_source_fap_id_offset));
    }

    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    if (is_mesh) { /*config dqcq map with local destinations*/
        for (group_ctx = 0, i = sys_fap_id; i < sys_fap_id + 1; ++group_ctx, ++i) {
            offset = (i % 16) * SOC_JER_FABRIC_GROUP_CTX_LENGTH; /*isolate bits 0:3*/
            if (offset < SOC_JER_FABRIC_STK_FAP_GROUP_SIZE) {
                SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, &reg64)); 
                *dqcq_map = soc_reg64_field32_get(unit,ECI_GLOBAL_DQCQ_MAP_CFG_1r, reg64, DEV_ID_TO_DQCQ_MAP_LSBf);
                SHR_BITCOPY_RANGE(dqcq_map, offset, &group_ctx, 0, SOC_JER_FABRIC_GROUP_CTX_LENGTH);
                soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MAP_LSBf, *dqcq_map);
                SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64));
            } else {
                offset -= SOC_JER_FABRIC_STK_FAP_GROUP_SIZE;
                SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, &reg64)); 
                *dqcq_map = soc_reg64_field32_get(unit,ECI_GLOBAL_DQCQ_MAP_CFG_2r, reg64, DEV_ID_TO_DQCQ_MAP_MSBf);
                SHR_BITCOPY_RANGE(dqcq_map, offset, &group_ctx, 0, SOC_JER_FABRIC_GROUP_CTX_LENGTH);
                soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MAP_MSBf, *dqcq_map);
                SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64));
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * get the FAP ID of the (core 0 of the) device
 */
uint32
  jer_mgmt_system_fap_id_get(
    SOC_SAND_IN  int       unit,
    SOC_SAND_OUT uint32    *sys_fap_id
  )
{
  uint32  fld_val;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(sys_fap_id);

  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, ECI_GLOBAL_GENERAL_CFG_2r, REG_PORT_ANY, 0, DEV_ID_0f, &fld_val)); 
  *sys_fap_id = fld_val;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_init_ctrl_cells_enable_set(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable,
    SOC_SAND_IN  uint32  flags

  )
{
    SOCDNX_INIT_FUNC_DEFS;

/*exit:*/
    SOCDNX_FUNC_RETURN;
}

int jer_mgmt_nof_block_instances(int unit, soc_block_types_t block_types, int *nof_block_instances) 
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(nof_block_instances);
    SOCDNX_NULL_CHECK(block_types);

    switch(block_types[0]) {
        case SOC_BLK_EDB:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_EDB; 
            break;
        case SOC_BLK_ILKN_PML:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_ILKN_PML; 
            break;
        case SOC_BLK_ILKN_PMH:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_ILKN_PMH; 
            break;
        case SOC_BLK_PPDB_A:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_PPDB_A; 
            break;
        case SOC_BLK_PPDB_B:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_PPDB_B; 
            break;
        case SOC_BLK_MRPS:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_MRPS; 
            break;
        case SOC_BLK_IPST:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_IPST; 
            break;
        case SOC_BLK_IQMT:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_IQMT; 
            break;
        case SOC_BLK_CFC:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_CFC; 
            break;
        case SOC_BLK_CGM:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_CGM;
            break;
        case SOC_BLK_CLP:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_CLP;
            break;
        case SOC_BLK_CRPS:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_CRPS;
            break;
        case SOC_BLK_DRCA:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_DRCA;
            break;
        case SOC_BLK_DRCB:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_DRCB;
            break;
        case SOC_BLK_DRCBROADCAST:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_DRCBROADCAST;
            break;
        case SOC_BLK_DRCC:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_DRCC;
            break;
        case SOC_BLK_DRCD:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_DRCD;
            break;
        case SOC_BLK_DRCE:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_DRCE;
            break;
        case SOC_BLK_DRCF:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_DRCF;
            break;
        case SOC_BLK_DRCG:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_DRCG;
            break;
        case SOC_BLK_DRCH:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_DRCH;
            break;
        case SOC_BLK_ECI:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_ECI;
            break;
        case SOC_BLK_EGQ:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_EGQ;
            break;
        case SOC_BLK_EPNI:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_EPNI;
            break;
        case SOC_BLK_FCR:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_FCR;
            break;
        case SOC_BLK_FCT:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_FCT;
            break;
        case SOC_BLK_FDR:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_FDR;
            break;
        case SOC_BLK_FDA:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_FDA;
            break;
        case SOC_BLK_FDT:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_FDT;
            break;
        case SOC_BLK_FMAC:  
            *nof_block_instances = SOC_DPP_DEFS_GET(unit, nof_instances_fmac);
            break;
        case SOC_BLK_FSRD:    
            *nof_block_instances = SOC_DPP_DEFS_GET(unit, nof_instances_fsrd);
            break;
        case SOC_BLK_IDR:           
            *nof_block_instances = SOC_JER_NOF_INSTANCES_IDR;
            break;            
        case SOC_BLK_IHB:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_IHB; 
            break;
        case SOC_BLK_IHP:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_IHP; 
            break;
        case SOC_BLK_IPS:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_IPS; 
            break;
        case SOC_BLK_IPT:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_IPT; 
            break;
        case SOC_BLK_IQM:     
            *nof_block_instances = SOC_JER_NOF_INSTANCES_IQM; 
            break;
        case SOC_BLK_IRE:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_IRE; 
            break;
        case SOC_BLK_IRR:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_IRR; 
            break;
        case SOC_BLK_MESH_TOPOLOGY:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_MESH_TOPOLOGY;                   
            break;
        case SOC_BLK_MMU:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_MMU;   
            break;
        case SOC_BLK_NBIL:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_NBIL;   
            break;
        case SOC_BLK_NBIH:     
            *nof_block_instances = SOC_JER_NOF_INSTANCES_NBIH;   
            break;
        case SOC_BLK_OAMP:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_OAMP;  
            break;
        case SOC_BLK_OCB:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_OCB;   
            break;
        case SOC_BLK_OLP:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_OLP;   
            break;
        case SOC_BLK_OTPC:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_OTPC;  
            break;
        case SOC_BLK_RTP:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_RTP;  
            break;
        case SOC_BLK_SCH:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_SCH;   
            break;
        case SOC_BLK_XLP:    
            *nof_block_instances = SOC_JER_NOF_INSTANCES_XLP;
            break;
        default:
            *nof_block_instances = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

