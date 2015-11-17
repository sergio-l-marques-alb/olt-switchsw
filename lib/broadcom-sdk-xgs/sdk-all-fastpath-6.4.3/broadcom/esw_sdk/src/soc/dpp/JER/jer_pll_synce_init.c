/*
 * $Id: jer_pll_synce_init.c, v1 17/09/2014 09:55:39 azarrin $
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
 */

/*************
 * INCLUDES  *
 *************/


/* SOC includes */
#include <soc/debug.h>
#include <soc/error.h>

/* SOC DPP includes */
#include <soc/dpp/drv.h>

#include <soc/dpp/JER/jer_ports.h>

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/*************
 * TYPE DEFS *
 *************/
/* This enum used as an index to pll register array. changing it require changing the register array */
typedef enum
{
    /*
    * fabric
    */
    JER_PLL_TYPE_FABRIC_0 = 0,
    JER_PLL_TYPE_FABRIC_1 = 1,
    /*
    * nif
    */
    JER_PLL_TYPE_NIF_PMH = 2,
    JER_PLL_TYPE_NIF_PML_0 = 3,
    JER_PLL_TYPE_NIF_PML_1 = 4,
    /*
    * synce
    */
    JER_PLL_TYPE_SYNCE_0 = 5,
    JER_PLL_TYPE_SYNCE_1 = 6,
    /*
    * 1588
    */
    JER_PLL_TYPE_TS = 7,
    JER_PLL_TYPE_BS = 8,
    /*
    * number of jericho pll types  
    */
    JER_NOF_PLL_TYPES = 9
}JER_PLL_TYPE;

#if 0 /* mcm release needed */
/*************
 * FUNCTIONS *
 *************/
/* 
 * PLL_3 configurations used by both pll and sync init 
 */ 
STATIC int jer_pll_3_set(
                    int                         unit,
                    JER_PLL_TYPE                pll_type,
                    uint32                      ndiv,
                    uint32                      mdiv,
                    uint32                      pdiv,
                    uint32                      is_bypass)
{
    uint32 val;
    soc_reg_above_64_val_t reg_above_64, field_above_64;

    const static soc_reg_t
        eci_srd_pll_config[] = {ECI_MISC_PLL_2_CONFIGr, ECI_MISC_PLL_3_CONFIGr, ECI_MISC_PLL_4_CONFIGr, ECI_MISC_PLL_5_CONFIGr, ECI_MISC_PLL_6_CONFIGr, ECI_MISC_PLL_7_CONFIGr, ECI_MISC_PLL_8_CONFIGr};
    const static soc_reg_t
        eci_srd_pll_status[] = {ECI_MISC_PLL_2_STATUSr, ECI_MISC_PLL_3_STATUSr, ECI_MISC_PLL_4_STATUSr, ECI_MISC_PLL_5_STATUSr, ECI_MISC_PLL_6_STATUSr, ECI_MISC_PLL_7_STATUSr, ECI_MISC_PLL_8_STATUSr};
    const static soc_field_t
        eci_srd_pll_locked[] = {MISC_PLL_2_LOCKEDf, MISC_PLL_3_LOCKEDf, MISC_PLL_4_LOCKEDf, MISC_PLL_5_LOCKEDf, MISC_PLL_6_LOCKEDf, MISC_PLL_7_LOCKEDf, MISC_PLL_8_LOCKEDf};

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_CLEAR(field_above_64);

    /* ndiv */
    SHR_BITCOPY_RANGE(field_above_64,0,&ndiv,0,sizeof(ndiv)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_NDIVf, field_above_64);

    /* mdiv ch 0 */
    SHR_BITCOPY_RANGE(field_above_64,0,&mdiv,0,sizeof(mdiv)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_CH_0_MDIVf, field_above_64);

    /* kp */
    val = 6;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_KPf, field_above_64);

    /* ki */
    val = 4;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_KIf, field_above_64);

    /* kpp */
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_KPPf, field_above_64);

    /* pdiv */
    SHR_BITCOPY_RANGE(field_above_64,0,&pdiv,0,sizeof(pdiv)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PDIVf, field_above_64);

    /* control stat_reset */
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_STAT_RESETf, field_above_64);

    /* control lc_boost */
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_LC_BOOSTf, field_above_64);

    /* control cmlbuf0_pwron */
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_CMLBUF_0_PWRONf, field_above_64);

    /* control cmlbuf1_pwron */
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_CMLBUF_1_PWRONf, field_above_64);

    /* control pwm_rate */
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_PWM_RATEf, field_above_64);

    /* control post_rst_sel */
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_POST_RST_SELf, field_above_64);

    /* pwron */
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PWRONf, field_above_64);

    /* pwron ldo*/
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PWRON_LDOf, field_above_64);

    /* set config register with init values */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

    sal_usleep(30);

    /* asserting the reset bit */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_RESET_Bf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

    /* polling on pll_locked */
    SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, eci_srd_pll_status[pll_type], REG_PORT_ANY, 0, eci_srd_pll_locked[pll_type], 1));

    /* post reset */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_POST_RESET_Bf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

    if(is_bypass){
        
    }

exit:    
    SOCDNX_FUNC_RETURN;
}

/* 
 * PLL_3 configurations used by both pll and sync init 
 */ 
STATIC int jer_pll_1_set(
                    int                         unit,
                    JER_PLL_TYPE                pll_type)
{
    uint32 val;
    soc_reg_above_64_val_t reg_above_64, field_above_64;

    const static soc_reg_t
        eci_srd_pll_config[] = {ECI_MISC_PLL_0_CONFIGr, ECI_MISC_PLL_1_CONFIGr};
    const static soc_reg_t
        eci_srd_pll_status[] = {ECI_MISC_PLL_0_STATUSr, ECI_MISC_PLL_1_STATUSr};
    const static soc_field_t
        eci_srd_pll_locked[] = {MISC_PLL_0_LOCKEDf, MISC_PLL_1_LOCKEDf};

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_CLEAR(field_above_64);

    /* ndiv */
    val = 140;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_NDIVf, field_above_64);

    /* mdiv ch 0 */
    if (pll_type == JER_PLL_TYPE_TS) {
        val = 7;
    } else {
        val = 140;
    }
    SHR_BITCOPY_RANGE(field_above_64, 0, &val, 0, sizeof(val)*8); 
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_CH_0_MDIVf, field_above_64);

    /* kp */
    val = 3;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_KPf, field_above_64);

    /* ki */
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_KIf, field_above_64);

    /* ka */
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_KAf, field_above_64);

    /* pdiv */
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_PDIVf, field_above_64);

    /* control pwm_rate */
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_PWM_RATEf, field_above_64);

    /* control post_rst_sel */
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_POST_RESET_SELECTf, field_above_64);

    /* control vco_fb_div2 */
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_VCO_FB_DIV_2f, field_above_64);

    /* control vco_range */
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_VCO_RANGEf, field_above_64);

    /* control ldo */
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_LDOf, field_above_64);

    /* pwron ldo*/
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PWRON_LDOf, field_above_64);

    /* set config register with init values wo pwron */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

    sal_usleep(1);

    /* pwron */
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PWRONf, field_above_64);

    /* set config register with init values with pwron */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

    sal_usleep(30);

    /* asserting the reset bit */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_RESET_Bf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

    /* polling on pll_locked */
    SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, eci_srd_pll_status[pll_type], REG_PORT_ANY, 0, eci_srd_pll_locked[pll_type], 1));

    /* post reset */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_MISC_PLL_2_CONFIGr, reg_above_64, FAB_0_PLL_CFG_POST_RESET_Bf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

exit:    
    SOCDNX_FUNC_RETURN;
}

/* 
 * PLL3 Configurations 
 */ 
STATIC int jer_pll_init(
                    int                         unit,
                    JER_PLL_TYPE                pll_type,
                    soc_dcmn_init_serdes_ref_clock_t  ref_clock_in,
                    soc_dcmn_init_serdes_ref_clock_t  ref_clock_out)
{
    uint32 ndiv, mdiv, is_bypass;
    uint32 pdiv = 1;

    SOCDNX_INIT_FUNC_DEFS;

    is_bypass = SOC_DPP_CONFIG(unit)->arad->init.fabric_synce.enable;

    if (ref_clock_in == soc_dcmn_init_serdes_ref_clock_125) {
        mdiv = 25;
    } else if (ref_clock_in == soc_dcmn_init_serdes_ref_clock_156_25) {
        mdiv = 20;
    } else {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (ref_clock_out == soc_dcmn_init_serdes_ref_clock_125) {
        ndiv = 25;
    } else if (ref_clock_out == soc_dcmn_init_serdes_ref_clock_156_25) {
        ndiv = 20;
    } else {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    SOCDNX_IF_ERR_EXIT(jer_pll_3_set(unit, pll_type, ndiv, mdiv, pdiv, is_bypass)); 

exit:    
    SOCDNX_FUNC_RETURN;
}

/* 
 * syncE Configurations 
 */ 
STATIC int jer_synce_init(int unit)
{
    uint32 ndiv, mdiv, pdiv, ref_clk_in;
    soc_dcmn_init_serdes_ref_clock_t  ref_clock_out;
    soc_port_t port;
    uint32 reg_val;
    int rv, synce_index;
    uint32 devide_value;
    ARAD_INIT_SYNCE        *info;

    SOCDNX_INIT_FUNC_DEFS;

    info = &(SOC_DPP_CONFIG(unit)->arad->init.synce);

    for (synce_index = 0; synce_index <= JER_PLL_TYPE_SYNCE_1 - JER_PLL_TYPE_SYNCE_0; synce_index++) {

        if (info->conf[synce_index].enable == 0) {
            continue;
        }

        ref_clock_out = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_synce_out[synce_index];
        port = info->conf[synce_index].port_id;
        SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_get(unit, port, SOC_PHY_CONTROL_REF_CLK, &ref_clk_in));

        /* Falcon 1.25 GHZ */
        devide_value = 1;
        pdiv = 1;
        ndiv = 50;
        if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
            mdiv = 25;
        } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
            mdiv = 20;
        } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
            mdiv = 125;
        } else {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }

        /* Falcon 10.3125 GHZ */
        devide_value = 11;
        pdiv = 3;
        ndiv = 200;
        if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
            mdiv = 25;
        } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
            mdiv = 20;
        } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
            mdiv = 125;
        } else {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }

        /* Eagle 10.3125  GHZ */
        pdiv = 1;
        if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
            devide_value = 11;
            mdiv = 24;
            ndiv = 128;
        } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
            devide_value = 9;
            mdiv = 22;
            ndiv = 120;
        } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
            devide_value = 11;
            mdiv = 120;
            ndiv = 128;
        } else {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }

        /* Eagle 25.78125 GHZ */
        devide_value = 11;
        pdiv = 3;
        ndiv = 160;
        if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
            mdiv = 25;
        } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
            mdiv = 20;
        } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
            mdiv = 125;
        } else {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }

        /* pll configuration */
        SOCDNX_IF_ERR_EXIT(jer_pll_3_set(unit, synce_index + JER_PLL_TYPE_SYNCE_0, ndiv, mdiv, pdiv));

        /* Configure nif/fabric mux according port and decide whether to configure the NIF */

        /* NBI configuration */
        SOCDNX_IF_ERR_EXIT(READ_NBIH_SYNC_ETH_CFGr(unit, synce_index, &reg_val));

        /* Configure first port serdes */
        rv = handle_sand_result(soc_jer_port_synce_clk_sel_set(unit, synce_index, info->conf[synce_index].port_id));
        SOCDNX_IF_ERR_EXIT(rv);

        /* Configure Clock divider */
        soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_CLOCK_DIV_Nf, 2);
        soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_DIVIDER_PHASE_ZERO_Nf, ((devide_value+1)/2) - 1);
        soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_DIVIDER_PHASE_ONE_Nf, (devide_value/2) - 1);

        /* Configure Squelch mode */
        soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_SQUELCH_EN_Nf,  info->conf[synce_index].squelch_enable);

        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_SYNC_ETH_CFGr(unit, synce_index, reg_val));
    }

    /* Configure Pads */
    reg_val = 0x0;
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_0_OE_Nf, 0x0 /* Output */);
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_1_OE_Nf, 0x0 /* Output */);
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_2_OE_Nf, 0x0 /* Output */);
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_3_OE_Nf, 0x0 /* Output */);

    if (info->mode == ARAD_NIF_SYNCE_MODE_TWO_DIFF_CLK) {
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_0_SELECTf, 0x0 /* clk 0 (P)*/);
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_1_SELECTf, 0x0 /* clk 0 (N) */);  
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_2_SELECTf, 0x1 /* clk 1 (P) */);
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_3_SELECTf, 0x1 /* clk 1 (N) */);
    } else if (info->mode == ARAD_NIF_SYNCE_MODE_TWO_CLK_AND_VALID) {
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_0_SELECTf, 0x0 /* clk 0 (P)*/);
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_1_SELECTf, 0x4 /* valid 0 */);  
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_2_SELECTf, 0x1 /* clk 1 (P) */);
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_3_SELECTf, 0x5 /* valid 1 */);
    }  
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, reg_val));

exit:    
    SOCDNX_FUNC_RETURN;
}
#endif

/* 
 * Init Jericho plls including Synce
 */ 
int jer_pll_synce_init(int unit)
{
#if 0 /* mcm release needed */
    soc_jer_pll_config_t        pll;
#endif
    SOCDNX_INIT_FUNC_DEFS;

#if 0 /* mcm release needed */
    /* ppl 3 */
    pll = SOC_DPP_CONFIG(unit)->jer->pll;
    SOCDNX_IF_ERR_EXIT(jer_pll_init(unit, JER_PLL_TYPE_NIF_PML_0, pll.ref_clk_pml_in[0], pll.ref_clk_pml_out[0])); 
    SOCDNX_IF_ERR_EXIT(jer_pll_init(unit, JER_PLL_TYPE_NIF_PML_1, pll.ref_clk_pml_in[1], pll.ref_clk_pml_out[1])); 
    SOCDNX_IF_ERR_EXIT(jer_pll_init(unit, JER_PLL_TYPE_NIF_PMH, pll.ref_clk_pmh_in, pll.ref_clk_pmh_out)); 
    SOCDNX_IF_ERR_EXIT(jer_pll_init(unit, JER_PLL_TYPE_FABRIC_0, pll.ref_clk_fabric_in[0], pll.ref_clk_fabric_out[0]));
    SOCDNX_IF_ERR_EXIT(jer_pll_init(unit, JER_PLL_TYPE_FABRIC_1, pll.ref_clk_fabric_in[1], pll.ref_clk_fabric_out[1]));

    /* synce ppl 3 */
    SOCDNX_IF_ERR_EXIT(jer_synce_init(unit));

    /* ts */
    if (SOC_DPP_CONFIG(unit)->arad->init.pll.ts_clk_mode == 0x1) {
        SOCDNX_IF_ERR_EXIT(jer_pll_1_set(unit, JER_PLL_TYPE_TS));
    }

    /* bs */
    if (SOC_DPP_CONFIG(unit)->arad->init.pll.bs_clk_mode == 0x1) {
        SOCDNX_IF_ERR_EXIT(jer_pll_1_set(unit, JER_PLL_TYPE_BS));
    }

exit:
#endif
    SOCDNX_FUNC_RETURN;
}      


