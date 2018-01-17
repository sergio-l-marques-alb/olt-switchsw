/*
 * $Id: avs.c $
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
 * File:        avs.c
 * Purpose:
 * Requires:
 */
 
#include <shared/bsl.h>

#ifdef INCLUDE_AVS
#include <soc/avs.h>
#include <soc/i2c.h>

#define SOC_AVS_HR3_NUMBER_OF_CENTRALS 36
#define SOC_AVS_HR3_CENTRAL_XBMP0 0x0
#define SOC_AVS_HR3_CENTRAL_XBMP1 0xFFFFFFF0
#define SOC_AVS_HR3_FIRST_CENTRAL 0

#define SOC_AVS_HR3_NUMBER_OF_REMOTES 11
#define SOC_AVS_HR3_REMOTE_XBMP0 0x0
#define SOC_AVS_HR3_FIRST_REMOTE 0

#define SOC_AVS_HR3_VMIN_AVS SOC_AVS_DEFAULT_VMIN_AVS
#define SOC_AVS_HR3_VMAX_AVS SOC_AVS_DEFAULT_VMAX_AVS
#define SOC_AVS_HR3_VMARGIN_HIGH SOC_AVS_DEFAULT_VMARGIN_HIGH
#define SOC_AVS_HR3_VMARGIN_LOW SOC_AVS_DEFAULT_VMARGIN_LOW



static soc_avs_reg_info_t _hr3_osc_cen_init_list[]={
/* AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r */
{AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r,CEN_ROSC_ENABLE_DEFAULTf},
{AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1r,CEN_ROSC_ENABLE_DEFAULTf},
{-1,-1},
};

static soc_avs_reg_info_t _hr3_osc_cen_thr_en_list[]={
{AVS_REG_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_0r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_1r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_2_CEN_ROSC_THRESHOLD2_EN_0r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_2_CEN_ROSC_THRESHOLD2_EN_1r,IRQ_ENf},
{-1,-1},
};

/* reset osc measurement */
static soc_avs_reg_info_t _hr3_osc_seq_reset_list[]={
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_0r,M_INIT_CEN_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1r,M_INIT_CEN_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_0r,M_INIT_RMT_ROSCf},
{-1,-1}
};

int
_soc_hr3_avs_info_init(int unit, soc_avs_info_t *avs_info)
{    

    if (avs_info == NULL) {
        return (SOC_E_PARAM);
    }
       
    avs_info->num_centrals = SOC_AVS_HR3_NUMBER_OF_CENTRALS;
    avs_info->cent_xbmp[0] = SOC_AVS_HR3_CENTRAL_XBMP0;
    avs_info->cent_xbmp[1] = SOC_AVS_HR3_CENTRAL_XBMP1;
    avs_info->first_cent = SOC_AVS_HR3_FIRST_CENTRAL;

    avs_info->num_remotes = SOC_AVS_HR3_NUMBER_OF_REMOTES;
    avs_info->rmt_xbmp[0] = SOC_AVS_HR3_REMOTE_XBMP0;
    avs_info->first_rmt = SOC_AVS_HR3_FIRST_REMOTE;

    avs_info->vmin_avs = SOC_AVS_INT(SOC_AVS_HR3_VMIN_AVS);
    avs_info->vmax_avs = SOC_AVS_INT(SOC_AVS_HR3_VMAX_AVS);
    avs_info->vmargin_low  = SOC_AVS_INT(SOC_AVS_HR3_VMARGIN_LOW);
    avs_info->vmargin_high = SOC_AVS_INT(SOC_AVS_HR3_VMARGIN_HIGH);
    
    avs_info->rosc_count_mode = SOC_AVS_ROSC_COUNT_MODE_1EDGE;
    avs_info->ref_clk_freq = 25;
    avs_info->measurement_time_control = 127;
    avs_info->osc_cen_init_info = _hr3_osc_cen_init_list;
    avs_info->osc_cen_thr_en_info = _hr3_osc_cen_thr_en_list;
    avs_info->osc_seq_reset_info = _hr3_osc_seq_reset_list;

    avs_info->avs_flags = SOC_AVS_INFO_F_SET_THRESHOLDS |
                          SOC_AVS_INFO_F_RSOC_THRESHOLD_CACHE;

#ifdef BCM_SBUSDMA_SUPPORT
    if (soc_feature(unit, soc_feature_sbusdma)) {
        avs_info->avs_flags |= SOC_AVS_INFO_F_RSOC_COUNT_DMA;
        avs_info->cen_osc_reg = AVS_REG_RO_REGISTERS_0_CEN_ROSC_STATUSr;
        avs_info->rmt_osc_reg = AVS_REG_PMB_SLAVE_AVS_ROSC_COUNTr;
    }
#endif
    return SOC_E_NONE;
}

int
_soc_hr3_avs_info_deinit(int unit)
{
    
    int rv = SOC_E_NONE;
        
    return rv;
}

soc_avs_functions_t _hr3_avs_functions;

int
soc_hr3_avs_init(int unit)
{
    /* register avs functions*/
    memset(&_hr3_avs_functions, 0, sizeof(soc_avs_functions_t));
    _hr3_avs_functions.init = &_soc_hr3_avs_info_init;
    _hr3_avs_functions.deinit = &_soc_hr3_avs_info_deinit;
    _hr3_avs_functions.voltage_set = NULL;
    _hr3_avs_functions.voltage_get = NULL;

    soc_avs_function_register(unit, &_hr3_avs_functions);
    return 0;
}
#endif /* INCLUDE_AVS */
