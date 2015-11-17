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
 * File : regulator.h
 *
 * Purpose : definition of regulator components
 */
#ifndef __SOC_REGULATOR_H__
#define __SOC_REGULATOR_H__


typedef enum regulator_func_e {
    REGULATOR_FUNC_INIT = 0,
    REGULATOR_FUNC_DEINIT,
    REGULATOR_FUNC_MAX /* Must be the last one */
}regulator_func_t;


typedef enum regulator_type_e {
    REGULATOR_TYPE_VOLT = 0,
    REGULATOR_TYPE_CURRENT,
    REGULATOR_TYPE_MAX /* Must be the last one */
}regulator_type_t;


typedef struct regulator_config_s {
    void *ptr;
    int table_size;
} regulator_config_t;


/*
 * soc_regulator_callback
 * Purpose:  function definition for the argument in soc_regulator_init
 *
 * Parameters:
 *    unit - StrataSwitch device number
 *    func - REGULATOR_FUNC_INIT if call back from soc_regulator_init
 *             REGULATOR_FUNC_DEINIT if call back from soc_regulator_deinit
 *    type - The configured regulator type as the argument passed to
 *             soc_regulator_init
 *    cfg - returned the regulator configuration table ptr and table size.
 *            Independent to regulator type.
 *            cfg ptr would be regulator_volt_t for REGULATOR_TYPE_VOLT.
 *            As for Other types are TBD.
*/
typedef int (*soc_regulator_callback)(int unit, regulator_func_t func,
                                        regulator_type_t type,
                                        regulator_config_t *cfg);


typedef struct regulator_ctrl_s {
    regulator_config_t cfg;
    soc_regulator_callback callback;
    regulator_type_t type;
}regulator_ctrl_t;


/* REGULATOR_VOLT attribute definition */
#define REGULATOR_VOLT_ATTR_RD 0x00000001
#define REGULATOR_VOLT_ATTR_WR 0x00000002

#define IS_REGULATOR_VOLT_ABLE_TO_RD(attr) \
            (((attr) & REGULATOR_VOLT_ATTR_RD) == REGULATOR_VOLT_ATTR_RD)
#define IS_REGULATOR_VOLT_ABLE_TO_WR(attr) \
            (((attr) & REGULATOR_VOLT_ATTR_WR) == REGULATOR_VOLT_ATTR_WR)


/*
 * typedef struct regulator_volt_s
 *
 * Purpose:  structure definition for REGULATOR_TYPE_VOLT regulator_config
 *
 * members:
 *    voltcfg_table - volt configuration table, interface independent.
 *                         Use i2c_regulator_volt_config_t for I2C interface.
 *                         Other interfaces are TBD.
 *    voltcfg_table_size - element numbers of the voltcfg_table
 *    get_voltcfg_and_name_by_index
 *          (soc_i2c_regulator_get_voltcfg_and_name_by_index)
 *    get_voltcfg_by_name (soc_i2c_regulator_get_voltcfg_by_name)
 *    get_voltcfg_attribute (soc_i2c_regulator_get_voltcfg_attribute)
 *    set_voltage (soc_i2c_regulator_set_volt)
 *    get_voltage (soc_i2c_regulator_get_volt)
 *        -The voltage config functions for the voltcfg_table.
 *          to be Interface independent.
 *          Could set to the functions in brackets for
 *          REGULATOR_TYPE_VOLT I2C interface
*/
typedef struct regulator_volt_s{
    void *voltcfg_table;
    int voltcfg_table_size;
    int (*get_voltcfg_and_name_by_index)(int unit, void *voltcfg_table,
                                        int voltcfg_table_size, int index,
                                        void **voltcfg, char **name);
    int (*get_voltcfg_by_name)(int unit, void *voltcfg_table,
                                int voltcfg_table_size, char *name,
                                void **voltcfg);
    int (*get_voltcfg_attribute)(int unit, void *voltcfg, int *attr);
    int (*set_voltage)(int unit, void *voltcfg, int uV);
    int (*get_voltage)(int unit, void *voltcfg, int *uV);
}regulator_volt_t;


/* Common interface for regulator support functions */

extern int soc_regulator_init(int unit, regulator_type_t type,
                                soc_regulator_callback callback,
                                void **regulator_handle);
extern int soc_regulator_deinit(int unit, void *regulator_handle);


extern int soc_regulator_get_cfg_nums(int unit, void *regulator_handle,
                                            int *cfgs_num);


extern int soc_regulator_get_voltcfg_and_name_by_index(int unit,
                                    void *regulator_handle, int index,
                                    void **voltcfg, char **name);
extern int soc_regulator_get_voltcfg_by_name(int unit,
                                    void *regulator_handle, char *name,
                                    void **voltcfg);
extern int soc_regulator_get_voltcfg_attribute(int unit,
                                    void *regulator_handle, void *voltcfg,
                                    int *volt_attr);
extern int soc_regulator_set_volt(int unit, void *regulator_handle,
                                    void *voltcfg, int uV);
extern int soc_regulator_get_volt(int unit, void *regulator_handle,
                                    void *voltcfg, int *uV);



/* I2C Regulator voltage Related */

typedef struct i2c_regulator_volt_dac_param_s {
    int uV_max;  /* max voltage could be set in uV*/
    int uV_min;  /* min voltage could be set in uV */
} i2c_regulator_volt_dac_param_t;

typedef struct i2c_regulator_volt_config_s {
    char *function; /* Voltage source to configure */
    uint8 chan;     /* A/D channel # */
    char *adc;      /* ADC device name, NULL measn volt get is not support */
    char *dac;      /* DAC device name, NULL means volt set is not support */
    char *mux;      /* MUX device name, NULL means no MUX control needed */
    i2c_regulator_volt_dac_param_t *dac_param; /* NULL if no volt limitation  */
} i2c_regulator_volt_config_t;

extern int soc_i2c_regulator_get_voltcfg_and_name_by_index(int unit,
                                void *voltcfg_table, int voltcfg_table_size,
                                int index, void **voltcfg, char **name);
extern int soc_i2c_regulator_get_voltcfg_by_name(int unit,
                                void *voltcfg_table, int voltcfg_table_size,
                                char *name, void **voltcfg);
extern int soc_i2c_regulator_get_voltcfg_attribute(int unit, void *voltcfg,
                                                int *volt_attr);
extern int soc_i2c_regulator_set_volt(int unit, void *voltcfg, int uV);
extern int soc_i2c_regulator_get_volt(int unit, void *voltcfg, int *uV);


#endif /* __SOC_REGULATOR_H__ */

