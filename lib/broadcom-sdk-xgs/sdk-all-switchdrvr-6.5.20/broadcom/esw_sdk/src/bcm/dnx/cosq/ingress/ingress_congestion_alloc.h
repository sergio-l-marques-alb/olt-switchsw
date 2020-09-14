/** \file src/bcm/dnx/cosq/ingress/ingress_congestion_alloc.h
 * $Id$
 * 
 * Allocation for ingress congestion module
 * 
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _INGRESS_CONGESTION_ALLOC_H_INCLUDED_
/** { */
#define _INGRESS_CONGESTION_ALLOC_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

int dnx_ingress_congestion_alloc_vsq_rate_class_template_mngr_init(
    int unit);

int dnx_ingress_congestion_alloc_vsq_rate_class_data_get(
    int unit,
    int core_id,
    dnx_ingress_congestion_vsq_group_e vsq_group,
    int vsq_id,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_cls);

int dnx_ingress_congestion_alloc_vsq_rate_class_exchange(
    int unit,
    int core_id,
    dnx_ingress_congestion_vsq_group_e vsq_group,
    int vsq_id,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_cls,
    int *old_rate_cls,
    int *is_last,
    int *new_rate_cls,
    int *is_allocated);

int dnx_ingress_congestion_alloc_vsq_pg_tc_mapping_profile_create(
    int unit);

#endif
