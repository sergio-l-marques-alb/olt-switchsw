/** \file diag_dnx_reset.c
 *
 * Diagnostics device reset.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*************
 * INCLUDES  *
 *************/
/*shared*/
#include <shared/bsl.h>
/*appl*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/cmdlist.h>

/*soc*/
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/dnx/drv.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * FUNCTIONS *
 *************/

shr_error_e
sh_dnx_reset_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int soft_reset, access_only, soft_reset_no_fabric;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("soft", soft_reset);
    SH_SAND_GET_BOOL("soft_NoFabric", soft_reset_no_fabric);
    SH_SAND_GET_BOOL("ACCessOnly", access_only);

    if (soft_reset)
    {
        SHR_IF_ERR_EXIT(soc_dnx_device_reset(unit, SOC_DNX_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET, 0));
    }
    else if (soft_reset_no_fabric)
    {
        SHR_IF_ERR_EXIT(soc_dnx_device_reset(unit, SOC_DNX_RESET_MODE_BLOCKS_SOFT_RESET, 0));
    }
    else if (access_only)
    {
        SHR_IF_ERR_EXIT(soc_dnx_device_reset(unit, SOC_DNX_RESET_MODE_REG_ACCESS, 0));
    }
    else if (ARG_CUR(args))
    {
        int rv;

        /** invoke legacy command */
        rv = sh_device_reset(unit, args);
        if (rv != CMD_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Legacy DeviceRest command failed");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * List of the supported commands, pointer to command function and command usage function.
 */

sh_sand_man_t sh_dnx_reset_man = {
    .brief = "Reset device",
    .full = "Reset device according to provided mode (soft, Accessonly, etc..).\n"
        "This command also supports legacy der command calls",
    .examples = "Soft\n" "AccessOnly\n" "0x4 0x40",
};

sh_sand_option_t sh_dnx_reset_options[] = {
    {"SoFT", SAL_FIELD_TYPE_BOOL, "Perform Soft reset", "No", NULL}
    ,
    {"SoFT_NoFabric", SAL_FIELD_TYPE_BOOL, "Perform Soft reset without Fabric", "No", NULL}
    ,
    {"ACCessOnly", SAL_FIELD_TYPE_BOOL, "Init in Access-only mode", "No", NULL}
    ,
    {"mode", SAL_FIELD_TYPE_INT32, "Legacy command mode", "0", NULL, NULL, SH_SAND_ARG_FREE}
    ,
    {"action", SAL_FIELD_TYPE_INT32, "Legacy command action", "0", NULL, NULL, SH_SAND_ARG_FREE}
    ,
    {NULL}
};
