/*
 * $Id: $
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
 * File : regulator.c
 *
 * Purpose : regulator functions implemetation for I2C interface
 *               see also : src\soc\common\regulator.c
 */

#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/bcmi2c.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <soc/regulator.h>

#define I2C_REGULATOR_VOLT_WR_DEV(_vcfg) ((_vcfg)->dac)
#define I2C_REGULATOR_VOLT_RD_DEV(_vcfg) ((_vcfg)->adc)

int soc_i2c_regulator_get_voltcfg_and_name_by_index(int unit,
                                void *voltcfg_table, int voltcfg_table_size,
                                int index, void **voltcfg, char **name)
{
    i2c_regulator_volt_config_t *vcfg =
        (i2c_regulator_volt_config_t *)voltcfg_table;

    if ((vcfg == NULL) || (index < 0) || (index >= voltcfg_table_size)) {
        *voltcfg = NULL;
        *name = NULL;
        return SOC_E_PARAM;
    }

    vcfg = vcfg + index;
    *voltcfg = vcfg;
    *name = vcfg->function;

    return SOC_E_NONE;
}


int soc_i2c_regulator_get_voltcfg_by_name(int unit,
                                void *voltcfg_table, int voltcfg_table_size,
                                char *name, void **voltcfg)
{
    i2c_regulator_volt_config_t *vcfg =
        (i2c_regulator_volt_config_t *)voltcfg_table;
    i2c_regulator_volt_config_t *cur_cfg;
    int rv = SOC_E_NONE;
    int i;

    *voltcfg = NULL;

    if ((vcfg == NULL) || (name == NULL)) {
        return SOC_E_PARAM;
    }

    for (i = 0; i < voltcfg_table_size; i++) {

        cur_cfg = vcfg + i;

        if (cur_cfg->function == NULL) {
            continue;
        }

        if (sal_strcmp(cur_cfg->function, name) == 0) {
	    *voltcfg = cur_cfg;
            break;
        }
    }

    if (*voltcfg == NULL) {
        rv = SOC_E_NOT_FOUND;
    }

    return rv;
}


int
soc_i2c_regulator_get_voltcfg_attribute(int unit, void *voltcfg,
                                                int *volt_attr)
{
    i2c_regulator_volt_config_t *vcfg = (i2c_regulator_volt_config_t *)voltcfg;

    *volt_attr = 0;

    if (vcfg == NULL) {
        return SOC_E_PARAM;
    }

    if (I2C_REGULATOR_VOLT_WR_DEV(vcfg) != NULL) {
        *volt_attr |= REGULATOR_VOLT_ATTR_WR;
    }

    if (I2C_REGULATOR_VOLT_RD_DEV(vcfg) != NULL) {
        *volt_attr |= REGULATOR_VOLT_ATTR_RD;
    }

    return SOC_E_NONE;
}


int
soc_i2c_regulator_set_volt(int unit, void *voltcfg, int uV)
{
    int rv = SOC_E_NONE;
    i2c_regulator_volt_config_t *vcfg = (i2c_regulator_volt_config_t *)voltcfg;
    int dac = -1;
    i2c_regulator_volt_dac_param_t *dac_param;

    if (vcfg == NULL) {
        return SOC_E_PARAM;
    }

    /* Check if the config is able to set volt */
    if (I2C_REGULATOR_VOLT_WR_DEV(vcfg) == NULL) {
        LOG_BSL_INFO(BSL_LS_SOC_I2C, 
                (BSL_META_U(unit, "%s is not configured to set!\n"), 
                            vcfg->function));
        return SOC_E_FAIL;
    }

    /* Check if the config has setting limitation */
    dac_param = vcfg->dac_param;
    if (dac_param != NULL) {
        if ((uV > dac_param->uV_max) || (uV < dac_param->uV_min)) {
            LOG_BSL_INFO(BSL_LS_SOC_I2C, 
                    (BSL_META_U(unit, "%s is configured with max=%duV "
                              "and min=%duV, value %duV is invalid!\n"),
                              vcfg->function, 
                              dac_param->uV_max, 
                              dac_param->uV_min,
                              uV));
            return SOC_E_PARAM;
        }
    }

    dac = bcm_i2c_open(unit, I2C_REGULATOR_VOLT_WR_DEV(vcfg), 0, 0);
    if (dac < 0) {
        LOG_BSL_INFO(BSL_LS_SOC_I2C, (BSL_META_U(unit, "Failed to open %s : %s\n"),
                                            I2C_REGULATOR_VOLT_WR_DEV(vcfg), 
                                            bcm_errmsg(dac)));
        return dac;
    }

    if ((rv = bcm_i2c_ioctl(unit, dac, I2C_REGULATOR_IOC_VOLT_SET,
                        &uV, vcfg->chan)) < 0) {
        LOG_BSL_INFO(BSL_LS_SOC_I2C, 
                    (BSL_META_U(unit, "%s Failed to set voltage : %s\n"),
                                I2C_REGULATOR_VOLT_WR_DEV(vcfg), 
                                bcm_errmsg(rv)));
    }

    return rv;
}

int
soc_i2c_regulator_get_volt(int unit, void *voltcfg, int *uV)
{
    int rv = SOC_E_NONE;
    i2c_regulator_volt_config_t *vcfg = (i2c_regulator_volt_config_t *)voltcfg;
    int adc = -1;

    if (vcfg == NULL) {
        return SOC_E_PARAM;
    }

    /* Check if the config is able to get volt */
    if (I2C_REGULATOR_VOLT_RD_DEV(vcfg) == NULL) {
        LOG_BSL_ERROR(BSL_LS_SOC_I2C, 
                    (BSL_META_U(unit, "%s is not configured to get!\n"), 
                                vcfg->function));
        return SOC_E_FAIL;
    }

    adc = bcm_i2c_open(unit, I2C_REGULATOR_VOLT_RD_DEV(vcfg), 0, 0);
    if (adc < 0) {
        LOG_BSL_INFO(BSL_LS_SOC_I2C, 
                    (BSL_META_U(unit, "Failed to open %s : %s\n"),
                                I2C_REGULATOR_VOLT_RD_DEV(vcfg), 
                                bcm_errmsg(adc)));
    }

    if ((rv = bcm_i2c_ioctl(unit, adc, I2C_REGULATOR_IOC_VOLT_GET, uV,
                                vcfg->chan)) < 0) {
        LOG_BSL_INFO(BSL_LS_SOC_I2C, 
                    (BSL_META_U(unit, "%s Failed to get voltage : %s\n"),
                                I2C_REGULATOR_VOLT_RD_DEV(vcfg), 
                                bcm_errmsg(rv)));
    }

    return rv;
}



