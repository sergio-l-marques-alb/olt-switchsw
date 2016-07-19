/* $Id: jer_pp_trap.c,v 1.111 Broadcom SDK $
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
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_TRAP

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/PPC/ppc_api_general.h>

#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_eg_mirror.h>

#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/JER/JER_PP/jer_pp_trap.h>



static soc_reg_t etpp_reg_names[ETPP_NOF_TRAPS] =
{
    /* SOC_PPC_TRAP_CODE_ETPP_OUT_VPORT_DISCARD */
    EPNI_CFG_EES_ACTION_TRAPr,
    /* SOC_PPC_TRAP_CODE_ETPP_STP_STATE_FAIL */
    EPNI_CFG_STP_STATE_TRAPr,
    /* SOC_PPC_TRAP_CODE_ETPP_PROTECTION_PATH_UNEXPECTED */
    EPNI_CFG_PROTECTION_PATH_TRAPr,
    /* SOC_PPC_TRAP_CODE_ETPP_VPORT_LOOKUP_FAIL */
    EPNI_CFG_GLEM_TRAPr,
    /*SOC_PPC_TRAP_CODE_ETPP_MTU_FILTER*/
    EPNI_CFG_MTU_TRAPr,
    /*SOC_PPC_TRAP_CODE_ETPP_ACC_FRAME_TYPE*/
    EPNI_CFG_ACC_FRAME_TYPE_TRAPr,
    /*SOC_PPC_TRAP_CODE_ETPP_SPLIT_HORIZON*/
    EPNI_CFG_SPLIT_HORIZON_TRAPr

};

static soc_field_t etpp_field_names[ETPP_NOF_TRAPS][ETPP_NUMBER_TRAP_FIELDS] =
{
    /* SOC_PPC_TRAP_CODE_ETPP_OUT_VPORT_DISCARD */
    {CFG_EES_ACTION_TRAP_MIRROR_CMDf,       CFG_EES_ACTION_TRAP_FWD_STRENGTHf,      CFG_EES_ACTION_TRAP_MIRROR_STRENGTHf,       CFG_EES_ACTION_TRAP_FWD_ENf,        CFG_EES_ACTION_TRAP_MIRROR_ENf},
    /* SOC_PPC_TRAP_CODE_ETPP_STP_STATE_FAIL */
    {CFG_STP_STATE_TRAP_MIRROR_CMDf,        CFG_STP_STATE_TRAP_FWD_STRENGTHf,       CFG_STP_STATE_TRAP_MIRROR_STRENGTHf,        CFG_STP_STATE_TRAP_FWD_ENf,         CFG_STP_STATE_TRAP_MIRROR_ENf},
    /* SOC_PPC_TRAP_CODE_ETPP_PROTECTION_PATH_UNEXPECTED */
    {CFG_PROTECTION_PATH_TRAP_MIRROR_CMDf,  CFG_PROTECTION_PATH_TRAP_FWD_STRENGTHf, CFG_PROTECTION_PATH_TRAP_MIRROR_STRENGTHf,  CFG_PROTECTION_PATH_TRAP_FWD_ENf,   CFG_PROTECTION_PATH_TRAP_MIRROR_ENf},
    /* SOC_PPC_TRAP_CODE_ETPP_VPORT_LOOKUP_FAIL */
    {CFG_GLEM_TRAP_MIRROR_CMDf,             CFG_GLEM_TRAP_FWD_STRENGTHf,            CFG_GLEM_TRAP_MIRROR_STRENGTHf,             CFG_GLEM_TRAP_FWD_ENf,              CFG_GLEM_TRAP_MIRROR_ENf},
    /* SOC_PPC_TRAP_CODE_ETPP_MTU_FILTER */
    {CFG_MTU_TRAP_MIRROR_CMDf,              CFG_MTU_TRAP_FWD_STRENGTHf,             CFG_MTU_TRAP_MIRROR_STRENGTHf,              CFG_MTU_TRAP_FWD_ENf,               CFG_MTU_TRAP_MIRROR_ENf},
    /*SOC_PPC_TRAP_CODE_ETPP_ACC_FRAME_TYPE*/
    {CFG_ACC_FRAME_TYPE_TRAP_MIRROR_CMDf,   CFG_ACC_FRAME_TYPE_TRAP_FWD_STRENGTHf,  CFG_ACC_FRAME_TYPE_TRAP_MIRROR_STRENGTHf,   CFG_ACC_FRAME_TYPE_TRAP_FWD_ENf,    CFG_ACC_FRAME_TYPE_TRAP_MIRROR_ENf},
    /*SOC_PPC_TRAP_CODE_ETPP_SPLIT_HORIZON*/
    {CFG_SPLIT_HORIZON_TRAP_MIRROR_CMDf,    CFG_SPLIT_HORIZON_TRAP_FWD_STRENGTHf,   CFG_SPLIT_HORIZON_TRAP_MIRROR_STRENGTHf,    CFG_SPLIT_HORIZON_TRAP_FWD_ENf,     CFG_SPLIT_HORIZON_TRAP_MIRROR_ENf}

};
/*
 * Update EPNI_PMF_MIRROR_PROFILE_TABLE for a new PMF mirror profile.
 *
 * 'mirror_profile' is used as index into this table.
 *
 * Other  parameters are on *void_config which is a pointer to dpp_outbound_mirror_config_t. 
 * It was set a 'void *' to overcome problems in adding the prototype to jer_pp_trap.h
 *
 * This procedure is called from bcm_petra_mirror_port_destination_add() when
 * flag BCM_MIRROR_PORT_EGRESS_ACL is set.
 *
 * Note that entry '0' (mirror_profile set to '0') is reserved as default and
 * may not be changed by routine API.
 */
soc_error_t
soc_jer_pp_eg_pmf_mirror_params_set(int unit, uint32 mirror_profile, void *void_config)
{

    uint32 tbl_data, mirror_index;
    dpp_outbound_mirror_config_t *config ;
     
    int
      table_index_min,
      table_index_max ;

    SOCDNX_INIT_FUNC_DEFS;

    config = (dpp_outbound_mirror_config_t *)void_config ;
    table_index_max = SOC_MEM_INFO(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm).index_max ;
    table_index_min = SOC_MEM_INFO(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm).index_min ;
    mirror_profile += table_index_min ;

    if (mirror_profile <= table_index_max  && mirror_profile >= table_index_min) 
    {
        mirror_index = mirror_profile - table_index_min ;

        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_COMMANDf, (uint32)(config->mirror_command));
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_STRENGTHf, (uint32)(config->forward_strength));
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_STRENGTHf, (uint32)(config->mirror_strength));
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_ENABLEf, (uint32)(config->forward_en));
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_ENABLEf, (uint32)(config->mirror_en));
 
        SOCDNX_IF_ERR_EXIT(soc_mem_write(
          unit,
          EPNI_PMF_MIRROR_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          mirror_index,
          &tbl_data
        ));
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(
            SOC_E_EXISTS,
            (_BSL_SOCDNX_MSG(
                "Mirror profile (%d) is out of range. Should be between %d and %d"),
                mirror_profile,table_index_min,table_index_max
            )
        ) ;
    }
 
exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_jer_eg_etpp_trap_set(int unit, SOC_PPC_TRAP_ETPP_TYPE trap, SOC_PPC_TRAP_ETPP_INFO *entry_info)
{

    uint32 tbl_data;
    int trap_index;

    SOCDNX_INIT_FUNC_DEFS;

    tbl_data = 0;

    SOCDNX_IF_ERR_EXIT(soc_jer_eg_etpp_verify_parmas(unit, entry_info));
   
    /*  EPNI_MIRROR_PROFILE_TABLE  */
    if (trap <= SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_15  && trap >= SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_0) 
    {
        trap_index = trap - SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_0;


        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_COMMANDf, entry_info->mirror_cmd);
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_STRENGTHf, entry_info->fwd_strength);
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_STRENGTHf, entry_info->mirror_strength);
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_ENABLEf, entry_info->fwd_enable);
        soc_mem_field32_set(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_ENABLEf, entry_info->mirror_enable);
 
 		SOCDNX_IF_ERR_EXIT(soc_mem_write(
          unit,
          EPNI_PMF_MIRROR_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          trap_index,
          &tbl_data
        ));
        
      
    }
    else
    {
        soc_jer_eg_etpp_trap_get_array_index(trap, &trap_index); 
       	if (trap_index == -1)
       	{
       		 SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Unknown trap id")));
       	}

          soc_reg_field_set(unit, etpp_reg_names[trap_index], &tbl_data, etpp_field_names[trap_index][ETPP_MIRROR_CMD_INDEX], entry_info->mirror_cmd);
          soc_reg_field_set(unit, etpp_reg_names[trap_index], &tbl_data, etpp_field_names[trap_index][ETPP_FWD_STRENGTH_INDEX], entry_info->fwd_strength);
          soc_reg_field_set(unit, etpp_reg_names[trap_index], &tbl_data, etpp_field_names[trap_index][ETPP_MIRROR_STRENGTH_INDEX], entry_info->mirror_strength);
          soc_reg_field_set(unit, etpp_reg_names[trap_index], &tbl_data, etpp_field_names[trap_index][ETPP_FWD_ENABLE_INDEX], entry_info->fwd_enable);
          soc_reg_field_set(unit, etpp_reg_names[trap_index], &tbl_data, etpp_field_names[trap_index][ETPP_MIRROR_ENABLE_INDEX], entry_info->mirror_enable);
          
          SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit,  etpp_reg_names[trap_index], REG_PORT_ANY, 0, tbl_data));
    }
 
exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_eg_etpp_verify_parmas(int unit, SOC_PPC_TRAP_ETPP_INFO *entry_info)
{

    SOCDNX_INIT_FUNC_DEFS;
            
    /* should not occur since the BCM layer specifis 0 or 1. No need to verify that the values are not lower than 0. They are unsigned.*/
    if( entry_info->mirror_enable > 1 || entry_info->fwd_enable > 1 )
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("valid bit must be 0 or 1")));
    }

    /* The castings to int are not really necessary. Just to avoid coverity defects */
    if( entry_info->mirror_strength > MIRROR_STRENGTH_MAX_VALUE || (int)entry_info->mirror_strength < MIRROR_STRENGTH_MIN_VALUE) 
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Mirror strength range is %d to %d"),MIRROR_STRENGTH_MIN_VALUE, MIRROR_STRENGTH_MAX_VALUE ));
    }

    if( entry_info->fwd_strength > FWD_STRENGTH_MAX_VALUE || (int)entry_info->fwd_strength < FWD_STRENGTH_MIN_VALUE) 
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Forward strength range is %d to %d"),FWD_STRENGTH_MIN_VALUE, FWD_STRENGTH_MIN_VALUE ));
    }

    if( entry_info->mirror_cmd > MIRROR_COMMAND_MAX_VALUE || (int)entry_info->mirror_cmd < MIRROR_COMMAND_MIN_VALUE) 
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Mirror command range is %d to %d"),MIRROR_COMMAND_MIN_VALUE, MIRROR_COMMAND_MAX_VALUE ));
    }


exit:
  SOCDNX_FUNC_RETURN;




}

soc_error_t
soc_jer_eg_etpp_trap_get(int unit, SOC_PPC_TRAP_ETPP_TYPE trap, SOC_PPC_TRAP_ETPP_INFO *entry_info)
{
   
    uint32 tbl_data;
    int trap_index;
    SOCDNX_INIT_FUNC_DEFS;

   tbl_data = 0;

    /*  EPNI_MIRROR_PROFILE_TABLE  */
    if (trap <= SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_15  && trap >= SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_0) 
    {
        trap_index = trap - SOC_PPC_TRAP_CODE_ETPP_FIELD_SNOOP_0;
        
         SOCDNX_IF_ERR_EXIT(soc_mem_read(
          unit,
          EPNI_PMF_MIRROR_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          trap_index,
          &tbl_data
        ));

                              
    entry_info->mirror_cmd = soc_mem_field32_get(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_COMMANDf);
    entry_info->fwd_strength = soc_mem_field32_get(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_STRENGTHf);
    entry_info->mirror_strength = soc_mem_field32_get(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_STRENGTHf);
    entry_info->fwd_enable = soc_mem_field32_get(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, FWD_ENABLEf);
    entry_info->mirror_enable = soc_mem_field32_get(unit,EPNI_PMF_MIRROR_PROFILE_TABLEm , &tbl_data, MIRROR_ENABLEf);

    }
    else
    {
	    soc_jer_eg_etpp_trap_get_array_index(trap, &trap_index);
	    
	   	if (trap_index == -1)
	   	{
	   		 SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Unknown trap id")));
	   	}

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit,  etpp_reg_names[trap_index], REG_PORT_ANY, 0, &tbl_data));

        entry_info->mirror_cmd = soc_reg_field_get(unit, etpp_reg_names[trap_index], tbl_data,etpp_field_names[trap_index][ETPP_MIRROR_CMD_INDEX]);
        entry_info->fwd_strength = soc_reg_field_get(unit, etpp_reg_names[trap_index], tbl_data, etpp_field_names[trap_index][ETPP_FWD_STRENGTH_INDEX]);
        entry_info->mirror_strength = soc_reg_field_get(unit, etpp_reg_names[trap_index], tbl_data, etpp_field_names[trap_index][ETPP_MIRROR_STRENGTH_INDEX]);
        entry_info->fwd_enable = soc_reg_field_get(unit, etpp_reg_names[trap_index], tbl_data, etpp_field_names[trap_index][ETPP_FWD_ENABLE_INDEX]);
        entry_info->mirror_enable = soc_reg_field_get(unit, etpp_reg_names[trap_index], tbl_data, etpp_field_names[trap_index][ETPP_MIRROR_ENABLE_INDEX]);
  	}



exit:
  SOCDNX_FUNC_RETURN;
} 

/* this function sets the trap_index with the correct offset in the etpp_reg_names table that matches the trap */
void
soc_jer_eg_etpp_trap_get_array_index( SOC_PPC_TRAP_CODE trap, int *trap_index)
{
	
	  switch (trap)
      {
      case SOC_PPC_TRAP_CODE_ETPP_OUT_VPORT_DISCARD:
      {
    			*trap_index = ETPP_OUT_VPORT_DISCARD_INDEX;
                break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_STP_STATE_FAIL:
      {
    			*trap_index = ETPP_STP_STATE_FAIL_INDEX;
                break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_PROTECTION_PATH_UNEXPECTED:
      {
    			*trap_index = ETPP_PROTECTION_PATH_UNEXPECTED_INDEX;
                break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_VPORT_LOOKUP_FAIL:
      {
    			*trap_index = ETPP_VPORT_LOOKUP_FAIL_INDEX;
                break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_MTU_FILTER:
      {
          *trap_index = ETPP_MTU_FILTER;
          break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_ACC_FRAME_TYPE:
      {
          *trap_index = ETPP_ACC_FRAME_TYPE;
          break;
      }
      case SOC_PPC_TRAP_CODE_ETPP_SPLIT_HORIZON:
      {
          *trap_index = ETPP_SPLIT_HORIZON;
          break;
      }
      default:
    			*trap_index = -1;
          
      }	
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

