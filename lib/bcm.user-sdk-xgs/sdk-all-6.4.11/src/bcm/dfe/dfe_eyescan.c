/*
 * $Id: dfe_eyescan.c,v 1.3 Broadcom SDK $
 *
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
 * DFE EYESCAN
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_PORT
#include <shared/bsl.h>
#include <soc/eyescan.h>
#include <shared/alloc.h>
#include <bcm_int/common/debug.h>
#include <bcm/error.h>
#include <bcm/stat.h>

#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_config_defs.h>
#include <soc/dfe/cmn/dfe_config_imp_defs.h>


static uint64* dfe_saved_counter_1[BCM_LOCAL_UNITS_MAX];
static uint64* dfe_saved_counter_2[BCM_LOCAL_UNITS_MAX];

STATIC int
_dfe_eyescan_mac_prbs_counter_clear(int unit, soc_port_t port)
{
    int  rc, err_count;
    BCMDNX_INIT_FUNC_DEFS;

   rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_rx_status_get,(unit, port, 1, &err_count));
   BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_dfe_eyescan_mac_prbs_counter_get(int unit, soc_port_t port, uint32* err_count)
{
    int  rc;
    BCMDNX_INIT_FUNC_DEFS;

   rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_rx_status_get,(unit, port, 1, (int*)err_count));
   BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_dfe_eyescan_mac_crc_counter_clear(int unit, soc_port_t port)
{
    int  rc;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_stat_sync(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = bcm_stat_get(unit, port, snmpBcmRxCrcErrors, &(dfe_saved_counter_1[unit][port]));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_dfe_eyescan_mac_crc_counter_get(int unit, soc_port_t port, uint32* err_count)
{
    int  rc;
    uint64 data;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_stat_sync(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = bcm_stat_get(unit, port, snmpBcmRxCrcErrors, &data);
    BCMDNX_IF_ERR_EXIT(rc);

    COMPILER_64_SUB_64(data, dfe_saved_counter_1[unit][port]);
   *err_count =  COMPILER_64_LO(data);
exit:
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_dfe_eyescan_mac_ber_counter_clear(int unit, soc_port_t port)
{
    int  rc, counter_type_1, counter_type_2;
    int pcs;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_port_control_get(unit, port, bcmPortControlPCS, &pcs);
    BCMDNX_IF_ERR_EXIT(rc);

    switch(pcs) {
    case soc_dcmn_port_pcs_64_66_bec:
        counter_type_1 = snmpBcmRxBecCrcErrors;
        counter_type_2 = snmpBcmRxBecRxFault;
        break;
    case soc_dcmn_port_pcs_8_9_legacy_fec:
    case soc_dcmn_port_pcs_64_66_fec:
        counter_type_1 = snmpBcmRxFecCorrectable;
        counter_type_2 = snmpBcmRxFecUncorrectable;
        break;
    case soc_dcmn_port_pcs_8_10:
        counter_type_1 = snmpBcmRxDisparityErrors;
        counter_type_2 = snmpBcmRxCodeErrors;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unsupported PCS")));
        break;
    }

    rc = bcm_stat_sync(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = bcm_stat_get(unit, port, counter_type_1, &(dfe_saved_counter_1[unit][port]));
    BCMDNX_IF_ERR_EXIT(rc);
    rc = bcm_stat_get(unit, port, counter_type_2, &(dfe_saved_counter_2[unit][port]));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_dfe_eyescan_mac_ber_counter_get(int unit, soc_port_t port, uint32* err_count)
{
    int  rc, counter_type_1, counter_type_2;
    int pcs;
    uint64 data1, data2;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_port_control_get(unit, port, bcmPortControlPCS, &pcs);
    BCMDNX_IF_ERR_EXIT(rc);

    switch(pcs) {
    case soc_dcmn_port_pcs_64_66_bec:
        counter_type_1 = snmpBcmRxBecCrcErrors;
        counter_type_2 = snmpBcmRxBecRxFault;
        break;
    case soc_dcmn_port_pcs_8_9_legacy_fec:
    case soc_dcmn_port_pcs_64_66_fec:
        counter_type_1 = snmpBcmRxFecCorrectable;
        counter_type_2 = snmpBcmRxFecUncorrectable;
        break;
    case soc_dcmn_port_pcs_8_10:
        counter_type_1 = snmpBcmRxDisparityErrors;
        counter_type_2 = snmpBcmRxCodeErrors;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unsupported PCS")));
        break;
    }

    rc = bcm_stat_sync(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = bcm_stat_get(unit, port, counter_type_1, &data1);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = bcm_stat_get(unit, port, counter_type_2, &data2);
    BCMDNX_IF_ERR_EXIT(rc);

    COMPILER_64_SUB_64(data1, dfe_saved_counter_1[unit][port]);
    COMPILER_64_SUB_64(data2, dfe_saved_counter_2[unit][port]);
   *err_count =  COMPILER_64_LO(data1) + COMPILER_64_LO(data2);
exit:
    BCMDNX_FUNC_RETURN; 
}

int 
bcm_dfe_eyescan_init(int unit)
{
    int  rv;
    soc_port_phy_eyescan_counter_cb_t cbs;
    int nof_links;
    BCMDNX_INIT_FUNC_DEFS;

    nof_links = SOC_DFE_DEFS_GET(unit, nof_links);

    BCMDNX_ALLOC(dfe_saved_counter_1[unit], sizeof(uint64)*nof_links, "Eyescan counters set1");
    BCMDNX_ALLOC(dfe_saved_counter_2[unit], sizeof(uint64)*nof_links, "Eyescan counters set2");

    cbs.clear_func = _dfe_eyescan_mac_prbs_counter_clear;
    cbs.get_func = _dfe_eyescan_mac_prbs_counter_get;
    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterPrbsMac, &cbs);
    BCMDNX_IF_ERR_EXIT(rv);

    cbs.clear_func = _dfe_eyescan_mac_crc_counter_clear;
    cbs.get_func = _dfe_eyescan_mac_crc_counter_get;
    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterCrcMac, &cbs);
    BCMDNX_IF_ERR_EXIT(rv);

    cbs.clear_func = _dfe_eyescan_mac_ber_counter_clear;
    cbs.get_func = _dfe_eyescan_mac_ber_counter_get;
    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterBerMac, &cbs);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN; 

}
 
int 
bcm_dfe_eyescan_deinit(int unit)
{
    int  rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterPrbsMac, NULL);
    BCMDNX_IF_ERR_CONT(rv);

    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterCrcMac, NULL);
    BCMDNX_IF_ERR_CONT(rv);

    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterBerMac, NULL);
    BCMDNX_IF_ERR_CONT(rv);

    BCM_FREE(dfe_saved_counter_1[unit]);
    BCM_FREE(dfe_saved_counter_2[unit]);

    BCMDNX_FUNC_RETURN; 

}

#undef _ERR_MSG_MODULE_NAME

