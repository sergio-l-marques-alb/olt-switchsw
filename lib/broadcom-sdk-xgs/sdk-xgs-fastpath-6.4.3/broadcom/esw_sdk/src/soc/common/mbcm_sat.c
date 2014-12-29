/*
 * $Id: mbcm.c,v 1.4 Broadcom SDK $
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
 * File:        mbcm.c
 * Purpose:     Implementation of bcm multiplexing
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/shared/mbcm_sat.h>
#ifdef BCM_SAT_SUPPORT

CONST mbcm_sat_functions_t    *mbcm_sat_driver[BCM_MAX_NUM_UNITS]={0};
soc_sat_chip_family_t    mbcm_sat_family[BCM_MAX_NUM_UNITS];

CONST mbcm_sat_functions_t mbcm_sat_fun_driver = {
    soc_sat_config_set,
    soc_sat_config_get,
    /* GTF functions */
    soc_sat_general_cfg_init,
    soc_sat_gtf_packet_gen_set,
    soc_sat_gtf_packet_gen_get,
    soc_sat_gtf_packet_config,
    soc_sat_gtf_bandwidth_set,
    soc_sat_gtf_bandwidth_get,
    soc_sat_gtf_rate_pattern_set,
    soc_sat_gtf_rate_pattern_get,
    soc_sat_gtf_stat_get,
    /* CTF functions */
    soc_sat_ctf_packet_config,
    soc_sat_ctf_identifier_map,
    soc_sat_ctf_identifier_unmap,
    soc_sat_ctf_trap_set,
    soc_sat_ctf_bin_limit_set,
    soc_sat_ctf_bin_limit_get,
    soc_sat_ctf_stat_config_set,
    soc_sat_ctf_stat_get,
    soc_sat_ctf_availability_config_set,
    soc_sat_ctf_trap_data_to_session_map,
    soc_sat_ctf_trap_data_to_session_unmap,
    soc_sat_ctf_reports_config_set,
};


/****************************************************************
 *
 * Function:        mbcm_sat_init
 * Parameters:      unit   --   unit to setup
 * 
 * Initialize the mbcm  sat driver for the indicated unit.
 *
 ****************************************************************/
int
mbcm_sat_init(int unit)
{
#ifdef BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit)) {
        mbcm_sat_driver[unit] = &mbcm_sat_fun_driver;
        mbcm_sat_family[unit] = SOC_SAT_FAMILY_JERICHO;
        return SOC_E_NONE;
    }
#endif /* BCM_JERICHO_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
        if (SOC_IS_SABER2(unit)) {
            mbcm_sat_driver[unit] = &mbcm_sat_fun_driver;
            mbcm_sat_family[unit] = SOC_SAT_FAMILY_SABER2;
            return SOC_E_NONE;
        }
#endif /* BCM_SABER2_SUPPORT */


    LOG_BSL_INFO(BSL_LS_BCM_INIT,
             (BSL_META_U(unit,
                         "ERROR: mbcm_sat_init unit %d: unsupported chip type\n"), unit));
    return SOC_E_UNAVAIL;
}
#endif /* BCM_SAT_SUPPORT  */
