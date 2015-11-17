/*
 * $Id: stat.c,v 1.30 Broadcom SDK $
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
 * SOC_PETRA STAT
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_STAT
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm/stat.h>
#include <soc/defs.h>
#include <bcm_int/dpp/stat.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/counters.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/gport_mgmt.h>
#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#endif /*BCM_PETRAB_SUPPORT*/

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_stat.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#endif /*BCM_ARAD_SUPPORT*/


#include <soc/dcmn/dcmn_wb.h>

#include <soc/drv.h>

#include <soc/error.h>

#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/mbcm.h>




/*
 * Function:
 *      bcm_petra_stat_init
 * Purpose:
 *      Initialize the BCM statistics module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      BCM_E_NONE        Success  
 *      BCM_E_INTERNAL    Device access failure  
 */
int 
bcm_petra_stat_init(
    int unit)
{
    int rc = BCM_E_NONE;
    int interval;
    uint32 flags;
    pbmp_t pbmp;
    soc_port_t          p;
    soc_control_t       *soc = SOC_CONTROL(unit);
    bcm_pbmp_t ports_to_remove;
    bcm_port_t port_base;
    int phy_port;
    BCMDNX_INIT_FUNC_DEFS;

    rc = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_fabric_init, (unit));
    BCMDNX_IF_ERR_EXIT(rc);
    if (!SOC_IS_ARDON(unit)) {
        rc = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_nif_init, (unit));
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if (soc_property_get_str(unit, spn_BCM_STAT_PBMP) == NULL) {
        SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
    } else {
        pbmp = soc_property_get_pbmp(unit, spn_BCM_STAT_PBMP, 0);
    }

    SOC_PBMP_CLEAR(ports_to_remove);
    /*remove channelized ports which is not port base*/
    PBMP_ITER(pbmp, p) {
        /*get port base for channelized case */
        phy_port = SOC_INFO(unit).port_l2p_mapping[p];
        port_base = SOC_INFO(unit).port_p2l_mapping[phy_port];
        if (!SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, p)  && p != port_base /*nchannelized and not port_Base*/) {
            SOC_PBMP_PORT_ADD(ports_to_remove, p);
        }
    }

    SOC_PBMP_REMOVE(pbmp, ports_to_remove);

    interval = (SAL_BOOT_BCMSIM) ? 25000000 : 1000000;
    interval = soc_property_get(unit, spn_BCM_STAT_INTERVAL, interval);
    flags = soc_property_get(unit, spn_BCM_STAT_FLAGS, 0);

    rc = soc_counter_start(unit, flags, interval, pbmp);
    BCMDNX_IF_ERR_EXIT(rc);

    /*update counters bitmap in case counter thread is not start*/
    if (interval == 0) {
        SOC_PBMP_ASSIGN(soc->counter_pbmp, pbmp);
        PBMP_ITER(soc->counter_pbmp, p) {
            if ((SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), p))) {
                SOC_PBMP_PORT_REMOVE(soc->counter_pbmp, p);
            }
            if (IS_LB_PORT(unit, p)) {
                SOC_PBMP_PORT_REMOVE(soc->counter_pbmp, p);
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stat_stop
 * Purpose:
 *      Deinit the BCM statistics module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      BCM_E_NONE        Success
 *      BCM_E_INTERNAL    Device access failure
 */
int
bcm_petra_stat_stop(int unit)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rc = soc_counter_stop(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stat_clear
 * Purpose:
 *      Clear the port-based statistics for the indicated device port
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number 
 * Returns:
 *      BCM_E_NONE        Success  
 *      BCM_E_INTERNAL    Device access failure  
 */
int 
bcm_petra_stat_clear(
    int unit, 
    bcm_port_t port)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    if (BCM_PBMP_MEMBER(PBMP_CMIC(unit), port)) {
        /* Rudimentary CPU statistics -- needs soc_reg_twork */
        SOC_CONTROL(unit)->stat.dma_rbyt = 0;
        SOC_CONTROL(unit)->stat.dma_rpkt = 0;
        SOC_CONTROL(unit)->stat.dma_tbyt = 0;
        SOC_CONTROL(unit)->stat.dma_tpkt = 0;
        BCM_EXIT;
    }

    if ( _SOC_CONTROLLED_COUNTER_USE(unit, port)){
        rv = soc_controlled_counter_clear(unit, port);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (!_SOC_CONTROLLED_COUNTER_USE(unit, port) && soc_feature(unit, soc_feature_generic_counters)) {
        bcm_port_t port_base;
        int phy_port;
        pbmp_t        pbm;

        /*get port base for channelized case */
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        port_base = SOC_INFO(unit).port_p2l_mapping[phy_port];

        SOC_PBMP_CLEAR(pbm);
        SOC_PBMP_PORT_ADD(pbm, port_base);
        BCM_IF_ERROR_RETURN(soc_counter_set32_by_port(unit, pbm, 0));

    }


exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stat_sync
 * Purpose:
 *      Synchronize software counters with hardware
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      BCM_E_NONE        Success  
 *      BCM_E_INTERNAL    Device access failure  
 *      BCM_E_DISABLED    Unit's counter disabled  
 */
int 
bcm_petra_stat_sync(
    int unit)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
     
    rc = soc_counter_sync(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stat_get
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number 
 *      type  - (IN)     SNMP statistics type defined in bcm_stat_val_t
 *      value - (OUT)    Counter value 
 * Returns:
 *      BCM_E_NONE       Success.  
 *      BCM_E_PARAM      Illegal parameter.  
 *      BCM_E_BADID      Illegal port number.  
 *      BCM_E_INTERNAL   Device access failure.  
 *      BCM_E_UNAVAIL    Counter/variable is not implemented on this current chip.  
 */
int 
bcm_petra_stat_get(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint64 *value)
{
    int rv;
    /*
     * This procedure is hard-coded to accept up to 2 counters for loading *value
     */
    uint32 cnt_type[3] = {0xffffff,0xffffff, 0xffffff} ;
    /*
     * Total number of counters actually used for loading *value
     */
    int num_cntrs, cntr_index ;
    /*
     * Accumulator used for summing 'num_cntrs' (eventually loaded into *value)
     */
    uint64 val ;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(value);

    rv = BCM_E_NONE ;
    num_cntrs = sizeof(cnt_type) / sizeof(cnt_type[0]) ;

    if (BCM_PBMP_MEMBER(PBMP_CMIC(unit), port)) {
        /* Rudimentary CPU statistics -- needs work */
        switch (type) {
        case snmpIfInOctets:
            COMPILER_64_SET(*value, 0, SOC_CONTROL(unit)->stat.dma_rbyt);
            break;
        case snmpIfInUcastPkts:
            COMPILER_64_SET(*value, 0, SOC_CONTROL(unit)->stat.dma_rpkt);
            break;
        case snmpIfOutOctets:
            COMPILER_64_SET(*value, 0, SOC_CONTROL(unit)->stat.dma_tbyt);
            break;
        case snmpIfOutUcastPkts:
            COMPILER_64_SET(*value, 0, SOC_CONTROL(unit)->stat.dma_tpkt);
            break;
        default:
            COMPILER_64_ZERO(*value);
            break;
        }
        BCM_EXIT;
    }
    
    if ( _SOC_CONTROLLED_COUNTER_USE(unit, port)) {


        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mapping_stat_get, (unit,port ,cnt_type,&num_cntrs,type,3));
        BCMDNX_IF_ERR_EXIT(rv);
  
        COMPILER_64_ZERO(val) ;
        for (cntr_index = 0 ; cntr_index < num_cntrs ; cntr_index++)
        {
            if (COUNTER_IS_COLLECTED(
                SOC_CONTROL(unit)->controlled_counters[cnt_type[cntr_index]]) && SOC_CONTROL(unit)->counter_interval != 0) {
                /*counter is collected by counter thread*/
                rv = soc_counter_get(unit, port, cnt_type[cntr_index], 0, value);
            } else {
                /*counter isn't collected by counter thread */
                rv = SOC_CONTROL(unit)->
                    controlled_counters[cnt_type[cntr_index]].controlled_counter_f(unit,
                        SOC_CONTROL(unit)->controlled_counters[cnt_type[cntr_index]].counter_id, port, value);
            }
            if (rv != BCM_E_NONE) {
                /*
                 * If an error is encountered then stop collecting data and quit.
                 */
                break ;
            }
            COMPILER_64_ADD_64(val,*value) ;  /*  val += (*value) ; */
        }
        *value = val ;
    } else if (soc_feature(unit, soc_feature_generic_counters)) {
        bcm_port_t port_base;
        int phy_port;
        /*get port base for channelized case */
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        port_base = SOC_INFO(unit).port_p2l_mapping[phy_port];

        rv = _bcm_petra_stat_generic_get(unit, port_base, type, value);
        _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
        return rv;
    } else {
        _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
        return BCM_E_UNAVAIL;
    }


    BCMDNX_IF_ERR_EXIT(rv);
    
exit:   
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stat_multi_get
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit      - (IN)     Unit number.
 *      port      - (IN)     Zero-based device or logical port number 
 *      nstat     - (IN)     Number of elements in stat array
 *      stat_arr  - (IN)     Array of SNMP statistics types defined in bcm_stat_val_t
 *      value_arr - (OUT)    Collected 64-bit or 32-bit statistics values 
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_petra_stat_multi_get(
    int unit, 
    bcm_port_t port, 
    int nstat,
    bcm_stat_val_t *stat_arr, 
    uint64 *value_arr)
{
    int rc = BCM_E_NONE;
    int i;
    BCMDNX_INIT_FUNC_DEFS;

    if (nstat <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid nstat")));
    }

    BCMDNX_NULL_CHECK(stat_arr);
    BCMDNX_NULL_CHECK(value_arr);

    for(i=0 ; i<nstat ; i++ ) {
        rc = bcm_petra_stat_get(unit, port, stat_arr[i], &(value_arr[i]));
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:   
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *  bcm_petra_stat_get32
 * Description:
 *  Get the specified statistic from the StrataSwitch
 * Parameters:
 *  unit - StrataSwitch PCI device unit number (driver internal).
 *  port - zero-based port number
 *  type - SNMP statistics type (see stat.h)
 *      val - (OUT) 32-bit counter value.
 * Returns:
 *  BCM_E_NONE - Success.
 *  BCM_E_PARAM - Illegal parameter.
 *  BCM_E_BADID - Illegal port number.
 *  BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *  Same as bcm_petra_stat_get, except converts result to 32-bit.
 */
int 
bcm_petra_stat_get32(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint32 *value)
{
    int      rv;
    uint64    val64;

    BCMDNX_INIT_FUNC_DEFS;

    
    rv = bcm_petra_stat_get(unit, port, type, &val64);

    COMPILER_64_TO_32_LO(*value, val64);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stat_multi_get32
 * Purpose:
 *      Get the specified statistics from the device.  The 64-bit
 *      values may be truncated to fit.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) 
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Array of SNMP statistics types defined in bcm_stat_val_t
 *      value_arr - (OUT) Collected 32-bit statistics values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_stat_multi_get32(
    int unit, 
    bcm_port_t port, 
    int nstat,
    bcm_stat_val_t *stat_arr, 
    uint32 *value_arr)
{
    int i;
    BCMDNX_INIT_FUNC_DEFS;
   
    if (nstat <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid nstat")));
    }

    BCMDNX_NULL_CHECK(stat_arr);
    BCMDNX_NULL_CHECK(value_arr);

    for(i=0 ; i<nstat ; i++ ) {
        BCMDNX_IF_ERR_EXIT
            (bcm_petra_stat_get32(unit, port, stat_arr[i],
                                &(value_arr[i])));
    }

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_stat_counter_non_dma_extra_get(int unit,
                                    soc_counter_non_dma_id_t non_dma_id,
                                    soc_port_t port,
                                    uint64 *val)
{
    return BCM_E_NONE;
}


#ifdef BCM_ARAD_SUPPORT
int
_bcm_petra_stat_recover(int unit, bcm_pbmp_t okay_nif_ports, uint32 counter_flags, int counter_interval, bcm_pbmp_t counter_pbmp) {
    bcm_pbmp_t pbmp_disabled;
    bcm_port_t p;
    bcm_pbmp_t ports_to_remove;
    bcm_port_t port_base;
    int phy_port;

    BCMDNX_INIT_FUNC_DEFS;

    /* add new port and start counter thread */
    BCM_PBMP_CLEAR(pbmp_disabled);
    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_ports_get(unit, SOC_PORT_FLAGS_DISABLED, &pbmp_disabled));
    BCM_PBMP_REMOVE(counter_pbmp, pbmp_disabled);
    BCM_PBMP_OR(counter_pbmp, okay_nif_ports);

    /*remove channelized ports which is not port base*/
    BCM_PBMP_CLEAR(ports_to_remove);
    PBMP_ITER(counter_pbmp, p) {
        /*get port base for channelized case */
        phy_port = SOC_INFO(unit).port_l2p_mapping[p];
        port_base = SOC_INFO(unit).port_p2l_mapping[phy_port];
        if (!SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, p)  && p != port_base /*nchannelized and not port_Base*/) {
            BCM_PBMP_PORT_ADD(ports_to_remove, p);
        }
    }
    BCM_PBMP_REMOVE(counter_pbmp, ports_to_remove);

    BCMDNX_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
    

exit:
    BCMDNX_FUNC_RETURN;
}
#endif
int
bcm_petra_stat_counter_config_set(
    int unit, 
    bcm_stat_counter_engine_t *engine, 
    bcm_stat_counter_config_t *config)
{
    int rv = BCM_E_NONE;
    uint32 valid_flags = BCM_STAT_COUNTER_CLEAR_CONFIGURATION | BCM_STAT_COUNTER_CACHE | BCM_STAT_COUNTER_SYMMETRICAL_MULTICORE_ENGINE;
    int index, offset;
    SOC_TMC_CNT_SRC_TYPE src_type = SOC_TMC_CNT_NOF_SRC_TYPES_ARAD;
    SOC_TMC_CNT_MODE_EG_TYPE eg_type = SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD;
    SOC_TMC_CNT_FORMAT format = SOC_TMC_CNT_NOF_FORMATS_ARAD;
    SOC_TMC_CNT_COUNTER_WE_BITMAP we_val = 0;
    SOC_TMC_CNT_CUSTOM_MODE_PARAMS custom_mode_params;
    SOC_TMC_CNT_VOQ_PARAMS voq_cnt;
    uint32 stag_lsb = 0; /*Right now SOC property INGRESS_STAG is not being used as a source in the API*/
    int num_voqs_per_entry_scale;
    int format_scale;
    int green_fwd = 0, yellow_fwd = 0, red_fwd = 0, black_fwd = 0, green_drop = 0, yellow_drop = 0, red_drop = 0, black_drop = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(engine);
    BCMDNX_NULL_CHECK(config);
    /*validate params*/
    if (engine->engine_id < 0 || engine->engine_id >= SOC_DPP_DEFS_GET(unit, nof_counter_processors)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Counter engine %d out of range"),engine->engine_id)); 
    }
    if (engine->flags & ~valid_flags){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags %d"),engine->flags)); 
    }
    if (config->format.format_type < 0 || config->format.format_type >= bcmStatCounterFormatCount) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid format type %d"), config->format.format_type)); 
    } 
    if (config->format.counter_set_mapping.counter_set_size < 1 || config->format.counter_set_mapping.counter_set_size > BCM_STAT_COUNTER_MAPPING_MAX_SIZE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid counter set size %d"), 
                                          config->format.counter_set_mapping.counter_set_size)); 
    }
    if (config->format.counter_set_mapping.num_of_mapping_entries < 1 || config->format.counter_set_mapping.num_of_mapping_entries > BCM_STAT_COUNTER_MAPPING_MAX_SIZE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid number of mapping entries %d"), 
                                          config->format.counter_set_mapping.num_of_mapping_entries)); 
    }
    if (config->format.counter_set_mapping.num_of_mapping_entries < config->format.counter_set_mapping.counter_set_size) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid counter set size(%d), must be equal or less than number of mapping entries (%d)"), 
                                          config->format.counter_set_mapping.num_of_mapping_entries, 
                                          config->format.counter_set_mapping.counter_set_size)); 
    }
    for (index = 0; index < config->format.counter_set_mapping.num_of_mapping_entries; ++index) {
        if (config->format.counter_set_mapping.entry_mapping[index].offset > config->format.counter_set_mapping.counter_set_size) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("entry (%d) offset mapping (%d) cannot be higher counter set size (%d)"), 
                                              index, 
                                              config->format.counter_set_mapping.entry_mapping[index].offset, 
                                              config->format.counter_set_mapping.counter_set_size)); 
        }
        if (config->format.counter_set_mapping.entry_mapping[index].entry.color < 0 || config->format.counter_set_mapping.entry_mapping[index].entry.color >= bcmColorPreserve) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid entry color (%d) for entry (%d)"), 
                                              config->format.counter_set_mapping.entry_mapping[index].entry.color, 
                                              index));
        }
    }
    if ((config->source.core_id < 0 || config->source.core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && 
        config->source.core_id != BCM_CORE_ALL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Source core id out of range %d"), config->source.core_id));
    }
    if (config->source.engine_source < 0 ||
        config->source.engine_source >= bcmStatCounterSourceCount) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid engine source type %d"), config->source.engine_source)); 
    }

    switch (config->format.format_type) {
    case bcmStatCounterFormatPacketsAndBytes:
        format = SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES;
        break;
    case bcmStatCounterFormatPackets:
        format = SOC_TMC_CNT_FORMAT_PKTS;
        break;
    case bcmStatCounterFormatBytes:
        format = SOC_TMC_CNT_FORMAT_BYTES;
        break;
    case bcmStatCounterFormatDoublePackets:
        format = SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS;
        break;
    case bcmStatCounterFormatMaxVoqSize:
        format = SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE;
        break;
    case bcmStatCounterFormatIngressLatency:
        BCM_EXIT;
    case bcmStatCounterFormatCount:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid format type %d"), config->format.format_type)); 
        break;
    }

    num_voqs_per_entry_scale = (config->source.engine_source == bcmStatCounterSourceIngressVoq) ? config->source.num_voqs_per_entry : 1;
    format_scale = (config->format.format_type == bcmStatCounterFormatDoublePackets) ? 2 : 1;

    if ((config->source.pointer_range.end - config->source.pointer_range.start) / num_voqs_per_entry_scale > 
                            SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) * format_scale) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Counter pointer range %d-%d is to high"), 
                                          config->source.pointer_range.start,
                                          config->source.pointer_range.end));
    }
    if (config->source.engine_source == bcmStatCounterSourceIngressVoq && 
        (( SOC_IS_JERICHO(unit) && (config->source.num_voqs_per_entry < 1 || config->source.num_voqs_per_entry > 8)) || 
         (!SOC_IS_JERICHO(unit) && (config->source.num_voqs_per_entry != 1 && 
                                    config->source.num_voqs_per_entry != 2 &&
                                    config->source.num_voqs_per_entry != 8)))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid number of VOQs per entry %d"), config->source.num_voqs_per_entry)); 
    }

    custom_mode_params.nof_counters = config->format.counter_set_mapping.counter_set_size;
    sal_memset(custom_mode_params.entries_bmaps, 0 , sizeof(uint32) * SOC_TMC_CNT_BMAP_OFFSET_COUNT);
    for (index = 0; index < config->format.counter_set_mapping.num_of_mapping_entries; ++index) {
        offset = config->format.counter_set_mapping.entry_mapping[index].offset;
        if (offset < 0 || offset >= custom_mode_params.nof_counters) {
            offset = SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL;
        }
        if (config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop) {
            if (config->format.counter_set_mapping.entry_mapping[index].entry.color == bcmColorGreen){
                custom_mode_params.entries_bmaps[offset] |= GREEN_FWD;
                green_fwd++;
            } else if (config->format.counter_set_mapping.entry_mapping[index].entry.color == bcmColorYellow) {
                custom_mode_params.entries_bmaps[offset] |= YELLOW_FWD;
                yellow_fwd++;
            } else if (config->format.counter_set_mapping.entry_mapping[index].entry.color == bcmColorRed) {
                custom_mode_params.entries_bmaps[offset] |= RED_FWD;
                red_fwd++;
            } else if (config->format.counter_set_mapping.entry_mapping[index].entry.color == bcmColorBlack) {
                custom_mode_params.entries_bmaps[offset] |= BLACK_FWD;
                black_fwd++;
            }
        } else {
            if (config->format.counter_set_mapping.entry_mapping[index].entry.color == bcmColorGreen){
                custom_mode_params.entries_bmaps[offset] |= GREEN_DROP;
                green_drop++;
            } else if (config->format.counter_set_mapping.entry_mapping[index].entry.color == bcmColorYellow) {
                custom_mode_params.entries_bmaps[offset] |= YELLOW_DROP;
                yellow_drop++;
            } else if (config->format.counter_set_mapping.entry_mapping[index].entry.color == bcmColorRed) {
                custom_mode_params.entries_bmaps[offset] |= RED_DROP;
                red_drop++;
            } else if (config->format.counter_set_mapping.entry_mapping[index].entry.color == bcmColorBlack) {
                custom_mode_params.entries_bmaps[offset] |= BLACK_DROP;
                black_drop++;
            }
        }
    }
    if (index < BCM_STAT_COUNTER_MAPPING_MAX_SIZE) {
        if (green_fwd > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Forward green cannot be configured twice"))); 
        } else if (green_fwd == 0) {
             custom_mode_params.entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] |= GREEN_FWD;
        }
        if (yellow_fwd > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Forward yellow cannot be configured twice"))); 
        } else if (yellow_fwd == 0) {
             custom_mode_params.entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] |= YELLOW_FWD;
        }
        if (red_fwd > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Forward red cannot be configured twice"))); 
        } else if (red_fwd == 0) {
             custom_mode_params.entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] |= RED_FWD;
        }
        if (black_fwd > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Forward black cannot be configured twice"))); 
        } else if (black_fwd == 0) {
             custom_mode_params.entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] |= BLACK_FWD;
        }


        if (green_drop > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Drop green cannot be configured twice"))); 
        } else if (green_drop == 0) {
             custom_mode_params.entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] |= GREEN_DROP;
        }
        if (yellow_drop > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Drop yellow cannot be configured twice"))); 
        } else if (yellow_drop == 0) {
             custom_mode_params.entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] |= YELLOW_DROP;
        }
        if (red_drop > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Drop red cannot be configured twice"))); 
        } else if (red_drop == 0) {
             custom_mode_params.entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] |= RED_DROP;
        }
        if (black_drop > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Drop black cannot be configured twice"))); 
        } else if (black_drop == 0) {
             custom_mode_params.entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] |= BLACK_DROP;
        }
    }
    
    if (config->source.engine_source == bcmStatCounterSourceIngressVoq) {
        voq_cnt.q_set_size = config->source.num_voqs_per_entry;
        voq_cnt.start_q = config->source.pointer_range.start;
    }

    switch (config->source.engine_source) {
    case bcmStatCounterSourceIngressOam:
        switch (config->source.command_id) {
        case 0:
            src_type = SOC_TMC_CNT_SRC_TYPE_OAM_ING_C;
            break;
        case 1:
            src_type = SOC_TMC_CNT_SRC_TYPE_OAM_ING_D;
            break;
        }
        break;
    case bcmStatCounterSourceEgressOam:
        switch (config->source.command_id) {
        case 0:
            src_type = SOC_TMC_CNT_SRC_TYPE_OAM_ING_A;
            break;
        case 1:
            src_type = SOC_TMC_CNT_SRC_TYPE_OAM_ING_B;
            break;
        }
        break;
    case bcmStatCounterSourceIngressVsi:
    case bcmStatCounterSourceIngressInlif:
    case bcmStatCounterSourceIngressField:
        switch (config->source.command_id) {
        case 0:
            if (((config->source.pointer_range.start) / num_voqs_per_entry_scale) < SOC_DPP_DEFS_GET(unit, counters_per_counter_processor)) {
                src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP;
            } else {
                src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP_MSB;
            }
            break;
        case 1:
            if (((config->source.pointer_range.start) / num_voqs_per_entry_scale) < SOC_DPP_DEFS_GET(unit, counters_per_counter_processor)) {
                src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB;
            } else {
                src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB;
            }
            break;
        }
        break;
    case bcmStatCounterSourceIngressCnm:
        src_type = SOC_TMC_CNT_SRC_TYPE_CNM_ID;
        break;
    case bcmStatCounterSourceIngressLatency:
        
        BCM_EXIT;
    case bcmStatCounterSourceIngressVoq:
        src_type = SOC_TMC_CNT_SRC_TYPE_VOQ;
        break;
    case bcmStatCounterSourceIngressVsq:
        src_type = SOC_TMC_CNT_SRC_TYPE_VSQ;
        break;
    case bcmStatCounterSourceEgressTransmitVsi:
    case bcmStatCounterSourceEgressTransmitOutlif:
    case bcmStatCounterSourceEgressTransmitQueue:
    case bcmStatCounterSourceEgressTransmitTmPort:
        switch (config->source.command_id) {
        case 0:
            src_type = SOC_TMC_CNT_SRC_TYPE_EPNI_A;
            break;
        case 1:
            src_type = SOC_TMC_CNT_SRC_TYPE_EPNI_B;
            break;
        case 2:
            src_type = SOC_TMC_CNT_SRC_TYPE_EPNI_C;
            break;
        case 3:
            src_type = SOC_TMC_CNT_SRC_TYPE_EPNI_D;
            break;
        }
        break;
    case bcmStatCounterSourceEgressReceiveVsi:
    case bcmStatCounterSourceEgressReceiveOutlif:
    case bcmStatCounterSourceEgressReceiveQueue:
    case bcmStatCounterSourceEgressReceiveTmPort:
    case bcmStatCounterSourceEgressField:
        if (((config->source.pointer_range.start) / num_voqs_per_entry_scale) < SOC_DPP_DEFS_GET(unit, counters_per_counter_processor)) {
            src_type = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
        } else {
            src_type = SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB;
        }
        break;
    case bcmStatCounterSourceCount:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid engine source type %d"), config->source.engine_source)); 
    }

    switch (config->source.engine_source) {
    case bcmStatCounterSourceIngressLatency:
        BCM_EXIT; 
    case bcmStatCounterSourceIngressVsi:
    case bcmStatCounterSourceEgressTransmitVsi:
    case bcmStatCounterSourceEgressReceiveVsi:
        eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
        break;
    case bcmStatCounterSourceIngressInlif:
    case bcmStatCounterSourceEgressTransmitOutlif:
    case bcmStatCounterSourceEgressReceiveOutlif:
        eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
        break;
    case bcmStatCounterSourceEgressReceiveQueue:
    case bcmStatCounterSourceEgressTransmitQueue:
        eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
        break;
    case bcmStatCounterSourceEgressReceiveTmPort:
    case bcmStatCounterSourceEgressTransmitTmPort:           
        eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
        break;
    case bcmStatCounterSourceEgressField:
    case bcmStatCounterSourceIngressField:
        eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
        break;
    case bcmStatCounterSourceIngressOam:
    case bcmStatCounterSourceEgressOam:
    case bcmStatCounterSourceIngressVoq:
    case bcmStatCounterSourceIngressVsq:
    case bcmStatCounterSourceIngressCnm:
        break;
    case bcmStatCounterSourceCount:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid engine source type %d"), config->source.engine_source)); 
    }
    format_scale = (config->format.format_type == bcmStatCounterFormatDoublePackets) ? 2 : 1;
    we_val = ((config->source.pointer_range.start) / num_voqs_per_entry_scale) / (SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) * format_scale);

    
    rv = bcm_dpp_counter_config_set(
           unit, 
           engine->engine_id, 
           config->source.core_id,
           src_type,
           config->source.command_id,
           eg_type, 
           format,
           we_val,
           &custom_mode_params,
           &voq_cnt, 
           stag_lsb);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stat_counter_config_get(
    int unit, 
    bcm_stat_counter_engine_t *engine, 
    bcm_stat_counter_config_t *config)
{
    int rv = BCM_E_NONE;
    int index, offset;
    SOC_TMC_CNT_SRC_TYPE src_type;
    SOC_TMC_CNT_MODE_EG_TYPE eg_type;
    SOC_TMC_CNT_FORMAT format;
    SOC_TMC_CNT_COUNTER_WE_BITMAP we_val = 0;
    SOC_TMC_CNT_CUSTOM_MODE_PARAMS custom_mode_params;
    SOC_TMC_CNT_VOQ_PARAMS voq_cnt;
    uint32 stag_lsb = 0; /*Right now SOC property INGRESS_STAG is not being used as a source in the API*/
    int command_id;
    int format_scale;
    int num_voqs_per_entry_scale;
    uint8 enabled = FALSE;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(engine);
    BCMDNX_NULL_CHECK(config);
    rv = bcm_dpp_counter_config_get(
            unit, 
            engine->engine_id,
            &enabled,
            &(config->source.core_id),
            &src_type,
            &command_id,
            &eg_type,
            &format,
            &we_val,
            &custom_mode_params,
            &voq_cnt,
            &stag_lsb);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!enabled) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EMPTY, (_BSL_BCM_MSG("Counter Engine is not configured"), engine->engine_id));
    }

    config->format.counter_set_mapping.counter_set_size = custom_mode_params.nof_counters;
    for (index = 0, offset = 0; index < BCM_STAT_COUNTER_MAPPING_MAX_SIZE && offset < BCM_STAT_COUNTER_MAPPING_MAX_SIZE; ++offset) {
        uint32 entries = custom_mode_params.entries_bmaps[offset];
        if (entries & GREEN_FWD) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorGreen;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = TRUE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
            continue;
        }
        if (entries & GREEN_DROP) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorGreen;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = FALSE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
            continue;
        }
        if (entries & YELLOW_FWD) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorYellow;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = TRUE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
            continue;
        }
        if (entries & YELLOW_DROP) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorYellow;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = FALSE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
            continue;
        }
        if (entries & RED_FWD) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorRed;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = TRUE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
            continue;
        }
        if (entries & RED_DROP) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorRed;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = FALSE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
            continue;
        }
        if (entries & BLACK_FWD) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorBlack;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = TRUE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
            continue;
        }
        if (entries & BLACK_DROP) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorBlack;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = FALSE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
            continue;
        }
    }
    config->format.counter_set_mapping.num_of_mapping_entries = index;
    
    if (config->source.engine_source == bcmStatCounterSourceIngressVoq) {
        voq_cnt.q_set_size = config->source.num_voqs_per_entry;
        voq_cnt.start_q = config->source.pointer_range.start;
    }

    switch (src_type){
    case SOC_TMC_CNT_SRC_TYPE_OAM_ING_A:
        config->source.engine_source = bcmStatCounterSourceEgressOam;
        config->source.command_id = 0;
        break;
    case SOC_TMC_CNT_SRC_TYPE_OAM_ING_B:
        config->source.engine_source = bcmStatCounterSourceEgressOam;
        config->source.command_id = 1;
        break;
    case SOC_TMC_CNT_SRC_TYPE_OAM_ING_C:
        config->source.engine_source = bcmStatCounterSourceIngressOam;
        config->source.command_id = 0;
        break;
    case SOC_TMC_CNT_SRC_TYPE_OAM_ING_D:
        config->source.engine_source = bcmStatCounterSourceIngressOam;
        config->source.command_id = 1;
        break;
    case SOC_TMC_CNT_SRC_TYPE_ING_PP:
    case SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB:
    case SOC_TMC_CNT_SRC_TYPE_ING_PP_MSB:
    case SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB:
        switch (eg_type) {
        case SOC_TMC_CNT_MODE_EG_TYPE_VSI:
            config->source.engine_source = bcmStatCounterSourceIngressVsi;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF:
            config->source.engine_source = bcmStatCounterSourceIngressInlif;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_PMF:
            config->source.engine_source = bcmStatCounterSourceIngressField;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT:
        case SOC_TMC_CNT_MODE_EG_TYPE_TM:
        case SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid engine source mode type %d"), eg_type)); 
            break;
        }
        if (src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP || src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_MSB) {
            config->source.command_id = 0;
        } else if (src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB || src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB) {
            config->source.command_id = 1;
        }
        break;
    case SOC_TMC_CNT_SRC_TYPE_CNM_ID:
        config->source.engine_source = bcmStatCounterSourceIngressCnm;
        break;
    /*we dont have a case for IPT latency yet */
    /*bcmStatCounterSourceIngressLatency:*/
        
        /*BCM_EXIT;*/
    case SOC_TMC_CNT_SRC_TYPE_VOQ:
        config->source.engine_source = bcmStatCounterSourceIngressVoq;
        break;
    case SOC_TMC_CNT_SRC_TYPE_VSQ:
        config->source.engine_source = bcmStatCounterSourceIngressVsq;
        break;
    case SOC_TMC_CNT_SRC_TYPE_EPNI_A:
    case SOC_TMC_CNT_SRC_TYPE_EPNI_B:
    case SOC_TMC_CNT_SRC_TYPE_EPNI_C:
    case SOC_TMC_CNT_SRC_TYPE_EPNI_D:
        switch (eg_type) {
        case SOC_TMC_CNT_MODE_EG_TYPE_VSI:
            config->source.engine_source = bcmStatCounterSourceIngressVsi;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF:
            config->source.engine_source = bcmStatCounterSourceIngressInlif;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT:
            config->source.engine_source = bcmStatCounterSourceEgressTransmitTmPort;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_TM:
            config->source.engine_source = bcmStatCounterSourceEgressTransmitQueue;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_PMF:
        case SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid engine source mode type %d"), eg_type)); 
            break;
        }
        if (src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_A) {
            config->source.command_id = 0;
        } else if (src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_B) {
            config->source.command_id = 1;
        } else if (src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_C) {
            config->source.command_id = 2;
        } else if (src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_D) {
            config->source.command_id = 3;
        }
        break;
    case SOC_TMC_CNT_SRC_TYPE_EGR_PP:
    case SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB:
        switch (eg_type) {
        case SOC_TMC_CNT_MODE_EG_TYPE_VSI:
            config->source.engine_source = bcmStatCounterSourceEgressReceiveVsi;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF:
            config->source.engine_source = bcmStatCounterSourceEgressReceiveOutlif;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT:
            config->source.engine_source = bcmStatCounterSourceEgressReceiveTmPort;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_TM:
            config->source.engine_source = bcmStatCounterSourceEgressReceiveQueue;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_PMF:
            config->source.engine_source = bcmStatCounterSourceEgressField;
            break;
        case SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid engine source mode type %d"), eg_type)); 
            break;
        }
        break;
    case SOC_TMC_CNT_SRC_TYPE_STAG:
        break;
    
    case SOC_TMC_CNT_NOF_SRC_TYPES_IPT_LATENCY:
    case SOC_TMC_CNT_NOF_SRC_TYPES_EGQ_TM:
    /*case SOC_TMC_CNT_NOF_SRC_TYPES_ARAD: Equal to SOC_TMC_CNT_NOF_SRC_TYPES_EGQ_TM*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid engine source type %d"), src_type)); 
    }
    
    config->source.command_id = command_id;
    switch (format) {
    case SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES:
        config->format.format_type = bcmStatCounterFormatPacketsAndBytes;
        break;
    case SOC_TMC_CNT_FORMAT_PKTS:
         config->format.format_type = bcmStatCounterFormatPackets;
        break;
    case SOC_TMC_CNT_FORMAT_BYTES:
        config->format.format_type = bcmStatCounterFormatBytes;
        break;
    case SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS:
        config->format.format_type = bcmStatCounterFormatDoublePackets;
        break;
    case SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE:
        config->format.format_type = bcmStatCounterFormatMaxVoqSize;
        break;
    case SOC_TMC_CNT_FORMAT_IHB_COMMANDS:
    case SOC_TMC_CNT_NOF_FORMATS_ARAD:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid format type %d"), config->format.format_type)); 
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid format type %d"), format)); 
    }
    num_voqs_per_entry_scale = (config->source.engine_source == bcmStatCounterSourceIngressVoq) ? config->source.num_voqs_per_entry : 1;
    format_scale = (config->format.format_type == bcmStatCounterFormatDoublePackets) ? 2 : 1;
    config->source.pointer_range.start = (we_val * num_voqs_per_entry_scale) * (SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) * format_scale);
    config->source.pointer_range.end = ((we_val + 1) * num_voqs_per_entry_scale * SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) * format_scale) - 1;
exit:
    BCMDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

