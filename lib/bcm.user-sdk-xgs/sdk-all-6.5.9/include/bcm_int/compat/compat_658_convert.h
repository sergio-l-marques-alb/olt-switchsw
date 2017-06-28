/*
 * Copyright: (c)  Broadcom Corp.
 * All Rights Reserved.
 *
 * File:    compat_658_convert.h
 * Purpose: Convert datatypes from/to version 6.5.8 
 * Generator:   mkdispatch 1.54
 */

#ifndef _COMPAT_658_CONVERT_H_
#define _COMPAT_658_CONVERT_H_

#ifdef  BCM_RPC_SUPPORT

#include <bcm_int/compat/compat_658.h>

#include <bcm/ecn.h>
#include <bcm/field.h>
#include <bcm/mpls.h>
#include <bcm/oam.h>
#include <bcm/policer.h>
#include <bcm/port.h>
#include <bcm/rx.h>
#include <bcm/stat.h>
#include <bcm/trunk.h>
#include <bcm/types.h>
#include <shared/phyconfig.h>
#include <shared/phyreg.h>
#include <shared/port.h>
#include <shared/port_ability.h>
#include <shared/portmode.h>
#include <shared/switch.h>


/*
 * Function: 
 *     _bcm_compat658in_policer_config_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_policer_config_t (
    bcm_compat658_policer_config_t *from,
    bcm_policer_config_t *to);

/*
 * Function: 
 *     _bcm_compat658in_policer_group_mode_attr_selector_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_policer_group_mode_attr_selector_t (
    bcm_compat658_policer_group_mode_attr_selector_t *from,
    bcm_policer_group_mode_attr_selector_t *to);

/*
 * Function: 
 *     _bcm_compat658out_policer_config_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_policer_config_t (
    bcm_policer_config_t *from,
    bcm_compat658_policer_config_t *to);

/*
 * Function: 
 *     _bcm_compat658out_policer_group_mode_attr_selector_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_policer_group_mode_attr_selector_t (
    bcm_policer_group_mode_attr_selector_t *from,
    bcm_compat658_policer_group_mode_attr_selector_t *to);

/*
 * Function: 
 *     _bcm_compat658in_field_group_config_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_field_group_config_t (
    bcm_compat658_field_group_config_t *from,
    bcm_field_group_config_t *to);

/*
 * Function: 
 *     _bcm_compat658out_field_group_config_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_field_group_config_t (
    bcm_field_group_config_t *from,
    bcm_compat658_field_group_config_t *to);

/*
 * Function: 
 *     _bcm_compat658out_port_config_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_port_config_t (
    bcm_port_config_t *from,
    bcm_compat658_port_config_t *to);

/*
 * Function: 
 *     _bcm_compat658in_trunk_info_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_trunk_info_t (
    bcm_compat658_trunk_info_t *from,
    bcm_trunk_info_t *to);

/*
 * Function: 
 *     _bcm_compat658out_trunk_info_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_trunk_info_t (
    bcm_trunk_info_t *from,
    bcm_compat658_trunk_info_t *to);


#if defined(INCLUDE_L3)
/*
 * Function: 
 *     _bcm_compat658in_ecn_map_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_ecn_map_t (
    bcm_compat658_ecn_map_t *from,
    bcm_ecn_map_t *to);


#endif


#if defined(INCLUDE_L3)
/*
 * Function: 
 *     _bcm_compat658out_ecn_map_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_ecn_map_t (
    bcm_ecn_map_t *from,
    bcm_compat658_ecn_map_t *to);


#endif

/*
 * Function: 
 *     _bcm_compat658in_stat_lif_counting_source_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_stat_lif_counting_source_t (
    bcm_compat658_stat_lif_counting_source_t *from,
    bcm_stat_lif_counting_source_t *to);

/*
 * Function: 
 *     _bcm_compat658in_stat_lif_counting_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_stat_lif_counting_t (
    bcm_compat658_stat_lif_counting_t *from,
    bcm_stat_lif_counting_t *to);

/*
 * Function: 
 *     _bcm_compat658out_stat_lif_counting_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_stat_lif_counting_t (
    bcm_stat_lif_counting_t *from,
    bcm_compat658_stat_lif_counting_t *to);

/*
 * Function: 
 *     _bcm_compat658in_oam_endpoint_info_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_oam_endpoint_info_t (
    bcm_compat658_oam_endpoint_info_t *from,
    bcm_oam_endpoint_info_t *to);

/*
 * Function: 
 *     _bcm_compat658out_oam_endpoint_info_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_oam_endpoint_info_t (
    bcm_oam_endpoint_info_t *from,
    bcm_compat658_oam_endpoint_info_t *to);


#endif  /* BCM_RPC_SUPPORT */


#endif /* _COMPAT_658_H_ */
