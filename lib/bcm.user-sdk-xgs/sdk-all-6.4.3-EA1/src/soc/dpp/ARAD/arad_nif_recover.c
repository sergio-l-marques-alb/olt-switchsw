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
 */

#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <bcm/error.h>
#include <soc/types.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/mcm/memregs.h>
#include <shared/util.h>
#include <soc/dpp/port_sw_db.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT


typedef struct bist_flow_descriptor_s {
        uint32 num_of_bursts;
        uint32 data_type;
        uint32 Flow0CreateErr;
        uint32 Flow0Class;
        uint32 Flow0ConstantCh;
        uint32 Flow0MinBurstSize;
        uint32 Flow0BurstSizeMask;
        uint32 Flow0IncrementalBurstSize;             
}bist_flow_descriptor_t;

STATIC int (*dynamic_port_recovery_sequence[SOC_MAX_NUM_DEVICES])(int unit, soc_pbmp_t *pbmp, int* rv) = {NULL};

STATIC int arad_nif_recover_bist_main(int unit, uint32 bist_mode, uint32 bist_ch_mode, uint32 bist_context_mask, int *rv);


/* function: control_tx
 * purpose: control traffic to and from the XLPort
 * args:    is_stop: 1 - to stop tx
 *                   0 - to resume tx
 *  
 */
STATIC int arad_nif_recover_control_tx(int unit, soc_pbmp_t* pbmp, uint32 is_stop)
{
    uint64 reg;
    bcm_port_t port;

    SOCDNX_INIT_FUNC_DEFS;

    PBMP_ITER(*pbmp, port) {
        SOCDNX_IF_ERR_EXIT(READ_XLP_REG_062500r(unit, port, &reg));
        soc_reg64_field32_set(unit, XLP_REG_062500r, &reg, FIELD_0_0f, is_stop);
        SOCDNX_IF_ERR_EXIT(WRITE_XLP_REG_062500r(unit, port, reg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* Reset the XLP Block*/
STATIC int arad_nif_recover_reset_port(int unit)
{
    soc_reg_above_64_val_t reg;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg));
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg, XLP_0_INITf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg));

    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg));
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg, XLP_0_INITf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg));
		
exit:
    SOCDNX_FUNC_RETURN;
}

/* Send packets using BIST*/
STATIC int arad_nif_recover_send_packets_bist(int unit, soc_pbmp_t* pbmp, int *rv){
    bcm_port_t port_i;
    uint32 mask = 0;
    uint32 phy_port;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ITER(*pbmp, port_i) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port_i, &phy_port /*one based*/));
        phy_port--;
        SHR_BITSET(&mask, phy_port);
    }

    SOCDNX_IF_ERR_EXIT(arad_nif_recover_bist_main(unit, 0, 0, mask, rv));

exit:
    SOCDNX_FUNC_RETURN;
}


/* Check the XLP FIFO Counters.
 * Returns 0 if counters values are not as expected.*/
STATIC int arad_nif_recover_check_counters(int unit, soc_pbmp_t* pbmp, int *counters_match){
    uint32 val1, val2, val3, val4;
    bcm_port_t port;
    SOCDNX_INIT_FUNC_DEFS;
   
    *counters_match = 1;

    PBMP_ITER(*pbmp, port) {
        SOCDNX_IF_ERR_EXIT(READ_PORT_TXFIFO_CELL_REQ_CNTr(unit, port, &val1)); 
        SOCDNX_IF_ERR_EXIT(READ_PORT_TXFIFO_CELL_CNTr(unit, port, &val2));
        SOCDNX_IF_ERR_EXIT(READ_PORT_CDC_TXFIFO_CELL_REQ_CNTr(unit, port, &val3));
        SOCDNX_IF_ERR_EXIT(READ_PORT_CDC_TXFIFO_CELL_CNTr(unit, port, &val4));
    
    	if( (val1 != val3) || (val2 != val4) )
    	{
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "PORT_TXFIFO_CELL_REQ_CNT %d\n"), val1));
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "PORT_TXFIFO_CELL_CNT %d\n"), val2));
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "PORT_CDC_TXFIFO_CELL_REQ_CNT %d\n"), val3));
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "PORT_CDC_TXFIFO_CELL_CNT %d\n"), val4));
            *counters_match = 0;
            break;
    	}
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int arad_nif_recover_bist_run_test(int unit, uint32 bist_mode, uint32 bist_ch_mode, uint32 bist_context_mask, uint64* bist_tx_burst_th,
              bist_flow_descriptor_t bist_flow_descriptor_0,
              bist_flow_descriptor_t bist_flow_descriptor_1,
              bist_flow_descriptor_t bist_flow_descriptor_2,
              bist_flow_descriptor_t bist_flow_descriptor_3,
              int* status)
{
    int counter;
    uint32 bist_tx_done = 0;
    uint32 bist_rx_done = 0;
    uint32 bist_status;
    uint64 bist_rx_ok_bursts_cnt;
    uint64 bist_tx_bursts_cnt;
    uint32 bist_rx_counters_1, bist_rx_counters_2, bist_rx_counters_3;
    uint32 bist_tx_crc_err_cnt;
    uint32 reg;
    uint64 reg64;
    char uint64_hexa_string_1[32];
    char uint64_hexa_string_2[32];

    SOCDNX_INIT_FUNC_DEFS;

    if (status){
        *status = 1;
        _shr_format_uint64_hexa_string(*bist_tx_burst_th, uint64_hexa_string_1); 
        LOG_INFO(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "bist_test start configuration bist_mode = %d, bist_ch_mode = %d, bist_context_mask = %x, bist_tx_burst_th = %s\n"),
                  bist_mode, bist_ch_mode, bist_context_mask, uint64_hexa_string_1));
    }

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_GENERAL_CONFIGURATIONr(unit, &reg));
    soc_reg_field_set(unit, NBI_BIST_GENERAL_CONFIGURATIONr, &reg, BIST_GENERATOR_RSTNf, 0);
    soc_reg_field_set(unit, NBI_BIST_GENERAL_CONFIGURATIONr, &reg, BIST_MODEf, bist_mode); 
    soc_reg_field_set(unit, NBI_BIST_GENERAL_CONFIGURATIONr, &reg, BIST_CH_CHOSING_METHODf, bist_ch_mode);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_GENERAL_CONFIGURATIONr(unit, reg));
    
    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_CONTEXTS_MASKr(unit, &reg););
    soc_reg_field_set(unit, NBI_BIST_CONTEXTS_MASKr, &reg, BIST_CONTEXTS_MASKf, bist_context_mask);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_CONTEXTS_MASKr(unit, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_TX_BURSTS_THRESHOLDr(unit, &reg64));
    soc_reg64_field_set(unit, NBI_BIST_TX_BURSTS_THRESHOLDr, &reg64, BIST_TX_BURST_THf, *bist_tx_burst_th);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_TX_BURSTS_THRESHOLDr(unit, reg64));
    

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 0, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_NUM_OF_BURSTSf, bist_flow_descriptor_0.num_of_bursts);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_DATA_TYPEf, bist_flow_descriptor_0.data_type);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CREATE_ERRf, bist_flow_descriptor_0.Flow0CreateErr);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CLASSf, bist_flow_descriptor_0.Flow0Class);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CONSTANT_CHf, bist_flow_descriptor_0.Flow0ConstantCh);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 0, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 0, &reg)); 
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_MIN_BURST_SIZEf, bist_flow_descriptor_0.Flow0MinBurstSize);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_BURST_SIZE_MASKf, bist_flow_descriptor_0.Flow0BurstSizeMask);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_INCREMENTAL_BURST_SIZEf, bist_flow_descriptor_0.Flow0IncrementalBurstSize);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 0, reg));


    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 1, &reg)); 
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_NUM_OF_BURSTSf, bist_flow_descriptor_1.num_of_bursts);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_DATA_TYPEf, bist_flow_descriptor_1.data_type);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CREATE_ERRf, bist_flow_descriptor_1.Flow0CreateErr);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CLASSf, bist_flow_descriptor_1.Flow0Class);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CONSTANT_CHf, bist_flow_descriptor_1.Flow0ConstantCh);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 1, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 1, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_MIN_BURST_SIZEf, bist_flow_descriptor_1.Flow0MinBurstSize);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_BURST_SIZE_MASKf, bist_flow_descriptor_1.Flow0BurstSizeMask);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_INCREMENTAL_BURST_SIZEf, bist_flow_descriptor_1.Flow0IncrementalBurstSize);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 1, reg));


    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 2, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_NUM_OF_BURSTSf, bist_flow_descriptor_2.num_of_bursts);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_DATA_TYPEf, bist_flow_descriptor_2.data_type);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CREATE_ERRf, bist_flow_descriptor_2.Flow0CreateErr);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CLASSf, bist_flow_descriptor_2.Flow0Class);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CONSTANT_CHf, bist_flow_descriptor_2.Flow0ConstantCh);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 2, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 2, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_MIN_BURST_SIZEf, bist_flow_descriptor_2.Flow0MinBurstSize);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_BURST_SIZE_MASKf, bist_flow_descriptor_2.Flow0BurstSizeMask);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_INCREMENTAL_BURST_SIZEf, bist_flow_descriptor_2.Flow0IncrementalBurstSize);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 2, reg));

 
    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 3, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_NUM_OF_BURSTSf, bist_flow_descriptor_3.num_of_bursts);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_DATA_TYPEf, bist_flow_descriptor_3.data_type);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CREATE_ERRf, bist_flow_descriptor_3.Flow0CreateErr);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CLASSf, bist_flow_descriptor_3.Flow0Class);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CONSTANT_CHf, bist_flow_descriptor_3.Flow0ConstantCh);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 3, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 3, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_MIN_BURST_SIZEf, bist_flow_descriptor_3.Flow0MinBurstSize);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_BURST_SIZE_MASKf, bist_flow_descriptor_3.Flow0BurstSizeMask);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_INCREMENTAL_BURST_SIZEf, bist_flow_descriptor_3.Flow0IncrementalBurstSize);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 3, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_GENERAL_CONFIGURATIONr(unit, &reg));
    soc_reg_field_set(unit, NBI_BIST_GENERAL_CONFIGURATIONr, &reg, BIST_GENERATOR_RSTNf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_GENERAL_CONFIGURATIONr(unit, reg));

     /* if BIST test output status is required - check all relevant counters */
    if (status != NULL) {

        LOG_INFO( BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "bist_test start generator & checker\n")));
        LOG_INFO( BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "bist_test wait for tx_done\n")));
        counter = 0;
        while ((bist_tx_done != 1) && (counter < 400)) 
        {      
            READ_NBI_BIST_STATUSr(unit, &bist_status);
            bist_tx_done = bist_status & 0x00000001;
            ++counter;
        }

        if (counter < 200) {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist_test TX_DONE\n")));
        }
        else {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR TX FAIL\n")));
        }

        LOG_INFO( BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "bist_test wait for rx_done\n")));
        counter = 0;
        while ((bist_rx_done != 1) && (counter < 400))
        {
            READ_NBI_BIST_STATUSr(unit, &bist_status);
            bist_rx_done = (bist_status >> 4) & 0x00000001;
            counter++;
        }
        if (counter < 200) {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist_test RX_DONE\n")));
        }
        else {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR RX FAIL\n")));
        }
   
        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_RX_OK_BURSTS_COUNTERr(unit, &bist_rx_ok_bursts_cnt)); 
        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_TX_BURSTS_COUNTERr(unit, &bist_tx_bursts_cnt));

        _shr_format_uint64_hexa_string(bist_rx_ok_bursts_cnt, uint64_hexa_string_1);
        _shr_format_uint64_hexa_string(bist_tx_bursts_cnt, uint64_hexa_string_2);

        if (COMPILER_64_NE(bist_rx_ok_bursts_cnt , bist_tx_bursts_cnt)) 
        {

            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR number_of_ok_packets received %s does not match number_of_packets_transmitted %s\n"), 
                                 uint64_hexa_string_1, uint64_hexa_string_2));
            *status = 0;
        }
        else
        {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist number_of_ok_packets received %s match number_of_packets_transmitted %s\n"),
                                 uint64_hexa_string_1, uint64_hexa_string_2));
        }

        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_RX_COUNTERS_1r(unit, &bist_rx_counters_1));
        if (bist_rx_counters_1 > 0) 
        {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR bist_rx_counters_1 %x reports on errors\n"),bist_rx_counters_1));
            *status = 0;     
        }

        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_RX_COUNTERS_2r(unit, &bist_rx_counters_2));
        if (bist_rx_counters_2 > 0) 
        {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR bist_rx_counters_2 %x reports on errors\n"),bist_rx_counters_2));
            *status = 0; 
        }

        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_RX_COUNTERS_3r(unit, &bist_rx_counters_3));
        if (bist_rx_counters_3 > 0) 
        {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR bist_rx_counters_3 %x reports on errors\n"),bist_rx_counters_3));
            *status = 0;   
        }

        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_TX_ADDITIONAL_COUNTERr(unit, &bist_tx_crc_err_cnt));
        bist_tx_crc_err_cnt &= 0x0000FFFF; /* bits 15:0 */
        if (bist_tx_crc_err_cnt > 0) 
        {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR bist_tx_crc_err_cnt %x reports on errors\n"), bist_tx_crc_err_cnt));
            *status = 0;    
        }

        if (*status == 1) {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist_test: PASS\n")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int arad_nif_recover_bist_main(int unit, uint32 bist_mode, uint32 bist_ch_mode, uint32 bist_context_mask, int *rv){

    uint64 bist_tx_burst_th;
    bist_flow_descriptor_t bist_flow_descriptor_tmp;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_SET(bist_tx_burst_th, 0, 100);
    
    /* for 2XILKN(14 channels each)-bist ch mode =3,100 packets,size=64 */
    bist_flow_descriptor_tmp.num_of_bursts = 1;
    bist_flow_descriptor_tmp.data_type = 0;             /* <0> - Constant, <1>-Incremental, <2>-Flipped */
    bist_flow_descriptor_tmp.Flow0CreateErr = 0;        /* <1> - insert error */
    bist_flow_descriptor_tmp.Flow0Class = 0xF;          /* <0...7> - COS, <f> - no COS */
    bist_flow_descriptor_tmp.Flow0ConstantCh = 0;        
    bist_flow_descriptor_tmp.Flow0MinBurstSize = 0x40;  /* Min Burst Size */
    bist_flow_descriptor_tmp.Flow0BurstSizeMask = 0;    /* Burst Size Mask */
    bist_flow_descriptor_tmp.Flow0IncrementalBurstSize = 0; /* Incremental Burst Size */
    
    SOCDNX_IF_ERR_EXIT(arad_nif_recover_bist_run_test(unit, bist_mode, bist_ch_mode, bist_context_mask, &bist_tx_burst_th, bist_flow_descriptor_tmp, bist_flow_descriptor_tmp,
            bist_flow_descriptor_tmp, bist_flow_descriptor_tmp, rv));

exit:
    SOCDNX_FUNC_RETURN;
}

/*clear genral configuration register and counters after Bist test has completed*/
STATIC int arad_nif_recover_clean_up(int unit, soc_pbmp_t* pbmp){

    soc_port_t port;
    uint32 reg;
    soc_reg_above_64_val_t reg64;

    SOCDNX_INIT_FUNC_DEFS;
    
    SOC_PBMP_ITER(*pbmp, port) {
       SOCDNX_IF_ERR_EXIT(READ_PORT_MIB_RESETr(unit, port, &reg));
       soc_reg_field_set(unit, PORT_MIB_RESETr, &reg, CLR_CNTf, 0xF);
       SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, port, reg));

       SOCDNX_IF_ERR_EXIT(READ_PORT_MIB_RESETr(unit, port, &reg));
       soc_reg_field_set(unit, PORT_MIB_RESETr, &reg, CLR_CNTf, 0);
       SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, port, reg));

       /*one port is enough- hence the break*/
       break;
    }

    /*Reset NBI block*/
    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg64));
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg64, NBI_INITf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg64));

    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg64));
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg64, NBI_INITf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg64));

exit:
    SOCDNX_FUNC_RETURN;
}

/*Reset all XLP ports*/		
STATIC int arad_nif_recover_reset_xlp_ports(int unit, soc_pbmp_t* pbmp){

    soc_port_t port;
    uint32 reg;

    SOCDNX_INIT_FUNC_DEFS;
    
    SOC_PBMP_ITER(*pbmp, port) {
        SOCDNX_IF_ERR_EXIT(READ_XLP_XMAC_CTRLr(unit, port, &reg));
        soc_reg_field_set(unit, XLP_XMAC_CTRLr, &reg, SOFT_RESETf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_XLP_XMAC_CTRLr(unit, port, reg));
        
        SOCDNX_IF_ERR_EXIT(READ_XLP_XMAC_CTRLr(unit, port, &reg));
        soc_reg_field_set(unit, XLP_XMAC_CTRLr, &reg, SOFT_RESETf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_XLP_XMAC_CTRLr(unit, port, reg));
    } 

exit:
    SOCDNX_FUNC_RETURN;
}



/* run the recovery test for XLP ports */
int arad_nif_recover_run_recovery_test(int unit, soc_pbmp_t* pbmp, int iterations){

    int i = 0, counters_match;
    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i <= iterations; ++i) {

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_control_tx(unit, pbmp, 1)); 

        if (dynamic_port_recovery_sequence[unit]) {

            SOCDNX_IF_ERR_EXIT(dynamic_port_recovery_sequence[unit](unit, pbmp, NULL));
        }else{
            SOCDNX_IF_ERR_EXIT(arad_nif_recover_send_packets_bist(unit, pbmp, NULL)); 
        }
        SOCDNX_IF_ERR_EXIT(arad_nif_recover_check_counters(unit, pbmp, &counters_match));

        SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_GENERAL_CONFIGURATIONr(unit, 0));    

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_reset_xlp_ports(unit, pbmp));

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_control_tx(unit, pbmp, 0));

        if (counters_match) {
            if (i>0) {
                LOG_INFO( BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "XLP recovered withing %d iterations\n"), i)); 
            }
            break;
        } else {
            if (i<iterations) {
                LOG_INFO( BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "XLP failed to recover, trying to recover, iteration %d/%d\n"), i, iterations)); 
                SOCDNX_IF_ERR_EXIT(arad_nif_recover_reset_port(unit));
                SOCDNX_IF_ERR_EXIT(arad_nif_recover_clean_up(unit, pbmp));
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL,(_BSL_SOCDNX_MSG("XLP failed to recover\n")));
            }
        } 
    }
    SOCDNX_IF_ERR_EXIT(arad_nif_recover_clean_up(unit, pbmp));

exit:
    SOCDNX_FUNC_RETURN;

}

#undef _ERR_MSG_MODULE_NAME
