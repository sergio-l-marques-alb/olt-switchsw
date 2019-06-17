/**
 * \file algo/egq/egq_alloc_mngr.h 
 *
 * Internal DNX EGQ Alloc Mngr APIs
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef EGQ_ALLOC_MNGR_H_INCLUDED
/* { */
#define EGQ_ALLOC_MNGR_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Includes
 * {
 */
#include <bcm/types.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <soc/dnxc/swstate/types/sw_state_res_mngr.h>
/*
 * }
 */

/*
 * Defines:
 * {
 */
/* COSQ Egress mapping */
#define DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_LOW_ID             (0)
#define DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_COUNT              (8)
#define DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_MAX_ENTITIES(unit) (dnx_data_port.general.nof_tm_ports_get(unit))
#define DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_SIZE               (sizeof(dnx_cosq_egress_queue_mapping_info_t))
/*
 * }
 */

/*
 * Enums:
 * {
 */

/*
 * }
 */

/*
 * Structs:
 * {
 */

/*
 * }
 */

/*
 * Function prototypes:
 * {
 */
/*
 * COSQ Egress queue mapping - Start
 * {
 */
int dnx_am_template_egress_queue_mapping_create(
    int unit,
    int template_init_id,
    dnx_cosq_egress_queue_mapping_info_t * mapping_profile);

int dnx_am_template_egress_queue_mapping_data_get(
    int unit,
    uint32 tm_port,
    int core,
    dnx_cosq_egress_queue_mapping_info_t * mapping_profile);
int dnx_am_template_egress_queue_mapping_exchange(
    int unit,
    uint32 tm_port,
    int core,
    const dnx_cosq_egress_queue_mapping_info_t * mapping_profile,
    int *old_profile,
    int *is_last,
    int *new_profile,
    int *is_allocated);
/*
 * \brief
 *   Print functions that needed for template manager.
 */
void dnx_algo_egr_tc_dp_mapping_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);
/*
 * COSQ Egress queue mapping - End
 * }
 */

/*
 * } 
 */
/* } */
#endif /* EGQ_ALLOC_MNGR_H_INCLUDED */
