/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * IFA - In-band Flow Analyzer Monitoring Embedded Application APP interface
 * Purpose: API to configure IFA embedded app interface.
 *
 */

#include <shared/bslenum.h>
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/ifa.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident3.h>

static sal_mutex_t mutex[BCM_MAX_NUM_UNITS];
#if defined(INCLUDE_IFA)
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
static uint8 ifa_uc_version = 0;
#endif  /* BCM_TRIDENT3_SUPPORT || BCM_MAVERICK2_SUPPORT */
#endif  /* INCLUDE_IFA */

int bcm_esw_ifa_lock(int unit)
{
    if (mutex[unit] == NULL) {
        return BCM_E_INIT;
    }

    sal_mutex_take(mutex[unit], sal_mutex_FOREVER);
    return BCM_E_NONE;
}

int bcm_esw_ifa_unlock(int unit)
{
    if (mutex[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (sal_mutex_give(mutex[unit]) != 0) {
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

/* Initialize the IFA app. */
int bcm_esw_ifa_init(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
    uint8 success;
#endif  /* BCM_TRIDENT3_SUPPORT || BCM_MAVERICK2_SUPPORT */

    /* Create mutex */
    if (mutex[unit] == NULL) {
        mutex[unit] = sal_mutex_create("ifa.mutex");
        if (mutex[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
    if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
        result = _bcm_td3_ifa_init(unit, &success);
        if (result == BCM_E_NONE) {
            if (success) {
                ifa_uc_version = 1;
            } else {
                /* IFAv1 app is not loaded, try loading IFAv2. */
                result = _bcm_xgs5_ifa2_init(unit, &success);
                if ((result == BCM_E_NONE) && (success)) {
                    ifa_uc_version = 2;
                }
            }
        }
    }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_TRIDENT3_SUPPORT)
    if ((SOC_WARM_BOOT(unit)) && (SOC_IS_TRIDENT3(unit))) {
        result = _bcm_td3_ifa_leap_init(unit);
        if (BCM_FAILURE(result)) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                            "IFA LEAP(unit %d) INIT Error%s\n"),
                        unit, bcm_errmsg(result)));
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT && BCM_TRIDENT3_SUPPORT */

    /* If init itself fails, there is no point in having the mutex.
     * Destroy it.
     */
    if (BCM_FAILURE(result)) {
        sal_mutex_destroy(mutex[unit]);
        mutex[unit] = NULL;
    }
#endif  /* INCLUDE_IFA */
    return result;
}

/* Shut down the IFA app. */
int bcm_esw_ifa_detach(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
    if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
        if (ifa_uc_version == 1) {
            result = _bcm_td3_ifa_detach(unit);
        } else if (ifa_uc_version == 2) {
            result = _bcm_xgs5_ifa2_detach(unit);
        } else {
            result = BCM_E_NONE;
        }
        ifa_uc_version = 0;
    }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

/*
 * Function:
 *      bcm_esw_ifa_collector_set
 * Purpose:
 *      Create a ifa collector with given collector info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Collector create options
 *      collector_info  - (IN)  Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */

int bcm_esw_ifa_collector_set(int unit,
        uint32 options,
        bcm_ifa_collector_info_t *collector_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            if (ifa_uc_version == 1) {
                result = _bcm_td3_ifa_collector_set(unit, options,
                                                    collector_info);
            } else if (ifa_uc_version == 2) {
                result = BCM_E_UNAVAIL;
            } else {
                result = BCM_E_INIT;
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

/*
 * Function:
 *      bcm_esw_ifa_collector_get
 * Purpose:
 *      Get ifa collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      collector_info  - (OUT) Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */

int bcm_esw_ifa_collector_get(int unit,
        bcm_ifa_collector_info_t *collector_info)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            if (ifa_uc_version == 1) {
                result = _bcm_td3_ifa_collector_get(unit,
                                                    collector_info);
            } else if (ifa_uc_version == 2) {
                result = BCM_E_UNAVAIL;
            } else {
                result = BCM_E_INIT;
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

 /*
  * Function:
  *      bcm_esw_ifa_collector_attach
  * Purpose:
  *      Attach collector and export profile.
  * Parameters:
  *      unit              - (IN) BCM device number
  *      collector_id      - (IN) Collector ID
  *      export_profile_id - (IN) Export profile ID.
  * Returns:
  *      BCM_E_XXX   - BCM error code.
  */
int bcm_esw_ifa_collector_attach(int unit,
                                 bcm_collector_t collector_id,
                                 int export_profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            if (ifa_uc_version == 1) {
                result = BCM_E_UNAVAIL;
            } else if (ifa_uc_version == 2) {
                result = _bcm_xgs5_ifa2_collector_attach(unit, collector_id,
                                                         export_profile_id);
            } else {
                result = BCM_E_INIT;
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

/*
 * Function:
 *      bcm_esw_ifa_collector_attach_get
 * Purpose:
 *      Get the attached collector and export profile.
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      collector_id      - (OUT) Collector ID
 *      export_profile_id - (OUT) Export profile ID.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_ifa_collector_attach_get(int unit,
                                     bcm_collector_t *collector_id,
                                     int *export_profile_id)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            if (ifa_uc_version == 1) {
                result = BCM_E_UNAVAIL;
            } else if (ifa_uc_version == 2) {
                result = _bcm_xgs5_ifa2_collector_attach_get(unit, collector_id,
                                                             export_profile_id);
            } else {
                result = BCM_E_INIT;
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

 /*
  * Function:
  *      bcm_esw_ifa_collector_detach
  * Purpose:
  *      Detach collector and export profile.
  * Parameters:
  *      unit              - (IN) BCM device number
  *      collector_id      - (IN) Collector ID
  *      export_profile_id - (IN) Export profile ID.
  * Returns:
  *      BCM_E_XXX   - BCM error code.
  */
int bcm_esw_ifa_collector_detach(int unit)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            if (ifa_uc_version == 1) {
                result = BCM_E_UNAVAIL;
            } else if (ifa_uc_version == 2) {
                result = _bcm_xgs5_ifa2_collector_detach(unit);
            } else {
                result = BCM_E_INIT;
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

int bcm_esw_ifa_config_info_set(int unit,
        uint32 options,
        bcm_ifa_config_info_t *config_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            if (ifa_uc_version == 1) {
                result = _bcm_td3_ifa_config_set(unit, options,
                                                 config_data);
            } else if (ifa_uc_version == 2) {
                result = _bcm_xgs5_ifa2_config_info_set(unit, config_data);
            } else {
                result = BCM_E_INIT;
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

int bcm_esw_ifa_config_info_get(int unit,
        bcm_ifa_config_info_t *config_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            if (ifa_uc_version == 1) {
                result = _bcm_td3_ifa_config_get(unit,
                                                 config_data);
            } else if (ifa_uc_version == 2) {
                result = _bcm_xgs5_ifa2_config_info_get(unit, config_data);
            } else {
                result = BCM_E_INIT;
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

/*
 * Function:
 *      bcm_esw_ifa_stat_info_get
 * Purpose:
 *      Get ifa statistics information data
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      stat_data       - (OUT) Statistics information data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_ifa_stat_info_get(int unit,
        bcm_ifa_stat_info_t *stat_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            if (ifa_uc_version == 1) {
                result = _bcm_td3_ifa_stat_get(unit,
                                               stat_data);
            } else if (ifa_uc_version == 2) {
                result = _bcm_xgs5_ifa2_stat_info_get(unit, stat_data);
            } else {
                result = BCM_E_INIT;
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

/*
 * Function:
 *      bcm_esw_ifa_stat_info_set
 * Purpose:
 *      Get ifa statistics information data
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      stat_data       - (OUT) Statistics information data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_esw_ifa_stat_info_set(int unit,
        bcm_ifa_stat_info_t *stat_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            if (ifa_uc_version == 1) {
                result = _bcm_td3_ifa_stat_set(unit,
                                               stat_data);
            } else if (ifa_uc_version == 2) {
                result = _bcm_xgs5_ifa2_stat_info_set(unit, stat_data);
            } else {
                result = BCM_E_INIT;
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_ifa_sync
 * Purpose:
 *      Warmboot scache sync
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_ifa_sync(int unit)
{
    int result = BCM_E_NONE;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            result = _bcm_td3_ifa_sync(unit);
            if (result == BCM_E_NONE) {
                result = _bcm_xgs5_ifa2_sync(unit);
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif /* INCLUDE_IFA */
    return result;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

int bcm_esw_ifa_leap_config_set(int unit,
                                bcm_ifa_leap_config_t *config_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3(unit)) {
            result = _bcm_td3_ifa_leap_config_set(unit, config_data);
        }
#endif  /* BCM_TRIDENT3_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}

int bcm_esw_ifa_leap_config_get(int unit,
                                bcm_ifa_leap_config_t *config_data)
{
    int result = BCM_E_UNAVAIL;
#if defined(INCLUDE_IFA)
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_lock(unit));

    /* IFA app APIs.  */
    if (soc_feature(unit, soc_feature_ifa)) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3(unit)) {
            result = _bcm_td3_ifa_leap_config_get(unit, config_data);
        }
#endif  /* BCM_TRIDENT3_SUPPORT */
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_esw_ifa_unlock(unit));
#endif  /* INCLUDE_IFA */
    return result;
}
