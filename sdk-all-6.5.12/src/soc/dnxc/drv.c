/** \file dnxc/drv.c
 * Slim SoC module to allow bcm actions.
 * 
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 */

/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_INIT

/*
 * INCLUDE FILES:
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/drv.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#endif /* BCM_DNX_SUPPORT */
#include <soc/cmicx_miim.h>
/*
 * }
 */

/**
 * \brief
 *   if proper reset action is given, resets device and makes sure device is out of reset.
 *   The possible reset_action values are:
 *   SOC_DNXC_RESET_ACTION_IN_RESET, SOC_DNXC_RESET_ACTION_INOUT_RESET.
 */
shr_error_e
soc_dnxc_cmicx_device_hard_reset(
    int unit,
    int reset_action)
{
    soc_timeout_t to;
    uint32 reg32_val;
    uint32 usleep = 1000;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_REG_IS_VALID(unit, DMU_CRU_RESETr))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Called for wrong device type\n");
    }
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "no_hard_reset", 0) != 0)
    {
        SHR_EXIT();
    }
    if ((reset_action == SOC_DNXC_RESET_ACTION_IN_RESET) || (reset_action == SOC_DNXC_RESET_ACTION_INOUT_RESET))
    {
        SHR_IF_ERR_EXIT(READ_DMU_CRU_RESETr(unit, &reg32_val)); /* read current reset register value */
        soc_reg_field_set(unit, DMU_CRU_RESETr, &reg32_val, SW_RESET_Nf, 0);    /* mark a reset */
        SHR_IF_ERR_EXIT(WRITE_DMU_CRU_RESETr(unit, reg32_val)); /* perform the reset */

#ifdef PLISIM
        if (SAL_BOOT_PLISIM)
        {
            SHR_EXIT();
        }
#endif
#ifdef FIND_DNXC_WAY_TO_MARK_RUNNING_IN_EMULATION       
        
        if (SOC_IS_DPP(unit) && SOC_DNX_CONFIG(unit)->emulation_system != 0 * /)
        {
            usleep *= 1000;
        }
#endif
        sal_usleep(usleep);     /* delay of 1ms that is much more than the required time for the reset to finish */
        /*
         * Wait for the reset to finish, although due to the delay above it should have already ended 
         */
        soc_timeout_init(&to, 100000, 100);
        for (;;)
        {
            SHR_IF_ERR_EXIT(READ_DMU_CRU_RESETr(unit, &reg32_val));     /* read current reset register value */
            if (soc_reg_field_get(unit, DMU_CRU_RESETr, reg32_val, SW_RESET_Nf) == 1)
            {
                break;
            }
            if (soc_timeout_check(&to))
            {
                SHR_EXIT_WITH_LOG(_SHR_E_INIT, "Error: DMU_CRU_RESET field not asserted correctly.\n%s%s%s", EMPTY,
                                  EMPTY, EMPTY);
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#ifdef BCM_DNX_SUPPORT

shr_error_e
soc_dnxc_mdio_config_set(
    int unit)
{
    int divisor, ext_divisor, mdio_delay;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Mdio - internal
     */
    divisor = dnx_data_access.mdio.int_divisor_get(unit);

    /*
     * Mdio - external
     */
    ext_divisor = dnx_data_access.mdio.ext_divisor_get(unit);

    /*
     * Mdio - output delay
     */
    mdio_delay = dnx_data_access.mdio.out_delay_get(unit);

    SHR_IF_ERR_EXIT(soc_cmicx_miim_init(unit));
    SHR_IF_ERR_EXIT(soc_cmicx_miim_divider_set_all(unit, divisor, ext_divisor, mdio_delay));

exit:
    SHR_FUNC_EXIT;
}
#endif /* BCM_DNX_SUPPORT */

int
soc_dnxc_avs_value_get(
    int unit,
    uint32 *avs_val)
{
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    *avs_val = 0;

    SHR_IF_ERR_EXIT(soc_reg32_get(unit, ECI_BOND_STATUSr, REG_PORT_ANY, 0, &reg_val));
    *avs_val = soc_reg_field_get(unit, ECI_BOND_STATUSr, reg_val, AVS_STATUSf);

exit:
    SHR_FUNC_EXIT;
}
