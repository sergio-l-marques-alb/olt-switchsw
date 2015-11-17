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

#ifdef BCM_ARAD_SUPPORT
/*
 * Function:
 *      soc_petra_stat_type_to_id
 * Purpose:
 *      given counter type return counter id
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      type  - (IN)     SNMP statistics type defined in bcm_stat_val_t
 *      cnt_type - (OUT) Array of identifiers of counters. Loaded by this procedure.
 *      num_cntrs_p - (IN + OUT)
 *                       As input: Maximal number of counters caller is willing to
 *                       accept.
 *                       As output: Number of counters actually loaded into 'cnt_type'
 *                       array.
 * Returns:
 *      SOC_E_NONE        Success  
 *      SOC_E_UNAVAIL     Counter not supported for the port
 *      SOC_E_XXX         Operation failed
 */
STATIC int
_bcm_arad_stat_type_to_id(int unit, bcm_port_t port,
    bcm_stat_val_t type, ARAD_NIF_COUNTER_TYPE *cnt_type, int *num_cntrs_p)
{
    int pcs;
    int num_cntrs ;
    BCMDNX_INIT_FUNC_DEFS;

    if ((cnt_type == (ARAD_NIF_COUNTER_TYPE *)0) || (num_cntrs_p == (int *)0)) {
        BCMDNX_ERR_EXIT_MSG(
            BCM_E_PARAM,
                (_BSL_BCM_MSG("Either cnt_type (0x%08lX) or num_cntrs_p (0x%08lX) is NULL pointer"),
                                            (unsigned long)cnt_type, (unsigned long)num_cntrs_p)); 
    }
    if (*num_cntrs_p <= 0) {
        BCMDNX_ERR_EXIT_MSG(
            BCM_E_PARAM,(_BSL_BCM_MSG("*num_cntrs_p (%d) is zero or negative"), *num_cntrs_p)); 
    }
    /*
     * At this point, *num_cntrs_p is at least '1'.
     */
    if (!SOC_ARAD_STAT_COUNTER_MODE_PHISYCAL(unit, port)) {
        *num_cntrs_p = 1 ;
        switch(type) {
        case snmpIfInBroadcastPkts:
            cnt_type[0] = ARAD_NIF_RX_BCAST_PACKETS;
            break;
        case snmpIfInMulticastPkts:
            cnt_type[0] = ARAD_NIF_RX_MCAST_BURSTS;
            break;
        case snmpIfInErrors:
            cnt_type[0] = ARAD_NIF_RX_ERR_PACKETS;
            break;
        case snmpEtherStatsUndersizePkts:
            cnt_type[0] = ARAD_NIF_RX_LEN_BELOW_MIN;
            break;
        case snmpIfOutBroadcastPkts: 
            cnt_type[0] = ARAD_NIF_TX_BCAST_PACKETS;
            break;
        case snmpIfOutMulticastPkts: 
            cnt_type[0] = ARAD_NIF_TX_MCAST_BURSTS;
            break;
        case snmpIfOutErrors: 
            cnt_type[0] = ARAD_NIF_TX_ERR_PACKETS;
            break;
        case snmpIfInOctets:  
            cnt_type[0] = ARAD_NIF_RX_OK_OCTETS;
            break;
        case snmpIfOutOctets:  
            cnt_type[0] = ARAD_NIF_TX_OK_OCTETS;
            break;
        case snmpEtherStatsRXNoErrors:
            cnt_type[0] = ARAD_NIF_RX_OK_PACKETS;
            break;
        case snmpIfInNUcastPkts:
            cnt_type[0] = ARAD_NIF_RX_NON_UNICAST_PACKETS;
            break;
        case snmpEtherStatsTXNoErrors:   
            cnt_type[0] = ARAD_NIF_TX_OK_PACKETS;
            break;
        case snmpIfOutNUcastPkts:
            cnt_type[0] = ARAD_NIF_TX_NON_UNICAST_PACKETS;
            break;
        default:
            *num_cntrs_p = 0 ;
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("stat type %d isn't supported"), type)); 
        }
    } else {
        num_cntrs = *num_cntrs_p ;
        *num_cntrs_p = 1 ;
        switch(type) {
            case snmpIfInBroadcastPkts:
                cnt_type[0] = ARAD_NIF_RX_BCAST_PACKETS;
                break;
            case snmpIfInMulticastPkts:
                cnt_type[0] = ARAD_NIF_RX_MCAST_BURSTS;
                break;
            case snmpIfInErrors:
                cnt_type[0] = ARAD_NIF_RX_ERR_PACKETS;
                break;
            case snmpEtherStatsUndersizePkts:
                cnt_type[0] = ARAD_NIF_RX_LEN_BELOW_MIN;
                break;
            
            case snmpBcmReceivedPkts64Octets:
            case snmpEtherStatsPkts64Octets:
                cnt_type[0] = ARAD_NIF_RX_LEN_60;
                break;
            case snmpBcmReceivedPkts65to127Octets:
            case snmpEtherStatsPkts65to127Octets: 
                cnt_type[0] = ARAD_NIF_RX_LEN_61_123;
                break;
            case snmpBcmReceivedPkts128to255Octets:
            case snmpEtherStatsPkts128to255Octets: 
                cnt_type[0] = ARAD_NIF_RX_LEN_124_251;
                break;
            case snmpBcmReceivedPkts256to511Octets:
            case snmpEtherStatsPkts256to511Octets: 
                cnt_type[0] = ARAD_NIF_RX_LEN_252_507;
                break;
            case snmpBcmReceivedPkts512to1023Octets:
            case snmpEtherStatsPkts512to1023Octets: 
                cnt_type[0] = ARAD_NIF_RX_LEN_508_1019;
                break;
            case snmpBcmReceivedPkts1024to1518Octets:
            case snmpEtherStatsPkts1024to1518Octets:  /* not same range */
                cnt_type[0] = ARAD_NIF_RX_LEN_1020_1514;
                break;
            case snmpEtherStatsOversizePkts: 
                cnt_type[0] = ARAD_NIF_RX_LEN_1515CFG_MAX;
                /*cnt_type = ARAD_NIF_RX_LEN_ABOVE_MAX;*/
                break;
            case snmpBcmReceivedPkts2048to4095Octets:
                cnt_type[0] = ARAD_NIF_RX_LEN_2044_4091;
                break;
            case snmpBcmReceivedPkts4095to9216Octets:
                cnt_type[0] = ARAD_NIF_RX_LEN_4092_9212;
                break;
            case snmpIfOutBroadcastPkts: 
                cnt_type[0] = ARAD_NIF_TX_BCAST_PACKETS;
                break;
            case snmpIfOutMulticastPkts: 
                cnt_type[0] = ARAD_NIF_TX_MCAST_BURSTS;
                break;
            case snmpIfOutErrors: 
                cnt_type[0] = ARAD_NIF_TX_ERR_PACKETS;
                break;
            case snmpIfInOctets:  
                cnt_type[0] = ARAD_NIF_RX_OK_OCTETS;
                break;
            case snmpIfOutOctets:  
                cnt_type[0] = ARAD_NIF_TX_OK_OCTETS;
                break;
            case snmpEtherStatsRXNoErrors:
                cnt_type[0] = ARAD_NIF_RX_OK_PACKETS;
                break;
            case snmpIfInNUcastPkts:
                cnt_type[0] = ARAD_NIF_RX_NON_UNICAST_PACKETS;
                break;
            case snmpEtherStatsTXNoErrors:   
                cnt_type[0] = ARAD_NIF_TX_OK_PACKETS;
                break;
            case snmpIfOutNUcastPkts:
                cnt_type[0] = ARAD_NIF_TX_NON_UNICAST_PACKETS;
                break;
            case snmpBcmTransmittedPkts64Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_60;
                break;
            case snmpBcmTransmittedPkts65to127Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_61_123;
                break;
            case snmpBcmTransmittedPkts128to255Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_124_251;
                break;
            case snmpBcmTransmittedPkts256to511Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_252_507;
                break;
            case snmpBcmTransmittedPkts512to1023Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_508_1019;
                break;
            case snmpBcmTransmittedPkts1024to1518Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_1020_1514;
                break;
            case snmpBcmTransmittedPkts1519to2047Octets: 
                cnt_type[0] = ARAD_NIF_TX_LEN_1519_2043;
                if (num_cntrs <= 1) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_FULL,
                        (_BSL_BCM_MSG("provided num_cntrs (%d) is smaller than required (2)"),num_cntrs)) ; 
                }
                *num_cntrs_p = 2 ;
                cnt_type[1] = ARAD_NIF_TX_LEN_1515_1518;
                break;
            case snmpBcmTransmittedPkts2048to4095Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_2044_4091;
                break;
            case snmpBcmTransmittedPkts4095to9216Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_4092_9212;
                break;
            case snmpBcmTxControlCells:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_TX_CONTROL_CELLS_COUNTER;
                break;
            case snmpBcmTxDataCells:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_TX_DATA_CELL_COUNTER;
                break;
            case snmpBcmTxDataBytes:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_TX_DATA_BYTE_COUNTER;
                break;
            case snmpBcmRxCrcErrors:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_CRC_ERRORS_COUNTER;
                break;
            case snmpBcmRxFecCorrectable:
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(ARAD_PORT_PCS_8_9_LEGACY_FEC != pcs && ARAD_PORT_PCS_64_66_FEC != pcs) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("port: %d, counter %d supported only for FEC ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_CORRECTABLE_ERROR;
                break;
            case snmpBcmRxBecCrcErrors:
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(ARAD_PORT_PCS_64_66_BEC != pcs) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("port: %d, counter %d supported only for BEC ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_BEC_CRC_ERROR;
                break;
            case snmpBcmRxDisparityErrors:
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(ARAD_PORT_PCS_8_10 != pcs) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("port: %d, counter %d supported only for 8b/10b ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_8B_10B_DISPARITY_ERRORS;
                break;
            case snmpBcmRxControlCells:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_CONTROL_CELLS_COUNTER;
                break;
            case snmpBcmRxDataCells:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_DATA_CELL_COUNTER;
                break;
            case snmpBcmRxDataBytes:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_DATA_BYTE_COUNTER;
                break;
            case snmpBcmRxDroppedRetransmittedControl:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_DROPPED_RETRANSMITTED_CONTROL;
                break;
            case snmpBcmTxBecRetransmit:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_TX_BEC_RETRANSMIT;
                break;
            case snmpBcmRxBecRetransmit:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_BEC_RETRANSMIT;
                break;
            case snmpBcmTxAsynFifoRate:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_TX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS;
                break;
            case snmpBcmRxAsynFifoRate:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS;
                break;
            case snmpBcmRxFecUncorrectable:
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(ARAD_PORT_PCS_8_9_LEGACY_FEC != pcs && ARAD_PORT_PCS_64_66_FEC != pcs) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("port: %d, counter %d supported only for FEC ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_UNCORRECTABLE_ERRORS;
                break;
            case snmpBcmRxBecRxFault:
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(ARAD_PORT_PCS_64_66_BEC != pcs) {
                    BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_SOC_MSG("port: %d, counter %d supported only for BEC ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_BEC_RX_FAULT;
                break;
            case snmpBcmRxCodeErrors:
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(ARAD_PORT_PCS_8_10 != pcs) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("port: %d, counter %d supported only for 8b/10b ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_8B_10B_CODE_ERRORS;
                break;
            case snmpBcmRxLlfcPrimary:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_LLFC_PRIMARY;
                break;
            case snmpBcmRxLlfcSecondary:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_LLFC_SECONDARY;
                break;
            default:
                *num_cntrs_p = 0 ;
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("stat type %d isn't supported"), type)); 
            }
        }
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_ARAD_SUPPORT */
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
    uint32 cnt_type[2] = {0xffffff,0xffffff} ;
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
        if(BCM_E_NONE != _bcm_arad_stat_type_to_id(unit, port, type, cnt_type, &num_cntrs)) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("type %d isn't supported for port %d"), type, port));
        }
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

#undef _ERR_MSG_MODULE_NAME

