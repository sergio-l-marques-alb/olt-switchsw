/*
 * $Id: stat.c,v 1.1 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <bcm/stat.h>
#include <sal/types.h>
#include <soc/mem.h>

/*
 * Function:
 *      bcm_stat_group_mode_attr_selector_t_init
 * Description:
 *      Initialize an attribute selector of Stat Flex Group Mode
 * Parameters:
 *      attr_selector : (INOUT) Attribute Selector for Stat Flex Group Mode
 * Returns:
 *      NONE
 *     
 */
void bcm_stat_group_mode_attr_selector_t_init(
     bcm_stat_group_mode_attr_selector_t *attr_selector)
{
     if (attr_selector != NULL) {
         sal_memset(attr_selector, 0,
                    sizeof(bcm_stat_group_mode_attr_selector_t));
     }
     return;
}

/*
 * Function:
 *      bcm_stat_value_t_init
 * Description:
 *      Initialize a data structure bcm_stat_value_t
 *      void bcm_stat_value_t_init(bcm_stat_value_t *stat_value)
 * Parameters:
 *      stat_value : (INOUT) Pointer to bcm_stat_value_t structure to be initialized
 * Returns:
 *      NONE
 *
 */
void bcm_stat_value_t_init(
     bcm_stat_value_t *stat_value)
{
     if (stat_value != NULL) {
         sal_memset(stat_value, 0,
                    sizeof(bcm_stat_value_t));
     }
     return;
}

void bcm_stat_group_mode_id_config_t_init(
    bcm_stat_group_mode_id_config_t *stat_config)
{
     if (stat_config != NULL) {
         sal_memset(stat_config, 0,
                    sizeof(bcm_stat_group_mode_id_config_t));
     }
     return;
}

void bcm_stat_group_mode_hint_t_init(
    bcm_stat_group_mode_hint_t *stat_hint)
{
     if (stat_hint != NULL) {
         sal_memset(stat_hint, 0,
                    sizeof(bcm_stat_group_mode_hint_t));
     }
     return;
}

/*
 * Function:
 * bcm_stat_counter_input_data_t_init
 * Purpose:
 * Initialize a stat_counter_input_data object struct.
 * Parameters:
 * stat_input_data - pointer to stat_counter_input_data object struct.
 * Returns:
 * NONE
 */
void bcm_stat_counter_input_data_t_init(
    bcm_stat_counter_input_data_t *stat_input_data)
{
    if (stat_input_data != NULL) {
        sal_memset(stat_input_data, 0,
                  sizeof (bcm_stat_counter_input_data_t));
        stat_input_data->counter_source_gport = -1;
        stat_input_data->counter_source_id = -1;
    }
    return;
}

/*
 * Function:
 * bcm_stat_flex_pool_stat_info_t_init
 * Purpose:
 * Initialize a stat_flex_pool_stat_info object struct.
 * Parameters:
 * stat_flex_pool_stat_info - pointer to stat_flex_pool_stat_info object struct.
 * Returns:
 * NONE
 */
void bcm_stat_flex_pool_stat_info_t_init(
        bcm_stat_flex_pool_stat_info_t *stat_flex_pool_stat_info)
{
    if (stat_flex_pool_stat_info != NULL) {
        sal_memset(stat_flex_pool_stat_info, 0,
                    sizeof (bcm_stat_flex_pool_stat_info_t));
    }
    return;
}

/*
 * Function:
 * bcm_stat_engine_t_init
 * Purpose:
 * Initialize a stat_engine object struct.
 * Parameters:
 * stat_engine - pointer to stat_engine object struct.
 * Returns:
 * NONE
 */
void bcm_stat_engine_t_init(
        bcm_stat_engine_t *stat_engine)
{
    return;
}

 /*
 * Function:
 *      bcm_stat_counter_explicit_input_data_t_init
 * Purpose:
 *      Initialize the bcm_stat_counter_explicit_input_data_t structure
 * Parameters:
 *      stat_counter_explicit_input_data - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_counter_explicit_input_data_t_init(
        bcm_stat_counter_explicit_input_data_t *stat_counter_explicit_input_data)
{
    return;
}

 /*
 * Function:
 *      bcm_stat_eviction_t_init
 * Purpose:
 *      Initialize the bcm_stat_eviction_t structure
 * Parameters:
 *      stat_eviction - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_eviction_t_init(
     bcm_stat_eviction_t *stat_eviction)
{
    return;
}

 /*
 * Function:
 *      bcm_stat_counter_set_map_t_init
 * Purpose:
 *      Initialize the bcm_stat_counter_set_map_t structure
 * Parameters:
 *      stat_counter_set_map - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_counter_set_map_t_init(
        bcm_stat_counter_set_map_t *stat_counter_set_map)
{
    return;
}

 /*
 * Function:
 *      bcm_stat_expansion_data_mapping_t_init
 * Purpose:
 *      Initialize the bcm_stat_expansion_data_mapping_t structure
 * Parameters:
 *      stat_expansion_data_mapping - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
 void bcm_stat_expansion_data_mapping_t_init(
        bcm_stat_expansion_data_mapping_t *stat_expansion_data_mapping)
{
    return;
}

 /*
 * Function:
 *      bcm_stat_counter_interface_key_t_init
 * Purpose:
 *      Initialize the bcm_stat_counter_interface_key_t structure
 * Parameters:
 *      stat_counter_interface_key - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_counter_interface_key_t_init(
        bcm_stat_counter_interface_key_t *stat_counter_interface_key)
{
    return;
}

 /*
 * Function:
 *      bcm_stat_expansion_select_t_init
 * Purpose:
 *      Initialize the bcm_stat_expansion_select_t structure
 * Parameters:
 *      stat_expansion_select - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_expansion_select_t_init(
        bcm_stat_expansion_select_t *stat_expansion_select)
{
    return;
}

 /*
 * Function:
 *      bcm_stat_counter_interface_t_init
 * Purpose:
 *      Initialize the bcm_stat_counter_interface_t structure
 * Parameters:
 *      stat_counter_interface - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_counter_interface_t_init(
        bcm_stat_counter_interface_t *stat_counter_interface)
{
    return;
}

/*
 * Function:
 *      bcm_stat_engine_enable_t_init
 * Purpose:
 *      Initialize the bcm_stat_engine_enable_t structure
 * Parameters:
 *      stat_engine_enable - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_engine_enable_t_init(
        bcm_stat_engine_enable_t *stat_engine_enable)
{
    return;
}

/*
 * Function:
 *      bcm_stat_engine_enable_t_init
 * Purpose:
 *      Initialize the bcm_stat_engine_enable_t structure
 * Parameters:
 *      stat_engine_enable - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
 void bcm_stat_counter_output_data_t_init(
        bcm_stat_counter_output_data_t *stat_counter_output_data)
{
    return;
}
