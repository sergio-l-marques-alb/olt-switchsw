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
 * Purpose : General interface for regulator functions
 */

#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/regulator.h>

#define REGULATOR_CALLBACK(_ctrl) (((regulator_ctrl_t *)(_ctrl))->callback)
#define REGULATOR_TYPE(_ctrl) (((regulator_ctrl_t *)(_ctrl))->type)
#define REGULATOR_CFG_PTR(_ctrl) (((regulator_ctrl_t *)(_ctrl))->cfg.ptr)
#define REGULATOR_CFG_SIZE(_ctrl) \
                            (((regulator_ctrl_t *)(_ctrl))->cfg.table_size)

#define TYPE_VALID_CHECK(_type) \
    if (((_type) < 0) || ((_type) >= REGULATOR_TYPE_MAX)) { \
        return SOC_E_PARAM; \
    }

#define PTR_VALID_CHECK(_ptr) \
    if ((_ptr) == NULL) { \
        return SOC_E_PARAM; \
    }

#define REGULATOR_TYPE_CHECK(_ctrl, _type) \
    if (REGULATOR_TYPE(_ctrl) != (_type)) { \
        return SOC_E_FAIL; \
    }

/*
 *
 * Function: soc_regulator_init
 *
 * Purpose:  constructor of regulator_ctrl
 *
 * Parameters:
 *    unit - StrataSwitch device number
 *    type - regulator type
 *    init_callback - callback funciton ptr provided by the caller to
 *                         fill out the supported regulator configs.
 *    regulator_handle - returns the handle of the regulator
 *
 *
 * Notes:
 *    None.
 */
int
soc_regulator_init(int unit, regulator_type_t type,
                        soc_regulator_callback callback,
                        void **regulator_handle)

{
    int rv = SOC_E_NONE;
    regulator_ctrl_t *rctrl;

    *regulator_handle = NULL;

    TYPE_VALID_CHECK(type);

    if (callback == NULL) {
        return SOC_E_PARAM;
    }

    rctrl = sal_alloc(sizeof(regulator_ctrl_t), "regulator_ctrl");
    if (rctrl == NULL) {
        return SOC_E_MEMORY;
    }

    rctrl->type = type;
    rctrl->callback = callback;

    rv = callback(unit, REGULATOR_FUNC_INIT, type, &rctrl->cfg);
    if (rv != SOC_E_NONE) {
        sal_free(rctrl);
        return rv;
    }

    *regulator_handle = rctrl;

    return rv;
}


/*
 *
 * Function: soc_regulator_deinit
 *
 * Purpose:  destructor of regulator_ctrl
 *
 * Parameters:
 *    unit - StrataSwitch device number
 *    regulator_handle - the handle of the regulator returned from
 *                               soc_regulator_init, the handle is no longer
 *                               used after calling this function
 *
 *
 * Notes:
 *    None.
 */
int
soc_regulator_deinit(int unit, void *regulator_handle)
{
    int rv = SOC_E_NONE;
    regulator_ctrl_t *rctrl = (regulator_ctrl_t *)regulator_handle;

    if (rctrl != NULL) {
        if (rctrl->callback != NULL) {
            rv = rctrl->callback(unit, REGULATOR_FUNC_DEINIT, rctrl->type,
                                                            &rctrl->cfg);
        }
        sal_free(rctrl);
    }

    return rv;
}


/*
 *
 * Function: soc_regulator_get_voltcfg_nums
 *
 * Purpose:  Get the total element numbers of the voltage config table.
 *
 * Parameters:
 *    unit - StrataSwitch device number
 *    regulator_handle - the handle of the regulator returned from
 *                               soc_regulator_init
 *    vcfgs_num - returns the voltage config table size
 *
 * Notes:
 *    None
 */
int
soc_regulator_get_cfg_nums(int unit, void *regulator_handle, int *cfgs_num)
{

    *cfgs_num = 0;

    PTR_VALID_CHECK(regulator_handle);

    *cfgs_num = REGULATOR_CFG_SIZE(regulator_handle);

    return SOC_E_NONE;
}


/*
 *
 * Function: soc_regulator_get_voltcfg_and_name_by_index
 *
 * Purpose:  Get the mapping voltage config and volt source name by voltage
 *               config table index.
 *
 * Parameters:
 *    unit - StrataSwitch device number
 *    regulator_handle - the handle of the regulator returned from
 *                               soc_regulator_init
 *    index - the index number of the voltage config table
 *    voltcfg - regturns the corresponding voltage config of
 *                 the voltage config table index
 *    name - returns the corresponding voltage source name
 *               of the voltage config table index
 *
 * Notes:
 *    None
 */
int soc_regulator_get_voltcfg_and_name_by_index(int unit,
                                                    void *regulator_handle,
                                                    int index,
                                                    void **voltcfg,
                                                    char **name)
{
    int rv = SOC_E_NONE;
    regulator_volt_t *volt;

    *voltcfg = NULL;
    *name = NULL;

    PTR_VALID_CHECK(regulator_handle);

    REGULATOR_TYPE_CHECK(regulator_handle, REGULATOR_TYPE_VOLT);

    volt = REGULATOR_CFG_PTR(regulator_handle);

    PTR_VALID_CHECK(volt);

    if (volt->get_voltcfg_and_name_by_index != NULL) {
        rv = volt->get_voltcfg_and_name_by_index(unit,
                                                volt->voltcfg_table,
                                                volt->voltcfg_table_size,
                                                index,
                                                voltcfg,
                                                name);
    } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}


/*
 *
 * Function: soc_regulator_get_voltcfg_by_name
 *
 * Purpose:  Get the mapping voltage config voltage source name.
 *
 * Parameters:
 *    unit - StrataSwitch device number
 *    regulator_handle - the handle of the regulator returned from
 *                               soc_regulator_init
 *    name - voltage source name
 *    voltcfg - regturns the corresponding voltage config of
 *                 the voltage source name
 *
 * Notes:
 *    None
 */
int
soc_regulator_get_voltcfg_by_name(int unit,
                                            void *regulator_handle,
                                            char * name,
                                            void **voltcfg)
{
    int rv = SOC_E_NONE;
    regulator_volt_t *volt;

    *voltcfg = NULL;

    PTR_VALID_CHECK(regulator_handle);

    REGULATOR_TYPE_CHECK(regulator_handle, REGULATOR_TYPE_VOLT);

    volt = REGULATOR_CFG_PTR(regulator_handle);

    PTR_VALID_CHECK(volt);

    if (volt->get_voltcfg_by_name != NULL) {
        rv = volt->get_voltcfg_by_name(unit,
                                        volt->voltcfg_table,
                                        volt->voltcfg_table_size,
                                        name,
                                        voltcfg);
    } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}


/*
 *
 * Function: soc_regulator_get_voltcfg_attribute
 *
 * Purpose:  Get the attribute of the voltage config
 *
 * Parameters:
 *    unit - StrataSwitch device number
 *    regulator_handle - the handle of the regulator returned from
 *                               soc_regulator_init
 *    voltcfg - The voltage config to be queried for the voltage attribute
 *    volt_attr - Returns the voltage attribute of the voltage config.
 *                     The result would be ORed with REGULATOR_VOLT_ATTR_RD
 *                     if the volcfg is able to get volt value.
 *                     The result would be ORed with REGULATOR_VOLT_ATTR_WR
 *                     if the voltcfg is able to set volt value.
 *
 * Notes:
 *    None
 */
int
soc_regulator_get_voltcfg_attribute(int unit,
                                            void *regulator_handle,
                                            void *voltcfg,
                                            int *volt_attr)
{
    int rv = SOC_E_NONE;
    regulator_volt_t *volt;

    *volt_attr = 0;

    PTR_VALID_CHECK(regulator_handle);

    REGULATOR_TYPE_CHECK(regulator_handle, REGULATOR_TYPE_VOLT);

    volt = REGULATOR_CFG_PTR(regulator_handle);

    PTR_VALID_CHECK(volt);

    if (volt->get_voltcfg_attribute != NULL) {
        rv = volt->get_voltcfg_attribute(unit, voltcfg, volt_attr);
    } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}


/*
 *
 * Function: soc_regulator_set_volt
 *
 * Purpose:  set voltage output value of a specific voltage config
 *
 * Parameters:
 *    unit - StrataSwitch device number
 *    regulator_handle - the handle of the regulator returned from
 *                               soc_regulator_init
 *    voltcfg - the voltage config source to be set, could be get through
 *                 function soc_regulator_get_voltconfig_by_name or
 *                 soc_regulator_get_voltcfg_and_name_by_index
 *    uV - voltage value(in milli-volts) to be set
 *
 */
int
soc_regulator_set_volt(int unit, void *regulator_handle, void *voltcfg,
                                                                    int uV)
{
    int rv = SOC_E_NONE;
    regulator_volt_t *volt;

    PTR_VALID_CHECK(regulator_handle);

    REGULATOR_TYPE_CHECK(regulator_handle, REGULATOR_TYPE_VOLT);

    volt = REGULATOR_CFG_PTR(regulator_handle);

    PTR_VALID_CHECK(volt);

    if (volt->set_voltage != NULL) {
        rv = volt->set_voltage(unit, voltcfg, uV);
    } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}


/*
 *
 * Function: soc_regulator_get_volt
 *
 * Purpose:  get voltage output value of a specific voltage config
 *
 * Parameters:
 *    unit - StrataSwitch device number
 *    regulator_handle - the handle of the regulator returned from
 *                               soc_regulator_init
 *    voltcfg - the voltage config source to be get, could be get through
 *                 function soc_regulator_get_voltconfig_by_name or
 *                 soc_regulator_get_voltcfg_and_name_by_index
 *    uV - returns the queried voltage value(in milli-volts)
 *
 */
int
soc_regulator_get_volt(int unit, void *regulator_handle, void *voltcfg,
                                                                    int *uV)
{
    int rv = SOC_E_NONE;
    regulator_volt_t *volt;

    *uV = 0;

    PTR_VALID_CHECK(regulator_handle);

    REGULATOR_TYPE_CHECK(regulator_handle, REGULATOR_TYPE_VOLT);

    volt = REGULATOR_CFG_PTR(regulator_handle);

    PTR_VALID_CHECK(volt);

    if (volt->get_voltage != NULL) {
        rv = volt->get_voltage(unit, voltcfg, uV);
    } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}


