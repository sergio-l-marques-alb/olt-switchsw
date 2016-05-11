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
 * File: qax_nif.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/dcmn_port.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/phy/phymod_sim.h>
#include <soc/dpp/port_sw_db.h>

#define SOC_QAX_IS_ILKN24(offset) ((offset == 0) || (offset == 2))
#define SOC_QAX_IS_ILKN12(offset) ((offset == 1) || (offset == 3))

#define SOC_QAX_NOF_QMLFS               12
#define SOC_QAX_NOF_LANES_PER_NBI       16
#define SOC_QAX_NOF_QUADS_PER_NBI       4
#define SOC_QAX_NOF_PORTS_NBIH          16
#define SOC_QAX_NOF_PORTS_EACH_NBIL     52
#define SOC_QAX_NIF_ILKN_MAX_NOF_LANES  24
#define SOC_QAX_SUB_PHYS_IN_QSGMII      4
#define SOC_QAX_NOF_ILKN_WRAP			2
#define SOC_QAX_MAX_PMS_PER_ILKN_PORT	6
#define SOC_QAX_HRF_SCH_PRIO_BIT_NUM	5
#define SOC_QAX_NOF_NBIL				2

static soc_dpp_pm_entry_t soc_qax_pml_table[] = {
     /* is_qsgmii, is_vaild, pml_instance , PHY ID (used as core address in PHYMOD*/
        {1,         1,          0,            0x00},
        {1,         1,          0,            0x04},
        {1,         1,          0,            0x08},
        {0,         1,          0,            0x0c},

        {1,         1,          1,            0x00},
        {1,         1,          1,            0x04},
        {1,         1,          1,            0x08},
        {0,         1,          1,            0x0c},
};

static portmod_pm_instances_t qax_pm_instances[] = {
    {portmodDispatchTypePm4x25, 4},
    {portmodDispatchTypeDnx_fabric_o_nif, 4},
    {portmodDispatchTypePm4x10, 2},
    {portmodDispatchTypePm4x10Q, 6},
    {portmodDispatchTypePmOsILKN, 2}
};

static portmod_pm_identifier_t qax_ilkn_pm_table[SOC_QAX_NOF_ILKN_WRAP][SOC_QAX_MAX_PMS_PER_ILKN_PORT] =
{
		{
		  {portmodDispatchTypePm4x25, 1},
		  {portmodDispatchTypePm4x25, 5},
		  {portmodDispatchTypePm4x25, 9},
		  {portmodDispatchTypePm4x25, 13},
		  {portmodDispatchTypePm4x10, 29},
		  {portmodDispatchTypePm4x10, 45}
		},
		{
		  {portmodDispatchTypePm4x10, 17},
		  {portmodDispatchTypePm4x10, 21},
		  {portmodDispatchTypePm4x10, 25},
		  {portmodDispatchTypePm4x10, 33},
		  {portmodDispatchTypePm4x10, 37},
		  {portmodDispatchTypePm4x10, 41}
		},
};

/* HRF0 (ilkn0 - data): PPC prio  12 - 16
   HRF1 (ilkn1 - data): PPC prio  17 - 21
   HRF4 (ilkn2 - data): PPC prio  22 - 26
   HRF6 (ilkn3 - data): PPC prio  27 - 31 */
STATIC uint32 qax_hrf_sch_index[] = {12, 17, 22, 27};

STATIC int soc_qax_port_sch_hrf_config(int unit, soc_port_t port);

int
soc_qax_pml_table_get(int unit, soc_dpp_pm_entry_t **soc_pml_table)
{
    SOCDNX_INIT_FUNC_DEFS;

    *soc_pml_table = &soc_qax_pml_table[0];
    
    SOCDNX_FUNC_RETURN;
}


int
soc_qax_pm_instances_get(int unit, portmod_pm_instances_t **pm_instances, int *pms_instances_arr_len)
{
    SOCDNX_INIT_FUNC_DEFS;

    *pm_instances = &qax_pm_instances[0];
    *pms_instances_arr_len = 5;

    SOCDNX_FUNC_RETURN;
}

int
soc_qax_qsgmii_offsets_add(int unit, uint32 phy, uint32 *new_phy) 
{
    int qsgmii_count, skip;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QUX(unit)) {
         skip = 0;
    } else {
        if(phy < 17) {
            skip = 0;
        } else {
            qsgmii_count = phy - 17;
            if (qsgmii_count > 12) {
                qsgmii_count = 12;
            }
            if (phy > 8*4) {
               qsgmii_count += phy - 33; 
               if (qsgmii_count > 24) {
                   qsgmii_count = 24;
               }
            }

            skip = qsgmii_count * (SOC_QAX_SUB_PHYS_IN_QSGMII - 1);
        }
    }

    *new_phy = phy + skip;

    SOCDNX_FUNC_RETURN;
}

int
soc_qax_qsgmii_offsets_remove(int unit, uint32 phy, uint32 *new_phy) 
{
    int qsgmii_count;
    int reduce = 0;
    SOCDNX_INIT_FUNC_DEFS;

    reduce = 0;

    if (!SOC_IS_QUX(unit) && (phy >= 17)) {
        qsgmii_count = phy - 17;
        if (qsgmii_count > 48) {
            qsgmii_count = 48;
        } 

        if (phy >= 69) {
           qsgmii_count += phy - 69;
        }
        if (qsgmii_count > 96) {
            qsgmii_count = 96;
        }
        reduce = qsgmii_count - (qsgmii_count / SOC_QAX_SUB_PHYS_IN_QSGMII);
    }

    *new_phy = phy - reduce;

    SOCDNX_FUNC_RETURN;
}

STATIC int qax_enable_ilkn_fields[] = {
    ENABLE_PORT_0f, 
    ENABLE_PORT_1f, 
    ENABLE_PORT_2f, 
    ENABLE_PORT_3f,
};


int
soc_qax_port_open_fab_o_nif_path(int unit, int port) {


    uint32 reg_val, otp_bits;
    int phy = 0;
    int quad, idx, reg_port;
    SOCDNX_INIT_FUNC_DEFS;


    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy = SOC_INFO(unit).port_l2p_mapping[port];
    } else{
        phy = port;
    }

    quad = (phy - 1) / 4; 
    reg_port = REG_PORT_ANY;
    idx = quad % 4;
            
    /* Enable power on active lanes */
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_NIF_PM_CFGr, reg_port, idx, &reg_val));
    otp_bits = soc_reg_field_get(unit, NBIH_NIF_PM_CFGr, reg_val, PMH_N_OTP_PORT_BOND_OPTIONf);
    otp_bits |= 0x100; /* enable this quad */
    otp_bits &= ~0xf0;
    soc_reg_field_set(unit, NBIH_NIF_PM_CFGr, &reg_val, PMH_N_OTP_PORT_BOND_OPTIONf, otp_bits);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_NIF_PM_CFGr, reg_port, idx, reg_val));
                
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC

int
soc_qax_port_fabric_o_nif_fgs_enable(int unit, int port, int enable) {


    uint32 reg_val;
    int lane;
    SOCDNX_INIT_FUNC_DEFS;

    lane = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);

    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_MACT_PM_CREDIT_FILTERr(unit, lane / 4, lane % 4, 0x2)); 

    /*FGS out of reset*/
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_FGS_RSTNr(unit, lane, enable));

    /* Mask Link Status (bit 0) which is not relevant for bypass mode.
     * Keep Signal OK (bit 1) and CDR Lock (bit 2) for SIGNAL DETECTION indication*/
    SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PMH_FGS_SRDS_SIG_DET_CFGr(unit, lane, &reg_val));
    soc_reg_field_set(unit, NBIH_NIF_PMH_FGS_SRDS_SIG_DET_CFGr, &reg_val, SRDS_SIG_DET_CFG_LANE_Nf, enable);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_FGS_SRDS_SIG_DET_CFGr(unit, lane, reg_val));

    if (enable) {
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_MACT_PM_CREDIT_FILTERr(unit, lane / 4, lane % 4, 0x0)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_fabric_o_nif_bypass_interface_enable(int unit, int port, int enable) {


    uint32 reg_val;
    uint32  rst_lanes = 0;
    int lane;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_qax_port_fabric_o_nif_fgs_enable(unit, port, 0));
    

    lane = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);

    rst_lanes |= 1 << lane;

    SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, &reg_val));
    if (enable) {
        reg_val |= rst_lanes; 
    } else {
        reg_val &= ~rst_lanes;
    }
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_TX_RSTNr(unit, &reg_val));
    if (enable) {
        reg_val |= rst_lanes; 
    } else {
        reg_val &= ~rst_lanes;
    }
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_ILKN_TX_RSTNr(unit, reg_val));

    if (enable) {
        SOCDNX_IF_ERR_EXIT(soc_qax_port_fabric_o_nif_fgs_enable(unit, port, 1));
    }

    

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_qax_ilkn_pmh_clk_data_mux_set(int unit)
{
    int port;
    SOCDNX_INIT_FUNC_DEFS;

    for (port=0 ; port < SOC_QAX_NOF_LANES_PER_NBI ; port++)
    {
    	SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_CLK_BYPASS_MUXr(unit, port, 1));
    	SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_DATA_BYPASS_MUXr(unit, port, 1));
    }

    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_CLK_DATA_PATH_MUXr(unit, 0));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_qax_ilkn_pml_clk_data_mux_set(int unit)
{
    int port;
    SOCDNX_INIT_FUNC_DEFS;

    for (port=0 ;  port < SOC_QAX_NOF_NBIL ; port++)
    {
    	SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PML_CLK_DATA_PATH_MUXr(unit, port, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_open_ilkn_path(int unit, int port) {

    ARAD_PORTS_ILKN_CONFIG *ilkn_config;
    SOC_TMC_PORT_HEADER_TYPE hdr_type;
    soc_pbmp_t phys, phy_lanes;
    soc_reg_t reg=0;
    soc_field_t field;
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val;
    uint32 reg_val, offset, field_value;
    uint32 is_master, tdm_index, num_lanes, egr_if, fld_val[1], credit_val;
    int mubits_tx_polarity, mubits_rx_polarity, fc_tx_polarity, fc_rx_polarity, core, nbil_reg_port, nbih_reg_offset;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phys);
    SOC_PBMP_CLEAR(phy_lanes);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
    /* offset = 0/1 -> ILKN_PMHL, offset = 2/3 -> ILKN_PMLQ */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));
        
    nbih_reg_offset = nbil_reg_port = offset >> 1;

    if (is_master) {

        SOC_PBMP_PORT_ADD(SOC_INFO(unit).custom_reg_access.custom_port_pbmp, port);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &phy_lanes));

        /* Enable ILKN in wrapper */
        field = (offset & 1) ? ILKN_1_PORT_RSTNf : ILKN_0_PORT_RSTNf;
        reg = (offset < 2 ) ? ILKN_PMH_ILKN_RESETr : ILKN_PML_ILKN_RESETr;

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val));
        soc_reg_field_set(unit, reg, &reg_val, field, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val));
        
        /* Enable NBIH - common to all ILKN ports*/
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKENr(unit, &reg_val)); 
        soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, qax_enable_ilkn_fields[offset], 1);

        if ( SOC_QAX_IS_ILKN24(offset) ) {
            field = (offset == 0) ? HRF_0_TAKES_TWO_SEGMENTSf : HRF_2_TAKES_TWO_SEGMENTSf;
            field_value = (num_lanes <= 12) ? 1 : 0;
            soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, field, field_value);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKENr(unit, reg_val));

        if( SOC_QAX_IS_ILKN12(offset) ) {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_ENABLE_INTERLAKENr(unit, nbil_reg_port, &reg_val));
            soc_reg_field_set(unit, NBIL_ENABLE_INTERLAKENr, &reg_val, ENABLE_PORT_0f, 1);
            soc_reg_field_set(unit, NBIL_ENABLE_INTERLAKENr, &reg_val, HRF_0_ACTIVEf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ENABLE_INTERLAKENr(unit, nbil_reg_port, reg_val));

            reg = (offset < 2 ) ? ILKN_PMH_ENABLE_INTERLAKENr : ILKN_PML_ENABLE_INTERLAKENr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val));
            soc_reg_field_set(unit, reg, &reg_val, SELECT_TX_WORDS_23_FROM_NBIL_NOT_NBIHf, 1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val));
        }

    }

    /* Config HRF credits in NBI */
    credit_val = ilkn_config->retransmit.enable_tx ? 1024 : 512;
    if ( SOC_QAX_IS_ILKN24(offset) ) {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_HRF_TX_CONFIG_HRFr(unit, nbih_reg_offset, &reg_val));
        soc_reg_field_set(unit, NBIH_HRF_TX_CONFIG_HRFr, &reg_val, HRF_TX_NUM_CREDITS_TO_EGQ_HRF_Nf, credit_val);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_HRF_TX_CONFIG_HRFr(unit, nbih_reg_offset, reg_val));
    } else { /* ILKN-12 */
        SOCDNX_IF_ERR_EXIT(READ_NBIL_HRF_TX_CONFIG_HRFr(unit, nbil_reg_port, 0, &reg_val));
        soc_reg_field_set(unit, NBIL_HRF_TX_CONFIG_HRFr, &reg_val, HRF_TX_NUM_CREDITS_TO_EGQ_HRF_Nf, credit_val);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_HRF_TX_CONFIG_HRFr(unit, nbil_reg_port, 0, reg_val));
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &hdr_type);
    SOCDNX_IF_ERR_EXIT(rv);

    if ( SOC_QAX_IS_ILKN24(offset) )
    {
    	tdm_index = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? 2 : 0;
    	SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, NBIH_HRF_RX_CONFIG_HRFr, REG_PORT_ANY, tdm_index + nbih_reg_offset, &reg64_val));
    	soc_reg64_field32_set(unit, NBIH_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, ilkn_config->interleaved ? 0 : 1);
    	SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, NBIH_HRF_RX_CONFIG_HRFr, REG_PORT_ANY, tdm_index + nbih_reg_offset, reg64_val));
    }
    else /* ILKN-12 */
    {
    	tdm_index = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? 1 : 0;
    	SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, NBIL_HRF_RX_CONFIG_HRFr, nbil_reg_port, tdm_index, &reg64_val));
    	soc_reg64_field32_set(unit, NBIL_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, ilkn_config->interleaved ? 0 : 1);
    	SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, NBIL_HRF_RX_CONFIG_HRFr, nbil_reg_port, tdm_index, reg64_val));
    }

    /* #### */
    /* set_ilkn_tx_hrf_rstn - Make sure HRF is not enabled before it is configured */ 
    if ( SOC_QAX_IS_ILKN24(offset) )
    {
    	SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_HRF_RESETr, REG_PORT_ANY, 0, &reg_val));
        field = (offset == 0) ? HRF_TX_0_CONTROLLER_RSTNf : HRF_TX_1_CONTROLLER_RSTNf;
        soc_reg_field_set(unit, NBIH_HRF_RESETr, &reg_val, field, 1);
        field = (offset == 0) ? HRF_RX_0_CONTROLLER_RSTNf : HRF_RX_2_CONTROLLER_RSTNf;
        soc_reg_field_set(unit, NBIH_HRF_RESETr, &reg_val, field, 1);
        if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
            field = (offset == 0) ? HRF_RX_1_CONTROLLER_RSTNf : HRF_RX_3_CONTROLLER_RSTNf;
            soc_reg_field_set(unit, reg, &reg_val, field, 1);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_HRF_RESETr, REG_PORT_ANY, 0, reg_val));
    }
    else /* ILKN-12 */
    {
    	SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIL_HRF_RESETr, nbil_reg_port, 0, &reg_val));
    	soc_reg_field_set(unit, NBIL_HRF_RESETr, &reg_val, HRF_RX_0_CONTROLLER_RSTNf, 1);
    	soc_reg_field_set(unit, NBIL_HRF_RESETr, &reg_val, HRF_TX_0_CONTROLLER_RSTNf, 1);
    	if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
    		soc_reg_field_set(unit, NBIL_HRF_RESETr, &reg_val, HRF_RX_1_CONTROLLER_RSTNf, 1);
    	}
    	SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIL_HRF_RESETr, nbil_reg_port, 0, reg_val));
    }

    /* All HRFs indications in NBIF for SCH */
    /* set_ilkn_rx_hrf_en */
    switch (offset) {
    case 0:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_2f : RX_HRF_ENABLE_HRF_0f;
        break;
    case 1:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_3f : RX_HRF_ENABLE_HRF_1f;
        break;
    case 2:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_6f : RX_HRF_ENABLE_HRF_4f;
        break;
    case 3:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_7f : RX_HRF_ENABLE_HRF_5f;
        break;
    default:
    	SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid ILKN offset %d"), offset));
    }
    SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKEN_HRFr(unit, &reg_val)); 
    soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKEN_HRFr, &reg_val, field, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKEN_HRFr(unit, reg_val));

    /* set_enable_ilkn_port */
    reg = (offset < 2) ? ILKN_PMH_ENABLE_INTERLAKENr : ILKN_PML_ENABLE_INTERLAKENr;
    field = (offset & 1) ? ENABLE_PORT_1f : ENABLE_PORT_0f;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, reg, &reg_val, field, 1);
    soc_reg_field_set(unit, reg, &reg_val, ENABLE_CORE_CLOCKf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val));

    /* set_fc_ilkn_cfg */
    mubits_tx_polarity = ilkn_config->mubits_tx_polarity;
    mubits_rx_polarity = ilkn_config->mubits_rx_polarity;
    fc_tx_polarity = ilkn_config->fc_tx_polarity;
    fc_rx_polarity = ilkn_config->fc_tx_polarity;
    if (mubits_tx_polarity || mubits_rx_polarity || fc_tx_polarity || fc_rx_polarity) {
        reg = (offset < 2) ? ILKN_PMH_ILKN_INVERT_POLARITY_SIGNALSr : ILKN_PML_ILKN_INVERT_POLARITY_SIGNALSr; 
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, (offset & 1), &reg_val));
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_RX_MUBITS_POLARITYf, mubits_rx_polarity);
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_TX_MUBITS_POLARITYf, mubits_tx_polarity);
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_RECEIVED_FC_POLARITYf, fc_rx_polarity);
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_TX_FC_POLARITYf, fc_tx_polarity);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, (offset & 1), reg_val));
    }

    /* set_tx_retransmit_enable */
    reg = (offset < 2) ? ILKN_PMH_ILKN_TX_CONFr : ILKN_PML_ILKN_TX_CONFr;
    field = (offset & 1) ? TX_1_RETRANS_ENf : TX_0_RETRANS_ENf;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_above_64_val)); 
    soc_reg_above_64_field32_set(unit, reg, &reg_val, field, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, reg_above_64_val));

    /* set fragmentation */
    if (is_master) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
        SOCDNX_IF_ERR_EXIT(rv);

        SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
        *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);        
        SHR_BITCLR(fld_val, egr_if);
        soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

        /* handle ILKN dedicated mode */
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
            if (IS_TDM_PORT(unit, port)) {
                egr_if++;
            } else {
                egr_if--;
            }

            SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
            *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);        
            SHR_BITCLR(fld_val, egr_if);
            soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);
            SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
        }
    }

    SOCDNX_IF_ERR_EXIT(soc_qax_port_sch_hrf_config(unit, port));

exit:
    SOCDNX_FUNC_RETURN;
}


int 
soc_qax_port_close_ilkn_path(int unit, int port) {

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_ilkn_init(int unit)
{
	int pm_index;
	soc_pbmp_t ilkn_phys;
    portmod_pm_create_info_t pm_info;
    portmod_pm_identifier_t ilkn_pms[MAX_NUM_OF_PMS_IN_ILKN];
    portmod_pm_identifier_t *ilkn_table;

    SOCDNX_INIT_FUNC_DEFS;


    /* ILKN 0,1 */
    SOC_PBMP_CLEAR(ilkn_phys);
    for(pm_index = 0 ; pm_index < MAX_NUM_OF_PMS_IN_ILKN ; pm_index++)
    {
        portmod_pm_identifier_t_init(unit, &ilkn_pms[pm_index]);
    }

    ilkn_table = qax_ilkn_pm_table[0];

    for(pm_index=0 ; pm_index<MAX_NUM_OF_PMS_IN_ILKN ; pm_index++)
    {
    	ilkn_pms[pm_index].type = ilkn_table[pm_index].type;
    	ilkn_pms[pm_index].phy = ilkn_table[pm_index].phy;

    	SOC_PBMP_PORTS_RANGE_ADD(ilkn_phys, ilkn_pms[pm_index].phy, NUM_OF_LANES_IN_PM);
    }

    SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN;
    pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms;
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = MAX_NUM_OF_PMS_IN_ILKN;
    pm_info.pm_specific_info.os_ilkn.wm_high = 10;
    pm_info.pm_specific_info.os_ilkn.wm_low = 11;
    SOC_PBMP_ASSIGN(pm_info.phys, ilkn_phys);
    SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));



    /* ILKN 2,3 */
    SOC_PBMP_CLEAR(ilkn_phys);
    for(pm_index = 0 ; pm_index < MAX_NUM_OF_PMS_IN_ILKN ; pm_index++)
    {
        portmod_pm_identifier_t_init(unit, &ilkn_pms[pm_index]);
    }

    ilkn_table = qax_ilkn_pm_table[1];

    for(pm_index=0 ; pm_index<MAX_NUM_OF_PMS_IN_ILKN ; pm_index++)
    {
    	ilkn_pms[pm_index].type = ilkn_table[pm_index].type;
    	ilkn_pms[pm_index].phy = ilkn_table[pm_index].phy;

    	SOC_PBMP_PORTS_RANGE_ADD(ilkn_phys, ilkn_pms[pm_index].phy, NUM_OF_LANES_IN_PM);
    }

    SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN;
    pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms;
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = MAX_NUM_OF_PMS_IN_ILKN;
    SOC_PBMP_ASSIGN(pm_info.phys, ilkn_phys);
    pm_info.pm_specific_info.os_ilkn.wm_high = 10;
    pm_info.pm_specific_info.os_ilkn.wm_low = 11;
    SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));


	if (!SOC_IS_KALIA(unit))
	{
		SOCDNX_IF_ERR_EXIT(soc_qax_ilkn_pmh_clk_data_mux_set(unit));
	}
	SOCDNX_IF_ERR_EXIT(soc_qax_ilkn_pml_clk_data_mux_set(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_sch_config(int unit, soc_port_t port)
{
    soc_reg_above_64_val_t reg_above_64_val, rx_req_low_en;
    uint32 reg_val;
    soc_pbmp_t quad_bmp;
    SOC_TMC_PORT_HEADER_TYPE hdr_type;
    uint32 rx_req_high_en[1] = {0}, rx_req_tdm_en[1] = {0}; 
    soc_error_t rv;
    int qmlf_index;
    SOCDNX_INIT_FUNC_DEFS;

    /* Port Mode */
    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_port_quad_get(unit, port, &quad_bmp));

    /* ReqEn register */
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &hdr_type);
    SOCDNX_IF_ERR_EXIT(rv);
    if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, &reg_val));
        *rx_req_tdm_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, reg_val, RX_REQ_PIPE_0_TDM_ENf);
        SOC_PBMP_ITER(quad_bmp, qmlf_index) {
            if (qmlf_index >= SOC_QAX_NOF_QMLFS)
                break;

            SHR_BITSET(rx_req_tdm_en, qmlf_index);
        }
        soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, &reg_val, RX_REQ_PIPE_0_TDM_ENf, *rx_req_tdm_en);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, reg_val));

    } else {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
        soc_reg_above_64_field_get(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en);
        SOC_PBMP_ITER(quad_bmp, qmlf_index) {
            if (qmlf_index >= SOC_QAX_NOF_QMLFS)
                break;

            SHR_BITSET(rx_req_low_en, qmlf_index);
            SHR_BITSET(rx_req_low_en, qmlf_index + 32);
            SHR_BITSET(rx_req_low_en, qmlf_index + 64);
            SHR_BITSET(rx_req_low_en, qmlf_index + 96);
        }
        soc_reg_above_64_field_set(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));

        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg_val));
        *rx_req_high_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, reg_val, RX_REQ_PIPE_0_HIGH_ENf);
        SHR_BITOR_RANGE(rx_req_high_en, rx_req_low_en, 0, 32, rx_req_high_en);
        soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, &reg_val, RX_REQ_PIPE_0_HIGH_ENf, *rx_req_high_en);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_qax_port_sch_hrf_config(int unit, soc_port_t port)
{
	soc_reg_above_64_val_t reg_above_64_val, rx_req_low_en;
	uint32 offset, sch_index;
	soc_error_t rv;
	uint32 reg_val;
	SOC_TMC_PORT_HEADER_TYPE hdr_type;
	uint32 rx_req_tdm_en[1] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    sch_index =  qax_hrf_sch_index[offset];

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &hdr_type);
    SOCDNX_IF_ERR_EXIT(rv);
    if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM)
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, &reg_val));
        *rx_req_tdm_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, reg_val, RX_REQ_PIPE_0_TDM_ENf);
        shr_bitop_range_set(rx_req_low_en, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, &reg_val, RX_REQ_PIPE_0_TDM_ENf, *rx_req_tdm_en);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, reg_val));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
        soc_reg_above_64_field_get(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en);
        shr_bitop_range_set(rx_req_low_en, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        shr_bitop_range_set(rx_req_low_en, sch_index + 32, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        shr_bitop_range_set(rx_req_low_en, sch_index + 64, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        shr_bitop_range_set(rx_req_low_en, sch_index + 96, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        soc_reg_above_64_field_set(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* Set nbih trigger and wait until it reset */
int
soc_qax_wait_gtimer_trigger(int unit)
{
    int rv, counter;
    uint64 reg_val_64;
    uint32 gtimer_trigger_f;

    SOCDNX_INIT_FUNC_DEFS;

    rv = READ_NBIH_GTIMER_CONFIGURATIONr(unit, &reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_reg64_field32_set(unit, NBIH_GTIMER_CONFIGURATIONr, &reg_val_64, GTIMER_TRIGGERf, 0x0);
    rv = WRITE_NBIH_GTIMER_CONFIGURATIONr(unit, reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);

    sal_usleep(500000);

    soc_reg64_field32_set(unit, NBIH_GTIMER_CONFIGURATIONr, &reg_val_64, GTIMER_TRIGGERf, 0x1);
    rv = WRITE_NBIH_GTIMER_CONFIGURATIONr(unit, reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = READ_NBIH_GTIMER_CONFIGURATIONr(unit, &reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);
    gtimer_trigger_f = soc_reg64_field32_get(unit, NBIH_GTIMER_CONFIGURATIONr, reg_val_64, GTIMER_TRIGGERf);

    counter = 0;
    while (gtimer_trigger_f == 0x1) {
        sal_usleep(500000);

        rv = READ_NBIH_GTIMER_CONFIGURATIONr(unit, &reg_val_64);
        SOCDNX_IF_ERR_EXIT(rv);
        gtimer_trigger_f = soc_reg64_field32_get(unit, NBIH_GTIMER_CONFIGURATIONr, reg_val_64, GTIMER_TRIGGERf);

        if(10 == counter){
            SOCDNX_EXIT_WITH_ERR(SOC_E_TIMEOUT, (_BSL_SOCDNX_MSG("soc_qax_wait_gtimer_trigger timeout")));
        }

        counter++;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_nif_ilkn_pbmp_get(int unit, soc_port_t port, uint32 ilkn_id, soc_pbmp_t* phys)
{
    int i, lanes;
    char* propval;
    char* propkey;
    int first_phy, ilkn_wrap_index, ilkn_wrap_pm, lanes_to_add;
    soc_pbmp_t bm, phy_pbmp;
    portmod_pm_identifier_t *ilkn_warp;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phy_pbmp);

    propkey = spn_ILKN_LANES;
    propval = soc_property_port_get_str(unit, ilkn_id, propkey);

    ilkn_wrap_index = ilkn_id>>1; /* ILKN0/1 - WRAP0, ILKN2/3 - WRAP1 */
    ilkn_warp = qax_ilkn_pm_table[ilkn_wrap_index];

    if(propval != NULL) {

    	ilkn_wrap_pm = ( (ilkn_id & 1) == 0) ? 0 : SOC_QAX_MAX_PMS_PER_ILKN_PORT/2; /* First ILKN PM */
        first_phy = ilkn_warp[ilkn_wrap_pm].phy;

        if (_shr_pbmp_decode(propval, &bm) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("failed to decode (\"%s\") for %s"), propval, propkey));
        }
        SOC_PBMP_COUNT(bm, lanes);
        if ((ilkn_id & 1) && lanes > 12) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN%d can't have more than 12 lanes"), ilkn_id));
        }

        SOC_PBMP_ITER(bm, i) {
            SOC_PBMP_PORT_ADD(phy_pbmp, first_phy + i);
        }

    } else {

        lanes = soc_property_port_get(unit, ilkn_id, spn_ILKN_NUM_LANES, 12);

        if ((ilkn_id & 1) && lanes > 12) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN%d can't have more than 12 lanes"), ilkn_id));
        }

    	ilkn_wrap_pm = ( (ilkn_id & 1) == 0) ? 0 : SOC_QAX_MAX_PMS_PER_ILKN_PORT/2; /* First ILKN PM */

        while (lanes > 0)
        {
        	lanes_to_add = (lanes < NUM_OF_LANES_IN_PM) ?  lanes : NUM_OF_LANES_IN_PM;
        	SOC_PBMP_PORTS_RANGE_ADD(phy_pbmp, ilkn_warp[ilkn_wrap_pm].phy, lanes_to_add);

        	ilkn_wrap_pm++;
        	lanes-=lanes_to_add;
        }

    }
    SOC_PBMP_ASSIGN(*phys, phy_pbmp);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_ilkn_bypass_interface_enable(int unit, int port, int enable) {

    soc_pbmp_t phys, phy_lanes;
    soc_port_t phy;
    int shift;
    uint32 rst_nbih_lanes=0, rst_nbil0_lanes=0, rst_nbil1_lanes=0, reg_val;
    uint32 pml_base_lane    = SOC_DPP_DEFS_GET(unit, pml_base_lane);
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &phy_lanes));

	SOC_PBMP_ITER(phy_lanes, phy){

	   if (phy == 0)  /* Coverity Fix */
	       continue;

	   shift = (phy - 1) % SOC_QAX_NOF_LANES_PER_NBI;

	   if (phy <= pml_base_lane)
	   {
		   rst_nbih_lanes  |= (1 << shift);  /* phy 1-16  : NBIH */
	   }
	   else if (phy <= (pml_base_lane + SOC_QAX_NOF_LANES_PER_NBI))
	   {
		   rst_nbil0_lanes |= (1 << shift);  /* phy 17-32 : NBIL0 */
	   }
	   else if (phy <= (pml_base_lane + (2 * SOC_QAX_NOF_LANES_PER_NBI)))
	   {
		   rst_nbil1_lanes |= (1 << shift);  /* phy 33-48 : NBIL1 */
	   }
	}

	if (rst_nbih_lanes != 0)
	{
		SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, &reg_val));
		reg_val = (enable) ? (reg_val | rst_nbih_lanes) : (reg_val & (~rst_nbih_lanes));
		SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, reg_val));

		SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_TX_RSTNr(unit, &reg_val));
		reg_val = (enable) ? (reg_val | rst_nbih_lanes) : (reg_val & (~rst_nbih_lanes));
		SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_ILKN_TX_RSTNr(unit, reg_val));
	}

	if (rst_nbil0_lanes != 0)
	{
		SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 0, &reg_val));
		reg_val = (enable) ? (reg_val | rst_nbil0_lanes) : (reg_val & (~rst_nbil0_lanes));
		SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 0, reg_val));

		SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_TX_RSTNr(unit, 0, &reg_val));
		reg_val = (enable) ? (reg_val | rst_nbil0_lanes) : (reg_val & (~rst_nbil0_lanes));
		SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_ILKN_TX_RSTNr(unit, 0, reg_val));
	}

	if (rst_nbil1_lanes != 0)
	{
		SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 1, &reg_val));
		reg_val = (enable) ? (reg_val | rst_nbil1_lanes) : (reg_val & (~rst_nbil1_lanes));
		SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 1, reg_val));

		SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_TX_RSTNr(unit, 1, &reg_val));
		reg_val = (enable) ? (reg_val | rst_nbil1_lanes) : (reg_val & (~rst_nbil1_lanes));
		SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_ILKN_TX_RSTNr(unit, 1, reg_val));
	}


exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_nif_qsgmii_pbmp_get(int unit, soc_port_t port, uint32 id, soc_pbmp_t *phy_pbmp)
{
    int first_phy;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*phy_pbmp);
    if(id < 48) {
        first_phy = id  + 17;
    } else {
        first_phy = id - 48 + 69;
    }

    SOC_PBMP_PORT_ADD(*phy_pbmp, first_phy);

    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
