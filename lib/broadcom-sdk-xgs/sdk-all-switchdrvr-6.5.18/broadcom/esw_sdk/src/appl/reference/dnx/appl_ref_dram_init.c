/** \file appl_ref_dram_init.c
 * $Id$
 *
 * L@ application procedures for DNX. 
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/switch.h>
#include <soc/i2c.h>
#include <sal/appl/i2c.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/* The I2C bus number to used based on CPU card */
#if defined(__DUNE_WRX_BCM_CPU__)
#define APPL_REF_DRAM_CPU_I2C_BUS 0
#else
#define APPL_REF_DRAM_CPU_I2C_BUS 1
#endif

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/**
* \brief
*   This function is doing dram power up.
*   The function should be called part of dram temperature procedure.
*   After wper down was made and all condition fulfil, 
*   user may power up the DRAM and after that call API bcm_switch_dram_init.
* \param [in] unit - The unit number.
*
* \return
*   Error code (as per 'shr_error_e').
* \see
*   shr_error_e
*/
shr_error_e
appl_dnx_dram_power_up_handle(
    int unit)
{
    uint8 data;
    SHR_FUNC_INIT_VARS(unit);

    /**********************************************************************************************************/
    /*
     * IMPORTANT!!! This routine belong only to DNX-SVK boards. Each board should has its own sequence of I2C 
     */
    /**********************************************************************************************************/

    /** mux open */
    data = 0x40;
    SHR_IF_ERR_EXIT(sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x70, 0x0, 0x0, &data, 0x1));

    /** HBM VPP on */
    data = 0x5;
    SHR_IF_ERR_EXIT(sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x0, 0x1, &data, 0x1));
    data = 0x80;
    SHR_IF_ERR_EXIT(sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x1, 0x1, &data, 0x1));
    /** HBM VDDC on */
    data = 0x3;
    SHR_IF_ERR_EXIT(sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x0, 0x1, &data, 0x1));
    data = 0x80;
    SHR_IF_ERR_EXIT(sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x1, 0x1, &data, 0x1));

    /** HBM VDDO on */
    data = 0x4;
    SHR_IF_ERR_EXIT(sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x0, 0x1, &data, 0x1));
    data = 0x80;
    SHR_IF_ERR_EXIT(sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x1, 0x1, &data, 0x1));

    /** mux close */
    data = 0x0;
    SHR_IF_ERR_EXIT(sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x70, 0x0, 0x0, &data, 0x1));

    sal_sleep(1);
    /** After power up, call re-init API */
    SHR_IF_ERR_EXIT(bcm_switch_dram_init(unit, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   This function is doing dram power down 
* \param [in] unit - The unit number.
*
* \return
*   Error code (as per 'shr_error_e').
* \see
*   shr_error_e
*/
void
appl_dnx_dram_power_down_handle(
    int unit,
    uint32 flags,
    void *userdata)
{
    int ret;
    uint8 data;

    /**********************************************************************************************************/
    /*
     * IMPORTANT!!! This routine belong only to DNX-SVK boards. Each board should has its own sequence of I2C 
     */
    /**********************************************************************************************************/

    /** mux open */
    data = 0x40;
    ret = sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x70, 0x0, 0x0, &data, 0x1);
    if (ret != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_BCMDNX_DRAM,
                  (BSL_META("1: Error in %s: sal_i2c_write() returned with error.\n"), FUNCTION_NAME()));
        return;
    }
    /** HBM VDDC off */
    data = 0x3;
    ret = sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x0, 0x1, &data, 0x1);
    if (ret != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_BCMDNX_DRAM,
                  (BSL_META("2: Error in %s: sal_i2c_write() returned with error.\n"), FUNCTION_NAME()));
        return;
    }
    data = 0x0;
    ret = sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x1, 0x1, &data, 0x1);
    if (ret != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_BCMDNX_DRAM,
                  (BSL_META("3: Error in %s: sal_i2c_write() returned with error.\n"), FUNCTION_NAME()));
        return;
    }
    /** HBM VDDO off */
    data = 0x4;
    ret = sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x0, 0x1, &data, 0x1);
    if (ret != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_BCMDNX_DRAM,
                  (BSL_META("4: Error in %s: sal_i2c_write() returned with error.\n"), FUNCTION_NAME()));
        return;
    }
    data = 0x0;
    ret = sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x1, 0x1, &data, 0x1);
    if (ret != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_BCMDNX_DRAM,
                  (BSL_META("5: Error in %s: sal_i2c_write() returned with error.\n"), FUNCTION_NAME()));
        return;
    }
    /** HBM VPP off */
    data = 0x5;
    ret = sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x0, 0x1, &data, 0x1);
    if (ret != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_BCMDNX_DRAM,
                  (BSL_META("6: Error in %s: sal_i2c_write() returned with error.\n"), FUNCTION_NAME()));
        return;
    }
    data = 0x0;
    ret = sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x21, 0x1, 0x1, &data, 0x1);
    if (ret != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_BCMDNX_DRAM,
                  (BSL_META("7: Error in %s: sal_i2c_write() returned with error.\n"), FUNCTION_NAME()));
        return;
    }
    /** mux close */
    data = 0x0;
    ret = sal_i2c_write(APPL_REF_DRAM_CPU_I2C_BUS, 0x70, 0x0, 0x0, &data, 0x1);
    if (ret != BCM_E_NONE)
    {
        LOG_ERROR(BSL_LS_BCMDNX_DRAM,
                  (BSL_META("8: Error in %s: sal_i2c_write() returned with error.\n"), FUNCTION_NAME()));
        return;
    }
}

/**
 * \brief
 *   This function doing dram power down callback registration, part of application init
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e
appl_dnx_dram_cb_register(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /** cb registration is relevant only for HBM architectures, if not HBM, get out */
    if (!dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcm_switch_dram_power_down_cb_register(unit, 0, appl_dnx_dram_power_down_handle, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   This function doing dram power down callback unregistration, part of application deinit
* \param [in] unit - The unit number.
*
* \return
*   Error code (as per 'shr_error_e').
* \see
*   shr_error_e
*/
shr_error_e
appl_dnx_dram_cb_unregister(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** cb registration is relevant only for HBM architectures, if not HBM, get out */
    if (!dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(bcm_switch_dram_power_down_cb_unregister(unit, appl_dnx_dram_power_down_handle, NULL));

exit:
    SHR_FUNC_EXIT;
}
