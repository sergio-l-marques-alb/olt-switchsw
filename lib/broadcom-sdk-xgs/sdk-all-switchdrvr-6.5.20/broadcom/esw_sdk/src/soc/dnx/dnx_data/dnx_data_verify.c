/** \file dnx_data_verify.c
 * 
 * MODULE DATA CUSTOME PROPRTY - 
 * Includes all custom functions implementations
 * 
 * Device Data
 * SW component that maintains per device data
 * The data is static and won't be changed after device initialization.
 *     
 * Supported data types:
 *     - Define             - a 'uint32' number (a max value for all devices is maintained)
 *     - feature            - 1 bit per each feature (supported/not supported) - support soc properties 
 *     - table              - the data is accessed with keys and/or can maintain multiple values and/or set by soc property
 *     - numeric            - a 'uint32' number that support soc properties
 * 
 * User interface for DNX DATA component can be found in "dnx_data_if.h" and "dnx_data_if_#module#.h"
 * 
 * Adding the data is done via XMLs placed in "tools/autocoder/DeviceData/dnx/.." 
 * "How to" User Guide can be found in confluence. 
 */

/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

/*
 * INCLUDE FILES:
 * {
 */
#include <bcm/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <sal/appl/sal.h>
#include <soc/drv.h> /**soc_property_port_get_str*/

/*
 * }
 */
/*
 * See .h file
 */
shr_error_e
dnx_data_property_unsupported_verify(
    int unit)
{
    const dnxc_data_table_info_t *table_info;
    const dnx_data_dev_init_properties_unsupported_t *prop_info;
    int property_index;
    char *val = NULL;
    bcm_port_t logical_port;
    int num_index;
    SHR_FUNC_INIT_VARS(unit);

    /** Get size of table default size */
    table_info = dnx_data_dev_init.properties.unsupported_info_get(unit);

    /*
     * Iterate over the table and check each soc property
     * according to the parameters.
     */
    for (property_index = 0; property_index < table_info->key_size[0]; property_index++)
    {
        prop_info = dnx_data_dev_init.properties.unsupported_get(unit, property_index);

        /*
         * The soc property is per port - make sure the soc property is not used for any of the ports
         */
        if (prop_info->per_port)
        {
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                val = soc_property_port_get_str(unit, logical_port, prop_info->property);
                if (val != NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "soc property is not supported: name: %s port: %d\n %s\n",
                                 prop_info->property, logical_port,
                                 prop_info->err_msg != NULL ? prop_info->err_msg : "");
                }
            }
        }
        /*
         * The soc property is read with suffix / suffix and a number / just number
         */
        else if (prop_info->suffix != NULL || prop_info->num_max != -1)
        {
            /** go over the range of the numbers */
            for (num_index = -1; num_index <= prop_info->num_max; num_index++)
            {
                val =
                    soc_property_suffix_num_str_get(unit, num_index, prop_info->property,
                                                    prop_info->suffix != NULL ? prop_info->suffix : "");
                if (val != NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "soc property is not supported: name: %s suffix: %s\n %s\n",
                                 prop_info->property, prop_info->suffix != NULL ? prop_info->suffix : "",
                                 prop_info->err_msg != NULL ? prop_info->err_msg : "");
                }
            }
        }
        /*
         * standard soc property
         */
        else
        {
            val = soc_property_get_str(unit, prop_info->property);
            if (val != NULL)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "soc property is not supported: name: %s \n %s\n",
                             prop_info->property, prop_info->err_msg != NULL ? prop_info->err_msg : "");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
