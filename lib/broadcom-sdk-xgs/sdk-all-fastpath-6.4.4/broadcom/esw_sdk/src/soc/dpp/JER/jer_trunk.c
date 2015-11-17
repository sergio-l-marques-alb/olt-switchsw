/*
 * $Id: jer_trunk.c Exp $
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
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TRUNK

/* 
 *  INCLUDES
 */

#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_drv.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/JER/jer_trunk.h>

/* 
 *  FUNCTIONS
 */ 

/* 
 * Configuration Functions
 */

/* 
 *  soc_jer_port_direct_lb_key_set - set min & max LB-Key 
 *  if set_min == FALSE  - dosn't set min value.
 *  if set_max == FALSE  - dosn't set max value.
 */
uint32 
  soc_jer_trunk_direct_lb_key_set( 
    SOC_SAND_IN int    unit, 
    SOC_SAND_IN int    core_id, 
    SOC_SAND_IN uint32 local_port,
    SOC_SAND_IN uint32 min_lb_key,
    SOC_SAND_IN uint32 set_min,
    SOC_SAND_IN uint32 max_lb_key,
    SOC_SAND_IN uint32 set_max
   )
{
    uint32 
        base_q_pair = 0,
        nof_pairs,
        curr_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        egq_pp_ppct_data;
    uint32 
        use_table_2 = 0x0, 
        field_val; 
    uint64 
        val64;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, local_port, &base_q_pair));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core_id, local_port, &nof_pairs));

    /* Check if lb-key table 2 (EGQ_PER_PORT_LB_KEY_RANGE) is used  */
    if(SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB) {  
        SOCDNX_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_1r(unit,SOC_CORE_ALL,&val64));
        field_val = soc_reg64_field32_get(unit, IHB_LBP_GENERAL_CONFIG_1r, val64, OVERRIDE_FTMH_LB_KEY_MSB_VALUEf);
        if (field_val == 0) {
            use_table_2 = 1;
        }
    }

    /* Run over all match q-pairs and set lb_key min/max */
    for (curr_q_pair = base_q_pair; curr_q_pair - base_q_pair < nof_pairs; curr_q_pair++)
    {
        SOCDNX_IF_ERR_EXIT(
           arad_egq_ppct_tbl_get_unsafe(
              unit,
              core_id,
              curr_q_pair,
              &egq_pp_ppct_data));

        if(use_table_2 == 1){
            /* update PPCT */
            if (0x1 == set_min) {
                egq_pp_ppct_data.second_range_lb_key_min = min_lb_key | 0x80; /*  added because msb is 1 */
            }
            if (0x1 == set_max) {
                egq_pp_ppct_data.second_range_lb_key_max = max_lb_key | 0x80; /*  added because msb is 1 */
            }
        } else {
            /* update PPCT */
            if (0x1 == set_min) {
                egq_pp_ppct_data.lb_key_min = min_lb_key;
            }
            if (0x1 == set_max) {
                egq_pp_ppct_data.lb_key_max = max_lb_key;
            }
        }

        SOCDNX_IF_ERR_EXIT(
           arad_egq_ppct_tbl_set_unsafe(
              unit,
              SOC_CORE_ALL,
              curr_q_pair,
              &egq_pp_ppct_data));
    }
exit:
  SOCDNX_FUNC_RETURN;
}

/* 
 * soc_jer_port_direct_lb_key_get - get min & max LB-Key 
 *  if set_min == NULL  - doesn't get min value.
 *  if set_max == NULL  - doesn't get max value.
 */
uint32 
    soc_jer_trunk_direct_lb_key_get(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN int core_id, 
      SOC_SAND_IN uint32  local_port,
      SOC_SAND_OUT uint32 *min_lb_key,
      SOC_SAND_OUT uint32 *max_lb_key
   )
{
    uint32 
        base_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        egq_pp_ppct_data;
    uint32 
        field_val, 
        use_table_2=0;
    uint64 
        val64;

    SOCDNX_INIT_FUNC_DEFS;  

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, local_port, &base_q_pair));

    /* Check if the 2nd lb-key fields are used  */
    if(SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB) {
        SOCDNX_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_1r(unit, 0, &val64));
        field_val = soc_reg64_field32_get(unit, IHB_LBP_GENERAL_CONFIG_1r, val64, OVERRIDE_FTMH_LB_KEY_MSB_VALUEf);
        if(field_val == 1) {
          use_table_2 = 1;
        }
    }

    SOCDNX_IF_ERR_EXIT(
       arad_egq_ppct_tbl_get_unsafe(
          unit,
          core_id,
          base_q_pair,
          &egq_pp_ppct_data));

     if(use_table_2 == 0){
         /* update return value */
         if (min_lb_key != NULL) {
           *min_lb_key = egq_pp_ppct_data.lb_key_min;
         }
         if (max_lb_key != NULL) {
           *max_lb_key = egq_pp_ppct_data.lb_key_max;
         }
     } else {
         if (min_lb_key != NULL) {
           *min_lb_key = egq_pp_ppct_data.second_range_lb_key_min & (~0x80); /* remove bit 7 witch allways set on */
         }
         if (max_lb_key != NULL) {
           *max_lb_key = egq_pp_ppct_data.second_range_lb_key_max & (~0x80); /* remove bit 7 witch allways set on */
         }
     }
exit:
  SOCDNX_FUNC_RETURN;
}

/* 
 * Init functions
 */

/* 
 * #LAG-groups * #LAG-members: 
 * 0x0 - 1K groups of 16
 * 0x1 - 512 groups of 32
 * 0x2 - 256 groups of 64
 * 0x3 - 128 groups of 128
 * 0x4 - 64 groups of 256
 */
uint32
  soc_jer_trunk_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_LAG_MODE  lag_mode
  )
{
  SOCDNX_INIT_FUNC_DEFS;

  /* Same encoding HW and SW */
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_GENERAL_CFG_3r, REG_PORT_ANY, 0, LAG_MODEf,  lag_mode));

  /* Enable LAG filtering on UM + MC */
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_LAG_FILTER_ENABLEr, SOC_CORE_ALL, 0, ENABLE_LAG_FILTER_MCf,  0x1));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_LAG_FILTER_ENABLEr, SOC_CORE_ALL, 0, ENABLE_LAG_FILTER_UCf,  0x0));

exit:
  SOCDNX_FUNC_RETURN;
}

int soc_jer_trunk_init_tables (int unit)
{
    uint32
        index,
        table_entry[128] = {0}, 
        egq_data[SOC_DPP_DEFS_MAX(EGQ_PPCT_NOF_LONGS)];
    soc_mem_t mem;
    SOCDNX_INIT_FUNC_DEFS;

    /* PER PORT CONFIGURATION TABLE */
    mem = EGQ_PPCTm;
    for (index = soc_mem_index_min(unit,mem) ; index < soc_mem_index_count(unit,mem) ; index++) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_PPCTm (unit, EGQ_BLOCK(unit, 0), index, egq_data));
        soc_mem_field32_set(unit, mem, egq_data, LB_KEY_MINf, 0x0);
        soc_mem_field32_set(unit, mem, egq_data, LB_KEY_MAXf, 0xff);
        soc_mem_field32_set(unit, mem, egq_data, SECOND_RANGE_LB_KEY_MINf, 0x0);
        soc_mem_field32_set(unit, mem, egq_data, SECOND_RANGE_LB_KEY_MAXf, 0xff);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PPCTm (unit, EGQ_BLOCK(unit, SOC_CORE_ALL), index, egq_data));
    }

    

    if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
        /* LAG MAPPING */
        SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, IRR_LAG_MAPPINGm, MEM_BLOCK_ANY, table_entry));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRR_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1));
    
    if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
        /* LAG NEXT MEMBER */
        SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, IRR_LAG_NEXT_MEMBERm, MEM_BLOCK_ANY, table_entry));

        /* LAG TO LAG RANGE */
        SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, IRR_LAG_TO_LAG_RANGEm, MEM_BLOCK_ANY, table_entry));
    }

    /* SMOOTH DIVISION */
    /* Write the smooth division table using DMA. */
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_variable_values_by_caching(unit, IRR_SMOOTH_DIVISIONm, 0, MEM_BLOCK_ANY, -1, -1, 
                                                    _arad_mgmt_irr_tbls_init_dma_callback, NULL));
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_trunk_resolve_use_msb(int unit)
{
    ARAD_MGMT_INIT *init = &(SOC_DPP_CONFIG(unit)->arad->init);
    uint32
        val;
    uint8
        smooth_division_use_msb = init->ports.smooth_division_resolve_using_msb,
        stack_use_msb = init->ports.stack_resolve_using_msb;

    SOCDNX_INIT_FUNC_DEFS;

    if (smooth_division_use_msb || stack_use_msb) { /* these fields' default values are 0 */
        SOCDNX_IF_ERR_EXIT(READ_IRR_STATIC_CONFIGURATIONr(unit, &val));
        soc_reg_field_set(unit, IRR_STATIC_CONFIGURATIONr, &val, LAG_LB_KEY_STACK_RESOLVE_USE_MSBf, stack_use_msb);
        soc_reg_field_set(unit, IRR_STATIC_CONFIGURATIONr, &val, LAG_LB_KEY_SMOOTH_DIVISION_USE_MSBf, smooth_division_use_msb);
        SOCDNX_IF_ERR_EXIT(WRITE_IRR_STATIC_CONFIGURATIONr(unit, val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_trunk_init (int unit)
{
    ARAD_MGMT_INIT *init = &(SOC_DPP_CONFIG(unit)->arad->init);

    SOCDNX_INIT_FUNC_DEFS;
    
    /* LAG mode configuration */
    SOCDNX_IF_ERR_EXIT(soc_jer_trunk_mode_set_unsafe(unit, init->ports.lag_mode));

    /* Using the lb-key's LSB/MSB in smooth-division and stacking resolutions */
    SOCDNX_IF_ERR_EXIT(soc_jer_trunk_resolve_use_msb(unit));

    /* Initialization of LAG tables */
    SOCDNX_IF_ERR_EXIT(soc_jer_trunk_init_tables(unit));

    
    /*SOCDNX_IF_ERR_EXIT(soc_jer_trunk_init_stack(unit));*/

exit:
  SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

