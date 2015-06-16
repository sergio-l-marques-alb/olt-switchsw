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
 * File: jer_mgmt.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/dpp/JER/jer_fabric.h>

/*
 * PVT monitor
 */

#define _SOC_JER_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT   (32)
#define _SOC_JER_ECI_PVT_MON_CONTROL_REG_RESET_BIT       (33)

#define _SOC_JER_ECI_PVT_MON_CONTROL_REG_RESET_BIT       (33)
#define _SOC_JER_PVT_MON_NOF                             (4)
#define _SOC_JER_PVT_FACTOR                              (49103)
#define _SOC_JER_PVT_BASE                                (41205000)

#define JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP 16
#define JER_MGMT_NOF_CORES 2
#define JER_MGMT_NOF_PROCESSOR_IDS 18
#define JER_CR_VAL_BMP_NOF_BITS 16



/*********************************************************************
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  jer_mgmt_credit_worth_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              credit_worth
  )
{
  uint32
    fld_val = 0;
  soc_reg_t credit_value_reg;
  soc_field_t credit_value_lcl_field;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_SAND_IF_ERR_EXIT(arad_mgmt_credit_worth_verify(unit, credit_worth)) ;

  fld_val = credit_worth;
  /*
   * Jericho: Note that the register is different than in Arad and the fields
   * are marked '0' and '1' while in Arad, the same fields are marked '1' and '2'.
   */
  credit_value_reg = IPST_CREDIT_CONFIG_1r ;
  credit_value_lcl_field = CREDIT_VALUE_0f ;
  SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, credit_value_reg, REG_PORT_ANY, credit_value_lcl_field, fld_val)) ;

  /* SCH { */
  
  SOCDNX_SAND_IF_ERR_EXIT(arad_calc_assigned_rebounded_credit_conf(unit, credit_worth, &fld_val));

  {
    /*
     * This clause will probably change when two calendars feature is
     * added (SCH_ASSIGNED_CREDIT_CONFIGURATION_1, SCH_REBOUNDED_CREDIT_CONFIGURATION_1, ...)
     */
    SOCDNX_IF_ERR_EXIT(
      soc_reg_above_64_field32_modify(unit, SCH_ASSIGNED_CREDIT_CONFIGURATIONr, SOC_CORE_ALL, 0, ASSIGNED_CREDIT_WORTHf,  fld_val));
    /* Set Rebounded Credit worth equals to assigned credit worth */
    SOCDNX_IF_ERR_EXIT(
      soc_reg_above_64_field32_modify(unit, SCH_REBOUNDED_CREDIT_CONFIGURATIONr, SOC_CORE_ALL, 0, REBOUNDED_CREDIT_WORTHf,  fld_val));
  }
  /* SCH } */
exit:
  SOCDNX_FUNC_RETURN;
}
/*********************************************************************
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
*     Details: in the H file. (search for prototype)
*********************************************************************/
int
   jer_mgmt_credit_worth_get(
            SOC_SAND_IN  int                 unit,
            SOC_SAND_OUT uint32              *credit_worth
           )
{
  uint32
      reg_val,
      fld_val = 0;
  soc_reg_t credit_value_reg;
  soc_field_t credit_value_lcl_field;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(credit_worth);

  /*
   * Jericho: Note that the register is different than in Arad and the fields
   * are marked '0' and '1' while in Arad, the same fields are marked '1' and '2'.
   */
  credit_value_reg = IPST_CREDIT_CONFIG_1r ;
  credit_value_lcl_field = CREDIT_VALUE_0f ;
  /*
   * Read selected register and, then, extract the filed.
   */
  SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, credit_value_reg, REG_PORT_ANY, 0, &reg_val)) ;
  fld_val = soc_reg_field_get(unit, credit_value_reg, reg_val, credit_value_lcl_field) ;
  *credit_worth = fld_val;

exit:
  SOCDNX_FUNC_RETURN;
}




/*
 * Jericho only: set local and remote (0 and 1) credit worth values
 */
uint32
  jer_mgmt_credit_worth_remote_set(
    SOC_SAND_IN  int    unit,
	SOC_SAND_IN  uint32    credit_worth_remote
  )
{
    uint32 res, reg_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	if (credit_worth_remote < SOC_TMC_CREDIT_SIZE_BYTES_MIN || credit_worth_remote > SOC_TMC_CREDIT_SIZE_BYTES_MAX) {
		LOG_ERROR(BSL_LS_SOC_MANAGEMENT, 
				  (BSL_META_U(unit,
							  "Remote size %d is not between %u..%u") , credit_worth_remote, SOC_TMC_CREDIT_SIZE_BYTES_MIN, SOC_TMC_CREDIT_SIZE_BYTES_MAX));
		SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
	} else {
		uint32 arg_local, arg_remote;
		jer_mgmt_credit_worth_remote_get(unit, &arg_remote); /*need to be in sand_error???*/
		jer_mgmt_credit_worth_get(unit, &arg_local); /*need to be in sand_error???*/
		if (credit_worth_remote != arg_remote) { /* are we changing the value? */
			if (SOC_DPP_CONFIG(unit)->jer->tm.nof_remote_faps_with_remote_credit_value) { /* is the current value being used (by remote FAPs)? */
				if (credit_worth_remote != arg_local) {
					LOG_ERROR(BSL_LS_SOC_MANAGEMENT,
							  (BSL_META_U(unit,"The Remote credit value is assigned to remote devices. To change the value you must first assign the local credit value to these devices.")));
					SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 20, exit);
				} else { /* the local and (previous) remote values are equal, so we can just mark all FAPs as using the local value */
					SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, jer_mgmt_change_all_faps_credit_worth_unsafe(unit, SOC_TMC_FAP_CREDIT_VALUE_LOCAL));				
				}
			} else {
				SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IPST_CREDIT_CONFIG_1r(unit, &reg_val));
				soc_reg_field_set(unit, IPST_CREDIT_CONFIG_1r, &reg_val, CREDIT_VALUE_1f, credit_worth_remote);
				SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, WRITE_IPST_CREDIT_CONFIG_1r(unit, reg_val));
			  }
		}
	}

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_credit_worth_remote_set()", unit, 0);
}

/*
 * Jericho only: set local and remote (0 and 1) credit worth values
 */
uint32
  jer_mgmt_credit_worth_remote_get(
    SOC_SAND_IN  int    unit,
	SOC_SAND_OUT uint32    *credit_worth_remote
  )
{
    uint32 res, reg_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	SOC_SAND_CHECK_NULL_INPUT(credit_worth_remote);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IPST_CREDIT_CONFIG_1r(unit, &reg_val));
	*credit_worth_remote = soc_reg_field_get(unit, IPST_CREDIT_CONFIG_1r, reg_val, CREDIT_VALUE_1f);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_credit_worth_remote_get()", unit, 0);
}


/*
 * Jericho only: map the module (fap_id) to the given credit value (local, remote or non mapped).
 * Has no special handling of the local device (should not be used for the local device).
 */
uint32
  jer_mgmt_module_to_credit_worth_map_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_IN  uint32    credit_value_type /* should be one of JERICHO_FAP_CREDIT_VALUE_* */
  )
{
	uint32 data = 0, fap_bitmap[1], res, remote_bit;
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);
	if (fap_id > SOC_TMC_NOF_FAPS_IN_SYSTEM) {
		SOC_SAND_SET_ERROR_CODE(ARAD_FAP_FABRIC_ID_OUT_OF_RANGE_ERR, 10, exit);
	}

	SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IPST_CREDIT_ARBITER_CONFIGr(unit, &data)); 
	soc_reg_field_set(unit, IPST_CREDIT_ARBITER_CONFIGr, &data, CR_VAL_SEL_ENABLEf, TRUE);
	SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IPST_CREDIT_ARBITER_CONFIGr(unit, data)); 

	data = 0;
	SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IPST_CRVSm(unit, MEM_BLOCK_ANY, fap_id / JER_CR_VAL_BMP_NOF_BITS, &data));
	*fap_bitmap = soc_mem_field32_get(unit, IPST_CRVSm, &data, CR_VAL_BMPf);

	remote_bit = SHR_BITGET(fap_bitmap,(fap_id % JER_CR_VAL_BMP_NOF_BITS));
	if (credit_value_type == SOC_TMC_FAP_CREDIT_VALUE_LOCAL) {

		if (remote_bit == SOC_TMC_FAP_CREDIT_VALUE_LOCAL) {
		}
		else {
			SHR_BITCLR(fap_bitmap,(fap_id % JER_CR_VAL_BMP_NOF_BITS));
			--SOC_DPP_CONFIG(unit)->jer->tm.nof_remote_faps_with_remote_credit_value; /*Need to add support in WarmBot*/
		}
	} else if (credit_value_type == SOC_TMC_FAP_CREDIT_VALUE_REMOTE) {
		if (remote_bit == SOC_TMC_FAP_CREDIT_VALUE_REMOTE) {
		}
		else {
			SHR_BITSET(fap_bitmap,(fap_id % JER_CR_VAL_BMP_NOF_BITS));
			++SOC_DPP_CONFIG(unit)->jer->tm.nof_remote_faps_with_remote_credit_value; /*Need to add support in WarmBot*/
		}
	} else {
		SOC_SAND_SET_ERROR_CODE(ARAD_CREDIT_TYPE_INVALID_ERR, 40, exit);
	}
	soc_mem_field32_set(unit, IPST_CRVSm, &data, CR_VAL_BMPf, *fap_bitmap); 
	SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IPST_CRVSm(unit, MEM_BLOCK_ANY, fap_id /JER_CR_VAL_BMP_NOF_BITS, &data)); 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_module_to_credit_worth_map_set()", unit, 0);
}

/*
 * Jericho only: Get the mapping the module (fap_id) to the given credit value (local, remote or non mapped).
 * Has no special handling of the local device (should not be used for the local device).
 */
uint32
  jer_mgmt_module_to_credit_worth_map_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_OUT uint32    *credit_value_type /* will be one of JERICHO_FAP_CREDIT_VALUE_* */
  )
{
	uint32 data = 0, fap_bitmap[1], res;
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	if (fap_id > SOC_TMC_NOF_FAPS_IN_SYSTEM) {
		SOC_SAND_SET_ERROR_CODE(ARAD_FAP_FABRIC_ID_OUT_OF_RANGE_ERR, 30, exit);
	}
	SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IPST_CRVSm(unit, MEM_BLOCK_ANY, fap_id / JER_CR_VAL_BMP_NOF_BITS, &data));
	*fap_bitmap = soc_mem_field32_get(unit, IPST_CRVSm, &data, CR_VAL_BMPf);
	*credit_value_type = SHR_BITGET(fap_bitmap, (fap_id % JER_CR_VAL_BMP_NOF_BITS));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_module_to_credit_worth_map_get()", unit, 0);

}


/*
 * Jericho only: in case the local and remote credit values are equal, change all configure remote FAPS to use the local or remote value.
 * The credit_value_to_use selects to which value we should make the FAPS use:
 *   SOC_TMC_FAP_CREDIT_VALUE_LOCAL  - use the local credit value
 *   SOC_TMC_FAP_CREDIT_VALUE_REMOTE - use the local credit value
 */
uint32
  jer_mgmt_change_all_faps_credit_worth_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT uint8     credit_value_to_use
  )
{
    uint32 res, reg_val;
    uint32 credit_worth_local, credit_worth_remote;
	uint32 data = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IPST_CREDIT_CONFIG_1r(unit, &reg_val));
    credit_worth_local = soc_reg_field_get(unit, IPST_CREDIT_CONFIG_1r, reg_val, CREDIT_VALUE_0f);
    credit_worth_remote = soc_reg_field_get(unit, IPST_CREDIT_CONFIG_1r, reg_val, CREDIT_VALUE_1f);
    if (credit_worth_local != credit_worth_remote) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit);
    } else if (credit_value_to_use == SOC_TMC_FAP_CREDIT_VALUE_LOCAL) {
		soc_mem_field32_set(unit, IPST_CRVSm, &data, CR_VAL_BMPf, 0); 
		res = arad_fill_table_with_entry(unit, IPST_CRVSm, MEM_BLOCK_ANY, &data); 
		SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 115, exit);
    } else if (credit_value_to_use == SOC_TMC_FAP_CREDIT_VALUE_REMOTE) {
		soc_mem_field32_set(unit, IPST_CRVSm, &data, CR_VAL_BMPf, 0xffff); 
		res = arad_fill_table_with_entry(unit, IPST_CRVSm, MEM_BLOCK_ANY, &data); 
		SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);
    } else {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 125, exit);
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_change_all_faps_credit_worth_unsafe()", unit, credit_value_to_use);
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

    if (SOC_DPP_CONFIG(unit)->tdm.is_bypass &&
        SOC_DPP_CONFIG(unit)->arad->init.fabric.is_128_in_system &&
        SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_TDM_CONFIGURATIONr,
          REG_PORT_ANY, 0, TDM_SOURCE_FAP_IDf,
          sys_fap_id + SOC_DPP_CONFIG(unit)->arad->tdm_source_fap_id_offset));
    }
    /* set DQCQ map according to system configurations*/
    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH ||
               SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP);
    if (is_mesh) { /*config dqcq map with group contexts for local destinations */
        for (group_ctx = 0, i = sys_fap_id; i < sys_fap_id + SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; ++group_ctx, ++i) {
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
    } else { /*configure dqcq map with fap-id's of all cores*/
        COMPILER_64_ZERO(reg64);
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MAP_LSBf, sys_fap_id | (sys_fap_id << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MASKf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64));
        COMPILER_64_ZERO(reg64);
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MAP_MSBf, fap_id_plus_1 | (fap_id_plus_1 << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MASK_1f, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64));
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
        case SOC_BLK_MTRPS_EM:
            *nof_block_instances = SOC_JER_NOF_INSTANCES_MTRPS_EM; 
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
/*
 * Note: This procedure is based on soc_fe3200_drv_temperature_monitor_get(). 
 * Apparently, they both use the same silicon piece.
 */
int 
   jer_mgmt_temp_pvt_get(int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count)
{
    int ii;
    uint32 reg32_val;
    int peak, curr;
    soc_reg_t temp_reg[] = {ECI_PVT_MON_A_THERMAL_DATAr, ECI_PVT_MON_B_THERMAL_DATAr, ECI_PVT_MON_C_THERMAL_DATAr, ECI_PVT_MON_D_THERMAL_DATAr};
    soc_field_t curr_field[] = {THERMAL_DATA_Af, THERMAL_DATA_Bf, THERMAL_DATA_Cf, THERMAL_DATA_Df};
    soc_field_t peak_field[] = {PEAK_THERMAL_DATA_Af, PEAK_THERMAL_DATA_Bf, PEAK_THERMAL_DATA_Cf, PEAK_THERMAL_DATA_Df};

    SOCDNX_INIT_FUNC_DEFS;

    if (temperature_max < _SOC_JER_PVT_MON_NOF)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Array size should be equal to or larger than %d.\n"), _SOC_JER_PVT_MON_NOF));
    }

    for (ii = 0; ii < _SOC_JER_PVT_MON_NOF; ii++)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, temp_reg[ii], REG_PORT_ANY, 0, &reg32_val));

        curr = soc_reg_field_get(unit, temp_reg[ii], reg32_val, curr_field[ii]);
        /*curr [0.1 C] = 4120.5 - curr * 4.9103*/
        temperature_array[ii].curr =  (_SOC_JER_PVT_BASE - curr * _SOC_JER_PVT_FACTOR) / 10000;

        peak = soc_reg_field_get(unit, temp_reg[ii], reg32_val, peak_field[ii]);
        /*peak [0.1 C] = 4120.5 - peak * 4.9103*/
        temperature_array[ii].peak = (_SOC_JER_PVT_BASE - peak * _SOC_JER_PVT_FACTOR) / 10000;
    }

    *temperature_count = _SOC_JER_PVT_MON_NOF;

exit:
    SOCDNX_FUNC_RETURN; 
}

/* 
 * PVT
 */

/*
 * Note: This procedure is based on soc_fe3200_drv_pvt_monitor_enable(). 
 * Apparently, they both use the same silicon piece.
 */
int jer_mgmt_drv_pvt_monitor_enable(int unit)
{
    uint64 reg64_val;
    soc_reg_t pvt_monitors[] = {ECI_PVT_MON_A_CONTROL_REGr, ECI_PVT_MON_B_CONTROL_REGr, ECI_PVT_MON_C_CONTROL_REGr, ECI_PVT_MON_D_CONTROL_REGr};
    int pvt_index;

    SOCDNX_INIT_FUNC_DEFS;

    /* Init */
    COMPILER_64_ZERO(reg64_val);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++) {
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    /* Powerdown */
    COMPILER_64_BITSET(reg64_val, _SOC_JER_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++) {
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    /* Powerup */
    COMPILER_64_ZERO(reg64_val);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++) {
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    /* Reset */
    COMPILER_64_BITSET(reg64_val, _SOC_JER_ECI_PVT_MON_CONTROL_REG_RESET_BIT);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++) {
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      jer_mgmt_revision_fixes
 * Purpose:
 *      turns on all the bits controlling the revision fixes (chicken bits) in the device.
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int jer_mgmt_revision_fixes (int unit)
{
    uint64 reg64_val;
    uint32 reg32_val;
    soc_reg_above_64_val_t reg_above_64_val;
    int array_index, core_index ;

    SOCDNX_INIT_FUNC_DEFS;

    /* 
     * CFC_SCH_OOB_RX_CFG
     */
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, CFC_SCH_OOB_RX_CFGr, REG_PORT_ANY,  0, &reg64_val));
    /* CFC-SCH OOB CRC is calculated wrong */
    soc_reg64_field32_set(unit, CFC_SCH_OOB_RX_CFGr, &reg64_val, SCH_OOB_CRC_CFGf, 0x7);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, CFC_SCH_OOB_RX_CFGr, REG_PORT_ANY,  0,  reg64_val));

    /* 
     * CRPS_CRPS_GENERAL_CFG
     */
    for (array_index = 0; array_index < JER_MGMT_NOF_PROCESSOR_IDS; ++array_index) {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY, array_index, &reg32_val));
        /* QPTS - "active counters" counter isn't accurate when using "Qsize"    */ 
        soc_reg_field_set(unit, CRPS_CRPS_GENERAL_CFGr, &reg32_val, CRPS_N_ACT_CNT_VALIDATE_ENf, 0x1);
        /* QPTS - Ovth memory bypass bug    */
        soc_reg_field_set(unit, CRPS_CRPS_GENERAL_CFGr, &reg32_val, CRPS_N_OVTH_MEM_RFRSH_BPASS_ENf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY, array_index,  reg32_val));
    }    

    /* 
     * IPT_CNM_CONTORL
     */
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, IPT_CNM_CONTORLr, REG_PORT_ANY,  0, &reg32_val));
    /* If set, fabric CRC for CNM packets will not be calculated and will be stamped to constant value - IPT-CNM CRC calculation in IPT */
    soc_reg_field_set(unit, IPT_CNM_CONTORLr, &reg32_val, CNM_CALC_FABRIC_CRC_DISf, 0x0);
    /* If set and CNM sampled packet size is less than configured then DRAM CRC will be copied into the packet payload - IPT-Remove original packet CRC for CNM */
    soc_reg_field_set(unit, IPT_CNM_CONTORLr, &reg32_val, CNM_REMOVE_DRAM_CRC_DISf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, IPT_CNM_CONTORLr, REG_PORT_ANY,  0,  reg32_val));

    /* 
     * IPT_IPT_ENABLES
     */
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, IPT_IPT_ENABLESr, REG_PORT_ANY,  0, &reg32_val));
    /* If set, fabric CRC for CNM packets will not be calculated and will be stamped to constant value - IPT-CNM CRC calculation in IPT */
    soc_reg_field_set(unit, IPT_IPT_ENABLESr, &reg32_val, CNM_FHEI_5_BYTES_FIX_ENf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, IPT_IPT_ENABLESr, REG_PORT_ANY,  0,  reg32_val));

    /* 
     * PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATION
     */
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY,  0, &reg64_val));
    /* If set, the limit on the number of entries in the MACT is according to FID,  else the limit is according to lif. - Default MACT limit per FID */
    soc_reg64_field32_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, &reg64_val, LARGE_EM_CFG_LIMIT_MODE_FIDf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY,  0,  reg64_val)); 

    for (core_index = 0; core_index < JER_MGMT_NOF_CORES; ++core_index) {
        /* 
         * EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1
         */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, core_index,  0, &reg32_val));
        /* Bug Desc: IHB  bounce back filter for unicast packets (1 - bug fix disabled, the logic is simillar to B0 implementation) Prevent bounce back filtering for UC packets. Fixes Plus-EBF1 */
        soc_reg_field_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_BUG_FIX_98_DISABLEf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, core_index,  0,  reg32_val));

        /* 
         * EGQ_EGRESS_SHAPER_ENABLE_SETTINGS
         */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, core_index,  0, &reg32_val));
        /* If set, then the value of each credit corresponding to the Q-Pair shapers calnedar is 1/128 bytes else 1/1256 bytes. */
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg32_val, QPAIR_SPR_RESOLUTIONf, 0x0);
        /* If set, then the value of each credit corresponding to the TCG shapers calnedar is 1/128 bytes else 1/1256 bytes. */
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg32_val, TCG_SPR_RESOLUTIONf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, core_index,  0,  reg32_val));

        /*PetraB FTMH and ARAD PPH mode*/
        if ((TRUE == SOC_DPP_CONFIG(unit)->arad->init.pp_enable) && (TRUE == SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) &&
            soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "petrab_in_tm_mode", 0) )
        {
            /* 
             * EGQ_PP_CONFIG
             */
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EGQ_PP_CONFIGr, core_index,  0, &reg32_val));
            /* Bug Desc: IHB  bounce back filter for unicast packets (1 - bug fix disabled, the logic is simillar to B0 implementation) Prevent bounce back filtering for UC packets. Fixes Plus-EBF1 */
            soc_reg_field_set(unit, EGQ_PP_CONFIGr, &reg32_val, FIELD_13_13f, 0x1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EGQ_PP_CONFIGr, core_index,  0,  reg32_val));
        }

        /* 
         * EGQ_QPAIR_SPR_DIS
         */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, EGQ_QPAIR_SPR_DISr, core_index,  0, reg_above_64_val));
        /* If set then the corresponding Q-Pair shaper is disabled (gets auto credit and no need to insert it to the calendar). */
        soc_reg_field_set(unit, EGQ_QPAIR_SPR_DISr, reg_above_64_val, QPAIR_SPR_DISf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_QPAIR_SPR_DISr, core_index,  0,  reg_above_64_val));

        /* 
         * EGQ_TCG_SPR_DIS
         */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, EGQ_TCG_SPR_DISr, core_index,  0, reg_above_64_val));
        /* If set then the corresponding Q-Pair shaper is disabled (gets auto credit and no need to insert it to the calendar). */
        soc_reg_field_set(unit, EGQ_TCG_SPR_DISr, reg_above_64_val, TCG_SPR_DISf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_TCG_SPR_DISr, core_index,  0,  reg_above_64_val));

        /* 
         * EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1
         */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, core_index,  0, &reg32_val));
        /* disable changes relating to Plus-F5 and are not affected by CfgEnVxlanEncapsulation - mainly in PRP - Enable ERSPAN Inbound mirror fix */
        soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_PLUS_F_5_DISABLEf, 0x0);
        /* 1 - Trill format in EEDB is the same as B0 - Enable PON 3tag fix */
        soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_NEW_TRILL_FORMAT_DISABLEf, 0x0);
        /* 0- Use AC EEDB Entry as Data Entry is enabled, 1- Feature is disabled, Enable PON 3tag fix */
        soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg32_val, CFG_PLUS_F_24_DISABLEf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, core_index,  0,  reg32_val));

        /* 
         * EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2
         */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, core_index,  0, &reg32_val));
        if ((SOC_DPP_CONFIG(unit)->pp.next_hop_mac_extension_enable)) 
        {
            /* for compatibilty with ARAD, Disable hardware computation of Host-Index for DA. 
             * Instead, do it the Arad way: PMF will add pph learn extension (system header ), egress program editor will stamp the DA  
             * if this soc property is disabled, then use hardware computation using the chicken bit
             */
            soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &reg32_val, CFG_USE_HOST_INDEX_FOR_DA_CALC_ENABLEf, SOC_IS_NEXT_HOP_MAC_EXT_ARAD_COMPATIBLE(unit) ? 0x0 : 0x1);
        }

        soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &reg32_val, CFG_EEI_OUTLIF_SWAP_ENABLEf, 0x1);

        /* 0- MPLS pipe model fix is disabled, 1- MPLS pipe model fix is enabled, Enable MPLS Pipe fix */ 
        soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &reg32_val, CFG_MPLS_PIPE_FIX_ENABLEf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, core_index,  0,  reg32_val));

        /* 
         * EPNI_CFG_DC_OVERLAY
         */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_CFG_DC_OVERLAYr, core_index,  0, &reg32_val));
        /* VxLAN: native support, 0x1 - Enables construction of VxLAN-UDP headers (in encapsulation block), and using Model-4 EEDB (in prp block), when the (EEDBoutLif.EncapMode == Eth_IP) 0x0 - DC Overlays Disabled */
        soc_reg_field_set(unit, EPNI_CFG_DC_OVERLAYr, &reg32_val, CFG_EN_VXLAN_ENCAPSULATIONf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_CFG_DC_OVERLAYr, core_index,  0,  reg32_val));

        /* 
         * EPNI_CFG_ENABLE_FILTERING_PER_FWD_CODE
         */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_CFG_ENABLE_FILTERING_PER_FWD_CODEr, core_index,  0, &reg32_val));
        /* 0 - Disable ETPP filters, 1 - Enable ETPP filters, Disable ETPP filtering for the Snoop/Mirroring forwarding code (code 4'hB). Fixes Plus-EBF5 */
        soc_reg_field_set(unit, EPNI_CFG_ENABLE_FILTERING_PER_FWD_CODEr, &reg32_val, CFG_ENABLE_FILTERING_PER_FWD_CODEf, 0xf7ff);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_CFG_ENABLE_FILTERING_PER_FWD_CODEr, core_index,  0,  reg32_val));

        /* 
         * EPNI_PP_CONFIG
         */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, EPNI_PP_CONFIGr, core_index,  0, &reg32_val));
        /* EPNI Access to OutLIF 0 */
        soc_reg_field_set(unit, EPNI_PP_CONFIGr, &reg32_val, INVALID_OUTLIF_FIX_ENf, 0x1);
        /*PetraB FTMH and ARAD PPH mode*/
        if ((TRUE == SOC_DPP_CONFIG(unit)->arad->init.pp_enable) && (TRUE == SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) &&
            soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "petrab_in_tm_mode", 0) )
        {
            /* EPNI_REGFILE.PetrabWithAradPpMode = 1 */
            soc_reg_field_set(unit, EPNI_PP_CONFIGr, &reg32_val, PETRAB_WITH_ARAD_PP_MODEf, 0x1);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, EPNI_PP_CONFIGr, core_index,  0,  reg32_val));

        /* 
         * IHB_LBP_GENERAL_CONFIG_0
         */
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IHB_LBP_GENERAL_CONFIG_0r, core_index,  0, &reg64_val));
        /*PetraB FTMH and ARAD PPH mode*/
        if ((TRUE == SOC_DPP_CONFIG(unit)->arad->init.pp_enable) && (TRUE == SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) &&
            soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "petrab_in_tm_mode", 0) )
        {
            /* if set than the Arad will issue a FTMH heade in Petra format and PPH header in Arad format. The PPH will have pph-eep-ext and oam-ts-ext if needed */
            soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &reg64_val, PETRA_FTMH_WITH_ARAD_PPH_MODEf, 0x1);
        }
        /* If set, in_lif_profile is added to FHEI Header instead of the reserved bits - Enable passing inlif profile in FHEI. */
        soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &reg64_val, ENABLE_FHEI_WITH_IN_LIF_PROFILEf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, IHB_LBP_GENERAL_CONFIG_0r, core_index,  0,  reg64_val));

        /* 
         * IHP_VTT_GENERAL_CONFIGS_1
         */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, IHP_VTT_GENERAL_CONFIGS_1r, core_index,  0, &reg32_val));
        /* If set, and forwarding header is mpls while Terminated-TTL is not Valid, cos marking will be updated according to mpls header. */
        soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, &reg32_val, ENABLE_COS_MARKING_UPGRADESf, 0x1);
        /* If unset,  an inner compatible multicast identification will be executed - IGMP feature: enable compatible-mc after tunnel-termination and upgrade second stage parsing for IGMP */
        soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, &reg32_val, DISABLE_INNER_COMPATIBLE_MCf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, core_index,  0,  reg32_val));

        /* 
         * IQM_RATE_CLASS_RD_WEIGHT
         */
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, IQM_RATE_CLASS_RD_WEIGHTr, core_index,  0, &reg32_val));
        /* This table maps IPS-Q-Priority (1b), RateClass5:4 into PDM-Read-Weight (4b) Actual number of consecutive reads is (RateClassRdWeight+1). */
        soc_reg_field_set(unit, IQM_RATE_CLASS_RD_WEIGHTr, &reg32_val, RATE_CLASS_RD_WEIGHTf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, IQM_RATE_CLASS_RD_WEIGHTr, core_index,  0,  reg32_val));  
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting traffic.
*********************************************************************/
uint32
  jer_mgmt_enable_traffic_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint8 enable
  )
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOCDNX_PCID_LITE_SKIP(unit);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = jer_mgmt_enable_traffic_set_unsafe(unit, enable);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_enable_traffic_set()",unit,enable);
}

uint32
  jer_mgmt_enable_traffic_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  uint8 *enable
  )
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = jer_mgmt_enable_traffic_get_unsafe(unit, enable);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_mgmt_enable_traffic_get()",unit,enable);
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32 jer_mgmt_enable_traffic_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable)
{
    uint32 core_for_write, core_for_read, data;

    SOCDNX_INIT_FUNC_DEFS;

    /*
     * if in single core mode, use core '0'. Otherwise, read from
     * core '0' and write to all cores. This procedure is currently symmetric.
     */
    core_for_read = 0 ;
    core_for_write = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores == 1 ? 0 : SOC_CORE_ALL;

    if (enable == FALSE) {
                /*IRE disable*/
        SOCDNX_IF_ERR_EXIT(READ_IRE_DYNAMIC_CONFIGURATIONr(unit, &data));
        soc_reg_field_set(unit, IRE_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATHf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_DYNAMIC_CONFIGURATIONr(unit, data));

                /*IDR disable*/
        SOCDNX_IF_ERR_EXIT(READ_IDR_DYNAMIC_CONFIGURATIONr(unit, &data));
        soc_reg_field_set(unit, IDR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATHf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_DYNAMIC_CONFIGURATIONr(unit, data));

                /*IRR disable*/
        SOCDNX_IF_ERR_EXIT(READ_IRR_DYNAMIC_CONFIGURATIONr(unit, &data));
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IDR_0f, 0x0);
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IDR_1f, 0x0);
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IQM_0f, 0x0);
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IQM_1f, 0x0);
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_0f, 0x0);
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_1f, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_IRR_DYNAMIC_CONFIGURATIONr(unit, data));

                /*EGQ disable */
        SOCDNX_IF_ERR_EXIT(READ_EGQ_GENERAL_RQP_DEBUG_CONFIGr(unit, core_for_read, &data));
        soc_reg_field_set(unit, EGQ_GENERAL_RQP_DEBUG_CONFIGr, &data, DBG_FDA_STOPf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GENERAL_RQP_DEBUG_CONFIGr(unit, core_for_write, data));
                /*
                 *  Stop credit reception from the fabric
                 */
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core_for_read, &data));
        soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &data, DISABLE_FABRIC_MSGSf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core_for_write, data));
                /*
                 *  Stop credit generation
                 */
        SOCDNX_IF_ERR_EXIT(READ_SCH_DVS_CONFIGr(unit, core_for_read, &data));
        soc_reg_field_set(unit, SCH_DVS_CONFIGr, &data, FORCE_PAUSEf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_DVS_CONFIGr(unit, core_for_write, data));
    } else {

                        /*EGQ enable */
        SOCDNX_IF_ERR_EXIT(READ_EGQ_GENERAL_RQP_DEBUG_CONFIGr(unit, core_for_read, &data));
        soc_reg_field_set(unit, EGQ_GENERAL_RQP_DEBUG_CONFIGr, &data, DBG_FDA_STOPf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GENERAL_RQP_DEBUG_CONFIGr(unit,  core_for_write, data));

                    /*IRR enable*/
        SOCDNX_IF_ERR_EXIT(READ_IRR_DYNAMIC_CONFIGURATIONr(unit, &data));
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IDR_0f, 0x1);
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IDR_1f, 0x1);
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IQM_0f, 0x1);
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_IQM_1f, 0x1);
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_0f, 0x1);
        soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATH_1f, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_IRR_DYNAMIC_CONFIGURATIONr(unit, data));

                        /*IDR enable*/
        SOCDNX_IF_ERR_EXIT(READ_IDR_DYNAMIC_CONFIGURATIONr(unit, &data));
        soc_reg_field_set(unit, IDR_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATHf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_DYNAMIC_CONFIGURATIONr(unit, data));


                /*IRE enable*/
        SOCDNX_IF_ERR_EXIT(READ_IRE_DYNAMIC_CONFIGURATIONr(unit, &data));
        soc_reg_field_set(unit, IRE_DYNAMIC_CONFIGURATIONr, &data, ENABLE_DATA_PATHf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_DYNAMIC_CONFIGURATIONr(unit, data));
                /*
                 *  Start credit reception from the fabric
                 */
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core_for_read, &data));
        soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &data, DISABLE_FABRIC_MSGSf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core_for_write, data));
                /*
                 *  Start credit generation
                 */
        SOCDNX_IF_ERR_EXIT(READ_SCH_DVS_CONFIGr(unit, core_for_read, &data));
        soc_reg_field_set(unit, SCH_DVS_CONFIGr, &data, FORCE_PAUSEf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_DVS_CONFIGr(unit, core_for_write, data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 jer_mgmt_enable_traffic_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT  uint8  *enable)
{
    uint32 reg_val32;
    uint8  enable_curr;

    SOCDNX_INIT_FUNC_DEFS;

    /*checking if IRE is enabled, if so traffic is enabled, otherwise disabled*/
    SOCDNX_IF_ERR_EXIT(READ_IRE_DYNAMIC_CONFIGURATIONr(unit, &reg_val32));
    enable_curr = soc_reg_field_get(unit, IRE_DYNAMIC_CONFIGURATIONr, reg_val32, ENABLE_DATA_PATHf);
    enable_curr = SOC_SAND_NUM2BOOL(enable_curr);

    *enable = enable_curr;
exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

