/*
 * $Id: drv.h,v 1.0 Broadcom SDK $
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
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/* 
 * Includes
 */ 
#include <shared/bsl.h>
#include <shared/bitop.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>

/* SAL includes */
#include <sal/appl/sal.h>

/* SOC includes */
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/iproc.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>

/* SOC DPP includes */
#include <soc/dpp/drv.h>
#include <soc/dcmn/dcmn_mem.h>

/* SOC DPP JER includes */
#include <soc/dpp/JER/jer_tbls.h>
#include <soc/dpp/JER/jer_fabric.h>

/* SOC DPP Arad includes */ 
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_mirror.h>


/* 
 * Defines
 */

#define JER_MBMP_SET_EXCLUDED(_mem)      SHR_BITSET(jer_excluded_mems_bmap_p, _mem)
#define JER_MBMP_IS_EXCLUDED(_mem)       SHR_BITGET( jer_excluded_mems_bmap_p, _mem)

/**
 *   explanation to the content of the following skip list
 *    1. ILKN* are not realy memories
 *    2.  EPNI*  is defines only not real memory 
 *    3.  KAPS_TCAM  need special initialization done in kap
 *        module sw
 *    4.  SER* not touched by asic so no need to init
 *    5. SCH_SCHEDULER_INIT  is write only memory
 */

static soc_mem_t jer_tbls_88375_excluded_mem_list[] = {
    IRR_MCDBm,
    IRR_QUEUE_IS_OCB_COMMITTEDm,
    EPNI_LINK_LAYER_OR_ARP_NEW_FORMATm,
    EPNI_PROTECTION_ENTRYm,
    EPNI_TRILL_FORMAT_FULL_ENTRYm,
    EPNI_TRILL_FORMAT_HALF_ENTRYm,
    ILKN_PMH_PORT_0_CPU_ACCESSm,
    ILKN_PMH_PORT_1_CPU_ACCESSm,
    ILKN_PML_PORT_0_CPU_ACCESSm,
    ILKN_PML_PORT_1_CPU_ACCESSm,
    SCH_SCHEDULER_INITm,
    SER_ACC_TYPE_MAPm,
    SER_MEMORYm,
    SER_RESULT_0m,
    SER_RESULT_1m,
    SER_RESULT_DATA_0m,
    SER_RESULT_DATA_1m,
    SER_RESULT_EXPECTED_0m,
    SER_RESULT_EXPECTED_1m,
    EGQ_VSI_MEMBERSHIPm,
    KAPS_TCMm,
    EDB_MAP_OUTLIF_TO_DSPm, /* will be initialized to a none zero value */

    /* Has to be last memory in array */
    INVALIDm
};

static soc_mem_t *jer_tbls_88675_excluded_mem_list = jer_tbls_88375_excluded_mem_list;



/* 
 * Tables Init Functions
 */ 
uint32 soc_jer_sch_tbls_init(int unit)
{
    uint32 table_entry[128] = {0};
    int core;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);


    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, SCH_FC_MAP_FCMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, SCH_CH_NIF_RATES_CONFIGURATION_CNRCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, SCH_FLOW_SUB_FLOW_FSFm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));

    /* all relevant bits initialized to SOC_TMC_MAX_FAP_ID */
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry, DEVICE_NUMBERf, SOC_TMC_MAX_FAP_ID);
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry, DEVICE_NUMBERf, 0);

    /* trigger SCH_SCHEDULER_INIT to reset dynamic SCH tables */
    soc_mem_field32_set(unit, SCH_SCHEDULER_INITm, table_entry, SCH_INITf, 0x1);
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_INITm(unit, SCH_BLOCK(unit, core), 0x0, table_entry));
    }
    soc_mem_field32_set(unit, SCH_SCHEDULER_INITm, table_entry, SCH_INITf, 0x0);

    soc_mem_field32_set(unit, SCH_PS_8P_RATES_PSRm, table_entry, PS_8P_RATES_PSRf, 128);
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, SCH_PS_8P_RATES_PSRm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
    soc_mem_field32_set(unit, SCH_PS_8P_RATES_PSRm, table_entry, PS_8P_RATES_PSRf, 0);
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_irr_tbls_init(int unit)
{
    uint32 m[2]= {0};
    ARAD_INIT_OCB *ocb;

    SOCDNX_INIT_FUNC_DEFS;
    ocb = &SOC_DPP_CONFIG(unit)->arad->init.ocb;

    if (ocb->ocb_enable == OCB_ENABLED) {
        soc_mem_field32_set(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, m, COMMITMENTf, 0xffffffff); 
    } else {
        soc_mem_field32_set(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, m, COMMITMENTf, 0x0); 
    }
    
    if (SOC_DPP_CONFIG(unit)->emulation_system) { /* partial init for emulation: one core and part of the queues */
        SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, 0, 0, MEM_BLOCK_ALL, 0, 10000, m));
    } else {
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, MEM_BLOCK_ALL, m));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 soc_jer_mrps_tbls_init (int unit){
	int core_id;
	uint32 table_entry[4] = {0};
	SOCDNX_INIT_FUNC_DEFS;

	for(core_id = 0; core_id < SOC_DPP_CONFIG(unit)->meter.nof_meter_cores; core_id++){
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDA_PRFCFG_0m, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDB_PRFCFG_0m, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDA_PRFCFG_1m, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDB_PRFCFG_1m, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDA_PRFSELm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDB_PRFSELm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDA_HEADER_APPEND_SIZE_PTR_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDB_HEADER_APPEND_SIZE_PTR_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDA_IN_PP_PORT_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDB_IN_PP_PORT_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MRPS_MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
	}

exit:
    SOCDNX_FUNC_RETURN;

}

uint32 soc_jer_mrpsEm_tbls_init (int unit){
	int core_id;
	uint32 table_entry[4] = {0};
	SOCDNX_INIT_FUNC_DEFS;

	for(core_id = 0; core_id < SOC_DPP_CONFIG(unit)->meter.nof_meter_cores; core_id++){
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDA_PRFCFG_0m, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDB_PRFCFG_0m, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDA_PRFCFG_1m, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDB_PRFCFG_1m, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDA_PRFSELm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDB_PRFSELm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDA_HEADER_APPEND_SIZE_PTR_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDB_HEADER_APPEND_SIZE_PTR_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDA_IN_PP_PORT_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDB_IN_PP_PORT_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, MTRPS_EM_MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
	}

exit:
    SOCDNX_FUNC_RETURN;

}

uint32 soc_jer_idr_tbls_init (int unit){
	uint32 table_entry = 0;
	SOCDNX_INIT_FUNC_DEFS;

	
	SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IDR_ETHERNET_METER_CONFIGm,0,1, MEM_BLOCK_ANY,0,1279, &table_entry));

exit:
    SOCDNX_FUNC_RETURN;

}

uint32 soc_jer_ire_tbls_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

/*exit:*/
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_ihb_tbls_init(int unit)
{
    uint32 res;
    uint32 vsi_low_cfg[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    /* default FID = VSI */
    soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_2m, vsi_low_cfg, FID_CLASSf, 7);
    res = arad_fill_table_with_entry(unit, IHP_VSI_LOW_CFG_2m, MEM_BLOCK_ANY, vsi_low_cfg);
    SOCDNX_SAND_IF_ERR_EXIT(res);

	/* In Jericho, IHB_FEC_ENTRY_ACCESSE is dynamic, therefore we take dynamic access in advance*/
	SOCDNX_IF_ERR_EXIT(WRITE_IHB_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, SOC_CORE_ALL, 1));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_iqm_tbls_init( int unit)
{
    uint32 table_default[SOC_MAX_MEM_WORDS] = {0};
    uint32 table_pcp_default[SOC_MAX_MEM_WORDS] = {0xaaaaaaaa};
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    /* Initialize IQM_PACKING_MODE_TABLE*/
    /* Each line in the table configures fabric-pcp mode of 16 devices*/
    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pcp_enable) {
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IQM_PACK_MODEm, IQM_BLOCK(unit, SOC_CORE_ALL), table_pcp_default)) ;
    } else {
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IQM_PACK_MODEm, IQM_BLOCK(unit, SOC_CORE_ALL), table_default)) ;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_iqmt_tbls_init(int unit)
{
    uint32 table_entry = 0;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    /* IQMT_ING_RPT_PCM is dynamic, so we need to take dynamic access */
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));

    /* Initialize IQMT_ING_RPT_PCM */
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IQMT_ING_RPT_PCMm, IQMT_BLOCK(unit), &table_entry));

    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_ips_tbls_init(int unit)
{
    uint32 table_entry[1] = {ARAD_IPQ_INVALID_FLOW_QUARTET};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);
    /*
     * Mark whole table as 'invalid'. See arad_interrupt_handles_corrective_action_ips_qdesc().
     */
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, IPS_FLWIDm, IPS_BLOCK(unit, SOC_CORE_ALL), table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_ipt_tbls_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    SOCDNX_SAND_IF_ERR_EXIT(arad_mgmt_ipt_tbls_init(unit, 1));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_fdt_tbls_init(int unit)
{
    uint32 table_default[SOC_MAX_MEM_WORDS] = {0};
    uint32 pcp_config_data = 2;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    if ((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_CORE_MESH_MODE) == 0 && /* If the table is used for mesh MC, it must be cleared */
         SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pcp_enable) {
        /* fabric-pcp is configured in bits 6:7 of each line */
        SHR_BITCOPY_RANGE(table_default, 6, &pcp_config_data, 0, SOC_JER_FABRIC_PCP_LENGTH);
    } else {
        
    }

    /* Initialize FDT_IPT_MESH_MC table */
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, FDT_IPT_MESH_MCm, MEM_BLOCK_ALL, table_default));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_egq_tbls_init(int unit)
{
    soc_reg_above_64_val_t data;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PCTm, data, CGM_PORT_PROFILEf, ARAD_EGR_PORT_THRESH_TYPE_15);
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_PCTm, MEM_BLOCK_ALL, data));

    /* Set the VSI-Membership table to all 1's */
    SOC_REG_ABOVE_64_ALLONES(data);
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, EGQ_VSI_MEMBERSHIPm, MEM_BLOCK_ALL, data));

    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EDB_MAP_OUTLIF_TO_DSPm, MEM_BLOCK_ALL, data)); /* map all CUDs to a none valid port */

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_DSP_PTR_MAPm, data, OUT_TM_PORTf, ARAD_EGR_INVALID_BASE_Q_PAIR);
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, EGQ_DSP_PTR_MAPm, MEM_BLOCK_ALL, data));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_epni_tbls_init(int unit)
{
    uint32 res = 0;
    uint32 tx_tag_table_entry[8] = {0};
    uint32 table_default[3] = {0x10040100, 0x04010040, 0x4010};
    uint32 mtu_table_entry[4] = {0, 0, 0xF8000000, 0x1FF}; /*setting the MTU to max value*/
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize EPNI_MIRROR_PROFILE_TABLE*/
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EPNI_MIRROR_PROFILE_TABLEm, MEM_BLOCK_ALL, table_default));
    

    res = soc_sand_bitstream_fill(tx_tag_table_entry, 8);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_fill_table_with_entry(unit, EPNI_TX_TAG_TABLEm, MEM_BLOCK_ANY, &tx_tag_table_entry);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_pp_eg_mirror_init_unsafe(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_fill_table_with_entry(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, MEM_BLOCK_ANY, &mtu_table_entry);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 soc_jer_oamp_tbls_init(int unit)
{
    soc_reg_above_64_val_t table_default;
    uint32 write_val = 1;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(table_default);

    /* initialize DMM_TRIGER (sic) table*/
    SOCDNX_IF_ERR_EXIT(WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit , write_val));
    SOCDNX_SAND_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, OAMP_DM_TRIGERm, MEM_BLOCK_ALL, table_default));
    SOCDNX_SAND_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, OAMP_SAT_RX_FLOW_PARAMSm , MEM_BLOCK_ALL, table_default));
    SOCDNX_SAND_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, OAMP_SAT_RX_FLOW_STATSm , MEM_BLOCK_ALL, table_default));
    SOCDNX_SAND_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, OAMP_SAT_TXm , MEM_BLOCK_ALL, table_default));
    SOCDNX_SAND_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, OAMP_SAT_TX_EVC_PARAMS_ENTRY_1m , MEM_BLOCK_ALL, table_default));
    SOCDNX_SAND_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, OAMP_SAT_TX_EVC_PARAMS_ENTRY_2m , MEM_BLOCK_ALL, table_default));
    SOCDNX_SAND_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, OAMP_SAT_TX_GEN_PARAMSm , MEM_BLOCK_ALL, table_default));

    write_val = 0;
    SOCDNX_IF_ERR_EXIT(WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit , write_val));

    

exit:
    SOCDNX_FUNC_RETURN;
}



/*
 * Function:
 *      soc_jer_excluded_tbls_list_set
 * Purpose:
 *      sets the excluded memory list with the relevant memories
 * Parameters:
 *      unit    - Device Number 
 * Returns:
 *      SOC_E_XXX 
 * Note:
 *      to insert a memory to excluded list write the memory's name in the relevant exclude list above
 */
int soc_jer_excluded_tbls_list_set(int unit) 
{
    SHR_BITDCL *jer_excluded_mems_bmap_p = NULL;
    int mem_iter = 0;
    soc_mem_t* excluded_list;

    SOCDNX_INIT_FUNC_DEFS;

    /* get relevant exclude mems bmap of wanted device and define jer_excluded_mems_bmap_p for MACROs */
    jer_excluded_mems_bmap_p = SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap;

    /* get relevant exclude mems list of wanted device type */
    if (SOC_IS_JERICHO_A0(unit)) {
        excluded_list = jer_tbls_88675_excluded_mem_list;
    } else if(SOC_IS_QMX_A0(unit)){ 
        excluded_list = jer_tbls_88375_excluded_mem_list;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unknown Device Type\n")));
    }

    /* set exclude bmap to zero */
    sal_memset(jer_excluded_mems_bmap_p, 0, SHR_BITALLOCSIZE(NUM_SOC_MEM));

    /* iterate over exclude list to set bmap */
    mem_iter = 0;
    while (excluded_list[mem_iter] != INVALIDm) {
        JER_MBMP_SET_EXCLUDED(excluded_list[mem_iter]);
        ++mem_iter;
    }

exit:
    SOCDNX_FUNC_RETURN;     
}

/*
 * Function:
 *      soc_jer_static_tbls_reset
 * Purpose:
 *      iterates over all memories and resets the static ones
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_static_tbls_reset (int unit)
{
    SHR_BITDCL *jer_excluded_mems_bmap_p = NULL;
    int mem_iter = 0;
    uint32 reset_value[SOC_JER_TBLS_MAX_WORDS_FOR_REGULAR_STATIC_TABLE_ENTRY] = {0};

    SOCDNX_INIT_FUNC_DEFS;


    
    /* set excluded mem list */
    SOCDNX_IF_ERR_EXIT(soc_jer_excluded_tbls_list_set(unit));

    /* define jer_excluded_mems_bmap_p for MACROs */
    jer_excluded_mems_bmap_p = SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap;

    /* iterate over all mems */
    for (mem_iter = 0; mem_iter < NUM_SOC_MEM; mem_iter++) 
    {
        if (SOC_MEM_IS_VALID(unit, mem_iter) && /* Memory must be valid for the device */
            /* memory must be static (not dynamic) and not read-only */
            (soc_mem_flags(unit, mem_iter) & (SOC_MEM_FLAG_SIGNAL | SOC_MEM_FLAG_READONLY)) == 0 &&

            /* memory must not be an alias, to avoid multiple resets of the same memory */
            (mem_iter == SOC_MEM_ALIAS_MAP(unit, mem_iter) || !SOC_MEM_IS_VALID(unit, SOC_MEM_ALIAS_MAP(unit, mem_iter))) &&

            /* if entry is bigger than max entry size and this is a wide mem that should be ignored */
            soc_mem_entry_words(unit, mem_iter) <= SOC_JER_TBLS_MAX_WORDS_FOR_REGULAR_STATIC_TABLE_ENTRY &&
            !JER_MBMP_IS_EXCLUDED(mem_iter)) { /* if mem is in excluded list */

            /* reset memory - set all values to 0 */
            LOG_VERBOSE(BSL_LS_SOC_INIT,(BSL_META_U(unit,"Reseting static memory # %d - %s\n"),mem_iter, SOC_MEM_NAME(unit, mem_iter)));

            
            if (mem_iter == EDB_EEDB_TOP_BANKm || mem_iter == EDB_EEDB_BANKm) {
               int i;
                for (i=0;i<SOC_MEM_NUMELS(unit, mem_iter);i++) {
                    int j;
                    for (j=soc_mem_index_min(unit, mem_iter);j<soc_mem_index_max(unit, mem_iter);j++) {
                        SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit,mem_iter,i,-1,j,reset_value));
                    }

                }

            }
            else
            {
                SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, mem_iter, MEM_BLOCK_ALL, reset_value));
            }
        }
    }

    /* Init tables which are wrongly marked as dynamic */

    /* Allow writes to FDA_FDA_MCm, and stay with this configuration for later writes to this memory */
    SOCDNX_IF_ERR_EXIT(WRITE_FDA_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));

    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, FDA_FDA_MCm, MEM_BLOCK_ALL, reset_value));

    SOCDNX_IF_ERR_EXIT(WRITE_PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_PPDB_B_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));

    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm, MEM_BLOCK_ALL, reset_value));
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, MEM_BLOCK_ALL, reset_value));
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATIONm, MEM_BLOCK_ALL, reset_value));
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_24m, MEM_BLOCK_ALL, reset_value));
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_25m, MEM_BLOCK_ALL, reset_value));
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_26m, MEM_BLOCK_ALL, reset_value));
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_27m, MEM_BLOCK_ALL, reset_value));
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_28m, MEM_BLOCK_ALL, reset_value));
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_29m, MEM_BLOCK_ALL, reset_value));
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_30m, MEM_BLOCK_ALL, reset_value));
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_31m, MEM_BLOCK_ALL, reset_value));

    exit:
        SOCDNX_FUNC_RETURN;     
}
/*
 * Read table SCH_DEVICE_RATE_MEMORY_DRM from block SCH,
 * doesn't take semaphore.
 * See also:
 *   jer_sch_device_rate_entry_core_get_unsafe(), arad_sch_drm_tbl_get_unsafe()
 *   arad_sch_device_rate_entry_get_unsafe()
 */
uint32
  jer_sch_drm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_DRM_TBL_ENTRY_SIZE];
  soc_mem_t mem ;
  soc_field_t field ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DRM_TBL_GET_UNSAFE);
  err = soc_sand_os_memset(&(data[0]),0x0,sizeof(data)) ;
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);
  err = soc_sand_os_memset(SCH_drm_tbl_data,0x0,sizeof(ARAD_SCH_DRM_TBL_DATA));
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);
  /*
   * Note that this table is only for multi-core devices. Not for Arad.
   * field = (soc_field_t)DEVICE_RATEf ;
   */
  mem = (soc_mem_t)SCH_DEVICE_RATE_MEMORY_DRMm ;
  field = (soc_field_t)DEVICE_RATEf ;
  err = soc_mem_read(unit,mem,SCH_BLOCK(unit, core),entry_offset,data) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);
  SCH_drm_tbl_data->device_rate = soc_mem_field32_get(unit,mem,data,field);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_sch_drm_tbl_get_unsafe()",0,0);
}
/*
 * Write table SCH_DEVICE_RATE_MEMORY_DRM from block SCH,
 * doesn't take semaphore.
 * See also:
 *   jer_sch_device_rate_entry_core_set_unsafe(), arad_sch_drm_tbl_set_unsafe()
 */
uint32
  jer_sch_drm_tbl_set_unsafe(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_IN   int                   core,
    SOC_SAND_IN   uint32                entry_offset,
    SOC_SAND_IN   ARAD_SCH_DRM_TBL_DATA *SCH_drm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_DRM_TBL_ENTRY_SIZE];
  soc_mem_t mem ;
  soc_field_t field ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DRM_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(&(data[0]),0x0,sizeof(data)) ;
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);
  /*
   * Note that this table is only for multi-core devices. Not for Arad.
   * field = (soc_field_t)DEVICE_RATEf ;
   */
  mem = (soc_mem_t)SCH_DEVICE_RATE_MEMORY_DRMm ;
  field = (soc_field_t)DEVICE_RATEf ;
  /*
   * Use SCH0 although, apparently, any of the two may be used.
   */
  soc_mem_field32_set(unit,mem,data,field,SCH_drm_tbl_data->device_rate);
  err = soc_mem_write(unit,mem,SCH_BLOCK(unit, core),entry_offset,data) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_sch_drm_tbl_set_unsafe()",0,0);
}
/*
 * Read indirect table shds_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  jer_sch_shds_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_SHDS_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SHDS_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_shds_tbl_data,
          0x0,
          sizeof(ARAD_SCH_SHDS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
          SCH_BLOCK(unit,core),
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  SCH_shds_tbl_data->peak_rate_man_even   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    PEAK_RATE_MAN_EVENf            );
  SCH_shds_tbl_data->peak_rate_exp_even   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    PEAK_RATE_EXP_EVENf            );
  SCH_shds_tbl_data->max_burst_even   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    MAX_BURST_EVENf            );
  SCH_shds_tbl_data->slow_rate2_sel_even   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    SLOW_RATE_2_SEL_EVENf            );
  SCH_shds_tbl_data->peak_rate_man_odd   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    PEAK_RATE_MAN_ODDf            );
  SCH_shds_tbl_data->peak_rate_exp_odd   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    PEAK_RATE_EXP_ODDf            );
  SCH_shds_tbl_data->max_burst_odd   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    MAX_BURST_ODDf            );
  SCH_shds_tbl_data->slow_rate2_sel_odd   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    SLOW_RATE_2_SEL_ODDf            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_shds_tbl_get_unsafe()",0,0);
}
/*
 * Write indirect table shds_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  jer_sch_shds_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          entry_offset,
    SOC_SAND_IN   ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  )
{
  uint32
    err;
  uint32
    data_shds[ARAD_SCH_SHDS_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SHDS_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data_shds[0]),
          0x0,
          sizeof(data_shds)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,PEAK_RATE_MAN_EVENf,
    SCH_shds_tbl_data->peak_rate_man_even );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,PEAK_RATE_EXP_EVENf,
    SCH_shds_tbl_data->peak_rate_exp_even );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,MAX_BURST_EVENf,
    SCH_shds_tbl_data->max_burst_even );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,SLOW_RATE_2_SEL_EVENf,
    SCH_shds_tbl_data->slow_rate2_sel_even );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,PEAK_RATE_MAN_ODDf,
    SCH_shds_tbl_data->peak_rate_man_odd );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,PEAK_RATE_EXP_ODDf,
    SCH_shds_tbl_data->peak_rate_exp_odd );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,MAX_BURST_ODDf,
    SCH_shds_tbl_data->max_burst_odd );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,SLOW_RATE_2_SEL_ODDf,
    SCH_shds_tbl_data->slow_rate2_sel_odd );
  {
    uint32  val32 ;
    uint32 slow_max_bucket_width ;
    soc_field_info_t *soc_field_info_p ;
    /*
     * maximal allowed value for 'max_burst' field
     */
    uint32 max_val_max_burst ;
    /*
     * Bits in field
     */
    uint16 max_burst_field_len ;
    /*
     * Space to hold data to be loaded into TMC table
     */
    uint32
      data_tmc[JERICHO_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_ENTRY_SIZE] ;

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(err,10,exit,ARAD_REG_ACCESS_ERR,READ_SCH_SHAPER_CONFIGURATION_REGISTER_1r(unit, core, &val32)) ;
    slow_max_bucket_width = soc_reg_field_get(unit, SCH_SHAPER_CONFIGURATION_REGISTER_1r, val32, SLOW_MAX_BUCKET_WIDTHf) ;
    /*
     * Verify: The SLOW_MAX_BUCKET_WIDTH field (on SCH_SHAPER_CONFIGURATION_REGISTER_1)
     * may only have values from 0 to 3.
     * Redundant but safer.
     */
    SOC_SAND_ERR_IF_ABOVE_MAX(slow_max_bucket_width, 3, ARAD_FLD_OUT_OF_RANGE, 20, exit) ;
    /*
     * We assume that the widths of MAX_BURST_EVEN and MAX_BURST_ODD are the same.
     */
    soc_field_info_p = soc_mem_fieldinfo_get(unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,MAX_BURST_EVENf) ;
    SOC_SAND_CHECK_NULL_PTR(soc_field_info_p, 30, exit) ;
    max_burst_field_len = soc_field_info_p->len ;
    max_val_max_burst = (1 << max_burst_field_len) - 1 ;
    /*
     * For slow_max_bucket_width = 2,3 the maximum allowed val for MAX_BURST_EVEN/MAX_BURST_ODD is 255,127
     * respectively. 
     * For slow_max_bucket_width = 0,1 the maximum allowed val for MAX_BURST_EVEN/MAX_BURST_ODD is 511
     */
    if (slow_max_bucket_width >= 2)
    {
      max_val_max_burst = (1 << (max_burst_field_len - slow_max_bucket_width + 1)) - 1 ;
    }
    /*
     * Start by verifying range for MAX_BURST_EVEN/MAX_BURST_ODD.
     *
     * Load fields TOKEN_COUNT,SLOW_STATUS on:
     * SCH_TOKEN_MEMORY_CONTROLLER_TMC (even flow)
     * SCH_TOKEN_MEMORY_CONTROLLER_TMC_MSB (odd flow)
     * TOKEN_COUNT gets the value of MAX_BURST_EVEN/MAX_BURST_ODD as set above.
     * Note that SLOW_STATUS is ALWAYS set to '1' 
     */
    if (SCH_shds_tbl_data->max_burst_update_even)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(SCH_shds_tbl_data->max_burst_even, max_val_max_burst,ARAD_FLD_OUT_OF_RANGE,40,exit) ;
      err =
        soc_sand_os_memset(&(data_tmc[0]),0x0,sizeof(data_tmc));
      soc_mem_field32_set(
        unit,SCH_TOKEN_MEMORY_CONTROLLER_TMCm,
        data_tmc,TOKEN_COUNTf,SCH_shds_tbl_data->max_burst_even );
      soc_mem_field32_set(
        unit,SCH_TOKEN_MEMORY_CONTROLLER_TMCm,
        data_tmc,SLOW_STATUSf,1 );
      err =
        soc_mem_write(
          unit,SCH_TOKEN_MEMORY_CONTROLLER_TMCm,
          SCH_BLOCK(unit,core),entry_offset,data_tmc);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 50, exit);
    }
    if (SCH_shds_tbl_data->max_burst_update_odd)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(SCH_shds_tbl_data->max_burst_odd, max_val_max_burst,ARAD_FLD_OUT_OF_RANGE,60,exit) ;
      err =
        soc_sand_os_memset(&(data_tmc[0]),0x0,sizeof(data_tmc));
      soc_mem_field32_set(
        unit,SCH_TOKEN_MEMORY_CONTROLLER_TMC_MSBm,
        data_tmc,TOKEN_COUNTf,
        SCH_shds_tbl_data->max_burst_odd );
      soc_mem_field32_set(
        unit,SCH_TOKEN_MEMORY_CONTROLLER_TMC_MSBm,
        data_tmc,SLOW_STATUSf,1 );
      err =
        soc_mem_write(
          unit,SCH_TOKEN_MEMORY_CONTROLLER_TMC_MSBm,
          SCH_BLOCK(unit,core),entry_offset,data_tmc);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 70, exit);
    }
  }
  err = soc_mem_write(
          unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
          SCH_BLOCK(unit,core),entry_offset,
          data_shds);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 80, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_shds_tbl_set_unsafe()",0,0);
}
