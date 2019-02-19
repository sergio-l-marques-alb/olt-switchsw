/** \file appl_ref_init_deinit.c
 * init and deinit functions to be used by the INIT_DNX command.
 */

/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

/*
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <appl/reference/dnxc/appl_ref_init_deinit.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>
#include <bcm/init.h>
#include <bcm/stack.h>
#include <soc/property.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include "appl_ref_init_deinit.h"
#include <appl/reference/dnxc/appl_ref_intr.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define APPL_DNXF_DEFAULT_MODID (5)

/*
 * }
 */

/*
 * MACROs:
 * {
 */

#define APPL_DNXF_DEVICE_PARAMS(unit)	(appl_dnxf_device_params[unit])

/*
 * }
 */

/*
 * Typedefs.
 * {  
 */

/**
 * \brief dnxf device parameters
 */
typedef struct appl_dnxf_device_params_s
{
    int modid;
} appl_dnxf_device_params_t;

/*
 * }
 */

/*
 * Globals.
 * {  
 */

appl_dnxf_device_params_t appl_dnxf_device_params[SOC_MAX_NUM_DEVICES];

/**
 * \brief - dnxf command line usage
 */
char appl_dnxf_init_usage[] = APPL_DNXC_INIT_USAGE "For additional info go to sequence struct appl_dnxf_init_steps.\n";

/*
 * }
 */

/*
 *  \brief Parse Device Application Parameters
 */
static shr_error_e
appl_dnxf_device_params_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    APPL_DNXF_DEVICE_PARAMS(unit).modid =
        soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "module_id", APPL_DNXF_DEFAULT_MODID);
    LOG_INFO_EX(BSL_LOG_MODULE, "   module id = %d.\n%s%s%s", APPL_DNXF_DEVICE_PARAMS(unit).modid, EMPTY, EMPTY, EMPTY);

    return SHR_ERR_TO_SHELL;
}

/*
 *  \brief DNXF STK init.
 *          * Set Device Modid
 *          * Enable Traffic (Unisolate Device)
 */
static shr_error_e
appl_dnxf_ref_stk_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_stk_modid_set(unit, APPL_DNXF_DEVICE_PARAMS(unit).modid));
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, APPL_DNXF_DEVICE_PARAMS(unit).modid, -1, 0x1));

exit:
    return SHR_ERR_TO_SHELL;
}

/*
 *  \brief DNXF STK deinit.
 *          * Disable Traffic (Isolate Device)
 */
static shr_error_e
appl_dnxf_ref_stk_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, APPL_DNXF_DEVICE_PARAMS(unit).modid, -1, 0x0));

exit:
    return SHR_ERR_TO_SHELL;
}

/* *INDENT-OFF* */

/**
 * \brief DNX reference applications sequence 
 *        * Each refernce application in the init sequence should be added here.
 *        * Each application can be enabled or disabled using a dedicated
 *          soc property which uses the "suffix" as an argument: 'appl_enable_<suffix>=0/1'
 *        * The last step must be all NULLs
 */
static const appl_dnxc_init_step_t appl_dnxf_ref_apps_steps[] = {
   /*STEP_ID                         NAME              SUFFIX         INIT_FUNC                        DEINIT_FUNC                   STEP_FLAGS                          TIME_THRESH   SUB_LIST*/
    {APPL_DNXF_STEP_DEVICE_PARAMS,   "Device Params",  "params",      appl_dnxf_device_params_init,    NULL,                         UTILEX_SEQ_STEP_F_STANDALONE_EN,    0,            NULL},
    {APPL_DNXF_STEP_STK_INIT,        "STK Init",       "stk_init",    appl_dnxf_ref_stk_init,          appl_dnxf_ref_stk_deinit,     APPL_DNXC_INIT_DEF_FLAG_PROP_EN,    0,            NULL},
    {APPL_DNXF_STEP_INTR_INIT,       "Interrupt Init", "intr_init",   appl_dnxc_interrupt_init,        appl_dnxc_interrupt_deinit,   APPL_DNXC_INIT_DEF_FLAG_PROP_EN,    0,            NULL},
    {APPL_DNXF_STEP_LAST_STEP,        NULL,             NULL,         NULL,                            NULL,                         0,                                  0,            NULL} /* Last step must be all NULLs*/
};


/**
 * \brief DNX init sequence 
 *        * Each step in the init sequence should be added here
 *        * The init sequence has 2 main phases:
 *          1. Initialize SDK (create unit and init bcm)
 *          2. Run default reference applications - each application can be enabled or disabled using a dedicated
 *             soc property which uses the "suffix" as an argument: 'appl_enable_<suffix>=0/1'
 *        * The last step must be all NULLs
 */
static const appl_dnxc_init_step_t appl_dnxf_init_steps[] = {
    /*STEP_ID                   NAME        SUFFIX      INIT_FUNC             DEINIT_FUNC             STEP_FLAGS                        TIME_THRESH   SUB_LIST*/
    {APPL_DNXF_STEP_SDK,         "SDK",      "",         appl_dnxc_sdk_init,   appl_dnxc_sdk_deinit,   UTILEX_SEQ_STEP_F_STANDALONE_EN, 0,            NULL},
    {APPL_DNXF_STEP_APPS,        "Appls",    "",         NULL,                 NULL,                   APPL_DNXC_INIT_DEF_FLAG_PROP_EN, 0,            appl_dnxf_ref_apps_steps},
    {APPL_DNXF_STEP_LAST_STEP,   NULL,       NULL,       NULL,                 NULL,                   0,                               0,            NULL} /* Last step must be all NULLs*/
};

/* *INDENT-ON* */

/*
 * See .h file
 */
shr_error_e
appl_dnxf_step_list_get(
    int unit,
    const appl_dnxc_init_step_t ** appl_steps)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * return pointer to the list of DNX application init steps 
     */
    *appl_steps = appl_dnxf_init_steps;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
cmd_result_t
cmd_dnxf_init_dnx(
    int unit,
    args_t * args)
{
    SHR_FUNC_INIT_VARS(unit);

    /** invoke common(dnx and dnxf) init sequence */
    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_run(unit, args, NULL));

exit:
    return SHR_ERR_TO_SHELL;
}
