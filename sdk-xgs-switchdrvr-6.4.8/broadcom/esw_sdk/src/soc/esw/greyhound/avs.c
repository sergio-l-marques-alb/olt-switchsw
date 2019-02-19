/*
 * $Id: avs.c $
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
 * File:        avs.c
 * Purpose:
 * Requires:
 */
 
#include <shared/bsl.h>

#ifdef INCLUDE_AVS
#include <soc/avs.h>
#include <soc/regulator.h>
#include <soc/i2c.h>

#define SOC_AVS_GREYHOUND_NUMBER_OF_CENTRALS 36
#define SOC_AVS_GREYHOUND_CENTRAL_XBMP0 0x0
#define SOC_AVS_GREYHOUND_CENTRAL_XBMP1 0xFFFFFFF0
#define SOC_AVS_GREYHOUND_FIRST_CENTRAL 0

#define SOC_AVS_GREYHOUND_NUMBER_OF_REMOTES 12
#define SOC_AVS_GREYHOUND_REMOTE_XBMP0 0x0
#define SOC_AVS_GREYHOUND_FIRST_REMOTE 0

#define SOC_AVS_GREYHOUND_VMIN_AVS SOC_AVS_DEFAULT_VMIN_AVS
#define SOC_AVS_GREYHOUND_VMAX_AVS SOC_AVS_DEFAULT_VMAX_AVS
#define SOC_AVS_GREYHOUND_VMARGIN_HIGH SOC_AVS_DEFAULT_VMARGIN_HIGH
#define SOC_AVS_GREYHOUND_VMARGIN_LOW SOC_AVS_DEFAULT_VMARGIN_LOW



static soc_avs_reg_info_t _gh_osc_cen_init_list[]={
/* AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r */
{AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r,CEN_ROSC_ENABLE_DEFAULTf},
{AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1r,CEN_ROSC_ENABLE_DEFAULTf},
{-1,-1},
};

static soc_avs_reg_info_t _gh_osc_cen_thr_en_list[]={
{AVS_REG_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_0r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_1r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_2_CEN_ROSC_THRESHOLD2_EN_0r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_2_CEN_ROSC_THRESHOLD2_EN_1r,IRQ_ENf},
{-1,-1},
};

/* reset osc measurement */
static soc_avs_reg_info_t _gh_osc_seq_reset_list[]={
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_0r,M_INIT_CEN_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1r,M_INIT_CEN_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_0r,M_INIT_RMT_ROSCf},
{-1,-1}
};

static int _gh_cent_freq_thr[] = {
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

static int _gh_rmt_freq_thr[] = {
    23800.00,    /* RMT_FREQ_THRESHOLD_GS */
    11384.50,     /* RMT_FREQ_THRESHOLD_GH */
};



typedef struct gh_avs_info_s {
    void *regulator_p;
    void *regulator_vcfg;
} gh_avs_info_t;

static gh_avs_info_t GH_AVS_INFO[SOC_MAX_NUM_DEVICES];


static i2c_regulator_volt_dac_param_t gh_core_dac_param = {
    .uV_max = 1050000,
    .uV_min = 850000,
};
static i2c_regulator_volt_config_t gh_avs_ltc3880[] = {
    {"Core", 0, I2C_LTC3880_62, I2C_LTC3880_62, NULL, &gh_core_dac_param}
};

/* The unit in Algorithm is 0.1mV while the unit in Regulator is uV*/
#define GH_REGULATOR_SCALE  (100)

int
_soc_greyhound_avs_voltage_set(int unit, uint32 val)
{
    void *hndl,*vcfg;
    int uV;

    hndl = GH_AVS_INFO[unit].regulator_p;
    vcfg = GH_AVS_INFO[unit].regulator_vcfg;  

    if ((hndl== NULL) || (vcfg == NULL)) {
        return SOC_E_INIT;
    }
    uV = val * (GH_REGULATOR_SCALE / SOC_AVS_S1); 

    return soc_regulator_set_volt(unit, hndl, vcfg, uV);
}

int
_soc_greyhound_avs_voltage_get(int unit, uint32 *val)
{
    void *hndl,*vcfg;
    int uV;
    
    hndl = GH_AVS_INFO[unit].regulator_p;
    vcfg = GH_AVS_INFO[unit].regulator_vcfg;

    if ((hndl == NULL) || (vcfg == NULL)) {
        return SOC_E_INIT;
    }
    soc_regulator_get_volt(unit, hndl, vcfg, &uV);

    *val = uV /(GH_REGULATOR_SCALE/SOC_AVS_S1);
    return 0;
}

STATIC int 
_soc_greyhound_regulator_volt_init(int unit, regulator_config_t *rcfg)
{
    regulator_volt_t *volt;
    int rv = SOC_E_NONE;

    volt = sal_alloc(sizeof(regulator_volt_t), "gh_regulator_volt");
    sal_memset(volt, 0, sizeof(regulator_volt_t));
    if (volt != NULL) {
        volt->voltcfg_table = gh_avs_ltc3880;
        volt->voltcfg_table_size = 1;
        volt->get_voltcfg_and_name_by_index = 
                            soc_i2c_regulator_get_voltcfg_and_name_by_index;
        volt->get_voltcfg_by_name = soc_i2c_regulator_get_voltcfg_by_name;
        volt->get_voltcfg_attribute = soc_i2c_regulator_get_voltcfg_attribute;
        volt->set_voltage = soc_i2c_regulator_set_volt;
        volt->get_voltage = soc_i2c_regulator_get_volt;

        rcfg->ptr = volt;
        rcfg->table_size = 1;
    } else {
        rv = SOC_E_MEMORY;
    }
    return rv;
}

STATIC int 
_soc_greyhound_regulator_volt_deinit(int unit, regulator_config_t *rcfg)
{
    if ((rcfg != NULL) && (rcfg->ptr != NULL)) {
        sal_free(rcfg->ptr);
    }

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_regulator_callback(int unit, regulator_func_t func,
        regulator_type_t type, regulator_config_t *rcfg)
{
    int rv = SOC_E_PARAM;

    if (type == REGULATOR_TYPE_VOLT) {
        if (func == REGULATOR_FUNC_INIT) {
            rv = _soc_greyhound_regulator_volt_init(unit, rcfg);
        } else if (func == REGULATOR_FUNC_DEINIT) {
            rv = _soc_greyhound_regulator_volt_deinit(unit, rcfg);
        }
    }
    return rv;
}

int
_soc_greyhound_avs_info_init(int unit, soc_avs_info_t *avs_info)
{    
    void *hndl;
    if (avs_info == NULL) {
        return (SOC_E_PARAM);
    }
    /* LTC 3880 device */
    sal_memset(&GH_AVS_INFO[unit], 0, sizeof(gh_avs_info_t));
    SOC_IF_ERROR_RETURN(
        soc_regulator_init(unit, REGULATOR_TYPE_VOLT, 
                           _soc_greyhound_regulator_callback, 
                           &(GH_AVS_INFO[unit].regulator_p)));    
    hndl = GH_AVS_INFO[unit].regulator_p;
    SOC_IF_ERROR_RETURN(
        soc_regulator_get_voltcfg_by_name(unit, 
            hndl, "Core", 
            &GH_AVS_INFO[unit].regulator_vcfg));
       
    avs_info->num_centrals = SOC_AVS_GREYHOUND_NUMBER_OF_CENTRALS;
    avs_info->cent_xbmp[0] = SOC_AVS_GREYHOUND_CENTRAL_XBMP0;
    avs_info->cent_xbmp[1] = SOC_AVS_GREYHOUND_CENTRAL_XBMP1;
    avs_info->first_cent = SOC_AVS_GREYHOUND_FIRST_CENTRAL;

    avs_info->num_remotes = SOC_AVS_GREYHOUND_NUMBER_OF_REMOTES;
    avs_info->rmt_xbmp[0] = SOC_AVS_GREYHOUND_REMOTE_XBMP0;
    avs_info->first_rmt = SOC_AVS_GREYHOUND_FIRST_REMOTE;

    avs_info->vmin_avs = SOC_AVS_INT(SOC_AVS_GREYHOUND_VMIN_AVS);
    avs_info->vmax_avs = SOC_AVS_INT(SOC_AVS_GREYHOUND_VMAX_AVS);
    avs_info->vmargin_low  = SOC_AVS_INT(SOC_AVS_GREYHOUND_VMARGIN_LOW);
    avs_info->vmargin_high = SOC_AVS_INT(SOC_AVS_GREYHOUND_VMARGIN_HIGH);
    
    avs_info->rosc_count_mode = SOC_AVS_ROSC_COUNT_MODE_1EDGE;
    avs_info->ref_clk_freq = 25;
    avs_info->measurement_time_control = 127;
    avs_info->osc_cen_init_info = _gh_osc_cen_init_list;
    avs_info->osc_cen_thr_en_info = _gh_osc_cen_thr_en_list;
    avs_info->osc_seq_reset_info = _gh_osc_seq_reset_list;

    avs_info->cen_freq_thr = _gh_cent_freq_thr;
    avs_info->rmt_freq_thr = _gh_rmt_freq_thr;

    avs_info->avs_flags = SOC_AVS_INFO_F_USE_SOFTWARE_TAKEOVER |
        SOC_AVS_INFO_F_SET_THRESHOLDS |
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
_soc_greyhound_avs_info_deinit(int unit)
{
    void *hndl;
    int rv = SOC_E_NONE;
    
    hndl = GH_AVS_INFO[unit].regulator_p;    
    sal_memset(&GH_AVS_INFO[unit], 0, sizeof(gh_avs_info_t));
    rv = soc_regulator_deinit(unit, hndl);

    return rv;
}

soc_avs_functions_t _gh_avs_functions;

int
soc_greyhound_avs_init(int unit)
{
    /* register avs functions*/
    memset(&_gh_avs_functions, 0, sizeof(soc_avs_functions_t));
    _gh_avs_functions.init = &_soc_greyhound_avs_info_init;
    _gh_avs_functions.deinit = &_soc_greyhound_avs_info_deinit;
    _gh_avs_functions.voltage_set = &_soc_greyhound_avs_voltage_set;
    _gh_avs_functions.voltage_get = &_soc_greyhound_avs_voltage_get;

    soc_avs_function_register(unit, &_gh_avs_functions);
    return 0;
}
#endif /* INCLUDE_AVS */
