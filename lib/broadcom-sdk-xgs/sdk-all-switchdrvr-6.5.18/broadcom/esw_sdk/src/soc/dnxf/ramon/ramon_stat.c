/*
 * $Id: ramon_stat.c,v 1.1.2.2 Broadcom SDK $
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC RAMON STAT
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_STAT

#include <shared/bsl.h>
#include <soc/dnxf/ramon/ramon_stat.h>
#include <soc/dnxf/ramon/ramon_port.h>
#include <soc/error.h>
#include <soc/dnxc/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <shared/bitop.h>
#include <soc/dnxc/error.h>
#include <bcm/stat.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/dnxc_ha.h>

/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>

soc_controlled_counter_t soc_ramon_controlled_counter[] = {
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_tx_control_cells_counter,
        "TX Control cells",
        "TX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_tx_data_cell_counter,
        "TX Data cell",
        "TX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_tx_data_byte_counter,
        "TX Data byte",
        "TX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_crc_errors_data_cells,
        "RX CRC errors",
        "RX CRC errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_crc_errors_control_cells_nonbypass,
        "RX CRC errors nonbypass",
        "RX CRC errors nonbypass",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_crc_errors_control_cells_bypass,
        "RX CRC errors bypass",
        "RX CRC errors bypass",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_fec_correctable_error,
        "RX FEC correctable error",
        "RX FEC correctable error",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_control_cells_counter,
        "RX Control cells",
        "RX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_data_cell_counter,
        "RX Data cell",
        "RX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_data_byte_counter,
        "RX Data byte ",
        "RX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_dropped_retransmitted_control,
        "RX Dropped retransmitted control",
        "RX Dropped retransmitted control",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_dummy_val_1,
        "Dummy value",
        "Dummy value",
        _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_dummy_val_2,
        "Dummy value",
        "Dummy value",
        _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_tx_asyn_fifo_rate,
        "TX Asyn fifo rate",
        "TX Asyn fifo rate",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_asyn_fifo_rate,
        "RX Asyn fifo rate",
        "RX Asyn fifo rate",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_lfec_fec_uncorrrectable_errors,
        "RX FEC uncorrectable errors",
        "RX FEC uncorrectable errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_llfc_primary_pipe,
        "RX Llfc primary pipe",
        "RX Llfc primary pipe",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_llfc_second_pipe,
        "RX Llfc second pipe",
        "RX Llfc second pipe",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_llfc_third_pipe,
        "RX Llfc third pipe",
        "RX Llfc third pipe",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_kpcs_errors_counter,
        "RX Kpcs errors",
        "RX Kpcs errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_kpcs_bypass_errors_counter,
        "RX Kpcs bypass errors",
        "RX Kpcs bypass errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_rs_fec_bit_error_counter,
        "RX RS Fec bit errors",
        "RX RS Fec bit errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_rs_fec_symbol_error_counter,
        "RX RS Fec sybmol errors",
        "RX RS Fec sybmol errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        NULL,
        -1,
        "",
        "",
        0,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    }
};
 
 
shr_error_e
soc_ramon_controlled_counter_set(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SOC_CONTROL(unit)->controlled_counters = soc_ramon_controlled_counter;

    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_3200_stat_counters_init
 * Purpose:
 *      Init RAMON stat
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      _SHR_E_XXX         Operation failed
 */
shr_error_e 
soc_ramon_stat_init(int unit){

    uint32 reg_val;
    int fmac_index = 0;
    soc_pbmp_t enabled_fmacs_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_ramon_port_enabled_fmac_blocks_get(unit, &enabled_fmacs_bmp));

    if (!SOC_WARM_BOOT(unit)) {
        SOC_PBMP_ITER(enabled_fmacs_bmp, fmac_index)
        {
            SHR_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, fmac_index, &reg_val));
            soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_COUNTER_MODEf, 0);
            soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_BYTE_COUNTER_HEADERf, 1);
            soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, COUNTER_CLEAR_ON_READf, 1);
            SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, fmac_index, reg_val));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_mapping_stat_get(int unit,bcm_port_t port, int *counters,int *array_size ,bcm_stat_val_t type,int max_array_size)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(counters, _SHR_E_PARAM, "counters");
    SHR_NULL_CHECK(array_size, _SHR_E_PARAM, "array_size");
    switch(type) {
    case snmpBcmTxControlCells:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_tx_control_cells_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmTxDataCells:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_tx_data_cell_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmTxDataBytes:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_tx_data_byte_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxCrcErrors:
        *array_size=3;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_rx_crc_errors_data_cells;
            counters[1] = soc_ramon_counters_rx_crc_errors_control_cells_nonbypass;
            counters[2] = soc_ramon_counters_rx_crc_errors_control_cells_bypass;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxFecCorrectable:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_rx_fec_correctable_error;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxControlCells:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_rx_control_cells_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxDataCells:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_rx_data_cell_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxDataBytes:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_rx_data_byte_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxDroppedRetransmittedControl:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_rx_dropped_retransmitted_control;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmTxAsynFifoRate:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_tx_asyn_fifo_rate;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxAsynFifoRate:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_rx_asyn_fifo_rate;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxFecUncorrectable:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_rx_lfec_fec_uncorrrectable_errors;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxRsFecBitError:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_rx_rs_fec_bit_error_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxRsFecSymbolError:
        *array_size=1;
        if (*array_size<=max_array_size)  /* checking that we haven't exceeded max_array_size */
        {
            counters[0] = soc_ramon_counters_rx_rs_fec_symbol_error_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    default:
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "port: %d, stat counter %d isn't supported",port, type); 
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_stat_counter_length_get(int unit, int counter_id, int *length)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (counter_id)
    {
        case soc_ramon_counters_rx_crc_errors_control_cells_nonbypass:
        case soc_ramon_counters_rx_crc_errors_control_cells_bypass:
        case soc_ramon_counters_rx_kpcs_errors_counter:
        case soc_ramon_counters_rx_kpcs_bypass_errors_counter:
            *length = 16;
            break;

        case soc_ramon_counters_rx_crc_errors_data_cells:
        case soc_ramon_counters_rx_fec_correctable_error:
        case soc_ramon_counters_tx_asyn_fifo_rate:
        case soc_ramon_counters_rx_asyn_fifo_rate:
        case soc_ramon_counters_rx_lfec_fec_uncorrrectable_errors:
            *length = 32;
            break;

        case soc_ramon_counters_tx_control_cells_counter:
        case soc_ramon_counters_tx_data_cell_counter:
        case soc_ramon_counters_tx_data_byte_counter:
        case soc_ramon_counters_rx_control_cells_counter:
        case soc_ramon_counters_rx_data_cell_counter:
        case soc_ramon_counters_rx_data_byte_counter:
        case soc_ramon_counters_rx_rs_fec_bit_error_counter:
        case soc_ramon_counters_rx_rs_fec_symbol_error_counter:
            *length = 48;
            break;

        case soc_ramon_counters_rx_llfc_primary_pipe:
        case soc_ramon_counters_rx_llfc_second_pipe:
        case soc_ramon_counters_rx_llfc_third_pipe:
            *length = 64;
            break;

        default:
            *length = 0;
            break;
    }


    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_stat_is_supported_type
 * Purpose:
 *      Check whether type supported by unit
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      type  - (IN)     SNMP statistics type defined in bcm_stat_val_t
 * Returns:
 *      _SHR_E_NONE        Success  
 *      _SHR_E_UNAVAIL     Counter not supported for the port
 *      _SHR_E_XXX         Operation failed
 */
shr_error_e 
soc_ramon_stat_is_supported_type(int unit, bcm_port_t port, bcm_stat_val_t type)
{
    bcm_port_resource_t resource;
    SHR_FUNC_INIT_VARS(unit);

    switch(type) {
    case snmpBcmTxControlCells:
    case snmpBcmTxDataCells:
    case snmpBcmTxDataBytes:
    case snmpBcmRxCrcErrors:
    case snmpBcmRxControlCells:
    case snmpBcmRxDataCells:
    case snmpBcmRxDataBytes:
    case snmpBcmRxDroppedRetransmittedControl:
    case snmpBcmTxAsynFifoRate:
    case snmpBcmRxAsynFifoRate:
        break;

    case snmpBcmRxRsFecBitError:
    case snmpBcmRxRsFecSymbolError:
        SHR_IF_ERR_EXIT(soc_dnxc_port_resource_get(unit, port, &resource));
        if(bcmPortPhyFecRs108 != resource.fec_type &&
           bcmPortPhyFecRs206 != resource.fec_type &&
           bcmPortPhyFecRs304 != resource.fec_type &&
           bcmPortPhyFecRs545 != resource.fec_type)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "port: %d, counter %d supported only for RS FEC ports", port, type); 
        }
        break;

    /*Counters for FEC ports only*/
    case snmpBcmRxFecCorrectable:
    case snmpBcmRxFecUncorrectable:
        SHR_IF_ERR_EXIT(soc_dnxc_port_resource_get(unit, port, &resource));
        if(bcmPortPhyFecBaseR != resource.fec_type &&
           bcmPortPhyFecRs108 != resource.fec_type &&
           bcmPortPhyFecRs206 != resource.fec_type &&
           bcmPortPhyFecRs304 != resource.fec_type &&
           bcmPortPhyFecRs545 != resource.fec_type)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "port: %d, counter %d supported only for FEC ports",port, type); 
        }
        break;

    default:
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "port: %d, stat type %d isn't supported",port, type); 
    }

exit:
    SHR_FUNC_EXIT;
}

int soc_ramon_controlled_counter_get(int unit, int counter_id, int port, uint64* val, uint32* clear_on_read){

    int blk_idx, lane_idx,nof_links_in_fmac;
    uint32 control_reg;
    uint64 mask;
    int length;
    int  rc = _SHR_E_NONE;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (clear_on_read) {
        *clear_on_read = TRUE;
    }
    nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
    blk_idx = INT_DEVIDE(port, nof_links_in_fmac);
    lane_idx = port % nof_links_in_fmac;

    DNXF_UNIT_LOCK_TAKE(unit);
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, &control_reg));
    soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, LANE_SELECTf, lane_idx);
    soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, COUNTER_SELECTf, counter_id);

    /*
     * Temporarily allow HW writes for current thread even if it's generally disabled.
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    WRITE_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, control_reg);
    /*
     * revert dnxc_allow_hw_write_enable.
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    SHR_IF_ERR_EXIT(rc);
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUTr(unit, blk_idx, val));

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stat_counter_length_get, (unit, counter_id, &length)));
    
    COMPILER_64_MASK_CREATE(mask, length, 0);
    COMPILER_64_AND((*val), mask);
    

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_stat_get
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number 
 *      counters - (IN) array of counters to be summed
 *      array size- (IN) size of the counters array
 *      value - (OUT)    Counter value 
 * Returns:
 *      _SHR_E_NONE       Success.  
 *      _SHR_E_PARAM      Illegal parameter.  
 *      _SHR_E_BADID      Illegal port number.  
 *      _SHR_E_INTERNAL   Device access failure.  
 *      _SHR_E_UNAVAIL    Counter/variable is not implemented on this current chip.  
 */

shr_error_e
soc_ramon_stat_get(int unit,bcm_port_t port,uint64 *value,int *counters,int arr_size)
{
    int i=0;
    int rc=0;
    uint64 val64;
    SHR_FUNC_INIT_VARS(unit);
    COMPILER_64_ZERO(*value);
    for (i=0;i<arr_size;i++)
    {
        if (COUNTER_IS_COLLECTED(SOC_CONTROL(unit)->controlled_counters[counters[i]])) {
            if (SOC_CONTROL(unit)->counter_interval != 0) {
                rc = soc_counter_get(unit, port, counters[i], 0, &val64); /*counter is collected by counter thread*/
            } else {
                /*counter isn't collected by counter thread*/
                rc = SOC_CONTROL(unit)->controlled_counters[counters[i]].controlled_counter_f(unit, SOC_CONTROL(unit)->controlled_counters[counters[i]].counter_id, port, &val64, NULL);
            }
        } else {
            /*counter isn't collected by counter thread*/
            rc = soc_ramon_controlled_counter_get(unit, counters[i], port, &val64, NULL);
        }
        SHR_IF_ERR_EXIT(rc);
        COMPILER_64_ADD_64(*value,val64);


    }
exit:
    SHR_FUNC_EXIT;

}

#undef BSL_LOG_MODULE
