/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/flowtracker.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident3.h>

static sal_mutex_t mutex[BCM_MAX_NUM_UNITS];

int bcm_esw_flowtracker_lock(int unit)
{
    if (mutex[unit] == NULL)
    {
        return BCM_E_INIT;
    }

    sal_mutex_take(mutex[unit], sal_mutex_FOREVER);

    return BCM_E_NONE;
}

int bcm_esw_flowtracker_unlock(int unit)
{
    if (mutex[unit] == NULL)
    {
        return BCM_E_INIT;
    }

    if (sal_mutex_give(mutex[unit]) != 0)
    {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_esw_flowtracker_init
 * Purpose:
 *      Initialize the Flowtracker subsystem.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_init(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Create mutex */
    if (mutex[unit] == NULL)
    {
        mutex[unit] = sal_mutex_create("ft.mutex");

        if (mutex[unit] == NULL)
        {
            return BCM_E_MEMORY;
        }
    }
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_init(unit);
    }

    /* If init itself fails, there is no point in having the mutex.
     * Destroy it.
     */
    if (BCM_FAILURE(result))
    {
        sal_mutex_destroy(mutex[unit]);

        mutex[unit] = NULL;
    }
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/* Shut down the Flowtracker subsystem. */
int bcm_esw_flowtracker_detach(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_detach(unit);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_export_template_create
 * Purpose:
 *      Create an export template
 * Parameters:
 *      unit                    - (IN)    BCM device number
 *      options                 - (IN)    Template create options
 *      id                      - (INOUT) Template Id
 *      set_id                  - (IN)    set_id to be used for the template
 *      num_export_elements     - (IN)    Number of elements in the template
 *      list_of_export_elements - (IN)    Export element list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_export_template_create(
        int unit,
        uint32 options,
        bcm_flowtracker_export_template_t *id,
        uint16 set_id,
        int num_export_elements,
        bcm_flowtracker_export_element_info_t *list_of_export_elements)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_export_template_create(unit,
                                                   options,
                                                   id,
                                                   set_id,
                                                   num_export_elements,
                                                   list_of_export_elements);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_export_template_get
 * Purpose:
 *      Get a flowtracker export template with ID.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Template Id
 *      set_id                  - (OUT) Set Id of the template
 *      max_size                - (IN)  Size of the export element list array
 *      list_of_export_elements - (OUT) Export element list
 *      list_size               - (OUT) Number of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_export_template_get(
        int unit,
        bcm_flowtracker_export_template_t id,
        uint16 *set_id,
        int max_size,
        bcm_flowtracker_export_element_info_t *list_of_export_elements,
        int *list_size)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_export_template_get(unit,
                                                id,
                                                set_id,
                                                max_size,
                                                list_of_export_elements,
                                                list_size);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_export_template_destroy
 * Purpose:
 *      Destroy a flowtracker export template
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_export_template_destroy(
        int unit,
        bcm_flowtracker_export_template_t id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_export_template_destroy(unit,
                id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}


/*
 * Function:
 *      bcm_esw_flowtracker_collector_create
 * Purpose:
 *      Create a flowtracker collector with given collector info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Collector create options
 *      id              - (INOUT)  Collector Id
 *      collector_info  - (IN)  Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_collector_create(
        int unit,
        uint32 options,
        bcm_flowtracker_collector_t *collector_id,
        bcm_flowtracker_collector_info_t *collector_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_collector_create(unit, options,
                                             collector_id,
                                             collector_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_collector_get
 * Purpose:
 *      Get flowtracker collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      id              - (IN)  Collector Id
 *      collector_info  - (OUT) Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_collector_get(
        int unit,
        bcm_flowtracker_collector_t id,
        bcm_flowtracker_collector_info_t *collector_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_collector_get(unit, id,
                                          collector_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;

}

/*
 * Function:
 *      bcm_esw_flowtracker_collector_get_all
 * Purpose:
 *      Get the list of all flowtracker collector Ids configured.
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      max_size             - (IN)  Size of the collector list array
 *      collector_list       - (OUT) List of collector Ids configured
 *      list_size            - (OUT) NUmber of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_collector_get_all(
                                     int unit,
                                     int max_size,
                                     bcm_flowtracker_collector_t *collector_list,
                                     int *list_size)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_collector_get_all(unit,
                                              max_size,
                                              collector_list,
                                              list_size);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_collector_destroy
 * Purpose:
 *      Destroy a flowtracker collector
 * Parameters:
 *      unit           - (IN)  BCM device number
 *      collector_id   - (IN) Collector Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_collector_destroy(
        int unit,
        bcm_flowtracker_collector_t id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_collector_destroy(unit, id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}


/*
 * Function:
 *      bcm_esw_flowtracker_group_create
 * Purpose:
 *      Create a flowtracker flow group
 * Parameters:
 *      unit              - (IN)    BCM device number
 *      options           - (IN)    Group create options
 *      id                - (INOUT) Group Id
 *      flow_group_info   - (IN)    Group Info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_create(
        int unit,
        uint32 options,
        bcm_flowtracker_group_t *flow_group_id,
        bcm_flowtracker_group_info_t *flow_group_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_create(unit, options,
                                              flow_group_id,
                                              flow_group_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_get
 * Purpose:
 *      Get flowtracker flow group information
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      id                - (IN)  Group Id
 *      flow_group_info   - (OUT) Group Info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_info_t *flow_group_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_get(unit, id,
                                           flow_group_info);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_get_all
 * Purpose:
 *      Get list of all flow groups created
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      max_size          - (IN)  Size of the flow group list array
 *      flow_group_list   - (OUT) List of flow groups created
 *      list_size         - (OUT) Number of flow grups in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_get_all(
        int unit,
        int max_size,
        bcm_flowtracker_group_t *flow_group_list,
        int *list_size)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_get_all(unit,
                                               max_size,
                                               flow_group_list,
                                               list_size);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_flow_limit_set
 * Purpose:
 *      Set flow limit on the flow group
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      id           - (IN)  Flow group Id
 *      flow_limit   - (IN) Max number of flows that can be learnt on the group
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_flow_limit_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flow_limit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_flow_limit_set(unit, id,
                                                      flow_limit);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_ft_flow_group_flow_limit_get
 * Purpose:
 *      Get flow limit of the flow group
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      id           - (IN)  Flow group Id
 *      flow_limit   - (OUT) Flow limit
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_flow_limit_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_limit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_flow_limit_get(unit, id,
                                                      flow_limit);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_age_timer_set
 * Purpose:
 *      Set aging timer interval in ms on the flow group. Aging interval has to
 *      be configured in steps of 1sec with a minimum of 1sec. Default value of
 *      1 minute.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      aging_interval_ms   - (IN) Aging interval in msec
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_age_timer_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 aging_interval_ms)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_age_timer_set(unit, id,
                                                     aging_interval_ms);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_age_timer_get
 * Purpose:
 *      Get aging timer interval in ms set on the flow group.
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      id                  - (IN)  Flow group Id
 *      aging_interval_ms   - (OUT) Aging interval in msec
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_age_timer_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *aging_interval_ms)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_age_timer_get(unit, id,
                                                     aging_interval_ms);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/* Set export trigger information of the flow group with ID. */
int bcm_esw_flowtracker_group_export_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    return BCM_E_UNAVAIL;
}

/* Get export trigger information of the flow group with ID. */
int bcm_esw_flowtracker_group_export_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_flow_count_get
 * Purpose:
 *      Get the number of flows learnt in the flow group
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      id               - (IN)  Flow group Id
 *      flow_count       - (OUT) Number of flows learnt
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_flow_count_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_count)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_flow_count_get(unit, id,
                                                      flow_count);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_destroy
 * Purpose:
 *      Destroy a flowtracker flow group
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      id               - (IN)  Flow group Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_destroy(
        int unit,
        bcm_flowtracker_group_t id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
            soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_destroy(unit, id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_clear
 * Purpose:
 *      Clear a flow group's flow entries.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      flags         - (IN)  Clear params
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_clear(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flags)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_group_clear(unit, id, flags);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_add
 * Purpose:
 *      Associate flow group to a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_collector_add(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_collector_t collector_id,
        bcm_flowtracker_export_template_t template_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_collector_add(unit, flow_group_id,
                                                     collector_id, template_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_delete
 * Purpose:
 *      Dis-associate flow group from a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_collector_delete(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_collector_t collector_id,
        bcm_flowtracker_export_template_t template_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_collector_delete(unit, flow_group_id,
                                                        collector_id, template_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_collector_get_all
 * Purpose:
 *      Get list of all collectors, templates  attached to a flow group
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      flow_group_id       - (IN)  Flow group Id
 *      max_list_size       - (IN)  Size of the list arrays
 *      list_of_collectors  - (OUT) Collector list
 *      list_of_templates   - (OUT) Template list
 *      list_size           - (OUT) Number of elements in the lists
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_collector_get_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_list_size,
    bcm_flowtracker_collector_t *list_of_collectors,
    bcm_flowtracker_export_template_t *list_of_templates,
    int *list_size)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_flow_group_collector_get_all(unit, flow_group_id,
                                                         max_list_size,
                                                         list_of_collectors,
                                                         list_of_templates,
                                                         list_size);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_data_get
 * Purpose:
 *      Get flow data for a given flow key within the given flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      flow_key      - (IN)  Five tuple that constitutes a flow
 *      flow_data     - (OUT) Data associated with the flow key
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_data_get(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_flow_key_t *flow_key,
        bcm_flowtracker_flow_data_t *flow_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    /* Learning and exporting features via UC are both
     * supported together only on TH series platforms.
     * Call TH flowtracker APIs.
     */
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_group_data_get(unit, flow_group_id,
                                           flow_key, flow_data);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_template_transmit_config_set
 * Purpose:
 *      Set the template set transmit configuration
 * Parameters:
 *      unit         - (IN) BCM device number
 *      template_id  - (IN) Template Id
 *      collector_id - (IN) Collector Id
 *      config       - (IN) Template transmit config
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_template_transmit_config_set(
                             int unit,
                             bcm_flowtracker_export_template_t template_id,
                             bcm_flowtracker_collector_t collector_id,
                             bcm_flowtracker_template_transmit_config_t *config)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_export)) {
            result = _bcm_th_ft_template_transmit_config_set(unit, template_id,
                                                             collector_id, config);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_template_transmit_config_get
 * Purpose:
 *      Get the template set transmit configuration
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      template_id  - (IN)  Template Id
 *      collector_id - (IN)  Collector Id
 *      config       - (OUT) Template transmit config
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_esw_flowtracker_template_transmit_config_get(
                             int unit,
                             bcm_flowtracker_export_template_t template_id,
                             bcm_flowtracker_collector_t collector_id,
                             bcm_flowtracker_template_transmit_config_t *config)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_export)) {
            result = _bcm_th_ft_template_transmit_config_get(unit,
                                                             template_id,
                                                             collector_id,
                                                             config);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_actions_set
 * Purpose:
 *      Set list of actions on a flow group.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      flags         - (IN) Flags
 *      num_actions   - (IN) Number of actions in the list.
 *      action_list   - (IN) Action list.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_actions_set(
                               int unit,
                               bcm_flowtracker_group_t flow_group_id,
                               uint32 flags,
                               int num_actions,
                               bcm_flowtracker_group_action_info_t *action_list)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_group_actions_set(unit, flow_group_id, flags,
                                              num_actions, action_list);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_actions_get
 * Purpose:
 *      Get list of actions applied on a flow group.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      flags         - (IN) Flags
 *      max_actions   - (IN) Maximum number of actions that can be
 *                           accomodated in the list.
 *      action_list   - (OUT) Action list.
 *      num_actions   - (OUT) Actual number of actions in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_actions_get(
                               int unit,
                               bcm_flowtracker_group_t flow_group_id,
                               uint32 flags,
                               int max_actions,
                               bcm_flowtracker_group_action_info_t *action_list,
                               int *num_actions)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_group_actions_get(unit, flow_group_id, flags,
                                              max_actions, action_list,
                                              num_actions);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_elephant_profile_create
 * Purpose:
 *      Create flowtracker elephant profile
 * Parameters:
 *      unit         - (IN)     BCM device number
 *      options      - (IN)     Elephant profile creation options.
 *      profile      - (IN)     Elephant profile information
 *      profile_id   - (IN/OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_elephant_profile_create(
                                int unit,
                                uint32 options,
                                bcm_flowtracker_elephant_profile_info_t *profile,
                                bcm_flowtracker_elephant_profile_t *profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_elephant_profile_create(unit, options,
                                                    profile, profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_elephant_profile_destroy
 * Purpose:
 *       Destroy a flowtracker elephant profile.
 * Parameters:
 *      unit         - (IN)     BCM device number
 *      profile_id   - (IN/OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_elephant_profile_destroy(
                                   int unit,
                                   bcm_flowtracker_elephant_profile_t profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_elephant_profile_destroy(unit, profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_elephant_profile_get
 * Purpose:
 *       Get flowtracker elephant profile information.
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      profile_id   - (IN)  Elephant profile id
 *      profile      - (OUT) Elephant profile information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_elephant_profile_get(
                                             int unit,
                                             bcm_flowtracker_elephant_profile_t profile_id,
                                             bcm_flowtracker_elephant_profile_info_t *profile)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_elephant_profile_get(unit,
                                                 profile_id,
                                                 profile);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_elephant_profile_get_all
 * Purpose:
 *       Get the list of all flowtracker elephant profiles configured.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      max           - (IN)  Max number of profile information that can be
 *                            accomodated within profile_list.
 *      profile_list  - (OUT) List of Elephant profile Ids configured
 *      count         - (OUT) Actual number of elephant profiles
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_elephant_profile_get_all(
                           int unit,
                           int max,
                           bcm_flowtracker_elephant_profile_t *profile_list,
                           int *count)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_elephant_profile_get_all(unit, max,
                                                     profile_list,
                                                     count);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_elephant_profile_attach
 * Purpose:
 *       Attach a flow group with an elephant profile.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      profile_id    - (IN) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_elephant_profile_attach(
                                   int unit,
                                   bcm_flowtracker_group_t flow_group_id,
                                   bcm_flowtracker_elephant_profile_t profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_group_elephant_profile_attach(unit,
                                                          flow_group_id,
                                                          profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_elephant_profile_attach_get
 * Purpose:
 *       Get the elephant profile Id attached with a flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      profile_id    - (OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_elephant_profile_attach_get(
                                  int unit,
                                  bcm_flowtracker_group_t flow_group_id,
                                  bcm_flowtracker_elephant_profile_t *profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_group_elephant_profile_attach_get(unit,
                                                              flow_group_id,
                                                              profile_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

/*
 * Function:
 *      bcm_esw_flowtracker_group_elephant_profile_detach
 * Purpose:
 *       Detach a flow group from an elephant profile.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_flowtracker_group_elephant_profile_detach(
                                           int unit,
                                           bcm_flowtracker_group_t flow_group_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_group_elephant_profile_detach(unit,
                                                          flow_group_id);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_flowtracker_sync
 * Purpose:
 *      Warmboot scache sync
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_flowtracker_sync(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_FLOWTRACKER)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_lock(unit));
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) &&
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        result = _bcm_th_ft_sync(unit);
    }
    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_unlock(unit));
#endif /* INCLUDE_FLOWTRACKER */
    return result;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_esw_flowtracker_ucast_cosq_resolve
 * Purpose:
 *       Convert the user passed cosq value to H/w cosq value
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      cosq          - (IN)  Cosq value passed by the user
 *      hw_cosq       - (OUT) Cosq value to be programmed in the H/w
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_esw_flowtracker_ucast_cosq_resolve(int unit, uint32 cosq, int *hw_cosq)
{
    int rv = BCM_E_NONE;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(cosq)) {
        if (SOC_IS_TOMAHAWKX(unit)) {
#if defined(BCM_TOMAHAWK_SUPPORT)
            rv = _bcm_th_cosq_index_resolve(unit, cosq, 0,
                                            _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            NULL, hw_cosq, NULL);
#endif /* BCM_TOMAHAWK_SUPPORT */
        } else if (SOC_IS_TRIDENT3(unit)) {
#if defined(BCM_TRIDENT3_SUPPORT)
            rv = _bcm_td3_cosq_index_resolve(unit, cosq, 0,
                                             _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             NULL, hw_cosq, NULL);
#endif /* BCM_TRIDENT3_SUPPORT */
        } else {
            rv = BCM_E_UNAVAIL;
        }
    } else {
        *hw_cosq = cosq;
        rv = BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flowtracker_mcast_cosq_resolve
 * Purpose:
 *       Convert the user passed cosq value to H/w cosq value
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      cosq          - (IN)  Cosq value passed by the user
 *      hw_cosq       - (OUT) Cosq value to be programmed in the H/w
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_esw_flowtracker_mcast_cosq_resolve(int unit, uint32 cosq, int *hw_cosq)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(cosq)) {
        if (SOC_IS_TOMAHAWKX(unit)) {
#if defined(BCM_TOMAHAWK_SUPPORT)
            rv = _bcm_th_cosq_index_resolve(unit, cosq, 0,
                                            _BCM_TH_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, hw_cosq, NULL);
#endif /* BCM_TOMAHAWK_SUPPORT */
        } else if (SOC_IS_TRIDENT3(unit)) {
#if defined(BCM_TRIDENT3_SUPPORT)
            rv = _bcm_td3_cosq_index_resolve(unit, cosq, 0,
                                             _BCM_TH_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             NULL, hw_cosq, NULL);
#endif /* BCM_TRIDENT3_SUPPORT */
        } else {
            rv = BCM_E_UNAVAIL;
        }
    } else {
        *hw_cosq = cosq;
        rv = BCM_E_NONE;
    }
    return rv;
}

void _bcm_esw_flowtracker_sw_dump(int unit)
{
#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        _bcm_xgs5_flowtracker_sw_dump(unit);
    }
#endif /* INCLUDE_FLOWTRACKER */
}
