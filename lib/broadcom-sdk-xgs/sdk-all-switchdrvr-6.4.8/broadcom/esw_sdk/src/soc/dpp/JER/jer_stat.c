/*
 * $Id: arad_stat.c,v 1.14 Broadcom SDK $
 *
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
 *
 * SOC ARAD FABRIC STAT
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_STAT

#include <soc/dpp/JER/jer_stat.h>


/*
 * Function:
 *      soc_jer_stat_nif_init(
 * Purpose:
 *      Init ARAD nif stat
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      SOC_E_XXX         Operation failed
 */
soc_error_t 
soc_jer_stat_nif_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

/*exit:*/
    SOCDNX_FUNC_RETURN;
}

soc_controlled_counter_t soc_jer_controlled_counter[] = {
    {
        soc_arad_mac_controlled_counter_get,
        soc_jer_counters_tx_control_cells_counter,
        "TX Control cells",
        "TX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
	{
        soc_arad_mac_controlled_counter_get,
        soc_jer_counters_tx_data_cell_counter,
        "TX Data cell",
        "TX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
	{
        soc_arad_mac_controlled_counter_get,
        soc_jer_counters_tx_data_byte_counter,
        "TX Data byte",
        "TX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_crc_errors_data_cells,
		"RX CRC errors",
		"RX CRC errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_crc_errors_control_cells_nonbypass,
		"RX CRC errors nonbypass",
		"RX CRC errors nonbypass",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_crc_errors_control_cells_bypass,
		"RX CRC errors bypass",
		"RX CRC errors bypass",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_fec_correctable_error,
		"RX FEC correctable",
		"RX FEC correctable",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_control_cells_counter,
		"RX Control cells",
		"RX Control cells",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_data_cell_counter,
		"RX Data cell",
		"RX Data cell",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_data_byte_counter,
		"RX Data byte ",
		"RX Data byte",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_dropped_retransmitted_control,
		"RX Dropped retransmitted control",
		"RX Dropped retransmitted control",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
    {
        soc_arad_mac_controlled_counter_get,
        soc_jer_counters_dummy_val_1,
        "Dummy value",
        "Dummy value",
        _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
        COUNTER_IDX_NOT_COLLECTED
    },
    {
        soc_arad_mac_controlled_counter_get,
        soc_jer_counters_dummy_val_2,
        "Dummy value",
        "Dummy value",
        _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
        COUNTER_IDX_NOT_COLLECTED
    },
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_tx_asyn_fifo_rate,
		"TX Asyn fifo rate",
		"TX Asyn fifo rate",
		_SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_asyn_fifo_rate,
		"RX Asyn fifo rate",
		"RX Asyn fifo rate",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_lfec_fec_uncorrrectable_errors,
		"RX FEC uncorrectable errors",
		"RX FEC uncorrectable errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_llfc_primary_pipe,
		"RX Llfc primary pipe",
		"RX Llfc primary pipe",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_llfc_second_pipe,
		"RX Llfc second pipe",
		"RX Llfc second pipe",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_llfc_third_pipe,
		"RX Llfc third pipe",
		"RX Llfc third pipe",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_kpcs_errors_counter,
		"RX Kpcs errors",
		"RX Kpcs errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_kpcs_bypass_errors_counter,
		"RX Kpcs bypass errors",
		"RX Kpcs bypass errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
        NULL,
        -1,
        "",
        "",
        0,
        COUNTER_IDX_NOT_COLLECTED
    }
};

/*
 * Function:
 *      soc_jer_fabric_stat_init
 * Purpose:
 *      Init jer stat
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      SOC_E_XXX         Operation failed
 */
soc_error_t 
soc_jer_fabric_stat_init(int unit){

    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_QAX(unit)) {  
    if (!SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, 0, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_COUNTER_MODEf, 0);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_BYTE_COUNTER_HEADERf, 1);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, COUNTER_CLEAR_ON_READf, 1);
        /*write to broadcast instead of updating each instance individually*/
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, reg_val));
    }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_jer_mapping_stat_get(int unit,soc_port_t port, uint32 *counters,int *array_size ,bcm_stat_val_t type,int max_array_size)
{
	SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(counters);
    SOCDNX_NULL_CHECK(array_size);
    switch(type) {
    case snmpBcmTxControlCells:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_tx_control_cells_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmTxDataCells:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_tx_data_cell_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    case snmpBcmTxDataBytes:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_tx_data_byte_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    case snmpBcmRxCrcErrors:
        *array_size=3;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_rx_crc_errors_data_cells;
            counters[1] = soc_jer_counters_rx_crc_errors_control_cells_nonbypass;
            counters[2] = soc_jer_counters_rx_crc_errors_control_cells_bypass;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmRxFecCorrectable:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_rx_fec_correctable_error;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    case snmpBcmRxControlCells:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_rx_control_cells_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    case snmpBcmRxDataCells:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_rx_data_cell_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    case snmpBcmRxDataBytes:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_rx_data_byte_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    case snmpBcmRxDroppedRetransmittedControl:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_rx_dropped_retransmitted_control;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    case snmpBcmTxAsynFifoRate:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_tx_asyn_fifo_rate;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    case snmpBcmRxAsynFifoRate:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_rx_asyn_fifo_rate;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    case snmpBcmRxFecUncorrectable:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_jer_counters_rx_lfec_fec_uncorrrectable_errors;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, stat counter %d isn't supported"),port, type)); 
    }
exit:
	SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_jer_stat_counter_length_get(int unit, int counter_id, int *length)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (counter_id)
    {
        case soc_jer_counters_rx_crc_errors_control_cells_nonbypass:
        case soc_jer_counters_rx_crc_errors_control_cells_bypass:
        case soc_jer_counters_rx_lfec_fec_uncorrrectable_errors:
        case soc_jer_counters_rx_kpcs_errors_counter:
        case soc_jer_counters_rx_kpcs_bypass_errors_counter:
            *length = 16;
            break;

        case soc_jer_counters_rx_crc_errors_data_cells:
        case soc_jer_counters_rx_fec_correctable_error:
        case soc_jer_counters_tx_asyn_fifo_rate:
        case soc_jer_counters_rx_asyn_fifo_rate:
            *length = 32;
            break;

        case soc_jer_counters_tx_control_cells_counter:
        case soc_jer_counters_tx_data_cell_counter:
        case soc_jer_counters_tx_data_byte_counter:
        case soc_jer_counters_rx_control_cells_counter:
        case soc_jer_counters_rx_data_cell_counter:
        case soc_jer_counters_rx_data_byte_counter:
            *length = 48;
            break;

        case soc_jer_counters_rx_llfc_primary_pipe:
        case soc_jer_counters_rx_llfc_second_pipe:
        case soc_jer_counters_rx_llfc_third_pipe:
            *length = 64;
            break;

        default:
            *length = 0;
            break;
    }


	SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
