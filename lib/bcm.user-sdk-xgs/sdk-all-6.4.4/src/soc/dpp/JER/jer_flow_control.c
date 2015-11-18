#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
/* $Id: arad_flow_control.c,v 1.59 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_FLOWCONTROL

/*************
 * INCLUDES  *
 *************/
#include <soc/mem.h>
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_flow_control.h>
#include <soc/dpp/JER/jer_flow_control.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_db.h>
#include <soc/dpp/ARAD/arad_pmf_pgm_mgmt.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/portmod/portmod.h>
#include <soc/dpp/JER/jer_nif.h>

#include <soc/mcm/allenum.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define JER_FC_REC_CAL_DEST_ARR_SIZE   7
#define JER_FC_GEN_CAL_SRC_ARR_SIZE    8


/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SET_FC_ENABLE(__FC_ENA, __FIELD) {                                            \
    if (SHR_BITGET(cfc_enables->bmp, __FC_ENA)) {                                     \
        if (soc_reg_field_valid(unit, reg, __FIELD)) {                                \
            field_value = (SHR_BITGET(ena_info->bmp, __FC_ENA))? 1: 0;                \
            soc_reg_above_64_field32_set(unit, reg, reg_data, __FIELD, field_value);  \
        }                                                                             \
    }                                                                                 \
}

#define GET_FC_ENABLE(__FC_ENA, __FIELD) {                                            \
    if (SHR_BITGET(cfc_enables->bmp, __FC_ENA)) {                                     \
        if (soc_reg_field_valid(unit, reg, __FIELD)) {                                \
            field_value = soc_reg_above_64_field32_get(unit, reg, reg_data, __FIELD); \
            if (field_value) {                                                        \
                SHR_BITSET(ena_info->bmp, __FC_ENA);                                  \
            }                                                                         \
        }                                                                             \
        else {                                                                        \
            SHR_BITCLR(ena_info->bmp, __FC_ENA);                                      \
        }                                                                             \
    }                                                                                 \
}

#define FC_ENABLE_NOT_SUPPORT_JER {     \
    SOC_TMC_FC_ILKN_RX_TO_RET_REQ_EN,   \
    SOC_TMC_FC_SPI_OOB_RX_TO_RET_REQ_EN \
}

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_TMC_FC_GEN_CAL_SRC 
  jer_fc_gen_cal_src_arr[JER_FC_GEN_CAL_SRC_ARR_SIZE] = 
    { SOC_TMC_FC_GEN_CAL_SRC_STE, 
      SOC_TMC_FC_GEN_CAL_SRC_STTSTCS_TAG,
      SOC_TMC_FC_GEN_CAL_SRC_LLFC_VSQ,
      SOC_TMC_FC_GEN_CAL_SRC_PFC_VSQ,
      SOC_TMC_FC_GEN_CAL_SRC_GLB_RCS,
      SOC_TMC_FC_GEN_CAL_SRC_HCFC,
      SOC_TMC_FC_GEN_CAL_SRC_LLFC,
      SOC_TMC_FC_GEN_CAL_SRC_CONST
    };

static SOC_TMC_FC_REC_CAL_DEST 
  jer_fc_rec_cal_dest_arr[JER_FC_REC_CAL_DEST_ARR_SIZE] = 
    { SOC_TMC_FC_REC_CAL_DEST_PFC, 
      SOC_TMC_FC_REC_CAL_DEST_NIF_LL,
      SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY,
      SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY,
      SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC,
      SOC_TMC_FC_REC_CAL_DEST_RETRANSMIT,
      SOC_TMC_FC_REC_CAL_DEST_NONE
    };

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


static uint32
jer_fc_cal_tbl_get(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN SOC_TMC_FC_CAL_MODE           cal_mode,
    SOC_SAND_IN ARAD_FC_CAL_TYPE              cal_type,
    SOC_SAND_IN SOC_TMC_FC_CAL_IF_ID          if_ndx,
    SOC_SAND_OUT soc_mem_t                    *cal_table
  )
{
    soc_mem_t cal_inb_rx_tables[] = {
        CFC_ILKN_INB_RX_0_CALm, CFC_ILKN_INB_RX_1_CALm, CFC_ILKN_INB_RX_2_CALm,
        CFC_ILKN_INB_RX_3_CALm, CFC_ILKN_INB_RX_4_CALm, CFC_ILKN_INB_RX_5_CALm};
    soc_mem_t cal_inb_tx_tables[] = {
        CFC_ILKN_INB_TX_0_CALm, CFC_ILKN_INB_TX_1_CALm, CFC_ILKN_INB_TX_2_CALm,
        CFC_ILKN_INB_TX_3_CALm, CFC_ILKN_INB_TX_4_CALm, CFC_ILKN_INB_TX_5_CALm};
    soc_mem_t 
	    cal_table_tmp = INVALIDm;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cal_table);
    if (cal_mode == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        if (if_ndx > SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn ndx is out of range")));
        }
    }
    else {
        if (if_ndx > SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("oob ndx is out of range")));
        }        
    }

    if (cal_type == ARAD_FC_CAL_TYPE_RX){
        if(cal_mode == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
            cal_table_tmp = cal_inb_rx_tables[if_ndx];
        }
        else {
            cal_table_tmp = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_OOB_RX_0_CALm : CFC_OOB_RX_1_CALm);
        }
    }
    else {
        if(cal_mode == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
            cal_table_tmp = cal_inb_tx_tables[if_ndx];
        }
        else {
            cal_table_tmp = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_OOB_TX_0_CALm : CFC_OOB_TX_1_CALm);
        }
    }
	
	(*cal_table) = cal_table_tmp;

exit:
    SOCDNX_FUNC_RETURN;    
}


/*********************************************************************
* NAME:
*     jer_flow_control_regs_init
* FUNCTION:
*   Initialization of the Arad blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
STATIC uint32
  jer_flow_control_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    SOC_TMC_FC_ILKN_MUB_GEN_CAL
        ilkn_cal;
    uint32
        i,
        ilkn_ndx,
        oob_ndx;
    SOC_TMC_FC_ENABLE_BITMAP
        cfc_enables; 

    SOCDNX_INIT_FUNC_DEFS;

    /* CFC Enablers */
    SHR_BITCLR_RANGE(cfc_enables.bmp, 0, SOC_TMC_FC_NOF_ENABLEs);

    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_EGQ_TO_SCH_DEVICE_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_EGQ_TO_SCH_ERP_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_EGQ_TO_SCH_ERP_TC_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_EGQ_TO_SCH_IF_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_EGQ_TO_SCH_PFC_EN);

    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_EGQ_RCL_PFC_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_HCFC_HP_CFG);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_HCFC_LP_CFG);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_HP_CFG);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_LP_CFG);

    rv = jer_fc_enables_set(unit, &cfc_enables, &cfc_enables);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Clear Calendars  - ILKN */
    for (ilkn_ndx = 0; ilkn_ndx < SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports; ilkn_ndx++) {
        rv = jer_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_ILKN_INBND, ARAD_FC_CAL_TYPE_RX, ilkn_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = jer_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_ILKN_INBND, ARAD_FC_CAL_TYPE_TX, ilkn_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* Clear Calendars  - SPI OOB */
    for (oob_ndx = 0; oob_ndx < SOC_DPP_CONFIG(unit)->tm.max_oob_ports; oob_ndx++) {
        rv = jer_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_SPI_OOB, ARAD_FC_CAL_TYPE_RX, oob_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = jer_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_SPI_OOB, ARAD_FC_CAL_TYPE_TX, oob_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* Init the ILKN MUB TX calendar */
    for (ilkn_ndx = 0; ilkn_ndx < SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports; ilkn_ndx++) {
        for(i = 0; i < SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN; i++) {
            ilkn_cal.entries[i].source = SOC_TMC_FC_GEN_CAL_SRC_CONST;
            ilkn_cal.entries[i].id = 0;
        }
        rv = jer_fc_ilkn_mub_gen_cal_set(unit, ilkn_ndx, &ilkn_cal);
        SOCDNX_IF_ERR_EXIT(rv);
    }


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_init_pfc_mapping(
      SOC_SAND_IN int  unit
    )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        nif_pfc_data[3],
        *data_select,
        entry_offset,
        field_offset,
        base_queue_pair,
        nof_priorities,
        i,
        interface_i,
        valid;
    soc_port_if_t
        interface_type;
    uint32
        index_field[] = 
            {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f, INDEX_4f, INDEX_5f, INDEX_6f, INDEX_7f};
    uint32
        select_field[] = 
            {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f, SELECT_4f, SELECT_5f, SELECT_6f, SELECT_7f};
    uint32
        valid_field[] = 
            {VALID_0f, VALID_1f, VALID_2f, VALID_3f, VALID_4f, VALID_5f, VALID_6f, VALID_7f};
    uint32
        phy_port;
    soc_pbmp_t
        pbmp;
    soc_port_t
        logical_port;
    uint32
        tm_port;
    int 
        core;

    SOCDNX_INIT_FUNC_DEFS;

    /* Default PFC/TC mapping */
    rv = soc_port_sw_db_valid_ports_get(unit, 0, &pbmp);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_PBMP_ITER(pbmp, logical_port)
    {
        rv = soc_port_sw_db_local_to_tm_port_get(unit, logical_port, &tm_port, &core);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_queue_pair);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_port_sw_db_interface_type_get(unit, logical_port, &interface_type);        
        SOCDNX_IF_ERR_EXIT(rv);

        if (interface_type == SOC_PORT_IF_NULL || interface_type == SOC_PORT_IF_CPU ||
            interface_type == SOC_PORT_IF_OLP || interface_type == SOC_PORT_IF_RCY ||
            interface_type == SOC_PORT_IF_ERP || interface_type == SOC_PORT_IF_OAMP) {
            continue;
        }

        rv = soc_port_sw_db_first_phy_port_get(unit, logical_port, &phy_port);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_jer_qsgmii_offsets_remove(unit, phy_port, &phy_port);
        SOCDNX_IF_ERR_EXIT(rv);

        interface_i = phy_port - 1;

        valid = 0x1;

        entry_offset = interface_i;
        if(entry_offset >= SOC_MEM_SIZE(unit, CFC_NIF_PFC_MAPm)) {
            /* Protection against miss-configured interfaces */
            continue;
        }
            
        rv = soc_mem_read(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
        SOCDNX_IF_ERR_EXIT(rv);

        for(i = 0; i < nof_priorities; i++) {
            field_offset = i;
            data_select = nif_pfc_data; 
      
            soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, data_select, index_field[field_offset], base_queue_pair + i);
            soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, data_select, select_field[field_offset], 0);
            soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, data_select, valid_field[field_offset], valid);
        }

        rv = soc_mem_write(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
        SOCDNX_IF_ERR_EXIT(rv);
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_flow_control_init_pp_rx_calendar(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        interface_number,
        fld_val,
        reg32_data = 0;
    soc_reg_t
	    reg = INVALIDr;
    soc_field_t
        fld = INVALIDf;
    soc_field_t 
        inb_sel_fld[] = 
            {ILKN_INB_RX_0_SELf, ILKN_INB_RX_1_SELf, ILKN_INB_RX_2_SELf, 
             ILKN_INB_RX_3_SELf, ILKN_INB_RX_4_SELf, ILKN_INB_RX_5_SELf},
        inb_en_fld[] = 
            {ILKN_INB_RX_0_ENf, ILKN_INB_RX_1_ENf, ILKN_INB_RX_2_ENf, 
             ILKN_INB_RX_3_ENf, ILKN_INB_RX_4_ENf, ILKN_INB_RX_5_ENf};
    uint32
        pp_intf_id;
    int 
        is_pp_enable[SOC_TMC_FC_NOF_PP_INTF] = {0},
        is_coe_enable[SOC_TMC_FC_NOF_PP_INTF] = {0},
        calendar_len[SOC_TMC_FC_NOF_PP_INTF] = {0};
    int
        coe_tick = 0;
    soc_dpp_tm_config_t  
        *tm;

    SOCDNX_INIT_FUNC_DEFS;

    tm = &(SOC_DPP_CONFIG(unit)->tm);

    /* Only one fc calendar interface is needed to map for PP calendar.
     * The connection between fc calendar interface and PP RX calendar is fixed. 
     *   PP0:  OOB0, InB0, InB2, InB4
     *   PP1:  OOB1, InB1, InB3, InB5
     */

    /* 1. Select PP as ILKN OOB source { */
    for(interface_number = 0; interface_number < tm->max_oob_ports; interface_number++){
        if((tm->fc_oob_type[interface_number] != SOC_TMC_FC_CAL_TYPE_COE) &&
           (tm->fc_oob_type[interface_number] != SOC_TMC_FC_CAL_TYPE_E2E))
            continue;

        pp_intf_id = interface_number % 2;
        is_pp_enable[pp_intf_id] = 1;
        calendar_len[pp_intf_id] = tm->fc_oob_calender_length[interface_number][SOC_TMC_CONNECTION_DIRECTION_RX];
        if (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_COE) {
            is_coe_enable[pp_intf_id] = 1;
        }

        /* Put the OOB interfaces into Reset */
        fld_val = 0x0;
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_RX_0_RSTNf : OOB_RX_1_RSTNf);
        rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
        SOCDNX_IF_ERR_EXIT(rv);

        /* Disable the FC before configuring it */
        fld_val = 0x0;
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_RX_0_ENf : ILKN_OOB_RX_1_ENf); 
        rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
        SOCDNX_IF_ERR_EXIT(rv);

        /* Select PP as ILKN OOB source { */
        fld_val = 1;
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_RX_0_SELf : ILKN_OOB_RX_1_SELf);
        rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
        SOCDNX_IF_ERR_EXIT(rv);
        /* Select PP as ILKN OOB source } */

        /* Take the OOB/ILKN interfaces out of Reset */
        fld_val = 0x1;
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_RX_0_RSTNf : OOB_RX_1_RSTNf);
        rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
        SOCDNX_IF_ERR_EXIT(rv);

        /* Enable the FC after configuring it */
        fld_val = 0x1;
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_RX_0_ENf : ILKN_OOB_RX_1_ENf); 
        rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    /* 1. Select PP as ILKN OOB source } */

    /* 2. Select PP as ILKN InB source { */
    for(interface_number = 0; interface_number < tm->max_interlaken_ports; interface_number++){
        if(tm->fc_inband_intlkn_type[interface_number] != SOC_TMC_FC_CAL_INB_TYPE_COE &&
           tm->fc_inband_intlkn_type[interface_number] != SOC_TMC_FC_CAL_INB_TYPE_E2E) {
            continue;
        }

        pp_intf_id = interface_number % 2;
        is_pp_enable[pp_intf_id] = 1;
        calendar_len[pp_intf_id] = tm->fc_inband_intlkn_calender_length[interface_number][SOC_TMC_CONNECTION_DIRECTION_RX];
        if (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_INB_TYPE_COE) {
            is_coe_enable[pp_intf_id] = 1;
        }

        /* Disable the FC before configuring it */
        fld_val = 0x0;
        fld = inb_en_fld[interface_number];
        rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
        SOCDNX_IF_ERR_EXIT(rv);

        /* Select PP as ILKN INB source { */
        fld_val = 1;
        fld = inb_sel_fld[interface_number];
        rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
        SOCDNX_IF_ERR_EXIT(rv);
        /* Select PP as ILKN INB source { */

        /* Enable the FC after configuring it */
        fld_val = 0x1;
        fld = inb_en_fld[interface_number];
        rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
        SOCDNX_IF_ERR_EXIT(rv);

    }
    /* 2. Select PP as ILKN InB source } */

    /* 3. Configure PP Rx calendar parameters { */
    for (pp_intf_id = 0; pp_intf_id < SOC_TMC_FC_NOF_PP_INTF; pp_intf_id++) {
        if (is_pp_enable[pp_intf_id]) {
            reg = (pp_intf_id == 0) ? CFC_IHB_0_PP_INBANDr : CFC_IHB_1_PP_INBANDr;

            /* Enable PP inband FC(both modes: COE and E2EFC)*/
            fld_val = 1;
            fld = (pp_intf_id == 0) ? IHB_0_PP_INB_ENBf : IHB_1_PP_INB_ENBf;
            soc_reg_field_set(unit, reg, &reg32_data, fld, fld_val);

            /* Setting PP inband Rx calendar length */
            fld_val = calendar_len[pp_intf_id] - 1;
            fld = (pp_intf_id == 0) ? IHB_0_PP_INB_CAL_LENf : IHB_1_PP_INB_CAL_LENf;
            soc_reg_field_set(unit, reg, &reg32_data, fld, fld_val);

            /* Setting PP inband calendar mode */
            fld_val = is_coe_enable[pp_intf_id] ? 0 : 1;
            fld = (pp_intf_id == 0) ? IHB_0_PP_MODEf : IHB_1_PP_MODEf;
            soc_reg_field_set(unit, reg, &reg32_data, fld, fld_val);

            if (is_coe_enable[pp_intf_id]) {
                /* Setting PP COE sub-mode: 0: Pause, 1: PFC */
                fld_val = tm->fc_calendar_coe_mode;
                fld = (pp_intf_id == 0) ? IHB_0_PP_COE_MODEf : IHB_1_PP_COE_MODEf;
                soc_reg_field_set(unit, reg, &reg32_data, fld, fld_val);

                if (tm->fc_calendar_coe_mode == SOC_TMC_FC_COE_MODE_PAUSE) {
                    /* Setting PP COE Pause counter rate - Configuration (per block) number of clocks 
                       between two successive decrements of each counter */
                    coe_tick = tm->fc_calendar_pause_resolution * SOC_INFO(unit).frequency;
                    fld_val = coe_tick - 1;
                    fld = (pp_intf_id == 0) ? IHB_0_PP_COE_TICKf : IHB_1_PP_COE_TICKf;
                    soc_reg_field_set(unit, reg, &reg32_data, fld, fld_val);                   
				}
            }
            else {
                /* Setting PP E2E FC status size */
                fld_val = tm->fc_calendar_e2e_status_of_entries;
                fld = (pp_intf_id == 0) ? IHB_0_PP_E_2_EFC_SIZEf : IHB_1_PP_E_2_EFC_SIZEf;
                soc_reg_field_set(unit, reg, &reg32_data, fld, fld_val);

                /* Setting PP E2E FC polarity */
                fld_val = tm->fc_calendar_indication_invert;
                fld = (pp_intf_id == 0) ? IHB_0_PP_E_2_EFC_POLf : IHB_1_PP_E_2_EFC_POLf;
                soc_reg_field_set(unit, reg, &reg32_data, fld, fld_val);                
            }

            rv = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg32_data);
            SOCDNX_IF_ERR_EXIT(rv);
        }        
    }
    /* 3. Configure PP Rx calendar parameters } */

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  jer_flow_control_init_oob_rx(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        interface_number,
        fld_val,
        fld;
    uint32
        per_rep_idx,
        rep_idx,
        entry_idx,
        cal_length = 0,
        cal_reps = 0;
    uint64
        field64;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t
        cal_table_data;
    SOC_TMC_FC_CAL_MODE
        cal_mode_ndx;
    soc_dpp_tm_config_t  *tm;

    SOCDNX_INIT_FUNC_DEFS;
 
    SOC_REG_ABOVE_64_CLEAR(cal_table_data);
    tm = &(SOC_DPP_CONFIG(unit)->tm);

    for(interface_number = 0; interface_number < SOC_TMC_FC_NOF_OOB_IDS; interface_number++)
    {
        if(tm->fc_oob_type[interface_number] != SOC_TMC_FC_CAL_TYPE_ILKN &&
           tm->fc_oob_type[interface_number] != SOC_TMC_FC_CAL_TYPE_SPI &&
           tm->fc_oob_type[interface_number] != SOC_TMC_FC_CAL_TYPE_HCFC) {
            continue;
        }

        /* Handles RX */
        if(tm->fc_oob_mode[interface_number] & SOC_DPP_FC_CAL_MODE_RX_ENABLE)
        {
            /* Put the OOB interfaces into Reset */
            fld_val = 0x0;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_RX_0_RSTNf : OOB_RX_1_RSTNf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            /* Disable the FC before configuring it */
            fld_val = 0x0;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_RX_0_ENf : ILKN_OOB_RX_1_ENf); 
            rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf);
            COMPILER_64_SET(field64,0,fld_val);
            rv = soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64);
            SOCDNX_IF_ERR_EXIT(rv);

            /* Configuration start { */

            /*Disable flow control on port fault*/

            /*  Configure OOB Type: ILKN/SPI/HCFC */
            fld_val = (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 1 : 0);
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_RX_0_MODEf : OOB_RX_1_MODEf);                            
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            if(tm->fc_oob_type[interface_number] != SOC_TMC_FC_CAL_TYPE_ILKN) {
                fld_val = (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_HCFC ? 1 : 0);
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_0_OOB_MODEf : SPI_1_OOB_MODEf);                                            
                rv = soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_CONFIGURATIONr, REG_PORT_ANY, 0, SPI_0_OOB_MODEf,  fld_val);
                SOCDNX_IF_ERR_EXIT(rv);
            }

            /* Calendar length (single repetition) */
            cal_length = tm->fc_oob_calender_length[interface_number][SOC_TMC_CONNECTION_DIRECTION_RX];
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN)
            {
                fld = (interface_number == ARAD_FC_ILKN_ID_A ? ILKN_OOB_RX_0_CAL_LENf : ILKN_OOB_RX_1_CAL_LENf);
                rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  cal_length - 1);
                SOCDNX_IF_ERR_EXIT(rv);
            }
            else
            {
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_LENf : SPI_OOB_RX_1_CAL_LENf);
                COMPILER_64_SET(field64, 0, cal_length);
                rv = soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64);
                SOCDNX_IF_ERR_EXIT(rv);
            } 

            /*  Calendar number of repetitions - SPI/OOB Only */
            cal_reps = tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 1 : 
                       tm->fc_oob_calender_rep_count[interface_number][SOC_TMC_CONNECTION_DIRECTION_RX];
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_Mf : SPI_OOB_RX_1_CAL_Mf);
            COMPILER_64_SET(field64,0,cal_reps);
            rv = soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64);
            SOCDNX_IF_ERR_EXIT(rv);

            /* Clear Calendar entry */
            cal_mode_ndx = (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN) ? SOC_TMC_FC_CAL_MODE_ILKN_OOB : SOC_TMC_FC_CAL_MODE_SPI_OOB;
            rv = jer_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_RX, interface_number, &cal_table);
            SOCDNX_IF_ERR_EXIT(rv);
            for (per_rep_idx = 0; per_rep_idx < cal_length; per_rep_idx++) {
                soc_mem_field32_set(unit, cal_table, cal_table_data, FC_INDEXf, 0);
                soc_mem_field32_set(unit, cal_table, cal_table_data, FC_DST_SELf, jer_fc_rec_cal_dest_type_to_val_internal(SOC_TMC_FC_REC_CAL_DEST_NONE));

                for (rep_idx = 0; rep_idx < cal_reps; rep_idx++) {
                    entry_idx = (rep_idx * cal_length) + per_rep_idx;
                    rv = soc_mem_write(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
                    SOCDNX_IF_ERR_EXIT(rv);
                }
            }

            /* Configuration end } */

            /* Take the OOB/ILKN interfaces out of Reset */
            fld_val = 0x1;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_RX_0_RSTNf : OOB_RX_1_RSTNf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            /* Enable the FC after configuring it */
            fld_val = 0x1;
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN) {
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_RX_0_ENf : ILKN_OOB_RX_1_ENf); 
                rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
                SOCDNX_IF_ERR_EXIT(rv);
            }
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_SPI || tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_HCFC) {
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf);
                COMPILER_64_SET(field64,0,fld_val);
                rv = soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_flow_control_init_oob_tx(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        interface_number,
        fld_val,
        fld;
    uint32
        per_rep_idx,
        rep_idx,
        entry_idx,
        cal_length = 0,
        cal_reps = 0;
    uint64
        field64;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t
        cal_table_data;
    SOC_TMC_FC_CAL_MODE
        cal_mode_ndx;
    soc_dpp_tm_config_t  *tm;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(cal_table_data);
    tm = &(SOC_DPP_CONFIG(unit)->tm);
  
    for(interface_number = 0; interface_number < SOC_TMC_FC_NOF_OOB_IDS; interface_number++)
    {
        /* OOB TX CLK Speed */
        rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_TX_0_OUT_SPEEDf, tm->fc_oob_tx_speed[interface_number]);
        SOCDNX_IF_ERR_EXIT(rv);

        if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_NONE)
            continue;

        /* Handles TX */
        if(tm->fc_oob_mode[interface_number] & SOC_DPP_FC_CAL_MODE_TX_ENABLE)
        {
            /* Put the OOB interfaces into Reset */
            fld_val = 0x0;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_TX_0_RSTNf : OOB_TX_1_RSTNf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            /* Disable the FC before configuring it */
            fld_val = 0x0;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_TX_0_ENf : ILKN_OOB_TX_1_ENf); 
            COMPILER_64_SET(field64,0,fld_val);
            rv = soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  field64);
            SOCDNX_IF_ERR_EXIT(rv);
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_ENf : SPI_OOB_TX_1_ENf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            /* Configuration start */

            /* Configure OOB Type: ILKN/SPI/HCFC */
            fld_val = (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 1 : 0);
            COMPILER_64_SET(field64,0,fld_val);
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_TX_0_MODEf : OOB_TX_1_MODEf);        
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_TX_0_MODEf,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            /* Calendar length (single repetition) */
            cal_length = tm->fc_oob_calender_length[interface_number][SOC_TMC_CONNECTION_DIRECTION_TX];
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN) {
                fld = (interface_number == ARAD_FC_ILKN_ID_A ? ILKN_OOB_TX_0_CAL_LENf : ILKN_OOB_TX_1_CAL_LENf);
                COMPILER_64_SET(field64,0,cal_length - 1);
                rv = soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  field64);
                SOCDNX_IF_ERR_EXIT(rv);
            }
            else
            { 
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_LENf : SPI_OOB_TX_1_CAL_LENf);
                rv = soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  cal_length);
                SOCDNX_IF_ERR_EXIT(rv);
            } 

            /*  Calendar number of repetitions - SPI/OOB Only */
            cal_reps = tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 
                           1 : 
                           tm->fc_oob_calender_rep_count[interface_number][SOC_TMC_CONNECTION_DIRECTION_TX];
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_Mf : SPI_OOB_TX_1_CAL_Mf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  cal_reps);
            SOCDNX_IF_ERR_EXIT(rv);

            /* Clear Calendar entry */
            cal_mode_ndx = (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN) ? SOC_TMC_FC_CAL_MODE_ILKN_OOB : SOC_TMC_FC_CAL_MODE_SPI_OOB;
            rv = jer_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_TX, interface_number, &cal_table);
            SOCDNX_IF_ERR_EXIT(rv);
            for (per_rep_idx = 0; per_rep_idx < cal_length; per_rep_idx++) {
                soc_mem_field32_set(unit, cal_table, cal_table_data, FC_INDEXf, 0);
                soc_mem_field32_set(unit, cal_table, cal_table_data, FC_SRC_SELf, jer_fc_gen_cal_src_type_to_val_internal(SOC_TMC_FC_GEN_CAL_SRC_CONST));

                for (rep_idx = 0; rep_idx < cal_reps; rep_idx++) {
                    entry_idx = (rep_idx * cal_length) + per_rep_idx;

                    rv = soc_mem_write(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
                    SOCDNX_IF_ERR_EXIT(rv);
                }
            }

            /* Configuration end */

            /* Take the OOB interface out of Reset */
            fld_val = 0x1;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_TX_0_RSTNf : OOB_TX_1_RSTNf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            /* Enable the FC after configuring it */
            fld_val = 0x1;
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN) {
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_TX_0_ENf : ILKN_OOB_TX_1_ENf); 
                COMPILER_64_SET(field64,0,fld_val);
                rv = soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  field64);
                SOCDNX_IF_ERR_EXIT(rv);
            }
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_SPI || tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_HCFC) {
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_ENf : SPI_OOB_TX_1_ENf);
                rv = soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
* NAME:
*     arad_flow_control_init
* FUNCTION:
*     Initialization of the Arad blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
soc_error_t
  jer_flow_control_init(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    rv = jer_flow_control_regs_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = jer_flow_control_init_oob_rx(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = jer_flow_control_init_oob_tx(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = jer_flow_control_init_pp_rx_calendar(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_enables_verify(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *cfc_enables,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *ena_info
    )
{
    int i = 0;
    SOC_TMC_FC_ENABLE fc_enable_not_support[] = FC_ENABLE_NOT_SUPPORT_JER;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(cfc_enables);
    SOCDNX_NULL_CHECK(ena_info);

    for (i = 0; i < sizeof(fc_enable_not_support)/sizeof(SOC_TMC_FC_ENABLE); i++) {
        if (SHR_BITGET(cfc_enables->bmp, fc_enable_not_support[i])) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("cfc_enables[%d] is not supported by this chip"), i));
        }
    }  

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
   jer_fc_enables_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *cfc_enables,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *ena_info
    )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        reg_data;
    soc_reg_t 
        reg = CFC_CFC_ENABLERSr;
    uint32 
        field_value = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = jer_fc_enables_verify(unit, cfc_enables, ena_info);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(rv);

    SET_FC_ENABLE(SOC_TMC_FC_EN, CFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_NIF_CNM_LLFC_EN, EGQ_TO_NIF_CNM_LLFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_NIF_CNM_PFC_EN, EGQ_TO_NIF_CNM_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_DEVICE_EN, EGQ_TO_SCH_DEVICE_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_ERP_EN, EGQ_TO_SCH_ERP_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_ERP_TC_EN, EGQ_TO_SCH_ERP_TC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_IF_EN, EGQ_TO_SCH_IF_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_PFC_EN, EGQ_TO_SCH_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_EGQ_RCL_PFC_EN, GLB_RSC_TO_EGQ_RCL_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_HCFC_HP_CFG, GLB_RSC_TO_HCFC_HP_CFGf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_HCFC_LP_CFG, GLB_RSC_TO_HCFC_LP_CFGf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_NIF_LLFC_EN, GLB_RSC_TO_NIF_LLFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_NIF_PFC_EN, GLB_RSC_TO_NIF_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_HP_CFG, GLB_RSC_TO_RCL_PFC_HP_CFGf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_LP_CFG, GLB_RSC_TO_RCL_PFC_LP_CFGf);
    SET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_EGQ_PFC_EN, ILKN_RX_TO_EGQ_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_EGQ_PORT_EN, ILKN_RX_TO_EGQ_PORT_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_GEN_PFC_EN, ILKN_RX_TO_GEN_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_NIF_FAST_LLFC_EN, ILKN_RX_TO_NIF_FAST_LLFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN, LLFC_VSQ_TO_NIF_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_NIF_TO_GEN_PFC_EN, NIF_TO_GEN_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_PFC_VSQ_TO_NIF_EN, PFC_VSQ_TO_NIF_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SCH_OOB_RX_EN, SCH_OOB_RX_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PFC_EN, SPI_OOB_RX_TO_EGQ_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PORT_EN, SPI_OOB_RX_TO_EGQ_PORT_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_GEN_PFC_EN, SPI_OOB_RX_TO_GEN_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_NIF_FAST_LLFC_EN, SPI_OOB_RX_TO_NIF_FAST_LLFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_STAT_VSQ_TO_HCFC_EN, STAT_VSQ_TO_HCFC_ENf);

    rv = soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(rv);
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
   jer_fc_enables_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *cfc_enables,
      SOC_SAND_OUT SOC_TMC_FC_ENABLE_BITMAP     *ena_info
    )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t 
        reg_data;
    soc_reg_t 
        reg = CFC_CFC_ENABLERSr;
    uint32 
        field_value = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = jer_fc_enables_verify(unit, cfc_enables, ena_info);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(rv);

    GET_FC_ENABLE(SOC_TMC_FC_EN, CFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_NIF_CNM_LLFC_EN, EGQ_TO_NIF_CNM_LLFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_NIF_CNM_PFC_EN, EGQ_TO_NIF_CNM_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_DEVICE_EN, EGQ_TO_SCH_DEVICE_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_ERP_EN, EGQ_TO_SCH_ERP_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_ERP_TC_EN, EGQ_TO_SCH_ERP_TC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_IF_EN, EGQ_TO_SCH_IF_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_PFC_EN, EGQ_TO_SCH_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_EGQ_RCL_PFC_EN, GLB_RSC_TO_EGQ_RCL_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_HCFC_HP_CFG, GLB_RSC_TO_HCFC_HP_CFGf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_HCFC_LP_CFG, GLB_RSC_TO_HCFC_LP_CFGf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_NIF_LLFC_EN, GLB_RSC_TO_NIF_LLFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_NIF_PFC_EN, GLB_RSC_TO_NIF_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_HP_CFG, GLB_RSC_TO_RCL_PFC_HP_CFGf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_LP_CFG, GLB_RSC_TO_RCL_PFC_LP_CFGf);
    GET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_EGQ_PFC_EN, ILKN_RX_TO_EGQ_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_EGQ_PORT_EN, ILKN_RX_TO_EGQ_PORT_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_GEN_PFC_EN, ILKN_RX_TO_GEN_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_NIF_FAST_LLFC_EN, ILKN_RX_TO_NIF_FAST_LLFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN, LLFC_VSQ_TO_NIF_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_NIF_TO_GEN_PFC_EN, NIF_TO_GEN_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_PFC_VSQ_TO_NIF_EN, PFC_VSQ_TO_NIF_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SCH_OOB_RX_EN, SCH_OOB_RX_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PFC_EN, SPI_OOB_RX_TO_EGQ_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PORT_EN, SPI_OOB_RX_TO_EGQ_PORT_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_GEN_PFC_EN, SPI_OOB_RX_TO_GEN_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_NIF_FAST_LLFC_EN, SPI_OOB_RX_TO_NIF_FAST_LLFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_STAT_VSQ_TO_HCFC_EN, STAT_VSQ_TO_HCFC_ENf);
  
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  jer_fc_gen_inbnd_pfc_set_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  soc_port_t                    port,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_PFC      *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        offset,
        reg_offset,
        bit_offset, 
        fld_offset;
    uint32
        base_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        data;
    int
        core;
    soc_field_t 
        rx_mlf_pfc_fld[4] = {
            FC_TX_N_PORT_0_GEN_PFC_FROM_MLF_MASKf,
            FC_TX_N_PORT_1_GEN_PFC_FROM_MLF_MASKf, 
            FC_TX_N_PORT_2_GEN_PFC_FROM_MLF_MASKf, 
            FC_TX_N_PORT_3_GEN_PFC_FROM_MLF_MASKf};
    uint32
        pfc_map_rx_mlf_reg;
    soc_mem_t 
        mem = INVALIDm;
    soc_reg_above_64_val_t
        map_data;
    uint32
        entry_offset = 0; 
    soc_reg_above_64_val_t 
        cat2tc_map_ena;
    soc_reg_t 
        cat2tc_map_ena_reg = INVALIDr;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_port_sw_db_core_get(unit, port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_first_phy_port_get(unit, port, &offset /*one based*/);
    SOCDNX_IF_ERR_EXIT(rv);
    offset--;

    rv = soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_egq_ppct_tbl_get_unsafe(
              unit,
              core,
              base_q_pair,
              &data
              );
    SOCDNX_IF_ERR_EXIT(rv);

    data.cnm_intrcpt_fc_en = SOC_SAND_BOOL2NUM(info->cnm_intercept_enable);
    data.cnm_intrcpt_fc_vec_pfc = info->cnm_pfc_channel;
    rv = arad_egq_ppct_tbl_set_unsafe(
          unit,
          core,
          base_q_pair,
          &data
          );
    SOCDNX_IF_ERR_EXIT(rv);


    /* Set the bit in the GLB RSC PFC bitmap that will be sent to the NIF (NIF_PORT x TC) { */
    if ((info->gen_src == SOC_TMC_FC_GEN_SRC_GLB_LOW) || (info->gen_src == SOC_TMC_FC_GEN_SRC_GLB_HIGH)) {
        mem = (core == 0) ? CFC_GLB_RSC_IQM_0_MAPm : CFC_GLB_RSC_IQM_1_MAPm;
        entry_offset = ((info->is_ocb_only == 1)?(4):(0)) + 
                       ((info->glbl_rcs_pool == 1)?(2):(0)) + 
                       ((info->gen_src == SOC_TMC_FC_GEN_SRC_GLB_HIGH)?(1):(0));

        /* set mapping from CAT2+TC to NIF PFC */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
        SOCDNX_IF_ERR_EXIT(rv);

        if (info->gen_src == SOC_TMC_FC_GEN_SRC_GLB_HIGH) {
            SHR_BITCOPY_RANGE(map_data, (offset*8), &(info->glbl_rcs_high), 0, 8);
        }
        else {
            SHR_BITCOPY_RANGE(map_data, (offset*8), &(info->glbl_rcs_low), 0, 8);
        }

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    /* Set the bit in the GLB RSC PFC bitmap that will be sent to the NIF (NIF_PORT x TC) } */

    /* Set the bit in the MLF PFC bitmap { */
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_NIF) {
        reg_offset = offset / ARAD_MAX_NIFS_PER_WC;
        fld_offset = offset % ARAD_MAX_NIFS_PER_WC;

        rv = READ_NBI_FC_PFC_MLF_MASKr(unit, reg_offset, &pfc_map_rx_mlf_reg);
        SOCDNX_IF_ERR_EXIT(rv);
  
        soc_reg_field_set(unit, NBI_FC_PFC_MLF_MASKr, &pfc_map_rx_mlf_reg, rx_mlf_pfc_fld[fld_offset], info->nif_cls_bitmap);

        rv = WRITE_NBI_FC_PFC_MLF_MASKr(unit, reg_offset, pfc_map_rx_mlf_reg);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    /* Set the bit in the MLF PFC bitmap } */

    /* Set the bit in the CAT2+TC PFC bitmap that will be sent to the NIF (NIF_PORT x TC) { */
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_VSQ_CAT2TC) {
        mem = (core == 0) ? CFC_CAT_2_TC_IQM_0_MAP_NIFm : CFC_CAT_2_TC_IQM_1_MAP_NIFm;
        entry_offset = info->cat2_tc + ((info->is_ocb_only == 1)?(8):(0));
        
        cat2tc_map_ena_reg = CFC_CAT_2_TC_MAP_NIF_ENAr;
        bit_offset = info->cat2_tc + ((info->is_ocb_only == 1)?(8):(0));

        SOC_REG_ABOVE_64_CLEAR(map_data);
        /* set mapping from CAT2+TC to NIF PFC */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
        SOCDNX_IF_ERR_EXIT(rv);

        SHR_BITCOPY_RANGE(map_data, (offset*8), &(info->cat2_tc_bitmap), 0, 8);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
        SOCDNX_IF_ERR_EXIT(rv);

        /* enable map from CAT2+TC to NIF PFC */
        SOC_REG_ABOVE_64_CLEAR(cat2tc_map_ena);
        rv = soc_reg_above_64_get(unit, cat2tc_map_ena_reg, REG_PORT_ANY, 0, cat2tc_map_ena);
        SOCDNX_IF_ERR_EXIT(rv);
        
        SHR_BITSET(cat2tc_map_ena, bit_offset);
        
        rv = soc_reg_above_64_set(unit, cat2tc_map_ena_reg, REG_PORT_ANY, 0, cat2tc_map_ena);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    /* Set the bit in the CAT2+TC PFC bitmap that will be sent to the NIF (NIF_PORT x TC) } */

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  jer_fc_gen_inbnd_ll_set_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  soc_port_t                    port,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_LL       *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        offset;
    uint32
        base_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        data;
    int
        core;
    soc_reg_above_64_val_t 
        llfc_map;
    soc_reg_t 
        llfc_map_reg = INVALIDr;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_port_sw_db_core_get(unit, port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_first_phy_port_get(unit, port, &offset /*one based*/);
    SOCDNX_IF_ERR_EXIT(rv);
    offset--;

    rv = soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Get PPCT data and update it */
    rv = arad_egq_ppct_tbl_get_unsafe(
              unit,
              core,
              base_q_pair,
              &data
              );
    SOCDNX_IF_ERR_EXIT(rv);

    data.cnm_intrcpt_fc_en = SOC_SAND_BOOL2NUM(info->cnm_enable);
    /* Map OTM-Port to the LLFC Channel (internal nif id) */
    data.cnm_intrcpt_fc_vec_llfc = offset; 

    rv = arad_egq_ppct_tbl_set_unsafe(
              unit,
              core,
              base_q_pair,
              &data
              );
    SOCDNX_IF_ERR_EXIT(rv);

    /* set mapping from global resource to LLFC { */
    if (info->gen_src ==  SOC_TMC_FC_GEN_SRC_GLB_HIGH) {
        if (info->is_ocb_only) {
            if (info->glbl_rcs_pool == 0) {
                llfc_map_reg = (core == 0) ? CFC_GLB_RSC_OCB_POOL_0_IQM_0_TO_NIF_LLFC_HP_MAPr : CFC_GLB_RSC_OCB_POOL_0_IQM_1_TO_NIF_LLFC_HP_MAPr;                    
            }
            else {
                llfc_map_reg = (core == 0) ? CFC_GLB_RSC_OCB_POOL_1_IQM_0_TO_NIF_LLFC_HP_MAPr : CFC_GLB_RSC_OCB_POOL_1_IQM_1_TO_NIF_LLFC_HP_MAPr;
            }
        }
        else {
            if (info->glbl_rcs_pool == 0) {
                llfc_map_reg = (core == 0) ? CFC_GLB_RSC_MIX_POOL_0_IQM_0_TO_NIF_LLFC_HP_MAPr : CFC_GLB_RSC_MIX_POOL_0_IQM_1_TO_NIF_LLFC_HP_MAPr;            
            }
            else {
                llfc_map_reg = (core == 0) ? CFC_GLB_RSC_MIX_POOL_1_IQM_0_TO_NIF_LLFC_HP_MAPr : CFC_GLB_RSC_MIX_POOL_1_IQM_1_TO_NIF_LLFC_HP_MAPr;
            }
        }
    }

    if (llfc_map_reg != INVALIDr) {
        SOC_REG_ABOVE_64_CLEAR(llfc_map);
        rv = soc_reg_above_64_get(unit, llfc_map_reg, REG_PORT_ANY, 0, llfc_map);
        SOCDNX_IF_ERR_EXIT(rv);
        if (info->glbl_rcs_enable) {
            SHR_BITSET(llfc_map, offset);
        }
        else {
            SHR_BITCLR(llfc_map, offset);
        }
        rv = soc_reg_above_64_set(unit, llfc_map_reg, REG_PORT_ANY, 0, llfc_map);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    /* set mapping from global resource to LLFC } */

exit:
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
*     Inband Flow Control Generation Configuration, based on
 *     Arad Ingress state indications.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t
  jer_fc_gen_inbnd_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  soc_port_t                         port,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_INFO          *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    SOC_TMC_FC_GEN_INBND_LL
        ll_disabled;
    SOC_TMC_FC_GEN_INBND_PFC
        pfc_disabled;
    SOC_TMC_FC_ENABLE_BITMAP
        cfc_enables;
  
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);


    SOC_TMC_FC_GEN_INBND_LL_clear(&ll_disabled);
    SOC_TMC_FC_GEN_INBND_PFC_clear(&pfc_disabled);
    pfc_disabled.inherit = SOC_TMC_FC_INBND_PFC_INHERIT_DISABLED;

    /* setting CFC enables - device level { */
    SHR_BITCLR_RANGE(cfc_enables.bmp, 0, SOC_TMC_FC_NOF_ENABLEs);
    
    switch (info->mode) {
    case SOC_TMC_FC_INBND_MODE_LL:
        if (info->ll.gen_src == SOC_TMC_FC_GEN_SRC_VSQ_LLFC) {
            SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN);
        }
        else if (info->ll.gen_src == SOC_TMC_FC_GEN_SRC_GLB_HIGH) {
            SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_NIF_LLFC_EN);
        }
        break;
    case SOC_TMC_FC_INBND_MODE_PFC:
    case SOC_TMC_FC_INBND_MODE_SAFC:
        if (info->pfc.gen_src == SOC_TMC_FC_GEN_SRC_VSQ_PFC) {
            SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_PFC_VSQ_TO_NIF_EN);
        }
        else if ((info->pfc.gen_src == SOC_TMC_FC_GEN_SRC_GLB_HIGH) || (info->pfc.gen_src <= SOC_TMC_FC_GEN_SRC_GLB_LOW)) {
            SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_NIF_PFC_EN);
        }
        break;
    case SOC_TMC_FC_INBND_MODE_DEVICE_DISABLED:
        SHR_BITSET(cfc_enables.bmp, (SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN | 
                                     SOC_TMC_FC_PFC_VSQ_TO_NIF_EN | 
                                     SOC_TMC_FC_GLB_RSC_TO_NIF_LLFC_EN | 
                                     SOC_TMC_FC_GLB_RSC_TO_NIF_PFC_EN));
        break;
    default:
    case SOC_TMC_FC_INBND_MODE_DISABLED:
        break;
    }

    rv = jer_fc_enables_set(unit, &cfc_enables, &cfc_enables);
    SOCDNX_IF_ERR_EXIT(rv);
    /* setting CFC enables - device level } */

    /* setting MAC registers - port level { */
    rv = jer_fc_inbnd_mode_set(unit, port, 1, info->mode);
    SOCDNX_IF_ERR_EXIT(rv);
    /* setting MAC registers - port level } */

    /* setting registers in CFC, NBI blocks { */
    switch (info->mode)
    {
    case SOC_TMC_FC_INBND_MODE_LL:
        /* Configure LL */
        rv = jer_fc_gen_inbnd_ll_set_unsafe(
            unit,
            port,
            &info->ll
          );
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_TMC_FC_INBND_MODE_PFC:
    case SOC_TMC_FC_INBND_MODE_SAFC:
        /* Configure PFC */
        rv = jer_fc_gen_inbnd_pfc_set_unsafe(
            unit,
            port,
            &info->pfc
          );
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_TMC_FC_INBND_MODE_DISABLED:
    default:
        break;
    }
    /* setting registers in CFC, NBI blocks } */

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  jer_fc_gen_inbnd_pfc_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  soc_port_t                    port,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_PFC      *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        offset,
        reg_offset,
        bit_offset,
        fld_offset;
    uint32
        base_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        data;
    soc_field_t 
        rx_mlf_pfc_fld[4] = {
        FC_TX_N_PORT_0_GEN_PFC_FROM_MLF_MASKf,
        FC_TX_N_PORT_1_GEN_PFC_FROM_MLF_MASKf, 
        FC_TX_N_PORT_2_GEN_PFC_FROM_MLF_MASKf, 
        FC_TX_N_PORT_3_GEN_PFC_FROM_MLF_MASKf};
    uint32
        pfc_map_rx_mlf_reg;
    int
        core;
    soc_mem_t 
        mem= INVALIDm;
    soc_reg_above_64_val_t
        map_data;
    uint32
        entry_offset = 0; 
    soc_reg_above_64_val_t 
        cat2tc_map_ena_data;
    soc_reg_t 
        cat2tc_map_ena_reg = INVALIDr;
    int 
        ca2tc_map_ena = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_port_sw_db_core_get(unit, port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_first_phy_port_get(unit, port, &offset /*one based*/);
    SOCDNX_IF_ERR_EXIT(rv);
    offset--;

    rv = soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_egq_ppct_tbl_get_unsafe(
                    unit,
                    core,
                    base_q_pair,
                    &data
                    );
    SOCDNX_IF_ERR_EXIT(rv);
    info->cnm_intercept_enable = SOC_SAND_NUM2BOOL(data.cnm_intrcpt_fc_en);


    /* Get the bit in the GLB RSC PFC bitmap that will be sent to the NIF (NIF_PORT x TC) { */
    if ((info->gen_src == SOC_TMC_FC_GEN_SRC_GLB_LOW) || (info->gen_src == SOC_TMC_FC_GEN_SRC_GLB_HIGH)) { 
        mem = (core == 0) ? CFC_GLB_RSC_IQM_0_MAPm : CFC_GLB_RSC_IQM_1_MAPm;
        entry_offset = ((info->is_ocb_only == 1)?(4):(0)) + 
                       ((info->glbl_rcs_pool == 1)?(2):(0)) + 
                       ((info->gen_src == SOC_TMC_FC_GEN_SRC_GLB_HIGH)?(1):(0));

        /* get mapping from CAT2+TC to NIF PFC */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
        SOCDNX_IF_ERR_EXIT(rv);

        if (info->gen_src == SOC_TMC_FC_GEN_SRC_GLB_HIGH) {
            SHR_BITCOPY_RANGE(&(info->glbl_rcs_high), 0, map_data, (offset*8), 8);
        }
        else {
            SHR_BITCOPY_RANGE(&(info->glbl_rcs_low), 0, map_data, (offset*8), 8);
        }
    }
    /* Get the bit in the GLB RSC PFC bitmap that will be sent to the NIF (NIF_PORT x TC) } */

    /* Get the bit in the MLF PFC bitmap { */
    reg_offset = offset / ARAD_MAX_NIFS_PER_WC;
    fld_offset = offset % ARAD_MAX_NIFS_PER_WC;

    rv = READ_NBI_FC_PFC_MLF_MASKr(unit, reg_offset, &pfc_map_rx_mlf_reg);
    SOCDNX_IF_ERR_EXIT(rv);

    info->nif_cls_bitmap = soc_reg_field_get(unit, NBI_FC_PFC_MLF_MASKr, pfc_map_rx_mlf_reg, rx_mlf_pfc_fld[fld_offset]);
    /* Get the bit in the MLF PFC bitmap } */

    /* Get the bit in the CAT2+TC PFC bitmap that will be sent to the NIF (NIF_PORT x TC) { */
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_VSQ_CAT2TC) {
        mem = (core == 0) ? CFC_CAT_2_TC_IQM_0_MAP_NIFm : CFC_CAT_2_TC_IQM_1_MAP_NIFm;
        entry_offset = info->cat2_tc + ((info->is_ocb_only == 1)?(8):(0));

        cat2tc_map_ena_reg = CFC_CAT_2_TC_MAP_NIF_ENAr;
        bit_offset = info->cat2_tc + ((info->is_ocb_only == 1)?(8):(0));

        /* check if map from CAT2+TC to NIF PFC is enable */
        SOC_REG_ABOVE_64_CLEAR(cat2tc_map_ena_data);
        rv = soc_reg_above_64_get(unit, cat2tc_map_ena_reg, REG_PORT_ANY, 0, cat2tc_map_ena_data);
        SOCDNX_IF_ERR_EXIT(rv);
        
        ca2tc_map_ena = SHR_BITGET(cat2tc_map_ena_data, bit_offset);

        /* get mapping from CAT2+TC to NIF PFC */
        if (ca2tc_map_ena) {
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
            SOCDNX_IF_ERR_EXIT(rv);

            SHR_BITCOPY_RANGE(&(info->cat2_tc_bitmap), 0, map_data, (offset*8), 8);
        }
        else {
            info->cat2_tc_bitmap = 0;
        }
    }
    /* Get the bit in the CAT2+TC PFC bitmap that will be sent to the NIF (NIF_PORT x TC) } */

    info->inherit = SOC_TMC_FC_INBND_PFC_INHERIT_DISABLED;

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  jer_fc_gen_inbnd_ll_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  soc_port_t                    port,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_LL       *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        base_q_pair,
        phy_port;
    ARAD_EGQ_PPCT_TBL_DATA
        data;
    int
        core;
    soc_reg_above_64_val_t llfc_map;
    soc_reg_t llfc_map_reg = INVALIDr;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_port_sw_db_core_get(unit, port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_egq_ppct_tbl_get_unsafe(
                    unit,
                    core,
                    base_q_pair,
                    &data
                  );
    SOCDNX_IF_ERR_EXIT(rv);
    info->cnm_enable = SOC_SAND_NUM2BOOL(data.cnm_intrcpt_fc_en);

    /* check if mapping from global resource to LLFC is enable { */
    if (info->gen_src ==  SOC_TMC_FC_GEN_SRC_GLB_HIGH) {
        if (info->is_ocb_only) {
            if (info->glbl_rcs_pool == 0) {
                llfc_map_reg = (core == 0) ? CFC_GLB_RSC_OCB_POOL_0_IQM_0_TO_NIF_LLFC_HP_MAPr : CFC_GLB_RSC_OCB_POOL_0_IQM_1_TO_NIF_LLFC_HP_MAPr;                    
            }
            else {
                llfc_map_reg = (core == 0) ? CFC_GLB_RSC_OCB_POOL_1_IQM_0_TO_NIF_LLFC_HP_MAPr : CFC_GLB_RSC_OCB_POOL_1_IQM_1_TO_NIF_LLFC_HP_MAPr;
            }
        }
        else {
            if (info->glbl_rcs_pool == 0) {
                llfc_map_reg = (core == 0) ? CFC_GLB_RSC_MIX_POOL_0_IQM_0_TO_NIF_LLFC_HP_MAPr : CFC_GLB_RSC_MIX_POOL_0_IQM_1_TO_NIF_LLFC_HP_MAPr;            
            }
            else {
                llfc_map_reg = (core == 0) ? CFC_GLB_RSC_MIX_POOL_1_IQM_0_TO_NIF_LLFC_HP_MAPr : CFC_GLB_RSC_MIX_POOL_1_IQM_1_TO_NIF_LLFC_HP_MAPr;
            }
        }

        if (llfc_map_reg != INVALIDr) {
            SOC_REG_ABOVE_64_CLEAR(llfc_map);
            rv = soc_reg_above_64_get(unit, llfc_map_reg, REG_PORT_ANY, 0, llfc_map);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/);
            SOCDNX_IF_ERR_EXIT(rv);

            phy_port--;

            info->glbl_rcs_enable = SHR_BITGET(llfc_map, phy_port);
        }
    }
    /* check if mapping from global resource to LLFC is enable } */

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Inband Flow Control Generation Configuration, based on
 *     Arad Ingress state indications.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t
  jer_fc_gen_inbnd_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_INFO  *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_TMC_FC_GEN_INBND_INFO_clear(info);

    /* Check which mode is enabled */
    rv = jer_fc_inbnd_mode_get(unit, port, 1, &(info->mode));
    SOCDNX_IF_ERR_EXIT(rv);
  
    /* Get the LL configuration */
    if (info->mode == SOC_TMC_FC_INBND_MODE_LL)
    {
        rv = jer_fc_gen_inbnd_ll_get_unsafe(
                unit,
                port,
                &info->ll
              );
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* Get the PFC configuration */
    if (info->mode == SOC_TMC_FC_INBND_MODE_PFC || info->mode == SOC_TMC_FC_INBND_MODE_SAFC)
    {
        rv = jer_fc_gen_inbnd_pfc_get_unsafe(
                unit,
                port,
                &info->pfc
              );
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
*     Inband Flow Control Reception Configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t
  jer_fc_rec_inbnd_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_REC_INBND_INFO           *info
  )
{
    soc_error_t
        res = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_nif_id_verify(
          nif_ndx
        );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if (ARAD_NIF_IS_TYPE_ID(ILKN, nif_ndx))
    {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_rec_inbnd_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx
  )
{
    uint32
        res = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_nif_id_verify(
          nif_ndx
        );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if (ARAD_NIF_IS_TYPE_ID(ILKN, nif_ndx))
    {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Generation (OOB/ILKN-Inband TX).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t
  jer_fc_gen_cal_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (cal_mode_ndx >= SOC_TMC_FC_NOF_CAL_MODES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
    else {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_gen_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        source_val = 0;
    uint32
        rep_idx = 0,      /* Calendar repetition index */
        per_rep_idx = 0,  /* Calendar entry index, per repetition */
        entry_idx = 0;    /* Calendar entry index, global */
    ARAD_CFC_CALTX_TBL_DATA
        tx_cal;
    uint64
        field64;
    uint32
        field32;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t 
        cal_table_data;
    soc_reg_t
        reg = INVALIDr;
    soc_field_t 
        fld = INVALIDf;
    soc_field_t ilkn_inb_tx_ena_flds[] = {
        ILKN_INB_TX_0_ENf, ILKN_INB_TX_1_ENf, ILKN_INB_TX_2_ENf, 
        ILKN_INB_TX_3_ENf, ILKN_INB_TX_4_ENf, ILKN_INB_TX_5_ENf};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cal_conf);
    SOCDNX_NULL_CHECK(cal_buff);

    rv = jer_fc_gen_cal_set_verify(unit, cal_mode_ndx, if_ndx, cal_conf, cal_buff);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(cal_table_data);

    rv = jer_fc_clear_calendar_unsafe(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_TX, if_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Calendar entries { */
    for (per_rep_idx = 0; per_rep_idx < cal_conf->cal_len; per_rep_idx++)
    {
        /* Convert source enum to HW value */
        source_val = jer_fc_gen_cal_src_type_to_val_internal(cal_buff[per_rep_idx].source);

        if (source_val == -1)
        {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        else
        {
            tx_cal.fc_source_sel = source_val;
            tx_cal.fc_index = cal_buff[per_rep_idx].id;
        }

        rv = jer_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_TX, if_ndx, &cal_table);
        SOCDNX_IF_ERR_EXIT(rv);
        for (rep_idx = 0; rep_idx < cal_conf->cal_reps; rep_idx++)
        {
            entry_idx = (rep_idx * cal_conf->cal_len) + per_rep_idx;

            soc_mem_field32_set(unit, cal_table, cal_table_data, FC_INDEXf, tx_cal.fc_index);
            soc_mem_field32_set(unit, cal_table, cal_table_data, FC_SRC_SELf, tx_cal.fc_source_sel);
            
            rv = soc_mem_write(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    } /* Calendar entries } */

    /* Enable the FC after configuring it { */
    
    /* Select OOB Type 1'b1 - ILKN; 1'b0 - SPI4 / HCFC */
    if ((cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) || (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)) {
        reg = CFC_OOB_PAD_CONFIGURATIONr;
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? OOB_TX_0_MODEf : OOB_TX_1_MODEf);
        field32 = SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB);       
        rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  field32);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* Enalbe SPI OOB */
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) {
        reg = CFC_SPI_OOB_TX_CONFIGURATIONr;        
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_ENf : SPI_OOB_TX_1_ENf);
        field32 = 1;
        rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld , field32);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* Enalbe ILKN && Select ILKN Type */
    reg = CFC_ILKN_TX_CONFIGURATIONr;        
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        /* Enable ILKN InBand */
        fld = ilkn_inb_tx_ena_flds[if_ndx];
    }
    else if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB) {
        /* Enable ILKN OOB */
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_TX_0_ENf : ILKN_OOB_TX_1_ENf);
    }
    field32 = cal_conf->enable; 
    rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  field32);
    SOCDNX_IF_ERR_EXIT(rv);

    COMPILER_64_SET(field64,0,0x1);
    rv = soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64);
    SOCDNX_IF_ERR_EXIT(rv);
    COMPILER_64_SET(field64,0,0x0);
    rv = soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64);
    SOCDNX_IF_ERR_EXIT(rv);
    /* Enable the FC after configuring it } */

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  jer_fc_gen_cal_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (cal_mode_ndx >= SOC_TMC_FC_NOF_CAL_MODES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
    else {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Generation (OOB/ILKN-Inband TX).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t
  jer_fc_gen_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        entry_idx = 0;    /* Calendar entry index, global */
    ARAD_CFC_CALTX_TBL_DATA
        tx_cal;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t
        cal_table_data;
    soc_reg_t
        en_reg = INVALIDr,
        cal_len_reg = INVALIDr,
        cal_rep_reg = INVALIDr;
    soc_field_t 
        en_fld = INVALIDf, 
        cal_len_fld = INVALIDf, 
        cal_rep_fld = INVALIDf;
    uint32
        en_fld_val = 0,
        cal_len_val = 0,
        cal_rep_val = 0;
    soc_field_t ilkn_inb_tx_ena_flds[] = {
        ILKN_INB_TX_0_ENf, ILKN_INB_TX_1_ENf, ILKN_INB_TX_2_ENf, 
        ILKN_INB_TX_3_ENf, ILKN_INB_TX_4_ENf, ILKN_INB_TX_5_ENf};
    soc_field_t ilkn_inb_tx_cal_len_flds[] = {
        ILKN_INB_TX_0_CAL_LENf, ILKN_INB_TX_1_CAL_LENf, ILKN_INB_TX_2_CAL_LENf, 
        ILKN_INB_TX_3_CAL_LENf, ILKN_INB_TX_4_CAL_LENf, ILKN_INB_TX_5_CAL_LENf};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cal_conf);
    SOCDNX_NULL_CHECK(cal_buff);

    rv = jer_fc_gen_cal_get_verify(unit, cal_mode_ndx, if_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_TMC_FC_CAL_IF_INFO_clear(cal_conf);
    SOC_TMC_FC_GEN_CALENDAR_clear(cal_buff);
    SOC_REG_ABOVE_64_CLEAR(cal_table_data);

    /* Calendar parameters { */
    /* Calendar Enable/disable { */
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB){
        en_reg = CFC_SPI_OOB_TX_CONFIGURATIONr;
        en_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SPI_OOB_TX_0_ENf : SPI_OOB_TX_1_ENf;
        rv = soc_reg_above_64_field32_read(unit, en_reg, REG_PORT_ANY, 0, en_fld, &en_fld_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else { /* ILKN */
        en_reg = CFC_ILKN_TX_CONFIGURATIONr;
        if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
            en_fld = ilkn_inb_tx_ena_flds[if_ndx];
        }
        else if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB) {
            en_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? ILKN_OOB_TX_0_ENf : ILKN_OOB_TX_1_ENf;
        }
        rv = soc_reg_above_64_field32_read(unit, en_reg, REG_PORT_ANY, 0, en_fld, &en_fld_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    cal_conf->enable = SOC_SAND_NUM2BOOL(en_fld_val);
    /* Calendar Enable/disable } */

    /* Calendar length (single repetition) { */
    /* The len register should be set to LEN-1 */
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
        cal_len_reg = CFC_SPI_OOB_TX_CONFIGURATIONr;
        cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_LENf : SPI_OOB_TX_1_CAL_LENf);
        rv = soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &cal_len_val);
        SOCDNX_IF_ERR_EXIT(rv);
        cal_conf->cal_len = cal_len_val;
    }
    else
    {
        cal_len_reg = CFC_ILKN_TX_CONFIGURATIONr;
        if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
            cal_len_fld = ilkn_inb_tx_cal_len_flds[if_ndx];
        }
        else if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB) {
            cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? ILKN_OOB_TX_0_CAL_LENf : ILKN_OOB_TX_1_CAL_LENf;
        }

        rv = soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &cal_len_val);
        SOCDNX_IF_ERR_EXIT(rv);
        cal_conf->cal_len = cal_len_val + 1;
    }
    /* Calendar length (single repetition) } */

    /* Calendar number of repetitions - SPI/OOB Only { */
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
        cal_rep_reg = CFC_SPI_OOB_TX_CONFIGURATIONr;
        cal_rep_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_Mf : SPI_OOB_TX_1_CAL_Mf);

        rv = soc_reg_above_64_field32_read(unit, cal_rep_reg, REG_PORT_ANY, 0, cal_rep_fld, &cal_rep_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else /* ILKN */
    {
        cal_rep_val = 1;
    }
    cal_conf->cal_reps = cal_rep_val;
    /* Calendar number of repetitions - SPI/OOB Only } */

    /* Calendar parameters } */

    /* Calendar entries { */

    rv = jer_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_TX, if_ndx, &cal_table);
    SOCDNX_IF_ERR_EXIT(rv);
    
    for (entry_idx = 0; entry_idx < cal_conf->cal_len; entry_idx++)
    {
        rv = soc_mem_read(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
        SOCDNX_IF_ERR_EXIT(rv);

        tx_cal.fc_index = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_INDEXf);
        tx_cal.fc_source_sel = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_SRC_SELf);

        if(tx_cal.fc_source_sel >= JER_FC_GEN_CAL_SRC_ARR_SIZE)
        {
            cal_buff[entry_idx].source = SOC_TMC_FC_GEN_CAL_SRC_NONE;
            cal_buff[entry_idx].id = 0;
        }
        else
        {
            cal_buff[entry_idx].source = jer_fc_gen_cal_src_arr[tx_cal.fc_source_sel];
            cal_buff[entry_idx].id = tx_cal.fc_index;
        }
    }
    /* Calendar entries } */

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_clear_calendar_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE              cal_mode_ndx,
    SOC_SAND_IN  ARAD_FC_CAL_TYPE                 cal_type,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             if_ndx
  )
{
    uint32
        res = SOC_SAND_OK;
    soc_error_t
        rv = SOC_E_NONE;
    uint32 
        cal_rx_entry[128] = {0};
    uint32 
        cal_tx_entry[128] = {0};
    soc_mem_t
        cal_table = INVALIDm;
  
    SOCDNX_INIT_FUNC_DEFS;

    rv = jer_fc_cal_tbl_get(unit, cal_mode_ndx, cal_type, if_ndx, &cal_table);
    SOCDNX_IF_ERR_EXIT(rv);

    if(cal_type == ARAD_FC_CAL_TYPE_RX)
    {
        soc_mem_field32_set(unit, cal_table, cal_rx_entry, FC_DST_SELf, jer_fc_rec_cal_dest_type_to_val_internal(SOC_TMC_FC_REC_CAL_DEST_NONE));
        soc_mem_field32_set(unit, cal_table, cal_rx_entry, FC_INDEXf, 0x0);

        res = arad_fill_table_with_entry(unit, cal_table, MEM_BLOCK_ANY, cal_rx_entry); 
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }
    
    if(cal_type == ARAD_FC_CAL_TYPE_TX)
    {
        soc_mem_field32_set(unit, cal_table, cal_tx_entry, FC_SRC_SELf, jer_fc_gen_cal_src_type_to_val_internal(SOC_TMC_FC_GEN_CAL_SRC_CONST));
        soc_mem_field32_set(unit, cal_table, cal_tx_entry, FC_INDEXf, 0x0);

        res = arad_fill_table_with_entry(unit, cal_table, MEM_BLOCK_ANY, cal_tx_entry); 
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t
  jer_fc_rec_cal_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (cal_mode_ndx >= SOC_TMC_FC_NOF_CAL_MODES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
    else {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
 
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  jer_fc_rec_cal_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        dest_sel = 0;
    uint32
        rep_idx = 0,      /* Calendar repetition index */
        per_rep_idx = 0,  /* Calendar entry index, per repetition */
        entry_idx = 0;    /* Calendar entry index, global */
    ARAD_CFC_CALRX_TBL_DATA
        rx_cal;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t 
        cal_table_data;
    uint64
        field64;
    SOC_TMC_FC_ENABLE_BITMAP
        cfc_enables; 
    uint32
        field32;
    soc_reg_t
        reg = INVALIDr;
    soc_field_t 
        fld = INVALIDf;
    soc_field_t ilkn_inb_rx_ena_flds[] = {
        ILKN_INB_RX_0_ENf, ILKN_INB_RX_1_ENf, ILKN_INB_RX_2_ENf, 
        ILKN_INB_RX_3_ENf, ILKN_INB_RX_4_ENf, ILKN_INB_RX_5_ENf};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cal_conf);
    SOCDNX_NULL_CHECK(cal_buff);

    rv = jer_fc_rec_cal_set_verify(unit, cal_mode_ndx, if_ndx, cal_conf, cal_buff);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Clear calendar */
    rv = jer_fc_clear_calendar_unsafe(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_RX, if_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Calendar entries { */
    for (per_rep_idx = 0; per_rep_idx < cal_conf->cal_len; per_rep_idx++)
    {
        /* Convert destination enum to HW value */
        dest_sel = jer_fc_rec_cal_dest_type_to_val_internal(cal_buff[per_rep_idx].destination);

        if(dest_sel == -1)
        {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        else if (cal_buff[per_rep_idx].destination == SOC_TMC_FC_REC_CAL_DEST_NONE)
        {
            rx_cal.fc_dest_sel = dest_sel;
            rx_cal.fc_index = 0;
        }
        else
        {
            rx_cal.fc_dest_sel = dest_sel;
            rx_cal.fc_index = cal_buff[per_rep_idx].id;
        }

        rv = jer_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_RX, if_ndx, &cal_table);
        SOCDNX_IF_ERR_EXIT(rv);
        for (rep_idx = 0; rep_idx < cal_conf->cal_reps; rep_idx++)
        {
            entry_idx = (rep_idx * cal_conf->cal_len) + per_rep_idx;

            soc_mem_field32_set(unit, cal_table, cal_table_data, FC_INDEXf, rx_cal.fc_index);
            soc_mem_field32_set(unit, cal_table, cal_table_data, FC_DST_SELf, rx_cal.fc_dest_sel);
            
            rv = soc_mem_write(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    /* Enable the FC after configuring it { */

    /* Enalbe SPI OOB */
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) {
        reg = CFC_SPI_OOB_RX_CONFIGURATION_0r;
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf);
        field32 = cal_conf->enable;
        rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld , field32);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* Enalbe ILKN && Select ILKN Type */
    reg = CFC_ILKN_RX_CONFIGURATIONr;
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        /* Enable ILKN InBand */
        fld = ilkn_inb_rx_ena_flds[if_ndx];
    }
    else if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB) {
        /* Enable ILKN OOB */
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_RX_0_ENf : ILKN_OOB_RX_0_ENf);
    }
    field32 = cal_conf->enable;
    rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  field32);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Set CFC Enablers { */
    SHR_BITCLR_RANGE(((cfc_enables).bmp), 0, SOC_TMC_FC_NOF_ENABLEs);

    /* Always enable the CFC Enablers for ILKN , it's device-based */
    SHR_BITSET(cfc_enables.bmp, (SOC_TMC_FC_ILKN_RX_TO_EGQ_PFC_EN | 
               SOC_TMC_FC_ILKN_RX_TO_EGQ_PORT_EN |
               SOC_TMC_FC_ILKN_RX_TO_GEN_PFC_EN |
               SOC_TMC_FC_ILKN_RX_TO_NIF_FAST_LLFC_EN));

    /* Always enable the CFC Enablers for OOB , it's device-based */
    SHR_BITSET(cfc_enables.bmp, (SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PFC_EN |
               SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PORT_EN |
               SOC_TMC_FC_SPI_OOB_RX_TO_GEN_PFC_EN |
               SOC_TMC_FC_SPI_OOB_RX_TO_NIF_FAST_LLFC_EN));
    
    rv = jer_fc_enables_set(unit, &cfc_enables, &cfc_enables);
    SOCDNX_IF_ERR_EXIT(rv);
    /* Set CFC Enablers } */

    COMPILER_64_SET(field64,0, 0x1);
    rv = soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64);
    SOCDNX_IF_ERR_EXIT(rv);
    COMPILER_64_SET(field64,0, 0x0);
    rv = soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Enable the FC after configuring it } */
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  jer_fc_rec_cal_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (cal_mode_ndx >= SOC_TMC_FC_NOF_CAL_MODES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
    else {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t
  jer_fc_rec_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        entry_idx = 0;    /* Calendar entry index, global */
    ARAD_CFC_CALRX_TBL_DATA
        rx_cal;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t
        cal_table_data;
    soc_reg_t
        en_reg = INVALIDr,
        cal_len_reg = INVALIDr,
        cal_rep_reg = INVALIDr;        
    soc_field_t 
        en_fld = INVALIDf, 
        cal_len_fld = INVALIDf, 
        cal_rep_fld = INVALIDf;
    uint32
        en_fld_val = 0,
        cal_len_val = 0,
        cal_rep_val = 0;
    soc_field_t ilkn_inb_rx_ena_flds[] = {
        ILKN_INB_RX_0_ENf, ILKN_INB_RX_1_ENf, ILKN_INB_RX_2_ENf, 
        ILKN_INB_RX_3_ENf, ILKN_INB_RX_4_ENf, ILKN_INB_RX_5_ENf};
    soc_field_t ilkn_inb_rx_cal_len_flds[] = {
        FC_ILKN_RX_0_CHFC_CAL_LENf, FC_ILKN_RX_1_CHFC_CAL_LENf, FC_ILKN_RX_2_CHFC_CAL_LENf, 
        FC_ILKN_RX_3_CHFC_CAL_LENf, FC_ILKN_RX_4_CHFC_CAL_LENf, FC_ILKN_RX_5_CHFC_CAL_LENf};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cal_conf);
    SOCDNX_NULL_CHECK(cal_buff);

    rv = jer_fc_rec_cal_get_verify(unit, cal_mode_ndx, if_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(cal_table_data);

    /* Calendar parameters { */
    /* Calendar Enable/disable { */
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB){
        en_reg = CFC_SPI_OOB_RX_CONFIGURATION_0r;
        en_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf;
        rv = soc_reg_above_64_field32_read(unit, en_reg, REG_PORT_ANY, 0, en_fld, &en_fld_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else { /* ILKN */
        en_reg = CFC_ILKN_RX_CONFIGURATIONr;
        if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
            en_fld = ilkn_inb_rx_ena_flds[if_ndx];
        }
        else if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB) {
            en_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? ILKN_OOB_RX_0_ENf : ILKN_OOB_RX_1_ENf;
        }
        rv = soc_reg_above_64_field32_read(unit, en_reg, REG_PORT_ANY, 0, en_fld, &en_fld_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    cal_conf->enable = SOC_SAND_NUM2BOOL(en_fld_val);
    /* Calendar Enable/disable } */

    /* Calendar length (single repetition) { */
    /* The len register should be set to LEN-1 */
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
        cal_len_reg = CFC_SPI_OOB_RX_CONFIGURATION_0r;
        cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_LENf : SPI_OOB_RX_1_CAL_LENf);
        rv = soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &cal_len_val);
        SOCDNX_IF_ERR_EXIT(rv);
        cal_conf->cal_len = cal_len_val;
    }
    else if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)
    {
        cal_len_reg = CFC_ILKN_RX_CONFIGURATIONr;
        cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_RX_0_CAL_LENf : ILKN_OOB_RX_1_CAL_LENf);
        rv = soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &cal_len_val);
        SOCDNX_IF_ERR_EXIT(rv);
        cal_conf->cal_len = cal_len_val+1;
    }
    else if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND)
    {
        cal_len_reg = NBIH_FC_ILKN_RX_CHFC_CAL_LENr;
        cal_len_fld = ilkn_inb_rx_cal_len_flds[if_ndx];
        rv = soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &cal_len_val);
        SOCDNX_IF_ERR_EXIT(rv);
        cal_conf->cal_len = cal_len_val;
    }
    /* Calendar length (single repetition) } */

    /* Calendar number of repetitions - SPI/OOB Only { */
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
        cal_rep_reg = CFC_SPI_OOB_RX_CONFIGURATION_0r;
        cal_rep_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_Mf : SPI_OOB_RX_1_CAL_Mf);

        rv = soc_reg_above_64_field32_read(unit, cal_rep_reg, REG_PORT_ANY, 0, cal_rep_fld, &cal_rep_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else /* ILKN */
    {
        cal_rep_val = 1;
    }
    cal_conf->cal_reps = cal_rep_val;
    /* Calendar number of repetitions - SPI/OOB Only } */
  
    /* Calendar parameters } */

    /* Calendar entries { */
    rv = jer_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_RX, if_ndx, &cal_table);
    SOCDNX_IF_ERR_EXIT(rv);
    for (entry_idx = 0; entry_idx < cal_conf->cal_len; entry_idx++)
    {
        rv = soc_mem_read(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
        SOCDNX_IF_ERR_EXIT(rv);

        rx_cal.fc_index = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_INDEXf);
        rx_cal.fc_dest_sel = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_DST_SELf);

        if(rx_cal.fc_dest_sel >= JER_FC_REC_CAL_DEST_ARR_SIZE)
        {
            cal_buff[entry_idx].destination = SOC_TMC_FC_REC_CAL_DEST_NONE;
            cal_buff[entry_idx].id = 0;
        }
        else
        {
            cal_buff[entry_idx].destination = jer_fc_rec_cal_dest_arr[rx_cal.fc_dest_sel];
            cal_buff[entry_idx].id = rx_cal.fc_index;
        }
    }
    /* Calendar entries } */

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Defines if and how LLFC can be received/generated using
 *     Interlaken NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t
  jer_fc_ilkn_llfc_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID          ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO           *info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (ilkn_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn_ndx is out of range")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_ilkn_llfc_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID          ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO        *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        ilkn_bit_offset;
    soc_reg_t
        ilkn_llfc_reg = INVALIDr,
        multiple_use_bits_reg = INVALIDr;
    soc_field_t 
        on_ch0_fld = INVALIDf, 
        every_16_chs_fld = INVALIDf,
        multiple_use_bits_fld = INVALIDf;
    uint32
        buffer;
    uint64
        field64;
    int reg_arr_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    rv = jer_fc_ilkn_llfc_set_verify(unit, ilkn_ndx, direction_ndx, info);
    SOCDNX_IF_ERR_EXIT(rv);

    ilkn_llfc_reg = NBIH_FC_ILKNr;
    multiple_use_bits_reg = NBIH_ILKN_MULTIPLE_USE_BITSr;
    if (ARAD_IS_DIRECTION_REC(direction_ndx)) {
        multiple_use_bits_fld = ILKN_RX_N_LLFC_STOP_TX_FROM_MULTIPLE_USE_BITS_MASKf;
    }
    else {
        multiple_use_bits_fld = ILKN_TX_N_MULTIPLE_USE_BITS_FROM_LLFC_MASKf;
    }
    reg_arr_index = ilkn_ndx;

    if (ARAD_IS_DIRECTION_REC(direction_ndx)) {
        on_ch0_fld                 = FC_ILKN_RX_LLFC_ON_CH_0f;
        every_16_chs_fld           = FC_ILKN_RX_LLFC_EVERY_16_CHSf;
    }
    else{
        on_ch0_fld                 = FC_ILKN_TX_GEN_LLFC_ON_CH_0f;
        every_16_chs_fld           = FC_ILKN_TX_GEN_LLFC_EVERY_16_CHSf;
    }

    ilkn_bit_offset = ilkn_ndx;
    rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, on_ch0_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
    buffer |= SOC_SAND_SET_FLD_IN_PLACE(
                  info->cal_channel == SOC_TMC_FC_ILKN_CAL_LLFC_CH_0 ? 1 : 0,
                  ilkn_bit_offset,
                  SOC_SAND_BIT(ilkn_bit_offset)
                );
    rv = soc_reg_above_64_field32_modify(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, on_ch0_fld,  buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, every_16_chs_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
    buffer |= SOC_SAND_SET_FLD_IN_PLACE(
                  info->cal_channel == SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N ? 1 : 0,
                  ilkn_bit_offset,
                  SOC_SAND_BIT(ilkn_bit_offset)
                );
    rv = soc_reg_above_64_field32_modify(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, every_16_chs_fld,  buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    
    buffer = info->multi_use_mask;
    COMPILER_64_SET(field64,0, buffer);
    rv = soc_reg_above_64_field64_modify(unit, multiple_use_bits_reg, REG_PORT_ANY, reg_arr_index, multiple_use_bits_fld,  field64);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_ilkn_llfc_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (ilkn_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn_ndx is out of range")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Defines if and how LLFC can be received/generated using
 *     Interlaken NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t
  jer_fc_ilkn_llfc_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *gen_info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        ilkn_bit_offset;
    uint32
        buffer;
    soc_reg_t
        ilkn_llfc_reg = INVALIDr,
        multiple_use_bits_reg = INVALIDr;
    soc_field_t 
        rx_on_ch0_fld = INVALIDf, 
        rx_every_16_chs_fld = INVALIDf,
        rx_multiple_use_bits_fld = INVALIDf,
        tx_on_ch0_fld = INVALIDf, 
        tx_every_16_chs_fld = INVALIDf,
        tx_multiple_use_bits_fld = INVALIDf;
    int reg_arr_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(rec_info);
    SOCDNX_NULL_CHECK(gen_info);

    rv = jer_fc_ilkn_llfc_get_verify(unit, ilkn_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_TMC_FC_ILKN_LLFC_INFO_clear(rec_info);
    SOC_TMC_FC_ILKN_LLFC_INFO_clear(gen_info);

    ilkn_llfc_reg = NBIH_FC_ILKNr;
    multiple_use_bits_reg = NBIH_ILKN_MULTIPLE_USE_BITSr;
    rx_multiple_use_bits_fld = ILKN_RX_N_LLFC_STOP_TX_FROM_MULTIPLE_USE_BITS_MASKf;
    tx_multiple_use_bits_fld = ILKN_TX_N_MULTIPLE_USE_BITS_FROM_LLFC_MASKf;
    
    rx_on_ch0_fld                 = FC_ILKN_RX_LLFC_ON_CH_0f;
    rx_every_16_chs_fld           = FC_ILKN_RX_LLFC_EVERY_16_CHSf;
    tx_on_ch0_fld                 = FC_ILKN_TX_GEN_LLFC_ON_CH_0f;
    tx_every_16_chs_fld           = FC_ILKN_TX_GEN_LLFC_EVERY_16_CHSf;

    reg_arr_index = ilkn_ndx;
    ilkn_bit_offset = ilkn_ndx;

    /*
     *  Determine Rx type
     */
    rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, rx_on_ch0_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
    {
        rec_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_0;
    }
    else
    {
        rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, rx_every_16_chs_fld, &buffer);
        SOCDNX_IF_ERR_EXIT(rv);
        if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
        {
            rec_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N;
        }
        else
        {
            rec_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_NONE;
        }
    }

    /*
     *  Determine Tx type
     */
    rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, tx_on_ch0_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
    {
        gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_0;
    }
    else
    {
        rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, tx_every_16_chs_fld, &buffer);
        SOCDNX_IF_ERR_EXIT(rv);
        if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
        {
            gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N;
        }
        else
        {
            gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_NONE;
        }
    }

    rv = soc_reg_above_64_field32_read(unit, multiple_use_bits_reg, REG_PORT_ANY, reg_arr_index, rx_multiple_use_bits_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    rec_info->multi_use_mask = (uint8) buffer;
    rv = soc_reg_above_64_field32_read(unit, multiple_use_bits_reg, REG_PORT_ANY, reg_arr_index, tx_multiple_use_bits_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    gen_info->multi_use_mask = (uint8) buffer;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_ilkn_mub_channel_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID           ilkn_ndx
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (ilkn_ndx >  SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn_ndx is out of range")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_ilkn_mub_channel_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID           ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_DIRECTION           direction_ndx,
    SOC_SAND_OUT uint8                          bitmap
  )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_reg_t
        reg = CFC_ILKN_MUB_ENABLEr;
    soc_field_t rx_flds[6] = {
        ILKN_0_MUB_RX_ENAf,
        ILKN_1_MUB_RX_ENAf, 
        ILKN_2_MUB_RX_ENAf,
        ILKN_3_MUB_RX_ENAf,
        ILKN_4_MUB_RX_ENAf,
        ILKN_5_MUB_RX_ENAf};
    soc_field_t tx_flds[6] = {
        ILKN_0_MUB_TX_ENAf,
        ILKN_1_MUB_TX_ENAf, 
        ILKN_2_MUB_TX_ENAf,
        ILKN_3_MUB_TX_ENAf,
        ILKN_4_MUB_TX_ENAf,
        ILKN_5_MUB_TX_ENAf};
    soc_field_t fld;
    uint32 reg_data[1];

    SOCDNX_INIT_FUNC_DEFS;

    res = jer_fc_ilkn_mub_channel_verify(unit, ilkn_ndx);
    SOCDNX_IF_ERR_EXIT(res);

    /* Enable mapping to/from the ILKN Multi-USE-Bits */
    fld = (direction_ndx == SOC_TMC_FC_DIRECTION_GEN) ? tx_flds[ilkn_ndx] : rx_flds[ilkn_ndx];

    res = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  bitmap);
    SOCDNX_IF_ERR_EXIT(res);

    /* Enable/disable Multi-USE-Bits to use GenericBitmapsNif */
    if (direction_ndx == SOC_TMC_FC_DIRECTION_REC) {
        reg = CFC_ILKN_MUB_RX_IF_ENAr;
        res = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, reg_data);
        SOCDNX_IF_ERR_EXIT(res);

        if (bitmap != 0x0) {
            SHR_BITSET(reg_data, ilkn_ndx);
        }
        else {
            SHR_BITCLR(reg_data, ilkn_ndx);
        }

        res = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, *reg_data);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_ilkn_mub_channel_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID           ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_DIRECTION           direction_ndx,
    SOC_SAND_OUT uint8                          *bitmap
  )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_reg_t 
        reg = CFC_ILKN_MUB_ENABLEr;
    soc_field_t rx_flds[6] = {
        ILKN_0_MUB_RX_ENAf,
        ILKN_1_MUB_RX_ENAf, 
        ILKN_2_MUB_RX_ENAf,
        ILKN_3_MUB_RX_ENAf,
        ILKN_4_MUB_RX_ENAf,
        ILKN_5_MUB_RX_ENAf};
    soc_field_t tx_flds[6] = {
        ILKN_1_MUB_TX_ENAf,
        ILKN_1_MUB_TX_ENAf, 
        ILKN_2_MUB_TX_ENAf,
        ILKN_3_MUB_TX_ENAf,
        ILKN_4_MUB_TX_ENAf,
        ILKN_5_MUB_TX_ENAf};
    soc_field_t fld;
    soc_reg_above_64_val_t reg_data_64;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(bitmap);

    res = jer_fc_ilkn_mub_channel_verify(unit, ilkn_ndx);
    SOCDNX_IF_ERR_EXIT(res);

    SOC_REG_ABOVE_64_CLEAR(reg_data_64);
    /* get mapping to/from the ILKN Multi-USE-Bits */
    fld = (direction_ndx == SOC_TMC_FC_DIRECTION_GEN) ? tx_flds[ilkn_ndx] : rx_flds[ilkn_ndx];

    res = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_data_64);
    SOCDNX_IF_ERR_EXIT(res);

    (*bitmap) = soc_reg_above_64_field32_get(unit, reg, reg_data_64, fld);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_ilkn_mub_gen_cal_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (ilkn_ndx > SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn_ndx is out of range")));
    }

    SOCDNX_NULL_CHECK(cal_info);
 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_ilkn_mub_gen_cal_set (
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
  )
{
    soc_error_t
        res = SOC_E_NONE;
    uint32 
        calendar_regs[] = 
            {CFC_ILKN_0_MUB_TX_CALr, CFC_ILKN_1_MUB_TX_CALr, CFC_ILKN_2_MUB_TX_CALr, 
             CFC_ILKN_3_MUB_TX_CALr, CFC_ILKN_4_MUB_TX_CALr, CFC_ILKN_5_MUB_TX_CALr};
    uint32
        fields_0[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_0_MUB_TX_MAP_0f, ILKN_0_MUB_TX_MAP_1f, ILKN_0_MUB_TX_MAP_2f, ILKN_0_MUB_TX_MAP_3f,
            ILKN_0_MUB_TX_MAP_4f, ILKN_0_MUB_TX_MAP_5f, ILKN_0_MUB_TX_MAP_6f, ILKN_0_MUB_TX_MAP_7f};
    uint32
        fields_1[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_1_MUB_TX_MAP_0f, ILKN_1_MUB_TX_MAP_1f, ILKN_1_MUB_TX_MAP_2f, ILKN_1_MUB_TX_MAP_3f,
            ILKN_1_MUB_TX_MAP_4f, ILKN_1_MUB_TX_MAP_5f, ILKN_1_MUB_TX_MAP_6f, ILKN_1_MUB_TX_MAP_7f};
    uint32
        fields_2[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_2_MUB_TX_MAP_0f, ILKN_2_MUB_TX_MAP_1f, ILKN_2_MUB_TX_MAP_2f, ILKN_2_MUB_TX_MAP_3f,
            ILKN_2_MUB_TX_MAP_4f, ILKN_2_MUB_TX_MAP_5f, ILKN_2_MUB_TX_MAP_6f, ILKN_2_MUB_TX_MAP_7f};
    uint32
        fields_3[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_3_MUB_TX_MAP_0f, ILKN_3_MUB_TX_MAP_1f, ILKN_3_MUB_TX_MAP_2f, ILKN_3_MUB_TX_MAP_3f,
            ILKN_3_MUB_TX_MAP_4f, ILKN_3_MUB_TX_MAP_5f, ILKN_3_MUB_TX_MAP_6f, ILKN_3_MUB_TX_MAP_7f};
    uint32
        fields_4[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_4_MUB_TX_MAP_0f, ILKN_4_MUB_TX_MAP_1f, ILKN_4_MUB_TX_MAP_2f, ILKN_4_MUB_TX_MAP_3f,
            ILKN_4_MUB_TX_MAP_4f, ILKN_4_MUB_TX_MAP_5f, ILKN_4_MUB_TX_MAP_6f, ILKN_4_MUB_TX_MAP_7f};
    uint32
        fields_5[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_5_MUB_TX_MAP_0f, ILKN_5_MUB_TX_MAP_1f, ILKN_5_MUB_TX_MAP_2f, ILKN_5_MUB_TX_MAP_3f,
            ILKN_5_MUB_TX_MAP_4f, ILKN_5_MUB_TX_MAP_5f, ILKN_5_MUB_TX_MAP_6f, ILKN_5_MUB_TX_MAP_7f};
    soc_reg_above_64_val_t  
        cal_data, entry_data;
    uint32
        entry_idx;
    uint32
        ilkn_id_offset = ilkn_ndx;
    uint32
        source_fld_offset = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = jer_fc_ilkn_mub_gen_cal_verify(unit, ilkn_ndx, cal_info);
    SOCDNX_IF_ERR_EXIT(res);
    
    SOC_REG_ABOVE_64_CLEAR(cal_data);

    source_fld_offset = soc_mem_field_length(unit, CFC_OOB_TX_0_CALm, FC_INDEXf);

    /* Set calendar data */
    for(entry_idx = 0; entry_idx < SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN; entry_idx++)
    {
        /* init */
        SOC_REG_ABOVE_64_CLEAR(entry_data);
        /* set source (bits 11:9) */
        entry_data[0] = jer_fc_gen_cal_src_type_to_val_internal(cal_info->entries[entry_idx].source) << source_fld_offset;
        /* set index (bits 8:0) */
        entry_data[0] |= (cal_info->entries[entry_idx].id & SOC_SAND_BITS_MASK(source_fld_offset - 1, 0));

        /* update the calendar data with the entry*/
        if(ilkn_ndx == 0) {
            soc_reg_above_64_field_set(unit, calendar_regs[ilkn_id_offset], cal_data, fields_0[entry_idx], entry_data);
        } else if(ilkn_ndx == 1){
            soc_reg_above_64_field_set(unit, calendar_regs[ilkn_id_offset], cal_data, fields_1[entry_idx], entry_data);
        } else if(ilkn_ndx == 2){
            soc_reg_above_64_field_set(unit, calendar_regs[ilkn_id_offset], cal_data, fields_2[entry_idx], entry_data);
        } else if(ilkn_ndx == 3){
            soc_reg_above_64_field_set(unit, calendar_regs[ilkn_id_offset], cal_data, fields_3[entry_idx], entry_data);
        } else if(ilkn_ndx == 4){
            soc_reg_above_64_field_set(unit, calendar_regs[ilkn_id_offset], cal_data, fields_4[entry_idx], entry_data);
        } else if(ilkn_ndx == 5){
            soc_reg_above_64_field_set(unit, calendar_regs[ilkn_id_offset], cal_data, fields_5[entry_idx], entry_data);
        }
    }
  
    /* Write the calendar */
    res = soc_reg_above_64_set(unit, calendar_regs[ilkn_id_offset], REG_PORT_ANY, 0, entry_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_ilkn_mub_gen_cal_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_OUT SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
  )
{
    soc_error_t
        res = SOC_E_NONE;
    uint32 
        calendar_regs[] = 
            {CFC_ILKN_0_MUB_TX_CALr, CFC_ILKN_1_MUB_TX_CALr, CFC_ILKN_2_MUB_TX_CALr, 
             CFC_ILKN_3_MUB_TX_CALr, CFC_ILKN_4_MUB_TX_CALr, CFC_ILKN_5_MUB_TX_CALr};
    uint32
        fields_0[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_0_MUB_TX_MAP_0f, ILKN_0_MUB_TX_MAP_1f, ILKN_0_MUB_TX_MAP_2f, ILKN_0_MUB_TX_MAP_3f,
            ILKN_0_MUB_TX_MAP_4f, ILKN_0_MUB_TX_MAP_5f, ILKN_0_MUB_TX_MAP_6f, ILKN_0_MUB_TX_MAP_7f};
    uint32
        fields_1[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_1_MUB_TX_MAP_0f, ILKN_1_MUB_TX_MAP_1f, ILKN_1_MUB_TX_MAP_2f, ILKN_1_MUB_TX_MAP_3f,
            ILKN_1_MUB_TX_MAP_4f, ILKN_1_MUB_TX_MAP_5f, ILKN_1_MUB_TX_MAP_6f, ILKN_1_MUB_TX_MAP_7f};
    uint32
        fields_2[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_2_MUB_TX_MAP_0f, ILKN_2_MUB_TX_MAP_1f, ILKN_2_MUB_TX_MAP_2f, ILKN_2_MUB_TX_MAP_3f,
            ILKN_2_MUB_TX_MAP_4f, ILKN_2_MUB_TX_MAP_5f, ILKN_2_MUB_TX_MAP_6f, ILKN_2_MUB_TX_MAP_7f};
    uint32
        fields_3[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_3_MUB_TX_MAP_0f, ILKN_3_MUB_TX_MAP_1f, ILKN_3_MUB_TX_MAP_2f, ILKN_3_MUB_TX_MAP_3f,
            ILKN_3_MUB_TX_MAP_4f, ILKN_3_MUB_TX_MAP_5f, ILKN_3_MUB_TX_MAP_6f, ILKN_3_MUB_TX_MAP_7f};
    uint32
        fields_4[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_4_MUB_TX_MAP_0f, ILKN_4_MUB_TX_MAP_1f, ILKN_4_MUB_TX_MAP_2f, ILKN_4_MUB_TX_MAP_3f,
            ILKN_4_MUB_TX_MAP_4f, ILKN_4_MUB_TX_MAP_5f, ILKN_4_MUB_TX_MAP_6f, ILKN_4_MUB_TX_MAP_7f};
    uint32
        fields_5[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_5_MUB_TX_MAP_0f, ILKN_5_MUB_TX_MAP_1f, ILKN_5_MUB_TX_MAP_2f, ILKN_5_MUB_TX_MAP_3f,
            ILKN_5_MUB_TX_MAP_4f, ILKN_5_MUB_TX_MAP_5f, ILKN_5_MUB_TX_MAP_6f, ILKN_5_MUB_TX_MAP_7f};
    soc_reg_above_64_val_t  
        cal_data, entry_data;
    uint32
        entry_idx;
    uint32
        ilkn_id_offset = ilkn_ndx;
    uint32
        source_fld_offset = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = jer_fc_ilkn_mub_gen_cal_verify(unit, ilkn_ndx, cal_info);
    SOCDNX_IF_ERR_EXIT(res);

    SOC_REG_ABOVE_64_CLEAR(cal_data);

    source_fld_offset = soc_mem_field_length(unit, CFC_OOB_TX_0_CALm, FC_INDEXf);

    /* Read the calendar */
    res = soc_reg_above_64_get(unit, calendar_regs[ilkn_id_offset], REG_PORT_ANY, 0, entry_data);
    SOCDNX_IF_ERR_EXIT(res);

    /* Get calendar data */
    for(entry_idx = 0; entry_idx < SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN; entry_idx++)
    {
        if(ilkn_ndx == 0) {
            soc_reg_above_64_field_get(unit, calendar_regs[ilkn_id_offset], cal_data, fields_0[entry_idx], entry_data);
        } else if(ilkn_ndx == 1){
            soc_reg_above_64_field_get(unit, calendar_regs[ilkn_id_offset], cal_data, fields_1[entry_idx], entry_data);
        } else if(ilkn_ndx == 2){
            soc_reg_above_64_field_get(unit, calendar_regs[ilkn_id_offset], cal_data, fields_2[entry_idx], entry_data);
        } else if(ilkn_ndx == 3){
            soc_reg_above_64_field_get(unit, calendar_regs[ilkn_id_offset], cal_data, fields_3[entry_idx], entry_data);
        } else if(ilkn_ndx == 4){
            soc_reg_above_64_field_get(unit, calendar_regs[ilkn_id_offset], cal_data, fields_4[entry_idx], entry_data);
        } else if(ilkn_ndx == 5){
            soc_reg_above_64_field_get(unit, calendar_regs[ilkn_id_offset], cal_data, fields_5[entry_idx], entry_data);
        }

        /* get source (bits 11:9) */
        cal_info->entries[entry_idx].source = jer_fc_gen_cal_src_arr[entry_data[0] >> source_fld_offset];

        /* get index (bits 8:0) */
        cal_info->entries[entry_idx].id = (entry_data[0] & SOC_SAND_BITS_MASK(source_fld_offset-1, 0));
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_pfc_generic_bitmap_verify(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   int       priority,
    SOC_SAND_IN   uint32    pfc_bitmap_index
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if ((priority < 0) || (priority > SOC_TMC_EGR_NOF_Q_PRIO_ARAD - 1)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if(pfc_bitmap_index >= SOC_DPP_DEFS_GET(unit, nof_fc_pfc_generic_bitmaps)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_pfc_generic_bitmap_set(
    SOC_SAND_IN   int                              unit,
    SOC_SAND_IN   int                              priority,
    SOC_SAND_IN   uint32                           pfc_bitmap_index,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t  
        pfc_map_data;
    soc_mem_t mem[] = {
        CFC_GENERIC_BITMAP_NIF_0_MAPm,
        CFC_GENERIC_BITMAP_NIF_1_MAPm,
        CFC_GENERIC_BITMAP_NIF_2_MAPm,
        CFC_GENERIC_BITMAP_NIF_3_MAPm,
        CFC_GENERIC_BITMAP_NIF_4_MAPm,
        CFC_GENERIC_BITMAP_NIF_5_MAPm,
        CFC_GENERIC_BITMAP_NIF_6_MAPm,
        CFC_GENERIC_BITMAP_NIF_7_MAPm            
        };          
    uint32 
        entry_offset = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = jer_fc_pfc_generic_bitmap_verify(unit, priority, pfc_bitmap_index);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(pfc_map_data);

    entry_offset = pfc_bitmap_index;

    /* set Generic Bitmap */
    rv = soc_mem_read(unit, mem[priority], MEM_BLOCK_ANY, entry_offset, pfc_map_data);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Update the Generic Bitmap */
    SHR_BITCOPY_RANGE(pfc_map_data, 0, pfc_bitmap->bitmap, 0, SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE);
    if (pfc_bitmap->core) {
        SHR_BITSET(pfc_map_data, SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE);
    }
    else {
        SHR_BITCLR(pfc_map_data, SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE);
    }

    rv = soc_mem_write(unit, mem[priority], MEM_BLOCK_ANY, entry_offset, pfc_map_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_pfc_generic_bitmap_get(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   int       priority,
    SOC_SAND_IN   uint32    pfc_bitmap_index,
    SOC_SAND_OUT   SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t  
        pfc_map_data;
    soc_mem_t mem[] = {
        CFC_GENERIC_BITMAP_NIF_0_MAPm,
        CFC_GENERIC_BITMAP_NIF_1_MAPm,
        CFC_GENERIC_BITMAP_NIF_2_MAPm,
        CFC_GENERIC_BITMAP_NIF_3_MAPm,
        CFC_GENERIC_BITMAP_NIF_4_MAPm,
        CFC_GENERIC_BITMAP_NIF_5_MAPm,
        CFC_GENERIC_BITMAP_NIF_6_MAPm,
        CFC_GENERIC_BITMAP_NIF_7_MAPm            
        };          
    uint32 entry_offset = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pfc_bitmap);
    rv = jer_fc_pfc_generic_bitmap_verify(unit, priority, pfc_bitmap_index);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(pfc_map_data);

    entry_offset = pfc_bitmap_index;

    /* Read Generic Bitmap */
    rv = soc_mem_read(unit, mem[priority], MEM_BLOCK_ANY, entry_offset, pfc_map_data);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Get the Generic Bitmap */
    SHR_BITCOPY_RANGE(pfc_bitmap->bitmap, 0, pfc_map_data, 0, SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE);
    pfc_bitmap->core = SHR_BITGET(pfc_map_data, SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_cat_2_tc_hcfc_bitmap_verify(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           tc,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (core > MAX_NUM_OF_CORES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("core is out of range")));
    }

    if ((tc < 0) || (tc > SOC_TMC_EGR_NOF_Q_PRIO_ARAD - 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("tc is out of range")));
    }

    SOCDNX_NULL_CHECK(hcfc_bitmap);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_cat_2_tc_hcfc_bitmap_set(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           tc,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    soc_error_t 
        rv = SOC_E_NONE;
    soc_mem_t
        mem = INVALIDm;          
    soc_reg_above_64_val_t
        map_data;
    uint32 
        entry_offset = 0,
        entry_length = 0;
    soc_reg_above_64_val_t 
        cat2tc_map_ena;
    soc_reg_t 
        cat2tc_map_ena_reg = INVALIDr;
    uint32
        bit_offset;

    SOCDNX_INIT_FUNC_DEFS;

    rv = jer_fc_cat_2_tc_hcfc_bitmap_verify(unit, core, tc, is_ocb_only, hcfc_bitmap);
    SOCDNX_IF_ERR_EXIT(rv);

    mem = (core == 0) ? CFC_CAT_2_TC_IQM_0_MAP_HCFCm : CFC_CAT_2_TC_IQM_1_MAP_HCFCm;
    entry_offset = tc + ((is_ocb_only == 1)?(8):(0));    

    cat2tc_map_ena_reg = CFC_CAT_2_TC_MAP_HCFC_ENAr;
    bit_offset = tc + ((is_ocb_only == 1)?(8):(0));

    entry_length = soc_mem_field_length(unit, mem, BITMAPf);

    SOC_REG_ABOVE_64_CLEAR(map_data);
    /* set mapping from CAT2+TC to HCFC vector */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
    SOCDNX_IF_ERR_EXIT(rv);

    SHR_BITCOPY_RANGE(map_data, 0, hcfc_bitmap->bitmap, 0, entry_length);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
    SOCDNX_IF_ERR_EXIT(rv);

    /* enable map from CAT2+TC to NIF PFC */
    SOC_REG_ABOVE_64_CLEAR(cat2tc_map_ena);
    rv = soc_reg_above_64_get(unit, cat2tc_map_ena_reg, REG_PORT_ANY, 0, cat2tc_map_ena);
    SOCDNX_IF_ERR_EXIT(rv);
        
    SHR_BITSET(cat2tc_map_ena, bit_offset);
        
    rv = soc_reg_above_64_set(unit, cat2tc_map_ena_reg, REG_PORT_ANY, 0, cat2tc_map_ena);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_cat_2_tc_hcfc_bitmap_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           tc,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t
        mem = INVALIDm;          
    soc_reg_above_64_val_t 
        map_data;
    uint32 
        entry_offset = 0,
        entry_length = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = jer_fc_cat_2_tc_hcfc_bitmap_verify(unit, core, tc, is_ocb_only, hcfc_bitmap);
    SOCDNX_IF_ERR_EXIT(rv);

    mem = (core == 0) ? CFC_CAT_2_TC_IQM_0_MAP_HCFCm : CFC_CAT_2_TC_IQM_1_MAP_HCFCm;
    entry_offset = tc + ((is_ocb_only == 1)?(8):(0));
    entry_length = soc_mem_field_length(unit, mem, BITMAPf);

    SOC_REG_ABOVE_64_CLEAR(map_data);
    /* get mapping from CAT2+TC to HCFC vector */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
    SOCDNX_IF_ERR_EXIT(rv);

    SHR_BITCOPY_RANGE(hcfc_bitmap->bitmap, 0, map_data, 0, entry_length);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_glb_hcfc_bitmap_verify(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           is_high_prio,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_IN   int                           pool_id,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (core > MAX_NUM_OF_CORES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("core is out of range")));
    }

    if ((pool_id != 0) && (pool_id != 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("pool_id is out of range")));
    }

    SOCDNX_NULL_CHECK(hcfc_bitmap);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_glb_hcfc_bitmap_set(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           is_high_prio,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_IN   int                           pool_id,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_mem_t 
        mem = INVALIDm;          
    soc_reg_above_64_val_t
        map_data;
    uint32 
        entry_offset = 0,
        entry_length = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = jer_fc_glb_hcfc_bitmap_verify(unit, core, is_high_prio, is_ocb_only, pool_id, hcfc_bitmap);
    SOCDNX_IF_ERR_EXIT(res);

    mem = (core == 0) ? CFC_GLB_RSC_IQM_0_MAPm : CFC_GLB_RSC_IQM_1_MAPm;
    entry_offset = ((is_ocb_only == 1)?(4):(0)) + 
                   ((pool_id == 1)?(2):(0)) + 
                   ((is_high_prio)?(1):(0));
    entry_length = soc_mem_field_length(unit, mem, BITMAPf);
        
    /* set mapping from CAT2+TC to HCFC vector */
    res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
    SOCDNX_IF_ERR_EXIT(res);

    SHR_BITCOPY_RANGE(map_data, 0, hcfc_bitmap->bitmap, 0, entry_length);

    res = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_glb_hcfc_bitmap_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           is_high_prio,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_IN   int                           pool_id,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_mem_t
        mem = INVALIDm;
    soc_reg_above_64_val_t
        map_data;
    uint32
        entry_offset = 0,
        entry_length = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = jer_fc_glb_hcfc_bitmap_verify(unit, core, is_high_prio, is_ocb_only, pool_id, hcfc_bitmap);
    SOCDNX_IF_ERR_EXIT(res);

    mem = (core == 0) ? CFC_GLB_RSC_IQM_0_MAPm : CFC_GLB_RSC_IQM_1_MAPm;
    entry_offset = ((is_ocb_only == 1)?(4):(0)) + 
                   ((pool_id == 1)?(2):(0)) + 
                   ((is_high_prio)?(1):(0));
    entry_length = soc_mem_field_length(unit, mem, BITMAPf);
        
    /* set mapping from CAT2+TC to HCFC vector */
    res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
    SOCDNX_IF_ERR_EXIT(res);

    SHR_BITCOPY_RANGE(hcfc_bitmap->bitmap, 0, map_data, 0, entry_length);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
jer_fc_rec_cal_dest_type_to_val_internal(
    SOC_SAND_IN SOC_TMC_FC_REC_CAL_DEST dest_type
  )
{
    uint32 ret;

    for(ret = 0; ret < JER_FC_REC_CAL_DEST_ARR_SIZE; ret++)
    {
        if(jer_fc_rec_cal_dest_arr[ret] == dest_type)
        {
            return ret;
        }
    }

    return -1;
}

uint32
jer_fc_gen_cal_src_type_to_val_internal(
    SOC_SAND_IN  SOC_TMC_FC_GEN_CAL_SRC     src_type
  )
{
    uint32 ret;

    for(ret = 0; ret < JER_FC_GEN_CAL_SRC_ARR_SIZE; ret++) {
        if(jer_fc_gen_cal_src_arr[ret] == src_type) {
            return ret;
        }
    }

    return -1;
}


soc_error_t
  jer_fc_pfc_mapping_verify(
      SOC_SAND_IN int  unit,
      SOC_SAND_IN uint32                       nif_id,
      SOC_SAND_IN uint32                       src_pfc_ndx,
      SOC_SAND_IN SOC_TMC_FC_PFC_MAP           *pfc_map
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (src_pfc_ndx > (SOC_TMC_EGR_NOF_Q_PRIO_ARAD - 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("src pfc index is out of range")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_pfc_mapping_set(
      SOC_SAND_IN int  unit,
      SOC_SAND_IN uint32                        nif_id,
      SOC_SAND_IN uint32                        src_pfc_ndx,
      SOC_SAND_IN SOC_TMC_FC_PFC_MAP            *pfc_map
    )
{
    soc_error_t
        res = SOC_E_NONE;
    uint32
        nif_pfc_data[3] = {0};
    uint32
        entry_offset, 
        field_offset;
    uint32 index_field[] = 
        {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f, INDEX_4f, INDEX_5f, INDEX_6f, INDEX_7f};
    uint32 select_field[] = 
        {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f, SELECT_4f, SELECT_5f, SELECT_6f, SELECT_7f};
    uint32 valid_field[] = 
        {VALID_0f, VALID_1f, VALID_2f, VALID_3f, VALID_4f, VALID_5f, VALID_6f, VALID_7f};
    uint32 
        select_data = 0,
        valid = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = jer_fc_pfc_mapping_verify(unit, nif_id, src_pfc_ndx, pfc_map);
    SOCDNX_IF_ERR_EXIT(res);

    entry_offset = nif_id;
    field_offset = src_pfc_ndx;

    if (pfc_map->mode == SOC_TMC_FC_PFC_MAP_EGQ) {
        select_data = 0;
    }
    else {
        select_data = 1;
    }
    valid = pfc_map->valid;

    res = soc_mem_read(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
    SOCDNX_IF_ERR_EXIT(res);

    soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, index_field[field_offset], pfc_map->index);
    soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, select_field[field_offset], select_data);
    soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, valid_field[field_offset], valid);

    res = soc_mem_write(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
    SOCDNX_IF_ERR_EXIT(res);
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_pfc_mapping_get(
      SOC_SAND_IN int  unit,
      SOC_SAND_IN uint32                        nif_id,
      SOC_SAND_IN uint32                        src_pfc_ndx,
      SOC_SAND_OUT SOC_TMC_FC_PFC_MAP           *pfc_map
    )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_reg_above_64_val_t 
        nif_pfc_data;
    uint32 
        entry_offset, 
        field_offset;
    uint32 index_field[] = 
        {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f, INDEX_4f, INDEX_5f, INDEX_6f, INDEX_7f};
    uint32 select_field[] = 
        {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f, SELECT_4f, SELECT_5f, SELECT_6f, SELECT_7f};
    uint32 valid_field[] = 
        {VALID_0f, VALID_1f, VALID_2f, VALID_3f, VALID_4f, VALID_5f, VALID_6f, VALID_7f};
    uint32 select_data = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = jer_fc_pfc_mapping_verify(unit, nif_id, src_pfc_ndx, pfc_map);
    SOCDNX_IF_ERR_EXIT(res);

    entry_offset = nif_id;
    field_offset = src_pfc_ndx;

    if (pfc_map->mode == SOC_TMC_FC_PFC_MAP_EGQ) {
        select_data = 0;
    }
    else {
        select_data = 1;
    }

    SOC_REG_ABOVE_64_CLEAR(nif_pfc_data);
    res = soc_mem_read(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
    SOCDNX_IF_ERR_EXIT(res);

    pfc_map->index = soc_mem_field32_get(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, index_field[field_offset]);
    select_data = soc_mem_field32_get(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, select_field[field_offset]);
    pfc_map->valid = soc_mem_field32_get(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, valid_field[field_offset]);

    if (select_data == 0) {
        pfc_map->mode = SOC_TMC_FC_PFC_MAP_EGQ;
    }
    else {
        pfc_map->mode = SOC_TMC_FC_PFC_MAP_GENERIC_BITMAP;
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_inbnd_mode_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_IN int                           is_generate,
      SOC_SAND_IN SOC_TMC_FC_INBND_MODE         mode
      )
{
    soc_error_t
        rv = SOC_E_NONE;    
    portmod_pause_control_t 
        pause_control;
    portmod_llfc_control_t  
        llfc_control;
    portmod_pfc_control_t   
        pfc_control;

    SOCDNX_INIT_FUNC_DEFS;

    /* 1. setting MAC registers - port level { */
    /* Enable / Disable - LL */
    rv = portmod_port_pause_control_get(unit, port, &pause_control);
    SOCDNX_IF_ERR_EXIT(rv);
    if (is_generate) {
        pause_control.tx_enable = SOC_SAND_BOOL2NUM(mode == SOC_TMC_FC_INBND_MODE_LL);
    }
    else {
        pause_control.rx_enable = SOC_SAND_BOOL2NUM(mode == SOC_TMC_FC_INBND_MODE_LL);
    }
    rv = portmod_port_pause_control_set(unit, port, &pause_control);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Enable / Disable - PFC */  
    rv = portmod_port_pfc_control_get(unit, port, &pfc_control);
    SOCDNX_IF_ERR_EXIT(rv);
    if (is_generate) {
        pfc_control.tx_enable = SOC_SAND_BOOL2NUM(mode == SOC_TMC_FC_INBND_MODE_PFC);
    }
    else {
        pfc_control.rx_enable = SOC_SAND_BOOL2NUM(mode == SOC_TMC_FC_INBND_MODE_PFC);
    }
    rv = portmod_port_pfc_control_set(unit, port, &pfc_control);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Enable / Disable - SAFC */
    rv = portmod_port_llfc_control_get(unit, port, &llfc_control);
    SOCDNX_IF_ERR_EXIT(rv);
    if (is_generate) {
        llfc_control.tx_enable = SOC_SAND_BOOL2NUM(mode == SOC_TMC_FC_INBND_MODE_SAFC);
    }
    else {
        llfc_control.rx_enable = SOC_SAND_BOOL2NUM(mode == SOC_TMC_FC_INBND_MODE_SAFC);
    }
    rv = portmod_port_llfc_control_set(unit, port, &llfc_control);
    SOCDNX_IF_ERR_EXIT(rv);    
    /* 1. setting MAC registers - port level } */

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_inbnd_mode_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_IN int                           is_generate,
      SOC_SAND_OUT SOC_TMC_FC_INBND_MODE        *mode
    )
{
    soc_error_t 
        rv = SOC_E_NONE;
    uint32 
        ll_enable = 0, 
        pfc_enable = 0, 
        safc_enable = 0;
    portmod_pause_control_t
        pause_control;
    portmod_llfc_control_t
        llfc_control;
    portmod_pfc_control_t
        pfc_control;

    SOCDNX_INIT_FUNC_DEFS;

    /* 1. Check which mode is enabled { */
    /* LL */
    rv = portmod_port_pause_control_get(unit, port, &pause_control);
    SOCDNX_IF_ERR_EXIT(rv);
    ll_enable = (is_generate) ? pause_control.tx_enable : pause_control.rx_enable;
    rv = portmod_port_pause_control_set(unit, port, &pause_control);
    SOCDNX_IF_ERR_EXIT(rv);

    /* PFC */  
    rv = portmod_port_pfc_control_get(unit, port, &pfc_control);
    SOCDNX_IF_ERR_EXIT(rv);
    pfc_enable = (is_generate) ? pfc_control.tx_enable : pfc_control.rx_enable;
    rv = portmod_port_pfc_control_set(unit, port, &pfc_control);
    SOCDNX_IF_ERR_EXIT(rv);

    /* SAFC */
    rv = portmod_port_llfc_control_get(unit, port, &llfc_control);
    SOCDNX_IF_ERR_EXIT(rv);
    safc_enable = (is_generate) ? llfc_control.tx_enable : llfc_control.rx_enable;
    rv = portmod_port_llfc_control_set(unit, port, &llfc_control);
    SOCDNX_IF_ERR_EXIT(rv);
    /* 1. Check which mode is enabled } */

    /* 2. set the Inband Mode according to the register values { */
    if (ll_enable)
    {
        (*mode) = SOC_TMC_FC_INBND_MODE_LL;
    }
    else if(pfc_enable)
    {
        (*mode) = SOC_TMC_FC_INBND_MODE_PFC;
    }
    else if(safc_enable)
    {
        (*mode) = SOC_TMC_FC_INBND_MODE_SAFC;
    }
    else
    {
        (*mode) = SOC_TMC_FC_INBND_MODE_DISABLED;
    }
    /* 2. set the Inband Mode according to the register values } */
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_glb_rcs_mask_verify(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN int                           core,
      SOC_SAND_IN int                           is_high_prio,
      SOC_SAND_IN SOC_TMC_FC_GLB_RES_TYPE       glb_res_dst
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    /* Place your code here */
    if (core > MAX_NUM_OF_CORES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("core is out of range")));
    }
    
    if (glb_res_dst < SOC_TMC_FC_GLB_RES_TYPE_MIX_P0 || glb_res_dst < SOC_TMC_FC_GLB_RES_TYPE_OCB_P1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("global resouce dest is error")));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_glb_rcs_mask_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN int                           core,
      SOC_SAND_IN int                           is_high_prio,
      SOC_SAND_IN SOC_TMC_FC_GLB_RES_TYPE       glb_res_dst,
      SOC_SAND_IN uint32                        glb_res_src_bitmap
    )
{
    soc_error_t res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data;
    soc_reg_t reg = CFC_GLB_RSC_IQM_MASKr;
    soc_field_t field = INVALIDf;
    uint32 field_value = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = jer_fc_glb_rcs_mask_verify(unit, core, is_high_prio, glb_res_dst);
    SOCDNX_IF_ERR_EXIT(res);

    if (glb_res_src_bitmap & (1<<SOC_TMC_FC_GLB_RES_TYPE_BDB)) {
        field_value |= 0x1;
    }
    if (glb_res_src_bitmap & (1<<SOC_TMC_FC_GLB_RES_TYPE_MINI_MC_DB)) {
        field_value |= 0x2;            
    }
    if (glb_res_src_bitmap & (1<<SOC_TMC_FC_GLB_RES_TYPE_MC_DB)) {
        field_value |= 0x4;            
    }
    if (glb_res_src_bitmap & (1<<SOC_TMC_FC_GLB_RES_TYPE_OCB_DB)) {
        field_value |= 0x8;            
    } 

    switch (glb_res_dst) {
    case SOC_TMC_FC_GLB_RES_TYPE_MIX_P0:
        if (is_high_prio) {
            field = (core == 0) ? GLB_RSC_MIX_POOL_0_IQM_0_HP_MASKf : GLB_RSC_MIX_POOL_0_IQM_1_HP_MASKf;
        }
        else {
            field = (core == 0) ? GLB_RSC_MIX_POOL_0_IQM_0_LP_MASKf : GLB_RSC_MIX_POOL_0_IQM_1_LP_MASKf;            
        }       
        if (glb_res_src_bitmap & (1<<SOC_TMC_FC_GLB_RES_TYPE_MIX_P0)) {
            field_value |= 0x10;
        }
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_MIX_P1:
        if (is_high_prio) {
            field = (core == 0) ? GLB_RSC_MIX_POOL_1_IQM_0_HP_MASKf : GLB_RSC_MIX_POOL_1_IQM_1_HP_MASKf;
        }
        else {
            field = (core == 0) ? GLB_RSC_MIX_POOL_1_IQM_0_LP_MASKf : GLB_RSC_MIX_POOL_1_IQM_1_LP_MASKf;            
        }       
        if (glb_res_src_bitmap & (1<<SOC_TMC_FC_GLB_RES_TYPE_MIX_P1)) {
            field_value |= 0x10;
        }
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_OCB_P0:
        if (is_high_prio) {
            field = (core == 0) ? GLB_RSC_OCB_POOL_0_IQM_0_HP_MASKf : GLB_RSC_OCB_POOL_0_IQM_1_HP_MASKf;
        }
        else {
            field = (core == 0) ? GLB_RSC_OCB_POOL_0_IQM_0_LP_MASKf : GLB_RSC_OCB_POOL_0_IQM_1_LP_MASKf;            
        }       
        if (glb_res_src_bitmap & (1<<SOC_TMC_FC_GLB_RES_TYPE_OCB_P0)) {
            field_value |= 0x10;
        }
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_OCB_P1:
        if (is_high_prio) {
            field = (core == 0) ? GLB_RSC_OCB_POOL_1_IQM_0_HP_MASKf: GLB_RSC_OCB_POOL_1_IQM_1_HP_MASKf;
        }
        else {
            field = (core == 0) ? GLB_RSC_OCB_POOL_1_IQM_0_LP_MASKf: GLB_RSC_OCB_POOL_1_IQM_1_LP_MASKf;            
        }       
        if (glb_res_src_bitmap & (1<<SOC_TMC_FC_GLB_RES_TYPE_OCB_P1)) {
            field_value |= 0x10;
        }
        break;
    default:
        break;
    }

    res = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(res);
        
    soc_reg_above_64_field32_set(unit, reg, reg_data, field, field_value);    
        
    res = soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(res);
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fc_glb_rcs_mask_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN int                           core,
      SOC_SAND_IN int                           is_high_prio,
      SOC_SAND_IN SOC_TMC_FC_GLB_RES_TYPE       glb_res_dst,
      SOC_SAND_OUT uint32                       *glb_res_src_bitmap
    )
{
    soc_error_t res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data;
    soc_reg_t reg = CFC_GLB_RSC_IQM_MASKr;
    soc_field_t field = INVALIDf;
    uint32 field_value = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = jer_fc_glb_rcs_mask_verify(unit, core, is_high_prio, glb_res_dst);
    SOCDNX_IF_ERR_EXIT(res);
    
    SOCDNX_NULL_CHECK(glb_res_src_bitmap);

    switch (glb_res_dst) {
    case SOC_TMC_FC_GLB_RES_TYPE_MIX_P0:
        if (is_high_prio) {
            field = (core == 0) ? GLB_RSC_MIX_POOL_0_IQM_0_HP_MASKf : GLB_RSC_MIX_POOL_0_IQM_1_HP_MASKf;
        }
        else {
            field = (core == 0) ? GLB_RSC_MIX_POOL_0_IQM_0_LP_MASKf : GLB_RSC_MIX_POOL_0_IQM_1_LP_MASKf;            
        }
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_MIX_P1:
        if (is_high_prio) {
            field = (core == 0) ? GLB_RSC_MIX_POOL_1_IQM_0_HP_MASKf : GLB_RSC_MIX_POOL_1_IQM_1_HP_MASKf;
        }
        else {
            field = (core == 0) ? GLB_RSC_MIX_POOL_1_IQM_0_LP_MASKf : GLB_RSC_MIX_POOL_1_IQM_1_LP_MASKf;            
        }
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_OCB_P0:
        if (is_high_prio) {
            field = (core == 0) ? GLB_RSC_OCB_POOL_0_IQM_0_HP_MASKf : GLB_RSC_OCB_POOL_0_IQM_1_HP_MASKf;
        }
        else {
            field = (core == 0) ? GLB_RSC_OCB_POOL_0_IQM_0_LP_MASKf : GLB_RSC_OCB_POOL_0_IQM_1_LP_MASKf;            
        }
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_OCB_P1:
        if (is_high_prio) {
            field = (core == 0) ? GLB_RSC_OCB_POOL_1_IQM_0_HP_MASKf: GLB_RSC_OCB_POOL_1_IQM_1_HP_MASKf;
        }
        else {
            field = (core == 0) ? GLB_RSC_OCB_POOL_1_IQM_0_LP_MASKf: GLB_RSC_OCB_POOL_1_IQM_1_LP_MASKf;            
        }
        break;
    default:
        break;
    }

    res = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(res);
        
    field_value = soc_reg_above_64_field32_get(unit, reg, reg_data, field);

    if (field_value & 0x1) {
        (*glb_res_src_bitmap) |= SOC_TMC_FC_GLB_RES_TYPE_BDB;
    }
    if (field_value & 0x2) {
        (*glb_res_src_bitmap) |= SOC_TMC_FC_GLB_RES_TYPE_MINI_MC_DB;
    }
    if (field_value & 0x4) {
        (*glb_res_src_bitmap) |= SOC_TMC_FC_GLB_RES_TYPE_MC_DB;
    }
    if (field_value & 0x8) {
        (*glb_res_src_bitmap) |= SOC_TMC_FC_GLB_RES_TYPE_OCB_DB;
    } 
    if (field_value & 0x10) {
        (*glb_res_src_bitmap) |= glb_res_dst;
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

