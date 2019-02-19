/*
 * $Id: stat.c,v 1.30 Broadcom SDK $
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

    interval = (SAL_BOOT_BCMSIM) ?
               SOC_COUNTER_INTERVAL_SIM_DEFAULT : SOC_COUNTER_INTERVAL_DEFAULT;
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
    SOC_TMC_CNT_Q_SET_SIZE q_set_size;
    uint32 stag_lsb = 0; /*Right now SOC property INGRESS_STAG is not being used as a source in the API*/
    uint8 have_stat_tag = FALSE;
    int num_voqs_per_entry_scale;
    int format_scale;
    int green_fwd = 0, yellow_fwd = 0, red_fwd = 0, black_fwd = 0, green_drop = 0, yellow_drop = 0, red_drop = 0, black_drop = 0;
    int command_base_offset, counter_sets_per_counter_processor;
    uint8 clear_cache = FALSE;
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
    clear_cache = (engine->flags & BCM_STAT_COUNTER_CACHE) ? TRUE : FALSE;
    if (engine->flags & BCM_STAT_COUNTER_CLEAR_CONFIGURATION) {
        rv = bcm_dpp_counter_config_clear(unit, engine->engine_id, clear_cache);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
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
        if (config->source.engine_source == bcmStatCounterSourceIngressOam || config->source.engine_source == bcmStatCounterSourceEgressOam) {
            format = SOC_TMC_CNT_FORMAT_IHB_COMMANDS;
        } else {
            format = SOC_TMC_CNT_FORMAT_PKTS;
        }
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
    if (config->source.engine_source == bcmStatCounterSourceIngressVoq && 
        (( SOC_IS_JERICHO(unit) && (config->source.num_voqs_per_entry < 1 || config->source.num_voqs_per_entry > 8)) || 
         (!SOC_IS_JERICHO(unit) && (config->source.num_voqs_per_entry != 1 && 
                                    config->source.num_voqs_per_entry != 2 &&
                                    config->source.num_voqs_per_entry != 8)))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid number of VOQs per entry %d"), config->source.num_voqs_per_entry)); 
    }

    custom_mode_params.set_size = config->format.counter_set_mapping.counter_set_size;
    sal_memset(custom_mode_params.entries_bmaps, 0 , sizeof(uint32) * SOC_TMC_CNT_BMAP_OFFSET_COUNT);
    for (index = 0; index < config->format.counter_set_mapping.num_of_mapping_entries; ++index) {
        offset = config->format.counter_set_mapping.entry_mapping[index].offset;
        if (offset < 0 || offset >= custom_mode_params.set_size) {
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
    switch (config->source.engine_source) {
    case bcmStatCounterSourceIngressOam:
    case bcmStatCounterSourceEgressOam:
        src_type = SOC_TMC_CNT_SRC_TYPE_OAM;
        break;
    case bcmStatCounterSourceIngressVsi:
    case bcmStatCounterSourceIngressInlif:
    case bcmStatCounterSourceIngressField:
        src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP;
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
        src_type = SOC_TMC_CNT_SRC_TYPE_EPNI;
        break;
    case bcmStatCounterSourceEgressReceiveVsi:
    case bcmStatCounterSourceEgressReceiveOutlif:
    case bcmStatCounterSourceEgressReceiveQueue:
    case bcmStatCounterSourceEgressReceiveTmPort:
    case bcmStatCounterSourceEgressField:
        src_type = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
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

    num_voqs_per_entry_scale = (config->source.engine_source == bcmStatCounterSourceIngressVoq) ? config->source.num_voqs_per_entry : 1;
    format_scale = (config->format.format_type == bcmStatCounterFormatDoublePackets) ? 2 : 1;
    counter_sets_per_counter_processor = ((SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) * format_scale * num_voqs_per_entry_scale) / (custom_mode_params.set_size));
    command_base_offset = (config->source.pointer_range.start % counter_sets_per_counter_processor);

    if ((config->source.pointer_range.end - config->source.pointer_range.start) > counter_sets_per_counter_processor) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Counter pointer range %d-%d is to high"), 
                                          config->source.pointer_range.start,
                                          config->source.pointer_range.end));
    }

    if (eg_type == SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF) {
        int lif_base_val;
        rv = bcm_dpp_counter_lif_base_val_get(unit, src_type, config->source.command_id, &lif_base_val);
        BCMDNX_IF_ERR_EXIT(rv);

        if (lif_base_val != command_base_offset) {           
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Counter source %s LIF-Counting base offset offset is %d (defined with bcm_stat_lif_counting_profile_set()), cannot set it to %d.\n" 
                                                           "Counter pointer range start is %d, and number of counter sets in engine is %d"), 
                                              SOC_TMC_CNT_SRC_TYPE_to_string(src_type), lif_base_val, command_base_offset, config->source.pointer_range.start, counter_sets_per_counter_processor));
        }
    }
    if (command_base_offset != 0 && 
        (src_type != SOC_TMC_CNT_SRC_TYPE_VOQ && 
         src_type != SOC_TMC_CNT_SRC_TYPE_STAG && 
         src_type != SOC_TMC_CNT_SRC_TYPE_VSQ && 
         src_type != SOC_TMC_CNT_SRC_TYPE_CNM_ID && 
         src_type != SOC_TMC_CNT_SRC_TYPE_ING_PP && 
         src_type != SOC_TMC_CNT_SRC_TYPE_EPNI)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Counter source %s, does not support base offset %d (!= 0)." 
                                                       "counter pointer range start is %d, and number of counter sets in engine is %d"), 
                                          SOC_TMC_CNT_SRC_TYPE_to_string(src_type), command_base_offset, config->source.pointer_range.start, counter_sets_per_counter_processor));
    }
    
    q_set_size = (config->source.engine_source == bcmStatCounterSourceIngressVoq) ? config->source.num_voqs_per_entry : 1;
    we_val = (config->source.pointer_range.start / counter_sets_per_counter_processor);

    if (src_type == SOC_TMC_CNT_SRC_TYPE_STAG) {
        rv = bcm_dpp_counter_stag_lsb_get(unit, &stag_lsb, &have_stat_tag);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!have_stat_tag) {
            stag_lsb = 0;
        }
    }
    rv = bcm_dpp_counter_config_set(
           unit, 
           engine->engine_id, 
           config->source.core_id,
           src_type,
           config->source.command_id,
           command_base_offset,
           eg_type, 
           format,
           we_val,
           &custom_mode_params,
           q_set_size, 
           stag_lsb,
           clear_cache);
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
    SOC_TMC_CNT_Q_SET_SIZE q_set_size;
    uint32 stag_lsb = 0; /*Right now SOC property INGRESS_STAG is not being used as a source in the API*/
    int command_id;
    int command_base_offset;
    int counter_sets_per_counter_processor;
    int format_scale;
    uint8 enabled = FALSE;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(engine);
    BCMDNX_NULL_CHECK(config);
    sal_memset(custom_mode_params.entries_bmaps, 0 , sizeof(uint32) * SOC_TMC_CNT_BMAP_OFFSET_COUNT);
    rv = bcm_dpp_counter_config_get(
            unit, 
            engine->engine_id,
            &enabled,
            &(config->source.core_id),
            &src_type,
            &command_id,
            &command_base_offset,
            &eg_type,
            &format,
            &we_val,
            &custom_mode_params,
            &q_set_size,
            &stag_lsb);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!enabled) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EMPTY, (_BSL_BCM_MSG("Counter Engine is not configured"), engine->engine_id));
    }
    config->format.counter_set_mapping.counter_set_size = custom_mode_params.set_size;
    for (index = 0, offset = 0; index < SOC_TMC_CNT_BMAP_OFFSET_COUNT && offset < custom_mode_params.set_size; ++offset) {
        uint32 entries = custom_mode_params.entries_bmaps[offset];
        if (entries & GREEN_FWD) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorGreen;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = TRUE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
        }
        if (entries & GREEN_DROP) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorGreen;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = FALSE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
        }
        if (entries & YELLOW_FWD) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorYellow;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = TRUE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
        }
        if (entries & YELLOW_DROP) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorYellow;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = FALSE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
        }
        if (entries & RED_FWD) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorRed;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = TRUE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
        }
        if (entries & RED_DROP) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorRed;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = FALSE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
        }
        if (entries & BLACK_FWD) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorBlack;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = TRUE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
        }
        if (entries & BLACK_DROP) {
            config->format.counter_set_mapping.entry_mapping[index].entry.color = bcmColorBlack;
            config->format.counter_set_mapping.entry_mapping[index].entry.is_forward_not_drop = FALSE;
            config->format.counter_set_mapping.entry_mapping[index].offset = offset;
            index++;
        }
    }
    config->format.counter_set_mapping.num_of_mapping_entries = index;
    switch (src_type){
    case SOC_TMC_CNT_SRC_TYPE_OAM:
        config->source.engine_source = bcmStatCounterSourceEgressOam;
        break;
    case SOC_TMC_CNT_SRC_TYPE_ING_PP:
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
    case SOC_TMC_CNT_SRC_TYPE_EPNI:
       switch (eg_type) {
        case SOC_TMC_CNT_MODE_EG_TYPE_VSI:
            config->source.engine_source = bcmStatCounterSourceEgressTransmitVsi;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF:
            config->source.engine_source = bcmStatCounterSourceEgressTransmitOutlif;
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
        break;
    case SOC_TMC_CNT_SRC_TYPE_EGR_PP:
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
    
    case SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY:
    case SOC_TMC_CNT_SRC_TYPES_EGQ_TM:
    default:
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
    config->source.num_voqs_per_entry = (src_type == SOC_TMC_CNT_SRC_TYPE_VOQ) ? q_set_size : 1;
    format_scale = (format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) ? 2 : 1;
    counter_sets_per_counter_processor = ((SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) * format_scale * config->source.num_voqs_per_entry) / (custom_mode_params.set_size));

    config->source.pointer_range.start = (we_val * counter_sets_per_counter_processor) + command_base_offset;
    config->source.pointer_range.end =  ((we_val + 1) * counter_sets_per_counter_processor) + command_base_offset - 1;
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      bcm_petra_stat_counter_lif_counting_get
 * Purpose:
 *      GET A {COUNTING_SOURCE, COUNTING_MASK} TO LIF_STACK_LEVEL MAPPING.
 *      For issuing a counter command with the the LIF in lif_stack_id_to_count in the packet LIF stack.
 *      
 * Parameters:
 *      unit                  - (IN)  Unit number.
 *      source                - (IN)  Counter source to configure.
 *                                    Only fields engine_source and command_id are relevant.
 *                                    For field engine_source only values bcmStatCounterSourceIngressInlif, and bcmStatCounterSourceEgressTransmitOutlif are legal sources.
 *      counting_mask         - (IN)  Defines the mask of range IDs per stack ID level, for which the packet will be counted.
 *      lif_stack_id_to_count - (OUT) Issue a counter command with the LIF in lif_stack_id_to_count in the packet LIF stack.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_petra_stat_counter_lif_counting_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_source_t *source, 
    bcm_stat_counter_lif_mask_t *counting_mask, 
    bcm_stat_counter_lif_stack_id_t *lif_stack_id_to_count)
{
    int rv = BCM_E_NONE;
    SOC_TMC_CNT_SRC_TYPE counter_source;
    bcm_stat_counter_lif_stack_id_t counter_lif_stack_id = 0;
    uint32 lif_counting_mask = 0;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(source);
    BCMDNX_NULL_CHECK(counting_mask);
    BCMDNX_NULL_CHECK(lif_stack_id_to_count);

    if (source->engine_source == bcmStatCounterSourceIngressInlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_ING_PP;
    } else if (source->engine_source == bcmStatCounterSourceEgressTransmitOutlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_EPNI;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source->engine_source));
    }
    if (source->command_id < 0 || source->command_id > 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid command ID %d."), source->command_id));
    }
    for (counter_lif_stack_id = bcmStatCounterLifStackId0; counter_lif_stack_id < BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS; counter_lif_stack_id++) {
        if (counting_mask->lif_counting_mask[counter_lif_stack_id] < bcmStatCounterLifRangeIdLifInvalid ||
            counting_mask->lif_counting_mask[counter_lif_stack_id] > bcmBcmStatCounterLifRangeId1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Lif counting mask %d, for counter level %d."), 
                                              counting_mask->lif_counting_mask[counter_lif_stack_id],
                                              counter_lif_stack_id));
        }
    }
    SOC_TMC_CNT_LIF_COUNTING_MASK_SET(counter_source,
                                      lif_counting_mask, 
                                      counting_mask->lif_counting_mask[bcmStatCounterLifStackId0],
                                      counting_mask->lif_counting_mask[bcmStatCounterLifStackId1],
                                      counting_mask->lif_counting_mask[bcmStatCounterLifStackId2],
                                      counting_mask->lif_counting_mask[bcmStatCounterLifStackId3]);
    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_lif_counting_get, (unit, counter_source, source->command_id, lif_counting_mask, lif_stack_id_to_count));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stat_counter_lif_counting_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_source_t *source, 
    bcm_stat_counter_lif_mask_t *counting_mask, 
    bcm_stat_counter_lif_stack_id_t *lif_stack_id_to_count)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported API for device type.")));
    }
    rv = _bcm_petra_stat_counter_lif_counting_get(unit, flags, source, counting_mask, lif_stack_id_to_count);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      bcm_petra_stat_counter_lif_counting_set
 * Purpose:
 *      Creates a {counting_source, counting_mask} to lif_stack_level mapping.
 *      For issuing a counter command with the the LIF in lif_stack_id_to_count in the packet LIF stack.
 *      
 * Parameters:
 *      unit                  - (IN) Unit number.
 *      source                - (IN) Counter source to configure.
 *                                   Only fields engine_source and command_id are relevant.
 *                                   For field engine_source only values bcmStatCounterSourceIngressInlif, and bcmStatCounterSourceEgressTransmitOutlif are legal sources.
 *      counting_mask         - (IN) Defines the mask of range IDs per stack ID level, for which the packet will be counted.
 *      lif_stack_id_to_count - (IN) Issue a counter command with the LIF in lif_stack_id_to_count in the packet LIF stack.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_stat_counter_lif_counting_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_source_t *source, 
    bcm_stat_counter_lif_mask_t *counting_mask,
    bcm_stat_counter_lif_stack_id_t lif_stack_id_to_count)
{
    int rv = BCM_E_NONE;
    SOC_TMC_CNT_SRC_TYPE counter_source;
    uint32 lif_counting_mask = 0;
    bcm_stat_counter_lif_stack_id_t counter_lif_stack_id = 0;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(source);
    BCMDNX_NULL_CHECK(counting_mask);

    if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported API for device type.")));
    }

    if (source->engine_source == bcmStatCounterSourceIngressInlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_ING_PP;
    } else if (source->engine_source == bcmStatCounterSourceEgressTransmitOutlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_EPNI;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source->engine_source));
    }
    if (source->command_id < 0 || source->command_id > 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid command ID %d."), source->command_id));
    }
    for (counter_lif_stack_id = bcmStatCounterLifStackId0; counter_lif_stack_id < BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS; counter_lif_stack_id++) {
        if (counting_mask->lif_counting_mask[counter_lif_stack_id] < bcmStatCounterLifRangeIdLifInvalid ||
            counting_mask->lif_counting_mask[counter_lif_stack_id] > bcmBcmStatCounterLifRangeId1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Lif counting mask %d, for counter level %d."), 
                                              counting_mask->lif_counting_mask[counter_lif_stack_id], 
                                              counter_lif_stack_id));
        }
    }
    if (lif_stack_id_to_count < bcmStatCounterLifStackIdNone || lif_stack_id_to_count > bcmStatCounterLifStackId3){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF stack level to count %d."), lif_stack_id_to_count));
    }
    if (lif_stack_id_to_count == bcmStatCounterLifStackIdNone && counter_source == SOC_TMC_CNT_SRC_TYPE_ING_PP) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF stack level to count %d, for ingress LIF."), lif_stack_id_to_count));
    }
    if (lif_stack_id_to_count == bcmStatCounterLifStackId3 && counter_source == SOC_TMC_CNT_SRC_TYPE_EPNI) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF stack level to count %d, for egress LIF."), lif_stack_id_to_count));
    }
    SOC_TMC_CNT_LIF_COUNTING_MASK_SET(counter_source, 
                                      lif_counting_mask, 
                                      counting_mask->lif_counting_mask[bcmStatCounterLifStackId0],
                                      counting_mask->lif_counting_mask[bcmStatCounterLifStackId1],
                                      counting_mask->lif_counting_mask[bcmStatCounterLifStackId2],
                                      counting_mask->lif_counting_mask[bcmStatCounterLifStackId3]);

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_lif_counting_set, (unit, counter_source, source->command_id, lif_counting_mask, lif_stack_id_to_count));
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stat_counter_lif_counting_range_get
 * Purpose:
 *      Get an Out-LIF/In-LIF counting ranges to be compared with.
 *      
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      source    - (IN)  The source lif that can be configured only bcmStatCounterSourceIngressInlif and bcmStatCounterSourceEgressTransmitOutlif are valid values.
 *      range_id  - (IN)  the range to be configured. only bcmBcmStatCounterLifRangeId0 and bcmBcmStatCounterLifRangeId1 are valid values.
 *      lif_range - (OUT) the range configuration.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_stat_counter_lif_counting_range_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_source_type_t source, 
    bcm_stat_counter_lif_range_id_t range_id, 
    bcm_stat_counter_lif_counting_range_t *lif_range)
{
    int rv = BCM_E_NONE;
    SOC_SAND_U32_RANGE range = {0};
    SOC_TMC_CNT_SRC_TYPE counter_source;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(lif_range);

    if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported API for device type.")));
    }
    if (source == bcmStatCounterSourceIngressInlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_ING_PP;
    } else if (source == bcmStatCounterSourceEgressTransmitOutlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_EPNI;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source));
    }

    if (range_id != bcmBcmStatCounterLifRangeId0 && range_id != bcmBcmStatCounterLifRangeId1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid range ID %d."), range_id));
    }
    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_lif_counting_range_get, (unit, counter_source, range_id, &range));
    BCMDNX_IF_ERR_EXIT(rv);
    lif_range->start = range.start;
    lif_range->end = range.end;

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      bcm_petra_stat_counter_lif_counting_range_get
 * Purpose:
 *      Set an Out-LIF/In-LIF counting ranges to be compared with.
 *      
 * Parameters:
 *      unit      - (IN) Unit number.
 *      source    - (IN) The source lif that can be configured only bcmStatCounterSourceIngressInlif and bcmStatCounterSourceEgressTransmitOutlif are valid values.
 *      range_id  - (IN) the range to be configured. only bcmBcmStatCounterLifRangeId0 and bcmBcmStatCounterLifRangeId1 are valid values.
 *      lif_range - (IN) the range configuration.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_stat_counter_lif_counting_range_set(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_source_type_t source, 
    bcm_stat_counter_lif_range_id_t range_id, 
    bcm_stat_counter_lif_counting_range_t *lif_range)
{
    int rv = BCM_E_NONE;
    SOC_SAND_U32_RANGE range = {0};
    SOC_TMC_CNT_SRC_TYPE counter_source;
    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported API for device type.")));
    }
    BCMDNX_NULL_CHECK(lif_range);
    if (lif_range->start >= SOC_DPP_DEFS_GET(unit, nof_local_lifs) || 
        lif_range->end >= SOC_DPP_DEFS_GET(unit, nof_local_lifs)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF range %d-%d."), lif_range->start, lif_range->end));
    }
    if (source == bcmStatCounterSourceIngressInlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_ING_PP;
    } else if (source == bcmStatCounterSourceEgressTransmitOutlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_EPNI;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source));
    }

    if (range_id != bcmBcmStatCounterLifRangeId0 && range_id != bcmBcmStatCounterLifRangeId1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid range ID %d."), range_id));
    }
    range.start = lif_range->start;
    range.end = lif_range->end;

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_lif_counting_range_set, (unit, counter_source, range_id, &range));
    BCMDNX_IF_ERR_EXIT(rv);
   
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Function:
 *      bcm_petra_stat_lif_counting_profile_get
 * Purpose:
 *      Get A LIF-counting-profile mapping to source X command-ID, and LIF-range.
 *      
 * Parameters:
 *      unit                  - (IN)  Unit number.
 *      flags                 - (IN)  flags - not in use
 *      lif_counting_profile  - (IN)  LIF-counting-profile.
 *                                    The profile to which we get the properties
 *      lif_counting          - (OUT) A struct consisting of
 *                                    * LIF-counting-source X command-ID: legal values - only bcmStatCounterSourceIngressInlif and bcmStatCounterSourceEgressTransmitOutlif.
 *                                    * LIF-counting-range: A range of local LIF IDs that are counted.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_petra_stat_lif_counting_profile_get(
    int unit, 
    uint32 flags, 
    int lif_counting_profile, 
    bcm_stat_lif_counting_t *lif_counting)
{
    int rv = BCM_E_NONE;
    SOC_SAND_U32_RANGE range = {0};
    int lif_counting_profile_in_source = lif_counting_profile % SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES_PER_SOURCE;
    int is_ingress = BCM_DPP_COUNTER_LIF_PROFILE_IS_INGRESS(lif_counting_profile);
    SOC_TMC_CNT_SRC_TYPE sources[SOC_TMC_CNT_LIF_COUNTING_NOF_SOURCES] = {SOC_TMC_CNT_SRC_TYPE_EPNI, SOC_TMC_CNT_SRC_TYPE_ING_PP};
    BCMDNX_INIT_FUNC_DEFS;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported API for device type.")));
    }
    BCMDNX_NULL_CHECK(lif_counting);
    if (lif_counting_profile < 0 || lif_counting_profile > SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF-counting-profile %d."), lif_counting_profile));
    }
    if (is_ingress) {
        lif_counting->source.type = bcmStatCounterSourceIngressInlif;
    } else {
        lif_counting->source.type = bcmStatCounterSourceEgressTransmitOutlif;
    }
    rv = bcm_dpp_counter_lif_range_get(unit, lif_counting_profile_in_source, sources[is_ingress], &(lif_counting->source.command_id), &range);
    BCMDNX_IF_ERR_EXIT(rv);
    lif_counting->range.start = range.start;
    lif_counting->range.end = range.end;
exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 *  
 * Function:
 *      bcm_petra_stat_lif_counting_profile_set
 * Purpose:
 *      Set A LIF-counting-profile mapping to source X command-ID, and LIF-range.
 *      
 * Parameters:
 *      unit                  - (IN)  Unit number.
 *      flags                 - (IN)  flags - not in use
 *      lif_counting_profile  - (IN)  LIF-counting-profile.
 *                                    The profile to which we get the properties
 *      lif_counting          - (IN) A struct consisting of
 *                                    * LIF-counting-source X command-ID: legal values - only bcmStatCounterSourceIngressInlif and bcmStatCounterSourceEgressTransmitOutlif.
 *                                    * LIF-counting-range: A range of local LIF IDs that are counted.
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_petra_stat_lif_counting_profile_set(
    int unit, 
    uint32 flags, 
    int lif_counting_profile, 
    bcm_stat_lif_counting_t *lif_counting)
{
    int rv = BCM_E_NONE;
    SOC_SAND_U32_RANGE range = {0};
    SOC_TMC_CNT_SRC_TYPE source;
    int lif_counting_profile_in_source = lif_counting_profile % SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES_PER_SOURCE;
    int nof_resereved_lifs = 0; 
    BCMDNX_INIT_FUNC_DEFS;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported API for device type.")));
    }
    BCMDNX_NULL_CHECK(lif_counting);
    if (lif_counting->source.type == bcmStatCounterSourceIngressInlif) {
        source = SOC_TMC_CNT_SRC_TYPE_ING_PP;
        nof_resereved_lifs = BCM_DPP_AM_LOCAL_IN_LIF_NOF_RESERVED_LIFS;
    } else if (lif_counting->source.type == bcmStatCounterSourceEgressTransmitOutlif) {
        source = SOC_TMC_CNT_SRC_TYPE_EPNI;
        nof_resereved_lifs = SOC_DPP_CONFIG(unit)->l3.nof_rifs;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), lif_counting->source.type));
    }
    if ((lif_counting->range.start > lif_counting->range.end)   ||
        (lif_counting->range.start < nof_resereved_lifs) || 
        (lif_counting->range.end >= SOC_DPP_DEFS_GET(unit, nof_local_lifs))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF range %d-%d."), lif_counting->range.start, lif_counting->range.end));
    }
    if ((source ==  SOC_TMC_CNT_SRC_TYPE_EPNI && 
         (lif_counting_profile < 0 || lif_counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES_PER_SOURCE)) ||
        (source == SOC_TMC_CNT_SRC_TYPE_ING_PP && 
         (lif_counting_profile < SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES_PER_SOURCE || lif_counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF-counting-profile %d for source type %d."), lif_counting_profile, lif_counting->source.type));
    }
    if (lif_counting->source.command_id != 0 && lif_counting->source.command_id != 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid command ID %d."), lif_counting->source.command_id));
    }
    range.start = lif_counting->range.start;
    range.end = lif_counting->range.end;

    rv = bcm_dpp_counter_lif_range_set(unit, lif_counting_profile_in_source, source, lif_counting->source.command_id /*for jericho - command ID is equal to range ID*/, &range);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}
/* 
 *  
 * Function:
 *      bcm_petra_stat_lif_counting_stack_level_priority_get
 * Purpose:
 *      Get priority LIF-stack-level X LIF-counting-source X command-ID
 *      For each packet the LIF-stack-level with the higher priority will be couted.
 *      If two LIF-stack-levels have the same priority the outter-more LIF will ve counted.
 *      
 * Parameters:
 *      unit                  - (IN)  Unit number.
 *      flags                 - (IN)  flags - not in use
 *      source                - (IN)  LIF-counting-source X command-ID:.
 *      lif_stack_level       - (IN)  A level of LIF on the packet
 *      priority              - (OUT) The priority assing to the lif_stack_level.
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_petra_stat_lif_counting_stack_level_priority_get(
    int unit, 
    uint32 flags, 
    bcm_stat_lif_counting_source_t *source, 
    bcm_stat_counter_lif_stack_id_t lif_stack_level, 
    int *priority)
{
    SOC_TMC_CNT_SRC_TYPE counter_source;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(source);
    BCMDNX_NULL_CHECK(priority);
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported API for device type.")));
    }
    if (source->type == bcmStatCounterSourceIngressInlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_ING_PP;
    } else if (source->type == bcmStatCounterSourceEgressTransmitOutlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_EPNI;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source->type));
    }
    if (source->command_id != 0 && source->command_id != 1){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid command ID %d."), source->command_id));
    }
    if (lif_stack_level >= BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF stack level %d."), lif_stack_level));
    }
    rv = bcm_dpp_counter_lif_counting_priority_get(unit, counter_source, source->command_id, lif_stack_level, priority);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Function:
 *      bcm_petra_stat_lif_counting_stack_level_priority_set
 * Purpose:
 *      Set priority LIF-stack-level X LIF-counting-source X command-ID
 *      For each packet the LIF-stack-level with the higher priority will be couted.
 *      If two LIF-stack-levels have the same priority the outter-more LIF will ve counted.
 *      
 * Parameters:
 *      unit                  - (IN) Unit number.
 *      flags                 - (IN) flags - not in use
 *      source                - (IN) LIF-counting-source X command-ID:.
 *      lif_stack_level       - (IN) A level of LIF on the packet
 *      priority              - (IN) The priority assing to the lif_stack_level.
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_petra_stat_lif_counting_stack_level_priority_set(
    int unit, 
    uint32 flags, 
    bcm_stat_lif_counting_source_t *source, 
    bcm_stat_counter_lif_stack_id_t lif_stack_level, 
    int priority)
{
    SOC_TMC_CNT_SRC_TYPE counter_source;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported API for device type.")));
    }
    BCMDNX_NULL_CHECK(source);
    if (source->type == bcmStatCounterSourceIngressInlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_ING_PP;
    } else if (source->type == bcmStatCounterSourceEgressTransmitOutlif) {
        counter_source = SOC_TMC_CNT_SRC_TYPE_EPNI;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source->type));
    }
    if (source->command_id != 0 && source->command_id != 1){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid command ID %d."), source->command_id));
    }
    if (priority < 0 || priority >= BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid priority %d."), priority));
    }
    rv = bcm_dpp_counter_lif_counting_priority_set(unit, counter_source, source->command_id, lif_stack_level, priority);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_stat_counter_bcm_to_soc_filter_convert(
        int unit,
        bcm_stat_counter_filter_t* bcm_filter_array,
        int bcm_filter_count, 
        SOC_TMC_CNT_FILTER_TYPE* soc_filter_array,
        int soc_filter_count_max,
        int* soc_filter_count)
{
    int filter_array_iter;
    bcm_stat_counter_filter_t current_bcm_filter;
    SOC_TMC_CNT_FILTER_TYPE current_soc_filter;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(bcm_filter_array);
    BCMDNX_NULL_CHECK(soc_filter_array);
    BCMDNX_NULL_CHECK(soc_filter_count);
    if (bcm_filter_count > soc_filter_count_max) 
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "overflow of filter array\n"))); 
        BCMDNX_IF_ERR_EXIT(BCM_E_FULL);     
    }

    for (filter_array_iter = 0; filter_array_iter < bcm_filter_count; ++filter_array_iter) 
    {
        current_bcm_filter = bcm_filter_array[filter_array_iter];
        switch (current_bcm_filter) {
        case bcmStatCounterTotalPDsThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_TOTAL_PDS_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterTotalPDsUcPoolSizeThresholdViolated:
             current_soc_filter = SOC_TMC_CNT_TOTAL_PDS_UC_POOL_SIZE_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterPerPortUcPDsThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_PER_PORT_UC_PDS_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterPerQueueUcPDsThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_PER_QUEUE_UC_PDS_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterPerPortUcDBsThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_PER_PORT_UC_DBS_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterPerQueueUcDBsThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_PER_QUEUE_UC_DBS_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterPerQueueDisableBit:
            current_soc_filter = SOC_TMC_CNT_PER_QUEUE_DISABLE_BIT;
            break;
        case bcmStatCounterTotalPDsMcPoolSizeThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_TOTAL_PDS_MC_POOL_SIZE_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterPerInterfacePDsThreholdViolated:
            current_soc_filter = SOC_TMC_CNT_PER_INTERFACE_PDS_THREHOLD_VIOLATED;
            break;
        case bcmStatCounterMcSPThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_MC_SP_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterPerMcTCThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_PER_MC_TC_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterMcPDsPerPortThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_MC_PDS_PER_PORT_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterMcPDsPerQueueThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_MC_PDS_PER_QUEUE_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterMcPerPortSizeThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_MC_PER_PORT_SIZE_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterMcPerQueueSizeThresholdViolated:
            current_soc_filter = SOC_TMC_CNT_MC_PER_QUEUE_SIZE_THRESHOLD_VIOLATED;
            break;
        case bcmStatCounterGlobalRejectDiscards:
            current_soc_filter = SOC_TMC_CNT_GLOBAL_REJECT_DISCARDS;
            break;
        case bcmStatCounterDramRejectDiscards:
            current_soc_filter = SOC_TMC_CNT_DRAM_REJECT_DISCARDS;
            break;
        case bcmStatCounterVoqTailDropDiscards:
            current_soc_filter = SOC_TMC_CNT_VOQ_TAIL_DROP_DISCARDS;
            break;
        case bcmStatCounterVoqStatisticsDiscards:
            current_soc_filter = SOC_TMC_CNT_VOQ_STATISTICS_DISCARDS;
            break;
        case bcmStatCounterVsqTailDropDiscards:
            current_soc_filter = SOC_TMC_CNT_VSQ_TAIL_DROP_DISCARDS;
            break;
        case bcmStatCounterVsqStatisticsDiscards:
            current_soc_filter = SOC_TMC_CNT_VSQ_STATISTICS_DISCARDS;
            break;
        case bcmStatCounterQueueNotValidDiscard:
            current_soc_filter = SOC_TMC_CNT_QUEUE_NOT_VALID_DISCARD;
            break;
        case bcmStatCounterOtherDiscards:
            current_soc_filter = SOC_TMC_CNT_OTHER_DISCARDS;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "filter criterion %d is invalid\n"), current_bcm_filter));
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        soc_filter_array[filter_array_iter] = current_soc_filter;
    }
    *soc_filter_count = bcm_filter_count;

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_stat_counter_soc_to_bcm_filter_convert(
        int unit,
        SOC_TMC_CNT_FILTER_TYPE* soc_filter_array,
        int soc_filter_count, 
        bcm_stat_counter_filter_t* bcm_filter_array,
        int bcm_filter_count_max,
        int* bcm_filter_count)
{
    int filter_array_iter;
    bcm_stat_counter_filter_t current_bcm_filter;
    SOC_TMC_CNT_FILTER_TYPE current_soc_filter;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(bcm_filter_array);
    BCMDNX_NULL_CHECK(soc_filter_array);
    BCMDNX_NULL_CHECK(bcm_filter_count);
    if (soc_filter_count > bcm_filter_count_max) 
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "overflow of filter array\n"))); 
        BCMDNX_IF_ERR_EXIT(BCM_E_FULL);     
    }

    for (filter_array_iter = 0; filter_array_iter < soc_filter_count; ++filter_array_iter) 
    {
        current_soc_filter = soc_filter_array[filter_array_iter];
        switch (current_soc_filter) {
        case SOC_TMC_CNT_TOTAL_PDS_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterTotalPDsThresholdViolated;
            break;
        case SOC_TMC_CNT_TOTAL_PDS_UC_POOL_SIZE_THRESHOLD_VIOLATED:
             current_bcm_filter = bcmStatCounterTotalPDsUcPoolSizeThresholdViolated;
            break;
        case SOC_TMC_CNT_PER_PORT_UC_PDS_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterPerPortUcPDsThresholdViolated;
            break;
        case SOC_TMC_CNT_PER_QUEUE_UC_PDS_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterPerQueueUcPDsThresholdViolated;
            break;
        case SOC_TMC_CNT_PER_PORT_UC_DBS_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterPerPortUcDBsThresholdViolated;
            break;
        case SOC_TMC_CNT_PER_QUEUE_UC_DBS_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterPerQueueUcDBsThresholdViolated;
            break;
        case SOC_TMC_CNT_PER_QUEUE_DISABLE_BIT:
            current_bcm_filter = bcmStatCounterPerQueueDisableBit;
            break;
        case SOC_TMC_CNT_TOTAL_PDS_MC_POOL_SIZE_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterTotalPDsMcPoolSizeThresholdViolated;
            break;
        case SOC_TMC_CNT_PER_INTERFACE_PDS_THREHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterPerInterfacePDsThreholdViolated;
            break;
        case SOC_TMC_CNT_MC_SP_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterMcSPThresholdViolated;
            break;
        case SOC_TMC_CNT_PER_MC_TC_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterPerMcTCThresholdViolated;
            break;
        case SOC_TMC_CNT_MC_PDS_PER_PORT_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterMcPDsPerPortThresholdViolated;
            break;
        case SOC_TMC_CNT_MC_PDS_PER_QUEUE_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterMcPDsPerQueueThresholdViolated;
            break;
        case SOC_TMC_CNT_MC_PER_PORT_SIZE_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterMcPerPortSizeThresholdViolated;
            break;
        case SOC_TMC_CNT_MC_PER_QUEUE_SIZE_THRESHOLD_VIOLATED:
            current_bcm_filter = bcmStatCounterMcPerQueueSizeThresholdViolated;
            break;
        case SOC_TMC_CNT_GLOBAL_REJECT_DISCARDS:
            current_bcm_filter = bcmStatCounterGlobalRejectDiscards;
            break;
        case SOC_TMC_CNT_DRAM_REJECT_DISCARDS:
            current_bcm_filter = bcmStatCounterDramRejectDiscards;
            break;
        case SOC_TMC_CNT_VOQ_TAIL_DROP_DISCARDS:
            current_bcm_filter = bcmStatCounterVoqTailDropDiscards;
            break;
        case SOC_TMC_CNT_VOQ_STATISTICS_DISCARDS:
            current_bcm_filter = bcmStatCounterVoqStatisticsDiscards;
            break;
        case SOC_TMC_CNT_VSQ_TAIL_DROP_DISCARDS:
            current_bcm_filter = bcmStatCounterVsqTailDropDiscards;
            break;
        case SOC_TMC_CNT_VSQ_STATISTICS_DISCARDS:
            current_bcm_filter = bcmStatCounterVsqStatisticsDiscards;
            break;
        case SOC_TMC_CNT_QUEUE_NOT_VALID_DISCARD:
            current_bcm_filter = bcmStatCounterQueueNotValidDiscard;
            break;
        case SOC_TMC_CNT_OTHER_DISCARDS:
            current_bcm_filter = bcmStatCounterOtherDiscards;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "filter criterion %d is invalid\n"), current_soc_filter));
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        soc_filter_array[filter_array_iter] = current_bcm_filter;
    }
    *bcm_filter_count = soc_filter_count;

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_stat_counter_bcm_to_soc_counter_source_convert(
    int unit,
    bcm_stat_counter_source_t* bcm_source,
    SOC_TMC_CNT_SOURCE* soc_source)
{
    bcm_stat_counter_source_type_t current_bcm_engine_source;
    SOC_TMC_CNT_SRC_TYPE current_soc_engine_source;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(bcm_source);
    BCMDNX_NULL_CHECK(soc_source);

    soc_source->command_id = bcm_source->command_id;

    current_bcm_engine_source = bcm_source->engine_source;
    switch (current_bcm_engine_source) {
    case bcmStatCounterSourceEgressReceiveTmPort:
    case bcmStatCounterSourceEgressReceiveQueue:
        current_soc_engine_source = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
        break;
    case bcmStatCounterSourceIngressVoq:
        current_soc_engine_source = SOC_TMC_CNT_SRC_TYPE_VOQ;
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unsupported counter source\n")));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

    soc_source->source_type = current_soc_engine_source;

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_stat_counter_filter_set(
      int unit,
      bcm_stat_counter_source_t source,
      bcm_stat_counter_filter_t* filter_array,
      int filter_count,
      int is_active)
{
    int internal_filter_count;
    /* soc layer source */
    SOC_TMC_CNT_SOURCE internal_source;
    /* soc layer filter array in max size */
    SOC_TMC_CNT_FILTER_TYPE internal_filter_array[SOC_TMC_CNT_DROP_REASON_COUNT];

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(filter_array);
    /* convert bcm layer source to soc layer source */
    BCMDNX_IF_ERR_EXIT(bcm_petra_stat_counter_bcm_to_soc_counter_source_convert(unit, &source, &internal_source));
    /* convert bcm layer filter_array to soc layer filter array */
    BCMDNX_IF_ERR_EXIT(bcm_petra_stat_counter_bcm_to_soc_filter_convert(unit, filter_array, filter_count, internal_filter_array, SOC_TMC_CNT_DROP_REASON_COUNT, &internal_filter_count));
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_counter_filter_set, (unit,&internal_source,internal_filter_array,internal_filter_count,is_active)));

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_stat_counter_filter_get(
      int unit,
      bcm_stat_counter_source_t source,
      int filter_max_count,
      bcm_stat_counter_filter_t* filter_array,
      int* filter_count)
{
    int internal_filter_count;
    /* soc layer source */
    SOC_TMC_CNT_SOURCE internal_source;
    /* soc layer filter array in max size */
    SOC_TMC_CNT_FILTER_TYPE internal_filter_array[SOC_TMC_CNT_DROP_REASON_COUNT];

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(filter_count);
    BCMDNX_NULL_CHECK(filter_array);
    /* convert bcm layer source to soc layer source */
    BCMDNX_IF_ERR_EXIT(bcm_petra_stat_counter_bcm_to_soc_counter_source_convert(unit, &source, &internal_source));
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_counter_filter_get, (unit,&internal_source,SOC_TMC_CNT_DROP_REASON_COUNT,internal_filter_array,&internal_filter_count)));
    /* convert soc layer filter array to bcm layer filter_array */
    BCMDNX_IF_ERR_EXIT(bcm_petra_stat_counter_soc_to_bcm_filter_convert(unit, internal_filter_array, internal_filter_count, filter_array, filter_max_count, filter_count));
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_stat_counter_filter_is_active_get(
      int unit,
      bcm_stat_counter_source_t source,
      bcm_stat_counter_filter_t filter,
      int* is_active)
{
    int filter_iter = 0;
    int filter_count = 0;
    bcm_stat_counter_filter_t filter_array[bcmStatCounterDropReasonCount];
    int internal_filter_count;
    /* soc layer source */
    SOC_TMC_CNT_SOURCE internal_source;
    /* soc layer filter array in max size */
    SOC_TMC_CNT_FILTER_TYPE internal_filter_array[SOC_TMC_CNT_DROP_REASON_COUNT];

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_active);
    *is_active = 0;

    /* convert bcm layer source to soc layer source */
    BCMDNX_IF_ERR_EXIT(bcm_petra_stat_counter_bcm_to_soc_counter_source_convert(unit, &source, &internal_source));
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_counter_filter_get, (unit,&internal_source,SOC_TMC_CNT_DROP_REASON_COUNT,internal_filter_array,&internal_filter_count)));
    /* convert soc layer filter array to bcm layer filter_array */
    BCMDNX_IF_ERR_EXIT(bcm_petra_stat_counter_soc_to_bcm_filter_convert(unit, internal_filter_array, internal_filter_count, filter_array, bcmStatCounterDropReasonCount, &filter_count));
    for (filter_iter = 0; filter_iter < filter_count; ++filter_iter) 
    {
        if (filter_array[filter_iter] == filter) {
            *is_active = 1;
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

