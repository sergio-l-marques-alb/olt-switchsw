/*
 * $Id: pvtmon.c $
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
 * File:        pvtmon.c
 * Purpose:
 * Requires:
 */

#include <shared/bsl.h>
#include <soc/katana2.h>
#include <soc/saber2.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>

#ifdef BCM_SABER2_SUPPORT
static int sb2_temperature_init = 0;

static const soc_reg_t sb2_pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r
};

int
_soc_saber2_temperature_monitor_init(int unit) {
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RSTBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, BG_ADJf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RSTBf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_SELECTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RSTBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));

    sal_usleep(1000);

    /* Bring PVTMON max and min data out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MAX_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MIN_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MAX_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MIN_DATA_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));


    sb2_temperature_init = 1;
    return SOC_E_NONE;
}

int
soc_sb2_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count)
{
    uint32 rval;
    soc_reg_t reg;
    int index;
    int fval, cur, peak;
    int num_entries_out;

    *temperature_count = 0;
    if (COUNTOF(sb2_pvtmon_result_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(sb2_pvtmon_result_reg);
    }
    
    /* Check to see if the thermal monitor is initialized */
    if(sb2_temperature_init == 0) {    
        SOC_IF_ERROR_RETURN(_soc_saber2_temperature_monitor_init(unit));
    }

    for (index = 0; index < num_entries_out; index++) {
        reg = sb2_pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, TEMP_DATAf);
        cur = (41004000 - (48505 * fval)) / 10000;
        fval = soc_reg_field_get(unit, reg, rval, PEAK_TEMP_DATAf);
        peak = (41004000 - (48505 * fval)) / 10000;
        (temperature_array + index)->curr = cur;
        (temperature_array + index)->peak = peak;
    }
    
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MAX_DATA_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MIN_DATA_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MAX_DATA_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MIN_DATA_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MAX_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MIN_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MAX_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MIN_DATA_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));
    *temperature_count=num_entries_out;

    return SOC_E_NONE;
}

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r
};

int
soc_sb2_show_voltage(int unit)
{
    soc_reg_t reg;
    int index;
    uint32 rval, fval, avg;

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, MODEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RSTBf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_SELECTf, 0x3);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RSTBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));

    sal_usleep(1000);
    avg = 0;

    /* Read Voltages */
    for (index = 0; index < COUNTOF(pvtmon_result_reg); index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, TEMP_DATAf);
        /* (0.99/1024) * 8/7 * TEMP_DATA */
        fval = (fval * 8 * 990) / (1024 * 7);
        avg += fval;
        LOG_CLI((BSL_META_U(unit,
                            "Voltage @ location %d: = %d.%03dV\n"),
                 index, (fval/1000), (fval %1000)));
    }
    avg /= COUNTOF(pvtmon_result_reg);
    LOG_CLI((BSL_META_U(unit,
                        "Average:            = %d.%03dV\n"),
             (avg/1000), (avg %1000)));

    return SOC_E_NONE;
}


#endif
