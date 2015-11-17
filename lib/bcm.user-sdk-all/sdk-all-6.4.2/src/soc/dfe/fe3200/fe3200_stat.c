/*
 * $Id: fe3200_stat.c,v 1.1.2.2 Broadcom SDK $
 *
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
 * SOC FE3200 STAT
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_STAT
#include <shared/bsl.h>
#include <soc/dfe/fe3200/fe3200_stat.h>
#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <shared/bitop.h>
#include <soc/dcmn/error.h>
#include <bcm/stat.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/dfe/fe1600/fe1600_stat.h>

soc_controlled_counter_t soc_fe3200_controlled_counter[] = {
    {
        soc_fe1600_controlled_counter_get,
        soc_fe3200_counters_tx_control_cells_counter,
        "TX Control cells",
        "TX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
	{
        soc_fe1600_controlled_counter_get,
        soc_fe3200_counters_tx_data_cell_counter,
        "TX Data cell",
        "TX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
	{
        soc_fe1600_controlled_counter_get,
        soc_fe3200_counters_tx_data_byte_counter,
        "TX Data byte",
        "TX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_COLLECTED
    },
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_crc_errors_data_cells,
		"RX CRC errors",
		"RX CRC errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_crc_errors_control_cells_nonbypass,
		"RX CRC errors nonbypass",
		"RX CRC errors nonbypass",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_crc_errors_control_cells_bypass,
		"RX CRC errors bypass",
		"RX CRC errors bypass",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_fec_correctable_error,
		"RX FEC error",
		"RX FEC error",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_control_cells_counter,
		"RX Control cells",
		"RX Control cells",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_data_cell_counter,
		"RX Data cell",
		"RX Data cell",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_data_byte_counter,
		"RX Data byte ",
		"RX Data byte",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_dropped_retransmitted_control,
		"RX Dropped retransmitted control",
		"RX Dropped retransmitted control",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_tx_asyn_fifo_rate,
		"TX Asyn fifo rate",
		"TX Asyn fifo rate",
		_SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_asyn_fifo_rate,
		"RX Asyn fifo rate",
		"RX Asyn fifo rate",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_lfec_fec_uncorrrectable_errors,
		"RX Lfec uncorrectable errors",
		"RX Lfec uncorrectable errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_llfc_primary_pipe,
		"RX Llfc primary pipe",
		"RX Llfc primary pipe",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_llfc_second_pipe,
		"RX Llfc second pipe",
		"RX Llfc second pipe",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_llfc_third_pipe,
		"RX Llfc third pipe",
		"RX Llfc third pipe",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_kpcs_errors_counter,
		"RX Kpcs errors",
		"RX Kpcs errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_COLLECTED
	},
	{
		soc_fe1600_controlled_counter_get,
		soc_fe3200_counters_rx_kpcs_bypass_errors_counter,
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
 
 
soc_error_t
soc_fe3200_controlled_counter_set(int unit)    /*?????*/
{
	SOCDNX_INIT_FUNC_DEFS;
	SOC_CONTROL(unit)->controlled_counters = soc_fe3200_controlled_counter;


    SOCDNX_FUNC_RETURN;

}

/*
 * Function:
 *      soc_3200_stat_counters_init
 * Purpose:
 *      Init FE3200 stat
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      SOC_E_XXX         Operation failed
 */
soc_error_t 
soc_fe3200_stat_init(int unit){

    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, 0, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_COUNTER_MODEf, 0);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_BYTE_COUNTER_HEADERf, 1);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, COUNTER_CLEAR_ON_READf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, reg_val));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_mapping_stat_get(int unit,bcm_port_t port, int *counters,int *array_size ,bcm_stat_val_t type,int max_array_size)
{
	SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(counters);
    SOCDNX_NULL_CHECK(array_size);
    switch(type) {
    case snmpBcmTxControlCells:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_fe3200_counters_tx_control_cells_counter;
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
            counters[0] = soc_fe3200_counters_tx_data_cell_counter;
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
            counters[0] = soc_fe3200_counters_tx_data_byte_counter;
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
            counters[0] = soc_fe3200_counters_rx_crc_errors_data_cells;
            counters[1] = soc_fe3200_counters_rx_crc_errors_control_cells_nonbypass;
            counters[2] = soc_fe3200_counters_rx_crc_errors_control_cells_bypass;
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
            counters[0] = soc_fe3200_counters_rx_fec_correctable_error;
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
            counters[0] = soc_fe3200_counters_rx_control_cells_counter;
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
            counters[0] = soc_fe3200_counters_rx_data_cell_counter;
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
            counters[0] = soc_fe3200_counters_rx_data_byte_counter;
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
            counters[0] = soc_fe3200_counters_rx_dropped_retransmitted_control;
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
            counters[0] = soc_fe3200_counters_tx_asyn_fifo_rate;
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
            counters[0] = soc_fe3200_counters_rx_asyn_fifo_rate;
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
            counters[0] = soc_fe3200_counters_rx_lfec_fec_uncorrrectable_errors;
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
#undef _ERR_MSG_MODULE_NAME
