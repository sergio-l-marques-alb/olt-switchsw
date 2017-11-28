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

#include <ibde.h>
#include <appl/reference/dnxc/appl_ref_init_deinit.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sysconf.h>
#include <soc/cmext.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <bcm/init.h>

/** these externs are needed for a commom step list get function - appl_dnxc_step_list_get*/
#ifdef BCM_DNX_SUPPORT
extern shr_error_e appl_dnx_step_list_get(int unit, const appl_dnxc_init_step_t **appl_steps);
#endif

#ifdef BCM_DNXF_SUPPORT
extern shr_error_e appl_dnxf_step_list_get(int unit, const appl_dnxc_init_step_t **appl_steps);
#endif


/*
 * }
 */

/*
 * Typedefs.
 * {  
 */

/*
 * }
 */


/*
 * Globals.
 * {  
 */
static appl_dnxc_init_param_t init_param[SOC_MAX_NUM_DEVICES];

/*
 * }
 */

/**
 * \brief - parse parameters for DNX_INIT command.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   \param [in] args - arguments recieved to parse over.
 *   \param [out] init_param - returned initialization
 *          parameters
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   Addidtional options may be added, however the general
 *   guidline is to control different init/deinit options using
 *   SOC properties. each new option added here needs to be
 *   added as well to the usage section above.
 * \see
 *   * None
 */
shr_error_e
appl_dnxc_init_parse_params(
    int unit,
    args_t * args)
{
    parse_table_t parse_table;

    SHR_FUNC_INIT_VARS(unit);

    parse_table_init(unit, &parse_table);
    /*
     * additional option might be added in the future, it is important to add
     * those options also to the usage string found in APPL_DNXC_INIT_USAGE. 
     */
    SHR_IF_ERR_EXIT(parse_table_add(&parse_table, "NoInit", PQ_INT, (void *) 0, &(init_param[unit].no_init), NULL));
    SHR_IF_ERR_EXIT(parse_table_add
                    (&parse_table, "NoDeinit", PQ_INT | PQ_DFL, (void *) 1, &(init_param[unit].no_deinit), NULL));

    SHR_IF_ERR_EXIT(parse_arg_eq(args, &parse_table));

    /*
     * make sure that no extra args were recieved as input
     */
    if (ARG_CNT(args) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%s: extra options starting with \"%s\"\n%s\n", ARG_CMD(args), ARG_CUR(args),
                     APPL_DNXC_INIT_USAGE);
    }

    /*
     * This is used to free memory allocated for parse_table_init
     */
    parse_arg_eq_done(&parse_table);

exit:
    SHR_FUNC_EXIT;
}

/* 
 * See .h file 
 */
shr_error_e
appl_dnxc_sdk_init(int unit)
{

    int rv;
    uint16 device_id;
    uint8 revision_id;
    const ibde_dev_t *device;

    SHR_FUNC_INIT_VARS(unit);

    /* 
     * Attach unit only if both deinit and init is required (full sequence). 
     * Meaning that on cold boot attach is not required since it's already invoked from the shell. 
     */
    if((init_param[unit].no_deinit == 0) && (init_param[unit].no_init == 0))
    {
        /*
         * Override device ID and revision if needed
         */
        device = bde->get_dev(unit);
        device_id = device->device;
        revision_id = device->rev;
        sysconf_chip_override(unit, &device_id, &revision_id);

        /*
         * Step 1 (cmext.h) 
         * Verify device type is supported 
         */
        rv = soc_cm_device_supported(device_id, revision_id);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "The Device is not supported, attemped to probe for Device:%u, Revision:%u\n%s",
                                 device_id, revision_id, EMPTY);
        /*
         * Step 2 (cmext.h): 
         * Create unit with id 
         */
        rv = soc_cm_device_create_id(device_id, revision_id, NULL, unit);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "soc_cm_device_create() for device_id %u Failed:\n%s%s", device_id, EMPTY, EMPTY);

        /*
         * Step 3 (cmext.h) 
         * Attach unit: 
         * perform all needed actions to call bcm_init, 
         * essentially this preliminary step need to be 
         * as lean as possible, as many steps as possible 
         * should be called from inside bcm_init.
         */
        LOG_INFO_EX(BSL_LOG_MODULE, "   %d: Attach unit.\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);
        rv = sysconf_attach(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "sysconf_attach() Failed:\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    LOG_INFO_EX(BSL_LOG_MODULE, "   %d: BCM Init\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);
    rv = bcm_init(unit);
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "bcm_init() Failed:\n%s%s%s", EMPTY, EMPTY, EMPTY);
    LOG_INFO_EX(BSL_LOG_MODULE, "   %d: BCM Init Done\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}


/* 
 * See .h file
 */
shr_error_e
appl_dnxc_sdk_deinit(int unit)
{
    int rv;
    uint16 device_id;
    uint8 revision_id;
    const ibde_dev_t *device;

    SHR_FUNC_INIT_VARS(unit);

    /* 
     * Deinit bcm: 
     * bcm_detach() API is the opposite of bcm_init() API
     */
    LOG_INFO_EX(BSL_LOG_MODULE, "   %d: BCM Deinit\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);
    rv = bcm_detach(unit);
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "bcm_detach() for unit %u Failed:\n%s%s", unit, EMPTY, EMPTY);
    LOG_INFO_EX(BSL_LOG_MODULE, "   %d: BCM Deinit Done\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);

    /* 
     * Detach unit only if both deinit and init is required (full sequence). 
     * Otherwise we won't be able to attach back a detached unit. 
     */
    if((init_param[unit].no_deinit == 0) && (init_param[unit].no_init == 0))
    {
        /*
         * Override device ID and revision if needed
         */
        device = bde->get_dev(unit);
        device_id = device->device;
        revision_id = device->rev;
        sysconf_chip_override(unit, &device_id, &revision_id);


        LOG_INFO_EX(BSL_LOG_MODULE, "   %d: Detach unit\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);
        rv = soc_cm_device_destroy(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "soc_cm_device_destroy() for device_id %u Failed:\n%s%s", device_id, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxc_step_list_get(
    int unit, 
    const appl_dnxc_init_step_t **appl_steps)
{
    SHR_FUNC_INIT_VARS(unit);
   
    if (SOC_IS_DNXF(unit))
    {
#ifdef BCM_DNXF_SUPPORT 
        SHR_IF_ERR_EXIT(appl_dnxf_step_list_get(unit, appl_steps));
#endif
    }
    else if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT 
        SHR_IF_ERR_EXIT(appl_dnx_step_list_get(unit, appl_steps));
#endif
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid Architecture\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Override init params. This function provies settin init params after cold boot. 
 * This function should be used by external modules (diagnostics, testing, etc...)
 * 
 * \param [in] unit - Unit ID
 * \param [Out] new_params - Pointer to init paramaters
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
appl_dnxc_init_params_set(
    int unit, 
    appl_dnxc_init_param_t *new_params)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memcpy(&(init_param[unit]), new_params, sizeof(appl_dnxc_init_param_t)); 

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxc_init_step_list_run(
    int unit,
    args_t *args,
    appl_dnxc_init_param_t *params)
{
    const appl_dnxc_init_step_t *appl_steps;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_param[unit], 0x0, sizeof(appl_dnxc_init_param_t));

    /** get the list to run */
    SHR_IF_ERR_EXIT(appl_dnxc_step_list_get(unit, &appl_steps));

    /*
     * default init_param values
     */
    init_param[unit].no_deinit = 1;

    /* Get init params.
     *  
     * Init params can be provided by the following 2 ways: 
     * 1. Through command line (usually by init application during cold boot) 
     * 2. Explicitly provided to this function (by testing or diagnostics) 
     *  
     * If args!=NULL then use args, otherwise use the provided params. 
     */
    {
        if (args != NULL) /** if args are provided, parse them */
        {
            SHR_IF_ERR_EXIT(appl_dnxc_init_parse_params(unit, args));
        }
        else if (params != NULL)/** if no args provided, use the explictily provided params */
        {
            SHR_IF_ERR_EXIT(appl_dnxc_init_params_set(unit, params));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid init params\n");
        }
    }

    /*
     * Deinit if needed
     */
    if (init_param[unit].no_deinit == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%d: APPL Deinit\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);    
        SHR_IF_ERR_EXIT(appl_dnxc_steps_convert_and_run(unit, appl_steps, FALSE));
        LOG_INFO_EX(BSL_LOG_MODULE, "%d: APPL Deinit Done\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);  
    }

    /*
     * Init if needed
     */
    if (init_param[unit].no_init == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%d: APPL Init\n%s%s%s", unit, EMPTY, EMPTY, EMPTY); 
        SHR_IF_ERR_EXIT(appl_dnxc_steps_convert_and_run(unit, appl_steps, TRUE));
        LOG_INFO_EX(BSL_LOG_MODULE, "%d: APPL Init Done\n%s%s%s", unit, EMPTY, EMPTY, EMPTY); 
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxc_step_list_id_run(
            int unit, 
            int appl_id, 
            const appl_dnxc_init_step_t *appl_steps,
            int is_forward)
{
    int is_found;
    utilex_seq_step_t *utilex_seq_list;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_convert(unit, appl_steps, &utilex_seq_list));

    /** run a step within a list by name */
    SHR_IF_ERR_EXIT(utilex_seq_run_step_by_id(unit, utilex_seq_list, appl_id, is_forward, &is_found));

    if (is_found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid step id = %d\n", appl_id);
    }

exit:
    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_destory(unit, utilex_seq_list));
    SHR_FUNC_EXIT;
}


shr_error_e
appl_dnxc_step_list_name_run(
    int unit, 
    char *appl_name, 
    const appl_dnxc_init_step_t *appl_steps,
    int is_forward)
{
    int is_found;
    utilex_seq_step_t *utilex_seq_list;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_convert(unit, appl_steps, &utilex_seq_list));

    /** run a step within a list by name */
    SHR_IF_ERR_EXIT(utilex_seq_run_step_by_name(unit, utilex_seq_list, appl_name, is_forward, &is_found));

    if (is_found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid step name = %s\n", appl_name);
    }

exit:
    /** free allocated memory */
    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_destory(unit, utilex_seq_list));
    SHR_FUNC_EXIT;
}
