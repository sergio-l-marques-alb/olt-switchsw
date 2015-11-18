/* 
 * $Id: utils.c,v 1.5 Broadcom SDK $
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
 * File:        bcm_sand.h
 * Purpose:     Conversion between BCM and SOC_SAND types, and common macros/function for
 *              handling Dune's code.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>

#include <sal/core/libc.h>

#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/cosq.h>

#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/mbcm.h>


/*
 *   Function
 *      pbmp_from_ppd_port_bitmap
 *   Purpose
 *      Convert array of port as used by ppd api to bcm_pbmp_t. 
 *
 *   Parameters
 *      (IN)  pbmp         : bcm_pbmp_t to be filled
 *      (IN)  ports        : ports bitmap in soc_sand format
 *      (IN)  ports_len    : length of ports array, in longs
 *   Returns
 *       BCM_E_NONE - success
 *       BCM_E_*    - failure
 */

int
pbmp_from_ppd_port_bitmap(
    int unit,
    bcm_pbmp_t *pbmp,
    uint32 *ports, 
    int ports_len)
{
    int port_i, bits_per_long;
    BCMDNX_INIT_FUNC_DEFS;
    bits_per_long = sizeof(*ports) * 8;

    BCM_PBMP_CLEAR(*pbmp);
    
    if (ports_len > _SHR_PBMP_WIDTH) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "pbmp_from_ppd_port_bitmap: pbmp size is smaller than 'ports_len'")));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    for (port_i = 0; port_i < ports_len * bits_per_long; ++port_i) {        
        if ((ports[port_i / bits_per_long] & (1<<(port_i % bits_per_long))) != 0) {

            BCM_PBMP_PORT_ADD(*pbmp, port_i);
        }
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      pbmp_to_ppd_port_bitmap
 *   Purpose
 *      Convert bcm_pbmp_t to array of port as used by ppd api. 
 *
 *   Parameters
 *      (IN)  pbmp         : source bcm_pbmp_t bitmap
 *      (IN)  ports        : ports bitmap in soc_sand format, to be filled
 *      (IN)  ports_len    : length of ports array, in longs
 *   Returns
 *       BCM_E_NONE - success
 *       BCM_E_*    - failure
 */

int
pbmp_to_ppd_port_bitmap(
    int unit,
    bcm_pbmp_t *pbmp,
    uint32 *ports, 
    int ports_len)
{
    int port_i, offset, bits_per_long;

    BCMDNX_INIT_FUNC_DEFS;
    bits_per_long = sizeof(*ports) * 8;
       
    sal_memset(ports, 0x0, ports_len * sizeof(*ports));

    BCM_PBMP_ITER(*pbmp, port_i) {
        offset = port_i / bits_per_long;

        if (offset >= ports_len) {
            /* Bitmap is longer than the ports array. Stop copying */
            break;
        }

        ports[offset] |= (1<<(port_i % bits_per_long));
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      pbmp_from_ppd_port_bitmap_convert
 *   Purpose
 *      Convert array of port as used by ppd api to bcm_pbmp_t.
 *      Currently : Converts input from pp_to_local;  
 * 
 *
 *   Parameters
 *      (IN)  core_id      : core id
 *      (IN)  pbmp         : bcm_pbmp_t to be filled
 *      (IN)  ports        : ports bitmap in soc_sand format
 *      (IN)  ports_len    : length of ports array, in longs
 *   Returns
 *       BCM_E_NONE - success
 *       BCM_E_*    - failure
 */

int
pbmp_from_ppd_port_bitmap_convert(
    int unit,
    int core_id,
    uint32 *ports, 
    int ports_len,
    _bcm_dpp_convert_flag_t flag,
    bcm_pbmp_t *pbmp
    )
{
    int port_i, bits_per_long;
    bcm_port_t port_from_ppd;
    BCMDNX_INIT_FUNC_DEFS;
    bits_per_long = sizeof(*ports) * 8;

    BCM_PBMP_CLEAR(*pbmp);
    
    if (ports_len > _SHR_PBMP_WIDTH) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "pbmp_from_ppd_port_bitmap: pbmp size is smaller than 'ports_len'")));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    for (port_i = 0; port_i < ports_len * bits_per_long; ++port_i) {        
        if ((ports[port_i / bits_per_long] & (1<<(port_i % bits_per_long))) != 0) {
            switch (flag) {
            case _BCM_DPP_CONVERT_FLAG_NONE:
                port_from_ppd = port_i;
                break;
            case _BCM_DPP_CONVERT_FLAG_PP_TO_LOCAL:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_pp_to_local_port_get, (unit, core_id, port_i, &port_from_ppd)));
                break;
            case _BCM_DPP_CONVERT_FLAG_TM_TO_LOCAL:
                BCMDNX_IF_ERR_EXIT( MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_tm_to_local_port_get, (unit, core_id, port_i, &port_from_ppd)));
                break;
            default:
                LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "Conversion flag not supported")));
                BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
            }

            BCM_PBMP_PORT_ADD(*pbmp, port_from_ppd);
        }
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 *   Function
 *      pbmp_to_ppd_port_bitmap
 *   Purpose
 *      Convert bcm_pbmp_t to array of port as used by ppd api.
 *      Currently : Converts input from local to pp. Ports with different core are filltered.  
 * 
 *
 *   Parameters
 *      (IN)  core_id      : Expected core.
 *      (IN)  pbmp         : source bcm_pbmp_t bitmap
 *      (IN)  ports        : ports bitmap in soc_sand format, to be filled
 *      (IN)  ports_len    : length of ports array, in longs
 *   Returns
 *       BCM_E_NONE - success
 *       BCM_E_*    - failure
 */

int
pbmp_to_ppd_port_bitmap_convert(
    int unit,
    int core_id,
    bcm_pbmp_t *pbmp,
    int ports_len,
    _bcm_dpp_convert_flag_t flag,
    uint32 *ports
    )
{
    int port_i, offset, bits_per_long, core;
    uint32 flags;
    SOC_PPD_PORT soc_ppd_port_i;

    BCMDNX_INIT_FUNC_DEFS;
    bits_per_long = sizeof(*ports) * 8;
       
    sal_memset(ports, 0x0, ports_len * sizeof(*ports));

    BCM_PBMP_ITER(*pbmp, port_i) {
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags)); 
        if (SOC_PORT_IS_STAT_INTERFACE(flags))
        {
            continue;
        }
        switch (flag) {
        case _BCM_DPP_CONVERT_FLAG_NONE:
            soc_ppd_port_i = port_i;
            core = core_id;
            break;
        case _BCM_DPP_CONVERT_FLAG_LOCAL_TO_PP:
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
            if (core != core_id) {
                continue;
            }
            break;
        case _BCM_DPP_CONVERT_FLAG_LOCAL_TO_TM:
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
            if (core != core_id) {
                continue;
            }
            break;

        default:
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  "Conversion flag not supported")));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }
        offset = soc_ppd_port_i / bits_per_long;

        if (offset >= ports_len) {
            /* Bitmap is longer than the ports array. Stop copying */
            break;
        }

        ports[offset] |= (1<<(soc_ppd_port_i % bits_per_long));
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_mac_to_sand_mac
 * Purpose:
 *      Convert an L2 BCM-MAC to Soc_petra-M strucutre
 * Parameters:
 *      bcm_mac     (IN) BCM mac address structure
 *      soc_ppd_mac     (OUT) PPD MAC address structure
 */
int
 _bcm_petra_mac_to_sand_mac(bcm_mac_t bcm_mac, SOC_SAND_PP_MAC_ADDRESS *soc_ppd_mac)
{
    int indx;
    
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    soc_sand_SAND_PP_MAC_ADDRESS_clear(soc_ppd_mac);
    for(indx = 0; indx < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++indx) {
        soc_ppd_mac->address[indx] = bcm_mac[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - indx - 1];
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_mac_from_sand_mac
 * Purpose:
 *      Convert an L2 BCM-MAC to Soc_petra-M strucutre
 * Parameters:
 *      soc_ppd_mac     (IN) PPD MAC address structure
 *      bcm_mac     (OUT) BCM mac address structure
 */
int
 _bcm_petra_mac_from_sand_mac(bcm_mac_t bcm_mac, SOC_SAND_PP_MAC_ADDRESS *soc_ppd_mac)
{
    int indx;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    for(indx = 0; indx < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++indx) {
        bcm_mac[indx] = soc_ppd_mac->address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - indx - 1];
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 *   Function
 *      _bcm_petra_pbmp_filter_by_core
 *   Purpose
 *      Returns only ports of the input core. 
 *
 *   Parameters
 *      (IN)  core_id      : required core
 *      (IN)  pbmp         : bcm_pbmp_t of local ports
 *      (OUT) core_pbmp    : returned pbmp: ports of the core
 *   Returns
 *       BCM_E_NONE - success
 *       BCM_E_*    - failure
 */

int
_bcm_pbmp_filter_by_core(
    int unit,
    int core_id,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t *core_pbmp)
{

    bcm_port_t port_i;
    int        core;
    uint32     soc_ppd_port_i; 

    BCMDNX_INIT_FUNC_DEFS;

    BCM_PBMP_CLEAR(*core_pbmp);
    
    BCM_PBMP_ITER(pbmp, port_i) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));

        if (core == core_id) {
             BCM_PBMP_PORT_ADD(*core_pbmp, port_i);
        }
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


