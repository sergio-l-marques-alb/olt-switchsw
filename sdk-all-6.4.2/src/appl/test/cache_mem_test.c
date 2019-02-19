/*
 * $Id: cache_mem_test.c,v 1.0 2014/08/14 MiryH Exp $
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

/* we need this junk function only to avoid building error of pedantic compilation */
void ___junk_function_cache_mem_test(void){
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined (BCM_ARAD_SUPPORT)
#include <shared/bsl.h>
#include <sal/types.h>
#include <sal/compiler.h>
#include <sal/core/libc.h>
#include <soc/defs.h>
#include <soc/mcm/memregs.h>
#include <soc/mem.h>

#include <soc/shared/sat.h>
#include <appl/diag/system.h>

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <bcm_int/dpp/counters.h>
#endif
#include "cache_mem_test.h"

/* EXTERN Definitions */
extern int bcm_common_linkscan_enable_set(int,int);

/* MACRO Definitions */
/*#define SOC_MEM_ALIAS_TO_ORIGINAL(unit, mem)    (mem)*/

/* FUNCTION Definitions */
int cache_mem_test_cache_set(SOC_SAND_IN int unit, SOC_SAND_IN soc_mem_t mem, void* enable);
int cache_mem_test_read_and_compare(SOC_SAND_IN int unit, SOC_SAND_IN soc_mem_t mem, void* data);
int cache_mem_test_cache_attach(int unit, soc_reg_t mem, int block_instance);
int cache_mem_test_skip_mem(uint32 unit, soc_mem_t mem);
uint32 cache_mem_test_generate_value(uint32 unit, soc_mem_t mem, uint32 array_index, uint32 index, cache_mem_test_write_value_pattern_t pattern);
void cache_mem_test_create_mask(SOC_SAND_IN  uint32 unit,SOC_SAND_IN soc_mem_t mem, SOC_SAND_OUT soc_reg_above_64_val_t mask);
void do_cache_mem_test_print_usage(void);


#if 0
int arad_tbl_is_aliase(soc_mem_t mem)
{
	switch (mem)
	{
		case			IPS_CRBALm:
		case			IPS_MEM_1E0000m:
		case			IPS_QDESCm:
		case			IPS_QDESC_TABLEm:
		case			IPT_EGQDATAm:
		case			IPT_MEM_80000m:
		case			PPDB_A_OEMA_MANAGEMENT_REQUESTm:
		case			PPDB_A_OEMB_MANAGEMENT_REQUESTm:
			/*ADDRBASE   => 0x140000*/
		case			EPNI_IPV4_TUNNEL_FORMATm:
		case			EPNI_MPLS_PUSH_FORMATm:
		case			EPNI_LINK_LAYER_OR_ARP_FORMATm:
		case			EPNI_MPLS_POP_FORMATm:
		case			EPNI_TRILL_FORMATm:
		case			EPNI_DATA_FORMATm:
		case			EPNI_EEDB_BANKm:
		case			EDB_EEDB_BANKm:
		case			EPNI_OUT_RIF_FORMATm:
		case			EPNI_MPLS_SWAP_FORMATm:
		case			EPNI_AC_FORMATm:
		case			EPNI_AC_HALF_ENTRY_FORMATm:
		case			EPNI_AC_ENTRY_WITH_DATA_FORMATm:
		case			EPNI_AC_FORMAT_WITH_DATAm:
			/*END of ADDRBASE   => 0x140000*/
			/*ADDRBASE   => 0x1070000*/
		case			PPDB_B_LIF_TABLE_AC_2_OUT_LIFm:
		case			IHP_LIF_TABLE_AC_P2P_TO_ACm:
		case			PPDB_B_LIF_TABLE_AC_2_EEIm:
		case			IHP_LIF_TABLE_AC_P2P_TO_PWEm:
		case			PPDB_B_LIF_TABLE_ISID_P2Pm:
		case			IHP_LIF_TABLE_ISID_P2Pm:
		case			PPDB_B_LIF_TABLE_AC_MPm:
		case			IHP_LIF_TABLE_AC_MPm:
		case			PPDB_B_LIF_TABLE_IP_TTm:
		case			IHP_LIF_TABLE_IP_TTm:
		case			PPDB_B_LIF_TABLE_LABEL_PWE_MPm:
		case			IHP_LIF_TABLE_LABEL_PWE_MPm:
		case			IHP_LIF_TABLE_AC_P2P_TO_PBBm:
		case			PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm:
		case			IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm:
		case			PPDB_B_LIF_TABLEm:
		case			IHP_LIF_TABLEm:
		case			PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm:
		case			IHP_LIF_TABLE_LABEL_PWE_P2Pm:
		case			PPDB_B_LIF_TABLE_TRILLm:
		case			IHP_LIF_TABLE_TRILLm:
		case			PPDB_B_LIF_TABLE_ISID_MPm:
		case			IHP_LIF_TABLE_ISID_MPm:
			/*END of ADDRBASE   => 0x1070000*/
			/*ADDRBASE   => 0x460000,*/
		case			EGQ_PORT_CONFIGURATION_TABLE_PCTm:
		case			EGQ_PCTm:
			/*END of ADDRBASE   => 0x460000,*/

		case			EGQ_TTL_SCOPE_MEMORY_TTL_SCOPEm:
		case			EGQ_TTL_SCOPEm:

		case			EGQ_VSI_PROFILE_MEMORY_VSI_PROFILEm:
		case			EGQ_VSI_PROFILEm:

		case			EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm:
		case			EPNI_LLVP_TABLEm:

		case			EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm:
		case			EPNI_PP_PCTm:

		case			EPNI_PCP_DEI_MAPm:
		case			EPNI_PCP_DEI_TABLEm:

		case			EPNI_SPANNING_TREE_PROTOCOL_STATE_MEMORY_STPm:
		case			EPNI_STPm:

			/*	ADDRBASE   => 0x240000,*/
		case			MRPS_MCDA_PRFCFG_SHARING_DISm:
		case			IDR_MCDA_PRFCFG_FORMAT_1m:
		case			MRPS_MCDA_PRFCFG_SHARING_ENm:
		case			IDR_MCDA_PRFCFG_FORMAT_0m:
		case			IDR_MCDA_PRFCFG_0m:
			 /*	END of ADDRBASE   => 0x240000,*/
		case			IHP_DSCP_EXP_MAPm:
		case			IHB_DSCP_EXP_MAPm:
		case			IHP_DSCP_EXP_REMARKm:
		case			IHB_DSCP_EXP_REMARKm:
		case			IHP_ETHERNET_OAM_OPCODE_MAPm:
		case			IHB_ETHERNET_OAM_OPCODE_MAPm:

			/* ADDRBASE   => 0xA50000,*/
		case			IHB_FEC_ENTRY_FORMAT_Am:
		case			IHB_FEC_ENTRYm:
		case			IHB_FEC_ENTRY_FORMAT_NULLm:
		case			IHB_FEC_ENTRY_FORMAT_Cm:
		case			IHB_FEC_ENTRY_FORMAT_Bm:
		case			IHB_FEC_ENTRY_GENERALm:
			/* END of ADDRBASE   => 0xA50000,*/
		case			IHB_FEM_10_19B_MAP_TABLEm:
		case			IHB_FEM_10_16B_MAP_TABLEm:
		case			IHB_FEM_11_16B_MAP_TABLEm:
		case			IHB_FEM_11_19B_MAP_TABLEm:
		case			IHB_FEM_12_16B_MAP_TABLEm:
		case			IHB_FEM_12_19B_MAP_TABLEm:
		case			IHB_FEM_14_19B_MAP_TABLEm:
		case			IHB_FEM_14_24B_MAP_TABLEm:
		case			IHB_FEM_15_19B_MAP_TABLEm:
		case			IHB_FEM_15_24B_MAP_TABLEm:

		case			IHB_FEM_2_19B_MAP_TABLEm:
		case			IHB_FEM_2_16B_MAP_TABLEm:
		case			IHB_FEM_3_19B_MAP_TABLEm:
		case			IHB_FEM_3_16B_MAP_TABLEm:
		case			IHB_FEM_4_16B_MAP_TABLEm:
		case			IHB_FEM_6_19B_MAP_TABLEm:
		case			IHB_FEM_6_24B_MAP_TABLEm:
		case			IHB_FEM_7_24B_MAP_TABLEm:

		case			IHP_FLP_LOOKUPSm:
		case			IHB_FLP_LOOKUPSm:

		case			IHP_UNKNOWN_DA_MAPm:
		case			IHB_UNKNOWN_DA_MAPm:

		case			IHP_IEEE_1588_ACTIONm:
		case			IHB_IEEE_1588_ACTIONm:
		case			EDB_ESEM_STEP_TABLEm:
		case			EPNI_ESEM_STEP_TABLEm:
		case			EGQ_AUXILIARY_DATA_MEMORY_AUX_TABLEm:
		case			EGQ_AUX_TABLEm:

		case			EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm:
		case			EGQ_IVEC_TABLEm:

		case			EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm:
		case			EGQ_PPCTm:
		case			EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm:
		case 			EGQ_PP_PPCTm:

		case			IHB_IEEE_1588_IDENTIFICATION_CAMm:
		case			IHP_IEEE_1588_IDENTIFICATION_CAMm:

		case			IHP_MY_BFD_DIPm:
		case			IHB_MY_BFD_DIPm:

		case			IHP_OAM_CHANNEL_TYPEm:
		case			IHB_OAM_CHANNEL_TYPEm:

		case			IHP_OAM_MY_CFM_MACm:
		case			IHB_OAM_MY_CFM_MACm:

		case			IHB_OEMA_STEP_TABLEm:
		case			PPDB_A_OEMA_STEP_TABLEm:

		case			IHB_OEMB_STEP_TABLEm:
		case			PPDB_A_OEMB_STEP_TABLEm:

		case			IHB_TCAM_ACCESS_PROFILEm:
		case			PPDB_A_TCAM_ACCESS_PROFILEm:

		case			IHB_TCAM_ACTION_HIT_INDICATIONm:
		case			PPDB_A_TCAM_ACTION_HIT_INDICATIONm:

		case			IHB_TCAM_ACTIONm:
		case			PPDB_A_TCAM_ACTIONm:

		case			IHB_TCAM_PD_PROFILEm:
		case			PPDB_A_TCAM_PD_PROFILEm:

		case			IHP_MACT_AGING_CONFIGURATION_TABLEm:
		case			PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm:

		case			IHP_MACT_FID_COUNTER_DBm:
		case			PPDB_B_LARGE_EM_FID_COUNTER_DBm:

		case			IHP_MACT_FID_COUNTER_PROFILE_DBm:
		case			PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm:

		case			IHP_MACT_FLUSH_DBm:
		case			PPDB_B_LARGE_EM_FLUSH_DBm:

		case			IHP_MACT_FORMAT_1m:
		case			IHP_MACT_FORMAT_3_TYPE_1m:
		case			PPDB_B_LARGE_EM_FORMAT_0_TYPE_0m:
		case			IHP_MACT_FORMAT_0_TYPE_0m:
		case			IHP_MACT_FORMAT_0_TYPE_2m:
		case			IHP_MACT_FORMAT_2m:
		case			IHP_MACT_FORMAT_0_TYPE_1m:
		case			IHP_MACT_FORMAT_3_TYPE_0m:
		case			IHP_MEM_590000m:

		case			IHP_MACT_PORT_MINE_TABLE_LAG_PORTm:
		case			PPDB_B_LARGE_EM_PORT_MINE_TABLE_LAG_PORTm:

		case			IHP_MACT_PORT_MINE_TABLE_PHYSICAL_PORTm:
		case			PPDB_B_LARGE_EM_PORT_MINE_TABLE_PHYSICAL_PORTm:

		case			IHP_MACT_STEP_TABLEm:
		case			PPDB_B_LARGE_EM_STEP_TABLEm:

		case			IHP_VLAN_PORT_MEMBERSHIP_TABLEm:
		case			IHP_VSI_PORT_MEMBERSHIPm:

		case			IPS_FLOW_ID_LOOKUP_TABLEm:
		case			IPS_FLWIDm:

		case			IPS_QPM_1_SYS_REDm:
		case			IPS_QPM_1m:
		case			IPS_QPM_1_NO_SYS_REDm:

		case			IPS_QPM_2_NO_SYS_REDm:
		case			IPS_QPM_2m:
		case			IPS_QPM_2_SYS_REDm:

		case			IPT_PRIORITY_BITS_MAPPING_2_FDTm:
        case			IPT_PRIORITY_BITS_MAP_2_FDTm:

		case			IRR_MCDB_EGRESS_FORMAT_0m:
		case			IRR_MCDB_EGRESS_FORMAT_1m:
		case			IRR_MCDB_EGRESS_FORMAT_2m:
		case			IRR_MCDB_EGRESS_FORMAT_4m:
		case			IRR_MCDB_EGRESS_FORMAT_5m:
		case			IRR_MCDB_EGRESS_FORMAT_6m:
		case			IRR_MCDB_EGRESS_FORMAT_7m:
		case			IRR_MCDBm:
		case			IRR_MCDB_EGRESS_TDM_FORMATm:
		case			IRR_MCDB_EGRESS_SPECIAL_FORMATm:

		case			IRR_SNOOP_MIRROR_DEST_TABLEm:
		case			IRR_SNOOP_MIRROR_TABLE_0m:

		case			IRR_SNOOP_MIRROR_TM_TABLEm:
		case			IRR_SNOOP_MIRROR_TABLE_1m:
			/* ADDRBASE   => 0x10000*/
		case			OAMP_MEP_DB_RFC_6374_ON_MPLSTPm:
		case			OAMP_MEP_DB_DM_STATm:
		case			OAMP_MEP_DB_LM_STATm:
		case			OAMP_MEP_DBm:
		case			OAMP_MEP_DB_LM_DBm:
		case			OAMP_MEP_DB_Y_1731_ON_MPLSTPm:
		case			OAMP_MEP_DB_BFD_ON_PWEm:
		case			OAMP_MEP_DB_BFD_ON_MPLSm:
		case			OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm:
		case			OAMP_MEP_DB_Y_1731_ON_PWEm:
		case			OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm:
			/* END of ADDRBASE   => 0x10000*/

		case			PPDB_A_TCAM_BANKm:
		case			IHB_TCAM_BANKm:
		case			PPDB_A_TCAM_BANK_COMMANDm:
		case			IHB_TCAM_BANK_COMMANDm:
		case			PPDB_A_TCAM_BANK_REPLYm:
		case			IHB_TCAM_BANK_REPLYm:

		case			SCH_DSP_2_PORT_MAP_DSPPm:
		case			SCH_DSP_2_PORT_MAP__DSPPm:

		case			SCH_PORT_SCHEDULER_MAP_PSMm:
		case			SCH_PORT_SCHEDULER_MAP__PSMm:

		case			PPDB_A_TCAM_ENTRY_PARITYm:
		case			IHB_TCAM_ENTRY_PARITYm:
		case			IHP_FLP_PROGRAM_KEY_GEN_VARm:
		case			IHB_FLP_PROGRAM_KEY_GEN_VARm:
		case			IHP_FLP_PTC_PROGRAM_SELECTm:
		case			IHB_FLP_PTC_PROGRAM_SELECTm:
		case			PPDB_B_LARGE_EM_FID_PROFILE_DBm:
		case			IHP_MACT_FID_PROFILE_DBm:
		case			OAMP_REMOTE_MEP_EXACT_MATCH_STEP_TABLEm:
		case			OAMP_RMAPEM_STEP_TABLEm:
		case			IDR_IRDBm:
		case			IRR_IRDBm:
		case			IHP_OAMAm:
		case			IHB_OAMAm:
		case			IHP_OAMBm:
		case			IHB_OAMBm:
		case			IRE_CTXT_MAPm:

/*	88650 */
		case 			OAMP_MEP_DB_BFD_CV_ON_MPLSTPm:


    	return 1;
	}
	return 0;
}
#endif

int
cache_mem_test_cache_attach(int unit, soc_reg_t mem, int block_instance)
{
    int rc;

    if (soc_mem_is_valid(unit, mem) &&
        ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)) {

       SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
       rc = soc_mem_cache_set(unit, mem, block_instance, 1);
    } else {
            rc = SOC_E_UNAVAIL;
    }

	return rc;
}

int cache_mem_test_skip_mem(uint32 unit, soc_mem_t mem)
{
	/* Verify that memory is present on the device. */
	if (!SOC_MEM_IS_VALID(unit, mem))
	{
		LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: unit %d Skip INVALID memory %d (%s)\n"), unit, mem, SOC_MEM_NAME(unit, mem)));
        return TRUE;
	}

	/* Skip the Read-only/Write-Only/Signal tables */
	if (soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) || soc_mem_is_signal(unit, mem))
	{
		LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: unit %d Skip %s memory %d (%s)\n"),
				unit,
				(soc_mem_is_readonly(unit, mem) ? "READ-ONLY" : ( soc_mem_is_writeonly(unit, mem)?"WRITE_ONLY":"SIGNAL")),
				mem,
				SOC_MEM_NAME(unit, mem)));
		return TRUE;
	}

#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit))
    {
    	if ( arad_tbl_is_dynamic(mem) /* || arad_tbl_is_aliase(mem)*/)
    	{
			/* skip dynamic memory */
    		LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: unit %d Skip DYNAMIC memory %d (%s)\n"), unit, mem, SOC_MEM_NAME(unit, mem)));
    		return TRUE;
    	}
    }
#endif
	switch(mem)
	{
		/*Skip big entry_len tables */
		case 			IHB_MEM_230000m:	/*causes ASSERT - memory len too big (1024) */
			LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: unit %d Skip big entry memory %d (%s)\n"), unit, mem, SOC_MEM_NAME(unit, mem)));
			return TRUE;

    	/*Skip Signal tables */
		case            CRPS_MEM_0080000m:
		case            CRPS_MEM_0090000m:
		case            CRPS_MEM_00A0000m:
		case            CRPS_MEM_00B0000m:
		case            ECI_MEM_00010000m:

		case			IPS_MEM_180000m:
		case			IPS_MEM_200000m:
		case			IPS_MEM_220000m:
		case			IPS_MEM_240000m:
		case			IPS_MEM_1A0000m:
		case			IQM_MEM_7E00000m:
		case 			PORT_WC_UCMEM_DATAm:
			/*88650 - Arad Only */
#ifdef BCM_ARAD_SUPPORT
		case			IRR_MEM_300000m:
		case			IRR_MEM_340000m:
		case			IRR_MEM_3C0000m:
#endif
		/* skip memory */
		LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: unit %d Skip SIGNAL memory %d (%s)\n"), unit, mem, SOC_MEM_NAME(unit, mem)));
		return TRUE;
	}

	return FALSE;
}

int cache_mem_test_cache_set(SOC_SAND_IN int unit, SOC_SAND_IN soc_mem_t mem, void* enable)
{
	soc_error_t rv = SOC_E_NONE;
	int blk;

	if (cache_mem_test_skip_mem(unit, mem))
		return rv;

	/* Skip Scheduler block (it's un-cachable)*/
	blk = SOC_MEM_INFO(unit, mem).minblock;
	if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_SCH)
		return rv;

	
	if (SOC_IS_ARADPLUS(unit))
	{
		if((mem == PPDB_B_LIF_TABLE_IP_TTm) || (mem == IHP_LIF_TABLE_IP_TTm) || (mem == IHP_LIF_TABLE_TRILLm) || (mem == PPDB_B_LIF_TABLE_TRILLm) ||
				(mem == EPNI_ESEM_MANAGEMENT_REQUESTm) || (mem == IHB_OEMA_MANAGEMENT_REQUESTm) || (mem == IHB_OEMB_MANAGEMENT_REQUESTm))
			return rv;
	}
	
	if (SOC_IS_ARAD(unit))
	{
		if((mem == IHB_TCAM_BANK_COMMANDm) || (mem == IHB_TCAM_BANK_REPLYm) || (mem == PPDB_A_TCAM_BANK_COMMANDm) || (mem == PPDB_A_TCAM_BANK_REPLYm))
			return rv;
	}
	if (SOC_IS_JERICHO(unit))
	{
		if( (mem == IRE_PPLB_CFGm) || (mem == OAMP_FLOW_STAT_10_SEC_ENTRY_1m) || (mem == OAMP_FLOW_STAT_10_SEC_ENTRY_2m) || (mem == OAMP_FLOW_STAT_10_SEC_ENTRY_3m) ||
			(mem == OAMP_FLOW_STAT_1_SEC_ENTRY_1m) || (mem == OAMP_FLOW_STAT_1_SEC_ENTRY_2m) || (mem == OAMP_FLOW_STAT_1_SEC_ENTRY_3m) ||
			(mem == OAMP_FLOW_STAT_ACCUM_ENTRY_1m) || (mem == OAMP_FLOW_STAT_ACCUM_ENTRY_2m) || (mem == OAMP_FLOW_STAT_ACCUM_ENTRY_3m) ||
			(mem == OAMP_FLOW_STAT_ACCUM_ENTRY_34m) || (mem == OAMP_SAT_TX_EVC_PARAMS_ENTRY_1m) || (mem == OAMP_SAT_TX_EVC_PARAMS_ENTRY_2m) ||
			(mem == OAMP_SAT_TX_GEN_PARAMSm) || (mem == PPDB_A_FEC_ENTRY_FORMAT_Am) || (mem == PPDB_A_FEC_ENTRY_FORMAT_Bm) || (mem == PPDB_A_FEC_ENTRY_FORMAT_Cm) ||
			(mem == PPDB_A_FEC_ENTRY_FORMAT_NULLm) || (mem == PPDB_A_FEC_ENTRY_GENERALm) || (mem == PPDB_B_LARGE_EM_FORMAT_1m) || (mem == PPDB_B_LARGE_EM_FORMAT_2m) ||
			(mem == PPDB_B_LARGE_EM_FORMAT_3_TYPE_0m) || (mem == PPDB_B_LARGE_EM_FORMAT_3_TYPE_1m) || (mem == PPDB_B_LARGE_EM_FORMAT_SLB_COUNTERm) || (mem == PPDB_B_LARGE_EM_LEARN_FORMATm))
			return rv;
	}

	/* Turn on cache memory for all tables */
	rv = cache_mem_test_cache_attach(unit, mem, COPYNO_ALL);
    if (rv != SOC_E_NONE)
    	LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: unit %d cache enable failed for %d (%s) rv %d\n"), unit, mem, SOC_MEM_NAME(unit, mem), rv));

    return rv;
}

uint32
cache_mem_test_generate_value(uint32 unit, soc_mem_t mem, uint32 array_index, uint32 index, cache_mem_test_write_value_pattern_t pattern)
{
	switch (pattern)
	{
		case 	cache_mem_test_write_value_pattern_all_ones:
			return 0xFFFFFFFF;
		case 	cache_mem_test_write_value_pattern_all_zeroes:
			return 0;
		case 	cache_mem_test_write_value_pattern_incremental:
			return index;
		case 	cache_mem_test_write_value_pattern_smart:
			return (unit + mem + array_index + index);
	}
	return 0;
}

void cache_mem_test_create_mask(
		SOC_SAND_IN  uint32					unit,
		SOC_SAND_IN  soc_mem_t 				mem,
		SOC_SAND_OUT soc_reg_above_64_val_t mask)
{
	uint32 					i;
	uint32 					entry_dw = soc_mem_entry_words(unit, mem);
    soc_mem_info_t  		*meminfo;
    sal_memset(mask, 0, sizeof(soc_reg_above_64_val_t));

    meminfo 		= &SOC_MEM_INFO(unit, mem);

    for ( i = 0; i < meminfo->nFields; i++)
    {
        /* Skip the Read-only/Write-Only/Signal/Reserved fields */
        if ((meminfo->fields[i].flags & SOCF_RO) ||  (meminfo->fields[i].flags & SOCF_WO) || (meminfo->fields[i].flags & SOCF_SIG) || (meminfo->fields[i].flags & SOCF_RES) )
    		continue;

        SOC_REG_ABOVE_64_CREATE_MASK(mask, meminfo->fields[i].len, meminfo->fields[i].bp);
    }

    /* Print the Mask */
    LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: Mask for memory %d %s, nFields %d mask 0x"),
                		mem, SOC_MEM_NAME(unit, mem), meminfo->nFields));

    for ( i = entry_dw; i > 0; i--)
    {
        LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("%X"), mask[i]));
    }

}

uint32 cache_mem_test_fill_values_soc_mem_write_callback(uint32 unit, soc_mem_t mem, uint32		i_array, tr_do_cache_mem_test_t *test_params)
{
	uint32 		start_index, end_index, index;
	uint32 		entry_index = 0;
	soc_reg_above_64_val_t 	write_value;
	soc_reg_above_64_val_t 	mem_field_mask;
	uint32 					entry_dw = soc_mem_entry_words(unit, mem);
	soc_error_t				rv = SOC_E_NONE;
	soc_mem_t 				mem_orig = mem;
	SOC_MEM_ALIAS_TO_ORIG(unit, mem_orig);

	start_index 	= parse_memory_index(unit, mem, "min");
	end_index 		= parse_memory_index(unit, mem, "max");

	cache_mem_test_create_mask(unit, mem, mem_field_mask);

    LOG_INFO(BSL_LS_APPL_TESTS,
                (BSL_META("CACHE_MEM_TEST: WRITE SOC: mem %d %s, num_of_entries %d, entry_dw %d\n"),
                		 mem, SOC_MEM_NAME(unit, mem), end_index - start_index + 1/*table_size*/, entry_dw));

	for (index = start_index; index <= end_index; index++ )
	{
		for (entry_index = 0; entry_index < entry_dw; entry_index++)
		{
			write_value[entry_index]  = cache_mem_test_generate_value(unit, mem_orig, i_array, index, test_params->write_value_pattern) & mem_field_mask[entry_index];
		}
		rv = soc_mem_array_write(unit, mem, i_array, MEM_BLOCK_ALL, index, write_value);
	}

	if (rv != SOC_E_NONE)
		test_params->result |= rv;

	return rv;
}



int cache_mem_test_fill_values_dma_callback(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int copyno,
    SOC_SAND_IN int array_index,
    SOC_SAND_IN int index,
    SOC_SAND_OUT uint32 *value,
    SOC_SAND_IN int entry_sz,
    SOC_SAND_IN void *opaque)
{
	uint32 			i 				= 0;
	uint32 			entry_size 		= entry_sz;
    soc_reg_above_64_val_t mem_field_mask;
	tr_do_cache_mem_test_t *test_params= ((tr_do_cache_mem_test_t *)opaque);
	soc_mem_t 		mem 		 	= test_params->mem_id;
	soc_mem_t 		mem_orig 		= mem;
	SOC_MEM_ALIAS_TO_ORIG(unit, mem_orig);
	*value = 0;

	cache_mem_test_create_mask(unit, mem, mem_field_mask);
    if (entry_size > SOC_REG_ABOVE_64_MAX_SIZE_U32) entry_size = SOC_REG_ABOVE_64_MAX_SIZE_U32; /* if the field size is bigger than 20 (sizeof(soc_reg_above_64_val_t)) then fixate it to 20. */

	for (i = 0; i < entry_size; i++)
    {
    	value[i] = cache_mem_test_generate_value(unit, mem_orig, array_index, index, test_params->write_value_pattern) & mem_field_mask[i];
        LOG_DEBUG(BSL_LS_APPL_TESTS,
                    (BSL_META("CACHE_MEM_TEST: WRITE DMA: mem  %d %s i %d value 0x%X mask 0x%X \n"),
                    		mem, SOC_MEM_NAME(unit, mem), i, value[i], mem_field_mask[i]));

    }

    return SOC_E_NONE;
}

int
cache_mem_test_write_iter_callback(int unit, soc_mem_t mem, void* data)
{
	uint32		rv = SOC_E_NONE;
	uint32		start_array_index = 0, end_array_index = 0;
	uint32		i_array;
	tr_do_cache_mem_test_t *test_params = (tr_do_cache_mem_test_t *)data;

    if(cache_mem_test_skip_mem(unit, mem))
    {
        /* skip memory*/
    	LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: WRITE: skipping memory %d %s\n"), mem, SOC_MEM_NAME(unit, mem)));
    	goto done;
    }

    if (SOC_MEM_IS_ARRAY(unit,mem))
    {
        start_array_index = parse_memory_array_index(unit, mem, "min");
        end_array_index = parse_memory_array_index(unit, mem, "max");
    }

    switch (test_params->write_type)
    {
		case cache_mem_test_write_type_dma:
		{
			for (i_array = start_array_index; i_array <= end_array_index; i_array++)
			{
		    	LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: WRITE DMA: write memory %d %s \n"), mem, SOC_MEM_NAME(unit, mem)));
				test_params->mem_id = mem;
#ifdef BCM_ARAD_SUPPORT
				/* DMA write via caching currently Implemented only in Arad.*/
				rv = arad_fill_table_with_variable_values_by_caching(unit, mem, i_array, MEM_BLOCK_ALL, -1, -1,
						cache_mem_test_fill_values_dma_callback, test_params);
#else
				/* Fill the values in the non-dma way - as we do for schan writes */
				
				rv = cache_mem_test_fill_values_soc_mem_write_callback( unit, mem, i_array, data);
#endif
			}
			break;
		}
		case cache_mem_test_write_type_schan:
		{
			for (i_array = start_array_index; i_array <= end_array_index; i_array++)
			{
				rv = cache_mem_test_fill_values_soc_mem_write_callback( unit, mem, i_array, data);
			}
		}
    }
done:
	if (rv != SOC_E_NONE)
		test_params->result |= rv;

    return rv;
}


int
cache_mem_test_read_and_compare(SOC_SAND_IN int unit, SOC_SAND_IN soc_mem_t mem, void* data)
{
	soc_error_t	rv = SOC_E_NONE;
    uint32		start_array_index = 0, end_array_index = 0, i_array;
	uint32 		start_index, end_index, index;
	uint32 		entry_index = 0;
    soc_reg_above_64_val_t 	read_value;
    soc_reg_above_64_val_t 	read_cache_value;
    soc_reg_above_64_val_t 	expected_value;
    soc_reg_above_64_val_t 	mem_field_mask;
	tr_do_cache_mem_test_t *test_params = (tr_do_cache_mem_test_t *)data;
    uint32 					entry_dw = soc_mem_entry_words(unit, mem);
	soc_mem_t 				mem_orig = mem;
	SOC_MEM_ALIAS_TO_ORIG(unit, mem_orig);

    if(cache_mem_test_skip_mem(unit, mem))
    {
        /* skip memory*/
    	LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ: skipping memory %d %s\n"), mem, SOC_MEM_NAME(unit, mem)));
    	goto done;
    }

    start_index 	= parse_memory_index(unit, mem, "min");
    end_index 		= parse_memory_index(unit, mem, "max");
    cache_mem_test_create_mask(unit, mem, mem_field_mask);

	LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ read memory %d %s, num_of_entries %d  \n"),
			 mem, SOC_MEM_NAME(unit, mem), end_index - start_index + 1/*table_size*/));

    if (SOC_MEM_IS_ARRAY(unit,mem)) {
        start_array_index = parse_memory_array_index(unit, mem, "min");
        end_array_index = parse_memory_array_index(unit, mem, "max");
    }

    for (i_array = start_array_index; i_array <= end_array_index; i_array++)
    {
    	for (index = start_index; index <= end_index; index++ )
    	{
			rv = soc_mem_array_read(unit, mem, i_array, MEM_BLOCK_ANY, index, read_cache_value);
			rv |= soc_mem_array_read_flags(unit, mem, i_array, MEM_BLOCK_ANY, index, read_value, SOC_MEM_DONT_USE_CACHE);
    		if (rv != SOC_E_NONE)
			{
    			LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST:Read FAILED rv %d: read_value 0x%X cache_value 0x%X mask 0x%X mem %d %s, index %d, entry_index %d array %d\n"),
    									rv, read_value[entry_index], read_cache_value[entry_index], mem_field_mask[entry_index], mem, SOC_MEM_NAME(unit, mem), index, entry_index, i_array));
    			goto done;
			}

    		for (entry_index = 0; entry_index < entry_dw; entry_index++)
    		{
    			expected_value[entry_index] = cache_mem_test_generate_value(unit,  mem_orig, i_array, index, test_params->write_value_pattern) & mem_field_mask[entry_index];

    			if ((read_value[entry_index] & mem_field_mask[entry_index] ) == (expected_value[entry_index] & mem_field_mask[entry_index]) &&
    					(read_cache_value[entry_index] & mem_field_mask[entry_index] ) == (expected_value[entry_index] & mem_field_mask[entry_index]))
    			{
    		    	LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ: read_value 0x%X cache_value 0x%X == expected_value 0x%X, (0x%X == 0x%X), mask 0x%X, mem %d, index %d, entry_index %d array %d, len %d\n"),
    		    			read_value[entry_index] & mem_field_mask[entry_index],
    		    			read_cache_value[entry_index] & mem_field_mask[entry_index],
							expected_value[entry_index] & mem_field_mask[entry_index],
							read_value[entry_index], expected_value[entry_index],
							mem_field_mask[entry_index],
							mem, index, entry_index, i_array, entry_dw));
    			}
    			else
    			{
    		    	LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ FAILED: read_value 0x%X, cache_value 0x%X, expected_value 0x%X, (0x%X != 0x%X != 0x%X), mask 0x%X mem %d, index %d, entry_index %d array %d\n"),
    		    			read_value[entry_index] & mem_field_mask[entry_index],
    		    			read_cache_value[entry_index] & mem_field_mask[entry_index],
							expected_value[entry_index] & mem_field_mask[entry_index],
							read_value[entry_index], read_cache_value[entry_index], expected_value[entry_index],
							mem_field_mask[entry_index],
							mem, index, entry_index, i_array));
    				rv = SOC_E_LIMIT /* Unknown error */;
    				goto done;
    			}
    		}
    	}
    }
done:
	if (rv != SOC_E_NONE)
		test_params->result |= rv;

	return rv;

}

void do_cache_mem_test_print_usage(void)
{
	LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("Usage for Cache Memory Test: \n"
			"test_type=X         where X=0 for specific memory, X=1 for all memories\n"
			"write_type=X        where X=0 for DMA write,       X=1 for SCHAN write\n"
			"pattern=X           where X=0 for all-ones,  X=1 for all-zeroes,     X=2 for incremental,    X=3 for smart pattern\n"
			"part=X              where X=0 for Full-test, X=1 for only-read-part, X=2 for only-Write-part\n"
			"mem_id=X            where X is memory id for specific memory test (applicable with test_type=0)\n")));
}

int
do_cache_mem_test_init(int unit, args_t *a, void **p)
{

	cmd_result_t			rv 		= CMD_OK;
	tr_do_cache_mem_test_t 	*test_params;
	char					*tab;
	uint32					num = 0;

    *p = NULL;

    if ((test_params = sal_alloc(sizeof (tr_do_cache_mem_test_t), "cache_mem_test")) == 0)
    {
    	rv = SOC_E_MEMORY;
		goto done;
    }
    memset(test_params, 0, sizeof (*test_params));

    *p = (void *) test_params;

    /* Set default params */
    test_params->test_type					= cache_mem_test_type_all_mems;
    test_params->write_type 				= cache_mem_test_write_type_schan;
    test_params->write_value_pattern 		= cache_mem_test_write_value_pattern_incremental;
    test_params->test_part					= cache_mem_test_full;
    test_params->mem_id						= 0;
    test_params->result						= CMD_OK;

	if ((tab = ARG_GET(a)) == NULL) {
		do_cache_mem_test_print_usage();
		rv = CMD_USAGE;
		goto done;
	}

	/* Get Test Type */
	tab = sal_strstr(tab, "test_type");
	if (tab != NULL)
	{
		num = sal_ctoi(tab + 10/*'test_type=' is 10 chars*/, NULL);
		if (num <= cache_mem_test_type_all_mems && num >= cache_mem_test_type_single )
		{
			test_params->test_type = num;
		}
	}
	if ((tab = ARG_GET(a)) == NULL) {
		do_cache_mem_test_print_usage();
		rv = CMD_USAGE;
		goto done;
	}

	/* Get Write Type */
	tab = sal_strstr(tab, "write_type");
	if (tab != NULL)
	{
		num = sal_ctoi(tab + 11/*'write_type=' is 11 chars*/, NULL);
		if (num <= cache_mem_test_write_type_schan && num >= cache_mem_test_write_type_dma)
		{
			test_params->write_type = num;
		}
	}
	if ((tab = ARG_GET(a)) == NULL) {
		do_cache_mem_test_print_usage();
		rv = CMD_USAGE;
		goto done;
	}


	/* Get Pattern */
	tab = sal_strstr(tab, "pattern");
	if (tab != NULL)
	{
		num = sal_ctoi(tab + 8/*'pattern=' is 8 chars*/, NULL);
		if (num <= cache_mem_test_write_value_pattern_smart && num >= cache_mem_test_write_value_pattern_all_ones )
		{
			test_params->write_value_pattern = num;
		}
	}
	if ((tab = ARG_GET(a)) == NULL) {
		do_cache_mem_test_print_usage();
		rv = CMD_USAGE;
		goto done;
	}

	/* Get Test partial */
	tab = sal_strstr(tab, "part");
	if (tab != NULL)
	{
		num = sal_ctoi(tab + 5/*'part=' is 5 chars*/, NULL);
		if (num <= NUM_SOC_MEM && num >= 0)
		{
			test_params->test_part = num;
		}
	}

	if ((tab = ARG_GET(a)) == NULL) {
		do_cache_mem_test_print_usage();
		rv = CMD_USAGE;
		goto done;
	}

	/* Get Memory ID */
	tab = sal_strstr(tab, "mem_id");
	if (tab != NULL)
	{
		num = sal_ctoi(tab + 7/*'mem_id=' is 7 chars*/, NULL);
		if (num <= NUM_SOC_MEM && num >= 0)
		{
			test_params->mem_id = num;
		}
	}


#ifdef BCM_ARAD_SUPPORT
	if (SOC_IS_ARAD(unit))
	{
		bcm_common_linkscan_enable_set(unit,0);
		soc_counter_stop(unit);
		rv = bcm_dpp_counter_bg_enable_set(unit, FALSE);
		if (CMD_OK == rv)
		{
			LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST:unit %d counter processor background accesses suspended\n"), unit));
		}
		else
		{
			LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: unit %d counter processor background access suspend failed: %d (%s)\n"),
					unit, rv, _SHR_ERRMSG(rv)));
		}

		if ((rv |= soc_dpp_device_reset(unit, SOC_DPP_RESET_MODE_REG_ACCESS, SOC_DPP_RESET_ACTION_INOUT_RESET)) < 0)
		{
			LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "CACHE_MEM_TEST: unit %d ERROR: Unable to reinit  \n"), unit));
			goto done;
		}
	}
#endif

done:
	if (rv != CMD_OK)
		test_params->result |= rv;

    return rv;
}



int
do_cache_mem_test_done(int unit,  void *p)
{
	soc_error_t rv;
	tr_do_cache_mem_test_t 	*test_params = (tr_do_cache_mem_test_t*)p;

	rv = test_params->result;

	sal_free (p);

	LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "CACHE_MEM_TEST: unit %d done\n"), unit));
    return rv;
}


int
do_cache_mem_test(int unit,  args_t *a, void* tr_do_cache_mem_test)
{
	soc_error_t rv = BCM_E_NONE;
	tr_do_cache_mem_test_t *test_params = tr_do_cache_mem_test;
	uint32 					cache_enable = 1;

	/* Turn on cache memory for all tables */
	if (test_params->test_part == cache_mem_test_full || test_params->test_part == cache_mem_test_write_only)
	{
		if (soc_mem_iterate(unit, cache_mem_test_cache_set, &cache_enable) < 0)
			LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "CACHE_MEM_TEST: unit %d cache enable failed\n"), unit));
	}

	switch(test_params->test_type)
	{
		case cache_mem_test_type_all_mems:	/* Run tests for all memories */
		{
			/* Do the write part of the test */
			if (test_params->test_part == cache_mem_test_full || test_params->test_part == cache_mem_test_write_only)
			{
				if ((soc_mem_iterate(unit, cache_mem_test_write_iter_callback, tr_do_cache_mem_test)) < 0)
					rv = BCM_E_INTERNAL;
			}
			/* Do the read part of the test */
			if (test_params->test_part == cache_mem_test_full || test_params->test_part == cache_mem_test_read_only)
			{
				if ((soc_mem_iterate(unit, cache_mem_test_read_and_compare, tr_do_cache_mem_test)) < 0)
					rv |= BCM_E_INTERNAL;
			}
			break;
		}
		case cache_mem_test_type_single:	/* Run test for specific memory */
		{
			/* Do the write part of the test */
			if (test_params->test_part == cache_mem_test_full || test_params->test_part == cache_mem_test_write_only)
			{
				rv = cache_mem_test_write_iter_callback(unit, test_params->mem_id, tr_do_cache_mem_test);
			}
			/* Do the read part of the test */
			if (test_params->test_part == cache_mem_test_full || test_params->test_part == cache_mem_test_read_only)
			{
				rv |= cache_mem_test_read_and_compare(unit, test_params->mem_id, tr_do_cache_mem_test);
			}
			break;
		}
	}

	/* Turn off cache memory for all tables */
	cache_enable = 0;
	if (test_params->test_part == cache_mem_test_full || test_params->test_part == cache_mem_test_read_only)
	{
		if (soc_mem_iterate(unit, cache_mem_test_cache_set, &cache_enable) < 0)
			LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "CACHE_MEM_TEST: unit %d cache disable failed\n"), unit));
	}

	if (rv != SOC_E_NONE)
		test_params->result |= rv;

	return rv;
}

#endif /*(BCM_ESW_SUPPORT) || (BCM_PETRA_SUPPORT) || (BCM_DFE_SUPPORT)|| (BCM_ARAD_SUPPORT)*/
