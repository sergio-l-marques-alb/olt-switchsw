/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        ft_class.h
 * Purpose:     Function declarations for flowtracker class.
 */

#ifndef _BCM_INT_FT_CLASS_H_
#define _BCM_INT_FT_CLASS_H_

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
#include <bcm_int/esw/flowtracker/ft_group.h>

extern int
bcmi_ftv3_aggregate_class_init(
    int unit);

extern int
bcmi_ftv3_aggregate_class_cleanup(
    int unit);

extern int
bcmi_ftv3_aggregate_class_create(
    int unit,
    uint32 options,
    bcm_flowtracker_group_type_t group_type,
    bcm_flowtracker_aggregate_class_info_t *aggregate_class_info,
    uint32 *aggregate_class);

extern int
bcmi_ftv3_aggregate_class_get(
    int unit,
    uint32 aggregate_class,
    bcm_flowtracker_group_type_t group_type,
    bcm_flowtracker_aggregate_class_info_t *aggregate_class_info);

extern int
bcmi_ftv3_aggregate_class_get_all(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    int size,
    uint32 *aggregate_class_array,
    int *actual_size);

extern int
bcmi_ftv3_aggregate_class_destroy(
    int unit,
    uint32 aggregate_class,
    bcm_flowtracker_group_type_t group_type);

extern int
bcmi_ftv3_aggregate_class_entry_add_check(
    int unit,
    uint32 aggregate_class);

extern int
bcmi_ftv3_aggregate_class_ref_update(int unit,
    uint32 aggregate_class,
    int count);

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */
#endif /* _BCM_INT_FT_CLASS_H_ */
