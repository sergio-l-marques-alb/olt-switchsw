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
 *
 */

#include <shared/bsl.h>

#ifdef INCLUDE_AVS
#include <soc/avs.h>
#include <soc/tomahawk.h>

#define SOC_AVS_TOMAHAWK_NUMBER_OF_CENTRALS 36
#define SOC_AVS_TOMAHAWK_CENTRAL_XBMP0 0xFF000000
#define SOC_AVS_TOMAHAWK_CENTRAL_XBMP1 0xFFFFFFFF
#define SOC_AVS_TOMAHAWK_FIRST_CENTRAL 0

#define SOC_AVS_TOMAHAWK_NUMBER_OF_REMOTES 80
#define SOC_AVS_TOMAHAWK_REMOTE_XBMP0 0x0
#define SOC_AVS_TOMAHAWK_REMOTE_XBMP1 0x0
#define SOC_AVS_TOMAHAWK_REMOTE_XBMP2 0xFFFF3000
#define SOC_AVS_TOMAHAWK_FIRST_REMOTE 0

#define SOC_AVS_TOMAHAWK_VMIN_AVS SOC_AVS_DEFAULT_VMIN_AVS
#define SOC_AVS_TOMAHAWK_VMAX_AVS SOC_AVS_DEFAULT_VMAX_AVS
#define SOC_AVS_TOMAHAWK_VMARGIN_HIGH SOC_AVS_DEFAULT_VMARGIN_HIGH
#define SOC_AVS_TOMAHAWK_VMARGIN_LOW SOC_AVS_DEFAULT_VMARGIN_LOW

static soc_avs_reg_info_t _th_osc_cen_init_list[]={
/* AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r */
{AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r,CEN_ROSC_ENABLE_DEFAULTf},
{AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1r,CEN_ROSC_ENABLE_DEFAULTf},
{INVALIDr,INVALIDf}
};

static soc_avs_reg_info_t _th_osc_cen_thr_en_list[]={
{AVS_REG_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_0r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_1r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_2_CEN_ROSC_THRESHOLD2_EN_0r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_2_CEN_ROSC_THRESHOLD2_EN_1r,IRQ_ENf},
{INVALIDr,INVALIDf}
};

/* reset osc measurement */
static soc_avs_reg_info_t _th_osc_seq_reset_list[]={
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_0r,M_INIT_CEN_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1r,M_INIT_CEN_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_0r,M_INIT_RMT_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_1r,M_INIT_RMT_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_2r,M_INIT_RMT_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_3r,M_INIT_RMT_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_4r,M_INIT_RMT_ROSCf},
{INVALIDr,INVALIDf}
};

static int _th_cent_freq_thr[] = { 
     27368.00,    /* CENT_FREQ_THRESHOLD_0 */
     23800.00,    /* CENT_FREQ_THRESHOLD_1 */
     19704.00,    /* CENT_FREQ_THRESHOLD_2 */
     11384.50,    /* CENT_FREQ_THRESHOLD_3 */
     37474.00,    /* CENT_FREQ_THRESHOLD_4 */
     32060.50,    /* CENT_FREQ_THRESHOLD_5 */
     26603.00,    /* CENT_FREQ_THRESHOLD_6 */
     15597.00,    /* CENT_FREQ_THRESHOLD_7 */
    105960.00,    /* CENT_FREQ_THRESHOLD_8 */
     89260.00,    /* CENT_FREQ_THRESHOLD_9 */
     75010.00,    /* CENT_FREQ_THRESHOLD_10*/
     42939.00,    /* CENT_FREQ_THRESHOLD_11*/
     77610.00,    /* CENT_FREQ_THRESHOLD_12*/
     63710.00,    /* CENT_FREQ_THRESHOLD_13*/
     52685.00,    /* CENT_FREQ_THRESHOLD_14*/
     29877.50,    /* CENT_FREQ_THRESHOLD_15*/
     73335.00,    /* CENT_FREQ_THRESHOLD_16*/
     58880.00,    /* CENT_FREQ_THRESHOLD_17*/
     50385.00,    /* CENT_FREQ_THRESHOLD_18*/
     29011.50,    /* CENT_FREQ_THRESHOLD_19*/
     43638.00,    /* CENT_FREQ_THRESHOLD_20*/
     35133.50,    /* CENT_FREQ_THRESHOLD_21*/
     30099.00,    /* CENT_FREQ_THRESHOLD_22*/
     17083.00,    /* CENT_FREQ_THRESHOLD_23*/
     08787.00,    /* CENT_FREQ_THRESHOLD_24*/
     07574.50,    /* CENT_FREQ_THRESHOLD_25*/
     06485.00,    /* CENT_FREQ_THRESHOLD_26*/
     05713.50,    /* CENT_FREQ_THRESHOLD_27*/
     05986.00,    /* CENT_FREQ_THRESHOLD_28*/
     07330.50,    /* CENT_FREQ_THRESHOLD_29*/
     03933.15,    /* CENT_FREQ_THRESHOLD_30*/
     04304.60,    /* CENT_FREQ_THRESHOLD_31*/
     01610.80,    /* CENT_FREQ_THRESHOLD_32*/
     01273.10,    /* CENT_FREQ_THRESHOLD_33*/
     01610.80,    /* CENT_FREQ_THRESHOLD_34*/
     01273.10,    /* CENT_FREQ_THRESHOLD_35*/
};

static int _th_rmt_freq_thr[] = { 
    23800.00,    /* RMT_FREQ_THRESHOLD_GS */
    11384.50,     /* RMT_FREQ_THRESHOLD_GH */
};

STATIC int
_soc_th_avs_ioctl(int unit, soc_avs_ioctl_t opcode, void* data, int len)
{
    uint32 value;
    int rv = SOC_E_NONE;
    switch (opcode) {
    case SOC_AVS_CTRL_PVTMON_VOLTAGE_GET:
        value = *(uint32 *)data; /* code */
        *(uint32 *)data = 10U*(value * 8800U) / 7168U;
        /* = ((code/1024)*880.0/0.7)/1000 */
        break;
    case SOC_AVS_CTRL_PVTMON_TEMP_GET:
        value = *(uint32 *)data; /* code */
        *(int32 *)data = (41004000 - (signed)(value*48705))/100;
        /* temper = 410.04 - (code * 0.48705);
         *        = (100000*410.04 - code * 48705)/100000
         *        = (41004000 - code*48705)/100000
         *        Above number represents Temperature in degrees
         *        To express temper in 1000th so multiply by 1000
         * temper = (41004000 - code*48705)/100; //in 1000 milli-Degree
         *
         *        When code = 0, temper = 410.04
         *        When code = 1023, temper = 41004000 - 49632891
         *                                 = -88.21
         */
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}

STATIC soc_error_t
_soc_th_avs_vcore_init(int unit)
{
    /* defined in avs_vctl.c */
    return soc_th_avs_vcore_init(unit);
}

STATIC soc_error_t
_soc_th_avs_vcore_deinit(int unit)
{
    /* defined in avs_vctl.c */
    return soc_th_avs_vcore_deinit(unit);
}

STATIC soc_error_t
_soc_th_avs_vcore_get(int unit, uint32 *voltage)
{
    /* defined in avs_vctl.c */
    return soc_th_avs_vcore_get(unit, voltage);
}

STATIC soc_error_t
_soc_th_avs_vcore_set(int unit, uint32 voltage)
{
    /* defined in avs_vctl.c */
    return soc_th_avs_vcore_set(unit, voltage);
}

STATIC soc_error_t
_soc_th_avs_init(int unit, soc_avs_info_t *avs_info)
{
    if (avs_info == NULL) {
        return (SOC_E_PARAM);
    }

    avs_info->num_centrals = SOC_AVS_TOMAHAWK_NUMBER_OF_CENTRALS;
    avs_info->cent_xbmp[0] = SOC_AVS_TOMAHAWK_CENTRAL_XBMP0;
    avs_info->cent_xbmp[1] = SOC_AVS_TOMAHAWK_CENTRAL_XBMP1;
    avs_info->first_cent = SOC_AVS_TOMAHAWK_FIRST_CENTRAL;

    avs_info->num_remotes = SOC_AVS_TOMAHAWK_NUMBER_OF_REMOTES;
    avs_info->rmt_xbmp[0] = SOC_AVS_TOMAHAWK_REMOTE_XBMP0;
    avs_info->rmt_xbmp[1] = SOC_AVS_TOMAHAWK_REMOTE_XBMP1;
    avs_info->rmt_xbmp[2] = SOC_AVS_TOMAHAWK_REMOTE_XBMP2;
    avs_info->first_rmt = SOC_AVS_TOMAHAWK_FIRST_REMOTE;

    avs_info->vmin_avs = SOC_AVS_INT(SOC_AVS_TOMAHAWK_VMIN_AVS);
    avs_info->vmax_avs = SOC_AVS_INT(SOC_AVS_TOMAHAWK_VMAX_AVS);
    avs_info->vmargin_low  = SOC_AVS_INT(SOC_AVS_TOMAHAWK_VMARGIN_LOW);
    avs_info->vmargin_high = SOC_AVS_INT(SOC_AVS_TOMAHAWK_VMARGIN_HIGH);
    
    avs_info->rosc_count_mode = SOC_AVS_ROSC_COUNT_MODE_1EDGE;
    avs_info->ref_clk_freq = 25;
    avs_info->measurement_time_control = 127;
    avs_info->osc_cen_init_info = _th_osc_cen_init_list;
    avs_info->osc_cen_thr_en_info = _th_osc_cen_thr_en_list;
    avs_info->osc_seq_reset_info = _th_osc_seq_reset_list;

    avs_info->cen_freq_thr = _th_cent_freq_thr;
    avs_info->rmt_freq_thr = _th_rmt_freq_thr;

    avs_info->avs_flags = SOC_AVS_INFO_F_USE_SOFTWARE_TAKEOVER |
        SOC_AVS_INFO_F_SET_THRESHOLDS |
        SOC_AVS_INFO_F_RSOC_THRESHOLD_CACHE |
        SOC_AVS_INFO_F_USE_READ_PVTMON_VOLTAGE; 

#ifdef BCM_SBUSDMA_SUPPORT
    if (soc_feature(unit, soc_feature_sbusdma)) {
        avs_info->avs_flags |= SOC_AVS_INFO_F_RSOC_COUNT_DMA;
        avs_info->cen_osc_reg = AVS_REG_RO_REGISTERS_0_CEN_ROSC_STATUSr;
        avs_info->rmt_osc_reg = AVS_REG_PMB_SLAVE_AVS_ROSC_COUNTr;
    }
#endif

    /* voltage related init (eg: calibrate) */
    SOC_IF_ERROR_RETURN(
        _soc_th_avs_vcore_init(unit));

    return SOC_E_NONE;
}

STATIC soc_error_t
_soc_th_avs_deinit(int unit)
{
    /* voltage related deinit, if any */
    SOC_IF_ERROR_RETURN(
        _soc_th_avs_vcore_deinit(unit));

    return SOC_E_NONE;
}

soc_avs_functions_t _th_avs_functions;

soc_error_t
soc_th_avs_init(int unit)
{
    /* register avs functions*/
    memset(&_th_avs_functions, 0, sizeof(soc_avs_functions_t));
    _th_avs_functions.init = &_soc_th_avs_init;
    _th_avs_functions.deinit = &_soc_th_avs_deinit;
    _th_avs_functions.voltage_set = &_soc_th_avs_vcore_set;
    _th_avs_functions.voltage_get = &_soc_th_avs_vcore_get;
    _th_avs_functions.ioctl = &_soc_th_avs_ioctl;
    soc_avs_function_register(unit, &_th_avs_functions);
    return SOC_E_NONE;
}

#endif /* INCLUDE_AVS */
