#include <shared/bsl.h>
#include <soc/mcm/memregs.h> 
#if defined(BCM_88675_A0)
/* $Id: jer_pp_metering.c,v 1.0 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_METERING
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/JER/jer_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/drv.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC uint32
  jer_pp_metering_sharing_mode_get(
		SOC_SAND_IN int unit,
		SOC_SAND_OUT arad_pp_mtr_two_meter_sharing_mode_t *sharing_mode
  )
{
	uint32 range_mode, val;
	SOCDNX_INIT_FUNC_DEFS;

	range_mode =  SOC_DPP_CONFIG(unit)->meter.meter_range_mode;
	val = soc_property_get(unit, spn_POLICER_INGRESS_SHARING_MODE, ARAD_PP_MTR_TWO_METER_SHARING_MODE_NONE);
	
	if (SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit) && !ARAD_PP_MTR_IS_SINGLE_CORE(unit)){
		/*Single core device, Two MRPS cores*/
		if (64 == range_mode && ARAD_PP_MTR_TWO_METER_SHARING_MODE_PARALLEL != val) {
			SOCDNX_IF_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Policer Sharing Mode have to be PARALLEL with Ingress Count 64.")));
		}
		if (128 == range_mode && ARAD_PP_MTR_TWO_METER_SHARING_MODE_NONE != val) {
			SOCDNX_IF_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Policer Sharing Mode have to be NONE with Ingress Count 128.")));
		}
	}
	else { /* dual device cores (with two mrps cores) or single core device with single mrps core */
		if (32 == range_mode && ARAD_PP_MTR_TWO_METER_SHARING_MODE_NONE == val) {
			SOCDNX_IF_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Policer Sharing Mode can't be NONE with Ingress Count 32.")));
		}
		if (64 == range_mode && ARAD_PP_MTR_TWO_METER_SHARING_MODE_NONE != val) {
			SOCDNX_IF_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Policer Sharing Mode have to be NONE with Ingress Count 64.")));
		}
	}

	*sharing_mode = val;
			
exit:
	SOCDNX_FUNC_RETURN;
}

uint32
  jer_pp_metering_init_mrps_config(
		SOC_SAND_IN int unit,
		SOC_SAND_IN arad_pp_mtr_two_meter_sharing_mode_t sharing_mode
  )
{
uint32
	range_mode,
	reg_val,
	reg_val_a,
	reg_val_b;
int
	core_index;
 
	SOCDNX_INIT_FUNC_DEFS;

	range_mode = SOC_DPP_CONFIG(unit)->meter.meter_range_mode;

	/* Global configuration*/
	SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, MRPS_GLBL_CFGr, 0, 0, &reg_val));
	
	/* Init MCDs */
	soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCDA_INITf, 0);
	soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCDB_INITf, 0);

	/* Hierarchical Mode disabled by default */
	soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, HIERARCHICAL_MODEf, 0);

	/* Set PacketModeEn to allow for meters with packet/sec rates */
    soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, PACKET_MODE_ENf, 1);

	/* Enable the PCD map for Parallel and Serial modes */
	soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, PCD_MAP_ENf, (ARAD_PP_MTR_TWO_METER_SHARING_MODE_NONE == sharing_mode) ? 0 : 1);

	/*
		Device core mode | Meters | McqSizeSel | MrpsSecondPtrEn | McdSecondPtrEn | McqMcdsParallel |
	    -----------------|--------|------------|-----------------|----------------|-----------------|																					   |
		Single           | 2x64   |     0      |       1         |      1         |       1         |
		-----------------|--------|------------|-----------------|----------------|-----------------|
		Single           | 1x128  |     1      |       0         |      1         |       1         |
		-----------------|--------|------------|-----------------|----------------|-----------------|
		Dual             | 2x2x32 |     0      |       1         |      1         |   SharingMode   |
		-----------------|--------|------------|-----------------|----------------|-----------------|
		Dual             | 2x64   |     1      |       1         |      0         |       1         |
	*/
	if (SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit) && !ARAD_PP_MTR_IS_SINGLE_CORE(unit)) {
		/* single device core with two mrps cores*/
		soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCQ_SIZE_SELf, (128 == range_mode)? 1 : 0);
		soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MRPS_SECOND_PTR_ENf, (64 == range_mode)? 1 : 0);
		soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCD_SECOND_PTR_ENf, 1);
		soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCQ_MCDS_PARALLELf, 1);

	} else {
		/* dual device cores (with two mrps cores) or single core device with single mrps core */
		soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCQ_SIZE_SELf, (64 == range_mode)? 1 : 0);
		soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MRPS_SECOND_PTR_ENf, 1);
		soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCD_SECOND_PTR_ENf, (32 == range_mode)? 1 : 0);
		soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCQ_MCDS_PARALLELf, 
						  (64 == range_mode || ARAD_PP_MTR_TWO_METER_SHARING_MODE_PARALLEL == sharing_mode) ? 1 : 0);
	}

	/* MEF 10.3 disabled by default */
	soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MEF_10_DOT_3_ENf, 0);

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MRPS_GLBL_CFGr, core_index, 0, reg_val));
	}

	reg_val_a = 0;
	reg_val_b = 0;

	/* Enable leaky buckets Update */
	soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_TIMER_ENf, 1);
	soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_TIMER_ENf, 1);

	/* Enable leaky buckets Refresh */
	soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_REFRESH_ENf, 1);
	soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_REFRESH_ENf, 1);

	/* Enable LFR */
	soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_CBL_RND_MODE_ENf, 1);
	soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_CBL_RND_MODE_ENf, 1);

	/* Set default values to the rest */
	soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_ERR_COMP_ENABLEf, 1);
	soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_ERR_COMP_ENABLEf, 1);

	soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_RND_RANGEf, 3);
	soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_RND_RANGEf, 3);

	soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_BUBBLE_RATEf, 0xff);
	soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_BUBBLE_RATEf, 0xff);

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MRPS_MCDA_CFGr, core_index, 0, reg_val_a));
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MRPS_MCDB_CFGr, core_index, 0, reg_val_b));
	}

	reg_val_a = 0;
	reg_val_b = 0;

	/* Set default values for refresh quartet index range */
	soc_reg_field_set(unit, MRPS_MCDA_REFRESH_CFGr, &reg_val_a, MCDA_REFRESH_START_INDEXf, 0);
	soc_reg_field_set(unit, MRPS_MCDB_REFRESH_CFGr, &reg_val_b, MCDB_REFRESH_START_INDEXf, 0);

	soc_reg_field_set(unit, MRPS_MCDA_REFRESH_CFGr, &reg_val_a, MCDA_REFRESH_END_INDEXf, 0x1fff);
	soc_reg_field_set(unit, MRPS_MCDB_REFRESH_CFGr, &reg_val_b, MCDB_REFRESH_END_INDEXf, 0x1fff);

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MRPS_MCDA_REFRESH_CFGr, core_index, 0, reg_val_a));
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MRPS_MCDB_REFRESH_CFGr, core_index, 0, reg_val_b));
	}

	reg_val_a = 0;
	reg_val_b = 0;

	/* Enable wrap prevention */
	soc_reg_field_set(unit, MRPS_MCDA_WRAP_INDEXr, &reg_val_a, MCDA_WRAP_INT_ENf, 1);
	soc_reg_field_set(unit, MRPS_MCDB_WRAP_INDEXr, &reg_val_b, MCDB_WRAP_INT_ENf, 1);

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MRPS_MCDA_WRAP_INDEXr, core_index, 0, reg_val_a));
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MRPS_MCDB_WRAP_INDEXr, core_index, 0, reg_val_b));
	}
	
exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  jer_pp_metering_init(
		SOC_SAND_IN int unit
  )
{
	arad_pp_mtr_two_meter_sharing_mode_t sharing_mode;

	SOCDNX_INIT_FUNC_DEFS;

	/*Init MRPS-In-DP and DP maps*/
	SOCDNX_IF_ERR_EXIT(jer_itm_setup_dp_map(unit));

	/* Set the FTMH DP source to always be the output of the DP map. */
	/* When the meter is invalid, the meter DP will be the In-DP. */
	SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, IDR_BLOCK(unit), 0, FTMH_DP_OVERWRITEf, 1));

	SOCDNX_IF_ERR_EXIT(jer_pp_metering_sharing_mode_get(unit, &sharing_mode));

	SOCDNX_IF_ERR_EXIT(jer_pp_metering_init_mrps_config(unit, sharing_mode));

	/* Init PCD for Parallel and Serial modes */
	if (sharing_mode != ARAD_PP_MTR_TWO_METER_SHARING_MODE_NONE) {
		SOCDNX_SAND_IF_ERR_EXIT(arad_pp_metering_pcd_init(unit, sharing_mode));
	}

exit:
  SOCDNX_FUNC_RETURN;
}

/* } */

#endif /*BCM_88675_A0*/
